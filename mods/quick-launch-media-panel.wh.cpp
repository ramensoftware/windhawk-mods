// ==WindhawkMod==
// @id              quick-launch-media-panel
// @name            Quick Launch & Media Panel
// @name:fr-FR      Panneau de lancement rapide et multimédia
// @description     A modern desktop panel with configurable app shortcuts, drag and drop, and Windows media controls.
// @description:fr-FR Un panneau de bureau moderne avec raccourcis configurables, glisser-déposer et commandes multimédias Windows.
// @version         2.2.0
// @author          Spartacus
// @github          https://github.com/spartaaacus
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lgdiplus -lgdi32 -luser32 -lshell32 -lshlwapi -ldwmapi -lcomdlg32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quick Launch & Media Panel

A modern, native Windows desktop panel for launching applications, organizing
multiple shortcut pages, and controlling Windows media sessions.

![Quick Launch & Media Panel](https://i.imgur.com/rckHuTB.png)

## Features

- A compact top capsule that expands into the complete panel.
- 1 to 8 configurable application or game shortcuts per page.
- Drag and drop support for applications, shortcuts, documents, folders, and URLs.
- Multiple pages with automatic layout persistence and import/export support.
- Configurable themes, colors, opacity, tile shape, size, spacing, and position.
- Live title, artist, artwork, timeline, volume, and media-session selection.
- Previous, play/pause, and next controls through Windows media sessions.
- A dedicated Windhawk tool process that automatically recovers after Explorer restarts.

## How it differs from existing media mods

Dynamic Island for Windows, Island Media Controls, Taskbar Fluent Media Player,
and Taskbar Music Lounge focus on media presentation in an overlay or the
taskbar. This mod is primarily a pageable desktop application launcher. Its
compact media card is part of the same stationary desktop dashboard, allowing
users to launch their usual applications and control music from one place
without modifying the taskbar or displaying a persistent foreground overlay.

## Usage

- Click the capsule at the top of the screen to open or close the panel.
- Drag a file or shortcut onto a tile to assign it.
- Click a populated tile to launch the target or bring its window to the foreground.
- Right-click a tile to edit its target, label, arguments, icon, color, or elevation mode.
- Drag one tile onto another to reorder shortcuts.
- Use the mouse wheel over the panel to change pages, or `Ctrl + mouse wheel` to resize tiles.
- Use the mouse wheel over the media card to change the master volume.
- Right-click the media card to select the Windows media session to control.
- Use the panel context menu to import or export the complete layout.

The panel doesn't authenticate with Spotify or YouTube. It controls the media
session already published to Windows by the playing application or browser.

---

# Panneau de lancement rapide et multimédia

Un panneau de bureau natif et discret pour lancer rapidement vos applications,
organiser plusieurs pages de raccourcis et contrôler les sessions multimédias
de Windows.

## Utilisation

- Cliquez sur la capsule située en haut de l'écran pour ouvrir ou refermer le panneau.
- Glissez un fichier ou un raccourci sur une tuile pour l'ajouter.
- Cliquez sur une tuile pour lancer l'application ou remettre sa fenêtre au premier plan.
- Faites un clic droit sur une tuile pour modifier son nom, son icône, sa couleur ou ses arguments.
- Faites glisser une tuile vers une autre pour réorganiser les raccourcis.
- Utilisez la molette sur le panneau pour changer de page, ou `Ctrl + molette` pour redimensionner les tuiles.
- Utilisez la molette sur le lecteur multimédia pour régler le volume principal.
- Faites un clic droit sur le lecteur pour choisir la session multimédia à contrôler.

La disposition est enregistrée automatiquement. Le menu contextuel du panneau
permet également de l'exporter ou de l'importer.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: en-US
  $name: Language
  $name:fr-FR: Langue
  $description: Language used by the panel and context menus.
  $description:fr-FR: Langue utilisée par le panneau et ses menus contextuels.
  $options:
    - en-US: English (US)
    - fr: Français
- startCollapsed: true
  $name: Start collapsed
  $name:fr-FR: Démarrer sous forme de capsule
  $description: The panel starts as a small capsule. Click it to open the panel.
  $description:fr-FR: Le panneau démarre sous la forme d'une petite capsule. Cliquez dessus pour l'ouvrir.
- capsuleWidth: 176
  $name: Capsule width
  $name:fr-FR: Largeur de la capsule
  $description: Width of the collapsed capsule, from 120 to 320 pixels.
  $description:fr-FR: Largeur de la capsule fermée, entre 120 et 320 pixels.
- shortcutCount: 4
  $name: Number of shortcuts
  $name:fr-FR: Nombre de raccourcis
  $description: Number of visible shortcut tiles, from 1 to 8.
  $description:fr-FR: Nombre de tuiles de raccourci visibles, entre 1 et 8.
- columns: 4
  $name: Columns
  $name:fr-FR: Colonnes
  $description: Number of shortcut columns.
  $description:fr-FR: Nombre de colonnes de raccourcis.
- tileSize: 64
  $name: Tile size
  $name:fr-FR: Taille des tuiles
  $description: Tile size, from 40 to 112 pixels.
  $description:fr-FR: Taille des tuiles, entre 40 et 112 pixels.
- gap: 12
  $name: Tile spacing
  $name:fr-FR: Espacement des tuiles
  $description: Space between tiles, from 6 to 24 pixels.
  $description:fr-FR: Espacement entre les tuiles, entre 6 et 24 pixels.
- offsetY: 8
  $name: Vertical offset
  $name:fr-FR: Décalage vertical
  $description: Distance from the top of the screen.
  $description:fr-FR: Distance depuis le haut de l'écran.
- opacity: 94
  $name: Panel opacity
  $name:fr-FR: Opacité du panneau
  $description: Global panel opacity, from 45 to 100 percent.
  $description:fr-FR: Opacité générale du panneau, entre 45 et 100 %.
- cornerRadius: 26
  $name: Corner radius
  $name:fr-FR: Arrondi des angles
  $description: Main panel corner radius, from 8 to 40 pixels.
  $description:fr-FR: Arrondi du panneau principal, entre 8 et 40 pixels.
- showLabels: true
  $name: Show shortcut names
  $name:fr-FR: Afficher le nom des raccourcis
- lockPosition: false
  $name: Lock panel position
  $name:fr-FR: Verrouiller la position
- animations: true
  $name: Enable animations
  $name:fr-FR: Activer les animations
- pageCount: 3
  $name: Number of pages
  $name:fr-FR: Nombre de pages
  $description: Number of shortcut pages, from 1 to 5.
  $description:fr-FR: Nombre de pages de raccourcis, entre 1 et 5.
- theme: midnight
  $name: Theme
  $name:fr-FR: Thème
  $options:
    - midnight: Midnight
    - graphite: Graphite
    - ocean: Ocean
    - emerald: Emerald
    - rose: Rose
    - automatic: Automatic
    - custom: Custom
  $options:fr-FR:
    - midnight: Minuit
    - graphite: Graphite
    - ocean: Océan
    - emerald: Émeraude
    - rose: Rose
    - automatic: Automatique
    - custom: Personnalisé
- autoThemeSource: system
  $name: Automatic theme source
  $name:fr-FR: Source du thème automatique
  $description: Accent source used when the Automatic theme is selected.
  $description:fr-FR: Source de la couleur utilisée avec le thème Automatique.
  $options:
    - system: Windows accent
    - wallpaper: Wallpaper
    - media: Album artwork
  $options:fr-FR:
    - system: Couleur Windows
    - wallpaper: Fond d'écran
    - media: Pochette du morceau
- tileShape: rounded
  $name: Tile shape
  $name:fr-FR: Forme des tuiles
  $options:
    - rounded: Rounded
    - circle: Circle
    - square: Square
    - minimal: Minimal
  $options:fr-FR:
    - rounded: Arrondie
    - circle: Ronde
    - square: Carrée
    - minimal: Minimaliste
- monitor: primary
  $name: Display monitor
  $name:fr-FR: Écran d'affichage
  $options:
    - primary: Primary monitor
    - cursor: Monitor containing the cursor
  $options:fr-FR:
    - primary: Écran principal
    - cursor: Écran contenant le curseur
- perMonitorLayouts: false
  $name: Separate layouts for each monitor
  $name:fr-FR: Dispositions séparées par écran
- backgroundColor: "#151821"
  $name: Custom background color
  $name:fr-FR: Couleur de fond personnalisée
  $description: Hexadecimal color used by the Custom theme.
  $description:fr-FR: Couleur hexadécimale utilisée par le thème Personnalisé.
- accentColor: "#7C5CFC"
  $name: Custom accent color
  $name:fr-FR: Couleur d'accent personnalisée
  $description: Hexadecimal color used by the Custom theme.
  $description:fr-FR: Couleur hexadécimale utilisée par le thème Personnalisé.
- shortcut1: ""
  $name: Default shortcut 1
  $name:fr-FR: Raccourci par défaut 1
- shortcut2: ""
  $name: Default shortcut 2
  $name:fr-FR: Raccourci par défaut 2
- shortcut3: ""
  $name: Default shortcut 3
  $name:fr-FR: Raccourci par défaut 3
- shortcut4: ""
  $name: Default shortcut 4
  $name:fr-FR: Raccourci par défaut 4
- shortcut5: ""
  $name: Default shortcut 5
  $name:fr-FR: Raccourci par défaut 5
- shortcut6: ""
  $name: Default shortcut 6
  $name:fr-FR: Raccourci par défaut 6
- shortcut7: ""
  $name: Default shortcut 7
  $name:fr-FR: Raccourci par défaut 7
- shortcut8: ""
  $name: Default shortcut 8
  $name:fr-FR: Raccourci par défaut 8
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <commdlg.h>
#include <shlwapi.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <climits>
#include <cstdint>
#include <cstring>
#include <cwctype>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace Gdiplus;
using MediaManager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager;
using PlaybackStatus = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus;

namespace {

constexpr wchar_t kClassName[] = L"Windhawk.QuickLaunchMediaPanel";
constexpr wchar_t kSinkClassName[] = L"Windhawk.QuickLaunchMediaPanel.Sink";
constexpr wchar_t kPromptClassName[] = L"Windhawk.QuickLaunchMediaPanel.Prompt";
constexpr UINT WM_APP_MEDIA = WM_APP + 40;
constexpr UINT WM_APP_RELOAD = WM_APP + 41;
constexpr UINT WM_APP_QUIT = WM_APP + 42;
constexpr UINT WM_APP_ACTIVE_STATE = WM_APP + 43;
constexpr UINT_PTR kDesktopTimer = 1;
constexpr int kMaxShortcuts = 8;
constexpr int kMaxPages = 5;
constexpr int kCapsuleHeight = 18;
constexpr int kExpandedHeaderHeight = 64;
constexpr int kMediaCardHeight = 84;

enum class TileShape { Rounded, Circle, Square, Minimal };
enum class AutoThemeSource { System, Wallpaper, Media };

struct Settings {
    int count = 4, columns = 4, tile = 64, gap = 12, offsetY = 18;
    int opacity = 94, radius = 26, pageCount = 3, capsuleWidth = 176;
    bool labels = true, cursorMonitor = false, french = false, perMonitorLayouts = false;
    bool lockPosition = false, animations = true, startCollapsed = true;
    TileShape tileShape = TileShape::Rounded;
    AutoThemeSource autoThemeSource = AutoThemeSource::System;
    std::wstring requestedTheme = L"midnight";
    COLORREF background = RGB(21, 24, 33);
    COLORREF accent = RGB(124, 92, 252);
    std::array<std::wstring, kMaxShortcuts> defaults;
};

struct MediaState {
    bool available = false, playing = false;
    std::wstring title, artist, sourceId;
    int64_t startTicks = 0, positionTicks = 0, endTicks = 0;
    std::vector<uint8_t> artwork;
    std::vector<std::pair<std::wstring, std::wstring>> sessions;
};

struct ShortcutData {
    std::wstring path, label, arguments, iconPath, resolvedPath;
    COLORREF color = 0;
    bool customColor = false;
    bool runAsAdmin = false;
    bool allowFileNameMatch = false;
};

Settings g_settings;
MediaState g_media;
std::mutex g_mediaMutex;
std::mutex g_mediaActionMutex;
std::array<std::wstring, kMaxShortcuts> g_paths;
std::array<ShortcutData, kMaxShortcuts> g_shortcuts;
std::array<HICON, kMaxShortcuts> g_icons{};
[[clang::no_destroy]] std::array<std::unique_ptr<Bitmap>, kMaxShortcuts>
    g_iconBitmaps;
std::array<HWND, kMaxShortcuts> g_runningWindows{};
std::array<wchar_t, 32768> g_processPathBuffer{};
HINSTANCE g_hInst = nullptr;
std::atomic<HWND> g_hwnd{nullptr};
HWND g_sinkWnd = nullptr;
HWND g_menuOwnerWnd = nullptr;
HWND g_modalDialog = nullptr;
HWND g_desktopHost = nullptr;
HWINEVENTHOOK g_foregroundHook = nullptr;
IDropTarget* g_dropTarget = nullptr;
bool g_dropRegistered = false;
bool g_uiInitialized = false;
bool g_modalActive = false;
bool g_ignoreNextButtonUp = false;
std::atomic_bool g_desktopVisible{false};
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
HANDLE g_readyEvent = nullptr;
HANDLE g_mediaThread = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_mediaActionEvent = nullptr;
HANDLE g_activityEvent = nullptr;
ULONG_PTR g_gdiplusToken = 0;
int g_width = 420, g_height = 210, g_hover = -1;
UINT g_dpi = 96;
bool g_dragging = false;
POINT g_dragStart{};
RECT g_dragWindowStart{};
int g_savedX = INT_MIN, g_savedY = INT_MIN;
int g_currentPage = 0;
std::wstring g_selectedMediaSession;
std::wstring g_layoutMonitorSuffix = L"primary";
std::wstring g_stateFile;
std::wstring g_currentPageName;
int g_pressedTile = -1;
POINT g_tilePressPoint{};
bool g_reordering = false;
bool g_panelExpanded = true;
bool g_capsuleHover = false;
float g_pageAnimation = 1.0f;
uint64_t g_lastArtworkHash = 0;
COLORREF g_cachedArtworkAccent = RGB(124, 92, 252);
std::wstring g_cachedWallpaperPath;
FILETIME g_cachedWallpaperWriteTime{};
COLORREF g_cachedWallpaperAccent = RGB(124, 92, 252);
ULONGLONG g_lastRunningWindowSweep = 0;

HDC g_renderDc = nullptr;
HBITMAP g_renderBitmap = nullptr;
HBITMAP g_renderOldBitmap = nullptr;
void* g_renderBits = nullptr;
int g_renderWidth = 0;
int g_renderHeight = 0;

enum class MediaActionKind { Previous, Toggle, Next, Seek, Volume };
struct MediaAction {
    MediaActionKind kind;
    double value = 0;
};
std::vector<MediaAction> g_mediaActions;

void Render();
void LoadAssignments();
void PositionWindow();
void LayoutMetrics();
void EnsurePanelWindow();
void DestroyRenderSurface();
bool RefreshRunningWindows();
std::wstring ResolveExecutable(const std::wstring& path);

int Clamp(int value, int lo, int hi) { return std::max(lo, std::min(hi, value)); }

int Scale(int value) { return MulDiv(value, static_cast<int>(g_dpi), 96); }
int CurrentLogicalWidth() {
    return g_panelExpanded ? g_width : g_settings.capsuleWidth;
}
int CurrentLogicalHeight() {
    return g_panelExpanded ? g_height : kCapsuleHeight;
}
int PixelWidth() { return Scale(CurrentLogicalWidth()); }
int PixelHeight() { return Scale(CurrentLogicalHeight()); }

POINT ToLogicalPoint(POINT point) {
    point.x = MulDiv(point.x, 96, static_cast<int>(g_dpi));
    point.y = MulDiv(point.y, 96, static_cast<int>(g_dpi));
    return point;
}

std::wstring GetStringSetting(const wchar_t* name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value;
    Wh_FreeStringSetting(value);
    return result;
}

COLORREF ParseColor(std::wstring value, COLORREF fallback) {
    if (!value.empty() && value[0] == L'#') value.erase(0, 1);
    if (value.size() != 6) return fallback;
    wchar_t* end = nullptr;
    unsigned long rgb = wcstoul(value.c_str(), &end, 16);
    if (!end || *end) return fallback;
    return RGB((rgb >> 16) & 255, (rgb >> 8) & 255, rgb & 255);
}

bool InitializeStateFile() {
    wchar_t storage[MAX_PATH]{};
    if (Wh_GetModStoragePath(storage, ARRAYSIZE(storage)) <= 0) {
        Wh_Log(L"Wh_GetModStoragePath failed");
        g_stateFile.clear();
        return false;
    }
    CreateDirectoryW(storage, nullptr);
    g_stateFile = std::wstring(storage) + L"\\layout.ini";
    return true;
}

const std::wstring& StateFile() {
    return g_stateFile;
}

void LoadSettings() {
    Settings next;
    next.count = Clamp(Wh_GetIntSetting(L"shortcutCount"), 1, kMaxShortcuts);
    next.columns = Clamp(Wh_GetIntSetting(L"columns"), 1, kMaxShortcuts);
    next.tile = Clamp(Wh_GetIntSetting(L"tileSize"), 40, 112);
    next.gap = Clamp(Wh_GetIntSetting(L"gap"), 6, 24);
    next.offsetY = Clamp(Wh_GetIntSetting(L"offsetY"), 0, 500);
    next.opacity = Clamp(Wh_GetIntSetting(L"opacity"), 45, 100);
    next.radius = Clamp(Wh_GetIntSetting(L"cornerRadius"), 8, 40);
    next.pageCount = Clamp(Wh_GetIntSetting(L"pageCount"), 1, kMaxPages);
    next.capsuleWidth = Clamp(Wh_GetIntSetting(L"capsuleWidth"), 120, 320);
    next.labels = Wh_GetIntSetting(L"showLabels") != 0;
    next.lockPosition = Wh_GetIntSetting(L"lockPosition") != 0;
    next.animations = Wh_GetIntSetting(L"animations") != 0;
    next.startCollapsed = Wh_GetIntSetting(L"startCollapsed") != 0;
    next.french = _wcsicmp(GetStringSetting(L"language").c_str(), L"fr") == 0;
    next.cursorMonitor = _wcsicmp(GetStringSetting(L"monitor").c_str(), L"cursor") == 0;
    next.perMonitorLayouts = Wh_GetIntSetting(L"perMonitorLayouts") != 0;
    const std::wstring theme = GetStringSetting(L"theme");
    next.requestedTheme = theme;
    const std::wstring tileShape = GetStringSetting(L"tileShape");
    if (_wcsicmp(tileShape.c_str(), L"circle") == 0) next.tileShape = TileShape::Circle;
    else if (_wcsicmp(tileShape.c_str(), L"square") == 0) next.tileShape = TileShape::Square;
    else if (_wcsicmp(tileShape.c_str(), L"minimal") == 0) next.tileShape = TileShape::Minimal;
    const std::wstring autoTheme = GetStringSetting(L"autoThemeSource");
    if (_wcsicmp(autoTheme.c_str(), L"wallpaper") == 0) next.autoThemeSource = AutoThemeSource::Wallpaper;
    else if (_wcsicmp(autoTheme.c_str(), L"media") == 0) next.autoThemeSource = AutoThemeSource::Media;
    if (_wcsicmp(theme.c_str(), L"graphite") == 0) {
        next.background = RGB(27, 27, 30);
        next.accent = RGB(191, 196, 204);
    } else if (_wcsicmp(theme.c_str(), L"ocean") == 0) {
        next.background = RGB(16, 32, 43);
        next.accent = RGB(56, 189, 248);
    } else if (_wcsicmp(theme.c_str(), L"emerald") == 0) {
        next.background = RGB(16, 35, 30);
        next.accent = RGB(52, 211, 153);
    } else if (_wcsicmp(theme.c_str(), L"rose") == 0) {
        next.background = RGB(37, 23, 31);
        next.accent = RGB(251, 113, 133);
    } else if (_wcsicmp(theme.c_str(), L"custom") == 0) {
        next.background = ParseColor(GetStringSetting(L"backgroundColor"), next.background);
        next.accent = ParseColor(GetStringSetting(L"accentColor"), next.accent);
    } else {
        next.background = RGB(21, 24, 33);
        next.accent = RGB(124, 92, 252);
    }
    for (int i = 0; i < kMaxShortcuts; ++i) {
        wchar_t key[24];
        swprintf_s(key, L"shortcut%d", i + 1);
        next.defaults[i] = GetStringSetting(key);
    }
    g_settings = std::move(next);
    if (g_currentPage >= g_settings.pageCount) g_currentPage = g_settings.pageCount - 1;
}

void DestroyIcons() {
    for (auto& bitmap : g_iconBitmaps) bitmap.reset();
    for (auto& icon : g_icons) {
        if (icon) DestroyIcon(icon);
        icon = nullptr;
    }
}

std::wstring PageSection() {
    std::wstring section = L"page." + std::to_wstring(g_currentPage + 1);
    if (g_settings.perMonitorLayouts) {
        section += L".monitor." + g_layoutMonitorSuffix;
    }
    return section;
}

std::wstring ReadIniString(const std::wstring& section, const std::wstring& key,
                           const std::wstring& fallback = L"") {
    const std::wstring ini = StateFile();
    if (ini.empty()) return fallback;
    std::vector<wchar_t> buffer(32768);
    GetPrivateProfileStringW(section.c_str(), key.c_str(), fallback.c_str(),
                             buffer.data(), static_cast<DWORD>(buffer.size()), ini.c_str());
    return buffer.data();
}

void WriteIniString(const std::wstring& section, const std::wstring& key,
                    const std::wstring& value) {
    const std::wstring ini = StateFile();
    if (!ini.empty()) WritePrivateProfileStringW(section.c_str(), key.c_str(), value.c_str(), ini.c_str());
}

void SaveShortcut(int slot) {
    if (slot < 0 || slot >= kMaxShortcuts) return;
    const std::wstring section = PageSection();
    const std::wstring n = std::to_wstring(slot + 1);
    const ShortcutData& s = g_shortcuts[slot];
    WriteIniString(section, L"path" + n, s.path);
    WriteIniString(section, L"label" + n, s.label);
    WriteIniString(section, L"arguments" + n, s.arguments);
    WriteIniString(section, L"icon" + n, s.iconPath);
    WriteIniString(section, L"color" + n, s.customColor ? std::to_wstring(s.color) : L"");
    WriteIniString(section, L"admin" + n, s.runAsAdmin ? L"1" : L"0");
}

void ClearShortcutOverride(int slot) {
    if (slot < 0 || slot >= kMaxShortcuts || StateFile().empty()) return;
    const std::wstring section = PageSection();
    const std::wstring n = std::to_wstring(slot + 1);
    const wchar_t* prefixes[] = {L"path", L"label", L"arguments", L"icon",
                                 L"color", L"admin"};
    for (const wchar_t* prefix : prefixes) {
        const std::wstring key = std::wstring(prefix) + n;
        WritePrivateProfileStringW(section.c_str(), key.c_str(), nullptr,
                                   StateFile().c_str());
    }
}

void LoadAssignments() {
    DestroyIcons();
    g_lastRunningWindowSweep = 0;
    const std::wstring section = PageSection();
    g_currentPageName = ReadIniString(section, L"name");
    if (g_currentPageName.empty()) {
        g_currentPageName = L"Page " + std::to_wstring(g_currentPage + 1);
    }
    for (int i = 0; i < kMaxShortcuts; ++i) {
        const std::wstring n = std::to_wstring(i + 1);
        ShortcutData item;
        item.path = ReadIniString(section, L"path" + n);
        if (item.path.empty() && g_currentPage == 0) {
            item.path = g_settings.defaults[i];
        }
        item.label = ReadIniString(section, L"label" + n);
        item.arguments = ReadIniString(section, L"arguments" + n);
        item.iconPath = ReadIniString(section, L"icon" + n);
        const std::wstring color = ReadIniString(section, L"color" + n);
        if (!color.empty()) {
            item.color = static_cast<COLORREF>(_wcstoui64(color.c_str(), nullptr, 10));
            item.customColor = true;
        }
        item.runAsAdmin = ReadIniString(section, L"admin" + n) == L"1";
        item.resolvedPath = ResolveExecutable(item.path);
        item.allowFileNameMatch =
            _wcsicmp(PathFindExtensionW(item.path.c_str()), L".lnk") == 0;
        g_shortcuts[i] = std::move(item);
        g_paths[i] = g_shortcuts[i].path;
        std::wstring iconSource = g_shortcuts[i].iconPath.empty() ? g_paths[i] : g_shortcuts[i].iconPath;
        if (!iconSource.empty()) {
            SHFILEINFOW info{};
            if (SHGetFileInfoW(iconSource.c_str(), 0, &info, sizeof(info),
                               SHGFI_ICON | SHGFI_LARGEICON)) {
                g_icons[i] = info.hIcon;
                g_iconBitmaps[i] = std::make_unique<Bitmap>(g_icons[i]);
            }
        }
        g_runningWindows[i] = nullptr;
    }
}

void SaveDroppedPath(int slot, const std::wstring& path) {
    if (slot < 0 || slot >= kMaxShortcuts) return;
    g_shortcuts[slot].path = path;
    if (path.empty()) {
        g_shortcuts[slot].label.clear();
        g_shortcuts[slot].arguments.clear();
        g_shortcuts[slot].iconPath.clear();
        g_shortcuts[slot].customColor = false;
        g_shortcuts[slot].runAsAdmin = false;
    }
    SaveShortcut(slot);
}

std::wstring MonitorStorageSuffix(HMONITOR monitor) {
    MONITORINFOEXW mi{};mi.cbSize=sizeof(mi);if(!GetMonitorInfoW(monitor,&mi))return L"primary";
    std::wstring key=mi.szDevice;for(wchar_t& c:key)if(!iswalnum(c))c=L'_';return key;
}

HMONITOR SelectedMonitor() {
    if (g_settings.cursorMonitor) {
        POINT cursor{};
        GetCursorPos(&cursor);
        return MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    }
    POINT origin{};
    return MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
}

UINT DpiForMonitor(HMONITOR monitor) {
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static HMODULE shcore = LoadLibraryExW(
        L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    static auto getDpiForMonitor = shcore ? reinterpret_cast<GetDpiForMonitor_t>(
                                               GetProcAddress(shcore, "GetDpiForMonitor"))
                                         : nullptr;
    UINT x = 96;
    UINT y = 96;
    if (getDpiForMonitor &&
        SUCCEEDED(getDpiForMonitor(monitor, 0, &x, &y))) {
        return x;
    }
    return 96;
}

void LoadPersistentUiState() {
    const std::wstring ini = StateFile();
    if (ini.empty()) return;
    int tileOverride = GetPrivateProfileIntW(L"ui", L"tileSize", -1, ini.c_str());
    if (tileOverride >= 40 && tileOverride <= 112) g_settings.tile = tileOverride;
    HMONITOR monitor = SelectedMonitor();
    std::wstring suffix=MonitorStorageSuffix(monitor);
    g_layoutMonitorSuffix=suffix;
    g_savedX = GetPrivateProfileIntW(L"ui", (L"windowX."+suffix).c_str(), GetPrivateProfileIntW(L"ui",L"windowX",INT_MIN,ini.c_str()), ini.c_str());
    g_savedY = GetPrivateProfileIntW(L"ui", (L"windowY."+suffix).c_str(), GetPrivateProfileIntW(L"ui",L"windowY",INT_MIN,ini.c_str()), ini.c_str());
    g_currentPage = Clamp(GetPrivateProfileIntW(L"ui", L"currentPage", 0, ini.c_str()),
                          0, std::max(0, g_settings.pageCount - 1));
    int lockOverride = GetPrivateProfileIntW(L"ui", L"lockPosition", -1, ini.c_str());
    if (lockOverride >= 0) g_settings.lockPosition = lockOverride != 0;
}

void SaveUiNumber(const wchar_t* key, int value) {
    const std::wstring ini = StateFile();
    if (ini.empty()) return;
    wchar_t text[32];
    swprintf_s(text, L"%d", value);
    WritePrivateProfileStringW(L"ui", key, text, ini.c_str());
}

void SaveWindowPosition() {
    if (!g_hwnd) return;
    RECT r{};
    GetWindowRect(g_hwnd, &r);
    g_savedX = r.left;
    g_savedY = r.top;
    SaveUiNumber(L"windowX", g_savedX);
    SaveUiNumber(L"windowY", g_savedY);
    std::wstring suffix=MonitorStorageSuffix(MonitorFromRect(&r,MONITOR_DEFAULTTONEAREST));
    SaveUiNumber((L"windowX."+suffix).c_str(),g_savedX);
    SaveUiNumber((L"windowY."+suffix).c_str(),g_savedY);
    if(g_settings.perMonitorLayouts&&suffix!=g_layoutMonitorSuffix){g_layoutMonitorSuffix=suffix;LoadAssignments();Render();}
}

void ClearSavedPosition() {
    const std::wstring ini = StateFile();
    if (!ini.empty()) {
        WritePrivateProfileStringW(L"ui", L"windowX", nullptr, ini.c_str());
        WritePrivateProfileStringW(L"ui", L"windowY", nullptr, ini.c_str());
        RECT r{};if(g_hwnd)GetWindowRect(g_hwnd,&r);
        std::wstring suffix=MonitorStorageSuffix(MonitorFromRect(&r,MONITOR_DEFAULTTONEAREST));
        WritePrivateProfileStringW(L"ui",(L"windowX."+suffix).c_str(),nullptr,ini.c_str());
        WritePrivateProfileStringW(L"ui",(L"windowY."+suffix).c_str(),nullptr,ini.c_str());
    }
    g_savedX = g_savedY = INT_MIN;
}

std::wstring DisplayName(const std::wstring& path, int slot) {
    if (slot >= 0 && slot < kMaxShortcuts && !g_shortcuts[slot].label.empty())
        return g_shortcuts[slot].label;
    if (path.empty()) return g_settings.french ? L"Ajouter" : L"Add";
    wchar_t copy[MAX_PATH];
    wcsncpy_s(copy, path.c_str(), _TRUNCATE);
    PathStripPathW(copy);
    PathRemoveExtensionW(copy);
    if (copy[0]) return copy;
    return L"App " + std::to_wstring(slot + 1);
}

BOOL CALLBACK FindDesktopHostProc(HWND hwnd, LPARAM lParam) {
    HWND defView = FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr);
    if (defView) {
        *reinterpret_cast<HWND*>(lParam) = hwnd;
        return FALSE;
    }
    return TRUE;
}

HWND FindDesktopHost() {
    static bool missingHostLogged = false;
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (!progman) {
        if (!missingHostLogged) {
            Wh_Log(L"Desktop host isn't ready yet; retrying");
            missingHostLogged = true;
        }
        return nullptr;
    }
    HWND host = nullptr;
    EnumWindows(FindDesktopHostProc, reinterpret_cast<LPARAM>(&host));
    missingHostLogged = false;
    return host ? host : progman;
}

void OpenWindhawk() {
    wchar_t path[MAX_PATH]{};
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (length > 0 && length < ARRAYSIZE(path)) {
        ShellExecuteW(nullptr, L"open", path, nullptr, nullptr, SW_SHOWNORMAL);
    }
}

void LayoutMetrics() {
    int columns = std::min(g_settings.columns, g_settings.count);
    int rows = (g_settings.count + columns - 1) / columns;
    int labelExtra = g_settings.labels ? 20 : 0;
    int gridHeight = rows * (g_settings.tile + labelExtra) +
                     (rows - 1) * g_settings.gap;
    g_width = std::max(460, 52 + columns * g_settings.tile +
                                (columns - 1) * g_settings.gap);
    g_height = kExpandedHeaderHeight + gridHeight + 18 +
               kMediaCardHeight + 22;
}

RECT TileRect(int index) {
    int columns = std::min(g_settings.columns, g_settings.count);
    int labelExtra = g_settings.labels ? 20 : 0;
    int row = index / columns, col = index % columns;
    int used = std::min(columns, g_settings.count - row * columns);
    int rowWidth = used * g_settings.tile + (used - 1) * g_settings.gap;
    int left = (g_width - rowWidth) / 2 + col * (g_settings.tile + g_settings.gap);
    int top = kExpandedHeaderHeight +
              row * (g_settings.tile + labelExtra + g_settings.gap);
    return {left, top, left + g_settings.tile, top + g_settings.tile};
}

int HitTile(POINT pt) {
    for (int i = 0; i < g_settings.count; ++i) {
        RECT r = TileRect(i);
        if (PtInRect(&r, pt)) return i;
    }
    return -1;
}

class PanelDropTarget final : public IDropTarget {
    LONG references_ = 1;
    bool AcceptsFiles(IDataObject* object) {
        FORMATETC format{CF_HDROP,nullptr,DVASPECT_CONTENT,-1,TYMED_HGLOBAL};
        return object&&SUCCEEDED(object->QueryGetData(&format));
    }
    void SelectEffect(DWORD* effect) {
        if(!effect)return;DWORD allowed=*effect;
        if(allowed&DROPEFFECT_LINK)*effect=DROPEFFECT_LINK;
        else if(allowed&DROPEFFECT_COPY)*effect=DROPEFFECT_COPY;
        else *effect=DROPEFFECT_NONE;
    }
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void** object) override {
        if(!object)return E_POINTER;*object=nullptr;
        if(iid==IID_IUnknown||iid==IID_IDropTarget){*object=static_cast<IDropTarget*>(this);AddRef();return S_OK;}
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {return InterlockedIncrement(&references_);}
    ULONG STDMETHODCALLTYPE Release() override {ULONG value=InterlockedDecrement(&references_);if(!value)delete this;return value;}
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* object,DWORD,POINTL,DWORD* effect) override {
        if(AcceptsFiles(object))SelectEffect(effect);else if(effect)*effect=DROPEFFECT_NONE;return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragOver(DWORD,POINTL,DWORD* effect) override {SelectEffect(effect);return S_OK;}
    HRESULT STDMETHODCALLTYPE DragLeave() override {return S_OK;}
    HRESULT STDMETHODCALLTYPE Drop(IDataObject* object,DWORD,POINTL point,DWORD* effect) override {
        FORMATETC format{CF_HDROP,nullptr,DVASPECT_CONTENT,-1,TYMED_HGLOBAL};STGMEDIUM medium{};
        if(!object||FAILED(object->GetData(&format,&medium))){if(effect)*effect=DROPEFFECT_NONE;return S_OK;}
        HDROP drop=static_cast<HDROP>(GlobalLock(medium.hGlobal));
        POINT client{point.x,point.y};ScreenToClient(g_hwnd,&client);client=ToLogicalPoint(client);int tile=HitTile(client);
        bool accepted=false;
        if(drop&&tile>=0){std::vector<wchar_t> path(32768);if(DragQueryFileW(drop,0,path.data(),static_cast<UINT>(path.size()))){g_shortcuts[tile]=ShortcutData{};SaveDroppedPath(tile,path.data());LoadAssignments();Render();accepted=true;}}
        if(drop)GlobalUnlock(medium.hGlobal);ReleaseStgMedium(&medium);
        if(accepted)SelectEffect(effect);else if(effect)*effect=DROPEFFECT_NONE;return S_OK;
    }
};

int MediaTop() {
    int columns = std::min(g_settings.columns, g_settings.count);
    int rows = (g_settings.count + columns - 1) / columns;
    return kExpandedHeaderHeight +
           rows * (g_settings.tile + (g_settings.labels ? 20 : 0)) +
           (rows - 1) * g_settings.gap + 18;
}

RECT PreviousPageRect() { return {18, 27, 50, 55}; }
RECT NextPageRect() { return {g_width - 50, 27, g_width - 18, 55}; }
RECT CapsuleToggleRect() {
    if (!g_panelExpanded) {
        return {0, 0, g_settings.capsuleWidth, kCapsuleHeight};
    }
    return {g_width / 2 - 54, 4, g_width / 2 + 54, 20};
}
RECT MediaCardRect() {
    const int top = MediaTop();
    return {18, top, g_width - 18, top + kMediaCardHeight};
}
RECT MediaTimelineRect() {
    const int top = MediaTop();
    return {32, top + 68, g_width - 32, top + 81};
}

std::wstring CurrentPageName() {
    return g_currentPageName;
}

void SwitchPage(int delta) {
    int next = (g_currentPage + delta + g_settings.pageCount) % g_settings.pageCount;
    if (next == g_currentPage) return;
    g_currentPage = next;
    SaveUiNumber(L"currentPage", g_currentPage);
    LoadAssignments();
    g_pageAnimation = g_settings.animations ? 0.15f : 1.0f;
    if (g_settings.animations && g_hwnd) SetTimer(g_hwnd, 7, 16, nullptr);
    Render();
}

void TogglePanelExpanded() {
    g_panelExpanded = !g_panelExpanded;
    g_capsuleHover = false;
    g_hover = -1;
    DestroyRenderSurface();
    PositionWindow();
    Render();
}

void PositionWindow() {
    HWND panel = g_hwnd.load();
    if (!panel) return;
    HMONITOR monitor = SelectedMonitor();
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(monitor, &mi);
    const int width = PixelWidth();
    const int height = PixelHeight();
    int x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - width) / 2;
    int y = mi.rcWork.top + Scale(g_settings.offsetY);
    if (g_savedX != INT_MIN && g_savedY != INT_MIN) {
        const int expandedWidth = Scale(g_width);
        x = g_panelExpanded
                ? g_savedX
                : g_savedX + (expandedWidth - width) / 2;
        y = g_savedY;
        x = Clamp(x, mi.rcWork.left,
                  std::max(mi.rcWork.left, mi.rcWork.right - width));
        y = Clamp(y, mi.rcWork.top,
                  std::max(mi.rcWork.top, mi.rcWork.bottom - height));
    }
    // Keep the panel immediately above the desktop host instead of placing it
    // at the absolute bottom of the Z order, where some Windows 11 desktop
    // configurations put it behind the wallpaper renderer.
    // Insert the panel above all Progman/WorkerW desktop layers but below the
    // first regular top-level window. This keeps it permanently attached to
    // the wallpaper without making it an always-on-top overlay.
    HWND insertAfter = g_desktopHost
                           ? GetWindow(g_desktopHost, GW_HWNDPREV)
                           : nullptr;
    while (insertAfter) {
        if (insertAfter == panel) {
            insertAfter = GetWindow(insertAfter, GW_HWNDPREV);
            continue;
        }
        wchar_t className[64]{};
        GetClassNameW(insertAfter, className, ARRAYSIZE(className));
        if (_wcsicmp(className, L"Progman") != 0 &&
            _wcsicmp(className, L"WorkerW") != 0) {
            break;
        }
        insertAfter = GetWindow(insertAfter, GW_HWNDPREV);
    }
    if (!insertAfter) insertAfter = HWND_TOP;

    if (!SetWindowPos(panel, insertAfter, x, y, width, height,
                      SWP_NOACTIVATE | SWP_SHOWWINDOW |
                          SWP_NOOWNERZORDER)) {
        Wh_Log(L"SetWindowPos failed, error=%u", GetLastError());
    }
}

void ResizeTiles(int delta) {
    int next = Clamp(g_settings.tile + delta, 40, 112);
    if (next == g_settings.tile) return;
    g_settings.tile = next;
    SaveUiNumber(L"tileSize", next);
    LayoutMetrics();
    PositionWindow();
    Render();
}

void RoundRectFill(Graphics& g, const RectF& r, float radius, const Color& color) {
    radius = std::max(0.0f, std::min(radius, std::min(r.Width, r.Height) / 2.0f));
    if (radius <= 0.0f) {
        SolidBrush brush(color);
        g.FillRectangle(&brush, r);
        return;
    }
    GraphicsPath path;
    float d = radius * 2;
    path.AddArc(r.X, r.Y, d, d, 180, 90); path.AddArc(r.GetRight()-d, r.Y, d, d, 270, 90);
    path.AddArc(r.GetRight()-d, r.GetBottom()-d, d, d, 0, 90); path.AddArc(r.X, r.GetBottom()-d, d, d, 90, 90);
    path.CloseFigure();
    SolidBrush brush(color); g.FillPath(&brush, &path);
}

void RoundRectGradient(Graphics& g, const RectF& r, float radius,
                       const Color& top, const Color& bottom) {
    radius = std::max(0.0f, std::min(radius, std::min(r.Width, r.Height) / 2.0f));
    GraphicsPath path;
    float d = radius * 2;
    path.AddArc(r.X, r.Y, d, d, 180, 90);
    path.AddArc(r.GetRight() - d, r.Y, d, d, 270, 90);
    path.AddArc(r.GetRight() - d, r.GetBottom() - d, d, d, 0, 90);
    path.AddArc(r.X, r.GetBottom() - d, d, d, 90, 90);
    path.CloseFigure();
    LinearGradientBrush brush(r, top, bottom, LinearGradientModeVertical);
    g.FillPath(&brush, &path);
}

void RoundRectStroke(Graphics& g, const RectF& r, float radius,
                     const Color& color, float width) {
    radius = std::max(0.0f, std::min(radius, std::min(r.Width, r.Height) / 2.0f));
    GraphicsPath path;
    float d = radius * 2;
    path.AddArc(r.X, r.Y, d, d, 180, 90);
    path.AddArc(r.GetRight() - d, r.Y, d, d, 270, 90);
    path.AddArc(r.GetRight() - d, r.GetBottom() - d, d, d, 0, 90);
    path.AddArc(r.X, r.GetBottom() - d, d, d, 90, 90);
    path.CloseFigure();
    Pen pen(color, width);
    g.DrawPath(&pen, &path);
}

void DrawCentered(Graphics& g, const std::wstring& text, RectF rect, float size, Color color, bool bold = false) {
    FontFamily family(L"Segoe UI");
    Font font(&family, size, bold ? FontStyleBold : FontStyleRegular, UnitPixel);
    StringFormat format; format.SetAlignment(StringAlignmentCenter); format.SetLineAlignment(StringAlignmentCenter); format.SetTrimming(StringTrimmingEllipsisCharacter);
    SolidBrush brush(color); g.DrawString(text.c_str(), -1, &font, rect, &format, &brush);
}

bool DrawImageBytes(Graphics& g, const std::vector<uint8_t>& bytes, const RectF& rect) {
    if(bytes.empty()) return false;
    HGLOBAL memory=GlobalAlloc(GMEM_MOVEABLE,bytes.size());if(!memory)return false;
    void* target=GlobalLock(memory);memcpy(target,bytes.data(),bytes.size());GlobalUnlock(memory);
    IStream* stream=nullptr;if(FAILED(CreateStreamOnHGlobal(memory,TRUE,&stream))){GlobalFree(memory);return false;}
    bool ok=false;{
        Bitmap bitmap(stream);if(bitmap.GetLastStatus()==Ok){g.DrawImage(&bitmap,rect);ok=true;}
    }stream->Release();return ok;
}

COLORREF AverageBitmap(Bitmap& bitmap, COLORREF fallback) {
    if(bitmap.GetLastStatus()!=Ok||bitmap.GetWidth()==0||bitmap.GetHeight()==0)return fallback;
    uint64_t r=0,g=0,b=0,count=0;UINT stepX=std::max<UINT>(1,bitmap.GetWidth()/24),stepY=std::max<UINT>(1,bitmap.GetHeight()/24);
    for(UINT y=0;y<bitmap.GetHeight();y+=stepY)for(UINT x=0;x<bitmap.GetWidth();x+=stepX){Color c;if(bitmap.GetPixel(x,y,&c)==Ok&&c.GetAlpha()>100){r+=c.GetRed();g+=c.GetGreen();b+=c.GetBlue();count++;}}
    return count?RGB(r/count,g/count,b/count):fallback;
}

COLORREF AccentFromArtwork(const std::vector<uint8_t>& bytes,COLORREF fallback) {
    if(bytes.empty())return fallback;HGLOBAL memory=GlobalAlloc(GMEM_MOVEABLE,bytes.size());if(!memory)return fallback;
    void* target=GlobalLock(memory);memcpy(target,bytes.data(),bytes.size());GlobalUnlock(memory);IStream* stream=nullptr;
    if(FAILED(CreateStreamOnHGlobal(memory,TRUE,&stream))){GlobalFree(memory);return fallback;}COLORREF result=fallback;
    {Bitmap bitmap(stream);result=AverageBitmap(bitmap,fallback);}stream->Release();return result;
}

bool ApplyAutomaticTheme() {
    if (_wcsicmp(g_settings.requestedTheme.c_str(), L"automatic") != 0) {
        return false;
    }

    const COLORREF previousAccent = g_settings.accent;
    const COLORREF previousBackground = g_settings.background;
    COLORREF accent = RGB(124, 92, 252);

    if (g_settings.autoThemeSource == AutoThemeSource::System) {
        DWORD color = 0;
        BOOL opaque = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
            accent = RGB((color >> 16) & 255, (color >> 8) & 255,
                         color & 255);
        }
    } else if (g_settings.autoThemeSource == AutoThemeSource::Wallpaper) {
        wchar_t path[MAX_PATH]{};
        if (SystemParametersInfoW(SPI_GETDESKWALLPAPER, ARRAYSIZE(path), path,
                                  0) &&
            path[0]) {
            WIN32_FILE_ATTRIBUTE_DATA attributes{};
            const bool hasAttributes = GetFileAttributesExW(
                path, GetFileExInfoStandard, &attributes);
            const bool cacheMatches =
                g_cachedWallpaperPath == path && hasAttributes &&
                CompareFileTime(&g_cachedWallpaperWriteTime,
                                &attributes.ftLastWriteTime) == 0;
            if (!cacheMatches) {
                Bitmap wallpaper(path);
                g_cachedWallpaperAccent = AverageBitmap(wallpaper, accent);
                g_cachedWallpaperPath = path;
                g_cachedWallpaperWriteTime =
                    hasAttributes ? attributes.ftLastWriteTime : FILETIME{};
            }
            accent = g_cachedWallpaperAccent;
        }
    } else {
        std::vector<uint8_t> art;
        {
            std::lock_guard lock(g_mediaMutex);
            art = g_media.artwork;
        }
        uint64_t hash = 1469598103934665603ULL;
        for (uint8_t byte : art) {
            hash ^= byte;
            hash *= 1099511628211ULL;
        }
        if (!art.empty() && hash != g_lastArtworkHash) {
            g_cachedArtworkAccent = AccentFromArtwork(art, accent);
            g_lastArtworkHash = hash;
        }
        accent = art.empty() ? accent : g_cachedArtworkAccent;
    }
    g_settings.accent = accent;
    g_settings.background =
        RGB(10 + GetRValue(accent) / 10, 12 + GetGValue(accent) / 10,
            15 + GetBValue(accent) / 10);
    return previousAccent != g_settings.accent ||
           previousBackground != g_settings.background;
}

