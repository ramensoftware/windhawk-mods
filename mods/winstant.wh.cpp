// ==WindhawkMod==
// @id              winstant
// @name            Winstant
// @description     Any window, instantly – right where you need it. Switch, arrange and launch from the minimize button or a hotkey
// @version         1.5
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         windhawk.exe
// @compilerOptions -ldwmapi -luser32 -lgdi32 -lgdiplus -lshcore -ladvapi32 -lshell32 -lversion -lole32 -lmsimg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Winstant

*Any window, instantly – right where you need it.*
![Winstant](https://i.imgur.com/qvqqV2I.png)

Working with several windows of the same app – Excel workbooks, Windows
Terminal sessions, Explorer folders, browser windows – usually means
juggling them from the taskbar and then dragging the one you want back into
place.


This mod adds a small switcher to the **minimize button** of every window.
Rest the mouse on it for a moment and a panel lists the **other windows of
the same app**. Pick one and it opens **exactly where the current window
is** – same position, same size, same state – even if it was maximized or
minimized somewhere else.

> Example: Edge on the left half of the screen, Excel on the right half.
> Hover Excel's minimize button, pick another workbook (which was maximized
> behind everything): it takes the right half of the screen.

The same panel is also a small **window manager**: it can list every open
window, arrange windows in layouts, move them between monitors, and save
and restore whole desktop arrangements.



## Quick start

1. Open two or more windows of the same app.
2. Rest the mouse on the **minimize** button of one of them (about half a
   second).
