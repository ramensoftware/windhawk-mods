// ==WindhawkMod==
// @id              windows-10-alt-tab-recreation
// @name            Windows 10 Alt+Tab Recreation
// @description     This mod recreates the Windows 10 Alt+Tab window switcher on Windows 11 without modifying explorer.exe
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         GPL-2.0
// @include         windhawk.exe
// @compilerOptions -ldwmapi -luser32 -lgdi32 -lmsimg32 -lshell32 -lole32
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Windows 10 Alt+Tab Recreation

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/alttab.png)

This mod recreates the Windows 10 Alt+Tab window switcher on Windows 10 and
Windows 11. Tested on Windows 11 24H2; if it doesn't work on your build, please
open an issue so that it can be improved.

## What it does

- Brings back the Windows 10 switcher: a dark, windowed grid of live window
  previews with the application icon and the title above each preview.
- Keeps the Windows 10 semantics: a 4x4 page, a second page for the remaining
  windows, and a white border around the selected window only.
- Shows live previews through the DWM thumbnail API, minimized windows
  included.
- Never touches `explorer.exe`: it runs in Windhawk's own tool process, hooks
  nothing inside the shell, and doesn't read or write the registry.
- Never asks to restart Explorer: enabling, disabling or updating the mod takes
  effect immediately, and Alt+Tab goes back to the stock switcher as soon as
  the mod is disabled.
- If the switcher can't be shown, the key press is handed back to the system,
  so Alt+Tab never ends up doing nothing.

## Requirements

- Windows 10 or Windows 11, 64-bit, with DWM enabled (the default).
- Windhawk installed, with `windhawk.exe` not excluded in the Windhawk
  settings.

## How to use

1. Install the mod in Windhawk and enable it.
2. Press Alt+Tab. The Windows 10 style switcher appears.
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
| `Alt+`` | Optional, off by default: windows of the same app only |
| `Ctrl+Alt+Tab`, `Win+Tab` | Left to Windows |
| `Ctrl+Shift+Esc` | Left to Windows, opens Task Manager |

## Settings

The switcher is always dark, like the original one was: switching Windows to
the light theme never turned the Alt+Tab switcher light either, so there is
nothing to configure about its appearance.

- **Show the switcher on**: primary monitor, monitor with the cursor, or monitor
  of the active window.
- **Row height**: the height of a row at 100% scaling, 230 px by default. Rows
  shrink automatically when there are many windows.
- **Columns** and **Rows**: the size of the grid; Windows 10 uses 4x4, and
  further windows go on the next page.
- **Switcher background opacity**: 85% by default, the dark panel of the
  original switcher, through which the desktop is still visible; 0% leaves the
  desktop untouched.
- **Desktop dimming**: how much the desktop is darkened; 0% by default.
- **Only windows of the current virtual desktop**: the filter of the original.
  Minimized windows are always listed, with a live preview.
- **Auto-cycle while Alt is held**, **Mouse navigation**, **Alt+`**,
  **Ctrl+Alt+F11**: the optional extras.

## How it works

The mod is a Windhawk tool mod: it runs in a dedicated process of its own and
never inside the shell, which is what makes it safe to enable and disable at
any time.

- A low-level keyboard hook, installed in the mod's own process, takes over
  Alt+Tab. It only classifies key events and posts them to the mod's UI thread,
  and the modifier keys always pass through.
- The window list is built the way the shell builds it: top-level, visible
  windows with a title, in z-order, skipping cloaked windows and tool windows.
- The previews are live DWM thumbnails of the source windows; the grid itself
  is drawn with GDI over a snapshot of the desktop.
- A watchdog re-installs the keyboard hook if Windows drops it, and a session
  whose key release was lost closes itself.

## Notes

- It is recommended to not use this mod together with another Alt+Tab replacement: enable one at a
  time.
- The desktop behind the switcher is a still image; the window previews are
  live.
- Windows 11 tabbed File Explorer windows appear as one item per window, since
  Windows 10 had no tabs.
- True fullscreen exclusive games may not show the switcher, like other overlay
  mods.
- A window whose preview DWM refuses to register is shown with its application
  icon, so no window ever disappears from the switcher.
- The mod is released under the GPL-2.0 license.

## Credits

- The look and the measurements follow the "Horizontal squared" layout of the
  **Simple Window Switcher** mod by Lone, a port of ExplorerPatcher's Simple
  Window Switcher by valinet: the squared translucent panel, the icon and title
  above the preview, the 20 px margin, the 2 px selection border, the 7 px
  paddings, the 230 px row height with its shrink steps, previews at most twice
  as wide as they are tall, and the switcher limited to 80% of the monitor.
