// ==WindhawkMod==
// @id              startpad
// @name            StartPad
// @description     macOS-style fullscreen app launcher with pages, folders, and drag-and-drop organization
// @version         1.1
// @author          Frankh93
// @github          https://github.com/frankh93
// @donateUrl       https://www.paypal.me/frankh1993
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshell32 -luser32 -lgdi32 -lmsimg32 -luuid -ladvapi32 -ldwmapi -lpowrprof
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# StartPad

![StartPad Preview](https://raw.githubusercontent.com/frankh93/StartPad/main/preview.png)

A macOS Launchpad-style fullscreen app launcher that replaces the Windows 11 Start menu, featuring horizontal paginated grids, interactive app folders, drag-and-drop reordering, and customizable visual themes.

## Features & How to Use

### Pages & Navigation
- **Horizontal Pages**: Apps are organized into pages rather than a vertical scrolling list.
- **Switching Pages**: Click and drag horizontally with the mouse, scroll the mouse wheel (vertical or horizontal), click the pagination dots at the bottom, or use **Page Up / Page Down**.
- **2D Arrow Key Navigation**: Use the **Arrow keys** or **Tab** to move selection across icons. Moving past the left or right edge of the grid automatically flips to the previous or next page.

### Drag-and-Drop & Folders
- **Reorder Apps**: **Click and hold** (~320 ms) on any app or folder icon to pick it up, then drag it to a new position on the grid or across pages (hold near the screen edge to flip pages).
- **Create Folders**:
  - **Drag & Drop**: Drag one app icon directly onto another app icon (or into an existing folder).
  - **Selection Mode**: Click the **More (`...`)** button on the right side of the search bar and choose **Create folder**, click the apps you want to bundle (marked with a blue checkmark), and click the **Done (`✓`)** button in the search bar (or press **Enter**).
- **Rename Folders**: When a folder is open, **click its title** at the top or press **F2** (also works when a folder is selected on the main grid), type the new name, and press **Enter**.
- **Remove or Ungroup**: Drag an icon outside the open folder panel to move it back to the main grid, or right-click a folder and select **Ungroup folder**.
- **Persistent Layout**: Your custom icon order, folders, and hidden apps are automatically saved to Windhawk's mod storage per user.

### Search & Launching
- **Instant Search**: Start typing anywhere to filter apps immediately. Press **Esc** or click the clear (`×`) button to reset the search.
- **Launch App**: Click an icon or press **Enter** on the selected app.
- **Run as Administrator**: Hold **Ctrl + Shift + Enter** on a selected app, or right-click the app and choose **Run as administrator**.
- **Context Menu**: Right-click any app to **Open**, **Run as administrator**, **Open file location**, **Remove from folder**, or **Hide**.

### More Options (`...`) & Hiding Apps
Click the **`...`** button inside the right side of the search bar to access:
- **Create folder**: Interactively select multiple apps to group into a new folder.
- **Edit hidden apps**: Toggle visibility badges on all apps to hide or unhide multiple items at once, then click the **Done (`✓`)** button in the search bar to save.
- **Search in hidden apps**: When checked, typing a search query will also return matching apps that you have hidden.
- **Reset layout**: Restores the default alphabetical layout and removes all custom folders and hidden-app rules.

### Power Menu & Visuals
- **Power Button**: Located at the bottom right (aligned with the page dots and last column), providing quick access to **Lock**, **Sign out**, **Sleep**, **Shut down**, and **Restart**.
- **Visual Styles**: Choose between **MacOS Style** (pill search bar & squircle folders) and **Windows 11 Fluent** (Acrylic-inspired search box with system accent bar), automatically adapting to Windows Light and Dark modes.
- **Legacy Icon Cleanup**: Automatically detects and removes the thin 1px border frame Windows adds around small legacy Win32 icons (such as 7-Zip) and smoothly upscales them.
- **App Name Text Size**: Scale the app and folder names under the icons from 60% to 200% in the settings. The hover zoom animation and the selection rectangle scale with it and stay fully inside their redraw area (no clipped edges).
- **Localized UI**: Built-in translations for English, Spanish, French, German, Italian, Portuguese, Russian, Chinese, Japanese, and Korean.

## How StartPad differs from "fullscreen-start" mod
Both mods replace the Start menu with a fullscreen app launcher, and **they cannot be enabled at the same time** because both intercept `XamlLauncher::ShowStartView`.
- **StartPad** focuses on a **paginated macOS Launchpad workflow**: horizontal pages with spring physics, **folders** (creation, renaming, ungrouping), **drag-and-drop icon reordering** with persistent layout storage, **hiding apps**, a **right-click context menu** (Run as administrator, Open file location), an integrated **Power menu**, a **Windows 11 Fluent visual style**, and **multilingual UI strings**.
- **Fullscreen Start (`fullscreen-start`)** by Rafael Mendes focuses on a single **vertically scrolled grid** with Direct2D rendering, live DWM Acrylic/Blur backdrops, `Ctrl+V` clipboard paste in search, keeping the taskbar visible, and integration with the Standalone Dock mod.

## Credits & License
- Hooking `XamlLauncher::ShowStartView` in `twinui.pcshell.dll` (`kShowMethodKeyboard` / `kShowFlagKeyboard`), secondary folder-process detection (`IsFolderWindowsProcess`), and thread initialization/uninitialization structure are adapted from [**Fullscreen Start (Launchpad style)** (`fullscreen-start`)](https://github.com/RafaelMendes1407) by **Rafael Mendes**, licensed under the MIT License (Copyright (c) Rafael Mendes).
- Licensed under the **MIT License**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- behavior:
  - triggerWinKey: true
    $name: Open with the Windows key & Ctrl+Esc
    $description: When off, the Windows key and Ctrl+Esc open the original Start menu.
  - triggerStartButton: true
    $name: Open with the taskbar Start button
    $description: Left-clicks on the Start button open the launcher. When off, it opens the original Start menu.
  $name: Behavior & Triggers
  $description: Settings related to how StartPad is launched
- layout:
  - gridColumns: 8
    $name: Columns per page
    $description: "4 to 12 (Suggested: 8)."
  - gridRows: 5
    $name: Rows per page
    $description: "3 to 8 (Suggested: 5)."
  $name: Grid Layout
  $description: Configure grid dimensions
- visuals:
  - visualStyle: macos
    $name: Visual style
    $options:
    - macos: MacOS Style
    - windows: Windows 11 Fluent
  - iconSize: 100
    $name: Icon size (%)
    $description: 50 to 150. Relative to the default size for the chosen grid.
  - textSize: 100
    $name: App name text size
    $description: 60 to 200. Size of the app and folder names under the icons.
  - searchTextSize: 100
    $name: Search bar text size
    $description: 50 to 150. Size of the text in the search bar.
  - backgroundDarkness: 35
    $name: Background tint intensity
    $description: 0 shows the blurred desktop as it is, 100 is a solid theme colour.
  - blur: medium
    $name: Background blur
    $options:
    - "off": Off (sharp desktop)
    - light: Light
    - medium: Medium
    - strong: Strong
  - animations: true
    $name: Open/close animation
    $description: Also skipped when Windows animations are turned off.
  $name: Appearance & Visuals
  $description: Themes, scaling, and effects
- features:
  - hideUninstallers: true
    $name: Automatically hide uninstallers
    $description: Hides shortcuts like Uninstall, Desinstalar, unins000.exe, etc.
  - showPowerButton: true
    $name: Show power button (bottom right)
  $name: Extra Features
  $description: Additional functionalities
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <powrprof.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <sddl.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwctype>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifndef SPI_GETCLIENTAREAANIMATION
#define SPI_GETCLIENTAREAANIMATION 0x1042
#endif
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef SHUTDOWN_FORCE_OTHERS
#define SHUTDOWN_FORCE_OTHERS 0x00000001
#endif
#ifndef SHUTDOWN_RESTART
#define SHUTDOWN_RESTART 0x00000004
#endif
#ifndef SHUTDOWN_POWEROFF
#define SHUTDOWN_POWEROFF 0x00000008
#endif
#ifndef SHUTDOWN_INSTALL_UPDATES
#define SHUTDOWN_INSTALL_UPDATES 0x00000040
#endif
#ifndef SHUTDOWN_HYBRID
#define SHUTDOWN_HYBRID 0x00000200
#endif
#ifndef ERROR_SHUTDOWN_USERS_LOGGED_ON
#define ERROR_SHUTDOWN_USERS_LOGGED_ON 1190L
#endif

namespace {

constexpr UINT WM_LP_TOGGLE = WM_APP + 1;
constexpr UINT WM_LP_RELOAD = WM_APP + 2;
constexpr UINT WM_LP_SETTINGS = WM_APP + 3;
constexpr UINT WM_LP_PREWARM = WM_APP + 4;
constexpr UINT WM_LP_SHELLCHANGE = WM_APP + 5;
constexpr UINT WM_LP_QUIT = WM_APP + 6;
constexpr UINT WM_LP_PAGESTEP = WM_APP + 7;

constexpr UINT_PTR kAnimTimer = 1;
constexpr UINT_PTR kHoldTimer = 3;
constexpr UINT_PTR kDragTimer = 4;
constexpr UINT_PTR kCaretTimer = 5;
constexpr UINT_PTR kHoverTimer = 6;
constexpr UINT_PTR kFolderAnimTimer = 7;
constexpr UINT_PTR kShellDebounceTimer = 8;
constexpr UINT_PTR kPrewarmTimer = 9;
constexpr UINT_PTR kSnapTimer = 10;
constexpr UINT kHoldMs = 320;
constexpr wchar_t kClassName[] = L"WH_LaunchpadStartWindow";

constexpr int kBlurDownscale = 12;
constexpr int kBlurPasses = 3;
constexpr double kOpenMs = 165.0;
constexpr double kCloseMs = 120.0;
constexpr double kFolderOpenMs = 165.0;
constexpr double kFolderCloseMs = 120.0;
constexpr double kHoverScale = 1.13;
// How much the icon itself grows while hovered/selected (the title uses kHoverScale).
constexpr double kIconHoverScale = 1.08;
constexpr double kIconTitleGapRatio = 0.22;
constexpr int kHoverFontSteps = 16;

struct UiStrings {
    const wchar_t *searchApps, *noResults, *defaultFolder, *open, *runAsAdmin,
        *openFileLoc, *hideApp, *createFolder, *editHiddenApps,
        *searchHiddenApps, *doneEditing, *renameFolder, *ungroupFolder,
        *removeFromFolder, *resetLayout, *resetConfirmMsg, *resetConfirmTitle,
        *lock, *signOut, *sleep, *shutDown, *restart, *othersSignedInMsg;
};

const UiStrings& GetStrings() {
    static const UiStrings kEn = {
        L"Search Applications", L"No Results", L"Folder", L"Open", L"Run as administrator", L"Open file location",
        L"Hide", L"Create folder", L"Edit hidden apps", L"Search in hidden apps", L"Done",
        L"Rename", L"Ungroup folder", L"Remove from folder",
        L"Reset layout",
        L"Are you sure you want to reset the layout?\n\nAll custom icon positions and folders will be removed.",
        L"Reset layout", L"Lock", L"Sign out", L"Sleep", L"Shut down", L"Restart",
        L"Someone else is still using this PC. If you shut down or restart now, they could lose unsaved work.\n\nContinue anyway?"
    };
    static const UiStrings kEs = {
        L"Buscar aplicaciones", L"Sin resultados", L"Carpeta", L"Abrir", L"Ejecutar como administrador", L"Abrir ubicaci\u00F3n del archivo",
        L"Ocultar", L"Crear carpeta", L"Editar aplicaciones ocultas", L"Buscar en aplicaciones ocultas", L"Listo",
        L"Cambiar nombre", L"Desagrupar carpeta", L"Quitar de la carpeta",
        L"Restablecer dise\u00F1o",
        L"\u00BFEst\u00E1s seguro de que deseas restablecer el dise\u00F1o?\n\nSe eliminar\u00E1n todas las carpetas y posiciones personalizadas.",
        L"Restablecer dise\u00F1o", L"Bloquear", L"Cerrar sesi\u00F3n", L"Suspender", L"Apagar", L"Reiniciar",
        L"Otra persona sigue usando este equipo. Si lo apagas o reinicias ahora, podr\u00EDa perder el trabajo no guardado.\n\n\u00BFContinuar de todos modos?"
    };
    static const UiStrings kFr = {
        L"Rechercher des applications", L"Aucun r\u00E9sultat", L"Dossier", L"Ouvrir", L"Ex\u00E9cuter en tant qu\u2019administrateur", L"Ouvrir l\u2019emplacement du fichier",
        L"Masquer", L"Cr\u00E9er un dossier", L"Modifier les applications masqu\u00E9es", L"Rechercher dans les applications masqu\u00E9es", L"Termin\u00E9",
        L"Renommer", L"Dissocier le dossier", L"Retirer du dossier",
        L"R\u00E9initialiser la disposition",
        L"Voulez-vous vraiment r\u00E9initialiser la disposition ?\n\nTous les dossiers et positions personnalis\u00E9s seront supprim\u00E9s.",
        L"R\u00E9initialiser la disposition", L"Verrouiller", L"Se d\u00E9connecter", L"Mettre en veille", L"Arr\u00EAter", L"Red\u00E9marrer",
        L"Une autre personne utilise encore ce PC. Si vous l\u2019arr\u00EAtez ou le red\u00E9marrez maintenant, elle risque de perdre son travail non enregistr\u00E9.\n\nContinuer quand m\u00EAme ?"
    };
    static const UiStrings kDe = {
        L"Programme suchen", L"Keine Ergebnisse", L"Ordner", L"\u00D6ffnen", L"Als Administrator ausf\u00FChren", L"Dateispeicherort \u00F6ffnen",
        L"Ausblenden", L"Ordner erstellen", L"Ausgeblendete Apps bearbeiten", L"In ausgeblendeten Apps suchen", L"Fertig",
        L"Umbenennen", L"Ordner aufl\u00F6sen", L"Aus Ordner entfernen",
        L"Layout zur\u00FCcksetzen",
        L"M\u00F6chten Sie das Layout wirklich zur\u00FCcksetzen?\n\nAlle benutzerdefinierten Positionen und Ordner werden entfernt.",
        L"Layout zur\u00FCcksetzen", L"Sperren", L"Abmelden", L"Energie sparen", L"Herunterfahren", L"Neu starten",
        L"Eine andere Person verwendet diesen PC noch. Wenn Sie ihn jetzt herunterfahren oder neu starten, gehen m\u00F6glicherweise nicht gespeicherte Daten verloren.\n\nTrotzdem fortfahren?"
    };
    static const UiStrings kIt = {
        L"Cerca applicazioni", L"Nessun risultato", L"Cartella", L"Apri", L"Esegui come amministratore", L"Apri percorso file",
        L"Nascondi", L"Crea cartella", L"Modifica app nascoste", L"Cerca nelle app nascoste", L"Fine",
        L"Rinomina", L"Separa cartella", L"Rimuovi dalla cartella",
        L"Reimposta layout",
        L"Sei sicuro di voler reimpostare il layout?\n\nTutte le cartelle e le posizioni personalizzate verranno rimosse.",
        L"Reimposta layout", L"Blocca", L"Disconnetti", L"Sospendi", L"Arresta il sistema", L"Riavvia",
        L"Un\u2019altra persona sta ancora usando questo PC. Se arresti o riavvii il sistema ora, potrebbe perdere il lavoro non salvato.\n\nContinuare comunque?"
    };
    static const UiStrings kPt = {
        L"Pesquisar aplicativos", L"Sem resultados", L"Pasta", L"Abrir", L"Executar como administrador", L"Abrir local do arquivo",
        L"Ocultar", L"Criar pasta", L"Editar aplicativos ocultos", L"Pesquisar em aplicativos ocultos", L"Conclu\u00EDdo",
        L"Renomear", L"Desagrupar pasta", L"Remover da pasta",
        L"Redefinir layout",
        L"Tem certeza de que deseja redefinir o layout?\n\nTodas as pastas e posi\u00E7\u00F5es personalizadas ser\u00E3o removidas.",
        L"Redefinir layout", L"Bloquear", L"Sair", L"Suspender", L"Desligar", L"Reiniciar",
        L"Outra pessoa ainda est\u00E1 usando este PC. Se voc\u00EA desligar ou reiniciar agora, ela poder\u00E1 perder trabalhos n\u00E3o salvos.\n\nContinuar mesmo assim?"
    };
    static const UiStrings kRu = {
        L"\u041F\u043E\u0438\u0441\u043A \u043F\u0440\u0438\u043B\u043E\u0436\u0435\u043D\u0438\u0439", L"\u041D\u0435\u0442 \u0440\u0435\u0437\u0443\u043B\u044C\u0442\u0430\u0442\u043E\u0432", L"\u041F\u0430\u043F\u043A\u0430",
        L"\u041E\u0442\u043A\u0440\u044B\u0442\u044C", L"\u0417\u0430\u043F\u0443\u0441\u043A \u043E\u0442 \u0438\u043C\u0435\u043D\u0438 \u0430\u0434\u043C\u0438\u043D\u0438\u0441\u0442\u0440\u0430\u0442\u043E\u0440\u0430", L"\u0420\u0430\u0441\u043F\u043E\u043B\u043E\u0436\u0435\u043D\u0438\u0435 \u0444\u0430\u0439\u043B\u0430",
        L"\u0421\u043A\u0440\u044B\u0442\u044C", L"\u0421\u043E\u0437\u0434\u0430\u0442\u044C \u043F\u0430\u043F\u043A\u0443", L"\u0421\u043A\u0440\u044B\u0442\u044B\u0435 \u043F\u0440\u0438\u043B\u043E\u0436\u0435\u043D\u0438\u044F", L"\u0418\u0441\u043A\u0430\u0442\u044C \u0432 \u0441\u043A\u0440\u044B\u0442\u044B\u0445", L"\u0413\u043E\u0442\u043E\u0432\u043E",
        L"\u041F\u0435\u0440\u0435\u0438\u043C\u0435\u043D\u043E\u0432\u0430\u0442\u044C", L"\u0420\u0430\u0437\u0433\u0440\u0443\u043F\u043F\u0438\u0440\u043E\u0432\u0430\u0442\u044C \u043F\u0430\u043F\u043A\u0443", L"\u0423\u0431\u0440\u0430\u0442\u044C \u0438\u0437 \u043F\u0430\u043F\u043A\u0438",
        L"\u0421\u0431\u0440\u043E\u0441\u0438\u0442\u044C \u043C\u0430\u043A\u0435\u0442",
        L"\u0412\u044B \u0443\u0432\u0435\u0440\u0435\u043D\u044B, \u0447\u0442\u043E \u0445\u043E\u0442\u0438\u0442\u0435 \u0441\u0431\u0440\u043E\u0441\u0438\u0442\u044C \u043C\u0430\u043A\u0435\u0442?\n\n\u0412\u0441\u0435 \u043F\u0430\u043F\u043A\u0438 \u0438 \u043F\u043E\u0437\u0438\u0446\u0438\u0438 \u0437\u043D\u0430\u0447\u043A\u043E\u0432 \u0431\u0443\u0434\u0443\u0442 \u0441\u0431\u0440\u043E\u0448\u0435\u043D\u044B.",
        L"\u0421\u0431\u0440\u043E\u0441\u0438\u0442\u044C \u043C\u0430\u043A\u0435\u0442", L"\u0417\u0430\u0431\u043B\u043E\u043A\u0438\u0440\u043E\u0432\u0430\u0442\u044C", L"\u0412\u044B\u0439\u0442\u0438", L"\u0421\u043F\u044F\u0449\u0438\u0439 \u0440\u0435\u0436\u0438\u043C", L"\u0417\u0430\u0432\u0435\u0440\u0448\u0435\u043D\u0438\u0435 \u0440\u0430\u0431\u043E\u0442\u044B", L"\u041F\u0435\u0440\u0435\u0437\u0430\u0433\u0440\u0443\u0437\u043A\u0430",
        L"\u042D\u0442\u0438\u043C \u043A\u043E\u043C\u043F\u044C\u044E\u0442\u0435\u0440\u043E\u043C \u043F\u043E\u043B\u044C\u0437\u0443\u0435\u0442\u0441\u044F \u043A\u0442\u043E-\u0442\u043E \u0435\u0449\u0435. \u0415\u0441\u043B\u0438 \u0432\u044B\u043A\u043B\u044E\u0447\u0438\u0442\u044C \u0438\u043B\u0438 \u043F\u0435\u0440\u0435\u0437\u0430\u0433\u0440\u0443\u0437\u0438\u0442\u044C \u0435\u0433\u043E \u0441\u0435\u0439\u0447\u0430\u0441, \u043D\u0435\u0441\u043E\u0445\u0440\u0430\u043D\u0435\u043D\u043D\u044B\u0435 \u0434\u0430\u043D\u043D\u044B\u0435 \u043C\u043E\u0433\u0443\u0442 \u0431\u044B\u0442\u044C \u043F\u043E\u0442\u0435\u0440\u044F\u043D\u044B.\n\n\u0412\u0441\u0435 \u0440\u0430\u0432\u043D\u043E \u043F\u0440\u043E\u0434\u043E\u043B\u0436\u0438\u0442\u044C?"
    };
    static const UiStrings kZh = {
        L"\u641C\u7D22\u5E94\u7528\u7A0B\u5E8F", L"\u65E0\u7ED3\u679C", L"\u6587\u4EF6\u5939", L"\u6253\u5F00", L"\u4EE5\u7BA1\u7406\u5458\u8EAB\u4EFD\u8FD0\u884C", L"\u6253\u5F00\u6587\u4EF6\u4F4D\u7F6E",
        L"\u9690\u85CF", L"\u65B0\u5EFA\u6587\u4EF6\u5939", L"\u7F16\u8F91\u9690\u85CF\u7684\u5E94\u7528", L"\u641C\u7D22\u9690\u85CF\u7684\u5E94\u7528", L"\u5B8C\u6210",
        L"\u91CD\u547D\u540D", L"\u89E3\u6563\u6587\u4EF6\u5939", L"\u4ECE\u6587\u4EF6\u5939\u4E2D\u79FB\u9664",
        L"\u91CD\u7F6E\u5E03\u5C40",
        L"\u786E\u5B9A\u8981\u91CD\u7F6E\u5E03\u5C40\u5417\uFF1F\n\n\u6240\u6709\u81EA\u5B9A\u4E49\u56FE\u6807\u4F4D\u7F6E\u548C\u6587\u4EF6\u5939\u90FD\u5C06\u88AB\u79FB\u9664\u3002",
        L"\u91CD\u7F6E\u5E03\u5C40", L"\u9501\u5B9A", L"\u6CE8\u9500", L"\u7761\u7720", L"\u5173\u673A", L"\u91CD\u542F",
        L"\u8FD8\u6709\u5176\u4ED6\u4EBA\u6B63\u5728\u4F7F\u7528\u6B64\u7535\u8111\u3002\u5982\u679C\u73B0\u5728\u5173\u673A\u6216\u91CD\u542F\uFF0C\u4ED6\u4EEC\u53EF\u80FD\u4F1A\u4E22\u5931\u672A\u4FDD\u5B58\u7684\u5DE5\u4F5C\u3002\n\n\u4ECD\u8981\u7EE7\u7EED\u5417\uFF1F"
    };
    static const UiStrings kJa = {
        L"\u30A2\u30D7\u30EA\u3092\u691C\u7D22", L"\u7D50\u679C\u306A\u3057", L"\u30D5\u30A9\u30EB\u30C0\u30FC", L"\u958B\u304F", L"\u7BA1\u7406\u8005\u3068\u3057\u3066\u5B9F\u884C", L"\u30D5\u30A1\u30A4\u30EB\u306E\u5834\u6240\u3092\u958B\u304F",
        L"\u975E\u8868\u793A", L"\u30D5\u30A9\u30EB\u30C0\u30FC\u3092\u4F5C\u6210", L"\u975E\u8868\u793A\u30A2\u30D7\u30EA\u3092\u7DE8\u96C6", L"\u975E\u8868\u793A\u30A2\u30D7\u30EA\u3082\u691C\u7D22", L"\u5B8C\u4E86",
        L"\u540D\u524D\u306E\u5909\u66F4", L"\u30D5\u30A9\u30EB\u30C0\u30FC\u3092\u89E3\u9664", L"\u30D5\u30A9\u30EB\u30C0\u30FC\u304B\u3089\u524A\u9664",
        L"\u30EC\u30A4\u30A2\u30A6\u30C8\u3092\u30EA\u30BB\u30C3\u30C8",
        L"\u30EC\u30A4\u30A2\u30A6\u30C8\u3092\u30EA\u30BB\u30C3\u30C8\u3057\u3066\u3082\u3088\u308D\u3057\u3044\u3067\u3059\u304B\uFF1F\n\n\u3059\u3079\u3066\u306E\u30AB\u30B9\u30BF\u30E0\u914D\u7F6E\u3068\u30D5\u30A9\u30EB\u30C0\u30FC\u304C\u524A\u9664\u3055\u308C\u307E\u3059\u3002",
        L"\u30EC\u30A4\u30A2\u30A6\u30C8\u3092\u30EA\u30BB\u30C3\u30C8", L"\u30ED\u30C3\u30AF", L"\u30B5\u30A4\u30F3\u30A2\u30A6\u30C8", L"\u30B9\u30EA\u30FC\u30D7", L"\u30B7\u30E3\u30C3\u30C8\u30C0\u30A6\u30F3", L"\u518D\u8D77\u52D5",
        L"\u4ED6\u306E\u30E6\u30FC\u30B6\u30FC\u304C\u3053\u306E PC \u3092\u4F7F\u7528\u3057\u3066\u3044\u307E\u3059\u3002\u4ECA\u3059\u3050\u30B7\u30E3\u30C3\u30C8\u30C0\u30A6\u30F3\u307E\u305F\u306F\u518D\u8D77\u52D5\u3059\u308B\u3068\u3001\u4FDD\u5B58\u3055\u308C\u3066\u3044\u306A\u3044\u4F5C\u696D\u5185\u5BB9\u304C\u5931\u308F\u308C\u308B\u53EF\u80FD\u6027\u304C\u3042\u308A\u307E\u3059\u3002\n\n\u7D9A\u884C\u3057\u307E\u3059\u304B\uFF1F"
    };
    static const UiStrings kKo = {
        L"\uC560\uD50C\uB9AC\uCF00\uC774\uC158 \uAC80\uC0C9", L"\uACB0\uACFC \uC5C6\uC74C", L"\uD3F4\uB354", L"\uC5F4\uAE30", L"\uAD00\uB9AC\uC790 \uAD8C\uD55C\uC73C\uB85C \uC2E4\uD589", L"\uD30C\uC77C \uC704\uCE58 \uC5F4\uAE30",
        L"\uC228\uAE30\uAE30", L"\uD3F4\uB354 \uB9CC\uB4E4\uAE30", L"\uC228\uAE34 \uC571 \uD3B8\uC9D1", L"\uC228\uAE34 \uC571\uB3C4 \uAC80\uC0C9", L"\uC644\uB8CC",
        L"\uC774\uB984 \uBC14\uAFB8\uAE30", L"\uD3F4\uB354 \uD574\uC81C", L"\uD3F4\uB354\uC5D0\uC11C \uC81C\uAC70",
        L"\uB808\uC774\uC544\uC6C3 \uCD08\uAE30\uD654",
        L"\uB808\uC774\uC544\uC6C3\uC744 \uCD08\uAE30\uD654\uD558\uC2DC\uACA0\uC2B5\uB2C8\uAE4C?\n\n\uBAA8\uB4E0 \uC0AC\uC6A9\uC790 \uC9C0\uC815 \uC544\uC774\uCF58 \uC704\uCE58\uC640 \uD3F4\uB354\uAC00 \uC81C\uAC70\uB429\uB2C8\uAE4C.",
        L"\uB808\uC774\uC544\uC6C3 \uCD08\uAE30\uD654", L"\uC7A0\uAE08", L"\uB85C\uADF8\uC544\uC6C3", L"\uC808\uC804", L"\uC2DC\uC2A4\uD15C \uC885\uB8CC", L"\uB2E4\uC2DC \uC2DC\uC791",
        L"\uB2E4\uB978 \uC0AC\uB78C\uC774 \uC544\uC9C1 \uC774 PC\uB97C \uC0AC\uC6A9\uD558\uACE0 \uC788\uC2B5\uB2C8\uB2E4. \uC9C0\uAE08 \uC885\uB8CC\uD558\uAC70\uB098 \uB2E4\uC2DC \uC2DC\uC791\uD558\uBA74 \uC800\uC7A5\uD558\uC9C0 \uC54A\uC740 \uC791\uC5C5\uC744 \uC783\uC744 \uC218 \uC788\uC2B5\uB2C8\uB2E4.\n\n\uACC4\uC18D\uD558\uC2DC\uACA0\uC2B5\uB2C8\uAE4C?"
    };

    LANGID id = GetUserDefaultUILanguage();
    switch (PRIMARYLANGID(id)) {
        case LANG_SPANISH: return kEs;
        case LANG_FRENCH: return kFr;
        case LANG_GERMAN: return kDe;
        case LANG_ITALIAN: return kIt;
        case LANG_PORTUGUESE: return kPt;
        case LANG_RUSSIAN:
        case LANG_UKRAINIAN: return kRu;
        case LANG_CHINESE: return kZh;
        case LANG_JAPANESE: return kJa;
        case LANG_KOREAN: return kKo;
        default: return kEn;
    }
}

volatile LONG g_optWinKey = 1;
volatile LONG g_optStartBtn = 1;
volatile LONG g_optStyle = 0;     // 0 = macOS, 1 = Windows 11 Fluent
volatile LONG g_optCols = 8;
volatile LONG g_optRows = 5;
volatile LONG g_optIconPct = 100;
volatile LONG g_optTextPct = 100;
volatile LONG g_optSearchTextPct = 100;
volatile LONG g_optDark = 35;
volatile LONG g_optBlur = 2;
volatile LONG g_optAnim = 1;
volatile LONG g_optHideUninstallers = 1;
volatile LONG g_optPower = 1;
volatile LONG g_searchHiddenApps = 0; // 1 = include hidden apps in search results
volatile LONG g_appsDirty = 1;

bool g_isLightTheme = false;
COLORREF g_themeAccent = RGB(96, 205, 255);
int g_tintR = 18, g_tintG = 20, g_tintB = 32;

bool g_editHiddenMode = false;
std::unordered_set<std::wstring> g_hiddenIds;

bool g_createFolderMode = false;
std::vector<std::wstring> g_folderBuildIds;

ULONG g_shellNotifyId = 0;

int ClampInt(int v, int lo, int hi) { return std::min(std::max(v, lo), hi); }

std::wstring MakeCustomSortKey(const std::wstring& displayName) {
    if (displayName.empty()) return std::wstring();
    int len = FoldStringW(MAP_COMPOSITE, displayName.data(),
                          (int)displayName.size(), nullptr, 0);
    std::wstring folded;
    if (len > 0) {
        folded.resize(len);
        FoldStringW(MAP_COMPOSITE, displayName.data(), (int)displayName.size(),
                    &folded[0], len);
    } else {
        folded = displayName;
    }
    if (!folded.empty()) CharLowerBuffW(&folded[0], (DWORD)folded.size());

    std::wstring key;
    key.reserve(folded.size());
    for (wchar_t c : folded) {
        if ((c >= 0x0300 && c <= 0x036F) || (c >= 0x200B && c <= 0x200F) ||
            (c >= 0x202A && c <= 0x202E) || c == 0xFEFF) {
            continue;
        }
        if (IsCharAlphaNumericW(c)) {
            key.push_back(c);
        } else if (c == L' ' && !key.empty() && key.back() != L' ') {
            key.push_back(L' ');
        }
    }
    return key.empty() ? folded : key;
}

bool CustomAlphaLess(const std::wstring& keyA, const std::wstring& keyB,
                     const std::wstring& rawA, const std::wstring& rawB) {
    if (keyA != keyB) return keyA < keyB;
    return rawA < rawB;
}

void LoadSettings() {
    InterlockedExchange(&g_optWinKey, Wh_GetIntSetting(L"behavior.triggerWinKey") ? 1 : 0);
    InterlockedExchange(&g_optStartBtn, Wh_GetIntSetting(L"behavior.triggerStartButton") ? 1 : 0);
    InterlockedExchange(&g_optCols, ClampInt(Wh_GetIntSetting(L"layout.gridColumns"), 4, 12));
    InterlockedExchange(&g_optRows, ClampInt(Wh_GetIntSetting(L"layout.gridRows"), 3, 8));
    InterlockedExchange(&g_optIconPct, ClampInt(Wh_GetIntSetting(L"visuals.iconSize"), 50, 150));
    InterlockedExchange(&g_optTextPct, ClampInt(Wh_GetIntSetting(L"visuals.textSize"), 60, 200));
    InterlockedExchange(&g_optSearchTextPct, ClampInt(Wh_GetIntSetting(L"visuals.searchTextSize"), 50, 150));
    InterlockedExchange(&g_optDark, ClampInt(Wh_GetIntSetting(L"visuals.backgroundDarkness"), 0, 100));
    InterlockedExchange(&g_optAnim, Wh_GetIntSetting(L"visuals.animations") ? 1 : 0);
    InterlockedExchange(&g_optHideUninstallers, Wh_GetIntSetting(L"features.hideUninstallers") ? 1 : 0);
    InterlockedExchange(&g_optPower, Wh_GetIntSetting(L"features.showPowerButton") ? 1 : 0);

    auto st = WindhawkUtils::StringSetting::make(L"visuals.visualStyle");
    InterlockedExchange(&g_optStyle, wcscmp(st, L"windows") == 0 ? 1 : 0);

    int blur = 2;
    auto s = WindhawkUtils::StringSetting::make(L"visuals.blur");
    if (wcscmp(s, L"off") == 0) blur = 0;
    else if (wcscmp(s, L"light") == 0) blur = 1;
    else if (wcscmp(s, L"strong") == 0) blur = 3;
    InterlockedExchange(&g_optBlur, blur);

    int savedSearchHidden = Wh_GetIntValue(L"searchHiddenApps", 0);
    InterlockedExchange(&g_searchHiddenApps, (savedSearchHidden != 0) ? 1 : 0);
}

struct App {
    std::wstring name, sortKey, lower, id, idLower;
    HBITMAP icon = nullptr, mini = nullptr;
    int iconPx = 0, miniPx = 0, titleWidth = -1, titleFontPx = 0;
    bool iconFailed = false;
};

struct LItem {
    bool folder = false;
    std::wstring id, name;
    std::vector<std::wstring> ids;
    int titleWidth = -1, titleFontPx = 0;
};

struct Item {
    int app = -1, folder = -1;
};

HANDLE g_thread = nullptr, g_ready = nullptr;
DWORD g_threadId = 0;
HWND g_wnd = nullptr;

IShellItem* g_appsFolder = nullptr;
std::vector<App> g_apps;
std::unordered_map<std::wstring, int> g_appIndex;
std::vector<LItem> g_layout;
std::vector<Item> g_view;
std::vector<double> g_viewHover, g_folderHover;
double g_hoverLastMs = 0.0;
size_t g_prewarmIdx = 0;

std::wstring g_query;
int g_page = 0, g_wheelAccum = 0, g_hwheelAccum = 0;
double g_pagePos = 0.0, g_pageLastMs = 0.0, g_pageVel = 0.0;
bool g_pageAnimating = false;
bool g_dragging = false, g_dragMoved = false, g_pressMoved = false;
int g_dragX0 = 0, g_dragY0 = 0, g_dragLastX = 0;
double g_dragLastMs = 0.0, g_dragSwipeVel = 0.0, g_dragPos0 = 0.0;
int g_pressItem = -1, g_pressFolderIdx = -1;

bool g_picked = false;
int g_pickKind = 0, g_pickIdx = -1, g_dragCx = 0, g_dragCy = 0;
int g_dropMode = 0, g_dropPos = -1;
DWORD g_edgeSince = 0;

int g_openFolder = -1, g_folderTarget = 0, g_folderSel = -1;
double g_folderVis = 0.0, g_folderAnimLastMs = 0.0;
bool g_renaming = false;
std::wstring g_renameBackup;

int g_sel = -1;
bool g_keyNav = false;
bool g_trackingMouse = false;
int g_lastMouseX = -1, g_lastMouseY = -1;
bool g_powerHover = false, g_menuOpen = false, g_caretVisible = true;
DWORD g_menuDismissTick = 0;
double g_lastClosed = 0.0;

int g_dpi = 96, g_loadPx = 0, g_fontPx = 0, g_fontBasePx = 0, g_fontSearchPx = 0, g_w = 0, g_h = 0;
HFONT g_font = nullptr, g_fontSearch = nullptr, g_fontFolderTitle = nullptr;
HFONT g_fontHover[kHoverFontSteps] = {};

// App-name titles are rendered once at several times their size (grayscale,
// effectively unhinted) and area-averaged down to the exact, fractional size
// needed at each animation frame. GDI fonts only exist in whole pixel sizes,
// so this is what makes the hover zoom smooth, and the same renderer is used
// at rest so the text never changes look when hovering starts or ends.
struct TitleMask {
    int w = 0, h = 0;              // full hover-size box (final pixels)
    int ss = 1;                    // supersampling factor of `a`
    std::vector<uint8_t> a;        // (w*ss) x (h*ss) coverage
    int restW = 0, restH = 0;      // cached resting (non-hover) size
    std::vector<uint8_t> rest;
};
std::unordered_map<std::wstring, TitleMask> g_titleMasks;

// Search-bar text is drawn like the app titles: rendered large in grayscale, area-averaged
// down and blended with real translucency (no ClearType colour fringes).
struct SearchTextMask {
    int w = 0, h = 0;      // final pixel size
    int baseRow = 0;       // row of the baseline inside the mask
    int originX = 0;       // x of the pen origin inside the mask
    double adv = 0.0;      // advance width in final pixels
    std::vector<uint8_t> a;
};
std::unordered_map<std::wstring, SearchTextMask> g_searchMasks;
HFONT g_fontSearchMask = nullptr;
int g_searchSS = 4;
double g_searchCapPx = 10.0;  // cap height of the search font in final pixels
size_t g_titleMaskBytes = 0;
HFONT g_fontMask = nullptr;
int g_maskSS = 4;
HDC g_scratchDC = nullptr, g_frameDC = nullptr;
HBITMAP g_dimBmp = nullptr, g_backdrop = nullptr, g_frame = nullptr;
HGDIOBJ g_frameOld = nullptr;

HDC g_gridCacheDC = nullptr;
HBITMAP g_gridCacheBmp = nullptr;
HGDIOBJ g_gridCacheOld = nullptr;
bool g_gridCacheValid = false;
int g_gridCachePage = -1;

std::vector<uint32_t> g_blurTmp;
std::vector<int> g_blurXi0, g_blurXi1, g_blurXf;

HBITMAP g_bmPill = nullptr, g_bmSel = nullptr, g_bmSelRect = nullptr, g_bmMag = nullptr, g_bmClear = nullptr;
HBITMAP g_bmMore = nullptr, g_bmDoneGlyph = nullptr, g_bmFolder = nullptr;
HBITMAP g_bmVisBadge = nullptr, g_bmHidBadge = nullptr, g_bmUncheckedBadge = nullptr, g_bmPanel = nullptr;
int g_bmKey[11] = {}, g_panelKey[5] = {};

// Anti-aliased page dots and power symbol (rebuilt when size/theme changes).
HBITMAP g_bmDotOn = nullptr, g_bmDotOff = nullptr, g_bmPowerGlyph = nullptr;
int g_dotKey[3] = {}, g_powerKey[3] = {}, g_powerGlyphSz = 0;

// Scratch surfaces for the smooth icon hover zoom.
HDC g_icoDC = nullptr;
HBITMAP g_icoSrc = nullptr, g_icoOut = nullptr;
uint32_t *g_icoSrcBits = nullptr, *g_icoOutBits = nullptr;
int g_icoSrcSz = 0, g_icoOutSz = 0;

// UAC shield shown next to "Run as administrator" in context menus.
HBITMAP g_bmAdminShield = nullptr;

bool g_dirty = true, g_animEnabled = true;
double g_vis = 0.0, g_animLastMs = 0.0;
int g_target = 0;

int S(int v) { return MulDiv(v, g_dpi, 96); }

double NowMs() {
    static LARGE_INTEGER freq = {};
    if (!freq.QuadPart) QueryPerformanceFrequency(&freq);
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)freq.QuadPart;
}

UINT QueryDpi(HWND hwnd) {
    using Fn = UINT(WINAPI*)(HWND);
    static Fn fn = reinterpret_cast<Fn>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
    if (fn && hwnd) {
        UINT d = fn(hwnd);
        if (d) return d;
    }
    HDC dc = GetDC(nullptr);
    int d = GetDeviceCaps(dc, LOGPIXELSX);
    ReleaseDC(nullptr, dc);
    return d > 0 ? d : 96;
}

void UpdateThemeColors() {
    DWORD light = 0, sz = sizeof(light);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &light,
                     &sz) == ERROR_SUCCESS) {
        g_isLightTheme = (light != 0);
    } else if (RegGetValueW(HKEY_CURRENT_USER,
                            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                            L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light,
                            &sz) == ERROR_SUCCESS) {
        g_isLightTheme = (light != 0);
    } else {
        g_isLightTheme = false;
    }

    DWORD argb = 0;
    BOOL opaque = FALSE;
    BYTE ar = 0, ag = 120, ab = 212;
    if (SUCCEEDED(DwmGetColorizationColor(&argb, &opaque))) {
        ar = (argb >> 16) & 0xFF;
        ag = (argb >> 8) & 0xFF;
        ab = argb & 0xFF;
    }

    if (g_isLightTheme) {
        g_themeAccent = RGB((BYTE)(ar * 80 / 100), (BYTE)(ag * 80 / 100), (BYTE)(ab * 80 / 100));
        g_tintR = (242 * 88 + ar * 12) / 100;
        g_tintG = (244 * 88 + ag * 12) / 100;
        g_tintB = (250 * 88 + ab * 12) / 100;
    } else {
        g_themeAccent = RGB((BYTE)std::min(255, (int)ar + (255 - ar) * 30 / 100),
                            (BYTE)std::min(255, (int)ag + (255 - ag) * 30 / 100),
                            (BYTE)std::min(255, (int)ab + (255 - ab) * 30 / 100));
        g_tintR = (18 * 85 + ar * 15) / 100;
        g_tintG = (20 * 85 + ag * 15) / 100;
        g_tintB = (30 * 85 + ab * 15) / 100;
    }
}