3. Click a window in the panel, or press its number.
![Winstant_1](https://i.imgur.com/4u7KigX.png)

Move the mouse away and the panel closes by itself.

Or press **Ctrl+Alt+Space** – or simply **tap Ctrl twice** – anywhere to
open the panel in the middle of the screen with every open window, most
recently used first: press `Enter` to
jump back to the previous window, or type a few letters to find another.

## Launcher: apps and files
![Winstant_2]https://i.imgur.com/CRkqAsu.png

In the hotkey panel, type to search: besides the open windows, the panel
suggests **installed apps** (everything in the Start menu, Store apps
included) and – if [Everything](https://www.voidtools.com/) is running –
**files and folders** from its index (most recently used first, or another
order of your choice; a last row opens the full result list in Everything).
How many apps and files to show is configurable. Press `Enter` to open the
first result,
`Ctrl+Enter` on a file to open its folder, or right-click a file to open its
location or copy its path. Both sources can be turned off, and app
suggestions can be enabled in the hover panel too.

## Telling identical windows apart

Apps like Windows Terminal often show the same title in every window. The
mod gives each window a **number and a color** that stay the same for as
long as that window is open:

- **Live preview** – in list view, hovering an entry shows a live thumbnail
  of that window, framed in its color. Grid view shows live thumbnails of
  all windows at once.
- **Window tags** – a small colored pill with the number (and custom name)
  is drawn in the title bar of every app that has two or more windows open,
  next to the caption buttons. It doesn't block clicks.
- **Custom names** – press `F2` or use the right-click menu to give a window
  a name such as "Server", "Logs" or "Build". The name shows up in the
  panel, in the preview and in the window tag.

## Only this app, or every window

By default the panel lists the windows of the current app. Set **Show** to
*All windows* to list every open window instead: the current app comes
first, then the other apps, each in its own section with its icon, ordered
by how recently you used them. Click a section header to collapse it.
Holding `Shift` while the panel appears switches to the other mode for that
time.

## Arranging windows

![Winstant_2]https://i.imgur.com/qnOL6ii.png


The bar at the bottom of the panel arranges windows on the current monitor:

| Button | Layout |
|---|---|
| Tile all | Every window in a grid (2 → side by side, 4 → 2×2, …) |
| Two / three columns | Side-by-side columns |
| 2 × 2 grid | Four quarters |
| Main + side | The current window on the left, the others stacked on the right |

The current window always takes the first place. The other places go to
the windows you **selected** (`Ctrl`+click or `Space`), or – if none is
selected – to the other windows of the same app.

Next to the layouts there are three group actions: **minimize the others**,
**bring the windows to this monitor**, **close the others**. They work on
the selected windows, or on the other windows of the same app.

Each entry also has a **place beside** button (two panes, the filled one is
where the window will go): the picked window gets the same size as the
current one, next to it, and the pair is centered on the monitor. A
maximized or very wide window splits the screen in two halves instead.

## Right-click menu

Right-click an entry for: open here / switch to, place beside, **move to
another monitor**, **always on top**, **opacity**, select, rename,
minimize, close.

## Saved layouts

The bookmark button at the right of the bottom bar saves the current
arrangement of all your windows (position, size, maximized/minimized state)
under a name, and restores it later with one click. Windows are matched by
program, kind of window and title. Programs that aren't running are
skipped – the mod never starts programs by itself.

## Keyboard and mouse reference

| Action | Mouse | Keyboard |
|---|---|---|
| Open the window here / switch to it | Click | `1`–`9`, or arrows + `Enter` |
| Most recently used windows | – | `Tab` / `Shift+Tab` |
| Place it beside the current window | Two-panes button | `Ctrl+Enter` |
| Select windows for layouts and actions | `Ctrl`+click | `Space` |
| Close that window | `×` button | `Delete` |
| Rename it | Right-click menu | `F2` |
| More actions | Right-click | – |
| Search by title or name | – | Just type |
| Scroll a long list | Wheel / drag the scrollbar | `PgUp`/`PgDn`, `Home`/`End` |
| Close the panel | Move away / click elsewhere | `Esc` (first clears the search) |

## Mouse wheel on the minimize button

Scroll the wheel while the cursor is on a minimize button to cycle through
the app's windows in place, without opening the panel. A small badge near
the cursor shows which window you landed on.

## Appearance

The **Appearance** settings change the look of the panel:

- **Theme** – follow Windows, or force light or dark.
- **Background material** – solid, **Acrylic** (blurred, see-through),
  **Mica** or **Mica Alt** (tinted by the wallpaper). Needs Windows 11
  22H2 or later; elsewhere the panel stays solid. An optional tint makes the
  material more or less opaque.
- **Colors** – a custom background color and accent color (hex, e.g.
  `1E1E2E`), and the palette of the window numbers: default, pastel, vivid,
  or shades of the accent color.
- **Shape and size** – rounded, slightly rounded or square corners; border
  following the theme, in the accent color, or none; round or square number
  badges; compact, normal or comfortable rows; font and text size.

## Settings

Settings are grouped in **Panel**, **Switching**, **Arranging**, **Window
tags** and **Appearance**. Every feature can be turned on or off on its own. The hotkey is
written like `Ctrl+Alt+Space` or `Win+Shift+W` (leave it empty to disable
it). The panel texts follow the Windows display language (English, Italian,
German, French, Spanish) or can be forced; the settings, too, are translated
into these languages.

## Notes and limitations

- Works with apps whose minimize button Windows can recognize: classic
  Win32 apps, Office, Chrome/Edge, File Explorer, Windows Terminal,
  Notepad++ and most others. Apps that draw a fully custom title bar
  without reporting the button to Windows are not detected.
- Windows on other virtual desktops are not listed, and the mod doesn't
  move windows between virtual desktops (Windows has no supported way to do
  it).
- and only when you save one: for each window the program path, window class, title, position, size and state, in the mod’s local storage provided by  Windhawk (nothing is written elsewhere or sent anywhere). Delete them from the bookmark menu; uninstalling the mod removes them.
  
  
  
  
- Opacity changes are undone when the mod is disabled.
- The mod runs in its own background `windhawk.exe` process instead of
  being injected into `explorer.exe`, so it can never destabilize the shell.
- Tested on Windows 11 (100% and higher display scaling). It's designed to
  work on Windows 10 version 1607 or later too, but hasn't been tested there
  yet; on Windows 10 the panel has square corners and no colored borders.
- Apps that keep everything as tabs in a single window (e.g. Notepad++ by
  default) have nothing to switch to unless you open separate windows.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- panel:
  - hoverDelay: 500
    $name: Hover delay (ms)
    $name:it-IT: Ritardo (ms)
    $name:de-DE: Verzögerung (ms)
    $name:fr-FR: Délai de survol (ms)
    $name:es-ES: Retardo (ms)
    $description: How long to rest on the minimize button before the panel appears (100-5000)
    $description:it-IT: Quanto restare sul pulsante Riduci a icona prima che compaia il pannello (100-5000)
    $description:de-DE: Wie lange auf der Minimieren-Schaltfläche verweilen, bevor das Panel erscheint (100-5000)
    $description:fr-FR: Durée de survol du bouton Réduire avant l'apparition du panneau (100-5000)
    $description:es-ES: Cuánto tiempo mantener el ratón sobre el botón Minimizar antes de que aparezca el panel (100-5000)
  - showWindows: sameApp
    $name: Show
    $name:it-IT: Mostra
    $name:de-DE: Anzeigen
    $name:fr-FR: Afficher
    $name:es-ES: Mostrar
    $options:
    - sameApp: Windows of the current app
    - all: All windows, current app first
    $options:it-IT:
    - sameApp: Finestre dell'app corrente
    - all: Tutte le finestre, prima l'app corrente
    $options:de-DE:
    - sameApp: Fenster der aktuellen App
    - all: Alle Fenster, aktuelle App zuerst
    $options:fr-FR:
    - sameApp: Fenêtres de l'application actuelle
    - all: Toutes les fenêtres, application actuelle en premier
    $options:es-ES:
    - sameApp: Ventanas de la aplicación actual
    - all: Todas las ventanas, primero la aplicación actual
  - shiftInverts: true
    $name: Hold Shift to switch between the two modes above
    $name:it-IT: Tieni premuto Maiusc per passare all'altra modalità
    $name:de-DE: Umschalt gedrückt halten, um zwischen den beiden Modi zu wechseln
    $name:fr-FR: Maintenir Maj pour passer d'un mode à l'autre
    $name:es-ES: Mantén Mayús para cambiar entre los dos modos
  - viewMode: list
    $name: Panel style
    $name:it-IT: Stile del pannello
    $name:de-DE: Panelstil
    $name:fr-FR: Style du panneau
    $name:es-ES: Estilo del panel
    $options:
    - list: List with live preview
    - grid: Grid of live thumbnails
    $options:it-IT:
    - list: Lista con anteprima dal vivo
    - grid: Griglia di anteprime dal vivo
    $options:de-DE:
    - list: Liste mit Live-Vorschau
    - grid: Raster mit Live-Miniaturen
    $options:fr-FR:
    - list: Liste avec aperçu en direct
    - grid: Grille de miniatures en direct
    $options:es-ES:
    - list: Lista con vista previa en vivo
    - grid: Cuadrícula de miniaturas en vivo
  - showPreview: true
    $name: Live preview (list style)
    $name:it-IT: Anteprima dal vivo (stile lista)
    $name:de-DE: Live-Vorschau (Listenstil)
    $name:fr-FR: Aperçu en direct (style liste)
    $name:es-ES: Vista previa en vivo (estilo lista)
  - closeButton: true
    $name: Close button on entries
    $name:it-IT: Pulsante chiudi sulle voci
    $name:de-DE: Schließen-Schaltfläche an den Einträgen
    $name:fr-FR: Bouton de fermeture sur les éléments
    $name:es-ES: Botón de cerrar en las entradas
  - renaming: true
    $name: Allow renaming windows
    $name:it-IT: Consenti di rinominare le finestre
    $name:de-DE: Umbenennen von Fenstern erlauben
    $name:fr-FR: Autoriser le renommage des fenêtres
    $name:es-ES: Permitir renombrar ventanas
  - autoClose: true
    $name: Close the panel when the mouse moves away
    $name:it-IT: Chiudi il pannello allontanando il mouse
    $name:de-DE: Panel schließen, wenn sich die Maus entfernt
    $name:fr-FR: Fermer le panneau quand la souris s'éloigne
    $name:es-ES: Cerrar el panel al alejar el ratón
  - autoCloseDelay: 300
    $name: Auto-close delay (ms)
    $name:it-IT: Ritardo chiusura automatica (ms)
    $name:de-DE: Verzögerung beim automatischen Schließen (ms)
    $name:fr-FR: Délai de fermeture automatique (ms)
    $name:es-ES: Retardo de cierre automático (ms)
    $description: 0-3000
  - sameClassOnly: true
    $name: Only windows of the same kind
    $name:it-IT: Solo finestre dello stesso tipo
    $name:de-DE: Nur Fenster derselben Art
    $name:fr-FR: Seulement les fenêtres du même type
    $name:es-ES: Solo ventanas del mismo tipo
    $description: Skip dialogs and secondary windows of the same app
    $description:it-IT: Esclude dialoghi e finestre secondarie della stessa app
    $description:de-DE: Dialoge und Nebenfenster derselben App überspringen
    $description:fr-FR: Ignorer les boîtes de dialogue et les fenêtres secondaires de la même application
    $description:es-ES: Omitir diálogos y ventanas secundarias de la misma aplicación
  - includeMinimized: true
    $name: Include minimized windows
    $name:it-IT: Includi le finestre ridotte a icona
    $name:de-DE: Minimierte Fenster einbeziehen
    $name:fr-FR: Inclure les fenêtres réduites
    $name:es-ES: Incluir ventanas minimizadas
  - language: auto
    $name: Language
    $name:it-IT: Lingua
    $name:de-DE: Sprache
    $name:fr-FR: Langue
    $name:es-ES: Idioma
    $options:
    - auto: Windows display language
    - en: English
    - it: Italiano
    - de: Deutsch
    - fr: Français
    - es: Español
    $options:it-IT:
    - auto: Lingua di Windows
    - en: English
    - it: Italiano
    - de: Deutsch
    - fr: Français
    - es: Español
    $options:de-DE:
    - auto: Windows-Anzeigesprache
    - en: English
    - it: Italiano
    - de: Deutsch
    - fr: Français
    - es: Español
    $options:fr-FR:
    - auto: Langue d'affichage de Windows
    - en: English
    - it: Italiano
    - de: Deutsch
    - fr: Français
    - es: Español
    $options:es-ES:
    - auto: Idioma de Windows
    - en: English
    - it: Italiano
    - de: Deutsch
    - fr: Français
    - es: Español
  $name: Panel
  $name:it-IT: Pannello
  $name:de-DE: Panel
  $name:fr-FR: Panneau
  $name:es-ES: Panel
- switching:
  - originalWindowAction: none
    $name: Current window
    $name:it-IT: Finestra corrente
    $name:de-DE: Aktuelles Fenster
    $name:fr-FR: Fenêtre actuelle
    $name:es-ES: Ventana actual
    $description: What happens to the window you started from when you open another one in its place
    $description:it-IT: Cosa succede alla finestra di partenza quando ne apri un'altra al suo posto
    $description:de-DE: Was mit dem Ausgangsfenster passiert, wenn ein anderes an seiner Stelle geöffnet wird
    $description:fr-FR: Ce qui arrive à la fenêtre de départ quand une autre s'ouvre à sa place
    $description:es-ES: Qué pasa con la ventana de partida cuando se abre otra en su lugar
    $options:
    - none: Leave it under the new window
    - minimize: Minimize it
    - swap: Swap positions with the picked window
    $options:it-IT:
    - none: Resta sotto la nuova finestra
    - minimize: Riducila a icona
    - swap: Scambia posizione con quella scelta
    $options:de-DE:
    - none: Unter dem neuen Fenster lassen
    - minimize: Minimieren
    - swap: Position mit dem gewählten Fenster tauschen
    $options:fr-FR:
    - none: La laisser sous la nouvelle fenêtre
    - minimize: La réduire
    - swap: Échanger sa position avec la fenêtre choisie
    $options:es-ES:
    - none: Dejarla debajo de la nueva ventana
    - minimize: Minimizarla
    - swap: Intercambiar su posición con la ventana elegida
  - animation: true
    $name: Animated transition
    $name:it-IT: Transizione animata
    $name:de-DE: Animierter Übergang
    $name:fr-FR: Transition animée
    $name:es-ES: Transición animada
  - animationDuration: 220
    $name: Animation duration (ms)
    $name:it-IT: Durata animazione (ms)
    $name:de-DE: Animationsdauer (ms)
    $name:fr-FR: Durée de l'animation (ms)
    $name:es-ES: Duración de la animación (ms)
    $description: 80-600
  - wheelSwitch: true
    $name: Mouse wheel on the minimize button switches windows
    $name:it-IT: La rotella sul pulsante Riduci a icona cambia finestra
    $name:de-DE: Mausrad auf der Minimieren-Schaltfläche wechselt Fenster
    $name:fr-FR: La molette sur le bouton Réduire change de fenêtre
    $name:es-ES: La rueda sobre el botón Minimizar cambia de ventana
  - hotkey: Ctrl+Alt+Space
    $name: Hotkey to open the panel
    $name:it-IT: Scorciatoia per aprire il pannello
    $name:de-DE: Tastenkürzel zum Öffnen des Panels
    $name:fr-FR: Raccourci pour ouvrir le panneau
    $name:es-ES: Atajo para abrir el panel
    $description: For example Ctrl+Alt+Space or Win+Shift+W. Leave empty to disable
    $description:it-IT: Per esempio Ctrl+Alt+Space o Win+Shift+W. Lascia vuoto per disattivarla
    $description:de-DE: Zum Beispiel Ctrl+Alt+Space oder Win+Shift+W. Leer lassen zum Deaktivieren
    $description:fr-FR: Par exemple Ctrl+Alt+Space ou Win+Shift+W. Laisser vide pour désactiver
    $description:es-ES: Por ejemplo Ctrl+Alt+Space o Win+Shift+W. Déjalo vacío para desactivarlo
  - searchApps: hotkey
    $name: Suggest installed apps when searching
    $name:it-IT: Suggerisci le app installate durante la ricerca
    $name:de-DE: Installierte Apps bei der Suche vorschlagen
    $name:fr-FR: Suggérer les applications installées lors de la recherche
    $name:es-ES: Sugerir aplicaciones instaladas al buscar
    $options:
    - off: Never
    - hotkey: In the hotkey panel
    - always: In every panel
    $options:it-IT:
    - off: Mai
    - hotkey: Nel pannello aperto con la scorciatoia
    - always: In tutti i pannelli
    $options:de-DE:
    - off: Nie
    - hotkey: Im Tastenkürzel-Panel
    - always: In jedem Panel
    $options:fr-FR:
    - off: Jamais
    - hotkey: Dans le panneau du raccourci
    - always: Dans tous les panneaux
    $options:es-ES:
    - off: Nunca
    - hotkey: En el panel del atajo
    - always: En todos los paneles
  - everything: true
    $name: Search files with Everything (if it's running)
    $name:it-IT: Cerca file con Everything (se è in esecuzione)
    $name:de-DE: Dateien mit Everything suchen (falls ausgeführt)
    $name:fr-FR: Rechercher des fichiers avec Everything (s'il est lancé)
    $name:es-ES: Buscar archivos con Everything (si se está ejecutando)
    $description: Adds files and folders from voidtools Everything to the hotkey panel search
    $description:it-IT: Aggiunge file e cartelle di voidtools Everything alla ricerca del pannello aperto con la scorciatoia
    $description:de-DE: Fügt der Suche im Tastenkürzel-Panel Dateien und Ordner aus voidtools Everything hinzu
    $description:fr-FR: Ajoute les fichiers et dossiers de voidtools Everything à la recherche du panneau du raccourci
    $description:es-ES: Añade archivos y carpetas de voidtools Everything a la búsqueda del panel del atajo
  - maxApps: 8
    $name: Maximum app suggestions (3-30)
    $name:it-IT: Numero massimo di app suggerite (3-30)
    $name:de-DE: Maximale App-Vorschläge (3-30)
    $name:fr-FR: Nombre maximal d'applications suggérées (3-30)
    $name:es-ES: Máximo de aplicaciones sugeridas (3-30)
  - maxFiles: 8
    $name: Maximum Everything results (3-30)
    $name:it-IT: Numero massimo di risultati di Everything (3-30)
    $name:de-DE: Maximale Everything-Ergebnisse (3-30)
    $name:fr-FR: Nombre maximal de résultats Everything (3-30)
    $name:es-ES: Máximo de resultados de Everything (3-30)
  - fileSort: recentRun
    $name: Order of Everything results
    $name:it-IT: Ordine dei risultati di Everything
    $name:de-DE: Reihenfolge der Everything-Ergebnisse
    $name:fr-FR: Ordre des résultats Everything
    $name:es-ES: Orden de los resultados de Everything
    $description: Orders other than by name can be slower if Everything doesn't index them (Everything > Tools > Options > Indexes)
    $description:it-IT: Gli ordini diversi dal nome possono essere più lenti se Everything non li indicizza (Everything > Strumenti > Opzioni > Indici)
    $description:de-DE: Andere Reihenfolgen als nach Name können langsamer sein, wenn Everything sie nicht indiziert (Everything > Extras > Optionen > Indizes)
    $description:fr-FR: Les ordres autres que par nom peuvent être plus lents si Everything ne les indexe pas (Everything > Outils > Options > Index)
    $description:es-ES: Los órdenes distintos del nombre pueden ser más lentos si Everything no los indexa (Everything > Herramientas > Opciones > Índices)
    $options:
    - recentRun: Recently opened first
    - runCount: Most opened first
    - modified: Recently modified first
    - recentChange: Recently changed first
    - name: By name
    $options:it-IT:
    - recentRun: Aperti di recente
    - runCount: Aperti più spesso
    - modified: Modificati di recente
    - recentChange: Cambiati di recente
    - name: Per nome
    $options:de-DE:
    - recentRun: Zuletzt geöffnete zuerst
    - runCount: Am häufigsten geöffnete zuerst
    - modified: Zuletzt geänderte zuerst
    - recentChange: Zuletzt veränderte zuerst
    - name: Nach Name
    $options:fr-FR:
    - recentRun: Ouverts récemment en premier
    - runCount: Les plus ouverts en premier
    - modified: Modifiés récemment en premier
    - recentChange: Changés récemment en premier
    - name: Par nom
    $options:es-ES:
    - recentRun: Abiertos recientemente primero
    - runCount: Más abiertos primero
    - modified: Modificados recientemente primero
    - recentChange: Cambiados recientemente primero
    - name: Por nombre
  - tapKey: ctrl
    $name: Open the panel by tapping a key
    $name:it-IT: Apri il pannello premendo più volte un tasto
    $name:de-DE: Panel durch mehrfaches Antippen einer Taste öffnen
    $name:fr-FR: Ouvrir le panneau en tapotant une touche
    $name:es-ES: Abrir el panel pulsando una tecla varias veces
    $description: Quickly tap the key (without other keys) to open the hotkey panel; do it again to close it
    $description:it-IT: Premi velocemente il tasto (da solo) per aprire il pannello della scorciatoia; ripeti per chiuderlo
    $description:de-DE: Die Taste schnell (ohne andere Tasten) antippen, um das Tastenkürzel-Panel zu öffnen; erneut, um es zu schließen
    $description:fr-FR: Tapotez rapidement la touche (sans autre touche) pour ouvrir le panneau du raccourci ; recommencez pour le fermer
    $description:es-ES: Pulsa rápidamente la tecla (sin otras teclas) para abrir el panel del atajo; repite para cerrarlo
    $options:
    - off: Off
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    $options:it-IT:
    - off: Disattivato
    - ctrl: Ctrl
    - shift: Maiusc
    - alt: Alt
    $options:de-DE:
    - off: Aus
    - ctrl: Strg
    - shift: Umschalt
    - alt: Alt
    $options:fr-FR:
    - off: Désactivé
    - ctrl: Ctrl
    - shift: Maj
    - alt: Alt
    $options:es-ES:
    - off: Desactivado
    - ctrl: Ctrl
    - shift: Mayús
    - alt: Alt
  - tapCount: 2
    $name: Number of taps (2-3)
    $name:it-IT: Numero di pressioni (2-3)
    $name:de-DE: Anzahl der Tastendrücke (2-3)
    $name:fr-FR: Nombre d'appuis (2-3)
    $name:es-ES: Número de pulsaciones (2-3)
  - tapInterval: 300
    $name: Maximum time between taps (ms)
    $name:it-IT: Tempo massimo tra le pressioni (ms)
    $name:de-DE: Maximale Zeit zwischen den Tastendrücken (ms)
    $name:fr-FR: Délai maximal entre les appuis (ms)
    $name:es-ES: Tiempo máximo entre pulsaciones (ms)
    $description: 150-800
  - hotkeyAction: activate
    $name: Picking a window from the hotkey panel
    $name:it-IT: Scegliere una finestra dal pannello aperto con la scorciatoia
    $name:de-DE: Fenster im Tastenkürzel-Panel auswählen
    $name:fr-FR: Choisir une fenêtre depuis le panneau du raccourci
    $name:es-ES: Elegir una ventana desde el panel del atajo
    $options:
    - activate: Switches to it where it is
    - openHere: Opens it in place of the current window
    $options:it-IT:
    - activate: La porta in primo piano dove si trova
    - openHere: La apre al posto della finestra corrente
    $options:de-DE:
    - activate: Wechselt zu ihm, wo es ist
    - openHere: Öffnet es anstelle des aktuellen Fensters
    $options:fr-FR:
    - activate: Bascule vers elle là où elle se trouve
    - openHere: L'ouvre à la place de la fenêtre actuelle
    $options:es-ES:
    - activate: Cambia a ella donde está
    - openHere: La abre en lugar de la ventana actual
  $name: Switching
  $name:it-IT: Cambio finestra
  $name:de-DE: Fensterwechsel
  $name:fr-FR: Changement de fenêtre
  $name:es-ES: Cambio de ventana
- arranging:
  - pairButton: true
    $name: '"Place beside the current window" button'
    $name:it-IT: Pulsante "Affianca alla finestra corrente"
    $name:de-DE: 'Schaltfläche „Neben das aktuelle Fenster“'
    $name:fr-FR: 'Bouton « Placer à côté de la fenêtre actuelle »'
    $name:es-ES: 'Botón "Colocar junto a la ventana actual"'
  - layoutButtons: true
    $name: Layout buttons (tile all, columns, 2 × 2, main + side)
    $name:it-IT: Pulsanti di disposizione (affianca tutte, colonne, 2 × 2, principale + laterale)
    $name:de-DE: Layout-Schaltflächen (alle anordnen, Spalten, 2 × 2, Haupt + Seite)
    $name:fr-FR: Boutons de disposition (tout juxtaposer, colonnes, 2 × 2, principale + latérale)
    $name:es-ES: Botones de diseño (organizar todas, columnas, 2 × 2, principal + lateral)
  - groupActions: true
    $name: Group actions (minimize, bring here, close the others)
    $name:it-IT: Azioni di gruppo (riduci, porta qui, chiudi le altre)
    $name:de-DE: Gruppenaktionen (minimieren, hierher holen, die anderen schließen)
    $name:fr-FR: Actions de groupe (réduire, amener ici, fermer les autres)
    $name:es-ES: Acciones de grupo (minimizar, traer aquí, cerrar las demás)
  - savedLayouts: true
    $name: Saved layouts
    $name:it-IT: Disposizioni salvate
    $name:de-DE: Gespeicherte Anordnungen
    $name:fr-FR: Dispositions enregistrées
    $name:es-ES: Diseños guardados
  $name: Arranging
  $name:it-IT: Disposizione
  $name:de-DE: Anordnen
  $name:fr-FR: Disposition
  $name:es-ES: Organización
- tags:
  - enabled: true
    $name: Window tags
    $name:it-IT: Etichette sulle finestre
    $name:de-DE: Fenster-Etiketten
    $name:fr-FR: Étiquettes de fenêtre
    $name:es-ES: Etiquetas de ventana
    $description: Show the window number (and custom name) in the title bar of apps with two or more windows open
    $description:it-IT: Mostra il numero (e il nome personalizzato) nella barra del titolo delle app con due o più finestre aperte
    $description:de-DE: Fensternummer (und eigenen Namen) in der Titelleiste von Apps mit zwei oder mehr offenen Fenstern anzeigen
    $description:fr-FR: Afficher le numéro (et le nom personnalisé) dans la barre de titre des applications ayant au moins deux fenêtres ouvertes
    $description:es-ES: Mostrar el número (y el nombre personalizado) en la barra de título de las aplicaciones con dos o más ventanas abiertas
  - apps: [""]
    $name: Only for these programs
    $name:it-IT: Solo per questi programmi
    $name:de-DE: Nur für diese Programme
    $name:fr-FR: Seulement pour ces programmes
    $name:es-ES: Solo para estos programas
    $description: Executable names, e.g. WindowsTerminal.exe. Leave empty for all apps
    $description:it-IT: Nomi degli eseguibili, es. WindowsTerminal.exe. Lascia vuoto per tutte le app
    $description:de-DE: Namen der ausführbaren Dateien, z. B. WindowsTerminal.exe. Leer lassen für alle Apps
    $description:fr-FR: Noms des exécutables, par ex. WindowsTerminal.exe. Laisser vide pour toutes les applications
    $description:es-ES: Nombres de los ejecutables, p. ej. WindowsTerminal.exe. Déjalo vacío para todas las aplicaciones
  $name: Window tags
  $name:it-IT: Etichette
  $name:de-DE: Fenster-Etiketten
  $name:fr-FR: Étiquettes de fenêtre
  $name:es-ES: Etiquetas de ventana
- appearance:
  - theme: system
    $name: Theme
    $name:it-IT: Tema
    $name:de-DE: Design
    $name:fr-FR: Thème
    $name:es-ES: Tema
    $options:
    - system: Follow Windows
    - light: Light
    - dark: Dark
    $options:it-IT:
    - system: Segui Windows
    - light: Chiaro
    - dark: Scuro
    $options:de-DE:
    - system: Wie Windows
    - light: Hell
    - dark: Dunkel
    $options:fr-FR:
    - system: Suivre Windows
    - light: Clair
    - dark: Sombre
    $options:es-ES:
    - system: Seguir Windows
    - light: Claro
    - dark: Oscuro
  - backdrop: none
    $name: Background material (Windows 11 22H2+)
    $name:it-IT: Materiale di sfondo (Windows 11 22H2+)
    $name:de-DE: Hintergrundmaterial (Windows 11 22H2+)
    $name:fr-FR: Matériau d'arrière-plan (Windows 11 22H2+)
    $name:es-ES: Material de fondo (Windows 11 22H2+)
    $options:
    - none: Solid
    - acrylic: Acrylic
    - mica: Mica
    - micaAlt: Mica Alt
    $options:it-IT:
    - none: Tinta unita
    - acrylic: Acrilico
    - mica: Mica
    - micaAlt: Mica Alt
    $options:de-DE:
    - none: Einfarbig
    - acrylic: Acryl
    - mica: Mica
    - micaAlt: Mica Alt
    $options:fr-FR:
    - none: Uni
    - acrylic: Acrylique
    - mica: Mica
    - micaAlt: Mica Alt
    $options:es-ES:
    - none: Sólido
    - acrylic: Acrílico
    - mica: Mica
    - micaAlt: Mica Alt
  - tintOpacity: 0
    $name: Tint over the material (%)
    $name:it-IT: Tinta sopra il materiale (%)
    $name:de-DE: Tönung über dem Material (%)
    $name:fr-FR: Teinte sur le matériau (%)
    $name:es-ES: Tinte sobre el material (%)
    $description: 0 = only the material, 100 = solid background color (0-100)
    $description:it-IT: 0 = solo il materiale, 100 = colore di sfondo pieno (0-100)
    $description:de-DE: 0 = nur das Material, 100 = deckende Hintergrundfarbe (0-100)
    $description:fr-FR: 0 = matériau seul, 100 = couleur d'arrière-plan opaque (0-100)
    $description:es-ES: 0 = solo el material, 100 = color de fondo sólido (0-100)
  - backgroundColor: ""
    $name: Custom background color
    $name:it-IT: Colore di sfondo personalizzato
    $name:de-DE: Eigene Hintergrundfarbe
    $name:fr-FR: Couleur d'arrière-plan personnalisée
    $name:es-ES: Color de fondo personalizado
    $description: RRGGBB, e.g. 1E1E2E. Leave empty to follow the theme
    $description:it-IT: RRGGBB, es. 1E1E2E. Lascia vuoto per seguire il tema
    $description:de-DE: RRGGBB, z. B. 1E1E2E. Leer lassen, um dem Design zu folgen
    $description:fr-FR: RRGGBB, par ex. 1E1E2E. Laisser vide pour suivre le thème
    $description:es-ES: RRGGBB, p. ej. 1E1E2E. Déjalo vacío para seguir el tema
  - accentColor: ""
    $name: Custom accent color
    $name:it-IT: Colore di accento personalizzato
    $name:de-DE: Eigene Akzentfarbe
    $name:fr-FR: Couleur d'accentuation personnalisée
    $name:es-ES: Color de énfasis personalizado
    $description: RRGGBB, e.g. FF6B6B. Leave empty to use the Windows accent color
    $description:it-IT: RRGGBB, es. FF6B6B. Lascia vuoto per usare il colore di accento di Windows
    $description:de-DE: RRGGBB, z. B. FF6B6B. Leer lassen für die Windows-Akzentfarbe
    $description:fr-FR: RRGGBB, par ex. FF6B6B. Laisser vide pour utiliser la couleur d'accentuation de Windows
    $description:es-ES: RRGGBB, p. ej. FF6B6B. Déjalo vacío para usar el color de énfasis de Windows
  - palette: default
    $name: Window number colors
    $name:it-IT: Colori dei numeri delle finestre
    $name:de-DE: Farben der Fensternummern
    $name:fr-FR: Couleurs des numéros de fenêtre
    $name:es-ES: Colores de los números de ventana
    $options:
    - default: Default
    - pastel: Pastel
    - vivid: Vivid
    - accent: Shades of the accent color
    $options:it-IT:
    - default: Predefiniti
    - pastel: Pastello
    - vivid: Vivaci
    - accent: Sfumature del colore di accento
    $options:de-DE:
    - default: Standard
    - pastel: Pastell
    - vivid: Kräftig
    - accent: Abstufungen der Akzentfarbe
    $options:fr-FR:
    - default: Par défaut
    - pastel: Pastel
    - vivid: Vives
    - accent: Nuances de la couleur d'accentuation
    $options:es-ES:
    - default: Predeterminados
    - pastel: Pastel
    - vivid: Vivos
    - accent: Tonos del color de énfasis
  - border: theme
    $name: Border
    $name:it-IT: Bordo
    $name:de-DE: Rahmen
    $name:fr-FR: Bordure
    $name:es-ES: Borde
    $options:
    - theme: Follow the theme
    - accent: Accent color
    - none: None
    $options:it-IT:
    - theme: Segui il tema
    - accent: Colore di accento
    - none: Nessuno
    $options:de-DE:
    - theme: Wie das Design
    - accent: Akzentfarbe
    - none: Keiner
    $options:fr-FR:
    - theme: Suivre le thème
    - accent: Couleur d'accentuation
    - none: Aucune
    $options:es-ES:
    - theme: Seguir el tema
    - accent: Color de énfasis
    - none: Ninguno
  - corners: round
    $name: Corners
    $name:it-IT: Angoli
    $name:de-DE: Ecken
    $name:fr-FR: Coins
    $name:es-ES: Esquinas
    $options:
    - round: Rounded
    - small: Slightly rounded
    - square: Square
    $options:it-IT:
    - round: Arrotondati
    - small: Leggermente arrotondati
    - square: Squadrati
    $options:de-DE:
    - round: Abgerundet
    - small: Leicht abgerundet
    - square: Eckig
    $options:fr-FR:
    - round: Arrondis
    - small: Légèrement arrondis
    - square: Carrés
    $options:es-ES:
    - round: Redondeadas
    - small: Ligeramente redondeadas
    - square: Cuadradas
  - badgeShape: rounded
    $name: Number badges
    $name:it-IT: Badge dei numeri
    $name:de-DE: Nummern-Abzeichen
    $name:fr-FR: Pastilles de numéro
    $name:es-ES: Insignias de número
    $options:
    - rounded: Rounded squares
    - circle: Circles
    $options:it-IT:
    - rounded: Quadrati arrotondati
    - circle: Cerchi
    $options:de-DE:
    - rounded: Abgerundete Quadrate
    - circle: Kreise
    $options:fr-FR:
    - rounded: Carrés arrondis
    - circle: Cercles
    $options:es-ES:
    - rounded: Cuadrados redondeados
    - circle: Círculos
  - density: normal
    $name: Row height
    $name:it-IT: Altezza delle righe
    $name:de-DE: Zeilenhöhe
    $name:fr-FR: Hauteur des lignes
    $name:es-ES: Altura de las filas
    $options:
    - compact: Compact
    - normal: Normal
    - comfortable: Comfortable
    $options:it-IT:
    - compact: Compatta
    - normal: Normale
    - comfortable: Ampia
    $options:de-DE:
    - compact: Kompakt
    - normal: Normal
    - comfortable: Großzügig
    $options:fr-FR:
    - compact: Compacte
    - normal: Normale
    - comfortable: Confortable
    $options:es-ES:
    - compact: Compacta
    - normal: Normal
    - comfortable: Amplia
  - fontFamily: ""
    $name: Font
    $name:it-IT: Carattere
    $name:de-DE: Schriftart
    $name:fr-FR: Police
    $name:es-ES: Fuente
    $description: e.g. Segoe UI Variable Text. Leave empty for the system font
    $description:it-IT: es. Segoe UI Variable Text. Lascia vuoto per il carattere di sistema
    $description:de-DE: z. B. Segoe UI Variable Text. Leer lassen für die Systemschrift
    $description:fr-FR: par ex. Segoe UI Variable Text. Laisser vide pour la police système
    $description:es-ES: p. ej. Segoe UI Variable Text. Déjalo vacío para la fuente del sistema
  - fontSize: 100
    $name: Text size (%)
    $name:it-IT: Dimensione del testo (%)
    $name:de-DE: Textgröße (%)
    $name:fr-FR: Taille du texte (%)
    $name:es-ES: Tamaño del texto (%)
    $description: 80-150
  $name: Appearance
  $name:it-IT: Aspetto
  $name:de-DE: Darstellung
  $name:fr-FR: Apparence
  $name:es-ES: Apariencia
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <algorithm>
using std::max;
using std::min;
#include <gdiplus.h>

#include <atomic>
#include <cmath>
#include <string>
#include <vector>

// ===========================================================================
// Settings
// ===========================================================================

enum class OriginalAction { None, Minimize, Swap };

// Panel
std::atomic<int> g_hoverDelay{500};
std::atomic<bool> g_showAll{false};
std::atomic<bool> g_shiftInverts{true};
std::atomic<bool> g_gridView{false};
std::atomic<bool> g_showPreview{true};
std::atomic<bool> g_closeButton{true};
std::atomic<bool> g_renaming{true};
std::atomic<bool> g_autoClose{true};
std::atomic<int> g_autoCloseDelay{300};
std::atomic<bool> g_sameClassOnly{true};
std::atomic<bool> g_includeMinimized{true};
std::atomic<int> g_languageSetting{0};  // 0 = auto, otherwise a LANG_* id
// Switching
std::atomic<int> g_originalAction{(int)OriginalAction::None};
std::atomic<bool> g_animation{true};
std::atomic<int> g_animationDuration{220};
std::atomic<bool> g_wheelSwitch{true};
std::atomic<UINT> g_hotkeyMods{0};
std::atomic<UINT> g_hotkeyVk{0};
std::atomic<bool> g_hotkeyActivate{true};
std::atomic<int> g_tapKey{1};  // 0 off, 1 ctrl, 2 shift, 3 alt
std::atomic<int> g_tapCount{2};
std::atomic<int> g_tapInterval{300};
// Arranging
std::atomic<bool> g_pairButton{true};
std::atomic<bool> g_layoutButtons{true};
std::atomic<bool> g_groupActions{true};
std::atomic<bool> g_savedLayouts{true};
// Window tags
std::atomic<bool> g_windowTags{true};
SRWLOCK g_tagAppsLock = SRWLOCK_INIT;
std::vector<std::wstring> g_tagApps;  // lowercase exe names, empty = all

// Clamps v to [lo, hi]. If the range is empty (hi < lo), lo wins, so a
// panel taller than the work area is pinned to its top edge.
int Clamp(int v, int lo, int hi) {
    if (hi < lo) hi = lo;
    return v < lo ? lo : (v > hi ? hi : v);
}

bool GetBoolSetting(PCWSTR name) { return Wh_GetIntSetting(name) != 0; }

bool StringSettingIs(PCWSTR name, PCWSTR value) {
    PCWSTR s = Wh_GetStringSetting(name);
    bool result = s && wcscmp(s, value) == 0;
    if (s) Wh_FreeStringSetting(s);
    return result;
}

std::wstring Trim(const std::wstring& s, const wchar_t* chars = L" \t") {
    size_t a = s.find_first_not_of(chars);
    if (a == std::wstring::npos) return L"";
    return s.substr(a, s.find_last_not_of(chars) - a + 1);
}

// Parses "Ctrl+Alt+Space", "win+shift+w", or Windhawk's hotkey format
// "ctrl+alt+32" (modifiers + virtual-key code). Returns false if empty or
// not understood.
bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    *mods = 0;
    *vk = 0;
    std::wstring s = text;
    CharLowerBuffW(s.empty() ? nullptr : &s[0], (DWORD)s.size());
    size_t start = 0;
    while (start <= s.size()) {
        size_t plus = s.find(L'+', start);
        // A trailing "+" key, as in "ctrl++".
        if (plus == start && plus + 1 == s.size()) plus = std::wstring::npos;
        std::wstring tok = Trim(s.substr(start, plus == std::wstring::npos
                                                    ? std::wstring::npos
                                                    : plus - start));
        bool last = plus == std::wstring::npos;
        if (!tok.empty()) {
            if (tok == L"ctrl" || tok == L"control") {
                *mods |= MOD_CONTROL;
            } else if (tok == L"alt") {
                *mods |= MOD_ALT;
            } else if (tok == L"shift") {
                *mods |= MOD_SHIFT;
            } else if (tok == L"win" || tok == L"windows") {
                *mods |= MOD_WIN;
            } else if (last) {
                if (tok.find_first_not_of(L"0123456789") == std::wstring::npos) {
                    *vk = (UINT)_wtoi(tok.c_str());
                } else if (tok.size() == 1 && iswalnum(tok[0])) {
                    *vk = (UINT)towupper(tok[0]);
                } else if (tok[0] == L'f' && tok.size() <= 3 &&
                           tok.find_first_not_of(L"0123456789", 1) == std::wstring::npos) {
                    int n = _wtoi(tok.c_str() + 1);
                    if (n >= 1 && n <= 24) *vk = VK_F1 + n - 1;
                } else {
                    struct {
                        const wchar_t* name;
                        UINT vk;
                    } static const names[] = {
                        {L"space", VK_SPACE},     {L"tab", VK_TAB},
                        {L"enter", VK_RETURN},    {L"return", VK_RETURN},
                        {L"esc", VK_ESCAPE},      {L"escape", VK_ESCAPE},
                        {L"insert", VK_INSERT},   {L"ins", VK_INSERT},
                        {L"delete", VK_DELETE},   {L"del", VK_DELETE},
                        {L"home", VK_HOME},       {L"end", VK_END},
                        {L"pageup", VK_PRIOR},    {L"pgup", VK_PRIOR},
                        {L"pagedown", VK_NEXT},   {L"pgdn", VK_NEXT},
                        {L"up", VK_UP},           {L"down", VK_DOWN},
                        {L"left", VK_LEFT},       {L"right", VK_RIGHT},
                        {L"`", VK_OEM_3},         {L"backquote", VK_OEM_3},
                        {L"backspace", VK_BACK},  {L"pause", VK_PAUSE},
                    };
                    for (auto& n : names) {
                        if (tok == n.name) *vk = n.vk;
                    }
                }
            }
        }
        if (last) break;
        start = plus + 1;
    }
    return *vk != 0;
}

void LoadLauncherSettings();
void LoadAppearanceSettings();

void LoadSettings() {
    // Panel
    g_hoverDelay = Clamp(Wh_GetIntSetting(L"panel.hoverDelay"), 100, 5000);
    g_showAll = StringSettingIs(L"panel.showWindows", L"all");
    g_shiftInverts = GetBoolSetting(L"panel.shiftInverts");
    g_gridView = StringSettingIs(L"panel.viewMode", L"grid");
    g_showPreview = GetBoolSetting(L"panel.showPreview");
    g_closeButton = GetBoolSetting(L"panel.closeButton");
    g_renaming = GetBoolSetting(L"panel.renaming");
    g_autoClose = GetBoolSetting(L"panel.autoClose");
    g_autoCloseDelay = Clamp(Wh_GetIntSetting(L"panel.autoCloseDelay"), 0, 3000);
    g_sameClassOnly = GetBoolSetting(L"panel.sameClassOnly");
    g_includeMinimized = GetBoolSetting(L"panel.includeMinimized");

    int lang = 0;
    if (StringSettingIs(L"panel.language", L"en")) lang = LANG_ENGLISH;
    if (StringSettingIs(L"panel.language", L"it")) lang = LANG_ITALIAN;
    if (StringSettingIs(L"panel.language", L"de")) lang = LANG_GERMAN;
    if (StringSettingIs(L"panel.language", L"fr")) lang = LANG_FRENCH;
    if (StringSettingIs(L"panel.language", L"es")) lang = LANG_SPANISH;
    g_languageSetting = lang;

    // Switching
    OriginalAction action = OriginalAction::None;
    if (StringSettingIs(L"switching.originalWindowAction", L"minimize")) {
        action = OriginalAction::Minimize;
    } else if (StringSettingIs(L"switching.originalWindowAction", L"swap")) {
        action = OriginalAction::Swap;
    }
    g_originalAction = (int)action;
    g_animation = GetBoolSetting(L"switching.animation");
    g_animationDuration =
        Clamp(Wh_GetIntSetting(L"switching.animationDuration"), 80, 600);
    g_wheelSwitch = GetBoolSetting(L"switching.wheelSwitch");
    UINT mods = 0, vk = 0;
    if (PCWSTR s = Wh_GetStringSetting(L"switching.hotkey")) {
        if (*s && !ParseHotkey(s, &mods, &vk)) {
            Wh_Log(L"Hotkey '%s' not understood", s);
        }
        Wh_FreeStringSetting(s);
    }
    g_hotkeyMods = mods;
    g_hotkeyVk = vk;
    g_hotkeyActivate = !StringSettingIs(L"switching.hotkeyAction", L"openHere");
    int tapKey = 1;
    if (StringSettingIs(L"switching.tapKey", L"off")) tapKey = 0;
    if (StringSettingIs(L"switching.tapKey", L"shift")) tapKey = 2;
    if (StringSettingIs(L"switching.tapKey", L"alt")) tapKey = 3;
    g_tapKey = tapKey;
    g_tapCount = Clamp(Wh_GetIntSetting(L"switching.tapCount"), 2, 3);
    int interval = Wh_GetIntSetting(L"switching.tapInterval");
    g_tapInterval = interval ? Clamp(interval, 150, 800) : 300;
    LoadLauncherSettings();
    LoadAppearanceSettings();

    // Arranging
    g_pairButton = GetBoolSetting(L"arranging.pairButton");
    g_layoutButtons = GetBoolSetting(L"arranging.layoutButtons");
    g_groupActions = GetBoolSetting(L"arranging.groupActions");
    g_savedLayouts = GetBoolSetting(L"arranging.savedLayouts");

    // Window tags
    g_windowTags = GetBoolSetting(L"tags.enabled");
    std::vector<std::wstring> list;
    for (int i = 0;; i++) {
        PCWSTR app = Wh_GetStringSetting(L"tags.apps[%d]", i);
        if (!app) break;
        bool end = !*app;
        std::wstring item = Trim(app, L" \t\"");
        Wh_FreeStringSetting(app);
        if (end) break;
        if (item.empty()) continue;
        CharLowerBuffW(&item[0], (DWORD)item.size());
        list.push_back(item);
    }
    AcquireSRWLockExclusive(&g_tagAppsLock);
    g_tagApps = std::move(list);
    ReleaseSRWLockExclusive(&g_tagAppsLock);
}

// ===========================================================================
// Localization (panel texts follow the Windows display language)
// ===========================================================================

struct Strings {
    const wchar_t* minimizedTag;  // short tag next to a list entry
    const wchar_t* minimized;     // placeholder in grid view
    const wchar_t* noPreview;
    const wchar_t* allWindows;
    const wchar_t* windowsFmt;  // %d windows
    const wchar_t* oneOther;
    const wchar_t* otherFmt;  // %d other windows
    const wchar_t* typeToSearch;
    const wchar_t* matchFmt;  // %d of %d
    const wchar_t* noMatches;
    const wchar_t* pairRight;
    const wchar_t* pairLeft;
    // Bottom bar
    const wchar_t* layGrid;
    const wchar_t* layCols2;
    const wchar_t* layCols3;
    const wchar_t* layQuad;
    const wchar_t* layMainSide;
    const wchar_t* actMinimize;
    const wchar_t* actBring;
    const wchar_t* actClose;
    const wchar_t* selectedFmt;  // %d selected
    const wchar_t* savedLayouts;
    // Entry menu
    const wchar_t* menuOpenHere;
    const wchar_t* menuSwitchTo;
    const wchar_t* menuPair;
    const wchar_t* menuMoveTo;
    const wchar_t* monitorFmt;  // Monitor %d (%d x %d)
    const wchar_t* menuTopmost;
    const wchar_t* menuOpacity;
    const wchar_t* menuSelect;
    const wchar_t* menuDeselect;
    const wchar_t* menuRename;
    const wchar_t* menuMinimize;
    const wchar_t* menuClose;
    // Saved layouts menu
    const wchar_t* layoutsSave;
    const wchar_t* layoutsDelete;
    const wchar_t* layoutsEmpty;
    const wchar_t* layoutNameFmt;  // Layout %d
    // Launcher
    const wchar_t* installedApps;
    const wchar_t* filesEverything;
    const wchar_t* menuOpen;
    const wchar_t* menuOpenLocation;
    const wchar_t* menuCopyPath;
    const wchar_t* showAllFmt;  // Show all %d results in Everything
};

constexpr Strings kStringsEn = {
    L"minimized",
    L"Minimized",
    L"Preview unavailable",
    L"All windows",
    L"%d windows",
    L"1 other window",
    L"%d other windows",
    L" · type to search",
    L"%d of %d",
    L"No matching windows",
    L"Place to the right of this window",
    L"Place to the left of this window",
    L"Tile all",
    L"Two columns",
    L"Three columns",
    L"2 × 2 grid",
    L"Main window + side stack",
    L"Minimize the others",
    L"Bring the windows to this monitor",
    L"Close the others",
    L"%d selected",
    L"Saved layouts",
    L"Open here",
    L"Switch to",
    L"Place beside the current window",
    L"Move to monitor",
    L"Monitor %d (%d × %d)",
    L"Always on top",
    L"Opacity",
    L"Select",
    L"Deselect",
    L"Rename\tF2",
    L"Minimize",
    L"Close\tDel",
    L"Save the current layout…",
    L"Delete",
    L"No saved layouts",
    L"Layout %d",
    L"Apps",
    L"Files · Everything",
    L"Open",
    L"Open file location",
    L"Copy path",
    L"Show all %d results in Everything",
};

constexpr Strings kStringsIt = {
    L"ridotta",
    L"Ridotta a icona",
    L"Anteprima non disponibile",
    L"Tutte le finestre",
    L"%d finestre",
    L"1 altra finestra",
    L"%d altre finestre",
    L" · digita per cercare",
    L"%d di %d",
    L"Nessuna finestra corrispondente",
    L"Affianca a destra di questa finestra",
    L"Affianca a sinistra di questa finestra",
    L"Affianca tutte",
    L"Due colonne",
    L"Tre colonne",
    L"Griglia 2 × 2",
    L"Finestra principale + colonna laterale",
    L"Riduci a icona le altre",
    L"Porta le finestre su questo monitor",
    L"Chiudi le altre",
    L"%d selezionate",
    L"Disposizioni salvate",
    L"Apri qui",
    L"Passa a",
    L"Affianca alla finestra corrente",
    L"Sposta sul monitor",
    L"Monitor %d (%d × %d)",
    L"Sempre in primo piano",
    L"Opacità",
    L"Seleziona",
    L"Deseleziona",
    L"Rinomina\tF2",
    L"Riduci a icona",
    L"Chiudi\tCanc",
    L"Salva la disposizione attuale…",
    L"Elimina",
    L"Nessuna disposizione salvata",
    L"Disposizione %d",
    L"App installate",
    L"File · Everything",
    L"Apri",
    L"Apri percorso file",
    L"Copia percorso",
    L"Mostra tutti i %d risultati in Everything",
};

constexpr Strings kStringsDe = {
    L"minimiert",
    L"Minimiert",
    L"Keine Vorschau verfügbar",
    L"Alle Fenster",
    L"%d Fenster",
    L"1 weiteres Fenster",
    L"%d weitere Fenster",
    L" · tippen zum Suchen",
    L"%d von %d",
    L"Keine passenden Fenster",
    L"Rechts neben diesem Fenster platzieren",
    L"Links neben diesem Fenster platzieren",
    L"Alle anordnen",
    L"Zwei Spalten",
    L"Drei Spalten",
    L"2 × 2 Raster",
    L"Hauptfenster + Seitenleiste",
    L"Die anderen minimieren",
    L"Fenster auf diesen Monitor holen",
    L"Die anderen schließen",
    L"%d ausgewählt",
    L"Gespeicherte Anordnungen",
    L"Hier öffnen",
    L"Wechseln zu",
    L"Neben das aktuelle Fenster",
    L"Auf Monitor verschieben",
    L"Monitor %d (%d × %d)",
    L"Immer im Vordergrund",
    L"Deckkraft",
    L"Auswählen",
    L"Auswahl aufheben",
    L"Umbenennen\tF2",
    L"Minimieren",
    L"Schließen\tEntf",
    L"Aktuelle Anordnung speichern…",
    L"Löschen",
    L"Keine gespeicherten Anordnungen",
    L"Anordnung %d",
    L"Installierte Apps",
    L"Dateien · Everything",
    L"Öffnen",
    L"Dateispeicherort öffnen",
    L"Pfad kopieren",
    L"Alle %d Ergebnisse in Everything anzeigen",
};

constexpr Strings kStringsFr = {
    L"réduite",
    L"Réduite",
    L"Aperçu indisponible",
    L"Toutes les fenêtres",
    L"%d fenêtres",
    L"1 autre fenêtre",
    L"%d autres fenêtres",
    L" · tapez pour chercher",
    L"%d sur %d",
    L"Aucune fenêtre correspondante",
    L"Placer à droite de cette fenêtre",
    L"Placer à gauche de cette fenêtre",
    L"Tout juxtaposer",
    L"Deux colonnes",
    L"Trois colonnes",
    L"Grille 2 × 2",
    L"Fenêtre principale + colonne latérale",
    L"Réduire les autres",
    L"Amener les fenêtres sur cet écran",
    L"Fermer les autres",
    L"%d sélectionnées",
    L"Dispositions enregistrées",
    L"Ouvrir ici",
    L"Basculer vers",
    L"Placer à côté de la fenêtre actuelle",
    L"Déplacer vers l'écran",
    L"Écran %d (%d × %d)",
    L"Toujours au premier plan",
    L"Opacité",
    L"Sélectionner",
    L"Désélectionner",
    L"Renommer\tF2",
    L"Réduire",
    L"Fermer\tSuppr",
    L"Enregistrer la disposition actuelle…",
    L"Supprimer",
    L"Aucune disposition enregistrée",
    L"Disposition %d",
    L"Applications installées",
    L"Fichiers · Everything",
    L"Ouvrir",
    L"Ouvrir l'emplacement du fichier",
    L"Copier le chemin",
    L"Afficher les %d résultats dans Everything",
};

constexpr Strings kStringsEs = {
    L"minimizada",
    L"Minimizada",
    L"Vista previa no disponible",
    L"Todas las ventanas",
    L"%d ventanas",
    L"1 ventana más",
    L"%d ventanas más",
    L" · escribe para buscar",
    L"%d de %d",
    L"No hay ventanas coincidentes",
    L"Colocar a la derecha de esta ventana",
    L"Colocar a la izquierda de esta ventana",
    L"Organizar todas",
    L"Dos columnas",
    L"Tres columnas",
    L"Cuadrícula 2 × 2",
    L"Ventana principal + columna lateral",
    L"Minimizar las demás",
    L"Traer las ventanas a este monitor",
    L"Cerrar las demás",
    L"%d seleccionadas",
    L"Diseños guardados",
    L"Abrir aquí",
    L"Cambiar a",
    L"Colocar junto a la ventana actual",
    L"Mover al monitor",
    L"Monitor %d (%d × %d)",
    L"Siempre visible",
    L"Opacidad",
    L"Seleccionar",
    L"Deseleccionar",
    L"Cambiar nombre\tF2",
    L"Minimizar",
    L"Cerrar\tSupr",
    L"Guardar el diseño actual…",
    L"Eliminar",
    L"No hay diseños guardados",
    L"Diseño %d",
    L"Aplicaciones instaladas",
    L"Archivos · Everything",
    L"Abrir",
    L"Abrir ubicación del archivo",
    L"Copiar ruta",
    L"Mostrar los %d resultados en Everything",
};

const Strings* g_str = &kStringsEn;

// Picks the text table from the setting, or from the Windows UI language.
void SelectLanguage() {
    int lang = g_languageSetting.load();
    if (!lang) lang = PRIMARYLANGID(GetUserDefaultUILanguage());
    switch (lang) {
        case LANG_ITALIAN:
            g_str = &kStringsIt;
            break;
        case LANG_GERMAN:
            g_str = &kStringsDe;
            break;
        case LANG_FRENCH:
            g_str = &kStringsFr;
            break;
        case LANG_SPANISH:
            g_str = &kStringsEs;
            break;
        default:
            g_str = &kStringsEn;
            break;
    }
}

std::wstring Format(const wchar_t* fmt, ...) {
    wchar_t buf[256];
    va_list args;
    va_start(args, fmt);
    _vsnwprintf(buf, ARRAYSIZE(buf) - 1, fmt, args);
    va_end(args);
    buf[ARRAYSIZE(buf) - 1] = L'\0';
    return buf;
}

// ===========================================================================
// Appearance settings and color helpers
// ===========================================================================

enum class Backdrop { None, Acrylic, Mica, MicaAlt };

std::atomic<int> g_themeMode{0};  // 0 system, 1 light, 2 dark
std::atomic<int> g_backdrop{(int)Backdrop::None};
std::atomic<int> g_tintOpacity{0};
std::atomic<int> g_customBg{-1};      // COLORREF, -1 = theme
std::atomic<int> g_customAccent{-1};  // COLORREF, -1 = Windows accent
std::atomic<int> g_paletteMode{0};    // 0 default, 1 pastel, 2 vivid, 3 accent
std::atomic<int> g_borderMode{0};     // 0 theme, 1 accent, 2 none
std::atomic<int> g_cornerMode{0};     // 0 round, 1 small, 2 square
std::atomic<bool> g_badgeCircle{false};
std::atomic<int> g_density{1};  // 0 compact, 1 normal, 2 comfortable
std::atomic<int> g_fontScale{100};
SRWLOCK g_fontLock = SRWLOCK_INIT;
std::wstring g_fontFamily;

// "RRGGBB" or "#RRGGBB" -> COLORREF, or -1.
int ParseHexColor(PCWSTR text) {
    if (!text) return -1;
    std::wstring s = Trim(text, L" \t#");
    if (s.size() != 6 || s.find_first_not_of(L"0123456789abcdefABCDEF") != std::wstring::npos) {
        return -1;
    }
    unsigned long v = wcstoul(s.c_str(), nullptr, 16);
    return (int)RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

void LoadAppearanceSettings() {
    int theme = 0;
    if (StringSettingIs(L"appearance.theme", L"light")) theme = 1;
    if (StringSettingIs(L"appearance.theme", L"dark")) theme = 2;
    g_themeMode = theme;

    Backdrop backdrop = Backdrop::None;
    if (StringSettingIs(L"appearance.backdrop", L"acrylic")) backdrop = Backdrop::Acrylic;
    if (StringSettingIs(L"appearance.backdrop", L"mica")) backdrop = Backdrop::Mica;
    if (StringSettingIs(L"appearance.backdrop", L"micaAlt")) backdrop = Backdrop::MicaAlt;
    g_backdrop = (int)backdrop;
    g_tintOpacity = Clamp(Wh_GetIntSetting(L"appearance.tintOpacity"), 0, 100);

    PCWSTR bg = Wh_GetStringSetting(L"appearance.backgroundColor");
    g_customBg = ParseHexColor(bg);
    if (bg) Wh_FreeStringSetting(bg);
    PCWSTR accent = Wh_GetStringSetting(L"appearance.accentColor");
    g_customAccent = ParseHexColor(accent);
    if (accent) Wh_FreeStringSetting(accent);

    int palette = 0;
    if (StringSettingIs(L"appearance.palette", L"pastel")) palette = 1;
    if (StringSettingIs(L"appearance.palette", L"vivid")) palette = 2;
    if (StringSettingIs(L"appearance.palette", L"accent")) palette = 3;
    g_paletteMode = palette;

    int border = 0;
    if (StringSettingIs(L"appearance.border", L"accent")) border = 1;
    if (StringSettingIs(L"appearance.border", L"none")) border = 2;
    g_borderMode = border;

    int corners = 0;
    if (StringSettingIs(L"appearance.corners", L"small")) corners = 1;
    if (StringSettingIs(L"appearance.corners", L"square")) corners = 2;
    g_cornerMode = corners;

    g_badgeCircle = StringSettingIs(L"appearance.badgeShape", L"circle");

    int density = 1;
    if (StringSettingIs(L"appearance.density", L"compact")) density = 0;
    if (StringSettingIs(L"appearance.density", L"comfortable")) density = 2;
    g_density = density;

    int scale = Wh_GetIntSetting(L"appearance.fontSize");
    g_fontScale = scale ? Clamp(scale, 80, 150) : 100;

    std::wstring family;
    if (PCWSTR f = Wh_GetStringSetting(L"appearance.fontFamily")) {
        family = Trim(f);
        Wh_FreeStringSetting(f);
    }
    if (family.size() >= LF_FACESIZE) family.clear();
    AcquireSRWLockExclusive(&g_fontLock);
    g_fontFamily = family;
    ReleaseSRWLockExclusive(&g_fontLock);
}

std::wstring FontFamilySetting() {
    AcquireSRWLockShared(&g_fontLock);
    std::wstring f = g_fontFamily;
    ReleaseSRWLockShared(&g_fontLock);
    return f;
}

COLORREF MixColor(COLORREF a, COLORREF b, double t) {
    auto mix = [t](int x, int y) { return (int)std::lround(x + (y - x) * t); };
    return RGB(mix(GetRValue(a), GetRValue(b)), mix(GetGValue(a), GetGValue(b)),
               mix(GetBValue(a), GetBValue(b)));
}

int Luminance(COLORREF c) {
    return (299 * GetRValue(c) + 587 * GetGValue(c) + 114 * GetBValue(c)) / 1000;
}

COLORREF AccentColor() {
    int custom = g_customAccent.load();
    if (custom >= 0) return (COLORREF)custom;
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        return RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    }
    return RGB(0x00, 0x78, 0xD4);
}

int ListRowH() {
    int density = g_density.load();
    return density == 0 ? 34 : density == 2 ? 50 : 42;
}

// ===========================================================================
// Globals
// ===========================================================================

constexpr wchar_t kClassName[] = L"WhWinstant";
constexpr wchar_t kSwitcherClassName[] = L"WhWinstantPanel";
constexpr wchar_t kPreviewClassName[] = L"WhWinstantPreview";
constexpr wchar_t kGhostClassName[] = L"WhWinstantGhost";
constexpr wchar_t kPillClassName[] = L"WhWinstantPill";

constexpr UINT_PTR kTimerId = 1;          // hover polling (main window)
constexpr UINT_PTR kSwTimerId = 2;        // panel auto-close (panel window)
constexpr UINT_PTR kTagTimerId = 3;       // periodic tag refresh
constexpr UINT_PTR kTagQuickTimerId = 4;  // debounced tag refresh
constexpr UINT_PTR kOsdTimerId = 5;       // hide wheel OSD
constexpr UINT kTickMs = 50;

constexpr UINT WM_APP_WHEEL = WM_APP + 1;
constexpr UINT WM_APP_SETTINGS = WM_APP + 2;
constexpr UINT WM_APP_EDIT_DONE = WM_APP + 3;
constexpr UINT WM_APP_TAP = WM_APP + 4;

constexpr DWORD kDwmUseDarkMode = 20;
constexpr DWORD kDwmCornerPreference = 33;
constexpr DWORD kDwmBorderColor = 34;

HINSTANCE g_instance = nullptr;
HANDLE g_thread = nullptr;
HANDLE g_stopEvent = nullptr;
std::atomic<HWND> g_hwnd{nullptr};

HWND g_hoverWnd = nullptr;
ULONGLONG g_hoverStart = 0;
HWND g_suppressWnd = nullptr;

// Shared with the mouse-wheel hook thread.
std::atomic<HWND> g_onMinWnd{nullptr};
std::atomic<LONG> g_onMinX{0};
std::atomic<LONG> g_onMinY{0};
std::atomic<bool> g_panelOpen{false};
std::atomic<HWND> g_panelHwnd{nullptr};
std::atomic<HWND> g_panelSource{nullptr};
HWND g_pendingWheelSource = nullptr;
int g_pendingWheelDelta = 0;

// ===========================================================================
// Generic helpers
// ===========================================================================

std::wstring GetProcessPath(DWORD pid) {
    std::wstring result;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return result;
    WCHAR buf[MAX_PATH * 2];
    DWORD size = ARRAYSIZE(buf);
    if (QueryFullProcessImageNameW(process, 0, buf, &size)) {
        result.assign(buf, size);
    }
    CloseHandle(process);
    return result;
}

std::wstring GetClassNameStr(HWND hwnd) {
    WCHAR buf[256];
    int len = GetClassNameW(hwnd, buf, ARRAYSIZE(buf));
    return std::wstring(buf, len > 0 ? len : 0);
}

std::wstring GetWindowTitle(HWND hwnd) {
    int len = GetWindowTextLengthW(hwnd);
    if (len <= 0) return {};
    std::wstring s(len + 1, L'\0');
    len = GetWindowTextW(hwnd, &s[0], len + 1);
    s.resize(len > 0 ? len : 0);
    return s;
}

std::wstring ToLower(std::wstring s) {
    if (!s.empty()) CharLowerBuffW(&s[0], (DWORD)s.size());
    return s;
}

std::wstring FileNameOf(const std::wstring& path) {
    size_t p = path.find_last_of(L"\\/");
    return p == std::wstring::npos ? path : path.substr(p + 1);
}

std::wstring GetFileDescription(const std::wstring& path) {
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeW(path.c_str(), &handle);
    if (!size) return {};
    std::vector<BYTE> buf(size);
    if (!GetFileVersionInfoW(path.c_str(), 0, size, buf.data())) return {};
    struct LangCp {
        WORD lang, cp;
    }* tr = nullptr;
    UINT len = 0;
    if (!VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation", (void**)&tr, &len) ||
        len < sizeof(LangCp)) {
        return {};
    }
    WCHAR query[80];
    wsprintfW(query, L"\\StringFileInfo\\%04x%04x\\FileDescription", tr[0].lang,
              tr[0].cp);
    WCHAR* value = nullptr;
    UINT valueLen = 0;
    if (VerQueryValueW(buf.data(), query, (void**)&value, &valueLen) && valueLen > 1) {
        return std::wstring(value, wcsnlen(value, valueLen));
    }
    return {};
}

std::wstring AppDisplayName(const std::wstring& path) {
    std::wstring desc = GetFileDescription(path);
    if (!desc.empty()) return desc;
    std::wstring name = FileNameOf(path);
    size_t dot = name.find_last_of(L'.');
    return dot == std::wstring::npos ? name : name.substr(0, dot);
}

bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked;
}

// Roughly the same filter Alt+Tab uses.
bool IsSwitchableWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsCloaked(hwnd)) return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;
    if (GetWindow(hwnd, GW_OWNER) && !(exStyle & WS_EX_APPWINDOW)) return false;
    return GetWindowTextLengthW(hwnd) > 0;
}