- The behavior recreates the Windows 10 switcher, which ExplorerPatcher
  (https://github.com/valinet/ExplorerPatcher, GPL-2.0) restores by reusing the
  components inside `explorer.exe`. This mod is an independent implementation
  that draws the switcher in a separate process instead.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- switcherMonitor: primary
  $name: Show the switcher on
  $description: This setting controls which monitor the switcher appears on. Windows 10 shows it on the primary monitor by default.
  $options:
  - primary: The primary monitor (the Windows 10 default)
  - cursor: The monitor with the mouse cursor
  - active-window: The monitor of the active window
- rowHeight: 230
  $name: Row height (px)
  $description: This setting controls the height of a window row at 100% scaling, before the automatic shrink for many windows. 230 is the value ExplorerPatcher's switcher uses.
- maxColumns: 4
  $name: Columns
  $description: This setting controls how many windows are placed in one row at most. Windows 10 uses 4.
- maxRows: 4
  $name: Rows
  $description: This setting controls how many rows are placed on one page at most. Windows 10 uses 4. Further windows are shown on the next page, like the original switcher.
- switcherBackground: 85
  $name: Switcher background opacity (%)
  $description: This setting controls the opacity of the panel behind the switcher. The default is the value that matches the dark panel of the Windows 10 switcher, which is still slightly translucent, since the original darkens the desktop with a tint instead of an opaque layer; 100 makes it opaque, 0 leaves the desktop untouched.
- backgroundDimming: 0
  $name: Desktop dimming (%)
  $description: This setting controls how much the whole desktop behind the switcher is darkened. 0 is the Windows 10 default, no dimming at all.
- currentDesktopOnly: true
  $name: Only windows of the current virtual desktop
  $description: This setting, like the original switcher, lists only the windows of the virtual desktop that is currently shown by default.
- autoCycleWhileAltHeld: false
  $name: Auto-cycle while Alt is held
  $description: This setting automatically advances the selection every 250 ms while Alt is held, in addition to Tab and Shift+Tab. Off by default.
- mouseNavigation: true
  $name: Mouse navigation
  $description: This setting allows moving the selection with the mouse wheel and activating a window by clicking its preview.
- sameAppHotkey: false
  $name: Alt+` switches between windows of the same app
  $description: This setting adds an extra shortcut that opens the switcher with only the windows of the application of the active window. Off by default.
- fallbackHotkey: true
  $name: Ctrl+Alt+F11 opens the switcher
  $description: This setting adds a fallback hotkey that opens or closes the switcher manually, useful when another program interferes with Alt+Tab.
*/
// ==/WindhawkModSettings==
// -----------------------------------------------------------------------------
// Design notes
//
// This mod is a recreation, not a recovery: instead of reusing the Windows 10
// components inside explorer.exe, it draws the switcher itself, in the
// dedicated process that Windhawk starts for tool mods. That is what makes the
// "no Explorer restart" property possible:
//
//  1. Nothing is injected into the shell and nothing inside the shell is
//     hooked, so a bug in the mod can't take down explorer.exe, and enabling
//     or disabling the mod can't leave the shell in a state that needs a
//     restart.
//  2. The switcher is a normal topmost window of the mod's own process, and
//     the previews are DWM thumbnails registered between that window and the
//     source windows. All of the rendering APIs used are documented and
//     available on Windows 10.
//  3. Alt+Tab is taken over with a low-level keyboard hook that lives in the
//     mod's process. The hook only classifies key events; the actual work
//     happens on the mod's UI thread, which keeps the hook callback short and
//     avoids the hook being dropped for being slow.
//  4. The grid follows the semantics of the original Windows 10 switcher
//     (AltTabViewHost): a 4x4 page with a second page for the remaining
//     windows, a focus border around the selection, an optional desktop
//     dimming layer and an optional grid background.
//  5. Everything is defensive: if the switcher can't be shown, the mod
//     re-sends Alt+Tab so the stock switcher handles it, if the hook stops
//     being called it is reinstalled by a watchdog, and if the switcher is
//     left open without Alt being held (a missed key event) it commits by
//     itself.
//
// Why this mod exists next to the Simple Window Switcher mod
//
// The Simple Window Switcher mod (Lone, a port of ExplorerPatcher's Simple
// Window Switcher) only partially recreates the Windows 10 switcher: it is a
// generic, configurable window switcher whose layout happens to be close to
// the Windows 10 one, and it is not presented as a Windows 10 recreation. This
// mod has a single, narrower goal: to be the Windows 10 Alt+Tab, with the
// Windows 10 semantics (4x4 page, second page, selection border, 85% dark
// panel, no desktop dimming) as the defaults, and nothing else.
//
// That difference matters to the people the mod is for. An average user who
// wants the Windows 10 switcher back does not know that a mod called "Simple
// Window Switcher" can give it to them, and has no reason to look for it under
// that name; a mod called "Windows 10 Alt+Tab Recreation" says what it does
// at first sight, in the catalog and in the search results. The name is part of
// the purpose of the mod, so it is a separate mod and not a settings preset of
// another one.
//
// Why this mod recreates the switcher instead of restoring the original one
//
// A restoration, which reuses the Windows 10 components inside explorer.exe the
// way ExplorerPatcher does, was the first approach and was abandoned on
// purpose: it stopped working on the author's own system, and it depends on a
// long list of internal, undocumented symbols that change from build to build.
// Chasing those symbols means a mod that breaks with every Windows update and
// that can't be verified on every build, which makes the effort not worth it.
// A recreation that uses only documented APIs (DWM thumbnails, GDI, a
// low-level keyboard hook) keeps working across builds, and it never touches
// the shell.
// -----------------------------------------------------------------------------

#include <windhawk_api.h>

#include <windows.h>
#include <dwmapi.h>
#include <objbase.h>
#include <shellapi.h>

#include <algorithm>
#include <atomic>
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

// Documented COM interfaces and constants, declared here instead of including
// shobjidl.h, so that the mod compiles against every toolchain.
constexpr GUID kCLSID_VirtualDesktopManager = {
    0xaa509086, 0x5ca9, 0x4c25,
    {0x8f, 0x95, 0x58, 0x9d, 0x3c, 0x07, 0xb4, 0x8a}};
constexpr GUID kIID_IVirtualDesktopManager = {
    0xa5cd92ff, 0x29be, 0x454c,
    {0x8d, 0x04, 0xd8, 0x28, 0x79, 0xfb, 0x3f, 0x1b}};

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

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

// Messages posted by the keyboard hook thread to the UI thread.
constexpr UINT kMsgSwitchShow = WM_APP + 1;    // wParam: 0 next, 1 previous
constexpr UINT kMsgSwitchCommit = WM_APP + 2;  // Alt was released
constexpr UINT kMsgSwitchCancel = WM_APP + 3;  // Esc
constexpr UINT kMsgSwitchClose = WM_APP + 4;   // the Win key was pressed
constexpr UINT kMsgSwitchCloseSelected = WM_APP + 5;  // Alt+F4
constexpr UINT kMsgSettingsChanged = WM_APP + 6;
constexpr UINT kMsgSameApp = WM_APP + 7;
constexpr UINT kMsgFallbackToggle = WM_APP + 8;

// Messages posted to the keyboard hook thread.
constexpr UINT kMsgInputThreadQuit = WM_APP + 100;
constexpr UINT kMsgInputThreadRehook = WM_APP + 101;

constexpr UINT_PTR kAutoCycleTimerId = 1;
constexpr UINT_PTR kSessionAgeTimerId = 2;
constexpr UINT_PTR kRebuildTimerId = 3;
constexpr UINT kRebuildDelayMs = 500;
constexpr UINT kAutoCycleIntervalMs = 250;
constexpr UINT kSessionAgeIntervalMs = 200;

// A session that is left open without Alt being held was abandoned by a missed
// key event; it commits on its own instead of staying on screen forever.
constexpr unsigned long long kSessionStuckLimitMs = 30000;

// The keyboard hook is reinstalled if it stops being called while input is
// still being delivered to the system (a hook can be dropped by Windows when
// the process that installed it is busy for too long).
constexpr DWORD kWatchdogIntervalMs = 5000;

// Ctrl+Alt+F11, the fallback hotkey.
constexpr int kFallbackHotkeyVk = VK_F11;

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

struct Settings {
    std::wstring switcherMonitor;
    int rowHeight = 230;
    int maxColumns = 4;
    int maxRows = 4;
    int switcherBackground = 85;
    int backgroundDimming = 0;
    bool currentDesktopOnly = true;
    bool autoCycleWhileAltHeld = false;
    bool mouseNavigation = true;
    bool sameAppHotkey = false;
    bool fallbackHotkey = true;
};

static Settings g_settings;
static std::mutex g_settingsMutex;

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
    Settings settings;

    // The settings are clamped here, because the settings schema of the mod
    // deliberately carries no numeric limits: a limit that Windhawk doesn't
    // recognize makes the whole settings block fail to parse, and defaults
    // that the user can't edit are worse than a value that is clamped.
    settings.switcherMonitor = PickOneOf(
        GetStringSettingOr(L"switcherMonitor", L"primary"),
        {L"primary", L"cursor", L"active-window"}, L"primary");

    settings.rowHeight = std::clamp(Wh_GetIntSetting(L"rowHeight"), 90, 480);
    settings.maxColumns = std::clamp(Wh_GetIntSetting(L"maxColumns"), 2, 6);
    settings.maxRows = std::clamp(Wh_GetIntSetting(L"maxRows"), 1, 6);
    settings.switcherBackground =
        std::clamp(Wh_GetIntSetting(L"switcherBackground"), 0, 100);
    settings.backgroundDimming =
        std::clamp(Wh_GetIntSetting(L"backgroundDimming"), 0, 100);
    settings.currentDesktopOnly = Wh_GetIntSetting(L"currentDesktopOnly") != 0;
    settings.autoCycleWhileAltHeld =
        Wh_GetIntSetting(L"autoCycleWhileAltHeld") != 0;
    settings.mouseNavigation = Wh_GetIntSetting(L"mouseNavigation") != 0;
    settings.sameAppHotkey = Wh_GetIntSetting(L"sameAppHotkey") != 0;
    settings.fallbackHotkey = Wh_GetIntSetting(L"fallbackHotkey") != 0;

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(settings);
}

static Settings GetSettingsCopy() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// -----------------------------------------------------------------------------
// Global state
//
// The session and everything that draws belongs to the UI thread. The atomics
// below are the only state shared with the keyboard hook thread and with the
// watchdog, and they are all simple flags.
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
static std::atomic<bool> g_sameAppHotkeyEnabled{false};
static std::atomic<bool> g_fallbackHotkeyEnabled{true};

static HHOOK g_keyboardHook = nullptr;  // the keyboard hook thread only
static DWORD g_inputThreadId = 0;
static HANDLE g_uiThread = nullptr;
static HANDLE g_inputThread = nullptr;
static HANDLE g_watchdogThread = nullptr;
static HANDLE g_uiReadyEvent = nullptr;
static HANDLE g_hookReadyEvent = nullptr;
static HANDLE g_stopEvent = nullptr;

// Defined further down, declared here for the functions that use them.
static void ApplySessionTimers();
static void RefreshThumbnails();
static void SendAltTabToSystem();
static void UpdateHookFlags(const Settings& settings);

// -----------------------------------------------------------------------------
// Exception guard
//
// The mod runs as a tool mod in its own process, so an exception that escapes
// can't take the shell down, but it can leave the switcher on screen with a
// state that is out of sync, or end the tool process while Alt+Tab is taken
// over. Every entry point that the system calls into is therefore guarded, and
// a failure closes the switcher instead of leaving it half alive.
// -----------------------------------------------------------------------------

static std::atomic<unsigned> g_exceptionCount{0};

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
//
// Every system resource the mod owns is released by a wrapper, so no exit path
// can leak it, including the paths taken when an API call fails.
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

// A device context obtained with GetDC(NULL), which belongs to the screen and
// must be given back with ReleaseDC. Releasing it with DeleteDC, which is what
// a ScopedDC would do, leaks the screen device context. The system only hands
// out a limited number of them, so the distinction matters over a long
// session with many switcher opens.
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

// The device context of a window, which has to be released instead of deleted
// for the same reason.
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

// A brush, for the paths that need a single fill with a solid color.
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

// A bitmap or any other GDI object that is released with DeleteObject, so the
// two buffers of the mod can't leak theirs on the failure paths of creation.
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

// The COM apartment of the mod's UI thread, closed on every exit path,
// including the early returns that the initialization takes when a step fails.
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

// A font owned by the mod. It is released when the handle is replaced and when
// the mod stops, so a DPI change can't leak the previous font.
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

// The device context states that the drawing code changes, restored when the
// scope ends, so no drawing path can leave the frame buffer in a state that
// makes the next one look wrong.
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

// A 32-bit top-down DIB section, used both for the desktop snapshot and for
// the composited frame that is blitted to the overlay window.
struct BitmapSurface {
    // The device context and the bitmap are owned by their guards, which
    // release them on destruction, so the intermediate failures of Ensure
    // can't leak either of them.
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

    // Creates the surface if it doesn't exist yet or if the size changed.
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

// Repeated failures (for example a thumbnail that can't be registered) are
// logged a few times and then only the total is reported on uninit.
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
//
// The list is built the way the shell builds its own switch list: top-level,
// visible windows with a title, in z-order, skipping cloaked windows, tool
// windows and the windows of the mod's own process. The filters are the ones
// the original switcher applies, and two extras: minimized windows and windows
// of other virtual desktops can be left out from the settings.
// -----------------------------------------------------------------------------

struct WindowItem {
    HWND hwnd = nullptr;
    std::wstring title;
    std::wstring appPath;
    HICON icon = nullptr;
    bool ownIcon = false;
    bool minimized = false;
    HTHUMBNAIL thumbnail = nullptr;
    bool hasThumbnail = false;
    RECT card{};   // the whole card, in client coordinates
    RECT thumb{};  // where the live preview goes, in client coordinates

    WindowItem() = default;
    ~WindowItem() {
        Reset();
    }

    WindowItem(const WindowItem&) = delete;
    WindowItem& operator=(const WindowItem&) = delete;

    WindowItem(WindowItem&& other) noexcept {
        MoveFrom(other);
    }

    WindowItem& operator=(WindowItem&& other) noexcept {
        if (this != &other) {
            Reset();
            MoveFrom(other);
        }
        return *this;
    }

    void MoveFrom(WindowItem& other) {
        hwnd = other.hwnd;
        title = std::move(other.title);
        appPath = std::move(other.appPath);
        icon = other.icon;
        ownIcon = other.ownIcon;
        minimized = other.minimized;
        thumbnail = other.thumbnail;
        hasThumbnail = other.hasThumbnail;
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

// The undecorated export the shell uses to read window titles. It never sends
// a message, so it can't hang on a busy application; WM_GETTEXT with a timeout
// is the fallback.
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

// The window of a UWP application belongs to ApplicationFrameHost.exe; the
// real process is the child window that hosts the content, and the icon and
// the application name have to come from there.
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

    // The frame host is shared by all UWP applications; the content window
    // inside it belongs to the application itself.
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

// The icon of the window, and whether the mod owns it. A borrowed icon is
// never destroyed, an icon loaded from the executable is owned by the mod.
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

    SHFILEINFOW fileInfo{};
    if (SHGetFileInfoW(appPath.c_str(), 0, &fileInfo, sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON) &&
        fileInfo.hIcon) {
        *owned = true;
        return fileInfo.hIcon;
    }

    return nullptr;
}

// DWM_CLOAKED_SHELL: the window is cloaked by the shell, which is how windows
// on other virtual desktops are hidden.
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

    // Only top-level windows that own themselves.
    if (GetAncestor(hwnd, GA_ROOTOWNER) != hwnd) {
        return false;
    }

    const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    // Owned popups aren't part of the switcher unless they ask for it.
    if (GetWindow(hwnd, GW_OWNER) && !(exStyle & WS_EX_APPWINDOW)) {
        return false;
    }

    // Tool windows and windows that can't be activated, such as the mod's own
    // overlay and other overlays.
    if ((exStyle & WS_EX_TOOLWINDOW) || (exStyle & WS_EX_NOACTIVATE)) {
        return false;
    }

    // Windows on another virtual desktop are cloaked by the shell, so a cloaked
    // window can only be accepted when the user asked for the windows of all
    // the desktops, and only if the shell is the sole reason it is cloaked and
    // the window really is on another desktop. Anything else that is cloaked,
    // such as a suspended UWP app on the current desktop, is not a window the
    // user can switch to.
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

struct EnumerationContext {
    const Settings* settings = nullptr;
    const std::wstring* appPathFilter = nullptr;
    std::vector<WindowItem>* items = nullptr;
    int skipped = 0;
};

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* context = reinterpret_cast<EnumerationContext*>(lParam);
    if (!context || !context->items || !context->settings) {
        return FALSE;
    }

    if (!IsSwitcherWindow(hwnd, !context->settings->currentDesktopOnly)) {
        ++context->skipped;
        return TRUE;
    }

    // Minimized windows are part of the switcher, like they are in the
    // Windows 10 switcher and in ExplorerPatcher's switcher.
    const bool minimized = IsIconic(hwnd) != FALSE;

    if (context->settings->currentDesktopOnly &&
        !IsOnCurrentVirtualDesktop(hwnd)) {
        return TRUE;
    }

    WindowItem item;
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

// Fills the list with the windows that the switcher shows, in z-order, which
// makes the order the order of last use, like the original switcher's.
static void BuildWindowList(const Settings& settings,
                            const std::wstring& appPathFilter,
                            std::vector<WindowItem>* items) {
    items->clear();

    EnumerationContext context;
    context.settings = &settings;
    context.appPathFilter = &appPathFilter;
    context.items = items;

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&context));

    Wh_Log(L"Window list built: %u window(s) in the switcher, %d candidate(s) skipped",
           static_cast<unsigned>(items->size()), context.skipped);
}

// -----------------------------------------------------------------------------
// Item geometry
//
// The measurements are the ones of ExplorerPatcher's Simple Window Switcher, as
// ported to Windhawk by the Simple Window Switcher mod by Lone, which the readme
// credits: the 20 px margin around the switcher, the 2 px selection contour, the
// 16 px title icons, a base row height of 230 px, the same shrink steps for many
// windows, a preview at most twice as wide as it is tall, and a switcher limited
// to 80% of the monitor.
//
// The items keep the aspect ratio of their window, so a wide window gets a wide
// item and a narrow window a narrow one, and the rows are centered. That is how
// ExplorerPatcher's switcher packs its rows; the Windows 10 switcher packs its
// grid the same way inside fixed cells, with the title above each preview.
// -----------------------------------------------------------------------------

constexpr int kMasterPadding = 20;          // margin around the switcher panel
constexpr int kCellGap = 4;                 // between two items
constexpr int kCellPadding = 7;             // SWS_PAD_LEFT/RIGHT/TOP/BOTTOM
constexpr int kTitleBandHeight = 30;        // SWS_ROW_TITLE_HEIGHT
constexpr int kTitleGap = 7;                // SWS_PAD_DIVIDER
constexpr int kContourSize = 2;             // SWS_CONTOUR_SIZE
constexpr int kTitleIconSize = 16;          // SWS_ICON_SIZE
constexpr int kTitleTextHeight = 13;        // the title row text
constexpr int kMinRowHeight = 90;           // SWS_AUTOFIT_MIN_ROWHEIGHT
constexpr int kMaxTileAspectPercent = 200;  // SWS_MAX_TILE_ASPECT, 2.0
constexpr int kMaxWidthPercent = 80;        // SWS maxWidthPercent
constexpr int kMaxHeightPercent = 80;       // SWS maxHeightPercent
constexpr int kMinPreviewWidth = 64;
constexpr int kTitleIconGap = 6;            // between the icon and the title text

struct ItemGeometry {
    RECT cell{};   // the item, selection border included
    RECT title{};  // the icon and title row
    RECT icon{};
    RECT text{};
    RECT thumb{};  // the preview
};

struct Layout {
    int firstIndex = 0;  // the index in the window list that the first item is
    int pageSize = 1;
    int pageCount = 1;
    int rowHeight = 0;
    int thumbH = 0;
    int cellH = 0;
    int cellPad = 0;
    int titleBand = 0;
    int titleGap = 0;
    int contour = 0;
    int iconSize = 0;
    int cellGap = 0;
    RECT panel{};
    std::vector<ItemGeometry> items;
};

static int ScaleForDpi(int value, int dpi) {
    return MulDiv(value, dpi, 96);
}

// The shrink steps ExplorerPatcher's switcher uses when there are many windows.
static int AutoFitPercent(size_t count) {
    if (count <= 8) {
        return 100;
    }
    if (count <= 14) {
        return 80;
    }
    if (count <= 22) {
        return 65;
    }
    if (count <= 32) {
        return 50;
    }
    return 40;
}

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

// Places the items of one page and returns the geometry of the whole switcher.
// The rows are shrunk until the page fits the monitor, which is what the
// original switcher does with its Thumbnail_min_width and Grid_* values.
static Layout ComputeLayout(const Settings& settings,
                            const std::vector<WindowItem>& items,
                            int pageIndex,
                            const RECT& area,
                            int dpi) {
    Layout layout;

    layout.cellPad = ScaleForDpi(kCellPadding, dpi);
    layout.titleBand = ScaleForDpi(kTitleBandHeight, dpi);
    layout.titleGap = ScaleForDpi(kTitleGap, dpi);
    layout.contour = std::max(1, ScaleForDpi(kContourSize, dpi));
    layout.iconSize = ScaleForDpi(kTitleIconSize, dpi);
    layout.cellGap = std::max(1, ScaleForDpi(kCellGap, dpi));

    const int count = static_cast<int>(items.size());
    const int columns = std::max(1, settings.maxColumns);
    const int rowsPerPage = std::max(1, settings.maxRows);
    layout.pageSize = columns * rowsPerPage;
    layout.pageCount = std::max(1, (count + layout.pageSize - 1) / layout.pageSize);

    int page = std::clamp(pageIndex, 0, layout.pageCount - 1);
    const int firstIndex = page * layout.pageSize;
    const int onPage = std::min(layout.pageSize, std::max(0, count - firstIndex));
    if (onPage <= 0) {
        return layout;
    }

    const int masterPad = ScaleForDpi(kMasterPadding, dpi);
    const int contentLimit = (area.right - area.left) * kMaxWidthPercent / 100 -
        2 * masterPad;
    const int heightLimit = (area.bottom - area.top) * kMaxHeightPercent / 100 -
        2 * masterPad;
    const int minRowHeight = ScaleForDpi(kMinRowHeight, dpi);
    const int minPreviewWidth = ScaleForDpi(kMinPreviewWidth, dpi);

    int rowHeight = ScaleForDpi(std::clamp(settings.rowHeight, 90, 480), dpi) *
        AutoFitPercent(items.size()) / 100;

    for (int attempt = 0; attempt < 64; ++attempt) {
        const int thumbH = std::max(
            1, rowHeight - (2 * layout.cellPad + layout.titleBand +
                            layout.titleGap));
        const int maxTileW = rowHeight * kMaxTileAspectPercent / 100;

        // The width of every preview follows the aspect ratio of its window,
        // and no preview is wider than twice its height or wider than its
        // window, so that small windows are not blown up.
        std::vector<int> widths;
        widths.reserve(onPage);
        for (int i = 0; i < onPage; ++i) {
            const WindowItem& item = items[firstIndex + i];
            int width =
                static_cast<int>(WindowAspect(item.hwnd) * thumbH + 0.5);
            width = std::min(width, std::min(maxTileW, thumbH * 3));
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

        // Fill the rows, respecting the configured column count and the width
        // of the monitor.
        std::vector<std::vector<int>> rows;
        std::vector<int> rowWidths;
        std::vector<int> row{};
        int rowWidth = 0;
        for (int i = 0; i < onPage; ++i) {
            const int cellW = widths[i] + 2 * layout.cellPad;
            const bool tooWide = !row.empty() &&
                rowWidth + layout.cellGap + cellW > contentLimit;
            if (tooWide || static_cast<int>(row.size()) >= columns) {
                rows.push_back(row);
                rowWidths.push_back(rowWidth);
                row.clear();
                rowWidth = 0;
            }
            rowWidth += (row.empty() ? 0 : layout.cellGap) + cellW;
            row.push_back(i);
        }
        if (!row.empty()) {
            rows.push_back(row);
            rowWidths.push_back(rowWidth);
        }

        const int cellH = thumbH + 2 * layout.cellPad + layout.titleBand +
            layout.titleGap;
        const int contentW = *std::max_element(rowWidths.begin(),
                                               rowWidths.end());
        const int contentH = static_cast<int>(rows.size()) * cellH +
            (static_cast<int>(rows.size()) - 1) * layout.cellGap;

        if ((contentW <= contentLimit && contentH <= heightLimit) ||
            rowHeight <= minRowHeight) {
            // The page fits, or the rows can't be shrunk any further.

            const int areaW = area.right - area.left;
            const int areaH = area.bottom - area.top;
            const int panelW = contentW + 2 * masterPad;
            const int panelH = contentH + 2 * masterPad;
            layout.panel = RECT{area.left + (areaW - panelW) / 2,
                                area.top + (areaH - panelH) / 2,
                                area.left + (areaW - panelW) / 2 + panelW,
                                area.top + (areaH - panelH) / 2 + panelH};

            layout.firstIndex = firstIndex;
            layout.rowHeight = rowHeight;
            layout.thumbH = thumbH;
            layout.cellH = cellH;
            layout.items.assign(onPage, ItemGeometry{});

            const int textGap = ScaleForDpi(kTitleIconGap, dpi);
            int y = layout.panel.top + masterPad;
            for (size_t r = 0; r < rows.size(); ++r) {
                int x = layout.panel.left + masterPad +
                    (contentW - rowWidths[r]) / 2;
                for (int index : rows[r]) {
                    ItemGeometry& geometry = layout.items[index];
                    const int cellW = widths[index] + 2 * layout.cellPad;

                    geometry.cell = RECT{x, y, x + cellW, y + cellH};
                    geometry.title = RECT{geometry.cell.left + layout.cellPad,
                                          y + layout.cellPad,
                                          geometry.cell.right - layout.cellPad,
                                          y + layout.cellPad + layout.titleBand};
                    geometry.icon = RECT{
                        geometry.title.left,
                        geometry.title.top +
                            (layout.titleBand - layout.iconSize) / 2,
                        geometry.title.left + layout.iconSize,
                        geometry.title.top +
                            (layout.titleBand - layout.iconSize) / 2 +
                            layout.iconSize};
                    geometry.text = geometry.title;
                    geometry.thumb = RECT{
                        geometry.cell.left + layout.cellPad,
                        geometry.title.bottom + layout.titleGap,
                        geometry.cell.right - layout.cellPad,
                        geometry.title.bottom + layout.titleGap + thumbH};

                    // The title starts after the icon: the caller finalizes
                    // this in AssignItemRects, where the icons are known.
                    geometry.text.left =
                        std::min(geometry.title.right,
                                 geometry.icon.right + textGap);

                    x += cellW + layout.cellGap;
                }
                y += cellH + layout.cellGap;
            }
            break;
        }

        rowHeight = std::max(minRowHeight, rowHeight * 9 / 10);
    }

    return layout;
}

// -----------------------------------------------------------------------------
// Desktop snapshot and blended fills
// -----------------------------------------------------------------------------

// A 16x16 DIB whose single color is set before every blended fill. GDI's
// AlphaBlend takes the constant alpha from the blend function when the source
// has no per-pixel alpha, which is exactly what a translucent black layer or
// a translucent white highlight needs.
struct SolidColorSource {
    // As in BitmapSurface, the device context and the bitmap are owned by
    // their guards, so the failure paths of Ensure can't leak them.
    ScopedDC dc;
    ScopedBitmap bitmap;
    HGDIOBJ oldBitmap = nullptr;
    uint32_t* pixel = nullptr;

    ~SolidColorSource() {
        reset();
    }

    SolidColorSource() = default;
    SolidColorSource(const SolidColorSource&) = delete;
    SolidColorSource& operator=(const SolidColorSource&) = delete;

    void reset() {
        if (dc.value && oldBitmap) {
            SelectObject(dc.value, oldBitmap);
            oldBitmap = nullptr;
        }
        bitmap.reset();
        dc.reset();
        pixel = nullptr;
    }

    bool Ensure() {
        if (dc.value && pixel) {
            return true;
        }

        ScopedScreenDC screen(nullptr, GetDC(nullptr));
        if (!screen) {
            return false;
        }

        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = 2;
        info.bmiHeader.biHeight = -2;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        ScopedBitmap newBitmap(CreateDIBSection(screen.value, &info,
                                               DIB_RGB_COLORS, &bits, nullptr,
                                               0));
        if (!newBitmap || !bits) {
            return false;
        }

        ScopedDC newDc(CreateCompatibleDC(screen.value));
        if (!newDc) {
            return false;
        }

        dc = std::move(newDc);
        bitmap = std::move(newBitmap);
        oldBitmap = SelectObject(dc.value, bitmap.value);
        pixel = reinterpret_cast<uint32_t*>(bits);

        // The four pixels of the 2x2 source are the same color, which keeps
        // the stretched source uniform on every GDI implementation.
        for (int i = 0; i < 4; ++i) {
            pixel[i] = 0;
        }
        return true;
    }

    void SetColor(COLORREF color) {
        if (!pixel) {
            return;
        }
        const uint32_t value = (static_cast<uint32_t>(GetRValue(color)) << 16) |
            (static_cast<uint32_t>(GetGValue(color)) << 8) |
            static_cast<uint32_t>(GetBValue(color));
        for (int i = 0; i < 4; ++i) {
            pixel[i] = value;
        }
    }
};

static SolidColorSource g_solidColor;

static bool g_alphaBlendFailed = false;

static void AlphaFillRect(HDC dc, const RECT& rect, COLORREF color,
                          BYTE alpha) {
    if (!dc || rect.right <= rect.left || rect.bottom <= rect.top ||
        alpha == 0) {
        return;
    }

    if (!g_solidColor.Ensure()) {
        if (!g_alphaBlendFailed) {
            g_alphaBlendFailed = true;
            Wh_Log(L"The blended fill source couldn't be created; the switcher is drawn without translucent layers");
        }
        return;
    }

    g_solidColor.SetColor(color);

    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = alpha;
    blend.AlphaFormat = 0;

    if (!AlphaBlend(dc, rect.left, rect.top, rect.right - rect.left,
                    rect.bottom - rect.top, g_solidColor.dc.value, 0, 0, 2, 2,
                    blend)) {
        if (!g_alphaBlendFailed) {
            g_alphaBlendFailed = true;
            Wh_Log(L"AlphaBlend failed; the switcher is drawn without translucent layers");
        }
    }
}

// Applies the desktop dimming, the equivalent of the original switcher's
// BackgroundDimmingLayer_percent value. It is applied once, when the snapshot
// is taken, so that the per-selection repaints stay cheap.
static void ApplyDesktopDimming(BitmapSurface& surface, int percent) {
    if (percent <= 0 || !surface) {
        return;
    }

    RECT rect{0, 0, surface.width, surface.height};
    const BYTE alpha = static_cast<BYTE>(percent * 255 / 100);
    AlphaFillRect(surface.dc.value, rect, RGB(0, 0, 0), alpha);
}

// The snapshot is taken from the screen while the overlay window is hidden, so
// the switcher never captures itself.
static bool CaptureDesktop(const RECT& monitorRect, BitmapSurface* surface) {
    const int width = monitorRect.right - monitorRect.left;
    const int height = monitorRect.bottom - monitorRect.top;
    if (!surface->Ensure(width, height)) {
        return false;
    }

    ScopedScreenDC screen(nullptr, GetDC(nullptr));
    if (!screen) {
        return false;
    }

    if (!BitBlt(surface->dc.value, 0, 0, width, height, screen.value,
                monitorRect.left, monitorRect.top,
                SRCCOPY | CAPTUREBLT)) {
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// Session state
//
// Everything here belongs to the mod's UI thread. The keyboard hook thread and
// the watchdog never touch the session directly; they post messages to the
// overlay window, which keeps the state single threaded and race free.
// -----------------------------------------------------------------------------

constexpr wchar_t kOverlayClassName[] = L"WindhawkWindows10AltTabOverlay";

struct Session {
    bool active = false;
    bool visible = false;
    HWND hwnd = nullptr;  // the overlay window the session is shown on
    Settings settings;
    std::wstring appPathFilter;
    std::vector<WindowItem> items;
    Layout layout;
    int selectedIndex = -1;
    int hoverIndex = -1;
    int page = 0;
    int dpi = 96;
    RECT monitorRect{};  // the monitor the switcher is shown on, screen coords
    RECT workArea{};     // the work area of that monitor, client coords
    BitmapSurface snapshot;
    BitmapSurface frame;
    unsigned long long startedTick = 0;
};

static Session g_session;

// Highlight and border values, tuned to sit close to the original switcher's
// Thumbnail_focus_percent and Thumbnail_focus_border_width look.
// -----------------------------------------------------------------------------
// Drawing
//
// The look is the one of ExplorerPatcher's switcher, which the readme credits:
// a rounded translucent panel, one item per window with the application icon
// and the title above the live preview, and a 2 px contour around the selected
// item only. Unselected items carry no frame of their own, exactly like there.
// -----------------------------------------------------------------------------

// The Windows 10 switcher was always dark: switching the operating system to
// the light theme, introduced in Windows 10 version 1903, did not turn the
// switcher light, and this recreation doesn't either. Everything the switcher
// draws uses these three fixed colors, and the mod reads nothing from the
// registry: it doesn't follow the system color setting, and it doesn't need to.
constexpr COLORREF kPanelColor = RGB(26, 27, 30);       // the dark panel
constexpr COLORREF kTextColor = RGB(255, 255, 255);     // the titles
constexpr COLORREF kSelectionColor = RGB(255, 255, 255); // the selected window

constexpr BYTE kHoverFillAlpha = 36;  // the hover highlight of an item

// The title font, cached per DPI and owned by the mod.
static ScopedFont g_titleFont;
static int g_titleFontDpi = 0;

static HFONT GetTitleFont(int dpi) {
    if (g_titleFont.value && g_titleFontDpi == dpi) {
        return g_titleFont.value;
    }
    g_titleFont.reset();

    LOGFONTW font{};
    font.lfHeight = -ScaleForDpi(kTitleTextHeight, dpi);
    font.lfWeight = FW_NORMAL;
    font.lfCharSet = DEFAULT_CHARSET;
    font.lfOutPrecision = OUT_TT_PRECIS;
    font.lfQuality = CLEARTYPE_QUALITY;
    font.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    lstrcpynW(font.lfFaceName, L"Segoe UI", ARRAYSIZE(font.lfFaceName));

    g_titleFont.reset(CreateFontIndirectW(&font));
    g_titleFontDpi = dpi;
    return g_titleFont.value;
}

// An inward contour, like the one the switcher draws around its selection.
static void DrawContour(HDC dc, const RECT& rect, int thickness,
                        COLORREF color) {
    if (thickness <= 0 || rect.right - rect.left <= 0 ||
        rect.bottom - rect.top <= 0) {
        return;
    }

    RECT current = rect;
    for (int i = 0; i < thickness; ++i) {
        if (current.right - current.left <= 0 ||
            current.bottom - current.top <= 0) {
            break;
        }

        AlphaFillRect(dc,
                      RECT{current.left, current.top, current.right,
                           current.top + 1},
                      color, 255);
        AlphaFillRect(dc,
                      RECT{current.left, current.bottom - 1, current.right,
                           current.bottom},
                      color, 255);
        AlphaFillRect(dc,
                      RECT{current.left, current.top + 1, current.left + 1,
                           current.bottom - 1},
                      color, 255);
        AlphaFillRect(dc,
                      RECT{current.right - 1, current.top + 1, current.right,
                           current.bottom - 1},
                      color, 255);
        InflateRect(&current, -1, -1);
    }
}

// The translucent panel behind the grid, through which the desktop stays
// visible, which is the look of the original switcher and of the Simple Window
// Switcher mod with its background opacity. The panel is squared, like the one
// of the Windows 10 switcher and of the "Horizontal squared" layout of the
// Simple Window Switcher mod. 0 leaves the desktop untouched.
static void DrawSwitcherPanel(HDC dc) {
    const RECT& panel = g_session.layout.panel;
    if (panel.right <= panel.left || panel.bottom <= panel.top) {
        return;
    }

    const int opacity =
        std::clamp(g_session.settings.switcherBackground, 0, 100);
    if (opacity <= 0) {
        return;
    }

    const BYTE alpha = static_cast<BYTE>(opacity * 255 / 100);
    AlphaFillRect(dc, panel, kPanelColor, alpha);
}

static void DrawSwitcherItem(HDC dc, const WindowItem& item,
                             const ItemGeometry& geometry, bool selected,
                             bool hovered) {
    // The selected window carries a 2 px contour and nothing else, which is
    // the selection style of the original switcher and of the "Horizontal
    // squared" layout of the Simple Window Switcher mod.
    if (selected) {
        DrawContour(dc, geometry.cell, g_session.layout.contour,
                    kSelectionColor);
    } else if (hovered) {
        AlphaFillRect(dc, geometry.cell, kSelectionColor, kHoverFillAlpha);
    }

    // A window whose live preview couldn't be registered with DWM gets its
    // application icon in the middle of the preview area instead. Minimized
    // windows do get a preview, so this only shows up when DWM refuses one.
    if (!item.hasThumbnail && item.icon &&
        geometry.thumb.right > geometry.thumb.left) {
        const int previewH = geometry.thumb.bottom - geometry.thumb.top;
        const int iconSize = std::max(ScaleForDpi(24, g_session.dpi),
                                      previewH * 2 / 5);
        const int x = geometry.thumb.left +
            (geometry.thumb.right - geometry.thumb.left - iconSize) / 2;
        const int y = geometry.thumb.top + (previewH - iconSize) / 2;
        DrawIconEx(dc, x, y, item.icon, iconSize, iconSize, 0, nullptr,
                   DI_NORMAL);
    }

    // The title row: the application icon, then the title.
    if (item.icon) {
        DrawIconEx(dc, geometry.icon.left, geometry.icon.top, item.icon,
                   g_session.layout.iconSize, g_session.layout.iconSize, 0,
                   nullptr, DI_NORMAL);
    }

    RECT text = item.icon ? geometry.text : geometry.title;
    text.right = geometry.title.right;
    if (text.right > text.left && !item.title.empty()) {
        // The guards restore the font, the text color and the background mode
        // when the scope ends, whatever happens in between.
        ScopedSelectedObject font(dc, GetTitleFont(g_session.dpi));
        ScopedBkMode bkMode(dc, TRANSPARENT);
        ScopedTextColor textColor(dc, kTextColor);

        DrawTextW(dc, item.title.c_str(), -1, &text,
                  DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
    }
}

// The frame is composed off screen, so showing it or repainting it for a new
// selection is a single BitBlt and can't flicker.
static void PaintFrame() {
    if (!g_session.frame || !g_session.snapshot) {
        return;
    }

    HDC dc = g_session.frame.dc.value;
    const int width = g_session.frame.width;
    const int height = g_session.frame.height;

    BitBlt(dc, 0, 0, width, height, g_session.snapshot.dc.value, 0, 0, SRCCOPY);

    DrawSwitcherPanel(dc);

    for (size_t index = 0; index < g_session.layout.items.size(); ++index) {
        const int itemIndex =
            g_session.layout.firstIndex + static_cast<int>(index);
        if (itemIndex < 0 ||
            itemIndex >= static_cast<int>(g_session.items.size())) {
            continue;
        }

        DrawSwitcherItem(dc, g_session.items[itemIndex],
                         g_session.layout.items[index],
                         itemIndex == g_session.selectedIndex,
                         itemIndex == g_session.hoverIndex);
    }

}

static void PresentFrame() {
    if (!g_session.frame || !g_session.hwnd || !IsWindow(g_session.hwnd)) {
        return;
    }

    ScopedWindowDC windowDc(g_session.hwnd);
    if (!windowDc) {
        return;
    }

    BitBlt(windowDc.value, 0, 0, g_session.frame.width, g_session.frame.height,
           g_session.frame.dc.value, 0, 0, SRCCOPY);
}

static void PaintAndPresent() {
    PaintFrame();
    PresentFrame();
}

// -----------------------------------------------------------------------------
// Session lifetime
//
// The session owns the window list, the layout, the desktop snapshot and the
// DWM thumbnails. It is created when the switcher is opened and released when
// it is closed, so no large buffer is kept between two Alt+Tab presses.
// -----------------------------------------------------------------------------

enum class SessionResult {
    Ok,
    NothingToShow,  // fewer than two windows: the original does nothing either
    Failed,
};

static void AssignItemRects() {
    const Layout& layout = g_session.layout;

    for (WindowItem& item : g_session.items) {
        item.card = RECT{};
        item.thumb = RECT{};
    }

    for (size_t index = 0; index < layout.items.size(); ++index) {
        const int itemIndex = layout.firstIndex + static_cast<int>(index);
        if (itemIndex < 0 ||
            itemIndex >= static_cast<int>(g_session.items.size())) {
            continue;
        }

        WindowItem& item = g_session.items[itemIndex];
        item.card = layout.items[index].cell;
        item.thumb = layout.items[index].thumb;
    }
}

static void RecomputeLayoutAndRects() {
    // The page follows the selection: the selected window is always on the
    // page that is drawn, which also matters right after the window list was
    // rebuilt and became shorter. Two passes are enough, because the number of
    // items that fit on a page doesn't depend on which page is shown.
    int page = std::max(0, g_session.page);
    for (int pass = 0; pass < 2; ++pass) {
        g_session.layout = ComputeLayout(g_session.settings, g_session.items,
                                         page, g_session.workArea,
                                         g_session.dpi);

        int derivedPage = page;
        if (g_session.selectedIndex >= 0 && !g_session.items.empty()) {
            derivedPage = g_session.selectedIndex /
                std::max(1, g_session.layout.pageSize);
        }
        derivedPage =
            std::clamp(derivedPage, 0, g_session.layout.pageCount - 1);
        if (derivedPage == page) {
            break;
        }
        page = derivedPage;
    }

    g_session.page = page;
    AssignItemRects();
}

static void SelectIndex(int index) {
    const int count = static_cast<int>(g_session.items.size());
    if (count <= 0) {
        return;
    }

    index = ((index % count) + count) % count;
    const int pageSize = std::max(1, g_session.layout.pageSize);
    const int page = index / pageSize;
    const bool pageChanged = page != g_session.page;

    g_session.selectedIndex = index;
    if (pageChanged) {
        g_session.page = page;
        g_session.hoverIndex = -1;
        RecomputeLayoutAndRects();

        // The windows of the new page need the live previews that the windows
        // of the previous page had, and the ones that left the page have
        // their previews released.
        PaintAndPresent();
        RefreshThumbnails();
    }

    PaintAndPresent();
}

static void MoveSelection(int delta) {
    const int count = static_cast<int>(g_session.items.size());
    if (count <= 0 || delta == 0) {
        return;
    }

    int index = g_session.selectedIndex;
    if (index < 0) {
        index = delta > 0 ? -1 : 0;
    }

    SelectIndex(index + delta);
}

static int IndexAtPoint(int x, int y) {
    const POINT point{x, y};
    for (int i = 0; i < static_cast<int>(g_session.items.size()); ++i) {
        const RECT& card = g_session.items[i].card;
        if (card.right > card.left && PtInRect(&card, point)) {
            return i;
        }
    }
    return -1;
}

// Registers, updates and releases the DWM thumbnails of the current page. The
// previews of the windows that leave the page are released, which is also how
// the mod stays within the DWM thumbnail budget with long window lists.
static void RefreshThumbnails() {
    unsigned int failures = 0;
    unsigned int registered = 0;

    for (WindowItem& item : g_session.items) {
        const bool wantsThumbnail = item.card.right > item.card.left &&
            item.hwnd && IsWindow(item.hwnd) &&
            item.thumb.right > item.thumb.left;

        if (!wantsThumbnail) {
            item.ReleaseThumbnail();
            continue;
        }

        if (!item.hasThumbnail) {
            HTHUMBNAIL thumbnail = nullptr;
            if (FAILED(DwmRegisterThumbnail(g_session.hwnd, item.hwnd,
                                            &thumbnail)) ||
                !thumbnail) {
                continue;
            }
            item.thumbnail = thumbnail;
            item.hasThumbnail = true;
            ++registered;
        }

        DWM_THUMBNAIL_PROPERTIES properties{};
        properties.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE |
            DWM_TNP_OPACITY | DWM_TNP_SOURCECLIENTAREAONLY;
        properties.fVisible = TRUE;
        properties.opacity = 255;
        properties.rcDestination = item.thumb;
        properties.fSourceClientAreaOnly = TRUE;

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

    // A newly registered preview is drawn by DWM above the frame, so the frame
    // is composed again: the fallback icons are then only kept for the windows
    // that really have no preview.
    if (registered || failures) {
        PaintAndPresent();
    }
}

static HMONITOR PickMonitor(const Settings& settings, HWND activeWindow) {
    if (settings.switcherMonitor == L"cursor") {
        POINT cursor{};
        if (GetCursorPos(&cursor)) {
            HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
            if (monitor) {
                return monitor;
            }
        }
    } else if (settings.switcherMonitor == L"active-window") {
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

static SessionResult BeginSession(const Settings& settings,
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
                    &g_session.items);
    if (g_session.items.size() < 2) {
        // With a single window the original switcher does nothing, so the
        // mod swallows the key press as well and nothing is re-sent.
        g_session.items.clear();
        return SessionResult::NothingToShow;
    }

    g_session.settings = settings;
    g_session.appPathFilter = sameApp ? appFilter : std::wstring();
    g_session.monitorRect = info.rcMonitor;
    g_session.workArea = info.rcWork;
    OffsetRect(&g_session.workArea, -info.rcMonitor.left, -info.rcMonitor.top);
    g_session.dpi = GetMonitorDpi(monitor, info);
    g_session.page = 0;
    g_session.hoverIndex = -1;
    g_session.selectedIndex = -1;

    // The active window is the first item of the list; the first Tab moves one
    // window away from it, which is how the original starts.
    for (int i = 0; i < static_cast<int>(g_session.items.size()); ++i) {
        if (g_session.items[i].hwnd == foreground) {
            g_session.selectedIndex = i;
            *foregroundFound = true;
            break;
        }
    }

    RecomputeLayoutAndRects();

    const int width = info.rcMonitor.right - info.rcMonitor.left;
    const int height = info.rcMonitor.bottom - info.rcMonitor.top;

    if (!g_session.frame.Ensure(width, height)) {
        Wh_Log(L"The frame buffer for %dx%d couldn't be created", width, height);
        g_session.items.clear();
        return SessionResult::Failed;
    }

    if (CaptureDesktop(info.rcMonitor, &g_session.snapshot)) {
        ApplyDesktopDimming(g_session.snapshot, settings.backgroundDimming);
    } else {
        // Without a snapshot the switcher still works, it just loses the
        // desktop behind it.
        static RateLimitedLog log{L"the desktop snapshot couldn't be taken; the switcher is drawn on a solid background", 1};
        if (log.ShouldLog()) {
            Wh_Log(L"The desktop snapshot couldn't be taken; the switcher is drawn on a solid background instead");
        }
        RECT rect{0, 0, width, height};
        ScopedBrush brush(CreateSolidBrush(RGB(16, 16, 16)));
        if (brush) {
            FillRect(g_session.frame.dc.value, &rect, brush.value);
        }
        // Make the snapshot a copy of that background, so that the repaints
        // stay consistent.
        if (g_session.snapshot.Ensure(width, height)) {
            BitBlt(g_session.snapshot.dc.value, 0, 0, width, height,
                   g_session.frame.dc.value, 0, 0, SRCCOPY);
        }
    }

    // Position the window and paint it while it is still hidden, so that
    // showing it can't flash an empty or outdated frame.
    SetWindowPos(g_session.hwnd, HWND_TOPMOST, info.rcMonitor.left,
                 info.rcMonitor.top, width, height,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER);

    PaintFrame();
    PresentFrame();

    g_session.active = true;
    g_session.visible = true;
    g_session.startedTick = GetTickCount64();
    g_switcherVisible.store(true, std::memory_order_release);

    ShowWindow(g_session.hwnd, SW_SHOWNOACTIVATE);

    RefreshThumbnails();
    // DWM draws the live previews above the frame, so the frame is composed
    // once more: the fallback icons are then only kept for the windows that
    // really have no preview.
    PaintAndPresent();
    ApplySessionTimers();

    // A quick Alt+Tab tap can release Alt while the switcher is still being
    // built, in which case the hook doesn't post the commit. The queued
    // commit is handled right after this function returns, so the selection
    // that the caller makes is the one that is activated.
    if (!g_altDown.load(std::memory_order_relaxed)) {
        PostMessageW(g_session.hwnd, kMsgSwitchCommit, 0, 0);
    }

    Wh_Log(L"The switcher is showing %u window(s), page %d of %d",
           static_cast<unsigned>(g_session.items.size()), g_session.page + 1,
           g_session.layout.pageCount);
    return SessionResult::Ok;
}

static void ApplySessionTimers() {
    if (!g_session.hwnd || !IsWindow(g_session.hwnd)) {
        return;
    }

    KillTimer(g_session.hwnd, kAutoCycleTimerId);
    KillTimer(g_session.hwnd, kSessionAgeTimerId);
    KillTimer(g_session.hwnd, kRebuildTimerId);

    if (!g_session.active) {
        return;
    }

    SetTimer(g_session.hwnd, kSessionAgeTimerId, kSessionAgeIntervalMs,
             nullptr);
    if (g_session.settings.autoCycleWhileAltHeld) {
        SetTimer(g_session.hwnd, kAutoCycleTimerId, kAutoCycleIntervalMs,
                 nullptr);
    }
}

static void EndSession() {
    if (g_session.hwnd && IsWindow(g_session.hwnd)) {
        KillTimer(g_session.hwnd, kAutoCycleTimerId);
        KillTimer(g_session.hwnd, kSessionAgeTimerId);
        KillTimer(g_session.hwnd, kRebuildTimerId);
        ShowWindow(g_session.hwnd, SW_HIDE);
    }

    for (WindowItem& item : g_session.items) {
        item.ReleaseThumbnail();
    }
    g_session.items.clear();

    // The desktop snapshot and the frame are monitor sized, so they are freed
    // between two Alt+Tab presses instead of being kept in memory.
    g_session.snapshot.reset();
    g_session.frame.reset();

    g_session.active = false;
    g_session.visible = false;
    g_session.selectedIndex = -1;
    g_session.hoverIndex = -1;
    g_switcherVisible.store(false, std::memory_order_release);
}

// -----------------------------------------------------------------------------
// Activating the selected window
//
// A process may only change the foreground window under certain conditions, and
// the mod's process is never the foreground process. The strategies below are
// tried in order, from the cheapest and most standard to the strongest, and
// each one is verified before the next is tried.
// -----------------------------------------------------------------------------

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

// Attaching to the thread of the foreground window is the classic workaround
// for the foreground lock.
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

// A key that no application acts on. A bare Alt press and release, with no
// other key in between, is the gesture that opens the menu bar of the window
// that has the focus (SC_KEYMENU, Office KeyTips, the Firefox menu bar), so
// every place that swallows or injects Alt without a real key between the two
// events puts this key between them, like AutoHotkey's "menu mask key". The
// mod's own hook skips it because it is marked as injected.
constexpr WORD kMenuMaskVk = 0xE8;  // unassigned virtual key

// Input that this process injects is attributed to this process, which then
// has the right to change the foreground window. The injected Alt press has
// the mask key between the press and the release, so the window that receives
// it doesn't take it for the gesture that opens its menu.
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

// Called as soon as the mod swallows the key that starts a session (the first
// Tab, Alt+` or the fallback hotkey). The foreground window then sees Alt down,
// the mask key and, later, Alt up, instead of a bare Alt press and release, so
// ending the session without a window switch (Esc, the current window picked,
// or a single open window) never opens the menu bar of the active window.
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

    // Last resort, and the one the shell's own switcher uses internally.
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
    if (!g_session.active) {
        return;
    }

    HWND target = nullptr;
    const HWND foreground = GetForegroundWindow();
    if (g_session.selectedIndex >= 0 &&
        g_session.selectedIndex < static_cast<int>(g_session.items.size())) {
        target = g_session.items[g_session.selectedIndex].hwnd;
    }

    // The overlay is hidden before the target is activated, so that the
    // topmost overlay can't interfere with the activation.
    EndSession();

    if (!target || target == foreground || !IsWindow(target)) {
        // The active window was selected, or nothing was selected: the
        // original switcher just closes in that case.
        return;
    }

    ActivateWindow(target);
}

