// ==WindhawkMod==
// @id              covertabby
// @name            CoverTabby
// @description     An animated Alt+Tab switcher with live previews and multiple layouts
// @version         1.1
// @author          LTHC
// @github          https://github.com/LoreTheHolyChip
// @license         GPL-2.0-only
// @include         explorer.exe
// @include         windhawk.exe
// @include         windhawk-mod.exe
// @compilerOptions -ldwmapi -lgdi32 -loleaut32 -lole32 -lshell32 -lversion -ladvapi32 -lhid -lruntimeobject -ld3d11 -ldxgi -ld2d1 -ldcomp -ldwrite -luuid -lwindowscodecs -lshcore -lpdh -lpsapi
// ==/WindhawkMod==

// CoverTabby by LTHC (LoreTheHolyChip).
// Touch pad controls by Lone and modified by LTHC
// And cuz i can't code also made with Codex.

// ==WindhawkModReadme==
/*
# CoverTabby
Replaces the alt+tab with a modded kind of the stardock alt tabby's layout.

By **LTHC (LoreTheHolyChip)**. Version **1.1**.

Win+Tab integration is experimental and disabled by default. Alt+Tab remains enabled.
Explorer forwards Alt+Tab to the switcher running in Windhawk’s own tool process. No separate application needs to be installed.

Credits: caliberda (MIT-licensed Alt+Tab Flip 3D renderer), Lone (touchpad integration),
and the Simple Window Switcher contributors (HID parsing).
Renderer: https://github.com/ramensoftware/windhawk-mods/blob/main/mods/alt-tab-flip-3d.wh.cpp
Touchpad source by Lone (Louis047): https://github.com/ramensoftware/windhawk-mods/pull/5396
Simple Window Switcher credits the GPLv2 upstream sws by valinet: https://github.com/valinet/sws
CoverTabby is distributed under GPL-2.0-only; MIT terms remain applicable to the renderer portions. License texts are included below in the source.

---

# Styles

---

| Style | Preview |
| --- | --- |
| Tiles — Windowed | ![Tiles — Windowed](https://raw.githubusercontent.com/LoreTheHolyChip/windhawk-mods/main/screenshots/tiles-windowed.jpg) |
| Tiles — Fullscreen | ![Tiles — Fullscreen](https://raw.githubusercontent.com/LoreTheHolyChip/windhawk-mods/main/screenshots/tiles-full.png) |
| Normal — Windowed | ![Normal — Windowed](https://raw.githubusercontent.com/LoreTheHolyChip/windhawk-mods/main/screenshots/normal-windowed.jpg) |
| Normal — Fullscreen | ![Normal — Fullscreen](https://raw.githubusercontent.com/LoreTheHolyChip/windhawk-mods/main/screenshots/normal-full.png) |
| Icons only | ![Icons only](https://raw.githubusercontent.com/LoreTheHolyChip/windhawk-mods/main/screenshots/icons-full.jpg) |
| No thumbnail | ![No thumbnail](https://raw.githubusercontent.com/LoreTheHolyChip/windhawk-mods/main/screenshots/list-full.jpg) |
| Text only | ![Text only](https://raw.githubusercontent.com/LoreTheHolyChip/windhawk-mods/main/screenshots/text-full.jpg) |
| Search | ![Search](https://raw.githubusercontent.com/LoreTheHolyChip/windhawk-mods/main/screenshots/search-full.jpg) |
| Aero 3D | Parallel tilted previews with cyclic navigation. |
| Windows 10/11 Enchanted | ![Windows 10/11 Enchanted](https://raw.githubusercontent.com/LoreTheHolyChip/windhawk-mods/main/screenshots/enchanted-full.png) |

The optional Win+Tab overview opens a desktop gallery; selecting a desktop expands its windows into a full-screen view with a minimized-window strip and rename/delete controls. Gallery, expanded-view and Aero 3D captures are pending manual visual validation.

These screenshots were captured using a macro. The same macro can generate fresh screenshots whenever I update CoverTabby.

thanks for using my mod :D

**Windows 11 supported; Windows 10 1607 (build 14393) and newer are experimental and use the built-in Segoe MDL2 Assets icon font.**
On Windows 10 builds earlier than 20348, live-captured windows can display a yellow capture border because those builds cannot disable it.
The desktop overview's virtual-desktop management requires Windows 11; on Windows 10, those controls are unavailable.

Pressing the top grip quickly reveals the real window without starting a drag.
Move while holding to drag it with its native decorations and a short
centered popup animation. Resizable windows shrink to the preview size while
the remaining interface fades out. Releasing the mouse stops the drag.
A simple click still switches windows. Canceling before dragging restores the
window's original size, position and minimized/maximized state.
Minimized and maximized windows are restored to a movable window before the
drag begins, including minimized windows that would normally restore maximized.
Drag from the top area of a preview: a standard Windows title bar or system
menu is not required. Clicking elsewhere selects the window. Custom title bars and borderless application windows keep
their own decorations. Fixed-size windows keep their native size while moving.
Minimized windows show a card matching the Windows light/dark theme, with a top
drag grip and a restore hint. The app icon remains in the icon strip below.
If CoverTabby previously captured the window, its last preview is kept in memory
and shown blurred and darkened while minimized. Holding the top grip reveals
the restored real window with sharp content; moving starts the held drag.
Windows without a saved preview use the regular theme-colored minimized card.

## Changelog

### v1.1
- Add Aero 3D, with parallel tilted Alt+Tab previews and cyclic navigation.
- Add a bottom selection indicator and localized window count that follow search results.
- Expand Win+Tab with desktop gallery layouts, an animated full-screen desktop view, and accent hover feedback.
- Add a minimized-window strip, desktop rename/delete controls, and window actions in the expanded overview.
- Stop navigation wrapping in stacks of five or more desktops and animate desktop-to-desktop transitions.
- Add optional Alt+Tab navigation across virtual desktops and refine quick-tap/hold Win+Tab behavior.
- Add live wallpaper-host previews while the switcher is open, with a static fallback.
- Rebuild search indices after entering the overview or removing closed windows, including Enchanted before typing.
- Use cached snapshots or icons for minimized previews without restoring other apps.

## Modes
- **Full** fills the monitor. Background Style chooses Blur, Opaque or Transparent.
- **Windowed** uses a compact centered popup. Windowed Scale resizes the entire popup.

Colors, fonts and rounded corners follow Windows 11 automatically.
The optional bottom position bar shows the number of open windows and tracks the selected window. It follows search results and resets to the start when navigation wraps. The Desktop card is excluded from the window count.

Enable **Show Desktop Card** to keep a Desktop item at the end of the carousel.
Selecting it uses Windows' native Show Desktop action.

## Language
Language is detected at every start: Italian for an Italian Windows display language, English otherwise. Window titles and custom desktop names keep their original text.

## Features
Live previews, application icons, grouped windows, Pin / Unpin, hover close
controls, reflections and smooth animations. Settings cover only the essentials. Hover Behavior offers Show Title, Select Window or None for unselected windows.
Opening uses a quick stationary fade. On normal selection, the selected preview smoothly resizes and returns to the real window position, starting fast and slowing down at the end. The real window remains visible beneath the topmost overlay throughout this return animation. The background fades in place while the remaining previews, icons and controls slide down and fade. Dragging keeps following the mouse instead.
Hover Animation offers None, Lift, Enlarge or Lift + Enlarge. It is disabled for
unselected windows when Hover Behavior is None. Click a preview to activate it.

## Search
While CoverTabby is open, press **Alt+S**, type a window title or application name, then press **Enter**.
Search Bar Scale changes the bar, icon and text together. Width adapts automatically.
Disable Search Bar to remove its reserved space and center the content. Search is unavailable in Windowed mode, Tiles and Aero 3D. Aero 3D also omits the icon strip, process statistics, hover Close/Pin/Kill controls, window count and reflections.

## Virtual desktops
In the Win+Tab overview, desktop cards include the Windows taskbar on the current
desktop when capture is available, with a themed app bar as the fallback on
that desktop only. Other desktops do not show a taskbar.
A left-side app dock offers animated selection and hover labels; minimized windows
use the separate upper dock. Desktop rename keeps the text caret and animates
new letters with a quick fade, upward overshoot and smooth settling.
Drag an app preview or dock icon onto another desktop card to move the window
there, keeping its original position and size. A simple click activates it.
Drag it away from the desktop cards to take the real window onto the current
desktop. Maximized and minimized windows stay unchanged until movement begins,
then restore to their normal size and follow the held pointer.

Hover the selected preview to reveal the green desktop icon and label alongside Pin and Close.
Click it to open CoverTabby's own desktop picker, then choose a desktop to move
the window there without changing its position or size. The current desktop stays active.
Use the mouse wheel to scroll the list when there are many desktops.

## Process information
CPU, RAM and GPU appear below the selected application's icon and refresh approximately every 1.5 seconds while the selection is stable.
These are metrics for the process owning the selected window, not its child processes.
RAM shows the working set in MB/GB; its color uses the percentage of physical RAM.
GPU reports the busiest GPU engine for that process, or N/A when counters are unavailable.
Usage colors are green up to 10%, white below 35%, yellow below 50%, orange up to 75%, and red above 75%.
Hover the selected preview to see relative process age at the top right, such as Opened 5 minutes ago. This is process age, not window age.
The skull button beside Close kills the owning process, including its other windows,
after a second confirming click within four seconds. Unsaved work will be lost. Close continues to request normal application closure.

## Icon controls
Hover an app icon for a small smooth lift, zoom and Windows accent background.
Right-click an icon or preview to open CoverTabby's animated menu: Close, Kill,
and Pin/Unpin. These actions target the clicked window without changing selection.
Click outside or press Escape to dismiss. Releasing Alt still closes CoverTabby.
The menu is rendered by CoverTabby and uses Segoe Fluent Icons.

## Navigation
Alt+Tab cycles forward; Shift+Alt+Tab or Alt+` cycles backward.
The first press selects the next or previous window relative to the active window,
including when pinned windows change the list order.
With Group Windows enabled, Up/Down selects a window in the current application.
Windows from the same application stay consecutive. Switching between them moves
the stack vertically; Left/Right changes applications horizontally.
Mouse selection and wheel navigation are also supported.

### Performance and background

Background Style offers Blur, Opaque and Transparent in Full and Windowed. Transparent Background Opacity controls the tint without applying blur. Background During Energy Saver automatically switches to Opaque or Transparent while Windows saves energy; Keep Selected Background disables this override.

The selected preview starts first. Other live previews start progressively, and opening waits at most 45 ms for the selected capture after startup. Desktop blur is captured at up to 960 pixels on its longest side. Preview Cache reuses up to twelve 512-pixel snapshots (384 during Energy Saver), updated gradually only while the switcher is open. When closed, captures stop except for a window currently being held and dragged. Dragging, search, groups, controls and animations remain available. VSync is always enabled: the render loop waits on DXGI frame availability with a one-frame queue and has no fixed 60 FPS cap. The actual rate follows the display compositor and available GPU performance.


## Precision touchpad
Three fingers starting in the lower third and moving upward open CoverTabby. Continuing that same stroke sideways navigates. Starting in the middle third and moving upward requests normal closure of the selected window while CoverTabby is open. Swipe downward to dismiss; a short three-finger tap selects the current window. Holding still gives animated press feedback. Existing touchpad mouse-click/hold dragging stays available.

Touchpad from Modern Window Switcher — **Lone**. HID parsing adapted from the downloaded Simple Window Switcher 3.0 source. The gesture controller is used where supported; older Windows builds may require setting native three-finger swipe/tap actions to Nothing in Windows touchpad settings to prevent duplicate system actions. Disable Precision Touchpad Gestures to stop listening to touchpad reports.


## Desktop overview
Hold to Preview is optional and disabled by default. In thumbnail styles, hold Alt on the same selection for five seconds to enlarge its native-resolution live capture to fit the monitor without stretching. Tab slides the enlarged image left; releasing Alt returns it toward the real window. Minimized previews also enlarge; two seconds after reaching full size, their minimized message changes to a playful message.

While CoverTabby is open, type a letter to focus Search and filter immediately without changing the selected style. Search must be enabled for the current mode. Press Enter to activate a result or Escape to cancel.

Win+Tab opens the optional desktop overview. A quick tap leaves it open. Holding Win for at least 280 ms, or pressing Tab again while holding Win, selects on release. Tab cycles desktops in the gallery and windows in an expanded desktop; Shift+Tab cycles backward. Left/right arrows select desktops. Disable Hold Win to Keep Desktop Overview Open for a permanently persistent overview. Renaming or inspecting with a right-click leaves hold mode so the overview stays open.

One to three desktops use a perspective carousel, four use a 2×2 grid, and larger sets use a Flip 3D stack. Click a desktop to activate it and close the overview; right-click to expand its preview and keep the overview open. The expanded view preserves each window's desktop position. The Desktops button returns to the gallery. Each HWND remains separate, including windows belonging to the same app.

Minimized windows appear in a top strip with their icon, title and available thumbnail. Previews use cached snapshots when available, otherwise the icon. Opening the overview never restores minimized windows to obtain a preview. Click once to restore a minimized window without leaving the overview, and again to focus it and close. Drag windows to desktop cards or the desktop destination strip. Rename supports Enter and Escape. Closing an occupied desktop asks to move its windows to the first desktop; an empty desktop asks for confirmation. Desktop 1 remains protected.

Live Desktop Wallpaper captures an icon-free Wallpaper Engine host while CoverTabby is open, with a static-image fallback. Host discovery repeats once per second to follow playlist changes and recreated wallpaper hosts. It displays the frames Wallpaper Engine supplies; it cannot animate a wallpaper that Wallpaper Engine itself has paused. Windows on other desktops may only supply cached frames. Wallpaper textures, gallery surfaces and additional live window captures are bounded; there is no wallpaper capture while the switcher is closed.

The optional Show Windows from Other Desktops setting applies only to Alt+Tab. Selecting a remote window switches to its desktop and focuses it. Aero 3D adds the parallel tilted window stack as a separate Alt+Tab style. The position bar and open-window count support English and Italian. Windows' internal desktop contracts vary between builds; unsupported builds show an availability message.


*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: full
  $name: Mode
  $name:it: "Modalità"
  $description: Choose whether CoverTabby covers the monitor or uses a centered popup.
  $description:it: "Scegli se CoverTabby occupa lo schermo o appare in una finestra centrata."
  $options:
  - full: Fullscreen
  - windowed: Windowed
  $options:it:
  - full: "Schermo intero"
  - windowed: "Finestra"
- style: normal
  $name: Styles
  $name:it: "Stili"
  $description: Choose the CoverTabby layout style.
  $description:it: "Scegli lo stile di disposizione delle finestre."
  $options:
  - normal: Normal
  - aero3d: Aero 3D
  - tiles: Tiles
  - icons: Icon only
  - list: No thumbnail
  - text: Text only
  - search: Search
  - enchanted: Windows 10/11 Enchanted
  $options:it:
  - normal: "Normale"
  - aero3d: "Aero 3D"
  - tiles: "Riquadri"
  - icons: "Solo icone"
  - list: "Senza anteprime"
  - text: "Solo testo"
  - search: "Ricerca"
  - enchanted: "Windows 10/11 Enchanted"
- enchantedMaterial: system
  $name: Material
  $name:it: "Materiale"
  $description: Use the same panel material throughout Alt+Tab and the desktop overview.
  $description:it: "Usa lo stesso materiale per Alt+Tab e la panoramica desktop."
  $options:
  - system: Follow Windows theme
  - dark: Dark
  - light: Light
  - acrylic: Acrylic
  $options:it:
  - system: "Segui tema Windows"
  - dark: "Scuro"
  - light: "Chiaro"
  - acrylic: "Acrilico"
- tileSelectedStyle: border
  $name: Tile Selected Style
  $name:it: "Stile del riquadro selezionato"
  $description: Choose how the selected tile is shown.
  $description:it: "Scegli come evidenziare il riquadro selezionato."
  $options:
  - border: Add a border
  - bigger: Make it bigger
  $options:it:
  - border: "Aggiungi bordo"
  - bigger: "Ingrandisci"
- selectionBar: true
  $name: Bottom window position bar
  $name:it: "Barra inferiore di posizione delle finestre"
  $description: Show the open-window count and a selection indicator. The indicator returns to the start when navigation wraps.
  $description:it: "Mostra il numero di finestre aperte e un indicatore di selezione. L’indicatore torna all’inizio quando la navigazione ricomincia dalla prima finestra."
- groupWindows: true
  $name: Group Windows
  $name:it: "Raggruppa finestre"
  $description: Group windows by application. Use Up/Down to navigate within a group.
  $description:it: "Raggruppa le finestre per applicazione. Usa Su/Giù per spostarti nel gruppo."
- windowScale: 100
  $name: Windowed Scale (%)
  $name:it: "Dimensione della modalità finestra (%)"
  $description: Scale the entire Windowed popup. Full is unaffected. Range 25-200.
  $description:it: "Ridimensiona tutta la finestra di CoverTabby. Non modifica lo schermo intero. Intervallo 25-200."
- ignoreRepeatedNavigationKeys: true
  $name: Ignore Repeated Navigation Keys
  $name:it: "Ignora ripetizione dei tasti di navigazione"
  $description: Holding Tab, backtick or an arrow key navigates only once. Release and press again to navigate further.
  $description:it: "Tenere premuto Tab, accento grave o una freccia sposta la selezione una sola volta. Rilascia e premi di nuovo per continuare."
- typingAnimation: false
  $name: Typing animation
  $name:it: Animazione di scrittura
  $description: Animate newly typed search text. Disabled by default for immediate text feedback. This might impact CoverTabby's performance.
  $description:it: Anima il testo digitato nella ricerca. Disattivata per impostazione predefinita per mostrare subito il testo. Questa opzione potrebbe influire sulle prestazioni di CoverTabby.
- searchEnabled: true
  $name: Search Bar
  $name:it: "Barra di ricerca"
  $description: Enable the search bar and Alt+S while CoverTabby is open. Search is unavailable in Windowed mode and Tiles. When disabled, remove its reserved space and center the content.
  $description:it: "Abilita la ricerca e Alt+S mentre CoverTabby è aperto. Ricerca non disponibile in modalità finestra e Riquadri. Disattivandola, il suo spazio viene rimosso e il contenuto viene centrato."
- searchScale: 100
  $name: Search Bar Scale (%)
  $name:it: "Dimensione della ricerca (%)"
  $description: Scale the search bar, text and icon together. Width adapts to the text. Range 50-200.
  $description:it: "Ridimensiona insieme barra di ricerca, testo e icona. La larghezza segue il testo. Intervallo 50-200."
- iconScale: 140
  $name: Icon Scale (%)
  $name:it: "Dimensione delle icone (%)"
  $description: Resize the application icons. Range 50-300.
  $description:it: "Ridimensiona le icone delle applicazioni. Intervallo 50-300."
- enable3D: true
  $name: 3D Windows
  $name:it: "Finestre 3D"
  $description: Tilt side windows toward the selected preview. Disable for flat, darker side previews.
  $description:it: "Inclina le finestre laterali verso quella selezionata. Disattiva per anteprime laterali piatte e più scure."
- reflections: true
  $name: Reflections
  $name:it: "Riflessi"
  $description: Show reflections below the window previews.
  $description:it: "Mostra i riflessi sotto le anteprime delle finestre."
- animation: smooth
  $name: Animation Style
  $name:it: "Stile di animazione"
  $options:
  - smooth: Smooth
  - original: Original
  $options:it:
  - smooth: "Fluida"
  - original: "Originale"
- hoverAction: title
  $name: Hover Behavior
  $name:it: "Comportamento al passaggio del mouse"
  $description: Choose what happens when hovering an unselected window. Show Title displays its title above the cursor. Select Window selects it after a short delay. None does nothing.
  $description:it: "Scegli cosa accade sulle finestre non selezionate: mostra il titolo, seleziona dopo un breve ritardo oppure nessuna azione."
  $options:
  - title: Show Title
  - select: Select Window
  - none: None
  $options:it:
  - title: "Mostra titolo"
  - select: "Seleziona finestra"
  - none: "Nessuno"
- hoverAnimation: lift
  $name: Hover Animation
  $name:it: "Animazione al passaggio del mouse"
  $description: Animate unselected window previews when hovering. Hover Behavior must be Title or Select Window.
  $description:it: "Anima le anteprime non selezionate al passaggio del mouse. Richiede Mostra titolo o Seleziona finestra."
  $options:
  - none: None
  - lift: Lift
  - enlarge: Enlarge
  - liftEnlarge: Lift + Enlarge
  $options:it:
  - none: "Nessuno"
  - lift: "Solleva"
  - enlarge: "Ingrandisci"
  - liftEnlarge: "Solleva e ingrandisci"
- backgroundStyle: blur
  $name: Background Style
  $name:it: "Stile dello sfondo"
  $description: Choose Blur, Opaque or Transparent for both Fullscreen and Windowed modes.
  $description:it: "Scegli Sfocato, Opaco o Trasparente in entrambe le modalità."
  $options:
  - blur: Blur
  - opaque: Opaque
  - transparent: Transparent
  $options:it:
  - blur: "Sfocato"
  - opaque: "Opaco"
  - transparent: "Trasparente"
- holdToPreview: false
  $name: Hold to Preview
  $name:it: "Tieni premuto per ingrandire"
  $description: Hold Alt on the same window for 5 seconds to slowly enlarge its live preview to fit the screen. Tab slides it left; releasing Alt returns it to the real window. Only available in styles with thumbnails.
  $description:it: "Tieni premuto Alt sulla stessa finestra per 5 secondi per ingrandire lentamente l’anteprima. Tab la fa scorrere; rilasciare Alt la riporta alla finestra reale. Solo negli stili con anteprime."
- showDesktopCard: true
  $name: Show Desktop Card
  $name:it: "Mostra riquadro Desktop"
  $description: Add Show Desktop as the last CoverTabby card. Selecting it toggles the Windows desktop.
  $description:it: "Aggiunge Mostra desktop come ultimo riquadro. Selezionalo per mostrare o nascondere il desktop di Windows."
- minimizedPreviewTint: accent
  $name: Minimized Preview Tint
  $name:it: "Colore delle anteprime ridotte a icona"
  $description: Choose a transparent tint for minimized previews. None removes the tint and keeps automatic text contrast.
  $description:it: "Scegli una tinta trasparente per le anteprime delle finestre ridotte a icona. Nessuno rimuove la tinta e conserva il contrasto automatico."
  $options:
  - accent: Accent
  - gray: Gray
  - none: None
  $options:it:
  - accent: "Colore principale"
  - gray: "Grigio"
  - none: "Nessuno"
- transparentOpacity: 35
  $name: Transparent Background Opacity (%)
  $name:it: "Opacità dello sfondo trasparente (%)"
  $description: Tint opacity without blur. Range 0-100. Only used for Transparent backgrounds.
  $description:it: "Opacità della tinta senza sfocatura. Intervallo 0-100. Si applica solo allo sfondo Trasparente."
- energySaverBackground: opaque
  $name: Background During Energy Saver
  $name:it: "Sfondo durante il risparmio energetico"
  $description: Follow Windows Energy Saver automatically. All controls and animations remain available.
  $description:it: "Segue automaticamente il risparmio energetico di Windows. Controlli e animazioni restano disponibili."
  $options:
  - opaque: Opaque
  - transparent: Transparent
  - unchanged: Keep Selected Background
  $options:it:
  - opaque: "Opaco"
  - transparent: "Trasparente"
  - unchanged: "Mantieni sfondo selezionato"
- previewCache: true
  $name: Preview Cache
  $name:it: "Cache delle anteprime"
  $description: Reuse up to 12 small previews for faster opening. Updated only while CoverTabby is open. No background capture while closed.
  $description:it: "Riutilizza fino a 12 piccole anteprime. Si aggiornano solo mentre CoverTabby è aperto. Nessuna acquisizione in background quando è chiuso."
- touchpadGestures: true
  $name: Precision Touchpad Gestures
  $name:it: "Gesti del touchpad di precisione"
  $description: Three fingers from the bottom open CoverTabby; from the center upward close a window. Swipe sideways to navigate, down to dismiss, and tap to select. Disable if native Windows gestures conflict.
  $description:it: "Tre dita dal basso aprono CoverTabby; dal centro verso l’alto chiudono una finestra. Scorri lateralmente per navigare, verso il basso per uscire e tocca per selezionare. Disattiva in caso di conflitto con i gesti di Windows."
- desktopOverviewHotkey: false
  $name: Win+Tab Desktop Overview
  $name:it: "Panoramica desktop con Win+Tab"
  $description: Open the fullscreen desktop overview with Win+Tab. Blur uses darkened wallpaper; other styles use opaque dark shading. Triple three-finger taps switch modes.
  $description:it: "Apre la panoramica desktop a schermo intero con Win+Tab. Funzione sperimentale, disattivata per impostazione predefinita. Tre tocchi rapidi con tre dita cambiano modalità."
- desktopOverviewHoldWin: true
  $name: Hold Win to Keep Desktop Overview Open
  $name:it: "Tieni premuto Win per mantenere la panoramica aperta"
  $description: "A quick Win+Tab tap keeps the overview open. Hold Win for at least 280 ms, or press Tab again while holding Win, to select on release. Tab cycles desktops in the gallery and windows in an expanded desktop; Shift+Tab reverses. Turn off to always keep the overview open."
  $description:it: "Un tocco rapido su Win+Tab lascia aperta la panoramica. Tieni Win premuto per almeno 280 ms, oppure premi di nuovo Tab tenendo Win premuto, per selezionare al rilascio. Tab cambia desktop nella galleria e finestra nel desktop espanso; Maiusc+Tab torna indietro. Disattiva per mantenere sempre aperta la panoramica."
- otherDesktopWindows: false
  $name: Show Windows from Other Desktops in Alt+Tab
  $name:it: "Mostra finestre degli altri desktop in Alt+Tab"
  $description: "Include windows from every virtual desktop in Alt+Tab. Selecting one switches to its desktop and focuses it. Win+Tab continues to show windows on their own desktops."
  $description:it: "Include in Alt+Tab le finestre di tutti i desktop virtuali. Selezionandone una, passa al suo desktop e la attiva. Win+Tab continua a mostrare le finestre nei rispettivi desktop."
- liveDesktopWallpaper: true
  $name: Live Desktop Wallpaper
  $name:it: "Sfondo desktop animato"
  $description: "Use a live preview of the wallpaper host when available, from Wallpaper Engine. Capture runs only while CoverTabby is open. Static wallpaper is the fallback."
  $description:it: "Mostra lo sfondo animato quando disponibile, incluso Wallpaper Engine. L'acquisizione avviene solo mentre CoverTabby è aperto. In alternativa usa lo sfondo statico."
*/
// ==/WindhawkModSettings==

#include <windows.h>
static PCWSTR CoverTabbyIconFont();

#include <roapi.h>
#include <winstring.h>
#include <hidsdi.h>
#include <hidpi.h>
#include <functional>
#include <pdh.h>
#include <pdhmsg.h>
#include <psapi.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <d3d11_4.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <dxgi1_3.h>
#include <inspectable.h>
#include <propsys.h>
#include <shellscalingapi.h>
#include <shobjidl.h>
#include <shldisp.h>
#include <objectarray.h>
#include <servprov.h>
#include <tlhelp32.h>
#include <wincodec.h>
#include <windows.graphics.capture.interop.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Security.Authorization.AppCapabilityAccess.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <windhawk_utils.h>
// CoverTabby renderer, based on the MIT-licensed Flip 3D capture engine.
static COLORREF GetAccentColor() {
    DWORD col = 0, sz = sizeof(col);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
        L"AccentColor", RRF_RT_REG_DWORD, NULL, &col, &sz) == ERROR_SUCCESS) return col & 0x00FFFFFF;
    return RGB(0, 120, 215);
}





// Defaults come from the Windhawk mod settings schema.
static bool LoadBoolSetting(LPCWSTR settingName) {
    return Wh_GetIntSetting(settingName) != 0;
}
static int LoadIntSetting(LPCWSTR settingName) {
    return Wh_GetIntSetting(settingName);
}

namespace CoverTabbyPosition {
struct Selection {int total=0,count=0,rank=-1;};
template<class Items>
Selection summarize(const Items& items,const std::vector<int>& matches,bool filtered,int selected){
    Selection result;
    auto valid=[&](int i){return i>=0&&i<(int)items.size()&&!items[i]->desktopPlaceholder&&!items[i]->windowGone;};
    for(int i=0;i<(int)items.size();++i)if(valid(i))++result.total;
    auto add=[&](int i){if(!valid(i))return;if(i==selected)result.rank=result.count;++result.count;};
    if(filtered){for(int i:matches)add(i);}else{for(int i=0;i<(int)items.size();++i)add(i);}
    return result;
}
struct Motion {
    double value=-1;int lastRank=-1,lastCount=0;
    double update(int rank,int count,double dt,bool animate){
        if(rank<0||count<=0){value=-1;lastRank=-1;lastCount=count;return value;}
        rank=std::clamp(rank,0,count-1);
        double target=count==1?.5:double(rank)/(count-1);
        bool wrap=count>1&&lastCount==count&&((lastRank==count-1&&rank==0)||(lastRank==0&&rank==count-1));
        if(value<0||!animate||wrap||count!=lastCount)value=target;
        else value+=(target-value)*(1-std::exp(-22*std::clamp(dt,0.0,.05)));
        if(std::fabs(value-target)<.001)value=target;
        value=std::clamp(value,0.0,1.0);lastRank=rank;lastCount=count;return value;
    }
};
struct Bounds {float left,right,labelTop,barTop,barBottom;};
inline Bounds bounds(float width,float height,float ui,bool windowed){
    const float trackWidth=std::max(0.f,std::min(240*ui,width-96*ui));
    const float bottom=height-(windowed?44.f:20.f)*ui;
    return {(width-trackWidth)*.5f,(width+trackWidth)*.5f,bottom-34*ui,bottom-4*ui,bottom};
}
}

namespace CoverTabbyNavigation {
struct WinTabGesture {
    unsigned winKeys=0,tabPresses=0;
    unsigned long long winDownAt=0,chordStartedAt=0;
    bool winTimeKnown=false,chordActive=false;
    void win(unsigned bit,bool down,unsigned long long now) {
        if(down) {
            if(!winKeys) {winDownAt=now;winTimeKnown=true;}
            winKeys|=bit;
        } else winKeys&=~bit;
    }
    void tab(unsigned long long now) {
        if(!chordActive) {
            chordActive=true;tabPresses=0;
            chordStartedAt=winTimeKnown ? winDownAt : now;
        }
        if(tabPresses<2) ++tabPresses;
    }
    bool finish(unsigned long long now) {
        if(winKeys) return false;
        const bool commit=chordActive && (tabPresses>1 || now-chordStartedAt>=280);
        chordActive=false;tabPresses=0;winTimeKnown=false;
        return commit;
    }
};
inline int tabStep(bool expanded,bool backwards) {
    return (backwards ? -1 : 1)*(expanded ? 1 : 3);
}
}

namespace covertabby {
namespace wgc = winrt::Windows::Graphics::Capture;
namespace wgdx = winrt::Windows::Graphics::DirectX;
namespace wgd3d = winrt::Windows::Graphics::DirectX::Direct3D11;
using winrt::com_ptr;

namespace {

// The llvm-mingw SDK doesn't ship windows.graphics.directx.direct3d11.interop.h,
// so the interop pieces needed here are declared manually.
struct IDirect3DDxgiInterfaceAccess : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetInterface(REFIID iid, void** p) = 0;
};
constexpr GUID kIID_IDirect3DDxgiInterfaceAccess = {
    0xa9b3d012,
    0x3df2,
    0x4ee3,
    {0xb8, 0xd1, 0x86, 0x95, 0xf4, 0x57, 0xd3, 0xc1}};
constexpr GUID kIID_IGraphicsCaptureItemInterop = {
    0x3628e81b,
    0x3cac,
    0x4c60,
    {0xb7, 0xf4, 0x23, 0xce, 0x0e, 0x0c, 0x33, 0x56}};
constexpr GUID kCLSID_DesktopWallpaper = {
    0xc2cf3110,
    0x460e,
    0x4fc1,
    {0xb9, 0xd0, 0x8a, 0x1c, 0x0c, 0x9c, 0xc4, 0xbd}};
// Used to find the icons of packaged (UWP) apps.
constexpr GUID kFOLDERID_AppsFolder = {
    0x1e87508d,
    0x89c2,
    0x42f0,
    {0x8a, 0x7e, 0x64, 0x5a, 0x0f, 0x50, 0xca, 0x58}};
constexpr PROPERTYKEY kPKEY_AppUserModel_ID = {
    {0x9f4c2855,
     0x9f79,
     0x4b39,
     {0xa8, 0xd0, 0xe1, 0xd4, 0x2d, 0xe1, 0xd5, 0xf3}},
    5};

using CreateDirect3D11DeviceFromDXGIDevice_t = HRESULT(WINAPI*)(IDXGIDevice*,
                                                                ::IInspectable**);

// Effect property values missing from the llvm-mingw d2d1effects.h, as
// documented in the Windows SDK.
constexpr UINT32 kTransform3DPropInterpolationMode = 0;
constexpr UINT32 kTransform3DPropBorderMode = 1;
constexpr UINT32 kTransform3DPropTransformMatrix = 2;
constexpr UINT32 kTransform3DInterpolationAnisotropic = 4;
constexpr UINT32 kShadowOptimizationSpeed = 0;
constexpr UINT32 kGaussianBlurOptimizationQuality = 2;

// Messages posted from the hook thread to the overlay window.
constexpr UINT WM_APP_ICON_MENU_DISMISS=WM_APP+34;
constexpr UINT WM_APP_DESKTOP_OVERVIEW=WM_APP+32;
constexpr UINT WM_APP_DESKTOP_WIN_RELEASE=WM_APP+33;
constexpr UINT WM_APP_SEARCH = WM_APP + 30;
constexpr UINT WM_APP_SEARCHCHAR = WM_APP + 31;
constexpr UINT WM_APP_START = WM_APP + 1;   // wParam: kStart* flags
constexpr UINT WM_APP_STEP = WM_APP + 2;    // wParam: signed step
constexpr UINT WM_APP_WHEEL = WM_APP + 3;   // wParam: signed wheel delta
constexpr UINT WM_APP_COMMIT = WM_APP + 4;  // switch to the front window
constexpr UINT WM_APP_CANCEL = WM_APP + 5;  // go back without switching
constexpr UINT WM_APP_SETTINGS = WM_APP + 6;
constexpr UINT WM_APP_REFRESH_ICONS = WM_APP + 7;  // Posted to itself.

constexpr WPARAM kStartBackwards = 1;
// Open persistently from a touchpad gesture or the desktop overview.
constexpr WPARAM kStartFromHotkey = 2;

// Thread messages handled by the hook thread.
constexpr UINT WM_HOOK_ENGAGE = WM_APP + 20;
constexpr UINT WM_HOOK_DISENGAGE = WM_APP + 21;
// Installs or removes the keyboard hook, following g_takeOver.
constexpr UINT WM_HOOK_UPDATE = WM_APP + 22;

// Marks input injected by this mod, so the hooks can ignore it.
constexpr ULONG_PTR kInjectedInputTag = 0x46334453;
// Unassigned virtual key, used to stop Alt from activating menu bars.
constexpr WORD kDummyVk = 0xE8;
constexpr WPARAM kTextSelectAll = 1;
constexpr WPARAM kTextDeleteWord = 2;
constexpr WPARAM kTextBackspace = 8;
constexpr WPARAM kTextPaste = 22;

constexpr WCHAR kOverlayClassName[] = L"WindhawkCoverTabbyOverlay";
// The overlay's title tells explorer whether the switcher can take over
// Alt+Tab right now. FindWindow compares titles without sending messages.
constexpr WCHAR kOverlayTitleIdle[] = L"CoverTabby";
constexpr WCHAR kOverlayTitleReady[] = L"CoverTabby (ready)";
// Posted by explorer to the overlay when it receives the Alt+Tab hotkey.
// wParam: kStartBackwards or 0.

enum class MonitorMode { Cursor, ActiveWindow };

enum class BackdropStyle { Blur, Opaque, Transparent };
static int g_energySaverStatus=-1, g_batterySaverStatus=-1;
static constexpr GUID kEnergySaverGuid{0x550e8400,0xe29b,0x41d4,{0xa7,0x16,0x44,0x66,0x55,0x44,0x00,0x00}};
static constexpr GUID kBatterySaverGuid{0xe00958c0,0xc213,0x4ace,{0xac,0x77,0xfe,0xcc,0xed,0x2e,0xee,0xa5}};

static bool WindowsEnergySaverActive() {
    if (g_energySaverStatus>=0) return g_energySaverStatus!=0;
    if (g_batterySaverStatus>=0) return g_batterySaverStatus!=0;
    SYSTEM_POWER_STATUS power{};
    return GetSystemPowerStatus(&power) && power.SystemStatusFlag==1;
}

static BackdropStyle EffectiveBackdrop(BackdropStyle selected,bool saver,bool follow,BackdropStyle replacement) {
    return saver && follow ? replacement : selected;
}

static float BackdropOpacity(BackdropStyle style,bool windowed,float transparent) {
    return style==BackdropStyle::Opaque ? 1.0f : style==BackdropStyle::Transparent ?
        std::clamp(transparent,0.0f,1.0f) : (windowed ? 0.65f : 0.18f);
}

static std::vector<int> CaptureOrder(int count,int selected) {
    std::vector<int> order;
    if(count<=0) return order;
    selected=std::clamp(selected,0,count-1);
    order.push_back(selected);
    for(int distance=1;(int)order.size()<count;++distance) {
        for(int direction:{1,-1}) {
            const int index=(selected+direction*distance+count)%count;
            if(std::find(order.begin(),order.end(),index)==order.end()) order.push_back(index);
        }
    }
    return order;
}

struct Settings {
    bool italian = false;
    bool typingAnimation = false;
    bool windowMode = false;
    int enchantedMaterial = 0; // System, dark, light, acrylic.
    int simpleStyle = 0; // 1: icon strip, 2: icon/title list, 3: centered text list, 4: searchable thumbnails.
    bool originalAnimations = false;
    bool disable3DWindows = false;
    bool darkBackground = true;
    bool blurBackground = true;
    bool showDesktopCard = true;
    bool holdToPreview = false;
    enum class MinimizedTint {Accent,Gray,None};
    MinimizedTint minimizedTint=MinimizedTint::Accent;
    BackdropStyle backdropStyle=BackdropStyle::Blur;
    bool energySaver=false, previewCache=true, touchpadGestures=true, desktopOverviewHotkey=false;
    bool desktopOverviewHoldWin=true, otherDesktopWindows=false;
    bool aero3D=false, liveDesktopWallpaper=true;
    bool ignoreRepeatedNavigationKeys=true;
    float transparentOpacity=0.35f;
    bool compactMaterial = true;
    D2D1_COLOR_F backgroundColor = D2D1::ColorF(0.125f,0.125f,0.125f,1);
    float backgroundOpacity = 0.65f;
    std::wstring fontFamily = L"Segoe UI";
    DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
    DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL;
    D2D1_COLOR_F borderColor{}, highlightColor{}, iconBackgroundColor{};
    float iconBackgroundOpacity = 0.2f;
    float panelRadius = 8;
    int popupWidthPercent = 80, popupHeightPercent = 80;
    int previewHeight = 230, previewWidth = 0;
    bool showSwitcherBorder = true, showThumbnails = true;
    bool animateSelection = true;
    bool animateHover = true;
    bool showCloseButton = true;
    bool showIcons = true;
    bool selectionBar = true;
    double hoverSelectionDelay = 0.35;
    bool hoverSelects = false;
    bool hoverShowsTitle = true;
    bool hoverLift = true, hoverEnlarge = false;
    bool enableSearch = true;
    float searchBarScale = 1.0f;
    int searchBarWidth = 0;
    float windowScale = 1;
    int titleSize = 20, hoverTitleSize = 14;
    int iconSize = 46, iconSpacing = 64, visibleIcons = 11;
    float sideWindowScale = 1, sideWindowSpacing = 1;
    float sideWindowBrightness = 0.8f, sideWindowOpacity = 1;
    bool compactCoverFlow = true;
    bool reflections = true;
    bool groupPrograms = true;
    bool tileGrid = false;
    bool tileSelectedBigger = false;
    bool windowBorders = true;
    bool hoverControls = true;
    MonitorMode monitor;
    int animationDurationMs;
    float flipSpeed;
    int showDelayMs;
    float tiltRadians;
    float stackSpacing;
    int maxWindows;
    float blurAmount;
    bool showTitle;
    bool shadows;
    bool includeMinimized;
    bool minimizedContent;
};

Settings LoadSettings() {
    Settings s;
    const auto hoverAction = WindhawkUtils::StringSetting::make(L"hoverAction");
    s.hoverSelects = wcscmp(hoverAction, L"select") == 0;
    s.hoverShowsTitle = !s.hoverSelects && wcscmp(hoverAction, L"none") != 0;
    const auto hoverAnimation = WindhawkUtils::StringSetting::make(L"hoverAnimation");
    s.hoverLift = wcscmp(hoverAnimation, L"lift") == 0 || wcscmp(hoverAnimation, L"liftEnlarge") == 0;
    s.hoverEnlarge = wcscmp(hoverAnimation, L"enlarge") == 0 || wcscmp(hoverAnimation, L"liftEnlarge") == 0;
    const auto theme = WindhawkUtils::StringSetting::make(L"theme");
    s.windowMode = wcscmp(theme, L"windowed") == 0;
    const auto style = WindhawkUtils::StringSetting::make(L"style");
    s.simpleStyle = wcscmp(style,L"icons")==0 ? 1 : wcscmp(style,L"list")==0 ? 2 : wcscmp(style,L"text")==0 ? 3 : wcscmp(style,L"search")==0 ? 4 : wcscmp(style,L"enchanted")==0 ? 5 : 0;
    s.aero3D=wcscmp(style,L"aero3d")==0;
    const auto material=WindhawkUtils::StringSetting::make(L"enchantedMaterial");
    s.enchantedMaterial=wcscmp(material,L"dark")==0 ? 1 : wcscmp(material,L"light")==0 ? 2 : wcscmp(material,L"acrylic")==0 ? 3 : 0;
    s.tileGrid = wcscmp(style, L"tiles") == 0;
    const auto tileSelectedStyle = WindhawkUtils::StringSetting::make(L"tileSelectedStyle");
    s.tileSelectedBigger = wcscmp(tileSelectedStyle, L"bigger") == 0;
    const auto animation = WindhawkUtils::StringSetting::make(L"animation");
    s.originalAnimations = wcscmp(animation, L"original") == 0;
    s.groupPrograms = ::LoadBoolSetting(L"groupWindows");
    s.selectionBar = ::LoadBoolSetting(L"selectionBar");
    s.windowScale = std::clamp(::LoadIntSetting(L"windowScale"), 25, 200) / 100.0f;
    s.enableSearch = ::LoadBoolSetting(L"searchEnabled");
    // Windowed mode is intentionally a compact picker: keep the area clear
    // for the selected preview and its icon/title, without search or dragging.
    if (s.windowMode) s.enableSearch = false;
    if (s.tileGrid) {
        s.groupPrograms = false;
        s.enableSearch = false;
    }
    s.searchBarScale = std::clamp(::LoadIntSetting(L"searchScale"), 50, 200) / 100.0f;
    s.searchBarWidth = 0;
    s.iconSize = (int)std::round(32 * std::clamp(::LoadIntSetting(L"iconScale"), 50, 300) / 100.0f);
    s.disable3DWindows = !::LoadBoolSetting(L"enable3D");
    s.reflections = ::LoadBoolSetting(L"reflections");
    const auto backdrop=WindhawkUtils::StringSetting::make(L"backgroundStyle");
    BackdropStyle selected=BackdropStyle::Blur;
    if (wcscmp(backdrop,L"opaque")==0) selected=BackdropStyle::Opaque;
    else if (wcscmp(backdrop,L"transparent")==0) selected=BackdropStyle::Transparent;
    const auto saverBackground=WindhawkUtils::StringSetting::make(L"energySaverBackground");
    s.energySaver=WindowsEnergySaverActive();
    s.backdropStyle=EffectiveBackdrop(selected,s.energySaver,wcscmp(saverBackground,L"unchanged")!=0,
        wcscmp(saverBackground,L"transparent")==0 ? BackdropStyle::Transparent : BackdropStyle::Opaque);
    s.blurBackground=s.backdropStyle==BackdropStyle::Blur;
    s.holdToPreview=::LoadBoolSetting(L"holdToPreview");
    s.showDesktopCard=::LoadBoolSetting(L"showDesktopCard");
    const auto minimizedTint=WindhawkUtils::StringSetting::make(L"minimizedPreviewTint");
    s.minimizedTint=wcscmp(minimizedTint,L"none")==0 ? Settings::MinimizedTint::None :
        (wcscmp(minimizedTint,L"gray")==0 ? Settings::MinimizedTint::Gray : Settings::MinimizedTint::Accent);
    s.transparentOpacity=std::clamp(::LoadIntSetting(L"transparentOpacity"),0,100)/100.0f;
    s.previewCache=::LoadBoolSetting(L"previewCache");
    s.touchpadGestures=::LoadBoolSetting(L"touchpadGestures");
    s.desktopOverviewHotkey=::LoadBoolSetting(L"desktopOverviewHotkey");
    s.desktopOverviewHoldWin=::LoadBoolSetting(L"desktopOverviewHoldWin");
    s.otherDesktopWindows=::LoadBoolSetting(L"otherDesktopWindows");
    s.liveDesktopWallpaper=::LoadBoolSetting(L"liveDesktopWallpaper");
    s.ignoreRepeatedNavigationKeys=::LoadBoolSetting(L"ignoreRepeatedNavigationKeys");
    s.italian = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_ITALIAN;
    s.typingAnimation = ::LoadBoolSetting(L"typingAnimation");

    // Appearance follows Windows 11; no separate color/font/corner controls.
    DWORD light = 0, bytes = sizeof(light);
    RegGetValueW(HKEY_CURRENT_USER,
        LR"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &bytes);
    s.darkBackground = !light;
    if(s.enchantedMaterial==1 || s.enchantedMaterial==2) s.darkBackground=s.enchantedMaterial==1;
    s.backgroundColor = s.darkBackground
        ? D2D1::ColorF(0.125f, 0.125f, 0.125f, 1)
        : D2D1::ColorF(0.953f, 0.953f, 0.953f, 1);
    s.backgroundOpacity=BackdropOpacity(s.backdropStyle,s.windowMode,s.transparentOpacity);
    s.compactMaterial = false;
    const COLORREF accent = ::GetAccentColor();
    s.borderColor = s.highlightColor = D2D1::ColorF(
        GetRValue(accent) / 255.0f, GetGValue(accent) / 255.0f, GetBValue(accent) / 255.0f, 1);
    s.iconBackgroundColor = s.highlightColor;
    s.iconBackgroundOpacity = 0.2f;
    s.fontFamily = L"Segoe UI Variable Text";
    s.fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
    s.fontStyle = DWRITE_FONT_STYLE_NORMAL;
    s.titleSize = 20;
    s.hoverTitleSize = 14;
    s.panelRadius = 8;

    BOOL systemAnimations = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &systemAnimations, 0);
    s.animateSelection = s.animateHover = systemAnimations != FALSE;
    s.animationDurationMs = systemAnimations ? 160 : 1;
    s.monitor = MonitorMode::Cursor;
    s.flipSpeed = 1;
    s.showDelayMs = 0;
    s.tiltRadians = 20 * 3.14159265f / 180.0f;
    s.stackSpacing = 1;
    s.maxWindows = 40;
    s.blurAmount = 20;
    s.windowBorders = s.showSwitcherBorder = true;
    s.hoverControls = s.showCloseButton = true;
    s.showTitle = s.showIcons = s.showThumbnails = true;
    s.shadows = true;
    s.includeMinimized = s.minimizedContent = true;
    if (s.simpleStyle) { s.groupPrograms=false; s.enableSearch=s.simpleStyle>=4; s.showThumbnails=s.simpleStyle>=4; s.hoverControls=false; }
    if(s.aero3D) {s.groupPrograms=false;s.compactCoverFlow=false;s.showThumbnails=true;s.disable3DWindows=false;}
    return s;
}
HMODULE g_module;
HANDLE g_uiThread;
DWORD g_uiThreadId;
HANDLE g_hookThread;
DWORD g_hookThreadId;
HWND g_overlayWnd;
HHOOK g_keyboardHook;  // Hook thread only.
HHOOK g_mouseHook;     // Hook thread only.

// The switcher is initialized and can take over Alt+Tab.
std::atomic<bool> g_ready;
std::atomic<bool> g_simpleNavigation;
// Alt+Tab is taken over (false with the native switcher style).
std::atomic<bool> g_takeOver;
// An Alt+Tab session is in progress (Alt is still held).
std::atomic<bool> g_switching;
std::atomic<bool> g_stickyControls{false};
std::atomic<bool> g_searchTyping{false};
std::atomic<ULONGLONG> g_lastNavigationInputTick{0};
std::atomic<bool> g_searchEnabled{false};
std::atomic<bool> g_searchHasResults{true};
std::atomic<bool> g_desktopOverviewHotkey{false};
std::atomic<bool> g_desktopOverviewHoldWin{true};
std::atomic<bool> g_ignoreRepeatedNavigationKeys{true};
std::atomic<bool> g_desktopWinHoldSession{false};
std::atomic<bool> g_iconMenuActive{false};
std::atomic<unsigned> g_physicalWinKeys{0};
std::atomic<bool> g_desktopOverviewActive{false};
std::atomic<bool> g_desktopRenameTyping{false};



double NowSeconds() {
    static const double frequency = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return (double)f.QuadPart;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart / frequency;
}

float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float PositiveMod(float value, float modulus) {
    float result = std::fmod(value, modulus);
    return result < 0 ? result + modulus : result;
}

int PositiveMod(long long value, int modulus) {
    long long result = value % modulus;
    return (int)(result < 0 ? result + modulus : result);
}

bool IsKeyDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

static bool IsHighSurrogate(wchar_t value) {
    return value >= 0xD800 && value <= 0xDBFF;
}

static bool IsLowSurrogate(wchar_t value) {
    return value >= 0xDC00 && value <= 0xDFFF;
}

static void EraseLastUtf16CodePoint(std::wstring& text) {
    if (text.empty()) return;
    const bool paired = IsLowSurrogate(text.back()) && text.size() >= 2 &&
                        IsHighSurrogate(text[text.size() - 2]);
    text.resize(text.size() - (paired ? 2 : 1));
}

static void EraseLastWord(std::wstring& text) {
    while (!text.empty() && iswspace(text.back()))
        EraseLastUtf16CodePoint(text);
    while (!text.empty() && !iswspace(text.back()))
        EraseLastUtf16CodePoint(text);
}

static void AppendUtf16Unit(std::wstring& text,wchar_t value,size_t maximumLength) {
    if(IsHighSurrogate(value)) {
        if(text.size()+1<maximumLength) text.push_back(value);
        return;
    }
    if(IsLowSurrogate(value) && (text.empty() || !IsHighSurrogate(text.back()))) return;
    if(text.size()<maximumLength) text.push_back(value);
}

static std::wstring ReadClipboardText(HWND owner, size_t maximumLength) {
    std::wstring result;
    if (!OpenClipboard(owner)) return result;
    HANDLE data = GetClipboardData(CF_UNICODETEXT);
    const auto* source = data ? static_cast<const wchar_t*>(GlobalLock(data)) : nullptr;
    if (source) {
        bool previousSpace = false;
        while (*source && result.size() < maximumLength) {
            wchar_t value = *source++;
            if (value < 32) {
                if (!previousSpace && !result.empty()) result.push_back(L' ');
                previousSpace = true;
                continue;
            }
            if(IsHighSurrogate(value) && IsLowSurrogate(*source)) {
                if(result.size()+2>maximumLength) break;
                result.push_back(value);
                result.push_back(*source++);
            } else if(!IsLowSurrogate(value)) {
                result.push_back(value);
            }
            previousSpace = false;
        }
        GlobalUnlock(data);
    }
    CloseClipboard();
    return result;
}

static bool ToggleWindowsDesktop() {
    com_ptr<IShellDispatch4> shell;
    const HRESULT create=CoCreateInstance(CLSID_Shell,nullptr,
        CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER,IID_PPV_ARGS(shell.put()));
    if(FAILED(create)) {
        Wh_Log(L"Show Desktop unavailable: 0x%08X",(UINT)create);
        return false;
    }
    const HRESULT toggle=shell->ToggleDesktop();
    if(FAILED(toggle)) Wh_Log(L"Show Desktop failed: 0x%08X",(UINT)toggle);
    return SUCCEEDED(toggle);
}

// Injects an unassigned key press. Sent right after the swallowed Tab, it stops
// the foreground app from treating the Alt release as a menu activation. It
// also makes this process the source of the last input, which allows it to
// call SetForegroundWindow.
void SendDummyKeyPress() {
    INPUT inputs[2] = {};
    for (INPUT& input : inputs) {
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = kDummyVk;
        input.ki.dwExtraInfo = kInjectedInputTag;
    }
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

////////////////////////////////////////////////////////////////////////////////
// Low-level hooks (hook thread).

// Ends the session from any thread. Returns false if it was already ended.
bool EndSwitching() {
    if (!g_switching.exchange(false)) {
        return false;
    }
    PostThreadMessageW(g_hookThreadId, WM_HOOK_DISENGAGE, 0, 0);
    return true;
}



LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    const auto* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
    if (kb->dwExtraInfo == kInjectedInputTag) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    const bool keyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    const DWORD vk = kb->vkCode;
    static bool shortcutHeld[10]{};
    if(vk>='0' && vk<='9' && !keyDown) {
        const bool consumed=shortcutHeld[vk-'0'];
        shortcutHeld[vk-'0']=false;
        if(consumed) return 1;
    }
    if(g_switching && g_simpleNavigation && vk>='0' && vk<='9' && (kb->flags & LLKHF_ALTDOWN)) {
        if(keyDown && !shortcutHeld[vk-'0']) PostMessageW(g_overlayWnd,WM_APP+36,vk-'0',0);
        shortcutHeld[vk-'0']=keyDown;
        return 1;
    }
    // Track releases even outside CoverTabby, so a new physical press remains
    // distinct from Windows keyboard auto-repeat. Leave ordinary app input alone.
    static bool navigationHeld[256]{};
    const bool navigationKey = vk == VK_TAB || vk == VK_OEM_3 || vk == VK_OEM_8 ||
        vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN;
    if (navigationKey) {
        if(keyDown && g_switching) g_lastNavigationInputTick=GetTickCount64();
        const bool repeated = keyDown && navigationHeld[vk];
        navigationHeld[vk] = keyDown;
        const bool altNavigation = (kb->flags & LLKHF_ALTDOWN) &&
            (vk == VK_TAB || vk == VK_OEM_3 || vk == VK_OEM_8) &&
            !IsKeyDown(VK_CONTROL) && !IsKeyDown(VK_LWIN) && !IsKeyDown(VK_RWIN);
        const bool winNavigation = vk == VK_TAB && g_desktopOverviewHotkey &&
            (IsKeyDown(VK_LWIN) || IsKeyDown(VK_RWIN));
        if (repeated && g_ignoreRepeatedNavigationKeys && g_takeOver &&
            (g_switching || altNavigation || winNavigation)) {
            return 1;
        }
    }
    if(g_iconMenuActive && keyDown && vk==VK_ESCAPE) {
        PostMessageW(g_overlayWnd,WM_APP_ICON_MENU_DISMISS,0,0);return 1;
    }
    if(keyDown && (kb->flags & LLKHF_ALTDOWN) &&
        (vk==VK_TAB || vk==VK_OEM_3 || vk==VK_OEM_8) && g_desktopOverviewActive)
        g_stickyControls=false;
    static bool desktopTabHeld=false;
    static CoverTabbyNavigation::WinTabGesture desktopGesture;
    static bool leftWinDown=false,rightWinDown=false;
    static unsigned handledWinReleases=0;
    if(vk==VK_LWIN) leftWinDown=keyDown;
    if(vk==VK_RWIN) rightWinDown=keyDown;
    if(vk==VK_LWIN || vk==VK_RWIN)
        g_physicalWinKeys=(leftWinDown ? 1u : 0u)|(rightWinDown ? 2u : 0u);
    if(vk==VK_LWIN || vk==VK_RWIN)
        desktopGesture.win(vk==VK_LWIN ? 1u : 2u,keyDown,GetTickCount64());
    if(!keyDown && (vk==VK_LWIN || vk==VK_RWIN)) {
        const unsigned bit=vk==VK_LWIN ? 1 : 2;
        if(handledWinReleases & bit) {
            handledWinReleases&=~bit;
            if(!leftWinDown && !rightWinDown) {
                const bool held=desktopGesture.finish(GetTickCount64());
                const bool active=g_desktopWinHoldSession.exchange(false);
                if(active && g_desktopOverviewHoldWin && held)
                    PostMessageW(g_overlayWnd,WM_APP_DESKTOP_WIN_RELEASE,0,0);
            }
            // Swallow the physical release. Queue a harmless chord key before
            // the tagged Win-up, so Start never receives a bare Windows tap.
            INPUT release[3]{};
            for(auto& input:release) {input.type=INPUT_KEYBOARD;input.ki.dwExtraInfo=kInjectedInputTag;}
            release[0].ki.wVk=kDummyVk;
            release[1].ki.wVk=kDummyVk;release[1].ki.dwFlags=KEYEVENTF_KEYUP;
            release[2].ki.wVk=(WORD)vk;release[2].ki.dwFlags=KEYEVENTF_KEYUP;
            const UINT sent=SendInput(3,release,sizeof(INPUT));
            if(sent!=3) {
                Wh_Log(L"Win+Tab masked release failed: %u",GetLastError());
                return CallNextHookEx(nullptr,code,wParam,lParam);
            }
            return 1;
        }
        if(!leftWinDown && !rightWinDown) desktopGesture.finish(GetTickCount64());
    }
    if(vk==VK_TAB && !keyDown && desktopTabHeld) {
        desktopTabHeld=false;return 1;
    }

    if(keyDown && vk==VK_TAB && (leftWinDown || rightWinDown || IsKeyDown(VK_LWIN) || IsKeyDown(VK_RWIN)) &&
        g_takeOver && g_desktopOverviewHotkey) {
        if(desktopTabHeld) return 1;
        desktopTabHeld=true;
        desktopGesture.tab(GetTickCount64());
        // Mask the completed chord on release; never hold Ctrl synthetically.
        if(leftWinDown || IsKeyDown(VK_LWIN)) handledWinReleases|=1;
        if(rightWinDown || IsKeyDown(VK_RWIN)) handledWinReleases|=2;
        if(!g_ready || !g_overlayWnd) return 1;
        g_switching=true;g_stickyControls=true;
        g_desktopWinHoldSession=g_desktopOverviewHoldWin.load();
        PostMessageW(g_overlayWnd,WM_APP_DESKTOP_OVERVIEW,IsKeyDown(VK_SHIFT) ? 1 : 0,0);
        PostThreadMessageW(g_hookThreadId,WM_HOOK_ENGAGE,0,0);
        return 1;
    }
    if (keyDown && (vk==VK_OEM_3 || vk==VK_OEM_8) && (kb->flags & LLKHF_ALTDOWN) &&
        g_ready && g_takeOver && !IsKeyDown(VK_CONTROL) && !IsKeyDown(VK_LWIN) && !IsKeyDown(VK_RWIN)) {
        if(!g_switching.exchange(true)) {
            PostMessageW(g_overlayWnd,WM_APP_START,kStartBackwards,0);
            PostThreadMessageW(g_hookThreadId,WM_HOOK_ENGAGE,0,0);
        } else PostMessageW(g_overlayWnd,WM_APP_STEP,(WPARAM)(INT_PTR)-1,0);
        return 1;
    }
    // Begin filtering with the first letter; keep the current visual style.
    // Activate synchronously in the hook so subsequent queued keys cannot leak.
    static bool searchLetterHeld[26]{};
    if(vk>='A' && vk<='Z' && !keyDown && searchLetterHeld[vk-'A']) {
        searchLetterHeld[vk-'A']=false;return 1;
    }
    if(keyDown && vk>='A' && vk<='Z' && g_switching && g_searchEnabled &&
        !g_searchTyping && !g_desktopRenameTyping && !g_desktopOverviewActive &&
        !IsKeyDown(VK_CONTROL) && !IsKeyDown(VK_LWIN) && !IsKeyDown(VK_RWIN)) {
        g_searchTyping=true;g_stickyControls=true;searchLetterHeld[vk-'A']=true;
        PostMessageW(g_overlayWnd,WM_APP_SEARCH,0,0);
        // Fall through to the existing Unicode input path to retain this letter.
    }
    if (keyDown && vk=='S' && g_switching && !IsKeyDown(VK_SHIFT) &&
        !IsKeyDown(VK_CONTROL) && (kb->flags & LLKHF_ALTDOWN) && g_ready && g_searchEnabled) {
        g_stickyControls=true;
        PostMessageW(g_overlayWnd,WM_APP_SEARCH,0,0);
        return 1;
    }
    if (g_switching && (g_searchTyping || g_desktopRenameTyping) && keyDown && vk!=VK_RETURN && vk!=VK_ESCAPE &&
        vk!=VK_TAB && vk!=VK_HOME && vk!=VK_END && vk!=VK_LEFT && vk!=VK_RIGHT && vk!=VK_UP && vk!=VK_DOWN &&
        vk!=VK_MENU && vk!=VK_LMENU && vk!=VK_RMENU && vk!=VK_SHIFT && vk!=VK_LSHIFT && vk!=VK_RSHIFT) {
        const bool control=IsKeyDown(VK_CONTROL);
        if (control && vk=='A') PostMessageW(g_overlayWnd,WM_APP_SEARCHCHAR,kTextSelectAll,0);
        else if (control && vk=='V') PostMessageW(g_overlayWnd,WM_APP_SEARCHCHAR,kTextPaste,0);
        else if (control && vk==VK_BACK) PostMessageW(g_overlayWnd,WM_APP_SEARCHCHAR,kTextDeleteWord,0);
        else if (control) { /* Keep unsupported editing shortcuts out of apps. */ }
        else if (vk==VK_BACK) PostMessageW(g_overlayWnd,WM_APP_SEARCHCHAR,kTextBackspace,0);
        else {
            BYTE state[256]{}; GetKeyboardState(state);
            state[VK_MENU]=state[VK_LMENU]=state[VK_RMENU]=0;
            state[VK_SHIFT]=IsKeyDown(VK_SHIFT) ? 0x80 : 0;
            wchar_t chars[8]{};
            const int length=ToUnicodeEx(vk,kb->scanCode,state,chars,8,0,GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(),nullptr)));
            for (int i=0;i<length;++i) if(chars[i]>=32) PostMessageW(g_overlayWnd,WM_APP_SEARCHCHAR,chars[i],0);
        }
        return 1;
    }
    // CoverTabby owns Alt+Tab while enabled, including renderer recovery.
    // Never let a temporarily unavailable renderer open the native switcher.
    static bool altTabHeld=false;
    if(vk==VK_TAB && !keyDown && altTabHeld) {altTabHeld=false;return 1;}
    const bool coverAltTab=keyDown && vk==VK_TAB && (kb->flags & LLKHF_ALTDOWN) &&
        g_takeOver && !IsKeyDown(VK_CONTROL) && !IsKeyDown(VK_LWIN) && !IsKeyDown(VK_RWIN);
    if(coverAltTab) {
        altTabHeld=true;
        if(!g_ready || !g_overlayWnd) return 1;
    }
    if (!g_switching) {
        if(coverAltTab) {
            g_switching = true;
            PostMessageW(g_overlayWnd, WM_APP_START,
                         IsKeyDown(VK_SHIFT) ? kStartBackwards : 0, 0);
            PostThreadMessageW(g_hookThreadId, WM_HOOK_ENGAGE, 0, 0);
            return 1;
        }
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    switch (vk) {
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
            if (!keyDown && !g_stickyControls && EndSwitching()) {
                PostMessageW(g_overlayWnd, WM_APP_COMMIT, 0, 0);
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);

        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_LWIN:
        case VK_RWIN:
            return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    // While switching, every other key belongs to the switcher. Otherwise the
    // foreground app would receive them as Alt+key accelerators.
    if (keyDown) {
        int step = 0;
        switch (vk) {
            case VK_HOME:
            case VK_END:
                PostMessageW(g_overlayWnd,WM_APP+37,vk==VK_END,0);
                break;
            case VK_TAB:
                step = IsKeyDown(VK_SHIFT) ? -1 : 1;
                break;
            case VK_RIGHT:
                step = 3; // Next application group.
                break;
            case VK_LEFT:
                step = -3;
                break;
            case VK_DOWN:
                step = 2; // Next window within this application.
                break;
            case VK_UP:
                step = -2;
                break;
            case VK_ESCAPE:
                if (EndSwitching()) {
                    PostMessageW(g_overlayWnd, WM_APP_CANCEL, 0, 0);
                }
                break;
            case VK_RETURN:
                if (g_searchTyping && !g_searchHasResults) break;
            case VK_SPACE:
                if (EndSwitching()) {
                    PostMessageW(g_overlayWnd, WM_APP_COMMIT, 0, 0);
                }
                break;
        }
        if (step) {
            PostMessageW(g_overlayWnd, WM_APP_STEP, (WPARAM)(INT_PTR)step, 0);
        }
    }
    return 1;
}

LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && wParam == WM_MOUSEWHEEL && g_switching) {
        const auto* ms = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
        if (ms->dwExtraInfo != kInjectedInputTag) {
            const short delta = (short)HIWORD(ms->mouseData);
            PostMessageW(g_overlayWnd, WM_APP_WHEEL, (WPARAM)(INT_PTR)delta,
                         0);
            return 1;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// The keyboard hook is only installed while Alt+Tab is taken over, so the
// native style leaves no system-wide hook behind.
void UpdateKeyboardHook() {
    if (g_takeOver && !g_keyboardHook) {
        g_keyboardHook =
            SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, g_module, 0);
        if (!g_keyboardHook) {
            Wh_Log(L"SetWindowsHookEx(WH_KEYBOARD_LL) failed: %u",
                   GetLastError());
        }
    } else if (!g_takeOver && g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
}

DWORD WINAPI HookThreadProc(LPVOID parameter) {
    MSG msg;
    // Create the message queue before the creator continues.
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    UpdateKeyboardHook();
    SetEvent((HANDLE)parameter);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd) {
            DispatchMessageW(&msg);
            continue;
        }

        switch (msg.message) {
            case WM_HOOK_UPDATE:
                UpdateKeyboardHook();
                break;

            case WM_HOOK_ENGAGE:
                SendDummyKeyPress();
                if (!g_mouseHook) {
                    g_mouseHook =
                        SetWindowsHookExW(WH_MOUSE_LL, MouseProc, g_module, 0);
                }
                break;

            case WM_HOOK_DISENGAGE:
                if (g_mouseHook && !g_switching) {
                    UnhookWindowsHookEx(g_mouseHook);
                    g_mouseHook = nullptr;
                }
                break;
        }
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Window list.

bool IsSwitchableWindow(HWND hwnd,bool includeCloaked=false) {
    if (hwnd == g_overlayWnd || GetPropW(hwnd,L"ITaskList_Deleted") || !IsWindowVisible(hwnd) ||
        hwnd == GetShellWindow()) {
        return false;
    }

    const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_APPWINDOW)) {
        if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
            return false;
        }
        if (GetWindow(hwnd, GW_OWNER)) {
            return false;
        }
    }

    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked && (!includeCloaked || (cloaked & DWM_CLOAKED_APP))) {
        return false;
    }

    WCHAR className[64];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        for (PCWSTR excluded : {L"Progman", L"WorkerW", L"Shell_TrayWnd",
                                L"Shell_SecondaryTrayWnd"}) {
            if (wcscmp(className, excluded) == 0) {
                return false;
            }
        }
    }
    return true;
}

// Window bounds in physical pixels. For minimized windows, the restored bounds.
bool GetWindowBounds(HWND hwnd, bool minimized, RECT* rect) {
    if (minimized) {
        WINDOWPLACEMENT placement{};
        placement.length=sizeof(placement);
        if (!GetWindowPlacement(hwnd, &placement)) {
            return false;
        }
        // rcNormalPosition is in workspace coordinates.
        *rect = placement.rcNormalPosition;
        HMONITOR monitor = MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info{};
        info.cbSize=sizeof(info);
        if (GetMonitorInfoW(monitor, &info)) {
            if (placement.flags & WPF_RESTORETOMAXIMIZED) {
                *rect = info.rcWork;
            } else if (!(GetWindowLongPtrW(hwnd, GWL_EXSTYLE) &
                         WS_EX_TOOLWINDOW)) {
                OffsetRect(rect, info.rcWork.left - info.rcMonitor.left,
                           info.rcWork.top - info.rcMonitor.top);
            }
        }
    } else if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                            rect, sizeof(*rect)))) {
        if (!GetWindowRect(hwnd, rect)) {
            return false;
        }
    }
    return rect->right - rect->left > 0 && rect->bottom - rect->top > 0;
}

HICON GetWindowIcon(HWND hwnd) {
    DWORD_PTR result = 0;
    for (WPARAM type : {(WPARAM)ICON_BIG, (WPARAM)ICON_SMALL2}) {
        if (SendMessageTimeoutW(hwnd, WM_GETICON, type, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &result) &&
            result) {
            return (HICON)result;
        }
    }
    if (HICON icon = (HICON)GetClassLongPtrW(hwnd, GCLP_HICON)) {
        return icon;
    }
    return (HICON)GetClassLongPtrW(hwnd, GCLP_HICONSM);
}

////////////////////////////////////////////////////////////////////////////////
// Geometry.

struct Pose {
    float x = 0, y = 0, z = 0;  // Card center, relative to the screen center.
    float scale = 1;
    float angleY = 0;  // Turn around the vertical axis, in radians.
    float angleX = 0;  // Tilt around the horizontal axis, in radians.
    float angleZ = 0;  // Roll in the screen plane, in radians.
    float opacity = 1;
    float brightness = 1;
    float highlight = 0;  // Selection outline, used by the grid.
};

Pose LerpPose(const Pose& a, const Pose& b, float t) {
    return {Lerp(a.x, b.x, t),
            Lerp(a.y, b.y, t),
            Lerp(a.z, b.z, t),
            Lerp(a.scale, b.scale, t),
            Lerp(a.angleY, b.angleY, t),
            Lerp(a.angleX, b.angleX, t),
            Lerp(a.angleZ, b.angleZ, t),
            Lerp(a.opacity, b.opacity, t),
            Lerp(a.brightness, b.brightness, t),
            Lerp(a.highlight, b.highlight, t)};
}

// Corners in order: top-left, top-right, bottom-right, bottom-left.
struct Quad {
    D2D1_POINT_2F p[4];
};

bool QuadContains(const Quad& quad, D2D1_POINT_2F pt) {
    bool hasPositive = false;
    bool hasNegative = false;
    for (int i = 0; i < 4; i++) {
        const D2D1_POINT_2F& a = quad.p[i];
        const D2D1_POINT_2F& b = quad.p[(i + 1) % 4];
        const float cross =
            (b.x - a.x) * (pt.y - a.y) - (b.y - a.y) * (pt.x - a.x);
        hasPositive |= cross > 0;
        hasNegative |= cross < 0;
    }
    return !(hasPositive && hasNegative);
}

// Projective transform mapping the (0,0)-(width,height) rectangle onto a quad
// (Heckbert's square-to-quad mapping), as a row-vector 4x4 matrix for D2D.
D2D1_MATRIX_4X4_F RectToQuadMatrix(float width, float height, const Quad& q) {
    const float x0 = q.p[0].x, y0 = q.p[0].y;
    const float x1 = q.p[1].x, y1 = q.p[1].y;
    const float x2 = q.p[2].x, y2 = q.p[2].y;
    const float x3 = q.p[3].x, y3 = q.p[3].y;

    const float sx = x0 - x1 + x2 - x3;
    const float sy = y0 - y1 + y2 - y3;

    float a, b, d, e, g, h;
    if (std::fabs(sx) < 1e-3f && std::fabs(sy) < 1e-3f) {
        a = x1 - x0;
        b = x3 - x0;
        d = y1 - y0;
        e = y3 - y0;
        g = 0;
        h = 0;
    } else {
        const float dx1 = x1 - x2, dx2 = x3 - x2;
        const float dy1 = y1 - y2, dy2 = y3 - y2;
        float den = dx1 * dy2 - dx2 * dy1;
        if (std::fabs(den) < 1e-6f) {
            den = den < 0 ? -1e-6f : 1e-6f;
        }
        g = (sx * dy2 - dx2 * sy) / den;
        h = (dx1 * sy - sx * dy1) / den;
        a = x1 - x0 + g * x1;
        b = x3 - x0 + h * x3;
        d = y1 - y0 + g * y1;
        e = y3 - y0 + h * y3;
    }

    D2D1_MATRIX_4X4_F m{};
    m._11 = a / width;
    m._12 = d / width;
    m._14 = g / width;
    m._21 = b / height;
    m._22 = e / height;
    m._24 = h / height;
    m._33 = 1;
    m._41 = x0;
    m._42 = y0;
    m._44 = 1;
    return m;
}

float EaseOutQuart(float t) {
    const float u = 1 - t;
    return 1 - u * u * u * u;
}

float EaseInOutCubic(float t) {
    const float u = -2 * t + 2;
    return t < 0.5f ? 4 * t * t * t : 1 - u * u * u / 2;
}

struct Tween {
    float from = 0;
    float to = 0;
    double start = 0;
    double duration = 0;
    bool easeInOut = false;

    float Progress(double now) const {
        if (duration <= 0) {
            return 1;
        }
        return (float)std::clamp((now - start) / duration, 0.0, 1.0);
    }
    float Value(double now) const {
        const float p = Progress(now);
        return Lerp(from, to, easeInOut ? EaseInOutCubic(p) : EaseOutQuart(p));
    }
    bool Done(double now) const { return Progress(now) >= 1; }
};

////////////////////////////////////////////////////////////////////////////////
// The switcher (UI thread).

struct EffectChain {
    com_ptr<ID2D1Effect> transform;
    com_ptr<ID2D1Effect> color;
    com_ptr<ID2D1Effect> shadow;
    ID2D1Image* input = nullptr;
};

struct Item {
    std::wstring searchableTitle,searchableText;
    float hoverAmount = 0;
    float previewHoverAmount = 0;
    float iconHoverAmount = 0;
    float tileSelectionAmount = 0;
    float pressAmount = 0;
    float gripHoverAmount = 0;
    float hoverGlowAmount = 0;
    float hoverGlowPhase = 0;
    float hoverVelocity = 0;
    float closeHoverAmount = 0;
    float pinHoverAmount = 0;
    float killHoverAmount = 0;
    float closeProgress = 0;
    float reflowOffset = 0;
    bool closeRequested = false;
    bool windowGone = false;
    double closeRequestedAt = 0;
    HWND hwnd = nullptr;
    DWORD pid = 0;
    ULONGLONG processCreation = 0;
    std::wstring title;
    std::wstring appKey;
    std::wstring appName;
    RECT rect{};
    bool minimized = false;
    bool captureAttempted=false;
    bool desktopPlaceholder=false;
    bool showDesktop=false;
    double lastFramePoll=0;
    double nextWindowValidation=0;
    uint64_t frameVersion=0;
    uint64_t snapshotVersion=~uint64_t{0};
    com_ptr<ID2D1Bitmap1> icon;
    com_ptr<ID2D1Bitmap1> placeholder;
    com_ptr<ID2D1Bitmap1> minimizedHint;
    EffectChain hintChain;

    wgc::GraphicsCaptureItem captureItem{nullptr};
    wgc::Direct3D11CaptureFramePool framePool{nullptr};
    wgc::GraphicsCaptureSession session{nullptr};
    winrt::Windows::Graphics::SizeInt32 poolSize{};
    com_ptr<ID3D11Texture2D> texture;
    com_ptr<ID2D1Bitmap1> content;

    // While an item wraps from the front to the back of the stack, it's drawn
    // twice (flying out, and fading in at the back), so it needs two chains.
    EffectChain chains[2];

    // Title for the panel styles.
    com_ptr<IDWriteTextLayout> label;
    float labelWidth = 0;

    float Width() const { return (float)(rect.right - rect.left); }
    float Height() const { return (float)(rect.bottom - rect.top); }
};

struct DrawEntry {
    Item* item;
    int chain;
    Pose pose;
    float order;  // Larger is farther away, drawn first.
    Quad quad;
    bool interactive = true;
    int group = -1;
    bool selectedGroup = false;
    float groupDistance = 0;
};

static Quad ReturnWindowQuad(const Quad& from,const Quad& to,float progress) {
    const float ease=EaseOutQuart(std::clamp(progress,0.0f,1.0f));
    Quad result{};
    for(int corner=0;corner<4;++corner) {
        result.p[corner].x=from.p[corner].x+(to.p[corner].x-from.p[corner].x)*ease;
        result.p[corner].y=from.p[corner].y+(to.p[corner].y-from.p[corner].y)*ease;
    }
    return result;
}

static bool DrawEntryBehind(const DrawEntry& a, const DrawEntry& b) {
    // Keep each application's stack together. Individual vertical depth must
    // never put a member of the selected stack behind another application.
    if (a.group != b.group) {
        if (a.selectedGroup != b.selectedGroup) return !a.selectedGroup;
        if (a.group < 0 || b.group < 0) return a.group < b.group;
        if (a.groupDistance != b.groupDistance)
            return a.groupDistance > b.groupDistance;
        return a.group < b.group;
    }
    return a.order > b.order;
}

// Windows 11 virtual desktop COM contracts. IIDs and method slots are versioned
// shell contracts, also documented by Ciantic/VirtualDesktopAccessor and the
// Windhawk Virtual Desktop Helper. No shell menus or simulated keystrokes.
class DesktopBridge {
    struct ViewCollection : IUnknown {
        virtual HRESULT STDMETHODCALLTYPE GetViews(IObjectArray**) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetViewsByZOrder(IObjectArray**) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetViewsByAppUserModelId(PCWSTR,IObjectArray**) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetViewForHwnd(HWND,IUnknown**) = 0;
    };
    struct Desktop : IUnknown {
        virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IUnknown*,BOOL*) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetId(GUID*) = 0;
    };
    template<typename Fn> static Fn Method(IUnknown* object, int slot) {
        return reinterpret_cast<Fn>((*reinterpret_cast<void***>(object))[slot]);
    }
    winrt::com_ptr<IServiceProvider> m_shell;
    winrt::com_ptr<IUnknown> m_internal;
    winrt::com_ptr<ViewCollection> m_views;
    winrt::com_ptr<IVirtualDesktopManager> m_public;
    bool m_monitorArgument = false;
    int m_managementShift=0;
    GUID m_desktopIID{};
public:
    struct Entry {
        GUID id{};
        std::wstring name;
        winrt::com_ptr<Desktop> object;
    };
    std::vector<Entry> entries;
    void Reset() {
        entries.clear(); m_views=nullptr; m_internal=nullptr;
        m_public=nullptr; m_shell=nullptr;
    }
    bool Initialize() {
        if (m_internal && m_public && m_views) return true;
        Reset();
        GUID shellID{}, managerService{}, viewsID{};
        CLSIDFromString(L"{C2F03A33-21F5-47FA-B4BB-156362A2F239}",&shellID);
        CLSIDFromString(L"{C5E0CDCA-7B6E-41B2-9FC4-D93975CC467B}",&managerService);
        CLSIDFromString(L"{1841C6D7-4F9D-42C0-AF41-8747538F10E5}",&viewsID);
        if (FAILED(CoCreateInstance(shellID,nullptr,CLSCTX_LOCAL_SERVER,
            IID_PPV_ARGS(m_shell.put())))) return false;
        struct Contract { PCWSTR manager; PCWSTR desktop; bool monitor; int managementShift; };
        const Contract contracts[] = {
            {L"{53F5CA0B-158F-4124-900C-057158060B27}",L"{3F07F4BE-B107-441A-AF0F-39D82529072C}",false,1},
            {L"{A3175F2D-239C-4BD2-8AA0-EEBA8B0B138E}",L"{3F07F4BE-B107-441A-AF0F-39D82529072C}",false,0},
            {L"{B2F925B9-5A0F-4D2E-9F4D-2B1507593C10}",L"{536D3495-B208-4CC9-AE26-DE8111275BF8}",true,0},
        };
        for (const auto& contract : contracts) {
            GUID managerID{}; CLSIDFromString(contract.manager,&managerID);
            if (SUCCEEDED(m_shell->QueryService(managerService,managerID,m_internal.put_void()))) {
                m_monitorArgument=contract.monitor;
                m_managementShift=contract.managementShift;
                CLSIDFromString(contract.desktop,&m_desktopIID);
                break;
            }
        }
        if (!m_internal || FAILED(m_shell->QueryService(viewsID,viewsID,m_views.put_void())) ||
            FAILED(CoCreateInstance(CLSID_VirtualDesktopManager,nullptr,CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(m_public.put())))) { Reset(); return false; }
        return true;
    }
    HRESULT Refresh() {
        entries.clear();
        if (!Initialize()) return E_NOINTERFACE;
        winrt::com_ptr<IObjectArray> array;
        HRESULT result;
        if (m_monitorArgument) {
            using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,HMONITOR,IObjectArray**);
            result=Method<Fn>(m_internal.get(),7)(m_internal.get(),nullptr,array.put());
        } else {
            using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,IObjectArray**);
            result=Method<Fn>(m_internal.get(),7)(m_internal.get(),array.put());
        }
        if (FAILED(result) || !array) { Reset(); return FAILED(result) ? result : E_FAIL; }
        UINT count=0; if (FAILED(array->GetCount(&count))) return E_FAIL;
        for (UINT index=0;index<count && index<128;++index) {
            Entry entry;
            if (FAILED(array->GetAt(index,m_desktopIID,entry.object.put_void())) ||
                FAILED(entry.object->GetId(&entry.id))) continue;
            wchar_t id[40]{}; StringFromGUID2(entry.id,id,ARRAYSIZE(id));
            const std::wstring key=std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops\\Desktops\\")+id;
            HSTRING nativeName=nullptr;
            using NameFn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,HSTRING*);
            if(SUCCEEDED(Method<NameFn>(entry.object.get(),5)(entry.object.get(),&nativeName)) && nativeName) {
                UINT32 length=0;const auto text=WindowsGetStringRawBuffer(nativeName,&length);entry.name.assign(text,length);
            }
            if(nativeName) WindowsDeleteString(nativeName);
            wchar_t name[256]{}; DWORD bytes=sizeof(name);
            if (entry.name.empty() && RegGetValueW(HKEY_CURRENT_USER,key.c_str(),L"Name",RRF_RT_REG_SZ,
                nullptr,name,&bytes)==ERROR_SUCCESS && name[0]) entry.name=name;
            else if(entry.name.empty()) entry.name=L"Desktop "+std::to_wstring(index+1);
            entries.push_back(std::move(entry));
        }
        return entries.empty() ? E_FAIL : S_OK;
    }
    bool ShowsInSwitcher(HWND window) {
        if(!Initialize()) return true;
        winrt::com_ptr<IUnknown> unknown,view;
        if(FAILED(m_views->GetViewForHwnd(window,unknown.put())) || !unknown) return false;
        const GUID id{0x372e1d3b,0x38d3,0x42e4,{0xa1,0x5b,0x8a,0xb2,0xb1,0x78,0xf5,0x13}};
        if(FAILED(unknown->QueryInterface(id,view.put_void()))) return false;
        // IApplicationView includes IInspectable's three methods; GetShowInSwitchers is slot 27.
        using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,INT*);
        INT show=0;
        return SUCCEEDED(Method<Fn>(view.get(),27)(view.get(),&show)) && show!=0;
    }
    HRESULT WindowDesktop(HWND window, GUID* id) {
        if (!Initialize()) return E_NOINTERFACE;
        return m_public->GetWindowDesktopId(window,id);
    }
    HRESULT Current(GUID* id) {
        if(!Initialize()) return E_NOINTERFACE;
        winrt::com_ptr<Desktop> desktop;
        HRESULT hr;
        if(m_monitorArgument) {
            using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,HMONITOR,Desktop**);
            hr=Method<Fn>(m_internal.get(),6)(m_internal.get(),nullptr,desktop.put());
        } else {
            using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,Desktop**);
            hr=Method<Fn>(m_internal.get(),6)(m_internal.get(),desktop.put());
        }
        return SUCCEEDED(hr) && desktop ? desktop->GetId(id) : hr;
    }
    HRESULT Switch(const GUID& id) {
        if(!Initialize()) return E_NOINTERFACE;
        const auto desktop=std::find_if(entries.begin(),entries.end(),[&](const Entry& entry){return IsEqualGUID(entry.id,id);});
        if(desktop==entries.end()) return E_INVALIDARG;
        HRESULT result;
        if(m_monitorArgument) {
            using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,HMONITOR,Desktop*);
            result=Method<Fn>(m_internal.get(),9)(m_internal.get(),nullptr,desktop->object.get());
        } else {
            using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,Desktop*);
            result=Method<Fn>(m_internal.get(),9)(m_internal.get(),desktop->object.get());
        }

        return result;
    }
    HRESULT Create(GUID* id) {
        if(!id) return E_POINTER;
        if(!Initialize()) return E_NOINTERFACE;
        winrt::com_ptr<Desktop> desktop;
        HRESULT hr;
        if(m_monitorArgument) {
            using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,HMONITOR,Desktop**);
            hr=Method<Fn>(m_internal.get(),10+m_managementShift)(m_internal.get(),nullptr,desktop.put());
        } else {
            using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,Desktop**);
            hr=Method<Fn>(m_internal.get(),10+m_managementShift)(m_internal.get(),desktop.put());
        }
        return SUCCEEDED(hr) && desktop ? desktop->GetId(id) : FAILED(hr) ? hr : E_FAIL;
    }
    HRESULT Remove(const GUID& id,const GUID& fallback) {
        if(entries.size()<2 || IsEqualGUID(id,entries[0].id) || IsEqualGUID(id,fallback)) return E_ACCESSDENIED;
        if(!Initialize()) return E_NOINTERFACE;
        const auto desktop=std::find_if(entries.begin(),entries.end(),[&](const Entry& e){return IsEqualGUID(e.id,id);});
        const auto target=std::find_if(entries.begin(),entries.end(),[&](const Entry& e){return IsEqualGUID(e.id,fallback);});
        if(desktop==entries.end() || target==entries.end()) return E_INVALIDARG;
        using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,Desktop*,Desktop*);
        return Method<Fn>(m_internal.get(),12+m_managementShift)(m_internal.get(),desktop->object.get(),target->object.get());
    }
    HRESULT Rename(const GUID& id,const std::wstring& name) {
        if(name.empty() || name.size()>128) return E_INVALIDARG;
        if(!Initialize()) return E_NOINTERFACE;
        const auto desktop=std::find_if(entries.begin(),entries.end(),[&](const Entry& e){return IsEqualGUID(e.id,id);});
        if(desktop==entries.end()) return E_INVALIDARG;
        auto text=winrt::hstring(name);
        using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,Desktop*,HSTRING);
        const auto hr=Method<Fn>(m_internal.get(),15+m_managementShift)(m_internal.get(),desktop->object.get(),reinterpret_cast<HSTRING>(winrt::get_abi(text)));
        if(SUCCEEDED(hr)) desktop->name=name;
        return hr;
    }
    std::wstring Wallpaper(const GUID& id) {
        const auto desktop=std::find_if(entries.begin(),entries.end(),[&](const Entry& entry){return IsEqualGUID(entry.id,id);});
        if(desktop==entries.end()) return {};
        HSTRING path=nullptr;
        using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,HSTRING*);
        const HRESULT hr=Method<Fn>(desktop->object.get(),6)(desktop->object.get(),&path);
        std::wstring result;
        if(SUCCEEDED(hr) && path) {UINT32 size=0;const auto text=WindowsGetStringRawBuffer(path,&size);result.assign(text,size);}
        if(path) WindowsDeleteString(path);
        return result;
    }
    HRESULT Move(HWND window, const GUID& destination) {
        if (!IsWindow(window)) return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);
        HRESULT result=Refresh(); if (FAILED(result)) return result;
        const auto desktop=std::find_if(entries.begin(),entries.end(),[&](const Entry& entry){return IsEqualGUID(entry.id,destination);});
        if (desktop==entries.end()) return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        GUID before{}; if (SUCCEEDED(WindowDesktop(window,&before)) && IsEqualGUID(before,destination)) return S_OK;
        WINDOWPLACEMENT placement{};
        placement.length=sizeof(placement);
        const bool havePlacement=GetWindowPlacement(window,&placement)!=FALSE;
        RECT rect{}; const bool haveRect=GetWindowRect(window,&rect)!=FALSE;
        winrt::com_ptr<IUnknown> view;
        result=m_views->GetViewForHwnd(window,view.put());
        if (FAILED(result)) return result;
        using Fn=HRESULT(STDMETHODCALLTYPE*)(IUnknown*,IUnknown*,Desktop*);
        result=Method<Fn>(m_internal.get(),4)(m_internal.get(),view.get(),desktop->object.get());
        if (FAILED(result)) return result;
        // Moving desktops normally leaves geometry intact. Restore it only if
        // the shell changes it; never activate the window or switch desktops.
        if (havePlacement && (IsIconic(window) || IsZoomed(window))) {
            WINDOWPLACEMENT after{};
            after.length=sizeof(after);
            if (GetWindowPlacement(window,&after) && !EqualRect(&placement.rcNormalPosition,&after.rcNormalPosition))
                SetWindowPlacement(window,&placement);
        } else if (haveRect) {
            RECT after{};
            if (GetWindowRect(window,&after) && !EqualRect(&rect,&after))
                SetWindowPos(window,nullptr,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
                    SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
        }
        GUID actual{};
        result=m_public->GetWindowDesktopId(window,&actual);
        return FAILED(result) ? result : (IsEqualGUID(actual,destination) ? S_OK : E_FAIL);
    }
};

// Sample the selected window's owning process off the rendering thread.
struct ProcessStats {
    DWORD pid = 0;
    double cpu = 0, gpu = 0, ramPercent = 0;
    SIZE_T ramBytes = 0;
    ULONGLONG creation = 0, sampledAt = 0;
    bool cpuKnown = false, gpuKnown = false, ramKnown = false;
};

static ULONGLONG FileTimeValue(FILETIME value) {
    return (ULONGLONG(value.dwHighDateTime) << 32) | value.dwLowDateTime;
}

class ProcessMonitor {
public:
    ~ProcessMonitor() { Stop(); }
    void Select(DWORD pid) {
        if (!m_thread && pid) {
            m_stop=CreateEventW(nullptr,TRUE,FALSE,nullptr);
            m_wake=CreateEventW(nullptr,FALSE,FALSE,nullptr);
            if (m_stop && m_wake) m_thread=CreateThread(nullptr,0,Thread,this,0,nullptr);
            if (!m_thread) { Stop(); return; }
        }
        if (m_requested.exchange(pid)!=pid && m_wake) SetEvent(m_wake);
    }
    ProcessStats Read(DWORD pid) {
        std::lock_guard<std::mutex> guard(m_mutex);
        const auto found=m_cache.find(pid);
        if (found!=m_cache.end() && GetTickCount64()-found->second.sampledAt<4000) return found->second;
        ProcessStats result; result.pid=pid; return result;
    }
    void Stop() {
        if (m_stop) SetEvent(m_stop);
        if (m_thread) { WaitForSingleObject(m_thread,INFINITE); CloseHandle(m_thread); m_thread=nullptr; }
        if (m_wake) { CloseHandle(m_wake); m_wake=nullptr; }
        if (m_stop) { CloseHandle(m_stop); m_stop=nullptr; }
        m_requested=0;
    }
private:
    static DWORD WINAPI Thread(void* data) { static_cast<ProcessMonitor*>(data)->Run(); return 0; }
    void Run() {
        PDH_HQUERY query=nullptr;
        PDH_HCOUNTER counter=nullptr;
        bool attemptedGpu=false;
        DWORD previousPid=0;
        ULONGLONG previousCpu=0, previousTime=0, previousCreation=0;
        const DWORD processors=std::max(1UL,GetActiveProcessorCount(ALL_PROCESSOR_GROUPS));
        HANDLE events[]={m_stop,m_wake};
        while (WaitForSingleObject(m_stop,0)!=WAIT_OBJECT_0) {
            const DWORD pid=m_requested.load();
            if (!pid) {
                previousPid=0;
                if (WaitForMultipleObjects(2,events,FALSE,INFINITE)==WAIT_OBJECT_0) break;
                continue;
            }
            if (!attemptedGpu) {
                attemptedGpu=true;
                if (PdhOpenQueryW(nullptr,0,&query)==ERROR_SUCCESS &&
                    PdhAddEnglishCounterW(query,L"\\GPU Engine(*)\\Utilization Percentage",0,&counter)!=ERROR_SUCCESS) {
                    PdhCloseQuery(query); query=nullptr; counter=nullptr;
                }
            }
            ProcessStats result; result.pid=pid;
            FILETIME now{}; GetSystemTimeAsFileTime(&now);
            const ULONGLONG wall=FileTimeValue(now);
            HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_VM_READ,FALSE,pid);
            if (!process) process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
            if (process) {
                FILETIME creation{},exit{},kernel{},user{};
                if (GetProcessTimes(process,&creation,&exit,&kernel,&user)) {
                    result.creation=FileTimeValue(creation);
                    const ULONGLONG cpu=FileTimeValue(kernel)+FileTimeValue(user);
                    if (pid==previousPid && result.creation==previousCreation && wall>previousTime && cpu>=previousCpu) {
                        result.cpu=std::clamp(100.0*(cpu-previousCpu)/(wall-previousTime)/processors,0.0,100.0);
                        result.cpuKnown=true;
                    }
                    previousCpu=cpu; previousTime=wall; previousCreation=result.creation; previousPid=pid;
                } else previousPid=0;
                PROCESS_MEMORY_COUNTERS memory{}; memory.cb=sizeof(memory);
                if (GetProcessMemoryInfo(process,&memory,sizeof(memory))) {
                    result.ramBytes=memory.WorkingSetSize; result.ramKnown=true;
                    MEMORYSTATUSEX total{}; total.dwLength=sizeof(total);
                    if (GlobalMemoryStatusEx(&total) && total.ullTotalPhys)
                        result.ramPercent=100.0*memory.WorkingSetSize/total.ullTotalPhys;
                }
                CloseHandle(process);
            } else previousPid=0;
            if (query && counter && PdhCollectQueryData(query)==ERROR_SUCCESS) {
                DWORD bytes=0,count=0;
                const PDH_STATUS sizeStatus=PdhGetFormattedCounterArrayW(
                    counter,PDH_FMT_DOUBLE,&bytes,&count,nullptr);
                if (sizeStatus==static_cast<PDH_STATUS>(PDH_MORE_DATA) && bytes<16*1024*1024) {
                    std::vector<BYTE> buffer(bytes);
                    auto values=reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
                    if (PdhGetFormattedCounterArrayW(counter,PDH_FMT_DOUBLE,&bytes,&count,values)==ERROR_SUCCESS) {
                        const std::wstring prefix=L"pid_"+std::to_wstring(pid)+L"_";
                        bool valid=false;
                        for (DWORD i=0;i<count;++i) {
                            const DWORD status=static_cast<DWORD>(values[i].FmtValue.CStatus);
                            if (status!=PDH_CSTATUS_VALID_DATA && status!=PDH_CSTATUS_NEW_DATA) continue;
                            if (!values[i].szName ||
                                wcsncmp(values[i].szName,prefix.c_str(),prefix.size())!=0) continue;
                            const double usage=values[i].FmtValue.doubleValue;
                            if (!std::isfinite(usage)) continue;
                            valid=true;
                            // Report the busiest engine, not a sum that can exceed 100%.
                            result.gpu=std::max(result.gpu,std::clamp(usage,0.0,100.0));
                        }
                        result.gpuKnown=valid;
                    }
                }
            }
            result.sampledAt=GetTickCount64();
            {
                std::lock_guard<std::mutex> guard(m_mutex);
                if (m_cache.size()>128) m_cache.clear();
                m_cache[pid]=result;
            }
            if (WaitForMultipleObjects(2,events,FALSE,1500)==WAIT_OBJECT_0) break;
        }
        if (query) PdhCloseQuery(query);
    }
    std::atomic<DWORD> m_requested{0};
    HANDLE m_thread=nullptr,m_stop=nullptr,m_wake=nullptr;
    std::mutex m_mutex;
    std::unordered_map<DWORD,ProcessStats> m_cache;
};

static std::wstring ProcessUptime(ULONGLONG creation, bool italian=false) {
    if (!creation) return italian ? L"Apertura: N/D" : L"Opened: N/A";
    FILETIME now{}; GetSystemTimeAsFileTime(&now);
    const ULONGLONG time=FileTimeValue(now);
    const ULONGLONG seconds=time>creation ? (time-creation)/10000000 : 0;
    if (seconds<60) return italian ? L"Aperta da pochi istanti" : L"Opened a few moments ago";
    const ULONGLONG amount=seconds<3600 ? seconds/60 : seconds<86400 ? seconds/3600 : seconds/86400;
    const wchar_t* unit=seconds<3600 ? (amount==1 ? L"minute" : L"minutes")
        : seconds<86400 ? (amount==1 ? L"hour" : L"hours") : (amount==1 ? L"day" : L"days");
    if (italian) unit=seconds<3600 ? (amount==1 ? L"minuto" : L"minuti")
        : seconds<86400 ? (amount==1 ? L"ora" : L"ore") : (amount==1 ? L"giorno" : L"giorni");
    wchar_t text[96];
    swprintf_s(text,italian ? L"Aperta %llu %ls fa" : L"Opened %llu %ls ago",amount,unit);
    return text;
}

static bool KillWindowProcess(HWND window,DWORD expectedPid,ULONGLONG expectedCreation) {
    DWORD pid=0; GetWindowThreadProcessId(window,&pid);
    if (!IsWindow(window) || pid!=expectedPid || !pid || pid==GetCurrentProcessId() || !expectedCreation) return false;
    HANDLE process=OpenProcess(PROCESS_TERMINATE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
    if (!process) return false;
    FILETIME creation{},exit{},kernel{},user{}; BOOL critical=TRUE;
    DWORD currentPid=0; GetWindowThreadProcessId(window,&currentPid);
    const bool sameProcess=currentPid==pid && GetProcessTimes(process,&creation,&exit,&kernel,&user) &&
        FileTimeValue(creation)==expectedCreation;
    const bool killed=sameProcess && IsProcessCritical(process,&critical) && !critical && TerminateProcess(process,1);
    CloseHandle(process);
    return killed;
}

struct HoverGeometry {
    D2D1_RECT_F surface{}, title{}, pin{}, close{};
    D2D1_RECT_F kill{}, uptime{};
    float uiScale = 1;
};

static bool InRect(D2D1_POINT_2F point, const D2D1_RECT_F& rect) {
    return rect.right > rect.left && rect.bottom > rect.top &&
        point.x >= rect.left && point.x <= rect.right &&
        point.y >= rect.top && point.y <= rect.bottom;
}

static bool InCloseCircle(D2D1_POINT_2F point, const D2D1_RECT_F& rect) {
    const float radius = (rect.bottom - rect.top) * 0.5f;
    if (radius <= 0) return false;
    const float nearestX=std::clamp(point.x,rect.left+radius,rect.right-radius);
    const float dx = point.x - nearestX;
    const float dy = point.y - (rect.top + rect.bottom) * 0.5f;
    return dx * dx + dy * dy <= radius * radius;
}

static Quad PreviewDragZone(const Quad& preview, float dpi) {
    const float leftHeight=std::hypot(preview.p[3].x-preview.p[0].x,
                                      preview.p[3].y-preview.p[0].y);
    const float rightHeight=std::hypot(preview.p[2].x-preview.p[1].x,
                                       preview.p[2].y-preview.p[1].y);
    const float height=std::max(1.0f,(leftHeight+rightHeight)*0.5f);
    const float fraction=std::clamp(36*dpi/height,0.08f,0.25f);
    Quad zone=preview;
    zone.p[2]={preview.p[1].x+(preview.p[2].x-preview.p[1].x)*fraction,
               preview.p[1].y+(preview.p[2].y-preview.p[1].y)*fraction};
    zone.p[3]={preview.p[0].x+(preview.p[3].x-preview.p[0].x)*fraction,
               preview.p[0].y+(preview.p[3].y-preview.p[0].y)*fraction};
    return zone;
}

// Touchpad from Modern Window Switcher — Lone.
// Adapted from the downloaded Simple Window Switcher 3.0 HID reader.
#define SWS_HID_PAGE_GENERIC        0x01
#define SWS_HID_USAGE_X             0x30
#define SWS_HID_USAGE_Y             0x31
#define SWS_HID_PAGE_DIGITIZER      0x0D
#define SWS_HID_USAGE_TOUCHPAD      0x05
#define SWS_HID_USAGE_TIP_SWITCH    0x42
#define SWS_HID_USAGE_CONTACT_ID    0x51
#define SWS_HID_USAGE_CONTACT_COUNT 0x54

struct TouchpadContact {
    ULONG id;
    LONG x;
    LONG y;
    bool tip;
};

struct TouchpadDevice {
    std::vector<BYTE> preparsed;
    std::vector<USHORT> fingerCollections;
    USHORT contactCountCollection = 0;
    double rangeX = 0.0;
    double rangeY = 0.0;
    bool hasContactCount = false;
    bool valid = false;
};

static std::unordered_map<HANDLE, TouchpadDevice> g_touchpadDevices;
static std::vector<TouchpadContact> g_touchpadFrame;
static ULONG g_touchpadFrameExpected = 0;

// Some devices declare a maximum which only fits unsigned.
static double TouchpadLogicalRange(const HIDP_VALUE_CAPS& caps) {
    LONGLONG min = caps.LogicalMin;
    LONGLONG max = caps.LogicalMax;
    if (max <= min && caps.BitSize > 0 && caps.BitSize <= 32) {
        min = 0;
        max = (1LL << caps.BitSize) - 1;
    }
    return (double)(max - min);
}

static const TouchpadDevice* TouchpadDeviceFor(HANDLE hDevice) {
    auto it = g_touchpadDevices.find(hDevice);
    if (it != g_touchpadDevices.end()) {
        return it->second.valid ? &it->second : NULL;
    }

    TouchpadDevice& dev = g_touchpadDevices[hDevice];

    UINT size = 0;
    if (GetRawInputDeviceInfoW(hDevice, RIDI_PREPARSEDDATA, NULL, &size) != 0 || !size) {
        return NULL;
    }

    dev.preparsed.resize(size);
    if (GetRawInputDeviceInfoW(hDevice, RIDI_PREPARSEDDATA, dev.preparsed.data(), &size) == (UINT)-1) {
        return NULL;
    }

    PHIDP_PREPARSED_DATA preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();
    HIDP_CAPS caps = {};
    if (HidP_GetCaps(preparsed, &caps) != HIDP_STATUS_SUCCESS) {
        return NULL;
    }

    USHORT valueCapsCount = caps.NumberInputValueCaps;
    std::vector<HIDP_VALUE_CAPS> valueCaps(valueCapsCount);
    if (!valueCapsCount ||
        HidP_GetValueCaps(HidP_Input, valueCaps.data(), &valueCapsCount, preparsed) != HIDP_STATUS_SUCCESS) {
        return NULL;
    }

    for (USHORT i = 0; i < valueCapsCount; i++) {
        const HIDP_VALUE_CAPS& vc = valueCaps[i];
        USAGE usage = vc.IsRange ? vc.Range.UsageMin : vc.NotRange.Usage;

        if (vc.UsagePage == SWS_HID_PAGE_DIGITIZER && usage == SWS_HID_USAGE_CONTACT_COUNT) {
            dev.hasContactCount = true;
            dev.contactCountCollection = vc.LinkCollection;
        } else if (vc.UsagePage == SWS_HID_PAGE_GENERIC && usage == SWS_HID_USAGE_X) {
            if (std::find(dev.fingerCollections.begin(), dev.fingerCollections.end(), vc.LinkCollection) ==
                dev.fingerCollections.end()) {
                dev.fingerCollections.push_back(vc.LinkCollection);
            }
            if (!dev.rangeX) dev.rangeX = TouchpadLogicalRange(vc);
        } else if (vc.UsagePage == SWS_HID_PAGE_GENERIC && usage == SWS_HID_USAGE_Y) {
            if (!dev.rangeY) dev.rangeY = TouchpadLogicalRange(vc);
        }
    }

    dev.valid = !dev.fingerCollections.empty() && dev.rangeX > 0 && dev.rangeY > 0;
    Wh_Log(L"SWS touchpad reader: device %p valid=%d slots=%u contactCount=%d range=%.0fx%.0f", hDevice,
           dev.valid, (UINT)dev.fingerCollections.size(), dev.hasContactCount, dev.rangeX, dev.rangeY);
    return dev.valid ? &dev : NULL;
}


static void TouchpadReaderOnReport(const TouchpadDevice& dev, PCHAR report, ULONG length,const std::function<void(ULONG,double,double)>& emit) {
    PHIDP_PREPARSED_DATA preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();

    if (dev.hasContactCount) {
        ULONG count = 0;
        if (HidP_GetUsageValue(HidP_Input, SWS_HID_PAGE_DIGITIZER, dev.contactCountCollection,
                               SWS_HID_USAGE_CONTACT_COUNT, &count, preparsed, report,
                               length) == HIDP_STATUS_SUCCESS &&
            count > 0) {
            g_touchpadFrame.clear();
            g_touchpadFrameExpected = count;
        }
        if (!g_touchpadFrameExpected) g_touchpadFrame.clear();
    }

    for (USHORT collection : dev.fingerCollections) {
        ULONG x = 0;
        ULONG y = 0;
        if (HidP_GetUsageValue(HidP_Input, SWS_HID_PAGE_GENERIC, collection, SWS_HID_USAGE_X, &x,
                               preparsed, report, length) != HIDP_STATUS_SUCCESS ||
            HidP_GetUsageValue(HidP_Input, SWS_HID_PAGE_GENERIC, collection, SWS_HID_USAGE_Y, &y,
                               preparsed, report, length) != HIDP_STATUS_SUCCESS) {
            continue;
        }

        ULONG id = collection;
        HidP_GetUsageValue(HidP_Input, SWS_HID_PAGE_DIGITIZER, collection, SWS_HID_USAGE_CONTACT_ID,
                           &id, preparsed, report, length);

        bool tip = false;
        USAGE usages[16];
        ULONG usageCount = ARRAYSIZE(usages);
        if (HidP_GetUsages(HidP_Input, SWS_HID_PAGE_DIGITIZER, collection, usages, &usageCount,
                           preparsed, report, length) == HIDP_STATUS_SUCCESS) {
            tip = std::find(usages, usages + usageCount, SWS_HID_USAGE_TIP_SWITCH) != usages + usageCount;
        }

        g_touchpadFrame.push_back({id, (LONG)x, (LONG)y, tip});
    }

    // Hybrid devices announce the contact count first and spread the contacts over
    // several reports; collect until the announced count is reached.
    if (g_touchpadFrame.empty()) return;
    if (!dev.hasContactCount || !g_touchpadFrameExpected || g_touchpadFrame.size() >= g_touchpadFrameExpected) {
        ULONG tips=0; double x=0,y=0;
        for(const auto& contact:g_touchpadFrame) if(contact.tip) {++tips;x+=contact.x;y+=contact.y;}
        emit(tips,tips ? std::clamp(x/tips/dev.rangeX,0.0,1.0) : 0,
                  tips ? std::clamp(y/tips/dev.rangeY,0.0,1.0) : 0);
        g_touchpadFrame.clear();
        g_touchpadFrameExpected = 0;
    }
}
static void ReadTouchpadRaw(HRAWINPUT input,const std::function<void(ULONG,double,double)>& emit) {
    UINT size=0;
    if(GetRawInputData(input,RID_INPUT,nullptr,&size,sizeof(RAWINPUTHEADER))!=0 || size<sizeof(RAWINPUTHEADER)) return;
    static std::vector<BYTE> buffer;
    buffer.resize(size);
    if(GetRawInputData(input,RID_INPUT,buffer.data(),&size,sizeof(RAWINPUTHEADER))==(UINT)-1) return;
    const auto* raw=reinterpret_cast<const RAWINPUT*>(buffer.data());
    if(raw->header.dwType!=RIM_TYPEHID) return;
    const auto* device=TouchpadDeviceFor(raw->header.hDevice);
    if(!device) return;
    static HANDLE currentDevice=nullptr;
    if(currentDevice!=raw->header.hDevice) {currentDevice=raw->header.hDevice;g_touchpadFrame.clear();g_touchpadFrameExpected=0;}
    const size_t offset=offsetof(RAWINPUT,data)+offsetof(RAWHID,bRawData);
    if(size<offset) return;
    const auto& hid=raw->data.hid;
    if(!hid.dwSizeHid || hid.dwCount>(size-offset)/hid.dwSizeHid) return;
    for(DWORD i=0;i<hid.dwCount;++i)
        TouchpadReaderOnReport(*device,(PCHAR)hid.bRawData+(size_t)i*hid.dwSizeHid,hid.dwSizeHid,emit);
}

enum class TouchAction {None,Open,Close,Dismiss,Next,Previous,Commit};
struct TouchStroke {
    bool active=false,moved=false,opened=false,beganOpen=false,acted=false,navigating=false;
    double x=0,y=0,navX=0,started=0;
    TouchAction Update(ULONG fingers,double px,double py,double now,bool switcherOpen) {
        if(fingers!=3) {
            if(fingers!=0) return TouchAction::None;
            const bool tap=active && beganOpen && !moved && !acted && now-started<0.28;
            *this={}; return tap ? TouchAction::Commit : TouchAction::None;
        }
        if(!active) {active=true;x=navX=px;y=py;started=now;beganOpen=switcherOpen;return TouchAction::None;}
        const double dx=px-x,dy=py-y;
        if(std::hypot(dx,dy)>0.025) moved=true;
        if(!acted && !beganOpen && y>=0.65 && dy<-0.06) {
            opened=acted=true; navX=px; y=py;
            return TouchAction::Open;
        }
        // An opening stroke stays owned even if it continues all the way up.
        if(switcherOpen && !acted && !navigating && y>=0.28 && y<0.65 && dy<-0.11 && std::fabs(dy)>std::fabs(dx)) {
            acted=true;return TouchAction::Close;
        }
        if(switcherOpen && !navigating && dy>0.13 && std::fabs(dy)>std::fabs(dx) && (!acted || opened)) {
            opened=false;acted=true;return TouchAction::Dismiss;
        }
        if(switcherOpen && (!acted || opened) && std::fabs(px-navX)>=0.075) {
            const bool next=px>navX;navigating=true;navX+=next ? 0.075 : -0.075;
            return next ? TouchAction::Next : TouchAction::Previous;
        }
        return TouchAction::None;
    }
    bool Holding(double now) const {return active && beganOpen && !moved && !acted && now-started>=0.28;}
};

// ABI used by Lone's reader integration; older Windows builds can decline it.
struct CoverTouchStatics : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE IsSupported(bool*)=0;
    virtual HRESULT STDMETHODCALLTYPE CreateForProcess(IInspectable**)=0;
};
struct CoverTouchController : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Enabled(bool*)=0;
    virtual HRESULT STDMETHODCALLTYPE put_Enabled(bool)=0;
    virtual HRESULT STDMETHODCALLTYPE get_SupportedGestures(UINT*)=0;
    virtual HRESULT STDMETHODCALLTYPE put_SupportedGestures(UINT)=0;
};
static constexpr GUID kTouchStaticsId{0x207ef171,0x1a73,0x51cd,{0xa6,0x94,0x88,0x40,0xe0,0x9d,0xba,0xfa}};

D2D1_COLOR_F StyleColor(D2D1_COLOR_F color,float alpha);

class Switcher {
   public:
    bool CreateOverlay();
    void Activate();
    void Run();
    void Shutdown();

    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

   private:
    enum class State { Idle, Pending, Open, Closing };

    void Deactivate();
    void PublishState();
    bool CreateDeviceResources();
    void ReleaseDeviceResources();
    void HandleDeviceLost();
    bool EnsureSwapChain(UINT width, UINT height);

    void ToggleDesktopOverview(bool force=false);
    void OnDesktopHotkey(bool backwards);
    void OnDesktopWinReleased();
    static BOOL CALLBACK EnumOverviewWindow(HWND hwnd, LPARAM parameter);
    static BOOL CALLBACK FindOverviewTaskbar(HWND hwnd, LPARAM parameter);
    void DrawDesktopOverview(float opacity);
    void DrawDesktopGallery(float opacity);
    void ExpandOverviewDesktop(int index);
    int HitOverviewDesktop(D2D1_POINT_2F point) const;
    void LoadOverviewWallpaper();
    void UpdateOverviewCaptures();
    void ClearOverviewCaptures();
    void UpdateLiveWallpaper();
    void ClearLiveWallpaper();
    bool DrawLiveWallpaper(const D2D1_RECT_F& destination,float opacity);
    bool RestoreOverviewWindow(HWND hwnd,DWORD pid);
    void DrawDesktopRemoval(float opacity);
    void RemoveOverviewDesktop();
    void RefreshOverviewDesktops(const GUID& selected);
    void FinishDesktopRename(bool save);
    void TickTouchpadTaps(double now);
    void ConfigureTouchpad();
    void OnTouchpad(ULONG fingers,double x,double y);
    void SetTouchpadOwnership(bool enabled);
    void OnStart(WPARAM flags);
    void OnStep(int step);
    void OnCommit(int index);
    void OnCancel();
    void OnClick(POINT pt);
    void OnIconContext(POINT pt);
    void DrawIconContext(float opacity);
    void DismissIconContext();
    void RunIconContextAction(int action);
    void OnPointerMove(POINT pt);
    void OnPointerUp();
    void ClearPointerPress();
    bool PrepareWindowDrag();
    void UpdateDragReveal();
    void RestoreDragWindow();
    bool BeginWindowDrag(POINT screenPoint);
    bool BeginOverviewWindowDrag(POINT clientPoint);
    void UpdateWindowDrag();
    void EndWindowDrag();
    void SaveHeldPreview(HWND target);

    void CollectWindows();
    void StartCaptures();
    void ContinueCaptures(int budget);
    bool FirstFramesReady();
    void StartCapture(Item& item);
    void StopCapture(Item& item);
    void PollFrames(Item& item);
    void Teardown();

    void TakeFocus();
    void ShowOverlay();
    void HideOverlay();
    bool AltReleased(double now);
    void BeginClose(bool commit);
    void ReopenDuringClose(bool backwards);
    void FinishClose();
    void EndSession();
    void ActivateWindow(HWND hwnd);

    void ComputeLayout();
    Pose FlatPose(const Item& item) const;
    Pose LayoutPose(const Item& item, int index, float rel) const;
    Quad Project(const Pose& pose, float width, float height) const;
    bool PreviewContains(const DrawEntry& entry, D2D1_POINT_2F point) const;
    void BuildDrawList(float t);

    void RenderFrame();
    void PrepareHoldPreview(double now);
    void DrawHoldPreview(float opacity);
    HWND m_holdWindow=nullptr;
    double m_holdSince=0,m_holdExitAt=0,m_holdFullAt=0;
    float m_holdAmount=0;
    bool m_holdExpanded=false;
    RECT m_holdHost{};
    D2D1_POINT_2F m_holdOffset{};
    Quad m_holdQuad{},m_holdExitQuad{};
    com_ptr<ID2D1Bitmap1> m_holdExitBitmap;
    com_ptr<ID2D1Effect> m_holdTransform;
    void DrawCard(const DrawEntry& entry, float t);
    void DrawReflection(const DrawEntry& entry, float t);
    HoverGeometry GetHoverGeometry(const DrawEntry& entry) const;
    void DrawHoverControls(float t);
    void DrawHoverTitle(float t);
    void UpdateDesktopInfo(double now);
    void DrawDesktopPicker(float t);
    D2D1_POINT_2F LayoutPoint(POINT point) const {
        auto inverse=m_popupTransform;
        if(!inverse.Invert()) return {(float)point.x,(float)point.y};
        return inverse.TransformPoint({(float)point.x,(float)point.y});
    }
    void DrawSimpleStyle(float t);
    void DrawTitle(float t);
    void DrawSelectionBar(float t);
    CoverTabbyPosition::Motion m_positionMotion;
    com_ptr<IDWriteTextFormat> m_positionFormat;
    float m_positionFontSize=0;
    int m_positionTextCount=-1;
    std::wstring m_positionLabel;
    void DrawTileSelection(float t);
    void DrawCompactIcons(float t);
    void DrawProcessStats(float t);
    void UpdateAppGroups(bool reset=false);
    void UpdateSearch();
    void DrawSearch(float t);
    void DrawSearchStyle(float t);
    void DrawWindowCount(float opacity,D2D1_RECT_F rect,int found,int total,bool forceDark=false);
    std::vector<D2D1_RECT_F> m_simplePinRects;
    std::vector<float> m_enchantedHover;
    float m_resultPanelWidth=0,m_resultPanelHeight=0;
    D2D1_RECT_F m_enchantedFrom{},m_enchantedTo{};
    int m_enchantedIndex=-1,m_enchantedPosition=-1,m_enchantedCount=0;
    double m_enchantedBirth=0;
    bool m_enchantedWrap=false;
    com_ptr<IDWriteTextFormat> m_enchantedSymbols;
    com_ptr<IDWriteTextFormat> m_countFormat;
    com_ptr<IDWriteTextLayout> m_countSuffixLayout;
    std::wstring m_countSuffixText;
    float m_countSuffixHeight=0,m_countSuffixWidth=0;
    int m_countFound=-1;
    std::vector<double> m_countFrom,m_countTo;
    double m_countBirth=0;
    struct UiTextAnimation {
        struct Glyph {UINT32 start=0,length=0;float left=0,width=0;double born=0;com_ptr<ID2D1CommandList> image;};
        std::wstring text;com_ptr<IDWriteTextFormat> format;com_ptr<IDWriteTextLayout> layout;
        std::vector<Glyph> glyphs;float width=0,height=0;bool dark=false;double changed=0;
    };
    UiTextAnimation m_searchTextAnimation,m_renameTextAnimation,m_holdMessageTitle,m_holdMessageBody;
    com_ptr<IDWriteTextFormat> m_holdMessageFormat;
    com_ptr<ID2D1Effect> m_uiTextBlur;
    struct UiPanelMask {float width,height,radius;com_ptr<ID2D1RoundedRectangleGeometry> geometry;};
    std::vector<UiPanelMask> m_uiPanelMasks;
    D2D1_COLOR_F UiForeground(float opacity) const;
    D2D1_COLOR_F UiSurface(float opacity,float defaultTint=.92f) const;
    com_ptr<ID2D1Bitmap1> m_showDesktopWallpaper;
    std::wstring m_showDesktopWallpaperKey;
    com_ptr<ID2D1Bitmap1> LoadWallpaperPreview(std::wstring path);
    void DrawUiPanel(D2D1_RECT_F rect,float radius,float opacity,float defaultTint=.92f,bool backdrop=true);
    void DrawAnimatedUiText(UiTextAnimation& state,const std::wstring& text,IDWriteTextFormat* format,
        D2D1_RECT_F rect,float opacity,bool selected=false,bool caret=false,bool placeholder=false);
    bool EnsureChain(EffectChain& chain);

    com_ptr<ID2D1Bitmap1> GetIconBitmap(HWND hwnd);
    void RefreshIcons();
    com_ptr<ID2D1Bitmap1> CreateIconBitmap(HICON icon);
    com_ptr<ID2D1Bitmap1> CreateAppIconBitmap(HWND hwnd);
    com_ptr<ID2D1Bitmap1> CreatePlaceholder(const Item& item, bool hintLayer=false);
    void SaveWindowSnapshot(Item& item);
    com_ptr<ID2D1Bitmap1> CreateTargetBitmap(UINT width, UINT height);
    void UpdateBackground();
    void CaptureAcrylicBackdrop();

    int SelectedIndex() const;
    const wchar_t* Text(const wchar_t* english,const wchar_t* italian) const {
        return m_settings.italian ? italian : english;
    }

    Settings m_settings{};
    ProcessMonitor m_processMonitor;
    ProcessStats m_processStats;
    com_ptr<IDWriteTextFormat> m_statsFormat;
    float m_statsFontSize = 0, m_statsBottom = 0;
    double m_killErrorUntil = 0;
    HWND m_killConfirmWindow=nullptr;
    DWORD m_killConfirmPid=0;
    ULONGLONG m_killConfirmCreation=0;
    double m_killConfirmUntil=0;
    bool ConfirmKill(const Item& item) {
        const double now=NowSeconds();
        if(m_killConfirmWindow==item.hwnd && m_killConfirmPid==item.pid &&
            m_killConfirmCreation==item.processCreation && now<m_killConfirmUntil) {
            m_killConfirmWindow=nullptr;m_killConfirmUntil=0;return true;
        }
        m_killConfirmWindow=item.hwnd;m_killConfirmPid=item.pid;
        m_killConfirmCreation=item.processCreation;m_killConfirmUntil=now+4;
        return false;
    }
    HWND m_hwnd = nullptr;
    TouchStroke m_touchStroke;
    int m_touchTapCount=0;
    double m_touchTapDeadline=0;
    bool m_desktopOverview=false,m_openingOverview=false,m_commitDesktop=false;
    GUID m_destinationDesktop{};
    int m_overviewSelected=0;
    HWND m_overviewSelectedWindow=nullptr;
    DWORD m_overviewSelectedPid=0;
    GUID m_overviewCurrentDesktop{};
    GUID m_overviewPreviousDesktop{};
    double m_overviewDesktopTransition=0;
    int m_overviewTransitionDirection=1;
    std::unique_ptr<Item> m_overviewTaskbar;
    std::unique_ptr<Item> m_liveWallpaper;
    double m_liveWallpaperCheckAt=0;
    bool m_overviewExpanded=false;
    GUID m_overviewDeleteId{};
    bool m_overviewDeletePending=false;
    D2D1_RECT_F m_overviewDeleteAccept{},m_overviewDeleteCancel{};
    HWND m_overviewPress=nullptr;
    DWORD m_overviewPressPid=0;
    POINT m_overviewPressPoint{};
    D2D1_POINT_2F m_overviewDragPoint{};
    bool m_overviewDragging=false;
    bool m_overviewPressFromDock=false;
    double m_overviewOutsideSince=0;
    double m_overviewScroll=0,m_overviewTransition=0,m_overviewRefreshAt=0;
    std::wstring m_overviewTooltip;
    D2D1_POINT_2F m_overviewTooltipPoint{};
    float m_overviewTooltipOpacity=0;
    RECT m_overviewWorld{};
    RECT m_overviewSavedMonitor{};
    float m_overviewSavedScale=1;
    com_ptr<ID2D1Bitmap1> m_overviewSavedBackground;
    std::wstring m_overviewSavedBackgroundKey;
    struct OverviewDesktop {
        GUID id{};
        std::wstring name;
        com_ptr<ID2D1Bitmap1> wallpaper;
        com_ptr<ID2D1Bitmap1> surface;
        com_ptr<ID2D1Effect> projection;
        Quad quad{};
        bool tried = false;
        D2D1_RECT_F hit{}, rename{}, remove{};
        float hoverAmount = 0;
    };
    D2D1_RECT_F m_overviewAdd{},m_overviewRenameBox{},m_overviewBack{};
    GUID m_overviewRenameId{};
    std::wstring m_overviewRenameText;
    bool m_overviewRenameSelectAll=false;
    double m_overviewActionErrorUntil=0;
    struct OverviewWindow {
        HWND hwnd;
        GUID desktop;
        RECT rect;
        std::wstring title;
        com_ptr<ID2D1Bitmap1> icon;
        DWORD pid;
        std::unique_ptr<Item> capture;
        D2D1_RECT_F hit{};
        int hitDesktop = -1;
        D2D1_RECT_F dockHit{};
        float dockHover = 0, dockFocus = 0, dockAmount = 0;
        float overviewHover = 0;
        D2D1_RECT_F dockVisual{};
        bool dockTarget = false, alive = true, minimized = false;
        bool restoredInOverview=false;
        double validateAt = 0;
    };
    std::vector<OverviewDesktop> m_overviewDesktops;
    std::vector<int> m_overviewPaintOrder;
    std::vector<D2D1_RECT_F> m_overviewDropTargets;
    std::vector<OverviewWindow> m_overviewWindows;
    com_ptr<IDWriteTextFormat> m_overviewFormat;
    com_ptr<IDWriteTextFormat> m_overviewTitleFormat;
    com_ptr<IDWriteTextFormat> m_overviewButtonFormat;
    com_ptr<IDWriteTextFormat> m_overviewSymbolFormat;
    bool m_touchSession=false,m_touchControllerTried=false;
    com_ptr<CoverTouchController> m_touchController;
    HPOWERNOTIFY m_energyNotification=nullptr, m_batteryNotification=nullptr;
    std::vector<HWND> m_captureQueue;
    size_t m_captureNext=0, m_snapshotNext=0;
    double m_snapshotAt=0;

    // Device resources.
    com_ptr<ID3D11Device> m_d3dDevice;
    com_ptr<ID3D11DeviceContext> m_d3dContext;
    com_ptr<IDXGIDevice> m_dxgiDevice;
    wgd3d::IDirect3DDevice m_captureDevice{nullptr};
    com_ptr<ID2D1Factory1> m_d2dFactory;
    com_ptr<ID2D1Device> m_d2dDevice;
    com_ptr<ID2D1DeviceContext> m_ctx;
    com_ptr<ID2D1DeviceContext> m_overviewDrawContext;
    com_ptr<IDCompositionDevice> m_dcompDevice;
    com_ptr<IDCompositionTarget> m_dcompTarget;
    com_ptr<IDCompositionVisual> m_dcompVisual;
    com_ptr<IDXGISwapChain1> m_swapChain;
    HANDLE m_frameLatency=nullptr;
    com_ptr<ID2D1Bitmap1> m_targetBitmap;
    com_ptr<ID2D1Effect> m_popupShadow;
    com_ptr<ID2D1RoundedRectangleGeometry> m_windowMaskGeometry;
    D2D1::Matrix3x2F m_popupTransform=D2D1::Matrix3x2F::Identity();
    float m_windowScale=1;
    // Normal selection returns the captured preview to its desktop rectangle.
    // The host expands temporarily so Windowed mode cannot clip that journey.
    bool m_returnActive=false;
    DrawEntry m_returnEntry{};
    Quad m_returnFrom{}, m_returnTo{};
    D2D1_POINT_2F m_closeSceneOffset{};
    com_ptr<ID2D1SolidColorBrush> m_brush;
    com_ptr<IDWriteFactory> m_dwriteFactory;
    com_ptr<IDWriteTextFormat> m_titleFormat;
    com_ptr<IWICImagingFactory> m_wicFactory;
    UINT m_swapWidth = 0;
    UINT m_swapHeight = 0;
    bool m_borderlessRequested = false;

    // Window icons, kept across sessions (see GetIconBitmap). The bitmap is
    // null for windows without an icon.
    struct CachedIcon {
        HICON source = nullptr;  // The window icon it was made from, if any.
        com_ptr<ID2D1Bitmap1> bitmap;
    };
    std::unordered_map<HWND, CachedIcon> m_iconCache;
    struct WindowSnapshot {
        DWORD pid;
        ULONGLONG creation;
        std::wstring title;
        com_ptr<ID2D1Bitmap1> bitmap;
        double used;
    };
    std::unordered_map<HWND,WindowSnapshot> m_windowSnapshots;

    // Background.
    com_ptr<ID2D1Bitmap1> m_background;
    std::wstring m_backgroundKey;

    // Session.
    State m_state = State::Idle;
    D2D1_RECT_F m_tileBorder{}, m_tileBorderFrom{}, m_tileBorderTo{};
    double m_tileBorderAt = 0;
    int m_tileBorderIndex = -1;
    bool m_tileBorderWrap = false;
    float m_tileCellWidth=0, m_tileCellHeight=0, m_tileGridTop=0;
    float m_tileIconY=0, m_tileTitleY=0;
    com_ptr<ID2D1LinearGradientBrush> m_tileFadeBrush;
    std::vector<std::unique_ptr<Item>> m_items;
    std::unique_ptr<Item> m_heldCapture;
    std::vector<DrawEntry> m_drawList;
    RECT m_monitor{};
    // Monitor DPI / 96, for text sizes.
    float m_dpiScale = 1;
    double m_showAt = 0;
    // Captures start once the show delay has passed. The overlay is shown
    // when their first frames arrived, or at this time at the latest.
    bool m_capturing = false;
    double m_captureDeadline = 0;
    double m_lastFrame = 0;
    double m_motionDt = 1.0 / 60.0;
    double m_altReleasedAt = 0;
    Tween m_open;
    bool m_commit = false;
    bool m_selectedOnTopWhenFlat = false;
    // The overlay took the keyboard focus (sessions started from the hotkey).
    bool m_hasFocus = false;
    HWND m_previousForeground = nullptr;
    HWND m_pressedWindow = nullptr;
    bool m_pressCanDrag = false;
    POINT m_pressScreen{};
    bool m_pressWasSticky = false;
    HWND m_dragWindow = nullptr;
    bool m_dragWasTopmost = false;
    bool m_dragMoving = false;
    bool m_dragDropped = false;
    WINDOWPLACEMENT m_dragOriginalPlacement{};
    RECT m_dragOriginalRect{}, m_dragVisibleRect{}, m_dragFrameMargins{};
    double m_dragRevealStart = 0;
    bool m_dragRevealDone = false;
    POINT m_dragMouseOffset{};
    POINT m_dragLastPosition{};
    DWORD m_dragPid = 0;
    GUID m_dragOriginalDesktop{};
    bool m_dragChangedDesktop=false;
    long long m_target = 0;  // Index of the front item, not wrapped.
    double m_scroll = 0;     // Animated towards m_target.
    DesktopBridge m_desktopBridge;
    HWND m_desktopInfoWindow = nullptr, m_desktopMenuWindow = nullptr;
    GUID m_windowDesktopId{};
    bool m_desktopKnown = false, m_desktopMenuOpen = false;
    double m_desktopRefreshAt = 0, m_desktopErrorUntil = 0;
    float m_desktopMenuAmount = 0;
    int m_desktopScroll = 0;
    D2D1_RECT_F m_desktopButton{}, m_desktopMenuRect{};
    std::vector<std::pair<GUID, D2D1_RECT_F>> m_desktopRows;
    com_ptr<IDWriteTextFormat> m_desktopTextFormat;
    float m_desktopFontSize = 0;
    HWND m_hoverWindow = nullptr;
    HWND m_hoverCandidate = nullptr;
    double m_hoverCandidateSince = 0;
    POINT m_lastHoverCursor{-10000,-10000};
    bool m_hoverSelectionArmed = true;
    D2D1_RECT_F m_hoverBar{};
    D2D1_RECT_F m_closeButton{};
    D2D1_RECT_F m_killButton{};
    D2D1_RECT_F m_pinButton{};
    com_ptr<IDWriteTextFormat> m_hoverTextFormat;
    float m_hoverFontSize = 0;
    com_ptr<IDWriteTextFormat> m_controlSymbolFormat;
    std::unordered_map<HWND, bool> m_pinned;
    std::vector<std::vector<int>> m_appGroups;
    std::vector<int> m_groupOf, m_groupRank;
    std::vector<float> m_groupFocus;
    std::unordered_map<std::wstring, float> m_animatedGroupFocus;
    double m_groupFocusTime = 0;
    std::unordered_map<HWND, Pose> m_searchFrom;
    double m_searchTransitionStart=0;
    float m_contentOffset=0;
    float m_searchBarWidth=0;
    double m_searchBarTime=0;
    bool m_searchActive=false;
    bool m_searchSelectAll=false;
    std::wstring m_searchQuery;
    std::vector<int> m_searchMatches;
    std::vector<uint8_t> m_searchMask;
    std::wstring m_searchMeasuredText;
    float m_searchMeasuredWidth=0;
    D2D1_RECT_F m_searchRect{};
    com_ptr<IDWriteTextFormat> m_searchFormat;
    com_ptr<IDWriteTextFormat> m_hoverTitleFormat;
    com_ptr<IDWriteTextLayout> m_hoverTitleLayout;
    std::wstring m_cursorTitle;
    D2D1_POINT_2F m_cursorTitlePoint{};
    float m_cursorTitleOpacity = 0;
    double m_groupScroll = 0;
    double m_iconScroll = 0; // Faster independent icon strip animation.
    double m_velocity = 0;
    int m_wheelRemainder = 0;
    int m_titleIndex = -1;
    com_ptr<IDWriteTextLayout> m_titleLayout;

    // Layout, in pixels.
    float m_width = 0, m_height = 0;
    float m_parallaxX = 0, m_parallaxY = 0;
    float m_cursorTiltX=0,m_cursorTiltY=0;
    com_ptr<ID2D1RoundedRectangleGeometry> m_overviewCardMask;
    float m_overviewMaskWidth=0,m_overviewMaskHeight=0,m_overviewMaskDpi=0;
    float m_overviewLayoutWidth=0,m_overviewLayoutHeight=0,m_overviewLayoutX=0,m_overviewLayoutY=0;
    float m_focal = 0;
    float m_boxWidth = 0, m_boxHeight = 0;
    float m_frontX = 0, m_frontY = 0;
    float m_stepX = 0, m_stepY = 0, m_stepZ = 0;

    // Panel styles: each window's cell and the parts inside it, the panel
    // around them, and the classic style's title box.
    std::vector<D2D1_RECT_F> m_iconRects;
    float m_simpleIndicator=-1;
    int m_simpleShortcutNumber=0;
    ULONGLONG m_simpleShortcutTime=0;
    std::vector<D2D1_RECT_F> m_simpleCloseRects;
    std::vector<float> m_simpleDockHover;
    HWND m_simpleRaised=nullptr;
    com_ptr<IDWriteTextFormat> m_simpleFormat;
    com_ptr<ID2D1Effect> m_simpleGray;
    com_ptr<ID2D1Bitmap1> m_simpleContrastSource;
    std::vector<D2D1_COLOR_F> m_simpleContrastSamples;
    bool m_iconMenuOpen=false;
    HWND m_iconMenuWindow=nullptr;
    DWORD m_iconMenuPid=0;
    ULONGLONG m_iconMenuCreation=0;
    std::wstring m_iconMenuTitle;
    float m_iconMenuAmount=0,m_iconMenuDpi=0;
    float m_iconMenuHover[3]{};
    D2D1_RECT_F m_iconMenuRect{},m_iconMenuRows[3]{};
    com_ptr<IDWriteTextFormat> m_iconMenuText,m_iconMenuSymbols;
};

Switcher* g_switcher;

LRESULT CALLBACK OverlayWndProc(HWND hwnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    if (g_switcher) {
        return g_switcher->HandleMessage(hwnd, msg, wParam, lParam);
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Creates the overlay window only. It's quick, so the mod finishes loading
// right away; the graphics devices are created afterwards, in Activate.
bool Switcher::CreateOverlay() {
    m_settings = LoadSettings();
    g_takeOver = true;
    g_searchEnabled = m_settings.compactCoverFlow && m_settings.enableSearch;
    g_desktopOverviewHotkey=m_settings.desktopOverviewHotkey;
    g_desktopOverviewHoldWin=m_settings.desktopOverviewHoldWin;
    g_ignoreRepeatedNavigationKeys=m_settings.ignoreRepeatedNavigationKeys;

    WNDCLASSEXW wc{};
    wc.cbSize=sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = g_module;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kOverlayClassName;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassEx failed: %u", GetLastError());
        return false;
    }

    m_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                                 WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP,
                             kOverlayClassName, kOverlayTitleIdle, WS_POPUP, 0,
                             0, 0, 0, nullptr, nullptr, g_module, nullptr);
    if (!m_hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return false;
    }
    g_overlayWnd = m_hwnd;
    ConfigureTouchpad();
    m_energyNotification=RegisterPowerSettingNotification(m_hwnd,&kEnergySaverGuid,DEVICE_NOTIFY_WINDOW_HANDLE);
    m_batteryNotification=RegisterPowerSettingNotification(m_hwnd,&kBatterySaverGuid,DEVICE_NOTIFY_WINDOW_HANDLE);

    BOOL disableTransitions = TRUE;
    DwmSetWindowAttribute(m_hwnd, DWMWA_TRANSITIONS_FORCEDISABLED,
                          &disableTransitions, sizeof(disableTransitions));
    DWM_WINDOW_CORNER_PREFERENCE corners = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(m_hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corners,
                          sizeof(corners));
    return true;
}

// Gets ready to take over Alt+Tab. With the native style, nothing is created.
void Switcher::Activate() {
    if (!g_takeOver || m_d3dDevice) {
        PublishState();
        return;
    }

    if (!CreateDeviceResources()) {
        // Alt+Tab isn't taken over, so the native switcher keeps working.
        ReleaseDeviceResources();
        PublishState();
        return;
    }

    if (!m_borderlessRequested) {
        m_borderlessRequested = true;
        // Unpackaged processes are granted borderless capture without a
        // prompt. Without it, Windows draws a yellow border around captured
        // windows.
        try {
            const auto status =
                wgc::GraphicsCaptureAccess::RequestAccessAsync(
                    wgc::GraphicsCaptureAccessKind::Borderless)
                    .get();
            if (status != winrt::Windows::Security::Authorization::
                              AppCapabilityAccess::AppCapabilityAccessStatus::
                                  Allowed) {
                Wh_Log(L"Borderless capture not allowed: %d", (int)status);
            }
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"RequestAccessAsync failed: 0x%08X", (UINT)e.code());
        }
    }

    g_ready = true;
    PublishState();
}

// Switched to the native style: let go of everything.
void Switcher::Deactivate() {
    EndWindowDrag();
    EndSwitching();
    HideOverlay();
    Teardown();
    m_state = State::Idle;
    ReleaseDeviceResources();
    PublishState();
}

// Tells explorer, through the overlay's title, whether to hand the Alt+Tab
// hotkey over (see ForwardHotkey).
void Switcher::PublishState() {
    if (m_hwnd) {
        SetWindowTextW(m_hwnd, g_ready && g_takeOver ? kOverlayTitleReady
                                                     : kOverlayTitleIdle);
    }
}

bool Switcher::CreateDeviceResources() {
    HRESULT hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
        m_d3dDevice.put(), nullptr, m_d3dContext.put());
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: 0x%08X", (UINT)hr);
        return false;
    }

    // Capture frame pools use the device from their own threads.
    if (auto multithread = m_d3dDevice.try_as<ID3D11Multithread>()) {
        multithread->SetMultithreadProtected(TRUE);
    }

    m_dxgiDevice = m_d3dDevice.as<IDXGIDevice>();

    auto createCaptureDevice =
        (CreateDirect3D11DeviceFromDXGIDevice_t)GetProcAddress(
            GetModuleHandleW(L"d3d11.dll"),
            "CreateDirect3D11DeviceFromDXGIDevice");
    if (!createCaptureDevice) {
        Wh_Log(L"CreateDirect3D11DeviceFromDXGIDevice not found");
        return false;
    }
    com_ptr<::IInspectable> inspectable;
    hr = createCaptureDevice(m_dxgiDevice.get(), inspectable.put());
    if (FAILED(hr)) {
        Wh_Log(L"CreateDirect3D11DeviceFromDXGIDevice failed: 0x%08X",
               (UINT)hr);
        return false;
    }
    hr = inspectable->QueryInterface(winrt::guid_of<wgd3d::IDirect3DDevice>(),
                                     winrt::put_abi(m_captureDevice));
    if (FAILED(hr)) {
        return false;
    }

    D2D1_FACTORY_OPTIONS options{};
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,
                           __uuidof(ID2D1Factory1), &options,
                           m_d2dFactory.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"D2D1CreateFactory failed: 0x%08X", (UINT)hr);
        return false;
    }
    hr = m_d2dFactory->CreateDevice(m_dxgiDevice.get(), m_d2dDevice.put());
    if (FAILED(hr)) {
        return false;
    }
    hr = m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                          m_ctx.put());
    if (FAILED(hr)) {
        return false;
    }
    // Everything is in physical pixels.
    m_ctx->SetDpi(96, 96);
    m_ctx->SetUnitMode(D2D1_UNIT_MODE_PIXELS);
    hr = m_ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White),
                                      m_brush.put());
    if (FAILED(hr)) {
        return false;
    }

    hr = DCompositionCreateDevice(m_dxgiDevice.get(),
                                  IID_PPV_ARGS(m_dcompDevice.put()));
    if (FAILED(hr)) {
        Wh_Log(L"DCompositionCreateDevice failed: 0x%08X", (UINT)hr);
        return false;
    }
    hr = m_dcompDevice->CreateTargetForHwnd(m_hwnd, TRUE, m_dcompTarget.put());
    if (FAILED(hr)) {
        return false;
    }
    hr = m_dcompDevice->CreateVisual(m_dcompVisual.put());
    if (FAILED(hr)) {
        return false;
    }
    m_dcompTarget->SetRoot(m_dcompVisual.get());

    if (!m_dwriteFactory) {
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(m_dwriteFactory.put()));
        if (FAILED(hr)) {
            return false;
        }
    }
    if (!m_wicFactory) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                              CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(m_wicFactory.put()));
        if (FAILED(hr)) {
            return false;
        }
    }
    return true;
}

void Switcher::ReleaseDeviceResources() {
    ClearLiveWallpaper();
    m_overviewDrawContext=nullptr;
    m_searchTextAnimation = {};
    m_renameTextAnimation = {};
    m_uiTextBlur = nullptr;
    m_uiPanelMasks.clear();
    m_holdMessageFormat = nullptr;
    m_holdMessageTitle = {};
    m_holdMessageBody = {};
    m_holdTransform = nullptr;
    m_holdExitBitmap = nullptr;
    m_showDesktopWallpaper=nullptr;m_showDesktopWallpaperKey.clear();
    m_simpleGray=nullptr;m_simpleContrastSource=nullptr;m_simpleContrastSamples.clear();
    g_ready = false;
    m_titleLayout = nullptr;
    m_tileFadeBrush=nullptr;
    m_holdWindow=nullptr;m_holdSince=0;m_holdAmount=0;m_holdFullAt=0;m_holdExpanded=false;m_holdOffset={};m_holdExitBitmap=nullptr;
    m_tileBorderIndex=-1;
    m_titleFormat = nullptr;
    m_titleIndex = -1;
    m_background = nullptr;
    m_backgroundKey.clear();
    // The bitmaps belong to the device.
    m_iconCache.clear();
    m_windowSnapshots.clear();
    m_renameTextAnimation={};
    m_overviewFormat=nullptr;m_overviewTitleFormat=nullptr;
    m_overviewButtonFormat=nullptr;m_overviewSymbolFormat=nullptr;
    if (m_ctx) {
        m_ctx->SetTarget(nullptr);
    }
    m_targetBitmap = nullptr;
    if(m_frameLatency) {CloseHandle(m_frameLatency);m_frameLatency=nullptr;}
    m_swapChain = nullptr;
    m_windowMaskGeometry=nullptr;
    m_iconMenuText=nullptr;m_iconMenuSymbols=nullptr;
    m_overviewCardMask=nullptr;
    m_swapWidth = m_swapHeight = 0;
    m_dcompVisual = nullptr;
    m_dcompTarget = nullptr;
    m_dcompDevice = nullptr;
    m_brush = nullptr;
    m_ctx = nullptr;
    m_d2dDevice = nullptr;
    m_d2dFactory = nullptr;
    m_captureDevice = nullptr;
    m_dxgiDevice = nullptr;
    m_d3dContext = nullptr;
    m_d3dDevice = nullptr;
}

void Switcher::HandleDeviceLost() {
    EndWindowDrag();
    Wh_Log(L"Graphics device lost, recreating");
    // The Alt release will reach the foreground app normally.
    EndSwitching();
    HideOverlay();
    Teardown();
    m_state = State::Idle;
    ReleaseDeviceResources();
    if (CreateDeviceResources()) {
        g_ready = true;
    } else {
        ReleaseDeviceResources();
    }
    PublishState();
}

bool Switcher::EnsureSwapChain(UINT width, UINT height) {
    if (m_swapChain && m_swapWidth == width && m_swapHeight == height) {
        return true;
    }

    m_ctx->SetTarget(nullptr);
    m_targetBitmap = nullptr;
    if(m_frameLatency) {CloseHandle(m_frameLatency);m_frameLatency=nullptr;}
    m_swapChain = nullptr;
    m_windowMaskGeometry=nullptr;
    m_swapWidth = m_swapHeight = 0;

    com_ptr<IDXGIAdapter> adapter;
    com_ptr<IDXGIFactory2> factory;
    if (FAILED(m_dxgiDevice->GetAdapter(adapter.put())) ||
        FAILED(adapter->GetParent(IID_PPV_ARGS(factory.put())))) {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    desc.Flags=DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    HRESULT hr = factory->CreateSwapChainForComposition(
        m_d3dDevice.get(), &desc, nullptr, m_swapChain.put());
    if(FAILED(hr)) {
        desc.Flags=0;
        hr=factory->CreateSwapChainForComposition(m_d3dDevice.get(),&desc,nullptr,m_swapChain.put());
    }
    if (FAILED(hr)) {
        Wh_Log(L"CreateSwapChainForComposition failed: 0x%08X", (UINT)hr);
        return false;
    }

    com_ptr<IDXGISwapChain2> latencyChain;
    if((desc.Flags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT) &&
        SUCCEEDED(m_swapChain->QueryInterface(IID_PPV_ARGS(latencyChain.put()))) &&
        SUCCEEDED(latencyChain->SetMaximumFrameLatency(1)))
        m_frameLatency=latencyChain->GetFrameLatencyWaitableObject();

    com_ptr<IDXGISurface> surface;
    hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(surface.put()));
    if (SUCCEEDED(hr)) {
        D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED));
        hr = m_ctx->CreateBitmapFromDxgiSurface(surface.get(), &props,
                                                m_targetBitmap.put());
    }
    if (SUCCEEDED(hr)) {
        hr = m_dcompVisual->SetContent(m_swapChain.get());
    }
    if (SUCCEEDED(hr)) {
        hr = m_dcompDevice->Commit();
    }
    if (FAILED(hr)) {
        Wh_Log(L"Swap chain setup failed: 0x%08X", (UINT)hr);
        m_targetBitmap = nullptr;
        m_swapChain = nullptr;
        return false;
    }

    m_swapWidth = width;
    m_swapHeight = height;
    return true;
}

void Switcher::Shutdown() {
    EndWindowDrag();
    m_processMonitor.Stop();
    RAWINPUTDEVICE remove{0x0D,0x05,RIDEV_REMOVE,nullptr};
    RegisterRawInputDevices(&remove,1,sizeof(remove));
    SetTouchpadOwnership(false); m_touchController=nullptr;
    g_touchpadDevices.clear(); g_touchpadFrame.clear(); g_touchpadFrameExpected=0;
    g_ready = false;
    EndSwitching();
    if (m_hwnd) {
        HideOverlay();
    }
    Teardown();
    ReleaseDeviceResources();
    m_dwriteFactory = nullptr;
    m_wicFactory = nullptr;
    if (m_energyNotification) { UnregisterPowerSettingNotification(m_energyNotification); m_energyNotification=nullptr; }
    if (m_batteryNotification) { UnregisterPowerSettingNotification(m_batteryNotification); m_batteryNotification=nullptr; }
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    UnregisterClassW(kOverlayClassName, g_module);
}

void Switcher::Run() {
    MSG msg;
    for (;;) {
        bool frameReady=true;
        if((m_state==State::Open || m_state==State::Closing) && m_frameLatency) {
            const DWORD wait=MsgWaitForMultipleObjectsEx(1,&m_frameLatency,INFINITE,QS_ALLINPUT,MWMO_INPUTAVAILABLE);
            frameReady=wait==WAIT_OBJECT_0;
            if(wait==WAIT_FAILED) {Wh_Log(L"Frame-latency wait failed: %u",GetLastError());CloseHandle(m_frameLatency);m_frameLatency=nullptr;frameReady=true;}
        }
        if (m_state == State::Idle || m_state == State::Pending) {
            DWORD timeout = m_dragMoving ? 8 : INFINITE;
            if (m_state == State::Pending) {
                // Wake up regularly to watch the Alt key (see AltReleased),
                // and often while waiting for the first frames.
                const double remaining = m_showAt - NowSeconds();
                timeout = m_capturing
                              ? 4
                              : (DWORD)std::clamp(std::ceil(remaining * 1000),
                                                  0.0, 15.0);
            }
            MsgWaitForMultipleObjectsEx(0, nullptr, timeout, QS_ALLINPUT,
                                        MWMO_INPUTAVAILABLE);
        }

        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        // Dragging outlives the overlay fade. Continue watching the physical
        // button and screen cursor even after the overlay is hidden/Idle.
        UpdateWindowDrag();
        if(!frameReady && m_frameLatency && (m_state==State::Open || m_state==State::Closing)) continue;

        if (m_state == State::Pending) {
            const double now = NowSeconds();
            if (AltReleased(now) && EndSwitching()) {
                OnCommit(-1);
            } else if (now >= m_showAt) {
                // A quick Alt+Tab never gets here, so it doesn't pay for
                // captures it wouldn't show.
                if (!m_capturing) {
                    StartCaptures();
                    m_captureDeadline = now + 0.045;
                }
                if (m_settings.simpleStyle || FirstFramesReady() || now >= m_captureDeadline) {
                    ShowOverlay();
                }
            }
        }

        if (m_state == State::Open || m_state == State::Closing) {
            // Blocks until the next vertical blank.
            RenderFrame();
        }
    }
}

LRESULT Switcher::HandleMessage(HWND hwnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (msg) {
        case WM_APP_DESKTOP_OVERVIEW:
            OnDesktopHotkey(wParam!=0);return 0;
        case WM_APP_DESKTOP_WIN_RELEASE:
            OnDesktopWinReleased();return 0;
        case WM_INPUT:
            if(m_settings.touchpadGestures)
                ReadTouchpadRaw((HRAWINPUT)lParam,[&](ULONG fingers,double x,double y){OnTouchpad(fingers,x,y);});
            return DefWindowProcW(hwnd,msg,wParam,lParam);
        case WM_INPUT_DEVICE_CHANGE:
            g_touchpadDevices.erase((HANDLE)lParam);
            if(wParam==GIDC_REMOVAL) {g_touchpadFrame.clear();g_touchpadFrameExpected=0;m_touchStroke={};}
            return 0;
        case WM_APP_SEARCH:
            if(m_desktopOverview) return 0;
            if (!m_settings.compactCoverFlow || !m_settings.enableSearch) return 0;
            m_searchActive=true; g_searchTyping=true; g_stickyControls=true;
            UpdateSearch();
            return 0;
        case WM_APP_SEARCHCHAR:
            if(g_desktopRenameTyping) {
                if(wParam==kTextSelectAll) m_overviewRenameSelectAll=true;
                else {
                    if(m_overviewRenameSelectAll) {m_overviewRenameText.clear();m_overviewRenameSelectAll=false;}
                    if(wParam==kTextPaste) {
                        const auto pasted=ReadClipboardText(m_hwnd,128-m_overviewRenameText.size());
                        m_overviewRenameText.append(pasted,0,128-m_overviewRenameText.size());
                    }
                    else if(wParam==kTextDeleteWord) EraseLastWord(m_overviewRenameText);
                    else if(wParam==kTextBackspace) EraseLastUtf16CodePoint(m_overviewRenameText);
                    else if(wParam>=32) AppendUtf16Unit(m_overviewRenameText,(wchar_t)wParam,128);
                }
                return 0;
            }
            if (!m_searchActive) return 0;
            if (wParam==kTextSelectAll) m_searchSelectAll=true;
            else {
                if(m_searchSelectAll) {m_searchQuery.clear();m_searchSelectAll=false;}
                if(wParam==kTextPaste) {
                    const auto pasted=ReadClipboardText(m_hwnd,128-m_searchQuery.size());
                    m_searchQuery.append(pasted,0,128-m_searchQuery.size());
                }
                else if(wParam==kTextDeleteWord) EraseLastWord(m_searchQuery);
                else if(wParam==kTextBackspace) EraseLastUtf16CodePoint(m_searchQuery);
                else if(wParam>=32) AppendUtf16Unit(m_searchQuery,(wchar_t)wParam,128);
            }
            UpdateSearch();
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_RBUTTONDOWN:
            OnIconContext({(short)LOWORD(lParam),(short)HIWORD(lParam)});return 0;
        case WM_RBUTTONUP:
        case WM_CONTEXTMENU:
            return 0;
        case WM_APP_ICON_MENU_DISMISS:
            DismissIconContext();return 0;
        case WM_LBUTTONDOWN:
            OnClick({(short)LOWORD(lParam), (short)HIWORD(lParam)});
            return 0;

        case WM_MOUSEMOVE:
            OnPointerMove({(short)LOWORD(lParam), (short)HIWORD(lParam)});
            return 0;

        case WM_LBUTTONUP:
            OnPointerUp();
            return 0;

        case WM_CAPTURECHANGED:
        case WM_CANCELMODE:
            ClearPointerPress();
            if (m_dragWindow && !m_dragMoving && !m_dragDropped) RestoreDragWindow();
            return 0;

        case WM_APP + 37: {
            if(g_desktopRenameTyping || (m_state!=State::Open && m_state!=State::Pending)) return 0;
            g_lastNavigationInputTick=GetTickCount64();DismissIconContext();
            const bool last=wParam!=0;
            if(m_desktopOverview) {
                const OverviewWindow* chosen=nullptr;
                for(const auto& window:m_overviewWindows) {
                    DWORD pid=0;GetWindowThreadProcessId(window.hwnd,&pid);
                    if(!m_overviewDesktops.empty() && IsWindow(window.hwnd) && pid==window.pid &&
                        (IsEqualGUID(window.desktop,m_overviewDesktops[m_overviewSelected].id) || IsEqualGUID(window.desktop,GUID_NULL))) {
                        chosen=&window;if(!last) break;
                    }
                }
                if(chosen) {m_overviewSelectedWindow=chosen->hwnd;m_overviewSelectedPid=chosen->pid;}
            } else if(m_searchActive || m_settings.simpleStyle>=4) {
                if(!m_searchMatches.empty()) m_target=last ? m_searchMatches.back() : m_searchMatches.front();
            } else if(!m_items.empty()) m_target=last ? (int)m_items.size()-1 : 0;
            return 0;
        }
        case WM_APP + 36: {
            if(!m_settings.simpleStyle || m_desktopOverview || m_items.empty()) return 0;
            const ULONGLONG now=GetTickCount64();
            const int digit=(int)wParam;
            int number=now-m_simpleShortcutTime<650 ? m_simpleShortcutNumber*10+digit : digit;
            if(number>(int)m_items.size()) number=digit;
            m_simpleShortcutNumber=number; m_simpleShortcutTime=now;
            if(number>=1 && number<=(int)m_items.size()) m_target=number-1;
            return 1;
        }
        case WM_APP + 35:
            if (!g_ready || !g_takeOver) return 0;
            if (!g_switching.exchange(true)) {
                PostMessageW(m_hwnd, WM_APP_START, wParam | kStartFromHotkey, 0);
                PostThreadMessageW(g_hookThreadId, WM_HOOK_ENGAGE, 0, 0);
            } else {
                PostMessageW(m_hwnd, WM_APP_STEP,
                    (WPARAM)(INT_PTR)((wParam & kStartBackwards) ? -1 : 1), 0);
            }
            return 1;

        case WM_APP_START:
            OnStart(wParam);
            return 0;

        case WM_APP_STEP:
            OnStep((int)(INT_PTR)wParam);
            return 0;

        case WM_APP_WHEEL:
            if (m_desktopMenuOpen) {
                const int direction=(INT_PTR)wParam>0 ? -1 : 1;
                m_desktopScroll=std::clamp(m_desktopScroll+direction,0,
                    std::max(0,(int)m_desktopBridge.entries.size()-1));
                return 0;
            }
            m_wheelRemainder += (int)(INT_PTR)wParam;
            while (std::abs(m_wheelRemainder) >= WHEEL_DELTA) {
                // Wheel up brings the previous window to the front.
                const int direction = m_wheelRemainder > 0 ? 1 : -1;
                m_wheelRemainder -= direction * WHEEL_DELTA;
                int navigation = 1;
                if (m_settings.compactCoverFlow && !m_items.empty()) {
                    UpdateAppGroups();
                    navigation = 3;
                    POINT cursor;
                    if (GetCursorPos(&cursor) && ScreenToClient(m_hwnd, &cursor)) {
                        const D2D1_POINT_2F point=LayoutPoint(cursor);
                        const int selected = SelectedIndex();
                        for (auto it = m_drawList.rbegin(); it != m_drawList.rend(); ++it) {
                            if (!it->interactive || it->pose.opacity < 0.2f || !QuadContains(it->quad, point)) continue;
                            // Wheel over the selected application's stack cycles its windows.
                            for (int i = 0; i < (int)m_items.size(); ++i) {
                                if (m_items[i].get() == it->item &&
                                    m_groupOf[i] == m_groupOf[selected] &&
                                    m_appGroups[m_groupOf[selected]].size() > 1) navigation = 2;
                            }
                            break;
                        }
                    }
                }
                OnStep(-direction * navigation);
            }
            return 0;

        case WM_APP_COMMIT:
            OnCommit(-1);
            return 0;

        case WM_APP_CANCEL:
            OnCancel();
            return 0;

        case WM_APP_REFRESH_ICONS:
            // Skipped if a new session started meanwhile; it posts this again
            // when it ends.
            if (m_state == State::Idle && m_ctx && !m_dragMoving) {
                RefreshIcons();
            }
            return 0;

        case WM_POWERBROADCAST:
            if (wParam==PBT_POWERSETTINGCHANGE && lParam) {
                const auto* power=reinterpret_cast<const POWERBROADCAST_SETTING*>(lParam);
                if (power->DataLength>=sizeof(DWORD) &&
                    (IsEqualGUID(power->PowerSetting,kEnergySaverGuid) || IsEqualGUID(power->PowerSetting,kBatterySaverGuid))) {
                    DWORD status=0; memcpy(&status,power->Data,sizeof(status));
                    if (IsEqualGUID(power->PowerSetting,kEnergySaverGuid)) g_energySaverStatus=(int)status;
                    else g_batterySaverStatus=(int)status;
                    m_settings=LoadSettings();
                    if (!m_settings.blurBackground) m_background=nullptr;
                    // Rebuild the blur on the next opening, never capture our own overlay.
                }
            }
            return TRUE;

        case WM_APP_SETTINGS:
            m_settings = LoadSettings();
            ConfigureTouchpad();
            if(!m_settings.previewCache) m_windowSnapshots.clear();
            g_takeOver = true;
    g_searchEnabled = m_settings.compactCoverFlow && m_settings.enableSearch;
    g_desktopOverviewHotkey=m_settings.desktopOverviewHotkey;
    g_desktopOverviewHoldWin=m_settings.desktopOverviewHoldWin;
    g_ignoreRepeatedNavigationKeys=m_settings.ignoreRepeatedNavigationKeys;
            PostThreadMessageW(g_hookThreadId, WM_HOOK_UPDATE, 0, 0);
            m_backgroundKey.clear();
            m_titleFormat = nullptr;
            m_searchFormat = nullptr;
            m_searchMeasuredText.clear();
            m_searchMeasuredWidth=0;
            m_hoverTextFormat = nullptr;
            m_iconMenuText=nullptr;m_iconMenuSymbols=nullptr;DismissIconContext();
            m_controlSymbolFormat = nullptr;
            m_hoverTitleFormat = nullptr;
            m_hoverTitleLayout = nullptr;
            m_cursorTitleOpacity = 0;
            m_cursorTitle.clear();
            m_searchBarWidth = 0;
            if (!m_settings.enableSearch) {
                m_searchActive = false;
                g_searchTyping = false;
                g_stickyControls = false;
                g_searchHasResults = true;
                m_searchSelectAll = false;
                m_searchQuery.clear();
                m_searchMatches.clear();
                m_searchMask.clear();
                m_searchFrom.clear();
                m_searchRect = {};
            }
            if (!g_takeOver) {
                Deactivate();
            } else if (!m_d3dDevice) {
                Activate();
            } else {
                // Theme and Scale require a new monitor/swapchain geometry.
                // End an active session instead of mixing old and new sizes.
                if (m_state != State::Idle) {
                    EndSwitching();
                    EndSession();
                }
                PublishState();
            }
            return 0;

        case WM_SETTINGCHANGE:
        case WM_DISPLAYCHANGE:
            m_backgroundKey.clear();
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int Switcher::SelectedIndex() const {
    if (m_items.empty()) {
        return -1;
    }
    return PositiveMod(m_target, (int)m_items.size());
}

// Explorer received the Alt+Tab hotkey and handed it over: the keyboard hook
// didn't see the keys, usually because an elevated window has the focus.
void Switcher::TickTouchpadTaps(double now) {
    if(m_touchTapCount && !m_touchStroke.active && now>=m_touchTapDeadline) {
        m_touchTapCount=0;
        if(m_state==State::Open) {EndSwitching();OnCommit(-1);}
    }
}

void Switcher::OnDesktopHotkey(bool backwards) {

    if(m_desktopOverview && (m_state==State::Pending || m_state==State::Open)) {
        OnStep(backwards ? -1 : 1);
    } else {
        ToggleDesktopOverview(true);
        if(m_desktopOverview && m_settings.desktopOverviewHoldWin && (IsKeyDown(VK_LWIN) || IsKeyDown(VK_RWIN)))
            g_desktopWinHoldSession=true;
    }
}

void Switcher::OnDesktopWinReleased() {

    if(!m_desktopOverview || (m_state!=State::Pending && m_state!=State::Open)) return;
    if(g_physicalWinKeys.load()!=0 || g_desktopRenameTyping || m_overviewDeletePending || m_overviewPress) return;
    EndSwitching();OnCommit(-1);
}

void Switcher::ToggleDesktopOverview(bool force) {
    DismissIconContext();
    if(g_desktopRenameTyping) FinishDesktopRename(false);
    if(!force && m_desktopOverview) {
        if(m_items.size()==1 && m_items[0]->desktopPlaceholder) return;
        ClearOverviewCaptures();
        m_monitor=m_overviewSavedMonitor;m_windowScale=m_overviewSavedScale;
        m_background=m_overviewSavedBackground;m_backgroundKey=m_overviewSavedBackgroundKey;
        m_overviewSavedBackground=nullptr;ComputeLayout();
        if(m_state==State::Open) {
            if(!EnsureSwapChain(m_monitor.right-m_monitor.left,m_monitor.bottom-m_monitor.top)) {HandleDeviceLost();return;}
            SetWindowPos(m_hwnd,HWND_TOPMOST,m_monitor.left,m_monitor.top,m_monitor.right-m_monitor.left,m_monitor.bottom-m_monitor.top,SWP_NOACTIVATE);
        }
        m_desktopOverview=false;g_desktopOverviewActive=false;
        m_overviewWindows.clear();m_overviewDesktops.clear();
        m_overviewTransition=NowSeconds();return;
    }
    if(m_state==State::Closing) FinishClose();
    if(m_state==State::Idle) {
        m_openingOverview=true;OnStart(kStartFromHotkey);m_openingOverview=false;
        if(m_state!=State::Pending) return;
    }
    m_touchSession=true;g_switching=true;g_stickyControls=true;
    g_searchTyping=false;m_searchActive=false;m_desktopMenuOpen=false;
    if(!m_hasFocus) TakeFocus();SetTouchpadOwnership(true);
    PostThreadMessageW(g_hookThreadId,WM_HOOK_ENGAGE,0,0);
    ClearOverviewCaptures();
    if(!m_desktopOverview) {
        m_overviewSavedMonitor=m_monitor;m_overviewSavedScale=m_windowScale;
        m_overviewSavedBackground=m_background;m_overviewSavedBackgroundKey=m_backgroundKey;
    }
    m_overviewDesktops.clear();m_overviewWindows.clear();m_overviewSelected=0;
    m_drawList.clear();
    const int target=SelectedIndex(); // Normalize cyclic navigation before changing the item count.
    std::erase_if(m_items,[](const auto& item){return item->showDesktop;});
    if(m_items.empty()) {
        auto item=std::make_unique<Item>();item->desktopPlaceholder=true;item->minimized=true;
        item->rect=m_monitor;item->title=Text(L"Desktops",L"Desktop");m_items.push_back(std::move(item));
    }
    // Search/Enchanted indices refer to m_items, even with an empty query.
    // Rebuild after removing Desktop and adding any empty-desktop placeholder.
    UpdateSearch();
    m_target=std::min(target,(int)m_items.size()-1);
    m_scroll=(double)m_target;m_iconScroll=m_scroll;
    m_overviewExpanded=false;m_overviewDeletePending=false;
    m_overviewSelectedWindow=nullptr;m_overviewSelectedPid=0;
    m_overviewRefreshAt=0;m_overviewTooltip.clear();m_overviewTooltipOpacity=0;
    m_desktopOverview=true;g_desktopOverviewActive=true;m_overviewTransition=NowSeconds();
    MONITORINFO monitor{};
    monitor.cbSize=sizeof(monitor);
    if(GetMonitorInfoW(MonitorFromWindow(m_hwnd,MONITOR_DEFAULTTONEAREST),&monitor)) m_overviewWorld=monitor.rcMonitor;
    else m_overviewWorld=m_monitor;
    m_monitor=m_overviewWorld;m_windowScale=1;ComputeLayout();
    if(m_state==State::Open) {
        if(!EnsureSwapChain(m_monitor.right-m_monitor.left,m_monitor.bottom-m_monitor.top)) {HandleDeviceLost();return;}
        m_background=nullptr;m_backgroundKey.clear();
        if(m_settings.blurBackground) UpdateBackground();
        SetWindowPos(m_hwnd,HWND_TOPMOST,m_monitor.left,m_monitor.top,m_monitor.right-m_monitor.left,m_monitor.bottom-m_monitor.top,SWP_NOACTIVATE);
    }
    const HRESULT hr=m_desktopBridge.Refresh();
    if(FAILED(hr)) {Wh_Log(L"Desktop overview unavailable: 0x%08X",(UINT)hr);return;}
    GUID current{};m_desktopBridge.Current(&current);m_overviewCurrentDesktop=current;
    for(const auto& entry:m_desktopBridge.entries) {
        if(IsEqualGUID(entry.id,current)) m_overviewSelected=(int)m_overviewDesktops.size();
        OverviewDesktop desktop{};desktop.id=entry.id;desktop.name=entry.name;
        m_overviewDesktops.push_back(std::move(desktop));
    }
    m_overviewScroll=m_overviewSelected;
    EnumWindows(EnumOverviewWindow,reinterpret_cast<LPARAM>(this));

    size_t explorerWindows=0;
    for(const auto& window:m_overviewWindows) {
        wchar_t className[64]{};GetClassNameW(window.hwnd,className,64);
        if(!wcscmp(className,L"CabinetWClass") || !wcscmp(className,L"ExploreWClass")) ++explorerWindows;
        if(window.hwnd==m_previousForeground) {m_overviewSelectedWindow=window.hwnd;m_overviewSelectedPid=window.pid;}
    }
    Wh_Log(L"Desktop overview: %u desktops, %u windows, %u File Explorer windows",
        (UINT)m_overviewDesktops.size(),(UINT)m_overviewWindows.size(),(UINT)explorerWindows);
    EnumWindows(FindOverviewTaskbar,reinterpret_cast<LPARAM>(this));

    m_overviewLayoutWidth=0;m_overviewLayoutHeight=0;
    LoadOverviewWallpaper();
}

BOOL CALLBACK Switcher::FindOverviewTaskbar(HWND hwnd,LPARAM parameter) {
    auto& self=*reinterpret_cast<Switcher*>(parameter);
    wchar_t name[64]{};GetClassNameW(hwnd,name,64);
    if(wcscmp(name,L"Shell_TrayWnd") && wcscmp(name,L"Shell_SecondaryTrayWnd")) return TRUE;
    MONITORINFO monitor{};
    monitor.cbSize=sizeof(monitor);
    if(!GetMonitorInfoW(MonitorFromWindow(hwnd,MONITOR_DEFAULTTONEAREST),&monitor) || !EqualRect(&monitor.rcMonitor,&self.m_overviewWorld)) return TRUE;
    auto item=std::make_unique<Item>();item->hwnd=hwnd;GetWindowThreadProcessId(hwnd,&item->pid);
    if(!GetWindowBounds(hwnd,false,&item->rect)) return TRUE;
    self.StartCapture(*item);self.m_overviewTaskbar=std::move(item);return FALSE;
}

BOOL CALLBACK Switcher::EnumOverviewWindow(HWND hwnd, LPARAM parameter) {
        auto& self=*reinterpret_cast<Switcher*>(parameter);
        if(!IsSwitchableWindow(hwnd,true) || !self.m_desktopBridge.ShowsInSwitcher(hwnd)) return TRUE;
        GUID desktop{};RECT bounds{};DWORD pid=0;
        if(FAILED(self.m_desktopBridge.WindowDesktop(hwnd,&desktop))) {
            // Keep visible shell-managed windows when their desktop ID is unavailable.
            BOOL cloaked=FALSE;
            DwmGetWindowAttribute(hwnd,DWMWA_CLOAKED,&cloaked,sizeof(cloaked));
            if(cloaked || FAILED(self.m_desktopBridge.Current(&desktop))) return TRUE;
        }
        if(!GetWindowBounds(hwnd,IsIconic(hwnd)!=FALSE,&bounds)) return TRUE;
        GetWindowThreadProcessId(hwnd,&pid);
        wchar_t title[256]{};GetWindowTextW(hwnd,title,256);
        for(auto& window:self.m_overviewWindows) if(window.hwnd==hwnd && window.pid==pid) {
            window.desktop=desktop;window.rect=bounds;window.title=title;return TRUE;
        }
        OverviewWindow window{};window.hwnd=hwnd;window.desktop=desktop;
        window.rect=bounds;window.title=title;window.icon=self.GetIconBitmap(hwnd);window.pid=pid;
        self.m_overviewWindows.push_back(std::move(window));
        return self.m_overviewWindows.size()<256;
}

void Switcher::RefreshOverviewDesktops(const GUID& selected) {
    const HRESULT hr=m_desktopBridge.Refresh();
    if(FAILED(hr)) {m_overviewActionErrorUntil=NowSeconds()+3;Wh_Log(L"Desktop list refresh failed: 0x%08X",(UINT)hr);return;}
    std::vector<OverviewDesktop> desktops;
    for(const auto& entry:m_desktopBridge.entries) {
        OverviewDesktop desktop;desktop.id=entry.id;desktop.name=entry.name;
        for(auto& old:m_overviewDesktops) if(IsEqualGUID(old.id,entry.id)) {
            desktop.wallpaper=std::move(old.wallpaper);desktop.tried=old.tried;
            desktop.hoverAmount=old.hoverAmount;break;
        }
        desktops.push_back(std::move(desktop));
    }
    m_overviewDesktops=std::move(desktops);m_overviewSelected=0;
    for(int i=0;i<(int)m_overviewDesktops.size();++i) if(IsEqualGUID(m_overviewDesktops[i].id,selected)) m_overviewSelected=i;
    m_overviewRefreshAt=0;m_overviewTransition=NowSeconds();
}

void Switcher::FinishDesktopRename(bool save) {
    if(!g_desktopRenameTyping.exchange(false)) return;
    if(save) {
        const auto first=m_overviewRenameText.find_first_not_of(L" \t");
        const auto last=m_overviewRenameText.find_last_not_of(L" \t");
        const std::wstring name=first==std::wstring::npos ? L"" : m_overviewRenameText.substr(first,last-first+1);
        const HRESULT hr=m_desktopBridge.Rename(m_overviewRenameId,name);
        if(SUCCEEDED(hr)) {
            for(auto& desktop:m_overviewDesktops) if(IsEqualGUID(desktop.id,m_overviewRenameId)) desktop.name=name;
        } else {m_overviewActionErrorUntil=NowSeconds()+3;Wh_Log(L"Desktop rename failed: 0x%08X",(UINT)hr);}
    }
    m_overviewRenameText.clear();m_overviewRenameBox={};m_renameTextAnimation={};
    g_switching=true;g_stickyControls=true;
    PostThreadMessageW(g_hookThreadId,WM_HOOK_ENGAGE,0,0);
}

void Switcher::ClearOverviewCaptures() {
    if(m_overviewTaskbar) {StopCapture(*m_overviewTaskbar);m_overviewTaskbar.reset();}
    for(auto& window:m_overviewWindows) if(window.capture) {
        SaveWindowSnapshot(*window.capture);StopCapture(*window.capture);window.capture.reset();
    }
}

bool Switcher::RestoreOverviewWindow(HWND target,DWORD expected) {
    DWORD pid=0;GetWindowThreadProcessId(target,&pid);
    if(!IsWindow(target) || pid!=expected) return false;
    {
        WINDOWPLACEMENT placement{};placement.length=sizeof(placement);
        if(!GetWindowPlacement(target,&placement)) return false;
        placement.flags|=WPF_ASYNCWINDOWPLACEMENT;placement.showCmd=SW_SHOWNOACTIVATE;
        if(!SetWindowPlacement(target,&placement)) return false;
    }
    for(auto& window:m_overviewWindows) if(window.hwnd==target && window.pid==expected) {
        window.restoredInOverview=true;window.minimized=false;window.validateAt=0;
    }
    g_desktopWinHoldSession=false;g_stickyControls=true;
    return true;
}

void Switcher::RemoveOverviewDesktop() {
    if(!m_overviewDeletePending || m_overviewDesktops.size()<2) return;
    const GUID id=m_overviewDeleteId,fallback=m_overviewDesktops.front().id;
    m_overviewDeletePending=false;
    // The shell operation atomically moves remaining windows to the fallback.
    // Refresh before invoking it so an externally removed desktop cannot be reused.
    const HRESULT refreshed=m_desktopBridge.Refresh();
    const HRESULT hr=SUCCEEDED(refreshed) ? m_desktopBridge.Remove(id,fallback) : refreshed;
    if(SUCCEEDED(hr)) {
        for(auto& window:m_overviewWindows) if(IsEqualGUID(window.desktop,id)) window.desktop=fallback;
        m_overviewSelectedWindow=nullptr;m_overviewSelectedPid=0;RefreshOverviewDesktops(fallback);
        m_overviewRefreshAt=0;
    } else {m_overviewActionErrorUntil=NowSeconds()+3;Wh_Log(L"Desktop removal failed: 0x%08X",(UINT)hr);}
    g_switching=true;g_stickyControls=true;PostThreadMessageW(g_hookThreadId,WM_HOOK_ENGAGE,0,0);
}

void Switcher::DrawDesktopRemoval(float opacity) {
    if(!m_overviewDeletePending || m_overviewDesktops.empty()) return;
    const bool occupied=std::any_of(m_overviewWindows.begin(),m_overviewWindows.end(),[&](const auto& window) {
        return window.alive && IsEqualGUID(window.desktop,m_overviewDeleteId);
    });
    m_brush->SetColor(D2D1::ColorF(0,0,0,.65f*opacity));
    m_ctx->FillRectangle({0,0,m_width,m_height},m_brush.get());
    const float width=std::min(540*m_dpiScale,m_width-40*m_dpiScale),height=184*m_dpiScale;
    const D2D1_RECT_F panel{(m_width-width)*.5f,(m_height-height)*.5f,(m_width+width)*.5f,(m_height+height)*.5f};
    DrawUiPanel(panel,16*m_dpiScale,opacity);
    std::wstring message=occupied
        ? std::wstring(Text(L"There are windows open on this virtual desktop. Move them to ",L"Ci sono finestre aperte in questo desktop virtuale. Spostarle in "))+m_overviewDesktops.front().name+L"?"
        : Text(L"Close this virtual desktop?",L"Chiudere questo desktop virtuale?");
    m_brush->SetColor(UiForeground(opacity));
    m_overviewTitleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
    m_ctx->DrawText(message.c_str(),(UINT32)message.size(),m_overviewTitleFormat.get(),
        {panel.left+24*m_dpiScale,panel.top+22*m_dpiScale,panel.right-24*m_dpiScale,panel.bottom-70*m_dpiScale},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
    m_overviewTitleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    const float button=118*m_dpiScale;
    m_overviewDeleteCancel={panel.right-24*m_dpiScale-button,panel.bottom-54*m_dpiScale,panel.right-24*m_dpiScale,panel.bottom-18*m_dpiScale};
    m_overviewDeleteAccept={m_overviewDeleteCancel.left-button-12*m_dpiScale,m_overviewDeleteCancel.top,m_overviewDeleteCancel.left-12*m_dpiScale,m_overviewDeleteCancel.bottom};
    m_brush->SetColor(StyleColor(m_settings.highlightColor,.8f*opacity));
    m_ctx->FillRoundedRectangle({m_overviewDeleteAccept,7*m_dpiScale,7*m_dpiScale},m_brush.get());
    DrawUiPanel(m_overviewDeleteCancel,7*m_dpiScale,opacity);
    m_brush->SetColor(UiForeground(opacity));
    const auto accept=occupied ? Text(L"Move",L"Sposta") : Text(L"Close desktop",L"Chiudi desktop");
    m_ctx->DrawText(accept,(UINT32)wcslen(accept),m_overviewButtonFormat.get(),m_overviewDeleteAccept,m_brush.get());
    const auto cancel=Text(L"Cancel",L"Annulla");
    m_ctx->DrawText(cancel,(UINT32)wcslen(cancel),m_overviewButtonFormat.get(),m_overviewDeleteCancel,m_brush.get());
}

void Switcher::ClearLiveWallpaper() {
    if(m_liveWallpaper) {StopCapture(*m_liveWallpaper);m_liveWallpaper.reset();}
    m_liveWallpaperCheckAt=0;
}

void Switcher::UpdateLiveWallpaper() {
    if(!m_settings.liveDesktopWallpaper || (!m_desktopOverview && !m_settings.showDesktopCard)) {
        ClearLiveWallpaper();return;
    }
    const double now=NowSeconds();
    if(now>=m_liveWallpaperCheckAt) {
        m_liveWallpaperCheckAt=now+1.0;
        struct Search {RECT monitor;HWND host=nullptr;DWORD pid=0;};
        Search search{m_monitor};
        EnumWindows([](HWND host,LPARAM parameter) WINAPI ->BOOL {
            auto& search=*reinterpret_cast<Search*>(parameter);
            wchar_t name[64]{};GetClassNameW(host,name,64);
            if(wcscmp(name,L"WorkerW") && wcscmp(name,L"Progman")) return TRUE;
            // Never capture the icon-bearing desktop or the monitor itself:
            // both could include the switcher and feed its image back into itself.
            if(!IsWindowVisible(host) || FindWindowExW(host,nullptr,L"SHELLDLL_DefView",nullptr)) return TRUE;
            RECT bounds{},intersection{};
            if(!GetWindowRect(host,&bounds) || !IntersectRect(&intersection,&bounds,&search.monitor)) return TRUE;
            struct ChildSearch {bool found=false;};ChildSearch children;
            EnumChildWindows(host,[](HWND child,LPARAM parameter) WINAPI ->BOOL {
                auto& result=*reinterpret_cast<ChildSearch*>(parameter);
                DWORD pid=0;GetWindowThreadProcessId(child,&pid);
                HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
                wchar_t path[MAX_PATH]{};DWORD length=MAX_PATH;
                if(process) {
                    if(QueryFullProcessImageNameW(process,0,path,&length)) {
                        const wchar_t* leaf=wcsrchr(path,L'\\');leaf=leaf ? leaf+1 : path;
                        result.found=!_wcsicmp(leaf,L"wallpaper64.exe") || !_wcsicmp(leaf,L"wallpaper32.exe") ||
                            !_wcsicmp(leaf,L"webwallpaper32.exe") || !_wcsicmp(leaf,L"webwallpaper64.exe");
                    }
                    CloseHandle(process);
                }
                return !result.found;
            },reinterpret_cast<LPARAM>(&children));
            if(!children.found) return TRUE;
            search.host=host;GetWindowThreadProcessId(host,&search.pid);return FALSE;
        },reinterpret_cast<LPARAM>(&search));
        if(m_liveWallpaper && (m_liveWallpaper->hwnd!=search.host || m_liveWallpaper->pid!=search.pid)) {
            StopCapture(*m_liveWallpaper);m_liveWallpaper.reset();
            for(auto& item:m_items) if(item->showDesktop) item->content=nullptr;
        }
        if(search.host && !m_liveWallpaper) {
            auto item=std::make_unique<Item>();item->hwnd=search.host;item->pid=search.pid;
            if(GetWindowRect(search.host,&item->rect)) {
                StartCapture(*item);
                if(item->framePool) m_liveWallpaper=std::move(item);
            }
        }
    }
    if(!m_liveWallpaper) return;
    PollFrames(*m_liveWallpaper);
    if(!m_liveWallpaper->content) return;
    const auto size=m_liveWallpaper->content->GetPixelSize();
    const RECT& source=m_liveWallpaper->rect;
    RECT crop{};if(!IntersectRect(&crop,&source,&m_monitor)) return;
    const float sx=(float)size.width/std::max(1L,source.right-source.left);
    const float sy=(float)size.height/std::max(1L,source.bottom-source.top);
    const D2D1_RECT_U pixels{(UINT32)std::max(0.f,(crop.left-source.left)*sx),
        (UINT32)std::max(0.f,(crop.top-source.top)*sy),
        std::min(size.width,(UINT32)std::max(0.f,(crop.right-source.left)*sx)),
        std::min(size.height,(UINT32)std::max(0.f,(crop.bottom-source.top)*sy))};
    if(pixels.right<=pixels.left || pixels.bottom<=pixels.top) return;
    for(auto& item:m_items) if(item->showDesktop) {
        const D2D1_SIZE_U desired{pixels.right-pixels.left,pixels.bottom-pixels.top};
        const auto existing=item->content ? item->content->GetPixelSize() : D2D1_SIZE_U{};
        if(existing.width!=desired.width || existing.height!=desired.height) {
            item->content=nullptr;
            const auto props=D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED));
            m_ctx->CreateBitmap(desired,nullptr,0,&props,item->content.put());
        }
        if(item->content) item->content->CopyFromBitmap(nullptr,m_liveWallpaper->content.get(),&pixels);
    }
}

bool Switcher::DrawLiveWallpaper(const D2D1_RECT_F& destination,float opacity) {
    if(!m_liveWallpaper || !m_liveWallpaper->content) return false;
    const auto size=m_liveWallpaper->content->GetSize();
    const RECT& source=m_liveWallpaper->rect;RECT crop{};
    if(!IntersectRect(&crop,&source,&m_monitor)) return false;
    const float sx=size.width/std::max(1L,source.right-source.left),sy=size.height/std::max(1L,source.bottom-source.top);
    const D2D1_RECT_F pixels{(crop.left-source.left)*sx,(crop.top-source.top)*sy,(crop.right-source.left)*sx,(crop.bottom-source.top)*sy};
    m_ctx->DrawBitmap(m_liveWallpaper->content.get(),destination,opacity,D2D1_INTERPOLATION_MODE_LINEAR,&pixels);
    return true;
}

void Switcher::UpdateOverviewCaptures() {
    if(m_overviewDesktops.empty()) return;
    if(m_overviewTaskbar) PollFrames(*m_overviewTaskbar);
    const double now=NowSeconds();
    if(now>=m_overviewRefreshAt) {
        m_overviewRefreshAt=now+0.75;
        GUID current{};
        if(SUCCEEDED(m_desktopBridge.Current(&current))) m_overviewCurrentDesktop=current;
        std::erase_if(m_overviewWindows,[&](OverviewWindow& window) {
            DWORD pid=0;GetWindowThreadProcessId(window.hwnd,&pid);
            if(IsSwitchableWindow(window.hwnd,true) && pid==window.pid && m_desktopBridge.ShowsInSwitcher(window.hwnd)) return false;
            if(window.capture) StopCapture(*window.capture);
            if(window.hwnd==m_overviewSelectedWindow) {m_overviewSelectedWindow=nullptr;m_overviewSelectedPid=0;}
            return true;
        });
        EnumWindows(EnumOverviewWindow,reinterpret_cast<LPARAM>(this));
    }
    // Never restore minimized apps for previews: use cached snapshots or icons.
    const size_t limit=m_settings.energySaver ? 2 : 4;
    size_t active=0;
    for(auto& window:m_overviewWindows) if(window.capture) {
        bool nearby=false;
        for(int i=std::max(0,m_overviewSelected-1);i<=std::min((int)m_overviewDesktops.size()-1,m_overviewSelected+1);++i)
            nearby|=IsEqualGUID(window.desktop,m_overviewDesktops[i].id)!=FALSE;
        DWORD pid=0;GetWindowThreadProcessId(window.hwnd,&pid);
        if(!nearby || !IsWindow(window.hwnd) || pid!=window.pid) {
            SaveWindowSnapshot(*window.capture);StopCapture(*window.capture);window.capture.reset();
        } else {PollFrames(*window.capture);++active;}
    }
    if(active>=limit) return;
    for(int index:CaptureOrder((int)m_overviewDesktops.size(),m_overviewSelected)) {
        if(std::abs(index-m_overviewSelected)>1) continue;
        for(auto& window:m_overviewWindows) {
            if(window.capture || !IsEqualGUID(window.desktop,m_overviewDesktops[index].id) || IsIconic(window.hwnd)) continue;
            bool existing=false;
            for(const auto& item:m_items) if(item->hwnd==window.hwnd && item->framePool) {existing=true;break;}
            if(existing) continue;
            DWORD pid=0;GetWindowThreadProcessId(window.hwnd,&pid);
            if(!IsWindow(window.hwnd) || pid!=window.pid) continue;
            auto item=std::make_unique<Item>();item->hwnd=window.hwnd;item->pid=pid;
            item->rect=window.rect;item->title=window.title;item->icon=window.icon;
            HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
            if(process) {FILETIME creation{},exit{},kernel{},user{};
                if(GetProcessTimes(process,&creation,&exit,&kernel,&user)) item->processCreation=FileTimeValue(creation);
                CloseHandle(process);}
            StartCapture(*item);window.capture=std::move(item);return;
        }
    }
}

void Switcher::LoadOverviewWallpaper() {
    if(m_overviewDesktops.empty()) return;
    const int count=(int)m_overviewDesktops.size(),limit=m_settings.energySaver ? 3 : 7;
    auto visible=[&](int index) {
        return count<=4 || PositiveMod(index-m_overviewSelected,count)<limit;
    };
    // Decode one visible wallpaper per frame and bound the retained textures.
    for(int offset=0;offset<count;++offset) {
        const int index=PositiveMod(m_overviewSelected+offset,count);
        if(!visible(index)) continue;
        auto& desktop=m_overviewDesktops[index];
        if(desktop.tried) continue;
        desktop.tried=true;
        std::wstring path=m_desktopBridge.Wallpaper(desktop.id);
        desktop.wallpaper=LoadWallpaperPreview(std::move(path));
        break;
    }
    for(int index=0;index<count;++index) if(!visible(index)) {
        m_overviewDesktops[index].wallpaper=nullptr;m_overviewDesktops[index].tried=false;
    }
}

struct OverviewLayout {
    float width,height,centerX,centerY,sidebar;
};

static Pose DesktopGalleryPose(int count,int index,float relative,float screenWidth,float screenHeight,
    float worldWidth,float worldHeight,float dpi) {
    Pose pose;
    float width=std::min(screenWidth*.52f,screenHeight*.52f*worldWidth/worldHeight);
    pose.scale=width/worldWidth;
    if(count==4) {
        width=std::min(screenWidth*.39f,(screenHeight-220*dpi)*.43f*worldWidth/worldHeight);
        pose.scale=std::max(1.f,width)/worldWidth;
        pose.x=(index%2 ? 1.f : -1.f)*(width*.5f+24*dpi);
        pose.y=(index/2 ? 1.f : -1.f)*(width*worldHeight/worldWidth*.5f+34*dpi)-10*dpi;
    } else if(count>4) {
        pose.scale*=.80f;
        pose.x=screenWidth*(.11f-.085f*relative);pose.y=screenHeight*(.015f-.055f*relative);
        pose.z=screenWidth*.15f*relative;pose.angleY=-.40f;pose.angleX=.06f;pose.angleZ=-.02f;
    } else {
        pose.x=relative*screenWidth*.38f;pose.z=std::fabs(relative)*screenWidth*.20f;
        pose.angleY=-std::clamp(relative,-1.f,1.f)*.38f;
    }
    return pose;
}

int Switcher::HitOverviewDesktop(D2D1_POINT_2F point) const {
    if(m_overviewDragging || !m_overviewExpanded) for(int i=0;i<(int)m_overviewDropTargets.size();++i)
        if(InRect(point,m_overviewDropTargets[i])) return i;
    if(!m_overviewExpanded) {
        for(auto it=m_overviewPaintOrder.rbegin();it!=m_overviewPaintOrder.rend();++it)
            if(*it>=0 && *it<(int)m_overviewDesktops.size() &&
                m_overviewDesktops[*it].hit.right>m_overviewDesktops[*it].hit.left &&
                QuadContains(m_overviewDesktops[*it].quad,point)) return *it;
        return -1;
    }
    return m_overviewSelected<(int)m_overviewDesktops.size() && InRect(point,m_overviewDesktops[m_overviewSelected].hit) ? m_overviewSelected : -1;
}

void Switcher::ExpandOverviewDesktop(int index) {
    if(index<0 || index>=(int)m_overviewDesktops.size()) return;
    const float worldWidth=std::max(1L,m_overviewWorld.right-m_overviewWorld.left);
    const float worldHeight=std::max(1L,m_overviewWorld.bottom-m_overviewWorld.top);
    const float aspect=worldWidth/worldHeight;
    const float startWidth=std::min(m_width*.52f,m_height*.52f*aspect);
    m_overviewSelected=index;
    m_overviewSelectedWindow=nullptr;
    m_overviewSelectedPid=0;
    m_overviewLayoutWidth=startWidth;
    m_overviewLayoutHeight=startWidth/aspect;
    m_overviewLayoutX=m_width*.5f;
    m_overviewLayoutY=m_height*.48f;
    m_overviewPreviousDesktop={};
    m_overviewDesktopTransition=NowSeconds();
    m_overviewExpanded=true;
    m_overviewTransition=NowSeconds();
}

void Switcher::DrawDesktopGallery(float opacity) {
    if(!m_overviewDrawContext && FAILED(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,m_overviewDrawContext.put()))) return;
    m_overviewDrawContext->SetDpi(96,96);m_overviewDrawContext->SetUnitMode(D2D1_UNIT_MODE_PIXELS);
    const int count=(int)m_overviewDesktops.size();
    const float worldWidth=std::max(1L,m_overviewWorld.right-m_overviewWorld.left);
    const float worldHeight=std::max(1L,m_overviewWorld.bottom-m_overviewWorld.top);
    const bool grid=count==4,stack=count>4;
    float delta=(float)m_overviewSelected-(float)m_overviewScroll;
    if(stack) {
        delta=(float)m_overviewSelected-(float)m_overviewScroll;
    } else if(!grid) {
        if(delta>count*.5f) delta-=count;
        if(delta< -count*.5f) delta+=count;
    }
    m_overviewScroll+=delta*(m_settings.animateSelection ? 1-std::exp(-18*m_motionDt) : 1);
    m_overviewPaintOrder.clear();
    for(int i=0;i<count;++i) {
        m_overviewDesktops[i].hit={};
        m_overviewDesktops[i].quad={};
        m_overviewDesktops[i].rename={};
        m_overviewDesktops[i].remove={};
    }
    auto distance=[&](int i) {
        if(stack) return (float)i-(float)m_overviewScroll;
        const float relative=PositiveMod((float)i-(float)m_overviewScroll,(float)count);
        return relative>count*.5f ? relative-count : relative;
    };
    for(int i=0;i<count;++i) if(grid || std::fabs(distance(i))<(m_settings.energySaver ? 3.0f : 7.0f)) m_overviewPaintOrder.push_back(i);
    for(int i=0;i<count;++i) if(std::find(m_overviewPaintOrder.begin(),m_overviewPaintOrder.end(),i)==m_overviewPaintOrder.end()) {
        m_overviewDesktops[i].surface=nullptr;m_overviewDesktops[i].projection=nullptr;
    }
    std::stable_sort(m_overviewPaintOrder.begin(),m_overviewPaintOrder.end(),[&](int a,int b) {return std::fabs(distance(a))>std::fabs(distance(b));});
    POINT cursor{};const bool hasCursor=GetCursorPos(&cursor) && ScreenToClient(m_hwnd,&cursor);
    const auto pointer=LayoutPoint(cursor);
    for(int index:m_overviewPaintOrder) {
        auto& desktop=m_overviewDesktops[index];
        const UINT32 textureWidth=(UINT32)std::min(960.f,worldWidth);
        const UINT32 textureHeight=(UINT32)std::max(1.f,textureWidth*worldHeight/worldWidth);
        const D2D1_SIZE_U size{textureWidth,textureHeight};
        if(!desktop.surface || desktop.surface->GetPixelSize().width!=size.width || desktop.surface->GetPixelSize().height!=size.height) {
            desktop.surface=nullptr;
            const auto properties=D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED));
            if(FAILED(m_ctx->CreateBitmap(size,nullptr,0,&properties,desktop.surface.put()))) continue;
        }
        const auto output=m_ctx;m_ctx=m_overviewDrawContext;
        m_ctx->SetTarget(desktop.surface.get());m_ctx->SetTransform(D2D1::Matrix3x2F::Identity());
        m_ctx->BeginDraw();m_ctx->Clear(m_settings.backgroundColor);
        const D2D1_RECT_F bounds{0,0,(float)size.width,(float)size.height};
        if(!DrawLiveWallpaper(bounds,1) && desktop.wallpaper) m_ctx->DrawBitmap(desktop.wallpaper.get(),bounds,1,D2D1_INTERPOLATION_MODE_LINEAR);
        // Render back to front, preserving the actual desktop positions.
        for(auto it=m_overviewWindows.rbegin();it!=m_overviewWindows.rend();++it) {
            const auto& window=*it;
            if(!window.alive || window.minimized || !IsEqualGUID(window.desktop,desktop.id)) continue;
            ID2D1Bitmap1* bitmap=window.capture ? window.capture->content.get() : nullptr;
            if(!bitmap) for(const auto& item:m_items) if(item->hwnd==window.hwnd && item->pid==window.pid) {bitmap=item->content.get();break;}
            if(!bitmap) {const auto saved=m_windowSnapshots.find(window.hwnd);
                if(saved!=m_windowSnapshots.end() && saved->second.pid==window.pid && saved->second.title==window.title) bitmap=saved->second.bitmap.get();}
            const D2D1_RECT_F windowRect{(window.rect.left-m_overviewWorld.left)*size.width/worldWidth,
                (window.rect.top-m_overviewWorld.top)*size.height/worldHeight,(window.rect.right-m_overviewWorld.left)*size.width/worldWidth,
                (window.rect.bottom-m_overviewWorld.top)*size.height/worldHeight};
            if(bitmap) m_ctx->DrawBitmap(bitmap,windowRect,1,D2D1_INTERPOLATION_MODE_LINEAR);
            else {m_brush->SetColor(m_settings.backgroundColor);m_ctx->FillRectangle(windowRect,m_brush.get());
                if(window.icon) {const float icon=std::min(28.f,std::min(windowRect.right-windowRect.left,windowRect.bottom-windowRect.top)*.5f);
                    const float x=(windowRect.left+windowRect.right-icon)*.5f,y=(windowRect.top+windowRect.bottom-icon)*.5f;
                    m_ctx->DrawBitmap(window.icon.get(),{x,y,x+icon,y+icon},1,D2D1_INTERPOLATION_MODE_LINEAR);}}
            if(window.hwnd==m_overviewSelectedWindow) {m_brush->SetColor(m_settings.highlightColor);m_ctx->DrawRectangle(windowRect,m_brush.get(),3);}
        }
        const HRESULT drawn=m_ctx->EndDraw();m_ctx->SetTarget(nullptr);m_ctx=output;
        if(FAILED(drawn)) continue;
        const float relative=distance(index);
        const Pose pose=DesktopGalleryPose(count,index,relative,m_width,m_height,worldWidth,worldHeight,m_dpiScale);
        desktop.quad=Project(pose,worldWidth,worldHeight);
        const auto& q=desktop.quad;
        D2D1_RECT_F hit{q.p[0].x,q.p[0].y,q.p[0].x,q.p[0].y};
        for(const auto p:q.p) {
            hit.left=std::min(hit.left,p.x);
            hit.right=std::max(hit.right,p.x);
            hit.top=std::min(hit.top,p.y);
            hit.bottom=std::max(hit.bottom,p.y);
        }
        desktop.hit=hit;
        if(!desktop.projection) m_ctx->CreateEffect(CLSID_D2D13DTransform,desktop.projection.put());
        if(desktop.projection) {
            desktop.projection->SetInput(0,desktop.surface.get());
            desktop.projection->SetValue(kTransform3DPropInterpolationMode,kTransform3DInterpolationAnisotropic);
            desktop.projection->SetValue(kTransform3DPropTransformMatrix,RectToQuadMatrix((float)size.width,(float)size.height,q));
            auto parameters=D2D1::LayerParameters1();parameters.opacity=opacity;
            m_ctx->PushLayer(parameters,nullptr);m_ctx->DrawImage(desktop.projection.get());m_ctx->PopLayer();
        }
        m_brush->SetColor(StyleColor(index==m_overviewSelected ? m_settings.highlightColor : m_settings.borderColor,opacity));
        for(int edge=0;edge<4;++edge) m_ctx->DrawLine(q.p[edge],q.p[(edge+1)%4],m_brush.get(),(index==m_overviewSelected ? 2 : 1)*m_dpiScale);
        if(grid || index==m_overviewSelected) {
            const D2D1_RECT_F footer{hit.left,hit.bottom+8*m_dpiScale,hit.right,hit.bottom+46*m_dpiScale};
            m_brush->SetColor(UiForeground(opacity));
            m_ctx->DrawText(desktop.name.c_str(),(UINT32)desktop.name.size(),m_overviewTitleFormat.get(),
                {footer.left+42*m_dpiScale,footer.top,footer.right-42*m_dpiScale,footer.bottom},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
            if(index==m_overviewSelected || (hasCursor && (InRect(pointer,hit) || InRect(pointer,footer)))) {
                desktop.rename={footer.left,footer.top,footer.left+36*m_dpiScale,footer.bottom};
                if(index>0) desktop.remove={footer.right-36*m_dpiScale,footer.top,footer.right,footer.bottom};
                const auto pencil=L"\uE70F",trash=L"\uE74D";
                m_ctx->DrawText(pencil,1,m_overviewSymbolFormat.get(),desktop.rename,m_brush.get());
                if(index>0) {m_brush->SetColor(D2D1::ColorF(.95f,.22f,.25f,opacity));m_ctx->DrawText(trash,1,m_overviewSymbolFormat.get(),desktop.remove,m_brush.get());}
            }
        }
    }
    const float x=m_width-72*m_dpiScale,y=24*m_dpiScale;
    m_overviewAdd={x,y,x+42*m_dpiScale,y+42*m_dpiScale};
    DrawUiPanel(m_overviewAdd,10*m_dpiScale,opacity);m_brush->SetColor(UiForeground(opacity));
    m_ctx->DrawText(L"+",1,m_overviewTitleFormat.get(),m_overviewAdd,m_brush.get());
    const int visible=std::min(count,9),start=std::clamp(m_overviewSelected-visible/2,0,std::max(0,count-visible));
    const float gap=8*m_dpiScale,chip=std::min(150*m_dpiScale,(m_width-48*m_dpiScale)/std::max(1,visible)-gap);
    const float left=(m_width-visible*(chip+gap)+gap)*.5f;
    m_overviewDropTargets.resize(count);
    for(int i=0;i<visible;++i) {
        const int index=start+i;
        const D2D1_RECT_F rect{left+i*(chip+gap),m_height-104*m_dpiScale,left+i*(chip+gap)+chip,m_height-66*m_dpiScale};
        m_overviewDropTargets[index]=rect;
        DrawUiPanel(rect,8*m_dpiScale,opacity);
        if(index==m_overviewSelected) {
            m_brush->SetColor(StyleColor(m_settings.highlightColor,.45f*opacity));
            m_ctx->FillRoundedRectangle({rect,8*m_dpiScale,8*m_dpiScale},m_brush.get());
            m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity));
            m_ctx->DrawRoundedRectangle({rect,8*m_dpiScale,8*m_dpiScale},m_brush.get(),2*m_dpiScale);
        }
        m_brush->SetColor(UiForeground(opacity));const auto& name=m_overviewDesktops[index].name;
        m_ctx->DrawText(name.c_str(),(UINT32)name.size(),m_overviewButtonFormat.get(),rect,m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }
    const auto hint=Text(L"Tab: next desktop · Click: switch · Right-click: preview",L"Tab: desktop successivo · Clic: cambia desktop · Clic destro: anteprima");
    m_ctx->DrawText(hint,(UINT32)wcslen(hint),m_overviewButtonFormat.get(),{20*m_dpiScale,m_height-48*m_dpiScale,m_width-20*m_dpiScale,m_height-12*m_dpiScale},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void Switcher::DrawDesktopOverview(float opacity) {
    if(!m_overviewFormat) {
        if(FAILED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,16*m_dpiScale,L"",m_overviewFormat.put()))) return;
        m_overviewFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_overviewFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    if(!m_overviewTitleFormat) {
        if(FAILED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,DWRITE_FONT_WEIGHT_SEMI_BOLD,
            DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,16*m_dpiScale,L"",m_overviewTitleFormat.put()))) return;
        m_overviewTitleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_overviewTitleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        m_overviewTitleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        com_ptr<IDWriteInlineObject> ellipsis;
        if(SUCCEEDED(m_dwriteFactory->CreateEllipsisTrimmingSign(m_overviewTitleFormat.get(),ellipsis.put()))) {
            const DWRITE_TRIMMING trim{DWRITE_TRIMMING_GRANULARITY_CHARACTER,0,0};
            m_overviewTitleFormat->SetTrimming(&trim,ellipsis.get());
        }
    }
    if(!m_overviewButtonFormat) {
        if(FAILED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,12*m_dpiScale,L"",m_overviewButtonFormat.put()))) return;
        m_overviewButtonFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_overviewButtonFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        m_overviewButtonFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    if(!m_overviewSymbolFormat) {
        if(FAILED(m_dwriteFactory->CreateTextFormat(CoverTabbyIconFont(),nullptr,DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,13*m_dpiScale,L"",m_overviewSymbolFormat.put()))) return;
        m_overviewSymbolFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_overviewSymbolFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        m_overviewSymbolFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    const float transition=std::clamp((float)((NowSeconds()-m_overviewTransition)/0.18),0.0f,1.0f);
    // The common overlay fade already handles entry opacity.
    m_brush->SetColor(D2D1::ColorF(1,1,1,opacity));
    const double overviewNow=NowSeconds();
    const float switchProgress=EaseOutQuart(std::clamp((float)((overviewNow-m_overviewDesktopTransition)/.28),0.f,1.f));
    if(switchProgress>=1.f) m_overviewPreviousDesktop={};
    const float dockBlend=(float)(1-std::exp(-18*m_motionDt));
    for(auto& window:m_overviewWindows) {
        window.hit={};window.hitDesktop=-1;window.dockHit={};window.dockTarget=false;
        if(overviewNow>=window.validateAt) {
            DWORD pid=0;GetWindowThreadProcessId(window.hwnd,&pid);
            window.alive=IsWindow(window.hwnd) && pid==window.pid;
            window.minimized=window.alive && IsIconic(window.hwnd);
            window.validateAt=overviewNow+0.20;
        }
    }
    for(auto& desktop:m_overviewDesktops) {desktop.hit={};desktop.rename={};desktop.remove={};}
    m_overviewAdd={};m_overviewRenameBox={};m_overviewBack={};m_overviewDropTargets.clear();
    m_brush->SetColor(UiForeground(opacity));
    if(m_overviewDesktops.empty()) {
        const auto error=Text(L"Desktop overview unavailable on this Windows build",L"Panoramica desktop non disponibile su questa versione di Windows");
        m_ctx->DrawText(error,(UINT32)wcslen(error),m_overviewFormat.get(),{0,m_height*.4f,m_width,m_height*.6f},m_brush.get());return;
    }

    if(!m_overviewExpanded) {DrawDesktopGallery(opacity);DrawDesktopRemoval(opacity);return;}
    m_overviewBack={20*m_dpiScale,20*m_dpiScale,140*m_dpiScale,58*m_dpiScale};
    DrawUiPanel(m_overviewBack,9*m_dpiScale,opacity);
    m_brush->SetColor(UiForeground(opacity));
    const auto back=Text(L"‹ Desktops",L"‹ Desktop");
    m_ctx->DrawText(back,(UINT32)wcslen(back),m_overviewButtonFormat.get(),m_overviewBack,m_brush.get());
    m_overviewScroll+=(m_overviewSelected-m_overviewScroll)*(1-std::exp(-18*m_motionDt));
    const float worldWidth=std::max(1L,m_overviewWorld.right-m_overviewWorld.left);
    const float worldHeight=std::max(1L,m_overviewWorld.bottom-m_overviewWorld.top);
    std::vector<OverviewWindow*> minimized;
    for(auto& window:m_overviewWindows) {
        if(window.alive && (window.minimized || window.restoredInOverview) &&
            (IsEqualGUID(window.desktop,m_overviewDesktops[m_overviewSelected].id) || IsEqualGUID(window.desktop,GUID_NULL)))
            minimized.push_back(&window);
    }
    // Ease the selected gallery card into a full-screen canvas.
    const OverviewLayout layout{m_width,m_height,m_width*.5f,m_height*.5f,0};
    if(m_overviewLayoutWidth<=0) {
        m_overviewLayoutWidth=layout.width;m_overviewLayoutHeight=layout.height;
        m_overviewLayoutX=layout.centerX;m_overviewLayoutY=layout.centerY;
    }
    const float settle=(float)(1-std::exp(-16*m_motionDt));
    m_overviewLayoutWidth=Lerp(m_overviewLayoutWidth,layout.width,settle);
    m_overviewLayoutHeight=Lerp(m_overviewLayoutHeight,layout.height,settle);
    m_overviewLayoutX=Lerp(m_overviewLayoutX,layout.centerX,settle);
    m_overviewLayoutY=Lerp(m_overviewLayoutY,layout.centerY,settle);
    if(std::fabs(m_overviewLayoutWidth-layout.width)<.1f) m_overviewLayoutWidth=layout.width;
    if(std::fabs(m_overviewLayoutHeight-layout.height)<.1f) m_overviewLayoutHeight=layout.height;
    const float width=m_overviewLayoutWidth,height=m_overviewLayoutHeight,sidebar=layout.sidebar;
    const float centerX=m_overviewLayoutX,centerY=m_overviewLayoutY+10*m_dpiScale*(1-EaseOutQuart(transition));
    const float dockClose=m_state==State::Closing ? EaseOutQuart(m_open.Progress(overviewNow)) : 0;
    POINT cursor{};const bool hasCursor=GetCursorPos(&cursor) && ScreenToClient(m_hwnd,&cursor);
    const auto pointer=LayoutPoint(cursor);
    std::wstring hoveredTitle;
    if(!minimized.empty()) {
        const float tileWidth=172*m_dpiScale,tileHeight=110*m_dpiScale,gap=8*m_dpiScale;
        const float top=(74*m_dpiScale)-12*m_dpiScale*dockClose;
        const int visible=std::max(1,(int)((m_width-sidebar-80*m_dpiScale)/(tileWidth+gap)));
        int selectedMini=-1;
        for(int i=0;i<(int)minimized.size();++i) if(minimized[i]->hwnd==m_overviewSelectedWindow) selectedMini=i;
        const int start=selectedMini<0 ? 0 : std::clamp(selectedMini-visible/2,0,std::max(0,(int)minimized.size()-visible));
        const int count=std::min(visible,(int)minimized.size());
        const float left=centerX-(count*tileWidth+(count-1)*gap)*.5f;
        const D2D1_RECT_F dock{
            24*m_dpiScale,
            top-10*m_dpiScale,
            m_width-24*m_dpiScale,
            top+tileHeight+10*m_dpiScale};
        m_brush->SetColor(m_settings.darkBackground ? D2D1::ColorF(.10f,.10f,.12f,opacity*.96f) : D2D1::ColorF(.94f,.94f,.96f,opacity*.96f));
        m_ctx->FillRoundedRectangle({dock,18*m_dpiScale,18*m_dpiScale},m_brush.get());
        m_brush->SetColor(D2D1::ColorF(1,1,1,opacity*.14f));
        m_ctx->DrawRoundedRectangle({dock,18*m_dpiScale,18*m_dpiScale},m_brush.get(),m_dpiScale);
        for(int i=0;i<count;++i) {
            auto& window=*minimized[start+i];
            const D2D1_RECT_F targetTile{left+i*(tileWidth+gap),top,left+i*(tileWidth+gap)+tileWidth,top+tileHeight};
            window.dockTarget=true;
            if(window.dockAmount<0.001f) window.dockVisual={targetTile.left,targetTile.top-8*m_dpiScale,targetTile.right,targetTile.bottom-8*m_dpiScale};
            window.dockAmount+=(1-window.dockAmount)*dockBlend;
            window.dockVisual={Lerp(window.dockVisual.left,targetTile.left,dockBlend),Lerp(window.dockVisual.top,targetTile.top,dockBlend),Lerp(window.dockVisual.right,targetTile.right,dockBlend),Lerp(window.dockVisual.bottom,targetTile.bottom,dockBlend)};
            const auto tile=window.dockVisual;
            window.hit=tile;window.dockHit=tile;window.hitDesktop=m_overviewSelected;
            const bool hovered=hasCursor && InRect(pointer,tile),selected=window.hwnd==m_overviewSelectedWindow;
            window.dockHover+=(float)(((hovered ? 1.0f : 0.0f)-window.dockHover)*(1-std::exp(-20*m_motionDt)));
            window.dockFocus+=(float)(((selected ? 1.0f : 0.0f)-window.dockFocus)*(1-std::exp(-18*m_motionDt)));
            if(window.dockHover>.01f || window.dockFocus>.01f) {
                m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity*(.25f*window.dockFocus+.14f*window.dockHover)));
                m_ctx->FillRoundedRectangle({tile,12*m_dpiScale,12*m_dpiScale},m_brush.get());
                if(selected) {m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity));
                    m_ctx->DrawRoundedRectangle({tile,12*m_dpiScale,12*m_dpiScale},m_brush.get(),m_dpiScale);}
            }
            ID2D1Bitmap1* preview=window.capture ? window.capture->content.get() : nullptr;
            if(!preview) {
                const auto saved=m_windowSnapshots.find(window.hwnd);
                if(saved!=m_windowSnapshots.end() && saved->second.pid==window.pid && saved->second.title==window.title) preview=saved->second.bitmap.get();
            }
            if(preview) m_ctx->DrawBitmap(preview,{tile.left+6*m_dpiScale,tile.top+6*m_dpiScale,tile.right-6*m_dpiScale,tile.bottom-32*m_dpiScale},opacity*window.dockAmount,D2D1_INTERPOLATION_MODE_LINEAR);
            if(window.icon) m_ctx->DrawBitmap(window.icon.get(),{tile.left+7*m_dpiScale,tile.bottom-26*m_dpiScale,tile.left+27*m_dpiScale,tile.bottom-6*m_dpiScale},opacity*window.dockAmount,D2D1_INTERPOLATION_MODE_LINEAR);
            m_brush->SetColor(UiForeground(opacity*window.dockAmount));
            m_ctx->DrawText(window.title.c_str(),(UINT32)window.title.size(),m_overviewButtonFormat.get(),
                {tile.left+32*m_dpiScale,tile.bottom-29*m_dpiScale,tile.right-6*m_dpiScale,tile.bottom-3*m_dpiScale},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
            if(hovered) hoveredTitle=window.title;
        }
    }
    m_ctx->PushAxisAlignedClip(D2D1::RectF(0,0,m_width,m_height),D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    for(int index=0;index<(int)m_overviewDesktops.size();++index) {
        const bool previousDesktop= !IsEqualGUID(m_overviewPreviousDesktop,GUID_NULL) &&
            IsEqualGUID(m_overviewDesktops[index].id,m_overviewPreviousDesktop) && index!=m_overviewSelected;
        if(index!=m_overviewSelected && !previousDesktop) continue;
        auto& desktop=m_overviewDesktops[index];
        const float slide=(previousDesktop
            ? -m_overviewTransitionDirection*m_width*switchProgress
            : m_overviewTransitionDirection*m_width*(1-switchProgress));
        const float center=centerX+slide;
        const float desktopOpacity=opacity*(previousDesktop ? 1-switchProgress : switchProgress);
        const D2D1_RECT_F card{center-width*.5f,centerY-height*.5f,center+width*.5f,centerY+height*.5f};
        desktop.hit=!previousDesktop ? D2D1::RectF(0,0,m_width,m_height) : D2D1_RECT_F{};
        if(card.right<0 || card.left>m_width) continue;
        if(!m_overviewCardMask || m_overviewMaskWidth!=width || m_overviewMaskHeight!=height || m_overviewMaskDpi!=m_dpiScale) {
            m_overviewCardMask=nullptr;
            const float corner=0.f;
            if(FAILED(m_d2dFactory->CreateRoundedRectangleGeometry({{0,0,width,height},corner,corner},m_overviewCardMask.put()))) continue;
            m_overviewMaskWidth=width;m_overviewMaskHeight=height;m_overviewMaskDpi=m_dpiScale;
        }
        m_brush->SetColor(D2D1::ColorF(0,0,0,opacity*.3f));
        const D2D1_RECT_F shadow{card.left-3*m_dpiScale,card.top+4*m_dpiScale,card.right+3*m_dpiScale,card.bottom+9*m_dpiScale};
        m_ctx->FillRoundedRectangle({shadow,14*m_dpiScale,14*m_dpiScale},m_brush.get());
        auto layer = D2D1::LayerParameters1();
        layer.geometricMask = m_overviewCardMask.get();
        layer.maskTransform = D2D1::Matrix3x2F::Translation(card.left, card.top);
        layer.opacity = desktopOpacity;
        m_ctx->PushLayer(layer,nullptr);
        m_brush->SetColor(m_settings.backgroundColor);m_ctx->FillRectangle(card,m_brush.get());
        if(!DrawLiveWallpaper(card,1) && desktop.wallpaper) m_ctx->DrawBitmap(desktop.wallpaper.get(),card,1,D2D1_INTERPOLATION_MODE_LINEAR);
        // Every HWND gets its own tile. Windows from the same application are
        // deliberately not stacked or grouped in the desktop overview.
        std::vector<OverviewWindow*> cardWindows;
        for(auto& window:m_overviewWindows) {
            if((IsEqualGUID(window.desktop,desktop.id) || IsEqualGUID(window.desktop,GUID_NULL)) &&
                window.alive && !window.minimized) cardWindows.push_back(&window);
        }
        std::reverse(cardWindows.begin(),cardWindows.end());
        const int count=(int)cardWindows.size();
        for(int tileIndex=0;tileIndex<count;++tileIndex) {
            auto& window=*cardWindows[tileIndex];DWORD pid=0;GetWindowThreadProcessId(window.hwnd,&pid);
            GetWindowBounds(window.hwnd,false,&window.rect);
            const float sourceWidth=std::max(1L,window.rect.right-window.rect.left);
            const float sourceHeight=std::max(1L,window.rect.bottom-window.rect.top);
            const float tileWidth=sourceWidth*width/worldWidth,tileHeight=sourceHeight*height/worldHeight;
            D2D1_RECT_F destination{card.left+(window.rect.left-m_overviewWorld.left)*width/worldWidth,
                card.top+(window.rect.top-m_overviewWorld.top)*height/worldHeight,
                card.left+(window.rect.right-m_overviewWorld.left)*width/worldWidth,
                card.top+(window.rect.bottom-m_overviewWorld.top)*height/worldHeight};
            const bool windowHovered=hasCursor && InRect(pointer,destination);
            window.overviewHover+=(float)(((windowHovered ? 1.f : 0.f)-window.overviewHover)*(1-std::exp(-18*m_motionDt)));
            if(window.overviewHover>.001f) {
                const float grow=1.f+.025f*window.overviewHover;
                const float cx=(destination.left+destination.right)*.5f;
                const float cy=(destination.top+destination.bottom)*.5f-10*m_dpiScale*window.overviewHover;
                const float halfWidth=(destination.right-destination.left)*grow*.5f;
                const float halfHeight=(destination.bottom-destination.top)*grow*.5f;
                destination={cx-halfWidth,cy-halfHeight,cx+halfWidth,cy+halfHeight};
            }
            if(index==m_overviewSelected) {
                window.hit={std::max(card.left,destination.left),std::max(card.top,destination.top),
                    std::min(card.right,destination.right),std::min(card.bottom,destination.bottom)};
                if(window.hit.right>window.hit.left && window.hit.bottom>window.hit.top) window.hitDesktop=index;
            }
            m_brush->SetColor(D2D1::ColorF(0,0,0,.30f));
            m_ctx->FillRoundedRectangle({{destination.left-2*m_dpiScale,destination.top+2*m_dpiScale,
                destination.right+2*m_dpiScale,destination.bottom+5*m_dpiScale},5*m_dpiScale,5*m_dpiScale},m_brush.get());
            ID2D1Bitmap1* bitmap=nullptr;
            for(const auto& item:m_items) if(item->hwnd==window.hwnd && item->pid==window.pid) {bitmap=item->content.get();break;}
            if(!bitmap && window.capture) bitmap=window.capture->content.get();
            if(!bitmap) {
                const auto saved=m_windowSnapshots.find(window.hwnd);
                if(saved!=m_windowSnapshots.end() && saved->second.pid==pid && saved->second.title==window.title) bitmap=saved->second.bitmap.get();
            }
            if(bitmap) m_ctx->DrawBitmap(bitmap,destination,1,D2D1_INTERPOLATION_MODE_LINEAR);
            else {
                m_brush->SetColor(m_settings.backgroundColor);m_ctx->FillRoundedRectangle({destination,4*m_dpiScale,4*m_dpiScale},m_brush.get());
                if(window.icon) {const float icon=std::min(28*m_dpiScale,std::min(tileWidth,tileHeight)*.45f);
                    m_ctx->DrawBitmap(window.icon.get(),{(destination.left+destination.right-icon)*.5f,
                        (destination.top+destination.bottom-icon)*.5f,(destination.left+destination.right+icon)*.5f,
                        (destination.top+destination.bottom+icon)*.5f},1,D2D1_INTERPOLATION_MODE_LINEAR);}
            }
            if(window.overviewHover>.005f) {
                const float glow=opacity*window.overviewHover;
                for(int halo=3;halo>=1;--halo) {
                    const float spread=halo*2*m_dpiScale;
                    m_brush->SetColor(StyleColor(m_settings.highlightColor,glow*(.12f/(float)halo)));
                    m_ctx->DrawRoundedRectangle({{destination.left-spread,destination.top-spread,
                        destination.right+spread,destination.bottom+spread},(4+halo)*m_dpiScale,(4+halo)*m_dpiScale},
                        m_brush.get(),(4-halo)*m_dpiScale);
                }
            }
            m_brush->SetColor(window.hwnd==m_overviewSelectedWindow || window.overviewHover>.15f
                ? StyleColor(m_settings.highlightColor,opacity) : StyleColor(m_settings.borderColor,.45f));
            m_ctx->DrawRoundedRectangle({destination,4*m_dpiScale,4*m_dpiScale},m_brush.get(),
                window.hwnd==m_overviewSelectedWindow || window.overviewHover>.15f ? 2*m_dpiScale : m_dpiScale);
        }
        const bool nativeTaskbar=IsEqualGUID(desktop.id,m_overviewCurrentDesktop) && m_overviewTaskbar && m_overviewTaskbar->content;
        if(nativeTaskbar) {
            const RECT& rect=m_overviewTaskbar->rect;
            m_ctx->DrawBitmap(m_overviewTaskbar->content.get(),{card.left+(rect.left-m_overviewWorld.left)*width/worldWidth,
                card.top+(rect.top-m_overviewWorld.top)*height/worldHeight,card.left+(rect.right-m_overviewWorld.left)*width/worldWidth,
                card.top+(rect.bottom-m_overviewWorld.top)*height/worldHeight},1,D2D1_INTERPOLATION_MODE_LINEAR);
        } else if(IsEqualGUID(desktop.id,m_overviewCurrentDesktop)) {
            const float barHeight=std::max(22*m_dpiScale,height*.047f),icon=barHeight*.52f,gap=icon*.45f;
            const D2D1_RECT_F bar{card.left,card.bottom-barHeight,card.right,card.bottom};
            m_brush->SetColor(m_settings.darkBackground ? D2D1::ColorF(.10f,.10f,.12f,.97f) : D2D1::ColorF(.94f,.94f,.96f,.97f));
            m_ctx->FillRectangle(bar,m_brush.get());
            std::vector<OverviewWindow*> apps;
            for(auto& window:m_overviewWindows) if(IsWindow(window.hwnd) &&
                (IsEqualGUID(window.desktop,desktop.id) || IsEqualGUID(window.desktop,GUID_NULL))) apps.push_back(&window);
            const int count=std::min((int)apps.size(),std::max(0,(int)((width-60*m_dpiScale)/(icon+gap))-1));
            float x=(card.left+card.right-(count+1)*(icon+gap))*.5f;
            const float y=bar.top+(barHeight-icon)*.5f;
            m_brush->SetColor(m_settings.highlightColor);
            const float square=(icon-2*m_dpiScale)*.5f;
            for(int row=0;row<2;++row) for(int column=0;column<2;++column)
                m_ctx->FillRectangle({x+column*(square+2*m_dpiScale),y+row*(square+2*m_dpiScale),x+column*(square+2*m_dpiScale)+square,y+row*(square+2*m_dpiScale)+square},m_brush.get());
            x+=icon+gap;
            for(int app=0;app<count;++app) {
                if(apps[app]->icon) m_ctx->DrawBitmap(apps[app]->icon.get(),{x,y,x+icon,y+icon},1,D2D1_INTERPOLATION_MODE_LINEAR);
                x+=icon+gap;
            }
        }
        m_ctx->PopLayer();
        if(previousDesktop) continue;
        {
            const bool editing=g_desktopRenameTyping && IsEqualGUID(desktop.id,m_overviewRenameId);
            const float buttonWidth=std::min(360*m_dpiScale,m_width*.42f),buttonHeight=44*m_dpiScale;
            const D2D1_RECT_F nameButton{(m_width-buttonWidth)*.5f,m_height-62*m_dpiScale,
                (m_width+buttonWidth)*.5f,m_height-18*m_dpiScale};
            const D2D1_RECT_F deleteButton{20*m_dpiScale,m_height-62*m_dpiScale,
                64*m_dpiScale,m_height-18*m_dpiScale};
            const bool nameHovered=hasCursor && InRect(pointer,nameButton);
            desktop.hoverAmount+=(float)(((editing || nameHovered ? 1.f : 0.f)-desktop.hoverAmount)*(1-std::exp(-20*m_motionDt)));
            desktop.rename=nameButton;
            desktop.remove=index>0 ? deleteButton : D2D1_RECT_F{};
            m_overviewRenameBox=editing ? nameButton : D2D1_RECT_F{};
            DrawUiPanel(nameButton,buttonHeight*.5f,opacity);
            if(index>0) {
                DrawUiPanel(deleteButton,buttonHeight*.5f,opacity);
                m_brush->SetColor(D2D1::ColorF(.95f,.22f,.25f,opacity));
                m_ctx->DrawText(L"\uE74D",1,m_overviewSymbolFormat.get(),deleteButton,m_brush.get());
            }
            if(editing) {
                m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity*.14f));
                m_ctx->FillRoundedRectangle({nameButton,buttonHeight*.5f,buttonHeight*.5f},m_brush.get());
                m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity));
                m_ctx->DrawRoundedRectangle({nameButton,buttonHeight*.5f,buttonHeight*.5f},m_brush.get(),2*m_dpiScale);
                DrawAnimatedUiText(m_renameTextAnimation,m_overviewRenameText,m_overviewTitleFormat.get(),
                    nameButton,opacity,m_overviewRenameSelectAll,true,false);
            } else {
                m_brush->SetColor(UiForeground(opacity));
                m_ctx->DrawText(desktop.name.c_str(),(UINT32)desktop.name.size(),m_overviewTitleFormat.get(),
                    {nameButton.left+12*m_dpiScale,nameButton.top,nameButton.right-42*m_dpiScale,nameButton.bottom},
                    m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
                if(nameHovered) {
                    m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity));
                    m_ctx->DrawText(L"\uE70F",1,m_overviewSymbolFormat.get(),
                        {nameButton.right-38*m_dpiScale,nameButton.top,nameButton.right-8*m_dpiScale,nameButton.bottom},m_brush.get());
                }
            }
            continue;
        }
    }
    m_ctx->PopAxisAlignedClip();

    for(auto& window:m_overviewWindows) if(!window.dockTarget && window.dockAmount>0.001f) {
        window.dockAmount*=1-dockBlend;
        if(window.alive && window.icon) {
            const auto r=window.dockVisual;
            const float size=34*m_dpiScale*(0.94f+0.06f*window.dockAmount);
            const float x=(r.left+r.right-size)*0.5f-8*m_dpiScale*(1-window.dockAmount);
            const float y=(r.top+r.bottom-size)*0.5f;
            m_ctx->DrawBitmap(window.icon.get(),{x,y,x+size,y+size},opacity*window.dockAmount,D2D1_INTERPOLATION_MODE_LINEAR);
        }
    }
    if(m_overviewActionErrorUntil>NowSeconds()) {
        const auto error=Text(L"The desktop operation could not be completed",L"Impossibile completare l'operazione sul desktop");
        m_brush->SetColor(D2D1::ColorF(1,.5f,.5f,opacity));
        m_ctx->DrawText(error,(UINT32)wcslen(error),m_overviewFormat.get(),{0,m_height*.91f,m_width,m_height*.97f},m_brush.get());
    }
    if(m_overviewDragging) {
        const int desktops=(int)m_overviewDesktops.size();
        const float gap=6*m_dpiScale,chip=std::min(150*m_dpiScale,(m_width-40*m_dpiScale)/std::max(1,std::min(desktops,7))-gap);
        const int visible=std::min(desktops,7),start=std::clamp(m_overviewSelected-visible/2,0,std::max(0,desktops-visible));
        const float left=(m_width-visible*(chip+gap)+gap)*.5f;
        m_overviewDropTargets.resize(desktops);
        for(int i=0;i<visible;++i) {
            const int index=start+i;
            const D2D1_RECT_F rect{left+i*(chip+gap),m_height-48*m_dpiScale,left+i*(chip+gap)+chip,m_height-12*m_dpiScale};
            m_overviewDropTargets[index]=rect;DrawUiPanel(rect,8*m_dpiScale,opacity);
            m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity*(InRect(m_overviewDragPoint,rect) ? .45f : .15f)));
            m_ctx->FillRoundedRectangle({rect,8*m_dpiScale,8*m_dpiScale},m_brush.get());
            m_brush->SetColor(UiForeground(opacity));
            const auto& name=m_overviewDesktops[index].name;
            m_ctx->DrawText(name.c_str(),(UINT32)name.size(),m_overviewButtonFormat.get(),rect,m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        for(const auto& desktop:m_overviewDesktops) if(InRect(m_overviewDragPoint,desktop.hit)) {
            m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity*.25f));
            m_ctx->FillRoundedRectangle({desktop.hit,8*m_dpiScale,8*m_dpiScale},m_brush.get());break;
        }
        for(const auto& window:m_overviewWindows) if(window.hwnd==m_overviewPress && window.pid==m_overviewPressPid) {
            const float w=std::min(220*m_dpiScale,m_width*.22f),h=w*std::max(1L,window.rect.bottom-window.rect.top)/std::max(1L,window.rect.right-window.rect.left);
            const D2D1_RECT_F ghost{m_overviewDragPoint.x-w*.5f,m_overviewDragPoint.y-24*m_dpiScale,m_overviewDragPoint.x+w*.5f,m_overviewDragPoint.y-24*m_dpiScale+h};
            m_brush->SetColor(D2D1::ColorF(.12f,.12f,.14f,opacity*.9f));
            m_ctx->FillRoundedRectangle({ghost,8*m_dpiScale,8*m_dpiScale},m_brush.get());
            ID2D1Bitmap1* bitmap=window.capture ? window.capture->content.get() : nullptr;
            if(!bitmap) for(const auto& item:m_items) if(item->hwnd==window.hwnd && item->pid==window.pid) {bitmap=item->content.get();break;}
            if(bitmap) m_ctx->DrawBitmap(bitmap,ghost,opacity*.85f,D2D1_INTERPOLATION_MODE_LINEAR);
            else if(window.icon) m_ctx->DrawBitmap(window.icon.get(),{ghost.left+12*m_dpiScale,ghost.top+12*m_dpiScale,ghost.left+44*m_dpiScale,ghost.top+44*m_dpiScale},opacity,D2D1_INTERPOLATION_MODE_LINEAR);
            m_brush->SetColor(StyleColor(m_settings.highlightColor, opacity));
            m_ctx->DrawRoundedRectangle(
                {ghost, 8 * m_dpiScale, 8 * m_dpiScale},
                m_brush.get(), 2 * m_dpiScale);
            break;
        }
        hoveredTitle.clear();
    }
    if(!hoveredTitle.empty()) {m_overviewTooltip=hoveredTitle;m_overviewTooltipPoint=pointer;}
    m_overviewTooltipOpacity+=(float)(((!hoveredTitle.empty() ? 1.0f : 0.0f)-m_overviewTooltipOpacity)*(1-std::exp(-22*m_motionDt)));
    if(m_overviewTooltipOpacity>.01f && !m_overviewTooltip.empty()) {
        com_ptr<IDWriteTextLayout> text;
        if(SUCCEEDED(m_dwriteFactory->CreateTextLayout(m_overviewTooltip.c_str(),(UINT32)m_overviewTooltip.size(),m_overviewFormat.get(),
            std::min(360*m_dpiScale,m_width-40*m_dpiScale),30*m_dpiScale,text.put()))) {
            DWRITE_TEXT_METRICS metrics{};text->GetMetrics(&metrics);
            const float tipWidth=std::min(metrics.width+24*m_dpiScale,m_width-20*m_dpiScale),tipHeight=metrics.height+16*m_dpiScale;
            const float x=std::clamp(m_overviewTooltipPoint.x-tipWidth*.5f,10*m_dpiScale,m_width-tipWidth-10*m_dpiScale);
            const float y=std::max(4*m_dpiScale,m_overviewTooltipPoint.y-tipHeight-20*m_dpiScale);
            const float alpha=opacity*m_overviewTooltipOpacity;
            m_brush->SetColor(D2D1::ColorF(.12f,.12f,.14f,alpha));
            m_ctx->FillRoundedRectangle({{x,y,x+tipWidth,y+tipHeight},8*m_dpiScale,8*m_dpiScale},m_brush.get());
            m_brush->SetColor(D2D1::ColorF(1,1,1,alpha));
            m_ctx->DrawText(m_overviewTooltip.c_str(),(UINT32)m_overviewTooltip.size(),m_overviewFormat.get(),
                {x+8*m_dpiScale,y+8*m_dpiScale,x+tipWidth-8*m_dpiScale,y+tipHeight},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
    }
    DrawDesktopRemoval(opacity);
}

void Switcher::ConfigureTouchpad() {
    RAWINPUTDEVICE device{0x0D,0x05,(DWORD)(m_settings.touchpadGestures ? RIDEV_INPUTSINK|RIDEV_DEVNOTIFY : RIDEV_REMOVE),
        m_settings.touchpadGestures ? m_hwnd : nullptr};
    if(!RegisterRawInputDevices(&device,1,sizeof(device))) Wh_Log(L"Touchpad registration failed: %u",GetLastError());
    if(!m_settings.touchpadGestures) {m_touchStroke={};SetTouchpadOwnership(false);}
}

void Switcher::SetTouchpadOwnership(bool enabled) {
    if(enabled && !m_touchController && !m_touchControllerTried) {
        m_touchControllerTried=true;
        try {
            auto name=winrt::hstring(L"Windows.UI.Input.TouchpadGesturesController");
            com_ptr<CoverTouchStatics> statics;
            const HRESULT hr=RoGetActivationFactory(reinterpret_cast<HSTRING>(winrt::get_abi(name)),kTouchStaticsId,statics.put_void());
            bool supported=false;
            if(SUCCEEDED(hr) && SUCCEEDED(statics->IsSupported(&supported)) && supported) {
                com_ptr<IInspectable> controller;
                if(SUCCEEDED(statics->CreateForProcess(controller.put()))) {
                    const GUID id{0x28c13cdd,0xe068,0x549f,{0x89,0xc6,0x1a,0x44,0x0c,0x6f,0xc3,0x27}};
                    if(SUCCEEDED(controller->QueryInterface(id,m_touchController.put_void())) &&
                        FAILED(m_touchController->put_SupportedGestures(0x1|0x8))) m_touchController=nullptr;
                }
            }
        } catch(...) {}
        if(!m_touchController) Wh_Log(L"Native touchpad takeover unavailable; disable conflicting Windows three-finger actions if necessary.");
    }
    if(m_touchController) m_touchController->put_Enabled(enabled);
}

void Switcher::OnTouchpad(ULONG fingers,double x,double y) {
    if(!m_settings.touchpadGestures) return;
    if(m_dragMoving || m_pressedWindow) {if(!fingers) m_touchStroke={};return;}
    const auto action=m_touchStroke.Update(fingers,x,y,NowSeconds(),m_state==State::Open || m_state==State::Pending);
    if(action!=TouchAction::None && action!=TouchAction::Commit) m_touchTapCount=0;
    if(action==TouchAction::Open && g_ready && m_state==State::Idle) {
        OnStart(kStartFromHotkey);
        if(m_state==State::Pending) {
            m_touchSession=true;g_switching=true;g_stickyControls=true;
            SetTouchpadOwnership(true);
            PostThreadMessageW(g_hookThreadId,WM_HOOK_ENGAGE,0,0);
        }
    } else if(action==TouchAction::Next || action==TouchAction::Previous) {
        if(m_state==State::Open || m_state==State::Pending) {
            m_touchSession=true;g_stickyControls=true;
            if(!m_hasFocus) TakeFocus();
            SetTouchpadOwnership(true);
            OnStep((action==TouchAction::Next ? 1 : -1)*(m_desktopOverview ? 3 : 1));
        }
    } else if(action==TouchAction::Dismiss) {
        EndSwitching();OnCancel();
    } else if(action==TouchAction::Commit) {
        ++m_touchTapCount;m_touchTapDeadline=NowSeconds()+0.34;
        if(m_touchTapCount==3) {m_touchTapCount=0;ToggleDesktopOverview();}
    } else if(action==TouchAction::Close) {
        if(m_desktopOverview) return;
        HWND target=GetForegroundWindow();
        if(m_state==State::Open && SelectedIndex()>=0) {
            auto& item=*m_items[SelectedIndex()];target=item.hwnd;
            item.closeRequested=true;item.closeRequestedAt=NowSeconds();
        } else return;
        wchar_t name[128]{};GetClassNameW(target,name,128);
        if(target && target!=m_hwnd && IsWindowVisible(target) &&
            wcscmp(name,L"Progman") && wcscmp(name,L"WorkerW") && wcscmp(name,L"Shell_TrayWnd"))
            PostMessageW(target,WM_CLOSE,0,0);
    }
}

void Switcher::OnStart(WPARAM flags) {
    g_lastNavigationInputTick=GetTickCount64();
    DismissIconContext();
    EndWindowDrag();
    if (!g_ready) {
        EndSwitching();
        return;
    }
    const bool backwards = (flags & kStartBackwards) != 0;
    const bool fromHotkey = (flags & kStartFromHotkey) != 0;

    // Reverse an unfinished close in-place. Keeping the current captures and
    // overlay avoids the desktop flash visible during very fast Alt+Tab use.
    if (m_state == State::Closing && !m_desktopOverview && !m_dragMoving &&
        !m_items.empty()) {
        ReopenDuringClose(backwards);
        return;
    } else if (m_state == State::Closing) {
        FinishClose();
    } else if (m_state != State::Idle) {
        HideOverlay();
        Teardown();
        m_state = State::Idle;
    }

    m_holdWindow=nullptr;m_holdSince=0;m_holdAmount=0;m_holdFullAt=0;m_holdExpanded=false;m_holdOffset={};m_holdExitBitmap=nullptr;
    m_holdMessageFormat=nullptr;
    // Refresh the OS theme before creating this session's placeholder cards.
    m_tileBorderIndex=-1;
    auto refreshedSettings=LoadSettings();
    if (refreshedSettings.darkBackground!=m_settings.darkBackground)
        m_backgroundKey.clear();
    m_settings=std::move(refreshedSettings);
    g_simpleNavigation=m_settings.simpleStyle>0 && m_settings.simpleStyle<4;
    m_searchTextAnimation={};m_searchFormat=nullptr;m_countFormat=nullptr;m_countSuffixLayout=nullptr;m_countSuffixText.clear();m_countFound=-1;
    m_enchantedIndex=-1;m_resultPanelWidth=0;m_resultPanelHeight=0;m_enchantedHover.clear();m_simplePinRects.clear();m_enchantedSymbols=nullptr;
    m_positionMotion={};m_positionFormat=nullptr;m_positionTextCount=-1;m_positionLabel.clear();
    m_simpleShortcutNumber=0; m_simpleShortcutTime=0;
    if(!m_settings.previewCache) m_windowSnapshots.clear();
    m_cursorTitleOpacity=0; m_cursorTitle.clear(); m_hoverTitleLayout=nullptr; m_hoverTitleFormat=nullptr;
    m_statsBottom=0; m_killErrorUntil=0; m_killConfirmWindow=nullptr; m_killConfirmUntil=0;
    m_hoverWindow=nullptr; m_hoverCandidate=nullptr;
    m_hoverSelectionArmed=true; m_lastHoverCursor={-10000,-10000};
    m_closeButton={}; m_killButton={}; m_pinButton={}; m_iconRects.clear();
    m_searchFrom.clear();
    m_searchTransitionStart = 0;
    m_searchBarWidth = 0;
    m_searchBarTime = 0;
    m_searchActive = false;
    m_searchSelectAll = false;
    g_searchTyping = false;
    g_searchHasResults = true;
    m_searchQuery.clear();
    m_searchMatches.clear();
    m_searchMask.clear();
    m_searchMeasuredText.clear();
    m_searchMeasuredWidth = 0;
    m_simpleIndicator=-1; m_simpleRaised=nullptr; m_simpleFormat=nullptr; m_simpleDockHover.clear();
    m_previousForeground = GetForegroundWindow();

    HMONITOR monitor;
    POINT cursor;
    if (m_settings.monitor == MonitorMode::Cursor && GetCursorPos(&cursor)) {
        monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY);
    } else {
        monitor =
            MonitorFromWindow(m_previousForeground, MONITOR_DEFAULTTOPRIMARY);
    }
    MONITORINFO info{};
    info.cbSize=sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        EndSwitching();
        return;
    }
    m_monitor = info.rcMonitor;
    MONITORINFOEXW display{};display.cbSize=sizeof(display);
    DEVMODEW mode{};mode.dmSize=sizeof(mode);
    if(GetMonitorInfoW(monitor,&display) && EnumDisplaySettingsW(display.szDevice,ENUM_CURRENT_SETTINGS,&mode))
        Wh_Log(L"CoverTabby VSync: %ls at %u Hz, no fixed FPS cap",display.szDevice,mode.dmDisplayFrequency);
    m_windowScale=1;
    if (m_settings.windowMode && m_settings.simpleStyle!=5) {
        const LONG marginX=(m_monitor.right-m_monitor.left)*(100-m_settings.popupWidthPercent)/200;
        const LONG marginY=(m_monitor.bottom-m_monitor.top)*(100-m_settings.popupHeightPercent)/200;
        InflateRect(&m_monitor,-marginX,-marginY);
        const float baseWidth=(float)(m_monitor.right-m_monitor.left),baseHeight=(float)(m_monitor.bottom-m_monitor.top);
        const float maxScale=std::min((info.rcMonitor.right-info.rcMonitor.left)*0.98f/baseWidth,
            (info.rcMonitor.bottom-info.rcMonitor.top)*0.98f/baseHeight);
        m_windowScale=std::min(m_settings.windowScale,maxScale);
        const LONG width=(LONG)std::round(baseWidth*m_windowScale),height=(LONG)std::round(baseHeight*m_windowScale);
        const LONG centerX=(info.rcMonitor.left+info.rcMonitor.right)/2,centerY=(info.rcMonitor.top+info.rcMonitor.bottom)/2;
        m_monitor={centerX-width/2,centerY-height/2,centerX-width/2+width,centerY-height/2+height};
    }

    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiY = 96;
    }
    if (m_dpiScale != dpiY / 96.0f) {
        m_dpiScale = dpiY / 96.0f;
        m_titleFormat = nullptr; m_overviewFormat=nullptr;m_overviewTitleFormat=nullptr;
        m_overviewButtonFormat=nullptr;m_overviewSymbolFormat=nullptr;
    }

    g_stickyControls = false;
    m_desktopMenuOpen=false; m_desktopMenuAmount=0; m_desktopScroll=0;
    m_desktopInfoWindow=nullptr; m_desktopRefreshAt=0; m_desktopErrorUntil=0;
    m_desktopButton={}; m_desktopRows.clear();
    CollectWindows();
    std::erase_if(m_pinned, [](const auto& p) { return !IsWindow(p.first); });
    std::stable_sort(m_items.begin(), m_items.end(), [&](const auto& a, const auto& b) {
        return m_pinned.contains(a->hwnd) && !m_pinned.contains(b->hwnd);
    });
    if (m_settings.groupPrograms) {
        // Preserve first-seen application order (including pinned priority),
        // but keep every application's windows in one uninterrupted block.
        std::unordered_map<std::wstring,int> order;
        for (const auto& item:m_items) {
            if (!order.contains(item->appKey)) order.emplace(item->appKey,(int)order.size());
        }
        std::stable_sort(m_items.begin(),m_items.end(),[&](const auto& a,const auto& b) {
            return order.at(a->appKey)<order.at(b->appKey);
        });
    }
    if(m_items.empty() && m_openingOverview) {
        auto placeholder=std::make_unique<Item>();
        placeholder->desktopPlaceholder=true;placeholder->minimized=true;
        placeholder->rect=m_monitor;placeholder->title=Text(L"Desktops",L"Desktop");
        placeholder->placeholder=CreatePlaceholder(*placeholder);
        if(placeholder->placeholder) m_items.push_back(std::move(placeholder));
    }
    if (m_items.empty()) {
        // Nothing to switch to. The rest of the keys go to the apps again.
        EndSwitching();
        return;
    }

    const int count = (int)m_items.size();
    // Pin ordering can move the foreground window away from index zero.
    // Start relative to the actual active window, including quick Alt+Tab.
    int foregroundIndex = -1;
    for (int i = 0; i < count; ++i) {
        if (m_items[i]->hwnd == m_previousForeground) { foregroundIndex = i; break; }
    }
    m_target = foregroundIndex >= 0
        ? PositiveMod(foregroundIndex + (backwards ? -1 : 1), count)
        : (backwards ? count - 1 : 0);
    m_snapshotNext=(size_t)m_target;
    m_scroll = (double)m_target;
    m_iconScroll = (double)m_target;
    UpdateAppGroups(true);
    m_velocity = 0;
    m_wheelRemainder = 0;
    m_altReleasedAt = 0;
    m_titleIndex = -1;
    m_commit = false;
    m_selectedOnTopWhenFlat = false;

    ComputeLayout();

    m_showAt = NowSeconds() + m_settings.showDelayMs / 1000.0;
    m_capturing = false;
    m_state = State::Pending;

    if(m_settings.simpleStyle==5 && !m_openingOverview) {
        const int target=m_target;UpdateSearch();m_target=target;m_scroll=target;m_iconScroll=target;
    }
    if(m_settings.simpleStyle==4 && !m_openingOverview) {
        m_searchActive=true;g_searchTyping=true;g_stickyControls=true;UpdateSearch();
    }
    if (fromHotkey || (m_settings.simpleStyle==4 && !m_openingOverview)) {
        TakeFocus();
    }
}

// Used when the keyboard hook can't see the keys (e.g. an elevated window has
// the focus). Like the native switcher, the overlay then takes the keyboard
// focus, so the rest of the session goes through the hook again. It stays
// fully transparent until the stack is shown.
void Switcher::TakeFocus() {
    const UINT width = (UINT)(m_monitor.right - m_monitor.left);
    const UINT height = (UINT)(m_monitor.bottom - m_monitor.top);
    if (!EnsureSwapChain(width, height)) {
        return;
    }

    m_ctx->SetTarget(m_targetBitmap.get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));

    HRESULT hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    if (SUCCEEDED(hr)) {
        hr = m_swapChain->Present(1, 0);
    }
    if (FAILED(hr)) {
        return;
    }

    SetWindowLongPtrW(m_hwnd, GWL_EXSTYLE,
                      GetWindowLongPtrW(m_hwnd, GWL_EXSTYLE) &
                          ~WS_EX_NOACTIVATE);
    SetWindowPos(m_hwnd, HWND_TOPMOST, m_monitor.left, m_monitor.top, width,
                 height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    // Receiving the hotkey allows this process to change the foreground.
    m_hasFocus = SetForegroundWindow(m_hwnd) != FALSE;
    if (!m_hasFocus) {
        Wh_Log(L"Couldn't take the focus");
    }
}

void Switcher::HideOverlay() {
    m_processMonitor.Select(0);
    ShowWindow(m_hwnd, SW_HIDE);
    SetWindowLongPtrW(m_hwnd, GWL_EXSTYLE,
                      GetWindowLongPtrW(m_hwnd, GWL_EXSTYLE) |
                          WS_EX_NOACTIVATE);
    m_hasFocus = false;
}

// Fallback for a missed Alt release (e.g. a secure desktop took the input, or
// the keys never reached the hook). The hook normally reports it first, so
// it only counts once Alt stays released for a moment.
bool Switcher::AltReleased(double now) {
    if (!g_switching || ((m_touchSession || m_desktopOverview || m_searchActive) && g_stickyControls) || IsKeyDown(VK_MENU)) {
        m_altReleasedAt = 0;
        return false;
    }
    if (!m_altReleasedAt) {
        m_altReleasedAt = now;
    }
    return now - m_altReleasedAt > 0.15;
}

void Switcher::OnStep(int step) {
    g_lastNavigationInputTick=GetTickCount64();
    DismissIconContext();
    if(g_desktopRenameTyping || m_overviewDeletePending) return;
    if(m_desktopOverview && !m_overviewDesktops.empty() &&
        (m_state==State::Open || m_state==State::Pending)) {
        // Route both bare Tab in a persistent overview and Win+Tab here.
        if(std::abs(step)==1) step=CoverTabbyNavigation::tabStep(m_overviewExpanded,step<0);
        if(std::abs(step)==3) {
            const int direction=step>0 ? 1 : -1;
            const int count=(int)m_overviewDesktops.size();
            const int previous=m_overviewSelected;
            if(count>=5) m_overviewSelected=std::clamp(m_overviewSelected+direction,0,count-1);
            else m_overviewSelected=PositiveMod(m_overviewSelected+direction,count);
            if(m_overviewSelected!=previous) {
                m_overviewPreviousDesktop=m_overviewDesktops[previous].id;
                m_overviewTransitionDirection=direction;
                m_overviewDesktopTransition=NowSeconds();
            }
            m_overviewSelectedWindow=nullptr;m_overviewSelectedPid=0;


        } else {
            std::vector<const OverviewWindow*> windows;
            for(const auto& window:m_overviewWindows) {
                DWORD pid=0;GetWindowThreadProcessId(window.hwnd,&pid);
                if(IsWindow(window.hwnd) && pid==window.pid &&
                    (IsEqualGUID(window.desktop,m_overviewDesktops[m_overviewSelected].id) || IsEqualGUID(window.desktop,GUID_NULL)))
                    windows.push_back(&window);
            }
            if(!windows.empty()) {
                int current=-1;
                for(int i=0;i<(int)windows.size();++i) if(windows[i]->hwnd==m_overviewSelectedWindow) current=i;
                const int next=current<0 ? (step>0 ? 0 : (int)windows.size()-1) : PositiveMod(current+(step>0 ? 1 : -1),(int)windows.size());
                m_overviewSelectedWindow=windows[next]->hwnd;m_overviewSelectedPid=windows[next]->pid;
            }
        }
        return;
    }
    if ((m_state != State::Pending && m_state != State::Open) ||
        m_items.size() < 2) {
        return;
    }
    if(m_settings.simpleStyle>=4 && !m_searchMatches.empty()) {
        if(m_searchMatches.empty()) return;
        const float searchWidth=std::min(std::max(470.f,(float)std::min(m_settings.simpleStyle==5 ? 6 : 3,std::max(1,(int)m_searchMatches.size()))*225.f)*m_dpiScale,m_width-40*m_dpiScale);
        const int columns=std::max(1,std::min(m_settings.simpleStyle==5 ? 6 : 3,(int)(searchWidth/(225*m_dpiScale))));
        auto current=std::find(m_searchMatches.begin(),m_searchMatches.end(),SelectedIndex());
        const int position=current==m_searchMatches.end() ? 0 : (int)(current-m_searchMatches.begin());
        const int delta=std::abs(step)==2 ? (step>0 ? columns : -columns) : (step>0 ? 1 : -1);
        m_target=m_searchMatches[PositiveMod(position+delta,(int)m_searchMatches.size())];return;
    }
    if(m_settings.simpleStyle>0 && m_settings.simpleStyle<4) {m_target+=step>0 ? 1 : -1;return;}
    if (m_settings.tileGrid) {
        const int count=(int)m_items.size(), columns=std::min(count<=6?3:4,count);
        const int delta=std::abs(step)==2 ? (step>0 ? columns : -columns) : (step>0 ? 1 : -1);
        m_target=PositiveMod((long long)SelectedIndex()+delta,count);
        return;
    }
    if (m_searchActive) {
        if(m_searchMatches.empty()) return;
        if (std::abs(step)>1 && m_settings.groupPrograms) {
            UpdateAppGroups();
            const int selected=SelectedIndex(), group=m_groupOf[selected];
            if (group<0) return;
            const int direction=step>0 ? 1 : -1;
            if (std::abs(step)==2) {
                const auto& members=m_appGroups[group];
                m_target=members[PositiveMod((long long)m_groupRank[selected]+direction,(int)members.size())];
            } else {
                const int nextGroup=PositiveMod((long long)group+direction,(int)m_appGroups.size());
                m_target=m_appGroups[nextGroup].front();
            }
            return;
        }
        auto it=std::find(m_searchMatches.begin(),m_searchMatches.end(),SelectedIndex());
        const int position=it==m_searchMatches.end() ? 0 : (int)(it-m_searchMatches.begin());
        const int next=PositiveMod((long long)position+(step>0 ? 1 : -1),(int)m_searchMatches.size());
        m_target=m_searchMatches[next]; return;
    }
    if (std::abs(step) > 1) {
        const int direction = step > 0 ? 1 : -1;
        if (m_settings.compactCoverFlow) {
            UpdateAppGroups();
            const int selected = SelectedIndex();
            const int group = m_groupOf[selected];
            int next = selected;
            if (std::abs(step) == 2) {
                const auto& members = m_appGroups[group];
                const int rank = PositiveMod((long long)m_groupRank[selected] + direction,
                                              (int)members.size());
                next = members[rank];
            } else {
                const int nextGroup = PositiveMod((long long)group + direction,
                                                   (int)m_appGroups.size());
                next = m_appGroups[nextGroup].front();
            }
            int delta = next - selected;
            const int count = (int)m_items.size();
            if (std::abs(step) == 3) {
                // Application arrows always continue in the requested direction.
                if (next != selected && direction > 0 && delta < 0) delta += count;
                if (next != selected && direction < 0 && delta > 0) delta -= count;
            } else {
                if (delta > count / 2) delta -= count;
                if (delta < -count / 2) delta += count;
            }
            m_target += delta;
            return;
        }
        step = direction;
    }
    m_target += step;
    if (m_state == State::Pending) {
        m_scroll = (double)m_target;
    }
}

void Switcher::OnCommit(int index) {

    if(m_overviewDeletePending) {RemoveOverviewDesktop();return;}
    if(g_desktopRenameTyping) {FinishDesktopRename(true);return;}
    if(m_desktopOverview) {
        if(m_overviewDesktops.empty() || (m_state!=State::Open && m_state!=State::Pending)) return;
        m_destinationDesktop=m_overviewDesktops[m_overviewSelected].id;
        m_commitDesktop=true;
        if(m_state==State::Pending) FinishClose();
        else BeginClose(false);
        return;
    }
    if (m_searchActive && m_searchMatches.empty()) { g_switching=true; PostThreadMessageW(g_hookThreadId,WM_HOOK_ENGAGE,0,0); return; }
    if (m_state == State::Idle || m_state == State::Closing) {
        return;
    }

    if (index >= 0 && !m_items.empty()) {
        // Bring the chosen item to the front the shortest way.
        const int count = (int)m_items.size();
        int delta = index - SelectedIndex();
        if (delta > count / 2) {
            delta -= count;
        } else if (delta < -count / 2) {
            delta += count;
        }
        m_target += delta;
    }

    if (m_state == State::Pending) {
        // Quick Alt+Tab: switch right away, without showing anything.
        const int selected = SelectedIndex();
        const bool showDesktop=selected>=0 && m_items[selected]->showDesktop;
        HWND target = selected >= 0 && !showDesktop ? m_items[selected]->hwnd : nullptr;
        if (target) {
            ActivateWindow(target);
        }
        EndSession();
        if(showDesktop) ToggleWindowsDesktop();
        return;
    }

    BeginClose(true);
}

void Switcher::OnCancel() {
    if(m_overviewDeletePending) {
        m_overviewDeletePending=false;g_switching=true;g_stickyControls=true;
        PostThreadMessageW(g_hookThreadId,WM_HOOK_ENGAGE,0,0);return;
    }
    if(g_desktopRenameTyping) {FinishDesktopRename(false);return;}
    g_searchTyping=false;
    if (m_state == State::Pending) {
        // If the overlay took the focus, give it back.
        if (m_hasFocus && m_previousForeground) {
            ActivateWindow(m_previousForeground);
        }
        EndSession();
    } else if (m_state == State::Open) {
        BeginClose(false);
    }
}

void Switcher::DismissIconContext() {
    m_iconMenuOpen=false;
    g_iconMenuActive=false;
}

void Switcher::OnIconContext(POINT pt) {
    if(m_desktopOverview && m_state==State::Open) {
        if(m_overviewDeletePending) return;
        const auto point=LayoutPoint(pt);
        const int i=HitOverviewDesktop(point);
        if(i>=0) {
            ExpandOverviewDesktop(i);
            g_desktopWinHoldSession=false;g_stickyControls=true;return;
        }
        if(m_overviewExpanded) m_overviewExpanded=false;
        return;
    }
    if(m_state!=State::Open || m_desktopOverview || m_pressedWindow) return;
    const auto point=LayoutPoint(pt);
    Item* target=nullptr;
    for(size_t i=0;i<m_iconRects.size() && i<m_items.size();++i) {
        const auto r=m_iconRects[i];
        if(r.right>r.left && InRect(point,r)) {target=m_items[i].get();break;}
    }
    if(!target && !m_settings.simpleStyle) for(auto it=m_drawList.rbegin();it!=m_drawList.rend();++it) {
        if(it->interactive && it->pose.opacity>.5f && PreviewContains(*it,point)) {
            target=it->item;break;
        }
    }
    if(!target || target->desktopPlaceholder || target->windowGone || target->closeRequested) {
        DismissIconContext();return;
    }
    m_desktopMenuOpen=false;
    m_iconMenuWindow=target->hwnd;m_iconMenuPid=target->pid;
    m_iconMenuCreation=target->processCreation;m_iconMenuTitle=target->title;
    m_iconMenuAmount=0;
    for(auto& value:m_iconMenuHover) value=0;
    const float ui=m_dpiScale,width=204*ui,height=148*ui,margin=32*ui;
    const float left=std::clamp(point.x,margin,std::max(margin,m_width-margin-width));
    const float preferred=point.y+height+margin>m_height ? point.y-height-8*ui : point.y+8*ui;
    const float top=std::clamp(preferred,margin,std::max(margin,m_height-margin-height));
    m_iconMenuRect={left,top,left+width,top+height};
    for(int i=0;i<3;++i) m_iconMenuRows[i]={left+6*ui,top+(36+34*i)*ui,left+width-6*ui,top+(68+34*i)*ui};
    m_iconMenuOpen=true;g_iconMenuActive=true;
    // Keep the current Alt session and selection; this menu does not focus
    // or activate the clicked application.
}

void Switcher::RunIconContextAction(int action) {
    if(!m_iconMenuOpen || action<0 || action>2) return;
    Item* target=nullptr;
    for(auto& item:m_items) if(item->hwnd==m_iconMenuWindow && item->pid==m_iconMenuPid &&
        item->processCreation==m_iconMenuCreation && !item->desktopPlaceholder) {target=item.get();break;}
    DWORD pid=0;
    if(!target || target->windowGone || !IsWindow(target->hwnd) ||
        !GetWindowThreadProcessId(target->hwnd,&pid) || pid!=m_iconMenuPid) return;
    if(action==1 && !ConfirmKill(*target)) return;
    DismissIconContext();
    if(action==2) {
        if(m_pinned.contains(target->hwnd)) m_pinned.erase(target->hwnd);
        else m_pinned[target->hwnd]=true;
    } else if(action==0) {
        if(PostMessageW(target->hwnd,WM_CLOSE,0,0)) {
            target->closeRequested=true;target->closeRequestedAt=NowSeconds();
        }
    } else {
        if(KillWindowProcess(target->hwnd,target->pid,target->processCreation)) {
            target->closeRequested=true;target->closeRequestedAt=NowSeconds();
        } else m_killErrorUntil=NowSeconds()+3;
    }
}

void Switcher::DrawIconContext(float opacity) {
    if(m_iconMenuOpen) {
        const bool present=std::any_of(m_items.begin(),m_items.end(),[&](const auto& item) {
            return item->hwnd==m_iconMenuWindow && item->pid==m_iconMenuPid &&
                item->processCreation==m_iconMenuCreation && !item->windowGone;
        });
        if(!present || m_state!=State::Open) DismissIconContext();
    }
    const float blend=m_settings.animateHover ? (float)(1-std::exp(-24*m_motionDt)) : 1;
    m_iconMenuAmount+=((m_iconMenuOpen ? 1.0f : 0.0f)-m_iconMenuAmount)*blend;
    const float alpha=opacity*m_iconMenuAmount;
    if(alpha<.003f) return;
    const float ui=m_dpiScale;
    if(!m_iconMenuText || !m_iconMenuSymbols || m_iconMenuDpi!=ui) {
        m_iconMenuText=nullptr;m_iconMenuSymbols=nullptr;m_iconMenuDpi=ui;
        if(FAILED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,
            13*ui,L"",m_iconMenuText.put())) ||
           FAILED(m_dwriteFactory->CreateTextFormat(CoverTabbyIconFont(),nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,
            14*ui,L"",m_iconMenuSymbols.put()))) return;
        m_iconMenuText->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        m_iconMenuText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        m_iconMenuSymbols->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_iconMenuSymbols->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        com_ptr<IDWriteInlineObject> ellipsis;
        if(SUCCEEDED(m_dwriteFactory->CreateEllipsisTrimmingSign(m_iconMenuText.get(),ellipsis.put()))) {
            const DWRITE_TRIMMING trim{DWRITE_TRIMMING_GRANULARITY_CHARACTER,0,0};
            m_iconMenuText->SetTrimming(&trim,ellipsis.get());
        }
    }
    POINT cursor{};
    D2D1_POINT_2F point{-1,-1};
    if(GetCursorPos(&cursor) && ScreenToClient(m_hwnd,&cursor)) point=LayoutPoint(cursor);
    const auto panel=m_iconMenuRect;
    D2D1_MATRIX_3X2_F previous;
    m_ctx->GetTransform(&previous);
    m_ctx->SetTransform(D2D1::Matrix3x2F::Translation(0,4*ui*(1-m_iconMenuAmount))*
        D2D1::Matrix3x2F(previous._11,previous._12,previous._21,previous._22,previous._31,previous._32));
    // A cheap drawn shadow and solid themed surface; no HWND menu or blur pass.
    m_brush->SetColor(D2D1::ColorF(0,0,0,.24f*alpha));
    m_ctx->FillRoundedRectangle({{panel.left-2*ui,panel.top+4*ui,panel.right+2*ui,panel.bottom+6*ui},10*ui,10*ui},m_brush.get());
    auto surface=m_settings.darkBackground ? D2D1::ColorF(.14f,.14f,.16f,alpha) : D2D1::ColorF(.98f,.98f,.99f,alpha);
    m_brush->SetColor(surface);m_ctx->FillRoundedRectangle({panel,9*ui,9*ui},m_brush.get());
    m_brush->SetColor(StyleColor(m_settings.borderColor,.4f*alpha));
    m_ctx->DrawRoundedRectangle({panel,9*ui,9*ui},m_brush.get(),ui);
    auto foreground=m_settings.darkBackground ? D2D1::ColorF(1,1,1,alpha) : D2D1::ColorF(.1f,.1f,.12f,alpha);
    m_brush->SetColor(StyleColor(foreground,.65f*alpha));
    m_ctx->DrawText(m_iconMenuTitle.c_str(),(UINT32)m_iconMenuTitle.size(),m_iconMenuText.get(),
        {panel.left+12*ui,panel.top+4*ui,panel.right-12*ui,panel.top+30*ui},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
    m_brush->SetColor(StyleColor(m_settings.borderColor,.25f*alpha));
    m_ctx->DrawLine({panel.left+8*ui,panel.top+32*ui},{panel.right-8*ui,panel.top+32*ui},m_brush.get(),ui);
    const bool pinned=m_pinned.contains(m_iconMenuWindow);
    const wchar_t* labels[3]={Text(L"Close",L"Chiudi"),(m_killConfirmWindow==m_iconMenuWindow && NowSeconds()<m_killConfirmUntil ? Text(L"Confirm kill",L"Conferma arresto") : Text(L"Kill",L"Termina")),pinned ? Text(L"Unpin",L"Sblocca") : Text(L"Pin",L"Fissa")};
    const wchar_t* glyphs[3]={L"\uE8BB",L"\uE74D",pinned ? L"\uE77A" : L"\uE718"};
    for(int row=0;row<3;++row) {
        const auto r=m_iconMenuRows[row];
        const bool over=m_iconMenuOpen && InRect(point,r);
        m_iconMenuHover[row]+=((over ? 1.f : 0.f)-m_iconMenuHover[row])*blend;
        const float hover=m_iconMenuHover[row];
        const auto accent=row<2 ? D2D1::ColorF(.91f,.12f,.16f,1) : m_settings.highlightColor;
        m_brush->SetColor(StyleColor(accent,alpha*(.20f*hover+(row==2 && pinned ? .10f : 0))));
        m_ctx->FillRoundedRectangle({r,5*ui,5*ui},m_brush.get());
        auto color=foreground;
        color.r=Lerp(color.r,accent.r,hover*.65f);color.g=Lerp(color.g,accent.g,hover*.65f);color.b=Lerp(color.b,accent.b,hover*.65f);
        m_brush->SetColor(color);
        m_ctx->DrawText(glyphs[row],1,m_iconMenuSymbols.get(),{r.left+4*ui,r.top,r.left+32*ui,r.bottom},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
        m_ctx->DrawText(labels[row],(UINT32)wcslen(labels[row]),m_iconMenuText.get(),{r.left+38*ui,r.top,r.right-8*ui,r.bottom},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }
    m_ctx->SetTransform(previous);
}

void Switcher::OnClick(POINT pt) {
    if (m_state != State::Open) {
        return;
    }
    const D2D1_POINT_2F point=LayoutPoint(pt);
    if(m_settings.simpleStyle>=4 && !m_desktopOverview && InRect(point,m_searchRect)) {
        m_searchActive=true;g_searchTyping=true;g_stickyControls=true;TakeFocus();UpdateSearch();return;
    }
    if(m_settings.simpleStyle && !m_desktopOverview && !m_iconMenuOpen) {
        for(int i=0;i<(int)m_simplePinRects.size() && i<(int)m_items.size();++i) {
            if(m_simplePinRects[i].right>m_simplePinRects[i].left && InRect(point,m_simplePinRects[i])) {
                if(m_pinned.contains(m_items[i]->hwnd)) m_pinned.erase(m_items[i]->hwnd);
                else m_pinned[m_items[i]->hwnd]=true;return;
            }
        }
        for(int i=0;i<(int)m_simpleCloseRects.size();++i) {
            const auto r=m_simpleCloseRects[i];
            if(r.right>r.left && (m_settings.simpleStyle==5 ? InRect(point,r) : InCloseCircle(point,r))) {
                PostMessageW(m_items[i]->hwnd,WM_CLOSE,0,0); return;
            }
        }
        for(int i=0;i<(int)m_iconRects.size();++i) if(InRect(point,m_iconRects[i])) {
            EndSwitching(); OnCommit(i); return;
        }
        return;
    }
    if(m_iconMenuOpen) {
        for(int row=0;row<3;++row) if(m_iconMenuAmount>.5f && InRect(point,m_iconMenuRows[row])) {
            RunIconContextAction(row);return;
        }
        DismissIconContext();return;
    }
    if(m_desktopOverview) {
        if(m_overviewDeletePending) {
            if(InRect(point,m_overviewDeleteAccept)) RemoveOverviewDesktop();
            else if(InRect(point,m_overviewDeleteCancel)) m_overviewDeletePending=false;
            return;
        }
        if(m_overviewExpanded && InRect(point,m_overviewBack)) {
            m_overviewExpanded=false;return;
        }
        if(g_desktopRenameTyping) {
            if(InRect(point,m_overviewRenameBox)) return;
            bool saveButton=false;
            for(const auto& desktop:m_overviewDesktops) if(IsEqualGUID(desktop.id,m_overviewRenameId) && InRect(point,desktop.rename)) saveButton=true;
            FinishDesktopRename(true);
            if(saveButton) return;
        }
        // The dock is painted above the cards, so it must win hit testing too.
        for(const auto& window:m_overviewWindows) if(window.hitDesktop>=0 && InRect(point,window.dockHit)) {
            m_overviewSelected=window.hitDesktop;m_overviewSelectedWindow=window.hwnd;m_overviewSelectedPid=window.pid;
            m_overviewPress=window.hwnd;m_overviewPressPid=window.pid;m_overviewPressPoint=pt;
            m_overviewDragging=false;m_overviewPressFromDock=true;g_desktopWinHoldSession=false;g_stickyControls=true;SetCapture(m_hwnd);return;
        }
        if(InRect(point,m_overviewAdd)) {
            GUID created{};const HRESULT hr=m_desktopBridge.Create(&created);
            if(SUCCEEDED(hr)) {m_overviewSelectedWindow=nullptr;m_overviewSelectedPid=0;RefreshOverviewDesktops(created);}
            else {m_overviewActionErrorUntil=NowSeconds()+3;Wh_Log(L"Desktop creation failed: 0x%08X",(UINT)hr);}
            return;
        }
        for(size_t i=0;i<m_overviewDesktops.size();++i) {
            auto& desktop=m_overviewDesktops[i];
            if(InRect(point,desktop.rename)) {
                ExpandOverviewDesktop((int)i);
                m_overviewRenameId=desktop.id;m_overviewRenameText=desktop.name;m_overviewRenameSelectAll=true;
                g_desktopRenameTyping=true;g_desktopWinHoldSession=false;g_stickyControls=true;if(!m_hasFocus) TakeFocus();return;
            }
            if(i>0 && InRect(point,desktop.remove)) {
                m_overviewDeleteId=desktop.id;m_overviewDeletePending=true;
                g_desktopWinHoldSession=false;g_stickyControls=true;
                return;
            }
        }
        for(const auto& window:m_overviewWindows) if(window.hitDesktop>=0 && InRect(point,window.hit)) {
            m_overviewSelected=window.hitDesktop;m_overviewSelectedWindow=window.hwnd;m_overviewSelectedPid=window.pid;
            m_overviewPress=window.hwnd;m_overviewPressPid=window.pid;m_overviewPressPoint=pt;
            m_overviewDragging=false;m_overviewPressFromDock=false;g_desktopWinHoldSession=false;g_stickyControls=true;SetCapture(m_hwnd);return;
        }
        const int desktopHit=HitOverviewDesktop(point);

        if(desktopHit>=0) {
            const int i=desktopHit;
            m_overviewSelected=(int)i;m_overviewSelectedWindow=nullptr;m_overviewSelectedPid=0;

            EndSwitching();OnCommit(-1);
        }
        return;
    }
    if (m_desktopMenuOpen) {
        for (const auto& row : m_desktopRows) {
            if (!InRect(point,row.second)) continue;
            const GUID destination=row.first;
            const HWND window=m_desktopMenuWindow;
            m_desktopMenuOpen=false;
            const HRESULT result=m_desktopBridge.Move(window,destination);
            if (FAILED(result)) {
                Wh_Log(L"Could not move window %p to desktop: 0x%08X",window,(UINT)result);
                m_desktopErrorUntil=NowSeconds()+3;
            }
            m_desktopRefreshAt=0;
            UpdateDesktopInfo(NowSeconds());
            return;
        }
        if (InRect(point,m_desktopMenuRect)) return;
        m_desktopMenuOpen=false;
        return;
    }
    if (InRect(point,m_desktopButton) && m_desktopKnown && !m_desktopBridge.entries.empty()) {
        if (SUCCEEDED(m_desktopBridge.Refresh())) {
            m_desktopMenuOpen=true;
            m_desktopMenuWindow=m_desktopInfoWindow;
            m_desktopScroll=0;
        }
        return;
    }
    if (m_settings.compactCoverFlow && m_settings.enableSearch && point.x>=m_searchRect.left && point.x<=m_searchRect.right &&
        point.y>=m_searchRect.top && point.y<=m_searchRect.bottom) {
        m_searchActive=true; g_searchTyping=true; g_stickyControls=true; UpdateSearch(); return;
    }
    if (m_settings.hoverControls && m_hoverWindow && InRect(point,m_killButton)) {
        for (auto& item:m_items) {
            if (item->hwnd!=m_hoverWindow) continue;
            if(!ConfirmKill(*item)) return;
            const bool killed=KillWindowProcess(item->hwnd,item->pid,item->processCreation);
            if (killed) {item->closeRequested=true;item->closeRequestedAt=NowSeconds();}
            else m_killErrorUntil=NowSeconds()+3;
            return;
        }
        return;
    }
    if (m_settings.compactCoverFlow && m_settings.hoverControls && m_hoverWindow &&
        InCloseCircle(point, m_closeButton)) {
        // Request normal app closure, preserving its save-confirmation behavior.
        for (auto& item : m_items) {
            if (item->hwnd == m_hoverWindow) {
                item->closeRequested = true;
                item->closeRequestedAt = NowSeconds();
                PostMessageW(m_hoverWindow, WM_CLOSE, 0, 0);
            }
        }
        return;
    }
    if (m_settings.compactCoverFlow && m_settings.hoverControls && m_hoverWindow &&
        point.x >= m_pinButton.left && point.x <= m_pinButton.right &&
        point.y >= m_pinButton.top && point.y <= m_pinButton.bottom) {
        if (m_pinned.contains(m_hoverWindow)) m_pinned.erase(m_hoverWindow);
        else m_pinned[m_hoverWindow] = true;
        return;
    }
    if (m_settings.compactCoverFlow) {
        for (size_t i = 0; i < m_iconRects.size(); ++i) {
            const auto& r = m_iconRects[i];
            if (r.right > r.left && point.x >= r.left && point.x < r.right &&
                point.y >= r.top && point.y < r.bottom) {
                EndSwitching(); OnCommit((int)i); return;
            }
        }
        if (point.y > (m_settings.tileGrid ? m_tileIconY-m_settings.iconSize*m_dpiScale*.5f-10*m_dpiScale : m_height * 0.77f)) return;
    }

    for (auto it = m_drawList.rbegin(); it != m_drawList.rend(); ++it) {
        if (!it->interactive || it->pose.opacity < 0.5f || !PreviewContains(*it, point)) {
            continue;
        }
        for (size_t i = 0; i < m_items.size(); i++) {
            if (m_items[i].get() == it->item) {
                // Preview clicks commit on release; moving while held hands
                // the real window to Windows' native move loop instead.
                m_pressedWindow = it->item->hwnd;
                m_pressCanDrag=!m_settings.simpleStyle && !m_settings.windowMode && !it->item->desktopPlaceholder &&
                    QuadContains(PreviewDragZone(it->quad,m_dpiScale),point);
                m_pressScreen = pt;
                ClientToScreen(m_hwnd, &m_pressScreen);
                m_pressWasSticky = g_stickyControls.exchange(true);
                SetWindowLongPtrW(m_hwnd, GWL_EXSTYLE,
                    GetWindowLongPtrW(m_hwnd, GWL_EXSTYLE) & ~WS_EX_NOACTIVATE);
                m_hasFocus = SetForegroundWindow(m_hwnd) != FALSE;
                SetCapture(m_hwnd);
                if (GetCapture()!=m_hwnd) {
                    ClearPointerPress();
                    EndSwitching();
                    OnCommit((int)i);
                    return;
                }
                if (m_pressCanDrag) PrepareWindowDrag();
                return;
            }
        }
    }
}

void Switcher::ClearPointerPress() {
    if(m_overviewPress) {
        m_overviewPress=nullptr;m_overviewPressPid=0;m_overviewDragging=false;m_overviewOutsideSince=0;m_overviewPressFromDock=false;
        if(GetCapture()==m_hwnd) ReleaseCapture();
    }
    if (!m_pressedWindow) return;
    m_pressedWindow = nullptr;
    m_pressCanDrag = false;
    g_stickyControls = m_pressWasSticky;
    if (GetCapture() == m_hwnd) ReleaseCapture();
}

void Switcher::OnPointerUp() {
    if(m_overviewPress) {
        const HWND target=m_overviewPress;const DWORD expected=m_overviewPressPid;
        const bool dragged=m_overviewDragging;
        POINT cursor{};int destination=-1;
        if(GetCursorPos(&cursor) && ScreenToClient(m_hwnd,&cursor)) {
            const auto point=LayoutPoint(cursor);
            destination=HitOverviewDesktop(point);
        }
        ClearPointerPress();DWORD pid=0;GetWindowThreadProcessId(target,&pid);
        if(m_state!=State::Open || !IsWindow(target) || pid!=expected) return;
        if(dragged) {
            if(destination>=0) {
                const GUID desktop=m_overviewDesktops[destination].id;
                const HRESULT hr=m_desktopBridge.Move(target,desktop);
                if(SUCCEEDED(hr)) {
                    for(auto& window:m_overviewWindows) if(window.hwnd==target && window.pid==pid) window.desktop=desktop;
                    m_overviewSelected=destination;
                } else {m_overviewActionErrorUntil=NowSeconds()+3;Wh_Log(L"Desktop drop failed: 0x%08X",(UINT)hr);}
            }
        } else {
            bool minimized=IsIconic(target);
            for(const auto& window:m_overviewWindows) if(window.hwnd==target && window.pid==expected) minimized|=window.minimized;
            if(minimized) RestoreOverviewWindow(target,expected);
            else {EndSwitching();OnCommit(-1);}
        }
        return;
    }
    if (m_dragMoving) { EndWindowDrag(); return; }
    const HWND target = m_pressedWindow;
    if (m_dragWindow && !m_dragMoving) {
        // A held peek updates the minimized card in place. It doesn't commit
        // a window switch unless the user clicks elsewhere on the preview.
        SaveHeldPreview(target);
        RestoreDragWindow();
        ClearPointerPress();
        for (auto& item:m_items)
            if (item->hwnd==target) { PollFrames(*item); break; }
        return;
    }
    ClearPointerPress();
    if (!target || m_state != State::Open) return;
    for (int i = 0; i < (int)m_items.size(); ++i) {
        if (m_items[i]->hwnd != target || m_items[i]->windowGone) continue;
        EndSwitching();
        OnCommit(i);
        return;
    }
}

void Switcher::OnPointerMove(POINT pt) {
    if(m_overviewPress && m_desktopOverview && m_state==State::Open) {
        if(!IsKeyDown(VK_LBUTTON)) {ClearPointerPress();return;}
        if(std::abs(pt.x-m_overviewPressPoint.x)>=GetSystemMetrics(SM_CXDRAG) || std::abs(pt.y-m_overviewPressPoint.y)>=GetSystemMetrics(SM_CYDRAG)) m_overviewDragging=true;
        m_overviewDragPoint=LayoutPoint(pt);
        bool nearDesktop=false;
        for(const auto& desktop:m_overviewDesktops) {
            const float bridge=18*m_dpiScale;
            const D2D1_RECT_F area{desktop.hit.left-bridge,desktop.hit.top-bridge,
                desktop.hit.right+bridge,desktop.hit.bottom+bridge};
            if(InRect(m_overviewDragPoint,area)) {nearDesktop=true;break;}
        }
        if(!m_overviewDragging || nearDesktop) m_overviewOutsideSince=0;
        else if(!m_overviewPressFromDock || (m_overviewOutsideSince && NowSeconds()-m_overviewOutsideSince>=.12)) {
            if(!BeginOverviewWindowDrag(pt)) m_overviewOutsideSince=NowSeconds();
        }
        else if(!m_overviewOutsideSince) m_overviewOutsideSince=NowSeconds();
        return;
    }
    if (!m_pressedWindow || !m_pressCanDrag || m_state != State::Open) return;
    if (!IsKeyDown(VK_LBUTTON)) { ClearPointerPress(); return; }
    ClientToScreen(m_hwnd, &pt);
    if (std::abs(pt.x - m_pressScreen.x) < GetSystemMetrics(SM_CXDRAG) &&
        std::abs(pt.y - m_pressScreen.y) < GetSystemMetrics(SM_CYDRAG)) return;
    if (!BeginWindowDrag(pt)) {
        RestoreDragWindow();
        ClearPointerPress();
    }
}

bool Switcher::BeginOverviewWindowDrag(POINT clientPoint) {
    const HWND target=m_overviewPress;const DWORD expected=m_overviewPressPid;
    DWORD pid=0;
    if(!target || !IsWindow(target) || !IsWindowEnabled(target) ||
        !GetWindowThreadProcessId(target,&pid) || pid!=expected || !IsKeyDown(VK_LBUTTON)) return false;

    OverviewWindow* overview=nullptr;
    for(auto& window:m_overviewWindows) if(window.hwnd==target && window.pid==pid) {overview=&window;break;}
    if(!overview) return false;

    WINDOWPLACEMENT original{};original.length=sizeof(original);RECT oldRect{};
    if(!GetWindowPlacement(target,&original) || !GetWindowRect(target,&oldRect)) return false;
    const bool restore=IsIconic(target) || IsZoomed(target);
    if(restore) {
        WINDOWPLACEMENT movable=original;movable.flags&=~WPF_RESTORETOMAXIMIZED;
        movable.showCmd=SW_SHOWNOACTIVATE;
        if(!SetWindowPlacement(target,&movable)) return false;
        DwmFlush();
        if(IsIconic(target) || IsZoomed(target)) {SetWindowPlacement(target,&original);return false;}
    }

    RECT outer{},visible{};
    if(!GetWindowRect(target,&outer)) {if(restore) SetWindowPlacement(target,&original);return false;}
    visible=outer;DwmGetWindowAttribute(target,DWMWA_EXTENDED_FRAME_BOUNDS,&visible,sizeof(visible));
    const LONG visibleWidth=std::max(1L,visible.right-visible.left);
    const LONG visibleHeight=std::max(1L,visible.bottom-visible.top);
    float ratio=.5f;
    const auto pressed=LayoutPoint(m_overviewPressPoint);
    const D2D1_RECT_F source=InRect(pressed,overview->hit) ? overview->hit : overview->dockHit;
    if(source.right>source.left) ratio=std::clamp((pressed.x-source.left)/(source.right-source.left),.08f,.92f);
    POINT screenPoint=clientPoint;ClientToScreen(m_hwnd,&screenPoint);
    const LONG visibleLeft=screenPoint.x-(LONG)std::lround(visibleWidth*ratio);
    const LONG visibleTop=screenPoint.y-(LONG)std::lround(24*m_dpiScale);
    const RECT margins{visible.left-outer.left,visible.top-outer.top,outer.right-visible.right,outer.bottom-visible.bottom};
    const LONG outerLeft=visibleLeft-margins.left,outerTop=visibleTop-margins.top;

    m_dragWindow=target;m_dragPid=pid;
    m_dragWasTopmost=(GetWindowLongPtrW(target,GWL_EXSTYLE)&WS_EX_TOPMOST)!=0;
    m_dragOriginalPlacement=original;m_dragOriginalRect=oldRect;m_dragFrameMargins=margins;
    m_dragVisibleRect={visibleLeft,visibleTop,visibleLeft+visibleWidth,visibleTop+visibleHeight};
    m_dragOriginalDesktop=overview->desktop;m_dragChangedDesktop=false;
    if(!IsEqualGUID(overview->desktop,m_overviewCurrentDesktop) && !IsEqualGUID(m_overviewCurrentDesktop,GUID_NULL)) {
        if(FAILED(m_desktopBridge.Move(target,m_overviewCurrentDesktop))) {
            m_dragWindow=nullptr;m_dragPid=0;SetWindowPlacement(target,&original);return false;
        }
        m_dragChangedDesktop=true;
    }
    if(!SetWindowPos(target,HWND_TOPMOST,outerLeft,outerTop,
        outer.right-outer.left,outer.bottom-outer.top,SWP_NOACTIVATE)) {
        RestoreDragWindow();return false;
    }
    m_dragMouseOffset={screenPoint.x-outerLeft,screenPoint.y-outerTop};
    m_dragLastPosition={outerLeft,outerTop};m_dragMoving=true;m_dragDropped=false;
    m_dragRevealDone=true;m_dragRevealStart=NowSeconds();
    ClearPointerPress();ActivateWindow(target);DwmFlush();EndSwitching();
    g_searchTyping=false;g_stickyControls=false;m_hasFocus=false;m_desktopMenuOpen=false;
    BeginClose(false);return true;
}

bool Switcher::PrepareWindowDrag() {
    const HWND target = m_pressedWindow;
    const DrawEntry* preview = nullptr;
    for (auto it = m_drawList.rbegin(); it != m_drawList.rend(); ++it)
        if (it->item->hwnd == target && it->interactive) { preview = &*it; break; }
    DWORD pid = 0;
    if (!preview || !IsWindow(target) || !IsWindowEnabled(target) ||
        !GetWindowThreadProcessId(target, &pid) || pid != preview->item->pid) return false;
    // Moving is driven by held pointer coordinates, not caption hit-testing.
    // Frameless apps can omit WS_SYSMENU and still be moved from any preview
    // pixel, even when their custom title bar is too tall to fit the preview.

    // Convert the preview's visible upper-left corner through the popup's
    // scale/translation to screen pixels, including monitors with negative X.
    D2D1_POINT_2F topLeft = m_popupTransform.TransformPoint(preview->quad.p[0]);
    D2D1_POINT_2F bottomRight = topLeft;
    for (const auto& corner : preview->quad.p) {
        const auto point = m_popupTransform.TransformPoint(corner);
        topLeft.x = std::min(topLeft.x, point.x);
        topLeft.y = std::min(topLeft.y, point.y);
        bottomRight.x = std::max(bottomRight.x, point.x);
        bottomRight.y = std::max(bottomRight.y, point.y);
    }
    const LONG left = m_monitor.left + (LONG)std::lround(topLeft.x);
    const LONG top = m_monitor.top + (LONG)std::lround(topLeft.y);

    WINDOWPLACEMENT original{};
    original.length=sizeof(original);
    if (!GetWindowPlacement(target, &original)) return false;
    RECT oldRect{};
    if (!GetWindowRect(target, &oldRect)) return false;
    const bool restore = IsIconic(target) || IsZoomed(target);
    if (restore) {
        // SW_RESTORE alone can restore a minimized window straight back to
        // maximized (WPF_RESTORETOMAXIMIZED), making it impossible to drag at
        // preview size. Explicitly enter the normal state, preserving the
        // original placement for a failed/canceled drag.
        WINDOWPLACEMENT movable=original;
        movable.flags &= ~WPF_RESTORETOMAXIMIZED;
        // Reveal without taking focus/capture away from the held overlay grip.
        movable.showCmd=SW_SHOWNOACTIVATE;
        if (!SetWindowPlacement(target,&movable)) {
            SetWindowPlacement(target,&original);
            return false;
        }
        DwmFlush();
        if (IsIconic(target) || IsZoomed(target)) {
            SetWindowPlacement(target,&original);
            return false;
        }
    }
    RECT outer{}, visible{};
    if (!GetWindowRect(target, &outer)) {
        if (restore) SetWindowPlacement(target, &original);
        return false;
    }
    visible = outer;
    DwmGetWindowAttribute(target, DWMWA_EXTENDED_FRAME_BOUNDS, &visible, sizeof(visible));
    const bool wasTopmost = (GetWindowLongPtrW(target, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    m_dragWindow = target;
    m_dragPid = pid;
    m_dragWasTopmost = wasTopmost;
    m_dragMoving = false;
    m_dragDropped = false;
    m_dragOriginalPlacement = original;
    m_dragOriginalRect = oldRect;
    m_dragChangedDesktop=false;m_dragOriginalDesktop=GUID_NULL;
    m_dragFrameMargins = {visible.left-outer.left,visible.top-outer.top,
                          outer.right-visible.right,outer.bottom-visible.bottom};
    m_dragVisibleRect = {left,top,
        m_monitor.left+(LONG)std::lround(bottomRight.x),
        m_monitor.top+(LONG)std::lround(bottomRight.y)};
    // Fixed-size windows retain their own dimensions; resizable windows match
    // the preview, with the application's minimum size still respected.
    if (!(GetWindowLongPtrW(target,GWL_STYLE) & WS_THICKFRAME)) {
        m_dragVisibleRect.right=left+visible.right-visible.left;
        m_dragVisibleRect.bottom=top+visible.bottom-visible.top;
    }
    m_dragRevealStart=NowSeconds();
    m_dragRevealDone=false;
    UpdateDragReveal();
    DwmFlush();
    if (m_dragWindow==target && preview->item->minimized && !IsIconic(target)) {
        // Start getting sharp frames immediately while the restored window is
        // being held, instead of waiting for the next full overlay render.
        auto& item=*preview->item;
        StopCapture(item);
        item.minimized=false;
        StartCapture(item);
    }
    return m_dragWindow==target && IsWindow(target);
}

void Switcher::UpdateDragReveal() {
    if (!m_dragWindow || m_dragMoving || m_dragDropped || m_dragRevealDone) return;
    if (!IsWindow(m_dragWindow)) { m_dragWindow=nullptr; return; }
    const float progress=std::clamp((float)((NowSeconds()-m_dragRevealStart)/0.14),0.0f,1.0f);
    const float scale=(GetWindowLongPtrW(m_dragWindow,GWL_STYLE)&WS_THICKFRAME)
        ? 1+0.06f*(1-EaseOutQuart(progress)) : 1;
    const float width=(float)(m_dragVisibleRect.right-m_dragVisibleRect.left);
    const float height=(float)(m_dragVisibleRect.bottom-m_dragVisibleRect.top);
    const LONG left=(LONG)std::lround(m_dragVisibleRect.left-width*(scale-1)*0.5f);
    const LONG top=(LONG)std::lround(m_dragVisibleRect.top-height*(scale-1)*0.5f);
    if (!SetWindowPos(m_dragWindow,HWND_TOPMOST,
        left-m_dragFrameMargins.left,top-m_dragFrameMargins.top,
        (LONG)std::lround(width*scale)+m_dragFrameMargins.left+m_dragFrameMargins.right,
        (LONG)std::lround(height*scale)+m_dragFrameMargins.top+m_dragFrameMargins.bottom,
        SWP_NOACTIVATE)) RestoreDragWindow();
    else if (progress>=1) m_dragRevealDone=true;
}

void Switcher::RestoreDragWindow() {
    const HWND target=m_dragWindow;
    const bool moving=m_dragMoving || m_dragDropped;
    const DWORD expectedPid=m_dragPid;
    const bool changedDesktop=m_dragChangedDesktop;
    const GUID originalDesktop=m_dragOriginalDesktop;
    m_dragWindow=nullptr;
    m_dragMoving=false;
    m_dragDropped=false;
    m_dragPid=0;
    m_dragChangedDesktop=false;m_dragOriginalDesktop=GUID_NULL;
    DWORD pid=0;
    if (!IsWindow(target) || !GetWindowThreadProcessId(target,&pid) ||
        (expectedPid && pid!=expectedPid)) return;
    if (!moving) {
        if(changedDesktop && !IsEqualGUID(originalDesktop,GUID_NULL))
            m_desktopBridge.Move(target,originalDesktop);
        SetWindowPos(target,m_dragWasTopmost ? HWND_TOPMOST : HWND_NOTOPMOST,
            m_dragOriginalRect.left,m_dragOriginalRect.top,
            m_dragOriginalRect.right-m_dragOriginalRect.left,
            m_dragOriginalRect.bottom-m_dragOriginalRect.top,SWP_NOACTIVATE);
        SetWindowPlacement(target,&m_dragOriginalPlacement);
    } else if (!m_dragWasTopmost) {
        SetWindowPos(target,HWND_NOTOPMOST,0,0,0,0,
                     SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
    }
}

bool Switcher::BeginWindowDrag(POINT screenPoint) {
    if (!m_pressCanDrag || !IsKeyDown(VK_LBUTTON)) return false;
    if (!m_dragWindow && !PrepareWindowDrag()) return false;
    const HWND target=m_dragWindow;
    if (!target || !IsWindow(target)) return false;
    // Reveal only once movement passes the drag threshold. Never queue a
    // native move loop that could start after the physical button is up.
    if (!m_dragWindow) return false;
    RECT rect{};
    if (!GetWindowRect(target,&rect)) return false;
    const LONG left=rect.left+screenPoint.x-m_pressScreen.x;
    const LONG top=rect.top+screenPoint.y-m_pressScreen.y;
    if (!SetWindowPos(target,HWND_TOPMOST,left,top,0,0,
                      SWP_NOSIZE|SWP_NOACTIVATE)) return false;
    const LONG settledLeft=m_dragVisibleRect.left-m_dragFrameMargins.left+
                           screenPoint.x-m_pressScreen.x;
    const LONG settledTop=m_dragVisibleRect.top-m_dragFrameMargins.top+
                          screenPoint.y-m_pressScreen.y;
    m_dragMouseOffset={screenPoint.x-settledLeft,screenPoint.y-settledTop};
    m_dragLastPosition={left,top};
    GetWindowThreadProcessId(target,&m_dragPid);
    m_dragMoving=true;
    ClearPointerPress();
    ActivateWindow(target);
    DwmFlush();
    EndSwitching();
    g_searchTyping = false;
    g_stickyControls = false;
    m_hasFocus = false;
    m_desktopMenuOpen = false;
    // The real window stays above the fading UI. Its position is driven only
    // while the physical left button remains down, independently of capture.
    BeginClose(false);
    return true;
}

void Switcher::UpdateWindowDrag() {
    if (!m_dragMoving || !m_dragWindow) return;
    DWORD pid=0;
    POINT cursor{};
    if (!IsKeyDown(VK_LBUTTON) || !IsWindow(m_dragWindow) ||
        !GetWindowThreadProcessId(m_dragWindow,&pid) || pid!=m_dragPid ||
        !GetCursorPos(&cursor)) {
        EndWindowDrag();
        return;
    }
    if (m_heldCapture) PollFrames(*m_heldCapture);
    const POINT position{cursor.x-m_dragMouseOffset.x,
                         cursor.y-m_dragMouseOffset.y};
    if (!m_dragRevealDone) {
        // Shrink around the preview's center while following the held cursor.
        // One path owns both size and position, so the popup cannot fight drag.
        const float progress=std::clamp((float)((NowSeconds()-m_dragRevealStart)/0.14),0.0f,1.0f);
        const float scale=(GetWindowLongPtrW(m_dragWindow,GWL_STYLE)&WS_THICKFRAME)
            ? 1+0.06f*(1-EaseOutQuart(progress)) : 1;
        const float width=(float)(m_dragVisibleRect.right-m_dragVisibleRect.left);
        const float height=(float)(m_dragVisibleRect.bottom-m_dragVisibleRect.top);
        const LONG left=position.x-(LONG)std::lround(width*(scale-1)*0.5f);
        const LONG top=position.y-(LONG)std::lround(height*(scale-1)*0.5f);
        if (!SetWindowPos(m_dragWindow,nullptr,left,top,
            (LONG)std::lround(width*scale)+m_dragFrameMargins.left+m_dragFrameMargins.right,
            (LONG)std::lround(height*scale)+m_dragFrameMargins.top+m_dragFrameMargins.bottom,
            SWP_NOZORDER|SWP_NOACTIVATE)) {
            EndWindowDrag();
            return;
        }
        m_dragLastPosition={left,top};
        if (progress>=1) m_dragRevealDone=true;
        return;
    }
    if (position.x==m_dragLastPosition.x && position.y==m_dragLastPosition.y) return;
    if (!SetWindowPos(m_dragWindow,nullptr,position.x,position.y,0,0,
                      SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE)) {
        EndWindowDrag();
        return;
    }
    m_dragLastPosition=position;
}

void Switcher::EndWindowDrag() {
    if (!m_dragMoving) return;
    SaveHeldPreview(m_dragWindow);
    m_dragMoving=false;
    m_dragDropped=true;
    m_dragRevealDone=true;
    // If the UI is still fading, keep the real window above it until teardown.
    // After the fade, restore only its topmost flag, preserving the drop rect.
    // No mouse message is posted on release, so it cannot restart moving.
    if (m_state!=State::Closing) RestoreDragWindow();
    if (m_hwnd) PostMessageW(m_hwnd,WM_APP_REFRESH_ICONS,0,0);
}

void Switcher::SaveHeldPreview(HWND target) {
    Item* held=m_heldCapture.get();
    if (!held) {
        for (auto& item:m_items)
            if (item && item->hwnd==target) { held=item.get(); break; }
    }
    if (held && held->hwnd==target) {
        PollFrames(*held);
        SaveWindowSnapshot(*held);
    }
    if (m_heldCapture) {
        StopCapture(*m_heldCapture);
        m_heldCapture.reset();
    }
}

struct EnumWindowsContext {
    std::vector<HWND> windows;
    size_t max;
    bool includeMinimized;
    bool includeOtherDesktops=false;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* context = reinterpret_cast<EnumWindowsContext*>(lParam);
    if (IsSwitchableWindow(hwnd,context->includeOtherDesktops) &&
        (context->includeMinimized || !IsIconic(hwnd))) {
        context->windows.push_back(hwnd);
    }
    return context->windows.size() < context->max;
}

void Switcher::CollectWindows() {
    for (auto it=m_windowSnapshots.begin();it!=m_windowSnapshots.end();) {
        DWORD pid=0;
        if (!IsWindow(it->first) || !GetWindowThreadProcessId(it->first,&pid) || pid!=it->second.pid)
            it=m_windowSnapshots.erase(it);
        else ++it;
    }
    EnumWindowsContext context{
        {}, (size_t)m_settings.maxWindows, m_settings.includeMinimized,
        m_settings.otherDesktopWindows && !m_openingOverview};
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&context));

    // EnumWindows returns the Z order, which puts topmost windows first. The
    // foreground window must come first, like in the native switcher.
    HWND foreground = GetForegroundWindow();
    auto it =
        std::find(context.windows.begin(), context.windows.end(), foreground);
    if (it != context.windows.end()) {
        std::rotate(context.windows.begin(), it, it + 1);
    }

    m_items.clear();
    for (HWND hwnd : context.windows) {
        if(context.includeOtherDesktops && !m_desktopBridge.ShowsInSwitcher(hwnd)) continue;
        auto item = std::make_unique<Item>();
        item->hwnd = hwnd;
        DWORD pid=0; GetWindowThreadProcessId(hwnd,&pid);
        item->pid=pid;
        HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
        if (process) {
            FILETIME creation{},exit{},kernel{},user{};
            if (GetProcessTimes(process,&creation,&exit,&kernel,&user)) item->processCreation=FileTimeValue(creation);
        }
        WCHAR image[32768]; DWORD imageLength=ARRAYSIZE(image);
        if (process && QueryFullProcessImageNameW(process,0,image,&imageLength)) {
            item->appKey.assign(image,imageLength);
            const wchar_t* leaf=wcsrchr(image,L'\\');
            item->appName=leaf ? leaf+1 : image;
            const auto extension=item->appName.rfind(L'.');
            if(extension!=std::wstring::npos) item->appName.resize(extension);
            struct CachedAppDescription {std::wstring name;ULONGLONG expires;};
            static std::unordered_map<std::wstring,CachedAppDescription> descriptions;
            auto cached=descriptions.find(item->appKey);
            if(cached!=descriptions.end()&&cached->second.expires>GetTickCount64()) item->appName=cached->second.name;
            else {
            DWORD ignored=0;
            const DWORD bytes=GetFileVersionInfoSizeW(image,&ignored);
            if(bytes && bytes<=1024*1024) {
                std::vector<BYTE> version(bytes);
                if(GetFileVersionInfoW(image,0,bytes,version.data())) {
                    struct Translation {WORD language,codepage;};
                    Translation* translations=nullptr;UINT translationBytes=0;
                    if(VerQueryValueW(version.data(),L"\\VarFileInfo\\Translation",(void**)&translations,&translationBytes)) {
                        for(UINT i=0;i<translationBytes/sizeof(Translation);++i) {
                            wchar_t key[80];
                            swprintf_s(key,L"\\StringFileInfo\\%04x%04x\\FileDescription",translations[i].language,translations[i].codepage);
                            wchar_t* description=nullptr;UINT length=0;
                            if(VerQueryValueW(version.data(),key,(void**)&description,&length) && length>1 && description[0]) {
                                item->appName=description;break;
                            }
                        }
                    }
                }
            }
                if(descriptions.size()>=64)descriptions.clear();
                descriptions[item->appKey]={item->appName,GetTickCount64()+300000};
            }
            CharLowerBuffW(item->appKey.data(),(DWORD)item->appKey.size());
        } else item->appKey=std::to_wstring(pid);
        if (process) CloseHandle(process);
        item->minimized = IsIconic(hwnd);
        if (!GetWindowBounds(hwnd, item->minimized, &item->rect)) {
            continue;
        }

        WCHAR title[256];
        const int length = GetWindowTextW(hwnd, title, ARRAYSIZE(title));
        item->title.assign(title, std::max(length, 0));

        item->icon = GetIconBitmap(hwnd);
        item->placeholder = CreatePlaceholder(*item);
        if (!item->placeholder) {
            continue;
        }
        if (item->minimized) item->minimizedHint=CreatePlaceholder(*item,true);
        m_items.push_back(std::move(item));
    }

    if(m_settings.showDesktopCard && !m_openingOverview && !m_desktopOverview) {
        HWND shell=GetShellWindow();
        if(shell) {
            auto item=std::make_unique<Item>();
            item->hwnd=shell;
            GetWindowThreadProcessId(shell,&item->pid);
            item->desktopPlaceholder=true;
            item->showDesktop=true;
            item->title=Text(L"Show Desktop",L"Mostra desktop");
            item->appKey=L"~covertabby-show-desktop";
            item->rect=m_monitor;
            SHSTOCKICONINFO stock{};stock.cbSize=sizeof(stock);
            if(SUCCEEDED(SHGetStockIconInfo(SIID_DESKTOPPC,
                SHGSI_ICON|SHGSI_LARGEICON,&stock))) {
                item->icon=CreateIconBitmap(stock.hIcon);
                if(stock.hIcon) DestroyIcon(stock.hIcon);
            }
            item->placeholder=LoadWallpaperPreview({});
            if(!item->placeholder) item->placeholder=CreatePlaceholder(*item);
            if(item->placeholder) m_items.push_back(std::move(item));
        }
    }
}

void Switcher::StartCaptures() {
    if(m_settings.simpleStyle && m_settings.simpleStyle<4 && !m_desktopOverview) return;
    m_capturing=true;
    m_captureQueue.clear(); m_captureNext=0;
    const int count=(int)m_items.size(), selected=SelectedIndex();
    // Selected first, then its nearest neighbors; remaining live captures
    // are spread over subsequent frames instead of blocking the first paint.
    for(int index:CaptureOrder(count,selected)) m_captureQueue.push_back(m_items[index]->hwnd);
    if(m_settings.simpleStyle<4) ContinueCaptures(std::min(3,count));
}

void Switcher::ContinueCaptures(int budget) {
    while(budget>0 && m_captureNext<m_captureQueue.size()) {
        const HWND target=m_captureQueue[m_captureNext++];
        const auto item=std::find_if(m_items.begin(),m_items.end(),
            [&](const auto& entry){return entry->hwnd==target;});
        if(item==m_items.end() || (*item)->captureAttempted || (*item)->windowGone) continue;
        StartCapture(**item);
        --budget;
    }
}

bool Switcher::FirstFramesReady() {
    const int selected=SelectedIndex();
    if(selected<0) return true;
    Item& item=*m_items[selected];
    PollFrames(item);
    return !item.framePool || item.content!=nullptr;
}

void Switcher::StartCapture(Item& item) {
    item.captureAttempted=true;
    HWND source = item.hwnd;
    if (item.minimized || item.desktopPlaceholder) {
        // Minimized windows may have no capture surface. Their white card is
        // still hit-tested as a normal preview and can be clicked or dragged.
        return;
    }

    try {
        auto factory =
            winrt::get_activation_factory<wgc::GraphicsCaptureItem>();
        com_ptr<IGraphicsCaptureItemInterop> interop;
        winrt::check_hresult(winrt::get_unknown(factory)->QueryInterface(
            kIID_IGraphicsCaptureItemInterop, interop.put_void()));
        winrt::check_hresult(interop->CreateForWindow(
            source, winrt::guid_of<wgc::GraphicsCaptureItem>(),
            winrt::put_abi(item.captureItem)));

        const auto size = item.captureItem.Size();
        if (size.Width <= 0 || size.Height <= 0) {
            StopCapture(item);
            return;
        }

        item.framePool = wgc::Direct3D11CaptureFramePool::CreateFreeThreaded(
            m_captureDevice, wgdx::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2, size);
        item.poolSize = size;
        item.session = item.framePool.CreateCaptureSession(item.captureItem);

        // Both are optional: older builds lack them, and the border can be
        // enforced by policy.
        try {
            item.session.IsCursorCaptureEnabled(false);
        } catch (...) {
        }
        try {
            item.session.IsBorderRequired(false);
        } catch (...) {
        }

        item.session.StartCapture();
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Capture failed for %p: 0x%08X", item.hwnd, (UINT)e.code());
        StopCapture(item);
    }
}

void Switcher::StopCapture(Item& item) {
    try {
        if (item.session) {
            item.session.Close();
        }
        if (item.framePool) {
            item.framePool.Close();
        }
    } catch (...) {
    }
    item.session = nullptr;
    item.framePool = nullptr;
    item.captureItem = nullptr;

}

void Switcher::PollFrames(Item& item) {
    item.lastFramePoll=NowSeconds();
    // A hidden window may supply blank/stale frames. Preserve the visible
    // frame from before closing until the return animation has finished.
    if (m_returnActive && m_returnEntry.item==&item) return;
    DWORD pid=0;
    if (IsWindow(item.hwnd) && GetWindowThreadProcessId(item.hwnd,&pid) && pid==item.pid) {
        const bool minimized=IsIconic(item.hwnd)!=FALSE;
        if (minimized!=item.minimized) {
            if (minimized) SaveWindowSnapshot(item);
            StopCapture(item);
            item.minimized=minimized;
            item.content=nullptr;
            item.texture=nullptr;
            RECT bounds{};
            if (GetWindowBounds(item.hwnd,minimized,&bounds)) item.rect=bounds;
            auto placeholder=CreatePlaceholder(item);
            if (placeholder) item.placeholder=placeholder;
            item.minimizedHint=minimized ? CreatePlaceholder(item,true) : nullptr;
            if (!minimized) StartCapture(item);
        }
    }
    if (!item.framePool || IsIconic(item.hwnd)) {
        return;
    }

    try {
        wgc::Direct3D11CaptureFrame latest{nullptr};
        while (auto frame = item.framePool.TryGetNextFrame()) {
            if (latest) {
                latest.Close();
            }
            latest = frame;
        }
        if (!latest) {
            return;
        }

        ++item.frameVersion;

        const auto contentSize = latest.ContentSize();
        com_ptr<IDirect3DDxgiInterfaceAccess> access;
        winrt::check_hresult(
            winrt::get_unknown(latest.Surface())
                ->QueryInterface(kIID_IDirect3DDxgiInterfaceAccess,
                                 access.put_void()));
        com_ptr<ID3D11Texture2D> source;
        winrt::check_hresult(access->GetInterface(IID_PPV_ARGS(source.put())));

        D3D11_TEXTURE2D_DESC sourceDesc;
        source->GetDesc(&sourceDesc);
        const UINT width = std::min((UINT)std::max(contentSize.Width, 1),
                                    sourceDesc.Width);
        const UINT height = std::min((UINT)std::max(contentSize.Height, 1),
                                     sourceDesc.Height);

        D3D11_TEXTURE2D_DESC desc{};
        if (item.texture) {
            item.texture->GetDesc(&desc);
        }
        if (!item.texture || desc.Width != width || desc.Height != height) {
            desc = {};
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            com_ptr<ID3D11Texture2D> texture;
            com_ptr<ID2D1Bitmap1> content;
            winrt::check_hresult(
                m_d3dDevice->CreateTexture2D(&desc, nullptr, texture.put()));
            D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_NONE,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED));
            winrt::check_hresult(m_ctx->CreateBitmapFromDxgiSurface(
                texture.as<IDXGISurface>().get(), &props, content.put()));
            item.texture = texture;
            item.content = content;
        }

        const D3D11_BOX box{0, 0, 0, width, height, 1};
        m_d3dContext->CopySubresourceRegion(item.texture.get(), 0, 0, 0, 0,
                                            source.get(), 0, &box);
        latest.Close();

        if (contentSize.Width != item.poolSize.Width ||
            contentSize.Height != item.poolSize.Height) {
            item.framePool.Recreate(
                m_captureDevice,
                wgdx::DirectXPixelFormat::B8G8R8A8UIntNormalized, 2,
                contentSize);
            item.poolSize = contentSize;
        }
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Frame update failed for %p: 0x%08X", item.hwnd,
               (UINT)e.code());
        StopCapture(item);
    }
}

void Switcher::Teardown() {
    DismissIconContext();m_iconMenuAmount=0;m_iconMenuWindow=nullptr;
    ClearPointerPress();
    m_touchSession=false;
    m_desktopOverview = false;
    g_desktopOverviewActive = false;
    g_desktopWinHoldSession = false;
    g_desktopRenameTyping = false;
    m_overviewRenameText.clear();
    m_commitDesktop = false;
    m_touchTapCount = 0;
    ClearOverviewCaptures();
    ClearLiveWallpaper();
    m_overviewWindows.clear();m_overviewDesktops.clear();
    m_overviewSavedBackground=nullptr;m_overviewSavedBackgroundKey.clear();
    SetTouchpadOwnership(false);
    ClearPointerPress();
    // A held drag must survive hiding and tearing down the overlay.
    if (!m_dragMoving) RestoreDragWindow();
    for (auto& item : m_items) {
        if (m_dragMoving && item->hwnd==m_dragWindow) {
            // Keep only the dragged window's capture until mouse release, even
            // though the overlay has already disappeared.
            m_heldCapture=std::move(item);
            continue;
        }
        SaveWindowSnapshot(*item);
        StopCapture(*item);
    }
    m_returnActive=false; m_returnEntry={}; m_closeSceneOffset={};
    m_holdWindow=nullptr;m_holdAmount=0;m_holdFullAt=0;m_holdExpanded=false;m_holdOffset={};m_holdExitBitmap=nullptr;
    m_drawList.clear();
    m_items.clear();
    m_titleLayout = nullptr;
    m_titleIndex = -1;
    m_capturing = false;
    m_captureQueue.clear(); m_captureNext=0; m_snapshotAt=0; m_snapshotNext=0;
}

void Switcher::ShowOverlay() {
    const UINT width = (UINT)(m_monitor.right - m_monitor.left);
    const UINT height = (UINT)(m_monitor.bottom - m_monitor.top);
    if (!EnsureSwapChain(width, height)) {
        HandleDeviceLost();
        return;
    }
    const int backdrop=1; // Captured blur only; no live Desktop Acrylic.
    DwmSetWindowAttribute(m_hwnd,(DWMWINDOWATTRIBUTE)38,&backdrop,sizeof(backdrop));
    const MARGINS margins{0,0,0,0};
    DwmExtendFrameIntoClientArea(m_hwnd,&margins);
    const int corners=1; // Transparent host; round and shadow the animated panel ourselves.
    DwmSetWindowAttribute(m_hwnd,DWMWA_WINDOW_CORNER_PREFERENCE,&corners,sizeof(corners));
    const COLORREF noBorder=0xFFFFFFFE; // DWMWA_COLOR_NONE.
    DwmSetWindowAttribute(m_hwnd,(DWMWINDOWATTRIBUTE)34,&noBorder,sizeof(noBorder));
    const DWMNCRENDERINGPOLICY noNativeShadow=DWMNCRP_DISABLED;
    DwmSetWindowAttribute(m_hwnd,DWMWA_NCRENDERING_POLICY,&noNativeShadow,sizeof(noNativeShadow));
    m_popupShadow=nullptr;
    if(m_settings.windowMode && !m_desktopOverview && !m_settings.simpleStyle) {
        auto silhouette=CreateTargetBitmap((UINT)std::ceil(m_width),(UINT)std::ceil(m_height));
        if(silhouette) {
            const float margin=24*m_dpiScale;
            m_ctx->SetTarget(silhouette.get());m_ctx->BeginDraw();
            m_ctx->Clear(D2D1::ColorF(0,0,0,0));
            m_brush->SetColor(D2D1::ColorF(0,0,0,1));
            m_ctx->FillRoundedRectangle({{margin,margin,m_width-margin,m_height-margin},m_settings.panelRadius*m_dpiScale,m_settings.panelRadius*m_dpiScale},m_brush.get());
            const HRESULT shadowResult=m_ctx->EndDraw();m_ctx->SetTarget(nullptr);
            if(SUCCEEDED(shadowResult) && SUCCEEDED(m_ctx->CreateEffect(CLSID_D2D1Shadow,m_popupShadow.put()))) {
                m_popupShadow->SetInput(0,silhouette.get());
                m_popupShadow->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION,8*m_dpiScale);
            }
        }
    }
    if (m_settings.blurBackground || (m_settings.simpleStyle && !m_desktopOverview)) {
        if(m_desktopOverview) UpdateBackground();
        else CaptureAcrylicBackdrop();
    } else { m_background=nullptr; }
    if (!m_capturing) {
        StartCaptures();
    }

    m_open = {0, 1, NowSeconds(), (m_settings.simpleStyle ? std::min(m_settings.animationDurationMs,100) : m_settings.animationDurationMs) / 1000.0,
              false};
    m_lastFrame = NowSeconds();
    m_state = State::Open;
    if(m_touchSession) {TakeFocus();SetTouchpadOwnership(true);}

    // Present the first frame (identical to the desktop) before showing the
    // window, so nothing stale ever appears.
    RenderFrame();
    if (m_state != State::Open) {
        return;
    }
    SetWindowPos(m_hwnd, HWND_TOPMOST, m_monitor.left, m_monitor.top, width,
                 height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void Switcher::BeginClose(bool commit) {
    DismissIconContext();
    ClearPointerPress();
    if (m_dragWindow && !m_dragMoving) RestoreDragWindow();
    const double now = NowSeconds();
    const float current = m_open.Value(now);
    m_returnActive=false;
    m_closeSceneOffset={};
    const int selected=SelectedIndex();
    const bool heldPreview=m_settings.holdToPreview && m_holdAmount>.001f && selected>=0 && m_items[selected]->hwnd==m_holdWindow;
    if (commit && (!m_settings.simpleStyle || heldPreview) && !m_dragMoving && !m_dragWindow && (m_settings.showThumbnails || heldPreview) && selected>=0 &&
        !m_items[selected]->showDesktop) {
        Item* item=m_items[selected].get();
        auto visible=std::find_if(m_drawList.begin(),m_drawList.end(),
            [&](const DrawEntry& entry) { return entry.item==item; });
        RECT target{};
        DWORD pid=0;
        GetWindowThreadProcessId(item->hwnd,&pid);
        if ((visible!=m_drawList.end() || heldPreview) && pid==item->pid &&
            GetWindowBounds(item->hwnd,IsIconic(item->hwnd)!=FALSE,&target)) {
            if(visible!=m_drawList.end()) m_returnEntry=*visible;
            else {m_returnEntry={};m_returnEntry.item=item;m_returnEntry.pose.opacity=1;m_returnEntry.pose.brightness=1;}
            m_returnFrom=heldPreview ? m_holdQuad : visible->quad;
            if(!heldPreview) for(auto& point:m_returnFrom.p) point=m_popupTransform.TransformPoint(point);
            const RECT previousHost=m_holdExpanded ? m_holdHost : m_monitor;
            RECT host=previousHost;
            RECT padded=target;
            InflateRect(&padded,32,32);
            UnionRect(&host,&host,&padded);
            const UINT width=host.right-host.left, height=host.bottom-host.top;
            if (!EnsureSwapChain(width,height)) { HandleDeviceLost(); return; }
            m_closeSceneOffset={(float)(m_monitor.left-host.left),(float)(m_monitor.top-host.top)};
            for(auto& point:m_returnFrom.p) {
                point.x+=(float)(previousHost.left-host.left); point.y+=(float)(previousHost.top-host.top);
            }
            m_returnTo.p[0]={(float)(target.left-host.left),(float)(target.top-host.top)};
            m_returnTo.p[1]={(float)(target.right-host.left),(float)(target.top-host.top)};
            m_returnTo.p[2]={(float)(target.right-host.left),(float)(target.bottom-host.top)};
            m_returnTo.p[3]={(float)(target.left-host.left),(float)(target.bottom-host.top)};
            SetWindowPos(m_hwnd,HWND_TOPMOST,host.left,host.top,width,height,SWP_NOACTIVATE);
            m_returnActive=true;
            m_holdOffset={};m_holdExpanded=false;m_holdExitBitmap=nullptr;
            // Keep the target visible; the topmost overlay covers the return animation.
        }
    }
    // Keep a full deceleration tail even if Alt is released during opening.
    // Ease-out starts immediately at full speed and settles with zero velocity.
    m_open = {current, 0, now,
              m_settings.animationDurationMs * 1.5 / 1000.0, false};
    if(heldPreview && m_returnActive) m_open.duration=0.38;
    m_commit = commit;
    m_selectedOnTopWhenFlat = commit;
    m_state = State::Closing;
}

void Switcher::ReopenDuringClose(bool backwards) {
    m_holdWindow=nullptr;m_holdSince=0;m_holdAmount=0;m_holdFullAt=0;m_holdExpanded=false;m_holdOffset={};m_holdExitBitmap=nullptr;
    const double now=NowSeconds();
    const float current=m_open.Value(now);
    m_returnActive=false;
    m_returnEntry={};
    m_closeSceneOffset={};
    m_commit=false;
    m_commitDesktop=false;
    m_selectedOnTopWhenFlat=false;
    m_altReleasedAt=0;

    const UINT width=(UINT)(m_monitor.right-m_monitor.left);
    const UINT height=(UINT)(m_monitor.bottom-m_monitor.top);
    if (!EnsureSwapChain(width,height)) {
        HandleDeviceLost();
        return;
    }
    SetWindowPos(m_hwnd,HWND_TOPMOST,m_monitor.left,m_monitor.top,width,height,
                 SWP_NOACTIVATE|SWP_SHOWWINDOW);

    const double fullDuration=m_settings.animationDurationMs/1000.0;
    m_open={current,1,now,std::max(0.06,fullDuration*(1-current)),false};
    m_state=State::Open;
    OnStep(backwards ? -1 : 1);
}

void Switcher::FinishClose() {

    if(m_commitDesktop) {
        const GUID destination=m_destinationDesktop;
        const HWND target=m_overviewSelectedWindow;const DWORD expectedPid=m_overviewSelectedPid;
        HideOverlay();
        const HRESULT hr=m_desktopBridge.Switch(destination);
        if(FAILED(hr)) Wh_Log(L"Desktop switch failed: 0x%08X",(UINT)hr);
        DWORD pid=0;GetWindowThreadProcessId(target,&pid);
        if(SUCCEEDED(hr) && target && IsSwitchableWindow(target,true) && pid==expectedPid && m_desktopBridge.ShowsInSwitcher(target)) ActivateWindow(target);
        EndSession();return;
    }
    HWND target = nullptr;
    bool showDesktop=false;
    if (m_commit) {
        const int selected = SelectedIndex();
        if (selected >= 0) {
            showDesktop=m_items[selected]->showDesktop;
            if(!showDesktop) target = m_items[selected]->hwnd;
        }
    } else if (m_hasFocus) {
        // Canceled after the overlay took the focus: give it back.
        target = m_previousForeground;
    }

    if (target) {
        ActivateWindow(target);
        // Give DWM a frame to bring the window up before revealing it.
        DwmFlush();
    }
    EndSession();
    if(showDesktop) ToggleWindowsDesktop();
}

void Switcher::EndSession() {
    HideOverlay();
    Teardown();
    m_state = State::Idle;
    // Nobody is waiting now, so check the cached icons (see GetIconBitmap).
    PostMessageW(m_hwnd, WM_APP_REFRESH_ICONS, 0, 0);
}

void Switcher::ActivateWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }

    // Keep cross-desktop activation in the common path, including quick
    // Alt+Tab, clicks and modifier-release activation.
    if(m_settings.otherDesktopWindows && !m_desktopOverview) {
        GUID destination{},current{};
        if(SUCCEEDED(m_desktopBridge.WindowDesktop(hwnd,&destination)) &&
            !IsEqualGUID(destination,GUID_NULL) &&
            SUCCEEDED(m_desktopBridge.Current(&current)) && !IsEqualGUID(destination,current)) {
            const HRESULT refreshed=m_desktopBridge.Refresh();
            const HRESULT switched=SUCCEEDED(refreshed) ? m_desktopBridge.Switch(destination) : refreshed;
            if(FAILED(switched)) {Wh_Log(L"Cross-desktop activation failed: 0x%08X",(UINT)switched);return;}
        }
    }

    HWND popup = GetLastActivePopup(hwnd);
    if (!popup || !IsWindowVisible(popup) || !IsWindowEnabled(popup)) {
        popup = hwnd;
    }

    if (IsIconic(hwnd)) {
        ShowWindowAsync(hwnd, SW_RESTORE);
    }

    SendDummyKeyPress();
    if (!SetForegroundWindow(popup)) {
        Wh_Log(L"SetForegroundWindow failed for %p", popup);
    }
}

void Switcher::ComputeLayout() {
    m_width = (float)(m_monitor.right - m_monitor.left)/m_windowScale;
    m_height = (float)(m_monitor.bottom - m_monitor.top)/m_windowScale;
    m_focal = m_width * 0.95f;
    const float spacing = m_settings.stackSpacing;
    if (m_settings.tileGrid) {
        const int columns=std::min(m_items.size()<=6?3:4,std::max(1,(int)m_items.size()));
        const int rows=std::max(1,((int)m_items.size()+columns-1)/columns);
        const float top=std::max(28*m_dpiScale,m_height*.07f);
        const float bottom=m_height-std::min(m_height*.30f,150*m_dpiScale);
        m_tileCellWidth=m_width*.92f/columns;
        m_tileCellHeight=std::min(m_height*.33f,(bottom-top)/rows);
        const float gridHeight=m_tileCellHeight*rows;
        m_tileGridTop=top+std::max(0.0f,(bottom-top-gridHeight)*.48f);
        const float tileInset=m_settings.tileSelectedBigger?1.30f:1.12f;
        m_boxWidth=m_tileCellWidth/tileInset;
        m_boxHeight=m_tileCellHeight/tileInset;
        m_tileIconY=std::min(m_height-(m_settings.selectionBar?150.f:100.f)*m_dpiScale,m_tileGridTop+gridHeight+24*m_dpiScale);
        m_tileTitleY=std::min(m_height-24*m_dpiScale,m_tileIconY+m_settings.iconSize*m_dpiScale*.5f+50*m_dpiScale);
    } else {
        m_boxWidth = m_width * 0.42f;
        m_boxHeight = std::min(m_height*0.64f,m_height*0.48f*m_settings.previewHeight/230.0f);
    }
    if(!m_settings.tileGrid && m_settings.previewWidth>0) m_boxWidth=std::min(m_width*0.70f,m_settings.previewWidth*m_dpiScale);
    m_frontX = m_width * 0.27f;  // Distance of the first side window.
    m_frontY = -m_height * (m_settings.compactCoverFlow ?
        (m_settings.windowMode ? 0.10f : 0.05f) : 0.02f);
    m_stepX = m_width * 0.075f * spacing;
    m_stepY = 0;
    m_stepZ = m_width * 0.22f;  // How far back the sides are.
    m_contentOffset = 0;
    if (m_settings.compactCoverFlow && !m_settings.enableSearch) {
        const float top = m_height * 0.5f + m_frontY - m_boxHeight * 0.5f;
        const float bottom = m_settings.showTitle
            ? m_height * 0.91f + m_settings.titleSize * m_dpiScale * 0.5f
            : (m_settings.showIcons ? m_height * 0.83f + m_settings.iconSize * m_dpiScale * 0.5f
                                    : m_height * 0.5f + m_frontY + m_boxHeight * 0.5f);
        m_contentOffset = (m_height - top - bottom) * 0.5f;
        m_frontY += m_contentOffset;
    }
}

Pose Switcher::FlatPose(const Item& item) const {
    Pose pose;
    pose.x = (item.rect.left + item.rect.right) / 2.0f - m_monitor.left -
             m_width / 2;
    pose.y = (item.rect.top + item.rect.bottom) / 2.0f - m_monitor.top -
             m_height / 2;
    pose.scale = item.minimized ? 0.85f : 1.0f;
    pose.opacity = item.minimized ? 0.0f : 1.0f;
    return pose;
}

// The pose of a window in the switcher. For the stacked styles, `rel` is the
// slot (0 is the front, negative is leaving the front). For the others, it's
// the signed distance from the selected window.
Pose Switcher::LayoutPose(const Item& item, int itemIndex, float rel) const {
    Pose pose;
    const float fit =
        std::min({m_boxWidth / item.Width(), m_boxHeight / item.Height(), 1.0f});
    pose.scale = fit;
    if(m_settings.aero3D) {
        // Flip 3D: parallel tilted planes recede towards the upper left.
        // The leaving front plane and arriving rear plane use separate chains.
        const float depth=std::max(0.f,rel);
        pose.x=m_width*(.13f-.105f*depth);
        pose.y=m_height*(.055f-.065f*depth);
        pose.z=m_width*.15f*depth;
        pose.angleY=-.48f;pose.angleX=.08f;pose.angleZ=-.035f;
        pose.scale*=1.12f;
        pose.brightness=std::max(.45f,1-.065f*depth);
        pose.opacity=std::clamp(8.f-depth,0.f,1.f);
        if(rel<0) {pose.x-=m_width*.32f*(-rel);pose.y+=m_height*.23f*(-rel);pose.opacity*=std::max(0.f,1+rel);}
        pose.highlight=std::max(0.f,1-std::fabs(rel));
        return pose;
    }
    if (m_settings.tileGrid) {
        const int columns = std::min(m_items.size()<=6?3:4, std::max(1, (int)m_items.size()));
        const int column = itemIndex % columns;
        const int row = itemIndex / columns;
        const int rowColumns=std::min(columns,(int)m_items.size()-row*columns);
        pose.x = (column - (rowColumns - 1) * 0.5f) * m_tileCellWidth;
        pose.y = m_tileGridTop+(row+.5f)*m_tileCellHeight-m_height*.5f;
        // Tiles are deliberately flat: no depth, perspective or tilt.
        pose.z = 0.0f;
        pose.angleY = 0.0f;
        pose.angleX = 0.0f;
        pose.opacity = 1.0f;
        pose.brightness = 1.0f;
        // Use the spring position instead of the snapped selected index. This
        // makes the border/size travel smoothly to the next tile.
        const float selectionAmount = item.tileSelectionAmount;
        if (m_settings.tileSelectedBigger && selectionAmount > 0.0f) {
            pose.scale *= 1.0f + 0.28f * selectionAmount;
        }
        pose.highlight = 0.0f;
        return pose;
    }
    const float tilt = m_settings.tiltRadians;
    const float distance = std::fabs(rel);
    const float side = rel < 0 ? -1.0f : 1.0f;

    // The selected window faces the viewer; the others are turned
    // towards it on both sides.
    const float inner = std::min(distance, 1.0f);
    const float outer = std::max(distance - 1, 0.0f);
    // Spread previews along a horizontal track rather than an arc in depth.
    // Scale supplies the distance cue; farther cards keep moving sideways.
    pose.x = side * (inner * m_width * 0.29f + outer * m_width * 0.115f);
    pose.y = m_frontY;
    pose.z = (inner * 0.055f + outer * 0.035f) * m_width;
    pose.scale /= 1 + 0.40f * distance;
    const float naturalTurn=std::clamp(rel,-1.55f,1.55f);
    pose.angleY = naturalTurn *
        (m_settings.compactCoverFlow ? -tilt : std::min(tilt * 2.2f, 1.3f));
    // A tiny forward emphasis and pointer parallax make the scene feel alive
    // without changing the layout or adding another heavy GPU effect.
    pose.scale*=1+0.025f*(1-inner);
    const float parallaxDepth=1-std::min(distance,3.0f)*0.12f;
    pose.x+=m_parallaxX*parallaxDepth;
    pose.y+=m_parallaxY*parallaxDepth;
    if(m_dpiScale>0) pose.angleY+=m_parallaxX/(6*m_dpiScale)*0.012f;
    if(!m_settings.disable3DWindows) {
        const float follow=1-std::min(distance,3.0f)*0.22f;
        pose.angleY+=m_cursorTiltX*follow;pose.angleX+=m_cursorTiltY*follow;
    }
    pose.brightness = 1-inner*(1-m_settings.sideWindowBrightness)-std::min(outer*0.04f,0.3f);
    if(m_settings.disable3DWindows) {
        pose.angleY=0;pose.angleX=0;pose.angleZ=0;
        pose.brightness=1-inner*(1-m_settings.sideWindowBrightness*0.825f)-std::min(outer*0.05f,0.22f);
    }
    pose.opacity = std::clamp((m_settings.compactCoverFlow ? 5.0f : 8.0f) - distance, 0.0f, 1.0f);
    pose.x*=m_settings.sideWindowSpacing;
    pose.scale*=Lerp(1.0f,m_settings.sideWindowScale,inner);
    pose.opacity*=Lerp(1.0f,m_settings.sideWindowOpacity,inner);
    pose.brightness=std::max(0.05f,pose.brightness);
    if (m_settings.compactCoverFlow) pose.highlight = 1 - inner;
    return pose;
}

Quad Switcher::Project(const Pose& pose, float width, float height) const {
    const float halfWidth = width * pose.scale / 2;
    const float halfHeight = height * pose.scale / 2;
    const float cosY = std::cos(pose.angleY), sinY = std::sin(pose.angleY);
    const float cosX = std::cos(pose.angleX), sinX = std::sin(pose.angleX);
    const float cosZ = std::cos(pose.angleZ), sinZ = std::sin(pose.angleZ);
    const float corners[4][2] = {{-halfWidth, -halfHeight},
                                 {halfWidth, -halfHeight},
                                 {halfWidth, halfHeight},
                                 {-halfWidth, halfHeight}};

    Quad quad;
    for (int i = 0; i < 4; i++) {
        // Roll in the screen plane, turn around the vertical axis, then tilt
        // (positive tilts the top away from the viewer).
        const float localX = corners[i][0] * cosZ - corners[i][1] * sinZ;
        const float localY = corners[i][0] * sinZ + corners[i][1] * cosZ;
        const float turnedZ = localX * sinY;
        const float x = pose.x + localX * cosY;
        const float y = pose.y + localY * cosX + turnedZ * sinX;
        const float z = pose.z + turnedZ * cosX - localY * sinX;
        const float k = m_focal / std::max(m_focal + z, m_focal * 0.05f);
        quad.p[i] = {m_width / 2 + x * k, m_height / 2 + y * k};
    }
    return quad;
}

bool Switcher::PreviewContains(const DrawEntry& entry, D2D1_POINT_2F point) const {
    if (QuadContains(entry.quad, point)) return true;
    // Keep the original footprint clickable while the preview lifts away from
    // the pointer. This also prevents repeated enter/leave animation flicker.
    const float hover = entry.item->previewHoverAmount;
    const float press = entry.item->pressAmount;
    if (hover <= 0.001f && press <= 0.001f) return false;
    Pose base = entry.pose;
    base.scale /= 1-0.04f*press;
    const float depth = m_focal / std::max(m_focal + base.z, m_focal * 0.05f);
    if (m_settings.hoverLift) base.y += 10 * m_dpiScale * hover / depth;
    if (m_settings.hoverEnlarge) base.scale /= 1 + 0.06f * hover;
    return QuadContains(Project(base, entry.item->Width(), entry.item->Height()), point);
}

void Switcher::BuildDrawList(float t) {
    m_drawList.clear();
    const int count = (int)m_items.size();
    if (!count) {
        return;
    }
    const int selected = SelectedIndex();

    auto add = [&](int index, int chain, float rel, float fade) {
        Item* item = m_items[index].get();
        if(m_searchActive && (index>=(int)m_searchMask.size() || !m_searchMask[index])) return;
        Pose layout = LayoutPose(*item, index, rel + item->reflowOffset);
        int drawGroup = -1;
        float groupDistance = 0;
        if (!m_settings.tileGrid && m_settings.compactCoverFlow && !m_appGroups.empty()) {
            const int group=m_groupOf[index];
            const float groupCount=(float)m_appGroups.size();
            float groupRel=PositiveMod((float)(group-m_groupScroll)+groupCount/2,groupCount)-groupCount/2;
            drawGroup = group;
            groupDistance = std::fabs(groupRel);
            layout=LayoutPose(*item,index,groupRel+item->reflowOffset);
            const float vertical=m_groupRank[index]-m_groupFocus[group];
            const float distance=std::fabs(vertical);
            layout.y+=vertical*m_height*0.065f;
            layout.z+=distance*m_width*0.035f;
            layout.opacity*=std::max(0.08f,1-distance*0.46f);
            layout.scale*=1-std::min(0.24f,distance*0.08f);
            layout.highlight*=std::max(0.0f,1-distance);
        }
        layout.opacity *= fade;

        const float depthOrder = layout.z / (m_width * 0.05f);

        DrawEntry entry;
        entry.item = item;
        entry.interactive = !item->windowGone;
        entry.group = drawGroup;
        entry.groupDistance = groupDistance;
        entry.selectedGroup = drawGroup >= 0 && selected >= 0 &&
                              drawGroup == m_groupOf[selected];
        entry.chain = chain;
        // Opening and closing fade the laid-out previews without sending them
        // back to their desktop positions along a curved path.
        entry.pose = layout;
        // Tiles should move as a solid grid; do not cross-fade cards while
        // changing selection. Other styles keep their normal transition fade.
        entry.pose.opacity *= t;
        entry.order = depthOrder;
        entry.pose.y -= m_height * 0.18f * item->closeProgress;
        entry.pose.opacity *= 1 - item->closeProgress;
        if (m_settings.hoverControls && item->hoverAmount > 0) {
            const float depth = m_focal / std::max(m_focal + entry.pose.z, m_focal * 0.05f);
            entry.pose.y += 12 * m_dpiScale * item->hoverAmount / depth;
        }
        if (item->previewHoverAmount > 0) {
            const float depth = m_focal / std::max(m_focal + entry.pose.z, m_focal * 0.05f);
            if (m_settings.hoverLift) entry.pose.y -= 10 * m_dpiScale * item->previewHoverAmount / depth;
            if (m_settings.hoverEnlarge) entry.pose.scale *= 1 + 0.06f * item->previewHoverAmount;
        }
        if(m_searchActive) {
            const float progress=std::clamp((float)((NowSeconds()-m_searchTransitionStart)/(m_settings.animateSelection ? 0.10 : 0.000001)),0.0f,1.0f);
            const float ease=1-std::pow(1-progress,3.0f);
            auto previous=m_searchFrom.find(item->hwnd);
            Pose start=entry.pose;
            if(previous!=m_searchFrom.end()) start=previous->second;
            else {start.opacity=0;start.scale*=0.94f;start.y+=18*m_dpiScale;}
            entry.pose=LerpPose(start,entry.pose,ease);
        }
        // Press feedback only scales the preview; no extra movement or glow.
        // Apply after search interpolation so it responds immediately to hold.
        entry.pose.scale *= 1-0.04f*item->pressAmount;
        entry.order=entry.pose.z/(m_width*0.05f);
        entry.quad = Project(entry.pose, item->Width(), item->Height());
        if (entry.pose.opacity > 0.003f) {
            m_drawList.push_back(entry);
        }
    };

    for (int i = 0; i < count; ++i) {
        const float slot = PositiveMod((float)(i - m_scroll), (float)count);
        if(m_settings.aero3D && !m_searchActive) {
            if(slot>count-1.f && count>1) {
                const float entering=slot-(count-1.f);
                add(i,0,slot-count,1);
                add(i,1,slot,1-entering);
            } else add(i,0,slot,1);
        } else add(i, 0, slot >= count / 2.0f ? slot - count : slot, 1);
    }

    if(m_searchActive) {
        const float progress=std::clamp((float)((NowSeconds()-m_searchTransitionStart)/(m_settings.animateSelection ? 0.10 : 0.000001)),0.0f,1.0f);
        if(progress<1) for(int index=0;index<count;++index) {
            if(index<(int)m_searchMask.size() && m_searchMask[index]) continue;
            Item* item=m_items[index].get();
            auto previous=m_searchFrom.find(item->hwnd);
            if(previous==m_searchFrom.end()) continue;
            Pose pose=previous->second;
            const float ease=1-std::pow(1-progress,3.0f);
            pose.opacity*=1-ease;pose.scale*=1-0.06f*ease;pose.y+=18*m_dpiScale*ease;
            if(pose.opacity>0.003f) m_drawList.push_back({item,0,pose,pose.z/(m_width*0.05f),Project(pose,item->Width(),item->Height()),false});
        }
    }
    std::stable_sort(m_drawList.begin(), m_drawList.end(),
                     DrawEntryBehind);
}

D2D1_COLOR_F StyleColor(D2D1_COLOR_F color,float alpha) {color.a=alpha;return color;}

bool Switcher::EnsureChain(EffectChain& chain) {
    if (chain.transform) {
        return true;
    }
    if (FAILED(m_ctx->CreateEffect(CLSID_D2D13DTransform,
                                   chain.transform.put())) ||
        FAILED(m_ctx->CreateEffect(CLSID_D2D1ColorMatrix,
                                   chain.color.put())) ||
        FAILED(m_ctx->CreateEffect(CLSID_D2D1Shadow, chain.shadow.put()))) {
        chain = {};
        return false;
    }

    chain.transform->SetValue(kTransform3DPropInterpolationMode,
                              kTransform3DInterpolationAnisotropic);
    chain.transform->SetValue(kTransform3DPropBorderMode,
                              (UINT32)D2D1_BORDER_MODE_SOFT);
    chain.color->SetInputEffect(0, chain.transform.get());
    chain.shadow->SetInputEffect(0, chain.color.get());
    chain.shadow->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION,
                           kShadowOptimizationSpeed);
    return true;
}

void Switcher::DrawCard(const DrawEntry& source, float t) {
    DrawEntry entry = source;
    const bool returning=m_returnActive && source.item==m_returnEntry.item && m_state==State::Closing;
    if (!returning && m_settings.hoverControls && entry.item->hoverAmount > 0.01f) {
        const auto geometry = GetHoverGeometry(entry);
        const float opacity = entry.item->hoverAmount * t * entry.pose.opacity;
        auto surface = m_settings.backgroundColor;
        surface.a = 0.98f * opacity;
        m_brush->SetColor(surface);
        const float radius = m_settings.panelRadius * m_dpiScale;
        m_ctx->FillRoundedRectangle({geometry.surface, radius, radius}, m_brush.get());
        m_brush->SetColor(StyleColor(m_settings.highlightColor, 0.38f * opacity));
        m_ctx->DrawRoundedRectangle({geometry.surface, radius, radius}, m_brush.get(), m_dpiScale);
    }
    Item& item = *entry.item;
    EffectChain& chain = item.chains[entry.chain];
    if (!EnsureChain(chain)) {
        return;
    }

    ID2D1Bitmap1* bitmap =
        item.content ? item.content.get() : item.placeholder.get();
    if (chain.input != bitmap) {
        chain.transform->SetInput(0, bitmap);
        chain.input = bitmap;
    }

    const D2D1_SIZE_F size = bitmap->GetSize();
    chain.transform->SetValue(
        kTransform3DPropTransformMatrix,
        RectToQuadMatrix(size.width, size.height, entry.quad));

    const float b = entry.pose.brightness;
    const D2D1_MATRIX_5X4_F colorMatrix =
        D2D1::Matrix5x4F(b, 0, 0, 0,                   //
                         0, b, 0, 0,                   //
                         0, 0, b, 0,                   //
                         0, 0, 0, entry.pose.opacity,  //
                         0, 0, 0, 0);
    chain.color->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, colorMatrix);

    if (m_settings.shadows && t > 0.01f) {
        const float depth =
            m_focal / std::max(m_focal + entry.pose.z, m_focal * 0.05f);
        chain.shadow->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION,
                               14.0f * depth);
        chain.shadow->SetValue(D2D1_SHADOW_PROP_COLOR,
                               D2D1::Vector4F(0, 0, 0, 0.55f * t));
        const D2D1_POINT_2F offset{0, 10.0f * depth * t};
        m_ctx->DrawImage(chain.shadow.get(), &offset);
    }
    m_ctx->DrawImage(chain.color.get());

    if (!returning && item.minimized && item.minimizedHint && item.pressAmount<0.999f && EnsureChain(item.hintChain)) {
        auto& hint=item.hintChain;
        hint.transform->SetInput(0,item.minimizedHint.get());
        const auto hintSize=item.minimizedHint->GetSize();
        hint.transform->SetValue(kTransform3DPropTransformMatrix,
            RectToQuadMatrix(hintSize.width,hintSize.height,entry.quad));
        hint.color->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX,
            D2D1::Matrix5x4F(b,0,0,0, 0,b,0,0, 0,0,b,0,
                0,0,0,entry.pose.opacity*(1-item.pressAmount), 0,0,0,0));
        m_ctx->DrawImage(hint.color.get());
    }

    // The top grab area uses the Windows accent on hover. Pressing only
    // scales the preview; the accent gently fades instead of adding effects.
    const float gripAlpha=returning ? 0 : item.gripHoverAmount*entry.pose.opacity*t;
    if (gripAlpha>0.005f) {
        const Quad grip=PreviewDragZone(entry.quad,m_dpiScale);
        com_ptr<ID2D1PathGeometry> geometry;
        com_ptr<ID2D1GeometrySink> sink;
        if (SUCCEEDED(m_d2dFactory->CreatePathGeometry(geometry.put())) &&
            SUCCEEDED(geometry->Open(sink.put()))) {
            sink->BeginFigure(grip.p[0],D2D1_FIGURE_BEGIN_FILLED);
            sink->AddLines(&grip.p[1],3);
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            if (SUCCEEDED(sink->Close())) {
                m_brush->SetColor(StyleColor(m_settings.highlightColor,0.18f*gripAlpha));
                m_ctx->FillGeometry(geometry.get(),m_brush.get());
                m_brush->SetColor(StyleColor(m_settings.highlightColor,0.85f*gripAlpha));
                m_ctx->DrawLine(grip.p[0],grip.p[1],m_brush.get(),1.5f*m_dpiScale);
            }
        }
    }

    // Selection outline.
    const float highlight = (m_settings.compactCoverFlow && m_settings.windowBorders
        ? std::max(0.30f, entry.pose.highlight) : entry.pose.highlight) * entry.pose.opacity * t;
    const float glow=returning ? 0 : item.hoverGlowAmount*entry.pose.opacity*t;
    if (highlight > 0.01f || glow > 0.005f) {
        com_ptr<ID2D1PathGeometry> outline;
        com_ptr<ID2D1GeometrySink> sink;
        if (SUCCEEDED(m_d2dFactory->CreatePathGeometry(outline.put())) &&
            SUCCEEDED(outline->Open(sink.put()))) {
            sink->BeginFigure(entry.quad.p[0], D2D1_FIGURE_BEGIN_HOLLOW);
            sink->AddLines(&entry.quad.p[1], 3);
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            if (SUCCEEDED(sink->Close())) {
                // Overlapping translucent strokes form a visible soft halo,
                // with a bright core rather than only tinting the grab area.
                if (glow>0.005f) {
                    const float pulse=0.5f-0.5f*std::cos(item.hoverGlowPhase);
                    const float spread=0.75f+0.5f*pulse;
                    const float intensity=0.8f+0.2f*pulse;
                    for (int layer=0;layer<6;++layer) {
                        const float width=(24-4*layer)*m_dpiScale*spread;
                        const float opacity=(0.035f+0.022f*layer)*glow*intensity;
                        m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity));
                        m_ctx->DrawGeometry(outline.get(),m_brush.get(),width);
                    }
                }
                m_brush->SetColor(StyleColor(m_settings.borderColor,std::max(0.9f*highlight,glow)));
                m_ctx->DrawGeometry(outline.get(), m_brush.get(),
                                    std::max(2.0f, m_height / 360));
            }
        }
    }
}

void Switcher::DrawTileSelection(float t) {
    if(!m_settings.tileGrid || m_settings.tileSelectedBigger || m_items.empty()) return;
    const int selected=SelectedIndex();
    const DrawEntry* target=nullptr;
    for(const auto& entry:m_drawList) if(entry.item==m_items[selected].get()) {target=&entry;break;}
    if(!target) return;
    D2D1_RECT_F rect{target->quad.p[0].x,target->quad.p[0].y,target->quad.p[0].x,target->quad.p[0].y};
    for(const auto& p:target->quad.p) {
        rect.left=std::min(rect.left,p.x);rect.right=std::max(rect.right,p.x);
        rect.top=std::min(rect.top,p.y);rect.bottom=std::max(rect.bottom,p.y);
    }
    const float pad=4*m_dpiScale;
    rect.left-=pad;rect.right+=pad;rect.top-=pad;rect.bottom+=pad;
    const double now=NowSeconds();
    if(m_tileBorderIndex<0) {
        m_tileBorder=m_tileBorderFrom=m_tileBorderTo=rect;
        m_tileBorderIndex=selected;m_tileBorderAt=now-.25;m_tileBorderWrap=false;
    } else if(m_tileBorderIndex!=selected) {
        m_tileBorderWrap=(m_tileBorderIndex==(int)m_items.size()-1 && selected==0) ||
            (m_tileBorderIndex==0 && selected==(int)m_items.size()-1);
        m_tileBorderFrom=m_tileBorder;m_tileBorderTo=rect;
        m_tileBorderIndex=selected;m_tileBorderAt=now;
    } else m_tileBorderTo=rect;
    const float progress=std::clamp((float)((now-m_tileBorderAt)/.24),0.0f,1.0f);
    float alpha=t;
    if(m_tileBorderWrap) {
        if(progress<.5f) {m_tileBorder=m_tileBorderFrom;alpha*=1-EaseOutQuart(progress*2);}
        else {m_tileBorder=m_tileBorderTo;alpha*=EaseOutQuart((progress-.5f)*2);}
    } else {
        const float ease=EaseOutQuart(progress);
        m_tileBorder={Lerp(m_tileBorderFrom.left,m_tileBorderTo.left,ease),
            Lerp(m_tileBorderFrom.top,m_tileBorderTo.top,ease),
            Lerp(m_tileBorderFrom.right,m_tileBorderTo.right,ease),
            Lerp(m_tileBorderFrom.bottom,m_tileBorderTo.bottom,ease)};
    }
    m_brush->SetColor(StyleColor(m_settings.highlightColor,alpha));
    m_ctx->DrawRoundedRectangle({m_tileBorder,8*m_dpiScale,8*m_dpiScale},m_brush.get(),2.5f*m_dpiScale);
}

void Switcher::DrawSimpleStyle(float t) {
    const int count=(int)m_items.size(), selected=SelectedIndex();
    if(!count || selected<0) return;
    const float ui=m_dpiScale;
    const bool strip=m_settings.simpleStyle==1, textOnly=m_settings.simpleStyle==3;
    const int visible=strip ? std::max(1,std::min(count,(int)((m_width-80*ui)/(68*ui)))) : std::min(count,10);
    const int first=std::clamp(selected-visible/2,0,count-visible);
    const float step=(strip ? 68.f : 46.f)*ui;
    const float width=strip ? visible*step+24*ui : std::min(500*ui,m_width-70*ui);
    const float height=strip ? 92*ui : visible*step+24*ui;
    const float left=(m_width-width)*.5f, top=(m_height-height)*.5f;
    const bool dark=m_settings.darkBackground;
    auto base=UiSurface(t,1.f);
    // Contrast samples are collected from CPU pixels during the backdrop
    // capture, avoiding a GPU readback and synchronization stall on opening.
    const float panelTint=m_settings.enchantedMaterial==3 ? (m_settings.energySaver ? .94f : (dark ? .36f : .42f))
        : (m_settings.enchantedMaterial==1 || m_settings.enchantedMaterial==2) ? .94f : .58f;
    const auto foreground = [&](D2D1_RECT_F rect,bool minimized,float selection) {
        auto behind=base;
        if(m_simpleContrastSamples.size()==32*18) {
            behind=D2D1::ColorF(0,0,0,1);
            for(int sample=0;sample<5;++sample) {
                const float px=rect.left+(rect.right-rect.left)*(sample+.5f)/5;
                const int x=std::clamp((int)(px*32/m_width),0,31);
                const int y=std::clamp((int)((rect.top+rect.bottom)*.5f*18/m_height),0,17);
                const auto color=m_simpleContrastSamples[y*32+x];
                behind.r+=color.r/5;behind.g+=color.g/5;behind.b+=color.b/5;
            }
            behind.r=base.r*panelTint+behind.r*(1-panelTint);
            behind.g=base.g*panelTint+behind.g*(1-panelTint);
            behind.b=base.b*panelTint+behind.b*(1-panelTint);
        }
        const float amount=.28f*selection;
        const auto linear=[](float c) { return c<=.04045f ? c/12.92f : std::pow((c+.055f)/1.055f,2.4f); };
        const float luminance=.2126f*linear(behind.r*(1-amount)+.5f*amount)+
            .7152f*linear(behind.g*(1-amount)+.5f*amount)+.0722f*linear(behind.b*(1-amount)+.5f*amount);
        const bool black=luminance>.179f;
        const float value=minimized ? (black ? .32f : .7f) : (black ? .04f : .98f);
        return D2D1::ColorF(value,value,value,t);
    };
    const auto panel = [&](D2D1_RECT_F rect,float radius) {
        DrawUiPanel(rect,radius,t,panelTint);
    };
    panel({left,top,left+width,top+height},12*ui);
    POINT mouse{};GetCursorPos(&mouse);ScreenToClient(m_hwnd,&mouse);
    const D2D1_POINT_2F pointer=LayoutPoint(mouse);
    if(!m_simpleFormat) {
        if(FAILED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,
            14*ui,L"",m_simpleFormat.put()))) return;
        m_simpleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        m_simpleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    m_iconRects.assign(count,{});
    m_simpleCloseRects.assign(count,{});m_simplePinRects.clear();
    m_simpleDockHover.resize(count,0.f);
    const bool dockHovered=strip && InRect(pointer,{left,top,left+width,top+height});
    for(int i=first;i<first+visible;++i) {
        auto& item=*m_items[i];
        const int slot=i-first;
        const D2D1_RECT_F cell=strip ? D2D1::RectF(left+12*ui+slot*step,top+10*ui,left+12*ui+(slot+1)*step,top+height-10*ui)
            : D2D1::RectF(left+12*ui,top+12*ui+slot*step,left+width-12*ui,top+12*ui+(slot+1)*step);
        m_iconRects[i]=cell;
        const float hoverTarget=InRect(pointer,cell) ? 1.f : 0.f;
        item.iconHoverAmount+=(hoverTarget-item.iconHoverAmount)*(float)(1-std::exp(-22*m_motionDt));
        const float hover=item.iconHoverAmount;
        item.tileSelectionAmount+=((i==selected ? 1.f : 0.f)-item.tileSelectionAmount)*(float)(1-std::exp(-20*m_motionDt));
        m_brush->SetColor(D2D1::ColorF(.5f,.5f,.5f,.28f*t*item.tileSelectionAmount));
        m_ctx->FillRoundedRectangle({cell,7*ui,7*ui},m_brush.get());
        const bool minimized=IsIconic(item.hwnd)!=FALSE;
        if(!textOnly && item.icon) {
            // A continuous mouse-distance wave lifts and magnifies neighboring dock icons.
            const float distance=(pointer.x-(cell.left+cell.right)*.5f)/(step*.85f);
            const float dockTarget=dockHovered ? std::exp(-distance*distance*1.6f) : 0.f;
            float& dock=m_simpleDockHover[i];
            dock+=(dockTarget-dock)*(float)(1-std::exp(-18*m_motionDt));
            const float size=(strip ? 42.f*(1.f+.30f*dock) : 26.f)*ui;
            const float x=strip ? (cell.left+cell.right-size)*.5f : cell.left+14*ui;
            const float y=(cell.top+cell.bottom-size)*.5f-(strip ? (3.f+13.f*dock)*ui : 0);
            if(minimized) {
                if(!m_simpleGray) m_ctx->CreateEffect(CLSID_D2D1ColorMatrix,m_simpleGray.put());
                if(m_simpleGray) {
                    D2D1_MATRIX_5X4_F matrix={.2126f,.2126f,.2126f,0,.7152f,.7152f,.7152f,0,.0722f,.0722f,.0722f,0,0,0,0,t*.65f,0,0,0,0};
                    m_simpleGray->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX,matrix);
                    m_simpleGray->SetInput(0,item.icon.get());
                    D2D1_MATRIX_3X2_F transform;m_ctx->GetTransform(&transform);
                    const auto bitmapSize=item.icon->GetSize();
                    if(bitmapSize.width>0 && bitmapSize.height>0) {
                        m_ctx->SetTransform(D2D1::Matrix3x2F::Scale(size/bitmapSize.width,size/bitmapSize.height)*D2D1::Matrix3x2F::Translation(x,y)*transform);
                        m_ctx->DrawImage(m_simpleGray.get());m_ctx->SetTransform(transform);
                    }
                }
            } else m_ctx->DrawBitmap(item.icon.get(),{x,y,x+size,y+size},t,D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
        }
        if(!strip) {
            m_simpleFormat->SetTextAlignment(textOnly ? DWRITE_TEXT_ALIGNMENT_CENTER : DWRITE_TEXT_ALIGNMENT_LEADING);
            m_brush->SetColor(foreground(cell,minimized,item.tileSelectionAmount));
            const auto& title=item.title;
            m_ctx->DrawText(title.c_str(),(UINT32)title.size(),m_simpleFormat.get(),
                {cell.left+(textOnly ? 10 : 52)*ui,cell.top,cell.right-100*ui,cell.bottom},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        if(!strip) {
            const float badgeRight=cell.right-10*ui-30*ui*hover;
            const D2D1_RECT_F badge{badgeRight-64*ui,cell.top+10*ui,badgeRight,cell.bottom-10*ui};
            m_brush->SetColor(D2D1::ColorF(.38f,.38f,.4f,.48f*t));
            m_ctx->FillRoundedRectangle({badge,5*ui,5*ui},m_brush.get());
            m_simpleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            m_brush->SetColor(D2D1::ColorF(1,1,1,t));
            const std::wstring shortcut=L"Alt + "+std::to_wstring(i+1);
            m_ctx->DrawText(shortcut.c_str(),(UINT32)shortcut.size(),m_simpleFormat.get(),badge,m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        if(hover>.01f) {
            const float cx=strip ? cell.right-9*ui : cell.right-15*ui;
            const float cy=strip ? cell.top+9*ui : (cell.top+cell.bottom)*.5f;
            const float radius=10*ui;
            if(hoverTarget>0) m_simpleCloseRects[i]={cx-radius,cy-radius,cx+radius,cy+radius};
            m_brush->SetColor(D2D1::ColorF(.95f,.12f,.16f,.2f*t*hover));
            m_ctx->FillEllipse({{cx,cy},radius,radius},m_brush.get());
            m_brush->SetColor(D2D1::ColorF(1,.2f,.23f,t*hover));
            m_ctx->DrawLine({cx-3*ui,cy-3*ui},{cx+3*ui,cy+3*ui},m_brush.get(),1.5f*ui);
            m_ctx->DrawLine({cx-3*ui,cy+3*ui},{cx+3*ui,cy-3*ui},m_brush.get(),1.5f*ui);
        }
    }
    const float destination=(selected-first)*step;
    if(m_simpleIndicator<0) m_simpleIndicator=destination;
    else m_simpleIndicator+=(destination-m_simpleIndicator)*(m_settings.animateSelection ? (float)(1-std::exp(-22*m_motionDt)) : 1.f);
    m_brush->SetColor(StyleColor(m_settings.highlightColor,t));
    // Icon underline touches the cell bottom; list indicator stays inset on the left.
    const D2D1_RECT_F indicator=strip ? D2D1::RectF(left+12*ui+m_simpleIndicator+step*.5f-18*ui,top+height-16*ui,left+12*ui+m_simpleIndicator+step*.5f+18*ui,top+height-10*ui)
        : D2D1::RectF(left+16*ui,top+12*ui+m_simpleIndicator+9*ui,left+21*ui,top+12*ui+m_simpleIndicator+37*ui);
    m_ctx->FillRoundedRectangle({indicator,2.5f*ui,2.5f*ui},m_brush.get());
    if(strip) {
        const auto& captionText=m_items[selected]->title;
        com_ptr<IDWriteTextLayout> measured;
        float captionWidth=width;
        if(SUCCEEDED(m_dwriteFactory->CreateTextLayout(captionText.c_str(),(UINT32)captionText.size(),
            m_simpleFormat.get(),m_width-40*ui,36*ui,measured.put()))) {
            DWRITE_TEXT_METRICS metrics{};measured->GetMetrics(&metrics);
            captionWidth=std::min(m_width-40*ui,metrics.widthIncludingTrailingWhitespace+12*ui);
        }
        const float captionLeft=(m_width-captionWidth)*.5f;
        const D2D1_RECT_F caption{captionLeft,top+height+6*ui,captionLeft+captionWidth,top+height+42*ui};
        panel(caption,8*ui);
        m_simpleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_brush->SetColor(foreground(caption,IsIconic(m_items[selected]->hwnd)!=FALSE,0));
        const auto& title=m_items[selected]->title;
        m_ctx->DrawText(title.c_str(),(UINT32)title.size(),m_simpleFormat.get(),{caption.left+6*ui,caption.top,caption.right-6*ui,caption.bottom},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }
    if(m_state!=State::Closing && m_simpleRaised!=m_items[selected]->hwnd) {
        m_simpleRaised=m_items[selected]->hwnd;
        SetWindowPos(m_hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
    }
}
void Switcher::DrawTitle(float t) {
    if (!m_settings.showTitle || m_items.empty() || (m_searchActive && m_searchMatches.empty())) {
        return;
    }

    // Visible while settled, faded out while flipping.
    const double nearest = std::round(m_scroll);
    const float settle = m_settings.tileGrid ? 1.0f :
        1 - std::clamp((float)std::fabs(m_scroll - nearest) * (m_settings.compactCoverFlow ? 2.0f : 3.0f), 0.0f, 1.0f);
    const float searchEase=m_searchActive ? 1-std::pow(1-std::clamp((float)((NowSeconds()-m_searchTransitionStart)/(m_settings.animateSelection ? 0.10 : 0.000001)),0.0f,1.0f),3.0f) : 1;
    const float opacity = t * t * settle * searchEase;
    if (opacity < 0.01f) {
        return;
    }

    const int index = (m_settings.tileGrid || m_settings.groupPrograms) ? SelectedIndex() : PositiveMod((long long)nearest, (int)m_items.size());
    const Item& item = *m_items[index];

    const float fontSize = std::round((m_settings.compactCoverFlow ? m_settings.titleSize : 21) * m_dpiScale);
    if (!m_titleFormat) {
        if (FAILED(m_dwriteFactory->CreateTextFormat(
                m_settings.fontFamily.c_str(), nullptr,
                m_settings.fontWeight, m_settings.fontStyle,
                DWRITE_FONT_STRETCH_NORMAL, fontSize, L"",
                m_titleFormat.put()))) {
            return;
        }
        m_titleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        const DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER,
                                       0, 0};
        com_ptr<IDWriteInlineObject> ellipsis;
        m_dwriteFactory->CreateEllipsisTrimmingSign(m_titleFormat.get(),
                                                    ellipsis.put());
        m_titleFormat->SetTrimming(&trimming, ellipsis.get());
        m_titleLayout = nullptr;
    }

    if (index != m_titleIndex || !m_titleLayout) {
        m_titleLayout = nullptr;
        m_titleIndex = index;
        if (FAILED(m_dwriteFactory->CreateTextLayout(
                item.title.c_str(), (UINT32)item.title.size(),
                m_titleFormat.get(), m_width * 0.6f, fontSize * 2,
                m_titleLayout.put()))) {
            return;
        }
    }

    DWRITE_TEXT_METRICS metrics;
    m_titleLayout->GetMetrics(&metrics);
    const float iconSize = item.icon && !m_settings.compactCoverFlow ? std::round(fontSize * 1.6f) : 0;
    const float gap = iconSize > 0 ? std::round(fontSize * 0.6f) : 0;
    const float left =
        std::round((m_width - (iconSize + gap + metrics.width)) / 2);
    const float titleDrop = m_settings.compactCoverFlow && m_state != State::Closing
        ? 18 * m_dpiScale * (1 - settle*searchEase) : 0;
    const float titleAnchor=m_settings.tileGrid?m_tileTitleY:m_settings.windowMode?m_height-32*m_dpiScale-metrics.height*.5f:m_height*.91f+m_contentOffset;
    const float titleCenter=std::max(titleAnchor,m_statsBottom+12*m_dpiScale+metrics.height*0.5f);
    const float footerTop=m_settings.selectionBar ? CoverTabbyPosition::bounds(m_width,m_height,m_dpiScale,m_settings.windowMode).labelTop-12*m_dpiScale : m_height-28*m_dpiScale;
    const float centerY = std::round(std::min(titleCenter+titleDrop,footerTop-metrics.height*0.5f));

    if (iconSize > 0) {
        const D2D1_RECT_F iconRect{left, centerY - iconSize / 2,
                                   left + iconSize, centerY + iconSize / 2};
        m_ctx->DrawBitmap(item.icon.get(), iconRect, opacity,
                          D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
    }

    const D2D1_POINT_2F origin{left + iconSize + gap - metrics.left,
                               centerY - metrics.height / 2 - metrics.top};
    m_brush->SetColor(D2D1::ColorF(0, 0, 0, 0.55f * opacity));
    m_ctx->DrawTextLayout({origin.x, origin.y + 2}, m_titleLayout.get(),
                          m_brush.get());
    m_brush->SetColor(m_settings.darkBackground ? D2D1::ColorF(1,1,1,opacity) : D2D1::ColorF(0.08f,0.08f,0.08f,opacity));
    m_ctx->DrawTextLayout(origin, m_titleLayout.get(), m_brush.get());
}

// Compact panel mode shares the existing capture, perspective and navigation engine.


void Switcher::UpdateAppGroups(bool reset) {
    m_appGroups.clear(); m_groupOf.assign(m_items.size(),-1); m_groupRank.assign(m_items.size(),0);
    std::unordered_map<std::wstring,int> groups;
    auto add=[&](int index) {
        const auto& item=*m_items[index];
        const std::wstring key=m_settings.groupPrograms ? item.appKey : std::to_wstring((uintptr_t)item.hwnd);
        auto [it,added]=groups.emplace(key,(int)m_appGroups.size());
        if (added) m_appGroups.emplace_back();
        const int group=it->second;
        m_groupOf[index]=group; m_groupRank[index]=(int)m_appGroups[group].size();
        m_appGroups[group].push_back(index);
    };
    if (m_searchActive) { for (int index:m_searchMatches) add(index); }
    else { for (int index=0;index<(int)m_items.size();++index) add(index); }
    const int count=(int)m_appGroups.size();
    m_groupFocus.assign(count,0);
    if (!count || m_items.empty()) return;
    const int selected=SelectedIndex();
    const int selectedGroup=m_groupOf[selected];
    if (selectedGroup<0) return;
    const double now=NowSeconds();
    const double dt=std::clamp(now-m_groupFocusTime,0.0,0.05);
    m_groupFocusTime=now;
    if (reset) {
        m_groupScroll=selectedGroup;
        m_animatedGroupFocus.clear();
    } else {
        // Horizontal movement depends only on the selected application. Moving
        // within a group never crosses unrelated groups, even when it wraps.
        double delta=selectedGroup-PositiveMod((float)m_groupScroll,(float)count);
        if (delta>count*0.5) delta-=count;
        else if (delta< -count*0.5) delta+=count;
        if (count>1 && std::fabs(std::fabs(delta)-count*0.5)<0.001 && std::fabs(m_target-m_scroll)>0.001)
            delta=std::copysign(std::fabs(delta),(double)m_target-m_scroll);
        const double blend=m_settings.animateSelection ? 1-std::exp(-18*m_settings.flipSpeed*dt) : 1;
        m_groupScroll+=delta*blend;
        if (std::fabs(delta)<0.001) m_groupScroll+=delta*(1-blend);
    }
    const float blend=m_settings.animateSelection && !reset
        ? (float)(1-std::exp(-14*m_settings.flipSpeed*dt)) : 1;
    for (int group=0;group<count;++group) {
        const auto& leader=*m_items[m_appGroups[group].front()];
        const std::wstring key=m_settings.groupPrograms ? leader.appKey : std::to_wstring((uintptr_t)leader.hwnd);
        const float target=group==selectedGroup ? (float)m_groupRank[selected] : 0;
        auto [it,added]=m_animatedGroupFocus.emplace(key,reset ? target : 0);
        it->second+=(target-it->second)*blend;
        if (std::fabs(target-it->second)<0.001f) it->second=target;
        m_groupFocus[group]=it->second;
    }
}

void Switcher::DrawProcessStats(float t) {
    m_statsBottom=0;
    if (!m_settings.showIcons || m_items.empty() || (m_searchActive && m_searchMatches.empty())) return;
    const int selected=SelectedIndex();
    if (selected<0 || selected>=(int)m_iconRects.size()) return;
    const auto icon=m_iconRects[selected];
    if (icon.right<=icon.left) return;
    const float ui=m_dpiScale;
    const float fontSize=11*ui;
    if (!m_statsFormat || std::fabs(fontSize-m_statsFontSize)>0.2f) {
        m_statsFormat=nullptr;
        if (FAILED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,
            fontSize,L"",m_statsFormat.put()))) return;
        m_statsFontSize=fontSize;
        m_statsFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        m_statsFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    const int groupCount=(int)m_appGroups.size();
    const int visible=std::min(groupCount,m_settings.visibleIcons);
    if (!visible) return;
    const float step=std::min(std::max((float)m_settings.iconSpacing,m_settings.iconSize/0.90f)*ui,m_width*0.70f/visible);
    const float size=std::min(m_settings.iconSize*ui,step*0.90f);
    const float distance=std::fabs(m_scroll-m_target);
    const float alpha=t*std::max(0.0f,1-std::min(1.0f,distance*2));
    wchar_t cpu[48],ram[64],gpu[48];
    if (m_processStats.cpuKnown) swprintf_s(cpu,L"CPU: %.0f%%",m_processStats.cpu);
    else wcscpy_s(cpu,Text(L"CPU: N/A",L"CPU: N/D"));
    if (m_processStats.ramKnown) {
        const double mb=m_processStats.ramBytes/(1024.0*1024.0);
        if (mb>=1024) swprintf_s(ram,L"RAM: %.1f GB",mb/1024);
        else swprintf_s(ram,L"RAM: %.0f MB",mb);
    } else wcscpy_s(ram,Text(L"RAM: N/A",L"RAM: N/D"));
    if (m_processStats.gpuKnown) swprintf_s(gpu,L"GPU: %.0f%%",m_processStats.gpu);
    else wcscpy_s(gpu,Text(L"GPU: N/A",L"GPU: N/D"));
    const wchar_t* segments[]={cpu,L" - ",ram,L" - ",gpu};
    const double percentages[]={m_processStats.cpu,-1,m_processStats.ramPercent,-1,m_processStats.gpu};
    const bool known[]={m_processStats.cpuKnown,false,m_processStats.ramKnown,false,m_processStats.gpuKnown};
    com_ptr<IDWriteTextLayout> layouts[5]; DWRITE_TEXT_METRICS metrics[5]{};
    float total=0;
    for (int i=0;i<5;++i) {
        if (FAILED(m_dwriteFactory->CreateTextLayout(segments[i],(UINT32)wcslen(segments[i]),
            m_statsFormat.get(),m_width,24*ui,layouts[i].put()))) return;
        layouts[i]->GetMetrics(&metrics[i]); total+=metrics[i].widthIncludingTrailingWhitespace;
    }
    const float xCenter=(icon.left+icon.right)*0.5f;
    const float top=(icon.top+icon.bottom)*0.5f+size*0.5f+5*ui;
    m_statsBottom=top+18*ui;
    float x=std::clamp(xCenter-total*0.5f,28*ui,std::max(28*ui,m_width-28*ui-total));
    for (int i=0;i<5;++i) {
        D2D1_COLOR_F color=m_settings.darkBackground ? D2D1::ColorF(1,1,1,alpha)
            : D2D1::ColorF(0.10f,0.10f,0.10f,alpha);
        if (known[i]) {
            const double value=percentages[i];
            if (value<=10) color=D2D1::ColorF(0.25f,0.86f,0.45f,alpha);
            else if (value<35) color=D2D1::ColorF(1,1,1,alpha);
            else if (value<50) color=D2D1::ColorF(1,0.85f,0.20f,alpha);
            else if (value<=75) color=D2D1::ColorF(1,0.52f,0.12f,alpha);
            else color=D2D1::ColorF(1,0.23f,0.27f,alpha);
        }
        // A subtle shadow keeps white readable on the Windows light theme.
        m_brush->SetColor(D2D1::ColorF(0,0,0,0.40f*alpha));
        m_ctx->DrawTextLayout({x,top+ui},layouts[i].get(),m_brush.get());
        m_brush->SetColor(color);
        m_ctx->DrawTextLayout({x,top},layouts[i].get(),m_brush.get());
        x+=metrics[i].widthIncludingTrailingWhitespace;
    }
}

void Switcher::DrawCompactIcons(float t) {
    if(!m_settings.showIcons) {m_iconRects.assign(m_items.size(),{});return;}
    if(m_searchActive) t*=1-std::pow(1-std::clamp((float)((NowSeconds()-m_searchTransitionStart)/(m_settings.animateSelection ? 0.10 : 0.000001)),0.0f,1.0f),3.0f);
    const int count = (int)m_items.size();
    if (!count) return;
    m_iconRects.assign(count, D2D1_RECT_F{});
    const int selected = SelectedIndex();
    const int groupCount = (int)m_appGroups.size();
    if (!groupCount) return;
    const int visible = std::min(groupCount, m_settings.visibleIcons);
    const float step = std::min(std::max((float)m_settings.iconSpacing,m_settings.iconSize/0.90f)*m_dpiScale,m_width*0.70f/visible);
    const float centerY = m_settings.tileGrid ? m_tileIconY : m_height * 0.80f + m_contentOffset;
    POINT iconCursor{};
    const bool hasIconCursor=m_state==State::Open && !m_iconMenuOpen &&
        GetCursorPos(&iconCursor) && ScreenToClient(m_hwnd,&iconCursor);
    const auto iconPointer=LayoutPoint(iconCursor);
    const float iconBlend=m_settings.animateHover ? (float)(1-std::exp(-18*m_motionDt)) : 1;
    // Resolve every group's nearest continuous carousel position. An integer
    // visible-slot range omitted incoming icons until the scroll settled.
    for (int group = 0; group < groupCount; ++group) {
        const auto& members = m_appGroups[group];
        const int index = m_groupOf[selected] == group ? selected : members.front();
        Item& item = *m_items[index];
        const float relative=PositiveMod((float)(group-m_groupScroll)+groupCount*0.5f,(float)groupCount)-groupCount*0.5f+item.reflowOffset;
        const float distance = std::fabs(relative);
        const float selection = std::max(0.0f, 1.0f - distance);
        const D2D1_RECT_F iconFootprint{m_width*.5f+relative*step-step*.46f,centerY-step*.46f,
            m_width*.5f+relative*step+step*.46f,centerY+step*.46f};
        const float hoverTarget=hasIconCursor && InRect(iconPointer,iconFootprint) ? 1.0f : 0.0f;
        item.iconHoverAmount+=(hoverTarget-item.iconHoverAmount)*iconBlend;
        const float iconLift=5*m_dpiScale*item.iconHoverAmount;
        const float size = std::min(m_settings.iconSize*m_dpiScale,step*0.90f) / (1 + distance * 0.24f)*(1+0.10f*item.iconHoverAmount);
        const float edgeProgress=groupCount>1 ? std::clamp((visible*0.5f-distance)/0.85f,0.0f,1.0f) : 1.0f;
        const float edgeFade=edgeProgress*edgeProgress*(3-2*edgeProgress);
        if(edgeFade<=0.001f) continue;
        const float x = m_width * 0.5f + relative * step;
        const float iconY = centerY - iconLift - m_height * 0.18f * item.closeProgress;
        const D2D1_RECT_F rect{x - size / 2, iconY - size / 2,
                              x + size / 2, iconY + size / 2};
        m_iconRects[index] = {x - step * 0.46f, centerY - step * 0.46f,
                              x + step * 0.46f, centerY + step * 0.46f};
        if(edgeFade<0.25f) m_iconRects[index]={};
        for (int member : members) if (member != index) m_iconRects[member] = {};
        if(item.iconHoverAmount>.005f) {
            const auto r=iconFootprint;
            m_brush->SetColor(StyleColor(m_settings.highlightColor,t*edgeFade*item.iconHoverAmount*.18f));
            m_ctx->FillRoundedRectangle({r,10*m_dpiScale,10*m_dpiScale},m_brush.get());
            m_brush->SetColor(StyleColor(m_settings.highlightColor,t*edgeFade*item.iconHoverAmount*.45f));
            m_ctx->DrawRoundedRectangle({r,10*m_dpiScale,10*m_dpiScale},m_brush.get(),m_dpiScale);
        }
        if (selection > 0.01f) {
            const auto& r = m_iconRects[index];
            m_brush->SetColor(StyleColor(m_settings.iconBackgroundColor,m_settings.iconBackgroundOpacity*t*selection));
            m_ctx->FillRoundedRectangle({r, 10 * m_dpiScale, 10 * m_dpiScale}, m_brush.get());
            m_brush->SetColor(StyleColor(m_settings.highlightColor,t * selection));
            m_ctx->DrawRoundedRectangle({r, 10 * m_dpiScale, 10 * m_dpiScale}, m_brush.get(), 2);
        }
        if (!members.empty()) {
            // Subtle inward lean, limited to six degrees; selection stays upright.
            const float lean = std::clamp(relative * -2.0f, -6.0f, 6.0f);
            D2D1_MATRIX_3X2_F previousTransform;
            m_ctx->GetTransform(&previousTransform);
            const auto rotation = D2D1::Matrix3x2F::Rotation(lean, {x, centerY});
            m_ctx->SetTransform(rotation * D2D1::Matrix3x2F(previousTransform._11, previousTransform._12, previousTransform._21, previousTransform._22, previousTransform._31, previousTransform._32));
            // Each window contributes its own HWND-specific icon; never duplicate
            // the selected window's bitmap for the other members of the group.
            std::vector<int> layers = members;
            const float focus = m_groupFocus[group];
            std::stable_sort(layers.begin(), layers.end(), [&](int a, int b) {
                return std::fabs(m_groupRank[a] - focus) > std::fabs(m_groupRank[b] - focus);
            });
            const float baseOpacity = t * std::max(0.25f, 1.0f - distance * 0.15f) * edgeFade;
            for (int member : layers) {
                Item& window = *m_items[member];
                const float vertical = m_groupRank[member] - focus;
                const float depth = std::fabs(vertical);
                const float fade = std::clamp(1.0f - depth * 0.42f, 0.0f, 1.0f);
                if (fade <= 0.01f) continue;
                const float layerSize = size * (1 - std::min(0.28f, depth * 0.10f));
                const float y = centerY - iconLift + vertical * size * 0.28f - m_height * 0.18f * window.closeProgress;
                const D2D1_RECT_F layerRect{x - layerSize / 2, y - layerSize / 2,
                                           x + layerSize / 2, y + layerSize / 2};
                const float alpha = baseOpacity * fade * (1 - window.closeProgress);
                if (window.icon) {
                    m_ctx->DrawBitmap(window.icon.get(), layerRect, alpha,
                                      D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
                } else {
                    m_brush->SetColor(D2D1::ColorF(0.7f, 0.75f, 0.8f, alpha));
                    m_ctx->DrawRoundedRectangle({layerRect, 4, 4}, m_brush.get(), 2);
                }
            }
            m_ctx->SetTransform(previousTransform);
        } else {
            m_brush->SetColor(D2D1::ColorF(0.7f, 0.75f, 0.8f, t));
            m_ctx->DrawRoundedRectangle({rect, 4, 4}, m_brush.get(), 2);
        }
    }
}

void Switcher::DrawReflection(const DrawEntry& entry, float t) {
    if (!m_settings.compactCoverFlow || !m_settings.reflections) return;
    Item& item = *entry.item;
    auto& chain = item.chains[entry.chain];
    if (!EnsureChain(chain)) return;
    auto bitmap = item.content ? item.content.get() : item.placeholder.get();
    if (!bitmap) return;
    chain.transform->SetInput(0, bitmap);
    chain.input = bitmap;
    Quad mirror = entry.quad;
    // Compress the upside-down image below each preview's lower edge.
    mirror.p[0] = {entry.quad.p[3].x, entry.quad.p[3].y + (entry.quad.p[3].y - entry.quad.p[0].y) * 0.24f};
    mirror.p[1] = {entry.quad.p[2].x, entry.quad.p[2].y + (entry.quad.p[2].y - entry.quad.p[1].y) * 0.24f};
    mirror.p[2] = entry.quad.p[2];
    mirror.p[3] = entry.quad.p[3];
    const auto size = bitmap->GetSize();
    chain.transform->SetValue(kTransform3DPropTransformMatrix, RectToQuadMatrix(size.width, size.height, mirror));
    const float alpha = 0.18f * entry.pose.opacity * t;
    chain.color->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX,
        D2D1::Matrix5x4F(0.7f,0,0,0, 0,0.7f,0,0, 0,0,0.7f,0, 0,0,0,alpha, 0,0,0,0));
    // Horizontal slices fade the reflection towards its far edge.
    float top = std::min(mirror.p[2].y, mirror.p[3].y);
    float bottom = std::max(mirror.p[0].y, mirror.p[1].y);
    for (int i = 0; i < 12; ++i) {
        const float opacity = 1 - (i + 0.5f) / 12;
        chain.color->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX,
            D2D1::Matrix5x4F(0.7f,0,0,0, 0,0.7f,0,0, 0,0,0.7f,0, 0,0,0,alpha * opacity, 0,0,0,0));
        m_ctx->PushAxisAlignedClip({0, top + (bottom-top)*i/12, m_width, top + (bottom-top)*(i+1)/12}, D2D1_ANTIALIAS_MODE_ALIASED);
        m_ctx->DrawImage(chain.color.get());
        m_ctx->PopAxisAlignedClip();
    }
}

HoverGeometry Switcher::GetHoverGeometry(const DrawEntry& entry) const {
    float left = m_width, right = 0, top = m_height, bottom = 0;
    for (auto point : entry.quad.p) {
        left = std::min(left, point.x); right = std::max(right, point.x);
        top = std::min(top, point.y); bottom = std::max(bottom, point.y);
    }
    HoverGeometry geometry;
    const float width = std::max(1.0f, right - left);
    geometry.uiScale = m_dpiScale * std::clamp(width / (500 * m_dpiScale), 0.6f, 1.0f);
    const float ui = geometry.uiScale;
    const float hover = entry.item->hoverAmount;
    const float pad = 6 * ui, header = 28 * ui, button = 24 * ui;
    geometry.surface = {left - pad * hover, top - header * hover,
                        right + pad * hover, bottom + (button + 2 * pad) * hover};
    geometry.title = {left + 4 * ui, top - header * hover, right - 4 * ui, top};
    const float controlsTop = bottom + pad * hover;
    geometry.pin = {left + 4 * ui, controlsTop, left + 34 * ui + 48*ui*entry.item->pinHoverAmount, controlsTop + button};
    geometry.close = {right - 4 * ui - button - 48*ui*entry.item->closeHoverAmount, controlsTop,
                      right - 4 * ui, controlsTop + button};
    geometry.kill = {geometry.close.left-6*ui-button-38*ui*entry.item->killHoverAmount,controlsTop,
                     geometry.close.left-6*ui,controlsTop+button};
    const float uptimeWidth=std::min(195*ui,width*0.52f);
    geometry.uptime={right-4*ui-uptimeWidth,geometry.title.top,right-4*ui,geometry.title.bottom};
    geometry.title.right=geometry.uptime.left-8*ui;
    return geometry;
}

void Switcher::DrawHoverControls(float t) {
    if (!m_settings.hoverControls || !m_hoverWindow) return;
    for (const auto& entry : m_drawList) {
        if (!entry.interactive || entry.item->hwnd != m_hoverWindow) continue;
        const auto geometry = GetHoverGeometry(entry);
        const float ui = geometry.uiScale;
        const float alpha = t * entry.item->hoverAmount * entry.pose.opacity;
        m_hoverBar = geometry.surface;
        m_pinButton = geometry.pin;
        m_closeButton = m_settings.showCloseButton ? geometry.close : D2D1_RECT_F{};
        m_killButton = m_settings.showCloseButton ? geometry.kill : D2D1_RECT_F{};
        POINT cursor{};
        D2D1_POINT_2F point{-1, -1};
        if (GetCursorPos(&cursor) && ScreenToClient(m_hwnd, &cursor)) point = LayoutPoint(cursor);
        const bool closeHovered = InCloseCircle(point, m_closeButton);
        const bool pinHovered = InRect(point, m_pinButton);
        const bool killHovered = InRect(point, m_killButton);
        const float ease = (float)(1 - std::exp(-20 * m_motionDt));
        entry.item->closeHoverAmount += ((closeHovered ? 1.0f : 0.0f) - entry.item->closeHoverAmount) * ease;
        entry.item->pinHoverAmount += ((pinHovered ? 1.0f : 0.0f) - entry.item->pinHoverAmount) * ease;
        entry.item->killHoverAmount += ((killHovered ? 1.0f : 0.0f) - entry.item->killHoverAmount) * ease;

        const float fontSize = m_settings.hoverTitleSize * ui;
        if (!m_hoverTextFormat || std::fabs(fontSize - m_hoverFontSize) > 0.2f) {
            m_hoverTextFormat = nullptr;
            m_controlSymbolFormat = nullptr;
            if (FAILED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(), nullptr,
                m_settings.fontWeight, m_settings.fontStyle, DWRITE_FONT_STRETCH_NORMAL,
                fontSize, L"", m_hoverTextFormat.put()))) return;
            m_hoverFontSize = fontSize;
            m_hoverTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            m_hoverTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            const DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
            com_ptr<IDWriteInlineObject> ellipsis;
            m_dwriteFactory->CreateEllipsisTrimmingSign(m_hoverTextFormat.get(), ellipsis.put());
            m_hoverTextFormat->SetTrimming(&trimming, ellipsis.get());
        }
        if (!m_controlSymbolFormat) {
            if (FAILED(m_dwriteFactory->CreateTextFormat(CoverTabbyIconFont(), nullptr,
                DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                13 * ui, L"", m_controlSymbolFormat.put()))) return;
            m_controlSymbolFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            m_controlSymbolFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            m_controlSymbolFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
        const auto foreground = m_settings.darkBackground ? D2D1::ColorF(1, 1, 1, alpha)
            : D2D1::ColorF(0.08f, 0.08f, 0.08f, alpha);
        m_brush->SetColor(foreground);
        m_ctx->DrawText(entry.item->title.c_str(), (UINT32)entry.item->title.size(),
            m_hoverTextFormat.get(), geometry.title, m_brush.get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        const std::wstring uptime=m_killConfirmWindow==entry.item->hwnd && NowSeconds()<m_killConfirmUntil
            ? Text(L"Click Kill again: unsaved work will be lost",L"Premi ancora Termina: perderai i dati non salvati")
            : NowSeconds()<m_killErrorUntil
            ? Text(L"Couldn't kill process",L"Impossibile uccidere il processo")
            : ProcessUptime(entry.item->processCreation,m_settings.italian);
        m_hoverTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        m_ctx->DrawText(uptime.c_str(),(UINT32)uptime.size(),m_hoverTextFormat.get(),
            geometry.uptime,m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
        m_hoverTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

        const bool pinned = m_pinned.contains(m_hoverWindow);
        const float radius = 8 * ui;
        m_brush->SetColor(StyleColor(m_settings.highlightColor,
            alpha * (pinned ? 0.8f : 0.12f + 0.2f * entry.item->pinHoverAmount)));
        m_ctx->FillRoundedRectangle({m_pinButton, radius, radius}, m_brush.get());
        m_brush->SetColor(StyleColor(m_settings.highlightColor, alpha * (pinned ? 1.0f : 0.55f)));
        m_ctx->DrawRoundedRectangle({m_pinButton, radius, radius}, m_brush.get(), ui);
        m_brush->SetColor(pinned ? D2D1::ColorF(1, 1, 1, alpha) : foreground);
        // Windows 11 Segoe Fluent Icons: Pin E718, Unpin E77A.
        const D2D1_RECT_F pinIcon{m_pinButton.left,m_pinButton.top,m_pinButton.left+30*ui,m_pinButton.bottom};
        m_ctx->DrawText(pinned ? L"\uE77A" : L"\uE718", 1, m_controlSymbolFormat.get(),
            pinIcon, m_brush.get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);

        if (m_settings.showCloseButton) {
            // Monochrome skull drawn as a vector, with WinUI-style hover fill.
            m_brush->SetColor(D2D1::ColorF(0.91f,0.12f,0.16f,alpha*(0.10f+0.42f*entry.item->killHoverAmount)));
            m_ctx->FillRoundedRectangle({m_killButton,8*ui,8*ui},m_brush.get());
            m_brush->SetColor(D2D1::ColorF(0.95f,0.22f,0.25f,alpha));
            m_ctx->DrawRoundedRectangle({m_killButton,8*ui,8*ui},m_brush.get(),ui);
            const float skullX=m_killButton.right-12*ui;
            const float skullY=(m_killButton.top+m_killButton.bottom)*0.5f-1*ui;
            m_brush->SetColor(D2D1::ColorF(1,0.75f,0.75f,alpha));
            m_ctx->FillEllipse({{skullX,skullY-1*ui},5.5f*ui,5*ui},m_brush.get());
            m_ctx->FillRoundedRectangle({{skullX-3.5f*ui,skullY+1*ui,skullX+3.5f*ui,skullY+6*ui},ui,ui},m_brush.get());
            auto hole=m_settings.backgroundColor; hole.a=alpha; m_brush->SetColor(hole);
            m_ctx->FillEllipse({{skullX-2.4f*ui,skullY},1.5f*ui,1.6f*ui},m_brush.get());
            m_ctx->FillEllipse({{skullX+2.4f*ui,skullY},1.5f*ui,1.6f*ui},m_brush.get());
            m_ctx->DrawLine({skullX,skullY+2*ui},{skullX,skullY+3*ui},m_brush.get(),ui);
            for (int tooth=-1;tooth<=1;++tooth)
                m_ctx->DrawLine({skullX+tooth*2*ui,skullY+4*ui},{skullX+tooth*2*ui,skullY+6*ui},m_brush.get(),0.7f*ui);
            const float cx = m_closeButton.right-12*ui;
            const float cy = (m_closeButton.top + m_closeButton.bottom) * 0.5f;
            const float radius = (m_closeButton.bottom - m_closeButton.top) * 0.5f;
            m_brush->SetColor(D2D1::ColorF(0.91f, 0.12f, 0.16f,
                alpha * (0.18f + 0.42f * entry.item->closeHoverAmount)));
            m_ctx->FillRoundedRectangle({m_closeButton,radius,radius},m_brush.get());
            m_brush->SetColor(D2D1::ColorF(0.95f, 0.22f, 0.25f, alpha));
            m_ctx->DrawRoundedRectangle({m_closeButton,radius,radius},m_brush.get(),1.2f*ui);
            const float r = 3.2f * ui;
            m_brush->SetColor(D2D1::ColorF(1, 0.65f, 0.65f, alpha));
            m_ctx->DrawLine({cx-r, cy-r}, {cx+r, cy+r}, m_brush.get(), 1.4f * ui);
            m_ctx->DrawLine({cx+r, cy-r}, {cx-r, cy+r}, m_brush.get(), 1.4f * ui);
        }
        // Labels live inside the expanding controls. Close stays anchored on
        // the right; Kill follows its left edge, keeping both on the same row.
        auto drawCaption=[&](const wchar_t* text,float progress,D2D1_RECT_F rect,bool danger) {
            if (progress<0.005f || rect.right<=rect.left) return;
            auto color=danger ? D2D1::ColorF(1,0.65f,0.65f,alpha*progress) : foreground;
            color.a=alpha*progress;
            m_brush->SetColor(color);
            m_hoverTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            m_ctx->DrawText(text,(UINT32)wcslen(text),m_hoverTextFormat.get(),rect,
                m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
            m_hoverTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        };
        drawCaption(pinned ? Text(L"Unpin",L"Sblocca") : Text(L"Pin",L"Fissa"),entry.item->pinHoverAmount,
            {geometry.pin.left+30*ui,geometry.pin.top,geometry.pin.right-4*ui,geometry.pin.bottom},false);
        if (m_settings.showCloseButton) {
            drawCaption(Text(L"Close",L"Chiudi"),entry.item->closeHoverAmount,
                {geometry.close.left+4*ui,geometry.close.top,geometry.close.right-24*ui,geometry.close.bottom},true);
            drawCaption(m_killConfirmWindow==entry.item->hwnd && NowSeconds()<m_killConfirmUntil ? Text(L"Confirm",L"Conferma") : Text(L"Kill",L"Termina"),entry.item->killHoverAmount,
                {geometry.kill.left+4*ui,geometry.kill.top,geometry.kill.right-24*ui,geometry.kill.bottom},true);
        }
        return;
    }
}
void Switcher::DrawHoverTitle(float t) {
    const float alpha=t*m_cursorTitleOpacity;
    if (alpha<0.005f || m_cursorTitle.empty()) return;
    if (!m_hoverTitleFormat) {
        if (FAILED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(), nullptr,
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            13*m_dpiScale, L"", m_hoverTitleFormat.put()))) return;
        m_hoverTitleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        m_hoverTitleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        const DWRITE_TRIMMING trim{DWRITE_TRIMMING_GRANULARITY_CHARACTER,0,0};
        com_ptr<IDWriteInlineObject> ellipsis;
        m_dwriteFactory->CreateEllipsisTrimmingSign(m_hoverTitleFormat.get(),ellipsis.put());
        m_hoverTitleFormat->SetTrimming(&trim,ellipsis.get());
    }
    const float margin=(m_settings.windowMode ? 32 : 8)*m_dpiScale;
    const float padding=10*m_dpiScale;
    const float maximum=std::max(1.0f,std::min(400*m_dpiScale,m_width-2*margin)-2*padding);
    if (!m_hoverTitleLayout) {
        if (FAILED(m_dwriteFactory->CreateTextLayout(m_cursorTitle.c_str(),(UINT32)m_cursorTitle.size(),
            m_hoverTitleFormat.get(),maximum,24*m_dpiScale,m_hoverTitleLayout.put()))) return;
    }
    DWRITE_TEXT_METRICS metrics{};
    m_hoverTitleLayout->GetMetrics(&metrics);
    const float width=std::min(maximum,metrics.widthIncludingTrailingWhitespace)+2*padding;
    const float height=30*m_dpiScale;
    const float left=std::clamp(m_cursorTitlePoint.x-width*0.5f,margin,std::max(margin,m_width-margin-width));
    const float top=std::clamp(m_cursorTitlePoint.y-height-12*m_dpiScale+
        4*m_dpiScale*(1-m_cursorTitleOpacity),margin,std::max(margin,m_height-margin-height));
    const D2D1_RECT_F rect{left,top,left+width,top+height};
    auto background=m_settings.backgroundColor;
    background.a=0.96f*alpha;
    m_brush->SetColor(background);
    m_ctx->FillRoundedRectangle({rect,6*m_dpiScale,6*m_dpiScale},m_brush.get());
    m_brush->SetColor(m_settings.darkBackground ? D2D1::ColorF(1,1,1,0.18f*alpha)
        : D2D1::ColorF(0,0,0,0.18f*alpha));
    m_ctx->DrawRoundedRectangle({rect,6*m_dpiScale,6*m_dpiScale},m_brush.get(),m_dpiScale);
    m_brush->SetColor(m_settings.darkBackground ? D2D1::ColorF(1,1,1,alpha)
        : D2D1::ColorF(0.08f,0.08f,0.08f,alpha));
    m_ctx->DrawTextLayout({left+padding,top+3*m_dpiScale},m_hoverTitleLayout.get(),
        m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void Switcher::UpdateDesktopInfo(double now) {
    if (m_items.empty()) { m_desktopButton={}; m_desktopMenuOpen=false; return; }
    if(m_items[SelectedIndex()]->desktopPlaceholder) {
        m_desktopButton={};m_desktopMenuOpen=false;m_desktopKnown=false;
        m_desktopInfoWindow=nullptr;return;
    }
    const HWND window=m_items[SelectedIndex()]->hwnd;
    if (window!=m_desktopInfoWindow) {
        m_desktopInfoWindow=window;
        m_desktopRefreshAt=0;
        m_desktopMenuOpen=false;
        m_desktopErrorUntil=0;
    }
    if (now<m_desktopRefreshAt) return;
    m_desktopRefreshAt=now+1;
    const HRESULT result=m_desktopBridge.Refresh();
    m_desktopKnown=SUCCEEDED(result) && SUCCEEDED(m_desktopBridge.WindowDesktop(window,&m_windowDesktopId));
    if (!m_desktopKnown) m_desktopMenuOpen=false;
}

void Switcher::DrawDesktopPicker(float t) {
    m_desktopButton={}; m_desktopRows.clear();
    if (m_items.empty() || (m_searchActive && m_searchMatches.empty())) return;
    const int selected=SelectedIndex();
    if(m_items[selected]->desktopPlaceholder) return;
    const DrawEntry* selectedEntry=nullptr;
    for (const auto& entry:m_drawList) {
        if (entry.interactive && entry.item==m_items[selected].get()) { selectedEntry=&entry; break; }
    }
    if (!selectedEntry || selectedEntry->pose.opacity<0.5f) return;
    const float hover=selectedEntry->item->hoverAmount;
    if (hover<0.005f && !m_desktopMenuOpen) return;
    const auto geometry=GetHoverGeometry(*selectedEntry);
    const float ui=geometry.uiScale;
    const float fontSize=12*ui;
    if (!m_desktopTextFormat || std::fabs(fontSize-m_desktopFontSize)>0.2f) {
        m_desktopTextFormat=nullptr;
        if (FAILED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,
            fontSize,L"",m_desktopTextFormat.put()))) return;
        m_desktopFontSize=fontSize;
        m_desktopTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        m_desktopTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_desktopTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        const DWRITE_TRIMMING trim{DWRITE_TRIMMING_GRANULARITY_CHARACTER,0,0};
        com_ptr<IDWriteInlineObject> ellipsis;
        m_dwriteFactory->CreateEllipsisTrimmingSign(m_desktopTextFormat.get(),ellipsis.put());
        m_desktopTextFormat->SetTrimming(&trim,ellipsis.get());
    }
    std::wstring label=Text(L"Desktop unavailable",L"Desktop non disponibile");
    if (m_desktopKnown) {
        label=Text(L"Desktop unavailable",L"Desktop non disponibile");
        for (const auto& desktop:m_desktopBridge.entries)
            if (IsEqualGUID(desktop.id,m_windowDesktopId)) { label=desktop.name; break; }
    }
    if (NowSeconds()<m_desktopErrorUntil) label=Text(L"Couldn't move window",L"Impossibile spostare la finestra");
    const float center=(geometry.surface.left+geometry.surface.right)*0.5f;
    const float width=std::max(24*ui,std::min({160*ui,2*(center-geometry.pin.right-8*ui),2*(geometry.kill.left-center-8*ui)}));
    const D2D1_RECT_F button={center-width*0.5f,geometry.pin.top,center+width*0.5f,geometry.pin.bottom};
    // Invisible/fading-out controls must not intercept clicks.
    if (hover>0.15f || m_desktopMenuOpen) m_desktopButton=button;
    POINT cursor{}; D2D1_POINT_2F point{-1,-1};
    if (GetCursorPos(&cursor) && ScreenToClient(m_hwnd,&cursor)) point=LayoutPoint(cursor);
    const float alpha=t*selectedEntry->pose.opacity*hover;
    const bool hovered=InRect(point,button);
    const auto green=m_settings.darkBackground ? D2D1::ColorF(0.25f,0.86f,0.45f,alpha)
        : D2D1::ColorF(0.05f,0.48f,0.20f,alpha);
    auto fill=green; fill.a=alpha*(hovered || m_desktopMenuOpen ? 0.24f : 0.12f);
    m_brush->SetColor(fill);
    m_ctx->FillRoundedRectangle({button,8*ui,8*ui},m_brush.get());
    m_brush->SetColor(green);
    m_ctx->DrawRoundedRectangle({button,8*ui,8*ui},m_brush.get(),ui);
    const float iconX=button.left+7*ui, iconY=(button.top+button.bottom)*0.5f;
    const D2D1_RECT_F screen={iconX,iconY-5*ui,iconX+14*ui,iconY+3*ui};
    m_ctx->DrawRoundedRectangle({screen,1.5f*ui,1.5f*ui},m_brush.get(),ui);
    m_ctx->DrawLine({iconX+7*ui,iconY+3*ui},{iconX+7*ui,iconY+6*ui},m_brush.get(),ui);
    m_ctx->DrawLine({iconX+3*ui,iconY+6*ui},{iconX+11*ui,iconY+6*ui},m_brush.get(),ui);
    const D2D1_RECT_F labelRect={button.left+26*ui,button.top,button.right-6*ui,button.bottom};
    m_ctx->DrawText(label.c_str(),(UINT32)label.size(),m_desktopTextFormat.get(),
        labelRect,m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
    if (m_desktopMenuAmount<0.005f || m_desktopBridge.entries.empty()) return;
    const float menuAlpha=alpha*m_desktopMenuAmount;
    const float margin=(m_settings.windowMode ? 32 : 8)*m_dpiScale;
    const float rowHeight=30*ui, pad=6*ui;
    const int fit=std::max(1,(int)((button.top-margin-2*pad-8*ui)/rowHeight));
    const int visible=std::min({6,fit,(int)m_desktopBridge.entries.size()});
    m_desktopScroll=std::clamp(m_desktopScroll,0,(int)m_desktopBridge.entries.size()-visible);
    const float menuWidth=std::min(220*ui,m_width-2*margin);
    const float left=std::clamp(center-menuWidth*0.5f,margin,std::max(margin,m_width-margin-menuWidth));
    const float bottom=button.top-8*ui+6*ui*(1-m_desktopMenuAmount);
    m_desktopMenuRect={left,bottom-visible*rowHeight-2*pad,left+menuWidth,bottom};
    auto background=m_settings.backgroundColor; background.a=0.98f*menuAlpha;
    m_brush->SetColor(background);
    m_ctx->FillRoundedRectangle({m_desktopMenuRect,8*ui,8*ui},m_brush.get());
    auto border=green; border.a=0.65f*menuAlpha; m_brush->SetColor(border);
    m_ctx->DrawRoundedRectangle({m_desktopMenuRect,8*ui,8*ui},m_brush.get(),ui);
    for (int row=0;row<visible;++row) {
        const auto& desktop=m_desktopBridge.entries[m_desktopScroll+row];
        const D2D1_RECT_F rect{left+pad,m_desktopMenuRect.top+pad+row*rowHeight,
            left+menuWidth-pad,m_desktopMenuRect.top+pad+(row+1)*rowHeight};
        if (m_desktopMenuOpen && m_desktopMenuAmount>0.5f) m_desktopRows.emplace_back(desktop.id,rect);
        const bool current=IsEqualGUID(desktop.id,m_windowDesktopId);
        if (current || InRect(point,rect)) {
            auto color=green; color.a=(current ? 0.23f : 0.12f)*menuAlpha;
            m_brush->SetColor(color);
            m_ctx->FillRoundedRectangle({rect,5*ui,5*ui},m_brush.get());
        }
        m_brush->SetColor(current ? D2D1::ColorF(green.r,green.g,green.b,menuAlpha)
            : (m_settings.darkBackground ? D2D1::ColorF(1,1,1,menuAlpha) : D2D1::ColorF(0.08f,0.08f,0.08f,menuAlpha)));
        const std::wstring text=current ? desktop.name+L"  \u2713" : desktop.name;
        m_ctx->DrawText(text.c_str(),(UINT32)text.size(),m_desktopTextFormat.get(),rect,
            m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }
}

void Switcher::UpdateSearch() {
    m_searchFrom.clear();
    for(const auto& entry:m_drawList) m_searchFrom[entry.item->hwnd]=entry.pose;
    m_searchTransitionStart=NowSeconds();
    m_hoverSelectionArmed=false; m_hoverCandidate=nullptr;
    POINT searchCursor{};
    if(GetCursorPos(&searchCursor) && ScreenToClient(m_hwnd,&searchCursor)) m_lastHoverCursor=searchCursor;
    m_searchMatches.clear();
    m_searchMask.assign(m_items.size(),0);
    std::wstring query=m_searchQuery;
    if(!query.empty()) CharLowerBuffW(query.data(),(DWORD)query.size());
    for(int i=0;i<(int)m_items.size();++i) {
        auto& item=*m_items[i];
        if(item.searchableText.empty()||item.searchableTitle!=item.title){
            item.searchableTitle=item.title;item.searchableText=item.title+L" "+item.appKey;
            CharLowerBuffW(item.searchableText.data(),(DWORD)item.searchableText.size());
        }
        if(query.empty() || item.searchableText.find(query)!=std::wstring::npos) {
            m_searchMatches.push_back(i);
            m_searchMask[i]=1;
        }
    }
    g_searchHasResults=!m_searchMatches.empty();
    if(!m_searchMatches.empty()) {m_target=m_searchMatches.front();m_scroll=(double)m_target;m_iconScroll=m_scroll;m_velocity=0;m_groupScroll=0;}
    m_titleIndex=-1; m_titleLayout=nullptr;
}

com_ptr<ID2D1Bitmap1> Switcher::LoadWallpaperPreview(std::wstring path) {
    const bool showDesktop=path.empty();
    if(path.empty()) {
        wchar_t fallback[32768]{};
        if(SystemParametersInfoW(SPI_GETDESKWALLPAPER,32768,fallback,0)) path=fallback;
    }
    WIN32_FILE_ATTRIBUTE_DATA attributes{};
    GetFileAttributesExW(path.c_str(),GetFileExInfoStandard,&attributes);
    const auto stamp=((ULONGLONG)attributes.ftLastWriteTime.dwHighDateTime<<32)|attributes.ftLastWriteTime.dwLowDateTime;
    const auto key=path+L"|"+std::to_wstring(stamp);
    if(showDesktop && m_showDesktopWallpaper && key==m_showDesktopWallpaperKey) return m_showDesktopWallpaper;
    com_ptr<ID2D1Bitmap1> bitmap;
    com_ptr<IWICBitmapDecoder> decoder;com_ptr<IWICBitmapFrameDecode> frame;
    com_ptr<IWICBitmapScaler> scaler;com_ptr<IWICFormatConverter> converter;
    UINT width=0,height=0;
    if(!path.empty() && SUCCEEDED(m_wicFactory->CreateDecoderFromFilename(path.c_str(),nullptr,GENERIC_READ,
        WICDecodeMetadataCacheOnDemand,decoder.put())) && SUCCEEDED(decoder->GetFrame(0,frame.put())) &&
        SUCCEEDED(frame->GetSize(&width,&height)) && width && height) {
        const float fit=std::min(1.0f,640.0f/std::max(width,height));
        if(SUCCEEDED(m_wicFactory->CreateBitmapScaler(scaler.put())) && SUCCEEDED(scaler->Initialize(frame.get(),
            std::max(1U,(UINT)(width*fit)),std::max(1U,(UINT)(height*fit)),WICBitmapInterpolationModeFant)) &&
            SUCCEEDED(m_wicFactory->CreateFormatConverter(converter.put())) && SUCCEEDED(converter->Initialize(scaler.get(),
                GUID_WICPixelFormat32bppPBGRA,WICBitmapDitherTypeNone,nullptr,0,WICBitmapPaletteTypeMedianCut)))
            m_ctx->CreateBitmapFromWicBitmap(converter.get(),nullptr,bitmap.put());
    }
    if(showDesktop) {m_showDesktopWallpaper=bitmap;m_showDesktopWallpaperKey=key;}
    return bitmap;
}

D2D1_COLOR_F Switcher::UiForeground(float opacity) const {
    return m_settings.darkBackground ? D2D1::ColorF(.98f,.98f,1,opacity) : D2D1::ColorF(.06f,.06f,.08f,opacity);
}
D2D1_COLOR_F Switcher::UiSurface(float opacity,float defaultTint) const {
    float tint=defaultTint;
    if(m_settings.enchantedMaterial==3) tint=m_settings.energySaver ? .94f : (m_settings.darkBackground ? .36f : .42f);
    else if(m_settings.enchantedMaterial==1 || m_settings.enchantedMaterial==2) tint=.94f;
    return m_settings.darkBackground ? D2D1::ColorF(.12f,.12f,.14f,opacity*tint) : D2D1::ColorF(.95f,.95f,.97f,opacity*tint);
}
void Switcher::DrawUiPanel(D2D1_RECT_F rect,float radius,float opacity,float defaultTint,bool backdrop) {
    if(opacity<=.001f || rect.right<=rect.left || rect.bottom<=rect.top) return;
    if(backdrop && m_background) {
        const float width=rect.right-rect.left,height=rect.bottom-rect.top;
        auto found=std::find_if(m_uiPanelMasks.begin(),m_uiPanelMasks.end(),[&](const auto& mask) {
            return std::abs(mask.width-width)<.1f && std::abs(mask.height-height)<.1f && std::abs(mask.radius-radius)<.1f;
        });
        if(found==m_uiPanelMasks.end()) {
            UiPanelMask mask{width,height,radius,{}};
            if(SUCCEEDED(m_d2dFactory->CreateRoundedRectangleGeometry({{0,0,width,height},radius,radius},mask.geometry.put()))) {
                if(m_uiPanelMasks.size()>=24) m_uiPanelMasks.erase(m_uiPanelMasks.begin());
                m_uiPanelMasks.push_back(std::move(mask));found=std::prev(m_uiPanelMasks.end());
            }
        }
        if(found!=m_uiPanelMasks.end()) {
            auto layer=D2D1::LayerParameters1();layer.geometricMask=found->geometry.get();
            layer.maskTransform=D2D1::Matrix3x2F::Translation(rect.left,rect.top);
            m_ctx->PushLayer(layer,nullptr);
            m_ctx->DrawBitmap(m_background.get(),{0,0,m_width,m_height},opacity,D2D1_INTERPOLATION_MODE_LINEAR);
            m_ctx->PopLayer();
        }
    }
    m_brush->SetColor(UiSurface(opacity,backdrop && m_background ? defaultTint : 1.f));
    m_ctx->FillRoundedRectangle({rect,radius,radius},m_brush.get());
}
void Switcher::DrawAnimatedUiText(UiTextAnimation& state,const std::wstring& text,IDWriteTextFormat* format,
    D2D1_RECT_F rect,float opacity,bool selected,bool caret,bool placeholder) {
    if(!format || opacity<=.001f || rect.right<=rect.left || rect.bottom<=rect.top) return;
    const float ui=m_dpiScale,height=rect.bottom-rect.top;
    const bool centered=format->GetTextAlignment()==DWRITE_TEXT_ALIGNMENT_CENTER;
    const float width=centered ? rect.right-rect.left : 10000.f;
    if(!state.layout || state.text!=text || state.format.get()!=format || state.width!=width || state.height!=height || state.dark!=m_settings.darkBackground) {
        size_t prefix=0;
        if(state.format.get()==format && state.dark==m_settings.darkBackground)
            while(prefix<text.size() && prefix<state.text.size() && text[prefix]==state.text[prefix]) ++prefix;
        auto previous=std::move(state.glyphs);state.glyphs.clear();state.layout=nullptr;
        state.changed=NowSeconds();state.text=text;state.format.copy_from(format);state.width=width;state.height=height;state.dark=m_settings.darkBackground;
        if(FAILED(m_dwriteFactory->CreateTextLayout(text.c_str(),(UINT32)text.size(),format,width,height,state.layout.put()))) return;
        state.layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        UINT32 count=0;if(&state!=&m_searchTextAnimation||m_settings.typingAnimation)state.layout->GetClusterMetrics(nullptr,0,&count);
        std::vector<DWRITE_CLUSTER_METRICS> clusters(count);
        if(count && SUCCEEDED(state.layout->GetClusterMetrics(clusters.data(),count,&count))) {
            com_ptr<ID2D1Image> target;m_ctx->GetTarget(target.put());
            D2D1_MATRIX_3X2_F transform;m_ctx->GetTransform(&transform);m_ctx->SetTransform(D2D1::Matrix3x2F::Identity());
            UINT32 position=0;
            for(const auto& cluster:clusters) {
                UiTextAnimation::Glyph glyph;glyph.start=position;glyph.length=cluster.length;
                float x=0,y=0;DWRITE_HIT_TEST_METRICS hit{};
                state.layout->HitTestTextPosition(position,FALSE,&x,&y,&hit);glyph.left=x;glyph.width=cluster.width;
                const auto old=std::find_if(previous.begin(),previous.end(),[&](const auto& entry){return entry.start==position;});
                glyph.born=position<prefix && old!=previous.end() ? old->born : state.changed+.012*std::min<size_t>(state.glyphs.size(),8);
                if(m_settings.animationDurationMs>1 && cluster.width>0 && SUCCEEDED(m_ctx->CreateCommandList(glyph.image.put()))) {
                    m_ctx->SetTarget(glyph.image.get());
                    m_ctx->PushAxisAlignedClip({x,-24*ui,x+cluster.width,height+24*ui},D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                    m_brush->SetColor(UiForeground(1));m_ctx->DrawTextLayout({0,0},state.layout.get(),m_brush.get());
                    m_ctx->PopAxisAlignedClip();m_ctx->SetTarget(target.get());glyph.image->Close();
                }
                state.glyphs.push_back(std::move(glyph));position+=cluster.length;
            }
            m_ctx->SetTarget(target.get());m_ctx->SetTransform(transform);
        }
    }
    DWRITE_TEXT_METRICS metrics{};state.layout->GetMetrics(&metrics);
    const float scroll=centered || placeholder ? 0 : std::max(0.f,metrics.widthIncludingTrailingWhitespace-(rect.right-rect.left)+2*ui);
    const D2D1_POINT_2F origin{rect.left-scroll,rect.top};
    m_ctx->PushAxisAlignedClip(rect,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    if(selected && !text.empty()) {
        m_brush->SetColor(StyleColor(m_settings.highlightColor,.3f*opacity));
        m_ctx->FillRoundedRectangle({{origin.x+metrics.left,origin.y+metrics.top,origin.x+metrics.left+metrics.widthIncludingTrailingWhitespace,origin.y+metrics.top+metrics.height},3*ui,3*ui},m_brush.get());
    }
    const double now=NowSeconds();
    const float alpha=opacity*(placeholder ? .55f : 1.f);
    const bool settled=m_settings.animationDurationMs<=1 || std::all_of(state.glyphs.begin(),state.glyphs.end(),[&](const auto& glyph){return now-glyph.born>=.28;});
    if(settled) {
        // Once typing settles, the entire shaped text takes one cached draw.
        m_brush->SetColor(UiForeground(alpha));m_ctx->DrawTextLayout(origin,state.layout.get(),m_brush.get());
    } else {
        if(!m_uiTextBlur) m_ctx->CreateEffect(CLSID_D2D1GaussianBlur,m_uiTextBlur.put());
        for(auto& glyph:state.glyphs) {
            const float progress=std::clamp((float)((now-glyph.born)/.28),0.f,1.f);
            const float glyphAlpha=alpha*EaseOutQuart(progress);
            if(glyphAlpha<=.001f || !glyph.image) continue;
            const float offset=-15*ui*std::exp(-7*progress)*std::cos(9*progress)*(1-progress);
            const D2D1_POINT_2F shifted{origin.x,origin.y+offset};
            m_ctx->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(),nullptr,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,D2D1::IdentityMatrix(),glyphAlpha),nullptr);
            if(progress<1 && m_uiTextBlur) {
                m_uiTextBlur->SetInput(0,glyph.image.get());
                m_uiTextBlur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,2.2f*ui*(1-EaseOutQuart(progress)));
                m_ctx->DrawImage(m_uiTextBlur.get(),shifted);
            } else m_ctx->DrawImage(glyph.image.get(),shifted);
            m_ctx->PopLayer();
        }
    }
    if(caret) {
        float x=0,y=0;DWRITE_HIT_TEST_METRICS hit{};
        state.layout->HitTestTextPosition(placeholder ? 0 : (UINT32)text.size(),FALSE,&x,&y,&hit);
        const float caretAlpha=.65f+.35f*(float)std::cos((now-state.changed)*5);
        m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity*caretAlpha));
        m_ctx->DrawLine({origin.x+x,origin.y+y+2*ui},{origin.x+x,origin.y+y+std::max(hit.height,14*ui)-2*ui},m_brush.get(),1.3f*ui);
    }
    m_ctx->PopAxisAlignedClip();
}


void Switcher::DrawSelectionBar(float opacity) {
    if(!m_settings.selectionBar||m_desktopOverview||opacity<.005f)return;
    const bool filtered=m_searchActive||m_settings.simpleStyle>=4;
    const auto selection=CoverTabbyPosition::summarize(m_items,m_searchMatches,filtered,SelectedIndex());
    const double position=m_positionMotion.update(selection.rank,selection.count,m_motionDt,m_settings.animateSelection);
    const float ui=m_dpiScale;
    const auto box=CoverTabbyPosition::bounds(m_width,m_height,ui,m_settings.windowMode&&m_settings.simpleStyle==0);
    if(box.right-box.left<32*ui||box.labelTop<0)return;
    const float fontSize=12*ui;
    if(!m_positionFormat||std::fabs(m_positionFontSize-fontSize)>.1f){
        m_positionFormat=nullptr;
        if(FAILED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,
            fontSize,L"",m_positionFormat.put())))return;
        m_positionFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_positionFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        m_positionFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        m_positionFontSize=fontSize;
    }
    if(m_positionTextCount!=selection.total){
        m_positionTextCount=selection.total;
        m_positionLabel=std::to_wstring(selection.total)+(selection.total==1 ?
            Text(L" window open",L" finestra aperta") : Text(L" windows open",L" finestre aperte"));
    }
    auto ink=UiForeground(opacity*.76f);m_brush->SetColor(ink);
    m_ctx->DrawText(m_positionLabel.c_str(),(UINT32)m_positionLabel.size(),m_positionFormat.get(),
        {box.left-24*ui,box.labelTop,box.right+24*ui,box.barTop-5*ui},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
    m_brush->SetColor(m_settings.darkBackground ? D2D1::ColorF(1,1,1,.16f*opacity) : D2D1::ColorF(0,0,0,.18f*opacity));
    m_ctx->FillRoundedRectangle({{box.left,box.barTop,box.right,box.barBottom},2*ui,2*ui},m_brush.get());
    if(position<0)return;
    const float thumbWidth=std::min(22*ui,box.right-box.left);
    const float x=box.left+(box.right-box.left-thumbWidth)*(float)position;
    m_brush->SetColor(StyleColor(m_settings.highlightColor,.95f*opacity));
    m_ctx->FillRoundedRectangle({{x,box.barTop,x+thumbWidth,box.barBottom},2*ui,2*ui},m_brush.get());
}

void Switcher::DrawWindowCount(float opacity,D2D1_RECT_F rect,int found,int total,bool forceDark) {
    const float ui=m_dpiScale;
    if(!m_countFormat) {
        m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,12*ui,L"",m_countFormat.put());
        if(!m_countFormat) return;
        m_countFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        m_countFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    m_brush->SetColor((forceDark || m_settings.darkBackground) ? D2D1::ColorF(.9f,.9f,.94f,.8f*opacity) : D2D1::ColorF(.1f,.1f,.13f,.85f*opacity));
    m_countFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    if(!found) {
        const auto empty=Text(L"No windows found",L"Nessuna finestra trovata");
        m_ctx->DrawText(empty,(UINT32)wcslen(empty),m_countFormat.get(),rect,m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
        m_countFound=0;m_countFrom.clear();m_countTo.clear();return;
    }
    const std::wstring digits=std::to_wstring(found);
    const double now=NowSeconds();
    const double previousProgress=std::clamp((now-m_countBirth)/.46,0.0,1.0);
    const double previousEase=1-std::pow(1-previousProgress,4);
    if(found!=m_countFound) {
        std::vector<double> previous=m_countTo;
        for(size_t i=0;i<previous.size();++i) previous[i]=m_countFrom[i]+(m_countTo[i]-m_countFrom[i])*previousEase;
        const bool initial=m_countFound<0;
        m_countFrom.resize(digits.size());m_countTo.resize(digits.size());
        for(size_t i=0;i<digits.size();++i) {
            const int digit=digits[i]-L'0';
            const int oldIndex=(int)previous.size()-(int)digits.size()+(int)i;
            const double from=oldIndex>=0 ? previous[oldIndex] : 0;
            m_countFrom[i]=initial ? digit : from;
            double destination=std::floor(from/10)*10+digit;
            if(destination<from-.001) destination+=10;
            m_countTo[i]=destination;
        }
        m_countBirth=now;m_countFound=found;
    }
    const std::wstring suffix=Text(L" windows found out of ",L" finestre trovate su ")+std::to_wstring(total);
    m_countFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    if(!m_countSuffixLayout || suffix!=m_countSuffixText || std::abs(m_countSuffixHeight-(rect.bottom-rect.top))>.1f) {
        m_countSuffixLayout=nullptr;
        if(FAILED(m_dwriteFactory->CreateTextLayout(suffix.c_str(),(UINT32)suffix.size(),m_countFormat.get(),1000,rect.bottom-rect.top,m_countSuffixLayout.put()))) return;
        DWRITE_TEXT_METRICS metrics{};m_countSuffixLayout->GetMetrics(&metrics);
        m_countSuffixWidth=metrics.widthIncludingTrailingWhitespace;m_countSuffixText=suffix;m_countSuffixHeight=rect.bottom-rect.top;
    }
    const float digitWidth=7.5f*ui,rollHeight=18*ui;
    const float start=std::max(rect.left,rect.right-m_countSuffixWidth-digitWidth*digits.size());
    m_ctx->PushAxisAlignedClip(rect,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    const double progress=std::clamp((now-m_countBirth)/.46,0.0,1.0),ease=1-std::pow(1-progress,4);
    for(size_t i=0;i<digits.size();++i) {
        const double position=m_countFrom[i]+(m_countTo[i]-m_countFrom[i])*ease;
        const int base=(int)std::floor(position);const float fraction=(float)(position-base);
        const D2D1_RECT_F cell{start+i*digitWidth,(rect.top+rect.bottom-rollHeight)*.5f,start+(i+1)*digitWidth,(rect.top+rect.bottom+rollHeight)*.5f};
        m_ctx->PushAxisAlignedClip(cell,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        for(int row=0;row<2;++row) {
            const wchar_t digit=L'0'+(base+row)%10;const float shift=(row-fraction)*rollHeight;
            m_ctx->DrawText(&digit,1,m_countFormat.get(),{cell.left,cell.top+shift,cell.right,cell.bottom+shift},m_brush.get());
        }
        m_ctx->PopAxisAlignedClip();
    }
    m_ctx->DrawTextLayout({start+digits.size()*digitWidth,rect.top},m_countSuffixLayout.get(),m_brush.get());
    m_ctx->PopAxisAlignedClip();
}

void Switcher::DrawSearchStyle(float opacity) {
    const float ui=m_dpiScale;
    const bool enchanted=m_settings.simpleStyle==5;
    const int matches=(int)m_searchMatches.size();
    const float desiredWidth=std::min(std::max(470.f,(float)std::min(enchanted ? 6 : 3,std::max(1,matches))*225.f)*ui,m_width-40*ui);
    const int columns=std::max(1,std::min(enchanted ? 6 : 3,(int)(desiredWidth/(225*ui))));
    const int capacity=columns*std::max(1,std::min(3,(int)((m_height-160*ui)/(170*ui))));
    const int visible=std::min(capacity,matches);
    const int rows=std::max(1,(visible+columns-1)/columns);
    const float header=(enchanted ? 12.f : 66.f)*ui;
    const float desiredHeight=(enchanted ? 24.f : 76.f)*ui+rows*170*ui;
    if(m_resultPanelWidth<=0) {m_resultPanelWidth=desiredWidth;m_resultPanelHeight=desiredHeight;}
    const float resizeBlend=(float)(1-std::exp(-18*m_motionDt));
    m_resultPanelWidth+=(desiredWidth-m_resultPanelWidth)*resizeBlend;
    m_resultPanelHeight+=(desiredHeight-m_resultPanelHeight)*resizeBlend;
    const float width=m_resultPanelWidth,height=m_resultPanelHeight;
    const float left=(m_width-width)*.5f,top=std::max((enchanted ? 90.f : 20.f)*ui,(m_height-height)*.5f);
    const D2D1_RECT_F panel{left,top,left+width,top+height};
    const float panelRadius=(enchanted ? 24.f : 12.f)*ui;
    DrawUiPanel(panel,panelRadius,opacity,enchanted ? .92f : .86f);
    const bool dark=m_settings.darkBackground;
    const auto ink=UiForeground(opacity);
    if(!m_searchFormat) {
        m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,15*ui,L"",m_searchFormat.put());
        if(!m_searchFormat) return;
        m_searchFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        m_searchFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    if(!m_simpleFormat) {
        m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,12*ui,L"",m_simpleFormat.put());
        if(!m_simpleFormat) return;
        m_simpleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        m_simpleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    const float searchWidth=std::min(width-24*ui,760*ui);
    const float searchLeft=(m_width-searchWidth)*.5f;
    const float searchTop=enchanted ? top-62*ui-24*ui*(1-EaseOutQuart(std::clamp(opacity,0.f,1.f))) : top+12*ui;
    m_searchRect={searchLeft,searchTop,searchLeft+searchWidth,searchTop+42*ui};
    if(enchanted) DrawUiPanel(m_searchRect,10*ui,opacity,.92f);
    m_brush->SetColor(dark ? D2D1::ColorF(1,1,1,.08f*opacity) : D2D1::ColorF(0,0,0,.06f*opacity));
    m_ctx->FillRoundedRectangle({m_searchRect,8*ui,8*ui},m_brush.get());
    m_brush->SetColor(StyleColor(m_settings.highlightColor,.8f*opacity));
    m_ctx->DrawLine({m_searchRect.left+8*ui,m_searchRect.bottom},{m_searchRect.right-8*ui,m_searchRect.bottom},m_brush.get(),2*ui);
    m_brush->SetColor(ink);
    const float sx=m_searchRect.left+17*ui,sy=(m_searchRect.top+m_searchRect.bottom)*.5f;
    m_ctx->DrawEllipse({{sx,sy-2*ui},5*ui,5*ui},m_brush.get(),1.5f*ui);
    m_ctx->DrawLine({sx+4*ui,sy+2*ui},{sx+8*ui,sy+6*ui},m_brush.get(),1.5f*ui);
    const float countWidth=std::min(210*ui,width*.43f);
    DrawWindowCount(opacity,{m_searchRect.right-countWidth,m_searchRect.top,m_searchRect.right-10*ui,m_searchRect.bottom},matches,(int)m_items.size());
    const auto text=m_searchQuery.empty() ? std::wstring(Text(L"Search windows...",L"Cerca finestre...")) : m_searchQuery;
    DrawAnimatedUiText(m_searchTextAnimation,text,m_searchFormat.get(),
        {m_searchRect.left+34*ui,m_searchRect.top+1,m_searchRect.right-countWidth-10*ui,m_searchRect.bottom-1},
        opacity,m_searchSelectAll && !m_searchQuery.empty(),m_searchActive,m_searchQuery.empty());
    m_ctx->PushAxisAlignedClip(panel,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    m_iconRects.assign(m_items.size(),{});m_simpleCloseRects.assign(m_items.size(),{});
    m_simplePinRects.assign(m_items.size(),{});m_enchantedHover.resize(m_items.size(),0.f);
    POINT mouse{};GetCursorPos(&mouse);ScreenToClient(m_hwnd,&mouse);const auto pointer=LayoutPoint(mouse);
    int position=0;
    const auto selected=std::find(m_searchMatches.begin(),m_searchMatches.end(),SelectedIndex());
    if(selected!=m_searchMatches.end()) position=(int)(selected-m_searchMatches.begin());
    const int first=matches>capacity ? std::clamp((position/columns)*columns,0,std::max(0,matches-capacity)) : 0;
    const float baseCardWidth=(width-24*ui-(columns-1)*8*ui)/columns;
    m_simpleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    for(int slot=0;slot<visible;++slot) {
        const int index=m_searchMatches[first+slot];auto& item=*m_items[index];
        float cardWidth=baseCardWidth;
        float x=left+12*ui+(slot%columns)*(cardWidth+8*ui);
        const float y=top+header+(slot/columns)*170*ui;
        if(enchanted) {
            const int rowStart=(slot/columns)*columns,rowEnd=std::min(visible,rowStart+columns);
            float rowWidth=0,preceding=0;
            for(int j=rowStart;j<rowEnd;++j) {
                const auto& candidate=*m_items[m_searchMatches[first+j]];
                const float aspect=std::max(1L,candidate.rect.right-candidate.rect.left)/(float)std::max(1L,candidate.rect.bottom-candidate.rect.top);
                const float desired=std::clamp(128*ui*aspect+12*ui,140*ui,baseCardWidth*1.5f);
                rowWidth+=desired;if(j<slot) preceding+=desired;if(j==slot) cardWidth=desired;
            }
            const float gaps=(rowEnd-rowStart-1)*8*ui;
            const float scale=std::min(1.f,(width-24*ui-gaps)/rowWidth);
            x=left+(width-rowWidth*scale-gaps)*.5f+preceding*scale+(slot-rowStart)*8*ui;cardWidth*=scale;
        }
        const D2D1_RECT_F card{x,y,x+cardWidth,y+160*ui};
        m_iconRects[index]={std::max(card.left,panel.left),std::max(card.top,panel.top),std::min(card.right,panel.right),std::min(card.bottom,panel.bottom)};
        if(m_iconRects[index].bottom<=m_iconRects[index].top) m_iconRects[index]={};
        item.tileSelectionAmount+=((index==SelectedIndex() ? 1.f : 0.f)-item.tileSelectionAmount)*(float)(1-std::exp(-22*m_motionDt));
        m_brush->SetColor(enchanted ? (dark ? D2D1::ColorF(1,1,1,.035f*opacity) : D2D1::ColorF(0,0,0,.035f*opacity)) : StyleColor(m_settings.highlightColor,.12f*opacity+.15f*opacity*item.tileSelectionAmount));
        m_ctx->FillRoundedRectangle({card,6*ui,6*ui},m_brush.get());
        if(!enchanted && item.tileSelectionAmount>.01f) {
            m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity*item.tileSelectionAmount));
            m_ctx->DrawRoundedRectangle({card,6*ui,6*ui},m_brush.get(),2*ui);
        }
        if(item.icon) m_ctx->DrawBitmap(item.icon.get(),{x+6*ui,y+6*ui,x+22*ui,y+22*ui},opacity,D2D1_INTERPOLATION_MODE_LINEAR);
        m_brush->SetColor(StyleColor(ink,opacity*(item.minimized ? .6f : 1.f)));
        const D2D1_RECT_F titleBox{x,y,x+cardWidth,y+28*ui};
        const bool titleHovered=enchanted && InRect(pointer,panel) && InRect(pointer,titleBox);
        float& hover=m_enchantedHover[index];hover+=((titleHovered ? 1.f : 0.f)-hover)*(float)(1-std::exp(-22*m_motionDt));
        const float controlsWidth=std::min(142*ui,cardWidth-12*ui);
        if(enchanted && hover>.01f) {
            const float buttonWidth=controlsWidth*.5f;
            const D2D1_RECT_F close{x+cardWidth-controlsWidth-4*ui,y+2*ui,x+cardWidth-buttonWidth-4*ui,y+26*ui};
            const D2D1_RECT_F pin{close.right,y+2*ui,x+cardWidth-4*ui,y+26*ui};
            if(titleHovered) {m_simpleCloseRects[index]=close;m_simplePinRects[index]=pin;}
            if(!m_enchantedSymbols) {
                m_dwriteFactory->CreateTextFormat(CoverTabbyIconFont(),nullptr,DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,12*ui,L"",m_enchantedSymbols.put());
                if(m_enchantedSymbols) m_enchantedSymbols->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            }
            const D2D1_RECT_F buttons[]={close,pin};
            const wchar_t* labels[]={Text(L"Close",L"Chiudi"),Text(L"Pin",L"Fissa")};
            const wchar_t* symbols[]={L"\uE8BB",m_pinned.contains(item.hwnd) ? L"\uE77A" : L"\uE718"};
            for(int button=0;button<2;++button) {
                const auto box=buttons[button];const bool hot=InRect(pointer,box);
                auto color=button==0 ? D2D1::ColorF(1,.22f,.26f) : m_settings.highlightColor;
                m_brush->SetColor(StyleColor(color,opacity*hover*(hot ? .28f : .12f)));
                m_ctx->FillRoundedRectangle({box,5*ui,5*ui},m_brush.get());
                m_brush->SetColor(StyleColor(color,opacity*hover));
                if(m_enchantedSymbols) m_ctx->DrawText(symbols[button],1,m_enchantedSymbols.get(),{box.left+5*ui,box.top,box.left+21*ui,box.bottom},m_brush.get());
                m_ctx->DrawText(labels[button],(UINT32)wcslen(labels[button]),m_simpleFormat.get(),{box.left+22*ui,box.top,box.right-3*ui,box.bottom},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
            }
        }
        m_brush->SetColor(StyleColor(ink,opacity*(1-hover)*(item.minimized ? .6f : 1.f)));
        m_ctx->DrawText(item.title.c_str(),(UINT32)item.title.size(),m_simpleFormat.get(),{x+27*ui,y,x+cardWidth-6*ui,y+28*ui},m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
        ID2D1Bitmap1* bitmap=item.content ? item.content.get() : item.placeholder.get();
        if(bitmap) {
            const auto size=bitmap->GetSize();const float scale=std::min((cardWidth-8*ui)/std::max(1.f,size.width),128*ui/std::max(1.f,size.height));
            const float w=size.width*scale,h=size.height*scale;
            m_ctx->DrawBitmap(bitmap,{x+(cardWidth-w)*.5f,y+30*ui+(128*ui-h)*.5f,x+(cardWidth+w)*.5f,y+30*ui+(128*ui+h)*.5f},opacity,D2D1_INTERPOLATION_MODE_LINEAR);
        }
    }
    if(enchanted && matches && SelectedIndex()>=0 && SelectedIndex()<(int)m_iconRects.size()) {
        const auto destination=m_iconRects[SelectedIndex()];
        if(destination.right>destination.left) {
            const double now=NowSeconds();
            const auto interpolate=[](D2D1_RECT_F a,D2D1_RECT_F b,float t) {
                return D2D1::RectF(a.left+(b.left-a.left)*t,a.top+(b.top-a.top)*t,a.right+(b.right-a.right)*t,a.bottom+(b.bottom-a.bottom)*t);
            };
            if(m_enchantedIndex<0) {m_enchantedFrom=m_enchantedTo=destination;m_enchantedBirth=now;}
            const float previous=std::clamp((float)((now-m_enchantedBirth)/.28),0.f,1.f);
            const auto current=m_enchantedWrap ? (previous<.5f ? m_enchantedFrom : m_enchantedTo) : interpolate(m_enchantedFrom,m_enchantedTo,EaseOutQuart(previous));
            if(m_enchantedIndex!=SelectedIndex() || m_enchantedTo.left!=destination.left || m_enchantedTo.top!=destination.top || m_enchantedTo.right!=destination.right) {
                m_enchantedWrap=m_enchantedCount==matches && matches>1 &&
                    ((m_enchantedPosition==matches-1 && position==0) || (m_enchantedPosition==0 && position==matches-1));
                m_enchantedFrom=current;m_enchantedTo=destination;m_enchantedBirth=now;
            }
            m_enchantedIndex=SelectedIndex();m_enchantedPosition=position;m_enchantedCount=matches;
            const float progress=std::clamp((float)((now-m_enchantedBirth)/.28),0.f,1.f);
            const float alpha=m_enchantedWrap ? std::abs(2*progress-1) : 1.f;
            const auto border=m_enchantedWrap ? (progress<.5f ? m_enchantedFrom : m_enchantedTo) : interpolate(m_enchantedFrom,m_enchantedTo,EaseOutQuart(progress));
            m_brush->SetColor(StyleColor(m_settings.highlightColor,opacity*alpha));
            m_ctx->DrawRoundedRectangle({{border.left-3*ui,border.top-3*ui,border.right+3*ui,border.bottom+3*ui},9*ui,9*ui},m_brush.get(),2.5f*ui);
        }
    } else if(enchanted) m_enchantedIndex=-1;
    if(!matches) {
        m_simpleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);m_brush->SetColor(StyleColor(ink,.65f*opacity));
        const auto empty=Text(L"No windows found",L"Nessuna finestra trovata");
        m_ctx->DrawText(empty,(UINT32)wcslen(empty),m_simpleFormat.get(),{left,top+76*ui,left+width,top+height},m_brush.get());
    }
    m_ctx->PopAxisAlignedClip();
}

void Switcher::DrawSearch(float t) {
    if(m_settings.windowMode || !m_settings.compactCoverFlow || !m_settings.enableSearch) {
        m_searchRect = {};
        return;
    }
    const float searchScale = m_dpiScale * m_settings.searchBarScale;
    if(!m_searchFormat) {
        if(FAILED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,m_settings.fontWeight,
            m_settings.fontStyle,DWRITE_FONT_STRETCH_NORMAL,14*searchScale,L"",m_searchFormat.put()))) return;
        m_searchFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        m_searchFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    std::wstring measured=m_searchQuery.empty() ? Text(L"Search windows...",L"Cerca finestre...") : m_searchQuery;
    if(measured!=m_searchMeasuredText) {
        com_ptr<IDWriteTextLayout> measure;
        DWRITE_TEXT_METRICS metrics{};
        m_searchMeasuredWidth=0;
        if(SUCCEEDED(m_dwriteFactory->CreateTextLayout(measured.c_str(),(UINT32)measured.size(),
            m_searchFormat.get(),10000,1000,measure.put())) && SUCCEEDED(measure->GetMetrics(&metrics)))
            m_searchMeasuredWidth=metrics.widthIncludingTrailingWhitespace;
        m_searchMeasuredText=measured;
    }
    const float availableWidth=std::max(1.0f,m_width-80*searchScale);
    const float maximum=std::min(availableWidth,std::min(m_width*0.82f,900*searchScale));
    const float minimum=std::min(maximum,400*searchScale);
    const float targetWidth=m_settings.searchBarWidth>0
        ? std::clamp(m_settings.searchBarWidth*searchScale,std::min(120*searchScale,availableWidth),availableWidth)
        : std::clamp(m_searchMeasuredWidth+96*searchScale,minimum,maximum);
    const double now=NowSeconds();
    if(m_searchBarWidth<=0) m_searchBarWidth=targetWidth;
    const double delta=std::clamp(now-m_searchBarTime,0.0,0.05);
    m_searchBarTime=now;
    m_searchBarWidth+=(targetWidth-m_searchBarWidth)*(float)(1-std::exp(-18*delta));
    const float width=m_searchBarWidth,height=36*searchScale;
    const float searchTop=std::max(24*m_dpiScale,m_height*0.075f);
    m_searchRect={(m_width-width)/2,searchTop,(m_width+width)/2,searchTop+height};
    DrawUiPanel(m_searchRect,m_settings.panelRadius*searchScale,t,.86f);
    m_brush->SetColor(m_searchActive ? StyleColor(m_settings.highlightColor,t) : UiForeground(.2f*t));
    m_ctx->DrawRoundedRectangle({m_searchRect,m_settings.panelRadius*searchScale,m_settings.panelRadius*searchScale},m_brush.get(),1);
    const float x=m_searchRect.left+17*searchScale,y=(m_searchRect.top+m_searchRect.bottom)/2;
    m_brush->SetColor(UiForeground(.8f*t));
    m_ctx->DrawEllipse({{x,y-2*searchScale},5*searchScale,5*searchScale},m_brush.get(),1.5f);
    m_ctx->DrawLine({x+4*searchScale,y+2*searchScale},{x+8*searchScale,y+6*searchScale},m_brush.get(),1.5f);
    std::wstring text=m_searchQuery.empty() ? Text(L"Search windows...",L"Cerca finestre...") : m_searchQuery;
    DrawAnimatedUiText(m_searchTextAnimation,text,m_searchFormat.get(),
        {m_searchRect.left+34*searchScale,m_searchRect.top+1,m_searchRect.right-10*searchScale,m_searchRect.bottom-1},
        t,m_searchActive && m_searchSelectAll && !m_searchQuery.empty(),m_searchActive,m_searchQuery.empty());
}

void Switcher::PrepareHoldPreview(double now) {
    if(!m_settings.holdToPreview || m_desktopOverview ||
        (m_settings.simpleStyle>0 && m_settings.simpleStyle<4) ||
        (!m_settings.simpleStyle && !m_settings.showThumbnails) || m_state!=State::Open) return;
    const int selected=SelectedIndex();
    if(selected<0 || selected>=(int)m_items.size()) return;
    auto& item=*m_items[selected];
    const bool changed=m_holdWindow!=item.hwnd;
    const bool navigated=GetTickCount64()-g_lastNavigationInputTick.load()<100;
    if(changed || (navigated && m_holdAmount>.001f)) {
        if(m_holdAmount>.001f) {
            const auto previous=std::find_if(m_items.begin(),m_items.end(),[&](const auto& candidate){return candidate->hwnd==m_holdWindow;});
            if(previous!=m_items.end()) {
                m_holdExitBitmap=(*previous)->content ? (*previous)->content : (*previous)->placeholder;
                m_holdExitQuad=m_holdQuad;m_holdExitAt=now;
            }
        }
        m_holdWindow=item.hwnd;m_holdSince=now;m_holdAmount=0;m_holdFullAt=0;
        m_holdMessageTitle={};m_holdMessageBody={};
    }
    if(!m_holdSince) m_holdSince=now;
    const bool alt=IsKeyDown(VK_MENU);
    if(!alt || navigated) m_holdSince=now;
    const bool enlarge=alt && now-m_holdSince>=5 && !item.windowGone &&
        (m_searchMatches.empty() ? !m_searchActive : true);
    if(enlarge && !m_holdExpanded) {
        MONITORINFO monitor{};monitor.cbSize=sizeof(monitor);
        if(GetMonitorInfoW(MonitorFromRect(&m_monitor,MONITOR_DEFAULTTONEAREST),&monitor)) {
            RECT host{};UnionRect(&host,&monitor.rcMonitor,&m_monitor);
            if(!EnsureSwapChain(host.right-host.left,host.bottom-host.top)) return;
            m_holdHost=host;m_holdOffset={(float)(m_monitor.left-host.left),(float)(m_monitor.top-host.top)};
            SetWindowPos(m_hwnd,HWND_TOPMOST,host.left,host.top,host.right-host.left,host.bottom-host.top,SWP_NOACTIVATE);
            m_holdExpanded=true;
        }
    }
    const float goal=enlarge && m_holdExpanded ? 1.f : 0.f;
    if(goal>0) {
        const float progress=(float)std::clamp((now-m_holdSince-5)/6.0,0.0,1.0);
        m_holdAmount=progress*progress*(3-2*progress);
        if(progress>=1 && m_holdFullAt==0) m_holdFullAt=now;
    } else {
        m_holdFullAt=0;
        m_holdAmount+=(goal-m_holdAmount)*(float)(1-std::exp(-18.0*m_motionDt));
    }
    if(m_holdAmount<.001f && goal==0) m_holdAmount=0;
    if(enlarge && !item.minimized && !item.captureAttempted && !item.desktopPlaceholder) StartCapture(item);
}

void Switcher::DrawHoldPreview(float opacity) {
    if(!m_settings.holdToPreview || m_desktopOverview || m_returnActive) return;
    auto bitmapFor=[](Item& item){return item.content ? item.content.get() : item.placeholder.get();};
    m_ctx->SetTransform(D2D1::Matrix3x2F::Identity());
    const double now=NowSeconds();
    if(m_holdExitBitmap) {
        const float progress=(float)std::clamp((now-m_holdExitAt)/.18,0.0,1.0);
        if(progress>=1) m_holdExitBitmap=nullptr;
        else {
            if(!m_holdTransform) m_ctx->CreateEffect(CLSID_D2D13DTransform,m_holdTransform.put());
            if(m_holdTransform) {
                Quad outgoing=m_holdExitQuad;
                for(auto& point:outgoing.p) point.x-=m_swapWidth*EaseOutQuart(progress);
                const auto size=m_holdExitBitmap->GetSize();
                m_holdTransform->SetInput(0,m_holdExitBitmap.get());
                m_holdTransform->SetValue(kTransform3DPropTransformMatrix,RectToQuadMatrix(size.width,size.height,outgoing));
                m_ctx->DrawImage(m_holdTransform.get());
            }
        }
    }
    if(m_holdAmount<=.001f) {
        if(m_holdTransform && !m_holdExitBitmap) m_holdTransform->SetInput(0,nullptr);
        return;
    }
    const int selected=SelectedIndex();
    if(selected<0 || selected>=(int)m_items.size() || m_items[selected]->hwnd!=m_holdWindow) return;
    auto& item=*m_items[selected];auto bitmap=bitmapFor(item);if(!bitmap) return;
    Quad base{};bool found=false;
    for(const auto& entry:m_drawList) if(entry.item==&item) {base=entry.quad;found=true;break;}
    if(!found && m_settings.simpleStyle>=4 && selected<(int)m_iconRects.size()) {
        const auto rect=m_iconRects[selected];
        if(rect.right>rect.left && rect.bottom>rect.top) {
            const auto size=bitmap->GetSize();
            const float scale=std::min((rect.right-rect.left-12*m_dpiScale)/size.width,
                std::max(1.f,rect.bottom-rect.top-32*m_dpiScale)/size.height);
            const float width=size.width*scale,height=size.height*scale;
            const float x=(rect.left+rect.right-width)*.5f,y=rect.top+30*m_dpiScale+(128*m_dpiScale-height)*.5f;
            base={{{x,y},{x+width,y},{x+width,y+height},{x,y+height}}};found=true;
        }
    }
    if(!found) return;
    for(auto& point:base.p) point=m_popupTransform.TransformPoint(point);
    const auto size=bitmap->GetSize();
    const float scale=std::min(m_swapWidth/size.width,m_swapHeight/size.height);
    const float width=size.width*scale,height=size.height*scale;
    const float x=(m_swapWidth-width)*.5f,y=(m_swapHeight-height)*.5f;
    const Quad full={{{x,y},{x+width,y},{x+width,y+height},{x,y+height}}};
    float amount=m_holdAmount;
    if(m_state==State::Closing) amount*=1-EaseOutQuart(m_open.Progress(now));
    for(int i=0;i<4;++i) m_holdQuad.p[i]={base.p[i].x+(full.p[i].x-base.p[i].x)*amount,base.p[i].y+(full.p[i].y-base.p[i].y)*amount};
    m_brush->SetColor(D2D1::ColorF(0,0,0,.85f*amount*opacity));
    m_ctx->FillRectangle({0,0,(float)m_swapWidth,(float)m_swapHeight},m_brush.get());
    if(!m_holdTransform) m_ctx->CreateEffect(CLSID_D2D13DTransform,m_holdTransform.put());
    if(m_holdTransform) {
        m_holdTransform->SetInput(0,bitmap);
        m_holdTransform->SetValue(kTransform3DPropInterpolationMode,kTransform3DInterpolationAnisotropic);
        m_holdTransform->SetValue(kTransform3DPropTransformMatrix,RectToQuadMatrix(size.width,size.height,m_holdQuad));
        m_ctx->DrawImage(m_holdTransform.get());
        if(item.minimized) {
            const bool replaceText=m_holdFullAt>0 && now-m_holdFullAt>=2;
            if(!replaceText && item.minimizedHint) {
                const auto hintSize=item.minimizedHint->GetSize();
                m_holdTransform->SetInput(0,item.minimizedHint.get());
                m_holdTransform->SetValue(kTransform3DPropTransformMatrix,RectToQuadMatrix(hintSize.width,hintSize.height,m_holdQuad));
                m_ctx->DrawImage(m_holdTransform.get());
            } else if(replaceText) {
                if(!m_holdMessageFormat) {
                    m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,DWRITE_FONT_WEIGHT_NORMAL,
                        DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,
                        std::clamp((m_holdQuad.p[1].x-m_holdQuad.p[0].x)/40.f,10*m_dpiScale,20*m_dpiScale),L"",m_holdMessageFormat.put());
                    if(m_holdMessageFormat) {
                        m_holdMessageFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                        m_holdMessageFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                        m_holdMessageFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                    }
                }
                const float center=(m_holdQuad.p[0].y+m_holdQuad.p[2].y)*.5f;
                const float left=m_holdQuad.p[0].x+12*m_dpiScale,right=m_holdQuad.p[1].x-12*m_dpiScale;
                DrawAnimatedUiText(m_holdMessageTitle,Text(L"Window minimized",L"Finestra ridotta a icona"),m_holdMessageFormat.get(),
                    {left,center-42*m_dpiScale,right,center},opacity);
                DrawAnimatedUiText(m_holdMessageBody,Text(L"Restore this window to see its live preview.",L"Ripristina la finestra per vedere l’anteprima in tempo reale."),m_holdMessageFormat.get(),
                    {left,center,right,center+42*m_dpiScale},opacity);
            }
        }
    }
}

void Switcher::RenderFrame() {
    const double now = NowSeconds();
    TickTouchpadTaps(now);
    const bool simple=m_settings.simpleStyle && !m_desktopOverview;
    const bool windowMode=m_settings.windowMode && !m_desktopOverview && !simple;
    UpdateDragReveal();
    const double dt = std::clamp(now - m_lastFrame, 0.0, 0.05);
    m_motionDt=dt;
    if(!m_settings.animateSelection) {m_scroll=(double)m_target;m_iconScroll=m_scroll;m_velocity=0;}
    m_lastFrame = now;
    UpdateLiveWallpaper();

    if(m_settings.tileGrid) {
        const float blend=(float)(1-std::exp(-18*dt));
        const int selected=SelectedIndex();
        for(int i=0;i<(int)m_items.size();++i)
            m_items[i]->tileSelectionAmount+=((i==selected ? 1.0f : 0.0f)-m_items[i]->tileSelectionAmount)*blend;
    }
    float parallaxTargetX=0,parallaxTargetY=0;
    POINT parallaxCursor{};
    if(!simple && !m_settings.disable3DWindows && !m_settings.energySaver && !m_desktopOverview &&
       GetCursorPos(&parallaxCursor)) {
        const float monitorWidth=(float)std::max(1L,m_monitor.right-m_monitor.left);
        const float monitorHeight=(float)std::max(1L,m_monitor.bottom-m_monitor.top);
        parallaxTargetX=std::clamp(((parallaxCursor.x-m_monitor.left)/monitorWidth-0.5f)*2,-1.0f,1.0f)*6*m_dpiScale;
        parallaxTargetY=std::clamp(((parallaxCursor.y-m_monitor.top)/monitorHeight-0.5f)*2,-1.0f,1.0f)*4*m_dpiScale;
    }
    const float parallaxBlend=(float)(1-std::exp(-10*dt));
    m_parallaxX+=(parallaxTargetX-m_parallaxX)*parallaxBlend;
    m_parallaxY+=(parallaxTargetY-m_parallaxY)*parallaxBlend;
    const float targetTiltX=m_dpiScale>0 ? parallaxTargetX/(6*m_dpiScale)*0.035f : 0;
    const float targetTiltY=m_dpiScale>0 ? -parallaxTargetY/(4*m_dpiScale)*0.025f : 0;
    m_cursorTiltX+=(targetTiltX-m_cursorTiltX)*parallaxBlend;
    m_cursorTiltY+=(targetTiltY-m_cursorTiltY)*parallaxBlend;

    if (m_state == State::Open && !g_stickyControls && AltReleased(now) && EndSwitching()) {
        BeginClose(true);
    }

    // Critically damped spring towards the target.
    const double omega = (m_settings.originalAnimations ? 20.0 : 30.0) * m_settings.flipSpeed;
    const int motionSteps=std::clamp((int)std::ceil(dt*120),1,4);
    const double motionStep=dt/motionSteps;
    for (int stepIndex=0;stepIndex<motionSteps;++stepIndex) {
        const double h=motionStep;
        const double acceleration =
            omega * omega * (m_target - m_scroll) - 2 * omega * m_velocity;
        m_velocity += acceleration * h;
        m_scroll += m_velocity * h;
    }
    if (std::fabs(m_target - m_scroll) < 1e-4 &&
        std::fabs(m_velocity) < 1e-3) {
        m_scroll = (double)m_target;
        m_velocity = 0;
    }
    // Icons and previews share the same horizontal animation.
    m_iconScroll = m_scroll; // Match the card motion exactly, including direction and timing.
    if (std::fabs((double)m_target - m_iconScroll) < 0.002) m_iconScroll = (double)m_target;
    // Keep the numbers small; only their value modulo the count matters.
    if (const int count = (int)m_items.size();
        count && std::llabs(m_target) > count * 1000LL) {
        const long long shift = m_target - PositiveMod(m_target, count);
        m_target -= shift;
        m_scroll -= (double)shift;
        m_iconScroll -= (double)shift;
    }

    const float t = m_open.Value(now);

    const int selectedForCapture=SelectedIndex();
    for (int index=0;index<(int)m_items.size();++index) {
        auto& item=*m_items[index];
        const bool visible=(m_settings.simpleStyle>=4 && index<(int)m_iconRects.size() && m_iconRects[index].right>m_iconRects[index].left) || index==selectedForCapture || std::any_of(
            m_drawList.begin(),m_drawList.end(),[&](const DrawEntry& entry) {
                return entry.item==&item && entry.pose.opacity>0.02f;
            });
        // Visible cards remain fully live. Far/off-screen captures are sampled
        // four times per second instead of once per display refresh.
        if((!simple || m_settings.simpleStyle>=4) && (visible || now-item.lastFramePoll>=0.25)) PollFrames(item);
    }
    HWND gripHovered=nullptr, previewHovered=nullptr;
    POINT gripCursor{};
    if (m_state==State::Open && !m_pressedWindow && !m_desktopMenuOpen &&
        GetCursorPos(&gripCursor) && ScreenToClient(m_hwnd,&gripCursor)) {
        const auto point=LayoutPoint(gripCursor);
        for (auto it=m_drawList.rbegin();it!=m_drawList.rend();++it) {
            if (!it->interactive || it->pose.opacity<0.5f || !PreviewContains(*it,point)) continue;
            previewHovered=it->item->hwnd;
            if (QuadContains(PreviewDragZone(it->quad,m_dpiScale),point)) gripHovered=it->item->hwnd;
            break;
        }
    }
    // Animate only genuinely closed windows, so a cancelled save prompt stays visible.
    for (auto& item : m_items) {
        const bool touchHold=m_touchSession && m_touchStroke.Holding(now) &&
            SelectedIndex()>=0 && item.get()==m_items[SelectedIndex()].get();
        const float pressTarget=m_state==State::Open && ((m_pressCanDrag &&
            item->hwnd==m_pressedWindow && IsKeyDown(VK_LBUTTON)) || touchHold) ? 1.0f : 0.0f;
        item->pressAmount += (pressTarget-item->pressAmount)*(float)(1-std::exp(-26*dt));
        if (m_state!=State::Closing) {
            const float gripTarget=item->hwnd==gripHovered ? 1.0f : 0.0f;
            item->gripHoverAmount += (gripTarget-item->gripHoverAmount)*(float)(1-std::exp(-22*dt));
            const float glowTarget=item->hwnd==previewHovered ? 1.0f : 0.0f;
            item->hoverGlowAmount += (glowTarget-item->hoverGlowAmount)*(float)(1-std::exp(-22*dt));
            // A cosine pulse expands and contracts with zero velocity at each
            // end. The hover fade remains independent, so entering/leaving
            // never switches the glow abruptly.
            if (item->hoverGlowAmount>0.005f)
                item->hoverGlowPhase=std::fmod(item->hoverGlowPhase+(float)dt*3.927f,6.2831853f);
            else item->hoverGlowPhase=0;
        }
        if(!item->windowGone && now>=item->nextWindowValidation) {
            DWORD currentPid=0;
            const bool alive=item->desktopPlaceholder || (IsWindow(item->hwnd) &&
                GetWindowThreadProcessId(item->hwnd,&currentPid) && currentPid==item->pid);
            item->windowGone=!alive;
            item->nextWindowValidation=now+0.20;
        }
        if (item->windowGone)
            item->closeProgress = std::min(1.0f, item->closeProgress + (float)dt / 0.30f);
        else if (item->closeRequested && now-item->closeRequestedAt>2) {
            // A cancelled save prompt must restore ordinary hover/navigation.
            item->closeRequested=false;
            item->closeRequestedAt=0;
        }
        item->reflowOffset *= (float)std::exp(-18 * dt);
    }
    if (!m_items.empty() && m_items[SelectedIndex()]->windowGone) {
        const int selected=SelectedIndex(), count=(int)m_items.size();
        int next=-1;
        for (int offset=1;offset<count;++offset) {
            const int candidate=PositiveMod((long long)selected+offset,count);
            if (m_items[candidate]->windowGone) continue;
            if (m_searchActive && (candidate>=(int)m_searchMask.size() || !m_searchMask[candidate])) continue;
            next=candidate; break;
        }
        if (next>=0) {
            int delta=next-selected;
            if (delta>count/2) delta-=count;
            if (delta< -count/2) delta+=count;
            m_target+=delta;
        }
    }
    for (int removed = (int)m_items.size()-1; removed >= 0; --removed) {
        if (m_items[removed]->closeProgress < 1) continue;
        const int oldSelected = SelectedIndex();
        StopCapture(*m_items[removed]);
        m_pinned.erase(m_items[removed]->hwnd);
        m_drawList.clear();
        m_items.erase(m_items.begin()+removed);
        if (m_items.empty()) {
            if(m_desktopOverview) {
                auto placeholder=std::make_unique<Item>();placeholder->desktopPlaceholder=true;
                placeholder->minimized=true;placeholder->rect=m_monitor;
                placeholder->title=Text(L"Desktops",L"Desktop");m_items.push_back(std::move(placeholder));m_target=0;
            } else {EndSwitching();EndSession();return;}
        }
        const int selected = oldSelected > removed ? oldSelected-1 :
            std::min(oldSelected,(int)m_items.size()-1);
        const double shift = (double)m_target-selected;
        m_target=selected; m_scroll-=shift; m_iconScroll-=shift;
        for (int i=0;i<(int)m_items.size();++i)
            m_items[i]->reflowOffset += (i>=removed ? 1.0f : 0.0f) + selected-oldSelected;
        m_titleIndex=-1; m_titleLayout=nullptr;
        ComputeLayout();
        if (m_searchActive || m_settings.simpleStyle>=4) {
            const long long target=m_target;
            UpdateSearch();
            // Enchanted uses search indices even before the user starts typing.
            if(!m_searchActive) m_target=target;
        }
    }
    const float contentT=t;
    if(!simple) {UpdateAppGroups();BuildDrawList(contentT);} else m_drawList.clear();
    if (!simple && m_state!=State::Closing) {
        const auto& selectedItem=*m_items[SelectedIndex()];
        const DWORD pid=selectedItem.desktopPlaceholder ? 0 : selectedItem.pid;
        m_processMonitor.Select(m_searchActive && m_searchMatches.empty() ? 0 : pid);
        m_processStats=m_processMonitor.Read(pid);
        if (selectedItem.processCreation && m_processStats.creation!=selectedItem.processCreation) {
            m_processStats={}; m_processStats.pid=pid;
        }
    }
    if(!simple && !m_desktopOverview) UpdateDesktopInfo(now);
    const float menuTarget=m_desktopMenuOpen ? 1.0f : 0.0f;
    m_desktopMenuAmount=m_settings.animateHover ? m_desktopMenuAmount+(menuTarget-m_desktopMenuAmount)*(float)(1-std::exp(-18*dt)) : menuTarget;
    if (m_state != State::Closing) {
    m_hoverWindow = nullptr;
    m_closeButton = {};
    m_killButton = {};
    m_pinButton = {};
    std::wstring cursorTitle;
    HWND previewHovered = nullptr;
    D2D1_POINT_2F titlePoint=m_cursorTitlePoint;
    if (m_settings.compactCoverFlow && m_settings.hoverControls && m_state == State::Open && !m_pressedWindow) {
        POINT cursor;
        if (GetCursorPos(&cursor) && ScreenToClient(m_hwnd, &cursor)) {
            const D2D1_POINT_2F point=LayoutPoint(cursor);
            if (std::abs(cursor.x-m_lastHoverCursor.x)>3 || std::abs(cursor.y-m_lastHoverCursor.y)>3) {
                m_hoverSelectionArmed=true;
                m_lastHoverCursor=cursor;
            }
            HWND hit=nullptr;
            if (!windowMode || m_open.Done(now)) {
                for (auto it=m_drawList.rbegin();it!=m_drawList.rend();++it) {
                    const bool selectedSurface = it->item->hwnd == m_items[SelectedIndex()]->hwnd &&
                        it->item->hoverAmount > 0.03f && InRect(point, GetHoverGeometry(*it).surface);
                    if (it->interactive && point.y <= (m_settings.tileGrid ? m_tileIconY-m_settings.iconSize*m_dpiScale*.5f-10*m_dpiScale : m_height*0.77f) && it->pose.opacity>0.5f &&
                        (PreviewContains(*it,point) || selectedSurface)) {
                        hit=it->item->hwnd; break;
                    }
                }
            }
            if (hit != m_hoverCandidate) { m_hoverCandidate=hit; m_hoverCandidateSince=now; }
            const int selected=SelectedIndex();
            if (!m_iconMenuOpen && !m_desktopMenuOpen && (m_settings.hoverSelects || m_settings.hoverShowsTitle) &&
                hit && hit != m_items[selected]->hwnd) previewHovered = hit;
            if (!m_iconMenuOpen && !m_desktopMenuOpen && m_settings.hoverSelects && hit && hit!=m_items[selected]->hwnd && m_hoverSelectionArmed && now-m_hoverCandidateSince>m_settings.hoverSelectionDelay) {
                for (int i=0;i<(int)m_items.size();++i) {
                    if (m_items[i]->hwnd!=hit || m_items[i]->closeRequested) continue;
                    int delta=i-selected, count=(int)m_items.size();
                    if (delta>count/2) delta-=count;
                    if (delta< -count/2) delta+=count;
                    m_target+=delta;
                    m_hoverSelectionArmed=false;
                    break;
                }
            }
            if (!m_iconMenuOpen && !m_desktopMenuOpen && m_settings.hoverShowsTitle && hit && hit!=m_items[selected]->hwnd) {
                for (const auto& item : m_items) {
                    if (item->hwnd!=hit || item->closeRequested) continue;
                    cursorTitle=item->title;
                    titlePoint=point;
                    break;
                }
            }
            // Only the selected preview exposes Pin and Close controls.
            if (hit && hit==m_items[SelectedIndex()]->hwnd &&
                !m_items[SelectedIndex()]->desktopPlaceholder &&
                std::fabs(m_scroll-m_target)<0.08) m_hoverWindow=hit;
        }
    }
    if (m_desktopMenuOpen && m_desktopMenuWindow==m_items[SelectedIndex()]->hwnd)
        m_hoverWindow=m_desktopMenuWindow;
    if (!cursorTitle.empty() && cursorTitle!=m_cursorTitle) {
        m_cursorTitle=cursorTitle;
        m_hoverTitleLayout=nullptr;
        m_cursorTitleOpacity=0;
    }
    if (!cursorTitle.empty()) m_cursorTitlePoint=titlePoint;
    const float cursorTitleTarget=cursorTitle.empty() ? 0.0f : 1.0f;
    if (!m_settings.animateHover) m_cursorTitleOpacity=cursorTitleTarget;
    else m_cursorTitleOpacity+=(cursorTitleTarget-m_cursorTitleOpacity)*(float)(1-std::exp(-18*dt));
    for (auto& item : m_items) {
        const float previewTarget = item->hwnd == previewHovered &&
            item->hwnd != m_items[SelectedIndex()]->hwnd && !item->closeRequested ? 1.0f : 0.0f;
        item->previewHoverAmount = m_settings.animateHover
            ? item->previewHoverAmount + (previewTarget-item->previewHoverAmount)*(float)(1-std::exp(-18*dt))
            : previewTarget;
        const float target=item->hwnd==m_hoverWindow ? 1.0f : 0.0f;
        if(!m_settings.animateHover) {item->hoverAmount=target;item->hoverVelocity=0;}
        else if(m_settings.originalAnimations) {
            item->hoverAmount+=(target-item->hoverAmount)*(float)(1-std::exp(-16*dt));
            item->hoverVelocity=0;
        } else {
            const int hoverSteps=std::clamp((int)std::ceil(dt*120),1,4);
            const float step=(float)(dt/hoverSteps);
            for(int stepIndex=0;stepIndex<hoverSteps;++stepIndex) {
                item->hoverVelocity+=(324*(target-item->hoverAmount)-36*item->hoverVelocity)*step;
                item->hoverAmount=std::clamp(item->hoverAmount+item->hoverVelocity*step,0.0f,1.0f);
            }
        }
    }

    }
    PrepareHoldPreview(now);
    m_ctx->SetTarget(m_targetBitmap.get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));
    const bool holdSceneLayer=m_settings.holdToPreview && m_holdAmount>.001f && !m_returnActive;
    if(holdSceneLayer) {
        const float sceneOpacity=1-EaseOutQuart(std::clamp(m_holdAmount*2,0.f,1.f));
        m_ctx->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(),nullptr,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
            D2D1::IdentityMatrix(),sceneOpacity),nullptr);
    }


    m_popupTransform=D2D1::Matrix3x2F::Translation(m_closeSceneOffset.x+m_holdOffset.x,m_closeSceneOffset.y+m_holdOffset.y);
    m_ctx->SetTransform(m_popupTransform);
    bool windowLayer = false;
    if (windowMode || !m_settings.originalAnimations) {
        // The backdrop stays stationary through both fades.
        m_ctx->SetTransform(D2D1::Matrix3x2F::Scale(m_windowScale,m_windowScale) * m_popupTransform);
        m_ctx->GetTransform(&m_popupTransform);
        const float margin=windowMode ? 24*m_dpiScale : 2;
        const D2D1_RECT_F bounds{margin,margin,m_width-margin,m_height-margin};
        if(windowMode && m_popupShadow) {
            m_popupShadow->SetValue(D2D1_SHADOW_PROP_COLOR,D2D1::Vector4F(0,0,0,0.45f*t));
            m_ctx->DrawImage(m_popupShadow.get(),D2D1::Point2F(0,6*m_dpiScale));
        }
        if(windowMode && !m_windowMaskGeometry)
            m_d2dFactory->CreateRoundedRectangleGeometry(
                {bounds,m_settings.panelRadius*m_dpiScale,m_settings.panelRadius*m_dpiScale},
                m_windowMaskGeometry.put());
        if (windowMode && m_windowMaskGeometry) {
            auto parameters=D2D1::LayerParameters1();
            parameters.geometricMask=m_windowMaskGeometry.get();
            m_ctx->PushLayer(parameters,nullptr); windowLayer=true;
        }
    }
    const D2D1_RECT_F screen{0, 0, m_width, m_height};
    if(simple && m_settings.simpleStyle==5 && !m_settings.windowMode) {
        if(m_background) m_ctx->DrawBitmap(m_background.get(),screen,t,D2D1_INTERPOLATION_MODE_LINEAR);
        m_brush->SetColor(m_settings.darkBackground ? D2D1::ColorF(0,0,0,.13f*t) : D2D1::ColorF(1,1,1,.10f*t));
        m_ctx->FillRectangle(screen,m_brush.get());
    }
    if(!simple) {
    if(m_desktopOverview) {
        m_brush->SetColor(D2D1::ColorF(0.055f,0.055f,0.065f,t));
        m_ctx->FillRectangle(screen,m_brush.get());
    }
    if (m_background && (!m_desktopOverview || m_settings.blurBackground)) {
        m_ctx->DrawBitmap(m_background.get(), screen, t,
                          D2D1_INTERPOLATION_MODE_LINEAR);
    }
    auto backgroundTint=m_settings.backgroundColor;
    backgroundTint.a=m_settings.backgroundOpacity*t;
    if(m_desktopOverview) backgroundTint=D2D1::ColorF(0.02f,0.02f,0.03f,
        (m_settings.blurBackground && m_background ? 0.65f : 1.0f)*t);
    if(windowMode && m_settings.compactMaterial) {
        m_brush->SetColor(m_settings.darkBackground ? D2D1::ColorF(0.125f,0.125f,0.125f,t) : D2D1::ColorF(0.953f,0.953f,0.953f,t));
        m_ctx->FillRoundedRectangle({{24*m_dpiScale,24*m_dpiScale,m_width-24*m_dpiScale,m_height-24*m_dpiScale},m_settings.panelRadius*m_dpiScale,m_settings.panelRadius*m_dpiScale},m_brush.get());
    }
    m_brush->SetColor(backgroundTint);
    if(windowMode)
        m_ctx->FillRoundedRectangle({{24*m_dpiScale,24*m_dpiScale,m_width-24*m_dpiScale,m_height-24*m_dpiScale},m_settings.panelRadius*m_dpiScale,m_settings.panelRadius*m_dpiScale},m_brush.get());
    else m_ctx->FillRectangle(screen,m_brush.get());


    }
    // Background and content have separate closing animations. Keep the
    // background fixed and move every foreground element as a single group.
    if (m_state == State::Closing) {
        const float progress=m_open.Progress(now);
        const float ease=EaseOutQuart(progress);
        m_ctx->PushAxisAlignedClip(screen,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        // Short travel keeps the lower labels inside the popup while fading.
        const float travel=std::min(32*m_dpiScale,m_height*0.035f);
        m_ctx->SetTransform(D2D1::Matrix3x2F::Translation(0,m_dragWindow ? 0 : travel*ease) * m_popupTransform);
    }
    if(m_desktopOverview) DrawDesktopOverview(contentT);
    else if(simple) { if(m_settings.simpleStyle>=4) DrawSearchStyle(contentT); else DrawSimpleStyle(contentT); DrawIconContext(contentT); }
    else {
    if (m_settings.compactCoverFlow && m_state != State::Closing) {
        m_ctx->PushAxisAlignedClip({0, 0,
                                    m_width, m_settings.tileGrid ? m_tileIconY-m_settings.iconSize*m_dpiScale*.5f-10*m_dpiScale : m_height * 0.77f},
                                   D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    }
    bool tileFadeLayer=false;
    if(m_settings.tileGrid) {
        if(!m_tileFadeBrush) {
            D2D1_GRADIENT_STOP stops[]={{0,D2D1::ColorF(1,1,1,1)},{1,D2D1::ColorF(1,1,1,0)}};
            com_ptr<ID2D1GradientStopCollection> collection;
            if(SUCCEEDED(m_ctx->CreateGradientStopCollection(stops,2,collection.put())))
                m_ctx->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties({0,0},{0,1}),collection.get(),m_tileFadeBrush.put());
        }
        if(m_tileFadeBrush) {
            const float end=m_tileIconY-m_settings.iconSize*m_dpiScale*.5f-10*m_dpiScale;
            m_tileFadeBrush->SetStartPoint({0,end-24*m_dpiScale});m_tileFadeBrush->SetEndPoint({0,end});
            m_ctx->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(),nullptr,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                D2D1::IdentityMatrix(),1,m_tileFadeBrush.get()),nullptr);
            tileFadeLayer=true;
        }
    }
    if(m_settings.showThumbnails && !m_settings.tileGrid) for (const DrawEntry& entry : m_drawList)
        if (entry.item->hwnd != m_dragWindow && (!m_returnActive || entry.item!=m_returnEntry.item)) DrawReflection(entry, contentT);
    for (const DrawEntry& entry : m_drawList) {
        if(m_settings.showThumbnails && entry.item->hwnd != m_dragWindow && (!m_returnActive || entry.item!=m_returnEntry.item)) DrawCard(entry, contentT);
    }
    DrawTileSelection(contentT);
    if(tileFadeLayer) m_ctx->PopLayer();
    if (m_settings.compactCoverFlow) {
        if (m_state != State::Closing) m_ctx->PopAxisAlignedClip();
        if(m_settings.showThumbnails && !m_dragWindow) DrawHoverControls(contentT);
        DrawCompactIcons(contentT);
        DrawProcessStats(contentT);
        DrawSearch(contentT);
        if(!windowMode) DrawWindowCount(contentT,{m_width-310*m_dpiScale,14*m_dpiScale,m_width-24*m_dpiScale,42*m_dpiScale},m_searchActive ? (int)m_searchMatches.size() : (int)m_items.size(),(int)m_items.size());
    }
    DrawTitle(contentT);
    DrawHoverTitle(contentT);
    DrawDesktopPicker(contentT);
    DrawIconContext(contentT);
    }
    DrawSelectionBar(contentT);
    if (m_state == State::Closing) {
        m_ctx->SetTransform(m_popupTransform);
        m_ctx->PopAxisAlignedClip();
    }

    if (windowMode || !m_settings.originalAnimations) {
        if (windowLayer) m_ctx->PopLayer();
        if(windowMode && m_settings.showSwitcherBorder) {
            m_brush->SetColor(StyleColor(m_settings.borderColor,t));
            m_ctx->DrawRoundedRectangle({{24*m_dpiScale,24*m_dpiScale,m_width-24*m_dpiScale,m_height-24*m_dpiScale},m_settings.panelRadius*m_dpiScale,m_settings.panelRadius*m_dpiScale},m_brush.get(),m_dpiScale);
        }
        m_ctx->SetTransform(D2D1::Matrix3x2F::Identity());
    }
    if(holdSceneLayer) m_ctx->PopLayer();
    DrawHoldPreview(contentT);
    // Draw outside the popup mask, after the fading UI. The preview itself
    // reaches the desktop bounds before focus is handed to the real window.
    if (m_returnActive && m_state==State::Closing && !m_returnEntry.item->windowGone) {
        m_ctx->SetTransform(D2D1::Matrix3x2F::Identity());
        DrawEntry returning=m_returnEntry;
        const float progress=m_open.Progress(now);
        const float ease=EaseOutQuart(progress);
        returning.quad=ReturnWindowQuad(m_returnFrom,m_returnTo,progress);
        returning.pose.opacity=m_returnEntry.pose.opacity+(1-m_returnEntry.pose.opacity)*ease;
        returning.pose.brightness=m_returnEntry.pose.brightness+(1-m_returnEntry.pose.brightness)*ease;
        DrawCard(returning,1-progress);
    }
    HRESULT hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    if (SUCCEEDED(hr)) {
        hr = m_swapChain->Present(1, 0);
    }
    if (FAILED(hr)) {
        Wh_Log(L"Rendering failed: 0x%08X", (UINT)hr);

        HandleDeviceLost();
        return;
    }

    if (m_state == State::Closing && m_open.Done(now)) {
        FinishClose();
    } else if (m_state==State::Open) {
        const int selected=SelectedIndex();
        // Icon/text layouts never start window capture sessions. For thumbnail
        // grids, only initialize visible captures after keyboard input settles.
        if(!simple) {
            if(selected>=0 && !m_items[selected]->captureAttempted && !m_items[selected]->windowGone)
                StartCapture(*m_items[selected]);
            ContinueCaptures(1);
        } else if(m_settings.simpleStyle>=4 && GetTickCount64()-g_lastNavigationInputTick.load()>=250) {
            for(size_t index=0;index<m_items.size() && index<m_iconRects.size();++index) {
                const auto& rect=m_iconRects[index];auto& item=*m_items[index];
                if(rect.right>rect.left && rect.bottom>rect.top && !item.captureAttempted && !item.windowGone) {
                    StartCapture(item);break;
                }
            }
        }
        if(m_desktopOverview) {LoadOverviewWallpaper();UpdateOverviewCaptures();}
        if(m_settings.previewCache && now>=m_snapshotAt && !m_items.empty()) {
            SaveWindowSnapshot(*m_items[m_snapshotNext++ % m_items.size()]);
            m_snapshotAt=now+(m_settings.energySaver ? 1.5 : 0.75);
        }
    }
}

// Looking an icon up can wait on the window's app (WM_GETICON) or the shell,
// which would delay even a quick Alt+Tab. So the icons are kept across
// sessions, and RefreshIcons checks them after a session.
com_ptr<ID2D1Bitmap1> Switcher::GetIconBitmap(HWND hwnd) {
    if (auto it = m_iconCache.find(hwnd); it != m_iconCache.end()) {
        return it->second.bitmap;
    }

    CachedIcon entry;
    if (HICON icon = GetWindowIcon(hwnd)) {
        entry.bitmap = CreateIconBitmap(icon);
        if (entry.bitmap) {
            entry.source = icon;
        }
    }
    if (!entry.bitmap) {
        entry.bitmap = CreateAppIconBitmap(hwnd);
    }
    m_iconCache[hwnd] = entry;
    return entry.bitmap;
}

// Forgets closed windows and picks up icons that changed.
void Switcher::RefreshIcons() {
    std::erase_if(m_iconCache,
                  [](const auto& entry) { return !IsWindow(entry.first); });

    for (auto& [hwnd, entry] : m_iconCache) {
        HICON icon = GetWindowIcon(hwnd);
        if (icon && icon != entry.source) {
            if (auto bitmap = CreateIconBitmap(icon)) {
                entry = {icon, bitmap};
            }
        } else if (!icon && !entry.bitmap) {
            // Still no window icon; the app may have finished starting.
            entry.bitmap = CreateAppIconBitmap(hwnd);
        }
    }
}

// Packaged (UWP) apps, hosted in an ApplicationFrameWindow, usually have no
// window icon. Their icon comes from the shell, by the app's AppUserModelID.
com_ptr<ID2D1Bitmap1> Switcher::CreateAppIconBitmap(HWND hwnd) {
    com_ptr<IPropertyStore> store;
    if (FAILED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(store.put())))) {
        return nullptr;
    }
    std::wstring appId;
    PROPVARIANT value;
    PropVariantInit(&value);
    if (SUCCEEDED(store->GetValue(kPKEY_AppUserModel_ID, &value)) &&
        value.vt == VT_LPWSTR && value.pwszVal) {
        appId = value.pwszVal;
    }
    PropVariantClear(&value);
    if (appId.empty()) {
        return nullptr;
    }

    com_ptr<IShellItemImageFactory> imageFactory;
    if (FAILED(SHCreateItemInKnownFolder(kFOLDERID_AppsFolder, 0,
                                         appId.c_str(),
                                         IID_PPV_ARGS(imageFactory.put())))) {
        return nullptr;
    }
    const int size = (int)std::round(64 * m_dpiScale);
    HBITMAP hbitmap = nullptr;
    if (FAILED(imageFactory->GetImage({size, size}, SIIGBF_ICONONLY,
                                      &hbitmap))) {
        return nullptr;
    }

    com_ptr<IWICBitmap> wicBitmap;
    com_ptr<IWICFormatConverter> converter;
    com_ptr<ID2D1Bitmap1> bitmap;
    const bool ok =
        SUCCEEDED(m_wicFactory->CreateBitmapFromHBITMAP(
            hbitmap, nullptr, WICBitmapUsePremultipliedAlpha,
            wicBitmap.put())) &&
        SUCCEEDED(m_wicFactory->CreateFormatConverter(converter.put())) &&
        SUCCEEDED(converter->Initialize(
            wicBitmap.get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0,
            WICBitmapPaletteTypeMedianCut)) &&
        SUCCEEDED(m_ctx->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                                   bitmap.put()));
    DeleteObject(hbitmap);
    return ok ? bitmap : nullptr;
}

com_ptr<ID2D1Bitmap1> Switcher::CreateIconBitmap(HICON icon) {
    com_ptr<IWICBitmap> wicBitmap;
    com_ptr<IWICFormatConverter> converter;
    com_ptr<ID2D1Bitmap1> bitmap;
    if (FAILED(m_wicFactory->CreateBitmapFromHICON(icon, wicBitmap.put())) ||
        FAILED(m_wicFactory->CreateFormatConverter(converter.put())) ||
        FAILED(converter->Initialize(wicBitmap.get(),
                                     GUID_WICPixelFormat32bppPBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0,
                                     WICBitmapPaletteTypeMedianCut)) ||
        FAILED(m_ctx->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                                bitmap.put()))) {
        return nullptr;
    }
    return bitmap;
}

com_ptr<ID2D1Bitmap1> Switcher::CreateTargetBitmap(UINT width, UINT height) {
    com_ptr<ID2D1Bitmap1> bitmap;
    const D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));
    if (FAILED(m_ctx->CreateBitmap({width, height}, nullptr, 0, &props,
                                   bitmap.put()))) {
        return nullptr;
    }
    return bitmap;
}

// Card shown until the first frame arrives, and for minimized windows.
void Switcher::SaveWindowSnapshot(Item& item) {
    if (!m_settings.previewCache || !m_ctx || !item.content || item.minimized || !item.processCreation || !IsWindow(item.hwnd)) return;
    const auto existing=m_windowSnapshots.find(item.hwnd);
    if(existing!=m_windowSnapshots.end() && item.snapshotVersion==item.frameVersion) return;
    DWORD pid=0;
    if (!GetWindowThreadProcessId(item.hwnd,&pid) || pid!=item.pid) return;
    const auto size=item.content->GetSize();
    if (size.width<=0 || size.height<=0) return;
    const float scale=std::min(1.0f,(m_settings.energySaver ? 384 : 512)/std::max(size.width,size.height));
    const UINT width=(UINT)std::max(1L,std::lround(size.width*scale));
    const UINT height=(UINT)std::max(1L,std::lround(size.height*scale));
    auto snapshot=CreateTargetBitmap(width,height);
    if (!snapshot) return;
    m_ctx->SetTarget(snapshot.get());m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0,0,0,1));
    m_ctx->DrawBitmap(item.content.get(),{0,0,(float)width,(float)height},1,
        D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
    const HRESULT result=m_ctx->EndDraw();m_ctx->SetTarget(nullptr);
    if (FAILED(result)) return;
    // At most twelve 512px snapshots, kept in memory on the current device.
    if (m_windowSnapshots.size()>=12 && !m_windowSnapshots.contains(item.hwnd)) {
        const auto oldest=std::min_element(m_windowSnapshots.begin(),m_windowSnapshots.end(),
            [](const auto& a,const auto& b){return a.second.used<b.second.used;});
        if (oldest!=m_windowSnapshots.end()) m_windowSnapshots.erase(oldest);
    }
    m_windowSnapshots[item.hwnd]={item.pid,item.processCreation,item.title,snapshot,NowSeconds()};
    item.snapshotVersion=item.frameVersion;
}

com_ptr<ID2D1Bitmap1> Switcher::CreatePlaceholder(const Item& item, bool hintLayer) {
    if(m_settings.previewCache && !item.minimized && !hintLayer) {
        const auto previous=m_windowSnapshots.find(item.hwnd);
        if(previous!=m_windowSnapshots.end() && previous->second.pid==item.pid &&
            previous->second.creation==item.processCreation && previous->second.title==item.title) {
            previous->second.used=NowSeconds();
            return previous->second.bitmap;
        }
    }
    const float scale =
        std::min(1.0f, 640 / std::max(item.Width(), item.Height()));
    const UINT width = (UINT)std::max(std::lround(item.Width() * scale), 1L);
    const UINT height = (UINT)std::max(std::lround(item.Height() * scale), 1L);

    com_ptr<ID2D1Bitmap1> bitmap = CreateTargetBitmap(width, height);
    if (!bitmap) {
        return nullptr;
    }

    m_ctx->SetTarget(bitmap.get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));

    const D2D1_ROUNDED_RECT card{{0.5f, 0.5f, width - 0.5f, height - 0.5f},
                                 8,
                                 8};
    const auto cached=m_windowSnapshots.find(item.hwnd);
    const bool hasSnapshot=m_settings.previewCache && item.minimized && cached!=m_windowSnapshots.end() &&
        cached->second.pid==item.pid && cached->second.creation==item.processCreation &&
        cached->second.title==item.title;
    if (!hintLayer) {
    m_brush->SetColor(item.minimized ? (m_settings.darkBackground
        ? m_settings.backgroundColor : D2D1::ColorF(1,1,1,1))
                                    : D2D1::ColorF(0.13f, 0.13f, 0.15f, 0.96f));
    m_ctx->FillRoundedRectangle(card, m_brush.get());
    m_brush->SetColor(item.minimized ? (m_settings.darkBackground
        ? D2D1::ColorF(1,1,1,0.14f) : D2D1::ColorF(0,0,0,0.12f))
                                    : D2D1::ColorF(1, 1, 1, 0.14f));
    m_ctx->DrawRoundedRectangle(card, m_brush.get(), 1);

    if (hasSnapshot) {
        com_ptr<ID2D1Effect> blur;
        com_ptr<ID2D1RoundedRectangleGeometry> mask;
        if (SUCCEEDED(m_ctx->CreateEffect(CLSID_D2D1GaussianBlur,blur.put())) &&
            SUCCEEDED(m_d2dFactory->CreateRoundedRectangleGeometry(card,mask.put()))) {
            blur->SetInput(0,cached->second.bitmap.get());
            blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,14.0f);
            blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE,(UINT32)D2D1_BORDER_MODE_HARD);
            m_ctx->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(),mask.get()),nullptr);
            const auto cachedSize=cached->second.bitmap->GetSize();
            m_ctx->SetTransform(D2D1::Matrix3x2F::Scale(width/cachedSize.width,height/cachedSize.height));
            m_ctx->DrawImage(blur.get());
            m_ctx->SetTransform(D2D1::Matrix3x2F::Identity());
            m_brush->SetColor(D2D1::ColorF(0,0,0,0.56f));
            m_ctx->FillRectangle({0,0,(float)width,(float)height},m_brush.get());
            m_ctx->PopLayer();
            cached->second.used=NowSeconds();
        }
    }
    }

    if (!hintLayer && item.icon && !item.minimized) {
        const float iconSize =
            std::round(std::min({width * 0.3f, height * 0.3f, 96.0f}));
        const float x = std::round((width - iconSize) / 2);
        const float y = item.minimized ? std::round(height*0.12f)
                                      : std::round((height - iconSize) / 2);
        m_ctx->DrawBitmap(item.icon.get(), {x, y, x + iconSize, y + iconSize},
                          1, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
    }

    if (item.minimized && hintLayer) {
        // Tint the whole minimized preview instead of placing a panel behind
        // the text. Choose foreground contrast from the composited tint.
        const float panelPadding=std::min(18.0f,width*.05f);
        auto tint=m_settings.highlightColor;
        if(m_settings.minimizedTint==Settings::MinimizedTint::Gray)
            tint=D2D1::ColorF(.5f,.5f,.5f,1);
        tint.a=m_settings.minimizedTint==Settings::MinimizedTint::None ? 0.0f : .24f;
        if(tint.a>0) {
            m_brush->SetColor(tint);
            m_ctx->FillRoundedRectangle(card,m_brush.get());
        }
        const auto linear=[](float channel) {
            return channel<=.04045f ? channel/12.92f : std::pow((channel+.055f)/1.055f,2.4f);
        };
        const auto base=hasSnapshot ? D2D1::ColorF(.18f,.18f,.18f,1) :
            (m_settings.darkBackground ? m_settings.backgroundColor : D2D1::ColorF(1,1,1,1));
        const float luminance=.2126f*linear(tint.r*tint.a+base.r*(1-tint.a))+
            .7152f*linear(tint.g*tint.a+base.g*(1-tint.a))+
            .0722f*linear(tint.b*tint.a+base.b*(1-tint.a));
        const auto textColor=luminance>.179f ? D2D1::ColorF(0,0,0,1) : D2D1::ColorF(1,1,1,1);
        com_ptr<IDWriteTextFormat> format;
        const float fontSize=std::clamp(height*0.065f,10.0f,24.0f);
        if (SUCCEEDED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),
            nullptr,DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,fontSize,m_settings.italian ? L"it-IT" : L"en-US",format.put()))) {
            format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            format->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
            const wchar_t* message=Text(
                L"This window is minimized.\nDrag from the top or select this window to restore it.",
                L"Questa finestra \u00E8 minimizzata.\nTrascinala dalla parte superiore o selezionala per ripristinarla.");
            const float padding=std::min(24.0f,width*0.06f);
            const float textTop=height*0.27f;
            m_brush->SetColor(textColor);
            m_ctx->DrawText(message,(UINT32)wcslen(message),format.get(),
                {padding,textTop,width-padding,height*.64f},
                m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
            const float smallSize=std::clamp(fontSize*.65f,7.0f,15.0f);
            com_ptr<IDWriteTextFormat> small;
            if(SUCCEEDED(m_dwriteFactory->CreateTextFormat(m_settings.fontFamily.c_str(),nullptr,
                DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,
                smallSize,L"",small.put()))) {
                small->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                small->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                const DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER,0,0};
                com_ptr<IDWriteInlineObject> ellipsis;
                m_dwriteFactory->CreateEllipsisTrimmingSign(small.get(),ellipsis.put());
                small->SetTrimming(&trimming,ellipsis.get());
                const std::wstring label=(item.appName.empty() ? Text(L"Application",L"Applicazione") : item.appName)+
                    (item.title.empty() ? L"" : L" - "+item.title);
                const float iconSize=item.icon ? std::min(20.0f,height*.09f) : 0;
                const float gap=iconSize>0 ? 6.0f : 0;
                const float available=std::max(1.0f,width-2*(panelPadding+10)-iconSize-gap);
                com_ptr<IDWriteTextLayout> layout;
                if(SUCCEEDED(m_dwriteFactory->CreateTextLayout(label.c_str(),(UINT32)label.size(),small.get(),available,smallSize*1.8f,layout.put()))) {
                    DWRITE_TEXT_METRICS metrics{};layout->GetMetrics(&metrics);
                    const float lineWidth=std::min(available,metrics.widthIncludingTrailingWhitespace);
                    const float left=(width-lineWidth-iconSize-gap)*.5f;
                    const float center=height*.73f;
                    if(item.icon) m_ctx->DrawBitmap(item.icon.get(),{left,center-iconSize*.5f,left+iconSize,center+iconSize*.5f},
                        1,D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
                    m_ctx->DrawTextLayout({left+iconSize+gap,center-smallSize*.9f},layout.get(),m_brush.get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
                }
            }
        }
    }
    if (item.minimized && !hintLayer) {
        // A visible grip marks the top area used to start dragging.
        const float gripWidth=std::min(40.0f,width*0.18f);
        const float gripY=std::min(14.0f,height*0.06f);
        m_brush->SetColor((hasSnapshot || m_settings.darkBackground) ? D2D1::ColorF(0.75f,0.77f,0.81f,0.8f)
                                                  : D2D1::ColorF(0.45f,0.47f,0.51f,0.7f));
        m_ctx->FillRoundedRectangle({{(width-gripWidth)*0.5f,gripY,
            (width+gripWidth)*0.5f,gripY+3},1.5f,1.5f},m_brush.get());
    }

    const HRESULT hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    return SUCCEEDED(hr) ? bitmap : nullptr;
}

// Compatibility Acrylic-like material: blur actual desktop pixels, not wallpaper.
// Refresh once per opening, before displaying the overlay, to avoid self-capture.
void Switcher::CaptureAcrylicBackdrop() {
    m_background=nullptr;m_simpleContrastSamples.clear();
    const UINT screenWidth=(UINT)(m_monitor.right-m_monitor.left);
    const UINT screenHeight=(UINT)(m_monitor.bottom-m_monitor.top);
    // Blurred desktop detail does not need a full-resolution GPU texture.
    const float captureScale=std::min(1.0f,960.0f/std::max(screenWidth,screenHeight));
    const UINT width=std::max(1U,(UINT)std::lround(screenWidth*captureScale));
    const UINT height=std::max(1U,(UINT)std::lround(screenHeight*captureScale));
    HDC screen=GetDC(nullptr), memory=screen ? CreateCompatibleDC(screen) : nullptr;
    BITMAPINFO info{};
    info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth=(LONG)width; info.bmiHeader.biHeight=-(LONG)height;
    info.bmiHeader.biPlanes=1; info.bmiHeader.biBitCount=32; info.bmiHeader.biCompression=BI_RGB;
    void* pixels=nullptr;
    HBITMAP capture=memory ? CreateDIBSection(screen,&info,DIB_RGB_COLORS,&pixels,nullptr,0) : nullptr;
    com_ptr<ID2D1Bitmap1> bitmap;
    if (capture && pixels) {
        HGDIOBJ previous=SelectObject(memory,capture);
        SetStretchBltMode(memory,COLORONCOLOR);
        if (StretchBlt(memory,0,0,width,height,screen,m_monitor.left,m_monitor.top,
                       screenWidth,screenHeight,SRCCOPY|CAPTUREBLT)) {
            GdiFlush();
            auto data=static_cast<UINT32*>(pixels);
            for (size_t i=0;i<(size_t)width*height;++i) data[i]|=0xFF000000;
            for(int gy=0;gy<18;++gy) for(int gx=0;gx<32;++gx) {
                float red=0,green=0,blue=0;
                for(int dy=-2;dy<=2;++dy) for(int dx=-2;dx<=2;++dx) {
                    const int px=std::clamp((int)((gx+.5f)*width/32)+dx,0,(int)width-1);
                    const int py=std::clamp((int)((gy+.5f)*height/18)+dy,0,(int)height-1);
                    const UINT32 pixel=data[py*width+px];
                    red+=(pixel>>16)&255;green+=(pixel>>8)&255;blue+=pixel&255;
                }
                m_simpleContrastSamples.push_back(D2D1::ColorF(red/(25*255),green/(25*255),blue/(25*255),1));
            }
            const auto properties=D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED));
            m_ctx->CreateBitmap({width,height},pixels,width*4,&properties,bitmap.put());
        }
        SelectObject(memory,previous);
    }
    if (capture) DeleteObject(capture);
    if (memory) DeleteDC(memory);
    if (screen) ReleaseDC(nullptr,screen);
    if (!bitmap) return;
    auto target=CreateTargetBitmap(width,height);
    com_ptr<ID2D1Effect> blur;
    if (!target || FAILED(m_ctx->CreateEffect(CLSID_D2D1GaussianBlur,blur.put()))) return;
    blur->SetInput(0,bitmap.get());
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,std::max(18.0f,m_settings.blurAmount)*captureScale);
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE,(UINT32)D2D1_BORDER_MODE_HARD);
    m_ctx->SetTarget(target.get()); m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0,0,0,1)); m_ctx->DrawImage(blur.get());
    m_brush->SetColor(D2D1::ColorF(0,0,0,0));
    m_ctx->FillRectangle({0,0,(float)width,(float)height},m_brush.get());
    const HRESULT result=m_ctx->EndDraw(); m_ctx->SetTarget(nullptr);
    if (SUCCEEDED(result)) m_background=target;
}

void Switcher::UpdateBackground() {
    const UINT width = m_swapWidth;
    const UINT height = m_swapHeight;

    std::wstring path;
    COLORREF color = 0;
    DESKTOP_WALLPAPER_POSITION position = DWPOS_FILL;

    com_ptr<IDesktopWallpaper> wallpaper;
    if (SUCCEEDED(CoCreateInstance(kCLSID_DesktopWallpaper, nullptr,
                                   CLSCTX_ALL,
                                   IID_PPV_ARGS(wallpaper.put())))) {
        wallpaper->GetBackgroundColor(&color);
        wallpaper->GetPosition(&position);

        // Use the wallpaper of the monitor closest to ours.
        UINT count = 0;
        wallpaper->GetMonitorDevicePathCount(&count);
        const LONG centerX = (m_monitor.left + m_monitor.right) / 2;
        const LONG centerY = (m_monitor.top + m_monitor.bottom) / 2;
        long long bestDistance = LLONG_MAX;
        for (UINT i = 0; i < count; i++) {
            LPWSTR monitorId = nullptr;
            if (FAILED(wallpaper->GetMonitorDevicePathAt(i, &monitorId))) {
                continue;
            }
            RECT rect;
            LPWSTR monitorPath = nullptr;
            if (SUCCEEDED(wallpaper->GetMonitorRECT(monitorId, &rect)) &&
                SUCCEEDED(wallpaper->GetWallpaper(monitorId, &monitorPath))) {
                const long long dx = (rect.left + rect.right) / 2 - centerX;
                const long long dy = (rect.top + rect.bottom) / 2 - centerY;
                if (dx * dx + dy * dy < bestDistance) {
                    bestDistance = dx * dx + dy * dy;
                    path = monitorPath ? monitorPath : L"";
                }
            }
            CoTaskMemFree(monitorPath);
            CoTaskMemFree(monitorId);
        }
    }

    // The wallpaper file is often replaced in place, so include its time.
    WIN32_FILE_ATTRIBUTE_DATA attributes{};
    if (!path.empty() && !GetFileAttributesExW(
                             path.c_str(), GetFileExInfoStandard, &attributes)) {
        path.clear();
    }
    const ULONGLONG fileTime =
        ((ULONGLONG)attributes.ftLastWriteTime.dwHighDateTime << 32) |
        attributes.ftLastWriteTime.dwLowDateTime;
    const std::wstring key =
        path + L"|" + std::to_wstring(fileTime) + L"|" +
        std::to_wstring(width) + L"x" + std::to_wstring(height) + L"|" +
        std::to_wstring(color) + L"|" + std::to_wstring((int)position) + L"|" +
        std::to_wstring(m_settings.blurAmount);
    if (m_background && key == m_backgroundKey) {
        return;
    }
    m_background = nullptr;
    m_backgroundKey = key;

    // Decode the image, scaled to the size it's displayed at.
    com_ptr<ID2D1Bitmap1> image;
    D2D1_RECT_F imageRect{};
    com_ptr<IWICBitmapDecoder> decoder;
    com_ptr<IWICBitmapFrameDecode> frame;
    UINT imageWidth = 0, imageHeight = 0;
    if (!path.empty() &&
        SUCCEEDED(m_wicFactory->CreateDecoderFromFilename(
            path.c_str(), nullptr, GENERIC_READ,
            WICDecodeMetadataCacheOnDemand, decoder.put())) &&
        SUCCEEDED(decoder->GetFrame(0, frame.put())) &&
        SUCCEEDED(frame->GetSize(&imageWidth, &imageHeight)) && imageWidth &&
        imageHeight) {
        float drawWidth = (float)width;
        float drawHeight = (float)height;
        const float scaleX = width / (float)imageWidth;
        const float scaleY = height / (float)imageHeight;
        switch (position) {
            case DWPOS_FIT:
                drawWidth = imageWidth * std::min(scaleX, scaleY);
                drawHeight = imageHeight * std::min(scaleX, scaleY);
                break;
            case DWPOS_STRETCH:
                break;
            case DWPOS_CENTER:
                drawWidth = (float)imageWidth;
                drawHeight = (float)imageHeight;
                break;
            default:  // Fill, span and tile.
                drawWidth = imageWidth * std::max(scaleX, scaleY);
                drawHeight = imageHeight * std::max(scaleX, scaleY);
                break;
        }
        imageRect = {(width - drawWidth) / 2, (height - drawHeight) / 2,
                     (width + drawWidth) / 2, (height + drawHeight) / 2};

        com_ptr<IWICBitmapScaler> scaler;
        com_ptr<IWICFormatConverter> converter;
        const UINT scaledWidth = (UINT)std::max(std::lround(drawWidth), 1L);
        const UINT scaledHeight = (UINT)std::max(std::lround(drawHeight), 1L);
        if (SUCCEEDED(m_wicFactory->CreateBitmapScaler(scaler.put())) &&
            SUCCEEDED(scaler->Initialize(frame.get(), scaledWidth,
                                         scaledHeight,
                                         WICBitmapInterpolationModeFant)) &&
            SUCCEEDED(m_wicFactory->CreateFormatConverter(converter.put())) &&
            SUCCEEDED(converter->Initialize(
                scaler.get(), GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone, nullptr, 0,
                WICBitmapPaletteTypeMedianCut))) {
            m_ctx->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                             image.put());
        }
    }

    com_ptr<ID2D1Bitmap1> composed = CreateTargetBitmap(width, height);
    if (!composed) {
        return;
    }
    m_ctx->SetTarget(composed.get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(GetRValue(color) / 255.0f,
                              GetGValue(color) / 255.0f,
                              GetBValue(color) / 255.0f, 1));
    if (image) {
        m_ctx->DrawBitmap(image.get(), imageRect, 1,
                          D2D1_INTERPOLATION_MODE_LINEAR);
    }

    HRESULT hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    if (FAILED(hr)) {
        return;
    }

    com_ptr<ID2D1Effect> blur;
    com_ptr<ID2D1Bitmap1> blurred = CreateTargetBitmap(width, height);
    if (!blurred ||
        FAILED(m_ctx->CreateEffect(CLSID_D2D1GaussianBlur, blur.put()))) {
        m_background = composed;
        return;
    }
    blur->SetInput(0, composed.get());
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
                   m_settings.blurAmount);
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE,
                   (UINT32)D2D1_BORDER_MODE_HARD);
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION,
                   kGaussianBlurOptimizationQuality);

    m_ctx->SetTarget(blurred.get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 1));
    m_ctx->DrawImage(blur.get());
    hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    m_background = SUCCEEDED(hr) ? blurred : composed;
}

DWORD WINAPI UiThreadProc(LPVOID parameter) {
    // Work in physical pixels everywhere.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"init_apartment failed: 0x%08X", (UINT)e.code());
        SetEvent((HANDLE)parameter);
        return 1;
    }

    {
        Switcher switcher;
        g_switcher = &switcher;
        const bool created = switcher.CreateOverlay();
        if (created) {
            switcher.Activate();
        }
        // Do not let the hook thread start until graphics initialization and
        // g_ready are complete. Otherwise the first Alt+Tab can leak through
        // to the native switcher during startup.
        SetEvent((HANDLE)parameter);
        if (created) {
            switcher.Run();
        }
        switcher.Shutdown();
        g_switcher = nullptr;
    }

    winrt::uninit_apartment();
    return 0;
}

bool StartThread(LPTHREAD_START_ROUTINE proc, HANDLE* thread, DWORD* threadId) {
    HANDLE started = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!started) {
        return false;
    }
    *thread = CreateThread(nullptr, 0, proc, started, 0, threadId);
    if (*thread) {
        WaitForSingleObject(started, INFINITE);
    }
    CloseHandle(started);
    return *thread != nullptr;
}

void StopThread(HANDLE* thread, DWORD threadId) {
    if (!*thread) {
        return;
    }
    PostThreadMessageW(threadId, WM_QUIT, 0, 0);
    WaitForSingleObject(*thread, INFINITE);
    CloseHandle(*thread);
    *thread = nullptr;
}

void InitModuleGlobals() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&InitModuleGlobals),
                       &g_module);
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// The switcher UI and input threads run in Windhawk’s tool process.

BOOL WhTool_ModInit() {
    Wh_Log(L">");

    InitModuleGlobals();

    if (!StartThread(UiThreadProc, &g_uiThread, &g_uiThreadId) ||
        !g_overlayWnd) {
        Wh_Log(L"Failed to start the UI thread");
        StopThread(&g_uiThread, g_uiThreadId);
        return FALSE;
    }

    if (!StartThread(HookThreadProc, &g_hookThread, &g_hookThreadId)) {
        Wh_Log(L"Failed to start the hook thread");
        StopThread(&g_uiThread, g_uiThreadId);
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L">");

    g_ready = false;
    StopThread(&g_hookThread, g_hookThreadId);
    g_switching = false;
    StopThread(&g_uiThread, g_uiThreadId);
    g_overlayWnd = nullptr;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");

    if (g_overlayWnd) {
        PostMessageW(g_overlayWnd, WM_APP_SETTINGS, 0, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace covertabby

// Cache the real Windows build once; RtlGetVersion is manifest-independent.
struct CoverTabbyWindowsVersion {
    bool supported = false;
    bool isWindows11 = false;
};

static const CoverTabbyWindowsVersion& GetCoverTabbyWindowsVersion() {
    static const CoverTabbyWindowsVersion info = [] {
        using RtlGetVersionFn = LONG(WINAPI*)(OSVERSIONINFOW*);
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        auto getVersion = ntdll ? reinterpret_cast<RtlGetVersionFn>(
            GetProcAddress(ntdll, "RtlGetVersion")) : nullptr;
        OSVERSIONINFOW version{};
        version.dwOSVersionInfoSize = sizeof(version);
        if (!getVersion || getVersion(&version) != 0) {
            return CoverTabbyWindowsVersion{};
        }
        return CoverTabbyWindowsVersion{
            version.dwMajorVersion == 10 && version.dwBuildNumber >= 14393,
            version.dwMajorVersion == 10 && version.dwBuildNumber >= 22000
        };
    }();
    return info;
}

// Segoe Fluent Icons on Windows 11, Segoe MDL2 Assets on Windows 10.
static PCWSTR CoverTabbyIconFont() {
    return GetCoverTabbyWindowsVersion().isWindows11
        ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";
}

static bool IsSupportedWindowsVersion() {
    return GetCoverTabbyWindowsVersion().supported;
}

// Explorer only forwards its Alt+Tab hotkey; rendering and input ownership live in the tool process.
namespace CoverTabbyShellBridge {
constexpr PCWSTR kAltTabThreadName=L"Immersive Shell";
constexpr DWORD kFindAltTabThreadIntervalMs=2000;
constexpr DWORD kFindAltTabThreadMaxIntervalMs=60000;
constexpr DWORD kWaitForTaskbarIntervalMs=1000;
// Explorer thread only.
HHOOK g_messageHook;
HANDLE g_messageHookThread;  // To know when it ends.
std::atomic<int> g_messageHookCalls;
HANDLE g_explorerStopEvent;
HANDLE g_explorerThread;

// Hands the hotkey to the switcher. Returns false if the switcher isn't running
// or can't take over Alt+Tab right now; the native switcher then opens.
bool ForwardHotkey(WPARAM flags) {
    HWND overlay=FindWindowW(covertabby::kOverlayClassName,covertabby::kOverlayTitleReady);
    if (!overlay) return false;
    DWORD pid=0;GetWindowThreadProcessId(overlay,&pid);
    if (pid) AllowSetForegroundWindow(pid);
    return PostMessageW(overlay,WM_APP+35,flags,0)!=FALSE;
}
LRESULT CALLBACK GetMessageProc(int code,WPARAM wParam,LPARAM lParam) {
    ++g_messageHookCalls;
    auto* msg=reinterpret_cast<MSG*>(lParam);
    if (code==HC_ACTION && wParam==PM_REMOVE && msg && msg->message==WM_HOTKEY &&
        HIWORD(msg->lParam)==VK_TAB) {
        UINT modifiers=LOWORD(msg->lParam);
        if ((modifiers&MOD_ALT) && !(modifiers&(MOD_CONTROL|MOD_WIN)) &&
            ForwardHotkey((modifiers&MOD_SHIFT)?covertabby::kStartBackwards:0)) {
            msg->message=WM_NULL;msg->wParam=0;msg->lParam=0;
        }
    }
    LRESULT result=CallNextHookEx(nullptr,code,wParam,lParam);
    --g_messageHookCalls;
    return result;
}

// The ID of explorer's thread that receives the hotkey, or 0 if it doesn't
// exist now.
DWORD FindAltTabThread() {
    DWORD found = 0;
    const DWORD processId = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return found;
    }

    THREADENTRY32 entry{sizeof(entry)};
    for (BOOL more = Thread32First(snapshot, &entry); more && !found;
         more = Thread32Next(snapshot, &entry)) {
        if (entry.th32OwnerProcessID != processId) {
            continue;
        }
        HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE,
                                   entry.th32ThreadID);
        if (!thread) {
            continue;
        }
        PWSTR name = nullptr;
        if (SUCCEEDED(GetThreadDescription(thread, &name)) && name &&
            wcscmp(name, kAltTabThreadName) == 0) {
            found = entry.th32ThreadID;
        }
        if (name) {
            LocalFree(name);
        }
        CloseHandle(thread);
    }

    CloseHandle(snapshot);
    return found;
}

// Hooks the thread that receives the hotkey, if it exists now. Returns true if
// it's hooked.
bool HookAltTabThread() {
    const DWORD threadId = FindAltTabThread();
    if (!threadId) {
        return false;
    }
    HANDLE thread = OpenThread(SYNCHRONIZE, FALSE, threadId);
    if (!thread) {
        return false;
    }
    // A thread of this process: no module (SetWindowsHookEx docs).
    HHOOK hook =
        SetWindowsHookExW(WH_GETMESSAGE, GetMessageProc, nullptr, threadId);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx(WH_GETMESSAGE) failed: %u", GetLastError());
        CloseHandle(thread);
        return false;
    }
    Wh_Log(L"Hooked explorer's thread %u (%s)", threadId, kAltTabThreadName);
    g_messageHook = hook;
    g_messageHookThread = thread;
    return true;
}

void UnhookAltTabThread() {
    if (g_messageHook) {
        UnhookWindowsHookEx(g_messageHook);
        g_messageHook = nullptr;
    }
    if (g_messageHookThread) {
        CloseHandle(g_messageHookThread);
        g_messageHookThread = nullptr;
    }

    // Let calls already running on explorer's thread leave the module before
    // it gets unloaded. No new calls can start after the hook is removed, so
    // this always ends.
    const ULONGLONG start = GetTickCount64();
    bool logged = false;
    while (g_messageHookCalls > 0) {
        if (!logged && GetTickCount64() - start > 1000) {
            Wh_Log(L"Still waiting for message hook calls to return");
            logged = true;
        }
        Sleep(1);
    }
}

DWORD WINAPI ExplorerThreadProc(LPVOID) {
    // Only the shell process (the one with the taskbar) receives the hotkey.
    // With "Launch folder windows in a separate process", folder windows run
    // in other explorer processes, which have nothing to do.
    for (;;) {
        if (HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr)) {
            DWORD processId = 0;
            GetWindowThreadProcessId(taskbar, &processId);
            if (processId != GetCurrentProcessId()) {
                Wh_Log(L"Not the shell process, nothing to do");
                return 0;
            }
            break;
        }
        // Explorer is still starting up.
        if (WaitForSingleObject(g_explorerStopEvent,
                                kWaitForTaskbarIntervalMs) != WAIT_TIMEOUT) {
            return 0;
        }
    }

    // Hooks the thread, and again if it's ever made anew. Until it's hooked,
    // it's looked for again, less and less often.
    DWORD interval = kFindAltTabThreadIntervalMs;
    bool loggedMissing = false;
    for (;;) {
        if (!g_messageHook && !HookAltTabThread() && !loggedMissing) {
            Wh_Log(L"Explorer's \"%s\" thread wasn't found yet",
                   kAltTabThreadName);
            loggedMissing = true;
        }

        const HANDLE events[] = {g_explorerStopEvent, g_messageHookThread};
        const DWORD result = WaitForMultipleObjects(
            g_messageHook ? 2 : 1, events, FALSE,
            g_messageHook ? INFINITE : interval);
        if (result == WAIT_TIMEOUT) {
            interval = std::min(interval * 2, kFindAltTabThreadMaxIntervalMs);
        } else if (result == WAIT_OBJECT_0 + 1) {
            // The thread ended (its ID can be reused).
            UnhookAltTabThread();
            interval = kFindAltTabThreadIntervalMs;
            loggedMissing = false;
        } else {
            break;
        }
    }

    UnhookAltTabThread();
    return 0;
}

void StopExplorerPart() {
    if (g_explorerThread) {
        SetEvent(g_explorerStopEvent);
        WaitForSingleObject(g_explorerThread, INFINITE);
        CloseHandle(g_explorerThread);
        g_explorerThread = nullptr;
    }
    if (g_explorerStopEvent) {
        CloseHandle(g_explorerStopEvent);
        g_explorerStopEvent = nullptr;
    }
}

// Doesn't wait for anything, so explorer's startup isn't delayed.
void StartExplorerPart() {
    g_explorerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_explorerStopEvent) {
        return;
    }
    g_explorerThread =
        CreateThread(nullptr, 0, ExplorerThreadProc, nullptr, 0, nullptr);
    if (!g_explorerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        StopExplorerPart();
    }
}


BOOL Init() {
    if (!IsSupportedWindowsVersion()) return FALSE;
    StartExplorerPart();return g_explorerThread!=nullptr;
}
}
bool g_isExplorer=false;
static bool IsCoverTabbyExplorerProcess() {
    WCHAR path[MAX_PATH]{};GetModuleFileNameW(nullptr,path,ARRAYSIZE(path));
    PCWSTR name=wcsrchr(path,L'\\');name=name?name+1:path;
    return _wcsicmp(name,L"explorer.exe")==0;
}
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    // explorer.exe part.
    if (IsCoverTabbyExplorerProcess()) {
        g_isExplorer = true;
        return CoverTabbyShellBridge::Init();
    }

    if (!IsSupportedWindowsVersion()) return FALSE;
    WCHAR hostPath[MAX_PATH]{};GetModuleFileNameW(nullptr,hostPath,ARRAYSIZE(hostPath));
    PCWSTR hostName=wcsrchr(hostPath,L'\\');hostName=hostName?hostName+1:hostPath;
    if (_wcsicmp(hostName,L"windhawk.exe") && _wcsicmp(hostName,L"windhawk-mod.exe")) return FALSE;
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

        if (!covertabby::WhTool_ModInit()) {
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
    // explorer.exe part.
    if (g_isExplorer) {
        return; // Explorer's Alt+Tab forwarder has no mod settings to reload.
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    covertabby::WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    // explorer.exe part.
    if (g_isExplorer) {
        CoverTabbyShellBridge::StopExplorerPart();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    covertabby::WhTool_ModUninit();
    ExitProcess(0);
}

/*
THIRD-PARTY LICENSE NOTICES

MIT License — renderer portions by caliberda (cesarkali)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

GPL-2.0-only

                    GNU GENERAL PUBLIC LICENSE
                       Version 2, June 1991

 Copyright (C) 1989, 1991 Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

                            Preamble

  The licenses for most software are designed to take away your
freedom to share and change it.  By contrast, the GNU General Public
License is intended to guarantee your freedom to share and change free
software--to make sure the software is free for all its users.  This
General Public License applies to most of the Free Software
Foundation's software and to any other program whose authors commit to
using it.  (Some other Free Software Foundation software is covered by
the GNU Lesser General Public License instead.)  You can apply it to
your programs, too.

  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
this service if you wish), that you receive source code or can get it
if you want it, that you can change the software or use pieces of it
in new free programs; and that you know you can do these things.

  To protect your rights, we need to make restrictions that forbid
anyone to deny you these rights or to ask you to surrender the rights.
These restrictions translate to certain responsibilities for you if you
distribute copies of the software, or if you modify it.

  For example, if you distribute copies of such a program, whether
gratis or for a fee, you must give the recipients all the rights that
you have.  You must make sure that they, too, receive or can get the
source code.  And you must show them these terms so they know their
rights.

  We protect your rights with two steps: (1) copyright the software, and
(2) offer you this license which gives you legal permission to copy,
distribute and/or modify the software.

  Also, for each author's protection and ours, we want to make certain
that everyone understands that there is no warranty for this free
software.  If the software is modified by someone else and passed on, we
want its recipients to know that what they have is not the original, so
that any problems introduced by others will not reflect on the original
authors' reputations.

  Finally, any free program is threatened constantly by software
patents.  We wish to avoid the danger that redistributors of a free
program will individually obtain patent licenses, in effect making the
program proprietary.  To prevent this, we have made it clear that any
patent must be licensed for everyone's free use or not licensed at all.

  The precise terms and conditions for copying, distribution and
modification follow.

                    GNU GENERAL PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. This License applies to any program or other work which contains
a notice placed by the copyright holder saying it may be distributed
under the terms of this General Public License.  The "Program", below,
refers to any such program or work, and a "work based on the Program"
means either the Program or any derivative work under copyright law:
that is to say, a work containing the Program or a portion of it,
either verbatim or with modifications and/or translated into another
language.  (Hereinafter, translation is included without limitation in
the term "modification".)  Each licensee is addressed as "you".

Activities other than copying, distribution and modification are not
covered by this License; they are outside its scope.  The act of
running the Program is not restricted, and the output from the Program
is covered only if its contents constitute a work based on the
Program (independent of having been made by running the Program).
Whether that is true depends on what the Program does.

  1. You may copy and distribute verbatim copies of the Program's
source code as you receive it, in any medium, provided that you
conspicuously and appropriately publish on each copy an appropriate
copyright notice and disclaimer of warranty; keep intact all the
notices that refer to this License and to the absence of any warranty;
and give any other recipients of the Program a copy of this License
along with the Program.

You may charge a fee for the physical act of transferring a copy, and
you may at your option offer warranty protection in exchange for a fee.

  2. You may modify your copy or copies of the Program or any portion
of it, thus forming a work based on the Program, and copy and
distribute such modifications or work under the terms of Section 1
above, provided that you also meet all of these conditions:

    a) You must cause the modified files to carry prominent notices
    stating that you changed the files and the date of any change.

    b) You must cause any work that you distribute or publish, that in
    whole or in part contains or is derived from the Program or any
    part thereof, to be licensed as a whole at no charge to all third
    parties under the terms of this License.

    c) If the modified program normally reads commands interactively
    when run, you must cause it, when started running for such
    interactive use in the most ordinary way, to print or display an
    announcement including an appropriate copyright notice and a
    notice that there is no warranty (or else, saying that you provide
    a warranty) and that users may redistribute the program under
    these conditions, and telling the user how to view a copy of this
    License.  (Exception: if the Program itself is interactive but
    does not normally print such an announcement, your work based on
    the Program is not required to print an announcement.)

These requirements apply to the modified work as a whole.  If
identifiable sections of that work are not derived from the Program,
and can be reasonably considered independent and separate works in
themselves, then this License, and its terms, do not apply to those
sections when you distribute them as separate works.  But when you
distribute the same sections as part of a whole which is a work based
on the Program, the distribution of the whole must be on the terms of
this License, whose permissions for other licensees extend to the
entire whole, and thus to each and every part regardless of who wrote it.

Thus, it is not the intent of this section to claim rights or contest
your rights to work written entirely by you; rather, the intent is to
exercise the right to control the distribution of derivative or
collective works based on the Program.

In addition, mere aggregation of another work not based on the Program
with the Program (or with a work based on the Program) on a volume of
a storage or distribution medium does not bring the other work under
the scope of this License.

  3. You may copy and distribute the Program (or a work based on it,
under Section 2) in object code or executable form under the terms of
Sections 1 and 2 above provided that you also do one of the following:

    a) Accompany it with the complete corresponding machine-readable
    source code, which must be distributed under the terms of Sections
    1 and 2 above on a medium customarily used for software interchange; or,

    b) Accompany it with a written offer, valid for at least three
    years, to give any third party, for a charge no more than your
    cost of physically performing source distribution, a complete
    machine-readable copy of the corresponding source code, to be
    distributed under the terms of Sections 1 and 2 above on a medium
    customarily used for software interchange; or,

    c) Accompany it with the information you received as to the offer
    to distribute corresponding source code.  (This alternative is
    allowed only for noncommercial distribution and only if you
    received the program in object code or executable form with such
    an offer, in accord with Subsection b above.)

The source code for a work means the preferred form of the work for
making modifications to it.  For an executable work, complete source
code means all the source code for all modules it contains, plus any
associated interface definition files, plus the scripts used to
control compilation and installation of the executable.  However, as a
special exception, the source code distributed need not include
anything that is normally distributed (in either source or binary
form) with the major components (compiler, kernel, and so on) of the
operating system on which the executable runs, unless that component
itself accompanies the executable.

If distribution of executable or object code is made by offering
access to copy from a designated place, then offering equivalent
access to copy the source code from the same place counts as
distribution of the source code, even though third parties are not
compelled to copy the source along with the object code.

  4. You may not copy, modify, sublicense, or distribute the Program
except as expressly provided under this License.  Any attempt
otherwise to copy, modify, sublicense or distribute the Program is
void, and will automatically terminate your rights under this License.
However, parties who have received copies, or rights, from you under
this License will not have their licenses terminated so long as such
parties remain in full compliance.

  5. You are not required to accept this License, since you have not
signed it.  However, nothing else grants you permission to modify or
distribute the Program or its derivative works.  These actions are
prohibited by law if you do not accept this License.  Therefore, by
modifying or distributing the Program (or any work based on the
Program), you indicate your acceptance of this License to do so, and
all its terms and conditions for copying, distributing or modifying
the Program or works based on it.

  6. Each time you redistribute the Program (or any work based on the
Program), the recipient automatically receives a license from the
original licensor to copy, distribute or modify the Program subject to
these terms and conditions.  You may not impose any further
restrictions on the recipients' exercise of the rights granted herein.
You are not responsible for enforcing compliance by third parties to
this License.

  7. If, as a consequence of a court judgment or allegation of patent
infringement or for any other reason (not limited to patent issues),
conditions are imposed on you (whether by court order, agreement or
otherwise) that contradict the conditions of this License, they do not
excuse you from the conditions of this License.  If you cannot
distribute so as to satisfy simultaneously your obligations under this
License and any other pertinent obligations, then as a consequence you
may not distribute the Program at all.  For example, if a patent
license would not permit royalty-free redistribution of the Program by
all those who receive copies directly or indirectly through you, then
the only way you could satisfy both it and this License would be to
refrain entirely from distribution of the Program.

If any portion of this section is held invalid or unenforceable under
any particular circumstance, the balance of the section is intended to
apply and the section as a whole is intended to apply in other
circumstances.

It is not the purpose of this section to induce you to infringe any
patents or other property right claims or to contest validity of any
such claims; this section has the sole purpose of protecting the
integrity of the free software distribution system, which is
implemented by public license practices.  Many people have made
generous contributions to the wide range of software distributed
through that system in reliance on consistent application of that
system; it is up to the author/donor to decide if he or she is willing
to distribute software through any other system and a licensee cannot
impose that choice.

This section is intended to make thoroughly clear what is believed to
be a consequence of the rest of this License.

  8. If the distribution and/or use of the Program is restricted in
certain countries either by patents or by copyrighted interfaces, the
original copyright holder who places the Program under this License
may add an explicit geographical distribution limitation excluding
those countries, so that distribution is permitted only in or among
countries not thus excluded.  In such case, this License incorporates
the limitation as if written in the body of this License.

  9. The Free Software Foundation may publish revised and/or new versions
of the General Public License from time to time.  Such new versions will
be similar in spirit to the present version, but may differ in detail to
address new problems or concerns.

Each version is given a distinguishing version number.  If the Program
specifies a version number of this License which applies to it and "any
later version", you have the option of following the terms and conditions
either of that version or of any later version published by the Free
Software Foundation.  If the Program does not specify a version number of
this License, you may choose any version ever published by the Free Software
Foundation.

  10. If you wish to incorporate parts of the Program into other free
programs whose distribution conditions are different, write to the author
to ask for permission.  For software which is copyrighted by the Free
Software Foundation, write to the Free Software Foundation; we sometimes
make exceptions for this.  Our decision will be guided by the two goals
of preserving the free status of all derivatives of our free software and
of promoting the sharing and reuse of software generally.

                            NO WARRANTY

  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
REPAIR OR CORRECTION.

  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED
TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

                     END OF TERMS AND CONDITIONS

            How to Apply These Terms to Your New Programs

  If you develop a new program, and you want it to be of the greatest
possible use to the public, the best way to achieve this is to make it
free software which everyone can redistribute and change under these terms.

  To do so, attach the following notices to the program.  It is safest
to attach them to the start of each source file to most effectively
convey the exclusion of warranty; and each file should have at least
the "copyright" line and a pointer to where the full notice is found.

    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

Also add information on how to contact you by electronic and paper mail.

If the program is interactive, make it output a short notice like this
when it starts in an interactive mode:

    Gnomovision version 69, Copyright (C) year name of author
    Gnomovision comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
    This is free software, and you are welcome to redistribute it
    under certain conditions; type `show c' for details.

The hypothetical commands `show w' and `show c' should show the appropriate
parts of the General Public License.  Of course, the commands you use may
be called something other than `show w' and `show c'; they could even be
mouse-clicks or menu items--whatever suits your program.

You should also get your employer (if you work as a programmer) or your
school, if any, to sign a "copyright disclaimer" for the program, if
necessary.  Here is a sample; alter the names:

  Yoyodyne, Inc., hereby disclaims all copyright interest in the program
  `Gnomovision' (which makes passes at compilers) written by James Hacker.

  <signature of Ty Coon>, 1 April 1989
  Ty Coon, President of Vice

This General Public License does not permit incorporating your program into
proprietary programs.  If your program is a subroutine library, you may
consider it more useful to permit linking proprietary applications with the
library.  If this is what you want to do, use the GNU Lesser General
Public License instead of this License.

*/