void FreeFonts() {
    g_titleMasks.clear();
    g_titleMaskBytes = 0;
    g_searchMasks.clear();
    if (g_fontSearchMask) { DeleteObject(g_fontSearchMask); g_fontSearchMask = nullptr; }
    if (g_fontMask) { DeleteObject(g_fontMask); g_fontMask = nullptr; }
    for (int i = 0; i < kHoverFontSteps; i++) {
        if (g_fontHover[i]) {
            DeleteObject(g_fontHover[i]);
            g_fontHover[i] = nullptr;
        }
    }
    g_font = nullptr;
    if (g_fontSearch) { DeleteObject(g_fontSearch); g_fontSearch = nullptr; }
    if (g_fontFolderTitle) { DeleteObject(g_fontFolderTitle); g_fontFolderTitle = nullptr; }
}

// Search bar height. Proportions come from measuring the real macOS Launchpad search
// field (60 px tall, 360 px wide, i.e. 6:1) and the Windows 11 sign-in PIN box (8:1).
int SearchBarHeight(int h) {
    bool winStyle = (g_optStyle == 1);
    return std::max(S(winStyle ? 32 : 36), (int)std::lround(h * (winStyle ? 0.034 : 0.0407)));
}

// Search text size, tied to the bar height (cap height = 25% of the bar on macOS,
// 31% on the Windows box) and the user's "Search bar text size" option.
int SearchFontPx(int h) {
    bool winStyle = (g_optStyle == 1);
    double px = SearchBarHeight(h) * (winStyle ? 0.43 : 0.357) * (double)g_optSearchTextPct / 100.0;
    return std::max(S(8), (int)std::lround(px));
}

