// ==WindhawkMod==
// @id              quick-launch-media-panel
// @name            Quick Launch & Media Panel
// @description     A modern top-center desktop panel with configurable app shortcuts, drag and drop, and Windows media controls.
// @version         2.0.2
// @author          Spartacus
// @github          https://github.com/spartaaacus
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lgdiplus -lgdi32 -luser32 -lshell32 -lshlwapi -ldwmapi -lcomdlg32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quick Launch & Media Panel

A modern, native Windows panel placed at the top-center of the selected monitor.

## Features

- 1 to 8 configurable application or game shortcuts.
- Drag an `.exe`, `.lnk`, document, folder, or URL shortcut onto a tile.
- Dropped shortcuts are saved for the current Windows user.
- Live title/artist and previous, play/pause, next controls through Windows Global System Media Transport Controls.
- Works with Spotify and with YouTube Music when the browser exposes a Windows media session.
- Is owned by the Windows desktop and locked to the bottom of the normal window stack.

## Usage

- Left-click a populated tile to launch it.
- Drag a file or shortcut onto a tile to assign it.
- Right-click a tile to clear its dropped assignment and return to the corresponding Windhawk setting.
- Drag any empty area of the panel to move it. Its position is remembered.
- Hold Ctrl and use the mouse wheel to resize the shortcut tiles.
- Right-click an empty area for resize and recenter commands.
- Right-click an empty area and choose the settings command to open Windhawk.
- Choose a built-in theme below the language setting, or use Custom for your own colors.
- Use multiple pages of shortcuts.
- Right-click a tile to edit its target, label, arguments, icon, color, and elevation mode.
- Drag one tile onto another to reorder them.
- Album art, timeline, volume control, and media-session selection are supported.
- Import or export the complete layout from the panel context menu.
- Change layout, colors, position, and default shortcuts in the mod settings.

The panel doesn't authenticate with Spotify or YouTube. It controls the media session
already published by the playing application to Windows.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: fr
  $name: Language / Langue
  $description: Language used by the panel and its context menu.
  $options:
    - fr: Français
    - en-US: English (US)
- theme: midnight
  $name: Theme / Thème
  $description: Choose a color palette. Select Custom to use the background and accent colors below.
  $options:
    - midnight: Midnight violet / Minuit violet
    - graphite: Graphite
    - ocean: Ocean blue / Bleu océan
    - emerald: Emerald / Émeraude
    - rose: Rose
    - automatic: Automatic / Automatique
    - custom: Custom / Personnalisé
- pageCount: 3
  $name: Number of pages / Nombre de pages
  $description: Between 1 and 5 shortcut pages.
- lockPosition: false
  $name: Lock position / Verrouiller la position
- tileShape: rounded
  $name: Tile shape / Forme des cases
  $options:
    - rounded: Rounded / Arrondie
    - circle: Circle / Cercle
    - square: Square / Carrée
    - minimal: Minimal
- animations: true
  $name: Animations
- autoThemeSource: system
  $name: Automatic theme source / Source du thème automatique
  $options:
    - system: Windows accent / Accent Windows
    - wallpaper: Wallpaper / Fond d'écran
    - media: Album artwork / Pochette
- shortcutCount: 4
  $name: Number of shortcuts
  $description: Number of visible shortcut tiles (1 to 8).
- columns: 4
  $name: Columns
  $description: Number of tiles per row (1 to 8).
- shortcut1: ""
  $name: Shortcut 1
  $description: Full path, URL, or shell target. You can also drag a file onto the tile.
- shortcut2: ""
  $name: Shortcut 2
- shortcut3: ""
  $name: Shortcut 3
- shortcut4: ""
  $name: Shortcut 4
- shortcut5: ""
  $name: Shortcut 5
- shortcut6: ""
  $name: Shortcut 6
- shortcut7: ""
  $name: Shortcut 7
- shortcut8: ""
  $name: Shortcut 8
- tileSize: 64
  $name: Tile size
  $description: Tile size in logical pixels (40 to 112). Ctrl+mouse wheel also changes it directly.
- gap: 12
  $name: Tile spacing
  $description: Gap between tiles in logical pixels (6 to 24).
- offsetY: 18
  $name: Vertical offset
  $description: Distance from the top edge of the selected monitor.
- monitor: primary
  $name: Monitor
  $options:
    - primary: Primary monitor
    - cursor: Monitor containing the mouse pointer
- perMonitorLayouts: false
  $name: Separate layout per monitor / Disposition distincte par écran
  $description: Stores independent pages and shortcuts for each monitor.
- showLabels: true
  $name: Show shortcut labels
- backgroundColor: "#151821"
  $name: Background color
- accentColor: "#7C5CFC"
  $name: Accent color
- opacity: 92
  $name: Panel opacity
  $description: From 45 to 100 percent.
- cornerRadius: 22
  $name: Corner radius
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#ifndef UNICODE
#define UNICODE
#endif
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
#include <cwctype>
#include <filesystem>
#include <mutex>
#include <string>
#include <thread>
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
constexpr UINT WM_APP_MEDIA = WM_APP + 40;
constexpr UINT WM_APP_RELOAD = WM_APP + 41;
constexpr int kMaxShortcuts = 8;
constexpr int kMaxPages = 5;

enum class TileShape { Rounded, Circle, Square, Minimal };
enum class AutoThemeSource { System, Wallpaper, Media };

struct Settings {
    int count = 4, columns = 4, tile = 64, gap = 12, offsetY = 18;
    int opacity = 92, radius = 22, pageCount = 3;
    bool labels = true, cursorMonitor = false, french = true, perMonitorLayouts = false;
    bool lockPosition = false, animations = true;
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
    int64_t positionTicks = 0, endTicks = 0;
    std::vector<uint8_t> artwork;
    std::vector<std::pair<std::wstring, std::wstring>> sessions;
};

struct ShortcutData {
    std::wstring path, label, arguments, iconPath;
    COLORREF color = 0;
    bool customColor = false;
    bool runAsAdmin = false;
};