void DestroyRenderSurface() {
    if (g_renderDc && g_renderOldBitmap) {
        SelectObject(g_renderDc, g_renderOldBitmap);
    }
    if (g_renderBitmap) DeleteObject(g_renderBitmap);
    if (g_renderDc) DeleteDC(g_renderDc);
    g_renderDc = nullptr;
    g_renderBitmap = nullptr;
    g_renderOldBitmap = nullptr;
    g_renderBits = nullptr;
    g_renderWidth = 0;
    g_renderHeight = 0;
}

bool EnsureRenderSurface(int width, int height) {
    if (g_renderDc && g_renderWidth == width && g_renderHeight == height) {
        return true;
    }
    DestroyRenderSurface();
    HDC screen = GetDC(nullptr);
    if (!screen) return false;
    g_renderDc = CreateCompatibleDC(screen);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    g_renderBitmap = CreateDIBSection(screen, &bi, DIB_RGB_COLORS,
                                      &g_renderBits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!g_renderDc || !g_renderBitmap || !g_renderBits) {
        DestroyRenderSurface();
        return false;
    }
    g_renderOldBitmap =
        static_cast<HBITMAP>(SelectObject(g_renderDc, g_renderBitmap));
    g_renderWidth = width;
    g_renderHeight = height;
    return true;
}