void BuildSearchMaskFont(int searchPx) {
    if (g_fontSearchMask) { DeleteObject(g_fontSearchMask); g_fontSearchMask = nullptr; }
    g_searchSS = ClampInt((int)std::lround(64.0 / std::max(1, searchPx)), 3, 6);
    g_fontSearchMask = CreateFontW(-searchPx * g_searchSS, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                   ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    g_searchCapPx = searchPx * 0.70;
    HDC dc = CreateCompatibleDC(nullptr);
    if (dc && g_fontSearchMask) {
        HGDIOBJ old = SelectObject(dc, g_fontSearchMask);
        GLYPHMETRICS gm = {};
        MAT2 id = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
        if (GetGlyphOutlineW(dc, L'H', GGO_METRICS, &gm, 0, nullptr, &id) != GDI_ERROR &&
            gm.gmptGlyphOrigin.y > 0) {
            g_searchCapPx = (double)gm.gmptGlyphOrigin.y / g_searchSS;
        }
        SelectObject(dc, old);
    }
    if (dc) DeleteDC(dc);
}

void RebuildFonts(int titlePx, int basePx, int searchPx) {
    FreeFonts();
    for (int i = 0; i < kHoverFontSteps; i++) {
        double t = (double)i / (double)(kHoverFontSteps - 1);
        double scale = 1.0 + (kHoverScale - 1.0) * t;
        int px = std::max(1, (int)std::lround(titlePx * scale));
        g_fontHover[i] = CreateFontW(-px, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    }
    g_font = g_fontHover[0];
    g_fontSearch = CreateFontW(-std::max(S(10), searchPx),
                               0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    g_fontFolderTitle = CreateFontW(-(int)std::lround(basePx * 1.85), 0, 0, 0,
                                    FW_NORMAL, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    g_maskSS = ClampInt((int)std::lround(96.0 / std::max(1.0, titlePx * kHoverScale)), 2, 4);
    g_fontMask = CreateFontW(-(int)std::lround(titlePx * kHoverScale * g_maskSS), 0, 0, 0,
                             FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    g_fontPx = titlePx;
    g_fontBasePx = basePx;
    g_fontSearchPx = searchPx;
    BuildSearchMaskFont(searchPx);
}

void EnsureScratchDC() {
    if (!g_scratchDC) {
        HDC scr = GetDC(nullptr);
        g_scratchDC = CreateCompatibleDC(scr);
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = 1;
        bi.bmiHeader.biHeight = -1;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        g_dimBmp = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (g_dimBmp && bits) *static_cast<uint32_t*>(bits) = 0xFF000000u;
        ReleaseDC(nullptr, scr);
    }
}

void FreeScratchDC() {
    if (g_dimBmp) { DeleteObject(g_dimBmp); g_dimBmp = nullptr; }
    if (g_scratchDC) { DeleteDC(g_scratchDC); g_scratchDC = nullptr; }
}

int MeasureTitleWidth(const std::wstring& s, int* cachedWidth, int* cachedFontPx) {
    if (*cachedWidth >= 0 && *cachedFontPx == g_fontPx) return *cachedWidth;
    EnsureScratchDC();
    HGDIOBJ old = SelectObject(g_scratchDC, g_font);
    SIZE sz = {};
    GetTextExtentPoint32W(g_scratchDC, s.c_str(), (int)s.size(), &sz);
    SelectObject(g_scratchDC, old);
    *cachedWidth = (int)sz.cx;
    *cachedFontPx = g_fontPx;
    return (int)sz.cx;
}

struct Layout {
    int cols, left, top, cellW, cellH, rowsVisible, perPage, pages, icon;
};

int GridCols() { return (int)g_optCols; }
int GridRows() { return (int)g_optRows; }

void CellSize(int w, int h, int* cw, int* ch) {
    *ch = (int)(h * 0.74 / GridRows());
    *cw = std::min((int)(w * 0.76 / GridCols()), (int)(*ch * 1.25));
}

int IconPxFor(int w, int h) {
    int cw, ch;
    CellSize(w, h, &cw, &ch);
    int px = (int)(std::min(cw, ch) * 0.46 * (double)g_optIconPct / 100.0);
    px = (px + 4) / 8 * 8;
    return std::min(std::max(px, 32), 256);
}

int BaseMiniPxFor(int iconPx) {
    int basePad = (int)(iconPx * 0.13);
    int baseGap = std::max(1, (int)(iconPx * 0.035));
    return (iconPx - 2 * basePad - 2 * baseGap) / 3;
}

// Unscaled title size: also drives the search box and folder-title fonts.
int BaseTitleFontPx(int h) {
    int px = (int)std::lround(h * 0.010);
    return std::min(std::max(px, S(9)), S(16));
}

// App-name size with the user's text-size option applied.
int TitleFontPx(int h) {
    double px = BaseTitleFontPx(h) * (double)g_optTextPct / 100.0;
    return std::max(S(6), (int)std::lround(px));
}

// Max title width grows with the text size, but is capped so that the title
// (and the selection rectangle around it) at full hover zoom always fits
// inside its own cell.
int MaxTitleWidth(int cellW) {
    int base = std::max(S(44), std::min(cellW - S(40), (int)(cellW * 0.60)));
    int scaled = (int)std::lround(base * (double)g_optTextPct / 100.0);
    int cap = (int)((cellW - S(24)) / kHoverScale);
    return std::max(S(32), std::min(scaled, cap));
}

void SelRectSize(int cellW, int icon, int* rw, int* rh) {
    int titleH = (int)std::lround(g_fontPx * 1.55);
    int gap = (int)(icon * kIconTitleGapRatio);
    int blockH = icon + gap + titleH;
    int maxTw = (int)std::lround(MaxTitleWidth(cellW) * kHoverScale);
    *rw = std::max(icon + 2 * S(14), maxTw + 2 * S(10));
    *rh = blockH + S(12) + S(10);
}

Layout GetLayout(int w, int h, size_t n) {
    Layout l;
    CellSize(w, h, &l.cellW, &l.cellH);
    l.cols = GridCols();
    l.rowsVisible = GridRows();
    l.left = (w - l.cols * l.cellW) / 2;
    l.top = (int)(h * 0.135);
    l.perPage = l.cols * l.rowsVisible;
    l.pages = std::max(1, (int)((n + l.perPage - 1) / l.perPage));
    l.icon = IconPxFor(w, h);
    return l;
}

struct SearchGeo {
    RECT bar, mag, rightBtn, text;
};

SearchGeo GetSearchGeo(int w, int h) {
    SearchGeo g;
    bool winStyle = (g_optStyle == 1);
    int bh = SearchBarHeight(h);
    int bw = std::min((int)std::lround(bh * (winStyle ? 8.0 : 6.0)), (int)(w * 0.6));
    int cy = (int)std::lround(h * (winStyle ? 0.068 : 0.0713));
    int top = cy - bh / 2;
    g.bar = RECT{w / 2 - bw / 2, top, w / 2 - bw / 2 + bw, top + bh};
    double cyF = top + bh / 2.0;

    // Positions are fractions of the bar height, taken from the reference screenshots.
    int mg = (int)std::lround(bh * 0.43);
    double mcx = g.bar.left + bh * (winStyle ? 0.52 : 0.608);
    int mx = (int)std::lround(mcx - mg / 2.0), my = (int)std::lround(cyF - mg / 2.0);
    g.mag = RECT{mx, my, mx + mg, my + mg};

    int cd = (int)std::lround(bh * 0.42);
    double ccx = g.bar.right - bh * (winStyle ? 0.46 : 0.525);
    int rx = (int)std::lround(ccx - cd / 2.0), ry = (int)std::lround(cyF - cd / 2.0);
    g.rightBtn = RECT{rx, ry, rx + cd, ry + cd};

    g.text = RECT{g.bar.left + (int)std::lround(bh * (winStyle ? 0.95 : 1.017)), g.bar.top,
                  g.bar.right - (int)std::lround(bh * (winStyle ? 0.88 : 0.95)), g.bar.bottom};
    return g;
}

void ClampPage(const Layout& l) {
    g_page = std::min(std::max(g_page, 0), l.pages - 1);
    g_pagePos = std::min(std::max(g_pagePos, 0.0), (double)(l.pages - 1));
}

constexpr int kDotSpacing = 22;
int DotCenterX(int i, int pages, int w) {
    return w / 2 - S(kDotSpacing) * (pages - 1) / 2 + i * S(kDotSpacing);
}
int DotCenterY(int h) { return (int)(h * 0.93); }

int DotHitTest(int x, int y, const Layout& l, int w, int h) {
    if (l.pages < 2 || std::abs(y - DotCenterY(h)) > S(16)) return -1;
    for (int i = 0; i < l.pages; i++) {
        if (std::abs(x - DotCenterX(i, l.pages, w)) <= S(kDotSpacing) / 2) return i;
    }
    return -1;
}

RECT PowerRect(int w, int h) {
    if (!g_optPower) return RECT{0, 0, 0, 0};
    Layout l = GetLayout(w, h, 0);
    int cx = l.left + (l.cols - 1) * l.cellW + l.cellW / 2;
    int cy = DotCenterY(h);
    int half = S(20);
    return RECT{cx - half, cy - half, cx + half, cy + half};
}

RECT GetPageSwipeRect(const Layout& l) {
    int top = std::max(0, l.top - S(16));
    int bottom = std::min(g_h, DotCenterY(g_h) + S(24));
    return RECT{0, top, g_w, bottom};
}

bool InRect(const RECT& r, int x, int y) {
    return x >= r.left && x < r.right && y >= r.top && y < r.bottom;
}

bool PageSettled() {
    return !g_pageAnimating && !g_dragMoved && std::fabs(g_pagePos - g_page) < 0.0002;
}
bool FolderActive() { return g_openFolder >= 0 && g_folderTarget == 1; }

int CellHitTest(int x, int y, const Layout& l) {
    if (!PageSettled() || x < l.left || y < l.top) return -1;
    int col = (x - l.left) / l.cellW;
    int row = (y - l.top) / l.cellH;
    if (col >= l.cols || row >= l.rowsVisible) return -1;
    int idx = g_page * l.perPage + row * l.cols + col;
    return idx < (int)g_view.size() ? idx : -1;
}

void CellIconPos(const Layout& l, int x, int y, int* ix, int* iy);

// Everything a hovered cell can paint: the cell itself united with the
// selection rectangle at full size (which may stick out of the cell when the
// text is large), plus a safety margin. Redraw/dirty rects must use this so the
// selection rectangle is never cut off.
RECT GetCellHoverBounds(const Layout& l, int x, int y) {
    int ix, iy;
    CellIconPos(l, x, y, &ix, &iy);
    int srW = 0, srH = 0;
    SelRectSize(l.cellW, l.icon, &srW, &srH);
    int sx = x + (l.cellW - srW) / 2;
    RECT cell = {x, y, x + l.cellW, y + l.cellH};
    RECT sel = {sx, iy - S(12), sx + srW, iy - S(12) + srH};
    RECT r = {};
    UnionRect(&r, &cell, &sel);
    InflateRect(&r, S(6), S(6));
    r.left = std::max(0L, r.left);
    r.top = std::max(0L, r.top);
    r.right = std::min((LONG)g_w, r.right);
    r.bottom = std::min((LONG)g_h, r.bottom);
    return r;
}

RECT GetViewCellBounds(const Layout& l, int viewIdx) {
    if (viewIdx < 0 || viewIdx >= (int)g_view.size()) return RECT{0, 0, 0, 0};
    int pg = viewIdx / l.perPage;
    int local = viewIdx % l.perPage;
    int xoff = (int)std::lround((pg - g_pagePos) * g_w);
    int x = l.left + (local % l.cols) * l.cellW + xoff;
    int y = l.top + (local / l.cols) * l.cellH;
    return GetCellHoverBounds(l, x, y);
}

// Identifies the current page-animation frame chain (stale step messages are ignored).
LPARAM g_pageChain = 0;

void WaitVBlank() {
    DwmFlush();
}

// Per-page alpha snapshots (cells only, no backdrop) used while pages slide.
std::vector<HBITMAP> g_snap;
void InvalidateSnaps() {
    for (HBITMAP& b : g_snap) if (b) DeleteObject(b);
    g_snap.clear();
}
void ScheduleSnapBuild() {
    if (g_wnd && g_animEnabled) SetTimer(g_wnd, kSnapTimer, 25, nullptr);
}

void Redraw() {
    g_dirty = true;
    g_gridCacheValid = false;
    InvalidateSnaps();
    ScheduleSnapBuild();
    if (g_wnd) InvalidateRect(g_wnd, nullptr, FALSE);
}

void RedrawRect(const RECT& r, bool invalidateStaticCache = false) {
    if (r.right <= r.left || r.bottom <= r.top) return;
    g_dirty = true;
    if (invalidateStaticCache) g_gridCacheValid = false;
    if (g_wnd) InvalidateRect(g_wnd, &r, FALSE);
}

double Eased(double t) {
    double u = 1.0 - t;
    return 1.0 - u * u * u;
}

void ForceForeground(HWND hwnd) {
    INPUT input = {};
    SendInput(1, &input, sizeof(INPUT));
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
}

void SwallowMenuDismissClicks(HWND hwnd) {
    g_menuDismissTick = GetTickCount();
    MSG m;
    while (PeekMessageW(&m, hwnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE)) {
    }
}

bool IsMenuDismissClick() {
    return (GetTickCount() - g_menuDismissTick) < 220;
}

void PrewarmPage(const Layout& l, int pageIdx);
int HitTest(int x, int y, const Layout& l);
struct FolderGeo {
    RECT panel, title;
    int cols, rows, count, left, top, cellW, cellH;
};
FolderGeo GetFolderGeo(int w, int h);

void SnapHoverStates() {
    if (g_viewHover.size() != g_view.size()) g_viewHover.resize(g_view.size(), 0.0);
    for (size_t i = 0; i < g_viewHover.size(); i++) {
        g_viewHover[i] = ((int)i == g_sel && !FolderActive() && !g_picked) ? 1.0 : 0.0;
    }
    if (g_openFolder >= 0 && g_openFolder < (int)g_layout.size()) {
        size_t fsz = g_layout[g_openFolder].ids.size();
        if (g_folderHover.size() != fsz) g_folderHover.resize(fsz, 0.0);
        for (size_t i = 0; i < fsz; i++) {
            g_folderHover[i] = ((int)i == g_folderSel && FolderActive() && !g_picked) ? 1.0 : 0.0;
        }
    }
}

void StartHoverAnim() {
    if (!g_wnd) return;
    if (!g_animEnabled || !PageSettled()) {
        KillTimer(g_wnd, kHoverTimer);
        SnapHoverStates();
        if (!g_animEnabled) Redraw();
        return;
    }
    g_hoverLastMs = NowMs();
    SetTimer(g_wnd, kHoverTimer, 10, nullptr);
}

void OnHoverTimer() {
    if (!PageSettled()) {
        KillTimer(g_wnd, kHoverTimer);
        SnapHoverStates();
        return;
    }
    double now = NowMs();
    double dt = std::min((now - g_hoverLastMs) / 1000.0, 0.05);
    g_hoverLastMs = now;
    double alpha = 1.0 - std::exp(-22.0 * dt);
    bool anyMoving = false;
    Layout l = GetLayout(g_w, g_h, g_view.size());
    RECT dirty = {0, 0, 0, 0};
    auto unionDirty = [&](const RECT& r) {
        if (r.right <= r.left || r.bottom <= r.top) return;
        if (dirty.right <= dirty.left) dirty = r;
        else UnionRect(&dirty, &dirty, &r);
    };

    if (g_viewHover.size() != g_view.size()) g_viewHover.resize(g_view.size(), 0.0);
    for (size_t i = 0; i < g_viewHover.size(); i++) {
        double target = (!FolderActive() && !g_picked && (int)i == g_sel) ? 1.0 : 0.0;
        double diff = target - g_viewHover[i];
        if (std::fabs(diff) > 0.006) {
            g_viewHover[i] += diff * alpha;
            anyMoving = true;
            unionDirty(GetViewCellBounds(l, (int)i));
        } else if (g_viewHover[i] != target) {
            g_viewHover[i] = target;
            unionDirty(GetViewCellBounds(l, (int)i));
        }
    }

    if (g_openFolder >= 0 && g_openFolder < (int)g_layout.size()) {
        size_t fsz = g_layout[g_openFolder].ids.size();
        if (g_folderHover.size() != fsz) g_folderHover.resize(fsz, 0.0);
        FolderGeo fg = GetFolderGeo(g_w, g_h);
        for (size_t i = 0; i < fsz; i++) {
            double target = (FolderActive() && !g_picked && (int)i == g_folderSel) ? 1.0 : 0.0;
            double diff = target - g_folderHover[i];
            if (std::fabs(diff) > 0.006) {
                g_folderHover[i] += diff * alpha;
                anyMoving = true;
                { RECT pd = fg.panel; InflateRect(&pd, S(24), S(24)); unionDirty(pd); }
            } else if (g_folderHover[i] != target) {
                g_folderHover[i] = target;
                { RECT pd = fg.panel; InflateRect(&pd, S(24), S(24)); unionDirty(pd); }
            }
        }
    }

    if (!anyMoving) KillTimer(g_wnd, kHoverTimer);
    if (dirty.right > dirty.left) {
        RedrawRect(dirty, false);
        UpdateWindow(g_wnd);
    }
}

void StartFolderAnim(int target) {
    g_folderTarget = target;
    if (!g_wnd || !g_animEnabled) {
        g_folderVis = (double)target;
        if (target == 0) { g_openFolder = -1; g_folderSel = -1; }
        Redraw();
        return;
    }
    g_folderAnimLastMs = NowMs();
    SetTimer(g_wnd, kFolderAnimTimer, 7, nullptr);
    Redraw();
}

void OnFolderAnimTimer() {
    double now = NowMs();
    double dt = std::min(now - g_folderAnimLastMs, 50.0);
    g_folderAnimLastMs = now;
    double step = dt / (g_folderTarget ? kFolderOpenMs : kFolderCloseMs);
    if (g_folderTarget) g_folderVis = std::min(1.0, g_folderVis + step);
    else g_folderVis = std::max(0.0, g_folderVis - step);

    if (g_folderVis == (double)g_folderTarget) {
        KillTimer(g_wnd, kFolderAnimTimer);
        if (g_folderTarget == 0) { g_openFolder = -1; g_folderSel = -1; }
    }
    Redraw();
    UpdateWindow(g_wnd);
}

void CloseFolder(bool immediate = false) {
    if (g_openFolder < 0) return;
    if (immediate || !g_animEnabled) {
        KillTimer(g_wnd, kFolderAnimTimer);
        g_openFolder = -1;
        g_folderVis = 0.0;
        g_folderTarget = 0;
        g_folderSel = -1;
        Redraw();
        return;
    }
    StartFolderAnim(0);
}

void StartPageAnim() {
    if (!g_wnd) return;
    KillTimer(g_wnd, kHoverTimer);
    SnapHoverStates();
    if (!g_animEnabled) {
        g_pageAnimating = false;
        g_pagePos = g_page;
        g_pageVel = 0.0;
        Layout l = GetLayout(g_w, g_h, g_view.size());
        PrewarmPage(l, g_page);
        PrewarmPage(l, g_page - 1);
        PrewarmPage(l, g_page + 1);
        Redraw();
        return;
    }
    std::fill(g_viewHover.begin(), g_viewHover.end(), 0.0);
    bool wasAnimating = g_pageAnimating;
    g_pageAnimating = true;
    g_pageLastMs = NowMs();
    // Frames are driven by a posted-message chain paced to the display's
    // vblank (see OnPageTimer), not by SetTimer, whose ~15.6 ms granularity
    // capped the animation near 60 fps.
    if (!wasAnimating) {
        ++g_pageChain;
        PostMessageW(g_wnd, WM_LP_PAGESTEP, 0, g_pageChain);
    }
    Layout l = GetLayout(g_w, g_h, g_view.size());
    RedrawRect(GetPageSwipeRect(l), false);
}

void OnPageTimer() {
    const double kOmega = 21.0;
    double now = NowMs();
    double dt = std::min((now - g_pageLastMs) / 1000.0, 0.05);
    g_pageLastMs = now;

    double a = g_pagePos - (double)g_page;
    double v = g_pageVel;
    double b = v + kOmega * a;
    double e = std::exp(-kOmega * dt);
    a = (a + b * dt) * e;
    v = (v - kOmega * b * dt) * e;
    g_pagePos = (double)g_page + a;
    g_pageVel = v;

    Layout l = GetLayout(g_w, g_h, g_view.size());
    if (std::fabs(a) * std::max(1, g_w) < 1.0 && std::fabs(v) * std::max(1, g_w) < 25.0) {
        g_pagePos = (double)g_page;
        g_pageVel = 0.0;
        g_pageAnimating = false;
        g_gridCacheValid = false;
        RedrawRect(GetPageSwipeRect(l), true);
        UpdateWindow(g_wnd);
        PrewarmPage(l, g_page - 1);
        PrewarmPage(l, g_page + 1);
        if (g_prewarmIdx < g_apps.size()) {
            PostMessageW(g_wnd, WM_LP_PREWARM, 0, 0);
        }
        ScheduleSnapBuild();
        if (!g_keyNav && !g_dragging && !g_picked && !FolderActive() && g_lastMouseX >= 0) {
            g_sel = HitTest(g_lastMouseX, g_lastMouseY, l);
        }
        StartHoverAnim();
    } else {
        double t0 = NowMs();
        RedrawRect(GetPageSwipeRect(l), false);
        UpdateWindow(g_wnd);
        WaitVBlank();
        if (NowMs() - t0 < 5.0) Sleep(2);  // safety if the vblank wait returned instantly

        // Our own step messages outrank queued input, so hand-deliver input
        // and timers here to keep the UI responsive during the flip.
        MSG pm;
        while (PeekMessageW(&pm, g_wnd, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE) ||
               PeekMessageW(&pm, g_wnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE) ||
               PeekMessageW(&pm, g_wnd, WM_TIMER, WM_TIMER, PM_REMOVE)) {
            TranslateMessage(&pm);
            DispatchMessageW(&pm);
            if (!g_pageAnimating) return;
        }
        PostMessageW(g_wnd, WM_LP_PAGESTEP, 0, g_pageChain);
    }
}

void GoToPage(int p) {
    Layout l = GetLayout(g_w, g_h, g_view.size());
    p = std::min(std::max(p, 0), l.pages - 1);
    if (p == g_page && PageSettled()) return;
    PrewarmPage(l, p);
    if (g_sel >= 0 && g_keyNav) {
        int local = g_sel % l.perPage;
        g_sel = std::min((int)g_view.size() - 1, p * l.perPage + local);
    } else if (!g_keyNav) {
        g_sel = -1;
    }
    g_page = p;
    StartPageAnim();
}

std::wstring GetCurrentUserSidString() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return std::wstring();
    }
    DWORD len = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &len);
    std::wstring result;
    if (len > 0) {
        std::vector<BYTE> buf(len);
        if (GetTokenInformation(token, TokenUser, buf.data(), len, &len)) {
            auto* tu = reinterpret_cast<TOKEN_USER*>(buf.data());
            LPWSTR sidStr = nullptr;
            if (ConvertSidToStringSidW(tu->User.Sid, &sidStr) && sidStr) {
                result = sidStr;
                LocalFree(sidStr);
            }
        }
    }
    CloseHandle(token);
    return result;
}

std::wstring LayoutPath(bool makeDir) {
    WCHAR dir[MAX_PATH];
    if (!Wh_GetModStoragePath(dir, ARRAYSIZE(dir))) return std::wstring();
    if (makeDir) CreateDirectoryW(dir, nullptr);
    std::wstring sid = GetCurrentUserSidString();
    if (!sid.empty()) {
        return std::wstring(dir) + L"\\layout_" + sid + L".txt";
    }
    return std::wstring(dir) + L"\\layout.txt";
}

std::string ToUtf8(const std::wstring& s) {
    if (s.empty()) return std::string();
    int n = WideCharToMultiByte(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0, nullptr, nullptr);
    std::string out((size_t)std::max(n, 0), '\0');
    if (n > 0) WideCharToMultiByte(CP_UTF8, 0, s.data(), (int)s.size(), &out[0], n, nullptr, nullptr);
    return out;
}

std::wstring FromUtf8(const char* p, int len) {
    if (len <= 0) return std::wstring();
    int n = MultiByteToWideChar(CP_UTF8, 0, p, len, nullptr, 0);
    std::wstring out((size_t)std::max(n, 0), L'\0');
    if (n > 0) MultiByteToWideChar(CP_UTF8, 0, p, len, &out[0], n);
    return out;
}

std::string Field(std::wstring s) {
    for (auto& c : s) if (c == L'\t' || c == L'\r' || c == L'\n') c = L' ';
    return ToUtf8(s);
}

bool IsUninstallerApp(const std::wstring& lowerName, const std::wstring& lowerId) {
    static const wchar_t* const kPrefixes[] = {
        L"uninstall ", L"uninstaller", L"desinstalar ", L"d\u00E9sinstaller ",
        L"deinstallieren ", L"disinstalla ", L"\u0443\u0434\u0430\u043B\u0438\u0442\u044C ", L"remove ",
    };
    for (const wchar_t* p : kPrefixes) if (lowerName.rfind(p, 0) == 0) return true;
    static const wchar_t* const kSubstrings[] = {
        L" uninstall", L" uninstaller", L" desinstalar", L" d\u00E9sinstaller",
        L" deinstallieren", L"(uninstall)", L"- uninstall",
    };
    for (const wchar_t* sub : kSubstrings) if (lowerName.find(sub) != std::wstring::npos) return true;
    static const wchar_t* const kIdSubstrings[] = {
        L"unins000", L"uninst.exe", L"uninstall.exe", L"_uninstall", L"\\uninstall",
    };
    for (const wchar_t* sub : kIdSubstrings) if (lowerId.find(sub) != std::wstring::npos) return true;
    return false;
}

bool IsUserHidden(const App& a) { return g_hiddenIds.count(a.id) > 0; }

bool IsFilteredOut(const App& a, bool isSearching = false) {
    if (g_optHideUninstallers && IsUninstallerApp(a.lower, a.idLower)) return true;
    if (isSearching && g_searchHiddenApps) return false;
    if (!g_editHiddenMode && IsUserHidden(a)) return true;
    return false;
}

bool IsInFolderBuild(const std::wstring& id) {
    return std::find(g_folderBuildIds.begin(), g_folderBuildIds.end(), id) !=
           g_folderBuildIds.end();
}

void SaveLayout() {
    std::wstring path = LayoutPath(true);
    if (path.empty()) return;

    std::string out = "LPv1\n";
    for (const LItem& it : g_layout) {
        if (!it.folder) {
            out += "A\t" + Field(it.id) + "\n";
        } else {
            out += "F\t" + Field(it.name) + "\n";
            for (const auto& id : it.ids) out += "f\t" + Field(id) + "\n";
        }
    }
    for (const auto& hid : g_hiddenIds) out += "H\t" + Field(hid) + "\n";

    std::wstring tmp = path + L".tmp";
    HANDLE f = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD written = 0;
    BOOL ok = WriteFile(f, out.data(), (DWORD)out.size(), &written, nullptr);
    CloseHandle(f);
    if (ok && written == (DWORD)out.size()) {
        MoveFileExW(tmp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING);
    } else {
        DeleteFileW(tmp.c_str());
    }
}

void LoadLayoutFile() {
    g_layout.clear();
    g_hiddenIds.clear();
    std::wstring path = LayoutPath(false);
    if (path.empty()) return;
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;

    std::string data;
    LARGE_INTEGER sz = {};
    if (GetFileSizeEx(f, &sz) && sz.QuadPart > 0 && sz.QuadPart < (8 << 20)) {
        data.resize((size_t)sz.QuadPart);
        DWORD got = 0;
        if (!ReadFile(f, &data[0], (DWORD)data.size(), &got, nullptr)) got = 0;
        data.resize(got);
    }
    CloseHandle(f);

    size_t pos = 0;
    bool first = true;
    while (pos < data.size()) {
        size_t end = data.find('\n', pos);
        if (end == std::string::npos) end = data.size();
        size_t lineEnd = end;
        if (lineEnd > pos && data[lineEnd - 1] == '\r') lineEnd--;
        std::string line = data.substr(pos, lineEnd - pos);
        pos = end + 1;

        if (first) {
            first = false;
            if (line != "LPv1") return;
            continue;
        }
        size_t tab = line.find('\t');
        if (tab == std::string::npos) continue;
        std::string kind = line.substr(0, tab);
        std::wstring val = FromUtf8(line.data() + tab + 1, (int)(line.size() - tab - 1));
        if (kind == "A") {
            LItem it; it.id = val; g_layout.push_back(it);
        } else if (kind == "F") {
            LItem it; it.folder = true; it.name = val; g_layout.push_back(it);
        } else if (kind == "f") {
            if (!g_layout.empty() && g_layout.back().folder) g_layout.back().ids.push_back(val);
        } else if (kind == "H" && !val.empty()) {
            g_hiddenIds.insert(val);
        }
    }
}

// Reconciles `g_layout` with `g_apps`:
// - Preserves existing user folders and custom positions (including temporarily
//   missing IDs so Store updates don't lose their spot).
// - Inserts any NEWLY installed app into its proper alphabetical position
//   among top-level apps (so installing "Mipony" places it right among the "M"
//   apps instead of dumping it at the very end of the last page!).
bool ReconcileLayout() {
    bool hadExistingLayout = !g_layout.empty();
    std::unordered_set<std::wstring> seen;
    std::vector<LItem> out;
    for (const LItem& it : g_layout) {
        if (!it.folder) {
            if (seen.insert(it.id).second) {
                out.push_back(it);
            }
        } else {
            LItem f = it;
            f.ids.clear();
            for (const auto& id : it.ids) {
                if (seen.insert(id).second) {
                    f.ids.push_back(id);
                }
            }
            if (!f.ids.empty()) {
                out.push_back(std::move(f));
            }
        }
    }

    bool addedNewApp = false;
    for (size_t idx = 0; idx < g_apps.size(); idx++) {
        const App& a = g_apps[idx];
        if (!seen.count(a.id) &&
            !(g_optHideUninstallers && IsUninstallerApp(a.lower, a.idLower))) {
            seen.insert(a.id);
            LItem n;
            n.id = a.id;

            if (!hadExistingLayout) {
                out.push_back(std::move(n));
            } else {
                // Find the first top-level app in `out` whose name sorts
                // alphabetically after `a`, and insert right before it!
                auto insertIt = out.end();
                for (auto it = out.begin(); it != out.end(); ++it) {
                    if (it->folder) continue;
                    auto ai = g_appIndex.find(it->id);
                    if (ai == g_appIndex.end()) continue;
                    const App& existing = g_apps[ai->second];
                    if (CustomAlphaLess(a.sortKey, existing.sortKey,
                                        a.lower, existing.lower)) {
                        insertIt = it;
                        break;
                    }
                }
                out.insert(insertIt, std::move(n));
                addedNewApp = true;
            }
        }
    }
    g_layout.swap(out);
    return addedNewApp;
}

void DissolveFolderAt(int li) {
    if (li < 0 || li >= (int)g_layout.size() || !g_layout[li].folder) return;
    std::vector<LItem> apps;
    for (const auto& id : g_layout[li].ids) { LItem a; a.id = id; apps.push_back(a); }
    g_layout.erase(g_layout.begin() + li);
    g_layout.insert(g_layout.begin() + li, apps.begin(), apps.end());
}

std::wstring FolderTitle(const LItem& f) {
    if (f.name.empty() || f.name == L"Folder") return std::wstring(GetStrings().defaultFolder);
    return f.name;
}

// ---------------------------------------------------------------------------
// High-quality Bilinear + Box Icon Scaler & Legacy Frame/Border Trimmer
// ---------------------------------------------------------------------------
HBITMAP CreatePremultipliedDIB(int w, int h, const uint32_t* pixels) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    EnsureScratchDC();
    HBITMAP dib = CreateDIBSection(g_scratchDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (dib && bits && pixels) {
        std::memcpy(bits, pixels, (size_t)w * h * sizeof(uint32_t));
    }
    return dib;
}

HBITMAP ResampleSubRect(const uint32_t* in, int sw, int sh,
                        int sx0, int sy0, int sx1, int sy1, int dst) {
    int cw = std::max(1, sx1 - sx0);
    int ch = std::max(1, sy1 - sy0);
    int side = std::max(cw, ch);
    double cx = (sx0 + sx1) * 0.5;
    double cy = (sy0 + sy1) * 0.5;
    double xStart = cx - side * 0.5;
    double yStart = cy - side * 0.5;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = dst;
    bi.bmiHeader.biHeight = -dst;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP out = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!out || !bits) { if (out) DeleteObject(out); return nullptr; }
    auto* o = static_cast<uint32_t*>(bits);

    auto samplePixel = [&](int x, int y) -> uint32_t {
        if (x < sx0 || x >= sx1 || y < sy0 || y >= sy1) return 0;
        if (x < 0 || x >= sw || y < 0 || y >= sh) return 0;
        return in[(size_t)y * sw + x];
    };

    if (side >= dst) {
        for (int y = 0; y < dst; y++) {
            int y0 = (int)std::floor(yStart + (double)y * side / dst);
            int y1 = std::max(y0 + 1, (int)std::ceil(yStart + (double)(y + 1) * side / dst));
            for (int x = 0; x < dst; x++) {
                int x0 = (int)std::floor(xStart + (double)x * side / dst);
                int x1 = std::max(x0 + 1, (int)std::ceil(xStart + (double)(x + 1) * side / dst));
                uint64_t a = 0, r = 0, g = 0, b = 0, cnt = 0;
                for (int yy = y0; yy < y1; yy++) {
                    for (int xx = x0; xx < x1; xx++) {
                        uint32_t p = samplePixel(xx, yy);
                        a += p >> 24; r += (p >> 16) & 255; g += (p >> 8) & 255; b += p & 255;
                        cnt++;
                    }
                }
                if (!cnt) cnt = 1;
                o[(size_t)y * dst + x] = ((uint32_t)(a / cnt) << 24) | ((uint32_t)(r / cnt) << 16) |
                                         ((uint32_t)(g / cnt) << 8) | (uint32_t)(b / cnt);
            }
        }
    } else {
        for (int y = 0; y < dst; y++) {
            double fy = yStart + ((y + 0.5) * side / dst) - 0.5;
            int y0 = (int)std::floor(fy);
            double wy = fy - y0;
            for (int x = 0; x < dst; x++) {
                double fx = xStart + ((x + 0.5) * side / dst) - 0.5;
                int x0 = (int)std::floor(fx);
                double wx = fx - x0;

                uint32_t p00 = samplePixel(x0, y0);
                uint32_t p10 = samplePixel(x0 + 1, y0);
                uint32_t p01 = samplePixel(x0, y0 + 1);
                uint32_t p11 = samplePixel(x0 + 1, y0 + 1);

                double w00 = (1.0 - wx) * (1.0 - wy);
                double w10 = wx * (1.0 - wy);
                double w01 = (1.0 - wx) * wy;
                double w11 = wx * wy;

                uint32_t a = (uint32_t)std::lround((p00 >> 24) * w00 + (p10 >> 24) * w10 +
                                                   (p01 >> 24) * w01 + (p11 >> 24) * w11);
                uint32_t r = (uint32_t)std::lround(((p00 >> 16) & 255) * w00 + ((p10 >> 16) & 255) * w10 +
                                                   ((p01 >> 16) & 255) * w01 + ((p11 >> 16) & 255) * w11);
                uint32_t g = (uint32_t)std::lround(((p00 >> 8) & 255) * w00 + ((p10 >> 8) & 255) * w10 +
                                                   ((p01 >> 8) & 255) * w01 + ((p11 >> 8) & 255) * w11);
                uint32_t b = (uint32_t)std::lround((p00 & 255) * w00 + (p10 & 255) * w10 +
                                                   (p01 & 255) * w01 + (p11 & 255) * w11);
                o[(size_t)y * dst + x] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }
    return out;
}

HBITMAP NormalizeAndTrimIcon(HBITMAP hbm, int targetPx) {
    if (!hbm) return nullptr;
    BITMAP bm = {};
    if (!GetObject(hbm, sizeof(bm), &bm) || bm.bmWidth < 1 || bm.bmHeight < 1) {
        DeleteObject(hbm);
        return nullptr;
    }
    int w = bm.bmWidth, h = std::abs(bm.bmHeight);
    std::vector<uint32_t> px((size_t)w * h);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    EnsureScratchDC();
    int got = GetDIBits(g_scratchDC, hbm, 0, h, px.data(), &bi, DIB_RGB_COLORS);
    DeleteObject(hbm);
    if (got != h) return nullptr;

    size_t count = (size_t)w * h;
    bool hasAlpha = false;
    for (size_t i = 0; i < count; i++) {
        if ((px[i] >> 24) != 0) { hasAlpha = true; break; }
    }
    bool alreadyPremult = true;
    if (hasAlpha) {
        for (size_t i = 0; i < count; i++) {
            uint32_t p = px[i], a = p >> 24;
            if (((p >> 16) & 255) > a || ((p >> 8) & 255) > a || (p & 255) > a) {
                alreadyPremult = false;
                break;
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        uint32_t p = px[i];
        uint32_t a = hasAlpha ? (p >> 24) : 255;
        if (!hasAlpha || !alreadyPremult) {
            uint32_t r = (((p >> 16) & 255) * a + 127) / 255;
            uint32_t g = (((p >> 8) & 255) * a + 127) / 255;
            uint32_t b = ((p & 255) * a + 127) / 255;
            px[i] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }

    auto findBounds = [&](int xMin, int yMin, int xMax, int yMax,
                          int* outL, int* outT, int* outR, int* outB) -> bool {
        int l = xMax, t = yMax, r = xMin - 1, b = yMin - 1;
        for (int y = yMin; y < yMax; y++) {
            for (int x = xMin; x < xMax; x++) {
                if ((px[(size_t)y * w + x] >> 24) > 12) {
                    if (x < l) l = x;
                    if (x > r) r = x;
                    if (y < t) t = y;
                    if (y > b) b = y;
                }
            }
        }
        if (r < l || b < t) return false;
        *outL = l; *outT = t; *outR = r + 1; *outB = b + 1;
        return true;
    };

    int l = 0, t = 0, r = w, b = h;
    if (!findBounds(0, 0, w, h, &l, &t, &r, &b)) {
        return CreatePremultipliedDIB(w, h, px.data());
    }

    int bw = r - l, bh = b - t;
    if (bw >= 24 && bh >= 24) {
        int inset = std::max(3, std::min(bw, bh) / 14);
        int ringTotal = 0, ringTransparent = 0;
        for (int y = t + inset; y < b - inset; y++) {
            for (int x = l + inset; x < r - inset; x++) {
                if (x < l + inset * 2 || x >= r - inset * 2 ||
                    y < t + inset * 2 || y >= b - inset * 2) {
                    ringTotal++;
                    if ((px[(size_t)y * w + x] >> 24) <= 12) ringTransparent++;
                }
            }
        }
        if (ringTotal > 0 && ringTransparent * 100 / ringTotal >= 92) {
            int il = 0, it = 0, ir = 0, ib = 0;
            if (findBounds(l + inset * 2, t + inset * 2, r - inset * 2, b - inset * 2,
                           &il, &it, &ir, &ib)) {
                l = il; t = it; r = ir; b = ib;
                bw = r - l; bh = b - t;
            }
        }
    }

    int full = std::max(w, h);
    int content = std::max(bw, bh);
    if (content < (int)(full * 0.68)) {
        int pad = std::max(1, (int)std::lround(content * 0.04));
        int sx0 = std::max(l - pad, 0);
        int sy0 = std::max(t - pad, 0);
        int sx1 = std::min(r + pad, w);
        int sy1 = std::min(b + pad, h);
        return ResampleSubRect(px.data(), w, h, sx0, sy0, sx1, sy1, targetPx);
    }

    if (w != targetPx || h != targetPx) {
        return ResampleSubRect(px.data(), w, h, 0, 0, w, h, targetPx);
    }
    return CreatePremultipliedDIB(w, h, px.data());
}

HBITMAP ScaleIconBox(HBITMAP src, int dst) {
    BITMAP bm = {};
    if (!GetObject(src, sizeof(bm), &bm) || bm.bmWidth < 1 || bm.bmHeight < 1 || dst < 1) return nullptr;
    int sw = bm.bmWidth, sh = std::abs(bm.bmHeight);
    std::vector<uint32_t> in((size_t)sw * sh);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = sw;
    bi.bmiHeader.biHeight = -sh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    EnsureScratchDC();
    if (GetDIBits(g_scratchDC, src, 0, sh, in.data(), &bi, DIB_RGB_COLORS) != sh) return nullptr;
    return ResampleSubRect(in.data(), sw, sh, 0, 0, sw, sh, dst);
}

void EnsureAppsFolder(bool forceRefresh = false) {
    if (forceRefresh && g_appsFolder) {
        g_appsFolder->Release();
        g_appsFolder = nullptr;
    }
    if (g_appsFolder) return;
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_AppsFolder, 0, nullptr, &pidl))) {
        SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&g_appsFolder));
        CoTaskMemFree(pidl);
    }
}

void EnsureAppIcon(App& a, int px) {
    if (a.iconPx == px && (a.icon || a.iconFailed)) return;
    if (a.icon) DeleteObject(a.icon);
    if (a.mini) DeleteObject(a.mini);
    a.icon = a.mini = nullptr;
    a.iconPx = px;
    a.iconFailed = false;
    EnsureAppsFolder();
    if (!g_appsFolder) return;

    IShellItem* item = nullptr;
    if (SUCCEEDED(SHCreateItemFromRelativeName(g_appsFolder, a.id.c_str(),
                                               nullptr, IID_PPV_ARGS(&item))) && item) {
        IShellItemImageFactory* factory = nullptr;
        if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory)))) {
            int reqPx = std::max(px, 256);
            SIZE sz = {reqPx, reqPx};
            HBITMAP raw = nullptr;
            if (SUCCEEDED(factory->GetImage(sz, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &raw)) && raw) {
                a.icon = NormalizeAndTrimIcon(raw, px);
            } else if (SUCCEEDED(factory->GetImage(SIZE{px, px}, SIIGBF_ICONONLY, &raw)) && raw) {
                a.icon = NormalizeAndTrimIcon(raw, px);
            }
            factory->Release();
        }
        item->Release();
        if (!a.icon) a.iconFailed = true;
    }
}

void FreeApps() {
    for (auto& a : g_apps) {
        if (a.icon) DeleteObject(a.icon);
        if (a.mini) DeleteObject(a.mini);
    }
    g_apps.clear();
    g_appIndex.clear();
}

struct CachedIcons { HBITMAP icon; int px; HBITMAP mini; int miniPx; };

void LoadApps(int px) {
    g_loadPx = px;
    EnsureAppsFolder(true);
    if (!g_appsFolder) return;

    std::unordered_map<std::wstring, CachedIcons> oldIcons;
    for (auto& a : g_apps) {
        if (a.icon && a.iconPx == px) {
            oldIcons[a.id] = {a.icon, a.iconPx, a.mini, a.miniPx};
            a.icon = nullptr;
            a.mini = nullptr;
        }
    }
    FreeApps();

    IEnumShellItems* en = nullptr;
    if (SUCCEEDED(g_appsFolder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&en)))) {
        IShellItem* item = nullptr;
        ULONG fetched = 0;
        while (en->Next(1, &item, &fetched) == S_OK && fetched == 1) {
            PWSTR name = nullptr, parse = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) &&
                SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &parse))) {
                PWSTR fsPath = nullptr;
                bool deadLink = false;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &fsPath)) && fsPath) {
                    if (GetFileAttributesW(fsPath) == INVALID_FILE_ATTRIBUTES) {
                        deadLink = true;
                    }
                    CoTaskMemFree(fsPath);
                }
                if (!deadLink) {
                    App a;
                    a.name = name;
                    a.sortKey = MakeCustomSortKey(a.name);
                    a.id = parse;
                    a.lower = a.name;
                    CharLowerBuffW(a.lower.data(), (DWORD)a.lower.size());
                    a.idLower = a.id;
                    CharLowerBuffW(a.idLower.data(), (DWORD)a.idLower.size());

                    auto it = oldIcons.find(a.id);
                    if (it != oldIcons.end()) {
                        a.icon = it->second.icon;
                        a.iconPx = it->second.px;
                        a.mini = it->second.mini;
                        a.miniPx = it->second.miniPx;
                        oldIcons.erase(it);
                    }
                    g_apps.push_back(std::move(a));
                }
            }
            if (name) CoTaskMemFree(name);
            if (parse) CoTaskMemFree(parse);
            item->Release();
            item = nullptr;
        }
        en->Release();
    }
    for (auto& kv : oldIcons) {
        if (kv.second.icon) DeleteObject(kv.second.icon);
        if (kv.second.mini) DeleteObject(kv.second.mini);
    }

    std::sort(g_apps.begin(), g_apps.end(), [](const App& a, const App& b) {
        return CustomAlphaLess(a.sortKey, b.sortKey, a.lower, b.lower);
    });
    for (size_t i = 0; i < g_apps.size(); i++) g_appIndex[g_apps[i].id] = (int)i;

    if (ReconcileLayout()) {
        SaveLayout();
    }
    InterlockedExchange(&g_appsDirty, 0);

    g_prewarmIdx = 0;
    if (g_wnd) PostMessageW(g_wnd, WM_LP_PREWARM, 0, 0);
}

int AppIndexOf(const std::wstring& id) {
    auto f = g_appIndex.find(id);
    return f == g_appIndex.end() ? -1 : f->second;
}

std::vector<int> GetVisibleFolderApps(const LItem& f) {
    std::vector<int> out;
    for (const auto& id : f.ids) {
        int ai = AppIndexOf(id);
        if (ai >= 0 && !IsFilteredOut(g_apps[ai], false)) out.push_back(ai);
    }
    return out;
}

void EnsureMini(App& a, int iconPx, int miniPx) {
    EnsureAppIcon(a, iconPx);
    if (a.mini && a.miniPx == miniPx) return;
    if (a.mini) { DeleteObject(a.mini); a.mini = nullptr; }
    a.miniPx = miniPx;
    if (a.icon && miniPx >= 4) a.mini = ScaleIconBox(a.icon, miniPx);
}

void PrewarmPage(const Layout& l, int pageIdx) {
    if (pageIdx < 0 || pageIdx >= l.pages) return;
    int baseMini = BaseMiniPxFor(l.icon);

    int startIdx = pageIdx * l.perPage;
    int endIdx = std::min((int)g_view.size(), startIdx + l.perPage);
    for (int i = startIdx; i < endIdx; i++) {
        const Item& it = g_view[i];
        if (it.app >= 0 && it.app < (int)g_apps.size()) {
            EnsureAppIcon(g_apps[it.app], l.icon);
        } else if (it.folder >= 0 && it.folder < (int)g_layout.size()) {
            std::vector<int> fApps = GetVisibleFolderApps(g_layout[it.folder]);
            for (size_t j = 0; j < 9 && j < fApps.size(); j++) {
                if (baseMini >= 4) EnsureMini(g_apps[fApps[j]], l.icon, baseMini);
            }
        }
    }
}

int MatchScore(const std::wstring& lowerName, const std::wstring& q) {
    size_t pos = lowerName.find(q);
    if (pos == std::wstring::npos) return -1;
    if (pos == 0) return 0;
    wchar_t prev = lowerName[pos - 1];
    if (prev == L' ' || prev == L'-' || prev == L'_' || prev == L'.') return 1;
    return 2;
}

void RebuildView() {
    std::wstring q = g_query;
    CharLowerBuffW(q.data(), (DWORD)q.size());
    g_view.clear();
    if (q.empty()) {
        for (size_t i = 0; i < g_layout.size(); i++) {
            Item it;
            if (g_layout[i].folder) {
                std::vector<int> vis = GetVisibleFolderApps(g_layout[i]);
                if (vis.empty()) continue;
                it.folder = (int)i;
            } else {
                auto f = g_appIndex.find(g_layout[i].id);
                if (f == g_appIndex.end() || IsFilteredOut(g_apps[f->second], false)) continue;
                it.app = f->second;
            }
            g_view.push_back(it);
        }
    } else {
        struct Scored { int score, appIdx; };
        std::vector<Scored> matches;
        for (size_t i = 0; i < g_apps.size(); i++) {
            if (IsFilteredOut(g_apps[i], true)) continue;
            int sc = MatchScore(g_apps[i].lower, q);
            if (sc >= 0) matches.push_back({sc, (int)i});
        }
        std::stable_sort(matches.begin(), matches.end(),
                         [](const Scored& a, const Scored& b) { return a.score < b.score; });
        for (const auto& m : matches) {
            Item it; it.app = m.appIdx; g_view.push_back(it);
        }
    }
    g_viewHover.assign(g_view.size(), 0.0);
    g_gridCacheValid = false;
    InvalidateSnaps();
}

