// ==WindhawkMod==
// @id              volume-mixer-popup
// @name            Volume Mixer Popup Pro
// @name:uk-UA       Спливаючий мікшер гучності Pro
// @name:pl-PL       Wyskakujący mikser głośności Pro
// @name:de-DE       Lautstärkemixer-Popup Pro
// @name:fr-FR       Mixeur de volume contextuel Pro
// @name:es-ES       Mezclador de volumen emergente Pro
// @name:ru-RU       Всплывающий микшер громкости Pro
// @description     Per-app volume mixer popup with live VU meters, per-app output device routing, volume locks, focused-app hotkeys, an optional tray icon and a volume OSD.
// @version         2.6.1
// @author          neMik
// @github          https://github.com/mykolazozul
// @include         windhawk.exe
// @compilerOptions -lole32 -luuid -lgdi32 -lgdiplus -luser32 -lshell32 -lversion -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 🔊 Volume Mixer Popup Pro

A fast, lightweight per-app volume mixer for Windows 11 and 10. It runs in its own small Windhawk
process, not inside Explorer, and opens on top of any window — including borderless and fullscreen games — without
taking the keyboard focus, so games are never minimized.

## ✨ Highlights
- **Per-app volume and mute** for every output device, with live VU meters (up to 60 FPS while open).
- **Per-app audio routing** – send any app to another output device with the ⇄ button or a right-click.
- **Volume lock** – pin the volume of an app or device; a watchdog restores it when a game or Windows changes it.
  Locks are kept across restarts and marked with a padlock.
- **Game hotkeys** – change or mute the focused game/app on every device it plays to, without opening anything.
  UWP apps and protected/elevated games are recognized too.
- **Deafen background (BETA)** – one hotkey mutes or ducks everything except the focused app and your whitelist;
  pressing it again restores every app individually.
- **Custom volume OSD (BETA)** – 9 styles, can replace the Windows volume flyout for the media keys and the volume
  tooltip of the taskbar, draggable. Like the Windows flyout, it stays while the pointer rests on it, and the mouse
  wheel over it changes the volume.
- **System tray icon (BETA)** – 9 icon styles that follow the volume and the light/dark taskbar, 7 popup styles,
  mouse wheel over the icon changes the volume, middle click mutes.
- **17 mixer layouts and 22 color themes.**
- **Correct scaling** – interface scale 30–300 % multiplied by the Windows display scaling of each monitor;
  the popup shrinks automatically when it would not fit on the screen.
- **Light on resources** – nothing is polled while the popup is closed: the tray icon and the volume locks react to
  Windows audio events, and the keyboard and mouse hooks are installed only while a feature needs them.

## 🎛️ Mixer layouts
Modern Cards · Compact Strip · Hi-Fi Studio Meters · Fluent Flyout · Cyber Radial HUD · Studio Mixing Desk ·
Floating Minimal Pill · Cyberpunk Neon Grid · Retro Cassette · LED Meter Bridge · Arcade Panel ·
Broadcast Console (dual VU) · Control Center Pills · Frosted Glass Tiles · Retro Terminal ·
Soft Neumorphic · Vertical Channel Strips.

## 🎨 Themes
Windows 11 Mica · Modern Dark · Light Fluent · AMOLED Black · Frosted Glass · System Accent · Classic Blue ·
Dracula · Nordic Frost · Cyberpunk Neon · Retro Amber · Tokyo Night · Sunset · Catppuccin Mocha · Forest ·
Monochrome · Deep Ocean · Rosé Pine · Gruvbox · Solarized Dark · Matrix Green · Midnight Gold.

## 🔔 Tray icon (BETA)
- Icons: Windows native, Fluent speaker, studio faders, spectrum, headphones, retro knob, status dot, equalizer bars, sonar pulse.
- Popup on click: Windows 11 quick flyout (default device + expandable app list), full mixer, compact fader,
  mini island, radial dial, vertical channel strips, Control Center pills.
- Scroll the mouse wheel over the icon to change the master volume; middle-click to mute.

## 📺 OSD styles (BETA)
Windows 11 capsule · compact pill · detailed (name, level and device) · circular ring · vertical edge bar ·
frosted glass · tactical game HUD · stealth dot matrix · Control Center pill.

- Once the pointer moves onto the OSD, it stays on screen until the pointer leaves; the mouse wheel over it
  changes the volume it shows (the master volume or the app). Clicks always pass through it (unless dragging is
  enabled), and a hidden pointer (games) is ignored.
- **Replace the Windows volume flyout**: the media volume keys and the mouse wheel over the taskbar notification
  area (tray icons and clock) go through the mod, so Windows doesn't show its own flyout or the Windows 11 volume
  tooltip. Other volume changes, such as touchpad gestures, headset buttons or laptop hotkey drivers, are made by
  Windows itself; when Windows shows its flyout for one of them, the mod keeps that flyout invisible (it clips
  Explorer's indicator window to an empty region and hides it) and shows its own OSD, which follows every step.
  Brightness and other Windows indicators stay as they are. While the option is on, other tray apps and mods that react to the wheel in the notification area
  (for example Taskbar Volume Control with the tray area as its scroll area) don't get it. Volume indicators of
  manufacturer utilities (laptop or keyboard software) are separate programs and aren't affected.

## ⌨️ Hotkeys
| Action | Default |
|---|---|
| Open / close mixer | `Win+Alt+V` |
| Active app volume up / down | not assigned |
| Active app mute / unmute | not assigned |
| Deafen background (when enabled) | not assigned |
| Master volume up / down / mute | not assigned |

Only the mixer hotkey is set by default, so the mod doesn't take shortcuts that other apps use.

Format: `Ctrl`, `Alt`, `Shift`, `Win` + one key (`A`–`Z`, `0`–`9`, `F1`–`F24`, `Up`, `Down`, `Left`, `Right`,
`Space`, `PageUp`, `PageDown`, `Num0`–`Num9`, `VolumeUp`, `VolumeDown`, `Mute`…), joined with `+`.
Leave a hotkey empty to disable it. On keyboard layouts with AltGr, Right Alt doesn't count as `Ctrl+Alt`.

## 🖱️ Using the mixer
- Drag a slider or scroll over it to change the volume.
- Click an app icon to mute or unmute it.
- Click ⇄ (or right-click a row) to route the app to another device or to lock its volume.
- Right-click or double-click a device header to make it the Windows default device.
- Press any key or click outside to close the mixer.

## 🌐 Language
The mixer, menus and OSD use **Language → Mod interface language** (automatic, English, Ukrainian, Polish,
German, French, Spanish, Russian). The settings page is translated into the same languages and follows
the language of Windhawk itself (**Windhawk → Settings → Language**); other languages show English.

## ⚠️ Notes
- Features marked BETA (tray icon, OSD, Deafen background) are off by default.
- "Route audio to" uses the same undocumented Windows API as the "App volume and device preferences"
  page of Windows and EarTrumpet; some apps only switch the device after their audio stream restarts.
- While a game or app running as administrator is focused, hotkeys work through standard Windows hotkeys:
  holding volume up/down doesn't repeat there, and `Ctrl+Alt` combinations don't work there.

## 🧹 What stays after the mod is disabled
Like the Windows volume mixer, the mod changes Windows' own audio preferences, and Windows keeps them:
- **Output device of an app** chosen with "Route audio to": stays until you pick "Windows default device" in
  the same menu, or use Settings → System → Sound → Volume mixer → Reset.
- **Default output device** chosen in the mixer: stays, like a change made in the Windows sound settings.
- **Volume and mute levels** of devices and apps: Windows remembers them, as with any mixer.
- **Deafen background**: if the mod stops while Deafen is on (crash, shutdown, power loss), it restores the
  muted and ducked apps the next time it starts.
- Volume locks are stored by the mod and are no longer enforced while it's disabled.
- **Windows volume flyout** (with "Replace the Windows volume flyout"): the mod gives Explorer's indicator window
  back its normal shape when it's disabled. If its process is killed instead, the Windows flyout stays invisible
  until Explorer restarts (for example after signing out).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- general:
  - language: "auto"
    $name: "Mod interface language"
    $name:uk-UA: "Мова інтерфейсу мода"
    $name:pl-PL: "Język interfejsu moda"
    $name:de-DE: "Sprache der Mod-Oberfläche"
    $name:fr-FR: "Langue de l'interface du mod"
    $name:es-ES: "Idioma de la interfaz del mod"
    $name:ru-RU: "Язык интерфейса мода"
    $description: "Language of the mixer, menus and OSD. This settings page follows Windhawk's own language: Windhawk → Settings → Language."
    $description:uk-UA: "Мова мікшера, меню та OSD. Мова цієї сторінки налаштувань береться з мови самого Windhawk: Windhawk → Налаштування → Мова."
    $description:pl-PL: "Język miksera, menu i OSD. Język tej strony ustawień zależy od języka Windhawk: Windhawk → Ustawienia → Język."
    $description:de-DE: "Sprache von Mixer, Menüs und OSD. Diese Einstellungsseite folgt der Sprache von Windhawk: Windhawk → Einstellungen → Sprache."
    $description:fr-FR: "Langue du mixeur, des menus et de l'OSD. Cette page suit la langue de Windhawk : Windhawk → Paramètres → Langue."
    $description:es-ES: "Idioma del mezclador, menús y OSD. Esta página sigue el idioma de Windhawk: Windhawk → Ajustes → Idioma."
    $description:ru-RU: "Язык микшера, меню и OSD. Язык этой страницы настроек берётся из языка Windhawk: Windhawk → Настройки → Язык."
    $options:
    - auto: "Automatic (Windows language)"
    - en: "English"
    - uk: "Українська"
    - pl: "Polski"
    - de: "Deutsch"
    - fr: "Français"
    - es: "Español"
    - ru: "Русский"
    $options:uk-UA:
    - auto: "Автоматично (мова Windows)"
    - en: "English"
    - uk: "Українська"
    - pl: "Polski"
    - de: "Deutsch"
    - fr: "Français"
    - es: "Español"
    - ru: "Русский"
    $options:pl-PL:
    - auto: "Automatycznie (język Windows)"
    - en: "English"
    - uk: "Українська"
    - pl: "Polski"
    - de: "Deutsch"
    - fr: "Français"
    - es: "Español"
    - ru: "Русский"
    $options:de-DE:
    - auto: "Automatisch (Windows-Sprache)"
    - en: "English"
    - uk: "Українська"
    - pl: "Polski"
    - de: "Deutsch"
    - fr: "Français"
    - es: "Español"
    - ru: "Русский"
    $options:fr-FR:
    - auto: "Automatique (langue de Windows)"
    - en: "English"
    - uk: "Українська"
    - pl: "Polski"
    - de: "Deutsch"
    - fr: "Français"
    - es: "Español"
    - ru: "Русский"
    $options:es-ES:
    - auto: "Automático (idioma de Windows)"
    - en: "English"
    - uk: "Українська"
    - pl: "Polski"
    - de: "Deutsch"
    - fr: "Français"
    - es: "Español"
    - ru: "Русский"
    $options:ru-RU:
    - auto: "Автоматически (язык Windows)"
    - en: "English"
    - uk: "Українська"
    - pl: "Polski"
    - de: "Deutsch"
    - fr: "Français"
    - es: "Español"
    - ru: "Русский"
  $name: "🌐 Language"
  $name:uk-UA: "🌐 Мова"
  $name:pl-PL: "🌐 Język"
  $name:de-DE: "🌐 Sprache"
  $name:fr-FR: "🌐 Langue"
  $name:es-ES: "🌐 Idioma"
  $name:ru-RU: "🌐 Язык"
- hotkeys:
  - hotkey: "Win+Alt+V"
    $name: "Open / close mixer"
    $name:uk-UA: "Відкрити / закрити мікшер"
    $name:pl-PL: "Otwórz / zamknij mikser"
    $name:de-DE: "Mixer öffnen / schließen"
    $name:fr-FR: "Ouvrir / fermer le mixeur"
    $name:es-ES: "Abrir / cerrar el mezclador"
    $name:ru-RU: "Открыть / закрыть микшер"
    $description: "Examples: Win+Alt+V, Ctrl+Shift+F9, Alt+F10. Leave empty to disable. On layouts with AltGr, Right Alt doesn't count as Ctrl+Alt."
    $description:uk-UA: "Приклади: Win+Alt+V, Ctrl+Shift+F9, Alt+F10. Залиште порожнім, щоб вимкнути. На розкладках з AltGr правий Alt не вважається Ctrl+Alt."
    $description:pl-PL: "Przykłady: Win+Alt+V, Ctrl+Shift+F9, Alt+F10. Pozostaw puste, aby wyłączyć. W układach z AltGr prawy Alt nie liczy się jako Ctrl+Alt."
    $description:de-DE: "Beispiele: Win+Alt+V, Ctrl+Shift+F9, Alt+F10. Leer lassen zum Deaktivieren. Bei Layouts mit AltGr zählt die rechte Alt-Taste nicht als Ctrl+Alt."
    $description:fr-FR: "Exemples : Win+Alt+V, Ctrl+Shift+F9, Alt+F10. Laisser vide pour désactiver. Sur les dispositions avec AltGr, Alt droite ne compte pas comme Ctrl+Alt."
    $description:es-ES: "Ejemplos: Win+Alt+V, Ctrl+Shift+F9, Alt+F10. Déjalo vacío para desactivar. En distribuciones con AltGr, Alt derecha no cuenta como Ctrl+Alt."
    $description:ru-RU: "Примеры: Win+Alt+V, Ctrl+Shift+F9, Alt+F10. Оставьте пустым, чтобы отключить. На раскладках с AltGr правый Alt не считается Ctrl+Alt."
  - hotkeyAppVolUp: ""
    $name: "Active app: volume up"
    $name:uk-UA: "Активна програма: гучніше"
    $name:pl-PL: "Aktywna aplikacja: głośniej"
    $name:de-DE: "Aktive App: lauter"
    $name:fr-FR: "App active : volume +"
    $name:es-ES: "App activa: subir volumen"
    $name:ru-RU: "Активное приложение: громче"
  - hotkeyAppVolDown: ""
    $name: "Active app: volume down"
    $name:uk-UA: "Активна програма: тихіше"
    $name:pl-PL: "Aktywna aplikacja: ciszej"
    $name:de-DE: "Aktive App: leiser"
    $name:fr-FR: "App active : volume −"
    $name:es-ES: "App activa: bajar volumen"
    $name:ru-RU: "Активное приложение: тише"
  - hotkeyAppMute: ""
    $name: "Active app: mute / unmute"
    $name:uk-UA: "Активна програма: вимкнути / увімкнути звук"
    $name:pl-PL: "Aktywna aplikacja: wycisz / przywróć"
    $name:de-DE: "Aktive App: stumm / laut"
    $name:fr-FR: "App active : couper / rétablir le son"
    $name:es-ES: "App activa: silenciar / activar"
    $name:ru-RU: "Активное приложение: выкл / вкл звук"
    $description: "Works on the focused game or app, on every audio device it plays to."
    $description:uk-UA: "Діє на гру чи програму у фокусі — на всіх аудіопристроях, куди вона виводить звук."
    $description:pl-PL: "Działa na grę lub aplikację w fokusie na wszystkich urządzeniach audio."
    $description:de-DE: "Wirkt auf das fokussierte Spiel bzw. die App auf allen Audiogeräten."
    $description:fr-FR: "Agit sur le jeu ou l'application au premier plan, sur tous les périphériques audio."
    $description:es-ES: "Actúa sobre el juego o la app en primer plano en todos los dispositivos de audio."
    $description:ru-RU: "Действует на игру или приложение в фокусе — на всех аудиоустройствах."
  - hotkeyMasterVolUp: ""
    $name: "Master volume up"
    $name:uk-UA: "Загальна гучність: гучніше"
    $name:pl-PL: "Głośność główna: głośniej"
    $name:de-DE: "Gesamtlautstärke: lauter"
    $name:fr-FR: "Volume principal +"
    $name:es-ES: "Volumen maestro: subir"
    $name:ru-RU: "Общая громкость: громче"
  - hotkeyMasterVolDown: ""
    $name: "Master volume down"
    $name:uk-UA: "Загальна гучність: тихіше"
    $name:pl-PL: "Głośność główna: ciszej"
    $name:de-DE: "Gesamtlautstärke: leiser"
    $name:fr-FR: "Volume principal −"
    $name:es-ES: "Volumen maestro: bajar"
    $name:ru-RU: "Общая громкость: тише"
  - hotkeyMasterMute: ""
    $name: "Master mute / unmute"
    $name:uk-UA: "Загальний звук: вимкнути / увімкнути"
    $name:pl-PL: "Wycisz wszystko / przywróć"
    $name:de-DE: "Alles stumm / laut"
    $name:fr-FR: "Couper / rétablir tout le son"
    $name:es-ES: "Silenciar / activar todo"
    $name:ru-RU: "Общий звук: выкл / вкл"
  $name: "⌨️ Hotkeys"
  $name:uk-UA: "⌨️ Гарячі клавіші"
  $name:pl-PL: "⌨️ Skróty klawiszowe"
  $name:de-DE: "⌨️ Tastenkürzel"
  $name:fr-FR: "⌨️ Raccourcis clavier"
  $name:es-ES: "⌨️ Atajos de teclado"
  $name:ru-RU: "⌨️ Горячие клавиши"
- appearance:
  - layoutMode: "modern_cards"
    $name: "Mixer layout"
    $name:uk-UA: "Стиль інтерфейсу мікшера"
    $name:pl-PL: "Układ miksera"
    $name:de-DE: "Mixer-Layout"
    $name:fr-FR: "Disposition du mixeur"
    $name:es-ES: "Diseño del mezclador"
    $name:ru-RU: "Стиль интерфейса микшера"
    $options:
    - modern_cards: "Modern Cards"
    - compact_strip: "Compact Strip"
    - hifi_meters: "Hi-Fi Studio Meters"
    - fluent_flyout: "Fluent Flyout (Windows 11)"
    - circular_hud: "Cyber Radial HUD (rings)"
    - studio_console: "Studio Mixing Desk"
    - minimal_pill: "Floating Minimal Pill"
    - cyberpunk_neon: "Cyberpunk Neon Grid"
    - retro_cassette: "Retro Cassette"
    - meter_bridge: "LED Meter Bridge"
    - arcade_panel: "Arcade Panel"
    - broadcast_console: "Broadcast Console (dual VU)"
    - control_center: "Control Center Pills"
    - glass_tiles: "Frosted Glass Tiles"
    - terminal_retro: "Retro Terminal"
    - neumorphic: "Soft Neumorphic"
    - vertical_dock: "Vertical Channel Strips"
    $options:uk-UA:
    - modern_cards: "Сучасні картки"
    - compact_strip: "Компактна смуга"
    - hifi_meters: "Студійні Hi-Fi шкали"
    - fluent_flyout: "Fluent-панель (Windows 11)"
    - circular_hud: "Кібер-HUD з кільцями"
    - studio_console: "Студійний мікшерний пульт"
    - minimal_pill: "Мінімалістичні капсули"
    - cyberpunk_neon: "Кіберпанк-неон"
    - retro_cassette: "Ретро-касета"
    - meter_bridge: "Світлодіодні індикатори рівня"
    - arcade_panel: "Аркадна панель"
    - broadcast_console: "Ефірний пульт (подвійні VU)"
    - control_center: "Капсули Control Center"
    - glass_tiles: "Плитки з матового скла"
    - terminal_retro: "Ретро-термінал"
    - neumorphic: "М'який неоморфізм"
    - vertical_dock: "Вертикальні фейдери"
    $options:pl-PL:
    - modern_cards: "Nowoczesne karty"
    - compact_strip: "Kompaktowy pasek"
    - hifi_meters: "Studyjne wskaźniki Hi-Fi"
    - fluent_flyout: "Panel Fluent (Windows 11)"
    - circular_hud: "Cyber HUD z pierścieniami"
    - studio_console: "Studyjna konsoleta"
    - minimal_pill: "Minimalistyczne kapsułki"
    - cyberpunk_neon: "Neon cyberpunk"
    - retro_cassette: "Kaseta retro"
    - meter_bridge: "Wskaźniki LED"
    - arcade_panel: "Panel arcade"
    - broadcast_console: "Konsola emisyjna (podwójne VU)"
    - control_center: "Kapsułki Control Center"
    - glass_tiles: "Kafelki z matowego szkła"
    - terminal_retro: "Terminal retro"
    - neumorphic: "Miękki neumorfizm"
    - vertical_dock: "Pionowe suwaki kanałów"
    $options:de-DE:
    - modern_cards: "Moderne Karten"
    - compact_strip: "Kompakte Leiste"
    - hifi_meters: "Hi-Fi-Studiometer"
    - fluent_flyout: "Fluent-Flyout (Windows 11)"
    - circular_hud: "Cyber-HUD mit Ringen"
    - studio_console: "Studio-Mischpult"
    - minimal_pill: "Minimalistische Pillen"
    - cyberpunk_neon: "Cyberpunk-Neon"
    - retro_cassette: "Retro-Kassette"
    - meter_bridge: "LED-Pegelanzeige"
    - arcade_panel: "Arcade-Panel"
    - broadcast_console: "Sendepult (Doppel-VU)"
    - control_center: "Control-Center-Pillen"
    - glass_tiles: "Milchglas-Kacheln"
    - terminal_retro: "Retro-Terminal"
    - neumorphic: "Weicher Neumorphismus"
    - vertical_dock: "Vertikale Kanalzüge"
    $options:fr-FR:
    - modern_cards: "Cartes modernes"
    - compact_strip: "Bande compacte"
    - hifi_meters: "Vu-mètres studio Hi-Fi"
    - fluent_flyout: "Volet Fluent (Windows 11)"
    - circular_hud: "HUD cyber à anneaux"
    - studio_console: "Table de mixage studio"
    - minimal_pill: "Pilules minimalistes"
    - cyberpunk_neon: "Néon cyberpunk"
    - retro_cassette: "Cassette rétro"
    - meter_bridge: "Vu-mètres LED"
    - arcade_panel: "Panneau arcade"
    - broadcast_console: "Console de diffusion (double VU)"
    - control_center: "Pilules Control Center"
    - glass_tiles: "Tuiles en verre dépoli"
    - terminal_retro: "Terminal rétro"
    - neumorphic: "Néomorphisme doux"
    - vertical_dock: "Tranches verticales"
    $options:es-ES:
    - modern_cards: "Tarjetas modernas"
    - compact_strip: "Tira compacta"
    - hifi_meters: "Medidores de estudio Hi-Fi"
    - fluent_flyout: "Panel Fluent (Windows 11)"
    - circular_hud: "HUD cyber con anillos"
    - studio_console: "Mesa de mezclas de estudio"
    - minimal_pill: "Píldoras minimalistas"
    - cyberpunk_neon: "Neón cyberpunk"
    - retro_cassette: "Casete retro"
    - meter_bridge: "Medidores LED"
    - arcade_panel: "Panel arcade"
    - broadcast_console: "Consola de emisión (doble VU)"
    - control_center: "Píldoras de Control Center"
    - glass_tiles: "Mosaicos de cristal esmerilado"
    - terminal_retro: "Terminal retro"
    - neumorphic: "Neumorfismo suave"
    - vertical_dock: "Canales verticales"
    $options:ru-RU:
    - modern_cards: "Современные карточки"
    - compact_strip: "Компактная полоса"
    - hifi_meters: "Студийные Hi-Fi шкалы"
    - fluent_flyout: "Fluent-панель (Windows 11)"
    - circular_hud: "Кибер-HUD с кольцами"
    - studio_console: "Студийный микшерный пульт"
    - minimal_pill: "Минималистичные капсулы"
    - cyberpunk_neon: "Киберпанк-неон"
    - retro_cassette: "Ретро-кассета"
    - meter_bridge: "Светодиодные индикаторы уровня"
    - arcade_panel: "Аркадная панель"
    - broadcast_console: "Эфирный пульт (двойные VU)"
    - control_center: "Капсулы Control Center"
    - glass_tiles: "Плитки из матового стекла"
    - terminal_retro: "Ретро-терминал"
    - neumorphic: "Мягкий неоморфизм"
    - vertical_dock: "Вертикальные фейдеры"
  - theme: "windows11"
    $name: "Color theme"
    $name:uk-UA: "Тема оформлення"
    $name:pl-PL: "Motyw kolorów"
    $name:de-DE: "Farbschema"
    $name:fr-FR: "Thème de couleurs"
    $name:es-ES: "Tema de color"
    $name:ru-RU: "Тема оформления"
    $options:
    - windows11: "Windows 11 Mica (Dark glass + accent)"
    - minimal: "Modern Dark"
    - light: "Light Fluent"
    - amoled: "AMOLED Black"
    - glass: "Frosted Glass"
    - accent: "System Accent"
    - classic: "Classic Blue"
    - dracula: "Dracula"
    - nord: "Nordic Frost"
    - neon: "Cyberpunk Neon"
    - retro: "Retro Amber"
    - tokyo_night: "Tokyo Night"
    - sunset: "Sunset"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Forest"
    - monochrome: "Monochrome (High contrast)"
    - ocean: "Deep Ocean"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized Dark"
    - matrix: "Matrix Green"
    - midnight_gold: "Midnight Gold"
    $options:uk-UA:
    - windows11: "Windows 11 Mica (темне скло + акцент)"
    - minimal: "Сучасна темна"
    - light: "Світла Fluent"
    - amoled: "AMOLED чорна"
    - glass: "Матове скло"
    - accent: "Системний акцент"
    - classic: "Класична синя"
    - dracula: "Dracula"
    - nord: "Північний іній"
    - neon: "Кіберпанк-неон"
    - retro: "Ретро-бурштин"
    - tokyo_night: "Tokyo Night"
    - sunset: "Захід сонця"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Ліс"
    - monochrome: "Монохром (висока контрастність)"
    - ocean: "Глибокий океан"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized темна"
    - matrix: "Зелена Матриця"
    - midnight_gold: "Опівнічне золото"
    $options:pl-PL:
    - windows11: "Windows 11 Mica (ciemne szkło + akcent)"
    - minimal: "Nowoczesny ciemny"
    - light: "Jasny Fluent"
    - amoled: "Czarny AMOLED"
    - glass: "Matowe szkło"
    - accent: "Akcent systemowy"
    - classic: "Klasyczny niebieski"
    - dracula: "Dracula"
    - nord: "Nordycki szron"
    - neon: "Neon cyberpunk"
    - retro: "Bursztyn retro"
    - tokyo_night: "Tokyo Night"
    - sunset: "Zachód słońca"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Las"
    - monochrome: "Monochromatyczny (wysoki kontrast)"
    - ocean: "Głęboki ocean"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized ciemny"
    - matrix: "Zielony Matrix"
    - midnight_gold: "Północne złoto"
    $options:de-DE:
    - windows11: "Windows 11 Mica (dunkles Glas + Akzent)"
    - minimal: "Modernes Dunkel"
    - light: "Helles Fluent"
    - amoled: "AMOLED-Schwarz"
    - glass: "Milchglas"
    - accent: "Systemakzent"
    - classic: "Klassisches Blau"
    - dracula: "Dracula"
    - nord: "Nordischer Frost"
    - neon: "Cyberpunk-Neon"
    - retro: "Retro-Bernstein"
    - tokyo_night: "Tokyo Night"
    - sunset: "Sonnenuntergang"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Wald"
    - monochrome: "Monochrom (hoher Kontrast)"
    - ocean: "Tiefsee"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized Dunkel"
    - matrix: "Matrix-Grün"
    - midnight_gold: "Mitternachtsgold"
    $options:fr-FR:
    - windows11: "Windows 11 Mica (verre sombre + accent)"
    - minimal: "Sombre moderne"
    - light: "Fluent clair"
    - amoled: "Noir AMOLED"
    - glass: "Verre dépoli"
    - accent: "Accent système"
    - classic: "Bleu classique"
    - dracula: "Dracula"
    - nord: "Givre nordique"
    - neon: "Néon cyberpunk"
    - retro: "Ambre rétro"
    - tokyo_night: "Tokyo Night"
    - sunset: "Coucher de soleil"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Forêt"
    - monochrome: "Monochrome (contraste élevé)"
    - ocean: "Océan profond"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized sombre"
    - matrix: "Vert Matrix"
    - midnight_gold: "Or de minuit"
    $options:es-ES:
    - windows11: "Windows 11 Mica (cristal oscuro + acento)"
    - minimal: "Oscuro moderno"
    - light: "Fluent claro"
    - amoled: "Negro AMOLED"
    - glass: "Cristal esmerilado"
    - accent: "Acento del sistema"
    - classic: "Azul clásico"
    - dracula: "Dracula"
    - nord: "Escarcha nórdica"
    - neon: "Neón cyberpunk"
    - retro: "Ámbar retro"
    - tokyo_night: "Tokyo Night"
    - sunset: "Atardecer"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Bosque"
    - monochrome: "Monocromo (alto contraste)"
    - ocean: "Océano profundo"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized oscuro"
    - matrix: "Verde Matrix"
    - midnight_gold: "Oro de medianoche"
    $options:ru-RU:
    - windows11: "Windows 11 Mica (тёмное стекло + акцент)"
    - minimal: "Современная тёмная"
    - light: "Светлая Fluent"
    - amoled: "AMOLED чёрная"
    - glass: "Матовое стекло"
    - accent: "Системный акцент"
    - classic: "Классическая синяя"
    - dracula: "Dracula"
    - nord: "Северный иней"
    - neon: "Киберпанк-неон"
    - retro: "Ретро-янтарь"
    - tokyo_night: "Tokyo Night"
    - sunset: "Закат"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Лес"
    - monochrome: "Монохром (высокая контрастность)"
    - ocean: "Глубокий океан"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized тёмная"
    - matrix: "Зелёная Матрица"
    - midnight_gold: "Полуночное золото"
  - scale: 100
    $name: "Interface scale (%)"
    $name:uk-UA: "Масштаб інтерфейсу (%)"
    $name:pl-PL: "Skala interfejsu (%)"
    $name:de-DE: "Oberflächenskalierung (%)"
    $name:fr-FR: "Échelle de l'interface (%)"
    $name:es-ES: "Escala de la interfaz (%)"
    $name:ru-RU: "Масштаб интерфейса (%)"
    $description: "30–300 %. Text, icons, sliders and click areas scale together. If the mixer would not fit on the screen it shrinks automatically."
    $description:uk-UA: "30–300 %. Текст, іконки, повзунки та області натискання масштабуються разом. Якщо мікшер не вміщається на екрані, він автоматично зменшується."
    $description:pl-PL: "30–300 %. Tekst, ikony, suwaki i obszary kliknięć skalują się razem. Jeśli mikser nie mieści się na ekranie, zostanie automatycznie zmniejszony."
    $description:de-DE: "30–300 %. Text, Symbole, Regler und Klickbereiche skalieren gemeinsam. Passt der Mixer nicht auf den Bildschirm, wird er automatisch verkleinert."
    $description:fr-FR: "30–300 %. Texte, icônes, curseurs et zones cliquables s'adaptent ensemble. Si le mixeur ne tient pas à l'écran, il est réduit automatiquement."
    $description:es-ES: "30–300 %. Texto, iconos, controles y zonas de clic se escalan juntos. Si el mezclador no cabe en la pantalla, se reduce automáticamente."
    $description:ru-RU: "30–300 %. Текст, значки, ползунки и области нажатия масштабируются вместе. Если микшер не помещается на экране, он автоматически уменьшается."
  - followDpi: true
    $name: "Follow Windows display scaling"
    $name:uk-UA: "Враховувати масштабування екрана Windows"
    $name:pl-PL: "Uwzględniaj skalowanie ekranu Windows"
    $name:de-DE: "Windows-Anzeigeskalierung berücksichtigen"
    $name:fr-FR: "Suivre la mise à l'échelle de Windows"
    $name:es-ES: "Seguir el escalado de pantalla de Windows"
    $name:ru-RU: "Учитывать масштабирование экрана Windows"
    $description: "Multiplies the scale by the monitor DPI (125 %, 150 %…), so the mixer has the same size on every display."
    $description:uk-UA: "Множить масштаб на DPI монітора (125 %, 150 %…), щоб мікшер мав однаковий розмір на всіх екранах."
    $description:pl-PL: "Mnoży skalę przez DPI monitora (125 %, 150 %…), aby mikser wyglądał tak samo na każdym ekranie."
    $description:de-DE: "Multipliziert die Skalierung mit der Monitor-DPI (125 %, 150 %…), damit der Mixer überall gleich groß ist."
    $description:fr-FR: "Multiplie l'échelle par le DPI du moniteur (125 %, 150 %…) pour une taille identique sur chaque écran."
    $description:es-ES: "Multiplica la escala por el DPI del monitor (125 %, 150 %…) para que el mezclador tenga el mismo tamaño en cada pantalla."
    $description:ru-RU: "Умножает масштаб на DPI монитора (125 %, 150 %…), чтобы микшер был одинакового размера на всех экранах."
  - width: 320
    $name: "Mixer width (px at 100 %)"
    $name:uk-UA: "Ширина мікшера (px при 100 %)"
    $name:pl-PL: "Szerokość miksera (px przy 100 %)"
    $name:de-DE: "Mixerbreite (px bei 100 %)"
    $name:fr-FR: "Largeur du mixeur (px à 100 %)"
    $name:es-ES: "Ancho del mezclador (px al 100 %)"
    $name:ru-RU: "Ширина микшера (px при 100 %)"
  - opacity: 100
    $name: "Window opacity (%)"
    $name:uk-UA: "Непрозорість вікна (%)"
    $name:pl-PL: "Krycie okna (%)"
    $name:de-DE: "Fensterdeckkraft (%)"
    $name:fr-FR: "Opacité de la fenêtre (%)"
    $name:es-ES: "Opacidad de la ventana (%)"
    $name:ru-RU: "Непрозрачность окна (%)"
  - animation: true
    $name: "Smooth animations"
    $name:uk-UA: "Плавні анімації"
    $name:pl-PL: "Płynne animacje"
    $name:de-DE: "Flüssige Animationen"
    $name:fr-FR: "Animations fluides"
    $name:es-ES: "Animaciones suaves"
    $name:ru-RU: "Плавные анимации"
  $name: "🎨 Appearance"
  $name:uk-UA: "🎨 Зовнішній вигляд"
  $name:pl-PL: "🎨 Wygląd"
  $name:de-DE: "🎨 Darstellung"
  $name:fr-FR: "🎨 Apparence"
  $name:es-ES: "🎨 Apariencia"
  $name:ru-RU: "🎨 Внешний вид"
- tray:
  - showTrayIcon: false
    $name: "Show tray icon [BETA]"
    $name:uk-UA: "Показувати іконку в треї (БЕТА)"
    $name:pl-PL: "Pokaż ikonę w zasobniku (BETA)"
    $name:de-DE: "Tray-Symbol anzeigen (BETA)"
    $name:fr-FR: "Afficher l'icône dans la zone de notification (BÊTA)"
    $name:es-ES: "Mostrar icono en la bandeja (BETA)"
    $name:ru-RU: "Показывать иконку в трее (БЕТА)"
    $description: "Add a volume icon to the system notification area. (BETA - experimental feature)."
    $description:uk-UA: "Додати іконку гучності в системний трей. (БЕТА - експериментальна функція)."
    $description:pl-PL: "Dodaje ikonę w zasobniku. (BETA - funkcja eksperymentalna)."
    $description:de-DE: "Symbol im Infobereich anzeigen. (BETA - experimentelle Funktion)."
    $description:fr-FR: "Ajouter l'icône dans la zone de notification. (BÊTA - fonctionnalité expérimentale)."
    $description:es-ES: "Añadir icono en la bandeja. (BETA - función experimental)."
    $description:ru-RU: "Добавить иконку в системный трей. (БЕТА - экспериментальная функция)."
  - trayIconStyle: "system"
    $name: "Tray icon style [BETA]"
    $name:uk-UA: "Стиль іконки в треї (БЕТА)"
    $name:pl-PL: "Styl ikony w zasobniku (BETA)"
    $name:de-DE: "Symbolstil (BETA)"
    $name:fr-FR: "Style de l'icône (BÊTA)"
    $name:es-ES: "Estilo del icono (BETA)"
    $name:ru-RU: "Стиль иконки в трее (БЕТА)"
    $options:
    - system: "Windows native icon"
    - speaker: "Fluent speaker"
    - faders: "Studio faders"
    - wave: "Spectrum"
    - headphones: "Headphones"
    - retro_knob: "Retro knob"
    - minimal_dot: "Status dot"
    - equalizer_bars: "Equalizer bars"
    - soundwave_circle: "Sonar pulse"
    $options:uk-UA:
    - system: "Системна іконка Windows"
    - speaker: "Динамік Fluent"
    - faders: "Студійні фейдери"
    - wave: "Спектр"
    - headphones: "Навушники"
    - retro_knob: "Ретро-ручка"
    - minimal_dot: "Індикатор-крапка"
    - equalizer_bars: "Смуги еквалайзера"
    - soundwave_circle: "Сонар"
    $options:pl-PL:
    - system: "Natywna ikona Windows"
    - speaker: "Głośnik Fluent"
    - faders: "Suwaki studyjne"
    - wave: "Widmo"
    - headphones: "Słuchawki"
    - retro_knob: "Pokrętło retro"
    - minimal_dot: "Kropka stanu"
    - equalizer_bars: "Paski korektora"
    - soundwave_circle: "Impuls sonaru"
    $options:de-DE:
    - system: "Natives Windows-Symbol"
    - speaker: "Fluent-Lautsprecher"
    - faders: "Studio-Fader"
    - wave: "Spektrum"
    - headphones: "Kopfhörer"
    - retro_knob: "Retro-Drehknopf"
    - minimal_dot: "Statuspunkt"
    - equalizer_bars: "Equalizer-Balken"
    - soundwave_circle: "Sonar-Impuls"
    $options:fr-FR:
    - system: "Icône native de Windows"
    - speaker: "Haut-parleur Fluent"
    - faders: "Faders de studio"
    - wave: "Spectre"
    - headphones: "Casque"
    - retro_knob: "Bouton rétro"
    - minimal_dot: "Point d'état"
    - equalizer_bars: "Barres d'égaliseur"
    - soundwave_circle: "Impulsion sonar"
    $options:es-ES:
    - system: "Icono nativo de Windows"
    - speaker: "Altavoz Fluent"
    - faders: "Faders de estudio"
    - wave: "Espectro"
    - headphones: "Auriculares"
    - retro_knob: "Perilla retro"
    - minimal_dot: "Punto de estado"
    - equalizer_bars: "Barras de ecualizador"
    - soundwave_circle: "Pulso de sonar"
    $options:ru-RU:
    - system: "Системная иконка Windows"
    - speaker: "Динамик Fluent"
    - faders: "Студийные фейдеры"
    - wave: "Спектр"
    - headphones: "Наушники"
    - retro_knob: "Ретро-ручка"
    - minimal_dot: "Точка-индикатор"
    - equalizer_bars: "Полосы эквалайзера"
    - soundwave_circle: "Сонар"
  - trayPopupStyle: "system_flyout"
    $name: "Popup shown on tray click [BETA]"
    $name:uk-UA: "Вигляд вікна при кліку в треї (БЕТА)"
    $name:pl-PL: "Okno po kliknięciu ikony (BETA)"
    $name:de-DE: "Fenster beim Klick auf das Symbol (BETA)"
    $name:fr-FR: "Fenêtre au clic sur l'icône (BÊTA)"
    $name:es-ES: "Ventana al hacer clic en el icono (BETA)"
    $name:ru-RU: "Окно при клике в трее (БЕТА)"
    $options:
    - system_flyout: "Windows 11 quick flyout (master + expandable apps)"
    - full_mixer: "Full mixer (main layout)"
    - compact_fader: "Compact fader strip"
    - mini_island: "Mini island"
    - radial_dial: "Radial dial"
    - vertical_dock: "Vertical channel strips"
    - control_center: "Control Center pills"
    $options:uk-UA:
    - system_flyout: "Швидка панель Windows 11 (загальна гучність + список програм)"
    - full_mixer: "Повний мікшер (основний стиль)"
    - compact_fader: "Компактна смуга фейдерів"
    - mini_island: "Міні-острівець"
    - radial_dial: "Радіальна ручка"
    - vertical_dock: "Вертикальні фейдери"
    - control_center: "Капсули Control Center"
    $options:pl-PL:
    - system_flyout: "Szybki panel Windows 11 (głośność główna + lista aplikacji)"
    - full_mixer: "Pełny mikser (główny układ)"
    - compact_fader: "Kompaktowy pasek suwaków"
    - mini_island: "Mini wyspa"
    - radial_dial: "Pokrętło radialne"
    - vertical_dock: "Pionowe suwaki kanałów"
    - control_center: "Kapsułki Control Center"
    $options:de-DE:
    - system_flyout: "Windows 11-Schnellflyout (Gesamtlautstärke + App-Liste)"
    - full_mixer: "Vollständiger Mixer (Hauptlayout)"
    - compact_fader: "Kompakte Faderleiste"
    - mini_island: "Mini-Insel"
    - radial_dial: "Drehregler"
    - vertical_dock: "Vertikale Kanalzüge"
    - control_center: "Control-Center-Pillen"
    $options:fr-FR:
    - system_flyout: "Volet rapide Windows 11 (volume principal + liste des applications)"
    - full_mixer: "Mixeur complet (disposition principale)"
    - compact_fader: "Bande de faders compacte"
    - mini_island: "Mini-îlot"
    - radial_dial: "Cadran radial"
    - vertical_dock: "Tranches verticales"
    - control_center: "Pilules Control Center"
    $options:es-ES:
    - system_flyout: "Panel rápido de Windows 11 (volumen maestro + lista de apps)"
    - full_mixer: "Mezclador completo (diseño principal)"
    - compact_fader: "Tira de faders compacta"
    - mini_island: "Mini isla"
    - radial_dial: "Dial radial"
    - vertical_dock: "Canales verticales"
    - control_center: "Píldoras de Control Center"
    $options:ru-RU:
    - system_flyout: "Быстрая панель Windows 11 (общая громкость + список приложений)"
    - full_mixer: "Полный микшер (основной стиль)"
    - compact_fader: "Компактная полоса фейдеров"
    - mini_island: "Мини-островок"
    - radial_dial: "Радиальная ручка"
    - vertical_dock: "Вертикальные фейдеры"
    - control_center: "Капсулы Control Center"
  - trayScrollVolume: true
    $name: "Mouse wheel over the icon changes volume [BETA]"
    $name:uk-UA: "Коліщатко миші над іконкою змінює гучність (БЕТА)"
    $name:pl-PL: "Kółko myszy nad ikoną zmienia głośność (BETA)"
    $name:de-DE: "Mausrad über dem Symbol ändert die Lautstärke (BETA)"
    $name:fr-FR: "La molette sur l'icône règle le volume (BÊTA)"
    $name:es-ES: "La rueda sobre el icono cambia el volumen (BETA)"
    $name:ru-RU: "Колесо мыши над иконкой меняет громкость (БЕТА)"
  $name: "🔔 Tray icon [BETA]"
  $name:uk-UA: "🔔 Іконка в треї (БЕТА)"
  $name:pl-PL: "🔔 Ikona w zasobniku (BETA)"
  $name:de-DE: "🔔 Tray-Symbol (BETA)"
  $name:fr-FR: "🔔 Icône de notification (BÊTA)"
  $name:es-ES: "🔔 Icono de la bandeja (BETA)"
  $name:ru-RU: "🔔 Иконка в трее (БЕТА)"
- position:
  - allowDragMove: false
    $name: "Allow moving the mixer with the mouse"
    $name:uk-UA: "Дозволити переміщення мікшера мишкою"
    $name:pl-PL: "Zezwól na przesuwanie miksera myszą"
    $name:de-DE: "Verschieben des Mixers mit der Maus erlauben"
    $name:fr-FR: "Autoriser le déplacement du mixeur à la souris"
    $name:es-ES: "Permitir mover el mezclador con el ratón"
    $name:ru-RU: "Разрешить перемещение микшера мышью"
    $description: "Drag an empty area or a device header. Off by default."
    $description:uk-UA: "Тягніть за порожню ділянку або заголовок пристрою. За замовчуванням вимкнено."
    $description:pl-PL: "Przeciągnij pusty obszar lub nagłówek urządzenia. Domyślnie wyłączone."
    $description:de-DE: "Leere Fläche oder Gerätekopf ziehen. Standardmäßig aus."
    $description:fr-FR: "Faites glisser une zone vide ou l'en-tête d'un périphérique. Désactivé par défaut."
    $description:es-ES: "Arrastra una zona vacía o el encabezado de un dispositivo. Desactivado por defecto."
    $description:ru-RU: "Тяните за пустую область или заголовок устройства. По умолчанию выключено."
  - rememberCustomPos: true
    $name: "Remember moved position"
    $name:uk-UA: "Запам'ятовувати переміщену позицію"
    $name:pl-PL: "Zapamiętaj przesuniętą pozycję"
    $name:de-DE: "Verschobene Position merken"
    $name:fr-FR: "Mémoriser la position"
    $name:es-ES: "Recordar la posición movida"
    $name:ru-RU: "Запоминать перемещённую позицию"
  - anchor: "bottom_right"
    $name: "Default position"
    $name:uk-UA: "Позиція за замовчуванням"
    $name:pl-PL: "Domyślna pozycja"
    $name:de-DE: "Standardposition"
    $name:fr-FR: "Position par défaut"
    $name:es-ES: "Posición predeterminada"
    $name:ru-RU: "Позиция по умолчанию"
    $options:
    - bottom_right: "Bottom right"
    - bottom_left: "Bottom left"
    - bottom_center: "Bottom center"
    - top_right: "Top right"
    - top_left: "Top left"
    - top_center: "Top center"
    - center: "Center"
    - tray: "Above the tray icon"
    $options:uk-UA:
    - bottom_right: "Праворуч унизу"
    - bottom_left: "Ліворуч унизу"
    - bottom_center: "Унизу по центру"
    - top_right: "Праворуч угорі"
    - top_left: "Ліворуч угорі"
    - top_center: "Угорі по центру"
    - center: "По центру"
    - tray: "Над іконкою в треї"
    $options:pl-PL:
    - bottom_right: "Prawy dolny róg"
    - bottom_left: "Lewy dolny róg"
    - bottom_center: "Na dole pośrodku"
    - top_right: "Prawy górny róg"
    - top_left: "Lewy górny róg"
    - top_center: "Na górze pośrodku"
    - center: "Na środku"
    - tray: "Nad ikoną w zasobniku"
    $options:de-DE:
    - bottom_right: "Unten rechts"
    - bottom_left: "Unten links"
    - bottom_center: "Unten mittig"
    - top_right: "Oben rechts"
    - top_left: "Oben links"
    - top_center: "Oben mittig"
    - center: "Mitte"
    - tray: "Über dem Tray-Symbol"
    $options:fr-FR:
    - bottom_right: "En bas à droite"
    - bottom_left: "En bas à gauche"
    - bottom_center: "En bas au centre"
    - top_right: "En haut à droite"
    - top_left: "En haut à gauche"
    - top_center: "En haut au centre"
    - center: "Centre"
    - tray: "Au-dessus de l'icône"
    $options:es-ES:
    - bottom_right: "Abajo a la derecha"
    - bottom_left: "Abajo a la izquierda"
    - bottom_center: "Abajo al centro"
    - top_right: "Arriba a la derecha"
    - top_left: "Arriba a la izquierda"
    - top_center: "Arriba al centro"
    - center: "Centro"
    - tray: "Sobre el icono de la bandeja"
    $options:ru-RU:
    - bottom_right: "Справа внизу"
    - bottom_left: "Слева внизу"
    - bottom_center: "Внизу по центру"
    - top_right: "Справа вверху"
    - top_left: "Слева вверху"
    - top_center: "Вверху по центру"
    - center: "По центру"
    - tray: "Над иконкой в трее"
  - monitor: "cursor"
    $name: "Monitor"
    $name:uk-UA: "Монітор"
    $name:pl-PL: "Monitor"
    $name:de-DE: "Monitor"
    $name:fr-FR: "Écran"
    $name:es-ES: "Monitor"
    $name:ru-RU: "Монитор"
    $options:
    - cursor: "Monitor with the cursor"
    - active: "Monitor with the active window"
    - primary: "Primary monitor"
    $options:uk-UA:
    - cursor: "Монітор з курсором"
    - active: "Монітор з активним вікном"
    - primary: "Основний монітор"
    $options:pl-PL:
    - cursor: "Monitor z kursorem"
    - active: "Monitor z aktywnym oknem"
    - primary: "Monitor główny"
    $options:de-DE:
    - cursor: "Monitor mit dem Mauszeiger"
    - active: "Monitor mit dem aktiven Fenster"
    - primary: "Hauptmonitor"
    $options:fr-FR:
    - cursor: "Écran du curseur"
    - active: "Écran de la fenêtre active"
    - primary: "Écran principal"
    $options:es-ES:
    - cursor: "Monitor con el cursor"
    - active: "Monitor con la ventana activa"
    - primary: "Monitor principal"
    $options:ru-RU:
    - cursor: "Монитор с курсором"
    - active: "Монитор с активным окном"
    - primary: "Основной монитор"
  - offsetX: 16
    $name: "Horizontal offset (px)"
    $name:uk-UA: "Відступ по горизонталі (px)"
    $name:pl-PL: "Przesunięcie poziome (px)"
    $name:de-DE: "Horizontaler Abstand (px)"
    $name:fr-FR: "Décalage horizontal (px)"
    $name:es-ES: "Desplazamiento horizontal (px)"
    $name:ru-RU: "Отступ по горизонтали (px)"
  - offsetY: 16
    $name: "Vertical offset (px)"
    $name:uk-UA: "Відступ по вертикалі (px)"
    $name:pl-PL: "Przesunięcie pionowe (px)"
    $name:de-DE: "Vertikaler Abstand (px)"
    $name:fr-FR: "Décalage vertical (px)"
    $name:es-ES: "Desplazamiento vertical (px)"
    $name:ru-RU: "Отступ по вертикали (px)"
  $name: "📐 Position"
  $name:uk-UA: "📐 Розташування"
  $name:pl-PL: "📐 Pozycja"
  $name:de-DE: "📐 Position"
  $name:fr-FR: "📐 Position"
  $name:es-ES: "📐 Posición"
  $name:ru-RU: "📐 Расположение"
- osd:
  - enableVolumeOsd: false
    $name: "Enable volume OSD (BETA)"
    $name:uk-UA: "Увімкнути OSD-індикатор (БЕТА)"
    $name:pl-PL: "Włącz OSD (BETA)"
    $name:de-DE: "Lautstärke-OSD aktivieren (BETA)"
    $name:fr-FR: "Activer l'OSD (BÊTA)"
    $name:es-ES: "Activar OSD (BETA)"
    $name:ru-RU: "Включить OSD-индикатор (БЕТА)"
  - replaceWindowsOsd: false
    $name: "Replace the Windows volume flyout"
    $name:uk-UA: "Замінити системний індикатор Windows"
    $name:pl-PL: "Zastąp systemowy wskaźnik Windows"
    $name:de-DE: "Windows-Lautstärkeanzeige ersetzen"
    $name:fr-FR: "Remplacer l'indicateur de Windows"
    $name:es-ES: "Reemplazar el indicador de Windows"
    $name:ru-RU: "Заменить системный индикатор Windows"
    $description: "Media volume keys and the mouse wheel over the taskbar notification area change the volume through this mod. For them and for other volume changes that bring up the Windows flyout (touchpad gestures, headset buttons), the mod's OSD is shown instead, even when the volume OSD above is off. Indicators of the manufacturer's own software (laptop or keyboard utilities) are not affected."
    $description:uk-UA: "Медіаклавіші гучності та колесо миші над областю сповіщень панелі завдань змінюють гучність через цей мод. Для них і для інших змін гучності, на які Windows показує свій індикатор (жести тачпада, кнопки навушників), замість нього показується OSD мода, навіть якщо OSD вище вимкнено. Не діє на індикатори програм виробника (утиліт ноутбука чи клавіатури)."
    $description:pl-PL: "Klawisze głośności i kółko myszy nad obszarem powiadomień paska zadań zmieniają głośność przez ten mod. Dla nich i dla innych zmian głośności, przy których Windows pokazuje swój wskaźnik (gesty touchpada, przyciski słuchawek), zamiast niego pokazywane jest OSD moda, nawet gdy OSD powyżej jest wyłączone. Nie dotyczy wskaźników oprogramowania producenta (narzędzi laptopa lub klawiatury)."
    $description:de-DE: "Medien-Lautstärketasten und das Mausrad über dem Infobereich der Taskleiste ändern die Lautstärke über diesen Mod. Dafür und für andere Lautstärkeänderungen, bei denen Windows seine Anzeige einblendet (Touchpad-Gesten, Headset-Tasten), erscheint stattdessen das OSD des Mods, auch wenn das OSD oben aus ist. Anzeigen von Herstellersoftware (Laptop- oder Tastatur-Tools) sind nicht betroffen."
    $description:fr-FR: "Les touches de volume et la molette au-dessus de la zone de notification de la barre des tâches règlent le volume via ce mod. Pour elles et pour les autres changements de volume qui font apparaître l'indicateur de Windows (gestes du pavé tactile, boutons du casque), l'OSD du mod s'affiche à la place, même si l'OSD ci-dessus est désactivé. Sans effet sur les indicateurs des logiciels du fabricant (outils du portable ou du clavier)."
    $description:es-ES: "Las teclas de volumen y la rueda del ratón sobre el área de notificación de la barra de tareas cambian el volumen mediante este mod. Para ellas y para otros cambios de volumen que muestran el indicador de Windows (gestos del panel táctil, botones de los auriculares), se muestra en su lugar el OSD del mod, aunque el OSD de arriba esté desactivado. No afecta a los indicadores del software del fabricante (utilidades del portátil o del teclado)."
    $description:ru-RU: "Медиаклавиши громкости и колесо мыши над областью уведомлений панели задач меняют громкость через этот мод. Для них и для других изменений громкости, на которые Windows показывает свой индикатор (жесты тачпада, кнопки наушников), вместо него показывается OSD мода, даже если OSD выше выключен. Не действует на индикаторы программ производителя (утилит ноутбука или клавиатуры)."
  - osdStyle: "capsule"
    $name: "OSD style"
    $name:uk-UA: "Стиль OSD"
    $name:pl-PL: "Styl OSD"
    $name:de-DE: "OSD-Stil"
    $name:fr-FR: "Style de l'OSD"
    $name:es-ES: "Estilo del OSD"
    $name:ru-RU: "Стиль OSD"
    $options:
    - capsule: "Windows 11 capsule"
    - compact_pill: "Compact pill"
    - detailed: "Detailed (name + bar)"
    - ring: "Circular ring"
    - vertical_bar: "Vertical edge bar"
    - glass_morphism: "Frosted glass"
    - game_hud_compact: "Tactical game HUD"
    - stealth_dot: "Stealth dot matrix"
    - pill_slider: "Control Center pill"
    $options:uk-UA:
    - capsule: "Капсула Windows 11"
    - compact_pill: "Компактна капсула"
    - detailed: "Детальний (назва + шкала)"
    - ring: "Кругове кільце"
    - vertical_bar: "Вертикальна бічна шкала"
    - glass_morphism: "Матове скло"
    - game_hud_compact: "Тактичний ігровий HUD"
    - stealth_dot: "Непомітна точкова матриця"
    - pill_slider: "Капсула Control Center"
    $options:pl-PL:
    - capsule: "Kapsułka Windows 11"
    - compact_pill: "Kompaktowa kapsułka"
    - detailed: "Szczegółowy (nazwa + pasek)"
    - ring: "Okrągły pierścień"
    - vertical_bar: "Pionowy pasek boczny"
    - glass_morphism: "Matowe szkło"
    - game_hud_compact: "Taktyczny HUD gry"
    - stealth_dot: "Dyskretna matryca punktów"
    - pill_slider: "Kapsułka Control Center"
    $options:de-DE:
    - capsule: "Windows 11-Kapsel"
    - compact_pill: "Kompakte Pille"
    - detailed: "Detailliert (Name + Balken)"
    - ring: "Kreisring"
    - vertical_bar: "Vertikaler Randbalken"
    - glass_morphism: "Milchglas"
    - game_hud_compact: "Taktisches Spiel-HUD"
    - stealth_dot: "Unauffällige Punktmatrix"
    - pill_slider: "Control-Center-Pille"
    $options:fr-FR:
    - capsule: "Capsule Windows 11"
    - compact_pill: "Pilule compacte"
    - detailed: "Détaillé (nom + barre)"
    - ring: "Anneau circulaire"
    - vertical_bar: "Barre verticale latérale"
    - glass_morphism: "Verre dépoli"
    - game_hud_compact: "HUD de jeu tactique"
    - stealth_dot: "Matrice de points discrète"
    - pill_slider: "Pilule Control Center"
    $options:es-ES:
    - capsule: "Cápsula de Windows 11"
    - compact_pill: "Píldora compacta"
    - detailed: "Detallado (nombre + barra)"
    - ring: "Anillo circular"
    - vertical_bar: "Barra vertical lateral"
    - glass_morphism: "Cristal esmerilado"
    - game_hud_compact: "HUD táctico de juego"
    - stealth_dot: "Matriz de puntos discreta"
    - pill_slider: "Píldora de Control Center"
    $options:ru-RU:
    - capsule: "Капсула Windows 11"
    - compact_pill: "Компактная капсула"
    - detailed: "Подробный (название + шкала)"
    - ring: "Круговое кольцо"
    - vertical_bar: "Вертикальная боковая шкала"
    - glass_morphism: "Матовое стекло"
    - game_hud_compact: "Тактический игровой HUD"
    - stealth_dot: "Незаметная точечная матрица"
    - pill_slider: "Капсула Control Center"
  - osdTheme: "inherit"
    $name: "OSD theme"
    $name:uk-UA: "Тема OSD"
    $name:pl-PL: "Motyw OSD"
    $name:de-DE: "OSD-Farbschema"
    $name:fr-FR: "Thème de l'OSD"
    $name:es-ES: "Tema del OSD"
    $name:ru-RU: "Тема OSD"
    $options:
    - inherit: "Same as the mixer"
    - windows11: "Windows 11 Mica (Dark glass + accent)"
    - minimal: "Modern Dark"
    - light: "Light Fluent"
    - amoled: "AMOLED Black"
    - glass: "Frosted Glass"
    - accent: "System Accent"
    - classic: "Classic Blue"
    - dracula: "Dracula"
    - nord: "Nordic Frost"
    - neon: "Cyberpunk Neon"
    - retro: "Retro Amber"
    - tokyo_night: "Tokyo Night"
    - sunset: "Sunset"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Forest"
    - monochrome: "Monochrome (High contrast)"
    - ocean: "Deep Ocean"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized Dark"
    - matrix: "Matrix Green"
    - midnight_gold: "Midnight Gold"
    $options:uk-UA:
    - inherit: "Як у мікшера"
    - windows11: "Windows 11 Mica (темне скло + акцент)"
    - minimal: "Сучасна темна"
    - light: "Світла Fluent"
    - amoled: "AMOLED чорна"
    - glass: "Матове скло"
    - accent: "Системний акцент"
    - classic: "Класична синя"
    - dracula: "Dracula"
    - nord: "Північний іній"
    - neon: "Кіберпанк-неон"
    - retro: "Ретро-бурштин"
    - tokyo_night: "Tokyo Night"
    - sunset: "Захід сонця"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Ліс"
    - monochrome: "Монохром (висока контрастність)"
    - ocean: "Глибокий океан"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized темна"
    - matrix: "Зелена Матриця"
    - midnight_gold: "Опівнічне золото"
    $options:pl-PL:
    - inherit: "Jak w mikserze"
    - windows11: "Windows 11 Mica (ciemne szkło + akcent)"
    - minimal: "Nowoczesny ciemny"
    - light: "Jasny Fluent"
    - amoled: "Czarny AMOLED"
    - glass: "Matowe szkło"
    - accent: "Akcent systemowy"
    - classic: "Klasyczny niebieski"
    - dracula: "Dracula"
    - nord: "Nordycki szron"
    - neon: "Neon cyberpunk"
    - retro: "Bursztyn retro"
    - tokyo_night: "Tokyo Night"
    - sunset: "Zachód słońca"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Las"
    - monochrome: "Monochromatyczny (wysoki kontrast)"
    - ocean: "Głęboki ocean"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized ciemny"
    - matrix: "Zielony Matrix"
    - midnight_gold: "Północne złoto"
    $options:de-DE:
    - inherit: "Wie der Mixer"
    - windows11: "Windows 11 Mica (dunkles Glas + Akzent)"
    - minimal: "Modernes Dunkel"
    - light: "Helles Fluent"
    - amoled: "AMOLED-Schwarz"
    - glass: "Milchglas"
    - accent: "Systemakzent"
    - classic: "Klassisches Blau"
    - dracula: "Dracula"
    - nord: "Nordischer Frost"
    - neon: "Cyberpunk-Neon"
    - retro: "Retro-Bernstein"
    - tokyo_night: "Tokyo Night"
    - sunset: "Sonnenuntergang"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Wald"
    - monochrome: "Monochrom (hoher Kontrast)"
    - ocean: "Tiefsee"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized Dunkel"
    - matrix: "Matrix-Grün"
    - midnight_gold: "Mitternachtsgold"
    $options:fr-FR:
    - inherit: "Comme le mixeur"
    - windows11: "Windows 11 Mica (verre sombre + accent)"
    - minimal: "Sombre moderne"
    - light: "Fluent clair"
    - amoled: "Noir AMOLED"
    - glass: "Verre dépoli"
    - accent: "Accent système"
    - classic: "Bleu classique"
    - dracula: "Dracula"
    - nord: "Givre nordique"
    - neon: "Néon cyberpunk"
    - retro: "Ambre rétro"
    - tokyo_night: "Tokyo Night"
    - sunset: "Coucher de soleil"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Forêt"
    - monochrome: "Monochrome (contraste élevé)"
    - ocean: "Océan profond"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized sombre"
    - matrix: "Vert Matrix"
    - midnight_gold: "Or de minuit"
    $options:es-ES:
    - inherit: "Igual que el mezclador"
    - windows11: "Windows 11 Mica (cristal oscuro + acento)"
    - minimal: "Oscuro moderno"
    - light: "Fluent claro"
    - amoled: "Negro AMOLED"
    - glass: "Cristal esmerilado"
    - accent: "Acento del sistema"
    - classic: "Azul clásico"
    - dracula: "Dracula"
    - nord: "Escarcha nórdica"
    - neon: "Neón cyberpunk"
    - retro: "Ámbar retro"
    - tokyo_night: "Tokyo Night"
    - sunset: "Atardecer"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Bosque"
    - monochrome: "Monocromo (alto contraste)"
    - ocean: "Océano profundo"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized oscuro"
    - matrix: "Verde Matrix"
    - midnight_gold: "Oro de medianoche"
    $options:ru-RU:
    - inherit: "Как у микшера"
    - windows11: "Windows 11 Mica (тёмное стекло + акцент)"
    - minimal: "Современная тёмная"
    - light: "Светлая Fluent"
    - amoled: "AMOLED чёрная"
    - glass: "Матовое стекло"
    - accent: "Системный акцент"
    - classic: "Классическая синяя"
    - dracula: "Dracula"
    - nord: "Северный иней"
    - neon: "Киберпанк-неон"
    - retro: "Ретро-янтарь"
    - tokyo_night: "Tokyo Night"
    - sunset: "Закат"
    - catppuccin: "Catppuccin Mocha"
    - forest: "Лес"
    - monochrome: "Монохром (высокая контрастность)"
    - ocean: "Глубокий океан"
    - rose_pine: "Rosé Pine"
    - gruvbox: "Gruvbox"
    - solarized: "Solarized тёмная"
    - matrix: "Зелёная Матрица"
    - midnight_gold: "Полуночное золото"
  - osdPosition: "top_center"
    $name: "OSD position"
    $name:uk-UA: "Розташування OSD"
    $name:pl-PL: "Pozycja OSD"
    $name:de-DE: "OSD-Position"
    $name:fr-FR: "Position de l'OSD"
    $name:es-ES: "Posición del OSD"
    $name:ru-RU: "Расположение OSD"
    $options:
    - top_center: "Top center"
    - bottom_center: "Bottom center"
    - top_right: "Top right"
    - top_left: "Top left"
    - bottom_right: "Bottom right"
    - bottom_left: "Bottom left"
    - left_center: "Left edge"
    - right_center: "Right edge"
    - center: "Center"
    $options:uk-UA:
    - top_center: "Угорі по центру"
    - bottom_center: "Унизу по центру"
    - top_right: "Праворуч угорі"
    - top_left: "Ліворуч угорі"
    - bottom_right: "Праворуч унизу"
    - bottom_left: "Ліворуч унизу"
    - left_center: "Лівий край"
    - right_center: "Правий край"
    - center: "По центру"
    $options:pl-PL:
    - top_center: "Na górze pośrodku"
    - bottom_center: "Na dole pośrodku"
    - top_right: "Prawy górny róg"
    - top_left: "Lewy górny róg"
    - bottom_right: "Prawy dolny róg"
    - bottom_left: "Lewy dolny róg"
    - left_center: "Lewa krawędź"
    - right_center: "Prawa krawędź"
    - center: "Na środku"
    $options:de-DE:
    - top_center: "Oben mittig"
    - bottom_center: "Unten mittig"
    - top_right: "Oben rechts"
    - top_left: "Oben links"
    - bottom_right: "Unten rechts"
    - bottom_left: "Unten links"
    - left_center: "Linker Rand"
    - right_center: "Rechter Rand"
    - center: "Mitte"
    $options:fr-FR:
    - top_center: "En haut au centre"
    - bottom_center: "En bas au centre"
    - top_right: "En haut à droite"
    - top_left: "En haut à gauche"
    - bottom_right: "En bas à droite"
    - bottom_left: "En bas à gauche"
    - left_center: "Bord gauche"
    - right_center: "Bord droit"
    - center: "Centre"
    $options:es-ES:
    - top_center: "Arriba al centro"
    - bottom_center: "Abajo al centro"
    - top_right: "Arriba a la derecha"
    - top_left: "Arriba a la izquierda"
    - bottom_right: "Abajo a la derecha"
    - bottom_left: "Abajo a la izquierda"
    - left_center: "Borde izquierdo"
    - right_center: "Borde derecho"
    - center: "Centro"
    $options:ru-RU:
    - top_center: "Вверху по центру"
    - bottom_center: "Внизу по центру"
    - top_right: "Справа вверху"
    - top_left: "Слева вверху"
    - bottom_right: "Справа внизу"
    - bottom_left: "Слева внизу"
    - left_center: "Левый край"
    - right_center: "Правый край"
    - center: "По центру"
  - osdMonitor: "cursor"
    $name: "OSD monitor"
    $name:uk-UA: "Монітор для OSD"
    $name:pl-PL: "Monitor OSD"
    $name:de-DE: "OSD-Monitor"
    $name:fr-FR: "Écran de l'OSD"
    $name:es-ES: "Monitor del OSD"
    $name:ru-RU: "Монитор для OSD"
    $options:
    - cursor: "Monitor with the cursor"
    - active: "Monitor with the active app"
    - primary: "Primary monitor"
    $options:uk-UA:
    - cursor: "Монітор з курсором"
    - active: "Монітор з активною програмою"
    - primary: "Основний монітор"
    $options:pl-PL:
    - cursor: "Monitor z kursorem"
    - active: "Monitor z aktywną aplikacją"
    - primary: "Monitor główny"
    $options:de-DE:
    - cursor: "Monitor mit dem Mauszeiger"
    - active: "Monitor mit der aktiven App"
    - primary: "Hauptmonitor"
    $options:fr-FR:
    - cursor: "Écran du curseur"
    - active: "Écran de l'application active"
    - primary: "Écran principal"
    $options:es-ES:
    - cursor: "Monitor con el cursor"
    - active: "Monitor con la app activa"
    - primary: "Monitor principal"
    $options:ru-RU:
    - cursor: "Монитор с курсором"
    - active: "Монитор с активным приложением"
    - primary: "Основной монитор"
  - osdOffsetX: 0
    $name: "OSD horizontal offset (px)"
    $name:uk-UA: "Відступ OSD по горизонталі (px)"
    $name:pl-PL: "Przesunięcie poziome OSD (px)"
    $name:de-DE: "Horizontaler OSD-Abstand (px)"
    $name:fr-FR: "Décalage horizontal de l'OSD (px)"
    $name:es-ES: "Desplazamiento horizontal del OSD (px)"
    $name:ru-RU: "Отступ OSD по горизонтали (px)"
  - osdOffsetY: 24
    $name: "OSD vertical offset (px)"
    $name:uk-UA: "Відступ OSD по вертикалі (px)"
    $name:pl-PL: "Przesunięcie pionowe OSD (px)"
    $name:de-DE: "Vertikaler OSD-Abstand (px)"
    $name:fr-FR: "Décalage vertical de l'OSD (px)"
    $name:es-ES: "Desplazamiento vertical del OSD (px)"
    $name:ru-RU: "Отступ OSD по вертикали (px)"
  - osdWidth: 260
    $name: "OSD width (px at 100 %)"
    $name:uk-UA: "Ширина OSD (px при 100 %)"
    $name:pl-PL: "Szerokość OSD (px przy 100 %)"
    $name:de-DE: "OSD-Breite (px bei 100 %)"
    $name:fr-FR: "Largeur de l'OSD (px à 100 %)"
    $name:es-ES: "Ancho del OSD (px al 100 %)"
    $name:ru-RU: "Ширина OSD (px при 100 %)"
  - osdScale: 100
    $name: "OSD scale (%)"
    $name:uk-UA: "Масштаб OSD (%)"
    $name:pl-PL: "Skala OSD (%)"
    $name:de-DE: "OSD-Skalierung (%)"
    $name:fr-FR: "Échelle de l'OSD (%)"
    $name:es-ES: "Escala del OSD (%)"
    $name:ru-RU: "Масштаб OSD (%)"
  - osdOpacity: 95
    $name: "OSD opacity (%)"
    $name:uk-UA: "Непрозорість OSD (%)"
    $name:pl-PL: "Krycie OSD (%)"
    $name:de-DE: "OSD-Deckkraft (%)"
    $name:fr-FR: "Opacité de l'OSD (%)"
    $name:es-ES: "Opacidad del OSD (%)"
    $name:ru-RU: "Непрозрачность OSD (%)"
  - osdAllowDrag: false
    $name: "Allow moving the OSD with the mouse (BETA)"
    $name:uk-UA: "Дозволити переміщення OSD мишкою (БЕТА)"
    $name:pl-PL: "Zezwól na przesuwanie OSD myszą (BETA)"
    $name:de-DE: "Verschieben des OSD mit der Maus erlauben (BETA)"
    $name:fr-FR: "Autoriser le déplacement de l'OSD (BÊTA)"
    $name:es-ES: "Permitir mover el OSD con el ratón (BETA)"
    $name:ru-RU: "Разрешить перемещение OSD мышью (БЕТА)"
    $description: "After saving, a preview OSD appears for 10 s so you can drag it. Turn off again to lock the position."
    $description:uk-UA: "Після збереження на 10 с з'явиться OSD, який можна перетягнути. Вимкніть опцію, щоб зафіксувати позицію."
    $description:pl-PL: "Po zapisaniu przez 10 s pojawi się podgląd OSD do przeciągnięcia. Wyłącz, aby zablokować pozycję."
    $description:de-DE: "Nach dem Speichern erscheint 10 s lang eine OSD-Vorschau zum Ziehen. Wieder ausschalten, um die Position zu sperren."
    $description:fr-FR: "Après l'enregistrement, un aperçu de l'OSD s'affiche 10 s pour le déplacer. Désactivez pour verrouiller la position."
    $description:es-ES: "Tras guardar, aparece una vista previa del OSD durante 10 s para arrastrarla. Desactívalo para fijar la posición."
    $description:ru-RU: "После сохранения на 10 с появится OSD, который можно перетащить. Выключите опцию, чтобы зафиксировать позицию."
  - osdRememberPos: true
    $name: "Remember OSD position"
    $name:uk-UA: "Запам'ятовувати позицію OSD"
    $name:pl-PL: "Zapamiętaj pozycję OSD"
    $name:de-DE: "OSD-Position merken"
    $name:fr-FR: "Mémoriser la position de l'OSD"
    $name:es-ES: "Recordar la posición del OSD"
    $name:ru-RU: "Запоминать позицию OSD"
  - osdTimeoutMs: 1600
    $name: "OSD display time (ms)"
    $name:uk-UA: "Час показу OSD (мс)"
    $name:pl-PL: "Czas wyświetlania OSD (ms)"
    $name:de-DE: "OSD-Anzeigedauer (ms)"
    $name:fr-FR: "Durée d'affichage de l'OSD (ms)"
    $name:es-ES: "Tiempo de visualización del OSD (ms)"
    $name:ru-RU: "Время показа OSD (мс)"
  $name: "✨ Volume OSD (BETA)"
  $name:uk-UA: "✨ OSD-індикатор гучності (БЕТА)"
  $name:pl-PL: "✨ Wskaźnik OSD (BETA)"
  $name:de-DE: "✨ Lautstärke-OSD (BETA)"
  $name:fr-FR: "✨ OSD du volume (BÊTA)"
  $name:es-ES: "✨ OSD de volumen (BETA)"
  $name:ru-RU: "✨ OSD-индикатор громкости (БЕТА)"
- deafen:
  - enableDeafenBackground: false
    $name: "Enable Deafen background (BETA)"
    $name:uk-UA: "Увімкнути Deafen Background (БЕТА)"
    $name:pl-PL: "Włącz Deafen Background (BETA)"
    $name:de-DE: "Deafen Background aktivieren (BETA)"
    $name:fr-FR: "Activer Deafen Background (BÊTA)"
    $name:es-ES: "Activar Deafen Background (BETA)"
    $name:ru-RU: "Включить Deafen Background (БЕТА)"
    $description: "The hotkey mutes or ducks every app except the focused one and the whitelist. Press again to restore."
    $description:uk-UA: "Гаряча клавіша вимикає або приглушує всі програми, крім активної та білого списку. Повторне натискання відновлює звук."
    $description:pl-PL: "Skrót wycisza lub przycisza wszystkie aplikacje poza aktywną i białą listą. Ponowne naciśnięcie przywraca dźwięk."
    $description:de-DE: "Das Tastenkürzel schaltet alle Apps außer der aktiven und der Whitelist stumm oder leiser. Erneut drücken stellt wieder her."
    $description:fr-FR: "Le raccourci coupe ou atténue toutes les applications sauf celle au premier plan et la liste blanche. Appuyez à nouveau pour rétablir."
    $description:es-ES: "El atajo silencia o atenúa todas las apps excepto la activa y la lista blanca. Púlsalo de nuevo para restaurar."
    $description:ru-RU: "Горячая клавиша выключает или приглушает все приложения, кроме активного и белого списка. Повторное нажатие восстанавливает звук."
  - hotkeyDeafen: ""
    $name: "Deafen hotkey"
    $name:uk-UA: "Гаряча клавіша Deafen"
    $name:pl-PL: "Skrót Deafen"
    $name:de-DE: "Deafen-Tastenkürzel"
    $name:fr-FR: "Raccourci Deafen"
    $name:es-ES: "Atajo Deafen"
    $name:ru-RU: "Горячая клавиша Deafen"
  - deafenMode: "mute"
    $name: "Deafen mode"
    $name:uk-UA: "Режим Deafen"
    $name:pl-PL: "Tryb Deafen"
    $name:de-DE: "Deafen-Modus"
    $name:fr-FR: "Mode Deafen"
    $name:es-ES: "Modo Deafen"
    $name:ru-RU: "Режим Deafen"
    $options:
    - mute: "Full mute"
    - duck: "Duck to a percentage"
    $options:uk-UA:
    - mute: "Повне вимкнення звуку"
    - duck: "Приглушити до відсотка"
    $options:pl-PL:
    - mute: "Pełne wyciszenie"
    - duck: "Przycisz do wartości procentowej"
    $options:de-DE:
    - mute: "Vollständig stumm"
    - duck: "Auf Prozentwert absenken"
    $options:fr-FR:
    - mute: "Coupure totale"
    - duck: "Atténuer à un pourcentage"
    $options:es-ES:
    - mute: "Silencio total"
    - duck: "Atenuar a un porcentaje"
    $options:ru-RU:
    - mute: "Полное отключение звука"
    - duck: "Приглушить до процента"
  - deafenDuckPercent: 20
    $name: "Ducking level (%)"
    $name:uk-UA: "Рівень приглушення (%)"
    $name:pl-PL: "Poziom przyciszenia (%)"
    $name:de-DE: "Absenkungspegel (%)"
    $name:fr-FR: "Niveau d'atténuation (%)"
    $name:es-ES: "Nivel de atenuación (%)"
    $name:ru-RU: "Уровень приглушения (%)"
  - deafenWhitelist: "discord.exe, ts3client_win64.exe, obs64.exe, spotify.exe"
    $name: "Never mute these apps (comma-separated)"
    $name:uk-UA: "Ніколи не глушити ці програми (через кому)"
    $name:pl-PL: "Nigdy nie wyciszaj tych aplikacji (po przecinku)"
    $name:de-DE: "Diese Apps nie stummschalten (kommagetrennt)"
    $name:fr-FR: "Ne jamais couper ces applications (séparées par des virgules)"
    $name:es-ES: "Nunca silenciar estas apps (separadas por comas)"
    $name:ru-RU: "Никогда не глушить эти приложения (через запятую)"
    $description: "File names such as discord.exe, or a folder path to spare every app inside it."
    $description:uk-UA: "Імена файлів, наприклад discord.exe, або шлях до папки, щоб не глушити всі програми в ній."
    $description:pl-PL: "Nazwy plików, np. discord.exe, lub ścieżka folderu, aby pominąć wszystkie aplikacje w nim."
    $description:de-DE: "Dateinamen wie discord.exe oder ein Ordnerpfad, um alle Apps darin auszunehmen."
    $description:fr-FR: "Noms de fichiers comme discord.exe, ou un chemin de dossier pour épargner toutes les applications qu'il contient."
    $description:es-ES: "Nombres de archivo como discord.exe, o la ruta de una carpeta para excluir todas sus apps."
    $description:ru-RU: "Имена файлов, например discord.exe, или путь к папке, чтобы не глушить все приложения в ней."
  $name: "🔇 Deafen background (BETA)"
  $name:uk-UA: "🔇 Глушіння фону Deafen (БЕТА)"
  $name:pl-PL: "🔇 Wyciszanie tła Deafen (BETA)"
  $name:de-DE: "🔇 Hintergrund stumm (BETA)"
  $name:fr-FR: "🔇 Sourdine de l'arrière-plan (BÊTA)"
  $name:es-ES: "🔇 Silenciar fondo (BETA)"
  $name:ru-RU: "🔇 Глушение фона Deafen (БЕТА)"
- behavior:
  - vuMeters: true
    $name: "Live VU meters"
    $name:uk-UA: "Живі VU-індикатори"
    $name:pl-PL: "Wskaźniki VU na żywo"
    $name:de-DE: "Live-VU-Meter"
    $name:fr-FR: "Vu-mètres en direct"
    $name:es-ES: "Medidores VU en vivo"
    $name:ru-RU: "Живые VU-индикаторы"
  - showAppNames: true
    $name: "Show app names"
    $name:uk-UA: "Показувати назви програм"
    $name:pl-PL: "Pokaż nazwy aplikacji"
    $name:de-DE: "App-Namen anzeigen"
    $name:fr-FR: "Afficher les noms des applications"
    $name:es-ES: "Mostrar nombres de apps"
    $name:ru-RU: "Показывать названия приложений"
  - showAllDevices: true
    $name: "Show all output devices"
    $name:uk-UA: "Показувати всі пристрої виводу"
    $name:pl-PL: "Pokaż wszystkie urządzenia wyjściowe"
    $name:de-DE: "Alle Ausgabegeräte anzeigen"
    $name:fr-FR: "Afficher tous les périphériques de sortie"
    $name:es-ES: "Mostrar todos los dispositivos de salida"
    $name:ru-RU: "Показывать все устройства вывода"
  - mergeApps: true
    $name: "Merge processes of the same app"
    $name:uk-UA: "Об'єднувати процеси однієї програми"
    $name:pl-PL: "Łącz procesy tej samej aplikacji"
    $name:de-DE: "Prozesse derselben App zusammenfassen"
    $name:fr-FR: "Fusionner les processus d'une même application"
    $name:es-ES: "Combinar procesos de la misma app"
    $name:ru-RU: "Объединять процессы одного приложения"
  - hideInactive: false
    $name: "Hide apps that are not playing"
    $name:uk-UA: "Приховувати програми без звуку"
    $name:pl-PL: "Ukryj aplikacje, które nie odtwarzają"
    $name:de-DE: "Nicht abspielende Apps ausblenden"
    $name:fr-FR: "Masquer les applications silencieuses"
    $name:es-ES: "Ocultar apps que no reproducen"
    $name:ru-RU: "Скрывать приложения без звука"
  - wheelStep: 2
    $name: "Volume step (%)"
    $name:uk-UA: "Крок гучності (%)"
    $name:pl-PL: "Krok głośności (%)"
    $name:de-DE: "Lautstärkeschritt (%)"
    $name:fr-FR: "Pas du volume (%)"
    $name:es-ES: "Paso de volumen (%)"
    $name:ru-RU: "Шаг громкости (%)"
    $description: "Used by the mouse wheel, hotkeys and media keys."
    $description:uk-UA: "Для коліщатка миші, гарячих клавіш і медіаклавіш."
    $description:pl-PL: "Dla kółka myszy, skrótów i klawiszy multimedialnych."
    $description:de-DE: "Für Mausrad, Tastenkürzel und Medientasten."
    $description:fr-FR: "Pour la molette, les raccourcis et les touches multimédias."
    $description:es-ES: "Para la rueda del ratón, los atajos y las teclas multimedia."
    $description:ru-RU: "Для колеса мыши, горячих клавиш и медиаклавиш."
  - autoHide: 0
    $name: "Auto-hide after (s, 0 = off)"
    $name:uk-UA: "Автозакриття через (с, 0 = вимк.)"
    $name:pl-PL: "Autoukrywanie po (s, 0 = wył.)"
    $name:de-DE: "Automatisch ausblenden nach (s, 0 = aus)"
    $name:fr-FR: "Masquer automatiquement après (s, 0 = désactivé)"
    $name:es-ES: "Ocultar automáticamente tras (s, 0 = no)"
    $name:ru-RU: "Автоскрытие через (с, 0 = выкл.)"
  $name: "⚙️ Behavior"
  $name:uk-UA: "⚙️ Поведінка"
  $name:pl-PL: "⚙️ Zachowanie"
  $name:de-DE: "⚙️ Verhalten"
  $name:fr-FR: "⚙️ Comportement"
  $name:es-ES: "⚙️ Comportamiento"
  $name:ru-RU: "⚙️ Поведение"
*/
// ==/WindhawkModSettings==

// Windows Headers, COM Interfaces, GUIDs, and Policy Config Definitions

#include <shellapi.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <hstring.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <sddl.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <map>
#include <string>
#include <vector>

using namespace Gdiplus;

////////////////////////////////////////////////////////////////////////////////
// COM Interfaces & GUIDs

[[maybe_unused]] const IID IID_IAudioMeterInformation = {
    0xC02216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}
};
struct IAudioMeterInformation : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float *pfPeak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT *pnChannelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 u32ChannelCount, float *afPeakValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD *pdwHardwareSupportMask) = 0;
};
template<> inline const GUID &__mingw_uuidof<IAudioMeterInformation>() {
    return IID_IAudioMeterInformation;
}

const IID IID_IPolicyConfig = {
    0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}
};
const CLSID CLSID_PolicyConfigClient = {
    0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}
};
struct IPolicyConfig : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, WAVEFORMATEX*, WAVEFORMATEX*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PINT64, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, struct DeviceShareMode*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, struct DeviceShareMode*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR wszDeviceId, ERole eRole) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

// WinRT AudioPolicyConfig Interfaces for Per-App Device Routing
const IID IID_IAudioPolicyConfigWin11 = {
    0xab3d4648, 0xe242, 0x459f, { 0xb0, 0x2f, 0x54, 0x1c, 0x70, 0x30, 0x63, 0x24 }
};
const IID IID_IAudioPolicyConfigWin10 = {
    0x2a59116d, 0x6c4f, 0x45e0, { 0xa7, 0x4f, 0x70, 0x7e, 0x3f, 0xef, 0x92, 0x58 }
};

typedef HRESULT (STDMETHODCALLTYPE *pfnSetPersistedDefaultAudioEndpoint)(
    void* self,
    DWORD processId,
    EDataFlow flow,
    ERole role,
    HSTRING deviceId
);

const PROPERTYKEY kPkeyFriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14};

// Runtime Configuration Struct Definition

struct Config {
    // Hotkeys (an empty string disables the hotkey)
    std::wstring hotkey = L"Win+Alt+V";
    std::wstring hotkeyVolUp;
    std::wstring hotkeyVolDown;
    std::wstring hotkeyMute;
    std::wstring hotkeyMasterVolUp;
    std::wstring hotkeyMasterVolDown;
    std::wstring hotkeyMasterMute;

    // Deafen Background (BETA)
    bool enableDeafenBackground = false;
    std::wstring hotkeyDeafen;
    std::wstring deafenMode = L"mute";
    int deafenDuckPercent = 20;
    std::wstring deafenWhitelist = L"discord.exe, ts3client_win64.exe, obs64.exe, spotify.exe";

    // Volume OSD (BETA)
    bool enableVolumeOsd = false;
    bool replaceWindowsOsd = false;
    std::wstring osdStyle = L"capsule";
    std::wstring osdTheme = L"inherit";
    std::wstring osdPosition = L"top_center";
    std::wstring osdMonitor = L"cursor";
    int osdOffsetX = 0;
    int osdOffsetY = 24;
    int osdWidth = 260;
    int osdScale = 100;
    int osdOpacity = 95;
    int osdTimeoutMs = 1600;
    bool osdAllowDrag = false;
    bool osdRememberPos = true;

    // System tray (BETA)
    bool showTrayIcon = false;
    std::wstring trayIconStyle = L"system";
    std::wstring trayPopupStyle = L"system_flyout";
    bool trayScrollVolume = true;

    // Window position & dragging
    bool allowDragMove = false;
    bool rememberCustomPos = true;
    std::wstring anchor = L"bottom_right";
    std::wstring monitor = L"cursor";
    int offsetX = 16;
    int offsetY = 16;

    // Appearance
    std::wstring language = L"auto";
    std::wstring layoutMode = L"modern_cards";
    std::wstring theme = L"windows11";
    int scale = 100;
    bool followDpi = true;
    int width = 320;
    int opacity = 100;
    bool animate = true;

    // Behavior
    bool vuMeters = true;
    bool showNames = true;
    bool showAllDevices = true;
    bool mergeApps = true;
    bool hideInactive = false;
    int wheelStep = 2;
    int autoHide = 0;
};

constexpr DWORD kAnimMs = 160;
constexpr int kMenuOpacity = 97;

// Multilingual Localization Strings (7 Languages)

enum class Lang { Uk, Pl, De, Fr, Es, Ru, En };

Lang GetCurrentLanguage() {
    extern Config g_cfg;
    if (g_cfg.language == L"uk") return Lang::Uk;
    if (g_cfg.language == L"pl") return Lang::Pl;
    if (g_cfg.language == L"de") return Lang::De;
    if (g_cfg.language == L"fr") return Lang::Fr;
    if (g_cfg.language == L"es") return Lang::Es;
    if (g_cfg.language == L"ru") return Lang::Ru;
    if (g_cfg.language == L"en") return Lang::En;

    // Auto-detect from system display language
    LANGID lid = GetUserDefaultUILanguage();
    WORD pri = PRIMARYLANGID(lid);
    if (pri == LANG_UKRAINIAN) return Lang::Uk;
    if (pri == LANG_POLISH) return Lang::Pl;
    if (pri == LANG_GERMAN) return Lang::De;
    if (pri == LANG_FRENCH) return Lang::Fr;
    if (pri == LANG_SPANISH) return Lang::Es;
    if (pri == LANG_RUSSIAN) return Lang::Ru;
    return Lang::En;
}

enum class StrId {
    LockVolume,
    UnlockVolume,
    RouteMenuHeader,
    SystemDefault,
    MasterVolume,
    Muted,
    ShowApps,
    HideApps,
    DeafenOn,
    DeafenOff,
    OsdDragHint,
    DefaultDeviceHeader,
    ActiveApp,
    NoAppAudio,
    Count
};

const WCHAR* L10n(StrId id) {
    // Columns follow enum class Lang: Uk, Pl, De, Fr, Es, Ru, En.
    static const WCHAR* const kStrings[(int)StrId::Count][7] = {
        {L"Зафіксувати гучність", L"Zablokuj głośność", L"Lautstärke sperren", L"Verrouiller le volume", L"Bloquear volumen", L"Зафиксировать громкость", L"Lock volume"},
        {L"Розблокувати гучність", L"Odblokuj głośność", L"Lautstärke entsperren", L"Déverrouiller le volume", L"Desbloquear volumen", L"Разблокировать громкость", L"Unlock volume"},
        {L"ВИВОДИТИ ЗВУК НА", L"PRZEKIERUJ DŹWIĘK DO", L"AUDIO AUSGEBEN AUF", L"SORTIE AUDIO VERS", L"ENVIAR AUDIO A", L"ВЫВОДИТЬ ЗВУК НА", L"ROUTE AUDIO TO"},
        {L"Пристрій Windows за замовчуванням", L"Domyślne urządzenie Windows", L"Windows-Standardgerät", L"Périphérique Windows par défaut", L"Dispositivo predeterminado de Windows", L"Устройство Windows по умолчанию", L"Windows default device"},
        {L"Загальна гучність", L"Głośność główna", L"Gesamtlautstärke", L"Volume principal", L"Volumen maestro", L"Общая громкость", L"Master volume"},
        {L"Звук вимкнено", L"Wyciszono", L"Stummgeschaltet", L"Muet", L"Silenciado", L"Звук выключен", L"Muted"},
        {L"Показати програми", L"Pokaż aplikacje", L"Apps anzeigen", L"Afficher les applications", L"Mostrar aplicaciones", L"Показать приложения", L"Show apps"},
        {L"Сховати програми", L"Ukryj aplikacje", L"Apps ausblenden", L"Masquer les applications", L"Ocultar aplicaciones", L"Скрыть приложения", L"Hide apps"},
        {L"Глушіння фону УВІМК", L"Wyciszenie tła WŁ.", L"Hintergrund stumm AN", L"Sourdine de fond ACTIVÉE", L"Silencio de fondo ACTIVADO", L"Глушение фона ВКЛ", L"Deafen ON"},
        {L"Глушіння фону ВИМК — звук відновлено", L"Wyciszenie tła WYŁ. — dźwięk przywrócony", L"Hintergrund stumm AUS — wiederhergestellt", L"Sourdine de fond DÉSACTIVÉE — son rétabli", L"Silencio de fondo DESACTIVADO — audio restaurado", L"Глушение фона ВЫКЛ — звук восстановлен", L"Deafen OFF — audio restored"},
        {L"Перетягніть OSD мишкою", L"Przeciągnij OSD myszą", L"OSD mit der Maus ziehen", L"Faites glisser l'OSD", L"Arrastra el OSD con el ratón", L"Перетащите OSD мышью", L"Drag the OSD with the mouse"},
        {L"ПРИСТРІЙ ЗА ЗАМОВЧУВАННЯМ", L"URZĄDZENIE DOMYŚLNE", L"STANDARDGERÄT", L"PÉRIPHÉRIQUE PAR DÉFAUT", L"DISPOSITIVO PREDETERMINADO", L"УСТРОЙСТВО ПО УМОЛЧАНИЮ", L"DEFAULT OUTPUT DEVICE"},
        {L"Активна програма", L"Aktywna aplikacja", L"Aktive App", L"Application active", L"App activa", L"Активное приложение", L"Active app"},
        {L"Активна програма не відтворює звук", L"Aktywna aplikacja nie odtwarza dźwięku", L"Die aktive App gibt keinen Ton aus", L"L'application active ne joue aucun son", L"La app activa no reproduce sonido", L"Активное приложение не воспроизводит звук", L"No sound from the active app"},
    };
    const int row = (int)id;
    const int col = (int)GetCurrentLanguage();
    if (row < 0 || row >= (int)StrId::Count || col < 0 || col > 6) return L"";
    return kStrings[row][col];
}

// Core Data Structures, Geometry Scaling, Vector Paths, and Device Classifiers

extern float g_scale;

inline int S(float v) {
    return (int)std::lround(v * g_scale);
}

inline void RoundRectPath(GraphicsPath& path, float x, float y, float w, float h, float r) {
    r = std::min(r, std::min(w, h) / 2.0f);
    if (r <= 0.0f) {
        path.AddRectangle(RectF(x, y, w, h));
        return;
    }
    float d = r * 2.0f;
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.AddArc(x, y, d, d, 180, 90);
    path.CloseFigure();
}

enum class DeviceType {
    Headphones,
    TV,
    Bluetooth,
    DAC,
    Optical,
    Speaker
};

inline DeviceType ClassifyDevice(const std::wstring& name) {
    auto toLowerW = [](std::wstring s) {
        for (auto& c : s) c = (WCHAR)towlower(c);
        return s;
    };
    std::wstring s = toLowerW(name);
    if (s.find(L"head") != std::wstring::npos ||
        s.find(L"ear") != std::wstring::npos ||
        s.find(L"airpod") != std::wstring::npos ||
        s.find(L"buds") != std::wstring::npos ||
        s.find(L"jbl") != std::wstring::npos ||
        s.find(L"wh-") != std::wstring::npos ||
        s.find(L"wf-") != std::wstring::npos ||
        s.find(L"bose") != std::wstring::npos ||
        s.find(L"hyperx") != std::wstring::npos ||
        s.find(L"arctis") != std::wstring::npos ||
        s.find(L"kraken") != std::wstring::npos ||
        s.find(L"steelseries") != std::wstring::npos ||
        s.find(L"corsair") != std::wstring::npos ||
        s.find(L"навушник") != std::wstring::npos ||
        s.find(L"наушник") != std::wstring::npos ||
        s.find(L"słuchawk") != std::wstring::npos) {
        return DeviceType::Headphones;
    }
    if (s.find(L"tv") != std::wstring::npos ||
        s.find(L"hdmi") != std::wstring::npos ||
        s.find(L"displayport") != std::wstring::npos ||
        s.find(L"display") != std::wstring::npos ||
        s.find(L"monitor") != std::wstring::npos ||
        s.find(L"nvidia") != std::wstring::npos ||
        s.find(L"amd high") != std::wstring::npos ||
        s.find(L"телевізор") != std::wstring::npos ||
        s.find(L"телевизор") != std::wstring::npos ||
        s.find(L"telewizor") != std::wstring::npos) {
        return DeviceType::TV;
    }
    if (s.find(L"bluetooth") != std::wstring::npos ||
        s.find(L"hands-free") != std::wstring::npos ||
        s.find(L"wireless") != std::wstring::npos) {
        return DeviceType::Bluetooth;
    }
    if (s.find(L"dac") != std::wstring::npos ||
        s.find(L"focusrite") != std::wstring::npos ||
        s.find(L"scarlett") != std::wstring::npos ||
        s.find(L"fiio") != std::wstring::npos ||
        s.find(L"topping") != std::wstring::npos ||
        s.find(L"behringer") != std::wstring::npos ||
        s.find(L"motu") != std::wstring::npos ||
        s.find(L"steinberg") != std::wstring::npos ||
        s.find(L"audient") != std::wstring::npos ||
        s.find(L"sound blaster") != std::wstring::npos ||
        s.find(L"usb audio") != std::wstring::npos) {
        return DeviceType::DAC;
    }
    if (s.find(L"spdif") != std::wstring::npos ||
        s.find(L"s/pdif") != std::wstring::npos ||
        s.find(L"optical") != std::wstring::npos ||
        s.find(L"оптич") != std::wstring::npos) {
        return DeviceType::Optical;
    }
    return DeviceType::Speaker;
}

// Line widths follow the icon size so icons stay balanced at 200-300 %.
inline void DrawSpeaker(Graphics& g, float cx, float cy, float sz, Color col, bool mute, float vol) {
    Pen pen(col, std::max(1.4f, sz * 0.075f));
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
    SolidBrush brush(col);

    float bodyW = sz * 0.22f;
    float bodyH = sz * 0.35f;
    float coneW = sz * 0.26f;
    float coneH = sz * 0.65f;
    float left = cx - sz * 0.38f;

    PointF pts[6] = {
        {left, cy - bodyH / 2},
        {left + bodyW, cy - bodyH / 2},
        {left + bodyW + coneW, cy - coneH / 2},
        {left + bodyW + coneW, cy + coneH / 2},
        {left + bodyW, cy + bodyH / 2},
        {left, cy + bodyH / 2}
    };
    g.FillPolygon(&brush, pts, 6);

    if (mute || vol <= 0.001f) {
        float xSz = sz * 0.25f;
        float xC = cx + sz * 0.20f;
        g.DrawLine(&pen, xC - xSz / 2, cy - xSz / 2, xC + xSz / 2, cy + xSz / 2);
        g.DrawLine(&pen, xC + xSz / 2, cy - xSz / 2, xC - xSz / 2, cy + xSz / 2);
    } else {
        float arcX = cx - sz * 0.05f;
        g.DrawArc(&pen, arcX, cy - sz * 0.22f, sz * 0.44f, sz * 0.44f, -45, 90);
        if (vol > 0.40f) {
            g.DrawArc(&pen, arcX - sz * 0.05f, cy - sz * 0.38f, sz * 0.76f, sz * 0.76f, -45, 90);
        }
    }
}

inline void DrawDeviceVectorIcon(Graphics& g, DeviceType type, float x, float y, float size, Color col) {
    Pen pen(col, std::max(1.3f, size * 0.085f));
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
    SolidBrush brush(col);

    switch (type) {
        case DeviceType::Headphones: {
            float r = size * 0.40f;
            g.DrawArc(&pen, x + size * 0.5f - r, y + size * 0.5f - r, r * 2.0f, r * 2.0f, 180, 180);
            float padW = size * 0.22f;
            float padH = size * 0.38f;
            GraphicsPath p1, p2;
            RoundRectPath(p1, x + size * 0.5f - r - padW * 0.4f, y + size * 0.42f, padW, padH, padW * 0.35f);
            RoundRectPath(p2, x + size * 0.5f + r - padW * 0.6f, y + size * 0.42f, padW, padH, padW * 0.35f);
            g.FillPath(&brush, &p1);
            g.FillPath(&brush, &p2);
            break;
        }
        case DeviceType::TV: {
            float scrW = size * 0.88f;
            float scrH = size * 0.58f;
            float scrX = x + (size - scrW) / 2.0f;
            float scrY = y + size * 0.12f;
            GraphicsPath scr;
            RoundRectPath(scr, scrX, scrY, scrW, scrH, 2.0f);
            g.DrawPath(&pen, &scr);
            float cx = x + size * 0.5f;
            g.DrawLine(&pen, cx, scrY + scrH, cx, scrY + scrH + size * 0.16f);
            g.DrawLine(&pen, cx - size * 0.25f, scrY + scrH + size * 0.16f, cx + size * 0.25f, scrY + scrH + size * 0.16f);
            break;
        }
        case DeviceType::Bluetooth: {
            float cx = x + size * 0.5f;
            float top = y + size * 0.12f;
            float bot = y + size * 0.88f;
            float w = size * 0.32f;
            PointF pts[6] = {
                {cx - w * 0.6f, y + size * 0.32f},
                {cx + w, y + size * 0.70f},
                {cx, bot},
                {cx, top},
                {cx + w, y + size * 0.30f},
                {cx - w * 0.6f, y + size * 0.68f}
            };
            g.DrawLines(&pen, pts, 6);
            break;
        }
        case DeviceType::DAC: {
            float boxW = size * 0.88f;
            float boxH = size * 0.60f;
            float bx = x + (size - boxW) / 2.0f;
            float by = y + (size - boxH) / 2.0f;
            GraphicsPath box;
            RoundRectPath(box, bx, by, boxW, boxH, 2.5f);
            g.DrawPath(&pen, &box);
            float kr = boxH * 0.25f;
            float kx = bx + boxW * 0.40f;
            float ky = by + boxH * 0.50f;
            g.DrawEllipse(&pen, kx - kr, ky - kr, kr * 2.0f, kr * 2.0f);
            g.FillEllipse(&brush, bx + boxW * 0.78f, by + boxH * 0.42f, size * 0.10f, size * 0.10f);
            break;
        }
        case DeviceType::Optical: {
            float cx = x + size * 0.5f;
            float cy = y + size * 0.5f;
            float r = size * 0.40f;
            PointF hex[6];
            for (int i = 0; i < 6; i++) {
                float a = i * 3.14159265f / 3.0f;
                hex[i] = {cx + r * std::cos(a), cy + r * std::sin(a)};
            }
            g.DrawPolygon(&pen, hex, 6);
            g.FillEllipse(&brush, cx - size * 0.12f, cy - size * 0.12f, size * 0.24f, size * 0.24f);
            break;
        }
        case DeviceType::Speaker:
        default: {
            float sx = x + size * 0.10f;
            float sy = y + size * 0.5f;
            PointF pts[6] = {
                {sx, sy - size * 0.18f},
                {sx + size * 0.22f, sy - size * 0.18f},
                {sx + size * 0.48f, sy - size * 0.38f},
                {sx + size * 0.48f, sy + size * 0.38f},
                {sx + size * 0.22f, sy + size * 0.18f},
                {sx, sy + size * 0.18f}
            };
            g.FillPolygon(&brush, pts, 6);
            g.DrawArc(&pen, sx + size * 0.35f, sy - size * 0.30f, size * 0.55f, size * 0.60f, -40, 80);
            break;
        }
    }
}

struct Theme {
    Color bg;
    Color border;
    Color rowHover;
    Color trackBg;
    Color trackFill;
    Color text;
    Color muted;
    Color thumb;
    Color peak;
    float radius = 10.0f;
    bool hasGlass = true;
};

struct Item {
    int kind = 0;              // 0 = device header, 1 = master, 2 = app
    std::wstring key;
    std::wstring name;
    std::wstring deviceId;
    DWORD pid = 0;
    std::vector<DWORD> pids;   // every process merged into this row
    float vol = 1.0f;
    bool mute = false;
    float peak = 0.0f;
    Bitmap* icon = nullptr;
    bool iconOwned = false;
    bool isDefaultDevice = false;

    bool locked = false;
    float lockedVol = 1.0f;

    RECT iconRc{};
    RECT routeBtnRc{};
    RECT nameRc{};
    RECT pillRc{};
    int rowTop = 0;
    int rowBottom = 0;
    int colL = 0;
    int colR = 0;
    int trackL = 0;
    int trackR = 0;
    int trackCy = 0;
    int trackH = 0;
    int trackTop = 0;
    int trackBottom = 0;
    bool verticalSlider = false;
    bool column = false;
    bool firstDevice = false;

    IAudioEndpointVolume* ep = nullptr;
    std::vector<ISimpleAudioVolume*> sav;
    std::vector<IAudioMeterInformation*> meters;
};

struct AppInfo {
    std::wstring name;
    std::wstring exeName;
    Bitmap* icon = nullptr;
};

struct CustomMenuItem {
    int id = 0;
    std::wstring text;
    bool isSeparator = false;
    bool isHeader = false;
    bool isChecked = false;
    int devType = -1;
    std::wstring deviceId;  // device rows; the device list can change while the menu is open
    int top = 0;
    int bottom = 0;
};

// Global State, Handles, Timers, and Constants

struct DeviceEntry {
    std::wstring id;
    std::wstring name;
};

// Off-screen 32-bit DIB used with UpdateLayeredWindow. A bitmap that is still
// selected into a DC cannot be deleted, so the new bitmap is selected first and
// only then is the previous one freed.
struct Surface {
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ oldBmp = nullptr;
    void* bits = nullptr;
    int w = 0;
    int h = 0;

    bool Ensure(int nw, int nh) {
        nw = std::max(1, nw);
        nh = std::max(1, nh);
        if (bmp && w == nw && h == nh) return true;
        if (!dc) {
            HDC screen = GetDC(nullptr);
            dc = CreateCompatibleDC(screen);
            ReleaseDC(nullptr, screen);
            if (!dc) return false;
        }
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = nw;
        bi.bmiHeader.biHeight = -nh;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* nbits = nullptr;
        HBITMAP nb = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &nbits, nullptr, 0);
        if (!nb || !nbits) {
            if (nb) DeleteObject(nb);
            return false;
        }
        HGDIOBJ prev = SelectObject(dc, nb);
        if (!oldBmp) oldBmp = prev;
        if (bmp) DeleteObject(bmp);
        bmp = nb;
        bits = nbits;
        w = nw;
        h = nh;
        return true;
    }

    void Free() {
        if (dc) {
            if (oldBmp) SelectObject(dc, oldBmp);
            DeleteDC(dc);
        }
        if (bmp) DeleteObject(bmp);
        *this = Surface{};
    }
};

Config g_cfg;
Theme g_theme;
std::vector<Item> g_items;
std::map<std::wstring, AppInfo> g_appCache;
std::map<std::wstring, float> g_lockedVolumes;  // item key -> locked level, persisted
bool g_locksDirty = false;
std::vector<DeviceEntry> g_deviceList;
std::wstring g_defaultDeviceId;

HINSTANCE g_hInst = nullptr;
HWND g_hwnd = nullptr;
Surface g_mainSurface;
int g_winW = 320;
int g_winH = 200;
POINT g_pos{0, 0};
float g_scale = 1.0f;  // user scale x monitor DPI (x auto-fit)
HMONITOR g_popupMonitor = nullptr;
bool g_openedFromTray = false;
std::wstring g_activeTrayPopupStyle;  // tray popup style while opened from the tray icon
std::vector<int> g_colSeparators;

// Custom Dragging and Remembering Window Position
bool g_movingWindow = false;
POINT g_dragStartCursor{0, 0};
POINT g_dragStartWinPos{0, 0};
int g_customPosX = 0;
int g_customPosY = 0;
bool g_hasCustomPos = false;
bool g_trayExpanded = false;
RECT g_trayExpandBtnRc{};

// System Tray Notification Icon
constexpr UINT WM_APP_TRAY = WM_APP + 10;
constexpr UINT TRAY_ICON_ID = 2001;
NOTIFYICONDATAW g_nid{};
bool g_trayAdded = false;
HICON g_hTrayIcon = nullptr;
std::wstring g_traySig;
DWORD g_lastTrayToggle = 0;

// Routing / lock context menu
HWND g_hwndMenu = nullptr;
Surface g_menuSurface;
int g_menuW = 0;
int g_menuH = 0;
POINT g_menuPos{0, 0};
int g_menuHover = -1;
bool g_menuTracking = false;
std::vector<CustomMenuItem> g_menuItems;
struct MenuTarget {
    int kind = -1;
    std::wstring key;
    std::wstring deviceId;
    std::vector<DWORD> pids;
} g_menuTarget;

// OSD
HWND g_hwndOsd = nullptr;
Surface g_osdSurface;
DWORD g_osdHideTick = 0;
bool g_osdDragging = false;
POINT g_osdDragStart{0, 0};
POINT g_osdWinStart{0, 0};
bool g_hasCustomOsdPos = false;
int g_customOsdX = 0;
int g_customOsdY = 0;
// What the OSD shows, so that the mouse wheel over it changes the same thing.
enum class OsdTarget { None, Master, App };
OsdTarget g_osdTarget = OsdTarget::None;
std::wstring g_osdTargetKey;
float g_osdMasterVol = -1.0f;  // the master level and mute the OSD shows
bool g_osdMasterMute = false;
POINT g_osdShowPointer{0, 0};  // pointer position when the OSD appeared
bool g_osdPointerMoved = false;

// Popup state (UI thread only)
bool g_visible = false;
bool g_hiding = false;
DWORD g_animStart = 0;
DWORD g_showTick = 0;
DWORD g_lastActive = 0;
DWORD g_lastValuesPoll = 0;
DWORD g_lastModelPoll = 0;
bool g_dragging = false;
int g_dragIdx = -1;
int g_hoverIdx = -1;
std::string g_sig;

HANDLE g_readyEvent = nullptr;
HANDLE g_thread = nullptr;
ULONG_PTR g_gdiplusToken = 0;

// Deafen background
struct DeafenSavedState {
    float originalVol = 1.0f;
    bool originalMute = false;
    bool ducked = false;  // the volume was lowered (otherwise only muted)
    DWORD day = 0;        // when the entry started waiting for its app (days since 1601)
};
bool g_deafenActive = false;
std::map<std::wstring, DeafenSavedState> g_deafenSavedStates;  // "key|deviceId" -> state while Deafen is on
std::map<std::wstring, DeafenSavedState> g_deafenPending;      // to restore when the app plays again

// Hotkeys. Index + 1 is the id used with WM_HOTKEY and RegisterHotKey.
enum HotkeyIndex {
    HK_MAIN,
    HK_APP_UP,
    HK_APP_DOWN,
    HK_APP_MUTE,
    HK_MASTER_UP,
    HK_MASTER_DOWN,
    HK_MASTER_MUTE,
    HK_DEAFEN,
    HK_COUNT
};
constexpr bool kHotkeyRepeats[HK_COUNT] = {false, true, true, false, true, true, false, false};
struct HotkeyDef {
    UINT mods = 0;
    UINT vk = 0;
};

// State shared with the low-level hook thread. The hooks live on their own
// thread so slow audio calls on the UI thread never stall system-wide input
// (Windows silently removes low-level hooks that time out).
SRWLOCK g_hookLock = SRWLOCK_INIT;  // guards g_hookHotkeys, g_hookTrayRect and g_hookOsdRect
HotkeyDef g_hookHotkeys[HK_COUNT];
RECT g_hookTrayRect{};
RECT g_hookOsdRect{};
std::atomic<bool> g_hookOsdHover{false};  // a visible pointer is over the OSD
std::atomic<bool> g_hookPopupOpen{false};
std::atomic<DWORD> g_hookShowTick{0};
std::atomic<DWORD> g_hookOutsideClickTick{0};
std::atomic<bool> g_hookReplaceOsd{false};
std::atomic<bool> g_hookTrayWheel{false};
std::atomic<bool> g_volumeMsgPending{false};
std::atomic<bool> g_deviceMsgPending{false};
std::atomic<bool> g_lockCheckPending{false};
std::atomic<bool> g_lockRebuildPending{false};
HANDLE g_hookThread = nullptr;
DWORD g_hookThreadId = 0;
bool g_mouseHookWanted = false;
bool g_kbdHookWanted = false;

constexpr UINT_PTR ID_TIMER_FAST = 1;
constexpr UINT_PTR ID_TIMER_ANIM = 2;
constexpr UINT_PTR ID_TIMER_OSD = 3;
constexpr UINT_PTR ID_TIMER_REFRESH_ONCE = 5;
constexpr UINT_PTR ID_TIMER_SAVE_LOCKS = 6;

constexpr UINT WM_APP_RELOAD = WM_APP + 1;
constexpr UINT WM_APP_QUIT = WM_APP + 2;
constexpr UINT WM_APP_CLOSE_MIXER = WM_APP + 3;
constexpr UINT WM_APP_CLOSE_MENU = WM_APP + 4;
constexpr UINT WM_APP_VOLUME_CHANGED = WM_APP + 5;
constexpr UINT WM_APP_DEVICES_CHANGED = WM_APP + 6;
constexpr UINT WM_APP_MEDIA_KEY = WM_APP + 7;
constexpr UINT WM_APP_TRAY_WHEEL = WM_APP + 8;
constexpr UINT WM_APP_TASKBAR_WHEEL = WM_APP + 9;  // wheel over the notification area, wParam = delta
constexpr UINT WM_APP_OSD_WHEEL = WM_APP + 11;     // wheel over the OSD, wParam = delta
constexpr UINT WM_APP_LOCK_CHECK = WM_APP + 12;
constexpr UINT WM_APP_LOCK_REBUILD = WM_APP + 13;
constexpr UINT WM_APP_NEW_SESSION = WM_APP + 14;  // wParam = PID, lParam = device index, watch generation
constexpr UINT WM_APP_HOOK_MOUSE = WM_APP + 20;  // hook thread messages, wParam = install
constexpr UINT WM_APP_HOOK_KBD = WM_APP + 21;

const wchar_t kClass[] = L"VolumeMixerFlyoutPro";
const wchar_t kOsdClass[] = L"VolumeMixerOsdPro";
const wchar_t kMenuClass[] = L"VolumeMixerMenuPro";

////////////////////////////////////////////////////////////////////////////////
// GDI+ Resources Cache
//
// Fonts use UnitPixel so the rendered size depends only on the scale factor
// (user scale x monitor DPI) and never on the DPI stored in the bitmap.

struct GdiCache {
    Font* fontName = nullptr;
    Font* fontHeader = nullptr;
    Font* fontValue = nullptr;
    Font* fontRoute = nullptr;
    Font* fontMenuText = nullptr;
    Font* fontMenuHeader = nullptr;
    Font* fontMenuCheck = nullptr;
    Font* fontMono = nullptr;
    Font* fontBig = nullptr;
    float cachedScale = 0.0f;
};

GdiCache g_gc;     // mixer popup + routing menu
GdiCache g_gcOsd;  // OSD (has its own scale)

static Font* MakeUiFont(const WCHAR* family, float px, INT style) {
    Font* f = new Font(family, px, style, UnitPixel);
    if (!f || f->GetLastStatus() != Ok) {
        delete f;
        f = new Font(L"Segoe UI", px, style, UnitPixel);
    }
    return f;
}

void FreeGdiCache(GdiCache& gc) {
    delete gc.fontName;
    delete gc.fontHeader;
    delete gc.fontValue;
    delete gc.fontRoute;
    delete gc.fontMenuText;
    delete gc.fontMenuHeader;
    delete gc.fontMenuCheck;
    delete gc.fontMono;
    delete gc.fontBig;
    gc = GdiCache{};
}

void FreeGdiCache() {
    FreeGdiCache(g_gc);
    FreeGdiCache(g_gcOsd);
}

void UpdateGdiCache(GdiCache& gc, float s) {
    if (gc.fontName && std::abs(gc.cachedScale - s) < 0.001f) return;
    FreeGdiCache(gc);
    gc.cachedScale = s;
    auto px = [s](float pt) { return std::max(7.0f, pt * (96.0f / 72.0f) * s); };
    // Clear, readable, modern typography
    gc.fontName = MakeUiFont(L"Segoe UI Variable Text", px(10.5f), FontStyleRegular);
    gc.fontHeader = MakeUiFont(L"Segoe UI Variable Display", px(11.0f), FontStyleBold);
    gc.fontValue = MakeUiFont(L"Segoe UI Variable Small", px(10.0f), FontStyleBold);
    gc.fontRoute = MakeUiFont(L"Segoe UI Variable Text", px(9.5f), FontStyleBold);
    gc.fontMenuText = MakeUiFont(L"Segoe UI Variable Text", px(10.0f), FontStyleRegular);
    gc.fontMenuHeader = MakeUiFont(L"Segoe UI Variable Display", px(10.5f), FontStyleBold);
    gc.fontMenuCheck = MakeUiFont(L"Segoe UI Symbol", px(10.0f), FontStyleBold);
    gc.fontMono = MakeUiFont(L"Consolas", px(9.5f), FontStyleRegular);
    gc.fontBig = MakeUiFont(L"Segoe UI Variable Display", px(13.0f), FontStyleBold);
}

void UpdateGdiCache(float s) {
    UpdateGdiCache(g_gc, s);
}

// Color Palettes, Theme Presets, Windows Accent Integration, and Hotkey Parsing

Color Dim(Color c, float factor) {
    return Color(c.GetA(),
                 (BYTE)std::clamp((int)(c.GetR() * factor), 0, 255),
                 (BYTE)std::clamp((int)(c.GetG() * factor), 0, 255),
                 (BYTE)std::clamp((int)(c.GetB() * factor), 0, 255));
}

Color Lerp(Color a, Color b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    auto m = [t](BYTE x, BYTE y) { return (BYTE)(x + (y - x) * t); };
    return Color(255, m(a.GetR(), b.GetR()), m(a.GetG(), b.GetG()), m(a.GetB(), b.GetB()));
}

Color GetAccentColor() {
    DWORD cr = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&cr, &opaque))) {
        return Color(255, (BYTE)((cr >> 16) & 0xFF), (BYTE)((cr >> 8) & 0xFF), (BYTE)(cr & 0xFF));
    }
    return Color(255, 0, 120, 215);
}

Theme MakeTheme(const std::wstring& nameIn) {
    std::wstring name = nameIn;
    // Ids used by older versions of the mod
    if (name == L"oled") name = L"amoled";
    else if (name == L"cyberpunk") name = L"neon";

    Theme t;
    if (name == L"light") {
        t.bg = Color(240, 245, 245, 248);
        t.border = Color(60, 0, 0, 0);
        t.rowHover = Color(15, 0, 0, 0);
        t.trackBg = Color(20, 0, 0, 0);
        t.trackFill = Color(240, 0, 103, 192);
        t.text = Color(240, 20, 20, 20);
        t.muted = Color(130, 20, 20, 20);
        t.thumb = Color(255, 255, 255, 255);
        t.peak = Color(180, 0, 150, 255);
        t.radius = 12.0f;
        t.hasGlass = true;
    } else if (name == L"amoled") {
        t.bg = Color(255, 0, 0, 0);
        t.border = Color(40, 255, 255, 255);
        t.rowHover = Color(18, 255, 255, 255);
        t.trackBg = Color(35, 255, 255, 255);
        t.trackFill = Color(255, 255, 255, 255);
        t.text = Color(255, 255, 255, 255);
        t.muted = Color(120, 255, 255, 255);
        t.thumb = Color(255, 255, 255, 255);
        t.peak = Color(200, 180, 180, 180);
        t.radius = 8.0f;
        t.hasGlass = false;
    } else if (name == L"glass") {
        t.bg = Color(120, 18, 20, 24);
        t.border = Color(45, 255, 255, 255);
        t.rowHover = Color(25, 255, 255, 255);
        t.trackBg = Color(35, 255, 255, 255);
        t.trackFill = Color(240, 96, 165, 250);
        t.text = Color(245, 255, 255, 255);
        t.muted = Color(120, 255, 255, 255);
        t.thumb = Color(255, 255, 255, 255);
        t.peak = Color(180, 147, 197, 253);
        t.radius = 14.0f;
        t.hasGlass = true;
    } else if (name == L"accent") {
        Color acc = GetAccentColor();
        t.bg = Color(225, 20, 22, 28);
        t.border = Color(50, 255, 255, 255);
        t.rowHover = Color(15, 255, 255, 255);
        t.trackBg = Color(30, 255, 255, 255);
        t.trackFill = acc;
        t.text = Color(240, 255, 255, 255);
        t.muted = Color(120, 255, 255, 255);
        t.thumb = Color(255, 255, 255, 255);
        t.peak = Dim(acc, 1.25f);
        t.radius = 10.0f;
        t.hasGlass = true;
    } else if (name == L"classic") {
        t.bg = Color(235, 32, 32, 32);
        t.border = Color(40, 255, 255, 255);
        t.rowHover = Color(12, 255, 255, 255);
        t.trackBg = Color(35, 255, 255, 255);
        t.trackFill = Color(255, 96, 205, 255);
        t.text = Color(240, 255, 255, 255);
        t.muted = Color(130, 255, 255, 255);
        t.thumb = Color(255, 255, 255, 255);
        t.peak = Color(180, 140, 220, 255);
        t.radius = 8.0f;
        t.hasGlass = true;
    } else if (name == L"neon") {
        t.bg = Color(240, 10, 10, 16);
        t.border = Color(120, 0, 255, 200);
        t.rowHover = Color(25, 0, 255, 200);
        t.trackBg = Color(35, 255, 255, 255);
        t.trackFill = Color(255, 0, 255, 200);
        t.text = Color(245, 255, 255, 255);
        t.muted = Color(140, 0, 255, 200);
        t.thumb = Color(255, 255, 255, 255);
        t.peak = Color(220, 255, 0, 128);
        t.radius = 12.0f;
        t.hasGlass = true;
    } else if (name == L"nord") {
        t.bg = Color(230, 46, 52, 64);
        t.border = Color(60, 216, 222, 233);
        t.rowHover = Color(20, 236, 239, 244);
        t.trackBg = Color(40, 216, 222, 233);
        t.trackFill = Color(245, 136, 192, 208);
        t.text = Color(245, 236, 239, 244);
        t.muted = Color(140, 216, 222, 233);
        t.thumb = Color(255, 236, 239, 244);
        t.peak = Color(200, 163, 190, 140);
        t.radius = 10.0f;
        t.hasGlass = true;
    } else if (name == L"retro") {
        t.bg = Color(245, 28, 30, 34);
        t.border = Color(75, 200, 160, 80);
        t.rowHover = Color(25, 255, 200, 100);
        t.trackBg = Color(45, 0, 0, 0);
        t.trackFill = Color(255, 245, 175, 45);
        t.text = Color(250, 240, 235, 220);
        t.muted = Color(140, 200, 185, 160);
        t.thumb = Color(255, 245, 240, 220);
        t.peak = Color(220, 255, 80, 40);
        t.radius = 4.0f;
        t.hasGlass = false;
    } else if (name == L"tokyo_night") {
        t.bg = Color(235, 26, 27, 38);
        t.border = Color(70, 187, 154, 247);
        t.rowHover = Color(22, 187, 154, 247);
        t.trackBg = Color(35, 255, 255, 255);
        t.trackFill = Color(245, 187, 154, 247);
        t.text = Color(245, 192, 202, 245);
        t.muted = Color(130, 169, 177, 214);
        t.thumb = Color(255, 255, 255, 255);
        t.peak = Color(220, 122, 162, 247);
        t.radius = 12.0f;
        t.hasGlass = true;
    } else if (name == L"sunset") {
        t.bg = Color(230, 36, 22, 50);
        t.border = Color(60, 255, 120, 100);
        t.rowHover = Color(20, 255, 120, 100);
        t.trackBg = Color(35, 255, 255, 255);
        t.trackFill = Color(250, 255, 110, 110);
        t.text = Color(245, 255, 240, 245);
        t.muted = Color(130, 230, 170, 200);
        t.thumb = Color(255, 255, 220, 180);
        t.peak = Color(220, 255, 180, 60);
        t.radius = 12.0f;
        t.hasGlass = true;
    } else if (name == L"catppuccin") {
        t.bg = Color(235, 30, 30, 46);
        t.border = Color(55, 203, 166, 247);
        t.rowHover = Color(18, 203, 166, 247);
        t.trackBg = Color(30, 255, 255, 255);
        t.trackFill = Color(245, 203, 166, 247);
        t.text = Color(245, 205, 214, 244);
        t.muted = Color(130, 166, 173, 200);
        t.thumb = Color(255, 242, 205, 205);
        t.peak = Color(200, 137, 220, 235);
        t.radius = 12.0f;
        t.hasGlass = true;
    } else if (name == L"ocean") {
        t.bg = Color(238, 10, 25, 38);
        t.border = Color(80, 64, 200, 220);
        t.rowHover = Color(26, 64, 200, 220);
        t.trackBg = Color(40, 255, 255, 255);
        t.trackFill = Color(255, 38, 198, 218);
        t.text = Color(245, 230, 250, 255);
        t.muted = Color(140, 150, 200, 215);
        t.thumb = Color(255, 240, 255, 255);
        t.peak = Color(220, 120, 240, 200);
        t.radius = 12.0f;
        t.hasGlass = true;
    } else if (name == L"rose_pine") {
        t.bg = Color(242, 25, 23, 36);
        t.border = Color(80, 196, 167, 231);
        t.rowHover = Color(24, 196, 167, 231);
        t.trackBg = Color(40, 224, 222, 244);
        t.trackFill = Color(255, 235, 188, 186);
        t.text = Color(245, 224, 222, 244);
        t.muted = Color(140, 144, 140, 170);
        t.thumb = Color(255, 246, 193, 119);
        t.peak = Color(220, 156, 207, 216);
        t.radius = 10.0f;
        t.hasGlass = false;
    } else if (name == L"gruvbox") {
        t.bg = Color(246, 40, 40, 40);
        t.border = Color(90, 254, 128, 25);
        t.rowHover = Color(26, 254, 128, 25);
        t.trackBg = Color(50, 235, 219, 178);
        t.trackFill = Color(255, 250, 189, 47);
        t.text = Color(250, 235, 219, 178);
        t.muted = Color(150, 168, 153, 132);
        t.thumb = Color(255, 251, 241, 199);
        t.peak = Color(230, 184, 187, 38);
        t.radius = 4.0f;
        t.hasGlass = false;
    } else if (name == L"solarized") {
        t.bg = Color(246, 0, 43, 54);
        t.border = Color(80, 42, 161, 152);
        t.rowHover = Color(24, 42, 161, 152);
        t.trackBg = Color(45, 238, 232, 213);
        t.trackFill = Color(255, 38, 139, 210);
        t.text = Color(245, 238, 232, 213);
        t.muted = Color(150, 131, 148, 150);
        t.thumb = Color(255, 253, 246, 227);
        t.peak = Color(230, 133, 153, 0);
        t.radius = 8.0f;
        t.hasGlass = false;
    } else if (name == L"matrix") {
        t.bg = Color(250, 0, 8, 2);
        t.border = Color(130, 0, 255, 70);
        t.rowHover = Color(26, 0, 255, 70);
        t.trackBg = Color(45, 0, 255, 70);
        t.trackFill = Color(255, 0, 230, 64);
        t.text = Color(255, 140, 255, 160);
        t.muted = Color(140, 0, 180, 60);
        t.thumb = Color(255, 190, 255, 200);
        t.peak = Color(235, 210, 255, 210);
        t.radius = 2.0f;
        t.hasGlass = false;
    } else if (name == L"midnight_gold") {
        t.bg = Color(246, 14, 14, 18);
        t.border = Color(100, 212, 175, 55);
        t.rowHover = Color(24, 212, 175, 55);
        t.trackBg = Color(40, 245, 240, 225);
        t.trackFill = Color(255, 212, 175, 55);
        t.text = Color(245, 245, 240, 225);
        t.muted = Color(140, 180, 160, 110);
        t.thumb = Color(255, 255, 236, 180);
        t.peak = Color(230, 255, 220, 120);
        t.radius = 10.0f;
        t.hasGlass = true;
    } else if (name == L"windows11") {
        // Dark Mica-like surface with the Windows accent color (lightened like Fluent sliders)
        const Color acc = Lerp(GetAccentColor(), Color(255, 255, 255, 255), 0.30f);
        t.bg = Color(242, 32, 32, 32);
        t.border = Color(48, 255, 255, 255);
        t.rowHover = Color(16, 255, 255, 255);
        t.trackBg = Color(70, 255, 255, 255);
        t.trackFill = acc;
        t.text = Color(255, 255, 255, 255);
        t.muted = Color(140, 255, 255, 255);
        t.thumb = acc;
        t.peak = Color(200, 255, 255, 255);
        t.radius = 8.0f;
        t.hasGlass = true;
    } else if (name == L"dracula") {
        t.bg = Color(245, 40, 42, 54);
        t.border = Color(90, 189, 147, 249);
        t.rowHover = Color(24, 189, 147, 249);
        t.trackBg = Color(70, 98, 114, 164);
        t.trackFill = Color(255, 189, 147, 249);
        t.text = Color(255, 248, 248, 242);
        t.muted = Color(160, 98, 114, 164);
        t.thumb = Color(255, 255, 121, 198);
        t.peak = Color(230, 80, 250, 123);
        t.radius = 10.0f;
        t.hasGlass = false;
    } else if (name == L"forest") {
        t.bg = Color(242, 22, 34, 26);
        t.border = Color(80, 120, 190, 120);
        t.rowHover = Color(22, 140, 200, 140);
        t.trackBg = Color(45, 200, 230, 200);
        t.trackFill = Color(255, 92, 184, 92);
        t.text = Color(245, 226, 240, 220);
        t.muted = Color(140, 160, 190, 150);
        t.thumb = Color(255, 236, 246, 228);
        t.peak = Color(220, 210, 230, 110);
        t.radius = 10.0f;
        t.hasGlass = true;
    } else if (name == L"monochrome") {
        t.bg = Color(250, 0, 0, 0);
        t.border = Color(200, 255, 255, 255);
        t.rowHover = Color(40, 255, 255, 255);
        t.trackBg = Color(70, 255, 255, 255);
        t.trackFill = Color(255, 255, 255, 255);
        t.text = Color(255, 255, 255, 255);
        t.muted = Color(170, 255, 255, 255);
        t.thumb = Color(255, 255, 255, 255);
        t.peak = Color(255, 160, 160, 160);
        t.radius = 4.0f;
        t.hasGlass = false;
    } else { // Minimal (default)
        t.bg = Color(220, 18, 20, 26);
        t.border = Color(40, 255, 255, 255);
        t.rowHover = Color(14, 255, 255, 255);
        t.trackBg = Color(28, 255, 255, 255);
        t.trackFill = Color(240, 96, 165, 250);
        t.text = Color(235, 255, 255, 255);
        t.muted = Color(120, 255, 255, 255);
        t.thumb = Color(255, 255, 255, 255);
        t.peak = Color(180, 147, 197, 253);
        t.radius = 10.0f;
        t.hasGlass = true;
    }
    return t;
}

std::wstring ToLower(std::wstring s) {
    for (auto& c : s) c = (WCHAR)towlower(c);
    return s;
}

std::wstring Trim(const std::wstring& s) {
    size_t start = s.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return L"";
    size_t end = s.find_last_not_of(L" \t\r\n");
    return s.substr(start, end - start + 1);
}

UINT KeyFromName(const std::wstring& name) {
    if (name.size() == 1) {
        WCHAR c = (WCHAR)towupper(name[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) return (UINT)c;
        if (c == L'-' || c == L'_') return VK_OEM_MINUS;
        if (c == L'=' || c == L'+') return VK_OEM_PLUS;
        if (c == L'[' || c == L'{') return VK_OEM_4;
        if (c == L']' || c == L'}') return VK_OEM_6;
        if (c == L';' || c == L':') return VK_OEM_1;
        if (c == L'\'' || c == L'\"') return VK_OEM_7;
        if (c == L',' || c == L'<') return VK_OEM_COMMA;
        if (c == L'.' || c == L'>') return VK_OEM_PERIOD;
        if (c == L'/' || c == L'?') return VK_OEM_2;
        if (c == L'\\' || c == L'|') return VK_OEM_5;
        if (c == L'`' || c == L'~') return VK_OEM_3;
    }
    if (name.size() >= 2 && (name[0] == L'f' || name[0] == L'F')) {
        int num = _wtoi(name.c_str() + 1);
        if (num >= 1 && num <= 24) return VK_F1 + (num - 1);
    }
    static const struct { const WCHAR* name; UINT vk; } kNamed[] = {
        {L"space", VK_SPACE}, {L"tab", VK_TAB}, {L"enter", VK_RETURN}, {L"return", VK_RETURN},
        {L"esc", VK_ESCAPE}, {L"escape", VK_ESCAPE}, {L"backspace", VK_BACK}, {L"delete", VK_DELETE},
        {L"del", VK_DELETE}, {L"insert", VK_INSERT}, {L"ins", VK_INSERT}, {L"home", VK_HOME},
        {L"end", VK_END}, {L"pageup", VK_PRIOR}, {L"pgup", VK_PRIOR}, {L"pagedown", VK_NEXT},
        {L"pgdn", VK_NEXT}, {L"up", VK_UP}, {L"down", VK_DOWN}, {L"left", VK_LEFT}, {L"right", VK_RIGHT},
        {L"volumeup", VK_VOLUME_UP}, {L"volumedown", VK_VOLUME_DOWN}, {L"volumemute", VK_VOLUME_MUTE},
        {L"mute", VK_VOLUME_MUTE}, {L"printscreen", VK_SNAPSHOT}, {L"pause", VK_PAUSE},
        {L"capslock", VK_CAPITAL}, {L"numlock", VK_NUMLOCK}, {L"scrolllock", VK_SCROLL},
        {L"numpad0", VK_NUMPAD0}, {L"numpad1", VK_NUMPAD1}, {L"numpad2", VK_NUMPAD2}, {L"numpad3", VK_NUMPAD3},
        {L"numpad4", VK_NUMPAD4}, {L"numpad5", VK_NUMPAD5}, {L"numpad6", VK_NUMPAD6}, {L"numpad7", VK_NUMPAD7},
        {L"numpad8", VK_NUMPAD8}, {L"numpad9", VK_NUMPAD9}, {L"num0", VK_NUMPAD0}, {L"num1", VK_NUMPAD1},
        {L"num2", VK_NUMPAD2}, {L"num3", VK_NUMPAD3}, {L"num4", VK_NUMPAD4}, {L"num5", VK_NUMPAD5},
        {L"num6", VK_NUMPAD6}, {L"num7", VK_NUMPAD7}, {L"num8", VK_NUMPAD8}, {L"num9", VK_NUMPAD9},
        {L"numpadadd", VK_ADD}, {L"numpadsub", VK_SUBTRACT}, {L"numpadmul", VK_MULTIPLY}, {L"numpaddiv", VK_DIVIDE},
        {L"minus", VK_OEM_MINUS}, {L"plus", VK_OEM_PLUS}, {L"comma", VK_OEM_COMMA}, {L"period", VK_OEM_PERIOD},
        {L"slash", VK_OEM_2}, {L"backslash", VK_OEM_5}, {L"semicolon", VK_OEM_1}, {L"quote", VK_OEM_7},
        {L"bracketleft", VK_OEM_4}, {L"bracketright", VK_OEM_6}, {L"tilde", VK_OEM_3}, {L"grave", VK_OEM_3}
    };
    for (const auto& n : kNamed) {
        if (name == n.name) return n.vk;
    }
    return 0;
}

bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    *mods = 0;
    *vk = 0;
    if (text.empty()) return false;

    size_t pos = 0;
    while (pos < text.size()) {
        size_t next = text.find_first_of(L"+- ", pos);
        if (next == std::wstring::npos) next = text.size();
        std::wstring tok = ToLower(Trim(text.substr(pos, next - pos)));
        pos = next + 1;
        if (tok.empty()) continue;

        if (tok == L"ctrl" || tok == L"control" || tok == L"ctl") *mods |= MOD_CONTROL;
        else if (tok == L"alt" || tok == L"opt" || tok == L"option" || tok == L"menu") *mods |= MOD_ALT;
        else if (tok == L"shift" || tok == L"shft") *mods |= MOD_SHIFT;
        else if (tok == L"win" || tok == L"windows" || tok == L"super" || tok == L"meta" || tok == L"cmd") *mods |= MOD_WIN;
        else {
            UINT key = KeyFromName(tok);
            if (key) *vk = key;
        }
    }
    return *vk != 0;
}

// Process Inspection, Icon Extraction, Device Management, and Routing

std::wstring GetProcessPath(DWORD pid) {
    std::wstring res;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProc) {
        WCHAR buf[MAX_PATH];
        DWORD sz = MAX_PATH;
        if (QueryFullProcessImageNameW(hProc, 0, buf, &sz)) res = buf;
        CloseHandle(hProc);
    }
    return res;
}

// The tool process is 32-bit in Windhawk 1.7.3: there, System32 is redirected
// to SysWOW64, so files of 64-bit system executables are read through the
// Sysnative alias instead.
std::wstring FileAccessPath(const std::wstring& path) {
#ifndef _WIN64
    static int s_wow64 = -1;
    if (s_wow64 < 0) {
        BOOL wow = FALSE;
        s_wow64 = (IsWow64Process(GetCurrentProcess(), &wow) && wow) ? 1 : 0;
    }
    if (s_wow64) {
        WCHAR winDir[MAX_PATH];
        const UINT n = GetWindowsDirectoryW(winDir, MAX_PATH);
        if (n && n < MAX_PATH) {
            const std::wstring prefix = std::wstring(winDir) + L"\\System32\\";
            if (path.size() > prefix.size() && ToLower(path.substr(0, prefix.size())) == ToLower(prefix)) {
                return std::wstring(winDir) + L"\\Sysnative\\" + path.substr(prefix.size());
            }
        }
    }
#endif
    return path;
}

std::wstring GetFileDescription(const std::wstring& path) {
    const std::wstring file = FileAccessPath(path);
    DWORD dummy = 0;
    DWORD sz = GetFileVersionInfoSizeW(file.c_str(), &dummy);
    if (!sz) return L"";
    std::vector<BYTE> buf(sz);
    if (!GetFileVersionInfoW(file.c_str(), 0, sz, buf.data())) return L"";

    struct LANGANDCODEPAGE { WORD wLanguage; WORD wCodePage; } *lpTranslate = nullptr;
    UINT cbTranslate = 0;
    if (!VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate) || !lpTranslate) {
        return L"";
    }

    for (UINT i = 0; i < (cbTranslate / sizeof(LANGANDCODEPAGE)); i++) {
        WCHAR subBlock[64];
        swprintf_s(subBlock, L"\\StringFileInfo\\%04x%04x\\FileDescription",
                   lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
        WCHAR* desc = nullptr;
        UINT descSz = 0;
        if (VerQueryValueW(buf.data(), subBlock, (LPVOID*)&desc, &descSz) && desc && descSz > 1) {
            return desc;
        }
    }
    return L"";
}

std::wstring GetProcessExeNameSnapshot(DWORD pid) {
    std::wstring res;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return res;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe)) {
        do {
            if (pe.th32ProcessID == pid) {
                res = pe.szExeFile;
                break;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return res;
}

// Bitmap::FromHICON drops the alpha channel (black halos around icons), so the
// colour and mask bitmaps are converted manually into a 32-bit ARGB bitmap.
static Bitmap* IconToBitmap(HICON hIcon) {
    ICONINFO ii{};
    if (!GetIconInfo(hIcon, &ii)) return nullptr;
    Bitmap* result = nullptr;
    if (ii.hbmColor) {
        BITMAP bm{};
        GetObjectW(ii.hbmColor, sizeof(bm), &bm);
        const int w = bm.bmWidth, h = bm.bmHeight;
        if (w > 0 && h > 0 && w <= 512 && h <= 512) {
            BITMAPINFO bi{};
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = w;
            bi.bmiHeader.biHeight = -h;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            std::vector<DWORD> px((size_t)w * h);
            HDC dc = GetDC(nullptr);
            bool ok = GetDIBits(dc, ii.hbmColor, 0, h, px.data(), &bi, DIB_RGB_COLORS) == h;
            if (ok) {
                bool hasAlpha = false;
                for (DWORD p : px) {
                    if (p & 0xFF000000) { hasAlpha = true; break; }
                }
                if (!hasAlpha) {
                    std::vector<DWORD> mask((size_t)w * h, 0);
                    bool maskOk = ii.hbmMask && GetDIBits(dc, ii.hbmMask, 0, h, mask.data(), &bi, DIB_RGB_COLORS) == h;
                    for (size_t i = 0; i < px.size(); i++) {
                        const bool transparent = maskOk && (mask[i] & 0x00FFFFFF) != 0;
                        px[i] = transparent ? 0 : (px[i] | 0xFF000000);
                    }
                }
                result = new Bitmap(w, h, PixelFormat32bppARGB);
                BitmapData bd{};
                Rect r(0, 0, w, h);
                if (result && result->LockBits(&r, ImageLockModeWrite, PixelFormat32bppARGB, &bd) == Ok) {
                    for (int y = 0; y < h; y++) {
                        memcpy((BYTE*)bd.Scan0 + (size_t)y * bd.Stride, &px[(size_t)y * w], (size_t)w * 4);
                    }
                    result->UnlockBits(&bd);
                } else {
                    delete result;
                    result = nullptr;
                }
            }
            ReleaseDC(nullptr, dc);
        }
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    if (!result) {
        Bitmap* b = Bitmap::FromHICON(hIcon);
        if (b && b->GetLastStatus() == Ok) result = b;
        else delete b;
    }
    return result;
}

const AppInfo& GetCachedAppInfo(const std::wstring& path, DWORD pid = 0) {
    const std::wstring cacheKey = path.empty() ? (L"pid:" + std::to_wstring(pid)) : path;
    auto it = g_appCache.find(cacheKey);
    if (it != g_appCache.end()) return it->second;

    AppInfo info;
    if (!path.empty()) {
        const size_t slash = path.find_last_of(L"\\/");
        info.exeName = slash == std::wstring::npos ? path : path.substr(slash + 1);
        info.name = Trim(GetFileDescription(path));
        if (info.name.empty()) {
            const size_t dot = info.exeName.find_last_of(L'.');
            info.name = dot == std::wstring::npos ? info.exeName : info.exeName.substr(0, dot);
        }
        SHFILEINFOW sfi{};
        if (SHGetFileInfoW(FileAccessPath(path).c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON) && sfi.hIcon) {
            info.icon = IconToBitmap(sfi.hIcon);
            DestroyIcon(sfi.hIcon);
        }
    } else {
        info.exeName = GetProcessExeNameSnapshot(pid);
        info.name = info.exeName;
        const size_t dot = info.name.find_last_of(L'.');
        if (dot != std::wstring::npos) info.name = info.name.substr(0, dot);
    }

    return g_appCache.emplace(cacheKey, info).first->second;
}

std::wstring GetDeviceName(IMMDevice* dev) {
    std::wstring r = L"Audio device";
    IPropertyStore* ps = nullptr;
    if (SUCCEEDED(dev->OpenPropertyStore(STGM_READ, &ps)) && ps) {
        PROPVARIANT pv;
        PropVariantInit(&pv);
        if (SUCCEEDED(ps->GetValue(kPkeyFriendlyName, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal) {
            r = pv.pwszVal;
        }
        PropVariantClear(&pv);
        ps->Release();
    }
    return r;
}

void SetDefaultAudioDevice(const std::wstring& devId) {
    IPolicyConfig* policy = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_PolicyConfigClient, nullptr, CLSCTX_ALL, IID_IPolicyConfig, (void**)&policy)) && policy) {
        policy->SetDefaultEndpoint(devId.c_str(), eConsole);
        policy->SetDefaultEndpoint(devId.c_str(), eMultimedia);
        policy->Release();
    }
}

bool SetAppAudioEndpoint(DWORD processId, const std::wstring& shortDeviceId) {
    if (!processId) return false;

    HMODULE hComBase = GetModuleHandleW(L"combase.dll");
    if (!hComBase) hComBase = LoadLibraryW(L"combase.dll");
    if (!hComBase) return false;

    typedef HRESULT (WINAPI *pfnWindowsCreateString)(PCWSTR, UINT32, HSTRING*);
    typedef HRESULT (WINAPI *pfnWindowsDeleteString)(HSTRING);
    typedef HRESULT (WINAPI *pfnRoGetActivationFactory)(HSTRING, REFIID, void**);

    auto fnCreateString = (pfnWindowsCreateString)GetProcAddress(hComBase, "WindowsCreateString");
    auto fnDeleteString = (pfnWindowsDeleteString)GetProcAddress(hComBase, "WindowsDeleteString");
    auto fnRoGetFactory = (pfnRoGetActivationFactory)GetProcAddress(hComBase, "RoGetActivationFactory");
    if (!fnCreateString || !fnDeleteString || !fnRoGetFactory) return false;

    HSTRING hClass = nullptr;
    const WCHAR* className = L"Windows.Media.Internal.AudioPolicyConfig";
    if (FAILED(fnCreateString(className, (UINT32)wcslen(className), &hClass)) || !hClass) {
        return false;
    }

    void* pFactoryUnk = nullptr;
    HRESULT hr = fnRoGetFactory(hClass, IID_IAudioPolicyConfigWin11, &pFactoryUnk);
    if (FAILED(hr) || !pFactoryUnk) {
        hr = fnRoGetFactory(hClass, IID_IAudioPolicyConfigWin10, &pFactoryUnk);
    }
    fnDeleteString(hClass);

    if (FAILED(hr) || !pFactoryUnk) {
        return false;
    }

    HSTRING hDev = nullptr;
    if (!shortDeviceId.empty()) {
        std::wstring fullId;
        if (shortDeviceId.rfind(L"\\\\?\\SWD#MMDEVAPI#", 0) == 0) {
            fullId = shortDeviceId;
        } else {
            fullId = L"\\\\?\\SWD#MMDEVAPI#" + shortDeviceId + L"#{e6327cad-dcec-4949-ae8a-991e976a79d2}";
        }
        fnCreateString(fullId.c_str(), (UINT32)fullId.length(), &hDev);
    }

    void** vtbl = *(void***)pFactoryUnk;
    auto fnSet = (pfnSetPersistedDefaultAudioEndpoint)vtbl[25];

    bool ok = false;
    if (fnSet) {
        HRESULT hr1 = fnSet(pFactoryUnk, processId, eRender, eConsole, hDev);
        HRESULT hr2 = fnSet(pFactoryUnk, processId, eRender, eMultimedia, hDev);
        ok = SUCCEEDED(hr1) || SUCCEEDED(hr2);
    }

    if (hDev) fnDeleteString(hDev);
    ((IUnknown*)pFactoryUnk)->Release();
    return ok;
}

// Audio Session Enumeration, Device Aggregation, and Volume Control

// Volume: only when the OSD is enabled. Banner: text message (Deafen), always.
// Always: the configured OSD style even when the OSD is off (drag preview,
// media keys that replace the Windows volume flyout).
enum class OsdKind { Volume, Banner, Always };

void ShowOsd(const std::wstring& name, float vol, bool mute, Bitmap* icon,
             OsdKind kind = OsdKind::Volume, const std::wstring& detail = L"", DWORD durationMs = 0);
void Render();
void HideCustomMenu();
void SyncPopupHookState();
bool GetTrayIconRect(RECT* rc);
void UpdateTrayIcon(bool force = false);

void ReleaseItems(std::vector<Item>& items) {
    for (Item& it : items) {
        if (it.ep) {
            it.ep->Release();
            it.ep = nullptr;
        }
        for (ISimpleAudioVolume* s : it.sav) s->Release();
        it.sav.clear();
        for (IAudioMeterInformation* m : it.meters) m->Release();
        it.meters.clear();
    }
}

static bool LockedLevel(const std::wstring& key, float* level) {
    auto it = g_lockedVolumes.find(key);
    if (it == g_lockedVolumes.end()) return false;
    if (level) *level = it->second;
    return true;
}

// Identity of an app across its processes and sessions: the lower-case image
// path, or the file name / PID for processes whose path can't be read.
std::wstring SessionKeyForPid(DWORD pid, std::wstring* pathOut) {
    std::wstring path = GetProcessPath(pid);
    std::wstring key = ToLower(path);
    if (key.empty()) {
        const std::wstring exe = ToLower(GetProcessExeNameSnapshot(pid));
        key = exe.empty() ? L"pid:" + std::to_wstring(pid) : L"exe:" + exe;
    }
    if (pathOut) *pathOut = std::move(path);
    return key;
}

void AddDevice(IMMDevice* dev, std::vector<Item>& out, bool forceIncludeInactive) {
    LPWSTR devIdStr = nullptr;
    if (FAILED(dev->GetId(&devIdStr)) || !devIdStr) return;
    std::wstring devId = devIdStr;
    CoTaskMemFree(devIdStr);

    // Header Item
    Item header;
    header.kind = 0;
    header.key = L"devhdr:" + devId;
    header.deviceId = devId;
    header.name = GetDeviceName(dev);
    out.push_back(std::move(header));

    // Master Volume Item
    IAudioEndpointVolume* ep = nullptr;
    if (SUCCEEDED(dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&ep)) && ep) {
        Item m;
        m.kind = 1;
        m.key = L"devmaster:" + devId;
        m.deviceId = devId;
        m.name = L10n(StrId::MasterVolume);
        m.ep = ep;
        ep->GetMasterVolumeLevelScalar(&m.vol);
        BOOL mute = FALSE;
        ep->GetMute(&mute);
        m.mute = mute != FALSE;
        m.locked = LockedLevel(m.key, &m.lockedVol);

        IAudioMeterInformation* meter = nullptr;
        if (SUCCEEDED(dev->Activate(IID_IAudioMeterInformation, CLSCTX_ALL, nullptr, (void**)&meter)) && meter) {
            m.meters.push_back(meter);
        }
        out.push_back(std::move(m));
    }

    // App Audio Sessions
    IAudioSessionManager2* mgr = nullptr;
    if (FAILED(dev->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&mgr)) || !mgr) return;

    IAudioSessionEnumerator* enm = nullptr;
    if (SUCCEEDED(mgr->GetSessionEnumerator(&enm)) && enm) {
        int count = 0;
        enm->GetCount(&count);

        std::map<std::wstring, size_t> keyToItem;
        for (int i = 0; i < count; i++) {
            IAudioSessionControl* ctl = nullptr;
            if (FAILED(enm->GetSession(i, &ctl)) || !ctl) continue;

            IAudioSessionControl2* ctl2 = nullptr;
            ctl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&ctl2);
            ctl->Release();
            if (!ctl2) continue;

            if (ctl2->IsSystemSoundsSession() == S_OK) {
                ctl2->Release();
                continue;
            }

            DWORD pid = 0;
            ctl2->GetProcessId(&pid);
            if (!pid) {
                ctl2->Release();
                continue;
            }

            if (g_cfg.hideInactive && !forceIncludeInactive) {
                AudioSessionState st = AudioSessionStateInactive;
                ctl2->GetState(&st);
                if (st != AudioSessionStateActive) {
                    ctl2->Release();
                    continue;
                }
            }

            ISimpleAudioVolume* sav = nullptr;
            ctl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&sav);

            IAudioMeterInformation* meter = nullptr;
            ctl2->QueryInterface(IID_IAudioMeterInformation, (void**)&meter);

            std::wstring path;
            const std::wstring key = SessionKeyForPid(pid, &path);

            if (g_cfg.mergeApps) {
                auto it = keyToItem.find(key);
                if (it != keyToItem.end()) {
                    Item& existing = out[it->second];
                    existing.pids.push_back(pid);
                    if (sav) existing.sav.push_back(sav);
                    if (meter) existing.meters.push_back(meter);
                    ctl2->Release();
                    continue;
                }
            }

            const AppInfo& info = GetCachedAppInfo(path, pid);

            Item app;
            app.kind = 2;
            app.key = key;
            app.deviceId = devId;
            app.pid = pid;
            app.pids.push_back(pid);
            app.name = info.name;
            app.icon = info.icon;
            app.iconOwned = false;
            app.locked = LockedLevel(key, &app.lockedVol);

            if (app.name.empty()) {
                LPWSTR dn = nullptr;
                if (SUCCEEDED(ctl2->GetDisplayName(&dn)) && dn) {
                    if (dn[0] && dn[0] != L'@') app.name = dn;
                    CoTaskMemFree(dn);
                }
                if (app.name.empty()) app.name = L"PID " + std::to_wstring(pid);
            }

            if (sav) {
                sav->GetMasterVolume(&app.vol);
                BOOL mute = FALSE;
                sav->GetMute(&mute);
                app.mute = mute != FALSE;
                app.sav.push_back(sav);
            }
            if (meter) {
                app.meters.push_back(meter);
            }

            keyToItem[key] = out.size();
            out.push_back(std::move(app));
            ctl2->Release();
        }
        enm->Release();
    }
    mgr->Release();
}

// Default device first, then the other active render devices.
void BuildItems(std::vector<Item>& out, bool allDevices, bool forceIncludeInactive = false) {
    IMMDeviceEnumerator* enm = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&enm)) || !enm) return;

    g_deviceList.clear();
    std::wstring defaultId;
    IMMDevice* def = nullptr;
    if (SUCCEEDED(enm->GetDefaultAudioEndpoint(eRender, eMultimedia, &def)) && def) {
        LPWSTR id = nullptr;
        if (SUCCEEDED(def->GetId(&id)) && id) {
            defaultId = id;
            CoTaskMemFree(id);
        }
    } else {
        def = nullptr;
    }
    g_defaultDeviceId = defaultId;

    std::vector<std::pair<IMMDevice*, std::wstring>> devices;
    IMMDeviceCollection* col = nullptr;
    if (SUCCEEDED(enm->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &col)) && col) {
        UINT n = 0;
        col->GetCount(&n);
        for (UINT i = 0; i < n; i++) {
            IMMDevice* d = nullptr;
            if (FAILED(col->Item(i, &d)) || !d) continue;
            LPWSTR dId = nullptr;
            std::wstring id;
            if (SUCCEEDED(d->GetId(&dId)) && dId) {
                id = dId;
                CoTaskMemFree(dId);
                DeviceEntry de;
                de.id = id;
                de.name = GetDeviceName(d);
                g_deviceList.push_back(de);
            }
            devices.emplace_back(d, id);
        }
        col->Release();
    }

    auto addOne = [&](IMMDevice* d, bool isDefault) {
        const size_t start = out.size();
        AddDevice(d, out, forceIncludeInactive);
        for (size_t i = start; i < out.size(); i++) out[i].isDefaultDevice = isDefault;
    };

    if (def) addOne(def, true);
    if (allDevices || !def) {
        for (auto& d : devices) {
            if (def && d.second == defaultId) continue;
            addOne(d.first, false);
            if (!allDevices) break;
        }
    }

    for (auto& d : devices) d.first->Release();
    if (def) def->Release();
    enm->Release();
}

// Identity of the rows (not their values): a change means a re-layout.
std::string SigOf(const std::vector<Item>& items) {
    std::string s;
    for (const auto& it : items) {
        s += std::to_string(it.kind) + ":";
        for (WCHAR c : it.key) s += (char)(c & 0x7F);
        s += ":";
        for (WCHAR c : it.deviceId) s += (char)(c & 0x7F);
        s += it.locked ? ":L|" : ":U|";
    }
    return s;
}

std::wstring DeviceNameOf(const std::wstring& id) {
    for (const auto& d : g_deviceList) {
        if (d.id == id) return d.name;
    }
    return L"";
}

// Sets the level without touching a volume lock (Deafen's temporary ducking).
void SetItemVolumeRaw(Item& it, float v) {
    v = std::clamp(v, 0.0f, 1.0f);
    it.vol = v;
    if (it.ep) it.ep->SetMasterVolumeLevelScalar(v, nullptr);
    for (ISimpleAudioVolume* s : it.sav) s->SetMasterVolume(v, nullptr);
}

// Lock changes are saved once the user stops changing them (a drag or a held
// hotkey changes them many times per second).
void ScheduleLockSave() {
    g_locksDirty = true;
    if (g_hwnd) SetTimer(g_hwnd, ID_TIMER_SAVE_LOCKS, 1000, nullptr);
}

// A user change of a locked item moves its lock to the new level.
void SetItemVolume(Item& it, float v) {
    v = std::clamp(v, 0.0f, 1.0f);
    if (it.locked) {
        it.lockedVol = v;
        g_lockedVolumes[it.key] = v;
        ScheduleLockSave();
    }
    SetItemVolumeRaw(it, v);
}

void SetItemMute(Item& it, bool mute) {
    it.mute = mute;
    if (it.ep) it.ep->SetMute(mute ? TRUE : FALSE, nullptr);
    for (ISimpleAudioVolume* s : it.sav) s->SetMute(mute ? TRUE : FALSE, nullptr);
}

void ToggleMute(Item& it) {
    SetItemMute(it, !it.mute);
}

// VU level with analog meter ballistics: the shown level follows the raw peak
// exponentially, rising over ~50 ms and falling over ~300 ms. The step depends
// on the real time between frames, not on the timer rate, so it stays smooth
// when frames arrive late.
bool UpdatePeakValues() {
    static LARGE_INTEGER s_freq{};
    static LARGE_INTEGER s_last{};
    if (!s_freq.QuadPart) QueryPerformanceFrequency(&s_freq);
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    float dt = 0.016f;
    if (s_last.QuadPart && s_freq.QuadPart) {
        dt = (float)(now.QuadPart - s_last.QuadPart) / (float)s_freq.QuadPart;
    }
    s_last = now;
    dt = std::clamp(dt, 0.001f, 0.1f);
    const float attack = 1.0f - std::exp(-dt / 0.050f);
    const float release = 1.0f - std::exp(-dt / 0.300f);

    bool changed = false;
    for (Item& it : g_items) {
        if (it.kind == 0) continue;
        float target = 0.0f;
        for (IAudioMeterInformation* m : it.meters) {
            float p = 0.0f;
            if (SUCCEEDED(m->GetPeakValue(&p)) && p > target) target = p;
        }
        const float k = target > it.peak ? attack : release;
        const float prev = it.peak;
        float next = prev + (target - prev) * k;
        if (next < 0.002f && target < 0.002f) next = 0.0f;
        // Always advance, so tiny levels keep decaying to zero; repaint only
        // for a visible change or when the meter goes silent.
        it.peak = next;
        if (std::abs(next - prev) > 0.0005f || (next == 0.0f && prev != 0.0f)) changed = true;
    }
    return changed;
}

// Re-reads volume/mute of the visible items through their existing COM
// interfaces. Cheap enough to run several times per second while visible.
bool RefreshValues() {
    bool changed = false;
    for (Item& it : g_items) {
        float v = it.vol;
        BOOL m = it.mute ? TRUE : FALSE;
        if (it.kind == 1 && it.ep) {
            it.ep->GetMasterVolumeLevelScalar(&v);
            it.ep->GetMute(&m);
        } else if (it.kind == 2 && !it.sav.empty()) {
            it.sav[0]->GetMasterVolume(&v);
            it.sav[0]->GetMute(&m);
        } else {
            continue;
        }
        const bool mb = m != FALSE;
        if (std::abs(v - it.vol) > 0.002f || mb != it.mute) {
            it.vol = v;
            it.mute = mb;
            changed = true;
        }
    }
    return changed;
}

////////////////////////////////////////////////////////////////////////////////
// Active App Control (Game Hotkeys)

// Endpoint volume of the device Windows actually plays to.
IAudioEndpointVolume* GetDefaultEndpointVolume(std::wstring* nameOut, std::wstring* idOut = nullptr) {
    IMMDeviceEnumerator* enm = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&enm)) || !enm) {
        return nullptr;
    }
    IAudioEndpointVolume* ep = nullptr;
    IMMDevice* dev = nullptr;
    if (SUCCEEDED(enm->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev)) && dev) {
        if (nameOut) *nameOut = GetDeviceName(dev);
        if (idOut) {
            LPWSTR id = nullptr;
            if (SUCCEEDED(dev->GetId(&id)) && id) {
                *idOut = id;
                CoTaskMemFree(id);
            }
        }
        if (FAILED(dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&ep))) ep = nullptr;
        dev->Release();
    }
    enm->Release();
    return ep;
}

std::wstring FileNameOf(const std::wstring& pathOrKey) {
    if (pathOrKey.rfind(L"exe:", 0) == 0) return ToLower(pathOrKey.substr(4));
    const size_t slash = pathOrKey.find_last_of(L"\\/");
    return ToLower(slash == std::wstring::npos ? pathOrKey : pathOrKey.substr(slash + 1));
}

struct ForegroundTarget {
    DWORD pid = 0;
    std::wstring key;  // lower-case full path (empty for protected processes)
    std::wstring exe;  // lower-case file name
};

static BOOL CALLBACK FindCoreWindowProc(HWND hwnd, LPARAM lParam) {
    DWORD* data = (DWORD*)lParam;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid && pid != data[0]) {
        data[1] = pid;
        return FALSE;
    }
    return TRUE;
}

static void FillTargetNames(ForegroundTarget& t) {
    const std::wstring path = GetProcessPath(t.pid);
    t.key = ToLower(path);
    t.exe = path.empty() ? ToLower(GetProcessExeNameSnapshot(t.pid)) : FileNameOf(path);
}

ForegroundTarget GetForegroundTarget() {
    ForegroundTarget t;
    HWND fg = GetForegroundWindow();
    if (!fg) return t;
    HWND root = GetAncestor(fg, GA_ROOTOWNER);
    if (root) fg = root;
    GetWindowThreadProcessId(fg, &t.pid);
    if (!t.pid || t.pid == GetCurrentProcessId()) {
        t.pid = 0;
        return t;
    }
    FillTargetNames(t);
    // UWP apps are hosted by ApplicationFrameHost; the audio session belongs
    // to the CoreWindow child that lives in the real app process.
    if (t.exe == L"applicationframehost.exe") {
        DWORD data[2] = {t.pid, 0};
        EnumChildWindows(fg, FindCoreWindowProc, (LPARAM)data);
        if (data[1]) {
            t.pid = data[1];
            FillTargetNames(t);
        }
    }
    return t;
}

static bool ItemMatchesTarget(const Item& it, const ForegroundTarget& t) {
    if (it.kind != 2) return false;
    if (t.pid) {
        for (DWORD p : it.pids) {
            if (p == t.pid) return true;
        }
    }
    if (!t.key.empty() && it.key == t.key) return true;
    if (!t.exe.empty() && FileNameOf(it.key) == t.exe) return true;
    return false;
}

// alwaysOsd: shows the OSD even when it is turned off (media keys replacing the
// Windows flyout must always give feedback).
void AdjustMasterVolume(float delta, bool toggleMute, bool alwaysOsd = false) {
    static DWORD s_lastMasterMute = 0;
    if (toggleMute) {
        DWORD now = GetTickCount();
        if (now - s_lastMasterMute < 250) return;
        s_lastMasterMute = now;
    }

    std::wstring devName, devId;
    IAudioEndpointVolume* ep = GetDefaultEndpointVolume(&devName, &devId);
    if (!ep) return;
    float v = 0.0f;
    BOOL m = FALSE;
    ep->GetMasterVolumeLevelScalar(&v);
    ep->GetMute(&m);
    if (toggleMute) {
        m = m ? FALSE : TRUE;
        ep->SetMute(m, nullptr);
    } else {
        v = std::clamp(v + delta, 0.0f, 1.0f);
        ep->SetMasterVolumeLevelScalar(v, nullptr);
        if (m && delta > 0.0f) {
            m = FALSE;
            ep->SetMute(FALSE, nullptr);
        }
        auto lock = g_lockedVolumes.find(L"devmaster:" + devId);
        if (lock != g_lockedVolumes.end()) {
            lock->second = v;
            ScheduleLockSave();
        }
    }
    ep->Release();
    g_osdTarget = OsdTarget::Master;
    g_osdMasterVol = v;
    g_osdMasterMute = m != FALSE;
    ShowOsd(L10n(StrId::MasterVolume), v, m != FALSE, nullptr, alwaysOsd ? OsdKind::Always : OsdKind::Volume, devName);
    if (g_visible && RefreshValues()) Render();
    UpdateTrayIcon();
}

// Changes every session of one app; the loudest session is the reference for
// the new level. The OSD then shows that app.
static void ChangeAppSessions(std::vector<Item*>& matches, float delta, bool toggleMute) {
    Item* ref = matches[0];
    float bestPeak = -1.0f;
    for (Item* m : matches) {
        float p = 0.0f;
        for (IAudioMeterInformation* meter : m->meters) {
            float v = 0.0f;
            if (SUCCEEDED(meter->GetPeakValue(&v)) && v > p) p = v;
        }
        if (p > bestPeak) {
            bestPeak = p;
            ref = m;
        }
    }

    if (toggleMute) {
        bool anyUnmuted = false;
        for (Item* m : matches) {
            if (!m->mute) anyUnmuted = true;
        }
        for (Item* m : matches) SetItemMute(*m, anyUnmuted);
    } else {
        const float nv = std::clamp(ref->vol + delta, 0.0f, 1.0f);
        for (Item* m : matches) {
            SetItemVolume(*m, nv);
            if (m->mute && delta > 0.0f) SetItemMute(*m, false);
        }
    }
    g_osdTarget = OsdTarget::App;
    g_osdTargetKey = ref->key;
    ShowOsd(ref->name, ref->vol, ref->mute, ref->icon, OsdKind::Volume, DeviceNameOf(ref->deviceId));
}

// Applies the change to every session of the focused app on every output
// device, so the hotkey also works when the app plays to a non-default device.
void AdjustForegroundAppVolume(float delta, bool toggleMute) {
    const ForegroundTarget t = GetForegroundTarget();
    std::vector<Item> items;
    // Inactive sessions are included so a muted or silent app can be found and unmuted.
    BuildItems(items, true, true);

    std::vector<Item*> matches;
    for (Item& it : items) {
        if (ItemMatchesTarget(it, t)) matches.push_back(&it);
    }
    if (matches.empty() && g_visible && g_hoverIdx >= 0 && g_hoverIdx < (int)g_items.size() &&
        g_items[g_hoverIdx].kind == 2) {
        const std::wstring hoverKey = g_items[g_hoverIdx].key;
        for (Item& it : items) {
            if (it.kind == 2 && it.key == hoverKey) matches.push_back(&it);
        }
    }
    if (matches.empty()) {
        // The focused app has no audio session. Changing the master volume
        // instead would be a surprise on the desktop or in File Explorer.
        ReleaseItems(items);
        if (g_cfg.enableVolumeOsd) ShowOsd(L10n(StrId::NoAppAudio), 0.0f, false, nullptr, OsdKind::Banner);
        return;
    }

    ChangeAppSessions(matches, delta, toggleMute);
    ReleaseItems(items);
    if (g_visible && RefreshValues()) Render();
}

// Mouse wheel over the OSD of an app: changes that app on every device.
void AdjustAppVolumeByKey(const std::wstring& key, float delta) {
    std::vector<Item> items;
    BuildItems(items, true, true);
    std::vector<Item*> matches;
    for (Item& it : items) {
        if (it.kind == 2 && it.key == key) matches.push_back(&it);
    }
    if (!matches.empty()) ChangeAppSessions(matches, delta, false);
    ReleaseItems(items);
    if (g_visible && RefreshValues()) Render();
}

// Whole wheel notches: high-resolution wheels and touchpads send fractions of
// WHEEL_DELTA.
int TakeWheelNotches(int delta) {
    static int accum = 0;
    if ((accum > 0 && delta < 0) || (accum < 0 && delta > 0)) accum = 0;
    accum += delta;
    const int notches = accum / WHEEL_DELTA;
    accum -= notches * WHEEL_DELTA;
    return notches;
}

// Mouse wheel over the OSD changes what it shows, like the Windows flyout.
void OnOsdWheel(int delta) {
    const int notches = TakeWheelNotches(delta);
    if (!notches) return;
    const float step = notches * g_cfg.wheelStep / 100.0f;
    if (g_osdTarget == OsdTarget::Master) AdjustMasterVolume(step, false, true);
    else if (g_osdTarget == OsdTarget::App && !g_osdTargetKey.empty()) AdjustAppVolumeByKey(g_osdTargetKey, step);
}

void ToggleForegroundAppMute() {
    static DWORD s_lastMuteTick = 0;
    DWORD now = GetTickCount();
    if (now - s_lastMuteTick < 250) return;
    s_lastMuteTick = now;
    AdjustForegroundAppVolume(0.0f, true);
}

////////////////////////////////////////////////////////////////////////////////
// Volume locks (persisted per user, enforced from audio events)

// Windhawk storage is shared by every user of the PC; locks and the Deafen
// state belong to the user whose apps they affect.
std::wstring UserValueName(const wchar_t* base) {
    static std::wstring s_sid;
    if (s_sid.empty()) {
        HANDLE token = nullptr;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            DWORD size = 0;
            GetTokenInformation(token, TokenUser, nullptr, 0, &size);
            std::vector<BYTE> buf(size ? size : 1);
            LPWSTR sidStr = nullptr;
            if (size && GetTokenInformation(token, TokenUser, buf.data(), size, &size) &&
                ConvertSidToStringSidW(((TOKEN_USER*)buf.data())->User.Sid, &sidStr) && sidStr) {
                s_sid = sidStr;
                LocalFree(sidStr);
            }
            CloseHandle(token);
        }
        if (s_sid.empty()) s_sid = L"user";
    }
    return std::wstring(base) + L"_" + s_sid;
}

void PersistLockedVolumes() {
    std::wstring s;
    for (const auto& p : g_lockedVolumes) {
        s += p.first;
        s += L'|';
        s += std::to_wstring((int)std::lround(p.second * 1000.0f));
        s += L'\n';
    }
    Wh_SetStringValue(UserValueName(L"lockedVolumes").c_str(), s.c_str());
    g_locksDirty = false;
}

void LoadLockedVolumes() {
    g_lockedVolumes.clear();
    std::vector<WCHAR> buf(32768, 0);
    if (Wh_GetStringValue(UserValueName(L"lockedVolumes").c_str(), buf.data(), buf.size()) == 0) return;
    const std::wstring s = buf.data();
    size_t pos = 0;
    while (pos < s.size()) {
        size_t nl = s.find(L'\n', pos);
        if (nl == std::wstring::npos) nl = s.size();
        const std::wstring line = s.substr(pos, nl - pos);
        const size_t bar = line.rfind(L'|');
        if (bar != std::wstring::npos && bar > 0) {
            const int v = _wtoi(line.c_str() + bar + 1);
            g_lockedVolumes[line.substr(0, bar)] = std::clamp(v / 1000.0f, 0.0f, 1.0f);
        }
        pos = nl + 1;
    }
}

// Posts msg to the UI thread unless one is already queued. Used by the audio
// callbacks, which run on audio service threads.
void PostCoalesced(std::atomic<bool>& pending, UINT msg) {
    HWND hwnd = g_hwnd;
    if (hwnd && !pending.exchange(true)) {
        if (!PostMessageW(hwnd, msg, 0, 0)) pending = false;
    }
}

// Volume locks are enforced from audio events instead of polling: the sessions
// of locked apps and the locked endpoints report volume changes.
class LockNotifier final : public IAudioSessionEvents, public IAudioEndpointVolumeCallback {
public:
    ULONG STDMETHODCALLTYPE AddRef() override { return (ULONG)InterlockedIncrement(&m_ref); }
    ULONG STDMETHODCALLTYPE Release() override {
        const LONG r = InterlockedDecrement(&m_ref);
        if (r == 0) delete this;
        return (ULONG)r;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == __uuidof(IAudioSessionEvents)) {
            *ppv = static_cast<IAudioSessionEvents*>(this);
        } else if (riid == __uuidof(IAudioEndpointVolumeCallback)) {
            *ppv = static_cast<IAudioEndpointVolumeCallback*>(this);
        } else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }

    // IAudioSessionEvents
    HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(LPCWSTR, LPCGUID) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnIconPathChanged(LPCWSTR, LPCGUID) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(float, BOOL, LPCGUID) override {
        PostCoalesced(g_lockCheckPending, WM_APP_LOCK_CHECK);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(DWORD, float*, DWORD, LPCGUID) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(LPCGUID, LPCGUID) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnStateChanged(AudioSessionState state) override {
        if (state == AudioSessionStateExpired) PostCoalesced(g_lockRebuildPending, WM_APP_LOCK_REBUILD);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnSessionDisconnected(AudioSessionDisconnectReason) override {
        PostCoalesced(g_lockRebuildPending, WM_APP_LOCK_REBUILD);
        return S_OK;
    }

    // IAudioEndpointVolumeCallback
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override {
        PostCoalesced(g_lockCheckPending, WM_APP_LOCK_CHECK);
        return S_OK;
    }

private:
    LONG m_ref = 1;
};

// PID of an app session; 0 for the system sounds session.
static DWORD SessionPid(IAudioSessionControl* ctl) {
    DWORD pid = 0;
    IAudioSessionControl2* ctl2 = nullptr;
    if (SUCCEEDED(ctl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&ctl2)) && ctl2) {
        if (ctl2->IsSystemSoundsSession() != S_OK) ctl2->GetProcessId(&pid);
        ctl2->Release();
    }
    return pid;
}

// Reports the new sessions of one device to the UI thread, which looks at a
// session only when its app is locked or waits for its levels after a Deafen.
class NewSessionNotifier final : public IAudioSessionNotification {
public:
    NewSessionNotifier(WORD index, WORD generation) : m_index(index), m_generation(generation) {}

    ULONG STDMETHODCALLTYPE AddRef() override { return (ULONG)InterlockedIncrement(&m_ref); }
    ULONG STDMETHODCALLTYPE Release() override {
        const LONG r = InterlockedDecrement(&m_ref);
        if (r == 0) delete this;
        return (ULONG)r;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid != IID_IUnknown && riid != __uuidof(IAudioSessionNotification)) {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        *ppv = static_cast<IAudioSessionNotification*>(this);
        AddRef();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnSessionCreated(IAudioSessionControl* ctl) override {
        const DWORD pid = ctl ? SessionPid(ctl) : 0;
        HWND hwnd = g_hwnd;
        if (pid && hwnd) PostMessageW(hwnd, WM_APP_NEW_SESSION, pid, MAKELPARAM(m_index, m_generation));
        return S_OK;
    }

private:
    LONG m_ref = 1;
    const WORD m_index;
    const WORD m_generation;
};

struct LockedSession {
    IAudioSessionControl* ctl = nullptr;
    ISimpleAudioVolume* sav = nullptr;
    std::wstring key;
    std::wstring deviceId;
    DWORD pid = 0;
};

struct LockedEndpoint {
    IAudioEndpointVolume* ep = nullptr;
    std::wstring key;
};

struct WatchedManager {
    IAudioSessionManager2* mgr = nullptr;
    NewSessionNotifier* notifier = nullptr;
    std::wstring deviceId;
};

LockNotifier* g_lockNotifier = nullptr;
std::vector<WatchedManager> g_lockManagers;
std::vector<LockedSession> g_lockSessions;
std::vector<LockedEndpoint> g_lockEndpoints;
WORD g_lockWatchGeneration = 0;  // new-session reports from an older watch are stale

static void ReleaseLockedSession(LockedSession& s) {
    s.ctl->UnregisterAudioSessionNotification(g_lockNotifier);
    s.ctl->Release();
    if (s.sav) s.sav->Release();
}

void StopLockWatch() {
    g_lockWatchGeneration++;
    for (WatchedManager& w : g_lockManagers) {
        w.mgr->UnregisterSessionNotification(w.notifier);
        w.notifier->Release();
        w.mgr->Release();
    }
    g_lockManagers.clear();
    for (LockedSession& s : g_lockSessions) ReleaseLockedSession(s);
    g_lockSessions.clear();
    for (LockedEndpoint& e : g_lockEndpoints) {
        e.ep->UnregisterControlChangeNotify(g_lockNotifier);
        e.ep->Release();
    }
    g_lockEndpoints.clear();
}

// Puts back every locked level that a game, a browser or Windows changed.
// Apps ducked by Deafen are left alone until Deafen is turned off.
void EnforceLockedVolumes() {
    g_lockCheckPending = false;
    for (LockedSession& s : g_lockSessions) {
        float level = 0.0f;
        float v = 0.0f;
        if (!s.sav || !LockedLevel(s.key, &level)) continue;
        if (g_deafenActive && g_deafenSavedStates.count(s.key + L"|" + s.deviceId)) continue;
        if (SUCCEEDED(s.sav->GetMasterVolume(&v)) && std::abs(v - level) > 0.004f) {
            s.sav->SetMasterVolume(level, nullptr);
        }
    }
    for (LockedEndpoint& e : g_lockEndpoints) {
        float level = 0.0f;
        float v = 0.0f;
        if (!LockedLevel(e.key, &level)) continue;
        if (SUCCEEDED(e.ep->GetMasterVolumeLevelScalar(&v)) && std::abs(v - level) > 0.004f) {
            e.ep->SetMasterVolumeLevelScalar(level, nullptr);
        }
    }
}

void RestorePendingDeafen();

// Watches the sessions of locked apps on one device. With onlyPid, just the
// sessions of that process, replacing the ones already watched.
static void WatchLockedSessions(IAudioSessionManager2* mgr, const std::wstring& devId, DWORD onlyPid) {
    if (onlyPid) {
        for (auto it = g_lockSessions.begin(); it != g_lockSessions.end();) {
            if (it->pid == onlyPid && it->deviceId == devId) {
                ReleaseLockedSession(*it);
                it = g_lockSessions.erase(it);
            } else {
                ++it;
            }
        }
    }
    IAudioSessionEnumerator* sessions = nullptr;
    if (FAILED(mgr->GetSessionEnumerator(&sessions)) || !sessions) return;
    int count = 0;
    sessions->GetCount(&count);
    for (int i = 0; i < count; i++) {
        IAudioSessionControl* ctl = nullptr;
        if (FAILED(sessions->GetSession(i, &ctl)) || !ctl) continue;
        const DWORD pid = SessionPid(ctl);
        const std::wstring key = pid && (!onlyPid || pid == onlyPid) ? SessionKeyForPid(pid, nullptr) : std::wstring();
        ISimpleAudioVolume* sav = nullptr;
        if (!key.empty() && LockedLevel(key, nullptr) &&
            SUCCEEDED(ctl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&sav)) && sav &&
            SUCCEEDED(ctl->RegisterAudioSessionNotification(g_lockNotifier))) {
            g_lockSessions.push_back({ctl, sav, key, devId, pid});  // keeps both references
            continue;
        }
        if (sav) sav->Release();
        ctl->Release();
    }
    sessions->Release();
}

static void WatchLockedDevice(IMMDevice* dev, const std::wstring& devId) {
    const std::wstring masterKey = L"devmaster:" + devId;
    if (LockedLevel(masterKey, nullptr)) {
        IAudioEndpointVolume* ep = nullptr;
        if (SUCCEEDED(dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&ep)) && ep) {
            if (SUCCEEDED(ep->RegisterControlChangeNotify(g_lockNotifier))) g_lockEndpoints.push_back({ep, masterKey});
            else ep->Release();
        }
    }

    IAudioSessionManager2* mgr = nullptr;
    if (FAILED(dev->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&mgr)) || !mgr) return;
    NewSessionNotifier* notifier = new NewSessionNotifier((WORD)g_lockManagers.size(), g_lockWatchGeneration);
    if (SUCCEEDED(mgr->RegisterSessionNotification(notifier))) {
        g_lockManagers.push_back({mgr, notifier, devId});  // keeps both references
    } else {
        notifier->Release();
        notifier = nullptr;
    }
    // The session enumerator has to be obtained for new-session notifications to arrive.
    WatchLockedSessions(mgr, devId, 0);
    if (!notifier) mgr->Release();
}

// Called when the locks or the devices change and when a watched session ends.
// New sessions are reported while any lock exists or apps wait to get their
// levels back from a Deafen.
void RebuildLockWatch() {
    g_lockRebuildPending = false;
    StopLockWatch();
    RestorePendingDeafen();
    if (g_lockedVolumes.empty() && g_deafenPending.empty()) return;
    if (!g_lockNotifier) g_lockNotifier = new LockNotifier();

    IMMDeviceEnumerator* enm = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&enm)) || !enm) return;
    IMMDeviceCollection* col = nullptr;
    if (SUCCEEDED(enm->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &col)) && col) {
        UINT n = 0;
        col->GetCount(&n);
        for (UINT i = 0; i < n; i++) {
            IMMDevice* dev = nullptr;
            if (FAILED(col->Item(i, &dev)) || !dev) continue;
            LPWSTR idStr = nullptr;
            if (SUCCEEDED(dev->GetId(&idStr)) && idStr) {
                WatchLockedDevice(dev, idStr);
                CoTaskMemFree(idStr);
            }
            dev->Release();
        }
        col->Release();
    }
    enm->Release();
    EnforceLockedVolumes();
}

void SetVolumeLock(const std::wstring& key, bool locked, float level) {
    if (locked) g_lockedVolumes[key] = std::clamp(level, 0.0f, 1.0f);
    else g_lockedVolumes.erase(key);
    for (Item& it : g_items) {
        if (it.key == key) {
            it.locked = locked;
            it.lockedVol = level;
        }
    }
    PersistLockedVolumes();
    RebuildLockWatch();
}

// A new session costs one process lookup. Only a session of a locked app, or of
// an app waiting for its levels after a Deafen, is looked at further.
void OnNewAudioSession(DWORD pid, WORD index, WORD generation) {
    if (generation != g_lockWatchGeneration || index >= g_lockManagers.size()) return;  // rebuilt since
    IAudioSessionManager2* mgr = g_lockManagers[index].mgr;
    const std::wstring devId = g_lockManagers[index].deviceId;
    const std::wstring key = SessionKeyForPid(pid, nullptr);
    if (LockedLevel(key, nullptr)) {
        WatchLockedSessions(mgr, devId, pid);
        EnforceLockedVolumes();
    }
    if (g_deafenPending.count(key + L"|" + devId)) {
        RestorePendingDeafen();
        if (g_lockedVolumes.empty() && g_deafenPending.empty()) StopLockWatch();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Deafen background

static std::vector<std::wstring> ParseWhitelist() {
    std::vector<std::wstring> whitelist;
    const std::wstring wl = ToLower(g_cfg.deafenWhitelist);
    size_t start = 0;
    while (start < wl.length()) {
        size_t comma = wl.find_first_of(L",;", start);
        if (comma == std::wstring::npos) comma = wl.length();
        std::wstring token = Trim(wl.substr(start, comma - start));
        if (!token.empty()) whitelist.push_back(token);
        start = comma + 1;
    }
    return whitelist;
}

// An entry is a file name (discord.exe or discord), or a folder path that
// spares every app inside it.
static bool IsWhitelisted(const Item& it, const std::vector<std::wstring>& whitelist) {
    const std::wstring exe = FileNameOf(it.key);
    const size_t dot = exe.find_last_of(L'.');
    const std::wstring stem = dot == std::wstring::npos ? exe : exe.substr(0, dot);
    for (const auto& w : whitelist) {
        if (w.find_first_of(L"\\/") != std::wstring::npos) {
            std::wstring folder = w;
            std::replace(folder.begin(), folder.end(), L'/', L'\\');
            if (folder.back() != L'\\') folder += L'\\';
            if (it.key.rfind(folder, 0) == 0) return true;
        } else if (exe == w || stem == w) {
            return true;
        }
    }
    return false;
}

static DWORD DayNumber() {
    FILETIME ft{};
    GetSystemTimeAsFileTime(&ft);
    const ULONGLONG t = ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return (DWORD)(t / (10000000ULL * 60 * 60 * 24));
}

// One line per app: "key|deviceId<TAB>volume x1000<TAB>mute<TAB>ducked<TAB>day".
static std::wstring SerializeDeafen(const std::map<std::wstring, DeafenSavedState>& states) {
    std::wstring s;
    for (const auto& p : states) {
        s += p.first;
        s += L'\t';
        s += std::to_wstring((int)std::lround(p.second.originalVol * 1000.0f));
        s += p.second.originalMute ? L"\t1" : L"\t0";
        s += p.second.ducked ? L"\t1\t" : L"\t0\t";
        s += std::to_wstring(p.second.day);
        s += L'\n';
    }
    return s;
}

// Entries already in `out` are kept (they hold the older, original levels).
static void LoadDeafenStates(const wchar_t* name, std::map<std::wstring, DeafenSavedState>& out) {
    std::vector<WCHAR> buf(65536, 0);
    if (Wh_GetStringValue(UserValueName(name).c_str(), buf.data(), buf.size()) == 0) return;
    const std::wstring s = buf.data();
    size_t start = 0;
    while (start < s.size()) {
        size_t nl = s.find(L'\n', start);
        if (nl == std::wstring::npos) nl = s.size();
        const std::wstring line = s.substr(start, nl - start);
        start = nl + 1;
        const size_t t1 = line.find(L'\t');
        const size_t t2 = t1 == std::wstring::npos ? std::wstring::npos : line.find(L'\t', t1 + 1);
        const size_t t3 = t2 == std::wstring::npos ? std::wstring::npos : line.find(L'\t', t2 + 1);
        if (t3 == std::wstring::npos) continue;
        DeafenSavedState ds;
        ds.originalVol = std::clamp(_wtoi(line.c_str() + t1 + 1) / 1000.0f, 0.0f, 1.0f);
        ds.originalMute = line.compare(t2 + 1, 1, L"1") == 0;
        ds.ducked = line.compare(t3 + 1, 1, L"1") == 0;
        const size_t t4 = line.find(L'\t', t3 + 1);
        ds.day = t4 == std::wstring::npos ? DayNumber() : (DWORD)wcstoul(line.c_str() + t4 + 1, nullptr, 10);
        out.emplace(line.substr(0, t1), ds);
    }
}

// The original levels are kept in the mod storage: while Deafen is on, and
// afterwards for apps that weren't running when it ended. A crash, a sign-out
// or a power loss can't leave the other apps muted or ducked for good.
void PersistDeafenState() {
    Wh_SetStringValue(UserValueName(L"deafenState").c_str(), SerializeDeafen(g_deafenSavedStates).c_str());
    Wh_SetStringValue(UserValueName(L"deafenPending").c_str(), SerializeDeafen(g_deafenPending).c_str());
}

// Restores the entries whose app has an audio session now and removes them.
static size_t RestoreDeafenEntries(std::map<std::wstring, DeafenSavedState>& states) {
    if (states.empty()) return 0;
    std::vector<Item> items;
    // Inactive sessions too: an app that went quiet still has to be restored.
    BuildItems(items, true, true);
    std::vector<std::wstring> restored;
    for (Item& it : items) {
        if (it.kind != 2) continue;
        const std::wstring id = it.key + L"|" + it.deviceId;
        auto found = states.find(id);
        if (found == states.end()) continue;
        if (found->second.ducked) SetItemVolumeRaw(it, found->second.originalVol);
        SetItemMute(it, found->second.originalMute);
        restored.push_back(id);
    }
    ReleaseItems(items);
    for (const auto& id : restored) states.erase(id);
    return restored.size();
}

// Apps that weren't running get their levels back as soon as they play again
// (called on every new audio session while any are waiting).
void RestorePendingDeafen() {
    if (!g_deafenPending.empty() && RestoreDeafenEntries(g_deafenPending) > 0) PersistDeafenState();
}

void RestoreDeafen(bool showBanner) {
    if (!g_deafenActive) return;
    RestoreDeafenEntries(g_deafenSavedStates);
    const DWORD today = DayNumber();
    for (auto& p : g_deafenSavedStates) {
        p.second.day = today;
        g_deafenPending.emplace(p.first, p.second);
    }
    g_deafenSavedStates.clear();
    g_deafenActive = false;
    PersistDeafenState();
    RebuildLockWatch();  // locks again, and watches new sessions for pending apps
    if (showBanner) ShowOsd(L10n(StrId::DeafenOff), 1.0f, false, nullptr, OsdKind::Banner);
}

// Drops waiting entries whose app can't play again: a PID key from an earlier
// run, an app that was uninstalled, a device that Windows no longer knows, or
// an entry older than 30 days.
static void PrunePendingDeafen() {
    if (g_deafenPending.empty()) return;
    const DWORD today = DayNumber();
    IMMDeviceEnumerator* enm = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&enm);
    size_t dropped = 0;
    for (auto it = g_deafenPending.begin(); it != g_deafenPending.end();) {
        const size_t bar = it->first.rfind(L'|');
        const std::wstring key = it->first.substr(0, bar);
        const std::wstring devId = bar == std::wstring::npos ? std::wstring() : it->first.substr(bar + 1);
        bool gone = key.rfind(L"pid:", 0) == 0 || (LONG)(today - it->second.day) > 30;
        if (!gone && key.size() > 2 && key[1] == L':' &&
            GetFileAttributesW(FileAccessPath(key).c_str()) == INVALID_FILE_ATTRIBUTES) {
            const DWORD err = GetLastError();
            gone = err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND;
        }
        IMMDevice* dev = nullptr;
        if (!gone && enm && enm->GetDevice(devId.c_str(), &dev) == HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) gone = true;
        if (dev) dev->Release();
        if (gone) {
            it = g_deafenPending.erase(it);
            dropped++;
        } else {
            ++it;
        }
    }
    if (enm) enm->Release();
    if (dropped) PersistDeafenState();
}

// At start: restores what a Deafen interrupted by a crash, a sign-out or a
// power loss left muted or ducked, now or when those apps play again.
void RecoverDeafenState() {
    LoadDeafenStates(L"deafenPending", g_deafenPending);
    PrunePendingDeafen();
    std::map<std::wstring, DeafenSavedState> interrupted;
    LoadDeafenStates(L"deafenState", interrupted);
    if (interrupted.empty()) {
        RestorePendingDeafen();
        return;
    }
    Wh_Log(L"Restoring %d apps from an interrupted Deafen", (int)interrupted.size());
    g_deafenSavedStates = std::move(interrupted);
    g_deafenActive = true;
    RestoreDeafen(false);
}

void ToggleDeafenBackground() {
    if (g_deafenActive) {
        RestoreDeafen(true);
    } else {
        if (!g_cfg.enableDeafenBackground) return;
        const ForegroundTarget t = GetForegroundTarget();
        const std::vector<std::wstring> whitelist = ParseWhitelist();
        const bool duck = (g_cfg.deafenMode == L"duck");
        const float duckVol = std::clamp(g_cfg.deafenDuckPercent / 100.0f, 0.0f, 1.0f);

        std::vector<Item> items;
        BuildItems(items, true, true);
        std::wstring activeName;
        Bitmap* activeIcon = nullptr;
        g_deafenSavedStates.clear();
        for (Item& it : items) {
            if (it.kind != 2) continue;
            if (ItemMatchesTarget(it, t)) {
                if (activeName.empty()) {
                    activeName = it.name;
                    activeIcon = it.icon;
                }
                continue;
            }
            if (IsWhitelisted(it, whitelist)) continue;

            DeafenSavedState ds;
            ds.originalVol = it.vol;
            ds.originalMute = it.mute;
            ds.ducked = duck;
            g_deafenSavedStates[it.key + L"|" + it.deviceId] = ds;
            if (duck) {
                if (it.vol > duckVol) SetItemVolumeRaw(it, duckVol);
            } else {
                SetItemMute(it, true);
            }
        }
        ReleaseItems(items);
        g_deafenActive = true;
        PersistDeafenState();
        if (activeName.empty()) activeName = L10n(StrId::ActiveApp);
        ShowOsd(std::wstring(L10n(StrId::DeafenOn)) + L": " + activeName, 1.0f, false, activeIcon, OsdKind::Banner);
    }
    if (g_visible && RefreshValues()) Render();
}

////////////////////////////////////////////////////////////////////////////////
// Layout & Rendering

float GetMonitorDpiScale(HMONITOR mon) {
    typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);
    static GetDpiForMonitor_t s_fn = nullptr;
    static bool s_tried = false;
    if (!s_tried) {
        s_tried = true;
        HMODULE h = GetModuleHandleW(L"shcore.dll");
        if (!h) h = LoadLibraryW(L"shcore.dll");
        if (h) s_fn = (GetDpiForMonitor_t)GetProcAddress(h, "GetDpiForMonitor");
    }
    UINT dx = 0, dy = 0;
    if (s_fn && mon && SUCCEEDED(s_fn(mon, 0 /* MDT_EFFECTIVE_DPI */, &dx, &dy)) && dx >= 48) {
        return dx / 96.0f;
    }
    HDC dc = GetDC(nullptr);
    const int d = dc ? GetDeviceCaps(dc, LOGPIXELSX) : 96;
    if (dc) ReleaseDC(nullptr, dc);
    return d >= 48 ? d / 96.0f : 1.0f;
}

bool IsDarkColor(const Color& c) {
    return (c.GetR() * 299 + c.GetG() * 587 + c.GetB() * 114) / 1000 < 140;
}

Color WithAlpha(const Color& c, int a) {
    return Color((BYTE)std::clamp(a, 0, 255), c.GetR(), c.GetG(), c.GetB());
}

Color ContrastOn(const Color& bg) {
    return IsDarkColor(bg) ? Color(255, 255, 255, 255) : Color(255, 20, 20, 24);
}

void FillRoundRect(Graphics& g, const Color& c, float x, float y, float w, float h, float r) {
    if (w <= 0.5f || h <= 0.5f) return;
    GraphicsPath p;
    RoundRectPath(p, x, y, w, h, r);
    SolidBrush b(c);
    g.FillPath(&b, &p);
}

void StrokeRoundRect(Graphics& g, const Color& c, float width, float x, float y, float w, float h, float r) {
    if (w <= 0.5f || h <= 0.5f) return;
    GraphicsPath p;
    RoundRectPath(p, x, y, w, h, r);
    Pen pen(c, width);
    g.DrawPath(&pen, &p);
}

const Color kLockColor(240, 245, 158, 11);
std::wstring g_layoutMode = L"modern_cards";  // effective layout of the open popup

std::wstring EffectiveLayoutMode() {
    const std::wstring& t = g_activeTrayPopupStyle;
    if (t == L"system_flyout") return L"fluent_flyout";
    if (t == L"compact_fader") return L"compact_strip";
    if (t == L"mini_island") return L"minimal_pill";
    if (t == L"radial_dial") return L"circular_hud";
    if (t == L"vertical_dock") return L"vertical_dock";
    if (t == L"control_center") return L"control_center";
    if (g_cfg.layoutMode.empty()) return L"modern_cards";
    return g_cfg.layoutMode;
}

// All values are in logical pixels at 100 % and are scaled with S().
struct LayoutMetrics {
    float pad = 14, gap = 6, iconSz = 20, valueW = 34, headerH = 28;
    float masterH = 42, appH = 50, appHNoName = 36, trackH = 6, btnSz = 18;
    bool names = true;     // separate name line above the slider
    bool headers = true;   // device header rows
    bool columns = false;  // vertical channel strips
    float colW = 58, colH = 214;
};

LayoutMetrics GetLayoutMetrics(const std::wstring& l) {
    LayoutMetrics m;
    if (l == L"compact_strip") {
        m.pad = 10; m.gap = 4; m.iconSz = 18; m.valueW = 28; m.headerH = 24;
        m.masterH = 32; m.appH = 42; m.appHNoName = 30; m.trackH = 4; m.btnSz = 16;
    } else if (l == L"hifi_meters") {
        m.pad = 16; m.gap = 8; m.iconSz = 22; m.valueW = 36; m.headerH = 30; m.masterH = 48; m.appH = 58; m.appHNoName = 42; m.trackH = 8;
    } else if (l == L"fluent_flyout") {
        m.pad = 16; m.headerH = 28; m.masterH = 46; m.appH = 54; m.appHNoName = 40; m.trackH = 4;
    } else if (l == L"circular_hud") {
        m.pad = 12; m.gap = 8; m.iconSz = 56; m.valueW = 36; m.headerH = 28; m.masterH = 72; m.appH = 72; m.appHNoName = 72; m.trackH = 4;
    } else if (l == L"studio_console") {
        m.headerH = 30; m.masterH = 52; m.appH = 60; m.appHNoName = 46; m.trackH = 4;
    } else if (l == L"minimal_pill") {
        m.pad = 8; m.gap = 4; m.iconSz = 16; m.valueW = 30; m.masterH = 38; m.appH = 38; m.appHNoName = 38;
        m.trackH = 4; m.btnSz = 16; m.names = false; m.headers = false;
    } else if (l == L"meter_bridge") {
        m.headerH = 30; m.masterH = 48; m.appH = 58; m.appHNoName = 42; m.trackH = 8;
    } else if (l == L"arcade_panel") {
        m.headerH = 30; m.masterH = 50; m.appH = 60; m.appHNoName = 44; m.trackH = 10;
    } else if (l == L"broadcast_console") {
        m.headerH = 32; m.masterH = 58; m.appH = 66; m.appHNoName = 50; m.trackH = 10;
    } else if (l == L"retro_cassette") {
        m.headerH = 30; m.masterH = 46; m.appH = 56; m.appHNoName = 42; m.trackH = 8;
    } else if (l == L"control_center") {
        m.pad = 12; m.iconSz = 18; m.valueW = 0; m.headerH = 28; m.masterH = 50; m.appH = 50; m.appHNoName = 50;
        m.trackH = 34; m.names = false;
    } else if (l == L"glass_tiles") {
        m.pad = 14; m.gap = 8; m.iconSz = 22; m.headerH = 30; m.masterH = 52; m.appH = 62; m.appHNoName = 48; m.trackH = 8;
    } else if (l == L"terminal_retro") {
        m.pad = 12; m.gap = 4; m.iconSz = 16; m.valueW = 44; m.headerH = 26; m.masterH = 36; m.appH = 48; m.appHNoName = 34; m.trackH = 10;
    } else if (l == L"neumorphic") {
        m.pad = 16; m.gap = 8; m.headerH = 30; m.masterH = 52; m.appH = 60; m.appHNoName = 48; m.trackH = 8;
    } else if (l == L"vertical_dock") {
        m.pad = 10; m.iconSz = 22; m.trackH = 8; m.columns = true; m.headers = false; m.names = false;
    }
    return m;
}

void Layout() {
    g_layoutMode = EffectiveLayoutMode();
    const LayoutMetrics m = GetLayoutMetrics(g_layoutMode);
    const bool trayFlyout = (g_activeTrayPopupStyle == L"system_flyout");
    const int pad = S(m.pad);
    const int gap = S(m.gap);
    const int btnSz = std::max(10, S(m.btnSz));
    const int iconSz = std::max(10, S(m.iconSz));

    int appCount = 0, deviceCount = 0;
    bool haveDefault = false;
    for (const Item& it : g_items) {
        if (it.kind == 2) appCount++;
        if (it.kind == 0) deviceCount++;
        if (it.isDefaultDevice) haveDefault = true;
    }
    // The Windows 11 style flyout starts with the default device only.
    const bool collapsed = trayFlyout && !g_trayExpanded;

    g_colSeparators.clear();
    SetRectEmpty(&g_trayExpandBtnRc);
    for (Item& it : g_items) {
        it.firstDevice = false;
        it.verticalSlider = false;
        it.column = false;
        SetRectEmpty(&it.nameRc);
        SetRectEmpty(&it.pillRc);
        SetRectEmpty(&it.iconRc);
        SetRectEmpty(&it.routeBtnRc);
        it.rowTop = it.rowBottom = -9999;
        it.trackTop = it.trackBottom = 0;
    }
    auto isHidden = [&](const Item& it) {
        if (collapsed && (it.kind == 2 || (haveDefault && !it.isDefaultDevice))) return true;
        if (it.kind == 0 && (!m.headers || m.columns)) return true;
        return false;
    };

    if (m.columns) {
        const int colW = S(m.colW), colH = S(m.colH), top = S(10);
        int x = pad;
        bool anyColumn = false;
        bool pendingSeparator = false;
        for (Item& it : g_items) {
            if (it.kind == 0) {
                if (anyColumn) pendingSeparator = true;
                continue;
            }
            if (isHidden(it)) continue;
            if (pendingSeparator) {
                g_colSeparators.push_back(x + S(4));
                x += S(8);
                pendingSeparator = false;
            }
            anyColumn = true;
            it.column = true;
            it.verticalSlider = true;
            it.colL = x;
            it.colR = x + colW;
            it.rowTop = top;
            it.rowBottom = top + colH;
            const int cx = x + colW / 2;
            const int tw = std::max(4, S(m.trackH));
            it.trackL = cx - tw / 2;
            it.trackR = it.trackL + tw;
            it.trackTop = top + S(30);
            it.trackBottom = top + colH - S(72);
            it.trackCy = (it.trackTop + it.trackBottom) / 2;
            it.trackH = tw;
            it.iconRc = {cx - iconSz / 2, it.trackBottom + S(14), cx - iconSz / 2 + iconSz, it.trackBottom + S(14) + iconSz};
            it.routeBtnRc = {cx - btnSz / 2, it.iconRc.bottom + S(8), cx - btnSz / 2 + btnSz, it.iconRc.bottom + S(8) + btnSz};
            x += colW;
        }
        g_winW = std::max(x + pad, S(160));
        g_winH = top + colH + S(10);
        return;
    }

    // Width follows the scale so nothing is clipped at 200-300 %.
    g_winW = S((float)std::clamp(g_cfg.width, 200, 1200));
    const int W = g_winW;
    int y = S(10);
    bool first = true;
    for (Item& it : g_items) {
        if (isHidden(it)) continue;
        it.colL = 0;
        it.colR = W;
        if (it.kind == 0) {
            if (!first) y += gap + S(6);
            it.firstDevice = first;
            first = false;
            it.rowTop = y;
            y += S(m.headerH);
            it.rowBottom = y;
            continue;
        }
        first = false;

        const bool withName = g_cfg.showNames && m.names && it.kind == 2;
        const float rowUnits = it.kind == 1 ? m.masterH : (withName ? m.appH : m.appHNoName);
        it.rowTop = y;
        y += S(rowUnits);
        it.rowBottom = y;
        const int rowH = it.rowBottom - it.rowTop;
        const int cy = it.rowTop + rowH / 2;

        int trackCy = cy;
        if (withName) {
            const int nameTop = it.rowTop + S(5);
            it.nameRc = {0, nameTop, 0, nameTop + S(18)};
            trackCy = it.nameRc.bottom + (it.rowBottom - it.nameRc.bottom) / 2 - S(1);
        }

        if (g_layoutMode == L"circular_hud") {
            const int d = std::max(S(20), std::min(iconSz, rowH - S(12)));
            it.iconRc = {pad, cy - d / 2, pad + d, cy - d / 2 + d};
        } else {
            it.iconRc = {pad, cy - iconSz / 2, pad + iconSz, cy - iconSz / 2 + iconSz};
        }
        it.routeBtnRc = {W - pad - btnSz, trackCy - btnSz / 2, W - pad, trackCy - btnSz / 2 + btnSz};
        it.trackL = it.iconRc.right + S(12);
        it.trackR = std::max(it.trackL + S(30), (int)it.routeBtnRc.left - S(m.valueW) - S(8));
        it.trackCy = trackCy;
        it.trackH = std::max(3, S(m.trackH));
        if (withName) {
            it.nameRc.left = it.trackL;
            it.nameRc.right = it.routeBtnRc.left - S(6);
        }

        if (g_layoutMode == L"control_center") {
            // The whole pill is the slider; the icon inside it toggles mute.
            const int ph = std::max(S(18), std::min(S(m.trackH), rowH - S(10)));
            it.routeBtnRc = {W - pad - btnSz, cy - btnSz / 2, W - pad, cy - btnSz / 2 + btnSz};
            it.pillRc = {pad, cy - ph / 2, it.routeBtnRc.left - S(8), cy - ph / 2 + ph};
            const int ic = std::min(iconSz, ph - S(8));
            it.iconRc = {it.pillRc.left + S(10), cy - ic / 2, it.pillRc.left + S(10) + ic, cy - ic / 2 + ic};
            it.trackL = it.pillRc.left;
            it.trackR = it.pillRc.right;
            it.trackCy = cy;
            it.trackH = ph;
        }
    }

    if (trayFlyout && (appCount > 0 || deviceCount > 1)) {
        y += S(8);
        g_trayExpandBtnRc = {pad, y, W - pad, y + S(30)};
        y += S(30);
    }
    g_winH = y + S(10);
}

// Specialized Slider Renderers for all Layouts

void DrawSlider(Graphics& g, const Item& it) {
    const Theme& th = g_theme;
    const std::wstring& layout = g_layoutMode;

    const bool isHiFi = (layout == L"hifi_meters");
    const bool isBroadcast = (layout == L"broadcast_console");
    const bool isConsole = (layout == L"studio_console" || isBroadcast);
    const bool isFluent = (layout == L"fluent_flyout");
    const bool isCyber = (layout == L"cyberpunk_neon");
    const bool isCassette = (layout == L"retro_cassette");
    const bool isMeterBridge = (layout == L"meter_bridge");
    const bool isArcade = (layout == L"arcade_panel");
    const bool isControlCenter = (layout == L"control_center");
    const bool isTerminal = (layout == L"terminal_retro");
    const bool isNeumorphic = (layout == L"neumorphic");

    const float vol = std::clamp(it.vol, 0.0f, 1.0f);
    const float peak = std::clamp(it.peak, 0.0f, 1.0f);
    const Color fill = it.locked ? kLockColor : th.trackFill;

    // =========================================================================
    // Vertical channel strip (vertical_dock)
    // =========================================================================
    if (it.verticalSlider) {
        const float x = (float)it.trackL;
        const float w = (float)(it.trackR - it.trackL);
        const float top = (float)it.trackTop;
        const float bottom = (float)it.trackBottom;
        const float h = std::max(4.0f, bottom - top);
        FillRoundRect(g, th.trackBg, x, top, w, h, w / 2.0f);
        if (!it.mute && vol > 0.005f) {
            const float fh = std::max(w, h * vol);
            FillRoundRect(g, fill, x, bottom - fh, w, fh, w / 2.0f);
            if (g_cfg.vuMeters && peak > 0.01f) {
                const float ph = std::min(h * peak, fh);
                if (ph > w) FillRoundRect(g, WithAlpha(ContrastOn(fill), 110), x, bottom - ph, w, ph, w / 2.0f);
            }
        }
        const float capW = (float)S(24.0f);
        const float capH = (float)S(10.0f);
        const float capY = bottom - h * vol - capH / 2.0f;
        const float capX = x + w / 2.0f - capW / 2.0f;
        FillRoundRect(g, it.mute ? WithAlpha(th.thumb, 110) : th.thumb, capX, capY, capW, capH, (float)S(3.0f));
        StrokeRoundRect(g, WithAlpha(Color(255, 0, 0, 0), 90), 1.0f, capX, capY, capW, capH, (float)S(3.0f));
        Pen grip(WithAlpha(Color(255, 0, 0, 0), 120), 1.0f);
        g.DrawLine(&grip, capX + S(5.0f), capY + capH / 2.0f, capX + capW - S(5.0f), capY + capH / 2.0f);
        return;
    }

    const float trackL = (float)it.trackL;
    const float trackR = (float)it.trackR;
    const float trackW = std::max(4.0f, trackR - trackL);
    const float trackH = (float)it.trackH;
    const float trackCy = (float)it.trackCy;
    const float trackY = trackCy - trackH / 2.0f;
    const float rad = trackH / 2.0f;

    // =========================================================================
    // 1. Control Center thick capsule slider
    // =========================================================================
    if (isControlCenter) {
        FillRoundRect(g, WithAlpha(th.text, 34), trackL, trackY, trackW, trackH, rad);
        if (vol > 0.01f && !it.mute) {
            const float fillW = std::max(trackH, trackW * vol);
            FillRoundRect(g, fill, trackL, trackY, fillW, trackH, rad);
            if (g_cfg.vuMeters && peak > 0.01f) {
                const float pw = std::min(trackW * peak, fillW);
                if (pw > trackH) FillRoundRect(g, WithAlpha(ContrastOn(fill), 45), trackL, trackY, pw, trackH, rad);
            }
        }
        StrokeRoundRect(g, WithAlpha(th.text, 40), 1.0f, trackL, trackY, trackW, trackH, rad);
        return;
    }

    // =========================================================================
    // 2. Meter Bridge: Discrete LED Ladder (Green / Amber / Red)
    // =========================================================================
    if (isMeterBridge) {
        const int numSegments = 16;
        const float segGap = (float)S(2.0f);
        const float segW = std::max(2.0f, (trackW - segGap * (numSegments - 1)) / numSegments);
        const int activeSegs = it.mute ? 0 : (int)std::lround(vol * numSegments);
        const int peakSeg = (it.mute || !g_cfg.vuMeters || peak < 0.01f) ? -1 : (int)std::lround(peak * (numSegments - 1));

        for (int s = 0; s < numSegments; s++) {
            const float sx = trackL + s * (segW + segGap);
            Color litCol;
            Color unlitCol;
            if (s < 10) {
                litCol = Color(240, 34, 197, 94);
                unlitCol = Color(40, 34, 197, 94);
            } else if (s < 13) {
                litCol = Color(245, 234, 179, 8);
                unlitCol = Color(40, 234, 179, 8);
            } else {
                litCol = Color(245, 239, 68, 68);
                unlitCol = Color(40, 239, 68, 68);
            }
            if (it.locked) litCol = kLockColor;
            const bool isLit = (s < activeSegs) || (s == peakSeg);
            SolidBrush br(isLit ? litCol : unlitCol);
            g.FillRectangle(&br, sx, trackY, segW, trackH);
        }
        return;
    }

    // =========================================================================
    // 3. Retro Terminal CRT Monospace Blocks
    // =========================================================================
    if (isTerminal) {
        const int numBlocks = 18;
        const float bGap = (float)S(1.5f);
        const float bW = std::max(2.0f, (trackW - bGap * (numBlocks - 1)) / numBlocks);
        const int litCount = it.mute ? 0 : (int)std::lround(vol * numBlocks);
        for (int b = 0; b < numBlocks; b++) {
            const float bx = trackL + b * (bW + bGap);
            if (b < litCount) {
                SolidBrush br(fill);
                g.FillRectangle(&br, bx, trackY, bW, trackH);
            } else {
                Pen p(WithAlpha(th.trackFill, 70), 1.0f);
                g.DrawRectangle(&p, bx, trackY, bW, trackH);
            }
        }
        return;
    }

    // =========================================================================
    // 4. Hi-Fi Meters & Studio / Broadcast Desks (tick marks)
    // =========================================================================
    if (isHiFi || isConsole) {
        Pen tickPen(WithAlpha(th.text, 90), 1.0f);
        const float ticks[] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};
        for (float t : ticks) {
            const float tx = trackL + trackW * t;
            g.DrawLine(&tickPen, tx, trackY - S(4.0f), tx, trackY - S(1.0f));
        }
    }

    // Background track
    if (isNeumorphic) {
        // Inset groove: dark upper edge, light lower edge
        FillRoundRect(g, Color(70, 0, 0, 0), trackL, trackY - 1.0f, trackW, trackH, rad);
        FillRoundRect(g, Color(40, 255, 255, 255), trackL, trackY + 1.0f, trackW, trackH, rad);
        FillRoundRect(g, th.trackBg, trackL, trackY, trackW, trackH, rad);
    } else {
        FillRoundRect(g, isCassette ? Color(70, 20, 15, 10) : th.trackBg, trackL, trackY, trackW, trackH, rad);
    }
    if (isCyber) {
        StrokeRoundRect(g, Color(140, 0, 240, 255), 1.2f, trackL, trackY, trackW, trackH, rad);
    } else if (isArcade) {
        StrokeRoundRect(g, Color(200, 255, 230, 0), 1.0f, trackL, trackY, trackW, trackH, rad);
    }

    // Active fill
    const float fillW = trackW * vol;
    if (fillW > 1.0f && !it.mute) {
        const Color fillCol = it.locked ? kLockColor :
                              isCyber ? Color(255, 0, 240, 255) :
                              isCassette ? Color(245, 230, 160, 40) :
                              isArcade ? Color(245, 255, 50, 100) : th.trackFill;
        FillRoundRect(g, fillCol, trackL, trackY, fillW, trackH, rad);

        // Peak / VU meter overlay
        if (g_cfg.vuMeters && peak > 0.01f) {
            const float peakW = std::min(trackW * peak, fillW);
            if (peakW > 2.0f) {
                if (isHiFi || isConsole) {
                    const Color peakCol = (peak > 0.85f) ? Color(230, 240, 60, 60) :
                                          (peak > 0.65f) ? Color(230, 240, 190, 40) :
                                          Color(200, 50, 215, 100);
                    FillRoundRect(g, peakCol, trackL, trackY, peakW, trackH, rad);
                } else {
                    FillRoundRect(g, WithAlpha(ContrastOn(fillCol), 110), trackL, trackY, peakW, trackH, rad);
                }
            }
        }
    }

    // Broadcast desk: dual VU strip under the fader
    if (isBroadcast && g_cfg.vuMeters) {
        const float vuH = (float)std::max(2, S(3.0f));
        const float vuY = trackY + trackH + S(4.0f);
        for (int ch = 0; ch < 2; ch++) {
            const float y = vuY + ch * (vuH + S(2.0f));
            FillRoundRect(g, WithAlpha(th.text, 30), trackL, y, trackW, vuH, vuH / 2.0f);
            const float lvl = it.mute ? 0.0f : peak * (ch == 0 ? 1.0f : 0.92f);
            if (lvl > 0.01f) {
                const Color c = lvl > 0.85f ? Color(235, 240, 60, 60) : lvl > 0.65f ? Color(235, 240, 190, 40) : Color(220, 50, 215, 100);
                FillRoundRect(g, c, trackL, y, trackW * lvl, vuH, vuH / 2.0f);
            }
        }
    }

    // Thumb / fader cap
    if (!it.mute) {
        const float thumbX = trackL + trackW * vol;
        if (isConsole) {
            const float fw = (float)S(10.0f);
            const float fh = (float)S(18.0f);
            FillRoundRect(g, Color(245, 220, 225, 230), thumbX - fw / 2, trackCy - fh / 2, fw, fh, (float)S(2.0f));
            Pen capLine(Color(220, 30, 35, 45), 1.2f);
            g.DrawLine(&capLine, thumbX, trackCy - fh / 2 + S(3.0f), thumbX, trackCy + fh / 2 - S(3.0f));
            StrokeRoundRect(g, Color(160, 0, 0, 0), 1.0f, thumbX - fw / 2, trackCy - fh / 2, fw, fh, (float)S(2.0f));
        } else if (isCassette) {
            const float fw = (float)S(8.0f);
            const float fh = (float)S(16.0f);
            SolidBrush cBr(Color(245, 210, 170, 110));
            g.FillRectangle(&cBr, thumbX - fw / 2, trackCy - fh / 2, fw, fh);
            Pen cPen(Color(200, 50, 40, 30), 1.0f);
            g.DrawRectangle(&cPen, thumbX - fw / 2, trackCy - fh / 2, fw, fh);
        } else {
            const float thumbR = (float)S(isHiFi ? 6.5f : (isFluent ? 7.0f : 5.5f));
            const Color thumbColor = isCyber ? Color(255, 255, 0, 128) :
                                     (isHiFi ? Color(245, 220, 225, 230) : th.thumb);
            if (isFluent) {
                // Windows 11 style: ring with an accent dot
                SolidBrush ring(IsDarkColor(th.bg) ? Color(255, 69, 69, 69) : Color(255, 255, 255, 255));
                g.FillEllipse(&ring, thumbX - thumbR, trackCy - thumbR, thumbR * 2.0f, thumbR * 2.0f);
                const float inner = thumbR * 0.55f;
                SolidBrush dot(fill);
                g.FillEllipse(&dot, thumbX - inner, trackCy - inner, inner * 2.0f, inner * 2.0f);
            } else {
                SolidBrush thBr(thumbColor);
                g.FillEllipse(&thBr, thumbX - thumbR, trackCy - thumbR, thumbR * 2.0f, thumbR * 2.0f);
                Pen thPen(WithAlpha(Color(255, 0, 0, 0), 70), 1.0f);
                g.DrawEllipse(&thPen, thumbX - thumbR, trackCy - thumbR, thumbR * 2.0f, thumbR * 2.0f);
            }
        }
    }
}

// UI Drawing, Layered Window Blit, Positioning, and Animation

void DrawLockBadge(Graphics& g, float cx, float cy, float sz, const Color& bg) {
    SolidBrush back(WithAlpha(bg, 235));
    g.FillEllipse(&back, cx - sz * 0.62f, cy - sz * 0.62f, sz * 1.24f, sz * 1.24f);
    const float bw = sz * 0.70f;
    const float bh = sz * 0.50f;
    const float by = cy - bh / 2.0f + sz * 0.14f;
    const float sw = bw * 0.62f;
    Pen shackle(kLockColor, std::max(1.2f, sz * 0.13f));
    g.DrawArc(&shackle, cx - sw / 2.0f, by - sw * 0.62f, sw, sw * 1.24f, 180.0f, 180.0f);
    FillRoundRect(g, kLockColor, cx - bw / 2.0f, by, bw, bh, sz * 0.10f);
}

void DrawRouteButton(Graphics& g, const RECT& rc, const Color& ink) {
    const float btnX = (float)rc.left;
    const float btnY = (float)rc.top;
    const float btnW = (float)(rc.right - rc.left);
    const float btnH = (float)(rc.bottom - rc.top);
    FillRoundRect(g, WithAlpha(ink, 22), btnX, btnY, btnW, btnH, (float)S(4.0f));
    StrokeRoundRect(g, WithAlpha(ink, 45), 1.0f, btnX, btnY, btnW, btnH, (float)S(4.0f));

    Pen arrPen(WithAlpha(ink, 215), std::max(1.2f, (float)S(1.2f)));
    arrPen.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
    const float cyArr = btnY + btnH / 2.0f;
    const float span = btnW * 0.52f;
    const float ax = btnX + (btnW - span) / 2.0f;
    const float head = btnW * 0.14f;
    const float off = btnH * 0.14f;

    const float yTop = cyArr - off;
    g.DrawLine(&arrPen, ax, yTop, ax + span, yTop);
    g.DrawLine(&arrPen, ax + span - head, yTop - head * 0.8f, ax + span, yTop);
    g.DrawLine(&arrPen, ax + span - head, yTop + head * 0.8f, ax + span, yTop);

    const float yBot = cyArr + off;
    g.DrawLine(&arrPen, ax, yBot, ax + span, yBot);
    g.DrawLine(&arrPen, ax + head, yBot - head * 0.8f, ax, yBot);
    g.DrawLine(&arrPen, ax + head, yBot + head * 0.8f, ax, yBot);
}

void DrawItemIcon(Graphics& g, const Item& it, float cx, float cy, float sz, bool deviceIcon) {
    const Theme& th = g_theme;
    if (it.kind == 2 && it.icon) {
        ImageAttributes ia;
        const float alpha = it.mute ? 0.35f : 1.0f;
        ColorMatrix cm = {{
            {1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, alpha, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
        }};
        ia.SetColorMatrix(&cm);
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        g.DrawImage(it.icon, RectF(cx - sz / 2, cy - sz / 2, sz, sz),
                    0.0f, 0.0f, (REAL)it.icon->GetWidth(), (REAL)it.icon->GetHeight(),
                    UnitPixel, &ia);
    } else if (deviceIcon && it.kind == 1) {
        // Without device headers the master rows show which device they belong to.
        const DeviceType dt = ClassifyDevice(DeviceNameOf(it.deviceId));
        DrawDeviceVectorIcon(g, dt, cx - sz * 0.45f, cy - sz * 0.45f, sz * 0.9f, it.mute ? th.muted : th.text);
        if (it.mute) {
            Pen x(Color(230, 240, 80, 80), std::max(1.5f, sz * 0.09f));
            g.DrawLine(&x, cx - sz * 0.4f, cy + sz * 0.4f, cx + sz * 0.4f, cy - sz * 0.4f);
        }
    } else {
        DrawSpeaker(g, cx, cy, sz * 0.95f, th.text, it.mute, it.vol);
    }
}

void DrawContent(Graphics& g) {
    const Theme& th = g_theme;
    const int W = g_winW;
    const std::wstring& layout = g_layoutMode;
    const LayoutMetrics lm = GetLayoutMetrics(layout);
    const int pad = S(lm.pad);

    const bool isCards = (layout == L"modern_cards");
    const bool isGlass = (layout == L"glass_tiles");
    const bool isNeumorphic = (layout == L"neumorphic");
    const bool isPill = (layout == L"minimal_pill");
    const bool isCircular = (layout == L"circular_hud");
    const bool isTerminal = (layout == L"terminal_retro");
    const bool isArcade = (layout == L"arcade_panel");
    const bool isCyber = (layout == L"cyberpunk_neon");
    const bool isControlCenter = (layout == L"control_center");
    const bool dark = IsDarkColor(th.bg);
    const Color ink = th.text;
    const bool deviceIcons = !lm.headers || lm.columns;

    UpdateGdiCache(g_scale);
    Font* fontName = isTerminal ? g_gc.fontMono : g_gc.fontName;
    Font* fontHeader = isTerminal ? g_gc.fontMono : g_gc.fontHeader;
    Font* fontValue = isTerminal ? g_gc.fontMono : g_gc.fontValue;

    SolidBrush textBrush(th.text);
    SolidBrush mutedBrush(th.muted);

    StringFormat left;
    left.SetAlignment(StringAlignmentNear);
    left.SetLineAlignment(StringAlignmentCenter);
    left.SetTrimming(StringTrimmingEllipsisCharacter);
    left.SetFormatFlags(StringFormatFlagsNoWrap);

    StringFormat right;
    right.SetAlignment(StringAlignmentFar);
    right.SetLineAlignment(StringAlignmentCenter);
    right.SetFormatFlags(StringFormatFlagsNoWrap);

    StringFormat center;
    center.SetAlignment(StringAlignmentCenter);
    center.SetLineAlignment(StringAlignmentCenter);
    center.SetFormatFlags(StringFormatFlagsNoWrap);

    int deviceCount = 0;
    for (const Item& it : g_items) {
        if (it.kind == 0) deviceCount++;
    }

    for (int x : g_colSeparators) {
        Pen sep(WithAlpha(ink, 40), 1.0f);
        g.DrawLine(&sep, (REAL)x, (REAL)S(14), (REAL)x, (REAL)(g_winH - S(14)));
    }

    for (size_t i = 0; i < g_items.size(); i++) {
        const Item& it = g_items[i];
        if (it.rowTop < 0) continue;

        // =====================================================================
        // Device Header Row (Kind 0)
        // =====================================================================
        if (it.kind == 0) {
            const float h = (float)(it.rowBottom - it.rowTop);
            if (!it.firstDevice) {
                Pen sep(WithAlpha(ink, 36), 1.0f);
                const float y = (float)(it.rowTop - S(5));
                g.DrawLine(&sep, (REAL)pad, y, (REAL)(W - pad), y);
            }

            const DeviceType dt = ClassifyDevice(it.name);
            const float iconSz = (float)S(16.0f);
            const float iconX = (REAL)pad;
            const float iconY = (REAL)it.rowTop + (h - iconSz) / 2.0f;
            DrawDeviceVectorIcon(g, dt, iconX, iconY, iconSz, th.text);

            float textX = iconX + iconSz + S(8);
            float textW = (float)(W - textX - pad);
            if (it.isDefaultDevice && deviceCount > 1) {
                // Accent dot marks the Windows default device
                const float d = (float)S(7.0f);
                SolidBrush dot(th.trackFill);
                g.FillEllipse(&dot, (REAL)(W - pad) - d, (REAL)it.rowTop + (h - d) / 2.0f, d, d);
                textW -= d + S(6);
            }
            const std::wstring title = isTerminal ? L"> " + it.name : it.name;
            g.DrawString(title.c_str(), -1, fontHeader, RectF(textX, (REAL)it.rowTop, textW, h), &left, &textBrush);
            continue;
        }

        // =====================================================================
        // Row / tile background
        // =====================================================================
        const float rowL = it.column ? (float)(it.colL + S(3)) : (float)(pad - S(4));
        const float rowW = it.column ? (float)(it.colR - it.colL - S(6)) : (float)(W - (pad - S(4)) * 2);
        const float rowT = (float)(it.rowTop + S(2));
        const float rowH = (float)(it.rowBottom - it.rowTop - S(4));
        if (isCards || isGlass || isNeumorphic || isPill || isArcade || it.column) {
            const float r = isPill ? rowH / 2.0f : (float)S(isGlass ? 10.0f : 8.0f);
            if (isNeumorphic) {
                FillRoundRect(g, Color(dark ? 90 : 45, 0, 0, 0), rowL + S(2.0f), rowT + S(3.0f), rowW, rowH, r);
                FillRoundRect(g, Color(dark ? 18 : 190, 255, 255, 255), rowL - S(1.5f), rowT - S(1.5f), rowW, rowH, r);
                FillRoundRect(g, WithAlpha(th.bg, 255), rowL, rowT, rowW, rowH, r);
            } else {
                const Color cardBg = isGlass ? WithAlpha(ink, 26) : isArcade ? Color(40, 255, 230, 0) : WithAlpha(ink, 14);
                const Color border = isGlass ? WithAlpha(ink, 60) : isArcade ? Color(160, 255, 230, 0) : WithAlpha(ink, 24);
                FillRoundRect(g, cardBg, rowL, rowT, rowW, rowH, r);
                StrokeRoundRect(g, border, 1.0f, rowL, rowT, rowW, rowH, r);
                if (isGlass) {
                    Pen hl(Color(70, 255, 255, 255), 1.0f);
                    g.DrawLine(&hl, rowL + r, rowT + 1.0f, rowL + rowW - r, rowT + 1.0f);
                }
            }
        } else if (isCyber) {
            Pen glow(Color(80, 0, 240, 255), 1.0f);
            g.DrawLine(&glow, (REAL)pad, (REAL)(it.rowBottom - 1), (REAL)(W - pad), (REAL)(it.rowBottom - 1));
        }
        if ((int)i == g_hoverIdx && !g_dragging) {
            FillRoundRect(g, th.rowHover, rowL, rowT, rowW, rowH, isPill ? rowH / 2.0f : (float)S(8.0f));
        }

        // =====================================================================
        // Icon / radial ring and slider
        // =====================================================================
        const float icx = (it.iconRc.left + it.iconRc.right) / 2.0f;
        const float icy = (it.iconRc.top + it.iconRc.bottom) / 2.0f;
        const float isz = (float)(it.iconRc.right - it.iconRc.left);

        if (isControlCenter) {
            DrawSlider(g, it);
            DrawItemIcon(g, it, icx, icy, isz, deviceIcons);
        } else if (isCircular) {
            const float r = isz / 2.0f - S(3.5f);
            const float ringW = (float)S(3.2f);
            Pen ringBg(th.trackBg, ringW);
            ringBg.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
            g.DrawArc(&ringBg, icx - r, icy - r, r * 2.0f, r * 2.0f, 135.0f, 270.0f);
            if (!it.mute && it.vol > 0.005f) {
                Pen ringFill(it.locked ? kLockColor : th.trackFill, ringW);
                ringFill.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
                const float sweep = 270.0f * std::clamp(it.vol, 0.0f, 1.0f);
                g.DrawArc(&ringFill, icx - r, icy - r, r * 2.0f, r * 2.0f, 135.0f, sweep);
                const float tip = (135.0f + sweep) * 3.14159265f / 180.0f;
                SolidBrush pipBr(ContrastOn(th.bg));
                const float pr = (float)S(2.5f);
                g.FillEllipse(&pipBr, icx + r * cosf(tip) - pr, icy + r * sinf(tip) - pr, pr * 2.0f, pr * 2.0f);
            }
            if (g_cfg.vuMeters && it.peak > 0.02f && !it.mute) {
                Pen vu(WithAlpha(th.peak, 160), (float)S(1.5f));
                const float vr = r - S(5.0f);
                g.DrawArc(&vu, icx - vr, icy - vr, vr * 2.0f, vr * 2.0f, 135.0f, 270.0f * std::clamp(it.peak, 0.0f, 1.0f));
            }
            DrawItemIcon(g, it, icx, icy, std::max((float)S(12.0f), isz * 0.46f), deviceIcons);
            DrawSlider(g, it);
        } else {
            DrawItemIcon(g, it, icx, icy, isz, deviceIcons);
            DrawSlider(g, it);
        }

        if (it.locked) {
            const float b = (float)S(9.0f);
            DrawLockBadge(g, (float)it.iconRc.right - b * 0.25f, (float)it.iconRc.top + b * 0.25f, b, th.bg);
        }

        // =====================================================================
        // Volume percentage
        // =====================================================================
        WCHAR val[16];
        swprintf_s(val, L"%d", (int)std::lround(it.vol * 100));
        SolidBrush* valBrush = it.mute ? &mutedBrush : &textBrush;
        if (it.column) {
            g.DrawString(val, -1, fontValue,
                         RectF((REAL)it.colL, (REAL)(it.rowTop + S(6)), (REAL)(it.colR - it.colL), (REAL)S(20)),
                         &center, valBrush);
        } else if (isControlCenter) {
            const RECT& p = it.pillRc;
            g.DrawString(val, -1, fontValue,
                         RectF((REAL)(p.right - S(56)), (REAL)p.top, (REAL)S(46), (REAL)(p.bottom - p.top)),
                         &right, valBrush);
        } else {
            const float vx = (float)(it.trackR + S(4));
            float vw = (float)(it.routeBtnRc.left - it.trackR - S(6));
            if (vw <= 0) vw = (float)S(28);
            g.DrawString(val, -1, fontValue, RectF(vx, (REAL)(it.trackCy - S(10)), vw, (REAL)S(20)), &right, valBrush);
        }

        // =====================================================================
        // Application name
        // =====================================================================
        if (it.kind == 2 && it.nameRc.right > it.nameRc.left) {
            g.DrawString(it.name.c_str(), -1, fontName,
                         RectF((REAL)it.nameRc.left, (REAL)it.nameRc.top,
                               (REAL)(it.nameRc.right - it.nameRc.left), (REAL)(it.nameRc.bottom - it.nameRc.top)),
                         &left, valBrush);
        }

        if (it.routeBtnRc.right > it.routeBtnRc.left) {
            DrawRouteButton(g, it.routeBtnRc, ink);
        }
    }

    // =========================================================================
    // Tray flyout expand / collapse button
    // =========================================================================
    if (!IsRectEmpty(&g_trayExpandBtnRc)) {
        const float bx = (float)g_trayExpandBtnRc.left;
        const float by = (float)g_trayExpandBtnRc.top;
        const float bw = (float)(g_trayExpandBtnRc.right - g_trayExpandBtnRc.left);
        const float bh = (float)(g_trayExpandBtnRc.bottom - g_trayExpandBtnRc.top);
        FillRoundRect(g, WithAlpha(ink, 20), bx, by, bw, bh, (float)S(6.0f));
        StrokeRoundRect(g, WithAlpha(ink, 40), 1.0f, bx, by, bw, bh, (float)S(6.0f));

        const float cs = (float)S(4.0f);
        const float cx = bx + S(16.0f);
        const float cy = by + bh / 2.0f;
        Pen chev(ink, std::max(1.3f, (float)S(1.4f)));
        chev.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
        const float dir = g_trayExpanded ? -1.0f : 1.0f;
        g.DrawLine(&chev, cx - cs, cy - dir * cs / 2.0f, cx, cy + dir * cs / 2.0f);
        g.DrawLine(&chev, cx, cy + dir * cs / 2.0f, cx + cs, cy - dir * cs / 2.0f);

        const WCHAR* label = L10n(g_trayExpanded ? StrId::HideApps : StrId::ShowApps);
        g.DrawString(label, -1, g_gc.fontRoute, RectF(bx + S(28), by, bw - S(36), bh), &left, &textBrush);
    }
}

void Render() {
    if (!g_hwnd || !g_mainSurface.Ensure(g_winW, g_winH)) return;
    const int w = g_mainSurface.w;
    const int h = g_mainSurface.h;

    Bitmap bmp(w, h, w * 4, PixelFormat32bppPARGB, (BYTE*)g_mainSurface.bits);
    Graphics g(&bmp);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    // ClearType needs an opaque background; on a layered window it leaves fringes.
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    g.Clear(Color(0, 0, 0, 0));

    const Theme& th = g_theme;
    const float radius = (float)S(th.radius);
    GraphicsPath path;
    RoundRectPath(path, 0.5f, 0.5f, (float)w - 1.0f, (float)h - 1.0f, radius);
    SolidBrush bgBrush(th.bg);
    g.FillPath(&bgBrush, &path);

    if (g_layoutMode == L"terminal_retro") {
        // CRT scanlines
        Pen scan(Color(18, 0, 0, 0), 1.0f);
        const int step = std::max(2, S(3.0f));
        for (int y = step; y < h - 1; y += step) g.DrawLine(&scan, 2.0f, (REAL)y, (REAL)(w - 2), (REAL)y);
    }

    Pen borderPen(th.border, 1.0f);
    g.DrawPath(&borderPen, &path);

    DrawContent(g);

    float alpha = 1.0f;
    if (g_cfg.animate) {
        const DWORD elapsed = GetTickCount() - g_animStart;
        const float progress = std::clamp((float)elapsed / (float)kAnimMs, 0.0f, 1.0f);
        alpha = g_hiding ? (1.0f - progress) : progress;
    }

    const int userOpacity = std::clamp(g_cfg.opacity, 20, 100);
    const BYTE finalA = (BYTE)std::clamp((int)std::lround(255.0f * (userOpacity / 100.0f) * alpha), 0, 255);

    POINT ptSrc{0, 0};
    SIZE sz{w, h};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, finalA, AC_SRC_ALPHA};
    POINT ptDst{g_pos.x, g_pos.y};
    UpdateLayeredWindow(g_hwnd, nullptr, &ptDst, &sz, g_mainSurface.dc, &ptSrc, 0, &bf, ULW_ALPHA);
}

////////////////////////////////////////////////////////////////////////////////
// Monitor selection, scaling and window placement

RECT MonitorWorkArea(HMONITOR mon) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (mon && GetMonitorInfoW(mon, &mi)) return mi.rcWork;
    RECT rc{0, 0, 1920, 1080};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &rc, 0);
    return rc;
}

static int ClampSpan(int v, int lo, int hi) {
    if (hi < lo) hi = lo;
    return std::clamp(v, lo, hi);
}

void ClampToWorkArea(const RECT& wa) {
    g_pos.x = ClampSpan(g_pos.x, wa.left + 4, wa.right - g_winW - 4);
    g_pos.y = ClampSpan(g_pos.y, wa.top + 4, wa.bottom - g_winH - 4);
}

HMONITOR TargetMonitor() {
    if (g_cfg.rememberCustomPos && g_hasCustomPos && !g_openedFromTray) {
        HMONITOR m = MonitorFromPoint(POINT{g_customPosX + 8, g_customPosY + 8}, MONITOR_DEFAULTTONULL);
        if (m) return m;
    }
    RECT tray{};
    if ((g_openedFromTray || g_cfg.anchor == L"tray") && GetTrayIconRect(&tray)) {
        return MonitorFromRect(&tray, MONITOR_DEFAULTTONEAREST);
    }
    if (g_cfg.monitor == L"primary") return MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (g_cfg.monitor == L"active") {
        HWND fg = GetForegroundWindow();
        if (fg) return MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    }
    POINT c{0, 0};
    GetCursorPos(&c);
    return MonitorFromPoint(c, MONITOR_DEFAULTTONEAREST);
}

// Scale = user scale x monitor DPI; shrinks automatically when the popup would
// not fit into the work area of the monitor.
void ComputeScaleAndLayout(HMONITOR mon) {
    float s = std::clamp(g_cfg.scale / 100.0f, 0.3f, 3.0f);
    if (g_cfg.followDpi) s *= GetMonitorDpiScale(mon);
    const RECT wa = MonitorWorkArea(mon);
    const int availW = std::max(100, (int)(wa.right - wa.left) - 16);
    const int availH = std::max(100, (int)(wa.bottom - wa.top) - 16);
    g_scale = s;
    Layout();
    for (int pass = 0; pass < 4 && (g_winW > availW || g_winH > availH) && g_scale > 0.31f; pass++) {
        const float f = std::min((float)availW / g_winW, (float)availH / g_winH);
        g_scale = std::max(0.3f, g_scale * f * 0.98f);
        Layout();
    }
    UpdateGdiCache(g_scale);
}

void PositionWindow(HMONITOR mon) {
    const RECT wa = MonitorWorkArea(mon);

    if (g_cfg.rememberCustomPos && g_hasCustomPos && !g_openedFromTray &&
        MonitorFromPoint(POINT{g_customPosX + 8, g_customPosY + 8}, MONITOR_DEFAULTTONULL)) {
        g_pos.x = g_customPosX;
        g_pos.y = g_customPosY;
        ClampToWorkArea(wa);
        return;
    }

    RECT tray{};
    if ((g_openedFromTray || g_cfg.anchor == L"tray") && GetTrayIconRect(&tray)) {
        const int gap = S(10);
        const int cx = (tray.left + tray.right) / 2;
        const int cy = (tray.top + tray.bottom) / 2;
        if (tray.left >= wa.right) {          // taskbar on the right
            g_pos.x = tray.left - g_winW - gap;
            g_pos.y = cy - g_winH / 2;
        } else if (tray.right <= wa.left) {   // taskbar on the left
            g_pos.x = tray.right + gap;
            g_pos.y = cy - g_winH / 2;
        } else if (tray.bottom <= wa.top) {   // taskbar on top
            g_pos.x = cx - g_winW / 2;
            g_pos.y = tray.bottom + gap;
        } else {                              // bottom taskbar or overflow flyout
            g_pos.x = cx - g_winW / 2;
            g_pos.y = tray.top - g_winH - gap;
        }
        ClampToWorkArea(wa);
        return;
    }

    const int offX = S((float)g_cfg.offsetX);
    const int offY = S((float)g_cfg.offsetY);
    const int midX = wa.left + (wa.right - wa.left - g_winW) / 2;
    const int midY = wa.top + (wa.bottom - wa.top - g_winH) / 2;
    const std::wstring& a = g_cfg.anchor;
    if (a == L"top_left") {
        g_pos = {wa.left + offX, wa.top + offY};
    } else if (a == L"top_right") {
        g_pos = {wa.right - g_winW - offX, wa.top + offY};
    } else if (a == L"top_center") {
        g_pos = {midX, wa.top + offY};
    } else if (a == L"bottom_left") {
        g_pos = {wa.left + offX, wa.bottom - g_winH - offY};
    } else if (a == L"bottom_center") {
        g_pos = {midX, wa.bottom - g_winH - offY};
    } else if (a == L"center") {
        g_pos = {midX, midY};
    } else {  // bottom_right, or "tray" while the tray icon is hidden
        g_pos = {wa.right - g_winW - offX, wa.bottom - g_winH - offY};
    }
    ClampToWorkArea(wa);
}

// Re-layout of the open popup (new apps, expanded flyout). The edge that is
// closest to the screen edge stays put, so the popup grows away from the tray.
void ApplyLayoutChange() {
    const int oldW = g_winW, oldH = g_winH;
    if (!g_popupMonitor) g_popupMonitor = TargetMonitor();
    ComputeScaleAndLayout(g_popupMonitor);
    if (g_visible && (g_winW != oldW || g_winH != oldH)) {
        const RECT wa = MonitorWorkArea(g_popupMonitor);
        if (g_pos.y + oldH / 2 > (wa.top + wa.bottom) / 2) g_pos.y += oldH - g_winH;
        if (g_pos.x + oldW / 2 > (wa.left + wa.right) / 2) g_pos.x += oldW - g_winW;
        ClampToWorkArea(wa);
    }
    g_sig = SigOf(g_items);
    Render();
}

// Rebuilds the session list. With onlyIfChanged the new list is used only when
// apps or devices appeared/disappeared; returns true when it was applied.
bool RefreshModel(bool onlyIfChanged) {
    std::vector<Item> next;
    BuildItems(next, g_cfg.showAllDevices);
    if (onlyIfChanged && SigOf(next) == g_sig) {
        ReleaseItems(next);
        return false;
    }
    for (Item& n : next) {
        for (const Item& o : g_items) {
            if (o.kind == n.kind && o.key == n.key && o.deviceId == n.deviceId) {
                n.peak = o.peak;
                break;
            }
        }
    }
    ReleaseItems(g_items);
    g_items = std::move(next);
    if (g_hoverIdx >= (int)g_items.size()) g_hoverIdx = -1;
    ApplyLayoutChange();
    return true;
}

void FinishHide() {
    KillTimer(g_hwnd, ID_TIMER_ANIM);
    KillTimer(g_hwnd, ID_TIMER_FAST);
    HideCustomMenu();
    if (g_dragging || g_movingWindow) {
        g_dragging = false;
        g_movingWindow = false;
        g_dragIdx = -1;
        ReleaseCapture();
    }
    g_visible = false;
    g_hiding = false;
    ShowWindow(g_hwnd, SW_HIDE);
    ReleaseItems(g_items);
    g_items.clear();
    g_sig.clear();
    g_activeTrayPopupStyle.clear();
    g_openedFromTray = false;
    g_hoverIdx = -1;
    SyncPopupHookState();
}

void HidePopup() {
    HideCustomMenu();
    if (!g_visible || g_hiding) return;
    if (g_dragging || g_movingWindow) {
        g_dragging = false;
        g_movingWindow = false;
        g_dragIdx = -1;
        ReleaseCapture();
    }
    if (g_cfg.animate) {
        g_hiding = true;
        g_animStart = GetTickCount();
        SetTimer(g_hwnd, ID_TIMER_ANIM, 16, nullptr);
        SyncPopupHookState();
    } else {
        FinishHide();
    }
}

// The popup never takes the focus: activating it would minimize exclusive
// fullscreen games. Clicks outside and key presses are seen by the hooks.
void ShowPopup(bool fromTray) {
    if (g_visible) FinishHide();

    g_openedFromTray = fromTray;
    g_activeTrayPopupStyle = fromTray ? g_cfg.trayPopupStyle : L"";
    if (g_activeTrayPopupStyle == L"full_mixer") g_activeTrayPopupStyle.clear();
    g_theme = MakeTheme(g_cfg.theme);
    g_hoverIdx = -1;

    std::vector<Item> next;
    BuildItems(next, g_cfg.showAllDevices);
    ReleaseItems(g_items);
    g_items = std::move(next);

    g_popupMonitor = TargetMonitor();
    ComputeScaleAndLayout(g_popupMonitor);
    PositionWindow(g_popupMonitor);
    g_sig = SigOf(g_items);

    g_visible = true;
    g_hiding = false;
    g_animStart = GetTickCount();
    g_showTick = g_animStart;
    g_lastActive = g_animStart;
    g_lastValuesPoll = g_animStart;
    g_lastModelPoll = g_animStart;

    Render();
    SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

    SetTimer(g_hwnd, ID_TIMER_FAST, g_cfg.vuMeters ? 16 : 50, nullptr);
    if (g_cfg.animate) {
        SetTimer(g_hwnd, ID_TIMER_ANIM, 16, nullptr);
    }
    SyncPopupHookState();
}

// zone: 0 = row, 1 = icon (mute), 2 = slider, 3 = route button, 4 = expand button
int HitTest(int x, int y, int* zone) {
    *zone = 0;
    POINT pt{x, y};
    if (!IsRectEmpty(&g_trayExpandBtnRc) && PtInRect(&g_trayExpandBtnRc, pt)) {
        *zone = 4;
        return -2;
    }
    for (int i = 0; i < (int)g_items.size(); i++) {
        const Item& it = g_items[i];
        if (it.rowTop < 0 || y < it.rowTop || y >= it.rowBottom) continue;
        if (it.column && (x < it.colL || x >= it.colR)) continue;
        if (it.kind != 0) {
            if (PtInRect(&it.iconRc, pt)) {
                *zone = 1;
                return i;
            }
            if (PtInRect(&it.routeBtnRc, pt)) {
                *zone = 3;
                return i;
            }
            if (it.verticalSlider) {
                const int cx = (it.trackL + it.trackR) / 2;
                if (std::abs(x - cx) <= S(16) && y >= it.trackTop - S(8) && y <= it.trackBottom + S(8)) {
                    *zone = 2;
                    return i;
                }
            } else if (x >= it.trackL - S(6) && x <= it.trackR + S(6)) {
                *zone = 2;
                return i;
            }
        }
        return i;
    }
    return -1;
}

void DragTo(int x, int y) {
    if (g_dragIdx < 0 || g_dragIdx >= (int)g_items.size()) return;
    Item& it = g_items[g_dragIdx];
    float v = 0.0f;
    if (it.verticalSlider) {
        const float span = (float)(it.trackBottom - it.trackTop);
        if (span <= 0.0f) return;
        v = (it.trackBottom - y) / span;
    } else {
        const float span = (float)(it.trackR - it.trackL);
        if (span <= 0.0f) return;
        v = (x - it.trackL) / span;
    }
    SetItemVolume(it, v);
    if (it.mute && it.vol > 0.0f) SetItemMute(it, false);
    Render();
}

// In-Game OSD and Custom Routing Context Menu

static HMONITOR OsdTargetMonitor() {
    if (g_cfg.osdMonitor == L"primary") return MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (g_cfg.osdMonitor == L"active") {
        HWND fg = GetForegroundWindow();
        if (fg) return MonitorFromWindow(fg, MONITOR_DEFAULTTOPRIMARY);
    }
    POINT c{0, 0};
    GetCursorPos(&c);
    return MonitorFromPoint(c, MONITOR_DEFAULTTOPRIMARY);
}

void UpdateHooks();

// Click-through unless the user wants to drag the OSD. The wheel over it is
// taken by the mouse hook, so it never swallows clicks.
void ApplyOsdWindowStyle() {
    if (!g_hwndOsd) return;
    const LONG_PTR ex = GetWindowLongPtrW(g_hwndOsd, GWL_EXSTYLE);
    const LONG_PTR want = g_cfg.osdAllowDrag ? (ex & ~(LONG_PTR)WS_EX_TRANSPARENT) : (ex | WS_EX_TRANSPARENT);
    if (want != ex) SetWindowLongPtrW(g_hwndOsd, GWL_EXSTYLE, want);
}

// Like the Windows flyout, the OSD stays while the pointer rests on it and the
// mouse wheel over it changes the volume. The pointer has to move after the OSD
// appears, so one that already rested there doesn't hold it, and a hidden
// pointer (games) never counts.
void UpdateOsdHover() {
    CURSORINFO ci{sizeof(ci)};
    RECT r{};
    const bool visible = IsWindowVisible(g_hwndOsd) && GetCursorInfo(&ci) && (ci.flags & CURSOR_SHOWING);
    if (visible && !g_osdPointerMoved) {
        g_osdPointerMoved = ci.ptScreenPos.x != g_osdShowPointer.x || ci.ptScreenPos.y != g_osdShowPointer.y;
    }
    const bool hover = visible && g_osdPointerMoved && GetWindowRect(g_hwndOsd, &r) && PtInRect(&r, ci.ptScreenPos);
    if (hover) {
        AcquireSRWLockExclusive(&g_hookLock);
        g_hookOsdRect = r;
        ReleaseSRWLockExclusive(&g_hookLock);
    }
    if (hover == g_hookOsdHover) return;
    g_hookOsdHover = hover;
    UpdateHooks();
    if (!hover) {
        const DWORD linger = GetTickCount() + 1000;
        if ((LONG)(linger - g_osdHideTick) > 0) g_osdHideTick = linger;
    }
}

void HideOsd() {
    KillTimer(g_hwndOsd, ID_TIMER_OSD);
    ShowWindow(g_hwndOsd, SW_HIDE);
    if (g_hookOsdHover) {
        g_hookOsdHover = false;
        UpdateHooks();
    }
}

void ShowOsd(const std::wstring& name, float vol, bool mute, Bitmap* icon, OsdKind kind,
             const std::wstring& detail, DWORD durationMs) {
    if (!g_hwndOsd || g_osdDragging) return;
    if (kind == OsdKind::Volume && !g_cfg.enableVolumeOsd) return;
    if (kind == OsdKind::Banner) g_osdTarget = OsdTarget::None;
    if (!IsWindowVisible(g_hwndOsd)) {
        GetCursorPos(&g_osdShowPointer);
        g_osdPointerMoved = false;
    }
    vol = std::clamp(vol, 0.0f, 1.0f);

    HMONITOR mon = nullptr;
    bool useCustom = false;
    if (g_cfg.osdRememberPos && g_hasCustomOsdPos) {
        mon = MonitorFromPoint(POINT{g_customOsdX + 4, g_customOsdY + 4}, MONITOR_DEFAULTTONULL);
        useCustom = (mon != nullptr);
    }
    if (!mon) mon = OsdTargetMonitor();

    float s = std::clamp(g_cfg.osdScale / 100.0f, 0.3f, 3.0f);
    if (g_cfg.followDpi) s *= GetMonitorDpiScale(mon);
    auto P = [s](float v) { return v * s; };
    auto PI = [s](float v) { return (int)std::lround(v * s); };

    UpdateGdiCache(g_gcOsd, s);
    const std::wstring style = (kind == OsdKind::Banner) ? std::wstring(L"banner") : g_cfg.osdStyle;
    const bool hud = (style == L"game_hud_compact");
    const bool glass = (style == L"glass_morphism");
    const bool smallPill = (style == L"compact_pill" || hud);
    Font* fName = hud ? g_gcOsd.fontMono : g_gcOsd.fontName;
    Font* fVal = hud ? g_gcOsd.fontMono : g_gcOsd.fontValue;

    // Short value for the narrow styles, "Muted" text for the wide ones.
    const bool shortValue = (style == L"ring" || style == L"vertical_bar" || style == L"stealth_dot" || smallPill);
    WCHAR valStr[64];
    if (mute && !shortValue) swprintf_s(valStr, L"%ls", L10n(StrId::Muted));
    else if (hud) swprintf_s(valStr, L"%03d", mute ? 0 : (int)std::lround(vol * 100));
    else swprintf_s(valStr, L"%d", mute ? 0 : (int)std::lround(vol * 100));

    int osdW = PI((float)g_cfg.osdWidth);
    int osdH = PI(46);
    if (style == L"banner") {
        Bitmap probe(1, 1, PixelFormat32bppARGB);
        Graphics mg(&probe);
        RectF bound;
        mg.MeasureString(name.c_str(), -1, fName, PointF(0, 0), &bound);
        osdW = std::clamp((int)std::ceil(bound.Width) + PI(icon ? 66 : 44), PI(160), PI(620));
        osdH = PI(40);
    } else if (style == L"detailed") {
        osdW = std::max(osdW, PI(280));
        osdH = PI(detail.empty() ? 64 : 76);
    } else if (style == L"compact_pill") {
        osdW = std::min(osdW, PI(210));
        osdH = PI(36);
    } else if (style == L"ring") {
        osdW = PI(84);
        osdH = PI(84);
    } else if (hud) {
        osdW = std::min(std::max(osdW, PI(200)), PI(260));
        osdH = PI(32);
    } else if (style == L"stealth_dot") {
        osdW = PI(130);
        osdH = PI(24);
    } else if (style == L"vertical_bar") {
        osdW = PI(40);
        osdH = PI(170);
    } else if (style == L"pill_slider") {
        osdW = std::clamp(osdW, PI(220), PI(360));
        osdH = PI(44);
    }

    if (!g_osdSurface.Ensure(osdW, osdH)) return;
    Bitmap bmp(osdW, osdH, osdW * 4, PixelFormat32bppPARGB, (BYTE*)g_osdSurface.bits);
    Graphics g(&bmp);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.Clear(Color(0, 0, 0, 0));

    const std::wstring thName = (g_cfg.osdTheme == L"inherit" || g_cfg.osdTheme.empty()) ? g_cfg.theme : g_cfg.osdTheme;
    const Theme th = MakeTheme(thName);

    const int userOp = std::clamp(g_cfg.osdOpacity, 20, 100);
    const BYTE op = (BYTE)std::clamp((int)std::lround(userOp * 2.55f), 30, 255);
    const Color bgCol = glass ? WithAlpha(th.bg, std::min<int>(op, 165)) : WithAlpha(th.bg, op);
    const Color borderCol = glass ? Color(130, 255, 255, 255) : hud ? th.trackFill : th.border;
    const float borderW = hud ? 1.5f : 1.0f;
    SolidBrush tBr(th.text);
    SolidBrush mBr(th.muted);

    StringFormat sfR;
    sfR.SetAlignment(StringAlignmentFar);
    sfR.SetLineAlignment(StringAlignmentCenter);
    sfR.SetFormatFlags(StringFormatFlagsNoWrap);
    StringFormat sfC;
    sfC.SetAlignment(StringAlignmentCenter);
    sfC.SetLineAlignment(StringAlignmentCenter);
    sfC.SetFormatFlags(StringFormatFlagsNoWrap);
    StringFormat sfL;
    sfL.SetAlignment(StringAlignmentNear);
    sfL.SetLineAlignment(StringAlignmentCenter);
    sfL.SetTrimming(StringTrimmingEllipsisCharacter);
    sfL.SetFormatFlags(StringFormatFlagsNoWrap);

    const float fW = (float)osdW;
    const float fH = (float)osdH;
    auto drawIcon = [&](float x, float y, float sz) {
        if (icon) g.DrawImage(icon, RectF(x, y, sz, sz));
        else DrawSpeaker(g, x + sz / 2.0f, y + sz / 2.0f, sz, th.text, mute, vol);
    };
    auto drawBar = [&](float x, float y, float w, float h) {
        FillRoundRect(g, th.trackBg, x, y, w, h, h / 2.0f);
        if (!mute && vol > 0.001f) FillRoundRect(g, th.trackFill, x, y, std::max(h, w * vol), h, h / 2.0f);
    };
    auto background = [&](float r) {
        FillRoundRect(g, bgCol, 1.0f, 1.0f, fW - 2.0f, fH - 2.0f, r);
        StrokeRoundRect(g, borderCol, borderW, 1.0f, 1.0f, fW - 2.0f, fH - 2.0f, r);
    };

    if (style == L"banner") {
        background((fH - 2.0f) / 2.0f);
        float x = P(16);
        if (icon) {
            drawIcon(x, (fH - P(20)) / 2.0f, P(20));
            x += P(28);
        }
        g.DrawString(name.c_str(), -1, fName, RectF(x, 0, fW - x - P(14), fH), &sfL, &tBr);
    } else if (style == L"stealth_dot") {
        background(fH / 2.0f);
        const int dotCount = 8;
        const float dotR = fH * 0.18f;
        const float startX = (fW - (dotCount * dotR * 2.8f)) / 2.0f;
        const int activeDots = (int)std::lround(vol * dotCount);
        for (int i = 0; i < dotCount; i++) {
            const float cx = startX + i * dotR * 2.8f + dotR;
            const Color dotCol = mute ? Color(180, 240, 80, 80) :
                                 (i < activeDots ? th.trackFill : WithAlpha(th.text, 60));
            SolidBrush dotBr(dotCol);
            g.FillEllipse(&dotBr, cx - dotR, fH / 2.0f - dotR, dotR * 2.0f, dotR * 2.0f);
        }
    } else if (style == L"ring") {
        SolidBrush bgBr(bgCol);
        g.FillEllipse(&bgBr, 2.0f, 2.0f, fW - 4.0f, fH - 4.0f);
        Pen bPen(borderCol, borderW);
        g.DrawEllipse(&bPen, 2.0f, 2.0f, fW - 4.0f, fH - 4.0f);

        const float arcPad = P(8);
        const float arcSz = fW - arcPad * 2.0f;
        Pen arcBgPen(th.trackBg, P(4.5f));
        arcBgPen.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
        g.DrawArc(&arcBgPen, arcPad, arcPad, arcSz, arcSz, -90.0f, 360.0f);
        if (!mute && vol > 0.001f) {
            Pen arcFillPen(th.trackFill, P(4.5f));
            arcFillPen.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
            g.DrawArc(&arcFillPen, arcPad, arcPad, arcSz, arcSz, -90.0f, 360.0f * vol);
        }
        const float icSz = P(22);
        drawIcon((fW - icSz) / 2.0f, fH * 0.20f, icSz);
        g.DrawString(valStr, -1, fVal, RectF(0, fH * 0.52f, fW, P(20)), &sfC, mute ? &mBr : &tBr);
    } else if (style == L"vertical_bar") {
        background(fW / 2.0f);
        const float pad = P(8);
        g.DrawString(valStr, -1, fVal, RectF(0, pad, fW, P(16)), &sfC, mute ? &mBr : &tBr);
        const float icSz = P(18);
        drawIcon((fW - icSz) / 2.0f, fH - pad - icSz, icSz);
        const float barW = P(8);
        const float barX = (fW - barW) / 2.0f;
        const float barY = pad + P(22);
        const float barH = fH - barY - pad - icSz - P(8);
        FillRoundRect(g, th.trackBg, barX, barY, barW, barH, barW / 2.0f);
        if (!mute && vol > 0.001f) {
            const float fillH = std::max(barW, barH * vol);
            FillRoundRect(g, th.trackFill, barX, barY + barH - fillH, barW, fillH, barW / 2.0f);
        }
    } else if (style == L"pill_slider") {
        // Control Center pill: the whole capsule is the level indicator
        const float r = (fH - 2.0f) / 2.0f;
        FillRoundRect(g, bgCol, 1.0f, 1.0f, fW - 2.0f, fH - 2.0f, r);
        if (!mute && vol > 0.001f) {
            FillRoundRect(g, WithAlpha(th.trackFill, op), 1.0f, 1.0f, std::max(fH - 2.0f, (fW - 2.0f) * vol), fH - 2.0f, r);
        }
        StrokeRoundRect(g, borderCol, borderW, 1.0f, 1.0f, fW - 2.0f, fH - 2.0f, r);
        const float icSz = P(20);
        drawIcon(P(14), (fH - icSz) / 2.0f, icSz);
        const float textX = P(14) + icSz + P(10);
        g.DrawString(name.c_str(), -1, fName, RectF(textX, 0, fW - textX - P(62), fH), &sfL, &tBr);
        g.DrawString(valStr, -1, fVal, RectF(fW - P(62), 0, P(48), fH), &sfR, mute ? &mBr : &tBr);
    } else {
        // capsule, compact_pill, detailed, glass_morphism, game_hud_compact
        const float rad = hud ? P(2) : (style == L"compact_pill") ? (fH - 2.0f) / 2.0f : P(12);
        background(rad);
        if (glass) {
            Pen hl(Color(90, 255, 255, 255), 1.0f);
            g.DrawLine(&hl, rad, 2.0f, fW - rad, 2.0f);
        }
        if (hud) {
            // Tactical corner brackets
            Pen br(th.trackFill, P(2));
            const float c = P(7);
            g.DrawLine(&br, 1.0f, 1.0f, c, 1.0f);
            g.DrawLine(&br, 1.0f, 1.0f, 1.0f, c);
            g.DrawLine(&br, fW - 1.0f, fH - 1.0f, fW - c, fH - 1.0f);
            g.DrawLine(&br, fW - 1.0f, fH - 1.0f, fW - 1.0f, fH - c);
        }

        const float icSz = (style == L"detailed" ? 26.0f : smallPill ? 16.0f : 20.0f) * s;
        const float icX = (smallPill ? 10.0f : 14.0f) * s;
        const float icY = (fH - icSz) / 2.0f;
        drawIcon(icX, icY, icSz);

        if (style == L"detailed") {
            const float textX = icX + icSz + P(12);
            const float textW = fW - textX - P(14);
            g.DrawString(name.c_str(), -1, fName, RectF(textX, P(8), textW - P(44), P(18)), &sfL, &tBr);
            g.DrawString(valStr, -1, g_gcOsd.fontBig, RectF(textX, P(4), textW, P(26)), &sfR, mute ? &mBr : &tBr);
            const float barY = P(34);
            drawBar(textX, barY, textW, P(6));
            if (g_cfg.vuMeters && !mute) {
                // Thin level line under the volume bar
                Pen tick(WithAlpha(th.peak, 160), P(2));
                g.DrawLine(&tick, textX, barY + P(10), textX + textW * vol, barY + P(10));
            }
            if (!detail.empty()) {
                g.DrawString(detail.c_str(), -1, fVal, RectF(textX, barY + P(16), textW, P(18)), &sfL, &mBr);
            }
        } else {
            const float valW = (smallPill ? 34.0f : 44.0f) * s;
            const float valX = fW - valW - (smallPill ? 10.0f : 14.0f) * s;
            g.DrawString(valStr, -1, fVal, RectF(valX, 0, valW, fH), &sfR, mute ? &mBr : &tBr);
            const float barX = icX + icSz + (smallPill ? 8.0f : 12.0f) * s;
            const float barW = valX - barX - (smallPill ? 6.0f : 10.0f) * s;
            const float barH = (smallPill ? 4.0f : 5.0f) * s;
            if (hud) {
                // Segmented ammo-style bar
                const int segs = 12;
                const float sg = P(2);
                const float sw = (barW - sg * (segs - 1)) / segs;
                const int lit = mute ? 0 : (int)std::lround(vol * segs);
                for (int i = 0; i < segs; i++) {
                    SolidBrush sb(i < lit ? th.trackFill : WithAlpha(th.text, 40));
                    g.FillRectangle(&sb, barX + i * (sw + sg), (fH - P(8)) / 2.0f, sw, P(8));
                }
            } else {
                drawBar(barX, (fH - barH) / 2.0f, barW, barH);
            }
        }
    }

    POINT ptDst{0, 0};
    const RECT wa = MonitorWorkArea(mon);
    if (useCustom) {
        ptDst.x = g_customOsdX;
        ptDst.y = g_customOsdY;
    } else {
        const int offX = PI((float)g_cfg.osdOffsetX);
        const int offY = PI((float)g_cfg.osdOffsetY);
        const int edge = PI(16);
        const int midX = wa.left + (wa.right - wa.left - osdW) / 2;
        const int midY = wa.top + (wa.bottom - wa.top - osdH) / 2;
        const std::wstring& p = g_cfg.osdPosition;
        if (p == L"bottom_center") ptDst = {midX + offX, wa.bottom - osdH - offY};
        else if (p == L"top_right") ptDst = {wa.right - osdW - edge - offX, wa.top + offY};
        else if (p == L"top_left") ptDst = {wa.left + edge + offX, wa.top + offY};
        else if (p == L"bottom_right") ptDst = {wa.right - osdW - edge - offX, wa.bottom - osdH - offY};
        else if (p == L"bottom_left") ptDst = {wa.left + edge + offX, wa.bottom - osdH - offY};
        else if (p == L"left_center") ptDst = {wa.left + edge + offX, midY};
        else if (p == L"right_center") ptDst = {wa.right - osdW - edge - offX, midY};
        else if (p == L"center") ptDst = {midX + offX, midY};
        else ptDst = {midX + offX, wa.top + offY};  // top_center
    }
    ptDst.x = ClampSpan(ptDst.x, wa.left, wa.right - osdW);
    ptDst.y = ClampSpan(ptDst.y, wa.top, wa.bottom - osdH);

    POINT ptSrc{0, 0};
    SIZE sz{osdW, osdH};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwndOsd, nullptr, &ptDst, &sz, g_osdSurface.dc, &ptSrc, 0, &bf, ULW_ALPHA);
    ApplyOsdWindowStyle();
    SetWindowPos(g_hwndOsd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

    DWORD timeout = durationMs ? durationMs : (DWORD)std::clamp(g_cfg.osdTimeoutMs, 300, 10000);
    if (kind == OsdKind::Banner) timeout = std::max<DWORD>(timeout, 1800);
    g_osdHideTick = GetTickCount() + timeout;
    SetTimer(g_hwndOsd, ID_TIMER_OSD, 50, nullptr);
    UpdateOsdHover();
}

void ShowOsdDragPreview() {
    g_osdTarget = OsdTarget::None;
    ShowOsd(L10n(StrId::OsdDragHint), 0.5f, false, nullptr, OsdKind::Always, L"", 10000);
}

LRESULT CALLBACK OsdWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;

    case WM_NCHITTEST:
        return g_cfg.osdAllowDrag ? HTCLIENT : HTTRANSPARENT;

    case WM_SETCURSOR:
        if (g_cfg.osdAllowDrag) {
            SetCursor(LoadCursorW(nullptr, IDC_SIZEALL));
            return TRUE;
        }
        break;

    case WM_MOUSEWHEEL:
        OnOsdWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        return 0;

    case WM_TIMER:
        if (wParam == ID_TIMER_OSD && !g_osdDragging) {
            UpdateOsdHover();
            if (!g_hookOsdHover && (LONG)(GetTickCount() - g_osdHideTick) >= 0) HideOsd();
        }
        return 0;

    case WM_LBUTTONDOWN:
        if (g_cfg.osdAllowDrag) {
            g_osdDragging = true;
            SetCapture(hwnd);
            GetCursorPos(&g_osdDragStart);
            RECT r{};
            GetWindowRect(hwnd, &r);
            g_osdWinStart.x = r.left;
            g_osdWinStart.y = r.top;
        }
        return 0;

    case WM_MOUSEMOVE:
        if (g_osdDragging) {
            POINT pt;
            GetCursorPos(&pt);
            g_customOsdX = g_osdWinStart.x + (pt.x - g_osdDragStart.x);
            g_customOsdY = g_osdWinStart.y + (pt.y - g_osdDragStart.y);
            SetWindowPos(hwnd, nullptr, g_customOsdX, g_customOsdY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
        return 0;

    case WM_LBUTTONUP:
        if (g_osdDragging) {
            g_osdDragging = false;
            ReleaseCapture();
            g_hasCustomOsdPos = true;
            Wh_SetIntValue(L"osdCustomX", g_customOsdX);
            Wh_SetIntValue(L"osdCustomY", g_customOsdY);
            Wh_SetIntValue(L"hasCustomOsdPos", 1);
            g_osdHideTick = GetTickCount() + 2500;
        }
        return 0;

    case WM_CAPTURECHANGED:
        g_osdDragging = false;
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Routing / volume-lock menu

void HideCustomMenu() {
    if (g_hwndMenu && IsWindowVisible(g_hwndMenu)) ShowWindow(g_hwndMenu, SW_HIDE);
    g_menuHover = -1;
    g_menuTracking = false;
}

static Item* FindMenuTarget() {
    for (Item& it : g_items) {
        if (it.kind == g_menuTarget.kind && it.key == g_menuTarget.key && it.deviceId == g_menuTarget.deviceId) return &it;
    }
    return nullptr;
}

void ExecuteMenuCommand(const CustomMenuItem& item) {
    const int id = item.id;
    const MenuTarget t = g_menuTarget;
    if (id == 999) {
        Item* it = FindMenuTarget();
        const bool lockedNow = it ? it->locked : g_lockedVolumes.count(t.key) > 0;
        SetVolumeLock(t.key, !lockedNow, it ? it->vol : 1.0f);
        g_sig = SigOf(g_items);
        Render();
        return;
    }
    if (id == 998) {
        for (DWORD pid : t.pids) SetAppAudioEndpoint(pid, L"");
    } else if (id >= 1000 && !item.deviceId.empty()) {
        const std::wstring& devId = item.deviceId;
        if (t.kind == 2) {
            for (DWORD pid : t.pids) SetAppAudioEndpoint(pid, devId);
        } else {
            SetDefaultAudioDevice(devId);
        }
    } else {
        return;
    }
    RefreshModel(false);
    // Streams move to the new device asynchronously; look again shortly.
    SetTimer(g_hwnd, ID_TIMER_REFRESH_ONCE, 600, nullptr);
}

void RenderCustomMenu() {
    if (!g_hwndMenu || g_menuW <= 0 || g_menuH <= 0 || !g_menuSurface.Ensure(g_menuW, g_menuH)) return;

    Bitmap bmp(g_menuW, g_menuH, g_menuW * 4, PixelFormat32bppPARGB, (BYTE*)g_menuSurface.bits);
    Graphics g(&bmp);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    g.Clear(Color(0, 0, 0, 0));

    const Theme& th = g_theme;
    const Color ink = th.text;
    const float rad = (float)S(std::max(6.0f, th.radius * 0.8f));
    FillRoundRect(g, WithAlpha(th.bg, std::max<int>(th.bg.GetA(), 240)), 0.5f, 0.5f, g_menuW - 1.0f, g_menuH - 1.0f, rad);
    StrokeRoundRect(g, th.border, 1.0f, 0.5f, 0.5f, g_menuW - 1.0f, g_menuH - 1.0f, rad);

    UpdateGdiCache(g_scale);
    Font* fontText = g_gc.fontMenuText;
    Font* fontHeader = g_gc.fontRoute;
    Font* fontCheck = g_gc.fontMenuCheck;

    StringFormat sf;
    sf.SetAlignment(StringAlignmentNear);
    sf.SetLineAlignment(StringAlignmentCenter);
    sf.SetTrimming(StringTrimmingEllipsisCharacter);
    sf.SetFormatFlags(StringFormatFlagsNoWrap);
    StringFormat sfCheck;
    sfCheck.SetAlignment(StringAlignmentCenter);
    sfCheck.SetLineAlignment(StringAlignmentCenter);

    SolidBrush textBr(ink);
    SolidBrush dimBr(th.muted);
    SolidBrush checkBr(th.trackFill);

    for (int i = 0; i < (int)g_menuItems.size(); i++) {
        const CustomMenuItem& m = g_menuItems[i];
        const float top = (float)m.top;
        const float h = (float)(m.bottom - m.top);
        if (m.isSeparator) {
            Pen sepPen(WithAlpha(ink, 40), 1.0f);
            g.DrawLine(&sepPen, (REAL)S(12), top + h / 2.0f, (REAL)(g_menuW - S(12)), top + h / 2.0f);
        } else if (m.isHeader) {
            g.DrawString(m.text.c_str(), -1, fontHeader, RectF((REAL)S(14), top, (REAL)(g_menuW - S(28)), h), &sf, &dimBr);
        } else {
            if (g_menuHover == i) {
                FillRoundRect(g, WithAlpha(ink, 26), (float)S(6), top + 1.0f, (float)(g_menuW - S(12)), h - 2.0f, (float)S(6));
            }
            if (m.isChecked) {
                g.DrawString(L"✓", -1, fontCheck, RectF((REAL)S(8), top, (REAL)S(18), h), &sfCheck, &checkBr);
            }
            float textX = (float)S(30);
            if (m.devType >= 0) {
                const float isz = (float)S(15);
                DrawDeviceVectorIcon(g, (DeviceType)m.devType, textX, top + (h - isz) / 2.0f, isz, ink);
                textX += isz + S(8);
            } else if (m.id == 999) {
                DrawLockBadge(g, textX + S(7), top + h / 2.0f, (float)S(10), th.bg);
                textX += S(22);
            }
            g.DrawString(m.text.c_str(), -1, fontText, RectF(textX, top, g_menuW - textX - S(10), h), &sf, &textBr);
        }
    }

    POINT ptSrc{0, 0};
    SIZE sz{g_menuW, g_menuH};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, (BYTE)std::lround(255.0 * kMenuOpacity / 100.0), AC_SRC_ALPHA};
    POINT ptDst{g_menuPos.x, g_menuPos.y};
    UpdateLayeredWindow(g_hwndMenu, nullptr, &ptDst, &sz, g_menuSurface.dc, &ptSrc, 0, &bf, ULW_ALPHA);
}

void ShowCustomRouteMenu(int itemIdx, POINT pt) {
    if (itemIdx < 0 || itemIdx >= (int)g_items.size() || !g_hwndMenu) return;
    const Item& it = g_items[itemIdx];
    g_menuTarget.kind = it.kind;
    g_menuTarget.key = it.key;
    g_menuTarget.deviceId = it.deviceId;
    g_menuTarget.pids = it.pids;
    g_menuHover = -1;

    g_menuItems.clear();
    if (it.kind != 0) {
        CustomMenuItem lockItem;
        lockItem.id = 999;
        lockItem.text = it.locked ? L10n(StrId::UnlockVolume) : L10n(StrId::LockVolume);
        lockItem.isChecked = it.locked;
        g_menuItems.push_back(lockItem);

        CustomMenuItem sep;
        sep.isSeparator = true;
        g_menuItems.push_back(sep);
    }

    CustomMenuItem hdr;
    hdr.isHeader = true;
    hdr.text = L10n(it.kind == 2 ? StrId::RouteMenuHeader : StrId::DefaultDeviceHeader);
    g_menuItems.push_back(hdr);

    if (it.kind == 2) {
        CustomMenuItem defDev;
        defDev.id = 998;
        defDev.text = L10n(StrId::SystemDefault);
        g_menuItems.push_back(defDev);
    }

    for (size_t i = 0; i < g_deviceList.size(); i++) {
        CustomMenuItem dev;
        dev.id = 1000 + (int)i;
        dev.text = g_deviceList[i].name;
        dev.deviceId = g_deviceList[i].id;
        dev.devType = (int)ClassifyDevice(g_deviceList[i].name);
        dev.isChecked = (g_deviceList[i].id == (it.kind == 2 ? it.deviceId : g_defaultDeviceId));
        g_menuItems.push_back(dev);
    }

    UpdateGdiCache(g_scale);
    int maxW = S(200);
    {
        Bitmap probe(1, 1, PixelFormat32bppARGB);
        Graphics mg(&probe);
        for (const auto& m : g_menuItems) {
            if (m.isSeparator) continue;
            RectF bound;
            mg.MeasureString(m.text.c_str(), -1, m.isHeader ? g_gc.fontRoute : g_gc.fontMenuText, PointF(0, 0), &bound);
            const int extra = m.isHeader ? S(30) : (m.devType >= 0 || m.id == 999) ? S(72) : S(46);
            maxW = std::max(maxW, (int)std::ceil(bound.Width) + extra);
        }
    }
    g_menuW = std::min(maxW, S(440));

    int curY = S(8);
    for (auto& m : g_menuItems) {
        m.top = curY;
        curY += m.isSeparator ? S(9) : m.isHeader ? S(24) : S(30);
        m.bottom = curY;
    }
    g_menuH = curY + S(8);

    const RECT wa = MonitorWorkArea(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
    const int rightCandidate = g_pos.x + g_winW + S(8);
    const int leftCandidate = g_pos.x - g_menuW - S(8);
    if (rightCandidate + g_menuW <= wa.right) {
        g_menuPos.x = rightCandidate;
    } else if (leftCandidate >= wa.left) {
        g_menuPos.x = leftCandidate;
    } else {
        g_menuPos.x = ClampSpan(pt.x, wa.left + S(8), wa.right - g_menuW - S(8));
    }
    g_menuPos.y = ClampSpan(pt.y - S(10), wa.top + S(8), wa.bottom - g_menuH - S(8));

    RenderCustomMenu();
    SetWindowPos(g_hwndMenu, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static int MenuItemAt(int x, int y) {
    if (x < 0 || x >= g_menuW) return -1;
    for (int i = 0; i < (int)g_menuItems.size(); i++) {
        const CustomMenuItem& m = g_menuItems[i];
        if (m.isSeparator || m.isHeader) continue;
        if (y >= m.top && y < m.bottom) return i;
    }
    return -1;
}

LRESULT CALLBACK MenuWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;

    case WM_MOUSEMOVE: {
        if (!g_menuTracking) {
            TRACKMOUSEEVENT tme{};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            g_menuTracking = TrackMouseEvent(&tme) != FALSE;
        }
        const int hovered = MenuItemAt((short)LOWORD(lParam), (short)HIWORD(lParam));
        if (hovered != g_menuHover) {
            g_menuHover = hovered;
            RenderCustomMenu();
        }
        g_lastActive = GetTickCount();
        return 0;
    }

    case WM_MOUSELEAVE:
        g_menuTracking = false;
        if (g_menuHover != -1) {
            g_menuHover = -1;
            RenderCustomMenu();
        }
        return 0;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN: {
        const int i = MenuItemAt((short)LOWORD(lParam), (short)HIWORD(lParam));
        if (i >= 0) {
            const CustomMenuItem item = g_menuItems[i];
            HideCustomMenu();
            ExecuteMenuCommand(item);
        }
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Audio Notifications, System Tray Icon, and Low-Level Hooks

////////////////////////////////////////////////////////////////////////////////
// Endpoint notifications: media keys, other apps and Windows itself change the
// volume or the default device; the tray icon and the open popup follow.

class AudioNotifier final : public IAudioEndpointVolumeCallback, public IMMNotificationClient {
public:
    ULONG STDMETHODCALLTYPE AddRef() override { return (ULONG)InterlockedIncrement(&m_ref); }
    ULONG STDMETHODCALLTYPE Release() override {
        const LONG r = InterlockedDecrement(&m_ref);
        if (r == 0) delete this;
        return (ULONG)r;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == __uuidof(IAudioEndpointVolumeCallback)) {
            *ppv = static_cast<IAudioEndpointVolumeCallback*>(this);
        } else if (riid == __uuidof(IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
        } else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }

    // Called on audio service threads: only post to the UI thread.
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override {
        PostCoalesced(g_volumeMsgPending, WM_APP_VOLUME_CHANGED);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override {
        PostCoalesced(g_deviceMsgPending, WM_APP_DEVICES_CHANGED);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override {
        PostCoalesced(g_deviceMsgPending, WM_APP_DEVICES_CHANGED);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override {
        PostCoalesced(g_deviceMsgPending, WM_APP_DEVICES_CHANGED);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR) override {
        if (flow == eRender && role == eMultimedia) PostCoalesced(g_deviceMsgPending, WM_APP_DEVICES_CHANGED);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override { return S_OK; }

private:
    LONG m_ref = 1;
};

IMMDeviceEnumerator* g_notifyEnum = nullptr;
IAudioEndpointVolume* g_notifyEp = nullptr;
AudioNotifier* g_notifier = nullptr;

void RebindDefaultEndpoint() {
    if (g_notifyEp) {
        g_notifyEp->UnregisterControlChangeNotify(g_notifier);
        g_notifyEp->Release();
        g_notifyEp = nullptr;
    }
    if (!g_notifyEnum || !g_notifier) return;
    IMMDevice* dev = nullptr;
    if (SUCCEEDED(g_notifyEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev)) && dev) {
        IAudioEndpointVolume* ep = nullptr;
        if (SUCCEEDED(dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&ep)) && ep) {
            if (SUCCEEDED(ep->RegisterControlChangeNotify(g_notifier))) g_notifyEp = ep;
            else ep->Release();
        }
        dev->Release();
    }
}

void StartAudioNotifications() {
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&g_notifyEnum))) {
        g_notifyEnum = nullptr;
        return;
    }
    g_notifier = new AudioNotifier();
    if (FAILED(g_notifyEnum->RegisterEndpointNotificationCallback(g_notifier))) {
        Wh_Log(L"RegisterEndpointNotificationCallback failed");
    }
    RebindDefaultEndpoint();
}

void StopAudioNotifications() {
    if (g_notifyEp) {
        g_notifyEp->UnregisterControlChangeNotify(g_notifier);
        g_notifyEp->Release();
        g_notifyEp = nullptr;
    }
    if (g_notifyEnum) {
        if (g_notifier) g_notifyEnum->UnregisterEndpointNotificationCallback(g_notifier);
        g_notifyEnum->Release();
        g_notifyEnum = nullptr;
    }
    if (g_notifier) {
        g_notifier->Release();
        g_notifier = nullptr;
    }
}

static bool ReadDefaultMaster(float* vol, bool* mute) {
    IAudioEndpointVolume* ep = g_notifyEp;
    if (ep) ep->AddRef();
    else ep = GetDefaultEndpointVolume(nullptr);
    if (!ep) return false;
    BOOL m = FALSE;
    ep->GetMasterVolumeLevelScalar(vol);
    ep->GetMute(&m);
    *mute = m != FALSE;
    ep->Release();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// System Tray Icon

// Cached; reset by the "ImmersiveColorSet" WM_SETTINGCHANGE broadcast.
int g_lightTaskbar = -1;

bool IsLightTaskbar() {
    if (g_lightTaskbar < 0) {
        DWORD v = 0;
        DWORD sz = sizeof(v);
        g_lightTaskbar = (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                                       L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz) == ERROR_SUCCESS && v) ? 1 : 0;
    }
    return g_lightTaskbar == 1;
}

HICON CreateTrayIcon(const std::wstring& style, float vol, bool mute, bool light) {
    int sz = GetSystemMetrics(SM_CXSMICON);
    sz = std::clamp(sz <= 0 ? 16 : sz, 16, 64);

    if (style == L"system" && !light) {
        // SndVolSSO.dll icons: 1 = muted, 2 = no waves, 3/4/5 = one/two/three waves.
        // They are white, so a light taskbar falls back to the drawn speaker.
        const int idx = mute ? 1 : vol <= 0.001f ? 2 : vol < 0.34f ? 3 : vol < 0.67f ? 4 : 5;
        WCHAR sysPath[MAX_PATH] = {0};
        GetSystemDirectoryW(sysPath, MAX_PATH);
        const std::wstring dll = FileAccessPath(std::wstring(sysPath) + L"\\SndVolSSO.dll");
        HICON hIcon = nullptr;
        if (SUCCEEDED(SHDefExtractIconW(dll.c_str(), idx, 0, &hIcon, nullptr, MAKELONG(sz, sz))) && hIcon) return hIcon;
        if (ExtractIconExW(dll.c_str(), idx, nullptr, &hIcon, 1) > 0 && hIcon) return hIcon;
    }

    Bitmap bmp(sz, sz, PixelFormat32bppARGB);
    Graphics g(&bmp);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.Clear(Color(0, 0, 0, 0));

    const Color fg = light ? Color(255, 28, 28, 28) : Color(255, 255, 255, 255);
    const Color col = mute ? Color(240, 232, 72, 72) : fg;
    Pen pen(col, sz * 0.07f);
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
    SolidBrush brush(col);

    const float fSz = (float)sz;

    if (style == L"faders") {
        const float x1 = fSz * 0.22f, x2 = fSz * 0.50f, x3 = fSz * 0.78f;
        const float top = fSz * 0.12f, bot = fSz * 0.88f;
        Pen trackPen(WithAlpha(fg, 120), fSz * 0.055f);
        trackPen.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
        g.DrawLine(&trackPen, x1, top, x1, bot);
        g.DrawLine(&trackPen, x2, top, x2, bot);
        g.DrawLine(&trackPen, x3, top, x3, bot);

        const float lvl = mute ? 0.0f : std::clamp(vol, 0.0f, 1.0f);
        const float k1 = top + (bot - top) * 0.65f;
        const float k2 = bot - (bot - top) * lvl;
        const float k3 = top + (bot - top) * 0.75f;
        const float kw = fSz * 0.20f, kh = fSz * 0.13f;
        FillRoundRect(g, col, x1 - kw / 2, k1 - kh / 2, kw, kh, 1.5f);
        FillRoundRect(g, col, x2 - kw / 2, k2 - kh / 2, kw, kh, 1.5f);
        FillRoundRect(g, col, x3 - kw / 2, k3 - kh / 2, kw, kh, 1.5f);
    } else if (style == L"wave" || style == L"equalizer_bars") {
        const bool eq = (style == L"equalizer_bars");
        const float shape[5] = {0.40f, 0.78f, 1.0f, 0.66f, 0.45f};
        const float level = mute ? 0.25f : 0.35f + 0.65f * std::clamp(vol, 0.0f, 1.0f);
        const float bw = fSz * (eq ? 0.13f : 0.11f);
        const float gap = fSz * (eq ? 0.05f : 0.07f);
        const float startX = (fSz - (5 * bw + 4 * gap)) / 2.0f;
        for (int i = 0; i < 5; i++) {
            const float bh = std::max(fSz * 0.15f, fSz * 0.88f * shape[i] * level);
            const float bx = startX + i * (bw + gap);
            const float by = eq ? (fSz * 0.94f - bh) : (fSz - bh) / 2.0f;  // equalizer grows from the floor
            FillRoundRect(g, col, bx, by, bw, bh, eq ? 1.0f : 1.5f);
        }
    } else if (style == L"soundwave_circle") {
        const float cx = fSz * 0.50f, cy = fSz * 0.50f;
        g.FillEllipse(&brush, cx - fSz * 0.15f, cy - fSz * 0.15f, fSz * 0.30f, fSz * 0.30f);
        if (!mute && vol > 0.05f) {
            Pen ring1(WithAlpha(fg, 180), fSz * 0.06f);
            g.DrawEllipse(&ring1, cx - fSz * 0.28f, cy - fSz * 0.28f, fSz * 0.56f, fSz * 0.56f);
        }
        if (!mute && vol > 0.5f) {
            Pen ring2(WithAlpha(fg, 100), fSz * 0.05f);
            g.DrawEllipse(&ring2, cx - fSz * 0.42f, cy - fSz * 0.42f, fSz * 0.84f, fSz * 0.84f);
        }
    } else if (style == L"headphones") {
        Pen hpPen(col, fSz * 0.08f);
        hpPen.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
        g.DrawArc(&hpPen, fSz * 0.18f, fSz * 0.15f, fSz * 0.64f, fSz * 0.60f, 180.0f, 180.0f);
        FillRoundRect(g, col, fSz * 0.12f, fSz * 0.45f, fSz * 0.16f, fSz * 0.40f, 2.0f);
        FillRoundRect(g, col, fSz * 0.72f, fSz * 0.45f, fSz * 0.16f, fSz * 0.40f, 2.0f);
    } else if (style == L"retro_knob") {
        const float cx = fSz * 0.50f, cy = fSz * 0.50f, r = fSz * 0.36f;
        Pen rimPen(WithAlpha(fg, 160), fSz * 0.065f);
        g.DrawEllipse(&rimPen, cx - r, cy - r, r * 2.0f, r * 2.0f);
        // 0 % points to the lower left (135 deg), 100 % to the lower right (405 deg)
        const float angle = 135.0f + std::clamp(vol, 0.0f, 1.0f) * 270.0f;
        const float rads = angle * 3.14159265f / 180.0f;
        const float nx = cx + (r - fSz * 0.08f) * cosf(rads);
        const float ny = cy + (r - fSz * 0.08f) * sinf(rads);
        Pen needlePen(col, fSz * 0.08f);
        needlePen.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
        g.DrawLine(&needlePen, cx, cy, nx, ny);
    } else if (style == L"minimal_dot") {
        const float cx = fSz * 0.50f, cy = fSz * 0.50f;
        const float dotR = mute ? fSz * 0.14f : fSz * 0.20f;
        g.FillEllipse(&brush, cx - dotR, cy - dotR, dotR * 2.0f, dotR * 2.0f);
        if (!mute) {
            Pen glowPen(WithAlpha(fg, 120), fSz * 0.06f);
            g.DrawEllipse(&glowPen, cx - fSz * 0.34f, cy - fSz * 0.34f, fSz * 0.68f, fSz * 0.68f);
        }
    } else {  // Fluent Vector Speaker (default; also "system" on a light taskbar)
        const float sx = fSz * 0.12f;
        const float sy = fSz * 0.50f;
        PointF pts[6] = {
            {sx, sy - fSz * 0.14f},
            {sx + fSz * 0.16f, sy - fSz * 0.14f},
            {sx + fSz * 0.35f, sy - fSz * 0.28f},
            {sx + fSz * 0.35f, sy + fSz * 0.28f},
            {sx + fSz * 0.16f, sy + fSz * 0.14f},
            {sx, sy + fSz * 0.14f}
        };
        SolidBrush spk(fg);
        g.FillPolygon(&spk, pts, 6);

        if (mute) {
            Pen xPen(Color(240, 232, 72, 72), fSz * 0.08f);
            xPen.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
            const float x1 = fSz * 0.58f, x2 = fSz * 0.86f;
            const float y1 = sy - fSz * 0.14f, y2 = sy + fSz * 0.14f;
            g.DrawLine(&xPen, x1, y1, x2, y2);
            g.DrawLine(&xPen, x2, y1, x1, y2);
        } else {
            Pen wave(fg, fSz * 0.07f);
            wave.SetLineCap(LineCapRound, LineCapRound, DashCapFlat);
            if (vol > 0.01f) g.DrawArc(&wave, fSz * 0.30f, sy - fSz * 0.20f, fSz * 0.40f, fSz * 0.40f, -40.0f, 80.0f);
            if (vol > 0.34f) g.DrawArc(&wave, fSz * 0.24f, sy - fSz * 0.30f, fSz * 0.60f, fSz * 0.60f, -45.0f, 90.0f);
            if (vol > 0.67f) g.DrawArc(&wave, fSz * 0.18f, sy - fSz * 0.40f, fSz * 0.80f, fSz * 0.80f, -50.0f, 100.0f);
        }
    }

    HICON hIcon = nullptr;
    bmp.GetHICON(&hIcon);
    return hIcon;
}

bool GetTrayIconRect(RECT* rc) {
    if (!g_trayAdded || !g_hwnd) return false;
    NOTIFYICONIDENTIFIER id{};
    id.cbSize = sizeof(id);
    id.hWnd = g_hwnd;
    id.uID = TRAY_ICON_ID;
    return SUCCEEDED(Shell_NotifyIconGetRect(&id, rc)) && !IsRectEmpty(rc);
}

// The mouse hook only gets a cached rectangle; the UI thread verifies it again.
void RefreshTrayRect() {
    RECT rc{};
    if (!GetTrayIconRect(&rc)) SetRectEmpty(&rc);
    AcquireSRWLockExclusive(&g_hookLock);
    g_hookTrayRect = rc;
    ReleaseSRWLockExclusive(&g_hookLock);
}

void RemoveTrayIcon() {
    if (g_trayAdded) Shell_NotifyIconW(NIM_DELETE, &g_nid);
    g_trayAdded = false;
    g_traySig.clear();
    memset(&g_nid, 0, sizeof(g_nid));
    if (g_hTrayIcon) {
        DestroyIcon(g_hTrayIcon);
        g_hTrayIcon = nullptr;
    }
    RefreshTrayRect();
    UpdateHooks();
}

void UpdateTrayIcon(bool force) {
    if (!g_cfg.showTrayIcon || !g_hwnd) {
        if (g_trayAdded || g_hTrayIcon) RemoveTrayIcon();
        return;
    }

    float vol = 1.0f;
    bool mute = false;
    ReadDefaultMaster(&vol, &mute);
    const bool light = IsLightTaskbar();
    const int pct = (int)std::lround(vol * 100);

    // Nothing to do when the icon would look the same (slider drags call this often).
    WCHAR sig[160];
    swprintf_s(sig, L"%ls|%d|%d|%d|%d", g_cfg.trayIconStyle.c_str(), pct, mute ? 1 : 0, light ? 1 : 0,
               (int)GetCurrentLanguage());
    if (!force && g_trayAdded && g_traySig == sig) return;

    HICON icon = CreateTrayIcon(g_cfg.trayIconStyle, vol, mute, light);

    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwnd;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
    nid.uCallbackMessage = WM_APP_TRAY;
    nid.hIcon = icon;
    if (mute) swprintf_s(nid.szTip, L"%ls: %ls", L10n(StrId::MasterVolume), L10n(StrId::Muted));
    else swprintf_s(nid.szTip, L"%ls: %d%%", L10n(StrId::MasterVolume), pct);

    bool ok = g_trayAdded && Shell_NotifyIconW(NIM_MODIFY, &nid);
    bool added = false;
    if (!ok) {
        ok = Shell_NotifyIconW(NIM_ADD, &nid) != FALSE;
        added = ok;
        if (ok) {
            nid.uVersion = NOTIFYICON_VERSION_4;
            Shell_NotifyIconW(NIM_SETVERSION, &nid);
        }
    }

    if (ok) {
        if (g_hTrayIcon) DestroyIcon(g_hTrayIcon);
        g_hTrayIcon = icon;
        g_nid = nid;
        g_trayAdded = true;
        g_traySig = sig;
        if (added) {
            Wh_Log(L"Tray icon added");
            RefreshTrayRect();
            UpdateHooks();
        }
    } else {
        // Usually the taskbar is not up yet; TaskbarCreated will retry.
        if (icon) DestroyIcon(icon);
        g_trayAdded = false;
        g_traySig.clear();
        Wh_Log(L"Shell_NotifyIconW failed, err=%lu", GetLastError());
    }
}

void ShowPopup(bool fromTray);
void HidePopup();

void TrayToggle() {
    const DWORD now = GetTickCount();
    // One click arrives as WM_LBUTTONUP + NIN_SELECT (WM_RBUTTONUP + WM_CONTEXTMENU).
    if (now - g_lastTrayToggle < 350) return;
    g_lastTrayToggle = now;
    if (g_visible && !g_hiding) {
        HidePopup();
        return;
    }
    // The mouse-down of this very click on the icon has already closed the popup.
    if (now - g_hookOutsideClickTick.load() < 500) return;
    ShowPopup(true);
}

////////////////////////////////////////////////////////////////////////////////
// Low-level keyboard & mouse hooks (own thread)

static bool IsModifierVk(DWORD vk) {
    return vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL ||
           vk == VK_MENU || vk == VK_LMENU || vk == VK_RMENU ||
           vk == VK_SHIFT || vk == VK_LSHIFT || vk == VK_RSHIFT ||
           vk == VK_LWIN || vk == VK_RWIN;
}

constexpr WORD kMenuMaskVk = 0xE8;  // unassigned virtual key

// When a hotkey with Alt or Win is swallowed, the foreground app only sees Alt
// (or Win) go down and up, which opens its menu bar (or the Start menu).
// An unassigned key in between prevents that.
static void SendMenuMaskKey() {
    INPUT in[2] = {};
    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wVk = kMenuMaskVk;
    in[1] = in[0];
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) return CallNextHookEx(nullptr, nCode, wParam, lParam);
    const KBDLLHOOKSTRUCT* kbd = (const KBDLLHOOKSTRUCT*)lParam;
    const DWORD vk = kbd->vkCode;
    static DWORD s_heldVk = 0;
    static DWORD s_lctrlDownTime = 0;
    static bool s_altGr = false;

    const bool volumeKey = (vk == VK_VOLUME_UP || vk == VK_VOLUME_DOWN || vk == VK_VOLUME_MUTE);

    if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        if (vk == s_heldVk) s_heldVk = 0;
        if (vk == VK_LCONTROL || vk == VK_RMENU) s_altGr = false;
        // The release of a replaced media key must not reach Windows either.
        if (volumeKey && g_hookReplaceOsd) return 1;
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    if ((wParam != WM_KEYDOWN && wParam != WM_SYSKEYDOWN) || vk == kMenuMaskVk) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    const bool repeat = (vk == s_heldVk);
    s_heldVk = vk;
    // AltGr arrives as a fake Left Ctrl (scan code 0x21D, same timestamp)
    // followed by Right Alt. On layouts with AltGr it types characters, so it
    // never counts as Ctrl+Alt.
    if (vk == VK_LCONTROL) {
        s_lctrlDownTime = kbd->time;
        if (kbd->scanCode == 0x21D) s_altGr = true;
    } else if (vk == VK_RMENU && kbd->time == s_lctrlDownTime) {
        s_altGr = true;
    }
    if (IsModifierVk(vk)) return CallNextHookEx(nullptr, nCode, wParam, lParam);

    const bool altGr = s_altGr && GetAsyncKeyState(VK_RMENU) < 0;
    const bool ctrl = altGr ? GetAsyncKeyState(VK_RCONTROL) < 0 : GetAsyncKeyState(VK_CONTROL) < 0;
    const bool alt = altGr ? GetAsyncKeyState(VK_LMENU) < 0
                           : ((kbd->flags & LLKHF_ALTDOWN) != 0 || GetAsyncKeyState(VK_MENU) < 0);
    const bool shift = GetAsyncKeyState(VK_SHIFT) < 0;
    const bool win = GetAsyncKeyState(VK_LWIN) < 0 || GetAsyncKeyState(VK_RWIN) < 0;
    const UINT mods = (ctrl ? MOD_CONTROL : 0) | (alt ? MOD_ALT : 0) | (shift ? MOD_SHIFT : 0) | (win ? MOD_WIN : 0);

    int hit = -1;
    AcquireSRWLockShared(&g_hookLock);
    for (int i = 0; i < HK_COUNT; i++) {
        if (g_hookHotkeys[i].vk == vk && g_hookHotkeys[i].mods == mods) {
            hit = i;
            break;
        }
    }
    ReleaseSRWLockShared(&g_hookLock);

    if (hit >= 0) {
        // Toggles fire once per press; volume steps repeat while the key is held.
        if (!repeat || kHotkeyRepeats[hit]) PostMessageW(g_hwnd, WM_HOTKEY, hit + 1, 0);
        if ((alt || win) && !repeat) SendMenuMaskKey();
        return 1;
    }

    if (g_hookReplaceOsd && volumeKey) {
        // Swallowed so Windows never sees the key: it neither changes the
        // volume nor shows its flyout; the mod does both instead.
        if (vk != VK_VOLUME_MUTE || !repeat) PostMessageW(g_hwnd, WM_APP_MEDIA_KEY, vk, 0);
        return 1;
    }

    // Any other key closes the mixer; Escape is swallowed.
    if (g_hookPopupOpen && GetTickCount() - g_hookShowTick.load() > 150) {
        PostMessageW(g_hwnd, WM_APP_CLOSE_MIXER, 0, 0);
        if (vk == VK_ESCAPE) return 1;
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// The notification area of the visible primary taskbar, where Windows 11 shows
// its own volume tooltip when the wheel turns over the speaker icon.
static bool PtInNotifyArea(POINT pt) {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    HWND notify = tray ? FindWindowExW(tray, nullptr, L"TrayNotifyWnd", nullptr) : nullptr;
    RECT rc{};
    if (!notify || !GetWindowRect(notify, &rc) || !PtInRect(&rc, pt)) return false;
    // A full-screen window can cover the taskbar.
    return GetAncestor(WindowFromPoint(pt), GA_ROOT) == tray;
}

// Clicks are never swallowed: a click outside closes the popup and still
// reaches the window under the cursor. The wheel is swallowed only over the OSD
// and, when the Windows flyout is replaced, over the notification area.
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const MSLLHOOKSTRUCT* ms = (const MSLLHOOKSTRUCT*)lParam;
        if (wParam == WM_MOUSEWHEEL) {
            const WPARAM delta = (WPARAM)(SHORT)HIWORD(ms->mouseData);
            if (g_hookOsdHover) {
                AcquireSRWLockShared(&g_hookLock);
                const RECT rc = g_hookOsdRect;
                ReleaseSRWLockShared(&g_hookLock);
                if (PtInRect(&rc, ms->pt)) {
                    PostMessageW(g_hwnd, WM_APP_OSD_WHEEL, delta, 0);
                    return 1;
                }
            }
            bool onTrayIcon = false;
            if (g_hookTrayWheel) {
                AcquireSRWLockShared(&g_hookLock);
                const RECT rc = g_hookTrayRect;
                ReleaseSRWLockShared(&g_hookLock);
                if (PtInRect(&rc, ms->pt)) {
                    onTrayIcon = true;
                    PostMessageW(g_hwnd, WM_APP_TRAY_WHEEL, delta, MAKELPARAM(ms->pt.x, ms->pt.y));
                }
            }
            if (g_hookReplaceOsd && PtInNotifyArea(ms->pt)) {
                if (!onTrayIcon) PostMessageW(g_hwnd, WM_APP_TASKBAR_WHEEL, delta, 0);
                return 1;
            }
        } else if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
                   wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN) {
            if (g_hookPopupOpen && GetTickCount() - g_hookShowTick.load() > 100) {
                RECT wr{}, mr{};
                const bool inMain = GetWindowRect(g_hwnd, &wr) && PtInRect(&wr, ms->pt);
                const bool menuOpen = g_hwndMenu && IsWindowVisible(g_hwndMenu);
                const bool inMenu = menuOpen && GetWindowRect(g_hwndMenu, &mr) && PtInRect(&mr, ms->pt);
                if (!inMain && !inMenu) {
                    g_hookOutsideClickTick = GetTickCount();
                    PostMessageW(g_hwnd, WM_APP_CLOSE_MIXER, 0, 0);
                } else if (menuOpen && !inMenu) {
                    PostMessageW(g_hwnd, WM_APP_CLOSE_MENU, 0, 0);
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID param) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // create the message queue
    HHOOK kbdHook = nullptr;
    HHOOK mouseHook = nullptr;
    SetEvent((HANDLE)param);

    // Each hook is installed and removed on request from the UI thread.
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP_HOOK_KBD) {
            if (msg.wParam && !kbdHook) {
                kbdHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, g_hInst, 0);
                if (!kbdHook) Wh_Log(L"Keyboard hook failed, err=%lu", GetLastError());
            } else if (!msg.wParam && kbdHook) {
                UnhookWindowsHookEx(kbdHook);
                kbdHook = nullptr;
            }
        } else if (msg.message == WM_APP_HOOK_MOUSE) {
            if (msg.wParam && !mouseHook) {
                mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, g_hInst, 0);
            } else if (!msg.wParam && mouseHook) {
                UnhookWindowsHookEx(mouseHook);
                mouseHook = nullptr;
            }
        }
    }

    if (mouseHook) UnhookWindowsHookEx(mouseHook);
    if (kbdHook) UnhookWindowsHookEx(kbdHook);
    return 0;
}

void StartHookThread() {
    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_mouseHookWanted = false;
    g_kbdHookWanted = false;
    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, ready, 0, &g_hookThreadId);
    if (g_hookThread && ready) WaitForSingleObject(ready, 5000);
    if (ready) CloseHandle(ready);
    if (!g_hookThread) {
        g_hookThreadId = 0;
        Wh_Log(L"Failed to create the hook thread");
    }
}

void StopHookThread() {
    if (!g_hookThread) return;
    PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_hookThread, INFINITE);
    CloseHandle(g_hookThread);
    g_hookThread = nullptr;
    g_hookThreadId = 0;
    g_mouseHookWanted = false;
    g_kbdHookWanted = false;
}

// Each hook is installed only while a feature needs it: the keyboard hook for
// configured hotkeys, the media key replacement and the open popup (any key
// closes it); the mouse hook for the open popup, the tray wheel, the wheel
// over a hovered OSD and the replacement of the Windows volume tooltip in the
// notification area.
void UpdateHooks() {
    const bool trayWheel = g_cfg.showTrayIcon && g_cfg.trayScrollVolume && g_trayAdded;
    g_hookTrayWheel = trayWheel;
    bool anyHotkey = false;
    AcquireSRWLockShared(&g_hookLock);
    for (const HotkeyDef& h : g_hookHotkeys) {
        if (h.vk) anyHotkey = true;
    }
    ReleaseSRWLockShared(&g_hookLock);
    if (!g_hookThreadId) return;

    const bool wantKbd = anyHotkey || g_hookReplaceOsd || g_visible;
    if (wantKbd != g_kbdHookWanted &&
        PostThreadMessageW(g_hookThreadId, WM_APP_HOOK_KBD, wantKbd ? 1 : 0, 0)) {
        g_kbdHookWanted = wantKbd;
    }
    const bool wantMouse = g_visible || trayWheel || g_hookReplaceOsd || g_hookOsdHover;
    if (wantMouse != g_mouseHookWanted &&
        PostThreadMessageW(g_hookThreadId, WM_APP_HOOK_MOUSE, wantMouse ? 1 : 0, 0)) {
        g_mouseHookWanted = wantMouse;
    }
}

void SyncPopupHookState() {
    g_hookPopupOpen = g_visible && !g_hiding;
    g_hookShowTick = g_showTick;
    UpdateHooks();
}

// Touchpad gestures, headset buttons and laptop hotkey drivers change the volume
// without going through the keyboard hook, and Windows then shows its own
// flyout. With "Replace the Windows volume flyout", that flyout is clipped to an
// empty region and hidden as soon as it appears, and this mod's OSD is shown
// instead and follows every further step.
HWINEVENTHOOK g_flyoutHook = nullptr;
DWORD g_flyoutHookPid = 0;
HWND g_sysFlyout = nullptr;         // Windows' indicator window, once clipped or unclipped
bool g_sysFlyoutClipped = false;
HWND g_sysFlyoutPending = nullptr;  // shown before its volume change was seen
DWORD g_sysFlyoutTick = 0;
DWORD g_volumeChangeTick = 0;
float g_seenMasterVol = -1.0f;      // the master level and mute as the mod last read them
bool g_seenMasterMute = false;
constexpr DWORD kFlyoutMatchMs = 500;  // the flyout and the volume event arrive in either order

// Windows' indicator window in Explorer: an untitled, non-activating XAML
// island host on Windows 11, a NativeHWNDHost with a DirectUIHWND on Windows 10.
// It also shows brightness and other indicators, so it's only hidden right
// after a volume change.
static bool IsSystemIndicatorWindow(HWND hwnd) {
    if (GetAncestor(hwnd, GA_ROOT) != hwnd || GetWindowTextLengthW(hwnd) != 0) return false;
    WCHAR cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"XamlExplorerHostIslandWindow") == 0) {
        return (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE) != 0;
    }
    return wcscmp(cls, L"NativeHWNDHost") == 0 && FindWindowExW(hwnd, nullptr, L"DirectUIHWND", nullptr);
}

static void ShowMasterVolumeOsd() {
    std::wstring devName;
    IAudioEndpointVolume* ep = GetDefaultEndpointVolume(&devName);
    if (!ep) return;
    float v = 0.0f;
    BOOL m = FALSE;
    ep->GetMasterVolumeLevelScalar(&v);
    ep->GetMute(&m);
    ep->Release();
    g_osdTarget = OsdTarget::Master;
    g_osdMasterVol = v;
    g_osdMasterMute = m != FALSE;
    ShowOsd(L10n(StrId::MasterVolume), v, m != FALSE, nullptr, OsdKind::Always, devName);
}

// An empty window region keeps Windows' indicator invisible even while Explorer
// shows it again for every step of a gesture, so it never flashes. Brightness
// and other indicators in the same window get it back unclipped.
static void ClipSystemFlyout(HWND hwnd, bool clip) {
    if (hwnd == g_sysFlyout && clip == g_sysFlyoutClipped) return;
    HRGN rgn = clip ? CreateRectRgn(0, 0, 0, 0) : nullptr;
    if (SetWindowRgn(hwnd, rgn, TRUE)) {  // the window owns the region from here on
        g_sysFlyout = hwnd;
        g_sysFlyoutClipped = clip;
    } else if (rgn) {
        DeleteObject(rgn);
    }
}

static void UnclipSystemFlyout() {
    if (g_sysFlyout && g_sysFlyoutClipped && IsWindow(g_sysFlyout)) SetWindowRgn(g_sysFlyout, nullptr, TRUE);
    g_sysFlyout = nullptr;
    g_sysFlyoutClipped = false;
}

// Reads the master level and mute; true when they differ from the last read.
static bool ReadMasterChange(float* vol, bool* mute) {
    if (!ReadDefaultMaster(vol, mute)) return false;
    const bool changed = std::abs(*vol - g_seenMasterVol) > 0.0005f || *mute != g_seenMasterMute;
    g_seenMasterVol = *vol;
    g_seenMasterMute = *mute;
    return changed;
}

static void ReplaceSystemFlyout(HWND hwnd) {
    ClipSystemFlyout(hwnd, true);
    ShowWindow(hwnd, SW_HIDE);
    g_sysFlyoutPending = nullptr;
    ShowMasterVolumeOsd();
}

// Explorer changes the volume before it shows its flyout, so a level that
// differs from the last read tells a volume flyout from a brightness one at
// once, without waiting for the volume event.
static void CALLBACK FlyoutWinEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd || !IsSystemIndicatorWindow(hwnd)) return;
    float v = 0.0f;
    bool m = false;
    if (ReadMasterChange(&v, &m) || GetTickCount() - g_volumeChangeTick < kFlyoutMatchMs) {
        ReplaceSystemFlyout(hwnd);
        return;
    }
    ClipSystemFlyout(hwnd, false);
    g_sysFlyoutPending = hwnd;
    g_sysFlyoutTick = GetTickCount();
}

// Called for every change of the default device's volume or mute.
void OnMasterVolumeChanged() {
    g_volumeChangeTick = GetTickCount();
    float v = 0.0f;
    bool m = false;
    ReadMasterChange(&v, &m);
    if (!g_flyoutHook) return;
    if (g_sysFlyoutPending && GetTickCount() - g_sysFlyoutTick < kFlyoutMatchMs &&
        IsWindowVisible(g_sysFlyoutPending)) {
        ReplaceSystemFlyout(g_sysFlyoutPending);
    } else if (IsWindowVisible(g_hwndOsd) && g_osdTarget == OsdTarget::Master &&
               (std::abs(v - g_osdMasterVol) > 0.0005f || m != g_osdMasterMute)) {
        ShowMasterVolumeOsd();  // follows a gesture step by step
    }
}

// Watches Explorer's windows being shown while the setting is on; follows
// Explorer restarts (TaskbarCreated).
void UpdateFlyoutHook() {
    DWORD pid = 0;
    HWND tray = g_cfg.replaceWindowsOsd ? FindWindowW(L"Shell_TrayWnd", nullptr) : nullptr;
    if (tray) GetWindowThreadProcessId(tray, &pid);
    if (pid == g_flyoutHookPid) return;
    if (g_flyoutHook) UnhookWinEvent(g_flyoutHook);
    UnclipSystemFlyout();
    g_sysFlyoutPending = nullptr;
    g_flyoutHook = pid ? SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, FlyoutWinEventProc, pid, 0,
                                         WINEVENT_OUTOFCONTEXT)
                       : nullptr;
    g_flyoutHookPid = g_flyoutHook ? pid : 0;
    float v = 0.0f;
    bool m = false;
    if (g_flyoutHook) ReadMasterChange(&v, &m);
}

// Leaves Windows' indicator as it was.
void StopFlyoutHook() {
    if (g_flyoutHook) UnhookWinEvent(g_flyoutHook);
    g_flyoutHook = nullptr;
    g_flyoutHookPid = 0;
    g_sysFlyoutPending = nullptr;
    UnclipSystemFlyout();
}

// Hotkeys go through the keyboard hook (works in fullscreen games, no repeats
// of toggles). RegisterHotKey stays as a fallback for elevated foreground
// windows, which a non-elevated hook cannot see.
void RegisterAllHotkeys() {
    const std::wstring* texts[HK_COUNT] = {
        &g_cfg.hotkey, &g_cfg.hotkeyVolUp, &g_cfg.hotkeyVolDown, &g_cfg.hotkeyMute,
        &g_cfg.hotkeyMasterVolUp, &g_cfg.hotkeyMasterVolDown, &g_cfg.hotkeyMasterMute, &g_cfg.hotkeyDeafen
    };
    HotkeyDef defs[HK_COUNT];
    for (int i = 0; i < HK_COUNT; i++) {
        UnregisterHotKey(g_hwnd, i + 1);
        if (!ParseHotkey(*texts[i], &defs[i].mods, &defs[i].vk)) defs[i] = HotkeyDef{};
    }
    if (!g_cfg.enableDeafenBackground) defs[HK_DEAFEN] = HotkeyDef{};

    AcquireSRWLockExclusive(&g_hookLock);
    for (int i = 0; i < HK_COUNT; i++) g_hookHotkeys[i] = defs[i];
    ReleaseSRWLockExclusive(&g_hookLock);

    for (int i = 0; i < HK_COUNT; i++) {
        // Not for Ctrl+Alt combinations: RegisterHotKey can't tell them from
        // AltGr and would take characters such as AltGr+V away from typing.
        const bool ctrlAlt = (defs[i].mods & (MOD_CONTROL | MOD_ALT)) == (MOD_CONTROL | MOD_ALT) &&
                             !(defs[i].mods & MOD_WIN);
        if (defs[i].vk && !ctrlAlt) RegisterHotKey(g_hwnd, i + 1, defs[i].mods | MOD_NOREPEAT, defs[i].vk);
    }
    // Independent of "Enable volume OSD": replacing the Windows flyout always
    // shows this mod's OSD for the media keys.
    g_hookReplaceOsd = g_cfg.replaceWindowsOsd;
    UpdateHooks();
    UpdateFlyoutHook();
}

void UnregisterAllHotkeys() {
    for (int i = 0; i < HK_COUNT; i++) UnregisterHotKey(g_hwnd, i + 1);
    AcquireSRWLockExclusive(&g_hookLock);
    for (int i = 0; i < HK_COUNT; i++) g_hookHotkeys[i] = HotkeyDef{};
    ReleaseSRWLockExclusive(&g_hookLock);
    g_hookReplaceOsd = false;
}

// Configuration Loader, Main Window Procedure, UI Thread, and Windhawk Lifecycle

static std::wstring SettingStr(PCWSTR name) {
    return Trim(WindhawkUtils::StringSetting::make(name).get());
}

// Unknown values (e.g. ids of older mod versions) fall back to the default.
static std::wstring SettingChoice(PCWSTR name, std::initializer_list<const wchar_t*> allowed, const wchar_t* def) {
    const std::wstring s = SettingStr(name);
    for (const wchar_t* a : allowed) {
        if (s == a) return s;
    }
    return def;
}

static int SettingInt(PCWSTR name, int lo, int hi) {
    return std::clamp(Wh_GetIntSetting(name), lo, hi);
}

static bool SettingBool(PCWSTR name) {
    return Wh_GetIntSetting(name) != 0;
}

// Windhawk writes the default of every setting declared in the settings block
// (also for keys added by an update), so values are read as they are: false,
// 0 and empty strings are real choices.
void LoadConfig() {
    Config c;

    c.language = SettingChoice(L"general.language", {L"auto", L"en", L"uk", L"pl", L"de", L"fr", L"es", L"ru"}, L"auto");

    // Hotkeys (empty = disabled)
    c.hotkey = SettingStr(L"hotkeys.hotkey");
    c.hotkeyVolUp = SettingStr(L"hotkeys.hotkeyAppVolUp");
    c.hotkeyVolDown = SettingStr(L"hotkeys.hotkeyAppVolDown");
    c.hotkeyMute = SettingStr(L"hotkeys.hotkeyAppMute");
    c.hotkeyMasterVolUp = SettingStr(L"hotkeys.hotkeyMasterVolUp");
    c.hotkeyMasterVolDown = SettingStr(L"hotkeys.hotkeyMasterVolDown");
    c.hotkeyMasterMute = SettingStr(L"hotkeys.hotkeyMasterMute");

    // Appearance
    c.layoutMode = SettingChoice(L"appearance.layoutMode",
        {L"modern_cards", L"compact_strip", L"hifi_meters", L"fluent_flyout", L"circular_hud", L"studio_console",
         L"minimal_pill", L"cyberpunk_neon", L"retro_cassette", L"meter_bridge", L"arcade_panel",
         L"broadcast_console", L"control_center", L"glass_tiles", L"terminal_retro", L"neumorphic", L"vertical_dock"},
        L"modern_cards");
    c.theme = SettingStr(L"appearance.theme");
    if (c.theme.empty()) c.theme = L"windows11";
    c.scale = SettingInt(L"appearance.scale", 30, 300);
    c.followDpi = SettingBool(L"appearance.followDpi");
    c.width = SettingInt(L"appearance.width", 200, 1200);
    c.opacity = SettingInt(L"appearance.opacity", 20, 100);
    c.animate = SettingBool(L"appearance.animation");

    // Tray
    c.showTrayIcon = SettingBool(L"tray.showTrayIcon");
    c.trayIconStyle = SettingChoice(L"tray.trayIconStyle",
        {L"system", L"speaker", L"faders", L"wave", L"headphones", L"retro_knob", L"minimal_dot",
         L"equalizer_bars", L"soundwave_circle"}, L"system");
    c.trayPopupStyle = SettingChoice(L"tray.trayPopupStyle",
        {L"system_flyout", L"full_mixer", L"compact_fader", L"mini_island", L"radial_dial", L"vertical_dock",
         L"control_center"}, L"system_flyout");
    c.trayScrollVolume = SettingBool(L"tray.trayScrollVolume");

    // Position
    c.allowDragMove = SettingBool(L"position.allowDragMove");
    c.rememberCustomPos = SettingBool(L"position.rememberCustomPos");
    c.anchor = SettingChoice(L"position.anchor",
        {L"bottom_right", L"bottom_left", L"bottom_center", L"top_right", L"top_left", L"top_center", L"center", L"tray"},
        L"bottom_right");
    c.monitor = SettingChoice(L"position.monitor", {L"cursor", L"active", L"primary"}, L"cursor");
    c.offsetX = SettingInt(L"position.offsetX", -2000, 2000);
    c.offsetY = SettingInt(L"position.offsetY", -2000, 2000);

    // Volume OSD
    c.enableVolumeOsd = SettingBool(L"osd.enableVolumeOsd");
    c.replaceWindowsOsd = SettingBool(L"osd.replaceWindowsOsd");
    c.osdStyle = SettingChoice(L"osd.osdStyle",
        {L"capsule", L"compact_pill", L"detailed", L"ring", L"vertical_bar", L"glass_morphism",
         L"game_hud_compact", L"stealth_dot", L"pill_slider"}, L"capsule");
    c.osdTheme = SettingStr(L"osd.osdTheme");
    if (c.osdTheme.empty()) c.osdTheme = L"inherit";
    c.osdPosition = SettingChoice(L"osd.osdPosition",
        {L"top_center", L"bottom_center", L"top_right", L"top_left", L"bottom_right", L"bottom_left",
         L"left_center", L"right_center", L"center"}, L"top_center");
    c.osdMonitor = SettingChoice(L"osd.osdMonitor", {L"cursor", L"active", L"primary"}, L"cursor");
    c.osdOffsetX = SettingInt(L"osd.osdOffsetX", -2000, 2000);
    c.osdOffsetY = SettingInt(L"osd.osdOffsetY", -2000, 2000);
    c.osdWidth = SettingInt(L"osd.osdWidth", 160, 800);
    c.osdScale = SettingInt(L"osd.osdScale", 30, 300);
    c.osdOpacity = SettingInt(L"osd.osdOpacity", 20, 100);
    c.osdAllowDrag = SettingBool(L"osd.osdAllowDrag");
    c.osdRememberPos = SettingBool(L"osd.osdRememberPos");
    c.osdTimeoutMs = SettingInt(L"osd.osdTimeoutMs", 300, 10000);

    // Deafen background
    c.enableDeafenBackground = SettingBool(L"deafen.enableDeafenBackground");
    c.hotkeyDeafen = SettingStr(L"deafen.hotkeyDeafen");
    c.deafenMode = SettingChoice(L"deafen.deafenMode", {L"mute", L"duck"}, L"mute");
    c.deafenDuckPercent = SettingInt(L"deafen.deafenDuckPercent", 0, 100);
    c.deafenWhitelist = SettingStr(L"deafen.deafenWhitelist");

    // Behavior
    c.vuMeters = SettingBool(L"behavior.vuMeters");
    c.showNames = SettingBool(L"behavior.showAppNames");
    c.showAllDevices = SettingBool(L"behavior.showAllDevices");
    c.mergeApps = SettingBool(L"behavior.mergeApps");
    c.hideInactive = SettingBool(L"behavior.hideInactive");
    c.wheelStep = SettingInt(L"behavior.wheelStep", 1, 25);
    c.autoHide = SettingInt(L"behavior.autoHide", 0, 600);

    g_cfg = c;
}

static void OnFastTimer() {
    if (!g_visible || g_hiding) return;
    const DWORD now = GetTickCount();

    if (g_cfg.autoHide > 0 && !g_dragging && !g_movingWindow &&
        now - g_lastActive > (DWORD)g_cfg.autoHide * 1000) {
        HidePopup();
        return;
    }

    bool dirty = g_cfg.vuMeters && UpdatePeakValues();
    if (!g_dragging && !g_movingWindow) {
        if (now - g_lastModelPoll >= 2000) {
            // New or closed apps / devices
            g_lastModelPoll = now;
            g_lastValuesPoll = now;
            if (RefreshModel(true)) return;
            if (RefreshValues()) dirty = true;
        } else if (now - g_lastValuesPoll >= 250) {
            g_lastValuesPoll = now;
            if (RefreshValues()) dirty = true;
        }
    }
    if (dirty) Render();
}

static void BeginWindowMove() {
    HideCustomMenu();
    g_movingWindow = true;
    GetCursorPos(&g_dragStartCursor);
    g_dragStartWinPos = g_pos;
    SetCapture(g_hwnd);
    SetCursor(LoadCursorW(nullptr, IDC_SIZEALL));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static const UINT s_wmTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    if (s_wmTaskbarCreated && msg == s_wmTaskbarCreated) {
        Wh_Log(L"TaskbarCreated received, restoring tray icon");
        g_trayAdded = false;
        UpdateTrayIcon(true);
        UpdateFlyoutHook();
        return 0;
    }

    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        // The popup is never activated on purpose, but if something else
        // activates it, losing the focus closes it like a normal flyout.
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && g_visible && !g_hiding && GetTickCount() - g_showTick > 100) {
                const HWND other = (HWND)lParam;
                if (other != g_hwndMenu && other != g_hwndOsd) HidePopup();
            }
            return 0;

        case WM_KILLFOCUS:
            if (g_visible && !g_hiding && GetTickCount() - g_showTick > 100) {
                const HWND other = (HWND)wParam;
                if (other != g_hwnd && other != g_hwndMenu && other != g_hwndOsd) HidePopup();
            }
            return 0;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (g_visible && !g_hiding && !IsModifierVk((DWORD)wParam)) {
                HidePopup();
                return 0;
            }
            break;

        case WM_SETTINGCHANGE:
            if (lParam && wcscmp((LPCWSTR)lParam, L"ImmersiveColorSet") == 0) {
                g_lightTaskbar = -1;
                UpdateTrayIcon(true);
                if (g_visible) {
                    g_theme = MakeTheme(g_cfg.theme);
                    Render();
                }
            }
            break;

        case WM_QUERYENDSESSION:
            return TRUE;

        // Sign-out or shutdown: the process is then ended without the normal
        // cleanup, so Deafen is undone now, while the other apps still run.
        case WM_ENDSESSION:
            if (wParam) {
                RestoreDeafen(false);
                if (g_locksDirty) PersistLockedVolumes();
            }
            return 0;

        case WM_DISPLAYCHANGE:
            if (g_visible) HidePopup();
            RefreshTrayRect();
            break;

        case WM_APP_TRAY:
            switch (LOWORD(lParam)) {
                case WM_LBUTTONUP:
                case NIN_SELECT:
                case NIN_KEYSELECT:
                case WM_RBUTTONUP:
                case WM_CONTEXTMENU:
                    TrayToggle();
                    break;
                case WM_MBUTTONUP:
                    AdjustMasterVolume(0.0f, true);
                    break;
                case WM_MOUSEMOVE: {
                    // Keeps the rectangle used by the tray wheel up to date.
                    static DWORD s_lastRectRefresh = 0;
                    const DWORD now = GetTickCount();
                    if (now - s_lastRectRefresh > 1000) {
                        s_lastRectRefresh = now;
                        RefreshTrayRect();
                    }
                    break;
                }
            }
            return 0;

        case WM_APP_TRAY_WHEEL: {
            RECT rc{};
            const POINT pt{(short)LOWORD(lParam), (short)HIWORD(lParam)};
            if (g_cfg.trayScrollVolume && GetTrayIconRect(&rc) && PtInRect(&rc, pt)) {
                const int notches = TakeWheelNotches((short)wParam);
                if (notches) AdjustMasterVolume(notches * g_cfg.wheelStep / 100.0f, false);
            } else {
                RefreshTrayRect();
            }
            return 0;
        }

        case WM_APP_TASKBAR_WHEEL: {
            const int notches = TakeWheelNotches((short)wParam);
            if (notches) AdjustMasterVolume(notches * g_cfg.wheelStep / 100.0f, false, true);
            return 0;
        }

        case WM_APP_OSD_WHEEL:
            OnOsdWheel((short)wParam);
            return 0;

        case WM_HOTKEY: {
            const float step = g_cfg.wheelStep / 100.0f;
            switch ((int)wParam - 1) {
                case HK_MAIN:
                    if (g_visible && !g_hiding) HidePopup();
                    else ShowPopup(false);
                    break;
                case HK_APP_UP: AdjustForegroundAppVolume(step, false); break;
                case HK_APP_DOWN: AdjustForegroundAppVolume(-step, false); break;
                case HK_APP_MUTE: ToggleForegroundAppMute(); break;
                case HK_MASTER_UP: AdjustMasterVolume(step, false); break;
                case HK_MASTER_DOWN: AdjustMasterVolume(-step, false); break;
                case HK_MASTER_MUTE: AdjustMasterVolume(0.0f, true); break;
                case HK_DEAFEN: ToggleDeafenBackground(); break;
            }
            UpdateTrayIcon();
            return 0;
        }

        case WM_APP_MEDIA_KEY: {
            const float step = g_cfg.wheelStep / 100.0f;
            if (wParam == VK_VOLUME_UP) AdjustMasterVolume(step, false, true);
            else if (wParam == VK_VOLUME_DOWN) AdjustMasterVolume(-step, false, true);
            else if (wParam == VK_VOLUME_MUTE) AdjustMasterVolume(0.0f, true, true);
            return 0;
        }

        case WM_APP_VOLUME_CHANGED:
            g_volumeMsgPending = false;
            OnMasterVolumeChanged();
            UpdateTrayIcon();
            if (g_visible && !g_dragging && RefreshValues()) Render();
            return 0;

        case WM_APP_DEVICES_CHANGED:
            g_deviceMsgPending = false;
            RebindDefaultEndpoint();
            UpdateTrayIcon(true);
            RebuildLockWatch();
            if (g_visible && !g_dragging) RefreshModel(false);
            return 0;

        case WM_APP_LOCK_CHECK:
            EnforceLockedVolumes();
            return 0;

        case WM_APP_LOCK_REBUILD:
            RebuildLockWatch();
            return 0;

        case WM_APP_NEW_SESSION:
            OnNewAudioSession((DWORD)wParam, LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_APP_RELOAD: {
            LoadConfig();
            if (g_deafenActive && !g_cfg.enableDeafenBackground) RestoreDeafen(false);
            RegisterAllHotkeys();
            ApplyOsdWindowStyle();
            UpdateTrayIcon(true);
            UpdateHooks();
            g_theme = MakeTheme(g_cfg.theme);
            if (g_visible) ShowPopup(g_openedFromTray);
            if (g_cfg.osdAllowDrag) ShowOsdDragPreview();
            return 0;
        }

        case WM_APP_QUIT:
            UnregisterAllHotkeys();
            FinishHide();
            RemoveTrayIcon();
            KillTimer(hwnd, ID_TIMER_REFRESH_ONCE);
            if (g_hwndOsd) {
                DestroyWindow(g_hwndOsd);
                g_hwndOsd = nullptr;
            }
            if (g_hwndMenu) {
                DestroyWindow(g_hwndMenu);
                g_hwndMenu = nullptr;
            }
            DestroyWindow(hwnd);
            return 0;

        case WM_APP_CLOSE_MIXER:
            HideCustomMenu();
            if (g_visible && !g_hiding) HidePopup();
            return 0;

        case WM_APP_CLOSE_MENU:
            HideCustomMenu();
            return 0;

        case WM_TIMER:
            if (wParam == ID_TIMER_ANIM) {
                const DWORD elapsed = GetTickCount() - g_animStart;
                if (g_hiding) {
                    if (elapsed >= kAnimMs) FinishHide();
                    else Render();
                } else {
                    if (elapsed >= kAnimMs) KillTimer(hwnd, ID_TIMER_ANIM);
                    if (g_visible) Render();
                }
            } else if (wParam == ID_TIMER_FAST) {
                OnFastTimer();
            } else if (wParam == ID_TIMER_REFRESH_ONCE) {
                KillTimer(hwnd, ID_TIMER_REFRESH_ONCE);
                if (g_visible && !g_hiding && !g_dragging) RefreshModel(false);
            } else if (wParam == ID_TIMER_SAVE_LOCKS) {
                KillTimer(hwnd, ID_TIMER_SAVE_LOCKS);
                if (g_locksDirty) PersistLockedVolumes();
            }
            return 0;

        case WM_LBUTTONDOWN: {
            if (!g_visible || g_hiding) return 0;
            g_lastActive = GetTickCount();
            const int x = (short)LOWORD(lParam);
            const int y = (short)HIWORD(lParam);
            int zone = 0;
            const int i = HitTest(x, y, &zone);

            if (zone == 4) {
                g_trayExpanded = !g_trayExpanded;
                ApplyLayoutChange();
                return 0;
            }
            if (i >= 0 && zone == 1) {
                ToggleMute(g_items[i]);
                Render();
                UpdateTrayIcon();
                return 0;
            }
            if (i >= 0 && zone == 3) {
                POINT pt;
                GetCursorPos(&pt);
                ShowCustomRouteMenu(i, pt);
                return 0;
            }
            if (i >= 0 && zone == 2) {
                HideCustomMenu();
                g_dragging = true;
                g_dragIdx = i;
                SetCapture(hwnd);
                DragTo(x, y);
                return 0;
            }
            // Moving the window: empty area or a device header
            if (g_cfg.allowDragMove && (i < 0 || g_items[i].kind == 0 || zone == 0)) {
                BeginWindowMove();
            }
            return 0;
        }

        case WM_RBUTTONDOWN: {
            if (!g_visible || g_hiding) return 0;
            g_lastActive = GetTickCount();
            int zone = 0;
            const int i = HitTest((short)LOWORD(lParam), (short)HIWORD(lParam), &zone);
            if (i >= 0) {
                POINT pt;
                GetCursorPos(&pt);
                ShowCustomRouteMenu(i, pt);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            const int x = (short)LOWORD(lParam);
            const int y = (short)HIWORD(lParam);
            g_lastActive = GetTickCount();

            if (g_movingWindow) {
                POINT pt;
                GetCursorPos(&pt);
                g_pos.x = g_dragStartWinPos.x + (pt.x - g_dragStartCursor.x);
                g_pos.y = g_dragStartWinPos.y + (pt.y - g_dragStartCursor.y);
                SetWindowPos(hwnd, nullptr, g_pos.x, g_pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                return 0;
            }
            if (g_dragging) {
                DragTo(x, y);
                UpdateTrayIcon();
                return 0;
            }
            int zone = 0;
            int i = HitTest(x, y, &zone);
            if (i >= 0 && g_items[i].kind == 0) i = -1;
            if (i != g_hoverIdx) {
                g_hoverIdx = i;
                if (g_visible && !g_hiding) Render();
            }
            TRACKMOUSEEVENT tme{};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_hoverIdx != -1 && !g_dragging) {
                g_hoverIdx = -1;
                if (g_visible && !g_hiding) Render();
            }
            return 0;

        case WM_SETCURSOR:
            if (g_movingWindow) {
                SetCursor(LoadCursorW(nullptr, IDC_SIZEALL));
                return TRUE;
            }
            break;

        case WM_LBUTTONUP:
            if (g_movingWindow) {
                g_movingWindow = false;
                ReleaseCapture();
                if (g_cfg.rememberCustomPos) {
                    g_customPosX = g_pos.x;
                    g_customPosY = g_pos.y;
                    g_hasCustomPos = true;
                    Wh_SetIntValue(L"customPosX", g_pos.x);
                    Wh_SetIntValue(L"customPosY", g_pos.y);
                }
                return 0;
            }
            if (g_dragging) {
                g_dragging = false;
                g_dragIdx = -1;
                ReleaseCapture();
                if (g_locksDirty) PersistLockedVolumes();
                UpdateTrayIcon();
            }
            return 0;

        case WM_CAPTURECHANGED:
            if ((HWND)lParam != hwnd) {
                g_dragging = false;
                g_dragIdx = -1;
                g_movingWindow = false;
            }
            return 0;

        case WM_LBUTTONDBLCLK: {
            int zone = 0;
            const int i = HitTest((short)LOWORD(lParam), (short)HIWORD(lParam), &zone);
            if (i >= 0 && g_items[i].kind == 0) {
                SetDefaultAudioDevice(g_items[i].deviceId);
                RefreshModel(false);
                UpdateTrayIcon(true);
                return 0;
            }
            // A quick second click on a slider or an icon is a normal click.
            return WndProc(hwnd, WM_LBUTTONDOWN, wParam, lParam);
        }

        case WM_MOUSEWHEEL: {
            if (!g_visible || g_hiding) return 0;
            g_lastActive = GetTickCount();
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            POINT pt{(short)LOWORD(lParam), (short)HIWORD(lParam)};
            ScreenToClient(hwnd, &pt);
            int zone = 0;
            const int i = HitTest(pt.x, pt.y, &zone);
            if (i >= 0 && g_items[i].kind != 0) {
                const float step = g_cfg.wheelStep / 100.0f;
                Item& it = g_items[i];
                SetItemVolume(it, it.vol + (delta > 0 ? step : -step));
                if (it.mute && it.vol > 0.0f && delta > 0) SetItemMute(it, false);
                Render();
                UpdateTrayIcon();
            }
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI UiThreadProc(LPVOID) {
    const HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    // The audio service delivers new-session notifications (used by the volume
    // locks and the Deafen restore) only to processes that have an MTA.
    // Resolved at run time: the 32-bit MinGW import library lacks these.
    using CoMtaIncFn = HRESULT(WINAPI*)(CO_MTA_USAGE_COOKIE*);
    using CoMtaDecFn = HRESULT(WINAPI*)(CO_MTA_USAGE_COOKIE);
    HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
    auto coMtaInc = ole32 ? (CoMtaIncFn)GetProcAddress(ole32, "CoIncrementMTAUsage") : nullptr;
    auto coMtaDec = ole32 ? (CoMtaDecFn)GetProcAddress(ole32, "CoDecrementMTAUsage") : nullptr;
    CO_MTA_USAGE_COOKIE mtaCookie = nullptr;
    const bool mtaOk = coMtaInc && coMtaDec && SUCCEEDED(coMtaInc(&mtaCookie));
    GdiplusStartupInput gsi;
    const bool gdiOk = (GdiplusStartup(&g_gdiplusToken, &gsi, nullptr) == Ok);

    // Window classes belong to this mod's DLL, not to the host executable, so
    // a reloaded mod never reuses a class whose window procedure was unloaded.
    static const int s_moduleAnchor = 0;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)&s_moduleAnchor, &g_hInst)) {
        g_hInst = GetModuleHandleW(nullptr);
    }

    if (gdiOk) {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = kClass;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.style = CS_DBLCLKS;
        RegisterClassExW(&wc);

        WNDCLASSEXW wcOsd{};
        wcOsd.cbSize = sizeof(wcOsd);
        wcOsd.lpfnWndProc = OsdWndProc;
        wcOsd.hInstance = g_hInst;
        wcOsd.lpszClassName = kOsdClass;
        wcOsd.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        RegisterClassExW(&wcOsd);

        WNDCLASSEXW wcMenu{};
        wcMenu.cbSize = sizeof(wcMenu);
        wcMenu.lpfnWndProc = MenuWndProc;
        wcMenu.hInstance = g_hInst;
        wcMenu.lpszClassName = kMenuClass;
        wcMenu.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        RegisterClassExW(&wcMenu);

        g_hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                                 kClass, L"VolumeMixerFlyoutPro", WS_POPUP,
                                 0, 0, g_winW, g_winH, nullptr, nullptr, g_hInst, nullptr);
        g_hwndOsd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                                    kOsdClass, L"VolumeMixerOsdPro", WS_POPUP,
                                    0, 0, 240, 64, nullptr, nullptr, g_hInst, nullptr);
        g_hwndMenu = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                                     kMenuClass, L"VolumeMixerMenuPro", WS_POPUP,
                                     0, 0, 200, 200, nullptr, nullptr, g_hInst, nullptr);
    }

    if (g_hwnd) {
        LoadConfig();
        g_theme = MakeTheme(g_cfg.theme);

        g_customPosX = Wh_GetIntValue(L"customPosX", -9999);
        g_customPosY = Wh_GetIntValue(L"customPosY", -9999);
        g_hasCustomPos = (g_customPosX != -9999 && g_customPosY != -9999);
        g_customOsdX = Wh_GetIntValue(L"osdCustomX", 0);
        g_customOsdY = Wh_GetIntValue(L"osdCustomY", 0);
        g_hasCustomOsdPos = (Wh_GetIntValue(L"hasCustomOsdPos", 0) != 0);
        LoadLockedVolumes();

        ApplyOsdWindowStyle();
        StartAudioNotifications();
        StartHookThread();
        RegisterAllHotkeys();
        UpdateTrayIcon(true);
        UpdateHooks();
        RecoverDeafenState();
        RebuildLockWatch();
    }

    SetEvent(g_readyEvent);

    if (g_hwnd) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    g_hwnd = nullptr;

    StopFlyoutHook();
    StopHookThread();
    if (g_hwndOsd) DestroyWindow(g_hwndOsd);
    if (g_hwndMenu) DestroyWindow(g_hwndMenu);
    g_hwndOsd = nullptr;
    g_hwndMenu = nullptr;

    RestoreDeafen(false);
    if (g_locksDirty) PersistLockedVolumes();
    StopLockWatch();
    if (g_lockNotifier) {
        g_lockNotifier->Release();
        g_lockNotifier = nullptr;
    }
    StopAudioNotifications();

    ReleaseItems(g_items);
    g_items.clear();
    for (auto& p : g_appCache) {
        delete p.second.icon;
    }
    g_appCache.clear();

    g_mainSurface.Free();
    g_menuSurface.Free();
    g_osdSurface.Free();

    UnregisterClassW(kClass, g_hInst);
    UnregisterClassW(kOsdClass, g_hInst);
    UnregisterClassW(kMenuClass, g_hInst);

    FreeGdiCache();
    if (gdiOk) GdiplusShutdown(g_gdiplusToken);
    if (mtaOk) coMtaDec(mtaCookie);
    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

// The mod runs in its own windhawk.exe process (see the tool mod code below).
// The UI thread keeps that process alive until WhTool_ModUninit.
BOOL WhTool_ModInit() {
    Wh_Log(L"Starting (PID %lu)", GetCurrentProcessId());
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"Failed to create UI thread");
        if (g_readyEvent) CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    // Lets the other callbacks rely on g_hwnd. After a timeout the event stays
    // open, since the UI thread may still signal it.
    if (g_readyEvent && WaitForSingleObject(g_readyEvent, 10000) == WAIT_OBJECT_0) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
    Wh_Log(L"Initialized, window=%p", g_hwnd);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninitializing");
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP_QUIT, 0, 0);
    if (g_thread) {
        // The process exits right after this returns. The UI thread finishes
        // its cleanup (Deafen, locks, tray icon, hooks) first; if it hangs, the
        // process exits anyway, so the mutex can't keep a restarted mod off.
        if (WaitForSingleObject(g_thread, 10000) != WAIT_OBJECT_0) Wh_Log(L"UI thread didn't stop in time");
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    Wh_Log(L"Uninitialized");
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