void PresentRenderSurface(int pixelWidth, int pixelHeight) {
    HDC screen = GetDC(nullptr);
    if (!screen) return;
    POINT source{0, 0};
    SIZE size{pixelWidth, pixelHeight};
    const BYTE opacity =
        static_cast<BYTE>(255 * g_settings.opacity / 100);
    BLENDFUNCTION blend{AC_SRC_OVER, 0, opacity, AC_SRC_ALPHA};
    if (!UpdateLayeredWindow(g_hwnd, screen, nullptr, &size, g_renderDc,
                             &source, 0, &blend, ULW_ALPHA)) {
        static bool logged = false;
        if (!logged) {
            Wh_Log(L"UpdateLayeredWindow failed, error=%u", GetLastError());
            logged = true;
        }
    }
    ReleaseDC(nullptr, screen);
}

void Render() {
    if (!g_hwnd) return;
    const int pixelWidth = PixelWidth();
    const int pixelHeight = PixelHeight();
    if (!EnsureRenderSurface(pixelWidth, pixelHeight)) return;
    memset(g_renderBits, 0, static_cast<size_t>(pixelWidth) * pixelHeight * 4);
    Bitmap surface(pixelWidth, pixelHeight, pixelWidth * 4,
                   PixelFormat32bppPARGB,
                   static_cast<BYTE*>(g_renderBits));
    Graphics g(&surface);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    const REAL scale = static_cast<REAL>(g_dpi) / 96.0f;
    g.ScaleTransform(scale, scale);

    const BYTE backgroundR = GetRValue(g_settings.background);
    const BYTE backgroundG = GetGValue(g_settings.background);
    const BYTE backgroundB = GetBValue(g_settings.background);
    const BYTE accentR = GetRValue(g_settings.accent);
    const BYTE accentG = GetGValue(g_settings.accent);
    const BYTE accentB = GetBValue(g_settings.accent);

    if (!g_panelExpanded) {
        const REAL width = static_cast<REAL>(g_settings.capsuleWidth);
        const REAL height = static_cast<REAL>(kCapsuleHeight);
        RoundRectFill(g, RectF(1, 2, width - 2, height - 2),
                      (height - 2) / 2.0f, Color(90, 0, 0, 0));
        RoundRectGradient(
            g, RectF(1, 0, width - 2, height - 3), (height - 3) / 2.0f,
            g_capsuleHover
                ? Color(255, std::min(255, backgroundR + 18),
                        std::min(255, backgroundG + 18),
                        std::min(255, backgroundB + 18))
                : Color(255, std::min(255, backgroundR + 10),
                        std::min(255, backgroundG + 10),
                        std::min(255, backgroundB + 10)),
            Color(255, backgroundR, backgroundG, backgroundB));
        RoundRectStroke(g, RectF(1.5f, 0.5f, width - 3, height - 4),
                        (height - 4) / 2.0f,
                        g_capsuleHover
                            ? Color(150, accentR, accentG, accentB)
                            : Color(62, 255, 255, 255),
                        1.0f);
        const REAL indicatorWidth = g_capsuleHover ? 72.0f : 48.0f;
        RoundRectFill(g,
                      RectF((width - indicatorWidth) / 2.0f, 7,
                            indicatorWidth, g_capsuleHover ? 3.5f : 3.0f),
                      2.0f, Color(235, accentR, accentG, accentB));
        PresentRenderSurface(pixelWidth, pixelHeight);
        return;
    }

    RoundRectFill(g, RectF(2, 4, (REAL)g_width - 4,
                           (REAL)g_height - 4),
                  (REAL)g_settings.radius, Color(115, 0, 0, 0));
    RoundRectGradient(
        g, RectF(1, 0, (REAL)g_width - 2, (REAL)g_height - 4),
        (REAL)g_settings.radius,
        Color(255, std::min(255, backgroundR + 11),
              std::min(255, backgroundG + 11),
              std::min(255, backgroundB + 11)),
        Color(255, std::max(0, backgroundR - 3),
              std::max(0, backgroundG - 3),
              std::max(0, backgroundB - 3)));
    RoundRectStroke(g,
                    RectF(1.5f, 0.5f, (REAL)g_width - 3,
                          (REAL)g_height - 5),
                    (REAL)g_settings.radius - 0.5f,
                    Color(62, 255, 255, 255), 1.0f);

    const REAL handleX = (REAL)g_width / 2.0f - 54.0f;
    RoundRectFill(g, RectF(handleX, 4, 108, 16), 8,
                  g_capsuleHover ? Color(45, accentR, accentG, accentB)
                                 : Color(26, 255, 255, 255));
    RoundRectStroke(g, RectF(handleX + 0.5f, 4.5f, 107, 15), 7.5f,
                    g_capsuleHover ? Color(115, accentR, accentG, accentB)
                                   : Color(28, 255, 255, 255),
                    1.0f);
    RoundRectFill(g, RectF((REAL)g_width / 2.0f - 23, 10, 46, 3),
                  1.5f, Color(220, accentR, accentG, accentB));

    RoundRectFill(g, RectF(14, 25, (REAL)g_width - 28, 32), 14,
                  Color(20, 255, 255, 255));
    RoundRectStroke(g, RectF(14.5f, 25.5f, (REAL)g_width - 29, 31),
                    13.5f, Color(22, 255, 255, 255), 1.0f);
    DrawCentered(g, L"\u2039", RectF(18, 28, 32, 26), 20,
                 Color(220, 235, 238, 245));
    DrawCentered(g, CurrentPageName(),
                 RectF(54, 28, (REAL)g_width - 108, 26), 12,
                 Color(238, 242, 244, 249), true);
    DrawCentered(g, L"\u203a", RectF((REAL)g_width - 50, 28, 32, 26),
                 20, Color(220, 235, 238, 245));
    for (int i = 0; i < g_settings.count; ++i) {
        RECT rr = TileRect(i); bool hover = i == g_hover;
        float radius = g_settings.tileShape==TileShape::Circle ? g_settings.tile/2.0f :
                       g_settings.tileShape==TileShape::Square ? 5.0f : 14.0f;
        BYTE animAlpha=static_cast<BYTE>(std::clamp(g_pageAnimation,0.0f,1.0f)*255);
        COLORREF tileAccent=g_shortcuts[i].customColor?g_shortcuts[i].color:g_settings.accent;
        if(g_settings.tileShape!=TileShape::Minimal){
            RoundRectFill(g, RectF((REAL)rr.left, (REAL)rr.top+4, (REAL)g_settings.tile, (REAL)g_settings.tile), radius, Color(70*animAlpha/255,0,0,0));
            Color tileTop = hover ? Color(92*animAlpha/255, GetRValue(tileAccent), GetGValue(tileAccent), GetBValue(tileAccent)) : Color(42*animAlpha/255, 255,255,255);
            Color tileBottom = hover ? Color(52*animAlpha/255, GetRValue(tileAccent), GetGValue(tileAccent), GetBValue(tileAccent)) : Color(20*animAlpha/255, 255,255,255);
            RoundRectGradient(g, RectF((REAL)rr.left, (REAL)rr.top, (REAL)g_settings.tile, (REAL)g_settings.tile), radius, tileTop, tileBottom);
            RoundRectStroke(g, RectF((REAL)rr.left+0.5f, (REAL)rr.top+0.5f, (REAL)g_settings.tile-1, (REAL)g_settings.tile-1), std::max(1.0f,radius-0.5f),
                            hover ? Color(190, GetRValue(tileAccent), GetGValue(tileAccent), GetBValue(tileAccent)) : Color(42,255,255,255), 1.0f);
        }
        if (g_iconBitmaps[i]) {
            int iconSize = Clamp(g_settings.tile - (hover ? 20 : 24), 28, hover ? 56 : 52);
            int x = rr.left + (g_settings.tile - iconSize) / 2, y = rr.top + (g_settings.tile - iconSize) / 2;
            g.DrawImage(g_iconBitmaps[i].get(), x, y, iconSize, iconSize);
        } else {
            DrawCentered(g, L"+", RectF((REAL)rr.left, (REAL)rr.top-2, (REAL)g_settings.tile, (REAL)g_settings.tile), 30, Color(190,255,255,255));
        }
        if(!g_shortcuts[i].path.empty()&&g_runningWindows[i])
            RoundRectFill(g,RectF((REAL)rr.left+g_settings.tile/2.0f-9,(REAL)rr.bottom-5,18,3),1.5f,
                          Color(235,GetRValue(tileAccent),GetGValue(tileAccent),GetBValue(tileAccent)));
        if (g_settings.labels) DrawCentered(g, DisplayName(g_paths[i], i), RectF((REAL)rr.left-5, (REAL)rr.bottom+1, (REAL)g_settings.tile+10, 18), 11, Color(205,235,238,245));
    }
    int mt = MediaTop();
    RoundRectFill(g, RectF(18, (REAL)mt+4, (REAL)g_width-36,
                           (REAL)kMediaCardHeight),
                  18, Color(60,0,0,0));
    RoundRectGradient(g, RectF(18, (REAL)mt, (REAL)g_width-36,
                               (REAL)kMediaCardHeight),
                      18, Color(42,255,255,255),
                      Color(22,255,255,255));
    RoundRectStroke(g, RectF(18.5f, (REAL)mt+0.5f, (REAL)g_width-37,
                             (REAL)kMediaCardHeight-1),
                    17.5f, Color(34,255,255,255), 1.0f);
    RoundRectFill(g, RectF(22, (REAL)mt+18, 3, 42), 1.5f,
                  Color(220, GetRValue(g_settings.accent), GetGValue(g_settings.accent), GetBValue(g_settings.accent)));
    MediaState media; { std::lock_guard lock(g_mediaMutex); media = g_media; }
    std::wstring title = media.available
        ? (media.title.empty() ? (g_settings.french ? L"Lecture en cours" : L"Now playing") : media.title)
        : (g_settings.french ? L"Aucun m\u00E9dia actif" : L"No active media");
    std::wstring artist = media.available ? media.artist
        : (g_settings.french ? L"Lancez Spotify ou YouTube Music" : L"Start Spotify or YouTube Music");
    StringFormat left; left.SetAlignment(StringAlignmentNear); left.SetLineAlignment(StringAlignmentCenter); left.SetTrimming(StringTrimmingEllipsisCharacter);
    FontFamily ff(L"Segoe UI"); Font titleFont(&ff, 13, FontStyleBold, UnitPixel), artistFont(&ff, 11, FontStyleRegular, UnitPixel);
    SolidBrush white(Color(235,245,246,250)), muted(Color(150,190,195,205));
    float textX=34;
    if(DrawImageBytes(g,media.artwork,RectF(31,(REAL)mt+10,52,52))) textX=94;
    g.DrawString(title.c_str(), -1, &titleFont, RectF(textX,(REAL)mt+12,(REAL)g_width-textX-154,22), &left, &white);
    g.DrawString(artist.c_str(), -1, &artistFont, RectF(textX,(REAL)mt+35,(REAL)g_width-textX-154,18), &left, &muted);
    int cx = g_width - 102;
    DrawCentered(g, L"|<", RectF((REAL)cx-54,(REAL)mt+18,32,36), 13, Color(235,245,246,250));
    RoundRectFill(g, RectF((REAL)cx-16,(REAL)mt+16,40,40), 20, Color(220, GetRValue(g_settings.accent), GetGValue(g_settings.accent), GetBValue(g_settings.accent)));
    DrawCentered(g, media.playing ? L"||" : L">", RectF((REAL)cx-16,(REAL)mt+16,40,40), 14, Color(255,255,255,255), true);
    DrawCentered(g, L">|", RectF((REAL)cx+30,(REAL)mt+18,32,36), 13, Color(235,245,246,250));
    const int64_t durationTicks = media.endTicks - media.startTicks;
    float progress=durationTicks>0?std::clamp(static_cast<float>(media.positionTicks-media.startTicks)/durationTicks,0.0f,1.0f):0.0f;
    RoundRectFill(g,RectF(32,(REAL)mt+72,(REAL)g_width-64,3),1.5f,Color(48,255,255,255));
    if(progress>0)RoundRectFill(g,RectF(32,(REAL)mt+72,((REAL)g_width-64)*progress,3),1.5f,
                               Color(220,GetRValue(g_settings.accent),GetGValue(g_settings.accent),GetBValue(g_settings.accent)));
    Font versionFont(&ff, 9, FontStyleRegular, UnitPixel);
    StringFormat versionFormat; versionFormat.SetAlignment(StringAlignmentFar); versionFormat.SetLineAlignment(StringAlignmentCenter);
    SolidBrush versionBrush(Color(210,255,255,255));
    g.DrawString(L"v" WH_MOD_VERSION, -1, &versionFont, RectF(0,(REAL)g_height-19,(REAL)g_width-12,14), &versionFormat, &versionBrush);
    PresentRenderSurface(pixelWidth, pixelHeight);
}