void Refilter() {
    RebuildView();
    g_page = 0;
    g_pagePos = 0.0;
    g_pageVel = 0.0;
    g_pageAnimating = false;
    if (!g_query.empty() && !g_view.empty()) {
        g_sel = 0;
        g_keyNav = true;
        g_viewHover[0] = 1.0;
    } else {
        g_sel = -1;
        g_keyNav = false;
    }
}

void HideSingleApp(int appIdx) {
    if (appIdx < 0 || appIdx >= (int)g_apps.size()) return;
    g_hiddenIds.insert(g_apps[appIdx].id);
    SaveLayout();
    RebuildView();
    Layout l = GetLayout(g_w, g_h, g_view.size());
    ClampPage(l);
    g_sel = -1;
    if (g_openFolder >= 0 && g_openFolder < (int)g_layout.size()) {
        if (GetVisibleFolderApps(g_layout[g_openFolder]).empty()) {
            CloseFolder(true);
        }
    }
    Redraw();
}

void ToggleEditHiddenApp(int appIdx) {
    if (appIdx < 0 || appIdx >= (int)g_apps.size()) return;
    const std::wstring& id = g_apps[appIdx].id;
    if (g_hiddenIds.count(id)) g_hiddenIds.erase(id);
    else g_hiddenIds.insert(id);
    Redraw();
}

void FinishEditHiddenMode() {
    if (!g_editHiddenMode) return;
    g_editHiddenMode = false;
    SaveLayout();
    RebuildView();
    Layout l = GetLayout(g_w, g_h, g_view.size());
    ClampPage(l);
    g_sel = -1;
    Redraw();
}

void ToggleFolderBuildApp(int appIdx) {
    if (appIdx < 0 || appIdx >= (int)g_apps.size()) return;
    const std::wstring& id = g_apps[appIdx].id;
    auto it = std::find(g_folderBuildIds.begin(), g_folderBuildIds.end(), id);
    if (it != g_folderBuildIds.end()) {
        g_folderBuildIds.erase(it);
    } else {
        g_folderBuildIds.push_back(id);
    }
    Redraw();
}

void OpenFolder(int li);
void StartRename();

void FinishCreateFolderMode(bool commit) {
    if (!g_createFolderMode) return;
    g_createFolderMode = false;
    if (!commit || g_folderBuildIds.size() < 2) {
        g_folderBuildIds.clear();
        Redraw();
        return;
    }

    std::unordered_set<std::wstring> picked(g_folderBuildIds.begin(),
                                            g_folderBuildIds.end());
    int insertPos = -1;
    std::vector<LItem> nextLayout;
    for (LItem& it : g_layout) {
        if (!it.folder) {
            if (picked.count(it.id)) {
                if (insertPos < 0) insertPos = (int)nextLayout.size();
                continue;
            }
            nextLayout.push_back(std::move(it));
        } else {
            it.ids.erase(
                std::remove_if(it.ids.begin(), it.ids.end(),
                               [&](const std::wstring& id) {
                                   return picked.count(id) > 0;
                               }),
                it.ids.end());
            if (it.ids.size() >= 2) {
                nextLayout.push_back(std::move(it));
            } else if (it.ids.size() == 1) {
                LItem single;
                single.id = it.ids[0];
                nextLayout.push_back(std::move(single));
            }
        }
    }

    if (insertPos < 0 || insertPos > (int)nextLayout.size()) {
        insertPos = (int)nextLayout.size();
    }
    LItem newFolder;
    newFolder.folder = true;
    newFolder.name = L"Folder";
    newFolder.ids = std::move(g_folderBuildIds);
    g_folderBuildIds.clear();

    nextLayout.insert(nextLayout.begin() + insertPos, std::move(newFolder));
    g_layout.swap(nextLayout);
    SaveLayout();
    RebuildView();
    g_sel = -1;

    OpenFolder(insertPos);
    StartRename();
    Redraw();
}

void LaunchApp(int appIdx, bool asAdmin = false) {
    if (appIdx < 0 || appIdx >= (int)g_apps.size()) return;
    std::wstring target = L"shell:AppsFolder\\" + g_apps[appIdx].id;
    AllowSetForegroundWindow(ASFW_ANY);
    ShellExecuteW(nullptr, asAdmin ? L"runas" : L"open", target.c_str(),
                  nullptr, nullptr, SW_SHOWNORMAL);
}

void OpenAppLocation(int appIdx) {
    if (appIdx < 0 || appIdx >= (int)g_apps.size()) return;
    EnsureAppsFolder();
    if (!g_appsFolder) return;
    IShellItem* item = nullptr;
    if (SUCCEEDED(SHCreateItemFromRelativeName(g_appsFolder, g_apps[appIdx].id.c_str(),
                                               nullptr, IID_PPV_ARGS(&item))) && item) {
        PWSTR fsPath = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &fsPath)) && fsPath) {
            PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(fsPath);
            if (pidl) { SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0); ILFree(pidl); }
            CoTaskMemFree(fsPath);
        } else {
            PIDLIST_ABSOLUTE pidl = nullptr;
            if (SUCCEEDED(SHGetIDListFromObject(item, &pidl)) && pidl) {
                SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
                CoTaskMemFree(pidl);
            }
        }
        item->Release();
    }
}

// Taskbar-pin / uninstall entries come straight from the shell's own context
// menu for the app, so the label is localized and flips between "Pin to
// taskbar" and "Unpin from taskbar" automatically. An entry is only offered
// when the shell exposes it for that app.
struct ShellVerbs {
    IContextMenu* cm = nullptr;
    HMENU hm = nullptr;
    int pinCmd = -1, uninstallCmd = -1;
    std::wstring pinText, uninstallText;
    ~ShellVerbs() {
        if (hm) DestroyMenu(hm);
        if (cm) cm->Release();
    }
};

void LoadShellVerbs(int appIdx, ShellVerbs& sv) {
    if (appIdx < 0 || appIdx >= (int)g_apps.size()) return;
    EnsureAppsFolder();
    if (!g_appsFolder) return;
    IShellItem* item = nullptr;
    if (FAILED(SHCreateItemFromRelativeName(g_appsFolder, g_apps[appIdx].id.c_str(),
                                            nullptr, IID_PPV_ARGS(&item))) || !item) return;
    HRESULT hr = item->BindToHandler(nullptr, BHID_SFUIObject, IID_PPV_ARGS(&sv.cm));
    item->Release();
    if (FAILED(hr) || !sv.cm) { sv.cm = nullptr; return; }

    sv.hm = CreatePopupMenu();
    if (!sv.hm || FAILED(sv.cm->QueryContextMenu(sv.hm, 0, 1, 0x7FFF, CMF_NORMAL))) return;

    int count = GetMenuItemCount(sv.hm);
    for (int i = 0; i < count; i++) {
        wchar_t text[160] = {};
        MENUITEMINFOW mii = {sizeof(mii)};
        mii.fMask = MIIM_ID | MIIM_FTYPE | MIIM_STRING;
        mii.dwTypeData = text;
        mii.cch = 159;
        if (!GetMenuItemInfoW(sv.hm, i, TRUE, &mii) || (mii.fType & MFT_SEPARATOR)) continue;
        if (mii.wID < 1 || mii.wID > 0x7FFF) continue;

        wchar_t verb[128] = {};
        if (FAILED(sv.cm->GetCommandString(mii.wID - 1, GCS_VERBW, nullptr,
                                           reinterpret_cast<LPSTR>(verb), 127))) continue;
        std::wstring v = verb;
        for (auto& c : v) c = (wchar_t)towlower(c);

        if (sv.pinCmd < 0 && (v == L"taskbarpin" || v == L"taskbarunpin" ||
                              v == L"pintotaskbar" || v == L"unpinfromtaskbar")) {
            sv.pinCmd = (int)mii.wID;
            sv.pinText = text;
        } else if (sv.uninstallCmd < 0 && v == L"uninstall") {
            sv.uninstallCmd = (int)mii.wID;
            sv.uninstallText = text;
        }
    }
}

void InvokeShellVerb(ShellVerbs& sv, int cmd) {
    if (!sv.cm || cmd < 1) return;
    CMINVOKECOMMANDINFOEX ci = {};
    ci.cbSize = sizeof(ci);
    ci.fMask = CMIC_MASK_UNICODE;
    ci.hwnd = g_wnd;
    ci.lpVerb = MAKEINTRESOURCEA(cmd - 1);
    ci.lpVerbW = MAKEINTRESOURCEW(cmd - 1);
    ci.nShow = SW_SHOWNORMAL;
    sv.cm->InvokeCommand(reinterpret_cast<CMINVOKECOMMANDINFO*>(&ci));
}

void FreeBackdrop() {
    if (g_backdrop) { DeleteObject(g_backdrop); g_backdrop = nullptr; }
}

void BoxBlurLine(const uint32_t* src, uint32_t* dst, int n, int step, int r) {
    const int win = 2 * r + 1;
    auto at = [&](int i) -> uint32_t {
        return src[std::min(std::max(i, 0), n - 1) * step];
    };
    int sb = 0, sg = 0, sr = 0;
    for (int i = -r; i <= r; i++) {
        uint32_t p = at(i);
        sb += p & 255; sg += (p >> 8) & 255; sr += (p >> 16) & 255;
    }
    for (int i = 0; i < n; i++) {
        dst[i * step] = (uint32_t)(sb / win) | ((uint32_t)(sg / win) << 8) | ((uint32_t)(sr / win) << 16);
        uint32_t add = at(i + r + 1), rem = at(i - r);
        sb += (int)(add & 255) - (int)(rem & 255);
        sg += (int)((add >> 8) & 255) - (int)((rem >> 8) & 255);
        sr += (int)((add >> 16) & 255) - (int)((rem >> 16) & 255);
    }
}

void BlurImage(uint32_t* px, int w, int h, int r, int passes) {
    size_t total = (size_t)w * h;
    if (g_blurTmp.size() < total) g_blurTmp.resize(total);
    for (int p = 0; p < passes; p++) {
        for (int y = 0; y < h; y++) BoxBlurLine(px + (size_t)y * w, g_blurTmp.data() + (size_t)y * w, w, 1, r);
        for (int x = 0; x < w; x++) BoxBlurLine(g_blurTmp.data() + x, px + x, h, w, r);
    }
}

void BuildBackdrop(int x, int y, int w, int h) {
    FreeBackdrop();
    HDC scr = GetDC(nullptr);
    if (!scr) return;

    int blurMode = (int)g_optBlur;
    int radius = blurMode == 1 ? 1 : (blurMode == 3 ? 4 : (blurMode == 0 ? 0 : 2));
    int down = blurMode == 0 ? 1 : kBlurDownscale;
    int keep = (256 * (100 - (int)g_optDark) + 50) / 100;
    int invKeep = 256 - keep;
    int tintB = g_tintB * invKeep, tintG = g_tintG * invKeep, tintR = g_tintR * invKeep;

    int sw = std::max(8, w / down), sh = std::max(8, h / down);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = sw;
    bi.bmiHeader.biHeight = -sh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* sbits = nullptr;
    HBITMAP shrunk = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &sbits, nullptr, 0);
    if (!shrunk || !sbits) {
        if (shrunk) DeleteObject(shrunk);
        ReleaseDC(nullptr, scr);
        return;
    }
    HDC smallDC = CreateCompatibleDC(scr);
    HGDIOBJ oldSmall = SelectObject(smallDC, shrunk);
    SetStretchBltMode(smallDC, HALFTONE);
    SetBrushOrgEx(smallDC, 0, 0, nullptr);
    StretchBlt(smallDC, 0, 0, sw, sh, scr, x, y, w, h, SRCCOPY);
    GdiFlush();

    auto* sp = static_cast<uint32_t*>(sbits);
    if (radius > 0) BlurImage(sp, sw, sh, radius, kBlurPasses);

    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    void* fbits = nullptr;
    HBITMAP full = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &fbits, nullptr, 0);
    if (full && fbits) {
        auto* fp = static_cast<uint32_t*>(fbits);
        if ((int)g_blurXi0.size() < w) {
            g_blurXi0.resize(w); g_blurXi1.resize(w); g_blurXf.resize(w);
        }
        for (int i = 0; i < w; i++) {
            long long sx = ((long long)(2 * i + 1) * sw * 128) / w - 128;
            int i0 = (int)(sx >> 8);
            g_blurXf[i] = (int)(sx & 255);
            g_blurXi0[i] = std::min(std::max(i0, 0), sw - 1);
            g_blurXi1[i] = std::min(std::max(i0 + 1, 0), sw - 1);
        }

        const int *xi0 = g_blurXi0.data(), *xi1 = g_blurXi1.data(), *xf = g_blurXf.data();
        for (int j = 0; j < h; j++) {
            long long sy = ((long long)(2 * j + 1) * sh * 128) / h - 128;
            int j0 = (int)(sy >> 8), fy = (int)(sy & 255), ify = 256 - fy;
            const uint32_t* row0 = sp + (size_t)std::min(std::max(j0, 0), sh - 1) * sw;
            const uint32_t* row1 = sp + (size_t)std::min(std::max(j0 + 1, 0), sh - 1) * sw;
            uint32_t* out = fp + (size_t)j * w;

            for (int i = 0; i < w; i++) {
                uint32_t p00 = row0[xi0[i]], p10 = row0[xi1[i]];
                uint32_t p01 = row1[xi0[i]], p11 = row1[xi1[i]];
                int fx = xf[i], ifx = 256 - fx;

                int topB = ((p00 & 255) * ifx + (p10 & 255) * fx) >> 8;
                int botB = ((p01 & 255) * ifx + (p11 & 255) * fx) >> 8;
                int vB = (((topB * ify + botB * fy) >> 8) * keep + tintB) >> 8;

                int topG = (((p00 >> 8) & 255) * ifx + ((p10 >> 8) & 255) * fx) >> 8;
                int botG = (((p01 >> 8) & 255) * ifx + ((p11 >> 8) & 255) * fx) >> 8;
                int vG = (((topG * ify + botG * fy) >> 8) * keep + tintG) >> 8;

                int topR = (((p00 >> 16) & 255) * ifx + ((p10 >> 16) & 255) * fx) >> 8;
                int botR = (((p01 >> 16) & 255) * ifx + ((p11 >> 16) & 255) * fx) >> 8;
                int vR = (((topR * ify + botR * fy) >> 8) * keep + tintR) >> 8;

                out[i] = (uint32_t)vB | ((uint32_t)vG << 8) | ((uint32_t)vR << 16);
            }
        }
        g_backdrop = full;
    } else if (full) {
        DeleteObject(full);
    }

    SelectObject(smallDC, oldSmall);
    DeleteObject(shrunk);
    DeleteDC(smallDC);
    ReleaseDC(nullptr, scr);
}

void FreeFrame() {
    if (g_frameDC) {
        if (g_frameOld) SelectObject(g_frameDC, g_frameOld);
        DeleteDC(g_frameDC);
        g_frameDC = nullptr;
        g_frameOld = nullptr;
    }
    if (g_frame) { DeleteObject(g_frame); g_frame = nullptr; }
    if (g_gridCacheDC) {
        if (g_gridCacheOld) SelectObject(g_gridCacheDC, g_gridCacheOld);
        DeleteDC(g_gridCacheDC);
        g_gridCacheDC = nullptr;
        g_gridCacheOld = nullptr;
    }
    if (g_gridCacheBmp) { DeleteObject(g_gridCacheBmp); g_gridCacheBmp = nullptr; }
    g_gridCacheValid = false;
    InvalidateSnaps();
}

void EnsureFrame(int w, int h) {
    FreeFrame();
    HDC ref = GetDC(g_wnd);
    g_frameDC = CreateCompatibleDC(ref);
    g_frame = CreateCompatibleBitmap(ref, w, h);
    if (g_frameDC && g_frame) g_frameOld = SelectObject(g_frameDC, g_frame);

    g_gridCacheDC = CreateCompatibleDC(ref);
    g_gridCacheBmp = CreateCompatibleBitmap(ref, w, h);
    if (g_gridCacheDC && g_gridCacheBmp) g_gridCacheOld = SelectObject(g_gridCacheDC, g_gridCacheBmp);
    ReleaseDC(g_wnd, ref);
}

struct Rgba { double r, g, b, a; };
double Clamp01(double v) { return v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v); }

double SdRoundRect(double px, double py, double hw, double hh, double r) {
    double qx = std::fabs(px) - (hw - r), qy = std::fabs(py) - (hh - r);
    double ox = std::max(qx, 0.0), oy = std::max(qy, 0.0);
    return std::sqrt(ox * ox + oy * oy) + std::min(std::max(qx, qy), 0.0) - r;
}

double SdSegment(double px, double py, double ax, double ay, double bx, double by) {
    double dx = bx - ax, dy = by - ay;
    double t = Clamp01(((px - ax) * dx + (py - ay) * dy) / (dx * dx + dy * dy));
    return std::hypot(ax + t * dx - px, ay + t * dy - py);
}

template <class F>
HBITMAP MakeAlphaBitmap(int w, int h, F shade) {
    if (w < 1 || h < 1) return nullptr;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) { if (bmp) DeleteObject(bmp); return nullptr; }
    auto* p = static_cast<uint32_t*>(bits);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Rgba c = shade(x + 0.5, y + 0.5);
            uint32_t a = (uint32_t)std::lround(c.a * 255.0);
            uint32_t r = (uint32_t)std::lround(c.r * c.a * 255.0);
            uint32_t g = (uint32_t)std::lround(c.g * c.a * 255.0);
            uint32_t b = (uint32_t)std::lround(c.b * c.a * 255.0);
            p[(size_t)y * w + x] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
    return bmp;
}

HBITMAP MakeRoundShape(int w, int h, double radius, double bw, Rgba fill, Rgba border) {
    return MakeAlphaBitmap(w, h, [=](double x, double y) {
        double d = SdRoundRect(x - w / 2.0, y - h / 2.0, w / 2.0, h / 2.0, radius);
        double outer = Clamp01(0.5 - d);
        double inner = Clamp01(0.5 - (d + bw));
        double ring = std::max(0.0, outer - inner);
        double af = fill.a * outer, ab = border.a * ring;
        double a = ab + af * (1.0 - ab);
        if (a <= 0.0) return Rgba{0, 0, 0, 0};
        double k = 1.0 / a;
        return Rgba{(border.r * ab + fill.r * af * (1.0 - ab)) * k,
                    (border.g * ab + fill.g * af * (1.0 - ab)) * k,
                    (border.b * ab + fill.b * af * (1.0 - ab)) * k, a};
    });
}

// Windows 11 style box, copied from the sign-in PIN box: near-neutral dark translucent
// fill, a faint 1 px light rim (white 12%), small corner radius and a 2 px accent underline.
HBITMAP MakeFluentSearchBox(int w, int h, double radius, double bw, COLORREF accent, bool lightTheme) {
    double ar = GetRValue(accent) / 255.0, ag = GetGValue(accent) / 255.0, ab = GetBValue(accent) / 255.0;
    double accentH = std::max(2.0, h * 0.057);
    Rgba fill = lightTheme ? Rgba{0.98, 0.98, 1.0, 0.80} : Rgba{29 / 255.0, 31 / 255.0, 38 / 255.0, 0.72};
    Rgba border = lightTheme ? Rgba{0.0, 0.0, 0.0, 0.12} : Rgba{1.0, 1.0, 1.0, 0.12};

    return MakeAlphaBitmap(w, h, [=](double x, double y) {
        double d = SdRoundRect(x - w / 2.0, y - h / 2.0, w / 2.0, h / 2.0, radius);
        double outer = Clamp01(0.5 - d);
        if (outer <= 0.0) return Rgba{0, 0, 0, 0};
        if (y >= h - accentH) {
            double barMask = Clamp01((y - (h - accentH - 0.5)));
            double a = outer * (barMask + fill.a * (1.0 - barMask));
            double k = outer / std::max(a, 1e-5);
            return Rgba{(ar * barMask + fill.r * fill.a * (1.0 - barMask)) * k,
                        (ag * barMask + fill.g * fill.a * (1.0 - barMask)) * k,
                        (ab * barMask + fill.b * fill.a * (1.0 - barMask)) * k, a};
        }
        double inner = Clamp01(0.5 - (d + bw));
        double ring = std::max(0.0, outer - inner);
        double af = fill.a * outer, abor = border.a * ring;
        double a = abor + af * (1.0 - abor);
        if (a <= 0.0) return Rgba{0, 0, 0, 0};
        double k = 1.0 / a;
        return Rgba{(border.r * abor + fill.r * af * (1.0 - abor)) * k,
                    (border.g * abor + fill.g * af * (1.0 - abor)) * k,
                    (border.b * abor + fill.b * af * (1.0 - abor)) * k, a};
    });
}

// macOS Launchpad search field, copied from a screenshot: a full pill, flat white 12% fill
// and a uniform 1 px white 20% rim (no gradient).
HBITMAP MakeMacSearchPill(int w, int h, double bw, bool lightTheme) {
    Rgba fill = lightTheme ? Rgba{1.0, 1.0, 1.0, 0.60} : Rgba{1.0, 1.0, 1.0, 0.12};
    Rgba border = lightTheme ? Rgba{0.1, 0.12, 0.18, 0.22} : Rgba{1.0, 1.0, 1.0, 0.20};
    return MakeAlphaBitmap(w, h, [=](double x, double y) {
        double d = SdRoundRect(x - w / 2.0, y - h / 2.0, w / 2.0, h / 2.0, h / 2.0);
        double outer = Clamp01(0.5 - d);
        if (outer <= 0.0) return Rgba{0, 0, 0, 0};
        double inner = Clamp01(0.5 - (d + bw));
        double ring = std::max(0.0, outer - inner);
        double af = fill.a * outer, abor = border.a * ring;
        double a = abor + af * (1.0 - abor);
        if (a <= 0.0) return Rgba{0, 0, 0, 0};
        double k = 1.0 / a;
        return Rgba{(border.r * abor + fill.r * af * (1.0 - abor)) * k,
                    (border.g * abor + fill.g * af * (1.0 - abor)) * k,
                    (border.b * abor + fill.b * af * (1.0 - abor)) * k, a};
    });
}

HBITMAP MakeFolderPanelBitmap(int w, int h, double radius, double bw, bool winStyle, bool lightTheme) {
    Rgba fill = lightTheme
                    ? (winStyle ? Rgba{0.96, 0.97, 0.99, 0.82} : Rgba{1.0, 1.0, 1.0, 0.42})
                    : (winStyle ? Rgba{0.12, 0.12, 0.14, 0.72} : Rgba{0.08, 0.09, 0.14, 0.24});
    return MakeAlphaBitmap(w, h, [=](double x, double y) {
        double d = SdRoundRect(x - w / 2.0, y - h / 2.0, w / 2.0, h / 2.0, radius);
        double outer = Clamp01(0.5 - d);
        if (outer <= 0.0) return Rgba{0, 0, 0, 0};
        double inner = Clamp01(0.5 - (d + bw));
        double ring = std::max(0.0, outer - inner);
        double vGrad = 1.0 - 0.55 * (y / std::max(1.0, (double)h));
        double borderAlpha = (winStyle ? 0.22 : 0.36) * vGrad;
        double br = lightTheme ? 0.15 : 1.0;
        double af = fill.a * outer, ab = borderAlpha * ring;
        double a = ab + af * (1.0 - ab);
        if (a <= 0.0) return Rgba{0, 0, 0, 0};
        double k = 1.0 / a;
        return Rgba{(br * ab + fill.r * af * (1.0 - ab)) * k,
                    (br * ab + fill.g * af * (1.0 - ab)) * k,
                    (br * ab + fill.b * af * (1.0 - ab)) * k, a};
    });
}

HBITMAP MakeMagnifier(int g, double sw, bool lightTheme, double alpha) {
    double R = g * 0.31, cx = g * 0.42, cy = g * 0.42;
    double hx0 = cx + R * 0.7071, hy0 = cy + R * 0.7071, hx1 = g * 0.90, hy1 = g * 0.90;
    double c = lightTheme ? 0.10 : 1.0;
    return MakeAlphaBitmap(g, g, [=](double x, double y) {
        double dRing = std::fabs(std::hypot(x - cx, y - cy) - R) - sw / 2.0;
        double dHandle = SdSegment(x, y, hx0, hy0, hx1, hy1) - sw / 2.0;
        return Rgba{c, c, c, alpha * Clamp01(0.5 - std::min(dRing, dHandle))};
    });
}

HBITMAP MakeClearGlyph(int g, double sw, bool lightTheme, double alpha) {
    double c = g / 2.0, R = g / 2.0 - sw, k = R * 0.42, col = lightTheme ? 0.10 : 1.0;
    return MakeAlphaBitmap(g, g, [=](double x, double y) {
        double dRing = std::fabs(std::hypot(x - c, y - c) - R) - sw / 2.0;
        double d1 = SdSegment(x, y, c - k, c - k, c + k, c + k) - sw / 2.0;
        double d2 = SdSegment(x, y, c - k, c + k, c + k, c - k) - sw / 2.0;
        return Rgba{col, col, col, alpha * Clamp01(0.5 - std::min(dRing, std::min(d1, d2)))};
    });
}

// Circled "..." (ring + three dots), proportions measured from macOS.
HBITMAP MakeMoreGlyph(int g, double sw, bool lightTheme, double alpha) {
    double c = g / 2.0, R = g / 2.0 - sw, dotR = std::max(1.1, g * 0.065), dx = R * 0.56;
    double col = lightTheme ? 0.10 : 1.0;
    return MakeAlphaBitmap(g, g, [=](double x, double y) {
        double dRing = std::fabs(std::hypot(x - c, y - c) - R) - sw / 2.0;
        double dDot1 = std::hypot(x - (c - dx), y - c) - dotR;
        double dDot2 = std::hypot(x - c, y - c) - dotR;
        double dDot3 = std::hypot(x - (c + dx), y - c) - dotR;
        double d = std::min(std::min(dRing, dDot1), std::min(dDot2, dDot3));
        return Rgba{col, col, col, alpha * Clamp01(0.5 - d)};
    });
}

HBITMAP MakeBadgeBitmap(int g, int mode) {
    double c = g / 2.0, R = g / 2.0 - 1.2, sw = std::max(1.5, g * 0.095);
    return MakeAlphaBitmap(g, g, [=](double x, double y) {
        double dist = std::hypot(x - c, y - c) - R;
        double outer = Clamp01(0.5 - dist);
        if (outer <= 0.0) return Rgba{0, 0, 0, 0};
        if (mode == 2) {
            double ring = std::fabs(dist) - sw * 0.55;
            double stroke = Clamp01(0.5 - ring);
            return Rgba{1.0, 1.0, 1.0, std::max(0.28 * outer, 0.85 * stroke)};
        }
        if (mode == 1) {
            double dBar = SdSegment(x, y, g * 0.28, c, g * 0.72, c) - sw * 0.55;
            double bar = Clamp01(0.5 - dBar);
            return Rgba{0.85 + 0.15 * bar, 0.24 + 0.76 * bar, 0.28 + 0.72 * bar, 0.92 * outer};
        }
        double d1 = SdSegment(x, y, g * 0.28, g * 0.52, g * 0.44, g * 0.68) - sw * 0.55;
        double d2 = SdSegment(x, y, g * 0.44, g * 0.68, g * 0.73, g * 0.34) - sw * 0.55;
        double chk = Clamp01(0.5 - std::min(d1, d2));
        return Rgba{0.20 + 0.80 * chk, 0.56 + 0.44 * chk, 0.98 + 0.02 * chk, 0.92 * outer};
    });
}

void FreeUiBitmaps() {
    HBITMAP* all[] = {&g_bmPill, &g_bmSel, &g_bmSelRect, &g_bmMag, &g_bmClear, &g_bmMore,
                      &g_bmDoneGlyph, &g_bmFolder,
                      &g_bmVisBadge, &g_bmHidBadge, &g_bmUncheckedBadge, &g_bmPanel,
                      &g_bmDotOn, &g_bmDotOff, &g_bmPowerGlyph, &g_icoSrc, &g_icoOut};
    for (HBITMAP* b : all) { if (*b) DeleteObject(*b); *b = nullptr; }
    g_icoSrcBits = g_icoOutBits = nullptr;
    g_icoSrcSz = g_icoOutSz = 0;
    if (g_icoDC) { DeleteDC(g_icoDC); g_icoDC = nullptr; }
    std::memset(g_dotKey, 0, sizeof(g_dotKey));
    std::memset(g_powerKey, 0, sizeof(g_powerKey));
    std::memset(g_bmKey, 0, sizeof(g_bmKey));
    std::memset(g_panelKey, 0, sizeof(g_panelKey));
}

int SelPad(int icon) { return (int)(icon * 0.14); }
int BadgePx(int icon) { return std::max(S(18), (int)(icon * 0.28)); }

void EnsureUiBitmaps(const SearchGeo& sg, const Layout& l) {
    int icon = l.icon;
    int pw = sg.bar.right - sg.bar.left, ph = sg.bar.bottom - sg.bar.top;
    int mg = sg.mag.right - sg.mag.left, cd = sg.rightBtn.right - sg.rightBtn.left;
    int selSz = icon + 2 * SelPad(icon), style = (int)g_optStyle, light = g_isLightTheme ? 1 : 0;
    int srW = 0, srH = 0;
    SelRectSize(l.cellW, icon, &srW, &srH);
    int key[11] = {pw, ph, mg, cd, selSz, srW, srH, g_dpi, style, (int)g_themeAccent, light};
    if (g_bmPill && std::memcmp(key, g_bmKey, sizeof(key)) == 0) return;

    FreeUiBitmaps();
    double bw = std::max(1.0, g_dpi / 96.0);

    if (style == 1) {
        g_bmPill = MakeFluentSearchBox(pw, ph, std::max(3.0, ph * 0.115), bw, g_themeAccent, g_isLightTheme);
    } else {
        g_bmPill = MakeMacSearchPill(pw, ph, bw, g_isLightTheme);
    }

    double folderRad = (style == 1) ? 0.16 : 0.24;
    g_bmSel = MakeRoundShape(selSz, selSz, selSz * 0.22, bw,
                             g_isLightTheme ? Rgba{0, 0, 0, 0.14} : Rgba{1, 1, 1, 0.18},
                             g_isLightTheme ? Rgba{0, 0, 0, 0.24} : Rgba{1, 1, 1, 0.30});
    g_bmSelRect = MakeRoundShape(srW, srH, S(style == 1 ? 8 : 14), bw,
                                 g_isLightTheme ? Rgba{0, 0, 0, 0.11} : Rgba{1, 1, 1, 0.15},
                                 g_isLightTheme ? Rgba{0, 0, 0, 0.20} : Rgba{1, 1, 1, 0.26});
    // macOS: white magnifier, 62% white "..." button (measured). Windows box: softer white.
    double magA = light ? 0.90 : (style == 1 ? 0.88 : 1.0);
    double sideA = light ? 0.74 : (style == 1 ? 0.72 : 0.62);
    g_bmMag = MakeMagnifier(mg, std::max(1.4, mg * 0.095), g_isLightTheme, magA);
    g_bmClear = MakeClearGlyph(cd, std::max(1.1, cd * 0.075), g_isLightTheme, std::min(1.0, sideA + 0.12));
    g_bmMore = MakeMoreGlyph(cd, std::max(1.1, cd * 0.075), g_isLightTheme, sideA);
    g_bmDoneGlyph = MakeBadgeBitmap(cd, 0);

    if (g_isLightTheme) {
        g_bmFolder = MakeRoundShape(icon, icon, icon * folderRad, bw, Rgba{1.0, 1.0, 1.0, 0.52}, Rgba{0.1, 0.12, 0.18, 0.24});
    } else {
        g_bmFolder = MakeRoundShape(icon, icon, icon * folderRad, bw, Rgba{0.78, 0.80, 0.88, 0.22}, Rgba{1, 1, 1, 0.34});
    }

    int bpx = BadgePx(icon);
    g_bmVisBadge = MakeBadgeBitmap(bpx, 0);
    g_bmHidBadge = MakeBadgeBitmap(bpx, 1);
    g_bmUncheckedBadge = MakeBadgeBitmap(bpx, 2);
    std::memcpy(g_bmKey, key, sizeof(key));
}