static void CloseSelectedWindow() {
    if (!g_session.active || g_session.selectedIndex < 0 ||
        g_session.selectedIndex >=
            static_cast<int>(g_session.items.size())) {
        return;
    }

    const HWND target = g_session.items[g_session.selectedIndex].hwnd;
    if (target && IsWindow(target)) {
        PostMessageW(target, WM_SYSCOMMAND, SC_CLOSE, 0);
    }

    // The window list is rebuilt shortly after, so that the closing window
    // disappears from the switcher.
    SetTimer(g_session.hwnd, kRebuildTimerId, kRebuildDelayMs, nullptr);
}

static void RebuildSessionItems() {
    if (!g_session.active) {
        return;
    }

    const HWND keep = (g_session.selectedIndex >= 0 &&
                       g_session.selectedIndex <
                           static_cast<int>(g_session.items.size()))
        ? g_session.items[g_session.selectedIndex].hwnd
        : nullptr;
    const int previousIndex = g_session.selectedIndex;
    const Settings settings = g_session.settings;
    const std::wstring appFilter = g_session.appPathFilter;

    std::vector<WindowItem> items;
    BuildWindowList(settings, appFilter, &items);

    for (WindowItem& item : g_session.items) {
        item.ReleaseThumbnail();
    }
    g_session.items = std::move(items);

    if (g_session.items.empty()) {
        EndSession();
        return;
    }

    int index = -1;
    for (int i = 0; i < static_cast<int>(g_session.items.size()); ++i) {
        if (g_session.items[i].hwnd == keep) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        index = std::clamp(previousIndex, 0,
                           static_cast<int>(g_session.items.size()) - 1);
    }

    g_session.selectedIndex = index;
    g_session.hoverIndex = -1;
    RecomputeLayoutAndRects();
    PaintFrame();
    RefreshThumbnails();
    PresentFrame();
}