struct PromptData {
    std::wstring title, prompt, value;
    HWND edit = nullptr;
    bool accepted = false;
};

LRESULT CALLBACK PromptWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* data = reinterpret_cast<PromptData*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        data = reinterpret_cast<PromptData*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));
    }
    switch (msg) {
    case WM_CREATE:
        CreateWindowExW(0, L"STATIC", data->prompt.c_str(), WS_CHILD|WS_VISIBLE,
                        14, 12, 330, 20, hwnd, nullptr, nullptr, nullptr);
        data->edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", data->value.c_str(),
                        WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,
                        14, 36, 330, 25, hwnd, reinterpret_cast<HMENU>(100), nullptr, nullptr);
        CreateWindowExW(0, L"BUTTON", L"OK", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON,
                        184, 73, 76, 26, hwnd, reinterpret_cast<HMENU>(IDOK), nullptr, nullptr);
        CreateWindowExW(0, L"BUTTON", g_settings.french ? L"Annuler" : L"Cancel", WS_CHILD|WS_VISIBLE|WS_TABSTOP,
                        268, 73, 76, 26, hwnd, reinterpret_cast<HMENU>(IDCANCEL), nullptr, nullptr);
        SendMessageW(data->edit, EM_SETSEL, 0, -1); SetFocus(data->edit); return 0;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            int len = GetWindowTextLengthW(data->edit);
            std::vector<wchar_t> text(len + 1);
            GetWindowTextW(data->edit, text.data(), len + 1);
            data->value = text.data(); data->accepted = true; DestroyWindow(hwnd); return 0;
        }
        if (LOWORD(wParam) == IDCANCEL) { DestroyWindow(hwnd); return 0; }
        break;
    case WM_CLOSE: DestroyWindow(hwnd); return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