void EnsurePanelBitmap(int pw, int ph, int radius) {
    int style = (int)g_optStyle, light = g_isLightTheme ? 1 : 0;
    int key[5] = {pw, ph, radius, style, light};
    if (g_bmPanel && std::memcmp(key, g_panelKey, sizeof(key)) == 0) return;
    if (g_bmPanel) DeleteObject(g_bmPanel);
    double bw = std::max(1.1, g_dpi / 96.0);
    g_bmPanel = MakeFolderPanelBitmap(pw, ph, radius, bw, style == 1, g_isLightTheme);
    std::memcpy(g_panelKey, key, sizeof(key));
}

void BlendBitmap(HDC dc, HBITMAP bmp, int x, int y, int w, int h, BYTE alpha = 255) {
    if (!bmp) return;
    EnsureScratchDC();
    HGDIOBJ old = SelectObject(g_scratchDC, bmp);
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    AlphaBlend(dc, x, y, w, h, g_scratchDC, 0, 0, w, h, bf);
    SelectObject(g_scratchDC, old);
}

void BlendBitmapScaled(HDC dc, HBITMAP bmp, int x, int y, int dw, int dh, int sw, int sh, BYTE alpha = 255) {
    if (!bmp) return;
    EnsureScratchDC();
    HGDIOBJ old = SelectObject(g_scratchDC, bmp);
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    AlphaBlend(dc, x, y, dw, dh, g_scratchDC, 0, 0, sw, sh, bf);
    SelectObject(g_scratchDC, old);
}

void AreaScaleMask(const uint8_t* src, int sw, int sh, int dw, int dh, std::vector<uint8_t>& dst);

// Text coverage curve shared by the app titles and the search text, so both have the same weight.
const uint8_t* CoverageLut() {
    static uint8_t lutDark[256], lutLight[256];
    static bool lutInit = false;
    if (!lutInit) {
        for (int i = 0; i < 256; i++) {
            lutDark[i] = (uint8_t)std::lround(255.0 * std::pow(i / 255.0, 0.78));
            lutLight[i] = (uint8_t)std::lround(255.0 * std::pow(i / 255.0, 0.92));
        }
        lutInit = true;
    }
    return g_isLightTheme ? lutLight : lutDark;
}

const SearchTextMask* GetSearchTextMask(const std::wstring& text) {
    if (!g_fontSearchMask || text.empty()) return nullptr;
    std::wstring key = text + L"\x1f" + std::to_wstring(g_fontSearchPx);
    auto it = g_searchMasks.find(key);
    if (it != g_searchMasks.end()) return &it->second;
    if (g_searchMasks.size() > 64) g_searchMasks.clear();

    EnsureScratchDC();
    int ss = g_searchSS, pad = 2;
    int saved = SaveDC(g_scratchDC);
    SelectObject(g_scratchDC, g_fontSearchMask);
    SIZE sz = {};
    GetTextExtentPoint32W(g_scratchDC, text.c_str(), (int)text.size(), &sz);
    TEXTMETRICW tm = {};
    GetTextMetricsW(g_scratchDC, &tm);

    // Put the baseline exactly on a final-pixel boundary so it renders crisp.
    int ascSS = tm.tmAscent;
    int topSS = pad * ss + (ss - ascSS % ss) % ss;
    int wFinal = (int)std::ceil(sz.cx / (double)ss) + 2 * pad;
    int hFinal = (int)std::ceil((topSS + tm.tmHeight + pad * ss) / (double)ss);
    int wSS = wFinal * ss, hSS = hFinal * ss;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = wSS;
    bi.bmiHeader.biHeight = -hSS;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(g_scratchDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        RestoreDC(g_scratchDC, saved);
        if (dib) DeleteObject(dib);
        return nullptr;
    }
    std::memset(bits, 0, (size_t)wSS * hSS * 4);
    SelectObject(g_scratchDC, dib);
    SetBkMode(g_scratchDC, TRANSPARENT);
    SetTextColor(g_scratchDC, RGB(255, 255, 255));
    TextOutW(g_scratchDC, pad * ss, topSS, text.c_str(), (int)text.size());
    GdiFlush();
    RestoreDC(g_scratchDC, saved);

    std::vector<uint8_t> big((size_t)wSS * hSS);
    const uint32_t* px = static_cast<const uint32_t*>(bits);
    for (size_t i = 0; i < big.size(); i++) {
        uint32_t v = px[i];
        big[i] = (uint8_t)((((v >> 16) & 255) + ((v >> 8) & 255) + (v & 255)) / 3);
    }
    DeleteObject(dib);

    SearchTextMask m;
    m.w = wFinal;
    m.h = hFinal;
    m.originX = pad;
    m.baseRow = (topSS + ascSS) / ss;
    m.adv = sz.cx / (double)ss;
    AreaScaleMask(big.data(), wSS, hSS, wFinal, hFinal, m.a);
    return &(g_searchMasks[key] = std::move(m));
}

// Shortens text with an ellipsis until it fits in `avail` pixels.
std::wstring FitSearchText(const std::wstring& s, int avail) {
    const SearchTextMask* m = GetSearchTextMask(s);
    if (!m || m->adv <= avail) return s;
    for (size_t n = s.size(); n-- > 1;) {
        std::wstring t = s.substr(0, n) + L"…";
        const SearchTextMask* mt = GetSearchTextMask(t);
        if (mt && mt->adv <= avail) return t;
    }
    return s;
}

// Vertical layout of the search text. The baseline puts the capital letters exactly on the
// bar's centre line (same as the icons); the caret is sized from the cap height, so it
// follows the text size instead of the font's (much taller) line box.
struct SearchTextGeo { int baselineY, caretTop, caretBottom; };

SearchTextGeo GetSearchTextGeo(const SearchGeo& sg) {
    double cap = g_searchCapPx;
    double mid = (sg.bar.top + sg.bar.bottom) / 2.0;
    SearchTextGeo g;
    g.baselineY = (int)std::lround(mid + cap / 2.0);
    g.caretTop = g.baselineY - (int)std::lround(cap * 1.12);
    g.caretBottom = g.baselineY + std::max(1, (int)std::lround(cap * 0.14));
    return g;
}

RECT GetSearchCaretRect(const SearchGeo& sg) {
    SearchTextGeo tg = GetSearchTextGeo(sg);
    // Empty: the caret sits exactly where the first letter will start.
    int caretX = sg.text.left;
    if (!g_query.empty()) {
        const SearchTextMask* m = GetSearchTextMask(g_query);
        double adv = m ? m->adv : 0.0;
        int avail = sg.text.right - sg.text.left;
        caretX = adv > avail ? sg.text.right + S(1) : sg.text.left + (int)std::lround(adv) + S(1);
    }
    return RECT{caretX, tg.caretTop, caretX + std::max(1, S(1)), tg.caretBottom};
}

void DrawSearchCaret(HDC dc, const SearchGeo& sg) {
    if (!g_caretVisible || g_renaming || FolderActive()) return;
    RECT caret = GetSearchCaretRect(sg);
    HBRUSH cb = CreateSolidBrush(g_isLightTheme ? RGB(20, 20, 20) : RGB(255, 255, 255));
    FillRect(dc, &caret, cb);
    DeleteObject(cb);
}

// Blends a text mask with the neutral text colour at the given opacity. `penX` is the x of
// the first letter's origin; pixels outside [clipL, clipR) are dropped, with an optional
// soft fade over `fadeL` pixels at the left clip edge.
void BlitSearchMask(HDC dc, const SearchTextMask& m, int penX, int baselineY, int clipL, int clipR,
                    int fadeL, double alpha) {
    int x0 = penX - m.originX, y0 = baselineY - m.baseRow;
    int i0 = std::max(0, clipL - x0), i1 = std::min(m.w, clipR - x0);
    if (i1 <= i0 || alpha <= 0.0) return;
    int bw = i1 - i0;
    std::vector<uint32_t> out((size_t)bw * m.h);
    const uint8_t* lut = CoverageLut();
    int tc = g_isLightTheme ? 20 : 255;
    for (int j = 0; j < m.h; j++) {
        for (int i = 0; i < bw; i++) {
            double a = lut[m.a[(size_t)j * m.w + i0 + i]] / 255.0 * alpha;
            if (fadeL > 0) {
                int dist = x0 + i0 + i - clipL;
                if (dist < fadeL) a *= (dist + 0.5) / fadeL;
            }
            uint32_t A = (uint32_t)std::lround(a * 255.0), C = (uint32_t)std::lround(tc * a);
            out[(size_t)j * bw + i] = (A << 24) | (C << 16) | (C << 8) | C;
        }
    }
    HBITMAP bmp = CreatePremultipliedDIB(bw, m.h, out.data());
    if (bmp) {
        BlendBitmap(dc, bmp, x0 + i0, y0, bw, m.h, 255);
        DeleteObject(bmp);
    }
}

void DrawSearchText(HDC dc, const SearchGeo& sg, double fade) {
    if (fade <= 0.0) return;
    SearchTextGeo tg = GetSearchTextGeo(sg);
    int availL = sg.text.left, availR = sg.text.right, avail = availR - availL;
    bool winStyle = (g_optStyle == 1);
    if (g_query.empty()) {
        // Same left edge as the caret, so the caret sits right in front of the first letter.
        const SearchTextMask* m = GetSearchTextMask(FitSearchText(GetStrings().searchApps, avail));
        if (!m) return;
        double a = g_isLightTheme ? (winStyle ? 0.60 : 0.52) : (winStyle ? 0.75 : 0.62);
        BlitSearchMask(dc, *m, availL, tg.baselineY, availL, availR, 0, a * fade);
    } else {
        const SearchTextMask* m = GetSearchTextMask(g_query);
        if (!m) return;
        bool overflow = m->adv > avail;
        int penX = overflow ? availR - (int)std::ceil(m->adv) : availL;
        BlitSearchMask(dc, *m, penX, tg.baselineY, availL, availR, overflow ? S(10) : 0, fade);
    }
}

void DrawSearchBar(HDC dc, const SearchGeo& sg, BYTE alpha = 255, bool includeCaret = true) {
    if (alpha == 0) return;
    int ph = sg.bar.bottom - sg.bar.top;
    BlendBitmap(dc, g_bmPill, sg.bar.left, sg.bar.top, sg.bar.right - sg.bar.left, ph, alpha);
    BlendBitmap(dc, g_bmMag, sg.mag.left, sg.mag.top, sg.mag.right - sg.mag.left, sg.mag.bottom - sg.mag.top, alpha);

    HBITMAP rightGlyph = !g_query.empty()
                             ? g_bmClear
                             : ((g_editHiddenMode || g_createFolderMode)
                                    ? g_bmDoneGlyph
                                    : g_bmMore);
    BlendBitmap(dc, rightGlyph, sg.rightBtn.left, sg.rightBtn.top,
                sg.rightBtn.right - sg.rightBtn.left, sg.rightBtn.bottom - sg.rightBtn.top, alpha);

    DrawSearchText(dc, sg, alpha / 255.0);
    if (includeCaret && alpha >= 140) DrawSearchCaret(dc, sg);
}

// Anti-aliased "power" symbol (open ring + vertical bar), shaded from signed
// distances so every edge is smooth at any DPI. Coordinates are in pixels of a
// sz x sz bitmap; R is the ring's centre-line radius, halfW half the stroke width,
// barTop/barBottom the bar's ends relative to the centre (negative = up).
Rgba PowerGlyphShade(double x, double y, double sz, double R, double halfW,
                     double barTop, double barBottom, Rgba col) {
    double c = sz / 2.0, gh = std::atan2(0.57, 0.82);  // half-angle of the gap at the top
    double dx = x - c, dy = y - c;
    double len = std::hypot(dx, dy);
    double t = std::atan2(dx, -dy);  // angle from 12 o'clock, clockwise
    double dRing;
    if (std::fabs(t) >= gh) {
        dRing = std::fabs(len - R);
    } else {
        double ex = R * std::sin(gh), ey = R * std::cos(gh);
        dRing = std::min(std::hypot(dx - ex, dy + ey), std::hypot(dx + ex, dy + ey));
    }
    double dBar = SdSegment(x, y, c, c + barTop, c, c + barBottom);
    double cov = std::max(Clamp01(0.5 - (dRing - halfW)), Clamp01(0.5 - (dBar - halfW)));
    if (cov <= 0.0) return Rgba{0, 0, 0, 0};
    return Rgba{col.r, col.g, col.b, cov * col.a};
}

// Anti-aliased filled circle of radius r centred on the middle pixel of a (2r+3)^2 bitmap.
HBITMAP MakeDotBitmap(int r, Rgba c) {
    int sz = 2 * r + 3;
    double rad = r + 0.5;
    return MakeAlphaBitmap(sz, sz, [=](double x, double y) {
        double a = Clamp01(0.5 - (std::hypot(x - sz / 2.0, y - sz / 2.0) - rad));
        return a > 0.0 ? Rgba{c.r, c.g, c.b, a} : Rgba{0, 0, 0, 0};
    });
}

void EnsureDotBitmaps() {
    int rOn = S(4), rOff = S(3), light = g_isLightTheme ? 1 : 0;
    int key[3] = {rOn, rOff, light};
    if (g_bmDotOn && g_bmDotOff && std::memcmp(key, g_dotKey, sizeof(key)) == 0) return;
    if (g_bmDotOn) DeleteObject(g_bmDotOn);
    if (g_bmDotOff) DeleteObject(g_bmDotOff);
    Rgba on = light ? Rgba{36 / 255.0, 42 / 255.0, 58 / 255.0, 1.0} : Rgba{248 / 255.0, 249 / 255.0, 1.0, 1.0};
    Rgba off = light ? Rgba{150 / 255.0, 156 / 255.0, 174 / 255.0, 1.0} : Rgba{130 / 255.0, 136 / 255.0, 156 / 255.0, 1.0};
    g_bmDotOn = MakeDotBitmap(rOn, on);
    g_bmDotOff = MakeDotBitmap(rOff, off);
    std::memcpy(g_dotKey, key, sizeof(key));
}

void EnsurePowerGlyphBitmap() {
    int rad = S(9), light = g_isLightTheme ? 1 : 0;
    int key[3] = {rad, g_dpi, light};
    if (g_bmPowerGlyph && std::memcmp(key, g_powerKey, sizeof(key)) == 0) return;
    if (g_bmPowerGlyph) DeleteObject(g_bmPowerGlyph);
    int sz = 2 * (rad + S(4));
    double halfW = std::max(1.0, 2.0 * g_dpi / 96.0) / 2.0;
    double barTop = -(rad + S(1)), barBottom = -S(2);
    Rgba col = light ? Rgba{36 / 255.0, 40 / 255.0, 52 / 255.0, 1.0}
                     : Rgba{238 / 255.0, 240 / 255.0, 248 / 255.0, 1.0};
    g_bmPowerGlyph = MakeAlphaBitmap(sz, sz, [=](double x, double y) {
        return PowerGlyphShade(x, y, sz, rad, halfW, barTop, barBottom, col);
    });
    g_powerGlyphSz = sz;
    std::memcpy(g_powerKey, key, sizeof(key));
}

void DrawPowerGlyph(HDC dc, const RECT& r, bool hover) {
    if (hover) {
        int w = r.right - r.left, h = r.bottom - r.top;
        Rgba fill = g_isLightTheme ? Rgba{0, 0, 0, 0.10} : Rgba{1, 1, 1, 0.16};
        Rgba bor = g_isLightTheme ? Rgba{0, 0, 0, 0.20} : Rgba{1, 1, 1, 0.25};
        HBITMAP bg = MakeRoundShape(w, h, S(12), 1.0, fill, bor);
        BlendBitmap(dc, bg, r.left, r.top, w, h);
        DeleteObject(bg);
    }
    EnsurePowerGlyphBitmap();
    if (!g_bmPowerGlyph) return;
    int cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    BlendBitmap(dc, g_bmPowerGlyph, cx - g_powerGlyphSz / 2, cy - g_powerGlyphSz / 2,
                g_powerGlyphSz, g_powerGlyphSz);
}

void DrawPageDots(HDC dc, const Layout& l, int w, int h) {
    if (l.pages < 2) return;
    EnsureDotBitmaps();
    if (!g_bmDotOn || !g_bmDotOff) return;
    int cy = DotCenterY(h), cur = (int)std::lround(g_pagePos);
    for (int i = 0; i < l.pages; i++) {
        bool active = i == cur;
        int r = S(active ? 4 : 3), cx = DotCenterX(i, l.pages, w), sz = 2 * r + 3;
        BlendBitmap(dc, active ? g_bmDotOn : g_bmDotOff, cx - (r + 1), cy - (r + 1), sz, sz);
    }
}

void CellIconPos(const Layout& l, int x, int y, int* ix, int* iy) {
    int titleH = (int)std::lround(g_fontPx * 1.55);
    int gap = (int)(l.icon * kIconTitleGapRatio);
    int block = l.icon + gap + titleH;
    *ix = x + (l.cellW - l.icon) / 2;
    *iy = y + (l.cellH - block) / 2;
}

bool OnCellContent(const Layout& l, int x, int y, int titleWidth, int px, int py) {
    int ix, iy;
    CellIconPos(l, x, y, &ix, &iy);

    int srW = 0, srH = 0;
    SelRectSize(l.cellW, l.icon, &srW, &srH);
    int rx = x + (l.cellW - srW) / 2;
    int ry = iy - S(12);
    RECT boxR = {rx, ry, rx + srW, ry + srH};
    return InRect(boxR, px, py);
}

int ItemTitleWidth(const Item& it) {
    if (it.folder >= 0 && it.folder < (int)g_layout.size()) {
        LItem& f = g_layout[it.folder];
        return MeasureTitleWidth(FolderTitle(f), &f.titleWidth, &f.titleFontPx);
    }
    if (it.app >= 0 && it.app < (int)g_apps.size()) {
        App& a = g_apps[it.app];
        return MeasureTitleWidth(a.name, &a.titleWidth, &a.titleFontPx);
    }
    return 0;
}

int HitTest(int x, int y, const Layout& l) {
    int idx = CellHitTest(x, y, l);
    if (idx < 0) return -1;
    int local = idx % l.perPage;
    int cellX = l.left + (local % l.cols) * l.cellW;
    int cellY = l.top + (local / l.cols) * l.cellH;
    int tw = ItemTitleWidth(g_view[idx]);
    if (tw <= 0) return -1;
    return OnCellContent(l, cellX, cellY, tw, x, y) ? idx : -1;
}

void DrawAppIcon(HDC dc, App& a, int x, int y, int dsz, BYTE alpha) {
    EnsureAppIcon(a, g_loadPx);
    if (!a.icon) return;
    if (dsz == a.iconPx) {
        BlendBitmap(dc, a.icon, x, y, dsz, dsz, alpha);
        return;
    }
    BITMAP bm = {};
    if (GetObject(a.icon, sizeof(bm), &bm)) {
        BlendBitmapScaled(dc, a.icon, x, y, dsz, dsz, bm.bmWidth, bm.bmHeight, alpha);
    }
}

void DrawFolderIcon(HDC dc, const LItem& f, int x, int y, int dsz, BYTE alpha) {
    if (dsz == g_loadPx && g_bmFolder) {
        BlendBitmap(dc, g_bmFolder, x, y, dsz, dsz, alpha);
    } else if (g_bmFolder) {
        BITMAP bm = {};
        if (GetObject(g_bmFolder, sizeof(bm), &bm)) {
            BlendBitmapScaled(dc, g_bmFolder, x, y, dsz, dsz, bm.bmWidth, bm.bmHeight, alpha);
        }
    }
    int baseMini = BaseMiniPxFor(g_loadPx);
    if (baseMini < 4) return;

    int pad = (int)(dsz * 0.13);
    int gapm = std::max(1, (int)(dsz * 0.035));
    int mini = (dsz - 2 * pad - 2 * gapm) / 3;
    if (mini < 4) return;

    std::vector<int> visApps = GetVisibleFolderApps(f);
    for (int i = 0; i < 9 && i < (int)visApps.size(); i++) {
        App& a = g_apps[visApps[i]];
        EnsureMini(a, g_loadPx, baseMini);
        if (!a.mini) continue;
        BYTE miniAlpha = (g_editHiddenMode && IsUserHidden(a)) ? (BYTE)(alpha * 35 / 100) : alpha;
        int mx = x + pad + (i % 3) * (mini + gapm);
        int my = y + pad + (i / 3) * (mini + gapm);
        if (mini == baseMini) BlendBitmap(dc, a.mini, mx, my, mini, mini, miniAlpha);
        else BlendBitmapScaled(dc, a.mini, mx, my, mini, mini, baseMini, baseMini, miniAlpha);
    }
}

// Area-average (box) downscale of an 8-bit mask with fractional edge weights.
void AreaScaleMask(const uint8_t* src, int sw, int sh, int dw, int dh, std::vector<uint8_t>& dst) {
    dst.assign((size_t)dw * dh, 0);
    double kx = (double)sw / dw, ky = (double)sh / dh;
    std::vector<float> tmp((size_t)dw * sh);
    for (int x = 0; x < dw; x++) {
        double x0 = x * kx, x1 = x0 + kx;
        int i0 = (int)std::floor(x0), i1 = std::min(sw, (int)std::ceil(x1));
        for (int y = 0; y < sh; y++) {
            const uint8_t* row = src + (size_t)y * sw;
            double acc = 0.0;
            for (int i = i0; i < i1; i++) {
                double wgt = std::min((double)i + 1.0, x1) - std::max((double)i, x0);
                acc += row[i] * wgt;
            }
            tmp[(size_t)y * dw + x] = (float)(acc / kx);
        }
    }
    for (int y = 0; y < dh; y++) {
        double y0 = y * ky, y1 = y0 + ky;
        int j0 = (int)std::floor(y0), j1 = std::min(sh, (int)std::ceil(y1));
        for (int x = 0; x < dw; x++) {
            double acc = 0.0;
            for (int j = j0; j < j1; j++) {
                double wgt = std::min((double)j + 1.0, y1) - std::max((double)j, y0);
                acc += tmp[(size_t)j * dw + x] * wgt;
            }
            dst[(size_t)y * dw + x] = (uint8_t)std::min(255.0, acc / ky + 0.5);
        }
    }
}

TitleMask* GetTitleMask(const std::wstring& name, int fullW, int fullH) {
    if (fullW < 1 || fullH < 1 || !g_fontMask) return nullptr;
    std::wstring key = name + L"\x1f" + std::to_wstring(g_fontPx) + L"\x1f" +
                       std::to_wstring(fullW) + L"x" + std::to_wstring(fullH);
    auto it = g_titleMasks.find(key);
    if (it != g_titleMasks.end()) return &it->second;

    int ss = g_maskSS;
    int w = fullW * ss, h = fullH * ss;
    if (g_titleMaskBytes > (size_t)48 << 20) { g_titleMasks.clear(); g_titleMaskBytes = 0; }

    EnsureScratchDC();
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(g_scratchDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) { if (dib) DeleteObject(dib); return nullptr; }
    std::memset(bits, 0, (size_t)w * h * 4);

    int saved = SaveDC(g_scratchDC);
    SelectObject(g_scratchDC, dib);
    SelectObject(g_scratchDC, g_fontMask);
    SetBkMode(g_scratchDC, TRANSPARENT);
    SetTextColor(g_scratchDC, RGB(255, 255, 255));
    RECT rc = {0, 0, w, h};
    DrawTextW(g_scratchDC, name.c_str(), -1, &rc,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    GdiFlush();
    RestoreDC(g_scratchDC, saved);

    TitleMask m;
    m.w = fullW;
    m.h = fullH;
    m.ss = ss;
    m.a.resize((size_t)w * h);
    const uint32_t* px = static_cast<const uint32_t*>(bits);
    for (size_t i = 0; i < m.a.size(); i++) {
        uint32_t p = px[i];
        m.a[i] = (uint8_t)((((p >> 16) & 255) + ((p >> 8) & 255) + (p & 255)) / 3);
    }
    DeleteObject(dib);

    m.restW = std::max(1, (int)std::lround(fullW / kHoverScale));
    m.restH = std::max(1, (int)std::lround(fullH / kHoverScale));
    AreaScaleMask(m.a.data(), w, h, m.restW, m.restH, m.rest);

    g_titleMaskBytes += m.a.size() + m.rest.size();
    return &(g_titleMasks[key] = std::move(m));
}

// fullW/fullH: size of the title box at full hover zoom (kHoverScale).
void DrawShadowedTitle(HDC dc, const std::wstring& name, RECT tr, double hover, bool ghosted,
                       int fullW, int fullH) {
    COLORREF shadowC, textC;
    if (g_isLightTheme) {
        shadowC = RGB(255, 255, 255);
        if (ghosted) textC = RGB(130, 136, 152);
        else { int c = 28 - (int)std::lround(18.0 * hover); textC = RGB(c, c + 4, c + 12); }
    } else {
        shadowC = RGB(10, 12, 20);
        if (ghosted) textC = RGB(145, 150, 170);
        else {
            int cR = 238 + (int)std::lround(17.0 * hover);
            int cG = 240 + (int)std::lround(15.0 * hover);
            int cB = 248 + (int)std::lround(7.0 * hover);
            textC = RGB(cR, cG, cB);
        }
    }

    TitleMask* m = GetTitleMask(name, fullW, fullH);
    if (m) {
        double hc = Clamp01(hover);
        std::vector<uint8_t> scaled;
        const uint8_t* sm;
        int dw, dh;
        if (hc <= 0.001) {
            dw = m->restW; dh = m->restH; sm = m->rest.data();
        } else {
            double sc = (1.0 + (kHoverScale - 1.0) * hc) / kHoverScale;
            dw = std::max(1, (int)std::lround(m->w * sc));
            dh = std::max(1, (int)std::lround(m->h * sc));
            AreaScaleMask(m->a.data(), m->w * m->ss, m->h * m->ss, dw, dh, scaled);
            sm = scaled.data();
        }

        // Coverage curve: light-on-dark text looks thin with plain linear
        // blending, so it is nudged up a little; dark-on-light stays linear.
        const uint8_t* lut = CoverageLut();

        int ow = dw + 1, oh = dh + 1;
        std::vector<uint32_t> out((size_t)ow * oh);
        int tR = GetRValue(textC), tG = GetGValue(textC), tB = GetBValue(textC);
        int sR = GetRValue(shadowC), sG = GetGValue(shadowC), sB = GetBValue(shadowC);
        for (int y = 0; y < oh; y++) {
            for (int x = 0; x < ow; x++) {
                int at = (x < dw && y < dh) ? lut[sm[(size_t)y * dw + x]] : 0;
                int as = (x >= 1 && y >= 1) ? lut[sm[(size_t)(y - 1) * dw + (x - 1)]] : 0;
                int sh = as * (255 - at) / 255;
                int a = at + sh;
                int r = (tR * at + sR * sh + 127) / 255;
                int g = (tG * at + sG * sh + 127) / 255;
                int b = (tB * at + sB * sh + 127) / 255;
                out[(size_t)y * ow + x] = ((uint32_t)a << 24) | ((uint32_t)r << 16) |
                                          ((uint32_t)g << 8) | (uint32_t)b;
            }
        }
        HBITMAP bmp = CreatePremultipliedDIB(ow, oh, out.data());
        if (bmp) {
            int left = (tr.left + tr.right) / 2 - dw / 2;
            int top = (tr.top + tr.bottom) / 2 - dh / 2;
            BlendBitmap(dc, bmp, left, top, ow, oh, 255);
            DeleteObject(bmp);
            return;
        }
    }

    // Fallback (should not happen): plain GDI text.
    int step = ClampInt((int)std::lround(Clamp01(hover) * (kHoverFontSteps - 1)),
                        0, kHoverFontSteps - 1);
    SelectObject(dc, g_fontHover[step] ? g_fontHover[step] : g_font);
    UINT flags = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
    RECT sr = {tr.left + 1, tr.top + 1, tr.right + 1, tr.bottom + 1};
    SetTextColor(dc, shadowC);
    DrawTextW(dc, name.c_str(), -1, &sr, flags);
    SetTextColor(dc, textC);
    DrawTextW(dc, name.c_str(), -1, &tr, flags);
}

// Bilinear resample of a premultiplied BGRA icon (base x base, stride base) into
// `out` (stride outStride), for the window [ox, ox+ow) x [oy, oy+oh) in cell
// coordinates, where the scaled icon occupies [left, left+base*scale) x [top, ...).
// Pixels outside the icon count as transparent, so edges stay anti-aliased.
void ScaleIconBilinear(const uint32_t* src, int base, uint32_t* out, int outStride,
                       int ox, int oy, int ow, int oh, double left, double top, double scale) {
    double inv = 1.0 / scale;
    std::vector<int> xi((size_t)ow);
    std::vector<int> xw((size_t)ow);  // weight of the right tap, 0..256
    for (int i = 0; i < ow; i++) {
        double fx = (ox + i + 0.5 - left) * inv - 0.5;
        double fl = std::floor(fx);
        xi[i] = (int)fl;
        xw[i] = (int)std::lround((fx - fl) * 256.0);
    }
    auto texel = [&](int x, int y) -> uint32_t {
        if (x < 0 || y < 0 || x >= base || y >= base) return 0;
        return src[(size_t)y * base + x];
    };
    for (int j = 0; j < oh; j++) {
        double fy = (oy + j + 0.5 - top) * inv - 0.5;
        double fl = std::floor(fy);
        int y0 = (int)fl, wy = (int)std::lround((fy - fl) * 256.0);
        for (int i = 0; i < ow; i++) {
            int x0 = xi[i], wx = xw[i];
            uint32_t p00 = texel(x0, y0), p10 = texel(x0 + 1, y0);
            uint32_t p01 = texel(x0, y0 + 1), p11 = texel(x0 + 1, y0 + 1);
            uint32_t res = 0;
            for (int sh = 0; sh < 32; sh += 8) {
                int c00 = (p00 >> sh) & 255, c10 = (p10 >> sh) & 255;
                int c01 = (p01 >> sh) & 255, c11 = (p11 >> sh) & 255;
                int top2 = c00 * (256 - wx) + c10 * wx;
                int bot2 = c01 * (256 - wx) + c11 * wx;
                int v = (top2 * (256 - wy) + bot2 * wy + 32768) >> 16;
                res |= (uint32_t)std::min(v, 255) << sh;
            }
            // Keep premultiplied colour <= alpha after rounding.
            uint32_t a = res >> 24;
            uint32_t rr = std::min((res >> 16) & 255, a), gg = std::min((res >> 8) & 255, a),
                     bb = std::min(res & 255, a);
            out[(size_t)j * outStride + i] = (a << 24) | (rr << 16) | (gg << 8) | bb;
        }
    }
}

HBITMAP CreateTopDownDIB32(int w, int h, uint32_t** bits) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* p = nullptr;
    HBITMAP b = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &p, nullptr, 0);
    *bits = static_cast<uint32_t*>(p);
    if (b && !p) { DeleteObject(b); b = nullptr; }
    return b;
}

