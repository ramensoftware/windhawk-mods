// ==WindhawkMod==
// @id              win-7-8-alt-tab-recreation
// @name            Windows 7/8.1 Alt+Tab Recreation
// @description     This mod recreates the Windows 7 and the Windows 8.1 Alt+Tab switchers on Windows 10 and 11 without modifying explorer.exe
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         GPL-2.0
// @include         windhawk.exe
// @compilerOptions -ldwmapi -luser32 -lgdi32 -lmsimg32 -lshell32 -lole32 -lcomctl32 -lgdiplus -lshlwapi
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Windows 7/8.1 Alt+Tab Recreation

**Windows 7**

![Windows 7 switcher](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win7alttab.png)

**Windows 8.1**

![Windows 8.1 switcher](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win81alttab.png)

This mod recreates the Windows 7 and the Windows 8.1 Alt+Tab switchers on
Windows 10 and Windows 11, with live previews of the windows. It runs in its own
process and does not modify `explorer.exe`.

## What it does

- Draws the switcher the way Windows 7 or Windows 8.1 drew it; the style is
  chosen in the settings.
- Shows a live preview of every window, minimized windows included.
- Shows the icon of the application in the corner of every preview.
- Draws the Windows 7 panel as Aero glass and the Windows 8.1 one as the solid
  band it was.
- Does not page: the previews shrink until every window fits, as the originals
  did.
- Does not touch `explorer.exe` and does not ask for an Explorer restart.
- If the switcher cannot be shown, Alt+Tab keeps working as usual.

## Requirements

- Windows 10 or Windows 11, 64-bit.
- Windhawk installed, with `windhawk.exe` not excluded.

## How to use

1. Install the mod in Windhawk and enable it.
2. Press Alt+Tab. The switcher appears in the style chosen in the settings.
3. To remove it, disable or uninstall the mod.

## Keyboard shortcuts

| Shortcut | Action |
| --- | --- |
| `Alt+Tab`, `Alt+Shift+Tab` | Opens the switcher and moves the selection |
| `Alt` (release) | Activates the selected window |
| `Tab` / `Shift+Tab`, arrow keys, wheel | Moves the selection while open |
| `Enter` / `Space` / click | Activates the selected window |
| `Alt+F4` while open | Closes the selected window |
| `Esc` while open | Closes the switcher without switching |
| `Ctrl+Alt+F11` | Opens or closes the switcher manually (fallback) |
| ``Alt+` `` | Optional, off by default: windows of the same app only |
| `Ctrl+Alt+Tab`, `Win+Tab` | Left to Windows |
| `Ctrl+Shift+Esc` | Left to Windows, opens Task Manager |

## Styles

- **Windows 7**: a glass panel with rounded corners and a light sheen, the name
  of the selected window above the row, the previews in a row with the
  application icon in the corner, and a thin double frame around the selected
  preview. A minimized window shows its application icon.
- **Windows 8.1**: a solid colored band with square corners, a white title above
  the row, and the previews in a row with the application icon half outside
  their corner. The selected preview has a white frame.

## Comparison with other Alt+Tab mods

**Windows 7/8.x Alt+Tab Loader (aubymori)** loads the original Alt+Tab component
of Windows 7 or Windows 8.x. It is the most faithful option, and the hardest to
set up: files have to be copied into the system folder, a matching theme has to
be prepared, and Explorer has to be restarted when the mod is enabled and again
when it is disabled.

**Simple Window Switcher (Lone)** is a configurable window switcher, built
around Windows 10 and 11 style layouts and around other arrangements as well;
recreating the Windows 7 or the 8.1 interface is not its purpose.

**This mod** recreates those two interfaces and nothing else. Configuration is
reduced to the style and a few options, and no file has to be copied, no theme
prepared and no restart done. It takes some drawing ideas from the Simple Window
Switcher, but it works in a different way and its purpose is a narrower one.

## Settings

- **Switcher style**: Windows 7 or Windows 8.1.
- **Show the switcher on**: the primary monitor, the monitor with the cursor, or
  the monitor of the active window.
- **Panel color**: follow the Windows accent color, as both originals did, or
  use a custom color.
- **Custom accent color**: used only when the panel color is set to custom.
- **Glass density**: how much the Windows 7 panel hides the desktop behind it.
- **Desktop dimming**: how much the desktop behind the switcher is darkened.
- **Only windows of the current virtual desktop**: the filter of the originals.
  Minimized windows are always listed.
- **The Desktop card**: the Desktop at the end of the row, as both originals
  had.
- **Auto-cycle while Alt is held**, **Mouse navigation**, **Alt+\`**,
  **Ctrl+Alt+F11**: the optional extras.
- **Blurred glass behind the panel**: for the Windows 7 style.
- **Fade the switcher in and out**: 250 ms, off by default.

## Notes

- Use one Alt+Tab replacement at a time.
- The previews are live, not screenshots: they show the window as it is now.
- The desktop behind the switcher is a still image.
- Windows 11 tabbed File Explorer windows appear as one item per window.
- True fullscreen exclusive games may not show the switcher, like other overlay
  mods.
- The mod is released under the GPL-2.0 license.

## Credits

- The look of both styles follows the measurements of the two original
  switchers.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- paneStyle: windows-7
  $name: Switcher style
  $description: This setting chooses which Windows version the switcher is drawn like. It changes the geometry, not only the colors, because the two switchers laid their previews out differently.
  $options:
  - windows-7: Windows 7
  - windows-8.1: Windows 8.1
- paneMonitor: primary
  $name: Show the switcher on
  $description: This setting controls which monitor the switcher appears on. Windows 7 and Windows 8.1 showed it on the primary monitor.
  $options:
  - primary: The primary monitor (the Windows 7 and 8.1 behaviour)
  - cursor: The monitor with the mouse cursor
  - active-window: The monitor of the active window
- accentSource: system
  $name: Panel color
  $description: This setting chooses where the color of the panels comes from. By default the Windows 7 glass and the Windows 8.1 band follow the Windows accent color, which is what the two original switchers did.
  $options:
  - system: Follow the Windows accent color
  - custom: Use the custom color below
- customAccentColor: "#4F7FB8"
  $name: Custom accent color
  $description: This setting sets the color used when the panel color is set to custom, written as RRGGBB or RGB. It is ignored when the panel color follows Windows. A reddish color gives a warm glass and a bluish one a cold glass, as the accent color did in Windows 7.
- frostDensity: 72
  $name: Glass density (%)
  $description: This setting controls how much the Windows 7 glass hides the desktop. The panel colors what is behind it with the system color and blurs it, so the color stays the accent color chosen in the Windows personalisation settings. 72 % is the density of the original, 100 makes the panel opaque, 0 makes it disappear.
- desktopShade: 0
  $name: Desktop dimming (%)
  $description: This setting controls how much the whole desktop behind the switcher is darkened. 0 is the default and matches both originals, which never dimmed the desktop.
- thisDesktopOnly: true
  $name: Only windows of the current virtual desktop
  $description: This setting lists only the windows of the virtual desktop that is currently shown, like the original switcher.
- cycleOnHold: false
  $name: Auto-cycle while Alt is held
  $description: This setting automatically advances the selection every 250 ms while Alt is held, in addition to Tab and Shift+Tab. Off by default.
- pointerSelect: true
  $name: Mouse navigation
  $description: This setting allows moving the selection with the mouse wheel and activating a window by clicking its preview.
- onlyThisAppHotkey: false
  $name: Alt+` switches between windows of the same app
  $description: This setting adds an extra shortcut that opens the switcher with only the windows of the application of the active window. Off by default.
- manualHotkey: true
  $name: Ctrl+Alt+F11 opens the switcher
  $description: This setting adds a fallback hotkey that opens or closes the switcher manually, useful when another program interferes with Alt+Tab.
- systemBlur: auto
  $name: Blurred glass behind the panel
  $description: This setting blurs the desktop behind the Windows 7 panel, which is what makes it look like real Aero glass. The Windows 8.1 band is opaque and is not affected.
  $options:
  - auto: Automatic (composition accent, then DWM blur)
  - dwm: Only the documented DWM blur
  - off: No blur, transparency only
- openFade: false
  $name: Fade the switcher in and out
  $description: This setting animates the panel from 25% to 100% opacity in 250 ms when it opens, and back when it is cancelled with Esc. Off by default, because the original switchers appeared instantly.
- desktopTile: true
  $name: The Desktop card
  $description: This setting adds the Desktop at the end of the row, as Windows 7 and Windows 8.1 did. It shows the desktop itself as its preview, uses the icon the shell uses for the desktop, and selecting it shows the desktop, like the Show desktop button on the taskbar.
*/
// ==/WindhawkModSettings==
// -----------------------------------------------------------------------------
// Note di progetto
//
// Questa mod non ripristina lo switcher di Windows 7 e 8.1: lo ridisegna. Non
// ricarica componenti interne del sistema e non aggancia niente dentro
// explorer.exe; gira nel processo che Windhawk avvia per le mod di questo tipo
// e disegna il pannello con API documentate. Da qui vengono le proprietà che
// contano per chi la usa:
//
//  1. Niente viene iniettato nella shell e niente al suo interno viene
//     agganciato: un errore nella mod non può portarsi via explorer.exe, e
//     attivare o disattivare la mod non lascia il sistema in uno stato che
//     richiede un riavvio della shell.
//  2. Il pannello è una normale finestra topmost del processo della mod, e le
//     anteprime sono miniature DWM registrate fra quella finestra e le finestre
//     di origine: API documentate, presenti da Windows 7 a Windows 11.
//  3. Alt+Tab viene preso con un hook di tastiera di basso livello che vive nel
//     processo della mod. L'hook classifica soltanto i tasti; il lavoro vero
//     avviene sul thread dell'interfaccia, così la callback resta breve e non
//     viene scartata dal sistema per lentezza.
//  4. La geometria non è inventata: altezze, distanze, spessori dei fili e
//     colori sono misurati fotogramma per fotogramma sugli switcher originali,
//     e ogni valore viene poi scalato con il DPI del monitor.
//  5. Tutto è difensivo: se il pannello non riesce a comparire la mod rimanda
//     Alt+Tab al sistema, se l'hook smette di essere chiamato un watchdog lo
//     reinstalla, e una sessione rimasta aperta senza Alt premuto (un tasto
//     perso) si chiude da sola.
//
// Perché ridisegnarlo invece di ripristinare l'originale
//
// Ripristinare lo switcher originale vuol dire riusare i componenti interni di
// explorer.exe, una strada già tentata e abbandonata: dipende da una lunga
// lista di simboli non documentati che cambiano da build a build, quindi si
// rompe a ogni aggiornamento e non si può verificare su ogni build.
// Ridisegnarlo con API documentate (miniature DWM, GDI, un hook di tastiera)
// funziona attraverso le build e non tocca la shell.
// -----------------------------------------------------------------------------

#include <windhawk_api.h>

#include <windows.h>
#include <dwmapi.h>
#include <objbase.h>
#include <shellapi.h>
#include <commctrl.h>  // ImageList_Create, ImageList_ReplaceIcon, ImageList_Draw

#include <algorithm>
#include <atomic>
#include <gdiplus.h>   // solo per leggere lo sfondo del desktop
#include <shldisp.h>   // IShellDispatch4::ToggleDesktop
#include <shlobj.h>    // FOLDERID_Desktop, SHGetNameFromIDList
#include <shobjidl.h>  // SHCreateItemFromParsingName
#include <shlwapi.h>   // SHCreateMemStream, per leggere il PNG incorporato
#include <cmath>     // pow(), per la correzione di gamma del testo
#include <cstdint>
#include <cwchar>
#include <mutex>
#include <string>
#include <vector>

#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif

#ifndef CAPTUREBLT
#define CAPTUREBLT 0x40000000
#endif

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

constexpr GUID kCLSID_VirtualDesktopManager = {
    0xaa509086, 0x5ca9, 0x4c25,
    {0x8f, 0x95, 0x58, 0x9d, 0x3c, 0x07, 0xb4, 0x8a}};
constexpr GUID kIID_IVirtualDesktopManager = {
    0xa5cd92ff, 0x29be, 0x454c,
    {0x8d, 0x04, 0xd8, 0x28, 0x79, 0xfb, 0x3f, 0x1b}};

#ifndef __IVirtualDesktopManager_INTERFACE_DEFINED__
struct IVirtualDesktopManager : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsWindowOnCurrentVirtualDesktop(
        HWND topLevelWindow,
        BOOL* onCurrentDesktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWindowDesktopId(HWND topLevelWindow,
                                                         GUID* desktopId) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveWindowToDesktop(
        HWND topLevelWindow,
        REFGUID desktopId) = 0;
};
#endif  // __IVirtualDesktopManager_INTERFACE_DEFINED__

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

constexpr UINT kMsgSwitchShow = WM_APP + 1;    // wParam: 0 next, 1 previous
constexpr UINT kMsgSwitchCommit = WM_APP + 2;  // Alt was released
constexpr UINT kMsgSwitchCancel = WM_APP + 3;  // Esc
constexpr UINT kMsgSwitchClose = WM_APP + 4;   // the Win key was pressed
constexpr UINT kMsgSwitchCloseSelected = WM_APP + 5;  // Alt+F4
constexpr UINT kMsgSettingsChanged = WM_APP + 6;
constexpr UINT kMsgSameApp = WM_APP + 7;
constexpr UINT kMsgFallbackToggle = WM_APP + 8;

constexpr UINT kMsgInputThreadQuit = WM_APP + 100;
constexpr UINT kMsgInputThreadRehook = WM_APP + 101;

constexpr UINT_PTR kAutoCycleTimerId = 1;
constexpr UINT_PTR kSessionAgeTimerId = 2;
constexpr UINT_PTR kRebuildTimerId = 3;
constexpr UINT_PTR kFadeTimerId = 4;     // l'animazione di comparsa
constexpr UINT kRebuildDelayMs = 500;
constexpr UINT kAutoCycleIntervalMs = 250;
constexpr UINT kSessionAgeIntervalMs = 200;

constexpr unsigned long long kSessionStuckLimitMs = 30000;

constexpr DWORD kWatchdogIntervalMs = 5000;

constexpr int kFallbackHotkeyVk = VK_F11;

// -----------------------------------------------------------------------------
// SwitcherSettings
// -----------------------------------------------------------------------------

enum class SwitcherStyle {
    Windows7,
    Windows81,
};

struct SwitcherSettings {
    SwitcherStyle style = SwitcherStyle::Windows7;
    std::wstring paneMonitor;
    int frostDensity = 72;
    int desktopShade = 0;
    bool thisDesktopOnly = true;
    bool cycleOnHold = false;
    bool pointerSelect = true;
    bool onlyThisAppHotkey = false;
    bool manualHotkey = true;
    // auto: prima l'attributo di composizione, poi la chiamata documentata;
    // dwm: solo DwmEnableBlurBehindWindow; off: nessuna sfocatura.
    std::wstring systemBlur = L"auto";
    // system: il pannello segue il colore di Windows; custom: usa quello qui
    // sotto. Il predefinito è il comportamento degli originali.
    std::wstring accentSource = L"system";
    std::wstring customAccentColor = L"#4F7FB8";
    bool openFade = false;  // dissolvenza in entrata e in uscita
    // La scheda del desktop in fondo alla fila, come nei due originali.
    bool desktopTile = true;
};

static SwitcherSettings g_settings;
static std::mutex g_settingsMutex;

static SwitcherStyle g_switcherStyle = SwitcherStyle::Windows7;

static std::wstring GetStringSettingOr(PCWSTR name,
                                     const wchar_t* fallback) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : fallback;
    Wh_FreeStringSetting(value);
    return result;
}

static std::wstring PickOneOf(const std::wstring& value,
                              std::initializer_list<const wchar_t*> allowed,
                              const wchar_t* fallback) {
    for (const wchar_t* candidate : allowed) {
        if (value == candidate) {
            return value;
        }
    }
    return fallback;
}

static void LoadSettings() {
    SwitcherSettings settings;

    // I valori vengono limitati qui, perché lo schema delle impostazioni non
    // dichiara limiti numerici di proposito: un limite che Windhawk non
    // riconosce fa fallire la lettura di tutto il blocco, e un valore
    // predefinito che l'utente non può correggere è peggio di un valore
    // riportato nei limiti.
    const std::wstring styleName =
        GetStringSettingOr(L"paneStyle", L"windows-7");
    settings.style =
        styleName == L"windows-8.1" ? SwitcherStyle::Windows81
                                    : SwitcherStyle::Windows7;

    settings.paneMonitor = PickOneOf(
        GetStringSettingOr(L"paneMonitor", L"primary"),
        {L"primary", L"cursor", L"active-window"}, L"primary");

    settings.frostDensity =
        std::clamp(Wh_GetIntSetting(L"frostDensity"), 0, 100);
    settings.desktopShade =
        std::clamp(Wh_GetIntSetting(L"desktopShade"), 0, 100);
    settings.thisDesktopOnly = Wh_GetIntSetting(L"thisDesktopOnly") != 0;
    settings.cycleOnHold =
        Wh_GetIntSetting(L"cycleOnHold") != 0;
    settings.pointerSelect = Wh_GetIntSetting(L"pointerSelect") != 0;
    settings.onlyThisAppHotkey = Wh_GetIntSetting(L"onlyThisAppHotkey") != 0;
    settings.manualHotkey = Wh_GetIntSetting(L"manualHotkey") != 0;
    settings.accentSource = PickOneOf(
        GetStringSettingOr(L"accentSource", L"system"),
        {L"system", L"custom"}, L"system");
    settings.customAccentColor =
        GetStringSettingOr(L"customAccentColor", L"#4F7FB8");
    settings.systemBlur = PickOneOf(
        GetStringSettingOr(L"systemBlur", L"auto"),
        {L"auto", L"dwm", L"off"}, L"auto");
    settings.openFade = Wh_GetIntSetting(L"openFade") != 0;
    settings.desktopTile =
        Wh_GetIntSetting(L"desktopTile") != 0;

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(settings);
}

static SwitcherSettings GetSettingsCopy() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// -----------------------------------------------------------------------------
// Global state
// -----------------------------------------------------------------------------

static std::atomic<bool> g_shuttingDown{false};
static std::atomic<bool> g_initFailed{false};
static std::atomic<HWND> g_overlayWindow{nullptr};
static std::atomic<bool> g_switcherVisible{false};
static std::atomic<bool> g_altDown{false};
static std::atomic<bool> g_ctrlDown{false};
static std::atomic<bool> g_shiftDown{false};
static std::atomic<bool> g_winDown{false};
static std::atomic<DWORD> g_forcePassthroughVk{0};
static std::atomic<unsigned long long> g_hookCallTick{0};
static std::atomic<bool> g_hookReinstallRequested{false};
static std::atomic<bool> g_onlyThisAppHotkeyEnabled{false};
static std::atomic<bool> g_manualHotkeyEnabled{true};

static HHOOK g_keyboardHook = nullptr;  // the keyboard hook thread only
static DWORD g_inputThreadId = 0;
static HANDLE g_uiThread = nullptr;
static HANDLE g_inputThread = nullptr;
static HANDLE g_watchdogThread = nullptr;
static HANDLE g_uiReadyEvent = nullptr;
static HANDLE g_hookReadyEvent = nullptr;
static HANDLE g_stopEvent = nullptr;

static void ApplySessionTimers();
static void RefreshThumbnails();
static void SendAltTabToSystem();
static void UpdateHookFlags(const SwitcherSettings& settings);

// -----------------------------------------------------------------------------
// Exception guard
// -----------------------------------------------------------------------------

static std::atomic<unsigned> g_exceptionCount{0};

#if defined(_MSC_VER) && !defined(__clang__)
#define WH_HAS_SEH 1
#else
#define WH_HAS_SEH 0
#endif

template <typename Function>
static bool RunGuarded(const wchar_t* step, Function&& function) {
    try {
        function();
        return true;
    } catch (...) {
        const unsigned count = g_exceptionCount.fetch_add(1) + 1;
        if (count <= 5) {
            Wh_Log(L"An exception escaped while %s; the mod continues without that step",
                   step);
        }
        return false;
    }
}

// -----------------------------------------------------------------------------
// Small resource wrappers
// -----------------------------------------------------------------------------

struct ScopedHandle {
    HANDLE value = nullptr;

    ScopedHandle() = default;
    explicit ScopedHandle(HANDLE handle) : value(handle) {}
    ~ScopedHandle() {
        reset();
    }

    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;

    ScopedHandle(ScopedHandle&& other) noexcept : value(other.value) {
        other.value = nullptr;
    }

    ScopedHandle& operator=(ScopedHandle&& other) noexcept {
        if (this != &other) {
            reset();
            value = other.value;
            other.value = nullptr;
        }
        return *this;
    }

    void reset(HANDLE handle = nullptr) {
        if (value) {
            CloseHandle(value);
        }
        value = handle;
    }

    explicit operator bool() const {
        return value != nullptr;
    }
};

struct ScopedScreenDC {
    HDC value = nullptr;

    ScopedScreenDC() = default;
    ScopedScreenDC(HWND window, HDC dc) {
        reset(dc, window);
    }

    ~ScopedScreenDC() {
        reset();
    }

    ScopedScreenDC(const ScopedScreenDC&) = delete;
    ScopedScreenDC& operator=(const ScopedScreenDC&) = delete;

    ScopedScreenDC(ScopedScreenDC&& other) noexcept {
        moveFrom(other);
    }

    ScopedScreenDC& operator=(ScopedScreenDC&& other) noexcept {
        if (this != &other) {
            reset();
            moveFrom(other);
        }
        return *this;
    }

    void moveFrom(ScopedScreenDC& other) {
        value = other.value;
        window_ = other.window_;
        other.value = nullptr;
    }

    void reset(HDC dc = nullptr, HWND window = nullptr) {
        if (value) {
            ReleaseDC(window_, value);
        }
        value = dc;
        window_ = window;
    }

    explicit operator bool() const {
        return value != nullptr;
    }

private:
    HWND window_ = nullptr;
};

struct ScopedWindowDC {
    HDC value = nullptr;

    ScopedWindowDC() = default;
    explicit ScopedWindowDC(HWND window) {
        if (window) {
            value = GetDC(window);
            window_ = window;
        }
    }

    ~ScopedWindowDC() {
        reset();
    }

    ScopedWindowDC(const ScopedWindowDC&) = delete;
    ScopedWindowDC& operator=(const ScopedWindowDC&) = delete;

    void reset() {
        if (value) {
            ReleaseDC(window_, value);
            value = nullptr;
        }
    }

    explicit operator bool() const {
        return value != nullptr;
    }

private:
    HWND window_ = nullptr;
};

struct ScopedBrush {
    HBRUSH value = nullptr;

    ScopedBrush() = default;
    explicit ScopedBrush(HBRUSH brush) : value(brush) {}
    ~ScopedBrush() {
        reset();
    }

    ScopedBrush(const ScopedBrush&) = delete;
    ScopedBrush& operator=(const ScopedBrush&) = delete;

    void reset(HBRUSH brush = nullptr) {
        if (value && value != brush) {
            DeleteObject(value);
        }
        value = brush;
    }

    explicit operator bool() const {
        return value != nullptr;
    }
};

struct ScopedBitmap {
    HBITMAP value = nullptr;

    ScopedBitmap() = default;
    explicit ScopedBitmap(HBITMAP bitmap) : value(bitmap) {}
    ~ScopedBitmap() {
        reset();
    }

    ScopedBitmap(const ScopedBitmap&) = delete;
    ScopedBitmap& operator=(const ScopedBitmap&) = delete;

    ScopedBitmap(ScopedBitmap&& other) noexcept : value(other.value) {
        other.value = nullptr;
    }

    ScopedBitmap& operator=(ScopedBitmap&& other) noexcept {
        if (this != &other) {
            reset(other.value);
            other.value = nullptr;
        }
        return *this;
    }

    void reset(HBITMAP bitmap = nullptr) {
        if (value && value != bitmap) {
            DeleteObject(value);
        }
        value = bitmap;
    }

    explicit operator bool() const {
        return value != nullptr;
    }
};

struct ScopedComApartment {
    bool initialized = false;

    ScopedComApartment() = default;
    explicit ScopedComApartment(bool succeeded) : initialized(succeeded) {}

    ~ScopedComApartment() {
        reset();
    }

    ScopedComApartment(const ScopedComApartment&) = delete;
    ScopedComApartment& operator=(const ScopedComApartment&) = delete;

    void reset(bool succeeded = false) {
        if (initialized) {
            CoUninitialize();
        }
        initialized = succeeded;
    }

    explicit operator bool() const {
        return initialized;
    }
};

struct ScopedDC {
    HDC value = nullptr;

    ScopedDC() = default;
    explicit ScopedDC(HDC dc) : value(dc) {}
    ~ScopedDC() {
        reset();
    }

    ScopedDC(const ScopedDC&) = delete;
    ScopedDC& operator=(const ScopedDC&) = delete;

    ScopedDC(ScopedDC&& other) noexcept : value(other.value) {
        other.value = nullptr;
    }

    ScopedDC& operator=(ScopedDC&& other) noexcept {
        if (this != &other) {
            reset(other.value);
            other.value = nullptr;
        }
        return *this;
    }

    void reset(HDC dc = nullptr) {
        if (value) {
            DeleteDC(value);
        }
        value = dc;
    }

    explicit operator bool() const {
        return value != nullptr;
    }
};

struct ScopedFont {
    HFONT value = nullptr;

    ScopedFont() = default;

    ~ScopedFont() {
        reset();
    }

    ScopedFont(const ScopedFont&) = delete;
    ScopedFont& operator=(const ScopedFont&) = delete;

    void reset(HFONT font = nullptr) {
        if (value && value != font) {
            DeleteObject(value);
        }
        value = font;
    }

    explicit operator bool() const {
        return value != nullptr;
    }
};

struct ScopedSelectedObject {
    HDC dc = nullptr;
    HGDIOBJ previous = nullptr;

    ScopedSelectedObject(HDC target, HGDIOBJ object) : dc(target) {
        if (dc && object) {
            previous = SelectObject(dc, object);
        }
    }

    ~ScopedSelectedObject() {
        if (dc && previous && previous != HGDI_ERROR) {
            SelectObject(dc, previous);
        }
    }

    ScopedSelectedObject(const ScopedSelectedObject&) = delete;
    ScopedSelectedObject& operator=(const ScopedSelectedObject&) = delete;
};

struct ScopedTextColor {
    HDC dc = nullptr;
    COLORREF previous = 0;

    ScopedTextColor(HDC target, COLORREF color) : dc(target) {
        if (dc) {
            previous = SetTextColor(dc, color);
        }
    }

    ~ScopedTextColor() {
        if (dc) {
            SetTextColor(dc, previous);
        }
    }

    ScopedTextColor(const ScopedTextColor&) = delete;
    ScopedTextColor& operator=(const ScopedTextColor&) = delete;
};

struct ScopedBkMode {
    HDC dc = nullptr;
    int previous = 0;

    ScopedBkMode(HDC target, int mode) : dc(target) {
        if (dc) {
            previous = SetBkMode(dc, mode);
        }
    }

    ~ScopedBkMode() {
        if (dc) {
            SetBkMode(dc, previous);
        }
    }

    ScopedBkMode(const ScopedBkMode&) = delete;
    ScopedBkMode& operator=(const ScopedBkMode&) = delete;
};

struct BitmapSurface {
    ScopedDC dc;
    ScopedBitmap bitmap;
    HGDIOBJ oldBitmap = nullptr;
    void* bits = nullptr;
    int width = 0;
    int height = 0;

    BitmapSurface() = default;
    ~BitmapSurface() {
        reset();
    }

    BitmapSurface(const BitmapSurface&) = delete;
    BitmapSurface& operator=(const BitmapSurface&) = delete;

    void reset() {
        if (dc.value && oldBitmap) {
            SelectObject(dc.value, oldBitmap);
            oldBitmap = nullptr;
        }
        bitmap.reset();
        dc.reset();
        bits = nullptr;
        width = 0;
        height = 0;
    }

    bool Ensure(int newWidth, int newHeight) {
        if (dc.value && width == newWidth && height == newHeight) {
            return true;
        }
        reset();
        if (newWidth <= 0 || newHeight <= 0) {
            return false;
        }

        ScopedScreenDC screen(nullptr, GetDC(nullptr));
        if (!screen) {
            return false;
        }

        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = newWidth;
        info.bmiHeader.biHeight = -newHeight;  // top-down
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;

        void* bitmapBits = nullptr;
        ScopedBitmap newBitmap(CreateDIBSection(screen.value, &info,
                                                DIB_RGB_COLORS, &bitmapBits,
                                                nullptr, 0));
        if (!newBitmap) {
            return false;
        }

        ScopedDC newDc(CreateCompatibleDC(screen.value));
        if (!newDc) {
            return false;  // the bitmap guard releases the bitmap
        }

        dc = std::move(newDc);
        bitmap = std::move(newBitmap);
        bits = bitmapBits;
        oldBitmap = SelectObject(dc.value, bitmap.value);
        width = newWidth;
        height = newHeight;
        return true;
    }

    explicit operator bool() const {
        return dc.value != nullptr && bitmap.value != nullptr;
    }
};

// -----------------------------------------------------------------------------
// Logging helpers
// -----------------------------------------------------------------------------

struct RateLimitedLog {
    const wchar_t* message;
    int limit;
    int count = 0;

    bool ShouldLog() {
        ++count;
        return count <= limit;
    }
};

// -----------------------------------------------------------------------------
// Window list
// -----------------------------------------------------------------------------

struct SwitcherItem {
    HWND hwnd = nullptr;
    std::wstring title;
    std::wstring appPath;
    HICON icon = nullptr;
    bool ownIcon = false;
    bool minimized = false;
    HTHUMBNAIL thumbnail = nullptr;
    bool hasThumbnail = false;
    // La voce "Desktop", che Windows 7 e 8.1 mettevano in fondo alla
    // fila: non è una finestra da attivare, è una scheda a sé.
    bool isDesktop = false;
    RECT card{};   // the whole card, in client coordinates
    RECT thumb{};  // where the live preview goes, in client coordinates

    SwitcherItem() = default;
    ~SwitcherItem() {
        Reset();
    }