class ModalGuard {
public:
    ModalGuard() { g_modalActive = true; }
    ~ModalGuard() {
        g_modalActive = false;
        if (!g_stopEvent ||
            WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
            EnsurePanelWindow();
        }
    }
};

int ShowPopupMenu(HMENU menu, POINT screen) {
    ModalGuard modalGuard;
    HWND owner = g_menuOwnerWnd ? g_menuOwnerWnd : g_hwnd.load();
    if (g_menuOwnerWnd) {
        SetWindowPos(g_menuOwnerWnd, nullptr, -32000, -32000, 1, 1,
                     SWP_NOZORDER | SWP_SHOWWINDOW);
        SetForegroundWindow(g_menuOwnerWnd);
    }
    int command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                 screen.x, screen.y, 0, owner, nullptr);
    if (owner) PostMessageW(owner, WM_NULL, 0, 0);
    if (g_menuOwnerWnd) ShowWindow(g_menuOwnerWnd, SW_HIDE);
    return command;
}

bool PromptText(const std::wstring& title, const std::wstring& prompt, std::wstring& value) {
    PromptData data{title, prompt, value};
    RECT owner{}; GetWindowRect(g_hwnd, &owner);
    HWND dialog = CreateWindowExW(WS_EX_DLGMODALFRAME|WS_EX_TOPMOST,
        kPromptClassName, title.c_str(), WS_CAPTION|WS_SYSMENU|WS_POPUP,
        owner.left + (owner.right-owner.left-370)/2, owner.top + 40, 370, 140,
        g_hwnd, nullptr, g_hInst, &data);
    if (!dialog) return false;
    ModalGuard modalGuard;
    g_modalDialog = dialog;
    EnableWindow(g_hwnd, FALSE); ShowWindow(dialog, SW_SHOW); UpdateWindow(dialog);
    MSG msg{};
    while (IsWindow(dialog)) {
        BOOL result = GetMessageW(&msg, nullptr, 0, 0);
        if (result <= 0) {
            if (result == 0) PostQuitMessage(static_cast<int>(msg.wParam));
            break;
        }
        if (!IsDialogMessageW(dialog, &msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    }
    g_modalDialog = nullptr;
    if (g_hwnd) EnableWindow(g_hwnd, TRUE);
    value = data.value;
    return data.accepted;
}

bool ChooseFile(std::wstring& path, const wchar_t* filter = L"All files\0*.*\0\0") {
    std::vector<wchar_t> buffer(32768);
    if (!path.empty()) wcsncpy_s(buffer.data(), buffer.size(), path.c_str(), _TRUNCATE);
    OPENFILENAMEW ofn{}; ofn.lStructSize=sizeof(ofn); ofn.hwndOwner=g_hwnd;
    ofn.lpstrFile=buffer.data(); ofn.nMaxFile=static_cast<DWORD>(buffer.size()); ofn.lpstrFilter=filter;
    ofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_EXPLORER;
    ModalGuard modalGuard;
    if (!GetOpenFileNameW(&ofn)) return false;
    path=buffer.data(); return true;
}

bool ChooseFolder(std::wstring& path) {
    BROWSEINFOW bi{}; bi.hwndOwner=g_hwnd;
    bi.lpszTitle=g_settings.french ? L"Choisir un dossier de raccourcis" : L"Choose a shortcut folder";
    bi.ulFlags=BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
    ModalGuard modalGuard;
    PIDLIST_ABSOLUTE pidl=SHBrowseForFolderW(&bi); if(!pidl) return false;
    wchar_t buffer[MAX_PATH]{}; bool ok=SHGetPathFromIDListW(pidl,buffer)!=FALSE;
    CoTaskMemFree(pidl); if(ok) path=buffer; return ok;
}

void ChooseShortcutColor(int slot) {
    static COLORREF custom[16]{};
    CHOOSECOLORW cc{}; cc.lStructSize=sizeof(cc); cc.hwndOwner=g_hwnd;
    cc.rgbResult=g_shortcuts[slot].customColor?g_shortcuts[slot].color:g_settings.accent;
    cc.lpCustColors=custom; cc.Flags=CC_FULLOPEN|CC_RGBINIT;
    ModalGuard modalGuard;
    if(ChooseColorW(&cc)){g_shortcuts[slot].color=cc.rgbResult;g_shortcuts[slot].customColor=true;SaveShortcut(slot);Render();}
}

std::wstring ResolveExecutable(const std::wstring& path) {
    if (_wcsicmp(PathFindExtensionW(path.c_str()), L".lnk") != 0) return path;
    winrt::com_ptr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(link.put())))) return path;
    auto persist = link.as<IPersistFile>();
    if (FAILED(persist->Load(path.c_str(), STGM_READ))) return path;
    wchar_t target[MAX_PATH]{}; WIN32_FIND_DATAW data{};
    if (FAILED(link->GetPath(target, ARRAYSIZE(target), &data, SLGP_RAWPATH))) return path;
    return target[0] ? target : path;
}

struct FindWindowData { std::wstring exeName; HWND found=nullptr; };
BOOL CALLBACK FindRunningWindowProc(HWND hwnd, LPARAM lp) {
    auto* data=reinterpret_cast<FindWindowData*>(lp);
    if(!IsWindowVisible(hwnd)||GetWindow(hwnd,GW_OWNER)) return TRUE;
    DWORD pid=0; GetWindowThreadProcessId(hwnd,&pid); HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
    if(!process) return TRUE; wchar_t path[MAX_PATH]; DWORD chars=ARRAYSIZE(path);
    bool match=QueryFullProcessImageNameW(process,0,path,&chars)&&_wcsicmp(PathFindFileNameW(path),data->exeName.c_str())==0;
    CloseHandle(process); if(match){data->found=hwnd;return FALSE;} return TRUE;
}