// Draws an icon (app or folder) at a fractional scale around the centre (cx, cy).
// drawBase(HDC) must draw the icon at baseSz x baseSz with its top-left at (0, 0).
// The icon is rendered once at its normal size and bilinearly resampled to the exact
// size of this animation frame, so the hover zoom is smooth rather than stepped.
template <class F>
void DrawIconSmooth(HDC dc, int baseSz, double cx, double cy, double scale, BYTE alpha, F drawBase) {
    if (baseSz < 2 || scale <= 0.0) return;
    if (!g_icoDC) g_icoDC = CreateCompatibleDC(nullptr);
    if (!g_icoDC) return;
    int outMax = (int)std::ceil(baseSz * std::max(scale, kIconHoverScale)) + 4;
    if (!g_icoSrc || g_icoSrcSz != baseSz) {
        if (g_icoSrc) DeleteObject(g_icoSrc);
        g_icoSrc = CreateTopDownDIB32(baseSz, baseSz, &g_icoSrcBits);
        g_icoSrcSz = g_icoSrc ? baseSz : 0;
    }
    if (!g_icoOut || g_icoOutSz < outMax) {
        if (g_icoOut) DeleteObject(g_icoOut);
        g_icoOut = CreateTopDownDIB32(outMax, outMax, &g_icoOutBits);
        g_icoOutSz = g_icoOut ? outMax : 0;
    }
    if (!g_icoSrc || !g_icoOut) return;

    std::memset(g_icoSrcBits, 0, (size_t)baseSz * baseSz * 4);
    HGDIOBJ old = SelectObject(g_icoDC, g_icoSrc);
    drawBase(g_icoDC);
    GdiFlush();
    SelectObject(g_icoDC, old);

    double curSz = baseSz * scale;
    double left = cx - curSz / 2.0, top = cy - curSz / 2.0;
    int ox = (int)std::floor(left) - 1, oy = (int)std::floor(top) - 1;
    int ow = std::min((int)std::ceil(curSz) + 3, g_icoOutSz);
    ScaleIconBilinear(g_icoSrcBits, baseSz, g_icoOutBits, g_icoOutSz, ox, oy, ow, ow, left, top, scale);
    BlendBitmap(dc, g_icoOut, ox, oy, ow, ow, alpha);
}

void DrawCell(HDC dc, const Layout& l, int x, int y, const Item& it,
              double hover, bool mergeTarget = false, BYTE alpha = 255) {
    if (alpha == 0) return;
    std::wstring name;
    bool ghosted = false;
    if (it.folder >= 0 && it.folder < (int)g_layout.size()) {
        name = FolderTitle(g_layout[it.folder]);
    } else if (it.app >= 0 && it.app < (int)g_apps.size()) {
        name = g_apps[it.app].name;
        if (g_editHiddenMode && IsUserHidden(g_apps[it.app])) ghosted = true;
    } else {
        return;
    }

    int dsz = l.icon;
    int titleH = (int)std::lround(g_fontPx * 1.55);
    int gap = (int)(dsz * kIconTitleGapRatio);
    int ix, iy;
    CellIconPos(l, x, y, &ix, &iy);

    double h = mergeTarget ? 1.0 : Clamp01(hover);

    if (mergeTarget) {
        int pad = SelPad(dsz);
        BlendBitmap(dc, g_bmSel, ix - pad, iy - pad, dsz + 2 * pad, dsz + 2 * pad, alpha);
    } else if (h > 0.005 && g_bmSelRect) {
        int srW = 0, srH = 0;
        SelRectSize(l.cellW, g_loadPx, &srW, &srH);
        int baseW = (dsz == g_loadPx) ? srW : (srW * dsz / std::max(1, g_loadPx));
        int baseH = (dsz == g_loadPx) ? srH : (srH * dsz / std::max(1, g_loadPx));
        int baseX = x + (l.cellW - baseW) / 2;
        int baseY = iy - S(12) * dsz / std::max(1, g_loadPx);

        double rectScale = 0.92 + 0.08 * h;
        int curW = std::max(1, (int)std::lround(baseW * rectScale));
        int curH = std::max(1, (int)std::lround(baseH * rectScale));
        int rx = baseX + (baseW - curW) / 2;
        int ry = baseY + (baseH - curH) / 2;
        BYTE rectAlpha = (BYTE)std::lround(alpha * h);
        if (curW == srW && curH == srH) {
            BlendBitmap(dc, g_bmSelRect, rx, ry, srW, srH, rectAlpha);
        } else {
            BlendBitmapScaled(dc, g_bmSelRect, rx, ry, curW, curH, srW, srH, rectAlpha);
        }
    }

    // Hovered/selected: the icon grows smoothly (fractional scale) and lifts a little.
    bool smooth = !mergeTarget && h > 0.001;
    double iconScale = smooth ? 1.0 + (kIconHoverScale - 1.0) * h : 1.0;
    double iconCx = ix + dsz / 2.0;
    double iconCy = iy + dsz / 2.0 - (smooth ? S(2) * h : 0.0);
    int drawSz = dsz, drawX = ix, drawY = iy;
    if (mergeTarget) {
        drawSz = (int)std::lround(dsz * 1.10);
        drawX = ix - (drawSz - dsz) / 2;
        drawY = iy - (drawSz - dsz) / 2;
    } else if (smooth) {
        drawSz = (int)std::lround(dsz * iconScale);
        drawX = (int)std::lround(iconCx - drawSz / 2.0);
        drawY = (int)std::lround(iconCy - drawSz / 2.0);
    }

    BYTE iconAlpha = ghosted ? (BYTE)((int)alpha * 35 / 100) : alpha;

    if (it.folder >= 0) {
        if (smooth) {
            const LItem& fl = g_layout[it.folder];
            DrawIconSmooth(dc, dsz, iconCx, iconCy, iconScale, iconAlpha,
                           [&](HDC d) { DrawFolderIcon(d, fl, 0, 0, dsz, 255); });
        } else {
            DrawFolderIcon(dc, g_layout[it.folder], drawX, drawY, drawSz, iconAlpha);
        }
    } else {
        if (smooth) {
            App& ap = g_apps[it.app];
            DrawIconSmooth(dc, dsz, iconCx, iconCy, iconScale, iconAlpha,
                           [&](HDC d) { DrawAppIcon(d, ap, 0, 0, dsz, 255); });
        } else {
            DrawAppIcon(dc, g_apps[it.app], drawX, drawY, drawSz, iconAlpha);
        }
        if (g_editHiddenMode) {
            int bpx = BadgePx(g_loadPx);
            HBITMAP badge = ghosted ? g_bmHidBadge : g_bmVisBadge;
            BlendBitmap(dc, badge, drawX + drawSz - bpx * 3 / 4,
                        drawY - bpx / 4, bpx, bpx, alpha);
        } else if (g_createFolderMode) {
            int bpx = BadgePx(g_loadPx);
            bool picked = IsInFolderBuild(g_apps[it.app].id);
            HBITMAP badge = picked ? g_bmVisBadge : g_bmUncheckedBadge;
            BlendBitmap(dc, badge, drawX + drawSz - bpx * 3 / 4,
                        drawY - bpx / 4, bpx, bpx, alpha);
        }
    }

    if (alpha > 140) {
        double textScale = 1.0 + (kHoverScale - 1.0) * h;
        int maxTw = (int)std::lround(MaxTitleWidth(l.cellW) * textScale);
        int cx = x + l.cellW / 2;
        RECT tr = {cx - maxTw / 2, iy + dsz + gap, cx + (maxTw + 1) / 2,
                   iy + dsz + gap + titleH};
        int fullW = (int)std::lround(MaxTitleWidth(l.cellW) * kHoverScale);
        int fullH = (int)std::lround(titleH * kHoverScale);
        DrawShadowedTitle(dc, name, tr, h, ghosted, fullW, fullH);
    }
}

FolderGeo GetFolderGeo(int w, int h) {
    FolderGeo g = {};
    Layout l = GetLayout(w, h, 0);
    int n = 0;
    if (g_openFolder >= 0 && g_openFolder < (int)g_layout.size()) {
        n = (int)GetVisibleFolderApps(g_layout[g_openFolder]).size();
    }
    int maxCols = std::min(7, GridCols());
    int cols = std::min(maxCols, std::max(4, (int)std::ceil(std::sqrt(n * 1.8))));
    int rows = std::min(5, std::max(1, (n + cols - 1) / cols));
    g.cols = cols;
    g.rows = rows;
    g.count = std::min(n, cols * rows);
    g.cellW = l.cellW;
    g.cellH = l.cellH;

    int padX = (int)(l.cellW * 0.36), padY = (int)(l.cellH * 0.22);
    int titleH = (int)(h * 0.055), titleGap = S(12);
    int pw = cols * l.cellW + 2 * padX, ph = rows * l.cellH + 2 * padY;
    int left = w / 2 - pw / 2, top = (int)(h * 0.52) - ph / 2;

    g.panel = RECT{left, top, left + pw, top + ph};
    g.title = RECT{left, top - titleGap - titleH, left + pw, top - titleGap};
    g.left = left + padX;
    g.top = top + padY;
    return g;
}

int FolderCellAt(int x, int y) {
    FolderGeo g = GetFolderGeo(g_w, g_h);
    if (x < g.left || y < g.top) return -1;
    int col = (x - g.left) / g.cellW, row = (y - g.top) / g.cellH;
    if (col >= g.cols || row >= g.rows) return -1;
    int idx = row * g.cols + col;
    return idx < g.count ? idx : -1;
}

int FolderHitTest(int x, int y) {
    if (!FolderActive()) return -1;
    int idx = FolderCellAt(x, y);
    if (idx < 0 || g_openFolder < 0 || g_openFolder >= (int)g_layout.size()) return -1;
    std::vector<int> visApps = GetVisibleFolderApps(g_layout[g_openFolder]);
    if (idx >= (int)visApps.size()) return -1;
    App& a = g_apps[visApps[idx]];
    FolderGeo g = GetFolderGeo(g_w, g_h);
    Layout l = GetLayout(g_w, g_h, 0);
    int col = idx % g.cols, row = idx / g.cols;
    int tw = MeasureTitleWidth(a.name, &a.titleWidth, &a.titleFontPx);
    return OnCellContent(l, g.left + col * g.cellW, g.top + row * g.cellH, tw, x, y) ? idx : -1;
}

void OpenFolder(int li) {
    if (li < 0 || li >= (int)g_layout.size() || !g_layout[li].folder) return;

    Layout l = GetLayout(g_w, g_h, g_view.size());
    std::vector<int> visApps = GetVisibleFolderApps(g_layout[li]);
    for (int ai : visApps) EnsureAppIcon(g_apps[ai], l.icon);

    g_openFolder = li;
    g_renaming = false;
    g_folderSel = -1;
    g_folderHover.assign(visApps.size(), 0.0);
    g_sel = -1;
    g_folderVis = g_animEnabled ? 0.0 : 1.0;
    StartHoverAnim();
    StartFolderAnim(1);
}

void StartRename() {
    if (g_openFolder < 0 || g_openFolder >= (int)g_layout.size()) return;
    LItem& f = g_layout[g_openFolder];
    g_renameBackup = f.name;
    if (f.name == L"Folder" || f.name == GetStrings().defaultFolder) f.name.clear();
    g_renaming = true;
    g_caretVisible = true;
    Redraw();
}

void CommitRename() {
    if (!g_renaming) return;
    g_renaming = false;
    if (g_openFolder >= 0 && g_openFolder < (int)g_layout.size()) {
        LItem& f = g_layout[g_openFolder];
        while (!f.name.empty() && f.name.back() == L' ') f.name.pop_back();
        size_t s = 0;
        while (s < f.name.size() && f.name[s] == L' ') s++;
        f.name.erase(0, s);
        if (f.name.empty()) f.name = L"Folder";
        f.titleWidth = -1;
        SaveLayout();
    }
    Redraw();
}

void CancelRename() {
    if (!g_renaming) return;
    g_renaming = false;
    if (g_openFolder >= 0 && g_openFolder < (int)g_layout.size()) {
        g_layout[g_openFolder].name = g_renameBackup;
        g_layout[g_openFolder].titleWidth = -1;
    }
    Redraw();
}

bool ActivateItem(int viewIdx, bool asAdmin = false) {
    if (viewIdx < 0 || viewIdx >= (int)g_view.size()) return false;
    const Item it = g_view[viewIdx];
    if (it.folder >= 0) {
        OpenFolder(it.folder);
        return false;
    }
    if (g_editHiddenMode) {
        ToggleEditHiddenApp(it.app);
        return false;
    }
    if (g_createFolderMode) {
        ToggleFolderBuildApp(it.app);
        return false;
    }
    LaunchApp(it.app, asAdmin);
    return true;
}

void ResetLayout() {
    std::wstring path = LayoutPath(false);
    if (!path.empty()) DeleteFileW(path.c_str());
    g_layout.clear();
    g_hiddenIds.clear();
    g_editHiddenMode = false;
    g_createFolderMode = false;
    g_folderBuildIds.clear();
    ReconcileLayout();
    CloseFolder(true);
    g_renaming = false;
    SaveLayout();
    RebuildView();
    g_sel = -1;
    GoToPage(0);
    Layout l = GetLayout(g_w, g_h, g_view.size());
    PrewarmPage(l, 0);
    Redraw();
    if (g_wnd) UpdateWindow(g_wnd);
}

void TakeOutOfFolder(int fi, int visPos) {
    if (fi < 0 || fi >= (int)g_layout.size() || !g_layout[fi].folder) return;
    std::vector<int> visApps = GetVisibleFolderApps(g_layout[fi]);
    if (visPos < 0 || visPos >= (int)visApps.size()) return;
    std::wstring id = g_apps[visApps[visPos]].id;

    auto& ids = g_layout[fi].ids;
    ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end());

    LItem a; a.id = id;
    g_layout.insert(g_layout.begin() + fi + 1, a);
    if (GetVisibleFolderApps(g_layout[fi]).size() < 2) DissolveFolderAt(fi);

    CloseFolder(true);
    g_renaming = false;
    SaveLayout();
    RebuildView();
    g_sel = -1;
    Layout l = GetLayout(g_w, g_h, g_view.size());
    GoToPage((fi + 1) / l.perPage);
    Redraw();
}

void UpdateDropTarget(int x, int y) {
    g_dropMode = 0;
    g_dropPos = -1;
    if (!g_picked) return;

    if (g_pickKind == 1) {
        FolderGeo fg = GetFolderGeo(g_w, g_h);
        if (InRect(fg.panel, x, y)) {
            int hit = FolderCellAt(x, y);
            if (hit >= 0 && hit != g_pickIdx) {
                g_dropMode = 1;
                g_dropPos = hit;
            }
        }
        return;
    }

    Layout l = GetLayout(g_w, g_h, g_view.size());
    if (!PageSettled()) return;
    int n = (int)g_view.size();
    if (x < l.left || y < l.top) return;
    int col = (x - l.left) / l.cellW, row = (y - l.top) / l.cellH;
    if (col >= l.cols || row >= l.rowsVisible) return;
    int idx = g_page * l.perPage + row * l.cols + col;

    if (idx >= n) { g_dropMode = 1; g_dropPos = n; return; }
    if (idx == g_pickIdx) return;

    int cellX = l.left + col * l.cellW, cellY = l.top + row * l.cellH;
    int ix, iy;
    CellIconPos(l, cellX, cellY, &ix, &iy);
    int shrink = (int)(l.icon * 0.15);
    RECT mergeZone = {ix + shrink, iy + shrink, ix + l.icon - shrink, iy + l.icon - shrink};
    bool srcIsApp = g_view[g_pickIdx].app >= 0;
    if (srcIsApp && InRect(mergeZone, x, y)) {
        g_dropMode = 2;
        g_dropPos = idx;
        return;
    }
    g_dropMode = 1;
    g_dropPos = idx + (x >= cellX + l.cellW / 2 ? 1 : 0);
}

int LayoutIndexFromView(int viewIdx) {
    if (viewIdx < 0 || viewIdx >= (int)g_view.size()) return -1;
    const Item& it = g_view[viewIdx];
    if (it.folder >= 0) return it.folder;
    if (it.app >= 0 && it.app < (int)g_apps.size()) {
        const std::wstring& id = g_apps[it.app].id;
        for (size_t i = 0; i < g_layout.size(); i++) {
            if (!g_layout[i].folder && g_layout[i].id == id) return (int)i;
        }
    }
    return -1;
}

void ApplyGridDrop() {
    if (!g_query.empty()) return;
    int src = LayoutIndexFromView(g_pickIdx);
    int n = (int)g_layout.size();
    if (src < 0 || src >= n) return;

    if (g_dropMode == 2) {
        int t = LayoutIndexFromView(g_dropPos);
        if (t < 0 || t >= n || t == src || g_layout[src].folder) return;
        LItem a = g_layout[src];
        int newFolder = -1;
        if (g_layout[t].folder) {
            g_layout[t].ids.push_back(a.id);
        } else {
            LItem f; f.folder = true; f.name = L"Folder";
            f.ids.push_back(g_layout[t].id);
            f.ids.push_back(a.id);
            g_layout[t] = f;
            newFolder = t;
        }
        g_layout.erase(g_layout.begin() + src);
        if (newFolder > src) newFolder--;
        SaveLayout();
        RebuildView();
        g_sel = -1;
        if (newFolder >= 0) { OpenFolder(newFolder); StartRename(); }
    } else if (g_dropMode == 1) {
        int pos = (g_dropPos >= (int)g_view.size()) ? n : LayoutIndexFromView(g_dropPos);
        if (pos < 0 || pos > n || pos == src || pos == src + 1) return;
        LItem moved = g_layout[src];
        g_layout.erase(g_layout.begin() + src);
        if (pos > src) pos--;
        g_layout.insert(g_layout.begin() + pos, moved);
        SaveLayout();
        RebuildView();
        g_sel = -1;
    }
}

void ApplyFolderDrop(int x, int y) {
    int fi = g_openFolder;
    if (fi < 0 || fi >= (int)g_layout.size() || !g_layout[fi].folder) return;
    std::vector<int> visApps = GetVisibleFolderApps(g_layout[fi]);
    int src = g_pickIdx;
    if (src < 0 || src >= (int)visApps.size()) return;

    FolderGeo fg = GetFolderGeo(g_w, g_h);
    if (!InRect(fg.panel, x, y)) {
        TakeOutOfFolder(fi, src);
        return;
    }
    if (g_dropMode == 1 && g_dropPos >= 0 && g_dropPos < (int)visApps.size() && g_dropPos != src) {
        std::wstring srcId = g_apps[visApps[src]].id;
        std::wstring dstId = g_apps[visApps[g_dropPos]].id;
        auto& ids = g_layout[fi].ids;
        ids.erase(std::remove(ids.begin(), ids.end(), srcId), ids.end());
        auto it = std::find(ids.begin(), ids.end(), dstId);
        ids.insert(it, srcId);
        SaveLayout();
    }
}

void DrawFolderOverlay(HDC dc, const Layout& l) {
    if (g_openFolder < 0 || g_openFolder >= (int)g_layout.size() || !g_layout[g_openFolder].folder) return;
    double v = Eased(Clamp01(g_folderVis));
    if (v <= 0.001) return;

    const LItem& f = g_layout[g_openFolder];
    std::vector<int> visApps = GetVisibleFolderApps(f);
    FolderGeo fg = GetFolderGeo(g_w, g_h);
    int pw = fg.panel.right - fg.panel.left, ph = fg.panel.bottom - fg.panel.top;
    int panelRadius = (g_optStyle == 1) ? S(14) : S(44);
    EnsurePanelBitmap(pw, ph, panelRadius);

    double scale = 0.88 + 0.12 * v;
    int dw = (int)std::lround(pw * scale), dh = (int)std::lround(ph * scale);
    double targetCx = (fg.panel.left + fg.panel.right) * 0.5;
    double targetCy = (fg.panel.top + fg.panel.bottom) * 0.5;
    int dx = (int)std::lround(targetCx - dw * 0.5), dy = (int)std::lround(targetCy - dh * 0.5);
    BYTE panelAlpha = (BYTE)std::lround(255.0 * v);

    if (dw == pw && dh == ph) BlendBitmap(dc, g_bmPanel, dx, dy, pw, ph, panelAlpha);
    else BlendBitmapScaled(dc, g_bmPanel, dx, dy, dw, dh, pw, ph, panelAlpha);

    if (v > 0.25) {
        std::wstring title = g_renaming ? f.name : FolderTitle(f);
        SelectObject(dc, g_fontFolderTitle);
        RECT tr = {fg.title.left, dy - S(12) - (fg.title.bottom - fg.title.top),
                   fg.title.right, dy - S(12)};
        UINT tflags = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
        RECT sr = {tr.left + 1, tr.top + 1, tr.right + 1, tr.bottom + 1};
        SetTextColor(dc, g_isLightTheme ? RGB(255, 255, 255) : RGB(10, 12, 20));
        DrawTextW(dc, title.c_str(), -1, &sr, tflags);
        SetTextColor(dc, g_isLightTheme ? RGB(20, 24, 34) : RGB(248, 249, 255));
        DrawTextW(dc, title.c_str(), -1, &tr, tflags);

        if (g_renaming && v > 0.9) {
            SIZE sz = {};
            GetTextExtentPoint32W(dc, title.c_str(), (int)title.size(), &sz);
            TEXTMETRICW tm = {};
            GetTextMetricsW(dc, &tm);
            int midX = (tr.left + tr.right) / 2, midY = (tr.top + tr.bottom) / 2;
            int halfW = std::max((int)(sz.cx / 2), S(30));
            HBRUSH br = CreateSolidBrush(g_isLightTheme ? RGB(30, 34, 46) : RGB(235, 238, 250));
            if (g_caretVisible) {
                RECT caret = {midX + (int)(sz.cx / 2) + S(2), midY - tm.tmHeight / 2,
                              midX + (int)(sz.cx / 2) + S(2) + std::max(1, S(1)), midY + tm.tmHeight / 2};
                FillRect(dc, &caret, br);
            }
            RECT ul = {midX - halfW - S(10), midY + tm.tmHeight / 2 + S(3),
                       midX + halfW + S(10), midY + tm.tmHeight / 2 + S(3) + std::max(1, S(1))};
            FillRect(dc, &ul, br);
            DeleteObject(br);
        }
    }

    Layout fl = l;
    fl.icon = std::max(16, (int)std::lround(l.icon * scale));
    for (int i = 0; i < fg.count && i < (int)visApps.size(); i++) {
        if (g_picked && g_pickKind == 1 && i == g_pickIdx) continue;
        Item item; item.app = visApps[i];
        double baseCx = fg.left + (i % fg.cols + 0.5) * fg.cellW;
        double baseCy = fg.top + (i / fg.cols + 0.5) * fg.cellH;
        int cellX = (int)std::lround(targetCx + (baseCx - targetCx) * scale - fg.cellW * 0.5);
        int cellY = (int)std::lround(targetCy + (baseCy - targetCy) * scale - fg.cellH * 0.5);
        double hov = (i < (int)g_folderHover.size()) ? g_folderHover[i] : 0.0;
        if (g_picked && g_pickKind == 1 && g_dropMode == 1 && i == g_dropPos) hov = 1.0;
        DrawCell(dc, fl, cellX, cellY, item, hov, false, panelAlpha);
    }
}

void DrawInsertIndicator(HDC dc, const Layout& l) {
    if (!g_picked || g_pickKind != 0 || g_dropMode != 1) return;
    int n = (int)g_view.size(), pos = g_dropPos, cellIdx;
    bool rightEdge;
    if (pos < n && pos / l.perPage == g_page) { cellIdx = pos; rightEdge = false; }
    else if (pos > 0 && (pos - 1) / l.perPage == g_page) { cellIdx = pos - 1; rightEdge = true; }
    else return;
    int local = cellIdx % l.perPage;
    int cellX = l.left + (local % l.cols) * l.cellW, cellY = l.top + (local / l.cols) * l.cellH;
    int ix, iy;
    CellIconPos(l, cellX, cellY, &ix, &iy);
    int bw = std::max(2, S(3));
    int x = cellX + (rightEdge ? l.cellW : 0) - bw / 2;
    RECT r = {x, iy, x + bw, iy + l.icon};
    HBRUSH br = CreateSolidBrush(g_isLightTheme ? RGB(40, 46, 62) : RGB(235, 238, 250));
    FillRect(dc, &r, br);
    DeleteObject(br);
}

void DrawCarried(HDC dc, const Layout& l) {
    if (!g_picked) return;
    Item it;
    if (g_pickKind == 1) {
        if (g_openFolder < 0 || g_openFolder >= (int)g_layout.size()) return;
        std::vector<int> visApps = GetVisibleFolderApps(g_layout[g_openFolder]);
        if (g_pickIdx < 0 || g_pickIdx >= (int)visApps.size()) return;
        it.app = visApps[g_pickIdx];
    } else {
        if (g_pickIdx < 0 || g_pickIdx >= (int)g_view.size()) return;
        it = g_view[g_pickIdx];
    }
    int dsz = (int)std::lround(l.icon * 1.15);
    int x = g_dragCx - dsz / 2, y = g_dragCy - dsz / 2;
    if (it.folder >= 0 && it.folder < (int)g_layout.size()) DrawFolderIcon(dc, g_layout[it.folder], x, y, dsz, 225);
    else if (it.app >= 0 && it.app < (int)g_apps.size()) DrawAppIcon(dc, g_apps[it.app], x, y, dsz, 225);
}

// The system's UAC shield as a 32-bit premultiplied bitmap for a menu item.
HBITMAP GetAdminShieldBitmap() {
    if (g_bmAdminShield) return g_bmAdminShield;
    SHSTOCKICONINFO sii = {};
    sii.cbSize = sizeof(sii);
    if (FAILED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_ICON | SHGSI_SMALLICON, &sii)) || !sii.hIcon) {
        return nullptr;
    }
    ICONINFO ii = {};
    if (GetIconInfo(sii.hIcon, &ii)) {
        BITMAP bm = {};
        if (ii.hbmColor && GetObject(ii.hbmColor, sizeof(bm), &bm) && bm.bmWidth > 0 && bm.bmHeight > 0) {
            int w = bm.bmWidth, h = bm.bmHeight;
            BITMAPINFO bi = {};
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = w;
            bi.bmiHeader.biHeight = -h;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            std::vector<uint32_t> col((size_t)w * h), msk((size_t)w * h);
            HDC scr = GetDC(nullptr);
            bool ok = GetDIBits(scr, ii.hbmColor, 0, h, col.data(), &bi, DIB_RGB_COLORS) == h;
            if (ok && ii.hbmMask) {
                // The mask of a colour icon is 1bpp, so it comes back as 0 or 0x00FFFFFF per pixel.
                BITMAP mb = {};
                GetObject(ii.hbmMask, sizeof(mb), &mb);
                if (mb.bmWidth == w && mb.bmHeight >= h) {
                    ok = GetDIBits(scr, ii.hbmMask, 0, h, msk.data(), &bi, DIB_RGB_COLORS) == h;
                } else {
                    std::fill(msk.begin(), msk.end(), 0u);
                }
            }
            ReleaseDC(nullptr, scr);
            if (ok) {
                bool hasAlpha = false;
                for (uint32_t px : col) { if (px >> 24) { hasAlpha = true; break; } }
                for (size_t i = 0; i < col.size(); i++) {
                    uint32_t px = col[i];
                    uint32_t a = hasAlpha ? (px >> 24) : ((msk[i] & 0x00FFFFFF) ? 0u : 255u);
                    uint32_t r = ((px >> 16) & 255) * a / 255, g = ((px >> 8) & 255) * a / 255,
                             b = (px & 255) * a / 255;
                    col[i] = (a << 24) | (r << 16) | (g << 8) | b;
                }
                g_bmAdminShield = CreatePremultipliedDIB(w, h, col.data());
            }
        }
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
        if (ii.hbmMask) DeleteObject(ii.hbmMask);
    }
    DestroyIcon(sii.hIcon);
    return g_bmAdminShield;
}

// After a pop-up menu closes, put the hover/selection back to whatever is under the cursor.
void RefreshHoverFromCursor(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || g_target != 1) return;
    POINT pt = {};
    GetCursorPos(&pt);
    ScreenToClient(hwnd, &pt);
    RECT rc = {};
    GetClientRect(hwnd, &rc);
    bool inside = PtInRect(&rc, pt) != FALSE;
    g_lastMouseX = pt.x;
    g_lastMouseY = pt.y;
    if (inside && !g_trackingMouse) {
        TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
        g_trackingMouse = TrackMouseEvent(&tme) != FALSE;
    }
    if (FolderActive()) {
        int fh = inside ? FolderHitTest(pt.x, pt.y) : -1;
        if (fh != g_folderSel) { g_folderSel = fh; StartHoverAnim(); }
        return;
    }
    Layout l = GetLayout(g_w, g_h, g_view.size());
    int idx = inside ? HitTest(pt.x, pt.y, l) : -1;
    if (idx != g_sel) { g_sel = idx; StartHoverAnim(); }
    bool ph = inside && InRect(PowerRect(g_w, g_h), pt.x, pt.y);
    if (ph != g_powerHover) {
        g_powerHover = ph;
        RedrawRect(PowerRect(g_w, g_h), false);
    }
}