    SwitcherItem(const SwitcherItem&) = delete;
    SwitcherItem& operator=(const SwitcherItem&) = delete;

    SwitcherItem(SwitcherItem&& other) noexcept {
        MoveFrom(other);
    }

    SwitcherItem& operator=(SwitcherItem&& other) noexcept {
        if (this != &other) {
            Reset();
            MoveFrom(other);
        }
        return *this;
    }

    void MoveFrom(SwitcherItem& other) {
        hwnd = other.hwnd;
        title = std::move(other.title);
        appPath = std::move(other.appPath);
        icon = other.icon;
        ownIcon = other.ownIcon;
        minimized = other.minimized;
        thumbnail = other.thumbnail;
        hasThumbnail = other.hasThumbnail;
        isDesktop = other.isDesktop;
        card = other.card;
        thumb = other.thumb;

        other.hwnd = nullptr;
        other.icon = nullptr;
        other.ownIcon = false;
        other.thumbnail = nullptr;
        other.hasThumbnail = false;
    }

    void ReleaseThumbnail() {
        if (hasThumbnail && thumbnail) {
            DwmUnregisterThumbnail(thumbnail);
        }
        thumbnail = nullptr;
        hasThumbnail = false;
    }

    void Reset() {
        ReleaseThumbnail();
        if (ownIcon && icon) {
            DestroyIcon(icon);
        }
        icon = nullptr;
        ownIcon = false;
    }
};

using InternalGetWindowTextFn = int(WINAPI*)(HWND, LPWSTR, int);
static InternalGetWindowTextFn g_internalGetWindowText = nullptr;

static void ResolveTitleApi() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        g_internalGetWindowText = reinterpret_cast<InternalGetWindowTextFn>(
            GetProcAddress(user32, "InternalGetWindowText"));
    }
    if (!g_internalGetWindowText) {
        Wh_Log(L"InternalGetWindowText isn't available; window titles are read with a timeout instead");
    }
}

static bool GetWindowTitle(HWND hwnd, std::wstring* title) {
    title->clear();

    if (g_internalGetWindowText) {
        WCHAR buffer[512] = {};
        const int length = g_internalGetWindowText(hwnd, buffer,
                                                   ARRAYSIZE(buffer));
        if (length > 0) {
            title->assign(buffer, static_cast<size_t>(length));
            return true;
        }
        return false;
    }

    DWORD_PTR result = 0;
    WCHAR buffer[512] = {};
    if (!SendMessageTimeoutW(hwnd, WM_GETTEXT, ARRAYSIZE(buffer),
                             reinterpret_cast<LPARAM>(buffer),
                             SMTO_ABORTIFHUNG | SMTO_NORMAL, 100, &result) ||
        result == 0) {
        return false;
    }

    title->assign(buffer, static_cast<size_t>(result));
    return true;
}

static bool GetWindowProcessPath(HWND hwnd, std::wstring* path) {
    path->clear();

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return false;
    }

    ScopedHandle process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                     pid));
    if (!process) {
        return false;
    }

    WCHAR buffer[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(buffer);
    if (!QueryFullProcessImageNameW(process.value, 0, buffer, &size)) {
        return false;
    }

    *path = buffer;
    return true;
}

struct FindUwpChildData {
    DWORD pid = 0;
};

static BOOL CALLBACK FindUwpChildProc(HWND hwnd, LPARAM lParam) {
    auto* data = reinterpret_cast<FindUwpChildData*>(lParam);

    WCHAR className[128] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (_wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid) {
            data->pid = pid;
            return FALSE;
        }
    }

    return TRUE;
}

static bool GetWindowApplicationPath(HWND hwnd, std::wstring* path) {
    if (!GetWindowProcessPath(hwnd, path)) {
        return false;
    }

    const wchar_t* fileName = wcsrchr(path->c_str(), L'\\');
    fileName = fileName ? fileName + 1 : path->c_str();
    if (_wcsicmp(fileName, L"ApplicationFrameHost.exe") != 0) {
        return true;
    }

    FindUwpChildData data;
    EnumChildWindows(hwnd, FindUwpChildProc, reinterpret_cast<LPARAM>(&data));
    if (!data.pid) {
        return true;
    }

    ScopedHandle process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                     data.pid));
    if (!process) {
        return true;
    }

    WCHAR buffer[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(buffer);
    if (QueryFullProcessImageNameW(process.value, 0, buffer, &size)) {
        *path = buffer;
    }
    return true;
}

constexpr int kIconSourceSize = 64;

static HICON LoadWindowIcon(HWND hwnd, const std::wstring& appPath,
                            bool* owned) {
    *owned = false;

    const WPARAM iconKinds[] = {ICON_BIG, ICON_SMALL2, ICON_SMALL};
    for (WPARAM kind : iconKinds) {
        DWORD_PTR result = 0;
        if (SendMessageTimeoutW(hwnd, WM_GETICON, kind, 0,
                                SMTO_ABORTIFHUNG | SMTO_NORMAL, 100,
                                &result) &&
            result) {
            return reinterpret_cast<HICON>(result);
        }
    }

    LONG_PTR classIcon = GetClassLongPtrW(hwnd, GCLP_HICON);
    if (!classIcon) {
        classIcon = GetClassLongPtrW(hwnd, GCLP_HICONSM);
    }
    if (classIcon) {
        return reinterpret_cast<HICON>(classIcon);
    }

    if (appPath.empty()) {
        return nullptr;
    }

    {
        HICON large = nullptr;
        const int size = kIconSourceSize;
        if (PrivateExtractIconsW(appPath.c_str(), 0, size, size, &large, nullptr,
                                 1, LR_DEFAULTCOLOR) > 0 &&
            large) {
            *owned = true;
            return large;
        }
    }

    SHFILEINFOW fileInfo{};
    if (SHGetFileInfoW(appPath.c_str(), 0, &fileInfo, sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON) &&
        fileInfo.hIcon) {
        *owned = true;
        return fileInfo.hIcon;
    }

    return nullptr;
}

#ifndef DWM_CLOAKED_SHELL
#define DWM_CLOAKED_SHELL 0x2
#endif

static DWORD GetWindowCloakReason(HWND hwnd) {
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked)))) {
        return cloaked;
    }
    return 0;
}

static IVirtualDesktopManager* g_virtualDesktopManager = nullptr;

static bool IsOnCurrentVirtualDesktop(HWND hwnd) {
    if (!g_virtualDesktopManager) {
        return true;
    }

    BOOL onCurrentDesktop = TRUE;
    if (FAILED(g_virtualDesktopManager->IsWindowOnCurrentVirtualDesktop(
            hwnd, &onCurrentDesktop))) {
        return true;
    }
    return onCurrentDesktop != FALSE;
}

static bool IsSwitcherWindow(HWND hwnd, bool includeOtherDesktops) {
    if (!hwnd || !IsWindowVisible(hwnd)) {
        return false;
    }

    if (GetAncestor(hwnd, GA_ROOTOWNER) != hwnd) {
        return false;
    }

    const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if (GetWindow(hwnd, GW_OWNER) && !(exStyle & WS_EX_APPWINDOW)) {
        return false;
    }

    if ((exStyle & WS_EX_TOOLWINDOW) || (exStyle & WS_EX_NOACTIVATE)) {
        return false;
    }

    const DWORD cloakReason = GetWindowCloakReason(hwnd);
    if (cloakReason != 0) {
        if (!includeOtherDesktops || cloakReason != DWM_CLOAKED_SHELL ||
            IsOnCurrentVirtualDesktop(hwnd)) {
            return false;
        }
    }

    if (GetWindowTextLengthW(hwnd) == 0) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || pid == GetCurrentProcessId()) {
        return false;
    }

    return true;
}

struct WindowEnumeration {
    const SwitcherSettings* settings = nullptr;
    const std::wstring* appPathFilter = nullptr;
    std::vector<SwitcherItem>* items = nullptr;
    int skipped = 0;
};

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* context = reinterpret_cast<WindowEnumeration*>(lParam);
    if (!context || !context->items || !context->settings) {
        return FALSE;
    }

    // La finestra della shell non è una finestra come le altre: la scheda
    // del desktop si costruisce a parte e non deve comparire due volte.
    if (hwnd == GetShellWindow()) {
        return TRUE;
    }

    if (!IsSwitcherWindow(hwnd, !context->settings->thisDesktopOnly)) {
        ++context->skipped;
        return TRUE;
    }

    const bool minimized = IsIconic(hwnd) != FALSE;

    if (context->settings->thisDesktopOnly &&
        !IsOnCurrentVirtualDesktop(hwnd)) {
        return TRUE;
    }

    SwitcherItem item;
    item.hwnd = hwnd;
    item.minimized = minimized;

    if (!GetWindowTitle(hwnd, &item.title) || item.title.empty()) {
        return TRUE;
    }

    GetWindowApplicationPath(hwnd, &item.appPath);

    if (context->appPathFilter && !context->appPathFilter->empty() &&
        _wcsicmp(item.appPath.c_str(), context->appPathFilter->c_str()) != 0) {
        return TRUE;
    }

    item.icon = LoadWindowIcon(hwnd, item.appPath, &item.ownIcon);
    context->items->push_back(std::move(item));
    return TRUE;
}

static bool BuildDesktopItem(SwitcherItem* item);
static bool ShowDesktopWithShell();

static void BuildWindowList(const SwitcherSettings& settings,
                            const std::wstring& appPathFilter,
                            std::vector<SwitcherItem>* items) {
    items->clear();

    WindowEnumeration context;
    context.settings = &settings;
    context.appPathFilter = &appPathFilter;
    context.items = items;

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&context));

    // La scheda del desktop, in fondo alla fila come nei due originali.
    // Nel modo "stessa applicazione" la fila contiene solo le finestre
    // di quell'applicazione, quindi il desktop non ci va.
    if (settings.desktopTile && appPathFilter.empty()) {
        SwitcherItem desktop;
        if (BuildDesktopItem(&desktop)) {
            items->push_back(std::move(desktop));
        }
    }

    Wh_Log(L"Window list built: %u window(s) in the switcher, %d candidate(s) skipped",
           static_cast<unsigned>(items->size()), context.skipped);
}

// -----------------------------------------------------------------------------
// Item geometry
// -----------------------------------------------------------------------------

// La scala per DPI serve già qui, ma la definizione sta più sotto, insieme al
// calcolo del layout.
static int ScaleForDpi(int value, int dpi);

// -----------------------------------------------------------------------------
// Le misure dello switcher di Windows 7 e di quello di Windows 8.1
//
// Le due versioni non differiscono solo per i colori: cambiano la geometria del
// pannello, l'altezza delle anteprime e il modo in cui la selezione viene
// disegnata. I valori qui sotto sono in pixel a 100% DPI e vengono scalati con
// ScaleForDpi quando vengono usati.
//
// Windows 7: misurato fotogramma per fotogramma su una registrazione 1280x720
// dello switcher originale. Con cinque finestre il pannello era largo 1074 px e
// alto 199 px, le anteprime erano alte 100 px, la riga cominciava 77 px sotto il
// bordo superiore del pannello, il titolo era una sola riga scura centrata e i
// bordi del pannello erano un contorno scuro con una riga chiara appena dentro.
//
// Windows 8.1: la banda è larga quanto lo schermo meno un piccolo margine,
// completamente opaca, con il titolo bianco centrato sopra la riga; le anteprime
// sono più alte di quelle di Windows 7 e la selezione è un riquadro chiaro
// distanziato dall'anteprima. Non c'è nessun bordo sulle anteprime non
// selezionate e nessuna etichetta sotto le anteprime.
// -----------------------------------------------------------------------------

// Windows 7, stile "vetro Aero".
constexpr int kWin7ThumbHeight      = 100;  // altezza dell'anteprima
constexpr int kWin7CellGapX         = 26;   // spazio orizzontale tra le celle
constexpr int kWin7CellGapY         = 24;   // spazio verticale tra le celle
constexpr int kWin7PadLeft          = 25;   // margine interno sinistro
constexpr int kWin7PadRight         = 15;   // margine interno destro
constexpr int kWin7PadTop           = 15;   // margine interno superiore
constexpr int kWin7PadBottom        = 18;   // margine interno inferiore
constexpr int kWin7TitleBand        = 59;   // fascia del titolo sopra la griglia
constexpr int kWin7TitleOffset      = 6;    // dal bordo interno al testo
constexpr int kWin7TitleFontSize    = 27;   // altezza della riga di titolo
constexpr int kWin7TitleInkTop      = 26;   // sommità dell'inchiostro, dal bordo alto
constexpr int kWin7CornerRadius     = 8;    // raggio degli angoli del pannello
constexpr int kWin7SelectionWidth   = 1;    // filo del riquadro di selezione
constexpr int kWin7SelectionGap     = 6;    // distanza del riquadro dall'anteprima
constexpr int kWin7SelectionRadius  = 3;    // angoli del riquadro
// Il riquadro di selezione dell'originale non è un filo solo: sono due
// adiacenti, uno scuro fuori e uno chiaro subito dentro. Misurati sul
// fotogramma: scuro RGB(123,158,190) sopra un vetro RGB(178,218,254), cioè
// nero al 30 %; chiaro RGB(223,241,255), cioè bianco al 60 %. È questa
// coppia a dare il bordo definito che con un filo solo non si vedeva.
constexpr COLORREF kWin7SelectDarkColor  = RGB(0, 0, 0);
constexpr int      kWin7SelectDarkAlpha  = 100;
constexpr COLORREF kWin7SelectLightColor = RGB(255, 255, 255);
constexpr int      kWin7SelectLightAlpha = 150;
// I due fili però non bastano: sul fotogramma il riquadro si legge perché è un
// incavo, non perché è una linea. Dentro il riquadro il vetro è più chiaro che
// fuori (luminanza 215 contro 203) e l'anteprima getta la propria ombra, che
// scende a luminanza 80 subito sotto il suo bordo e risale fino al filo in sei
// pixel. Senza queste due cose i fili restano due righe appoggiate sul vetro.
constexpr int kWin7WellAlpha         = 22;   // schiarita del vetro dentro il riquadro
constexpr int kWin7ThumbShadowAlpha  = 135;  // ombra dell'anteprima sul suo bordo
constexpr int kWin7ThumbShadowRadius = 2;    // angoli dell'ombra dell'anteprima
constexpr int kWin7ThumbShadowTopPercent = 30;  // quanto vale l'ombra sopra
constexpr int kWin7GlowWidth        = 0;    // nessun alone: il riquadro è netto
constexpr int kWin7IconSize         = 32;   // icona dell'applicazione
constexpr int kWin7IconInset        = 4;    // quanto rientra nell'angolo
constexpr int kWin7MinThumbPercent  = 10;   // MinThumbSizePcent dell'originale
constexpr int kWin7MinWidth         = 360;  // MinWidthPx dell'originale
constexpr int kWin7MaxPercent       = 92;   // quota massima dello schermo
constexpr int kWin7MaxColumns       = 7;    // Win7 non superava le sette colonne
constexpr int kWin7ShadowSpread     = 9;    // ombra morbida attorno al pannello
constexpr int kWin7ShadowAlpha      = 170;  // opacità dell'ombra sul bordo

// Windows 8.1, banda indaco opaca.
constexpr int kW81ThumbHeight       = 108;
constexpr int kW81CellGap           = 32;   // spazio tra una cella e l'altra
constexpr int kW81PadLeft           = 30;
constexpr int kW81PadRight          = 30;
constexpr int kW81PadTop            = 30;
constexpr int kW81PadBottom         = 56;   // spazio sotto la riga
constexpr int kW81TitleBand         = 57;   // fascia del titolo sopra la riga
constexpr int kW81TitleOffset       = 12;
constexpr int kW81TitleInkTop       = 42;   // sommità dell'inchiostro, dal bordo alto
constexpr int kW81TitleFontSize     = 21;
constexpr int kW81CornerRadius      = 0;    // la banda dell'originale è squadrata
constexpr int kW81FramePad          = 9;    // distanza del riquadro dall'anteprima
constexpr int kW81FrameWidth        = 2;
constexpr int kW81IconSize          = 32;   // centrata sull'angolo: metà fuori
constexpr int kW81IconInset         = -16;  // negativo: sporge dall'angolo
constexpr int kW81MinThumbPercent   = 10;
constexpr int kW81MinWidth          = 0;
constexpr int kW81MinRowHeight      = 48;
constexpr int kW81MaxPercent        = 96;   // la banda arriva quasi ai bordi
constexpr int kW81MaxColumns        = 0;    // nessun limite: si va a capo

// Colori dello stile Windows 7. Il pannello è vetro: il colore viene dalla
// colorizzazione di Windows (DwmGetColorizationColor), quindi non è mai un
// valore fisso, e l'alfa resta basso perché sotto c'è il desktop sfocato.
constexpr COLORREF kWin7GlassTint   = RGB(226, 240, 250);  // ripiego senza Aero
// Quanto bianco si mescola nel colore di sistema per ottenere il vetro di
// Windows 7: il vetro era colorato ma chiaro, non una tinta piena.
// Il vetro di Windows 7 era *colorato*, non slavato: il colore di sistema si
// vedeva bene, alleggerito appena. Con troppo bianco dentro il pannello diventa
// una tinta pastello e non sembra più vetro.
constexpr int      kFrostWhitePercent  = 8;    // 0 = colore pieno, 100 = bianco
// Nel fotogramma il pannello è quasi tutto colore di sistema: il fondale si
    // vede, ma come modulazione, non come immagine. Con una quota più bassa il
    // pannello prende il colore di quello che c'è dietro e sembra sporco.
constexpr int      kFrostColorMix      = 84;   // quanta tinta sopra il fondale
constexpr int      kFrostLift         = 2;    // schiarita dovuta alla sfocatura
// La texture generata: grana finissima e due riflessi orizzontali. Serve a far
// leggere il pannello come vetro smerigliato anche quando il fondale non è
// disponibile, senza dipendere da nessuna API di sfocatura.
constexpr int      kGlassGrainAlpha    = 7;    // ampiezza della grana, in livelli
constexpr int      kGlassGlossTopAlpha = 46;   // riflesso forte sotto il bordo alto
constexpr int      kGlassGlossMidAlpha = 16;   // riflesso debole a un quinto
constexpr int      kWin7FrostAlpha  = 184;  // 72% del pieno: vetro, non una lastra
// Il bordo del pannello, misurato sul fotogramma dell'originale: due pixel
// scuri all'esterno e due pixel quasi bianchi subito dentro. L'ordine è questo,
// non il contrario: è quello che fa leggere il pannello come vetro spesso.
constexpr int      kWin7BorderDark  = 2;    // filo scuro esterno
constexpr int      kWin7BorderLight = 2;    // riga chiara interna
constexpr int      kWin7BorderAlpha = 190;  // opacità del filo scuro
constexpr int      kWin7InnerAlpha  = 210;  // opacità della riga chiara
constexpr int      kWin7SheenAlpha  = 34;   // riflesso: tenue, non latteo
constexpr int      kWin7SheenPercent = 35;  // quanto scende il riflesso
constexpr int      kWin7BottomShade = 26;   // ombra interna in basso
constexpr COLORREF kWin7TitleColor  = RGB(16, 20, 26);

// Colori dello stile Windows 8.1, letti dai pixel dell'immagine di riferimento:
// la banda è RGB(36, 4, 110) e non è traslucida.
// Ripiego per la banda 8.1 quando il sistema non risponde: è il colore
// misurato sul fotogramma di riferimento (indaco, l'accento predefinito di
// Windows 8). Non è più un colore fisso: la banda segue il colore di sistema.
constexpr COLORREF kW81PanelColor   = RGB(36, 4, 110);
// Il canale più alto della banda: il colore di sistema viene portato a questa
// profondità, che è quella misurata sul fotogramma, conservando tinta e
// saturazione scelte dall'utente.
constexpr int      kW81BandValue       = 115;
constexpr COLORREF kW81TitleColor   = RGB(236, 232, 246);
constexpr COLORREF kW81FrameColor   = RGB(240, 245, 255);
constexpr int      kW81FrameAlpha   = 235;
constexpr int      kW81FrameFillAlpha = 20;  // velo bianco dentro il riquadro

// Tutto quello che il layout deve sapere di uno stile, risolto una volta sola
// per ogni fotogramma.
struct StyleMetrics {
    int padTop = 0;
    int padBottom = 0;
    int padLeft = 0;
    int padRight = 0;
    int titleBand = 0;          // dalla sommità interna alla prima riga
    int titleOffset = 0;        // dall'interno del pannello al testo
    int titleFontSize = 0;
    int titleInkTop = -1;       // sommità dell'inchiostro, dal bordo del pannello
    int cellGapX = 0;
    int cellGapY = 0;
    int contour = 0;            // spessore del riquadro di selezione
    int framePad = 0;           // distanza del riquadro dall'anteprima
    int glow = 0;               // alone esterno
    int iconSize = 0;
    int iconInset = 0;          // pixel di rientro dell'icona nell'angolo
    int selectionGap = 0;       // distanza del riquadro dall'anteprima
    int selectionRadius = 0;
    int cornerRadius = 0;
    int startRowHeight = 0;     // altezza dell'anteprima alla prima iterazione
    int minRowHeight = 0;
    int minRowHeightPercent = 0;
    int minPanelWidth = 0;
    int maxWidthPercent = 92;
    int maxHeightPercent = 92;
    int columns = 0;            // 0: nessun limite di colonne
    int maxTileAspectPercent = 250;
    int minPreviewWidth = 48;
    int minPanelPercent = 0;    // larghezza minima del pannello, in % dello schermo
    bool opaque = false;        // la banda dell'8.1 è opaca, il vetro del 7 no
};

static StyleMetrics MetricsFor(SwitcherStyle style, const SwitcherSettings& settings,
                               int dpi) {
    (void)settings;
    StyleMetrics metrics;

    if (style == SwitcherStyle::Windows81) {
        metrics.padTop = ScaleForDpi(kW81PadTop, dpi);
        metrics.padBottom = ScaleForDpi(kW81PadBottom, dpi);
        metrics.padLeft = ScaleForDpi(kW81PadLeft, dpi);
        metrics.padRight = ScaleForDpi(kW81PadRight, dpi);
        metrics.titleBand = ScaleForDpi(kW81TitleBand, dpi);
        metrics.titleOffset = ScaleForDpi(kW81TitleOffset, dpi);
        metrics.titleFontSize = ScaleForDpi(kW81TitleFontSize, dpi);
    metrics.titleInkTop = ScaleForDpi(kW81TitleInkTop, dpi);
        metrics.cellGapX = ScaleForDpi(kW81CellGap, dpi);
        metrics.cellGapY = ScaleForDpi(kW81CellGap, dpi);
        metrics.contour = std::max(1, ScaleForDpi(kW81FrameWidth, dpi));
        metrics.framePad = ScaleForDpi(kW81FramePad, dpi);
        metrics.glow = 0;
        metrics.iconSize = ScaleForDpi(kW81IconSize, dpi);
        metrics.iconInset = ScaleForDpi(kW81IconInset, dpi);
        metrics.selectionGap = 0;
        metrics.selectionRadius = 0;
        metrics.cornerRadius = ScaleForDpi(kW81CornerRadius, dpi);
        metrics.startRowHeight = ScaleForDpi(kW81ThumbHeight, dpi);
        metrics.minRowHeight = ScaleForDpi(kW81MinRowHeight, dpi);
        metrics.minRowHeightPercent = kW81MinThumbPercent;
        metrics.minPanelWidth = ScaleForDpi(kW81MinWidth, dpi);
        metrics.maxWidthPercent = kW81MaxPercent;
        metrics.maxHeightPercent = 92;
        metrics.columns = kW81MaxColumns;
        metrics.maxTileAspectPercent = 250;
        metrics.minPreviewWidth = ScaleForDpi(48, dpi);
        // La banda dell'8.1 si stringe sul contenuto, come quella del 7: nel
        // fotogramma di riferimento era larga 1089 px con cinque anteprime che
        // ne occupavano 1088, cioè esattamente il contenuto più i margini. Con
        // poche finestre deve restare piccola, non attraversare lo schermo.
        metrics.minPanelPercent = 0;
        metrics.opaque = true;
        return metrics;
    }

    metrics.padTop = ScaleForDpi(kWin7PadTop, dpi);
    metrics.padBottom = ScaleForDpi(kWin7PadBottom, dpi);
    metrics.padLeft = ScaleForDpi(kWin7PadLeft, dpi);
    metrics.padRight = ScaleForDpi(kWin7PadRight, dpi);
    metrics.titleBand = ScaleForDpi(kWin7TitleBand, dpi);
    metrics.titleOffset = ScaleForDpi(kWin7TitleOffset, dpi);
    metrics.titleFontSize = ScaleForDpi(kWin7TitleFontSize, dpi);
    metrics.titleInkTop = ScaleForDpi(kWin7TitleInkTop, dpi);
    metrics.cellGapX = ScaleForDpi(kWin7CellGapX, dpi);
    metrics.cellGapY = ScaleForDpi(kWin7CellGapY, dpi);
    metrics.contour = std::max(1, ScaleForDpi(kWin7SelectionWidth, dpi));
    metrics.framePad = 0;
    metrics.glow = ScaleForDpi(kWin7GlowWidth, dpi);
    metrics.iconSize = ScaleForDpi(kWin7IconSize, dpi);
    metrics.iconInset = ScaleForDpi(kWin7IconInset, dpi);
    metrics.selectionGap = ScaleForDpi(kWin7SelectionGap, dpi);
    metrics.selectionRadius = ScaleForDpi(kWin7SelectionRadius, dpi);
    metrics.cornerRadius = ScaleForDpi(kWin7CornerRadius, dpi);
    metrics.startRowHeight = ScaleForDpi(kWin7ThumbHeight, dpi);
    // Il vero minimo viene da MinThumbSizePcent, una percentuale dell'area di
    // lavoro: questo è solo un ripiego per i monitor molto piccoli.
    metrics.minRowHeight = ScaleForDpi(40, dpi);
    metrics.minRowHeightPercent = kWin7MinThumbPercent;
    metrics.minPanelWidth = ScaleForDpi(kWin7MinWidth, dpi);
    metrics.maxWidthPercent = kWin7MaxPercent;
    metrics.maxHeightPercent = kWin7MaxPercent;
    metrics.columns = kWin7MaxColumns;
    metrics.maxTileAspectPercent = 250;
    metrics.minPreviewWidth = ScaleForDpi(48, dpi);
    metrics.opaque = false;
    return metrics;
}

// La geometria di una voce: la cella (che è anche l'area sensibile al mouse),
// il riquadro dell'anteprima e l'angolo in cui va l'icona.
struct ItemGeometry {
    RECT cell{};   // la voce, riquadro di selezione escluso
    RECT thumb{};  // l'anteprima
    RECT icon{};   // dove va l'icona dell'applicazione
};

struct SwitcherLayout {
    int firstIndex = 0;  // indice, nell'elenco, della prima voce disegnata
    int pageSize = 1;
    int pageCount = 1;
    int rowHeight = 0;   // altezza della cella
    int thumbH = 0;      // altezza dell'anteprima
    int cellH = 0;
    int cellPad = 0;
    int titleBand = 0;
    int titleGap = 0;
    int contour = 0;
    int framePad = 0;
    int glow = 0;
    int iconSize = 0;
    int iconInset = 0;
    int selectionGap = 0;
    int selectionRadius = 0;
    int radius = 0;
    int titleFontSize = 0;
    int titleInkTop = -1;
    int titleOffset = 0;
    int cellGap = 0;
    int cellGapX = 0;
    int cellGapY = 0;
    int panelTitleBand = 0;
    int panelLabelBand = 0;
    SwitcherStyle style = SwitcherStyle::Windows7;
    bool opaquePanel = false;
    RECT panel{};
    std::vector<ItemGeometry> items;
};

static int ScaleForDpi(int value, int dpi) {
    return MulDiv(value, dpi, 96);
}

// Il rapporto d'aspetto del client della finestra: la larghezza dell'anteprima
// lo segue, come nelle due versioni originali.
static double WindowAspect(HWND hwnd) {
    double aspect = 16.0 / 9.0;

    RECT client{};
    if (hwnd && GetClientRect(hwnd, &client)) {
        const int width = client.right - client.left;
        const int height = client.bottom - client.top;
        if (width > 0 && height > 0) {
            aspect = static_cast<double>(width) / height;
        }
    }

    return std::clamp(aspect, 0.5, 3.0);
}