BOOL CALLBACK RefreshRunningWindowsProc(HWND hwnd, LPARAM) {
    if (!IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER)) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return TRUE;
    DWORD chars = static_cast<DWORD>(g_processPathBuffer.size());
    bool queried = QueryFullProcessImageNameW(
        process, 0, g_processPathBuffer.data(), &chars);
    CloseHandle(process);
    if (!queried) return TRUE;
    for (int i = 0; i < g_settings.count; ++i) {
        const std::wstring& target = g_shortcuts[i].resolvedPath;
        if (target.empty() ||
            _wcsicmp(PathFindExtensionW(target.c_str()), L".exe") != 0) {
            continue;
        }
        const bool fullPathMatch =
            _wcsicmp(g_processPathBuffer.data(), target.c_str()) == 0;
        const bool shortcutFileNameMatch =
            g_shortcuts[i].allowFileNameMatch &&
            _wcsicmp(PathFindFileNameW(g_processPathBuffer.data()),
                     PathFindFileNameW(target.c_str())) == 0;
        if (fullPathMatch || shortcutFileNameMatch) {
            if (!g_runningWindows[i]) g_runningWindows[i] = hwnd;
        }
    }
    return TRUE;
}

bool RefreshRunningWindows() {
    const auto previous = g_runningWindows;
    g_runningWindows.fill(nullptr);
    EnumWindows(RefreshRunningWindowsProc, 0);
    return previous != g_runningWindows;
}

bool IsDesktopForeground() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) return true;
    foreground = GetAncestor(foreground, GA_ROOT);
    DWORD processId = 0;
    GetWindowThreadProcessId(foreground, &processId);
    if (processId == GetCurrentProcessId()) {
        return g_desktopVisible.load();
    }
    if (foreground == GetShellWindow()) return true;

    wchar_t className[64]{};
    GetClassNameW(foreground, className, ARRAYSIZE(className));
    return _wcsicmp(className, L"Progman") == 0 ||
           _wcsicmp(className, L"WorkerW") == 0;
}

void SyncActiveState() {
    if (g_modalActive) return;
    const bool active = IsDesktopForeground();
    const bool changed = g_desktopVisible.exchange(active) != active;
    if (changed && g_activityEvent) SetEvent(g_activityEvent);
    if (!active) return;
    if (changed) g_lastRunningWindowSweep = 0;

    const ULONGLONG now = GetTickCount64();
    if (g_lastRunningWindowSweep == 0 ||
        now - g_lastRunningWindowSweep >= 15000) {
        g_lastRunningWindowSweep = now;
        if (RefreshRunningWindows()) Render();
    }
}

void CALLBACK ForegroundChangedProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG,
                                    DWORD, DWORD) {
    HWND sink = g_sinkWnd;
    if (sink) PostMessageW(sink, WM_APP_ACTIVE_STATE, 0, 0);
}

void ShowFolderContents(int slot, POINT screen) {
    HMENU menu=CreatePopupMenu(); std::vector<std::wstring> entries;
    try {
        for(const auto& item:std::filesystem::directory_iterator(g_shortcuts[slot].path)){
            if(entries.size()>=30) break;
            entries.push_back(item.path().wstring());
            AppendMenuW(menu,MF_STRING,500+entries.size()-1,item.path().filename().c_str());
        }
    } catch(...) {}
    if(entries.empty()) AppendMenuW(menu,MF_GRAYED,0,g_settings.french?L"Dossier vide":L"Empty folder");
    int cmd=ShowPopupMenu(menu,screen);
    DestroyMenu(menu); if(cmd>=500&&static_cast<size_t>(cmd-500)<entries.size())
        ShellExecuteW(g_hwnd,L"open",entries[cmd-500].c_str(),nullptr,nullptr,SW_SHOWNORMAL);
}

void LaunchSlot(int slot) {
    if(slot<0||slot>=kMaxShortcuts||g_shortcuts[slot].path.empty()) return;
    DWORD attributes=GetFileAttributesW(g_shortcuts[slot].path.c_str());
    if(attributes!=INVALID_FILE_ATTRIBUTES&&(attributes&FILE_ATTRIBUTE_DIRECTORY)){POINT p{};GetCursorPos(&p);ShowFolderContents(slot,p);return;}
    if(HWND running=g_runningWindows[slot]){
        if(IsIconic(running)) ShowWindow(running,SW_RESTORE); SetForegroundWindow(running); return;
    }
    ShellExecuteW(g_hwnd,g_shortcuts[slot].runAsAdmin?L"runas":L"open",g_shortcuts[slot].path.c_str(),
                  g_shortcuts[slot].arguments.empty()?nullptr:g_shortcuts[slot].arguments.c_str(),nullptr,SW_SHOWNORMAL);
}

void ReloadShortcutVisuals() { LoadAssignments(); Render(); }

void ShowTileContextMenu(int slot, POINT screen) {
    HMENU menu=CreatePopupMenu();
    if(!g_shortcuts[slot].path.empty()){
        AppendMenuW(menu,MF_STRING,19,g_settings.french?L"Supprimer le raccourci":L"Remove shortcut");
        AppendMenuW(menu,MF_SEPARATOR,0,nullptr);
    }
    AppendMenuW(menu,MF_STRING,10,g_settings.french?L"Choisir une application...":L"Choose application...");
    AppendMenuW(menu,MF_STRING,11,g_settings.french?L"Choisir un dossier/groupe...":L"Choose folder/group...");
    AppendMenuW(menu,MF_SEPARATOR,0,nullptr);
    AppendMenuW(menu,MF_STRING,12,g_settings.french?L"Modifier le nom...":L"Edit label...");
    AppendMenuW(menu,MF_STRING,13,g_settings.french?L"Arguments de lancement...":L"Launch arguments...");
    AppendMenuW(menu,MF_STRING,14,g_settings.french?L"Ic\u00f4ne personnalis\u00e9e...":L"Custom icon...");
    AppendMenuW(menu,MF_STRING,15,g_settings.french?L"Couleur personnalis\u00e9e...":L"Custom color...");
    if(g_shortcuts[slot].customColor) AppendMenuW(menu,MF_STRING,16,g_settings.french?L"R\u00e9initialiser la couleur":L"Reset color");
    AppendMenuW(menu,MF_STRING|(g_shortcuts[slot].runAsAdmin?MF_CHECKED:0),17,
                g_settings.french?L"Ex\u00e9cuter en administrateur":L"Run as administrator");
    if(!g_shortcuts[slot].path.empty()){
        AppendMenuW(menu,MF_STRING,18,g_settings.french?L"Ouvrir l'emplacement":L"Open file location");
    }
    int cmd=ShowPopupMenu(menu,screen);DestroyMenu(menu);
    if(cmd==10){std::wstring path=g_shortcuts[slot].path;if(ChooseFile(path)){g_shortcuts[slot].path=path;SaveShortcut(slot);ReloadShortcutVisuals();}}
    else if(cmd==11){std::wstring path=g_shortcuts[slot].path;if(ChooseFolder(path)){g_shortcuts[slot].path=path;SaveShortcut(slot);ReloadShortcutVisuals();}}
    else if(cmd==12){std::wstring value=g_shortcuts[slot].label;if(PromptText(g_settings.french?L"Nom du raccourci":L"Shortcut label",g_settings.french?L"Nom affich\u00e9 :":L"Displayed name:",value)){g_shortcuts[slot].label=value;SaveShortcut(slot);Render();}}
    else if(cmd==13){std::wstring value=g_shortcuts[slot].arguments;if(PromptText(g_settings.french?L"Arguments":L"Arguments",g_settings.french?L"Arguments de lancement :":L"Launch arguments:",value)){g_shortcuts[slot].arguments=value;SaveShortcut(slot);}}
    else if(cmd==14){std::wstring value=g_shortcuts[slot].iconPath;if(ChooseFile(value,L"Icons and programs\0*.ico;*.exe;*.dll\0All files\0*.*\0\0")){g_shortcuts[slot].iconPath=value;SaveShortcut(slot);ReloadShortcutVisuals();}}
    else if(cmd==15) ChooseShortcutColor(slot);
    else if(cmd==16){g_shortcuts[slot].customColor=false;SaveShortcut(slot);Render();}
    else if(cmd==17){g_shortcuts[slot].runAsAdmin=!g_shortcuts[slot].runAsAdmin;SaveShortcut(slot);}
    else if(cmd==18){std::wstring args=L"/select,\""+g_shortcuts[slot].path+L"\"";ShellExecuteW(g_hwnd,L"open",L"explorer.exe",args.c_str(),nullptr,SW_SHOWNORMAL);}
    else if(cmd==19){ClearShortcutOverride(slot);ReloadShortcutVisuals();}
}

void SwapShortcuts(int a,int b) {
    if(a<0||b<0||a>=kMaxShortcuts||b>=kMaxShortcuts||a==b)return;
    std::swap(g_shortcuts[a],g_shortcuts[b]);SaveShortcut(a);SaveShortcut(b);ReloadShortcutVisuals();
}

bool SelectBackupFile(bool save,std::wstring& path) {
    std::vector<wchar_t> buffer(32768); wcscpy_s(buffer.data(),buffer.size(),L"quick-launch-media-panel-backup.ini");
    OPENFILENAMEW ofn{};ofn.lStructSize=sizeof(ofn);ofn.hwndOwner=g_hwnd;ofn.lpstrFile=buffer.data();ofn.nMaxFile=static_cast<DWORD>(buffer.size());
    ofn.lpstrFilter=L"Panel backup (*.ini)\0*.ini\0All files\0*.*\0\0";ofn.lpstrDefExt=L"ini";
    ofn.Flags=OFN_EXPLORER|OFN_PATHMUSTEXIST|(save?OFN_OVERWRITEPROMPT:OFN_FILEMUSTEXIST);
    ModalGuard modalGuard;
    bool ok=save?GetSaveFileNameW(&ofn)!=FALSE:GetOpenFileNameW(&ofn)!=FALSE;if(ok)path=buffer.data();return ok;
}

void ExportLayout() {std::wstring path;if(SelectBackupFile(true,path))CopyFileW(StateFile().c_str(),path.c_str(),FALSE);}
void ImportLayout() {std::wstring path;if(SelectBackupFile(false,path)&&CopyFileW(path.c_str(),StateFile().c_str(),FALSE)){LoadPersistentUiState();LoadAssignments();LayoutMetrics();PositionWindow();Render();}}

void QueueMediaAction(MediaAction action) {
    {
        std::lock_guard lock(g_mediaActionMutex);
        g_mediaActions.push_back(action);
    }
    if (g_mediaActionEvent) SetEvent(g_mediaActionEvent);
}

void MediaCommand(int command) {
    MediaActionKind kind = MediaActionKind::Toggle;
    if (command == 0) kind = MediaActionKind::Previous;
    else if (command == 2) kind = MediaActionKind::Next;
    QueueMediaAction({kind});
}

void MediaSeek(double fraction) {
    QueueMediaAction({MediaActionKind::Seek,
                      std::clamp(fraction, 0.0, 1.0)});
}

void ChangeMasterVolume(float delta) {
    QueueMediaAction({MediaActionKind::Volume, delta});
}

void ApplyMasterVolume(float delta) {
    winrt::com_ptr<IMMDeviceEnumerator> enumerator;
    if(FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,IID_PPV_ARGS(enumerator.put()))))return;
    winrt::com_ptr<IMMDevice> device;if(FAILED(enumerator->GetDefaultAudioEndpoint(eRender,eMultimedia,device.put())))return;
    winrt::com_ptr<IAudioEndpointVolume> volume;if(FAILED(device->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,nullptr,volume.put_void())))return;
    float value=0;if(SUCCEEDED(volume->GetMasterVolumeLevelScalar(&value)))volume->SetMasterVolumeLevelScalar(std::clamp(value+delta,0.0f,1.0f),nullptr);
}