Settings g_settings;
MediaState g_media;
std::mutex g_mediaMutex;
std::array<std::wstring, kMaxShortcuts> g_paths;
std::array<ShortcutData, kMaxShortcuts> g_shortcuts;
std::array<HICON, kMaxShortcuts> g_icons{};
HWND g_hwnd = nullptr;
HWND g_desktopHost = nullptr;
IDropTarget* g_dropTarget = nullptr;
bool g_dropRegistered = false;
HANDLE g_uiThread = nullptr, g_mediaThread = nullptr, g_stopEvent = nullptr;
std::atomic<bool> g_running = false;
std::atomic<int> g_asyncOperations = 0;
struct AsyncOperationGuard { ~AsyncOperationGuard(){--g_asyncOperations;} };
ULONG_PTR g_gdiplusToken = 0;
int g_width = 420, g_height = 210, g_hover = -1;
bool g_dragging = false;
POINT g_dragStart{};
RECT g_dragWindowStart{};
int g_savedX = INT_MIN, g_savedY = INT_MIN;
int g_currentPage = 0;
std::wstring g_selectedMediaSession;
std::wstring g_layoutMonitorSuffix = L"primary";
int g_pressedTile = -1;
POINT g_tilePressPoint{};
bool g_reordering = false;
float g_pageAnimation = 1.0f;

void Render();
HWND FindRunningWindow(const std::wstring& rawPath);

int Clamp(int value, int lo, int hi) { return std::max(lo, std::min(hi, value)); }

std::wstring GetStringSetting(const wchar_t* name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
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

std::wstring StateFile() {
    wchar_t storage[MAX_PATH]{};
    if (Wh_GetModStoragePath(storage, ARRAYSIZE(storage)) > 0) {
        CreateDirectoryW(storage, nullptr);
        std::wstring official = std::wstring(storage) + L"\\layout.ini";
        if (GetFileAttributesW(official.c_str()) == INVALID_FILE_ATTRIBUTES) {
            wchar_t oldDir[MAX_PATH]{};
            if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, oldDir))) {
                std::wstring oldFile = std::wstring(oldDir) + L"\\Windhawk\\quick-launch-media-panel.ini";
                CopyFileW(oldFile.c_str(), official.c_str(), TRUE);
            }
        }
        return official;
    }
    wchar_t dir[MAX_PATH]{};
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, dir))) return L"";
    std::wstring folder = std::wstring(dir) + L"\\Windhawk";
    CreateDirectoryW(folder.c_str(), nullptr);
    return folder + L"\\quick-launch-media-panel.ini";
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
    next.labels = Wh_GetIntSetting(L"showLabels") != 0;
    next.lockPosition = Wh_GetIntSetting(L"lockPosition") != 0;
    next.animations = Wh_GetIntSetting(L"animations") != 0;
    next.french = _wcsicmp(GetStringSetting(L"language").c_str(), L"en-US") != 0;
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
    for (auto& icon : g_icons) { if (icon) DestroyIcon(icon); icon = nullptr; }
}

std::wstring ProfilePageSection() {
    // Keep the previous default-profile section name to preserve existing pages.
    std::wstring section=L"profile.default.page." + std::to_wstring(g_currentPage + 1);
    if(g_settings.perMonitorLayouts)section+=L".monitor."+g_layoutMonitorSuffix;
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
    const std::wstring section = ProfilePageSection();
    const std::wstring n = std::to_wstring(slot + 1);
    const ShortcutData& s = g_shortcuts[slot];
    WriteIniString(section, L"path" + n, s.path);
    WriteIniString(section, L"label" + n, s.label);
    WriteIniString(section, L"arguments" + n, s.arguments);
    WriteIniString(section, L"icon" + n, s.iconPath);
    WriteIniString(section, L"color" + n, s.customColor ? std::to_wstring(s.color) : L"");
    WriteIniString(section, L"admin" + n, s.runAsAdmin ? L"1" : L"0");
    WriteIniString(section, L"configured" + n, L"1");
}