// Dispone le voci e restituisce la geometria dell'intero switcher.
//
// Né Windows 7 né Windows 8.1 sfogliavano: le celle venivano rimpicciolite
// finché tutto entrava nello schermo. Il ciclo qui sotto fa lo stesso, partendo
// dall'altezza misurata dello stile e scendendo di un decimo per volta.
static SwitcherLayout ComputeLayout(const SwitcherSettings& settings,
                            const std::vector<SwitcherItem>& items,
                            int pageIndex,
                            const RECT& area,
                            int dpi) {
    SwitcherLayout layout;

    const StyleMetrics metrics = MetricsFor(settings.style, settings, dpi);

    layout.style = settings.style;
    layout.opaquePanel = metrics.opaque;
    layout.cellPad = 0;
    layout.titleBand = metrics.titleBand;
    layout.titleGap = 0;
    layout.contour = metrics.contour;
    layout.framePad = metrics.framePad;
    layout.glow = metrics.glow;
    layout.iconSize = metrics.iconSize;
    layout.iconInset = metrics.iconInset;
    layout.selectionGap = metrics.selectionGap;
    layout.selectionRadius = metrics.selectionRadius;
    layout.radius = metrics.cornerRadius;
    layout.titleFontSize = metrics.titleFontSize;
    layout.titleInkTop = metrics.titleInkTop;
    layout.cellGap = metrics.cellGapX;
    layout.cellGapX = metrics.cellGapX;
    layout.cellGapY = metrics.cellGapY;
    // Distanze dal bordo superiore del pannello, che è come le usa il disegno.
    layout.panelTitleBand = metrics.padTop + metrics.titleBand;
    layout.panelLabelBand = 0;
    layout.titleOffset = metrics.padTop + metrics.titleOffset;

    const int count = static_cast<int>(items.size());
    const int columns = metrics.columns > 0
        ? std::max(1, std::min(metrics.columns, count))
        : std::max(1, count);
    const int rowsPerPage = std::max(1, (count + columns - 1) / columns);
    layout.pageSize = columns * rowsPerPage;
    layout.pageCount = std::max(1, (count + layout.pageSize - 1) / layout.pageSize);

    const int page = std::clamp(pageIndex, 0, layout.pageCount - 1);
    const int firstIndex = page * layout.pageSize;
    const int onPage = std::min(layout.pageSize, std::max(0, count - firstIndex));
    if (onPage <= 0) {
        return layout;
    }

    const int areaW = area.right - area.left;
    const int areaH = area.bottom - area.top;
    const int contentLimit =
        areaW * metrics.maxWidthPercent / 100 - metrics.padLeft - metrics.padRight;
    const int heightLimit = areaH * metrics.maxHeightPercent / 100 -
        metrics.padTop - metrics.padBottom - metrics.titleBand;
    // L'originale non scendeva mai sotto MinThumbSizePcent dell'area di lavoro,
    // qualunque fosse il numero di finestre.
    const int minRowHeight = std::max(
        metrics.minRowHeight,
        static_cast<int>(areaH) * metrics.minRowHeightPercent / 100);
    const int minPreviewWidth = metrics.minPreviewWidth;

    int rowHeight = std::max(1, metrics.startRowHeight);

    for (int attempt = 0; attempt < 64; ++attempt) {
        const int thumbH = std::max(1, rowHeight);

        // Ogni anteprima ha l'altezza comune e la larghezza che le dà il
        // rapporto d'aspetto della sua finestra, senza superare due volte e
        // mezzo l'altezza e senza ingrandire finestre più strette.
        std::vector<int> widths;
        widths.reserve(onPage);
        for (int i = 0; i < onPage; ++i) {
            const SwitcherItem& item = items[firstIndex + i];

            int width = static_cast<int>(WindowAspect(item.hwnd) * thumbH + 0.5);
            width = std::min(width, thumbH * metrics.maxTileAspectPercent / 100);
            width = std::max(width, std::min(minPreviewWidth, thumbH));

            RECT client{};
            if (item.hwnd && GetClientRect(item.hwnd, &client)) {
                const int windowWidth = client.right - client.left;
                if (windowWidth > 0) {
                    width = std::min(width, windowWidth);
                }
            }

            widths.push_back(std::max(width, 1));
        }

        // Le righe si riempiono rispettando il numero massimo di colonne: con
        // sette finestre l'originale teneva una riga sola e rimpiccioliva le
        // anteprime, non ne aggiungeva una seconda.
        std::vector<std::vector<int>> rows;
        std::vector<int> rowWidths;
        std::vector<int> row{};
        int rowWidth = 0;
        for (int i = 0; i < onPage; ++i) {
            const int cellW = widths[i];
            if (static_cast<int>(row.size()) >= columns) {
                rows.push_back(row);
                rowWidths.push_back(rowWidth);
                row.clear();
                rowWidth = 0;
            }
            rowWidth += (row.empty() ? 0 : layout.cellGapX) + cellW;
            row.push_back(i);
        }
        if (!row.empty()) {
            rows.push_back(row);
            rowWidths.push_back(rowWidth);
        }

        const int contentW =
            *std::max_element(rowWidths.begin(), rowWidths.end());
        const int contentH = static_cast<int>(rows.size()) * thumbH +
            (static_cast<int>(rows.size()) - 1) * layout.cellGapY;

        if ((contentW <= contentLimit && contentH <= heightLimit) ||
            rowHeight <= minRowHeight) {
            // Tutto entra, oppure non si può più rimpicciolire.

            const int panelW = std::max(
                std::max(contentW + metrics.padLeft + metrics.padRight,
                         metrics.minPanelWidth),
                areaW * metrics.minPanelPercent / 100);
            const int panelH = contentH + metrics.padTop +
                metrics.titleBand + metrics.padBottom;

            // La posizione è quella della specifica: metà dello spazio libero
            // in orizzontale e il 35% in verticale.
            int panelX = static_cast<int>(area.left) + (areaW - panelW) / 2;
            int panelY = static_cast<int>(area.top) + (areaH - panelH) * 35 / 100;
            // I membri di RECT sono LONG: vanno riportati a int prima di
            // std::clamp, che vuole un solo tipo dedotto.
            const int minX = static_cast<int>(area.left);
            const int minY = static_cast<int>(area.top);
            panelX = std::clamp(panelX, minX,
                                std::max(minX, static_cast<int>(area.right) - panelW));
            panelY = std::clamp(panelY, minY,
                                std::max(minY, static_cast<int>(area.bottom) - panelH));
            layout.panel = RECT{panelX, panelY, panelX + panelW,
                                panelY + panelH};

            layout.firstIndex = firstIndex;
            layout.rowHeight = rowHeight;
            layout.thumbH = thumbH;
            layout.cellH = thumbH;
            layout.items.assign(onPage, ItemGeometry{});

            int y = layout.panel.top + metrics.padTop + metrics.titleBand;
            for (size_t r = 0; r < rows.size(); ++r) {
                // La riga è centrata nel pannello, non nel contenuto: con la
                // banda a tutta larghezza le due cose non coincidono.
                int x = layout.panel.left + (panelW - rowWidths[r]) / 2;
                for (int index : rows[r]) {
                    ItemGeometry& geometry = layout.items[index];
                    const int cellW = widths[index];

                    geometry.cell = RECT{x, y, x + cellW, y + thumbH};
                    geometry.thumb = geometry.cell;
                    // L'icona sta nell'angolo in basso a destra; con un inset
                    // negativo (stile 8.1) sporge di metà oltre l'angolo.
                    geometry.icon = RECT{
                        geometry.thumb.right - metrics.iconSize -
                            metrics.iconInset,
                        geometry.thumb.bottom - metrics.iconSize -
                            metrics.iconInset,
                        geometry.thumb.right - metrics.iconInset,
                        geometry.thumb.bottom - metrics.iconInset};

                    x += cellW + layout.cellGapX;
                }
                y += thumbH + layout.cellGapY;
            }
            break;
        }

        rowHeight = std::max(minRowHeight, rowHeight * 9 / 10);
    }

    return layout;
}

// -----------------------------------------------------------------------------
// Superficie premoltiplicata
//
// Il pannello non è più una finestra normale su cui si fa BitBlt dello schermo:
// è una finestra a livelli il cui contenuto viene composto pixel per pixel in un
// DIB a 32 bpp e poi consegnato a DWM con UpdateLayeredWindow(ULW_ALPHA).
// In quel formato ogni pixel è 0xAARRGGBB *premoltiplicato*, cioè con i canali
// di colore già moltiplicati per l'alfa: è l'unica rappresentazione con cui
// DWM sa comporre un livello con angoli arrotondati e trasparenze morbide.
//
// Le regole di compositing qui sotto sono quelle documentate da Microsoft per
// AlphaBlend/AC_SRC_OVER:  out = src + dst * (1 - srcA).
// -----------------------------------------------------------------------------

static inline int MulDiv255(int value, int factor) {
    return (value * factor + 127) / 255;
}

static inline uint32_t PackPremultiplied(int r, int g, int b, int alpha) {
    const int a = std::clamp(alpha, 0, 255);
    return (static_cast<uint32_t>(a) << 24) |
           (static_cast<uint32_t>(MulDiv255(std::clamp(r, 0, 255), a)) << 16) |
           (static_cast<uint32_t>(MulDiv255(std::clamp(g, 0, 255), a)) << 8) |
           static_cast<uint32_t>(MulDiv255(std::clamp(b, 0, 255), a));
}

// Scala un pixel premoltiplicato, alfa compresa: serve per la copertura dei
// bordi arrotondati, dove un pixel è coperto solo in parte.
static inline uint32_t ScalePremultiplied(uint32_t pixel, int coverage) {
    if (coverage >= 16) {
        return pixel;
    }
    if (coverage <= 0) {
        return 0;
    }
    const uint32_t a = ((pixel >> 24) & 0xFF) * static_cast<uint32_t>(coverage) / 16;
    const uint32_t r = ((pixel >> 16) & 0xFF) * static_cast<uint32_t>(coverage) / 16;
    const uint32_t g = ((pixel >> 8) & 0xFF) * static_cast<uint32_t>(coverage) / 16;
    const uint32_t b = (pixel & 0xFF) * static_cast<uint32_t>(coverage) / 16;
    return (a << 24) | (r << 16) | (g << 8) | b;
}

// Source-over di un pixel premoltiplicato su un altro.
static inline uint32_t BlendPremultipliedPixel(uint32_t destination,
                                               uint32_t source) {
    const int sourceAlpha = static_cast<int>((source >> 24) & 0xFF);
    if (sourceAlpha == 0) {
        return destination;
    }
    if (sourceAlpha == 255) {
        return source;
    }
    const int inverse = 255 - sourceAlpha;
    const uint32_t a = static_cast<uint32_t>(sourceAlpha) +
        static_cast<uint32_t>(MulDiv255(static_cast<int>((destination >> 24) & 0xFF), inverse));
    const uint32_t r = ((source >> 16) & 0xFF) +
        static_cast<uint32_t>(MulDiv255(static_cast<int>((destination >> 16) & 0xFF), inverse));
    const uint32_t g = ((source >> 8) & 0xFF) +
        static_cast<uint32_t>(MulDiv255(static_cast<int>((destination >> 8) & 0xFF), inverse));
    const uint32_t b = (source & 0xFF) +
        static_cast<uint32_t>(MulDiv255(static_cast<int>(destination & 0xFF), inverse));
    return (std::min<uint32_t>(a, 255) << 24) |
           (std::min<uint32_t>(r, 255) << 16) |
           (std::min<uint32_t>(g, 255) << 8) |
           std::min<uint32_t>(b, 255);
}

static inline uint32_t GetSurfacePixel(const BitmapSurface& surface, int x,
                                       int y) {
    if (x < 0 || y < 0 || x >= surface.width || y >= surface.height ||
        !surface.bits) {
        return 0;
    }
    const auto* pixels = static_cast<const uint32_t*>(surface.bits);
    return pixels[static_cast<size_t>(y) * static_cast<size_t>(surface.width) + x];
}

static inline void SetSurfacePixel(BitmapSurface& surface, int x, int y,
                                   uint32_t premultiplied) {
    if (x < 0 || y < 0 || x >= surface.width || y >= surface.height ||
        !surface.bits) {
        return;
    }
    auto* pixels = static_cast<uint32_t*>(surface.bits);
    pixels[static_cast<size_t>(y) * static_cast<size_t>(surface.width) + x] =
        premultiplied;
}

static inline void BlendPixel(BitmapSurface& surface, int x, int y,
                              uint32_t premultiplied) {
    if (x < 0 || y < 0 || x >= surface.width || y >= surface.height) {
        return;
    }
    auto* pixels = static_cast<uint32_t*>(surface.bits);
    pixels[static_cast<size_t>(y) * static_cast<size_t>(surface.width) + x] =
        BlendPremultipliedPixel(
            pixels[static_cast<size_t>(y) * static_cast<size_t>(surface.width) + x],
            premultiplied);
}

// Un punto è dentro il rettangolo arrotondato? La verifica è esatta, quindi la
// copertura dei bordi si può ricavare da quattro campioni per lato.
static bool PointInRoundedRect(double x, double y, const RECT& rect,
                               double radius) {
    if (x < rect.left || x >= rect.right || y < rect.top || y >= rect.bottom) {
        return false;
    }
    if (radius <= 0.0) {
        return true;
    }

    double nearestX = x;
    double nearestY = y;
    if (x < rect.left + radius) {
        nearestX = rect.left + radius;
    } else if (x > rect.right - radius) {
        nearestX = rect.right - radius;
    }
    if (y < rect.top + radius) {
        nearestY = rect.top + radius;
    } else if (y > rect.bottom - radius) {
        nearestY = rect.bottom - radius;
    }

    const double dx = x - nearestX;
    const double dy = y - nearestY;
    return dx * dx + dy * dy <= radius * radius;
}

// Copertura in sedicesimi di un pixel: 16 significa pieno, 0 vuoto.
static int RoundedRectCoverage(int x, int y, const RECT& rect, int radius) {
    if (radius <= 0) {
        return (x >= rect.left && x < rect.right && y >= rect.top &&
                y < rect.bottom)
            ? 16
            : 0;
    }

    // I pixel lontani dagli angoli sono pieni o vuoti senza dover campionare.
    const bool cornerColumn =
        x < rect.left + radius || x >= rect.right - radius;
    const bool cornerRow = y < rect.top + radius || y >= rect.bottom - radius;
    if (!cornerColumn || !cornerRow) {
        return (x >= rect.left && x < rect.right && y >= rect.top &&
                y < rect.bottom)
            ? 16
            : 0;
    }

    int covered = 0;
    for (int subY = 0; subY < 4; ++subY) {
        const double sampleY = y + (subY + 0.5) / 4.0;
        for (int subX = 0; subX < 4; ++subX) {
            const double sampleX = x + (subX + 0.5) / 4.0;
            if (PointInRoundedRect(sampleX, sampleY, rect,
                                   static_cast<double>(radius))) {
                ++covered;
            }
        }
    }
    return covered;
}

static void ClearSurface(BitmapSurface& surface) {
    if (surface.bits && surface.width > 0 && surface.height > 0) {
        ZeroMemory(surface.bits, static_cast<size_t>(surface.width) *
                                     static_cast<size_t>(surface.height) * 4);
    }
}

// Un rettangolo pieno, con angoli arrotondati, in colore premoltiplicato.
static void FillRoundedRect(BitmapSurface& surface, const RECT& rect,
                            int radius, COLORREF color, int alpha) {
    if (alpha <= 0 || rect.right <= rect.left || rect.bottom <= rect.top) {
        return;
    }

    const uint32_t premultiplied =
        PackPremultiplied(GetRValue(color), GetGValue(color), GetBValue(color),
                          alpha);
    const int top = std::max<int>(0, rect.top);
    const int bottom = std::min<int>(surface.height, rect.bottom);
    const int left = std::max<int>(0, rect.left);
    const int right = std::min<int>(surface.width, rect.right);

    for (int y = top; y < bottom; ++y) {
        for (int x = left; x < right; ++x) {
            const int coverage = RoundedRectCoverage(x, y, rect, radius);
            if (coverage > 0) {
                BlendPixel(surface, x, y,
                           ScalePremultiplied(premultiplied, coverage));
            }
        }
    }
}

// Un rettangolo arrotondato con una sfumatura verticale fra due alfa: è il
// riflesso chiaro che il vetro Aero ha nella metà superiore.
static void FillRoundedRectGradient(BitmapSurface& surface, const RECT& rect,
                                    int radius, COLORREF topColor,
                                    int topAlpha, COLORREF bottomColor,
                                    int bottomAlpha) {
    if (rect.right <= rect.left || rect.bottom <= rect.top) {
        return;
    }

    const int height = std::max(1, static_cast<int>(rect.bottom - rect.top));
    const int top = std::max<int>(0, rect.top);
    const int bottom = std::min<int>(surface.height, rect.bottom);
    const int left = std::max<int>(0, rect.left);
    const int right = std::min<int>(surface.width, rect.right);

    for (int y = top; y < bottom; ++y) {
        const int position = std::clamp(
            static_cast<int>((y - rect.top) * 255 / height), 0, 255);
        const int alpha = (topAlpha * (255 - position) + bottomAlpha * position) / 255;
        if (alpha <= 0) {
            continue;
        }
        const int r = (GetRValue(topColor) * (255 - position) +
                       GetRValue(bottomColor) * position) / 255;
        const int g = (GetGValue(topColor) * (255 - position) +
                       GetGValue(bottomColor) * position) / 255;
        const int b = (GetBValue(topColor) * (255 - position) +
                       GetBValue(bottomColor) * position) / 255;
        const uint32_t premultiplied = PackPremultiplied(r, g, b, alpha);

        for (int x = left; x < right; ++x) {
            const int coverage = RoundedRectCoverage(x, y, rect, radius);
            if (coverage > 0) {
                BlendPixel(surface, x, y,
                           ScalePremultiplied(premultiplied, coverage));
            }
        }
    }
}

// Il contorno di un rettangolo arrotondato: la differenza fra la copertura del
// rettangolo esterno e quella del rettangolo rientrato di "thickness".
static void StrokeRoundedRect(BitmapSurface& surface, const RECT& rect,
                              int radius, int thickness, COLORREF color,
                              int alpha) {
    if (alpha <= 0 || thickness <= 0 || rect.right <= rect.left ||
        rect.bottom <= rect.top) {
        return;
    }

    RECT inner = rect;
    InflateRect(&inner, -thickness, -thickness);
    const int innerRadius = std::max(0, radius - thickness);
    const uint32_t premultiplied =
        PackPremultiplied(GetRValue(color), GetGValue(color), GetBValue(color),
                          alpha);

    const int top = std::max<int>(0, rect.top);
    const int bottom = std::min<int>(surface.height, rect.bottom);
    const int left = std::max<int>(0, rect.left);
    const int right = std::min<int>(surface.width, rect.right);

    for (int y = top; y < bottom; ++y) {
        for (int x = left; x < right; ++x) {
            const int outer = RoundedRectCoverage(x, y, rect, radius);
            if (outer == 0) {
                continue;
            }
            const int innerCoverage =
                inner.right > inner.left && inner.bottom > inner.top
                ? RoundedRectCoverage(x, y, inner, innerRadius)
                : 0;
            const int coverage =
                std::clamp(outer - innerCoverage, 0, 16);
            if (coverage > 0) {
                BlendPixel(surface, x, y,
                           ScalePremultiplied(premultiplied, coverage));
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Testo
//
// GDI riempie il DIB con il colore del testo, non con l'alfa, quindi il testo
// viene prima reso su una superficie nera con inchiostro bianco e antialiasing
// in scala di grigi: il valore dei canali è esattamente la copertura del glifo.
// Da lì ombra, contorno e testo si compongono con l'alfa che serve.
// -----------------------------------------------------------------------------

struct TextMask {
    BitmapSurface surface;
    int width = 0;
    int height = 0;

    void reset() {
        surface.reset();
        width = 0;
        height = 0;
    }
};

static ScopedFont g_uiFont;
static int g_uiFontHeight = 0;

// Il font del titolo. Niente ClearType: il pannello può stare sopra qualunque
// cosa, e il ClearType presuppone un colore di sfondo noto, quindi qui si usa
// l'antialiasing in scala di grigi, come chiede la specifica.
static HFONT GetUiFont(int heightPx) {
    const int height = std::max(8, heightPx);
    if (g_uiFont.value && g_uiFontHeight == height) {
        return g_uiFont.value;
    }
    g_uiFont.reset();

    LOGFONTW font{};
    font.lfHeight = -height;
    font.lfWeight = FW_NORMAL;
    font.lfCharSet = DEFAULT_CHARSET;
    font.lfOutPrecision = OUT_TT_PRECIS;
    font.lfQuality = ANTIALIASED_QUALITY;
    font.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    lstrcpynW(font.lfFaceName, L"Segoe UI", ARRAYSIZE(font.lfFaceName));

    g_uiFont.reset(CreateFontIndirectW(&font));
    g_uiFontHeight = height;
    return g_uiFont.value;
}

// Il testo viene disegnato a due volte la dimensione e poi ridotto: GDI
// antialiasa il glifo su una griglia di pixel, e a 20 px la scala di grigi che
// ne esce è più magra del testo vero. Disegnando a 2x e mediando i quattro
// campioni si ottiene la copertura che avrebbe un rendering a 2x ridotto, cioè
// un titolo della stessa consistenza dell'originale. La correzione di gamma
// compensa il fatto che la media avviene in sRGB e non in luce lineare.
constexpr int kTextMaskSupersample = 2;
constexpr double kTextMaskGamma = 0.80;

static unsigned char TextCoverage(double value) {
    if (value <= 0.0) {
        return 0;
    }
    if (value >= 1.0) {
        return 255;
    }
    // La riduzione avviene in sRGB, la copertura va riportata verso la luce
    // lineare: senza la correzione i glifi risultano più magri del vero.
    const double corrected = std::pow(value, kTextMaskGamma);
    return static_cast<unsigned char>(std::clamp(corrected, 0.0, 1.0) * 255.0 +
                                      0.5);
}

static bool BuildTextMask(TextMask& mask, const std::wstring& text,
                          int fontHeight, int maxWidth) {
    mask.reset();
    if (text.empty() || fontHeight <= 0 || maxWidth <= 0) {
        return false;
    }

    // Il font è quello del pannello, ma a due volte l'altezza: GDI non scala i
    // glifi, quindi il "2x" è un secondo font, non un ingrandimento.
    HFONT largeFont = GetUiFont(fontHeight * kTextMaskSupersample);
    if (!largeFont) {
        return false;
    }

    ScopedScreenDC screen(nullptr, GetDC(nullptr));
    if (!screen) {
        return false;
    }
    ScopedDC measuring(CreateCompatibleDC(screen.value));
    if (!measuring) {
        return false;
    }

    {
        ScopedSelectedObject fontSelection(measuring.value, largeFont);
        RECT bounds{0, 0, maxWidth * kTextMaskSupersample, 0};
        DrawTextW(measuring.value, text.c_str(), -1, &bounds,
                  DT_SINGLELINE | DT_CALCRECT | DT_NOPREFIX);
        const int largeWidth = std::clamp<int>(
            bounds.right - bounds.left, kTextMaskSupersample,
            maxWidth * kTextMaskSupersample);
        const int largeHeight = std::max<int>(kTextMaskSupersample,
                                             bounds.bottom - bounds.top);

        BitmapSurface large;
        if (!large.Ensure(largeWidth, largeHeight)) {
            return false;
        }
        ClearSurface(large);
        ScopedBkMode bkMode(large.dc.value, TRANSPARENT);
        ScopedTextColor textColor(large.dc.value, RGB(255, 255, 255));
        ScopedSelectedObject drawFont(large.dc.value, largeFont);
        RECT target{0, 0, largeWidth, largeHeight};
        DrawTextW(large.dc.value, text.c_str(), -1, &target,
                  DT_SINGLELINE | DT_LEFT | DT_TOP | DT_END_ELLIPSIS |
                      DT_NOPREFIX);
        GdiFlush();

        const int width = std::max(1, largeWidth / kTextMaskSupersample);
        const int height = std::max(1, largeHeight / kTextMaskSupersample);
        if (!mask.surface.Ensure(width, height)) {
            return false;
        }
        ClearSurface(mask.surface);

        const auto* source = static_cast<const uint8_t*>(large.bits);
        auto* destination = static_cast<uint8_t*>(mask.surface.bits);
        const int samples =
            kTextMaskSupersample * kTextMaskSupersample;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int sum = 0;
                for (int subY = 0; subY < kTextMaskSupersample; ++subY) {
                    const size_t row =
                        static_cast<size_t>(y * kTextMaskSupersample + subY) *
                        static_cast<size_t>(largeWidth);
                    for (int subX = 0; subX < kTextMaskSupersample; ++subX) {
                        sum += source[(row +
                                       static_cast<size_t>(x * kTextMaskSupersample +
                                                           subX)) * 4];
                    }
                }
                const uint8_t coverage = TextCoverage(
                    static_cast<double>(sum) / (samples * 255.0));
                uint8_t* out = destination +
                    (static_cast<size_t>(y) * static_cast<size_t>(width) +
                     static_cast<size_t>(x)) * 4;
                out[0] = coverage;
                out[1] = coverage;
                out[2] = coverage;
            }
        }

        mask.width = width;
        mask.height = height;
    }

    return true;
}

// Compone la maschera nel colore e nell'alfa richiesti, con uno scostamento,
// che è come nascono l'ombra e il contorno del titolo.
static void BlendTextMask(BitmapSurface& target, const TextMask& mask, int x,
                          int y, COLORREF color, int alpha) {
    if (!mask.surface.bits || alpha <= 0) {
        return;
    }

    const int baseR = GetRValue(color);
    const int baseG = GetGValue(color);
    const int baseB = GetBValue(color);
    // La copertura sta nel canale blu: l'inchiostro è bianco, quindi i tre
    // canali hanno lo stesso valore.
    const auto* source = static_cast<const uint8_t*>(mask.surface.bits);

    for (int row = 0; row < mask.height; ++row) {
        const uint8_t* sourceRow =
            source + static_cast<size_t>(row) * static_cast<size_t>(mask.width) * 4;
        for (int column = 0; column < mask.width; ++column) {
            const int coverage = sourceRow[column * 4];
            if (coverage == 0) {
                continue;
            }
            const int pixelAlpha = coverage * alpha / 255;
            if (pixelAlpha == 0) {
                continue;
            }
            BlendPixel(target, x + column, y + row,
                       PackPremultiplied(baseR, baseG, baseB, pixelAlpha));
        }
    }
}

// -----------------------------------------------------------------------------
// Stato della sessione
//
// Tutto quello che sta qui appartiene al thread dell'interfaccia della mod. Il
// thread del gancio della tastiera e il watchdog non toccano mai la sessione:
// mandano messaggi alla finestra del pannello, così lo stato resta a un solo
// thread e non ci sono gare.
// -----------------------------------------------------------------------------

constexpr wchar_t kOverlayClassName[] = L"WindhawkAltTabRecreationOverlay";

// Quanta parte del titolo viene disegnata come ombra e come contorno: sono i
// valori della specifica, 0x64 e 0x32.
constexpr int kTitleShadowAlpha = 0x64;
constexpr int kTitleOutlineAlpha = 0x32;
constexpr int kTitleShadowOffset = 2;

struct Session {
    bool active = false;
    bool visible = false;
    HWND hwnd = nullptr;          // la finestra del pannello, a livelli
    HWND previewLayer = nullptr;  // la finestra dei preview DWM, sopra il pannello
    HWND dimWindow = nullptr;     // lo strato che scurisce il desktop, se richiesto
    SwitcherSettings settings;
    std::wstring appPathFilter;
    std::vector<SwitcherItem> items;
    SwitcherLayout layout;
    int selectedIndex = -1;
    int hoverIndex = -1;
    int page = 0;
    int dpi = 96;
    RECT monitorRect{};   // il monitor su cui è mostrato, in coordinate schermo
    RECT workArea{};      // la sua area di lavoro, in coordinate monitor
    RECT panelScreen{};   // il pannello, in coordinate schermo
    BitmapSurface surface;  // il pannello composto, premoltiplicato
    COLORREF panelTint = RGB(226, 240, 250);  // il vetro del 7, dal colore di sistema
    COLORREF bandColor = RGB(36, 4, 110);     // la banda dell'8.1, dallo stesso colore
    int panelAlpha = kWin7FrostAlpha;
    int fadeState = 0;    // 0 nascosto, 1 in entrata, 2 visibile, 3 in uscita
    int fadeLevel = 0;    // alfa in virgola fissa 8.8: 0x0000 - 0xFF00
    bool pendingClose = false;  // la dissolvenza in uscita deve chiudere la sessione
    unsigned long long startedTick = 0;
};

static Session g_switcher;

// -----------------------------------------------------------------------------
// Vetro e sfocatura
//
// Il pannello di Windows 7 non è un rettangolo semitrasparente: è vetro, cioè
// il desktop sotto viene sfocato e poi colorato. La catena di ripiego è quella
// della specifica:
//   1. DwmEnableBlurBehindWindow con una regione arrotondata (API documentata);
//   2. SetWindowCompositionAttribute con ACCENT_ENABLE_BLURBEHIND (API non
//      documentata, ma è quella che funziona su Windows 10 e 11, dove la 1 non
//      produce più sfocatura);
//   3. solo trasparenza, senza sfocatura.
// Il livello raggiunto viene riportato nel log una volta per sessione.
// -----------------------------------------------------------------------------

enum class BlurLevel {
    None = 0,
    Dwm = 1,
    Accent = 2,
};

struct AccentPolicy {
    int state = 0;
    int flags = 0;
    int gradientColor = 0;
    int animationId = 0;
};

struct WindowCompositionAttributeData {
    DWORD attribute = 0;
    void* data = nullptr;
    SIZE_T sizeOfData = 0;
};

using SetWindowCompositionAttributeFn =
    BOOL(WINAPI*)(HWND, WindowCompositionAttributeData*);

constexpr DWORD kWcaAccentPolicy = 19;
constexpr int kAccentDisabled = 0;
constexpr int kAccentEnableBlurBehind = 3;
constexpr int kAccentFlagDrawAllBorders = 2;

static BlurLevel g_blurLevel = BlurLevel::None;
static HRGN g_blurRegion = nullptr;  // posseduta dalla finestra, cancellata a fine sessione

// Chiede la sfocatura dietro la finestra del pannello. La regione è in
// coordinate della finestra: la finestra è grande quanto il pannello, quindi la
// regione è semplicemente tutto il suo client, con gli angoli arrotondati.
//
// L'ordine della catena è quello della specifica, con una inversione motivata:
// da Windows 8 in poi DwmEnableBlurBehindWindow riesce ma non sfoca più niente,
// perché la sfocatura è passata all'attributo di composizione. Su Windows 10 e
// 11 la prova che conta è quindi SetWindowCompositionAttribute; la chiamata
// documentata resta come ripiego per i sistemi in cui l'attributo non esiste.
static SetWindowCompositionAttributeFn ResolveCompositionAttribute() {
    static HMODULE user32 = GetModuleHandleW(L"user32.dll");
    static SetWindowCompositionAttributeFn resolved =
        user32 ? reinterpret_cast<SetWindowCompositionAttributeFn>(
                     GetProcAddress(user32, "SetWindowCompositionAttribute"))
               : nullptr;
    return resolved;
}

static bool SetAccentPolicy(HWND hwnd, int state) {
    SetWindowCompositionAttributeFn setCompositionAttribute =
        ResolveCompositionAttribute();
    if (!setCompositionAttribute) {
        return false;
    }

    AccentPolicy policy{};
    policy.state = state;
    policy.flags = kAccentFlagDrawAllBorders;
    policy.gradientColor = 0;

    WindowCompositionAttributeData data{};
    data.attribute = kWcaAccentPolicy;
    data.data = &policy;
    data.sizeOfData = sizeof(policy);
    return setCompositionAttribute(hwnd, &data) != FALSE;
}

// mode: "auto" prova prima l'attributo di composizione e poi la chiamata
// documentata; "dwm" usa solo quella documentata; "off" non sfoca affatto.
static BlurLevel ApplyPanelBlur(HWND hwnd, int width, int height, int radius,
                                const std::wstring& mode) {
    if (!hwnd || !IsWindow(hwnd) || mode == L"off") {
        return BlurLevel::None;
    }

    if (g_blurRegion) {
        DeleteObject(g_blurRegion);
        g_blurRegion = nullptr;
    }

    g_blurRegion = CreateRoundRectRgn(0, 0, width + 1, height + 1,
                                      std::max(0, radius) * 2,
                                      std::max(0, radius) * 2);

    if (mode != L"dwm" && SetAccentPolicy(hwnd, kAccentEnableBlurBehind)) {
        return BlurLevel::Accent;
    }

    if (g_blurRegion) {
        DWM_BLURBEHIND blur{};
        blur.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION |
                       DWM_BB_TRANSITIONONMAXIMIZED;
        blur.fEnable = TRUE;
        blur.hRgnBlur = g_blurRegion;
        blur.fTransitionOnMaximized = FALSE;
        if (SUCCEEDED(DwmEnableBlurBehindWindow(hwnd, &blur))) {
            return BlurLevel::Dwm;
        }
    }

    return BlurLevel::None;
}

static void ClearPanelBlur(HWND hwnd) {
    if (hwnd && IsWindow(hwnd)) {
        DWM_BLURBEHIND blur{};
        blur.dwFlags = DWM_BB_ENABLE;
        blur.fEnable = FALSE;
        DwmEnableBlurBehindWindow(hwnd, &blur);

        // L'attributo di composizione va spento esplicitamente, altrimenti la
        // regione dietro la finestra resta sfocata anche a pannello nascosto.
        SetAccentPolicy(hwnd, kAccentDisabled);
    }

    if (g_blurRegion) {
        DeleteObject(g_blurRegion);
        g_blurRegion = nullptr;
    }
    g_blurLevel = BlurLevel::None;
}

// -----------------------------------------------------------------------------
// Lo strato che scurisce il desktop
//
// Con una finestra a livelli il pannello può essere semitrasparente davvero, e
// dietro di lui si vede il desktop vero, non uno screenshot: lo scurimento è
// quindi un secondo livello, grande quanto lo schermo, dipinto una volta sola
// con un nero uniforme. Con lo scurimento a 0 la finestra non esiste nemmeno.
// -----------------------------------------------------------------------------

constexpr wchar_t kDimClassName[] = L"WindhawkAltTabRecreationDim";

static bool EnsureDimWindow(const RECT& monitorScreen, int alpha) {
    HINSTANCE instance = GetModuleHandleW(nullptr);

    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = DefWindowProcW;
        windowClass.hInstance = instance;
        windowClass.lpszClassName = kDimClassName;
        if (!RegisterClassExW(&windowClass) &&
            GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
        classRegistered = true;
    }

    const int width = monitorScreen.right - monitorScreen.left;
    const int height = monitorScreen.bottom - monitorScreen.top;
    if (width <= 0 || height <= 0) {
        return false;
    }

    if (!g_switcher.dimWindow || !IsWindow(g_switcher.dimWindow)) {
        g_switcher.dimWindow = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                WS_EX_TRANSPARENT,
            kDimClassName, L"", WS_POPUP, monitorScreen.left, monitorScreen.top,
            width, height, nullptr, nullptr, instance, nullptr);
        if (!g_switcher.dimWindow) {
            return false;
        }
    }

    SetWindowPos(g_switcher.dimWindow, HWND_TOPMOST, monitorScreen.left,
                 monitorScreen.top, width, height,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
    SetLayeredWindowAttributes(g_switcher.dimWindow, 0,
                               static_cast<BYTE>(std::clamp(alpha, 0, 255)),
                               LWA_ALPHA);
    return true;
}

static void DestroyDimWindow() {
    if (g_switcher.dimWindow && IsWindow(g_switcher.dimWindow)) {
        DestroyWindow(g_switcher.dimWindow);
    }
    g_switcher.dimWindow = nullptr;
}

// -----------------------------------------------------------------------------
// L'ombra del pannello
//
// Il vetro Aero non finisce sul bordo: attorno c'è un'ombra morbida, ed è una
// buona parte di quello che lo fa sembrare vetro e non un rettangolo grigio. Una
// finestra a livelli non riceve l'ombra di DWM, quindi l'ombra si disegna, in una
// finestra a parte sotto il pannello e più grande di lui: così non entra nella
// regione della sfocatura e non viene ritagliata.
// -----------------------------------------------------------------------------

constexpr wchar_t kShadowClassName[] = L"WindhawkAltTabRecreationShadow";

// Distanza con segno dal bordo di un rettangolo arrotondato: negativa dentro,
// positiva fuori. È la formula standard della "rounded box".
static double RoundedRectDistance(double x, double y, const RECT& rect,
                                  double radius) {
    const double halfWidth = (rect.right - rect.left) / 2.0;
    const double halfHeight = (rect.bottom - rect.top) / 2.0;
    const double centerX = rect.left + halfWidth;
    const double centerY = rect.top + halfHeight;
    const double qx = std::abs(x - centerX) - (halfWidth - radius);
    const double qy = std::abs(y - centerY) - (halfHeight - radius);
    const double outsideX = std::max(qx, 0.0);
    const double outsideY = std::max(qy, 0.0);
    const double outside = std::sqrt(outsideX * outsideX + outsideY * outsideY);
    const double inside = std::min(std::max(qx, qy), 0.0);
    return outside + inside - radius;
}

// topPercent vale 100 quando l'ombra gira tutta attorno. L'ombra di una
// miniatura viva invece cade soprattutto in basso: sul fotogramma il vetro
// subito sopra l'anteprima resta chiaro (luminanza 214 contro i 215 del resto),
// mentre sotto scende a 80. Un anello scuro uguale sui quattro lati non
// assomiglia a un incavo, assomiglia a una macchia.
static void DrawSoftShadow(BitmapSurface& surface, const RECT& rect,
                           int radius, int spread, int maxAlpha,
                           int topPercent = 100) {
    if (spread <= 0 || maxAlpha <= 0 || rect.right <= rect.left ||
        rect.bottom <= rect.top) {
        return;
    }

    const int left = std::max<int>(0, rect.left - spread);
    const int top = std::max<int>(0, rect.top - spread);
    const int right = std::min<int>(surface.width, rect.right + spread);
    const int bottom = std::min<int>(surface.height, rect.bottom + spread);

    for (int y = top; y < bottom; ++y) {
        for (int x = left; x < right; ++x) {
            const double distance = RoundedRectDistance(
                x + 0.5, y + 0.5, rect, static_cast<double>(radius));
            // Solo fuori dal rettangolo: dentro la distanza \u00e8 negativa e senza
            // questo controllo l'ombra riempirebbe di nero tutto l'interno, cosa
            // che per l'ombra del pannello non si vedeva (il pannello la copre)
            // ma per quella dell'anteprima si vedrebbe eccome.
            if (distance <= 0.0 || distance >= spread) {
                continue;
            }
            // Caduta quadratica: piena sul bordo, nulla alla fine della spread.
            double falloff = 1.0 - std::max(distance, 0.0) / spread;
            falloff *= falloff;
            int alpha = static_cast<int>(maxAlpha * falloff + 0.5);
            if (topPercent < 100 && y + 0.5 < rect.top) {
                alpha = alpha * topPercent / 100;
            }
            if (alpha <= 0) {
                continue;
            }
            // L'ombra è nera, quindi il pixel premoltiplicato è solo alfa.
            BlendPixel(surface, x, y,
                       static_cast<uint32_t>(alpha) << 24);
        }
    }
}

struct ShadowWindow {
    HWND hwnd = nullptr;
    bool classRegistered = false;
    BitmapSurface surface;
};

static ShadowWindow g_shadowWindow;

static void DestroyShadowWindow() {
    if (g_shadowWindow.hwnd && IsWindow(g_shadowWindow.hwnd)) {
        DestroyWindow(g_shadowWindow.hwnd);
    }
    g_shadowWindow.hwnd = nullptr;
    g_shadowWindow.surface.reset();
}

// Costruisce l'ombra attorno al pannello. Il rettangolo passato è quello del
// pannello in coordinate schermo; la finestra è più grande di "spread" per lato.
static bool EnsureShadowWindow(const RECT& panel, int spread, int radius,
                               int maxAlpha) {
    HINSTANCE instance = GetModuleHandleW(nullptr);

    if (!g_shadowWindow.classRegistered) {
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = DefWindowProcW;
        windowClass.hInstance = instance;
        windowClass.lpszClassName = kShadowClassName;
        if (!RegisterClassExW(&windowClass) &&
            GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
        g_shadowWindow.classRegistered = true;
    }

    const RECT windowRect{panel.left - spread, panel.top - spread,
                          panel.right + spread, panel.bottom + spread};
    const int width = windowRect.right - windowRect.left;
    const int height = windowRect.bottom - windowRect.top;
    if (width <= 0 || height <= 0 ||
        !g_shadowWindow.surface.Ensure(width, height)) {
        return false;
    }

    ClearSurface(g_shadowWindow.surface);
    const RECT inner{spread, spread, spread + (panel.right - panel.left),
                     spread + (panel.bottom - panel.top)};
    DrawSoftShadow(g_shadowWindow.surface, inner, radius, spread, maxAlpha);

    if (!g_shadowWindow.hwnd || !IsWindow(g_shadowWindow.hwnd)) {
        g_shadowWindow.hwnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                WS_EX_TRANSPARENT,
            kShadowClassName, L"", WS_POPUP, windowRect.left, windowRect.top,
            width, height, nullptr, nullptr, instance, nullptr);
        if (!g_shadowWindow.hwnd) {
            return false;
        }
    }

    // HWND_TOPMOST, ma prima del pannello: la sessione posiziona il pannello
    // subito dopo, quindi l'ombra resta sotto.
    SetWindowPos(g_shadowWindow.hwnd, HWND_TOPMOST, windowRect.left,
                 windowRect.top, width, height,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);

    ScopedScreenDC screen(nullptr, GetDC(nullptr));
    if (!screen) {
        return false;
    }
    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    POINT destination{windowRect.left, windowRect.top};
    SIZE size{width, height};
    POINT source{0, 0};
    UpdateLayeredWindow(g_shadowWindow.hwnd, screen.value, &destination, &size,
                        g_shadowWindow.surface.dc.value, &source, 0, &blend,
                        ULW_ALPHA);
    return true;
}

// -----------------------------------------------------------------------------
// Lo strato dei preview
//
// DWM compone le anteprime sopra il contenuto della finestra di destinazione,
// quindi non possono stare nella finestra del pannello: se ci stessero,
// coprirebbero il vetro, i bordi e la selezione. Vivono perciò in una seconda
// finestra, sopra il pannello, ritagliata con una regione esattamente grande
// quanto i rettangoli delle anteprime: quello che sta fuori dalla regione non
// esiste, e sotto si vede il pannello.
//
// La finestra non è a livelli: una finestra a livelli con UpdateLayeredWindow
// non mostra le anteprime di DWM, mentre una finestra ritagliata da una regione
// le mostra e resta invisibile in tutto il resto.
// -----------------------------------------------------------------------------

constexpr wchar_t kPreviewLayerClassName[] =
    L"WindhawkAltTabRecreationPreviews";

static void DestroyPreviewLayer() {
    if (g_switcher.previewLayer && IsWindow(g_switcher.previewLayer)) {
        DestroyWindow(g_switcher.previewLayer);
    }
    g_switcher.previewLayer = nullptr;
}

static bool EnsurePreviewLayer() {
    if (g_switcher.previewLayer && IsWindow(g_switcher.previewLayer)) {
        return true;
    }

    HINSTANCE instance = GetModuleHandleW(nullptr);

    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = DefWindowProcW;
        windowClass.hInstance = instance;
        windowClass.lpszClassName = kPreviewLayerClassName;
        if (!RegisterClassExW(&windowClass) &&
            GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            static RateLimitedLog log{L"the preview layer class couldn't be registered", 1};
            if (log.ShouldLog()) {
                Wh_Log(L"The class of the preview layer couldn't be registered (error %lu)",
                       GetLastError());
            }
            return false;
        }
        classRegistered = true;
    }

    // Senza attivazione, fuori dall'elenco delle finestre, trasparente al mouse
    // e sempre sopra il pannello.
    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
            WS_EX_TRANSPARENT,
        kPreviewLayerClassName, L"", WS_POPUP, g_switcher.panelScreen.left,
        g_switcher.panelScreen.top,
        g_switcher.panelScreen.right - g_switcher.panelScreen.left,
        g_switcher.panelScreen.bottom - g_switcher.panelScreen.top,
        g_switcher.hwnd, nullptr, instance, nullptr);
    if (!hwnd) {
        static RateLimitedLog log{L"the preview layer couldn't be created", 1};
        if (log.ShouldLog()) {
            Wh_Log(L"The preview layer couldn't be created (error %lu)",
                   GetLastError());
        }
        return false;
    }

    g_switcher.previewLayer = hwnd;
    return true;
}