void BringMediaAppToFront() {
    std::wstring id;{std::lock_guard lock(g_mediaMutex);id=g_media.sourceId;}
    std::wstring lower=id;std::transform(lower.begin(),lower.end(),lower.begin(),towlower);
    const wchar_t* candidates[]={L"spotify.exe",L"chrome.exe",L"msedge.exe",L"vivaldi.exe",L"firefox.exe"};
    for(const wchar_t* exe:candidates){if(lower.find(std::wstring(exe,exe+wcslen(exe)-4))!=std::wstring::npos){FindWindowData d{exe};EnumWindows(FindRunningWindowProc,reinterpret_cast<LPARAM>(&d));if(d.found){if(IsIconic(d.found))ShowWindow(d.found,SW_RESTORE);SetForegroundWindow(d.found);return;}}}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_NCHITTEST: return HTCLIENT;
    case WM_LBUTTONDOWN: {
        g_ignoreNextButtonUp = false;
        POINT p=ToLogicalPoint({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
        RECT capsule = CapsuleToggleRect();
        if (PtInRect(&capsule, p)) {
            TogglePanelExpanded();
            return 0;
        }
        if (!g_panelExpanded) return 0;
        int mt = MediaTop(), cx = g_width - 102;
        int tile=HitTile(p);
        if(tile>=0){g_pressedTile=tile;g_tilePressPoint=p;g_reordering=false;SetCapture(hwnd);return 0;}
        bool onMediaControl = p.y >= mt+10 && p.y <= mt+64 && p.x >= cx-58 && p.x <= cx+66;
        RECT timeline = MediaTimelineRect();
        bool onTimeline = PtInRect(&timeline, p);
        RECT previousPage=PreviousPageRect(),nextPage=NextPageRect();
        bool onPageControl=PtInRect(&previousPage,p)||PtInRect(&nextPage,p);
        if (!onMediaControl && !onTimeline && !onPageControl &&
            !g_settings.lockPosition) {
            g_dragging = true;
            GetCursorPos(&g_dragStart);
            GetWindowRect(hwnd, &g_dragWindowStart);
            SetCapture(hwnd);
        } else if (!onMediaControl && !onTimeline && !onPageControl &&
                   g_settings.lockPosition) {
            g_ignoreNextButtonUp = true;
        }
        return 0;
    }
    case WM_MOUSEMOVE: {
        POINT pointer = ToLogicalPoint(
            {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
        RECT capsule = CapsuleToggleRect();
        bool capsuleHover = PtInRect(&capsule, pointer) != FALSE;
        if (capsuleHover != g_capsuleHover) {
            g_capsuleHover = capsuleHover;
            Render();
        }
        if (!g_panelExpanded) {
            TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&track);
            return 0;
        }
        if(g_pressedTile>=0){
            POINT p=ToLogicalPoint({GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)});
            if(abs(p.x-g_tilePressPoint.x)>7||abs(p.y-g_tilePressPoint.y)>7)g_reordering=true;
            int hit=HitTile(p);if(hit!=g_hover){g_hover=hit;Render();}return 0;
        }
        if (g_dragging) {
            POINT current{};
            GetCursorPos(&current);
            int x = g_dragWindowStart.left + current.x - g_dragStart.x;
            int y = g_dragWindowStart.top + current.y - g_dragStart.y;
            const int width = PixelWidth();
            const int height = PixelHeight();
            RECT proposed{x, y, x + width, y + height};
            HMONITOR monitor = MonitorFromRect(&proposed, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);
            if (GetMonitorInfoW(monitor, &mi)) {
                x = Clamp(x, mi.rcWork.left,
                          std::max(mi.rcWork.left, mi.rcWork.right - width));
                y = Clamp(y, mi.rcWork.top,
                          std::max(mi.rcWork.top, mi.rcWork.bottom - height));
            }
            SetWindowPos(hwnd, nullptr, x, y, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            return 0;
        }
        if (g_ignoreNextButtonUp) {
            g_ignoreNextButtonUp = false;
            return 0;
        }
        POINT p=ToLogicalPoint({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}); int hit = HitTile(p);
        if (hit != g_hover) { g_hover = hit; Render(); }
        TRACKMOUSEEVENT t{sizeof(t),TME_LEAVE,hwnd,0}; TrackMouseEvent(&t); return 0;
    }
    case WM_MOUSELEAVE:
        g_hover = -1;
        g_capsuleHover = false;
        Render();
        return 0;
    case WM_LBUTTONUP: {
        if(g_pressedTile>=0){
            int source=g_pressedTile;POINT p=ToLogicalPoint({GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)});int target=HitTile(p);
            g_pressedTile=-1;ReleaseCapture();
            if(g_reordering&&target>=0&&target!=source)SwapShortcuts(source,target);else if(!g_reordering)LaunchSlot(source);
            g_reordering=false;return 0;
        }
        if (g_dragging) {
            g_dragging = false;
            ReleaseCapture();
            SaveWindowPosition();
            return 0;
        }
        if (!g_panelExpanded) return 0;
        POINT p=ToLogicalPoint({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}); int tile = HitTile(p);
        RECT prev=PreviousPageRect(),next=NextPageRect();
        if(PtInRect(&prev,p)){SwitchPage(-1);return 0;}if(PtInRect(&next,p)){SwitchPage(1);return 0;}
        if (tile >= 0) { LaunchSlot(tile); return 0; }
        int mt = MediaTop(), cx = g_width - 102;
        RECT timeline = MediaTimelineRect();
        if(PtInRect(&timeline,p)){MediaSeek((p.x-32.0)/(g_width-64.0));return 0;}
        if (p.y >= mt+10 && p.y <= mt+64) {
            if (p.x >= cx-58 && p.x < cx-20) MediaCommand(0);
            else if (p.x >= cx-20 && p.x < cx+28) MediaCommand(1);
            else if (p.x >= cx+28 && p.x <= cx+66) MediaCommand(2);
            else BringMediaAppToFront();
        } return 0;
    }
    case WM_RBUTTONUP: {
        POINT screen{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};ClientToScreen(hwnd,&screen);
        POINT p=ToLogicalPoint({GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)}); int tile=HitTile(p);
        if(tile>=0){ShowTileContextMenu(tile,screen);return 0;}
        RECT mediaCard=MediaCardRect();
        if(g_panelExpanded&&PtInRect(&mediaCard,p)){
            MediaState media;{std::lock_guard lock(g_mediaMutex);media=g_media;}
            HMENU sessions=CreatePopupMenu();AppendMenuW(sessions,MF_STRING,299,g_settings.french?L"Session automatique":L"Automatic session");
            for(size_t i=0;i<media.sessions.size()&&i<30;i++)AppendMenuW(sessions,MF_STRING,300+i,media.sessions[i].second.c_str());
            int command=ShowPopupMenu(sessions,screen);DestroyMenu(sessions);
            {std::lock_guard lock(g_mediaMutex);if(command==299)g_selectedMediaSession.clear();else if(command>=300&&static_cast<size_t>(command-300)<media.sessions.size())g_selectedMediaSession=media.sessions[command-300].first;}
            return 0;
        }
        HMENU menu = CreatePopupMenu();
        AppendMenuW(menu, MF_STRING, 1, g_settings.french ? L"R\u00E9duire les carr\u00E9s" : L"Smaller tiles");
        AppendMenuW(menu, MF_STRING, 2, g_settings.french ? L"Agrandir les carr\u00E9s" : L"Larger tiles");
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu, MF_STRING, 3, g_settings.french ? L"Recentrer le panneau" : L"Recenter panel");
        AppendMenuW(menu, MF_STRING, 4, g_settings.french ? L"Ouvrir les param\u00E8tres" : L"Open settings");
        AppendMenuW(menu, MF_STRING|(g_settings.lockPosition?MF_CHECKED:0), 5, g_settings.french ? L"Verrouiller la position" : L"Lock position");
        AppendMenuW(menu, MF_STRING, 6, g_settings.french ? L"Renommer cette page..." : L"Rename this page...");
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu, MF_STRING, 7, g_settings.french ? L"Exporter la disposition..." : L"Export layout...");
        AppendMenuW(menu, MF_STRING, 8, g_settings.french ? L"Importer une disposition..." : L"Import layout...");
        int command = ShowPopupMenu(menu, screen);
        DestroyMenu(menu);
        if (command == 1) ResizeTiles(-4);
        else if (command == 2) ResizeTiles(4);
        else if (command == 3) { ClearSavedPosition(); PositionWindow(); Render(); }
        else if (command == 4) OpenWindhawk();
        else if(command==5){g_settings.lockPosition=!g_settings.lockPosition;SaveUiNumber(L"lockPosition",g_settings.lockPosition?1:0);}
        else if(command==6){std::wstring name=CurrentPageName();if(PromptText(g_settings.french?L"Nom de la page":L"Page name",g_settings.french?L"Nom :":L"Name:",name)){WriteIniString(PageSection(),L"name",name);g_currentPageName=name.empty()?L"Page "+std::to_wstring(g_currentPage+1):name;Render();}}
        else if(command==7)ExportLayout();else if(command==8)ImportLayout();
        return 0;
    }
    case WM_MOUSEWHEEL:
        if (!g_panelExpanded) return 0;
        if (GET_KEYSTATE_WPARAM(wParam) & MK_CONTROL) {
            ResizeTiles(GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 4 : -4);
            return 0;
        }
        {POINT screen{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};POINT client=screen;ScreenToClient(hwnd,&client);client=ToLogicalPoint(client);RECT mediaCard=MediaCardRect();
        if(PtInRect(&mediaCard,client))ChangeMasterVolume(GET_WHEEL_DELTA_WPARAM(wParam)>0?0.05f:-0.05f);else SwitchPage(GET_WHEEL_DELTA_WPARAM(wParam)>0?-1:1);return 0;}
    case WM_TIMER:
        if(wParam==7){g_pageAnimation=std::min(1.0f,g_pageAnimation+0.12f);Render();if(g_pageAnimation>=1.0f)KillTimer(hwnd,7);}return 0;
    case WM_CAPTURECHANGED: g_dragging = false;g_pressedTile=-1;g_reordering=false; return 0;
    case WM_APP_MEDIA: {
        if(g_settings.autoThemeSource==AutoThemeSource::Media)ApplyAutomaticTheme();
        Render();return 0;
    }
    case WM_DPICHANGED: {
        g_dpi = HIWORD(wParam);
        DestroyRenderSurface();
        const RECT* suggested = reinterpret_cast<const RECT*>(lParam);
        SetWindowPos(hwnd, nullptr, suggested->left, suggested->top,
                     PixelWidth(), PixelHeight(),
                     SWP_NOACTIVATE | SWP_NOZORDER);
        Render();
        return 0;
    }
    case WM_CLOSE: DestroyWindow(hwnd);return 0;
    case WM_DESTROY:
        if(g_dropRegistered){RevokeDragDrop(hwnd);g_dropRegistered=false;}
        if(g_dropTarget){g_dropTarget->Release();g_dropTarget=nullptr;}
        if(g_hwnd.load()==hwnd)g_hwnd.store(nullptr);
        g_desktopHost=nullptr;
        return 0;
    }
    return DefWindowProcW(hwnd,msg,wParam,lParam);
}

void DestroyPanelWindow() {
    HWND panel = g_hwnd.load();
    if (g_dropRegistered && panel) {
        RevokeDragDrop(panel);
        g_dropRegistered = false;
    }
    if (g_dropTarget) {
        g_dropTarget->Release();
        g_dropTarget = nullptr;
    }
    g_hwnd.store(nullptr);
    g_desktopHost = nullptr;
    if (panel && IsWindow(panel)) DestroyWindow(panel);
}

bool CreatePanelWindow(HWND desktopHost) {
    g_desktopHost = desktopHost;
    g_dpi = DpiForMonitor(SelectedMonitor());
    DestroyRenderSurface();
    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    HWND panel = CreateWindowExW(
        exStyle, kClassName, L"Quick Launch & Media Panel", WS_POPUP, 0, 0,
        PixelWidth(), PixelHeight(), desktopHost, nullptr, g_hInst, nullptr);
    if (!panel) {
        Wh_Log(L"CreateWindowExW failed, error=%u", GetLastError());
        g_desktopHost = nullptr;
        return false;
    }
    g_hwnd.store(panel);
    ChangeWindowMessageFilterEx(panel, WM_DROPFILES, MSGFLT_ALLOW, nullptr);
    ChangeWindowMessageFilterEx(panel, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
    ChangeWindowMessageFilterEx(panel, 0x0049, MSGFLT_ALLOW, nullptr);
    UINT windowDpi = GetDpiForWindow(panel);
    if (windowDpi) g_dpi = windowDpi;
    g_dropTarget = new (std::nothrow) PanelDropTarget();
    if (g_dropTarget) {
        HRESULT result = RegisterDragDrop(panel, g_dropTarget);
        g_dropRegistered = SUCCEEDED(result);
        if (FAILED(result)) {
            Wh_Log(L"RegisterDragDrop failed, error=0x%08X", result);
        }
    }
    PositionWindow();
    Render();
    RECT panelRect{};
    GetWindowRect(panel, &panelRect);
    wchar_t hostClass[64]{};
    GetClassNameW(desktopHost, hostClass, ARRAYSIZE(hostClass));
    Wh_Log(L"Panel window created, owner=%p (%s), rect=%d,%d-%d,%d, visible=%d",
           desktopHost, hostClass, panelRect.left, panelRect.top,
           panelRect.right, panelRect.bottom, IsWindowVisible(panel));
    return true;
}

void EnsurePanelWindow() {
    if (g_hwnd && IsWindow(g_hwnd) && IsWindow(g_desktopHost)) return;
    DestroyPanelWindow();
    HWND desktopHost = FindDesktopHost();
    if (desktopHost) CreatePanelWindow(desktopHost);
}

void ReloadSettingsOnUiThread() {
    LoadSettings();
    if (!StateFile().empty()) {
        WritePrivateProfileStringW(L"ui", L"tileSize", nullptr,
                                   StateFile().c_str());
        WritePrivateProfileStringW(L"ui", L"lockPosition", nullptr,
                                   StateFile().c_str());
    }
    g_dpi = DpiForMonitor(SelectedMonitor());
    LoadAssignments();
    ApplyAutomaticTheme();
    LayoutMetrics();
    DestroyRenderSurface();
    EnsurePanelWindow();
    PositionWindow();
    Render();
}

LRESULT CALLBACK SinkWindowProc(HWND hwnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == kDesktopTimer) {
                if (!g_modalActive) EnsurePanelWindow();
                SyncActiveState();
            }
            return 0;
        case WM_APP_ACTIVE_STATE:
            SyncActiveState();
            return 0;
        case WM_APP_RELOAD:
            ReloadSettingsOnUiThread();
            return 0;
        case WM_APP_QUIT:
            DestroyPanelWindow();
            PostQuitMessage(0);
            return 0;
        case WM_DISPLAYCHANGE:
            if (!g_modalActive) EnsurePanelWindow();
            PositionWindow();
            Render();
            return 0;
        case WM_SETTINGCHANGE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            if (ApplyAutomaticTheme()) Render();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI UiThreadProc(void*) {
    g_uiInitialized = false;
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT apartment = OleInitialize(nullptr);
    GdiplusStartupInput input;
    GdiplusStartup(&g_gdiplusToken, &input, nullptr);

    WNDCLASSEXW panelClass{};
    panelClass.cbSize = sizeof(panelClass);
    panelClass.lpfnWndProc = WindowProc;
    panelClass.hInstance = g_hInst;
    panelClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    panelClass.lpszClassName = kClassName;

    WNDCLASSEXW sinkClass{};
    sinkClass.cbSize = sizeof(sinkClass);
    sinkClass.lpfnWndProc = SinkWindowProc;
    sinkClass.hInstance = g_hInst;
    sinkClass.lpszClassName = kSinkClassName;

    WNDCLASSEXW promptClass{};
    promptClass.cbSize = sizeof(promptClass);
    promptClass.lpfnWndProc = PromptWindowProc;
    promptClass.hInstance = g_hInst;
    promptClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    promptClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    promptClass.lpszClassName = kPromptClassName;

    bool panelRegistered = RegisterClassExW(&panelClass) != 0;
    bool sinkRegistered = RegisterClassExW(&sinkClass) != 0;
    bool promptRegistered = RegisterClassExW(&promptClass) != 0;
    if (!panelRegistered || !sinkRegistered || !promptRegistered) {
        Wh_Log(L"RegisterClassExW failed, error=%u", GetLastError());
        if (promptRegistered) UnregisterClassW(kPromptClassName, g_hInst);
        if (sinkRegistered) UnregisterClassW(kSinkClassName, g_hInst);
        if (panelRegistered) UnregisterClassW(kClassName, g_hInst);
        if (g_gdiplusToken) GdiplusShutdown(g_gdiplusToken);
        if (SUCCEEDED(apartment)) OleUninitialize();
        if (g_readyEvent) SetEvent(g_readyEvent);
        return 1;
    }

    LoadPersistentUiState();
    g_dpi = DpiForMonitor(SelectedMonitor());
    LayoutMetrics();
    LoadAssignments();
    ApplyAutomaticTheme();

    g_sinkWnd = CreateWindowExW(WS_EX_TOOLWINDOW, kSinkClassName, L"",
                                WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                g_hInst, nullptr);
    if (!g_sinkWnd) {
        Wh_Log(L"Failed to create controller window, error=%u", GetLastError());
        PostQuitMessage(0);
    } else {
        g_menuOwnerWnd = CreateWindowExW(
            WS_EX_TOOLWINDOW, L"STATIC", L"", WS_POPUP, -32000, -32000, 1,
            1, nullptr, nullptr, g_hInst, nullptr);
        g_foregroundHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
            ForegroundChangedProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        SetTimer(g_sinkWnd, kDesktopTimer, 1000, nullptr);
        EnsurePanelWindow();
        SyncActiveState();
        g_uiInitialized = true;
    }
    if (g_readyEvent) SetEvent(g_readyEvent);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd && msg.message == WM_APP_QUIT) {
            PostQuitMessage(0);
            continue;
        }
        if (!msg.hwnd && msg.message == WM_APP_RELOAD) {
            ReloadSettingsOnUiThread();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    DestroyPanelWindow();
    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }
    if (g_menuOwnerWnd) {
        DestroyWindow(g_menuOwnerWnd);
        g_menuOwnerWnd = nullptr;
    }
    if (g_sinkWnd) {
        KillTimer(g_sinkWnd, kDesktopTimer);
        DestroyWindow(g_sinkWnd);
        g_sinkWnd = nullptr;
    }
    DestroyIcons();
    DestroyRenderSurface();
    UnregisterClassW(kPromptClassName, g_hInst);
    UnregisterClassW(kSinkClassName, g_hInst);
    UnregisterClassW(kClassName, g_hInst);
    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    if (SUCCEEDED(apartment)) OleUninitialize();
    return 0;
}