// Brings hwnd to the foreground. Windows only lets a background process do
// this in some cases (foreground lock), so when the plain call is refused we
// briefly attach to the foreground thread's input queue, which is the
// standard workaround. The attachment is released immediately.
void ForceForeground(HWND hwnd) {
    if (SetForegroundWindow(hwnd) && GetForegroundWindow() == hwnd) return;
    HWND fg = GetForegroundWindow();
    DWORD cur = GetCurrentThreadId();
    DWORD fgThread = fg ? GetWindowThreadProcessId(fg, nullptr) : 0;
    bool attached = false;
    if (fgThread && fgThread != cur) {
        attached = AttachThreadInput(cur, fgThread, TRUE);
    }
    BringWindowToTop(hwnd);
    SetForegroundWindow(hwnd);
    if (attached) {
        AttachThreadInput(cur, fgThread, FALSE);
    }
    if (GetForegroundWindow() != hwnd) {
        Wh_Log(L"Couldn't bring %p to the foreground", hwnd);
    }
}

int ScD(int v, UINT dpi) { return MulDiv(v, (int)dpi, 96); }

UINT DpiForMonitor(HMONITOR mon) {
    UINT x = 96, y = 96;
    if (FAILED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &x, &y))) x = 96;
    return x;
}

RECT GetFrameBounds(HWND hwnd) {
    RECT r{};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &r,
                                     sizeof(r))) ||
        IsRectEmpty(&r)) {
        GetWindowRect(hwnd, &r);
    }
    return r;
}

MONITORINFO MonitorInfoFromPoint(POINT pt) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
    return mi;
}

bool PtInInflatedWindow(HWND hwnd, POINT pt, int margin) {
    RECT r;
    if (!hwnd || !IsWindowVisible(hwnd) || !GetWindowRect(hwnd, &r)) return false;
    InflateRect(&r, margin, margin);
    return PtInRect(&r, pt);
}

// ===========================================================================
// Candidate windows
// ===========================================================================

struct Candidate {
    HWND hwnd;
    std::wstring title;
    std::wstring path;
    bool minimized;
    bool sameApp;  // same program (and kind of window) as the source
};

struct EnumContext {
    HWND source;
    DWORD sourcePid;
    std::wstring sourcePath;
    std::wstring sourceClass;
    bool sameClassOnly;
    bool includeMinimized;
    bool allApps;
    std::vector<std::pair<DWORD, std::wstring>> pathCache;
    std::vector<Candidate> result;
};

std::wstring PathForPid(std::vector<std::pair<DWORD, std::wstring>>& cache, DWORD pid) {
    for (auto& e : cache) {
        if (e.first == pid) return e.second;
    }
    cache.push_back({pid, GetProcessPath(pid)});
    return cache.back().second;
}

BOOL CALLBACK EnumCandidatesProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<EnumContext*>(lParam);
    if (hwnd == ctx->source || !IsSwitchableWindow(hwnd)) return TRUE;

    bool minimized = IsIconic(hwnd);
    if (minimized && !ctx->includeMinimized) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return TRUE;
    std::wstring path = ctx->sourcePid && pid == ctx->sourcePid
                            ? ctx->sourcePath
                            : PathForPid(ctx->pathCache, pid);

    bool sameApp = ctx->source && !ctx->sourcePath.empty() &&
                   (pid == ctx->sourcePid ||
                    (!path.empty() && CompareStringOrdinal(path.c_str(), -1,
                                                           ctx->sourcePath.c_str(), -1,
                                                           TRUE) == CSTR_EQUAL));
    if (sameApp && ctx->sameClassOnly && GetClassNameStr(hwnd) != ctx->sourceClass) {
        // Same program, different kind of window (a dialog, a tool window...):
        // only listed among "all windows", in its own right.
        sameApp = false;
        if (!ctx->allApps) return TRUE;
    }
    if (!sameApp && !ctx->allApps) return TRUE;
    if (path.empty()) return TRUE;

    ctx->result.push_back({hwnd, GetWindowTitle(hwnd), path, minimized, sameApp});
    return TRUE;
}

std::vector<Candidate> CollectCandidates(HWND source, std::wstring* appPath,
                                         bool allApps) {
    EnumContext ctx{};
    ctx.source = source;
    if (source) {
        GetWindowThreadProcessId(source, &ctx.sourcePid);
        ctx.sourcePath = GetProcessPath(ctx.sourcePid);
        ctx.sourceClass = GetClassNameStr(source);
    }
    ctx.sameClassOnly = g_sameClassOnly;
    ctx.includeMinimized = g_includeMinimized;
    ctx.allApps = allApps;
    EnumWindows(EnumCandidatesProc, reinterpret_cast<LPARAM>(&ctx));
    if (appPath) *appPath = ctx.sourcePath;
    return std::move(ctx.result);
}

// ===========================================================================
// Window layout capture / apply
// ===========================================================================

enum class WinState { Normal, Maximized, Minimized };

struct WinLayout {
    WinState state;
    RECT rect;
    WINDOWPLACEMENT wp;
};

WinLayout CaptureLayout(HWND hwnd) {
    WinLayout l{};
    l.wp.length = sizeof(l.wp);
    GetWindowPlacement(hwnd, &l.wp);
    GetWindowRect(hwnd, &l.rect);
    if (IsIconic(hwnd)) {
        l.state = WinState::Minimized;
    } else if (IsZoomed(hwnd)) {
        l.state = WinState::Maximized;
    } else {
        l.state = WinState::Normal;
    }
    return l;
}

RECT ScreenToWorkspace(const RECT& rc) {
    RECT out = rc;
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST), &mi)) {
        OffsetRect(&out, -(mi.rcWork.left - mi.rcMonitor.left),
                   -(mi.rcWork.top - mi.rcMonitor.top));
    }
    return out;
}

void RestoreNoActivate(HWND hwnd, const RECT* normalRect) {
    WINDOWPLACEMENT wp{};
    wp.length = sizeof(wp);
    GetWindowPlacement(hwnd, &wp);
    wp.flags = 0;
    wp.showCmd = SW_SHOWNOACTIVATE;
    if (normalRect) wp.rcNormalPosition = ScreenToWorkspace(*normalRect);
    SetWindowPlacement(hwnd, &wp);
}

void ApplyLayout(HWND hwnd, const WinLayout& l) {
    switch (l.state) {
        case WinState::Minimized:
            ShowWindow(hwnd, SW_SHOWMINNOACTIVE);
            break;

        case WinState::Maximized: {
            HMONITOR targetMon = MonitorFromRect(&l.rect, MONITOR_DEFAULTTONEAREST);
            if (IsZoomed(hwnd) &&
                MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) == targetMon) {
                break;
            }
            WINDOWPLACEMENT wp{};
            wp.length = sizeof(wp);
            GetWindowPlacement(hwnd, &wp);
            wp.flags = 0;
            wp.showCmd = SW_SHOWNOACTIVATE;
            wp.rcNormalPosition = l.wp.rcNormalPosition;
            SetWindowPlacement(hwnd, &wp);
            ShowWindow(hwnd, SW_MAXIMIZE);
            break;
        }

        case WinState::Normal:
            if (IsIconic(hwnd) || IsZoomed(hwnd)) RestoreNoActivate(hwnd, &l.rect);
            SetWindowPos(hwnd, HWND_TOP, l.rect.left, l.rect.top,
                         l.rect.right - l.rect.left, l.rect.bottom - l.rect.top,
                         SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
            break;
    }
}

// ===========================================================================
// Instance identity: number, color and custom name
// ===========================================================================

constexpr COLORREF kPalette[] = {
    RGB(0x4C, 0x8D, 0xF6),  // blue
    RGB(0xF5, 0x9E, 0x0B),  // amber
    RGB(0x22, 0xC5, 0x5E),  // green
    RGB(0xEF, 0x44, 0x44),  // red
    RGB(0xA8, 0x55, 0xF7),  // purple
    RGB(0x14, 0xB8, 0xA6),  // teal
    RGB(0xEC, 0x48, 0x99),  // pink
    RGB(0x84, 0xCC, 0x16),  // lime
};

constexpr COLORREF kVividPalette[] = {
    RGB(0x00, 0x78, 0xD4), RGB(0xFF, 0x8C, 0x00), RGB(0x10, 0xB0, 0x3C),
    RGB(0xE8, 0x11, 0x23), RGB(0x88, 0x17, 0xD6), RGB(0x00, 0xB7, 0xC3),
    RGB(0xE3, 0x00, 0x8C), RGB(0x9A, 0xC8, 0x00),
};

COLORREF ColorForNumber(int number) {
    int i = (number - 1) % ARRAYSIZE(kPalette);
    switch (g_paletteMode.load()) {
        case 1:  // pastel
            return MixColor(kPalette[i], RGB(0xFF, 0xFF, 0xFF), 0.42);
        case 2:  // vivid
            return kVividPalette[i];
        case 3: {  // shades of the accent color
            static const double shades[] = {0, 0.35, -0.3, 0.6, -0.5, 0.18, -0.15, 0.75};
            double s = shades[i];
            return s >= 0 ? MixColor(AccentColor(), RGB(0xFF, 0xFF, 0xFF), s)
                          : MixColor(AccentColor(), RGB(0, 0, 0), -s);
        }
    }
    return kPalette[i];
}

struct Slot {
    HWND hwnd;
    std::wstring app;
    int number;
    std::wstring label;
};
std::vector<Slot> g_slots;

Slot* FindSlot(HWND hwnd) {
    for (auto& s : g_slots) {
        if (s.hwnd == hwnd) return &s;
    }
    return nullptr;
}

int GetSlotNumber(HWND hwnd, const std::wstring& app) {
    g_slots.erase(std::remove_if(g_slots.begin(), g_slots.end(),
                                 [](const Slot& s) { return !IsWindow(s.hwnd); }),
                  g_slots.end());
    if (Slot* s = FindSlot(hwnd)) return s->number;
    int n = 1;
    for (;;) {
        bool used = false;
        for (auto& s : g_slots) {
            if (s.app == app && s.number == n) {
                used = true;
                break;
            }
        }
        if (!used) break;
        n++;
    }
    g_slots.push_back({hwnd, app, n, L""});
    return n;
}

std::wstring GetLabel(HWND hwnd) {
    Slot* s = FindSlot(hwnd);
    return s ? s->label : std::wstring();
}

void SetLabel(HWND hwnd, const std::wstring& label) {
    if (Slot* s = FindSlot(hwnd)) s->label = label;
}

// ===========================================================================
// Theme, fonts and drawing helpers
// ===========================================================================

struct Theme {
    COLORREF bg, card, hover, thumbBg, border, text, subtext;
};

UINT g_menuDpi = 96;
bool g_dark = false;
HFONT g_font = nullptr;
HFONT g_fontBold = nullptr;
HFONT g_fontSmall = nullptr;
HFONT g_fontSmallBold = nullptr;

int Sc(int v) { return MulDiv(v, (int)g_menuDpi, 96); }

bool IsDarkMode() {
    int custom = g_customBg.load();
    if (custom >= 0) return Luminance((COLORREF)custom) < 140;
    if (g_themeMode == 1) return false;
    if (g_themeMode == 2) return true;
    DWORD value = 1, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return value == 0;
}

Theme GetTheme() {
    int custom = g_customBg.load();
    if (custom >= 0) {
        // Derive the whole palette from the chosen background.
        COLORREF bg = (COLORREF)custom;
        COLORREF towards = g_dark ? RGB(0xFF, 0xFF, 0xFF) : RGB(0, 0, 0);
        COLORREF text = g_dark ? RGB(0xF3, 0xF3, 0xF3) : RGB(0x1A, 0x1A, 0x1A);
        return {bg,
                MixColor(bg, towards, 0.05),
                MixColor(bg, towards, 0.12),
                MixColor(bg, RGB(0, 0, 0), g_dark ? 0.25 : 0.07),
                MixColor(bg, towards, 0.2),
                text,
                MixColor(text, bg, 0.4)};
    }
    if (g_dark) {
        return {RGB(0x2B, 0x2B, 0x2B), RGB(0x32, 0x32, 0x32), RGB(0x3D, 0x3D, 0x3D),
                RGB(0x1F, 0x1F, 0x1F), RGB(0x48, 0x48, 0x48), RGB(0xF3, 0xF3, 0xF3),
                RGB(0x9E, 0x9E, 0x9E)};
    }
    return {RGB(0xF9, 0xF9, 0xF9), RGB(0xF1, 0xF1, 0xF1), RGB(0xE4, 0xE4, 0xE4),
            RGB(0xE6, 0xE6, 0xE6), RGB(0xD6, 0xD6, 0xD6), RGB(0x1A, 0x1A, 0x1A),
            RGB(0x6B, 0x6B, 0x6B)};
}

COLORREF ContrastText(COLORREF c) {
    int lum = (299 * GetRValue(c) + 587 * GetGValue(c) + 114 * GetBValue(c)) / 1000;
    return lum > 165 ? RGB(0x1A, 0x1A, 0x1A) : RGB(0xFF, 0xFF, 0xFF);
}

void DestroyFonts() {
    for (HFONT* f : {&g_font, &g_fontBold, &g_fontSmall, &g_fontSmallBold}) {
        if (*f) DeleteObject(*f);
        *f = nullptr;
    }
}

void CreateFonts() {
    DestroyFonts();
    NONCLIENTMETRICSW ncm{};
    ncm.cbSize = sizeof(ncm);
    SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0,
                               g_menuDpi);
    LOGFONTW base = ncm.lfMenuFont;
    std::wstring family = FontFamilySetting();
    if (!family.empty()) wcsncpy_s(base.lfFaceName, family.c_str(), _TRUNCATE);
    base.lfHeight = base.lfHeight * g_fontScale.load() / 100;
    // ClearType's colored fringes look wrong over a see-through material.
    if (g_backdrop != (int)Backdrop::None) base.lfQuality = ANTIALIASED_QUALITY;
    LOGFONTW lf = base;
    g_font = CreateFontIndirectW(&lf);
    lf.lfWeight = FW_SEMIBOLD;
    g_fontBold = CreateFontIndirectW(&lf);
    lf = base;
    lf.lfHeight = lf.lfHeight * 85 / 100;
    g_fontSmall = CreateFontIndirectW(&lf);
    lf.lfWeight = FW_SEMIBOLD;
    g_fontSmallBold = CreateFontIndirectW(&lf);
}

Gdiplus::Color ToColor(COLORREF c, BYTE a = 255) {
    return Gdiplus::Color(a, GetRValue(c), GetGValue(c), GetBValue(c));
}

void AddRoundRect(Gdiplus::GraphicsPath& path, float x, float y, float w, float h,
                  float r) {
    r = std::min(r, std::min(w, h) / 2);
    if (r <= 0.5f) {
        path.AddRectangle(Gdiplus::RectF(x, y, w, h));
        return;
    }
    float d = r * 2;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

// While painting over Acrylic/Mica, cards and highlights are translucent so
// the material shows through them too.
bool g_glassPaint = false;
Theme g_glassTheme{};

BYTE GlassFillAlpha(COLORREF c, BYTE alpha) {
    if (!g_glassPaint) return alpha;
    if (c == g_glassTheme.hover) return std::min<BYTE>(alpha, 0x99);
    if (c == g_glassTheme.card) return std::min<BYTE>(alpha, 0x50);
    if (c == g_glassTheme.thumbBg) return std::min<BYTE>(alpha, 0x66);
    return alpha;
}

void FillRoundRect(Gdiplus::Graphics& g, COLORREF c, float x, float y, float w,
                   float h, float r, BYTE alpha = 255) {
    Gdiplus::GraphicsPath path;
    AddRoundRect(path, x, y, w, h, r);
    Gdiplus::SolidBrush brush(ToColor(c, GlassFillAlpha(c, alpha)));
    g.FillPath(&brush, &path);
}

void StrokeRoundRect(Gdiplus::Graphics& g, COLORREF c, float width, float x, float y,
                     float w, float h, float r) {
    Gdiplus::GraphicsPath path;
    AddRoundRect(path, x, y, w, h, r);
    Gdiplus::Pen pen(ToColor(c), width);
    g.DrawPath(&pen, &path);
}

void FillSolid(HDC dc, const RECT& rc, COLORREF c) {
    HBRUSH b = CreateSolidBrush(c);
    FillRect(dc, &rc, b);
    DeleteObject(b);
}

int TextWidth(HDC dc, HFONT font, const std::wstring& text) {
    HGDIOBJ old = SelectObject(dc, font);
    SIZE sz{};
    GetTextExtentPoint32W(dc, text.c_str(), (int)text.size(), &sz);
    SelectObject(dc, old);
    return sz.cx;
}

void DrawTextIn(HDC dc, HFONT font, COLORREF color, const std::wstring& text,
                RECT rc, UINT format) {
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    HGDIOBJ old = SelectObject(dc, font);
    DrawTextW(dc, text.c_str(), (int)text.size(), &rc,
              format | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
    SelectObject(dc, old);
}

HICON GetWindowIcon(HWND hwnd, const std::wstring& path, bool* owns) {
    *owns = false;
    for (WPARAM type : {(WPARAM)ICON_SMALL2, (WPARAM)ICON_SMALL, (WPARAM)ICON_BIG}) {
        DWORD_PTR r = 0;
        if (SendMessageTimeoutW(hwnd, WM_GETICON, type, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &r) &&
            r) {
            return (HICON)r;
        }
    }
    if (HICON h = (HICON)GetClassLongPtrW(hwnd, GCLP_HICONSM)) return h;
    if (HICON h = (HICON)GetClassLongPtrW(hwnd, GCLP_HICON)) return h;
    if (!path.empty()) {
        HICON small = nullptr;
        if (ExtractIconExW(path.c_str(), 0, nullptr, &small, 1) > 0 && small) {
            *owns = true;
            return small;
        }
    }
    return nullptr;
}

// ===========================================================================
// Transition animation (a "ghost" showing a live thumbnail of the window)
// ===========================================================================

struct Ghost {
    HWND wnd = nullptr;
    HTHUMBNAIL thumb = nullptr;
};

constexpr COLORREF kGhostKey = RGB(0xFF, 0x00, 0xFF);

LRESULT CALLBACK GhostWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ERASEBKGND: {
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillSolid((HDC)wParam, rc, kGhostKey);
            return 1;
        }
        case WM_NCHITTEST:
            return HTTRANSPARENT;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void DestroyGhost(Ghost& g) {
    if (g.thumb) DwmUnregisterThumbnail(g.thumb);
    if (g.wnd) DestroyWindow(g.wnd);
    g = Ghost{};
}

RECT LerpRect(const RECT& a, const RECT& b, double t) {
    auto lerp = [t](LONG x, LONG y) { return (LONG)std::lround(x + (y - x) * t); };
    return {lerp(a.left, b.left), lerp(a.top, b.top), lerp(a.right, b.right),
            lerp(a.bottom, b.bottom)};
}

bool AnimateGhost(HWND target, const RECT& from, const RECT& to, int durationMs,
                  Ghost* out) {
    Ghost g;
    g.wnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                                WS_EX_NOACTIVATE | WS_EX_TOPMOST,
                            kGhostClassName, L"", WS_POPUP, from.left, from.top,
                            from.right - from.left, from.bottom - from.top, nullptr,
                            nullptr, g_instance, nullptr);
    if (!g.wnd) return false;
    SetLayeredWindowAttributes(g.wnd, kGhostKey, 0, LWA_COLORKEY);
    if (FAILED(DwmRegisterThumbnail(g.wnd, target, &g.thumb))) {
        g.thumb = nullptr;
        DestroyGhost(g);
        return false;
    }
    SIZE src{};
    DwmQueryThumbnailSourceSize(g.thumb, &src);
    if (src.cx < 50 || src.cy < 50) {
        DestroyGhost(g);
        return false;
    }

    ShowWindow(g.wnd, SW_SHOWNOACTIVATE);
    ULONGLONG start = GetTickCount64();
    for (;;) {
        double t = std::min(1.0, (double)(GetTickCount64() - start) / durationMs);
        double e = 1.0 - std::pow(1.0 - t, 3.0);  // ease-out cubic
        RECT r = LerpRect(from, to, e);
        int w = std::max(1L, r.right - r.left), h = std::max(1L, r.bottom - r.top);
        SetWindowPos(g.wnd, HWND_TOPMOST, r.left, r.top, w, h,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        DWM_THUMBNAIL_PROPERTIES props{};
        props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY |
                        DWM_TNP_SOURCECLIENTAREAONLY;
        props.rcDestination = {0, 0, w, h};
        props.fVisible = TRUE;
        props.opacity = (BYTE)(190 + 65 * e);
        props.fSourceClientAreaOnly = FALSE;
        DwmUpdateThumbnailProperties(g.thumb, &props);
        DwmFlush();
        if (t >= 1.0) break;
    }
    *out = g;
    return true;
}

// ===========================================================================
// Switching and tiling
// ===========================================================================

void SwitchTo(HWND source, HWND target, int animMs) {
    WinLayout src = CaptureLayout(source);
    WinLayout tgt = CaptureLayout(target);
    auto action = (OriginalAction)g_originalAction.load();

    Ghost ghost;
    if (animMs > 0 && tgt.state != WinState::Minimized &&
        src.state != WinState::Minimized) {
        AnimateGhost(target, tgt.rect, src.rect, animMs, &ghost);
    }

    if (action == OriginalAction::Swap) ApplyLayout(source, tgt);

    ApplyLayout(target, src);
    SetWindowPos(target, HWND_TOP, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
    ForceForeground(target);

    if (action == OriginalAction::Minimize) {
        ShowWindow(source, SW_SHOWMINNOACTIVE);
        ForceForeground(target);
    }

    if (ghost.wnd) {
        DwmFlush();
        DwmFlush();
        DestroyGhost(ghost);
    }
    Wh_Log(L"Switched %p -> %p", source, target);
}

// Window rect that makes the *visible* frame of hwnd match `cell`
// (compensates for the invisible resize borders).
RECT FrameToWindowRect(HWND hwnd, const RECT& cell) {
    RECT wr, fr = GetFrameBounds(hwnd);
    GetWindowRect(hwnd, &wr);
    return {cell.left - (fr.left - wr.left), cell.top - (fr.top - wr.top),
            cell.right + (wr.right - fr.right), cell.bottom + (wr.bottom - fr.bottom)};
}

void PlaceFrame(HWND hwnd, const RECT& cell) {
    if (IsIconic(hwnd) || IsZoomed(hwnd)) RestoreNoActivate(hwnd, &cell);
    RECT r = FrameToWindowRect(hwnd, cell);
    SetWindowPos(hwnd, HWND_TOP, r.left, r.top, r.right - r.left, r.bottom - r.top,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
}

// Which side the picked window goes to: the side with more room.
bool PairGoesRight(HWND source) {
    if (IsZoomed(source)) return true;
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromWindow(source, MONITOR_DEFAULTTONEAREST), &mi);
    RECT f = GetFrameBounds(source);
    return (f.left + f.right) / 2 <= (mi.rcWork.left + mi.rcWork.right) / 2;
}

// Puts `target` next to `source` with the same size, centering the pair on
// the monitor. Maximized or too-wide sources split the screen in halves.
void PairWindows(HWND source, HWND target, bool toRight, int animMs) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromWindow(source, MONITOR_DEFAULTTONEAREST), &mi);
    RECT wa = mi.rcWork;
    int workW = wa.right - wa.left, workH = wa.bottom - wa.top;

    RECT sf = GetFrameBounds(source);
    int w = sf.right - sf.left, h = sf.bottom - sf.top, top = sf.top;
    if (IsZoomed(source) || 2 * w > workW) {
        w = workW / 2;
        h = workH;
        top = wa.top;
    }
    h = std::min(h, workH);
    top = Clamp(top, wa.top, wa.bottom - h);
    int left = wa.left + (workW - 2 * w) / 2;

    RECT leftCell{left, top, left + w, top + h};
    RECT rightCell{left + w, top, left + 2 * w, top + h};
    if (2 * w >= workW - 1) rightCell.right = wa.right;  // no rounding gap
    const RECT& srcCell = toRight ? leftCell : rightCell;
    const RECT& tgtCell = toRight ? rightCell : leftCell;

    PlaceFrame(source, srcCell);

    Ghost ghost;
    if (animMs > 0 && !IsIconic(target)) {
        RECT from;
        GetWindowRect(target, &from);
        RECT to = IsZoomed(target) ? tgtCell : FrameToWindowRect(target, tgtCell);
        AnimateGhost(target, from, to, animMs, &ghost);
    }
    PlaceFrame(target, tgtCell);
    ForceForeground(target);
    if (ghost.wnd) {
        DwmFlush();
        DwmFlush();
        DestroyGhost(ghost);
    }
    Wh_Log(L"Paired %p with %p (%s)", target, source, toRight ? L"right" : L"left");
}

void TileWindows(const std::vector<HWND>& windows, HWND activate) {
    int n = (int)windows.size();
    if (n == 0) return;
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromWindow(activate, MONITOR_DEFAULTTONEAREST), &mi);
    RECT wa = mi.rcWork;

    int cols = n <= 3 ? n : (int)std::ceil(std::sqrt((double)n));
    int rows = (n + cols - 1) / cols;
    int cellH = (wa.bottom - wa.top) / rows;

    for (int i = 0; i < n; i++) {
        HWND h = windows[i];
        int row = i / cols, col = i % cols;
        int inRow = row == rows - 1 ? n - row * cols : cols;
        int cellW = (wa.right - wa.left) / inRow;
        RECT cell{wa.left + col * cellW, wa.top + row * cellH, 0, 0};
        cell.right = col == inRow - 1 ? wa.right : cell.left + cellW;
        cell.bottom = row == rows - 1 ? wa.bottom : cell.top + cellH;

        PlaceFrame(h, cell);
    }
    ForceForeground(activate);
    Wh_Log(L"Tiled %d windows", n);
}

// ===========================================================================
// Pills: layered rounded labels used for window tags and the wheel OSD
// ===========================================================================

SIZE RenderPill(HWND wnd, int number, const std::wstring& label, COLORREF color,
                UINT dpi, BYTE alpha) {
    int h = ScD(22, dpi);
    float fontPx = (float)ScD(12, dpi);
    std::wstring familyName = FontFamilySetting();
    Gdiplus::FontFamily custom(familyName.empty() ? L"Segoe UI" : familyName.c_str());
    Gdiplus::FontFamily fallback(L"Segoe UI");
    Gdiplus::FontFamily& family =
        custom.GetLastStatus() == Gdiplus::Ok ? custom : fallback;
    Gdiplus::Font bold(&family, fontPx, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font regular(&family, fontPx, Gdiplus::FontStyleRegular,
                          Gdiplus::UnitPixel);
    Gdiplus::StringFormat sf;
    sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    sf.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

    std::wstring num = std::to_wstring(number);
    std::wstring text = label;
    if (text.size() > 40) {
        text.resize(39);
        text += L"…";
    }

    HDC screen = GetDC(nullptr);
    float numW = 0, textW = 0;
    {
        Gdiplus::Graphics mg(screen);
        Gdiplus::RectF box;
        mg.MeasureString(num.c_str(), -1, &bold, Gdiplus::PointF(0, 0), &sf, &box);
        numW = box.Width;
        if (!text.empty()) {
            mg.MeasureString(text.c_str(), -1, &regular, Gdiplus::PointF(0, 0), &sf,
                             &box);
            textW = box.Width;
        }
    }
    int padX = ScD(7, dpi);
    int w = (int)std::ceil(numW) + 2 * padX;
    if (!text.empty()) w += ScD(2, dpi) + (int)std::ceil(textW);
    w = std::max(w, h);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        if (dib) DeleteObject(dib);
        ReleaseDC(nullptr, screen);
        return {0, 0};
    }
    HDC mem = CreateCompatibleDC(screen);
    HGDIOBJ oldBmp = SelectObject(mem, dib);
    {
        Gdiplus::Bitmap bmp(w, h, w * 4, PixelFormat32bppPARGB, (BYTE*)bits);
        Gdiplus::Graphics g(&bmp);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        FillRoundRect(g, color, 0, 0, (float)w, (float)h, (float)h / 2);
        COLORREF fg = ContrastText(color);
        Gdiplus::SolidBrush textBrush(ToColor(fg));
        if (text.empty()) {
            Gdiplus::StringFormat center(&sf);
            center.SetAlignment(Gdiplus::StringAlignmentCenter);
            g.DrawString(num.c_str(), -1, &bold, Gdiplus::RectF(0, 0, (float)w, (float)h),
                         &center, &textBrush);
        } else {
            g.DrawString(num.c_str(), -1, &bold,
                         Gdiplus::RectF((float)padX, 0, numW + 2, (float)h), &sf,
                         &textBrush);
            Gdiplus::SolidBrush soft(ToColor(fg, 225));
            g.DrawString(text.c_str(), -1, &regular,
                         Gdiplus::RectF((float)padX + numW + ScD(2, dpi), 0, textW + 2,
                                        (float)h),
                         &sf, &soft);
        }
    }
    POINT srcPt{0, 0};
    SIZE size{w, h};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    UpdateLayeredWindow(wnd, screen, nullptr, &size, mem, &srcPt, 0, &bf, ULW_ALPHA);
    SelectObject(mem, oldBmp);
    DeleteDC(mem);
    DeleteObject(dib);
    ReleaseDC(nullptr, screen);
    return size;
}