// Ritaglia la finestra dei preview sull'unione dei rettangoli delle anteprime
// registrate: quello che resta fuori non viene né disegnato né composto.
static void UpdatePreviewLayerRegion() {
    if (!g_switcher.previewLayer || !IsWindow(g_switcher.previewLayer)) {
        return;
    }

    HRGN region = CreateRectRgn(0, 0, 0, 0);
    if (!region) {
        return;
    }

    for (const SwitcherItem& item : g_switcher.items) {
        if (!item.hasThumbnail || item.thumb.right <= item.thumb.left ||
            item.thumb.bottom <= item.thumb.top) {
            continue;
        }
        RECT rect = item.thumb;
        OffsetRect(&rect, -g_switcher.layout.panel.left,
                   -g_switcher.layout.panel.top);
        HRGN piece = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
        if (piece) {
            CombineRgn(region, region, piece, RGN_OR);
            DeleteObject(piece);
        }
    }

    // La finestra va riportata sulla posizione del pannello a ogni sessione:
    // può essere stata riusata dopo che il pannello si è spostato, e va
    // mostrata, altrimenti il sistema non compone le anteprime.
    SetWindowPos(g_switcher.previewLayer, HWND_TOPMOST,
                 g_switcher.panelScreen.left, g_switcher.panelScreen.top,
                 g_switcher.panelScreen.right - g_switcher.panelScreen.left,
                 g_switcher.panelScreen.bottom - g_switcher.panelScreen.top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);

    SetWindowRgn(g_switcher.previewLayer, region, TRUE);
    // SetWindowRgn possiede la regione da questo momento: non va cancellata.
}

// -----------------------------------------------------------------------------
// L'animazione di comparsa
//
// Alfa in virgola fissa 8.8, da 0x4000 a 0xFF00 in 250 ms con un passo di 40 ms,
// come nella specifica. L'alfa viene applicato da UpdateLayeredWindow con
// SourceConstantAlpha, quindi non serve ricomporre il pannello a ogni passo.
// -----------------------------------------------------------------------------

constexpr int kFadeStartLevel = 0x4000;
constexpr int kFadeEndLevel = 0xFF00;
constexpr UINT kFadeTickMs = 40;
constexpr int kFadeStep = (kFadeEndLevel - kFadeStartLevel) / 6;  // 250 ms / 40 ms

static void PresentFrame();
// Le icone vivono in un altro blocco: la presentazione le rinfresca.
static void UpdateIconOverlay();

static int CurrentFadeAlpha() {
    if (!g_switcher.settings.openFade) {
        return 255;
    }
    return std::clamp(g_switcher.fadeLevel >> 8, 0, 255);
}

static void StopFadeTimer() {
    if (g_switcher.hwnd && IsWindow(g_switcher.hwnd)) {
        KillTimer(g_switcher.hwnd, kFadeTimerId);
    }
}

static void StartFade(bool entering) {
    StopFadeTimer();
    if (!g_switcher.settings.openFade) {
        g_switcher.fadeState = entering ? 2 : 0;
        g_switcher.fadeLevel = entering ? kFadeEndLevel : 0;
        PresentFrame();
        return;
    }

    g_switcher.fadeState = entering ? 1 : 3;
    g_switcher.fadeLevel = entering ? kFadeStartLevel : kFadeEndLevel;
    if (g_switcher.hwnd && IsWindow(g_switcher.hwnd)) {
        SetTimer(g_switcher.hwnd, kFadeTimerId, kFadeTickMs, nullptr);
    }
    PresentFrame();
}

// Un passo dell'animazione. Restituisce true quando la dissolvenza in uscita è
// finita e la sessione può essere chiusa davvero.
static bool StepFade() {
    if (g_switcher.fadeState == 1) {
        g_switcher.fadeLevel = std::min(kFadeEndLevel,
                                       g_switcher.fadeLevel + kFadeStep);
        PresentFrame();
        if (g_switcher.fadeLevel >= kFadeEndLevel) {
            g_switcher.fadeState = 2;
            StopFadeTimer();
        }
        return false;
    }

    if (g_switcher.fadeState == 3) {
        g_switcher.fadeLevel = std::max(0, g_switcher.fadeLevel - kFadeStep);
        PresentFrame();
        if (g_switcher.fadeLevel <= 0) {
            g_switcher.fadeState = 0;
            StopFadeTimer();
            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------
// Composizione del pannello
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Il colore di sistema
//
// Il vetro di Windows 7 e la banda di Windows 8.1 non avevano un colore loro:
// prendevano il colore di personalizzazione di Windows. Era il motivo per cui
// due PC con due colori diversi avevano due switcher diversi, e per cui lo
// switcher cambiava subito dopo aver toccato "Colore" nelle impostazioni.
//
// Qui il colore si legge da due fonti, in quest'ordine:
//   1. HKCU\Software\Microsoft\Windows\DWM\AccentColor, il colore scelto
//      dall'utente in Impostazioni > Personalizzazione > Colori. Su Windows 10 e
//      11 è l'unico posto in cui il colore c'è sempre, anche quando la
//      colorizzazione delle finestre è spenta;
//   2. DwmGetColorizationColor, l'API documentata che restituisce il colore
//      usato da DWM per comporre il vetro (0xAARRGGBB, con l'alfa che ne indica
//      l'intensità). È la fonte che usava Windows 7.
// Se nessuna delle due risponde restano i due colori misurati sui fotogrammi.
// -----------------------------------------------------------------------------

struct SystemAccent {
    COLORREF color = RGB(66, 156, 230);  // "Sky", il colore predefinito di Windows 7
    int intensity = 0xC4;                // canale alfa della colorizzazione
    bool valid = false;
};

// Legge un DWORD dal registro senza toccare percorsi di uscita: RegGetValue
// riporta l'esito e non lascia handle aperti.
static bool ReadAccentFromRegistry(DWORD* value) {
    if (!value) {
        return false;
    }
    DWORD data = 0;
    DWORD size = sizeof(data);
    const LSTATUS status =
        RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &data, &size);
    if (status != ERROR_SUCCESS) {
        return false;
    }
    *value = data;
    return true;
}

static SystemAccent ResolveSystemAccent() {
    SystemAccent accent;

    // AccentColor è in formato 0xAABBGGRR, cioè lo stesso ordine di COLORREF:
    // i tre canali si leggono al contrario rispetto ad ARGB.
    DWORD accentValue = 0;
    if (ReadAccentFromRegistry(&accentValue)) {
        accent.color = RGB(static_cast<int>(accentValue & 0xFF),
                           static_cast<int>((accentValue >> 8) & 0xFF),
                           static_cast<int>((accentValue >> 16) & 0xFF));
        accent.intensity = static_cast<int>((accentValue >> 24) & 0xFF);
        accent.valid = accent.intensity != 0;
    }

    DWORD colorization = 0;
    BOOL opaqueBlend = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaqueBlend))) {
        const COLORREF glass =
            RGB(static_cast<int>((colorization >> 16) & 0xFF),
                static_cast<int>((colorization >> 8) & 0xFF),
                static_cast<int>(colorization & 0xFF));
        if (!accent.valid) {
            accent.color = glass;
            accent.intensity = static_cast<int>((colorization >> 24) & 0xFF);
            accent.valid = true;
        }
    }

    return accent;
}

// Il vetro del 7: il colore di sistema alleggerito verso il bianco. Non è una
// tinta fissa, quindi un accento rosso dà un vetro rosato e uno blu un vetro
// azzurro, come faceva Aero.
// "#RRGGBB", "RRGGBB", "#RGB" o "RGB": il formato che si scrive a mano nelle
// impostazioni. Restituisce false se il testo non è un colore, così un refuso
// non spegne il pannello ma fa tornare al colore di sistema.
static bool ParseHexColor(PCWSTR text, COLORREF* color) {
    if (!text || !color) {
        return false;
    }
    const wchar_t* cursor = text;
    while (*cursor == L' ') {
        ++cursor;
    }
    if (*cursor == L'#') {
        ++cursor;
    }

    int digits[6] = {0, 0, 0, 0, 0, 0};
    int count = 0;
    while (*cursor) {
        const wchar_t character = *cursor;
        int value = -1;
        if (character >= L'0' && character <= L'9') {
            value = character - L'0';
        } else if (character >= L'a' && character <= L'f') {
            value = character - L'a' + 10;
        } else if (character >= L'A' && character <= L'F') {
            value = character - L'A' + 10;
        } else {
            break;
        }
        if (count >= 6) {
            return false;
        }
        digits[count++] = value;
        ++cursor;
    }

    int r = 0;
    int g = 0;
    int b = 0;
    if (count == 6) {
        r = digits[0] * 16 + digits[1];
        g = digits[2] * 16 + digits[3];
        b = digits[4] * 16 + digits[5];
    } else if (count == 3) {
        r = digits[0] * 17;
        g = digits[1] * 17;
        b = digits[2] * 17;
    } else {
        return false;
    }
    *color = RGB(r, g, b);
    return true;
}

// Il colore da usare, secondo l'impostazione: quello scelto a mano oppure quello
// di Windows. Una funzione sola perché la stessa scelta serve quando la sessione
// comincia e quando il sistema cambia colore a switcher aperto.
static SystemAccent ResolvePanelAccent(const SwitcherSettings& settings,
                                      bool* usedCustom) {
    SystemAccent accent;
    if (usedCustom) {
        *usedCustom = false;
    }

    COLORREF customColor = 0;
    if (settings.accentSource == L"custom") {
        if (ParseHexColor(settings.customAccentColor.c_str(), &customColor)) {
            accent.color = customColor;
            accent.valid = true;
            if (usedCustom) {
                *usedCustom = true;
            }
            return accent;
        }
        Wh_Log(L"The custom color \"%s\" isn't a color, the Windows one is used",
               settings.customAccentColor.c_str());
    }

    return ResolveSystemAccent();
}

static COLORREF FrostColorFromAccent(COLORREF accent) {
    const int percent = std::clamp(kFrostWhitePercent, 0, 100);
    auto mix = [](int channel) {
        return std::clamp(channel + (255 - channel) * percent / 100, 0, 255);
    };
    return RGB(mix(GetRValue(accent)), mix(GetGValue(accent)),
               mix(GetBValue(accent)));
}

// La banda dell'8.1: lo stesso colore di sistema, portato alla profondità
// misurata sul fotogramma. Si conservano tinta e saturazione e si fissa il
// canale più alto, così un giallo resta giallo e un indaco resta indaco.
static COLORREF BandColorFromAccent(COLORREF accent) {
    const int r = GetRValue(accent);
    const int g = GetGValue(accent);
    const int b = GetBValue(accent);
    const int highest = std::max(r, std::max(g, b));
    if (highest <= 8) {
        // Un accento nero o quasi non è un colore: resta il valore misurato.
        return kW81PanelColor;
    }
    auto scale = [highest](int channel) {
        return std::clamp(channel * kW81BandValue / highest, 0, 255);
    };
    return RGB(scale(r), scale(g), scale(b));
}

// -----------------------------------------------------------------------------
// La texture del vetro, generata
//
// Un pannello traslucido senza texture si legge come un rettangolo di colore
// piatto, ed è esattamente il difetto che si vedeva. Il vetro di Aero aveva due
// cose che qui vengono ricostruite senza leggere niente dal sistema:
//
//  * la grana dello smerigliato, un rumore finissimo di pochi livelli che toglie
//    al colore l'aspetto di tinta unita;
//  * i riflessi orizzontali, uno forte subito sotto il bordo alto e uno più
//    debole a un quinto dell'altezza.
//
// Il rumore non è casuale: è una funzione deterministica delle coordinate, quindi
// è identico a ogni fotogramma e il pannello non "sfarfalla" quando viene
// ridisegnato (per esempio quando cambia la selezione).
// -----------------------------------------------------------------------------

static unsigned int GlassNoise(int x, int y) {
    unsigned int value = static_cast<unsigned int>(x) * 374761393u +
                         static_cast<unsigned int>(y) * 668265263u;
    value = (value ^ (value >> 13)) * 1274126177u;
    return value ^ (value >> 16);
}

// Il colore premoltiplicato cambia insieme all'alfa: aggiungere livelli al
// colore di un pixel mezzo trasparente, senza riscalarli, lo renderebbe più
// chiaro dell'alfa che dichiara.
static void ApplyFrostGrain(BitmapSurface& surface, const RECT& rect,
                            int radius, int amplitude) {
    if (amplitude <= 0) {
        return;
    }
    for (int y = rect.top; y < rect.bottom; ++y) {
        for (int x = rect.left; x < rect.right; ++x) {
            if (RoundedRectCoverage(x, y, rect, radius) <= 0) {
                continue;
            }
            const uint32_t pixel = GetSurfacePixel(surface, x, y);
            const int alpha = static_cast<int>((pixel >> 24) & 0xFF);
            if (alpha == 0) {
                continue;
            }
            const int span = amplitude * 2 + 1;
            const int delta =
                static_cast<int>(GlassNoise(x, y) % static_cast<unsigned>(span)) -
                amplitude;
            const int scaled = delta * alpha / 255;
            if (scaled == 0) {
                continue;
            }
            auto adjust = [scaled, alpha](int channel) {
                return std::clamp(channel + scaled, 0, alpha);
            };
            const uint32_t updated =
                (static_cast<uint32_t>(alpha) << 24) |
                (static_cast<uint32_t>(adjust(static_cast<int>((pixel >> 16) & 0xFF)))
                 << 16) |
                (static_cast<uint32_t>(adjust(static_cast<int>((pixel >> 8) & 0xFF)))
                 << 8) |
                static_cast<uint32_t>(adjust(static_cast<int>(pixel & 0xFF)));
            SetSurfacePixel(surface, x, y, updated);
        }
    }
}

// -----------------------------------------------------------------------------
// Il vetro vero
//
// Il pannello non può contare sulla sfocatura di DWM: su Windows 10 e 11
// DwmEnableBlurBehindWindow su una finestra a livelli il più delle volte non fa
// nulla, e quello che resta è una tinta piatta sopra il desktop, che è
// esattamente il difetto da correggere.
//
// Qui il vetro viene costruito come lo costruiva Aero: si prende il desktop che
// sta dietro al pannello, lo si sfoca, lo si colora con il colore della
// colorizzazione e ci si compone sopra la tinta. Il risultato non dipende da
// quello che DWM decide di fare, e resta identico su ogni macchina.
//
// La cattura avviene una volta per sessione, mentre il pannello è ancora
// nascosto: così non si fotografa mai se stesso. Il campionamento è a metà
// risoluzione, perché dopo la sfocatura i dettagli fini non esistono più e la
// cattura costa un quarto.
// -----------------------------------------------------------------------------

// Raggio del box blur sul fondale a metà risoluzione: due passate da 3 valgono
// una gaussiana di sigma ~5, cioè circa 10 px pieni di sfocatura.
constexpr int kGlassBlurRadius = 3;

struct GlassBackdrop {
    std::vector<uint32_t> pixels;  // 0x00BBGGRR, a metà risoluzione
    int width = 0;
    int height = 0;
    RECT screen{};                 // zona di schermo copiata
    int panelWidth = 0;            // dimensioni per cui è stata preparata
    int panelHeight = 0;
    int style = -1;

    bool Matches(const RECT& rect, int w, int h, int styleId) const {
        return !pixels.empty() && screen.left == rect.left &&
               screen.top == rect.top && screen.right == rect.right &&
               screen.bottom == rect.bottom && panelWidth == w &&
               panelHeight == h && style == styleId;
    }

    void reset() {
        pixels.clear();
        std::vector<uint32_t>().swap(pixels);
        width = 0;
        height = 0;
        *this = GlassBackdrop();
    }
};

static GlassBackdrop g_glass;

static void ReleaseGlassBackdrop() {
    g_glass = GlassBackdrop();
}