int ContextMenu(HWND hwnd, int x, int y, const std::vector<std::pair<int, const wchar_t*>>& items) {
    HMENU menu = CreatePopupMenu();
    for (const auto& it : items) {
        if (it.first == 0) {
            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            continue;
        }
        AppendMenuW(menu, MF_STRING, it.first, it.second);
        if (it.second && it.second == GetStrings().runAsAdmin) {
            if (HBITMAP shield = GetAdminShieldBitmap()) {
                MENUITEMINFOW mi = {};
                mi.cbSize = sizeof(mi);
                mi.fMask = MIIM_BITMAP;
                mi.hbmpItem = shield;
                SetMenuItemInfoW(menu, it.first, FALSE, &mi);
            }
        }
    }
    POINT p = {x, y};
    ClientToScreen(hwnd, &p);
    g_menuOpen = true;
    SetForegroundWindow(hwnd);
    int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, p.x, p.y, 0, hwnd, nullptr);
    DestroyMenu(menu);
    SwallowMenuDismissClicks(hwnd);
    g_menuOpen = false;
    ForceForeground(hwnd);
    RefreshHoverFromCursor(hwnd);
    return cmd;
}

void ShowMoreOptionsMenu(HWND hwnd, const RECT& btnRect) {
    const UiStrings& str = GetStrings();
    HMENU menu = CreatePopupMenu();

    AppendMenuW(menu, MF_STRING, 12, str.createFolder);
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 11, str.editHiddenApps);
    UINT searchHidFlags = MF_STRING | (g_searchHiddenApps ? MF_CHECKED : MF_UNCHECKED);
    AppendMenuW(menu, searchHidFlags, 13, str.searchHiddenApps);
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 3, str.resetLayout);

    POINT p = {btnRect.right, btnRect.bottom + S(6)};
    ClientToScreen(hwnd, &p);
    g_menuOpen = true;
    SetForegroundWindow(hwnd);
    int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_TOPALIGN,
                             p.x, p.y, 0, hwnd, nullptr);
    DestroyMenu(menu);
    SwallowMenuDismissClicks(hwnd);

    if (cmd == 12) {
        g_menuOpen = false;
        ForceForeground(hwnd);
        g_editHiddenMode = false;
        g_createFolderMode = true;
        g_folderBuildIds.clear();
        RebuildView();
        g_sel = -1;
        Redraw();
    } else if (cmd == 11) {
        g_menuOpen = false;
        ForceForeground(hwnd);
        g_createFolderMode = false;
        g_folderBuildIds.clear();
        g_editHiddenMode = true;
        RebuildView();
        g_sel = -1;
        Redraw();
    } else if (cmd == 13) {
        g_menuOpen = false;
        ForceForeground(hwnd);
        LONG nextVal = g_searchHiddenApps ? 0 : 1;
        InterlockedExchange(&g_searchHiddenApps, nextVal);
        Wh_SetIntValue(L"searchHiddenApps", nextVal);
        if (!g_query.empty()) {
            Refilter();
            Redraw();
        }
    } else if (cmd == 3) {
        int res = MessageBoxW(hwnd, str.resetConfirmMsg, str.resetConfirmTitle,
                              MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 | MB_TOPMOST);
        SwallowMenuDismissClicks(hwnd);
        ForceForeground(hwnd);
        g_menuOpen = false;
        if (res == IDYES) ResetLayout();
    } else {
        g_menuOpen = false;
        ForceForeground(hwnd);
    }
    RefreshHoverFromCursor(hwnd);
}

void DrawBackdropRect(HDC dc, const RECT& clip) {
    int cw = clip.right - clip.left, ch = clip.bottom - clip.top;
    if (cw <= 0 || ch <= 0) return;
    if (g_backdrop) {
        EnsureScratchDC();
        HGDIOBJ old = SelectObject(g_scratchDC, g_backdrop);
        BitBlt(dc, clip.left, clip.top, cw, ch, g_scratchDC, clip.left, clip.top, SRCCOPY);
        SelectObject(g_scratchDC, old);
    } else {
        HBRUSH bg = CreateSolidBrush(RGB(g_tintR, g_tintG, g_tintB));
        FillRect(dc, &clip, bg);
        DeleteObject(bg);
    }
}

void EnsureStaticGridCache(const Layout& l, const SearchGeo& sg) {
    if (!g_gridCacheDC) return;
    if (g_gridCacheValid && g_gridCachePage == g_page) return;

    int w = g_w, h = g_h;
    RECT full = {0, 0, w, h};
    DrawBackdropRect(g_gridCacheDC, full);
    SetBkMode(g_gridCacheDC, TRANSPARENT);

    DrawSearchBar(g_gridCacheDC, sg, 255, false);

    int pg = g_page;
    if (pg >= 0 && pg < l.pages) {
        for (int row = 0; row < l.rowsVisible; row++) {
            for (int col = 0; col < l.cols; col++) {
                int idx = pg * l.perPage + row * l.cols + col;
                if (idx >= (int)g_view.size()) break;
                int x = l.left + col * l.cellW;
                int y = l.top + row * l.cellH;
                DrawCell(g_gridCacheDC, l, x, y, g_view[idx], 0.0, false, 255);
            }
        }
    }
    DrawPageDots(g_gridCacheDC, l, w, h);
    if (g_view.empty() && !g_query.empty()) {
        SelectObject(g_gridCacheDC, g_fontSearch);
        SetTextColor(g_gridCacheDC, g_isLightTheme ? RGB(96, 96, 96) : RGB(190, 190, 190));
        RECT nr = {0, l.top + S(40), w, l.top + S(100)};
        DrawTextW(g_gridCacheDC, GetStrings().noResults, -1, &nr, DT_CENTER | DT_SINGLELINE);
    }
    if (g_optPower) DrawPowerGlyph(g_gridCacheDC, PowerRect(w, h), false);

    g_gridCachePage = g_page;
    g_gridCacheValid = true;
}

bool BuildPageSnap(const Layout& l, int pg) {
    if (pg < 0 || pg >= l.pages) return false;
    if ((int)g_snap.size() != l.pages) { InvalidateSnaps(); g_snap.assign(l.pages, nullptr); }
    if (g_snap[pg]) return true;
    int gl = l.left, top = l.top, w = l.cols * l.cellW, sh = l.rowsVisible * l.cellH;
    if (sh <= 0 || w <= 0) return false;

    SearchGeo sg = GetSearchGeo(g_w, g_h);
    EnsureUiBitmaps(sg, l);
    PrewarmPage(l, pg);

    HDC ref = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(ref);
    ReleaseDC(nullptr, ref);
    if (!dc) return false;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -sh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void *bb = nullptr, *wb = nullptr;
    HBITMAP bmpB = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bb, nullptr, 0);
    HBITMAP bmpW = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &wb, nullptr, 0);
    if (!bmpB || !bmpW || !bb || !wb) {
        if (bmpB) DeleteObject(bmpB);
        if (bmpW) DeleteObject(bmpW);
        DeleteDC(dc);
        return false;
    }

    size_t n = (size_t)w * sh;
    auto renderPass = [&](HBITMAP bmp, void* bits, uint32_t fill) {
        HGDIOBJ old = SelectObject(dc, bmp);
        auto* px = static_cast<uint32_t*>(bits);
        std::fill(px, px + n, fill);
        SetBkMode(dc, TRANSPARENT);
        SetViewportOrgEx(dc, -gl, -top, nullptr);
        for (int row = 0; row < l.rowsVisible; row++) {
            for (int col = 0; col < l.cols; col++) {
                int idx = pg * l.perPage + row * l.cols + col;
                if (idx >= (int)g_view.size()) break;
                DrawCell(dc, l, l.left + col * l.cellW, l.top + row * l.cellH,
                         g_view[idx], 0.0, false, 255);
            }
        }
        GdiFlush();
        SetViewportOrgEx(dc, 0, 0, nullptr);
        SelectObject(dc, old);
    };
    renderPass(bmpB, bb, 0x00000000u);
    renderPass(bmpW, wb, 0x00FFFFFFu);

    // Recover per-pixel alpha from the same content drawn over black and over
    // white: (white - black) = 255 * (1 - alpha). Black pass is already
    // premultiplied colour.
    auto* pb = static_cast<uint32_t*>(bb);
    auto* pw = static_cast<uint32_t*>(wb);
    for (size_t i = 0; i < n; i++) {
        uint32_t b = pb[i], wv = pw[i];
        int db = (int)(wv & 255) - (int)(b & 255);
        int dg = (int)((wv >> 8) & 255) - (int)((b >> 8) & 255);
        int dr = (int)((wv >> 16) & 255) - (int)((b >> 16) & 255);
        int d = std::min(255, std::max(0, (db + dg + dr) / 3));
        uint32_t a = (uint32_t)(255 - d);
        if (a == 0) { pb[i] = 0; continue; }
        uint32_t cb = std::min<uint32_t>(b & 255, a);
        uint32_t cg = std::min<uint32_t>((b >> 8) & 255, a);
        uint32_t cr = std::min<uint32_t>((b >> 16) & 255, a);
        pb[i] = (a << 24) | (cr << 16) | (cg << 8) | cb;
    }

    DeleteObject(bmpW);
    DeleteDC(dc);
    g_snap[pg] = bmpB;

    for (int i = 0; i < (int)g_snap.size(); i++) {
        if (g_snap[i] && std::abs(i - g_page) > 3) {
            DeleteObject(g_snap[i]);
            g_snap[i] = nullptr;
        }
    }
    return true;
}

bool BlitPageSnap(HDC dc, const Layout& l, int pg, int xoff, const RECT& clip) {
    if (!BuildPageSnap(l, pg)) return false;
    int gl = l.left + xoff, gt = l.top, gw = l.cols * l.cellW, gh = l.rowsVisible * l.cellH;
    int x0 = std::max((int)clip.left, gl), x1 = std::min((int)clip.right, gl + gw);
    int y0 = std::max((int)clip.top, gt), y1 = std::min((int)clip.bottom, gt + gh);
    if (x1 <= x0 || y1 <= y0) return true;
    EnsureScratchDC();
    HGDIOBJ old = SelectObject(g_scratchDC, g_snap[pg]);
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(dc, x0, y0, x1 - x0, y1 - y0, g_scratchDC,
               x0 - gl, y0 - gt, x1 - x0, y1 - y0, bf);
    SelectObject(g_scratchDC, old);
    return true;
}

void OnSnapTimer() {
    KillTimer(g_wnd, kSnapTimer);
    if (!g_wnd || !IsWindowVisible(g_wnd) || !g_frameDC) return;
    if (g_vis < 0.999 || g_picked || g_dragging || g_pageAnimating ||
        FolderActive() || g_menuOpen) {
        SetTimer(g_wnd, kSnapTimer, 40, nullptr);
        return;
    }
    MSG m;
    if (PeekMessageW(&m, g_wnd, WM_KEYFIRST, WM_KEYLAST, PM_NOREMOVE) ||
        PeekMessageW(&m, g_wnd, WM_LP_TOGGLE, WM_LP_TOGGLE, PM_NOREMOVE)) {
        SetTimer(g_wnd, kSnapTimer, 40, nullptr);
        return;
    }
    Layout l = GetLayout(g_w, g_h, g_view.size());
    int order[7] = {g_page, g_page + 1, g_page - 1, g_page + 2, g_page - 2, g_page + 3, g_page - 3};
    bool built = false, more = false;
    for (int pg : order) {
        if (pg < 0 || pg >= l.pages) continue;
        if ((int)g_snap.size() == l.pages && g_snap[pg]) continue;
        if (built) { more = true; continue; }
        if (!BuildPageSnap(l, pg)) return;
        built = true;
    }
    if (more) SetTimer(g_wnd, kSnapTimer, 10, nullptr);
}

void RenderFrame(const RECT* clipRect = nullptr) {
    if (!g_frameDC) return;
    HDC dc = g_frameDC;
    int w = g_w, h = g_h;
    RECT clip = clipRect ? *clipRect : RECT{0, 0, w, h};
    clip.left = std::max(0L, clip.left);
    clip.top = std::max(0L, clip.top);
    clip.right = std::min((LONG)w, clip.right);
    clip.bottom = std::min((LONG)h, clip.bottom);
    if (clip.right <= clip.left || clip.bottom <= clip.top) return;

    Layout l = GetLayout(w, h, g_view.size());
    ClampPage(l);
    SearchGeo sg = GetSearchGeo(w, h);
    EnsureUiBitmaps(sg, l);

    double fv = Eased(Clamp01(g_folderVis));
    bool canUseStaticCache = PageSettled() && fv <= 0.001 && !g_picked && g_gridCacheDC != nullptr;

    if (canUseStaticCache) {
        EnsureStaticGridCache(l, sg);
        BitBlt(dc, clip.left, clip.top, clip.right - clip.left, clip.bottom - clip.top,
               g_gridCacheDC, clip.left, clip.top, SRCCOPY);
        SetBkMode(dc, TRANSPARENT);

        // Hovered cells: first wipe only their own cell rect (never the
        // neighbours), then draw them. Everything is clipped to the dirty
        // rect so the persistent frame is never half-painted.
        int pg = g_page;
        int saved = SaveDC(dc);
        IntersectClipRect(dc, clip.left, clip.top, clip.right, clip.bottom);
        for (int pass = 0; pass < 2; pass++) {
            for (int row = 0; row < l.rowsVisible; row++) {
                for (int col = 0; col < l.cols; col++) {
                    int idx = pg * l.perPage + row * l.cols + col;
                    if (idx >= (int)g_view.size()) break;
                    double hov = (idx < (int)g_viewHover.size()) ? g_viewHover[idx] : 0.0;
                    if (hov <= 0.001) continue;
                    int x = l.left + col * l.cellW;
                    int y = l.top + row * l.cellH;
                    RECT cb = GetCellHoverBounds(l, x, y);
                    RECT inter = {};
                    if (!IntersectRect(&inter, &clip, &cb)) continue;
                    if (pass == 0) {
                        RECT cellR = {x, y, x + l.cellW, y + l.cellH};
                        RECT wipe = {};
                        if (IntersectRect(&wipe, &cellR, &clip)) DrawBackdropRect(dc, wipe);
                    } else {
                        DrawCell(dc, l, x, y, g_view[idx], hov, false, 255);
                    }
                }
            }
        }
        RestoreDC(dc, saved);

        RECT inter = {};
        if (g_caretVisible && !g_renaming && IntersectRect(&inter, &clip, &sg.bar)) {
            DrawSearchCaret(dc, sg);
        }

        if (g_optPower && g_powerHover) {
            RECT pr = PowerRect(w, h);
            if (IntersectRect(&inter, &clip, &pr)) {
                DrawBackdropRect(dc, pr);
                DrawPowerGlyph(dc, pr, true);
            }
        }
        return;
    }

    if (!PageSettled() && fv <= 0.001 && !g_picked) {
        DrawBackdropRect(dc, clip);
        SetBkMode(dc, TRANSPARENT);
        RECT inter = {};
        if (IntersectRect(&inter, &clip, &sg.bar)) DrawSearchBar(dc, sg, 255, true);
        bool ok = true;
        int p0 = (int)std::floor(g_pagePos);
        for (int pg = p0; pg <= p0 + 1 && ok; pg++) {
            if (pg < 0 || pg >= l.pages) continue;
            int xoff = (int)std::lround((pg - g_pagePos) * w);
            if (xoff >= w || xoff <= -w) continue;
            ok = BlitPageSnap(dc, l, pg, xoff, clip);
        }
        if (ok) {
            DrawPageDots(dc, l, w, h);
            if (g_view.empty() && !g_query.empty()) {
                SelectObject(dc, g_fontSearch);
                SetTextColor(dc, g_isLightTheme ? RGB(96, 96, 96) : RGB(190, 190, 190));
                RECT nr = {0, l.top + S(40), w, l.top + S(100)};
                DrawTextW(dc, GetStrings().noResults, -1, &nr, DT_CENTER | DT_SINGLELINE);
            }
            if (g_optPower) DrawPowerGlyph(dc, PowerRect(w, h), g_powerHover);
            return;
        }
    }

    DrawBackdropRect(dc, clip);
    SetBkMode(dc, TRANSPARENT);

    BYTE bgAlpha = (BYTE)std::lround(255.0 * (1.0 - fv));
    if (bgAlpha > 5) {
        RECT inter = {};
        if (IntersectRect(&inter, &clip, &sg.bar)) {
            DrawSearchBar(dc, sg, bgAlpha, true);
        }

        int p0 = (int)std::floor(g_pagePos);
        for (int pg = p0; pg <= p0 + 1; pg++) {
            if (pg < 0 || pg >= l.pages) continue;
            int xoff = (int)std::lround((pg - g_pagePos) * w);
            if (xoff >= w || xoff <= -w) continue;

            for (int row = 0; row < l.rowsVisible; row++) {
                int y = l.top + row * l.cellH;
                if (y + l.cellH <= clip.top || y >= clip.bottom) continue;
                for (int col = 0; col < l.cols; col++) {
                    int idx = pg * l.perPage + row * l.cols + col;
                    if (idx >= (int)g_view.size()) break;
                    if (g_picked && g_pickKind == 0 && idx == g_pickIdx) continue;
                    int x = l.left + col * l.cellW + xoff;
                    if (x + l.cellW <= clip.left || x >= clip.right) continue;
                    bool mergeTarget = (g_picked && g_pickKind == 0 &&
                                        g_dropMode == 2 && idx == g_dropPos);
                    double hov = (idx < (int)g_viewHover.size()) ? g_viewHover[idx] : 0.0;
                    DrawCell(dc, l, x, y, g_view[idx], hov, mergeTarget, bgAlpha);
                }
            }
        }
        DrawInsertIndicator(dc, l);
        if (bgAlpha > 140) {
            DrawPageDots(dc, l, w, h);
            if (g_view.empty() && !g_query.empty()) {
                SelectObject(dc, g_fontSearch);
                SetTextColor(dc, g_isLightTheme ? RGB(96, 96, 96) : RGB(190, 190, 190));
                RECT nr = {0, l.top + S(40), w, l.top + S(100)};
                DrawTextW(dc, GetStrings().noResults, -1, &nr, DT_CENTER | DT_SINGLELINE);
            }
            if (g_optPower) DrawPowerGlyph(dc, PowerRect(w, h), g_powerHover);
        }
    }

    DrawFolderOverlay(dc, l);
    DrawCarried(dc, l);
}

void Present(HDC dc, const RECT& rcPaint) {
    if (!g_frameDC) return;
    double v = Eased(g_vis);
    if (v >= 0.999) {
        int pw = rcPaint.right - rcPaint.left;
        int ph = rcPaint.bottom - rcPaint.top;
        if (pw <= 0 || ph <= 0) return;
        BitBlt(dc, rcPaint.left, rcPaint.top, pw, ph, g_frameDC, rcPaint.left, rcPaint.top, SRCCOPY);
    } else {
        double s = 1.0 + 0.04 * (1.0 - v);
        int dw = (int)(g_w * s), dh = (int)(g_h * s);
        SetStretchBltMode(dc, COLORONCOLOR);
        StretchBlt(dc, (g_w - dw) / 2, (g_h - dh) / 2, dw, dh, g_frameDC, 0, 0,
                   g_w, g_h, SRCCOPY);
    }
}

void Paint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(hwnd, &ps);
    if (g_dirty && g_frameDC) {
        RenderFrame(&ps.rcPaint);
        g_dirty = false;
    }
    Present(dc, ps.rcPaint);
    EndPaint(hwnd, &ps);
}

void ApplyVisual() {
    double v = Eased(g_vis);
    SetLayeredWindowAttributes(g_wnd, 0, (BYTE)(v * 255.0 + 0.5), LWA_ALPHA);
    InvalidateRect(g_wnd, nullptr, FALSE);
}

void HideNow() {
    if (!g_wnd) return;
    KillTimer(g_wnd, kAnimTimer);
    KillTimer(g_wnd, kHoldTimer);
    KillTimer(g_wnd, kDragTimer);
    KillTimer(g_wnd, kCaretTimer);
    KillTimer(g_wnd, kHoverTimer);
    KillTimer(g_wnd, kFolderAnimTimer);
    KillTimer(g_wnd, kSnapTimer);
    g_pageAnimating = false;
    g_pageChain++;
    CommitRename();
    if (g_editHiddenMode) {
        g_editHiddenMode = false;
        SaveLayout();
    }
    g_createFolderMode = false;
    g_folderBuildIds.clear();
    g_picked = false;
    g_trackingMouse = false;
    g_openFolder = -1;
    g_folderVis = 0.0;
    g_folderTarget = 0;
    if (g_dragging) {
        g_dragging = false;
        ReleaseCapture();
    }
    g_vis = 0.0;
    g_target = 0;
    if (IsWindowVisible(g_wnd)) {
        ShowWindow(g_wnd, SW_HIDE);
        if (g_prewarmIdx < g_apps.size()) {
            PostMessageW(g_wnd, WM_LP_PREWARM, 0, 0);
        }
    }
    FreeBackdrop();
    FreeFrame();
}

void StartAnimation(int target) {
    g_target = target;
    g_animLastMs = NowMs();
    SetTimer(g_wnd, kAnimTimer, 8, nullptr);
}

void BeginClose() {
    if (!g_animEnabled) { HideNow(); return; }
    StartAnimation(0);
}

void ShowLaunchpad() {
    POINT pt;
    GetCursorPos(&pt);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
    const RECT r = mi.rcMonitor;
    int w = r.right - r.left, h = r.bottom - r.top;

    UpdateThemeColors();

    SetWindowPos(g_wnd, HWND_TOPMOST, r.left, r.top, w, h, SWP_NOACTIVATE);
    g_w = w;
    g_h = h;
    g_dpi = (int)QueryDpi(g_wnd);
    g_lastMouseX = pt.x - r.left;
    g_lastMouseY = pt.y - r.top;

    int iconPx = IconPxFor(w, h);
    if (g_apps.empty() || g_appsDirty || iconPx != g_loadPx) {
        LoadApps(iconPx);
    } else if (ReconcileLayout()) {
        SaveLayout();
    }
    int fontPx = TitleFontPx(h), fontBasePx = BaseTitleFontPx(h);
    int fontSearchPx = SearchFontPx(h);
    if (!g_font || fontPx != g_fontPx || fontBasePx != g_fontBasePx || fontSearchPx != g_fontSearchPx) {
        RebuildFonts(fontPx, fontBasePx, fontSearchPx);
    }

    BuildBackdrop(r.left, r.top, w, h);
    EnsureFrame(w, h);

    g_query.clear();
    g_powerHover = false;
    g_wheelAccum = 0;
    g_hwheelAccum = 0;
    g_pageAnimating = false;
    g_dragging = false;
    g_dragMoved = false;
    g_pressMoved = false;
    g_picked = false;
    g_keyNav = false;
    g_trackingMouse = false;
    g_editHiddenMode = false;
    g_createFolderMode = false;
    g_folderBuildIds.clear();
    g_openFolder = -1;
    g_folderVis = 0.0;
    g_folderTarget = 0;
    g_renaming = false;
    g_folderSel = -1;
    g_caretVisible = true;
    Refilter();

    Layout l = GetLayout(w, h, g_view.size());
    PrewarmPage(l, 0);
    RenderFrame(nullptr);
    g_dirty = false;

    BOOL anim = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &anim, 0);
    g_animEnabled = anim != FALSE && g_optAnim != 0;

    g_vis = g_animEnabled ? 0.0 : 1.0;
    g_target = 1;
    SetLayeredWindowAttributes(g_wnd, 0, g_animEnabled ? 0 : 255, LWA_ALPHA);
    ShowWindow(g_wnd, SW_SHOW);
    ForceForeground(g_wnd);
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetTimer(g_wnd, kCaretTimer, 530, nullptr);
    if (g_animEnabled) {
        StartAnimation(1);
    } else if (g_prewarmIdx < g_apps.size()) {
        PostMessageW(g_wnd, WM_LP_PREWARM, 0, 0);
    }
}

void OnAnimTimer() {
    double now = NowMs();
    double dt = std::min(now - g_animLastMs, 50.0);
    g_animLastMs = now;
    double step = dt / (g_target ? kOpenMs : kCloseMs);
    if (g_target) g_vis = std::min(1.0, g_vis + step);
    else g_vis = std::max(0.0, g_vis - step);

    ApplyVisual();
    UpdateWindow(g_wnd);
    if (g_vis == (double)g_target) {
        KillTimer(g_wnd, kAnimTimer);
        if (g_target == 0) {
            HideNow();
        } else {
            ScheduleSnapBuild();
            Layout l = GetLayout(g_w, g_h, g_view.size());
            if (l.pages > 1) PrewarmPage(l, 1);
            if (g_prewarmIdx < g_apps.size()) {
                PostMessageW(g_wnd, WM_LP_PREWARM, 0, 0);
            }
        }
    }
}

void EnsureSelVisible(const Layout& l) {
    if (g_sel < 0) return;
    int p = std::min(g_sel / l.perPage, l.pages - 1);
    if (p != g_page) {
        g_page = p;
        StartPageAnim();
    }
}

void MoveFolderSelection(WPARAM key) {
    FolderGeo fg = GetFolderGeo(g_w, g_h);
    int n = fg.count;
    if (n <= 0) return;

    g_keyNav = true;
    if (g_folderSel < 0) {
        g_folderSel = 0;
    } else {
        int cols = fg.cols, row = g_folderSel / cols;
        switch (key) {
            case VK_LEFT: g_folderSel = std::max(0, g_folderSel - 1); break;
            case VK_RIGHT: g_folderSel = std::min(n - 1, g_folderSel + 1); break;
            case VK_TAB:
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) g_folderSel = (g_folderSel + n - 1) % n;
                else g_folderSel = (g_folderSel + 1) % n;
                break;
            case VK_UP: if (row > 0) g_folderSel -= cols; break;
            case VK_DOWN:
                if (g_folderSel + cols < n) g_folderSel += cols;
                else if (row < (n - 1) / cols) g_folderSel = n - 1;
                break;
            case VK_HOME: g_folderSel = 0; break;
            case VK_END: g_folderSel = n - 1; break;
        }
    }
    StartHoverAnim();
}

void MoveSelection(WPARAM key) {
    int n = (int)g_view.size();
    if (n == 0) return;
    Layout l = GetLayout(g_w, g_h, g_view.size());

    if (key == VK_PRIOR || key == VK_NEXT) {
        GoToPage(g_page + (key == VK_NEXT ? 1 : -1));
        return;
    }

    g_keyNav = true;
    if (g_sel < 0 || g_sel >= n || (g_sel / l.perPage) != g_page) {
        g_sel = std::min(n - 1, g_page * l.perPage);
        EnsureSelVisible(l);
        StartHoverAnim();
        return;
    }

    int cols = l.cols;
    int curPage = g_page;
    int local = g_sel % l.perPage;
    int row = local / cols;
    int col = local % cols;
    int pageStart = curPage * l.perPage;
    int pageEnd = std::min(n, pageStart + l.perPage);

    switch (key) {
        case VK_LEFT:
            if (col > 0) {
                g_sel--;
            } else if (curPage > 0) {
                int prevStart = (curPage - 1) * l.perPage;
                int target = prevStart + row * cols + (cols - 1);
                g_sel = std::min(n - 1, target);
            }
            break;

        case VK_RIGHT:
            if (col < cols - 1) {
                if (g_sel + 1 < pageEnd) {
                    g_sel++;
                }
            } else if (curPage < l.pages - 1) {
                int nextStart = (curPage + 1) * l.perPage;
                int nextEnd = std::min(n, nextStart + l.perPage);
                int target = nextStart + row * cols;
                if (target >= nextEnd) {
                    int lastRowOnNext = (nextEnd - 1 - nextStart) / cols;
                    target = nextStart + lastRowOnNext * cols;
                }
                g_sel = std::min(n - 1, target);
            }
            break;

        case VK_TAB:
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) g_sel = (g_sel + n - 1) % n;
            else g_sel = (g_sel + 1) % n;
            break;

        case VK_UP:
            if (row > 0) g_sel -= cols;
            break;

        case VK_DOWN:
            if (row < l.rowsVisible - 1) {
                if (g_sel + cols < pageEnd) {
                    g_sel += cols;
                } else {
                    int lastRowInPage = (pageEnd - 1 - pageStart) / cols;
                    if (row < lastRowInPage) g_sel = pageEnd - 1;
                }
            }
            break;

        case VK_HOME: g_sel = 0; break;
        case VK_END: g_sel = n - 1; break;
    }

    EnsureSelVisible(l);
    StartHoverAnim();
}

bool EnableShutdownPrivilege() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
        return false;
    }
    TOKEN_PRIVILEGES tp = {};
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    BOOL ok = LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME,
                                    &tp.Privileges[0].Luid) &&
              AdjustTokenPrivileges(token, FALSE, &tp, 0, nullptr, nullptr) &&
              GetLastError() == ERROR_SUCCESS;
    CloseHandle(token);
    return ok != FALSE;
}

void DoSleep() {
    EnableShutdownPrivilege();
    SetSuspendState(FALSE, FALSE, FALSE);
}

void DoShutdownOrRestart(bool restart) {
    EnableShutdownPrivilege();
    DWORD flags = SHUTDOWN_INSTALL_UPDATES |
                  (restart ? SHUTDOWN_RESTART : (SHUTDOWN_POWEROFF | SHUTDOWN_HYBRID));
    DWORD reason = SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER;
    DWORD err = InitiateShutdownW(nullptr, nullptr, 0, flags, reason);
    if (err == ERROR_SHUTDOWN_USERS_LOGGED_ON) {
        const UiStrings& str = GetStrings();
        if (MessageBoxW(nullptr, str.othersSignedInMsg, restart ? str.restart : str.shutDown,
                        MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 | MB_TOPMOST) == IDYES) {
            InitiateShutdownW(nullptr, nullptr, 0, flags | SHUTDOWN_FORCE_OTHERS, reason);
        }
    } else if (err != ERROR_SUCCESS) {
        ExitWindowsEx(restart ? EWX_REBOOT : EWX_SHUTDOWN, 0);
    }
}

void ShowPowerMenu(HWND hwnd) {
    const UiStrings& str = GetStrings();
    RECT pr = PowerRect(g_w, g_h);
    POINT p = {pr.right, pr.top - S(6)};
    ClientToScreen(hwnd, &p);

    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, 1, str.lock);
    AppendMenuW(menu, MF_STRING, 2, str.signOut);
    AppendMenuW(menu, MF_STRING, 3, str.sleep);
    AppendMenuW(menu, MF_STRING, 4, str.shutDown);
    AppendMenuW(menu, MF_STRING, 5, str.restart);

    g_menuOpen = true;
    SetForegroundWindow(hwnd);
    int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_BOTTOMALIGN,
                             p.x, p.y, 0, hwnd, nullptr);
    DestroyMenu(menu);
    SwallowMenuDismissClicks(hwnd);
    g_menuOpen = false;

    if (cmd == 0) { ForceForeground(hwnd); RefreshHoverFromCursor(hwnd); return; }
    HideNow();
    switch (cmd) {
        case 1: LockWorkStation(); break;
        case 2: ExitWindowsEx(EWX_LOGOFF, 0); break;
        case 3: DoSleep(); break;
        case 4: DoShutdownOrRestart(false); break;
        case 5: DoShutdownOrRestart(true); break;
    }
}

