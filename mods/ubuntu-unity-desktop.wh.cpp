// ==WindhawkMod==
// @id              ubuntu-unity-desktop
// @name            Ubuntu Unity Desktop
// @description     Turns Windows 11 into Ubuntu's Unity desktop: a top panel with menus and window tabs, the launcher on the left and the Dash as Start menu
// @version         1.0
// @author          dion
// @github          https://github.com/dismedan-dion
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ld2d1 -ldwrite -lwindowscodecs -lole32 -loleaut32 -lshell32 -lshlwapi -luuid -ldwmapi -lcomctl32 -lgdi32 -lshcore -lpdh -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Ubuntu Unity Desktop

Turns Windows 11 into the **Ubuntu Unity 7** desktop (Ubuntu 12.04 - 16.04): a panel along the top, the launcher
along the left and the Dash instead of the Start menu. Everything runs inside Explorer: there is no separate
program to install, and turning the mod off gives you the normal Windows desktop back right away.

## What you get

**The panel (top bar)**
* The title of the window in front, and for a maximized window its minimize / restore / close buttons: the
  window loses its own title bar, like in Unity.
* **Tabs in the panel**: maximize a browser (Brave, Chrome, Edge, ...) or another window with tabs and its
  whole tab row moves into the panel, with the sites' icons. Click a tab to switch, x to close it, + for a new one.
* Drag a maximized window down out of the panel to restore it, and drop it back on the panel to maximize it.
* Indicators on the right: background apps (tray icons), keyboard language, network, messages, volume (with a
  slider), clock (with a calendar) and the session menu (lock, log out, restart, shut down).

**The launcher (left bar)**
* Pinned and running apps, with a pip per open window and an arrow for the app in front. Click to open, switch
  or minimize; right-click to pin / unpin, run as administrator, open the file location or quit.
* Hover for the app's name and live previews of its windows.
* Drag icons to reorder them, onto each other to make a group, or off the launcher to unpin them.
* **Music controls** (cover, play / pause, previous / next) slide open under the app that plays music.
* **Discord call buttons** (mute, deafen, hang up) slide open under Discord while you're in a call.
* Drag the launcher's right edge to make it (and its icons) bigger or smaller. The trash sits at the bottom.

**The Dash (Start menu)**
* Opens with the Windows key, Ctrl+Esc or the button at the top of the launcher.
* Type to search apps, settings and files; lenses and filters like Unity's; widgets (clock, music, weather app, system, disks) on Home.
* Resize it with the grip in its corner (the size is remembered) or maximize it.

**And more**
* Unity's animations: windows fly into the launcher when minimized, the launcher icons pop and wiggle, the panel
  and launcher slide away for full-screen games and videos.
* Glass (blur or acrylic), your accent colour or any colour, dark (Ambiance) or light (Radiance) panel.

## Settings

Everything can be adjusted in the mod's settings, grouped by panel, launcher, Dash, look, windows and Windows
integration: sizes, opacity, glass, clock format, which indicators show, animation speed, tabs in the panel, and
whether the Windows taskbar is hidden.

## Good to know

* Windows 11, 64-bit. The shell lives on the main screen. With the taskbar hidden, other screens have no taskbar
  (and no launcher) at all; their windows stay where they are.
* **The taskbar's "automatically hide" setting is turned on while the mod runs** (that gives its room on the screen
  back) and is set back as it was when you turn the mod off. If Explorer is killed or crashes and the mod is
  removed before it runs again, it may stay on: switch it off under Settings > Personalization > Taskbar.
* The Windows key and Ctrl+Esc open the Dash; shortcuts like Win+E, Win+R and Win+Shift+S keep working. If
  Windows' own Start menu opens anyway, the mod closes it and shows the Dash.
* Tabs in the panel read the window's tabs through Windows' accessibility interface. They work best with
  Chromium browsers (Brave, Chrome, Edge, Vivaldi, Opera). The new-tab button and the Discord call buttons are
  found by their names in English and Dutch apps.
* The shell's texts are in English, or in Dutch when Windows is.
* The Dash's weather card opens Windows' own Weather app: the mod itself never connects to the internet.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- panel:
  - height: 24
    $name: Height (px at 100% scaling)
  - style: auto
    $name: Style
    $options:
    - auto: Follow Windows (light or dark mode)
    - dark: Dark (Ambiance)
    - light: Light (Radiance)
  - opacity: 80
    $name: Opacity (%)
    $description: Lower = more glass
  - clock: time
    $name: Clock
    $options:
    - time: Time (13:08)
    - dayTime: Day and time (Wed 13:08)
    - dateTime: Day, date and time (Wed 29 Oct 13:08)
  - showTray: true
    $name: Arrow for apps running in the background (tray icons)
  - showVolume: true
    $name: Volume indicator
  - showMessages: true
    $name: Messages indicator (opens the notification centre)
  - showNetwork: true
    $name: Network indicator
  - showLanguage: true
    $name: Keyboard language indicator
  - doubleClick: restore
    $name: Double-click on the empty panel
    $options:
    - restore: Restore the maximized window in front
    - minimize: Minimize the maximized window in front
    - nothing: Nothing
  $name: Panel (top bar)
- launcher:
  - iconSize: 48
    $name: Icon size (px at 100% scaling, 32-80)
    $description: You can also drag the launcher's right edge to make it wider or narrower (remembered until you change this)
  - opacity: 80
    $name: Opacity (%)
    $description: Lower = more glass
  - tiles: colour
    $name: Icon tiles
    $options:
    - colour: Backlit in the icon's own colour (Unity)
    - plain: Plain grey
  - hoverZoom: true
    $name: Icons grow a little under the pointer
  - mediaControls: true
    $name: Music controls under the playing app (Spotify, browsers, ...)
  - discordControls: true
    $name: Call buttons under Discord while you're in a call (mute, deafen, hang up)
  $name: Launcher (left bar)
- dash:
  - width: 880
    $name: Width
  - height: 600
    $name: Height
  - opacity: 72
    $name: Opacity (%)
    $description: Lower = more glass
  - showWidgets: true
    $name: Widgets on the Home page
  $name: Dash (Start menu)
- look:
  - tint: accent
    $name: Colour (accent or a hex colour like 5A1A26)
  - glass: blur
    $name: Glass effect
    $description: What shows through the see-through parts of the panel, launcher and Dash
    $options:
    - blur: Blur (Windows' own blur)
    - acrylic: Acrylic (stronger blur, like Windows 11's own menus)
    - none: None (plain see-through)
  - animations: normal
    $name: Animation speed
    $options:
    - normal: Normal
    - fast: Fast
    - slow: Slow
    - off: Off
  - windowEffects: true
    $name: Window effects
    $description: The outline that flies when windows open, close, minimize and maximize
  $name: Look
- windows:
  - mergeMaximized: true
    $name: Maximized windows lose their title bar
    $description: The panel shows their title and buttons instead, like Unity
  - liftTitleBars: true
    $name: Tabs of maximized windows move into the panel
    $description: Browsers, File Explorer, Terminal and other windows with their own top bar
  - tabIcons: true
    $name: Site icons in the panel's tabs
  - mergeExplorer: true
    $name: Also for File Explorer
    $description: Explorer keeps its tab row; best with an Explorer theme without tabs
  - fullscreenHides: true
    $name: Panel and launcher slide away for full-screen games and videos
  $name: Windows
- system:
  - hideTaskbar: true
    $name: Hide the Windows taskbar
  - startOpensDash: true
    $name: Windows key / Ctrl+Esc open the Dash
  $name: Windows integration
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <uiautomation.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <tlhelp32.h>
#include <pdh.h>
#include <windhawk_utils.h>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Search.h>
#include <atomic>

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <vector>

// ============================================================================================================
// settings, logging, small helpers
// ============================================================================================================

static bool g_dutch;   // Windows is in Dutch: the shell too (see Tr)

struct Settings {
    int panelH, iconSize, dashW, dashH, opacity, panelOpacity, dashOpacity;
    std::wstring tint, panelStyle, glass, clock, doubleClick;
    bool hideTaskbar, startOpensDash, showWidgets, mergeMaximized, mergeExplorer, liftTitleBars;
    bool showTray, showVolume, showMessages, showNetwork, showLanguage;   // the panel's indicators
    bool plainTiles, hoverZoom, mediaControls, discordControls;           // the launcher
    bool windowEffects, tabIcons, fullscreenHides;
    float animScale = 1;                                                  // 0: no animations
} g_set;

static std::wstring StringSetting(const wchar_t* name, const wchar_t* fallback) {
    auto v = WindhawkUtils::StringSetting::make(name);
    return v.get() && *v.get() ? std::wstring(v.get()) : std::wstring(fallback);
}

static void LoadSettings() {
    g_dutch = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_DUTCH;
    g_set.panelH = std::clamp(Wh_GetIntSetting(L"panel.height"), 20, 40);
    g_set.panelStyle = StringSetting(L"panel.style", L"auto");
    g_set.panelOpacity = std::clamp(Wh_GetIntSetting(L"panel.opacity"), 20, 100);
    g_set.clock = StringSetting(L"panel.clock", L"time");
    g_set.showTray = Wh_GetIntSetting(L"panel.showTray") != 0;
    g_set.showVolume = Wh_GetIntSetting(L"panel.showVolume") != 0;
    g_set.showMessages = Wh_GetIntSetting(L"panel.showMessages") != 0;
    g_set.showNetwork = Wh_GetIntSetting(L"panel.showNetwork") != 0;
    g_set.showLanguage = Wh_GetIntSetting(L"panel.showLanguage") != 0;
    g_set.doubleClick = StringSetting(L"panel.doubleClick", L"restore");

    g_set.iconSize = std::clamp(Wh_GetIntSetting(L"launcher.iconSize"), 32, 80);
    // the launcher's right edge dragged wider or narrower: that size, until the setting itself changes
    int dragged = Wh_GetIntValue(L"launcherIcon", 0);
    if (dragged && Wh_GetIntValue(L"launcherIconFrom", 0) == g_set.iconSize) g_set.iconSize = std::clamp(dragged, 32, 80);
    g_set.opacity = std::clamp(Wh_GetIntSetting(L"launcher.opacity"), 20, 100);
    g_set.plainTiles = StringSetting(L"launcher.tiles", L"colour") == L"plain";
    g_set.hoverZoom = Wh_GetIntSetting(L"launcher.hoverZoom") != 0;
    g_set.mediaControls = Wh_GetIntSetting(L"launcher.mediaControls") != 0;
    g_set.discordControls = Wh_GetIntSetting(L"launcher.discordControls") != 0;

    g_set.dashW = std::clamp(Wh_GetIntSetting(L"dash.width"), 500, 2000);
    g_set.dashH = std::clamp(Wh_GetIntSetting(L"dash.height"), 350, 1400);
    g_set.dashOpacity = std::clamp(Wh_GetIntSetting(L"dash.opacity"), 20, 100);
    g_set.showWidgets = Wh_GetIntSetting(L"dash.showWidgets") != 0;

    g_set.tint = StringSetting(L"look.tint", L"accent");
    g_set.glass = StringSetting(L"look.glass", L"blur");
    std::wstring speed = StringSetting(L"look.animations", L"normal");
    g_set.animScale = speed == L"fast" ? 0.6f : speed == L"slow" ? 1.6f : speed == L"off" ? 0.f : 1.f;
    g_set.windowEffects = Wh_GetIntSetting(L"look.windowEffects") != 0;

    g_set.mergeMaximized = Wh_GetIntSetting(L"windows.mergeMaximized") != 0;
    g_set.liftTitleBars = Wh_GetIntSetting(L"windows.liftTitleBars") != 0;
    g_set.tabIcons = Wh_GetIntSetting(L"windows.tabIcons") != 0;
    g_set.mergeExplorer = Wh_GetIntSetting(L"windows.mergeExplorer") != 0;
    g_set.fullscreenHides = Wh_GetIntSetting(L"windows.fullscreenHides") != 0;

    g_set.hideTaskbar = Wh_GetIntSetting(L"system.hideTaskbar") != 0;
    g_set.startOpensDash = Wh_GetIntSetting(L"system.startOpensDash") != 0;

}

// the shell's texts: English, or Dutch when Windows itself is in Dutch. The texts are written in Dutch (the
// filters, app kinds and settings use them as keys too) and shown through Tr, which gives the English one
static const wchar_t* Tr(const wchar_t* nl) {
    if (g_dutch) return nl;
    static const std::map<std::wstring, const wchar_t*> english = {
        {L"Zoek op je computer", L"Search your computer"},
        {L"Prullenbak", L"Trash"},
        {L"Uit groep halen", L"Remove from group"},
        {L"Openen", L"Open"},
        {L"Nieuw venster", L"New window"},
        {L"Als administrator uitvoeren", L"Run as administrator"},
        {L"Bestandslocatie openen", L"Open file location"},
        {L"App-instellingen", L"App settings"},
        {L"Verwijderen", L"Uninstall"},
        {L"Naam wijzigen", L"Rename"},
        {L"Groep opheffen", L"Ungroup"},
        {L"Uit launcher verwijderen", L"Remove from launcher"},
        {L"Losmaken van launcher", L"Unpin from launcher"},
        {L"Vastmaken aan launcher", L"Pin to launcher"},
        {L"Alle vensters sluiten", L"Close all windows"},
        {L"Venster sluiten", L"Close window"},
        {L"Zoek op je computer en in instellingen", L"Search your computer and settings"},
        {L"Zoek applicaties", L"Search applications"},
        {L"Zoek bestanden en mappen", L"Search files and folders"},
        {L"Zoek video's", L"Search videos"},
        {L"Zoek muziek", L"Search music"},
        {L"Zoek foto's", L"Search photos"},
        {L"Beeldscherm", L"Display"},
        {L"Nachtlampje", L"Night light"},
        {L"Grafische instellingen", L"Graphics"},
        {L"Geluid", L"Sound"},
        {L"Volumemixer", L"Volume mixer"},
        {L"Meldingen", L"Notifications"},
        {L"Energie en batterij", L"Power & battery"},
        {L"Opslag", L"Storage"},
        {L"Klembord", L"Clipboard"},
        {L"Projecteren naar deze pc", L"Projecting to this PC"},
        {L"Systeeminfo", L"About"},
        {L"Herstel", L"Recovery"},
        {L"Activering", L"Activation"},
        {L"Bluetooth en apparaten", L"Bluetooth & devices"},
        {L"Printers en scanners", L"Printers & scanners"},
        {L"Muis", L"Mouse"},
        {L"Camera's", L"Cameras"},
        {L"Automatisch afspelen", L"AutoPlay"},
        {L"Netwerk en internet", L"Network & internet"},
        {L"Mobiele hotspot", L"Mobile hotspot"},
        {L"Vliegtuigmodus", L"Airplane mode"},
        {L"Achtergrond", L"Background"},
        {L"Kleuren", L"Colors"},
        {L"Thema's", L"Themes"},
        {L"Vergrendelingsscherm", L"Lock screen"},
        {L"Taakbalk", L"Taskbar"},
        {L"Lettertypen", L"Fonts"},
        {L"Geïnstalleerde apps", L"Installed apps"},
        {L"Standaard-apps", L"Default apps"},
        {L"Opstarten", L"Startup"},
        {L"Optionele onderdelen", L"Optional features"},
        {L"Uw info", L"Your info"},
        {L"Aanmeldingsopties", L"Sign-in options"},
        {L"E-mail en accounts", L"Email & accounts"},
        {L"Datum en tijd", L"Date & time"},
        {L"Taal en regio", L"Language & region"},
        {L"Toetsenbord", L"Keyboard"},
        {L"Spraak", L"Speech"},
        {L"Gamemodus", L"Game Mode"},
        {L"Opnames", L"Captures"},
        {L"Tekstgrootte", L"Text size"},
        {L"Muisaanwijzer en aanraken", L"Mouse pointer and touch"},
        {L"Vergrootglas", L"Magnifier"},
        {L"Kleurfilters", L"Color filters"},
        {L"Privacy en beveiliging", L"Privacy & security"},
        {L"Locatie", L"Location"},
        {L"Microfoon-privacy", L"Microphone privacy"},
        {L"Camera-privacy", L"Camera privacy"},
        {L"Windows-beveiliging", L"Windows Security"},
        {L"Voor ontwikkelaars", L"For developers"},
        {L"Configuratiescherm", L"Control Panel"},
        {L"Apparaatbeheer", L"Device Manager"},
        {L"Taakbeheer", L"Task Manager"},
        {L"Schijfbeheer", L"Disk Management"},
        {L"Systeemeigenschappen", L"System Properties"},
        {L"Energiebeheer (klassiek)", L"Power Options (classic)"},
        {L"Netwerkverbindingen", L"Network Connections"},
        {L"Geluid (klassiek)", L"Sound (classic)"},
        {L"Grafisch", L"Graphics"},
        {L"Ontwikkeling", L"Development"},
        {L"Kantoor", L"Office"},
        {L"Bureau-accessoires", L"Accessories"},
        {L"Aanpassen", L"Customization"},
        {L"Overig", L"Other"},
        {L"Systeem", L"System"},
        {L"Groep", L"Group"},
        {L"Categorieën", L"Categories"},
        {L"Applicaties", L"Applications"},
        {L"Instellingen", L"Settings"},
        {L"Bestanden en mappen", L"Files & folders"},
        {L"Bronnen", L"Sources"},
        {L"Laatst gewijzigd", L"Last modified"},
        {L"Afgelopen 7 dagen", L"Last 7 days"},
        {L"Afgelopen 30 dagen", L"Last 30 days"},
        {L"Afgelopen jaar", L"Last year"},
        {L"Documenten", L"Documents"},
        {L"Mappen", L"Folders"},
        {L"Afbeeldingen", L"Images"},
        {L"Video's", L"Videos"},
        {L"Presentaties", L"Presentations"},
        {L"Recente bestanden", L"Recent files"},
        {L"Recent gebruikt", L"Recently used"},
        {L"Muziek", L"Music"},
        {L"Foto's", L"Photos"},
        {L"Vandaag", L"Today"},
        {L"Nu aan het spelen", L"Now playing"},
        {L"Er speelt niets", L"Nothing playing"},
        {L"%.2ls  %.0f GB vrij", L"%.2ls  %.0f GB free"},
        {L"Weer", L"Weather"},
        {L"Open de Weer-app", L"Open the Weather app"},
        {L"Weer-app installeren", L"Install the Weather app"},
        {L"Resultaten filteren  ▾", L"Filter results  ▾"},
        {L"Resultaten filteren  ▸", L"Filter results  ▸"},
        {L"Minder tonen", L"Show less"},
        {L"Alles tonen", L"Show all"},
        {L"Zoeken…", L"Search…"},
        {L"Sorry, er is niets gevonden.", L"Sorry, nothing was found."},
        {L"Geen filters voor deze lens.", L"No filters for this lens."}
    };
    auto it = english.find(nl);
    return it != english.end() ? it->second : nl;
}

static std::wstring Lower(std::wstring s) {
    CharLowerBuffW(s.data(), (DWORD)s.size());
    return s;
}

static bool StartsWith(const std::wstring& s, const std::wstring& p) {
    return s.size() >= p.size() && _wcsnicmp(s.c_str(), p.c_str(), p.size()) == 0;
}

template <class T>
static void Release(T*& p) {
    if (p) { p->Release(); p = nullptr; }
}

static const wchar_t kAppsFolder[] = L"shell:AppsFolder\\";

// property keys, spelled out so they don't depend on a library defining them
static const PROPERTYKEY kKeyAumid = {{0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}}, 5};
static const PROPERTYKEY kKeyLinkTarget = {{0xB9B4B3FC, 0x2B51, 0x4A42, {0xB5, 0xD8, 0x32, 0x41, 0x46, 0xAF, 0xCF, 0x25}}, 2};

// ============================================================================================================
// global state (everything below runs on the shell's own UI thread, except the Explorer hooks)
// ============================================================================================================

static HANDLE g_uiThread;
static DWORD g_uiThreadId;
static volatile bool g_running;       // the shell's windows are up
static HWND g_host, g_panel, g_launcher, g_dash, g_tray;
static double g_scale = 1;            // DPI scale
static UINT g_shellHookMsg;
static int g_taskbarWasAutoHide = -1;
static DWORD g_lastToggle;   // when the Dash was last opened/closed by Start
static volatile bool g_dashOpen;   // the Dash is open (or opening); false while it fades out
static bool g_dashMax;             // maximized next to the launcher
static HINSTANCE ModInstance();
static bool g_hiddenForFullscreen; // a full-screen game or video is in front: the shell steps aside
static int g_awayPanel, g_awayLauncher;   // pixels the panel / launcher are slid out of sight (stepping aside)
// now playing (filled by the media thread, see MediaWorker)
static std::mutex g_mediaLock;
static bool g_mediaReady;          // the lock exists
static std::wstring g_mTitle, g_mArtist, g_mSource;   // g_mSource: the player's app id
static bool g_mHas, g_mPlaying;
static IWICBitmapSource* g_mCover;
static int g_mCoverKey;
static volatile LONG g_mediaCmd;   // 1 previous, 2 play/pause, 3 next
static HANDLE g_mediaWake, g_mediaThread;

enum : UINT {
    WM_APP_TOGGLEDASH = WM_APP + 1,
    WM_APP_REFRESH,
};

enum : UINT_PTR {
    TIMER_REFRESH = 1,
    TIMER_CLOCK,
    TIMER_SOON,
    TIMER_FILESEARCH,
    TIMER_WIDGETS,
    TIMER_TASKBAR,
    TIMER_STARTESC,
    TIMER_ANIM,
    TIMER_PREVIEWSHOW,
    TIMER_PREVIEWHIDE,
    TIMER_GHOST,
    TIMER_CHECKWIN,
};

static int Px(double dip) { return (int)(dip * g_scale + 0.5); }

// ============================================================================================================
// colours: the Windows accent (or the chosen hex colour), darkened like Unity's tinted panels
// ============================================================================================================

struct Rgb { float r, g, b; };

static Rgb Shade(Rgb c, float f) {
    return {std::clamp(c.r * f, 0.f, 1.f), std::clamp(c.g * f, 0.f, 1.f), std::clamp(c.b * f, 0.f, 1.f)};
}

static Rgb g_accent = {0.91f, 0.33f, 0.13f};   // Ubuntu orange until we know better
static Rgb g_tint = {0.35f, 0.10f, 0.15f};

static void ReadColours() {
    DWORD color = 0, size = sizeof(color);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"AccentColor", RRF_RT_REG_DWORD, nullptr, &color, &size) == ERROR_SUCCESS) {
        // stored as 0xAABBGGRR
        g_accent = {(color & 0xFF) / 255.f, ((color >> 8) & 0xFF) / 255.f, ((color >> 16) & 0xFF) / 255.f};
    }
    std::wstring hex = !g_set.tint.empty() && g_set.tint[0] == L'#' ? g_set.tint.substr(1) : g_set.tint;
    if (hex.size() == 6 && hex.find_first_not_of(L"0123456789abcdefABCDEF") == std::wstring::npos) {
        unsigned v = wcstoul(hex.c_str(), nullptr, 16);
        g_tint = {((v >> 16) & 0xFF) / 255.f, ((v >> 8) & 0xFF) / 255.f, (v & 0xFF) / 255.f};
    } else {
        g_tint = Shade(g_accent, 0.45f);
    }
}

// ============================================================================================================
// drawing: Direct2D into a DIB, shown with per-pixel alpha (UpdateLayeredWindow), blurred behind by DWM
// ============================================================================================================

static ID2D1Factory* g_d2d;
static IDWriteFactory* g_dw;
static IWICImagingFactory* g_wic;
static std::wstring g_fontFamily = L"Segoe UI";
static const wchar_t kGlyphs[] = L"Segoe Fluent Icons";

struct Surface {
    HWND hwnd = nullptr;
    int w = 0, h = 0;            // pixels
    HDC dc = nullptr;
    HBITMAP bmp = nullptr, old = nullptr;
    ID2D1DCRenderTarget* rt = nullptr;
    ID2D1SolidColorBrush* brush = nullptr;
    std::map<std::wstring, ID2D1Bitmap*> bitmaps;

    void Reset() {
        for (auto& kv : bitmaps) Release(kv.second);
        bitmaps.clear();
        Release(brush);
        Release(rt);
    }

    void Free() {
        Reset();
        if (dc) {
            SelectObject(dc, old);
            DeleteObject(bmp);
            DeleteDC(dc);
            dc = nullptr;
        }
    }

    bool Begin(int pw, int ph) {
        if (pw <= 0 || ph <= 0) return false;
        if (!dc || pw != w || ph != h) {
            if (dc) { SelectObject(dc, old); DeleteObject(bmp); DeleteDC(dc); }
            w = pw;
            h = ph;
            BITMAPINFO bi = {};
            bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
            bi.bmiHeader.biWidth = w;
            bi.bmiHeader.biHeight = -h;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            void* bits = nullptr;
            dc = CreateCompatibleDC(nullptr);
            bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
            old = (HBITMAP)SelectObject(dc, bmp);
        }
        if (!rt) {
            auto props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
            if (FAILED(g_d2d->CreateDCRenderTarget(&props, &rt))) return false;
            rt->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &brush);
        }
        RECT r = {0, 0, w, h};
        if (FAILED(rt->BindDC(dc, &r))) return false;
        rt->SetDpi(96.f * (float)g_scale, 96.f * (float)g_scale);   // draw in DIPs
        rt->BeginDraw();
        rt->Clear(D2D1::ColorF(0, 0, 0, 0));
        rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        return true;
    }

    void Finish() {
        if (rt->EndDraw() == (HRESULT)D2DERR_RECREATE_TARGET) Reset();
    }

    // show a part of the picture (sx, sy, cw x ch) in a window at x, y
    void Present(HWND to, int x, int y, int sx, int sy, int cw, int ch, BYTE alpha) {
        if (!to || cw <= 0 || ch <= 0) return;
        POINT pos = {x, y}, src = {sx, sy};
        SIZE size = {cw, ch};
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        HDC screen = GetDC(nullptr);
        UpdateLayeredWindow(to, screen, &pos, &size, dc, &src, 0, &blend, ULW_ALPHA);
        ReleaseDC(nullptr, screen);
    }

    void End(int x, int y, BYTE alpha = 255) {
        Finish();
        Present(hwnd, x, y, 0, 0, w, h, alpha);
    }

    ID2D1SolidColorBrush* Brush(Rgb c, float a) {
        brush->SetColor(D2D1::ColorF(c.r, c.g, c.b, a));
        return brush;
    }

    ID2D1SolidColorBrush* White(float a) { return Brush({1, 1, 1}, a); }
};

static Surface g_panelS, g_launcherS, g_dashS;

static std::map<std::wstring, IDWriteTextFormat*> g_formats;   // see Text

static void Text(Surface& s, const std::wstring& text, D2D1_RECT_F r, float size, float alpha,
                 DWRITE_TEXT_ALIGNMENT align = DWRITE_TEXT_ALIGNMENT_LEADING, bool wrap = false,
                 const wchar_t* family = nullptr, DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL, Rgb colour = {1, 1, 1}) {
    // the format is made once per look (font, size, weight, alignment, wrapping) and kept: animations draw a lot of
    // text, many times a second
    const wchar_t* font = family ? family : g_fontFamily.c_str();
    wchar_t key[160];
    swprintf(key, 160, L"%s|%.2f|%d|%d|%d", font, size, (int)weight, (int)align, wrap ? 1 : 0);
    IDWriteTextFormat*& fmt = g_formats[key];
    if (!fmt) {
        if (FAILED(g_dw->CreateTextFormat(font, nullptr, weight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"", &fmt)) || !fmt) {
            g_formats.erase(key);
            return;
        }
        fmt->SetTextAlignment(align);
        fmt->SetParagraphAlignment(wrap ? DWRITE_PARAGRAPH_ALIGNMENT_NEAR : DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        fmt->SetWordWrapping(wrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);
        DWRITE_TRIMMING trim = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        IDWriteInlineObject* ellipsis = nullptr;
        g_dw->CreateEllipsisTrimmingSign(fmt, &ellipsis);
        fmt->SetTrimming(&trim, ellipsis);   // the format keeps its own reference
        Release(ellipsis);
    }
    s.rt->DrawText(text.c_str(), (UINT32)text.size(), fmt, r, s.Brush(colour, alpha), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

static void FreeFormats() {
    for (auto& kv : g_formats) Release(kv.second);
    g_formats.clear();
}

static void Glyph(Surface& s, const wchar_t* glyph, D2D1_RECT_F r, float size, float alpha, Rgb colour = {1, 1, 1}) {
    Text(s, glyph, r, size, alpha, DWRITE_TEXT_ALIGNMENT_CENTER, false, kGlyphs, DWRITE_FONT_WEIGHT_NORMAL, colour);
}

static void PickFont() {
    IDWriteFontCollection* fonts = nullptr;
    if (SUCCEEDED(g_dw->GetSystemFontCollection(&fonts, FALSE))) {
        UINT32 index;
        BOOL exists = FALSE;
        if (SUCCEEDED(fonts->FindFamilyName(L"Ubuntu", &index, &exists)) && exists) g_fontFamily = L"Ubuntu";
        fonts->Release();
    }
}

// blur behind a window (the same "accent" Windows uses for its own blurred surfaces)
static void BlurBehind(HWND hwnd, bool on = true) {
    struct ACCENTPOLICY { int state, flags, color, anim; };
    struct WINCOMPATTRDATA { int attr; void* data; SIZE_T size; };
    using SetWCA_t = BOOL(WINAPI*)(HWND, WINCOMPATTRDATA*);
    static auto setWca = (SetWCA_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (!setWca) return;
    // the glass setting: Windows' blur, acrylic (its colour nearly clear: the shell paints its own tint over it) or none
    int state = !on || g_set.glass == L"none" ? 0 : g_set.glass == L"acrylic" ? 4 /* ACCENT_ENABLE_ACRYLICBLURBEHIND */ : 3 /* ACCENT_ENABLE_BLURBEHIND */;
    ACCENTPOLICY policy = {state, 0, state == 4 ? 0x01000000 : 0, 0};
    WINCOMPATTRDATA data = {19 /* WCA_ACCENT_POLICY */, &policy, sizeof(policy)};
    setWca(hwnd, &data);
}

// ============================================================================================================
// icons: any app / file / Store app through the shell's image factory, cached
// ============================================================================================================

struct Icon {
    IWICBitmapSource* src = nullptr;
    Rgb average = {0.5f, 0.5f, 0.5f};
};
static std::map<std::wstring, Icon> g_icons;

static Icon* GetIcon(const std::wstring& parsing, bool thumb = false) {
    std::wstring key = (thumb ? L"t|" : L"") + parsing;
    auto it = g_icons.find(key);
    if (it != g_icons.end()) return it->second.src ? &it->second : nullptr;
    Icon& icon = g_icons[key];
    IWICBitmap* wbmp = nullptr;
    HRESULT hr;
    if (parsing.rfind(L"hwnd:", 0) == 0) {   // "hwnd:<hex>": the window's own icon (an app Windows has no icon for)
        HWND w = (HWND)(UINT_PTR)wcstoull(parsing.c_str() + 5, nullptr, 16);
        DWORD_PTR got = 0;
        HICON hi = nullptr;
        if (SendMessageTimeoutW(w, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG, 200, &got) && got) hi = (HICON)got;
        if (!hi) hi = (HICON)GetClassLongPtrW(w, GCLP_HICON);
        if (!hi && SendMessageTimeoutW(w, WM_GETICON, ICON_SMALL2, 0, SMTO_ABORTIFHUNG, 200, &got) && got) hi = (HICON)got;
        if (!hi || FAILED(g_wic->CreateBitmapFromHICON(hi, &wbmp))) return nullptr;
    } else {
        IShellItemImageFactory* factory = nullptr;
        if (FAILED(SHCreateItemFromParsingName(parsing.c_str(), nullptr, IID_PPV_ARGS(&factory)))) return nullptr;
        int px = std::clamp(Px(g_set.iconSize) * 2, 128, 256);   // big: scaled down well for each place it's drawn
        HBITMAP hbmp = nullptr;
        if (thumb) px = std::max(px, 112);
        hr = factory->GetImage(SIZE{px, px}, thumb ? SIIGBF_BIGGERSIZEOK : SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &hbmp);
        factory->Release();
        if (FAILED(hr) || !hbmp) return nullptr;
        hr = g_wic->CreateBitmapFromHBITMAP(hbmp, nullptr, WICBitmapUsePremultipliedAlpha, &wbmp);
        DeleteObject(hbmp);
        if (FAILED(hr)) return nullptr;
    }
    IWICFormatConverter* conv = nullptr;
    g_wic->CreateFormatConverter(&conv);
    if (!conv || FAILED(conv->Initialize(wbmp, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom))) {
        Release(conv);
        wbmp->Release();
        return nullptr;
    }
    wbmp->Release();
    icon.src = conv;
    // the icon's main colour, for the launcher tile's backlight (like Unity)
    UINT w = 0, h = 0;
    conv->GetSize(&w, &h);
    if (w && h && w <= 512 && h <= 512) {
        std::vector<BYTE> px4(w * h * 4);
        if (SUCCEEDED(conv->CopyPixels(nullptr, w * 4, (UINT)px4.size(), px4.data()))) {
            double r = 0, g = 0, b = 0, weight = 0;
            for (size_t i = 0; i < px4.size(); i += 4) {
                double a = px4[i + 3] / 255.0;
                if (a < 0.5) continue;
                double bb = px4[i] / 255.0 / a, gg = px4[i + 1] / 255.0 / a, rr = px4[i + 2] / 255.0 / a;
                double sat = std::max({rr, gg, bb}) - std::min({rr, gg, bb});   // colourful pixels count more
                double wgt = 0.15 + sat;
                r += rr * wgt; g += gg * wgt; b += bb * wgt; weight += wgt;
            }
            if (weight > 0) icon.average = {(float)(r / weight), (float)(g / weight), (float)(b / weight)};
        }
    }
    return &icon;
}

// px > 0: scaled once, with a high-quality filter, to exactly the size it's drawn at (Direct2D's own scaling of a
// big icon down to a small one leaves jagged edges)
static ID2D1Bitmap* IconBitmap(Surface& s, const std::wstring& parsing, bool thumb = false, int px = 0) {
    std::wstring key = (thumb ? L"t|" : L"") + parsing + (px > 0 ? L"@" + std::to_wstring(px) : L"");
    auto it = s.bitmaps.find(key);
    if (it != s.bitmaps.end()) return it->second;
    Icon* icon = GetIcon(parsing, thumb);
    ID2D1Bitmap* bmp = nullptr;
    UINT w = 0, h = 0;
    if (icon && px > 0 && SUCCEEDED(icon->src->GetSize(&w, &h)) && w && h && ((int)w != px || (int)h != px)) {
        UINT sw = w >= h ? px : std::max(1u, (UINT)(px * w / h)), sh = h >= w ? px : std::max(1u, (UINT)(px * h / w));
        IWICBitmapScaler* scaler = nullptr;
        IWICBitmap* scaled = nullptr;
        if (SUCCEEDED(g_wic->CreateBitmapScaler(&scaler)) &&
            (SUCCEEDED(scaler->Initialize(icon->src, sw, sh, (WICBitmapInterpolationMode)4 /* HighQualityCubic */)) ||
             SUCCEEDED(scaler->Initialize(icon->src, sw, sh, WICBitmapInterpolationModeFant))) &&
            SUCCEEDED(g_wic->CreateBitmapFromSource(scaler, WICBitmapCacheOnLoad, &scaled)))
            s.rt->CreateBitmapFromWicBitmap(scaled, nullptr, &bmp);
        Release(scaled);
        Release(scaler);
    }
    if (icon && !bmp) s.rt->CreateBitmapFromWicBitmap(icon->src, nullptr, &bmp);
    s.bitmaps[key] = bmp;
    return bmp;
}

// an icon drawn crisp: on whole pixels, from a copy scaled to exactly that size
static void DrawIcon(Surface& s, const std::wstring& parsing, D2D1_RECT_F r, float alpha) {
    int px = Px(r.right - r.left);
    if (px <= 0) return;
    ID2D1Bitmap* bmp = IconBitmap(s, parsing, false, px);
    if (!bmp) return;
    float left = std::round(r.left * (float)g_scale) / (float)g_scale, top = std::round(r.top * (float)g_scale) / (float)g_scale;
    D2D1_SIZE_U sz = bmp->GetPixelSize();
    float bw = sz.width / (float)g_scale, bh = sz.height / (float)g_scale;
    float x = left + std::round((px - (int)sz.width) / 2.f) / (float)g_scale, y = top + std::round((px - (int)sz.height) / 2.f) / (float)g_scale;
    s.rt->DrawBitmap(bmp, D2D1::RectF(x, y, x + bw, y + bh), alpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
}

// ============================================================================================================
// apps (the same list as Start's "All apps") and running windows
// ============================================================================================================

struct App {
    std::wstring name, id;   // id: AppsFolder parsing name (an AppUserModelID or a path)
    std::wstring nameLower;
};
static std::vector<App> g_apps;
static DWORD g_appsAt;

static void LoadApps() {
    g_apps.clear();
    IShellItem* folder = nullptr;
    if (FAILED(SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_PPV_ARGS(&folder)))) return;
    IEnumShellItems* items = nullptr;
    if (SUCCEEDED(folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&items)))) {
        IShellItem* item;
        while (items->Next(1, &item, nullptr) == S_OK) {
            PWSTR name = nullptr, id = nullptr;
            item->GetDisplayName(SIGDN_NORMALDISPLAY, &name);
            item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &id);
            if (name && id) {
                std::wstring n = name, lower = Lower(name);
                // leave out uninstallers, readmes and the like
                bool junk = lower.find(L"uninstall") != std::wstring::npos || lower.find(L"verwijder") != std::wstring::npos ||
                            lower.find(L"readme") != std::wstring::npos || lower.find(L"help") == 0;
                if (!junk) g_apps.push_back({n, id, lower});
            }
            CoTaskMemFree(name);
            CoTaskMemFree(id);
            item->Release();
        }
        items->Release();
    }
    folder->Release();
    std::sort(g_apps.begin(), g_apps.end(), [](const App& a, const App& b) { return a.nameLower < b.nameLower; });
    g_appsAt = GetTickCount();
    Wh_Log(L"%d apps", (int)g_apps.size());
}

static std::wstring ProcessPath(DWORD pid) {
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return L"";
    wchar_t buf[MAX_PATH * 2];
    DWORD size = ARRAYSIZE(buf);
    std::wstring path;
    if (QueryFullProcessImageNameW(h, 0, buf, &size)) path = buf;
    CloseHandle(h);
    return path;
}

static std::wstring WindowAumid(HWND hwnd) {
    IPropertyStore* ps = nullptr;
    std::wstring id;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(&ps)))) {
        PROPVARIANT v;
        PropVariantInit(&v);
        if (SUCCEEDED(ps->GetValue(kKeyAumid, &v)) && v.vt == VT_LPWSTR && v.pwszVal) id = v.pwszVal;
        PropVariantClear(&v);
        ps->Release();
    }
    return id;
}

static std::wstring WindowTitle(HWND hwnd) {
    wchar_t buf[256];
    int n = GetWindowTextW(hwnd, buf, ARRAYSIZE(buf));
    return std::wstring(buf, std::max(0, n));
}

struct Win {
    HWND hwnd;
    std::wstring title, exe, aumid;   // exe lowercased
    bool minimized;
};

static std::map<DWORD, std::wstring> g_pidExe;

static BOOL CALLBACK CollectWindow(HWND h, LPARAM l) {
    auto& list = *(std::vector<Win>*)l;
    if (!IsWindowVisible(h) || GetWindow(h, GW_OWNER)) return TRUE;
    LONG ex = GetWindowLongW(h, GWL_EXSTYLE);
    if ((ex & WS_EX_TOOLWINDOW) && !(ex & WS_EX_APPWINDOW)) return TRUE;
    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) return TRUE;
    wchar_t cls[64];
    GetClassNameW(h, cls, 64);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") || !wcscmp(cls, L"Shell_TrayWnd") ||
        !wcscmp(cls, L"Shell_SecondaryTrayWnd") || !wcscmp(cls, L"Windows.UI.Core.CoreWindow")) return TRUE;
    if (h == g_panel || h == g_launcher || h == g_dash) return TRUE;
    std::wstring title = WindowTitle(h);
    if (title.empty()) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    auto it = g_pidExe.find(pid);
    std::wstring exe = it != g_pidExe.end() ? it->second : (g_pidExe[pid] = Lower(ProcessPath(pid)));
    list.push_back({h, title, exe, WindowAumid(h), IsIconic(h) != FALSE});
    return TRUE;
}

static std::vector<Win> Snapshot() {
    std::vector<Win> list;
    EnumWindows(CollectWindow, (LPARAM)&list);
    if (g_pidExe.size() > 400) g_pidExe.clear();
    return list;
}

// ============================================================================================================
// the launcher's items: pins (kept in the mod's storage) followed by running apps that aren't pinned
// ============================================================================================================

struct Item {
    std::wstring launch;   // what ShellExecute opens: "shell:AppsFolder\<id>" or a path
    std::wstring name;
    std::wstring aumid, exe;   // for matching windows (exe lowercased)
    bool pinned = false;
    std::vector<HWND> windows;
    bool focused = false;
    int folder = -1;   // a group: its index in g_folders, and its apps (aumid, exe) for matching windows
    std::vector<std::pair<std::wstring, std::wstring>> members;
};
static std::vector<std::wstring> g_pins;
static std::vector<Item> g_items;
// groups on the launcher (pinned as "folder:<id>"): a name and their apps, kept in the mod's storage
struct PinGroup {
    std::wstring id, name;
    std::vector<std::wstring> apps;
};
static std::vector<PinGroup> g_folders;

static int FolderIndex(const std::wstring& id) {
    for (int i = 0; i < (int)g_folders.size(); i++)
        if (g_folders[i].id == id) return i;
    return -1;
}

static void SaveFolders() {   // one line per group: id, name, apps, separated by tabs
    std::wstring all;
    for (auto& f : g_folders) {
        all += f.id + L"\t" + f.name;
        for (auto& a : f.apps) all += L"\t" + a;
        all += L"\n";
    }
    Wh_SetStringValue(L"folders", all.c_str());
}

static void LoadFolders() {
    g_folders.clear();
    std::vector<wchar_t> buf(65536);
    if (!Wh_GetStringValue(L"folders", buf.data(), (int)buf.size())) return;
    std::wstring s = buf.data();
    size_t start = 0, nl;
    while ((nl = s.find(L'\n', start)) != std::wstring::npos) {
        std::wstring line = s.substr(start, nl - start);
        start = nl + 1;
        std::vector<std::wstring> parts;
        size_t a = 0, tab;
        while ((tab = line.find(L'\t', a)) != std::wstring::npos) {
            parts.push_back(line.substr(a, tab - a));
            a = tab + 1;
        }
        parts.push_back(line.substr(a));
        if (parts.size() < 3) continue;
        PinGroup f;
        f.id = parts[0];
        f.name = parts[1];
        f.apps.assign(parts.begin() + 2, parts.end());
        g_folders.push_back(f);
    }
}


static std::map<std::wstring, std::pair<std::wstring, std::wstring>> g_resolved;   // launch -> (aumid, exe)
static HWND g_foreground;

static void SavePins() {
    std::wstring all;
    for (auto& p : g_pins) all += p + L"\n";
    Wh_SetStringValue(L"pins", all.c_str());
}

static void LoadPins() {
    g_pins.clear();
    wchar_t buf[8192] = {};
    if (Wh_GetStringValue(L"pins", buf, ARRAYSIZE(buf)) == 0) {
        // first run: Unity-like defaults
        g_pins = {std::wstring(kAppsFolder) + L"Microsoft.Windows.Explorer", std::wstring(kAppsFolder) + L"MSEdge",
                  std::wstring(kAppsFolder) + L"windows.immersivecontrolpanel_cw5n1h2txyewy!microsoft.windows.immersivecontrolpanel"};
        SavePins();
        return;
    }
    std::wstring s = buf;
    size_t start = 0, nl;
    while ((nl = s.find(L'\n', start)) != std::wstring::npos) {
        if (nl > start) g_pins.push_back(s.substr(start, nl - start));
        start = nl + 1;
    }
}

// what a pin is, for matching it to windows: its AppUserModelID and/or the program it runs
static std::pair<std::wstring, std::wstring> Resolve(const std::wstring& launch) {
    auto it = g_resolved.find(launch);
    if (it != g_resolved.end()) return it->second;
    std::wstring aumid, exe;
    IShellItem2* item = nullptr;
    if (StartsWith(launch, kAppsFolder)) aumid = launch.substr(wcslen(kAppsFolder));
    if (SUCCEEDED(SHCreateItemFromParsingName(launch.c_str(), nullptr, IID_PPV_ARGS(&item)))) {
        PWSTR target = nullptr;
        if (SUCCEEDED(item->GetString(kKeyLinkTarget, &target)) && target) exe = target;
        CoTaskMemFree(target);
        if (exe.empty()) {
            PWSTR path = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) && path) exe = path;
            CoTaskMemFree(path);
        }
        item->Release();
    }
    // AppsFolder ids for older desktop apps are "{KNOWNFOLDERID}\relative\path.exe"
    if (exe.empty() && !aumid.empty() && aumid[0] == L'{') {
        size_t close = aumid.find(L'}');
        GUID guid;
        if (close != std::wstring::npos && SUCCEEDED(CLSIDFromString(aumid.substr(0, close + 1).c_str(), &guid))) {
            PWSTR base = nullptr;
            if (SUCCEEDED(SHGetKnownFolderPath(guid, 0, nullptr, &base))) exe = std::wstring(base) + aumid.substr(close + 1);
            CoTaskMemFree(base);
        }
    }
    if (exe.size() > 4 && Lower(exe.substr(exe.size() - 4)) != L".exe") exe.clear();
    auto result = std::make_pair(Lower(aumid), Lower(exe));
    g_resolved[launch] = result;
    return result;
}

static std::wstring DisplayName(const std::wstring& launch) {
    if (StartsWith(launch, kAppsFolder)) {
        std::wstring id = launch.substr(wcslen(kAppsFolder));
        for (auto& a : g_apps)
            if (_wcsicmp(a.id.c_str(), id.c_str()) == 0) return a.name;
    }
    IShellItem* item = nullptr;
    std::wstring name;
    if (SUCCEEDED(SHCreateItemFromParsingName(launch.c_str(), nullptr, IID_PPV_ARGS(&item)))) {
        PWSTR n = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &n)) && n) name = n;
        CoTaskMemFree(n);
        item->Release();
    }
    if (name.size() > 4 && Lower(name.substr(name.size() - 4)) == L".exe") name = name.substr(0, name.size() - 4);
    return name;
}

static bool Matches(const Item& it, const Win& w) {
    for (auto& m : it.members) {   // a group: the windows of any of its apps
        Item one;
        one.aumid = m.first;
        one.exe = m.second;
        if (Matches(one, w)) return true;
    }
    if (!it.aumid.empty() && Lower(w.aumid) == it.aumid) return true;
    if (!it.exe.empty() && w.exe == it.exe && w.exe.find(L"applicationframehost") == std::wstring::npos) return true;
    return false;
}

static std::map<std::wstring, std::wstring> g_names;   // launch -> display name (cached)

static std::wstring NameFor(const std::wstring& launch) {
    auto it = g_names.find(launch);
    if (it != g_names.end()) return it->second;
    return g_names[launch] = DisplayName(launch);
}

// the order running apps and their windows showed up in, so icons keep their place: Windows lists windows front to
// back, and that changes with every click
static std::map<std::wstring, unsigned> g_runSeq;
static std::map<HWND, unsigned> g_winSeq;
static unsigned g_seq;
static std::map<HWND, int> g_zRank;   // front to back: which of an app's windows was used last

static void BuildItems() {
    auto wins = Snapshot();
    g_zRank.clear();
    for (int i = 0; i < (int)wins.size(); i++) g_zRank[wins[i].hwnd] = i;
    g_foreground = GetForegroundWindow();
    std::vector<Item> items;
    for (auto& p : g_pins) {
        Item it;
        it.launch = p;
        it.pinned = true;
        if (StartsWith(p, L"folder:")) {
            it.folder = FolderIndex(p.substr(7));
            if (it.folder < 0) continue;
            for (auto& m : g_folders[it.folder].apps) it.members.push_back(Resolve(m));
        } else {
            auto r = Resolve(p);
            it.aumid = r.first;
            it.exe = r.second;
        }
        items.push_back(it);
    }
    for (auto& w : wins) {
        Item* owner = nullptr;
        for (auto& it : items)
            if (Matches(it, w)) { owner = &it; break; }
        if (!owner) {
            Item it;
            it.launch = !w.aumid.empty() ? std::wstring(kAppsFolder) + w.aumid : w.exe;
            it.aumid = Lower(w.aumid);
            it.exe = w.aumid.empty() ? w.exe : L"";
            items.push_back(it);
            owner = &items.back();
        }
        owner->windows.push_back(w.hwnd);
        if (w.hwnd == g_foreground && !IsIconic(w.hwnd)) owner->focused = true;   // a minimized window can still be "in front"
    }
    std::map<std::wstring, unsigned> runSeq;
    std::map<HWND, unsigned> winSeq;
    for (auto& it : items) {
        for (HWND w : it.windows) {
            auto known = g_winSeq.find(w);
            winSeq[w] = known != g_winSeq.end() ? known->second : ++g_seq;
        }
        std::sort(it.windows.begin(), it.windows.end(), [&](HWND a, HWND b) { return winSeq[a] < winSeq[b]; });
        if (it.pinned) continue;
        auto known = g_runSeq.find(it.launch);
        runSeq[it.launch] = known != g_runSeq.end() ? known->second : ++g_seq;
    }
    size_t pinned = std::min(g_pins.size(), items.size());
    std::stable_sort(items.begin() + pinned, items.end(), [&](const Item& a, const Item& b) { return runSeq[a.launch] < runSeq[b.launch]; });
    g_runSeq = std::move(runSeq);
    g_winSeq = std::move(winSeq);
    for (auto& it : items) it.name = it.folder >= 0 ? g_folders[it.folder].name : NameFor(it.launch);
    g_items = std::move(items);
}

// ============================================================================================================
// actions
// ============================================================================================================

static void Launch(const std::wstring& what, const wchar_t* verb = nullptr) {
    SHELLEXECUTEINFOW sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = verb;
    sei.lpFile = what.c_str();
    sei.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&sei)) Wh_Log(L"could not open %s (%u)", what.c_str(), GetLastError());
}

// windows looked at again a moment after a restore or minimize: some apps ignore the polite way
static std::vector<std::pair<HWND, bool>> g_checkLater;   // the window, and whether it should be minimized

static void CheckLater(HWND hwnd, bool minimized) {
    g_checkLater.push_back({hwnd, minimized});
    if (g_host) SetTimer(g_host, TIMER_CHECKWIN, 280, nullptr);
}

static void CheckWindowsNow() {
    for (auto& c : g_checkLater) {
        if (!IsWindow(c.first)) continue;
        if (c.second && !IsIconic(c.first)) ShowWindowAsync(c.first, SW_MINIMIZE);
        if (!c.second && IsIconic(c.first)) SwitchToThisWindow(c.first, TRUE);   // Windows lets Alt+Tab's switch through (apps run as administrator)
    }
    g_checkLater.clear();
}

// bring a window to the front with the focus, restoring it when minimized (the app's way: borrow the input of the
// window in front for a moment, so Windows lets us hand the focus over)
static void Activate(HWND hwnd) {
    bool wasMinimized = IsIconic(hwnd);
    if (wasMinimized) ShowWindowAsync(hwnd, SW_RESTORE);
    // an empty input event first: Windows then lets the shell hand the focus over, even when the last real input
    // wasn't for one of its windows (the Dash opened by the Windows key)
    INPUT none = {};
    SendInput(1, &none, sizeof(none));
    SetForegroundWindow(hwnd);
    if (wasMinimized) CheckLater(hwnd, false);
}

// like the window's own minimize button (apps with their own title bar listen to that), else a plain minimize
static void Minimize(HWND hwnd) {
    PostMessageW(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    CheckLater(hwnd, true);
}

static bool IsPinned(const std::wstring& launch) {
    for (auto& p : g_pins)
        if (_wcsicmp(p.c_str(), launch.c_str()) == 0) return true;
    return false;
}

static void Pin(const std::wstring& launch, bool on) {
    auto same = [&](const std::wstring& p) { return _wcsicmp(p.c_str(), launch.c_str()) == 0; };
    g_pins.erase(std::remove_if(g_pins.begin(), g_pins.end(), same), g_pins.end());
    if (on) g_pins.push_back(launch);
    SavePins();
}

// ============================================================================================================
// volume (default speakers)
// ============================================================================================================

static IAudioEndpointVolume* Endpoint() {
    IMMDeviceEnumerator* en = nullptr;
    IMMDevice* dev = nullptr;
    IAudioEndpointVolume* vol = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&en))) &&
        SUCCEEDED(en->GetDefaultAudioEndpoint(eRender, eConsole, &dev)))
        dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&vol);
    Release(dev);
    Release(en);
    return vol;
}

static float g_volume = 0.5f;
static bool g_muted;

static void ReadVolume() {
    if (auto* v = Endpoint()) {
        v->GetMasterVolumeLevelScalar(&g_volume);
        BOOL m = FALSE;
        v->GetMute(&m);
        g_muted = m != FALSE;
        v->Release();
    }
}

static void ChangeVolume(float delta) {
    if (auto* v = Endpoint()) {
        float level = 0;
        v->GetMasterVolumeLevelScalar(&level);
        v->SetMasterVolumeLevelScalar(std::clamp(level + delta, 0.f, 1.f), nullptr);
        v->Release();
    }
    ReadVolume();
}

// ============================================================================================================
// geometry
// ============================================================================================================

static RECT g_screen;   // primary monitor, pixels

static double PanelH() { return g_set.panelH; }
static double LauncherW() { return g_set.iconSize + 16; }
static double ItemH() { return g_set.iconSize + 10; }

static bool LightTheme() {   // the light panel: chosen, or Windows in light mode (the system part: taskbar, Start)
    if (g_set.panelStyle == L"dark" || g_set.panelStyle == L"light") return g_set.panelStyle == L"light";
    DWORD v = 0, size = sizeof(v);
    return RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme",
                        RRF_RT_REG_DWORD, nullptr, &v, &size) == ERROR_SUCCESS && v == 1;
}

static void Measure() {
    // the main screen's scale as it is now (the system DPI stays what it was at sign-in)
    UINT dx = 96, dy = 96;
    if (FAILED(GetDpiForMonitor(MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY), MDT_EFFECTIVE_DPI, &dx, &dy))) dx = GetDpiForSystem();
    g_scale = dx / 96.0;
    g_screen = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
}

// ---------------- maximized windows: their title bar merges into the panel (Unity) ----------------
// A maximized window with a standard Windows title bar loses its caption; the panel shows its title and buttons.
// The caption comes back when the window is restored, and for every window when the mod stops.

static std::map<HWND, LONG> g_stripped;     // window -> its style before
static std::map<HWND, bool> g_ownCaption;   // windows that draw their own title bar: left alone

static std::wstring ClassOf(HWND h) {
    wchar_t cls[64] = {};
    GetClassNameW(h, cls, 64);
    return cls;
}

static void DecorRestore(HWND h) {
    auto it = g_stripped.find(h);
    if (it == g_stripped.end()) return;
    LONG style = it->second;
    g_stripped.erase(it);
    if (!IsWindow(h) || IsHungAppWindow(h)) return;   // a hung app would hold the shell (and its shutdown) up
    SetWindowLongW(h, GWL_STYLE, GetWindowLongW(h, GWL_STYLE) | (style & WS_CAPTION));
    SetWindowPos(h, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

static void DecorRestoreAll() {
    std::vector<HWND> all;
    for (auto& kv : g_stripped) all.push_back(kv.first);
    for (HWND h : all) DecorRestore(h);
}

static void DecorStrip(HWND h) {
    LONG style = GetWindowLongW(h, GWL_STYLE);
    if ((style & WS_CAPTION) != WS_CAPTION) return;
    if (ClassOf(h) == L"CabinetWClass") {   // File Explorer: its tab row stays, its caption buttons go
        // with tabs into the panel, its whole top bar goes (see the tab code); otherwise only its caption buttons
        if (!g_set.mergeExplorer || g_set.liftTitleBars) { g_ownCaption[h] = true; return; }
    } else {
        // a standard title bar puts the client area a caption below the work area's top; apps with their own
        // title bar (Chrome, Electron, Discord...) start it at the top: leave those alone
        RECT wr;
        POINT p = {0, 0};
        if (!GetWindowRect(h, &wr) || !ClientToScreen(h, &p)) return;
        int frame = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
        if (p.y - wr.top < frame + GetSystemMetrics(SM_CYCAPTION) * 0.8) { g_ownCaption[h] = true; return; }
    }
    g_stripped[h] = style;
    SetWindowLongW(h, GWL_STYLE, style & ~WS_CAPTION);
    SetWindowPos(h, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// ---------------- windows with tabs (browsers, File Explorer, Terminal, Notepad): when maximized, their whole top
// bar goes away and the panel takes over what was in it. The panel draws the window's tabs itself, Unity-style, and
// works them (pick, close, new) through UI Automation, Windows' accessibility interface. The window is maximized by
// the shell itself: full size, its tab bar just above the panel's bottom edge, out of sight (a really maximized
// browser can't be moved there). Restoring, or the mod stopping, gives it its real maximize back ----------------

struct TabInfo {
    std::wstring name;
    bool selected = false;
    int iconPx = 0;                 // its site's icon, cut from the window's tab bar: iconPx² premultiplied BGRA
    std::vector<uint32_t> icon;
    uint32_t iconHash = 0;
};
static std::mutex g_tabLock;
static bool g_tabLockReady;
static HWND g_tabTarget, g_tabOf;      // whose tabs the reader reads (the shell sets it) / whose these are
static std::vector<TabInfo> g_tabs;
static bool g_tabStrip, g_tabNew;      // that window has a tab strip / a new-tab button
static int g_tabBarBottom;             // screen y of the strip's bottom edge
static std::vector<std::pair<int, int>> g_tabCmds;   // (1 pick, 2 close, 3 new tab; tab index)
static HANDLE g_tabWake, g_tabThread;
static volatile bool g_tabStop;
static const UINT WM_APP_TABS = WM_APP + 61;
static const UINT WM_APP_DISCORD = WM_APP + 62;   // news from the Discord thread
static std::map<HWND, WINDOWPLACEMENT> g_fake;   // windows the shell maximizes itself, and how they were before
static std::map<HWND, int> g_fakeTries;          // corrections so far (an app that keeps moving is left alone)
static const CLSID kClsidUIA = {0xff48dba4, 0x60ef, 0x4201, {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};
static const CLSID kClsidUIA8 = {0xe22ad333, 0xb25f, 0x460c, {0x83, 0xd0, 0x05, 0x81, 0x10, 0x73, 0x95, 0xc9}};

// UI Automation for a reader thread, with short timeouts: a hung app answers nothing, and the call gives up instead
// of holding the thread (and the shell's shutdown) up
static IUIAutomation* MakeUia() {
    IUIAutomation* uia = nullptr;
    if (FAILED(CoCreateInstance(kClsidUIA8, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&uia)) || !uia)
        CoCreateInstance(kClsidUIA, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&uia);
    IUIAutomation2* uia2 = nullptr;
    if (uia && SUCCEEDED(uia->QueryInterface(__uuidof(IUIAutomation2), (void**)&uia2)) && uia2) {
        uia2->put_ConnectionTimeout(1500);
        uia2->put_TransactionTimeout(2500);
        uia2->Release();
    }
    return uia;
}

// the tabs' site icons: the window's tab bar drawn into a bitmap (PrintWindow draws it even out of sight above the
// screen), each tab's icon found left in it and cut out, its background taken away ("colour to alpha")
static void CaptureTabIcons(HWND w, std::vector<TabInfo>& tabs, const std::vector<RECT>& rects) {
    RECT wr;
    if (IsHungAppWindow(w) || !GetWindowRect(w, &wr) || tabs.empty()) return;   // PrintWindow waits on the app
    int bottom = 0;
    for (auto& r : rects) bottom = std::max(bottom, (int)(r.bottom - wr.top));
    int W = wr.right - wr.left, H = std::min(bottom + 2, 400);
    if (W <= 0 || H <= 0) return;
    BITMAPINFO bi = {};
    bi.bmiHeader = {sizeof(BITMAPINFOHEADER), W, -H, 1, 32, BI_RGB};
    void* bits = nullptr;
    HDC screen = GetDC(nullptr), dc = CreateCompatibleDC(screen);
    HBITMAP bmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!bmp || !bits) {
        if (bmp) DeleteObject(bmp);
        DeleteDC(dc);
        return;
    }
    HGDIOBJ old = SelectObject(dc, bmp);
    bool ok = PrintWindow(w, dc, 2 /* PW_RENDERFULLCONTENT */);
    SelectObject(dc, old);
    DeleteDC(dc);
    const uint32_t* px = (const uint32_t*)bits;
    float dpi = GetDpiForWindow(w) / 96.f;
    int size = std::max(8, (int)std::lround(16 * dpi));
    auto at = [&](int x, int y) { return px[std::clamp(y, 0, H - 1) * W + std::clamp(x, 0, W - 1)]; };
    auto diff = [](uint32_t a, uint32_t b) {
        return abs((int)(a & 255) - (int)(b & 255)) + abs((int)(a >> 8 & 255) - (int)(b >> 8 & 255)) + abs((int)(a >> 16 & 255) - (int)(b >> 16 & 255));
    };
    for (size_t i = 0; ok && i < tabs.size() && i < rects.size(); i++) {
        RECT r = rects[i];
        int left = r.left - wr.left, cy = (r.top + r.bottom) / 2 - wr.top, top = cy - size / 2;
        if (r.right - r.left < size * 2 || top < 0 || top + size > H) continue;
        uint32_t bg = at(left + (int)(4 * dpi), cy);
        // the first column (from the left padding on) with something on it: the icon starts there
        int x0 = -1;
        for (int x = left + (int)(6 * dpi); x < left + (int)(40 * dpi) && x0 < 0; x++)
            for (int y = top; y < top + size && x0 < 0; y++)
                if (diff(at(x, y), bg) > 60) x0 = x;
        if (x0 < 0) continue;
        TabInfo& t = tabs[i];
        t.iconPx = size;
        t.icon.resize(size * size);
        uint32_t hash = 2166136261u;
        int B[3] = {(int)(bg & 255), (int)(bg >> 8 & 255), (int)(bg >> 16 & 255)};
        for (int y = 0; y < size; y++)
            for (int x = 0; x < size; x++) {
                uint32_t p = at(x0 + x, top + y);
                int P[3] = {(int)(p & 255), (int)(p >> 8 & 255), (int)(p >> 16 & 255)};
                // the least see-through colour that, over the background, gives this pixel
                float a = 0;
                for (int c = 0; c < 3; c++) {
                    float room = P[c] > B[c] ? 255.f - B[c] : (float)B[c];
                    if (room > 0) a = std::max(a, abs(P[c] - B[c]) / room);
                }
                a = std::min(1.f, a);
                uint32_t out = 0;
                if (a > 0.02f) {
                    // premultiplied: colour * alpha = pixel - background * (1 - alpha)
                    int o[3];
                    for (int c = 0; c < 3; c++) o[c] = std::clamp((int)std::lround(P[c] - B[c] * (1 - a)), 0, (int)(a * 255 + 0.5f));
                    out = (uint32_t)o[0] | (uint32_t)o[1] << 8 | (uint32_t)o[2] << 16 | (uint32_t)std::lround(a * 255) << 24;
                }
                t.icon[y * size + x] = out;
                hash = (hash ^ out) * 16777619u;
            }
        t.iconHash = hash;
    }
    DeleteObject(bmp);
}

// the reader: its own thread, as UI Automation asks the other app and waits for its answer
static DWORD WINAPI TabWorker(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    IUIAutomation* uia = MakeUia();
    IUIAutomationCondition *isStrip = nullptr, *isTab = nullptr, *isButton = nullptr;
    if (uia) {
        VARIANT v;
        v.vt = VT_I4;
        v.lVal = UIA_TabControlTypeId;
        uia->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &isStrip);
        v.lVal = UIA_TabItemControlTypeId;
        uia->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &isTab);
        v.lVal = UIA_ButtonControlTypeId;
        uia->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &isButton);
    }
    HWND cached = nullptr;
    IUIAutomationElement *strip = nullptr, *newTab = nullptr;
    std::wstring lastKey;
    while (!g_tabStop) {
        WaitForSingleObject(g_tabWake, 600);
        if (g_tabStop) break;
        g_tabLock.lock();
        HWND target = g_tabTarget;
        auto cmds = g_tabCmds;
        g_tabCmds.clear();
        g_tabLock.unlock();
        if (!uia || !isStrip || !target || !IsWindow(target)) continue;
        if (target != cached) {   // find the window's tab strip, and its new-tab button
            Release(strip);
            Release(newTab);
            cached = target;
            lastKey.clear();
            IUIAutomationElement* root = nullptr;
            if (SUCCEEDED(uia->ElementFromHandle(target, &root)) && root) {
                // the window's own tab row sits at its very top; a web page's tabs (GitHub's Write / Preview, ...)
                // lie further down, under the address bar, and are no tab row of the window's
                RECT wr = {};
                GetWindowRect(target, &wr);
                int reach = (int)(90 * GetDpiForWindow(target) / 96), best = INT_MAX;
                IUIAutomationElementArray* strips = nullptr;
                if (SUCCEEDED(root->FindAll(TreeScope_Descendants, isStrip, &strips)) && strips) {
                    int count = 0;
                    strips->get_Length(&count);
                    for (int i = 0; i < count; i++) {
                        IUIAutomationElement* e = nullptr;
                        RECT r = {};
                        if (SUCCEEDED(strips->GetElement(i, &e)) && e && SUCCEEDED(e->get_CurrentBoundingRectangle(&r)) &&
                            r.right > r.left && r.top - wr.top < reach && r.top < best) {
                            best = r.top;
                            Release(strip);
                            strip = e;
                            e = nullptr;
                        }
                        Release(e);
                    }
                }
                Release(strips);
                // the new-tab button sits near the strip: look around it, a level further out each time (up to three)
                IUIAutomationTreeWalker* walker = nullptr;
                IUIAutomationElement* around = nullptr;
                if (strip && SUCCEEDED(uia->get_ControlViewWalker(&walker)) && walker) walker->GetParentElement(strip, &around);
                for (int level = 0; level < 3 && around && !newTab; level++) {
                IUIAutomationElementArray* buttons = nullptr;
                if (SUCCEEDED(around->FindAll(TreeScope_Descendants, isButton, &buttons)) && buttons) {
                    int n = 0;
                    buttons->get_Length(&n);
                    for (int i = 0; i < n && !newTab; i++) {
                        IUIAutomationElement* b = nullptr;
                        BSTR name = nullptr;
                        if (SUCCEEDED(buttons->GetElement(i, &b)) && b && SUCCEEDED(b->get_CurrentName(&name)) && name) {
                            std::wstring nm = Lower(name);
                            if (nm.find(L"new tab") != std::wstring::npos || nm.find(L"nieuw tabblad") != std::wstring::npos ||
                                nm.find(L"add new") != std::wstring::npos || nm.find(L"toevoegen") != std::wstring::npos) {
                                newTab = b;
                                b = nullptr;
                            }
                        }
                        SysFreeString(name);
                        Release(b);
                    }
                }
                Release(buttons);
                IUIAutomationElement* outer = nullptr;
                if (!newTab) walker->GetParentElement(around, &outer);
                Release(around);
                around = outer;
                }
                Release(around);
                Release(walker);
                root->Release();
            }
        }
        IUIAutomationElementArray* items = nullptr;
        if (strip) strip->FindAll(TreeScope_Children, isTab, &items);
        int n = 0;
        if (items) items->get_Length(&n);
        // what the panel asked for
        for (auto& c : cmds) {
            if (c.first == 3) {
                IUIAutomationInvokePattern* inv = nullptr;
                if (newTab && SUCCEEDED(newTab->GetCurrentPatternAs(UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern), (void**)&inv)) && inv) inv->Invoke();
                Release(inv);
                continue;
            }
            IUIAutomationElement* tab = nullptr;
            if (!items || c.second < 0 || c.second >= n || FAILED(items->GetElement(c.second, &tab)) || !tab) continue;
            if (c.first == 1) {
                IUIAutomationSelectionItemPattern* sel = nullptr;
                if (SUCCEEDED(tab->GetCurrentPatternAs(UIA_SelectionItemPatternId, __uuidof(IUIAutomationSelectionItemPattern), (void**)&sel)) && sel) sel->Select();
                Release(sel);
            } else if (c.first == 2) {
                // only the active tab has its close button (Chromium): another one is picked first, then closed, and
                // the one that was active is picked again
                IUIAutomationElement* was = nullptr;
                for (int k = 0; k < n && !was; k++) {
                    IUIAutomationElement* t = nullptr;
                    IUIAutomationSelectionItemPattern* sel = nullptr;
                    BOOL on = FALSE;
                    if (k != c.second && SUCCEEDED(items->GetElement(k, &t)) && t &&
                        SUCCEEDED(t->GetCurrentPatternAs(UIA_SelectionItemPatternId, __uuidof(IUIAutomationSelectionItemPattern), (void**)&sel)) && sel &&
                        SUCCEEDED(sel->get_CurrentIsSelected(&on)) && on) { was = t; t = nullptr; }
                    Release(sel);
                    Release(t);
                }
                auto pressClose = [&]() {
                    IUIAutomationElement* close = nullptr;
                    IUIAutomationInvokePattern* inv = nullptr;
                    bool done = SUCCEEDED(tab->FindFirst(TreeScope_Descendants, isButton, &close)) && close &&
                                SUCCEEDED(close->GetCurrentPatternAs(UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern), (void**)&inv)) && inv &&
                                SUCCEEDED(inv->Invoke());
                    Release(inv);
                    Release(close);
                    return done;
                };
                if (!pressClose()) {
                    IUIAutomationSelectionItemPattern* sel = nullptr;
                    if (SUCCEEDED(tab->GetCurrentPatternAs(UIA_SelectionItemPatternId, __uuidof(IUIAutomationSelectionItemPattern), (void**)&sel)) && sel) sel->Select();
                    Release(sel);
                    for (int tries = 0; tries < 10 && !pressClose(); tries++) Sleep(40);   // its button shows up a moment later
                    if (was) {
                        Sleep(60);
                        IUIAutomationSelectionItemPattern* back = nullptr;
                        if (SUCCEEDED(was->GetCurrentPatternAs(UIA_SelectionItemPatternId, __uuidof(IUIAutomationSelectionItemPattern), (void**)&back)) && back) back->Select();
                        Release(back);
                    }
                }
                Release(was);
            }
            tab->Release();
        }
        if (!cmds.empty()) {   // read again after the change
            Release(items);
            n = 0;
            if (strip) strip->FindAll(TreeScope_Children, isTab, &items);
            if (items) items->get_Length(&n);
        }
        // the tabs as they are now
        std::vector<TabInfo> tabs;
        std::vector<RECT> rects;
        std::wstring key;
        for (int i = 0; i < n; i++) {
            IUIAutomationElement* tab = nullptr;
            if (FAILED(items->GetElement(i, &tab)) || !tab) continue;
            TabInfo t;
            RECT rc = {};
            tab->get_CurrentBoundingRectangle(&rc);
            rects.push_back(rc);
            BSTR name = nullptr;
            if (SUCCEEDED(tab->get_CurrentName(&name)) && name) t.name = name;
            SysFreeString(name);
            size_t extra = t.name.find(L" - Memory usage");   // Chromium adds its memory use to the name
            if (extra == std::wstring::npos) extra = t.name.find(L" - Geheugengebruik");
            if (extra != std::wstring::npos) t.name.resize(extra);
            IUIAutomationSelectionItemPattern* sel = nullptr;
            BOOL on = FALSE;
            if (SUCCEEDED(tab->GetCurrentPatternAs(UIA_SelectionItemPatternId, __uuidof(IUIAutomationSelectionItemPattern), (void**)&sel)) && sel) sel->get_CurrentIsSelected(&on);
            Release(sel);
            t.selected = on != FALSE;
            key += t.name + (t.selected ? L"*|" : L"|");
            tabs.push_back(t);
            tab->Release();
        }
        Release(items);
        if (g_set.tabIcons) CaptureTabIcons(target, tabs, rects);
        for (auto& t : tabs) key += std::to_wstring(t.iconHash) + L",";   // an icon that came in (or changed)
        RECT box = {};
        if (strip) strip->get_CurrentBoundingRectangle(&box);
        key += std::to_wstring(box.bottom) + (strip ? L"s" : L"") + (newTab ? L"n" : L"");
        if (key == lastKey) continue;
        if (lastKey.empty()) {   // first look at this window: what was found (for windows whose tabs don't show up)
            wchar_t cls[64] = {};
            GetClassNameW(target, cls, 64);
            Wh_Log(L"tab reader: %s: strip %d, %d tabs, new-tab button %d, bar bottom %d", cls, strip != nullptr, (int)tabs.size(), newTab != nullptr, (int)box.bottom);
        }
        lastKey = key;
        g_tabLock.lock();
        g_tabOf = target;
        g_tabs = tabs;
        g_tabStrip = strip != nullptr;
        g_tabNew = newTab != nullptr;
        g_tabBarBottom = box.bottom;
        g_tabLock.unlock();
        if (g_host) PostMessageW(g_host, WM_APP_TABS, 0, 0);
    }
    Release(strip);
    Release(newTab);
    Release(isStrip);
    Release(isTab);
    Release(isButton);
    Release(uia);
    CoUninitialize();
    return 0;
}

static void TabCommand(int kind, int index) {
    g_tabLock.lock();
    g_tabCmds.push_back({kind, index});
    g_tabLock.unlock();
    SetEvent(g_tabWake);
}

static void StartTabReader() {
    if (!g_tabLockReady) {
        g_tabLockReady = true;
    }
    g_tabStop = false;
    g_tabWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_tabThread = CreateThread(nullptr, 0, TabWorker, nullptr, 0, nullptr);
}

static void StopTabReader() {
    g_tabStop = true;
    if (g_tabWake) SetEvent(g_tabWake);
    if (g_tabThread) {
        WaitForSingleObject(g_tabThread, INFINITE);   // every call it makes is bounded: it always ends
        CloseHandle(g_tabThread);
    }
    if (g_tabWake) CloseHandle(g_tabWake);
    g_tabThread = g_tabWake = nullptr;
    g_tabTarget = g_tabOf = nullptr;
}

// a top bar of its own: its client area starts right at the window's top (no Windows caption above it)
static bool OwnTopBar(HWND h) {
    if (g_stripped.count(h)) return false;
    wchar_t cls[32] = {};
    GetClassNameW(h, cls, 32);
    if (!wcscmp(cls, L"CabinetWClass")) return true;   // File Explorer: Windows' caption, with its tab row drawn in it
    RECT r;
    POINT p = {0, 0};
    if (!GetWindowRect(h, &r) || !ClientToScreen(h, &p)) return false;
    int frame = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
    return p.y - r.top < frame + GetSystemMetrics(SM_CYCAPTION) * 0.8;
}

static bool IsShellWindow(HWND h) {
    wchar_t cls[64] = {};
    GetClassNameW(h, cls, 64);
    return !wcsncmp(cls, L"UnityShell", 10);
}

static RECT WorkArea() {
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint({g_screen.left, g_screen.top}, MONITOR_DEFAULTTOPRIMARY), &mi);
    return mi.rcWork;
}

// on the screen with the panel (windows on other screens keep their own title bars and stay where they are)
static bool OnPanelScreen(HWND h) {
    return MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) == MonitorFromPoint({g_screen.left, g_screen.top}, MONITOR_DEFAULTTOPRIMARY);
}

// covering its whole screen (a game, or a video / browser gone full screen with F11)
static bool CoversMonitor(HWND h) {
    RECT r;
    if (!GetWindowRect(h, &r)) return false;
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST), &mi);
    return r.left <= mi.rcMonitor.left && r.top <= mi.rcMonitor.top && r.right >= mi.rcMonitor.right && r.bottom >= mi.rcMonitor.bottom;
}

// full size with its tab bar hidden: hidden = pixels from the window's top down to the bar's bottom edge
static std::map<HWND, RECT> g_fakeRect;     // where the shell put each one (anything else moving it: put back)
static std::map<HWND, double> g_fakeAt;     // when (the reader's next measurement comes a moment later)
static std::map<HWND, RECT> g_fakeWork;     // the work area then (a different one: the screen changed, not the user)
static void StartGhost(RECT from, RECT to, double ms, float a0, float a1);
static void RenderPanel();

// Windows' own min/max/restore animation, off while the shell moves the window itself (a ghost is shown instead)
static void Transitions(HWND w, bool on) {
    BOOL off = on ? FALSE : TRUE;
    DwmSetWindowAttribute(w, DWMWA_TRANSITIONS_FORCEDISABLED, &off, sizeof(off));
}

static RECT ScreenRect(const RECT& workspace) {   // a placement's rectangle (work area coordinates) on the screen
    RECT work = WorkArea();
    return {workspace.left + work.left, workspace.top + work.top, workspace.right + work.left, workspace.bottom + work.top};
}

// forget a window the shell maximized, leaving it where it is
static void FakeForget(HWND w) {
    g_fake.erase(w);
    g_fakeTries.erase(w);
    g_fakeRect.erase(w);
    g_fakeAt.erase(w);
    g_fakeWork.erase(w);
    if (IsWindow(w)) Transitions(w, true);
}

static void PlaceFake(HWND w, int hidden) {
    // never more than a tab row's height out of sight: anything else is a misreading (it would push the window up
    // off the screen, onto a screen above this one)
    hidden = std::clamp(hidden, 0, (int)(110 * GetDpiForWindow(w) / 96));
    // out of the real maximize first; where it lands doesn't matter (an old size, or a snap layout Windows picks):
    // it is placed explicitly right after
    Transitions(w, false);
    if (IsZoomed(w)) ShowWindow(w, SW_RESTORE);
    RECT work = WorkArea(), wr, vis;
    int dl = Px(7), dr = Px(7), db = Px(7);   // the invisible resize borders, until they can be measured
    if (GetWindowRect(w, &wr) && SUCCEEDED(DwmGetWindowAttribute(w, DWMWA_EXTENDED_FRAME_BOUNDS, &vis, sizeof(vis)))) {
        dl = vis.left - wr.left;
        dr = wr.right - vis.right;
        db = wr.bottom - vis.bottom;
    }
    RECT to = {work.left - dl, work.top - hidden, work.right + dr, work.bottom + db};
    SetWindowPos(w, nullptr, to.left, to.top, to.right - to.left, to.bottom - to.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    GetWindowRect(w, &g_fakeRect[w]);
    g_fakeAt[w] = (double)GetTickCount64();
    g_fakeWork[w] = work;
}

static void FakeMaximize(HWND w) {
    WINDOWPLACEMENT before = {sizeof(before)};
    GetWindowPlacement(w, &before);
    g_fake[w] = before;
    g_fakeTries[w] = 0;
    PlaceFake(w, Px(44));   // a first guess; the tab reader measures the bar and it is corrected
    RECT work = WorkArea();   // growing out of where it was into the whole screen
    StartGhost(ScreenRect(before.rcNormalPosition), work, 230, 0.45f, 0);
    Wh_Log(L"tabs into the panel: %s", WindowTitle(w).c_str());
}

static void FakeRestore(HWND w, bool maximize) {   // its own restore (or the real maximize back)
    auto it = g_fake.find(w);
    if (it == g_fake.end()) return;
    WINDOWPLACEMENT wp = it->second;
    RECT full = {};
    if (IsWindow(w)) GetWindowRect(w, &full);
    FakeForget(w);
    if (!IsWindow(w) || IsHungAppWindow(w)) return;
    if (maximize) {   // its own size and place first (what it remembers), then the real maximize; both queued, never waited on
        RECT normal = ScreenRect(wp.rcNormalPosition);
        SetWindowPos(w, nullptr, normal.left, normal.top, normal.right - normal.left, normal.bottom - normal.top,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS);
        ShowWindowAsync(w, SW_MAXIMIZE);
        return;
    }
    wp.length = sizeof(wp);
    wp.flags = 0;
    wp.showCmd = SW_SHOWNORMAL;
    Transitions(w, false);
    SetWindowPlacement(w, &wp);
    StartGhost(WorkArea(), ScreenRect(wp.rcNormalPosition), 230, 0.45f, 0);   // shrinking back to its own size
    Transitions(w, true);
}

static void FakeRestoreAll() {
    std::vector<HWND> all;
    for (auto& kv : g_fake) all.push_back(kv.first);
    for (HWND w : all) FakeRestore(w, true);
}

// after each refresh and each word from the tab reader: who has its tabs read, who gets maximized by the shell
static void LiftUpdate() {
    for (auto it = g_fake.begin(); it != g_fake.end();)
        if (!IsWindow(it->first)) it = g_fake.erase(it);
        else ++it;
    if (!g_set.liftTitleBars) {
        FakeRestoreAll();
        return;
    }
    HWND fg = GetForegroundWindow();
    if (!fg || IsShellWindow(fg) || !g_tabLockReady) return;   // the shell's own menus and popups change nothing
    bool app = false;
    for (auto& it : g_items)
        for (HWND w : it.windows)
            if (w == fg) app = true;
    // a window that was closed while the shell maximized it comes back that way (its bar above the screen): ours again,
    // with a sensible size to restore to
    RECT fr, work = WorkArea();
    if (app && !IsIconic(fg) && !IsZoomed(fg) && !g_fake.count(fg) && OnPanelScreen(fg) && GetWindowRect(fg, &fr) && fr.top < work.top - Px(16) &&
        fr.right - fr.left >= work.right - work.left && OwnTopBar(fg)) {
        WINDOWPLACEMENT wp = {sizeof(wp)};
        GetWindowPlacement(fg, &wp);
        int w = (work.right - work.left) * 7 / 10, h = (work.bottom - work.top) * 7 / 10;
        wp.rcNormalPosition = {(work.right - work.left - w) / 2, (work.bottom - work.top - h) / 2, (work.right - work.left + w) / 2, (work.bottom - work.top + h) / 2};
        g_fake[fg] = wp;
        g_fakeTries[fg] = 0;
    }
    HWND want = app && !IsIconic(fg) && (g_fake.count(fg) || (IsZoomed(fg) && OwnTopBar(fg) && OnPanelScreen(fg))) ? fg : nullptr;
    g_tabLock.lock();
    bool changed = want != g_tabTarget;
    g_tabTarget = want;
    bool strip = want && g_tabOf == want && g_tabStrip;
    int barBottom = g_tabBarBottom;
    g_tabLock.unlock();
    if (changed) SetEvent(g_tabWake);
    if (!want) return;
    if (!IsZoomed(want) && CoversMonitor(want)) return;   // gone full screen (a video): hands off until it comes back
    if (g_fake.count(want) && !OnPanelScreen(want)) {   // sent to another screen (Win+Shift+arrow): it lets go
        FakeForget(want);
        RenderPanel();
        return;
    }
    if (!g_fake.count(want)) {
        if (strip && IsZoomed(want)) FakeMaximize(want);
        return;
    }
    if (IsZoomed(want)) {   // maximized again the normal way (Win+Up, ...): ours again
        PlaceFake(want, Px(44));
        return;
    }
    // keep it where the shell put it (a resolution change or a snap moves it), with the bar's bottom edge exactly on
    // the work area's top (the reader measures it, a moment after each move)
    RECT wr;
    if (!GetWindowRect(want, &wr)) return;
    bool fresh = (double)GetTickCount64() - g_fakeAt[want] > 1000;
    int hidden = strip && fresh && barBottom > wr.top ? barBottom - wr.top : g_fakeRect.count(want) ? work.top - g_fakeRect[want].top : Px(44);
    bool moved = !g_fakeRect.count(want) || !EqualRect(&wr, &g_fakeRect[want]);
    if (moved && fresh && g_fakeWork.count(want) && EqualRect(&g_fakeWork[want], &work) && wr.top > work.top - Px(8)) {
        FakeForget(want);   // dragged away (or snapped): it lets go, keeping its new place
        RenderPanel();
        return;
    }
    bool barOff = strip && fresh && abs(barBottom - work.top) > 1;
    if ((moved || barOff) && g_fakeTries[want]++ < 8) PlaceFake(want, hidden);
    else if (!moved && !barOff) g_fakeTries[want] = 0;
}

static void DecorUpdate() {
    bool on = g_set.mergeMaximized;
    std::vector<HWND> back;
    for (auto& kv : g_stripped) {
        HWND h = kv.first;
        if (!on || !IsWindow(h) || (!IsZoomed(h) && !IsIconic(h)) || (!IsIconic(h) && !OnPanelScreen(h)) || ((!g_set.mergeExplorer || g_set.liftTitleBars) && ClassOf(h) == L"CabinetWClass")) back.push_back(h);
    }
    for (HWND h : back) DecorRestore(h);
    if (!on) return;
    for (auto& it : g_items)
        for (HWND w : it.windows)
            if (!IsIconic(w) && IsZoomed(w) && OnPanelScreen(w) && !g_stripped.count(w) && !g_ownCaption.count(w)) DecorStrip(w);
    if (g_ownCaption.size() > 300) g_ownCaption.clear();
}

// the focused app window, when it is maximized: the panel shows its title and buttons
static HWND MaximizedForeground() {
    HWND fg = g_foreground;
    if (!fg || !IsWindow(fg) || (!IsZoomed(fg) && !g_fake.count(fg)) || IsIconic(fg)) return nullptr;   // the shell's own maximize counts
    if (!g_fake.count(fg) && !OnPanelScreen(fg)) return nullptr;   // maximized on another screen: not the panel's
    for (auto& it : g_items)
        for (HWND w : it.windows)
            if (w == fg) return fg;
    return nullptr;
}

// ============================================================================================================
// animation: one clock for the whole shell (a timer on the host window while anything moves), with the same
// easing curves the UnityDash app used
// ============================================================================================================

static const float kOpenMs = 220, kCloseMs = 150;   // one timing for every piece of the Dash

static double NowMs() {
    static LARGE_INTEGER freq;
    if (!freq.QuadPart) QueryPerformanceFrequency(&freq);
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return c.QuadPart * 1000.0 / freq.QuadPart;
}

enum Ease { EASE_LINEAR, EASE_OUT_CUBIC, EASE_IN_CUBIC, EASE_OUT_QUART, EASE_OUT_BACK, EASE_SPRING };

static float Eased(int ease, float t) {
    switch (ease) {
        case EASE_OUT_CUBIC: return 1 - powf(1 - t, 3);
        case EASE_IN_CUBIC: return t * t * t;
        case EASE_OUT_QUART: return 1 - powf(1 - t, 4);
        case EASE_OUT_BACK: {   // overshoots a little, then settles
            const float c1 = 1.4f, c3 = c1 + 1;
            return 1 + c3 * powf(t - 1, 3) + c1 * powf(t - 1, 2);
        }
        case EASE_SPRING: return 1 - (1 - t) * expf(-4 * t) * cosf(3 * 3.14159265f * t);   // one bounce, like Unity's pop
        default: return t;
    }
}

struct Anim {
    float value = 0, from = 0, to = 0, ms = 1;
    double start = 0;
    int ease = EASE_LINEAR;
    bool running = false;

    void Set(float v) { value = from = to = v; running = false; }

    void Go(float target, float duration, int how, float startValue) {
        from = startValue;
        value = startValue;
        to = target;
        ms = std::max(1.f, duration * g_set.animScale);   // the animation speed setting (0: done at once)
        ease = how;
        start = NowMs();
        running = true;
    }

    void Go(float target, float duration, int how) { Go(target, duration, how, value); }

    bool Step(double now) {   // true while it moved this frame
        if (!running) return false;
        float t = (float)std::min(1.0, (now - start) / ms);
        value = from + (to - from) * Eased(ease, t);
        if (t >= 1) { value = to; running = false; }
        return true;
    }
};

static Anim g_aDash, g_aZoom, g_aUnder, g_aDim, g_aBfb;   // Dash fade, Dash zoom, launcher/panel taking the Dash colour, icon dimming, Dash button press
static bool g_dashClosing;
// the Dash's content coming in (the app's): rows fade in and rise 10 px, 22 ms apart; the filter sidebar slides in
static double g_rowsStart = -1e9;
static const double kRowsMs = 13 * 22 + 230;
static Anim g_aFilter;

static void KickAnim();

static void StartRowsIn() {
    if (g_set.animScale <= 0) return;   // animations off: the rows are simply there
    g_rowsStart = NowMs();
    KickAnim();
}

// per launcher item (by what it launches)
struct ItemFx {
    Anim hover, pop, wiggle;
    Anim shift;                // slots it is moved up/down while another icon is dragged past
    double pulseStart = 0;     // launching: pulse until a window shows up
    double lastWiggle = -1e9;
    int lastWindows = -1;
};
static std::map<std::wstring, ItemFx> g_fx;

static ItemFx& Fx(const std::wstring& launch) {
    auto& fx = g_fx[launch];
    if (fx.pop.to == 0 && !fx.pop.running) fx.pop.Set(1);
    return fx;
}

static void KickAnim() {
    if (g_host) SetTimer(g_host, TIMER_ANIM, 10, nullptr);
}

// the wiggle Unity gives an app that wants attention: angle at a point (0..1) of two shakes
static float WiggleAngle(float p) {
    static const float angles[] = {0, -14, 14, -10, 10, -5, 5, 0};
    float cycle = fmodf(p * 2, 1);
    float pos = cycle * 7;
    int i = std::min(6, (int)pos);
    return angles[i] + (angles[i + 1] - angles[i]) * (pos - i);
}


// ============================================================================================================
// app bars: the panel and the launcher reserve their edge of the screen, like the taskbar does
// ============================================================================================================

static void AppBar(HWND hwnd, UINT edge, RECT r, bool add) {
    APPBARDATA abd = {sizeof(abd)};
    abd.hWnd = hwnd;
    if (!add) {
        SHAppBarMessage(ABM_REMOVE, &abd);
        return;
    }
    abd.uCallbackMessage = 0;   // no notifications wanted: the shell lays itself out on display changes
    SHAppBarMessage(ABM_NEW, &abd);
    abd.uEdge = edge;
    abd.rc = r;
    SHAppBarMessage(ABM_QUERYPOS, &abd);
    abd.rc = r;   // keep exactly our rectangle
    SHAppBarMessage(ABM_SETPOS, &abd);
}

// ============================================================================================================
// the Windows taskbar: hidden (with auto-hide so it gives its space back) and restored as it was
// ============================================================================================================

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Original;
using SetWindowPos_t = decltype(&SetWindowPos);
static SetWindowPos_t SetWindowPos_Original;

static bool IsTaskbar(HWND hwnd) {
    wchar_t cls[64];
    if (!hwnd || !GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) return false;
    return wcscmp(cls, L"Shell_TrayWnd") == 0 || wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

static bool KeepHidden(HWND hwnd) {
    return g_running && g_set.hideTaskbar && IsTaskbar(hwnd);
}

// Explorer shows its taskbar again now and then (Start, auto-hide, a restart of the tray); stop that at the source
BOOL WINAPI ShowWindow_Hook(HWND hwnd, int cmd) {
    if (cmd != SW_HIDE && KeepHidden(hwnd)) return FALSE;
    return ShowWindow_Original(hwnd, cmd);
}

BOOL WINAPI SetWindowPos_Hook(HWND hwnd, HWND after, int x, int y, int cx, int cy, UINT flags) {
    if ((flags & SWP_SHOWWINDOW) && KeepHidden(hwnd)) flags &= ~SWP_SHOWWINDOW;
    return SetWindowPos_Original(hwnd, after, x, y, cx, cy, flags);
}

static std::vector<HWND> Taskbars() {
    std::vector<HWND> list;
    if (HWND main = FindWindowW(L"Shell_TrayWnd", nullptr)) list.push_back(main);
    HWND s = nullptr;
    while ((s = FindWindowExW(nullptr, s, L"Shell_SecondaryTrayWnd", nullptr))) list.push_back(s);
    return list;
}

static bool TaskbarAutoHide() {
    APPBARDATA abd = {sizeof(abd)};
    abd.hWnd = g_tray;
    return (SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE) != 0;
}

static void SetTaskbarAutoHide(bool on) {
    APPBARDATA abd = {sizeof(abd)};
    abd.hWnd = g_tray;
    abd.lParam = on ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
    SHAppBarMessage(ABM_SETSTATE, &abd);
}

static void HideTaskbar() {
    if (!g_set.hideTaskbar) return;
    // remember the user's own auto-hide setting across Explorer restarts (stored, not just in memory)
    g_taskbarWasAutoHide = Wh_GetIntValue(L"taskbarWasAutoHide", -1);
    if (g_taskbarWasAutoHide < 0) {
        g_taskbarWasAutoHide = TaskbarAutoHide() ? 1 : 0;
        Wh_SetIntValue(L"taskbarWasAutoHide", g_taskbarWasAutoHide);
    }
    if (!TaskbarAutoHide()) SetTaskbarAutoHide(true);
    for (HWND bar : Taskbars()) ShowWindow_Original(bar, SW_HIDE);
}

static void RestoreTaskbar() {
    for (HWND bar : Taskbars()) ShowWindow_Original(bar, SW_SHOWNA);
    if (g_taskbarWasAutoHide >= 0) {
        if (TaskbarAutoHide() != (g_taskbarWasAutoHide == 1)) SetTaskbarAutoHide(g_taskbarWasAutoHide == 1);
        Wh_DeleteValue(L"taskbarWasAutoHide");
    }
}

// ============================================================================================================
// top panel
// ============================================================================================================

// a menu line (see ShowMenu): an item, a bold heading or a separator
enum { MENU_ITEM, MENU_HEADING, MENU_SEPARATOR };
enum { MENU_RIGHT, MENU_BELOW, MENU_POINT };   // where it opens: right of / under its anchor, or at the pointer
struct MenuEntry {
    int kind = MENU_ITEM, id = 0;
    std::wstring text;
    const wchar_t* glyph = nullptr;
    bool enabled = true;
};
static MenuEntry MItem(int id, const std::wstring& text, const wchar_t* glyph = nullptr, bool enabled = true) {
    MenuEntry e;
    e.id = id;
    e.text = text;
    e.glyph = glyph;
    e.enabled = enabled;
    return e;
}
static MenuEntry MHeading(const std::wstring& text) {
    MenuEntry e;
    e.kind = MENU_HEADING;
    e.text = text;
    return e;
}
static MenuEntry MSeparator() {
    MenuEntry e;
    e.kind = MENU_SEPARATOR;
    return e;
}
static int ShowMenu(const std::vector<MenuEntry>& items, RECT anchor, int place);
enum { POP_VOLUME = 1, POP_CALENDAR, POP_TRAY };   // the panel's popups (see ShowPopup)
static void ShowPopup(int kind, RECT anchor);
static float TextWidth(const std::wstring& text, float size, DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL);
static std::wstring KeyboardLanguage();
static bool Confirm(const wchar_t* question) {
    return MessageBoxW(nullptr, question, L"Unity", MB_OKCANCEL | MB_ICONQUESTION | MB_TOPMOST) == IDOK;
}
// "Unity settings": the mod's settings live in Windhawk
static void OpenShellSettings() {
    // Windhawk's own engine is loaded in this process: windhawk.exe is a few folders up from it (any install place)
    wchar_t path[MAX_PATH] = {};
    HMODULE engine = GetModuleHandleW(L"windhawk.dll");
    if (!engine || !GetModuleFileNameW(engine, path, MAX_PATH)) return;
    std::wstring dir = path;
    for (int up = 0; up < 5; up++) {
        size_t slash = dir.find_last_of(L'\\');
        if (slash == std::wstring::npos) return;
        dir.resize(slash);
        std::wstring exe = dir + L"\\windhawk.exe";
        if (GetFileAttributesW(exe.c_str()) != INVALID_FILE_ATTRIBUTES) { Launch(exe); return; }
    }
}

struct Hot { D2D1_RECT_F r; int id; };
static std::vector<Hot> g_panelHot;
static int g_panelHover = -1;
enum { HOT_NETWORK = 1, HOT_VOLUME, HOT_CLOCK, HOT_SESSION, HOT_WINCLOSE, HOT_WINMIN, HOT_WINMAX, HOT_MESSAGES, HOT_LANGUAGE, HOT_TRAY };
static void ShowDash(bool show);
static void SetDashMax(bool on);

static bool Inside(const D2D1_RECT_F& r, float x, float y) { return x >= r.left && x < r.right && y >= r.top && y < r.bottom; }

// the window's tabs, drawn by the panel (the window's own tab bar is out of sight): Unity-style pills, the picked one
// lit with an accent line, a × on it and under the pointer, then a + for a new tab
enum { HOT_TAB = 1000, HOT_TABCLOSE = 2000, HOT_NEWTAB = 3000 };
static Anim g_aBtns;          // a maximized window's buttons on the panel's right: 0 out of sight .. 1 in
static bool g_btnsWanted;
static const float kBtnW = 40;   // one Windows-style button

// a tab's site icon as a bitmap of the panel's (kept by its picture: the same icon is made once)
static ID2D1Bitmap* TabIconBitmap(Surface& s, const TabInfo& t) {
    if (t.iconPx <= 0 || (int)t.icon.size() != t.iconPx * t.iconPx) return nullptr;
    std::wstring key = L"fav#" + std::to_wstring(t.iconHash);
    auto it = s.bitmaps.find(key);
    if (it != s.bitmaps.end()) return it->second;
    int favs = 0;
    for (auto& kv : s.bitmaps) favs += kv.first.rfind(L"fav#", 0) == 0;
    if (favs > 200) {   // old pictures (sites long closed) go
        for (auto b = s.bitmaps.begin(); b != s.bitmaps.end();) {
            if (b->first.rfind(L"fav#", 0) == 0) { Release(b->second); b = s.bitmaps.erase(b); }
            else ++b;
        }
    }
    ID2D1Bitmap* bmp = nullptr;
    s.rt->CreateBitmap(D2D1::SizeU(t.iconPx, t.iconPx), t.icon.data(), t.iconPx * 4,
                       D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &bmp);
    s.bitmaps[key] = bmp;
    return bmp;
}

static void DrawPanelTabs(Surface& s, HWND w, float left, float right, float H, Rgb ink, float alpha) {
    g_tabLock.lock();
    std::vector<TabInfo> tabs;
    if (g_tabOf == w) tabs = g_tabs;
    bool canNew = g_tabOf == w && g_tabNew;
    g_tabLock.unlock();
    if (tabs.empty()) {   // not read yet: the title meanwhile
        Text(s, WindowTitle(w), D2D1::RectF(left, 0, right, H), 13, alpha, DWRITE_TEXT_ALIGNMENT_LEADING, false, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, ink);
        return;
    }
    float plus = canNew ? 28 : 0, avail = right - left - plus;
    if (avail < 40) return;
    float tw = std::clamp(avail / (float)tabs.size(), 44.f, 200.f), x = left;
    for (int i = 0; i < (int)tabs.size() && x + 30 <= left + avail; i++) {
        float w2 = std::min(tw, left + avail - x);
        D2D1_RECT_F r = D2D1::RectF(x, 3, x + w2 - 4, H - 3);
        bool hot = g_panelHover == HOT_TAB + i || g_panelHover == HOT_TABCLOSE + i, picked = tabs[i].selected;
        s.rt->FillRoundedRectangle(D2D1::RoundedRect(r, 4, 4), s.Brush(ink, (picked ? 0.22f : hot ? 0.14f : 0.07f) * alpha));
        if (picked) s.rt->FillRectangle(D2D1::RectF(r.left + 6, r.bottom - 2, r.right - 6, r.bottom), s.Brush(Shade(g_accent, 1.4f), alpha));
        bool withClose = (picked || hot) && w2 > 64;
        D2D1_RECT_F cr = D2D1::RectF(r.right - 18, r.top + 2, r.right - 3, r.bottom - 2);
        g_panelHot.push_back({D2D1::RectF(r.left, 0, withClose ? cr.left : r.right, H), HOT_TAB + i});
        float textLeft = r.left + 8;
        if (ID2D1Bitmap* fav = TabIconBitmap(s, tabs[i])) {   // the site's icon, on whole pixels
            D2D1_SIZE_U sz = fav->GetPixelSize();
            float iw = sz.width / (float)g_scale, ih = sz.height / (float)g_scale;
            float ix = std::round((r.left + 7) * (float)g_scale) / (float)g_scale, iy = std::round(((r.top + r.bottom) / 2 - ih / 2) * (float)g_scale) / (float)g_scale;
            s.rt->DrawBitmap(fav, D2D1::RectF(ix, iy, ix + iw, iy + ih), alpha * (picked ? 1 : 0.85f), D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            textLeft = ix + iw + 6;
        }
        Text(s, tabs[i].name, D2D1::RectF(textLeft, r.top, (withClose ? cr.left : r.right) - 3, r.bottom), 12, alpha * (picked ? 1 : 0.78f), DWRITE_TEXT_ALIGNMENT_LEADING,
             false, nullptr, DWRITE_FONT_WEIGHT_NORMAL, ink);
        if (withClose) {
            g_panelHot.push_back({cr, HOT_TABCLOSE + i});
            if (g_panelHover == HOT_TABCLOSE + i) s.rt->FillRoundedRectangle(D2D1::RoundedRect(cr, 3, 3), s.Brush(ink, 0.2f * alpha));
            Glyph(s, L"\uE8BB", cr, 7, alpha, ink);
        }
        x += w2;
    }
    if (canNew) {
        D2D1_RECT_F pr = D2D1::RectF(x + 2, 3, x + 24, H - 3);
        g_panelHot.push_back({pr, HOT_NEWTAB});
        if (g_panelHover == HOT_NEWTAB) s.rt->FillRoundedRectangle(D2D1::RoundedRect(pr, 4, 4), s.Brush(ink, 0.14f * alpha));
        Glyph(s, L"\uE710", pr, 10, alpha, ink);
    }
}

// the panel's clock, as the clock setting asks: "13:08", "Wed 13:08" or "Wed 29 Oct 13:08" (in the user's own format)
static std::wstring PanelClock() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t time[32] = {}, date[64] = {};
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr, time, 32);
    if (g_set.clock == L"dayTime") GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, L"ddd", date, 64, nullptr);
    else if (g_set.clock == L"dateTime") GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, L"ddd d MMM", date, 64, nullptr);
    return date[0] ? std::wstring(date) + L"  " + time : std::wstring(time);
}

static void RenderPanel() {
    if (!g_panel) return;
    Surface& s = g_panelS;
    int w = g_screen.right - g_screen.left, h = Px(PanelH());
    if (!s.Begin(w, h)) return;
    float W = (float)(w / g_scale), H = (float)PanelH();
    float a = g_set.panelOpacity / 100.f;
    // Unity's Ambiance panel (the app's): a warm dark-grey gradient, or the light Radiance one when Windows is in light
    // mode; while the Dash is open it takes the Dash's tint (cross-faded in step with the Dash)
    float u = g_aUnder.value;
    HWND maxed = MaximizedForeground();
    bool tabsHere = maxed && g_fake.count(maxed);   // the window's tab bar is out of sight: its tabs are the panel's
    bool light = LightTheme();
    Rgb top = light ? Rgb{0xF4 / 255.f, 0xF3 / 255.f, 0xF1 / 255.f} : Rgb{0x3C / 255.f, 0x3B / 255.f, 0x37 / 255.f};
    Rgb bottom = light ? Rgb{0xDC / 255.f, 0xD8 / 255.f, 0xD0 / 255.f} : Rgb{0x2C / 255.f, 0x2B / 255.f, 0x28 / 255.f};
    // the ink: dark on the light panel, light on the dark one and on the Dash's tint (shifting with the Dash)
    Rgb lightInk = {0xDF / 255.f, 0xDB / 255.f, 0xD2 / 255.f}, darkInk = {0x3C / 255.f, 0x3B / 255.f, 0x37 / 255.f};
    Rgb ink = light ? darkInk : lightInk;
    if (light) ink = {ink.r + (lightInk.r - ink.r) * u, ink.g + (lightInk.g - ink.g) * u, ink.b + (lightInk.b - ink.b) * u};
    if (u < 1) {
        ID2D1GradientStopCollection* stops = nullptr;
        D2D1_GRADIENT_STOP gs[2] = {{0, D2D1::ColorF(top.r, top.g, top.b, 1 - u)}, {1, D2D1::ColorF(bottom.r, bottom.g, bottom.b, 1 - u)}};
        s.rt->CreateGradientStopCollection(gs, 2, &stops);
        ID2D1LinearGradientBrush* grad = nullptr;
        if (stops) s.rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, H)), stops, &grad);
        if (grad) s.rt->FillRectangle(D2D1::RectF(0, 0, W, H), grad);
        Release(grad);
        Release(stops);
    }
    if (u > 0) s.rt->FillRectangle(D2D1::RectF(0, 0, W, H), s.Brush(Shade(g_tint, 0.62f), std::min(1.f, a + 20 / 255.f) * u));

    g_panelHot.clear();
    // the Dash's window buttons on the left (Ubuntu's: an orange close, grey minimize and maximize), fading in with it;
    // a maximized window's on the far right instead (Windows-style), sliding in and pushing the indicators left
    bool dashMode = g_dashOpen;
    bool btns = maxed && !dashMode;
    if (btns != g_btnsWanted) {
        g_btnsWanted = btns;
        g_aBtns.Go(btns ? 1.f : 0.f, btns ? 260.f : 180.f, btns ? EASE_OUT_CUBIC : EASE_IN_CUBIC);
        KickAnim();
    }
    float slide = std::clamp(g_aBtns.value, 0.f, 1.f), btnsW = 3 * kBtnW * slide;
    float show = u, titleLeft = 12;
    if (show > 0) {
        float d = roundf((float)H * 0.72f), bx = 6;
        const wchar_t* glyphs[3] = {L"\uE8BB", L"\uE921", dashMode && !g_dashMax ? L"\uE922" : L"\uE923"};
        int ids[3] = {HOT_WINCLOSE, HOT_WINMIN, HOT_WINMAX};
        for (int k = 0; k < 3; k++) {
            D2D1_RECT_F r = D2D1::RectF(bx, (H - d) / 2, bx + d, (H + d) / 2);
            bx += d + 4;
            g_panelHot.push_back({D2D1::RectF(r.left - 2, 0, r.right + 2, H), ids[k]});
            bool disabled = k == 1 && dashMode;   // the Dash can't be minimized
            float ba = show * (disabled ? 0.35f : g_panelHover == ids[k] ? 0.8f : 1.f);
            Rgb top = k == 0 ? Rgb{0xF4 / 255.f, 0x7A / 255.f, 0x4A / 255.f} : Rgb{0x8E / 255.f, 0x8C / 255.f, 0x84 / 255.f};
            Rgb bottom = k == 0 ? Rgb{0xDF / 255.f, 0x4A / 255.f, 0x16 / 255.f} : Rgb{0x6A / 255.f, 0x68 / 255.f, 0x62 / 255.f};
            ID2D1GradientStopCollection* stops = nullptr;
            D2D1_GRADIENT_STOP gs[2] = {{0, D2D1::ColorF(top.r, top.g, top.b, ba)}, {1, D2D1::ColorF(bottom.r, bottom.g, bottom.b, ba)}};
            s.rt->CreateGradientStopCollection(gs, 2, &stops);
            ID2D1LinearGradientBrush* grad = nullptr;
            if (stops) s.rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.left, r.bottom)), stops, &grad);
            auto circle = D2D1::Ellipse(D2D1::Point2F((r.left + r.right) / 2, (r.top + r.bottom) / 2), d / 2, d / 2);
            if (grad) s.rt->FillEllipse(circle, grad);
            Release(grad);
            Release(stops);
            s.rt->DrawEllipse(circle, s.Brush({0, 0, 0}, 0x90 / 255.f * ba), 0.6f);
            Text(s, glyphs[k], r, roundf(d * 0.45f), 0xE0 / 255.f * ba, DWRITE_TEXT_ALIGNMENT_CENTER, false, kGlyphs, DWRITE_FONT_WEIGHT_BOLD, {0x2B / 255.f, 0x29 / 255.f, 0x26 / 255.f});
        }
        titleLeft = bx + 6;
    }
    // the title: the app's name, or the window's own title when it is maximized; none for the Dash
    std::wstring title;
    if (maxed) title = WindowTitle(maxed);
    else
        for (auto& it : g_items)
            if (it.focused) { title = it.name; break; }
    if (!title.empty() && u < 1 && !tabsHere) Text(s, title, D2D1::RectF(titleLeft, 0, W * 0.45f, H), 13, 1 - u, DWRITE_TEXT_ALIGNMENT_LEADING, false, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, ink);

    std::wstring lang = KeyboardLanguage(), clock = PanelClock();
    float lw = TextWidth(lang, 11, DWRITE_FONT_WEIGHT_BOLD) + 6, cw = std::max(58.f, TextWidth(clock, 13) + 18);
    // the indicators' room (the ones the settings show)
    float indicators = 30 + cw + (g_set.showVolume ? 30 : 0) + (g_set.showMessages ? 30 : 0) + (g_set.showNetwork ? 30 : 0) +
                       (g_set.showLanguage ? lw + 18 : 0) + (g_set.showTray ? 26 : 0);
    if (tabsHere && u < 1) DrawPanelTabs(s, maxed, titleLeft, W - 6 - btnsW - indicators - 16, H, ink, 1 - u);

    // the maximized window's buttons: minimize, restore, close, like Windows' own (close turns red under the pointer)
    if (btnsW > 0.5f) {
        s.rt->PushAxisAlignedClip(D2D1::RectF(W - btnsW, 0, W, H), D2D1_ANTIALIAS_MODE_ALIASED);
        const wchar_t* glyphs[3] = {L"\uE921", L"\uE923", L"\uE8BB"};
        int ids[3] = {HOT_WINMIN, HOT_WINMAX, HOT_WINCLOSE};
        for (int k = 0; k < 3; k++) {
            float bx = W - btnsW + k * kBtnW * slide;
            D2D1_RECT_F r = D2D1::RectF(bx, 0, bx + kBtnW, H);
            if (slide > 0.99f) g_panelHot.push_back({r, ids[k]});
            bool hot = g_panelHover == ids[k];
            if (hot && k == 2) s.rt->FillRectangle(r, s.Brush({0xC4 / 255.f, 0x2B / 255.f, 0x1C / 255.f}, 1));
            else if (hot) s.rt->FillRectangle(r, s.Brush(ink, 0.14f));
            Glyph(s, glyphs[k], r, 10, slide, hot && k == 2 ? Rgb{1, 1, 1} : ink);
        }
        s.rt->PopAxisAlignedClip();
    }

    // indicators, right to left (Unity's order): session, clock, volume, messages, network, keyboard language
    float x = W - 6 - btnsW;
    auto slot = [&](float width, int id) {
        D2D1_RECT_F r = D2D1::RectF(x - width, 0, x, H);
        x -= width;
        g_panelHot.push_back({r, id});
        if (g_panelHover == id) s.rt->FillRectangle(r, s.White(0x26 / 255.f));
        return r;
    };
    Glyph(s, L"\uE7E8", slot(30, HOT_SESSION), 13, 1, ink);
    D2D1_RECT_F cr = slot(cw, HOT_CLOCK);
    Text(s, clock, cr, 13, 1, DWRITE_TEXT_ALIGNMENT_CENTER, false, nullptr, DWRITE_FONT_WEIGHT_NORMAL, ink);
    const wchar_t* vol = g_muted || g_volume < 0.01f ? L"\uE74F" : g_volume < 0.34f ? L"\uE993" : g_volume < 0.67f ? L"\uE994" : L"\uE995";
    if (g_set.showVolume) Glyph(s, vol, slot(30, HOT_VOLUME), 13, 1, ink);
    if (g_set.showMessages) Glyph(s, L"\uE715", slot(30, HOT_MESSAGES), 13, 1, ink);   // the messaging menu's envelope
    if (g_set.showNetwork) Glyph(s, L"\uE839", slot(30, HOT_NETWORK), 12, 1, ink);
    if (g_set.showLanguage) {   // the keyboard language in a little box in the panel's ink, like Unity
        D2D1_RECT_F lr = slot(lw + 18, HOT_LANGUAGE), lb = D2D1::RectF((lr.left + lr.right - lw) / 2, H / 2 - 7.5f, (lr.left + lr.right + lw) / 2, H / 2 + 7.5f);
        s.rt->FillRoundedRectangle(D2D1::RoundedRect(lb, 2, 2), s.Brush(ink, 1));
        Rgb paper = light && u < 0.5f ? Rgb{0xF2 / 255.f, 0xF1 / 255.f, 0xF0 / 255.f} : darkInk;
        Text(s, lang, lb, 11, 1, DWRITE_TEXT_ALIGNMENT_CENTER, false, nullptr, DWRITE_FONT_WEIGHT_BOLD, paper);
    }
    if (g_set.showTray) Glyph(s, L"\uE70D", slot(26, HOT_TRAY), 10, 1, ink);   // the apps running in the background (their tray icons)
    s.End(g_screen.left, g_screen.top - g_awayPanel);
}

static void SessionMenu(RECT at) {
    std::vector<MenuEntry> m = {MItem(1, L"About This Computer"), MItem(2, L"Unity Settings…"), MItem(3, L"System Settings…"), MSeparator(),
                                MItem(4, L"Lock"), MItem(5, L"Log Out…"), MSeparator(), MItem(6, L"Restart…"), MItem(7, L"Shut Down…")};
    switch (ShowMenu(m, at, MENU_BELOW)) {
        case 1: Launch(L"ms-settings:about"); break;
        case 2: OpenShellSettings(); break;
        case 3: Launch(L"ms-settings:"); break;
        case 4: LockWorkStation(); break;
        case 5: if (Confirm(L"Log out now? Unsaved work in open apps may be lost.")) ShellExecuteW(nullptr, nullptr, L"shutdown.exe", L"/l", nullptr, SW_HIDE); break;
        case 6: if (Confirm(L"Restart the computer now?")) ShellExecuteW(nullptr, nullptr, L"shutdown.exe", L"/r /t 0", nullptr, SW_HIDE); break;
        case 7: if (Confirm(L"Shut down the computer now?")) ShellExecuteW(nullptr, nullptr, L"shutdown.exe", L"/s /t 0", nullptr, SW_HIDE); break;
    }
}

// pulling a maximized window out of the panel (Unity): it shrinks back to its own size under the pointer and is
// handed over to Windows' own window move, with the button still held
static HWND g_panelDragWin;
static POINT g_panelPress;
static bool g_pullDone;   // the button-up that ends a pull isn't a click on what's under it

static void RestoreMaximized(HWND w) {
    if (g_fake.count(w)) FakeRestore(w, false);   // its tab bar comes back
    else {                                        // its own title bar comes back first
        DecorRestore(w);
        ShowWindowAsync(w, SW_RESTORE);
    }
}

static void PullOut(HWND w, POINT at) {
    RECT full;
    if (!GetWindowRect(w, &full)) return;
    WINDOWPLACEMENT wp = {sizeof(wp)};
    if (g_fake.count(w)) wp = g_fake[w];
    else GetWindowPlacement(w, &wp);
    int ww = wp.rcNormalPosition.right - wp.rcNormalPosition.left, wh = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
    RECT work = WorkArea();
    if (ww < Px(200) || ww >= work.right - work.left) ww = (work.right - work.left) * 6 / 10;   // a sensible size
    if (wh < Px(150) || wh >= work.bottom - work.top) wh = (work.bottom - work.top) * 6 / 10;
    // the same spot of it stays under the pointer, its top edge just above the pointer
    float frac = std::clamp((float)(at.x - full.left) / std::max(1L, full.right - full.left), 0.1f, 0.9f);
    // never with its top under the panel (the panel stays on top: its title bar would be out of reach)
    int left = at.x - (int)(ww * frac), top = std::max((int)at.y - Px(14), (int)work.top);
    RECT to = {left, top, left + ww, top + wh};
    bool wasFake = g_fake.count(w) > 0;
    if (wasFake) FakeForget(w);
    else DecorRestore(w);
    Transitions(w, false);
    wp.length = sizeof(wp);
    wp.flags = 0;
    wp.showCmd = SW_SHOWNORMAL;
    wp.rcNormalPosition = {to.left - work.left, to.top - work.top, to.right - work.left, to.bottom - work.top};
    SetWindowPlacement(w, &wp);
    Transitions(w, true);
    StartGhost(full, to, 160, 0.4f, 0);
    // it follows the pointer itself until the button goes up (Windows' own move loop can't be started reliably from
    // a press on someone else's window); let go on the panel and it's maximized again
    g_pullDone = true;
    POINT grab = {at.x - to.left, at.y - to.top}, last = at;
    int button = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
    while (GetAsyncKeyState(button) & 0x8000) {
        MsgWaitForMultipleObjects(0, nullptr, FALSE, 8, QS_ALLINPUT);
        MSG m;
        while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE)) {
            if (m.message == WM_QUIT) { PostQuitMessage((int)m.wParam); return; }
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
        if (!IsWindow(w)) return;
        POINT p;
        GetCursorPos(&p);
        if (p.x == last.x && p.y == last.y) continue;
        last = p;
        int y = p.y - grab.y;
        if (MonitorFromPoint(p, MONITOR_DEFAULTTOPRIMARY) == MonitorFromPoint({g_screen.left, g_screen.top}, MONITOR_DEFAULTTOPRIMARY))
            y = std::max(y, (int)work.top);   // on the panel's screen: never behind the panel
        SetWindowPos(w, nullptr, p.x - grab.x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
    ReleaseCapture();
    if (!IsWindow(w)) return;
    RECT panel = {g_screen.left, g_screen.top, g_screen.right, work.top};
    if (PtInRect(&panel, last)) ShowWindow(w, SW_MAXIMIZE);   // back on the panel
    SetForegroundWindow(w);
}

static LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_DPICHANGED:   // the main screen's scale changed: everything measured and laid out again
            PostThreadMessageW(GetCurrentThreadId(), WM_APP_REFRESH, 0, 0);
            return 0;
        case WM_LBUTTONDOWN: {
            float x = GET_X_LPARAM(lp) / (float)g_scale, y = GET_Y_LPARAM(lp) / (float)g_scale;
            bool onSomething = false;
            for (auto& h : g_panelHot)
                if (Inside(h.r, x, y)) onSomething = true;
            HWND maxed = MaximizedForeground();
            g_pullDone = false;
            if (!onSomething && maxed && !g_dashOpen) {
                g_panelDragWin = maxed;
                GetCursorPos(&g_panelPress);
                SetCapture(hwnd);
            }
            return 0;
        }
        case WM_CAPTURECHANGED:
            g_panelDragWin = nullptr;
            return 0;
        case WM_MOUSEMOVE: {
            if (g_panelDragWin && (wp & MK_LBUTTON)) {
                POINT pt;
                GetCursorPos(&pt);
                if (pt.y - g_panelPress.y > Px(6) || abs(pt.x - g_panelPress.x) > Px(12)) {
                    HWND w = g_panelDragWin;
                    g_panelDragWin = nullptr;
                    PullOut(w, pt);
                }
                return 0;
            }
            float x = GET_X_LPARAM(lp) / (float)g_scale, y = GET_Y_LPARAM(lp) / (float)g_scale;
            int hover = -1;
            for (auto& h : g_panelHot)
                if (Inside(h.r, x, y)) hover = h.id;
            if (hover != g_panelHover) { g_panelHover = hover; RenderPanel(); }
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }
        case WM_MOUSELEAVE:
            if (g_panelHover != -1) { g_panelHover = -1; RenderPanel(); }
            return 0;
        case WM_LBUTTONUP: {
            if (g_pullDone) { g_pullDone = false; return 0; }
            if (g_panelDragWin) ReleaseCapture();   // a click on the empty panel: nothing
            float x = GET_X_LPARAM(lp) / (float)g_scale, y = GET_Y_LPARAM(lp) / (float)g_scale;
            for (auto& h : g_panelHot) {
                if (!Inside(h.r, x, y)) continue;
                RECT at = {Px(h.r.left), Px(h.r.top), Px(h.r.right), Px(h.r.bottom)};
                MapWindowPoints(hwnd, nullptr, (POINT*)&at, 2);
                HWND maxed = MaximizedForeground();
                if (h.id == HOT_WINCLOSE) {
                    if (g_dashOpen) ShowDash(false);
                    else if (maxed) PostMessageW(maxed, WM_CLOSE, 0, 0);
                } else if (h.id == HOT_WINMIN) {
                    if (!g_dashOpen && maxed) Minimize(maxed);
                } else if (h.id == HOT_WINMAX) {
                    if (g_dashOpen) SetDashMax(!g_dashMax);
                    else if (maxed) RestoreMaximized(maxed);
                } else if (h.id >= HOT_TAB && h.id < HOT_TAB + 1000) TabCommand(1, h.id - HOT_TAB);
                else if (h.id >= HOT_TABCLOSE && h.id < HOT_TABCLOSE + 1000) TabCommand(2, h.id - HOT_TABCLOSE);
                else if (h.id == HOT_NEWTAB) TabCommand(3, 0);
                else if (h.id == HOT_SESSION) SessionMenu(at);
                else if (h.id == HOT_CLOCK) ShowPopup(POP_CALENDAR, at);
                else if (h.id == HOT_VOLUME) ShowPopup(POP_VOLUME, at);
                else if (h.id == HOT_TRAY) ShowPopup(POP_TRAY, at);
                else if (h.id == HOT_MESSAGES) Launch(L"ms-actioncenter:");
                else if (h.id == HOT_NETWORK) Launch(L"ms-settings:network-status");
                else if (h.id == HOT_LANGUAGE) Launch(L"ms-settings:regionlanguage");
            }
            return 0;
        }
        case WM_LBUTTONDBLCLK: {   // like a title bar: the maximized window in front is restored
            float x = GET_X_LPARAM(lp) / (float)g_scale, y = GET_Y_LPARAM(lp) / (float)g_scale;
            bool onSomething = false;
            for (auto& h : g_panelHot)
                if (Inside(h.r, x, y)) onSomething = true;
            HWND maxed = MaximizedForeground();
            if (!onSomething && maxed && !g_dashOpen) {   // the double-click setting
                if (g_set.doubleClick == L"restore") RestoreMaximized(maxed);
                else if (g_set.doubleClick == L"minimize") Minimize(maxed);
            }
            return 0;
        }
        case WM_MOUSEWHEEL: {
            ChangeVolume(GET_WHEEL_DELTA_WPARAM(wp) > 0 ? 0.05f : -0.05f);
            RenderPanel();
            return 0;
        }
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ============================================================================================================
// launcher
// ============================================================================================================

static const int kTrash = -3;          // the trash at the bottom of the launcher
static const int kExt = -4;            // the media block under an icon (see the media extension)
static int g_extHost = -1, g_extHover = -1;   // the item it hangs under; the part under the pointer
static Anim g_aExt;                    // 0 shut .. 1 open
static double g_extLastPlaying = -1e9;
static std::wstring g_extKey;
static bool g_extPlaying;
static float ExtFullH();
static float ExtH() { return g_extHost >= 0 ? ExtFullH() * std::clamp(g_aExt.value, 0.f, 1.f) : 0; }
// the Discord call block under Discord's icon (see the Discord extension)
static const int kDc = -5;
static int g_dcHost = -1, g_dcHover = -1;
static Anim g_aDc;
static volatile LONG g_dcCmd;   // 1 mic, 2 sound, 3 hang up
static volatile bool g_dcCall, g_dcMic, g_dcDeaf, g_dcStop;
static ULONGLONG g_dcHangAt;   // when the hang-up button was pressed
static HANDLE g_dcWake, g_dcThread;
static float DcFullH();
static Anim g_aAway;   // 0 in place .. 1 slid out of sight (a full-screen game or video in front)
static float DcH() { return g_dcHost >= 0 ? DcFullH() * std::clamp(g_aDc.value, 0.f, 1.f) : 0; }
static const wchar_t kBfbFx[] = L"<dash button>", kTrashFx[] = L"<trash>";   // their animations' keys
static float HoverZoom(float hover) { return g_set.hoverZoom ? 1 + 0.07f * hover : 1; }   // a hovered tile grows a little (the app's)
static int g_launcherHover = -2;   // -1 = the Dash button, -2 = nothing, kTrash = the trash

static D2D1_RECT_F ItemRect(int index) {   // index -1 = Dash button
    float x = 8;
    if (index == kTrash) {
        float h = (float)((g_screen.bottom - g_screen.top) / g_scale - PanelH());
        return D2D1::RectF(x, h - 6 - g_set.iconSize, x + g_set.iconSize, h - 6);
    }
    float y = 6 + (index + 1) * (float)ItemH() + (g_extHost >= 0 && index > g_extHost ? ExtH() : 0) + (g_dcHost >= 0 && index > g_dcHost ? DcH() : 0);
    return D2D1::RectF(x, y, x + g_set.iconSize, y + g_set.iconSize);
}

static int VisibleItems() {   // the icons that fit above the trash
    float limit = ItemRect(kTrash).top - 4;
    int n = 0;
    while (n < (int)g_items.size() && ItemRect(n).bottom <= limit) n++;
    return n;
}

static bool InExtension(float x, float y);
static bool InDcExtension(float x, float y);

static int ItemAt(float x, float y) {
    if (InExtension(x, y)) return kExt;
    if (InDcExtension(x, y)) return kDc;
    D2D1_RECT_F t = ItemRect(kTrash);
    if (y >= t.top - 4 && y < t.bottom + 4) return kTrash;
    for (int i = -1; i < VisibleItems(); i++) {
        D2D1_RECT_F r = ItemRect(i);
        if (y >= r.top - 4 && y < r.bottom + 4) return i;
    }
    return -2;
}

// ---------------- the trash ----------------

static const wchar_t kTrashParsing[] = L"::{645FF040-5081-101B-9F08-00AA002F954E}";
static bool g_trashFull;

static void CheckTrash(bool now = false) {
    static DWORD last;
    if (!now && last && GetTickCount() - last < 3000) return;
    last = GetTickCount();
    SHQUERYRBINFO q = {sizeof(q)};
    bool full = SUCCEEDED(SHQueryRecycleBinW(nullptr, &q)) && q.i64NumItems > 0;
    if (full == g_trashFull) return;
    g_trashFull = full;
    // its picture follows: drop the cached one so the next frame loads the full / empty bin
    auto icon = g_icons.find(kTrashParsing);
    if (icon != g_icons.end()) { Release(icon->second.src); g_icons.erase(icon); }
    for (auto bmp = g_launcherS.bitmaps.begin(); bmp != g_launcherS.bitmaps.end();)   // every size of it
        if (bmp->first.rfind(kTrashParsing, 0) == 0) { Release(bmp->second); bmp = g_launcherS.bitmaps.erase(bmp); }
        else ++bmp;
}

// ---------------- dragging icons to reorder (or off the launcher to unpin) ----------------

static int g_pressIndex = -2, g_dragIndex = -2, g_dragTarget = -1, g_dragOnto = -1;   // onto: an icon to group with
static POINT g_pressPt;
static float g_dragY;              // the dragged icon's centre, DIPs
static bool g_dragOut;             // dragged off the launcher: drop to unpin

static int DragSlot(float y) {
    int n = VisibleItems();
    if (n == 0) return 0;
    return std::clamp((int)floorf((y - 6) / (float)ItemH()) - 1, 0, n - 1);
}

static void DrawTile(Surface& s, D2D1_RECT_F r, Rgb back, float hover, float alpha = 1) {
    float rad = 8;
    auto rr = D2D1::RoundedRect(r, rad, rad);
    // Unity's backlit tile: the icon's colour, brighter at the top; hover lights it up (animated)
    float fill = (0.8f + 0.15f * hover) * alpha;
    ID2D1GradientStopCollection* stops = nullptr;
    D2D1_GRADIENT_STOP gs[2] = {{0, D2D1::ColorF(std::min(1.f, back.r * 1.25f), std::min(1.f, back.g * 1.25f), std::min(1.f, back.b * 1.25f), fill)},
                                {1, D2D1::ColorF(back.r * 0.7f, back.g * 0.7f, back.b * 0.7f, fill)}};
    s.rt->CreateGradientStopCollection(gs, 2, &stops);
    ID2D1LinearGradientBrush* grad = nullptr;
    if (stops) s.rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.left, r.bottom)), stops, &grad);
    if (grad) s.rt->FillRoundedRectangle(rr, grad);
    Release(grad);
    Release(stops);
    s.rt->DrawRoundedRectangle(rr, s.White((0.3f + 0.25f * hover) * alpha), 1);
    // gloss on the upper half
    s.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + 1, r.top + 1, r.right - 1, (r.top + r.bottom) / 2), rad - 1, rad - 1), s.White(0.08f * alpha));
}

static void Triangle(Surface& s, D2D1_POINT_2F a, D2D1_POINT_2F b, D2D1_POINT_2F c, float alpha) {
    ID2D1PathGeometry* tri = nullptr;
    g_d2d->CreatePathGeometry(&tri);
    ID2D1GeometrySink* sink = nullptr;
    if (tri && SUCCEEDED(tri->Open(&sink))) {
        sink->BeginFigure(a, D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(b);
        sink->AddLine(c);
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();
        s.rt->FillGeometry(tri, s.White(alpha));
    }
    Release(sink);
    Release(tri);
}

static ID2D1PathGeometry* Path(const std::function<void(ID2D1GeometrySink*)>& build) {
    ID2D1PathGeometry* geo = nullptr;
    g_d2d->CreatePathGeometry(&geo);
    ID2D1GeometrySink* sink = nullptr;
    if (geo && SUCCEEDED(geo->Open(&sink))) {
        build(sink);
        sink->Close();
    }
    Release(sink);
    return geo;
}

static D2D1_ARC_SEGMENT Arc(float x, float y, float r, bool clockwise) {
    return D2D1::ArcSegment(D2D1::Point2F(x, y), D2D1::SizeF(r, r), 0, clockwise ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_SMALL);
}

// the Dash button's "Windows remix" logo (the app's): Ubuntu's circle of friends round the four Windows panes
static void DrawRemixLogo(Surface& s, float cx, float cy, float d, float alpha) {
    float ring = d * 0.74f, stroke = d * 0.085f, hd = d * 0.22f, rr = ring / 2 + stroke * 0.1f, gap = (hd + stroke * 0.9f) / 2;
    float ang[3];
    for (int i = 0; i < 3; i++) ang[i] = (float)((i * 120 - 30) * 3.14159265 / 180);
    // the ring, broken around the three heads (cut out of a layer mask)
    ID2D1RectangleGeometry* area = nullptr;
    g_d2d->CreateRectangleGeometry(D2D1::RectF(cx - d, cy - d, cx + d, cy + d), &area);
    ID2D1Geometry* mask = area;
    for (int i = 0; mask && i < 3; i++) {
        ID2D1EllipseGeometry* hole = nullptr;
        g_d2d->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(cx + cosf(ang[i]) * rr, cy + sinf(ang[i]) * rr), gap, gap), &hole);
        ID2D1PathGeometry* rest = nullptr;
        g_d2d->CreatePathGeometry(&rest);
        ID2D1GeometrySink* sink = nullptr;
        if (hole && rest && SUCCEEDED(rest->Open(&sink))) {
            mask->CombineWithGeometry(hole, D2D1_COMBINE_MODE_EXCLUDE, nullptr, sink);
            sink->Close();
        }
        Release(sink);
        Release(hole);
        mask->Release();
        mask = rest;
    }
    ID2D1Layer* layer = nullptr;
    if (mask && SUCCEEDED(s.rt->CreateLayer(&layer))) {
        s.rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), mask), layer);
        s.rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), ring / 2, ring / 2), s.White(alpha), stroke);
        s.rt->PopLayer();
    }
    Release(layer);
    Release(mask);
    for (int i = 0; i < 3; i++)
        s.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + cosf(ang[i]) * rr, cy + sinf(ang[i]) * rr), hd / 2, hd / 2), s.White(alpha));
    float panes = ring * 0.44f, gapW = panes * 0.09f, pane = (panes - gapW) / 2, pr = pane * 0.12f;
    for (int px = 0; px < 2; px++)
        for (int py = 0; py < 2; py++) {
            float l = cx - panes / 2 + px * (pane + gapW), t = cy - panes / 2 + py * (pane + gapW);
            s.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(l, t, l + pane, t + pane), pr, pr), s.White(alpha));
        }
}

static const float kFrame = 28, kBendR = 12, kStrip = 3;   // the Dash's band, the radius of its bends, the strip along the launcher

// the Dash's size: the setting, or what its grip was last dragged to (kept until the setting is changed), or the
// whole screen next to the launcher when maximized (then without the band)
static float g_dashW, g_dashH;   // dragged size in DIPs, 0 = the setting's
static const float kDashMinW = 560, kDashMinH = 380;

static float Frame() { return g_dashMax ? 0.f : kFrame; }
static float DashRoomW() { return (float)((g_screen.right - g_screen.left) / g_scale - LauncherW()); }
static float DashRoomH() { return (float)((g_screen.bottom - g_screen.top) / g_scale - PanelH()); }
static float DashW() {
    if (g_dashMax) return DashRoomW();
    return std::min(std::max(g_dashW > 0 ? g_dashW : (float)g_set.dashW, kDashMinW), DashRoomW() - kFrame);
}
static float DashH() {
    if (g_dashMax) return DashRoomH();
    return std::min(std::max(g_dashH > 0 ? g_dashH : (float)g_set.dashH, kDashMinH), DashRoomH() - kFrame);
}

static void LoadDashSize() {
    g_dashMax = Wh_GetIntValue(L"dashMax", 0) != 0;
    bool sameSetting = Wh_GetIntValue(L"dashFromW", 0) == g_set.dashW && Wh_GetIntValue(L"dashFromH", 0) == g_set.dashH;
    g_dashW = sameSetting ? (float)Wh_GetIntValue(L"dashW", 0) : 0;
    g_dashH = sameSetting ? (float)Wh_GetIntValue(L"dashH", 0) : 0;
}

static void SaveDashSize() {
    Wh_SetIntValue(L"dashMax", g_dashMax ? 1 : 0);
    Wh_SetIntValue(L"dashW", (int)g_dashW);
    Wh_SetIntValue(L"dashH", (int)g_dashH);
    Wh_SetIntValue(L"dashFromW", g_set.dashW);
    Wh_SetIntValue(L"dashFromH", g_set.dashH);
}

static bool DashShown() { return g_dash && IsWindowVisible(g_dash); }

static void ShowDash(bool show);
static void MediaCommand(LONG cmd);
static void RenderLauncher();
static void DrawFolderTile(Surface& s, D2D1_RECT_F r, const PinGroup& f, float hover, float alpha);
static void MakeGroup(int from, int onto);
// ---------------- the media extension (the app's): under the icon of the app playing music, a block with the
// cover, play/pause and previous/next slides open; it stays while playing and for a minute after pausing ----------------

static float ExtFullH() {   // the block plus its margins
    float w = (float)g_set.iconSize;
    return 1 + 4 + (w - 8) + 3 + (w * 0.42f + 2) + (w * 0.36f + 2) + 4 + 6;
}

static D2D1_RECT_F ExtRect() {   // where it is now (growing / shrinking with its animation)
    D2D1_RECT_F r = ItemRect(g_extHost);
    return D2D1::RectF(r.left, r.bottom + 5, r.right, r.bottom + 5 + std::max(0.f, ExtH() - 6));
}

// its parts at full size: 0 cover, 1 play/pause, 2 previous, 3 next
static void ExtParts(D2D1_RECT_F parts[4]) {
    D2D1_RECT_F e = ExtRect();
    float w = (float)g_set.iconSize, x0 = e.left + 4, x1 = e.right - 4, y = e.top + 4;
    parts[0] = D2D1::RectF(x0, y, x1, y + (w - 8));
    y += (w - 8) + 3;
    parts[1] = D2D1::RectF(x0, y, x1, y + w * 0.42f);
    y += w * 0.42f + 2;
    float mid = (x0 + x1) / 2;
    parts[2] = D2D1::RectF(x0, y, mid - 1, y + w * 0.36f);
    parts[3] = D2D1::RectF(mid + 1, y, x1, y + w * 0.36f);
}

static int ExtPartAt(float x, float y) {
    D2D1_RECT_F parts[4];
    ExtParts(parts);
    for (int k = 0; k < 4; k++)
        if (Inside(parts[k], x, y)) return k;
    return -1;
}

// "Spotify.exe" -> "spotify", "Microsoft.ZuneMusic_8wekyb3d8bbwe!App" -> "zunemusic"
static std::wstring MediaHint(const std::wstring& aumid) {
    std::wstring a = Lower(aumid);
    size_t cut = a.find_first_of(L"!_");
    if (cut != std::wstring::npos && cut > 0) a = a.substr(0, cut);
    if (a.size() > 4 && a.ends_with(L".exe")) a = a.substr(0, a.size() - 4);
    size_t slash = a.find_last_of(L"\\");
    if (slash != std::wstring::npos) a = a.substr(slash + 1);
    size_t dot = a.find_last_of(L'.');
    if (dot != std::wstring::npos) a = a.substr(dot + 1);
    return a;
}

static bool ItemPlays(const Item& it, const std::wstring& hint) {
    return !hint.empty() && (Lower(it.name).find(hint) != std::wstring::npos || Lower(it.launch).find(hint) != std::wstring::npos ||
                             (!it.exe.empty() && Lower(it.exe).find(hint) != std::wstring::npos));
}

// after news from the media thread, or a refresh: which icon gets the block, open or closing
static void UpdateExtension() {
    if (!g_mediaReady) return;
    g_mediaLock.lock();
    bool has = g_mHas, playing = g_mPlaying;
    std::wstring hint = MediaHint(g_mSource), key = g_mTitle + L"|" + std::to_wstring(g_mCoverKey);
    g_mediaLock.unlock();
    if (playing) g_extLastPlaying = NowMs();
    int host = -1;
    if (g_set.mediaControls && has && (playing || NowMs() - g_extLastPlaying < 60000))
        for (int i = 0; i < (int)g_items.size() && host < 0; i++)
            if (ItemPlays(g_items[i], hint)) host = i;
    bool changed = key != g_extKey || playing != g_extPlaying;
    g_extKey = key;
    g_extPlaying = playing;
    if (host >= 0) {
        bool wasOpen = g_extHost >= 0 && g_aExt.to > 0;
        g_extHost = host;
        if (!wasOpen) g_aExt.Go(1, 450, EASE_OUT_CUBIC, 0);
        KickAnim();
    } else if (g_extHost >= 0 && g_aExt.to > 0) {
        g_aExt.Go(0, 280, EASE_OUT_CUBIC);   // slides shut, then lets go of its icon (see AnimFrame)
        KickAnim();
    }
    if (changed) RenderLauncher();
}

static void DrawExtension(Surface& s) {
    if (g_extHost < 0 || g_extHost >= VisibleItems() || ExtH() < 7) return;
    D2D1_RECT_F e = ExtRect(), parts[4];
    ExtParts(parts);
    float w = (float)g_set.iconSize, a = std::clamp(g_aExt.value * 1.5f, 0.f, 1.f);
    D2D1_RECT_F full = D2D1::RectF(e.left, e.top, e.right, e.top + ExtFullH() - 7);
    s.rt->PushAxisAlignedClip(D2D1::RectF(e.left - 1, e.top - 1, e.right + 1, e.bottom + 1), D2D1_ANTIALIAS_MODE_ALIASED);
    auto box = D2D1::RoundedRect(D2D1::RectF(full.left + 0.5f, full.top + 0.5f, full.right - 0.5f, full.bottom - 0.5f), w * 0.16f, w * 0.16f);
    s.rt->FillRoundedRectangle(box, s.White(0x16 / 255.f * a));
    s.rt->DrawRoundedRectangle(box, s.White(0x38 / 255.f * a), 1);
    // the cover, with rounded corners
    g_mediaLock.lock();
    std::wstring coverKey = L"cover#" + std::to_wstring(g_mCoverKey);
    ID2D1Bitmap* cover = nullptr;
    auto cached = s.bitmaps.find(coverKey);
    if (cached != s.bitmaps.end()) cover = cached->second;
    else if (g_mCover) { s.rt->CreateBitmapFromWicBitmap(g_mCover, nullptr, &cover); s.bitmaps[coverKey] = cover; }
    bool playing = g_mPlaying;
    g_mediaLock.unlock();
    ID2D1RoundedRectangleGeometry* round = nullptr;
    ID2D1Layer* layer = nullptr;
    if (cover && SUCCEEDED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(parts[0], 4, 4), &round)) && SUCCEEDED(s.rt->CreateLayer(&layer))) {
        s.rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), round), layer);
        s.rt->DrawBitmap(cover, parts[0], a, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        s.rt->PopLayer();
    } else s.rt->FillRoundedRectangle(D2D1::RoundedRect(parts[0], 4, 4), s.White(0.08f * a));
    Release(layer);
    Release(round);
    const wchar_t* glyphs[4] = {nullptr, playing ? L"\uE769" : L"\uE768", L"\uE892", L"\uE893"};
    for (int k = 1; k < 4; k++) {
        float h = parts[k].bottom - parts[k].top;
        if (g_extHover == k) s.rt->FillRoundedRectangle(D2D1::RoundedRect(parts[k], h * 0.3f, h * 0.3f), s.White(0x30 / 255.f * a));
        Glyph(s, glyphs[k], parts[k], roundf(std::min(w * 0.3f, h * 0.6f)), a);
    }
    s.rt->PopAxisAlignedClip();
}

// a click on the block: the cover brings the player forward, the buttons control it
static void ExtClick(float x, float y) {
    int part = ExtPartAt(x, y);
    if (part == 1) MediaCommand(2);
    else if (part == 2) MediaCommand(1);
    else if (part == 3) MediaCommand(3);
    else if (part == 0 && g_extHost >= 0 && g_extHost < (int)g_items.size() && !g_items[g_extHost].windows.empty()) {
        if (g_dashOpen) ShowDash(false);
        Activate(g_items[g_extHost].windows[0]);
    }
}

static bool InExtension(float x, float y) {
    if (g_extHost < 0 || ExtH() < 7) return false;
    D2D1_RECT_F e = ExtRect();
    return x >= e.left - 4 && x < e.right + 4 && y >= e.top && y < e.bottom;
}

// ---------------- the Discord extension (the app's): while you're in a call (Discord is using a microphone), a
// block under Discord's icon with mic, sound and hang-up buttons; they press Discord's own buttons ----------------

static float DcFullH() {   // the block plus its margins
    float w = (float)g_set.iconSize;
    return 1 + 4 + 3 * (w * 0.48f) + 2 * 2 + 4 + 6;
}

static D2D1_RECT_F DcRect() {   // where it is now (growing / shrinking with its animation)
    D2D1_RECT_F r = ItemRect(g_dcHost);
    if (g_extHost == g_dcHost) r.bottom += ExtH();   // under the media block, if Discord plays music too
    return D2D1::RectF(r.left, r.bottom + 5, r.right, r.bottom + 5 + std::max(0.f, DcH() - 6));
}

// its buttons at full size: 0 mic, 1 sound, 2 hang up
static void DcParts(D2D1_RECT_F parts[3]) {
    D2D1_RECT_F e = DcRect();
    float bh = g_set.iconSize * 0.48f, y = e.top + 4;
    for (int k = 0; k < 3; k++, y += bh + 2) parts[k] = D2D1::RectF(e.left + 4, y, e.right - 4, y + bh);
}

static int DcPartAt(float x, float y) {
    D2D1_RECT_F parts[3];
    DcParts(parts);
    for (int k = 0; k < 3; k++)
        if (Inside(parts[k], x, y)) return k;
    return -1;
}

static bool InDcExtension(float x, float y) {
    if (g_dcHost < 0 || DcH() < 7) return false;
    D2D1_RECT_F e = DcRect();
    return x >= e.left - 4 && x < e.right + 4 && y >= e.top && y < e.bottom;
}

// after news from the Discord thread, or a refresh: which icon gets the block, open or closing
static void UpdateDcExtension() {
    int host = -1;
    if (g_dcCall && GetTickCount64() - g_dcHangAt > 2500)
        for (int i = 0; i < (int)g_items.size() && host < 0; i++)
            if (ItemPlays(g_items[i], L"discord")) host = i;
    if (host >= 0) {
        bool wasOpen = g_dcHost >= 0 && g_aDc.to > 0;
        g_dcHost = host;
        if (!wasOpen) g_aDc.Go(1, 450, EASE_OUT_CUBIC, 0);
        KickAnim();
    } else if (g_dcHost >= 0 && g_aDc.to > 0) {
        g_aDc.Go(0, 280, EASE_OUT_CUBIC);   // slides shut, then lets go of its icon (see AnimFrame)
        KickAnim();
    }
    RenderLauncher();
}

static void DrawDcExtension(Surface& s) {
    if (g_dcHost < 0 || g_dcHost >= VisibleItems() || DcH() < 7) return;
    D2D1_RECT_F e = DcRect(), parts[3];
    DcParts(parts);
    float w = (float)g_set.iconSize, a = std::clamp(g_aDc.value * 1.5f, 0.f, 1.f);
    D2D1_RECT_F full = D2D1::RectF(e.left, e.top, e.right, e.top + DcFullH() - 7);
    s.rt->PushAxisAlignedClip(D2D1::RectF(e.left - 1, e.top - 1, e.right + 1, e.bottom + 1), D2D1_ANTIALIAS_MODE_ALIASED);
    auto box = D2D1::RoundedRect(D2D1::RectF(full.left + 0.5f, full.top + 0.5f, full.right - 0.5f, full.bottom - 0.5f), w * 0.16f, w * 0.16f);
    s.rt->FillRoundedRectangle(box, s.Brush({0x58 / 255.f, 0x65 / 255.f, 0xF2 / 255.f}, 0x70 / 255.f * a));   // Discord's blurple
    s.rt->DrawRoundedRectangle(box, s.White(0x38 / 255.f * a), 1);
    bool deaf = g_dcDeaf, mic = g_dcMic || deaf;   // deafen mutes the mic too, in Discord
    bool alert[3] = {mic, deaf, true};
    const wchar_t* glyphs[3] = {mic ? L"\uEC54" : L"\uE720", deaf ? L"\uE74F" : L"\uE767", L"\uE717"};
    for (int k = 0; k < 3; k++) {
        float h = parts[k].bottom - parts[k].top, rad = h * 0.3f;
        auto rr = D2D1::RoundedRect(parts[k], rad, rad);
        if (k == 2) s.rt->FillRoundedRectangle(rr, s.Brush({0xC4 / 255.f, 0x2B / 255.f, 0x1C / 255.f}, (g_dcHover == 2 ? 0.95f : 0x0B0 / 255.f) * a));   // hang up: red
        else if (alert[k]) s.rt->FillRoundedRectangle(rr, s.Brush({0xC4 / 255.f, 0x2B / 255.f, 0x1C / 255.f}, (g_dcHover == k ? 0.75f : 0.55f) * a));   // muted: reddish
        else if (g_dcHover == k) s.rt->FillRoundedRectangle(rr, s.White(0x30 / 255.f * a));
        float cx = (parts[k].left + parts[k].right) / 2, cy = (parts[k].top + parts[k].bottom) / 2;
        if (k == 2) s.rt->SetTransform(D2D1::Matrix3x2F::Rotation(135, D2D1::Point2F(cx, cy)));   // the handset upside down: hang up
        Glyph(s, glyphs[k], parts[k], roundf(std::min(w * 0.3f, h * 0.6f)), a);
        if (k == 2) s.rt->SetTransform(D2D1::Matrix3x2F::Identity());
    }
    s.rt->PopAxisAlignedClip();
}

static void DcClick(float x, float y) {
    int part = DcPartAt(x, y);
    if (part < 0) return;
    // shown at once; the next read of Discord's window confirms it
    if (part == 0) g_dcMic = !g_dcMic;
    if (part == 1) g_dcDeaf = !g_dcDeaf;
    if (part == 2) {   // hung up: the block shuts right away (it opens again if the call goes on after all)
        g_dcHangAt = GetTickCount64();
        g_dcHover = -1;
        g_aDc.Go(0, 280, EASE_OUT_CUBIC);
        KickAnim();
    }
    InterlockedExchange(&g_dcCmd, part + 1);
    if (g_dcWake) SetEvent(g_dcWake);
    RenderLauncher();
}

// ---- the Discord thread: is it in a call (Core Audio), mute / deafen (its window, through UI Automation) ----

static std::vector<DWORD> DiscordPids() {   // listing the processes is the expensive part: once per 10 s
    static std::vector<DWORD> cache;
    static ULONGLONG at;
    if (at && GetTickCount64() - at < 10000) return cache;
    at = GetTickCount64();
    cache.clear();
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return cache;
    PROCESSENTRY32W pe = {sizeof(pe)};
    for (BOOL ok = Process32FirstW(snap, &pe); ok; ok = Process32NextW(snap, &pe))
        if (!_wcsicmp(pe.szExeFile, L"Discord.exe")) cache.push_back(pe.th32ProcessID);
    CloseHandle(snap);
    return cache;
}

// in a call = Discord is using a microphone right now
static bool DiscordInCall(const std::vector<DWORD>& pids) {
    if (pids.empty()) return false;
    bool call = false;
    IMMDeviceEnumerator* en = nullptr;
    IMMDeviceCollection* devices = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&en)) &&
        SUCCEEDED(en->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &devices))) {
        UINT n = 0;
        devices->GetCount(&n);
        for (UINT i = 0; i < n && !call; i++) {
            IMMDevice* dev = nullptr;
            IAudioSessionManager2* mgr = nullptr;
            IAudioSessionEnumerator* sessions = nullptr;
            if (SUCCEEDED(devices->Item(i, &dev)) && SUCCEEDED(dev->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&mgr)) &&
                SUCCEEDED(mgr->GetSessionEnumerator(&sessions))) {
                int count = 0;
                sessions->GetCount(&count);
                for (int k = 0; k < count && !call; k++) {
                    IAudioSessionControl* ctl = nullptr;
                    IAudioSessionControl2* ctl2 = nullptr;
                    if (SUCCEEDED(sessions->GetSession(k, &ctl)) && SUCCEEDED(ctl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&ctl2))) {
                        DWORD pid = 0;
                        AudioSessionState state = AudioSessionStateInactive;
                        ctl2->GetProcessId(&pid);
                        ctl2->GetState(&state);
                        if (state == AudioSessionStateActive && std::find(pids.begin(), pids.end(), pid) != pids.end()) call = true;
                    }
                    Release(ctl2);
                    Release(ctl);
                }
            }
            Release(sessions);
            Release(mgr);
            Release(dev);
        }
    }
    Release(devices);
    Release(en);
    return call;
}

static HWND DiscordWindow(const std::vector<DWORD>& pids) {   // its main window (the one with the thumbnail buttons)
    struct Find { const std::vector<DWORD>* pids; HWND found; } f = {&pids, nullptr};
    EnumWindows([](HWND h, LPARAM lp) -> BOOL {
        auto* f = (Find*)lp;
        DWORD pid = 0;
        GetWindowThreadProcessId(h, &pid);
        if (std::find(f->pids->begin(), f->pids->end(), pid) == f->pids->end() || !IsWindowVisible(h) || GetWindow(h, GW_OWNER) || !GetWindowTextLengthW(h)) return TRUE;
        f->found = h;
        return FALSE;
    }, (LPARAM)&f);
    return f.found;
}

static IUIAutomationCondition* NamesCondition(IUIAutomation* uia, std::initializer_list<const wchar_t*> names) {
    std::vector<IUIAutomationCondition*> parts;
    for (const wchar_t* n : names) {
        VARIANT v;
        v.vt = VT_BSTR;
        v.bstrVal = SysAllocString(n);
        IUIAutomationCondition* c = nullptr;
        if (SUCCEEDED(uia->CreatePropertyCondition(UIA_NamePropertyId, v, &c)) && c) parts.push_back(c);
        SysFreeString(v.bstrVal);
    }
    IUIAutomationCondition* any = nullptr;
    if (!parts.empty()) uia->CreateOrConditionFromNativeArray(parts.data(), (int)parts.size(), &any);
    for (auto* c : parts) c->Release();
    return any;
}

static bool PressInDiscord(IUIAutomation* uia, HWND w, IUIAutomationCondition* which) {
    IUIAutomationElement *root = nullptr, *btn = nullptr;
    bool done = false;
    if (w && which && SUCCEEDED(uia->ElementFromHandle(w, &root)) && root && SUCCEEDED(root->FindFirst(TreeScope_Descendants, which, &btn)) && btn) {
        IUIAutomationInvokePattern* inv = nullptr;
        IUIAutomationTogglePattern* tog = nullptr;
        if (SUCCEEDED(btn->GetCurrentPatternAs(UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern), (void**)&inv)) && inv) done = SUCCEEDED(inv->Invoke());
        else if (SUCCEEDED(btn->GetCurrentPatternAs(UIA_TogglePatternId, __uuidof(IUIAutomationTogglePattern), (void**)&tog)) && tog) done = SUCCEEDED(tog->Toggle());
        Release(inv);
        Release(tog);
    }
    Release(btn);
    Release(root);
    return done;
}

// its Disconnect button is there: still in the call (muted or deafened, Discord lets go of the microphone)
static bool DiscordHasDisconnect(IUIAutomation* uia, HWND w, IUIAutomationCondition* hang) {
    IUIAutomationElement *root = nullptr, *btn = nullptr;
    bool found = w && hang && SUCCEEDED(uia->ElementFromHandle(w, &root)) && root && SUCCEEDED(root->FindFirst(TreeScope_Descendants, hang, &btn)) && btn;
    Release(btn);
    Release(root);
    return found;
}

static bool NameIn(BSTR name, std::initializer_list<const wchar_t*> names) {
    for (const wchar_t* n : names)
        if (name && !wcscmp(name, n)) return true;
    return false;
}

#define DC_MUTE {L"Mute", L"Unmute", L"Dempen", L"Dempen opheffen"}
#define DC_DEAF {L"Deafen", L"Undeafen", L"Onhoorbaar maken", L"Hoorbaar maken"}

// Discord's real mute / deafen state, from the toggle buttons in its window
static void ReadDiscordState(IUIAutomation* uia, HWND w, IUIAutomationCondition* toggles) {
    IUIAutomationElement* root = nullptr;
    IUIAutomationElementArray* found = nullptr;
    if (w && toggles && SUCCEEDED(uia->ElementFromHandle(w, &root)) && root && SUCCEEDED(root->FindAll(TreeScope_Descendants, toggles, &found)) && found) {
        int n = 0;
        found->get_Length(&n);
        for (int i = 0; i < n; i++) {
            IUIAutomationElement* e = nullptr;
            IUIAutomationTogglePattern* tog = nullptr;
            BSTR name = nullptr;
            ToggleState st = ToggleState_Off;
            if (SUCCEEDED(found->GetElement(i, &e)) && e && SUCCEEDED(e->get_CurrentName(&name)) &&
                SUCCEEDED(e->GetCurrentPatternAs(UIA_TogglePatternId, __uuidof(IUIAutomationTogglePattern), (void**)&tog)) && tog &&
                SUCCEEDED(tog->get_CurrentToggleState(&st))) {
                if (NameIn(name, DC_MUTE)) g_dcMic = st == ToggleState_On;
                else if (NameIn(name, DC_DEAF)) g_dcDeaf = st == ToggleState_On;
            }
            if (name) SysFreeString(name);
            Release(tog);
            Release(e);
        }
    }
    Release(found);
    Release(root);
}

static DWORD WINAPI DiscordWorker(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    IUIAutomation* uia = MakeUia();
    IUIAutomationCondition *toggles = nullptr, *mute = nullptr, *deaf = nullptr, *hang = nullptr;
    if (uia) {
        VARIANT v;
        v.vt = VT_BOOL;
        v.boolVal = VARIANT_TRUE;
        uia->CreatePropertyCondition(UIA_IsTogglePatternAvailablePropertyId, v, &toggles);
        mute = NamesCondition(uia, DC_MUTE);
        deaf = NamesCondition(uia, DC_DEAF);
        IUIAutomationCondition *names = NamesCondition(uia, {L"Disconnect", L"Verbinding verbreken", L"Ophangen"}), *button = nullptr;
        v.vt = VT_I4;
        v.lVal = UIA_ButtonControlTypeId;
        uia->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &button);
        if (names && button) uia->CreateAndCondition(names, button, &hang);
        Release(names);
        Release(button);
    }
    int ticks = 0;
    while (!g_dcStop) {
        WaitForSingleObject(g_dcWake, 1000);
        if (g_dcStop) break;
        LONG cmd = InterlockedExchange(&g_dcCmd, 0);
        ticks++;
        // not in a call: a look every 3 s is plenty (a call doesn't start in a blink); in a call: every second
        if (!cmd && !g_dcCall && ticks % 3 != 0) continue;
        auto pids = DiscordPids();
        HWND w = DiscordWindow(pids);
        if (uia && cmd == 1) PressInDiscord(uia, w, mute);
        if (uia && cmd == 2) PressInDiscord(uia, w, deaf);
        if (cmd == 3) {   // its window's own Disconnect button; failing that, its thumbnail one (Electron numbers them 0, 1, 2)
            if (uia) PressInDiscord(uia, w, hang);
            Sleep(300);
        }
        if (cmd == 1 || cmd == 2) Sleep(400);   // let Discord catch up before reading it back
        // in a call: Discord uses the microphone, or (muted, deafened: it lets go of it) its Disconnect button is there;
        // right after hanging up only the button counts (the microphone takes a moment to be let go of)
        bool call = cmd == 3 ? uia && DiscordHasDisconnect(uia, w, hang) : DiscordInCall(pids);
        if (!call && g_dcCall && cmd != 3 && uia) call = DiscordHasDisconnect(uia, w, hang);
        bool was = g_dcCall;
        if (call && uia && (cmd || !was || ticks % 2 == 0)) ReadDiscordState(uia, w, toggles);
        g_dcCall = call;
        if (g_host && (call || was)) PostMessageW(g_host, WM_APP_DISCORD, 0, 0);
    }
    Release(toggles);
    Release(mute);
    Release(deaf);
    Release(hang);
    Release(uia);
    CoUninitialize();
    return 0;
}

static void StartDiscordWatch() {
    g_dcStop = false;
    g_dcWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_dcThread = CreateThread(nullptr, 0, DiscordWorker, nullptr, 0, nullptr);
}

static void StopDiscordWatch() {
    g_dcStop = true;
    if (g_dcWake) SetEvent(g_dcWake);
    if (g_dcThread) {
        WaitForSingleObject(g_dcThread, INFINITE);   // every call it makes is bounded: it always ends
        CloseHandle(g_dcThread);
    }
    if (g_dcWake) CloseHandle(g_dcWake);
    g_dcThread = g_dcWake = nullptr;
    g_dcCall = false;
    g_dcHost = -1;
}

// an item's picture: its app's icon, or (when Windows has none for it) its window's own
static std::wstring LauncherIconKey(const Item& it) {
    if (GetIcon(it.launch) || it.windows.empty()) return it.launch;
    wchar_t key[40];
    swprintf(key, 40, L"hwnd:%llx", (unsigned long long)(UINT_PTR)it.windows[0]);
    return key;
}

static void RenderLauncher() {
    if (!g_launcher) return;
    Surface& s = g_launcherS;
    int w = Px(LauncherW()), h = g_screen.bottom - g_screen.top - Px(PanelH());
    if (!s.Begin(w, h)) return;
    float W = (float)LauncherW(), H = (float)(h / g_scale);
    float a = g_set.opacity / 100.f;
    // while the Dash is open, the whole launcher takes the Dash's colour (one colour top to bottom), cross-faded in
    // step with the Dash; the Dash and the strip along the launcher draw the edge lines then
    float u = DashShown() ? g_aUnder.value : 0;
    s.rt->FillRectangle(D2D1::RectF(0, 0, W, H), s.Brush(Shade(g_tint, 0.55f), a));
    if (u > 0) {
        s.rt->FillRectangle(D2D1::RectF(0, 0, W, H), s.Brush(Shade(g_tint, 0.55f), a * u));   // hide the launcher colour...
        s.rt->FillRectangle(D2D1::RectF(0, 0, W, H), s.Brush(Shade(g_tint, 0.75f), a * u));  // ...for the Dash's
    }
    s.rt->FillRectangle(D2D1::RectF(W - 1, 0, W, H), s.White(0.18f * (1 - u)));   // the light edge Unity has on the right

    // Dash button: a tile in the shell's colour with the remix logo, with a springy press
    D2D1_RECT_F b = ItemRect(-1);
    float bx = (b.left + b.right) / 2, by = (b.top + b.bottom) / 2;
    float bh = Fx(kBfbFx).hover.value, press = (g_aBfb.running ? g_aBfb.value : 1) * HoverZoom(bh);
    s.rt->SetTransform(D2D1::Matrix3x2F::Scale(press, press, D2D1::Point2F(bx, by)));
    DrawTile(s, b, Shade(g_tint, 1.1f), g_dashOpen ? 1.f : bh);
    DrawRemixLogo(s, bx, by, g_set.iconSize * 0.68f, 1);
    s.rt->SetTransform(D2D1::Matrix3x2F::Identity());

    // Unity fades the launcher icons while the Dash is open (the Dash button stays bright)
    float dim = g_aDim.value <= 0 ? 1 : g_aDim.value;
    double now = NowMs();
    // the dragged icon goes last, on top of the others
    std::vector<int> order;
    for (int i = 0; i < VisibleItems(); i++)
        if (i != g_dragIndex) order.push_back(i);
    if (g_dragIndex >= 0 && g_dragIndex < VisibleItems()) order.push_back(g_dragIndex);
    for (int i : order) {
        auto& it = g_items[i];
        ItemFx& fx = Fx(it.launch);
        D2D1_RECT_F r = ItemRect(i);
        bool dragged = i == g_dragIndex;
        if (dragged) {   // it follows the pointer, see-through when dropping it would unpin it
            float top = g_dragY - g_set.iconSize / 2.f;
            r = D2D1::RectF(r.left, top, r.right, top + g_set.iconSize);
        } else {         // the others make room for it
            float dy = fx.shift.value * (float)ItemH();
            r.top += dy;
            r.bottom += dy;
        }
        float alpha = dim * (dragged ? (g_dragOut ? 0.35f : 0.9f) : 1.f);
        // starting up: pulse until its first window shows (at most 9 s)
        if (fx.pulseStart > 0) {
            double t = now - fx.pulseStart;
            if (!it.windows.empty() || t > 9000) fx.pulseStart = 0;
            else alpha *= 0.35f + 0.65f * (0.5f + 0.5f * cosf((float)(t / 900.0 * 6.2831853)));
        }
        float tcx = (r.left + r.right) / 2, tcy = (r.top + r.bottom) / 2, zoom = HoverZoom(fx.hover.value);
        auto tileZoom = D2D1::Matrix3x2F::Scale(zoom, zoom, D2D1::Point2F(tcx, tcy));
        s.rt->SetTransform(tileZoom);
        if (it.folder >= 0) DrawFolderTile(s, r, g_folders[it.folder], fx.hover.value, alpha);
        else {
            Icon* icon = GetIcon(LauncherIconKey(it));
            DrawTile(s, r, g_set.plainTiles ? Rgb{0.4f, 0.4f, 0.4f} : icon ? icon->average : Rgb{0.5f, 0.5f, 0.5f}, fx.hover.value, alpha);
        }
        if (i == g_dragOnto) {   // dropping here makes (or fills) a group
            auto ring = D2D1::RoundedRect(D2D1::RectF(r.left - 3, r.top - 3, r.right + 3, r.bottom + 3), 10, 10);
            s.rt->DrawRoundedRectangle(ring, s.White(0.9f), 2);
        }
        s.rt->SetTransform(D2D1::Matrix3x2F::Identity());
        if (it.folder < 0) {
            float pad = g_set.iconSize * 0.14f;
            float cx = tcx, cy = tcy;
            // pop (a new window) and wiggle (wants attention) move the icon only, like Unity
            float sc = fx.pop.value > 0 ? fx.pop.value : 1;
            float ang = fx.wiggle.running ? WiggleAngle(fx.wiggle.value) : 0;
            s.rt->SetTransform(D2D1::Matrix3x2F::Rotation(ang, D2D1::Point2F(cx, cy)) * D2D1::Matrix3x2F::Scale(sc, sc, D2D1::Point2F(cx, cy)) * tileZoom);
            DrawIcon(s, LauncherIconKey(it), D2D1::RectF(r.left + pad, r.top + pad, r.right - pad, r.bottom - pad), alpha);
            s.rt->SetTransform(D2D1::Matrix3x2F::Identity());
        }
        if (dragged) continue;
        // a pip per open window on the left (max 3), an arrow on the right for the focused app
        int n = std::min(3, (int)it.windows.size());
        float my = (r.top + r.bottom) / 2;
        for (int k = 0; k < n; k++) {
            float py = my + (k - (n - 1) / 2.f) * 7;
            Triangle(s, D2D1::Point2F(0, py - 3), D2D1::Point2F(4, py), D2D1::Point2F(0, py + 3), 0.95f * dim);
        }
        if (it.focused) Triangle(s, D2D1::Point2F(W - 1, my - 5), D2D1::Point2F(W - 7, my), D2D1::Point2F(W - 1, my + 5), 0.95f * dim);
    }

    DrawExtension(s);
    DrawDcExtension(s);

    // the trash at the bottom
    D2D1_RECT_F tr = ItemRect(kTrash);
    float th = Fx(kTrashFx).hover.value, tz = HoverZoom(th);
    s.rt->SetTransform(D2D1::Matrix3x2F::Scale(tz, tz, D2D1::Point2F((tr.left + tr.right) / 2, (tr.top + tr.bottom) / 2)));
    DrawTile(s, tr, {0.44f, 0.44f, 0.44f}, th, dim);
    float tpad = g_set.iconSize * 0.14f;
    DrawIcon(s, kTrashParsing, D2D1::RectF(tr.left + tpad, tr.top + tpad, tr.right - tpad, tr.bottom - tpad), dim);
    s.rt->SetTransform(D2D1::Matrix3x2F::Identity());
    s.End(g_screen.left - g_awayLauncher, g_screen.top + Px(PanelH()));
}

// ---------------- tooltips: Unity's dark bubble with an arrow pointing at the icon ----------------

static HWND g_tip;
static Surface g_tipS;
static int g_tipFor = -2;

static float TextWidth(const std::wstring& text, float size, DWRITE_FONT_WEIGHT weight) {
    IDWriteTextFormat* fmt = nullptr;
    IDWriteTextLayout* layout = nullptr;
    float w = 0;
    if (SUCCEEDED(g_dw->CreateTextFormat(g_fontFamily.c_str(), nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL, size, L"", &fmt)) &&
        SUCCEEDED(g_dw->CreateTextLayout(text.c_str(), (UINT32)text.size(), fmt, 4000, 100, &layout))) {
        DWRITE_TEXT_METRICS m;
        if (SUCCEEDED(layout->GetMetrics(&m))) w = m.widthIncludingTrailingWhitespace;
    }
    Release(layout);
    Release(fmt);
    return w;
}

static void HideTip() {
    g_tipFor = -2;
    if (g_tip) ShowWindow(g_tip, SW_HIDE);
}

static void ShowTip(int index) {
    if (!g_tip || index == g_tipFor) return;
    std::wstring text = index == -1 ? Tr(L"Zoek op je computer") : index == kTrash ? Tr(L"Prullenbak")
                      : index >= 0 && index < (int)g_items.size() ? g_items[index].name : L"";
    if (text.empty() || g_dragIndex >= 0) { HideTip(); return; }
    g_tipFor = index;
    float arrow = 7, tw = std::min(360.f, TextWidth(text, 13) + 22), th = 28, W = tw + arrow;
    Surface& s = g_tipS;
    if (!s.Begin(Px(W), Px(th))) return;
    if (auto* bubble = Path([&](ID2D1GeometrySink* k) {
            float r = 5, l = arrow + 0.5f, t = 0.5f, rr = W - 0.5f, b = th - 0.5f;
            k->BeginFigure(D2D1::Point2F(l, th / 2 - arrow), D2D1_FIGURE_BEGIN_FILLED);
            k->AddLine(D2D1::Point2F(0.5f, th / 2));
            k->AddLine(D2D1::Point2F(l, th / 2 + arrow));
            k->AddLine(D2D1::Point2F(l, b - r));
            k->AddArc(Arc(l + r, b, r, false));
            k->AddLine(D2D1::Point2F(rr - r, b));
            k->AddArc(Arc(rr, b - r, r, false));
            k->AddLine(D2D1::Point2F(rr, t + r));
            k->AddArc(Arc(rr - r, t, r, false));
            k->AddLine(D2D1::Point2F(l + r, t));
            k->AddArc(Arc(l, t + r, r, false));
            k->EndFigure(D2D1_FIGURE_END_CLOSED);
        })) {
        s.rt->FillGeometry(bubble, s.Brush(Shade(g_tint, 0.35f), 0.96f));
        s.rt->DrawGeometry(bubble, s.White(0.28f), 1);
        bubble->Release();
    }
    Text(s, text, D2D1::RectF(arrow + 10, 0, W - 10, th), 13, 0.97f);
    D2D1_RECT_F r = ItemRect(index);
    int y = g_screen.top + Px(PanelH()) + Px((r.top + r.bottom) / 2 - th / 2);
    s.End(g_screen.left + Px(LauncherW()) + Px(3), y);
    ShowWindow(g_tip, SW_SHOWNA);
}

// ---------------- window previews: live DWM thumbnails of an app's windows next to its icon (like the app) ----------------

static HWND g_preview;
static Surface g_previewS;
struct PreviewCard {
    HWND win;
    HTHUMBNAIL thumb = nullptr;
    D2D1_RECT_F card, slot, close;
};
static std::vector<PreviewCard> g_cards;
static int g_previewFor = -2, g_previewHover = -1;
static bool g_previewHoverClose;
static std::wstring g_previewLaunch;
static float g_previewW, g_previewH;
static const float kThumbW = 240, kThumbH = 150;

static bool PreviewShown() { return g_preview && IsWindowVisible(g_preview); }

static void HidePreview() {
    if (!g_preview) return;
    KillTimer(g_launcher, TIMER_PREVIEWSHOW);
    KillTimer(g_launcher, TIMER_PREVIEWHIDE);
    for (auto& c : g_cards)
        if (c.thumb) DwmUnregisterThumbnail(c.thumb);
    g_cards.clear();
    g_previewFor = -2;
    g_previewHover = -1;
    ShowWindow(g_preview, SW_HIDE);
}

static void PaintPreview() {
    Surface& s = g_previewS;
    if (!s.Begin(Px(g_previewW), Px(g_previewH))) return;
    s.rt->FillRectangle(D2D1::RectF(0, 0, g_previewW, g_previewH), s.Brush(Shade(g_tint, 0.42f), 1));
    s.rt->DrawRectangle(D2D1::RectF(0.5f, 0.5f, g_previewW - 0.5f, g_previewH - 0.5f), s.White(0.25f), 1);
    for (int i = 0; i < (int)g_cards.size(); i++) {
        auto& c = g_cards[i];
        bool hot = i == g_previewHover;
        if (hot) s.rt->FillRoundedRectangle(D2D1::RoundedRect(c.card, 6, 6), s.White(0.19f));
        float hx = c.card.left + 8, hy = c.card.top + 6;
        DrawIcon(s, g_previewLaunch, D2D1::RectF(hx, hy + 1, hx + 16, hy + 17), 1);
        Text(s, WindowTitle(c.win), D2D1::RectF(hx + 22, hy, (hot ? c.close.left : c.slot.right) - 4, hy + 18), 12, 0.97f);
        if (hot) {
            if (g_previewHoverClose) s.rt->FillRectangle(c.close, s.Brush({0.77f, 0.17f, 0.11f}, 1));
            Glyph(s, L"\uE8BB", c.close, 9, 1);
        }
        s.rt->FillRoundedRectangle(D2D1::RoundedRect(c.slot, 3, 3), s.Brush({0, 0, 0}, 0.25f));
        if (!c.thumb) {   // a minimized window has no live picture: its app's icon instead, like Windows
            float cx = (c.slot.left + c.slot.right) / 2, cy = (c.slot.top + c.slot.bottom) / 2;
            DrawIcon(s, g_previewLaunch, D2D1::RectF(cx - 24, cy - 24, cx + 24, cy + 24), 1);
        }
    }
    s.Finish();
    InvalidateRect(g_preview, nullptr, FALSE);
}

static void ShowPreview(int index) {
    if (!g_preview || index < 0 || index >= (int)g_items.size() || g_items[index].windows.empty() || g_dragIndex >= 0) { HidePreview(); return; }
    KillTimer(g_launcher, TIMER_PREVIEWHIDE);
    if (index == g_previewFor && PreviewShown()) return;
    for (auto& c : g_cards)
        if (c.thumb) DwmUnregisterThumbnail(c.thumb);
    g_cards.clear();
    HideTip();
    auto& it = g_items[index];
    g_previewFor = index;
    g_previewLaunch = it.launch;
    g_previewHover = -1;
    // cards in rows of at most five: a title row, then the picture
    float pad = 6, x = pad, y = pad, rowH = 0, right = 0;
    int col = 0;
    for (HWND w : it.windows) {
        PreviewCard c;
        c.win = w;
        float tw = kThumbW, th = kThumbH * 0.75f;
        if (IsIconic(w) || FAILED(DwmRegisterThumbnail(g_preview, w, &c.thumb))) c.thumb = nullptr;
        else {
            SIZE src = {};
            if (SUCCEEDED(DwmQueryThumbnailSourceSize(c.thumb, &src)) && src.cx > 0 && src.cy > 0) {
                float sw = (float)(src.cx / g_scale), sh = (float)(src.cy / g_scale), k = std::min(kThumbW / sw, kThumbH / sh);
                tw = std::max(80.f, sw * k);
                th = std::max(50.f, sh * k);
            }
        }
        if (col == 5) { x = pad; y += rowH; rowH = 0; col = 0; }
        float cw = tw + 20, ch = th + 42;
        c.card = D2D1::RectF(x + 2, y + 2, x + cw - 2, y + ch - 2);
        c.slot = D2D1::RectF(c.card.left + 8, c.card.top + 30, c.card.left + 8 + tw, c.card.top + 30 + th);
        c.close = D2D1::RectF(c.card.right - 30, c.card.top + 4, c.card.right - 8, c.card.top + 24);
        x += cw;
        rowH = std::max(rowH, ch);
        right = std::max(right, x);
        col++;
        g_cards.push_back(c);
    }
    g_previewW = right + pad;
    g_previewH = y + rowH + pad;
    // right of the launcher, centred on the icon, kept on the screen
    D2D1_RECT_F r = ItemRect(index);
    int left = g_screen.left + Px(LauncherW()) + Px(8);
    int top = g_screen.top + Px(PanelH()) + Px((r.top + r.bottom) / 2 - g_previewH / 2);
    top = std::max((int)g_screen.top + Px(PanelH()) + Px(4), std::min(top, (int)g_screen.bottom - Px(g_previewH) - Px(4)));
    PaintPreview();
    SetWindowPos(g_preview, HWND_TOPMOST, left, top, Px(g_previewW), Px(g_previewH), SWP_NOACTIVATE | SWP_SHOWWINDOW);
    for (auto& c : g_cards) {
        if (!c.thumb) continue;
        DWM_THUMBNAIL_PROPERTIES tp = {};
        tp.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;
        tp.rcDestination = {Px(c.slot.left), Px(c.slot.top), Px(c.slot.right), Px(c.slot.bottom)};
        tp.fVisible = TRUE;
        tp.fSourceClientAreaOnly = FALSE;
        DwmUpdateThumbnailProperties(c.thumb, &tp);
    }
}

static void ScheduleHidePreview() {
    if (PreviewShown()) SetTimer(g_launcher, TIMER_PREVIEWHIDE, 350, nullptr);
}

static void ShowDash(bool show);

static LRESULT CALLBACK PreviewProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            if (g_previewS.dc) BitBlt(dc, 0, 0, g_previewS.w, g_previewS.h, g_previewS.dc, 0, 0, SRCCOPY);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEMOVE: {
            KillTimer(g_launcher, TIMER_PREVIEWHIDE);
            float x = GET_X_LPARAM(lp) / (float)g_scale, y = GET_Y_LPARAM(lp) / (float)g_scale;
            int hover = -1;
            bool onClose = false;
            for (int i = 0; i < (int)g_cards.size(); i++)
                if (Inside(g_cards[i].card, x, y)) { hover = i; onClose = Inside(g_cards[i].close, x, y); }
            if (hover != g_previewHover || onClose != g_previewHoverClose) {
                g_previewHover = hover;
                g_previewHoverClose = onClose;
                PaintPreview();
            }
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }
        case WM_MOUSELEAVE:
            g_previewHover = -1;
            PaintPreview();
            ScheduleHidePreview();
            return 0;
        case WM_LBUTTONUP:
        case WM_MBUTTONUP: {
            if (g_previewHover < 0 || g_previewHover >= (int)g_cards.size()) return 0;
            HWND w = g_cards[g_previewHover].win;
            bool close = msg == WM_MBUTTONUP || g_previewHoverClose;
            HidePreview();
            if (close) PostMessageW(w, WM_CLOSE, 0, 0);
            else {
                if (g_dashOpen) ShowDash(false);
                Activate(w);
            }
            return 0;
        }
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------- menus: the app's own style (Windows 11-like: dark, rounded, an icon column, a soft hover) ----------------

static HWND g_menu;
static Surface g_menuS;
static std::vector<MenuEntry> g_menuItems;
static std::vector<D2D1_RECT_F> g_menuRects;
static int g_menuHover = -1, g_menuResult, g_menuX, g_menuY;
static bool g_menuDone;
static HWND g_menuLostTo;           // what took the focus from it (a click elsewhere)
static float g_menuW, g_menuH;
static const float kMenuShadow = 10;   // room for its drop shadow on the right and bottom

static bool Selectable(int i) {
    return i >= 0 && i < (int)g_menuItems.size() && g_menuItems[i].kind == MENU_ITEM && g_menuItems[i].enabled;
}

static void PaintMenu() {
    Surface& s = g_menuS;
    if (!s.Begin(Px(g_menuW), Px(g_menuH))) return;
    float bw = g_menuW - kMenuShadow, bh = g_menuH - kMenuShadow;
    for (int i = 7; i >= 1; i--)   // a soft shadow, a little down and to the right
        s.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(2, 3, bw + i, bh + i), 8.f + i, 8.f + i), s.Brush({0, 0, 0}, 0.055f));
    auto body = D2D1::RoundedRect(D2D1::RectF(0.5f, 0.5f, bw - 0.5f, bh - 0.5f), 8, 8);
    s.rt->FillRoundedRectangle(body, s.Brush({0x2B / 255.f, 0x2B / 255.f, 0x2B / 255.f}, 0xF4 / 255.f));
    s.rt->DrawRoundedRectangle(body, s.White(0x38 / 255.f), 1);
    for (int i = 0; i < (int)g_menuItems.size(); i++) {
        auto& e = g_menuItems[i];
        D2D1_RECT_F r = g_menuRects[i];
        if (e.kind == MENU_SEPARATOR) {
            float y = (r.top + r.bottom) / 2;
            s.rt->FillRectangle(D2D1::RectF(r.left + 4, y, r.right - 4, y + 1), s.White(0x24 / 255.f));
            continue;
        }
        if (i == g_menuHover && Selectable(i)) s.rt->FillRoundedRectangle(D2D1::RoundedRect(r, 4, 4), s.White(0x1F / 255.f));
        float alpha = e.kind == MENU_HEADING || e.enabled ? 1.f : 0x80 / 255.f;
        if (e.glyph) Text(s, e.glyph, D2D1::RectF(r.left + 10, r.top, r.left + 30, r.bottom), 15, alpha, DWRITE_TEXT_ALIGNMENT_LEADING, false, kGlyphs);
        Text(s, e.text, D2D1::RectF(r.left + 38, r.top, r.right - 24, r.bottom), 13, alpha, DWRITE_TEXT_ALIGNMENT_LEADING, false, nullptr,
             e.kind == MENU_HEADING ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL);
    }
    s.End(g_menuX, g_menuY);
}

static void ChooseMenu(int i) {
    if (!Selectable(i)) return;
    g_menuResult = g_menuItems[i].id;
    g_menuDone = true;
}

static LRESULT CALLBACK MenuProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE && !g_menuDone) { g_menuLostTo = (HWND)lp; g_menuDone = true; }
            return 0;
        case WM_MOUSEMOVE: {
            float x = GET_X_LPARAM(lp) / (float)g_scale, y = GET_Y_LPARAM(lp) / (float)g_scale;
            int hover = -1;
            for (int i = 0; i < (int)g_menuRects.size(); i++)
                if (Inside(g_menuRects[i], x, y) && Selectable(i)) hover = i;
            if (hover != g_menuHover) { g_menuHover = hover; PaintMenu(); }
            return 0;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP: {
            float x = GET_X_LPARAM(lp) / (float)g_scale, y = GET_Y_LPARAM(lp) / (float)g_scale;
            for (int i = 0; i < (int)g_menuRects.size(); i++)
                if (Inside(g_menuRects[i], x, y)) ChooseMenu(i);
            return 0;
        }
        case WM_KEYDOWN: {
            int n = (int)g_menuItems.size();
            if (wp == VK_ESCAPE) g_menuDone = true;
            else if (wp == VK_RETURN || wp == VK_SPACE) ChooseMenu(g_menuHover);
            else if ((wp == VK_DOWN || wp == VK_UP) && n > 0) {
                int step = wp == VK_DOWN ? 1 : -1, i = g_menuHover;
                for (int k = 0; k < n; k++) {
                    i = ((i < 0 && step < 0 ? 0 : i) + step + n) % n;
                    if (Selectable(i)) break;
                }
                if (Selectable(i)) { g_menuHover = i; PaintMenu(); }
            }
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// Shows a menu next to anchor (screen pixels) and waits for a choice, like TrackPopupMenu: the chosen id, or 0
static int ShowMenu(const std::vector<MenuEntry>& items, RECT anchor, int place) {
    if (!g_menu || IsWindowVisible(g_menu) || items.empty()) return 0;
    HideTip();
    HidePreview();
    g_menuItems = items;
    g_menuRects.clear();
    // size: the widest line, at least 230 wide; rows of 33, separators of 9
    float textW = 0;
    for (auto& e : items)
        if (e.kind != MENU_SEPARATOR) textW = std::max(textW, TextWidth(e.text, 13, e.kind == MENU_HEADING ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL));
    float bw = std::max(230.f, std::min(520.f, 10 + 28 + textW + 24 + 8)), y = 4;
    for (auto& e : items) {
        float h = e.kind == MENU_SEPARATOR ? 9.f : 33.f;
        g_menuRects.push_back(e.kind == MENU_SEPARATOR ? D2D1::RectF(4, y, bw - 4, y + h) : D2D1::RectF(4, y + 1, bw - 4, y + h - 1));
        y += h;
    }
    float bh = y + 4;
    g_menuW = bw + kMenuShadow;
    g_menuH = bh + kMenuShadow;
    // place it: right of the anchor (launcher), under it (panel) or at the pointer, kept on the screen
    int pw = Px(bw), ph = Px(bh);
    int x = place == MENU_RIGHT ? anchor.right + Px(4) : anchor.left, top = place == MENU_BELOW ? anchor.bottom : anchor.top;
    if (place == MENU_BELOW && x + pw > g_screen.right) x = anchor.right - pw;
    g_menuX = std::max((int)g_screen.left, std::min(x, (int)g_screen.right - pw));
    g_menuY = std::max((int)g_screen.top, std::min(top, (int)g_screen.bottom - ph));
    g_menuHover = -1;
    g_menuResult = 0;
    g_menuDone = false;
    g_menuLostTo = nullptr;
    PaintMenu();
    ShowWindow(g_menu, SW_SHOW);
    SetForegroundWindow(g_menu);
    SetFocus(g_menu);
    // its own little message loop until something is chosen or it loses the focus
    MSG m;
    while (!g_menuDone) {
        BOOL got = GetMessageW(&m, nullptr, 0, 0);
        if (got <= 0) {
            if (got == 0) PostQuitMessage((int)m.wParam);
            break;
        }
        bool click = m.message == WM_LBUTTONDOWN || m.message == WM_RBUTTONDOWN || m.message == WM_MBUTTONDOWN ||
                     m.message == WM_NCLBUTTONDOWN || m.message == WM_NCRBUTTONDOWN;
        if (click && m.hwnd != g_menu) break;   // a click on another of the shell's windows closes it
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }
    g_menuDone = true;
    ShowWindow(g_menu, SW_HIDE);
    // an open Dash gets the focus back, unless the menu was closed by clicking somewhere else
    if (g_dashOpen) {
        bool elsewhere = g_menuLostTo && GetAncestor(g_menuLostTo, GA_ROOT) != g_dash;
        if (elsewhere) ShowDash(false);
        else {
            SetForegroundWindow(g_dash);
            SetFocus(g_dash);
        }
    }
    return g_menuResult;
}

// ---------------- the panel's popups (the app's): the sound slider under the volume icon, a calendar under the clock ----------------

// ---------------- the apps' tray icons (Steam, Discord, ...: what runs in the background). Apps hand them to
// Explorer's taskbar window (Shell_NotifyIcon arrives there as WM_COPYDATA); the shell listens in on that window
// and shows them in a popup under the panel's arrow. A click goes to the app just like a click on the taskbar ----------------

struct TrayIcon {
    HWND hwnd = nullptr;
    UINT id = 0, callback = 0, version = 0;
    GUID guid = {};
    bool hasGuid = false, hidden = false;
    HICON icon = nullptr;   // our own copy
    int serial = 0;         // a new number for each new picture
    std::wstring tip;
};
static std::vector<TrayIcon> g_trayIcons;
static std::mutex g_trayLock;
static bool g_trayLockReady;
static int g_traySerial;
static const UINT WM_APP_TRAY = WM_APP + 63;   // the tray icons changed

// what Shell_NotifyIcon sends, with handles cut to 32 bits so 32- and 64-bit apps send the same
struct TrayNid32 {
    DWORD cbSize, hWnd;
    UINT uID, uFlags, uCallbackMessage;
    DWORD hIcon;
    WCHAR szTip[128];
    DWORD dwState, dwStateMask;
    WCHAR szInfo[256];
    UINT uVersion;
    WCHAR szInfoTitle[64];
    DWORD dwInfoFlags;
    GUID guidItem;
    DWORD hBalloonIcon;
};
struct TrayData {
    DWORD signature, message;
    TrayNid32 nid;
};

// on Explorer's taskbar thread: keep our list in step (Explorer itself handles the message as always)
static void TrayNotify(const COPYDATASTRUCT* cds) {
    if (!g_trayLockReady || !cds || cds->dwData != 1 || cds->cbData < offsetof(TrayData, nid) + offsetof(TrayNid32, szTip)) return;
    const TrayData* d = (const TrayData*)cds->lpData;
    size_t have = cds->cbData - offsetof(TrayData, nid);   // old apps send a shorter structure
    const TrayNid32& n = d->nid;
    bool byGuid = (n.uFlags & NIF_GUID) && have >= offsetof(TrayNid32, guidItem) + sizeof(GUID);
    HWND hwnd = (HWND)(LONG_PTR)(LONG)n.hWnd;
    g_trayLock.lock();
    auto it = std::find_if(g_trayIcons.begin(), g_trayIcons.end(), [&](const TrayIcon& t) {
        return byGuid ? t.hasGuid && IsEqualGUID(t.guid, n.guidItem) : !t.hasGuid && t.hwnd == hwnd && t.id == n.uID;
    });
    bool changed = false;
    if (d->message == NIM_DELETE) {
        if (it != g_trayIcons.end()) {
            if (it->icon) DestroyIcon(it->icon);
            g_trayIcons.erase(it);
            changed = true;
        }
    } else if (d->message == NIM_ADD || d->message == NIM_MODIFY || d->message == NIM_SETVERSION) {
        if (it == g_trayIcons.end()) {   // (an icon from before the shell listened arrives as a change: new to us)
            TrayIcon t;
            t.hasGuid = byGuid;
            if (byGuid) t.guid = n.guidItem;
            t.id = n.uID;
            g_trayIcons.push_back(t);
            it = g_trayIcons.end() - 1;
        }
        TrayIcon& t = *it;
        if (n.hWnd) t.hwnd = hwnd;
        if (d->message == NIM_SETVERSION) {
            if (have >= offsetof(TrayNid32, uVersion) + sizeof(UINT)) t.version = n.uVersion;
        } else {
            if (n.uFlags & NIF_MESSAGE) t.callback = n.uCallbackMessage;
            if ((n.uFlags & NIF_ICON) && n.hIcon) {
                HICON copy = CopyIcon((HICON)(LONG_PTR)(LONG)n.hIcon);
                if (copy) {
                    if (t.icon) DestroyIcon(t.icon);
                    t.icon = copy;
                    t.serial = ++g_traySerial;
                }
            }
            if (n.uFlags & NIF_TIP) {
                size_t max = have >= offsetof(TrayNid32, dwState) ? 128 : 64;
                t.tip.assign(n.szTip, wcsnlen(n.szTip, max));
            }
            if ((n.uFlags & NIF_STATE) && have >= offsetof(TrayNid32, szInfo) && (n.dwStateMask & NIS_HIDDEN)) t.hidden = (n.dwState & NIS_HIDDEN) != 0;
        }
        changed = true;
    }
    g_trayLock.unlock();
    if (changed && g_host) PostMessageW(g_host, WM_APP_TRAY, 0, 0);
}

// the icons to show (still there, not hidden), oldest first
static std::vector<TrayIcon> TrayIcons() {
    std::vector<TrayIcon> list;
    if (!g_trayLockReady) return list;
    g_trayLock.lock();
    for (auto it = g_trayIcons.begin(); it != g_trayIcons.end();) {
        if (!IsWindow(it->hwnd)) {   // its app is gone
            if (it->icon) DestroyIcon(it->icon);
            it = g_trayIcons.erase(it);
            continue;
        }
        if (!it->hidden && it->icon) list.push_back(*it);
        ++it;
    }
    g_trayLock.unlock();
    return list;
}

static void StartTrayWatch() {
    if (!g_trayLockReady) {
        g_trayLockReady = true;
    }
}

static bool g_unloading;   // the mod is being turned off (not just restarted for a settings change)
static void StopTrayWatch() {
    if (!g_trayLockReady || !g_unloading) return;   // a restart keeps the list (the apps aren't asked again)
    g_trayLock.lock();
    for (auto& t : g_trayIcons)
        if (t.icon) DestroyIcon(t.icon);
    g_trayIcons.clear();
    g_trayLock.unlock();
}

// a click on an icon, as the taskbar sends it: version 4 apps get the event and the spot, older ones the id
static void TrayClick(const TrayIcon& t, bool right) {
    if (!IsWindow(t.hwnd) || !t.callback) return;
    DWORD pid = 0;
    GetWindowThreadProcessId(t.hwnd, &pid);
    AllowSetForegroundWindow(pid);   // so its window or menu can come to the front
    POINT p;
    GetCursorPos(&p);
    auto send = [&](UINT event) {
        if (t.version >= 4) PostMessageW(t.hwnd, t.callback, MAKEWPARAM(p.x, p.y), MAKELPARAM(event, t.id));
        else PostMessageW(t.hwnd, t.callback, t.id, event);
    };
    if (right) {
        send(WM_RBUTTONDOWN);
        send(WM_RBUTTONUP);
        if (t.version >= 3) send(WM_CONTEXTMENU);
    } else {
        send(WM_LBUTTONDOWN);
        send(WM_LBUTTONUP);
        if (t.version >= 3) send(NIN_SELECT);
    }
}

// an icon's picture, scaled once with a high-quality filter to exactly px
static ID2D1Bitmap* TrayBitmap(Surface& s, const TrayIcon& t, int px) {
    std::wstring key = L"tray#" + std::to_wstring(t.serial) + L"@" + std::to_wstring(px);
    auto it = s.bitmaps.find(key);
    if (it != s.bitmaps.end()) return it->second;
    ID2D1Bitmap* bmp = nullptr;
    IWICBitmap* src = nullptr;
    IWICFormatConverter* conv = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    IWICBitmap* done = nullptr;
    if (t.icon && SUCCEEDED(g_wic->CreateBitmapFromHICON(t.icon, &src)) && SUCCEEDED(g_wic->CreateFormatConverter(&conv)) &&
        SUCCEEDED(conv->Initialize(src, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom)) &&
        SUCCEEDED(g_wic->CreateBitmapScaler(&scaler)) &&
        (SUCCEEDED(scaler->Initialize(conv, px, px, (WICBitmapInterpolationMode)4 /* HighQualityCubic */)) ||
         SUCCEEDED(scaler->Initialize(conv, px, px, WICBitmapInterpolationModeFant))) &&
        SUCCEEDED(g_wic->CreateBitmapFromSource(scaler, WICBitmapCacheOnLoad, &done)))
        s.rt->CreateBitmapFromWicBitmap(done, nullptr, &bmp);
    Release(done);
    Release(scaler);
    Release(conv);
    Release(src);
    s.bitmaps[key] = bmp;
    return bmp;
}

enum { PH_MUTE = 1, PH_SLIDER, PH_SOUND, PH_PREV, PH_NEXT, PH_DATETIME, PH_TRAY = 100 };
static std::vector<TrayIcon> g_popTray;   // the tray popup's icons (as when it opened)
static int g_popTrayClick = -1;
static bool g_popTrayRight;
static HWND g_pop;
static Surface g_popS;
static int g_popKind, g_popX, g_popY, g_popHover = -1, g_calYear, g_calMonth;
static bool g_popDone, g_popSliding;
static float g_popW, g_popH;
static std::vector<Hot> g_popHot;
static D2D1_RECT_F g_popTrack;   // the volume slider's track

static void SetVolume(float level) {
    if (auto* v = Endpoint()) {
        v->SetMasterVolumeLevelScalar(std::clamp(level, 0.f, 1.f), nullptr);
        if (level > 0 && g_muted) v->SetMute(FALSE, nullptr);
        v->Release();
    }
    ReadVolume();
}

static void ToggleMute() {
    if (auto* v = Endpoint()) {
        v->SetMute(g_muted ? FALSE : TRUE, nullptr);
        v->Release();
    }
    ReadVolume();
}

static int DaysIn(int year, int month) {
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    return month == 2 && leap ? 29 : days[month - 1];
}

static int WeekdayOf(int year, int month, int day) {   // 0 = Sunday
    SYSTEMTIME t = {};
    t.wYear = (WORD)year;
    t.wMonth = (WORD)month;
    t.wDay = (WORD)day;
    FILETIME f;
    SystemTimeToFileTime(&t, &f);
    FileTimeToSystemTime(&f, &t);
    return t.wDayOfWeek;
}

static std::wstring DateText(const SYSTEMTIME& t, const wchar_t* format) {
    wchar_t buf[96] = {};
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &t, format, buf, 96, nullptr);
    return buf;
}

static void PaintPopup() {
    Surface& s = g_popS;
    if (!s.Begin(Px(g_popW), Px(g_popH))) return;
    float W = g_popW, H = g_popH;
    // the app's box: dark, a light rim, square at the top (it hangs from the panel), rounded at the bottom
    if (auto* box = Path([&](ID2D1GeometrySink* k) {
            float r = 5, l = 0.5f, t = 0.5f, rr = W - 0.5f, b = H - 0.5f;
            k->BeginFigure(D2D1::Point2F(l, t), D2D1_FIGURE_BEGIN_FILLED);
            k->AddLine(D2D1::Point2F(rr, t));
            k->AddLine(D2D1::Point2F(rr, b - r));
            k->AddArc(Arc(rr - r, b, r, true));
            k->AddLine(D2D1::Point2F(l + r, b));
            k->AddArc(Arc(l, b - r, r, true));
            k->EndFigure(D2D1_FIGURE_END_CLOSED);
        })) {
        s.rt->FillGeometry(box, s.Brush({0x28 / 255.f, 0x26 / 255.f, 0x24 / 255.f}, 0xF2 / 255.f));
        s.rt->DrawGeometry(box, s.White(0x50 / 255.f), 1);
        box->Release();
    }
    g_popHot.clear();
    float x0 = 10, cw = W - 20, y = 10;
    auto link = [&](int id, const std::wstring& text) {   // a line you click, lit in the accent colour under the pointer
        D2D1_RECT_F r = D2D1::RectF(x0, y, x0 + cw, y + 27);
        g_popHot.push_back({r, id});
        if (g_popHover == id) s.rt->FillRectangle(r, s.Brush(g_accent, 1));
        Text(s, text, D2D1::RectF(r.left + 6, r.top, r.right - 6, r.bottom), 13, 1);
        y += 27;
    };
    auto line = [&]() {
        y += 2;
        s.rt->FillRectangle(D2D1::RectF(x0, y, x0 + cw, y + 1), s.White(0x40 / 255.f));
        y += 5;
    };
    if (g_popKind == POP_VOLUME) {
        link(PH_MUTE, g_muted ? L"Unmute" : L"Mute");
        y += 8;
        float cy = y + 10;
        Glyph(s, L"\uE993", D2D1::RectF(x0 + 6, y, x0 + 22, y + 20), 14, 1);
        Glyph(s, L"\uE995", D2D1::RectF(x0 + cw - 22, y, x0 + cw - 6, y + 20), 14, 1);
        g_popTrack = D2D1::RectF(x0 + 30, cy - 2, x0 + cw - 30, cy + 2);
        g_popHot.push_back({D2D1::RectF(x0 + 22, y, x0 + cw - 22, y + 20), PH_SLIDER});
        float level = g_muted ? 0 : g_volume, fill = g_popTrack.left + (g_popTrack.right - g_popTrack.left) * level;
        s.rt->FillRoundedRectangle(D2D1::RoundedRect(g_popTrack, 2, 2), s.White(0x55 / 255.f));
        s.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(g_popTrack.left, g_popTrack.top, fill, g_popTrack.bottom), 2, 2), s.Brush(g_accent, 1));
        s.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(fill, cy), 8, 8), s.White(1));
        s.rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(fill, cy), 8, 8), s.Brush({0, 0, 0}, 0x40 / 255.f), 1);
        y += 28;
        line();
        link(PH_SOUND, L"Sound Settings…");
    } else if (g_popKind == POP_TRAY) {
        // a grid of the icons, the hovered one's name underneath
        int cols = std::clamp((int)g_popTray.size(), 1, 6);
        for (int i = 0; i < (int)g_popTray.size(); i++) {
            D2D1_RECT_F r = D2D1::RectF(x0 + (i % cols) * 36, y + (i / cols) * 36, x0 + (i % cols) * 36 + 34, y + (i / cols) * 36 + 34);
            g_popHot.push_back({r, PH_TRAY + i});
            if (g_popHover == PH_TRAY + i) s.rt->FillRoundedRectangle(D2D1::RoundedRect(r, 4, 4), s.White(0x30 / 255.f));
            int px = Px(20);
            if (ID2D1Bitmap* bmp = TrayBitmap(s, g_popTray[i], px)) {
                float iw = px / (float)g_scale;
                float ix = std::round(((r.left + r.right) / 2 - iw / 2) * (float)g_scale) / (float)g_scale, iy = std::round(((r.top + r.bottom) / 2 - iw / 2) * (float)g_scale) / (float)g_scale;
                s.rt->DrawBitmap(bmp, D2D1::RectF(ix, iy, ix + iw, iy + iw), 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
        }
        if (g_popTray.empty()) Text(s, L"No apps in the background", D2D1::RectF(x0 + 6, y, x0 + cw, y + 26), 13, 0.7f);
        else {
            y += ((int)g_popTray.size() + cols - 1) / cols * 36 + 2;
            line();
            int h = g_popHover - PH_TRAY;
            std::wstring tip = h >= 0 && h < (int)g_popTray.size() ? g_popTray[h].tip : L"Click: open · right-click: menu";
            size_t nl = tip.find_first_of(L"\r\n");   // the first line of it
            if (nl != std::wstring::npos) tip.resize(nl);
            Text(s, tip, D2D1::RectF(x0 + 6, y, x0 + cw - 4, y + 22), 12, h >= 0 ? 1.f : 0.6f);
        }
    } else {
        SYSTEMTIME now;
        GetLocalTime(&now);
        Text(s, DateText(now, L"dddd d MMMM yyyy"), D2D1::RectF(x0 + 6, y + 2, x0 + cw, y + 20), 13, 1);
        y += 28;
        // the month: arrows either side of its name, the weekdays, then six weeks of days with today marked
        SYSTEMTIME first = {};
        first.wYear = (WORD)g_calYear;
        first.wMonth = (WORD)g_calMonth;
        first.wDay = 1;
        D2D1_RECT_F prev = D2D1::RectF(x0, y, x0 + 28, y + 28), next = D2D1::RectF(x0 + cw - 28, y, x0 + cw, y + 28);
        g_popHot.push_back({prev, PH_PREV});
        g_popHot.push_back({next, PH_NEXT});
        if (g_popHover == PH_PREV) s.rt->FillRoundedRectangle(D2D1::RoundedRect(prev, 4, 4), s.White(0x26 / 255.f));
        if (g_popHover == PH_NEXT) s.rt->FillRoundedRectangle(D2D1::RoundedRect(next, 4, 4), s.White(0x26 / 255.f));
        Glyph(s, L"\uE76B", prev, 11, 1);
        Glyph(s, L"\uE76C", next, 11, 1);
        Text(s, DateText(first, L"MMMM yyyy"), D2D1::RectF(x0 + 28, y, x0 + cw - 28, y + 28), 14, 1, DWRITE_TEXT_ALIGNMENT_CENTER, false, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD);
        y += 32;
        wchar_t firstDay[4] = L"0";   // 0 = Monday ... 6 = Sunday
        GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, firstDay, 4);
        int start = (firstDay[0] - L'0' + 1) % 7;   // as 0 = Sunday
        float cell = cw / 7;
        for (int c = 0; c < 7; c++) {
            wchar_t name[16] = {};
            int dow = (start + c) % 7;   // LOCALE_SSHORTESTDAYNAME1 is Monday
            GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SSHORTESTDAYNAME1 + (dow + 6) % 7, name, 16);
            Text(s, name, D2D1::RectF(x0 + c * cell, y, x0 + (c + 1) * cell, y + 22), 11, 0.7f, DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        y += 22;
        int lead = (WeekdayOf(g_calYear, g_calMonth, 1) - start + 7) % 7;
        int pm = g_calMonth == 1 ? 12 : g_calMonth - 1, py = g_calMonth == 1 ? g_calYear - 1 : g_calYear, prevDays = DaysIn(py, pm);
        int days = DaysIn(g_calYear, g_calMonth);
        for (int i = 0; i < 42; i++) {
            int day = i - lead + 1;
            bool inMonth = day >= 1 && day <= days;
            int shown = inMonth ? day : day < 1 ? prevDays + day : day - days;
            D2D1_RECT_F r = D2D1::RectF(x0 + (i % 7) * cell, y + (i / 7) * 28, x0 + (i % 7 + 1) * cell, y + (i / 7 + 1) * 28);
            bool today = inMonth && g_calYear == now.wYear && g_calMonth == now.wMonth && day == now.wDay;
            if (today) s.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + 2, r.top + 2, r.right - 2, r.bottom - 2), 4, 4), s.Brush(g_accent, 1));
            Text(s, std::to_wstring(shown), r, 12, inMonth ? 1.f : 0.4f, DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        y += 6 * 28 + 6;
        line();
        link(PH_DATETIME, L"Time & Date Settings…");
    }
    s.End(g_popX, g_popY);
}

static void SlideVolume(float x) {
    float t = (x - g_popTrack.left) / std::max(1.f, g_popTrack.right - g_popTrack.left);
    SetVolume(std::clamp(t, 0.f, 1.f));
    PaintPopup();
    RenderPanel();
}

static LRESULT CALLBACK PopupProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    float x = GET_X_LPARAM(lp) / (float)g_scale, y = GET_Y_LPARAM(lp) / (float)g_scale;
    auto hotAt = [&]() {
        for (auto& h : g_popHot)
            if (Inside(h.r, x, y)) return h.id;
        return -1;
    };
    switch (msg) {
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE) g_popDone = true;
            return 0;
        case WM_MOUSEMOVE:
            if (g_popSliding) { SlideVolume(x); return 0; }
            if (hotAt() != g_popHover) { g_popHover = hotAt(); PaintPopup(); }
            return 0;
        case WM_LBUTTONDOWN:
            if (hotAt() == PH_SLIDER) {
                g_popSliding = true;
                SetCapture(hwnd);
                SlideVolume(x);
            }
            return 0;
        case WM_LBUTTONUP: {
            if (g_popSliding) {
                g_popSliding = false;
                ReleaseCapture();
                return 0;
            }
            int id = hotAt();
            if (id >= PH_TRAY) { g_popTrayClick = id - PH_TRAY; g_popTrayRight = false; g_popDone = true; }
            else if (id == PH_MUTE) { ToggleMute(); RenderPanel(); g_popDone = true; }
            else if (id == PH_SOUND) { g_popDone = true; Launch(L"ms-settings:sound"); }
            else if (id == PH_DATETIME) { g_popDone = true; Launch(L"ms-settings:dateandtime"); }
            else if (id == PH_PREV || id == PH_NEXT) {
                g_calMonth += id == PH_NEXT ? 1 : -1;
                if (g_calMonth < 1) { g_calMonth = 12; g_calYear--; }
                if (g_calMonth > 12) { g_calMonth = 1; g_calYear++; }
                PaintPopup();
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            int id = hotAt();
            if (id >= PH_TRAY) { g_popTrayClick = id - PH_TRAY; g_popTrayRight = true; g_popDone = true; }
            return 0;
        }
        case WM_MOUSEWHEEL:
            if (g_popKind == POP_VOLUME) {
                ChangeVolume(GET_WHEEL_DELTA_WPARAM(wp) > 0 ? 0.05f : -0.05f);
                PaintPopup();
                RenderPanel();
            }
            return 0;
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE) g_popDone = true;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// opens a popup under an indicator (screen pixels) and stays until it loses the focus or something is chosen
static void ShowPopup(int kind, RECT anchor) {
    if (!g_pop || IsWindowVisible(g_pop)) return;
    HideTip();
    HidePreview();
    g_popKind = kind;
    ReadVolume();
    SYSTEMTIME now;
    GetLocalTime(&now);
    g_calYear = now.wYear;
    g_calMonth = now.wMonth;
    g_popW = kind == POP_VOLUME ? 270.f : 244.f;
    g_popH = kind == POP_VOLUME ? 118.f : 311.f;
    g_popTrayClick = -1;
    if (kind == POP_TRAY) {
        g_popTray = TrayIcons();
        int n = (int)g_popTray.size(), cols = std::clamp(n, 1, 6), rows = std::max(1, (n + cols - 1) / cols);
        g_popW = std::max(240.f, 20.f + cols * 36);
        g_popH = n ? 10 + rows * 36 + 2 + 7 + 22 + 10 : 46;
    }
    int pw = Px(g_popW);
    g_popX = anchor.left + pw > g_screen.right ? anchor.right - pw : anchor.left;
    g_popY = anchor.bottom;
    g_popHover = -1;
    g_popDone = g_popSliding = false;
    PaintPopup();
    ShowWindow(g_pop, SW_SHOW);
    SetForegroundWindow(g_pop);
    SetFocus(g_pop);
    MSG m;
    while (!g_popDone) {
        BOOL got = GetMessageW(&m, nullptr, 0, 0);
        if (got <= 0) {
            if (got == 0) PostQuitMessage((int)m.wParam);
            break;
        }
        bool click = m.message == WM_LBUTTONDOWN || m.message == WM_RBUTTONDOWN || m.message == WM_NCLBUTTONDOWN;
        if (click && m.hwnd != g_pop) break;   // a click on another of the shell's windows (the indicator again) closes it
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }
    g_popDone = true;
    if (g_popSliding) { g_popSliding = false; ReleaseCapture(); }
    ShowWindow(g_pop, SW_HIDE);
    if (kind == POP_TRAY && g_popTrayClick >= 0 && g_popTrayClick < (int)g_popTray.size()) TrayClick(g_popTray[g_popTrayClick], g_popTrayRight);
}

// the keyboard language of the window in front, as Unity shows it ("En", "Nl")
static std::wstring KeyboardLanguage() {
    HKL layout = GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), nullptr));
    wchar_t name[LOCALE_NAME_MAX_LENGTH] = {}, two[16] = {};
    if (!LCIDToLocaleName(MAKELCID(LOWORD((UINT_PTR)layout), SORT_DEFAULT), name, LOCALE_NAME_MAX_LENGTH, 0)) return L"En";
    GetLocaleInfoEx(name, LOCALE_SISO639LANGNAME, two, 16);
    std::wstring t = two[0] ? two : L"en";
    t[0] = towupper(t[0]);
    return t;
}

// hover fades a tile up (quick) and down (a bit slower), like the app's colour animation
static void SetLauncherHover(int hover) {
    auto anim = [](int i, bool on) {
        std::wstring key = i == -1 ? kBfbFx : i == kTrash ? kTrashFx : i >= 0 && i < (int)g_items.size() ? g_items[i].launch : L"";
        if (!key.empty()) Fx(key).hover.Go(on ? 1.f : 0.f, on ? 90.f : 160.f, EASE_OUT_CUBIC);
    };
    anim(g_launcherHover, false);
    anim(hover, true);
    g_launcherHover = hover;
    // the name next to the icon, and its windows' previews after a moment (at once while previews are showing)
    bool windows = hover >= 0 && hover < (int)g_items.size() && !g_items[hover].windows.empty() && g_items[hover].folder < 0;
    KillTimer(g_launcher, TIMER_PREVIEWSHOW);
    if (PreviewShown() && windows) ShowPreview(hover);
    else {
        ShowTip(hover);
        if (PreviewShown()) ScheduleHidePreview();
        else if (windows) SetTimer(g_launcher, TIMER_PREVIEWSHOW, 450, nullptr);
    }
    KickAnim();
    RenderLauncher();
}

// dropping a dragged icon: a new place among the pins, or unpinned when dropped off the launcher
static void DropItem() {
    int from = g_dragIndex, to = g_dragTarget, onto = g_dragOnto;
    bool out = g_dragOut;
    g_dragIndex = -2;
    g_dragOnto = -1;
    for (auto& kv : g_fx) kv.second.shift.Set(0);
    if (!out && onto >= 0 && from >= 0 && from < (int)g_items.size() && onto < (int)g_items.size()) {
        MakeGroup(from, onto);
        return;
    }
    if (from >= 0 && from < (int)g_items.size()) {
        Item moved = g_items[from];
        if (out) {
            if (moved.pinned) Pin(moved.launch, false);
        } else if (to != from) {
            std::vector<Item> order = g_items;
            order.erase(order.begin() + from);
            to = std::clamp(to, 0, (int)order.size());
            order.insert(order.begin() + to, moved);
            // a running app dropped above a pinned one is pinned there
            for (int k = to + 1; k < (int)order.size(); k++)
                if (order[k].pinned) order[to].pinned = true;
            g_pins.clear();
            for (auto& it : order)
                if (it.pinned) g_pins.push_back(it.launch);
            SavePins();
        }
    }
    BuildItems();
    RenderLauncher();
}

static void CancelDrag() {
    g_dragIndex = -2;
    g_dragOnto = -1;
    for (auto& kv : g_fx) kv.second.shift.Set(0);
    RenderLauncher();
}

static void ToggleDash();
static void ShowDash(bool show);
static void RecordRecent(const std::wstring& launch);

static std::wstring Trim(const std::wstring& s);
// ---------------- groups (the app's, iOS-like): drag an app onto another to make a group; a click opens a panel
// with its apps, zooming out of the icon ----------------

static std::wstring FileOf(const std::wstring& launch);
static void ShowInFolder(const std::wstring& file);
static std::wstring GroupNameFor(const std::wstring& a, const std::wstring& b);
static const UINT WM_APP_OPENGROUP = WM_APP + 60;   // to the launcher: open group (wParam: item)

static void GroupsChanged() {
    SaveFolders();
    SavePins();
    BuildItems();
    RenderLauncher();
}

static std::wstring NewFolderId() {
    wchar_t id[16];
    swprintf(id, 16, L"%08x", (unsigned)(GetTickCount() * 2654435761u) ^ (unsigned)g_folders.size());
    return id;
}

// a group with one app left turns back into that app; an empty one disappears
static void NormalizeFolder(int fi) {
    PinGroup& f = g_folders[fi];
    if (f.apps.size() > 1) return;
    auto at = std::find(g_pins.begin(), g_pins.end(), L"folder:" + f.id);
    if (at != g_pins.end()) {
        if (f.apps.size() == 1 && !IsPinned(f.apps[0])) *at = f.apps[0];
        else g_pins.erase(at);
    }
    g_folders.erase(g_folders.begin() + fi);
}

// dropped dragged onto onto: into that group, or a new group out of the two apps (it opens right away, like iOS)
static void MakeGroup(int from, int onto) {
    Item dragged = g_items[from], target = g_items[onto];
    if (dragged.folder >= 0) return;   // no groups inside groups
    std::wstring key;
    if (target.folder >= 0) {
        PinGroup& f = g_folders[target.folder];
        if (std::find(f.apps.begin(), f.apps.end(), dragged.launch) == f.apps.end()) f.apps.push_back(dragged.launch);
        key = L"folder:" + f.id;
    } else {
        PinGroup f;
        f.id = NewFolderId();
        f.name = GroupNameFor(target.launch, dragged.launch);
        f.apps = {target.launch, dragged.launch};
        g_folders.push_back(f);
        key = L"folder:" + f.id;
        auto at = std::find(g_pins.begin(), g_pins.end(), target.launch);
        if (at != g_pins.end()) *at = key;
        else {   // a running app: the group goes where it stood among the pins
            int before = 0;
            for (int i = 0; i < onto; i++)
                if (g_items[i].pinned) before++;
            g_pins.insert(g_pins.begin() + std::min(before, (int)g_pins.size()), key);
        }
    }
    if (dragged.pinned) g_pins.erase(std::remove(g_pins.begin(), g_pins.end(), dragged.launch), g_pins.end());
    GroupsChanged();
    for (int i = 0; i < (int)g_items.size(); i++)
        if (g_items[i].launch == key) PostMessageW(g_launcher, WM_APP_OPENGROUP, i, 0);
}

static void RemoveFromGroup(int fi, const std::wstring& member) {
    PinGroup& f = g_folders[fi];
    f.apps.erase(std::remove(f.apps.begin(), f.apps.end(), member), f.apps.end());
    auto at = std::find(g_pins.begin(), g_pins.end(), L"folder:" + f.id);
    if (!IsPinned(member)) g_pins.insert(at != g_pins.end() ? at + 1 : g_pins.end(), member);
    NormalizeFolder(fi);
    GroupsChanged();
}

static void Ungroup(int fi) {   // its apps go back on the launcher where the group was
    PinGroup f = g_folders[fi];
    auto at = std::find(g_pins.begin(), g_pins.end(), L"folder:" + f.id);
    size_t pos = at != g_pins.end() ? at - g_pins.begin() : g_pins.size();
    if (at != g_pins.end()) g_pins.erase(at);
    std::vector<std::wstring> back;
    for (auto& a : f.apps)
        if (!IsPinned(a)) back.push_back(a);
    g_pins.insert(g_pins.begin() + std::min(pos, g_pins.size()), back.begin(), back.end());
    g_folders.erase(g_folders.begin() + fi);
    GroupsChanged();
}

static void RemoveGroup(int fi) {
    std::wstring key = L"folder:" + g_folders[fi].id;
    g_pins.erase(std::remove(g_pins.begin(), g_pins.end(), key), g_pins.end());
    g_folders.erase(g_folders.begin() + fi);
    GroupsChanged();
}

// the group's tile: frosted, with its first four apps as small icons
static void DrawFolderTile(Surface& s, D2D1_RECT_F r, const PinGroup& f, float hover, float alpha) {
    float rad = 8;
    auto rr = D2D1::RoundedRect(r, rad, rad);
    ID2D1GradientStopCollection* stops = nullptr;
    D2D1_GRADIENT_STOP gs[2] = {{0, D2D1::ColorF(1, 1, 1, (0x55 / 255.f + 0.08f * hover) * alpha)}, {1, D2D1::ColorF(1, 1, 1, (0x22 / 255.f + 0.05f * hover) * alpha)}};
    s.rt->CreateGradientStopCollection(gs, 2, &stops);
    ID2D1LinearGradientBrush* grad = nullptr;
    if (stops) s.rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.left, r.bottom)), stops, &grad);
    if (grad) s.rt->FillRoundedRectangle(rr, grad);
    Release(grad);
    Release(stops);
    s.rt->DrawRoundedRectangle(rr, s.White((0.3f + 0.25f * hover) * alpha), 1);
    float inner = g_set.iconSize * 0.76f, mini = inner / 2 - 2, cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    for (size_t k = 0; k < f.apps.size() && k < 4; k++) {
        float x = cx - inner / 2 + (k % 2) * (inner / 2) + 1, y = cy - inner / 2 + (k / 2) * (inner / 2) + 1;
        DrawIcon(s, f.apps[k], D2D1::RectF(x, y, x + mini, y + mini), alpha);
    }
}

// ---- the open group ----

static HWND g_group;
static Surface g_groupS;
static int g_groupFolder = -1, g_groupHover = -1, g_groupX, g_groupY;
static bool g_groupDone, g_groupEditing;
static std::wstring g_groupEdit;
static float g_groupW, g_groupH;
static Anim g_aGroup;
struct GroupCell {
    D2D1_RECT_F r;
    std::wstring launch;
    std::vector<HWND> windows;
};
static std::vector<GroupCell> g_groupCells;
static D2D1_RECT_F g_groupEditR;
static const int kGroupRename = -10;
static const float kGroupShadow = 12;

static void PaintGroup() {
    if (!g_group || g_groupFolder < 0 || g_groupFolder >= (int)g_folders.size()) return;
    Surface& s = g_groupS;
    float W = g_groupW, H = g_groupH;
    if (!s.Begin(Px(W + kGroupShadow), Px(H + kGroupShadow))) return;
    // it zooms out of the launcher icon (from its left middle) and fades in, like opening a folder on iOS
    float z = g_aGroup.running ? g_aGroup.value : 1, fade = std::clamp((z - 0.8f) / 0.15f, 0.f, 1.f);
    s.rt->SetTransform(D2D1::Matrix3x2F::Scale(z, z, D2D1::Point2F(0, H / 2)));
    ID2D1Layer* layer = nullptr;
    if (fade < 1 && SUCCEEDED(s.rt->CreateLayer(&layer)))
        s.rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), fade), layer);
    for (int i = 8; i >= 1; i--)
        s.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(2, 3, W + i, H + i), 16.f + i, 16.f + i), s.Brush({0, 0, 0}, 0.05f));
    auto box = D2D1::RoundedRect(D2D1::RectF(0.5f, 0.5f, W - 0.5f, H - 0.5f), 16, 16);
    s.rt->FillRoundedRectangle(box, s.Brush(Shade(g_tint, 0.45f), 0xF2 / 255.f));
    s.rt->DrawRoundedRectangle(box, s.White(0x38 / 255.f), 1);
    // the name (typed into while renaming) and the rename button
    PinGroup& f = g_folders[g_groupFolder];
    D2D1_RECT_F nameR = D2D1::RectF(18, 12, W - 52, 40);
    if (g_groupEditing) s.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(12, 10, W - 48, 42), 6, 6), s.White(0.1f));
    Text(s, g_groupEditing ? g_groupEdit + L"\u200A|" : f.name, nameR, 17, 1, DWRITE_TEXT_ALIGNMENT_LEADING, false, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD);
    g_groupEditR = D2D1::RectF(W - 44, 12, W - 14, 40);
    if (g_groupHover == kGroupRename) s.rt->FillRoundedRectangle(D2D1::RoundedRect(g_groupEditR, 6, 6), s.White(0x22 / 255.f));
    Glyph(s, g_groupEditing ? L"\uE73E" : L"\uE70F", g_groupEditR, 13, 0xC0 / 255.f);
    for (int k = 0; k < (int)g_groupCells.size(); k++) {
        auto& c = g_groupCells[k];
        if (k == g_groupHover) s.rt->FillRoundedRectangle(D2D1::RoundedRect(c.r, 10, 10), s.White(0x26 / 255.f));
        float cx = (c.r.left + c.r.right) / 2;
        DrawIcon(s, c.launch, D2D1::RectF(cx - 23, c.r.top + 8, cx + 23, c.r.top + 54), 1);
        Text(s, NameFor(c.launch), D2D1::RectF(c.r.left + 3, c.r.top + 59, c.r.right - 3, c.r.top + 89), 11.5f, 1, DWRITE_TEXT_ALIGNMENT_CENTER, true);
        if (!c.windows.empty()) s.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, c.r.top + 93), 2.5f, 2.5f), s.White(1));   // running, like iOS / macOS
    }
    if (layer) {
        s.rt->PopLayer();
        layer->Release();
    }
    s.rt->SetTransform(D2D1::Matrix3x2F::Identity());
    s.End(g_groupX, g_groupY);
}

static void ApplyGroupName() {
    std::wstring name = Trim(g_groupEdit);
    g_groupEditing = false;
    if (name.empty() || g_groupFolder < 0 || g_groupFolder >= (int)g_folders.size()) return;
    g_folders[g_groupFolder].name = name;
    GroupsChanged();
}

static int GroupHit(float x, float y) {
    if (Inside(g_groupEditR, x, y)) return kGroupRename;
    for (int k = 0; k < (int)g_groupCells.size(); k++)
        if (Inside(g_groupCells[k].r, x, y)) return k;
    return -1;
}

static void OpenMember(const GroupCell& c) {
    if (c.windows.empty()) {
        RecordRecent(c.launch);
        Launch(c.launch);
    } else Activate(c.windows[0]);
    SetTimer(g_host, TIMER_SOON, 150, nullptr);
}

static void MemberMenu(int k) {
    GroupCell c = g_groupCells[k];
    int fi = g_groupFolder;
    std::wstring file = FileOf(c.launch);
    std::vector<MenuEntry> m = {MHeading(NameFor(c.launch)), MSeparator(), MItem(1, Tr(L"Uit groep halen"), L"\uE738"),
                                MItem(2, c.windows.empty() ? Tr(L"Openen") : Tr(L"Nieuw venster"), L"\uE8A7"), MItem(3, Tr(L"Als administrator uitvoeren"), L"\uEA18")};
    if (!file.empty()) m.push_back(MItem(4, Tr(L"Bestandslocatie openen"), L"\uE838"));
    m.push_back(MItem(5, Tr(L"App-instellingen"), L"\uE713"));
    m.push_back(MSeparator());
    m.push_back(MItem(5, Tr(L"Verwijderen"), L"\uE74D"));
    RECT at = {g_groupX + Px(c.r.left), g_groupY + Px(c.r.top), g_groupX + Px(c.r.right), g_groupY + Px(c.r.bottom)};
    int cmd = ShowMenu(m, at, MENU_BELOW);
    if (cmd) g_groupDone = true;
    if (cmd == 1) RemoveFromGroup(fi, c.launch);
    else if (cmd == 2) { RecordRecent(c.launch); Launch(c.launch); }
    else if (cmd == 3) Launch(c.launch, L"runas");
    else if (cmd == 4) ShowInFolder(file);
    else if (cmd == 5) Launch(L"ms-settings:appsfeatures");   // uninstalling happens in Settings, like Start
    else if (!g_groupDone && IsWindowVisible(g_group)) {
        SetForegroundWindow(g_group);
        SetFocus(g_group);
    }
}

static LRESULT CALLBACK GroupProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    float x = GET_X_LPARAM(lp) / (float)g_scale, y = GET_Y_LPARAM(lp) / (float)g_scale;
    switch (msg) {
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE && (HWND)lp != g_menu) g_groupDone = true;   // its own menu may take the focus
            return 0;
        case WM_MOUSEMOVE: {
            int h = GroupHit(x, y);
            if (h != g_groupHover) { g_groupHover = h; PaintGroup(); }
            return 0;
        }
        case WM_LBUTTONUP: {
            int h = GroupHit(x, y);
            if (h == kGroupRename) {
                if (g_groupEditing) ApplyGroupName();
                else {
                    g_groupEditing = true;
                    g_groupEdit = g_folders[g_groupFolder].name;
                }
                PaintGroup();
            } else if (h >= 0) {
                GroupCell c = g_groupCells[h];
                g_groupDone = true;
                ShowWindow(hwnd, SW_HIDE);
                OpenMember(c);
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            int h = GroupHit(x, y);
            if (h >= 0) MemberMenu(h);
            return 0;
        }
        case WM_CHAR:
            if (g_groupEditing && wp >= 32 && wp != 127 && g_groupEdit.size() < 40) {
                g_groupEdit += (wchar_t)wp;
                PaintGroup();
            }
            return 0;
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE) {
                if (g_groupEditing) { g_groupEditing = false; PaintGroup(); }
                else g_groupDone = true;
            } else if (wp == VK_RETURN && g_groupEditing) {
                ApplyGroupName();
                PaintGroup();
            } else if (wp == VK_BACK && g_groupEditing && !g_groupEdit.empty()) {
                g_groupEdit.pop_back();
                PaintGroup();
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// opens the group of launcher item index next to its icon, and stays until it loses the focus or an app is opened
static void OpenGroup(int index, bool rename) {
    if (!g_group || index < 0 || index >= (int)g_items.size() || g_items[index].folder < 0 || IsWindowVisible(g_group)) return;
    HideTip();
    HidePreview();
    if (g_dashOpen) ShowDash(false);
    g_groupFolder = g_items[index].folder;
    PinGroup& f = g_folders[g_groupFolder];
    auto wins = Snapshot();
    int cols = f.apps.size() <= 9 ? 3 : 4, rows = std::max(1, ((int)f.apps.size() + cols - 1) / cols);
    g_groupCells.clear();
    for (size_t k = 0; k < f.apps.size(); k++) {
        GroupCell c;
        c.launch = f.apps[k];
        float x = 14 + (k % cols) * 94.f + 2, y = 52 + (k / cols) * 104.f + 2;
        c.r = D2D1::RectF(x, y, x + 90, y + 100);
        Item probe;
        auto r = Resolve(c.launch);
        probe.aumid = r.first;
        probe.exe = r.second;
        for (auto& w : wins)
            if (Matches(probe, w)) c.windows.push_back(w.hwnd);
        g_groupCells.push_back(c);
    }
    g_groupW = 28 + cols * 94.f;
    g_groupH = 52 + rows * 104.f + 12;
    // right of the launcher, centred on the icon, kept on the screen
    D2D1_RECT_F ir = ItemRect(index);
    int mid = g_screen.top + Px(PanelH()) + Px((ir.top + ir.bottom) / 2);
    g_groupX = g_screen.left + Px(LauncherW()) + Px(8);
    g_groupY = std::max((int)g_screen.top + Px(PanelH()) + Px(4), std::min(mid - Px(g_groupH) / 2, (int)g_screen.bottom - Px(g_groupH + kGroupShadow)));
    g_groupEditing = rename;
    g_groupEdit = rename ? f.name : L"";
    g_groupHover = -1;
    g_groupDone = false;
    g_aGroup.Go(1, 220, EASE_OUT_BACK, 0.8f);
    PaintGroup();
    ShowWindow(g_group, SW_SHOW);
    SetForegroundWindow(g_group);
    SetFocus(g_group);
    KickAnim();
    MSG m;
    while (!g_groupDone) {
        BOOL got = GetMessageW(&m, nullptr, 0, 0);
        if (got <= 0) {
            if (got == 0) PostQuitMessage((int)m.wParam);
            break;
        }
        bool click = m.message == WM_LBUTTONDOWN || m.message == WM_RBUTTONDOWN || m.message == WM_NCLBUTTONDOWN;
        if (click && m.hwnd != g_group && m.hwnd != g_menu) break;   // a click on another of the shell's windows closes it
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }
    if (g_groupEditing) ApplyGroupName();   // a new name counts when clicking away too
    g_groupDone = true;
    ShowWindow(g_group, SW_HIDE);
}

static void ClickItem(int index) {
    if (index >= 0 && index < (int)g_items.size() && g_items[index].folder >= 0) {
        OpenGroup(index, false);
        return;
    }
    if (index == kTrash) {
        if (g_dashOpen) ShowDash(false);
        Launch(L"shell:RecycleBinFolder");
        return;
    }
    if (index == -1) {
        g_aBfb.Go(1, 380, EASE_OUT_BACK, 0.86f);   // the button gives a little
        KickAnim();
        ToggleDash();
        return;
    }
    if (index < 0 || index >= (int)g_items.size()) return;
    auto& it = g_items[index];
    if (g_dashOpen) ShowDash(false);
    if (it.windows.empty()) {
        RecordRecent(it.launch);
        Fx(it.launch).pulseStart = NowMs();
        KickAnim();
        Launch(it.launch);
        return;
    }
    // the focused app (asked live: the list can be a moment old): minimize (one window) or go to its next window
    // (several), like Unity
    HWND fg = GetForegroundWindow();
    bool focused = fg && !IsIconic(fg) && std::find(it.windows.begin(), it.windows.end(), fg) != it.windows.end();
    if (focused) {
        if (it.windows.size() == 1) { Minimize(it.windows[0]); return; }
        size_t cur = 0;
        for (size_t k = 0; k < it.windows.size(); k++)
            if (it.windows[k] == fg) cur = k;
        Activate(it.windows[(cur + 1) % it.windows.size()]);
        return;
    }
    // otherwise: the app's windows come up, the one used last on top with the focus
    HWND last = it.windows[0];
    for (HWND w : it.windows)
        if (g_zRank.count(w) && (!g_zRank.count(last) || g_zRank[w] < g_zRank[last])) last = w;
    std::vector<HWND> others;   // back to front, so they keep their order on top of everything else
    for (HWND w : it.windows)
        if (w != last && !IsIconic(w)) others.push_back(w);
    std::sort(others.begin(), others.end(), [](HWND a, HWND b) { return g_zRank[a] > g_zRank[b]; });
    for (HWND w : others) SetWindowPos(w, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    Activate(last);
    SetTimer(g_host, TIMER_SOON, 150, nullptr);
}

// the app's file on disk, for "open file location": the pinned file itself, or the program a Start entry runs
static std::wstring FileOf(const std::wstring& launch) {
    if (!StartsWith(launch, kAppsFolder) && GetFileAttributesW(launch.c_str()) != INVALID_FILE_ATTRIBUTES) return launch;
    std::wstring exe = Resolve(launch).second;
    return !exe.empty() && GetFileAttributesW(exe.c_str()) != INVALID_FILE_ATTRIBUTES ? exe : L"";
}

static void ShowInFolder(const std::wstring& file) {
    ShellExecuteW(nullptr, nullptr, L"explorer.exe", (L"/select,\"" + file + L"\"").c_str(), nullptr, SW_SHOWNORMAL);
}

static void SetLens(int lens);

// the right-click menus of the launcher (the app's quicklists): the Dash button, the trash, and every app
static void ItemMenu(int index) {
    D2D1_RECT_F ir = ItemRect(index);
    RECT at = {g_screen.left + Px(ir.left), g_screen.top + Px(PanelH()) + Px(ir.top), g_screen.left + Px(LauncherW()), g_screen.top + Px(PanelH()) + Px(ir.bottom)};
    if (index == -1) {
        static const wchar_t* names[] = {L"Home", L"Applications", L"Files", L"Video", L"Music", L"Photos"};   // the lenses, in order
        std::vector<MenuEntry> m = {MHeading(L"Dash"), MSeparator()};
        for (int l = 0; l < 6; l++) m.push_back(MItem(10 + l, names[l]));
        m.push_back(MSeparator());
        m.push_back(MItem(1, L"Unity settings…"));
        int cmd = ShowMenu(m, at, MENU_RIGHT);
        if (cmd >= 10) {
            if (!g_dashOpen) ShowDash(true);
            SetLens(cmd - 10);
        } else if (cmd == 1) OpenShellSettings();
        return;
    }
    if (index == kTrash) {
        std::vector<MenuEntry> m = {MHeading(L"Trash"), MSeparator(), MItem(1, L"Open"), MItem(2, L"Empty Trash…", nullptr, g_trashFull)};
        int cmd = ShowMenu(m, at, MENU_RIGHT);
        if (cmd == 1) Launch(L"shell:RecycleBinFolder");
        else if (cmd == 2) {
            SHEmptyRecycleBinW(g_launcher, nullptr, 0);   // Windows asks for confirmation itself
            CheckTrash(true);
            RenderLauncher();
        }
        return;
    }
    if (index < 0 || index >= (int)g_items.size()) return;
    if (g_items[index].folder >= 0) {   // a group (the app's menu)
        int fi = g_items[index].folder;
        std::vector<MenuEntry> m = {MHeading(g_items[index].name), MSeparator(), MItem(1, Tr(L"Openen"), L"\uE8A7"), MItem(2, Tr(L"Naam wijzigen"), L"\uE70F"),
                                    MItem(3, Tr(L"Groep opheffen"), L"\uE8C6"), MSeparator(), MItem(4, Tr(L"Uit launcher verwijderen"), L"\uE74D")};
        int cmd = ShowMenu(m, at, MENU_RIGHT);
        if (cmd == 1 || cmd == 2) PostMessageW(g_launcher, WM_APP_OPENGROUP, index, cmd == 2);
        else if (cmd == 3) Ungroup(fi);
        else if (cmd == 4) RemoveGroup(fi);
        return;
    }
    Item it = g_items[index];
    std::wstring file = FileOf(it.launch);
    std::vector<MenuEntry> m = {MHeading(it.name), MSeparator()};
    if (it.windows.size() > 1) {   // Unity's quicklist: straight to one of the app's windows
        for (size_t k = 0; k < it.windows.size() && k < 10; k++) {
            std::wstring t = WindowTitle(it.windows[k]);
            if (t.size() > 50) t = t.substr(0, 50) + L"…";
            m.push_back(MItem(100 + (int)k, t, L"\uE737"));
        }
        m.push_back(MSeparator());
    }
    m.push_back(MItem(1, it.windows.empty() ? Tr(L"Openen") : Tr(L"Nieuw venster"), L"\uE8A7"));
    m.push_back(MItem(2, Tr(L"Als administrator uitvoeren"), L"\uEA18"));
    m.push_back(it.pinned ? MItem(3, Tr(L"Losmaken van launcher"), L"\uE77A") : MItem(3, Tr(L"Vastmaken aan launcher"), L"\uE718"));
    if (!file.empty()) m.push_back(MItem(4, Tr(L"Bestandslocatie openen"), L"\uE838"));
    m.push_back(MItem(5, Tr(L"App-instellingen"), L"\uE713"));
    m.push_back(MSeparator());
    if (!it.windows.empty()) m.push_back(MItem(6, it.windows.size() > 1 ? Tr(L"Alle vensters sluiten") : Tr(L"Venster sluiten"), L"\uE8BB"));
    m.push_back(MItem(7, Tr(L"Verwijderen"), L"\uE74D"));
    int cmd = ShowMenu(m, at, MENU_RIGHT);
    if (cmd >= 100) {
        if (g_dashOpen) ShowDash(false);
        Activate(it.windows[cmd - 100]);
    } else if (cmd == 1) Launch(it.launch);
    else if (cmd == 2) Launch(it.launch, L"runas");
    else if (cmd == 3) { Pin(it.launch, !it.pinned); BuildItems(); RenderLauncher(); }
    else if (cmd == 4) ShowInFolder(file);
    else if (cmd == 5 || cmd == 7) Launch(L"ms-settings:appsfeatures");   // uninstalling happens in Settings, like Start
    else if (cmd == 6) for (HWND w : it.windows) PostMessageW(w, WM_CLOSE, 0, 0);
}

// the launcher's right edge: drag it to make the launcher (its icons) bigger or smaller, remembered until the
// icon size setting itself changes
static void Layout();
static bool g_launcherSizing;
static int g_sizingFrom;
static bool OnLauncherEdge(float x) { return x >= LauncherW() - 4; }

static void EndLauncherSizing() {
    g_launcherSizing = false;
    if (g_set.iconSize == g_sizingFrom) return;
    Wh_SetIntValue(L"launcherIcon", g_set.iconSize);
    Wh_SetIntValue(L"launcherIconFrom", std::clamp(Wh_GetIntSetting(L"launcher.iconSize"), 32, 80));
    // the icons again at the new size (they were loaded for the old one)
    for (auto it = g_icons.begin(); it != g_icons.end();)
        if (it->first.rfind(L"t|", 0) != 0) { Release(it->second.src); it = g_icons.erase(it); }
        else ++it;
    for (Surface* s : {&g_launcherS, &g_dashS, &g_groupS}) {
        for (auto& kv : s->bitmaps) Release(kv.second);
        s->bitmaps.clear();
    }
    RenderLauncher();
}

static LRESULT CALLBACK LauncherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_SETCURSOR: {
            POINT p;
            GetCursorPos(&p);
            ScreenToClient(hwnd, &p);
            if (LOWORD(lp) == HTCLIENT && g_dragIndex < 0 && (g_launcherSizing || OnLauncherEdge(p.x / (float)g_scale))) {
                SetCursor(LoadCursorW(nullptr, IDC_SIZEWE));
                return TRUE;
            }
            break;
        }
        case WM_MOUSEMOVE: {
            float x = GET_X_LPARAM(lp) / (float)g_scale, y = GET_Y_LPARAM(lp) / (float)g_scale;
            if (g_launcherSizing) {
                int size = std::clamp((int)std::lround(x - 16 + 2), 32, 80);   // the pointer stays on the edge
                if (size != g_set.iconSize) {
                    g_set.iconSize = size;
                    Layout();
                    RenderLauncher();
                }
                return 0;
            }
            // pressing an icon and moving a little picks it up
            if (g_dragIndex < 0 && g_pressIndex >= 0 && (wp & MK_LBUTTON) &&
                (abs(GET_X_LPARAM(lp) - g_pressPt.x) > Px(6) || abs(GET_Y_LPARAM(lp) - g_pressPt.y) > Px(6))) {
                g_dragIndex = g_dragTarget = g_pressIndex;
                HideTip();
                HidePreview();
                SetLauncherHover(-2);
            }
            if (g_dragIndex >= 0) {
                g_dragY = y;
                g_dragOut = x > LauncherW() + 24;
                // over the middle of another icon: a group; between icons: a new place
                g_dragOnto = -1;
                if (!g_dragOut && g_items[g_dragIndex].folder < 0)
                    for (int i = 0; i < VisibleItems(); i++) {
                        D2D1_RECT_F r = ItemRect(i);
                        float q = g_set.iconSize * 0.25f;
                        if (i != g_dragIndex && y >= r.top + q && y <= r.bottom - q) g_dragOnto = i;
                    }
                g_dragTarget = g_dragOut || g_dragOnto >= 0 ? g_dragIndex : DragSlot(y);
                for (int i = 0; i < VisibleItems(); i++) {   // the others slide out of its way
                    if (i == g_dragIndex) continue;
                    int v = i;
                    if (g_dragIndex < i && i <= g_dragTarget) v = i - 1;
                    if (g_dragTarget <= i && i < g_dragIndex) v = i + 1;
                    Anim& shift = Fx(g_items[i].launch).shift;
                    if (shift.to != (float)(v - i)) shift.Go((float)(v - i), 140, EASE_OUT_CUBIC);
                }
                KickAnim();
                RenderLauncher();
                return 0;
            }
            int hover = ItemAt(x, y);
            int part = hover == kExt ? ExtPartAt(x, y) : -1, dcPart = hover == kDc ? DcPartAt(x, y) : -1;
            if (part != g_extHover || dcPart != g_dcHover) { g_extHover = part; g_dcHover = dcPart; RenderLauncher(); }
            if (hover != g_launcherHover) { SetLauncherHover(hover); }
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }
        case WM_MOUSELEAVE:
            if (g_extHover != -1 || g_dcHover != -1) { g_extHover = g_dcHover = -1; RenderLauncher(); }
            if (g_launcherHover != -2) SetLauncherHover(-2);
            HideTip();
            KillTimer(hwnd, TIMER_PREVIEWSHOW);
            ScheduleHidePreview();
            return 0;
        case WM_LBUTTONDOWN:
            if (OnLauncherEdge(GET_X_LPARAM(lp) / (float)g_scale)) {
                g_launcherSizing = true;
                g_sizingFrom = g_set.iconSize;
                g_pressIndex = -2;
                HideTip();
                HidePreview();
                SetCapture(hwnd);
                return 0;
            }
            g_pressIndex = ItemAt(GET_X_LPARAM(lp) / (float)g_scale, GET_Y_LPARAM(lp) / (float)g_scale);
            if (g_pressIndex == kExt) {   // the block's buttons are pressed, not dragged
                g_pressIndex = -2;
                ExtClick(GET_X_LPARAM(lp) / (float)g_scale, GET_Y_LPARAM(lp) / (float)g_scale);
                return 0;
            }
            if (g_pressIndex == kDc) {
                g_pressIndex = -2;
                DcClick(GET_X_LPARAM(lp) / (float)g_scale, GET_Y_LPARAM(lp) / (float)g_scale);
                return 0;
            }
            g_pressPt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            SetCapture(hwnd);
            return 0;
        case WM_LBUTTONUP: {
            if (g_launcherSizing) {
                ReleaseCapture();   // ends it (WM_CAPTURECHANGED)
                return 0;
            }
            int pressed = g_pressIndex;
            bool dragged = g_dragIndex >= 0;
            g_pressIndex = -2;
            if (dragged) DropItem();
            ReleaseCapture();
            if (!dragged && pressed == kExt) {
                ExtClick(GET_X_LPARAM(lp) / (float)g_scale, GET_Y_LPARAM(lp) / (float)g_scale);
                return 0;
            }
            if (!dragged && pressed != -2 && pressed == ItemAt(GET_X_LPARAM(lp) / (float)g_scale, GET_Y_LPARAM(lp) / (float)g_scale)) {
                HideTip();
                HidePreview();
                ClickItem(pressed);
            }
            return 0;
        }
        case WM_CAPTURECHANGED:
            g_pressIndex = -2;
            if (g_launcherSizing) EndLauncherSizing();
            if (g_dragIndex >= 0 && (HWND)lp != hwnd) CancelDrag();
            return 0;
        case WM_APP_OPENGROUP:
            OpenGroup((int)wp, lp != 0);
            return 0;
        case WM_TIMER:
            if (wp == TIMER_PREVIEWSHOW) {
                KillTimer(hwnd, TIMER_PREVIEWSHOW);
                if (g_dragIndex < 0) ShowPreview(g_launcherHover);
            } else if (wp == TIMER_PREVIEWHIDE) {
                KillTimer(hwnd, TIMER_PREVIEWHIDE);
                // stays while the pointer is on the previews or back on their icon
                POINT pt;
                GetCursorPos(&pt);
                HWND under = WindowFromPoint(pt);
                if (under != g_preview && !(under == hwnd && g_launcherHover == g_previewFor)) HidePreview();
            }
            return 0;
        case WM_MBUTTONUP: {
            int i = ItemAt(GET_X_LPARAM(lp) / (float)g_scale, GET_Y_LPARAM(lp) / (float)g_scale);
            if (i >= 0 && i < (int)g_items.size()) Launch(g_items[i].launch);   // middle click: a new window
            return 0;
        }
        case WM_RBUTTONUP: {
            POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            int i = ItemAt(pt.x / (float)g_scale, pt.y / (float)g_scale);
            HideTip();
            HidePreview();
            ItemMenu(i);
            return 0;
        }
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ============================================================================================================
// the Dash: lenses (Home, Apps, Files, Video, Music, Photos), sections of results, and widgets on Home
// ============================================================================================================

enum Lens { LENS_HOME, LENS_APPS, LENS_FILES, LENS_VIDEO, LENS_MUSIC, LENS_PHOTOS, LENS_COUNT };
static const wchar_t* kLensGlyph[LENS_COUNT] = {L"\uE80F", L"\uE71D", L"\uE8B7", L"\uE714", L"\uE8D6", L"\uEB9F"};
static const wchar_t* kLensHint[LENS_COUNT] = {L"Zoek op je computer en in instellingen", L"Zoek applicaties", L"Zoek bestanden en mappen",
                                               L"Zoek video's", L"Zoek muziek", L"Zoek foto's"};
static int g_lens = LENS_HOME;
static std::wstring g_query;

enum ResultKind { RK_APP, RK_SETTING, RK_FILE };
struct Result {
    std::wstring name, open;   // what is shown, what is opened
    int kind = RK_APP;
    bool thumb = false;        // show a thumbnail instead of the file's icon
    int group = 0;             // which file list it belongs to (recent / downloads)
};
struct Section {
    std::wstring title;
    const wchar_t* glyph;
    std::vector<Result> items;
    int rows;                  // rows shown before "Alles tonen"
};
static std::vector<Section> g_sections;
static std::map<std::wstring, bool> g_expanded;   // "lens/title" -> show all

enum { HIT_TILE, HIT_MORE, HIT_LENS, HIT_MEDIA, HIT_GRIP, HIT_FILTERS, HIT_FGROUP, HIT_FCHIP, HIT_WEATHER };
static bool g_resizing;   // dragging the Dash's grip
struct Hit { D2D1_RECT_F r; int kind, a, b; };
static std::vector<Hit> g_hits;
struct Flat { int section, index; D2D1_RECT_F r; };
static std::vector<Flat> g_flat;   // the visible tiles in reading order, for the keyboard
static int g_sel = 0, g_hoverHit = -1;
static float g_scroll = 0, g_contentH = 0;

static const float kTileW = 124, kTileH = 112, kMargin = 18, kSearchH = 40, kLensBarH = 46;

static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t"), b = s.find_last_not_of(L" \t");
    return a == std::wstring::npos ? L"" : s.substr(a, b - a + 1);
}

// ---------------- settings catalogue (the Dutch names are its keys; they are shown through Tr) ----------------

struct SettingEntry { const wchar_t *name, *words, *open; };
static const SettingEntry kSettings[] = {
    {L"Beeldscherm", L"display screen resolution scale monitor resolutie schaal", L"ms-settings:display"},
    {L"Nachtlampje", L"night light blue light warm", L"ms-settings:nightlight"},
    {L"Grafische instellingen", L"graphics gpu hdr advanced graphics videokaart", L"ms-settings:display-advancedgraphics"},
    {L"Geluid", L"sound audio speakers output input luidspreker microfoon", L"ms-settings:sound"},
    {L"Volumemixer", L"volume mixer app volume", L"ms-settings:apps-volume"},
    {L"Meldingen", L"notifications do not disturb niet storen", L"ms-settings:notifications"},
    {L"Focus", L"focus assist quiet hours", L"ms-settings:quiethours"},
    {L"Energie en batterij", L"power battery sleep slaapstand energiebeheer", L"ms-settings:powersleep"},
    {L"Opslag", L"storage disk space cleanup schijf opslagzin", L"ms-settings:storagesense"},
    {L"Multitasking", L"snap windows multitasking vensters vastmaken", L"ms-settings:multitasking"},
    {L"Klembord", L"clipboard history copy paste", L"ms-settings:clipboard"},
    {L"Projecteren naar deze pc", L"project projecting cast", L"ms-settings:project"},
    {L"Systeeminfo", L"about system info pc name specs over", L"ms-settings:about"},
    {L"Herstel", L"recovery reset pc opnieuw instellen", L"ms-settings:recovery"},
    {L"Activering", L"activation product key windows license", L"ms-settings:activation"},
    {L"Bluetooth en apparaten", L"bluetooth devices pair koppelen", L"ms-settings:bluetooth"},
    {L"Printers en scanners", L"printer scanner print", L"ms-settings:printers"},
    {L"Muis", L"mouse pointer speed cursor muisaanwijzer", L"ms-settings:mousetouchpad"},
    {L"Touchpad", L"touchpad gestures", L"ms-settings:devices-touchpad"},
    {L"Camera's", L"camera webcam", L"ms-settings:camera"},
    {L"Automatisch afspelen", L"autoplay usb", L"ms-settings:autoplay"},
    {L"Netwerk en internet", L"network internet status netwerkstatus", L"ms-settings:network-status"},
    {L"Wi-Fi", L"wifi wireless draadloos", L"ms-settings:network-wifi"},
    {L"Ethernet", L"ethernet wired cable kabel", L"ms-settings:network-ethernet"},
    {L"VPN", L"vpn", L"ms-settings:network-vpn"},
    {L"Mobiele hotspot", L"hotspot tethering", L"ms-settings:network-mobilehotspot"},
    {L"Vliegtuigmodus", L"airplane mode flight", L"ms-settings:network-airplanemode"},
    {L"Proxy", L"proxy", L"ms-settings:network-proxy"},
    {L"Achtergrond", L"background wallpaper bureaubladachtergrond personalize personalisatie", L"ms-settings:personalization-background"},
    {L"Kleuren", L"colors accent color dark mode light mode donker licht thema kleur", L"ms-settings:colors"},
    {L"Thema's", L"themes theme", L"ms-settings:themes"},
    {L"Vergrendelingsscherm", L"lock screen", L"ms-settings:lockscreen"},
    {L"Start", L"start menu personalization", L"ms-settings:personalization-start"},
    {L"Taakbalk", L"taskbar auto hide automatisch verbergen", L"ms-settings:taskbar"},
    {L"Lettertypen", L"fonts font", L"ms-settings:fonts"},
    {L"Geïnstalleerde apps", L"apps installed uninstall remove programs programma's verwijderen apps en onderdelen", L"ms-settings:appsfeatures"},
    {L"Standaard-apps", L"default apps browser file associations standaardprogramma", L"ms-settings:defaultapps"},
    {L"Opstarten", L"startup apps autostart opstartprogramma's", L"ms-settings:startupapps"},
    {L"Optionele onderdelen", L"optional features", L"ms-settings:optionalfeatures"},
    {L"Uw info", L"account profile picture accounts", L"ms-settings:yourinfo"},
    {L"Aanmeldingsopties", L"sign in options password pin windows hello wachtwoord", L"ms-settings:signinoptions"},
    {L"E-mail en accounts", L"email accounts mail", L"ms-settings:emailandaccounts"},
    {L"Datum en tijd", L"date time clock timezone tijdzone klok", L"ms-settings:dateandtime"},
    {L"Taal en regio", L"language region locale taal regio", L"ms-settings:regionlanguage"},
    {L"Toetsenbord", L"keyboard layout input typing typen", L"ms-settings:keyboard"},
    {L"Spraak", L"speech voice", L"ms-settings:speech"},
    {L"Gamemodus", L"game mode gaming", L"ms-settings:gaming-gamemode"},
    {L"Xbox Game Bar", L"game bar xbox capture opnames", L"ms-settings:gaming-gamebar"},
    {L"Opnames", L"captures game dvr screen recording", L"ms-settings:gaming-gamedvr"},
    {L"Tekstgrootte", L"text size accessibility toegankelijkheid", L"ms-settings:easeofaccess-display"},
    {L"Muisaanwijzer en aanraken", L"mouse pointer size color cursor", L"ms-settings:easeofaccess-mousepointer"},
    {L"Vergrootglas", L"magnifier zoom", L"ms-settings:easeofaccess-magnifier"},
    {L"Kleurfilters", L"color filters colorblind", L"ms-settings:easeofaccess-colorfilter"},
    {L"Privacy en beveiliging", L"privacy security permissions machtigingen", L"ms-settings:privacy"},
    {L"Locatie", L"location gps", L"ms-settings:privacy-location"},
    {L"Microfoon-privacy", L"microphone permissions", L"ms-settings:privacy-microphone"},
    {L"Camera-privacy", L"camera permissions webcam", L"ms-settings:privacy-webcam"},
    {L"Windows-beveiliging", L"windows security defender antivirus firewall virus", L"windowsdefender:"},
    {L"Windows Update", L"update updates bijwerken", L"ms-settings:windowsupdate"},
    {L"Voor ontwikkelaars", L"developer mode developers", L"ms-settings:developers"},
    {L"Configuratiescherm", L"control panel classic", L"control.exe"},
    {L"Apparaatbeheer", L"device manager drivers stuurprogramma", L"devmgmt.msc"},
    {L"Taakbeheer", L"task manager processes performance prestaties", L"taskmgr.exe"},
    {L"Schijfbeheer", L"disk management partitions partities", L"diskmgmt.msc"},
    {L"Systeemeigenschappen", L"advanced system settings environment variables omgevingsvariabelen", L"SystemPropertiesAdvanced.exe"},
    {L"Energiebeheer (klassiek)", L"power options plan", L"powercfg.cpl"},
    {L"Netwerkverbindingen", L"network adapters connections adapter", L"ncpa.cpl"},
    {L"Geluid (klassiek)", L"sound control panel playback recording devices", L"mmsys.cpl"},
};

static std::vector<Result> FindSettings(const std::wstring& q) {
    std::vector<Result> out;
    std::vector<std::wstring> words;
    for (size_t i = 0; i < q.size();) {
        size_t j = q.find(L' ', i);
        if (j == std::wstring::npos) j = q.size();
        if (j > i) words.push_back(q.substr(i, j - i));
        i = j + 1;
    }
    for (auto& e : kSettings) {
        std::wstring hay = Lower(std::wstring(e.name) + L" " + Tr(e.name) + L" " + e.words);   // either language finds it
        bool all = !words.empty();
        for (auto& w : words)
            if (hay.find(w) == std::wstring::npos) all = false;
        if (all) out.push_back({Tr(e.name), e.open, RK_SETTING});
    }
    return out;
}

// ---------------- recently used apps (kept in the mod's storage) ----------------

static std::vector<std::wstring> g_recentApps;

static void LoadRecentApps() {
    g_recentApps.clear();
    wchar_t buf[8192] = {};
    Wh_GetStringValue(L"recentApps", buf, ARRAYSIZE(buf));
    std::wstring s = buf;
    size_t start = 0, nl;
    while ((nl = s.find(L'\n', start)) != std::wstring::npos) {
        if (nl > start) g_recentApps.push_back(s.substr(start, nl - start));
        start = nl + 1;
    }
}

static void RecordRecent(const std::wstring& launch) {
    auto same = [&](const std::wstring& p) { return _wcsicmp(p.c_str(), launch.c_str()) == 0; };
    g_recentApps.erase(std::remove_if(g_recentApps.begin(), g_recentApps.end(), same), g_recentApps.end());
    g_recentApps.insert(g_recentApps.begin(), launch);
    if (g_recentApps.size() > 18) g_recentApps.resize(18);
    std::wstring all;
    for (auto& p : g_recentApps) all += p + L"\n";
    Wh_SetStringValue(L"recentApps", all.c_str());
}

// ---------------- files: recent, Downloads and Windows Search, on a worker thread ----------------

struct FileReq { int gen, type, lens; std::wstring query; };   // type: 0 recent, 1 recent + downloads, 2 search
static std::mutex g_fileLock;
static FileReq g_fileReq;
static bool g_fileReqPending;
static std::vector<Result> g_fileResults;
static int g_fileGen = 0, g_fileResultsGen = -1;
static HANDLE g_fileWake, g_fileThread;
static volatile bool g_workersStop;

static bool IsImageOrVideo(const std::wstring& path) {
    static const wchar_t* exts[] = {L".jpg", L".jpeg", L".png", L".gif", L".bmp", L".webp", L".heic", L".tif", L".tiff",
                                    L".mp4", L".mkv", L".mov", L".avi", L".wmv", L".webm"};
    std::wstring l = Lower(path);
    for (auto e : exts)
        if (l.size() > wcslen(e) && l.compare(l.size() - wcslen(e), wcslen(e), e) == 0) return true;
    return false;
}

static std::wstring FileName(std::wstring path) {
    while (path.size() > 3 && (path.back() == L'\\' || path.back() == L'/')) path.pop_back();
    size_t p = path.find_last_of(L"\\/");
    std::wstring name = p == std::wstring::npos ? path : path.substr(p + 1);
    return name.empty() ? path : name;   // a drive such as U:\ keeps its full name
}

static std::vector<Result> RecentFiles(size_t max) {
    std::vector<Result> out;
    PWSTR dir = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_Recent, 0, nullptr, &dir))) return out;
    std::wstring folder = dir;
    CoTaskMemFree(dir);
    std::vector<std::pair<ULONGLONG, std::wstring>> links;
    WIN32_FIND_DATAW fd;
    HANDLE f = FindFirstFileW((folder + L"\\*.lnk").c_str(), &fd);
    if (f != INVALID_HANDLE_VALUE) {
        do {
            ULONGLONG t = ((ULONGLONG)fd.ftLastWriteTime.dwHighDateTime << 32) | fd.ftLastWriteTime.dwLowDateTime;
            links.push_back({t, folder + L"\\" + fd.cFileName});
        } while (FindNextFileW(f, &fd));
        FindClose(f);
    }
    std::sort(links.begin(), links.end(), [](auto& a, auto& b) { return a.first > b.first; });
    std::map<std::wstring, bool> seen;
    for (auto& l : links) {
        if (out.size() >= max || g_workersStop) break;
        IShellLinkW* link = nullptr;
        if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link)))) continue;
        IPersistFile* pf = nullptr;
        wchar_t target[MAX_PATH] = {};
        if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&pf)))) {
            if (SUCCEEDED(pf->Load(l.second.c_str(), STGM_READ))) link->GetPath(target, MAX_PATH, nullptr, SLGP_RAWPATH);
            pf->Release();
        }
        link->Release();
        std::wstring t = target;
        if (t.empty() || seen[Lower(t)] || GetFileAttributesW(t.c_str()) == INVALID_FILE_ATTRIBUTES) continue;
        seen[Lower(t)] = true;
        out.push_back({FileName(t), t, RK_FILE, IsImageOrVideo(t), 0});
    }
    return out;
}

static std::vector<Result> DownloadFiles(size_t max) {
    std::vector<Result> out;
    PWSTR dir = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_Downloads, 0, nullptr, &dir))) return out;
    std::wstring folder = dir;
    CoTaskMemFree(dir);
    std::vector<std::pair<ULONGLONG, std::wstring>> files;
    WIN32_FIND_DATAW fd;
    HANDLE f = FindFirstFileW((folder + L"\\*").c_str(), &fd);
    if (f != INVALID_HANDLE_VALUE) {
        do {
            if (fd.cFileName[0] == L'.' || (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) continue;
            std::wstring n = Lower(fd.cFileName);
            if (n.ends_with(L".crdownload") || n.ends_with(L".part") || n.ends_with(L".tmp") || n == L"desktop.ini") continue;
            ULONGLONG t = ((ULONGLONG)fd.ftLastWriteTime.dwHighDateTime << 32) | fd.ftLastWriteTime.dwLowDateTime;
            files.push_back({t, folder + L"\\" + fd.cFileName});
        } while (FindNextFileW(f, &fd));
        FindClose(f);
    }
    std::sort(files.begin(), files.end(), [](auto& a, auto& b) { return a.first > b.first; });
    for (auto& p : files) {
        if (out.size() >= max) break;
        out.push_back({FileName(p.second), p.second, RK_FILE, IsImageOrVideo(p.second), 1});
    }
    return out;
}

// a WinRT call waited on for at most this long, then cancelled: a slow search or a hung media app never holds a
// worker thread (and with it the shell's shutdown) up
template <typename Op>
static auto Await(Op const& op, int ms) {
    if (op.wait_for(std::chrono::milliseconds(ms)) != winrt::Windows::Foundation::AsyncStatus::Completed) {
        try { op.Cancel(); } catch (...) {}
        throw winrt::hresult_error(HRESULT_FROM_WIN32(ERROR_TIMEOUT));
    }
    return op.GetResults();
}

// Windows Search: the Storage query API uses the search index (and walks the folders where there is none)
static std::vector<Result> SearchFiles(const std::wstring& query, int lens, size_t max) {
    using namespace winrt::Windows::Storage;
    using namespace winrt::Windows::Storage::Search;
    std::vector<Result> out;
    std::wstring aqs = Trim(query);
    if (lens == LENS_VIDEO) aqs = L"kind:=video " + aqs;
    if (lens == LENS_MUSIC) aqs = L"kind:=music " + aqs;
    if (lens == LENS_PHOTOS) aqs = L"kind:=picture " + aqs;
    PWSTR profile = nullptr;
    SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &profile);
    std::wstring root = profile ? profile : L"C:\\";
    CoTaskMemFree(profile);
    try {
        auto folder = Await(StorageFolder::GetFolderFromPathAsync(root), 2000);
        QueryOptions opts;
        opts.FolderDepth(FolderDepth::Deep);
        opts.IndexerOption(IndexerOption::UseIndexerWhenAvailable);
        opts.UserSearchFilter(Trim(aqs));
        auto items = Await(folder.CreateItemQueryWithOptions(opts).GetItemsAsync(0, (uint32_t)max), 4000);
        for (auto const& item : items) {
            if (g_workersStop) break;
            std::wstring path(item.Path()), name(item.Name());
            if (path.empty()) continue;
            out.push_back({name.empty() ? FileName(path) : name, path, RK_FILE, lens == LENS_PHOTOS || lens == LENS_VIDEO || IsImageOrVideo(path), 0});
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"file search failed: 0x%08X %s", (unsigned)e.code(), e.message().c_str());
    } catch (...) {
    }
    return out;
}
enum : UINT { WM_APP_FILES = WM_APP + 10, WM_APP_WEATHER, WM_APP_MEDIA };

static DWORD WINAPI FileWorker(LPVOID) {
    // multi-threaded: the Storage search API waits for its results, which isn't allowed on a UI-style thread
    try { winrt::init_apartment(winrt::apartment_type::multi_threaded); } catch (...) {}
    while (WaitForSingleObject(g_fileWake, INFINITE) == WAIT_OBJECT_0 && !g_workersStop) {
        FileReq req;
        g_fileLock.lock();
        bool pending = g_fileReqPending;
        req = g_fileReq;
        g_fileReqPending = false;
        g_fileLock.unlock();
        if (!pending) continue;
        std::vector<Result> out;
        if (req.type == 2) out = SearchFiles(req.query, req.lens, 48);
        else {
            out = RecentFiles(40);
            if (req.type == 1) {
                auto dl = DownloadFiles(40);
                out.insert(out.end(), dl.begin(), dl.end());
            }
        }
        g_fileLock.lock();
        if (req.gen == g_fileGen) {
            g_fileResults = std::move(out);
            g_fileResultsGen = req.gen;
        }
        g_fileLock.unlock();
        if (g_host) PostMessageW(g_host, WM_APP_FILES, req.gen, 0);
    }
    winrt::uninit_apartment();
    return 0;
}

// what the current lens + query needs from the file worker (nothing for the Apps lens)
static void RequestFiles() {
    std::wstring q = Trim(g_query);
    int type = -1;
    if (g_lens == LENS_HOME) type = q.empty() ? 0 : 2;
    else if (g_lens == LENS_FILES) type = q.empty() ? 1 : 2;
    else if (g_lens >= LENS_VIDEO) type = 2;
    g_fileLock.lock();
    g_fileGen++;
    if (type >= 0) {
        g_fileReq = {g_fileGen, type, g_lens, q};
        g_fileReqPending = true;
    }
    g_fileLock.unlock();
    if (type >= 0) SetEvent(g_fileWake);
}

// ---------------- widgets: system (CPU / GPU / RAM), disks, weather, now playing ----------------

static PDH_HQUERY g_pdh;
static PDH_HCOUNTER g_cpuCounter, g_gpuCounter;
static float g_cpu, g_gpu, g_ramUsed, g_ramTotal;
static std::vector<float> g_cpuHist, g_gpuHist, g_ramHist;

static void PushHist(std::vector<float>& h, float v) {
    h.push_back(v);
    if (h.size() > 40) h.erase(h.begin());
}

static void SampleStats() {
    if (!g_pdh && PdhOpenQueryW(nullptr, 0, &g_pdh) == ERROR_SUCCESS) {
        PdhAddEnglishCounterW(g_pdh, L"\\Processor(_Total)\\% Processor Time", 0, &g_cpuCounter);
        PdhAddEnglishCounterW(g_pdh, L"\\GPU Engine(*engtype_3D)\\Utilization Percentage", 0, &g_gpuCounter);
        PdhCollectQueryData(g_pdh);
    }
    if (g_pdh && PdhCollectQueryData(g_pdh) == ERROR_SUCCESS) {
        PDH_FMT_COUNTERVALUE v;
        if (g_cpuCounter && PdhGetFormattedCounterValue(g_cpuCounter, PDH_FMT_DOUBLE, nullptr, &v) == ERROR_SUCCESS)
            g_cpu = (float)std::clamp(v.doubleValue, 0.0, 100.0);
        if (g_gpuCounter) {
            DWORD size = 0, count = 0;
            PdhGetFormattedCounterArrayW(g_gpuCounter, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, &size, &count, nullptr);
            if (size) {
                std::vector<BYTE> buf(size);
                auto* items = (PDH_FMT_COUNTERVALUE_ITEM_W*)buf.data();
                if (PdhGetFormattedCounterArrayW(g_gpuCounter, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, &size, &count, items) == ERROR_SUCCESS) {
                    double sum = 0;
                    for (DWORD i = 0; i < count; i++)
                        if (items[i].FmtValue.CStatus == ERROR_SUCCESS) sum += items[i].FmtValue.doubleValue;
                    g_gpu = (float)std::min(100.0, sum);
                }
            }
        }
    }
    MEMORYSTATUSEX m = {sizeof(m)};
    if (GlobalMemoryStatusEx(&m)) {
        g_ramTotal = m.ullTotalPhys / 1073741824.f;
        g_ramUsed = (m.ullTotalPhys - m.ullAvailPhys) / 1073741824.f;
    }
    PushHist(g_cpuHist, g_cpu / 100.f);
    PushHist(g_gpuHist, g_gpu / 100.f);
    PushHist(g_ramHist, g_ramTotal > 0 ? g_ramUsed / g_ramTotal : 0);
}

// now playing: Windows' media controls (Spotify, browsers, ...) through WinRT, on its own thread

static IWICBitmapSource* DecodeImage(const BYTE* data, UINT size) {
    IStream* stream = SHCreateMemStream(data, size);
    if (!stream) return nullptr;
    IWICBitmapDecoder* dec = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* conv = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    IWICBitmap* bmp = nullptr;
    if (SUCCEEDED(g_wic->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnLoad, &dec)) && SUCCEEDED(dec->GetFrame(0, &frame)) &&
        SUCCEEDED(g_wic->CreateFormatConverter(&conv)) &&
        SUCCEEDED(conv->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom)) &&
        SUCCEEDED(g_wic->CreateBitmapScaler(&scaler)) && SUCCEEDED(scaler->Initialize(conv, 180, 180, WICBitmapInterpolationModeFant)))
        g_wic->CreateBitmapFromSource(scaler, WICBitmapCacheOnLoad, &bmp);
    Release(scaler);
    Release(conv);
    Release(frame);
    Release(dec);
    stream->Release();
    return bmp;
}

static DWORD WINAPI MediaWorker(LPVOID) {
    using namespace winrt::Windows::Media::Control;
    try { winrt::init_apartment(winrt::apartment_type::multi_threaded); } catch (...) {}
    GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
    try { mgr = Await(GlobalSystemMediaTransportControlsSessionManager::RequestAsync(), 3000); } catch (...) {}
    std::wstring lastKey;
    while (!g_workersStop) {
        WaitForSingleObject(g_mediaWake, 1500);
        if (g_workersStop || !mgr) break;
        LONG cmd = InterlockedExchange(&g_mediaCmd, 0);
        try {
            GlobalSystemMediaTransportControlsSession session{nullptr};
            for (auto const& x : mgr.GetSessions()) {
                std::wstring id = Lower(std::wstring(x.SourceAppUserModelId()));
                if (id.find(L"spotify") != std::wstring::npos) { session = x; break; }
                if (!session && x.GetPlaybackInfo().PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) session = x;
            }
            if (!session) session = mgr.GetCurrentSession();
            if (session && cmd == 1) Await(session.TrySkipPreviousAsync(), 1500);
            if (session && cmd == 2) Await(session.TryTogglePlayPauseAsync(), 1500);
            if (session && cmd == 3) Await(session.TrySkipNextAsync(), 1500);
            if (!session) {
                g_mediaLock.lock();
                g_mHas = false;
                g_mPlaying = false;
                g_mediaLock.unlock();
                lastKey.clear();
                if (g_host) PostMessageW(g_host, WM_APP_MEDIA, 0, 0);
                continue;
            }
            bool playing = session.GetPlaybackInfo().PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
            auto props = Await(session.TryGetMediaPropertiesAsync(), 1500);
            std::wstring title(props.Title()), artist(props.Artist());
            IWICBitmapSource* cover = nullptr;
            bool newCover = title + L"|" + artist != lastKey;
            if (newCover) {
                lastKey = title + L"|" + artist;
                if (auto thumb = props.Thumbnail()) {
                    auto stream = Await(thumb.OpenReadAsync(), 1500);
                    UINT size = (UINT)std::min<uint64_t>(stream.Size(), 8u << 20);
                    winrt::Windows::Storage::Streams::Buffer buf(size);
                    auto got = Await(stream.ReadAsync(buf, size, winrt::Windows::Storage::Streams::InputStreamOptions::None), 1500);
                    cover = DecodeImage(got.data(), got.Length());
                }
            }
            std::wstring source(session.SourceAppUserModelId());
            g_mediaLock.lock();
            g_mHas = !title.empty();
            g_mSource = source;
            g_mTitle = title;
            g_mArtist = artist;
            g_mPlaying = playing;
            if (newCover) {
                Release(g_mCover);
                g_mCover = cover;
                g_mCoverKey++;
            }
            g_mediaLock.unlock();
            if (g_host) PostMessageW(g_host, WM_APP_MEDIA, 0, 0);
        } catch (...) {
        }
    }
    mgr = nullptr;
    winrt::uninit_apartment();
    return 0;
}

static void MediaCommand(LONG cmd) {
    InterlockedExchange(&g_mediaCmd, cmd);
    SetEvent(g_mediaWake);
}

static void StartWorkers() {
    g_mediaReady = true;
    g_workersStop = false;
    g_fileWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_mediaWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_fileThread = CreateThread(nullptr, 0, FileWorker, nullptr, 0, nullptr);
    g_mediaThread = CreateThread(nullptr, 0, MediaWorker, nullptr, 0, nullptr);
}

static void StopWorkers() {
    g_workersStop = true;
    SetEvent(g_fileWake);
    SetEvent(g_mediaWake);
    for (HANDLE* t : {&g_fileThread, &g_mediaThread})
        if (*t) { WaitForSingleObject(*t, INFINITE); CloseHandle(*t); *t = nullptr; }   // their calls are bounded: they always end
    CloseHandle(g_fileWake);
    CloseHandle(g_mediaWake);
    Release(g_mCover);
    if (g_pdh) { PdhCloseQuery(g_pdh); g_pdh = nullptr; }
}

// ---------------- building the sections for the current lens and query ----------------

static int Score(const std::wstring& nameLower, const std::wstring& q) {
    if (q.empty()) return 1;
    if (StartsWith(nameLower, q)) return 3;
    if (nameLower.find(L" " + q) != std::wstring::npos) return 2;
    if (nameLower.find(q) != std::wstring::npos) return 1;
    return 0;
}

static Result AppResult(const App& a) {
    return {a.name, std::wstring(kAppsFolder) + a.id, RK_APP};
}

static std::vector<Result> RankedApps(const std::wstring& q, size_t max) {
    std::vector<std::pair<int, int>> scored;
    for (int i = 0; i < (int)g_apps.size(); i++)
        if (int s = Score(g_apps[i].nameLower, q)) scored.push_back({-s, i});
    std::stable_sort(scored.begin(), scored.end());
    std::vector<Result> out;
    for (auto& p : scored) {
        if (out.size() >= max) break;
        out.push_back(AppResult(g_apps[p.second]));
    }
    return out;
}

static std::vector<Result> RecentAppResults() {
    std::vector<Result> out;
    for (auto& launch : g_recentApps) out.push_back({NameFor(launch), launch, RK_APP});
    return out;
}

// ---------------- kinds of apps (the app's Apps lens groups them, and filters on them) ----------------

struct AppKind { const wchar_t *name, *glyph, *words; };
static const AppKind kAppKinds[] = {
    {L"Games", L"\uE7FC", L"steam|battle.net|epic games|ea app|ea desktop|ubisoft|gog galaxy|minecraft|curseforge|crossout|riot|xbox|game|roblox|lutris|playnite"},
    {L"Internet", L"\uE774", L"brave|chrome|firefox|edge|opera|vivaldi|discord|teams|whatsapp|telegram|zoom|outlook|mail|thunderbird|skype|slack|chatgpt|claude"},
    {L"Media", L"\uE8B2", L"spotify|vlc|media|music|video|player|crunchyroll|netflix|obs|audacity|camera|clipchamp|foto|photos|movies|itunes|plex"},
    {L"Grafisch", L"\uE790", L"blender|photoshop|gimp|krita|paint|inkscape|affinity|figma|illustrator|aseprite|substance|3d|image"},
    {L"Ontwikkeling", L"\uE943", L"visual studio|code|godot|git|python|node|terminal|powershell|command prompt|java|unity|android studio|cmake|developer|sdk|wsl|ubuntu"},
    {L"Kantoor", L"\uE8A5", L"word|excel|powerpoint|onenote|libreoffice|pdf|acrobat|microsoft access|publisher|notion|obsidian"},
    {L"Bureau-accessoires", L"\uE8EF", L"calculator|rekenmachine|notepad|kladblok|snipping|knipprogramma|clock|klok|alarm|sticky|character map|wordpad|7-zip|winrar|kalender|calendar"},
    {L"Aanpassen", L"\uE771", L"windhawk|wallpaper|powertoys|rainmeter|start11|stardock|unity"},
    {L"Overig", L"\uE74C", L""},
    {L"Systeem", L"\uE770", L"settings|instellingen|control panel|configuratiescherm|task manager|taakbeheer|management|beheer|disk|nvidia|amd|intel|core temp|cpu-z|hwmonitor|asrock|msi|equalizer|benchmark|registry|services|tools|explorer|verkenner|defender|beveiliging|recovery|driver|update|uefi|recycle"},
};
static const int kOtherKind = 8;

// guessed from its name and id, like the app (the first kind whose words match; "Overig" when none does)
static int KindOf(const App& a);

// a new group's name: the kind both apps are (Games, Internet, ...), else "Groep" (the app's)
static std::wstring GroupNameFor(const std::wstring& a, const std::wstring& b) {
    auto kindOf = [](const std::wstring& launch) {
        for (auto& app : g_apps)
            if (_wcsicmp((std::wstring(kAppsFolder) + app.id).c_str(), launch.c_str()) == 0) return KindOf(app);
        return kOtherKind;
    };
    int ka = kindOf(a), kb = kindOf(b);
    return ka == kb && ka != kOtherKind ? Tr(kAppKinds[ka].name) : Tr(L"Groep");
}

static int KindOf(const App& a) {
    std::wstring hay = a.nameLower + L" " + Lower(a.id);
    if (hay.find(L"steam://") != std::wstring::npos || hay.find(L"\\steamapps\\") != std::wstring::npos || hay.find(L"\\epic games\\") != std::wstring::npos) return 0;
    for (int k = 0; k < (int)ARRAYSIZE(kAppKinds); k++) {
        std::wstring words = kAppKinds[k].words;
        size_t start = 0;
        while (!words.empty() && start <= words.size()) {
            size_t bar = words.find(L'|', start);
            std::wstring w = words.substr(start, bar == std::wstring::npos ? std::wstring::npos : bar - start);
            // at the start of a word only ("msteams" is not Steam)
            for (size_t at = w.empty() ? std::wstring::npos : hay.find(w); at != std::wstring::npos; at = hay.find(w, at + 1))
                if (at == 0 || !iswalpha(hay[at - 1])) return k;
            if (bar == std::wstring::npos) break;
            start = bar + 1;
        }
    }
    return kOtherKind;
}

static bool IsStoreApp(const std::wstring& launch) { return launch.find(L'!') != std::wstring::npos; }

// ---------------- the "Filter results" sidebar (Unity's): chips per lens, all off = everything ----------------

static bool g_filtersOpen;
static std::map<std::wstring, bool> g_filterOn, g_filterShut;   // "lens/group/value" chosen, "lens/group" folded
static const float kFilterW = 330;

struct FilterGroup {
    std::wstring name;
    std::vector<std::wstring> values;
    bool single = false;   // one at a time (dates)
};

static std::vector<FilterGroup> LensFilters() {
    std::vector<FilterGroup> g;
    if (g_lens == LENS_HOME) {
        g.push_back({L"Categorieën", {L"Applicaties", L"Instellingen", L"Bestanden en mappen"}});
        g.push_back({L"Bronnen", {L"Desktop-apps", L"Store-apps", L"Instellingen", L"Windows Search"}});
    }
    if (g_lens == LENS_APPS) {
        FilterGroup kinds{L"Type", {}};
        for (auto& k : kAppKinds) kinds.values.push_back(k.name);
        g.push_back(kinds);
        g.push_back({L"Bronnen", {L"Desktop-apps", L"Store-apps"}});
    }
    if (g_lens == LENS_FILES || g_lens == LENS_VIDEO || g_lens == LENS_MUSIC || g_lens == LENS_PHOTOS)
        g.push_back({L"Laatst gewijzigd", {L"Afgelopen 7 dagen", L"Afgelopen 30 dagen", L"Afgelopen jaar"}, true});
    if (g_lens == LENS_FILES)
        g.push_back({L"Type", {L"Documenten", L"Mappen", L"Afbeeldingen", L"Audio", L"Video's", L"Presentaties", L"Overig"}});
    return g;
}

static std::wstring FilterKey(const std::wstring& group) { return std::to_wstring(g_lens) + L"/" + group; }

static bool AnyFilter(const std::wstring& group) {
    std::wstring prefix = FilterKey(group) + L"/";
    for (auto& kv : g_filterOn)
        if (kv.second && StartsWith(kv.first, prefix)) return true;
    return false;
}

static bool Wants(const std::wstring& group, const std::wstring& value) {
    if (!AnyFilter(group)) return true;
    auto it = g_filterOn.find(FilterKey(group) + L"/" + value);
    return it != g_filterOn.end() && it->second;
}

static void ToggleFilter(const FilterGroup& g, const std::wstring& value) {
    std::wstring key = FilterKey(g.name) + L"/" + value;
    bool was = g_filterOn[key];
    if (g.single)
        for (auto& v : g.values) g_filterOn[FilterKey(g.name) + L"/" + v] = false;
    g_filterOn[key] = !was;
}

static bool WantsApp(const std::wstring& launch) {
    return Wants(L"Bronnen", IsStoreApp(launch) ? L"Store-apps" : L"Desktop-apps");
}

// a file's kind for the Type filter, from its extension
// its kind, as the Type filter's key (Dutch, like all filter keys; shown through Tr)
static std::wstring FileKind(const std::wstring& path) {
    DWORD attr = GetFileAttributesW(path.c_str());
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) return L"Mappen";
    size_t dot = path.find_last_of(L'.');
    std::wstring ext = dot == std::wstring::npos ? L"" : Lower(path.substr(dot)) + L".";
    auto in = [&](const wchar_t* list) { return !ext.empty() && std::wstring(list).find(ext) != std::wstring::npos; };
    if (in(L".doc.docx.pdf.txt.rtf.odt.xls.xlsx.csv.md.ods.")) return L"Documenten";
    if (in(L".png.jpg.jpeg.gif.bmp.webp.heic.tif.tiff.svg.ico.")) return L"Afbeeldingen";
    if (in(L".mp3.wav.flac.ogg.m4a.aac.wma.opus.")) return L"Audio";
    if (in(L".mp4.mkv.avi.mov.wmv.webm.m4v.")) return L"Video's";
    if (in(L".ppt.pptx.odp.key.")) return L"Presentaties";
    return L"Overig";
}

// how many days ago a file was last changed (cached: the lists come back on every keystroke)
static double FileAgeDays(const std::wstring& path) {
    static std::map<std::wstring, ULONGLONG> written;
    auto it = written.find(path);
    ULONGLONG t = 0;
    if (it != written.end()) t = it->second;
    else {
        WIN32_FILE_ATTRIBUTE_DATA d;
        if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &d)) t = ((ULONGLONG)d.ftLastWriteTime.dwHighDateTime << 32) | d.ftLastWriteTime.dwLowDateTime;
        if (written.size() > 5000) written.clear();
        written[path] = t;
    }
    if (!t) return 1e9;
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    ULONGLONG n = ((ULONGLONG)now.dwHighDateTime << 32) | now.dwLowDateTime;
    return n > t ? (n - t) / 864000000000.0 : 0;
}

static std::vector<Result> FilterFiles(const std::vector<Result>& files) {
    std::vector<Result> out;
    bool byDate = AnyFilter(L"Laatst gewijzigd"), byType = g_lens == LENS_FILES && AnyFilter(L"Type");
    for (auto& f : files) {
        if (byDate) {
            double age = FileAgeDays(f.open);
            if (!(Wants(L"Laatst gewijzigd", L"Afgelopen 7 dagen") && age <= 7) && !(Wants(L"Laatst gewijzigd", L"Afgelopen 30 dagen") && age <= 30) &&
                !(Wants(L"Laatst gewijzigd", L"Afgelopen jaar") && age <= 365))
                continue;
        }
        if (byType && !Wants(L"Type", FileKind(f.open))) continue;
        out.push_back(f);
    }
    return out;
}

static bool g_filesPending;

static std::vector<Result> OnlyWanted(const std::vector<Result>& apps) {
    std::vector<Result> out;
    for (auto& r : apps)
        if (WantsApp(r.open)) out.push_back(r);
    return out;
}

static void BuildSections() {
    g_sections.clear();
    std::wstring q = Lower(Trim(g_query));
    std::vector<Result> files;
    g_fileLock.lock();
    bool current = g_fileResultsGen == g_fileGen;
    if (current) files = g_fileResults;
    g_fileLock.unlock();
    g_filesPending = !current && g_lens != LENS_APPS;
    std::vector<Result> group0, group1;
    for (auto& f : files) (f.group == 1 ? group1 : group0).push_back(f);

    group0 = FilterFiles(group0);
    group1 = FilterFiles(group1);
    bool wantApps = Wants(L"Categorieën", L"Applicaties"), wantFiles = Wants(L"Categorieën", L"Bestanden en mappen") && Wants(L"Bronnen", L"Windows Search");
    switch (g_lens) {
        case LENS_HOME:
            if (q.empty()) {
                auto recent = RecentAppResults();
                if (recent.empty()) recent = RankedApps(L"", 12);
                if (wantApps) g_sections.push_back({Tr(L"Applicaties"), L"\uE71D", OnlyWanted(recent), 1});
                if (wantFiles) g_sections.push_back({Tr(L"Recente bestanden"), L"\uE8B7", group0, 1});
            } else {
                if (wantApps) g_sections.push_back({Tr(L"Applicaties"), L"\uE71D", OnlyWanted(RankedApps(q, 60)), 2});
                if (Wants(L"Categorieën", L"Instellingen") && Wants(L"Bronnen", L"Instellingen")) g_sections.push_back({Tr(L"Instellingen"), L"\uE713", FindSettings(q), 1});
                if (wantFiles) g_sections.push_back({Tr(L"Bestanden en mappen"), L"\uE8B7", group0, 2});
            }
            break;
        case LENS_APPS:
            if (q.empty()) {
                // browsing: recently used, then one group per kind of app (system tools last)
                if (!AnyFilter(L"Type") && !AnyFilter(L"Bronnen")) g_sections.push_back({Tr(L"Recent gebruikt"), L"\uE81C", RecentAppResults(), 1});
                std::vector<Result> byKind[ARRAYSIZE(kAppKinds)];
                for (auto& a : g_apps) {
                    int k = KindOf(a);
                    Result r = AppResult(a);
                    if (Wants(L"Type", kAppKinds[k].name) && WantsApp(r.open)) byKind[k].push_back(r);
                }
                for (int k = 0; k < (int)ARRAYSIZE(kAppKinds); k++)
                    g_sections.push_back({Tr(kAppKinds[k].name), kAppKinds[k].glyph, byKind[k], k == ARRAYSIZE(kAppKinds) - 1 ? 1 : 2});
            } else {
                std::vector<Result> found;
                for (auto& r : RankedApps(q, 200))
                    if (WantsApp(r.open)) {
                        bool kindOk = true;
                        for (auto& a : g_apps)
                            if (std::wstring(kAppsFolder) + a.id == r.open) kindOk = Wants(L"Type", kAppKinds[KindOf(a)].name);
                        if (kindOk) found.push_back(r);
                    }
                g_sections.push_back({Tr(L"Applicaties"), L"\uE71D", found, 100});
                g_sections.push_back({Tr(L"Instellingen"), L"\uE713", FindSettings(q), 1});
            }
            break;
        case LENS_FILES:
            if (q.empty()) {
                g_sections.push_back({L"Recent", L"\uE81C", group0, 2});
                g_sections.push_back({L"Downloads", L"\uE896", group1, 2});
            } else {
                g_sections.push_back({Tr(L"Bestanden en mappen"), L"\uE8B7", group0, 100});
            }
            break;
        default:
            g_sections.push_back({g_lens == LENS_VIDEO ? Tr(L"Video's") : g_lens == LENS_MUSIC ? Tr(L"Muziek") : Tr(L"Foto's"), kLensGlyph[g_lens], group0, 100});
            break;
    }
    g_sections.erase(std::remove_if(g_sections.begin(), g_sections.end(), [](const Section& s) { return s.items.empty(); }), g_sections.end());
}

static void QueryChanged() {
    BuildSections();
    g_sel = 0;
    g_scroll = 0;
}

// ---------------- drawing ----------------

static int DashCols() {
    return std::max(1, (int)((DashW() - (g_filtersOpen ? kFilterW : 0) - 2 * kMargin) / kTileW));
}

static bool ShowWidgets() {
    return g_set.showWidgets && g_lens == LENS_HOME && Trim(g_query).empty();
}

// Windows' Weather app is there (it registers the bingweather: link)
static bool g_weatherApp;   // looked up when the Dash opens (see ShowDash)
static bool WeatherAppInstalled() { return g_weatherApp; }
static bool FindWeatherApp() {
    HKEY k;
    if (RegOpenKeyExW(HKEY_CLASSES_ROOT, L"bingweather", 0, KEY_READ, &k) != ERROR_SUCCESS) return false;
    RegCloseKey(k);
    return true;
}

static void Card(Surface& s, D2D1_RECT_F r, const wchar_t* glyph, const wchar_t* title) {
    s.rt->FillRoundedRectangle(D2D1::RoundedRect(r, 12, 12), s.White(0.07f));
    s.rt->DrawRoundedRectangle(D2D1::RoundedRect(r, 12, 12), s.White(0.12f), 1);
    Glyph(s, glyph, D2D1::RectF(r.left + 10, r.top + 8, r.left + 26, r.top + 24), 11, 0.8f);
    Text(s, title, D2D1::RectF(r.left + 30, r.top + 8, r.right - 10, r.top + 24), 11.5f, 0.65f);
}

static void Sparkline(Surface& s, const std::vector<float>& h, D2D1_RECT_F r) {
    if (h.size() < 2) return;
    ID2D1PathGeometry* line = nullptr;
    g_d2d->CreatePathGeometry(&line);
    ID2D1GeometrySink* sink = nullptr;
    if (line && SUCCEEDED(line->Open(&sink))) {
        float w = r.right - r.left, hh = r.bottom - r.top;
        size_t n = h.size();
        for (size_t i = 0; i < n; i++) {
            D2D1_POINT_2F p = D2D1::Point2F(r.right - (n - 1 - i) * w / 39, r.bottom - 1 - (hh - 2) * std::clamp(h[i], 0.f, 1.f));
            if (i == 0) sink->BeginFigure(p, D2D1_FIGURE_BEGIN_HOLLOW);
            else sink->AddLine(p);
        }
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();
        s.rt->DrawGeometry(line, s.Brush(Shade(g_accent, 1.6f), 1), 1.5f);
    }
    Release(sink);
    Release(line);
}

static float DrawWidgets(Surface& s, float y, float W) {
    float colW = (W - 2 * kMargin - 30) / 4, cardH = 150, gap = 10;
    auto cell = [&](int col, int row, int span) {
        float x = kMargin + col * (colW + gap);
        float top = y + row * (cardH + gap);
        return D2D1::RectF(x, top, x + colW * span + gap * (span - 1), top + cardH);
    };

    // today: the time and the date, large
    D2D1_RECT_F wr = cell(0, 0, 1);
    Card(s, wr, L"\uE823", Tr(L"Vandaag"));
    SYSTEMTIME now;
    GetLocalTime(&now);
    wchar_t tm[32] = {}, day[64] = {}, date[64] = {};
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &now, nullptr, tm, 32);
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &now, L"dddd", day, 64, nullptr);
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &now, L"d MMMM yyyy", date, 64, nullptr);
    Text(s, tm, D2D1::RectF(wr.left + 12, wr.top + 34, wr.right - 8, wr.top + 80), 32, 1);
    Text(s, day, D2D1::RectF(wr.left + 12, wr.top + 84, wr.right - 8, wr.top + 104), 13, 0.9f, DWRITE_TEXT_ALIGNMENT_LEADING, false, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD);
    Text(s, date, D2D1::RectF(wr.left + 12, wr.top + 104, wr.right - 8, wr.top + 124), 12, 0.75f);

    // now playing
    D2D1_RECT_F mr = cell(1, 0, 2);
    Card(s, mr, L"\uE8D6", Tr(L"Nu aan het spelen"));
    g_mediaLock.lock();
    bool has = g_mHas, playing = g_mPlaying;
    std::wstring title = g_mTitle, artist = g_mArtist;
    std::wstring coverKey = L"cover#" + std::to_wstring(g_mCoverKey);
    ID2D1Bitmap* cover = nullptr;
    auto cached = s.bitmaps.find(coverKey);
    if (cached != s.bitmaps.end()) cover = cached->second;
    else if (g_mCover) { s.rt->CreateBitmapFromWicBitmap(g_mCover, nullptr, &cover); s.bitmaps[coverKey] = cover; }
    g_mediaLock.unlock();
    D2D1_RECT_F cr = D2D1::RectF(mr.left + 12, mr.top + 32, mr.left + 108, mr.top + 128);
    s.rt->FillRoundedRectangle(D2D1::RoundedRect(cr, 8, 8), s.White(0.08f));
    if (cover && has) s.rt->DrawBitmap(cover, cr, 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    Text(s, has ? title : Tr(L"Er speelt niets"), D2D1::RectF(cr.right + 14, mr.top + 40, mr.right - 10, mr.top + 62), 15, 1, DWRITE_TEXT_ALIGNMENT_LEADING, false, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD);
    Text(s, artist, D2D1::RectF(cr.right + 14, mr.top + 62, mr.right - 10, mr.top + 80), 12, 0.75f);
    const wchar_t* glyphs[3] = {L"\uE892", playing ? L"\uE769" : L"\uE768", L"\uE893"};
    for (int k = 0; k < 3; k++) {
        D2D1_RECT_F b = D2D1::RectF(cr.right + 8 + k * 38, mr.top + 92, cr.right + 42 + k * 38, mr.top + 122);
        int hit = (int)g_hits.size();
        g_hits.push_back({b, HIT_MEDIA, k + 1, 0});
        if (g_hoverHit == hit) s.rt->FillRoundedRectangle(D2D1::RoundedRect(b, 6, 6), s.White(0.15f));
        Glyph(s, glyphs[k], b, 15, 1);
    }

    // disks
    D2D1_RECT_F dr = cell(3, 0, 1);
    Card(s, dr, L"\uEDA2", Tr(L"Opslag"));
    wchar_t drives[256];
    DWORD len = GetLogicalDriveStringsW(255, drives);
    float dy = dr.top + 34;
    for (wchar_t* d = drives; len && *d && dy < dr.bottom - 30; d += wcslen(d) + 1) {
        if (GetDriveTypeW(d) != DRIVE_FIXED) continue;
        ULARGE_INTEGER freeB, total;
        if (!GetDiskFreeSpaceExW(d, nullptr, &total, &freeB) || !total.QuadPart) continue;
        float used = 1 - (float)((double)freeB.QuadPart / total.QuadPart);
        wchar_t label[64];
        swprintf(label, 64, Tr(L"%.2ls  %.0f GB vrij"), d, freeB.QuadPart / 1073741824.0);
        Text(s, label, D2D1::RectF(dr.left + 12, dy, dr.right - 10, dy + 18), 12, 0.9f);
        D2D1_RECT_F bar = D2D1::RectF(dr.left + 12, dy + 20, dr.right - 12, dy + 24);
        s.rt->FillRoundedRectangle(D2D1::RoundedRect(bar, 2, 2), s.White(0.15f));
        bar.right = bar.left + (bar.right - bar.left) * used;
        s.rt->FillRoundedRectangle(D2D1::RoundedRect(bar, 2, 2), used > 0.9f ? s.Brush({1, 0.42f, 0.42f}, 1) : s.Brush(Shade(g_accent, 1.5f), 1));
        dy += 34;
    }

    // CPU / GPU / RAM
    D2D1_RECT_F sr = cell(0, 1, 2);
    Card(s, sr, L"\uE9D9", Tr(L"Systeem"));
    wchar_t v[32];
    struct Row { const wchar_t* label; float value; const std::vector<float>* hist; bool ram; } rows[3] = {
        {L"CPU", g_cpu, &g_cpuHist, false}, {L"GPU", g_gpu, &g_gpuHist, false}, {L"RAM", g_ramUsed, &g_ramHist, true}};
    for (int k = 0; k < 3; k++) {
        float ry = sr.top + 34 + k * 36;
        Text(s, rows[k].label, D2D1::RectF(sr.left + 12, ry, sr.left + 52, ry + 28), 12.5f, 1, DWRITE_TEXT_ALIGNMENT_LEADING, false, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD);
        if (rows[k].ram) swprintf(v, 32, L"%.1f GB", rows[k].value);
        else swprintf(v, 32, L"%d%%", (int)lroundf(rows[k].value));
        Text(s, v, D2D1::RectF(sr.left + 52, ry, sr.left + 122, ry + 28), 12.5f, 0.85f);
        Sparkline(s, *rows[k].hist, D2D1::RectF(sr.left + 126, ry + 4, sr.right - 14, ry + 26));
    }
    // the weather: Windows' own Weather app (the mod itself fetches nothing from the internet)
    D2D1_RECT_F wa = cell(2, 1, 1);
    int weatherHit = (int)g_hits.size();
    g_hits.push_back({wa, HIT_WEATHER, 0, 0});
    if (g_hoverHit == weatherHit) s.rt->FillRoundedRectangle(D2D1::RoundedRect(wa, 10, 10), s.White(0.06f));
    Card(s, wa, L"\uE753", Tr(L"Weer"));
    Text(s, L"⛅", D2D1::RectF(wa.left + 10, wa.top + 34, wa.right - 10, wa.top + 92), 38, 1, DWRITE_TEXT_ALIGNMENT_CENTER, false, L"Segoe UI Emoji");
    Text(s, WeatherAppInstalled() ? Tr(L"Open de Weer-app") : Tr(L"Weer-app installeren"), D2D1::RectF(wa.left + 12, wa.top + 100, wa.right - 10, wa.top + 122), 12, 0.8f, DWRITE_TEXT_ALIGNMENT_CENTER);
    return y + 2 * (cardH + gap);
}

static Rgb BandColour() { return Shade(g_tint, 0.5f); }
// the blur fills a window's whole rectangle, so the Dash's bottom row goes to two windows of its own: the foot
// (blurred) and the rounded tip (not blurred), which keeps the blur from showing outside the round corner
static HWND g_dashFoot, g_dashTip;
static float BandAlpha() { return std::min(1.f, g_set.dashOpacity / 100.f + 30 / 255.f); }

static void RenderDash() {
    if (!g_dash || !IsWindowVisible(g_dash)) return;
    Surface& s = g_dashS;
    // the window holds the content plus Unity's band on the right and bottom
    if (!s.Begin(Px(DashW() + Frame()), Px(DashH() + Frame()))) return;
    float W = DashW(), H = DashH(), WW = W + Frame(), HH = H + Frame(), R = kBendR;
    bool band = !g_dashMax;
    float a = g_set.dashOpacity / 100.f;
    // the zoom out of the top-left corner (near the Dash button) while it opens and closes
    float z = g_aZoom.value > 0 ? g_aZoom.value : 1;
    s.rt->SetTransform(D2D1::Matrix3x2F::Scale(z, z, D2D1::Point2F(0, 0)));

    // the band: darker and a bit more see-through than the content, rounded at the bottom-right
    if (auto* bandPath = !band ? nullptr : Path([&](ID2D1GeometrySink* k) {
            k->BeginFigure(D2D1::Point2F(0, 0), D2D1_FIGURE_BEGIN_FILLED);
            k->AddLine(D2D1::Point2F(WW, 0));
            k->AddLine(D2D1::Point2F(WW, HH - R));
            k->AddArc(Arc(WW - R, HH, R, true));
            k->AddLine(D2D1::Point2F(0, HH));
            k->EndFigure(D2D1_FIGURE_END_CLOSED);
        })) {
        s.rt->FillGeometry(bandPath, s.Brush(BandColour(), BandAlpha()));
        bandPath->Release();
    }
    // the content, with its thin light line on the right
    s.rt->FillRectangle(D2D1::RectF(0, 0, W, H), s.Brush(Shade(g_tint, 0.75f), a));
    if (band) s.rt->FillRectangle(D2D1::RectF(W - 1, 0, W, H), s.White(0x40 / 255.f));
    // the band's top-left gets a round inside corner in the content colour (Unity's upper bend)
    if (auto* corner = !band ? nullptr : Path([&](ID2D1GeometrySink* k) {
            k->BeginFigure(D2D1::Point2F(0, H), D2D1_FIGURE_BEGIN_FILLED);
            k->AddLine(D2D1::Point2F(R, H));
            k->AddArc(Arc(0, H + R, R, false));
            k->EndFigure(D2D1_FIGURE_END_CLOSED);
        })) {
        s.rt->FillGeometry(corner, s.Brush(Shade(g_tint, 0.75f), a));
        corner->Release();
    }
    g_hits.clear();
    g_flat.clear();

    // the search box (the app's: dark, rounded, a light rim) and "Filter results" on the right
    float CW = W - (g_filtersOpen ? kFilterW : 0);   // the results' width, beside the sidebar
    std::wstring filterLabel = g_filtersOpen ? Tr(L"Resultaten filteren  ▾") : Tr(L"Resultaten filteren  ▸");
    float filterW = TextWidth(filterLabel, 17) + 4;
    D2D1_RECT_F sb = D2D1::RectF(10, 10, 10 + std::min(640.f, W - 28 - filterW - 24), 54);   // up to the filter toggle
    s.rt->FillRoundedRectangle(D2D1::RoundedRect(sb, 10, 10), s.Brush({0, 0, 0}, 0x55 / 255.f));
    s.rt->DrawRoundedRectangle(D2D1::RoundedRect(sb, 10, 10), s.White(0x70 / 255.f), 1);
    Glyph(s, L"\uE721", D2D1::RectF(sb.left + 8, sb.top, sb.left + 34, sb.bottom), 18, 1);
    bool empty = g_query.empty();
    if (empty) {
        IDWriteTextFormat* fmt = nullptr;   // the hint in italics, like the app
        if (SUCCEEDED(g_dw->CreateTextFormat(g_fontFamily.c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_ITALIC, DWRITE_FONT_STRETCH_NORMAL, 19, L"", &fmt))) {
            fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            std::wstring hint = Tr(kLensHint[g_lens]);
            s.rt->DrawText(hint.c_str(), (UINT32)hint.size(), fmt, D2D1::RectF(sb.left + 42, sb.top, sb.right - 10, sb.bottom), s.White(0x99 / 255.f), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            fmt->Release();
        }
    } else Text(s, g_query + L"\u200A|", D2D1::RectF(sb.left + 42, sb.top, sb.right - 10, sb.bottom), 19, 1);
    {
        D2D1_RECT_F fr = D2D1::RectF(W - 18 - filterW, sb.top, W - 18, sb.bottom);
        int hit = (int)g_hits.size();
        g_hits.push_back({fr, HIT_FILTERS, 0, 0});
        Text(s, filterLabel, fr, 17, g_hoverHit == hit ? 1.f : 0.92f, DWRITE_TEXT_ALIGNMENT_TRAILING);
    }

    // sections
    float top = kMargin + kSearchH + 8, bottom = H - kLensBarH;
    s.rt->PushAxisAlignedClip(D2D1::RectF(0, top, CW, bottom), D2D1_ANTIALIAS_MODE_ALIASED);
    float y = top - 6 - g_scroll;
    int cols = DashCols();
    int selSection = g_sel >= 0 && g_sel < (int)g_flat.size() ? g_flat[g_sel].section : -1;
    // coming in: each row (a section's heading, a line of tiles, the widgets) fades in and rises, 22 ms apart
    double since = NowMs() - g_rowsStart;
    ID2D1Layer* rowLayer = nullptr;
    if (since < kRowsMs) s.rt->CreateLayer(&rowLayer);
    D2D1_MATRIX_3X2_F base;
    s.rt->GetTransform(&base);
    auto enter = [&](int row) {
        if (!rowLayer) return;
        float t = (float)std::clamp((since - std::min(row, 13) * 22) / 220.0, 0.0, 1.0), e = 1 - powf(1 - t, 3);
        s.rt->SetTransform(D2D1::Matrix3x2F::Translation(0, 10 * (1 - e)) * base);
        s.rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), e), rowLayer);
    };
    auto leave = [&]() {
        if (!rowLayer) return;
        s.rt->PopLayer();
        s.rt->SetTransform(base);
    };
    int row = 0;
    Rgb glyphColour = Shade(g_accent, 1.5f);
    // the app's section header: icon, title and a count on the left, "show all" on the right, a thin line under it
    auto header = [&](const wchar_t* glyph, const std::wstring& title, int count, bool lit) {
        y += 14;
        D2D1_RECT_F band = D2D1::RectF(10, y, CW - 8, y + 34);
        if (lit) s.rt->FillRoundedRectangle(D2D1::RoundedRect(band, 8, 8), s.White(0x12 / 255.f));
        s.rt->FillRectangle(D2D1::RectF(band.left + 4, band.bottom - 1, band.right - 4, band.bottom), s.White(0x18 / 255.f));
        Glyph(s, glyph, D2D1::RectF(band.left + 10, band.top, band.left + 30, band.bottom), 16, 1, glyphColour);
        float tw = TextWidth(title, 16, DWRITE_FONT_WEIGHT_SEMI_BOLD);
        Text(s, title, D2D1::RectF(band.left + 40, band.top, band.left + 40 + tw + 4, band.bottom), 16, 1, DWRITE_TEXT_ALIGNMENT_LEADING, false, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD);
        if (count >= 0) {
            std::wstring n = std::to_wstring(count);
            float cx = band.left + 40 + tw + 12, cw = TextWidth(n, 11) + 14, cy = (band.top + band.bottom) / 2;
            D2D1_RECT_F pill = D2D1::RectF(cx, cy - 9, cx + cw, cy + 9);
            s.rt->FillRoundedRectangle(D2D1::RoundedRect(pill, 9, 9), s.White(0x1E / 255.f));
            Text(s, n, pill, 11, 0xC8 / 255.f, DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        return band;
    };
    for (int si = 0; si < (int)g_sections.size(); si++) {
        auto& sec = g_sections[si];
        std::wstring key = std::to_wstring(g_lens) + L"/" + sec.title;
        bool open = g_expanded[key];
        int limit = open ? (int)sec.items.size() : std::min((int)sec.items.size(), sec.rows * cols);
        enter(row);
        D2D1_RECT_F band = header(sec.glyph, sec.title, (int)sec.items.size(), si == selSection);
        if ((int)sec.items.size() > sec.rows * cols) {   // "show all" / "show less", a pill on the right
            std::wstring label = open ? Tr(L"Minder tonen") : Tr(L"Alles tonen");
            float pw = TextWidth(label, 12) + 40, cy = (band.top + band.bottom) / 2;
            D2D1_RECT_F mb = D2D1::RectF(band.right - 8 - pw, cy - 12, band.right - 8, cy + 12);
            int hit = (int)g_hits.size();
            g_hits.push_back({mb, HIT_MORE, si, 0});
            s.rt->FillRoundedRectangle(D2D1::RoundedRect(mb, 12, 12), s.White((g_hoverHit == hit ? 0x34 : 0x18) / 255.f));
            Text(s, label, D2D1::RectF(mb.left + 12, mb.top, mb.right - 22, mb.bottom), 12, 1);
            Glyph(s, open ? L"\uE70E" : L"\uE70D", D2D1::RectF(mb.right - 24, mb.top + 1, mb.right - 8, mb.bottom), 10, 1);
        }
        leave();
        row++;
        y = band.bottom + 6;
        for (int k = 0; k < limit; k++) {
            float x = kMargin + (float)(k % cols) * kTileW;
            float ty = y + (float)(k / cols) * kTileH;
            D2D1_RECT_F r = D2D1::RectF(x, ty, x + kTileW - 6, ty + kTileH - 6);
            int flat = (int)g_flat.size();
            g_flat.push_back({si, k, r});
            g_hits.push_back({r, HIT_TILE, si, k});
            if (r.bottom < top || r.top > bottom) continue;
            enter(row + k / cols);
            auto& res = sec.items[k];
            bool sel = flat == g_sel;
            if (sel) {
                s.rt->FillRoundedRectangle(D2D1::RoundedRect(r, 10, 10), s.White(0.16f));
                s.rt->DrawRoundedRectangle(D2D1::RoundedRect(r, 10, 10), s.Brush(Shade(g_accent, 1.4f), 0.85f), 1.5f);
            }
            float cx = (r.left + r.right) / 2;
            if (res.kind == RK_SETTING) {
                Glyph(s, L"\uE713", D2D1::RectF(cx - 28, r.top + 12, cx + 28, r.top + 64), 34, 0.95f);
            } else if (ID2D1Bitmap* bmp = IconBitmap(s, res.open, res.thumb)) {
                D2D1_SIZE_F sz = bmp->GetSize();
                float bw = 52, bh = 52;
                if (res.thumb && sz.width > 0 && sz.height > 0) {   // thumbnails keep their shape
                    float f = std::min(100 / sz.width, 56 / sz.height);
                    bw = sz.width * f;
                    bh = sz.height * f;
                }
                s.rt->DrawBitmap(bmp, D2D1::RectF(cx - bw / 2, r.top + 38 - bh / 2, cx + bw / 2, r.top + 38 + bh / 2), 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
            Text(s, res.name, D2D1::RectF(r.left + 6, r.top + 70, r.right - 6, r.bottom - 2), 12.5f, 0.95f, DWRITE_TEXT_ALIGNMENT_CENTER, true);
            leave();
        }
        row += (limit + cols - 1) / cols;
        y += (float)((limit + cols - 1) / cols) * kTileH;
    }
    if (g_sections.empty()) {
        const wchar_t* msg = g_filesPending ? Tr(L"Zoeken…") : Tr(L"Sorry, er is niets gevonden.");
        Text(s, msg, D2D1::RectF(0, y + 40, CW, y + 70), 16, 0.7f, DWRITE_TEXT_ALIGNMENT_CENTER);
        y += 80;
    }
    if (ShowWidgets()) {
        enter(row++);
        D2D1_RECT_F band = header(L"\uF0E2", L"Widgets", -1, false);
        leave();
        enter(row++);
        y = DrawWidgets(s, band.bottom + 10, CW);
        leave();
    }
    Release(rowLayer);
    g_contentH = y + g_scroll - top + 10;
    s.rt->PopAxisAlignedClip();

    // the filter sidebar: per group a heading (click to fold) and two columns of on/off chips
    if (g_filtersOpen) {
        s.rt->PushAxisAlignedClip(D2D1::RectF(CW, top, W, bottom), D2D1_ANTIALIAS_MODE_ALIASED);
        float slide = g_aFilter.running ? g_aFilter.value : 1;
        ID2D1Layer* slideLayer = nullptr;
        if (slide < 1 && SUCCEEDED(s.rt->CreateLayer(&slideLayer))) {
            s.rt->SetTransform(D2D1::Matrix3x2F::Translation(14 * (1 - slide), 0) * base);
            s.rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), slide), slideLayer);
        }
        float fx = CW + 14, fw = kFilterW - 30, fy = top;
        auto groups = LensFilters();
        if (groups.empty()) Text(s, Tr(L"Geen filters voor deze lens."), D2D1::RectF(fx, fy + 8, fx + fw, fy + 30), 14, 0x99 / 255.f);
        for (int gi = 0; gi < (int)groups.size(); gi++) {
            auto& g = groups[gi];
            bool shut = g_filterShut[FilterKey(g.name)];
            fy += 12;
            D2D1_RECT_F hr = D2D1::RectF(fx, fy, fx + fw, fy + 24);
            int hit = (int)g_hits.size();
            g_hits.push_back({hr, HIT_FGROUP, gi, 0});
            Text(s, std::wstring(Tr(g.name.c_str())) + (shut ? L"  \u25B8" : L"  \u25BE"), hr, 17, g_hoverHit == hit ? 1.f : 0.95f);
            fy += 34;
            if (shut) continue;
            float chipW = (fw - 10) / 2;
            for (int vi = 0; vi < (int)g.values.size(); vi++) {
                float cx = fx + (vi % 2) * (chipW + 10), cy = fy + (vi / 2) * 42;
                D2D1_RECT_F cr = D2D1::RectF(cx, cy, cx + chipW, cy + 30);
                bool on = g_filterOn[FilterKey(g.name) + L"/" + g.values[vi]];
                int chit = (int)g_hits.size();
                g_hits.push_back({cr, HIT_FCHIP, gi, vi});
                s.rt->FillRoundedRectangle(D2D1::RoundedRect(cr, 4, 4), s.White((on ? 0x2C : g_hoverHit == chit ? 0x16 : 0x06) / 255.f));
                s.rt->DrawRoundedRectangle(D2D1::RoundedRect(cr, 4, 4), s.White((on ? 0xF0 : 0x60) / 255.f), on ? 1.6f : 1);
                Text(s, Tr(g.values[vi].c_str()), D2D1::RectF(cr.left + 6, cr.top, cr.right - 6, cr.bottom), 14, 1, DWRITE_TEXT_ALIGNMENT_CENTER);
            }
            fy += ((g.values.size() + 1) / 2) * 42.f;
        }
        if (slideLayer) {
            s.rt->PopLayer();
            s.rt->SetTransform(base);
            slideLayer->Release();
        }
        s.rt->PopAxisAlignedClip();
    }

    // lens bar
    s.rt->FillRectangle(D2D1::RectF(0, H - kLensBarH, W, H), s.Brush({0, 0, 0}, 0.25f));
    s.rt->FillRectangle(D2D1::RectF(0, H - kLensBarH, W, H - kLensBarH + 1), s.White(0.12f));
    float lx = W / 2 - LENS_COUNT * 52 / 2.f;
    for (int l = 0; l < LENS_COUNT; l++) {
        D2D1_RECT_F r = D2D1::RectF(lx + l * 52, H - kLensBarH + 4, lx + l * 52 + 48, H - 4);
        int hit = (int)g_hits.size();
        g_hits.push_back({r, HIT_LENS, l, 0});
        if (g_hoverHit == hit) s.rt->FillRoundedRectangle(D2D1::RoundedRect(r, 6, 6), s.White(0.1f));
        Glyph(s, kLensGlyph[l], r, 17, l == g_lens ? 1.f : 0.55f);
        if (l == g_lens) {   // Unity's little marker above the active lens
            float cx = (r.left + r.right) / 2;
            s.rt->FillRectangle(D2D1::RectF(cx - 3, H - kLensBarH, cx + 3, H - kLensBarH + 3), s.White(0.95f));
        }
    }
    // the resize grip, in the very corner of the band (like the app)
    if (band) {
        D2D1_RECT_F gr = D2D1::RectF(WW - 22, HH - 22, WW - 4, HH - 4);
        int hit = (int)g_hits.size();
        g_hits.push_back({gr, HIT_GRIP, 0, 0});
        float gl = gr.left, gt = gr.top, ga = g_hoverHit == hit || g_resizing ? 0.95f : 0.63f;
        for (float k : {5.f, 9.f, 13.f})
            s.rt->DrawLine(D2D1::Point2F(gl + 16, gt + k), D2D1::Point2F(gl + k, gt + 16), s.White(ga), 1.3f);
    }
    // the band's lines: along its top, bending down at the left into the strip along the launcher...
    if (auto* top = !band ? nullptr : Path([&](ID2D1GeometrySink* k) {
            k->BeginFigure(D2D1::Point2F(W, H + 0.5f), D2D1_FIGURE_BEGIN_HOLLOW);
            k->AddLine(D2D1::Point2F(R, H + 0.5f));
            k->AddArc(Arc(0.5f, H + R, R - 0.5f, false));
            k->AddLine(D2D1::Point2F(0.5f, HH));
            k->EndFigure(D2D1_FIGURE_END_OPEN);
        })) {
        s.rt->DrawGeometry(top, s.White(0x3C / 255.f), 1);
        top->Release();
    }
    // ...and the light rim round the outside: from the bend under the Dash, along the bottom, round the
    // bottom-right corner and straight up the right side
    if (auto* rim = !band ? nullptr : Path([&](ID2D1GeometrySink* k) {
            float w = WW - 0.5f, b = HH - 0.5f;
            k->BeginFigure(D2D1::Point2F(kStrip + R + 0.5f, b), D2D1_FIGURE_BEGIN_HOLLOW);
            k->AddLine(D2D1::Point2F(w - R, b));
            k->AddArc(Arc(w, b - R, R, false));
            k->AddLine(D2D1::Point2F(w, 0));
            k->EndFigure(D2D1_FIGURE_END_OPEN);
        })) {
        s.rt->DrawGeometry(rim, s.White(0x55 / 255.f), 1);
        rim->Release();
    }
    s.rt->SetTransform(D2D1::Matrix3x2F::Identity());
    s.Finish();
    int px = g_screen.left + Px(LauncherW()), py = g_screen.top + Px(PanelH()), r = band ? Px(R) : 0;
    BYTE alpha = (BYTE)std::clamp(g_aDash.value * 255.f, 0.f, 255.f);
    s.Present(g_dash, px, py, 0, 0, s.w, s.h - r, alpha);
    if (band) {
        s.Present(g_dashFoot, px, py + s.h - r, 0, s.h - r, s.w - r, r, alpha);
        s.Present(g_dashTip, px + s.w - r, py + s.h - r, s.w - r, s.h - r, r, r, alpha);
    }
}

// ---------------- the bend and strip under the Dash, along the launcher (two little windows of their own) ----------------

static HWND g_corner, g_strip;
static Surface g_cornerS, g_stripS;

static void RenderEdge() {
    if (!g_corner || !g_strip) return;
    if (!DashShown() || g_dashMax) {
        for (HWND h : {g_corner, g_strip, g_dashFoot, g_dashTip}) ShowWindow(h, SW_HIDE);
        return;
    }
    float R = kBendR, z = g_aZoom.value > 0 ? g_aZoom.value : 1;
    BYTE alpha = (BYTE)std::clamp(g_aDash.value * 255.f, 0.f, 255.f);
    int left = g_screen.left + Px(LauncherW());
    int dashBottom = g_screen.top + Px(PanelH()) + Px((DashH() + kFrame) * z);

    // the bend: the strip's top with a round inside corner where it meets the band, and the rim curving down
    float cw = kStrip + R + 2, ch = R + 1;
    Surface& c = g_cornerS;
    if (c.Begin(Px(cw), Px(ch))) {
        if (auto* fill = Path([&](ID2D1GeometrySink* k) {
                k->BeginFigure(D2D1::Point2F(0, 1), D2D1_FIGURE_BEGIN_FILLED);
                k->AddLine(D2D1::Point2F(kStrip + R, 1));
                k->AddArc(Arc(kStrip, R, R - 1, false));
                k->AddLine(D2D1::Point2F(kStrip, ch));
                k->AddLine(D2D1::Point2F(0, ch));
                k->EndFigure(D2D1_FIGURE_END_CLOSED);
            })) {
            c.rt->FillGeometry(fill, c.Brush(BandColour(), BandAlpha()));
            fill->Release();
        }
        if (auto* rim = Path([&](ID2D1GeometrySink* k) {
                k->BeginFigure(D2D1::Point2F(kStrip + R + 0.5f, 0.5f), D2D1_FIGURE_BEGIN_HOLLOW);
                k->AddArc(Arc(kStrip + 0.5f, R + 0.5f, R, false));
                k->AddLine(D2D1::Point2F(kStrip + 0.5f, ch));
                k->EndFigure(D2D1_FIGURE_END_OPEN);
            })) {
            c.rt->DrawGeometry(rim, c.White(0x55 / 255.f), 1);
            rim->Release();
        }
        c.rt->FillRectangle(D2D1::RectF(0, 0, 1, ch), c.White(0x3C / 255.f));
        c.End(left, dashBottom - Px(1), alpha);
    }

    // the strip: the band's colour (blurred behind, like the band) down to the bottom of the screen
    int stripTop = dashBottom + Px(R);
    int stripH = std::max(1, (int)g_screen.bottom - stripTop);
    Surface& st = g_stripS;
    if (st.Begin(Px(kStrip + 1), stripH)) {
        float hh = (float)(stripH / g_scale);
        st.rt->FillRectangle(D2D1::RectF(0, 0, kStrip + 1, hh), st.Brush(BandColour(), BandAlpha()));
        st.rt->FillRectangle(D2D1::RectF(0, 0, 1, hh), st.White(0x3C / 255.f));
        st.rt->FillRectangle(D2D1::RectF(kStrip, 0, kStrip + 1, hh), st.White(0x55 / 255.f));
        st.End(left, stripTop, alpha);
    }
    for (HWND h : {g_corner, g_strip, g_dashFoot, g_dashTip}) ShowWindow(h, SW_SHOWNA);
}

// maximize / restore: the whole screen next to the launcher, without the band (remembered)
static void SetDashMax(bool on) {
    g_dashMax = on;
    SaveDashSize();
    g_hoverHit = -1;
    RenderDash();
    RenderEdge();
    RenderLauncher();
    RenderPanel();
}

// ---------------- behaviour ----------------

static void SetLens(int lens) {
    g_lens = (lens + LENS_COUNT) % LENS_COUNT;
    QueryChanged();
    RequestFiles();
    StartRowsIn();
    RenderDash();
}

// one step of the opening / closing: the Dash, its bend and strip, the launcher and the panel move together
static void DashFrame() {
    if (g_dashClosing && !g_aDash.running) {
        // fully faded: hide everything (the last picture shown was an empty one, so nothing flashes next time)
        g_dashClosing = false;
        ShowWindow(g_dash, SW_HIDE);
        RenderEdge();
        RenderLauncher();
        RenderPanel();
        return;
    }
    RenderDash();
    RenderEdge();
}

static void ShowDash(bool show) {
    if (!g_dash) return;
    if (!show) {
        if (!g_dashOpen) return;
        g_dashOpen = false;
        g_dashClosing = true;
        KillTimer(g_dash, TIMER_WIDGETS);
        // a quick fade with a hint of zooming back into the corner (Unity)
        g_aDash.Go(0, kCloseMs, EASE_IN_CUBIC);
        g_aZoom.Go(0.97f, kCloseMs, EASE_IN_CUBIC);
        g_aUnder.Go(0, kCloseMs, EASE_LINEAR);
        g_aDim.Go(1, kCloseMs, EASE_LINEAR);
        KickAnim();
        return;
    }
    HideTip();
    HidePreview();
    if (g_apps.empty() || GetTickCount() - g_appsAt > 5 * 60 * 1000) LoadApps();
    g_query.clear();
    g_lens = LENS_HOME;
    g_hoverHit = -1;
    QueryChanged();
    RequestFiles();
    g_dashOpen = true;
    g_weatherApp = FindWeatherApp();
    SampleStats();
    SetEvent(g_mediaWake);
    // Unity fades the Dash in with a slight zoom out of its top-left corner (near the Dash button)
    bool fromHidden = !DashShown();
    g_dashClosing = false;
    g_aDash.Go(1, kOpenMs, EASE_OUT_CUBIC, fromHidden ? 0.f : g_aDash.value);
    g_aZoom.Go(1, kOpenMs + 60, EASE_OUT_QUART, fromHidden ? 0.94f : g_aZoom.value);
    g_aUnder.Go(1, kOpenMs, EASE_OUT_CUBIC);
    g_aDim.Go(1, kOpenMs, EASE_LINEAR, g_aDim.value > 0 ? g_aDim.value : 1.f);   // the icons keep their colours (like the app looks)
    StartRowsIn();
    if (fromHidden) RenderDash();   // the first (invisible) frame goes up before the window shows
    ShowWindow(g_dash, SW_SHOWNA);
    DashFrame();
    SetForegroundWindow(g_dash);
    SetFocus(g_dash);
    SetTimer(g_dash, TIMER_WIDGETS, 1000, nullptr);
    KickAnim();
}

static void ToggleDash() {
    static DWORD lastHide;
    if (g_dashOpen) { ShowDash(false); lastHide = GetTickCount(); return; }
    if (GetTickCount() - lastHide < 300) return;   // the click that just closed it by taking focus away
    ShowDash(true);
}

static void OpenResult(const Result& r, const wchar_t* verb = nullptr) {
    ShowDash(false);
    if (r.kind == RK_APP) RecordRecent(r.open);
    Launch(r.open, verb);
}

static const Result* Selected() {
    if (g_sel < 0 || g_sel >= (int)g_flat.size()) return nullptr;
    auto& f = g_flat[g_sel];
    return &g_sections[f.section].items[f.index];
}

static void EnsureVisible() {
    if (g_sel < 0 || g_sel >= (int)g_flat.size()) return;
    D2D1_RECT_F r = g_flat[g_sel].r;
    float top = kMargin + kSearchH + 12, bottom = DashH() - kLensBarH - 6;
    if (r.top < top) g_scroll -= top - r.top;
    if (r.bottom > bottom) g_scroll += r.bottom - bottom;
    g_scroll = std::max(0.f, g_scroll);
}

static void MoveSel(int dx, int dy) {
    if (g_flat.empty()) return;
    if (dx) g_sel = std::clamp(g_sel + dx, 0, (int)g_flat.size() - 1);
    else {
        // the nearest tile in the row above / below
        D2D1_RECT_F cur = g_flat[std::clamp(g_sel, 0, (int)g_flat.size() - 1)].r;
        int best = -1;
        float bestRow = 0, bestDx = 1e9f;
        for (int i = 0; i < (int)g_flat.size(); i++) {
            float t = g_flat[i].r.top;
            bool ok = dy > 0 ? t > cur.top + 5 : t < cur.top - 5;
            if (!ok) continue;
            float row = dy > 0 ? t : -t;
            float d = fabsf(g_flat[i].r.left - cur.left);
            if (best < 0 || row < bestRow - 1 || (fabsf(row - bestRow) <= 1 && d < bestDx)) { best = i; bestRow = row; bestDx = d; }
        }
        if (best >= 0) g_sel = best;
    }
    RenderDash();   // updates rects
    EnsureVisible();
    RenderDash();
}

static int HitAt(float x, float y) {
    float top = kMargin + kSearchH + 8, bottom = DashH() - kLensBarH;
    for (int i = (int)g_hits.size() - 1; i >= 0; i--) {
        auto& h = g_hits[i];
        if (!Inside(h.r, x, y)) continue;
        if ((h.kind == HIT_TILE || h.kind == HIT_MORE || h.kind == HIT_MEDIA || h.kind == HIT_WEATHER) && (y < top || y > bottom)) continue;   // scrolled out of view
        return i;
    }
    return -1;
}

static int FlatIndex(int section, int index) {
    for (int i = 0; i < (int)g_flat.size(); i++)
        if (g_flat[i].section == section && g_flat[i].index == index) return i;
    return -1;
}

static void Activate(const Hit& h) {
    switch (h.kind) {
        case HIT_TILE: OpenResult(g_sections[h.a].items[h.b]); break;
        case HIT_MORE: {
            std::wstring key = std::to_wstring(g_lens) + L"/" + g_sections[h.a].title;
            g_expanded[key] = !g_expanded[key];
            StartRowsIn();
            RenderDash();
            break;
        }
        case HIT_LENS: SetLens(h.a); break;
        case HIT_MEDIA: MediaCommand(h.a); break;
        case HIT_WEATHER:   // Windows' own Weather app, or its Store page to install it
            ShowDash(false);
            Launch(WeatherAppInstalled() ? L"bingweather:" : L"ms-windows-store://pdp/?ProductId=9WZDNCRFJ3Q2");
            break;
        case HIT_FILTERS:
            g_filtersOpen = !g_filtersOpen;
            if (g_filtersOpen) {
                g_aFilter.Go(1, 220, EASE_OUT_CUBIC, 0);
                KickAnim();
            }
            g_hoverHit = -1;
            RenderDash();
            break;
        case HIT_FGROUP: {
            auto groups = LensFilters();
            if (h.a < (int)groups.size()) g_filterShut[FilterKey(groups[h.a].name)] = !g_filterShut[FilterKey(groups[h.a].name)];
            RenderDash();
            break;
        }
        case HIT_FCHIP: {
            auto groups = LensFilters();
            if (h.a < (int)groups.size() && h.b < (int)groups[h.a].values.size()) ToggleFilter(groups[h.a], groups[h.a].values[h.b]);
            g_hoverHit = -1;
            QueryChanged();
            StartRowsIn();
            RenderDash();
            break;
        }
    }
}

static void CopyText(const std::wstring& text) {
    if (!OpenClipboard(g_dash)) return;
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    if (HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes)) {
        memcpy(GlobalLock(mem), text.c_str(), bytes);
        GlobalUnlock(mem);
        if (!SetClipboardData(CF_UNICODETEXT, mem)) GlobalFree(mem);
    }
    CloseClipboard();
}

// the right-click menu of a Dash result (the app's): apps get Start's actions, settings and files their own
static void ResultMenu(const Result& r, POINT pt) {
    Result copy = r;
    std::vector<MenuEntry> m = {MHeading(r.name), MSeparator()};
    std::wstring file;
    if (r.kind == RK_APP) {
        file = FileOf(r.open);
        m.push_back(MItem(1, Tr(L"Openen"), L"\uE8A7"));
        m.push_back(MItem(2, Tr(L"Als administrator uitvoeren"), L"\uEA18"));
        m.push_back(IsPinned(r.open) ? MItem(3, Tr(L"Losmaken van launcher"), L"\uE77A") : MItem(3, Tr(L"Vastmaken aan launcher"), L"\uE718"));
        if (!file.empty()) m.push_back(MItem(4, Tr(L"Bestandslocatie openen"), L"\uE838"));
        m.push_back(MItem(5, Tr(L"App-instellingen"), L"\uE713"));
        m.push_back(MSeparator());
        m.push_back(MItem(5, Tr(L"Verwijderen"), L"\uE74D"));
    } else if (r.kind == RK_SETTING) {
        m.push_back(MItem(1, L"Open"));
    } else {
        file = r.open;
        m.push_back(MItem(1, L"Open"));
        m.push_back(MItem(4, L"Show in folder"));
        m.push_back(MItem(6, L"Copy path"));
    }
    int cmd = ShowMenu(m, RECT{pt.x, pt.y, pt.x, pt.y}, MENU_POINT);
    if (cmd == 1) OpenResult(copy);
    else if (cmd == 2) OpenResult(copy, L"runas");
    else if (cmd == 3) { Pin(copy.open, !IsPinned(copy.open)); BuildItems(); RenderLauncher(); }
    else if (cmd == 4) { ShowDash(false); ShowInFolder(file); }
    else if (cmd == 5) { ShowDash(false); Launch(L"ms-settings:appsfeatures"); }
    else if (cmd == 6) CopyText(copy.open);
    if (g_dashOpen) RenderDash();
}

static void TextChanged() {
    QueryChanged();
    RenderDash();
    SetTimer(g_dash, TIMER_FILESEARCH, 250, nullptr);   // ask Windows Search once typing pauses
}

static LRESULT CALLBACK DashProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

// the Dash's bottom row (foot and tip windows): the mouse there belongs to the Dash (its grip sits in the corner)
static LRESULT CALLBACK DashPartProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_SETCURSOR:
            return DashProc(g_dash, msg, (WPARAM)g_dash, lp);
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP: {
            POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            MapWindowPoints(hwnd, g_dash, &pt, 1);
            return DashProc(g_dash, msg, wp, MAKELPARAM(pt.x, pt.y));
        }
        case WM_MOUSEWHEEL:
            return DashProc(g_dash, msg, wp, lp);
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK DashProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_ACTIVATE:
            // clicking elsewhere closes it (not its own right-click menu taking the focus)
            if (LOWORD(wp) == WA_INACTIVE && g_dashOpen && (HWND)lp != g_menu) ToggleDash();
            return 0;
        case WM_TIMER:
            if (wp == TIMER_FILESEARCH) {
                KillTimer(hwnd, TIMER_FILESEARCH);
                RequestFiles();
            } else if (wp == TIMER_WIDGETS) {
                if (ShowWidgets()) {
                    SampleStats();
                    RenderDash();
                }
            }
            return 0;
        case WM_CHAR:
            if (wp >= 32 && wp != 127 && GetKeyState(VK_CONTROL) >= 0) {
                g_query += (wchar_t)wp;
                TextChanged();
            }
            return 0;
        case WM_KEYDOWN:
            switch (wp) {
                case VK_ESCAPE:
                    if (!g_query.empty()) { g_query.clear(); TextChanged(); }
                    else ToggleDash();
                    return 0;
                case VK_BACK:
                    if (!g_query.empty()) {
                        if (GetKeyState(VK_CONTROL) < 0) g_query.clear();
                        else g_query.pop_back();
                        TextChanged();
                    }
                    return 0;
                case VK_RETURN:
                    if (const Result* r = Selected()) OpenResult(*r);
                    return 0;
                case VK_TAB:
                    if (GetKeyState(VK_CONTROL) < 0) SetLens(g_lens + (GetKeyState(VK_SHIFT) < 0 ? -1 : 1));
                    return 0;
                case VK_RIGHT: MoveSel(1, 0); return 0;
                case VK_LEFT: MoveSel(-1, 0); return 0;
                case VK_DOWN: MoveSel(0, 1); return 0;
                case VK_UP: MoveSel(0, -1); return 0;
            }
            break;
        case WM_SETCURSOR:
            if (LOWORD(lp) == HTCLIENT && (g_resizing || (g_hoverHit >= 0 && g_hoverHit < (int)g_hits.size() && g_hits[g_hoverHit].kind == HIT_GRIP))) {
                SetCursor(LoadCursorW(nullptr, IDC_SIZENWSE));
                return TRUE;
            }
            break;
        case WM_LBUTTONDOWN: {
            int hit = HitAt(GET_X_LPARAM(lp) / (float)g_scale, GET_Y_LPARAM(lp) / (float)g_scale);
            if (hit >= 0 && g_hits[hit].kind == HIT_GRIP) {   // start dragging the grip: the Dash follows the pointer
                g_resizing = true;
                SetCapture(hwnd);
            }
            return 0;
        }
        case WM_CAPTURECHANGED:
            if (g_resizing && (HWND)lp != hwnd) { g_resizing = false; SaveDashSize(); RenderDash(); }
            return 0;
        case WM_MOUSEMOVE: {
            if (g_resizing) {
                POINT pt;
                GetCursorPos(&pt);
                RECT wr;
                GetWindowRect(hwnd, &wr);
                // the grip sits 13 DIPs in from the band's outer corner
                g_dashW = std::max(kDashMinW, (float)((pt.x - wr.left) / g_scale) + 13 - kFrame);
                g_dashH = std::max(kDashMinH, (float)((pt.y - wr.top) / g_scale) + 13 - kFrame);
                g_dashW = std::min(g_dashW, DashRoomW() - kFrame);
                g_dashH = std::min(g_dashH, DashRoomH() - kFrame);
                RenderDash();
                RenderEdge();
                RenderLauncher();
                return 0;
            }
            int hit = HitAt(GET_X_LPARAM(lp) / (float)g_scale, GET_Y_LPARAM(lp) / (float)g_scale);
            if (hit != g_hoverHit) {
                g_hoverHit = hit;
                if (hit >= 0 && g_hits[hit].kind == HIT_TILE) g_sel = FlatIndex(g_hits[hit].a, g_hits[hit].b);
                RenderDash();
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (g_resizing) {
                ReleaseCapture();   // saves the size (WM_CAPTURECHANGED)
                return 0;
            }
            int hit = HitAt(GET_X_LPARAM(lp) / (float)g_scale, GET_Y_LPARAM(lp) / (float)g_scale);
            if (hit >= 0) Activate(Hit(g_hits[hit]));
            return 0;
        }
        case WM_RBUTTONUP: {
            POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            int hit = HitAt(pt.x / (float)g_scale, pt.y / (float)g_scale);
            if (hit < 0 || g_hits[hit].kind != HIT_TILE) return 0;
            ClientToScreen(hwnd, &pt);
            ResultMenu(g_sections[g_hits[hit].a].items[g_hits[hit].b], pt);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            float view = DashH() - kLensBarH - (kMargin + kSearchH + 8);
            float maxScroll = std::max(0.f, g_contentH - view);
            g_scroll = std::clamp(g_scroll - GET_WHEEL_DELTA_WPARAM(wp) / 120.f * 70, 0.f, maxScroll);
            RenderDash();
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ============================================================================================================
// keeping Start and the taskbar ours
// ============================================================================================================

static LRESULT CALLBACK TraySubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR);

// Explorer brings its taskbar back now and then in ways the hooks don't see (Windows Search, a re-created
// taskbar window): check twice a second, hide it again, and follow a new taskbar window
static void GuardTaskbar() {
    if (!g_set.hideTaskbar || !g_running) return;
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray && tray != g_tray) {
        Wh_Log(L"the taskbar window was re-created");
        if (g_tray) WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_tray, TraySubclass);
        g_tray = tray;
        WindhawkUtils::SetWindowSubclassFromAnyThread(g_tray, TraySubclass, 0);
    }
    for (HWND bar : Taskbars())
        if (IsWindowVisible(bar)) ShowWindow_Original(bar, SW_HIDE);
}

// the keyboard hook's thread (see KeyProc), so Start getting through can renew the hook
static DWORD g_keyThreadId;
static const UINT WM_APP_RENEWHOOK = WM_APP + 50;
static bool IsStartMenu(HWND h);
static void StartOpened();

static void GuardStart() {
    HWND fg = GetForegroundWindow();
    if (g_set.startOpensDash && g_running && IsStartMenu(fg)) StartOpened();
}

// the safety net: if Windows' own Start opens anyway, swap it for the Dash
static HWINEVENTHOOK g_fgHook;

static bool IsStartMenu(HWND h) {
    if (!h) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    std::wstring exe = Lower(ProcessPath(pid));
    if (exe.ends_with(L"\\startmenuexperiencehost.exe")) return true;
    wchar_t cls[64];
    GetClassNameW(h, cls, 64);
    if (wcscmp(cls, L"Windows.UI.Core.CoreWindow")) return false;
    // Start's search box lives in SearchHost: with Start invisible, that is what the Windows key brings up
    return WindowTitle(h) == L"Start" || exe.ends_with(L"\\searchhost.exe");
}

static void StartOpened() {
    Wh_Log(L"Windows Start opened anyway -> Dash");
    if (g_keyThreadId) PostThreadMessageW(g_keyThreadId, WM_APP_RENEWHOOK, 0, 0);
    if (!IsWindowVisible(g_dash)) ShowDash(true);   // taking the focus closes Start
    SetTimer(g_host, TIMER_STARTESC, 250, nullptr);
}

// a window turning maximized or back: refresh soon (the panel's title and buttons, the merged title bar)
static HWINEVENTHOOK g_moveHook, g_moveEndHook;

// a window let go of with its title bar behind the panel (or above the screen) comes down just far enough to grab
static void CALLBACK MoveEnded(HWINEVENTHOOK, DWORD, HWND hwnd, LONG obj, LONG, DWORD, DWORD) {
    if (obj != OBJID_WINDOW || !hwnd || GetAncestor(hwnd, GA_ROOT) != hwnd || IsZoomed(hwnd) || IsIconic(hwnd) || g_fake.count(hwnd)) return;
    RECT wr, vis, work = WorkArea();
    if (!GetWindowRect(hwnd, &wr)) return;
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &vis, sizeof(vis)))) vis = wr;
    if (vis.top >= work.top || vis.bottom <= work.top) return;
    if (MonitorFromPoint({(vis.left + vis.right) / 2, vis.top}, MONITOR_DEFAULTTONULL)) {   // its top is on a screen
        RECT pr = {g_screen.left, g_screen.top, g_screen.right, work.top};
        POINT top = {(vis.left + vis.right) / 2, vis.top};
        if (!PtInRect(&pr, top)) return;   // on another screen above this one, not behind the panel
    }
    int down = work.top - vis.top;
    SetWindowPos(hwnd, nullptr, wr.left, wr.top + down, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

static void CALLBACK WindowMoved(HWINEVENTHOOK, DWORD, HWND hwnd, LONG obj, LONG child, DWORD, DWORD) {
    if (obj != OBJID_WINDOW || child != CHILDID_SELF || !hwnd || !g_host || GetAncestor(hwnd, GA_ROOT) != hwnd) return;
    // maximized or not, and for the window in front also full screen or not (a video going full screen: the shell
    // steps aside right away)
    static std::map<HWND, int> state;
    int z = (IsZoomed(hwnd) ? 1 : 0) | (hwnd == GetForegroundWindow() && CoversMonitor(hwnd) ? 2 : 0);
    auto it = state.find(hwnd);
    if (it != state.end() && it->second == z) return;
    bool known = it != state.end();
    if (state.size() > 500) state.clear();
    state[hwnd] = z;
    if (known || z) SetTimer(g_host, TIMER_SOON, 30, nullptr);
}

static void CALLBACK ForegroundChanged(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD, DWORD) {
    if (!g_set.startOpensDash || !g_running || !IsStartMenu(hwnd)) return;
    StartOpened();
}
// ============================================================================================================
// the host window: shell notifications, timers, the "open Start" request from the taskbar
// ============================================================================================================

static bool IsFullscreen(HWND h) {
    if (!h || h == g_dash) return false;
    wchar_t cls[64];
    GetClassNameW(h, cls, 64);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW")) return false;
    // Explorer's own full-screen surfaces (Alt+Tab, Task View, ...) aren't games or videos; a maximized window isn't either
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId() || IsZoomed(h)) return false;
    // nor are click-through overlays over the whole screen (game bars, recording overlays): they'd make the shell blink
    if (!IsWindowVisible(h) || (GetWindowLongW(h, GWL_EXSTYLE) & WS_EX_TRANSPARENT)) return false;
    // only the panel's screen counts: something full-screen on another one leaves the shell alone
    return OnPanelScreen(h) && CoversMonitor(h);
}


// an app got a new window: its icon pops (Unity)
static void DetectPops() {
    for (auto& it : g_items) {
        ItemFx& fx = Fx(it.launch);
        int n = (int)it.windows.size();
        if (fx.lastWindows >= 0 && n > fx.lastWindows) {
            fx.pop.Go(1, 380, EASE_SPRING, 1.25f);
            KickAnim();
        }
        fx.lastWindows = n;
    }
}

static int ItemOfWindow(HWND hwnd) {
    for (int i = 0; i < (int)g_items.size(); i++)
        for (HWND w : g_items[i].windows)
            if (w == hwnd) return i;
    return -1;
}

// an app wants attention (a flashing taskbar button): its icon wiggles, at most once every 10 s
static void Wiggle(HWND hwnd) {
    int i = ItemOfWindow(hwnd);
    if (i < 0) return;
    ItemFx& fx = Fx(g_items[i].launch);
    double now = NowMs();
    if (now - fx.lastWiggle < 10000) return;
    fx.lastWiggle = now;
    fx.wiggle.Go(1, 1400, EASE_LINEAR, 0);
    KickAnim();
}

// where a window's launcher icon is on screen (pixels), for the minimize animation
static bool IconScreenRect(HWND hwnd, RECT* out) {
    int i = ItemOfWindow(hwnd);
    if (i < 0 || g_hiddenForFullscreen) return false;
    D2D1_RECT_F r = ItemRect(i);
    int top = g_screen.top + Px(PanelH());
    *out = {g_screen.left + Px(r.left), top + Px(r.top), g_screen.left + Px(r.right), top + Px(r.bottom)};
    return true;
}

// ---------------- window ghosts (the app's): an accent-coloured shape grows out of the launcher icon into a new
// window, and shrinks from a closing window back into its icon. A plain window with one alpha for all of it, so
// DWM moves it cheaply even at full-screen size ----------------

struct Ghost {
    HWND hwnd;
    RECT from, to;
    double start, ms;
    float a0, a1;
};
static std::vector<Ghost> g_ghosts;
static std::vector<std::pair<HWND, double>> g_opening;          // windows just created, looked at a moment later
static std::map<HWND, std::pair<RECT, std::wstring>> g_lastSpot;   // where each app window last was, and its icon

static LRESULT CALLBACK GhostProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_ERASEBKGND) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        Rgb c = Shade(g_accent, 1.15f);
        HBRUSH b = CreateSolidBrush(RGB((int)(c.r * 255), (int)(c.g * 255), (int)(c.b * 255)));
        FillRect((HDC)wp, &rc, b);
        DeleteObject(b);
        return 1;
    }
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static bool g_ghostClass;
static void StartGhost(RECT from, RECT to, double ms, float a0, float a1) {
    if (!g_ghostClass) return;
    if (g_ghosts.size() >= 6 || !g_set.windowEffects || g_set.animScale <= 0) return;   // the window effects setting
    ms *= g_set.animScale;
    HWND h = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST, L"UnityShellGhost", L"", WS_POPUP,
                             from.left, from.top, std::max(4L, from.right - from.left), std::max(4L, from.bottom - from.top), nullptr, nullptr, ModInstance(), nullptr);
    if (!h) return;
    SetLayeredWindowAttributes(h, 0, (BYTE)(a0 * 255), LWA_ALPHA);
    int round = 2;   // rounded, with a white rim (Windows 11)
    DwmSetWindowAttribute(h, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &round, sizeof(round));
    COLORREF rim = RGB(255, 255, 255);
    DwmSetWindowAttribute(h, 34 /* DWMWA_BORDER_COLOR */, &rim, sizeof(rim));
    ShowWindow(h, SW_SHOWNA);
    g_ghosts.push_back({h, from, to, NowMs(), ms, a0, a1});
    KickAnim();
}

static bool StepGhosts(double now) {
    auto lerp = [](LONG a, LONG b, double e) { return (int)lround(a + (b - a) * e); };
    for (size_t i = 0; i < g_ghosts.size();) {
        Ghost& g = g_ghosts[i];
        double t = std::min(1.0, (now - g.start) / g.ms), e = 1 - pow(1 - t, 3);   // ease out
        int l = lerp(g.from.left, g.to.left, e), tp = lerp(g.from.top, g.to.top, e);
        int w = std::max(4, lerp(g.from.right - g.from.left, g.to.right - g.to.left, e));
        int h = std::max(4, lerp(g.from.bottom - g.from.top, g.to.bottom - g.to.top, e));
        SetWindowPos(g.hwnd, HWND_TOPMOST, l, tp, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        SetLayeredWindowAttributes(g.hwnd, 0, (BYTE)(255 * (g.a0 + (g.a1 - g.a0) * t)), LWA_ALPHA);
        if (t >= 1) {
            DestroyWindow(g.hwnd);
            g_ghosts.erase(g_ghosts.begin() + i);
        } else i++;
    }
    return !g_ghosts.empty();
}

// remember where every app window is, so a closing one can shrink from its last spot into its icon
static void RememberSpots() {
    std::map<HWND, std::pair<RECT, std::wstring>> spots;
    for (auto& it : g_items)
        for (HWND w : it.windows) {
            RECT r;
            if (!IsIconic(w) && GetWindowRect(w, &r)) spots[w] = {r, it.launch};
            else if (g_lastSpot.count(w)) spots[w] = g_lastSpot[w];
        }
    g_lastSpot = std::move(spots);
}

static void WindowOpened(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || g_hiddenForFullscreen) return;
    RECT to, from;
    if (!GetWindowRect(hwnd, &to) || !IconScreenRect(hwnd, &from)) return;   // not an app window (a tooltip, a popup)
    StartGhost(from, to, 240, 0.55f, 0);
}

static void WindowClosed(HWND hwnd) {
    auto spot = g_lastSpot.find(hwnd);
    if (spot == g_lastSpot.end() || g_hiddenForFullscreen) return;
    RECT last = spot->second.first;
    std::wstring launch = spot->second.second;
    g_lastSpot.erase(spot);
    for (int i = 0; i < VisibleItems(); i++) {
        if (g_items[i].launch != launch) continue;
        D2D1_RECT_F r = ItemRect(i);
        int top = g_screen.top + Px(PanelH());
        StartGhost(last, {g_screen.left + Px(r.left), top + Px(r.top), g_screen.left + Px(r.right), top + Px(r.bottom)}, 260, 0.45f, 0.1f);
        return;
    }
}

// one frame of every running animation
static void AnimFrame() {
    double now = NowMs();
    bool dash = false, launcher = false, panel = false;
    dash |= g_aDash.Step(now);
    dash |= g_aZoom.Step(now);
    panel |= g_aUnder.Step(now);
    launcher |= panel;
    launcher |= g_aDim.Step(now);
    launcher |= g_aBfb.Step(now);
    launcher |= g_aExt.Step(now);
    launcher |= g_aDc.Step(now);
    if (g_aAway.Step(now)) {   // the bars slide out of the way (up, left) or back
        float v = std::clamp(g_aAway.value, 0.f, 1.f);
        g_awayPanel = (int)std::lround(Px(PanelH()) * v);
        g_awayLauncher = (int)std::lround(Px(LauncherW()) * v);
        launcher = panel = true;
        if (!g_aAway.running && v >= 1) {
            ShowWindow(g_panel, SW_HIDE);
            ShowWindow(g_launcher, SW_HIDE);
        }
    }
    if (!g_aDc.running && g_aDc.value <= 0) g_dcHost = -1;
    panel |= g_aBtns.Step(now);
    if (!g_aExt.running && g_aExt.value <= 0) g_extHost = -1;   // shut: it lets go of its icon
    for (auto& kv : g_fx) {
        ItemFx& fx = kv.second;
        launcher |= fx.hover.Step(now);
        launcher |= fx.pop.Step(now);
        launcher |= fx.wiggle.Step(now);
        launcher |= fx.shift.Step(now);
        if (fx.pulseStart > 0) launcher = true;
    }
    dash |= g_aFilter.Step(now);
    bool rows = DashShown() && now - g_rowsStart < kRowsMs;
    dash |= rows;
    bool ghosts = StepGhosts(now);
    if (g_aGroup.Step(now) && g_group && IsWindowVisible(g_group)) PaintGroup();
    if (dash || g_dashClosing) DashFrame();
    if (dash || launcher) RenderLauncher();
    if (panel) RenderPanel();
    bool any = g_aDash.running || g_aZoom.running || g_aUnder.running || g_aDim.running || g_aBfb.running || g_dashClosing || g_aFilter.running || rows || ghosts || g_aExt.running || g_aDc.running || g_aAway.running || g_aGroup.running || g_aBtns.running;
    for (auto& kv : g_fx) any = any || kv.second.hover.running || kv.second.pop.running || kv.second.wiggle.running || kv.second.shift.running || kv.second.pulseStart > 0;
    if (!any) KillTimer(g_host, TIMER_ANIM);
}

static void Refresh() {
    BuildItems();
    RememberSpots();
    DecorUpdate();
    LiftUpdate();
    DetectPops();
    CheckTrash();
    UpdateExtension();
    UpdateDcExtension();
    if (PreviewShown() && (g_previewFor < 0 || g_previewFor >= (int)g_items.size() || g_items[g_previewFor].launch != g_previewLaunch)) HidePreview();
    // get out of the way of full-screen games and videos
    bool fs = g_set.fullscreenHides && IsFullscreen(GetForegroundWindow());
    if (fs != g_hiddenForFullscreen) {   // the bars slide away (and back), then hide
        g_hiddenForFullscreen = fs;
        if (!fs) {
            ShowWindow(g_panel, SW_SHOWNA);
            ShowWindow(g_launcher, SW_SHOWNA);
        }
        g_aAway.Go(fs ? 1.f : 0.f, fs ? 220.f : 300.f, fs ? EASE_IN_CUBIC : EASE_OUT_CUBIC);
        KickAnim();
    }
    if (!fs) {
        RenderLauncher();
        RenderPanel();
    }
}

static LRESULT CALLBACK HostProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_shellHookMsg && g_shellHookMsg) {
        if (wp == HSHELL_GETMINRECT) {
            // Windows asks where a window's taskbar button is, to minimize into it / restore out of it: answer with
            // its launcher icon, so windows fly into the launcher
            struct MinRect { HWND hwnd; RECT rc; };
            auto* info = (MinRect*)lp;
            RECT r;
            if (info && IconScreenRect(info->hwnd, &r)) {
                info->rc = r;
                return 1;
            }
        }
        if (wp == HSHELL_FLASH) Wiggle((HWND)lp);
        if (wp == HSHELL_WINDOWDESTROYED) WindowClosed((HWND)lp);
        if (wp == HSHELL_WINDOWCREATED) {   // give the app a moment to put its window where it wants it
            g_opening.push_back({(HWND)lp, NowMs() + 70});
            SetTimer(hwnd, TIMER_GHOST, 70, nullptr);
        }
        SetTimer(hwnd, TIMER_SOON, 120, nullptr);   // a window opened, closed, activated or was renamed
        return 0;
    }
    switch (msg) {
        case WM_TIMER:
            if (wp == TIMER_ANIM) { AnimFrame(); return 0; }
            if (wp == TIMER_CHECKWIN) { KillTimer(hwnd, TIMER_CHECKWIN); CheckWindowsNow(); return 0; }
            if (wp == TIMER_GHOST) {
                KillTimer(hwnd, TIMER_GHOST);
                Refresh();   // so the new window has its icon
                double now = NowMs();
                std::vector<std::pair<HWND, double>> later;
                for (auto& o : g_opening)
                    if (o.second <= now + 5) WindowOpened(o.first);
                    else later.push_back(o);
                g_opening = later;
                if (!g_opening.empty()) SetTimer(hwnd, TIMER_GHOST, 30, nullptr);
                return 0;
            }
            if (wp == TIMER_SOON) KillTimer(hwnd, TIMER_SOON);
            if (wp == TIMER_CLOCK) { ReadVolume(); RenderPanel(); return 0; }
            if (wp == TIMER_TASKBAR) { GuardTaskbar(); GuardStart(); return 0; }
            if (wp == TIMER_STARTESC) {
                KillTimer(hwnd, TIMER_STARTESC);
                if (IsStartMenu(GetForegroundWindow())) {   // still open: close it like a key press would
                    keybd_event(VK_ESCAPE, 0, 0, 0);
                    keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0);
                    if (IsWindowVisible(g_dash)) { SetForegroundWindow(g_dash); SetFocus(g_dash); }
                }
                return 0;
            }
            Refresh();
            return 0;
        case WM_APP_TOGGLEDASH:
            if (wp == 1) Wh_Log(L"Windows key tapped -> Dash");
            if (GetTickCount() - g_lastToggle > 250) ToggleDash();   // one press can arrive twice (hook + taskbar)
            g_lastToggle = GetTickCount();
            return 0;
        case WM_APP_FILES:
            if (IsWindowVisible(g_dash)) {
                int keep = g_sel;
                BuildSections();
                g_sel = keep;
                RenderDash();
            }
            return 0;
        case WM_APP_DISCORD:
            UpdateDcExtension();
            return 0;
        case WM_APP_TABS:
            LiftUpdate();
            RenderPanel();
            return 0;
        case WM_APP_WEATHER:
        case WM_APP_MEDIA:
            if (msg == WM_APP_MEDIA) UpdateExtension();
            if (IsWindowVisible(g_dash) && ShowWidgets()) RenderDash();
            return 0;
        case WM_SETTINGCHANGE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            ReadColours();
            g_panelS.Reset();
            g_launcherS.Reset();
            g_dashS.Reset();
            Refresh();
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:   // a new resolution, scale or main screen: everything measured and laid out again
            PostThreadMessageW(GetCurrentThreadId(), WM_APP_REFRESH, 0, 0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ============================================================================================================
// Start -> Dash: the Windows key and Ctrl+Esc reach the taskbar window as "open the task list" (SC_TASKLIST),
// whatever app is in front (also one running as administrator)
// ============================================================================================================

static LRESULT CALLBACK TraySubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR) {
    if (msg == WM_COPYDATA) TrayNotify((const COPYDATASTRUCT*)lp);   // an app's tray icon (see the tray popup)
    if (msg == WM_SYSCOMMAND) Wh_Log(L"taskbar: WM_SYSCOMMAND 0x%IX", wp);
    if (msg == WM_SYSCOMMAND && (wp & 0xFFF0) == SC_TASKLIST && g_set.startOpensDash && g_running && g_host) {
        PostMessageW(g_host, WM_APP_TOGGLEDASH, 0, 0);
        return 0;
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// ============================================================================================================
// the Windows key: tapped on its own it opens the Dash instead of Start. A low-level keyboard hook on its own
// thread (so a busy shell never delays typing). Windows never sees a tap at all: the key goes down only when a
// second key follows (Win+E, Win+Shift+S, ...), replayed just before that key so the shortcut still works.
// ============================================================================================================

static HANDLE g_keyThread;
enum { WIN_UP, WIN_HELD, WIN_PASSED, WIN_EATEN };   // held (swallowed so far), replayed for a shortcut, used up
static int g_win;
static DWORD g_winVk, g_winTime;

static void InjectKey(DWORD vk, DWORD scan, bool extended, bool up) {
    INPUT in = {INPUT_KEYBOARD};
    in.ki.wVk = (WORD)vk;
    in.ki.wScan = (WORD)scan;
    in.ki.dwFlags = (extended ? KEYEVENTF_EXTENDEDKEY : 0) | (up ? KEYEVENTF_KEYUP : 0);
    SendInput(1, &in, sizeof(in));
}

static LRESULT CALLBACK KeyProc(int code, WPARAM wp, LPARAM lp) {
    if (code != HC_ACTION || !g_running || !g_set.startOpensDash) return CallNextHookEx(nullptr, code, wp, lp);
    auto* k = (KBDLLHOOKSTRUCT*)lp;
    if (k->flags & LLKHF_INJECTED) return CallNextHookEx(nullptr, code, wp, lp);
    bool down = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
    if ((g_win == WIN_HELD || g_win == WIN_PASSED) && k->time - g_winTime > 10000 && !(GetAsyncKeyState(g_winVk) & 0x8000)) g_win = WIN_UP;   // its key-up got lost
    if (k->vkCode == VK_LWIN || k->vkCode == VK_RWIN) {
        if (down) {
            if (g_win == WIN_UP) { g_win = WIN_HELD; g_winVk = k->vkCode; g_winTime = k->time; }
            return g_win == WIN_PASSED ? CallNextHookEx(nullptr, code, wp, lp) : 1;   // auto-repeat follows what Windows saw
        }
        int was = g_win;
        g_win = WIN_UP;
        if (was == WIN_PASSED) {   // replayed too, so it lands after the replayed key-down (else Win stays "held")
            InjectKey(g_winVk, 0, true, true);
            return 1;
        }
        if (was == WIN_HELD) PostMessageW(g_host, WM_APP_TOGGLEDASH, 1, 0);   // no logging here: a slow hook gets dropped
        return 1;
    }
    if (g_win == WIN_PASSED) {   // during a replayed shortcut every key is replayed too: it keeps them in order
        InjectKey(k->vkCode, k->scanCode, k->flags & LLKHF_EXTENDED, !down);
        return 1;
    }
    if (down && g_win == WIN_HELD) {   // a shortcut after all
        if (k->vkCode == 'L') { g_win = WIN_EATEN; LockWorkStation(); return 1; }   // Windows ignores a replayed Win+L
        g_win = WIN_PASSED;
        InjectKey(g_winVk, 0, true, false);
        InjectKey(k->vkCode, k->scanCode, k->flags & LLKHF_EXTENDED, false);
        return 1;
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

static DWORD WINAPI KeyThread(LPVOID) {
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyProc, GetModuleHandleW(nullptr), 0);
    if (hook) Wh_Log(L"keyboard hook installed");
    else Wh_Log(L"keyboard hook failed (%u)", GetLastError());
    // Windows silently drops a low-level hook that once answered too slowly (a busy PC): it is renewed only when
    // that shows, Start getting through anyway (renewing it more often would keep moving it ahead of other apps' hooks)
    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0) > 0) {
        if (m.message != WM_APP_RENEWHOOK) continue;
        if (HHOOK fresh = SetWindowsHookExW(WH_KEYBOARD_LL, KeyProc, GetModuleHandleW(nullptr), 0)) {
            if (hook) UnhookWindowsHookEx(hook);
            hook = fresh;
        } else Wh_Log(L"keyboard hook renewal failed (%u)", GetLastError());
        if (m.message == WM_APP_RENEWHOOK) Wh_Log(L"keyboard hook renewed");
    }
    if (hook) UnhookWindowsHookEx(hook);
    return 0;
}
// ============================================================================================================
// the shell's UI thread
// ============================================================================================================

// the window classes belong to this mod's own DLL, not to Explorer, and go when the shell stops: a class left behind
// would hand the next version's windows to this version's code after it's gone (Explorer crashed on every update)
static HINSTANCE ModInstance() {
    static HMODULE self;
    if (!self) GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&ModInstance, &self);
    return self ? self : GetModuleHandleW(nullptr);
}
static std::vector<std::wstring> g_classes;

static HWND MakeWindow(const wchar_t* cls, WNDPROC proc, DWORD exStyle, bool dblclk = false) {
    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = proc;
    wc.hInstance = ModInstance();
    wc.lpszClassName = cls;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.style = dblclk ? CS_DBLCLKS : 0;
    if (!RegisterClassExW(&wc)) {   // never a class left by someone else: its code may be gone
        Wh_Log(L"window class %s could not be registered (%u)", cls, GetLastError());
        return nullptr;
    }
    g_classes.push_back(cls);
    return CreateWindowExW(exStyle, cls, cls, WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, wc.hInstance, nullptr);
}

static void Layout() {
    Measure();
    int ph = Px(PanelH()), lw = Px(LauncherW());
    RECT pr = {g_screen.left, g_screen.top, g_screen.right, g_screen.top + ph};
    RECT lr = {g_screen.left, g_screen.top + ph, g_screen.left + lw, g_screen.bottom};
    SetWindowPos(g_panel, HWND_TOPMOST, pr.left, pr.top, pr.right - pr.left, pr.bottom - pr.top, SWP_NOACTIVATE);
    SetWindowPos(g_launcher, HWND_TOPMOST, lr.left, lr.top, lr.right - lr.left, lr.bottom - lr.top, SWP_NOACTIVATE);
    // the Dash window without its bottom row, which the foot and tip windows show
    SetWindowPos(g_dash, HWND_TOPMOST, lr.right, pr.bottom, Px(DashW() + Frame()), Px(DashH() + Frame()) - (g_dashMax ? 0 : Px(kBendR)), SWP_NOACTIVATE);
    AppBar(g_panel, ABE_TOP, pr, true);
    AppBar(g_launcher, ABE_LEFT, lr, true);
}

static DWORD WINAPI UiThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    // wait for the taskbar; only the Explorer process that owns it runs the shell
    for (int i = 0;; i++) {
        g_tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        DWORD pid = 0;
        if (g_tray) GetWindowThreadProcessId(g_tray, &pid);
        if (pid == GetCurrentProcessId()) break;
        MSG m;
        if (PeekMessageW(&m, nullptr, WM_QUIT, WM_QUIT, PM_REMOVE) || i >= 240) { CoUninitialize(); return 0; }
        Sleep(250);
    }
    Sleep(1500);   // let the fresh taskbar settle

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&g_dw);
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_wic));
    if (!g_d2d || !g_dw || !g_wic) { Wh_Log(L"graphics init failed"); CoUninitialize(); return 0; }
    PickFont();
    ReadColours();
    Measure();

    DWORD tool = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
    g_host = MakeWindow(L"UnityShellHost", HostProc, WS_EX_TOOLWINDOW);
    g_panel = MakeWindow(L"UnityShellPanel", PanelProc, tool | WS_EX_NOACTIVATE, true);
    g_launcher = MakeWindow(L"UnityShellLauncher", LauncherProc, tool | WS_EX_NOACTIVATE);
    g_dash = MakeWindow(L"UnityShellDash", DashProc, tool);
    g_panelS.hwnd = g_panel;
    g_launcherS.hwnd = g_launcher;
    g_dashS.hwnd = g_dash;
    // the bend and strip under the Dash: click-through, and only the strip blurred (like the app)
    g_corner = MakeWindow(L"UnityShellDashBend", DefWindowProcW, tool | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT);
    g_strip = MakeWindow(L"UnityShellDashStrip", DefWindowProcW, tool | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT);
    g_cornerS.hwnd = g_corner;
    g_stripS.hwnd = g_strip;
    g_dashFoot = MakeWindow(L"UnityShellDashFoot", DashPartProc, tool | WS_EX_NOACTIVATE);
    g_dashTip = MakeWindow(L"UnityShellDashTip", DashPartProc, tool | WS_EX_NOACTIVATE);
    // the launcher's tooltip, and its window previews (a normal window: DWM draws thumbnails only into those)
    g_tip = MakeWindow(L"UnityShellTooltip", DefWindowProcW, tool | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT);
    g_tipS.hwnd = g_tip;
    g_menu = MakeWindow(L"UnityShellMenu", MenuProc, tool);
    g_menuS.hwnd = g_menu;
    g_pop = MakeWindow(L"UnityShellPopup", PopupProc, tool);
    g_popS.hwnd = g_pop;
    g_group = MakeWindow(L"UnityShellGroup", GroupProc, tool);
    g_groupS.hwnd = g_group;
    WNDCLASSEXW ghost = {sizeof(ghost)};
    ghost.lpfnWndProc = GhostProc;
    ghost.hInstance = ModInstance();
    ghost.lpszClassName = L"UnityShellGhost";
    g_ghostClass = RegisterClassExW(&ghost) != 0;   // no class of our own: no ghosts (never someone else's class)
    if (g_ghostClass) g_classes.push_back(ghost.lpszClassName);
    g_preview = MakeWindow(L"UnityShellPreviews", PreviewProc, WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE);
    // any window missing: the shell doesn't start (what was made goes again)
    HWND made[] = {g_host, g_panel, g_launcher, g_dash, g_corner, g_strip, g_dashFoot, g_dashTip, g_tip, g_menu, g_pop, g_group, g_preview};
    if (std::any_of(std::begin(made), std::end(made), [](HWND h) { return !h; })) {
        Wh_Log(L"the shell's windows could not be made: not starting");
        for (HWND h : made)
            if (h) DestroyWindow(h);
        for (auto& c : g_classes) UnregisterClassW(c.c_str(), ModInstance());
        g_classes.clear();
        g_host = g_panel = g_launcher = g_dash = g_corner = g_strip = g_dashFoot = g_dashTip = g_tip = g_menu = g_pop = g_group = g_preview = nullptr;
        Release(g_wic);
        Release(g_dw);
        Release(g_d2d);
        CoUninitialize();
        return 0;
    }
    int round = 2;   // DWMWCP_ROUND
    DwmSetWindowAttribute(g_preview, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &round, sizeof(round));
    for (HWND h : {g_panel, g_launcher, g_dash, g_dashFoot, g_strip}) BlurBehind(h);
    g_aDash.Set(0);
    g_aZoom.Set(1);
    g_aUnder.Set(0);
    g_aDim.Set(1);
    g_aBfb.Set(1);

    LoadPins();
    LoadFolders();
    LoadRecentApps();
    LoadApps();
    StartWorkers();
    StartTabReader();
    if (g_set.discordControls) StartDiscordWatch();
    HideTaskbar();
    g_running = true;
    LoadDashSize();
    Layout();
    BuildItems();
    ReadVolume();
    ShowWindow(g_panel, SW_SHOWNA);
    ShowWindow(g_launcher, SW_SHOWNA);
    RenderPanel();
    RenderLauncher();

    g_shellHookMsg = RegisterWindowMessageW(L"SHELLHOOK");
    RegisterShellHookWindow(g_host);
    SetTimer(g_host, TIMER_REFRESH, 2000, nullptr);
    SetTimer(g_host, TIMER_CLOCK, 5000, nullptr);
    SetTimer(g_host, TIMER_TASKBAR, 500, nullptr);
    if (g_set.startOpensDash) g_keyThread = CreateThread(nullptr, 0, KeyThread, nullptr, 0, &g_keyThreadId);   // the Windows key opens the Dash
    g_fgHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, ForegroundChanged, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    g_moveHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr, WindowMoved, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_moveEndHook = SetWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND, nullptr, MoveEnded, 0, 0, WINEVENT_OUTOFCONTEXT);
    StartTrayWatch();
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(g_tray, TraySubclass, 0)) Wh_Log(L"listening for Start on the taskbar");
    // every app hands its tray icons over again, as when Explorer starts (so the ones from before are known too);
    // once: the list stays while the shell restarts for a settings change
    static bool asked;
    if (!asked) {
        asked = true;
        PostMessageW(HWND_BROADCAST, RegisterWindowMessageW(L"TaskbarCreated"), 0, 0);
    }
    Wh_Log(L"shell up (scale %.2f) mergeMaximized=%d liftTitleBars=%d", g_scale, g_set.mergeMaximized, g_set.liftTitleBars);

    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0) > 0) {
        if (m.message == WM_APP_REFRESH) { Layout(); Refresh(); continue; }
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }

    // shutting down: give everything back
    g_running = false;
    if (g_fgHook) { UnhookWinEvent(g_fgHook); g_fgHook = nullptr; }
    if (g_moveHook) { UnhookWinEvent(g_moveHook); g_moveHook = nullptr; }
    if (g_moveEndHook) { UnhookWinEvent(g_moveEndHook); g_moveEndHook = nullptr; }
    DecorRestoreAll();
    FakeRestoreAll();
    StopTabReader();
    StopDiscordWatch();
    if (g_keyThread) {
        do PostThreadMessageW(g_keyThreadId, WM_QUIT, 0, 0);   // again until it ends (see StopUi)
        while (WaitForSingleObject(g_keyThread, 100) == WAIT_TIMEOUT);
        CloseHandle(g_keyThread);
        g_keyThread = nullptr;
    }
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_tray, TraySubclass);
    StopTrayWatch();
    DeregisterShellHookWindow(g_host);
    AppBar(g_panel, ABE_TOP, {}, false);
    AppBar(g_launcher, ABE_LEFT, {}, false);
    RestoreTaskbar();
    StopWorkers();
    g_panelS.Free();
    g_launcherS.Free();
    g_dashS.Free();
    g_cornerS.Free();
    g_stripS.Free();
    HidePreview();
    g_tipS.Free();
    g_previewS.Free();
    g_menuS.Free();
    g_popS.Free();
    g_groupS.Free();
    DestroyWindow(g_group);
    g_group = nullptr;
    for (auto& g : g_ghosts) DestroyWindow(g.hwnd);
    g_ghosts.clear();
    for (HWND h : {g_corner, g_strip, g_dashFoot, g_dashTip, g_tip, g_preview, g_menu, g_pop}) DestroyWindow(h);
    g_corner = g_strip = g_dashFoot = g_dashTip = g_tip = g_preview = g_menu = g_pop = nullptr;
    for (auto& kv : g_icons) Release(kv.second.src);
    g_icons.clear();
    DestroyWindow(g_dash);
    DestroyWindow(g_launcher);
    DestroyWindow(g_panel);
    DestroyWindow(g_host);
    g_dash = g_launcher = g_panel = g_host = nullptr;
    FreeFormats();
    for (auto& c : g_classes) UnregisterClassW(c.c_str(), ModInstance());
    g_classes.clear();
    Release(g_wic);
    Release(g_dw);
    Release(g_d2d);
    CoUninitialize();
    return 0;
}

static void StartUi() {
    g_uiThread = CreateThread(nullptr, 0, UiThread, nullptr, 0, &g_uiThreadId);
}

static void StopUi() {
    if (!g_uiThread) return;
    // posted again until the thread ends: a thread that hasn't made its message queue yet would lose it
    do PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    while (WaitForSingleObject(g_uiThread, 100) == WAIT_TIMEOUT);
    CloseHandle(g_uiThread);
    g_uiThread = nullptr;
}

// ============================================================================================================
// Windhawk
// ============================================================================================================

BOOL Wh_ModInit() {
    LoadSettings();
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original);
    StartUi();   // waits for the taskbar first, by which time the hooks above are in place
    return TRUE;
}

void Wh_ModUninit() {
    g_unloading = true;
    StopUi();
}

void Wh_ModSettingsChanged() {
    // simplest and safest: rebuild the whole shell with the new settings
    StopUi();
    LoadSettings();
    StartUi();
}