// Sfocatura a scatola separabile, applicata due volte: due passate di raggio r
// danno una gaussiana di sigma ~ r * sqrt(2/3), più che sufficiente per il vetro.
static void BoxBlurPixels(std::vector<uint32_t>& pixels, int width, int height,
                          int radius) {
    if (radius <= 0 || width <= 0 || height <= 0) {
        return;
    }

    std::vector<uint32_t> temporary(pixels.size(), 0);
    const int window = radius * 2 + 1;

    // Passata orizzontale.
    for (int y = 0; y < height; ++y) {
        const uint32_t* row = pixels.data() + static_cast<size_t>(y) * width;
        uint32_t* out = temporary.data() + static_cast<size_t>(y) * width;
        int sumB = 0;
        int sumG = 0;
        int sumR = 0;
        for (int x = -radius; x < width + radius; ++x) {
            if (x + radius < width) {
                const uint32_t added = row[std::clamp(x + radius, 0, width - 1)];
                sumB += static_cast<int>(added & 0xFF);
                sumG += static_cast<int>((added >> 8) & 0xFF);
                sumR += static_cast<int>((added >> 16) & 0xFF);
            }
            if (x - radius - 1 >= 0) {
                const uint32_t removed = row[x - radius - 1];
                sumB -= static_cast<int>(removed & 0xFF);
                sumG -= static_cast<int>((removed >> 8) & 0xFF);
                sumR -= static_cast<int>((removed >> 16) & 0xFF);
            }
            if (x >= 0 && x < width) {
                out[x] = (static_cast<uint32_t>(sumR / window) << 16) |
                         (static_cast<uint32_t>(sumG / window) << 8) |
                         static_cast<uint32_t>(sumB / window);
            }
        }
    }

    // Passata verticale.
    for (int x = 0; x < width; ++x) {
        int sumB = 0;
        int sumG = 0;
        int sumR = 0;
        for (int y = -radius; y < height + radius; ++y) {
            if (y + radius < height) {
                const uint32_t added =
                    temporary[static_cast<size_t>(std::clamp(y + radius, 0,
                                                             height - 1)) *
                                  width + x];
                sumB += static_cast<int>(added & 0xFF);
                sumG += static_cast<int>((added >> 8) & 0xFF);
                sumR += static_cast<int>((added >> 16) & 0xFF);
            }
            if (y - radius - 1 >= 0) {
                const uint32_t removed =
                    temporary[static_cast<size_t>(y - radius - 1) * width + x];
                sumB -= static_cast<int>(removed & 0xFF);
                sumG -= static_cast<int>((removed >> 8) & 0xFF);
                sumR -= static_cast<int>((removed >> 16) & 0xFF);
            }
            if (y >= 0 && y < height) {
                pixels[static_cast<size_t>(y) * width + x] =
                    (static_cast<uint32_t>(sumR / window) << 16) |
                    (static_cast<uint32_t>(sumG / window) << 8) |
                    static_cast<uint32_t>(sumB / window);
            }
        }
    }
}

// Copia la zona di schermo del pannello a metà risoluzione e la sfoca. Un
// fallimento non è un errore: senza vetro si ripiega sulla tinta, come prima.
static bool CaptureGlassBackdrop(const RECT& panelScreen, int panelWidth,
                                 int panelHeight, int styleId) {
    if (g_glass.Matches(panelScreen, panelWidth, panelHeight, styleId)) {
        return true;
    }
    g_glass = GlassBackdrop();

    const int width = std::max(2, panelWidth / 2);
    const int height = std::max(2, panelHeight / 2);

    ScopedScreenDC screen(nullptr, GetDC(nullptr));
    if (!screen) {
        return false;
    }
    ScopedDC memory(CreateCompatibleDC(screen.value));
    if (!memory) {
        return false;
    }

    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;  // righe dall'alto
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(screen.value, &info, DIB_RGB_COLORS, &bits,
                                      nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        return false;
    }
    HGDIOBJ previous = SelectObject(memory.value, bitmap);

    // HALFTONE rende la riduzione una media dei pixel, non un campione: è
    // quello che serve prima della sfocatura.
    const int previousMode = SetStretchBltMode(memory.value, HALFTONE);
    SetBrushOrgEx(memory.value, 0, 0, nullptr);
    const BOOL copied = StretchBlt(
        memory.value, 0, 0, width, height, screen.value, panelScreen.left,
        panelScreen.top, panelWidth, panelHeight, SRCCOPY);
    SetStretchBltMode(memory.value, previousMode);

    if (copied) {
        std::vector<uint32_t> pixels(static_cast<size_t>(width) * height, 0);
        const auto* source = static_cast<const uint32_t*>(bits);
        for (size_t i = 0; i < pixels.size(); ++i) {
            // Il DIB è 0x00RRGGBB; qui interessa tenere i canali separati e
            // nell'ordine in cui li legge il resto del disegno.
            pixels[i] = source[i] & 0x00FFFFFFu;
        }
        BoxBlurPixels(pixels, width, height, kGlassBlurRadius);
        BoxBlurPixels(pixels, width, height, kGlassBlurRadius);

        Wh_Log(L"Glass backdrop: %dx%d captured, blur radius %d", width, height,
               kGlassBlurRadius);
        g_glass.pixels = std::move(pixels);
        g_glass.width = width;
        g_glass.height = height;
        g_glass.screen = panelScreen;
        g_glass.panelWidth = panelWidth;
        g_glass.panelHeight = panelHeight;
        g_glass.style = styleId;
    }

    SelectObject(memory.value, previous);
    DeleteObject(bitmap);
    if (g_glass.pixels.empty()) {
        // Il fondale non è disponibile: resta la texture generata, che è il
        // motivo per cui il pannello non si legge comunque come tinta unita.
        Wh_Log(L"Glass backdrop: the capture failed, the generated frost texture is used");
    }
    return !g_glass.pixels.empty();
}

// Campione bilineare del fondale sfocato, in coordinate del pannello.
static uint32_t SampleGlass(int x, int y, int panelWidth, int panelHeight) {
    if (g_glass.pixels.empty() || g_glass.width < 2 || g_glass.height < 2) {
        return 0;
    }

    const double fx = (static_cast<double>(x) + 0.5) * g_glass.width /
                      std::max(1, panelWidth) - 0.5;
    const double fy = (static_cast<double>(y) + 0.5) * g_glass.height /
                      std::max(1, panelHeight) - 0.5;

    const int x0 = std::clamp(static_cast<int>(std::floor(fx)), 0,
                              g_glass.width - 1);
    const int y0 = std::clamp(static_cast<int>(std::floor(fy)), 0,
                              g_glass.height - 1);
    const int x1 = std::min(x0 + 1, g_glass.width - 1);
    const int y1 = std::min(y0 + 1, g_glass.height - 1);
    const int wx = std::clamp(static_cast<int>((fx - std::floor(fx)) * 256), 0,
                              256);
    const int wy = std::clamp(static_cast<int>((fy - std::floor(fy)) * 256), 0,
                              256);

    const uint32_t p00 = g_glass.pixels[static_cast<size_t>(y0) * g_glass.width + x0];
    const uint32_t p10 = g_glass.pixels[static_cast<size_t>(y0) * g_glass.width + x1];
    const uint32_t p01 = g_glass.pixels[static_cast<size_t>(y1) * g_glass.width + x0];
    const uint32_t p11 = g_glass.pixels[static_cast<size_t>(y1) * g_glass.width + x1];

    uint32_t result = 0;
    for (int channel = 0; channel < 3; ++channel) {
        const int shift = channel * 8;
        const int top = (((p00 >> shift) & 0xFF) * (256 - wx) +
                         ((p10 >> shift) & 0xFF) * wx) >> 8;
        const int bottom = (((p01 >> shift) & 0xFF) * (256 - wx) +
                            ((p11 >> shift) & 0xFF) * wx) >> 8;
        const int value = (top * (256 - wy) + bottom * wy) >> 8;
        result |= static_cast<uint32_t>(std::clamp(value, 0, 255)) << shift;
    }
    return result;
}

// La scheda del desktop disegna lo sfondo quando DWM non concede la miniatura.
// L'implementazione sta nel blocco delle icone, più sotto: qui basta la
// dichiarazione.
static void DrawDesktopCardBackground(BitmapSurface& surface, const RECT& rect);

static void DrawPanelBackground(BitmapSurface& surface, const SwitcherLayout& layout) {
    const RECT panel{0, 0, surface.width, surface.height};
    const int radius = layout.radius;

    if (layout.style == SwitcherStyle::Windows81) {
        // La banda dell'8.1 è opaca: nessuna sfocatura, nessuna trasparenza.
        FillRoundedRect(surface, panel, radius, g_switcher.bandColor, 255);
        return;
    }

    // Vetro Aero, nell'ordine in cui l'occhio lo legge sul fotogramma
    // dell'originale:
    //  1. la tinta della colorizzazione, quasi uniforme (la trasparenza vera la
    //     dà la sfocatura dietro, non un alfa alto);
    //  2. un riflesso chiaro molto tenue nella parte alta;
    //  3. una leggera ombra interna in basso;
    //  4. due pixel scuri sul bordo esterno;
    //  5. due pixel quasi bianchi subito dentro i precedenti.
    const int alphaTop = std::clamp(g_switcher.panelAlpha + 6, 0, 255);
    const int alphaBottom = std::clamp(g_switcher.panelAlpha - 8, 0, 255);
    const int baseR = GetRValue(g_switcher.panelTint);
    const int baseG = GetGValue(g_switcher.panelTint);
    const int baseB = GetBValue(g_switcher.panelTint);

    // Il fondale sfocato viene colorato con la tinta della colorizzazione: Aero
    // non metteva un velo sopra il desktop, ne cambiava il colore. Se la
    // cattura non c'è, resta la tinta piena.
    const int panelWidth = panel.right - panel.left;
    const int panelHeight = panel.bottom - panel.top;
    // Il fondale vale solo se è stato preso per questo pannello, nella stessa
    // posizione: dopo un cambio di layout o di monitor sarebbe sfalsato, e
    // sfalsato si vedrebbe subito (il vetro mostrerebbe il desktop sbagliato).
    const bool useGlass =
        !g_glass.pixels.empty() && g_glass.panelWidth == panelWidth &&
        g_glass.panelHeight == panelHeight &&
        g_glass.screen.left == g_switcher.panelScreen.left &&
        g_glass.screen.top == g_switcher.panelScreen.top;

    for (int row = 0; row < panelHeight; ++row) {
        const int y = panel.top + row;
        const int alpha = panelHeight <= 1
            ? alphaBottom
            : alphaTop + (alphaBottom - alphaTop) * row / (panelHeight - 1);
        for (int column = 0; column < panelWidth; ++column) {
            const int x = panel.left + column;
            const int coverage = RoundedRectCoverage(x, y, panel, radius);
            if (coverage <= 0) {
                continue;
            }

            int r = baseR;
            int g = baseG;
            int b = baseB;
            if (useGlass) {
                const uint32_t sample =
                    SampleGlass(column, row, panelWidth, panelHeight);
                const int glassB = static_cast<int>(sample & 0xFF);
                const int glassG = static_cast<int>((sample >> 8) & 0xFF);
                const int glassR = static_cast<int>((sample >> 16) & 0xFF);
                // kFrostColorMix: quanto la tinta copre il fondale. Troppo
                // alta e il vetro torna piatto, troppo bassa e si legge il
                // desktop attraverso le anteprime.
                const int tintMix = kFrostColorMix;
                const int blurMix = 100 - tintMix;
                r = (glassR * blurMix + baseR * tintMix) / 100;
                g = (glassG * blurMix + baseG * tintMix) / 100;
                b = (glassB * blurMix + baseB * tintMix) / 100;
                // La sfocatura di Windows schiarisce leggermente il vetro:
                // senza questa riga il pannello risulta più cupo dell'originale.
                r = std::clamp(r + kFrostLift, 0, 255);
                g = std::clamp(g + kFrostLift, 0, 255);
                b = std::clamp(b + kFrostLift, 0, 255);
            }

            // RoundedRectCoverage conta i campioni coperti su sedici, non su
            // 255: la divisione è per 16, altrimenti il pannello uscirebbe
            // quasi trasparente.
            const int pixelAlpha = std::clamp(alpha * coverage / 16, 0, 255);
            if (pixelAlpha <= 0) {
                continue;
            }
            BlendPixel(surface, x, y,
                       PackPremultiplied(r, g, b, pixelAlpha));
        }
    }

    RECT sheen = panel;
    sheen.bottom = panel.top + (panel.bottom - panel.top) *
                                  kWin7SheenPercent / 100;
    if (sheen.bottom > sheen.top) {
        FillRoundedRectGradient(surface, sheen, radius, RGB(255, 255, 255),
                                kWin7SheenAlpha, RGB(255, 255, 255), 0);
    }

    const int shadeTop = panel.bottom - (panel.bottom - panel.top) * 25 / 100;
    if (shadeTop < panel.bottom) {
        RECT shade{panel.left, shadeTop, panel.right, panel.bottom};
        FillRoundedRectGradient(surface, shade, radius, RGB(0, 0, 0), 0,
                                RGB(0, 0, 0), kWin7BottomShade);
    }

    // I due riflessi orizzontali del vetro: quello forte occupa il primo ottavo
    // dell'altezza, quello debole il resto fino a un quinto. Sono generati qui,
    // non letti da nessuna parte, quindi il pannello resta identico su ogni
    // macchina.
    const int panelHeightForGloss = panel.bottom - panel.top;
    RECT glossTop = panel;
    glossTop.bottom = panel.top + std::max(2, panelHeightForGloss / 8);
    FillRoundedRectGradient(surface, glossTop, radius, RGB(255, 255, 255),
                            kGlassGlossTopAlpha, RGB(255, 255, 255), 0);

    RECT glossMid = panel;
    glossMid.top = glossTop.bottom;
    glossMid.bottom = panel.top + std::max(
        static_cast<int>(glossTop.bottom) - static_cast<int>(panel.top) + 2,
        panelHeightForGloss / 5);
    FillRoundedRectGradient(surface, glossMid, radius, RGB(255, 255, 255),
                            kGlassGlossMidAlpha, RGB(255, 255, 255), 0);

    // La grana va per ultima, sopra tutto: è quella che toglie al pannello
    // l'aspetto di tinta piatta.
    ApplyFrostGrain(surface, panel, radius, kGlassGrainAlpha);

    const int borderDark = std::max(1, ScaleForDpi(kWin7BorderDark,
                                                   g_switcher.dpi));
    const int borderLight = std::max(1, ScaleForDpi(kWin7BorderLight,
                                                    g_switcher.dpi));

    StrokeRoundedRect(surface, panel, radius, borderDark, RGB(0, 0, 0),
                      kWin7BorderAlpha);

    RECT light = panel;
    InflateRect(&light, -borderDark, -borderDark);
    StrokeRoundedRect(surface, light, std::max(0, radius - borderDark),
                      borderLight, RGB(255, 255, 255), kWin7InnerAlpha);
}

// I fili e i riquadri attorno alle anteprime. L'anteprima vera è disegnata da
// DWM nella finestra dei preview, che sta sopra: qui si disegna solo quello che
// deve restare visibile attorno ad essa.
// Il riquadro dell'anteprima di una voce che DWM non sa mostrare, quando la
// voce è il desktop: al posto del rettangolo vuoto ci va lo sfondo, ritagliato
// come lo ritaglierebbe la miniatura.
static void DrawDesktopCards(BitmapSurface& surface, const SwitcherLayout& layout) {
    const POINT origin{layout.panel.left, layout.panel.top};

    for (size_t index = 0; index < layout.items.size(); ++index) {
        const int itemIndex = layout.firstIndex + static_cast<int>(index);
        if (itemIndex < 0 ||
            itemIndex >= static_cast<int>(g_switcher.items.size())) {
            continue;
        }

        const SwitcherItem& item = g_switcher.items[itemIndex];
        if (!item.isDesktop || item.hasThumbnail) {
            continue;
        }

        RECT rect = layout.items[index].thumb;
        OffsetRect(&rect, -origin.x, -origin.y);
        DrawDesktopCardBackground(surface, rect);
    }
}

static void DrawItemDecorations(BitmapSurface& surface, const SwitcherLayout& layout) {
    const POINT origin{layout.panel.left, layout.panel.top};

    for (size_t index = 0; index < layout.items.size(); ++index) {
        const int itemIndex = layout.firstIndex + static_cast<int>(index);
        if (itemIndex < 0 ||
            itemIndex >= static_cast<int>(g_switcher.items.size())) {
            continue;
        }

        const bool selected = itemIndex == g_switcher.selectedIndex;
        RECT thumb = layout.items[index].thumb;
        OffsetRect(&thumb, -origin.x, -origin.y);

        if (layout.style == SwitcherStyle::Windows81) {
            if (!selected) {
                // Nessun bordo sulle anteprime non selezionate, come
                // nell'originale.
                continue;
            }
            RECT frame = thumb;
            InflateRect(&frame, layout.framePad, layout.framePad);
            FillRoundedRect(surface, frame, 0, RGB(255, 255, 255),
                            kW81FrameFillAlpha);
            StrokeRoundedRect(surface, frame, 0, layout.contour,
                              kW81FrameColor, kW81FrameAlpha);
            continue;
        }

        // Windows 7: nessun bordo attorno alle anteprime. Attorno a ognuna però
        // resta l'ombra della miniatura viva, che il fotogramma mostra anche
        // attorno alle anteprime non selezionate: uscendo da una di quelle la
        // luminanza scende da 171 a 112.
        // L'ordine conta: prima la schiarita del vetro dentro il riquadro, poi
        // l'ombra dell'anteprima (che si ferma un pixel prima del filo, se no lo
        // sporca), infine i due fili.
        RECT frame = thumb;
        InflateRect(&frame, layout.selectionGap, layout.selectionGap);
        if (selected) {
            FillRoundedRect(surface, frame, layout.selectionRadius,
                            RGB(255, 255, 255), kWin7WellAlpha);
        }

        const int shadowSpread = std::max(1, layout.selectionGap - 1);
        if (g_switcher.items[itemIndex].hasThumbnail) {
            DrawSoftShadow(surface, thumb, kWin7ThumbShadowRadius, shadowSpread,
                           kWin7ThumbShadowAlpha, kWin7ThumbShadowTopPercent);
        }
        if (!selected) {
            continue;
        }

        const int thickness = std::max(1, layout.contour);
        StrokeRoundedRect(surface, frame, layout.selectionRadius, thickness,
                          kWin7SelectDarkColor, kWin7SelectDarkAlpha);

        RECT innerFrame = frame;
        InflateRect(&innerFrame, -thickness, -thickness);
        StrokeRoundedRect(surface, innerFrame,
                          std::max(0, layout.selectionRadius - thickness),
                          thickness, kWin7SelectLightColor,
                          kWin7SelectLightAlpha);
    }
}

// Una sola riga di titolo, centrata, con il nome della finestra selezionata:
// scura sul vetro di Windows 7, chiara sulla banda di Windows 8.1. Prima
// l'ombra, poi il contorno di un pixel sui quattro lati, poi il testo.
static void DrawPanelTitle(BitmapSurface& surface, const SwitcherLayout& layout) {
    const std::wstring* title = nullptr;
    if (g_switcher.selectedIndex >= 0 &&
        g_switcher.selectedIndex < static_cast<int>(g_switcher.items.size())) {
        title = &g_switcher.items[g_switcher.selectedIndex].title;
    }
    if (!title || title->empty()) {
        return;
    }

    const int fontHeight = layout.titleFontSize > 0
        ? layout.titleFontSize
        : ScaleForDpi(20, g_switcher.dpi);

    TextMask mask;
    if (!BuildTextMask(mask, *title, fontHeight,
                       std::max(1, surface.width - 24))) {
        return;
    }

    // Il carattere è posizionato sull'inchiostro, non sulla cella del font: la
    // cella di Segoe UI lascia sopra il maiuscolo uno spazio che cambia con la
    // dimensione, e allineando la cella il titolo finiva sempre troppo in alto
    // o troppo in basso. Qui si misura il primo pixel scritto e lo si porta
    // esattamente dove sta nell'originale.
    const auto* maskBits = static_cast<const uint8_t*>(mask.surface.bits);
    int inkLeft = mask.width;
    int inkRight = -1;
    int inkTop = mask.height;
    int inkBottom = -1;
    for (int row = 0; row < mask.height; ++row) {
        const uint8_t* maskRow =
            maskBits + static_cast<size_t>(row) * static_cast<size_t>(mask.width) * 4;
        for (int column = 0; column < mask.width; ++column) {
            if (maskRow[column * 4] < 8) {
                continue;
            }
            inkLeft = std::min(inkLeft, column);
            inkRight = std::max(inkRight, column);
            inkTop = std::min(inkTop, row);
            inkBottom = std::max(inkBottom, row);
        }
    }

    int x = (surface.width - mask.width) / 2;
    int y = std::max(0, layout.titleOffset);
    if (inkRight >= inkLeft && inkBottom >= inkTop) {
        // Il centro ottico dell'inchiostro, non quello della cella.
        x -= (inkLeft + inkRight + 1) / 2 - mask.width / 2;
    }
    if (inkBottom >= inkTop && layout.titleInkTop >= 0) {
        // La posizione verticale è quella dell'inchiostro, misurata dal bordo
        // alto del pannello: y è un valore assoluto, non uno scostamento da
        // titleOffset. Sommare le due cose (come faceva la prima versione)
        // spostava il titolo di venti pixel verso il basso.
        y = std::max(0, layout.titleInkTop - inkTop);
    }

    if (layout.style == SwitcherStyle::Windows81) {
        BlendTextMask(surface, mask, x + kTitleShadowOffset,
                      y + kTitleShadowOffset, RGB(0, 0, 0),
                      kTitleShadowAlpha);
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) {
                    continue;
                }
                BlendTextMask(surface, mask, x + dx, y + dy, RGB(0, 0, 0),
                              kTitleOutlineAlpha);
            }
        }
        BlendTextMask(surface, mask, x, y, kW81TitleColor, 255);
        return;
    }

    // Sul vetro il testo è scuro: un'ombra nera lo renderebbe illeggibile,
    // quindi l'ombra è chiara e debole e il contorno è bianco.
    BlendTextMask(surface, mask, x + 1, y + 1, RGB(255, 255, 255), 90);
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) {
                continue;
            }
            BlendTextMask(surface, mask, x + dx, y + dy, RGB(255, 255, 255),
                          60);
        }
    }
    BlendTextMask(surface, mask, x, y, kWin7TitleColor, 255);
}

static void PaintFrameBody() {
    if (!g_switcher.surface || g_switcher.layout.panel.right <=
                                 g_switcher.layout.panel.left) {
        return;
    }

    ClearSurface(g_switcher.surface);
    DrawPanelBackground(g_switcher.surface, g_switcher.layout);

    // Le schede senza miniatura: il desktop mostra lo sfondo, le finestre
    // mostrano l'icona grande (che disegna lo strato delle icone, sopra).
    DrawDesktopCards(g_switcher.surface, g_switcher.layout);
    DrawItemDecorations(g_switcher.surface, g_switcher.layout);
    DrawPanelTitle(g_switcher.surface, g_switcher.layout);
}

static void PaintFrame() {
    RunGuarded(L"painting the switcher frame", [] { PaintFrameBody(); });
}

// Consegna il pannello a DWM. L'alfa dell'animazione viaggia in
// SourceConstantAlpha: il livello è già premoltiplicato, quindi il risultato è
// esattamente l'alfa in virgola fissa dell'animazione.
static void PresentFrame() {
    if (!g_switcher.hwnd || !IsWindow(g_switcher.hwnd) || !g_switcher.surface) {
        return;
    }

    ScopedScreenDC screen(nullptr, GetDC(nullptr));
    if (!screen) {
        return;
    }

    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = static_cast<BYTE>(CurrentFadeAlpha());
    blend.AlphaFormat = AC_SRC_ALPHA;

    // UpdateLayeredWindow vuole puntatori non costanti, quindi questi sono
    // semplici variabili locali.
    POINT destination{g_switcher.panelScreen.left, g_switcher.panelScreen.top};
    SIZE size{g_switcher.surface.width, g_switcher.surface.height};
    POINT source{0, 0};
    UpdateLayeredWindow(g_switcher.hwnd, screen.value, &destination, &size,
                        g_switcher.surface.dc.value, &source, 0, &blend,
                        ULW_ALPHA);

    // Le icone stanno in una finestra a parte, sopra i preview, e vanno
    // rinfrescate a ogni presentazione.
    UpdateIconOverlay();
}

static void PaintAndPresent() {
    PaintFrame();
    PresentFrame();
}

// -----------------------------------------------------------------------------
// Le icone delle applicazioni
//
// DWM compone un'anteprima sopra il contenuto della finestra di destinazione,
// quindi un'icona disegnata nel pannello finirebbe sotto il preview. Le icone
// vivono perciò in un terzo livello, sopra quello dei preview: è una finestra a
// livelli, così solo i pixel dell'icona sono opachi.
//
// I pixel dell'icona non vengono presi da DrawIconEx, che in un DIB 32 bpp lascia
// l'alfa a zero (e quindi un rettangolo nero), né da un'immagine casuale: si
// leggono i bitmap documentati da GetIconInfo, e quando il piano dell'alfa è
// vuoto la trasparenza si ricostruisce dalla maschera AND. È la stessa logica
// della mod "Windows 10 1507 Start menu recreation", che è quella che disegna
// bene le icone, riportata qui senza GDI+ per poter comporre l'alfa a mano.
// -----------------------------------------------------------------------------

constexpr wchar_t kIconOverlayClassName[] = L"WindhawkAltTabIconOverlay";

// Un'icona decodificata: BGRA non premoltiplicato, righe dall'alto.
struct IconPixels {
    int width = 0;
    int height = 0;
    std::vector<unsigned char> pixels;

    bool Valid() const {
        return width > 0 && height > 0 && !pixels.empty();
    }
};

// Le icone delle applicazioni si ripetono: ogni finestra di Explorer ha la
// stessa icona. La cache evita di rileggere i bitmap a ogni fotogramma.
struct IconCacheEntry {
    HICON icon = nullptr;
    IconPixels pixels;
};

constexpr size_t kIconCacheLimit = 64;
static std::vector<IconCacheEntry> g_iconCache;

static void ClearIconCache() {
    g_iconCache.clear();
}

// Decodifica un HICON in pixel BGRA con alfa vero.
//
// Il percorso è quello documentato: GetIconInfo restituisce i due bitmap
// (colore e maschera AND), GetDIBits li legge in un DIB a 32 bpp. Se il piano
// dell'alfa è tutto a zero l'icona è vecchio stile e la trasparenza sta nella
// maschera: un bit a 1 nella maschera significa "trasparente". Se il piano
// dell'alfa è tutto a 255 ma la maschera ha dei buchi, vale la maschera.
static bool LoadIconPixels(HICON icon, IconPixels* out) {
    if (!icon || !out) {
        return false;
    }
    *out = IconPixels{};

    ICONINFO info{};
    if (!GetIconInfo(icon, &info)) {
        return false;
    }

    // I bitmap restituiti da GetIconInfo sono copie e vanno cancellate.
    ScopedBitmap colorBitmap(info.hbmColor);
    ScopedBitmap maskBitmap(info.hbmMask);
    if (!colorBitmap) {
        return false;  // icona monocromatica
    }

    BITMAP description{};
    if (GetObjectW(colorBitmap.value, sizeof(description), &description) !=
            sizeof(description) ||
        description.bmWidth <= 0 || description.bmHeight <= 0 ||
        description.bmWidth > 512 || description.bmHeight > 512) {
        return false;
    }

    const int width = description.bmWidth;
    const int height = description.bmHeight;
    const size_t pixelCount = static_cast<size_t>(width) *
                              static_cast<size_t>(height);
    out->pixels.assign(pixelCount * 4, 0);

    BITMAPINFO colorInfo{};
    colorInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    colorInfo.bmiHeader.biWidth = width;
    colorInfo.bmiHeader.biHeight = -height;  // righe dall'alto
    colorInfo.bmiHeader.biPlanes = 1;
    colorInfo.bmiHeader.biBitCount = 32;
    colorInfo.bmiHeader.biCompression = BI_RGB;

    ScopedScreenDC screen(nullptr, GetDC(nullptr));
    if (!screen) {
        return false;
    }
    if (GetDIBits(screen.value, colorBitmap.value, 0, static_cast<UINT>(height),
                  out->pixels.data(), &colorInfo, DIB_RGB_COLORS) != height) {
        return false;
    }

    bool hasAlpha = false;
    bool hasPartialAlpha = false;
    bool alphaAllOpaque = true;
    for (size_t i = 3; i < out->pixels.size(); i += 4) {
        const unsigned char alpha = out->pixels[i];
        hasAlpha = hasAlpha || alpha != 0;
        hasPartialAlpha = hasPartialAlpha || (alpha != 0 && alpha != 255);
        alphaAllOpaque = alphaAllOpaque && alpha == 255;
    }

    // La maschera AND, se c'è.
    std::vector<unsigned char> maskBits;
    const size_t maskStride =
        ((static_cast<size_t>(width) + 31u) / 32u) * 4u;
    bool maskHasTransparentPixel = false;
    if (maskBitmap) {
        maskBits.assign(maskStride * static_cast<size_t>(height), 0);

        struct MaskBitmapInfo {
            BITMAPINFOHEADER header;
            RGBQUAD colors[2];
        } maskInfo{};
        maskInfo.header.biSize = sizeof(BITMAPINFOHEADER);
        maskInfo.header.biWidth = width;
        maskInfo.header.biHeight = -height;
        maskInfo.header.biPlanes = 1;
        maskInfo.header.biBitCount = 1;
        maskInfo.header.biCompression = BI_RGB;
        maskInfo.colors[0] = RGBQUAD{0, 0, 0, 0};
        maskInfo.colors[1] = RGBQUAD{255, 255, 255, 0};

        if (GetDIBits(screen.value, maskBitmap.value, 0,
                      static_cast<UINT>(height), maskBits.data(),
                      reinterpret_cast<BITMAPINFO*>(&maskInfo),
                      DIB_RGB_COLORS) == height) {
            for (int y = 0; y < height && !maskHasTransparentPixel; ++y) {
                const unsigned char* row =
                    maskBits.data() + static_cast<size_t>(y) * maskStride;
                for (int x = 0; x < width; ++x) {
                    if ((row[x / 8] & (0x80u >> (x % 8))) != 0) {
                        maskHasTransparentPixel = true;
                        break;
                    }
                }
            }
        } else {
            maskBits.clear();
        }
    }

    // Nessun piano d'alfa utile e nessuna maschera: l'icona non si può
    // decodificare senza inventare i pixel, quindi non viene disegnata.
    if (!hasAlpha && maskBits.empty()) {
        return false;
    }

    const bool useMask = !hasPartialAlpha &&
        ((!hasAlpha && !maskBits.empty()) ||
         (alphaAllOpaque && maskHasTransparentPixel));

    for (int y = 0; y < height; ++y) {
        unsigned char* row =
            out->pixels.data() + static_cast<size_t>(y) *
                                     static_cast<size_t>(width) * 4;
        const unsigned char* maskRow =
            maskBits.empty() ? nullptr
                             : maskBits.data() + static_cast<size_t>(y) * maskStride;
        for (int x = 0; x < width; ++x) {
            unsigned char alpha = row[x * 4 + 3];
            if (useMask && maskRow) {
                alpha = (maskRow[x / 8] & (0x80u >> (x % 8))) ? 0 : 255;
            } else if (!hasAlpha && !maskRow) {
                alpha = 255;
            }
            if (alpha == 0) {
                row[x * 4 + 0] = 0;
                row[x * 4 + 1] = 0;
                row[x * 4 + 2] = 0;
            }
            row[x * 4 + 3] = alpha;
        }
    }

    out->width = width;
    out->height = height;
    return true;
}