// -----------------------------------------------------------------------------
// Key requests coming from the keyboard hook thread
// -----------------------------------------------------------------------------

static void OnShowRequest(bool previous) {
    if (!g_session.active) {
        const Settings settings = GetSettingsCopy();
        bool foregroundFound = false;
        const SessionResult result =
            BeginSession(settings, false, &foregroundFound);

        if (result == SessionResult::NothingToShow ||
            result == SessionResult::Ok) {
            // The Tab of this Alt press was swallowed, whatever happens next.
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

        // The switcher couldn't be shown. The key press that was swallowed is
        // passed on to the system, so that the stock switcher still handles
        // it and Alt+Tab never ends up doing nothing.
        EndSession();
        SendAltTabToSystem();
        return;
    }

    MoveSelection(previous ? -1 : 1);
}

static void OnSameAppRequest() {
    if (g_session.active) {
        EndSession();
        return;
    }

    const Settings settings = GetSettingsCopy();
    bool foregroundFound = false;
    const SessionResult result =
        BeginSession(settings, true, &foregroundFound);
    // The Alt+` of this Alt press was swallowed, whatever happens next.
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
    if (g_session.active) {
        EndSession();
        return;
    }

    OnShowRequest(false);
}

// The safety net for the case where the key release was lost or the keyboard
// hook was dropped: the switcher must never stay on screen forever.
static void OnSessionHealthCheck() {
    if (!g_session.visible) {
        return;
    }

    if (!g_altDown.load(std::memory_order_relaxed)) {
        CommitSession();
        return;
    }

    if (GetTickCount64() - g_session.startedTick > kSessionStuckLimitMs) {
        Wh_Log(L"The switcher has been open for too long with Alt held, and the keyboard hook has been silent for %llu ms; closing the switcher and reinstalling the hook",
               GetTickCount64() - g_hookCallTick.load());
        g_hookReinstallRequested.store(true, std::memory_order_release);
        EndSession();
    }
}

static void OnSettingsChanged() {
    const Settings settings = GetSettingsCopy();
    UpdateHookFlags(settings);
    if (g_session.active) {
        EndSession();
    }
    Wh_Log(L"Settings applied");
}

// -----------------------------------------------------------------------------
// Mouse handling
// -----------------------------------------------------------------------------

static void OnMouseMove(int x, int y) {
    if (!g_session.visible || !g_session.settings.mouseNavigation) {
        return;
    }

    const int hovered = IndexAtPoint(x, y);
    if (hovered == g_session.hoverIndex) {
        return;
    }

    g_session.hoverIndex = hovered;
    PaintAndPresent();
}

static void OnMouseClick(int x, int y) {
    if (!g_session.visible || !g_session.settings.mouseNavigation) {
        return;
    }

    const int index = IndexAtPoint(x, y);
    if (index < 0) {
        return;
    }

    SelectIndex(index);
    CommitSession();
}

static void OnMouseWheel(int delta) {
    if (!g_session.visible || !g_session.settings.mouseNavigation ||
        delta == 0) {
        return;
    }

    MoveSelection(delta > 0 ? -1 : 1);
}

// -----------------------------------------------------------------------------
// The overlay window
// -----------------------------------------------------------------------------

// Handles one message of the overlay window. Returns true when the message was
// answered, in which case *outResult carries the result to return.
static bool HandleOverlayMessage(HWND hwnd, UINT message, WPARAM wParam,
                                 LPARAM lParam, LRESULT* outResult) {
    switch (message) {
        case WM_ERASEBKGND:
            *outResult = 1;
            return true;

        case WM_PAINT: {
            PAINTSTRUCT paint{};
            HDC dc = BeginPaint(hwnd, &paint);
            if (dc && g_session.frame) {
                BitBlt(dc, 0, 0, g_session.frame.width, g_session.frame.height,
                       g_session.frame.dc.value, 0, 0, SRCCOPY);
            }
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
            EndSession();
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
            // The monitor layout changed while the switcher was open.
            EndSession();
            *outResult = 0;
            return true;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            *outResult = 0;
            return true;

        case WM_DESTROY:
            g_overlayWindow.store(nullptr, std::memory_order_release);
            PostQuitMessage(0);
            *outResult = 0;
            return true;

        default:
            break;
    }

    return false;
}

// The window procedure is called by the system, so it is one of the entry
// points that must never let an exception through. A failure closes the
// switcher: an overlay left on screen with a broken state would keep Alt+Tab
// captured, while closing it hands Alt+Tab back to the stock switcher.
static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT message, WPARAM wParam,
                                       LPARAM lParam) {
    LRESULT result = 0;
    bool handled = false;

    const bool ok = RunGuarded(L"handling an overlay window message", [&] {
        handled = HandleOverlayMessage(hwnd, message, wParam, lParam, &result);
    });

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
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
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

    // The window is topmost, so that it covers the taskbar, and it never
    // activates itself and never appears in the switcher's own list or in
    // Task View: it is a tool window.
    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kOverlayClassName, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
        instance, nullptr);

    if (!hwnd) {
        Wh_Log(L"The overlay window couldn't be created (error %lu)",
               GetLastError());
        return false;
    }

    g_session.hwnd = hwnd;
    g_overlayWindow.store(hwnd, std::memory_order_release);
    return true;
}