LRESULT CALLBACK PillWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HWND CreatePillWindow(bool topmost) {
    return CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                               WS_EX_NOACTIVATE | (topmost ? WS_EX_TOPMOST : 0),
                           kPillClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
                           g_instance, nullptr);
}

// --- Wheel OSD -------------------------------------------------------------

HWND g_osdHwnd = nullptr;

void ShowOsd(int number, const std::wstring& text, COLORREF color, POINT pt) {
    if (!g_osdHwnd) g_osdHwnd = CreatePillWindow(true);
    if (!g_osdHwnd) return;
    UINT dpi = DpiForMonitor(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
    SIZE sz = RenderPill(g_osdHwnd, number, text, color, dpi, 245);
    MONITORINFO mi = MonitorInfoFromPoint(pt);
    int x = Clamp(pt.x - sz.cx / 2, mi.rcWork.left, mi.rcWork.right - sz.cx);
    int y = Clamp(pt.y + ScD(24, dpi), mi.rcWork.top, mi.rcWork.bottom - sz.cy);
    SetWindowPos(g_osdHwnd, HWND_TOPMOST, x, y, 0, 0,
                 SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    if (HWND main = g_hwnd.load()) SetTimer(main, kOsdTimerId, 1100, nullptr);
}

// --- Window tags -------------------------------------------------------------

struct Tag {
    HWND target;
    HWND wnd;
    int number;
    std::wstring label;
    UINT dpi;
    SIZE size;
};
std::vector<Tag> g_tags;
std::vector<HWINEVENTHOOK> g_winEventHooks;

bool ComputeTagPos(const Tag& t, POINT* out) {
    HWND h = t.target;
    if (!IsWindow(h) || !IsWindowVisible(h) || IsIconic(h) || IsCloaked(h)) {
        return false;
    }
    RECT wr;
    GetWindowRect(h, &wr);
    RECT frame = GetFrameBounds(h);

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST), &mi);
    LONG_PTR style = GetWindowLongPtrW(h, GWL_STYLE);
    if (!(style & WS_CAPTION) && EqualRect(&frame, &mi.rcMonitor)) {
        return false;  // borderless full screen (video, presentation...)
    }

    int btnLeft, top, captionH;
    RECT b{};
    if (SUCCEEDED(DwmGetWindowAttribute(h, DWMWA_CAPTION_BUTTON_BOUNDS, &b,
                                        sizeof(b))) &&
        !IsRectEmpty(&b)) {
        btnLeft = wr.left + b.left;
        top = wr.top + b.top;
        captionH = b.bottom - b.top;
    } else {
        btnLeft = frame.right - ScD(46 * 3, t.dpi);
        top = frame.top;
        captionH = ScD(32, t.dpi);
    }
    if (top < frame.top) top = frame.top;
    out->x = btnLeft - ScD(10, t.dpi) - t.size.cx;
    out->y = top + (captionH - t.size.cy) / 2;
    return out->x > frame.left + ScD(60, t.dpi);
}

void PlaceTag(Tag& t) {
    POINT p;
    if (!ComputeTagPos(t, &p)) {
        if (IsWindowVisible(t.wnd)) ShowWindow(t.wnd, SW_HIDE);
        return;
    }
    // Keep the tag directly above its window in the z-order.
    HWND above = GetWindow(t.target, GW_HWNDPREV);
    UINT flags = SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOOWNERZORDER;
    HWND after = above;
    if (above == t.wnd) {
        flags |= SWP_NOZORDER;
    } else if (!above || ((GetWindowLongPtrW(above, GWL_EXSTYLE) & WS_EX_TOPMOST) &&
                          !(GetWindowLongPtrW(t.target, GWL_EXSTYLE) & WS_EX_TOPMOST))) {
        // Target is the highest normal window: top of the non-topmost band.
        after = HWND_TOP;
    }
    SetWindowPos(t.wnd, after, p.x, p.y, 0, 0, flags);
}

void DestroyAllTags() {
    for (auto& t : g_tags) DestroyWindow(t.wnd);
    g_tags.clear();
}

struct TagEnumItem {
    HWND hwnd;
    std::wstring pathLower;
    std::wstring key;
};

struct TagEnumContext {
    std::vector<std::pair<DWORD, std::wstring>> pathCache;
    std::vector<TagEnumItem> items;
    bool sameClassOnly;
};

BOOL CALLBACK EnumTagWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<TagEnumContext*>(lParam);
    if (!IsSwitchableWindow(hwnd)) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return TRUE;
    std::wstring path = ToLower(PathForPid(ctx->pathCache, pid));
    if (path.empty()) return TRUE;
    std::wstring key = path;
    if (ctx->sameClassOnly) key += L"|" + GetClassNameStr(hwnd);
    ctx->items.push_back({hwnd, path, key});
    return TRUE;
}

void RefreshTags() {
    if (!g_windowTags) {
        DestroyAllTags();
        return;
    }

    std::vector<std::wstring> apps;
    AcquireSRWLockShared(&g_tagAppsLock);
    apps = g_tagApps;
    ReleaseSRWLockShared(&g_tagAppsLock);

    TagEnumContext ctx{};
    ctx.sameClassOnly = g_sameClassOnly;
    EnumWindows(EnumTagWindowsProc, reinterpret_cast<LPARAM>(&ctx));

    std::vector<HWND> wanted;
    std::vector<std::wstring> wantedApp;
    for (auto& it : ctx.items) {
        int count = 0;
        for (auto& other : ctx.items) {
            if (other.key == it.key) count++;
        }
        if (count < 2) continue;
        if (!apps.empty() &&
            std::find(apps.begin(), apps.end(), FileNameOf(it.pathLower)) == apps.end()) {
            continue;
        }
        wanted.push_back(it.hwnd);
        wantedApp.push_back(it.pathLower);
    }

    // Remove stale tags.
    for (size_t i = 0; i < g_tags.size();) {
        if (std::find(wanted.begin(), wanted.end(), g_tags[i].target) == wanted.end()) {
            DestroyWindow(g_tags[i].wnd);
            g_tags.erase(g_tags.begin() + i);
        } else {
            i++;
        }
    }

    for (size_t i = 0; i < wanted.size(); i++) {
        HWND h = wanted[i];
        int number = GetSlotNumber(h, wantedApp[i]);
        std::wstring label = GetLabel(h);
        UINT dpi = DpiForMonitor(MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST));

        Tag* tag = nullptr;
        for (auto& t : g_tags) {
            if (t.target == h) tag = &t;
        }
        bool render = false;
        if (!tag) {
            HWND wnd = CreatePillWindow(false);
            if (!wnd) continue;
            g_tags.push_back({h, wnd, 0, L"", 0, {}});
            tag = &g_tags.back();
            render = true;
        }
        if (render || tag->number != number || tag->label != label || tag->dpi != dpi) {
            tag->number = number;
            tag->label = label;
            tag->dpi = dpi;
            tag->size = RenderPill(tag->wnd, number, label, ColorForNumber(number), dpi, 235);
        }
        PlaceTag(*tag);
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                           LONG idChild, DWORD, DWORD) {
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        for (auto& t : g_tags) {
            if (t.target == hwnd) PlaceTag(t);
        }
        return;
    }
    if (event == EVENT_SYSTEM_FOREGROUND) {
        for (auto& t : g_tags) PlaceTag(t);
    }
    if (HWND main = g_hwnd.load()) SetTimer(main, kTagQuickTimerId, 80, nullptr);
}

void InstallWinEventHooks() {
    if (!g_winEventHooks.empty()) return;
    const DWORD ranges[][2] = {
        {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
        {EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND},
        {EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE},
        {EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE},
        {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
    };
    for (auto& r : ranges) {
        HWINEVENTHOOK h = SetWinEventHook(r[0], r[1], nullptr, WinEventProc, 0, 0,
                                          WINEVENT_OUTOFCONTEXT);
        if (h) g_winEventHooks.push_back(h);
    }
}

void UninstallWinEventHooks() {
    for (auto h : g_winEventHooks) UnhookWinEvent(h);
    g_winEventHooks.clear();
}

// ===========================================================================
// Most recently used order (updated from foreground changes)
// ===========================================================================

std::vector<HWND> g_mru;  // front = most recent
HWINEVENTHOOK g_foregroundHook = nullptr;

void TouchMru(HWND hwnd) {
    if (!hwnd) return;
    hwnd = GetAncestor(hwnd, GA_ROOT);
    if (!hwnd) return;
    auto it = std::find(g_mru.begin(), g_mru.end(), hwnd);
    if (it != g_mru.end()) g_mru.erase(it);
    g_mru.insert(g_mru.begin(), hwnd);
    if (g_mru.size() > 256) g_mru.resize(256);
}

int MruRank(HWND hwnd) {
    auto it = std::find(g_mru.begin(), g_mru.end(), hwnd);
    return it == g_mru.end() ? 100000 : (int)(it - g_mru.begin());
}

BOOL CALLBACK SeedMruProc(HWND hwnd, LPARAM) {
    if (IsSwitchableWindow(hwnd)) g_mru.push_back(hwnd);
    return TRUE;
}

void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject,
                                  LONG, DWORD, DWORD) {
    if (idObject == OBJID_WINDOW && hwnd && IsSwitchableWindow(GetAncestor(hwnd, GA_ROOT))) {
        TouchMru(hwnd);
    }
}

void StartMruTracking() {
    if (g_foregroundHook) return;
    g_mru.clear();
    EnumWindows(SeedMruProc, 0);  // z-order is a good first guess
    g_foregroundHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                        ForegroundEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
}

void StopMruTracking() {
    if (g_foregroundHook) UnhookWinEvent(g_foregroundHook);
    g_foregroundHook = nullptr;
}

// ===========================================================================
// Window management helpers
// ===========================================================================

void ActivateWindow(HWND hwnd) {
    if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
    ForceForeground(hwnd);
}

// Moves a window to another monitor, keeping its relative position and size
// within the work area (and its maximized/minimized state).
void MoveToMonitor(HWND hwnd, HMONITOR target) {
    HMONITOR current = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (current == target) return;
    MONITORINFO from{}, to{};
    from.cbSize = to.cbSize = sizeof(MONITORINFO);
    if (!GetMonitorInfoW(current, &from) || !GetMonitorInfoW(target, &to)) return;
    RECT a = from.rcWork, b = to.rcWork;
    int aw = std::max(1L, a.right - a.left), ah = std::max(1L, a.bottom - a.top);
    int bw = b.right - b.left, bh = b.bottom - b.top;
    auto map = [&](RECT r) {
        int w = std::min((int)((r.right - r.left) * (double)bw / aw), bw);
        int h = std::min((int)((r.bottom - r.top) * (double)bh / ah), bh);
        int x = b.left + (int)((r.left - a.left) * (double)bw / aw);
        int y = b.top + (int)((r.top - a.top) * (double)bh / ah);
        x = Clamp(x, b.left, b.right - w);
        y = Clamp(y, b.top, b.bottom - h);
        return RECT{x, y, x + w, y + h};
    };

    WINDOWPLACEMENT wp{};
    wp.length = sizeof(wp);
    GetWindowPlacement(hwnd, &wp);
    if (IsIconic(hwnd) || IsZoomed(hwnd)) {
        // rcNormalPosition is in workspace coordinates of the old monitor.
        RECT normal = wp.rcNormalPosition;
        OffsetRect(&normal, from.rcWork.left - from.rcMonitor.left,
                   from.rcWork.top - from.rcMonitor.top);
        RECT mapped = map(normal);
        bool zoomed = IsZoomed(hwnd);
        wp.rcNormalPosition = ScreenToWorkspace(mapped);
        wp.flags = 0;
        if (zoomed) {
            wp.showCmd = SW_SHOWNOACTIVATE;
            SetWindowPlacement(hwnd, &wp);
            ShowWindow(hwnd, SW_MAXIMIZE);
        } else {
            wp.showCmd = SW_SHOWMINNOACTIVE;
            SetWindowPlacement(hwnd, &wp);
        }
        return;
    }
    PlaceFrame(hwnd, map(GetFrameBounds(hwnd)));
}

struct MonitorList {
    std::vector<HMONITOR> monitors;
};

BOOL CALLBACK EnumMonitorsProc(HMONITOR mon, HDC, LPRECT, LPARAM lParam) {
    reinterpret_cast<MonitorList*>(lParam)->monitors.push_back(mon);
    return TRUE;
}

std::vector<HMONITOR> GetMonitors() {
    MonitorList list;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsProc,
                        reinterpret_cast<LPARAM>(&list));
    return list.monitors;
}

// --- Opacity (undone when the mod is unloaded) ------------------------------

struct OpacityChange {
    HWND hwnd;
    bool addedLayered;
};
std::vector<OpacityChange> g_opacityChanges;

int GetOpacityPercent(HWND hwnd) {
    if (!(GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED)) return 100;
    BYTE alpha = 255;
    DWORD flags = 0;
    if (!GetLayeredWindowAttributes(hwnd, nullptr, &alpha, &flags) ||
        !(flags & LWA_ALPHA)) {
        return 100;
    }
    return (alpha * 100 + 127) / 255;
}

void SetOpacityPercent(HWND hwnd, int percent) {
    OpacityChange* change = nullptr;
    for (auto& c : g_opacityChanges) {
        if (c.hwnd == hwnd) change = &c;
    }
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (percent >= 100) {
        if (!change) return;  // never touched by us
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
        if (change->addedLayered) {
            SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
        }
        g_opacityChanges.erase(g_opacityChanges.begin() + (change - g_opacityChanges.data()));
        return;
    }
    if (!change) {
        g_opacityChanges.push_back({hwnd, !(ex & WS_EX_LAYERED)});
        if (!(ex & WS_EX_LAYERED)) SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
    }
    SetLayeredWindowAttributes(hwnd, 0, (BYTE)(percent * 255 / 100), LWA_ALPHA);
}

void RestoreAllOpacity() {
    for (auto& c : g_opacityChanges) {
        if (!IsWindow(c.hwnd)) continue;
        SetLayeredWindowAttributes(c.hwnd, 0, 255, LWA_ALPHA);
        if (c.addedLayered) {
            SetWindowLongPtrW(c.hwnd, GWL_EXSTYLE,
                              GetWindowLongPtrW(c.hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
        }
    }
    g_opacityChanges.clear();
}

void ToggleTopmost(HWND hwnd) {
    bool topmost = GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;
    SetWindowPos(hwnd, topmost ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

// --- Layouts ----------------------------------------------------------------

enum class Act {
    Grid,
    Cols2,
    Cols3,
    Quad,
    MainSide,
    MinimizeOthers,
    BringHere,
    CloseOthers,
    SavedLayouts,
};

// Arranges `windows` (the first one is the "main" window) on a monitor.
void ArrangeWindows(Act layout, std::vector<HWND> windows, HMONITOR monitor) {
    if (windows.empty()) return;
    if (layout == Act::Grid) {
        TileWindows(windows, windows[0]);
        return;
    }
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(monitor, &mi);
    RECT wa = mi.rcWork;
    int W = wa.right - wa.left, H = wa.bottom - wa.top;

    size_t slots = layout == Act::Cols2 ? 2 : layout == Act::Cols3 ? 3 : 4;
    if (windows.size() > slots) windows.resize(slots);
    int n = (int)windows.size();

    std::vector<RECT> cells;
    switch (layout) {
        case Act::Cols2:
        case Act::Cols3:
            for (int i = 0; i < n; i++) {
                cells.push_back({wa.left + W * i / n, wa.top, wa.left + W * (i + 1) / n,
                                 wa.bottom});
            }
            break;
        case Act::Quad:
            for (int i = 0; i < n; i++) {
                int col = i % 2, row = i / 2;
                int rows = n > 2 ? 2 : 1;
                cells.push_back({wa.left + W * col / 2, wa.top + H * row / rows,
                                 wa.left + W * (col + 1) / 2,
                                 wa.top + H * (row + 1) / rows});
                if (n == 1) cells.back().right = wa.right;
            }
            break;
        case Act::MainSide: {
            int mainW = n == 1 ? W : W * 60 / 100;
            cells.push_back({wa.left, wa.top, wa.left + mainW, wa.bottom});
            int side = n - 1;
            for (int i = 0; i < side; i++) {
                cells.push_back({wa.left + mainW, wa.top + H * i / side, wa.right,
                                 wa.top + H * (i + 1) / side});
            }
            break;
        }
        default:
            return;
    }
    for (int i = n - 1; i >= 0; i--) PlaceFrame(windows[i], cells[i]);
    ForceForeground(windows[0]);
    Wh_Log(L"Arranged %d windows (layout %d)", n, (int)layout);
}

// --- Saved layouts -------------------------------------------------------------

struct SavedItem {
    std::wstring path;  // lowercase
    std::wstring cls;
    std::wstring title;
    int state;  // 0 normal, 1 maximized, 2 minimized
    RECT rect;  // window rect (normal state)
};

struct SavedLayout {
    std::wstring name;
    std::vector<SavedItem> items;
};

constexpr int kMaxSavedLayouts = 20;

// Values are stored as one string each; fields are '|'-separated, items
// ';'-separated, and those characters (plus '%', '=', line breaks) are
// %-escaped so that the INI storage of portable installs accepts them.
std::wstring EscapeField(const std::wstring& s) {
    std::wstring out;
    for (wchar_t c : s) {
        if (c == L'%' || c == L'|' || c == L';' || c == L'=' || c == L'\r' ||
            c == L'\n') {
            wchar_t buf[8];
            wsprintfW(buf, L"%%%02X", (unsigned)c);
            out += buf;
        } else {
            out += c;
        }
    }
    return out;
}

std::wstring UnescapeField(const std::wstring& s) {
    std::wstring out;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == L'%' && i + 2 < s.size()) {
            wchar_t hex[3] = {s[i + 1], s[i + 2], 0};
            out += (wchar_t)wcstol(hex, nullptr, 16);
            i += 2;
        } else {
            out += s[i];
        }
    }
    return out;
}

std::vector<std::wstring> Split(const std::wstring& s, wchar_t sep) {
    std::vector<std::wstring> parts;
    size_t start = 0;
    for (;;) {
        size_t p = s.find(sep, start);
        parts.push_back(s.substr(start, p == std::wstring::npos ? p : p - start));
        if (p == std::wstring::npos) break;
        start = p + 1;
    }
    return parts;
}

std::vector<SavedLayout> LoadSavedLayouts() {
    std::vector<SavedLayout> layouts;
    int count = Clamp(Wh_GetIntValue(L"layoutCount", 0), 0, kMaxSavedLayouts);
    std::vector<wchar_t> buf(1 << 17);  // 128K chars: room for ~200 windows
    for (int i = 0; i < count; i++) {
        wchar_t name[32];
        wsprintfW(name, L"layout%d", i);
        if (!Wh_GetStringValue(name, buf.data(), buf.size())) continue;
        std::vector<std::wstring> parts = Split(buf.data(), L';');
        SavedLayout layout;
        layout.name = UnescapeField(parts[0]);
        for (size_t k = 1; k < parts.size(); k++) {
            std::vector<std::wstring> f = Split(parts[k], L'|');
            if (f.size() < 8) continue;
            SavedItem item;
            item.path = UnescapeField(f[0]);
            item.cls = UnescapeField(f[1]);
            item.title = UnescapeField(f[2]);
            item.state = _wtoi(f[3].c_str());
            item.rect = {_wtoi(f[4].c_str()), _wtoi(f[5].c_str()), _wtoi(f[6].c_str()),
                         _wtoi(f[7].c_str())};
            layout.items.push_back(item);
        }
        if (!layout.name.empty()) layouts.push_back(layout);
    }
    return layouts;
}

void StoreSavedLayouts(const std::vector<SavedLayout>& layouts) {
    int oldCount = Wh_GetIntValue(L"layoutCount", 0);
    int count = std::min((int)layouts.size(), kMaxSavedLayouts);
    for (int i = 0; i < count; i++) {
        std::wstring s = EscapeField(layouts[i].name);
        for (auto& it : layouts[i].items) {
            s += L";" + EscapeField(it.path) + L"|" + EscapeField(it.cls) + L"|" +
                 EscapeField(it.title) + L"|" + std::to_wstring(it.state) + L"|" +
                 std::to_wstring(it.rect.left) + L"|" + std::to_wstring(it.rect.top) +
                 L"|" + std::to_wstring(it.rect.right) + L"|" +
                 std::to_wstring(it.rect.bottom);
        }
        wchar_t name[32];
        wsprintfW(name, L"layout%d", i);
        Wh_SetStringValue(name, s.c_str());
    }
    for (int i = count; i < oldCount; i++) {
        wchar_t name[32];
        wsprintfW(name, L"layout%d", i);
        Wh_DeleteValue(name);
    }
    Wh_SetIntValue(L"layoutCount", count);
}

struct OpenWindow {
    HWND hwnd;
    std::wstring path;
    std::wstring cls;
    std::wstring title;
    bool used = false;
};

struct OpenWindowsContext {
    std::vector<std::pair<DWORD, std::wstring>> pathCache;
    std::vector<OpenWindow> windows;
};

BOOL CALLBACK EnumOpenWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<OpenWindowsContext*>(lParam);
    if (!IsSwitchableWindow(hwnd)) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return TRUE;
    std::wstring path = ToLower(PathForPid(ctx->pathCache, pid));
    if (path.empty()) return TRUE;
    ctx->windows.push_back({hwnd, path, GetClassNameStr(hwnd), GetWindowTitle(hwnd)});
    return TRUE;
}

std::vector<OpenWindow> GetOpenWindows() {
    OpenWindowsContext ctx;
    EnumWindows(EnumOpenWindowsProc, reinterpret_cast<LPARAM>(&ctx));
    return std::move(ctx.windows);
}

void SaveCurrentLayout(const std::wstring& name) {
    SavedLayout layout;
    layout.name = name;
    std::vector<OpenWindow> open = GetOpenWindows();
    if (open.size() > 200) open.resize(200);  // keep the stored value bounded
    for (auto& w : open) {  // z-order: topmost first
        SavedItem item{w.path, w.cls, w.title, 0, {}};
        WINDOWPLACEMENT wp{};
        wp.length = sizeof(wp);
        GetWindowPlacement(w.hwnd, &wp);
        if (IsIconic(w.hwnd)) {
            item.state = 2;
        } else if (IsZoomed(w.hwnd)) {
            item.state = 1;
        }
        if (item.state == 0) {
            GetWindowRect(w.hwnd, &item.rect);
        } else {
            // Normal position, converted from workspace to screen coordinates.
            RECT r = wp.rcNormalPosition;
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);
            GetMonitorInfoW(MonitorFromRect(&r, MONITOR_DEFAULTTONEAREST), &mi);
            OffsetRect(&r, mi.rcWork.left - mi.rcMonitor.left,
                       mi.rcWork.top - mi.rcMonitor.top);
            item.rect = r;
        }
        layout.items.push_back(item);
    }
    std::vector<SavedLayout> layouts = LoadSavedLayouts();
    bool replaced = false;
    for (auto& l : layouts) {
        if (l.name == name) {
            l = layout;
            replaced = true;
        }
    }
    if (!replaced) {
        if ((int)layouts.size() >= kMaxSavedLayouts) layouts.erase(layouts.begin());
        layouts.push_back(layout);
    }
    StoreSavedLayouts(layouts);
    Wh_Log(L"Saved layout '%s' (%d windows)", name.c_str(), (int)layout.items.size());
}

void RestoreSavedLayout(const SavedLayout& layout) {
    std::vector<OpenWindow> open = GetOpenWindows();
    std::vector<HWND> placed;
    // Exact title matches first, then any window of the same program/kind.
    std::vector<int> match(layout.items.size(), -1);
    for (int pass = 0; pass < 2; pass++) {
        for (size_t i = 0; i < layout.items.size(); i++) {
            if (match[i] >= 0) continue;
            const SavedItem& it = layout.items[i];
            for (size_t k = 0; k < open.size(); k++) {
                OpenWindow& w = open[k];
                if (w.used || w.path != it.path || w.cls != it.cls) continue;
                if (pass == 0 && w.title != it.title) continue;
                w.used = true;
                match[i] = (int)k;
                break;
            }
        }
    }
    // Apply bottom-up, so the first saved window ends up on top.
    for (int i = (int)layout.items.size() - 1; i >= 0; i--) {
        if (match[i] < 0) continue;
        HWND h = open[match[i]].hwnd;
        const SavedItem& it = layout.items[i];
        if (IsIconic(h) || IsZoomed(h)) RestoreNoActivate(h, &it.rect);
        SetWindowPos(h, HWND_TOP, it.rect.left, it.rect.top, it.rect.right - it.rect.left,
                     it.rect.bottom - it.rect.top,
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
        if (it.state == 1) ShowWindow(h, SW_MAXIMIZE);
        if (it.state == 2) ShowWindow(h, SW_SHOWMINNOACTIVE);
        placed.push_back(h);
    }
    for (int i = 0; i < (int)layout.items.size(); i++) {
        if (match[i] >= 0 && layout.items[i].state != 2) {
            ForceForeground(open[match[i]].hwnd);
            break;
        }
    }
    Wh_Log(L"Restored layout '%s': %d of %d windows", layout.name.c_str(),
           (int)placed.size(), (int)layout.items.size());
}

// ===========================================================================
// Launcher: installed apps (Start menu "Apps" folder) and files (Everything)
// ===========================================================================

// Defined here so the mod doesn't depend on libuuid.
constexpr GUID kFolderIdAppsFolder = {
    0x1e87508d, 0x89c2, 0x42f0, {0x8a, 0x7e, 0x64, 0x5a, 0x0f, 0x50, 0xca, 0x58}};
constexpr GUID kBhidEnumItems = {
    0x94f60519, 0x2850, 0x4924, {0xaa, 0x5a, 0xd1, 0x5e, 0x84, 0x86, 0x80, 0x39}};

struct AppItem {
    std::wstring name;
    std::wstring nameLower;
    std::wstring id;  // parsing name inside shell:AppsFolder
};

SRWLOCK g_appIndexLock = SRWLOCK_INIT;
std::vector<AppItem> g_appIndex;
std::atomic<bool> g_appIndexBusy{false};
std::atomic<ULONGLONG> g_appIndexTime{0};
HANDLE g_appIndexThread = nullptr;

bool EndsWith(const std::wstring& s, const wchar_t* suffix) {
    size_t n = wcslen(suffix);
    return s.size() >= n && s.compare(s.size() - n, n, suffix) == 0;
}

// Skips the clutter of the Start menu: uninstallers, readmes, web links.
bool IsUninterestingApp(const std::wstring& nameLower, const std::wstring& idLower) {
    if (idLower.rfind(L"http:", 0) == 0 || idLower.rfind(L"https:", 0) == 0) return true;
    for (const wchar_t* ext : {L".txt", L".chm", L".url", L".htm", L".html", L".pdf",
                               L".rtf", L".ini", L".log", L".hlp"}) {
        if (EndsWith(idLower, ext)) return true;
    }
    std::wstring file = FileNameOf(idLower);
    return file.rfind(L"unins", 0) == 0 || nameLower.find(L"uninstall") != std::wstring::npos;
}

DWORD WINAPI AppIndexThreadProc(LPVOID) {
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    std::vector<AppItem> items;
    IShellItem* folder = nullptr;
    if (SUCCEEDED(SHGetKnownFolderItem(kFolderIdAppsFolder, KF_FLAG_DEFAULT, nullptr,
                                       IID_PPV_ARGS(&folder)))) {
        IEnumShellItems* items_enum = nullptr;
        if (SUCCEEDED(folder->BindToHandler(nullptr, kBhidEnumItems,
                                            IID_PPV_ARGS(&items_enum)))) {
            IShellItem* item = nullptr;
            while (items_enum->Next(1, &item, nullptr) == S_OK) {
                LPWSTR name = nullptr, id = nullptr;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) &&
                    SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &id)) && name &&
                    id && *name) {
                    AppItem a{name, ToLower(name), id};
                    if (!IsUninterestingApp(a.nameLower, ToLower(a.id))) items.push_back(a);
                }
                CoTaskMemFree(name);
                CoTaskMemFree(id);
                item->Release();
                if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) break;
            }
            items_enum->Release();
        }
        folder->Release();
    }
    std::sort(items.begin(), items.end(), [](const AppItem& a, const AppItem& b) {
        return a.nameLower < b.nameLower;
    });

    AcquireSRWLockExclusive(&g_appIndexLock);
    g_appIndex.swap(items);
    size_t count = g_appIndex.size();
    ReleaseSRWLockExclusive(&g_appIndexLock);
    g_appIndexTime = GetTickCount64();
    Wh_Log(L"Indexed %d installed apps", (int)count);
    (void)count;

    if (SUCCEEDED(hrInit)) CoUninitialize();
    g_appIndexBusy = false;
    return 0;
}

std::atomic<int> g_searchApps{1};  // 0 off, 1 hotkey panel, 2 every panel
std::atomic<bool> g_everything{true};
std::atomic<int> g_maxApps{8};
std::atomic<int> g_maxFiles{8};
std::atomic<int> g_fileSort{26};  // EVERYTHING_IPC_SORT_*

// Rebuilds the index in the background (it can take a moment with many
// apps). The panel keeps using the previous index meanwhile.
void RefreshAppIndexAsync(bool force) {
    if (g_searchApps == 0) return;
    ULONGLONG last = g_appIndexTime.load();
    if (!force && last && GetTickCount64() - last < 120000) return;
    if (g_appIndexBusy.exchange(true)) return;
    if (g_appIndexThread) {
        WaitForSingleObject(g_appIndexThread, INFINITE);  // already finished
        CloseHandle(g_appIndexThread);
    }
    g_appIndexThread = CreateThread(nullptr, 0, AppIndexThreadProc, nullptr, 0, nullptr);
    if (!g_appIndexThread) g_appIndexBusy = false;
}

void StopAppIndex() {
    if (g_appIndexThread) {
        WaitForSingleObject(g_appIndexThread, INFINITE);
        CloseHandle(g_appIndexThread);
        g_appIndexThread = nullptr;
    }
}

// Ranks matches: name starts with the text, then a word starts with it,
// then it appears anywhere.
std::vector<AppItem> SearchApps(const std::wstring& textLower, size_t maxResults,
                                const std::vector<std::wstring>& excludeLower) {
    struct Match {
        int score;
        size_t len;
        AppItem item;
    };
    std::vector<Match> matches;
    AcquireSRWLockShared(&g_appIndexLock);
    for (auto& a : g_appIndex) {
        size_t pos = a.nameLower.find(textLower);
        if (pos == std::wstring::npos) continue;
        if (std::find(excludeLower.begin(), excludeLower.end(), a.nameLower) !=
            excludeLower.end()) {
            continue;
        }
        int score = 2;
        if (pos == 0) {
            score = 0;
        } else if (wcschr(L" -_.(", a.nameLower[pos - 1])) {
            score = 1;
        }
        matches.push_back({score, a.name.size(), a});
    }
    ReleaseSRWLockShared(&g_appIndexLock);
    std::stable_sort(matches.begin(), matches.end(), [](const Match& x, const Match& y) {
        return x.score != y.score ? x.score < y.score : x.len < y.len;
    });
    std::vector<AppItem> result;
    for (size_t i = 0; i < matches.size() && i < maxResults; i++) {
        // The Start menu can list the same app twice; keep one.
        bool dup = false;
        for (auto& r : result) dup |= r.nameLower == matches[i].item.nameLower;
        if (!dup) result.push_back(matches[i].item);
    }
    return result;
}

HBITMAP LoadAppBitmap(const std::wstring& id, int size) {
    IShellItem* item = nullptr;
    std::wstring path = L"shell:AppsFolder\\" + id;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item)))) {
        return nullptr;
    }
    HBITMAP bmp = nullptr;
    IShellItemImageFactory* factory = nullptr;
    if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory)))) {
        if (FAILED(factory->GetImage({size, size}, SIIGBF_ICONONLY, &bmp))) bmp = nullptr;
        factory->Release();
    }
    item->Release();
    return bmp;
}

void DrawBitmapIcon(HDC dc, HBITMAP bmp, int x, int y, int size) {
    BITMAP bm{};
    if (!bmp || !GetObjectW(bmp, sizeof(bm), &bm)) return;
    HDC mem = CreateCompatibleDC(dc);
    HGDIOBJ old = SelectObject(mem, bmp);
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(dc, x, y, size, size, mem, 0, 0, bm.bmWidth, std::abs(bm.bmHeight), bf);
    SelectObject(mem, old);
    DeleteDC(mem);
}