void LoadAssignments() {
    DestroyIcons();
    const std::wstring section = ProfilePageSection();
    for (int i = 0; i < kMaxShortcuts; ++i) {
        const std::wstring n = std::to_wstring(i + 1);
        ShortcutData item;
        item.path = ReadIniString(section, L"path" + n);
        // Migrate the v1 default-profile first page transparently.
        bool explicitlyConfigured=ReadIniString(section,L"configured"+n)==L"1";
        if (item.path.empty() && !explicitlyConfigured && g_currentPage == 0) {
            item.path = ReadIniString(L"shortcuts", L"slot" + n, g_settings.defaults[i]);
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
        g_shortcuts[i] = std::move(item);
        g_paths[i] = g_shortcuts[i].path;
        std::wstring iconSource = g_shortcuts[i].iconPath.empty() ? g_paths[i] : g_shortcuts[i].iconPath;
        if (!iconSource.empty()) {
            SHFILEINFOW info{};
            if (SHGetFileInfoW(iconSource.c_str(), 0, &info, sizeof(info), SHGFI_ICON | SHGFI_LARGEICON)) g_icons[i] = info.hIcon;
        }
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

void LoadPersistentUiState() {
    const std::wstring ini = StateFile();
    if (ini.empty()) return;
    int tileOverride = GetPrivateProfileIntW(L"ui", L"tileSize", -1, ini.c_str());
    if (tileOverride >= 40 && tileOverride <= 112) g_settings.tile = tileOverride;
    POINT cursor{};if(g_settings.cursorMonitor)GetCursorPos(&cursor);
    HMONITOR monitor=MonitorFromPoint(cursor,g_settings.cursorMonitor?MONITOR_DEFAULTTONEAREST:MONITOR_DEFAULTTOPRIMARY);
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
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (progman) {
        DWORD_PTR ignored = 0;
        SendMessageTimeoutW(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, &ignored);
    }
    HWND host = nullptr;
    EnumWindows(FindDesktopHostProc, reinterpret_cast<LPARAM>(&host));
    return host ? host : progman;
}

void OpenWindhawk() {
    wchar_t path[MAX_PATH]{};
    HKEY key = nullptr;
    DWORD bytes = sizeof(path);
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Windhawk.exe",
                      0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
        RegQueryValueExW(key, nullptr, nullptr, nullptr,
                         reinterpret_cast<BYTE*>(path), &bytes);
        RegCloseKey(key);
    }
    if (!path[0]) {
        wchar_t programFiles[MAX_PATH]{};
        ExpandEnvironmentStringsW(L"%ProgramFiles%", programFiles, ARRAYSIZE(programFiles));
        swprintf_s(path, L"%s\\Windhawk\\Windhawk.exe", programFiles);
    }
    if (GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES) {
        ShellExecuteW(nullptr, L"open", path, nullptr, nullptr, SW_SHOWNORMAL);
    } else {
        ShellExecuteW(nullptr, L"open", L"windhawk.exe", nullptr, nullptr, SW_SHOWNORMAL);
    }
}

void LayoutMetrics() {
    int columns = std::min(g_settings.columns, g_settings.count);
    int rows = (g_settings.count + columns - 1) / columns;
    int labelExtra = g_settings.labels ? 20 : 0;
    g_width = std::max(340, 40 + columns * g_settings.tile + (columns - 1) * g_settings.gap);
    g_height = 48 + rows * (g_settings.tile + labelExtra) + (rows - 1) * g_settings.gap + 104;
}

RECT TileRect(int index) {
    int columns = std::min(g_settings.columns, g_settings.count);
    int labelExtra = g_settings.labels ? 20 : 0;
    int row = index / columns, col = index % columns;
    int used = std::min(columns, g_settings.count - row * columns);
    int rowWidth = used * g_settings.tile + (used - 1) * g_settings.gap;
    int left = (g_width - rowWidth) / 2 + col * (g_settings.tile + g_settings.gap);
    int top = 42 + row * (g_settings.tile + labelExtra + g_settings.gap);
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
        POINT client{point.x,point.y};ScreenToClient(g_hwnd,&client);int tile=HitTile(client);
        bool accepted=false;
        if(drop&&tile>=0){wchar_t path[32768]{};if(DragQueryFileW(drop,0,path,ARRAYSIZE(path))){g_shortcuts[tile]=ShortcutData{};SaveDroppedPath(tile,path);LoadAssignments();Render();accepted=true;}}
        if(drop)GlobalUnlock(medium.hGlobal);ReleaseStgMedium(&medium);
        if(accepted)SelectEffect(effect);else if(effect)*effect=DROPEFFECT_NONE;return S_OK;
    }
};

int MediaTop() {
    int columns = std::min(g_settings.columns, g_settings.count);
    int rows = (g_settings.count + columns - 1) / columns;
    return 42 + rows * (g_settings.tile + (g_settings.labels ? 20 : 0)) + (rows - 1) * g_settings.gap + 12;
}

RECT PreviousPageRect() { return {18, 12, 48, 34}; }
RECT NextPageRect() { return {g_width - 48, 12, g_width - 18, 34}; }

std::wstring CurrentPageName() {
    std::wstring name = ReadIniString(ProfilePageSection(), L"name");
    if (!name.empty()) return name;
    return (g_settings.french ? L"Page " : L"Page ") + std::to_wstring(g_currentPage + 1);
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

void PositionWindow() {
    POINT pt{};
    if (g_settings.cursorMonitor) GetCursorPos(&pt);
    HMONITOR monitor = g_settings.cursorMonitor ? MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY) : MonitorFromPoint({0,0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(monitor, &mi);
    int x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - g_width) / 2;
    int y = mi.rcWork.top + g_settings.offsetY;
    if (g_savedX != INT_MIN && g_savedY != INT_MIN) {
        x = g_savedX;
        y = g_savedY;
        x = Clamp(x, mi.rcWork.left, std::max(mi.rcWork.left, mi.rcWork.right - g_width));
        y = Clamp(y, mi.rcWork.top, std::max(mi.rcWork.top, mi.rcWork.bottom - g_height));
    }
    // The panel is an owned top-level window. HWND_BOTTOM puts it at the lowest
    // legal position, immediately above its desktop owner and below normal apps.
    SetWindowPos(g_hwnd, HWND_BOTTOM, x, y, g_width, g_height,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOOWNERZORDER);
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
    GraphicsPath path;
    float d = radius * 2;
    path.AddArc(r.X, r.Y, d, d, 180, 90); path.AddArc(r.GetRight()-d, r.Y, d, d, 270, 90);
    path.AddArc(r.GetRight()-d, r.GetBottom()-d, d, d, 0, 90); path.AddArc(r.X, r.GetBottom()-d, d, d, 90, 90);
    path.CloseFigure();
    SolidBrush brush(color); g.FillPath(&brush, &path);
}

void RoundRectStroke(Graphics& g, const RectF& r, float radius,
                     const Color& color, float width) {
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

void ApplyAutomaticTheme() {
    if(_wcsicmp(g_settings.requestedTheme.c_str(),L"automatic")!=0)return;
    COLORREF accent=RGB(124,92,252);
    if(g_settings.autoThemeSource==AutoThemeSource::System){DWORD color=0;BOOL opaque=FALSE;if(SUCCEEDED(DwmGetColorizationColor(&color,&opaque)))accent=RGB((color>>16)&255,(color>>8)&255,color&255);}
    else if(g_settings.autoThemeSource==AutoThemeSource::Wallpaper){wchar_t path[MAX_PATH]{};if(SystemParametersInfoW(SPI_GETDESKWALLPAPER,ARRAYSIZE(path),path,0)&&path[0]){Bitmap wallpaper(path);accent=AverageBitmap(wallpaper,accent);}}
    else {std::vector<uint8_t> art;{std::lock_guard lock(g_mediaMutex);art=g_media.artwork;}accent=AccentFromArtwork(art,accent);}
    g_settings.accent=accent;
    g_settings.background=RGB(10+GetRValue(accent)/10,12+GetGValue(accent)/10,15+GetBValue(accent)/10);
}

void Render() {
    if (!g_hwnd) return;
    HDC screen = GetDC(nullptr), memory = CreateCompatibleDC(screen);
    BITMAPINFO bi{}; bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bi.bmiHeader.biWidth = g_width; bi.bmiHeader.biHeight = -g_height;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr; HBITMAP bitmap = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HGDIOBJ old = SelectObject(memory, bitmap);
    Graphics g(memory); g.SetSmoothingMode(SmoothingModeAntiAlias); g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    BYTE alpha = static_cast<BYTE>(255 * g_settings.opacity / 100);
    RoundRectFill(g, RectF(0, 0, (REAL)g_width, (REAL)g_height), (REAL)g_settings.radius,
                  Color(alpha, GetRValue(g_settings.background), GetGValue(g_settings.background), GetBValue(g_settings.background)));
    RoundRectStroke(g, RectF(0.7f, 0.7f, (REAL)g_width-1.4f, (REAL)g_height-1.4f),
                    (REAL)g_settings.radius-0.7f, Color(65,255,255,255), 1.2f);
    DrawCentered(g, L"\u2039", RectF(18,10,30,24), 20, Color(205,235,238,245));
    DrawCentered(g, CurrentPageName(), RectF(52,10,(REAL)g_width-104,24), 12, Color(225,240,242,248), true);
    DrawCentered(g, L"\u203a", RectF((REAL)g_width-48,10,30,24), 20, Color(205,235,238,245));
    for (int i = 0; i < g_settings.count; ++i) {
        RECT rr = TileRect(i); bool hover = i == g_hover;
        float radius = g_settings.tileShape==TileShape::Circle ? g_settings.tile/2.0f :
                       g_settings.tileShape==TileShape::Square ? 5.0f : 14.0f;
        BYTE animAlpha=static_cast<BYTE>(std::clamp(g_pageAnimation,0.0f,1.0f)*255);
        COLORREF tileAccent=g_shortcuts[i].customColor?g_shortcuts[i].color:g_settings.accent;
        if(g_settings.tileShape!=TileShape::Minimal){
            RoundRectFill(g, RectF((REAL)rr.left, (REAL)rr.top+3, (REAL)g_settings.tile, (REAL)g_settings.tile), radius, Color(45*animAlpha/255,0,0,0));
            Color tileColor = hover ? Color(90*animAlpha/255, GetRValue(tileAccent), GetGValue(tileAccent), GetBValue(tileAccent)) : Color(45*animAlpha/255, 255,255,255);
            RoundRectFill(g, RectF((REAL)rr.left, (REAL)rr.top, (REAL)g_settings.tile, (REAL)g_settings.tile), radius, tileColor);
            RoundRectStroke(g, RectF((REAL)rr.left+0.5f, (REAL)rr.top+0.5f, (REAL)g_settings.tile-1, (REAL)g_settings.tile-1), std::max(1.0f,radius-0.5f),
                            hover ? Color(170, GetRValue(tileAccent), GetGValue(tileAccent), GetBValue(tileAccent)) : Color(35,255,255,255), 1.0f);
        }
        if (g_icons[i]) {
            Bitmap icon(g_icons[i]); int iconSize = Clamp(g_settings.tile - 24, 28, 52);
            int x = rr.left + (g_settings.tile - iconSize) / 2, y = rr.top + (g_settings.tile - iconSize) / 2;
            g.DrawImage(&icon, x, y, iconSize, iconSize);
        } else {
            DrawCentered(g, L"+", RectF((REAL)rr.left, (REAL)rr.top-2, (REAL)g_settings.tile, (REAL)g_settings.tile), 30, Color(190,255,255,255));
        }
        if(!g_shortcuts[i].path.empty()&&FindRunningWindow(g_shortcuts[i].path))
            RoundRectFill(g,RectF((REAL)rr.left+g_settings.tile/2.0f-9,(REAL)rr.bottom-5,18,3),1.5f,
                          Color(235,GetRValue(tileAccent),GetGValue(tileAccent),GetBValue(tileAccent)));
        if (g_settings.labels) DrawCentered(g, DisplayName(g_paths[i], i), RectF((REAL)rr.left-5, (REAL)rr.bottom+1, (REAL)g_settings.tile+10, 18), 11, Color(205,235,238,245));
    }
    int mt = MediaTop();
    RoundRectFill(g, RectF(18, (REAL)mt+3, (REAL)g_width-36, 76), 16, Color(38,0,0,0));
    RoundRectFill(g, RectF(18, (REAL)mt, (REAL)g_width-36, 76), 16, Color(34,255,255,255));
    RoundRectStroke(g, RectF(18.5f, (REAL)mt+0.5f, (REAL)g_width-37, 75), 15.5f, Color(28,255,255,255), 1.0f);
    RoundRectFill(g, RectF(22, (REAL)mt+18, 3, 36), 1.5f,
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
    if(DrawImageBytes(g,media.artwork,RectF(31,(REAL)mt+10,46,46))) textX=86;
    g.DrawString(title.c_str(), -1, &titleFont, RectF(textX,(REAL)mt+8,(REAL)g_width-textX-150,22), &left, &white);
    g.DrawString(artist.c_str(), -1, &artistFont, RectF(textX,(REAL)mt+31,(REAL)g_width-textX-150,18), &left, &muted);
    int cx = g_width - 102;
    DrawCentered(g, L"|<", RectF((REAL)cx-54,(REAL)mt+14,32,36), 13, Color(235,245,246,250));
    RoundRectFill(g, RectF((REAL)cx-16,(REAL)mt+12,40,40), 20, Color(220, GetRValue(g_settings.accent), GetGValue(g_settings.accent), GetBValue(g_settings.accent)));
    DrawCentered(g, media.playing ? L"||" : L">", RectF((REAL)cx-16,(REAL)mt+12,40,40), 14, Color(255,255,255,255), true);
    DrawCentered(g, L">|", RectF((REAL)cx+30,(REAL)mt+14,32,36), 13, Color(235,245,246,250));
    float progress=media.endTicks>0?std::clamp(static_cast<float>(media.positionTicks)/media.endTicks,0.0f,1.0f):0.0f;
    RoundRectFill(g,RectF(32,(REAL)mt+64,(REAL)g_width-64,3),1.5f,Color(45,255,255,255));
    if(progress>0)RoundRectFill(g,RectF(32,(REAL)mt+64,((REAL)g_width-64)*progress,3),1.5f,
                               Color(220,GetRValue(g_settings.accent),GetGValue(g_settings.accent),GetBValue(g_settings.accent)));
    Font versionFont(&ff, 9, FontStyleRegular, UnitPixel);
    StringFormat versionFormat; versionFormat.SetAlignment(StringAlignmentFar); versionFormat.SetLineAlignment(StringAlignmentCenter);
    SolidBrush versionBrush(Color(210,255,255,255));
    g.DrawString(L"v2.0.2", -1, &versionFont, RectF(0,(REAL)g_height-19,(REAL)g_width-12,14), &versionFormat, &versionBrush);
    POINT src{0,0}; SIZE size{g_width,g_height};
    BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
    if (!UpdateLayeredWindow(g_hwnd,screen,nullptr,&size,memory,&src,0,&blend,ULW_ALPHA)) {
        static bool logged = false;
        if (!logged) {
            Wh_Log(L"UpdateLayeredWindow failed, error=%u", GetLastError());
            logged = true;
        }
    }
    SelectObject(memory,old); DeleteObject(bitmap); DeleteDC(memory); ReleaseDC(nullptr,screen);
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
        CreateWindowExW(0, L"BUTTON", L"Cancel", WS_CHILD|WS_VISIBLE|WS_TABSTOP,
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

bool PromptText(const std::wstring& title, const std::wstring& prompt, std::wstring& value) {
    static bool registered = false;
    if (!registered) {
        WNDCLASSW wc{}; wc.lpfnWndProc = PromptWindowProc; wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW); wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);
        wc.lpszClassName = L"Windhawk.QuickPanelPrompt";
        registered = RegisterClassW(&wc) != 0 || GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    }
    PromptData data{title, prompt, value};
    RECT owner{}; GetWindowRect(g_hwnd, &owner);
    HWND dialog = CreateWindowExW(WS_EX_DLGMODALFRAME|WS_EX_TOPMOST,
        L"Windhawk.QuickPanelPrompt", title.c_str(), WS_CAPTION|WS_SYSMENU|WS_POPUP,
        owner.left + (owner.right-owner.left-370)/2, owner.top + 40, 370, 140,
        g_hwnd, nullptr, GetModuleHandleW(nullptr), &data);
    if (!dialog) return false;
    EnableWindow(g_hwnd, FALSE); ShowWindow(dialog, SW_SHOW); UpdateWindow(dialog);
    MSG msg{};
    while (IsWindow(dialog) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageW(dialog, &msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    }
    EnableWindow(g_hwnd, TRUE);
    value = data.value;
    return data.accepted;
}

bool ChooseFile(std::wstring& path, const wchar_t* filter = L"All files\0*.*\0\0") {
    wchar_t buffer[32768]{};
    if (!path.empty()) wcsncpy_s(buffer, path.c_str(), _TRUNCATE);
    OPENFILENAMEW ofn{}; ofn.lStructSize=sizeof(ofn); ofn.hwndOwner=g_hwnd;
    ofn.lpstrFile=buffer; ofn.nMaxFile=ARRAYSIZE(buffer); ofn.lpstrFilter=filter;
    ofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_EXPLORER;
    if (!GetOpenFileNameW(&ofn)) return false;
    path=buffer; return true;
}

bool ChooseFolder(std::wstring& path) {
    BROWSEINFOW bi{}; bi.hwndOwner=g_hwnd;
    bi.lpszTitle=g_settings.french ? L"Choisir un dossier de raccourcis" : L"Choose a shortcut folder";
    bi.ulFlags=BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
    PIDLIST_ABSOLUTE pidl=SHBrowseForFolderW(&bi); if(!pidl) return false;
    wchar_t buffer[MAX_PATH]{}; bool ok=SHGetPathFromIDListW(pidl,buffer)!=FALSE;
    CoTaskMemFree(pidl); if(ok) path=buffer; return ok;
}

void ChooseShortcutColor(int slot) {
    static COLORREF custom[16]{};
    CHOOSECOLORW cc{}; cc.lStructSize=sizeof(cc); cc.hwndOwner=g_hwnd;
    cc.rgbResult=g_shortcuts[slot].customColor?g_shortcuts[slot].color:g_settings.accent;
    cc.lpCustColors=custom; cc.Flags=CC_FULLOPEN|CC_RGBINIT;
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

HWND FindRunningWindow(const std::wstring& rawPath) {
    std::wstring target=ResolveExecutable(rawPath); std::wstring name=PathFindFileNameW(target.c_str());
    if(name.empty()||_wcsicmp(PathFindExtensionW(name.c_str()),L".exe")!=0) return nullptr;
    FindWindowData data{name}; EnumWindows(FindRunningWindowProc,reinterpret_cast<LPARAM>(&data)); return data.found;
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
    int cmd=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_RIGHTBUTTON,screen.x,screen.y,0,g_hwnd,nullptr);
    DestroyMenu(menu); if(cmd>=500&&static_cast<size_t>(cmd-500)<entries.size())
        ShellExecuteW(g_hwnd,L"open",entries[cmd-500].c_str(),nullptr,nullptr,SW_SHOWNORMAL);
}

void LaunchSlot(int slot) {
    if(slot<0||slot>=kMaxShortcuts||g_shortcuts[slot].path.empty()) return;
    DWORD attributes=GetFileAttributesW(g_shortcuts[slot].path.c_str());
    if(attributes!=INVALID_FILE_ATTRIBUTES&&(attributes&FILE_ATTRIBUTE_DIRECTORY)){POINT p{};GetCursorPos(&p);ShowFolderContents(slot,p);return;}
    if(HWND running=FindRunningWindow(g_shortcuts[slot].path)){
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
    int cmd=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_RIGHTBUTTON,screen.x,screen.y,0,g_hwnd,nullptr);DestroyMenu(menu);
    if(cmd==10){std::wstring path=g_shortcuts[slot].path;if(ChooseFile(path)){g_shortcuts[slot].path=path;SaveShortcut(slot);ReloadShortcutVisuals();}}
    else if(cmd==11){std::wstring path=g_shortcuts[slot].path;if(ChooseFolder(path)){g_shortcuts[slot].path=path;SaveShortcut(slot);ReloadShortcutVisuals();}}
    else if(cmd==12){std::wstring value=g_shortcuts[slot].label;if(PromptText(g_settings.french?L"Nom du raccourci":L"Shortcut label",g_settings.french?L"Nom affich\u00e9 :":L"Displayed name:",value)){g_shortcuts[slot].label=value;SaveShortcut(slot);Render();}}
    else if(cmd==13){std::wstring value=g_shortcuts[slot].arguments;if(PromptText(g_settings.french?L"Arguments":L"Arguments",g_settings.french?L"Arguments de lancement :":L"Launch arguments:",value)){g_shortcuts[slot].arguments=value;SaveShortcut(slot);}}
    else if(cmd==14){std::wstring value=g_shortcuts[slot].iconPath;if(ChooseFile(value,L"Icons and programs\0*.ico;*.exe;*.dll\0All files\0*.*\0\0")){g_shortcuts[slot].iconPath=value;SaveShortcut(slot);ReloadShortcutVisuals();}}
    else if(cmd==15) ChooseShortcutColor(slot);
    else if(cmd==16){g_shortcuts[slot].customColor=false;SaveShortcut(slot);Render();}
    else if(cmd==17){g_shortcuts[slot].runAsAdmin=!g_shortcuts[slot].runAsAdmin;SaveShortcut(slot);}
    else if(cmd==18){std::wstring args=L"/select,\""+g_shortcuts[slot].path+L"\"";ShellExecuteW(g_hwnd,L"open",L"explorer.exe",args.c_str(),nullptr,SW_SHOWNORMAL);}
    else if(cmd==19){SaveDroppedPath(slot,L"");ReloadShortcutVisuals();}
}

void SwapShortcuts(int a,int b) {
    if(a<0||b<0||a>=kMaxShortcuts||b>=kMaxShortcuts||a==b)return;
    std::swap(g_shortcuts[a],g_shortcuts[b]);SaveShortcut(a);SaveShortcut(b);ReloadShortcutVisuals();
}

bool SelectBackupFile(bool save,std::wstring& path) {
    wchar_t buffer[32768]{}; wcscpy_s(buffer,L"quick-launch-media-panel-backup.ini");
    OPENFILENAMEW ofn{};ofn.lStructSize=sizeof(ofn);ofn.hwndOwner=g_hwnd;ofn.lpstrFile=buffer;ofn.nMaxFile=ARRAYSIZE(buffer);
    ofn.lpstrFilter=L"Panel backup (*.ini)\0*.ini\0All files\0*.*\0\0";ofn.lpstrDefExt=L"ini";
    ofn.Flags=OFN_EXPLORER|OFN_PATHMUSTEXIST|(save?OFN_OVERWRITEPROMPT:OFN_FILEMUSTEXIST);
    bool ok=save?GetSaveFileNameW(&ofn)!=FALSE:GetOpenFileNameW(&ofn)!=FALSE;if(ok)path=buffer;return ok;
}

void ExportLayout() {std::wstring path;if(SelectBackupFile(true,path))CopyFileW(StateFile().c_str(),path.c_str(),FALSE);}
void ImportLayout() {std::wstring path;if(SelectBackupFile(false,path)&&CopyFileW(path.c_str(),StateFile().c_str(),FALSE)){LoadPersistentUiState();LoadAssignments();LayoutMetrics();PositionWindow();Render();}}

void MediaCommand(int command) {
    ++g_asyncOperations;
    std::thread([command] {
        AsyncOperationGuard operationGuard;
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            auto manager = MediaManager::RequestAsync().get();
            winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session{nullptr};
            std::wstring requested;{std::lock_guard lock(g_mediaMutex);requested=g_selectedMediaSession;}
            if(manager&&!requested.empty())for(auto const& candidate:manager.GetSessions())
                if(candidate.SourceAppUserModelId().c_str()==requested){session=candidate;break;}
            if(!session&&manager)session=manager.GetCurrentSession();
            if (!session) return;
            if (command == 0) session.TrySkipPreviousAsync().get();
            else if (command == 1) session.TryTogglePlayPauseAsync().get();
            else session.TrySkipNextAsync().get();
        } catch (...) { Wh_Log(L"Media command failed"); }
    }).detach();
}

void MediaSeek(double fraction) {
    fraction=std::clamp(fraction,0.0,1.0);
    ++g_asyncOperations;
    std::thread([fraction]{
        AsyncOperationGuard operationGuard;
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try{
            auto manager=MediaManager::RequestAsync().get();
            winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session{nullptr};
            std::wstring requested;{std::lock_guard lock(g_mediaMutex);requested=g_selectedMediaSession;}
            if(manager&&!requested.empty())for(auto const& candidate:manager.GetSessions())
                if(candidate.SourceAppUserModelId().c_str()==requested){session=candidate;break;}
            if(!session&&manager)session=manager.GetCurrentSession();
            if(session){auto timeline=session.GetTimelineProperties();session.TryChangePlaybackPositionAsync(static_cast<int64_t>(timeline.EndTime().count()*fraction)).get();}
        }catch(...){}
    }).detach();
}

void ChangeMasterVolume(float delta) {
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
        POINT p{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        int mt = MediaTop(), cx = g_width - 102;
        int tile=HitTile(p);
        if(tile>=0){g_pressedTile=tile;g_tilePressPoint=p;g_reordering=false;SetCapture(hwnd);return 0;}
        bool onMediaControl = p.y >= mt+6 && p.y <= mt+72 && p.x >= cx-58 && p.x <= cx+66;
        RECT previousPage=PreviousPageRect(),nextPage=NextPageRect();
        bool onPageControl=PtInRect(&previousPage,p)||PtInRect(&nextPage,p);
        if (!onMediaControl && !onPageControl && !g_settings.lockPosition) {
            g_dragging = true;
            GetCursorPos(&g_dragStart);
            GetWindowRect(hwnd, &g_dragWindowStart);
            SetCapture(hwnd);
        }
        return 0;
    }
    case WM_MOUSEMOVE: {
        if(g_pressedTile>=0){
            POINT p{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
            if(abs(p.x-g_tilePressPoint.x)>7||abs(p.y-g_tilePressPoint.y)>7)g_reordering=true;
            int hit=HitTile(p);if(hit!=g_hover){g_hover=hit;Render();}return 0;
        }
        if (g_dragging) {
            POINT current{};
            GetCursorPos(&current);
            int x = g_dragWindowStart.left + current.x - g_dragStart.x;
            int y = g_dragWindowStart.top + current.y - g_dragStart.y;
            RECT proposed{x, y, x + g_width, y + g_height};
            HMONITOR monitor = MonitorFromRect(&proposed, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);
            if (GetMonitorInfoW(monitor, &mi)) {
                x = Clamp(x, mi.rcWork.left,
                          std::max(mi.rcWork.left, mi.rcWork.right - g_width));
                y = Clamp(y, mi.rcWork.top,
                          std::max(mi.rcWork.top, mi.rcWork.bottom - g_height));
            }
            SetWindowPos(hwnd, nullptr, x, y, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            return 0;
        }
        POINT p{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}; int hit = HitTile(p);
        if (hit != g_hover) { g_hover = hit; Render(); }
        TRACKMOUSEEVENT t{sizeof(t),TME_LEAVE,hwnd,0}; TrackMouseEvent(&t); return 0;
    }
    case WM_MOUSELEAVE: g_hover = -1; Render(); return 0;
    case WM_LBUTTONUP: {
        if(g_pressedTile>=0){
            int source=g_pressedTile;POINT p{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};int target=HitTile(p);
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
        POINT p{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}; int tile = HitTile(p);
        RECT prev=PreviousPageRect(),next=NextPageRect();
        if(PtInRect(&prev,p)){SwitchPage(-1);return 0;}if(PtInRect(&next,p)){SwitchPage(1);return 0;}
        if (tile >= 0) { LaunchSlot(tile); return 0; }
        int mt = MediaTop(), cx = g_width - 102;
        if(p.y>=mt+59&&p.y<=mt+72&&p.x>=32&&p.x<=g_width-32){MediaSeek((p.x-32.0)/(g_width-64.0));return 0;}
        if (p.y >= mt+6 && p.y <= mt+58) {
            if (p.x >= cx-58 && p.x < cx-20) MediaCommand(0);
            else if (p.x >= cx-20 && p.x < cx+28) MediaCommand(1);
            else if (p.x >= cx+28 && p.x <= cx+66) MediaCommand(2);
            else BringMediaAppToFront();
        } return 0;
    }
    case WM_RBUTTONUP: {
        POINT p{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)}; int tile=HitTile(p);
        POINT screen=p;ClientToScreen(hwnd,&screen);
        if(tile>=0){ShowTileContextMenu(tile,screen);return 0;}
        int mt=MediaTop();
        if(p.y>=mt&&p.y<=mt+76){
            MediaState media;{std::lock_guard lock(g_mediaMutex);media=g_media;}
            HMENU sessions=CreatePopupMenu();AppendMenuW(sessions,MF_STRING,299,g_settings.french?L"Session automatique":L"Automatic session");
            for(size_t i=0;i<media.sessions.size()&&i<30;i++)AppendMenuW(sessions,MF_STRING,300+i,media.sessions[i].second.c_str());
            int command=TrackPopupMenu(sessions,TPM_RETURNCMD|TPM_RIGHTBUTTON,screen.x,screen.y,0,hwnd,nullptr);DestroyMenu(sessions);
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
        int command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, screen.x, screen.y, 0, hwnd, nullptr);
        DestroyMenu(menu);
        if (command == 1) ResizeTiles(-4);
        else if (command == 2) ResizeTiles(4);
        else if (command == 3) { ClearSavedPosition(); PositionWindow(); Render(); }
        else if (command == 4) OpenWindhawk();
        else if(command==5){g_settings.lockPosition=!g_settings.lockPosition;SaveUiNumber(L"lockPosition",g_settings.lockPosition?1:0);}
        else if(command==6){std::wstring name=CurrentPageName();if(PromptText(g_settings.french?L"Nom de la page":L"Page name",g_settings.french?L"Nom :":L"Name:",name)){WriteIniString(ProfilePageSection(),L"name",name);Render();}}
        else if(command==7)ExportLayout();else if(command==8)ImportLayout();
        return 0;
    }
    case WM_MOUSEWHEEL:
        if (GET_KEYSTATE_WPARAM(wParam) & MK_CONTROL) {
            ResizeTiles(GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 4 : -4);
            return 0;
        }
        {POINT screen{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};POINT client=screen;ScreenToClient(hwnd,&client);int mt=MediaTop();
        if(client.y>=mt&&client.y<=mt+76)ChangeMasterVolume(GET_WHEEL_DELTA_WPARAM(wParam)>0?0.05f:-0.05f);else SwitchPage(GET_WHEEL_DELTA_WPARAM(wParam)>0?-1:1);return 0;}
    case WM_TIMER:
        if(wParam==7){g_pageAnimation=std::min(1.0f,g_pageAnimation+0.12f);Render();if(g_pageAnimation>=1.0f)KillTimer(hwnd,7);}return 0;
    case WM_CAPTURECHANGED: g_dragging = false;g_pressedTile=-1;g_reordering=false; return 0;
    case WM_DROPFILES: {
        HDROP drop=(HDROP)wParam; POINT p{}; DragQueryPoint(drop,&p); int tile=HitTile(p);
        if(tile>=0){ wchar_t path[32768]{}; if(DragQueryFileW(drop,0,path,ARRAYSIZE(path))){ g_shortcuts[tile]=ShortcutData{};SaveDroppedPath(tile,path); LoadAssignments(); Render(); }}
        DragFinish(drop); return 0;
    }
    case WM_APP_MEDIA: {
        if(g_settings.autoThemeSource==AutoThemeSource::Media)ApplyAutomaticTheme();
        Render();return 0;
    }
    case WM_APP_RELOAD: {
        LoadSettings();
        const std::wstring ini = StateFile();
        if (!ini.empty()) WritePrivateProfileStringW(L"ui", L"tileSize", nullptr, ini.c_str());
        LoadAssignments(); ApplyAutomaticTheme(); LayoutMetrics(); PositionWindow(); Render(); return 0;
    }
    case WM_DISPLAYCHANGE: PositionWindow(); Render(); return 0;
    case WM_CLOSE:
        if(g_dropRegistered){RevokeDragDrop(hwnd);g_dropRegistered=false;}
        DestroyWindow(hwnd);return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd,msg,wParam,lParam);
}

DWORD WINAPI UiThreadProc(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT apartment=OleInitialize(nullptr);
    GdiplusStartupInput input; GdiplusStartup(&g_gdiplusToken,&input,nullptr);
    WNDCLASSEXW wc{}; wc.cbSize=sizeof(wc); wc.lpfnWndProc=WindowProc; wc.hInstance=GetModuleHandleW(nullptr); wc.hCursor=LoadCursorW(nullptr,IDC_ARROW); wc.lpszClassName=kClassName;
    RegisterClassExW(&wc); LoadPersistentUiState(); LayoutMetrics();
    g_desktopHost = FindDesktopHost();
    DWORD ex=WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE;
    g_hwnd=CreateWindowExW(ex,kClassName,L"Quick Launch & Media Panel",WS_POPUP,0,0,g_width,g_height,g_desktopHost,nullptr,wc.hInstance,nullptr);
    if(!g_hwnd) {
        Wh_Log(L"CreateWindowExW failed, error=%u", GetLastError());
        return 1;
    }
    Wh_Log(L"Panel window created, desktop owner=%p", g_desktopHost);
    g_dropTarget=new PanelDropTarget();
    HRESULT dropRegistration=RegisterDragDrop(g_hwnd,g_dropTarget);
    g_dropRegistered=SUCCEEDED(dropRegistration);
    if(FAILED(dropRegistration))Wh_Log(L"RegisterDragDrop failed, error=0x%08X",dropRegistration);
    LoadAssignments(); ApplyAutomaticTheme(); PositionWindow(); Render(); ShowWindow(g_hwnd,SW_SHOWNOACTIVATE);
    MSG msg; while(GetMessageW(&msg,nullptr,0,0)>0){ TranslateMessage(&msg); DispatchMessageW(&msg); }
    if(g_dropRegistered){RevokeDragDrop(g_hwnd);g_dropRegistered=false;}
    if(g_dropTarget){g_dropTarget->Release();g_dropTarget=nullptr;}
    DestroyIcons(); g_hwnd=nullptr; g_desktopHost=nullptr; UnregisterClassW(kClassName,wc.hInstance); GdiplusShutdown(g_gdiplusToken);if(SUCCEEDED(apartment))OleUninitialize(); return 0;
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

DWORD WINAPI MediaThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded); MediaManager manager{nullptr};
    while(WaitForSingleObject(g_stopEvent,0)==WAIT_TIMEOUT){
        MediaState next;
        try {
            if(!manager) manager=MediaManager::RequestAsync().get();
            if(manager){
                auto sessions=manager.GetSessions();
                std::wstring requestedSession;{std::lock_guard lock(g_mediaMutex);requestedSession=g_selectedMediaSession;}
                winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession selected{nullptr};
                for(auto const& candidate:sessions){
                    std::wstring id=candidate.SourceAppUserModelId().c_str();
                    next.sessions.push_back({id,FriendlySourceName(id)});
                    if(!requestedSession.empty()&&id==requestedSession)selected=candidate;
                }
                if(!selected)selected=manager.GetCurrentSession();
                if(selected){
                    auto p=selected.TryGetMediaPropertiesAsync().get();auto info=selected.GetPlaybackInfo();auto timeline=selected.GetTimelineProperties();
                    next.available=true;next.playing=info.PlaybackStatus()==PlaybackStatus::Playing;
                    next.title=p.Title().c_str();next.artist=p.Artist().c_str();next.sourceId=selected.SourceAppUserModelId().c_str();
                    next.positionTicks=timeline.Position().count();next.endTicks=timeline.EndTime().count();
                    bool reuse=false;{std::lock_guard lock(g_mediaMutex);if(g_media.title==next.title&&!g_media.artwork.empty()){next.artwork=g_media.artwork;reuse=true;}}
                    if(!reuse)next.artwork=ReadArtwork(p.Thumbnail());
                }
            }
        }
        catch(...){ manager=nullptr; }
        { std::lock_guard lock(g_mediaMutex); g_media=std::move(next); }
        if(g_hwnd) PostMessageW(g_hwnd,WM_APP_MEDIA,0,0);
        WaitForSingleObject(g_stopEvent,1000);
    } return 0;
}

bool Start() {
    g_stopEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr); if(!g_stopEvent) return false;
    g_running=true; g_uiThread=CreateThread(nullptr,0,UiThreadProc,nullptr,0,nullptr); g_mediaThread=CreateThread(nullptr,0,MediaThreadProc,nullptr,0,nullptr);
    return g_uiThread&&g_mediaThread;
}
void Stop() {
    if(g_stopEvent) SetEvent(g_stopEvent); if(g_hwnd) PostMessageW(g_hwnd,WM_CLOSE,0,0);
    HANDLE hs[]={g_uiThread,g_mediaThread}; for(HANDLE h:hs) if(h){WaitForSingleObject(h,3000);CloseHandle(h);} g_uiThread=g_mediaThread=nullptr;
    for(int i=0;i<100&&g_asyncOperations.load()>0;i++)Sleep(20);
    if(g_stopEvent){CloseHandle(g_stopEvent);g_stopEvent=nullptr;} g_running=false;
}
} // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Quick Launch & Media Panel in explorer.exe");
    LoadSettings();
    if (!Start()) {
        Wh_Log(L"Failed to start panel threads");
        Stop();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP_RELOAD, 0, 0);
    else LoadSettings();
}

void Wh_ModUninit() {
    Stop();
    Wh_Log(L"Quick Launch & Media Panel stopped");
}