// -----------------------------------------------------------------------------
// The keyboard hook
//
// The hook only classifies key events and posts messages to the UI thread, so
// its callback finishes in microseconds and Windows never drops it for being
// slow. It is installed by the mod's own thread, in the mod's own process:
// nothing is injected anywhere.
//
// Plain Alt+Tab and Alt+Shift+Tab are swallowed and answered by this mod, and
// so are the keys used while the switcher is open. The modifier keys
// themselves always pass through, which keeps the keyboard state of the
// applications, including their Alt menus and Shift states, exactly as it
// would be without the mod.
// -----------------------------------------------------------------------------

static void PostToOverlay(UINT message, WPARAM wParam) {
    HWND hwnd = g_overlayWindow.load(std::memory_order_acquire);
    if (hwnd) {
        PostMessageW(hwnd, message, wParam, 0);
    }
}

static void UpdateHookFlags(const Settings& settings) {
    g_sameAppHotkeyEnabled.store(settings.sameAppHotkey,
                                 std::memory_order_relaxed);
    g_fallbackHotkeyEnabled.store(settings.fallbackHotkey,
                                  std::memory_order_relaxed);
}

// The safety net that keeps a failure of this mod from breaking Alt+Tab: the
// user is still holding Alt, so passing a Tab press on to the system lets the
// stock switcher answer this press. The mod's own hook skips injected events,
// so the press can't come back to this mod.
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

    // Modifier tracking makes the decision independent of the thread message
    // queue state, and the modifiers are never swallowed.
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
                // The user switched to another Alt-based gesture: the switcher
                // is closed and the keys pass through, so that Task View, for
                // example, opens on the first try.
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

    // While a key is on its way to a gesture that belongs to Windows, every
    // key passes through until that key is released.
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
        // Ctrl+Shift+Esc must always reach the system.
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
                // Alt+F4 closes the selected window, like the original.
                if (isDown && alt) {
                    PostToOverlay(kMsgSwitchCloseSelected, 0);
                }
                return true;

            default:
                break;
        }

        // Everything else is swallowed while the switcher is open, which is
        // what the original does, so typing can't reach the windows behind it.
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
            // The release that matches the press that was swallowed.
            return true;
        }

        return false;
    }

    if (vk == VK_OEM_3 && alt && isDown &&
        g_sameAppHotkeyEnabled.load(std::memory_order_relaxed)) {
        PostToOverlay(kMsgSameApp, 0);
        return true;
    }

    if (vk == static_cast<DWORD>(kFallbackHotkeyVk) && ctrl && alt && isDown &&
        g_fallbackHotkeyEnabled.load(std::memory_order_relaxed)) {
        PostToOverlay(kMsgFallbackToggle, 0);
        return true;
    }

    return false;
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

    try {
        if (HandleKeyEvent(info->vkCode, isDown, isUp)) {
            return 1;
        }
    } catch (...) {
        // The hook must never throw into the system.
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

// The hook procedure lives in the mod's own DLL, so the module handle that
// SetWindowsHookEx needs is the one of that DLL.
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

    // Seed the modifier state from the system, which matters only when the
    // mod is loaded while Alt is already held; the first key event then keeps
    // the state exact.
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
            // Without its window the mod can't show anything while it still
            // takes over Alt+Tab. Exiting restores the stock switcher, which
            // is better than a mod that swallows a key and shows nothing.
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

// The switcher works in physical pixels, so the tool process must not have its
// coordinates virtualized by the DPI subsystem.
static void MakeProcessDpiAware() {
    // A handle typed function pointer instead of the DPI_AWARENESS_CONTEXT
    // type, so that the mod compiles with every SDK version.
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

    // The apartment is closed by the guard when this function returns, on
    // every path, including the early return taken when the overlay window
    // can't be created.
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
        DestroyWindow(g_session.hwnd);
        g_session.hwnd = nullptr;
        SetEvent(g_uiReadyEvent);
        return 1;
    }

    g_watchdogThread = CreateThread(nullptr, 0, WatchdogThreadProc, nullptr, 0,
                                    nullptr);
    if (!g_watchdogThread) {
        // The watchdog is a safety net, not a requirement.
        Wh_Log(L"The watchdog thread couldn't be started");
    }

    SetEvent(g_uiReadyEvent);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        // The window procedure guards itself; this is the backstop for the
        // message loop itself, so the UI thread can't die on a failure of the
        // dispatch machinery.
        if (!RunGuarded(L"dispatching a message", [&] {
                DispatchMessageW(&msg);
            })) {
            EndSession();
        }
    }

    // Teardown: release everything the mod owns, then let the threads end.
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

    g_titleFont.reset();
    g_titleFontDpi = 0;
    g_solidColor.reset();

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

    // Loading the settings allocates, and the two calls below are the only
    // ones that run before the mod has a fallback: a failure there leaves the
    // stock switcher in place, because no hook has been installed yet.
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
    // This callback is called by the Windhawk engine, so it is guarded as well.
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