void ShellOpen(const std::wstring& file, const wchar_t* params = nullptr) {
    // Our process had the foreground a moment ago; let the new app take it.
    AllowSetForegroundWindow(ASFW_ANY);
    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"open";
    sei.lpFile = file.c_str();
    sei.lpParameters = params;
    sei.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"Couldn't open %s (error %u)", file.c_str(), GetLastError());
    }
}

void LaunchApp(const std::wstring& id) { ShellOpen(L"shell:AppsFolder\\" + id); }

void OpenFileLocation(const std::wstring& path) {
    std::wstring params = L"/select,\"" + path + L"\"";
    ShellOpen(L"explorer.exe", params.c_str());
}

void CopyTextToClipboard(HWND owner, const std::wstring& text) {
    if (!OpenClipboard(owner)) return;
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    if (HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, bytes)) {
        if (void* p = GlobalLock(h)) {
            memcpy(p, text.c_str(), bytes);
            GlobalUnlock(h);
            if (!SetClipboardData(CF_UNICODETEXT, h)) GlobalFree(h);
        } else {
            GlobalFree(h);
        }
    }
    CloseClipboard();
}

// --- Everything (voidtools) IPC ----------------------------------------------
// Uses the documented WM_COPYDATA protocol of Everything 1.4+ (everything_ipc.h),
// so no DLL is needed. Results arrive asynchronously as a WM_COPYDATA message
// to the panel window.

constexpr wchar_t kEverythingWindowClass[] = L"EVERYTHING_TASKBAR_NOTIFICATION";
constexpr ULONG_PTR kEverythingCopyDataQuery2W = 18;
constexpr DWORD kEverythingReplyId = 0x4D484953;  // arbitrary tag for our replies
constexpr DWORD kEverythingRequestName = 0x00000001;
constexpr DWORD kEverythingRequestPath = 0x00000002;
constexpr DWORD kEverythingItemFolder = 0x00000001;

#pragma pack(push, 1)
struct EverythingQuery2 {
    DWORD replyHwnd;
    DWORD replyCopydataMessage;
    DWORD searchFlags;
    DWORD offset;
    DWORD maxResults;
    DWORD requestFlags;
    DWORD sortType;
    // followed by the null-terminated search text
};
struct EverythingList2 {
    DWORD totalItems;
    DWORD numItems;
    DWORD offset;
    DWORD requestFlags;
    DWORD sortType;
    // followed by EverythingItem2[numItems], then the item data
};
struct EverythingItem2 {
    DWORD flags;
    DWORD dataOffset;
};
#pragma pack(pop)

struct FileResult {
    std::wstring name;
    std::wstring folder;
    std::wstring fullPath;
    bool isFolder;
};

bool IsEverythingRunning() { return FindWindowW(kEverythingWindowClass, nullptr) != nullptr; }

bool SendEverythingQuery(HWND reply, const std::wstring& search, DWORD maxResults) {
    HWND everything = FindWindowW(kEverythingWindowClass, nullptr);
    if (!everything) return false;
    size_t size = sizeof(EverythingQuery2) + (search.size() + 1) * sizeof(wchar_t);
    std::vector<BYTE> buf(size);
    auto* q = reinterpret_cast<EverythingQuery2*>(buf.data());
    q->replyHwnd = (DWORD)(DWORD_PTR)reply;
    q->replyCopydataMessage = kEverythingReplyId;
    q->searchFlags = 0;
    q->offset = 0;
    q->maxResults = maxResults;
    q->requestFlags = kEverythingRequestName | kEverythingRequestPath;
    q->sortType = (DWORD)g_fileSort.load();
    memcpy(q + 1, search.c_str(), (search.size() + 1) * sizeof(wchar_t));
    COPYDATASTRUCT cds{kEverythingCopyDataQuery2W, (DWORD)size, buf.data()};
    // Not SMTO_BLOCK: Everything may answer while we wait.
    DWORD_PTR result = 0;
    return SendMessageTimeoutW(everything, WM_COPYDATA, (WPARAM)reply, (LPARAM)&cds,
                               SMTO_ABORTIFHUNG, 300, &result) &&
           result;
}

// Reads a "DWORD length + null-terminated text" field, with bounds checks.
bool ReadEverythingString(const BYTE* base, size_t size, size_t* pos, std::wstring* out) {
    if (*pos + sizeof(DWORD) > size) return false;
    DWORD len = *reinterpret_cast<const DWORD*>(base + *pos);
    *pos += sizeof(DWORD);
    size_t bytes = ((size_t)len + 1) * sizeof(wchar_t);
    if (len > 32767 || *pos + bytes > size) return false;
    out->assign(reinterpret_cast<const wchar_t*>(base + *pos), len);
    *pos += bytes;
    return true;
}

std::vector<FileResult> ParseEverythingReply(const COPYDATASTRUCT* cds, DWORD* total) {
    std::vector<FileResult> results;
    *total = 0;
    if (!cds->lpData || cds->cbData < sizeof(EverythingList2)) return results;
    const BYTE* base = static_cast<const BYTE*>(cds->lpData);
    size_t size = cds->cbData;
    auto* list = reinterpret_cast<const EverythingList2*>(base);
    *total = list->totalItems;
    if ((list->requestFlags & (kEverythingRequestName | kEverythingRequestPath)) !=
        (kEverythingRequestName | kEverythingRequestPath)) {
        return results;
    }
    size_t itemsEnd = sizeof(EverythingList2) + (size_t)list->numItems * sizeof(EverythingItem2);
    if (list->numItems > 1000 || itemsEnd > size) return results;
    auto* items = reinterpret_cast<const EverythingItem2*>(list + 1);
    for (DWORD i = 0; i < list->numItems; i++) {
        size_t pos = items[i].dataOffset;
        FileResult r;
        if (!ReadEverythingString(base, size, &pos, &r.name) ||
            !ReadEverythingString(base, size, &pos, &r.folder)) {
            break;
        }
        r.isFolder = items[i].flags & kEverythingItemFolder;
        r.fullPath = r.folder.empty() ? r.name
                     : r.folder.back() == L'\\' ? r.folder + r.name
                                                : r.folder + L"\\" + r.name;
        results.push_back(r);
    }
    return results;
}

// Opens Everything's own window with the same search, for the full list.
void OpenEverythingSearch(const std::wstring& search) {
    HWND everything = FindWindowW(kEverythingWindowClass, nullptr);
    DWORD pid = 0;
    if (everything) GetWindowThreadProcessId(everything, &pid);
    std::wstring exe = pid ? GetProcessPath(pid) : L"";
    if (exe.empty()) return;
    std::wstring text;
    for (wchar_t c : search) {
        if (c != L'"') text += c;
    }
    std::wstring params = L"-search \"" + text + L"\"";
    ShellOpen(exe, params.c_str());
}

HICON LoadFileIcon(const std::wstring& path) {
    SHFILEINFOW sfi{};
    if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON)) {
        return sfi.hIcon;
    }
    return nullptr;
}

void LoadLauncherSettings() {
    int mode = 1;
    if (StringSettingIs(L"switching.searchApps", L"off")) mode = 0;
    if (StringSettingIs(L"switching.searchApps", L"always")) mode = 2;
    g_searchApps = mode;
    g_everything = GetBoolSetting(L"switching.everything");
    int maxApps = Wh_GetIntSetting(L"switching.maxApps");
    g_maxApps = maxApps ? Clamp(maxApps, 3, 30) : 8;
    int maxFiles = Wh_GetIntSetting(L"switching.maxFiles");
    g_maxFiles = maxFiles ? Clamp(maxFiles, 3, 30) : 8;
    // EVERYTHING_IPC_SORT_* values from everything_ipc.h.
    int sort = 26;  // DATE_RUN_DESCENDING
    if (StringSettingIs(L"switching.fileSort", L"name")) sort = 1;
    if (StringSettingIs(L"switching.fileSort", L"modified")) sort = 14;
    if (StringSettingIs(L"switching.fileSort", L"runCount")) sort = 20;
    if (StringSettingIs(L"switching.fileSort", L"recentChange")) sort = 22;
    g_fileSort = sort;
}

// ===========================================================================
// Panel state
// ===========================================================================

struct Entry {
    HWND hwnd;
    std::wstring title;
    std::wstring label;
    std::wstring app;
    int number;   // stable instance number (per app)
    int display;  // number shown and typed; 0 = none (other apps)
    COLORREF color;
    bool minimized;
    HICON icon;
    bool ownsIcon;
    int section;
    bool sameApp;
    int mru;
    int kind = 0;  // EntryWindow / EntryApp / EntryFile
    HBITMAP bitmap = nullptr;  // app icon (owned by the panel's cache)
};

constexpr int EntryWindow = 0;
constexpr int EntryApp = 1;
constexpr int EntryFile = 2;
constexpr int EntryMore = 3;  // "show all results in Everything"

struct Section {
    std::wstring key;
    std::wstring name;
    HICON icon = nullptr;
    bool current = false;
    int total = 0;
    int matches = 0;
    bool collapsed = false;
    int first = 0;  // range in Panel::entries
    int count = 0;
    RECT headerRect{};  // content coordinates
    int kind = EntryWindow;  // what the section lists
};

std::vector<std::wstring> g_collapsedApps;  // remembered for the session

enum class PanelResult { None, Select, Pair, Action, RestoreLayout };

struct FooterButton {
    Act act;
    RECT rect;
};

constexpr UINT_PTR kScrollAnimTimerId = 6;

struct Panel {
    HWND hwnd = nullptr;
    HWND source = nullptr;
    bool grid = false;
    bool showAll = false;
    bool hotkeyMode = false;
    std::wstring appName;
    HICON appIcon = nullptr;
    bool ownsAppIcon = false;
    std::vector<Section> secs;
    std::vector<Entry> all;      // every window (owns the icons)
    std::vector<Entry> entries;  // visible: matching the filter, not collapsed
    std::wstring filter;
    std::vector<HWND> selected;

    // Item geometry is in "content" coordinates: client coordinates as if
    // the list were scrolled to the top. Subtract scrollY to draw.
    std::vector<RECT> itemRects, closeRects, pairRects, thumbAreas, textRects, thumbDest;
    std::vector<SIZE> thumbSrc;
    std::vector<HTHUMBNAIL> thumbs;
    RECT headerRect{}, listRect{}, footerRect{};
    std::vector<FooterButton> footer;
    int cols = 1;
    int width = 0;
    bool pairToRight = true;

    int contentH = 0;
    int scrollY = 0;
    int scrollTarget = 0;
    int maxScroll = 0;
    RECT scrollThumb{};
    bool dragThumb = false;
    int dragStartY = 0;
    int dragStartScroll = 0;

    int hot = -1;
    bool hotClose = false;
    bool hotPair = false;
    bool hotScrollbar = false;
    int hotHeader = -1;
    int hotFooter = -1;
    bool tracking = false;
    bool menuOpen = false;

    HWND edit = nullptr;
    int editIndex = -1;     // entry being renamed
    bool namingLayout = false;
    WNDPROC editOldProc = nullptr;
    HBRUSH editBrush = nullptr;

    POINT origin{};
    ULONGLONG outsideSince = 0;
    // True once the panel really became the foreground window. Until then a
    // WM_ACTIVATE(WA_INACTIVE) is just Windows refusing to hand us the
    // foreground (e.g. right after the user clicked or maximized a window),
    // not the user leaving the panel.
    bool wasForeground = false;
    bool glass = false;  // see-through material active

    bool done = false;
    PanelResult result = PanelResult::None;
    int resultIndex = -1;
    Act resultAct = Act::Grid;
    std::vector<SavedLayout> layouts;  // loaded when the menu opens

    // Launcher
    int appSection = -1;
    int fileSection = -1;
    std::vector<FileResult> fileResults;
    DWORD fileTotal = 0;
    std::wstring fileQuery;  // last text sent to Everything
    std::vector<std::pair<std::wstring, HBITMAP>> appBitmaps;
    std::vector<std::pair<std::wstring, HICON>> fileIcons;
};

bool IsWindowEntry(const Entry& e) { return e.kind == EntryWindow; }

Panel* g_panel = nullptr;

RECT ShiftRect(RECT r, int dy) {
    OffsetRect(&r, 0, dy);
    return r;
}

// Item rect in client coordinates (after scrolling).
RECT ItemClientRect(const Panel& p, int i) { return ShiftRect(p.itemRects[i], -p.scrollY); }

bool IsSelected(const Panel& p, HWND hwnd) {
    return std::find(p.selected.begin(), p.selected.end(), hwnd) != p.selected.end();
}

void ToggleSelected(Panel& p, HWND hwnd) {
    auto it = std::find(p.selected.begin(), p.selected.end(), hwnd);
    if (it == p.selected.end()) {
        p.selected.push_back(hwnd);
    } else {
        p.selected.erase(it);
    }
    InvalidateRect(p.hwnd, nullptr, FALSE);
}

void DrawSearchGlyph(HDC dc, int x, int cy, COLORREF c);

int DrawEntryBadge(HDC dc, int x, int cy, int size, const Entry& e) {
    if (e.kind == EntryMore) {
        DrawSearchGlyph(dc, x + size / 2 - Sc(7), cy, e.color);
        return x + size;
    }
    if (e.kind == EntryApp) {
        int is = size * 5 / 6;
        DrawBitmapIcon(dc, e.bitmap, x + (size - is) / 2, cy - is / 2, is);
        return x + size;
    }
    if (!e.sameApp) {
        int is = size * 5 / 6;
        if (e.icon) {
            DrawIconEx(dc, x + (size - is) / 2, cy - is / 2, e.icon, is, is, 0, nullptr,
                       DI_NORMAL);
        }
        return x + size;
    }
    RECT badge{x, cy - size / 2, x + size, cy - size / 2 + size};
    {
        Gdiplus::Graphics g(dc);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        FillRoundRect(g, e.color, (float)badge.left, (float)badge.top, (float)size,
                      (float)size, g_badgeCircle ? (float)size / 2 : (float)size / 4);
    }
    DrawTextIn(dc, size >= Sc(24) ? g_fontBold : g_fontSmallBold, ContrastText(e.color),
               std::to_wstring(e.number), badge, DT_CENTER | DT_VCENTER);
    return badge.right;
}

// ===========================================================================
// Window chrome (corners, border, backdrop) and see-through painting
// ===========================================================================

constexpr DWORD kDwmSystemBackdropType = 38;
constexpr DWORD kDwmColorNone = 0xFFFFFFFE;

// Applies dark mode, corners, border and backdrop to a panel-like window.
// Returns true if a see-through material (Acrylic/Mica) is active.
// Acrylic through the window "accent" (the same mechanism the taskbar and
// many shell mods use). Unlike the DWMSBT_TRANSIENTWINDOW backdrop, it is
// drawn on popup windows right away and whether or not they're active.
struct AccentPolicy {
    int accentState;
    DWORD accentFlags;
    DWORD gradientColor;  // AABBGGRR
    DWORD animationId;
};
struct WindowCompositionAttribData {
    DWORD attribute;
    PVOID data;
    SIZE_T sizeOfData;
};
constexpr DWORD kWcaAccentPolicy = 19;
constexpr int kAccentDisabled = 0;
constexpr int kAccentAcrylicBlurBehind = 4;

bool SetAccent(HWND hwnd, int state, COLORREF tint, int alpha) {
    using SetWindowCompositionAttribute_t =
        BOOL(WINAPI*)(HWND, WindowCompositionAttribData*);
    static auto setAttribute = reinterpret_cast<SetWindowCompositionAttribute_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute"));
    if (!setAttribute) return false;
    AccentPolicy policy{state, 0,
                        ((DWORD)alpha << 24) | ((DWORD)GetBValue(tint) << 16) |
                            ((DWORD)GetGValue(tint) << 8) | GetRValue(tint),
                        0};
    WindowCompositionAttribData data{kWcaAccentPolicy, &policy, sizeof(policy)};
    return setAttribute(hwnd, &data);
}

// True when the tint is already part of the material (accent Acrylic), so
// the panel doesn't paint it again.
bool g_materialHasTint = false;

// Applies dark mode, corners, border and background material to a panel-like
// window. Returns true if a see-through material is active.
bool ApplyWindowChrome(HWND hwnd, COLORREF themedBorder) {
    BOOL dark = g_dark;
    DwmSetWindowAttribute(hwnd, kDwmUseDarkMode, &dark, sizeof(dark));
    int cornerMode = g_cornerMode.load();
    DWORD corner = cornerMode == 2 ? 1 : cornerMode == 1 ? 3 : 2;  // DWMWCP_*
    DwmSetWindowAttribute(hwnd, kDwmCornerPreference, &corner, sizeof(corner));
    int borderMode = g_borderMode.load();
    COLORREF border = borderMode == 2   ? kDwmColorNone
                      : borderMode == 1 ? AccentColor()
                                        : themedBorder;
    DwmSetWindowAttribute(hwnd, kDwmBorderColor, &border, sizeof(border));

    Backdrop backdrop = (Backdrop)g_backdrop.load();
    bool glass = false;
    g_materialHasTint = false;

    if (backdrop == Backdrop::Acrylic) {
        int none = 1;  // DWMSBT_NONE: the accent draws the material
        DwmSetWindowAttribute(hwnd, kDwmSystemBackdropType, &none, sizeof(none));
        // A completely clear tint can make Windows skip the blur: keep a
        // little of the theme color in it.
        int alpha = 0x30 + g_tintOpacity.load() * (0xFF - 0x30) / 100;
        if (SetAccent(hwnd, kAccentAcrylicBlurBehind, GetTheme().bg, alpha)) {
            glass = true;
            g_materialHasTint = true;
        }
    } else {
        SetAccent(hwnd, kAccentDisabled, 0, 0);
        int type = backdrop == Backdrop::Mica      ? 2   // DWMSBT_MAINWINDOW
                   : backdrop == Backdrop::MicaAlt ? 4   // DWMSBT_TABBEDWINDOW
                                                   : 1;  // DWMSBT_NONE
        HRESULT hr = DwmSetWindowAttribute(hwnd, kDwmSystemBackdropType, &type, sizeof(type));
        glass = SUCCEEDED(hr) && type != 1;
        if (type != 1 && FAILED(hr)) {
            Wh_Log(L"Backdrop not supported (0x%08X)", (unsigned)hr);
        }
    }

    MARGINS margins{0, 0, 0, 0};
    if (glass) margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    return glass;
}

// System backdrops on popup windows are often only picked up after a
// resize: nudge the size once after showing.
void RefreshMaterial(HWND hwnd) {
    RECT r;
    if (!GetWindowRect(hwnd, &r)) return;
    UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE;
    SetWindowPos(hwnd, nullptr, 0, 0, r.right - r.left + 1, r.bottom - r.top, flags);
    SetWindowPos(hwnd, nullptr, 0, 0, r.right - r.left, r.bottom - r.top, flags);
    SendMessageW(hwnd, WM_NCACTIVATE, TRUE, 0);
}

// Off-screen 32-bit buffer for flicker-free painting. With a see-through
// material, the theme's background colors are made (partly) transparent
// when the buffer is copied to the window.
struct PaintBuffer {
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    DWORD* bits = nullptr;
    int w = 0, h = 0;
};

bool BeginPaintBuffer(HDC target, int w, int h, PaintBuffer* pb) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    pb->bmp = CreateDIBSection(target, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!pb->bmp || !bits) {
        if (pb->bmp) DeleteObject(pb->bmp);
        pb->bmp = nullptr;
        return false;
    }
    pb->bits = static_cast<DWORD*>(bits);
    pb->dc = CreateCompatibleDC(target);
    pb->old = SelectObject(pb->dc, pb->bmp);
    pb->w = w;
    pb->h = h;
    return true;
}

void FreePaintBuffer(PaintBuffer& pb) {
    if (!pb.dc) return;
    SelectObject(pb.dc, pb.old);
    DeleteDC(pb.dc);
    DeleteObject(pb.bmp);
    pb = PaintBuffer{};
}

void EndPaintBuffer(HDC target, PaintBuffer& pb) {
    GdiFlush();
    BitBlt(target, 0, 0, pb.w, pb.h, pb.dc, 0, 0, SRCCOPY);
    FreePaintBuffer(pb);
}

// Paints for a see-through material. GDI can't produce transparency, so the
// content is painted twice, on black and on white: the difference between
// the two gives each pixel's exact coverage (text edges, rounded corners,
// icons), and the result is composed over the tint. This keeps text crisp
// instead of leaving halos of the background color around the glyphs.
template <typename PaintFn>
void PaintOverMaterial(HDC target, int w, int h, const Theme& t, PaintFn paint) {
    PaintBuffer onBlack, onWhite;
    if (!BeginPaintBuffer(target, w, h, &onBlack)) return;
    if (!BeginPaintBuffer(target, w, h, &onWhite)) {
        FreePaintBuffer(onBlack);
        return;
    }
    g_glassPaint = true;
    g_glassTheme = t;
    paint(onBlack.dc, RGB(0, 0, 0));
    paint(onWhite.dc, RGB(0xFF, 0xFF, 0xFF));
    g_glassPaint = false;
    GdiFlush();

    int tintA = g_materialHasTint ? 0 : g_tintOpacity.load() * 255 / 100;
    int tr = GetRValue(t.bg) * tintA / 255, tg = GetGValue(t.bg) * tintA / 255,
        tb = GetBValue(t.bg) * tintA / 255;
    size_t n = (size_t)w * h;
    for (size_t i = 0; i < n; i++) {
        DWORD pb = onBlack.bits[i], pw = onWhite.bits[i];
        int rb = (pb >> 16) & 0xFF, gb = (pb >> 8) & 0xFF, bb = pb & 0xFF;
        int rw = (pw >> 16) & 0xFF, gw = (pw >> 8) & 0xFF, bw = pw & 0xFF;
        int alpha = 255 - ((rw - rb) + (gw - gb) + (bw - bb)) / 3;
        alpha = Clamp(alpha, 0, 255);
        // On black, the color is already premultiplied by its coverage.
        rb = std::min(rb, alpha);
        gb = std::min(gb, alpha);
        bb = std::min(bb, alpha);
        int inv = 255 - alpha;
        DWORD outA = alpha + tintA * inv / 255;
        DWORD outR = rb + tr * inv / 255, outG = gb + tg * inv / 255, outB = bb + tb * inv / 255;
        onBlack.bits[i] = (outA << 24) | (outR << 16) | (outG << 8) | outB;
    }
    BitBlt(target, 0, 0, w, h, onBlack.dc, 0, 0, SRCCOPY);
    FreePaintBuffer(onBlack);
    FreePaintBuffer(onWhite);
}

// ===========================================================================
// Live preview (list mode)
// ===========================================================================

HWND g_previewHwnd = nullptr;
HTHUMBNAIL g_thumb = nullptr;
int g_previewIndex = -1;
bool g_previewGlass = false;
RECT g_thumbRect{};

void HidePreview() {
    if (g_thumb) {
        DwmUnregisterThumbnail(g_thumb);
        g_thumb = nullptr;
    }
    g_previewIndex = -1;
    if (g_previewHwnd) ShowWindow(g_previewHwnd, SW_HIDE);
}

void ShowPreview(int index) {
    if (index == g_previewIndex) return;
    HidePreview();
    Panel* p = g_panel;
    if (!p || p->grid || p->menuOpen || !g_showPreview || !g_previewHwnd || index < 0 ||
        index >= (int)p->entries.size()) {
        return;
    }
    const Entry& e = p->entries[index];
    if (!IsWindowEntry(e)) return;
    if (FAILED(DwmRegisterThumbnail(g_previewHwnd, e.hwnd, &g_thumb))) {
        g_thumb = nullptr;
        return;
    }
    g_previewIndex = index;

    SIZE src{};
    DwmQueryThumbnailSourceSize(g_thumb, &src);
    if (src.cx <= 0 || src.cy <= 0) src = {16, 10};
    double scale =
        std::min({(double)Sc(400) / src.cx, (double)Sc(260) / src.cy, 1.0});
    int tw = std::max(1, (int)(src.cx * scale));
    int th = std::max(1, (int)(src.cy * scale));

    int pad = Sc(10), header = Sc(26), gap = Sc(8);
    int w = std::max(tw, Sc(220)) + pad * 2;
    int h = pad + header + gap + th + pad;
    g_thumbRect = {pad + (w - 2 * pad - tw) / 2, pad + header + gap, 0, 0};
    g_thumbRect.right = g_thumbRect.left + tw;
    g_thumbRect.bottom = g_thumbRect.top + th;

    RECT anchor{};
    GetWindowRect(p->hwnd, &anchor);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromRect(&anchor, MONITOR_DEFAULTTONEAREST), &mi);
    int x = anchor.right + Sc(8);
    if (x + w > mi.rcWork.right) x = anchor.left - Sc(8) - w;
    if (x < mi.rcWork.left) x = mi.rcWork.left;
    // Align with the highlighted row so it stays close in long lists.
    RECT item = ItemClientRect(*p, index);
    int y = Clamp(anchor.top + item.top - Sc(6), mi.rcWork.top, mi.rcWork.bottom - h);

    g_previewGlass = ApplyWindowChrome(g_previewHwnd, e.color);
    if (g_borderMode == 0) {
        // The preview is framed in the window's own color.
        COLORREF border = e.color;
        DwmSetWindowAttribute(g_previewHwnd, kDwmBorderColor, &border, sizeof(border));
    }

    SetWindowPos(g_previewHwnd, HWND_TOPMOST, x, y, w, h,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    if (g_previewGlass) RefreshMaterial(g_previewHwnd);

    DWM_THUMBNAIL_PROPERTIES props{};
    props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY |
                    DWM_TNP_SOURCECLIENTAREAONLY;
    props.rcDestination = g_thumbRect;
    props.fVisible = TRUE;
    props.opacity = 255;
    props.fSourceClientAreaOnly = FALSE;
    DwmUpdateThumbnailProperties(g_thumb, &props);
    InvalidateRect(g_previewHwnd, nullptr, TRUE);
}