static const IconPixels* GetIconPixels(HICON icon) {
    if (!icon) {
        return nullptr;
    }

    for (const IconCacheEntry& entry : g_iconCache) {
        if (entry.icon == icon) {
            return &entry.pixels;
        }
    }

    if (g_iconCache.size() >= kIconCacheLimit) {
        g_iconCache.erase(g_iconCache.begin());
    }

    IconCacheEntry entry;
    entry.icon = icon;
    if (!LoadIconPixels(icon, &entry.pixels)) {
        return nullptr;
    }
    g_iconCache.push_back(std::move(entry));
    return &g_iconCache.back().pixels;
}

// Disegna un'icona nella superficie, scalata con un'interpolazione bilineare e
// composta con l'alfa che l'icona ha davvero: è quello che toglie di mezzo i
// bordi quadrati e i riquadri neri attorno alle icone.
static void DrawIconIntoSurface(BitmapSurface& surface, const IconPixels& icon,
                                int left, int top, int size, int alpha) {
    if (!icon.Valid() || size <= 0 || alpha <= 0) {
        return;
    }

    const int right = std::min(surface.width, left + size);
    const int bottom = std::min(surface.height, top + size);
    const int startX = std::max(0, left);
    const int startY = std::max(0, top);
    const double scaleX = static_cast<double>(icon.width) / size;
    const double scaleY = static_cast<double>(icon.height) / size;

    for (int y = startY; y < bottom; ++y) {
        const double sourceY = (y - top + 0.5) * scaleY - 0.5;
        const int y0 = std::clamp(static_cast<int>(sourceY), 0, icon.height - 1);
        const int y1 = std::min(icon.height - 1, y0 + 1);
        const double fy = std::clamp(sourceY - y0, 0.0, 1.0);

        for (int x = startX; x < right; ++x) {
            const double sourceX = (x - left + 0.5) * scaleX - 0.5;
            const int x0 = std::clamp(static_cast<int>(sourceX), 0, icon.width - 1);
            const int x1 = std::min(icon.width - 1, x0 + 1);
            const double fx = std::clamp(sourceX - x0, 0.0, 1.0);

            const unsigned char* p00 = icon.pixels.data() +
                (static_cast<size_t>(y0) * icon.width + x0) * 4;
            const unsigned char* p10 = icon.pixels.data() +
                (static_cast<size_t>(y0) * icon.width + x1) * 4;
            const unsigned char* p01 = icon.pixels.data() +
                (static_cast<size_t>(y1) * icon.width + x0) * 4;
            const unsigned char* p11 = icon.pixels.data() +
                (static_cast<size_t>(y1) * icon.width + x1) * 4;

            double channels[4] = {0, 0, 0, 0};  // B, G, R, A
            for (int channel = 0; channel < 4; ++channel) {
                const double topValue =
                    p00[channel] * (1.0 - fx) + p10[channel] * fx;
                const double bottomValue =
                    p01[channel] * (1.0 - fx) + p11[channel] * fx;
                channels[channel] =
                    topValue * (1.0 - fy) + bottomValue * fy;
            }

            const int pixelAlpha =
                static_cast<int>(channels[3] * (alpha / 255.0) + 0.5);
            if (pixelAlpha <= 0) {
                continue;
            }
            BlendPixel(surface, x, y,
                       PackPremultiplied(static_cast<int>(channels[2] + 0.5),
                                         static_cast<int>(channels[1] + 0.5),
                                         static_cast<int>(channels[0] + 0.5),
                                         pixelAlpha));
        }
    }
}

// -----------------------------------------------------------------------------
// La finestra delle icone
// -----------------------------------------------------------------------------

struct IconOverlayWindow {
    HWND hwnd = nullptr;
    bool classRegistered = false;
    BitmapSurface surface;
};

static IconOverlayWindow g_iconOverlay;

static LRESULT CALLBACK IconOverlayWndProc(HWND hwnd, UINT message,
                                           WPARAM wParam, LPARAM lParam) {
    // Tutto quello che questa finestra mostra arriva da UpdateLayeredWindow,
    // quindi non ha niente da dipingere.
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static void HideIconOverlay() {
    if (g_iconOverlay.hwnd && IsWindow(g_iconOverlay.hwnd)) {
        ShowWindow(g_iconOverlay.hwnd, SW_HIDE);
    }
}

static void DestroyIconOverlay() {
    if (g_iconOverlay.hwnd && IsWindow(g_iconOverlay.hwnd)) {
        DestroyWindow(g_iconOverlay.hwnd);
    }
    g_iconOverlay.hwnd = nullptr;
    g_iconOverlay.surface.reset();
}

static bool EnsureIconOverlayWindow() {
    if (g_iconOverlay.hwnd && IsWindow(g_iconOverlay.hwnd)) {
        return true;
    }
    g_iconOverlay.hwnd = nullptr;

    HINSTANCE instance = GetModuleHandleW(nullptr);

    if (!g_iconOverlay.classRegistered) {
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = IconOverlayWndProc;
        windowClass.hInstance = instance;
        windowClass.lpszClassName = kIconOverlayClassName;
        if (!RegisterClassExW(&windowClass) &&
            GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            static RateLimitedLog log{L"the window of the application icons couldn't be registered", 1};
            if (log.ShouldLog()) {
                Wh_Log(L"The window of the application icons couldn't be registered (error %lu)",
                       GetLastError());
            }
            return false;
        }
        g_iconOverlay.classRegistered = true;
    }

    // A livelli perché solo le icone sono opache, trasparente al mouse, mai
    // attivata: è una decorazione del pannello, non una finestra sua.
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE |
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        kIconOverlayClassName, L"", WS_POPUP, 0, 0, 0, 0, g_switcher.hwnd,
        nullptr, instance, nullptr);
    if (!hwnd) {
        static RateLimitedLog log{L"the window of the application icons couldn't be created", 1};
        if (log.ShouldLog()) {
            Wh_Log(L"The window of the application icons couldn't be created (error %lu)",
                   GetLastError());
        }
        return false;
    }

    g_iconOverlay.hwnd = hwnd;
    return true;
}

static void UpdateIconOverlayBody() {
    if (!g_switcher.active || !g_switcher.hwnd || g_switcher.items.empty() ||
        g_switcher.layout.panel.right <= g_switcher.layout.panel.left) {
        HideIconOverlay();
        return;
    }

    const int width = g_switcher.layout.panel.right - g_switcher.layout.panel.left;
    const int height = g_switcher.layout.panel.bottom - g_switcher.layout.panel.top;
    if (width <= 0 || height <= 0 || !EnsureIconOverlayWindow()) {
        HideIconOverlay();
        return;
    }

    if (!g_iconOverlay.surface.Ensure(width, height)) {
        HideIconOverlay();
        return;
    }
    ClearSurface(g_iconOverlay.surface);

    const POINT origin{g_switcher.layout.panel.left, g_switcher.layout.panel.top};

    for (size_t index = 0; index < g_switcher.layout.items.size(); ++index) {
        const int itemIndex = g_switcher.layout.firstIndex + static_cast<int>(index);
        if (itemIndex < 0 ||
            itemIndex >= static_cast<int>(g_switcher.items.size())) {
            continue;
        }

        const SwitcherItem& item = g_switcher.items[itemIndex];
        if (!item.icon) {
            continue;
        }

        const IconPixels* pixels = GetIconPixels(item.icon);
        if (!pixels) {
            continue;
        }

        const ItemGeometry& geometry = g_switcher.layout.items[index];
        if (item.hasThumbnail) {
            // L'icona dell'applicazione nell'angolo in basso a destra
            // dell'anteprima, sopra il riquadro di selezione.
            RECT icon = geometry.icon;
            OffsetRect(&icon, -origin.x, -origin.y);
            DrawIconIntoSurface(g_iconOverlay.surface, *pixels, icon.left,
                                icon.top, geometry.icon.right - geometry.icon.left,
                                255);
            continue;
        }

        // Il desktop senza anteprima mostra lo sfondo, che disegna il pannello:
        // qui non ci va niente, altrimenti l'icona coprirebbe lo sfondo.
        if (item.isDesktop) {
            continue;
        }

        // Una finestra senza anteprima mostra l'icona grande al centro, come
        // faceva Windows 7 con le finestre ridotte a icona.
        const int thumbH = geometry.thumb.bottom - geometry.thumb.top;
        const int thumbW = geometry.thumb.right - geometry.thumb.left;
        const int size = std::max(16, std::min(thumbH * 2 / 5, thumbW));
        const int x = geometry.thumb.left + (thumbW - size) / 2 - origin.x;
        const int y = geometry.thumb.top + (thumbH - size) / 2 - origin.y;
        DrawIconIntoSurface(g_iconOverlay.surface, *pixels, x, y, size, 255);
    }

    SetWindowPos(g_iconOverlay.hwnd, HWND_TOPMOST, g_switcher.panelScreen.left,
                 g_switcher.panelScreen.top, width, height,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);

    ScopedScreenDC screen(nullptr, GetDC(nullptr));
    if (!screen) {
        return;
    }

    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = static_cast<BYTE>(CurrentFadeAlpha());
    blend.AlphaFormat = AC_SRC_ALPHA;

    POINT destination{g_switcher.panelScreen.left, g_switcher.panelScreen.top};
    SIZE size{width, height};
    POINT source{0, 0};
    UpdateLayeredWindow(g_iconOverlay.hwnd, screen.value, &destination, &size,
                        g_iconOverlay.surface.dc.value, &source, 0, &blend,
                        ULW_ALPHA);
}

static void UpdateIconOverlay() {
    RunGuarded(L"drawing the application icons",
               [] { UpdateIconOverlayBody(); });
}

// -----------------------------------------------------------------------------
// La scheda del desktop
//
// Windows 7 e Windows 8.1 mettevano in fondo alla fila delle anteprime anche il
// desktop, con il nome del desktop e un'anteprima viva dello sfondo: era la via
// più rapida per arrivare alle icone del desktop senza minimizzare a mano ogni
// finestra. È la voce che si vede selezionata nel fotogramma di riferimento,
// con il titolo "Desktop".
//
// Qui la si ricostruisce con API pubbliche:
//
//  * la finestra sorgente dell'anteprima è quella del desktop, GetShellWindow(),
//    che è la finestra di Explorer che disegna sfondo e icone; DWM ne sa fare
//    la miniatura come per qualunque altra finestra di primo livello
//    (DwmRegisterThumbnail documenta che la sorgente deve essere una finestra di
//    primo livello, e questa lo è);
//  * se DWM rifiuta la miniatura, si disegna lo sfondo letto da
//    SystemParametersInfo(SPI_GETDESKWALLPAPER) con GDI+, ritagliato come farebbe
//    la miniatura;
//  * l'icona è quella che la shell usa per la propria finestra: WM_GETICON su
//    GetShellWindow(), poi l'icona della classe, poi quella della cartella
//    Desktop dal namespace della shell (SHGetFileInfo con SHGFI_USEFILEATTRIBUTES,
//    che non dipende dal fatto che la cartella esista davvero);
//  * l'attivazione è IShellDispatch4::ToggleDesktop, la stessa chiamata del
//    pulsante "Mostra desktop" della barra delle applicazioni, con
//    IShellDispatch::MinimizeAll come ripiego.
//
// Nessuna di queste chiamate tocca explorer.exe: sono tutte API documentate.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// RAII
//
// Le risorse che questa parte della mod prende in prestito dal sistema (un
// oggetto COM, un apartments COM, il runtime di GDI+) vengono chiuse da un
// distruttore e non da un percorso di uscita scritto a mano: così una `return`
// anticipata, un errore di HRESULT o un'eccezione non lasciano niente aperto.
//
// Nota sulle eccezioni, perché è il punto su cui si sbaglia facilmente: le API
// COM e Win32 non lanciano, riportano il fallimento nel valore di ritorno, e con
// clang `catch (...)` non intercetta le eccezioni strutturate di Windows (SEH).
// L'unica cosa che può uscire da qui è un'eccezione C++ di allocazione, per
// esempio std::bad_alloc da una std::wstring; i try/catch di questo file
// esistono per quella, non per le chiamate Win32.
// -----------------------------------------------------------------------------

// Un puntatore a interfaccia COM che rilascia da solo.
template <typename T>
class ScopedComPtr {
  public:
    ScopedComPtr() = default;
    ~ScopedComPtr() {
        reset();
    }

    ScopedComPtr(const ScopedComPtr&) = delete;
    ScopedComPtr& operator=(const ScopedComPtr&) = delete;

    T* get() const {
        return value;
    }

    T** put() {
        reset();
        return &value;
    }

    T* operator->() const {
        return value;
    }

    explicit operator bool() const {
        return value != nullptr;
    }

    void reset() {
        if (value) {
            value->Release();
            value = nullptr;
        }
    }

  private:
    T* value = nullptr;
};

// L'apartment COM del thread. Il tipo ScopedComApartment esiste già nel file:
// qui c'è solo il modo di aprirlo, perché la chiamata a CoInitializeEx deve
// restare bilanciata con una CoUninitialize su ogni percorso di uscita.
//   S_OK / S_FALSE  -> la chiamata è nostra e va chiusa (S_FALSE vuol dire che
//                      l'apartment era già aperto, ma il riferimento è nostro);
//   RPC_E_CHANGED_MODE -> il thread è già in un altro apartment: non si chiude
//                      niente, e le chiamate restano possibili.
static bool OpenComApartment(ScopedComApartment* apartment) {
    const HRESULT result = CoInitializeEx(
        nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (result == RPC_E_CHANGED_MODE) {
        return true;
    }
    if (FAILED(result)) {
        return false;
    }
    apartment->reset(true);
    return true;
}

// Il runtime di GDI+ serve solo al ripiego dello sfondo: si avvia alla prima
// richiesta e si chiude quando la mod viene scaricata.
class ScopedGdiplus {
  public:
    ScopedGdiplus() {
        Gdiplus::GdiplusStartupInput input;
        token_ = 0;
        started_ =
            Gdiplus::GdiplusStartup(&token_, &input, nullptr) == Gdiplus::Ok;
    }

    ~ScopedGdiplus() {
        if (started_ && token_) {
            Gdiplus::GdiplusShutdown(token_);
        }
    }

    ScopedGdiplus(const ScopedGdiplus&) = delete;
    ScopedGdiplus& operator=(const ScopedGdiplus&) = delete;

    bool started() const {
        return started_;
    }

  private:
    ULONG_PTR token_ = 0;
    bool started_ = false;
};

static bool EnsureGdiplus() {
    static ScopedGdiplus gdiplus;
    return gdiplus.started();
}

// -----------------------------------------------------------------------------
// Nome, percorso e icona del desktop
// -----------------------------------------------------------------------------

// Le tre costanti che la mod non può prendere dai file di intestazione.
//
// FOLDERID_Desktop, IID_IShellDispatch e IID_IShellDispatch4 sono dichiarate
// dagli header ma *definite* in uuid.lib, e il link della mod non ha quella
// libreria: erano gli unici simboli che il linker non trovava. Qui sono scritte
// per esteso, con lo stesso criterio già usato in questo file per
// kCLSID_VirtualDesktopManager, così la mod si collega senza aggiungere
// librerie di sistema.
constexpr GUID kFOLDERID_Desktop = {
    0xB4BFCC3A, 0xDB2C, 0x424C, {0xB0, 0x29, 0x7F, 0xE9, 0x9A, 0x87, 0xC6, 0x41}};
constexpr GUID kIID_IShellDispatch = {
    0xD8F015C0, 0xC278, 0x11CE, {0xA4, 0x9E, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
constexpr GUID kIID_IShellDispatch4 = {
    0xEFD84B2D, 0x4BCF, 0x4298, {0xBE, 0x25, 0xEB, 0x54, 0x2A, 0x59, 0xFB, 0xDA}};

// Il nome che la shell dà al desktop nella lingua di Windows: "Desktop" in
// inglese, "Scrivania" in italiano. SHGetNameFromIDList su FOLDERID_Desktop è
// l'unico modo documentato per ottenerlo senza scriverlo a mano.
static std::wstring GetDesktopDisplayName() {
    std::wstring name = L"Desktop";

    ScopedComApartment apartment;
    if (!OpenComApartment(&apartment)) {
        return name;
    }

    PIDLIST_ABSOLUTE idList = nullptr;
    if (FAILED(SHGetKnownFolderIDList(kFOLDERID_Desktop, 0, nullptr, &idList)) ||
        !idList) {
        return name;
    }

    PWSTR displayName = nullptr;
    if (SUCCEEDED(SHGetNameFromIDList(idList, SIGDN_NORMALDISPLAY,
                                      &displayName)) &&
        displayName) {
        if (displayName[0]) {
            name = displayName;
        }
        CoTaskMemFree(displayName);
    }

    CoTaskMemFree(idList);
    return name;
}

static std::wstring GetDesktopFolderPath() {
    std::wstring path;

    ScopedComApartment apartment;
    if (!OpenComApartment(&apartment)) {
        return path;
    }

    PWSTR folder = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(kFOLDERID_Desktop, 0, nullptr, &folder)) &&
        folder) {
        path = folder;
        CoTaskMemFree(folder);
    }
    return path;
}

// -----------------------------------------------------------------------------
// L'icona della scheda del desktop
//
// È il ritratto del desktop di Windows 7 — lo sfondo azzurro con la barra delle
// applicazioni in basso e il pulsante Start a sinistra — disegnato a 64×64 e
// incorporato qui dentro come PNG in base 64. Non si legge nessun file, non si
// chiede niente all'utente e non si tocca il registro: i byte stanno nel codice.
//
// Se per qualunque motivo la decodifica non riuscisse, la mod ripiega
// sull'icona che la shell dà al desktop (WM_GETICON sulla finestra della shell,
// poi SHGetFileInfoW sulla cartella Desktop), quindi la scheda non resta mai
// senza icona.
// -----------------------------------------------------------------------------

static const char kDesktopIconPngBase64[] =
    "iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAO0ElEQVR42u1bTY9lR3l+nqo6997u"
    "np6WP2ZMHMNYIbaxSIIgiQRiEUikrKKIReQVISz4F1nkL2QZCZRICAlhJZIXkULY2CAQIJFFMiEx"
    "NhB7GNuZ6ZlJ94ynu889p943i6o6pz5OD2aRbDxXut3n655T79fzPu9bdYCHn4efh5/384dpQ1Xt"
    "K3H/Mw/4wSv4PzlZPvUzDx7DL33KKw9+5uHhob7wwgseAKiqBACS+r4yvSoVgEuCX339F5/1yo9Q"
    "xHSbNecrBfCAZLuApH+AAbz47BqBCAB4eBHEUxARQDwEgPfhh9YCBhbGANYYGGvQGYuuM3Cug+0M"
    "nO3QOYPOAsZYWBjApGEJRDzEA8Mo6IcB/TBi6Iew3Q846Xv0w4ixH7D1XkUpMoyvfpF8mQB4eHi4"
    "Pwq/+sTjB58jFQqANFBRKBSiClVARKGq83EJx1XTNeGbrhNRiMr0W1GFSthXKBi8DsYQxhhYY+Cs"
    "hXMG1jl0zsI5B2ftezLoMHpstyP67RD/b3HWD3E/KmYYMIwewzDi+Pjdl07M6gv86ZvXv/yBS5e/"
    "9Jd//fL4P/cV65WFMYSzRNc5rDqDlQtfZw2sJYwBCIIMQkSfmv9mihFR+PgdvWCUTEmafsWgEEOQ"
    "JiolfC0Jaw0Yt8M1nMGLADQYTlWhECDdO1gIqjKNThRYO+K3P7Tnju7c/Iq7eePw8ytj/T9857a9"
    "fnqJe3trdJbYWRnsbiz2dix21xabtcG6M+gcYY2BNbMFgyLCaBKQiCi8AqMoRg8MXjF6nf6PAnhJ"
    "Aw1eoZwVC0OQCkPG5+Qek2+XX2uCkqZjRBhjVKAH8eiO6odPrvutHz7vtsOwURU8crCD073HsL+3"
    "wcoBO2uD3bWJwhPrjuisgbPIhM6FjwogoFG4UQAnwOAB6xWDANYDooBXTMKHm2C6p7HRylHwIMi8"
    "zSjYJCgJGsRrwpiKMUalGRBC4uJqpMg1C8A6VUlpECoCqIAgLBWGCkOBoYlOGvFAASpgTHDhlD80"
    "ng+WB7xXDD4oIFl9FIUoAyZEj0lhZExQhoKThYMXE2rCtQqCcd9AgbStiNcE5aT/NISRcFwYnq2q"
    "0zNdilsSMJbo3PxdRZd3k9thij+T5QmdMku0biG8Tgrwkiyv82/ifQxTQIcDyavI7BvPR4cJ16Zt"
    "ZscTRpDhN5zhIvdagnAa0APGEJ0SzhqsHbByRGcDGFob3SsbVDaKEPPRnUcfrFwLPybhZQY+VEpk"
    "Fgaz8FGI9JNJyOza+jhmQ3Haz2lflIUIClAFrDVwJFYOWHUMudcFBRjOHgCG63Psny0fBB28Yhhn"
    "BYQsgOD6QdRZYMU88pyiRkvNgrcWnk+zsvx8j9ydWHhX2HMpPTjDqAAzhYCzAVWNYfVgniv8WAkf"
    "0D5ZXgNZIoMKlBHtZyXkLp/2J6uCy5bnsuVzz6jvM4VAAjZniA6shG9dKVlEogAh1wM+c/0xpTqP"
    "LO6TxwQwrV1yfgYzL8itmwTIwiE7l8d6HvN52ZOPPzxf4RDJiHXECkTnMMV+yqN5qgv/dYp7iYg/"
    "eMUgrfVFMBEezV2XVVXGTMjC3Zf2s3swV0imtGzA+b2hOYEjXEhdGmKeCfgAazAJH2uHeTu5fozt"
    "cSI5Sfho/Qb0tJI2s0bmBagQvrYcq3M5NiD3ltYpAOokfMCAQMPQ2agAF+huin2wHG8e96Izw9vW"
    "wkfLS/Z4apU/M0rbov8c57MXlO5fhEWNAbkSc4UoJqI0ZwFg8gDnEGM/z51lrk/WHyPCp7gfvGIc"
    "Aa+BBicSgxhvMxpXoEUuo3+bbYsQIlncqzB34QEsFJJIOwE4xCrOOWLFmecn9zfZnXPrB8RH6fZJ"
    "eEmgR0h8mEFpmRoHakrN2t0zhSG/tiE/GTiiBcWp1oj3cZJhgANn67PM9WlDYoFTML1RK6YXrc+E"
    "+5wKnRrdl8lPZd1IhlhlghoPUIMj8ywWT2qJnQEDADhn4NRExjdz5bxNlPK9j3E+imIcNShE5pQ3"
    "Kyzep4r1PK4Lz2WbGkrEr8IHNQbwHOArwzlXqkOs250hbO4BGWNCBDMfc/6YWT6BXkJ91dTuiNiR"
    "D2gB7ScF5ZZnyzsKIpSBQ4sB1fFKgdDy3i7FtbWEkRIxZ88Pgouk+r4SPNb+U8qr47JC5Nbd6+KH"
    "JQXO3b8qZvgA7s+CLJeUKN3HiQ8tLmsMTJVKE2eXnO5mjY3E80eZhZ/K4wWiU7K8Ct5Ro3/V9Wnc"
    "viYBqF2tpMoFAAKpB2w09K8C68vvlOW9Ke1lyD+OqfKLwsfrylxVCdZw99bduVTxNdUj0TBfnvO/"
    "UTiLfOQ09uhC86BMHRpjXyLApZgffA58UUE52clyXEo5RRrLjZelnLb0ZVP6LqZHcjEV1h7CBa8w"
    "qXE4V3wz4QE0trcyujvmPb0UHsz8v3VFosSVRbJzLvdnBYjL+Z6LfKAMm9prCMBIjPH5oazSXtng"
    "GCfw08n6GoESWdYoDFB3c+rSt+H6dS2w3PQAy1qiLQjZMkiUDVyD2L8HS7QM4KcT8A1jYHpDzP8J"
    "9CSmkVLTbLowc+zzfLSvwasiPwWm58pB60FgS7LQhJ8GD1CN7ckM+4L1OQFf8IKZ8UkOepHvaca0"
    "wDLlFJZYQPu6B9iarSxtG+4PLHjA0ixoFiYKOImzNbn0M+dPrC+julWNz6ZCyRGerQeg7PQUlqnA"
    "swiVBu2rJkpVIdYgUJTO2bNNmsHJtT3V+pnwyQtEFTJNbumya9UtqMl6LEKhrgXKOqCqG84pfVts"
    "qAjRQiWYtjUoAFDRuc+XLC8lAE6uL3NJnN8o98s61eGcPF9ng5zDoMKIJYuXfJ+VN7bVZpF1Ypab"
    "PaAAv1Tn18LrxArzMolZfV1Q6Zx4sHXxORy4TJEXUmTb6GwLoMIbWLHbAhAZFKAi06yepomNjPen"
    "trZqNk3eFNus2tV8gAe0gjaguOi1rUmJc9jnOctBCm6kCiMic9OyAb68p69TZxfacu+GcLBmfed3"
    "fuqCKK/0moZogQG10nhOFik5Tp7qXZq7T6kPk/URav2ohBb5Q/DTVDU+quko4nxBqyoPNXFa4v5V"
    "6buYMitPqnEmc4E5BNIE6TSXP84THV6C6ysWqq66/FxAfxTWZlUG5/OBbHoANSkiW+5Qon8NrC1J"
    "SJgVPEBnYBMJDYyp2+Pz5iaKxE/UTYqqjV1jwpLlF2uBc+oAVjyfC2VzHe/NfEY2vZeIUMj5wQO8"
    "KITIWtvR9eNyIC7g3mIZukA8fpVaAEtTYGh5/nllNljWAmxmiqIPqCQmqNOKjokA1eCny1xzsQxt"
    "ZmPr2F9ifRUoFsNl00cEl7MDz88B1bNjL1TiwqcJ/TVbyJDa2wskn6h5Ocsaf6Hvz7zoWbBwMfff"
    "9P3ZelhFfsosszzJWi6OTA0RDUvbhqiIwStCq2zuCdawQi7U3tXMTV0NAgvhUc/gNFiyVM+zmRdg"
    "k17ZsEBWnWeF5iAYhB5EoEoM4nC6FXgFVtbAWoGqwGCeME09RJNp3FDjfrCZQew2TYAWWuVm2sZ8"
    "Hpha8qRm6B/vEWYY2lDJftO6OiviouHeUxaQkAZHD/SjAewax+8eY4e3cWW/h1fixt0N3u0fxe7u"
    "HrbiYWP30DD0ERnX6BgJi5WshnU8hvG4EkYJ1TBTlPYN5zVA1LjAKq0BYmpYmHltUCy5Deb0OWcB"
    "FsWPIbO1CLPHBkCfJzodoNiOio8+BTx3pcePrl7Fx55b41Of/BA2K4sNPc7un+Gff/Bz3D/d4Jmn"
    "n8J2GGCMyeLMTEva0PTxytmZvBo0LLvDuafkcwWG7URJ+p1ZANM6hHK26r3isd11WMsIhVMReAEO"
    "Vif42Rv/gSuPAb9x5SkMX/8bbMbb+Nff/DT6x5/H7z+zh9d++jOs7h/j8sE+/DiG1R1a1/NZnKYi"
    "S9lWZAsLG+p6v7526u7qYnXbVoBVa5gEdPQwvAjsdYBEDDDG4L9v3Mbrb76NR559Bv/+rW/ir/AN"
    "7D1/Ae/wFK/JD/H9fznBk+53cefI48neQ0SqdTxs2/JVU3T5mmVQa1eDVHH9QAUwnwgqQHoYRljr"
    "8OjeASRhAI3Bm9eu4Xjo8Yv/egfPHd3E9z7+aWx37uKxZ57FHz35Bn5+823856vP49cOOsjpPYj4"
    "YqqrXJikjfDEgqekayuuX/f93otiit8tzAukNLndDtDhBE9/4CDygEiEbt26hR9euwPsvIvXfutP"
    "sf97f4xH3/wedt74AS4c7+Dk+gdx9a0tbh/fwJPbuzi6dxfW2Er4yvLnkKX2XB0KSwqpptrOK5U5"
    "r0Rqu1XEWd/j/t2n8alPPJ+IUOAA6keMSmB9Aevdi/i3d27hL564hT//6AmG6z/GP/GTGDYX4XEP"
    "9+4e47TvYY1ZbkbWS9wqdphWnS4taSvyOLWd3VmoI9BcI9n+PNtjSJyd9ej7fuYBhu6MZHd2emIu"
    "nB3x3uYK+rd+gu8f3cMfPPcd+P1v48evH+BH/lnwcYvdG2/j6PgIQlO5tVaDqOp7LMds2/ioGpyK"
    "qWPNrApizUwLRbRjSQo4Oetx/+RESQqBwR0cHHxts7PzJbO+MF7cvoX99QZnH/4d7F++jL+1H8Q3"
    "r/4hju6uYX79ETx77Sdw945hNxexMma5CGLZ4srp8Lx4Gc0ML6pU2bLMPA22HjVNdSEsxjRpXIYF"
    "Szztt3ji8uO4sH/Rde7m3/Gll767f8/3X6Xd+RwU6JwFNzvoOgu7WQN2hV1L7DhgDYHtHFZdh85Z"
    "dJ2DteFlB2PMvHxOJBZVHt4LRvHwo8c4egzew/sR4xjOeR/fLIlY1M7flWuD6/I2X31YupJO5Yui"
    "nOoniTu377x0662jL0yX//0/vvLZzrmPjKpmJcKuM3DG4sLaoNus0a3XcDu72F134ZWWrkPnHDpr"
    "o/CYhB+8h4yC3g8YeolvaoS3Nvp+wBBfbRlHH86JQIYhvmojyZSBBwogRuIhk17WCW/NZO/x+Lrk"
    "8/Go5Phg1DknJ0P/6hf/7E9efl+/NKWqZP7S1IsvvmgvXbrE9/Rq26/wWtt7eY3t//tzeHioJP3D"
    "tyYffh5+Hn7+F+XYHCDPB7InAAAAAElFTkSuQmCC";

// Decodifica in base 64. Restituisce i byte del PNG, o un vettore vuoto se la
// stringa non è valida.
static std::vector<unsigned char> DecodeBase64(const char* text) {
    auto valueOf = [](char character) -> int {
        if (character >= 'A' && character <= 'Z') {
            return character - 'A';
        }
        if (character >= 'a' && character <= 'z') {
            return character - 'a' + 26;
        }
        if (character >= '0' && character <= '9') {
            return character - '0' + 52;
        }
        if (character == '+') {
            return 62;
        }
        if (character == '/') {
            return 63;
        }
        return -1;
    };

    std::vector<unsigned char> result;
    if (!text) {
        return result;
    }

    unsigned int buffer = 0;
    int bits = 0;
    for (const char* cursor = text; *cursor; ++cursor) {
        if (*cursor == '=') {
            break;
        }
        const int value = valueOf(*cursor);
        if (value < 0) {
            continue;  // righe vuote, a capo, spazi
        }
        buffer = (buffer << 6) | static_cast<unsigned int>(value);
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            result.push_back(
                static_cast<unsigned char>((buffer >> bits) & 0xFFu));
        }
    }
    return result;
}

