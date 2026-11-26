// ==WindhawkMod==
// @id              macos-widgets
// @name            MacOS Widgets
// @description     macOS 27-style Liquid Glass desktop widgets, running in their own process (not inside Explorer)
// @version         1.0.0
// @author          Thiago Silva
// @github          https://github.com/brideinflame-rgb
// @homepage        https://github.com/brideinflame-rgb/Mac-OS-Widgets
// @donateUrl       https://livepix.gg/kurokobr
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -lwinhttp -lruntimeobject -lole32 -loleaut32 -luser32 -lgdi32 -lshell32 -lwindowscodecs -lsetupapi -liphlpapi -luuid -ladvapi32 -lversion -lpdh
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MacOS Widgets

macOS 27-style desktop widgets for Windows 11, with the same themes and the same Liquid Glass as Siri Plus Launchpad.

![MacOS Widgets: the themes](https://raw.githubusercontent.com/brideinflame-rgb/Mac-OS-Widgets/main/screenshots/preview.webp)

[Watch the full demo video](https://github.com/brideinflame-rgb/Mac-OS-Widgets/blob/main/screenshots/demo.mp4)

The widgets run in their own Windhawk process, not inside Explorer: a problem in them can never take the taskbar or the
desktop down.

## Widgets
In up to four sizes (small, medium, large and extra large, a portrait size as in macOS 27):
- **Clock**: time and date, with hands, digital or both (right-click the Clock).
- **Weather**: temperature now, the next hours and the next days (Open-Meteo, no API key). The moon at night. A click on
  it the first time asks for your city.
- **Music**: what's playing, with the cover, previous / play-pause / next. A click opens the player.
- **System**: CPU, GPU, memory and drive C:; the large size adds memory and disk in GB, processors and processes.
- **Calendar**: today, the month and the next events from iCal links or .ics files (right-click > Import Calendar).
- **World Clock**: up to 4 cities.
- **Sunrise**: sunrise and sunset in the Weather city.
- **Batteries**: the PC's battery and those of Bluetooth devices that report it to Windows.
- **Network**: download and upload right now, and the last minute.
- **Photos**: a photo from the OneDrive Pictures folder, changed every 20 minutes.
- **Notifications**: the notifications that reach Windows (WhatsApp, Instagram, Outlook...).
- **Notes**: write right in the widget (+ creates one, a click opens a note to edit; the first line is the title). They're
  saved by themselves as .txt files in Documents\MacOS Widgets\Notes.
- **Reminders**: + writes a new one (Enter starts the next), a click on the text edits it, the circle checks it off. They're
  kept one per line in Documents\MacOS Widgets\Reminders.txt.
- **Shortcuts**: apps (Store apps too), folders and sites one click away, chosen from a list.
- **Stocks**: stocks, indexes, currencies and crypto (Yahoo Finance's public data).
- **News**: headlines with their covers from any site or RSS feed, newest first. The mouse wheel goes through them.
- **Screen Time**: how long you spent in each app today, counted while the widget is on (Store apps that run in a frame
  window count together as "Application Frame Host").
- **Sensors**: CPU and GPU temperatures, usage, clock, memory, fan, power, disk and network, live; you choose what goes in
  each slot. CPU temperature and power need LibreHardwareMonitor running.
- **Home**: lights, plugs, scenes and sensors from Home Assistant (which brings together SmartThings, Matter and the device
  brands); a tap turns them on and off. Favorites are chosen from a list.

Clock, Weather, Music and System start on; the others are turned on by right-clicking any widget or in Settings > Widgets.

The notes and reminders folder (Documents\MacOS Widgets, or the one chosen in the settings) is only created when the first
note or reminder is saved. It's yours: it stays when the mod is disabled or removed.

## Themes (the ones of Siri Plus)
| | | |
|:-:|:-:|:-:|
| ![Glass](https://raw.githubusercontent.com/brideinflame-rgb/Mac-OS-Widgets/main/screenshots/theme-vidro.jpg) Glass | ![Liquid glass](https://raw.githubusercontent.com/brideinflame-rgb/Mac-OS-Widgets/main/screenshots/theme-vidro-liquido.jpg) Liquid glass | ![Custom glass](https://raw.githubusercontent.com/brideinflame-rgb/Mac-OS-Widgets/main/screenshots/theme-vidro-personalizado.jpg) Custom glass |
| ![Light](https://raw.githubusercontent.com/brideinflame-rgb/Mac-OS-Widgets/main/screenshots/theme-claro.jpg) Light | ![Siri gradient](https://raw.githubusercontent.com/brideinflame-rgb/Mac-OS-Widgets/main/screenshots/theme-degrade-siri.jpg) Siri gradient | ![Dark](https://raw.githubusercontent.com/brideinflame-rgb/Mac-OS-Widgets/main/screenshots/theme-escuro.jpg) Dark |
| ![OLED](https://raw.githubusercontent.com/brideinflame-rgb/Mac-OS-Widgets/main/screenshots/theme-oled.jpg) OLED | | |

Each theme keeps its own values (transparency, blur, refraction, effects and colors); "Restore default theme" brings the
chosen one back to how it comes.

## Settings
Right-click any widget > MacOS Widgets Settings…: a window in the style of Siri Plus, with sections on the side (one for
each widget that has settings), search and cards. Everything is also in Windhawk's Settings tab.

## Languages
Português, English, Español, Français, Deutsch, Italiano and 中文（简体）. "Automatic" follows the language of Windows;
another one can be chosen in Settings > Language. Dates and weekdays change with it.

## Backup
Settings > Backup and defaults: "Back up…" saves to a file all the settings (of every theme), the widgets that are on,
their sizes and places; "Restore backup…" brings everything back, on this PC or another.

## Live glass
The glass is made by the mod from a picture of what is behind the widget, as in Siri Plus. With "Live glass" on, the glass
follows what changes behind it, up to 20 times a second and only where something changed (Windows' Desktop Duplication),
and follows the widget while you drag it: videos and animated wallpapers show in the glass. Off, the glass uses the
wallpaper and the icons, and is lighter.

## Network access
Nothing is sent anywhere unless a widget that needs it is on. No API keys, no accounts, no telemetry; TLS certificate checks
are never turned off.

| Host | What is sent | When | Turned off by |
|---|---|---|---|
| api.open-meteo.com | The latitude and longitude of your city | Every 15 minutes, and on every settings change | Weather and Sunrise both off (and nothing is asked before a city is chosen) |
| geocoding-api.open-meteo.com | The city name you type | Only when you search for a city in the settings window | — |
| query1.finance.yahoo.com | The stock symbols, one request each | Every 5 minutes | Stocks widget off (off by default) |
| The news sites you add | Nothing beyond the request | Every 15 minutes. When an address is a page and not a feed, finding its feed takes up to 10 more requests: the feeds the page announces, links on the page that look like feeds (these can be on other hosts) and the usual places on the site (/feed, /rss, /rss.xml, /feed.xml, /index.xml, /atom.xml, /rss/&lt;site&gt;/). That happens the first time, again an hour after no feed was found, and again if the feed found goes 30 days without a story. A site that doesn't answer is left alone for 1, 2, 4… up to 60 minutes, and no other addresses are tried on it | News widget off (off by default; no sites until you add them) |
| The hosts the feeds name for their cover images | Nothing beyond the request | Up to 30 images after each news update | News widget off |
| Your calendar links | The link itself, which for a private iCal address includes its secret token | Every 15 minutes | Calendar widget off (off by default) |
| Your Home Assistant | Your access token (`Authorization: Bearer`) | Every 5 seconds (every minute while it doesn't answer), and on each tap | Home widget off (off by default) or no token |

The default Home Assistant address is `http://homeassistant.local:8123`: on plain http the token travels unencrypted over your
network. Use an `https://` address if your Home Assistant has one.

A click on a news story opens it in the browser; only `http://` and `https://` links are opened.

## Use
- Drag a widget: a glass outline shows the grid cell where it will go, and it snaps into it when released.
- Right-click: size, Edit Location, Remove Widget, show/hide each widget, Arrange Widgets (puts all of them in order, the
  biggest first, without leaving any out) and the Settings.
- The widgets stay right above the desktop: Win+D doesn't hide them.

---

## Support the project
If you enjoy MacOS Widgets, you can support its development with a Pix donation:

**[Donate with LivePix (Pix)](https://livepix.gg/kurokobr)**
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: "auto"
  $name: "Language"
  $name:pt-BR: "Idioma"
  $description: "Automatic follows the language of Windows. Everything here is also in the widgets’ own settings window (right-click any widget > MacOS Widgets Settings…)"
  $description:pt-BR: "Automático segue o idioma do Windows. Tudo aqui também está na janela de ajustes dos próprios widgets (botão direito em qualquer widget > Ajustes do MacOS Widgets…)"
  $options:
  - auto: "Automatic (Windows)"
  - pt: "Português"
  - en: "English"
  - es: "Español"
  - fr: "Français"
  - de: "Deutsch"
  - it: "Italiano"
  - zh: "中文（简体）"
  $options:pt-BR:
  - auto: "Automático (Windows)"
  - pt: "Português"
  - en: "English"
  - es: "Español"
  - fr: "Français"
  - de: "Deutsch"
  - it: "Italiano"
  - zh: "中文（简体）"
- showClock: true
  $name: "Widget: Clock"
  $name:pt-BR: "Widget: Relógio"
- showWeather: true
  $name: "Widget: Weather"
  $name:pt-BR: "Widget: Tempo"
- showMusic: true
  $name: "Widget: Music"
  $name:pt-BR: "Widget: Música"
- showSystem: true
  $name: "Widget: System"
  $name:pt-BR: "Widget: Sistema"
- showCalendar: false
  $name: "Widget: Calendar"
  $name:pt-BR: "Widget: Calendário"
- showWorld: false
  $name: "Widget: World Clock"
  $name:pt-BR: "Widget: Relógio Mundial"
- showSun: false
  $name: "Widget: Sunrise"
  $name:pt-BR: "Widget: Nascer do Sol"
- showBattery: false
  $name: "Widget: Batteries"
  $name:pt-BR: "Widget: Baterias"
- showNetwork: false
  $name: "Widget: Network"
  $name:pt-BR: "Widget: Rede"
- showPhotos: false
  $name: "Widget: Photos"
  $name:pt-BR: "Widget: Fotos"
- showNotif: false
  $name: "Widget: Notifications"
  $name:pt-BR: "Widget: Notificações"
- showNotes: false
  $name: "Widget: Notes"
  $name:pt-BR: "Widget: Notas"
- showReminders: false
  $name: "Widget: Reminders"
  $name:pt-BR: "Widget: Lembretes"
- showShortcuts: false
  $name: "Widget: Shortcuts"
  $name:pt-BR: "Widget: Atalhos"
- showStocks: false
  $name: "Widget: Stocks"
  $name:pt-BR: "Widget: Bolsa"
- showNews: false
  $name: "Widget: News"
  $name:pt-BR: "Widget: Notícias"
- showScreenTime: false
  $name: "Widget: Screen Time"
  $name:pt-BR: "Widget: Tempo de Uso"
- showSensors: false
  $name: "Widget: Sensors (temperatures and live data)"
  $name:pt-BR: "Widget: Sensores (temperaturas e dados ao vivo)"
- showHome: false
  $name: "Widget: Home (Home Assistant)"
  $name:pt-BR: "Widget: Casa (Home Assistant)"
- lockPosition: false
  $name: "Lock the widgets in place"
  $name:pt-BR: "Travar posição dos widgets"
- showGrid: false
  $name: "Show the grid while dragging"
  $name:pt-BR: "Mostrar a grade ao arrastar"
- liveGlass: true
  $name: "Live glass"
  $name:pt-BR: "Vidro ao vivo"
  $description: "Videos and animated wallpapers show in the glass; widgets don’t appear in screenshots. Off: the glass uses the wallpaper (lighter)"
  $description:pt-BR: "Vídeos e papéis de parede animados aparecem no vidro; os widgets não aparecem em prints. Desligado: o vidro usa o papel de parede (mais leve)"
- theme: "glass"
  $name: "Theme"
  $name:pt-BR: "Tema"
  $options:
  - frost: "Glass"
  - glass: "Liquid glass"
  - tintedliquid: "Custom glass"
  - light: "Light"
  - siri: "Siri gradient"
  - dark: "Dark"
  - oled: "OLED (deep black)"
  $options:pt-BR:
  - frost: "Vidro"
  - glass: "Vidro líquido"
  - tintedliquid: "Vidro personalizado"
  - light: "Claro"
  - siri: "Degradê Siri"
  - dark: "Escuro"
  - oled: "OLED (preto profundo)"
- transparency: 80
  $name: "Glass transparency (0-100)"
  $name:pt-BR: "Transparência do vidro (0-100)"
- blur: 60
  $name: "Background blur (0-100)"
  $name:pt-BR: "Desfoque do fundo (0-100)"
- refraction: 70
  $name: "Edge refraction (0-100)"
  $name:pt-BR: "Refração nas bordas (0-100)"
- glassEffect: true
  $name: "Glass effect"
  $name:pt-BR: "Efeito vidro"
  $description: "A shine at the top, the glass’ thickness at the edges and a beveled border"
  $description:pt-BR: "Brilho no topo, a espessura do vidro nas bordas e uma borda chanfrada"
- chromeBorder: false
  $name: "Animated chrome border"
  $name:pt-BR: "Borda cromada animada"
- backlight: false
  $name: "Backlight"
  $name:pt-BR: "Retroiluminação"
- backlightIntensity: 70
  $name: "Backlight intensity (5-100)"
  $name:pt-BR: "Intensidade da retroiluminação (5-100)"
- backlightColors: 3
  $name: "Backlight colors (1-5)"
  $name:pt-BR: "Cores da retroiluminação (1-5)"
- backlightColor1: "#2F6BFF"
  $name: "Backlight color 1"
  $name:pt-BR: "Cor 1 da retroiluminação"
- backlightColor2: "#B44BFF"
  $name: "Backlight color 2"
  $name:pt-BR: "Cor 2 da retroiluminação"
- backlightColor3: "#FF4B8B"
  $name: "Backlight color 3"
  $name:pt-BR: "Cor 3 da retroiluminação"
- backlightColor4: "#FFB020"
  $name: "Backlight color 4"
  $name:pt-BR: "Cor 4 da retroiluminação"
- backlightColor5: "#20D3A0"
  $name: "Backlight color 5"
  $name:pt-BR: "Cor 5 da retroiluminação"
- tintColor: "#050505"
  $name: "Custom glass - color"
  $name:pt-BR: "Vidro personalizado - cor"
- tintLevel: 100
  $name: "Custom glass - color strength at the top (0-100)"
  $name:pt-BR: "Vidro personalizado - força da cor no topo (0-100)"
- tintReach: 29
  $name: "Custom glass - how far down the color goes (0-100)"
  $name:pt-BR: "Vidro personalizado - até onde a cor desce (0-100)"
- cornerRadius: 26
  $name: "Corner roundness"
  $name:pt-BR: "Arredondamento dos cantos"
- textColor: "auto"
  $name: "Text color"
  $name:pt-BR: "Cor do texto"
  $options:
  - auto: "Automatic"
  - white: "White"
  - black: "Black"
  $options:pt-BR:
  - auto: "Automática"
  - white: "Branco"
  - black: "Preto"
- accentColor: "#0A84FF"
  $name: "Accent color (System bars)"
  $name:pt-BR: "Cor de destaque (barras do Sistema)"
- clockStyle: "both"
  $name: "Clock style"
  $name:pt-BR: "Estilo do relógio"
  $options:
  - both: "Hands and digital"
  - analog: "Hands"
  - digital: "Digital"
  $options:pt-BR:
  - both: "Ponteiros e digital"
  - analog: "Ponteiros"
  - digital: "Digital"
- city: ""
  $name: "Weather - city name"
  $name:pt-BR: "Tempo - nome da cidade"
  $description: "Easier in the widgets’ settings window: search the city by name"
  $description:pt-BR: "Mais fácil na janela de ajustes dos widgets: procure a cidade pelo nome"
- latitude: ""
  $name: "Weather - latitude"
  $name:pt-BR: "Tempo - latitude"
- longitude: ""
  $name: "Weather - longitude"
  $name:pt-BR: "Tempo - longitude"
- worldCity1: "Pacific Standard Time"
  $name: "World Clock - city 1 (Windows time zone)"
  $name:pt-BR: "Relógio Mundial - cidade 1 (fuso do Windows)"
- worldCity2: "Tokyo Standard Time"
  $name: "World Clock - city 2 (Windows time zone)"
  $name:pt-BR: "Relógio Mundial - cidade 2 (fuso do Windows)"
- worldCity3: "AUS Eastern Standard Time"
  $name: "World Clock - city 3 (Windows time zone)"
  $name:pt-BR: "Relógio Mundial - cidade 3 (fuso do Windows)"
- worldCity4: "Romance Standard Time"
  $name: "World Clock - city 4 (Windows time zone)"
  $name:pt-BR: "Relógio Mundial - cidade 4 (fuso do Windows)"
- calendarIcs: ""
  $name: "Calendar - iCal links or .ics files (separated by ;)"
  $name:pt-BR: "Calendário - links iCal ou arquivos .ics (separados por ;)"
  $description: "Google Calendar: Settings > your calendar > Secret address in iCal format. Outlook: Settings > Calendar > Shared calendars > Publish"
  $description:pt-BR: "Google Agenda: Configurações > seu calendário > Endereço secreto no formato iCal. Outlook: Configurações > Calendário > Calendários compartilhados > Publicar"
- notesFolder: ""
  $name: "Notes and reminders folder (empty = Documents\\MacOS Widgets)"
  $name:pt-BR: "Pasta das notas e lembretes (vazio = Documentos\\MacOS Widgets)"
- shortcuts: "File Explorer=explorer.exe; Settings=ms-settings:; Calculator=calc.exe; Notepad=notepad.exe"
  $name: "Shortcuts (Name=target; ...)"
  $name:pt-BR: "Atalhos (Nome=alvo; ...)"
  $description: "Easier in the widgets’ settings window: Choose shortcuts…, a list of your apps and folders"
  $description:pt-BR: "Mais fácil na janela de ajustes dos widgets: Escolher atalhos…, uma lista dos seus apps e pastas"
- stocks: "^GSPC, ^IXIC, ^DJI, BTC-USD"
  $name: "Stocks - Yahoo Finance symbols"
  $name:pt-BR: "Bolsa - códigos do Yahoo Finance"
- newsFeeds: ""
  $name: "News - sites or RSS feeds (separated by ;)"
  $name:pt-BR: "Notícias - sites ou feeds RSS (separados por ;)"
  $description: "Paste a site’s address and the widget finds its feed by itself; newest news first"
  $description:pt-BR: "Cole o endereço de um site e o widget acha o feed dele sozinho; as notícias mais novas primeiro"
- photosFolder: ""
  $name: "Photos folder (empty = OneDrive Pictures)"
  $name:pt-BR: "Pasta das fotos (vazio = Imagens do OneDrive)"
- sensor1: "cpuTemp"
  $name: "Sensors - slot 1"
  $name:pt-BR: "Sensores - espaço 1"
  $description: "Small and medium show slots 1-4; large, 1-6; extra large, all 8"
  $description:pt-BR: "Pequeno e médio mostram os espaços 1-4; grande, 1-6; extra-grande, os 8"
  $options:
  - cpuTemp: "CPU temperature"
  - gpuTemp: "GPU temperature"
  - gpuHotspot: "GPU hotspot"
  - cpuLoad: "CPU usage"
  - gpuLoad: "GPU usage"
  - ram: "RAM"
  - vram: "GPU memory"
  - cpuClock: "CPU clock"
  - gpuFan: "GPU fan"
  - gpuPower: "GPU power"
  - cpuPower: "CPU power"
  - disk: "Disk activity"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nothing"
  $options:pt-BR:
  - cpuTemp: "Temperatura da CPU"
  - gpuTemp: "Temperatura da GPU"
  - gpuHotspot: "Hotspot da GPU"
  - cpuLoad: "Uso da CPU"
  - gpuLoad: "Uso da GPU"
  - ram: "Memória RAM"
  - vram: "Memória da GPU"
  - cpuClock: "Clock da CPU"
  - gpuFan: "Ventoinha da GPU"
  - gpuPower: "Consumo da GPU"
  - cpuPower: "Consumo da CPU"
  - disk: "Atividade do disco"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nada"
- sensor2: "gpuTemp"
  $name: "Sensors - slot 2"
  $name:pt-BR: "Sensores - espaço 2"
  $options:
  - cpuTemp: "CPU temperature"
  - gpuTemp: "GPU temperature"
  - gpuHotspot: "GPU hotspot"
  - cpuLoad: "CPU usage"
  - gpuLoad: "GPU usage"
  - ram: "RAM"
  - vram: "GPU memory"
  - cpuClock: "CPU clock"
  - gpuFan: "GPU fan"
  - gpuPower: "GPU power"
  - cpuPower: "CPU power"
  - disk: "Disk activity"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nothing"
  $options:pt-BR:
  - cpuTemp: "Temperatura da CPU"
  - gpuTemp: "Temperatura da GPU"
  - gpuHotspot: "Hotspot da GPU"
  - cpuLoad: "Uso da CPU"
  - gpuLoad: "Uso da GPU"
  - ram: "Memória RAM"
  - vram: "Memória da GPU"
  - cpuClock: "Clock da CPU"
  - gpuFan: "Ventoinha da GPU"
  - gpuPower: "Consumo da GPU"
  - cpuPower: "Consumo da CPU"
  - disk: "Atividade do disco"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nada"
- sensor3: "cpuLoad"
  $name: "Sensors - slot 3"
  $name:pt-BR: "Sensores - espaço 3"
  $options:
  - cpuTemp: "CPU temperature"
  - gpuTemp: "GPU temperature"
  - gpuHotspot: "GPU hotspot"
  - cpuLoad: "CPU usage"
  - gpuLoad: "GPU usage"
  - ram: "RAM"
  - vram: "GPU memory"
  - cpuClock: "CPU clock"
  - gpuFan: "GPU fan"
  - gpuPower: "GPU power"
  - cpuPower: "CPU power"
  - disk: "Disk activity"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nothing"
  $options:pt-BR:
  - cpuTemp: "Temperatura da CPU"
  - gpuTemp: "Temperatura da GPU"
  - gpuHotspot: "Hotspot da GPU"
  - cpuLoad: "Uso da CPU"
  - gpuLoad: "Uso da GPU"
  - ram: "Memória RAM"
  - vram: "Memória da GPU"
  - cpuClock: "Clock da CPU"
  - gpuFan: "Ventoinha da GPU"
  - gpuPower: "Consumo da GPU"
  - cpuPower: "Consumo da CPU"
  - disk: "Atividade do disco"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nada"
- sensor4: "gpuLoad"
  $name: "Sensors - slot 4"
  $name:pt-BR: "Sensores - espaço 4"
  $options:
  - cpuTemp: "CPU temperature"
  - gpuTemp: "GPU temperature"
  - gpuHotspot: "GPU hotspot"
  - cpuLoad: "CPU usage"
  - gpuLoad: "GPU usage"
  - ram: "RAM"
  - vram: "GPU memory"
  - cpuClock: "CPU clock"
  - gpuFan: "GPU fan"
  - gpuPower: "GPU power"
  - cpuPower: "CPU power"
  - disk: "Disk activity"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nothing"
  $options:pt-BR:
  - cpuTemp: "Temperatura da CPU"
  - gpuTemp: "Temperatura da GPU"
  - gpuHotspot: "Hotspot da GPU"
  - cpuLoad: "Uso da CPU"
  - gpuLoad: "Uso da GPU"
  - ram: "Memória RAM"
  - vram: "Memória da GPU"
  - cpuClock: "Clock da CPU"
  - gpuFan: "Ventoinha da GPU"
  - gpuPower: "Consumo da GPU"
  - cpuPower: "Consumo da CPU"
  - disk: "Atividade do disco"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nada"
- sensor5: "ram"
  $name: "Sensors - slot 5"
  $name:pt-BR: "Sensores - espaço 5"
  $options:
  - cpuTemp: "CPU temperature"
  - gpuTemp: "GPU temperature"
  - gpuHotspot: "GPU hotspot"
  - cpuLoad: "CPU usage"
  - gpuLoad: "GPU usage"
  - ram: "RAM"
  - vram: "GPU memory"
  - cpuClock: "CPU clock"
  - gpuFan: "GPU fan"
  - gpuPower: "GPU power"
  - cpuPower: "CPU power"
  - disk: "Disk activity"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nothing"
  $options:pt-BR:
  - cpuTemp: "Temperatura da CPU"
  - gpuTemp: "Temperatura da GPU"
  - gpuHotspot: "Hotspot da GPU"
  - cpuLoad: "Uso da CPU"
  - gpuLoad: "Uso da GPU"
  - ram: "Memória RAM"
  - vram: "Memória da GPU"
  - cpuClock: "Clock da CPU"
  - gpuFan: "Ventoinha da GPU"
  - gpuPower: "Consumo da GPU"
  - cpuPower: "Consumo da CPU"
  - disk: "Atividade do disco"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nada"
- sensor6: "vram"
  $name: "Sensors - slot 6"
  $name:pt-BR: "Sensores - espaço 6"
  $options:
  - cpuTemp: "CPU temperature"
  - gpuTemp: "GPU temperature"
  - gpuHotspot: "GPU hotspot"
  - cpuLoad: "CPU usage"
  - gpuLoad: "GPU usage"
  - ram: "RAM"
  - vram: "GPU memory"
  - cpuClock: "CPU clock"
  - gpuFan: "GPU fan"
  - gpuPower: "GPU power"
  - cpuPower: "CPU power"
  - disk: "Disk activity"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nothing"
  $options:pt-BR:
  - cpuTemp: "Temperatura da CPU"
  - gpuTemp: "Temperatura da GPU"
  - gpuHotspot: "Hotspot da GPU"
  - cpuLoad: "Uso da CPU"
  - gpuLoad: "Uso da GPU"
  - ram: "Memória RAM"
  - vram: "Memória da GPU"
  - cpuClock: "Clock da CPU"
  - gpuFan: "Ventoinha da GPU"
  - gpuPower: "Consumo da GPU"
  - cpuPower: "Consumo da CPU"
  - disk: "Atividade do disco"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nada"
- sensor7: "gpuFan"
  $name: "Sensors - slot 7"
  $name:pt-BR: "Sensores - espaço 7"
  $options:
  - cpuTemp: "CPU temperature"
  - gpuTemp: "GPU temperature"
  - gpuHotspot: "GPU hotspot"
  - cpuLoad: "CPU usage"
  - gpuLoad: "GPU usage"
  - ram: "RAM"
  - vram: "GPU memory"
  - cpuClock: "CPU clock"
  - gpuFan: "GPU fan"
  - gpuPower: "GPU power"
  - cpuPower: "CPU power"
  - disk: "Disk activity"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nothing"
  $options:pt-BR:
  - cpuTemp: "Temperatura da CPU"
  - gpuTemp: "Temperatura da GPU"
  - gpuHotspot: "Hotspot da GPU"
  - cpuLoad: "Uso da CPU"
  - gpuLoad: "Uso da GPU"
  - ram: "Memória RAM"
  - vram: "Memória da GPU"
  - cpuClock: "Clock da CPU"
  - gpuFan: "Ventoinha da GPU"
  - gpuPower: "Consumo da GPU"
  - cpuPower: "Consumo da CPU"
  - disk: "Atividade do disco"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nada"
- sensor8: "cpuClock"
  $name: "Sensors - slot 8"
  $name:pt-BR: "Sensores - espaço 8"
  $options:
  - cpuTemp: "CPU temperature"
  - gpuTemp: "GPU temperature"
  - gpuHotspot: "GPU hotspot"
  - cpuLoad: "CPU usage"
  - gpuLoad: "GPU usage"
  - ram: "RAM"
  - vram: "GPU memory"
  - cpuClock: "CPU clock"
  - gpuFan: "GPU fan"
  - gpuPower: "GPU power"
  - cpuPower: "CPU power"
  - disk: "Disk activity"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nothing"
  $options:pt-BR:
  - cpuTemp: "Temperatura da CPU"
  - gpuTemp: "Temperatura da GPU"
  - gpuHotspot: "Hotspot da GPU"
  - cpuLoad: "Uso da CPU"
  - gpuLoad: "Uso da GPU"
  - ram: "Memória RAM"
  - vram: "Memória da GPU"
  - cpuClock: "Clock da CPU"
  - gpuFan: "Ventoinha da GPU"
  - gpuPower: "Consumo da GPU"
  - cpuPower: "Consumo da CPU"
  - disk: "Atividade do disco"
  - netDown: "Download"
  - netUp: "Upload"
  - none: "Nada"
- tempUnit: "c"
  $name: "Sensors - temperature unit"
  $name:pt-BR: "Sensores - unidade de temperatura"
  $description: "CPU temperature needs LibreHardwareMonitor running"
  $description:pt-BR: "A temperatura da CPU precisa do LibreHardwareMonitor aberto"
  $options:
  - c: "Celsius (°C)"
  - f: "Fahrenheit (°F)"
  $options:pt-BR:
  - c: "Celsius (°C)"
  - f: "Fahrenheit (°F)"
- haUrl: "http://homeassistant.local:8123"
  $name: "Home Assistant - address"
  $name:pt-BR: "Home Assistant - endereço"
- haToken: ""
  $name: "Home Assistant - long-lived access token"
  $name:pt-BR: "Home Assistant - token de acesso de longa duração"
  $description: "Home Assistant: your profile > Security > Long-lived access tokens"
  $description:pt-BR: "Home Assistant: seu perfil > Segurança > Tokens de acesso de longa duração"
- haEntities: ""
  $name: "Home Assistant - entity IDs (empty = automatic)"
  $name:pt-BR: "Home Assistant - IDs das entidades (vazio = automático)"
  $description: "Easier in the widgets’ settings window: Choose favorites…"
  $description:pt-BR: "Mais fácil na janela de ajustes dos widgets: Escolher favoritos…"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <winhttp.h>
#include <psapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <wincodec.h>
#include <setupapi.h>
#include <devpropdef.h>
#include <iphlpapi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <wbemidl.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <cmath>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <array>
#include <cstdarg>
#include <deque>
#include <unordered_map>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#ifndef WW_NO_WINRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#endif

#ifndef WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
#define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY 4
#endif
#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

using Microsoft::WRL::ComPtr;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

enum class Theme { Frost, Glass, Tinted, Light, Siri, Dark, Oled };

struct Settings {
    Theme theme = Theme::Glass;
    int transparency = 80, blur = 60, refraction = 70;
    bool glassEffect = true, chromeBorder = false, backlight = false;
    int backlightIntensity = 70, backlightCount = 3;
    COLORREF backlightColors[5] = {RGB(0x2F, 0x6B, 0xFF), RGB(0xB4, 0x4B, 0xFF), RGB(0xFF, 0x4B, 0x8B),
                                   RGB(0xFF, 0xB0, 0x20), RGB(0x20, 0xD3, 0xA0)};
    COLORREF tintColor = RGB(5, 5, 5);
    int tintLevel = 100, tintReach = 29;
    int cornerRadius = 26;
    int textColor = -1;  // -1 automatic, 0 white, 1 black
    COLORREF accent = RGB(10, 132, 255);
    bool liveGlass = true;
    bool showClock = true, showWeather = true, showMusic = true, showSystem = true;
    int clockStyle = 0;  // 0 both (as the Mac), 1 hands only, 2 digital only
    std::wstring city;
    std::wstring latitude, longitude;  // empty until a city is chosen
    bool lockPosition = false;
    bool showGrid = false;
    bool showCalendar = false, showWorld = false, showSun = false, showBattery = false, showNetwork = false,
         showPhotos = false, showNotif = false;
    std::wstring worldCity[4];
    std::wstring photosFolder;
    bool showNotes = false, showReminders = false, showShortcuts = false, showStocks = false, showNews = false,
         showScreenTime = false;
    std::wstring calendarIcs, stocks, newsFeeds, notesFolder, shortcuts;
    bool showHome = false;
    std::wstring haUrl, haToken, haEntities;
    bool showSensors = false, fahrenheit = false;
    int sensorSlots[8] = {};
};

static std::mutex g_dataMutex;  // guards g_settings, the stored values, g_weather, g_media, g_commands
static Settings g_settings;

static COLORREF ParseHex(const std::wstring& s, COLORREF def) {
    std::wstring h = s;
    if (!h.empty() && h[0] == L'#') h = h.substr(1);
    if (h.size() != 6) return def;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(h.c_str(), &end, 16);
    if (!end || *end) return def;
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

// Keeps only characters valid in a coordinate, so the value is safe in a URL.
static std::wstring SanitizeCoord(const std::wstring& s, PCWSTR def) {
    std::wstring r;
    for (wchar_t c : s) {
        if ((c >= L'0' && c <= L'9') || c == L'.' || c == L'-') r += c;
        else if (c == L',') r += L'.';
    }
    return r.empty() ? std::wstring(def) : r;
}

static bool HasCity(const Settings& s) { return !s.latitude.empty() && !s.longitude.empty(); }

// ---------------------------------------------------------------------------
// The settings table: Windhawk's Settings tab and the mod's own settings window
// ---------------------------------------------------------------------------

enum CfgType { kCfgToggle, kCfgSlider, kCfgChoice, kCfgColor, kCfgText, kCfgButton, kCfgGroup };
struct CfgOpt {
    PCWSTR value, label;
};
struct CfgItem {
    int section;
    CfgType type;
    PCWSTR key;    // Windhawk setting name ("#..." = only in the window)
    PCWSTR label;
    PCWSTR desc;   // may be nullptr
    PCWSTR def;    // the Windhawk default, as text
    bool themed;   // each theme keeps its own value
    int min, max;
    PCWSTR unit;
    const CfgOpt* opts;
    int optCount;
};
struct CfgSection {
    PCWSTR name, desc, glyph;
    UINT32 color;
};

// ---------------------------------------------------------------------------
// Languages: the same seven as Siri Plus Launchpad and the Finder Bar. Every text of the mod is written in Portuguese and
// looked up in kTr when drawn (Text, TextWrap, CText and their measures), so what you write, read or name (notes, news,
// apps) stays as it is. "Automático" follows the language of Windows; a missing translation falls back to English
// ---------------------------------------------------------------------------

enum Lang { LANG_PT, LANG_EN, LANG_ES, LANG_FR, LANG_DE, LANG_IT, LANG_ZH, LANG_COUNT };
static std::atomic<int> g_lang{LANG_PT};
static std::atomic<bool> g_langAuto{true};

static int WindowsLang() {
    LANGID id = GetUserDefaultUILanguage();
    switch (PRIMARYLANGID(id)) {
        case LANG_PORTUGUESE: return LANG_PT;
        case LANG_SPANISH: return LANG_ES;
        case LANG_FRENCH: return LANG_FR;
        case LANG_GERMAN: return LANG_DE;
        case LANG_ITALIAN: return LANG_IT;
        case LANG_CHINESE:
            return SUBLANGID(id) == SUBLANG_CHINESE_SIMPLIFIED || SUBLANGID(id) == SUBLANG_CHINESE_SINGAPORE ? LANG_ZH : LANG_EN;
        default: return LANG_EN;
    }
}

struct TrRow {
    PCWSTR v[LANG_COUNT];  // pt, en, es, fr, de, it, zh
};
static const TrRow kTr[] = {
    {{L"previsão do tempo", L"weather forecast", L"pronóstico del tiempo", L"météo", L"Wettervorhersage", L"previsioni meteo", L"天气预报"}},
    {{L"nascer e pôr do sol", L"sunrise and sunset", L"amanecer y atardecer", L"lever et coucher du soleil", L"Sonnenaufgang und Sonnenuntergang", L"alba e tramonto", L"日出日落"}},
    {{L"Clique para escolher sua cidade", L"Click to choose your city", L"Haz clic para elegir tu ciudad", L"Cliquez pour choisir votre ville", L"Klicke, um deine Stadt zu wählen", L"Fai clic per scegliere la tua città", L"点击选择你的城市"}},
    {{L"Clique para escolher os sites de notícias", L"Click to choose your news sites", L"Haz clic para elegir tus sitios de noticias", L"Cliquez pour choisir vos sites d'actualités", L"Klicke, um deine Nachrichtenseiten zu wählen", L"Fai clic per scegliere i siti di notizie", L"点击选择新闻网站"}},
    {{L"Clique para escolher os códigos", L"Click to choose the symbols", L"Haz clic para elegir los símbolos", L"Cliquez pour choisir les symboles", L"Klicke, um die Symbole zu wählen", L"Fai clic per scegliere i simboli", L"点击选择代码"}},
    {{L"Calculadora", L"Calculator", L"Calculadora", L"Calculatrice", L"Rechner", L"Calcolatrice", L"计算器"}},
    {{L"Bloco de Notas", L"Notepad", L"Bloc de notas", L"Bloc-notes", L"Editor", L"Blocco note", L"记事本"}},
    {{L"Sua cidade", L"Your city", L"Tu ciudad", L"Votre ville", L"Deine Stadt", L"La tua città", L"你的城市"}},
    {{L"Widgets", L"Widgets", L"Widgets", L"Widgets", L"Widgets", L"Widget", L"小组件"}},
    {{L"Quais widgets aparecem na área de trabalho e como eles se comportam", L"Which widgets appear on the desktop and how they behave", L"Qué widgets aparecen en el escritorio y cómo se comportan", L"Quels widgets apparaissent sur le bureau et comment ils se comportent", L"Welche Widgets auf dem Desktop erscheinen und wie sie sich verhalten", L"Quali widget compaiono sul desktop e come si comportano", L"桌面上显示哪些小组件以及它们的行为"}},
    {{L"Aparência", L"Appearance", L"Apariencia", L"Apparence", L"Darstellung", L"Aspetto", L"外观"}},
    {{L"O tema e, logo abaixo, tudo o que é dele: cada tema guarda o seu; o que ele não usa fica escondido", L"The theme and, right below it, everything that belongs to it: each theme keeps its own; what it doesn't use stays hidden", L"El tema y, justo debajo, todo lo suyo: cada tema guarda lo suyo; lo que no usa queda oculto", L"Le thème et, juste en dessous, tout ce qui lui appartient : chaque thème garde les siens ; ce qu'il n'utilise pas reste caché", L"Das Design und direkt darunter alles, was dazugehört: Jedes Design behält seine eigenen Werte; was es nicht nutzt, bleibt verborgen", L"Il tema e, subito sotto, tutto ciò che gli appartiene: ogni tema conserva i suoi; ciò che non usa resta nascosto", L"主题及其下方所属的所有设置：每个主题各自保存；不使用的设置会隐藏"}},
    {{L"Idioma", L"Language", L"Idioma", L"Langue", L"Sprache", L"Lingua", L"语言"}},
    {{L"O idioma dos widgets, dos menus e desta janela", L"The language of the widgets, the menus and this window", L"El idioma de los widgets, los menús y esta ventana", L"La langue des widgets, des menus et de cette fenêtre", L"Die Sprache der Widgets, der Menüs und dieses Fensters", L"La lingua dei widget, dei menu e di questa finestra", L"小组件、菜单和此窗口的语言"}},
    {{L"Relógio", L"Clock", L"Reloj", L"Horloge", L"Uhr", L"Orologio", L"时钟"}},
    {{L"Como o widget Relógio mostra a hora", L"How the Clock widget shows the time", L"Cómo muestra la hora el widget Reloj", L"Comment le widget Horloge affiche l'heure", L"Wie das Uhr-Widget die Zeit anzeigt", L"Come il widget Orologio mostra l'ora", L"时钟小组件如何显示时间"}},
    {{L"Tempo", L"Weather", L"Tiempo", L"Météo", L"Wetter", L"Meteo", L"天气"}},
    {{L"A cidade do widget Tempo", L"The city of the Weather widget", L"La ciudad del widget Tiempo", L"La ville du widget Météo", L"Die Stadt des Wetter-Widgets", L"La città del widget Meteo", L"天气小组件的城市"}},
    {{L"Relógio Mundial", L"World Clock", L"Reloj mundial", L"Horloge mondiale", L"Weltuhr", L"Orologio mondiale", L"世界时钟"}},
    {{L"As cidades do widget Relógio Mundial", L"The cities of the World Clock widget", L"Las ciudades del widget Reloj mundial", L"Les villes du widget Horloge mondiale", L"Die Städte des Weltuhr-Widgets", L"Le città del widget Orologio mondiale", L"世界时钟小组件的城市"}},
    {{L"Calendário", L"Calendar", L"Calendario", L"Calendrier", L"Kalender", L"Calendario", L"日历"}},
    {{L"De onde vêm os eventos: links iCal (Google, Outlook…) ou arquivos .ics", L"Where the events come from: iCal links (Google, Outlook…) or .ics files", L"De dónde vienen los eventos: enlaces iCal (Google, Outlook…) o archivos .ics", L"D'où viennent les événements : liens iCal (Google, Outlook…) ou fichiers .ics", L"Woher die Termine kommen: iCal-Links (Google, Outlook…) oder .ics-Dateien", L"Da dove arrivano gli eventi: link iCal (Google, Outlook…) o file .ics", L"日程来源：iCal 链接（Google、Outlook…）或 .ics 文件"}},
    {{L"Notas e Lembretes", L"Notes and Reminders", L"Notas y recordatorios", L"Notes et rappels", L"Notizen & Erinnerungen", L"Note e promemoria", L"备忘录和提醒事项"}},
    {{L"Onde as notas e os lembretes são guardados", L"Where notes and reminders are kept", L"Dónde se guardan las notas y los recordatorios", L"Où sont gardés les notes et les rappels", L"Wo Notizen und Erinnerungen gespeichert werden", L"Dove vengono salvati note e promemoria", L"备忘录和提醒事项的保存位置"}},
    {{L"Atalhos", L"Shortcuts", L"Atajos", L"Raccourcis", L"Kurzbefehle", L"Scorciatoie", L"快捷方式"}},
    {{L"Os apps, pastas e sites do widget Atalhos", L"The apps, folders and sites of the Shortcuts widget", L"Las apps, carpetas y sitios del widget Atajos", L"Les apps, dossiers et sites du widget Raccourcis", L"Die Apps, Ordner und Websites des Kurzbefehle-Widgets", L"App, cartelle e siti del widget Scorciatoie", L"快捷方式小组件中的应用、文件夹和网站"}},
    {{L"Bolsa", L"Stocks", L"Bolsa", L"Bourse", L"Aktien", L"Borsa", L"股市"}},
    {{L"As ações, moedas e índices do widget Bolsa", L"The stocks, currencies and indexes of the Stocks widget", L"Las acciones, monedas e índices del widget Bolsa", L"Les actions, devises et indices du widget Bourse", L"Die Aktien, Währungen und Indizes des Aktien-Widgets", L"Azioni, valute e indici del widget Borsa", L"股市小组件中的股票、货币和指数"}},
    {{L"Notícias", L"News", L"Noticias", L"Actualités", L"Nachrichten", L"Notizie", L"新闻"}},
    {{L"De onde vêm as notícias", L"Where the news comes from", L"De dónde vienen las noticias", L"D'où viennent les actualités", L"Woher die Nachrichten kommen", L"Da dove arrivano le notizie", L"新闻来源"}},
    {{L"Sensores", L"Sensors", L"Sensores", L"Capteurs", L"Sensoren", L"Sensori", L"传感器"}},
    {{L"O que cada espaço do widget Sensores mostra", L"What each slot of the Sensors widget shows", L"Qué muestra cada espacio del widget Sensores", L"Ce que montre chaque emplacement du widget Capteurs", L"Was jeder Platz des Sensoren-Widgets zeigt", L"Cosa mostra ogni spazio del widget Sensori", L"传感器小组件每个位置显示的内容"}},
    {{L"Fotos", L"Photos", L"Fotos", L"Photos", L"Fotos", L"Foto", L"照片"}},
    {{L"A pasta das fotos do widget Fotos", L"The folder of the Photos widget", L"La carpeta del widget Fotos", L"Le dossier du widget Photos", L"Der Ordner des Fotos-Widgets", L"La cartella del widget Foto", L"照片小组件的文件夹"}},
    {{L"Casa", L"Home", L"Casa", L"Maison", L"Zuhause", L"Casa", L"家庭"}},
    {{L"Suas luzes, tomadas, cenas e sensores pelo Home Assistant (que reúne SmartThings, Matter e as marcas dos aparelhos)", L"Your lights, plugs, scenes and sensors through Home Assistant (which brings together SmartThings, Matter and the device brands)", L"Tus luces, enchufes, escenas y sensores a través de Home Assistant (que reúne SmartThings, Matter y las marcas de los aparatos)", L"Vos lumières, prises, scènes et capteurs via Home Assistant (qui réunit SmartThings, Matter et les marques des appareils)", L"Deine Lampen, Steckdosen, Szenen und Sensoren über Home Assistant (das SmartThings, Matter und die Gerätemarken vereint)", L"Luci, prese, scene e sensori tramite Home Assistant (che riunisce SmartThings, Matter e le marche dei dispositivi)", L"通过 Home Assistant 控制你的灯、插座、场景和传感器（它整合了 SmartThings、Matter 和各品牌设备）"}},
    {{L"Backup e padrões", L"Backup and defaults", L"Copia y restablecer", L"Sauvegarde", L"Sicherung", L"Backup e ripristino", L"备份和默认设置"}},
    {{L"Guardar seus ajustes num arquivo, trazê-los de volta ou recomeçar do zero", L"Keep your settings in a file, bring them back or start over", L"Guarda tus ajustes en un archivo, recupéralos o empieza de cero", L"Gardez vos réglages dans un fichier, récupérez-les ou repartez de zéro", L"Deine Einstellungen in einer Datei sichern, zurückholen oder neu anfangen", L"Salva le impostazioni in un file, ripristinale o ricomincia da zero", L"将设置保存到文件、恢复或从头开始"}},
    {{L"Vidro", L"Glass", L"Vidrio", L"Verre", L"Glas", L"Vetro", L"玻璃"}},
    {{L"Vidro líquido", L"Liquid glass", L"Vidrio líquido", L"Verre liquide", L"Flüssiges Glas", L"Vetro liquido", L"液态玻璃"}},
    {{L"Vidro personalizado", L"Custom glass", L"Vidrio personalizado", L"Verre personnalisé", L"Eigenes Glas", L"Vetro personalizzato", L"自定义玻璃"}},
    {{L"Claro", L"Light", L"Claro", L"Clair", L"Hell", L"Chiaro", L"浅色"}},
    {{L"Degradê Siri", L"Siri gradient", L"Degradado Siri", L"Dégradé Siri", L"Siri-Verlauf", L"Sfumatura Siri", L"Siri 渐变"}},
    {{L"Escuro", L"Dark", L"Oscuro", L"Sombre", L"Dunkel", L"Scuro", L"深色"}},
    {{L"OLED (preto profundo)", L"OLED (deep black)", L"OLED (negro profundo)", L"OLED (noir profond)", L"OLED (tiefes Schwarz)", L"OLED (nero profondo)", L"OLED（纯黑）"}},
    {{L"Nova York", L"New York", L"Nueva York", L"New York", L"New York", L"New York", L"纽约"}},
    {{L"Cidade do México", L"Mexico City", L"Ciudad de México", L"Mexico", L"Mexiko-Stadt", L"Città del Messico", L"墨西哥城"}},
    {{L"Londres", L"London", L"Londres", L"Londres", L"London", L"Londra", L"伦敦"}},
    {{L"Berlim", L"Berlin", L"Berlín", L"Berlin", L"Berlin", L"Berlino", L"柏林"}},
    {{L"Joanesburgo", L"Johannesburg", L"Johannesburgo", L"Johannesburg", L"Johannesburg", L"Johannesburg", L"约翰内斯堡"}},
    {{L"Moscou", L"Moscow", L"Moscú", L"Moscou", L"Moskau", L"Mosca", L"莫斯科"}},
    {{L"Nova Délhi", L"New Delhi", L"Nueva Delhi", L"New Delhi", L"Neu-Delhi", L"Nuova Delhi", L"新德里"}},
    {{L"Singapura", L"Singapore", L"Singapur", L"Singapour", L"Singapur", L"Singapore", L"新加坡"}},
    {{L"Pequim", L"Beijing", L"Pekín", L"Pékin", L"Peking", L"Pechino", L"北京"}},
    {{L"Seul", L"Seoul", L"Seúl", L"Séoul", L"Seoul", L"Seul", L"首尔"}},
    {{L"Tóquio", L"Tokyo", L"Tokio", L"Tokyo", L"Tokio", L"Tokyo", L"东京"}},
    {{L"Automática", L"Automatic", L"Automático", L"Automatique", L"Automatisch", L"Automatico", L"自动"}},
    {{L"Branco", L"White", L"Blanco", L"Blanc", L"Weiß", L"Bianco", L"白色"}},
    {{L"Preto", L"Black", L"Negro", L"Noir", L"Schwarz", L"Nero", L"黑色"}},
    {{L"Nada", L"Nothing", L"Nada", L"Rien", L"Nichts", L"Niente", L"无"}},
    {{L"Temperatura da CPU", L"CPU temperature", L"Temperatura de la CPU", L"Température du processeur", L"CPU-Temperatur", L"Temperatura CPU", L"CPU 温度"}},
    {{L"Temperatura da GPU", L"GPU temperature", L"Temperatura de la GPU", L"Température du GPU", L"GPU-Temperatur", L"Temperatura GPU", L"GPU 温度"}},
    {{L"Hotspot da GPU", L"GPU hotspot", L"Punto caliente de la GPU", L"Point chaud du GPU", L"GPU-Hotspot", L"Hotspot GPU", L"GPU 热点"}},
    {{L"Uso da CPU", L"CPU usage", L"Uso de la CPU", L"Utilisation du processeur", L"CPU-Auslastung", L"Uso CPU", L"CPU 使用率"}},
    {{L"Uso da GPU", L"GPU usage", L"Uso de la GPU", L"Utilisation du GPU", L"GPU-Auslastung", L"Uso GPU", L"GPU 使用率"}},
    {{L"Memória RAM", L"RAM", L"Memoria RAM", L"Mémoire vive", L"Arbeitsspeicher", L"Memoria RAM", L"内存"}},
    {{L"Memória da GPU", L"GPU memory", L"Memoria de la GPU", L"Mémoire du GPU", L"GPU-Speicher", L"Memoria GPU", L"显存"}},
    {{L"Clock da CPU", L"CPU clock", L"Frecuencia de la CPU", L"Fréquence du processeur", L"CPU-Takt", L"Frequenza CPU", L"CPU 频率"}},
    {{L"Ventoinha da GPU", L"GPU fan", L"Ventilador de la GPU", L"Ventilateur du GPU", L"GPU-Lüfter", L"Ventola GPU", L"GPU 风扇"}},
    {{L"Consumo da GPU", L"GPU power", L"Consumo de la GPU", L"Consommation du GPU", L"GPU-Leistung", L"Consumo GPU", L"GPU 功耗"}},
    {{L"Consumo da CPU", L"CPU power", L"Consumo de la CPU", L"Consommation du processeur", L"CPU-Leistung", L"Consumo CPU", L"CPU 功耗"}},
    {{L"Atividade do disco", L"Disk activity", L"Actividad del disco", L"Activité du disque", L"Datenträgeraktivität", L"Attività del disco", L"磁盘活动"}},
    {{L"Automático (o do Windows)", L"Automatic (Windows')", L"Automático (el de Windows)", L"Automatique (celle de Windows)", L"Automatisch (wie Windows)", L"Automatico (quella di Windows)", L"自动（跟随 Windows）"}},
    {{L"Ponteiros e digital", L"Hands and digital", L"Agujas y digital", L"Aiguilles et numérique", L"Zeiger und digital", L"Lancette e digitale", L"指针和数字"}},
    {{L"Ponteiros", L"Hands", L"Agujas", L"Aiguilles", L"Zeiger", L"Lancette", L"指针"}},
    {{L"Digital", L"Digital", L"Digital", L"Numérique", L"Digital", L"Digitale", L"数字"}},
    {{L"Celsius (°C)", L"Celsius (°C)", L"Celsius (°C)", L"Celsius (°C)", L"Celsius (°C)", L"Celsius (°C)", L"摄氏度 (°C)"}},
    {{L"Fahrenheit (°F)", L"Fahrenheit (°F)", L"Fahrenheit (°F)", L"Fahrenheit (°F)", L"Fahrenheit (°F)", L"Fahrenheit (°F)", L"华氏度 (°F)"}},
    {{L"Na área de trabalho", L"On the desktop", L"En el escritorio", L"Sur le bureau", L"Auf dem Desktop", L"Sul desktop", L"桌面上"}},
    {{L"Dia da semana, hora e data", L"Day of the week, time and date", L"Día de la semana, hora y fecha", L"Jour de la semaine, heure et date", L"Wochentag, Uhrzeit und Datum", L"Giorno della settimana, ora e data", L"星期、时间和日期"}},
    {{L"Estilo", L"Style", L"Estilo", L"Style", L"Stil", L"Stile", L"样式"}},
    {{L"Estilo do relógio", L"Clock style", L"Estilo del reloj", L"Style de l'horloge", L"Uhrstil", L"Stile dell'orologio", L"时钟样式"}},
    {{L"Também no botão direito do Relógio", L"Also in the Clock's right-click menu", L"También en el clic derecho del Reloj", L"Aussi dans le clic droit de l'Horloge", L"Auch im Rechtsklickmenü der Uhr", L"Anche nel clic destro dell'Orologio", L"也可在时钟的右键菜单中更改"}},
    {{L"Temperatura, condição, máxima e mínima", L"Temperature, conditions, high and low", L"Temperatura, condición, máxima y mínima", L"Température, conditions, max. et min.", L"Temperatur, Wetterlage, Höchst- und Tiefstwert", L"Temperatura, condizioni, massima e minima", L"温度、天气状况、最高和最低气温"}},
    {{L"Música", L"Music", L"Música", L"Musique", L"Musik", L"Musica", L"音乐"}},
    {{L"O que está tocando, com anterior, tocar/pausar e próxima", L"What's playing, with previous, play/pause and next", L"Lo que suena, con anterior, reproducir/pausar y siguiente", L"Ce qui joue, avec précédent, lecture/pause et suivant", L"Was gerade läuft, mit Zurück, Wiedergabe/Pause und Weiter", L"Ciò che è in riproduzione, con precedente, play/pausa e successivo", L"正在播放的内容，带上一首、播放/暂停和下一首"}},
    {{L"Sistema", L"System", L"Sistema", L"Système", L"System", L"Sistema", L"系统"}},
    {{L"CPU, GPU, memória, disco C: e tempo ligado", L"CPU, GPU, memory, drive C: and uptime", L"CPU, GPU, memoria, disco C: y tiempo encendido", L"Processeur, GPU, mémoire, disque C: et durée d'activité", L"CPU, GPU, Speicher, Laufwerk C: und Betriebszeit", L"CPU, GPU, memoria, disco C: e tempo di attività", L"CPU、GPU、内存、C 盘和运行时间"}},
    {{L"O dia de hoje e o mês", L"Today and the month", L"El día de hoy y el mes", L"Aujourd'hui et le mois", L"Heute und der Monat", L"Il giorno di oggi e il mese", L"今天和本月"}},
    {{L"A hora em outras cidades (escolha em Ajustes > Relógio Mundial)", L"The time in other cities (choose them in Settings > World Clock)", L"La hora en otras ciudades (elígelas en Ajustes > Reloj mundial)", L"L'heure dans d'autres villes (à choisir dans Réglages > Horloge mondiale)", L"Die Uhrzeit in anderen Städten (wähle sie unter Einstellungen > Weltuhr)", L"L'ora in altre città (scegline in Impostazioni > Orologio mondiale)", L"其他城市的时间（在设置 > 世界时钟中选择）"}},
    {{L"Nascer do Sol", L"Sunrise", L"Amanecer", L"Lever du soleil", L"Sonnenaufgang", L"Alba", L"日出"}},
    {{L"Nascer e pôr do sol na cidade do Tempo", L"Sunrise and sunset in the Weather city", L"Amanecer y atardecer en la ciudad del Tiempo", L"Lever et coucher du soleil dans la ville de la Météo", L"Sonnenauf- und -untergang in der Wetter-Stadt", L"Alba e tramonto nella città del Meteo", L"天气城市的日出和日落"}},
    {{L"Baterias", L"Batteries", L"Baterías", L"Batteries", L"Batterien", L"Batterie", L"电池"}},
    {{L"A bateria do PC e a dos aparelhos Bluetooth que a informam ao Windows", L"The PC's battery and those of Bluetooth devices that report it to Windows", L"La batería del PC y la de los aparatos Bluetooth que la informan a Windows", L"La batterie du PC et celle des appareils Bluetooth qui la communiquent à Windows", L"Der PC-Akku und die Akkus von Bluetooth-Geräten, die ihn an Windows melden", L"La batteria del PC e quella dei dispositivi Bluetooth che la comunicano a Windows", L"电脑电池以及向 Windows 报告电量的蓝牙设备电池"}},
    {{L"Rede", L"Network", L"Red", L"Réseau", L"Netzwerk", L"Rete", L"网络"}},
    {{L"Velocidade de download e upload agora", L"Download and upload speed right now", L"Velocidad de descarga y subida ahora", L"Débit de téléchargement et d'envoi en ce moment", L"Aktuelle Download- und Upload-Geschwindigkeit", L"Velocità di download e upload ora", L"当前下载和上传速度"}},
    {{L"Uma foto da sua pasta Imagens do OneDrive, trocada a cada 20 minutos", L"A photo from your OneDrive Pictures folder, changed every 20 minutes", L"Una foto de tu carpeta Imágenes de OneDrive, cambiada cada 20 minutos", L"Une photo de votre dossier Images OneDrive, changée toutes les 20 minutes", L"Ein Foto aus deinem OneDrive-Bilderordner, alle 20 Minuten ein neues", L"Una foto dalla cartella Immagini di OneDrive, cambiata ogni 20 minuti", L"来自 OneDrive 图片文件夹的一张照片，每 20 分钟更换一次"}},
    {{L"Notificações", L"Notifications", L"Notificaciones", L"Notifications", L"Mitteilungen", L"Notifiche", L"通知"}},
    {{L"As notificações que chegam no Windows (WhatsApp, Instagram, Outlook...), lidas do banco de notificações do Windows", L"The notifications that reach Windows (WhatsApp, Instagram, Outlook...), read from Windows' notification database", L"Las notificaciones que llegan a Windows (WhatsApp, Instagram, Outlook...), leídas de la base de notificaciones de Windows", L"Les notifications reçues par Windows (WhatsApp, Instagram, Outlook...), lues dans la base de notifications de Windows", L"Die Mitteilungen, die in Windows ankommen (WhatsApp, Instagram, Outlook...), aus der Mitteilungsdatenbank von Windows gelesen", L"Le notifiche che arrivano su Windows (WhatsApp, Instagram, Outlook...), lette dal database delle notifiche di Windows", L"Windows 收到的通知（WhatsApp、Instagram、Outlook…），从 Windows 通知数据库读取"}},
    {{L"Notas", L"Notes", L"Notas", L"Notes", L"Notizen", L"Note", L"备忘录"}},
    {{L"Suas notas em texto (Documentos\\MacOS Widgets\\Notes)", L"Your text notes (Documents\\MacOS Widgets\\Notes)", L"Tus notas de texto (Documentos\\MacOS Widgets\\Notes)", L"Vos notes texte (Documents\\MacOS Widgets\\Notes)", L"Deine Textnotizen (Dokumente\\MacOS Widgets\\Notes)", L"Le tue note di testo (Documenti\\MacOS Widgets\\Notes)", L"你的文本备忘录（文档\\MacOS Widgets\\Notes）"}},
    {{L"Lembretes", L"Reminders", L"Recordatorios", L"Rappels", L"Erinnerungen", L"Promemoria", L"提醒事项"}},
    {{L"Uma lista de tarefas para marcar como feitas", L"A to-do list to check off", L"Una lista de tareas para marcar como hechas", L"Une liste de tâches à cocher", L"Eine Aufgabenliste zum Abhaken", L"Un elenco di cose da fare da spuntare", L"可勾选完成的待办清单"}},
    {{L"Apps, pastas e sites a um clique", L"Apps, folders and sites one click away", L"Apps, carpetas y sitios a un clic", L"Apps, dossiers et sites en un clic", L"Apps, Ordner und Websites mit einem Klick", L"App, cartelle e siti a portata di clic", L"一键打开应用、文件夹和网站"}},
    {{L"Ações, índices, moedas e cripto", L"Stocks, indexes, currencies and crypto", L"Acciones, índices, monedas y cripto", L"Actions, indices, devises et crypto", L"Aktien, Indizes, Währungen und Krypto", L"Azioni, indici, valute e cripto", L"股票、指数、货币和加密货币"}},
    {{L"As manchetes dos sites que você escolher (RSS)", L"Headlines from the sites you choose (RSS)", L"Los titulares de los sitios que elijas (RSS)", L"Les titres des sites de votre choix (RSS)", L"Schlagzeilen der Websites deiner Wahl (RSS)", L"I titoli dei siti che scegli (RSS)", L"你所选网站的头条（RSS）"}},
    {{L"Tempo de Uso", L"Screen Time", L"Tiempo de uso", L"Temps d'écran", L"Bildschirmzeit", L"Tempo di utilizzo", L"屏幕使用时间"}},
    {{L"Quanto tempo você passou em cada app hoje", L"How long you spent in each app today", L"Cuánto tiempo pasaste en cada app hoy", L"Le temps passé dans chaque app aujourd'hui", L"Wie lange du heute jede App genutzt hast", L"Quanto tempo hai passato in ogni app oggi", L"今天在每个应用中花费的时间"}},
    {{L"Luzes, tomadas, cenas e sensores do Home Assistant (ligue em Casa)", L"Lights, plugs, scenes and sensors from Home Assistant (set it up in Home)", L"Luces, enchufes, escenas y sensores de Home Assistant (configúralo en Casa)", L"Lumières, prises, scènes et capteurs de Home Assistant (à configurer dans Maison)", L"Lampen, Steckdosen, Szenen und Sensoren aus Home Assistant (einrichten unter Zuhause)", L"Luci, prese, scene e sensori di Home Assistant (configuralo in Casa)", L"Home Assistant 的灯、插座、场景和传感器（在“家庭”中设置）"}},
    {{L"Temperaturas da CPU e da GPU, uso, clock, ventoinha e consumo, ao vivo", L"CPU and GPU temperatures, usage, clock, fan and power, live", L"Temperaturas de CPU y GPU, uso, frecuencia, ventilador y consumo, en vivo", L"Températures du processeur et du GPU, utilisation, fréquence, ventilateur et consommation, en direct", L"CPU- und GPU-Temperaturen, Auslastung, Takt, Lüfter und Leistung, live", L"Temperature di CPU e GPU, uso, frequenza, ventola e consumo, in tempo reale", L"实时显示 CPU 和 GPU 温度、使用率、频率、风扇和功耗"}},
    {{L"Comportamento", L"Behavior", L"Comportamiento", L"Comportement", L"Verhalten", L"Comportamento", L"行为"}},
    {{L"Travar posição", L"Lock position", L"Bloquear posición", L"Verrouiller la position", L"Position sperren", L"Blocca posizione", L"锁定位置"}},
    {{L"Os widgets deixam de ser arrastados", L"Widgets can no longer be dragged", L"Los widgets dejan de poder arrastrarse", L"Les widgets ne peuvent plus être déplacés", L"Widgets lassen sich nicht mehr ziehen", L"I widget non si possono più trascinare", L"小组件将无法拖动"}},
    {{L"Mostrar a grade ao arrastar", L"Show the grid while dragging", L"Mostrar la cuadrícula al arrastrar", L"Afficher la grille pendant le glissement", L"Raster beim Ziehen anzeigen", L"Mostra la griglia durante il trascinamento", L"拖动时显示网格"}},
    {{L"Ao arrastar um widget, todas as casas aparecem de leve; a casa onde ele vai ficar aparece sempre", L"While dragging a widget, all cells show faintly; the cell where it will go always shows", L"Al arrastrar un widget, todas las celdas se ven tenues; la celda donde quedará se ve siempre", L"Pendant le glissement d'un widget, toutes les cases apparaissent légèrement ; celle où il ira apparaît toujours", L"Beim Ziehen eines Widgets erscheinen alle Felder leicht; das Feld, in dem es landet, immer", L"Trascinando un widget, tutte le caselle appaiono tenui; quella in cui andrà appare sempre", L"拖动小组件时会淡淡显示所有格子；其目标格子始终显示"}},
    {{L"Vidro ao vivo", L"Live glass", L"Vidrio en vivo", L"Verre en direct", L"Live-Glas", L"Vetro dal vivo", L"实时玻璃"}},
    {{L"Ligado: vídeos e papéis de parede animados aparecem no vidro, mas usa mais o PC e os widgets somem de prints. Desligado: o vidro mostra o papel de parede e os ícones, mais leve", L"On: videos and animated wallpapers show in the glass, but it uses more of the PC and widgets vanish from screenshots. Off: the glass shows the wallpaper and the icons, lighter", L"Activado: los videos y fondos animados aparecen en el vidrio, pero usa más el PC y los widgets desaparecen de las capturas. Desactivado: el vidrio muestra el fondo y los iconos, más ligero", L"Activé : les vidéos et fonds d'écran animés apparaissent dans le verre, mais cela sollicite davantage le PC et les widgets disparaissent des captures. Désactivé : le verre montre le fond d'écran et les icônes, plus léger", L"An: Videos und animierte Hintergründe erscheinen im Glas, aber es belastet den PC mehr und Widgets fehlen auf Screenshots. Aus: Das Glas zeigt Hintergrund und Symbole, ressourcenschonender", L"Attivo: video e sfondi animati appaiono nel vetro, ma usa di più il PC e i widget spariscono dagli screenshot. Disattivo: il vetro mostra lo sfondo e le icone, più leggero", L"开启：玻璃中会显示视频和动态壁纸，但更占用电脑资源，且小组件不会出现在截图中。关闭：玻璃显示壁纸和图标，更轻量"}},
    {{L"Cor de destaque", L"Accent color", L"Color de realce", L"Couleur d'accentuation", L"Akzentfarbe", L"Colore principale", L"强调色"}},
    {{L"As barras do widget Sistema", L"The bars of the System widget", L"Las barras del widget Sistema", L"Les barres du widget Système", L"Die Balken des System-Widgets", L"Le barre del widget Sistema", L"系统小组件中的进度条"}},
    {{L"Tema", L"Theme", L"Tema", L"Thème", L"Design", L"Tema", L"主题"}},
    {{L"As cores e o vidro de tudo: widgets e esta janela", L"The colors and the glass of everything: widgets and this window", L"Los colores y el vidrio de todo: widgets y esta ventana", L"Les couleurs et le verre de tout : widgets et cette fenêtre", L"Farben und Glas von allem: Widgets und dieses Fenster", L"I colori e il vetro di tutto: widget e questa finestra", L"所有内容的颜色和玻璃效果：小组件和此窗口"}},
    {{L"Personalizar tema", L"Customize theme", L"Personalizar tema", L"Personnaliser le thème", L"Design anpassen", L"Personalizza tema", L"自定义主题"}},
    {{L"Restaurar tema padrão", L"Restore default theme", L"Restaurar tema predeterminado", L"Rétablir le thème par défaut", L"Standarddesign wiederherstellen", L"Ripristina tema predefinito", L"恢复默认主题"}},
    {{L"Volta este tema a como ele vem. Os outros temas não mudam", L"Brings this theme back to how it comes. The other themes don't change", L"Devuelve este tema a como viene. Los demás temas no cambian", L"Remet ce thème comme à l'origine. Les autres thèmes ne changent pas", L"Setzt dieses Design auf den Auslieferungszustand zurück. Andere Designs bleiben unverändert", L"Riporta questo tema com'era all'inizio. Gli altri temi non cambiano", L"将此主题恢复为初始状态，其他主题不受影响"}},
    {{L"Visual", L"Look", L"Aspecto", L"Aspect", L"Aussehen", L"Aspetto", L"外观"}},
    {{L"Transparência do vidro", L"Glass transparency", L"Transparencia del vidrio", L"Transparence du verre", L"Glastransparenz", L"Trasparenza del vetro", L"玻璃透明度"}},
    {{L"Quanto do que está atrás aparece. 100 = totalmente transparente", L"How much of what's behind shows through. 100 = fully transparent", L"Cuánto se ve de lo que hay detrás. 100 = totalmente transparente", L"Ce qu'on voit de l'arrière-plan. 100 = entièrement transparent", L"Wie viel vom Hintergrund durchscheint. 100 = völlig transparent", L"Quanto si vede di ciò che c'è dietro. 100 = completamente trasparente", L"背后内容的显示程度。100 = 完全透明"}},
    {{L"Desfoque do fundo", L"Background blur", L"Desenfoque del fondo", L"Flou de l'arrière-plan", L"Hintergrundunschärfe", L"Sfocatura dello sfondo", L"背景模糊"}},
    {{L"Quão embaçado fica o que está atrás. 0 = nenhum", L"How blurred what's behind looks. 0 = none", L"Qué tan difuminado se ve lo de atrás. 0 = nada", L"À quel point l'arrière-plan est flou. 0 = aucun", L"Wie unscharf der Hintergrund wirkt. 0 = gar nicht", L"Quanto è sfocato ciò che c'è dietro. 0 = nessuna", L"背后内容的模糊程度。0 = 不模糊"}},
    {{L"Arredondamento dos cantos", L"Corner roundness", L"Redondeo de las esquinas", L"Arrondi des coins", L"Eckenrundung", L"Arrotondamento degli angoli", L"圆角大小"}},
    {{L"Cor do texto", L"Text color", L"Color del texto", L"Couleur du texte", L"Textfarbe", L"Colore del testo", L"文字颜色"}},
    {{L"Automática: no vidro transparente, preto sobre um fundo claro e branco sobre um escuro", L"Automatic: on transparent glass, black over a light background and white over a dark one", L"Automático: en vidrio transparente, negro sobre fondo claro y blanco sobre uno oscuro", L"Automatique : sur verre transparent, noir sur fond clair et blanc sur fond sombre", L"Automatisch: auf transparentem Glas Schwarz auf hellem und Weiß auf dunklem Hintergrund", L"Automatico: sul vetro trasparente, nero su sfondo chiaro e bianco su sfondo scuro", L"自动：透明玻璃上，浅色背景用黑色，深色背景用白色"}},
    {{L"Refração nas bordas", L"Edge refraction", L"Refracción en los bordes", L"Réfraction sur les bords", L"Lichtbrechung an den Rändern", L"Rifrazione sui bordi", L"边缘折射"}},
    {{L"Quanto o vidro curva o que está atrás dele nas bordas. 0 = sem curvatura", L"How much the glass bends what's behind it at the edges. 0 = no bending", L"Cuánto curva el vidrio lo de atrás en los bordes. 0 = sin curvatura", L"À quel point le verre courbe l'arrière-plan sur les bords. 0 = aucune courbure", L"Wie stark das Glas den Hintergrund an den Rändern krümmt. 0 = keine Krümmung", L"Quanto il vetro curva ciò che c'è dietro ai bordi. 0 = nessuna curvatura", L"玻璃边缘对背后内容的弯曲程度。0 = 不弯曲"}},
    {{L"Efeito vidro", L"Glass effect", L"Efecto vidrio", L"Effet verre", L"Glaseffekt", L"Effetto vetro", L"玻璃效果"}},
    {{L"Brilho no topo, a espessura do vidro nas bordas e uma borda chanfrada", L"A shine at the top, the glass' thickness at the edges and a beveled border", L"Brillo arriba, el grosor del vidrio en los bordes y un borde biselado", L"Un reflet en haut, l'épaisseur du verre sur les bords et une bordure biseautée", L"Glanz oben, die Glasdicke an den Rändern und eine abgeschrägte Kante", L"Un riflesso in alto, lo spessore del vetro ai bordi e un bordo smussato", L"顶部高光、边缘玻璃厚度和斜面边框"}},
    {{L"Borda e retroiluminação", L"Border and backlight", L"Borde y retroiluminación", L"Bordure et rétroéclairage", L"Rand und Hintergrundbeleuchtung", L"Bordo e retroilluminazione", L"边框和背光"}},
    {{L"Borda cromada animada", L"Animated chrome border", L"Borde cromado animado", L"Bordure chromée animée", L"Animierter Chromrand", L"Bordo cromato animato", L"动态镀铬边框"}},
    {{L"Uma borda polida, com a luz correndo em volta", L"A polished border with light running around it", L"Un borde pulido con la luz recorriéndolo", L"Une bordure polie parcourue par la lumière", L"Ein polierter Rand, um den Licht läuft", L"Un bordo lucido con la luce che vi scorre intorno", L"光线环绕流动的抛光边框"}},
    {{L"Retroiluminação", L"Backlight", L"Retroiluminación", L"Rétroéclairage", L"Hintergrundbeleuchtung", L"Retroilluminazione", L"背光"}},
    {{L"Um brilho colorido em volta, com as cores girando", L"A colored glow around it, with the colors turning", L"Un brillo de color alrededor, con los colores girando", L"Une lueur colorée autour, aux couleurs tournantes", L"Ein farbiges Leuchten ringsum mit kreisenden Farben", L"Un bagliore colorato intorno, con i colori che ruotano", L"周围的彩色光晕，颜色循环流转"}},
    {{L"Intensidade da retroiluminação", L"Backlight intensity", L"Intensidad de la retroiluminación", L"Intensité du rétroéclairage", L"Stärke der Hintergrundbeleuchtung", L"Intensità della retroilluminazione", L"背光强度"}},
    {{L"Cores da retroiluminação", L"Backlight colors", L"Colores de la retroiluminación", L"Couleurs du rétroéclairage", L"Farben der Hintergrundbeleuchtung", L"Colori della retroilluminazione", L"背光颜色"}},
    {{L"Quantas cores giram em volta (1 = uma cor com a luz correndo)", L"How many colors turn around it (1 = one color with the light running)", L"Cuántos colores giran alrededor (1 = un color con la luz recorriéndolo)", L"Combien de couleurs tournent autour (1 = une couleur avec la lumière qui court)", L"Wie viele Farben ringsum kreisen (1 = eine Farbe mit laufendem Licht)", L"Quanti colori ruotano intorno (1 = un colore con la luce che scorre)", L"环绕的颜色数量（1 = 单色流光）"}},
    {{L"Cor 1", L"Color 1", L"Color 1", L"Couleur 1", L"Farbe 1", L"Colore 1", L"颜色 1"}},
    {{L"Cor 2", L"Color 2", L"Color 2", L"Couleur 2", L"Farbe 2", L"Colore 2", L"颜色 2"}},
    {{L"Cor 3", L"Color 3", L"Color 3", L"Couleur 3", L"Farbe 3", L"Colore 3", L"颜色 3"}},
    {{L"Cor 4", L"Color 4", L"Color 4", L"Couleur 4", L"Farbe 4", L"Colore 4", L"颜色 4"}},
    {{L"Cor 5", L"Color 5", L"Color 5", L"Couleur 5", L"Farbe 5", L"Colore 5", L"颜色 5"}},
    {{L"Força da cor no topo", L"Color strength at the top", L"Intensidad del color arriba", L"Intensité de la couleur en haut", L"Farbstärke oben", L"Intensità del colore in alto", L"顶部颜色浓度"}},
    {{L"100 = sólido no topo; menos deixa o fundo aparecer também na parte escura", L"100 = solid at the top; less lets the background show in the dark part too", L"100 = sólido arriba; menos deja ver el fondo también en la parte oscura", L"100 = opaque en haut ; moins laisse aussi voir le fond dans la partie sombre", L"100 = oben deckend; weniger lässt den Hintergrund auch im dunklen Teil durchscheinen", L"100 = pieno in alto; meno lascia vedere lo sfondo anche nella parte scura", L"100 = 顶部完全不透明；数值越小，深色部分也越透出背景"}},
    {{L"Até onde a cor desce", L"How far down the color goes", L"Hasta dónde baja el color", L"Jusqu'où descend la couleur", L"Wie weit die Farbe nach unten reicht", L"Fin dove scende il colore", L"颜色向下延伸的范围"}},
    {{L"Onde a cor dá lugar ao vidro transparente", L"Where the color gives way to clear glass", L"Dónde el color da paso al vidrio transparente", L"Où la couleur laisse place au verre transparent", L"Wo die Farbe in klares Glas übergeht", L"Dove il colore lascia il posto al vetro trasparente", L"颜色过渡为透明玻璃的位置"}},
    {{L"Cor do vidro", L"Glass color", L"Color del vidrio", L"Couleur du verre", L"Glasfarbe", L"Colore del vetro", L"玻璃颜色"}},
    {{L"Começa preto; qualquer cor funciona. Cores claras ganham texto escuro", L"Starts black; any color works. Light colors get dark text", L"Empieza en negro; cualquier color sirve. Los colores claros llevan texto oscuro", L"Commence en noir ; toute couleur convient. Les couleurs claires ont un texte sombre", L"Beginnt schwarz; jede Farbe geht. Helle Farben bekommen dunklen Text", L"Parte nero; va bene qualsiasi colore. I colori chiari hanno il testo scuro", L"默认为黑色；任何颜色都可以。浅色会使用深色文字"}},
    {{L"Procurar cidade", L"Find a city", L"Buscar ciudad", L"Rechercher une ville", L"Stadt suchen", L"Cerca città", L"查找城市"}},
    {{L"Pesquisar", L"Search", L"Buscar", L"Rechercher", L"Suchen", L"Cerca", L"搜索"}},
    {{L"Digite o nome da cidade e tecle Enter; depois clique em Usar", L"Type the city's name and press Enter; then click Use", L"Escribe el nombre de la ciudad y pulsa Intro; luego haz clic en Usar", L"Tapez le nom de la ville et appuyez sur Entrée ; puis cliquez sur Utiliser", L"Gib den Namen der Stadt ein und drücke die Eingabetaste; dann klicke auf Verwenden", L"Digita il nome della città e premi Invio; poi fai clic su Usa", L"输入城市名称并按 Enter，然后点击“使用”"}},
    {{L"Local em uso", L"Location in use", L"Ubicación en uso", L"Lieu utilisé", L"Verwendeter Ort", L"Posizione in uso", L"当前位置"}},
    {{L"Nome da cidade", L"City name", L"Nombre de la ciudad", L"Nom de la ville", L"Name der Stadt", L"Nome della città", L"城市名称"}},
    {{L"O nome que aparece no widget", L"The name shown in the widget", L"El nombre que aparece en el widget", L"Le nom affiché dans le widget", L"Der Name, der im Widget erscheint", L"Il nome che appare nel widget", L"小组件中显示的名称"}},
    {{L"Por exemplo -23.55", L"For example -23.55", L"Por ejemplo -23.55", L"Par exemple -23.55", L"Zum Beispiel -23.55", L"Per esempio -23.55", L"例如 -23.55"}},
    {{L"Por exemplo -46.63", L"For example -46.63", L"Por ejemplo -46.63", L"Par exemple -46.63", L"Zum Beispiel -46.63", L"Per esempio -46.63", L"例如 -46.63"}},
    {{L"Cidades", L"Cities", L"Ciudades", L"Villes", L"Städte", L"Città", L"城市"}},
    {{L"Cidade 1", L"City 1", L"Ciudad 1", L"Ville 1", L"Stadt 1", L"Città 1", L"城市 1"}},
    {{L"A que aparece no tamanho pequeno", L"The one shown in the small size", L"La que aparece en el tamaño pequeño", L"Celle affichée en petite taille", L"Die in der kleinen Größe angezeigte", L"Quella mostrata nella dimensione piccola", L"小尺寸时显示的城市"}},
    {{L"Cidade 2", L"City 2", L"Ciudad 2", L"Ville 2", L"Stadt 2", L"Città 2", L"城市 2"}},
    {{L"Cidade 3", L"City 3", L"Ciudad 3", L"Ville 3", L"Stadt 3", L"Città 3", L"城市 3"}},
    {{L"Cidade 4", L"City 4", L"Ciudad 4", L"Ville 4", L"Stadt 4", L"Città 4", L"城市 4"}},
    {{L"Calendários", L"Calendars", L"Calendarios", L"Calendriers", L"Kalender", L"Calendari", L"日历"}},
    {{L"Calendários (links iCal ou arquivos .ics)", L"Calendars (iCal links or .ics files)", L"Calendarios (enlaces iCal o archivos .ics)", L"Calendriers (liens iCal ou fichiers .ics)", L"Kalender (iCal-Links oder .ics-Dateien)", L"Calendari (link iCal o file .ics)", L"日历（iCal 链接或 .ics 文件）"}},
    {{L"Vários, separados por ponto e vírgula. Google Agenda: Configurações > seu calendário > Endereço secreto no formato iCal. Outlook: Configurações > Calendário > Calendários compartilhados > Publicar (link ICS)", L"Several, separated by semicolons. Google Calendar: Settings > your calendar > Secret address in iCal format. Outlook: Settings > Calendar > Shared calendars > Publish (ICS link)", L"Varios, separados por punto y coma. Google Calendar: Configuración > tu calendario > Dirección secreta en formato iCal. Outlook: Configuración > Calendario > Calendarios compartidos > Publicar (enlace ICS)", L"Plusieurs, séparés par des points-virgules. Google Agenda : Paramètres > votre agenda > Adresse secrète au format iCal. Outlook : Paramètres > Calendrier > Calendriers partagés > Publier (lien ICS)", L"Mehrere, durch Semikolon getrennt. Google Kalender: Einstellungen > dein Kalender > Privatadresse im iCal-Format. Outlook: Einstellungen > Kalender > Freigegebene Kalender > Veröffentlichen (ICS-Link)", L"Più di uno, separati da punto e virgola. Google Calendar: Impostazioni > il tuo calendario > Indirizzo segreto in formato iCal. Outlook: Impostazioni > Calendario > Calendari condivisi > Pubblica (link ICS)", L"可填写多个，用分号分隔。Google 日历：设置 > 你的日历 > iCal 格式的私密地址。Outlook：设置 > 日历 > 共享日历 > 发布（ICS 链接）"}},
    {{L"Importar arquivo .ics…", L"Import .ics file…", L"Importar archivo .ics…", L"Importer un fichier .ics…", L".ics-Datei importieren…", L"Importa file .ics…", L"导入 .ics 文件…"}},
    {{L"Um arquivo exportado de qualquer agenda. O widget relê o arquivo sozinho, então trocar o arquivo atualiza os eventos", L"A file exported from any calendar. The widget rereads the file by itself, so replacing the file updates the events", L"Un archivo exportado de cualquier agenda. El widget vuelve a leer el archivo solo, así que reemplazarlo actualiza los eventos", L"Un fichier exporté de n'importe quel agenda. Le widget relit le fichier tout seul : le remplacer met à jour les événements", L"Eine aus einem beliebigen Kalender exportierte Datei. Das Widget liest sie selbst neu ein, daher aktualisiert das Ersetzen der Datei die Termine", L"Un file esportato da qualsiasi calendario. Il widget rilegge il file da solo, quindi sostituirlo aggiorna gli eventi", L"从任意日历导出的文件。小组件会自动重新读取，替换文件即可更新日程"}},
    {{L"Pasta", L"Folder", L"Carpeta", L"Dossier", L"Ordner", L"Cartella", L"文件夹"}},
    {{L"Pasta das notas e lembretes", L"Notes and reminders folder", L"Carpeta de notas y recordatorios", L"Dossier des notes et rappels", L"Ordner für Notizen und Erinnerungen", L"Cartella di note e promemoria", L"备忘录和提醒事项文件夹"}},
    {{L"Vazio: Documentos\\MacOS Widgets. Notas são arquivos .txt ou .md na subpasta Notes; os lembretes ficam em Reminders.txt", L"Empty: Documents\\MacOS Widgets. Notes are .txt or .md files in the Notes subfolder; reminders go in Reminders.txt", L"Vacío: Documentos\\MacOS Widgets. Las notas son archivos .txt o .md en la subcarpeta Notes; los recordatorios van en Reminders.txt", L"Vide : Documents\\MacOS Widgets. Les notes sont des fichiers .txt ou .md dans le sous-dossier Notes ; les rappels vont dans Reminders.txt", L"Leer: Dokumente\\MacOS Widgets. Notizen sind .txt- oder .md-Dateien im Unterordner Notes; Erinnerungen stehen in Reminders.txt", L"Vuoto: Documenti\\MacOS Widgets. Le note sono file .txt o .md nella sottocartella Notes; i promemoria vanno in Reminders.txt", L"留空：文档\\MacOS Widgets。备忘录是 Notes 子文件夹中的 .txt 或 .md 文件；提醒事项保存在 Reminders.txt"}},
    {{L"No widget", L"In the widget", L"En el widget", L"Dans le widget", L"Im Widget", L"Nel widget", L"小组件中"}},
    {{L"Nome=programa, pasta ou site, separados por ponto e vírgula", L"Name=program, folder or site, separated by semicolons", L"Nombre=programa, carpeta o sitio, separados por punto y coma", L"Nom=programme, dossier ou site, séparés par des points-virgules", L"Name=Programm, Ordner oder Website, durch Semikolon getrennt", L"Nome=programma, cartella o sito, separati da punto e virgola", L"名称=程序、文件夹或网站，用分号分隔"}},
    {{L"Escolher atalhos…", L"Choose shortcuts…", L"Elegir atajos…", L"Choisir les raccourcis…", L"Kurzbefehle auswählen…", L"Scegli scorciatoie…", L"选择快捷方式…"}},
    {{L"Uma lista com seus apps (da Loja também) e pastas, com pesquisa", L"A list of your apps (Store ones too) and folders, with search", L"Una lista con tus apps (también de la Tienda) y carpetas, con búsqueda", L"Une liste de vos apps (y compris du Store) et dossiers, avec recherche", L"Eine Liste deiner Apps (auch aus dem Store) und Ordner, mit Suche", L"Un elenco delle tue app (anche dello Store) e cartelle, con ricerca", L"你的应用（包括应用商店应用）和文件夹列表，可搜索"}},
    {{L"Códigos", L"Symbols", L"Códigos", L"Symboles", L"Symbole", L"Simboli", L"代码"}},
    {{L"Códigos da Bolsa", L"Stock symbols", L"Códigos de bolsa", L"Symboles boursiers", L"Börsensymbole", L"Simboli di borsa", L"股票代码"}},
    {{L"Os do Yahoo Finance, separados por vírgula: PETR4.SA, ^BVSP (Ibovespa), USDBRL=X (dólar), BTC-USD", L"Yahoo Finance's, separated by commas: PETR4.SA, ^BVSP (Ibovespa), USDBRL=X (dollar), BTC-USD", L"Los de Yahoo Finance, separados por comas: PETR4.SA, ^BVSP (Ibovespa), USDBRL=X (dólar), BTC-USD", L"Ceux de Yahoo Finance, séparés par des virgules : PETR4.SA, ^BVSP (Ibovespa), USDBRL=X (dollar), BTC-USD", L"Die von Yahoo Finance, durch Komma getrennt: PETR4.SA, ^BVSP (Ibovespa), USDBRL=X (Dollar), BTC-USD", L"Quelli di Yahoo Finance, separati da virgole: PETR4.SA, ^BVSP (Ibovespa), USDBRL=X (dollaro), BTC-USD", L"Yahoo Finance 代码，用逗号分隔：PETR4.SA、^BVSP（Ibovespa）、USDBRL=X（美元）、BTC-USD"}},
    {{L"Feeds", L"Feeds", L"Fuentes", L"Flux", L"Feeds", L"Feed", L"订阅源"}},
    {{L"Endereços RSS das notícias", L"News RSS addresses", L"Direcciones RSS de las noticias", L"Adresses RSS des actualités", L"RSS-Adressen der Nachrichten", L"Indirizzi RSS delle notizie", L"新闻 RSS 地址"}},
    {{L"Cole o endereço do site que você quer (ex.: https://www.bbc.com/portuguese): o widget acha o feed dele sozinho. Se não achar, procure no site por \"RSS\" e cole esse endereço. Vários: separe com ponto e vírgula. As notícias de todos se juntam, as mais novas primeiro. Do g1, cada seção tem o seu: g1.globo.com/rss/g1/ + tecnologia/, esporte/, sp/...", L"Paste the address of the site you want (e.g. https://www.bbc.com/news): the widget finds its feed by itself. If it doesn't, look for \"RSS\" on the site and paste that address. Several: separate them with semicolons. News from all of them come together, newest first. On g1, each section has its own: g1.globo.com/rss/g1/ + tecnologia/, esporte/, sp/...", L"Pega la dirección del sitio que quieras (ej.: https://www.bbc.com/mundo): el widget encuentra su feed solo. Si no lo encuentra, busca \"RSS\" en el sitio y pega esa dirección. Varios: sepáralos con punto y coma. Las noticias de todos se juntan, las más nuevas primero. En g1, cada sección tiene el suyo: g1.globo.com/rss/g1/ + tecnologia/, esporte/, sp/...", L"Collez l'adresse du site voulu (ex. : https://www.lemonde.fr) : le widget trouve son flux tout seul. Sinon, cherchez « RSS » sur le site et collez cette adresse. Plusieurs : séparez-les par des points-virgules. Les actualités de tous se mélangent, les plus récentes d'abord. Sur g1, chaque rubrique a le sien : g1.globo.com/rss/g1/ + tecnologia/, esporte/, sp/...", L"Füge die Adresse der gewünschten Website ein (z. B. https://www.tagesschau.de): Das Widget findet ihren Feed selbst. Falls nicht, suche auf der Website nach \"RSS\" und füge diese Adresse ein. Mehrere: mit Semikolon trennen. Die Nachrichten aller werden gemischt, die neuesten zuerst. Bei g1 hat jede Rubrik ihren eigenen: g1.globo.com/rss/g1/ + tecnologia/, esporte/, sp/...", L"Incolla l'indirizzo del sito che vuoi (es.: https://www.ansa.it): il widget trova il suo feed da solo. Se non lo trova, cerca \"RSS\" sul sito e incolla quell'indirizzo. Più siti: separali con punto e virgola. Le notizie di tutti si uniscono, le più recenti prima. Su g1, ogni sezione ha il suo: g1.globo.com/rss/g1/ + tecnologia/, esporte/, sp/...", L"粘贴你想要的网站地址（例如 https://www.bbc.com/zhongwen/simp）：小组件会自动找到它的订阅源。如果找不到，请在网站上查找“RSS”并粘贴该地址。多个地址用分号分隔。所有来源的新闻会合并显示，最新的在前。g1 的每个栏目各有一个：g1.globo.com/rss/g1/ + tecnologia/、esporte/、sp/..."}},
    {{L"O que mostrar", L"What to show", L"Qué mostrar", L"Quoi afficher", L"Was angezeigt wird", L"Cosa mostrare", L"显示内容"}},
    {{L"Espaço 1", L"Slot 1", L"Espacio 1", L"Emplacement 1", L"Platz 1", L"Spazio 1", L"位置 1"}},
    {{L"Pequeno e médio mostram os 4 primeiros; grande, 6; extra-grande, os 8", L"Small and medium show the first 4; large, 6; extra large, all 8", L"Pequeño y mediano muestran los 4 primeros; grande, 6; extragrande, los 8", L"Petit et moyen affichent les 4 premiers ; grand, 6 ; très grand, les 8", L"Klein und mittel zeigen die ersten 4; groß 6; extragroß alle 8", L"Piccolo e medio mostrano i primi 4; grande, 6; extra grande, tutti gli 8", L"小号和中号显示前 4 个；大号显示 6 个；超大号显示全部 8 个"}},
    {{L"Espaço 2", L"Slot 2", L"Espacio 2", L"Emplacement 2", L"Platz 2", L"Spazio 2", L"位置 2"}},
    {{L"Espaço 3", L"Slot 3", L"Espacio 3", L"Emplacement 3", L"Platz 3", L"Spazio 3", L"位置 3"}},
    {{L"Espaço 4", L"Slot 4", L"Espacio 4", L"Emplacement 4", L"Platz 4", L"Spazio 4", L"位置 4"}},
    {{L"Espaço 5", L"Slot 5", L"Espacio 5", L"Emplacement 5", L"Platz 5", L"Spazio 5", L"位置 5"}},
    {{L"Só no grande e no extra-grande", L"Large and extra large only", L"Solo en grande y extragrande", L"Grand et très grand uniquement", L"Nur bei groß und extragroß", L"Solo grande ed extra grande", L"仅大号和超大号"}},
    {{L"Espaço 6", L"Slot 6", L"Espacio 6", L"Emplacement 6", L"Platz 6", L"Spazio 6", L"位置 6"}},
    {{L"Espaço 7", L"Slot 7", L"Espacio 7", L"Emplacement 7", L"Platz 7", L"Spazio 7", L"位置 7"}},
    {{L"Só no extra-grande", L"Extra large only", L"Solo en extragrande", L"Très grand uniquement", L"Nur bei extragroß", L"Solo extra grande", L"仅超大号"}},
    {{L"Espaço 8", L"Slot 8", L"Espacio 8", L"Emplacement 8", L"Platz 8", L"Spazio 8", L"位置 8"}},
    {{L"Temperatura em", L"Temperature in", L"Temperatura en", L"Température en", L"Temperatur in", L"Temperatura in", L"温度单位"}},
    {{L"A da CPU vem do LibreHardwareMonitor aberto (de graça, roda como administrador). Sem ele, a zona térmica ACPI, que em muitos PCs é da placa-mãe e não do processador (o widget marca \"ACPI\")", L"The CPU's comes from LibreHardwareMonitor running (free, runs as administrator). Without it, the ACPI thermal zone, which on many PCs belongs to the motherboard, not the processor (the widget marks it \"ACPI\")", L"La de la CPU viene de LibreHardwareMonitor abierto (gratis, se ejecuta como administrador). Sin él, la zona térmica ACPI, que en muchos PC es de la placa base y no del procesador (el widget marca \"ACPI\")", L"Celle du processeur vient de LibreHardwareMonitor ouvert (gratuit, s'exécute en administrateur). Sans lui, la zone thermique ACPI, qui sur beaucoup de PC est celle de la carte mère et non du processeur (le widget indique \"ACPI\")", L"Die CPU-Temperatur kommt vom laufenden LibreHardwareMonitor (kostenlos, läuft als Administrator). Ohne ihn die ACPI-Thermalzone, die bei vielen PCs zum Mainboard und nicht zum Prozessor gehört (das Widget markiert sie mit \"ACPI\")", L"Quella della CPU arriva da LibreHardwareMonitor aperto (gratuito, si esegue come amministratore). Senza di esso, la zona termica ACPI, che su molti PC è della scheda madre e non del processore (il widget indica \"ACPI\")", L"CPU 温度来自正在运行的 LibreHardwareMonitor（免费，需以管理员身份运行）。没有它时使用 ACPI 热区，在许多电脑上它是主板的温度而非处理器的（小组件会标记 \"ACPI\"）"}},
    {{L"Pasta das fotos", L"Photos folder", L"Carpeta de fotos", L"Dossier des photos", L"Fotoordner", L"Cartella delle foto", L"照片文件夹"}},
    {{L"Vazio: a pasta Imagens do OneDrive (ou a Imagens do PC)", L"Empty: the OneDrive Pictures folder (or the PC's Pictures)", L"Vacío: la carpeta Imágenes de OneDrive (o la Imágenes del PC)", L"Vide : le dossier Images de OneDrive (ou les Images du PC)", L"Leer: der OneDrive-Bilderordner (oder die Bilder des PCs)", L"Vuoto: la cartella Immagini di OneDrive (o le Immagini del PC)", L"留空：OneDrive 图片文件夹（或电脑的图片文件夹）"}},
    {{L"Endereço", L"Address", L"Dirección", L"Adresse", L"Adresse", L"Indirizzo", L"地址"}},
    {{L"O mesmo que você abre no navegador, por exemplo http://192.168.0.10:8123", L"The same one you open in the browser, for example http://192.168.0.10:8123", L"La misma que abres en el navegador, por ejemplo http://192.168.0.10:8123", L"La même que dans le navigateur, par exemple http://192.168.0.10:8123", L"Dieselbe, die du im Browser öffnest, z. B. http://192.168.0.10:8123", L"La stessa che apri nel browser, per esempio http://192.168.0.10:8123", L"与你在浏览器中打开的地址相同，例如 http://192.168.0.10:8123"}},
    {{L"Token de acesso", L"Access token", L"Token de acceso", L"Jeton d'accès", L"Zugriffstoken", L"Token di accesso", L"访问令牌"}},
    {{L"No Home Assistant: seu perfil > Segurança > Tokens de acesso de longa duração > Criar token. Ele dá acesso à sua casa: não compartilhe", L"In Home Assistant: your profile > Security > Long-lived access tokens > Create token. It gives access to your home: don't share it", L"En Home Assistant: tu perfil > Seguridad > Tokens de acceso de larga duración > Crear token. Da acceso a tu casa: no lo compartas", L"Dans Home Assistant : votre profil > Sécurité > Jetons d'accès longue durée > Créer un jeton. Il donne accès à votre maison : ne le partagez pas", L"In Home Assistant: dein Profil > Sicherheit > Langlebige Zugriffstoken > Token erstellen. Es gewährt Zugriff auf dein Zuhause: nicht weitergeben", L"In Home Assistant: il tuo profilo > Sicurezza > Token di accesso a lunga durata > Crea token. Dà accesso alla tua casa: non condividerlo", L"在 Home Assistant 中：你的个人资料 > 安全 > 长期访问令牌 > 创建令牌。它可访问你的家：请勿分享"}},
    {{L"Vazio: luzes, tomadas, ventiladores, cortinas, cenas e temperaturas, nessa ordem. Ou os IDs, separados por vírgula: light.sala, switch.tv, sensor.temperatura_quarto", L"Empty: lights, plugs, fans, blinds, scenes and temperatures, in that order. Or the IDs, separated by commas: light.sala, switch.tv, sensor.temperatura_quarto", L"Vacío: luces, enchufes, ventiladores, cortinas, escenas y temperaturas, en ese orden. O los ID, separados por comas: light.sala, switch.tv, sensor.temperatura_quarto", L"Vide : lumières, prises, ventilateurs, stores, scènes et températures, dans cet ordre. Ou les ID, séparés par des virgules : light.sala, switch.tv, sensor.temperatura_quarto", L"Leer: Lampen, Steckdosen, Ventilatoren, Rollos, Szenen und Temperaturen in dieser Reihenfolge. Oder die IDs, durch Komma getrennt: light.sala, switch.tv, sensor.temperatura_quarto", L"Vuoto: luci, prese, ventilatori, tende, scene e temperature, in quest'ordine. Oppure gli ID, separati da virgole: light.sala, switch.tv, sensor.temperatura_quarto", L"留空：依次显示灯、插座、风扇、窗帘、场景和温度。或填写 ID，用逗号分隔：light.sala、switch.tv、sensor.temperatura_quarto"}},
    {{L"Escolher favoritos…", L"Choose favorites…", L"Elegir favoritos…", L"Choisir les favoris…", L"Favoriten auswählen…", L"Scegli preferiti…", L"选择个人收藏…"}},
    {{L"Marque numa lista, com pesquisa, o que aparece no widget Casa", L"Check in a list, with search, what shows in the Home widget", L"Marca en una lista, con búsqueda, lo que aparece en el widget Casa", L"Cochez dans une liste, avec recherche, ce qui apparaît dans le widget Maison", L"Wähle in einer Liste mit Suche aus, was im Zuhause-Widget erscheint", L"Seleziona da un elenco, con ricerca, cosa appare nel widget Casa", L"在可搜索的列表中勾选家庭小组件要显示的内容"}},
    {{L"Idioma do MacOS Widgets", L"MacOS Widgets language", L"Idioma de MacOS Widgets", L"Langue de MacOS Widgets", L"Sprache von MacOS Widgets", L"Lingua di MacOS Widgets", L"MacOS Widgets 的语言"}},
    {{L"Automático segue o idioma do Windows. Datas e dias da semana mudam junto", L"Automatic follows the language of Windows. Dates and weekdays change with it", L"Automático sigue el idioma de Windows. Las fechas y los días de la semana cambian con él", L"Automatique suit la langue de Windows. Les dates et les jours de la semaine changent avec elle", L"Automatisch folgt der Sprache von Windows. Datum und Wochentage ändern sich mit", L"Automatico segue la lingua di Windows. Date e giorni della settimana cambiano di conseguenza", L"自动跟随 Windows 的语言。日期和星期也会随之改变"}},
    {{L"Backup", L"Backup", L"Copia de seguridad", L"Sauvegarde", L"Sicherung", L"Backup", L"备份"}},
    {{L"Fazer backup…", L"Back up…", L"Hacer copia de seguridad…", L"Sauvegarder…", L"Sichern…", L"Esegui backup…", L"备份…"}},
    {{L"Guarda num arquivo todos os ajustes (de todos os temas), os widgets ligados, seus tamanhos e lugares. Notas e lembretes já são arquivos: copie a pasta deles", L"Saves to a file all the settings (of every theme), the widgets that are on, their sizes and places. Notes and reminders are already files: copy their folder", L"Guarda en un archivo todos los ajustes (de todos los temas), los widgets activos, sus tamaños y posiciones. Las notas y los recordatorios ya son archivos: copia su carpeta", L"Enregistre dans un fichier tous les réglages (de tous les thèmes), les widgets activés, leurs tailles et emplacements. Les notes et rappels sont déjà des fichiers : copiez leur dossier", L"Speichert alle Einstellungen (aller Designs), die aktiven Widgets, ihre Größen und Positionen in einer Datei. Notizen und Erinnerungen sind bereits Dateien: kopiere ihren Ordner", L"Salva in un file tutte le impostazioni (di tutti i temi), i widget attivi, le loro dimensioni e posizioni. Note e promemoria sono già file: copia la loro cartella", L"将所有设置（包括每个主题）、已启用的小组件及其大小和位置保存到文件中。备忘录和提醒事项本身就是文件：复制它们的文件夹即可"}},
    {{L"Restaurar backup…", L"Restore backup…", L"Restaurar copia de seguridad…", L"Restaurer une sauvegarde…", L"Sicherung wiederherstellen…", L"Ripristina backup…", L"恢复备份…"}},
    {{L"Traz de volta tudo de um arquivo de backup, deste PC ou de outro", L"Brings everything back from a backup file, from this PC or another", L"Recupera todo de un archivo de copia de seguridad, de este PC o de otro", L"Restaure tout depuis un fichier de sauvegarde, de ce PC ou d'un autre", L"Stellt alles aus einer Sicherungsdatei wieder her, von diesem oder einem anderen PC", L"Ripristina tutto da un file di backup, di questo PC o di un altro", L"从备份文件恢复所有内容，可来自本机或其他电脑"}},
    {{L"Padrões", L"Defaults", L"Valores predeterminados", L"Valeurs par défaut", L"Standardwerte", L"Valori predefiniti", L"默认设置"}},
    {{L"Restaurar tudo", L"Restore everything", L"Restaurar todo", L"Tout rétablir", L"Alles zurücksetzen", L"Ripristina tutto", L"全部恢复"}},
    {{L"Desfaz tudo o que foi mudado nesta janela, em todos os temas: valem de novo os ajustes da aba Settings do Windhawk", L"Undoes everything changed in this window, in every theme: the settings in Windhawk's Settings tab apply again", L"Deshace todo lo cambiado en esta ventana, en todos los temas: vuelven a valer los ajustes de la pestaña Settings de Windhawk", L"Annule tout ce qui a été modifié dans cette fenêtre, dans tous les thèmes : les réglages de l'onglet Settings de Windhawk s'appliquent à nouveau", L"Macht alle Änderungen in diesem Fenster in allen Designs rückgängig: Es gelten wieder die Einstellungen im Settings-Tab von Windhawk", L"Annulla tutto ciò che è stato cambiato in questa finestra, in tutti i temi: tornano validi i valori della scheda Settings di Windhawk", L"撤销在此窗口中对所有主题所做的更改：重新使用 Windhawk 设置选项卡中的设置"}},
    {{L"Céu limpo", L"Clear sky", L"Cielo despejado", L"Ciel dégagé", L"Klarer Himmel", L"Cielo sereno", L"晴"}},
    {{L"Predominantemente limpo", L"Mostly clear", L"Mayormente despejado", L"Plutôt dégagé", L"Überwiegend klar", L"Prevalentemente sereno", L"大部晴朗"}},
    {{L"Parcialmente nublado", L"Partly cloudy", L"Parcialmente nublado", L"Partiellement nuageux", L"Teilweise bewölkt", L"Parzialmente nuvoloso", L"局部多云"}},
    {{L"Nublado", L"Cloudy", L"Nublado", L"Nuageux", L"Bewölkt", L"Nuvoloso", L"多云"}},
    {{L"Neblina", L"Fog", L"Niebla", L"Brouillard", L"Nebel", L"Nebbia", L"雾"}},
    {{L"Garoa", L"Drizzle", L"Llovizna", L"Bruine", L"Nieselregen", L"Pioviggine", L"毛毛雨"}},
    {{L"Chuva", L"Rain", L"Lluvia", L"Pluie", L"Regen", L"Pioggia", L"雨"}},
    {{L"Neve", L"Snow", L"Nieve", L"Neige", L"Schnee", L"Neve", L"雪"}},
    {{L"Pancadas de chuva", L"Rain showers", L"Chubascos", L"Averses", L"Regenschauer", L"Rovesci", L"阵雨"}},
    {{L"Pancadas de neve", L"Snow showers", L"Chubascos de nieve", L"Averses de neige", L"Schneeschauer", L"Rovesci di neve", L"阵雪"}},
    {{L"Trovoadas", L"Thunderstorms", L"Tormentas", L"Orages", L"Gewitter", L"Temporali", L"雷暴"}},
    {{L"Este PC", L"This PC", L"Este equipo", L"Ce PC", L"Dieser PC", L"Questo PC", L"此电脑"}},
    {{L"Vincular ao Celular", L"Phone Link", L"Vincular al teléfono", L"Mobile connecté", L"Smartphone-Link", L"Collegamento al telefono", L"手机连接"}},
    {{L"Segurança do Windows", L"Windows Security", L"Seguridad de Windows", L"Sécurité Windows", L"Windows-Sicherheit", L"Sicurezza di Windows", L"Windows 安全中心"}},
    {{L"Configurações", L"Settings", L"Configuración", L"Paramètres", L"Einstellungen", L"Impostazioni", L"设置"}},
    {{L"O Windows não tem o leitor de notificações", L"Windows doesn't have the notification reader", L"Windows no tiene el lector de notificaciones", L"Windows n'a pas le lecteur de notifications", L"Windows hat den Mitteilungsleser nicht", L"Windows non ha il lettore di notifiche", L"Windows 缺少通知读取组件"}},
    {{L"Não deu para ler as notificações do Windows", L"Couldn't read Windows' notifications", L"No se pudieron leer las notificaciones de Windows", L"Impossible de lire les notifications de Windows", L"Die Windows-Mitteilungen konnten nicht gelesen werden", L"Impossibile leggere le notifiche di Windows", L"无法读取 Windows 通知"}},
    {{L"Não deu para ler os calendários (confira os links e arquivos)", L"Couldn't read the calendars (check the links and files)", L"No se pudieron leer los calendarios (revisa los enlaces y archivos)", L"Impossible de lire les calendriers (vérifiez les liens et fichiers)", L"Die Kalender konnten nicht gelesen werden (Links und Dateien prüfen)", L"Impossibile leggere i calendari (controlla link e file)", L"无法读取日历（请检查链接和文件）"}},
    {{L"Não deu para ler o calendário (confira o link ou o arquivo)", L"Couldn't read the calendar (check the link or the file)", L"No se pudo leer el calendario (revisa el enlace o el archivo)", L"Impossible de lire le calendrier (vérifiez le lien ou le fichier)", L"Der Kalender konnte nicht gelesen werden (Link oder Datei prüfen)", L"Impossibile leggere il calendario (controlla il link o il file)", L"无法读取日历（请检查链接或文件）"}},
    {{L"Um dos calendários não abriu", L"One of the calendars didn't open", L"Uno de los calendarios no se abrió", L"Un des calendriers ne s'est pas ouvert", L"Einer der Kalender ließ sich nicht öffnen", L"Uno dei calendari non si è aperto", L"有一个日历无法打开"}},
    {{L"Não deu para buscar as cotações agora", L"Couldn't get the quotes right now", L"No se pudieron obtener las cotizaciones ahora", L"Impossible d'obtenir les cours pour l'instant", L"Die Kurse konnten gerade nicht abgerufen werden", L"Impossibile ottenere le quotazioni ora", L"暂时无法获取行情"}},
    {{L"há %lld min", L"%lld min ago", L"hace %lld min", L"il y a %lld min", L"vor %lld Min.", L"%lld min fa", L"%lld 分钟前"}},
    {{L"há %lld h", L"%lld h ago", L"hace %lld h", L"il y a %lld h", L"vor %lld Std.", L"%lld h fa", L"%lld 小时前"}},
    {{L"há %lld dia", L"%lld day ago", L"hace %lld día", L"il y a %lld jour", L"vor %lld Tag", L"%lld giorno fa", L"%lld 天前"}},
    {{L"há %lld dias", L"%lld days ago", L"hace %lld días", L"il y a %lld jours", L"vor %lld Tagen", L"%lld giorni fa", L"%lld 天前"}},
    {{L"Nenhuma notícia nesse endereço. Ele precisa ser um feed RSS (ex.: g1.globo.com/rss/g1/brasil/)", L"No news at this address. It needs to be an RSS feed (e.g. g1.globo.com/rss/g1/brasil/)", L"No hay noticias en esta dirección. Debe ser un feed RSS (ej.: g1.globo.com/rss/g1/brasil/)", L"Aucune actualité à cette adresse. Il doit s'agir d'un flux RSS (ex. : g1.globo.com/rss/g1/brasil/)", L"Keine Nachrichten unter dieser Adresse. Es muss ein RSS-Feed sein (z. B. g1.globo.com/rss/g1/brasil/)", L"Nessuna notizia a questo indirizzo. Deve essere un feed RSS (es.: g1.globo.com/rss/g1/brasil/)", L"此地址没有新闻。它必须是 RSS 订阅源（例如 g1.globo.com/rss/g1/brasil/）"}},
    {{L"Ligue o Home Assistant em Ajustes > Casa", L"Set up Home Assistant in Settings > Home", L"Configura Home Assistant en Ajustes > Casa", L"Configurez Home Assistant dans Réglages > Maison", L"Richte Home Assistant unter Einstellungen > Zuhause ein", L"Configura Home Assistant in Impostazioni > Casa", L"请在设置 > 家庭中连接 Home Assistant"}},
    {{L"O Home Assistant recusou o token (confira em Ajustes > Casa)", L"Home Assistant refused the token (check it in Settings > Home)", L"Home Assistant rechazó el token (revísalo en Ajustes > Casa)", L"Home Assistant a refusé le jeton (vérifiez dans Réglages > Maison)", L"Home Assistant hat das Token abgelehnt (prüfe es unter Einstellungen > Zuhause)", L"Home Assistant ha rifiutato il token (controlla in Impostazioni > Casa)", L"Home Assistant 拒绝了令牌（请在设置 > 家庭中检查）"}},
    {{L"Não deu para falar com o Home Assistant em %ls", L"Couldn't reach Home Assistant at %ls", L"No se pudo conectar con Home Assistant en %ls", L"Impossible de joindre Home Assistant à %ls", L"Home Assistant unter %ls nicht erreichbar", L"Impossibile raggiungere Home Assistant su %ls", L"无法连接到 %ls 上的 Home Assistant"}},
    {{L"Disco", L"Disk", L"Disco", L"Disque", L"Datenträger", L"Disco", L"磁盘"}},
    {{L"ligado há %llu d %llu h", L"up %llu d %llu h", L"encendido hace %llu d %llu h", L"actif depuis %llu j %llu h", L"seit %llu T. %llu Std. an", L"acceso da %llu g %llu h", L"已运行 %llu 天 %llu 小时"}},
    {{L"ligado há %llu h %llu min", L"up %llu h %llu min", L"encendido hace %llu h %llu min", L"actif depuis %llu h %llu min", L"seit %llu Std. %llu Min. an", L"acceso da %llu h %llu min", L"已运行 %llu 小时 %llu 分钟"}},
    {{L"Carregando…", L"Loading…", L"Cargando…", L"Chargement…", L"Wird geladen…", L"Caricamento…", L"正在加载…"}},
    {{L"Máx. %.0f°  Mín. %.0f°", L"H %.0f°  L %.0f°", L"Máx. %.0f°  Mín. %.0f°", L"Max. %.0f°  Min. %.0f°", L"H %.0f°  T %.0f°", L"Max %.0f°  Min %.0f°", L"最高 %.0f°  最低 %.0f°"}},
    {{L"Agora", L"Now", L"Ahora", L"Maintenant", L"Jetzt", L"Ora", L"现在"}},
    {{L"Hoje", L"Today", L"Hoy", L"Aujourd'hui", L"Heute", L"Oggi", L"今天"}},
    {{L"SENSAÇÃO", L"FEELS LIKE", L"SENSACIÓN", L"RESSENTI", L"GEFÜHLT", L"PERCEPITA", L"体感"}},
    {{L"UMIDADE", L"HUMIDITY", L"HUMEDAD", L"HUMIDITÉ", L"FEUCHTIGKEIT", L"UMIDITÀ", L"湿度"}},
    {{L"VENTO", L"WIND", L"VIENTO", L"VENT", L"WIND", L"VENTO", L"风"}},
    {{L"CHANCE DE CHUVA", L"CHANCE OF RAIN", L"PROB. DE LLUVIA", L"RISQUE DE PLUIE", L"REGENRISIKO", L"PROB. DI PIOGGIA", L"降雨概率"}},
    {{L"Amanhã", L"Tomorrow", L"Mañana", L"Demain", L"Morgen", L"Domani", L"明天"}},
    {{L"Ontem", L"Yesterday", L"Ayer", L"Hier", L"Gestern", L"Ieri", L"昨天"}},
    {{L"NASCER DO SOL", L"SUNRISE", L"AMANECER", L"LEVER DU SOLEIL", L"SONNENAUFGANG", L"ALBA", L"日出"}},
    {{L"PÔR DO SOL", L"SUNSET", L"ATARDECER", L"COUCHER DU SOLEIL", L"SONNENUNTERGANG", L"TRAMONTO", L"日落"}},
    {{L"Nascer: ", L"Sunrise: ", L"Amanecer: ", L"Lever : ", L"Aufgang: ", L"Alba: ", L"日出："}},
    {{L"Pôr: ", L"Sunset: ", L"Atardecer: ", L"Coucher : ", L"Untergang: ", L"Tramonto: ", L"日落："}},
    {{L"Nascer  ", L"Sunrise  ", L"Amanecer  ", L"Lever  ", L"Aufgang  ", L"Alba  ", L"日出  "}},
    {{L"Pôr  ", L"Sunset  ", L"Atardecer  ", L"Coucher  ", L"Untergang  ", L"Tramonto  ", L"日落  "}},
    {{L"Duração do dia", L"Day length", L"Duración del día", L"Durée du jour", L"Tageslänge", L"Durata del giorno", L"白昼时长"}},
    {{L"Amanhã, nascer", L"Tomorrow, sunrise", L"Mañana, amanecer", L"Demain, lever", L"Morgen, Aufgang", L"Domani, alba", L"明天日出"}},
    {{L"Amanhã, pôr", L"Tomorrow, sunset", L"Mañana, atardecer", L"Demain, coucher", L"Morgen, Untergang", L"Domani, tramonto", L"明天日落"}},
    {{L"Cidade", L"City", L"Ciudad", L"Ville", L"Stadt", L"Città", L"城市"}},
    {{L"Nenhuma bateria encontrada", L"No battery found", L"No se encontró ninguna batería", L"Aucune batterie trouvée", L"Kein Akku gefunden", L"Nessuna batteria trovata", L"未找到电池"}},
    {{L"Fones, mouses e controles Bluetooth que informam a bateria ao Windows aparecem aqui", L"Bluetooth headphones, mice and controllers that report their battery to Windows show up here", L"Aquí aparecen los auriculares, ratones y mandos Bluetooth que informan su batería a Windows", L"Les casques, souris et manettes Bluetooth qui communiquent leur batterie à Windows apparaissent ici", L"Bluetooth-Kopfhörer, -Mäuse und -Controller, die ihren Akkustand an Windows melden, erscheinen hier", L"Qui compaiono cuffie, mouse e controller Bluetooth che comunicano la batteria a Windows", L"向 Windows 报告电量的蓝牙耳机、鼠标和手柄会显示在这里"}},
    {{L"Enviado desde que o widget abriu", L"Sent since the widget opened", L"Enviado desde que se abrió el widget", L"Envoyé depuis l'ouverture du widget", L"Gesendet, seit das Widget geöffnet ist", L"Inviato da quando il widget è aperto", L"自小组件打开以来已发送"}},
    {{L"Pico de download (1 min)", L"Download peak (1 min)", L"Pico de descarga (1 min)", L"Pic de téléchargement (1 min)", L"Download-Spitze (1 Min.)", L"Picco di download (1 min)", L"下载峰值（1 分钟）"}},
    {{L"Nenhuma foto encontrada", L"No photo found", L"No se encontró ninguna foto", L"Aucune photo trouvée", L"Kein Foto gefunden", L"Nessuna foto trovata", L"未找到照片"}},
    {{L"Fotos baixadas na pasta Imagens do OneDrive (ou a escolhida nos Ajustes) aparecem aqui", L"Photos downloaded to the OneDrive Pictures folder (or the one chosen in Settings) show up here", L"Aquí aparecen las fotos descargadas en la carpeta Imágenes de OneDrive (o la elegida en Ajustes)", L"Les photos téléchargées dans le dossier Images de OneDrive (ou celui choisi dans les Réglages) apparaissent ici", L"Fotos im OneDrive-Bilderordner (oder dem in den Einstellungen gewählten) erscheinen hier", L"Qui compaiono le foto scaricate nella cartella Immagini di OneDrive (o in quella scelta nelle Impostazioni)", L"下载到 OneDrive 图片文件夹（或设置中所选文件夹）的照片会显示在这里"}},
    {{L"FOTOS", L"PHOTOS", L"FOTOS", L"PHOTOS", L"FOTOS", L"FOTO", L"照片"}},
    {{L"Nenhuma notificação", L"No notifications", L"Ninguna notificación", L"Aucune notification", L"Keine Mitteilungen", L"Nessuna notifica", L"没有通知"}},
    {{L"Limpar", L"Clear", L"Borrar", L"Effacer", L"Löschen", L"Cancella", L"清除"}},
    {{L"+ %d notificação anterior", L"+ %d earlier notification", L"+ %d notificación anterior", L"+ %d notification précédente", L"+ %d frühere Mitteilung", L"+ %d notifica precedente", L"+ %d 条更早的通知"}},
    {{L"+ %d notificações anteriores", L"+ %d earlier notifications", L"+ %d notificaciones anteriores", L"+ %d notifications précédentes", L"+ %d frühere Mitteilungen", L"+ %d notifiche precedenti", L"+ %d 条更早的通知"}},
    {{L"Dia inteiro", L"All day", L"Todo el día", L"Toute la journée", L"Ganztägig", L"Tutto il giorno", L"全天"}},
    {{L"(sem título)", L"(untitled)", L"(sin título)", L"(sans titre)", L"(ohne Titel)", L"(senza titolo)", L"（无标题）"}},
    {{L"Ligue seu calendário em Ajustes > Calendário", L"Connect your calendar in Settings > Calendar", L"Conecta tu calendario en Ajustes > Calendario", L"Connectez votre calendrier dans Réglages > Calendrier", L"Verbinde deinen Kalender unter Einstellungen > Kalender", L"Collega il tuo calendario in Impostazioni > Calendario", L"请在设置 > 日历中连接你的日历"}},
    {{L"Nenhum evento", L"No events", L"Ningún evento", L"Aucun événement", L"Keine Termine", L"Nessun evento", L"没有日程"}},
    {{L"Título", L"Title", L"Título", L"Titre", L"Titel", L"Titolo", L"标题"}},
    {{L"Nenhuma nota ainda", L"No notes yet", L"Aún no hay notas", L"Aucune note pour l'instant", L"Noch keine Notizen", L"Ancora nessuna nota", L"还没有备忘录"}},
    {{L"Clique em + e escreva aqui mesmo. Elas ficam em Documentos\\MacOS Widgets\\Notes", L"Click + and write right here. They're kept in Documents\\MacOS Widgets\\Notes", L"Haz clic en + y escribe aquí mismo. Se guardan en Documentos\\MacOS Widgets\\Notes", L"Cliquez sur + et écrivez ici même. Elles sont gardées dans Documents\\MacOS Widgets\\Notes", L"Klicke auf + und schreib direkt hier. Sie liegen in Dokumente\\MacOS Widgets\\Notes", L"Fai clic su + e scrivi qui. Vengono salvate in Documenti\\MacOS Widgets\\Notes", L"点击 + 直接在这里书写。它们保存在 文档\\MacOS Widgets\\Notes"}},
    {{L"Sem mais texto", L"No more text", L"Sin más texto", L"Pas d'autre texte", L"Kein weiterer Text", L"Nessun altro testo", L"无更多文本"}},
    {{L"Novo lembrete", L"New reminder", L"Nuevo recordatorio", L"Nouveau rappel", L"Neue Erinnerung", L"Nuovo promemoria", L"新提醒事项"}},
    {{L"Clique em + para escrever um lembrete", L"Click + to write a reminder", L"Haz clic en + para escribir un recordatorio", L"Cliquez sur + pour écrire un rappel", L"Klicke auf +, um eine Erinnerung zu schreiben", L"Fai clic su + per scrivere un promemoria", L"点击 + 添加提醒事项"}},
    {{L"Escolha os apps, pastas e sites em Ajustes > Atalhos", L"Choose the apps, folders and sites in Settings > Shortcuts", L"Elige las apps, carpetas y sitios en Ajustes > Atajos", L"Choisissez les apps, dossiers et sites dans Réglages > Raccourcis", L"Wähle Apps, Ordner und Websites unter Einstellungen > Kurzbefehle", L"Scegli app, cartelle e siti in Impostazioni > Scorciatoie", L"请在设置 > 快捷方式中选择应用、文件夹和网站"}},
    {{L"Conta o tempo de cada app a partir de agora (enquanto você usa o PC)", L"Counts the time in each app from now on (while you use the PC)", L"Cuenta el tiempo de cada app a partir de ahora (mientras usas el PC)", L"Compte le temps de chaque app à partir de maintenant (pendant que vous utilisez le PC)", L"Zählt ab jetzt die Zeit jeder App (während du den PC nutzt)", L"Conta il tempo di ogni app da adesso (mentre usi il PC)", L"从现在开始统计每个应用的使用时间（在你使用电脑时）"}},
    {{L"Indisponível", L"Unavailable", L"No disponible", L"Indisponible", L"Nicht verfügbar", L"Non disponibile", L"不可用"}},
    {{L"Tocar para ativar", L"Tap to activate", L"Toca para activar", L"Touchez pour activer", L"Zum Aktivieren tippen", L"Tocca per attivare", L"点击启用"}},
    {{L"Aberta", L"Open", L"Abierta", L"Ouvert", L"Offen", L"Aperta", L"已打开"}},
    {{L"Fechada", L"Closed", L"Cerrada", L"Fermé", L"Geschlossen", L"Chiusa", L"已关闭"}},
    {{L"Abrindo", L"Opening", L"Abriendo", L"Ouverture", L"Öffnet", L"In apertura", L"正在打开"}},
    {{L"Fechando", L"Closing", L"Cerrando", L"Fermeture", L"Schließt", L"In chiusura", L"正在关闭"}},
    {{L"Trancada", L"Locked", L"Cerrada con llave", L"Verrouillé", L"Verriegelt", L"Chiusa a chiave", L"已上锁"}},
    {{L"Destrancada", L"Unlocked", L"Abierta con llave", L"Déverrouillé", L"Entriegelt", L"Sbloccata", L"已解锁"}},
    {{L"Tocando", L"Playing", L"Reproduciendo", L"En lecture", L"Wiedergabe", L"In riproduzione", L"正在播放"}},
    {{L"Pausado", L"Paused", L"En pausa", L"En pause", L"Pausiert", L"In pausa", L"已暂停"}},
    {{L"Desligado", L"Off", L"Apagado", L"Éteint", L"Aus", L"Spento", L"关"}},
    {{L"Ligado", L"On", L"Encendido", L"Allumé", L"An", L"Acceso", L"开"}},
    {{L"Resfriando", L"Cooling", L"Enfriando", L"Refroidissement", L"Kühlt", L"Raffreddamento", L"制冷中"}},
    {{L"Aquecendo", L"Heating", L"Calentando", L"Chauffage", L"Heizt", L"Riscaldamento", L"制热中"}},
    {{L"Acesa", L"On", L"Encendida", L"Allumée", L"An", L"Accesa", L"开"}},
    {{L"Apagada", L"Off", L"Apagada", L"Éteinte", L"Aus", L"Spenta", L"关"}},
    {{L"TOCANDO", L"PLAYING", L"REPRODUCIENDO", L"EN LECTURE", L"WIEDERGABE", L"IN RIPRODUZIONE", L"正在播放"}},
    {{L"PAUSADO", L"PAUSED", L"EN PAUSA", L"EN PAUSE", L"PAUSIERT", L"IN PAUSA", L"已暂停"}},
    {{L"NADA TOCANDO", L"NOTHING PLAYING", L"NADA EN REPRODUCCIÓN", L"RIEN EN LECTURE", L"KEINE WIEDERGABE", L"NIENTE IN RIPRODUZIONE", L"未在播放"}},
    {{L"Abra um player de música", L"Open a music player", L"Abre un reproductor de música", L"Ouvrez un lecteur de musique", L"Öffne einen Musikplayer", L"Apri un lettore musicale", L"打开音乐播放器"}},
    {{L"Sem título", L"Untitled", L"Sin título", L"Sans titre", L"Ohne Titel", L"Senza titolo", L"无标题"}},
    {{L"Memória", L"Memory", L"Memoria", L"Mémoire", L"Speicher", L"Memoria", L"内存"}},
    {{L"Disco C:", L"Drive C:", L"Disco C:", L"Disque C:", L"Laufwerk C:", L"Disco C:", L"C 盘"}},
    {{L"Memória em uso", L"Memory in use", L"Memoria en uso", L"Mémoire utilisée", L"Belegter Speicher", L"Memoria in uso", L"已用内存"}},
    {{L"%ls de %ls", L"%ls of %ls", L"%ls de %ls", L"%ls sur %ls", L"%ls von %ls", L"%ls di %ls", L"%ls / %ls"}},
    {{L"Disco C: livre", L"Drive C: free", L"Disco C: libre", L"Disque C: libre", L"Laufwerk C: frei", L"Disco C: libero", L"C 盘可用"}},
    {{L"Processadores lógicos", L"Logical processors", L"Procesadores lógicos", L"Processeurs logiques", L"Logische Prozessoren", L"Processori logici", L"逻辑处理器"}},
    {{L"Processos abertos", L"Open processes", L"Procesos abiertos", L"Processus ouverts", L"Offene Prozesse", L"Processi aperti", L"打开的进程"}},
    {{L"Escolha o que mostrar em Ajustes > Sensores", L"Choose what to show in Settings > Sensors", L"Elige qué mostrar en Ajustes > Sensores", L"Choisissez quoi afficher dans Réglages > Capteurs", L"Wähle unter Einstellungen > Sensoren, was angezeigt wird", L"Scegli cosa mostrare in Impostazioni > Sensori", L"请在设置 > 传感器中选择要显示的内容"}},
    {{L"Abra o LibreHardwareMonitor", L"Open LibreHardwareMonitor", L"Abre LibreHardwareMonitor", L"Ouvrez LibreHardwareMonitor", L"Öffne LibreHardwareMonitor", L"Apri LibreHardwareMonitor", L"请打开 LibreHardwareMonitor"}},
    {{L"Ex.: Campinas", L"E.g.: Campinas", L"Ej.: Campinas", L"Ex. : Campinas", L"Z. B.: Campinas", L"Es.: Campinas", L"例如：Campinas"}},
    {{L"Pesquisar ajustes", L"Search settings", L"Buscar ajustes", L"Rechercher dans les réglages", L"Einstellungen durchsuchen", L"Cerca impostazioni", L"搜索设置"}},
    {{L"Resultados", L"Results", L"Resultados", L"Résultats", L"Ergebnisse", L"Risultati", L"结果"}},
    {{L"Concluído", L"Done", L"Listo", L"Terminé", L"Fertig", L"Fine", L"完成"}},
    {{L"Usar", L"Use", L"Usar", L"Utiliser", L"Verwenden", L"Usa", L"使用"}},
    {{L"Nenhum ajuste encontrado", L"No settings found", L"No se encontró ningún ajuste", L"Aucun réglage trouvé", L"Keine Einstellungen gefunden", L"Nessuna impostazione trovata", L"未找到设置"}},
    {{L"Procurando \u201C%ls\u201D…", L"Searching “%ls”…", L"Buscando “%ls”…", L"Recherche de “%ls”…", L"Suche nach “%ls”…", L"Ricerca di “%ls”…", L"正在搜索“%ls”…"}},
    {{L"Calendário (*.ics)", L"Calendar (*.ics)", L"Calendario (*.ics)", L"Calendrier (*.ics)", L"Kalender (*.ics)", L"Calendario (*.ics)", L"日历 (*.ics)"}},
    {{L"Todos os arquivos", L"All files", L"Todos los archivos", L"Tous les fichiers", L"Alle Dateien", L"Tutti i file", L"所有文件"}},
    {{L"Importar calendário", L"Import calendar", L"Importar calendario", L"Importer un calendrier", L"Kalender importieren", L"Importa calendario", L"导入日历"}},
    {{L"Usando %ls", L"Using %ls", L"Usando %ls", L"Utilisation de %ls", L"Verwendet: %ls", L"In uso: %ls", L"正在使用 %ls"}},
    {{L"Pequeno", L"Small", L"Pequeño", L"Petit", L"Klein", L"Piccolo", L"小"}},
    {{L"Médio", L"Medium", L"Mediano", L"Moyen", L"Mittel", L"Medio", L"中"}},
    {{L"Grande", L"Large", L"Grande", L"Grand", L"Groß", L"Grande", L"大"}},
    {{L"Extra-grande", L"Extra Large", L"Extragrande", L"Très grand", L"Extragroß", L"Extra grande", L"超大"}},
    {{L"Editar Local…", L"Edit Location…", L"Editar ubicación…", L"Modifier le lieu…", L"Ort bearbeiten…", L"Modifica posizione…", L"编辑位置…"}},
    {{L"Ponteiros e Digital", L"Hands and Digital", L"Agujas y digital", L"Aiguilles et numérique", L"Zeiger und digital", L"Lancette e digitale", L"指针和数字"}},
    {{L"Abrir Pasta das Notas", L"Open Notes Folder", L"Abrir carpeta de notas", L"Ouvrir le dossier des notes", L"Notizordner öffnen", L"Apri cartella delle note", L"打开备忘录文件夹"}},
    {{L"Abrir no Bloco de Notas", L"Open in Notepad", L"Abrir en el Bloc de notas", L"Ouvrir dans le Bloc-notes", L"Im Editor öffnen", L"Apri nel Blocco note", L"在记事本中打开"}},
    {{L"Importar Calendário (.ics)…", L"Import Calendar (.ics)…", L"Importar calendario (.ics)…", L"Importer un calendrier (.ics)…", L"Kalender importieren (.ics)…", L"Importa calendario (.ics)…", L"导入日历 (.ics)…"}},
    {{L"Atualizar Notícias", L"Refresh News", L"Actualizar noticias", L"Actualiser les actualités", L"Nachrichten aktualisieren", L"Aggiorna notizie", L"刷新新闻"}},
    {{L"Escolher Favoritos…", L"Choose Favorites…", L"Elegir favoritos…", L"Choisir les favoris…", L"Favoriten auswählen…", L"Scegli preferiti…", L"选择个人收藏…"}},
    {{L"Escolher Atalhos…", L"Choose Shortcuts…", L"Elegir atajos…", L"Choisir les raccourcis…", L"Kurzbefehle auswählen…", L"Scegli scorciatoie…", L"选择快捷方式…"}},
    {{L"Remover Widget", L"Remove Widget", L"Eliminar widget", L"Supprimer le widget", L"Widget entfernen", L"Rimuovi widget", L"移除小组件"}},
    {{L"Organizar Widgets", L"Arrange Widgets", L"Organizar widgets", L"Organiser les widgets", L"Widgets anordnen", L"Disponi widget", L"整理小组件"}},
    {{L"Ajustes do MacOS Widgets…", L"MacOS Widgets Settings…", L"Ajustes de MacOS Widgets…", L"Réglages de MacOS Widgets…", L"MacOS Widgets-Einstellungen…", L"Impostazioni di MacOS Widgets…", L"MacOS Widgets 设置…"}},
    {{L"Backup do MacOS Widgets", L"MacOS Widgets backup", L"Copia de seguridad de MacOS Widgets", L"Sauvegarde de MacOS Widgets", L"MacOS Widgets-Sicherung", L"Backup di MacOS Widgets", L"MacOS Widgets 备份"}},
    {{L"Fazer backup", L"Back up", L"Hacer copia de seguridad", L"Sauvegarder", L"Sichern", L"Esegui backup", L"备份"}},
    {{L"Restaurar backup", L"Restore backup", L"Restaurar copia de seguridad", L"Restaurer une sauvegarde", L"Sicherung wiederherstellen", L"Ripristina backup", L"恢复备份"}},
    {{L"Luz", L"Light", L"Luz", L"Lumière", L"Licht", L"Luce", L"灯"}},
    {{L"Tomada / interruptor", L"Plug / switch", L"Enchufe / interruptor", L"Prise / interrupteur", L"Steckdose / Schalter", L"Presa / interruttore", L"插座 / 开关"}},
    {{L"Ventilador", L"Fan", L"Ventilador", L"Ventilateur", L"Ventilator", L"Ventilatore", L"风扇"}},
    {{L"Cortina / persiana", L"Blind / shade", L"Cortina / persiana", L"Store / volet", L"Rollo / Jalousie", L"Tenda / tapparella", L"窗帘 / 百叶窗"}},
    {{L"Ar-condicionado / termostato", L"Air conditioning / thermostat", L"Aire acondicionado / termostato", L"Climatisation / thermostat", L"Klimaanlage / Thermostat", L"Condizionatore / termostato", L"空调 / 恒温器"}},
    {{L"Mídia", L"Media", L"Multimedia", L"Média", L"Medien", L"Media", L"媒体"}},
    {{L"Fechadura", L"Lock", L"Cerradura", L"Serrure", L"Schloss", L"Serratura", L"门锁"}},
    {{L"Cena", L"Scene", L"Escena", L"Scène", L"Szene", L"Scena", L"场景"}},
    {{L"Chave", L"Toggle", L"Interruptor", L"Interrupteur", L"Schalter", L"Interruttore", L"开关"}},
    {{L"Favoritos da Casa", L"Home Favorites", L"Favoritos de Casa", L"Favoris de Maison", L"Zuhause-Favoriten", L"Preferiti di Casa", L"家庭个人收藏"}},
    {{L"%d escolhido · na ordem em que você marcar", L"%d chosen · in the order you check them", L"%d elegido · en el orden en que los marques", L"%d choisi · dans l'ordre où vous les cochez", L"%d ausgewählt · in der Reihenfolge des Anhakens", L"%d scelto · nell'ordine in cui li selezioni", L"已选 %d 项 · 按勾选顺序排列"}},
    {{L"%d escolhidos · na ordem em que você marcar", L"%d chosen · in the order you check them", L"%d elegidos · en el orden en que los marques", L"%d choisis · dans l'ordre où vous les cochez", L"%d ausgewählt · in der Reihenfolge des Anhakens", L"%d scelti · nell'ordine in cui li selezioni", L"已选 %d 项 · 按勾选顺序排列"}},
    {{L"Nenhum marcado: o widget escolhe sozinho", L"None checked: the widget chooses by itself", L"Ninguno marcado: el widget elige solo", L"Aucun coché : le widget choisit tout seul", L"Nichts angehakt: Das Widget wählt selbst", L"Nessuno selezionato: il widget sceglie da solo", L"未勾选：由小组件自动选择"}},
    {{L"Marque o que quer no widget", L"Check what you want in the widget", L"Marca lo que quieres en el widget", L"Cochez ce que vous voulez dans le widget", L"Hake an, was ins Widget soll", L"Seleziona ciò che vuoi nel widget", L"勾选要在小组件中显示的内容"}},
    {{L"Nada com esse nome", L"Nothing by that name", L"Nada con ese nombre", L"Rien avec ce nom", L"Nichts mit diesem Namen", L"Niente con questo nome", L"没有该名称的项目"}},
    {{L"Cancelar", L"Cancel", L"Cancelar", L"Annuler", L"Abbrechen", L"Annulla", L"取消"}},
    {{L"A lista vem do seu Home Assistant: ligue o widget Casa e preencha o endereço e o token em Ajustes > Casa. Depois de alguns segundos, abra de novo.", L"The list comes from your Home Assistant: turn on the Home widget and fill in the address and the token in Settings > Home. After a few seconds, open this again.", L"La lista viene de tu Home Assistant: activa el widget Casa y completa la dirección y el token en Ajustes > Casa. Tras unos segundos, vuelve a abrirla.", L"La liste vient de votre Home Assistant : activez le widget Maison et renseignez l'adresse et le jeton dans Réglages > Maison. Après quelques secondes, rouvrez-la.", L"Die Liste kommt aus deinem Home Assistant: Schalte das Zuhause-Widget ein und trage Adresse und Token unter Einstellungen > Zuhause ein. Öffne sie nach ein paar Sekunden erneut.", L"L'elenco arriva dal tuo Home Assistant: attiva il widget Casa e inserisci indirizzo e token in Impostazioni > Casa. Dopo qualche secondo, riaprilo.", L"列表来自你的 Home Assistant：请开启家庭小组件，并在设置 > 家庭中填写地址和令牌。几秒钟后再次打开。"}},
    {{L"Documentos", L"Documents", L"Documentos", L"Documents", L"Dokumente", L"Documenti", L"文档"}},
    {{L"Downloads", L"Downloads", L"Descargas", L"Téléchargements", L"Downloads", L"Download", L"下载"}},
    {{L"Imagens", L"Pictures", L"Imágenes", L"Images", L"Bilder", L"Immagini", L"图片"}},
    {{L"Músicas", L"Music", L"Música", L"Musique", L"Musik", L"Musica", L"音乐"}},
    {{L"Vídeos", L"Videos", L"Vídeos", L"Vidéos", L"Videos", L"Video", L"视频"}},
    {{L"Área de Trabalho", L"Desktop", L"Escritorio", L"Bureau", L"Desktop", L"Desktop", L"桌面"}},
    {{L"Este Computador", L"This PC", L"Este equipo", L"Ce PC", L"Dieser PC", L"Questo PC", L"此电脑"}},
    {{L"Lixeira", L"Recycle Bin", L"Papelera de reciclaje", L"Corbeille", L"Papierkorb", L"Cestino", L"回收站"}},
    {{L"Explorador de Arquivos", L"File Explorer", L"Explorador de archivos", L"Explorateur de fichiers", L"Datei-Explorer", L"Esplora file", L"文件资源管理器"}},
    {{L"Nenhum app encontrado", L"No app found", L"No se encontró ninguna app", L"Aucune app trouvée", L"Keine App gefunden", L"Nessuna app trovata", L"未找到应用"}},
    {{L"Não foi possível procurar agora (sem internet?)", L"Couldn't search right now (no internet?)", L"No se pudo buscar ahora (¿sin internet?)", L"Impossible de rechercher pour l'instant (pas d'internet ?)", L"Suche gerade nicht möglich (kein Internet?)", L"Impossibile cercare ora (niente internet?)", L"暂时无法搜索（没有网络？）"}},
    {{L"Nenhuma cidade encontrada com esse nome", L"No city found with that name", L"No se encontró ninguna ciudad con ese nombre", L"Aucune ville trouvée avec ce nom", L"Keine Stadt mit diesem Namen gefunden", L"Nessuna città trovata con questo nome", L"未找到该名称的城市"}},
    {{L"%lld min", L"%lld min", L"%lld min", L"%lld min", L"%lld Min.", L"%lld min", L"%lld 分钟"}},
    {{L"%lld h", L"%lld h", L"%lld h", L"%lld h", L"%lld Std.", L"%lld h", L"%lld 小时"}},
    {{L"%lld d", L"%lld d", L"%lld d", L"%lld j", L"%lld T.", L"%lld g", L"%lld 天"}},
    {{L"%lld h %02lld min", L"%lld h %02lld min", L"%lld h %02lld min", L"%lld h %02lld min", L"%lld Std. %02lld Min.", L"%lld h %02lld min", L"%lld 小时 %02lld 分钟"}},
    {{L"Recebido desde que o widget abriu", L"Received since the widget opened", L"Recibido desde que se abrió el widget", L"Reçu depuis l'ouverture du widget", L"Empfangen, seit das Widget geöffnet ist", L"Ricevuto da quando il widget è aperto", L"自小组件打开以来已接收"}},
    {{L"agora", L"now", L"ahora", L"maintenant", L"jetzt", L"ora", L"刚刚"}},
    {{L"ontem", L"yesterday", L"ayer", L"hier", L"gestern", L"ieri", L"昨天"}},
};

static const std::vector<std::array<std::wstring, LANG_COUNT>>& TrTable() {
    static const auto table = [] {
        std::vector<std::array<std::wstring, LANG_COUNT>> t;
        for (const TrRow& r : kTr) {
            std::array<std::wstring, LANG_COUNT> a;
            for (int i = 0; i < LANG_COUNT; i++) a[i] = r.v[i] ? r.v[i] : L"";
            t.push_back(a);
        }
        return t;
    }();
    return table;
}

static const std::unordered_map<std::wstring, int>& TrIndex() {
    static const auto index = [] {
        std::unordered_map<std::wstring, int> m;
        const auto& t = TrTable();
        for (int i = 0; i < (int)t.size(); i++) m.emplace(t[i][LANG_PT], i);
        return m;
    }();
    return index;
}

// A text of the mod in the language in use (the same text when it isn't one of ours)
static const std::wstring& T(const std::wstring& s) {
    int lang = g_lang;
    if (lang == LANG_PT || s.empty()) return s;
    const auto& idx = TrIndex();
    auto it = idx.find(s);
    if (it == idx.end()) return s;
    const auto& row = TrTable()[it->second];
    return !row[lang].empty() ? row[lang] : !row[LANG_EN].empty() ? row[LANG_EN] : s;
}

// The same for a printf pattern (its %s stay in the same order in every language)
static PCWSTR TF(PCWSTR fmt) {
    int lang = g_lang;
    if (lang == LANG_PT) return fmt;
    const auto& idx = TrIndex();
    auto it = idx.find(fmt);
    if (it == idx.end()) return fmt;
    const auto& row = TrTable()[it->second];
    return !row[lang].empty() ? row[lang].c_str() : !row[LANG_EN].empty() ? row[LANG_EN].c_str() : fmt;
}

static std::wstring Fmt(PCWSTR fmt, ...) {
    wchar_t b[1024];
    va_list ap;
    va_start(ap, fmt);
    vswprintf(b, 1024, fmt, ap);
    va_end(ap);
    return b;
}

// Dates in the language in use ("Automático": the user's own region)
static PCWSTR LangCode() {
    static const PCWSTR codes[LANG_COUNT] = {L"pt", L"en", L"es", L"fr", L"de", L"it", L"zh"};
    return codes[g_lang];
}
static PCWSTR LangLocale() {
    static const PCWSTR names[LANG_COUNT] = {L"pt-BR", L"en-US", L"es-ES", L"fr-FR", L"de-DE", L"it-IT", L"zh-CN"};
    return g_langAuto ? LOCALE_NAME_USER_DEFAULT : names[g_lang];
}
static PCWSTR DayMonthFormat() {
    static const PCWSTR f[LANG_COUNT] = {L"d 'de' MMMM", L"MMMM d", L"d 'de' MMMM", L"d MMMM", L"d. MMMM", L"d MMMM", L"M月d日"};
    return f[g_lang];
}
static PCWSTR MonthYearFormat() {
    static const PCWSTR f[LANG_COUNT] = {L"MMMM 'de' yyyy", L"MMMM yyyy", L"MMMM 'de' yyyy", L"MMMM yyyy", L"MMMM yyyy", L"MMMM yyyy",
                                         L"yyyy年M月"};
    return f[g_lang];
}
static PCWSTR WeekdayShort(int d) {
    static const PCWSTR w[LANG_COUNT][7] = {
        {L"Dom", L"Seg", L"Ter", L"Qua", L"Qui", L"Sex", L"Sáb"}, {L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"},
        {L"Dom", L"Lun", L"Mar", L"Mié", L"Jue", L"Vie", L"Sáb"}, {L"Dim", L"Lun", L"Mar", L"Mer", L"Jeu", L"Ven", L"Sam"},
        {L"So", L"Mo", L"Di", L"Mi", L"Do", L"Fr", L"Sa"},        {L"Dom", L"Lun", L"Mar", L"Mer", L"Gio", L"Ven", L"Sab"},
        {L"周日", L"周一", L"周二", L"周三", L"周四", L"周五", L"周六"}};
    return w[g_lang][((d % 7) + 7) % 7];
}
static PCWSTR WeekdayLetter(int d) {
    static const PCWSTR w[LANG_COUNT][7] = {{L"D", L"S", L"T", L"Q", L"Q", L"S", L"S"}, {L"S", L"M", L"T", L"W", L"T", L"F", L"S"},
                                            {L"D", L"L", L"M", L"X", L"J", L"V", L"S"}, {L"D", L"L", L"M", L"M", L"J", L"V", L"S"},
                                            {L"S", L"M", L"D", L"M", L"D", L"F", L"S"}, {L"D", L"L", L"M", L"M", L"G", L"V", L"S"},
                                            {L"日", L"一", L"二", L"三", L"四", L"五", L"六"}};
    return w[g_lang][((d % 7) + 7) % 7];
}

// The sections of Ajustes, in the side bar's order: the general ones, one per widget that has settings, Backup last
enum CfgSectionId {
    SEC_WIDGETS, SEC_APPEARANCE, SEC_LANGUAGE, SEC_CLOCK, SEC_WEATHER, SEC_WORLD, SEC_CALENDAR, SEC_NOTES, SEC_SHORTCUTS, SEC_STOCKS, SEC_NEWS,
    SEC_SENSORS, SEC_PHOTOS, SEC_HOME, SEC_BACKUP
};
static const CfgSection kCfgSections[] = {
    {L"Widgets", L"Quais widgets aparecem na área de trabalho e como eles se comportam", L"", 0x30D158},
    {L"Aparência", L"O tema e, logo abaixo, tudo o que é dele: cada tema guarda o seu; o que ele não usa fica escondido",
     L"", 0x0A84FF},
    {L"Idioma", L"O idioma dos widgets, dos menus e desta janela", L"\uF2B7", 0x007AFF},
    {L"Relógio", L"Como o widget Relógio mostra a hora", L"", 0x5E5CE6},
    {L"Tempo", L"A cidade do widget Tempo", L"", 0x64D2FF},
    {L"Relógio Mundial", L"As cidades do widget Relógio Mundial", L"", 0x0A84FF},
    {L"Calendário", L"De onde vêm os eventos: links iCal (Google, Outlook…) ou arquivos .ics", L"", 0xFF453A},
    {L"Notas e Lembretes", L"Onde as notas e os lembretes são guardados", L"", 0xFFCC00},
    {L"Atalhos", L"Os apps, pastas e sites do widget Atalhos", L"", 0xBF5AF2},
    {L"Bolsa", L"As ações, moedas e índices do widget Bolsa", L"", 0x30D158},
    {L"Notícias", L"De onde vêm as notícias", L"", 0xFF375F},
    {L"Sensores", L"O que cada espaço do widget Sensores mostra", L"", 0xFF9F0A},
    {L"Fotos", L"A pasta das fotos do widget Fotos", L"", 0xFF9F0A},
    {L"Casa", L"Suas luzes, tomadas, cenas e sensores pelo Home Assistant (que reúne SmartThings, Matter e as marcas dos aparelhos)",
     L"", 0xFF6B35},
    {L"Backup e padrões", L"Guardar seus ajustes num arquivo, trazê-los de volta ou recomeçar do zero", L"", 0x8E8E93},
};
static const int kCfgSectionCount = (int)(sizeof(kCfgSections) / sizeof(kCfgSections[0]));

static const CfgOpt kOptTheme[] = {{L"frost", L"Vidro"},          {L"glass", L"Vidro líquido"},
                                   {L"tintedliquid", L"Vidro personalizado"}, {L"light", L"Claro"},
                                   {L"siri", L"Degradê Siri"},    {L"dark", L"Escuro"},
                                   {L"oled", L"OLED (preto profundo)"}};
// World clock cities: the Windows time zone of each
static const CfgOpt kOptCities[] = {
    {L"Pacific Standard Time", L"Cupertino"},          {L"Mountain Standard Time", L"Denver"},
    {L"Central Standard Time", L"Chicago"},            {L"Eastern Standard Time", L"Nova York"},
    {L"Central Standard Time (Mexico)", L"Cidade do México"}, {L"SA Pacific Standard Time", L"Bogotá"},
    {L"Pacific SA Standard Time", L"Santiago"},        {L"Argentina Standard Time", L"Buenos Aires"},
    {L"E. South America Standard Time", L"São Paulo"}, {L"UTC", L"UTC"},
    {L"GMT Standard Time", L"Londres"},                {L"Romance Standard Time", L"Paris"},
    {L"W. Europe Standard Time", L"Berlim"},           {L"South Africa Standard Time", L"Joanesburgo"},
    {L"Russian Standard Time", L"Moscou"},             {L"Arabian Standard Time", L"Dubai"},
    {L"India Standard Time", L"Nova Délhi"},           {L"Singapore Standard Time", L"Singapura"},
    {L"China Standard Time", L"Pequim"},               {L"Korea Standard Time", L"Seul"},
    {L"Tokyo Standard Time", L"Tóquio"},               {L"AUS Eastern Standard Time", L"Sydney"},
    {L"New Zealand Standard Time", L"Auckland"},
};
static const CfgOpt kOptText[] = {{L"auto", L"Automática"}, {L"white", L"Branco"}, {L"black", L"Preto"}};
#define CFG_OPTS(a) a, (int)(sizeof(a) / sizeof(a[0]))

// What a Sensors slot shows (in the order of SensorId)
static const CfgOpt kOptSensors[] = {
    {L"none", L"Nada"},
    {L"cpuTemp", L"Temperatura da CPU"},
    {L"gpuTemp", L"Temperatura da GPU"},
    {L"gpuHotspot", L"Hotspot da GPU"},
    {L"cpuLoad", L"Uso da CPU"},
    {L"gpuLoad", L"Uso da GPU"},
    {L"ram", L"Memória RAM"},
    {L"vram", L"Memória da GPU"},
    {L"cpuClock", L"Clock da CPU"},
    {L"gpuFan", L"Ventoinha da GPU"},
    {L"gpuPower", L"Consumo da GPU"},
    {L"cpuPower", L"Consumo da CPU"},
    {L"disk", L"Atividade do disco"},
    {L"netDown", L"Download"},
    {L"netUp", L"Upload"},
};
static_assert(sizeof(kOptSensors) / sizeof(kOptSensors[0]) == 15, "kOptSensors must follow SensorId");
static const CfgOpt kOptLanguage[] = {{L"auto", L"Automático (o do Windows)"}, {L"pt", L"Português"}, {L"en", L"English"},
                                      {L"es", L"Español"}, {L"fr", L"Français"}, {L"de", L"Deutsch"}, {L"it", L"Italiano"},
                                      {L"zh", L"中文（简体）"}};
static const CfgOpt kOptClockStyle[] = {{L"both", L"Ponteiros e digital"}, {L"analog", L"Ponteiros"}, {L"digital", L"Digital"}};
static const CfgOpt kOptTempUnit[] = {{L"c", L"Celsius (°C)"}, {L"f", L"Fahrenheit (°F)"}};

static const CfgItem kCfgItems[] = {
    {SEC_WIDGETS, kCfgGroup, L"#g.widgets", L"Na área de trabalho", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showClock", L"Relógio", L"Dia da semana, hora e data", L"1", false, 0, 1, nullptr, nullptr, 0},
    {SEC_CLOCK, kCfgGroup, L"#g.clock", L"Estilo", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_CLOCK, kCfgChoice, L"clockStyle", L"Estilo do relógio", L"Também no botão direito do Relógio", L"both", false, 0, 0, nullptr,
     CFG_OPTS(kOptClockStyle)},
    {SEC_WIDGETS, kCfgToggle, L"showWeather", L"Tempo", L"Temperatura, condição, máxima e mínima", L"1", false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showMusic", L"Música", L"O que está tocando, com anterior, tocar/pausar e próxima", L"1", false, 0, 1,
     nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showSystem", L"Sistema", L"CPU, GPU, memória, disco C: e tempo ligado", L"1", false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showCalendar", L"Calendário", L"O dia de hoje e o mês", L"0", false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showWorld", L"Relógio Mundial", L"A hora em outras cidades (escolha em Ajustes > Relógio Mundial)", L"0", false, 0, 1,
     nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showSun", L"Nascer do Sol", L"Nascer e pôr do sol na cidade do Tempo", L"0", false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showBattery", L"Baterias", L"A bateria do PC e a dos aparelhos Bluetooth que a informam ao Windows", L"0",
     false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showNetwork", L"Rede", L"Velocidade de download e upload agora", L"0", false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showPhotos", L"Fotos", L"Uma foto da sua pasta Imagens do OneDrive, trocada a cada 20 minutos", L"0", false,
     0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showNotif", L"Notificações",
     L"As notificações que chegam no Windows (WhatsApp, Instagram, Outlook...), lidas do banco de notificações do Windows",
     L"0", false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showNotes", L"Notas", L"Suas notas em texto (Documentos\\MacOS Widgets\\Notes)", L"0", false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showReminders", L"Lembretes", L"Uma lista de tarefas para marcar como feitas", L"0", false, 0, 1, nullptr,
     nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showShortcuts", L"Atalhos", L"Apps, pastas e sites a um clique", L"0", false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showStocks", L"Bolsa", L"Ações, índices, moedas e cripto", L"0", false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showNews", L"Notícias", L"As manchetes dos sites que você escolher (RSS)", L"0", false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showScreenTime", L"Tempo de Uso", L"Quanto tempo você passou em cada app hoje", L"0", false, 0, 1, nullptr,
     nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showHome", L"Casa", L"Luzes, tomadas, cenas e sensores do Home Assistant (ligue em Casa)", L"0", false, 0, 1,
     nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showSensors", L"Sensores", L"Temperaturas da CPU e da GPU, uso, clock, ventoinha e consumo, ao vivo", L"0",
     false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgGroup, L"#g.behavior", L"Comportamento", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"lockPosition", L"Travar posição", L"Os widgets deixam de ser arrastados", L"0", false, 0, 1, nullptr,
     nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"showGrid", L"Mostrar a grade ao arrastar",
     L"Ao arrastar um widget, todas as casas aparecem de leve; a casa onde ele vai ficar aparece sempre", L"0", false, 0, 1,
     nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgToggle, L"liveGlass", L"Vidro ao vivo",
     L"Ligado: vídeos e papéis de parede animados aparecem no vidro, mas usa mais o PC e os widgets somem de prints. Desligado: o "
     L"vidro mostra o papel de parede e os ícones, mais leve",
     L"1", false, 0, 1, nullptr, nullptr, 0},
    {SEC_WIDGETS, kCfgColor, L"accentColor", L"Cor de destaque", L"As barras do widget Sistema", L"#0A84FF", false, 0, 0, nullptr,
     nullptr, 0},

    {SEC_APPEARANCE, kCfgChoice, L"theme", L"Tema", L"As cores e o vidro de tudo: widgets e esta janela", L"glass", false, 0, 0, nullptr,
     CFG_OPTS(kOptTheme)},
    {SEC_APPEARANCE, kCfgGroup, L"#g.customize", L"Personalizar tema", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgButton, L"#themeReset", L"Restaurar tema padrão",
     L"Volta este tema a como ele vem. Os outros temas não mudam", L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgGroup, L"#g.look", L"Visual", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgSlider, L"transparency", L"Transparência do vidro", L"Quanto do que está atrás aparece. 100 = totalmente transparente",
     L"80", true, 0, 100, L"%", nullptr, 0},
    {SEC_APPEARANCE, kCfgSlider, L"blur", L"Desfoque do fundo", L"Quão embaçado fica o que está atrás. 0 = nenhum", L"60", true, 0, 100,
     nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgSlider, L"cornerRadius", L"Arredondamento dos cantos", nullptr, L"26", true, 0, 60, L"px", nullptr, 0},
    {SEC_APPEARANCE, kCfgChoice, L"textColor", L"Cor do texto",
     L"Automática: no vidro transparente, preto sobre um fundo claro e branco sobre um escuro", L"auto", true, 0, 0, nullptr,
     CFG_OPTS(kOptText)},
    {SEC_APPEARANCE, kCfgGroup, L"#g.glass", L"Vidro", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgSlider, L"refraction", L"Refração nas bordas", L"Quanto o vidro curva o que está atrás dele nas bordas. 0 = sem curvatura",
     L"70", true, 0, 100, L"%", nullptr, 0},
    {SEC_APPEARANCE, kCfgToggle, L"glassEffect", L"Efeito vidro", L"Brilho no topo, a espessura do vidro nas bordas e uma borda chanfrada",
     L"1", true, 0, 1, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgGroup, L"#g.light", L"Borda e retroiluminação", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgToggle, L"chromeBorder", L"Borda cromada animada", L"Uma borda polida, com a luz correndo em volta", L"0", true, 0,
     1, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgToggle, L"backlight", L"Retroiluminação", L"Um brilho colorido em volta, com as cores girando", L"0", true, 0, 1,
     nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgSlider, L"backlightIntensity", L"Intensidade da retroiluminação", nullptr, L"70", true, 5, 100, L"%", nullptr, 0},
    {SEC_APPEARANCE, kCfgSlider, L"backlightColors", L"Cores da retroiluminação", L"Quantas cores giram em volta (1 = uma cor com a luz correndo)",
     L"3", true, 1, 5, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgColor, L"backlightColor1", L"Cor 1", nullptr, L"#2F6BFF", true, 0, 0, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgColor, L"backlightColor2", L"Cor 2", nullptr, L"#B44BFF", true, 0, 0, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgColor, L"backlightColor3", L"Cor 3", nullptr, L"#FF4B8B", true, 0, 0, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgColor, L"backlightColor4", L"Cor 4", nullptr, L"#FFB020", true, 0, 0, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgColor, L"backlightColor5", L"Cor 5", nullptr, L"#20D3A0", true, 0, 0, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgGroup, L"#g.tinted", L"Vidro personalizado", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_APPEARANCE, kCfgSlider, L"tintLevel", L"Força da cor no topo",
     L"100 = sólido no topo; menos deixa o fundo aparecer também na parte escura", L"100", true, 0, 100, L"%", nullptr, 0},
    {SEC_APPEARANCE, kCfgSlider, L"tintReach", L"Até onde a cor desce", L"Onde a cor dá lugar ao vidro transparente", L"29", true, 0, 100,
     L"%", nullptr, 0},
    {SEC_APPEARANCE, kCfgColor, L"tintColor", L"Cor do vidro", L"Começa preto; qualquer cor funciona. Cores claras ganham texto escuro",
     L"#050505", true, 0, 0, nullptr, nullptr, 0},

    {SEC_WEATHER, kCfgGroup, L"#g.find", L"Procurar cidade", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_WEATHER, kCfgText, L"#citySearch", L"Pesquisar", L"Digite o nome da cidade e tecle Enter; depois clique em Usar", L"", false, 0, 0,
     nullptr, nullptr, 0},
    {SEC_WEATHER, kCfgGroup, L"#g.place", L"Local em uso", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_WEATHER, kCfgText, L"city", L"Nome da cidade", L"O nome que aparece no widget", L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_WEATHER, kCfgText, L"latitude", L"Latitude", L"Por exemplo -23.55", L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_WEATHER, kCfgText, L"longitude", L"Longitude", L"Por exemplo -46.63", L"", false, 0, 0, nullptr, nullptr, 0},

    {SEC_WORLD, kCfgGroup, L"#g.world", L"Cidades", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_WORLD, kCfgChoice, L"worldCity1", L"Cidade 1", L"A que aparece no tamanho pequeno", L"Pacific Standard Time", false, 0, 0,
     nullptr, CFG_OPTS(kOptCities)},
    {SEC_WORLD, kCfgChoice, L"worldCity2", L"Cidade 2", nullptr, L"Tokyo Standard Time", false, 0, 0, nullptr, CFG_OPTS(kOptCities)},
    {SEC_WORLD, kCfgChoice, L"worldCity3", L"Cidade 3", nullptr, L"AUS Eastern Standard Time", false, 0, 0, nullptr, CFG_OPTS(kOptCities)},
    {SEC_WORLD, kCfgChoice, L"worldCity4", L"Cidade 4", nullptr, L"Romance Standard Time", false, 0, 0, nullptr, CFG_OPTS(kOptCities)},
    {SEC_CALENDAR, kCfgGroup, L"#g.calendar", L"Calendários", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_CALENDAR, kCfgText, L"calendarIcs", L"Calendários (links iCal ou arquivos .ics)",
     L"Vários, separados por ponto e vírgula. Google Agenda: Configurações > seu calendário > Endereço secreto no formato iCal. "
     L"Outlook: Configurações > Calendário > Calendários compartilhados > Publicar (link ICS)",
     L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_CALENDAR, kCfgButton, L"#importIcs", L"Importar arquivo .ics…",
     L"Um arquivo exportado de qualquer agenda. O widget relê o arquivo sozinho, então trocar o arquivo atualiza os eventos", L"",
     false, 0, 0, nullptr, nullptr, 0},
    {SEC_NOTES, kCfgGroup, L"#g.texts", L"Pasta", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_NOTES, kCfgText, L"notesFolder", L"Pasta das notas e lembretes",
     L"Vazio: Documentos\\MacOS Widgets. Notas são arquivos .txt ou .md na subpasta Notes; os lembretes ficam em Reminders.txt",
     L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_SHORTCUTS, kCfgGroup, L"#g.shortcuts", L"No widget", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_SHORTCUTS, kCfgText, L"shortcuts", L"Atalhos", L"Nome=programa, pasta ou site, separados por ponto e vírgula",
     L"File Explorer=explorer.exe; Settings=ms-settings:; Calculator=calc.exe; Notepad=notepad.exe", false, 0, 0, nullptr,
     nullptr, 0},
    {SEC_SHORTCUTS, kCfgButton, L"#pickShortcuts", L"Escolher atalhos…", L"Uma lista com seus apps (da Loja também) e pastas, com pesquisa",
     L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_STOCKS, kCfgGroup, L"#g.stocks", L"Códigos", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_STOCKS, kCfgText, L"stocks", L"Códigos da Bolsa",
     L"Os do Yahoo Finance, separados por vírgula: PETR4.SA, ^BVSP (Ibovespa), USDBRL=X (dólar), BTC-USD", L"^GSPC, ^IXIC, ^DJI, BTC-USD",
     false, 0, 0, nullptr, nullptr, 0},
    {SEC_NEWS, kCfgGroup, L"#g.news", L"Feeds", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_NEWS, kCfgText, L"newsFeeds", L"Endereços RSS das notícias",
     L"Cole o endereço do site que você quer (ex.: https://www.bbc.com/portuguese): o widget acha o feed dele sozinho. Se não "
     L"achar, procure no site por \"RSS\" e cole esse endereço. Vários: separe com ponto e vírgula. As notícias de todos se "
     L"juntam, as mais novas primeiro. Do g1, cada seção tem o seu: g1.globo.com/rss/g1/ + tecnologia/, esporte/, sp/...",
     L"", false, 0, 0,
     nullptr, nullptr, 0},
    {SEC_SENSORS, kCfgGroup, L"#g.sensors", L"O que mostrar", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_SENSORS, kCfgChoice, L"sensor1", L"Espaço 1", L"Pequeno e médio mostram os 4 primeiros; grande, 6; extra-grande, os 8", L"cpuTemp", false, 0, 0, nullptr, CFG_OPTS(kOptSensors)},
    {SEC_SENSORS, kCfgChoice, L"sensor2", L"Espaço 2", nullptr, L"gpuTemp", false, 0, 0, nullptr, CFG_OPTS(kOptSensors)},
    {SEC_SENSORS, kCfgChoice, L"sensor3", L"Espaço 3", nullptr, L"cpuLoad", false, 0, 0, nullptr, CFG_OPTS(kOptSensors)},
    {SEC_SENSORS, kCfgChoice, L"sensor4", L"Espaço 4", nullptr, L"gpuLoad", false, 0, 0, nullptr, CFG_OPTS(kOptSensors)},
    {SEC_SENSORS, kCfgChoice, L"sensor5", L"Espaço 5", L"Só no grande e no extra-grande", L"ram", false, 0, 0, nullptr, CFG_OPTS(kOptSensors)},
    {SEC_SENSORS, kCfgChoice, L"sensor6", L"Espaço 6", L"Só no grande e no extra-grande", L"vram", false, 0, 0, nullptr, CFG_OPTS(kOptSensors)},
    {SEC_SENSORS, kCfgChoice, L"sensor7", L"Espaço 7", L"Só no extra-grande", L"gpuFan", false, 0, 0, nullptr, CFG_OPTS(kOptSensors)},
    {SEC_SENSORS, kCfgChoice, L"sensor8", L"Espaço 8", L"Só no extra-grande", L"cpuClock", false, 0, 0, nullptr, CFG_OPTS(kOptSensors)},
    {SEC_SENSORS, kCfgChoice, L"tempUnit", L"Temperatura em",
     L"A da CPU vem do LibreHardwareMonitor aberto (de graça, roda como administrador). Sem ele, a zona térmica ACPI, que em muitos "
     L"PCs é da placa-mãe e não do processador (o widget marca \"ACPI\")",
     L"c", false, 0, 0, nullptr, CFG_OPTS(kOptTempUnit)},
    {SEC_PHOTOS, kCfgGroup, L"#g.photos", L"Pasta", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_PHOTOS, kCfgText, L"photosFolder", L"Pasta das fotos", L"Vazio: a pasta Imagens do OneDrive (ou a Imagens do PC)", L"", false, 0, 0,
     nullptr, nullptr, 0},
    {SEC_HOME, kCfgGroup, L"#g.ha", L"Home Assistant", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_HOME, kCfgText, L"haUrl", L"Endereço", L"O mesmo que você abre no navegador, por exemplo http://192.168.0.10:8123", L"http://homeassistant.local:8123",
     false, 0, 0, nullptr, nullptr, 0},
    {SEC_HOME, kCfgText, L"haToken", L"Token de acesso",
     L"No Home Assistant: seu perfil > Segurança > Tokens de acesso de longa duração > Criar token. Ele dá acesso à sua casa: não "
     L"compartilhe",
     L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_HOME, kCfgText, L"haEntities", L"O que mostrar",
     L"Vazio: luzes, tomadas, ventiladores, cortinas, cenas e temperaturas, nessa ordem. Ou os IDs, separados por vírgula: "
     L"light.sala, switch.tv, sensor.temperatura_quarto",
     L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_HOME, kCfgButton, L"#pickHome", L"Escolher favoritos…", L"Marque numa lista, com pesquisa, o que aparece no widget Casa",
     L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_LANGUAGE, kCfgGroup, L"#g.language", L"Idioma", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_LANGUAGE, kCfgChoice, L"language", L"Idioma do MacOS Widgets",
     L"Automático segue o idioma do Windows. Datas e dias da semana mudam junto", L"auto", false, 0, 0, nullptr,
     CFG_OPTS(kOptLanguage)},
    {SEC_BACKUP, kCfgGroup, L"#g.backup", L"Backup", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_BACKUP, kCfgButton, L"#backupSave", L"Fazer backup…",
     L"Guarda num arquivo todos os ajustes (de todos os temas), os widgets ligados, seus tamanhos e lugares. Notas e lembretes já "
     L"são arquivos: copie a pasta deles",
     L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_BACKUP, kCfgButton, L"#backupLoad", L"Restaurar backup…",
     L"Traz de volta tudo de um arquivo de backup, deste PC ou de outro", L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_BACKUP, kCfgGroup, L"#g.defaults", L"Padrões", nullptr, L"", false, 0, 0, nullptr, nullptr, 0},
    {SEC_BACKUP, kCfgButton, L"#resetAll", L"Restaurar tudo",
     L"Desfaz tudo o que foi mudado nesta janela, em todos os temas: valem de novo os ajustes da aba Settings do Windhawk",
     L"", false, 0, 0, nullptr, nullptr, 0},
};
static const int kCfgItemCount = (int)(sizeof(kCfgItems) / sizeof(kCfgItems[0]));

static const CfgItem* CfgFind(PCWSTR key) {
    for (const CfgItem& it : kCfgItems)
        if (!wcscmp(it.key, key)) return &it;
    return nullptr;
}

// A value changed in the window wins over the Settings tab while the tab still holds the value it had then (`base`).
// As soon as the tab holds anything else, the override is dropped for good: the latest change wins, wherever it was made
struct CfgOverride {
    std::wstring value, base;
};
static std::vector<std::pair<std::wstring, CfgOverride>> g_cfgOver;                                     // key -> override
static std::vector<std::pair<std::wstring, std::vector<std::pair<std::wstring, CfgOverride>>>> g_themeVals;  // theme -> key -> override

// Stored records are separated by this, not by line breaks: portable Windhawk keeps values in an INI file, where a
// line break can't be stored
constexpr wchar_t kRecSep = L'\x1F';
static bool g_cfgLoaded = false;

template <class V>
static V* FindIn(std::vector<std::pair<std::wstring, V>>& v, const std::wstring& key) {
    for (auto& p : v)
        if (p.first == key) return &p.second;
    return nullptr;
}

static std::vector<std::wstring> Split(const std::wstring& s, wchar_t sep) {
    std::vector<std::wstring> out;
    size_t start = 0;
    while (true) {
        size_t p = s.find(sep, start);
        out.push_back(s.substr(start, p == std::wstring::npos ? std::wstring::npos : p - start));
        if (p == std::wstring::npos) break;
        start = p + 1;
    }
    return out;
}

static std::wstring ReadValue(PCWSTR name) {
    std::vector<wchar_t> buf(32768);
    Wh_GetStringValue(name, buf.data(), buf.size());
    buf.back() = 0;
    return buf.data();
}

static std::wstring CfgTab(const CfgItem& it);

static void CfgLoadStored() {  // (g_dataMutex held)
    g_cfgLoaded = true;
    g_cfgOver.clear();
    g_themeVals.clear();
    for (const auto& line : Split(ReadValue(L"cfgOverrides"), kRecSep)) {
        auto f = Split(line, L'\t');
        if (f.size() == 3 && CfgFind(f[0].c_str())) g_cfgOver.push_back({f[0], {f[1], f[2]}});
    }
    for (const auto& line : Split(ReadValue(L"themeValues"), kRecSep)) {
        auto f = Split(line, L'\t');
        if (f.size() != 4) continue;
        const CfgItem* it = CfgFind(f[1].c_str());
        if (!it || !it->themed) continue;
        auto* vals = FindIn(g_themeVals, f[0]);
        if (!vals) {
            g_themeVals.push_back({f[0], {}});
            vals = &g_themeVals.back().second;
        }
        vals->push_back({f[1], {f[2], f[3]}});
    }
}

static std::wstring CleanValue(std::wstring v) {
    for (auto& c : v)
        if (c == L'\t' || c == L'\n' || c == L'\r' || c == kRecSep) c = L' ';
    return v;
}

static void CfgSaveStored() {  // (g_dataMutex held)
    std::wstring a, b;
    for (const auto& p : g_cfgOver) a += p.first + L"\t" + CleanValue(p.second.value) + L"\t" + CleanValue(p.second.base) + kRecSep;
    for (const auto& t : g_themeVals)
        for (const auto& p : t.second)
            b += t.first + L"\t" + p.first + L"\t" + CleanValue(p.second.value) + L"\t" + CleanValue(p.second.base) + kRecSep;
    Wh_SetStringValue(L"cfgOverrides", a.c_str());
    Wh_SetStringValue(L"themeValues", b.c_str());
}

// What the Settings tab holds now
static std::wstring CfgTab(const CfgItem& it) {
    if (it.type == kCfgToggle || it.type == kCfgSlider) return std::to_wstring(Wh_GetIntSetting(it.key));
    PCWSTR v = Wh_GetStringSetting(it.key);
    // A list cleared in the tab stays empty; for the rest, empty means the starting value
    bool list = !wcscmp(it.key, L"stocks") || !wcscmp(it.key, L"newsFeeds") || !wcscmp(it.key, L"shortcuts");
    std::wstring r = (*v || list) ? v : it.def;
    Wh_FreeStringSetting(v);
    return r;
}

static std::wstring CfgShared(const CfgItem& it) {  // (g_dataMutex held)
    std::wstring tab = CfgTab(it);
    for (size_t i = 0; i < g_cfgOver.size(); i++)
        if (g_cfgOver[i].first == it.key) {
            if (g_cfgOver[i].second.base == tab) return g_cfgOver[i].second.value;
            g_cfgOver.erase(g_cfgOver.begin() + i);  // changed in the tab since: the tab wins from now on
            CfgSaveStored();
            break;
        }
    return tab;
}

// A theme's own starting value for a setting, as in Siri Plus Launchpad
static PCWSTR ThemeDefaultFor(const std::wstring& theme, PCWSTR key) {
    if (!wcscmp(key, L"transparency"))
        return theme == L"light" || theme == L"oled" ? L"0" : theme == L"tintedliquid" ? L"100" : L"";
    if (!wcscmp(key, L"blur")) return theme == L"tintedliquid" ? L"0" : L"";  // the bent picture stays sharp
    if (!wcscmp(key, L"chromeBorder")) return theme == L"oled" ? L"1" : L"";
    if (!wcscmp(key, L"glassEffect")) return theme == L"oled" ? L"0" : L"";
    return L"";
}

static std::wstring CfgThemeName() {  // (g_dataMutex held)
    const CfgItem* it = CfgFind(L"theme");
    return CfgShared(*it);
}

// The value in use: the theme's own, else its starting value (while the shared one is still the default), else shared
static std::wstring CfgNowLocked(const CfgItem& it) {
    if (!g_cfgLoaded) CfgLoadStored();
    std::wstring shared = CfgShared(it);
    if (!it.themed) return shared;
    std::wstring theme = CfgThemeName();
    if (auto* vals = FindIn(g_themeVals, theme))
        for (size_t i = 0; i < vals->size(); i++)
            if ((*vals)[i].first == it.key) {
                if ((*vals)[i].second.base == shared) return (*vals)[i].second.value;
                vals->erase(vals->begin() + i);  // changed in the tab since: the tab wins
                CfgSaveStored();
                break;
            }
    PCWSTR td = ThemeDefaultFor(theme, it.key);
    if (*td && shared == it.def) return td;
    return shared;
}

static std::wstring CfgNow(PCWSTR key) {
    const CfgItem* it = CfgFind(key);
    if (!it) return L"";
    std::lock_guard<std::mutex> lock(g_dataMutex);
    return CfgNowLocked(*it);
}

static void CfgSet(PCWSTR key, const std::wstring& value) {
    const CfgItem* it = CfgFind(key);
    if (!it) return;
    std::lock_guard<std::mutex> lock(g_dataMutex);
    if (!g_cfgLoaded) CfgLoadStored();
    std::wstring tab = CfgTab(*it);
    if (it->themed) {
        std::wstring theme = CfgThemeName();
        auto* vals = FindIn(g_themeVals, theme);
        if (!vals) {
            g_themeVals.push_back({theme, {}});
            vals = &g_themeVals.back().second;
        }
        if (CfgOverride* v = FindIn(*vals, key)) *v = {value, tab};
        else vals->push_back({key, {value, tab}});
    } else {
        if (CfgOverride* o = FindIn(g_cfgOver, key)) *o = {value, tab};
        else g_cfgOver.push_back({key, {value, tab}});
    }
    CfgSaveStored();
}

static bool CfgThemeChanged() {
    std::lock_guard<std::mutex> lock(g_dataMutex);
    if (!g_cfgLoaded) CfgLoadStored();
    auto* vals = FindIn(g_themeVals, CfgThemeName());
    return vals && !vals->empty();
}

static void CfgResetTheme() {
    std::lock_guard<std::mutex> lock(g_dataMutex);
    std::wstring theme = CfgThemeName();
    for (size_t i = 0; i < g_themeVals.size(); i++)
        if (g_themeVals[i].first == theme) {
            g_themeVals.erase(g_themeVals.begin() + i);
            break;
        }
    CfgSaveStored();
}

static void CfgResetAll() {
    std::lock_guard<std::mutex> lock(g_dataMutex);
    g_cfgOver.clear();
    g_themeVals.clear();
    CfgSaveStored();
}

static int CfgInt(PCWSTR key, int lo, int hi) { return std::clamp(_wtoi(CfgNow(key).c_str()), lo, hi); }

static std::atomic<bool> g_liveGlassOn{true};  // the "Vidro ao vivo" setting, for the glass thread
static std::atomic<bool> g_ddaOn{false};        // the live glass' screen duplication is working (else a 1-second capture)

static void LoadSettings() {
    Settings s;
    std::wstring t = CfgNow(L"theme");
    s.theme = t == L"frost" ? Theme::Frost : t == L"tintedliquid" ? Theme::Tinted : t == L"light" ? Theme::Light
            : t == L"siri" ? Theme::Siri : t == L"dark" ? Theme::Dark : t == L"oled" ? Theme::Oled : Theme::Glass;
    s.transparency = CfgInt(L"transparency", 0, 100);
    s.blur = CfgInt(L"blur", 0, 100);
    s.refraction = CfgInt(L"refraction", 0, 100);
    s.glassEffect = CfgInt(L"glassEffect", 0, 1) != 0;
    s.chromeBorder = CfgInt(L"chromeBorder", 0, 1) != 0;
    s.backlight = CfgInt(L"backlight", 0, 1) != 0;
    s.backlightIntensity = CfgInt(L"backlightIntensity", 5, 100);
    s.backlightCount = CfgInt(L"backlightColors", 1, 5);
    PCWSTR colorKeys[5] = {L"backlightColor1", L"backlightColor2", L"backlightColor3", L"backlightColor4", L"backlightColor5"};
    for (int i = 0; i < 5; i++) s.backlightColors[i] = ParseHex(CfgNow(colorKeys[i]), s.backlightColors[i]);
    s.tintColor = ParseHex(CfgNow(L"tintColor"), RGB(5, 5, 5));
    s.tintLevel = CfgInt(L"tintLevel", 0, 100);
    s.tintReach = CfgInt(L"tintReach", 0, 100);
    s.cornerRadius = CfgInt(L"cornerRadius", 0, 60);
    std::wstring tc = CfgNow(L"textColor");
    s.textColor = tc == L"white" ? 0 : tc == L"black" ? 1 : -1;
    s.accent = ParseHex(CfgNow(L"accentColor"), RGB(10, 132, 255));
    s.liveGlass = CfgInt(L"liveGlass", 0, 1) != 0;
    {
        std::wstring lang = CfgNow(L"language");
        static const PCWSTR codes[LANG_COUNT] = {L"pt", L"en", L"es", L"fr", L"de", L"it", L"zh"};
        int l = -1;
        for (int i = 0; i < LANG_COUNT; i++)
            if (lang == codes[i]) l = i;
        g_langAuto = l < 0;
        g_lang = l < 0 ? WindowsLang() : l;
    }
    s.showClock = CfgInt(L"showClock", 0, 1) != 0;
    {
        std::wstring cs = CfgNow(L"clockStyle");
        s.clockStyle = cs == L"analog" ? 1 : cs == L"digital" ? 2 : 0;
    }
    s.showWeather = CfgInt(L"showWeather", 0, 1) != 0;
    s.showMusic = CfgInt(L"showMusic", 0, 1) != 0;
    s.showSystem = CfgInt(L"showSystem", 0, 1) != 0;
    s.city = CfgNow(L"city");
    s.latitude = SanitizeCoord(CfgNow(L"latitude"), L"");
    s.longitude = SanitizeCoord(CfgNow(L"longitude"), L"");
    s.lockPosition = CfgInt(L"lockPosition", 0, 1) != 0;
    s.showGrid = CfgInt(L"showGrid", 0, 1) != 0;
    s.showCalendar = CfgInt(L"showCalendar", 0, 1) != 0;
    s.showWorld = CfgInt(L"showWorld", 0, 1) != 0;
    s.showSun = CfgInt(L"showSun", 0, 1) != 0;
    s.showBattery = CfgInt(L"showBattery", 0, 1) != 0;
    s.showNetwork = CfgInt(L"showNetwork", 0, 1) != 0;
    s.showPhotos = CfgInt(L"showPhotos", 0, 1) != 0;
    s.showNotif = CfgInt(L"showNotif", 0, 1) != 0;
    PCWSTR cityKeys[4] = {L"worldCity1", L"worldCity2", L"worldCity3", L"worldCity4"};
    for (int i = 0; i < 4; i++) s.worldCity[i] = CfgNow(cityKeys[i]);
    s.photosFolder = CfgNow(L"photosFolder");
    s.showNotes = CfgInt(L"showNotes", 0, 1) != 0;
    s.showReminders = CfgInt(L"showReminders", 0, 1) != 0;
    s.showShortcuts = CfgInt(L"showShortcuts", 0, 1) != 0;
    s.showStocks = CfgInt(L"showStocks", 0, 1) != 0;
    s.showNews = CfgInt(L"showNews", 0, 1) != 0;
    s.showScreenTime = CfgInt(L"showScreenTime", 0, 1) != 0;
    s.calendarIcs = CfgNow(L"calendarIcs");
    s.stocks = CfgNow(L"stocks");
    s.newsFeeds = CfgNow(L"newsFeeds");
    // The g1 feed that was the default is mostly automatic weather forecasts: the main sections instead
    if (s.newsFeeds == L"https://g1.globo.com/rss/g1/") s.newsFeeds = L"https://g1.globo.com/rss/g1/brasil/; https://g1.globo.com/rss/g1/politica/; https://g1.globo.com/rss/g1/economia/; https://g1.globo.com/rss/g1/mundo/";
    s.notesFolder = CfgNow(L"notesFolder");
    s.shortcuts = CfgNow(L"shortcuts");
    s.showHome = CfgInt(L"showHome", 0, 1) != 0;
    s.haUrl = CfgNow(L"haUrl");
    s.haToken = CfgNow(L"haToken");
    s.haEntities = CfgNow(L"haEntities");
    s.showSensors = CfgInt(L"showSensors", 0, 1) != 0;
    s.fahrenheit = CfgNow(L"tempUnit") == L"f";
    for (int i = 0; i < 8; i++) {
        std::wstring v = CfgNow((L"sensor" + std::to_wstring(i + 1)).c_str());
        s.sensorSlots[i] = 0;
        for (int k = 0; k < (int)(sizeof(kOptSensors) / sizeof(kOptSensors[0])); k++)
            if (v == kOptSensors[k].value) s.sensorSlots[i] = k;
    }
    if (s.theme == Theme::Frost) s.refraction = 0;  // Glass never bends

    std::lock_guard<std::mutex> lock(g_dataMutex);
    g_settings = s;
    g_liveGlassOn = s.liveGlass;
}

static Settings GetSettings() {
    std::lock_guard<std::mutex> lock(g_dataMutex);
    return g_settings;
}

// ---------------------------------------------------------------------------
// Shared data (filled by the data thread, read by the UI thread)
// ---------------------------------------------------------------------------

struct WeatherData {
    bool valid = false;
    double temp = 0, tmax = 0, tmin = 0;
    int code = 0;
    bool day = true;
    // The next hours (from the current one) and the next days (today first)
    int hours = 0;
    double hourTemp[6] = {};
    int hourCode[6] = {}, hourOf[6] = {};
    bool hourDay[6] = {};
    int days = 0;
    double dayMax[5] = {}, dayMin[5] = {};
    int dayCode[5] = {}, dayWeekday[5] = {};
    long long sunrise[2] = {}, sunset[2] = {};  // today and tomorrow (unix time)
    double feels = 0, humidity = 0, wind = 0, rainChance = 0, uv = 0;
    long long offset = 0;                       // the city's offset from UTC, in seconds
};

// A place found by the city search in the settings
struct Place {
    std::wstring name, region;  // "Campinas", "São Paulo, Brasil"
    std::wstring lat, lon;
};

struct Image;

struct MediaData {
    bool hasSession = false;
    bool playing = false;
    std::wstring title, artist, app;
    std::wstring aumid;  // the player's app id, to open it
    double posSec = 0, endSec = 0;
    ULONGLONG snapTick = 0;  // GetTickCount64 when posSec was taken
    std::shared_ptr<Image> art;  // the album art, if the player gives one
};

enum MediaCommand { CMD_PREV, CMD_TOGGLE, CMD_NEXT };

static WeatherData g_weather;
static MediaData g_media;
static std::deque<MediaCommand> g_commands;
static bool g_weatherRefresh = true;
static std::vector<Place> g_places;   // last city search results (UI thread)
static std::wstring g_placeStatus;    // "Procurando…", "Nada encontrado"...

static std::atomic<bool> g_running{false};
static std::condition_variable g_cv;
static std::thread g_dataThread;

static HANDLE g_uiThread = nullptr;
static DWORD g_uiThreadId = 0;
static HANDLE g_uiReady = nullptr;

#define WM_APP_RELOAD (WM_APP + 1)
#define WM_APP_REDRAW (WM_APP + 2)
#define WM_APP_PLACES (WM_APP + 3)  // a city search finished (lParam: std::vector<Place>*, or null on failure)

// ---------------------------------------------------------------------------
// Weather (Open-Meteo over WinHTTP)
// ---------------------------------------------------------------------------

// The requests in flight, so unloading can cancel them (closing a request's handle ends a call waiting on it) instead
// of waiting for each one's timeout
static std::mutex g_httpMutex;
static std::vector<HINTERNET> g_httpOpen;
static bool g_httpStopped = false;

static HINTERNET HttpTrack(HINTERNET req) {
    if (!req) return nullptr;
    std::lock_guard<std::mutex> lock(g_httpMutex);
    if (g_httpStopped) {
        WinHttpCloseHandle(req);
        return nullptr;
    }
    g_httpOpen.push_back(req);
    return req;
}

static void HttpRelease(HINTERNET req) {
    if (!req) return;
    std::lock_guard<std::mutex> lock(g_httpMutex);
    auto it = std::find(g_httpOpen.begin(), g_httpOpen.end(), req);
    if (it == g_httpOpen.end()) return;  // already closed by HttpAbortAll
    g_httpOpen.erase(it);
    WinHttpCloseHandle(req);
}

static void HttpAbortAll() {
    std::lock_guard<std::mutex> lock(g_httpMutex);
    g_httpStopped = true;
    for (HINTERNET h : g_httpOpen) WinHttpCloseHandle(h);
    g_httpOpen.clear();
}

static std::string HttpsGet(PCWSTR host, const std::wstring& path) {
    std::string out;
    HINTERNET session = WinHttpOpen(L"MacOSWidgets/0.1", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) {
        session = WinHttpOpen(L"MacOSWidgets/0.1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                              WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    }
    if (!session) return out;
    WinHttpSetTimeouts(session, 5000, 5000, 5000, 5000);
    HINTERNET conn = WinHttpConnect(session, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
    HINTERNET req = HttpTrack(conn ? WinHttpOpenRequest(conn, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
                                                        WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE)
                                   : nullptr);
    if (req && WinHttpSendRequest(req, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(req, nullptr)) {
        DWORD status = 0, size = sizeof(status);
        WinHttpQueryHeaders(req, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
                            &status, &size, WINHTTP_NO_HEADER_INDEX);
        if (status == 200) {
            DWORD avail = 0;
            while (WinHttpQueryDataAvailable(req, &avail) && avail > 0 && out.size() < 1024 * 1024) {
                std::string buf(avail, '\0');
                DWORD read = 0;
                if (!WinHttpReadData(req, &buf[0], avail, &read) || read == 0) break;
                out.append(buf.data(), read);
            }
        }
    }
    HttpRelease(req);
    if (conn) WinHttpCloseHandle(conn);
    WinHttpCloseHandle(session);
    return out;
}

// Finds `"key":` after `from` and parses the number that follows (skipping a '[').
static bool JsonNumber(const std::string& json, size_t from, const char* key, double* out) {
    std::string k = std::string("\"") + key + "\":";
    size_t p = json.find(k, from);
    if (p == std::string::npos) return false;
    p += k.size();
    while (p < json.size() && (json[p] == '[' || json[p] == ' ')) p++;
    const char* start = json.c_str() + p;
    char* end = nullptr;
    double v = strtod(start, &end);
    if (end == start) return false;
    *out = v;
    return true;
}

// The numbers of the JSON array `"key":[...]` found after `from`
static std::vector<double> JsonArray(const std::string& json, size_t from, const char* key) {
    std::vector<double> out;
    std::string k = std::string("\"") + key + "\":[";
    size_t p = json.find(k, from);
    if (p == std::string::npos) return out;
    p += k.size();
    while (p < json.size() && json[p] != ']') {
        const char* start = json.c_str() + p;
        char* end = nullptr;
        double v = strtod(start, &end);
        if (end == start) {  // null or something else: skip to the next element
            out.push_back(0);
            size_t comma = json.find_first_of(",]", p);
            if (comma == std::string::npos) break;
            p = comma;
        } else {
            out.push_back(v);
            p += end - start;
        }
        if (p < json.size() && json[p] == ',') p++;
    }
    return out;
}

static void FetchWeather() {
    Settings s = GetSettings();
    std::wstring path = L"/v1/forecast?latitude=" + s.latitude + L"&longitude=" + s.longitude +
                        L"&current=temperature_2m,weather_code,is_day,apparent_temperature,relative_humidity_2m,wind_speed_10m"
                        L"&hourly=temperature_2m,weather_code,is_day"
                        L"&daily=weather_code,temperature_2m_max,temperature_2m_min,sunrise,sunset,"
                        L"precipitation_probability_max,uv_index_max"
                        L"&timezone=auto&forecast_days=5&timeformat=unixtime";
    std::string json = HttpsGet(L"api.open-meteo.com", path);
    if (json.empty()) {
        Wh_Log(L"Weather request failed");
        return;
    }
    WeatherData w;
    size_t cur = json.find("\"current\":{");
    size_t hourly = json.find("\"hourly\":{");
    size_t daily = json.find("\"daily\":{");
    double code = 0, isDay = 1, now = 0, offset = 0;
    if (cur == std::string::npos || daily == std::string::npos || !JsonNumber(json, cur, "temperature_2m", &w.temp) ||
        !JsonNumber(json, cur, "weather_code", &code) || !JsonNumber(json, cur, "time", &now)) {
        Wh_Log(L"Weather response could not be parsed");
        return;
    }
    JsonNumber(json, cur, "is_day", &isDay);
    JsonNumber(json, 0, "utc_offset_seconds", &offset);
    w.code = (int)code;
    w.day = isDay != 0;
    if (hourly != std::string::npos) {
        auto t = JsonArray(json, hourly, "time"), temp = JsonArray(json, hourly, "temperature_2m");
        auto c = JsonArray(json, hourly, "weather_code"), d = JsonArray(json, hourly, "is_day");
        size_t n = std::min({t.size(), temp.size(), c.size(), d.size()});
        size_t first = 0;
        while (first + 1 < n && t[first + 1] <= now) first++;  // the hour we are in
        for (size_t i = first; i < n && w.hours < 6; i++, w.hours++) {
            long long local = (long long)t[i] + (long long)offset;
            w.hourTemp[w.hours] = temp[i];
            w.hourCode[w.hours] = (int)c[i];
            w.hourDay[w.hours] = d[i] != 0;
            w.hourOf[w.hours] = (int)(((local / 3600) % 24 + 24) % 24);
        }
    }
    auto dt = JsonArray(json, daily, "time"), dmax = JsonArray(json, daily, "temperature_2m_max");
    auto dmin = JsonArray(json, daily, "temperature_2m_min"), dcode = JsonArray(json, daily, "weather_code");
    size_t n = std::min({dt.size(), dmax.size(), dmin.size(), dcode.size()});
    for (size_t i = 0; i < n && w.days < 5; i++, w.days++) {
        long long local = (long long)dt[i] + (long long)offset;
        w.dayMax[w.days] = dmax[i];
        w.dayMin[w.days] = dmin[i];
        w.dayCode[w.days] = (int)dcode[i];
        w.dayWeekday[w.days] = (int)(((local / 86400) + 4) % 7);  // 1970-01-01 was a Thursday; 0 = Sunday
    }
    if (w.days > 0) {
        w.tmax = w.dayMax[0];
        w.tmin = w.dayMin[0];
    }
    auto rise = JsonArray(json, daily, "sunrise"), set = JsonArray(json, daily, "sunset");
    for (size_t i = 0; i < 2 && i < rise.size() && i < set.size(); i++) {
        w.sunrise[i] = (long long)rise[i];
        w.sunset[i] = (long long)set[i];
    }
    w.offset = (long long)offset;
    JsonNumber(json, cur, "apparent_temperature", &w.feels);
    JsonNumber(json, cur, "relative_humidity_2m", &w.humidity);
    JsonNumber(json, cur, "wind_speed_10m", &w.wind);
    auto rain = JsonArray(json, daily, "precipitation_probability_max"), uvs = JsonArray(json, daily, "uv_index_max");
    if (!rain.empty()) w.rainChance = rain[0];
    if (!uvs.empty()) w.uv = uvs[0];
    w.valid = true;
    std::lock_guard<std::mutex> lock(g_dataMutex);
    g_weather = w;
}

// ---------- City search (Open-Meteo geocoding) ----------

static std::wstring FromUtf8(const std::string& s) {
    if (s.empty()) return L"";
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), &w[0], n);
    return w;
}

static std::wstring UrlEncode(const std::wstring& text) {
    int n = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), nullptr, 0, nullptr, nullptr);
    std::string u(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), &u[0], n, nullptr, nullptr);
    std::wstring out;
    wchar_t buf[8];
    for (unsigned char c : u) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') out += (wchar_t)c;
        else {
            swprintf(buf, 8, L"%%%02X", c);
            out += buf;
        }
    }
    return out;
}

// The text of `"key":"..."` inside [from, to)
static std::wstring JsonString(const std::string& json, size_t from, size_t to, const char* key) {
    std::string k = std::string("\"") + key + "\":\"";
    size_t p = json.find(k, from);
    if (p == std::string::npos || p >= to) return L"";
    p += k.size();
    std::string out;
    while (p < json.size() && json[p] != '"') {
        if (json[p] == '\\' && p + 1 < json.size()) {
            char e = json[p + 1];
            if (e == 'u' && p + 5 < json.size()) {
                unsigned v = (unsigned)strtoul(json.substr(p + 2, 4).c_str(), nullptr, 16);
                wchar_t wc = (wchar_t)v;
                int n = WideCharToMultiByte(CP_UTF8, 0, &wc, 1, nullptr, 0, nullptr, nullptr);
                std::string b(n, '\0');
                WideCharToMultiByte(CP_UTF8, 0, &wc, 1, &b[0], n, nullptr, nullptr);
                out += b;
                p += 6;
                continue;
            }
            out += e == 'n' ? ' ' : e;
            p += 2;
            continue;
        }
        out += json[p++];
    }
    return FromUtf8(out);
}

static std::wstring CoordText(double v) {
    wchar_t buf[32];
    swprintf(buf, 32, L"%.4f", v);
    return buf;
}

// Runs on its own thread; the result goes to the UI thread
static void SearchPlaces(std::wstring query) {
    auto* found = new std::vector<Place>();
    std::string json = HttpsGet(L"geocoding-api.open-meteo.com",
                                L"/v1/search?name=" + UrlEncode(query) + L"&count=6&language=" + std::wstring(LangCode()) + L"&format=json");
    bool ok = !json.empty();
    size_t p = json.find("\"results\":[");
    while (p != std::string::npos) {
        size_t start = json.find('{', p);
        if (start == std::string::npos) break;
        size_t end = json.find('}', start);
        if (end == std::string::npos) break;
        Place pl;
        double lat = 0, lon = 0;
        pl.name = JsonString(json, start, end, "name");
        std::wstring admin = JsonString(json, start, end, "admin1"), country = JsonString(json, start, end, "country");
        if (!pl.name.empty() && JsonNumber(json, start, "latitude", &lat) && JsonNumber(json, start, "longitude", &lon)) {
            pl.region = admin.empty() ? country : country.empty() ? admin : admin + L", " + country;
            pl.lat = CoordText(lat);
            pl.lon = CoordText(lon);
            found->push_back(pl);
        }
        p = end + 1;
        if (p >= json.size() || json[p] == ']') break;
    }
    if (!ok) {
        delete found;
        found = nullptr;
    }
    if (!g_uiThreadId || !PostThreadMessageW(g_uiThreadId, WM_APP_PLACES, 0, (LPARAM)found)) delete found;
}

static std::wstring WeatherText(int code) {
    if (code == 0) return L"Céu limpo";
    if (code == 1) return L"Predominantemente limpo";
    if (code == 2) return L"Parcialmente nublado";
    if (code == 3) return L"Nublado";
    if (code == 45 || code == 48) return L"Neblina";
    if (code >= 51 && code <= 57) return L"Garoa";
    if (code >= 61 && code <= 67) return L"Chuva";
    if (code >= 71 && code <= 77) return L"Neve";
    if (code >= 80 && code <= 82) return L"Pancadas de chuva";
    if (code >= 85 && code <= 86) return L"Pancadas de neve";
    if (code >= 95) return L"Trovoadas";
    return L"—";
}

// ---------------------------------------------------------------------------
// Pictures (album art, photos): decoded with WIC on the data thread into premultiplied BGRA pixels
// ---------------------------------------------------------------------------

struct Image {
    int w = 0, h = 0;
    std::vector<UINT32> px;
};

constexpr GUID kClsidWicFactory = {0xcacaf262, 0x9370, 0x4615, {0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a}};
constexpr GUID kWicFormatPBGRA = {0x6fddc324, 0x4e03, 0x4bfe, {0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10}};

static ComPtr<IWICImagingFactory> WicFactory() {
    ComPtr<IWICImagingFactory> f;
    CoCreateInstance(kClsidWicFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)f.GetAddressOf());
    return f;
}

// The first frame, turned upright (EXIF orientation), scaled to fit `maxSide`
static std::shared_ptr<Image> DecodeFrom(IWICImagingFactory* f, IWICBitmapDecoder* dec, UINT maxSide) {
    ComPtr<IWICBitmapFrameDecode> frame;
    if (FAILED(dec->GetFrame(0, &frame))) return nullptr;
    ComPtr<IWICBitmapSource> src = frame;
    UINT w = 0, h = 0;
    frame->GetSize(&w, &h);
    if (!w || !h) return nullptr;
    if (std::max(w, h) > maxSide) {
        double k = (double)maxSide / std::max(w, h);
        UINT nw = std::max(1u, (UINT)(w * k)), nh = std::max(1u, (UINT)(h * k));
        ComPtr<IWICBitmapScaler> scaler;
        if (SUCCEEDED(f->CreateBitmapScaler(&scaler)) &&
            SUCCEEDED(scaler->Initialize(src.Get(), nw, nh, WICBitmapInterpolationModeFant)))
            src = scaler;
    }
    ComPtr<IWICMetadataQueryReader> meta;
    if (SUCCEEDED(frame->GetMetadataQueryReader(&meta))) {
        PROPVARIANT pv;
        PropVariantInit(&pv);
        WICBitmapTransformOptions t = WICBitmapTransformRotate0;
        if (SUCCEEDED(meta->GetMetadataByName(L"/app1/ifd/{ushort=274}", &pv)) && pv.vt == VT_UI2) {
            if (pv.uiVal == 3) t = WICBitmapTransformRotate180;
            else if (pv.uiVal == 6) t = WICBitmapTransformRotate90;
            else if (pv.uiVal == 8) t = WICBitmapTransformRotate270;
        }
        PropVariantClear(&pv);
        ComPtr<IWICBitmapFlipRotator> rot;
        if (t != WICBitmapTransformRotate0 && SUCCEEDED(f->CreateBitmapFlipRotator(&rot)) &&
            SUCCEEDED(rot->Initialize(src.Get(), t)))
            src = rot;
    }
    ComPtr<IWICFormatConverter> conv;
    if (FAILED(f->CreateFormatConverter(&conv)) ||
        FAILED(conv->Initialize(src.Get(), kWicFormatPBGRA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom)))
        return nullptr;
    auto img = std::make_shared<Image>();
    UINT iw = 0, ih = 0;
    conv->GetSize(&iw, &ih);
    img->w = (int)iw;
    img->h = (int)ih;
    img->px.resize((size_t)iw * ih);
    if (FAILED(conv->CopyPixels(nullptr, iw * 4, iw * ih * 4, (BYTE*)img->px.data()))) return nullptr;
    return img;
}

[[maybe_unused]] static std::shared_ptr<Image> DecodeBytes(const std::vector<uint8_t>& bytes, UINT maxSide) {
    auto f = WicFactory();
    ComPtr<IWICStream> stream;
    ComPtr<IWICBitmapDecoder> dec;
    if (!f || bytes.empty() || FAILED(f->CreateStream(&stream)) ||
        FAILED(stream->InitializeFromMemory((BYTE*)bytes.data(), (DWORD)bytes.size())) ||
        FAILED(f->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, &dec)))
        return nullptr;
    return DecodeFrom(f.Get(), dec.Get(), maxSide);
}

static std::shared_ptr<Image> DecodeFile(const std::wstring& path, UINT maxSide) {
    auto f = WicFactory();
    ComPtr<IWICBitmapDecoder> dec;
    if (!f || FAILED(f->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &dec)))
        return nullptr;
    return DecodeFrom(f.Get(), dec.Get(), maxSide);
}

// ---------------------------------------------------------------------------
// Media (Windows media session API)
// ---------------------------------------------------------------------------

static std::wstring AppNameFromAumid(const std::wstring& aumid) {
    std::wstring n = aumid;
    size_t bang = n.find(L'!');
    if (bang != std::wstring::npos) n = n.substr(bang + 1);
    size_t slash = n.find_last_of(L"\\/");
    if (slash != std::wstring::npos) n = n.substr(slash + 1);
    if (n.size() > 4 && _wcsicmp(n.c_str() + n.size() - 4, L".exe") == 0) n = n.substr(0, n.size() - 4);
    if (!n.empty()) n[0] = towupper(n[0]);
    return n;
}

#ifndef WW_NO_WINRT
using namespace winrt::Windows::Media::Control;

static void PollMedia(GlobalSystemMediaTransportControlsSessionManager& mgr) {
    MediaData m;
    try {
        if (!mgr) mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        auto session = mgr.GetCurrentSession();
        if (session) {
            // Run queued button presses first, so the poll below shows their result.
            std::deque<MediaCommand> cmds;
            {
                std::lock_guard<std::mutex> lock(g_dataMutex);
                cmds.swap(g_commands);
            }
            for (MediaCommand c : cmds) {
                if (c == CMD_PREV) session.TrySkipPreviousAsync().get();
                else if (c == CMD_NEXT) session.TrySkipNextAsync().get();
                else session.TryTogglePlayPauseAsync().get();
            }

            auto props = session.TryGetMediaPropertiesAsync().get();
            auto playback = session.GetPlaybackInfo();
            auto timeline = session.GetTimelineProperties();
            m.hasSession = true;
            m.title = props.Title().c_str();
            m.artist = props.Artist().c_str();
            m.aumid = session.SourceAppUserModelId().c_str();
            m.app = AppNameFromAumid(m.aumid.c_str());
            m.playing = playback.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
            using secs = std::chrono::duration<double>;
            m.posSec = std::chrono::duration_cast<secs>(timeline.Position()).count();
            m.endSec = std::chrono::duration_cast<secs>(timeline.EndTime()).count();
            if (m.playing) {
                auto since = winrt::clock::now() - timeline.LastUpdatedTime();
                double add = std::chrono::duration_cast<secs>(since).count();
                if (add > 0 && add < 24 * 3600) m.posSec += add;
            }
            m.snapTick = GetTickCount64();
            // The album art: read again only when the song changes
            static std::wstring lastKey;
            static std::shared_ptr<Image> lastArt;
            std::wstring key = m.app + L"\n" + m.title + L"\n" + m.artist;
            if (key != lastKey) {
                lastKey = key;
                lastArt = nullptr;
                try {
                    if (auto thumb = props.Thumbnail()) {
                        using namespace winrt::Windows::Storage::Streams;
                        auto stream = thumb.OpenReadAsync().get();
                        uint32_t size = (uint32_t)std::min<uint64_t>(stream.Size(), 16 * 1024 * 1024);
                        Buffer buf(size);
                        auto got = stream.ReadAsync(buf, size, InputStreamOptions::None).get();
                        std::vector<uint8_t> bytes(got.Length());
                        DataReader::FromBuffer(got).ReadBytes(bytes);
                        lastArt = DecodeBytes(bytes, 512);
                    }
                } catch (...) {
                    lastArt = nullptr;  // some players give no picture
                }
            }
            m.art = lastArt;
        } else {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            g_commands.clear();
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Media error 0x%08X", (unsigned)(int32_t)e.code());
        mgr = nullptr;
    } catch (...) {
        mgr = nullptr;
    }
    std::lock_guard<std::mutex> lock(g_dataMutex);
    g_media = m;
    if (!m.hasSession) g_commands.clear();  // presses that couldn't reach a player don't keep the thread awake
}
#endif

// ---------------------------------------------------------------------------
// Batteries: this PC's, and Bluetooth devices that report theirs to Windows
// ---------------------------------------------------------------------------

struct BatteryInfo {
    std::wstring name;
    int level = 0;  // %
    bool charging = false, pc = false;
};
static std::vector<BatteryInfo> g_batteries;

static void ReadBatteries() {
    std::vector<BatteryInfo> out;
    SYSTEM_POWER_STATUS ps;
    if (GetSystemPowerStatus(&ps) && !(ps.BatteryFlag & 128) && ps.BatteryLifePercent <= 100) {
        BatteryInfo b;
        b.name = L"Este PC";
        b.level = ps.BatteryLifePercent;
        b.charging = ps.ACLineStatus == 1;
        b.pc = true;
        out.push_back(b);
    }
    // The battery level Windows keeps for Bluetooth devices (the same one Settings > Bluetooth shows)
    static const DEVPROPKEY kLevel = {{0x104EA319, 0x6EE2, 0x4701, {0xBD, 0x47, 0x8D, 0xDB, 0xF4, 0x25, 0xBB, 0xE5}}, 2};
    static const DEVPROPKEY kName = {{0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}}, 14};
    HDEVINFO set = SetupDiGetClassDevsW(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (set != INVALID_HANDLE_VALUE) {
        SP_DEVINFO_DATA d = {sizeof(d)};
        for (DWORD i = 0; SetupDiEnumDeviceInfo(set, i, &d); i++) {
            DEVPROPTYPE type = 0;
            BYTE level = 0;
            if (!SetupDiGetDevicePropertyW(set, &d, &kLevel, &type, &level, sizeof(level), nullptr, 0) || type != DEVPROP_TYPE_BYTE)
                continue;
            wchar_t name[256] = L"";
            if (!SetupDiGetDevicePropertyW(set, &d, &kName, &type, (PBYTE)name, sizeof(name) - 2, nullptr, 0)) continue;
            bool seen = false;
            for (auto& b : out)
                if (b.name == name) seen = true;
            if (seen || level > 100) continue;
            BatteryInfo b;
            b.name = name;
            b.level = level;
            out.push_back(b);
        }
        SetupDiDestroyDeviceInfoList(set);
    }
    std::lock_guard<std::mutex> lock(g_dataMutex);
    g_batteries = out;
}

// ---------------------------------------------------------------------------
// Notifications: Windows keeps them in a SQLite database of the user; it is read (never written) with the
// SQLite that comes with Windows (winsqlite3.dll)
// ---------------------------------------------------------------------------

struct NotifInfo {
    std::wstring app, title, body;
    long long arrival = 0;  // FILETIME
};
static std::vector<NotifInfo> g_notifs;
static std::wstring g_notifError;
static std::atomic<long long> g_notifCleared{-1};  // "Limpar": hides what arrived up to then (-1 = not loaded yet)

typedef int(WINAPI* sq_open_t)(const char*, void**, int, const char*);
typedef int(WINAPI* sq_prepare_t)(void*, const char*, int, void**, const char**);
typedef int(WINAPI* sq_step_t)(void*);
typedef const void*(WINAPI* sq_blob_t)(void*, int);
typedef int(WINAPI* sq_bytes_t)(void*, int);
typedef long long(WINAPI* sq_int64_t)(void*, int);
typedef const unsigned char*(WINAPI* sq_text_t)(void*, int);
typedef int(WINAPI* sq_finalize_t)(void*);
typedef int(WINAPI* sq_close_t)(void*);

// One pass, left to right: "&amp;lt;" becomes "&lt;" (not "<"), and numeric ones (&#8217; &#x2019;) are decoded too
static std::wstring XmlDecode(const std::wstring& s) {
    static const std::pair<PCWSTR, wchar_t> named[] = {{L"lt", L'<'}, {L"gt", L'>'}, {L"quot", L'"'}, {L"apos", L'\''}, {L"amp", L'&'}};
    std::wstring out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); i++) {
        size_t semi = s[i] == L'&' ? s.find(L';', i + 1) : std::wstring::npos;
        if (semi == std::wstring::npos || semi - i > 10) {
            out += s[i];
            continue;
        }
        std::wstring name = s.substr(i + 1, semi - i - 1);
        bool done = false;
        if (name.size() > 1 && name[0] == L'#') {
            bool hex = name[1] == L'x' || name[1] == L'X';
            unsigned long cp = wcstoul(name.c_str() + (hex ? 2 : 1), nullptr, hex ? 16 : 10);
            if (cp > 0 && cp <= 0x10FFFF && !(cp >= 0xD800 && cp <= 0xDFFF)) {
                if (cp >= 0x10000) {
                    cp -= 0x10000;
                    out += (wchar_t)(0xD800 + (cp >> 10));
                    out += (wchar_t)(0xDC00 + (cp & 0x3FF));
                } else {
                    out += (wchar_t)cp;
                }
                done = true;
            }
        } else {
            for (const auto& n : named)
                if (name == n.first) {
                    out += n.second;
                    done = true;
                    break;
                }
        }
        if (done) i = semi;
        else out += s[i];
    }
    return out;
}

// The <text> lines of a toast
static std::vector<std::wstring> ToastTexts(const std::wstring& xml) {
    std::vector<std::wstring> out;
    size_t p = 0;
    while ((p = xml.find(L"<text", p)) != std::wstring::npos) {
        size_t gt = xml.find(L'>', p);
        if (gt == std::wstring::npos) break;
        if (xml[gt - 1] == L'/') {  // <text/>
            p = gt;
            continue;
        }
        size_t end = xml.find(L"</text>", gt);
        if (end == std::wstring::npos) break;
        std::wstring t = XmlDecode(xml.substr(gt + 1, end - gt - 1));
        if (!t.empty()) out.push_back(t);
        p = end + 7;
    }
    return out;
}

static std::wstring AppFromHandler(const std::wstring& id) {
    struct Known {
        PCWSTR part, name;
    } known[] = {{L"WhatsApp", L"WhatsApp"},  {L"Instagram", L"Instagram"}, {L"Facebook", L"Facebook"},
                 {L"OUTLOOK", L"Outlook"},     {L"Outlook", L"Outlook"},     {L"Teams", L"Teams"},
                 {L"Telegram", L"Telegram"},   {L"Discord", L"Discord"},     {L"Chrome", L"Chrome"},
                 {L"MSEdge", L"Edge"},         {L"Spotify", L"Spotify"},     {L"YourPhone", L"Vincular ao Celular"},
                 {L"OneDrive", L"OneDrive"},   {L"Windows.Defender", L"Segurança do Windows"},
                 {L"windows.immersivecontrolpanel", L"Configurações"}};
    for (auto& k : known)
        if (id.find(k.part) != std::wstring::npos) return k.name;
    return AppNameFromAumid(id);
}

static void ReadNotifications() {
    static HMODULE lib = LoadLibraryExW(L"winsqlite3.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!lib) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        g_notifError = L"O Windows não tem o leitor de notificações";
        return;
    }
    auto open = (sq_open_t)(void*)GetProcAddress(lib, "sqlite3_open_v2");
    auto prepare = (sq_prepare_t)(void*)GetProcAddress(lib, "sqlite3_prepare_v2");
    auto step = (sq_step_t)(void*)GetProcAddress(lib, "sqlite3_step");
    auto blob = (sq_blob_t)(void*)GetProcAddress(lib, "sqlite3_column_blob");
    auto bytes = (sq_bytes_t)(void*)GetProcAddress(lib, "sqlite3_column_bytes");
    auto int64 = (sq_int64_t)(void*)GetProcAddress(lib, "sqlite3_column_int64");
    auto text = (sq_text_t)(void*)GetProcAddress(lib, "sqlite3_column_text");
    auto finalize = (sq_finalize_t)(void*)GetProcAddress(lib, "sqlite3_finalize");
    auto close = (sq_close_t)(void*)GetProcAddress(lib, "sqlite3_close");
    if (!open || !prepare || !step || !blob || !bytes || !int64 || !text || !finalize || !close) return;

    if (g_notifCleared < 0) {
        std::wstring v = ReadValue(L"notifCleared");
        g_notifCleared = v.empty() ? 0 : _wtoi64(v.c_str());
    }
    wchar_t local[MAX_PATH] = L"";
    GetEnvironmentVariableW(L"LOCALAPPDATA", local, MAX_PATH);
    std::wstring path = std::wstring(local) + L"\\Microsoft\\Windows\\Notifications\\wpndatabase.db";
    int n = WideCharToMultiByte(CP_UTF8, 0, path.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string u(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, path.c_str(), -1, &u[0], n, nullptr, nullptr);

    std::vector<NotifInfo> out;
    std::wstring error;
    void* db = nullptr;
    if (open(u.c_str(), &db, 0x00000001 /* SQLITE_OPEN_READONLY */, nullptr) != 0) {
        error = L"Não deu para ler as notificações do Windows";
    } else {
        void* st = nullptr;
        const char* sql =
            "SELECT n.Payload, n.ArrivalTime, h.PrimaryId FROM Notification n JOIN NotificationHandler h "
            "ON n.HandlerId = h.RecordId WHERE n.Type = 'toast' ORDER BY n.ArrivalTime DESC LIMIT 30";
        if (prepare(db, sql, -1, &st, nullptr) == 0 && st) {
            while (step(st) == 100 /* SQLITE_ROW */) {
                const char* p = (const char*)blob(st, 0);
                int len = bytes(st, 0);
                NotifInfo ni;
                ni.arrival = int64(st, 1);
                if (ni.arrival <= g_notifCleared) continue;
                const unsigned char* id = text(st, 2);
                ni.app = AppFromHandler(id ? FromUtf8((const char*)id) : L"");
                auto lines = ToastTexts(p && len > 0 ? FromUtf8(std::string(p, len)) : L"");
                if (lines.empty()) continue;
                ni.title = lines[0];
                for (size_t i = 1; i < lines.size(); i++) ni.body += (i > 1 ? L" " : L"") + lines[i];
                out.push_back(ni);
            }
            finalize(st);
        } else {
            error = L"Não deu para ler as notificações do Windows";
        }
        close(db);
    }
    std::lock_guard<std::mutex> lock(g_dataMutex);
    g_notifs = out;
    g_notifError = error;
}

// ---------------------------------------------------------------------------
// Photos: a picture from the OneDrive Pictures folder (or the one chosen), changed every 20 minutes
// ---------------------------------------------------------------------------

struct PhotoInfo {
    std::shared_ptr<Image> img;
    std::wstring path, caption;
};
static PhotoInfo g_photo;
static bool g_photoNext = false;  // a new one now (settings changed)

static std::wstring PhotosFolder() {
    std::wstring chosen = GetSettings().photosFolder;
    if (!chosen.empty() && GetFileAttributesW(chosen.c_str()) != INVALID_FILE_ATTRIBUTES) return chosen;
    wchar_t od[MAX_PATH] = L"";
    if (GetEnvironmentVariableW(L"OneDrive", od, MAX_PATH)) {
        for (PCWSTR sub : {L"\\Pictures", L"\\Imagens", L"\\Fotos"}) {
            std::wstring p = std::wstring(od) + sub;
            if (GetFileAttributesW(p.c_str()) != INVALID_FILE_ATTRIBUTES) return p;
        }
    }
    PWSTR pics = nullptr;
    std::wstring out;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Pictures, 0, nullptr, &pics))) out = pics;
    CoTaskMemFree(pics);
    return out;
}

static void ListPhotos(const std::wstring& dir, int depth, std::vector<std::wstring>& out) {
    if (out.size() >= 3000) return;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileExW((dir + L"\\*").c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, 0);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        std::wstring name = fd.cFileName;
        if (name == L"." || name == L"..") continue;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (depth < 3 && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) ListPhotos(dir + L"\\" + name, depth + 1, out);
            continue;
        }
        // Files kept only in the cloud would be downloaded just to be shown: skipped
        if (fd.dwFileAttributes & (FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS | FILE_ATTRIBUTE_OFFLINE)) continue;
        size_t dot = name.find_last_of(L'.');
        if (dot == std::wstring::npos) continue;
        std::wstring ext = name.substr(dot);
        for (auto& c : ext) c = towlower(c);
        if (ext == L".jpg" || ext == L".jpeg" || ext == L".png" || ext == L".bmp" || ext == L".heic" || ext == L".webp")
            out.push_back(dir + L"\\" + name);
    } while (FindNextFileW(h, &fd) && out.size() < 3000);
    FindClose(h);
}

static void PickPhoto() {
    std::vector<std::wstring> files;
    std::wstring dir = PhotosFolder();
    if (!dir.empty()) ListPhotos(dir, 0, files);
    PhotoInfo p;
    for (int tries = 0; tries < 6 && !files.empty() && !p.img; tries++) {
        p.path = files[(size_t)((GetTickCount64() / 7 + tries * 7919) % files.size())];
        p.img = DecodeFile(p.path, 900);
    }
    if (p.img) {
        WIN32_FILE_ATTRIBUTE_DATA a;
        SYSTEMTIME st;
        FILETIME lt;
        if (GetFileAttributesExW(p.path.c_str(), GetFileExInfoStandard, &a) && FileTimeToLocalFileTime(&a.ftLastWriteTime, &lt) &&
            FileTimeToSystemTime(&lt, &st)) {
            wchar_t buf[64] = L"";
            GetDateFormatEx(LangLocale(), 0, &st, MonthYearFormat(), buf, 64, nullptr);
            if (buf[0]) buf[0] = towupper(buf[0]);
            p.caption = buf;
        }
    }
    std::lock_guard<std::mutex> lock(g_dataMutex);
    g_photo = p;
}

// ---------------------------------------------------------------------------
// Any web address (calendar links, news feeds, quotes)
// ---------------------------------------------------------------------------

static std::string HttpGetUrl(const std::wstring& url, PCWSTR agent = L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) MacOSWidgets") {
    std::string out;
    std::wstring u = url;
    while (!u.empty() && u.back() == L' ') u.pop_back();
    while (!u.empty() && u.front() == L' ') u.erase(0, 1);
    if (u.rfind(L"webcal://", 0) == 0) u = L"https://" + u.substr(9);
    URL_COMPONENTSW uc = {sizeof(uc)};
    wchar_t host[256] = L"", path[4096] = L"";
    uc.lpszHostName = host;
    uc.dwHostNameLength = 256;
    uc.lpszUrlPath = path;
    uc.dwUrlPathLength = 2048;
    wchar_t extra[2048] = L"";
    uc.lpszExtraInfo = extra;
    uc.dwExtraInfoLength = 2048;
    if (!WinHttpCrackUrl(u.c_str(), 0, 0, &uc)) return out;
    std::wstring full = std::wstring(path) + extra;
    bool secure = uc.nScheme == INTERNET_SCHEME_HTTPS;
    HINTERNET session = WinHttpOpen(agent, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) session = WinHttpOpen(agent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) return out;
    WinHttpSetTimeouts(session, 6000, 6000, 8000, 8000);
    HINTERNET conn = WinHttpConnect(session, host, uc.nPort, 0);
    HINTERNET req = HttpTrack(conn ? WinHttpOpenRequest(conn, L"GET", full.c_str(), nullptr, WINHTTP_NO_REFERER,
                                                        WINHTTP_DEFAULT_ACCEPT_TYPES, secure ? WINHTTP_FLAG_SECURE : 0)
                                   : nullptr);
    // Always the current version, not a copy kept by a proxy or the site's cache
    PCWSTR fresh = L"Cache-Control: no-cache\r\nPragma: no-cache\r\n";
    if (req && WinHttpSendRequest(req, fresh, (DWORD)-1L, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) && WinHttpReceiveResponse(req, nullptr)) {
        DWORD status = 0, size = sizeof(status);
        WinHttpQueryHeaders(req, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status, &size,
                            WINHTTP_NO_HEADER_INDEX);
        if (status == 200) {
            DWORD avail = 0;
            while (WinHttpQueryDataAvailable(req, &avail) && avail > 0 && out.size() < 8 * 1024 * 1024) {
                std::string buf(avail, '\0');
                DWORD read = 0;
                if (!WinHttpReadData(req, &buf[0], avail, &read) || read == 0) break;
                out.append(buf.data(), read);
            }
        }
    }
    HttpRelease(req);
    if (conn) WinHttpCloseHandle(conn);
    WinHttpCloseHandle(session);
    return out;
}

static std::vector<std::wstring> SplitList(const std::wstring& s, PCWSTR seps) {
    std::vector<std::wstring> out;
    std::wstring cur;
    auto push = [&]() {
        while (!cur.empty() && iswspace(cur.back())) cur.pop_back();
        size_t a = 0;
        while (a < cur.size() && iswspace(cur[a])) a++;
        if (a < cur.size()) out.push_back(cur.substr(a));
        cur.clear();
    };
    for (wchar_t c : s) {
        if (wcschr(seps, c)) push();
        else cur += c;
    }
    push();
    return out;
}

// ---------------------------------------------------------------------------
// Calendar events, from the calendar's private iCal link (Google Calendar, Outlook...)
// ---------------------------------------------------------------------------

struct CalEvent {
    long long start = 0, end = 0;  // local time, minutes (as MinutesOf)
    bool allDay = false;
    std::wstring title;
};
static std::vector<CalEvent> g_events;
static std::wstring g_eventsError;
static bool g_eventsRefresh = false;
static bool g_newsRefresh = false;  // "Atualizar Notícias"
static bool g_coversWanted = false;  // news covers still to download (by CoverThread)
static std::thread g_coverThread;
static std::thread g_netThread;

static int DaysInMonth(int year, int month) {
    static const int d[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    return month == 2 && leap ? 29 : d[(month - 1) % 12];
}

static long long LocalMinutes(const SYSTEMTIME& s) {
    FILETIME ft;
    if (!SystemTimeToFileTime(&s, &ft)) return 0;
    return (long long)((((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 600000000ULL);
}
static SYSTEMTIME FromMinutes(long long m) {
    ULONGLONG v = (ULONGLONG)m * 600000000ULL;
    FILETIME ft = {(DWORD)v, (DWORD)(v >> 32)};
    SYSTEMTIME s = {};
    FileTimeToSystemTime(&ft, &s);
    return s;
}
static long long NowLocalMinutes() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    return LocalMinutes(st);
}

// "20261125" (all day), "20261125T140000" (local) or "20261125T170000Z" (UTC)
static bool ParseIcsTime(const std::wstring& v, long long* out, bool* allDay) {
    if (v.size() < 8) return false;
    SYSTEMTIME s = {};
    s.wYear = (WORD)_wtoi(v.substr(0, 4).c_str());
    s.wMonth = (WORD)_wtoi(v.substr(4, 2).c_str());
    s.wDay = (WORD)_wtoi(v.substr(6, 2).c_str());
    *allDay = v.size() < 15;
    if (!*allDay) {
        s.wHour = (WORD)_wtoi(v.substr(9, 2).c_str());
        s.wMinute = (WORD)_wtoi(v.substr(11, 2).c_str());
        if (v.size() >= 16 && v[15] == L'Z') {
            SYSTEMTIME local;
            if (SystemTimeToTzSpecificLocalTime(nullptr, &s, &local)) s = local;
        }
        // A TZID other than the PC's is taken as the PC's time (the usual case: your own calendar)
    }
    *out = LocalMinutes(s);
    return *out != 0;
}

static std::wstring IcsUnescape(std::wstring s) {
    std::wstring o;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == L'\\' && i + 1 < s.size()) {
            wchar_t n = s[++i];
            o += (n == L'n' || n == L'N') ? L' ' : n;
        } else {
            o += s[i];
        }
    }
    return o;
}

// The occurrences of one event inside [from, to)
static void ExpandEvent(const CalEvent& base, const std::wstring& rrule, const std::vector<long long>& exdates, long long from,
                        long long to, std::vector<CalEvent>& out) {
    auto add = [&](long long start) {
        for (long long x : exdates)
            if (x == start) return;
        CalEvent e = base;
        e.end = start + (base.end - base.start);
        e.start = start;
        if (e.end > from && e.start < to) out.push_back(e);
    };
    if (rrule.empty()) {
        add(base.start);
        return;
    }
    std::wstring freq;
    int interval = 1, count = -1;
    long long until = LLONG_MAX;
    std::vector<int> byday;  // 0 = Sunday
    for (const auto& part : SplitList(rrule, L";")) {
        size_t eq = part.find(L'=');
        if (eq == std::wstring::npos) continue;
        std::wstring k = part.substr(0, eq), v = part.substr(eq + 1);
        if (k == L"FREQ") freq = v;
        else if (k == L"INTERVAL") interval = std::clamp(_wtoi(v.c_str()), 1, 1000);  // the date math stays in range
        else if (k == L"COUNT") count = _wtoi(v.c_str());
        else if (k == L"UNTIL") {
            bool ad = false;
            ParseIcsTime(v, &until, &ad);
            if (ad) until += 1439;
        } else if (k == L"BYDAY") {
            static PCWSTR names[7] = {L"SU", L"MO", L"TU", L"WE", L"TH", L"FR", L"SA"};
            for (const auto& d : SplitList(v, L",")) {
                std::wstring dd = d.size() >= 2 ? d.substr(d.size() - 2) : d;
                for (int i = 0; i < 7; i++)
                    if (dd == names[i]) byday.push_back(i);
            }
        }
    }
    int made = 0;
    SYSTEMTIME st = FromMinutes(base.start);
    long long minuteOfDay = base.start % 1440;
    // Old daily/weekly series: start near the window instead of walking from their first day
    int firstStep = 0;
    if (count < 0 && (freq == L"DAILY" || freq == L"WEEKLY") && from > base.start) {
        long long period = (long long)interval * (freq == L"DAILY" ? 1440 : 7 * 1440);
        firstStep = (int)std::max(0LL, (from - base.start) / period - 1);
    }
    for (int step = firstStep; step < firstStep + 4000; step++) {
        std::vector<long long> starts;
        if (freq == L"DAILY") {
            starts.push_back(base.start + (long long)step * interval * 1440);
        } else if (freq == L"WEEKLY") {
            long long weekStart = base.start - (long long)st.wDayOfWeek * 1440 + (long long)step * interval * 7 * 1440;
            if (byday.empty()) starts.push_back(base.start + (long long)step * interval * 7 * 1440);
            else
                for (int d : byday) starts.push_back(weekStart - weekStart % 1440 + d * 1440LL + minuteOfDay);
            std::sort(starts.begin(), starts.end());
        } else if (freq == L"MONTHLY" || freq == L"YEARLY") {
            SYSTEMTIME s = st;
            int months = freq == L"MONTHLY" ? step * interval : step * interval * 12;
            int m = (st.wMonth - 1) + months;
            s.wYear = (WORD)(st.wYear + m / 12);
            s.wMonth = (WORD)(m % 12 + 1);
            if (s.wDay <= DaysInMonth(s.wYear, s.wMonth)) starts.push_back(LocalMinutes(s));
        } else {
            add(base.start);  // a rule this doesn't know: just the first one
            return;
        }
        bool past = false;
        for (long long s : starts) {
            if (s < base.start) continue;
            if (s > until || s >= to || (count >= 0 && made >= count)) {
                past = true;
                break;
            }
            made++;
            add(s);
        }
        if (past) return;
    }
}

static std::wstring Lower(std::wstring s);
static std::wstring ReadTextFile(const std::wstring& path, size_t maxBytes);

static void FetchCalendar() {
    std::wstring link = GetSettings().calendarIcs;
    std::vector<CalEvent> out;
    std::wstring error;
    if (link.empty()) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        g_events.clear();
        g_eventsError.clear();
        return;
    }
    // Links and .ics files, separated by ";": all their events together
    std::wstring text;
    int sources = 0, failed = 0;
    for (const auto& src : SplitList(link, L";\n")) {
        sources++;
        std::wstring got;
        std::wstring low = Lower(src);
        if (low.rfind(L"http", 0) == 0 || low.rfind(L"webcal", 0) == 0) {
            got = FromUtf8(HttpGetUrl(src));
        } else {
            std::wstring path = src;
            if (low.rfind(L"file:///", 0) == 0) path = path.substr(8);
            if (path.size() > 1 && path.front() == L'"' && path.back() == L'"') path = path.substr(1, path.size() - 2);
            got = ReadTextFile(path, 16 * 1024 * 1024);
        }
        if (got.find(L"BEGIN:VCALENDAR") == std::wstring::npos) failed++;
        else text += got + L"\n";
    }
    if (text.empty()) {
        error = sources > 1 ? L"Não deu para ler os calendários (confira os links e arquivos)"
                            : L"Não deu para ler o calendário (confira o link ou o arquivo)";
    } else {
        if (failed) error = L"Um dos calendários não abriu";
        // Unfold: a line starting with a space continues the one before it
        std::vector<std::wstring> lines;
        for (auto& l : Split(text, L'\n')) {
            if (!l.empty() && l.back() == L'\r') l.pop_back();
            if (!l.empty() && (l[0] == L' ' || l[0] == L'\t') && !lines.empty()) lines.back() += l.substr(1);
            else lines.push_back(l);
        }
        long long now = NowLocalMinutes(), from = now - now % 1440, to = from + 31 * 1440;
        bool in = false, cancelled = false;
        CalEvent ev;
        std::wstring rrule;
        std::vector<long long> exdates;
        for (const auto& l : lines) {
            if (l == L"BEGIN:VEVENT") {
                in = true;
                cancelled = false;
                ev = CalEvent();
                rrule.clear();
                exdates.clear();
                continue;
            }
            if (!in) continue;
            if (l == L"END:VEVENT") {
                in = false;
                if (!cancelled && ev.start) {
                    if (!ev.end) ev.end = ev.start + (ev.allDay ? 1440 : 60);
                    ExpandEvent(ev, rrule, exdates, from, to, out);
                }
                continue;
            }
            size_t colon = l.find(L':');
            if (colon == std::wstring::npos) continue;
            std::wstring name = l.substr(0, colon), value = l.substr(colon + 1);
            std::wstring key = name.substr(0, name.find(L';'));
            bool ad = false;
            if (key == L"DTSTART") {
                ParseIcsTime(value, &ev.start, &ad);
                ev.allDay = ad;
            } else if (key == L"DTEND") {
                ParseIcsTime(value, &ev.end, &ad);
            } else if (key == L"SUMMARY") {
                ev.title = IcsUnescape(value);
            } else if (key == L"RRULE") {
                rrule = value;
            } else if (key == L"EXDATE") {
                for (const auto& d : SplitList(value, L",")) {
                    long long m;
                    if (ParseIcsTime(d, &m, &ad)) exdates.push_back(m);
                }
            } else if (key == L"STATUS" && value == L"CANCELLED") {
                cancelled = true;
            }
        }
        std::sort(out.begin(), out.end(), [](const CalEvent& a, const CalEvent& b) { return a.start < b.start; });
        if (out.size() > 60) out.resize(60);
    }
    std::lock_guard<std::mutex> lock(g_dataMutex);
    if (error.empty() || g_events.empty() || !out.empty()) g_events = out;  // a failed refresh keeps what it had
    g_eventsError = error;
}

// ---------------------------------------------------------------------------
// Quotes (stocks, indexes, currencies, crypto) from Yahoo Finance's public chart data
// ---------------------------------------------------------------------------

struct Quote {
    std::wstring symbol, name, currency;
    double price = 0, prev = 0;
    std::vector<double> closes;
};
static std::vector<Quote> g_quotes;
static std::wstring g_quotesError;

static void FetchQuotes() {
    std::vector<Quote> out;
    for (const auto& sym : SplitList(GetSettings().stocks, L",; ")) {
        if (out.size() >= 9) break;
        std::string json = HttpGetUrl(L"https://query1.finance.yahoo.com/v8/finance/chart/" + UrlEncode(sym) + L"?range=1d&interval=15m");
        size_t meta = json.find("\"meta\":{");
        if (meta == std::string::npos) continue;
        Quote q;
        q.symbol = sym;
        JsonNumber(json, meta, "regularMarketPrice", &q.price);
        if (!JsonNumber(json, meta, "chartPreviousClose", &q.prev)) JsonNumber(json, meta, "previousClose", &q.prev);
        size_t metaEnd = json.find("\"indicators\"", meta);
        q.name = JsonString(json, meta, metaEnd == std::string::npos ? json.size() : metaEnd, "shortName");
        q.currency = JsonString(json, meta, metaEnd == std::string::npos ? json.size() : metaEnd, "currency");
        size_t quote = json.find("\"quote\":[{");
        if (quote != std::string::npos)
            for (double v : JsonArray(json, quote, "close"))
                if (v > 0) q.closes.push_back(v);
        if (q.price > 0) out.push_back(q);
    }
    // The settings first: GetSettings takes the same lock, and taking it twice would freeze the thread
    bool none = GetSettings().stocks.empty();
    std::lock_guard<std::mutex> lock(g_dataMutex);
    if (!out.empty() || none) g_quotes = out;
    g_quotesError = out.empty() && !none ? L"Não deu para buscar as cotações agora" : L"";
}

// ---------------------------------------------------------------------------
// News: the headlines of RSS / Atom feeds
// ---------------------------------------------------------------------------

struct NewsItem {
    std::wstring title, link, source, image;
    long long when = 0;  // published, in seconds since 1970 (UTC); 0 = unknown
    std::shared_ptr<Image> img;  // its cover, once downloaded
};
static std::vector<NewsItem> g_news;
static std::wstring g_newsError;

static std::wstring TagText(const std::wstring& xml, size_t from, size_t to, PCWSTR tag) {
    std::wstring open = std::wstring(L"<") + tag;
    size_t p = from;
    while (true) {
        p = xml.find(open, p);
        if (p == std::wstring::npos || p >= to) return L"";
        wchar_t after = p + open.size() < xml.size() ? xml[p + open.size()] : L'>';
        if (after == L'>' || after == L' ') break;  // not <titlexyz
        p += open.size();
    }
    size_t gt = xml.find(L'>', p);
    if (gt == std::wstring::npos || gt >= to) return L"";
    if (xml[gt - 1] == L'/') {  // <link href="..."/> (Atom)
        size_t h = xml.find(L"href=\"", p);
        if (h == std::wstring::npos || h > gt) return L"";
        size_t e = xml.find(L'"', h + 6);
        return XmlDecode(xml.substr(h + 6, e - h - 6));
    }
    size_t end = xml.find(std::wstring(L"</") + tag + L">", gt);
    if (end == std::wstring::npos || end > to) return L"";
    std::wstring v = xml.substr(gt + 1, end - gt - 1);
    size_t c = v.find(L"<![CDATA[");
    if (c != std::wstring::npos) {
        size_t ce = v.find(L"]]>", c);
        v = v.substr(c + 9, ce == std::wstring::npos ? std::wstring::npos : ce - c - 9);
    }
    // Tags left inside a title go away
    std::wstring clean;
    bool inTag = false;
    for (wchar_t ch : v) {
        if (ch == L'<') inTag = true;
        else if (ch == L'>') inTag = false;
        else if (!inTag) clean += ch == L'\n' || ch == L'\r' || ch == L'\t' ? L' ' : ch;
    }
    clean = XmlDecode(clean);
    while (!clean.empty() && clean.back() == L' ') clean.pop_back();
    while (!clean.empty() && clean.front() == L' ') clean.erase(0, 1);
    return clean;
}

// The value of an attribute inside a tag that starts at 'at'
static std::wstring AttrOf(const std::wstring& xml, size_t at, PCWSTR name) {
    size_t gt = xml.find(L'>', at);
    std::wstring tag = xml.substr(at, gt == std::wstring::npos ? 400 : gt - at);
    std::wstring n = std::wstring(name) + L"=";
    size_t p = tag.find(n);
    if (p == std::wstring::npos) return L"";
    p += n.size();
    std::wstring v;
    if (p < tag.size() && (tag[p] == L'"' || tag[p] == L'\'')) {
        wchar_t q = tag[p];
        size_t e = tag.find(q, p + 1);
        v = tag.substr(p + 1, e == std::wstring::npos ? std::wstring::npos : e - p - 1);
    } else {
        size_t e = tag.find_first_of(L" >", p);
        v = tag.substr(p, e == std::wstring::npos ? std::wstring::npos : e - p);
    }
    return XmlDecode(v);
}

static bool LooksLikeImage(const std::wstring& url) {
    std::wstring u = url;
    for (auto& c : u) c = towlower(c);
    return u.find(L".jpg") != std::wstring::npos || u.find(L".jpeg") != std::wstring::npos || u.find(L".png") != std::wstring::npos ||
           u.find(L".webp") != std::wstring::npos || u.find(L"image") != std::wstring::npos || u.find(L"/img") != std::wstring::npos;
}

// The cover of a story: media:content / media:thumbnail / an image enclosure / the first <img> in its text
static std::wstring NewsImage(const std::wstring& xml, size_t from, size_t to) {
    std::wstring item = xml.substr(from, to - from);
    for (PCWSTR tag : {L"<media:content", L"<media:thumbnail", L"<enclosure", L"<image"}) {
        size_t p = 0;
        while ((p = item.find(tag, p)) != std::wstring::npos) {
            std::wstring url = AttrOf(item, p, L"url");
            if (url.empty()) url = AttrOf(item, p, L"href");
            std::wstring type = AttrOf(item, p, L"type") + AttrOf(item, p, L"medium");
            if (!url.empty() && (type.find(L"image") != std::wstring::npos || LooksLikeImage(url))) return url;
            p++;
        }
    }
    // An <img> in the description, written as tags or escaped (&lt;img src=&quot;...)
    std::wstring text = XmlDecode(item);
    size_t p = text.find(L"<img");
    if (p != std::wstring::npos) {
        std::wstring url = AttrOf(text, p, L"src");
        if (url.rfind(L"//", 0) == 0) url = L"https:" + url;
        if (url.rfind(L"http", 0) == 0) return url;
    }
    return L"";
}

static std::wstring Lower(std::wstring s) {
    for (auto& c : s) c = towlower(c);
    return s;
}

// Stories made by the dozen (forecasts for each city, lottery results...) aren't the news of the day
static bool AutomaticStory(const NewsItem& n) {
    std::wstring t = Lower(n.title), l = Lower(n.link);
    for (PCWSTR w : {L"previsão do tempo", L"previsao do tempo", L"/previsao-do-tempo", L"resultado da mega", L"resultado da lotof",
                     L"resultado da quina", L"horóscopo", L"horoscopo"})
        if (t.find(w) != std::wstring::npos || l.find(w) != std::wstring::npos) return true;
    return false;
}

static std::wstring FirstWords(const std::wstring& s, int n) {
    std::wstring out;
    int words = 0;
    for (wchar_t c : Lower(s)) {
        if (c == L' ' && ++words >= n) break;
        out += c;
    }
    return out;
}

static std::wstring Trimmed(const std::wstring& s);

// Now, in seconds since 1970 (UTC)
static long long UnixNow() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return (long long)((((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000000ULL) - 11644473600LL;
}

static long long UnixFrom(int y, int mo, int d, int h, int mi, int sec) {
    SYSTEMTIME st = {(WORD)y, (WORD)mo, 0, (WORD)d, (WORD)h, (WORD)mi, (WORD)sec, 0};
    FILETIME ft;
    if (y < 1990 || mo < 1 || mo > 12 || d < 1 || d > 31 || !SystemTimeToFileTime(&st, &ft)) return 0;
    return (long long)((((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000000ULL) - 11644473600LL;
}

// A feed's date: RSS "Wed, 25 Nov 2026 14:32:00 -0300" (or GMT) and Atom "2026-11-25T14:32:00-03:00" (or Z)
static long long ParseFeedDate(const std::wstring& raw) {
    std::wstring t = Trimmed(raw);
    if (t.empty()) return 0;
    int y = 0, mo = 0, d = 0, h = 0, mi = 0, sec = 0;
    long long zone = 0;  // seconds east of UTC
    if (t.size() >= 10 && iswdigit(t[0]) && t[4] == L'-') {
        swscanf(t.c_str(), L"%d-%d-%dT%d:%d:%d", &y, &mo, &d, &h, &mi, &sec);
        size_t z = t.find_first_of(L"Z+-", 19);
        if (z != std::wstring::npos && t[z] != L'Z') {
            int zh = 0, zm = 0;
            swscanf(t.c_str() + z + 1, L"%d:%d", &zh, &zm);
            zone = (t[z] == L'-' ? -1 : 1) * (zh * 3600LL + zm * 60LL);
        }
    } else {
        size_t comma = t.find(L',');
        std::wstring r = comma == std::wstring::npos ? t : t.substr(comma + 1);
        wchar_t mon[8] = L"", tz[16] = L"";
        swscanf(r.c_str(), L" %d %7ls %d %d:%d:%d %15ls", &d, mon, &y, &h, &mi, &sec, tz);
        static const PCWSTR months[] = {L"jan", L"feb", L"mar", L"apr", L"may", L"jun", L"jul", L"aug", L"sep", L"oct", L"nov", L"dec"};
        static const PCWSTR meses[] = {L"jan", L"fev", L"mar", L"abr", L"mai", L"jun", L"jul", L"ago", L"set", L"out", L"nov", L"dez"};
        std::wstring m = Lower(mon).substr(0, 3);
        for (int i = 0; i < 12; i++)
            if (m == months[i] || m == meses[i]) mo = i + 1;
        if (tz[0] == L'+' || tz[0] == L'-') {
            int v = _wtoi(tz + 1);
            zone = (tz[0] == L'-' ? -1 : 1) * ((v / 100) * 3600LL + (v % 100) * 60LL);
        }
        if (y < 100) y += 2000;
    }
    long long u = UnixFrom(y, mo, d, h, mi, sec);
    return u ? u - zone : 0;
}

// "agora", "há 12 min", "há 3 h", "há 2 dias"
static std::wstring NewsAgo(long long when) {
    if (!when) return L"";
    long long s = UnixNow() - when;
    if (s < 90) return T(L"agora");
    if (s < 3600) return Fmt(TF(L"há %lld min"), s / 60);
    if (s < 86400) return Fmt(TF(L"há %lld h"), s / 3600);
    long long days = s / 86400;
    return Fmt(TF(days == 1 ? L"há %lld dia" : L"há %lld dias"), days);
}

static bool IsWebUrl(const std::wstring& u);

static bool IsFeed(const std::wstring& xml) {
    bool root = xml.find(L"<rss") != std::wstring::npos || xml.find(L"<feed") != std::wstring::npos ||
                xml.find(L"<rdf:RDF") != std::wstring::npos;
    return root && (xml.find(L"<item") != std::wstring::npos || xml.find(L"<entry") != std::wstring::npos);
}

// An address found in a page, made whole: //host/x, /x and x are relative to the page
static std::wstring AbsoluteUrl(const std::wstring& page, std::wstring href) {
    if (href.empty() || IsWebUrl(href)) return href;
    size_t scheme = page.find(L"://");
    if (scheme == std::wstring::npos) return L"";
    if (href.rfind(L"//", 0) == 0) return page.substr(0, scheme + 1) + href;
    size_t slash = page.find(L'/', scheme + 3);
    std::wstring origin = page.substr(0, slash);
    if (href[0] == L'/') return origin + href;
    if (href.find(L':') != std::wstring::npos) return L"";  // another scheme (mailto:, javascript:...)
    std::wstring dir = slash == std::wstring::npos ? origin + L"/" : page.substr(0, page.find_last_of(L'/') + 1);
    return dir + href;
}

// The feed behind an address: the address itself if it's a feed; else the feed the page announces
// (<link rel="alternate" type="application/rss+xml">), a link on the page that looks like one, or the usual places
// (/feed, /rss...). What was found is remembered, so a site is only searched once
static std::vector<std::pair<std::wstring, std::wstring>> g_feedFound;  // address -> its feed (network thread only)
struct FeedWait {
    ULONGLONG until = 0;  // not asked again before this (GetTickCount64)
    int fails = 0;        // failures in a row
};
static std::vector<std::pair<std::wstring, FeedWait>> g_feedWait;  // address -> how long to leave it alone

// A site that failed is left alone for a while, longer each time: 1, 2, 4, 8... up to 60 minutes
static void FeedFailed(const std::wstring& url, bool noFeed) {
    FeedWait* w = FindIn(g_feedWait, url);
    if (!w) {
        g_feedWait.push_back({url, {}});
        w = &g_feedWait.back().second;
    }
    w->fails++;
    ULONGLONG minutes = noFeed ? 60 : std::min<ULONGLONG>(60, 1ULL << std::min(w->fails - 1, 6));
    w->until = GetTickCount64() + minutes * 60 * 1000;
}

static long long ParseFeedDate(const std::wstring& raw);
static long long UnixNow();

// When the newest story of a feed came out (0: no dates)
static long long NewestStory(const std::wstring& xml) {
    long long best = 0;
    for (PCWSTR tag : {L"<pubDate", L"<published", L"<updated", L"<dc:date"}) {
        std::wstring open = tag, close = std::wstring(L"</") + (tag + 1) + L">";
        for (size_t p = 0; (p = xml.find(open, p)) != std::wstring::npos;) {
            size_t gt = xml.find(L'>', p), end = xml.find(close, p);
            if (gt == std::wstring::npos || end == std::wstring::npos || end < gt) break;
            best = std::max(best, ParseFeedDate(xml.substr(gt + 1, end - gt - 1)));
            p = end;
        }
    }
    return best;
}

// A site's own pages can point to feeds nobody updates any more (an old section, a region): one whose newest story
// is older than this doesn't count
constexpr long long kStaleFeed = 30LL * 24 * 3600;

// One request for the address; only when it's a page and not a feed, up to 10 more to find the feed (see the README)
static std::wstring FeedXml(std::wstring url, std::wstring* feedUrl) {
    if (url.find(L"://") == std::wstring::npos) url = L"https://" + url;  // "g1.globo.com" is enough
    if (!IsWebUrl(url)) return L"";
    *feedUrl = url;
    if (FeedWait* w = FindIn(g_feedWait, url))
        if (GetTickCount64() < w->until) return L"";
    if (auto* known = FindIn(g_feedFound, url)) {
        std::wstring xml = FromUtf8(HttpGetUrl(*known));
        if (xml.empty()) {  // didn't answer: nothing else is tried
            FeedFailed(url, false);
            return L"";
        }
        long long newest = IsFeed(xml) ? NewestStory(xml) : -1;
        if (newest == 0 || (newest > 0 && UnixNow() - newest < kStaleFeed)) {
            if (FeedWait* w = FindIn(g_feedWait, url)) w->fails = 0;
            *feedUrl = *known;
            return xml;
        }
        // No longer a feed, or abandoned: searched again below
    }
    std::wstring page = FromUtf8(HttpGetUrl(url));
    if (page.empty()) {  // no network, blocked or gone: no guessing at other addresses on that site
        FeedFailed(url, false);
        return L"";
    }
    if (IsFeed(page)) {
        if (FeedWait* w = FindIn(g_feedWait, url)) w->fails = 0;
        return page;
    }
    std::vector<std::wstring> tries;
    auto addTry = [&](const std::wstring& u) {
        if (!u.empty() && IsWebUrl(u) && std::find(tries.begin(), tries.end(), u) == tries.end()) tries.push_back(u);
    };
    for (size_t p = 0; (p = page.find(L"<link", p)) != std::wstring::npos; p++) {
        std::wstring type = Lower(AttrOf(page, p, L"type"));
        if (type.find(L"rss+xml") != std::wstring::npos || type.find(L"atom+xml") != std::wstring::npos)
            addTry(AbsoluteUrl(url, AttrOf(page, p, L"href")));
    }
    for (size_t p = 0; (p = page.find(L"<a ", p)) != std::wstring::npos && tries.size() < 6; p++) {
        std::wstring href = AttrOf(page, p, L"href"), h = Lower(href);
        if (h.find(L"rss") != std::wstring::npos || h.find(L"/feed") != std::wstring::npos || h.find(L"atom") != std::wstring::npos)
            addTry(AbsoluteUrl(url, href));
    }
    size_t scheme = url.find(L"://");
    std::wstring origin = url.substr(0, url.find(L'/', scheme + 3));
    std::wstring base = url.back() == L'/' ? url.substr(0, url.size() - 1) : url;
    // Sites that keep their feeds under /rss/<site>/ (as g1.globo.com/rss/g1/)
    std::wstring host = origin.substr(scheme + 3);
    addTry(origin + L"/rss/" + host.substr(0, host.find(L'.')) + L"/");
    for (PCWSTR tail : {L"/feed", L"/rss", L"/rss.xml", L"/feed.xml", L"/index.xml", L"/atom.xml", L"/feed/"}) {
        addTry(base + tail);
        addTry(origin + tail);
    }
    // Of the feeds found, the one with the newest stories (a recent one ends the search)
    std::wstring bestUrl, bestXml;
    long long bestWhen = -1;
    for (size_t i = 0; i < tries.size() && i < 10 && g_running; i++) {
        std::wstring xml = FromUtf8(HttpGetUrl(tries[i]));
        if (!IsFeed(xml)) continue;
        long long when = NewestStory(xml);
        if (when > 0 && UnixNow() - when >= kStaleFeed) continue;  // abandoned
        if (when > bestWhen) {
            bestWhen = when;
            bestUrl = tries[i];
            bestXml = xml;
        }
        if (when > 0 && UnixNow() - when < 2 * 24 * 3600) break;
    }
    if (!bestUrl.empty()) {
        if (auto* known = FindIn(g_feedFound, url)) *known = bestUrl;
        else g_feedFound.push_back({url, bestUrl});
        if (FeedWait* w = FindIn(g_feedWait, url)) w->fails = 0;
        *feedUrl = bestUrl;
        return bestXml;
    }
    FeedFailed(url, true);  // no feed on this site: searched again in an hour
    return L"";
}

// Only web pages: a feed (or anyone between it and us, on plain http) could name a program, a file or another protocol
static bool IsWebUrl(const std::wstring& u) {
    return _wcsnicmp(u.c_str(), L"http://", 7) == 0 || _wcsnicmp(u.c_str(), L"https://", 8) == 0;
}

// True when every site answered (even with no recent stories): only a real failure is tried again in a minute
static bool FetchNews() {
    std::vector<std::vector<NewsItem>> perFeed;
    bool allAnswered = true;
    for (const auto& url : SplitList(GetSettings().newsFeeds, L",; \n")) {
        if (perFeed.size() >= 5 || !g_running) break;
        std::wstring feedUrl;
        std::wstring xml = FeedXml(url, &feedUrl);
        if (xml.empty()) {
            allAnswered = false;
            continue;
        }
        std::vector<NewsItem> items;
        bool atom = xml.find(L"<entry") != std::wstring::npos && xml.find(L"<item") == std::wstring::npos;
        PCWSTR itemTag = atom ? L"<entry" : L"<item";
        size_t first = xml.find(itemTag);
        std::wstring source = TagText(xml, 0, first == std::wstring::npos ? xml.size() : first, L"title");
        size_t p = first;
        while (p != std::wstring::npos && items.size() < 200) {
            size_t end = xml.find(atom ? L"</entry>" : L"</item>", p);
            if (end == std::wstring::npos) break;
            NewsItem n;
            n.title = TagText(xml, p, end, L"title");
            n.link = TagText(xml, p, end, L"link");
            if (n.link.empty() && atom) {  // Atom: <link href="..."/>
                size_t lp = xml.find(L"<link", p);
                if (lp != std::wstring::npos && lp < end) n.link = AttrOf(xml, lp, L"href");
            }
            if (!n.link.empty() && !IsWebUrl(n.link)) n.link = AbsoluteUrl(feedUrl, n.link);  // a relative link (or "")
            n.source = source;
            n.image = NewsImage(xml, p, end);
            if (!n.image.empty() && !IsWebUrl(n.image)) n.image = AbsoluteUrl(feedUrl, n.image);
            for (PCWSTR tag : {L"pubDate", L"published", L"updated", L"dc:date"}) {
                if (n.when) break;
                n.when = ParseFeedDate(TagText(xml, p, end, tag));
            }
            if (!n.title.empty() && !AutomaticStory(n)) items.push_back(n);
            p = xml.find(itemTag, end);
        }
        // The newest of this feed, whatever order it lists them in (some put old ones first, or mix them); stories from
        // more than 30 days ago aren't news
        long long now = UnixNow();
        items.erase(std::remove_if(items.begin(), items.end(), [&](const NewsItem& n) { return n.when > 0 && now - n.when > kStaleFeed; }),
                    items.end());
        std::stable_sort(items.begin(), items.end(), [](const NewsItem& a, const NewsItem& b) { return a.when > b.when; });
        if (items.size() > 20) items.resize(20);
        // A title pattern repeated over and over is a series of automatic stories: only its first one stays
        std::vector<NewsItem> kept;
        for (auto& n : items) {
            std::wstring head = FirstWords(n.title, 3);
            int same = 0;
            for (auto& m : items) same += FirstWords(m.title, 3) == head;
            bool seen = false;
            for (auto& k : kept) seen = seen || FirstWords(k.title, 3) == head;
            if (same < 3 || !seen) kept.push_back(n);
        }
        if (!kept.empty()) perFeed.push_back(kept);
    }
    // One from each feed in turn
    std::vector<NewsItem> out;
    for (size_t i = 0; i < 20; i++)
        for (auto& f : perFeed)
            if (i < f.size()) {
                bool dup = false;  // the same story in two sections
                for (auto& o : out) dup = dup || (!o.link.empty() && o.link == f[i].link) || o.title == f[i].title;
                if (!dup) out.push_back(f[i]);
            }
    // The headlines go out at once; the covers are downloaded after, on their own thread (they'd hold up the music,
    // notifications and everything else on this one). Covers already downloaded are kept
    std::vector<NewsItem> before;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        before = g_news;
    }
    // The newest first, whatever section they came from (as the site's latest news). Stories without a date keep their
    // place after the dated ones
    std::stable_sort(out.begin(), out.end(), [](const NewsItem& a, const NewsItem& b) { return a.when > b.when; });
    if (out.size() > 40) out.resize(40);
    bool missing = false;
    for (size_t i = 0; i < out.size(); i++) {
        if (out[i].image.empty()) continue;
        for (auto& b : before)
            if (b.image == out[i].image) out[i].img = b.img;
        if (!out[i].img && i < 30) missing = true;
    }
    bool none = GetSettings().newsFeeds.empty();  // before the lock (GetSettings takes it too)
    std::lock_guard<std::mutex> lock(g_dataMutex);
    if (!out.empty() || none) g_news = out;
    g_newsError = out.empty() && !none ? L"Nenhuma notícia nesse endereço. Ele precisa ser um feed RSS (ex.: g1.globo.com/rss/g1/brasil/)" : L"";
    if (missing) g_coversWanted = true;
    return allAnswered;
}

// ---------------------------------------------------------------------------
// Notes and reminders: plain text files in a folder of yours (Documents\MacOS Widgets by default), so they open in
// Notepad and can live in OneDrive
// ---------------------------------------------------------------------------

struct NoteInfo {
    std::wstring path, title, text;
    long long written = 0;
};
struct ReminderLine {
    std::wstring text;
    bool done = false;
    int line = 0;  // its line in the file
};
static std::vector<NoteInfo> g_notes;
static std::vector<ReminderLine> g_reminders;

static std::wstring NotesFolder() {
    std::wstring chosen = GetSettings().notesFolder;
    if (!chosen.empty()) return chosen;
    PWSTR docs = nullptr;
    std::wstring out;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &docs))) {
        out = std::wstring(docs) + L"\\MacOS Widgets";
    }
    CoTaskMemFree(docs);
    return out;
}

enum class ReadResult { Ok, Missing, Failed };

// A file that isn't there and one that couldn't be read (locked by sync or an antivirus, online-only while offline...)
// are different: only the first may be treated as empty, or writing back would wipe what's in it
static ReadResult ReadText(const std::wstring& path, size_t maxBytes, std::wstring* out) {
    out->clear();
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        DWORD e = GetLastError();
        return e == ERROR_FILE_NOT_FOUND || e == ERROR_PATH_NOT_FOUND ? ReadResult::Missing : ReadResult::Failed;
    }
    LARGE_INTEGER size = {};
    if (!GetFileSizeEx(h, &size)) {
        CloseHandle(h);
        return ReadResult::Failed;
    }
    size_t want = (size_t)std::min<long long>(size.QuadPart, (long long)maxBytes);
    std::string b(want, '\0');
    DWORD read = 0;
    BOOL ok = want == 0 || ReadFile(h, &b[0], (DWORD)want, &read, nullptr);
    CloseHandle(h);
    if (!ok) return ReadResult::Failed;
    b.resize(read);
    std::wstring& w = *out;
    if (b.size() >= 2 && (unsigned char)b[0] == 0xFF && (unsigned char)b[1] == 0xFE) {
        w.assign((const wchar_t*)(b.data() + 2), (b.size() - 2) / 2);
        return ReadResult::Ok;
    }
    if (b.size() >= 3 && (unsigned char)b[0] == 0xEF && (unsigned char)b[1] == 0xBB && (unsigned char)b[2] == 0xBF) b.erase(0, 3);
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, b.data(), (int)b.size(), nullptr, 0);
    UINT cp = n > 0 || b.empty() ? CP_UTF8 : CP_ACP;
    n = MultiByteToWideChar(cp, 0, b.data(), (int)b.size(), nullptr, 0);
    w.assign(n, L'\0');
    if (n > 0) MultiByteToWideChar(cp, 0, b.data(), (int)b.size(), &w[0], n);
    return ReadResult::Ok;
}

// For showing only: anything unreadable shows as empty
static std::wstring ReadTextFile(const std::wstring& path, size_t maxBytes) {
    std::wstring w;
    ReadText(path, maxBytes, &w);
    return w;
}

static bool WriteTextFile(const std::wstring& path, const std::wstring& text) {
    int n = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), nullptr, 0, nullptr, nullptr);
    std::string u(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), &u[0], n, nullptr, nullptr);
    // Written beside it first, then put in its place: a failed write never leaves the file half written
    std::wstring tmp = path + L".mwtmp";
    HANDLE h = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    DWORD w = 0;
    BOOL ok = WriteFile(h, u.data(), (DWORD)u.size(), &w, nullptr) && w == u.size() && FlushFileBuffers(h);
    CloseHandle(h);
    if (ok) ok = MoveFileExW(tmp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
    if (!ok) DeleteFileW(tmp.c_str());
    return ok;
}

static std::wstring NotesDir();

// Nothing is created until the first note or reminder is saved
static std::wstring RemindersPath() { return NotesFolder() + L"\\Reminders.txt"; }
static std::wstring NotesDir() { return NotesFolder() + L"\\Notes"; }

static void ReadNotesAndReminders() {
    std::wstring dir = NotesFolder();
    if (dir.empty()) return;
    std::wstring notesDir = NotesDir();
    std::wstring rp = RemindersPath();

    std::vector<NoteInfo> notes;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileExW((notesDir + L"\\*").c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, 0);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            std::wstring name = fd.cFileName;
            size_t dot = name.find_last_of(L'.');
            std::wstring ext = dot == std::wstring::npos ? L"" : name.substr(dot);
            for (auto& c : ext) c = towlower(c);
            if (ext != L".txt" && ext != L".md") continue;
            NoteInfo n;
            n.path = notesDir + L"\\" + name;
            n.written = (long long)(((ULONGLONG)fd.ftLastWriteTime.dwHighDateTime << 32) | fd.ftLastWriteTime.dwLowDateTime);
            notes.push_back(n);
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    // Only the 12 newest are shown, so only they are read
    std::sort(notes.begin(), notes.end(), [](const NoteInfo& a, const NoteInfo& b) { return a.written > b.written; });
    if (notes.size() > 12) notes.resize(12);
    for (NoteInfo& n : notes) {
        std::wstring name = n.path.substr(notesDir.size() + 1);
        size_t dot = name.find_last_of(L'.');
        std::wstring text = ReadTextFile(n.path, 4096);
        auto lines = Split(text, L'\n');
        for (auto& l : lines) {
            if (!l.empty() && l.back() == L'\r') l.pop_back();
            while (!l.empty() && (l[0] == L'#' || l[0] == L' ')) l.erase(0, 1);
        }
        size_t i = 0;
        while (i < lines.size() && lines[i].empty()) i++;
        n.title = i < lines.size() ? lines[i] : name.substr(0, dot);
        for (size_t j = i + 1; j < lines.size() && n.text.size() < 600; j++)
            if (!lines[j].empty()) n.text += (n.text.empty() ? L"" : L"\n") + lines[j];
    }

    std::vector<ReminderLine> rem;
    std::wstring remText;
    bool remOk = ReadText(rp, 16 * 1024 * 1024, &remText) != ReadResult::Failed;
    auto lines = Split(remText, L'\n');
    for (size_t i = 0; i < lines.size(); i++) {
        std::wstring l = lines[i];
        if (!l.empty() && l.back() == L'\r') l.pop_back();
        size_t a = 0;
        while (a < l.size() && l[a] == L' ') a++;
        l = l.substr(a);
        if (l.empty()) continue;
        ReminderLine r;
        r.line = (int)i;
        if (l.size() >= 3 && l[0] == L'[' && l[2] == L']') {
            r.done = l[1] == L'x' || l[1] == L'X';
            r.text = l.substr(3);
        } else if (l.size() >= 2 && (l[0] == L'-' || l[0] == L'*') && l[1] == L' ') {
            r.text = l.substr(2);
        } else {
            r.text = l;
        }
        while (!r.text.empty() && r.text[0] == L' ') r.text.erase(0, 1);
        if (!r.text.empty()) rem.push_back(r);
    }
    std::stable_sort(rem.begin(), rem.end(), [](const ReminderLine& a, const ReminderLine& b) { return !a.done && b.done; });
    std::lock_guard<std::mutex> lock(g_dataMutex);
    g_notes = notes;
    if (remOk) g_reminders = rem;  // unreadable for a moment: the ones shown stay
}

// Marks a reminder done (or not), changing only its line in the file
static void ToggleReminder(int line) {
    std::wstring rp = RemindersPath();
    std::wstring text;
    if (ReadText(rp, 16 * 1024 * 1024, &text) != ReadResult::Ok) return;  // never write over what couldn't be read
    auto lines = Split(text, L'\n');
    if (line < 0 || line >= (int)lines.size()) return;
    std::wstring& l = lines[line];
    bool cr = !l.empty() && l.back() == L'\r';
    if (cr) l.pop_back();
    size_t a = 0;
    while (a < l.size() && l[a] == L' ') a++;
    std::wstring body = l.substr(a);
    if (body.size() >= 3 && body[0] == L'[' && body[2] == L']') body[1] = (body[1] == L'x' || body[1] == L'X') ? L' ' : L'x';
    else if (body.size() >= 2 && (body[0] == L'-' || body[0] == L'*') && body[1] == L' ') body = L"[x] " + body.substr(2);
    else body = L"[x] " + body;
    l = l.substr(0, a) + body + (cr ? L"\r" : L"");
    std::wstring out;
    for (size_t i = 0; i < lines.size(); i++) out += lines[i] + (i + 1 < lines.size() ? L"\n" : L"");
    WriteTextFile(rp, out);
    ReadNotesAndReminders();
}

// ---------------------------------------------------------------------------
// Home (Home Assistant): its REST API with a long-lived access token. Home Assistant gathers SmartThings, Matter
// devices (also shared from Google Home and Alexa) and most brands' own integrations in one place
// ---------------------------------------------------------------------------

struct HaEntity {
    std::wstring id, name, state, unit, deviceClass;
    std::wstring domain() const { return id.substr(0, id.find(L'.')); }
};
static std::vector<HaEntity> g_ha;
static std::vector<HaEntity> g_haAll;  // everything useful in the Home Assistant, for the favorites picker
static std::wstring g_haError;
static std::deque<std::pair<std::wstring, std::wstring>> g_haCommands;  // (domain/service, entity) for the data thread

static std::string HttpRequest(PCWSTR method, const std::wstring& url, const std::wstring& headers, const std::string& body,
                               DWORD* statusOut) {
    std::string out;
    *statusOut = 0;
    URL_COMPONENTSW uc = {sizeof(uc)};
    wchar_t host[256] = L"", path[2048] = L"";
    uc.lpszHostName = host;
    uc.dwHostNameLength = 256;
    uc.lpszUrlPath = path;
    uc.dwUrlPathLength = 2048;
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &uc)) return out;
    HINTERNET session = WinHttpOpen(L"MacOSWidgets", WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) return out;
    WinHttpSetTimeouts(session, 3000, 3000, 4000, 6000);
    HINTERNET conn = WinHttpConnect(session, host, uc.nPort, 0);
    HINTERNET req = HttpTrack(conn ? WinHttpOpenRequest(conn, method, path, nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                                        uc.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0)
                                   : nullptr);
    if (req && WinHttpSendRequest(req, headers.c_str(), (DWORD)-1L, body.empty() ? WINHTTP_NO_REQUEST_DATA : (LPVOID)body.data(),
                                  (DWORD)body.size(), (DWORD)body.size(), 0) &&
        WinHttpReceiveResponse(req, nullptr)) {
        DWORD status = 0, size = sizeof(status);
        WinHttpQueryHeaders(req, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status, &size,
                            WINHTTP_NO_HEADER_INDEX);
        *statusOut = status;
        DWORD avail = 0;
        while (WinHttpQueryDataAvailable(req, &avail) && avail > 0 && out.size() < 16 * 1024 * 1024) {
            std::string buf(avail, '\0');
            DWORD read = 0;
            if (!WinHttpReadData(req, &buf[0], avail, &read) || read == 0) break;
            out.append(buf.data(), read);
        }
    }
    HttpRelease(req);
    if (conn) WinHttpCloseHandle(conn);
    WinHttpCloseHandle(session);
    return out;
}

static std::wstring HaBase() {
    std::wstring u = GetSettings().haUrl;
    while (!u.empty() && (u.back() == L'/' || u.back() == L' ')) u.pop_back();
    while (!u.empty() && u.front() == L' ') u.erase(0, 1);
    if (!u.empty() && u.find(L"://") == std::wstring::npos) u = L"http://" + u;
    return u;
}
static std::wstring HaHeaders() {
    std::wstring t = GetSettings().haToken;
    while (!t.empty() && t.back() == L' ') t.pop_back();
    while (!t.empty() && t.front() == L' ') t.erase(0, 1);
    return L"Authorization: Bearer " + t + L"\r\nContent-Type: application/json\r\n";
}

// Which entities a widget shows by itself, in this order (before the rest of each kind)
static int HaRank(const HaEntity& e) {
    std::wstring d = e.domain();
    if (d == L"light") return 0;
    if (d == L"switch") return 1;
    if (d == L"fan") return 2;
    if (d == L"cover") return 3;
    if (d == L"climate") return 4;
    if (d == L"media_player") return 5;
    if (d == L"lock") return 6;
    if (d == L"scene" || d == L"script") return 7;
    if (d == L"input_boolean") return 8;
    if (d == L"sensor" && (e.deviceClass == L"temperature" || e.deviceClass == L"humidity")) return 9;
    return -1;
}

static void FetchHome() {
    std::wstring base = HaBase();
    Settings s = GetSettings();
    std::vector<HaEntity> out;
    std::wstring error;
    if (base.empty() || s.haToken.empty()) {
        error = L"Ligue o Home Assistant em Ajustes > Casa";
    } else {
        DWORD status = 0;
        std::string json = HttpRequest(L"GET", base + L"/api/states", HaHeaders(), "", &status);
        if (status == 401) error = L"O Home Assistant recusou o token (confira em Ajustes > Casa)";
        else if (status != 200 || json.empty()) error = Fmt(TF(L"Não deu para falar com o Home Assistant em %ls"), base.c_str());
        else {
            // Each entity starts with {"entity_id":"..." (inside attributes the key never follows a "{")
            const std::string key = "{\"entity_id\":\"";
            std::vector<size_t> starts;
            for (size_t p = json.find(key); p != std::string::npos; p = json.find(key, p + key.size())) starts.push_back(p);
            for (size_t i = 0; i < starts.size(); i++) {
                size_t a = starts[i], b = i + 1 < starts.size() ? starts[i + 1] : json.size();
                HaEntity e;
                e.id = JsonString(json, a, b, "entity_id");
                e.state = JsonString(json, a, b, "state");
                e.name = JsonString(json, a, b, "friendly_name");
                e.unit = JsonString(json, a, b, "unit_of_measurement");
                e.deviceClass = JsonString(json, a, b, "device_class");
                if (e.name.empty()) e.name = e.id;
                out.push_back(e);
            }
            {
                std::vector<HaEntity> useful;
                for (const auto& e : out)
                    if (HaRank(e) >= 0 || e.domain() == L"sensor" || e.domain() == L"binary_sensor") useful.push_back(e);
                std::lock_guard<std::mutex> lock(g_dataMutex);
                g_haAll = useful;
            }
            std::vector<std::wstring> wanted = SplitList(s.haEntities, L",; \n");
            std::vector<HaEntity> chosen;
            if (!wanted.empty()) {
                for (const auto& id : wanted)
                    for (const auto& e : out)
                        if (e.id == id) chosen.push_back(e);
            } else {
                for (const auto& e : out)
                    if (HaRank(e) >= 0) chosen.push_back(e);
                std::stable_sort(chosen.begin(), chosen.end(), [](const HaEntity& x, const HaEntity& y) { return HaRank(x) < HaRank(y); });
            }
            if (chosen.size() > 24) chosen.resize(24);
            out = chosen;
        }
    }
    std::lock_guard<std::mutex> lock(g_dataMutex);
    if (error.empty() || g_ha.empty()) g_ha = out;
    g_haError = error;
}

static void RunHomeCommands() {
    std::deque<std::pair<std::wstring, std::wstring>> cmds;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        cmds.swap(g_haCommands);
    }
    if (cmds.empty()) return;
    std::wstring base = HaBase(), headers = HaHeaders();
    for (const auto& c : cmds) {
        std::wstring idw = c.second;
        std::string body = "{\"entity_id\":\"";
        for (wchar_t ch : idw) body += (char)ch;  // entity ids are plain ASCII
        body += "\"}";
        DWORD status = 0;
        HttpRequest(L"POST", base + L"/api/services/" + c.first, headers, body, &status);
        if (status != 200 && status != 201) Wh_Log(L"Home Assistant: %ls %ls -> %u", c.first.c_str(), idw.c_str(), (unsigned)status);
    }
    FetchHome();
}

// What a tap does to it: nullptr = shown only (locks, alarms, thermostats and sensors are never changed by a tap)
static PCWSTR HaService(const HaEntity& e) {
    std::wstring d = e.domain();
    if (d == L"light") return L"light/toggle";
    if (d == L"switch") return L"switch/toggle";
    if (d == L"fan") return L"fan/toggle";
    if (d == L"input_boolean") return L"input_boolean/toggle";
    if (d == L"cover") return L"cover/toggle";
    if (d == L"media_player") return L"media_player/media_play_pause";
    if (d == L"scene") return L"scene/turn_on";
    if (d == L"script") return L"script/turn_on";
    return nullptr;
}

// The news covers, downloaded one by one after each news update, the first stories first
static void CoverThread() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    while (g_running) {
        {
            std::unique_lock<std::mutex> lock(g_dataMutex);
            g_cv.wait_for(lock, std::chrono::seconds(5), [] { return !g_running || g_coversWanted; });
            if (!g_running) break;
            if (!g_coversWanted) continue;
            g_coversWanted = false;
        }
        for (int i = 0; i < 30 && g_running; i++) {
            std::wstring url;
            {
                std::lock_guard<std::mutex> lock(g_dataMutex);
                if (i >= (int)g_news.size()) break;
                if (g_news[i].img || g_news[i].image.empty()) continue;
                url = g_news[i].image;
            }
            std::string bytes = HttpGetUrl(url);
            std::shared_ptr<Image> img;
            if (!bytes.empty() && bytes.size() <= 8 * 1024 * 1024)
                img = DecodeBytes(std::vector<uint8_t>(bytes.begin(), bytes.end()), 520);
            if (!img) continue;  // tried again at the next news update
            std::lock_guard<std::mutex> lock(g_dataMutex);
            for (auto& n : g_news)
                if (n.image == url) n.img = img;
        }
    }
    CoUninitialize();
}

// Everything that comes from the internet (and Home Assistant), on its own thread: a slow site or a server that
// doesn't answer holds up only these, never the music, notifications, notes or the clock
static void NetThread() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    ULONGLONG lastWeather = 0, lastCal = 0, lastQuotes = 0, lastNews = 0, lastHome = 0;
    while (g_running) {
        Settings s = GetSettings();
        bool weatherNow, calNow, newsNow;
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            weatherNow = g_weatherRefresh;
            calNow = g_eventsRefresh;
            newsNow = g_newsRefresh;
            g_weatherRefresh = g_eventsRefresh = g_newsRefresh = false;
        }
        RunHomeCommands();  // taps on the Home widget (a slow Home Assistant holds up only this thread)
        ULONGLONG now = GetTickCount64();
        // Only when Weather or Sunrise is shown, and there is a city to ask about
        if ((s.showWeather || s.showSun) && HasCity(s) && (weatherNow || !lastWeather || now - lastWeather > 15 * 60 * 1000)) {
            FetchWeather();
            lastWeather = now;
        }
        bool haDown;
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            haDown = !g_haError.empty();
        }
        // Every 5 s; when Home Assistant doesn't answer, every minute
        if (g_running && s.showHome && (calNow || !lastHome || now - lastHome > (haDown ? 60 : 5) * 1000)) {
            FetchHome();
            lastHome = now;
        }
        if (g_running && s.showStocks && (calNow || !lastQuotes || now - lastQuotes > 5 * 60 * 1000)) {
            FetchQuotes();
            lastQuotes = now;
        }
        if (g_running && s.showCalendar && (calNow || !lastCal || now - lastCal > 15 * 60 * 1000)) {
            FetchCalendar();
            lastCal = now;
        }
        if (g_running && s.showNews && (calNow || newsNow || !lastNews || now - lastNews > 15 * 60 * 1000)) {
            bool ok = FetchNews();
            g_cv.notify_all();  // the covers thread starts on them
            // A failed update (the internet not back yet after the PC wakes up, for one) is tried again in a minute
            lastNews = ok ? now : now - 14 * 60 * 1000;
        }
        if (g_uiThreadId) PostThreadMessageW(g_uiThreadId, WM_APP_REDRAW, 0, 0);

        std::unique_lock<std::mutex> lock(g_dataMutex);
        g_cv.wait_for(lock, std::chrono::seconds(1),
                      [] { return !g_running || g_weatherRefresh || g_eventsRefresh || g_newsRefresh || !g_haCommands.empty(); });
    }
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        g_events.clear();
        g_quotes.clear();
        g_news.clear();
    }
    CoUninitialize();
}

static void DataThread() {
#ifndef WW_NO_WINRT
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
#else
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
    ULONGLONG lastBattery = 0, lastNotif = 0, lastPhoto = 0, lastNotes = 0;
    while (g_running) {
#ifndef WW_NO_WINRT
        PollMedia(mgr);
#endif
        // Only what a shown widget needs
        Settings s = GetSettings();
        ULONGLONG now = GetTickCount64();
        if (s.showBattery && (!lastBattery || now - lastBattery > 60 * 1000)) {
            ReadBatteries();
            lastBattery = now;
        }
        if (s.showNotif && (!lastNotif || now - lastNotif > 4 * 1000)) {
            ReadNotifications();
            lastNotif = now;
        }
        bool nextPhoto;
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            nextPhoto = g_photoNext;
            g_photoNext = false;
        }
        if (s.showPhotos && (nextPhoto || !lastPhoto || now - lastPhoto > 20 * 60 * 1000)) {
            PickPhoto();
            lastPhoto = now;
        }
        if (!g_running) break;
        if ((s.showNotes || s.showReminders) && (!lastNotes || now - lastNotes > 3 * 1000)) {
            ReadNotesAndReminders();
            lastNotes = now;
        }
        if (g_uiThreadId) PostThreadMessageW(g_uiThreadId, WM_APP_REDRAW, 0, 0);

        std::unique_lock<std::mutex> lock(g_dataMutex);
        g_cv.wait_for(lock, std::chrono::seconds(1), [] { return !g_running || !g_commands.empty(); });
    }
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        g_media = MediaData();  // its picture is released on this thread
        g_photo = PhotoInfo();
    }
#ifndef WW_NO_WINRT
    mgr = nullptr;
    winrt::uninit_apartment();
#else
    CoUninitialize();
#endif
}

// ---------------------------------------------------------------------------
// System stats
// ---------------------------------------------------------------------------

static ULONGLONG FtToU64(const FILETIME& f) { return ((ULONGLONG)f.dwHighDateTime << 32) | f.dwLowDateTime; }

struct SystemStats {
    double cpu = 0, mem = 0, disk = 0;
    double memUsed = 0, memTotal = 0, diskFree = 0, diskTotal = 0;
    unsigned cores = 0, processes = 0;
};
static SystemStats g_sys;

struct NetStats {
    std::atomic<double> down{0}, up{0};  // bytes per second (written on the UI thread, read by the sensors thread too)
    double histDown[60] = {}, histUp[60] = {};
    int histPos = 0;
    double totalDown = 0, totalUp = 0;
    std::wstring adapter;
};
static NetStats g_net;
struct IfCounters {
    BYTE mac[8];
    DWORD in, out;
};
static std::vector<IfCounters> g_ifPrev;
static ULONGLONG g_prevIdle = 0, g_prevTotal = 0;
static ULONGLONG g_netTick = 0;

// Download and upload of the network adapters that are up (the 32-bit counters wrap: deltas are taken per adapter)
static void UpdateNetwork() {
    ULONG size = 0;
    if (GetIfTable(nullptr, &size, FALSE) != ERROR_INSUFFICIENT_BUFFER || !size) return;
    std::vector<BYTE> buf(size);
    auto* table = (MIB_IFTABLE*)buf.data();
    if (GetIfTable(table, &size, FALSE) != NO_ERROR) return;
    // The same adapter can be listed several times (filter drivers): one entry per hardware address
    std::vector<IfCounters> now;
    std::wstring adapter;
    DWORD best = 0;
    for (DWORD i = 0; i < table->dwNumEntries; i++) {
        const MIB_IFROW& r = table->table[i];
        if (r.dwType == MIB_IF_TYPE_LOOPBACK || r.dwOperStatus != MIB_IF_OPER_STATUS_OPERATIONAL || r.dwPhysAddrLen == 0) continue;
        IfCounters c = {};
        memcpy(c.mac, r.bPhysAddr, std::min<DWORD>(r.dwPhysAddrLen, 8));
        c.in = r.dwInOctets;
        c.out = r.dwOutOctets;
        bool merged = false;
        for (auto& n : now)
            if (!memcmp(n.mac, c.mac, 8)) {
                if (c.in > n.in) n.in = c.in;
                if (c.out > n.out) n.out = c.out;
                merged = true;
            }
        if (!merged) now.push_back(c);
        if (r.dwInOctets >= best && r.dwDescrLen > 0) {
            best = r.dwInOctets;
            adapter = FromUtf8(std::string((const char*)r.bDescr, strnlen((const char*)r.bDescr, r.dwDescrLen)));
        }
    }
    ULONGLONG tick = GetTickCount64();
    if (g_netTick && tick > g_netTick) {
        double din = 0, dout = 0;
        for (auto& n : now)
            for (auto& p : g_ifPrev)
                if (!memcmp(n.mac, p.mac, 8)) {
                    din += (DWORD)(n.in - p.in);  // unsigned: right across a wrap
                    dout += (DWORD)(n.out - p.out);
                }
        double secs = (tick - g_netTick) / 1000.0;
        g_net.down = din / secs;
        g_net.up = dout / secs;
        g_net.totalDown += din;
        g_net.totalUp += dout;
        g_net.histDown[g_net.histPos] = g_net.down;
        g_net.histUp[g_net.histPos] = g_net.up;
        g_net.histPos = (g_net.histPos + 1) % 60;
    }
    g_ifPrev = now;
    g_netTick = tick;
    g_net.adapter = adapter;
}

static void UpdateSystemStats() {
    FILETIME idle, kernel, user;
    if (GetSystemTimes(&idle, &kernel, &user)) {
        ULONGLONG i = FtToU64(idle), t = FtToU64(kernel) + FtToU64(user);
        if (g_prevTotal && t > g_prevTotal) {
            double di = (double)(i - g_prevIdle), dt = (double)(t - g_prevTotal);
            g_sys.cpu = 100.0 * (1.0 - di / dt);
            if (g_sys.cpu < 0) g_sys.cpu = 0;
        }
        g_prevIdle = i;
        g_prevTotal = t;
    }
    MEMORYSTATUSEX ms = {sizeof(ms)};
    if (GlobalMemoryStatusEx(&ms)) {
        g_sys.mem = ms.dwMemoryLoad;
        g_sys.memTotal = (double)ms.ullTotalPhys;
        g_sys.memUsed = (double)(ms.ullTotalPhys - ms.ullAvailPhys);
    }
    ULARGE_INTEGER freeBytes, total, totalFree;
    if (GetDiskFreeSpaceExW(L"C:\\", &freeBytes, &total, &totalFree) && total.QuadPart) {
        g_sys.disk = 100.0 * (1.0 - (double)totalFree.QuadPart / (double)total.QuadPart);
        g_sys.diskFree = (double)totalFree.QuadPart;
        g_sys.diskTotal = (double)total.QuadPart;
    }
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    g_sys.cores = si.dwNumberOfProcessors;
    PERFORMANCE_INFORMATION pi = {sizeof(pi)};
    if (K32GetPerformanceInfo(&pi, sizeof(pi))) g_sys.processes = pi.ProcessCount;
    UpdateNetwork();
}

// ---------------------------------------------------------------------------
// Sensors: temperatures and live data of the PC, on their own thread (WMI and PDH can take a moment)
// - GPU: what Windows itself reads for Task Manager (WDDM 2.9 or newer, dedicated GPUs): temperature, fan, power
// - Load, clock, disk, video memory: Windows performance counters (PDH)
// - CPU temperature and power: Windows has no way to read them without a driver. LibreHardwareMonitor (or
//   OpenHardwareMonitor) shares its sensors in WMI while it runs; without it, the ACPI thermal zone, which on many PCs
//   is a board value, not the processor's
// ---------------------------------------------------------------------------

enum SensorId {
    S_NONE, S_CPU_TEMP, S_GPU_TEMP, S_GPU_HOTSPOT, S_CPU_LOAD, S_GPU_LOAD, S_RAM, S_VRAM, S_CPU_CLOCK, S_GPU_FAN, S_GPU_POWER,
    S_CPU_POWER, S_DISK, S_NET_DOWN, S_NET_UP, S_COUNT
};
static const PCWSTR kSensorShort[S_COUNT] = {L"",      L"CPU",   L"GPU",    L"Hotspot", L"CPU",      L"GPU",      L"RAM", L"VRAM",
                                             L"Clock", L"Fan",   L"GPU",    L"CPU",     L"Disco",    L"Download", L"Upload"};
static const PCWSTR kSensorLong[S_COUNT] = {L"",
                                            L"Temperatura da CPU",
                                            L"Temperatura da GPU",
                                            L"Hotspot da GPU",
                                            L"Uso da CPU",
                                            L"Uso da GPU",
                                            L"Memória RAM",
                                            L"Memória da GPU",
                                            L"Clock da CPU",
                                            L"Ventoinha da GPU",
                                            L"Consumo da GPU",
                                            L"Consumo da CPU",
                                            L"Atividade do disco",
                                            L"Download",
                                            L"Upload"};

static_assert(sizeof(kSensorShort) / sizeof(kSensorShort[0]) == S_COUNT, "kSensorShort");
static_assert(sizeof(kSensorLong) / sizeof(kSensorLong[0]) == S_COUNT, "kSensorLong");

struct SensorValue {
    bool have = false;
    double v = 0;     // °C, %, MHz, rpm, W, bytes/s
    double frac = 0;  // how full its ring is, 0..1
    std::wstring source;  // where a temperature came from, when it's worth saying
    float hist[60] = {};  // the last 90 seconds
    int histN = 0, histPos = 0;
};
static SensorValue g_sensors[S_COUNT];  // under g_dataMutex
static std::wstring g_sensorGpu;        // the GPU's name
static bool g_sensorLhm = false;        // LibreHardwareMonitor / OpenHardwareMonitor is answering
static std::thread g_sensorThread;

// --- The GPU Windows chose for games (the one with the most memory of its own) and its numbers from the driver ---

struct KmtOpenLuid {
    LUID AdapterLuid;
    UINT hAdapter;
};
struct KmtQuery {
    UINT hAdapter;
    UINT Type;
    void* pData;
    UINT DataSize;
};
struct KmtClose {
    UINT hAdapter;
};
struct KmtPerf {
    UINT32 PhysicalAdapterIndex;
    ULONGLONG MemoryFrequency, MaxMemoryFrequency, MaxMemoryFrequencyOC, MemoryBandwidth, PCIEBandwidth;
    ULONG FanRPM, Power, Temperature;  // rpm, tenths of a percent of its limit, tenths of °C
    UCHAR PowerStateOverride;
};
struct KmtPerfCaps {
    UINT32 PhysicalAdapterIndex;
    ULONGLONG MaxMemoryBandwidth, MaxPCIEBandwidth;
    ULONG MaxFanRPM, TemperatureMax, TemperatureWarning;
};
constexpr UINT kKmtAdapterPerf = 62, kKmtAdapterPerfCaps = 63;  // KMTQAITYPE_ADAPTERPERFDATA(_CAPS)

struct GpuState {
    bool found = false;
    LUID luid = {};
    std::wstring luidText;  // "luid_0x00000000_0x0000d1f2", as the counters name it
    double vram = 0;
    UINT hAdapter = 0;
    KmtPerfCaps caps = {};
    LONG(WINAPI* query)(const KmtQuery*) = nullptr;
};

static void FindGpu(GpuState& g) {
    HMODULE dxgi = LoadLibraryExW(L"dxgi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto create = dxgi ? (HRESULT(WINAPI*)(REFIID, void**))(void*)GetProcAddress(dxgi, "CreateDXGIFactory1") : nullptr;
    ComPtr<IDXGIFactory1> f;
    if (!create || FAILED(create(__uuidof(IDXGIFactory1), (void**)f.GetAddressOf()))) return;
    DXGI_ADAPTER_DESC1 best = {};
    bool any = false;
    ComPtr<IDXGIAdapter1> a;
    for (UINT i = 0; f->EnumAdapters1(i, &a) != DXGI_ERROR_NOT_FOUND; i++, a.Reset()) {
        DXGI_ADAPTER_DESC1 d;
        if (FAILED(a->GetDesc1(&d)) || (d.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) || d.VendorId == 0x1414) continue;
        if (!any || d.DedicatedVideoMemory > best.DedicatedVideoMemory) best = d;
        any = true;
    }
    if (!any) return;
    g.found = true;
    g.luid = best.AdapterLuid;
    g.vram = (double)best.DedicatedVideoMemory;
    wchar_t t[64];
    swprintf(t, 64, L"luid_0x%08x_0x%08x", (unsigned)best.AdapterLuid.HighPart, (unsigned)best.AdapterLuid.LowPart);
    g.luidText = t;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        g_sensorGpu = best.Description;
    }
    HMODULE gdi = GetModuleHandleW(L"gdi32.dll");
    auto open = gdi ? (LONG(WINAPI*)(KmtOpenLuid*))(void*)GetProcAddress(gdi, "D3DKMTOpenAdapterFromLuid") : nullptr;
    g.query = gdi ? (LONG(WINAPI*)(const KmtQuery*))(void*)GetProcAddress(gdi, "D3DKMTQueryAdapterInfo") : nullptr;
    KmtOpenLuid o = {best.AdapterLuid, 0};
    if (open && g.query && open(&o) == 0) {
        g.hAdapter = o.hAdapter;
        KmtQuery q = {g.hAdapter, kKmtAdapterPerfCaps, &g.caps, sizeof(g.caps)};
        if (g.query(&q) != 0) g.caps = {};
    }
}

static void CloseGpu(GpuState& g) {
    HMODULE gdi = GetModuleHandleW(L"gdi32.dll");
    auto close = gdi ? (LONG(WINAPI*)(const KmtClose*))(void*)GetProcAddress(gdi, "D3DKMTCloseAdapter") : nullptr;
    if (g.hAdapter && close) {
        KmtClose c = {g.hAdapter};
        close(&c);
    }
    g.hAdapter = 0;
}

static bool ReadGpuPerf(GpuState& g, KmtPerf* p) {
    if (!g.hAdapter || !g.query) return false;
    *p = {};
    KmtQuery q = {g.hAdapter, kKmtAdapterPerf, p, sizeof(*p)};
    return g.query(&q) == 0;
}

// --- Performance counters ---

struct PdhState {
    PDH_HQUERY q = nullptr;
    PDH_HCOUNTER perf = nullptr, freq = nullptr, diskIdle = nullptr, gpuEngine = nullptr, gpuMem = nullptr, thermal = nullptr;
    ULONGLONG built = 0;
};

static void PdhOpen(PdhState& p) {
    if (PdhOpenQueryW(nullptr, 0, &p.q) != ERROR_SUCCESS) {
        p.q = nullptr;
        return;
    }
    auto add = [&](PCWSTR path, PDH_HCOUNTER* c) {
        if (PdhAddEnglishCounterW(p.q, path, 0, c) != ERROR_SUCCESS) *c = nullptr;
    };
    add(L"\\Processor Information(_Total)\\% Processor Performance", &p.perf);
    add(L"\\Processor Information(_Total)\\Processor Frequency", &p.freq);
    add(L"\\PhysicalDisk(_Total)\\% Idle Time", &p.diskIdle);
    add(L"\\GPU Engine(*)\\Utilization Percentage", &p.gpuEngine);
    add(L"\\GPU Adapter Memory(*)\\Dedicated Usage", &p.gpuMem);
    add(L"\\Thermal Zone Information(*)\\Temperature", &p.thermal);
    PdhCollectQueryData(p.q);
    p.built = GetTickCount64();
}

static void PdhClose(PdhState& p) {
    if (p.q) PdhCloseQuery(p.q);
    p = PdhState{};
}

static bool PdhOne(PDH_HCOUNTER c, double* v) {
    PDH_FMT_COUNTERVALUE val;
    if (!c || PdhGetFormattedCounterValue(c, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, nullptr, &val) != ERROR_SUCCESS) return false;
    if (val.CStatus != PDH_CSTATUS_VALID_DATA && val.CStatus != PDH_CSTATUS_NEW_DATA) return false;
    *v = val.doubleValue;
    return true;
}

static bool PdhAll(PDH_HCOUNTER c, std::vector<std::pair<std::wstring, double>>& out) {
    DWORD size = 0, count = 0;
    if (!c || PdhGetFormattedCounterArrayW(c, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, &size, &count, nullptr) != (PDH_STATUS)PDH_MORE_DATA)
        return false;
    std::vector<BYTE> buf(size + 16);
    auto* items = (PDH_FMT_COUNTERVALUE_ITEM_W*)buf.data();
    if (PdhGetFormattedCounterArrayW(c, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, &size, &count, items) != ERROR_SUCCESS) return false;
    for (DWORD i = 0; i < count; i++)
        if (items[i].FmtValue.CStatus == PDH_CSTATUS_VALID_DATA || items[i].FmtValue.CStatus == PDH_CSTATUS_NEW_DATA)
            out.push_back({Lower(items[i].szName ? items[i].szName : L""), items[i].FmtValue.doubleValue});
    return true;
}

// --- LibreHardwareMonitor / OpenHardwareMonitor through WMI ---

constexpr GUID kClsidWbemLocator = {0x4590f811, 0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
constexpr GUID kIidWbemLocator = {0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};

struct HwReading {
    std::wstring type, name, parent;  // parent: "/amdcpu/0", "/intelcpu/0", "/gpu-nvidia/0"...
    double v = 0;
};

static bool HwConnect(ComPtr<IWbemServices>& svc) {
    svc.Reset();
    ComPtr<IWbemLocator> loc;
    if (FAILED(CoCreateInstance(kClsidWbemLocator, nullptr, CLSCTX_INPROC_SERVER, kIidWbemLocator, (void**)loc.GetAddressOf())))
        return false;
    for (PCWSTR ns : {L"ROOT\\LibreHardwareMonitor", L"ROOT\\OpenHardwareMonitor"}) {
        BSTR b = SysAllocString(ns);
        HRESULT hr = loc->ConnectServer(b, nullptr, nullptr, nullptr, 0, nullptr, nullptr, svc.GetAddressOf());
        SysFreeString(b);
        if (SUCCEEDED(hr) && svc) {
            CoSetProxyBlanket(svc.Get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL,
                              RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
            return true;
        }
        svc.Reset();
    }
    return false;
}

static std::wstring VarText(VARIANT& v) { return v.vt == VT_BSTR && v.bstrVal ? std::wstring(v.bstrVal) : L""; }

static bool HwRead(IWbemServices* svc, std::vector<HwReading>& out) {
    BSTR lang = SysAllocString(L"WQL");
    BSTR q = SysAllocString(
        L"SELECT SensorType, Name, Parent, Value FROM Sensor WHERE SensorType='Temperature' OR SensorType='Power' OR SensorType='Fan'");
    ComPtr<IEnumWbemClassObject> en;
    HRESULT hr = svc->ExecQuery(lang, q, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, en.GetAddressOf());
    SysFreeString(lang);
    SysFreeString(q);
    if (FAILED(hr) || !en) return false;
    bool any = false;
    while (true) {
        ComPtr<IWbemClassObject> o;
        ULONG n = 0;
        if (en->Next(3000, 1, o.GetAddressOf(), &n) != WBEM_S_NO_ERROR || !n) break;
        HwReading r;
        VARIANT v;
        VariantInit(&v);
        if (SUCCEEDED(o->Get(L"SensorType", 0, &v, nullptr, nullptr))) r.type = VarText(v);
        VariantClear(&v);
        if (SUCCEEDED(o->Get(L"Name", 0, &v, nullptr, nullptr))) r.name = Lower(VarText(v));
        VariantClear(&v);
        if (SUCCEEDED(o->Get(L"Parent", 0, &v, nullptr, nullptr))) r.parent = Lower(VarText(v));
        VariantClear(&v);
        if (SUCCEEDED(o->Get(L"Value", 0, &v, nullptr, nullptr))) {
            if (v.vt == VT_R4) r.v = v.fltVal;
            else if (v.vt == VT_R8) r.v = v.dblVal;
        }
        VariantClear(&v);
        out.push_back(r);
        any = true;
    }
    return any;
}

// The best sensor of a kind on a part: the first name found in 'prefer', else the highest value
static bool HwPick(const std::vector<HwReading>& all, PCWSTR type, PCWSTR part, std::initializer_list<PCWSTR> prefer, double* v,
                   bool highest = true) {
    const HwReading* best = nullptr;
    for (PCWSTR want : prefer) {
        for (auto& r : all)
            if (r.type == type && r.parent.find(part) != std::wstring::npos && r.name.find(want) != std::wstring::npos && r.v > 0) {
                best = &r;
                break;
            }
        if (best) break;
    }
    if (!best && highest)
        for (auto& r : all)
            if (r.type == type && r.parent.find(part) != std::wstring::npos && r.v > 0 && (!best || r.v > best->v)) best = &r;
    if (!best) return false;
    *v = best->v;
    return true;
}

// --- The thread ---

static void SensorThread() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    GpuState gpu;
    PdhState pdh;
    ComPtr<IWbemServices> hw;
    ULONGLONG lastHwTry = 0, lastGpuTry = 0;
    ULONGLONG prevIdle = 0, prevTotal = 0;
    double clockTop = 0, powerTop = 0;
    while (g_running) {
        Settings cfg = GetSettings();
        if (cfg.showSensors || cfg.showSystem) {  // Sistema shows the GPU's use too
            ULONGLONG now = GetTickCount64();
            if (!gpu.found && (!lastGpuTry || now - lastGpuTry > 60 * 1000)) {
                lastGpuTry = now;
                FindGpu(gpu);
            }
            if (!pdh.q || now - pdh.built > 20 * 1000) {  // made again now and then: the GPU counters come per program
                PdhClose(pdh);
                PdhOpen(pdh);
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }
            if (pdh.q) PdhCollectQueryData(pdh.q);
            SensorValue v[S_COUNT];

            std::vector<HwReading> hwr;
            if (!cfg.showSensors) hw.Reset();  // Sistema alone needs no hardware monitor
            else if (!hw && (!lastHwTry || now - lastHwTry > 20 * 1000)) {
                lastHwTry = now;
                HwConnect(hw);
            }
            if (hw && !HwRead(hw.Get(), hwr)) hw.Reset();  // closed: try again later
            bool hwOn = !hwr.empty();
            auto fromHw = [&](SensorId id, PCWSTR type, PCWSTR part, std::initializer_list<PCWSTR> prefer, bool highest = true) {
                double x;
                if (!v[id].have && HwPick(hwr, type, part, prefer, &x, highest)) {
                    v[id].have = true;
                    v[id].v = x;
                }
            };

            // CPU
            FILETIME idle, kernel, user;
            if (GetSystemTimes(&idle, &kernel, &user)) {
                ULONGLONG i = FtToU64(idle), t = FtToU64(kernel) + FtToU64(user);
                if (prevTotal && t > prevTotal) {
                    v[S_CPU_LOAD].have = true;
                    v[S_CPU_LOAD].v = std::clamp(100.0 * (1.0 - (double)(i - prevIdle) / (double)(t - prevTotal)), 0.0, 100.0);
                }
                prevIdle = i;
                prevTotal = t;
            }
            fromHw(S_CPU_TEMP, L"Temperature", L"cpu", {L"package", L"tctl", L"tdie", L"core average", L"core max"});
            if (v[S_CPU_TEMP].have) v[S_CPU_TEMP].source = L"";
            if (!v[S_CPU_TEMP].have) {
                std::vector<std::pair<std::wstring, double>> zones;
                double top = 0;
                if (PdhAll(pdh.thermal, zones))
                    for (auto& z : zones) top = std::max(top, z.second - 273.15);
                if (top > 1 && top < 150) {
                    v[S_CPU_TEMP].have = true;
                    v[S_CPU_TEMP].v = top;
                    v[S_CPU_TEMP].source = L"ACPI";
                }
            }
            fromHw(S_CPU_POWER, L"Power", L"cpu", {L"package", L"cpu cores"});
            double perf, base;
            if (PdhOne(pdh.perf, &perf) && PdhOne(pdh.freq, &base) && base > 0) {
                v[S_CPU_CLOCK].have = true;
                v[S_CPU_CLOCK].v = base * perf / 100.0;
                clockTop = std::max(clockTop, v[S_CPU_CLOCK].v);
                v[S_CPU_CLOCK].frac = clockTop > 0 ? v[S_CPU_CLOCK].v / clockTop : 0;
            }

            // Memory and disk
            MEMORYSTATUSEX ms = {sizeof(ms)};
            if (GlobalMemoryStatusEx(&ms)) {
                v[S_RAM].have = true;
                v[S_RAM].v = (double)(ms.ullTotalPhys - ms.ullAvailPhys);
                v[S_RAM].frac = ms.ullTotalPhys ? v[S_RAM].v / (double)ms.ullTotalPhys : 0;
            }
            double idlePct;
            if (PdhOne(pdh.diskIdle, &idlePct)) {
                v[S_DISK].have = true;
                v[S_DISK].v = std::clamp(100.0 - idlePct, 0.0, 100.0);
            }

            // GPU
            KmtPerf gp;
            if (gpu.found && ReadGpuPerf(gpu, &gp)) {
                if (gp.Temperature > 0) {
                    v[S_GPU_TEMP].have = true;
                    v[S_GPU_TEMP].v = gp.Temperature / 10.0;
                    if (gpu.caps.TemperatureMax > 0) v[S_GPU_TEMP].frac = v[S_GPU_TEMP].v / (gpu.caps.TemperatureMax / 10.0);
                }
                if (gp.FanRPM > 0 || gpu.caps.MaxFanRPM > 0) {  // 0 rpm is real: many fans stop when the GPU is cool
                    v[S_GPU_FAN].have = true;
                    v[S_GPU_FAN].v = gp.FanRPM;
                    v[S_GPU_FAN].frac = gp.FanRPM / (double)std::max<ULONG>(gpu.caps.MaxFanRPM, 3000);
                }
            }
            fromHw(S_GPU_TEMP, L"Temperature", L"gpu", {L"gpu core", L"core"});
            fromHw(S_GPU_HOTSPOT, L"Temperature", L"gpu", {L"hot spot", L"hotspot", L"junction"}, false);
            fromHw(S_GPU_FAN, L"Fan", L"gpu", {L"gpu fan", L"fan"});
            fromHw(S_GPU_POWER, L"Power", L"gpu", {L"package", L"ppt", L"total", L"board", L"gpu power"});
            if (v[S_GPU_POWER].have) {
                powerTop = std::max(powerTop, v[S_GPU_POWER].v);
                v[S_GPU_POWER].frac = powerTop > 0 ? v[S_GPU_POWER].v / std::max(powerTop, 60.0) : 0;
            } else if (gpu.found && gpu.hAdapter && ReadGpuPerf(gpu, &gp) && gp.Power > 0) {
                v[S_GPU_POWER].have = true;  // only how far it is from its limit: shown as %
                v[S_GPU_POWER].v = -gp.Power / 10.0;
                v[S_GPU_POWER].frac = gp.Power / 1000.0;
            }
            if (gpu.found) {
                std::vector<std::pair<std::wstring, double>> eng;
                if (PdhAll(pdh.gpuEngine, eng)) {
                    // As Task Manager: each engine's use is added over the programs, and the busiest engine counts
                    std::vector<std::pair<std::wstring, double>> perEngine;
                    for (auto& e : eng) {
                        if (e.first.find(gpu.luidText) == std::wstring::npos) continue;
                        size_t at = e.first.find(L"phys_");
                        std::wstring key = at == std::wstring::npos ? e.first : e.first.substr(at);
                        bool added = false;
                        for (auto& p : perEngine)
                            if (p.first == key) {
                                p.second += e.second;
                                added = true;
                            }
                        if (!added) perEngine.push_back({key, e.second});
                    }
                    double top = 0;
                    for (auto& p : perEngine) top = std::max(top, p.second);
                    v[S_GPU_LOAD].have = true;
                    v[S_GPU_LOAD].v = std::clamp(top, 0.0, 100.0);
                }
                std::vector<std::pair<std::wstring, double>> mem;
                if (PdhAll(pdh.gpuMem, mem)) {
                    double used = 0;
                    for (auto& m : mem)
                        if (m.first.find(gpu.luidText) != std::wstring::npos) used += m.second;
                    v[S_VRAM].have = true;
                    v[S_VRAM].v = used;
                    v[S_VRAM].frac = gpu.vram > 0 ? used / gpu.vram : 0;
                }
            }

            // Network (measured by the UI thread every second)
            v[S_NET_DOWN].have = v[S_NET_UP].have = true;
            v[S_NET_DOWN].v = g_net.down;
            v[S_NET_UP].v = g_net.up;
            static double downTop = 0, upTop = 0;
            downTop = std::max({downTop * 0.995, v[S_NET_DOWN].v, 1e6});
            upTop = std::max({upTop * 0.995, v[S_NET_UP].v, 2e5});
            v[S_NET_DOWN].frac = v[S_NET_DOWN].v / downTop;
            v[S_NET_UP].frac = v[S_NET_UP].v / upTop;

            for (SensorId id : {S_CPU_TEMP, S_GPU_TEMP, S_GPU_HOTSPOT})
                if (v[id].have && v[id].frac <= 0) v[id].frac = v[id].v / 100.0;
            for (SensorId id : {S_CPU_LOAD, S_GPU_LOAD, S_DISK})
                if (v[id].have) v[id].frac = v[id].v / 100.0;
            if (v[S_CPU_POWER].have) v[S_CPU_POWER].frac = v[S_CPU_POWER].v / 150.0;

            std::lock_guard<std::mutex> lock(g_dataMutex);
            g_sensorLhm = hwOn;
            for (int i = 1; i < S_COUNT; i++) {
                SensorValue& s = g_sensors[i];
                float hist[60];
                memcpy(hist, s.hist, sizeof(hist));
                int n = s.histN, pos = s.histPos;
                s = v[i];
                memcpy(s.hist, hist, sizeof(hist));
                s.histN = n;
                s.histPos = pos;
                s.frac = std::clamp(s.frac, 0.0, 1.0);
                if (s.have) {
                    s.hist[s.histPos] = (float)std::fabs(s.v);
                    s.histPos = (s.histPos + 1) % 60;
                    s.histN = std::min(60, s.histN + 1);
                }
            }
        }
        std::unique_lock<std::mutex> lock(g_dataMutex);
        g_cv.wait_for(lock, std::chrono::milliseconds(1500), [] { return !g_running; });
    }
    hw.Reset();
    PdhClose(pdh);
    CloseGpu(gpu);
    CoUninitialize();
}

static std::wstring UptimeText() {
    ULONGLONG m = GetTickCount64() / 60000;
    ULONGLONG d = m / 1440, h = (m / 60) % 24, mi = m % 60;
    wchar_t buf[64];
    if (d > 0) swprintf(buf, 64, TF(L"ligado há %llu d %llu h"), d, h);
    else swprintf(buf, 64, TF(L"ligado há %llu h %llu min"), h, mi);
    return buf;
}

// ---------------------------------------------------------------------------
// Widgets: windows
// ---------------------------------------------------------------------------

enum WidgetKind {
    W_CLOCK, W_WEATHER, W_MUSIC, W_SYSTEM, W_CALENDAR, W_WORLD, W_SUN, W_BATTERY, W_NETWORK, W_PHOTOS, W_NOTIF,
    W_NOTES, W_REMINDERS, W_SHORTCUTS, W_STOCKS, W_NEWS, W_SCREENTIME, W_HOME, W_SENSORS,
    W_SETTINGS, W_PICKER, W_MENU, W_GHOST, W_GRID
};

static const float kMargin = 28;   // DIPs around the glass for its shadow and backlight
static const float kCapture = 64;  // DIPs of background taken around the glass (the refraction pulls it in from there)

struct Widget {
    WidgetKind kind = W_CLOCK;
    PCWSTR key = L"";
    float w = 0, h = 0;        // glass size in DIPs
    float defX = 0, defY = 0;  // default offset from the work area, in DIPs
    HWND hwnd = nullptr;
    float scale = 1.0f;
    int size = 1;            // 0 small, 1 medium, 2 large
    std::shared_ptr<Image> shownImg;  // the picture it shows (album art, photo) and its bitmap
    ComPtr<ID2D1Bitmap> imgBmp;
    std::vector<std::pair<std::wstring, ComPtr<ID2D1Bitmap>>> iconBmps;  // shortcut icons
    int newsFirst = 0, newsStep = 1;  // News: the first story shown (the mouse wheel moves it) and how many fit
    ULONGLONG newsWheel = 0;          // when the wheel last moved it
    int hoverBtn = -1, pressBtn = -1;  // Music: the button the mouse is on / held down
    std::vector<std::pair<D2D1_RECT_F, std::wstring>> actions;           // what a click does where (DIPs), set when drawn
    int col = -1, row = -1;  // its cell on the grid (top-left one it covers)

    // Drawing surface: a DIB that UpdateLayeredWindow shows with its per-pixel alpha
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    void* bits = nullptr;
    int pw = 0, ph = 0;
    ComPtr<ID2D1DCRenderTarget> rt;
    ComPtr<ID2D1DeviceContext> dc;

    // The picture of what is behind the glass
    std::vector<UINT32> pixels;
    int snapW = 0, snapH = 0;
    float capOffX = 0, capOffY = 0;  // where the picture starts, in window pixels
    bool fresh = false;
    UINT64 capHash = 0;    // of the last picture taken behind it: the same picture doesn't rebuild the glass
    ULONGLONG capTick = 0;
    float lum = -1;
    ComPtr<ID2D1Bitmap> snap;
    ComPtr<ID2D1Effect> source, blurFx, mapXf, disp, color;
    ComPtr<ID2D1Bitmap> map;
    int mapKey[5] = {};
};

static std::vector<Widget*> g_widgets;
// Not destroyed at process exit: the process ends during the UI thread's teardown (see WhTool_ModUninit), and the
// single-threaded factory must not be released from the exiting thread
[[clang::no_destroy]] static ComPtr<ID2D1Factory1> g_d2d;
[[clang::no_destroy]] static ComPtr<IDWriteFactory> g_dw;
static Settings g_ui;  // the UI thread's copy of the settings
static const wchar_t kClassName[] = L"MacOSWidgets_Widget";
static UINT_PTR g_secondTimer = 0, g_animTimer = 0, g_glassTimer = 0;

// While one widget is drawn
static ID2D1RenderTarget* g_t = nullptr;
static Widget* g_cw = nullptr;
static float g_s = 1.0f;
static bool g_darkText = false;


constexpr GUID kClsidGaussianBlur = {0x1feb6d69, 0x2fe6, 0x4ac9, {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};
constexpr GUID kClsidAffine = {0x6aa97485, 0x6354, 0x4cfc, {0x90, 0x8c, 0xe4, 0xa7, 0x4f, 0x62, 0xc9, 0x6c}};
constexpr GUID kClsidDisplacement = {0xedc48364, 0x0417, 0x4111, {0x94, 0x50, 0x43, 0x84, 0x5f, 0xa9, 0xf8, 0x90}};
constexpr GUID kClsidColorMatrix = {0x921f03d6, 0x641c, 0x47df, {0x85, 0x2d, 0xb4, 0xbb, 0x61, 0x53, 0xae, 0x11}};

// ---------------------------------------------------------------------------
// Drawing helpers
// ---------------------------------------------------------------------------

static D2D1_COLOR_F Alpha(D2D1_COLOR_F c, float a) {
    c.a = a;
    return c;
}
static D2D1_COLOR_F ColorOf(COLORREF c, float a = 1.0f) {
    return D2D1::ColorF(GetRValue(c) / 255.f, GetGValue(c) / 255.f, GetBValue(c) / 255.f, a);
}
static D2D1_COLOR_F White(float a) { return D2D1::ColorF(1, 1, 1, a); }
// Text and lines: white on dark glass, near-black on light glass
static D2D1_COLOR_F Ink(float a) { return g_darkText ? D2D1::ColorF(0.07f, 0.07f, 0.08f, a) : White(a); }
static float Luma(const D2D1_COLOR_F& c) { return 0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b; }
static double Now() { return GetTickCount64() / 1000.0; }

static ComPtr<ID2D1SolidColorBrush> Solid(D2D1_COLOR_F c) {
    ComPtr<ID2D1SolidColorBrush> b;
    g_t->CreateSolidColorBrush(c, &b);
    return b;
}

static ComPtr<ID2D1LinearGradientBrush> MakeGradient(const D2D1_GRADIENT_STOP* stops, UINT32 count, D2D1_POINT_2F from,
                                                     D2D1_POINT_2F to) {
    ComPtr<ID2D1GradientStopCollection> coll;
    ComPtr<ID2D1LinearGradientBrush> brush;
    if (SUCCEEDED(g_t->CreateGradientStopCollection(stops, count, &coll)))
        g_t->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(from, to), coll.Get(), &brush);
    return brush;
}

static ComPtr<ID2D1RadialGradientBrush> MakeRadial(const D2D1_GRADIENT_STOP* stops, UINT32 count, D2D1_POINT_2F center,
                                                   float radius) {
    ComPtr<ID2D1GradientStopCollection> coll;
    ComPtr<ID2D1RadialGradientBrush> brush;
    if (SUCCEEDED(g_t->CreateGradientStopCollection(stops, count, &coll)))
        g_t->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties(center, D2D1::Point2F(0, 0), radius, radius), coll.Get(), &brush);
    return brush;
}

static void Fill(const D2D1_RECT_F& r, D2D1_COLOR_F c, float radius) {
    if (auto b = Solid(c)) g_t->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), b.Get());
}

// Everything but a rounded rectangle (the area a shadow or a glow may paint)
static ComPtr<ID2D1PathGeometry> OutsideOf(const D2D1_RECT_F& r, float radius) {
    ComPtr<ID2D1RectangleGeometry> area;
    ComPtr<ID2D1RoundedRectangleGeometry> hole;
    ComPtr<ID2D1PathGeometry> geo;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(g_d2d->CreateRectangleGeometry(D2D1::RectF(-10000, -10000, 10000, 10000), &area)) ||
        FAILED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, radius, radius), &hole)) ||
        FAILED(g_d2d->CreatePathGeometry(&geo)) || FAILED(geo->Open(&sink)))
        return nullptr;
    area->CombineWithGeometry(hole.Get(), D2D1_COMBINE_MODE_EXCLUDE, nullptr, sink.Get());
    sink->Close();
    return geo;
}

// ---------------------------------------------------------------------------
// The glass of Siri Plus Launchpad (same recipes, one shape per widget)
// ---------------------------------------------------------------------------

static bool FlatGlass() { return g_ui.theme == Theme::Frost || g_ui.refraction <= 0; }
static bool LiquidWanted() {
    Theme t = g_ui.theme;
    if (t == Theme::Frost || t == Theme::Glass) return true;
    bool glassTheme = t == Theme::Siri || t == Theme::Tinted ||
                      ((t == Theme::Light || t == Theme::Dark || t == Theme::Oled) && g_ui.glassEffect);
    return glassTheme && g_ui.refraction > 0;
}
static bool LiquidActive() { return LiquidWanted() && g_cw->snapW > 0 && g_cw->dc; }
static float GlassSigma(float s, bool flat) {
    return flat ? g_ui.blur / 100.0f * 24 * s : std::pow(g_ui.blur / 100.0f, 2.0f) * 4 * s;
}
static bool g_noBacklight = false;  // while the settings window is drawn: it never glows
static bool BacklightOn() { return g_ui.backlight && g_ui.backlightIntensity > 0 && !g_noBacklight; }

static ID2D1Bitmap* SnapBitmap(Widget* wd) {
    if (wd->snapW <= 0) return nullptr;
    if (wd->snap) {
        D2D1_SIZE_U sz = wd->snap->GetPixelSize();
        if ((int)sz.width != wd->snapW || (int)sz.height != wd->snapH) wd->snap.Reset();
    }
    D2D1_BITMAP_PROPERTIES bp =
        D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    if (!wd->snap) {
        g_t->CreateBitmap(D2D1::SizeU(wd->snapW, wd->snapH), wd->pixels.data(), wd->snapW * 4, bp, &wd->snap);
        wd->fresh = false;
    } else if (wd->fresh) {
        wd->snap->CopyFromMemory(nullptr, wd->pixels.data(), wd->snapW * 4);
        wd->fresh = false;
    }
    return wd->snap.Get();
}

static bool EnsureEffects(Widget* wd) {
    if (wd->color) return true;
    ID2D1DeviceContext* dc = wd->dc.Get();
    if (FAILED(dc->CreateEffect(kClsidAffine, &wd->source)) || FAILED(dc->CreateEffect(kClsidGaussianBlur, &wd->blurFx)) ||
        FAILED(dc->CreateEffect(kClsidAffine, &wd->mapXf)) || FAILED(dc->CreateEffect(kClsidDisplacement, &wd->disp)) ||
        FAILED(dc->CreateEffect(kClsidColorMatrix, &wd->color))) {
        wd->color.Reset();
        return false;
    }
    wd->blurFx->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
    wd->mapXf->SetValue(D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE, D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_LINEAR);
    wd->disp->SetValue(D2D1_DISPLACEMENTMAP_PROP_X_CHANNEL_SELECT, D2D1_CHANNEL_SELECTOR_R);
    wd->disp->SetValue(D2D1_DISPLACEMENTMAP_PROP_Y_CHANNEL_SELECT, D2D1_CHANNEL_SELECTOR_G);
    // A touch more saturated and brighter, like light passing through glass
    float sat = 1.12f, gain = 1.03f, lw[3] = {0.2126f, 0.7152f, 0.0722f};
    D2D1_MATRIX_5X4_F cm{};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) cm.m[i][j] = ((1 - sat) * lw[i] + (i == j ? sat : 0.0f)) * gain;
    cm.m[3][3] = 1;
    for (int j = 0; j < 3; j++) cm.m[4][j] = 0.015f;
    wd->color->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, cm);
    return true;
}

// Displacement map of a rounded rectangle (half resolution): R/G = where to look, around 0.5 = right here.
// Within `band` of the edge the picture is taken from up to `depth` pixels further out, more right at the edge.
static ID2D1Bitmap* MapFor(Widget* wd, float w, float h, float radius, float band, float depth) {
    int mw = std::max(1, (int)std::ceil(w / 2)), mh = std::max(1, (int)std::ceil(h / 2));
    int key[5] = {mw, mh, (int)std::lround(radius), (int)std::lround(band), (int)std::lround(depth)};
    if (wd->map && !memcmp(key, wd->mapKey, sizeof(key))) return wd->map.Get();
    std::vector<UINT32> px((size_t)mw * mh);
    float hw = mw * 0.5f, hh = mh * 0.5f;
    float r = std::min({radius / 2, hw, hh}), b = std::max(1.0f, band / 2), dep = std::max(0.5f, depth / 2);
    for (int y = 0; y < mh; y++) {
        for (int x = 0; x < mw; x++) {
            float px_ = x + 0.5f - hw, py = y + 0.5f - hh;
            float qx = std::fabs(px_) - (hw - r), qy = std::fabs(py) - (hh - r);
            float ox = std::max(qx, 0.0f), oy = std::max(qy, 0.0f), len = std::sqrt(ox * ox + oy * oy);
            float dist = len + std::min(std::max(qx, qy), 0.0f) - r;  // < 0 inside
            float nx, ny;
            if (qx > 0 && qy > 0 && len > 0) {
                nx = ox / len * (px_ < 0 ? -1.0f : 1.0f);
                ny = oy / len * (py < 0 ? -1.0f : 1.0f);
            } else if (qx > qy) {
                nx = px_ < 0 ? -1.0f : 1.0f;
                ny = 0;
            } else {
                nx = 0;
                ny = py < 0 ? -1.0f : 1.0f;
            }
            float t = std::clamp(1.0f + dist / b, 0.0f, 1.0f);  // 1 at the edge, 0 deeper than the band
            float k = dep * std::pow(t, 2.2f);
            int R = std::clamp((int)std::lround(255 * (0.5f + nx * k / (2 * dep))), 0, 255);
            int G = std::clamp((int)std::lround(255 * (0.5f + ny * k / (2 * dep))), 0, 255);
            px[(size_t)y * mw + x] = 0xFF000000u | ((UINT32)R << 16) | ((UINT32)G << 8);
        }
    }
    wd->map.Reset();
    if (FAILED(g_t->CreateBitmap(D2D1::SizeU(mw, mh), px.data(), mw * 4,
                                 D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                                                          D2D1_ALPHA_MODE_PREMULTIPLIED)),
                                 &wd->map)))
        return nullptr;
    memcpy(wd->mapKey, key, sizeof(key));
    return wd->map.Get();
}

// The picture behind the glass, blurred and (with `bend`) refracted at the edges. The caller has clipped to the shape.
static bool DrawGlassPicture(const D2D1_RECT_F& r, float radius, bool bend) {
    Widget* wd = g_cw;
    if (!wd->dc) return false;
    ID2D1Bitmap* snap = SnapBitmap(wd);
    if (!snap || !EnsureEffects(wd)) return false;
    float w = r.right - r.left, h = r.bottom - r.top, s = g_s;
    if (w < 2 || h < 2) return false;
    float extra = std::clamp((g_ui.refraction - 60) / 40.0f, 0.0f, 1.0f);
    float band = std::min(std::min(w, h) * (0.42f + 0.06f * extra), (30 + 50 * extra) * s);
    float depth = band * g_ui.refraction / 100.0f * 1.6f * (1 + 0.5f * extra);
    depth = std::min(depth, kCapture * s);  // only that much picture is taken around the glass
    float sigma = GlassSigma(s, !bend);

    wd->source->SetInput(0, snap);
    wd->source->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX,
                         D2D1::Matrix3x2F::Translation(wd->capOffX, wd->capOffY));
    ComPtr<ID2D1Image> picture;
    wd->source->GetOutput(&picture);
    if (sigma > 0.3f) {
        wd->blurFx->SetInputEffect(0, wd->source.Get());
        wd->blurFx->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, sigma);
        picture.Reset();
        wd->blurFx->GetOutput(&picture);
    }
    if (bend && depth >= 0.5f) {
        if (ID2D1Bitmap* map = MapFor(wd, w, h, radius, band, depth)) {
            D2D1_SIZE_U ms = map->GetPixelSize();
            wd->mapXf->SetInput(0, map);
            wd->mapXf->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX,
                                D2D1::Matrix3x2F::Scale(w / ms.width, h / ms.height) *
                                    D2D1::Matrix3x2F::Translation(r.left, r.top));
            wd->disp->SetInput(0, picture.Get());
            wd->disp->SetInputEffect(1, wd->mapXf.Get());
            wd->disp->SetValue(D2D1_DISPLACEMENTMAP_PROP_SCALE, 2 * std::max(0.5f, depth / 2) * (w / ms.width));
            picture.Reset();
            wd->disp->GetOutput(&picture);
        }
    }
    wd->color->SetInput(0, picture.Get());
    ComPtr<ID2D1Image> out;
    wd->color->GetOutput(&out);
    wd->dc->DrawImage(out.Get(), D2D1::Point2F(r.left, r.top), r, D2D1_INTERPOLATION_MODE_LINEAR);
    return true;
}

static void DrawThemeShadow(const D2D1_RECT_F& r, float radius, float strength, float spread, float dy) {
    auto outside = OutsideOf(r, radius);
    if (!outside) return;
    g_t->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), outside.Get()), nullptr);
    const int steps = 10;
    for (int i = steps; i >= 1; i--) {
        float g = spread * i / steps;
        Fill(D2D1::RectF(r.left - g, r.top - g + dy, r.right + g, r.bottom + g + dy), D2D1::ColorF(0, 0, 0, strength / steps),
             radius + g);
    }
    g_t->PopLayer();
}

// The backlight's colors as one smooth gradient whose direction turns (a full turn every 7 s)
static ComPtr<ID2D1LinearGradientBrush> BacklightBrush(const D2D1_RECT_F& r, float extra) {
    const float ang = (float)(std::fmod(Now() / 7.0, 1.0) * 2 * 3.14159265);
    const float dx = std::cos(ang), dy = std::sin(ang);
    const float cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    const float half = std::fabs(dx) * (r.right - r.left) / 2 + std::fabs(dy) * (r.bottom - r.top) / 2 + extra;
    D2D1_GRADIENT_STOP stops[7];
    UINT32 count;
    int n = g_ui.backlightCount;
    if (n <= 1) {
        const D2D1_COLOR_F c = ColorOf(g_ui.backlightColors[0]);
        stops[0] = {0.0f, Alpha(c, 0.25f)};
        stops[1] = {0.55f, Alpha(c, 0.55f)};
        stops[2] = {1.0f, Alpha(c, 1.0f)};
        count = 3;
    } else {
        for (int i = 0; i <= n; i++) stops[i] = {(float)i / n, ColorOf(g_ui.backlightColors[i % n])};
        count = n + 1;
    }
    return MakeGradient(stops, count, D2D1::Point2F(cx - dx * half, cy - dy * half),
                        D2D1::Point2F(cx + dx * half, cy + dy * half));
}

static void DrawBacklightGlow(const D2D1_RECT_F& r, float radius, float spread) {
    if (!BacklightOn()) return;
    auto outside = OutsideOf(r, radius);
    auto brush = BacklightBrush(r, spread);
    if (!outside || !brush) return;
    const float k = g_ui.backlightIntensity / 100.0f;
    g_t->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), outside.Get()), nullptr);
    const int steps = 14;
    for (int i = steps; i >= 1; i--) {
        float g = spread * i / steps;
        brush->SetOpacity(std::min(1.0f, 1.1f * k / steps));
        g_t->FillRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(r.left - g, r.top - g, r.right + g, r.bottom + g), radius + g, radius + g),
            brush.Get());
    }
    g_t->PopLayer();
}

static void DrawBacklightRim(const D2D1_RECT_F& r, float radius) {
    if (!BacklightOn()) return;
    float s = g_s, k = g_ui.backlightIntensity / 100.0f, bw = std::max(1.0f, 1.4f * s), half = bw / 2;
    auto brush = BacklightBrush(r, 0);
    if (!brush) return;
    D2D1_ROUNDED_RECT edge = D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half),
                                               std::max(0.0f, radius - half), std::max(0.0f, radius - half));
    brush->SetOpacity(std::min(1.0f, 0.95f * k + 0.05f));
    g_t->DrawRoundedRectangle(edge, brush.Get(), bw);
    if (auto b = Solid(White(0.12f * k))) g_t->DrawRoundedRectangle(edge, b.Get(), bw * 0.6f);
}

// Points along a rounded rectangle's edge, clockwise from the middle of the top: u = 0..1 of the way around
struct EdgeWalk {
    D2D1_RECT_F r;
    float rad, straightW, straightH, arc, total;
    EdgeWalk(const D2D1_RECT_F& rect, float radius) : r(rect) {
        rad = std::min(radius, std::min(r.right - r.left, r.bottom - r.top) / 2);
        straightW = std::max(0.0f, r.right - r.left - 2 * rad);
        straightH = std::max(0.0f, r.bottom - r.top - 2 * rad);
        arc = 3.14159265f * rad / 2;
        total = std::max(1.0f, 2 * straightW + 2 * straightH + 4 * arc);
    }
    D2D1_POINT_2F At(float u) const {
        float d = (u - std::floor(u)) * total;
        float cx0 = r.left + rad, cx1 = r.right - rad, cy0 = r.top + rad, cy1 = r.bottom - rad;
        auto corner = [&](float cx, float cy, float a0, float t) {
            float a = (a0 + 90.0f * t) * 3.14159265f / 180;
            return D2D1::Point2F(cx + rad * std::cos(a), cy + rad * std::sin(a));
        };
        float half = straightW / 2;
        if (d < half) return D2D1::Point2F((cx0 + cx1) / 2 + d, r.top);
        d -= half;
        if (d < arc) return corner(cx1, cy0, -90, d / arc);
        d -= arc;
        if (d < straightH) return D2D1::Point2F(r.right, cy0 + d);
        d -= straightH;
        if (d < arc) return corner(cx1, cy1, 0, d / arc);
        d -= arc;
        if (d < straightW) return D2D1::Point2F(cx1 - d, r.bottom);
        d -= straightW;
        if (d < arc) return corner(cx0, cy1, 90, d / arc);
        d -= arc;
        if (d < straightH) return D2D1::Point2F(r.left, cy1 - d);
        d -= straightH;
        if (d < arc) return corner(cx0, cy0, 180, d / arc);
        d -= arc;
        return D2D1::Point2F(cx0 + std::min(d, half), r.top);
    }
};

// A chrome edge whose light runs around it: two highlights on opposite sides travelling slowly
static void DrawChromeEdge(const D2D1_RECT_F& r, float radius, float a, float turnsPerSecond) {
    float s = g_s, w = std::max(1.2f, 1.6f * s), in = w / 2 + 0.3f * s;
    D2D1_RECT_F er = D2D1::RectF(r.left + in, r.top + in, r.right - in, r.bottom - in);
    float rr = std::max(0.0f, std::min(radius - in, std::min(er.right - er.left, er.bottom - er.top) / 2));
    D2D1_ROUNDED_RECT edge = D2D1::RoundedRect(er, rr, rr);
    const bool lightSurface = g_darkText;
    if (auto b = Solid(D2D1::ColorF(0, 0, 0, (lightSurface ? 0.10f : 0.22f) * a)))
        g_t->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(er.left - w * 0.5f, er.top - w * 0.5f, er.right + w * 0.5f,
                                                                er.bottom + w * 0.5f),
                                                    rr + w * 0.5f, rr + w * 0.5f),
                                  b.Get(), std::max(1.0f, 0.6f * s));
    if (auto b = Solid(lightSurface ? D2D1::ColorF(0.18f, 0.20f, 0.24f, 0.30f * a) : White(0.16f * a)))
        g_t->DrawRoundedRectangle(edge, b.Get(), w);
    EdgeWalk e(er, rr);
    const float phase = (float)std::fmod(Now() * turnsPerSecond, 1.0);
    D2D1_GRADIENT_STOP spot[5] = {{0.0f, D2D1::ColorF(1, 1, 1, 1.0f)},
                                  {0.30f, D2D1::ColorF(0.96f, 0.98f, 1, 0.85f)},
                                  {0.55f, D2D1::ColorF(0.62f, 0.80f, 1, 0.50f)},
                                  {0.78f, D2D1::ColorF(1, 0.86f, 0.58f, 0.22f)},
                                  {1.0f, D2D1::ColorF(1, 1, 1, 0.0f)}};
    if (lightSurface) {
        spot[0] = {0.0f, D2D1::ColorF(0.10f, 0.11f, 0.14f, 0.95f)};
        spot[1] = {0.30f, D2D1::ColorF(0.22f, 0.25f, 0.32f, 0.80f)};
        spot[2] = {0.55f, D2D1::ColorF(0.10f, 0.38f, 0.85f, 0.50f)};
        spot[3] = {0.78f, D2D1::ColorF(0.80f, 0.50f, 0.10f, 0.25f)};
        spot[4] = {1.0f, D2D1::ColorF(0.1f, 0.1f, 0.1f, 0.0f)};
    }
    float size = std::clamp(e.total * 0.09f, 9 * s, 80 * s);
    for (int k = 0; k < 2; k++) {
        D2D1_POINT_2F c = e.At(phase + 0.5f * k);
        if (auto b = MakeRadial(spot, 5, c, size * (k ? 0.85f : 1.0f))) {
            b->SetOpacity(a * (k ? 0.6f : 1.0f));
            g_t->DrawRoundedRectangle(edge, b.Get(), w);
        }
    }
}

// Glass and Liquid glass: down to 80 % transparency the glass is the clear smoky one as designed. Below that the
// smoke fades out and a thin white veil takes its place, and the edge turns into a bright fine line
static float ClearGlassK(int transparency) { return std::clamp((80.0f - transparency) / 80.0f, 0.0f, 1.0f); }
static void ClearGlassBody(int transparency, D2D1_GRADIENT_STOP out[3]) {
    const float k = ClearGlassK(transparency), smoke = std::min((100 - transparency) / 100.0f * 0.6f, 0.12f) * (1 - k);
    const float white = 0.30f * k;
    auto mix = [&](float sm, float wh, float g) {
        float a = sm + wh * (1 - sm);
        float c = a > 0 ? (wh * 1.0f + sm * (1 - wh) * g) / a : 1.0f;
        return D2D1::ColorF(c, c, std::min(1.0f, c + 0.01f * k), a);
    };
    out[0] = {0.0f, mix(smoke * 0.85f, white * 1.15f, 0.07f)};
    out[1] = {0.5f, mix(smoke, white, 0.04f)};
    out[2] = {1.0f, mix(smoke * 0.9f, white * 0.85f, 0.06f)};
}
static void ClearGlassRim(int transparency, D2D1_GRADIENT_STOP rim[7]) {
    const float k = ClearGlassK(transparency);
    const D2D1_COLOR_F bright[7] = {White(0.62f), White(0.26f), White(0.16f), White(0.14f),
                                    White(0.18f), White(0.30f), White(0.50f)};
    for (int i = 0; i < 7; i++) {
        const D2D1_COLOR_F a = rim[i].color, b = bright[i];
        rim[i].color = D2D1::ColorF(a.r + (b.r - a.r) * k, a.g + (b.g - a.g) * k, a.b + (b.b - a.b) * k, a.a + (b.a - a.a) * k);
    }
}
static void SoftRim(D2D1_GRADIENT_STOP rim[7]) {
    const D2D1_GRADIENT_STOP v[7] = {
        {0.0f, D2D1::ColorF(0.83f, 0.83f, 0.83f, 0.44f)}, {0.1f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.31f)},
        {0.25f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.38f)}, {0.5f, D2D1::ColorF(0.13f, 0.13f, 0.13f, 0.44f)},
        {0.75f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.56f)}, {0.9f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.56f)},
        {1.0f, D2D1::ColorF(0.76f, 0.76f, 0.76f, 0.44f)},
    };
    for (int i = 0; i < 7; i++) rim[i] = v[i];
}
static void StrokeInside(const D2D1_RECT_F& r, float radius, ID2D1Brush* b, float bw) {
    float half = bw / 2, rr = std::max(0.0f, radius - half);
    g_t->DrawRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half), rr, rr), b, bw);
}

// A liquid glass shape: the refracted background with the same body and edge as the frosted glass
static void DrawLiquidShape(const D2D1_RECT_F& r, float radius, bool shadow) {
    float s = g_s, h = r.bottom - r.top, w = r.right - r.left;
    radius = std::min(radius, std::min(w, h) / 2);
    if (shadow) DrawThemeShadow(r, radius, 0.16f, 10 * s, 4 * s);
    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    if (FAILED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, radius, radius), &clip))) return;
    g_t->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get()), nullptr);
    DrawGlassPicture(r, radius, !FlatGlass());
    D2D1_GRADIENT_STOP body[3];
    ClearGlassBody(g_ui.transparency, body);
    if (auto brush = MakeGradient(body, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))) g_t->FillRectangle(r, brush.Get());
    float glowH = std::min(h * 0.22f, 14 * s);
    D2D1_GRADIENT_STOP glow[2] = {{0.0f, White(0.07f)}, {1.0f, White(0.0f)}};
    if (auto top = MakeGradient(glow, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.top + glowH)))
        g_t->FillRectangle(D2D1::RectF(r.left, r.top, r.right, r.top + glowH), top.Get());
    g_t->PopLayer();
    D2D1_GRADIENT_STOP rim[7];
    SoftRim(rim);
    ClearGlassRim(g_ui.transparency, rim);
    if (auto b = MakeGradient(rim, 7, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom)))
        StrokeInside(r, radius, b.Get(), std::max(1.0f, 1.2f * s));
}

static void DrawThemedShapeBody(const D2D1_RECT_F& r, float radius, bool shadow) {
    Theme theme = g_ui.theme;
    if (theme == Theme::Frost) theme = Theme::Glass;  // Glass is drawn like Liquid glass, without bending
    const int transparency = g_ui.transparency;
    float s = g_s, h = r.bottom - r.top;
    if (h < 1 || r.right - r.left < 1) return;
    if (theme == Theme::Glass && LiquidActive()) {
        DrawLiquidShape(r, radius, shadow);
        return;
    }
    if (shadow) {
        if (theme == Theme::Glass) DrawThemeShadow(r, radius, 0.16f, 10 * s, 4 * s);
        else
            DrawThemeShadow(r, radius,
                            theme == Theme::Light ? 0.14f : (theme == Theme::Oled || theme == Theme::Tinted) ? 0.30f : 0.18f,
                            10 * s, 4 * s);
    }

    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    if (FAILED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, radius, radius), &clip))) return;
    g_t->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get()), nullptr);
    if (theme != Theme::Glass && LiquidActive()) DrawGlassPicture(r, radius, !FlatGlass());
    else if (g_ui.blur > 0) DrawGlassPicture(r, radius, false);  // the frosted background (the "backdrop" in Siri Plus)

    D2D1_GRADIENT_STOP lightBody[3] = {
        {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.52f)},
        {0.5f, D2D1::ColorF(0.97f, 0.97f, 0.99f, 0.40f)},
        {1.0f, D2D1::ColorF(0.93f, 0.94f, 0.97f, 0.34f)},
    };
    D2D1_GRADIENT_STOP glassBody[3];
    ClearGlassBody(transparency, glassBody);
    D2D1_GRADIENT_STOP siriBody[3] = {
        {0.0f, D2D1::ColorF(0.05f, 0.05f, 0.06f, 0.80f)},
        {0.55f, D2D1::ColorF(0.11f, 0.11f, 0.13f, 0.54f)},
        {1.0f, D2D1::ColorF(0.30f, 0.29f, 0.34f, 0.26f)},
    };
    // Custom glass: the color is solid at the top and gives way to clear glass further down
    const D2D1_COLOR_F bc = ColorOf(g_ui.tintColor);
    float tintTop = g_ui.tintLevel / 100.0f;
    float tintBottom = std::min(tintTop, (100 - transparency) / 100.0f * 0.8f);
    float reach = std::clamp(g_ui.tintReach / 100.0f, 0.0f, 1.0f), soft = 0.48f;
    constexpr int kTintStops = 25;
    D2D1_GRADIENT_STOP tintBody[kTintStops];
    for (int i = 0; i < kTintStops; i++) {
        float t = i / (float)(kTintStops - 1);
        float u = std::clamp((t - (reach - soft)) / (2 * soft), 0.0f, 1.0f);
        float e = u * u * u * (u * (u * 6 - 15) + 10);  // smootherstep
        tintBody[i] = {t, Alpha(bc, tintTop + (tintBottom - tintTop) * e)};
    }
    D2D1_GRADIENT_STOP darkBody[3] = {
        {0.0f, D2D1::ColorF(0.16f, 0.16f, 0.17f, 0.74f)},
        {0.5f, D2D1::ColorF(0.12f, 0.12f, 0.13f, 0.72f)},
        {1.0f, D2D1::ColorF(0.11f, 0.11f, 0.12f, 0.74f)},
    };
    float oledA = 1.0f - transparency / 100.0f * 0.6f;
    D2D1_GRADIENT_STOP oledBody[3] = {
        {0.0f, D2D1::ColorF(0, 0, 0, oledA)}, {0.5f, D2D1::ColorF(0, 0, 0, oledA)}, {1.0f, D2D1::ColorF(0, 0, 0, oledA)}};
    if (g_ui.glassEffect) {
        for (int i = 0; i < 3; i++) {
            darkBody[i].color.a *= 0.78f;
            oledBody[i].color.a = std::min(oledBody[i].color.a, 0.80f);
        }
    }
    float density = std::clamp((100 - transparency) / 20.0f, 0.0f, 2.5f);
    for (auto* stops : {lightBody, siriBody, darkBody})
        for (int i = 0; i < 3; i++) stops[i].color.a = std::min(1.0f, stops[i].color.a * density);
    const D2D1_GRADIENT_STOP* body = theme == Theme::Light ? lightBody : theme == Theme::Glass ? glassBody
                                   : theme == Theme::Dark  ? darkBody
                                   : theme == Theme::Oled  ? oledBody
                                                           : siriBody;
    if (auto brush = theme == Theme::Tinted
                         ? MakeGradient(tintBody, kTintStops, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))
                         : MakeGradient(body, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom)))
        g_t->FillRectangle(r, brush.Get());
    // A fully clear glass would let the mouse fall through (pixels with no alpha are not hit)
    if (auto b = Solid(D2D1::ColorF(0, 0, 0, 1 / 255.0f))) g_t->FillRectangle(r, b.Get());

    const bool optics = theme == Theme::Siri || theme == Theme::Tinted ||
                        (g_ui.glassEffect && (theme == Theme::Light || theme == Theme::Dark || theme == Theme::Oled));
    const float ok = theme == Theme::Light ? 0.7f : theme == Theme::Tinted ? 0.5f : 1.0f;
    if (optics) {
        // A sheen on the upper part, light bouncing back along the bottom, and the thickness of the glass
        float sheenH = std::min(h * 0.42f, 34 * s);
        D2D1_GRADIENT_STOP sheen[3] = {{0.0f, White(0.18f * ok)}, {0.45f, White(0.05f * ok)}, {1.0f, White(0.0f)}};
        if (auto top = MakeGradient(sheen, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.top + sheenH)))
            g_t->FillRectangle(D2D1::RectF(r.left, r.top, r.right, r.top + sheenH), top.Get());
        float bounceH = std::min(h * 0.30f, 22 * s);
        D2D1_GRADIENT_STOP bounce[2] = {{0.0f, D2D1::ColorF(0.85f, 0.88f, 1.0f, 0.0f)},
                                        {1.0f, D2D1::ColorF(0.85f, 0.88f, 1.0f, 0.09f * ok)}};
        if (auto bottom = MakeGradient(bounce, 2, D2D1::Point2F(0, r.bottom - bounceH), D2D1::Point2F(0, r.bottom)))
            g_t->FillRectangle(D2D1::RectF(r.left, r.bottom - bounceH, r.right, r.bottom), bottom.Get());
        float tw = 3.5f * s, ti = tw / 2 + 1.2f * s;
        if (auto b = Solid(White((theme == Theme::Dark || theme == Theme::Oled ? 0.16f : 0.09f) * ok)))
            g_t->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + ti, r.top + ti, r.right - ti, r.bottom - ti),
                                                        std::max(0.0f, radius - ti), std::max(0.0f, radius - ti)),
                                      b.Get(), tw);
    }
    if (theme == Theme::Dark) {
        float glowH = std::min(h * 0.30f, 22 * s);
        D2D1_GRADIENT_STOP glow[2] = {{0.0f, White(0.06f)}, {1.0f, White(0.0f)}};
        if (auto top = MakeGradient(glow, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.top + glowH)))
            g_t->FillRectangle(D2D1::RectF(r.left, r.top, r.right, r.top + glowH), top.Get());
    }
    if (theme == Theme::Light || theme == Theme::Glass) {
        float glowH = std::min(h * (theme == Theme::Glass ? 0.22f : 0.35f), (theme == Theme::Glass ? 14 : 26) * s);
        float glowA = theme == Theme::Glass ? 0.07f : 0.55f;
        D2D1_GRADIENT_STOP glow[2] = {{0.0f, White(glowA)}, {1.0f, White(0.0f)}};
        if (auto top = MakeGradient(glow, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.top + glowH)))
            g_t->FillRectangle(D2D1::RectF(r.left, r.top, r.right, r.top + glowH), top.Get());
    }
    g_t->PopLayer();

    if ((theme == Theme::Dark || theme == Theme::Oled) && !optics) {
        D2D1_GRADIENT_STOP darkRim[3] = {{0.0f, White(0.22f)}, {0.35f, White(0.08f)}, {1.0f, White(0.12f)}};
        D2D1_GRADIENT_STOP oledRim[2] = {{0.0f, White(0.16f)}, {1.0f, White(0.10f)}};
        auto edge = theme == Theme::Dark ? MakeGradient(darkRim, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))
                                         : MakeGradient(oledRim, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom));
        if (edge) StrokeInside(r, radius, edge.Get(), std::max(1.0f, 1.0f * s));
        return;
    }
    if (theme == Theme::Tinted) {
        // The light gathers along the bottom edge, where the glass is clear, as a fine bright line
        D2D1_GRADIENT_STOP glowLine[4] = {{0.0f, White(0.0f)}, {0.62f, White(0.0f)}, {0.88f, White(0.04f)}, {1.0f, White(0.13f)}};
        if (auto glow = MakeGradient(glowLine, 4, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))) {
            float gw = 2.4f * s, gi = gw / 2 + 0.4f * s, gr = std::max(0.0f, radius - gi);
            g_t->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + gi, r.top + gi, r.right - gi, r.bottom - gi), gr, gr),
                                      glow.Get(), gw);
        }
        D2D1_GRADIENT_STOP line[6] = {{0.0f, White(0.16f)}, {0.12f, White(0.05f)}, {0.55f, White(0.05f)},
                                      {0.80f, White(0.14f)}, {0.93f, White(0.36f)}, {1.0f, White(0.62f)}};
        if (auto edge = MakeGradient(line, 6, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom)))
            StrokeInside(r, radius, edge.Get(), std::max(0.75f, 0.75f * s));
        return;
    }
    if (optics) {
        // Light caught by a beveled glass edge: strongest at the top-left and bottom-right corners
        float rk = theme == Theme::Light ? 1.0f : ok;
        D2D1_COLOR_F ink = theme == Theme::Light ? D2D1::ColorF(0.55f, 0.57f, 0.62f) : D2D1::ColorF(1, 1, 1);
        D2D1_GRADIENT_STOP bevel[5] = {
            {0.0f, theme == Theme::Light ? White(0.95f) : Alpha(ink, 0.80f * rk)},
            {0.28f, Alpha(ink, 0.20f * rk)},
            {0.55f, Alpha(ink, 0.10f * rk)},
            {0.82f, Alpha(ink, 0.22f * rk)},
            {1.0f, Alpha(ink, 0.50f * rk)},
        };
        float w = r.right - r.left, side = std::min(w, h);
        if (auto edge = MakeGradient(bevel, 5, D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.left + side * 1.5f, r.top + side)))
            StrokeInside(r, radius, edge.Get(), std::max(1.0f, 1.1f * s));
        return;
    }
    D2D1_GRADIENT_STOP softLightRim[5] = {
        {0.0f, White(0.75f)}, {0.15f, D2D1::ColorF(0.75f, 0.75f, 0.78f, 0.35f)}, {0.5f, D2D1::ColorF(0.62f, 0.62f, 0.66f, 0.30f)},
        {0.85f, D2D1::ColorF(0.75f, 0.75f, 0.78f, 0.40f)}, {1.0f, D2D1::ColorF(0.96f, 0.96f, 0.98f, 0.60f)},
    };
    D2D1_GRADIENT_STOP rim[7];
    SoftRim(rim);
    if (theme == Theme::Glass) ClearGlassRim(transparency, rim);
    auto edge = theme == Theme::Light ? MakeGradient(softLightRim, 5, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))
                                      : MakeGradient(rim, 7, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom));
    if (edge) StrokeInside(r, radius, edge.Get(), std::max(1.0f, 1.2f * s));
}

static void DrawThemedShape(const D2D1_RECT_F& r, float radius) {
    float rad = std::min(radius, std::min(r.right - r.left, r.bottom - r.top) / 2);
    DrawBacklightGlow(r, rad, 22 * g_s);
    DrawThemedShapeBody(r, radius, true);
    DrawBacklightRim(r, rad);
    if (g_ui.chromeBorder) DrawChromeEdge(r, rad, 0.9f, 0.08f);
}

// ---------------------------------------------------------------------------
// Widget content (drawn in DIPs, origin at the glass' top-left corner)
// ---------------------------------------------------------------------------

static void Text(const std::wstring& src, float x, float y, float w, float h, float size, DWRITE_FONT_WEIGHT weight,
                 D2D1_COLOR_F color, DWRITE_TEXT_ALIGNMENT align = DWRITE_TEXT_ALIGNMENT_LEADING) {
    const std::wstring& s = T(src);
    if (s.empty()) return;
    ComPtr<IDWriteTextFormat> fmt;
    if (FAILED(g_dw->CreateTextFormat(L"Segoe UI Variable Display", nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
                                      DWRITE_FONT_STRETCH_NORMAL, size, L"", &fmt)))
        return;
    fmt->SetTextAlignment(align);
    fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    ComPtr<IDWriteInlineObject> ellipsis;
    if (SUCCEEDED(g_dw->CreateEllipsisTrimmingSign(fmt.Get(), &ellipsis))) {
        DWRITE_TRIMMING trim = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        fmt->SetTrimming(&trim, ellipsis.Get());
    }
    if (auto br = Solid(color))
        g_t->DrawText(s.c_str(), (UINT32)s.size(), fmt.Get(), D2D1::RectF(x, y, x + w, y + h), br.Get(),
                      D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

static void FillRound(float x, float y, float w, float h, float r, D2D1_COLOR_F c) {
    Fill(D2D1::RectF(x, y, x + w, y + h), c, r);
}
static void FillEllipse(float cx, float cy, float rx, float ry, D2D1_COLOR_F c) {
    if (auto b = Solid(c)) g_t->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rx, ry), b.Get());
}
static void Line(float x1, float y1, float x2, float y2, float width, D2D1_COLOR_F c) {
    if (auto b = Solid(c)) {
        ComPtr<ID2D1StrokeStyle> round;
        D2D1_STROKE_STYLE_PROPERTIES p = D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                                                     D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND);
        g_d2d->CreateStrokeStyle(p, nullptr, 0, &round);
        g_t->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), b.Get(), width, round.Get());
    }
}

static void DrawCloud(float x, float y, float s, D2D1_COLOR_F c) {
    FillEllipse(x + s * 0.38f, y + s * 0.55f, s * 0.22f, s * 0.20f, c);
    FillEllipse(x + s * 0.60f, y + s * 0.48f, s * 0.26f, s * 0.26f, c);
    FillRound(x + s * 0.18f, y + s * 0.55f, s * 0.70f, s * 0.22f, s * 0.11f, c);
}

static void DrawWeatherIcon(float x, float y, float s, int code) {
    D2D1_COLOR_F sun = D2D1::ColorF(1.0f, 0.84f, 0.04f, 1.0f);
    D2D1_COLOR_F cloud = g_darkText ? D2D1::ColorF(0.55f, 0.58f, 0.65f, 1.0f) : White(0.92f);
    if (code <= 1) {
        FillEllipse(x + s / 2, y + s / 2, s * 0.30f, s * 0.30f, sun);
        return;
    }
    if (code == 2) FillEllipse(x + s * 0.36f, y + s * 0.36f, s * 0.22f, s * 0.22f, sun);
    DrawCloud(x, y, s, cloud);
    D2D1_COLOR_F rain = D2D1::ColorF(0.39f, 0.82f, 1.0f, 1.0f);
    if ((code >= 51 && code <= 67) || (code >= 80 && code <= 82)) {
        for (int i = 0; i < 3; i++)
            Line(x + s * (0.32f + i * 0.18f), y + s * 0.84f, x + s * (0.27f + i * 0.18f), y + s * 0.98f, 2, rain);
    } else if (code >= 95) {
        Line(x + s * 0.52f, y + s * 0.80f, x + s * 0.44f, y + s * 0.96f, 2.5f, sun);
    } else if (code == 45 || code == 48) {
        Line(x + s * 0.20f, y + s * 0.90f, x + s * 0.80f, y + s * 0.90f, 2, Ink(0.7f));
    }
}

static void DrawBar(float x, float y, float w, double pct, D2D1_COLOR_F c) {
    FillRound(x, y, w, 6, 3, Ink(0.15f));
    pct = std::clamp(pct, 0.0, 100.0);
    float fw = (float)(w * pct / 100.0);
    if (fw > 0.5f) FillRound(x, y, fw < 6 ? 6 : fw, 6, 3, c);
}

// Sizes (glass, in DIPs): small 1×1 cell, medium 2×1, large 2×2
static const float kCell = 164, kGap = 16;
static const float kEdge = 24;  // from the work area's edge to the first cell of the grid
static float SizeW(int size) { return size == 0 ? kCell : 2 * kCell + kGap; }
static float SizeH(int size) { return size == 3 ? 3 * kCell + 2 * kGap : size == 2 ? 2 * kCell + kGap : kCell; }  // 3: extra large (portrait)
static bool AllowsExtraLarge(int kind) {
    return !(kind == W_CLOCK || kind == W_SYSTEM || kind == W_SUN || kind == W_BATTERY || kind == W_NETWORK);
}

static void DrawAnalog(float cx, float cy, float r, const SYSTEMTIME& st) {
    FillEllipse(cx, cy, r, r, Ink(0.07f));
    if (auto b = Solid(Ink(0.16f))) g_t->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r - 0.5f, r - 0.5f), b.Get(), 1);
    for (int i = 0; i < 60; i++) {
        if (r < 70 && i % 5) continue;
        float a = i * 6.0f * 3.14159265f / 180, s = std::sin(a), c = -std::cos(a);
        float len = i % 5 ? r * 0.04f : r * 0.10f;
        Line(cx + s * (r * 0.88f - len), cy + c * (r * 0.88f - len), cx + s * r * 0.88f, cy + c * r * 0.88f,
             i % 15 == 0 ? 2.4f : i % 5 ? 0.8f : 1.5f, Ink(i % 5 ? 0.35f : 0.75f));
    }
    float sec = st.wSecond, min = st.wMinute + sec / 60.0f, hour = (st.wHour % 12) + min / 60.0f;
    auto hand = [&](float turns, float len, float width, D2D1_COLOR_F col, float back) {
        float a = turns * 2 * 3.14159265f, s = std::sin(a), c = -std::cos(a);
        Line(cx - s * back, cy - c * back, cx + s * len, cy + c * len, width, col);
    };
    hand(hour / 12.0f, r * 0.50f, std::max(3.0f, r * 0.06f), Ink(0.95f), 0);
    hand(min / 60.0f, r * 0.76f, std::max(2.5f, r * 0.045f), Ink(0.95f), 0);
    D2D1_COLOR_F orange = D2D1::ColorF(1.0f, 0.62f, 0.04f, 1.0f);
    hand(sec / 60.0f, r * 0.82f, 1.3f, orange, r * 0.16f);
    FillEllipse(cx, cy, std::max(2.5f, r * 0.045f), std::max(2.5f, r * 0.045f), orange);
}

static void DrawClock(Widget* wd) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t day[64] = L"", date[64] = L"", time[16];
    GetDateFormatEx(LangLocale(), 0, &st, L"dddd", day, 64, nullptr);
    GetDateFormatEx(LangLocale(), 0, &st, DayMonthFormat(), date, 64, nullptr);
    CharUpperW(day);
    swprintf(time, 16, L"%02d:%02d", st.wHour, st.wMinute);
    float W = wd->w, H = wd->h;
    if (g_ui.clockStyle == 1) {  // hands only
        if (wd->size == 0) {
            DrawAnalog(W / 2, H / 2, 66, st);
        } else if (wd->size == 1) {
            DrawAnalog(16 + 66, H / 2, 66, st);
            Text(day, 166, 44, W - 182, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.75f));
            Text(date, 166, 64, W - 182, 26, 18, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        } else {
            Text(day, 18, 18, W - 36, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.75f));
            Text(date, 18, 36, W - 36, 22, 16, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
            DrawAnalog(W / 2, 200, 124, st);
        }
        return;
    }
    if (g_ui.clockStyle == 2) {  // digital only
        if (wd->size == 0) {
            Text(day, 16, 16, W - 32, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.75f));
            Text(time, 10, 50, W - 20, 60, 46, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
            Text(date, 16, 126, W - 32, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.8f));
        } else if (wd->size == 1) {
            Text(day, 18, 16, W - 36, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.75f));
            Text(time, 0, 34, W, 96, 80, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_CENTER);
            Text(date, 18, 130, W - 36, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.8f), DWRITE_TEXT_ALIGNMENT_CENTER);
        } else {
            Text(day, 18, 18, W - 36, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.75f));
            Text(date, 18, 36, W - 36, 22, 16, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
            Text(time, 0, 112, W, 130, 104, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_CENTER);
            wchar_t secs[8];
            swprintf(secs, 8, L"%02d", st.wSecond);
            Text(secs, 0, 250, W, 34, 26, DWRITE_FONT_WEIGHT_MEDIUM, Ink(0.55f), DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        return;
    }
    if (wd->size == 0) {
        Text(day, 16, 16, wd->w - 32, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.75f));
        Text(time, 14, 50, wd->w - 28, 60, 46, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(date, 16, 126, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.8f));
    } else if (wd->size == 1) {
        Text(day, 18, 18, 190, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.75f));
        Text(time, 16, 48, 200, 76, 60, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(date, 18, 126, 190, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.8f));
        DrawAnalog(wd->w - 82, wd->h / 2, 64, st);
    } else {
        Text(day, 18, 18, wd->w - 36, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.75f));
        Text(date, 18, 36, wd->w - 36, 22, 16, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        DrawAnalog(wd->w / 2, 186, 112, st);
        Text(time, 0, 304, wd->w, 26, 18, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.85f), DWRITE_TEXT_ALIGNMENT_CENTER);
    }
}


// A crescent moon (a disc with a smaller one taken out of it)
static void DrawMoon(float x, float y, float s) {
    D2D1_COLOR_F moon = g_darkText ? D2D1::ColorF(0.42f, 0.45f, 0.58f, 1.0f) : D2D1::ColorF(0.95f, 0.95f, 1.0f, 1.0f);
    ComPtr<ID2D1EllipseGeometry> a, b;
    ComPtr<ID2D1PathGeometry> crescent;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(g_d2d->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(x + s * 0.48f, y + s * 0.52f), s * 0.34f, s * 0.34f), &a)) ||
        FAILED(g_d2d->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(x + s * 0.68f, y + s * 0.36f), s * 0.28f, s * 0.28f), &b)) ||
        FAILED(g_d2d->CreatePathGeometry(&crescent)) || FAILED(crescent->Open(&sink)))
        return;
    a->CombineWithGeometry(b.Get(), D2D1_COMBINE_MODE_EXCLUDE, nullptr, sink.Get());
    sink->Close();
    if (auto br = Solid(moon)) g_t->FillGeometry(crescent.Get(), br.Get());
}

static void DrawWeatherIconAt(float x, float y, float s, int code, bool day) {
    if (day) {
        DrawWeatherIcon(x, y, s, code);
        return;
    }
    if (code <= 1) {  // clear night
        DrawMoon(x, y, s);
        return;
    }
    if (code == 2) {  // partly cloudy night: the moon behind a cloud
        DrawMoon(x - s * 0.08f, y - s * 0.12f, s * 0.78f);
        DrawCloud(x, y, s, g_darkText ? D2D1::ColorF(0.55f, 0.58f, 0.65f, 1.0f) : White(0.92f));
        return;
    }
    DrawWeatherIcon(x, y, s, code);  // clouds, rain, snow: the same as by day
}

static void TextWrap(const std::wstring& src, float x, float y, float w, float h, float size, DWRITE_FONT_WEIGHT weight,
                     D2D1_COLOR_F color);

static std::wstring CityName() { return g_ui.city.empty() ? T(L"Sua cidade") : g_ui.city; }

static void DrawWeather(Widget* wd) {
    WeatherData w;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        w = g_weather;
    }
    float W = wd->w;
    if (!HasCity(g_ui)) {
        Text(L"Tempo", 16, 14, W - 32, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        TextWrap(L"Clique para escolher sua cidade", 16, 44, W - 32, wd->h - 56, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.7f));
        return;
    }
    if (!w.valid) {
        Text(CityName(), 16, 14, W - 32, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(L"Carregando…", 16, 70, W - 32, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.7f));
        return;
    }
    wchar_t buf[64];
    if (wd->size == 0) {
        Text(CityName(), 16, 14, W - 32, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        swprintf(buf, 64, L"%.0f°", w.temp);
        Text(buf, 14, 32, W - 28, 52, 40, DWRITE_FONT_WEIGHT_MEDIUM, Ink(1));
        DrawWeatherIconAt(16, 88, 26, w.code, w.day);
        Text(WeatherText(w.code), 16, 116, W - 32, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(1));
        swprintf(buf, 64, TF(L"Máx. %.0f°  Mín. %.0f°"), w.tmax, w.tmin);
        Text(buf, 16, 132, W - 32, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
        return;
    }
    // Medium and large: now on top, the next hours under it
    Text(CityName(), 16, 14, W / 2, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
    swprintf(buf, 64, L"%.0f°", w.temp);
    Text(buf, 14, 30, W / 2, 50, 38, DWRITE_FONT_WEIGHT_MEDIUM, Ink(1));
    DrawWeatherIconAt(W - 16 - 24, 14, 24, w.code, w.day);
    Text(WeatherText(w.code), W / 2, 44, W / 2 - 16, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_TRAILING);
    swprintf(buf, 64, TF(L"Máx. %.0f°  Mín. %.0f°"), w.tmax, w.tmin);
    Text(buf, W / 2 - 40, 62, W / 2 + 24, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f), DWRITE_TEXT_ALIGNMENT_TRAILING);
    int n = std::min(w.hours, 6);
    float colW = (W - 24) / 6;
    for (int i = 0; i < n; i++) {
        float x = 12 + i * colW;
        if (i == 0) wcscpy(buf, L"Agora");
        else swprintf(buf, 64, L"%dh", w.hourOf[i]);
        Text(buf, x, 90, colW, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.75f), DWRITE_TEXT_ALIGNMENT_CENTER);
        DrawWeatherIconAt(x + colW / 2 - 10, 108, 20, w.hourCode[i], w.hourDay[i]);
        swprintf(buf, 64, L"%.0f°", w.hourTemp[i]);
        Text(buf, x, 132, colW, 18, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    if (wd->size < 2) return;
    // Large: the next days, with a bar from the week's lowest to its highest
    Line(16, 164, W - 16, 164, 1, Ink(0.12f));
    int days = std::min(w.days, 5);
    double lo = 1e9, hi = -1e9;
    for (int i = 0; i < days; i++) {
        lo = std::min(lo, w.dayMin[i]);
        hi = std::max(hi, w.dayMax[i]);
    }
    float barL = 150, barR = W - 60;
    for (int i = 0; i < days; i++) {
        float y = 174 + i * 32;
        Text(i == 0 ? L"Hoje" : WeekdayShort(w.dayWeekday[i]), 16, y, 60, 24, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        DrawWeatherIconAt(74, y + 2, 20, w.dayCode[i], true);
        swprintf(buf, 64, L"%.0f°", w.dayMin[i]);
        Text(buf, barL - 50, y, 42, 24, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f), DWRITE_TEXT_ALIGNMENT_TRAILING);
        FillRound(barL, y + 10, barR - barL, 4, 2, Ink(0.14f));
        if (hi > lo) {
            float a = (float)((w.dayMin[i] - lo) / (hi - lo)), b = (float)((w.dayMax[i] - lo) / (hi - lo));
            D2D1_GRADIENT_STOP st[2] = {{0.0f, D2D1::ColorF(0.39f, 0.82f, 1.0f, 1.0f)}, {1.0f, D2D1::ColorF(1.0f, 0.62f, 0.04f, 1.0f)}};
            if (auto br = MakeGradient(st, 2, D2D1::Point2F(barL, 0), D2D1::Point2F(barR, 0))) {
                float x0 = barL + (barR - barL) * a, x1 = std::max(x0 + 4, barL + (barR - barL) * b);
                g_t->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x0, y + 10, x1, y + 14), 2, 2), br.Get());
            }
        }
        swprintf(buf, 64, L"%.0f°", w.dayMax[i]);
        Text(buf, barR + 8, y, 42, 24, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
    }
    if (wd->size < 3) return;
    // Extra large: four tiles with the details of now
    wchar_t v[4][32];
    swprintf(v[0], 32, L"%.0f°", w.feels);
    swprintf(v[1], 32, L"%.0f%%", w.humidity);
    swprintf(v[2], 32, L"%.0f km/h", w.wind);
    swprintf(v[3], 32, L"%.0f%%", w.rainChance);
    PCWSTR names[4] = {L"SENSAÇÃO", L"UMIDADE", L"VENTO", L"CHANCE DE CHUVA"};
    float tw = (W - 40) / 2, th = 76, ty = 344;
    for (int i = 0; i < 4; i++) {
        float x = 16 + (i % 2) * (tw + 8), y = ty + (i / 2) * (th + 8);
        Fill(D2D1::RectF(x, y, x + tw, y + th), Ink(0.07f), 14);
        Text(names[i], x + 12, y + 10, tw - 24, 16, 10.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.6f));
        Text(v[i], x + 12, y + 30, tw - 24, 34, 24, DWRITE_FONT_WEIGHT_MEDIUM, Ink(1));
    }
}

static std::wstring GbText(double bytes);

// ---------- Pictures in widgets ----------

// The picture fills the rectangle (cropped to keep its proportions), with rounded corners
static void DrawImageFill(Widget* wd, const std::shared_ptr<Image>& img, const D2D1_RECT_F& r, float radius) {
    if (!img || img->w <= 0 || img->h <= 0) return;
    if (wd->shownImg != img || !wd->imgBmp) {
        wd->imgBmp.Reset();
        wd->shownImg = img;
        g_t->CreateBitmap(D2D1::SizeU(img->w, img->h), img->px.data(), img->w * 4,
                          D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
                          &wd->imgBmp);
    }
    if (!wd->imgBmp) return;
    float rw = r.right - r.left, rh = r.bottom - r.top;
    float k = std::max(rw / img->w, rh / img->h);
    float sw = rw / k, sh = rh / k, sx = (img->w - sw) / 2, sy = (img->h - sh) / 2;
    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    if (FAILED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, radius, radius), &clip))) return;
    g_t->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get()), nullptr);
    D2D1_RECT_F src = D2D1::RectF(sx, sy, sx + sw, sy + sh);
    g_t->DrawBitmap(wd->imgBmp.Get(), r, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &src);
    g_t->PopLayer();
}

// ---------- Calendar ----------

static int DaysIn(int year, int month) {
    static const int d[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    return month == 2 && leap ? 29 : d[(month - 1) % 12];
}
static int FirstWeekday(int year, int month) {  // 0 = Sunday
    SYSTEMTIME s = {};
    s.wYear = (WORD)year;
    s.wMonth = (WORD)month;
    s.wDay = 1;
    FILETIME ft;
    SystemTimeToFileTime(&s, &ft);
    FileTimeToSystemTime(&ft, &s);
    return s.wDayOfWeek;
}

static const D2D1_COLOR_F kRed = {1.0f, 0.27f, 0.23f, 1.0f};

static void DrawMonthGrid(float x, float y, float w, float h, const SYSTEMTIME& st, float fs) {
    PCWSTR letters[7];
    for (int i = 0; i < 7; i++) letters[i] = WeekdayLetter(i);
    float cw = w / 7, rh = h / 7;
    for (int i = 0; i < 7; i++)
        Text(letters[i], x + i * cw, y, cw, rh, fs * 0.85f, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.55f), DWRITE_TEXT_ALIGNMENT_CENTER);
    int first = FirstWeekday(st.wYear, st.wMonth), days = DaysIn(st.wYear, st.wMonth);
    for (int d = 1; d <= days; d++) {
        int cell = first + d - 1, c = cell % 7, r = cell / 7 + 1;
        float cx = x + c * cw, cy = y + r * rh;
        bool today = d == st.wDay;
        if (today) {
            float rad = std::min(cw, rh) * 0.46f;
            FillEllipse(cx + cw / 2, cy + rh / 2, rad, rad, kRed);
        }
        Text(std::to_wstring(d), cx, cy + (rh - fs * 1.35f) / 2, cw, fs * 1.4f, fs,
             today ? DWRITE_FONT_WEIGHT_SEMI_BOLD : DWRITE_FONT_WEIGHT_NORMAL, today ? White(1) : Ink(c == 0 ? 0.55f : 0.92f),
             DWRITE_TEXT_ALIGNMENT_CENTER);
    }
}

// ---------- World clock ----------

struct ZoneCache {
    std::wstring key;
    DYNAMIC_TIME_ZONE_INFORMATION tz;
    bool ok;
};
static std::vector<ZoneCache> g_zones;

static bool TimeInZone(const std::wstring& key, SYSTEMTIME* out) {
    const DYNAMIC_TIME_ZONE_INFORMATION* tz = nullptr;
    for (auto& z : g_zones)
        if (z.key == key) tz = z.ok ? &z.tz : nullptr;
    bool known = false;
    for (auto& z : g_zones)
        if (z.key == key) known = true;
    if (!known) {
        ZoneCache z{key, {}, false};
        DYNAMIC_TIME_ZONE_INFORMATION d;
        for (DWORD i = 0; EnumDynamicTimeZoneInformation(i, &d) == ERROR_SUCCESS; i++)
            if (key == d.TimeZoneKeyName) {
                z.tz = d;
                z.ok = true;
                break;
            }
        g_zones.push_back(z);
        tz = z.ok ? &g_zones.back().tz : nullptr;
    }
    if (!tz) return false;
    SYSTEMTIME utc;
    GetSystemTime(&utc);
    return SystemTimeToTzSpecificLocalTimeEx(tz, &utc, out) != 0;
}

static long long MinutesOf(const SYSTEMTIME& s) {
    FILETIME ft;
    SystemTimeToFileTime(&s, &ft);
    return (long long)((((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 600000000ULL);
}

static std::wstring CityLabel(const std::wstring& key) {
    for (const CfgOpt& o : kOptCities)
        if (key == o.value) return o.label;
    return key;
}

// "Hoje, +4h" / "Amanhã, +13h"
static std::wstring ZoneNote(const SYSTEMTIME& there) {
    SYSTEMTIME here;
    GetLocalTime(&here);
    long long diff = MinutesOf(there) - MinutesOf(here);
    diff = (diff >= 0 ? diff + 1 : diff - 1) / 15 * 15;  // to the quarter hour
    long long dayDiff = MinutesOf({there.wYear, there.wMonth, 0, there.wDay}) / 1440 - MinutesOf({here.wYear, here.wMonth, 0, here.wDay}) / 1440;
    std::wstring s = T(dayDiff > 0 ? L"Amanhã" : dayDiff < 0 ? L"Ontem" : L"Hoje");
    wchar_t buf[32];
    long long a = diff < 0 ? -diff : diff;
    if (a % 60) swprintf(buf, 32, L", %lc%lldh%02lld", diff < 0 ? L'-' : L'+', a / 60, a % 60);
    else swprintf(buf, 32, L", %lc%lldh", diff < 0 ? L'-' : L'+', a / 60);
    return s + buf;
}

static void DrawWorld(Widget* wd) {
    auto cityAt = [&](int i, SYSTEMTIME* t) { return TimeInZone(g_ui.worldCity[i], t); };
    wchar_t hm[16];
    if (wd->size == 0) {
        SYSTEMTIME t;
        if (!cityAt(0, &t)) return;
        DrawAnalog(wd->w / 2, 66, 50, t);
        Text(CityLabel(g_ui.worldCity[0]), 8, 122, wd->w - 16, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_CENTER);
        Text(ZoneNote(t), 8, 140, wd->w - 16, 16, 11, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f), DWRITE_TEXT_ALIGNMENT_CENTER);
    } else if (wd->size == 1) {
        float cw = (wd->w - 16) / 4;
        for (int i = 0; i < 4; i++) {
            SYSTEMTIME t;
            if (!cityAt(i, &t)) continue;
            float cx = 8 + cw * i + cw / 2;
            DrawAnalog(cx, 58, 32, t);
            Text(CityLabel(g_ui.worldCity[i]), cx - cw / 2, 100, cw, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_CENTER);
            swprintf(hm, 16, L"%02d:%02d", t.wHour, t.wMinute);
            Text(hm, cx - cw / 2, 118, cw, 16, 11, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f), DWRITE_TEXT_ALIGNMENT_CENTER);
        }
    } else {
        for (int i = 0; i < 4; i++) {
            SYSTEMTIME t;
            if (!cityAt(i, &t)) continue;
            float rowH = (wd->h - 24) / 4, y = 12 + i * rowH + (rowH - 80) / 2;
            DrawAnalog(48, y + 40, 32, t);
            Text(CityLabel(g_ui.worldCity[i]), 94, y + 20, 150, 22, 16, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
            Text(ZoneNote(t), 94, y + 42, 150, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f));
            swprintf(hm, 16, L"%02d:%02d", t.wHour, t.wMinute);
            Text(hm, wd->w - 120, y + 22, 104, 36, 28, DWRITE_FONT_WEIGHT_MEDIUM, Ink(1), DWRITE_TEXT_ALIGNMENT_TRAILING);
            if (i < 3) Line(94, 12 + (i + 1) * rowH - 0.5f, wd->w - 16, 12 + (i + 1) * rowH - 0.5f, 1, Ink(0.08f));
        }
    }
}

// ---------- Sunrise and sunset ----------

static std::wstring ClockAt(long long unix, long long offset) {
    long long m = (((unix + offset) / 60) % 1440 + 1440) % 1440;
    wchar_t b[16];
    swprintf(b, 16, L"%02lld:%02lld", m / 60, m % 60);
    return b;
}

// The sun's path over the day as a curve, the horizon, and where the sun is now
static void DrawSunArc(float x, float y, float w, float h, double dayFrac) {
    float horizon = y + h * 0.62f;
    Line(x, horizon, x + w, horizon, 1, Ink(0.25f));
    ComPtr<ID2D1PathGeometry> path;
    ComPtr<ID2D1GeometrySink> sink;
    if (SUCCEEDED(g_d2d->CreatePathGeometry(&path)) && SUCCEEDED(path->Open(&sink))) {
        for (int i = 0; i <= 64; i++) {
            float t = i / 64.0f;
            float px = x + w * t, py = horizon - std::sin((t - 0.25f) * 2 * 3.14159265f) * h * 0.55f * 0.62f - h * 0.0f;
            if (i == 0) sink->BeginFigure(D2D1::Point2F(px, py), D2D1_FIGURE_BEGIN_HOLLOW);
            else sink->AddLine(D2D1::Point2F(px, py));
        }
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();
        if (auto b = Solid(Ink(0.35f))) g_t->DrawGeometry(path.Get(), b.Get(), 1.5f);
    }
    float t = (float)std::clamp(dayFrac, 0.0, 1.0);
    float sx = x + w * t, sy = horizon - std::sin((t - 0.25f) * 2 * 3.14159265f) * h * 0.55f * 0.62f;
    bool up = sy < horizon;
    FillEllipse(sx, sy, 9, 9, up ? D2D1::ColorF(1.0f, 0.84f, 0.04f, 0.30f) : Ink(0.10f));
    FillEllipse(sx, sy, 5.5f, 5.5f, up ? D2D1::ColorF(1.0f, 0.84f, 0.04f, 1.0f) : Ink(0.45f));
}

static void DrawSun(Widget* wd) {
    WeatherData w;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        w = g_weather;
    }
    if (!w.valid || w.sunrise[0] == 0) {
        Text(L"Nascer do Sol", 16, 14, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        if (HasCity(g_ui)) Text(L"Carregando…", 16, 70, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.7f));
        else TextWrap(L"Clique para escolher sua cidade", 16, 44, wd->w - 32, wd->h - 56, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.7f));
        return;
    }
    long long now = UnixNow(), off = w.offset;
    long long rise = w.sunrise[0], set = w.sunset[0];
    bool beforeRise = now < rise, afterSet = now >= set;
    long long next = beforeRise ? rise : !afterSet ? set : w.sunrise[1];
    std::wstring nextLabel = (beforeRise || afterSet) ? L"NASCER DO SOL" : L"PÔR DO SOL";
    // Fraction of the day: 0.25 at sunrise, 0.75 at sunset, the night spread around them
    double frac;
    if (!beforeRise && !afterSet) frac = 0.25 + 0.5 * (double)(now - rise) / std::max(1LL, set - rise);
    else if (afterSet) frac = 0.75 + 0.25 * std::min(1.0, (double)(now - set) / std::max(1LL, (w.sunrise[1] ? w.sunrise[1] : rise + 86400) - set));
    else frac = 0.25 * (double)(now - (rise - 6 * 3600)) / (6 * 3600);
    long long dayLen = (set - rise) / 60;
    wchar_t buf[64];
    if (wd->size == 0) {
        Text(nextLabel, 16, 14, wd->w - 32, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.7f));
        Text(ClockAt(next, off), 14, 30, wd->w - 28, 44, 34, DWRITE_FONT_WEIGHT_MEDIUM, Ink(1));
        DrawSunArc(16, 80, wd->w - 32, 50, frac);
        Text((nextLabel == L"PÔR DO SOL" ? T(L"Nascer: ") + ClockAt(rise, off) : T(L"Pôr: ") + ClockAt(set, off)), 16, 136, wd->w - 32, 18, 12,
             DWRITE_FONT_WEIGHT_NORMAL, Ink(0.7f));
    } else {
        Text(nextLabel, 16, 14, 150, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.7f));
        Text(ClockAt(next, off), 14, 30, 150, 44, 34, DWRITE_FONT_WEIGHT_MEDIUM, Ink(1));
        Text(T(L"Nascer  ") + ClockAt(rise, off), 16, 96, 150, 18, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.8f));
        Text(T(L"Pôr  ") + ClockAt(set, off), 16, 118, 150, 18, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.8f));
        DrawSunArc(170, 24, wd->w - 186, 116, frac);
        if (wd->size == 2) {
            Line(16, 164, wd->w - 16, 164, 1, Ink(0.12f));
            swprintf(buf, 64, TF(L"%lld h %02lld min"), dayLen / 60, dayLen % 60);
            std::pair<std::wstring, std::wstring> rows[] = {
                {L"Duração do dia", buf},
                {L"Amanhã, nascer", w.sunrise[1] ? ClockAt(w.sunrise[1], off) : L"—"},
                {L"Amanhã, pôr", w.sunset[1] ? ClockAt(w.sunset[1], off) : L"—"},
                {L"Cidade", g_ui.city},
            };
            for (int i = 0; i < 4; i++) {
                float y = 178 + i * 38.0f;
                Text(rows[i].first, 16, y, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
                Text(rows[i].second, 16, y, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_TRAILING);
            }
        }
    }
}

// ---------- Batteries ----------

static void DrawRing(float cx, float cy, float r, float width, double pct, D2D1_COLOR_F c) {
    if (auto b = Solid(Ink(0.14f))) g_t->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), b.Get(), width);
    pct = std::clamp(pct, 0.0, 100.0);
    if (pct <= 0) return;
    ComPtr<ID2D1PathGeometry> path;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(g_d2d->CreatePathGeometry(&path)) || FAILED(path->Open(&sink))) return;
    float a = (float)(pct / 100.0 * 2 * 3.14159265);
    sink->BeginFigure(D2D1::Point2F(cx, cy - r), D2D1_FIGURE_BEGIN_HOLLOW);
    if (pct >= 99.9) {
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(cx, cy + r), D2D1::SizeF(r, r), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(cx, cy - r), D2D1::SizeF(r, r), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    } else {
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(cx + r * std::sin(a), cy - r * std::cos(a)), D2D1::SizeF(r, r), 0,
                                      D2D1_SWEEP_DIRECTION_CLOCKWISE, a > 3.14159265f ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL));
    }
    sink->EndFigure(D2D1_FIGURE_END_OPEN);
    sink->Close();
    ComPtr<ID2D1StrokeStyle> round;
    g_d2d->CreateStrokeStyle(D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND), nullptr, 0, &round);
    if (auto b = Solid(c)) g_t->DrawGeometry(path.Get(), b.Get(), width, round.Get());
}

static D2D1_COLOR_F BatteryColor(const BatteryInfo& b) {
    if (b.charging) return D2D1::ColorF(0.19f, 0.82f, 0.35f, 1.0f);
    if (b.level <= 20) return kRed;
    return b.level <= 40 ? D2D1::ColorF(1.0f, 0.62f, 0.04f, 1.0f) : D2D1::ColorF(0.19f, 0.82f, 0.35f, 1.0f);
}

static void DrawBattery(Widget* wd) {
    std::vector<BatteryInfo> list;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        list = g_batteries;
    }
    if (list.empty()) {
        Text(L"Baterias", 16, 14, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(L"Nenhuma bateria encontrada", 16, 46, wd->w - 32, 40, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
        if (wd->size > 0)
            Text(L"Fones, mouses e controles Bluetooth que informam a bateria ao Windows aparecem aqui", 16, 70, wd->w - 32, 60, 11,
                 DWRITE_FONT_WEIGHT_NORMAL, Ink(0.55f));
        return;
    }
    wchar_t pct[16];
    if (wd->size == 0) {
        const BatteryInfo& b = list[0];
        DrawRing(52, 56, 30, 7, b.level, BatteryColor(b));
        swprintf(pct, 16, L"%d%%", b.level);
        Text(pct, 22, 46, 60, 22, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_CENTER);
        Text(b.name, 96, 44, wd->w - 108, 30, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.8f));
        for (size_t i = 1; i < list.size() && i < 3; i++) {
            float y = 98 + (i - 1) * 24.0f;
            swprintf(pct, 16, L"%d%%", list[i].level);
            Text(list[i].name, 16, y, wd->w - 70, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
            Text(pct, 16, y, wd->w - 32, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, BatteryColor(list[i]), DWRITE_TEXT_ALIGNMENT_TRAILING);
        }
    } else if (wd->size == 1) {
        size_t n = std::min<size_t>(list.size(), 4);
        float cw = (wd->w - 16) / 4;
        for (size_t i = 0; i < n; i++) {
            float cx = 8 + cw * i + cw / 2;
            DrawRing(cx, 62, 30, 7, list[i].level, BatteryColor(list[i]));
            swprintf(pct, 16, L"%d%%", list[i].level);
            Text(pct, cx - 30, 52, 60, 22, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_CENTER);
            Text(list[i].name, cx - cw / 2 + 2, 104, cw - 4, 34, 11, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f), DWRITE_TEXT_ALIGNMENT_CENTER);
        }
    } else {
        Text(L"Baterias", 16, 14, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.85f));
        for (size_t i = 0; i < list.size() && i < 6; i++) {
            float y = 46 + i * 48.0f;
            swprintf(pct, 16, L"%d%%%ls", list[i].level, list[i].charging ? L" ⚡" : L"");
            Text(list[i].name, 16, y, wd->w - 100, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.9f));
            Text(pct, 16, y, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_TRAILING);
            DrawBar(16, y + 24, wd->w - 32, list[i].level, BatteryColor(list[i]));
        }
    }
}

// ---------- Network ----------

static std::wstring SpeedText(double bytesPerSec) {
    wchar_t b[32];
    if (bytesPerSec >= 1024.0 * 1024) swprintf(b, 32, L"%.1f MB/s", bytesPerSec / (1024.0 * 1024));
    else swprintf(b, 32, L"%.0f KB/s", bytesPerSec / 1024.0);
    std::wstring s = b;
    for (auto& c : s)
        if (c == L'.') c = L',';
    return s;
}

// A line of values: from zero (speeds), or from the lowest to the highest (prices, where the change is what matters)
static void DrawSparkline(float x, float y, float w, float h, const double* v, int n, D2D1_COLOR_F c, bool fromLowest = false) {
    if (n < 2) return;
    double mx = fromLowest ? v[0] : 1, mn = fromLowest ? v[0] : 0;
    for (int i = 0; i < n; i++) {
        mx = std::max(mx, v[i]);
        mn = std::min(mn, v[i]);
    }
    if (mx - mn < 1e-12) mx = mn + 1;
    ComPtr<ID2D1PathGeometry> path;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(g_d2d->CreatePathGeometry(&path)) || FAILED(path->Open(&sink))) return;
    for (int i = 0; i < n; i++) {
        float px = x + w * i / (n - 1), py = y + h - (float)((v[i] - mn) / (mx - mn)) * h;
        if (i == 0) sink->BeginFigure(D2D1::Point2F(px, py), D2D1_FIGURE_BEGIN_HOLLOW);
        else sink->AddLine(D2D1::Point2F(px, py));
    }
    sink->EndFigure(D2D1_FIGURE_END_OPEN);
    sink->Close();
    if (auto b = Solid(c)) g_t->DrawGeometry(path.Get(), b.Get(), 1.6f);
}

static void DrawNetwork(Widget* wd) {
    D2D1_COLOR_F down = D2D1::ColorF(0.39f, 0.82f, 1.0f, 1.0f), up = D2D1::ColorF(1.0f, 0.62f, 0.04f, 1.0f);
    auto arrow = [&](float x, float y, bool isDown, D2D1_COLOR_F c) {
        Line(x + 6, y + 2, x + 6, y + 12, 2.2f, c);
        if (isDown) {
            Line(x + 1, y + 7, x + 6, y + 12, 2.2f, c);
            Line(x + 11, y + 7, x + 6, y + 12, 2.2f, c);
        } else {
            Line(x + 1, y + 7, x + 6, y + 2, 2.2f, c);
            Line(x + 11, y + 7, x + 6, y + 2, 2.2f, c);
        }
    };
    float colW = wd->size == 0 ? wd->w - 32 : 140;
    Text(L"Rede", 16, 14, colW, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
    if (!g_net.adapter.empty() && wd->size > 0)
        Text(g_net.adapter, 16, 15, wd->w - 32, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f), DWRITE_TEXT_ALIGNMENT_TRAILING);
    arrow(16, 48, true, down);
    Text(L"Download", 34, 46, colW - 18, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
    Text(SpeedText(g_net.down), 16, 64, colW, 28, 20, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
    arrow(16, 102, false, up);
    Text(L"Upload", 34, 100, colW - 18, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
    Text(SpeedText(g_net.up), 16, 118, colW, 28, 20, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
    if (wd->size == 0) return;
    // The last minute, newest on the right
    double dh[60], uh[60];
    for (int i = 0; i < 60; i++) {
        dh[i] = g_net.histDown[(g_net.histPos + i) % 60];
        uh[i] = g_net.histUp[(g_net.histPos + i) % 60];
    }
    float gx = 170, gw = wd->w - 186, gy = 46, gh = wd->size == 2 ? 120 : 98;
    FillRound(gx, gy, gw, gh, 10, Ink(0.05f));
    DrawSparkline(gx + 6, gy + 8, gw - 12, gh - 16, dh, 60, down);
    DrawSparkline(gx + 6, gy + 8, gw - 12, gh - 16, uh, 60, up);
    if (wd->size < 2) return;
    Line(16, 180, wd->w - 16, 180, 1, Ink(0.12f));
    std::pair<std::wstring, std::wstring> rows[] = {
        {L"Recebido desde que o widget abriu", GbText(g_net.totalDown)},
        {L"Enviado desde que o widget abriu", GbText(g_net.totalUp)},
        {L"Pico de download (1 min)", SpeedText(*std::max_element(dh, dh + 60))},
    };
    for (int i = 0; i < 3; i++) {
        float y = 194 + i * 40.0f;
        Text(rows[i].first, 16, y, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
        Text(rows[i].second, 16, y, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_TRAILING);
    }
}

// ---------- Photos ----------

static void DrawPhotos(Widget* wd) {
    PhotoInfo p;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        p = g_photo;
    }
    float r = (float)g_ui.cornerRadius;
    if (!p.img) {
        Text(L"Fotos", 16, 14, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(L"Nenhuma foto encontrada", 16, 46, wd->w - 32, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
        Text(L"Fotos baixadas na pasta Imagens do OneDrive (ou a escolhida nos Ajustes) aparecem aqui", 16, 70, wd->w - 32, 70, 11,
             DWRITE_FONT_WEIGHT_NORMAL, Ink(0.55f));
        return;
    }
    D2D1_RECT_F all = D2D1::RectF(0, 0, wd->w, wd->h);
    DrawImageFill(wd, p.img, all, r);
    // A shade at the bottom for the caption
    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    if (SUCCEEDED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(all, r, r), &clip))) {
        g_t->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get()), nullptr);
        D2D1_GRADIENT_STOP shade[2] = {{0.0f, D2D1::ColorF(0, 0, 0, 0)}, {1.0f, D2D1::ColorF(0, 0, 0, 0.55f)}};
        if (auto b = MakeGradient(shade, 2, D2D1::Point2F(0, wd->h - 70), D2D1::Point2F(0, wd->h)))
            g_t->FillRectangle(D2D1::RectF(0, wd->h - 70, wd->w, wd->h), b.Get());
        g_t->PopLayer();
    }
    Text(L"FOTOS", 16, wd->h - 50, wd->w - 32, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, White(0.8f));
    Text(p.caption, 16, wd->h - 34, wd->w - 32, 22, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, White(1));
}

// ---------- Notifications ----------

static std::wstring AgoText(long long arrival) {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    long long now = (long long)(((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime);
    long long min = (now - arrival) / 600000000LL;
    wchar_t b[32];
    if (min < 1) return T(L"agora");
    if (min < 60) swprintf(b, 32, TF(L"%lld min"), min);
    else if (min < 24 * 60) swprintf(b, 32, TF(L"%lld h"), min / 60);
    else if (min < 48 * 60) return T(L"ontem");
    else swprintf(b, 32, TF(L"%lld d"), min / 1440);
    return b;
}

static D2D1_COLOR_F AppColor(const std::wstring& app) {
    if (app == L"WhatsApp") return D2D1::ColorF(0.12f, 0.62f, 0.33f, 1.0f);
    if (app == L"Instagram") return D2D1::ColorF(0.72f, 0.20f, 0.42f, 1.0f);
    if (app == L"Outlook") return D2D1::ColorF(0.11f, 0.44f, 0.82f, 1.0f);
    if (app == L"Facebook") return D2D1::ColorF(0.09f, 0.47f, 0.95f, 1.0f);
    static const D2D1_COLOR_F pal[6] = {{0.37f, 0.36f, 0.90f, 1}, {0.67f, 0.56f, 0.41f, 1}, {0.19f, 0.69f, 0.78f, 1},
                                        {0.95f, 0.45f, 0.20f, 1}, {0.55f, 0.35f, 0.80f, 1}, {0.28f, 0.28f, 0.29f, 1}};
    unsigned h = 0;
    for (wchar_t c : app) h = h * 31 + c;
    return pal[h % 6];
}

static D2D1_RECT_F NotifClearRect(const Widget* wd) { return D2D1::RectF(wd->w - 16 - 72, 10, wd->w - 16, 36); }

static void DrawNotifCard(float x, float y, float w, float h, const NotifInfo& n) {
    Fill(D2D1::RectF(x, y, x + w, y + h), Ink(0.07f), 14);
    FillEllipse(x + 26, y + h / 2, 16, 16, AppColor(n.app));
    Text(n.app.empty() ? L"?" : n.app.substr(0, 1), x + 10, y + h / 2 - 11, 32, 22, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, White(1),
         DWRITE_TEXT_ALIGNMENT_CENTER);
    float tx = x + 52, tw = w - 62;
    Text(n.app, tx, y + 7, tw - 50, 16, 11.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.65f));
    Text(AgoText(n.arrival), tx, y + 7, tw, 16, 11.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.65f), DWRITE_TEXT_ALIGNMENT_TRAILING);
    Text(n.title, tx, y + 23, tw, 18, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
    if (!n.body.empty()) Text(n.body, tx, y + 41, tw, 18, 12.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.85f));
}

static void DrawNotif(Widget* wd) {
    std::vector<NotifInfo> list;
    std::wstring error;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        list = g_notifs;
        error = g_notifError;
    }
    if (wd->size == 0) {
        // A bell with the count, and the newest one
        float bx = 16, by = 16;
        Line(bx + 4, by + 16, bx + 18, by + 16, 2, Ink(1));
        FillRound(bx + 6, by + 3, 10, 13, 5, Ink(1));
        FillEllipse(bx + 11, by + 19, 2.2f, 2.2f, Ink(1));
        if (!list.empty()) {
            std::wstring count = list.size() > 9 ? L"9+" : std::to_wstring(list.size());
            Fill(D2D1::RectF(wd->w - 16 - 26, 14, wd->w - 16, 38), kRed, 12);
            Text(count, wd->w - 16 - 26, 15, 26, 22, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, White(1), DWRITE_TEXT_ALIGNMENT_CENTER);
            Text(list[0].app + L" · " + AgoText(list[0].arrival), 16, 82, wd->w - 32, 16, 11.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.7f));
            Text(list[0].title, 16, 100, wd->w - 32, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
            Text(list[0].body, 16, 120, wd->w - 32, 18, 12.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.85f));
        } else {
            Text(error.empty() ? L"Nenhuma notificação" : error, 16, 100, wd->w - 32, 40, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.7f));
        }
        return;
    }
    Text(L"Notificações", 16, 12, wd->w - 120, 22, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
    if (!list.empty()) {
        D2D1_RECT_F c = NotifClearRect(wd);
        Fill(c, Ink(0.12f), 13);
        Text(L"Limpar", c.left, c.top + 4, c.right - c.left, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(1), DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    if (list.empty()) {
        Text(error.empty() ? L"Nenhuma notificação" : error, 16, 64, wd->w - 32, 40, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.7f));
        return;
    }
    int show = wd->size == 1 ? 2 : wd->size == 2 ? 4 : 6;
    float cardH = 62, y = 44;
    for (int i = 0; i < show && i < (int)list.size(); i++) {
        DrawNotifCard(12, y, wd->w - 24, cardH, list[i]);
        y += cardH + 8;
    }
    int more = (int)list.size() - show;
    if (more > 0 && wd->size >= 2)
        Text(Fmt(TF(more == 1 ? L"+ %d notificação anterior" : L"+ %d notificações anteriores"), (int)more), 16, wd->h - 30,
             wd->w - 32, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.65f), DWRITE_TEXT_ALIGNMENT_CENTER);
}

// The events still to come (or happening now), at most `max`
static std::vector<CalEvent> UpcomingEvents(size_t max) {
    std::vector<CalEvent> out;
    long long now = NowLocalMinutes();
    std::lock_guard<std::mutex> lock(g_dataMutex);
    for (auto& e : g_events)
        if (e.end > now && out.size() < max) out.push_back(e);
    return out;
}

static std::wstring EventWhen(const CalEvent& e) {
    long long now = NowLocalMinutes(), today = now - now % 1440;
    SYSTEMTIME s = FromMinutes(e.start);
    wchar_t b[48];
    std::wstring day;
    if (e.start - e.start % 1440 == today) day = L"";
    else if (e.start - e.start % 1440 == today + 1440) day = T(L"Amanhã") + L" ";
    else day = std::wstring(WeekdayShort(s.wDayOfWeek)) + L" " + std::to_wstring(s.wDay) + L" ";
    if (e.allDay) return day + T(L"Dia inteiro");
    swprintf(b, 48, L"%02d:%02d", s.wHour, s.wMinute);
    return day + b;
}

static void DrawEventRows(Widget* wd, float x, float y, float w, float h, const std::vector<CalEvent>& events, float rowH) {
    D2D1_COLOR_F accent = ColorOf(g_ui.accent);
    for (size_t i = 0; i < events.size() && y + rowH <= h + 0.5f; i++, y += rowH) {
        FillRound(x, y + 4, 4, rowH - 8, 2, accent);
        Text(events[i].title.empty() ? L"(sem título)" : events[i].title, x + 12, y + 3, w - 12, 18, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(EventWhen(events[i]), x + 12, y + 21, w - 12, 16, 11.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f));
    }
}

static void DrawCalendar(Widget* wd) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t day[64] = L"", month[64] = L"", monthYear[64] = L"";
    GetDateFormatEx(LangLocale(), 0, &st, L"dddd", day, 64, nullptr);
    GetDateFormatEx(LangLocale(), 0, &st, L"MMMM", month, 64, nullptr);
    GetDateFormatEx(LangLocale(), 0, &st, MonthYearFormat(), monthYear, 64, nullptr);
    CharUpperW(day);
    if (month[0]) month[0] = towupper(month[0]);
    if (monthYear[0]) monthYear[0] = towupper(monthYear[0]);
    std::wstring err;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        err = g_eventsError;
    }
    float W = wd->w, H = wd->h;
    if (wd->size == 0) {
        auto ev = UpcomingEvents(1);
        Text(day, 16, 16, W - 32, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, kRed);
        if (ev.empty()) {
            Text(std::to_wstring(st.wDay), 12, 34, W - 24, 80, 64, DWRITE_FONT_WEIGHT_NORMAL, Ink(1));
            Text(month, 16, 124, W - 32, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.8f));
        } else {
            Text(std::to_wstring(st.wDay), 14, 32, W - 28, 50, 40, DWRITE_FONT_WEIGHT_NORMAL, Ink(1));
            DrawEventRows(wd, 16, 98, W - 32, H - 8, ev, 44);
        }
    } else if (wd->size == 1) {
        auto ev = UpcomingEvents(3);
        Text(day, 16, 16, 120, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, kRed);
        Text(std::to_wstring(st.wDay), 12, 34, 120, 80, 64, DWRITE_FONT_WEIGHT_NORMAL, Ink(1));
        Text(month, 16, 124, 120, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.8f));
        if (ev.empty()) {
            DrawMonthGrid(150, 12, W - 162, H - 22, st, 11);
        } else {
            DrawEventRows(wd, 146, 12, W - 162, H - 8, ev, 46);
        }
    } else {
        float gridH = wd->size == 2 ? 170 : 236;
        Text(day, 18, 16, W - 36, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, kRed);
        Text(monthYear, 18, 34, W - 36, 26, 20, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        DrawMonthGrid(12, 66, W - 24, gridH, st, wd->size == 2 ? 13 : 15);
        float y = 66 + gridH + 8;
        Line(16, y, W - 16, y, 1, Ink(0.12f));
        auto ev = UpcomingEvents(wd->size == 2 ? 2 : 5);
        if (!ev.empty()) DrawEventRows(wd, 16, y + 8, W - 32, H - 6, ev, 44);
        else
            Text(!err.empty() ? err : g_ui.calendarIcs.empty() ? L"Ligue seu calendário em Ajustes > Calendário" : L"Nenhum evento",
                 16, y + 12, W - 32, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f));
    }
}

// ---------- Helpers for the widgets that list things ----------

static void TextWrap(const std::wstring& src, float x, float y, float w, float h, float size, DWRITE_FONT_WEIGHT weight,
                     D2D1_COLOR_F color) {
    const std::wstring& s = T(src);
    if (s.empty()) return;
    ComPtr<IDWriteTextFormat> fmt;
    if (FAILED(g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, weight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                                      size, L"", &fmt)))
        return;
    fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
    ComPtr<IDWriteInlineObject> ellipsis;
    if (SUCCEEDED(g_dw->CreateEllipsisTrimmingSign(fmt.Get(), &ellipsis))) {
        DWRITE_TRIMMING trim = {DWRITE_TRIMMING_GRANULARITY_WORD, 0, 0};
        fmt->SetTrimming(&trim, ellipsis.Get());
    }
    if (auto br = Solid(color))
        g_t->DrawText(s.c_str(), (UINT32)s.size(), fmt.Get(), D2D1::RectF(x, y, x + w, y + h), br.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

// A clickable place in the widget (DIPs) and what a click there does
static void AddAction(Widget* wd, float x, float y, float w, float h, const std::wstring& action) {
    wd->actions.push_back({D2D1::RectF(x, y, x + w, y + h), action});
}

// A round "+" button in the top-right corner
static void PlusButton(Widget* wd, const std::wstring& action) {
    float x = wd->w - 16 - 26, y = 12;
    FillEllipse(x + 13, y + 13, 13, 13, Ink(0.12f));
    Line(x + 13, y + 7, x + 13, y + 19, 2, Ink(0.9f));
    Line(x + 7, y + 13, x + 19, y + 13, 2, Ink(0.9f));
    AddAction(wd, x - 4, y - 4, 34, 34, action);
}

static std::wstring Number2(double v) {
    wchar_t b[32];
    swprintf(b, 32, v >= 1000 ? L"%.0f" : L"%.2f", v);
    std::wstring s = b;
    for (auto& c : s)
        if (c == L'.') c = L',';
    return s;
}

static std::wstring MinutesText(double secs) {
    long long m = (long long)(secs / 60);
    wchar_t b[32];
    if (m < 1) return L"< 1 min";
    if (m < 60) swprintf(b, 32, TF(L"%lld min"), m);
    else swprintf(b, 32, TF(L"%lld h %02lld min"), m / 60, m % 60);
    return b;
}

// ---------- Writing right in the Notes and Reminders widgets ----------

struct InlineEdit {
    Widget* wd = nullptr;
    int mode = 0;       // 1 a note, 2 a new reminder, 3 a reminder being changed
    std::wstring path;  // the note's file ("" until a new note gets text)
    std::wstring text;
    size_t caret = 0;
    size_t anchor = 0;  // the other end of the selection (== caret: nothing selected)
    bool selecting = false;  // the mouse is down in the text, selecting
    ULONGLONG lastClick = 0;  // for double (word) and triple (all) clicks
    int clicks = 0;
    int line = -1;  // the reminder's line in the file
    bool dirty = false, caretOn = true;
    float scroll = 0;
    D2D1_RECT_F area = {};  // where the text is (DIPs), set when drawn
};
static InlineEdit g_edit;
constexpr UINT_PTR kCaretTimer = 0x5E1, kSaveTimer = 0x5E2;

static bool IsEditing(Widget* wd, int mode = 0) { return g_edit.wd == wd && wd && (!mode || g_edit.mode == mode); }

static ComPtr<IDWriteTextLayout> EditLayout(float w) {
    bool note = g_edit.mode == 1;
    ComPtr<IDWriteTextFormat> fmt;
    ComPtr<IDWriteTextLayout> lay;
    if (FAILED(g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                      DWRITE_FONT_STRETCH_NORMAL, note ? 13.0f : 13.0f, L"", &fmt)))
        return lay;
    fmt->SetWordWrapping(note ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);
    const std::wstring& t = g_edit.text;
    if (FAILED(g_dw->CreateTextLayout(t.c_str(), (UINT32)t.size(), fmt.Get(), std::max(10.0f, w), note ? 100000.0f : 40.0f, &lay)))
        return lay;
    if (note) {  // the first line is the title, as in Notes
        size_t nl = t.find(L'\n');
        DWRITE_TEXT_RANGE r = {0, (UINT32)(nl == std::wstring::npos ? t.size() : nl)};
        if (r.length) {
            lay->SetFontWeight(DWRITE_FONT_WEIGHT_SEMI_BOLD, r);
            lay->SetFontSize(15.0f, r);
        }
    }
    return lay;
}

// The text being written, its blinking caret, and a click there moves the caret
static void DrawEditText(Widget* wd, const D2D1_RECT_F& area, PCWSTR placeholder, D2D1_COLOR_F caretColor) {
    bool note = g_edit.mode == 1;
    g_edit.area = area;
    float w = area.right - area.left, h = area.bottom - area.top;
    auto lay = EditLayout(w);
    if (!lay) return;
    float cx = 0, cy = 0;
    DWRITE_HIT_TEST_METRICS m = {};
    lay->HitTestTextPosition((UINT32)g_edit.caret, FALSE, &cx, &cy, &m);
    float lineH = m.height > 0 ? m.height : 18;
    float& sc = g_edit.scroll;  // keeps the caret in view
    if (note) {
        if (cy - sc > h - lineH) sc = cy - h + lineH;
        if (cy < sc) sc = cy;
    } else {
        if (cx - sc > w - 4) sc = cx - w + 4;
        if (cx < sc) sc = cx;
    }
    if (sc < 0) sc = 0;
    float ox = area.left - (note ? 0 : sc), oy = area.top - (note ? sc : 0);
    g_t->PushAxisAlignedClip(D2D1::RectF(area.left - 2, area.top, area.right + 2, area.bottom), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    if (g_edit.text.empty() && placeholder)
        Text(placeholder, area.left, area.top + (note ? 0 : 1), w, 22, note ? 15 : 13, note ? DWRITE_FONT_WEIGHT_SEMI_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
             Ink(0.35f));
    // The selection, under the text
    size_t s0 = std::min(g_edit.anchor, g_edit.caret), s1 = std::max(g_edit.anchor, g_edit.caret);
    bool sel = s1 > s0 && s1 <= g_edit.text.size();
    if (sel) {
        UINT32 count = 0;
        lay->HitTestTextRange((UINT32)s0, (UINT32)(s1 - s0), 0, 0, nullptr, 0, &count);
        std::vector<DWRITE_HIT_TEST_METRICS> rs(count);
        if (count && SUCCEEDED(lay->HitTestTextRange((UINT32)s0, (UINT32)(s1 - s0), 0, 0, rs.data(), count, &count)))
            for (auto& r : rs)
                FillRound(ox + r.left, oy + r.top, std::max(r.width, 4.0f), r.height, 3,
                          D2D1::ColorF(caretColor.r, caretColor.g, caretColor.b, 0.38f));
    }
    if (auto b = Solid(Ink(0.95f))) g_t->DrawTextLayout(D2D1::Point2F(ox, oy), lay.Get(), b.Get());
    if (g_edit.caretOn && !sel) FillRound(ox + cx - 0.8f, oy + cy + 1, 1.8f, lineH - 2, 0.9f, caretColor);
    g_t->PopAxisAlignedClip();
    AddAction(wd, area.left - 4, area.top - 4, w + 8, h + 8, L"editcaret");
}

// "OK" in the top-right corner while writing
static void DoneButton(Widget* wd, D2D1_COLOR_F color) {
    float x = wd->w - 16 - 46, y = 13;
    FillRound(x, y, 46, 24, 12, Ink(0.12f));
    Text(L"OK", x, y + 3, 46, 18, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, color, DWRITE_TEXT_ALIGNMENT_CENTER);
    AddAction(wd, x - 4, y - 4, 54, 32, L"editdone");
}

// ---------- Notes ----------

static void DrawNotes(Widget* wd) {
    std::vector<NoteInfo> notes;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        notes = g_notes;
    }
    float W = wd->w, H = wd->h;
    D2D1_COLOR_F yellow = D2D1::ColorF(1.0f, 0.80f, 0.0f, 1.0f);
    Text(L"Notas", 16, 14, W - 70, 22, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, yellow);
    if (IsEditing(wd, 1)) {
        DoneButton(wd, yellow);
        DrawEditText(wd, D2D1::RectF(16, 46, W - 16, H - 12), L"Título", yellow);
        return;
    }
    PlusButton(wd, L"newnote");
    if (notes.empty()) {
        Text(L"Nenhuma nota ainda", 16, 52, W - 32, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.9f));
        TextWrap(L"Clique em + e escreva aqui mesmo. Elas ficam em Documentos\\MacOS Widgets\\Notes", 16, 74, W - 32, H - 90, 12,
                 DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f));
        return;
    }
    if (wd->size <= 1) {
        const NoteInfo& n = notes[0];
        Text(n.title, 16, 46, W - 32, 22, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        TextWrap(n.text, 16, 70, W - 32, H - 84, 12.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.8f));
        AddAction(wd, 0, 40, W, H - 40, L"editnote:" + n.path);
        return;
    }
    float y = 48, rowH = 56;
    for (size_t i = 0; i < notes.size() && y + rowH <= H - 8; i++, y += rowH) {
        Text(notes[i].title, 16, y + 6, W - 32, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        std::wstring first = notes[i].text.substr(0, notes[i].text.find(L'\n'));
        Text(first.empty() ? L"Sem mais texto" : first, 16, y + 27, W - 32, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f));
        if (y + rowH * 2 <= H - 8) Line(16, y + rowH - 0.5f, W - 16, y + rowH - 0.5f, 1, Ink(0.08f));
        AddAction(wd, 8, y, W - 16, rowH, L"editnote:" + notes[i].path);
    }
}

// ---------- Reminders ----------

static void DrawReminders(Widget* wd) {
    std::vector<ReminderLine> list;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        list = g_reminders;
    }
    float W = wd->w, H = wd->h;
    D2D1_COLOR_F accent = ColorOf(g_ui.accent);
    int pending = 0;
    for (auto& r : list) pending += !r.done;
    Text(L"Lembretes", 16, 14, W - 110, 22, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, accent);
    Text(std::to_wstring(pending), 16, 12, W - (IsEditing(wd) ? 84 : 60), 26, 20, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1),
         DWRITE_TEXT_ALIGNMENT_TRAILING);
    bool editing = IsEditing(wd);
    if (editing) DoneButton(wd, accent);
    else PlusButton(wd, L"newrem");
    if (IsEditing(wd, 2)) {  // the new one is written at the top
        ReminderLine fresh;
        fresh.line = -2;
        list.insert(list.begin(), fresh);
    }
    float y = 46, rowH = wd->size == 0 ? 36 : 32;
    for (size_t i = 0; i < list.size() && y + rowH <= H - 6; i++, y += rowH) {
        const ReminderLine& r = list[i];
        float cy = y + rowH / 2;
        bool here = editing && (r.line == -2 || (g_edit.mode == 3 && r.line == g_edit.line));
        if (r.done) {
            FillEllipse(26, cy, 9, 9, accent);
            Line(21.5f, cy, 25, cy + 3.5f, 1.8f, White(1));
            Line(25, cy + 3.5f, 30.5f, cy - 3.5f, 1.8f, White(1));
        } else if (auto b = Solid(Ink(0.45f))) {
            g_t->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(26, cy), 8.5f, 8.5f), b.Get(), 1.5f);
        }
        if (here) {
            DrawEditText(wd, D2D1::RectF(44, cy - 10, W - 16, cy + 10), L"Novo lembrete", accent);
        } else {
            Text(r.text, 44, cy - 10, W - 60, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(r.done ? 0.45f : 0.95f));
            AddAction(wd, 44, y, W - 52, rowH, L"editrem:" + std::to_wstring(r.line));
        }
        if (i + 1 < list.size() && y + rowH * 2 <= H - 6) Line(44, y + rowH - 0.5f, W - 16, y + rowH - 0.5f, 1, Ink(0.08f));
        if (r.line >= 0) AddAction(wd, 8, y, 36, rowH, L"toggle:" + std::to_wstring(r.line));
    }
    if (list.empty()) Text(L"Clique em + para escrever um lembrete", 16, 52, W - 32, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f));
}

// ---------- Shortcuts ----------

struct Shortcut {
    std::wstring name, target;
};
static std::vector<Shortcut> ParseShortcuts(const std::wstring& s) {
    std::vector<Shortcut> out;
    for (const auto& item : SplitList(s, L";\n")) {
        Shortcut sc;
        size_t eq = item.find(L'=');
        if (eq != std::wstring::npos) {
            sc.name = item.substr(0, eq);
            sc.target = item.substr(eq + 1);
        } else {
            sc.target = item;
            size_t slash = item.find_last_of(L"\\/:");
            sc.name = slash == std::wstring::npos ? item : item.substr(slash + 1);
            size_t dot = sc.name.find_last_of(L'.');
            if (dot != std::wstring::npos && dot > 0) sc.name = sc.name.substr(0, dot);
        }
        while (!sc.name.empty() && sc.name.back() == L' ') sc.name.pop_back();
        while (!sc.target.empty() && sc.target.front() == L' ') sc.target.erase(0, 1);
        if (!sc.target.empty()) out.push_back(sc);
    }
    return out;
}

// The icon Windows shows for a program, file or folder (null for web addresses and the like)
static std::vector<std::pair<std::wstring, std::shared_ptr<Image>>> g_iconCache;
static std::shared_ptr<Image> IconFor(const std::wstring& target) {
    for (auto& c : g_iconCache)
        if (c.first == target) return c.second;
    std::shared_ptr<Image> img;
    std::wstring path = target;
    if (path.find(L'\\') == std::wstring::npos && path.find(L':') == std::wstring::npos) {
        wchar_t found[MAX_PATH];
        if (SearchPathW(nullptr, path.c_str(), nullptr, MAX_PATH, found, nullptr)) path = found;
    }
    ComPtr<IShellItemImageFactory> f;
    HBITMAP bmp = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&f))) &&
        SUCCEEDED(f->GetImage(SIZE{64, 64}, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &bmp)) && bmp) {
        BITMAP bm = {};
        GetObjectW(bmp, sizeof(bm), &bm);
        if (bm.bmWidth > 0 && bm.bmHeight > 0) {
            auto im = std::make_shared<Image>();
            im->w = bm.bmWidth;
            im->h = bm.bmHeight;
            im->px.resize((size_t)im->w * im->h);
            BITMAPINFO bi = {};
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = im->w;
            bi.bmiHeader.biHeight = -im->h;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            HDC dc = GetDC(nullptr);
            if (GetDIBits(dc, bmp, 0, im->h, im->px.data(), &bi, DIB_RGB_COLORS)) {
                bool anyAlpha = false;
                for (UINT32 p : im->px)
                    if (p >> 24) anyAlpha = true;
                if (!anyAlpha)
                    for (auto& p : im->px) p |= 0xFF000000;
                img = im;
            }
            ReleaseDC(nullptr, dc);
        }
        DeleteObject(bmp);
    }
    g_iconCache.push_back({target, img});
    return img;
}

static void DrawIconImage(Widget* wd, const std::wstring& key, const std::shared_ptr<Image>& img, const D2D1_RECT_F& r) {
    ID2D1Bitmap* bmp = nullptr;
    for (auto& b : wd->iconBmps)
        if (b.first == key) bmp = b.second.Get();
    if (!bmp) {
        ComPtr<ID2D1Bitmap> nb;
        g_t->CreateBitmap(D2D1::SizeU(img->w, img->h), img->px.data(), img->w * 4,
                          D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &nb);
        if (!nb) return;
        wd->iconBmps.push_back({key, nb});
        bmp = nb.Get();
    }
    g_t->DrawBitmap(bmp, r, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
}

// The starting shortcuts have English names; they show in the UI's language
static std::wstring ShortcutLabel(const std::wstring& name) {
    static const std::pair<PCWSTR, PCWSTR> known[] = {{L"File Explorer", L"Explorador de Arquivos"}, {L"Settings", L"Configurações"},
                                                     {L"Calculator", L"Calculadora"}, {L"Notepad", L"Bloco de Notas"}};
    for (const auto& k : known)
        if (name == k.first) return T(k.second);
    return name;
}

static void DrawShortcuts(Widget* wd) {
    auto list = ParseShortcuts(g_ui.shortcuts);
    int cols = wd->size == 0 ? 2 : 4, rows = wd->size == 0 ? 2 : wd->size == 1 ? 2 : wd->size == 2 ? 4 : 6;
    float cw = (wd->w - 16) / cols, ch = (wd->h - 16) / rows;
    if (list.empty()) {
        Text(L"Atalhos", 16, 14, wd->w - 32, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        TextWrap(L"Escolha os apps, pastas e sites em Ajustes > Atalhos", 16, 44, wd->w - 32, wd->h - 60, 12,
                 DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f));
        return;
    }
    for (int i = 0; i < (int)list.size() && i < cols * rows; i++) {
        float x = 8 + (i % cols) * cw, y = 8 + (i / cols) * ch;
        D2D1_RECT_F tile = D2D1::RectF(x + 4, y + 4, x + cw - 4, y + ch - 4);
        Fill(tile, Ink(0.07f), 14);
        float icon = std::min(40.0f, ch - 40), cx = (tile.left + tile.right) / 2, iy = tile.top + (tile.bottom - tile.top - icon - 22) / 2;
        auto img = IconFor(list[i].target);
        if (img) {
            DrawIconImage(wd, list[i].target, img, D2D1::RectF(cx - icon / 2, iy, cx + icon / 2, iy + icon));
        } else {
            FillEllipse(cx, iy + icon / 2, icon / 2, icon / 2, AppColor(list[i].name));
            Text(list[i].name.substr(0, 1), cx - icon / 2, iy + icon / 2 - icon * 0.35f, icon, icon * 0.7f, icon * 0.45f,
                 DWRITE_FONT_WEIGHT_SEMI_BOLD, White(1), DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        Text(ShortcutLabel(list[i].name), tile.left + 4, iy + icon + 4, tile.right - tile.left - 8, 18, 11.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.9f),
             DWRITE_TEXT_ALIGNMENT_CENTER);
        AddAction(wd, tile.left, tile.top, tile.right - tile.left, tile.bottom - tile.top, L"open:" + list[i].target);
    }
}

// ---------- Stocks ----------

static void DrawStocks(Widget* wd) {
    std::vector<Quote> list;
    std::wstring error;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        list = g_quotes;
        error = g_quotesError;
    }
    D2D1_COLOR_F green = D2D1::ColorF(0.19f, 0.82f, 0.35f, 1.0f);
    float W = wd->w, H = wd->h;
    if (list.empty()) {
        Text(L"Bolsa", 16, 14, W - 32, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        PCWSTR hint = g_ui.stocks.empty() ? L"Clique para escolher os códigos" : L"Carregando…";
        TextWrap(error.empty() ? hint : error, 16, 44, W - 32, H - 60, 12.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f));
        return;
    }
    auto change = [](const Quote& q) { return q.prev > 0 ? (q.price - q.prev) / q.prev * 100 : 0.0; };
    auto pctText = [](double p) {
        wchar_t b[32];
        swprintf(b, 32, L"%+.2f%%", p);
        std::wstring s = b;
        for (auto& c : s)
            if (c == L'.') c = L',';
        return s;
    };
    if (wd->size == 0) {
        const Quote& q = list[0];
        double p = change(q);
        Text(q.symbol, 16, 14, W - 32, 20, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(q.name, 16, 34, W - 32, 16, 11, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f));
        if (q.closes.size() > 1) DrawSparkline(16, 56, W - 32, 40, q.closes.data(), (int)q.closes.size(), p >= 0 ? green : kRed, true);
        Text(Number2(q.price), 16, 104, W - 32, 30, 22, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(pctText(p), 16, 134, W - 32, 18, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, p >= 0 ? green : kRed);
        return;
    }
    int rows = wd->size == 1 ? 3 : wd->size == 2 ? 6 : 9;
    float rowH = (H - 16) / rows;
    for (int i = 0; i < rows && i < (int)list.size(); i++) {
        const Quote& q = list[i];
        double p = change(q);
        float y = 8 + i * rowH, cy = y + rowH / 2;
        Text(q.symbol, 16, cy - 18, 110, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(q.name, 16, cy + 2, 110, 16, 11, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.55f));
        if (q.closes.size() > 1) DrawSparkline(132, cy - 14, 80, 28, q.closes.data(), (int)q.closes.size(), p >= 0 ? green : kRed, true);
        Text(Number2(q.price), 216, cy - 18, W - 232, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_TRAILING);
        D2D1_RECT_F pill = D2D1::RectF(W - 16 - 66, cy + 1, W - 16, cy + 19);
        Fill(pill, p >= 0 ? green : kRed, 5);
        Text(pctText(p), pill.left, pill.top + 1, 66, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, White(1), DWRITE_TEXT_ALIGNMENT_CENTER);
        if (i + 1 < rows && i + 1 < (int)list.size()) Line(16, y + rowH - 0.5f, W - 16, y + rowH - 0.5f, 1, Ink(0.08f));
    }
}

// ---------- News ----------

// A picture filling 'r' (cropped to keep its shape), seen only inside 'clip' (rounded). Each picture is turned into a
// bitmap once per widget
static void DrawCover(Widget* wd, const std::wstring& key, const std::shared_ptr<Image>& img, const D2D1_RECT_F& r,
                      const D2D1_RECT_F& clip, float radius) {
    if (!img || img->w <= 0 || img->h <= 0) return;
    ID2D1Bitmap* bmp = nullptr;
    for (auto& b : wd->iconBmps)
        if (b.first == key) bmp = b.second.Get();
    if (!bmp) {
        if (wd->iconBmps.size() > 24) wd->iconBmps.clear();
        ComPtr<ID2D1Bitmap> nb;
        g_t->CreateBitmap(D2D1::SizeU(img->w, img->h), img->px.data(), img->w * 4,
                          D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &nb);
        if (!nb) return;
        wd->iconBmps.push_back({key, nb});
        bmp = nb.Get();
    }
    float rw = r.right - r.left, rh = r.bottom - r.top;
    float k = std::max(rw / img->w, rh / img->h);
    float sw = rw / k, sh = rh / k, sx = (img->w - sw) / 2, sy = (img->h - sh) / 2.6f;  // a bit above the middle: faces
    ComPtr<ID2D1RoundedRectangleGeometry> geo;
    if (FAILED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(clip, radius, radius), &geo))) return;
    g_t->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), geo.Get()), nullptr);
    g_t->PushAxisAlignedClip(r, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    D2D1_RECT_F src = D2D1::RectF(sx, sy, sx + sw, sy + sh);
    g_t->DrawBitmap(bmp, r, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &src);
    g_t->PopAxisAlignedClip();
    g_t->PopLayer();
}

// A dark shade over the bottom of a picture, so white text reads on it
static void CoverShade(const D2D1_RECT_F& r, const D2D1_RECT_F& clip, float radius, float from) {
    ComPtr<ID2D1RoundedRectangleGeometry> geo;
    if (FAILED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(clip, radius, radius), &geo))) return;
    g_t->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), geo.Get()), nullptr);
    float top = r.top + (r.bottom - r.top) * from;
    D2D1_GRADIENT_STOP st[3] = {{0.0f, D2D1::ColorF(0, 0, 0, 0)}, {0.45f, D2D1::ColorF(0, 0, 0, 0.45f)}, {1.0f, D2D1::ColorF(0, 0, 0, 0.82f)}};
    if (auto b = MakeGradient(st, 3, D2D1::Point2F(0, top), D2D1::Point2F(0, r.bottom)))
        g_t->FillRectangle(D2D1::RectF(r.left, top, r.right, r.bottom), b.Get());
    g_t->PopLayer();
}

// The red "Notícias" tag over a picture
static void NewsTag(float x, float y) {
    FillRound(x, y, 74, 20, 10, kRed);
    Text(L"Notícias", x, y + 2, 74, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, White(1), DWRITE_TEXT_ALIGNMENT_CENTER);
}

// "g1 · há 12 min"
static std::wstring SourceLine(const NewsItem& n) {
    std::wstring ago = NewsAgo(n.when);
    return ago.empty() ? n.source : n.source + L" · " + ago;
}

static void DrawNews(Widget* wd) {
    std::vector<NewsItem> list;
    std::wstring error;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        list = g_news;
        error = g_newsError;
    }
    float W = wd->w, H = wd->h, rad = (float)g_ui.cornerRadius;
    D2D1_RECT_F all = D2D1::RectF(0, 0, W, H);
    // Where the wheel left it (back to the top stories after a few minutes untouched)
    if (wd->newsFirst && GetTickCount64() - wd->newsWheel > 4 * 60 * 1000) wd->newsFirst = 0;
    if (wd->newsFirst >= (int)list.size()) wd->newsFirst = 0;
    int total = (int)list.size();
    if (wd->newsFirst > 0) list.erase(list.begin(), list.begin() + wd->newsFirst);
    if (list.empty()) {
        Text(L"Notícias", 16, 12, W - 32, 22, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, kRed);
        PCWSTR hint = g_ui.newsFeeds.empty() ? L"Clique para escolher os sites de notícias" : L"Carregando…";
        TextWrap(error.empty() ? hint : error, 16, 44, W - 32, H - 60, 12.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f));
        return;
    }
    const NewsItem& top = list[0];
    // Which page of how many, in the top-right corner, once the wheel has moved
    auto pageMark = [&](bool onPicture) {
        if (wd->newsFirst == 0 || wd->newsStep <= 0) return;
        int pages = (total + wd->newsStep - 1) / wd->newsStep, page = wd->newsFirst / wd->newsStep + 1;
        std::wstring t = std::to_wstring(std::min(page, pages)) + L"/" + std::to_wstring(pages);
        FillRound(W - 14 - 46, 14, 46, 20, 10, onPicture ? D2D1::ColorF(0, 0, 0, 0.45f) : Ink(0.10f));
        Text(t, W - 14 - 46, 16, 46, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, onPicture ? White(0.95f) : Ink(0.8f),
             DWRITE_TEXT_ALIGNMENT_CENTER);
    };
    // Small and medium: the main story fills the widget, its headline over the picture
    if (wd->size <= 1) {
        wd->newsStep = 1;
        if (top.img) {
            DrawCover(wd, L"n:" + top.image, top.img, all, all, rad);
            CoverShade(all, all, rad, 0.25f);
            NewsTag(14, 14);
            float tw = W - 32, th = wd->size == 0 ? 60 : 44;
            Text(SourceLine(top), 16, H - th - 34, tw, 15, 10.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD, White(0.75f));
            TextWrap(top.title, 16, H - th - 16, tw, th + 4, wd->size == 0 ? 13.5f : 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, White(1));
        } else {
            Text(L"Notícias", 16, 12, W - 32, 22, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, kRed);
            Text(SourceLine(top), 16, 38, W - 32, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.6f));
            TextWrap(top.title, 16, 56, W - 32, H - 68, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        }
        pageMark(top.img != nullptr);
        AddAction(wd, 0, 0, W, H, L"url:" + top.link);
        return;
    }
    // Large and extra large: the main story's picture on top, the next ones in rows with a thumbnail
    float heroH = wd->size == 2 ? 176 : 232;
    size_t next = 0;
    float y;
    if (top.img) {
        D2D1_RECT_F hero = D2D1::RectF(0, 0, W, heroH);
        DrawCover(wd, L"n:" + top.image, top.img, hero, all, rad);
        CoverShade(hero, all, rad, 0.2f);
        NewsTag(14, 14);
        Text(SourceLine(top), 16, heroH - 68, W - 32, 15, 10.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD, White(0.75f));
        TextWrap(top.title, 16, heroH - 50, W - 32, 44, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, White(1));
        AddAction(wd, 0, 0, W, heroH, L"url:" + top.link);
        next = 1;
        y = heroH + 4;
    } else {
        Text(L"Notícias", 16, 12, W - 32, 22, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, kRed);
        y = 40;
    }
    int rows = (int)((H - y - 6) / 76);
    float rowH = (H - y - 6) / std::max(1, rows);
    wd->newsStep = (int)next + std::max(1, rows);
    pageMark(top.img != nullptr);
    for (int i = 0; i < rows && next < list.size(); i++, next++, y += rowH) {
        const NewsItem& n = list[next];
        float thumb = rowH - 18, tw = W - 32 - (n.img ? thumb + 12 : 0);
        if (n.img) {
            D2D1_RECT_F t = D2D1::RectF(W - 16 - thumb, y + 9, W - 16, y + 9 + thumb);
            DrawCover(wd, L"n:" + n.image, n.img, t, t, 10);
        }
        Text(SourceLine(n), 16, y + 8, tw, 15, 10.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.55f));
        TextWrap(n.title, 16, y + 24, tw, rowH - 30, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        if (i + 1 < rows && next + 1 < list.size()) Line(16, y + rowH - 0.5f, W - 16, y + rowH - 0.5f, 1, Ink(0.08f));
        AddAction(wd, 8, y, W - 16, rowH, L"url:" + n.link);
    }
}

// ---------- Screen time ----------

struct AppUse {
    std::wstring name;
    double secs = 0;
};
static std::vector<AppUse> g_usage;  // today, UI thread
static int g_usageDay = -1;
static ULONGLONG g_usageSaved = 0;
static std::vector<std::pair<std::wstring, std::wstring>> g_appNames;  // exe path -> its description

static std::wstring AppDescription(const std::wstring& path) {
    for (auto& p : g_appNames)
        if (p.first == path) return p.second;
    std::wstring name;
    DWORD dummy = 0, size = GetFileVersionInfoSizeW(path.c_str(), &dummy);
    if (size) {
        std::vector<BYTE> data(size);
        struct Lang {
            WORD lang, cp;
        }* langs = nullptr;
        UINT len = 0;
        if (GetFileVersionInfoW(path.c_str(), 0, size, data.data()) &&
            VerQueryValueW(data.data(), L"\\VarFileInfo\\Translation", (void**)&langs, &len) && len >= sizeof(Lang)) {
            wchar_t key[64];
            swprintf(key, 64, L"\\StringFileInfo\\%04x%04x\\FileDescription", langs[0].lang, langs[0].cp);
            wchar_t* desc = nullptr;
            if (VerQueryValueW(data.data(), key, (void**)&desc, &len) && desc && *desc) name = desc;
        }
    }
    if (name.empty()) {
        size_t slash = path.find_last_of(L'\\');
        name = slash == std::wstring::npos ? path : path.substr(slash + 1);
        if (name.size() > 4 && _wcsicmp(name.c_str() + name.size() - 4, L".exe") == 0) name.resize(name.size() - 4);
    }
    g_appNames.push_back({path, name});
    return name;
}

static int TodayKey() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    return st.wYear * 10000 + st.wMonth * 100 + st.wDay;
}

static void SaveUsage() {
    std::wstring s = std::to_wstring(g_usageDay) + kRecSep;
    for (auto& u : g_usage) s += CleanValue(u.name) + L"\t" + std::to_wstring((long long)u.secs) + kRecSep;
    Wh_SetStringValue(L"screenTime", s.c_str());
}

// Called every second: the app in front gets the second (not while away from the PC for 5 minutes)
static void TrackUsage() {
    int today = TodayKey();
    if (g_usageDay < 0) {
        auto lines = Split(ReadValue(L"screenTime"), kRecSep);
        if (!lines.empty() && _wtoi(lines[0].c_str()) == today) {
            for (size_t i = 1; i < lines.size(); i++) {
                auto f = Split(lines[i], L'\t');
                if (f.size() == 2 && !f[0].empty()) g_usage.push_back({f[0], (double)_wtoi(f[1].c_str())});
            }
        }
        g_usageDay = today;
    }
    if (today != g_usageDay) {
        g_usage.clear();
        g_usageDay = today;
    }
    LASTINPUTINFO li = {sizeof(li)};
    if (GetLastInputInfo(&li) && GetTickCount() - li.dwTime > 5 * 60 * 1000) return;
    HWND fg = GetForegroundWindow();
    DWORD pid = 0;
    if (!fg || !GetWindowThreadProcessId(fg, &pid) || !pid) return;
    std::wstring name;
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (p) {
        wchar_t path[MAX_PATH];
        DWORD n = MAX_PATH;
        if (QueryFullProcessImageNameW(p, 0, path, &n)) name = AppDescription(path);
        CloseHandle(p);
    }
    if (name.empty()) return;
    bool found = false;
    for (auto& u : g_usage)
        if (u.name == name) {
            u.secs += 1;
            found = true;
        }
    if (!found) g_usage.push_back({name, 1});
    if (GetTickCount64() - g_usageSaved > 60 * 1000) {
        g_usageSaved = GetTickCount64();
        SaveUsage();
    }
}

static void DrawScreenTime(Widget* wd) {
    std::vector<AppUse> list = g_usage;
    std::sort(list.begin(), list.end(), [](const AppUse& a, const AppUse& b) { return a.secs > b.secs; });
    double total = 0;
    for (auto& u : list) total += u.secs;
    float W = wd->w, H = wd->h;
    D2D1_COLOR_F purple = D2D1::ColorF(0.55f, 0.36f, 0.96f, 1.0f);
    Text(L"Tempo de Uso", 16, 14, W - 80, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, purple);
    Text(L"Hoje", 16, 14, W - 32, 20, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f), DWRITE_TEXT_ALIGNMENT_TRAILING);
    Text(MinutesText(total), 16, 36, W - 32, 34, 26, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
    double top = list.empty() ? 1 : std::max(1.0, list[0].secs);
    int rows = wd->size == 0 ? 2 : wd->size == 1 ? 2 : wd->size == 2 ? 6 : 10;
    float y = 80, rowH = wd->size == 1 ? 36 : 40;
    D2D1_COLOR_F barColors[3] = {purple, D2D1::ColorF(0.39f, 0.82f, 1.0f, 1.0f), D2D1::ColorF(1.0f, 0.62f, 0.04f, 1.0f)};
    for (int i = 0; i < rows && i < (int)list.size() && y + rowH <= H - 4; i++, y += rowH) {
        Text(list[i].name, 16, y, W - 110, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.9f));
        Text(MinutesText(list[i].secs), 16, y, W - 32, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_TRAILING);
        FillRound(16, y + 21, W - 32, 5, 2.5f, Ink(0.12f));
        FillRound(16, y + 21, std::max(5.0f, (float)((W - 32) * list[i].secs / top)), 5, 2.5f, barColors[std::min(i, 2)]);
    }
    if (list.empty())
        TextWrap(L"Conta o tempo de cada app a partir de agora (enquanto você usa o PC)", 16, 80, W - 32, H - 90, 12,
                 DWRITE_FONT_WEIGHT_NORMAL, Ink(0.6f));
}

// ---------- Home ----------

static bool HaIsOn(const HaEntity& e) {
    const std::wstring& s = e.state;
    return s == L"on" || s == L"open" || s == L"opening" || s == L"playing" || s == L"heat" || s == L"cool" || s == L"auto" ||
           s == L"heat_cool" || s == L"unlocked";
}

static std::wstring HaStateText(const HaEntity& e) {
    std::wstring d = e.domain(), s = e.state;
    if (s == L"unavailable") return L"Indisponível";
    if (s == L"unknown") return L"—";
    if (d == L"sensor") {
        std::wstring v = s;
        for (auto& c : v)
            if (c == L'.') c = L',';
        return v + (e.unit.empty() ? L"" : (e.unit[0] == L'°' || e.unit == L"%" ? L"" : L" ") + e.unit);
    }
    if (d == L"scene" || d == L"script") return L"Tocar para ativar";
    if (d == L"cover") return s == L"open" ? L"Aberta" : s == L"closed" ? L"Fechada" : s == L"opening" ? L"Abrindo" : L"Fechando";
    if (d == L"lock") return s == L"locked" ? L"Trancada" : s == L"unlocked" ? L"Destrancada" : s;
    if (d == L"media_player") return s == L"playing" ? L"Tocando" : s == L"paused" ? L"Pausado" : s == L"off" ? L"Desligado" : L"Ligado";
    if (d == L"climate") return s == L"off" ? L"Desligado" : s == L"cool" ? L"Resfriando" : s == L"heat" ? L"Aquecendo" : L"Ligado";
    if (s == L"on") return d == L"light" ? L"Acesa" : L"Ligado";
    if (s == L"off") return d == L"light" ? L"Apagada" : L"Desligado";
    return s;
}

// Small line drawings for each kind of thing
static void HaIcon(const HaEntity& e, float cx, float cy, float s, D2D1_COLOR_F c) {
    std::wstring d = e.domain();
    float w = std::max(1.6f, s * 0.09f);
    if (d == L"light") {  // a bulb
        if (auto b = Solid(c)) g_t->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy - s * 0.10f), s * 0.28f, s * 0.28f), b.Get(), w);
        Line(cx - s * 0.13f, cy + s * 0.26f, cx + s * 0.13f, cy + s * 0.26f, w, c);
        Line(cx - s * 0.10f, cy + s * 0.38f, cx + s * 0.10f, cy + s * 0.38f, w, c);
    } else if (d == L"sensor" || d == L"climate") {  // a thermometer
        if (auto b = Solid(c))
            g_t->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - s * 0.09f, cy - s * 0.42f, cx + s * 0.09f, cy + s * 0.14f), s * 0.09f,
                                                        s * 0.09f),
                                      b.Get(), w);
        FillEllipse(cx, cy + s * 0.24f, s * 0.16f, s * 0.16f, c);
    } else if (d == L"lock") {  // a padlock
        FillRound(cx - s * 0.26f, cy - s * 0.04f, s * 0.52f, s * 0.40f, s * 0.08f, c);
        if (auto b = Solid(c)) {
            ComPtr<ID2D1PathGeometry> arc;
            ComPtr<ID2D1GeometrySink> sink;
            if (SUCCEEDED(g_d2d->CreatePathGeometry(&arc)) && SUCCEEDED(arc->Open(&sink))) {
                sink->BeginFigure(D2D1::Point2F(cx - s * 0.17f, cy - s * 0.04f), D2D1_FIGURE_BEGIN_HOLLOW);
                sink->AddLine(D2D1::Point2F(cx - s * 0.17f, cy - s * 0.18f));
                sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(cx + s * 0.17f, cy - s * 0.18f), D2D1::SizeF(s * 0.17f, s * 0.17f), 0,
                                              D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
                sink->AddLine(D2D1::Point2F(cx + s * 0.17f, cy - s * 0.04f));
                sink->EndFigure(D2D1_FIGURE_END_OPEN);
                sink->Close();
                g_t->DrawGeometry(arc.Get(), b.Get(), w);
            }
        }
    } else if (d == L"scene" || d == L"script" || d == L"media_player") {  // play
        ComPtr<ID2D1PathGeometry> tri;
        ComPtr<ID2D1GeometrySink> sink;
        if (SUCCEEDED(g_d2d->CreatePathGeometry(&tri)) && SUCCEEDED(tri->Open(&sink))) {
            sink->BeginFigure(D2D1::Point2F(cx - s * 0.18f, cy - s * 0.26f), D2D1_FIGURE_BEGIN_FILLED);
            sink->AddLine(D2D1::Point2F(cx + s * 0.26f, cy));
            sink->AddLine(D2D1::Point2F(cx - s * 0.18f, cy + s * 0.26f));
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            sink->Close();
            if (auto b = Solid(c)) g_t->FillGeometry(tri.Get(), b.Get());
        }
    } else if (d == L"cover") {  // blinds
        for (int i = 0; i < 4; i++) Line(cx - s * 0.28f, cy - s * 0.24f + i * s * 0.15f, cx + s * 0.28f, cy - s * 0.24f + i * s * 0.15f, w, c);
    } else {  // power
        Line(cx, cy - s * 0.36f, cx, cy - s * 0.04f, w, c);
        if (auto b = Solid(c)) {
            ComPtr<ID2D1PathGeometry> arc;
            ComPtr<ID2D1GeometrySink> sink;
            float r = s * 0.28f;
            if (SUCCEEDED(g_d2d->CreatePathGeometry(&arc)) && SUCCEEDED(arc->Open(&sink))) {
                sink->BeginFigure(D2D1::Point2F(cx - r * 0.70f, cy - r * 0.62f), D2D1_FIGURE_BEGIN_HOLLOW);
                sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(cx + r * 0.70f, cy - r * 0.62f), D2D1::SizeF(r, r), 0,
                                              D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
                sink->EndFigure(D2D1_FIGURE_END_OPEN);
                sink->Close();
                ComPtr<ID2D1StrokeStyle> round;
                g_d2d->CreateStrokeStyle(D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND), nullptr, 0, &round);
                g_t->DrawGeometry(arc.Get(), b.Get(), w, round.Get());
            }
        }
    }
}

static void DrawHome(Widget* wd) {
    std::vector<HaEntity> list;
    std::wstring error;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        list = g_ha;
        error = g_haError;
    }
    float W = wd->w, H = wd->h;
    if (list.empty()) {
        Text(L"Casa", 16, 14, W - 32, 20, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, D2D1::ColorF(1.0f, 0.62f, 0.04f, 1.0f));
        TextWrap(error.empty() ? L"Carregando…" : error, 16, 44, W - 32, H - 56, 12.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.65f));
        return;
    }
    int cols = wd->size == 0 ? 2 : 4, rows = wd->size == 0 ? 2 : wd->size == 1 ? 2 : wd->size == 2 ? 4 : 6;
    float cw = (W - 16) / cols, ch = (H - 16) / rows;
    D2D1_COLOR_F warm = D2D1::ColorF(1.0f, 0.80f, 0.30f, 1.0f), accent = ColorOf(g_ui.accent);
    for (int i = 0; i < (int)list.size() && i < cols * rows; i++) {
        const HaEntity& e = list[i];
        float x = 8 + (i % cols) * cw, y = 8 + (i / cols) * ch;
        D2D1_RECT_F tile = D2D1::RectF(x + 4, y + 4, x + cw - 4, y + ch - 4);
        bool on = HaIsOn(e), off = e.state == L"unavailable";
        bool sensor = e.domain() == L"sensor";
        D2D1_COLOR_F tint = e.domain() == L"light" ? warm : accent;
        // On: a light tile (like the Mac's Home tiles); off: the plain glass tile
        Fill(tile, on && !sensor ? Alpha(tint, 0.90f) : Ink(0.08f), 14);
        D2D1_COLOR_F ink = on && !sensor ? D2D1::ColorF(0.10f, 0.10f, 0.12f, 1.0f) : Ink(off ? 0.4f : 0.95f);
        D2D1_COLOR_F sub = on && !sensor ? D2D1::ColorF(0.10f, 0.10f, 0.12f, 0.65f) : Ink(off ? 0.3f : 0.6f);
        float iconS = std::min(26.0f, ch * 0.32f);
        HaIcon(e, tile.left + 10 + iconS / 2, tile.top + 10 + iconS / 2, iconS, on && !sensor ? ink : (sensor ? accent : Ink(0.7f)));
        float th = tile.bottom - tile.top;
        if (sensor && th > 60) {
            Text(HaStateText(e), tile.left + 10, tile.top + 10 + iconS + 2, tile.right - tile.left - 16, 26, 18, DWRITE_FONT_WEIGHT_SEMI_BOLD, ink);
            Text(e.name, tile.left + 10, tile.bottom - 24, tile.right - tile.left - 16, 18, 11.5f, DWRITE_FONT_WEIGHT_NORMAL, sub);
        } else {
            TextWrap(e.name, tile.left + 10, tile.bottom - 46, tile.right - tile.left - 16, 30, 11.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD, ink);
            Text(HaStateText(e), tile.left + 10, tile.bottom - 18, tile.right - tile.left - 16, 15, 10.5f, DWRITE_FONT_WEIGHT_NORMAL, sub);
        }
        if (HaService(e) && !off) AddAction(wd, tile.left, tile.top, tile.right - tile.left, tile.bottom - tile.top, L"ha:" + e.id);
    }
}

// A tap: the tile changes at once, the command goes to Home Assistant on the data thread
static void HaTap(const std::wstring& id) {
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        for (auto& e : g_ha) {
            if (e.id != id) continue;
            PCWSTR svc = HaService(e);
            if (!svc) return;
            std::wstring d = e.domain();
            if (d == L"light" || d == L"switch" || d == L"fan" || d == L"input_boolean") e.state = e.state == L"on" ? L"off" : L"on";
            else if (d == L"cover") e.state = e.state == L"open" ? L"closing" : L"opening";
            g_haCommands.push_back({svc, id});
            break;
        }
    }
    g_cv.notify_all();
}

// Where the music buttons are for this size (x of prev / play / next; a negative x = not shown), in DIPs
struct MusicButtons {
    float x[3], y, size;
};
static MusicButtons MusicButtonsFor(const Widget* wd) {
    if (wd->size == 0) return {{-1000, wd->w - 16 - 40, -1000}, 16, 40};
    if (wd->size == 1) return {{162, 212, 262}, 100, 44};
    float c = wd->w / 2;
    return {{c - 22 - 54, c - 22, c - 22 + 54}, wd->size == 3 ? wd->h - 60 : 286, 44};
}

// An outlined shape (closed), drawn with rounded corners, as the media controls of Windows 11
static void OutlineShape(std::initializer_list<D2D1_POINT_2F> pts, float width, D2D1_COLOR_F c) {
    ComPtr<ID2D1PathGeometry> path;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(g_d2d->CreatePathGeometry(&path)) || FAILED(path->Open(&sink))) return;
    auto it = pts.begin();
    sink->BeginFigure(*it, D2D1_FIGURE_BEGIN_HOLLOW);
    for (++it; it != pts.end(); ++it) sink->AddLine(*it);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    ComPtr<ID2D1StrokeStyle> round;
    g_d2d->CreateStrokeStyle(D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                                         D2D1_LINE_JOIN_ROUND),
                             nullptr, 0, &round);
    if (auto b = Solid(c)) g_t->DrawGeometry(path.Get(), b.Get(), width, round.Get());
}

static void DrawMusicButtons(const Widget* wd, const MediaData& m) {
    MusicButtons b = MusicButtonsFor(wd);
    D2D1_COLOR_F ic = Ink(m.hasSession ? 1.0f : 0.4f);
    float cy = b.y + b.size / 2, k = b.size / 44, sw = 2.0f;
    int pressed = wd->pressBtn;
    for (int i = 0; i < 3; i++) {
        if (b.x[i] < -100) continue;
        float cx = b.x[i] + b.size / 2;
        // A round light under the button the mouse is on (stronger while pressed); the small widget's only button
        // always has a faint one, as it stands alone in the corner
        float a = i == pressed ? 0.26f : i == wd->hoverBtn ? 0.15f : wd->size == 0 ? 0.10f : 0.0f;
        if (a > 0 && m.hasSession) FillEllipse(cx, cy, b.size / 2, b.size / 2, Ink(a));
        float s = i == pressed ? 0.92f : 1.0f;  // pressed: a little smaller
        float kk = k * s;
        if (i == 0) {  // previous: a bar and a triangle pointing left
            OutlineShape({D2D1::Point2F(cx - 7.5f * kk, cy - 8 * kk), D2D1::Point2F(cx - 7.5f * kk, cy + 8 * kk)}, sw, ic);
            OutlineShape({D2D1::Point2F(cx + 7.5f * kk, cy - 8 * kk), D2D1::Point2F(cx - 3.5f * kk, cy),
                          D2D1::Point2F(cx + 7.5f * kk, cy + 8 * kk)},
                         sw, ic);
        } else if (i == 2) {  // next: mirrored
            OutlineShape({D2D1::Point2F(cx + 7.5f * kk, cy - 8 * kk), D2D1::Point2F(cx + 7.5f * kk, cy + 8 * kk)}, sw, ic);
            OutlineShape({D2D1::Point2F(cx - 7.5f * kk, cy - 8 * kk), D2D1::Point2F(cx + 3.5f * kk, cy),
                          D2D1::Point2F(cx - 7.5f * kk, cy + 8 * kk)},
                         sw, ic);
        } else if (m.playing) {  // pause: two outlined bars
            for (float x0 : {-7.0f, 2.0f}) {
                D2D1_RECT_F r = D2D1::RectF(cx + x0 * kk, cy - 9 * kk, cx + (x0 + 5) * kk, cy + 9 * kk);
                if (auto br = Solid(ic)) g_t->DrawRoundedRectangle(D2D1::RoundedRect(r, 1.8f * kk, 1.8f * kk), br.Get(), sw);
            }
        } else {  // play: an outlined triangle
            OutlineShape({D2D1::Point2F(cx - 5 * kk, cy - 9.5f * kk), D2D1::Point2F(cx + 9 * kk, cy),
                          D2D1::Point2F(cx - 5 * kk, cy + 9.5f * kk)},
                         sw, ic);
        }
    }
}

static void DrawArt(Widget* wd, const std::shared_ptr<Image>& img, const D2D1_RECT_F& r, float radius) {
    if (img) {
        DrawImageFill(wd, img, r, radius);
        return;
    }
    D2D1_GRADIENT_STOP art[] = {{0.0f, D2D1::ColorF(0xF0A35E)}, {0.55f, D2D1::ColorF(0xC2417A)}, {1.0f, D2D1::ColorF(0x4B2A8C)}};
    if (auto br = MakeGradient(art, 3, D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.right, r.bottom)))
        g_t->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), br.Get());
    float h = r.bottom - r.top;
    Text(L"♪", r.left, r.top, r.right - r.left, h, h * 0.33f, DWRITE_FONT_WEIGHT_NORMAL, White(0.75f), DWRITE_TEXT_ALIGNMENT_CENTER);
}

static void DrawMusic(Widget* wd) {
    MediaData m;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        m = g_media;
    }
    std::wstring label = m.hasSession ? (m.playing ? L"TOCANDO" : L"PAUSADO") : L"NADA TOCANDO";
    if (m.hasSession && !m.app.empty()) {
        std::wstring app = m.app;
        CharUpperBuffW(&app[0], (DWORD)app.size());
        label += L" · " + app;
    }
    std::wstring title = !m.hasSession ? L"Abra um player de música" : m.title.empty() ? L"Sem título" : m.title;
    double frac = 0;
    if (m.hasSession) {
        double pos = m.posSec;
        if (m.playing) pos += (GetTickCount64() - m.snapTick) / 1000.0;
        frac = m.endSec > 0 ? std::min(1.0, pos / m.endSec) : 0;
    }
    auto progress = [&](float x, float y, float w) {
        FillRound(x, y, w, 4, 2, Ink(0.18f));
        if (frac > 0) FillRound(x, y, (float)(w * frac), 4, 2, Ink(1));
    };
    float W = wd->w;
    if (wd->size == 0) {
        DrawArt(wd, m.art, D2D1::RectF(16, 16, 72, 72), 10);
        Text(label, 16, 84, W - 32, 16, 10, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.7f));
        Text(title, 16, 100, W - 32, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(m.artist, 16, 120, W - 32, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
        if (m.hasSession) progress(16, 144, W - 32);
    } else if (wd->size == 1) {
        DrawArt(wd, m.art, D2D1::RectF(16, 16, 148, 148), 16);
        float tx = 162, tw = W - tx - 16;
        Text(label, tx, 18, tw, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.7f));
        Text(title, tx, 36, tw, 22, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(m.artist, tx, 58, tw, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
        if (m.hasSession) progress(tx, 86, tw);
    } else if (wd->size == 2) {
        DrawArt(wd, m.art, D2D1::RectF(16, 16, W - 16, 196), 18);
        Text(label, 16, 206, W - 32, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.7f));
        Text(title, 16, 224, W - 32, 24, 17, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(m.artist, 16, 248, W - 32, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
        if (m.hasSession) progress(16, 274, W - 32);
    } else {
        DrawArt(wd, m.art, D2D1::RectF(16, 16, W - 16, W - 16), 20);
        Text(label, 16, W - 2, W - 32, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.7f));
        Text(title, 16, W + 16, W - 32, 26, 19, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(m.artist, 16, W + 42, W - 32, 20, 14, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
        if (m.hasSession) progress(16, W + 72, W - 32);
    }
    DrawMusicButtons(wd, m);
}

static std::wstring GbText(double bytes) {
    wchar_t buf[32];
    swprintf(buf, 32, L"%.1f GB", bytes / (1024.0 * 1024 * 1024));
    std::wstring s = buf;
    for (auto& c : s)
        if (c == L'.') c = L',';
    return s;
}

static void DrawSystem(Widget* wd) {
    bool hasGpu;
    double gpu;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        hasGpu = g_sensors[S_GPU_LOAD].have;
        gpu = g_sensors[S_GPU_LOAD].v;
    }
    struct Row {
        PCWSTR name;
        double v;
        bool have;
    } rows[] = {{L"CPU", g_sys.cpu, true}, {L"GPU", gpu, hasGpu}, {L"Memória", g_sys.mem, true}, {L"Disco C:", g_sys.disk, true}};
    auto pctOf = [](const Row& r) {
        wchar_t b[16];
        if (r.have) swprintf(b, 16, L"%.0f%%", r.v);
        else wcscpy(b, L"—");
        return std::wstring(b);
    };
    D2D1_COLOR_F accent = ColorOf(g_ui.accent);
    D2D1_COLOR_F warn = D2D1::ColorF(1.0f, 0.62f, 0.04f, 1.0f);
    float x = 16, w = wd->w - 32;
    if (wd->size == 0) {
        Text(L"Sistema", x, 11, w, 18, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.85f));
        for (int i = 0; i < 4; i++) {
            float y = 33 + i * 31.0f;
            Text(rows[i].name, x, y, w, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
            Text(pctOf(rows[i]), x, y, w, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_TRAILING);
            DrawBar(x, y + 18, w, rows[i].have ? rows[i].v : 0, rows[i].v >= 85 ? warn : accent);
        }
        return;
    }
    Text(L"Sistema", x, 14, w, 18, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.85f));
    Text(UptimeText(), x, 15, w, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.7f), DWRITE_TEXT_ALIGNMENT_TRAILING);
    for (int i = 0; i < 4; i++) {
        float y = 40 + i * 29.0f;
        Text(rows[i].name, x, y, w, 18, 12, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
        Text(pctOf(rows[i]), x, y, w, 18, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_TRAILING);
        DrawBar(x, y + 18, w, rows[i].have ? rows[i].v : 0, rows[i].v >= 85 ? warn : accent);
    }
    if (wd->size < 2) return;
    Line(16, 164, wd->w - 16, 164, 1, Ink(0.12f));
    wchar_t cores[16];
    swprintf(cores, 16, L"%u", g_sys.cores);
    std::pair<std::wstring, std::wstring> info[] = {
        {L"Memória em uso", Fmt(TF(L"%ls de %ls"), GbText(g_sys.memUsed).c_str(), GbText(g_sys.memTotal).c_str())},
        {L"Disco C: livre", Fmt(TF(L"%ls de %ls"), GbText(g_sys.diskFree).c_str(), GbText(g_sys.diskTotal).c_str())},
        {L"Processadores lógicos", cores},
        {L"Processos abertos", std::to_wstring(g_sys.processes)},
    };
    for (int i = 0; i < 4; i++) {
        float y = 178 + i * 38.0f;
        Text(info[i].first, x, y, w, 20, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.75f));
        Text(info[i].second, x, y, w, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1), DWRITE_TEXT_ALIGNMENT_TRAILING);
        if (i < 3) Line(x, y + 29, x + w, y + 29, 1, Ink(0.08f));
    }
}

// ---------- Sensors ----------

static bool SensorIsTemp(int id) { return id == S_CPU_TEMP || id == S_GPU_TEMP || id == S_GPU_HOTSPOT; }

static std::wstring SensorText(int id, const SensorValue& s, bool unit) {
    if (!s.have) return L"—";
    wchar_t b[48];
    double x = s.v;
    if (SensorIsTemp(id)) {
        if (g_ui.fahrenheit) x = x * 9 / 5 + 32;
        swprintf(b, 48, unit ? (g_ui.fahrenheit ? L"%.0f°F" : L"%.0f°C") : L"%.0f°", x);
        return b;
    }
    switch (id) {
        case S_CPU_LOAD:
        case S_GPU_LOAD:
        case S_DISK: swprintf(b, 48, L"%.0f%%", x); break;
        case S_RAM:
        case S_VRAM: return GbText(x);
        case S_CPU_CLOCK:
            if (x >= 1000) {
                swprintf(b, 48, L"%.2f", x / 1000);
                for (wchar_t* p = b; *p; p++)
                    if (*p == L'.') *p = L',';
                wcscat(b, unit ? L" GHz" : L"G");
            } else {
                swprintf(b, 48, unit ? L"%.0f MHz" : L"%.0f", x);
            }
            break;
        case S_GPU_FAN: swprintf(b, 48, unit ? L"%.0f rpm" : L"%.0f", x); break;
        case S_GPU_POWER:
        case S_CPU_POWER:
            if (x < 0) swprintf(b, 48, L"%.0f%%", -x);  // the GPU only gave how far it is from its limit
            else swprintf(b, 48, L"%.0f W", x);
            break;
        case S_NET_DOWN:
        case S_NET_UP: return SpeedText(x);
        default: return L"";
    }
    return b;
}

static D2D1_COLOR_F SensorColor(int id, const SensorValue& s) {
    if (SensorIsTemp(id)) {
        if (!s.have) return Ink(0.3f);
        return s.v < 60 ? D2D1::ColorF(0.19f, 0.82f, 0.35f, 1.0f) : s.v < 80 ? D2D1::ColorF(1.0f, 0.62f, 0.04f, 1.0f) : kRed;
    }
    if (id == S_NET_DOWN) return D2D1::ColorF(0.04f, 0.52f, 1.0f, 1.0f);
    if (id == S_NET_UP) return D2D1::ColorF(1.0f, 0.62f, 0.04f, 1.0f);
    if (id == S_GPU_FAN) return D2D1::ColorF(0.39f, 0.82f, 1.0f, 1.0f);
    if (id == S_GPU_POWER || id == S_CPU_POWER) return D2D1::ColorF(1.0f, 0.80f, 0.0f, 1.0f);
    if ((id == S_CPU_LOAD || id == S_GPU_LOAD || id == S_RAM || id == S_VRAM || id == S_DISK) && s.frac >= 0.85)
        return D2D1::ColorF(1.0f, 0.62f, 0.04f, 1.0f);
    return ColorOf(g_ui.accent);
}

// The last 90 seconds as a line with a soft fill below it
static void SensorGraph(const SensorValue& s, int id, float x, float y, float w, float h, D2D1_COLOR_F c) {
    if (s.histN < 2) return;
    float lo = 1e30f, hi = -1e30f;
    for (int i = 0; i < s.histN; i++) {
        float v = s.hist[(s.histPos - s.histN + i + 60) % 60];
        lo = std::min(lo, v);
        hi = std::max(hi, v);
    }
    if (SensorIsTemp(id)) {
        lo = std::min(lo, 30.0f);
        hi = std::max(hi, 90.0f);
    } else if (id == S_CPU_LOAD || id == S_GPU_LOAD || id == S_DISK) {
        lo = 0;
        hi = 100;
    } else {
        lo = 0;
        hi = std::max(hi * 1.15f, 1.0f);
    }
    ComPtr<ID2D1PathGeometry> line, area;
    ComPtr<ID2D1GeometrySink> ls, as;
    if (FAILED(g_d2d->CreatePathGeometry(&line)) || FAILED(line->Open(&ls)) || FAILED(g_d2d->CreatePathGeometry(&area)) ||
        FAILED(area->Open(&as)))
        return;
    auto pt = [&](int i) {
        float v = s.hist[(s.histPos - s.histN + i + 60) % 60];
        float px = x + w * i / 59.0f + w * (60 - s.histN) / 59.0f;
        float py = y + h - h * std::clamp((v - lo) / (hi - lo), 0.0f, 1.0f);
        return D2D1::Point2F(std::min(px, x + w), py);
    };
    ls->BeginFigure(pt(0), D2D1_FIGURE_BEGIN_HOLLOW);
    as->BeginFigure(D2D1::Point2F(pt(0).x, y + h), D2D1_FIGURE_BEGIN_FILLED);
    as->AddLine(pt(0));
    for (int i = 1; i < s.histN; i++) {
        ls->AddLine(pt(i));
        as->AddLine(pt(i));
    }
    as->AddLine(D2D1::Point2F(pt(s.histN - 1).x, y + h));
    ls->EndFigure(D2D1_FIGURE_END_OPEN);
    as->EndFigure(D2D1_FIGURE_END_CLOSED);
    ls->Close();
    as->Close();
    D2D1_GRADIENT_STOP st[2] = {{0.0f, D2D1::ColorF(c.r, c.g, c.b, 0.35f)}, {1.0f, D2D1::ColorF(c.r, c.g, c.b, 0.0f)}};
    if (auto b = MakeGradient(st, 2, D2D1::Point2F(0, y), D2D1::Point2F(0, y + h))) g_t->FillGeometry(area.Get(), b.Get());
    ComPtr<ID2D1StrokeStyle> round;
    g_d2d->CreateStrokeStyle(D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                                         D2D1_LINE_JOIN_ROUND),
                             nullptr, 0, &round);
    if (auto b = Solid(c)) g_t->DrawGeometry(line.Get(), b.Get(), 1.8f, round.Get());
}

static void DrawSensors(Widget* wd) {
    SensorValue s[S_COUNT];
    std::wstring gpuName;
    bool hwOn;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        for (int i = 0; i < S_COUNT; i++) s[i] = g_sensors[i];
        gpuName = g_sensorGpu;
        hwOn = g_sensorLhm;
    }
    float W = wd->w, H = wd->h;
    int want = wd->size <= 1 ? 4 : wd->size == 2 ? 6 : 8;
    std::vector<int> ids;
    for (int i = 0; i < 8 && (int)ids.size() < want; i++)
        if (g_ui.sensorSlots[i] > S_NONE && g_ui.sensorSlots[i] < S_COUNT) ids.push_back(g_ui.sensorSlots[i]);
    if (ids.empty()) {
        Text(L"Sensores", 16, 14, W - 32, 20, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.85f));
        TextWrap(L"Escolha o que mostrar em Ajustes > Sensores", 16, 44, W - 32, H - 56, 12, DWRITE_FONT_WEIGHT_NORMAL,
                 Ink(0.6f));
        return;
    }
    if (wd->size <= 1) {
        // Rings, as the Mac's battery widget: 2 × 2 in the small one, a row of 4 in the medium
        int cols = wd->size == 0 ? 2 : 4, rows = wd->size == 0 ? 2 : 1;
        float cw = (W - 16) / cols, ch = (H - 12) / rows;
        float r = wd->size == 0 ? 20 : 29, width = wd->size == 0 ? 5 : 6.5f;
        for (int i = 0; i < (int)ids.size() && i < cols * rows; i++) {
            int id = ids[i];
            float cx = 8 + cw * (i % cols) + cw / 2, top = 6 + ch * (i / cols);
            float cy = top + (wd->size == 0 ? 30 : 58);
            D2D1_COLOR_F c = SensorColor(id, s[id]);
            DrawRing(cx, cy, r, width, s[id].have ? s[id].frac * 100 : 0, c);
            float fs = wd->size == 0 ? 11.5f : 14.5f;
            Text(SensorText(id, s[id], false), cx - r, cy - fs * 0.68f, r * 2, fs + 6, fs, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1),
                 DWRITE_TEXT_ALIGNMENT_CENTER);
            Text(kSensorShort[id], cx - cw / 2, cy + r + (wd->size == 0 ? 3 : 9), cw, 16, wd->size == 0 ? 10.5f : 12,
                 DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.6f), DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        return;
    }
    // Tiles with the last 90 seconds
    Text(L"Sensores", 16, 14, 120, 20, 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.9f));
    Text(gpuName, 120, 16, W - 136, 18, 11, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.5f), DWRITE_TEXT_ALIGNMENT_TRAILING);
    int rows = (int)(ids.size() + 1) / 2;
    float gap = 10, top = 44, tw = (W - 32 - gap) / 2, th = (H - top - 14 - gap * (rows - 1)) / rows;
    for (int i = 0; i < (int)ids.size(); i++) {
        int id = ids[i];
        float x = 16 + (tw + gap) * (i % 2), y = top + (th + gap) * (i / 2);
        D2D1_COLOR_F c = SensorColor(id, s[id]);
        FillRound(x, y, tw, th, 14, Ink(0.07f));
        Text(kSensorLong[id], x + 12, y + 9, tw - 24, 16, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.6f));
        Text(SensorText(id, s[id], true), x + 12, y + 25, tw - 24, 30, 22, DWRITE_FONT_WEIGHT_SEMI_BOLD, s[id].have ? Ink(1) : Ink(0.4f));
        if (!s[id].source.empty())
            Text(s[id].source, x + 12, y + 33, tw - 24, 14, 10, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.4f), DWRITE_TEXT_ALIGNMENT_TRAILING);
        float gy = y + 60, gh = th - 68;
        if (gh > 8) SensorGraph(s[id], id, x + 10, gy, tw - 20, gh, c);
        if (!s[id].have && (id == S_CPU_TEMP || id == S_CPU_POWER || id == S_GPU_HOTSPOT) && !hwOn)
            TextWrap(L"Abra o LibreHardwareMonitor", x + 12, y + 58, tw - 24, th - 62, 10.5f, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.5f));
    }
}

// ---------------------------------------------------------------------------
// Background picture
// ---------------------------------------------------------------------------

// Without the live glass the widgets stay visible in screenshots, so the screen can't be read under them:
// the desktop window (wallpaper and icons) is drawn into a picture instead, kept for a few seconds
static HDC g_deskDC = nullptr;
static HBITMAP g_deskBmp = nullptr;
static HGDIOBJ g_deskOld = nullptr;
static RECT g_deskRc = {};
static ULONGLONG g_deskTick = 0;

static void FreeDesktopPicture() {
    if (g_deskDC) {
        SelectObject(g_deskDC, g_deskOld);
        DeleteObject(g_deskBmp);
        DeleteDC(g_deskDC);
    }
    g_deskDC = nullptr;
    g_deskBmp = nullptr;
    g_deskTick = 0;
}

static HDC DesktopPicture() {
    if (g_deskDC && GetTickCount64() - g_deskTick < 3000) return g_deskDC;
    FreeDesktopPicture();
    HWND shell = GetShellWindow();
    if (!shell) return nullptr;
    GetWindowRect(shell, &g_deskRc);
    int w = g_deskRc.right - g_deskRc.left, h = g_deskRc.bottom - g_deskRc.top;
    if (w <= 0 || h <= 0) return nullptr;
    HDC screen = GetDC(nullptr);
    g_deskDC = CreateCompatibleDC(screen);
    g_deskBmp = CreateCompatibleBitmap(screen, w, h);
    ReleaseDC(nullptr, screen);
    if (!g_deskDC || !g_deskBmp) {
        if (g_deskBmp) DeleteObject(g_deskBmp);
        if (g_deskDC) DeleteDC(g_deskDC);
        g_deskDC = nullptr;
        g_deskBmp = nullptr;
        return nullptr;
    }
    g_deskOld = SelectObject(g_deskDC, g_deskBmp);
    PrintWindow(shell, g_deskDC, PW_RENDERFULLCONTENT);
    g_deskTick = GetTickCount64();
    return g_deskDC;
}

// The part of the screen a widget's glass is made from: its glass and a border around it for the blur and refraction
static RECT CaptureRect(Widget* wd) {
    RECT wr;
    GetWindowRect(wd->hwnd, &wr);
    int m = (int)std::lround(kMargin * wd->scale), c = (int)std::lround(kCapture * wd->scale);
    int gw = (int)std::lround(wd->w * wd->scale), gh = (int)std::lround(wd->h * wd->scale);
    return {wr.left + m - c, wr.top + m - c, wr.left + m + gw + c, wr.top + m + gh + c};
}

// Where the picture is off the screen there's nothing (black), which the liquid themes bend into a dark band at the
// edge: the nearest pixels of the screen are repeated there instead. 'valid' is in the picture's own pixels
static void ExtendEdges(std::vector<UINT32>& px, int w, int h, RECT valid) {
    valid.left = std::clamp((int)valid.left, 0, w);
    valid.right = std::clamp((int)valid.right, 0, w);
    valid.top = std::clamp((int)valid.top, 0, h);
    valid.bottom = std::clamp((int)valid.bottom, 0, h);
    if (valid.left >= valid.right || valid.top >= valid.bottom) return;
    if (valid.left == 0 && valid.top == 0 && valid.right == w && valid.bottom == h) return;
    for (int y = valid.top; y < valid.bottom; y++) {
        UINT32* row = &px[(size_t)y * w];
        for (int x = 0; x < valid.left; x++) row[x] = row[valid.left];
        for (int x = valid.right; x < w; x++) row[x] = row[valid.right - 1];
    }
    for (int y = 0; y < valid.top; y++) memcpy(&px[(size_t)y * w], &px[(size_t)valid.top * w], w * 4);
    for (int y = valid.bottom; y < h; y++) memcpy(&px[(size_t)y * w], &px[(size_t)(valid.bottom - 1) * w], w * 4);
}

// What the glass may show of the screen: the work area of the widget's monitor (without the taskbar and the bars that
// take space at the edges, like the Finder Bar)
static RECT GlassClip(const RECT& rc) {
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST), &mi)) return mi.rcWork;
    return rc;
}

// A new picture behind the widget: false when it's the same as before (nothing to rebuild)
static bool ApplyCapture(Widget* wd, const RECT& rc, std::vector<UINT32>&& px) {
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0 || px.size() != (size_t)w * h) return false;
    RECT wr;
    GetWindowRect(wd->hwnd, &wr);
    wd->capTick = GetTickCount64();
    UINT64 hash = 1469598103934665603ULL ^ ((UINT64)w << 32) ^ (UINT64)h ^ ((UINT64)(UINT32)rc.left << 20) ^ (UINT64)(UINT32)rc.top;
    for (size_t i = 0; i < px.size(); i += 3) hash = (hash ^ (px[i] | 0xFF000000)) * 1099511628211ULL;
    if (hash == wd->capHash && wd->snapW == w && wd->snapH == h) return false;  // nothing changed behind it
    wd->capHash = hash;
    double sum = 0;
    int n = 0;
    for (size_t i = 0; i < px.size(); i++) {
        UINT32 p = px[i] | 0xFF000000;  // opaque
        px[i] = p;
        if ((i & 15) == 0) {
            sum += 0.2126 * ((p >> 16) & 0xFF) + 0.7152 * ((p >> 8) & 0xFF) + 0.0722 * (p & 0xFF);
            n++;
        }
    }
    wd->pixels = std::move(px);
    wd->lum = n ? (float)(sum / n / 255.0) : -1;
    wd->snapW = w;
    wd->snapH = h;
    wd->capOffX = (float)(rc.left - wr.left);
    wd->capOffY = (float)(rc.top - wr.top);
    wd->fresh = true;
    return true;
}

static void Capture(Widget* wd) {
    RECT rc = CaptureRect(wd);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    HBITMAP bmp = dc ? CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
    if (!bmp) {
        if (dc) DeleteDC(dc);
        ReleaseDC(nullptr, screen);
        return;
    }
    HGDIOBJ old = SelectObject(dc, bmp);
    RECT area = {};  // what the source really has (the screens, or the desktop's picture)
    if (g_ui.liveGlass) {
        // The widget itself is excluded from screen capture, so this reads what is behind it
        BitBlt(dc, 0, 0, w, h, screen, rc.left, rc.top, SRCCOPY);
        area = {GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN),
                GetSystemMetrics(SM_XVIRTUALSCREEN) + GetSystemMetrics(SM_CXVIRTUALSCREEN),
                GetSystemMetrics(SM_YVIRTUALSCREEN) + GetSystemMetrics(SM_CYVIRTUALSCREEN)};
    } else if (HDC desk = DesktopPicture()) {
        BitBlt(dc, 0, 0, w, h, desk, rc.left - g_deskRc.left, rc.top - g_deskRc.top, SRCCOPY);
        area = g_deskRc;
    }
    GdiFlush();
    ReleaseDC(nullptr, screen);
    std::vector<UINT32> px((const UINT32*)bits, (const UINT32*)bits + (size_t)w * h);
    SelectObject(dc, old);
    DeleteObject(bmp);
    DeleteDC(dc);
    // Off the screen: the screen's edge repeated, so no dark band
    RECT valid = {area.left - rc.left, area.top - rc.top, area.right - rc.left, area.bottom - rc.top};
    // ...and only the desktop's own area: not another monitor, not the taskbar or bars at the screen's edges (as
    // the Finder Bar), which would show at the glass' border
    RECT clip = GlassClip(rc);
    valid.left = std::max(valid.left, clip.left - rc.left);
    valid.top = std::max(valid.top, clip.top - rc.top);
    valid.right = std::min(valid.right, clip.right - rc.left);
    valid.bottom = std::min(valid.bottom, clip.bottom - rc.top);
    ExtendEdges(px, w, h, valid);
    ApplyCapture(wd, rc, std::move(px));
}

// ---------------------------------------------------------------------------
// Settings window (as in Siri Plus Launchpad: sections on the left with search, cards on the right)
// ---------------------------------------------------------------------------

static void Render(Widget* wd);
static void RenderAll();
static void ReleaseSurface(Widget* wd);
static void RebuildWidgets();
static void UpdateAnimTimer();

static const float kCfgW = 856, kCfgH = 600, kSideW = 236;

struct CfgRow {
    int item;
    D2D1_RECT_F r, ctl;
    float labelH, descH;
};
struct CfgCaption {
    D2D1_RECT_F r;
    std::wstring text;
};
struct CfgSideRow {
    int section;
    D2D1_RECT_F r;
};
struct CfgUi {
    Widget* wd = nullptr;
    int section = 1;
    std::wstring search;
    bool searchFocus = false;
    float scroll = 0, contentH = 0;
    int hover = -1, sideHover = -1, popupHover = -1;
    bool doneHover = false;
    int drag = -1;  // item being dragged (slider)
    int edit = -1;  // item being typed in (color, text)
    std::wstring editText;
    int popup = -1;  // item whose option list is open
    D2D1_RECT_F popupRect{};
    std::vector<CfgRow> rows;
    std::vector<D2D1_RECT_F> cards;
    std::vector<CfgCaption> captions;
    std::vector<CfgSideRow> side;
    D2D1_RECT_F header{}, view{}, done{}, bar{};
    bool nothing = false;
    ULONGLONG lastApply = 0;
};
static CfgUi g_cfg;
static std::wstring g_cityQuery;
// Rows that aren't settings: the cities found (item = kPlaceRow - index) and the search status (kStatusRow)
static const int kPlaceRow = -1000, kStatusRow = -2000;

static std::wstring g_glyphFamily;
static PCWSTR GlyphFamily() {
    if (g_glyphFamily.empty()) {
        g_glyphFamily = L"Segoe MDL2 Assets";
        ComPtr<IDWriteFontCollection> fonts;
        UINT32 index = 0;
        BOOL exists = FALSE;
        if (SUCCEEDED(g_dw->GetSystemFontCollection(&fonts)) && fonts &&
            SUCCEEDED(fonts->FindFamilyName(L"Segoe Fluent Icons", &index, &exists)) && exists)
            g_glyphFamily = L"Segoe Fluent Icons";
    }
    return g_glyphFamily.c_str();
}

static ComPtr<IDWriteTextFormat> Format(PCWSTR family, float size, DWRITE_FONT_WEIGHT weight, DWRITE_TEXT_ALIGNMENT align,
                                        bool wrap) {
    ComPtr<IDWriteTextFormat> f;
    if (FAILED(g_dw->CreateTextFormat(family, nullptr, weight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size,
                                      L"", &f)))
        return nullptr;
    f->SetTextAlignment(align);
    if (wrap) {
        f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        f->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
    } else {
        f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        ComPtr<IDWriteInlineObject> ellipsis;
        if (SUCCEEDED(g_dw->CreateEllipsisTrimmingSign(f.Get(), &ellipsis))) {
            DWRITE_TRIMMING trim = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
            f->SetTrimming(&trim, ellipsis.Get());
        }
    }
    return f;
}

static const PCWSTR kTextFont = L"Segoe UI Variable Text", kDisplayFont = L"Segoe UI Variable Display";

static void CText(const std::wstring& src, const D2D1_RECT_F& r, float size, DWRITE_FONT_WEIGHT weight, D2D1_COLOR_F color,
                  DWRITE_TEXT_ALIGNMENT align = DWRITE_TEXT_ALIGNMENT_LEADING, bool wrap = false, PCWSTR family = kTextFont) {
    const std::wstring& s = T(src);
    if (s.empty()) return;
    auto f = Format(family, size, weight, align, wrap);
    auto b = Solid(color);
    if (f && b) g_t->DrawText(s.c_str(), (UINT32)s.size(), f.Get(), r, b.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

static float TextHeight(const std::wstring& src, float size, float width) {
    const std::wstring& s = T(src);
    auto f = Format(kTextFont, size, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, true);
    ComPtr<IDWriteTextLayout> layout;
    if (!f || s.empty() || FAILED(g_dw->CreateTextLayout(s.c_str(), (UINT32)s.size(), f.Get(), std::max(10.0f, width), 2000, &layout)))
        return 0;
    DWRITE_TEXT_METRICS m{};
    layout->GetMetrics(&m);
    return std::ceil(m.height);
}

static float TextWidth(const std::wstring& src, float size) {
    const std::wstring& s = T(src);
    auto f = Format(kTextFont, size, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, false);
    ComPtr<IDWriteTextLayout> layout;
    if (!f || s.empty() || FAILED(g_dw->CreateTextLayout(s.c_str(), (UINT32)s.size(), f.Get(), 4000, 200, &layout))) return 0;
    DWRITE_TEXT_METRICS m{};
    layout->GetMetrics(&m);
    return m.widthIncludingTrailingWhitespace;
}

static D2D1_COLOR_F Fg(float a) { return Ink(a); }
static D2D1_COLOR_F CfgRgb(UINT32 c) {
    return D2D1::ColorF(((c >> 16) & 0xFF) / 255.f, ((c >> 8) & 0xFF) / 255.f, (c & 0xFF) / 255.f, 1.0f);
}
static bool g_cfgDrawLocked = false;
static D2D1_COLOR_F CfgAccent() { return g_cfgDrawLocked ? Fg(0.30f) : CfgRgb(g_darkText ? 0x007AFF : 0x0A84FF); }
static bool InRect(const D2D1_RECT_F& r, D2D1_POINT_2F p) {
    return p.x >= r.left && p.x < r.right && p.y >= r.top && p.y < r.bottom;
}

// Clear polished glass piece (buttons, fields, the switch), from Siri Plus Launchpad's glass buttons
static void DrawClearGlassButton(const D2D1_RECT_F& r, float radius, float a, float level, bool shadow) {
    const float s = 1.0f, h = r.bottom - r.top;
    const bool onLight = g_darkText;
    const float L = std::clamp(level, 0.5f, 2.2f);
    radius = std::min(radius, std::min(r.right - r.left, h) / 2);
    if (shadow) DrawThemeShadow(r, radius, 0.13f * a, 6 * s, 2.5f * s);
    const float fill = onLight ? 0.16f : 0.05f;
    D2D1_GRADIENT_STOP body[3] = {{0.0f, White(std::min(0.9f, fill * 1.6f * L))},
                                  {0.5f, White(std::min(0.9f, fill * 0.6f * L))},
                                  {1.0f, White(std::min(0.9f, fill * 1.0f * L))}};
    if (auto b = MakeGradient(body, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))) {
        b->SetOpacity(a);
        g_t->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), b.Get());
    }
    float inset = 1.2f * s, tr = std::max(0.0f, radius - inset);
    D2D1_RECT_F top = D2D1::RectF(r.left + inset, r.top + inset, r.right - inset, r.top + h * 0.5f);
    D2D1_GRADIENT_STOP glow[2] = {{0.0f, White(std::min(0.5f, 0.10f * L))}, {1.0f, White(0.0f)}};
    if (auto g = MakeGradient(glow, 2, D2D1::Point2F(0, top.top), D2D1::Point2F(0, top.bottom))) {
        g->SetOpacity(a);
        g_t->FillRoundedRectangle(D2D1::RoundedRect(top, tr, tr), g.Get());
    }
    float bw = 1.0f, half = bw / 2, rr = std::max(0.0f, radius - half);
    D2D1_ROUNDED_RECT edge =
        D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half), rr, rr);
    D2D1_GRADIENT_STOP rim[4] = {{0.0f, White(std::min(1.0f, 0.62f * std::sqrt(L)))}, {0.32f, White(0.14f)},
                                 {0.70f, White(0.10f)}, {1.0f, White(std::min(1.0f, 0.38f * std::sqrt(L)))}};
    if (auto e = MakeGradient(rim, 4, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))) {
        e->SetOpacity(a);
        g_t->DrawRoundedRectangle(edge, e.Get(), bw);
    }
    if (onLight)
        if (auto b = Solid(D2D1::ColorF(0.40f, 0.42f, 0.48f, 0.22f * a))) g_t->DrawRoundedRectangle(edge, b.Get(), bw * 0.8f);
    float in2 = std::max(2.0f * s, std::min(3.5f * s, h * 0.07f)), ir = std::max(0.0f, radius - in2);
    D2D1_GRADIENT_STOP depth[3] = {{0.0f, White(0.0f)}, {0.6f, White(0.02f)}, {1.0f, White(std::min(0.6f, 0.24f * std::sqrt(L)))}};
    if (auto d = MakeGradient(depth, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))) {
        d->SetOpacity(a);
        g_t->DrawRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(r.left + in2, r.top + in2, r.right - in2, r.bottom - in2), ir, ir), d.Get(), 0.8f);
    }
}
static void GlassField(const D2D1_RECT_F& r, float radius, float level) { DrawClearGlassButton(r, radius, 1.0f, level, false); }

// ---------- What is shown ----------

static Theme CfgTheme() { return g_ui.theme; }

static bool CfgItemHidden(const CfgItem& it) {
    std::wstring k = it.key;
    if (k == L"tintLevel" || k == L"tintReach" || k == L"tintColor" || k == L"#g.tinted") return CfgTheme() != Theme::Tinted;
    if (k == L"refraction") return CfgTheme() == Theme::Frost;
    if (k == L"backlightIntensity" || k == L"backlightColors") return !g_ui.backlight;
    if (k.rfind(L"backlightColor", 0) == 0 && k.size() == 15) return !g_ui.backlight || (k[14] - L'0') > g_ui.backlightCount;
    return false;
}
// Dimmed: the chosen theme doesn't use it
static bool CfgItemUnused(const CfgItem& it) {
    if (!wcscmp(it.key, L"glassEffect")) {
        Theme t = CfgTheme();
        return t == Theme::Glass || t == Theme::Frost || t == Theme::Siri || t == Theme::Tinted;
    }
    return false;
}

static std::wstring Fold(std::wstring s) {
    static const wchar_t from[] = L"áàâãäéèêëíìîïóòôõöúùûüçñÁÀÂÃÄÉÈÊËÍÌÎÏÓÒÔÕÖÚÙÛÜÇÑ",
                         to[] = L"aaaaaeeeeiiiiooooouuuucnaaaaaeeeeiiiiooooouuuucn";
    for (auto& c : s) {
        for (int i = 0; from[i]; i++)
            if (c == from[i]) {
                c = to[i];
                break;
            }
        c = towlower(c);
    }
    return s;
}

static bool CfgMatches(const CfgItem& it) {
    if (CfgItemHidden(it)) return false;
    if (g_cfg.search.empty()) return it.section == g_cfg.section;
    if (it.type == kCfgGroup) return true;  // kept while it has a match under it (see CfgBuild)
    std::wstring q = Fold(g_cfg.search);
    return Fold(T(it.label)).find(q) != std::wstring::npos || (it.desc && Fold(T(it.desc)).find(q) != std::wstring::npos) ||
           Fold(T(kCfgSections[it.section].name)).find(q) != std::wstring::npos;
}
static bool CfgSectionHasMatch(int section) {
    for (const CfgItem& it : kCfgItems)
        if (it.section == section && it.type != kCfgGroup && CfgMatches(it)) return true;
    return false;
}

static float CfgControlWidth(const CfgItem& it) {
    switch (it.type) {
        case kCfgToggle: return 40;
        case kCfgSlider: return 230;
        case kCfgChoice: return 200;
        case kCfgColor: return 150;
        case kCfgText: return 220;
        case kCfgButton: return 190;
        default: return 0;
    }
}

static std::wstring CfgValue(const CfgItem& it) { return CfgNow(it.key); }

static void CfgBuild() {
    CfgUi& c = g_cfg;
    c.rows.clear();
    c.cards.clear();
    c.captions.clear();
    c.side.clear();
    const float W = kCfgW, H = kCfgH, pad = 22, titleH = 34;
    c.done = D2D1::RectF(W - pad - 104, pad, W - pad, pad + 30);
    c.view = D2D1::RectF(kSideW, pad + titleH + 10, W, H - 10);
    c.bar = D2D1::RectF(12, 16, kSideW - 12, 46);
    float cardL = kSideW + pad, cardR = W - pad, in = 14;
    float y = c.view.top + 4 - std::round(c.scroll), startY = y;
    c.header = D2D1::RectF(0, 0, 0, 0);
    if (c.search.empty()) {
        float textW = cardR - cardL - in - 56 - in;
        float dh = TextHeight(kCfgSections[c.section].desc, 12, textW);
        float h = std::max(68.0f, 16 + 20 + dh + 16);
        c.header = D2D1::RectF(cardL, y, cardR, y + h);
        y += h + 14;
    }
    auto addCard = [&](int section) {
        float top = y;
        int title = -1;
        for (int i = 0; i < kCfgItemCount; i++) {
            const CfgItem& it = kCfgItems[i];
            if (it.section != section || !CfgMatches(it)) continue;
            if (it.type == kCfgGroup) {
                title = i;
                continue;
            }
            if (title >= 0) {  // a title: the card so far ends, the next ones start under it
                if (y > top) {
                    c.cards.push_back(D2D1::RectF(cardL, top, cardR, y));
                    y += 16;
                }
                c.captions.push_back({D2D1::RectF(cardL + 4, y, cardR, y + 24), kCfgItems[title].label});
                y += 26;
                top = y;
                title = -1;
            }
            float cw = CfgControlWidth(it);
            float labelW = cardR - cardL - 2 * in - cw - 16;
            bool hasDesc = it.desc && *it.desc;
            float labelH = 19, descH = hasDesc ? TextHeight(it.desc, 11.5f, labelW) : 0;
            float h = std::max(46.0f, 12 + labelH + (hasDesc ? 2 + descH : 0) + 12);
            D2D1_RECT_F r = D2D1::RectF(cardL, y, cardR, y + h);
            float ch = it.type == kCfgToggle ? 22.0f : it.type == kCfgSlider ? 24.0f : it.type == kCfgButton ? 28.0f : 26.0f;
            float cy = (r.top + r.bottom) / 2;
            D2D1_RECT_F ctl = D2D1::RectF(cardR - in - cw, std::round(cy - ch / 2), cardR - in, std::round(cy - ch / 2) + ch);
            c.rows.push_back({i, r, ctl, labelH, descH});
            y += h;
            if (!wcscmp(it.key, L"#citySearch")) {  // the cities found, each with "Usar"
                if (!g_placeStatus.empty()) {
                    c.rows.push_back({kStatusRow, D2D1::RectF(cardL, y, cardR, y + 40), D2D1::RectF(0, 0, 0, 0), 19, 0});
                    y += 40;
                }
                for (size_t k = 0; k < g_places.size(); k++) {
                    D2D1_RECT_F pr = D2D1::RectF(cardL, y, cardR, y + 50);
                    float pcy = (pr.top + pr.bottom) / 2;
                    D2D1_RECT_F pctl = D2D1::RectF(cardR - in - 90, std::round(pcy - 14), cardR - in, std::round(pcy - 14) + 28);
                    c.rows.push_back({kPlaceRow - (int)k, pr, pctl, 19, 15});
                    y += 50;
                }
            }
        }
        if (y > top) c.cards.push_back(D2D1::RectF(cardL, top, cardR, y));
    };
    c.nothing = false;
    if (c.search.empty()) addCard(c.section);
    else {
        for (int sec = 0; sec < kCfgSectionCount; sec++) {
            if (!CfgSectionHasMatch(sec)) continue;
            c.captions.push_back({D2D1::RectF(cardL + 4, y, cardR, y + 26), kCfgSections[sec].name});
            y += 26;
            addCard(sec);
            y += 14;
        }
        c.nothing = c.rows.empty();
    }
    c.contentH = y - startY + 16;
    float sy = c.bar.bottom + 14;
    for (int sec = 0; sec < kCfgSectionCount; sec++) {
        if (!c.search.empty() && !CfgSectionHasMatch(sec)) continue;
        c.side.push_back({sec, D2D1::RectF(8, sy, kSideW - 8, sy + 28)});
        sy += 30;
    }
}

static float CfgMaxScroll() { return std::max(0.0f, g_cfg.contentH - (g_cfg.view.bottom - g_cfg.view.top)); }

// ---------- Drawing ----------

static void CfgIcon(int section, const D2D1_RECT_F& r, float glyphSize) {
    const CfgSection& sec = kCfgSections[section];
    float rad = (r.right - r.left) * 0.24f;
    D2D1_COLOR_F c = CfgRgb(sec.color);
    D2D1_GRADIENT_STOP stops[2] = {
        {0.0f, D2D1::ColorF(std::min(1.0f, c.r + 0.10f), std::min(1.0f, c.g + 0.10f), std::min(1.0f, c.b + 0.10f))}, {1.0f, c}};
    if (auto fill = MakeGradient(stops, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom)))
        g_t->FillRoundedRectangle(D2D1::RoundedRect(r, rad, rad), fill.Get());
    CText(sec.glyph, r, glyphSize, DWRITE_FONT_WEIGHT_NORMAL, White(1), DWRITE_TEXT_ALIGNMENT_CENTER, false, GlyphFamily());
}

static void DrawCfgToggle(const D2D1_RECT_F& c, bool on) {
    float h = c.bottom - c.top, r = h / 2;
    if (on) Fill(c, Alpha(CfgAccent(), 0.85f), r);
    DrawClearGlassButton(c, r, 1.0f, on ? 0.7f : 0.9f, false);
    float d = h - 4, cx = on ? c.right - 2 - d / 2 : c.left + 2 + d / 2, cy = (c.top + c.bottom) / 2;
    D2D1_RECT_F k = D2D1::RectF(cx - d / 2, cy - d / 2, cx + d / 2, cy + d / 2);
    if (auto b = Solid(White(g_cfgDrawLocked ? 0.45f : 0.80f))) g_t->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), d / 2, d / 2), b.Get());
    DrawClearGlassButton(k, d / 2, 1.0f, 2.0f, true);
}

static D2D1_RECT_F CfgSliderTrack(const D2D1_RECT_F& c) {
    float cy = (c.top + c.bottom) / 2;
    return D2D1::RectF(c.left + 62, cy - 2, c.right - 9, cy + 2);
}

static std::wstring CfgOptionLabel(const CfgItem& it, const std::wstring& v) {
    for (int i = 0; i < it.optCount; i++)
        if (v == it.opts[i].value) return it.opts[i].label;
    return v;
}

static void EditBox(const D2D1_RECT_F& box, const std::wstring& text, bool editing, bool hover, PCWSTR placeholder) {
    GlassField(box, 7, editing || hover ? 1.2f : 0.85f);
    if (editing)
        if (auto b = Solid(CfgAccent()))
            g_t->DrawRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(box.left + 0.75f, box.top + 0.75f, box.right - 0.75f, box.bottom - 0.75f), 7, 7),
                b.Get(), 1.5f);
    D2D1_RECT_F tr = D2D1::RectF(box.left + 10, box.top, box.right - 8, box.bottom);
    if (text.empty() && !editing && placeholder) {
        CText(placeholder, tr, 13.5f, DWRITE_FONT_WEIGHT_NORMAL, Fg(0.40f));
        return;
    }
    float w = TextWidth(text, 13.5f), room = tr.right - tr.left;
    g_t->PushAxisAlignedClip(tr, D2D1_ANTIALIAS_MODE_ALIASED);
    float x = editing && w > room ? tr.right - w - 2 : tr.left;
    CText(text, D2D1::RectF(x, tr.top, x + w + 4, tr.bottom), 13.5f, DWRITE_FONT_WEIGHT_NORMAL, Fg(g_cfgDrawLocked ? 0.40f : 0.92f));
    if (editing) Line(x + w + 1, box.top + 6, x + w + 1, box.bottom - 6, 1.2f, Fg(0.9f));
    g_t->PopAxisAlignedClip();
}

static bool CfgButtonEnabled(const CfgItem& it) { return wcscmp(it.key, L"#themeReset") || CfgThemeChanged(); }

static void DrawCfgControl(const CfgRow& row, bool hover) {
    const CfgItem& it = kCfgItems[row.item];
    const D2D1_RECT_F& c = row.ctl;
    std::wstring v = CfgValue(it);
    switch (it.type) {
        case kCfgToggle: DrawCfgToggle(c, _wtoi(v.c_str()) != 0); break;
        case kCfgSlider: {
            int value = std::clamp(_wtoi(v.c_str()), it.min, it.max);
            D2D1_RECT_F tr = CfgSliderTrack(c);
            float k = it.max > it.min ? (float)(value - it.min) / (it.max - it.min) : 0, x = tr.left + (tr.right - tr.left) * k;
            CText(std::to_wstring(value) + (it.unit ? it.unit : L""), D2D1::RectF(c.left, c.top, tr.left - 12, c.bottom), 13,
                  DWRITE_FONT_WEIGHT_NORMAL, Fg(g_cfgDrawLocked ? 0.35f : 0.60f), DWRITE_TEXT_ALIGNMENT_TRAILING);
            Fill(tr, Fg(0.18f), 2);
            Fill(D2D1::RectF(tr.left, tr.top, x, tr.bottom), CfgAccent(), 2);
            float kr = row.item == g_cfg.drag ? 10.0f : 9.0f, cy = (tr.top + tr.bottom) / 2;
            FillEllipse(x, cy + 1, kr, kr, D2D1::ColorF(0, 0, 0, 0.22f));
            FillEllipse(x, cy, kr, kr, White(g_cfgDrawLocked ? 0.55f : 1.0f));
            break;
        }
        case kCfgChoice: {
            GlassField(c, 7, hover || g_cfg.popup == row.item ? 1.3f : 0.9f);
            CText(CfgOptionLabel(it, v), D2D1::RectF(c.left + 10, c.top, c.right - 26, c.bottom), 13.5f, DWRITE_FONT_WEIGHT_NORMAL,
                  Fg(g_cfgDrawLocked ? 0.40f : 0.92f));
            CText(L"", D2D1::RectF(c.right - 24, c.top, c.right - 6, c.bottom), 9, DWRITE_FONT_WEIGHT_NORMAL, Fg(0.70f),
                  DWRITE_TEXT_ALIGNMENT_CENTER, false, GlyphFamily());
            break;
        }
        case kCfgColor: {
            float d = c.bottom - c.top, cy = (c.top + c.bottom) / 2;
            COLORREF col = ParseHex(v, RGB(128, 128, 128));
            FillEllipse(c.left + d / 2, cy, d / 2 - 1, d / 2 - 1, ColorOf(col));
            if (auto b = Solid(Fg(0.35f))) g_t->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(c.left + d / 2, cy), d / 2 - 1, d / 2 - 1), b.Get(), 1);
            bool editing = g_cfg.edit == row.item;
            EditBox(D2D1::RectF(c.left + d + 8, c.top, c.right, c.bottom), editing ? g_cfg.editText : v, editing, hover, nullptr);
            break;
        }
        case kCfgText: {
            bool editing = g_cfg.edit == row.item;
            bool search = !wcscmp(it.key, L"#citySearch");
            std::wstring shown = search ? g_cityQuery : v;
            if (!wcscmp(it.key, L"haToken") && !shown.empty())  // a secret: only its end shows
                shown = L"\u2022\u2022\u2022\u2022\u2022\u2022" + shown.substr(shown.size() > 4 ? shown.size() - 4 : 0);
            EditBox(c, editing ? g_cfg.editText : shown, editing, hover, search ? L"Ex.: Campinas" : L"");
            break;
        }
        case kCfgButton: {
            bool enabled = CfgButtonEnabled(it), danger = !wcscmp(it.key, L"#resetAll");
            GlassField(c, 7, enabled && hover ? 1.3f : 0.9f);
            D2D1_COLOR_F ink = !enabled ? Fg(0.35f) : danger ? D2D1::ColorF(1.0f, 0.27f, 0.23f, 1.0f) : Fg(0.92f);
            CText(it.label, c, 13, DWRITE_FONT_WEIGHT_MEDIUM, ink, DWRITE_TEXT_ALIGNMENT_CENTER);
            break;
        }
        default: break;
    }
}

static void DrawCfgPopup() {
    CfgUi& c = g_cfg;
    if (c.popup < 0) return;
    const CfgItem& it = kCfgItems[c.popup];
    const CfgRow* row = nullptr;
    for (const CfgRow& r : c.rows)
        if (r.item == c.popup) row = &r;
    if (!row) {
        c.popup = -1;
        return;
    }
    float itemH = 30, w = std::max(row->ctl.right - row->ctl.left, 210.0f), h = it.optCount * itemH + 10;
    float top = row->ctl.bottom + 6;
    if (top + h > kCfgH - 10) top = row->ctl.top - 6 - h;
    c.popupRect = D2D1::RectF(row->ctl.right - w, top, row->ctl.right, top + h);
    const D2D1_RECT_F& r = c.popupRect;
    DrawThemeShadow(r, 10, 0.28f, 14, 6);
    Fill(r, g_darkText ? D2D1::ColorF(0.97f, 0.97f, 0.98f, 0.97f) : D2D1::ColorF(0.13f, 0.13f, 0.15f, 0.97f), 10);
    if (auto b = Solid(Fg(0.14f))) g_t->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + 0.5f, r.top + 0.5f, r.right - 0.5f, r.bottom - 0.5f), 10, 10), b.Get(), 1);
    std::wstring v = CfgValue(it);
    for (int i = 0; i < it.optCount; i++) {
        D2D1_RECT_F ir = D2D1::RectF(r.left + 5, r.top + 5 + i * itemH, r.right - 5, r.top + 5 + (i + 1) * itemH);
        bool hot = i == c.popupHover;
        if (hot) Fill(ir, CfgAccent(), 6);
        D2D1_COLOR_F ink = hot ? White(1) : Fg(0.92f);
        if (v == it.opts[i].value)
            CText(L"", D2D1::RectF(ir.left + 4, ir.top, ir.left + 24, ir.bottom), 11, DWRITE_FONT_WEIGHT_NORMAL, ink,
                  DWRITE_TEXT_ALIGNMENT_CENTER, false, GlyphFamily());
        CText(it.opts[i].label, D2D1::RectF(ir.left + 28, ir.top, ir.right - 8, ir.bottom), 13.5f, DWRITE_FONT_WEIGHT_NORMAL, ink);
    }
}

static void DrawSettings(Widget* wd) {
    CfgUi& c = g_cfg;
    CfgBuild();
    c.scroll = std::clamp(c.scroll, 0.0f, CfgMaxScroll());
    const float W = kCfgW, H = kCfgH, pad = 22;

    // The sidebar is a darker (on light glass: lighter) part of the same glass
    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    float rad = (float)g_ui.cornerRadius;
    if (SUCCEEDED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(0, 0, W, H), rad, rad), &clip))) {
        g_t->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get()), nullptr);
        if (auto b = Solid(g_darkText ? White(0.38f) : D2D1::ColorF(0.05f, 0.05f, 0.07f, 0.34f)))
            g_t->FillRectangle(D2D1::RectF(0, 0, kSideW, H), b.Get());
        g_t->PopLayer();
    }
    Line(kSideW, 1, kSideW, H - 1, 1, Fg(0.08f));

    // Search field
    Fill(c.bar, Fg(0.08f), 8);
    float bcy = (c.bar.top + c.bar.bottom) / 2;
    CText(L"", D2D1::RectF(c.bar.left + 6, bcy - 14, c.bar.left + 34, bcy + 14), 15, DWRITE_FONT_WEIGHT_NORMAL, Fg(0.70f),
          DWRITE_TEXT_ALIGNMENT_CENTER, false, GlyphFamily());
    D2D1_RECT_F stext = D2D1::RectF(c.bar.left + 40, c.bar.top, c.bar.right - 10, c.bar.bottom);
    if (c.search.empty()) CText(L"Pesquisar ajustes", stext, 14, DWRITE_FONT_WEIGHT_NORMAL, Fg(0.50f));
    else CText(c.search, stext, 14, DWRITE_FONT_WEIGHT_NORMAL, Fg(0.95f));
    if (c.searchFocus || !c.search.empty()) {
        float x = c.search.empty() ? stext.left : std::min(stext.right, stext.left + TextWidth(c.search, 14) + 1);
        Line(x, bcy - 9, x, bcy + 9, 1.3f, Fg(0.9f));
    }

    // Sections
    for (const CfgSideRow& row : c.side) {
        bool selected = c.search.empty() && row.section == c.section;
        if (selected) Fill(row.r, CfgAccent(), 7);
        else if (row.section == c.sideHover) Fill(row.r, Fg(0.09f), 7);
        float cy = (row.r.top + row.r.bottom) / 2, d = 22;
        D2D1_RECT_F icon = D2D1::RectF(row.r.left + 8, cy - d / 2, row.r.left + 8 + d, cy + d / 2);
        CfgIcon(row.section, icon, 13);
        CText(kCfgSections[row.section].name, D2D1::RectF(icon.right + 10, row.r.top, row.r.right - 8, row.r.bottom), 13.5f,
              DWRITE_FONT_WEIGHT_NORMAL, selected ? White(1) : Fg(0.92f));
    }

    // Title and Done
    std::wstring title = c.search.empty() ? kCfgSections[c.section].name : L"Resultados";
    CText(title, D2D1::RectF(kSideW + pad, pad, c.done.left - 12, pad + 30), 21, DWRITE_FONT_WEIGHT_SEMI_BOLD, Fg(0.95f),
          DWRITE_TEXT_ALIGNMENT_LEADING, false, kDisplayFont);
    DrawClearGlassButton(c.done, (c.done.bottom - c.done.top) / 2, 1.0f, c.doneHover ? 1.7f : 1.15f, false);
    CText(L"Concluído", c.done, 13, DWRITE_FONT_WEIGHT_MEDIUM, Fg(0.95f), DWRITE_TEXT_ALIGNMENT_CENTER);

    g_t->PushAxisAlignedClip(c.view, D2D1_ANTIALIAS_MODE_ALIASED);
    if (c.header.right > c.header.left) {
        const CfgSection& sec = kCfgSections[c.section];
        Fill(c.header, Fg(0.07f), 12);
        float d = 42, cy = (c.header.top + c.header.bottom) / 2;
        D2D1_RECT_F icon = D2D1::RectF(c.header.left + 14, cy - d / 2, c.header.left + 14 + d, cy + d / 2);
        CfgIcon(c.section, icon, 22);
        float tx = icon.right + 14, textW = c.header.right - 14 - tx;
        float dh = TextHeight(sec.desc, 12, textW), block = 20 + dh, top = cy - block / 2;
        CText(sec.name, D2D1::RectF(tx, top, c.header.right - 14, top + 20), 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, Fg(0.95f),
              DWRITE_TEXT_ALIGNMENT_LEADING, false, kDisplayFont);
        CText(sec.desc, D2D1::RectF(tx, top + 20, tx + textW, top + block + 4), 12, DWRITE_FONT_WEIGHT_NORMAL, Fg(0.58f),
              DWRITE_TEXT_ALIGNMENT_LEADING, true);
    }
    for (const CfgCaption& cap : c.captions)
        CText(cap.text, cap.r, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, Fg(0.55f));
    for (const D2D1_RECT_F& card : c.cards) Fill(card, Fg(0.07f), 12);
    for (size_t i = 0; i < c.rows.size(); i++) {
        const CfgRow& row = c.rows[i];
        if (row.r.bottom < c.view.top || row.r.top > c.view.bottom) continue;
        float in = 14;
        if (row.item <= kPlaceRow) {
            if (i > 0 && c.rows[i - 1].r.bottom == row.r.top)
                Line(row.r.left + in, row.r.top + 0.5f, row.r.right - in, row.r.top + 0.5f, 1, Fg(0.10f));
            if (row.item == kStatusRow) {
                CText(g_placeStatus, D2D1::RectF(row.r.left + in, row.r.top, row.r.right - in, row.r.bottom), 12.5f,
                      DWRITE_FONT_WEIGHT_NORMAL, Fg(0.60f));
                continue;
            }
            const Place& pl = g_places[kPlaceRow - row.item];
            float top = (row.r.top + row.r.bottom) / 2 - 18;
            CText(pl.name, D2D1::RectF(row.r.left + in, top, row.ctl.left - 16, top + 19), 13.5f, DWRITE_FONT_WEIGHT_NORMAL, Fg(0.94f));
            CText(pl.region + L"  ·  " + pl.lat + L", " + pl.lon, D2D1::RectF(row.r.left + in, top + 20, row.ctl.left - 16, top + 36),
                  11.5f, DWRITE_FONT_WEIGHT_NORMAL, Fg(0.55f));
            GlassField(row.ctl, 7, (int)i == c.hover ? 1.3f : 0.9f);
            CText(L"Usar", row.ctl, 13, DWRITE_FONT_WEIGHT_MEDIUM, Fg(0.92f), DWRITE_TEXT_ALIGNMENT_CENTER);
            continue;
        }
        const CfgItem& it = kCfgItems[row.item];
        if (i > 0 && c.rows[i - 1].r.bottom == row.r.top)
            Line(row.r.left + in, row.r.top + 0.5f, row.r.right - in, row.r.top + 0.5f, 1, Fg(0.10f));
        bool unused = CfgItemUnused(it);
        bool hasDesc = it.desc && *it.desc;
        float block = row.labelH + (hasDesc ? 2 + row.descH : 0);
        float top = (row.r.top + row.r.bottom) / 2 - block / 2;
        float labelRight = row.ctl.left - 16;
        CText(it.label, D2D1::RectF(row.r.left + in, top, labelRight, top + row.labelH), 13.5f, DWRITE_FONT_WEIGHT_NORMAL,
              Fg(unused ? 0.55f : 0.94f));
        if (hasDesc)
            CText(it.desc, D2D1::RectF(row.r.left + in, top + row.labelH + 2, labelRight, top + block + 4), 11.5f,
                  DWRITE_FONT_WEIGHT_NORMAL, Fg(unused ? 0.42f : 0.55f), DWRITE_TEXT_ALIGNMENT_LEADING, true);
        g_cfgDrawLocked = unused;
        DrawCfgControl(row, (int)i == c.hover);
        g_cfgDrawLocked = false;
    }
    if (c.nothing)
        CText(L"Nenhum ajuste encontrado", D2D1::RectF(kSideW + pad, c.view.top + 10, W - pad, c.view.top + 40), 13.5f,
              DWRITE_FONT_WEIGHT_NORMAL, Fg(0.55f));
    g_t->PopAxisAlignedClip();

    float maxScroll = CfgMaxScroll();
    if (maxScroll > 1) {
        float viewH = c.view.bottom - c.view.top, barH = std::max(30.0f, viewH * viewH / (viewH + maxScroll));
        float y = c.view.top + (viewH - barH) * (c.scroll / maxScroll);
        Fill(D2D1::RectF(W - 8, y, W - 4.5f, y + barH), Fg(0.22f), 2);
    }
    DrawCfgPopup();
}

// ---------- Changing values ----------

static bool IsStructural(PCWSTR key) {
    return (!wcsncmp(key, L"show", 4) && wcscmp(key, L"showGrid")) || !wcscmp(key, L"liveGlass");
}

// The new value reaches the widgets (now, or every 80 ms while a slider is dragged)
static void CfgApply(PCWSTR key, bool now) {
    LoadSettings();
    g_ui = GetSettings();
    if (!wcscmp(key, L"calendarIcs") || !wcscmp(key, L"stocks") || !wcscmp(key, L"newsFeeds") || !wcsncmp(key, L"ha", 2)) {
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            g_eventsRefresh = true;
        }
        g_cv.notify_all();
    }
    if (!wcscmp(key, L"photosFolder")) {
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            g_photoNext = true;
        }
        g_cv.notify_all();
    }
    if (!wcscmp(key, L"language")) {  // messages already fetched come again in the new language
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            g_eventsRefresh = g_weatherRefresh = true;
        }
        g_cv.notify_all();
    }
    if (!wcscmp(key, L"city") || !wcscmp(key, L"latitude") || !wcscmp(key, L"longitude")) {
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            g_weatherRefresh = true;
        }
        g_cv.notify_all();
    }
    if (IsStructural(key)) {
        RebuildWidgets();
        return;
    }
    UpdateAnimTimer();
    ULONGLONG t = GetTickCount64();
    if (now || t - g_cfg.lastApply >= 80) {
        g_cfg.lastApply = t;
        RenderAll();
    } else if (g_cfg.wd) {
        Render(g_cfg.wd);
    }
}

static void CfgSetValue(int item, const std::wstring& v, bool now = true) {
    const CfgItem& it = kCfgItems[item];
    if (CfgNow(it.key) == v) return;
    CfgSet(it.key, v);
    CfgApply(it.key, now);
}

static void CfgSliderFromX(int item, float x) {
    for (const CfgRow& row : g_cfg.rows)
        if (row.item == item) {
            const CfgItem& it = kCfgItems[item];
            D2D1_RECT_F tr = CfgSliderTrack(row.ctl);
            float k = std::clamp((x - tr.left) / std::max(1.0f, tr.right - tr.left), 0.0f, 1.0f);
            int v = it.min + (int)std::lround(k * (it.max - it.min));
            CfgSetValue(item, std::to_wstring(v), false);
            return;
        }
}

static void CfgCommitEdit() {
    CfgUi& c = g_cfg;
    if (c.edit < 0) return;
    int item = c.edit;
    std::wstring v = c.editText;
    c.edit = -1;
    const CfgItem& it = kCfgItems[item];
    if (!wcscmp(it.key, L"#citySearch")) {
        g_cityQuery = v;
        g_places.clear();
        while (!g_cityQuery.empty() && g_cityQuery.back() == L' ') g_cityQuery.pop_back();
        if (g_cityQuery.empty()) {
            g_placeStatus.clear();
            return;
        }
        g_placeStatus = Fmt(TF(L"Procurando \u201C%ls\u201D…"), g_cityQuery.c_str());
        std::thread([q = g_cityQuery] { SearchPlaces(q); }).detach();
        return;
    }
    if (it.type == kCfgColor) {
        std::wstring h = v;
        if (!h.empty() && h[0] == L'#') h = h.substr(1);
        if (h.size() != 6 || h.find_first_not_of(L"0123456789abcdefABCDEF") != std::wstring::npos) return;  // not a color
        for (auto& ch : h) ch = towupper(ch);
        v = L"#" + h;
    }
    CfgSetValue(item, v);
}

static void CloseSettings() {
    CfgUi& c = g_cfg;
    if (!c.wd) return;
    CfgCommitEdit();
    Widget* wd = c.wd;
    c.wd = nullptr;
    SetWindowLongPtrW(wd->hwnd, GWLP_USERDATA, 0);
    ReleaseSurface(wd);
    DestroyWindow(wd->hwnd);
    delete wd;
    c.search.clear();
    c.searchFocus = false;
    c.scroll = 0;
    c.hover = c.sideHover = c.popup = c.popupHover = c.drag = c.edit = -1;
    c.doneHover = false;
}

// Picks an .ics file and adds it to the calendars (a file already there isn't added twice)
static bool ImportIcs(HWND owner) {
    ComPtr<IFileOpenDialog> dlg;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg)))) return false;
    COMDLG_FILTERSPEC types[] = {{L"Calendário (*.ics)", L"*.ics;*.ical;*.ifb"}, {L"Todos os arquivos", L"*.*"}};
    dlg->SetFileTypes(2, types);
    dlg->SetTitle(L"Importar calendário");
    DWORD opts = 0;
    dlg->GetOptions(&opts);
    dlg->SetOptions(opts | FOS_FILEMUSTEXIST | FOS_FORCEFILESYSTEM | FOS_ALLOWMULTISELECT);
    if (FAILED(dlg->Show(owner))) return false;  // cancelled
    ComPtr<IShellItemArray> items;
    if (FAILED(dlg->GetResults(&items))) return false;
    DWORD count = 0;
    items->GetCount(&count);
    std::wstring now = CfgNow(L"calendarIcs");
    auto have = SplitList(now, L";\n");
    for (DWORD i = 0; i < count; i++) {
        ComPtr<IShellItem> item;
        PWSTR path = nullptr;
        if (FAILED(items->GetItemAt(i, &item)) || FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) continue;
        std::wstring p = path;
        CoTaskMemFree(path);
        bool dup = false;
        for (auto& h : have) dup = dup || Lower(h) == Lower(p);
        if (dup) continue;
        have.push_back(p);
        now += (now.empty() ? L"" : L"; ") + p;
    }
    CfgSet(L"calendarIcs", now);
    CfgApply(L"calendarIcs", true);
    return true;
}

static void OpenPicker(int kind);
static void RebuildWidgets();

static void BackupSave(HWND owner);
static void BackupLoad(HWND owner);

static void CfgRunButton(const CfgItem& it) {
    if (!CfgButtonEnabled(it)) return;
    if (!wcscmp(it.key, L"#importIcs")) {
        ImportIcs(g_cfg.wd ? g_cfg.wd->hwnd : nullptr);
        return;
    }
    if (!wcscmp(it.key, L"#backupSave") || !wcscmp(it.key, L"#backupLoad")) {
        HWND owner = g_cfg.wd ? g_cfg.wd->hwnd : nullptr;
        if (!wcscmp(it.key, L"#backupSave")) BackupSave(owner);
        else BackupLoad(owner);
        return;
    }
    if (!wcscmp(it.key, L"#pickShortcuts") || !wcscmp(it.key, L"#pickHome")) {
        OpenPicker(!wcscmp(it.key, L"#pickHome") ? 0 : 1);
        return;
    }
    if (!wcscmp(it.key, L"#themeReset")) CfgResetTheme();
    else if (!wcscmp(it.key, L"#resetAll")) CfgResetAll();
    LoadSettings();
    g_ui = GetSettings();
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        g_weatherRefresh = true;
    }
    g_cv.notify_all();
    RebuildWidgets();
}

// ---------- Input ----------

static void CfgUpdateHover(D2D1_POINT_2F p) {
    CfgUi& c = g_cfg;
    int hover = -1, side = -1, popupHover = -1;
    bool done = InRect(c.done, p);
    if (c.popup >= 0 && InRect(c.popupRect, p)) {
        popupHover = std::clamp((int)((p.y - c.popupRect.top - 5) / 30), 0, kCfgItems[c.popup].optCount - 1);
    } else {
        for (const CfgSideRow& r : c.side)
            if (InRect(r.r, p)) side = r.section;
        if (InRect(c.view, p))
            for (size_t i = 0; i < c.rows.size(); i++)
                if (InRect(c.rows[i].ctl, p)) hover = (int)i;
    }
    if (hover != c.hover || side != c.sideHover || done != c.doneHover || popupHover != c.popupHover) {
        c.hover = hover;
        c.sideHover = side;
        c.doneHover = done;
        c.popupHover = popupHover;
        Render(c.wd);
    }
}

static void CfgClick(D2D1_POINT_2F p) {
    CfgUi& c = g_cfg;
    if (c.popup >= 0) {
        if (InRect(c.popupRect, p)) {
            const CfgItem& it = kCfgItems[c.popup];
            int i = std::clamp((int)((p.y - c.popupRect.top - 5) / 30), 0, it.optCount - 1);
            int item = c.popup;
            c.popup = -1;
            CfgSetValue(item, it.opts[i].value);
        } else {
            c.popup = -1;
        }
        Render(c.wd);
        return;
    }
    if (c.edit >= 0) CfgCommitEdit();
    if (InRect(c.done, p)) {
        CloseSettings();
        return;
    }
    if (InRect(c.bar, p)) {
        c.searchFocus = true;
        Render(c.wd);
        return;
    }
    c.searchFocus = false;
    for (const CfgSideRow& r : c.side)
        if (InRect(r.r, p)) {
            c.section = r.section;
            c.search.clear();
            c.scroll = 0;
            Render(c.wd);
            return;
        }
    if (InRect(c.view, p))
        for (const CfgRow& row : c.rows) {
            if (!InRect(row.ctl, p)) continue;
            if (row.item <= kPlaceRow) {
                if (row.item != kStatusRow && kPlaceRow - row.item < (int)g_places.size()) {
                    Place pl = g_places[kPlaceRow - row.item];
                    CfgSet(L"city", pl.name);
                    CfgSet(L"latitude", pl.lat);
                    CfgSet(L"longitude", pl.lon);
                    g_places.clear();
                    g_placeStatus = Fmt(TF(L"Usando %ls"), (pl.name + (pl.region.empty() ? L"" : L" (" + pl.region + L")")).c_str());
                    CfgApply(L"city", true);
                }
                if (g_cfg.wd) Render(g_cfg.wd);
                return;
            }
            const CfgItem& it = kCfgItems[row.item];
            switch (it.type) {
                case kCfgToggle: CfgSetValue(row.item, _wtoi(CfgNow(it.key).c_str()) ? L"0" : L"1"); break;
                case kCfgSlider:
                    c.drag = row.item;
                    SetCapture(c.wd->hwnd);
                    CfgSliderFromX(row.item, p.x);
                    break;
                case kCfgChoice: c.popup = row.item; break;
                case kCfgColor:
                case kCfgText:
                    c.edit = row.item;
                    c.editText = !wcscmp(it.key, L"#citySearch") ? g_cityQuery : CfgNow(it.key);
                    break;
                case kCfgButton: CfgRunButton(it); break;
                default: break;
            }
            if (g_cfg.wd) Render(g_cfg.wd);
            return;
        }
    Render(c.wd);
}

static bool CfgMessage(Widget* wd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result) {
    CfgUi& c = g_cfg;
    auto point = [&]() {
        POINT pt = {(short)LOWORD(lp), (short)HIWORD(lp)};
        return D2D1::Point2F(pt.x / wd->scale - kMargin, pt.y / wd->scale - kMargin);
    };
    switch (msg) {
        case WM_MOUSEMOVE: {
            D2D1_POINT_2F p = point();
            if (c.drag >= 0) {
                CfgSliderFromX(c.drag, p.x);
            } else {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, wd->hwnd, 0};
                TrackMouseEvent(&tme);
                CfgUpdateHover(p);
            }
            *result = 0;
            return true;
        }
        case WM_MOUSELEAVE:
            if (c.drag < 0) CfgUpdateHover(D2D1::Point2F(-1000, -1000));
            *result = 0;
            return true;
        case WM_LBUTTONDOWN:
            CfgClick(point());
            *result = 0;
            return true;
        case WM_LBUTTONUP:
            if (c.drag >= 0) {
                int item = c.drag;
                c.drag = -1;
                ReleaseCapture();
                CfgApply(kCfgItems[item].key, true);
            }
            *result = 0;
            return true;
        case WM_CAPTURECHANGED:
            c.drag = -1;
            *result = 0;
            return true;
        case WM_MOUSEWHEEL:
            c.scroll = std::clamp(c.scroll - GET_WHEEL_DELTA_WPARAM(wp) / 120.0f * 48, 0.0f, CfgMaxScroll());
            c.popup = -1;
            Render(wd);
            *result = 0;
            return true;
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE) {
                if (c.popup >= 0) c.popup = -1;
                else if (c.edit >= 0) c.edit = -1;
                else if (!c.search.empty()) c.search.clear();
                else {
                    CloseSettings();
                    *result = 0;
                    return true;
                }
            } else if (wp == VK_RETURN) {
                CfgCommitEdit();
            } else if (wp == VK_BACK) {
                std::wstring& t = c.edit >= 0 ? c.editText : c.search;
                if (!t.empty()) t.pop_back();
                if (c.edit < 0) c.scroll = 0;
            } else {
                *result = 0;
                return true;
            }
            if (c.wd) Render(c.wd);
            *result = 0;
            return true;
        case WM_CHAR:
            if (wp == 22 && (c.edit >= 0 || c.searchFocus)) {  // Ctrl+V: links, tokens and paths are pasted, not typed
                if (OpenClipboard(wd->hwnd)) {
                    if (HANDLE h = GetClipboardData(CF_UNICODETEXT)) {
                        if (auto* p = (const wchar_t*)GlobalLock(h)) {
                            std::wstring t;
                            for (const wchar_t* q = p; *q && t.size() < 4000; q++)
                                if (*q >= 32) t += *q;
                                else if (*q == L'\n' && !t.empty() && t.back() != L' ') t += L"; ";  // several lines: a list
                            GlobalUnlock(h);
                            std::wstring& dst = c.edit >= 0 ? c.editText : c.search;
                            size_t room = c.edit >= 0 ? 4000 : 40;
                            dst += t.substr(0, room > dst.size() ? room - dst.size() : 0);
                        }
                    }
                    CloseClipboard();
                }
                Render(wd);
                *result = 0;
                return true;
            }
            if (wp >= 32 && wp != 127) {
                if (c.edit >= 0) {
                    if (c.editText.size() < 4000) c.editText += (wchar_t)wp;  // room for several calendars and feeds
                } else if (c.search.size() < 40) {
                    c.search += (wchar_t)wp;
                    c.searchFocus = true;
                    c.scroll = 0;
                    c.popup = -1;
                }
                Render(wd);
            }
            *result = 0;
            return true;
        case WM_NCHITTEST: {
            POINT pt = {(short)LOWORD(lp), (short)HIWORD(lp)};
            ScreenToClient(wd->hwnd, &pt);
            D2D1_POINT_2F p = D2D1::Point2F(pt.x / wd->scale - kMargin, pt.y / wd->scale - kMargin);
            if (p.x < 0 || p.y < 0 || p.x > kCfgW || p.y > kCfgH) *result = HTTRANSPARENT;
            else if (c.popup < 0 && ((p.x > kSideW && p.y < c.view.top && !InRect(c.done, p)) ||
                                     (p.x < kSideW && (c.side.empty() || p.y > c.side.back().r.bottom + 4))))
                *result = HTCAPTION;  // drag the window by its title area or the empty part of the sidebar
            else *result = HTCLIENT;
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Rendering and windows
// ---------------------------------------------------------------------------

static void ReleaseSurface(Widget* wd) {
    wd->source.Reset();
    wd->blurFx.Reset();
    wd->mapXf.Reset();
    wd->disp.Reset();
    wd->color.Reset();
    wd->map.Reset();
    wd->snap.Reset();
    wd->imgBmp.Reset();
    wd->shownImg = nullptr;
    wd->iconBmps.clear();
    wd->dc.Reset();
    wd->rt.Reset();
    if (wd->memDC) {
        SelectObject(wd->memDC, wd->oldBmp);
        DeleteDC(wd->memDC);
    }
    if (wd->dib) DeleteObject(wd->dib);
    wd->memDC = nullptr;
    wd->dib = nullptr;
    wd->bits = nullptr;
    wd->pw = wd->ph = 0;
}

static void WindowSize(Widget* wd, int* pw, int* ph) {
    *pw = (int)std::ceil((wd->w + 2 * kMargin) * wd->scale);
    *ph = (int)std::ceil((wd->h + 2 * kMargin) * wd->scale);
}

static bool EnsureSurface(Widget* wd) {
    int pw, ph;
    WindowSize(wd, &pw, &ph);
    if (wd->rt && wd->pw == pw && wd->ph == ph) return true;
    ReleaseSurface(wd);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = pw;
    bi.bmiHeader.biHeight = -ph;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC screen = GetDC(nullptr);
    wd->memDC = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!wd->memDC) return false;
    wd->dib = CreateDIBSection(wd->memDC, &bi, DIB_RGB_COLORS, &wd->bits, nullptr, 0);
    if (!wd->dib) {
        ReleaseSurface(wd);
        return false;
    }
    wd->oldBmp = SelectObject(wd->memDC, wd->dib);
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
    if (FAILED(g_d2d->CreateDCRenderTarget(&props, &wd->rt))) {
        ReleaseSurface(wd);
        return false;
    }
    if (FAILED(wd->rt.As(&wd->dc))) Wh_Log(L"No device context: the glass is drawn without blur");
    wd->rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    wd->pw = pw;
    wd->ph = ph;
    wd->fresh = true;
    return true;
}

static void DrawMenu(Widget* wd);

// The cell a dragged widget will land on: clear glass with a bright rim
static void DrawGhost(Widget* wd) {
    float r = (float)g_ui.cornerRadius;
    D2D1_RECT_F rc = D2D1::RectF(1, 1, wd->w - 1, wd->h - 1);
    Fill(rc, White(0.13f), r);
    D2D1_GRADIENT_STOP rim[4] = {{0.0f, White(0.75f)}, {0.35f, White(0.25f)}, {0.70f, White(0.18f)}, {1.0f, White(0.50f)}};
    if (auto b = MakeGradient(rim, 4, D2D1::Point2F(0, 0), D2D1::Point2F(wd->w, wd->h)))
        g_t->DrawRoundedRectangle(D2D1::RoundedRect(rc, r, r), b.Get(), 1.5f);
    D2D1_GRADIENT_STOP glow[2] = {{0.0f, White(0.12f)}, {1.0f, White(0.0f)}};
    if (auto b = MakeGradient(glow, 2, D2D1::Point2F(0, 0), D2D1::Point2F(0, std::min(wd->h * 0.4f, 40.0f))))
        g_t->FillRoundedRectangle(D2D1::RoundedRect(rc, r, r), b.Get());
}

// Every cell of the grid, faint, while a widget is dragged ("Mostrar a grade ao arrastar")
static void DrawGridCells(Widget* wd) {
    float pitch = kCell + kGap, r = (float)g_ui.cornerRadius;
    for (float y = kEdge - kMargin; y + kCell <= wd->h + 2 * kMargin - kEdge + 0.5f - kMargin; y += pitch)
        for (float x = kEdge - kMargin; x + kCell <= wd->w + 2 * kMargin - kEdge + 0.5f - kMargin; x += pitch) {
            D2D1_RECT_F rc = D2D1::RectF(x, y, x + kCell, y + kCell);
            Fill(rc, White(0.05f), r);
            if (auto b = Solid(White(0.18f))) g_t->DrawRoundedRectangle(D2D1::RoundedRect(rc, r, r), b.Get(), 1);
        }
}

static void PublishGlassTargets();
static void DrawPicker(Widget* wd);
static bool PickerMessage(Widget* wd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result);
static void OpenPicker(int kind);
static void ClosePicker();

static void Render(Widget* wd) {
    if (!wd->hwnd || !EnsureSurface(wd)) return;
    g_cw = wd;
    g_s = wd->scale;
    g_t = wd->rt.Get();
    Theme t = g_ui.theme;
    g_darkText = t == Theme::Light ? true : t == Theme::Tinted ? Luma(ColorOf(g_ui.tintColor)) > 0.6f : false;
    if (g_ui.textColor >= 0) g_darkText = g_ui.textColor == 1;
    else if ((t == Theme::Glass || t == Theme::Frost) && wd->lum >= 0) g_darkText = wd->lum > 0.55f;

    RECT bind = {0, 0, wd->pw, wd->ph};
    if (FAILED(wd->rt->BindDC(wd->memDC, &bind))) return;
    g_t->BeginDraw();
    g_t->SetTransform(D2D1::Matrix3x2F::Identity());
    g_t->Clear(D2D1::ColorF(0, 0, 0, 0));
    float m = kMargin * g_s;
    D2D1_RECT_F r = D2D1::RectF(m, m, m + wd->w * g_s, m + wd->h * g_s);
    g_noBacklight = wd->kind == W_SETTINGS || wd->kind == W_MENU || wd->kind == W_PICKER;
    float radius = (float)(wd->kind == W_MENU ? std::min(g_ui.cornerRadius, 14) : g_ui.cornerRadius);
    if (wd->kind != W_GHOST && wd->kind != W_GRID) DrawThemedShape(r, radius * g_s);
    g_t->SetTransform(D2D1::Matrix3x2F::Scale(g_s, g_s) * D2D1::Matrix3x2F::Translation(m, m));
    wd->actions.clear();
    switch (wd->kind) {
        case W_CLOCK: DrawClock(wd); break;
        case W_WEATHER: DrawWeather(wd); break;
        case W_MUSIC: DrawMusic(wd); break;
        case W_SYSTEM: DrawSystem(wd); break;
        case W_CALENDAR: DrawCalendar(wd); break;
        case W_WORLD: DrawWorld(wd); break;
        case W_SUN: DrawSun(wd); break;
        case W_BATTERY: DrawBattery(wd); break;
        case W_NETWORK: DrawNetwork(wd); break;
        case W_PHOTOS: DrawPhotos(wd); break;
        case W_NOTIF: DrawNotif(wd); break;
        case W_NOTES: DrawNotes(wd); break;
        case W_REMINDERS: DrawReminders(wd); break;
        case W_SHORTCUTS: DrawShortcuts(wd); break;
        case W_STOCKS: DrawStocks(wd); break;
        case W_NEWS: DrawNews(wd); break;
        case W_SCREENTIME: DrawScreenTime(wd); break;
        case W_HOME: DrawHome(wd); break;
        case W_SENSORS: DrawSensors(wd); break;
        case W_SETTINGS: DrawSettings(wd); break;
        case W_PICKER: DrawPicker(wd); break;
        case W_MENU: DrawMenu(wd); break;
        case W_GHOST: DrawGhost(wd); break;
        case W_GRID: DrawGridCells(wd); break;
    }
    g_noBacklight = false;
    HRESULT hr = g_t->EndDraw();
    g_t = nullptr;
    if (hr == (HRESULT)D2DERR_RECREATE_TARGET) {
        ReleaseSurface(wd);
        return;
    }
    RECT wr;
    GetWindowRect(wd->hwnd, &wr);
    POINT dst = {wr.left, wr.top}, src = {0, 0};
    SIZE sz = {wd->pw, wd->ph};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    HDC screen = GetDC(nullptr);
    UpdateLayeredWindow(wd->hwnd, screen, &dst, &sz, wd->memDC, &src, 0, &bf, ULW_ALPHA);
    ReleaseDC(nullptr, screen);
}

static Widget* FromHwnd(HWND h) { return (Widget*)GetWindowLongPtrW(h, GWLP_USERDATA); }

// A point in window pixels, as DIPs inside the glass
static D2D1_POINT_2F GlassPoint(Widget* wd, POINT pt) {
    return D2D1::Point2F(pt.x / wd->scale - kMargin, pt.y / wd->scale - kMargin);
}
static bool InGlass(Widget* wd, D2D1_POINT_2F p) { return p.x >= 0 && p.y >= 0 && p.x <= wd->w && p.y <= wd->h; }
static bool IsDesktopWidget(const Widget* wd) {
    return wd && wd->kind <= W_SENSORS;
}

static int HitMusicButton(Widget* wd, D2D1_POINT_2F p) {
    MusicButtons b = MusicButtonsFor(wd);
    if (p.y < b.y || p.y > b.y + b.size) return -1;
    for (int i = 0; i < 3; i++)
        if (b.x[i] > -100 && p.x >= b.x[i] && p.x <= b.x[i] + b.size) return i;
    return -1;
}

// Every desktop widget: its kind, key, the setting that shows it, its name, default size and cell
struct WidgetDef {
    WidgetKind kind;
    PCWSTR key, showKey, name;
    int size, col, row;
};
static const WidgetDef kWidgetDefs[] = {
    {W_CLOCK, L"clock", L"showClock", L"Relógio", 0, 0, 0},
    {W_WEATHER, L"weather", L"showWeather", L"Tempo", 0, 1, 0},
    {W_MUSIC, L"music", L"showMusic", L"Música", 1, 0, 1},
    {W_SYSTEM, L"system", L"showSystem", L"Sistema", 1, 0, 2},
    {W_CALENDAR, L"calendar", L"showCalendar", L"Calendário", 0, 2, 0},
    {W_WORLD, L"world", L"showWorld", L"Relógio Mundial", 1, 2, 1},
    {W_SUN, L"sun", L"showSun", L"Nascer do Sol", 0, 3, 0},
    {W_BATTERY, L"battery", L"showBattery", L"Baterias", 0, 2, 2},
    {W_NETWORK, L"network", L"showNetwork", L"Rede", 0, 3, 2},
    {W_PHOTOS, L"photos", L"showPhotos", L"Fotos", 2, 4, 0},
    {W_NOTIF, L"notif", L"showNotif", L"Notificações", 2, 0, 3},
    {W_NOTES, L"notes", L"showNotes", L"Notas", 0, 4, 2},
    {W_REMINDERS, L"reminders", L"showReminders", L"Lembretes", 0, 5, 2},
    {W_SHORTCUTS, L"shortcuts", L"showShortcuts", L"Atalhos", 1, 2, 3},
    {W_STOCKS, L"stocks", L"showStocks", L"Bolsa", 1, 4, 3},
    {W_NEWS, L"news", L"showNews", L"Notícias", 1, 2, 4},
    {W_SCREENTIME, L"screentime", L"showScreenTime", L"Tempo de Uso", 0, 6, 0},
    {W_HOME, L"home", L"showHome", L"Casa", 1, 0, 4},
    {W_SENSORS, L"sensors", L"showSensors", L"Sensores", 1, 2, 2},
};
static const int kWidgetDefCount = (int)(sizeof(kWidgetDefs) / sizeof(kWidgetDefs[0]));

static PCWSTR ShowKeyOf(const std::wstring& key) {
    for (const WidgetDef& d : kWidgetDefs)
        if (key == d.key) return d.showKey;
    return L"showClock";
}

// ---------------------------------------------------------------------------
// The desktop layer: widgets sit right above the desktop window, under every app window. When something brings
// the desktop up (Win+D, "Show desktop"), they are put back above it.
// ---------------------------------------------------------------------------

static bool g_placing = false;  // we are changing the z-order ourselves
static HWINEVENTHOOK g_fgHook = nullptr;
#define WM_APP_DESK (WM_APP + 4)
#define WM_APP_MENUCLOSE (WM_APP + 5)

static bool IsDeskClass(HWND h) {
    wchar_t c[32] = {};
    GetClassNameW(h, c, 32);
    return !wcscmp(c, L"Progman") || !wcscmp(c, L"WorkerW");
}
static bool IsOurWindow(HWND h) {
    wchar_t c[64] = {};
    GetClassNameW(h, c, 64);
    return !wcscmp(c, kClassName);
}
static HWND TopDesktopWindow() {
    for (HWND w = GetTopWindow(nullptr); w; w = GetWindow(w, GW_HWNDNEXT))
        if (IsWindowVisible(w) && IsDeskClass(w)) return w;
    return nullptr;
}

struct Spring {
    Widget* wd;
    POINT from, to;
    double t0;
};
struct DragState {
    Widget* wd = nullptr;
    bool dragging = false;
    int button = -1;  // a music button pressed (a click, never a drag)
    POINT start{}, winStart{};
    int tc = -1, tr = -1;  // the cell it will land on
};
static DragState g_drag;
static std::vector<Spring> g_springs;
static Widget* g_ghost = nullptr;  // the glass outline of the cell the widget will land on
static Widget* g_gridWin = nullptr;  // every cell, faint (optional)
static double g_gx = 0, g_gy = 0, g_lastFrame = 0;
static POINT g_ghostTo{};
static UINT_PTR g_frameTimer = 0;

static void PlaceAtDesktop() {
    HWND desk = TopDesktopWindow();
    HWND above = desk ? GetWindow(desk, GW_HWNDPREV) : nullptr;
    while (above && IsOurWindow(above)) above = GetWindow(above, GW_HWNDPREV);
    HWND after = desk ? (above ? above : HWND_TOP) : HWND_BOTTOM;
    std::vector<HWND> order;  // top to bottom: the one being dragged, its outline, the others, the grid
    if (g_drag.dragging && g_drag.wd) order.push_back(g_drag.wd->hwnd);
    if (g_ghost) order.push_back(g_ghost->hwnd);
    for (Widget* wd : g_widgets)
        if (!(g_drag.dragging && wd == g_drag.wd)) order.push_back(wd->hwnd);
    if (g_gridWin) order.push_back(g_gridWin->hwnd);
    g_placing = true;
    for (HWND h : order) {
        SetWindowPos(h, after, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        after = h;
    }
    g_placing = false;
}

// Did the desktop come up over the widgets?
static bool WidgetsBelowDesktop() {
    for (HWND w = GetTopWindow(nullptr); w; w = GetWindow(w, GW_HWNDNEXT)) {
        if (IsOurWindow(w)) {
            if (IsDesktopWidget(FromHwnd(w))) return false;
            continue;
        }
        if (IsWindowVisible(w) && IsDeskClass(w)) return true;
    }
    return false;
}

static void CALLBACK OnForeground(HWINEVENTHOOK, DWORD, HWND h, LONG obj, LONG, DWORD, DWORD) {
    if (obj == OBJID_WINDOW && h && IsDeskClass(h)) PostThreadMessageW(g_uiThreadId, WM_APP_DESK, 0, 0);
}

// ---------------------------------------------------------------------------
// The grid: cells of 164 DIPs with 16 between them, from the top-left of the main screen's work area.
// A medium widget covers 2 × 1 cells, a large one 2 × 2.
// ---------------------------------------------------------------------------


struct Grid {
    RECT work;
    float sc;
    int cols, rows;
};
static Grid GridNow() {
    Grid g;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &g.work, 0);
    g.sc = GetDpiForSystem() / 96.0f;
    float w = (g.work.right - g.work.left) / g.sc - 2 * kEdge + kGap, h = (g.work.bottom - g.work.top) / g.sc - 2 * kEdge + kGap;
    g.cols = std::max(1, (int)(w / (kCell + kGap)));
    g.rows = std::max(1, (int)(h / (kCell + kGap)));
    return g;
}
static int SpanC(int size) { return size == 0 ? 1 : 2; }
static int SpanR(int size) { return size == 3 ? 3 : size == 2 ? 2 : 1; }
// Where a widget's window goes for that cell (the window is larger than the glass by kMargin)
static POINT WindowAt(const Grid& g, int c, int r) {
    return {g.work.left + (LONG)std::lround((kEdge + c * (kCell + kGap) - kMargin) * g.sc),
            g.work.top + (LONG)std::lround((kEdge + r * (kCell + kGap) - kMargin) * g.sc)};
}
static bool Fits(const Grid& g, int size, int c, int r) {
    return c >= 0 && r >= 0 && c + SpanC(size) <= g.cols && r + SpanR(size) <= g.rows;
}
static bool CellFree(const Widget* self, int size, int c, int r) {
    for (Widget* o : g_widgets) {
        if (o == self || o->col < 0) continue;
        if (c < o->col + SpanC(o->size) && o->col < c + SpanC(size) && r < o->row + SpanR(o->size) && o->row < r + SpanR(size))
            return false;
    }
    return true;
}
// The free cell whose window position is nearest to `win`
static bool NearestFree(const Grid& g, const Widget* wd, POINT win, int* bc, int* br) {
    double best = 1e300;
    bool found = false;
    for (int r = 0; r < g.rows; r++)
        for (int c = 0; c < g.cols; c++) {
            if (!Fits(g, wd->size, c, r) || !CellFree(wd, wd->size, c, r)) continue;
            POINT p = WindowAt(g, c, r);
            double dx = p.x - win.x, dy = p.y - win.y, d = dx * dx + dy * dy;
            if (d < best) {
                best = d;
                *bc = c;
                *br = r;
                found = true;
            }
        }
    return found;
}

static void SaveCell(Widget* wd) {
    std::wstring k = wd->key;
    Wh_SetIntValue((L"cell_" + k).c_str(), wd->col >= 0 ? wd->col * 1000 + wd->row : -1);
}

// ---------------------------------------------------------------------------
// Dragging: the widget follows the mouse; a glass outline slides to the free cell it will land on; when it is
// let go, it springs into that cell
// ---------------------------------------------------------------------------

static void StartFrames() {
    if (!g_frameTimer) g_frameTimer = SetTimer(nullptr, 0, 15, nullptr);
    g_lastFrame = GetTickCount64();
}

static void DestroyHelper(Widget*& wd) {
    if (!wd) return;
    SetWindowLongPtrW(wd->hwnd, GWLP_USERDATA, 0);
    ReleaseSurface(wd);
    DestroyWindow(wd->hwnd);
    delete wd;
    wd = nullptr;
}

static Widget* CreateHelper(WidgetKind kind, float w, float h, POINT at, float scale) {
    Widget* wd = new Widget();
    wd->kind = kind;
    wd->key = L"";
    wd->w = w;
    wd->h = h;
    wd->scale = scale;
    int pw, ph;
    WindowSize(wd, &pw, &ph);
    wd->hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kClassName, L"",
                               WS_POPUP, at.x, at.y, pw, ph, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!wd->hwnd) {
        delete wd;
        return nullptr;
    }
    SetWindowLongPtrW(wd->hwnd, GWLP_USERDATA, (LONG_PTR)wd);
    Render(wd);
    ShowWindow(wd->hwnd, SW_SHOWNOACTIVATE);
    return wd;
}

static void BeginDrag(Widget* wd) {
    Grid g = GridNow();
    g_drag.dragging = true;
    g_drag.tc = wd->col;
    g_drag.tr = wd->row;
    wd->col = -1;  // its cell is free while it moves
    if (g_drag.tc < 0 && !NearestFree(g, wd, g_drag.winStart, &g_drag.tc, &g_drag.tr)) g_drag.tc = g_drag.tr = 0;
    g_springs.erase(std::remove_if(g_springs.begin(), g_springs.end(), [&](const Spring& s) { return s.wd == wd; }),
                    g_springs.end());
    if (g_ui.showGrid && !g_gridWin) {
        POINT at = {g.work.left, g.work.top};
        g_gridWin = CreateHelper(W_GRID, (g.work.right - g.work.left) / g.sc - 2 * kMargin,
                                 (g.work.bottom - g.work.top) / g.sc - 2 * kMargin, at, g.sc);
        if (g_gridWin) {  // its window covers the work area exactly
            g_placing = true;
            SetWindowPos(g_gridWin->hwnd, nullptr, g.work.left, g.work.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            g_placing = false;
            Render(g_gridWin);
        }
    }
    g_ghostTo = WindowAt(g, g_drag.tc, g_drag.tr);
    g_gx = g_ghostTo.x;
    g_gy = g_ghostTo.y;
    DestroyHelper(g_ghost);
    g_ghost = CreateHelper(W_GHOST, wd->w, wd->h, g_ghostTo, wd->scale);
    PlaceAtDesktop();
    StartFrames();
}

static void Drop(Widget* wd) {
    g_drag.dragging = false;
    wd->col = g_drag.tc;
    wd->row = g_drag.tr;
    SaveCell(wd);
    RECT r;
    GetWindowRect(wd->hwnd, &r);
    POINT to = WindowAt(GridNow(), wd->col, wd->row);
    g_springs.push_back({wd, {r.left, r.top}, to, (double)GetTickCount64()});
    DestroyHelper(g_gridWin);
    StartFrames();
}

// A spring that settles with a small overshoot (0 → 1 over about 0.6 s)
static double SpringAt(double t) {
    const double omega = 2 * 3.14159265 / 0.42, zeta = 0.78, wd = omega * std::sqrt(1 - zeta * zeta);
    return 1 - std::exp(-zeta * omega * t) * (std::cos(wd * t) + zeta / std::sqrt(1 - zeta * zeta) * std::sin(wd * t));
}

static void MoveWindowTo(Widget* wd, int x, int y) {
    g_placing = true;
    SetWindowPos(wd->hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    g_placing = false;
}

static void Capture(Widget* wd);

static void Frame() {
    double now = (double)GetTickCount64();
    double dt = std::min(50.0, now - g_lastFrame);
    g_lastFrame = now;
    for (size_t i = 0; i < g_springs.size();) {
        Spring& s = g_springs[i];
        double t = (now - s.t0) / 1000.0;
        if (t >= 0.9) {
            MoveWindowTo(s.wd, s.to.x, s.to.y);
            Capture(s.wd);
            Render(s.wd);
            g_springs.erase(g_springs.begin() + i);
            continue;
        }
        double p = SpringAt(t);
        MoveWindowTo(s.wd, (int)std::lround(s.from.x + (s.to.x - s.from.x) * p), (int)std::lround(s.from.y + (s.to.y - s.from.y) * p));
        i++;
    }
    if (g_ghost) {  // the outline glides to its cell
        double k = 1 - std::exp(-dt / 55.0);
        g_gx += (g_ghostTo.x - g_gx) * k;
        g_gy += (g_ghostTo.y - g_gy) * k;
        MoveWindowTo(g_ghost, (int)std::lround(g_gx), (int)std::lround(g_gy));
        if (!g_drag.dragging && g_springs.empty()) DestroyHelper(g_ghost);
    }
    if (g_springs.empty() && !g_ghost && !g_drag.dragging && g_frameTimer) {
        KillTimer(nullptr, g_frameTimer);
        g_frameTimer = 0;
    }
}

static void AbortDrag() {
    if (g_drag.wd && GetCapture() == g_drag.wd->hwnd) ReleaseCapture();
    g_drag = DragState{};
    g_springs.clear();
    DestroyHelper(g_ghost);
    DestroyHelper(g_gridWin);
    if (g_frameTimer) KillTimer(nullptr, g_frameTimer);
    g_frameTimer = 0;
}

// ---------- Writing in Notes and Reminders: saving and the keyboard ----------

static std::wstring Trimmed(const std::wstring& s) {
    size_t a = 0, b = s.size();
    while (a < b && iswspace(s[a])) a++;
    while (b > a && iswspace(s[b - 1])) b--;
    return s.substr(a, b - a);
}

static std::wstring NoCR(std::wstring s) {
    s.erase(std::remove(s.begin(), s.end(), L'\r'), s.end());
    return s;
}

// To the Recycle Bin, so an emptied note can still come back
static void RecycleFile(const std::wstring& path) {
    std::wstring from = path + L'\0';
    SHFILEOPSTRUCTW op = {};
    op.wFunc = FO_DELETE;
    op.pFrom = from.c_str();
    op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
    SHFileOperationW(&op);
}

static void SaveNoteNow(bool finishing) {
    if (g_edit.mode != 1 || !g_edit.dirty) return;
    bool empty = Trimmed(g_edit.text).empty();
    if (empty) {
        if (finishing && !g_edit.path.empty()) RecycleFile(g_edit.path);  // an emptied note goes away, as on the Mac
        if (finishing) g_edit.path.clear();
        g_edit.dirty = false;
        return;
    }
    if (g_edit.path.empty()) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        wchar_t name[64];
        swprintf(name, 64, L"\\Note %04d-%02d-%02d %02d%02d%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        SHCreateDirectoryExW(nullptr, NotesDir().c_str(), nullptr);  // the first note makes the folder
        g_edit.path = NotesDir() + name;
    }
    std::wstring out;  // Windows line ends, so any editor shows the lines
    for (wchar_t c : g_edit.text) out += c == L'\n' ? std::wstring(L"\r\n") : std::wstring(1, c);
    if (WriteTextFile(g_edit.path, out)) g_edit.dirty = false;
}

// Rewrites Reminders.txt with one line changed (empty text: the line goes away) or a new line on top
static void WriteReminder(int line, const std::wstring& text) {
    std::wstring rp = RemindersPath();
    std::wstring old;
    ReadResult rr = ReadText(rp, 16 * 1024 * 1024, &old);
    if (rr == ReadResult::Failed) return;  // never write over what couldn't be read
    std::wstring t = Trimmed(text);
    for (auto& c : t)
        if (c == L'\n') c = L' ';
    if (line < 0) {
        if (t.empty()) return;
        if (rr == ReadResult::Missing) SHCreateDirectoryExW(nullptr, NotesFolder().c_str(), nullptr);  // the first reminder
        WriteTextFile(rp, L"[ ] " + t + L"\n" + old);
        return;
    }
    if (rr != ReadResult::Ok) return;
    auto lines = Split(old, L'\n');
    if (line >= (int)lines.size()) return;
    std::wstring& l = lines[line];
    bool cr = !l.empty() && l.back() == L'\r';
    if (cr) l.pop_back();
    size_t a = 0;
    while (a < l.size() && l[a] == L' ') a++;
    std::wstring body = l.substr(a);
    std::wstring mark = body.size() >= 3 && body[0] == L'[' && body[2] == L']' ? body.substr(0, 3) + L" " : L"[ ] ";
    bool drop = t.empty();
    if (!drop) l = l.substr(0, a) + mark + t + (cr ? L"\r" : L"");
    std::wstring out;
    for (size_t i = 0; i < lines.size(); i++) {
        if (drop && (int)i == line) continue;
        out += lines[i] + (i + 1 < lines.size() ? L"\n" : L"");
    }
    WriteTextFile(rp, out);
}

static void FinishEdit(bool keep = true) {
    if (!g_edit.wd) return;
    Widget* wd = g_edit.wd;
    if (wd->hwnd) {
        KillTimer(wd->hwnd, kCaretTimer);
        KillTimer(wd->hwnd, kSaveTimer);
        if (g_edit.selecting && GetCapture() == wd->hwnd) ReleaseCapture();
    }
    if (g_edit.mode == 1) {
        SaveNoteNow(true);  // what was typed is kept, even with Esc (as on the Mac); untouched, the file isn't written
    } else if (keep) {
        WriteReminder(g_edit.mode == 2 ? -1 : g_edit.line, g_edit.text);
    }
    g_edit = InlineEdit{};
    ReadNotesAndReminders();
    Render(wd);
}

static void BeginEdit(Widget* wd, int mode, const std::wstring& path, int line, const std::wstring& text) {
    if (g_edit.wd) FinishEdit(true);
    g_edit = InlineEdit{};
    g_edit.wd = wd;
    g_edit.mode = mode;
    g_edit.path = path;
    g_edit.line = line;
    g_edit.text = text;
    g_edit.caret = g_edit.anchor = text.size();
    SetTimer(wd->hwnd, kCaretTimer, 530, nullptr);
    // The widget takes the keyboard (it doesn't come to the front: it stays on the desktop)
    SetForegroundWindow(wd->hwnd);
    SetFocus(wd->hwnd);
    Render(wd);
}

static void EditChanged() {
    g_edit.caretOn = true;
    g_edit.dirty = true;
    if (g_edit.wd && g_edit.wd->hwnd) {
        SetTimer(g_edit.wd->hwnd, kCaretTimer, 530, nullptr);
        if (g_edit.mode == 1) SetTimer(g_edit.wd->hwnd, kSaveTimer, 1200, nullptr);  // saved a moment after you stop
    }
}

static bool HasSelection() { return g_edit.anchor != g_edit.caret; }

static std::wstring SelectedText() {
    size_t a = std::min(g_edit.anchor, g_edit.caret), b = std::max(g_edit.anchor, g_edit.caret);
    return b <= g_edit.text.size() ? g_edit.text.substr(a, b - a) : L"";
}

// Removes what is selected (true if there was something)
static bool EditDeleteSelection() {
    if (!HasSelection()) return false;
    size_t a = std::min(g_edit.anchor, g_edit.caret), b = std::min(std::max(g_edit.anchor, g_edit.caret), g_edit.text.size());
    g_edit.text.erase(a, b - a);
    g_edit.caret = g_edit.anchor = a;
    EditChanged();
    return true;
}

static void EditInsert(const std::wstring& s) {
    EditDeleteSelection();  // typing over a selection replaces it
    if (g_edit.text.size() + s.size() > 20000) return;
    g_edit.text.insert(g_edit.caret, s);
    g_edit.caret += s.size();
    g_edit.anchor = g_edit.caret;
    EditChanged();
}

static void EditCopy(HWND hwnd) {
    std::wstring t = SelectedText();
    if (t.empty() || !OpenClipboard(hwnd)) return;
    std::wstring out;  // Windows line ends for other apps
    for (wchar_t c : t) out += c == L'\n' ? std::wstring(L"\r\n") : std::wstring(1, c);
    EmptyClipboard();
    if (HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, (out.size() + 1) * sizeof(wchar_t))) {
        if (void* p = GlobalLock(h)) {
            memcpy(p, out.c_str(), (out.size() + 1) * sizeof(wchar_t));
            GlobalUnlock(h);
            if (!SetClipboardData(CF_UNICODETEXT, h)) GlobalFree(h);
        } else {
            GlobalFree(h);
        }
    }
    CloseClipboard();
}

// The word around a position (letters, digits and accents, or a run of anything else)
static void SelectWordAt(size_t p) {
    const std::wstring& t = g_edit.text;
    if (t.empty()) return;
    p = std::min(p, t.size() - 1);
    auto word = [](wchar_t c) { return iswalnum(c) || c == L'_' || c >= 0xC0; };
    bool w = word(t[p]);
    size_t a = p, b = p;
    while (a > 0 && word(t[a - 1]) == w && t[a - 1] != L'\n') a--;
    while (b < t.size() && word(t[b]) == w && t[b] != L'\n') b++;
    g_edit.anchor = a;
    g_edit.caret = b;
}

static size_t PrevPos(size_t p) {
    const std::wstring& t = g_edit.text;
    if (p == 0) return 0;
    if (p >= 2 && IS_LOW_SURROGATE(t[p - 1]) && IS_HIGH_SURROGATE(t[p - 2])) return p - 2;
    return p - 1;
}
static size_t NextPos(size_t p) {
    const std::wstring& t = g_edit.text;
    if (p >= t.size()) return t.size();
    if (p + 1 < t.size() && IS_HIGH_SURROGATE(t[p]) && IS_LOW_SURROGATE(t[p + 1])) return p + 2;
    return p + 1;
}

static void EditCaretAt(D2D1_POINT_2F p) {
    const D2D1_RECT_F& a = g_edit.area;
    auto lay = EditLayout(a.right - a.left);
    if (!lay) return;
    bool note = g_edit.mode == 1;
    BOOL trailing = FALSE, inside = FALSE;
    DWRITE_HIT_TEST_METRICS m = {};
    lay->HitTestPoint(p.x - a.left + (note ? 0 : g_edit.scroll), p.y - a.top + (note ? g_edit.scroll : 0), &trailing, &inside, &m);
    g_edit.caret = std::min(g_edit.text.size(), (size_t)m.textPosition + (trailing ? m.length : 0));
    g_edit.caretOn = true;
}

// Up / down a line in a note
static void EditMoveLine(int dir) {
    const D2D1_RECT_F& a = g_edit.area;
    auto lay = EditLayout(a.right - a.left);
    if (!lay) return;
    float x = 0, y = 0;
    DWRITE_HIT_TEST_METRICS m = {};
    lay->HitTestTextPosition((UINT32)g_edit.caret, FALSE, &x, &y, &m);
    float ty = y + m.height / 2 + dir * m.height;
    if (ty < 0) {
        g_edit.caret = 0;
        return;
    }
    BOOL trailing = FALSE, inside = FALSE;
    lay->HitTestPoint(x, ty, &trailing, &inside, &m);
    g_edit.caret = std::min(g_edit.text.size(), (size_t)m.textPosition + (trailing ? m.length : 0));
}

static void EditPaste(HWND hwnd) {
    if (!OpenClipboard(hwnd)) return;
    if (HANDLE h = GetClipboardData(CF_UNICODETEXT)) {
        if (auto* p = (const wchar_t*)GlobalLock(h)) {
            std::wstring s = NoCR(p);
            GlobalUnlock(h);
            if (g_edit.mode != 1)
                for (auto& c : s)
                    if (c == L'\n' || c == L'\t') c = L' ';
            EditInsert(s);
        }
    }
    CloseClipboard();
}

// The keyboard while writing in a widget: true when the message was used
static bool EditMessage(Widget* wd, UINT msg, WPARAM wp, LPARAM lp) {
    if (!IsEditing(wd)) return false;
    bool note = g_edit.mode == 1;
    std::wstring& t = g_edit.text;
    switch (msg) {
        case WM_TIMER:
            if (wp == kCaretTimer) {
                g_edit.caretOn = !g_edit.caretOn;
                Render(wd);
                return true;
            }
            if (wp == kSaveTimer) {
                KillTimer(wd->hwnd, kSaveTimer);
                SaveNoteNow(false);
                return true;
            }
            return false;
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE) FinishEdit(true);  // clicked somewhere else
            return false;
        case WM_LBUTTONDOWN: {
            // In the text, the mouse selects (the widget doesn't move): click places the caret, drag selects,
            // Shift+click selects up to there, double click a word, triple click everything
            POINT pt = {(short)LOWORD(lp), (short)HIWORD(lp)};
            D2D1_POINT_2F gp = GlassPoint(wd, pt);
            const D2D1_RECT_F& a = g_edit.area;
            if (gp.x < a.left - 6 || gp.x > a.right + 6 || gp.y < a.top - 6 || gp.y > a.bottom + 6) return false;
            ULONGLONG now = GetTickCount64();
            g_edit.clicks = now - g_edit.lastClick <= GetDoubleClickTime() ? g_edit.clicks + 1 : 1;
            g_edit.lastClick = now;
            bool shift = GetKeyState(VK_SHIFT) < 0;
            EditCaretAt(gp);
            if (g_edit.clicks == 2) {
                SelectWordAt(g_edit.caret);
            } else if (g_edit.clicks >= 3) {
                g_edit.anchor = 0;
                g_edit.caret = t.size();
            } else {
                if (!shift) g_edit.anchor = g_edit.caret;
                g_edit.selecting = true;
                SetCapture(wd->hwnd);
            }
            Render(wd);
            return true;
        }
        case WM_MOUSEMOVE:
            if (g_edit.selecting) {
                POINT pt = {(short)LOWORD(lp), (short)HIWORD(lp)};
                size_t before = g_edit.caret;
                EditCaretAt(GlassPoint(wd, pt));
                if (g_edit.caret != before) Render(wd);
                return true;
            }
            return false;
        case WM_LBUTTONUP:
            if (g_edit.selecting) {
                g_edit.selecting = false;
                if (GetCapture() == wd->hwnd) ReleaseCapture();
                return true;
            }
            return false;
        case WM_CAPTURECHANGED:
            g_edit.selecting = false;
            return false;
        case WM_SETCURSOR: {
            // The text cursor over the text being written
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(wd->hwnd, &pt);
            D2D1_POINT_2F gp = GlassPoint(wd, pt);
            const D2D1_RECT_F& a = g_edit.area;
            if (LOWORD(lp) == HTCLIENT && gp.x >= a.left - 6 && gp.x <= a.right + 6 && gp.y >= a.top - 6 && gp.y <= a.bottom + 6) {
                SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(32513)));  // IDC_IBEAM
                return true;
            }
            return false;
        }
        case WM_CHAR: {
            wchar_t ch = (wchar_t)wp;
            if (ch == 27) {  // Esc: a note is kept, a reminder being written is dropped
                FinishEdit(note);
                return true;
            }
            if (ch == L'\r') {
                if (note) {
                    EditInsert(L"\n");
                } else {
                    bool another = g_edit.mode == 2 && !Trimmed(t).empty();
                    FinishEdit(true);
                    if (another) BeginEdit(wd, 2, L"", -1, L"");  // Enter: the next reminder, as on the Mac
                    return true;
                }
            } else if (ch == 8) {
                if (!EditDeleteSelection()) {
                    size_t p = PrevPos(g_edit.caret);
                    t.erase(p, g_edit.caret - p);
                    g_edit.caret = g_edit.anchor = p;
                    EditChanged();
                }
            } else if (ch == 22) {  // Ctrl+V
                EditPaste(wd->hwnd);
            } else if (ch == 1) {  // Ctrl+A
                g_edit.anchor = 0;
                g_edit.caret = t.size();
            } else if (ch == 3) {  // Ctrl+C
                EditCopy(wd->hwnd);
            } else if (ch == 24) {  // Ctrl+X
                EditCopy(wd->hwnd);
                EditDeleteSelection();
            } else if (ch >= 32 && ch != 127) {
                EditInsert(std::wstring(1, ch));
            } else {
                return true;
            }
            Render(wd);
            return true;
        }
        case WM_KEYDOWN: {
            bool ctrl = GetKeyState(VK_CONTROL) < 0, shift = GetKeyState(VK_SHIFT) < 0;
            size_t& c = g_edit.caret;
            // Arrows with a selection and no Shift: to its start / end, as everywhere
            if (!shift && HasSelection() && (wp == VK_LEFT || wp == VK_RIGHT)) {
                c = wp == VK_LEFT ? std::min(c, g_edit.anchor) : std::max(c, g_edit.anchor);
                g_edit.anchor = c;
                g_edit.caretOn = true;
                Render(wd);
                return true;
            }
            switch (wp) {
                case VK_LEFT:
                    c = PrevPos(c);
                    if (ctrl)
                        while (c > 0 && !iswspace(t[c - 1])) c--;
                    break;
                case VK_RIGHT:
                    c = NextPos(c);
                    if (ctrl)
                        while (c < t.size() && !iswspace(t[c])) c++;
                    break;
                case VK_HOME:
                    if (ctrl || !note) c = 0;
                    else
                        while (c > 0 && t[c - 1] != L'\n') c--;
                    break;
                case VK_END:
                    if (ctrl || !note) c = t.size();
                    else
                        while (c < t.size() && t[c] != L'\n') c++;
                    break;
                case VK_UP:
                case VK_DOWN:
                    if (note) EditMoveLine(wp == VK_UP ? -1 : 1);
                    break;
                case VK_DELETE:
                    if (!EditDeleteSelection() && c < t.size()) {
                        t.erase(c, NextPos(c) - c);
                        EditChanged();
                    }
                    break;
                default:
                    return wp != VK_ESCAPE && wp != VK_RETURN ? false : true;
            }
            if (!shift || wp == VK_DELETE) g_edit.anchor = c;  // Shift + arrows / Home / End select
            g_edit.caretOn = true;
            SetTimer(wd->hwnd, kCaretTimer, 530, nullptr);
            Render(wd);
            return true;
        }
    }
    return false;
}

static bool RunActionAt(Widget* wd, D2D1_POINT_2F p) {
    for (auto it = wd->actions.rbegin(); it != wd->actions.rend(); ++it) {
        const D2D1_RECT_F& r = it->first;
        if (p.x < r.left || p.x > r.right || p.y < r.top || p.y > r.bottom) continue;
        const std::wstring a = it->second;  // a copy: drawing again (below) remakes the list
        // While a reminder is being written, a click on another one first saves it (the lines may move)
        if (IsEditing(wd) && (a.rfind(L"toggle:", 0) == 0 || a.rfind(L"editrem:", 0) == 0) &&
            (g_edit.mode == 2 || Trimmed(g_edit.text).empty())) {
            FinishEdit(true);
            return true;
        }
        if (a.rfind(L"open:", 0) == 0) {  // the user's own shortcuts
            std::wstring target = a.substr(5);
            if (!target.empty()) ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        } else if (a.rfind(L"url:", 0) == 0) {  // links from the network: web pages only
            std::wstring target = a.substr(4);
            if (IsWebUrl(target)) ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        } else if (a.rfind(L"toggle:", 0) == 0) {
            ToggleReminder(_wtoi(a.c_str() + 7));
            Render(wd);
        } else if (a == L"newnote") {
            BeginEdit(wd, 1, L"", -1, L"");
        } else if (a.rfind(L"editnote:", 0) == 0) {
            std::wstring path = a.substr(9);
            WIN32_FILE_ATTRIBUTE_DATA fa;
            bool big = GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fa) && (fa.nFileSizeHigh || fa.nFileSizeLow > 256 * 1024);
            if (big) ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);  // too long for the widget
            else {
                std::wstring text;
                if (ReadText(path, 1024 * 1024, &text) == ReadResult::Ok) BeginEdit(wd, 1, path, -1, NoCR(text));
                else ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);  // let its own app deal with it
            }
        } else if (a == L"newrem") {
            BeginEdit(wd, 2, L"", -1, L"");
        } else if (a.rfind(L"editrem:", 0) == 0) {
            int line = _wtoi(a.c_str() + 8);
            std::wstring text;
            {
                std::lock_guard<std::mutex> lock(g_dataMutex);
                for (auto& r : g_reminders)
                    if (r.line == line) text = r.text;
            }
            BeginEdit(wd, 3, L"", line, text);
        } else if (a == L"editdone") {
            FinishEdit(true);
        } else if (a == L"editcaret") {
            EditCaretAt(p);
            Render(wd);
        } else if (a.rfind(L"ha:", 0) == 0) {
            HaTap(a.substr(3));
            Render(wd);
        }
        return true;
    }
    return false;
}

// The player to the front: its window if it is open (Spotify, browsers and most players are desktop apps, whose id is
// their program's name), else the app started by its id
struct FindPlayer {
    std::wstring exe;
    HWND found = nullptr;
};
static BOOL CALLBACK FindPlayerWindow(HWND h, LPARAM lp) {
    auto* f = (FindPlayer*)lp;
    if ((!IsWindowVisible(h) && !IsIconic(h)) || GetWindow(h, GW_OWNER) || (GetWindowLongW(h, GWL_EXSTYLE) & WS_EX_TOOLWINDOW))
        return TRUE;
    wchar_t title[4];
    if (!GetWindowTextW(h, title, 4)) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return TRUE;
    wchar_t path[MAX_PATH];
    DWORD n = MAX_PATH;
    bool match = false;
    if (QueryFullProcessImageNameW(p, 0, path, &n)) {
        std::wstring full = path;
        std::wstring name = Lower(full.substr(full.find_last_of(L'\\') + 1));
        match = name == f->exe;
    }
    CloseHandle(p);
    if (!match) return TRUE;
    f->found = h;
    return FALSE;
}

static void OpenMediaApp(const std::wstring& aumid) {
    if (aumid.empty()) return;
    std::wstring exe = Lower(aumid);
    size_t bang = exe.find(L'!');  // a Store app: "SpotifyAB.SpotifyMusic_zpdnekdrzrea0!Spotify"
    if (bang != std::wstring::npos) exe = exe.substr(bang + 1);
    exe = exe.substr(exe.find_last_of(L'\\') + 1);
    if (exe.size() < 4 || exe.substr(exe.size() - 4) != L".exe") exe += L".exe";
    FindPlayer f{exe};
    EnumWindows(FindPlayerWindow, (LPARAM)&f);
    if (f.found) {
        if (IsIconic(f.found)) ShowWindow(f.found, SW_RESTORE);
        SetForegroundWindow(f.found);
        return;
    }
    std::wstring target = aumid.find(L'\\') != std::wstring::npos ? aumid : L"shell:AppsFolder\\" + aumid;
    ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static void OpenSettings(int section);

// A click (not a drag) opens what the widget shows, as on the Mac
static void OpenFor(Widget* wd) {
    PCWSTR target = nullptr;
    std::wstring url;
    switch (wd->kind) {
        case W_CLOCK: target = L"ms-clock:"; break;
        case W_WEATHER:
            if (!HasCity(g_ui)) {
                OpenSettings(SEC_WEATHER);
                return;
            }
            url = L"https://www.google.com/search?q=" + UrlEncode(T(L"previsão do tempo") + L" " + g_ui.city);
            target = url.c_str();
            break;
        case W_SYSTEM: target = L"taskmgr.exe"; break;
        case W_CALENDAR: target = L"outlookcal:"; break;
        case W_WORLD: target = L"ms-clock:"; break;
        case W_SUN:
            if (!HasCity(g_ui)) {
                OpenSettings(SEC_WEATHER);
                return;
            }
            url = L"https://www.google.com/search?q=" + UrlEncode(T(L"nascer e pôr do sol") + L" " + g_ui.city);
            target = url.c_str();
            break;
        case W_BATTERY: target = L"ms-settings:bluetooth"; break;
        case W_NETWORK: target = L"ms-settings:network-status"; break;
        case W_PHOTOS: {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            url = g_photo.path;
            target = url.empty() ? nullptr : url.c_str();
            break;
        }
        case W_NOTIF: target = L"ms-actioncenter:"; break;
        case W_NEWS:
            if (g_ui.newsFeeds.empty()) OpenSettings(SEC_NEWS);  // the stories open themselves
            return;
        case W_STOCKS:
            if (g_ui.stocks.empty()) {
                OpenSettings(SEC_STOCKS);
                return;
            }
            url = L"https://finance.yahoo.com/";
            target = url.c_str();
            break;
        case W_SCREENTIME: target = L"ms-settings:"; break;
        case W_SENSORS: target = L"taskmgr.exe"; break;
        case W_MUSIC: {
            std::wstring id;
            {
                std::lock_guard<std::mutex> lock(g_dataMutex);
                id = g_media.hasSession ? g_media.aumid : L"";
            }
            if (!id.empty()) OpenMediaApp(id);
            else target = L"mswindowsmusic:";  // nothing playing: the Windows media player
            break;
        }
        case W_HOME:
            url = HaBase();
            target = url.empty() ? nullptr : url.c_str();
            break;
        default: break;
    }
    if (target) ShellExecuteW(nullptr, L"open", target, nullptr, nullptr, SW_SHOWNORMAL);
}

// ---------------------------------------------------------------------------
// Right-click menu (glass, like the rest)
// ---------------------------------------------------------------------------

struct MenuItem {
    std::wstring label;
    int id = 0;
    bool check = false, sep = false;
};
struct MenuState {
    Widget* wd = nullptr;
    std::vector<MenuItem> items;
    std::wstring target;  // the widget it was opened on
    int hover = -1;
    ULONGLONG opened = 0;
};
static MenuState g_menu;
static const float kMenuW = 236, kMenuRow = 28, kMenuSep = 9, kMenuPad = 6;

static float MenuHeight() {
    float h = 2 * kMenuPad;
    for (auto& it : g_menu.items) h += it.sep ? kMenuSep : kMenuRow;
    return h;
}
static int MenuItemAt(D2D1_POINT_2F p) {
    float y = kMenuPad;
    for (int i = 0; i < (int)g_menu.items.size(); i++) {
        float h = g_menu.items[i].sep ? kMenuSep : kMenuRow;
        if (!g_menu.items[i].sep && p.x >= 0 && p.x <= kMenuW && p.y >= y && p.y < y + h) return i;
        y += h;
    }
    return -1;
}

static void DrawMenu(Widget* wd) {
    float y = kMenuPad;
    for (int i = 0; i < (int)g_menu.items.size(); i++) {
        const MenuItem& it = g_menu.items[i];
        if (it.sep) {
            Line(14, y + kMenuSep / 2, wd->w - 14, y + kMenuSep / 2, 1, Ink(0.14f));
            y += kMenuSep;
            continue;
        }
        D2D1_RECT_F r = D2D1::RectF(kMenuPad, y, wd->w - kMenuPad, y + kMenuRow);
        bool hot = i == g_menu.hover;
        if (hot) Fill(r, ColorOf(g_darkText ? RGB(0, 122, 255) : RGB(10, 132, 255)), 6);
        D2D1_COLOR_F ink = hot ? White(1) : Ink(0.92f);
        if (it.check) {
            auto f = Format(GlyphFamily(), 11, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER, false);
            if (auto b = Solid(ink))
                if (f) g_t->DrawText(L"", 1, f.Get(), D2D1::RectF(r.left + 4, r.top, r.left + 24, r.bottom), b.Get());
        }
        Text(it.label, r.left + 28, r.top + 5, r.right - r.left - 36, 20, 13.5f, DWRITE_FONT_WEIGHT_NORMAL, ink);
        y += kMenuRow;
    }
}

static void CloseMenu() {
    if (!g_menu.wd) return;
    Widget* wd = g_menu.wd;
    g_menu.wd = nullptr;
    SetWindowLongPtrW(wd->hwnd, GWLP_USERDATA, 0);
    ReleaseSurface(wd);
    DestroyWindow(wd->hwnd);
    delete wd;
}

static void OpenSettings(int section);

static void ShowMenu(Widget* target, POINT cursor) {
    CloseMenu();
    g_menu = MenuState{};
    g_menu.target = target->key;
    auto add = [&](std::wstring label, int id, bool check = false) {
        MenuItem it;
        it.label = label;
        it.id = id;
        it.check = check;
        g_menu.items.push_back(it);
    };
    auto sep = [&]() {
        MenuItem it;
        it.sep = true;
        g_menu.items.push_back(it);
    };
    add(L"Pequeno", 20, target->size == 0);
    add(L"Médio", 21, target->size == 1);
    add(L"Grande", 22, target->size == 2);
    if (AllowsExtraLarge(target->kind)) add(L"Extra-grande", 23, target->size == 3);
    sep();
    if (target->kind == W_WEATHER) add(L"Editar Local…", 3);
    if (target->kind == W_CLOCK) {
        add(L"Ponteiros e Digital", 11, g_ui.clockStyle == 0);
        add(L"Ponteiros", 12, g_ui.clockStyle == 1);
        add(L"Digital", 13, g_ui.clockStyle == 2);
        sep();
    }
    if (target->kind == W_NOTES) add(L"Abrir Pasta das Notas", 5);
    if (target->kind == W_REMINDERS) add(L"Abrir no Bloco de Notas", 5);
    if (target->kind == W_CALENDAR) add(L"Importar Calendário (.ics)…", 6);
    if (target->kind == W_NEWS) add(L"Atualizar Notícias", 7);
    if (target->kind == W_HOME) add(L"Escolher Favoritos…", 9);
    if (target->kind == W_SHORTCUTS) add(L"Escolher Atalhos…", 10);
    add(L"Remover Widget", 1);
    sep();
    for (int i = 0; i < kWidgetDefCount; i++) add(kWidgetDefs[i].name, 100 + i, CfgNow(kWidgetDefs[i].showKey) == L"1");
    add(L"Organizar Widgets", 2);
    sep();
    add(L"Ajustes do MacOS Widgets…", 4);

    Widget* wd = new Widget();
    wd->kind = W_MENU;
    wd->key = L"menu";
    wd->w = kMenuW;
    wd->h = MenuHeight();
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST), &mi);
    wd->scale = GetDpiForSystem() / 96.0f;
    int pw, ph;
    WindowSize(wd, &pw, &ph);
    int m = (int)std::lround(kMargin * wd->scale);
    // Kept on the screen; one taller (or wider) than the screen starts at its top (or left)
    int left = (int)mi.rcWork.left - m, top = (int)mi.rcWork.top - m;
    int x = std::clamp((int)cursor.x - m, left, std::max(left, (int)mi.rcWork.right - pw + m));
    int y = std::clamp((int)cursor.y - m, top, std::max(top, (int)mi.rcWork.bottom - ph + m));
    wd->hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kClassName, L"MacOS Widgets", WS_POPUP, x, y, pw,
                               ph, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!wd->hwnd) {
        delete wd;
        return;
    }
    SetWindowDisplayAffinity(wd->hwnd, g_ui.liveGlass ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
    g_menu.wd = wd;
    g_menu.opened = GetTickCount64();
    Capture(wd);
    Render(wd);
    SetWindowLongPtrW(wd->hwnd, GWLP_USERDATA, (LONG_PTR)wd);
    ShowWindow(wd->hwnd, SW_SHOW);
    SetForegroundWindow(wd->hwnd);
}

// Organizar: every widget that is on gets a place, packed from the top-left corner down each column, the biggest first
// (so small ones can't leave holes a big one won't fit in). One that has no room left at its size gets smaller
static void OrganizeWidgets() {
    Grid g = GridNow();
    struct Item {
        const WidgetDef* d;
        int size;
    };
    std::vector<Item> items;
    for (const WidgetDef& d : kWidgetDefs) {
        if (CfgNow(d.showKey) != L"1") continue;
        int size = std::clamp(Wh_GetIntValue((std::wstring(L"size_") + d.key).c_str(), d.size), 0, 3);
        if (size == 3 && !AllowsExtraLarge(d.kind)) size = 2;
        items.push_back({&d, size});
    }
    auto area = [](int s) { return SpanC(s) * SpanR(s); };
    std::stable_sort(items.begin(), items.end(), [&](const Item& a, const Item& b) { return area(a.size) > area(b.size); });
    std::vector<char> used((size_t)g.cols * g.rows, 0);
    auto fits = [&](int s, int c, int r) {
        if (c + SpanC(s) > g.cols || r + SpanR(s) > g.rows) return false;
        for (int y = r; y < r + SpanR(s); y++)
            for (int x = c; x < c + SpanC(s); x++)
                if (used[(size_t)y * g.cols + x]) return false;
        return true;
    };
    for (const Item& it : items) {
        std::wstring k = it.d->key;
        bool placed = false;
        for (int s = it.size; s >= 0 && !placed; s--)
            for (int c = 0; c < g.cols && !placed; c++)
                for (int r = 0; r < g.rows && !placed; r++) {
                    if (!fits(s, c, r)) continue;
                    for (int y = r; y < r + SpanR(s); y++)
                        for (int x = c; x < c + SpanC(s); x++) used[(size_t)y * g.cols + x] = 1;
                    Wh_SetIntValue((L"cell_" + k).c_str(), c * 1000 + r);
                    if (s != it.size) Wh_SetIntValue((L"size_" + k).c_str(), s);
                    placed = true;
                }
        if (!placed) Wh_SetIntValue((L"cell_" + k).c_str(), -1);
    }
    RebuildWidgets();
}

static void RunMenu(int id, const std::wstring& key) {
    if (id >= 20 && id <= 23) {
        Wh_SetIntValue((L"size_" + key).c_str(), id - 20);  // the same cell if it fits, else the nearest free one
        RebuildWidgets();
    } else if (id == 1) {
        PCWSTR k = ShowKeyOf(key);
        CfgSet(k, L"0");
        CfgApply(k, true);
    } else if (id >= 100 && id < 100 + kWidgetDefCount) {
        PCWSTR k = kWidgetDefs[id - 100].showKey;
        CfgSet(k, CfgNow(k) == L"1" ? L"0" : L"1");
        CfgApply(k, true);
    } else if (id == 2) {
        OrganizeWidgets();
    } else if (id == 3) {
        OpenSettings(SEC_WEATHER);
    } else if (id == 4) {
        OpenSettings(-1);
    } else if (id == 5) {
        std::wstring target = key == L"reminders" ? RemindersPath() : NotesDir();
        if (GetFileAttributesW(target.c_str()) == INVALID_FILE_ATTRIBUTES) {
            target = NotesFolder();  // nothing saved yet: the folder they'll go in (asked for, so it's made)
            SHCreateDirectoryExW(nullptr, target.c_str(), nullptr);
        }
        ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    } else if (id == 6) {
        ImportIcs(nullptr);
    } else if (id >= 11 && id <= 13) {
        static const PCWSTR styles[] = {L"both", L"analog", L"digital"};
        CfgSet(L"clockStyle", styles[id - 11]);
        CfgApply(L"clockStyle", true);
    } else if (id == 9 || id == 10) {
        OpenPicker(id == 9 ? 0 : 1);
    } else if (id == 7) {
        for (Widget* w : g_widgets)
            if (w->kind == W_NEWS) w->newsFirst = 0;  // back to the top stories
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            g_newsRefresh = true;
        }
        g_cv.notify_all();
    }
}

// ---------------------------------------------------------------------------
// Window procedure
// ---------------------------------------------------------------------------

static LRESULT WidgetProcInner(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

// Nothing thrown in a widget may reach Explorer (an exception crossing Windows' code would take the taskbar down)
static LRESULT CALLBACK WidgetProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        return WidgetProcInner(hwnd, msg, wp, lp);
    } catch (...) {
        Wh_Log(L"Unexpected error in message 0x%04X", msg);
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
}

static LRESULT WidgetProcInner(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Widget* wd = FromHwnd(hwnd);
    if (wd && wd->kind == W_PICKER) {
        LRESULT result = 0;
        if (PickerMessage(wd, msg, wp, lp, &result)) return result;
    }
    const bool settings = wd && wd->kind == W_SETTINGS;
    if (settings) {
        LRESULT result = 0;
        if (CfgMessage(wd, msg, wp, lp, &result)) return result;
    }
    const bool menu = wd && wd->kind == W_MENU;
    const bool widget = IsDesktopWidget(wd);
    if (EditMessage(wd, msg, wp, lp)) return 0;
    // A click on another widget ends the writing (those clicks don't move the keyboard focus)
    if ((msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN) && g_edit.wd && g_edit.wd != wd && !menu) FinishEdit(true);
    switch (msg) {
        case WM_NCHITTEST: {
            if (!wd) return HTCLIENT;
            if (wd->kind == W_GHOST || wd->kind == W_GRID) return HTTRANSPARENT;
            POINT pt = {(short)LOWORD(lp), (short)HIWORD(lp)};
            ScreenToClient(hwnd, &pt);
            return InGlass(wd, GlassPoint(wd, pt)) ? HTCLIENT : HTTRANSPARENT;
        }
        case WM_MOUSEACTIVATE:
            return settings || menu ? MA_ACTIVATE : MA_NOACTIVATE;
        case WM_RBUTTONDOWN:
            return 0;
        case WM_RBUTTONUP:
            if (widget) {
                POINT c;
                GetCursorPos(&c);
                ShowMenu(wd, c);
            }
            return 0;
        case WM_LBUTTONDOWN:
            if (menu) return 0;
            if (widget) {
                POINT pt = {(short)LOWORD(lp), (short)HIWORD(lp)};
                g_drag = DragState{};
                g_drag.wd = wd;
                g_drag.button = wd->kind == W_MUSIC ? HitMusicButton(wd, GlassPoint(wd, pt)) : -1;
                GetCursorPos(&g_drag.start);
                RECT r;
                GetWindowRect(hwnd, &r);
                g_drag.winStart = {r.left, r.top};
                SetCapture(hwnd);
                wd->pressBtn = g_drag.button;
                if (g_drag.button >= 0) Render(wd);  // shows it pressed
            }
            return 0;
        case WM_MOUSEMOVE:
            if (menu) {
                POINT pt = {(short)LOWORD(lp), (short)HIWORD(lp)};
                int h = MenuItemAt(GlassPoint(wd, pt));
                if (h != g_menu.hover) {
                    g_menu.hover = h;
                    Render(wd);
                }
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                return 0;
            }
            if (widget && wd->kind == W_MUSIC && !g_drag.dragging) {
                POINT pt = {(short)LOWORD(lp), (short)HIWORD(lp)};
                int h = HitMusicButton(wd, GlassPoint(wd, pt));
                if (h != wd->hoverBtn) {
                    wd->hoverBtn = h;
                    Render(wd);
                }
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
            }
            if (widget && g_drag.wd == wd && GetCapture() == hwnd && g_drag.button < 0 && !g_ui.lockPosition) {
                POINT c;
                GetCursorPos(&c);
                int dx = c.x - g_drag.start.x, dy = c.y - g_drag.start.y;
                int limit = std::max(8, GetSystemMetrics(SM_CXDRAG) * 2);  // a shaky click isn't a drag
                if (!g_drag.dragging && (abs(dx) > limit || abs(dy) > limit)) BeginDrag(wd);
                if (g_drag.dragging) {
                    POINT p = {g_drag.winStart.x + dx, g_drag.winStart.y + dy};
                    MoveWindowTo(wd, p.x, p.y);
                    Grid g = GridNow();
                    if (NearestFree(g, wd, p, &g_drag.tc, &g_drag.tr)) g_ghostTo = WindowAt(g, g_drag.tc, g_drag.tr);
                }
            }
            return 0;
        case WM_MOUSELEAVE:
            if (menu && g_menu.hover >= 0) {
                g_menu.hover = -1;
                Render(wd);
            }
            if (widget && wd->hoverBtn >= 0) {
                wd->hoverBtn = -1;
                Render(wd);
            }
            return 0;
        case WM_LBUTTONUP:
            if (menu) {
                POINT pt = {(short)LOWORD(lp), (short)HIWORD(lp)};
                int i = MenuItemAt(GlassPoint(wd, pt));
                if (i >= 0) {
                    int id = g_menu.items[i].id;
                    std::wstring key = g_menu.target;
                    CloseMenu();
                    RunMenu(id, key);
                }
                return 0;
            }
            if (widget && g_drag.wd == wd) {
                bool dragged = g_drag.dragging;
                int button = g_drag.button;
                POINT pt = {(short)LOWORD(lp), (short)HIWORD(lp)};
                g_drag.wd = nullptr;  // so WM_CAPTURECHANGED (sent by ReleaseCapture) leaves it to us
                wd->pressBtn = -1;
                if (GetCapture() == hwnd) ReleaseCapture();
                if (dragged) Drop(wd);
                else if (button >= 0) {
                    if (HitMusicButton(wd, GlassPoint(wd, pt)) == button) {
                        {
                            std::lock_guard<std::mutex> lock(g_dataMutex);
                            g_commands.push_back(button == 0 ? CMD_PREV : button == 1 ? CMD_TOGGLE : CMD_NEXT);
                        }
                        g_cv.notify_all();
                    }
                    Render(wd);  // no longer pressed
                } else if (RunActionAt(wd, GlassPoint(wd, pt))) {
                    // a clickable part of the widget (a note, a reminder, a shortcut, a headline...)
                } else if (IsEditing(wd)) {
                    // writing: a click beside the text doesn't open anything
                } else if (wd->kind == W_NOTIF && wd->size > 0) {
                    D2D1_POINT_2F gp = GlassPoint(wd, pt);
                    D2D1_RECT_F c = NotifClearRect(wd);
                    if (gp.x >= c.left && gp.x <= c.right && gp.y >= c.top && gp.y <= c.bottom) {
                        std::lock_guard<std::mutex> lock(g_dataMutex);
                        long long newest = 0;
                        for (auto& n : g_notifs) newest = std::max(newest, n.arrival);
                        if (newest > 0) {
                            g_notifCleared = newest;
                            Wh_SetStringValue(L"notifCleared", std::to_wstring(newest).c_str());
                            g_notifs.clear();
                        }
                    } else {
                        OpenFor(wd);
                    }
                    Render(wd);
                } else {
                    OpenFor(wd);
                }
            }
            return 0;
        case WM_MOUSEWHEEL:
            // News: the wheel goes through the stories, a page at a time
            if (widget && wd->kind == W_NEWS) {
                int notches = GET_WHEEL_DELTA_WPARAM(wp) / WHEEL_DELTA;
                if (!notches) notches = GET_WHEEL_DELTA_WPARAM(wp) > 0 ? 1 : -1;
                int total;
                {
                    std::lock_guard<std::mutex> lock(g_dataMutex);
                    total = (int)g_news.size();
                }
                int step = std::max(1, wd->newsStep), last = total > 0 ? ((total - 1) / step) * step : 0;
                int first = wd->newsFirst - notches * step;  // down (negative) shows the next ones
                if (first > last) first = 0;                 // past the end: back to the top stories
                else if (first < 0) first = wd->newsFirst == 0 ? last : 0;
                wd->newsFirst = first;
                wd->newsWheel = GetTickCount64();
                Render(wd);
            }
            return 0;
        case WM_CAPTURECHANGED:
            if (widget && wd->pressBtn >= 0) {
                wd->pressBtn = -1;
                Render(wd);
            }
            if (widget && g_drag.wd == wd) {  // the capture was taken away mid-drag: land where it was heading
                if (g_drag.dragging) Drop(wd);
                g_drag.wd = nullptr;
            }
            return 0;
        case WM_KEYDOWN:
            if (menu && wp == VK_ESCAPE) PostMessageW(hwnd, WM_APP_MENUCLOSE, 0, 0);
            return 0;
        case WM_ACTIVATE:
            if (menu && LOWORD(wp) == WA_INACTIVE) PostMessageW(hwnd, WM_APP_MENUCLOSE, 0, 0);
            break;
        case WM_APP_MENUCLOSE:
            if (menu) CloseMenu();
            return 0;
        case WM_MOVE:
            // The glass shows what is under its new place. While a widget moves (dragged, or springing into its
            // cell) it follows: from the live glass thread, or taken here at most ~30 times a second. Ajustes and the
            // picker, dragged by their title, the same (and once more when they stop: WM_EXITSIZEMOVE)
            if (wd && wd->rt && wd->kind != W_GHOST && wd->kind != W_GRID) {
                bool moving = (g_drag.dragging && g_drag.wd == wd) || (GetCapture() == hwnd && !IsDesktopWidget(wd));
                for (auto& sp : g_springs)
                    if (sp.wd == wd) moving = true;
                if (!moving) {
                    Capture(wd);
                    Render(wd);
                } else if (g_ui.liveGlass && g_ddaOn && IsDesktopWidget(wd)) {
                    PublishGlassTargets();  // the glass thread reads the new place from the screen it already has
                } else if (GetTickCount64() - wd->capTick >= 33) {
                    // Without the glass thread: the picture taken here, at most 30 times a second (with the live glass
                    // off it comes from the desktop's picture, which is cheap)
                    Capture(wd);
                    Render(wd);
                }
            }
            return 0;
        case WM_EXITSIZEMOVE:
            if (wd && wd->rt) {
                Capture(wd);
                Render(wd);
            }
            return 0;
        case WM_WINDOWPOSCHANGING: {
            // Only we move widgets in the z-order: they stay right above the desktop
            auto* pos = (WINDOWPOS*)lp;
            if (wd && !settings && !menu && !g_placing) pos->flags |= SWP_NOZORDER;
            return 0;
        }
        case WM_DPICHANGED:
            if (wd && (settings || menu || wd->kind == W_PICKER)) {
                wd->scale = HIWORD(wp) / 96.0f;
                auto* r = (RECT*)lp;
                int pw, ph;
                WindowSize(wd, &pw, &ph);
                SetWindowPos(hwnd, nullptr, r->left, r->top, pw, ph, SWP_NOZORDER | SWP_NOACTIVATE);
                Capture(wd);
                Render(wd);
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void CreateWidget(WidgetKind kind, PCWSTR key, int defSize, int defC, int defR) {
    Widget* wd = new Widget();
    wd->kind = kind;
    wd->key = key;
    std::wstring k = key;
    wd->size = std::clamp(Wh_GetIntValue((L"size_" + k).c_str(), defSize), 0, 3);
    if (wd->size == 3 && !AllowsExtraLarge(kind)) wd->size = 2;
    wd->w = SizeW(wd->size);
    wd->h = SizeH(wd->size);
    Grid g = GridNow();
    int cell = Wh_GetIntValue((L"cell_" + k).c_str(), -1);
    int c = cell >= 0 ? cell / 1000 : defC, r = cell >= 0 ? cell % 1000 : defR;
    if (!Fits(g, wd->size, c, r) || !CellFree(wd, wd->size, c, r)) {
        if (!Fits(g, wd->size, c, r)) {
            c = std::clamp(c, 0, std::max(0, g.cols - SpanC(wd->size)));
            r = std::clamp(r, 0, std::max(0, g.rows - SpanR(wd->size)));
        }
        if (!CellFree(wd, wd->size, c, r) && !NearestFree(g, wd, WindowAt(g, c, r), &c, &r)) {
            // No room at its size: smaller, rather than gone
            bool ok = false;
            while (!ok && wd->size > 0) {
                wd->size--;
                wd->w = SizeW(wd->size);
                wd->h = SizeH(wd->size);
                ok = NearestFree(g, wd, WindowAt(g, c, r), &c, &r);
            }
            if (!ok) {
                delete wd;  // the screen is full
                return;
            }
        }
    }
    wd->col = c;
    wd->row = r;
    wd->scale = g.sc;
    int pw, ph;
    WindowSize(wd, &pw, &ph);
    POINT at = WindowAt(g, c, r);
    wd->hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kClassName, L"MacOS Widgets", WS_POPUP, at.x,
                               at.y, pw, ph, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!wd->hwnd) {
        delete wd;
        return;
    }
    float sc = GetDpiForWindow(wd->hwnd) / 96.0f;
    if (sc != wd->scale) {
        wd->scale = sc;
        WindowSize(wd, &pw, &ph);
        MoveWindow(wd->hwnd, at.x, at.y, pw, ph, FALSE);
    }
    SetWindowDisplayAffinity(wd->hwnd, g_ui.liveGlass ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
    g_widgets.push_back(wd);
    Capture(wd);  // still hidden: the picture can't contain the widget itself
    Render(wd);
    SetWindowLongPtrW(wd->hwnd, GWLP_USERDATA, (LONG_PTR)wd);
    ShowWindow(wd->hwnd, SW_SHOWNOACTIVATE);
}

static bool Animated() { return g_ui.chromeBorder || BacklightOn(); }

static void UpdateAnimTimer() {
    bool want = Animated();  // the moving light of the backlight and chrome
    if (want && !g_animTimer) g_animTimer = SetTimer(nullptr, 0, 50, nullptr);
    if (!want && g_animTimer) {
        KillTimer(nullptr, g_animTimer);
        g_animTimer = 0;
    }
}

static RECT g_lastWork = {};

static void CreateWidgets() {
    g_ui = GetSettings();
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &g_lastWork, 0);
    for (const WidgetDef& d : kWidgetDefs)
        if (CfgNow(d.showKey) == L"1") CreateWidget(d.kind, d.key, d.size, d.col, d.row);
    PlaceAtDesktop();
    UpdateAnimTimer();
}

// ---------- Backup: one text file with every setting, each theme's own values and where each widget is ----------

static std::wstring PickBackupFile(HWND owner, bool save) {
    ComPtr<IFileDialog> dlg;
    if (FAILED(CoCreateInstance(save ? CLSID_FileSaveDialog : CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                                save ? __uuidof(IFileSaveDialog) : __uuidof(IFileOpenDialog), (void**)dlg.GetAddressOf())))
        return L"";
    std::wstring kind = T(L"Backup do MacOS Widgets") + L" (*.mwbackup)", all = T(L"Todos os arquivos");
    COMDLG_FILTERSPEC types[] = {{kind.c_str(), L"*.mwbackup"}, {all.c_str(), L"*.*"}};
    dlg->SetFileTypes(2, types);
    dlg->SetTitle(T(save ? L"Fazer backup" : L"Restaurar backup").c_str());
    if (save) {
        dlg->SetDefaultExtension(L"mwbackup");
        SYSTEMTIME st;
        GetLocalTime(&st);
        dlg->SetFileName(Fmt(L"MacOS Widgets %04d-%02d-%02d", st.wYear, st.wMonth, st.wDay).c_str());
    }
    DWORD opts = 0;
    dlg->GetOptions(&opts);
    dlg->SetOptions(opts | FOS_FORCEFILESYSTEM | (save ? FOS_OVERWRITEPROMPT : FOS_FILEMUSTEXIST));
    ComPtr<IShellItem> item;
    PWSTR path = nullptr;
    if (FAILED(dlg->Show(owner)) || FAILED(dlg->GetResult(&item)) || FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) return L"";
    std::wstring p = path;
    CoTaskMemFree(path);
    return p;
}

static void BackupSave(HWND owner) {
    std::wstring path = PickBackupFile(owner, true);
    if (path.empty()) return;
    std::wstring out = L"MacOS Widgets backup 1\n[settings]\n";
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        if (!g_cfgLoaded) CfgLoadStored();
        for (const CfgItem& it : kCfgItems)
            if (it.key[0] != L'#') out += std::wstring(it.key) + L"\t" + CleanValue(CfgShared(it)) + L"\n";
        out += L"[themes]\n";
        for (const auto& t : g_themeVals)
            for (const auto& p : t.second) out += t.first + L"\t" + p.first + L"\t" + CleanValue(p.second.value) + L"\n";
    }
    out += L"[widgets]\n";
    for (const WidgetDef& d : kWidgetDefs) {
        std::wstring k = d.key;
        out += k + L"\t" + std::to_wstring(Wh_GetIntValue((L"cell_" + k).c_str(), -1)) + L"\t" +
               std::to_wstring(Wh_GetIntValue((L"size_" + k).c_str(), d.size)) + L"\n";
    }
    if (!WriteTextFile(path, out)) Wh_Log(L"Backup: could not write %ls", path.c_str());
}

static void BackupLoad(HWND owner) {
    std::wstring path = PickBackupFile(owner, false);
    if (path.empty()) return;
    auto lines = Split(NoCR(ReadTextFile(path, 4 * 1024 * 1024)), L'\n');
    if (lines.empty() || lines[0].rfind(L"MacOS Widgets backup", 0) != 0) {
        Wh_Log(L"Backup: %ls is not a MacOS Widgets backup", path.c_str());
        return;
    }
    std::wstring part;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        if (!g_cfgLoaded) CfgLoadStored();
        g_cfgOver.clear();
        g_themeVals.clear();
        for (size_t i = 1; i < lines.size(); i++) {
            const std::wstring& l = lines[i];
            if (!l.empty() && l[0] == L'[') {
                part = l;
                continue;
            }
            auto f = Split(l, L'\t');
            if (part == L"[settings]" && f.size() == 2) {
                const CfgItem* it = CfgFind(f[0].c_str());
                if (it && f[0][0] != L'#') g_cfgOver.push_back({f[0], {f[1], CfgTab(*it)}});
            } else if (part == L"[themes]" && f.size() == 3) {
                const CfgItem* it = CfgFind(f[1].c_str());
                if (!it || !it->themed) continue;
                auto* vals = FindIn(g_themeVals, f[0]);
                if (!vals) {
                    g_themeVals.push_back({f[0], {}});
                    vals = &g_themeVals.back().second;
                }
                vals->push_back({f[1], {f[2], CfgTab(*it)}});
            }
        }
        CfgSaveStored();
    }
    part.clear();
    for (size_t i = 1; i < lines.size(); i++) {  // where each widget was, and its size
        const std::wstring& l = lines[i];
        if (!l.empty() && l[0] == L'[') {
            part = l;
            continue;
        }
        auto f = Split(l, L'\t');
        if (part != L"[widgets]" || f.size() != 3) continue;
        for (const WidgetDef& d : kWidgetDefs)
            if (f[0] == d.key) {
                Wh_SetIntValue((L"cell_" + f[0]).c_str(), _wtoi(f[1].c_str()));
                Wh_SetIntValue((L"size_" + f[0]).c_str(), std::clamp(_wtoi(f[2].c_str()), 0, 3));
            }
    }
    LoadSettings();
    RebuildWidgets();
    if (g_cfg.wd) Render(g_cfg.wd);
}

static void OpenSettings(int section) {
    if (section >= 0) {
        g_cfg.section = section;
        g_cfg.search.clear();
        g_cfg.scroll = 0;
    }
    if (g_cfg.wd) {
        Render(g_cfg.wd);
        SetForegroundWindow(g_cfg.wd->hwnd);
        return;
    }
    Widget* wd = new Widget();
    wd->kind = W_SETTINGS;
    wd->key = L"settings";
    wd->w = kCfgW;
    wd->h = kCfgH;
    POINT cur;
    GetCursorPos(&cur);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(cur, MONITOR_DEFAULTTOPRIMARY), &mi);
    wd->scale = GetDpiForSystem() / 96.0f;
    int pw, ph;
    WindowSize(wd, &pw, &ph);
    auto center = [&](int w, int h, int* x, int* y) {
        *x = (mi.rcWork.left + mi.rcWork.right - w) / 2;
        *y = (mi.rcWork.top + mi.rcWork.bottom - h) / 2;
    };
    int x, y;
    center(pw, ph, &x, &y);
    wd->hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW, kClassName, L"MacOS Widgets - Ajustes", WS_POPUP, x, y, pw, ph,
                               nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!wd->hwnd) {
        delete wd;
        return;
    }
    wd->scale = GetDpiForWindow(wd->hwnd) / 96.0f;
    WindowSize(wd, &pw, &ph);
    center(pw, ph, &x, &y);
    SetWindowPos(wd->hwnd, HWND_TOP, x, y, pw, ph, SWP_NOACTIVATE);
    SetWindowDisplayAffinity(wd->hwnd, g_ui.liveGlass ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
    g_cfg.wd = wd;
    Capture(wd);
    Render(wd);
    SetWindowLongPtrW(wd->hwnd, GWLP_USERDATA, (LONG_PTR)wd);
    ShowWindow(wd->hwnd, SW_SHOW);
    SetForegroundWindow(wd->hwnd);
}

// ---------------------------------------------------------------------------
// Picker: choose from a list what a widget shows (Casa's favorites, the Atalhos), with search, as the Mac's widget
// editing. It writes the same settings you could type in Ajustes
// ---------------------------------------------------------------------------

struct PickItem {
    std::wstring id, label, sub;
    bool on = false;
    int order = -1;  // when it was chosen (the widget shows them in this order)
};
struct PickerUi {
    Widget* wd = nullptr;
    int kind = 0;  // 0 Casa, 1 Atalhos
    std::vector<PickItem> items;
    std::vector<int> shown;  // after the search
    std::wstring search, empty;
    float scroll = 0;
    int hover = -1, nextOrder = 0;
    D2D1_RECT_F list{}, ok{}, cancel{};
};
static PickerUi g_pick;
constexpr float kPickW = 380, kPickH = 540, kPickRow = 46;

constexpr GUID kFolderApps = {0x1e87508d, 0x89c2, 0x42f0, {0x8a, 0x7e, 0x64, 0x5a, 0x0f, 0x50, 0xca, 0x58}};  // FOLDERID_AppsFolder
constexpr GUID kBhidEnumItems = {0x94f60519, 0x2850, 0x4924, {0xaa, 0x5a, 0xd1, 0x5e, 0x84, 0x86, 0x80, 0x39}};

// Every app of the Start menu, Store apps included ("shell:AppsFolder\<its id>" opens it)
static void ListApps(std::vector<PickItem>& out) {
    ComPtr<IShellItem> folder;
    ComPtr<IEnumShellItems> en;
    if (FAILED(SHGetKnownFolderItem(kFolderApps, KF_FLAG_DEFAULT, nullptr, __uuidof(IShellItem), (void**)folder.GetAddressOf())) ||
        FAILED(folder->BindToHandler(nullptr, kBhidEnumItems, __uuidof(IEnumShellItems), (void**)en.GetAddressOf())))
        return;
    std::vector<PickItem> apps;
    ComPtr<IShellItem> it;
    while (apps.size() < 800 && en->Next(1, it.ReleaseAndGetAddressOf(), nullptr) == S_OK) {
        PWSTR name = nullptr, parse = nullptr;
        if (SUCCEEDED(it->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && SUCCEEDED(it->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &parse))) {
            std::wstring n = name, low = Lower(n), p = parse;
            bool junk = low.find(L"uninstall") != std::wstring::npos || low.find(L"desinstalar") != std::wstring::npos ||
                        Lower(p).rfind(L"http", 0) == 0;
            if (!junk && !n.empty()) apps.push_back({L"shell:AppsFolder\\" + p, n, L"App"});
        }
        CoTaskMemFree(name);
        CoTaskMemFree(parse);
    }
    std::sort(apps.begin(), apps.end(), [](const PickItem& a, const PickItem& b) { return _wcsicmp(a.label.c_str(), b.label.c_str()) < 0; });
    out.insert(out.end(), apps.begin(), apps.end());
}

static std::wstring HaKindName(const std::wstring& domain) {
    if (domain == L"light") return L"Luz";
    if (domain == L"switch") return L"Tomada / interruptor";
    if (domain == L"fan") return L"Ventilador";
    if (domain == L"cover") return L"Cortina / persiana";
    if (domain == L"climate") return L"Ar-condicionado / termostato";
    if (domain == L"media_player") return L"Mídia";
    if (domain == L"lock") return L"Fechadura";
    if (domain == L"scene") return L"Cena";
    if (domain == L"script") return L"Script";
    if (domain == L"input_boolean") return L"Chave";
    if (domain == L"sensor") return L"Sensor";
    if (domain == L"binary_sensor") return L"Sensor";
    return domain;
}

static void PickerFilter() {
    g_pick.shown.clear();
    std::wstring q = Lower(Trimmed(g_pick.search));
    for (int i = 0; i < (int)g_pick.items.size(); i++) {
        const PickItem& p = g_pick.items[i];
        if (q.empty() || Lower(p.label).find(q) != std::wstring::npos || Lower(p.sub).find(q) != std::wstring::npos) g_pick.shown.push_back(i);
    }
}

static void ClosePicker() {
    if (!g_pick.wd) return;
    Widget* wd = g_pick.wd;
    g_pick = PickerUi{};
    SetWindowLongPtrW(wd->hwnd, GWLP_USERDATA, 0);
    ReleaseSurface(wd);
    DestroyWindow(wd->hwnd);
    delete wd;
}

// OK: the chosen ones, in the order they were chosen, go to the widget's setting
static void PickerCommit() {
    std::vector<const PickItem*> on;
    for (auto& p : g_pick.items)
        if (p.on) on.push_back(&p);
    std::stable_sort(on.begin(), on.end(), [](const PickItem* a, const PickItem* b) { return a->order < b->order; });
    std::wstring v;
    if (g_pick.kind == 0) {
        for (auto* p : on) v += (v.empty() ? L"" : L", ") + p->id;
        CfgSet(L"haEntities", v);
        CfgApply(L"haEntities", true);
    } else {
        for (auto* p : on) {
            std::wstring name;
            for (wchar_t c : p->label)
                if (c != L'=' && c != L';') name += c;
            v += (v.empty() ? L"" : L"; ") + name + L"=" + p->id;
        }
        CfgSet(L"shortcuts", v);
        CfgApply(L"shortcuts", true);
    }
}

static void DrawPicker(Widget* wd) {
    PickerUi& g = g_pick;
    float W = wd->w, H = wd->h;
    D2D1_COLOR_F accent = ColorOf(g_ui.accent);
    int count = 0;
    for (auto& p : g.items) count += p.on;
    Text(g.kind == 0 ? L"Favoritos da Casa" : L"Atalhos", 20, 16, W - 40, 26, 18, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
    std::wstring sub = count ? Fmt(TF(count == 1 ? L"%d escolhido · na ordem em que você marcar" : L"%d escolhidos · na ordem em que você marcar"), count)
                             : g.kind == 0 ? L"Nenhum marcado: o widget escolhe sozinho" : L"Marque o que quer no widget";
    Text(sub, 20, 42, W - 40, 16, 11, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.55f));
    // Search (typing anywhere writes here)
    FillRound(16, 66, W - 32, 32, 10, Ink(0.08f));
    Text(L"", 26, 73, 20, 18, 13, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.5f));
    Text(g.search.empty() ? L"Pesquisar" : g.search + L"|", 48, 72, W - 80, 20, 13, DWRITE_FONT_WEIGHT_NORMAL,
         Ink(g.search.empty() ? 0.4f : 0.95f));
    // The list
    g.list = D2D1::RectF(10, 108, W - 10, H - 66);
    float listH = g.list.bottom - g.list.top, contentH = g.shown.size() * kPickRow;
    g.scroll = std::clamp(g.scroll, 0.0f, std::max(0.0f, contentH - listH));
    if (g.items.empty() || g.shown.empty()) {
        TextWrap(g.items.empty() ? g.empty : L"Nada com esse nome", 24, g.list.top + 12, W - 48, 80, 12.5f, DWRITE_FONT_WEIGHT_NORMAL,
                 Ink(0.6f));
    }
    g_t->PushAxisAlignedClip(g.list, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    int first = (int)(g.scroll / kPickRow);
    for (int i = first; i < (int)g.shown.size(); i++) {
        float y = g.list.top + i * kPickRow - g.scroll;
        if (y > g.list.bottom) break;
        const PickItem& p = g.items[g.shown[i]];
        if (i == g.hover) FillRound(g.list.left, y + 2, g.list.right - g.list.left, kPickRow - 4, 10, Ink(0.07f));
        float cx = 32, cy = y + kPickRow / 2;
        if (p.on) {
            FillEllipse(cx, cy, 10, 10, accent);
            Line(cx - 4.5f, cy, cx - 1, cy + 3.5f, 2, White(1));
            Line(cx - 1, cy + 3.5f, cx + 5, cy - 3.5f, 2, White(1));
        } else if (auto b = Solid(Ink(0.4f))) {
            g_t->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 9.5f, 9.5f), b.Get(), 1.5f);
        }
        Text(p.label, 54, y + 6, W - 84, 20, 13.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(1));
        Text(p.sub, 54, y + 25, W - 84, 16, 11, DWRITE_FONT_WEIGHT_NORMAL, Ink(0.5f));
    }
    g_t->PopAxisAlignedClip();
    if (contentH > listH) {  // where in the list you are
        float bh = std::max(30.0f, listH * listH / contentH), by = g.list.top + (listH - bh) * (g.scroll / (contentH - listH));
        FillRound(W - 8, by, 3, bh, 1.5f, Ink(0.25f));
    }
    // Buttons
    g.cancel = D2D1::RectF(W - 16 - 200, H - 50, W - 16 - 106, H - 16);
    g.ok = D2D1::RectF(W - 16 - 96, H - 50, W - 16, H - 16);
    Fill(g.cancel, Ink(0.10f), 17);
    Text(L"Cancelar", g.cancel.left, g.cancel.top + 8, 94, 18, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, Ink(0.9f), DWRITE_TEXT_ALIGNMENT_CENTER);
    Fill(g.ok, accent, 17);
    Text(L"OK", g.ok.left, g.ok.top + 8, 96, 18, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, White(1), DWRITE_TEXT_ALIGNMENT_CENTER);
}

static int PickerRowAt(D2D1_POINT_2F p) {
    const PickerUi& g = g_pick;
    if (p.x < g.list.left || p.x > g.list.right || p.y < g.list.top || p.y > g.list.bottom) return -1;
    int i = (int)((p.y - g.list.top + g.scroll) / kPickRow);
    return i >= 0 && i < (int)g.shown.size() ? i : -1;
}

static bool PickerMessage(Widget* wd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result) {
    PickerUi& g = g_pick;
    *result = 0;
    auto at = [&](LPARAM l) {
        POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
        return GlassPoint(wd, pt);
    };
    switch (msg) {
        case WM_MOUSEACTIVATE:
            *result = MA_ACTIVATE;
            return true;
        case WM_NCHITTEST: {
            POINT pt = {(short)LOWORD(lp), (short)HIWORD(lp)};
            ScreenToClient(wd->hwnd, &pt);
            D2D1_POINT_2F p = GlassPoint(wd, pt);
            if (p.x < 0 || p.y < 0 || p.x > wd->w || p.y > wd->h) *result = HTTRANSPARENT;
            else *result = p.y < 60 ? HTCAPTION : HTCLIENT;  // dragged by its title
            return true;
        }
        case WM_WINDOWPOSCHANGING:
            return true;  // moved freely, kept on top
        case WM_MOUSEMOVE: {
            int h = PickerRowAt(at(lp));
            if (h != g.hover) {
                g.hover = h;
                Render(wd);
            }
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, wd->hwnd, 0};
            TrackMouseEvent(&tme);
            return true;
        }
        case WM_MOUSELEAVE:
            if (g.hover >= 0) {
                g.hover = -1;
                Render(wd);
            }
            return true;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            return true;
        case WM_LBUTTONUP: {
            D2D1_POINT_2F p = at(lp);
            if (InRect(g.ok, p)) {
                PickerCommit();
                ClosePicker();
            } else if (InRect(g.cancel, p)) {
                ClosePicker();
            } else {
                int i = PickerRowAt(p);
                if (i >= 0) {
                    PickItem& it = g.items[g.shown[i]];
                    it.on = !it.on;
                    if (it.on) it.order = g.nextOrder++;  // the newest goes last
                    Render(wd);
                }
            }
            return true;
        }
        case WM_MOUSEWHEEL:
            g.scroll -= GET_WHEEL_DELTA_WPARAM(wp) / (float)WHEEL_DELTA * kPickRow * 2;
            g.hover = -1;
            Render(wd);
            return true;
        case WM_KEYDOWN:
            if (wp == VK_PRIOR || wp == VK_NEXT) {
                g.scroll += (wp == VK_NEXT ? 1 : -1) * (g.list.bottom - g.list.top - kPickRow);
                Render(wd);
            }
            return true;
        case WM_CHAR:
            if (wp == 27) {
                ClosePicker();
            } else if (wp == L'\r') {
                PickerCommit();
                ClosePicker();
            } else if (wp == 8) {
                if (!g.search.empty()) g.search.pop_back();
            } else if (wp == 22) {
                if (OpenClipboard(wd->hwnd)) {
                    if (HANDLE h = GetClipboardData(CF_UNICODETEXT))
                        if (auto* t = (const wchar_t*)GlobalLock(h)) {
                            for (const wchar_t* q = t; *q && g.search.size() < 40; q++)
                                if (*q >= 32) g.search += *q;
                            GlobalUnlock(h);
                        }
                    CloseClipboard();
                }
            } else if (wp >= 32 && wp != 127 && g.search.size() < 40) {
                g.search += (wchar_t)wp;
            } else {
                return true;
            }
            if (g.wd) {
                g.scroll = 0;
                g.hover = -1;
                PickerFilter();
                Render(wd);
            }
            return true;
    }
    return false;
}

// kind 0: Casa's favorites, 1: the Atalhos
static void OpenPicker(int kind) {
    ClosePicker();
    PickerUi& g = g_pick;
    g.kind = kind;
    if (kind == 0) {
        std::vector<HaEntity> all, shown;
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            all = g_haAll;
            shown = g_ha;
        }
        auto chosen = SplitList(g_ui.haEntities, L",; \n");
        if (chosen.empty())  // automatic so far: start from what the widget shows now
            for (auto& e : shown) chosen.push_back(e.id);
        for (auto& e : all) {
            PickItem p{e.id, e.name, HaKindName(e.domain()) + L" · " + e.id};
            for (size_t i = 0; i < chosen.size(); i++)
                if (chosen[i] == e.id) {
                    p.on = true;
                    p.order = (int)i;
                }
            g.items.push_back(p);
        }
        std::stable_sort(g.items.begin(), g.items.end(), [](const PickItem& a, const PickItem& b) {
            if (a.on != b.on) return a.on;
            if (a.on) return a.order < b.order;
            return _wcsicmp(a.sub.c_str(), b.sub.c_str()) < 0;
        });
        g.nextOrder = (int)chosen.size();
        g.empty = L"A lista vem do seu Home Assistant: ligue o widget Casa e preencha o endereço e o token em Ajustes > Casa. "
                  L"Depois de alguns segundos, abra de novo.";
    } else {
        auto current = ParseShortcuts(g_ui.shortcuts);
        for (size_t i = 0; i < current.size(); i++) {
            PickItem p{current[i].target, current[i].name, L"No widget"};
            p.on = true;
            p.order = (int)i;
            g.items.push_back(p);
        }
        g.nextOrder = (int)current.size();
        std::vector<PickItem> more;
        auto folder = [&](REFKNOWNFOLDERID id, PCWSTR name) {
            PWSTR path = nullptr;
            if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &path))) more.push_back({path, name, L"Pasta"});
            CoTaskMemFree(path);
        };
        folder(FOLDERID_Documents, L"Documentos");
        folder(FOLDERID_Downloads, L"Downloads");
        folder(FOLDERID_Pictures, L"Imagens");
        folder(FOLDERID_Music, L"Músicas");
        folder(FOLDERID_Videos, L"Vídeos");
        folder(FOLDERID_Desktop, L"Área de Trabalho");
        more.push_back({L"shell:MyComputerFolder", L"Este Computador", L"Pasta"});
        more.push_back({L"shell:RecycleBinFolder", L"Lixeira", L"Pasta"});
        more.push_back({L"explorer.exe", L"Explorador de Arquivos", L"App"});
        more.push_back({L"ms-settings:", L"Configurações", L"App"});
        ListApps(more);
        for (auto& m : more) {
            bool dup = false;
            for (auto& p : g.items) dup = dup || Lower(p.id) == Lower(m.id);
            if (!dup) g.items.push_back(m);
        }
        g.empty = L"Nenhum app encontrado";
    }
    PickerFilter();

    Widget* wd = new Widget();
    wd->kind = W_PICKER;
    wd->key = L"picker";
    wd->w = kPickW;
    wd->h = kPickH;
    POINT cur;
    GetCursorPos(&cur);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(cur, MONITOR_DEFAULTTOPRIMARY), &mi);
    wd->scale = GetDpiForSystem() / 96.0f;
    int pw, ph;
    WindowSize(wd, &pw, &ph);
    int x = (mi.rcWork.left + mi.rcWork.right - pw) / 2, y = (mi.rcWork.top + mi.rcWork.bottom - ph) / 2;
    wd->hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kClassName, L"MacOS Widgets - Escolher", WS_POPUP, x, y,
                               pw, ph, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!wd->hwnd) {
        delete wd;
        g = PickerUi{};
        return;
    }
    wd->scale = GetDpiForWindow(wd->hwnd) / 96.0f;  // the monitor it opened on may have another scale
    WindowSize(wd, &pw, &ph);
    SetWindowPos(wd->hwnd, nullptr, (mi.rcWork.left + mi.rcWork.right - pw) / 2, (mi.rcWork.top + mi.rcWork.bottom - ph) / 2, pw, ph,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowDisplayAffinity(wd->hwnd, g_ui.liveGlass ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
    g.wd = wd;
    Capture(wd);
    Render(wd);
    SetWindowLongPtrW(wd->hwnd, GWLP_USERDATA, (LONG_PTR)wd);
    ShowWindow(wd->hwnd, SW_SHOW);
    SetForegroundWindow(wd->hwnd);
}


// ---------------------------------------------------------------------------
// Live glass through Desktop Duplication: Windows says which parts of the screen changed in each frame, so only the
// widgets with something new behind them are taken again, up to 20 times a second, on a thread of their own. The
// widgets are excluded from capture, so what comes is what is behind them
// ---------------------------------------------------------------------------

#define WM_APP_GLASS (WM_APP + 6)  // a new picture behind a widget (lParam: GlassShot*)

struct GlassTarget {
    HWND hwnd;
    RECT rc;       // CaptureRect, screen pixels
    RECT clip;     // what of the screen may show (GlassClip)
    bool visible;  // the desktop shows around it (not covered by a window)
    bool moving;   // being dragged or springing into its cell: taken as often as it can
};
struct GlassShot {
    HWND hwnd;
    RECT rc;
    std::vector<UINT32> px;
};
static std::mutex g_glassMutex;
static std::vector<GlassTarget> g_glassTargets;
static std::vector<std::unique_ptr<GlassShot>> g_glassBox;  // the newest picture for each widget, not yet used
static std::thread g_glassThread;

struct Duplication {
    ComPtr<ID3D11Device> dev;
    ComPtr<ID3D11DeviceContext> ctx;
    ComPtr<IDXGIOutputDuplication> dup;
    ComPtr<ID3D11Texture2D> last, stage;  // the last frame (kept for later), and a small one to read pixels back
    int stageW = 0, stageH = 0;
    RECT out = {};  // the monitor, in screen pixels
};

static bool OpenDuplication(Duplication& d) {
    d = Duplication{};
    HMODULE d3d = LoadLibraryExW(L"d3d11.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32),
            dxgi = LoadLibraryExW(L"dxgi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto create = d3d ? (PFN_D3D11_CREATE_DEVICE)(void*)GetProcAddress(d3d, "D3D11CreateDevice") : nullptr;
    auto factory = dxgi ? (HRESULT(WINAPI*)(REFIID, void**))(void*)GetProcAddress(dxgi, "CreateDXGIFactory1") : nullptr;
    ComPtr<IDXGIFactory1> f;
    if (!create || !factory || FAILED(factory(__uuidof(IDXGIFactory1), (void**)f.GetAddressOf()))) return false;
    // The main monitor (where the grid is), on whichever GPU drives it
    HMONITOR primary = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    ComPtr<IDXGIAdapter1> a;
    for (UINT i = 0; f->EnumAdapters1(i, &a) != DXGI_ERROR_NOT_FOUND; i++, a.Reset()) {
        ComPtr<IDXGIOutput> o;
        for (UINT j = 0; a->EnumOutputs(j, &o) != DXGI_ERROR_NOT_FOUND; j++, o.Reset()) {
            DXGI_OUTPUT_DESC od;
            if (FAILED(o->GetDesc(&od)) || od.Monitor != primary) continue;
            // A turned screen gives its frames unturned: there the 1-second capture is used
            if (od.Rotation != DXGI_MODE_ROTATION_IDENTITY && od.Rotation != DXGI_MODE_ROTATION_UNSPECIFIED) return false;
            ComPtr<IDXGIOutput1> o1;
            if (FAILED(o.As(&o1))) return false;
            D3D_FEATURE_LEVEL fl;
            if (FAILED(create(a.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
                              &d.dev, &fl, &d.ctx)))
                return false;
            if (FAILED(o1->DuplicateOutput(d.dev.Get(), &d.dup))) {
                d = Duplication{};
                return false;
            }
            d.out = od.DesktopCoordinates;
            return true;
        }
    }
    return false;
}

// Reads one widget's part of the last frame
static GlassShot* TakeShot(Duplication& d, const GlassTarget& t) {
    int w = t.rc.right - t.rc.left, h = t.rc.bottom - t.rc.top;
    RECT in, part;
    if (w <= 0 || h <= 0 || !d.last || !IntersectRect(&part, &t.rc, &d.out) || !IntersectRect(&in, &part, &t.clip)) return nullptr;
    if (!d.stage || d.stageW < w || d.stageH < h) {
        d.stage.Reset();
        D3D11_TEXTURE2D_DESC td = {};
        td.Width = std::max(w, d.stageW);
        td.Height = std::max(h, d.stageH);
        td.MipLevels = td.ArraySize = 1;
        td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        td.SampleDesc.Count = 1;
        td.Usage = D3D11_USAGE_STAGING;
        td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        if (FAILED(d.dev->CreateTexture2D(&td, nullptr, &d.stage))) return nullptr;
        d.stageW = td.Width;
        d.stageH = td.Height;
    }
    D3D11_BOX box = {(UINT)(in.left - d.out.left), (UINT)(in.top - d.out.top), 0, (UINT)(in.right - d.out.left),
                     (UINT)(in.bottom - d.out.top), 1};
    d.ctx->CopySubresourceRegion(d.stage.Get(), 0, 0, 0, 0, d.last.Get(), 0, &box);
    D3D11_MAPPED_SUBRESOURCE map;
    if (FAILED(d.ctx->Map(d.stage.Get(), 0, D3D11_MAP_READ, 0, &map))) return nullptr;
    auto* shot = new GlassShot{t.hwnd, t.rc, std::vector<UINT32>((size_t)w * h)};
    int ox = in.left - t.rc.left, oy = in.top - t.rc.top, iw = in.right - in.left;
    for (int y = 0; y < in.bottom - in.top; y++)
        memcpy(&shot->px[(size_t)(oy + y) * w + ox], (const BYTE*)map.pData + (size_t)y * map.RowPitch, (size_t)iw * 4);
    d.ctx->Unmap(d.stage.Get(), 0);
    ExtendEdges(shot->px, w, h, {ox, oy, ox + iw, oy + (in.bottom - in.top)});
    return shot;
}

static void GlassThread() {
    Duplication d;
    ULONGLONG retryAt = 0;
    struct State {
        HWND hwnd;
        ULONGLONG last;
        bool pending, visible;
        RECT rc;
    };
    std::vector<State> states;
    std::vector<BYTE> meta;
    while (g_running) {
        if (!g_liveGlassOn) {
            if (d.dup) d = Duplication{};
            g_ddaOn = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            continue;
        }
        ULONGLONG now = GetTickCount64();
        if (!d.dup) {
            g_ddaOn = false;
            if (now < retryAt) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            }
            if (!OpenDuplication(d)) {
                retryAt = now + 5000;  // the 1-second capture keeps working meanwhile
                continue;
            }
            g_ddaOn = true;
            states.clear();
        }
        DXGI_OUTDUPL_FRAME_INFO info;
        ComPtr<IDXGIResource> res;
        bool anyMoving = false;
        {
            std::lock_guard<std::mutex> lock(g_glassMutex);
            for (auto& t : g_glassTargets) anyMoving = anyMoving || t.moving;
        }
        // While a widget moves, its new place is read from the last frame right away (nothing on the screen needs to
        // change for that): short waits, so the glass follows the drag
        HRESULT hr = d.dup->AcquireNextFrame(anyMoving ? 8 : 40, &info, &res);
        std::vector<RECT> changed;
        if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
            // nothing new on the screen: only what was held back below goes out
        } else if (FAILED(hr)) {
            d = Duplication{};  // a mode change, the lock screen...: opened again in a moment
            retryAt = now + 1000;
            continue;
        } else {
            if (info.LastPresentTime.QuadPart != 0) {  // not just the mouse moving
                ComPtr<ID3D11Texture2D> frame;
                if (SUCCEEDED(res.As(&frame))) {
                    D3D11_TEXTURE2D_DESC fd;
                    frame->GetDesc(&fd);
                    if (fd.Format != DXGI_FORMAT_B8G8R8A8_UNORM) {  // HDR and others: read as BGRA it would be noise
                        d.dup->ReleaseFrame();
                        d = Duplication{};
                        retryAt = now + 60 * 1000;
                        continue;
                    }
                    if (!d.last) {
                        D3D11_TEXTURE2D_DESC td = fd;
                        td.Usage = D3D11_USAGE_DEFAULT;
                        td.BindFlags = 0;
                        td.CPUAccessFlags = 0;
                        td.MiscFlags = 0;
                        d.dev->CreateTexture2D(&td, nullptr, &d.last);
                        changed.push_back({d.out.left, d.out.top, d.out.right, d.out.bottom});
                    }
                    if (d.last) d.ctx->CopyResource(d.last.Get(), frame.Get());
                }
                if (info.TotalMetadataBufferSize > 0) {
                    meta.resize(info.TotalMetadataBufferSize);
                    UINT used = 0;
                    if (SUCCEEDED(d.dup->GetFrameMoveRects((UINT)meta.size(), (DXGI_OUTDUPL_MOVE_RECT*)meta.data(), &used)))
                        for (UINT i = 0; i < used / sizeof(DXGI_OUTDUPL_MOVE_RECT); i++) {
                            RECT r = ((DXGI_OUTDUPL_MOVE_RECT*)meta.data())[i].DestinationRect;
                            OffsetRect(&r, d.out.left, d.out.top);
                            changed.push_back(r);
                        }
                    if (SUCCEEDED(d.dup->GetFrameDirtyRects((UINT)meta.size(), (RECT*)meta.data(), &used)))
                        for (UINT i = 0; i < used / sizeof(RECT); i++) {
                            RECT r = ((RECT*)meta.data())[i];
                            OffsetRect(&r, d.out.left, d.out.top);
                            changed.push_back(r);
                        }
                }
            }
            d.dup->ReleaseFrame();
        }
        std::vector<GlassTarget> targets;
        {
            std::lock_guard<std::mutex> lock(g_glassMutex);
            targets = g_glassTargets;
        }
        for (const GlassTarget& t : targets) {
            State* st = nullptr;
            for (auto& s : states)
                if (s.hwnd == t.hwnd) st = &s;
            if (!st) {
                states.push_back({t.hwnd, 0, true, false, t.rc});
                st = &states.back();
            }
            if (t.visible && (!st->visible || !EqualRect(&st->rc, &t.rc))) st->pending = true;  // just uncovered / moved
            st->visible = t.visible;
            st->rc = t.rc;
            if (!t.visible) continue;
            RECT x;
            for (const RECT& c : changed)
                if (IntersectRect(&x, &c, &t.rc)) st->pending = true;
            if (st->pending && now - st->last >= (t.moving ? 16u : 50u)) {  // 60 a second moving, else 20 at most
                if (GlassShot* shot = TakeShot(d, t)) {
                    {
                        std::lock_guard<std::mutex> lock(g_glassMutex);
                        bool replaced = false;
                        for (auto& b : g_glassBox)
                            if (b->hwnd == shot->hwnd) {
                                b.reset(shot);  // an older one not used yet: only the newest matters
                                replaced = true;
                            }
                        if (!replaced) g_glassBox.emplace_back(shot);
                    }
                    PostThreadMessageW(g_uiThreadId, WM_APP_GLASS, 0, 0);
                }
                st->pending = false;
                st->last = now;
            }
        }
        // Widgets that are gone (rebuilt, removed) are forgotten
        states.erase(std::remove_if(states.begin(), states.end(),
                                    [&](const State& st) {
                                        for (auto& t : targets)
                                            if (t.hwnd == st.hwnd) return false;
                                        return true;
                                    }),
                     states.end());
    }
    g_ddaOn = false;
    std::lock_guard<std::mutex> lock(g_glassMutex);
    g_glassBox.clear();
}

// Tells the glass thread where the widgets are and which ones show the desktop (4 times a second)
static bool ShowsDesktop(Widget* wd);
static void PublishGlassTargets() {
    std::vector<GlassTarget> t;
    auto add = [&](Widget* wd, bool visible) {
        if (!wd || !wd->hwnd) return;
        bool moving = g_drag.dragging && wd == g_drag.wd;
        for (auto& sp : g_springs)
            if (sp.wd == wd) moving = true;
        RECT rc = CaptureRect(wd);
        t.push_back({wd->hwnd, rc, GlassClip(rc), visible || moving, moving});
    };
    for (Widget* wd : g_widgets) add(wd, ShowsDesktop(wd));
    add(g_cfg.wd, true);
    std::lock_guard<std::mutex> lock(g_glassMutex);
    g_glassTargets = t;
}

static void OnGlassShot(GlassShot* shot);
static void TakeGlassBox() {
    std::vector<std::unique_ptr<GlassShot>> box;
    {
        std::lock_guard<std::mutex> lock(g_glassMutex);
        box.swap(g_glassBox);
    }
    for (auto& b : box) OnGlassShot(b.release());
}

static void OnGlassShot(GlassShot* shot) {
    Widget* wd = nullptr;
    for (Widget* w : g_widgets)
        if (w->hwnd == shot->hwnd) wd = w;
    if (g_cfg.wd && g_cfg.wd->hwnd == shot->hwnd) wd = g_cfg.wd;
    bool moving = wd && g_drag.dragging && wd == g_drag.wd;
    for (auto& sp : g_springs)
        if (sp.wd == wd) moving = true;
    if (wd && g_ui.liveGlass) {
        RECT now = CaptureRect(wd);
        // Still: only a picture of where it is now. Moving: the newest picture, even of a few pixels back (it is drawn
        // at its own place on the screen, and the border around the glass covers the difference)
        bool sameSize = now.right - now.left == shot->rc.right - shot->rc.left && now.bottom - now.top == shot->rc.bottom - shot->rc.top;
        if ((moving ? sameSize : EqualRect(&now, &shot->rc)) && ApplyCapture(wd, shot->rc, std::move(shot->px))) Render(wd);
    }
    delete shot;
}

// Live glass, a little each second: only widgets the desktop can be seen around (what is behind a covered widget is
// the window over it), the ones taken longest ago first, a few at a time so the screen never stalls
static bool ShowsDesktop(Widget* wd) {
    RECT r;
    GetWindowRect(wd->hwnd, &r);
    int m = (int)std::lround((kMargin + 14) * wd->scale);
    POINT pts[5] = {{(r.left + r.right) / 2, (r.top + r.bottom) / 2}, {r.left + m, r.top + m}, {r.right - m, r.top + m},
                    {r.left + m, r.bottom - m}, {r.right - m, r.bottom - m}};
    for (POINT p : pts) {
        HWND h = WindowFromPoint(p);
        if (!h) continue;
        HWND root = GetAncestor(h, GA_ROOT);
        if (!root || IsOurWindow(root) || IsDeskClass(root)) continue;
        return false;  // a window is over it
    }
    return true;
}

static void LiveGlassTick() {
    if (g_ddaOn) return;  // the glass thread is doing it, much more often
    std::vector<Widget*> due;
    for (Widget* wd : g_widgets) {
        bool moving = g_drag.dragging && wd == g_drag.wd;
        for (auto& sp : g_springs)
            if (sp.wd == wd) moving = true;
        if (!moving && ShowsDesktop(wd)) due.push_back(wd);
    }
    std::sort(due.begin(), due.end(), [](Widget* a, Widget* b) { return a->capTick < b->capTick; });
    for (size_t i = 0; i < due.size() && i < 3; i++) Capture(due[i]);
    if (g_cfg.wd) Capture(g_cfg.wd);
}

// Each second: what changes by the second always, the rest every 5 seconds or when its glass changed
static void RenderTick() {
    static unsigned tick = 0;
    tick++;
    for (Widget* wd : g_widgets) {
        bool live = wd->kind == W_CLOCK || wd->kind == W_WORLD || wd->kind == W_SYSTEM || wd->kind == W_NETWORK ||
                    wd->kind == W_MUSIC || wd->kind == W_SENSORS || IsEditing(wd);
        if (live || wd->fresh || tick % 5 == 0) Render(wd);
    }
    if (g_cfg.wd) Render(g_cfg.wd);
    if (g_menu.wd) Render(g_menu.wd);
}

static void DestroyWidgets() {
    FinishEdit(true);
    AbortDrag();
    for (Widget* wd : g_widgets) {
        SetWindowLongPtrW(wd->hwnd, GWLP_USERDATA, 0);
        ReleaseSurface(wd);
        if (wd->hwnd) DestroyWindow(wd->hwnd);
        delete wd;
    }
    g_widgets.clear();
    FreeDesktopPicture();
}

static void RenderAll() {
    for (Widget* wd : g_widgets) Render(wd);
    if (g_cfg.wd) Render(g_cfg.wd);
    if (g_menu.wd) Render(g_menu.wd);
}

static void RebuildWidgets() {
    // The new widgets come up before the old ones go: destroying first left the desktop empty for a moment (every
    // widget gone, then back) on each change of size, Arrange Widgets or settings. Captures don't see our windows
    // (the live glass excludes them, the other mode reads the desktop's own picture), so the old ones being there
    // changes nothing in the new glass, and they're out of g_widgets, so their cells count as free
    FinishEdit(true);
    AbortDrag();
    std::vector<Widget*> old;
    old.swap(g_widgets);
    for (Widget* wd : old) SetWindowLongPtrW(wd->hwnd, GWLP_USERDATA, 0);  // no more input for them
    FreeDesktopPicture();
    CreateWidgets();
    for (Widget* wd : old) {
        ReleaseSurface(wd);
        if (wd->hwnd) DestroyWindow(wd->hwnd);
        delete wd;
    }
    if (g_cfg.wd) {
        SetWindowDisplayAffinity(g_cfg.wd->hwnd, g_ui.liveGlass ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
        Capture(g_cfg.wd);
        Render(g_cfg.wd);
    }
}

static void StartWorkers();

static std::atomic<bool> g_uiDone{false};  // the UI thread has saved what it had to (it's closing)

static DWORD WINAPI UiThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // creates the message queue
    SetEvent(g_uiReady);

    D2D1_FACTORY_OPTIONS fo = {};
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &fo, (void**)g_d2d.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)g_dw.GetAddressOf());

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = WidgetProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    wc.lpszClassName = kClassName;
    RegisterClassExW(&wc);

    if (g_d2d && g_dw) {
        UpdateSystemStats();
        CreateWidgets();
        g_secondTimer = SetTimer(nullptr, 0, 1000, nullptr);
        g_glassTimer = SetTimer(nullptr, 0, 250, nullptr);
        g_fgHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, OnForeground, 0, 0,
                                   WINEVENT_OUTOFCONTEXT);
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            if (!msg.hwnd) {
                if (msg.message == WM_TIMER) {
                    if (msg.wParam == g_frameTimer) {
                        Frame();
                        continue;
                    }
                    if (msg.wParam == g_glassTimer) {
                        if (g_ui.liveGlass) PublishGlassTargets();
                        TakeGlassBox();  // pictures whose message was lost (e.g. in the file picker's loop)
                        continue;
                    }
                    if (msg.wParam == g_secondTimer) {
                        UpdateSystemStats();
                        if (g_ui.showScreenTime) TrackUsage();
                        RECT work;
                        SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
                        if (!EqualRect(&work, &g_lastWork) && !g_drag.wd && g_springs.empty()) {
                            RebuildWidgets();  // the taskbar or a bar changed the work area: the grid moved
                            continue;
                        }
                        if (!g_drag.wd && WidgetsBelowDesktop()) PlaceAtDesktop();
                        // A menu that never got the focus closes by itself
                        if (g_menu.wd && GetForegroundWindow() != g_menu.wd->hwnd && GetTickCount64() - g_menu.opened > 1500)
                            CloseMenu();
                        if (g_ui.liveGlass) LiveGlassTick();
                        RenderTick();
                        continue;
                    }
                    RenderAll();
                    continue;
                }
                if (msg.message == WM_APP_GLASS) {
                    TakeGlassBox();
                    continue;
                }
                if (msg.message == WM_APP_DESK) {
                    if (!g_drag.wd && WidgetsBelowDesktop()) PlaceAtDesktop();
                    continue;
                }
                if (msg.message == WM_APP_PLACES) {
                    auto* found = (std::vector<Place>*)msg.lParam;
                    if (!found) g_placeStatus = L"Não foi possível procurar agora (sem internet?)";
                    else if (found->empty()) g_placeStatus = L"Nenhuma cidade encontrada com esse nome";
                    else g_placeStatus.clear();
                    g_places = found ? *found : std::vector<Place>();
                    delete found;
                    if (g_cfg.wd) Render(g_cfg.wd);
                    continue;
                }
                if (msg.message == WM_APP_REDRAW) {
                    // New data: the second timer redraws everything anyway; only what should react at once here
                    // (a music button just pressed, a notification that arrived)
                    for (Widget* wd : g_widgets)
                        if (wd->kind == W_MUSIC || wd->kind == W_NOTIF) Render(wd);
                    continue;
                }
                if (msg.message == WM_APP_RELOAD) {
                    CloseMenu();
                    ClosePicker();
                    CloseSettings();
                    DestroyWidgets();
                    CreateWidgets();
                    continue;
                }
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    } else {
        Wh_Log(L"Direct2D/DirectWrite init failed");
    }

    // What must not be lost goes first: the note being written and today's Screen Time
    FinishEdit(true);
    if (g_usageDay >= 0) SaveUsage();
    g_uiDone = true;  // the rest is tidying up, which ending the process does anyway
    if (g_fgHook) UnhookWinEvent(g_fgHook);
    g_fgHook = nullptr;
    if (g_animTimer) KillTimer(nullptr, g_animTimer);
    if (g_secondTimer) KillTimer(nullptr, g_secondTimer);
    if (g_glassTimer) KillTimer(nullptr, g_glassTimer);
    g_glassTimer = 0;
    g_animTimer = g_secondTimer = 0;
    CloseMenu();
    ClosePicker();
    CloseSettings();
    DestroyWidgets();
    UnregisterClassW(kClassName, GetModuleHandleW(nullptr));
    g_dw.Reset();
    g_d2d.Reset();
    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

// A worker thread whose unexpected error only stops that thread's widgets from updating, not the whole process
template <class F>
static std::thread GuardedThread(PCWSTR name, F body) {
    return std::thread([name, body] {
        try {
            body();
        } catch (...) {
            Wh_Log(L"%s: unexpected error, stopped updating", name);
        }
    });
}

// The threads that fetch and measure
static void StartWorkers() {
    try {
        g_dataThread = GuardedThread(L"Data", DataThread);
        g_sensorThread = GuardedThread(L"Sensors", SensorThread);
        g_glassThread = GuardedThread(L"Glass", GlassThread);
        g_coverThread = GuardedThread(L"Covers", CoverThread);
        g_netThread = GuardedThread(L"Network", NetThread);
    } catch (...) {
        Wh_Log(L"Could not start the worker threads");
    }
}

BOOL WhTool_ModInit() {
    // Widgets keep their pace when the PC decides this process is "in the background" (efficiency mode would slow
    // the clock, the music's progress and the live glass)
    PROCESS_POWER_THROTTLING_STATE pt = {PROCESS_POWER_THROTTLING_CURRENT_VERSION, PROCESS_POWER_THROTTLING_EXECUTION_SPEED, 0};
    SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &pt, sizeof(pt));

    LoadSettings();
    g_running = true;
    g_uiReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiThread = CreateThread(nullptr, 0, UiThread, nullptr, 0, &g_uiThreadId);
    if (!g_uiThread) return FALSE;
    WaitForSingleObject(g_uiReady, 5000);  // its message queue exists: settings changes can be posted to it
    StartWorkers();
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        g_weatherRefresh = true;
    }
    g_cv.notify_all();
    if (g_uiThreadId) PostThreadMessageW(g_uiThreadId, WM_APP_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    // The process ends right after this (ExitProcess), and an update's new copy starts at the same moment and gives
    // up if this one is still around: so this only saves what matters and returns, without tidying up
    {
        // Under the lock, so a thread about to wait can't miss it
        std::lock_guard<std::mutex> lock(g_dataMutex);
        g_running = false;
    }
    g_cv.notify_all();
    HttpAbortAll();
    if (g_uiThread) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        for (int i = 0; i < 100 && !g_uiDone && WaitForSingleObject(g_uiThread, 10) == WAIT_TIMEOUT; i++) {
        }
    }
    // The threads are left to the end of the process (detached, so their objects don't object to it)
    for (std::thread* t : {&g_dataThread, &g_sensorThread, &g_glassThread, &g_coverThread, &g_netThread})
        if (t->joinable()) t->detach();
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
        // MacOS Widgets: the only change to the snippet. When the mod is updated, Windhawk starts the new tool
        // process at the same moment it tells the old one to unload, so the new one usually finds the old one's
        // mutex still there and quits, and the widgets only come back after disabling and enabling the mod. It
        // waits for the old process to exit instead (up to 10 s).
        for (int tries = 0;; tries++) {
            g_toolModProcessMutex =
                CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
            if (!g_toolModProcessMutex) {
                Wh_Log(L"CreateMutex failed");
                ExitProcess(1);
            }

            if (GetLastError() != ERROR_ALREADY_EXISTS) {
                break;
            }

            CloseHandle(g_toolModProcessMutex);
            g_toolModProcessMutex = nullptr;
            if (tries >= 40) {
                Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
                ExitProcess(1);
            }
            Sleep(250);
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