LRESULT CALLBACK PreviewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC wdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            Theme t = GetTheme();
            auto paint = [&](HDC dc, COLORREF background) {
                FillSolid(dc, rc, background);
                Panel* p = g_panel;
                if (p && g_previewIndex >= 0 && g_previewIndex < (int)p->entries.size()) {
                    const Entry& e = p->entries[g_previewIndex];
                    int pad = Sc(10), header = Sc(26);
                    int x = DrawEntryBadge(dc, pad, pad + header / 2, Sc(24), e) + Sc(10);
                    std::wstring text = e.label.empty() ? e.title : e.label;
                    DrawTextIn(dc, e.label.empty() ? g_font : g_fontBold, t.text, text,
                               {x, pad, rc.right - pad, pad + header}, DT_LEFT | DT_VCENTER);
                    RECT frame = g_thumbRect;
                    InflateRect(&frame, 1, 1);
                    FillSolid(dc, frame, t.border);
                }
            };
            if (g_previewGlass) {
                PaintOverMaterial(wdc, rc.right, rc.bottom, t, paint);
            } else {
                PaintBuffer pb;
                if (BeginPaintBuffer(wdc, rc.right, rc.bottom, &pb)) {
                    paint(pb.dc, t.bg);
                    EndPaintBuffer(wdc, pb);
                }
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_NCACTIVATE:
            // Acrylic/Mica turn solid on inactive windows: always look active.
            return DefWindowProcW(hwnd, msg, TRUE, lParam);
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ===========================================================================
// Panel layout
// ===========================================================================

constexpr int kSectionH = 30;
constexpr int kGridTileW = 236;
constexpr int kGridThumbH = 138;
constexpr int kFooterH = 40;

bool HasSectionHeaders(const Panel& p) {
    if (p.showAll) return true;
    for (int s : {p.appSection, p.fileSection}) {
        if (s >= 0 && p.secs[s].matches > 0) return true;
    }
    return false;
}

int ComputePanelWidth(Panel& p) {
    int pad = Sc(6);
    if (p.grid) {
        MONITORINFO mi = MonitorInfoFromPoint(p.origin);
        int maxPerSection = 1;
        for (auto& s : p.secs) maxPerSection = std::max(maxPerSection, s.total);
        int tileW = Sc(kGridTileW), gap = Sc(6);
        int cols = std::min(maxPerSection, 4);
        if (p.showAll) cols = 4;
        while (cols > 1 && pad * 2 + cols * tileW + (cols - 1) * gap + Sc(10) >
                               (mi.rcWork.right - mi.rcWork.left) * 9 / 10) {
            cols--;
        }
        p.cols = std::max(cols, 1);
        return pad * 2 + p.cols * tileW + (p.cols - 1) * gap + Sc(10);
    }
    HDC dc = GetDC(nullptr);
    int maxText = 0;
    for (auto& e : p.all) {
        int w = std::max(TextWidth(dc, g_font, e.title),
                         e.label.empty() ? 0 : TextWidth(dc, g_fontBold, e.label));
        if (e.minimized) w += Sc(14) + TextWidth(dc, g_fontSmall, g_str->minimizedTag);
        maxText = std::max(maxText, std::min(w, Sc(440)));
    }
    ReleaseDC(nullptr, dc);
    int buttons = (g_closeButton ? Sc(30) : 0) + (g_pairButton && p.source ? Sc(30) : 0);
    // Wide enough for the bottom bar too.
    int footerW = Sc(24) + (int)p.footer.size() * Sc(38) + Sc(20) + Sc(110);
    int minW = std::min(std::max(Sc(p.hotkeyMode ? 480 : 320), footerW), Sc(640));
    return Clamp(pad * 2 + Sc(12 + 24 + 12) + maxText + Sc(16) + buttons + Sc(10), minW,
                 Sc(640));
}

void LayoutFooter(Panel& p, int y) {
    p.footerRect = {Sc(6), y, p.width - Sc(6), y + Sc(kFooterH)};
    int bw = Sc(34), bh = Sc(30), gap = Sc(4);
    int cy = (p.footerRect.top + p.footerRect.bottom) / 2;
    int x = p.footerRect.left + Sc(4);
    Act prev = Act::Grid;
    bool first = true;
    for (auto& b : p.footer) {
        if (b.act == Act::SavedLayouts) continue;
        bool isGroup = b.act >= Act::MinimizeOthers;
        bool prevGroup = prev >= Act::MinimizeOthers;
        if (!first && isGroup != prevGroup) x += Sc(10);  // gap between groups
        b.rect = {x, cy - bh / 2, x + bw, cy + bh / 2};
        x += bw + gap;
        prev = b.act;
        first = false;
    }
    for (auto& b : p.footer) {
        if (b.act != Act::SavedLayouts) continue;
        int right = p.footerRect.right - Sc(4);
        b.rect = {right - bw, cy - bh / 2, right, cy + bh / 2};
    }
}

SIZE LayoutPanel(Panel& p) {
    int n = (int)p.entries.size();
    int pad = Sc(6), headerH = Sc(34);
    int W = p.width;
    p.itemRects.assign(n, {});
    p.closeRects.assign(n, {});
    p.pairRects.assign(n, {});
    p.thumbAreas.assign(n, {});
    p.textRects.assign(n, {});

    int y = pad;
    p.headerRect = {pad, y, W - pad, y + headerH};
    y += headerH;
    int listTop = y;
    int right = W - pad - Sc(10);  // leave room for the scrollbar
    bool headers = HasSectionHeaders(p);
    bool pairBtn = g_pairButton && p.source;

    for (auto& sec : p.secs) {
        sec.headerRect = {};
        if (sec.matches == 0) continue;
        if (headers) {
            sec.headerRect = {pad, y, right, y + Sc(kSectionH)};
            y += Sc(kSectionH);
        }
        if (!p.grid) {
            int rowH = Sc(ListRowH());
            for (int i = sec.first; i < sec.first + sec.count; i++) {
                RECT r{pad, y, right, y + rowH};
                int cy = (r.top + r.bottom) / 2;
                p.itemRects[i] = r;
                p.closeRects[i] = {r.right - Sc(8) - Sc(26), cy - Sc(13), r.right - Sc(8),
                                   cy + Sc(13)};
                int btnRight = g_closeButton ? p.closeRects[i].left - Sc(4) : r.right - Sc(8);
                p.pairRects[i] = {btnRight - Sc(26), cy - Sc(13), btnRight, cy + Sc(13)};
                int textRight = pairBtn ? p.pairRects[i].left
                                        : (g_closeButton ? p.closeRects[i].left : r.right);
                p.textRects[i] = {r.left + Sc(12 + 24 + 12), r.top, textRight - Sc(8),
                                  r.bottom};
                y += rowH;
            }
        } else {
            int tileW = Sc(kGridTileW), thumbH = Sc(kGridThumbH), gap = Sc(6),
                titleH = Sc(32);
            int tileH = Sc(8) + thumbH + Sc(4) + titleH + Sc(2);
            int rows = (sec.count + p.cols - 1) / p.cols;
            for (int k = 0; k < sec.count; k++) {
                int i = sec.first + k;
                int row = k / p.cols, col = k % p.cols;
                RECT r{pad + col * (tileW + gap), y + row * (tileH + gap), 0, 0};
                r.right = r.left + tileW;
                r.bottom = r.top + tileH;
                p.itemRects[i] = r;
                p.thumbAreas[i] = {r.left + Sc(8), r.top + Sc(8), r.right - Sc(8),
                                   r.top + Sc(8) + thumbH};
                int ty = p.thumbAreas[i].bottom + Sc(4);
                int cy = ty + titleH / 2;
                p.closeRects[i] = {r.right - Sc(8) - Sc(24), cy - Sc(12), r.right - Sc(8),
                                   cy + Sc(12)};
                int btnRight = g_closeButton ? p.closeRects[i].left - Sc(4) : r.right - Sc(8);
                p.pairRects[i] = {btnRight - Sc(24), cy - Sc(12), btnRight, cy + Sc(12)};
                int textRight = pairBtn ? p.pairRects[i].left
                                        : (g_closeButton ? p.closeRects[i].left : r.right);
                p.textRects[i] = {r.left + Sc(8 + 20 + 8), ty, textRight - Sc(6),
                                  ty + titleH};
            }
            if (rows > 0) y += rows * tileH + (rows - 1) * gap + Sc(6);
        }
        if (headers) y += Sc(4);
    }
    bool anyMatch = false;
    for (auto& s : p.secs) anyMatch |= s.matches > 0;
    if (!anyMatch) y += Sc(56);  // "no results" message
    p.contentH = y - listTop;

    // Cap the list height so the panel fits on screen; the rest scrolls.
    MONITORINFO mi = MonitorInfoFromPoint(p.origin);
    int footerH = p.footer.empty() ? 0 : Sc(7 + kFooterH);
    int maxPanelH = (mi.rcWork.bottom - mi.rcWork.top) * 8 / 10;
    int maxListH = std::max(Sc(ListRowH()) * 2, maxPanelH - pad * 2 - headerH - footerH);
    int visibleH = std::min(p.contentH, maxListH);
    p.listRect = {pad, listTop, W - pad, listTop + visibleH};
    p.maxScroll = std::max(0, p.contentH - visibleH);
    p.scrollY = Clamp(p.scrollY, 0, p.maxScroll);
    p.scrollTarget = Clamp(p.scrollTarget, 0, p.maxScroll);

    y = p.listRect.bottom;
    if (!p.footer.empty()) {
        y += Sc(7);
        LayoutFooter(p, y);
        y += Sc(kFooterH);
    }
    return {W, y + pad};
}

void UnregisterThumbs(Panel& p) {
    for (HTHUMBNAIL t : p.thumbs) {
        if (t) DwmUnregisterThumbnail(t);
    }
    p.thumbs.clear();
}

// Positions grid thumbnails for the current scroll offset, cropping the
// ones that are partly outside the visible list area.
void UpdateThumbs(Panel& p) {
    for (size_t i = 0; i < p.thumbs.size(); i++) {
        if (!p.thumbs[i]) continue;
        RECT d = ShiftRect(p.thumbDest[i], -p.scrollY);
        RECT v;
        DWM_THUMBNAIL_PROPERTIES props{};
        props.dwFlags = DWM_TNP_VISIBLE;
        if (p.menuOpen || !IntersectRect(&v, &d, &p.listRect)) {
            props.fVisible = FALSE;
        } else {
            double sx = (double)p.thumbSrc[i].cx / std::max(1L, d.right - d.left);
            double sy = (double)p.thumbSrc[i].cy / std::max(1L, d.bottom - d.top);
            props.dwFlags |= DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE |
                             DWM_TNP_OPACITY | DWM_TNP_SOURCECLIENTAREAONLY;
            props.rcDestination = v;
            props.rcSource = {(LONG)((v.left - d.left) * sx), (LONG)((v.top - d.top) * sy),
                              (LONG)((v.right - d.left) * sx),
                              (LONG)((v.bottom - d.top) * sy)};
            props.fVisible = TRUE;
            props.opacity = 255;
            props.fSourceClientAreaOnly = FALSE;
        }
        DwmUpdateThumbnailProperties(p.thumbs[i], &props);
    }
}

void RegisterThumbs(Panel& p) {
    UnregisterThumbs(p);
    if (!p.grid) return;
    size_t n = p.entries.size();
    p.thumbs.assign(n, nullptr);
    p.thumbDest.assign(n, {});
    p.thumbSrc.assign(n, {});
    for (size_t i = 0; i < n; i++) {
        if (p.entries[i].minimized || !IsWindowEntry(p.entries[i])) continue;
        HTHUMBNAIL th = nullptr;
        if (FAILED(DwmRegisterThumbnail(p.hwnd, p.entries[i].hwnd, &th))) continue;
        SIZE src{};
        DwmQueryThumbnailSourceSize(th, &src);
        if (src.cx <= 0 || src.cy <= 0) {
            DwmUnregisterThumbnail(th);
            continue;
        }
        RECT a = p.thumbAreas[i];
        int aw = a.right - a.left, ah = a.bottom - a.top;
        double scale = std::min((double)aw / src.cx, (double)ah / src.cy);
        int tw = std::max(1, (int)(src.cx * scale)), tht = std::max(1, (int)(src.cy * scale));
        RECT d{a.left + (aw - tw) / 2, a.top + (ah - tht) / 2, 0, 0};
        d.right = d.left + tw;
        d.bottom = d.top + tht;
        p.thumbDest[i] = d;
        p.thumbSrc[i] = src;
        p.thumbs[i] = th;
    }
    UpdateThumbs(p);
}

// ===========================================================================
// Panel painting
// ===========================================================================

void DrawCloseGlyph(HDC dc, const RECT& r, bool hot, const Theme& t) {
    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    if (hot) {
        FillRoundRect(g, RGB(0xC4, 0x2B, 0x1C), (float)r.left, (float)r.top,
                      (float)(r.right - r.left), (float)(r.bottom - r.top), (float)Sc(5));
    }
    Gdiplus::Pen pen(ToColor(hot ? RGB(0xFF, 0xFF, 0xFF) : t.subtext), (float)Sc(3) / 2);
    float cx = (r.left + r.right) / 2.0f, cy = (r.top + r.bottom) / 2.0f, a = (float)Sc(4);
    g.DrawLine(&pen, cx - a, cy - a, cx + a, cy + a);
    g.DrawLine(&pen, cx - a, cy + a, cx + a, cy - a);
}

// Two side-by-side panes: the one where the picked window will go is filled
// with its color, the current window's side is just outlined.
void DrawPairGlyph(HDC dc, const RECT& r, bool hot, bool toRight, COLORREF accent,
                   const Theme& t) {
    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    if (hot) {
        FillRoundRect(g, t.border, (float)r.left, (float)r.top, (float)(r.right - r.left),
                      (float)(r.bottom - r.top), (float)Sc(5));
    }
    float pw = (float)Sc(7), ph = (float)Sc(12), gap = (float)Sc(2);
    float cx = (r.left + r.right) / 2.0f, cy = (r.top + r.bottom) / 2.0f;
    float lx = cx - pw - gap / 2, rx = cx + gap / 2, y = cy - ph / 2;
    float sw = (float)Sc(3) / 2;
    COLORREF outline = hot ? t.text : t.subtext;
    float curX = toRight ? lx : rx, newX = toRight ? rx : lx;
    StrokeRoundRect(g, outline, sw, curX + sw / 2, y + sw / 2, pw - sw, ph - sw, (float)Sc(2));
    FillRoundRect(g, accent, newX, y, pw, ph, (float)Sc(2));
}

void DrawSearchGlyph(HDC dc, int x, int cy, COLORREF c) {
    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::Pen pen(ToColor(c), (float)Sc(3) / 2);
    float r = (float)Sc(5);
    g.DrawEllipse(&pen, (float)x, cy - r - Sc(1), r * 2, r * 2);
    g.DrawLine(&pen, x + r * 1.7f, cy + r * 0.7f - Sc(1), x + r * 2.5f, cy + r * 1.5f - Sc(1));
}

void DrawChevron(HDC dc, int x, int cyInt, bool expanded, COLORREF c) {
    float cy = (float)cyInt;
    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::Pen pen(ToColor(c), (float)Sc(3) / 2);
    float s = (float)Sc(4);
    float cx = x + s;
    if (expanded) {
        g.DrawLine(&pen, cx - s, cy - s / 2, cx, cy + s / 2);
        g.DrawLine(&pen, cx, cy + s / 2, cx + s, cy - s / 2);
    } else {
        g.DrawLine(&pen, cx - s / 2, cy - s, cx + s / 2, cy);
        g.DrawLine(&pen, cx + s / 2, cy, cx - s / 2, cy + s);
    }
}

// Glyph for a bottom-bar button: a small "screen" with the layout drawn in.
void DrawActionGlyph(HDC dc, const RECT& r, Act act, const Theme& t, COLORREF accent) {
    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    float w = (float)Sc(20), h = (float)Sc(14);
    float x = (r.left + r.right) / 2.0f - w / 2, y = (r.top + r.bottom) / 2.0f - h / 2;
    float gap = (float)Sc(2), rad = (float)Sc(2);
    float sw = (float)Sc(3) / 2;

    auto cell = [&](float fx0, float fy0, float fx1, float fy1, COLORREF c) {
        float cx0 = x + w * fx0 + (fx0 > 0 ? gap / 2 : 0);
        float cy0 = y + h * fy0 + (fy0 > 0 ? gap / 2 : 0);
        float cx1 = x + w * fx1 - (fx1 < 1 ? gap / 2 : 0);
        float cy1 = y + h * fy1 - (fy1 < 1 ? gap / 2 : 0);
        FillRoundRect(g, c, cx0, cy0, cx1 - cx0, cy1 - cy0, rad);
    };
    auto outline = [&](float ox, float oy, float ow, float oh, COLORREF c) {
        StrokeRoundRect(g, c, sw, ox + sw / 2, oy + sw / 2, ow - sw, oh - sw, rad);
    };

    COLORREF c = t.text;
    switch (act) {
        case Act::Grid:
            for (int k = 0; k < 6; k++) {
                cell((k % 3) / 3.0f, (k / 3) / 2.0f, (k % 3 + 1) / 3.0f, (k / 3 + 1) / 2.0f,
                     k == 0 ? accent : c);
            }
            break;
        case Act::Cols2:
            cell(0, 0, 0.5f, 1, accent);
            cell(0.5f, 0, 1, 1, c);
            break;
        case Act::Cols3:
            cell(0, 0, 1 / 3.0f, 1, accent);
            cell(1 / 3.0f, 0, 2 / 3.0f, 1, c);
            cell(2 / 3.0f, 0, 1, 1, c);
            break;
        case Act::Quad:
            cell(0, 0, 0.5f, 0.5f, accent);
            cell(0.5f, 0, 1, 0.5f, c);
            cell(0, 0.5f, 0.5f, 1, c);
            cell(0.5f, 0.5f, 1, 1, c);
            break;
        case Act::MainSide:
            cell(0, 0, 0.6f, 1, accent);
            cell(0.6f, 0, 1, 0.5f, c);
            cell(0.6f, 0.5f, 1, 1, c);
            break;
        case Act::MinimizeOthers: {
            outline(x + w * 0.15f, y, w * 0.7f, h * 0.7f, c);
            FillRoundRect(g, accent, x + w * 0.2f, y + h - Sc(3), w * 0.6f, (float)Sc(3),
                          (float)Sc(1));
            break;
        }
        case Act::BringHere: {
            outline(x, y, w, h, c);
            float bx = x + w * 0.3f, by = y + h * 0.3f;
            FillRoundRect(g, accent, bx, by, w * 0.4f, h * 0.4f, (float)Sc(1));
            break;
        }
        case Act::CloseOthers: {
            outline(x + w * 0.1f, y + h * 0.15f, w * 0.6f, h * 0.7f, c);
            Gdiplus::Pen pen(ToColor(RGB(0xE8, 0x48, 0x3A)), (float)Sc(2));
            float ax = x + w * 0.82f, ay = y + h * 0.3f, s = (float)Sc(3);
            g.DrawLine(&pen, ax - s, ay - s, ax + s, ay + s);
            g.DrawLine(&pen, ax - s, ay + s, ax + s, ay - s);
            break;
        }
        case Act::SavedLayouts: {
            // Bookmark ribbon.
            float bw = w * 0.55f, bx = x + (w - bw) / 2;
            Gdiplus::PointF pts[] = {{bx, y},
                                     {bx + bw, y},
                                     {bx + bw, y + h + Sc(2)},
                                     {bx + bw / 2, y + h * 0.7f},
                                     {bx, y + h + Sc(2)}};
            Gdiplus::SolidBrush brush(ToColor(accent));
            g.FillPolygon(&brush, pts, 5);
            break;
        }
    }
}

const wchar_t* ActionHint(Act act) {
    switch (act) {
        case Act::Grid:
            return g_str->layGrid;
        case Act::Cols2:
            return g_str->layCols2;
        case Act::Cols3:
            return g_str->layCols3;
        case Act::Quad:
            return g_str->layQuad;
        case Act::MainSide:
            return g_str->layMainSide;
        case Act::MinimizeOthers:
            return g_str->actMinimize;
        case Act::BringHere:
            return g_str->actBring;
        case Act::CloseOthers:
            return g_str->actClose;
        case Act::SavedLayouts:
            return g_str->savedLayouts;
    }
    return L"";
}

void PaintHeader(HDC dc, Panel& p, const Theme& t) {
    RECT h = p.headerRect;
    int cy = (h.top + h.bottom) / 2;
    int x = h.left + Sc(8);

    if (!p.filter.empty()) {
        DrawSearchGlyph(dc, x, cy, t.text);
        x += Sc(20);
        // Open windows only: apps and files have their own counts.
        int matches = 0;
        for (auto& s : p.secs) {
            if (s.kind == EntryWindow) matches += s.matches;
        }
        std::wstring count = Format(g_str->matchFmt, matches, (int)p.all.size());
        int cw = TextWidth(dc, g_fontSmall, count);
        DrawTextIn(dc, g_fontSmall, t.subtext, count,
                   {h.right - Sc(8) - cw, h.top, h.right - Sc(8), h.bottom},
                   DT_RIGHT | DT_VCENTER);
        RECT tr{x, h.top, h.right - Sc(16) - cw, h.bottom};
        DrawTextIn(dc, g_fontBold, t.text, p.filter, tr, DT_LEFT | DT_VCENTER);
        int tw = std::min(TextWidth(dc, g_fontBold, p.filter), (int)(tr.right - tr.left));
        RECT caret{x + tw + Sc(2), cy - Sc(8), x + tw + Sc(4), cy + Sc(8)};
        FillSolid(dc, caret, t.text);
        return;
    }
    if (p.namingLayout) return;  // the name editor sits here

    if (p.appIcon && !p.showAll) {
        DrawIconEx(dc, x, cy - Sc(8), p.appIcon, Sc(16), Sc(16), 0, nullptr, DI_NORMAL);
        x += Sc(16) + Sc(8);
    }
    int n = (int)p.all.size();
    std::wstring count = p.showAll ? Format(g_str->windowsFmt, n)
                         : n == 1  ? std::wstring(g_str->oneOther)
                                   : Format(g_str->otherFmt, n);
    if (n > 6) count += g_str->typeToSearch;
    if (p.hotPair) {
        count = p.pairToRight ? g_str->pairRight : g_str->pairLeft;
    } else if (p.hotFooter >= 0 && p.hotFooter < (int)p.footer.size()) {
        count = ActionHint(p.footer[p.hotFooter].act);
    }
    int cw = std::min(TextWidth(dc, g_fontSmall, count), (int)(h.right - h.left) * 2 / 3);
    DrawTextIn(dc, g_fontSmall, t.subtext, count,
               {h.right - Sc(8) - cw, h.top, h.right - Sc(8), h.bottom}, DT_RIGHT | DT_VCENTER);
    DrawTextIn(dc, g_fontSmallBold, t.subtext, p.appName,
               {x, h.top, h.right - Sc(16) - cw, h.bottom}, DT_LEFT | DT_VCENTER);
}

void PaintSectionHeader(HDC dc, Panel& p, int s, const Theme& t) {
    const Section& sec = p.secs[s];
    RECT r = ShiftRect(sec.headerRect, -p.scrollY);
    int cy = (r.top + r.bottom) / 2;
    if (p.hotHeader == s) {
        Gdiplus::Graphics g(dc);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        FillRoundRect(g, t.hover, (float)r.left + Sc(2), (float)r.top + Sc(2),
                      (float)(r.right - r.left - Sc(4)), (float)(r.bottom - r.top - Sc(4)),
                      (float)Sc(5));
    }
    int x = r.left + Sc(8);
    bool expanded = !sec.collapsed || !p.filter.empty();
    DrawChevron(dc, x, cy, expanded, t.subtext);
    x += Sc(16);
    if (sec.icon) {
        DrawIconEx(dc, x, cy - Sc(8), sec.icon, Sc(16), Sc(16), 0, nullptr, DI_NORMAL);
        x += Sc(16) + Sc(8);
    }
    std::wstring count = p.filter.empty() ? std::to_wstring(sec.total)
                                          : Format(g_str->matchFmt, sec.matches, sec.total);
    int cw = TextWidth(dc, g_fontSmall, count);
    DrawTextIn(dc, g_fontSmall, t.subtext, count, {r.right - Sc(8) - cw, r.top, r.right - Sc(8), r.bottom},
               DT_RIGHT | DT_VCENTER);
    DrawTextIn(dc, g_fontSmallBold, sec.current ? t.text : t.subtext, sec.name,
               {x, r.top, r.right - Sc(16) - cw, r.bottom}, DT_LEFT | DT_VCENTER);
}

void PaintSelection(Gdiplus::Graphics& g, const RECT& r, COLORREF color, float radius) {
    FillRoundRect(g, color, (float)r.left + Sc(2), (float)r.top + Sc(1),
                  (float)(r.right - r.left - Sc(4)), (float)(r.bottom - r.top - Sc(2)),
                  radius, 48);
    float bw = (float)Sc(3) / 2;
    StrokeRoundRect(g, color, bw, r.left + Sc(2) + bw / 2, r.top + Sc(1) + bw / 2,
                    (float)(r.right - r.left - Sc(4)) - bw,
                    (float)(r.bottom - r.top - Sc(2)) - bw, radius);
}

void PaintListItem(HDC dc, Panel& p, int i, const Theme& t) {
    const Entry& e = p.entries[i];
    RECT r = ItemClientRect(p, i);
    bool hot = p.hot == i;
    bool editing = p.editIndex == i;
    bool selected = IsSelected(p, e.hwnd);
    int cy = (r.top + r.bottom) / 2;
    {
        Gdiplus::Graphics g(dc);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        if (selected) PaintSelection(g, r, e.color, (float)Sc(5));
        if (hot || editing) {
            FillRoundRect(g, t.hover, (float)r.left + Sc(2), (float)r.top + Sc(1),
                          (float)(r.right - r.left - Sc(4)), (float)(r.bottom - r.top - Sc(2)),
                          (float)Sc(5), selected ? 160 : 255);
            FillRoundRect(g, e.color, (float)r.left + Sc(2), (float)cy - Sc(9), (float)Sc(3),
                          (float)Sc(18), (float)Sc(3) / 2);
        }
    }
    DrawEntryBadge(dc, r.left + Sc(12), cy, Sc(24), e);

    RECT tr = ShiftRect(p.textRects[i], -p.scrollY);
    bool windowEntry = IsWindowEntry(e);
    if (g_closeButton && hot && !editing && windowEntry) {
        DrawCloseGlyph(dc, ShiftRect(p.closeRects[i], -p.scrollY), p.hotClose, t);
    }
    if (g_pairButton && p.source && hot && !editing && windowEntry) {
        DrawPairGlyph(dc, ShiftRect(p.pairRects[i], -p.scrollY), p.hotPair, p.pairToRight,
                      e.color, t);
    }
    if (!windowEntry) tr.right = ItemClientRect(p, i).right - Sc(12);
    if (e.minimized && !editing) {
        int mw = TextWidth(dc, g_fontSmall, g_str->minimizedTag);
        DrawTextIn(dc, g_fontSmall, t.subtext, g_str->minimizedTag,
                   {tr.right - mw, tr.top, tr.right, tr.bottom}, DT_RIGHT | DT_VCENTER);
        tr.right -= mw + Sc(12);
    }
    if (editing) {
        RECT line{tr.left, cy + Sc(12), tr.right, cy + Sc(14)};
        FillSolid(dc, line, e.color);
    } else if (!e.label.empty()) {
        DrawTextIn(dc, g_fontBold, t.text, e.label,
                   {tr.left, r.top + Sc(4), tr.right, cy + Sc(1)}, DT_LEFT | DT_BOTTOM);
        DrawTextIn(dc, g_fontSmall, t.subtext, e.title,
                   {tr.left, cy + Sc(1), tr.right, r.bottom - Sc(4)}, DT_LEFT | DT_TOP);
    } else {
        DrawTextIn(dc, g_font, t.text, e.title, tr, DT_LEFT | DT_VCENTER);
    }
}

void PaintGridTile(HDC dc, Panel& p, int i, const Theme& t) {
    const Entry& e = p.entries[i];
    RECT r = ItemClientRect(p, i);
    bool hot = p.hot == i;
    bool editing = p.editIndex == i;
    bool selected = IsSelected(p, e.hwnd);
    RECT a = ShiftRect(p.thumbAreas[i], -p.scrollY);
    {
        Gdiplus::Graphics g(dc);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        FillRoundRect(g, hot ? t.hover : t.card, (float)r.left, (float)r.top,
                      (float)(r.right - r.left), (float)(r.bottom - r.top), (float)Sc(8));
        if (selected) {
            FillRoundRect(g, e.color, (float)r.left, (float)r.top, (float)(r.right - r.left),
                          (float)(r.bottom - r.top), (float)Sc(8), 40);
        }
        if (hot || selected) {
            float bw = (float)Sc(2);
            StrokeRoundRect(g, e.color, bw, r.left + bw / 2, r.top + bw / 2,
                            (float)(r.right - r.left) - bw, (float)(r.bottom - r.top) - bw,
                            (float)Sc(8));
        }
        FillRoundRect(g, t.thumbBg, (float)a.left, (float)a.top, (float)(a.right - a.left),
                      (float)(a.bottom - a.top), (float)Sc(4));
    }
    bool hasThumb = i < (int)p.thumbs.size() && p.thumbs[i];
    if (!hasThumb) {
        int is = Sc(32);
        int cx = (a.left + a.right) / 2, cy = (a.top + a.bottom) / 2;
        if (e.kind == EntryApp) {
            DrawBitmapIcon(dc, e.bitmap, cx - is / 2, cy - is / 2 - Sc(8), is);
        } else if (e.icon) {
            DrawIconEx(dc, cx - is / 2, cy - is / 2 - Sc(8), e.icon, is, is, 0, nullptr,
                       DI_NORMAL);
        }
        const wchar_t* note = e.kind == EntryApp    ? g_str->installedApps
                              : e.kind == EntryFile ? L"Everything"
                              : e.minimized         ? g_str->minimized
                                                    : g_str->noPreview;
        DrawTextIn(dc, g_fontSmall, t.subtext, note, {a.left, cy + Sc(14), a.right, cy + Sc(32)},
                   DT_CENTER | DT_VCENTER);
    }
    RECT tr = ShiftRect(p.textRects[i], -p.scrollY);
    int cy = (tr.top + tr.bottom) / 2;
    DrawEntryBadge(dc, r.left + Sc(8), cy, Sc(20), e);
    bool windowEntry = IsWindowEntry(e);
    if (g_closeButton && hot && !editing && windowEntry) {
        DrawCloseGlyph(dc, ShiftRect(p.closeRects[i], -p.scrollY), p.hotClose, t);
    }
    if (g_pairButton && p.source && hot && !editing && windowEntry) {
        DrawPairGlyph(dc, ShiftRect(p.pairRects[i], -p.scrollY), p.hotPair, p.pairToRight,
                      e.color, t);
    }
    if (editing) {
        RECT line{tr.left, cy + Sc(11), tr.right, cy + Sc(13)};
        FillSolid(dc, line, e.color);
    } else {
        bool lab = !e.label.empty();
        DrawTextIn(dc, lab ? g_fontBold : g_font, t.text, lab ? e.label : e.title, tr,
                   DT_LEFT | DT_VCENTER);
    }
}

void PaintScrollbar(HDC dc, Panel& p, const Theme& t) {
    p.scrollThumb = {};
    if (p.maxScroll <= 0) return;
    RECT lr = p.listRect;
    int trackTop = lr.top + Sc(4), trackH = (lr.bottom - lr.top) - Sc(8);
    int visibleH = lr.bottom - lr.top;
    int thumbH = std::max(Sc(28), trackH * visibleH / std::max(1, p.contentH));
    int thumbY = trackTop + (trackH - thumbH) * p.scrollY / p.maxScroll;
    bool wide = p.hotScrollbar || p.dragThumb;
    int w = wide ? Sc(6) : Sc(3);
    int x = lr.right - Sc(5) - w / 2;
    p.scrollThumb = {lr.right - Sc(14), thumbY, lr.right, thumbY + thumbH};

    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    if (wide) {
        FillRoundRect(g, t.subtext, (float)x, (float)trackTop, (float)w, (float)trackH,
                      (float)w / 2, 40);
    }
    FillRoundRect(g, t.subtext, (float)x, (float)thumbY, (float)w, (float)thumbH,
                  (float)w / 2, wide ? 230 : 150);
}

void PaintEdgeFades(HDC dc, Panel& p, const Theme& t) {
    // Gradients would turn into opaque bands over a see-through material.
    if (p.maxScroll <= 0 || p.grid || p.glass) return;
    Gdiplus::Graphics g(dc);
    RECT lr = p.listRect;
    float fh = (float)Sc(18);
    float w = (float)(lr.right - lr.left - Sc(12));
    if (p.scrollY > 0) {
        Gdiplus::LinearGradientBrush b(Gdiplus::PointF(0, (float)lr.top - 1),
                                       Gdiplus::PointF(0, lr.top + fh), ToColor(t.bg, 255),
                                       ToColor(t.bg, 0));
        g.FillRectangle(&b, (float)lr.left, (float)lr.top, w, fh);
    }
    if (p.scrollY < p.maxScroll) {
        Gdiplus::LinearGradientBrush b(Gdiplus::PointF(0, lr.bottom - fh),
                                       Gdiplus::PointF(0, (float)lr.bottom + 1),
                                       ToColor(t.bg, 0), ToColor(t.bg, 255));
        g.FillRectangle(&b, (float)lr.left, lr.bottom - fh, w, fh);
    }
}


void PaintFooter(HDC dc, Panel& p, const Theme& t) {
    if (p.footer.empty()) return;
    RECT r = p.footerRect;
    RECT sep{r.left + Sc(6), r.top - Sc(4), r.right - Sc(6), r.top - Sc(3)};
    FillSolid(dc, sep, t.border);
    COLORREF accent = AccentColor();
    int rightmost = r.left;
    int layoutsLeft = r.right;
    for (int i = 0; i < (int)p.footer.size(); i++) {
        const FooterButton& b = p.footer[i];
        if (p.hotFooter == i) {
            Gdiplus::Graphics g(dc);
            g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            FillRoundRect(g, t.hover, (float)b.rect.left, (float)b.rect.top,
                          (float)(b.rect.right - b.rect.left),
                          (float)(b.rect.bottom - b.rect.top), (float)Sc(5));
        }
        DrawActionGlyph(dc, b.rect, b.act, t, accent);
        if (b.act == Act::SavedLayouts) {
            layoutsLeft = b.rect.left;
        } else {
            rightmost = std::max(rightmost, (int)b.rect.right);
        }
    }
    if (!p.selected.empty()) {
        std::wstring text = Format(g_str->selectedFmt, (int)p.selected.size());
        DrawTextIn(dc, g_fontSmallBold, accent, text,
                   {rightmost + Sc(10), r.top, layoutsLeft - Sc(8), r.bottom},
                   DT_RIGHT | DT_VCENTER);
    }
}

void PaintPanelContent(HDC dc, Panel& p, const Theme& t, const RECT& rc,
                       COLORREF background) {
    FillSolid(dc, rc, background);

    PaintHeader(dc, p, t);

    int saved = SaveDC(dc);
    IntersectClipRect(dc, p.listRect.left, p.listRect.top, p.listRect.right,
                      p.listRect.bottom);
    bool anyMatch = false;
    for (auto& s : p.secs) anyMatch |= s.matches > 0;
    if (!anyMatch) {
        DrawTextIn(dc, g_font, t.subtext, g_str->noMatches, p.listRect,
                   DT_CENTER | DT_VCENTER);
    }
    if (HasSectionHeaders(p)) {
        for (int s = 0; s < (int)p.secs.size(); s++) {
            if (p.secs[s].matches > 0) PaintSectionHeader(dc, p, s, t);
        }
    }
    for (int i = 0; i < (int)p.entries.size(); i++) {
        RECT r = ItemClientRect(p, i);
        if (r.bottom < p.listRect.top || r.top > p.listRect.bottom) continue;
        if (p.grid) {
            PaintGridTile(dc, p, i, t);
        } else {
            PaintListItem(dc, p, i, t);
        }
    }
    PaintEdgeFades(dc, p, t);
    PaintScrollbar(dc, p, t);
    RestoreDC(dc, saved);

    PaintFooter(dc, p, t);
}

void PaintPanel(Panel& p) {
    PAINTSTRUCT ps;
    HDC wdc = BeginPaint(p.hwnd, &ps);
    RECT rc;
    GetClientRect(p.hwnd, &rc);
    Theme t = GetTheme();
    auto paint = [&](HDC dc, COLORREF background) {
        PaintPanelContent(dc, p, t, rc, background);
    };
    if (p.glass) {
        PaintOverMaterial(wdc, rc.right, rc.bottom, t, paint);
    } else {
        PaintBuffer pb;
        if (BeginPaintBuffer(wdc, rc.right, rc.bottom, &pb)) {
            paint(pb.dc, t.bg);
            EndPaintBuffer(wdc, pb);
        }
    }
    EndPaint(p.hwnd, &ps);
}

// ===========================================================================
// Panel interaction
// ===========================================================================

struct PanelHit {
    int index = -1;
    bool pair = false;
    bool close = false;
    bool scrollbar = false;
    int header = -1;
    int footer = -1;
};

PanelHit HitTestPanel(const Panel& p, POINT pt) {
    PanelHit h;
    for (int i = 0; i < (int)p.footer.size(); i++) {
        if (PtInRect(&p.footer[i].rect, pt)) {
            h.footer = i;
            return h;
        }
    }
    if (!PtInRect(&p.listRect, pt)) return h;
    if (p.maxScroll > 0 && pt.x >= p.listRect.right - Sc(14)) {
        h.scrollbar = true;
        return h;
    }
    POINT cp{pt.x, pt.y + p.scrollY};
    if (HasSectionHeaders(p)) {
        for (int s = 0; s < (int)p.secs.size(); s++) {
            if (p.secs[s].matches > 0 && PtInRect(&p.secs[s].headerRect, cp)) {
                h.header = s;
                return h;
            }
        }
    }
    for (int i = 0; i < (int)p.itemRects.size(); i++) {
        if (PtInRect(&p.itemRects[i], cp)) {
            h.index = i;
            bool windowEntry = IsWindowEntry(p.entries[i]);
            h.close = windowEntry && g_closeButton && PtInRect(&p.closeRects[i], cp);
            h.pair = windowEntry && g_pairButton && p.source && PtInRect(&p.pairRects[i], cp);
            return h;
        }
    }
    return h;
}

void FinishPanel(Panel& p, PanelResult r, int index = -1) {
    if (p.done) return;
    p.done = true;
    p.result = r;
    p.resultIndex = index;
}

void SetHot(Panel& p, const PanelHit& h) {
    if (p.hot == h.index && p.hotClose == h.close && p.hotPair == h.pair &&
        p.hotScrollbar == h.scrollbar && p.hotHeader == h.header && p.hotFooter == h.footer) {
        return;
    }
    p.hot = h.index;
    p.hotClose = h.close;
    p.hotPair = h.pair;
    p.hotScrollbar = h.scrollbar;
    p.hotHeader = h.header;
    p.hotFooter = h.footer;
    InvalidateRect(p.hwnd, nullptr, FALSE);
    ShowPreview(h.index);
}

void SetHotItem(Panel& p, int index) {
    PanelHit h;
    h.index = index;
    SetHot(p, h);
}

void RefreshHotFromCursor(Panel& p) {
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(p.hwnd, &pt);
    RECT rc;
    GetClientRect(p.hwnd, &rc);
    if (!PtInRect(&rc, pt)) return;
    SetHot(p, HitTestPanel(p, pt));
}

void ApplyScroll(Panel& p, int y) {
    y = Clamp(y, 0, p.maxScroll);
    if (y == p.scrollY) return;
    p.scrollY = y;
    if (p.edit && p.editIndex >= 0) {
        RECT tr = ShiftRect(p.textRects[p.editIndex], -p.scrollY);
        int cy = (tr.top + tr.bottom) / 2;
        SetWindowPos(p.edit, nullptr, tr.left, cy - Sc(11), 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    UpdateThumbs(p);
    InvalidateRect(p.hwnd, nullptr, FALSE);
}

// Smoothly scroll towards the target.
void ScrollTo(Panel& p, int target) {
    p.scrollTarget = Clamp(target, 0, p.maxScroll);
    if (p.scrollTarget != p.scrollY) SetTimer(p.hwnd, kScrollAnimTimerId, 15, nullptr);
}

void OnScrollAnimTick(Panel& p) {
    int diff = p.scrollTarget - p.scrollY;
    if (diff == 0) {
        KillTimer(p.hwnd, kScrollAnimTimerId);
        RefreshHotFromCursor(p);
        return;
    }
    int step = (int)(diff * 0.35);
    if (step == 0) step = diff > 0 ? 1 : -1;
    ApplyScroll(p, p.scrollY + step);
}

void EnsureVisible(Panel& p, int i) {
    if (i < 0 || i >= (int)p.itemRects.size() || p.maxScroll <= 0) return;
    int top = p.itemRects[i].top - p.listRect.top;
    int bottom = p.itemRects[i].bottom - p.listRect.top;
    // Keep the section header visible above the first entry of a section.
    if (HasSectionHeaders(p)) {
        const Section& sec = p.secs[p.entries[i].section];
        if (sec.first == i) top = sec.headerRect.top - p.listRect.top;
    }
    int visibleH = p.listRect.bottom - p.listRect.top;
    int target = p.scrollTarget;
    if (top < target) target = top - Sc(4);
    if (bottom > target + visibleH) target = bottom - visibleH + Sc(4);
    ScrollTo(p, target);
}

void RelayoutPanel(Panel& p, bool keepPosition = true) {
    HidePreview();
    SIZE sz = LayoutPanel(p);
    RECT wr;
    GetWindowRect(p.hwnd, &wr);
    MONITORINFO mi = MonitorInfoFromPoint(p.origin);
    int x = Clamp(wr.left, mi.rcWork.left, mi.rcWork.right - sz.cx);
    int y = Clamp(wr.top, mi.rcWork.top, mi.rcWork.bottom - sz.cy);
    if (!keepPosition && p.hotkeyMode) {
        x = (mi.rcWork.left + mi.rcWork.right - sz.cx) / 2;
        y = Clamp(mi.rcWork.top + (mi.rcWork.bottom - mi.rcWork.top - sz.cy) / 3,
                  mi.rcWork.top, mi.rcWork.bottom - sz.cy);
    }
    SetWindowPos(p.hwnd, nullptr, x, y, sz.cx, sz.cy, SWP_NOZORDER | SWP_NOACTIVATE);
    RegisterThumbs(p);
    SetHot(p, PanelHit{});
    InvalidateRect(p.hwnd, nullptr, FALSE);
}

HBITMAP CachedAppBitmap(Panel& p, const std::wstring& id) {
    for (auto& b : p.appBitmaps) {
        if (b.first == id) return b.second;
    }
    HBITMAP bmp = LoadAppBitmap(id, Sc(32));
    p.appBitmaps.push_back({id, bmp});
    return bmp;
}

HICON CachedFileIcon(Panel& p, const std::wstring& path) {
    for (auto& f : p.fileIcons) {
        if (f.first == path) return f.second;
    }
    HICON icon = LoadFileIcon(path);
    p.fileIcons.push_back({path, icon});
    return icon;
}

// Rebuilds the visible entries from the search text and collapsed sections,
// plus the launcher suggestions (installed apps, Everything results).
void BuildEntries(Panel& p) {
    std::wstring f = ToLower(Trim(p.filter));
    p.entries.clear();
    for (int s = 0; s < (int)p.secs.size(); s++) {
        Section& sec = p.secs[s];
        if (sec.kind != EntryWindow) continue;
        sec.matches = 0;
        sec.first = (int)p.entries.size();
        for (auto& e : p.all) {
            if (e.section != s) continue;
            if (!f.empty() && ToLower(e.title).find(f) == std::wstring::npos &&
                ToLower(e.label).find(f) == std::wstring::npos &&
                ToLower(sec.name).find(f) == std::wstring::npos) {
                continue;
            }
            sec.matches++;
            // While searching, collapsed sections open up.
            if (!sec.collapsed || !f.empty()) p.entries.push_back(e);
        }
        sec.count = (int)p.entries.size() - sec.first;
    }

    if (p.appSection >= 0) {
        Section& sec = p.secs[p.appSection];
        sec.first = (int)p.entries.size();
        sec.matches = 0;
        if (!f.empty()) {
            // Don't suggest apps that already have a window section.
            std::vector<std::wstring> exclude;
            for (auto& other : p.secs) {
                if (other.kind == EntryWindow) exclude.push_back(ToLower(other.name));
            }
            for (auto& a : SearchApps(f, (size_t)g_maxApps.load(), exclude)) {
                Entry e{nullptr, a.name, L"", a.id, 0, 0, AccentColor(), false, nullptr,
                        false, p.appSection, false, 100000};
                e.kind = EntryApp;
                e.bitmap = CachedAppBitmap(p, a.id);
                p.entries.push_back(e);
                sec.matches++;
            }
        }
        sec.total = sec.count = sec.matches;
    }

    if (p.fileSection >= 0) {
        Section& sec = p.secs[p.fileSection];
        sec.first = (int)p.entries.size();
        sec.matches = 0;
        if (!f.empty()) {
            for (auto& r : p.fileResults) {
                Entry e{nullptr, r.folder, r.name, r.fullPath, 0, 0, AccentColor(), false,
                        CachedFileIcon(p, r.fullPath), false, p.fileSection, false, 100000};
                e.kind = EntryFile;
                p.entries.push_back(e);
                sec.matches++;
            }
            if (sec.matches > 0 && p.fileTotal > (DWORD)sec.matches) {
                Entry more{nullptr, Format(g_str->showAllFmt, (int)p.fileTotal), L"",
                           p.fileQuery, 0, 0, AccentColor(), false, nullptr, false,
                           p.fileSection, false, 100001};
                more.kind = EntryMore;
                p.entries.push_back(more);
            }
        }
        sec.total = std::max((int)p.fileTotal, sec.matches);
        sec.count = (int)p.entries.size() - sec.first;
    }
}

// Asks Everything for files matching the search (results arrive later).
void QueryEverything(Panel& p) {
    if (p.fileSection < 0) return;
    std::wstring f = Trim(p.filter);
    if (f == p.fileQuery) return;
    p.fileQuery = f;
    if (f.size() < 2) {
        p.fileResults.clear();
        p.fileTotal = 0;
        return;
    }
    if (!SendEverythingQuery(p.hwnd, f, (DWORD)g_maxFiles.load())) {
        p.fileResults.clear();
        p.fileTotal = 0;
    }
}

void ApplyFilter(Panel& p, bool resetScroll = true) {
    QueryEverything(p);
    BuildEntries(p);
    if (resetScroll) {
        p.scrollY = p.scrollTarget = 0;
        KillTimer(p.hwnd, kScrollAnimTimerId);
    }
    RelayoutPanel(p);
    if (!p.filter.empty() && !p.entries.empty()) SetHotItem(p, 0);
}

void ToggleSection(Panel& p, int s) {
    Section& sec = p.secs[s];
    sec.collapsed = !sec.collapsed;
    auto it = std::find(g_collapsedApps.begin(), g_collapsedApps.end(), sec.key);
    if (sec.collapsed && it == g_collapsedApps.end()) g_collapsedApps.push_back(sec.key);
    if (!sec.collapsed && it != g_collapsedApps.end()) g_collapsedApps.erase(it);
    ApplyFilter(p, false);
    RefreshHotFromCursor(p);
}

void EndRename(Panel& p, bool commit);

void RemoveEntry(Panel& p, HWND hwnd) {
    p.selected.erase(std::remove(p.selected.begin(), p.selected.end(), hwnd),
                     p.selected.end());
    for (size_t k = 0; k < p.all.size(); k++) {
        if (p.all[k].hwnd == hwnd) {
            int s = p.all[k].section;
            if (p.all[k].ownsIcon && p.all[k].icon) {
                // The section header may share this icon.
                if (p.secs[s].icon == p.all[k].icon) p.secs[s].icon = nullptr;
                DestroyIcon(p.all[k].icon);
            }
            p.secs[s].total--;
            p.all.erase(p.all.begin() + k);
            break;
        }
    }
}

void CloseEntry(Panel& p, int i) {
    if (i < 0 || i >= (int)p.entries.size()) return;
    if (p.edit) EndRename(p, false);
    HWND hwnd = p.entries[i].hwnd;
    Wh_Log(L"Closing %p", hwnd);
    PostMessageW(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
    RemoveEntry(p, hwnd);
    if (p.all.empty()) {
        FinishPanel(p, PanelResult::None);
        return;
    }
    ApplyFilter(p, false);
    RefreshHotFromCursor(p);
}

LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Panel* p = g_panel;
    WNDPROC old = p ? p->editOldProc : nullptr;
    switch (msg) {
        case WM_KEYDOWN:
            if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
                if (p) PostMessageW(p->hwnd, WM_APP_EDIT_DONE, wParam == VK_RETURN, 0);
                return 0;
            }
            break;
        case WM_CHAR:
            if (wParam == L'\r' || wParam == 27) return 0;
            break;
        case WM_KILLFOCUS:
            if (p) PostMessageW(p->hwnd, WM_APP_EDIT_DONE, 1, 0);
            break;
    }
    return old ? CallWindowProcW(old, hwnd, msg, wParam, lParam)
               : DefWindowProcW(hwnd, msg, wParam, lParam);
}

HWND CreateInlineEdit(Panel& p, const RECT& r, const std::wstring& text) {
    HWND edit = CreateWindowExW(0, L"EDIT", text.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                r.left, r.top, r.right - r.left, r.bottom - r.top, p.hwnd,
                                nullptr, g_instance, nullptr);
    if (!edit) return nullptr;
    if (p.glass) {
        // Over a see-through material, GDI-painted controls would turn
        // transparent; a layered child keeps the edit box opaque.
        SetWindowLongPtrW(edit, GWL_EXSTYLE, GetWindowLongPtrW(edit, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(edit, 0, 255, LWA_ALPHA);
    }
    SendMessageW(edit, WM_SETFONT, (WPARAM)g_font, FALSE);
    SendMessageW(edit, EM_SETSEL, 0, -1);
    p.editOldProc = (WNDPROC)SetWindowLongPtrW(edit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
    SetFocus(edit);
    return edit;
}

void BeginRename(Panel& p, int i) {
    if (!g_renaming || i < 0 || i >= (int)p.entries.size()) return;
    if (p.edit) EndRename(p, true);
    HidePreview();
    EnsureVisible(p, i);
    p.scrollY = p.scrollTarget;  // jump, so the editor lands in the right place
    UpdateThumbs(p);
    const Entry& e = p.entries[i];
    RECT tr = ShiftRect(p.textRects[i], -p.scrollY);
    int cy = (tr.top + tr.bottom) / 2;
    p.editIndex = i;
    p.edit = CreateInlineEdit(p, {tr.left, cy - Sc(11), tr.right, cy + Sc(11)},
                              e.label.empty() ? e.title : e.label);
    if (!p.edit) p.editIndex = -1;
    InvalidateRect(p.hwnd, nullptr, FALSE);
}

void BeginLayoutNaming(Panel& p) {
    if (p.edit) EndRename(p, true);
    HidePreview();
    p.filter.clear();
    std::vector<SavedLayout> layouts = LoadSavedLayouts();
    std::wstring name = Format(g_str->layoutNameFmt, (int)layouts.size() + 1);
    RECT h = p.headerRect;
    int cy = (h.top + h.bottom) / 2;
    p.namingLayout = true;
    p.edit = CreateInlineEdit(p, {h.left + Sc(8), cy - Sc(11), h.right - Sc(8), cy + Sc(11)},
                              name);
    if (!p.edit) p.namingLayout = false;
    InvalidateRect(p.hwnd, nullptr, FALSE);
}

void RefreshTags();

void EndRename(Panel& p, bool commit) {
    if (!p.edit) return;
    HWND edit = p.edit;
    int i = p.editIndex;
    bool naming = p.namingLayout;
    p.edit = nullptr;
    p.editIndex = -1;
    p.namingLayout = false;
    std::wstring text = Trim(GetWindowTitle(edit));
    SetWindowLongPtrW(edit, GWLP_WNDPROC, (LONG_PTR)p.editOldProc);
    DestroyWindow(edit);

    if (naming) {
        if (commit && !text.empty()) SaveCurrentLayout(text);
    } else if (commit && i >= 0 && i < (int)p.entries.size()) {
        Entry& e = p.entries[i];
        if (text == e.title) text.clear();
        e.label = text;
        for (auto& x : p.all) {
            if (x.hwnd == e.hwnd) x.label = text;
        }
        SetLabel(e.hwnd, text);
        Wh_Log(L"Renamed %p to '%s'", e.hwnd, text.c_str());
        RefreshTags();
    }
    if (IsWindow(p.hwnd) && !p.done) SetFocus(p.hwnd);
    InvalidateRect(p.hwnd, nullptr, FALSE);
}

void MoveHot(Panel& p, int delta) {
    int n = (int)p.entries.size();
    if (!n) return;
    int h = p.hot < 0 ? (delta > 0 ? 0 : n - 1) : ((p.hot + delta) % n + n) % n;
    SetHotItem(p, h);
    EnsureVisible(p, h);
}

// Tab / Shift+Tab walk the visible entries in most-recently-used order.
void MoveHotMru(Panel& p, int delta) {
    int n = (int)p.entries.size();
    if (!n) return;
    std::vector<int> order(n);
    for (int i = 0; i < n; i++) order[i] = i;
    std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
        return p.entries[a].mru < p.entries[b].mru;
    });
    int pos = -1;
    for (int k = 0; k < n; k++) {
        if (order[k] == p.hot) pos = k;
    }
    int next = pos < 0 ? (delta > 0 ? 0 : n - 1) : ((pos + delta) % n + n) % n;
    SetHotItem(p, order[next]);
    EnsureVisible(p, order[next]);
}

void OnPanelTick(Panel& p) {
    if (p.done) return;
    if (!p.wasForeground && GetForegroundWindow() == p.hwnd) {
        p.wasForeground = true;  // e.g. the user clicked the panel
    }
    if (p.edit || p.dragThumb || p.menuOpen) return;
    POINT pt;
    if (!GetCursorPos(&pt)) return;
    RECT origin{p.origin.x - Sc(40), p.origin.y - Sc(30), p.origin.x + Sc(40),
                p.origin.y + Sc(40)};
    bool inside = PtInInflatedWindow(p.hwnd, pt, Sc(16)) ||
                  PtInInflatedWindow(g_previewHwnd, pt, Sc(16)) ||
                  (!p.hotkeyMode && PtInRect(&origin, pt));
    if (inside) {
        p.outsideSince = 0;
        return;
    }
    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
        FinishPanel(p, PanelResult::None);  // click outside
        return;
    }
    // The hotkey panel isn't tied to the mouse; keep it while searching too.
    if (!g_autoClose || p.hotkeyMode || !p.filter.empty() || !p.selected.empty()) return;
    ULONGLONG now = GetTickCount64();
    if (!p.outsideSince) p.outsideSince = now;
    if (now - p.outsideSince >= (ULONGLONG)g_autoCloseDelay.load()) {
        FinishPanel(p, PanelResult::None);
    }
}

void DragScrollTo(Panel& p, int mouseY) {
    int trackH = (p.listRect.bottom - p.listRect.top) - Sc(8);
    int thumbH = p.scrollThumb.bottom - p.scrollThumb.top;
    int range = std::max(1, trackH - thumbH);
    int y = p.dragStartScroll + (mouseY - p.dragStartY) * p.maxScroll / range;
    p.scrollTarget = Clamp(y, 0, p.maxScroll);
    ApplyScroll(p, p.scrollTarget);
}

// Shows a menu from the panel without the panel closing or the previews
// covering it.
UINT TrackPanelMenu(Panel& p, HMENU menu, POINT screenPt) {
    p.menuOpen = true;
    HidePreview();
    UpdateThumbs(p);
    UINT cmd = (UINT)TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, screenPt.x,
                                      screenPt.y, p.hwnd, nullptr);
    p.menuOpen = false;
    UpdateThumbs(p);
    p.outsideSince = 0;
    return cmd;
}

// Menus run their own message loop, during which the list can change (e.g.
// Everything results arrive): find the entry again afterwards.
int FindEntry(const Panel& p, int kind, HWND hwnd, const std::wstring& app) {
    for (int k = 0; k < (int)p.entries.size(); k++) {
        const Entry& e = p.entries[k];
        if (e.kind == kind && e.hwnd == hwnd && e.app == app) return k;
    }
    return -1;
}

void ShowFileMenu(Panel& p, int i, POINT screenPt) {
    enum : UINT { kOpen = 1, kLocation, kCopy };
    std::wstring path = p.entries[i].app;
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, kOpen, g_str->menuOpen);
    SetMenuDefaultItem(menu, kOpen, FALSE);
    AppendMenuW(menu, MF_STRING, kLocation, g_str->menuOpenLocation);
    AppendMenuW(menu, MF_STRING, kCopy, g_str->menuCopyPath);
    UINT cmd = TrackPanelMenu(p, menu, screenPt);
    DestroyMenu(menu);
    if (cmd == kOpen) {
        i = FindEntry(p, EntryFile, nullptr, path);
        if (i >= 0) {
            FinishPanel(p, PanelResult::Select, i);
        } else {
            ShellOpen(path);
            FinishPanel(p, PanelResult::None);
        }
    } else if (cmd == kLocation) {
        OpenFileLocation(path);
        FinishPanel(p, PanelResult::None);
    } else if (cmd == kCopy) {
        CopyTextToClipboard(p.hwnd, path);
    }
}