// Il flusso PNG vive il tempo di una chiamata: viene letto tutto e chiuso
// subito, così non resta nessun handle aperto se più avanti qualcosa fallisce.
class ScopedPngStream {
  public:
    explicit ScopedPngStream(const std::vector<unsigned char>& bytes)
        : stream_(nullptr) {
        stream_ = SHCreateMemStream(bytes.data(),
                                    static_cast<UINT>(bytes.size()));
    }

    ~ScopedPngStream() {
        if (stream_) {
            stream_->Release();
        }
    }

    ScopedPngStream(const ScopedPngStream&) = delete;
    ScopedPngStream& operator=(const ScopedPngStream&) = delete;

    IStream* get() const {
        return stream_;
    }

    bool valid() const {
        return stream_ != nullptr;
    }

  private:
    IStream* stream_;
};

// L'icona incorporata, creata una volta sola. Il distruttore la distrugge
// quando la mod viene scaricata, e anche questa volta senza percorsi di uscita
// scritti a mano.
struct EmbeddedDesktopIcon {
    HICON icon = nullptr;
    bool attempted = false;

    ~EmbeddedDesktopIcon() {
        if (icon) {
            DestroyIcon(icon);
            icon = nullptr;
        }
    }
};

static HICON GetEmbeddedDesktopIcon() {
    static EmbeddedDesktopIcon embedded;
    if (embedded.attempted) {
        return embedded.icon;
    }
    embedded.attempted = true;

    // GDI+ è già usato per lo sfondo: qui serve solo per leggere il PNG.
    if (!EnsureGdiplus()) {
        return nullptr;
    }

    std::vector<unsigned char> bytes;
    try {
        bytes = DecodeBase64(kDesktopIconPngBase64);
    } catch (...) {
        Wh_Log(L"The embedded desktop icon couldn't be decoded");
        return nullptr;
    }
    if (bytes.empty()) {
        return nullptr;
    }

    ScopedPngStream stream(bytes);
    if (!stream.valid()) {
        return nullptr;
    }

    Gdiplus::Bitmap bitmap(stream.get(), FALSE);
    if (bitmap.GetLastStatus() != Gdiplus::Ok) {
        return nullptr;
    }

    HICON created = nullptr;
    if (bitmap.GetHICON(&created) != Gdiplus::Ok || !created) {
        return nullptr;
    }
    embedded.icon = created;
    return embedded.icon;
}

// L'icona del desktop, con la stessa catena con cui si legge quella delle
// applicazioni: prima quella che la finestra dichiara, poi quella della classe,
// poi quella che la shell usa per la cartella Desktop.
static HICON LoadDesktopIcon(bool* owned) {
    *owned = false;

    // Prima di tutto l'icona incorporata: è quella che si vede nello switcher
    // originale, e non dipende da quello che il tema di turno decide di dare
    // alla cartella "Desktop" (che è la ragione per cui prima si vedeva una
    // comune icona di cartella).
    if (HICON embedded = GetEmbeddedDesktopIcon()) {
        return embedded;
    }

    const HWND shellWindow = GetShellWindow();
    if (shellWindow) {
        const WPARAM iconKinds[] = {ICON_BIG, ICON_SMALL2, ICON_SMALL};
        for (WPARAM kind : iconKinds) {
            DWORD_PTR result = 0;
            if (SendMessageTimeoutW(shellWindow, WM_GETICON, kind, 0,
                                    SMTO_ABORTIFHUNG | SMTO_NORMAL, 100,
                                    &result) &&
                result) {
                return reinterpret_cast<HICON>(result);
            }
        }

        LONG_PTR classIcon = GetClassLongPtrW(shellWindow, GCLP_HICON);
        if (!classIcon) {
            classIcon = GetClassLongPtrW(shellWindow, GCLP_HICONSM);
        }
        if (classIcon) {
            return reinterpret_cast<HICON>(classIcon);
        }
    }

    // SHGFI_USEFILEATTRIBUTES: l'icona della cartella senza toccare il disco.
    // FILE_ATTRIBUTE_DIRECTORY è quello che rende la richiesta "una cartella".
    std::wstring folder = GetDesktopFolderPath();
    if (folder.empty()) {
        folder = L"Desktop";
    }

    SHFILEINFOW fileInfo{};
    if (SHGetFileInfoW(folder.c_str(), FILE_ATTRIBUTE_DIRECTORY, &fileInfo,
                       sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES) &&
        fileInfo.hIcon) {
        *owned = true;
        return fileInfo.hIcon;
    }

    return nullptr;
}

// -----------------------------------------------------------------------------
// La voce del desktop nella lista delle finestre
// -----------------------------------------------------------------------------

// Aggiunge il desktop in fondo alla lista, come facevano i due originali. La
// finestra sorgente è quella della shell: è lei che disegna sfondo e icone.
static bool BuildDesktopItem(SwitcherItem* item) {
    const HWND shellWindow = GetShellWindow();
    if (!shellWindow || !IsWindow(shellWindow)) {
        return false;
    }

    item->hwnd = shellWindow;
    item->isDesktop = true;
    item->minimized = false;
    item->thumbnail = nullptr;
    item->hasThumbnail = false;

    // Le due sole cose che possono lanciare sono le stringhe; le chiamate di
    // shell riportano il loro esito e basta.
    try {
        item->title = GetDesktopDisplayName();
        item->appPath = GetDesktopFolderPath();
    } catch (...) {
        Wh_Log(L"The desktop card couldn't be built");
        return false;
    }

    item->icon = LoadDesktopIcon(&item->ownIcon);
    return true;
}

// -----------------------------------------------------------------------------
// Mostrare il desktop
//
// ToggleDesktop è la chiamata che sta dietro al pulsante "Mostra desktop": se il
// desktop è già in primo piano lo rimette via, esattamente come fa il pulsante.
// Se la coclasse Shell.Application non risponde si ripiega su MinimizeAll, che
// è la stessa cosa vista dalla parte delle finestre.
// -----------------------------------------------------------------------------

// CLSID della coclasse "Shell.Application" (shldisp.idl): la definisce il file
// di intestazione dell'SDK, non quello di mingw, quindi la si dichiara qui.
static const CLSID kShellApplicationClsid = {
    0x13709620, 0xC279, 0x11CE, {0xA4, 0x9E, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};

static bool ShowDesktopWithShell() {
    ScopedComApartment apartment;
    if (!OpenComApartment(&apartment)) {
        return false;
    }

    ScopedComPtr<IShellDispatch4> shell4;
    if (SUCCEEDED(CoCreateInstance(kShellApplicationClsid, nullptr,
                                   CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER,
                                   kIID_IShellDispatch4,
                                   reinterpret_cast<void**>(shell4.put()))) &&
        shell4) {
        if (SUCCEEDED(shell4->ToggleDesktop())) {
            return true;
        }
    }

    ScopedComPtr<IShellDispatch> shell;
    if (SUCCEEDED(CoCreateInstance(kShellApplicationClsid, nullptr,
                                   CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER,
                                   kIID_IShellDispatch,
                                   reinterpret_cast<void**>(shell.put()))) &&
        shell) {
        if (SUCCEEDED(shell->MinimizeAll())) {
            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------
// Lo sfondo, quando DWM non concede la miniatura
//
// SPI_GETDESKWALLPAPER restituisce il percorso del file dello sfondo. GDI+ lo
// legge e lo ritaglia come farebbe DWM ("cover": tutto il riquadro riempito,
// proporzioni conservate, il resto tagliato ai lati).
//
// Il disegno avviene in una superficie GDI+ premoltiplicata e poi si copia a
// mano: disegnare GDI+ direttamente nel DIB della mod passando per un HDC
// azzererebbe il canale alfa, che è il difetto classico di questo passaggio.
// -----------------------------------------------------------------------------

static bool DrawWallpaperInto(BitmapSurface& surface, const RECT& rect) {
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) {
        return false;
    }

    wchar_t wallpaperPath[MAX_PATH]{};
    if (!SystemParametersInfoW(SPI_GETDESKWALLPAPER, ARRAYSIZE(wallpaperPath),
                               wallpaperPath, 0) ||
        !wallpaperPath[0]) {
        return false;
    }

    if (!EnsureGdiplus()) {
        return false;
    }

    // Le classi di GDI+ sono in un namespace: qui si entra solo per questa
    // funzione, così il resto del file resta scritto come prima.
    using namespace Gdiplus;

    Bitmap source(wallpaperPath, FALSE);
    if (source.GetLastStatus() != Ok) {
        return false;
    }

    Bitmap target(width, height, PixelFormat32bppPARGB);
    if (target.GetLastStatus() != Ok) {
        return false;
    }

    const int sourceWidth = static_cast<int>(source.GetWidth());
    const int sourceHeight = static_cast<int>(source.GetHeight());
    if (sourceWidth <= 0 || sourceHeight <= 0) {
        return false;
    }

    // Ritaglio "cover": si prendono dal centro dell'immagine le proporzioni
    // del riquadro.
    double scale = static_cast<double>(width) / sourceWidth;
    const double verticalScale = static_cast<double>(height) / sourceHeight;
    if (verticalScale > scale) {
        scale = verticalScale;
    }
    const double cropWidth = width / scale;
    const double cropHeight = height / scale;
    const double cropLeft = (sourceWidth - cropWidth) / 2;
    const double cropTop = (sourceHeight - cropHeight) / 2;

    {
        Graphics graphics(&target);
        if (graphics.GetLastStatus() != Ok) {
            return false;
        }
        graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
        graphics.SetCompositingMode(CompositingModeSourceCopy);

        RectF destination(0.0f, 0.0f, static_cast<REAL>(width),
                          static_cast<REAL>(height));
        RectF crop(static_cast<REAL>(cropLeft), static_cast<REAL>(cropTop),
                   static_cast<REAL>(cropWidth), static_cast<REAL>(cropHeight));
        if (graphics.DrawImage(&source, destination, crop.X, crop.Y, crop.Width,
                               crop.Height, UnitPixel) != Ok) {
            return false;
        }
    }

    BitmapData data{};
    Rect lockRect(0, 0, width, height);
    if (target.LockBits(&lockRect, ImageLockModeRead, PixelFormat32bppPARGB,
                        &data) != Ok) {
        return false;
    }

    bool drawn = false;
    if (data.Scan0 && data.Stride > 0) {
        for (int row = 0; row < height; ++row) {
            const auto* sourceRow = reinterpret_cast<const uint32_t*>(
                static_cast<const BYTE*>(data.Scan0) +
                static_cast<size_t>(row) * static_cast<size_t>(data.Stride));
            for (int column = 0; column < width; ++column) {
                // I pixel di GDI+ sono già premoltiplicati: si sommano al
                // contenuto della superficie con la regola AC_SRC_OVER.
                BlendPixel(surface, rect.left + column, rect.top + row,
                           sourceRow[column] | 0xFF000000u);
            }
        }
        drawn = true;
    }

    target.UnlockBits(&data);
    return drawn;
}

// Il riquadro dell'anteprima quando la miniatura non c'è: prima lo sfondo, poi
// una tinta unita, così la scheda non resta mai vuota.
static void DrawDesktopCardBackground(BitmapSurface& surface, const RECT& rect) {
    if (DrawWallpaperInto(surface, rect)) {
        return;
    }
    FillRoundedRect(surface, rect, 0, RGB(28, 56, 92), 255);
}

// Session lifetime
// -----------------------------------------------------------------------------

enum class SessionResult {
    Ok,
    NothingToShow,  // fewer than two windows: the original does nothing either
    Failed,
};

static void AssignItemRects() {
    const SwitcherLayout& layout = g_switcher.layout;

    for (SwitcherItem& item : g_switcher.items) {
        item.card = RECT{};
        item.thumb = RECT{};
    }

    for (size_t index = 0; index < layout.items.size(); ++index) {
        const int itemIndex = layout.firstIndex + static_cast<int>(index);
        if (itemIndex < 0 ||
            itemIndex >= static_cast<int>(g_switcher.items.size())) {
            continue;
        }

        SwitcherItem& item = g_switcher.items[itemIndex];
        item.card = layout.items[index].cell;
        item.thumb = layout.items[index].thumb;
    }
}

static void RecomputeLayoutAndRects() {
    int page = std::max(0, g_switcher.page);
    for (int pass = 0; pass < 2; ++pass) {
        g_switcher.layout = ComputeLayout(g_switcher.settings, g_switcher.items,
                                         page, g_switcher.workArea,
                                         g_switcher.dpi);

        int derivedPage = page;
        if (g_switcher.selectedIndex >= 0 && !g_switcher.items.empty()) {
            derivedPage = g_switcher.selectedIndex /
                std::max(1, g_switcher.layout.pageSize);
        }
        derivedPage =
            std::clamp(derivedPage, 0, g_switcher.layout.pageCount - 1);
        if (derivedPage == page) {
            break;
        }
        page = derivedPage;
    }

    g_switcher.page = page;
    AssignItemRects();
}

static void SelectIndex(int index) {
    const int count = static_cast<int>(g_switcher.items.size());
    if (count <= 0) {
        return;
    }

    index = ((index % count) + count) % count;
    const int pageSize = std::max(1, g_switcher.layout.pageSize);
    const int page = index / pageSize;
    const bool pageChanged = page != g_switcher.page;

    g_switcher.selectedIndex = index;
    if (pageChanged) {
        g_switcher.page = page;
        g_switcher.hoverIndex = -1;
        RecomputeLayoutAndRects();

        PaintAndPresent();
        RefreshThumbnails();
    }

    PaintAndPresent();
}

static void MoveSelection(int delta) {
    const int count = static_cast<int>(g_switcher.items.size());
    if (count <= 0 || delta == 0) {
        return;
    }

    int index = g_switcher.selectedIndex;
    if (index < 0) {
        index = delta > 0 ? -1 : 0;
    }

    SelectIndex(index + delta);
}

static int IndexAtPoint(int x, int y) {
    const POINT point{x, y};
    for (int i = 0; i < static_cast<int>(g_switcher.items.size()); ++i) {
        const RECT& card = g_switcher.items[i].card;
        if (card.right > card.left && PtInRect(&card, point)) {
            return i;
        }
    }
    return -1;
}

static void RefreshThumbnails() {
    unsigned int failures = 0;
    unsigned int registered = 0;

    for (SwitcherItem& item : g_switcher.items) {
        const bool iconInsteadOfPreview =
            g_switcherStyle == SwitcherStyle::Windows7 && item.minimized;

        const bool wantsThumbnail = !iconInsteadOfPreview &&
            item.card.right > item.card.left && item.hwnd &&
            IsWindow(item.hwnd) && item.thumb.right > item.thumb.left;

        if (!wantsThumbnail) {
            item.ReleaseThumbnail();
            continue;
        }

        if (!item.hasThumbnail) {
            HTHUMBNAIL thumbnail = nullptr;
            if (!g_switcher.previewLayer ||
                FAILED(DwmRegisterThumbnail(g_switcher.previewLayer,
                                            item.hwnd, &thumbnail)) ||
                !thumbnail) {
                continue;
            }
            item.thumbnail = thumbnail;
            item.hasThumbnail = true;
            ++registered;
        }

        DWM_THUMBNAIL_PROPERTIES properties{};
        // 25 = RECTDESTINATION | VISIBLE | SOURCECLIENTAREAONLY, i flag con cui
        // gli originali registravano le anteprime. fSourceClientAreaOnly resta
        // FALSE: la cornice della finestra fa parte dell'anteprima.
        properties.dwFlags = 25;
        properties.fVisible = TRUE;
        properties.opacity = 255;
        // La finestra dei preview è grande quanto il pannello, quindi il
        // rettangolo di destinazione va riportato nelle sue coordinate.
        properties.rcDestination = item.thumb;
        OffsetRect(&properties.rcDestination, -g_switcher.layout.panel.left,
                   -g_switcher.layout.panel.top);
        properties.fSourceClientAreaOnly = FALSE;

        if (FAILED(DwmUpdateThumbnailProperties(item.thumbnail,
                                                &properties))) {
            item.ReleaseThumbnail();
            ++failures;
        }
    }

    if (failures) {
        static RateLimitedLog log{L"some previews couldn't be shown live; they are drawn as application icons instead", 2};
        if (log.ShouldLog()) {
            Wh_Log(L"%u preview(s) couldn't be registered with DWM; they are drawn as application icons instead",
                   failures);
        }
    }

    // La regione dello strato dei preview è l'unione dei rettangoli
    // registrati: le finestre senza anteprima restano fuori e sotto si vede il
    // pannello, con l'icona grande al centro.
    UpdatePreviewLayerRegion();

    if (registered || failures) {
        PaintAndPresent();
    }
}

static HMONITOR PickMonitor(const SwitcherSettings& settings, HWND activeWindow) {
    if (settings.paneMonitor == L"cursor") {
        POINT cursor{};
        if (GetCursorPos(&cursor)) {
            HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
            if (monitor) {
                return monitor;
            }
        }
    } else if (settings.paneMonitor == L"active-window") {
        if (activeWindow) {
            HMONITOR monitor = MonitorFromWindow(activeWindow,
                                                 MONITOR_DEFAULTTONEAREST);
            if (monitor) {
                return monitor;
            }
        }
    }

    POINT origin{0, 0};
    HMONITOR monitor = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    return monitor ? monitor : MonitorFromWindow(nullptr,
                                                 MONITOR_DEFAULTTOPRIMARY);
}

static int GetMonitorDpi(HMONITOR monitor, const MONITORINFO& info) {
    using GetDpiForMonitorFn = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    HMODULE shcore = GetModuleHandleW(L"shcore.dll");
    if (shcore) {
        auto getDpiForMonitor = reinterpret_cast<GetDpiForMonitorFn>(
            GetProcAddress(shcore, "GetDpiForMonitor"));
        if (getDpiForMonitor) {
            UINT dpiX = 96;
            UINT dpiY = 96;
            // MDT_EFFECTIVE_DPI
            if (SUCCEEDED(getDpiForMonitor(monitor, 0, &dpiX, &dpiY)) &&
                dpiX >= 48 && dpiX <= 768) {
                return static_cast<int>(dpiX);
            }
        }
    }

    const int width = info.rcMonitor.right - info.rcMonitor.left;
    const int height = info.rcMonitor.bottom - info.rcMonitor.top;
    return (width >= 2560 || height > 1440) ? 120 : 96;
}

static SessionResult BeginSession(const SwitcherSettings& settings,
                                  bool sameApp,
                                  bool* foregroundFound) {
    *foregroundFound = false;

    const HWND foreground = GetForegroundWindow();

    HMONITOR monitor = PickMonitor(settings, foreground);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        Wh_Log(L"The monitor for the switcher couldn't be resolved");
        return SessionResult::Failed;
    }

    std::wstring appFilter;
    if (sameApp) {
        if (!foreground || !GetWindowApplicationPath(foreground, &appFilter) ||
            appFilter.empty()) {
            return SessionResult::NothingToShow;
        }
    }

    BuildWindowList(settings, sameApp ? appFilter : std::wstring(),
                    &g_switcher.items);
    if (g_switcher.items.size() < 2) {
        g_switcher.items.clear();
        return SessionResult::NothingToShow;
    }

    g_switcher.settings = settings;
    g_switcherStyle = settings.style;
    g_switcher.appPathFilter = sameApp ? appFilter : std::wstring();
    g_switcher.monitorRect = info.rcMonitor;
    g_switcher.workArea = info.rcWork;
    OffsetRect(&g_switcher.workArea, -info.rcMonitor.left, -info.rcMonitor.top);
    g_switcher.dpi = GetMonitorDpi(monitor, info);
    g_switcher.page = 0;
    g_switcher.hoverIndex = -1;
    g_switcher.selectedIndex = -1;

    for (int i = 0; i < static_cast<int>(g_switcher.items.size()); ++i) {
        if (g_switcher.items[i].hwnd == foreground) {
            g_switcher.selectedIndex = i;
            *foregroundFound = true;
            break;
        }
    }

    RecomputeLayoutAndRects();

    const RECT& panel = g_switcher.layout.panel;
    const int panelWidth = panel.right - panel.left;
    const int panelHeight = panel.bottom - panel.top;
    if (panelWidth <= 0 || panelHeight <= 0) {
        g_switcher.items.clear();
        return SessionResult::Failed;
    }

    // Il layout è calcolato in coordinate del monitor: quelle dello schermo
    // servono alla finestra a livelli e a quelle dei preview e delle icone.
    g_switcher.panelScreen = RECT{info.rcMonitor.left + panel.left,
                                 info.rcMonitor.top + panel.top,
                                 info.rcMonitor.left + panel.right,
                                 info.rcMonitor.top + panel.bottom};

    if (!g_switcher.surface.Ensure(panelWidth, panelHeight)) {
        Wh_Log(L"The panel surface for %dx%d couldn't be created", panelWidth,
               panelHeight);
        g_switcher.items.clear();
        return SessionResult::Failed;
    }

    // Il colore arriva dal sistema, una volta per sessione: il vetro del 7 ne è
    // la versione chiara, la banda dell'8.1 la versione profonda. Se il sistema
    // non risponde restano i due colori misurati sui fotogrammi.
    bool usedCustomColor = false;
    const SystemAccent accent =
        ResolvePanelAccent(settings, &usedCustomColor);
    g_switcher.panelTint = accent.valid ? FrostColorFromAccent(accent.color)
                                       : kWin7GlassTint;
    g_switcher.bandColor = accent.valid ? BandColorFromAccent(accent.color)
                                       : kW81PanelColor;
    static bool loggedAccent = false;
    if (!loggedAccent) {
        loggedAccent = true;
        Wh_Log(L"Panel color: %s, accent 0x%06X, glass 0x%06X, band 0x%06X%s",
               usedCustomColor ? L"custom" : L"Windows",
               static_cast<unsigned>(accent.color),
               static_cast<unsigned>(g_switcher.panelTint),
               static_cast<unsigned>(g_switcher.bandColor),
               accent.valid ? L"" : L" (fallback, the system didn't answer)");
    }
    // L'opacità del vetro sta sulla scala piena 0-255: il pannello è un livello
    // premoltiplicato, quindi è l'alfa stessa a decidere quanto desktop resta
    // visibile, non una tinta chiara a bassa opacità.
    g_switcher.panelAlpha = std::clamp(settings.frostDensity, 0, 100) *
                           255 / 100;

    // Il fondale sfocato si cattura adesso, prima che lo scurimento e il
    // pannello compaiano: così la cattura contiene il desktop e non lo switcher.
    if (g_switcherStyle == SwitcherStyle::Windows7) {
        if (!CaptureGlassBackdrop(g_switcher.panelScreen, panelWidth, panelHeight,
                                  static_cast<int>(g_switcherStyle))) {
            static RateLimitedLog log{
                L"the glass backdrop couldn't be captured", 1};
            if (log.ShouldLog()) {
                Wh_Log(L"The desktop behind the panel couldn't be captured; the panel keeps the plain tint");
            }
        }
    }

    // Scurimento del desktop: uno strato sotto il pannello. Il livello è già
    // semitrasparente, quindi il valore richiesto viene diviso per due per
    // restare nella stessa scala dell'opacità del pannello.
    if (settings.desktopShade > 0) {
        const int dimAlpha = std::clamp(settings.desktopShade, 0, 100) *
                             255 / 100 / 2;
        if (!EnsureDimWindow(info.rcMonitor, dimAlpha)) {
            static RateLimitedLog log{L"the dimming layer couldn't be created", 1};
            if (log.ShouldLog()) {
                Wh_Log(L"The layer that dims the desktop couldn't be created");
            }
        }
    } else {
        DestroyDimWindow();
    }

    // L'ombra del vetro: una finestra a livelli sotto il pannello, più grande di
    // lui, perché una finestra a livelli non riceve l'ombra di DWM.
    if (g_switcherStyle == SwitcherStyle::Windows7) {
        const int spread = ScaleForDpi(kWin7ShadowSpread, g_switcher.dpi);
        if (!EnsureShadowWindow(g_switcher.panelScreen, spread,
                                g_switcher.layout.radius, kWin7ShadowAlpha)) {
            static RateLimitedLog log{L"the shadow of the panel couldn't be drawn", 1};
            if (log.ShouldLog()) {
                Wh_Log(L"The shadow of the panel couldn't be drawn");
            }
        } else {
            // Una riga sola, la prima volta: serve a capire dal log se l'ombra
            // esiste davvero quando il vetro sembra incollato al desktop.
            static bool logged = false;
            if (!logged) {
                logged = true;
                Wh_Log(L"Panel shadow: %d px spread, alpha %d", spread,
                       kWin7ShadowAlpha);
            }
        }
    } else {
        DestroyShadowWindow();
    }

    // Il pannello è grande esattamente quanto il rettangolo calcolato dal
    // layout: così la sfocatura riguarda solo il vetro e non tutto lo schermo.
    SetWindowPos(g_switcher.hwnd, HWND_TOPMOST, g_switcher.panelScreen.left,
                 g_switcher.panelScreen.top, panelWidth, panelHeight,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER);

    HRGN shape = CreateRoundRectRgn(0, 0, panelWidth + 1, panelHeight + 1,
                                    std::max(0, g_switcher.layout.radius) * 2,
                                    std::max(0, g_switcher.layout.radius) * 2);
    if (shape) {
        // SetWindowRgn possiede la regione: non va cancellata.
        SetWindowRgn(g_switcher.hwnd, shape, TRUE);
    }

    // Si compone il pannello mentre la finestra è ancora nascosta, così non
    // può comparire un fotogramma vuoto o vecchio.
    PaintFrame();

    g_switcher.active = true;
    g_switcher.visible = true;
    g_switcher.startedTick = GetTickCount64();
    g_switcherVisible.store(true, std::memory_order_release);

    ShowWindow(g_switcher.hwnd, SW_SHOWNOACTIVATE);

    // La sfocatura si chiede a finestra già visibile: alla prima sessione dopo
    // l'avvio della mod l'attributo di composizione non viene accettato su una
    // finestra che non è ancora stata mostrata nemmeno una volta.
    g_blurLevel = g_switcherStyle == SwitcherStyle::Windows7
        ? ApplyPanelBlur(g_switcher.hwnd, panelWidth, panelHeight,
                         g_switcher.layout.radius, settings.systemBlur)
        : BlurLevel::None;
    static int reportedBlur = -1;
    if (static_cast<int>(g_blurLevel) != reportedBlur) {
        reportedBlur = static_cast<int>(g_blurLevel);
        Wh_Log(L"Panel blur: %s",
               g_blurLevel == BlurLevel::Dwm
                   ? L"DwmEnableBlurBehindWindow"
                   : (g_blurLevel == BlurLevel::Accent
                          ? L"SetWindowCompositionAttribute"
                          : L"none, transparency only"));
    }

    if (!EnsurePreviewLayer()) {
        static RateLimitedLog log{L"the preview layer is missing; the switcher shows icons only", 1};
        if (log.ShouldLog()) {
            Wh_Log(L"The layer of the previews couldn't be created; only the icons are shown");
        }
    }

    RefreshThumbnails();
    // I preview di DWM arrivano dopo il primo disegno, quindi il pannello e le
    // icone vengono presentati di nuovo quando le anteprime sono registrate.
    PaintAndPresent();
    StartFade(true);
    ApplySessionTimers();

    if (!g_altDown.load(std::memory_order_relaxed)) {
        PostMessageW(g_switcher.hwnd, kMsgSwitchCommit, 0, 0);
    }

    Wh_Log(L"The switcher is showing %u window(s), page %d of %d",
           static_cast<unsigned>(g_switcher.items.size()), g_switcher.page + 1,
           g_switcher.layout.pageCount);
    return SessionResult::Ok;
}

static void ApplySessionTimers() {
    if (!g_switcher.hwnd || !IsWindow(g_switcher.hwnd)) {
        return;
    }

    KillTimer(g_switcher.hwnd, kAutoCycleTimerId);
    KillTimer(g_switcher.hwnd, kSessionAgeTimerId);
    KillTimer(g_switcher.hwnd, kRebuildTimerId);

    if (!g_switcher.active) {
        return;
    }

    SetTimer(g_switcher.hwnd, kSessionAgeTimerId, kSessionAgeIntervalMs,
             nullptr);
    if (g_switcher.settings.cycleOnHold) {
        SetTimer(g_switcher.hwnd, kAutoCycleTimerId, kAutoCycleIntervalMs,
                 nullptr);
    }
}

static void EndSession() {
    StopFadeTimer();
    HideIconOverlay();
    // La cattura vale per la sessione che l'ha presa: alla successiva il
    // desktop sarà diverso.
    ReleaseGlassBackdrop();

    if (g_switcher.hwnd && IsWindow(g_switcher.hwnd)) {
        KillTimer(g_switcher.hwnd, kAutoCycleTimerId);
        KillTimer(g_switcher.hwnd, kSessionAgeTimerId);
        KillTimer(g_switcher.hwnd, kRebuildTimerId);
        // La sfocatura va tolta prima di nascondere la finestra, altrimenti
        // DWM continuerebbe a comporre la regione dietro un pannello sparito.
        ClearPanelBlur(g_switcher.hwnd);
        ShowWindow(g_switcher.hwnd, SW_HIDE);
    }

    for (SwitcherItem& item : g_switcher.items) {
        item.ReleaseThumbnail();
    }
    g_switcher.items.clear();

    // La superficie del pannello è grande quanto il pannello e le icone
    // decodificate si ripetono: vengono liberate fra due pressioni di Alt+Tab.
    g_switcher.surface.reset();
    ClearIconCache();
    DestroyPreviewLayer();
    DestroyDimWindow();
    DestroyShadowWindow();

    g_switcher.active = false;
    g_switcher.visible = false;
    g_switcher.selectedIndex = -1;
    g_switcher.hoverIndex = -1;
    g_switcher.fadeState = 0;
    g_switcher.fadeLevel = 0;
    g_switcher.pendingClose = false;
    g_switcherVisible.store(false, std::memory_order_release);
}

// La chiusura annullata (Esc) può essere ammorbidita dalla dissolvenza: la
// sessione resta viva finché l'alfa non arriva a zero, e il timer la chiude.
// L'attivazione di una finestra invece chiude subito, perché il pannello non
// deve restare sopra la finestra che ha appena preso il fuoco.
static void CloseWithFade() {
    if (!g_switcher.active || !g_switcher.settings.openFade) {
        EndSession();
        return;
    }
    g_switcher.pendingClose = true;
    StartFade(false);
}


static bool TrySetForeground(HWND hwnd) {
    SetForegroundWindow(hwnd);
    if (GetForegroundWindow() == hwnd) {
        return true;
    }

    BringWindowToTop(hwnd);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    return GetForegroundWindow() == hwnd;
}

static bool AttachAndSetForeground(HWND hwnd) {
    const HWND foreground = GetForegroundWindow();
    const DWORD foregroundThread =
        foreground ? GetWindowThreadProcessId(foreground, nullptr) : 0;
    const DWORD currentThread = GetCurrentThreadId();

    bool attached = false;
    if (foregroundThread && foregroundThread != currentThread) {
        attached = AttachThreadInput(currentThread, foregroundThread, TRUE) !=
            FALSE;
    }

    const bool activated = TrySetForeground(hwnd);

    if (attached) {
        AttachThreadInput(currentThread, foregroundThread, FALSE);
    }
    return activated;
}

constexpr WORD kMenuMaskVk = 0xE8;  // unassigned virtual key

static void InjectNeutralKey() {
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_MENU;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = kMenuMaskVk;
    inputs[2] = inputs[1];
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_MENU;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

static void SendMenuMaskKey() {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = kMenuMaskVk;
    inputs[1] = inputs[0];
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    if (!SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT))) {
        Wh_Log(L"The menu mask key couldn't be sent (error %lu)",
               GetLastError());
    }
}

static bool ActivateWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }

    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }

    if (GetForegroundWindow() == hwnd) {
        return true;
    }

    if (TrySetForeground(hwnd)) {
        return true;
    }

    if (AttachAndSetForeground(hwnd)) {
        return true;
    }

    InjectNeutralKey();
    if (TrySetForeground(hwnd)) {
        return true;
    }

    SwitchToThisWindow(hwnd, TRUE);
    if (GetForegroundWindow() == hwnd) {
        return true;
    }

    Wh_Log(L"The selected window couldn't be activated; the window is still the one that was used before");
    return false;
}