void DeletePrevWord(std::wstring& s) {
    while (!s.empty() && s.back() == L' ') s.pop_back();
    while (!s.empty() && s.back() != L' ') s.pop_back();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_LP_TOGGLE:
            if ((!IsWindowVisible(hwnd) || g_target == 0) &&
                NowMs() - g_lastClosed < 500.0) {
                return 0;
            }
            if (!IsWindowVisible(hwnd)) {
                ShowLaunchpad();
            } else if (g_target == 1) {
                BeginClose();
            } else {
                ForceForeground(hwnd);
                StartAnimation(1);
            }
            return 0;

        case WM_LP_RELOAD:
            LoadApps(g_loadPx);
            RebuildView();
            if (IsWindowVisible(hwnd)) {
                Layout l = GetLayout(g_w, g_h, g_view.size());
                ClampPage(l);
                PrewarmPage(l, g_page);
                Redraw();
            }
            return 0;

        case WM_LP_SHELLCHANGE: {
            PIDLIST_ABSOLUTE* pidls = nullptr;
            LONG event = 0;
            HANDLE lock = SHChangeNotification_Lock(
                reinterpret_cast<HANDLE>(wp), static_cast<DWORD>(lp), &pidls,
                &event);
            bool shouldReload = false;
            constexpr LONG kStructureEvents =
                SHCNE_CREATE | SHCNE_DELETE | SHCNE_MKDIR | SHCNE_RMDIR |
                SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER | SHCNE_UPDATEDIR;
            if ((event & kStructureEvents) != 0) {
                shouldReload = true;
            } else if ((event & SHCNE_UPDATEITEM) != 0 && pidls && pidls[0]) {
                // Only treat SHCNE_UPDATEITEM as an app change if it's a real
                // filesystem shortcut (.lnk/.url) in Start Menu\Programs, not a
                // noisy virtual UWP tile/badge ping inside AppsFolder!
                WCHAR fsPath[MAX_PATH];
                if (SHGetPathFromIDListW(pidls[0], fsPath)) {
                    shouldReload = true;
                }
            }
            if (lock) {
                SHChangeNotification_Unlock(lock);
            }
            if (shouldReload) {
                InterlockedExchange(&g_appsDirty, 1);
                SetTimer(hwnd, kShellDebounceTimer, 600, nullptr);
            }
            return 0;
        }

        case WM_LP_PREWARM: {
            if (g_loadPx <= 0) return 0;
            if (IsWindowVisible(hwnd) &&
                (!PageSettled() || g_vis < 0.999 || g_dragging || FolderActive())) {
                SetTimer(hwnd, kPrewarmTimer, 100, nullptr);
                return 0;
            }
            MSG m;
            if (PeekMessageW(&m, hwnd, WM_KEYFIRST, WM_KEYLAST, PM_NOREMOVE) ||
                PeekMessageW(&m, hwnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE) ||
                PeekMessageW(&m, hwnd, WM_LP_TOGGLE, WM_LP_TOGGLE, PM_NOREMOVE)) {
                SetTimer(hwnd, kPrewarmTimer, 40, nullptr);
                return 0;
            }
            int baseMini = BaseMiniPxFor(g_loadPx);
            while (g_prewarmIdx < g_apps.size()) {
                App& a = g_apps[g_prewarmIdx++];
                if (!a.icon || a.iconPx != g_loadPx ||
                    (baseMini >= 4 && (!a.mini || a.miniPx != baseMini))) {
                    EnsureAppIcon(a, g_loadPx);
                    if (baseMini >= 4) EnsureMini(a, g_loadPx, baseMini);
                    break;
                }
            }
            if (g_prewarmIdx < g_apps.size()) PostMessageW(hwnd, WM_LP_PREWARM, 0, 0);
            return 0;
        }

        case WM_LP_SETTINGS:
            LoadSettings();
            FreeUiBitmaps();
            ReconcileLayout();
            RebuildView();
            if (IsWindowVisible(hwnd)) HideNow();
            return 0;

        case WM_LP_PAGESTEP:
            if (g_pageAnimating && lp == g_pageChain) OnPageTimer();
            return 0;

        case WM_LP_QUIT:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_TIMER:
            if (wp == kAnimTimer) OnAnimTimer();
            else if (wp == kHoverTimer) OnHoverTimer();
            else if (wp == kFolderAnimTimer) OnFolderAnimTimer();
            else if (wp == kSnapTimer) OnSnapTimer();
            else if (wp == kPrewarmTimer) {
                KillTimer(hwnd, kPrewarmTimer);
                PostMessageW(hwnd, WM_LP_PREWARM, 0, 0);
            } else if (wp == kShellDebounceTimer) {
                KillTimer(hwnd, kShellDebounceTimer);
                PostMessageW(hwnd, WM_LP_RELOAD, 0, 0);
            } else if (wp == kCaretTimer) {
                g_caretVisible = !g_caretVisible;
                if (g_renaming && FolderActive()) {
                    FolderGeo fg = GetFolderGeo(g_w, g_h);
                    RedrawRect(fg.title, false);
                } else if (!FolderActive()) {
                    SearchGeo sg = GetSearchGeo(g_w, g_h);
                    RECT cr = GetSearchCaretRect(sg);
                    InflateRect(&cr, S(2), S(2));
                    RedrawRect(cr, false);
                }
            } else if (wp == kHoldTimer) {
                KillTimer(hwnd, kHoldTimer);
                bool target = FolderActive() ? g_pressFolderIdx >= 0 : g_pressItem >= 0;
                if (g_dragging && !g_pressMoved && !g_dragMoved && target &&
                    g_query.empty() && !g_renaming && !g_editHiddenMode && !g_createFolderMode) {
                    g_picked = true;
                    g_pickKind = FolderActive() ? 1 : 0;
                    g_pickIdx = FolderActive() ? g_pressFolderIdx : g_pressItem;
                    g_sel = -1;
                    g_folderSel = -1;
                    StartHoverAnim();
                    g_dragCx = g_dragX0;
                    g_dragCy = g_dragY0;
                    g_edgeSince = 0;
                    SetTimer(hwnd, kDragTimer, 40, nullptr);
                    UpdateDropTarget(g_dragCx, g_dragCy);
                    Redraw();
                }
            } else if (wp == kDragTimer) {
                if (!g_picked) {
                    KillTimer(hwnd, kDragTimer);
                } else if (g_pickKind == 0) {
                    int zone = (int)(g_w * 0.04);
                    int dir = g_dragCx < zone ? -1 : (g_dragCx > g_w - zone ? 1 : 0);
                    DWORD now = GetTickCount();
                    bool flipped = false;
                    if (dir == 0) g_edgeSince = 0;
                    else if (g_edgeSince == 0) g_edgeSince = now;
                    else if (now - g_edgeSince > 550) {
                        int before = g_page;
                        GoToPage(g_page + dir);
                        flipped = g_page != before;
                        g_edgeSince = now;
                    }
                    int om = g_dropMode, op = g_dropPos;
                    UpdateDropTarget(g_dragCx, g_dragCy);
                    if (flipped || om != g_dropMode || op != g_dropPos) Redraw();
                }
            }
            return 0;

        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE && !g_menuOpen &&
                IsWindowVisible(hwnd) && g_target == 1) {
                g_lastClosed = NowMs();
                BeginClose();
            }
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            Paint(hwnd);
            return 0;

        case WM_KEYDOWN: {
            if (g_picked) {
                if (wp == VK_ESCAPE) {
                    g_picked = false;
                    g_dragging = false;
                    g_pickIdx = -1;
                    ReleaseCapture();
                    KillTimer(hwnd, kDragTimer);
                    UpdateDropTarget(0, 0);
                    Redraw();
                }
                return 0;
            }
            if (g_renaming) {
                if (wp == VK_RETURN) CommitRename();
                else if (wp == VK_ESCAPE) CancelRename();
                return 0;
            }
            bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

            if (FolderActive()) {
                switch (wp) {
                    case VK_ESCAPE: CloseFolder(); break;
                    case VK_F2: if (!g_editHiddenMode && !g_createFolderMode) StartRename(); break;
                    case VK_RETURN:
                        if (g_folderSel >= 0 && g_openFolder < (int)g_layout.size()) {
                            std::vector<int> visApps = GetVisibleFolderApps(g_layout[g_openFolder]);
                            if (g_folderSel < (int)visApps.size()) {
                                int ai = visApps[g_folderSel];
                                if (g_editHiddenMode) ToggleEditHiddenApp(ai);
                                else if (g_createFolderMode) ToggleFolderBuildApp(ai);
                                else { LaunchApp(ai, ctrl && shift); BeginClose(); }
                            }
                        }
                        break;
                    case VK_LEFT:
                    case VK_RIGHT:
                    case VK_UP:
                    case VK_DOWN:
                    case VK_TAB:
                    case VK_HOME:
                    case VK_END:
                        MoveFolderSelection(wp);
                        break;
                }
                return 0;
            }

            switch (wp) {
                case VK_ESCAPE:
                    if (!g_query.empty()) { g_query.clear(); Refilter(); Redraw(); }
                    else if (g_createFolderMode) FinishCreateFolderMode(false);
                    else if (g_editHiddenMode) FinishEditHiddenMode();
                    else BeginClose();
                    break;
                case VK_DELETE:
                    if (!g_query.empty()) { g_query.clear(); Refilter(); Redraw(); }
                    break;
                case VK_F2:
                    if (!g_editHiddenMode && !g_createFolderMode && g_sel >= 0 &&
                        g_sel < (int)g_view.size() && g_view[g_sel].folder >= 0) {
                        OpenFolder(g_view[g_sel].folder);
                        StartRename();
                    }
                    break;
                case VK_RETURN:
                    if (g_createFolderMode && g_sel < 0) FinishCreateFolderMode(true);
                    else if (g_editHiddenMode && g_sel < 0) FinishEditHiddenMode();
                    else if (g_sel >= 0 && ActivateItem(g_sel, ctrl && shift)) BeginClose();
                    break;
                case VK_LEFT:
                case VK_RIGHT:
                case VK_UP:
                case VK_DOWN:
                case VK_TAB:
                case VK_HOME:
                case VK_END:
                case VK_PRIOR:
                case VK_NEXT:
                    MoveSelection(wp);
                    break;
            }
            return 0;
        }

        case WM_CHAR: {
            if (g_picked) return 0;
            bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            if (g_renaming) {
                if (g_openFolder >= 0 && g_openFolder < (int)g_layout.size()) {
                    std::wstring& name = g_layout[g_openFolder].name;
                    if (wp == 8) { if (!name.empty()) name.pop_back(); }
                    else if (wp == 127 || (wp == 8 && ctrl)) DeletePrevWord(name);
                    else if (wp >= 32 && wp != 127 && name.size() < 40) name.push_back((wchar_t)wp);
                    g_layout[g_openFolder].titleWidth = -1;
                    g_caretVisible = true;
                    Redraw();
                }
                return 0;
            }
            if (FolderActive()) return 0;
            if (wp == 127 || (wp == 8 && ctrl)) {
                if (!g_query.empty()) { DeletePrevWord(g_query); g_caretVisible = true; Refilter(); Redraw(); }
            } else if (wp == 8) {
                if (!g_query.empty()) { g_query.pop_back(); g_caretVisible = true; Refilter(); Redraw(); }
            } else if (wp >= 32 && wp != 127) {
                g_query.push_back((wchar_t)wp);
                g_caretVisible = true;
                Refilter();
                Redraw();
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            if (g_dragging || FolderActive()) return 0;
            g_wheelAccum += GET_WHEEL_DELTA_WPARAM(wp);
            int steps = g_wheelAccum / WHEEL_DELTA;
            if (steps != 0) {
                g_wheelAccum -= steps * WHEEL_DELTA;
                GoToPage(g_page - steps);
            }
            return 0;
        }

        case WM_MOUSEHWHEEL: {
            if (g_dragging || FolderActive()) return 0;
            g_hwheelAccum += GET_WHEEL_DELTA_WPARAM(wp);
            int steps = g_hwheelAccum / WHEEL_DELTA;
            if (steps != 0) {
                g_hwheelAccum -= steps * WHEEL_DELTA;
                GoToPage(g_page + steps);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            Layout l = GetLayout(g_w, g_h, g_view.size());

            if (g_dragging) {
                g_lastMouseX = x;
                g_lastMouseY = y;
                if (g_picked) {
                    g_dragCx = x; g_dragCy = y;
                    UpdateDropTarget(x, y);
                    Redraw();
                    return 0;
                }
                int dx = x - g_dragX0, dy = y - g_dragY0;
                if (!g_pressMoved && (std::abs(dx) > S(10) || std::abs(dy) > S(10))) {
                    g_pressMoved = true;
                    KillTimer(hwnd, kHoldTimer);
                }
                if (!FolderActive() && !g_dragMoved && std::abs(dx) > S(10)) {
                    g_dragMoved = true;
                    g_pageAnimating = false;
                    KillTimer(hwnd, kHoverTimer);
                    g_pageVel = 0.0;
                    g_dragLastX = x;
                    g_dragLastMs = NowMs();
                    g_dragSwipeVel = 0.0;
                }
                if (g_dragMoved) {
                    double now = NowMs(), dt = now - g_dragLastMs;
                    if (dt > 1.0) {
                        double instVel = (double)(x - g_dragLastX) / dt;
                        g_dragSwipeVel = 0.6 * g_dragSwipeVel + 0.4 * instVel;
                        g_dragLastX = x;
                        g_dragLastMs = now;
                    }
                    double pos = g_dragPos0 - (double)dx / (double)g_w;
                    g_pagePos = std::min(std::max(pos, 0.0), (double)(l.pages - 1));
                    RedrawRect(GetPageSwipeRect(l), false);
                    UpdateWindow(hwnd);
                }
                return 0;
            }

            if (x == g_lastMouseX && y == g_lastMouseY) return 0;
            g_lastMouseX = x;
            g_lastMouseY = y;
            g_keyNav = false;

            if (!g_trackingMouse) {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
                g_trackingMouse = TrackMouseEvent(&tme) != FALSE;
            }

            if (FolderActive()) {
                int fh = FolderHitTest(x, y);
                if (fh != g_folderSel) { g_folderSel = fh; StartHoverAnim(); }
                return 0;
            }

            int idx = HitTest(x, y, l);
            bool ph = InRect(PowerRect(g_w, g_h), x, y);
            if (idx != g_sel) {
                g_sel = idx;
                StartHoverAnim();
            }
            if (ph != g_powerHover) {
                g_powerHover = ph;
                RedrawRect(PowerRect(g_w, g_h), false);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            g_trackingMouse = false;
            // A pop-up menu captures the mouse and triggers this; the item that was
            // right-clicked must stay selected until the menu is closed.
            if (g_menuOpen) return 0;
            if (!g_keyNav) {
                if (FolderActive() && g_folderSel >= 0) {
                    g_folderSel = -1;
                    StartHoverAnim();
                } else if (!FolderActive() && g_sel >= 0) {
                    g_sel = -1;
                    StartHoverAnim();
                }
            }
            if (g_powerHover) {
                g_powerHover = false;
                RedrawRect(PowerRect(g_w, g_h), false);
            }
            return 0;

        case WM_LBUTTONDOWN: {
            if (IsMenuDismissClick()) return 0;
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            g_dragging = true;
            g_dragMoved = false;
            g_pressMoved = false;
            g_picked = false;
            g_pressItem = -1;
            g_pressFolderIdx = -1;
            g_dragX0 = x;
            g_dragY0 = y;
            g_dragLastX = x;
            g_dragLastMs = NowMs();
            g_dragSwipeVel = 0.0;
            g_dragPos0 = g_pagePos;
            if (FolderActive()) {
                g_pressFolderIdx = FolderHitTest(x, y);
            } else if (g_query.empty() && PageSettled()) {
                Layout l = GetLayout(g_w, g_h, g_view.size());
                g_pressItem = HitTest(x, y, l);
            }
            if (!g_editHiddenMode && !g_createFolderMode &&
                (g_pressItem >= 0 || g_pressFolderIdx >= 0)) {
                SetTimer(hwnd, kHoldTimer, kHoldMs, nullptr);
            }
            SetCapture(hwnd);
            return 0;
        }

        case WM_LBUTTONUP: {
            if (IsMenuDismissClick()) {
                if (g_dragging) {
                    g_dragging = false;
                    KillTimer(hwnd, kHoldTimer);
                    ReleaseCapture();
                }
                return 0;
            }
            if (!g_dragging) return 0;
            g_dragging = false;
            KillTimer(hwnd, kHoldTimer);
            ReleaseCapture();
            Layout l = GetLayout(g_w, g_h, g_view.size());

            if (g_picked) {
                int px = GET_X_LPARAM(lp), py = GET_Y_LPARAM(lp);
                KillTimer(hwnd, kDragTimer);
                g_dragCx = px; g_dragCy = py;
                UpdateDropTarget(px, py);
                if (g_pickKind == 1) ApplyFolderDrop(px, py);
                else ApplyGridDrop();
                g_picked = false;
                g_pickIdx = -1;
                g_dropMode = 0;
                g_dropPos = -1;
                g_edgeSince = 0;
                Redraw();
                return 0;
            }

            if (g_dragMoved) {
                g_dragMoved = false;
                double delta = g_pagePos - g_dragPos0;
                int base = (int)std::lround(g_dragPos0), target = base;
                if (delta > 0.12 || g_dragSwipeVel < -0.45) target = base + 1;
                else if (delta < -0.12 || g_dragSwipeVel > 0.45) target = base - 1;
                target = std::min(std::max(target, 0), l.pages - 1);
                g_page = target;
                double idleMs = NowMs() - g_dragLastMs;
                double relVel = (idleMs < 80.0) ? -g_dragSwipeVel * 1000.0 / std::max(1, g_w) : 0.0;
                g_pageVel = std::min(std::max(relVel, -5.0), 5.0);
                StartPageAnim();
                return 0;
            }

            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);

            if (FolderActive()) {
                FolderGeo fg = GetFolderGeo(g_w, g_h);
                if (g_renaming) {
                    if (InRect(fg.title, x, y)) return 0;
                    CommitRename();
                }
                if (!g_editHiddenMode && !g_createFolderMode && InRect(fg.title, x, y)) {
                    StartRename();
                    return 0;
                }
                if (InRect(fg.panel, x, y)) {
                    int hit = FolderHitTest(x, y);
                    if (hit >= 0 && hit == g_pressFolderIdx && g_openFolder < (int)g_layout.size()) {
                        std::vector<int> visApps = GetVisibleFolderApps(g_layout[g_openFolder]);
                        if (hit < (int)visApps.size()) {
                            int ai = visApps[hit];
                            if (g_editHiddenMode) ToggleEditHiddenApp(ai);
                            else if (g_createFolderMode) ToggleFolderBuildApp(ai);
                            else { LaunchApp(ai); BeginClose(); }
                        }
                    }
                    return 0;
                }
                CloseFolder();
                return 0;
            }

            SearchGeo sg = GetSearchGeo(g_w, g_h);
            RECT rb = sg.rightBtn;
            InflateRect(&rb, S(6), S(6));
            if (InRect(rb, x, y)) {
                if (!g_query.empty()) { g_query.clear(); Refilter(); Redraw(); }
                else if (g_editHiddenMode) FinishEditHiddenMode();
                else if (g_createFolderMode) FinishCreateFolderMode(true);
                else ShowMoreOptionsMenu(hwnd, sg.rightBtn);
                return 0;
            }
            if (InRect(sg.bar, x, y)) return 0;
            if (InRect(PowerRect(g_w, g_h), x, y)) { ShowPowerMenu(hwnd); return 0; }
            int dot = DotHitTest(x, y, l, g_w, g_h);
            if (dot >= 0) { GoToPage(dot); return 0; }
            if (!PageSettled()) return 0;
            int idx = HitTest(x, y, l);
            if (idx >= 0) {
                if (ActivateItem(idx)) BeginClose();
                return 0;
            }
            if (g_editHiddenMode || g_createFolderMode) {
                return 0;
            }
            BeginClose();
            return 0;
        }

        case WM_RBUTTONUP: {
            if (IsMenuDismissClick()) return 0;
            if (g_dragging || g_picked || g_menuOpen || g_editHiddenMode || g_createFolderMode) return 0;
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            Layout l = GetLayout(g_w, g_h, g_view.size());
            if (g_renaming) CommitRename();
            const UiStrings& str = GetStrings();

            if (FolderActive()) {
                int hit = FolderHitTest(x, y);
                if (hit < 0 || g_openFolder >= (int)g_layout.size()) return 0;
                std::vector<int> visApps = GetVisibleFolderApps(g_layout[g_openFolder]);
                if (hit >= (int)visApps.size()) return 0;

                int fi = g_openFolder, ai = visApps[hit];
                // Keep the right-clicked app selected while its menu is open.
                g_folderSel = hit;
                SnapHoverStates();
                {
                    FolderGeo pg = GetFolderGeo(g_w, g_h);
                    RECT pr = pg.panel;
                    InflateRect(&pr, S(24), S(24));
                    RedrawRect(pr, false);
                    UpdateWindow(hwnd);
                }
                ShellVerbs sv;
                LoadShellVerbs(ai, sv);
                std::vector<std::pair<int, const wchar_t*>> items = {
                    {1, str.open}, {2, str.runAsAdmin}, {3, str.openFileLoc}, {0, nullptr}};
                if (sv.pinCmd >= 0) items.push_back({6, sv.pinText.c_str()});
                items.push_back({4, str.removeFromFolder});
                items.push_back({5, str.hideApp});
                if (sv.uninstallCmd >= 0) {
                    items.push_back({0, nullptr});
                    items.push_back({7, sv.uninstallText.c_str()});
                }
                int cmd = ContextMenu(hwnd, x, y, items);
                if (cmd == 1) { LaunchApp(ai, false); BeginClose(); }
                else if (cmd == 2) { LaunchApp(ai, true); BeginClose(); }
                else if (cmd == 3) { OpenAppLocation(ai); BeginClose(); }
                else if (cmd == 4) TakeOutOfFolder(fi, hit);
                else if (cmd == 5) HideSingleApp(ai);
                else if (cmd == 6) { BeginClose(); InvokeShellVerb(sv, sv.pinCmd); }
                else if (cmd == 7) { BeginClose(); InvokeShellVerb(sv, sv.uninstallCmd); }
                return 0;
            }

            int idx = PageSettled() ? HitTest(x, y, l) : -1;
            if (idx >= 0 && idx < (int)g_view.size()) {
                const Item it = g_view[idx];
                // Keep the right-clicked icon selected while its menu is open.
                g_sel = idx;
                SnapHoverStates();
                {
                    RECT cb = GetViewCellBounds(l, idx);
                    RedrawRect(cb, false);
                    UpdateWindow(hwnd);
                }
                if (it.folder >= 0) {
                    int li = it.folder;
                    int cmd = ContextMenu(hwnd, x, y,
                                          {{1, str.open}, {2, str.renameFolder},
                                           {0, nullptr}, {3, str.ungroupFolder}});
                    if (cmd == 1) OpenFolder(li);
                    else if (cmd == 2) { OpenFolder(li); StartRename(); }
                    else if (cmd == 3 && li < (int)g_layout.size()) {
                        DissolveFolderAt(li);
                        SaveLayout();
                        RebuildView();
                        g_sel = -1;
                        Redraw();
                    }
                } else if (it.app >= 0) {
                    ShellVerbs sv;
                    LoadShellVerbs(it.app, sv);
                    std::vector<std::pair<int, const wchar_t*>> items = {
                        {1, str.open}, {2, str.runAsAdmin}, {3, str.openFileLoc}, {0, nullptr}};
                    if (sv.pinCmd >= 0) items.push_back({6, sv.pinText.c_str()});
                    items.push_back({4, str.hideApp});
                    if (sv.uninstallCmd >= 0) {
                        items.push_back({0, nullptr});
                        items.push_back({7, sv.uninstallText.c_str()});
                    }
                    int cmd = ContextMenu(hwnd, x, y, items);
                    if (cmd == 1) { LaunchApp(it.app, false); BeginClose(); }
                    else if (cmd == 2) { LaunchApp(it.app, true); BeginClose(); }
                    else if (cmd == 3) { OpenAppLocation(it.app); BeginClose(); }
                    else if (cmd == 4) { HideSingleApp(it.app); }
                    else if (cmd == 6) { BeginClose(); InvokeShellVerb(sv, sv.pinCmd); }
                    else if (cmd == 7) { BeginClose(); InvokeShellVerb(sv, sv.uninstallCmd); }
                }
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            if (g_dragging) {
                g_dragging = false;
                KillTimer(hwnd, kHoldTimer);
                if (g_picked) {
                    g_picked = false;
                    g_pickIdx = -1;
                    g_dropMode = 0;
                    g_dropPos = -1;
                    KillTimer(hwnd, kDragTimer);
                    Redraw();
                }
                if (g_dragMoved) {
                    g_dragMoved = false;
                    Layout l = GetLayout(g_w, g_h, g_view.size());
                    g_page = std::min(std::max((int)std::lround(g_pagePos), 0), l.pages - 1);
                    StartPageAnim();
                }
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// Native `XamlLauncher::ShowStartView` Symbol Hook (twinui.pcshell.dll)
// Adapted from fullscreen-start by Rafael Mendes (MIT License)
// ---------------------------------------------------------------------------
constexpr int kShowMethodKeyboard = 1;
constexpr int kShowFlagKeyboard = 0x8;

bool IsKeyboardShow(int showMethod, int showFlags) {
    return showMethod == kShowMethodKeyboard && (showFlags & kShowFlagKeyboard);
}

using XamlLauncher_ShowStartView_t = HRESULT(WINAPI*)(void* pThis,
                                                      int showMethod,
                                                      int showFlags);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original = nullptr;

HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis,
                                               int showMethod,
                                               int showFlags) {
    Wh_Log(L"ShowStartView: method=%d, flags=0x%x", showMethod, showFlags);
    bool enabled = IsKeyboardShow(showMethod, showFlags)
                       ? (g_optWinKey != 0)
                       : (g_optStartBtn != 0);
    HWND wnd = g_wnd;
    if (!enabled || !wnd) {
        return XamlLauncher_ShowStartView_Original(pThis, showMethod, showFlags);
    }
    PostMessageW(wnd, WM_LP_TOGGLE, 0, 0);
    return S_OK;
}

bool HookShowStartView() {
    HMODULE module = LoadLibraryExW(L"twinui.pcshell.dll", nullptr,
                                    LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return false;
    }
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
            &XamlLauncher_ShowStartView_Original,
            XamlLauncher_ShowStartView_Hook,
        },
    };
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    return true;
}

bool IsFolderWindowsProcess() {
    std::wstring cmd = GetCommandLineW();
    if (!cmd.empty()) CharLowerBuffW(&cmd[0], (DWORD)cmd.size());
    return cmd.find(L"/factory") != std::wstring::npos ||
           cmd.find(L"-embedding") != std::wstring::npos ||
           cmd.find(L"/separate") != std::wstring::npos;
}

void RegisterShellAppWatcher(HWND hwnd) {
    PIDLIST_ABSOLUTE pApps = nullptr, pProg = nullptr, pCommonProg = nullptr;
    SHGetKnownFolderIDList(FOLDERID_AppsFolder, 0, nullptr, &pApps);
    SHGetKnownFolderIDList(FOLDERID_Programs, 0, nullptr, &pProg);
    SHGetKnownFolderIDList(FOLDERID_CommonPrograms, 0, nullptr, &pCommonProg);

    SHChangeNotifyEntry entries[3] = {};
    int cnt = 0;
    if (pApps) entries[cnt++] = {pApps, TRUE};
    if (pProg) entries[cnt++] = {pProg, TRUE};
    if (pCommonProg) entries[cnt++] = {pCommonProg, TRUE};

    if (cnt > 0) {
        constexpr LONG kWatchEvents =
            SHCNE_CREATE | SHCNE_DELETE | SHCNE_MKDIR | SHCNE_RMDIR |
            SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER | SHCNE_UPDATEDIR |
            SHCNE_UPDATEITEM;
        g_shellNotifyId = SHChangeNotifyRegister(
            hwnd,
            SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
            kWatchEvents, WM_LP_SHELLCHANGE, cnt, entries);
    }
    if (pApps) CoTaskMemFree(pApps);
    if (pProg) CoTaskMemFree(pProg);
    if (pCommonProg) CoTaskMemFree(pCommonProg);
}

DWORD WINAPI ThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    HINSTANCE inst = GetModuleHandleW(nullptr);
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kClassName;
    if (!RegisterClassW(&wc)) {
        CoUninitialize();
        SetEvent(g_ready);
        return 0;
    }

    g_wnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
                            kClassName, L"StartPad", WS_POPUP, 0, 0, 0, 0,
                            nullptr, nullptr, inst, nullptr);
    if (!g_wnd) {
        UnregisterClassW(kClassName, inst);
        CoUninitialize();
        SetEvent(g_ready);
        return 0;
    }

    SetLayeredWindowAttributes(g_wnd, 0, 255, LWA_ALPHA);
    RegisterShellAppWatcher(g_wnd);

    int scrW = GetSystemMetrics(SM_CXSCREEN), scrH = GetSystemMetrics(SM_CYSCREEN);
    g_dpi = (int)QueryDpi(nullptr);
    UpdateThemeColors();
    RebuildFonts(TitleFontPx(scrH), BaseTitleFontPx(scrH), SearchFontPx(scrH));
    SetEvent(g_ready);

    LoadLayoutFile();
    LoadApps(IconPxFor(scrW, scrH));

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_shellNotifyId) {
        SHChangeNotifyDeregister(g_shellNotifyId);
        g_shellNotifyId = 0;
    }
    KillTimer(g_wnd, kAnimTimer);
    KillTimer(g_wnd, kCaretTimer);
    KillTimer(g_wnd, kHoverTimer);
    KillTimer(g_wnd, kFolderAnimTimer);
    KillTimer(g_wnd, kShellDebounceTimer);
    KillTimer(g_wnd, kPrewarmTimer);
    FreeBackdrop();
    FreeFrame();
    FreeUiBitmaps();
    if (g_bmAdminShield) { DeleteObject(g_bmAdminShield); g_bmAdminShield = nullptr; }
    FreeScratchDC();
    g_wnd = nullptr;
    FreeApps();
    if (g_appsFolder) { g_appsFolder->Release(); g_appsFolder = nullptr; }
    FreeFonts();
    UnregisterClassW(kClassName, inst);
    CoUninitialize();
    return 0;
}

}  // namespace

void Wh_ModSettingsChanged() {
    if (g_wnd) PostMessageW(g_wnd, WM_LP_SETTINGS, 0, 0);
}

BOOL Wh_ModInit() {
    if (IsFolderWindowsProcess()) {
        Wh_Log(L"Folder windows process; skipping StartPad");
        return FALSE;
    }
    LoadSettings();
    if (!HookShowStartView()) {
        return FALSE;
    }

    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, &g_threadId);
    if (!g_thread) {
        CloseHandle(g_ready);
        g_ready = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_thread) return;
    WaitForSingleObject(g_ready, INFINITE);
    if (HWND wnd = g_wnd) {
        PostMessageW(wnd, WM_LP_QUIT, 0, 0);
    } else if (g_threadId) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
    }
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;
    CloseHandle(g_ready);
    g_ready = nullptr;
}