void ShowEntryMenu(Panel& p, int i, POINT screenPt) {
    if (i < 0 || i >= (int)p.entries.size()) return;
    if (p.entries[i].kind == EntryFile) {
        ShowFileMenu(p, i, screenPt);
        return;
    }
    if (p.entries[i].kind != EntryWindow) return;
    HWND hwnd = p.entries[i].hwnd;
    enum : UINT {
        kOpen = 1,
        kPair,
        kTopmost,
        kSelect,
        kRename,
        kMinimize,
        kClose,
        kMonitorBase = 100,
        kOpacityBase = 200,
    };
    bool activateOnly = !p.source || (p.hotkeyMode && g_hotkeyActivate);
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, kOpen, activateOnly ? g_str->menuSwitchTo : g_str->menuOpenHere);
    SetMenuDefaultItem(menu, kOpen, FALSE);
    if (g_pairButton && p.source) AppendMenuW(menu, MF_STRING, kPair, g_str->menuPair);
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

    std::vector<HMONITOR> monitors = GetMonitors();
    HMONITOR current = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (monitors.size() > 1) {
        HMENU sub = CreatePopupMenu();
        for (size_t m = 0; m < monitors.size() && m < 16; m++) {
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);
            GetMonitorInfoW(monitors[m], &mi);
            std::wstring label = Format(g_str->monitorFmt, (int)m + 1,
                                        (int)(mi.rcMonitor.right - mi.rcMonitor.left),
                                        (int)(mi.rcMonitor.bottom - mi.rcMonitor.top));
            AppendMenuW(sub, MF_STRING | (monitors[m] == current ? MF_CHECKED | MF_GRAYED : 0),
                        kMonitorBase + m, label.c_str());
        }
        AppendMenuW(menu, MF_POPUP, (UINT_PTR)sub, g_str->menuMoveTo);
    }
    bool topmost = GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;
    AppendMenuW(menu, MF_STRING | (topmost ? MF_CHECKED : 0), kTopmost, g_str->menuTopmost);
    {
        HMENU sub = CreatePopupMenu();
        int current = GetOpacityPercent(hwnd);
        for (int pct = 100; pct >= 40; pct -= 10) {
            bool checked = std::abs(current - pct) <= 4;
            AppendMenuW(sub, MF_STRING | (checked ? MF_CHECKED : 0), kOpacityBase + pct,
                        (std::to_wstring(pct) + L" %").c_str());
        }
        AppendMenuW(menu, MF_POPUP, (UINT_PTR)sub, g_str->menuOpacity);
    }
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, kSelect,
                IsSelected(p, hwnd) ? g_str->menuDeselect : g_str->menuSelect);
    if (g_renaming) AppendMenuW(menu, MF_STRING, kRename, g_str->menuRename);
    if (!IsIconic(hwnd)) AppendMenuW(menu, MF_STRING, kMinimize, g_str->menuMinimize);
    AppendMenuW(menu, MF_STRING, kClose, g_str->menuClose);

    UINT cmd = TrackPanelMenu(p, menu, screenPt);
    DestroyMenu(menu);
    if (!cmd || !IsWindow(hwnd)) return;
    i = -1;
    for (int k = 0; k < (int)p.entries.size(); k++) {
        if (p.entries[k].kind == EntryWindow && p.entries[k].hwnd == hwnd) i = k;
    }
    if (i < 0) return;

    if (cmd == kOpen) {
        FinishPanel(p, PanelResult::Select, i);
    } else if (cmd == kPair) {
        FinishPanel(p, PanelResult::Pair, i);
    } else if (cmd == kTopmost) {
        ToggleTopmost(hwnd);
    } else if (cmd == kSelect) {
        ToggleSelected(p, hwnd);
    } else if (cmd == kRename) {
        BeginRename(p, i);
    } else if (cmd == kMinimize) {
        ShowWindow(hwnd, SW_SHOWMINNOACTIVE);
        for (auto* list : {&p.all, &p.entries}) {
            for (auto& e : *list) {
                if (e.hwnd == hwnd) e.minimized = true;
            }
        }
        if (p.grid) RegisterThumbs(p);
        InvalidateRect(p.hwnd, nullptr, FALSE);
    } else if (cmd == kClose) {
        CloseEntry(p, i);
    } else if (cmd >= kMonitorBase && cmd < kMonitorBase + monitors.size()) {
        MoveToMonitor(hwnd, monitors[cmd - kMonitorBase]);
    } else if (cmd > kOpacityBase && cmd <= kOpacityBase + 100) {
        SetOpacityPercent(hwnd, (int)(cmd - kOpacityBase));
    }
}

void ShowLayoutsMenu(Panel& p, POINT screenPt) {
    enum : UINT { kSave = 1, kRestoreBase = 100, kDeleteBase = 200 };
    p.layouts = LoadSavedLayouts();
    HMENU menu = CreatePopupMenu();
    if (p.layouts.empty()) {
        AppendMenuW(menu, MF_STRING | MF_GRAYED, 0, g_str->layoutsEmpty);
    }
    for (size_t i = 0; i < p.layouts.size(); i++) {
        std::wstring label = p.layouts[i].name;
        for (size_t k = 0; k < label.size(); k++) {
            if (label[k] == L'&') label.insert(k++, 1, L'&');
        }
        AppendMenuW(menu, MF_STRING, kRestoreBase + i, label.c_str());
    }
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, kSave, g_str->layoutsSave);
    if (!p.layouts.empty()) {
        HMENU sub = CreatePopupMenu();
        for (size_t i = 0; i < p.layouts.size(); i++) {
            AppendMenuW(sub, MF_STRING, kDeleteBase + i, p.layouts[i].name.c_str());
        }
        AppendMenuW(menu, MF_POPUP, (UINT_PTR)sub, g_str->layoutsDelete);
    }
    UINT cmd = TrackPanelMenu(p, menu, screenPt);
    DestroyMenu(menu);
    if (cmd == kSave) {
        BeginLayoutNaming(p);
    } else if (cmd >= kRestoreBase && cmd < kRestoreBase + p.layouts.size()) {
        FinishPanel(p, PanelResult::RestoreLayout, (int)(cmd - kRestoreBase));
    } else if (cmd >= kDeleteBase && cmd < kDeleteBase + p.layouts.size()) {
        p.layouts.erase(p.layouts.begin() + (cmd - kDeleteBase));
        StoreSavedLayouts(p.layouts);
    }
}

void OnFooterClick(Panel& p, int i) {
    const FooterButton& b = p.footer[i];
    if (b.act == Act::SavedLayouts) {
        POINT pt{b.rect.left, b.rect.bottom};
        ClientToScreen(p.hwnd, &pt);
        ShowLayoutsMenu(p, pt);
        return;
    }
    p.resultAct = b.act;
    FinishPanel(p, PanelResult::Action);
}