// -----------------------------------------------------------------------------
// Committing, cancelling, rebuilding
// -----------------------------------------------------------------------------

static void CommitSession() {
    if (!g_switcher.active) {
        return;
    }

    HWND target = nullptr;
    const HWND foreground = GetForegroundWindow();
    if (g_switcher.selectedIndex >= 0 &&
        g_switcher.selectedIndex < static_cast<int>(g_switcher.items.size())) {
        target = g_switcher.items[g_switcher.selectedIndex].hwnd;
    }

    // La scheda del desktop non è una finestra da attivare: si mostra il
    // desktop con la stessa chiamata del pulsante "Mostra desktop".
    const bool desktopSelected =
        g_switcher.selectedIndex >= 0 &&
        g_switcher.selectedIndex < static_cast<int>(g_switcher.items.size()) &&
        g_switcher.items[g_switcher.selectedIndex].isDesktop;

    EndSession();

    if (desktopSelected) {
        if (!ShowDesktopWithShell()) {
            Wh_Log(L"The desktop couldn't be shown");
        }
        return;
    }

    if (!target || target == foreground || !IsWindow(target)) {
        return;
    }

    ActivateWindow(target);
}

static void CloseSelectedWindow() {
    if (!g_switcher.active || g_switcher.selectedIndex < 0 ||
        g_switcher.selectedIndex >=
            static_cast<int>(g_switcher.items.size())) {
        return;
    }

    // Il desktop non è una finestra da chiudere.
    if (g_switcher.items[g_switcher.selectedIndex].isDesktop) {
        return;
    }

    const HWND target = g_switcher.items[g_switcher.selectedIndex].hwnd;
    if (target && IsWindow(target)) {
        PostMessageW(target, WM_SYSCOMMAND, SC_CLOSE, 0);
    }

    SetTimer(g_switcher.hwnd, kRebuildTimerId, kRebuildDelayMs, nullptr);
}

static void RebuildSessionItems() {
    if (!g_switcher.active) {
        return;
    }

    const HWND keep = (g_switcher.selectedIndex >= 0 &&
                       g_switcher.selectedIndex <
                           static_cast<int>(g_switcher.items.size()))
        ? g_switcher.items[g_switcher.selectedIndex].hwnd
        : nullptr;
    const int previousIndex = g_switcher.selectedIndex;
    const SwitcherSettings settings = g_switcher.settings;
    const std::wstring appFilter = g_switcher.appPathFilter;

    std::vector<SwitcherItem> items;
    BuildWindowList(settings, appFilter, &items);

    for (SwitcherItem& item : g_switcher.items) {
        item.ReleaseThumbnail();
    }
    g_switcher.items = std::move(items);

    if (g_switcher.items.empty()) {
        EndSession();
        return;
    }

    int index = -1;
    for (int i = 0; i < static_cast<int>(g_switcher.items.size()); ++i) {
        if (g_switcher.items[i].hwnd == keep) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        index = std::clamp(previousIndex, 0,
                           static_cast<int>(g_switcher.items.size()) - 1);
    }

    g_switcher.selectedIndex = index;
    g_switcher.hoverIndex = -1;
    RecomputeLayoutAndRects();
    PaintFrame();
    RefreshThumbnails();
    PresentFrame();
}


static void OnShowRequest(bool previous) {
    if (!g_switcher.active) {
        const SwitcherSettings settings = GetSettingsCopy();
        bool foregroundFound = false;
        const SessionResult result =
            BeginSession(settings, false, &foregroundFound);

        if (result == SessionResult::NothingToShow ||
            result == SessionResult::Ok) {
            SendMenuMaskKey();
            if (result == SessionResult::Ok) {
                if (foregroundFound) {
                    MoveSelection(previous ? -1 : 1);
                } else {
                    SelectIndex(0);
                }
            }
            return;
        }

        EndSession();
        SendAltTabToSystem();
        return;
    }

    MoveSelection(previous ? -1 : 1);
}

static void OnSameAppRequest() {
    if (g_switcher.active) {
        EndSession();
        return;
    }

    const SwitcherSettings settings = GetSettingsCopy();
    bool foregroundFound = false;
    const SessionResult result =
        BeginSession(settings, true, &foregroundFound);
    SendMenuMaskKey();
    if (result != SessionResult::Ok) {
        EndSession();
        return;
    }

    if (foregroundFound) {
        MoveSelection(1);
    } else {
        SelectIndex(0);
    }
}

static void OnFallbackHotkey() {
    if (g_switcher.active) {
        EndSession();
        return;
    }

    OnShowRequest(false);
}

static void OnSessionHealthCheck() {
    if (!g_switcher.visible) {
        return;
    }

    if (!g_altDown.load(std::memory_order_relaxed)) {
        CommitSession();
        return;
    }

    if (GetTickCount64() - g_switcher.startedTick > kSessionStuckLimitMs) {
        Wh_Log(L"The switcher has been open for too long with Alt held, and the keyboard hook has been silent for %llu ms; closing the switcher and reinstalling the hook",
               GetTickCount64() - g_hookCallTick.load());
        g_hookReinstallRequested.store(true, std::memory_order_release);
        EndSession();
    }
}

static void OnSettingsChanged() {
    const SwitcherSettings settings = GetSettingsCopy();
    UpdateHookFlags(settings);
    if (g_switcher.active) {
        EndSession();
    }
    Wh_Log(L"SwitcherSettings applied");
}

// -----------------------------------------------------------------------------
// Mouse handling
// -----------------------------------------------------------------------------

static void OnMouseMove(int x, int y) {
    if (!g_switcher.visible || !g_switcher.settings.pointerSelect) {
        return;
    }

    // Le coordinate del messaggio sono relative alla finestra del
    // pannello, il layout è in coordinate del monitor.
    const int hovered = IndexAtPoint(x + g_switcher.layout.panel.left,
                                     y + g_switcher.layout.panel.top);
    if (hovered == g_switcher.hoverIndex) {
        return;
    }

    g_switcher.hoverIndex = hovered;
    PaintAndPresent();
}

static void OnMouseClick(int x, int y) {
    if (!g_switcher.visible || !g_switcher.settings.pointerSelect) {
        return;
    }

    const int index = IndexAtPoint(x + g_switcher.layout.panel.left,
                                   y + g_switcher.layout.panel.top);
    if (index < 0) {
        return;
    }

    SelectIndex(index);
    CommitSession();
}

static void OnMouseWheel(int delta) {
    if (!g_switcher.visible || !g_switcher.settings.pointerSelect ||
        delta == 0) {
        return;
    }

    MoveSelection(delta > 0 ? -1 : 1);
}


static bool HandleOverlayMessage(HWND hwnd, UINT message, WPARAM wParam,
                                 LPARAM lParam, LRESULT* outResult) {
    switch (message) {
        case WM_ERASEBKGND:
            *outResult = 1;
            return true;

        case WM_PAINT: {
            // La finestra è a livelli: tutto quello che mostra è già stato
            // consegnato a DWM da UpdateLayeredWindow, quindi qui non c'è
            // niente da dipingere, ma il messaggio va comunque validato.
            PAINTSTRUCT paint{};
            BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
            *outResult = 0;
            return true;
        }

        case WM_MOUSEACTIVATE:
            *outResult = MA_NOACTIVATE;
            return true;

        case WM_MOUSEMOVE:
            OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            *outResult = 0;
            return true;

        case WM_LBUTTONDOWN:
            OnMouseClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            *outResult = 0;
            return true;

        case WM_MOUSEWHEEL:
            OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
            *outResult = 0;
            return true;

        case WM_TIMER:
            if (wParam == kAutoCycleTimerId) {
                if (g_altDown.load(std::memory_order_relaxed)) {
                    MoveSelection(1);
                } else {
                    CommitSession();
                }
            } else if (wParam == kSessionAgeTimerId) {
                OnSessionHealthCheck();
            } else if (wParam == kRebuildTimerId) {
                KillTimer(hwnd, kRebuildTimerId);
                RebuildSessionItems();
            } else if (wParam == kFadeTimerId) {
                if (StepFade() && g_switcher.pendingClose) {
                    g_switcher.pendingClose = false;
                    EndSession();
                }
            }
            *outResult = 0;
            return true;

        case kMsgSwitchShow:
            OnShowRequest(wParam != 0);
            *outResult = 0;
            return true;

        case kMsgSwitchCommit:
            CommitSession();
            *outResult = 0;
            return true;

        case kMsgSwitchCancel:
        case kMsgSwitchClose:
            CloseWithFade();
            *outResult = 0;
            return true;

        case kMsgSwitchCloseSelected:
            CloseSelectedWindow();
            *outResult = 0;
            return true;

        case kMsgSameApp:
            OnSameAppRequest();
            *outResult = 0;
            return true;

        case kMsgFallbackToggle:
            OnFallbackHotkey();
            *outResult = 0;
            return true;

        case kMsgSettingsChanged:
            OnSettingsChanged();
            *outResult = 0;
            return true;

        case WM_DISPLAYCHANGE:
            EndSession();
            *outResult = 0;
            return true;

        case WM_DWMCOLORIZATIONCOLORCHANGED: {
            // Il colore di personalizzazione è cambiato mentre lo switcher è
            // aperto: il vetro e la banda lo seguono subito, come facevano i due
            // originali. Il messaggio è documentato da Microsoft insieme a
            // DwmGetColorizationColor.
            const SystemAccent accent =
                ResolvePanelAccent(g_switcher.settings, nullptr);
            if (accent.valid) {
                g_switcher.panelTint = FrostColorFromAccent(accent.color);
                g_switcher.bandColor = BandColorFromAccent(accent.color);
                PaintAndPresent();
            }
            *outResult = 0;
            return true;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            *outResult = 0;
            return true;

        case WM_DESTROY:
            g_overlayWindow.store(nullptr, std::memory_order_release);
            DestroyIconOverlay();
            PostQuitMessage(0);
            *outResult = 0;
            return true;

        default:
            break;
    }

    return false;
}

static bool HandleOverlayMessageGuarded(HWND hwnd, UINT message, WPARAM wParam,
                                        LPARAM lParam, bool* handled,
                                        LRESULT* result) {
#if WH_HAS_SEH
    __try {
        *handled = HandleOverlayMessage(hwnd, message, wParam, lParam, result);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        const unsigned count = g_exceptionCount.fetch_add(1) + 1;
        if (count <= 5) {
            Wh_Log(L"A structured exception escaped while handling an overlay window message");
        }
        return false;
    }
#else
    return RunGuarded(L"handling an overlay window message", [&] {
        *handled = HandleOverlayMessage(hwnd, message, wParam, lParam, result);
    });
#endif
}

static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT message, WPARAM wParam,
                                       LPARAM lParam) {
    LRESULT result = 0;
    bool handled = false;

    const bool ok = HandleOverlayMessageGuarded(hwnd, message, wParam, lParam,
                                               &handled, &result);

    if (!ok) {
        RunGuarded(L"closing the switcher after a failure", [] {
            EndSession();
        });
        return 0;
    }

    if (handled) {
        return result;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static bool CreateOverlayWindow() {
    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    // CS_DBLCLKS: i doppi clic arrivano alla finestra invece di essere
    // trasformati in due clic singoli dal sistema.
    windowClass.style = CS_DBLCLKS;
    windowClass.lpfnWndProc = OverlayWndProc;
    windowClass.hInstance = instance;
    windowClass.hCursor =
        LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_ARROW));
    windowClass.lpszClassName = kOverlayClassName;

    if (!RegisterClassExW(&windowClass)) {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"The overlay window class couldn't be registered (error %lu)",
                   GetLastError());
            return false;
        }
    }

    // WS_EX_LAYERED: il contenuto del pannello arriva da
    // UpdateLayeredWindow, quindi il livello ha un alfa per pixel.
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE,
        kOverlayClassName, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
        instance, nullptr);

    if (!hwnd) {
        Wh_Log(L"The overlay window couldn't be created (error %lu)",
               GetLastError());
        return false;
    }

    g_switcher.hwnd = hwnd;
    g_overlayWindow.store(hwnd, std::memory_order_release);
    return true;
}


static void PostToOverlay(UINT message, WPARAM wParam) {
    HWND hwnd = g_overlayWindow.load(std::memory_order_acquire);
    if (hwnd) {
        PostMessageW(hwnd, message, wParam, 0);
    }
}

static void UpdateHookFlags(const SwitcherSettings& settings) {
    g_onlyThisAppHotkeyEnabled.store(settings.onlyThisAppHotkey,
                                 std::memory_order_relaxed);
    g_manualHotkeyEnabled.store(settings.manualHotkey,
                                  std::memory_order_relaxed);
}

static void SendAltTabToSystem() {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_TAB;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_TAB;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    if (!SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT))) {
        Wh_Log(L"The Alt+Tab press for the stock switcher couldn't be sent (error %lu)",
               GetLastError());
    }
}

static bool HandleKeyEvent(DWORD vk, bool isDown, bool isUp) {
    if (!isDown && !isUp) {
        return false;
    }

    if (vk == VK_LMENU || vk == VK_RMENU || vk == VK_MENU) {
        if (isDown) {
            g_altDown.store(true, std::memory_order_relaxed);
        } else {
            g_altDown.store(false, std::memory_order_relaxed);
            if (g_switcherVisible.load(std::memory_order_acquire)) {
                PostToOverlay(kMsgSwitchCommit, 0);
            }
        }
        return false;
    }

    if (vk == VK_LCONTROL || vk == VK_RCONTROL || vk == VK_CONTROL) {
        g_ctrlDown.store(isDown, std::memory_order_relaxed);
        return false;
    }

    if (vk == VK_LSHIFT || vk == VK_RSHIFT || vk == VK_SHIFT) {
        g_shiftDown.store(isDown, std::memory_order_relaxed);
        return false;
    }

    if (vk == VK_LWIN || vk == VK_RWIN) {
        if (isDown) {
            g_winDown.store(true, std::memory_order_relaxed);
            if (g_switcherVisible.load(std::memory_order_acquire)) {
                g_forcePassthroughVk.store(vk, std::memory_order_relaxed);
                PostToOverlay(kMsgSwitchClose, 0);
            }
        } else if (isUp) {
            g_winDown.store(false, std::memory_order_relaxed);
            if (g_forcePassthroughVk.load(std::memory_order_relaxed) == vk) {
                g_forcePassthroughVk.store(0, std::memory_order_relaxed);
            }
        }
        return false;
    }

    const DWORD passthroughVk =
        g_forcePassthroughVk.load(std::memory_order_relaxed);
    if (passthroughVk) {
        if (vk == passthroughVk && isUp) {
            g_forcePassthroughVk.store(0, std::memory_order_relaxed);
        }
        return false;
    }

    const bool alt = g_altDown.load(std::memory_order_relaxed);
    const bool ctrl = g_ctrlDown.load(std::memory_order_relaxed);
    const bool shift = g_shiftDown.load(std::memory_order_relaxed);
    const bool win = g_winDown.load(std::memory_order_relaxed);

    if (g_switcherVisible.load(std::memory_order_acquire)) {
        if (ctrl && shift && vk == VK_ESCAPE) {
            g_forcePassthroughVk.store(vk, std::memory_order_relaxed);
            PostToOverlay(kMsgSwitchClose, 0);
            return false;
        }

        switch (vk) {
            case VK_TAB:
                if (isDown) {
                    PostToOverlay(kMsgSwitchShow, shift ? 1 : 0);
                }
                return true;

            case VK_LEFT:
            case VK_UP:
                if (isDown) {
                    PostToOverlay(kMsgSwitchShow, 1);
                }
                return true;

            case VK_RIGHT:
            case VK_DOWN:
                if (isDown) {
                    PostToOverlay(kMsgSwitchShow, 0);
                }
                return true;

            case VK_RETURN:
            case VK_SPACE:
                if (isDown) {
                    PostToOverlay(kMsgSwitchCommit, 0);
                }
                return true;

            case VK_ESCAPE:
                if (isDown) {
                    PostToOverlay(kMsgSwitchCancel, 0);
                }
                return true;

            case VK_F4:
                if (isDown && alt) {
                    PostToOverlay(kMsgSwitchCloseSelected, 0);
                }
                return true;

            default:
                break;
        }

        return true;
    }

    if (vk == VK_TAB) {
        if (ctrl || win) {
            return false;  // Ctrl+Alt+Tab and Win+Tab belong to Windows
        }

        if (alt && isDown) {
            PostToOverlay(kMsgSwitchShow, shift ? 1 : 0);
            return true;
        }

        if (alt && isUp) {
            return true;
        }

        return false;
    }

    if (vk == VK_OEM_3 && alt && isDown &&
        g_onlyThisAppHotkeyEnabled.load(std::memory_order_relaxed)) {
        PostToOverlay(kMsgSameApp, 0);
        return true;
    }

    if (vk == static_cast<DWORD>(kFallbackHotkeyVk) && ctrl && alt && isDown &&
        g_manualHotkeyEnabled.load(std::memory_order_relaxed)) {
        PostToOverlay(kMsgFallbackToggle, 0);
        return true;
    }

    return false;
}

static bool HandleKeyEventGuarded(DWORD vk, bool isDown, bool isUp) {
#if WH_HAS_SEH
    __try {
        return HandleKeyEvent(vk, isDown, isUp);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        const unsigned count = g_exceptionCount.fetch_add(1) + 1;
        if (count <= 5) {
            Wh_Log(L"A structured exception escaped while handling a key event");
        }
        return false;
    }
#else
    try {
        return HandleKeyEvent(vk, isDown, isUp);
    } catch (...) {
        const unsigned count = g_exceptionCount.fetch_add(1) + 1;
        if (count <= 5) {
            Wh_Log(L"A C++ exception escaped while handling a key event");
        }
        return false;
    }
#endif
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam,
                                             LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    const auto* info = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
    g_hookCallTick.store(GetTickCount64(), std::memory_order_relaxed);

    if (!info || (info->flags & LLKHF_INJECTED) ||
        g_shuttingDown.load(std::memory_order_relaxed)) {
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    const bool isDown =
        wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    const bool isUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

    if (HandleKeyEventGuarded(info->vkCode, isDown, isUp)) {
        return 1;
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

static HMODULE g_modModule = nullptr;

static bool InstallKeyboardHook() {
    if (!g_modModule) {
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&InstallKeyboardHook),
                           &g_modModule);
    }

    g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                                       g_modModule, 0);
    if (!g_keyboardHook) {
        Wh_Log(L"The keyboard hook couldn't be installed (error %lu)",
               GetLastError());
        return false;
    }

    g_altDown.store((GetAsyncKeyState(VK_MENU) & 0x8000) != 0,
                    std::memory_order_relaxed);
    g_ctrlDown.store((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0,
                     std::memory_order_relaxed);
    g_shiftDown.store((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0,
                      std::memory_order_relaxed);
    g_winDown.store(((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) ||
                        ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0),
                    std::memory_order_relaxed);

    g_hookCallTick.store(GetTickCount64(), std::memory_order_relaxed);
    return true;
}

static void UninstallKeyboardHook() {
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
}

// -----------------------------------------------------------------------------
// Threads
// -----------------------------------------------------------------------------

static DWORD WINAPI InputThreadProc(void* /*parameter*/) {
    g_inputThreadId = GetCurrentThreadId();

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    if (!InstallKeyboardHook()) {
        g_initFailed.store(true, std::memory_order_release);
    }
    SetEvent(g_hookReadyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == kMsgInputThreadQuit) {
            break;
        }

        if (msg.message == kMsgInputThreadRehook) {
            Wh_Log(L"Reinstalling the keyboard hook");
            UninstallKeyboardHook();
            if (!InstallKeyboardHook()) {
                Wh_Log(L"The keyboard hook couldn't be reinstalled; until the mod is restarted, Alt+Tab belongs to the stock switcher");
            }
        }
    }

    UninstallKeyboardHook();
    return 0;
}

static DWORD WINAPI WatchdogThreadProc(void* /*parameter*/) {
    for (;;) {
        if (WaitForSingleObject(g_stopEvent, kWatchdogIntervalMs) ==
            WAIT_OBJECT_0) {
            break;
        }
        if (g_shuttingDown.load(std::memory_order_relaxed)) {
            break;
        }

        HWND overlay = g_overlayWindow.load(std::memory_order_acquire);
        const bool alive = RunGuarded(L"checking the overlay window", [&] {
            if (overlay && !IsWindow(overlay)) {
                overlay = nullptr;
            }
        });
        if (!alive) {
            continue;
        }

        if (!overlay || !IsWindow(overlay)) {
            Wh_Log(L"The overlay window is gone; the mod is stopping so that Alt+Tab goes back to the stock switcher");
            g_switcherVisible.store(false, std::memory_order_release);
            ExitProcess(0);
        }

        if (g_hookReinstallRequested.exchange(false,
                                              std::memory_order_acq_rel) &&
            g_inputThreadId) {
            PostThreadMessageW(g_inputThreadId, kMsgInputThreadRehook, 0, 0);
        }
    }
    return 0;
}

static void MakeProcessDpiAware() {
    using SetProcessDpiAwarenessContextFn = BOOL(WINAPI*)(HANDLE);

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        auto setContext = reinterpret_cast<SetProcessDpiAwarenessContextFn>(
            GetProcAddress(user32, "SetProcessDpiAwarenessContext"));
        // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        if (setContext && setContext(reinterpret_cast<HANDLE>(-4))) {
            return;
        }
    }

    SetProcessDPIAware();
}

static bool InitializeVirtualDesktopManager() {
    const HRESULT result = CoCreateInstance(
        kCLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
        kIID_IVirtualDesktopManager,
        reinterpret_cast<LPVOID*>(&g_virtualDesktopManager));

    if (FAILED(result) || !g_virtualDesktopManager) {
        g_virtualDesktopManager = nullptr;
        Wh_Log(L"The virtual desktop service isn't available; windows of all virtual desktops are listed");
        return false;
    }
    return true;
}

static DWORD WINAPI UiThreadProc(void* /*parameter*/) {
    MakeProcessDpiAware();

    ScopedComApartment com(
        SUCCEEDED(CoInitializeEx(nullptr,
                                 COINIT_APARTMENTTHREADED |
                                     COINIT_DISABLE_OLE1DDE)));
    if (!com) {
        Wh_Log(L"COM couldn't be initialized; the virtual desktop filter isn't available");
    }

    InitializeVirtualDesktopManager();
    ResolveTitleApi();

    if (!CreateOverlayWindow()) {
        g_initFailed.store(true, std::memory_order_release);
        SetEvent(g_uiReadyEvent);
        return 1;
    }

    g_inputThread = CreateThread(nullptr, 0, InputThreadProc, nullptr, 0,
                                 nullptr);
    if (!g_inputThread ||
        WaitForSingleObject(g_hookReadyEvent, 5000) != WAIT_OBJECT_0 ||
        g_initFailed.load(std::memory_order_acquire)) {
        Wh_Log(L"The keyboard hook thread couldn't be started");
        g_initFailed.store(true, std::memory_order_release);
        DestroyWindow(g_switcher.hwnd);
        g_switcher.hwnd = nullptr;
        SetEvent(g_uiReadyEvent);
        return 1;
    }

    g_watchdogThread = CreateThread(nullptr, 0, WatchdogThreadProc, nullptr, 0,
                                    nullptr);
    if (!g_watchdogThread) {
        Wh_Log(L"The watchdog thread couldn't be started");
    }

    SetEvent(g_uiReadyEvent);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        if (!RunGuarded(L"dispatching a message", [&] {
                DispatchMessageW(&msg);
            })) {
            EndSession();
        }
    }

    EndSession();

    SetEvent(g_stopEvent);

    if (g_inputThreadId) {
        PostThreadMessageW(g_inputThreadId, kMsgInputThreadQuit, 0, 0);
    }
    if (g_inputThread) {
        WaitForSingleObject(g_inputThread, 3000);
        CloseHandle(g_inputThread);
        g_inputThread = nullptr;
    }
    if (g_watchdogThread) {
        WaitForSingleObject(g_watchdogThread, 3000);
        CloseHandle(g_watchdogThread);
        g_watchdogThread = nullptr;
    }

    // I font e le icone decodificate appartengono al thread
    // dell'interfaccia: si liberano qui, che è l'ultimo punto in cui il
    // thread è ancora vivo.
    g_uiFont.reset();
    ClearIconCache();

    if (g_virtualDesktopManager) {
        g_virtualDesktopManager->Release();
        g_virtualDesktopManager = nullptr;
    }

    return 0;
}

static void SignalStop() {
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
}

// -----------------------------------------------------------------------------
// Mod lifetime
// -----------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"Windows 10 Alt+Tab Recreation is starting");

    if (!RunGuarded(L"loading the settings", [] {
            LoadSettings();
            UpdateHookFlags(GetSettingsCopy());
        })) {
        return FALSE;
    }

    g_uiReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hookReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_uiReadyEvent || !g_hookReadyEvent || !g_stopEvent) {
        Wh_Log(L"The mod's events couldn't be created");
        return FALSE;
    }

    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (!g_uiThread) {
        Wh_Log(L"The mod's UI thread couldn't be started");
        return FALSE;
    }

    if (WaitForSingleObject(g_uiReadyEvent, 15000) != WAIT_OBJECT_0 ||
        g_initFailed.load(std::memory_order_acquire)) {
        Wh_Log(L"The switcher couldn't be started; the mod isn't activated and Alt+Tab is left as it is");
        g_shuttingDown.store(true, std::memory_order_release);
        SignalStop();
        WaitForSingleObject(g_uiThread, 3000);
        return FALSE;
    }

    Wh_Log(L"Ready: Alt+Tab is handled by the Windows 10 switcher recreation");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    RunGuarded(L"applying new settings", [] {
        LoadSettings();
        UpdateHookFlags(GetSettingsCopy());
        PostToOverlay(kMsgSettingsChanged, 0);
    });
}

void WhTool_ModUninit() {
    Wh_Log(L"Stopping");

    g_shuttingDown.store(true, std::memory_order_release);
    g_switcherVisible.store(false, std::memory_order_release);
    SignalStop();

    HWND overlay = g_overlayWindow.load(std::memory_order_acquire);
    if (overlay) {
        PostMessageW(overlay, WM_CLOSE, 0, 0);
    }

    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 5000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }

    if (g_uiReadyEvent) {
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
    }
    if (g_hookReadyEvent) {
        CloseHandle(g_hookReadyEvent);
        g_hookReadyEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
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