std::vector<uint8_t> ReadArtwork(
    const winrt::Windows::Storage::Streams::IRandomAccessStreamReference& reference) {
    std::vector<uint8_t> bytes;
    if(!reference)return bytes;
    auto stream=reference.OpenReadAsync().get();if(!stream)return bytes;
    uint64_t size64=stream.Size();if(size64==0||size64>8*1024*1024)return bytes;
    uint32_t size=static_cast<uint32_t>(size64);
    winrt::Windows::Storage::Streams::DataReader reader(stream.GetInputStreamAt(0));
    reader.LoadAsync(size).get();bytes.resize(size);
    reader.ReadBytes(winrt::array_view<uint8_t>(bytes.data(),bytes.data()+bytes.size()));return bytes;
}

std::wstring FriendlySourceName(const std::wstring& id) {
    std::wstring lower=id;std::transform(lower.begin(),lower.end(),lower.begin(),towlower);
    if(lower.find(L"spotify")!=std::wstring::npos)return L"Spotify";
    if(lower.find(L"vivaldi")!=std::wstring::npos)return L"Vivaldi";
    if(lower.find(L"chrome")!=std::wstring::npos)return L"Google Chrome";
    if(lower.find(L"msedge")!=std::wstring::npos)return L"Microsoft Edge";
    if(lower.find(L"firefox")!=std::wstring::npos)return L"Mozilla Firefox";
    return id;
}

using MediaSession = winrt::Windows::Media::Control::
    GlobalSystemMediaTransportControlsSession;

MediaSession SelectMediaSession(const MediaManager& manager) {
    if (!manager) return nullptr;
    std::wstring requested;
    {
        std::lock_guard lock(g_mediaMutex);
        requested = g_selectedMediaSession;
    }
    if (!requested.empty()) {
        for (const auto& candidate : manager.GetSessions()) {
            if (candidate.SourceAppUserModelId().c_str() == requested) {
                return candidate;
            }
        }
    }
    return manager.GetCurrentSession();
}

void ExecuteMediaActions(const MediaManager& manager) {
    std::vector<MediaAction> actions;
    {
        std::lock_guard lock(g_mediaActionMutex);
        actions.swap(g_mediaActions);
    }
    MediaSession session{nullptr};
    for (const MediaAction& action : actions) {
        if (action.kind == MediaActionKind::Volume) {
            ApplyMasterVolume(static_cast<float>(action.value));
            continue;
        }
        if (!session) session = SelectMediaSession(manager);
        if (!session) continue;
        try {
            switch (action.kind) {
                case MediaActionKind::Previous:
                    session.TrySkipPreviousAsync().get();
                    break;
                case MediaActionKind::Toggle:
                    session.TryTogglePlayPauseAsync().get();
                    break;
                case MediaActionKind::Next:
                    session.TrySkipNextAsync().get();
                    break;
                case MediaActionKind::Seek: {
                    auto timeline = session.GetTimelineProperties();
                    int64_t start = timeline.StartTime().count();
                    int64_t duration =
                        timeline.EndTime().count() - timeline.StartTime().count();
                    if (duration > 0) {
                        session.TryChangePlaybackPositionAsync(
                                   start + static_cast<int64_t>(duration * action.value))
                            .get();
                    }
                    break;
                }
                case MediaActionKind::Volume:
                    break;
            }
        } catch (...) {
            Wh_Log(L"Media command failed");
        }
    }
}

MediaState ReadMediaState(const MediaManager& manager) {
    MediaState next;
    if (!manager) return next;
    auto sessions = manager.GetSessions();
    std::wstring requested;
    {
        std::lock_guard lock(g_mediaMutex);
        requested = g_selectedMediaSession;
    }
    MediaSession selected{nullptr};
    for (const auto& candidate : sessions) {
        std::wstring id = candidate.SourceAppUserModelId().c_str();
        next.sessions.push_back({id, FriendlySourceName(id)});
        if (!requested.empty() && id == requested) selected = candidate;
    }
    if (!selected) selected = manager.GetCurrentSession();
    if (!selected) return next;

    auto properties = selected.TryGetMediaPropertiesAsync().get();
    auto playback = selected.GetPlaybackInfo();
    auto timeline = selected.GetTimelineProperties();
    next.available = true;
    next.playing = playback.PlaybackStatus() == PlaybackStatus::Playing;
    next.title = properties.Title().c_str();
    next.artist = properties.Artist().c_str();
    next.sourceId = selected.SourceAppUserModelId().c_str();
    next.startTicks = timeline.StartTime().count();
    next.positionTicks = timeline.Position().count();
    next.endTicks = timeline.EndTime().count();

    bool reuseArtwork = false;
    {
        std::lock_guard lock(g_mediaMutex);
        if (g_media.title == next.title && g_media.artist == next.artist &&
            g_media.sourceId == next.sourceId && !g_media.artwork.empty()) {
            next.artwork = g_media.artwork;
            reuseArtwork = true;
        }
    }
    if (!reuseArtwork) next.artwork = ReadArtwork(properties.Thumbnail());
    return next;
}

uint64_t ByteHash(const std::vector<uint8_t>& bytes) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint8_t byte : bytes) {
        hash ^= byte;
        hash *= 1099511628211ULL;
    }
    return hash;
}

int MediaProgressBucket(const MediaState& state) {
    const int64_t duration = state.endTicks - state.startTicks;
    if (!state.available || duration <= 0) return 0;
    const int64_t position =
        std::clamp(state.positionTicks - state.startTicks, int64_t{0}, duration);
    return static_cast<int>((position * 200) / duration);
}

bool MediaVisualStateChanged(const MediaState& before,
                             const MediaState& after) {
    if (before.available != after.available ||
        before.playing != after.playing || before.title != after.title ||
        before.artist != after.artist || before.sourceId != after.sourceId ||
        MediaProgressBucket(before) != MediaProgressBucket(after) ||
        before.artwork.size() != after.artwork.size()) {
        return true;
    }
    return !before.artwork.empty() &&
           ByteHash(before.artwork) != ByteHash(after.artwork);
}

DWORD WINAPI MediaThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    MediaManager manager{nullptr};
    HANDLE waits[] = {g_stopEvent, g_mediaActionEvent, g_activityEvent};
    for (;;) {
        const DWORD timeout = g_desktopVisible.load() ? 1000 : INFINITE;
        DWORD waitResult =
            WaitForMultipleObjects(ARRAYSIZE(waits), waits, FALSE, timeout);
        if (waitResult == WAIT_OBJECT_0) break;
        if (waitResult == WAIT_OBJECT_0 + 2 && !g_desktopVisible.load()) {
            continue;
        }

        bool visualChanged = false;
        try {
            if (!manager) manager = MediaManager::RequestAsync().get();
            ExecuteMediaActions(manager);
            MediaState next = ReadMediaState(manager);
            {
                std::lock_guard lock(g_mediaMutex);
                visualChanged = MediaVisualStateChanged(g_media, next);
                g_media = std::move(next);
            }
        } catch (...) {
            manager = nullptr;
            std::lock_guard lock(g_mediaMutex);
            visualChanged = g_media.available || !g_media.title.empty() ||
                            !g_media.artwork.empty();
            g_media = {};
        }
        HWND panel = g_hwnd.load();
        if (visualChanged && panel) {
            PostMessageW(panel, WM_APP_MEDIA, 0, 0);
        }
    }
    manager = nullptr;
    winrt::uninit_apartment();
    return 0;
}

HINSTANCE GetCurrentModuleHandle() {
    HINSTANCE instance = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
                       &instance);
    return instance;
}

BOOL CALLBACK CloseUiThreadWindow(HWND hwnd, LPARAM) {
    if (hwnd != g_sinkWnd) PostMessageW(hwnd, WM_CLOSE, 0, 0);
    return TRUE;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Quick Launch & Media Panel tool process");
    g_hInst = GetCurrentModuleHandle();
    InitializeStateFile();
    LoadSettings();
    g_panelExpanded = !g_settings.startCollapsed;

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_mediaActionEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_activityEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent || !g_mediaActionEvent || !g_activityEvent ||
        !g_readyEvent) {
        Wh_Log(L"Failed to create synchronization events");
        if (g_stopEvent) CloseHandle(g_stopEvent);
        if (g_mediaActionEvent) CloseHandle(g_mediaActionEvent);
        if (g_activityEvent) CloseHandle(g_activityEvent);
        if (g_readyEvent) CloseHandle(g_readyEvent);
        g_stopEvent = nullptr;
        g_mediaActionEvent = nullptr;
        g_activityEvent = nullptr;
        g_readyEvent = nullptr;
        return FALSE;
    }

    g_uiThread =
        CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    g_mediaThread = CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0,
                                 nullptr);
    if (!g_uiThread || !g_mediaThread) {
        Wh_Log(L"Failed to create worker threads");
        SetEvent(g_stopEvent);
        if (g_sinkWnd) PostMessageW(g_sinkWnd, WM_APP_QUIT, 0, 0);
        else if (g_uiThreadId) PostThreadMessageW(g_uiThreadId, WM_APP_QUIT, 0, 0);
        if (g_uiThread) WaitForSingleObject(g_uiThread, INFINITE);
        if (g_mediaThread) WaitForSingleObject(g_mediaThread, INFINITE);
        return FALSE;
    }

    if (WaitForSingleObject(g_readyEvent, 5000) != WAIT_OBJECT_0) {
        Wh_Log(L"UI thread did not signal ready in time");
    }
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
    return g_uiInitialized ? TRUE : FALSE;
}

void WhTool_ModSettingsChanged() {
    HWND sink = g_sinkWnd;
    if (sink) PostMessageW(sink, WM_APP_RELOAD, 0, 0);
    else if (g_uiThreadId)
        PostThreadMessageW(g_uiThreadId, WM_APP_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_mediaActionEvent) SetEvent(g_mediaActionEvent);
    if (g_activityEvent) SetEvent(g_activityEvent);
    HWND modal = g_modalDialog;
    if (modal && IsWindow(modal)) PostMessageW(modal, WM_CLOSE, 0, 0);
    if (g_uiThreadId) EnumThreadWindows(g_uiThreadId, CloseUiThreadWindow, 0);
    HWND sink = g_sinkWnd;
    if (sink) PostMessageW(sink, WM_APP_QUIT, 0, 0);
    else if (g_uiThreadId)
        PostThreadMessageW(g_uiThreadId, WM_APP_QUIT, 0, 0);

    HANDLE threads[] = {g_uiThread, g_mediaThread};
    for (HANDLE thread : threads) {
        if (thread && WaitForSingleObject(thread, 5000) != WAIT_OBJECT_0) {
            Wh_Log(L"A tool thread did not exit in time");
            ExitProcess(1);
        }
    }
    for (HANDLE thread : threads) {
        if (thread) CloseHandle(thread);
    }
    g_uiThread = nullptr;
    g_mediaThread = nullptr;
    g_uiThreadId = 0;
    if (g_stopEvent) CloseHandle(g_stopEvent);
    if (g_mediaActionEvent) CloseHandle(g_mediaActionEvent);
    if (g_activityEvent) CloseHandle(g_activityEvent);
    if (g_readyEvent) CloseHandle(g_readyEvent);
    g_stopEvent = nullptr;
    g_mediaActionEvent = nullptr;
    g_activityEvent = nullptr;
    g_readyEvent = nullptr;
    Wh_Log(L"Quick Launch & Media Panel stopped");
}
}  // namespace

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

    STARTUPINFO si{};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi{};
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