LRESULT CALLBACK PanelWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCACTIVATE) {
        // Acrylic/Mica turn solid while a window is inactive, and the panel
        // often isn't the active window (e.g. opened by hovering): keep the
        // "active" look so the material always shows.
        return DefWindowProcW(hwnd, msg, TRUE, lParam);
    }
    Panel* pp = g_panel;
    if (!pp || pp->hwnd != hwnd) return DefWindowProcW(hwnd, msg, wParam, lParam);
    Panel& p = *pp;

    switch (msg) {
        case WM_PAINT:
            PaintPanel(p);
            return 0;
        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEMOVE: {
            if (!p.tracking) {
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                p.tracking = true;
            }
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (p.dragThumb) {
                DragScrollTo(p, pt.y);
                return 0;
            }
            SetHot(p, HitTestPanel(p, pt));
            return 0;
        }
        case WM_MOUSELEAVE:
            p.tracking = false;
            if (!p.dragThumb && !p.menuOpen) SetHot(p, PanelHit{});
            return 0;

        case WM_LBUTTONDOWN: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            PanelHit h = HitTestPanel(p, pt);
            if (h.scrollbar) {
                if (pt.y >= p.scrollThumb.top && pt.y < p.scrollThumb.bottom) {
                    p.dragThumb = true;
                    p.dragStartY = pt.y;
                    p.dragStartScroll = p.scrollY;
                    KillTimer(hwnd, kScrollAnimTimerId);
                    SetCapture(hwnd);
                } else {
                    int page = (p.listRect.bottom - p.listRect.top) * 9 / 10;
                    ScrollTo(p, p.scrollTarget + (pt.y < p.scrollThumb.top ? -page : page));
                }
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (p.dragThumb) {
                p.dragThumb = false;
                ReleaseCapture();
                RefreshHotFromCursor(p);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            PanelHit h = HitTestPanel(p, pt);
            if (h.footer >= 0) {
                OnFooterClick(p, h.footer);
            } else if (h.header >= 0) {
                ToggleSection(p, h.header);
            } else if (h.index >= 0 && (wParam & MK_CONTROL)) {
                if (IsWindowEntry(p.entries[h.index])) {
                    ToggleSelected(p, p.entries[h.index].hwnd);
                }
            } else if (h.index >= 0 && h.pair) {
                FinishPanel(p, PanelResult::Pair, h.index);
            } else if (h.index >= 0 && h.close) {
                CloseEntry(p, h.index);
            } else if (h.index >= 0) {
                FinishPanel(p, PanelResult::Select, h.index);
            }
            return 0;
        }
        case WM_CAPTURECHANGED:
            if (p.dragThumb) {
                p.dragThumb = false;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_RBUTTONUP: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            PanelHit h = HitTestPanel(p, pt);
            if (h.index >= 0) {
                ClientToScreen(hwnd, &pt);
                ShowEntryMenu(p, h.index, pt);
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            if (p.edit) return 0;
            int notches = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            if (notches == 0) notches = GET_WHEEL_DELTA_WPARAM(wParam) < 0 ? -1 : 1;
            if (p.maxScroll > 0) {
                int step = p.grid ? Sc(kGridThumbH) : Sc(ListRowH()) * 2;
                ScrollTo(p, p.scrollTarget - notches * step);
            } else {
                MoveHot(p, notches < 0 ? 1 : -1);
            }
            return 0;
        }

        case WM_KEYDOWN: {
            int n = (int)p.entries.size();
            bool ctrl = GetKeyState(VK_CONTROL) & 0x8000;
            bool shift = GetKeyState(VK_SHIFT) & 0x8000;
            switch (wParam) {
                case VK_ESCAPE:
                    if (!p.filter.empty()) {
                        p.filter.clear();
                        ApplyFilter(p);
                    } else {
                        FinishPanel(p, PanelResult::None);
                    }
                    break;
                case VK_RETURN: {
                    int i = p.hot >= 0 ? p.hot : (n > 0 && !p.filter.empty() ? 0 : -1);
                    if (i < 0) break;
                    if (ctrl && p.entries[i].kind == EntryFile) {
                        OpenFileLocation(p.entries[i].app);
                        FinishPanel(p, PanelResult::None);
                    } else if (ctrl && g_pairButton && p.source && IsWindowEntry(p.entries[i])) {
                        FinishPanel(p, PanelResult::Pair, i);
                    } else {
                        FinishPanel(p, PanelResult::Select, i);
                    }
                    break;
                }
                case VK_TAB:
                    MoveHotMru(p, shift ? -1 : 1);
                    break;
                case VK_DOWN:
                    MoveHot(p, p.grid ? std::min(p.cols, std::max(n, 1)) : 1);
                    break;
                case VK_UP:
                    MoveHot(p, p.grid ? -std::min(p.cols, std::max(n, 1)) : -1);
                    break;
                case VK_RIGHT:
                    MoveHot(p, 1);
                    break;
                case VK_LEFT:
                    MoveHot(p, -1);
                    break;
                case VK_NEXT:
                    ScrollTo(p, p.scrollTarget + (p.listRect.bottom - p.listRect.top));
                    break;
                case VK_PRIOR:
                    ScrollTo(p, p.scrollTarget - (p.listRect.bottom - p.listRect.top));
                    break;
                case VK_HOME:
                    if (n) {
                        SetHotItem(p, 0);
                        EnsureVisible(p, 0);
                    }
                    break;
                case VK_END:
                    if (n) {
                        SetHotItem(p, n - 1);
                        EnsureVisible(p, n - 1);
                    }
                    break;
                case VK_DELETE:
                    if (g_closeButton && p.hot >= 0 && IsWindowEntry(p.entries[p.hot])) {
                        CloseEntry(p, p.hot);
                    }
                    break;
                case VK_F2:
                    if (p.hot >= 0 && IsWindowEntry(p.entries[p.hot])) BeginRename(p, p.hot);
                    break;
                case VK_APPS:
                    if (p.hot >= 0) {
                        RECT r = ItemClientRect(p, p.hot);
                        POINT pt{r.left + Sc(40), r.bottom};
                        ClientToScreen(hwnd, &pt);
                        ShowEntryMenu(p, p.hot, pt);
                    }
                    break;
            }
            return 0;
        }
        case WM_CHAR: {
            wchar_t ch = (wchar_t)wParam;
            p.outsideSince = 0;
            if (ch == 8) {  // backspace
                if (!p.filter.empty()) {
                    p.filter.pop_back();
                    ApplyFilter(p);
                }
                return 0;
            }
            if (ch < 32) return 0;
            if (p.filter.empty() && ch >= L'1' && ch <= L'9') {
                int d = (int)(ch - L'0');
                for (int i = 0; i < (int)p.entries.size(); i++) {
                    if (p.entries[i].display == d) {
                        FinishPanel(p, PanelResult::Select, i);
                        break;
                    }
                }
                return 0;
            }
            if (p.filter.empty() && ch == L' ') {
                if (p.hot >= 0 && IsWindowEntry(p.entries[p.hot])) {
                    ToggleSelected(p, p.entries[p.hot].hwnd);
                }
                return 0;
            }
            if (p.filter.size() < 60) {
                p.filter += ch;
                ApplyFilter(p);
            }
            return 0;
        }

        case WM_APP_EDIT_DONE:
            EndRename(p, wParam != 0);
            return 0;

        case WM_COPYDATA: {
            auto* cds = reinterpret_cast<COPYDATASTRUCT*>(lParam);
            if (!cds || cds->dwData != kEverythingReplyId || p.fileSection < 0) break;
            p.fileResults = ParseEverythingReply(cds, &p.fileTotal);
            int hot = p.hot;
            BuildEntries(p);
            RelayoutPanel(p);
            if (!p.entries.empty()) {
                SetHotItem(p, hot >= 0 && hot < (int)p.entries.size() ? hot : 0);
            }
            return TRUE;
        }

        case WM_CTLCOLOREDIT: {
            Theme t = GetTheme();
            SetTextColor((HDC)wParam, t.text);
            SetBkColor((HDC)wParam, t.hover);
            if (!p.editBrush) p.editBrush = CreateSolidBrush(t.hover);
            return (LRESULT)p.editBrush;
        }

        case WM_ACTIVATE:
            if (LOWORD(wParam) != WA_INACTIVE) {
                if (GetForegroundWindow() == hwnd) p.wasForeground = true;
            } else if (p.wasForeground && !p.menuOpen) {
                if (p.edit) EndRename(p, true);
                FinishPanel(p, PanelResult::None);
            }
            return 0;

        case WM_TIMER:
            if (wParam == kSwTimerId) {
                OnPanelTick(p);
            } else if (wParam == kScrollAnimTimerId) {
                OnScrollAnimTick(p);
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ===========================================================================
// Showing the panel
// ===========================================================================

void OnWheel(HWND source, int delta);

// The windows that layouts and group actions work on: the selection, or the
// other windows of the current app.
std::vector<HWND> ActionTargets(const Panel& p) {
    std::vector<HWND> targets;
    if (!p.selected.empty()) {
        for (auto& e : p.all) {
            if (IsSelected(p, e.hwnd) && e.hwnd != p.source) targets.push_back(e.hwnd);
        }
        return targets;
    }
    for (auto& e : p.all) {
        if (e.sameApp) targets.push_back(e.hwnd);
    }
    return targets;
}

void RunAction(const Panel& p, Act act) {
    std::vector<HWND> targets = ActionTargets(p);
    HMONITOR monitor = p.source ? MonitorFromWindow(p.source, MONITOR_DEFAULTTONEAREST)
                                : MonitorFromPoint(p.origin, MONITOR_DEFAULTTONEAREST);
    switch (act) {
        case Act::Grid:
        case Act::Cols2:
        case Act::Cols3:
        case Act::Quad:
        case Act::MainSide: {
            std::vector<HWND> windows;
            if (p.source) windows.push_back(p.source);
            for (HWND h : targets) {
                if (IsWindow(h)) windows.push_back(h);
            }
            ArrangeWindows(act, windows, monitor);
            break;
        }
        case Act::MinimizeOthers:
            for (HWND h : targets) ShowWindow(h, SW_SHOWMINNOACTIVE);
            if (p.source) ForceForeground(p.source);
            break;
        case Act::BringHere:
            for (HWND h : targets) MoveToMonitor(h, monitor);
            break;
        case Act::CloseOthers:
            for (HWND h : targets) PostMessageW(h, WM_SYSCOMMAND, SC_CLOSE, 0);
            break;
        case Act::SavedLayouts:
            break;
    }
}

void BuildFooter(Panel& p) {
    p.footer.clear();
    if (g_layoutButtons) {
        for (Act a : {Act::Grid, Act::Cols2, Act::Cols3, Act::Quad, Act::MainSide}) {
            p.footer.push_back({a, {}});
        }
    }
    if (g_groupActions) {
        for (Act a : {Act::MinimizeOthers, Act::BringHere, Act::CloseOthers}) {
            p.footer.push_back({a, {}});
        }
    }
    if (g_savedLayouts) p.footer.push_back({Act::SavedLayouts, {}});
}

// Shows the panel. `source` is the window it was opened from (may be null
// for the hotkey panel), `pt` the anchor point.
void ShowSwitcher(HWND source, POINT pt, bool hotkey) {
    bool shift = GetAsyncKeyState(VK_SHIFT) & 0x8000;
    bool showAll = hotkey || (g_showAll != (g_shiftInverts && shift));

    std::wstring appPath;
    std::vector<Candidate> candidates = CollectCandidates(source, &appPath, showAll);
    if (!showAll) {
        candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                        [](const Candidate& c) { return !c.sameApp; }),
                         candidates.end());
    }
    bool launcherApps = g_searchApps == 2 || (g_searchApps == 1 && hotkey);
    bool launcherFiles = hotkey && g_everything && IsEverythingRunning();
    // The hotkey panel opens even with no windows: it's also a launcher.
    if (candidates.empty() && !hotkey) {
        Wh_Log(L"Nothing to show for %p (%s)", source, appPath.c_str());
        return;
    }
    if (launcherApps) RefreshAppIndexAsync(false);
    Wh_Log(L"Showing %d window(s) for %p (%s)%s", (int)candidates.size(), source,
           appPath.c_str(), showAll ? L" [all]" : L"");

    SelectLanguage();

    Panel p;
    p.source = source;
    p.grid = g_gridView;
    p.showAll = showAll;
    p.hotkeyMode = hotkey;
    p.origin = pt;

    std::wstring appKey = ToLower(appPath);
    if (source) GetSlotNumber(source, appKey);

    // Sections: the current app first, then the other apps by recent use.
    bool hasCurrent = false;
    for (auto& c : candidates) hasCurrent |= c.sameApp;
    if (hasCurrent) {
        Section s;
        s.key = appKey;
        s.name = AppDisplayName(appPath);
        s.current = true;
        p.secs.push_back(s);
    }
    for (auto& c : candidates) {
        std::wstring key = ToLower(c.path);
        int sectionIndex = -1;
        if (c.sameApp) {
            sectionIndex = 0;
        } else {
            for (int s = hasCurrent ? 1 : 0; s < (int)p.secs.size(); s++) {
                if (p.secs[s].key == key) sectionIndex = s;
            }
            if (sectionIndex < 0) {
                Section s;
                s.key = key;
                s.name = AppDisplayName(c.path);
                s.collapsed = std::find(g_collapsedApps.begin(), g_collapsedApps.end(),
                                        key) != g_collapsedApps.end();
                p.secs.push_back(s);
                sectionIndex = (int)p.secs.size() - 1;
            }
        }
        int n = GetSlotNumber(c.hwnd, key);
        Entry e{c.hwnd,
                c.title,
                GetLabel(c.hwnd),
                key,
                n,
                c.sameApp ? n : 0,
                ColorForNumber(n),
                c.minimized,
                nullptr,
                false,
                sectionIndex,
                c.sameApp,
                MruRank(c.hwnd)};
        e.icon = GetWindowIcon(c.hwnd, c.path, &e.ownsIcon);
        p.secs[sectionIndex].total++;
        if (!p.secs[sectionIndex].icon) p.secs[sectionIndex].icon = e.icon;
        p.all.push_back(e);
    }

    // Order sections (other apps by their most recent window) and entries.
    std::vector<int> bestMru(p.secs.size(), 100000);
    for (auto& e : p.all) bestMru[e.section] = std::min(bestMru[e.section], e.mru);
    std::vector<int> order(p.secs.size());
    for (size_t i = 0; i < order.size(); i++) order[i] = (int)i;
    std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
        if (p.secs[a].current != p.secs[b].current) return p.secs[a].current;
        return bestMru[a] < bestMru[b];
    });
    std::vector<int> remap(p.secs.size());
    std::vector<Section> sorted;
    for (size_t i = 0; i < order.size(); i++) {
        remap[order[i]] = (int)i;
        sorted.push_back(p.secs[order[i]]);
    }
    p.secs = sorted;
    for (auto& e : p.all) e.section = remap[e.section];
    if (launcherApps) {
        Section s;
        s.key = L"::apps";
        s.name = g_str->installedApps;
        s.kind = EntryApp;
        p.secs.push_back(s);
        p.appSection = (int)p.secs.size() - 1;
    }
    if (launcherFiles) {
        Section s;
        s.key = L"::files";
        s.name = g_str->filesEverything;
        s.kind = EntryFile;
        p.secs.push_back(s);
        p.fileSection = (int)p.secs.size() - 1;
    }
    std::stable_sort(p.all.begin(), p.all.end(), [](const Entry& a, const Entry& b) {
        if (a.section != b.section) return a.section < b.section;
        if (a.sameApp) return a.number < b.number;
        return a.mru < b.mru;
    });

    if (showAll) {
        p.appName = g_str->allWindows;
    } else {
        p.appName = AppDisplayName(appPath);
        if (source) p.appIcon = GetWindowIcon(source, appPath, &p.ownsAppIcon);
    }
    p.pairToRight = source ? PairGoesRight(source) : true;

    g_menuDpi = DpiForMonitor(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
    g_dark = IsDarkMode();
    CreateFonts();
    BuildFooter(p);

    // Visible entries (also computes the per-section ranges).
    BuildEntries(p);

    p.width = ComputePanelWidth(p);
    SIZE sz = LayoutPanel(p);
    MONITORINFO mi = MonitorInfoFromPoint(pt);
    int x, y;
    if (hotkey) {
        x = (mi.rcWork.left + mi.rcWork.right - sz.cx) / 2;
        y = Clamp(mi.rcWork.top + (mi.rcWork.bottom - mi.rcWork.top - sz.cy) / 3,
                  mi.rcWork.top, mi.rcWork.bottom - sz.cy);
    } else {
        x = Clamp(pt.x - sz.cx + Sc(60), mi.rcWork.left, mi.rcWork.right - sz.cx);
        y = Clamp(pt.y + Sc(14), mi.rcWork.top, mi.rcWork.bottom - sz.cy);
    }

    g_panel = &p;
    // The wheel-on-minimize-button shortcut only applies to the hover panel.
    g_panelSource = hotkey ? nullptr : source;
    g_pendingWheelSource = nullptr;
    g_pendingWheelDelta = 0;
    g_panelOpen = true;
    p.hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kSwitcherClassName, L"",
                             WS_POPUP, x, y, sz.cx, sz.cy, nullptr, nullptr, g_instance,
                             nullptr);
    if (p.hwnd) {
        g_panelHwnd = p.hwnd;
        // Everything may run elevated: let its replies through.
        if (launcherFiles) {
            ChangeWindowMessageFilterEx(p.hwnd, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
        }
        p.glass = ApplyWindowChrome(p.hwnd, GetTheme().border);

        RegisterThumbs(p);
        // Show without activating first, then ask for the foreground: if
        // Windows refuses, the panel still stays up and works with the mouse.
        ShowWindow(p.hwnd, SW_SHOWNOACTIVATE);
        if (p.glass) RefreshMaterial(p.hwnd);
        ForceForeground(p.hwnd);
        if (GetForegroundWindow() == p.hwnd) {
            p.wasForeground = true;
            SetFocus(p.hwnd);
        } else {
            Wh_Log(L"Panel shown without keyboard focus");
        }
        SetTimer(p.hwnd, kSwTimerId, 50, nullptr);

        if (hotkey) {
            // Preselect the most recently used window, so Enter goes back.
            int best = -1;
            for (int i = 0; i < (int)p.entries.size(); i++) {
                if (best < 0 || p.entries[i].mru < p.entries[best].mru) best = i;
            }
            if (best >= 0) {
                SetHotItem(p, best);
                EnsureVisible(p, best);
            }
        }

        // Local modal loop.
        while (!p.done) {
            DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE,
                                                QS_ALLINPUT);
            if (r != WAIT_OBJECT_0 + 1) {
                p.done = true;
                break;
            }
            MSG msg;
            while (!p.done && PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    PostQuitMessage((int)msg.wParam);
                    p.done = true;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        KillTimer(p.hwnd, kSwTimerId);
        KillTimer(p.hwnd, kScrollAnimTimerId);
        if (p.edit) EndRename(p, true);
        if (p.dragThumb) ReleaseCapture();
        HidePreview();
        UnregisterThumbs(p);
        g_panel = nullptr;
        DestroyWindow(p.hwnd);
    }
    g_panel = nullptr;
    g_panelOpen = false;
    g_panelHwnd = nullptr;
    g_panelSource = nullptr;
    if (p.editBrush) DeleteObject(p.editBrush);
    DestroyFonts();

    Wh_Log(L"Panel result: %d (%d)", (int)p.result, p.resultIndex);
    bool stopping = WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0;
    bool validIndex = p.resultIndex >= 0 && p.resultIndex < (int)p.entries.size();
    HWND target = validIndex ? p.entries[p.resultIndex].hwnd : nullptr;
    bool sourceOk = source && IsWindow(source);

    if (stopping) {
        // Nothing: the mod is being unloaded.
    } else if (g_pendingWheelSource) {
        // The wheel was used on the minimize button while the panel was open.
        HWND wheelSource = g_pendingWheelSource;
        int delta = g_pendingWheelDelta;
        g_pendingWheelSource = nullptr;
        OnWheel(wheelSource, delta);
    } else if (p.result == PanelResult::Select && validIndex &&
               p.entries[p.resultIndex].kind == EntryApp) {
        LaunchApp(p.entries[p.resultIndex].app);
    } else if (p.result == PanelResult::Select && validIndex &&
               p.entries[p.resultIndex].kind == EntryFile) {
        ShellOpen(p.entries[p.resultIndex].app);
    } else if (p.result == PanelResult::Select && validIndex &&
               p.entries[p.resultIndex].kind == EntryMore) {
        OpenEverythingSearch(p.entries[p.resultIndex].app);
    } else if (p.result == PanelResult::Select && target && IsWindow(target)) {
        if (!sourceOk || (hotkey && g_hotkeyActivate)) {
            ActivateWindow(target);
        } else {
            SwitchTo(source, target, g_animation ? g_animationDuration.load() : 0);
        }
    } else if (p.result == PanelResult::Pair && target && IsWindow(target) && sourceOk) {
        PairWindows(source, target, p.pairToRight,
                    g_animation ? g_animationDuration.load() : 0);
    } else if (p.result == PanelResult::Action) {
        RunAction(p, p.resultAct);
    } else if (p.result == PanelResult::RestoreLayout && p.resultIndex >= 0 &&
               p.resultIndex < (int)p.layouts.size()) {
        RestoreSavedLayout(p.layouts[p.resultIndex]);
    } else if (sourceOk) {
        HWND fg = GetForegroundWindow();
        if (!fg) SetForegroundWindow(source);
    }

    for (auto& e : p.all) {
        if (e.ownsIcon && e.icon) DestroyIcon(e.icon);
    }
    if (p.ownsAppIcon && p.appIcon) DestroyIcon(p.appIcon);
    for (auto& b : p.appBitmaps) {
        if (b.second) DeleteObject(b.second);
    }
    for (auto& f : p.fileIcons) {
        if (f.second) DestroyIcon(f.second);
    }
}

// ===========================================================================
// Mouse wheel on the minimize button
// ===========================================================================

HANDLE g_wheelThread = nullptr;
DWORD g_wheelThreadId = 0;
ULONGLONG g_lastWheel = 0;

// Key-tap detection state (only touched on the input hook thread).
int g_tapTaps = 0;
bool g_tapKeyDown = false;
bool g_tapDirty = false;  // another key/button was used while the tap key was down
DWORD g_tapDownTime = 0;
DWORD g_tapLastUpTime = 0;

bool IsTapKey(DWORD vk) {
    switch (g_tapKey.load()) {
        case 1:
            return vk == VK_LCONTROL || vk == VK_RCONTROL || vk == VK_CONTROL;
        case 2:
            return vk == VK_LSHIFT || vk == VK_RSHIFT || vk == VK_SHIFT;
        case 3:
            return vk == VK_LMENU || vk == VK_RMENU || vk == VK_MENU;
    }
    return false;
}

void ResetTaps() {
    g_tapTaps = 0;
    if (g_tapKeyDown) g_tapDirty = true;
}

// Recognizes quick taps of a lone modifier key (e.g. Ctrl, Ctrl) and asks
// the main thread to open the panel. Keys are never swallowed.
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_tapKey) {
        auto* k = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
        bool up = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
        if (IsTapKey(k->vkCode)) {
            if (down && !g_tapKeyDown) {  // ignore auto-repeat
                g_tapKeyDown = true;
                g_tapDirty = false;
                g_tapDownTime = k->time;
                if (g_tapTaps && k->time - g_tapLastUpTime > (DWORD)g_tapInterval.load()) {
                    g_tapTaps = 0;
                }
            } else if (up && g_tapKeyDown) {
                g_tapKeyDown = false;
                // A tap is short and alone; anything else starts over.
                if (!g_tapDirty && k->time - g_tapDownTime < 400) {
                    g_tapTaps++;
                    g_tapLastUpTime = k->time;
                    if (g_tapTaps >= g_tapCount.load()) {
                        g_tapTaps = 0;
                        if (HWND main = g_hwnd.load()) PostMessageW(main, WM_APP_TAP, 0, 0);
                    }
                } else {
                    g_tapTaps = 0;
                }
            }
        } else if (down) {
            ResetTaps();
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
                               wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN)) {
        ResetTaps();
    }
    if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL && g_wheelSwitch) {
        auto* m = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        HWND main = g_hwnd.load();
        HWND source = g_panelOpen ? g_panelSource.load() : g_onMinWnd.load();
        bool nearButton = std::abs(m->pt.x - g_onMinX.load()) <= 30 &&
                          std::abs(m->pt.y - g_onMinY.load()) <= 30;
        if (main && source && nearButton) {
            // While the panel is open, a wheel over the panel itself is for
            // the panel (it moves the highlight), not for switching.
            HWND panel = g_panelHwnd.load();
            HWND under = WindowFromPoint(m->pt);
            bool overPanel = panel && under && GetAncestor(under, GA_ROOT) == panel;
            if (!overPanel) {
                PostMessageW(main, WM_APP_WHEEL, (WPARAM)(SHORT)HIWORD(m->mouseData),
                             (LPARAM)source);
                return 1;  // swallow the scroll
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI WheelThreadProc(LPVOID param) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);  // create the queue
    // The mouse hook serves the wheel and resets key taps on clicks; the
    // keyboard hook is only installed for key taps and only looks at
    // modifier keys (nothing is recorded or swallowed).
    HHOOK hook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, g_instance, 0);
    Wh_Log(L"Mouse hook %s (error %u)", hook ? L"installed" : L"FAILED",
           hook ? 0 : GetLastError());
    HHOOK keyboardHook = nullptr;
    if (g_tapKey) {
        keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, g_instance, 0);
        Wh_Log(L"Keyboard hook %s (error %u)", keyboardHook ? L"installed" : L"FAILED",
               keyboardHook ? 0 : GetLastError());
    }
    SetEvent((HANDLE)param);
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
    }
    if (hook) UnhookWindowsHookEx(hook);
    if (keyboardHook) UnhookWindowsHookEx(keyboardHook);
    return 0;
}

void StartWheelHook() {
    if (g_wheelThread) return;
    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wheelThread = CreateThread(nullptr, 0, WheelThreadProc, ready, 0, &g_wheelThreadId);
    if (g_wheelThread) WaitForSingleObject(ready, INFINITE);
    CloseHandle(ready);
}

void StopWheelHook() {
    if (!g_wheelThread) return;
    PostThreadMessageW(g_wheelThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_wheelThread, INFINITE);
    CloseHandle(g_wheelThread);
    g_wheelThread = nullptr;
    g_wheelThreadId = 0;
}

void ResetHover();

void OnWheel(HWND source, int delta) {
    if (GetTickCount64() - g_lastWheel < 120 || !IsWindow(source)) return;

    std::wstring appPath;
    std::vector<Candidate> cands = CollectCandidates(source, &appPath, false);
    if (cands.empty()) {
        Wh_Log(L"Wheel: no other windows of %s", appPath.c_str());
        return;
    }
    std::wstring appKey = ToLower(appPath);

    struct Item {
        int number;
        HWND hwnd;
        std::wstring title;
    };
    std::vector<Item> list{{GetSlotNumber(source, appKey), source, GetWindowTitle(source)}};
    for (auto& c : cands) list.push_back({GetSlotNumber(c.hwnd, appKey), c.hwnd, c.title});
    std::sort(list.begin(), list.end(),
              [](const Item& a, const Item& b) { return a.number < b.number; });

    int idx = 0;
    for (int i = 0; i < (int)list.size(); i++) {
        if (list[i].hwnd == source) idx = i;
    }
    int n = (int)list.size();
    int next = delta < 0 ? (idx + 1) % n : (idx - 1 + n) % n;
    const Item& target = list[next];

    int anim = g_animation ? std::min(g_animationDuration.load(), 150) : 0;
    SwitchTo(source, target.hwnd, anim);

    // Keep the cursor "on" the new window's button and delay the panel.
    g_onMinWnd = target.hwnd;
    g_suppressWnd = target.hwnd;
    ResetHover();

    POINT pt;
    GetCursorPos(&pt);
    std::wstring label = GetLabel(target.hwnd);
    ShowOsd(target.number, label.empty() ? target.title : label,
            ColorForNumber(target.number), pt);

    // Drop wheel notches queued while we were switching.
    MSG m;
    if (HWND main = g_hwnd.load()) {
        while (PeekMessageW(&m, main, WM_APP_WHEEL, WM_APP_WHEEL, PM_REMOVE)) {
        }
    }
    g_lastWheel = GetTickCount64();
}

// ===========================================================================
// Hover detection
// ===========================================================================

bool SendHitTest(HWND hwnd, POINT pt) {
    DWORD_PTR result = 0;
    if (!SendMessageTimeoutW(hwnd, WM_NCHITTEST, 0, MAKELPARAM((short)pt.x, (short)pt.y),
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &result)) {
        return false;
    }
    return (LRESULT)result == HTMINBUTTON;
}

bool HitTestIsMinButton(HWND hwnd, POINT pt) {
    if (SendHitTest(hwnd, pt)) return true;
    DPI_AWARENESS_CONTEXT ctx = GetWindowDpiAwarenessContext(hwnd);
    if (ctx &&
        !AreDpiAwarenessContextsEqual(ctx, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) &&
        !AreDpiAwarenessContextsEqual(ctx, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)) {
        POINT logical = pt;
        if (PhysicalToLogicalPointForPerMonitorDPI(hwnd, &logical) &&
            (logical.x != pt.x || logical.y != pt.y)) {
            return SendHitTest(hwnd, logical);
        }
    }
    return false;
}

bool IsOverMinButtonByGeometry(HWND hwnd, POINT pt) {
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (!(style & WS_SYSMENU) || !(style & WS_MINIMIZEBOX) || IsIconic(hwnd)) {
        return false;
    }
    RECT buttons{};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_CAPTION_BUTTON_BOUNDS, &buttons,
                                     sizeof(buttons))) ||
        IsRectEmpty(&buttons)) {
        return false;
    }
    RECT wr;
    if (!GetWindowRect(hwnd, &wr)) return false;
    OffsetRect(&buttons, wr.left, wr.top);
    int width = (buttons.right - buttons.left) / 3;
    RECT minRect = buttons;
    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
        minRect.left = buttons.right - width;
    } else {
        minRect.right = buttons.left + width;
    }
    return PtInRect(&minRect, pt);
}

void ResetHover() {
    g_hoverWnd = nullptr;
    g_hoverStart = 0;
}

void OnTick(HWND hwndOwner) {
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 || GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
        ResetHover();
        g_onMinWnd = nullptr;
        return;
    }

    POINT pt;
    if (!GetCursorPos(&pt)) {
        ResetHover();
        g_onMinWnd = nullptr;
        return;
    }

    HWND hit = WindowFromPoint(pt);
    HWND root = hit ? GetAncestor(hit, GA_ROOT) : nullptr;

    // Caption buttons live near the top edge: skip the (cross-process) hit
    // test entirely while the cursor is in the body of a window.
    if (root) {
        RECT frame = GetFrameBounds(root);
        UINT dpi = DpiForMonitor(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
        if (pt.y > frame.top + ScD(96, dpi)) root = nullptr;
    }

    bool onMin = root && root != hwndOwner && IsSwitchableWindow(root) &&
                 (HitTestIsMinButton(root, pt) || (hit != root && HitTestIsMinButton(hit, pt)) ||
                  IsOverMinButtonByGeometry(root, pt));
    if (!onMin) {
        g_suppressWnd = nullptr;
        g_onMinWnd = nullptr;
        ResetHover();
        return;
    }

    g_onMinX = pt.x;
    g_onMinY = pt.y;
    g_onMinWnd = root;

    if (root == g_suppressWnd) return;

    ULONGLONG now = GetTickCount64();
    if (root != g_hoverWnd) {
        g_hoverWnd = root;
        g_hoverStart = now;
        return;
    }

    if (now - g_hoverStart >= (ULONGLONG)g_hoverDelay.load()) {
        g_suppressWnd = root;
        ResetHover();
        KillTimer(hwndOwner, kTimerId);
        ShowSwitcher(root, pt, false);
        if (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
            SetTimer(hwndOwner, kTimerId, kTickMs, nullptr);
        }
    }
}

// ===========================================================================
// Worker thread
// ===========================================================================

constexpr int kHotkeyId = 1;
bool g_hotkeyRegistered = false;

void UpdateHotkey(HWND hwnd) {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(hwnd, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    UINT vk = g_hotkeyVk.load();
    if (!vk) return;
    g_hotkeyRegistered =
        RegisterHotKey(hwnd, kHotkeyId, g_hotkeyMods.load() | MOD_NOREPEAT, vk);
    if (!g_hotkeyRegistered) {
        Wh_Log(L"RegisterHotKey failed (error %u): the shortcut is probably used by "
               L"another program",
               GetLastError());
    }
}

// Opens the panel from the hotkey, for the current foreground window.
void ShowSwitcherFromHotkey(HWND hwndOwner) {
    HWND fg = GetForegroundWindow();
    HWND source = fg ? GetAncestor(fg, GA_ROOT) : nullptr;
    DWORD pid = 0;
    if (source) GetWindowThreadProcessId(source, &pid);
    if (!source || pid == GetCurrentProcessId() || !IsSwitchableWindow(source)) {
        source = nullptr;
    }
    POINT pt;
    if (source) {
        RECT f = GetFrameBounds(source);
        pt = {(f.left + f.right) / 2, (f.top + f.bottom) / 2};
    } else {
        GetCursorPos(&pt);
    }
    KillTimer(hwndOwner, kTimerId);
    g_onMinWnd = nullptr;
    ShowSwitcher(source, pt, true);
    if (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
        SetTimer(hwndOwner, kTimerId, kTickMs, nullptr);
    }
}

void ApplyRuntimeSettings(HWND hwnd) {
    UpdateHotkey(hwnd);
    RefreshAppIndexAsync(false);
    if (g_windowTags) {
        InstallWinEventHooks();
        SetTimer(hwnd, kTagTimerId, 1000, nullptr);
        DestroyAllTags();
        RefreshTags();
    } else {
        KillTimer(hwnd, kTagTimerId);
        UninstallWinEventHooks();
        DestroyAllTags();
    }
    // One input-hook thread serves both the wheel and the key taps; it's
    // restarted so that only the hooks the settings need are installed.
    StopWheelHook();
    if (g_wheelSwitch || g_tapKey) StartWheelHook();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            switch (wParam) {
                case kTimerId:
                    OnTick(hwnd);
                    break;
                case kTagTimerId:
                    RefreshTags();
                    break;
                case kTagQuickTimerId:
                    KillTimer(hwnd, kTagQuickTimerId);
                    RefreshTags();
                    break;
                case kOsdTimerId:
                    KillTimer(hwnd, kOsdTimerId);
                    if (g_osdHwnd) ShowWindow(g_osdHwnd, SW_HIDE);
                    break;
            }
            return 0;
        case WM_APP_WHEEL:
            Wh_Log(L"Wheel %d on %p (panel open: %d)", (int)(SHORT)wParam, (HWND)lParam,
                   (int)g_panelOpen.load());
            if (g_panelOpen && g_panel) {
                // Close the panel and switch right after it's gone.
                g_pendingWheelSource = (HWND)lParam;
                g_pendingWheelDelta = (int)(SHORT)wParam;
                FinishPanel(*g_panel, PanelResult::None);
            } else if (!g_panelOpen) {
                OnWheel((HWND)lParam, (int)(SHORT)wParam);
            }
            return 0;
        case WM_APP_SETTINGS:
            ApplyRuntimeSettings(hwnd);
            return 0;
        case WM_APP_TAP:
            Wh_Log(L"Key taps");
            if (g_panelOpen) {
                // Tapping again closes the panel.
                if (g_panel && g_panel->hotkeyMode) FinishPanel(*g_panel, PanelResult::None);
            } else {
                ShowSwitcherFromHotkey(hwnd);
            }
            return 0;
        case WM_HOTKEY:
            if (wParam != kHotkeyId) break;
            if (g_panelOpen) {
                // Pressing the hotkey again walks the recently used windows.
                if (g_panel && g_panel->hotkeyMode) MoveHotMru(*g_panel, 1);
            } else {
                ShowSwitcherFromHotkey(hwnd);
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HINSTANCE GetModuleInstance() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&WndProc), &module);
    return module;
}

void RegisterWindowClass(const wchar_t* name, WNDPROC proc, UINT style) {
    WNDCLASSW wc{};
    wc.style = style;
    wc.lpfnWndProc = proc;
    wc.hInstance = g_instance;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));  // IDC_ARROW
    wc.lpszClassName = name;
    RegisterClassW(&wc);
}

DWORD WINAPI ThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    // Shell APIs (app icons, launching, file icons) need COM.
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    Gdiplus::GdiplusStartupInput gdiplusInput;
    ULONG_PTR gdiplusToken = 0;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusInput, nullptr);

    g_instance = GetModuleInstance();
    RegisterWindowClass(kClassName, WndProc, 0);
    RegisterWindowClass(kSwitcherClassName, PanelWndProc, CS_DROPSHADOW);
    RegisterWindowClass(kPreviewClassName, PreviewWndProc, CS_DROPSHADOW);
    RegisterWindowClass(kGhostClassName, GhostWndProc, 0);
    RegisterWindowClass(kPillClassName, PillWndProc, 0);

    g_previewHwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                                    kPreviewClassName, L"", WS_POPUP, 0, 0, 0, 0, nullptr,
                                    nullptr, g_instance, nullptr);

    HWND hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, kClassName, L"", WS_POPUP, 0, 0, 0, 0,
                                nullptr, nullptr, g_instance, nullptr);
    if (hwnd) {
        g_hwnd = hwnd;
        StartMruTracking();
        SetTimer(hwnd, kTimerId, kTickMs, nullptr);
        ApplyRuntimeSettings(hwnd);

        bool quit = false;
        while (!quit) {
            DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE,
                                                QS_ALLINPUT);
            if (r == WAIT_OBJECT_0 || r == WAIT_FAILED) break;
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    quit = true;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        KillTimer(hwnd, kTimerId);
        KillTimer(hwnd, kTagTimerId);
        if (g_hotkeyRegistered) UnregisterHotKey(hwnd, kHotkeyId);
        g_hotkeyRegistered = false;
        g_hwnd = nullptr;
        DestroyWindow(hwnd);
    }

    StopMruTracking();
    StopAppIndex();
    RestoreAllOpacity();
    StopWheelHook();
    UninstallWinEventHooks();
    DestroyAllTags();
    HidePreview();
    if (g_previewHwnd) DestroyWindow(g_previewHwnd);
    g_previewHwnd = nullptr;
    if (g_osdHwnd) DestroyWindow(g_osdHwnd);
    g_osdHwnd = nullptr;
    for (const wchar_t* c : {kClassName, kSwitcherClassName, kPreviewClassName,
                             kGhostClassName, kPillClassName}) {
        UnregisterClassW(c, g_instance);
    }
    Gdiplus::GdiplusShutdown(gdiplusToken);
    if (SUCCEEDED(hrCom)) CoUninitialize();

    return 0;
}

// ===========================================================================
// Windhawk entry points (tool mod: runs in its own windhawk.exe process)
// ===========================================================================

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return FALSE;

    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    if (g_stopEvent) SetEvent(g_stopEvent);
    // A context menu runs its own message loop that doesn't watch the stop
    // event: cancel it so the thread can exit.
    if (HWND panel = g_panelHwnd.load()) PostMessageW(panel, WM_CANCELMODE, 0, 0);
    if (g_thread) {
        // Must not return while the thread still runs code from this module.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
    if (HWND hwnd = g_hwnd.load()) PostMessageW(hwnd, WM_APP_SETTINGS, 0, 0);
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
