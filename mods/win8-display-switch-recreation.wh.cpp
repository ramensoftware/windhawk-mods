// ==WindhawkMod==
// @id              win8-display-switch-recreation
// @name            Windows 8/8.1 Display Switch Recreation
// @description     This mod recreates the Windows 8/8.1 Display Switch (Second screen / Project) pane on Windows 10 and Windows 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         GPL-3.0
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32 -lshell32 -lole32 -ldwmapi -lshcore -lshlwapi -lcrypt32 -lwindowscodecs -lmsimg32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 8/8.1 Display Switch Recreation

> **IMPORTANT:** This modification runs in `explorer.exe` and does not require adding the critical
> `dwm.exe` process to Windhawk's inclusion list. When an elevated window or a secure desktop is
> active, Windows may not deliver keyboard input to Explorer; in that case the native Project
> flyout remains available.

This mod recreates the Windows 8/8.1 display switch panel (the **Second screen** flyout in
Windows 8 and the **Project** pane in Windows 8.1) on Windows 10 and Windows 11. It uses the
real Windows display configuration and does not replace system files, patch the graphics driver,
or modify native display binaries. Topology changes and **Set as main display** use documented
Windows APIs and persist like the native Project flyout. This modification has been tested on Windows 10 21H2 and Windows 11 24H2.

## Screenshot

![Windows 8/8.1 Display Switch screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win8display.png)

## Features

- Four native display modes: **PC screen only**, **Duplicate**, **Extend**, and **Second screen only**.
- Live display state with monitor hotplug and Settings-app changes reflected while the panel is open.
- Windows 8.1-style layout, embedded icons, accent color, per-monitor DPI support, and translations
  for English, Italian, Spanish, French, Portuguese, German, Russian, Simplified Chinese, Japanese,
  and Korean.
- Right-click commands: **Set as main display**, **Detect displays**, **More settings**, and
  **Copy monitor information**.
- Keyboard controls: `Win+P` to open or toggle, `Left`/`Right`/`Up`/`Down` to select, `Enter` or
  `Space` to apply, `1`-`4` for direct selection, `F5` to detect displays, and `Esc` to dismiss.
- While the panel is open, repeated `Win+P` presses cycle through the modes; releasing Win applies
  the selected mode and closes the panel.
- Click-outside and foreground-change dismissal, plus a status line for display API errors.
- The Windows 8/8.1 **Devices > Second screen/Project** route and a no-argument
  `DisplaySwitch.exe` launch replace the native `DisplaySwitchUIWnd`; direct commands such as
  `DisplaySwitch.exe /extend` remain untouched.

## Win+P input path

When enabled, the mod installs one narrow `WH_KEYBOARD_LL` hook in the main `explorer.exe` process.
It consumes only the physical `Win+P` sequence, passes injected input and every other key through,
and sends private messages to the panel. The hook is removed when the setting is disabled or the mod
is unloaded. If an elevated window or a secure desktop is active, the hook may not receive the input;
the native Windows Project flyout is intentionally preserved in that case.

The panel loads only in the Explorer process that owns the shell window, so separate folder-window
processes do not create duplicate panels.

## Windows APIs

The panel uses documented public APIs:

| Purpose | API |
| --- | --- |
| Read paths, topology, and monitor names | `QueryDisplayConfig`, `GetDisplayConfigBufferSizes`, `DisplayConfigGetDeviceInfo` |
| Apply or validate a topology | `SetDisplayConfig` with `SDC_APPLY`, `SDC_VALIDATE`, and the topology flags |
| Detect displays | `SetDisplayConfig` with `SDC_USE_DATABASE_CURRENT` |
| Set the primary display | `ChangeDisplaySettingsEx` with `CDS_SET_PRIMARY` and `CDS_UPDATEREGISTRY` |
| Monitor information and DPI | `EnumDisplayMonitors`, `GetMonitorInfo`, `EnumDisplaySettingsEx`, `GetDpiForMonitor` |
| Other display settings | `ms-settings:display` deep links |
| Replace another native Project flyout | `SetWinEventHook(EVENT_SYSTEM_FOREGROUND)` and `DisplaySwitchUIWnd` detection |
| Error reporting | documented return codes and `FormatMessage` |

The mod does not write arbitrary registry values, call driver-private interfaces, launch
`DisplaySwitch.exe`, or undo user-requested display changes when disabled. `ERROR_ACCESS_DENIED`
is shown in the panel when the current session cannot change the display configuration.

## Settings

- **Open with Win+P**: replace the native Project flyout and supported native entry points.
- **Panel color**: accent, Charcoal, or Custom color.
- **Full-height pane**: use the Windows 8.1 full-height layout or fit the content.
- **Validate before applying**: validate the topology with `SDC_VALIDATE` before applying it.
- **Allow path persistence**: add `SDC_PATH_PERSIST_IF_REQUIRED` when applying a topology.
- **Disable unavailable options**: gray out modes that require more than one connected display.

## Limitations

- The mod cannot make a topology work when the display driver does not support it.
- Wireless-display discovery remains part of the native Windows Connect experience.
- Resolution, refresh rate, orientation, HDR, and the embedded artwork are not re-created or changed.

## Credits

- **AdministratoX** — Metro UI interface template.
- **Meteoni** — `IsMainExplorerProcess` pattern.
- The Windows 8/8.1 Search Charm Recreation mod — panel, animation, light dismiss, z-band,
  context-menu, and translation components.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- useWinP: true
  $name: Open with Win+P
  $description: This setting replaces Win+P through the Explorer keyboard hook and other native Project flyout entry points with this panel, including a DisplaySwitchUIWnd opened by the Windows 8/8.1 Devices route.
- language: auto
  $name: Language
  $description: This setting modifies the UI language for the display switch panel. Automatic follows the Windows display language.
  $options:
    - auto: Automatic (Windows display language)
    - en: English
    - it: Italiano
    - es: Español
    - fr: Français
    - pt: Português
    - de: Deutsch
    - ru: Русский
    - zh: 简体中文
    - ja: 日本語
    - ko: 한국어
- paneTitle: project
  $name: Panel title
  $description: Windows 8 called this flyout "Second screen", Windows 8.1 renamed it to "Project".
  $options:
    - project: Project (Windows 8.1)
    - secondScreen: Second screen (Windows 8)
- colorMode: accent
  $name: Panel color
  $description: Windows 8.1 painted the Project flyout with the accent color, so that is the default. The other two choices are for a flat background.
  $options:
    - accent: Accent color (Windows 8.1)
    - charcoal: Charcoal
    - custom: Custom color
- customColor: "#1D1D1D"
  $name: Custom color (hexadecimal)
  $description: This setting is used only when Custom color is selected. For example, 087900 is the green of the search charm mod.
- fullHeight: true
  $name: Full-height pane
  $description: The Windows 8.1 pane spans the whole right edge of the screen. Turn this off to end the panel right below the fourth option.
- moreSettings: display
  $name: More settings target
  $description: The page opened by the More settings command of the context menu, one of the documented ms-settings deep links of the Settings app.
  $options:
    - display: Display settings
    - advanced: Advanced display settings
    - graphics: Graphics settings
- validateFirst: true
  $name: Validate before applying
  $description: This setting makes the panel call SetDisplayConfig with SDC_VALIDATE first, so an unsupported topology is refused without changing the desktop.
- persistPaths: false
  $name: Allow path persistence
  $description: This setting adds SDC_PATH_PERSIST_IF_REQUIRED, so the system can force path persistence on a target to satisfy the request.
- disableUnavailable: false
  $name: Disable unavailable options
  $description: This setting dims Duplicate, Extend and Second screen only when a single display is connected. When disabled, those options stay clickable and the system decides.
- allowSetPrimary: true
  $name: Allow "Set as main display"
  $description: This setting adds a "Set as main display" command for each connected display to the context menu, implemented with ChangeDisplaySettingsEx and CDS_SET_PRIMARY.
- openMs: 300
  $name: Opening duration (ms)
  $description: Duration of the panel opening animation in milliseconds.
- closeMs: 250
  $name: Closing duration (ms)
  $description: This setting modifies the duration of the panel closing animation in milliseconds.
- closeAfterApply: true
  $name: Close after applying
  $description: Windows 8/8.1 dismissed the pane as soon as a projection mode was chosen. Turn this off to keep it open after applying.
- closeOnClickOutside: true
  $name: Close when clicking outside
  $description: This setting allows to close the panel when it loses activation, such as after clicking another window.
- aboveTaskbar: true
  $name: Keep above the taskbar
  $description: This setting allows to maintain the panel above the taskbar in Z-order.
- useZBand: true
  $name: Use higher z-band (experimental)
  $description: This setting allows to place the panel in the UIAccess z-band (like Task Manager) so it stays above the taskbar, Start menu and fullscreen UWP apps. Requires CreateWindowInBand; falls back to ordinary topmost if unavailable.
- applyCooldownMs: 700
  $name: Apply cooldown (ms)
  $description: This setting changes the minimum time between two display configuration changes, so a burst of key presses cannot queue several topology changes.
- hotkeyCooldownMs: 400
  $name: Hotkey cooldown (ms)
  $description: This setting changes the minimum time between two Win+P presses to prevent multiple openings.
- monitor: 0
  $name: Monitor
  $description: 0 = monitor under the cursor, 1 = first monitor, 2 = second monitor, and so on.
- debugLog: false
  $name: Debug log
  $description: This setting allows the mod to write z-order and display configuration diagnostics to the Windhawk log (does not change how anything looks).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <objbase.h>
#include <shellscalingapi.h>
#include <shlwapi.h>
#include <wincrypt.h>
#include <initguid.h>
#include <wincodec.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <strsafe.h>
#include <atomic>

extern "C" IMAGE_DOS_HEADER __ImageBase;

// ---------------------------------------------------------------- RAII / utility
class SrwGuard {
    SRWLOCK* l_; bool excl_;
public:
    SrwGuard(SRWLOCK* l, bool exclusive = false) : l_(l), excl_(exclusive) {
        if (excl_) AcquireSRWLockExclusive(l_); else AcquireSRWLockShared(l_);
    }
    ~SrwGuard() { if (excl_) ReleaseSRWLockExclusive(l_); else ReleaseSRWLockShared(l_); }
    SrwGuard(const SrwGuard&) = delete; SrwGuard& operator=(const SrwGuard&) = delete;
};

template <typename T> class GdiObj {
    T h_;
public:
    explicit GdiObj(T h = NULL) : h_(h) {}
    ~GdiObj() { if (h_) DeleteObject(h_); }
    GdiObj(const GdiObj&) = delete; GdiObj& operator=(const GdiObj&) = delete;
    GdiObj(GdiObj&& o) noexcept : h_(o.h_) { o.h_ = NULL; }
    GdiObj& operator=(GdiObj&& o) noexcept { if (this != &o) { if (h_) DeleteObject(h_); h_ = o.h_; o.h_ = NULL; } return *this; }
    T get() const { return h_; } operator T() const { return h_; }
    T release() { T t = h_; h_ = NULL; return t; }
    void reset(T h = NULL) { if (h_) DeleteObject(h_); h_ = h; }
};

class WinHandle {
    HANDLE h_;
public:
    explicit WinHandle(HANDLE h = NULL) : h_(h) {}
    ~WinHandle() { if (h_) CloseHandle(h_); }
    WinHandle(const WinHandle&) = delete; WinHandle& operator=(const WinHandle&) = delete;
    WinHandle(WinHandle&& o) noexcept : h_(o.h_) { o.h_ = NULL; }
    WinHandle& operator=(WinHandle&& o) noexcept { if (this != &o) { if (h_) CloseHandle(h_); h_ = o.h_; o.h_ = NULL; } return *this; }
    HANDLE get() const { return h_; } HANDLE release() { HANDLE t = h_; h_ = NULL; return t; }
    void reset(HANDLE h = NULL) { if (h_) CloseHandle(h_); h_ = h; }
};

class DcObj {
    HDC h_;
public:
    explicit DcObj(HDC h = NULL) : h_(h) {}
    ~DcObj() { if (h_) DeleteDC(h_); }
    DcObj(const DcObj&) = delete; DcObj& operator=(const DcObj&) = delete;
    DcObj(DcObj&& o) noexcept : h_(o.h_) { o.h_ = NULL; }
    DcObj& operator=(DcObj&& o) noexcept { if (this != &o) { if (h_) DeleteDC(h_); h_ = o.h_; o.h_ = NULL; } return *this; }
    HDC get() const { return h_; } operator HDC() const { return h_; }
    HDC release() { HDC t = h_; h_ = NULL; return t; }
};

class ScopedWindowDc {
    HWND wnd_;
    HDC dc_;
public:
    explicit ScopedWindowDc(HWND wnd = NULL) : wnd_(wnd), dc_(GetDC(wnd)) {}
    ~ScopedWindowDc() { if (dc_) ReleaseDC(wnd_, dc_); }
    ScopedWindowDc(const ScopedWindowDc&) = delete;
    ScopedWindowDc& operator=(const ScopedWindowDc&) = delete;
    HDC get() const { return dc_; }
    operator HDC() const { return dc_; }
};

class ScopedPaint {
    HWND wnd_;
    PAINTSTRUCT ps_{};
    HDC dc_;
public:
    explicit ScopedPaint(HWND wnd) : wnd_(wnd), dc_(BeginPaint(wnd, &ps_)) {}
    ~ScopedPaint() { if (wnd_) EndPaint(wnd_, &ps_); }
    ScopedPaint(const ScopedPaint&) = delete;
    ScopedPaint& operator=(const ScopedPaint&) = delete;
    HDC get() const { return dc_; }
    operator HDC() const { return dc_; }
};

class ScopedClipRgn {
    HDC dc_;
public:
    ScopedClipRgn(HDC dc, int left, int top, int right, int bottom) : dc_(dc) {
        if (dc_) IntersectClipRect(dc_, left, top, right, bottom);
    }
    ~ScopedClipRgn() { if (dc_) SelectClipRgn(dc_, NULL); }
    ScopedClipRgn(const ScopedClipRgn&) = delete;
    ScopedClipRgn& operator=(const ScopedClipRgn&) = delete;
};

template <typename T>
class ScopedComPtr {
    T* p_ = nullptr;
public:
    ScopedComPtr() = default;
    explicit ScopedComPtr(T* p) : p_(p) {}
    ~ScopedComPtr() { if (p_) p_->Release(); }
    ScopedComPtr(const ScopedComPtr&) = delete;
    ScopedComPtr& operator=(const ScopedComPtr&) = delete;
    ScopedComPtr(ScopedComPtr&& o) noexcept : p_(o.p_) { o.p_ = nullptr; }
    ScopedComPtr& operator=(ScopedComPtr&& o) noexcept {
        if (this != &o) { if (p_) p_->Release(); p_ = o.p_; o.p_ = nullptr; }
        return *this;
    }
    T* get() const { return p_; }
    T* operator->() const { return p_; }
    explicit operator bool() const { return p_ != nullptr; }
    T** put() { if (p_) { p_->Release(); p_ = nullptr; } return &p_; }
};

class ScopedWhString {
    PCWSTR str_ = nullptr;
public:
    explicit ScopedWhString(PCWSTR settingName) : str_(Wh_GetStringSetting(settingName)) {}
    ~ScopedWhString() { if (str_) Wh_FreeStringSetting(str_); }
    ScopedWhString(const ScopedWhString&) = delete;
    ScopedWhString& operator=(const ScopedWhString&) = delete;
    PCWSTR get() const { return str_; }
    operator PCWSTR() const { return str_; }
};

class ScopedLocalFree {
    HLOCAL mem_ = NULL;
public:
    explicit ScopedLocalFree(HLOCAL mem = NULL) : mem_(mem) {}
    ~ScopedLocalFree() { if (mem_) LocalFree(mem_); }
    ScopedLocalFree(const ScopedLocalFree&) = delete;
    ScopedLocalFree& operator=(const ScopedLocalFree&) = delete;
    HLOCAL* put() { if (mem_) { LocalFree(mem_); mem_ = NULL; } return &mem_; }
    wchar_t* asWstr() const { return (wchar_t*)mem_; }
};

class ScopedClipboard {
    bool open_ = false;
public:
    explicit ScopedClipboard(HWND owner) : open_(OpenClipboard(owner) != FALSE) {}
    ~ScopedClipboard() { if (open_) CloseClipboard(); }
    ScopedClipboard(const ScopedClipboard&) = delete;
    ScopedClipboard& operator=(const ScopedClipboard&) = delete;
    bool isOpen() const { return open_; }
};

class ScopedGlobalAlloc {
    HGLOBAL h_ = NULL;
public:
    explicit ScopedGlobalAlloc(SIZE_T bytes, UINT flags = GMEM_MOVEABLE)
        : h_(GlobalAlloc(flags, bytes)) {}
    ~ScopedGlobalAlloc() { if (h_) GlobalFree(h_); }
    ScopedGlobalAlloc(const ScopedGlobalAlloc&) = delete;
    ScopedGlobalAlloc& operator=(const ScopedGlobalAlloc&) = delete;
    HGLOBAL get() const { return h_; }
    HGLOBAL release() { HGLOBAL t = h_; h_ = NULL; return t; }
};

class ScopedGlobalLock {
    HGLOBAL h_ = NULL;
    void* ptr_ = nullptr;
public:
    explicit ScopedGlobalLock(HGLOBAL h) : h_(h), ptr_(h ? GlobalLock(h) : nullptr) {}
    ~ScopedGlobalLock() { if (ptr_) GlobalUnlock(h_); }
    ScopedGlobalLock(const ScopedGlobalLock&) = delete;
    ScopedGlobalLock& operator=(const ScopedGlobalLock&) = delete;
    void* get() const { return ptr_; }
};

class ScopedTimer {
    HWND wnd_ = NULL;
    UINT_PTR id_ = 0;
public:
    ScopedTimer(HWND wnd, UINT_PTR id, UINT elapse)
        : wnd_(wnd), id_(SetTimer(wnd, id, elapse, NULL)) {}
    ~ScopedTimer() { if (id_) KillTimer(wnd_, id_); }
    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
};

class ScopedHook {
    HHOOK h_ = NULL;
public:
    explicit ScopedHook(HHOOK h = NULL) : h_(h) {}
    ~ScopedHook() { if (h_) UnhookWindowsHookEx(h_); }
    ScopedHook(const ScopedHook&) = delete;
    ScopedHook& operator=(const ScopedHook&) = delete;
    ScopedHook(ScopedHook&& o) noexcept : h_(o.h_) { o.h_ = NULL; }
    ScopedHook& operator=(ScopedHook&& o) noexcept {
        if (this != &o) { if (h_) UnhookWindowsHookEx(h_); h_ = o.h_; o.h_ = NULL; }
        return *this;
    }
    HHOOK get() const { return h_; }
    operator HHOOK() const { return h_; }
    void reset(HHOOK h = NULL) {
        HHOOK old = h_;
        h_ = h;
        if (old) UnhookWindowsHookEx(old);
    }
};

class ScopedWinEventHook {
    HWINEVENTHOOK h_ = NULL;
public:
    explicit ScopedWinEventHook(HWINEVENTHOOK h = NULL) : h_(h) {}
    ~ScopedWinEventHook() { if (h_) UnhookWinEvent(h_); }
    ScopedWinEventHook(const ScopedWinEventHook&) = delete;
    ScopedWinEventHook& operator=(const ScopedWinEventHook&) = delete;
    HWINEVENTHOOK get() const { return h_; }
    operator HWINEVENTHOOK() const { return h_; }
    void reset(HWINEVENTHOOK h = NULL) {
        HWINEVENTHOOK old = h_;
        h_ = h;
        if (old) UnhookWinEvent(old);
    }
};

struct ScopedSelectedObject {
    HDC dc = nullptr;
    HGDIOBJ previous = nullptr;
    ScopedSelectedObject(HDC target, HGDIOBJ object) : dc(target) {
        if (dc && object) previous = SelectObject(dc, object);
    }
    ~ScopedSelectedObject() {
        if (dc && previous && previous != HGDI_ERROR) SelectObject(dc, previous);
    }
    ScopedSelectedObject(const ScopedSelectedObject&) = delete;
    ScopedSelectedObject& operator=(const ScopedSelectedObject&) = delete;
};

struct ScopedTextColor {
    HDC dc = nullptr;
    COLORREF previous = 0;
    ScopedTextColor(HDC target, COLORREF color) : dc(target) {
        if (dc) previous = SetTextColor(dc, color);
    }
    ~ScopedTextColor() { if (dc) SetTextColor(dc, previous); }
    ScopedTextColor(const ScopedTextColor&) = delete;
    ScopedTextColor& operator=(const ScopedTextColor&) = delete;
};

struct ScopedBkMode {
    HDC dc = nullptr;
    int previous = 0;
    ScopedBkMode(HDC target, int mode) : dc(target) {
        if (dc) previous = SetBkMode(dc, mode);
    }
    ~ScopedBkMode() { if (dc) SetBkMode(dc, previous); }
    ScopedBkMode(const ScopedBkMode&) = delete;
    ScopedBkMode& operator=(const ScopedBkMode&) = delete;
};

struct ScopedComApartment {
    bool initialized = false;
    explicit ScopedComApartment(bool ok) : initialized(ok) {}
    ~ScopedComApartment() { if (initialized) CoUninitialize(); }
    ScopedComApartment(const ScopedComApartment&) = delete;
    ScopedComApartment& operator=(const ScopedComApartment&) = delete;
};

// ---------------------------------------------------------------- exception guard
static std::atomic<unsigned> g_exceptionCount{0};

static void LogCaughtException(const wchar_t* step, const char* what) {
    (void)step;
    (void)what;
    const unsigned count = g_exceptionCount.fetch_add(1) + 1;
    if (count <= 8) {
        if (what && *what)
            Wh_Log(L"[Guard] std::exception while %s: %S", step, what);
        else
            Wh_Log(L"[Guard] unknown exception while %s", step);
    }
}

template <typename Function>
static bool RunGuarded(const wchar_t* step, Function&& function) {
    try {
        function();
        return true;
    } catch (const std::exception& e) {
        LogCaughtException(step, e.what());
        return false;
    } catch (...) {
        LogCaughtException(step, nullptr);
        return false;
    }
}

template <typename T, typename Function>
static T RunGuardedValue(const wchar_t* step, T fallback, Function&& function) {
    try {
        return function();
    } catch (const std::exception& e) {
        LogCaughtException(step, e.what());
        return fallback;
    } catch (...) {
        LogCaughtException(step, nullptr);
        return fallback;
    }
}

struct RateLimitedLog {
    const wchar_t* message;
    int limit;
    int count = 0;
    bool ShouldLog() { ++count; return count <= limit; }
};

// ---------------------------------------------------------------- state
// The four documented projection options of the Windows 8/8.1 pane, in the order
// the flyout shows them. Each one maps to exactly one SDC_TOPOLOGY_xxx flag.
enum Topo {
    TOPO_INTERNAL = 0,   // PC screen only      (DISPLAYCONFIG_TOPOLOGY_INTERNAL)
    TOPO_CLONE = 1,      // Duplicate           (DISPLAYCONFIG_TOPOLOGY_CLONE)
    TOPO_EXTEND = 2,     // Extend              (DISPLAYCONFIG_TOPOLOGY_EXTEND)
    TOPO_EXTERNAL = 3,   // Second screen only  (DISPLAYCONFIG_TOPOLOGY_EXTERNAL)
    TOPO_COUNT = 4,
    TOPO_UNKNOWN = -1
};

// One connected display. Everything here comes from a documented API: the name
// from DISPLAYCONFIG_TARGET_DEVICE_NAME, the GDI device name from
// DISPLAYCONFIG_SOURCE_DEVICE_NAME, the mode from EnumDisplaySettingsEx, the
// rectangle and primary flag from GetMonitorInfo, the scale from
// GetDpiForMonitor. A row can be connected but inactive (for example the
// built-in panel while "Second screen only" is active).
struct MonRow {
    std::wstring name;
    std::wstring gdi;
    LUID adapter = {};
    UINT32 targetId = 0;
    UINT32 sourceId = 0;
    int width = 0, height = 0, refresh = 0;
    int scale = 100;
    int x = 0, y = 0;
    bool active = false;
    bool primary = false;
};

// Layout struct: needs to be fully defined before the g_cachedLay global.
// Every value is a Windows 8.1 measurement from the capture, in physical
// pixels at 96 DPI, scaled by S() for the monitor the pane is shown on.
struct Lay {
    int mx;              // left margin of the title, the tagline and the icons
    int titleInk;        // ink top of the title
    int subInk;          // ink top of the tagline
    int listY;           // top of the first option row
    int rowH;            // row height, and the height of the selection band
    int glyphX, glyphY;  // icon position inside the row
    int textX;           // left edge of the option label
    int rowInk;          // ink top of the option label, relative to the row
    int border;          // thickness of the white frame of the selection band
    int bandX;           // left inset of the selection band
};

static const UINT WM_APP_TOGGLE       = WM_APP + 1;
static const UINT WM_APP_CYCLE        = WM_APP + 3;   // Win+P pressed while open
static const UINT WM_APP_COMMIT       = WM_APP + 4;   // Win released after cycling
// Posted by the low-level mouse hook for every button press while the panel is
// visible. wParam/lParam carry the screen point (per-monitor-aware coordinates).
static const UINT WM_APP_POINTER_DOWN = WM_APP + 2;
// Thread messages for the input-hook thread (msg.hwnd == NULL).
static const UINT WM_HOOKCTL_MOUSE_ON     = WM_APP + 20;
static const UINT WM_HOOKCTL_MOUSE_OFF    = WM_APP + 21;
static const UINT WM_HOOKCTL_KEYBOARD_ON  = WM_APP + 22;
static const UINT WM_HOOKCTL_KEYBOARD_OFF = WM_APP + 23;
static const wchar_t* kClass = L"Win81DisplaySwitchMod";
static const int k81FlyoutWidth = 346;
static const int k81GlyphWidth = 89, k81GlyphHeight = 45;
static const wchar_t* kMenuClass = L"Win81DisplaySwitchMenu";
static const ULONG_PTR kMagic = 0x44535750;
static const COLORREF KEY = RGB(255, 0, 255);
static const UINT_PTR kStatusMs = 6000;

static std::vector<MonRow> g_monitors;
static SRWLOCK g_lock = SRWLOCK_INIT;
static int g_topology = TOPO_UNKNOWN;
static int g_connected = 0;
static int g_active = 0;
static int g_sel = 0;
static int g_hoverOpt = -1;
static std::wstring g_status;
static DWORD g_statusUntil = 0;
// Bumped by RefreshState whenever the reported state changes; the state
// watchdog uses it to avoid repainting a panel that has nothing new to show.
static volatile unsigned g_stateStamp = 0;

static HWND g_wnd = NULL;
static WinHandle g_uiThread, g_keyboardThread, g_stateThread;
static WinHandle g_stopEvent;
static WinHandle g_uiReady, g_keyboardReady;
static DWORD g_uiTid = 0, g_keyboardTid = 0;
static HHOOK g_mouseHook = NULL;           // owned by the input-hook thread
static HHOOK g_hook = NULL;                // Explorer's Win+P hook, same thread
static bool g_mouseHookWanted = false;     // UI-thread view of the requested state
static bool g_keyboardHookWanted = false;  // Explorer Win+P hook requested state
static bool g_winPConsumed = false;
static bool g_winPSequence = false;
static bool g_winPCycle = false;
static bool g_lwinDown = false;
static bool g_rwinDown = false;
static ScopedWinEventHook g_foregroundHook;    // owned by the UI thread
static DWORD g_openTick = 0;               // GetTickCount() of the last OpenPane
static volatile LONG g_uiStarted = 0, g_keyboardStarted = 0;
static DWORD g_lastApply = 0;

static bool g_useWinP = true;
static int g_titleMode = 0;                // 0 = Project, 1 = Second screen
static int g_colorMode = 0, g_openMs = 300, g_closeMs = 250;
static int g_moreSettings = 0;              // 0 = display, 1 = advanced, 2 = graphics
static COLORREF g_custom = RGB(29, 29, 29);
static int g_lang = 0;
static bool g_fullHeight = true, g_validateFirst = true, g_persistPaths = false;
static bool g_disableUnavailable = false, g_allowSetPrimary = true;
static bool g_closeAfterApply = true;
static bool g_closeOnClickOutside = true, g_aboveTaskbar = true;
static bool g_useZBand = true;
static int g_applyCooldownMs = 700, g_hotkeyCooldownMs = 400;
static int g_monitorChoice = 0;
static bool g_debug = false;

enum { ST_IDLE, ST_IN, ST_OUT };
static int g_state = ST_IDLE;
static bool g_open = false;
static int g_w = 0, g_h = 0;
static double g_scale = 1.0;
static int g_paneOff = 0, g_contOff = 0;
static int g_fromPane, g_toPane, g_fromCont, g_dur, g_contDur;
static LONGLONG g_t0 = 0, g_freq = 1;

static GdiObj<HFONT> g_fTitle, g_fLabel, g_fText;
static double g_fontScale = 0;

// Metro context menu state
enum MenuCmd {
    MC_NONE = 0, MC_SEP, MC_SETMAIN, MC_MORE, MC_COPY, MC_DETECT
};
// Why the menu went away. Only MENU_END_INSIDE gives focus back to the panel.
enum MenuEnd {
    MENU_END_NONE = 0,
    MENU_END_INSIDE,    // command chosen, Esc, or focus moved to the panel itself
    MENU_END_OUTSIDE,   // another window took the focus (click outside)
    MENU_END_CANCEL     // dismissed programmatically (hotkey toggle, panel closing)
};
// `row` is the display a command applies to, -1 when it applies to the pane.
struct MenuEntry { std::wstring text; int cmd; int row; };
static HWND g_menuWnd = NULL;
static bool g_menuClassRegistered = false;
static std::vector<MenuEntry> g_menu;
static int g_menuHover = -1;
static int g_menuResult = MC_NONE;
static int g_menuResultRow = -1;
static int g_menuEndReason = MENU_END_NONE;
static bool g_inMenu = false;

// Layout cache (Lay is fully defined above, so this global is valid)
static Lay g_cachedLay = {};
static int g_cachedLayW = -1;
static int g_cachedLayH = -1;
static double g_cachedLayScale = -1.0;

static int S(int v) { return (int)(v * g_scale + 0.5); }

// ---------------------------------------------------------------- forward declarations
static void EnforceTopmost();
static void OpenPane();
static void ClosePane();
static void DismissContextMenu();
static void RefreshState();
static void ApplyTopology(int which, const wchar_t* origin);
static void DetectDisplays();
static void OpenMoreSettings();
static void SetStatus(const std::wstring& text);
static void InvalidatePanel();
static Lay Layout();
static int OptionAt(int x, int y);
static int PaneContentHeight();
static void ShowContextMenu(HWND h, int x, int y, bool fromKeyboard);
static void SetOutsideClickHook(bool on);
static void SetKeyboardHook(bool on);
static void OnGlobalPointerDown(POINT pt);

// ---------------------------------------------------------------- z-band overlay (from the search charm mod)
typedef HWND (WINAPI *CreateWindowInBand_t)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand);

static CreateWindowInBand_t g_createInBand = NULL;
static int g_band = -1;
static DWORD g_panelBand = 0;   // band the panel window actually lives in (0 = default)
static const DWORD kBandUIAccess = 2;
static const DWORD kBandSystemTools = 16;

static HWND CreateOverlayWindow(const wchar_t* cls, int w, int h, DWORD* bandOut) {
    const DWORD ex = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED;
    HINSTANCE hi = (HINSTANCE)&__ImageBase;
    if (bandOut) *bandOut = 0;

    HWND hw = NULL;
    if (g_createInBand && g_useZBand && g_band != 0) {
        const DWORD bands[2] = { kBandUIAccess, kBandSystemTools };
        for (DWORD b : bands) {
            if (g_band > 0 && (DWORD)g_band != b) continue;
            hw = g_createInBand(ex, cls, L"", WS_POPUP, 0, 0, w, h,
                                NULL, NULL, hi, NULL, b);
            if (hw) {
                if (g_band != (int)b)
                    Wh_Log(L"[ZBand] window created in band %u (above taskbar/Start/fullscreen)",
                           (unsigned)b);
                g_band = (int)b;
                if (bandOut) *bandOut = b;
                break;
            }
            Wh_Log(L"[ZBand] CreateWindowInBand(%u) failed, err=%u",
                   (unsigned)b, (unsigned)GetLastError());
        }
        if (!hw && g_band < 0) {
            g_band = 0;
            Wh_Log(L"[ZBand] no higher band available, falling back to ordinary topmost");
        }
    }
    if (!hw)
        hw = CreateWindowExW(ex, cls, L"", WS_POPUP, 0, 0, w, h, NULL, NULL, hi, NULL);
    return hw;
}

struct EnumCtx { RECT r; int idx; };
static BOOL CALLBACK EnumTop(HWND w, LPARAM lp) {
    return RunGuardedValue<BOOL>(L"EnumTop", TRUE, [&]() -> BOOL {
        EnumCtx* c = (EnumCtx*)lp;
        c->idx++;
        if (!IsWindowVisible(w)) return TRUE;
        RECT wr, x;
        GetWindowRect(w, &wr);
        if (!IntersectRect(&x, &wr, &c->r)) return TRUE;
        wchar_t cls[96] = L"";
        GetClassNameW(w, cls, 96);
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        Wh_Log(L"[dump] z=%d hwnd=%p class=%s pid=%u ex=%08X rect=%d,%d,%d,%d%s",
               c->idx, w, cls, (unsigned)pid,
               (unsigned)GetWindowLongW(w, GWL_EXSTYLE),
               (int)wr.left, (int)wr.top, (int)wr.right, (int)wr.bottom,
               (w == g_wnd) ? L"  <== OUR PANEL" : L"");
        return TRUE;
    });
}

static void DebugDump(const wchar_t* tag) {
    if (!g_debug || !g_wnd) return;
    HWND tray = FindWindowW(L"Shell_TrayWnd", NULL);
    Wh_Log(L"[dump] ---- %s ---- panel=%p band=%d pid=%u tray=%p",
           tag, g_wnd, g_band, (unsigned)GetCurrentProcessId(), tray);
    EnumCtx c;
    RECT r; GetWindowRect(g_wnd, &r);
    c.r = r;
    c.idx = 0;
    Wh_Log(L"[dump] top-level windows intersecting the panel, from the top of the z-order:");
    EnumWindows(EnumTop, (LPARAM)&c);
}

// ---------------------------------------------------------------- utility
static COLORREF Blend(COLORREF bg, int pct) {
    int r = GetRValue(bg), g = GetGValue(bg), b = GetBValue(bg);
    r += (255 - r) * pct / 100; g += (255 - g) * pct / 100; b += (255 - b) * pct / 100;
    return RGB(r, g, b);
}


static COLORREF ParseHex(const wchar_t* t, COLORREF def) {
    if (!t) return def;
    while (*t == L' ') t++;
    if (*t == L'#') t++;
    wchar_t* e = NULL; unsigned long v = wcstoul(t, &e, 16);
    if (e == t) return def;
    return RGB((v >> 16) & 255, (v >> 8) & 255, v & 255);
}

// The system accent color: what Windows 8.1 painted its flyout with, and what
// DwmGetColorizationColor reports on Windows 10 and 11. Falls back to the Metro
// blue (#0074CB) when DWM does not provide a color.
static COLORREF SystemAccentColor() {
    DWORD c = 0; BOOL o = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&c, &o)))
        return RGB((c >> 16) & 255, (c >> 8) & 255, c & 255);
    return RGB(0, 116, 203);
}

static COLORREF PaneColor() {
    if (g_colorMode == 1) return RGB(29, 29, 29);
    if (g_colorMode == 2) return g_custom;
    return SystemAccentColor();
}

static COLORREF PaneBorderColor(COLORREF pane) {
    return RGB(GetRValue(pane) * 78 / 100,
               GetGValue(pane) * 78 / 100,
               GetBValue(pane) * 83 / 100);
}

// The selection band uses a lighter shade of the panel color (such as #008CE0
// on the #0074CB Metro blue panel, or a +18% lighten on any other color).
static COLORREF SelectFillColor(COLORREF pane) {
    const int r = GetRValue(pane), g = GetGValue(pane), b = GetBValue(pane);
    if (r < 16 && b > 120) {
        return RGB(r, std::min(255, g + 24), std::min(255, b + 21));
    }
    return Blend(pane, 18);
}

// The English system message for a Win32 error code (falling back to the
// neutral language only when English message resources are absent), so all
// Windhawk log entries remain in English.
static std::wstring WinErrText(DWORD err) {
    ScopedLocalFree buf;
    DWORD len = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, err, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPWSTR)buf.put(), 0, NULL);
    if (!len || !buf.asWstr()) {
        len = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, err, 0, (LPWSTR)buf.put(), 0, NULL);
    }
    std::wstring out;
    if (len && buf.asWstr()) {
        out.assign(buf.asWstr(), len);
        while (!out.empty() && (out.back() == L'\r' || out.back() == L'\n' ||
                                out.back() == L'.' || out.back() == L' '))
            out.pop_back();
    }
    return out;
}

static void LoadSettings() {
    g_useWinP = Wh_GetIntSetting(L"useWinP") != 0;
    g_openMs = std::min(3000, std::max(80, (int)Wh_GetIntSetting(L"openMs")));
    g_closeMs = std::min(3000, std::max(80, (int)Wh_GetIntSetting(L"closeMs")));
    {
        ScopedWhString cm(L"colorMode");
        g_colorMode = (cm && !wcscmp(cm, L"charcoal")) ? 1 : (cm && !wcscmp(cm, L"custom")) ? 2 : 0;
    }
    {
        ScopedWhString cc(L"customColor");
        g_custom = ParseHex(cc, RGB(29, 29, 29));
    }
    {
        ScopedWhString pt(L"paneTitle");
        g_titleMode = (pt && !wcscmp(pt, L"secondScreen")) ? 1 : 0;
    }
    {
        ScopedWhString ms(L"moreSettings");
        g_moreSettings = (ms && !wcscmp(ms, L"advanced")) ? 1 : (ms && !wcscmp(ms, L"graphics")) ? 2 : 0;
    }
    {
        ScopedWhString lg(L"language");
        if (lg && !wcscmp(lg, L"it")) g_lang = 0;
        else if (lg && !wcscmp(lg, L"en")) g_lang = 1;
        else if (lg && !wcscmp(lg, L"es")) g_lang = 2;
        else if (lg && !wcscmp(lg, L"fr")) g_lang = 3;
        else if (lg && !wcscmp(lg, L"pt")) g_lang = 4;
        else if (lg && !wcscmp(lg, L"de")) g_lang = 5;
        else if (lg && !wcscmp(lg, L"ru")) g_lang = 6;
        else if (lg && !wcscmp(lg, L"zh")) g_lang = 7;
        else if (lg && !wcscmp(lg, L"ja")) g_lang = 8;
        else if (lg && !wcscmp(lg, L"ko")) g_lang = 9;
        else {
            LANGID lid = GetUserDefaultUILanguage();
            switch (PRIMARYLANGID(lid)) {
                case LANG_ITALIAN: g_lang = 0; break;
                case LANG_SPANISH: g_lang = 2; break;
                case LANG_FRENCH: g_lang = 3; break;
                case LANG_PORTUGUESE: g_lang = 4; break;
                case LANG_GERMAN: g_lang = 5; break;
                case LANG_RUSSIAN: g_lang = 6; break;
                case LANG_CHINESE: g_lang = 7; break;
                case LANG_JAPANESE: g_lang = 8; break;
                case LANG_KOREAN: g_lang = 9; break;
                default: g_lang = 1; break;
            }
        }
    }
    g_fullHeight = Wh_GetIntSetting(L"fullHeight") != 0;
    g_validateFirst = Wh_GetIntSetting(L"validateFirst") != 0;
    g_persistPaths = Wh_GetIntSetting(L"persistPaths") != 0;
    g_disableUnavailable = Wh_GetIntSetting(L"disableUnavailable") != 0;
    g_allowSetPrimary = Wh_GetIntSetting(L"allowSetPrimary") != 0;
    g_closeAfterApply = Wh_GetIntSetting(L"closeAfterApply") != 0;
    g_closeOnClickOutside = Wh_GetIntSetting(L"closeOnClickOutside") != 0;
    g_aboveTaskbar = Wh_GetIntSetting(L"aboveTaskbar") != 0;
    g_useZBand = Wh_GetIntSetting(L"useZBand") != 0;
    g_applyCooldownMs = std::min(5000, std::max(0, (int)Wh_GetIntSetting(L"applyCooldownMs")));
    g_hotkeyCooldownMs = std::min(2000, std::max(0, (int)Wh_GetIntSetting(L"hotkeyCooldownMs")));
    g_monitorChoice = std::max(0, (int)Wh_GetIntSetting(L"monitor"));
    g_debug = Wh_GetIntSetting(L"debugLog") != 0;
    Wh_Log(L"[Settings] useWinP=%d lang=%d monitor=%d colorMode=%d zBand=%d debug=%d",
           g_useWinP ? 1 : 0, g_lang, g_monitorChoice, g_colorMode,
           g_useZBand ? 1 : 0, g_debug ? 1 : 0);
}

// Strings: 0=it,1=en,2=es,3=fr,4=pt,5=de,6=ru,7=zh,8=ja,9=ko
enum {
    STR_TITLE_PROJECT, STR_TITLE_SECOND,
    STR_OPT_INTERNAL, STR_OPT_CLONE, STR_OPT_EXTEND, STR_OPT_EXTERNAL,
    STR_MORE, STR_DETECT, STR_UNAVAILABLE,
    STR_SETMAIN, STR_COPYINFO,
    STR_APPLIED, STR_REFUSED, STR_DETECTING, STR_MAIN_CHANGED, STR_MAIN_FAILED,
    STR_ALREADY, STR_SUBTITLE, STR_COUNT
};

static const wchar_t* T(int id) {
    static const wchar_t* S[10][STR_COUNT] = {
        {
            L"Proietta", L"Secondo schermo", L"Solo schermo PC", L"Duplica",
            L"Estendi", L"Solo secondo schermo", L"Altre impostazioni", L"Rileva display",
            L"Collega un secondo display per usare questa opzione.", L"Imposta come display principale", L"Copia informazioni sul display", L"Configurazione display applicata.",
            L"Modifica rifiutata: ", L"Rilevamento...", L"Display principale modificato.", L"Impossibile modificare il display principale.",
            L"Questa opzione è già attiva.", L"Proietta su uno schermo connesso"
        },
        {
            L"Project", L"Second screen", L"PC screen only", L"Duplicate",
            L"Extend", L"Second screen only", L"More settings", L"Detect displays",
            L"Connect a second display to use this option.", L"Set as main display", L"Copy monitor information", L"Display configuration applied.",
            L"Change refused: ", L"Detecting...", L"Main display changed.", L"The main display could not be changed.",
            L"This option is already active.", L"Project to a connected screen"
        },
        {
            L"Proyectar", L"Segunda pantalla", L"Solo pantalla del equipo", L"Duplicar",
            L"Extender", L"Solo segunda pantalla", L"Más configuración", L"Detectar pantallas",
            L"Conecta una segunda pantalla para usar esta opción.", L"Establecer como pantalla principal", L"Copiar información de la pantalla", L"Configuración de pantalla aplicada.",
            L"Cambio rechazado: ", L"Detectando...", L"Pantalla principal cambiada.", L"No se pudo cambiar la pantalla principal.",
            L"Esta opción ya está activa.", L"Proyecta en una pantalla conectada"
        },
        {
            L"Projection", L"Second écran", L"Écran du PC uniquement", L"Dupliquer",
            L"Étendre", L"Second écran uniquement", L"Plus de paramètres", L"Détecter des écrans",
            L"Connectez un second écran pour utiliser cette option.", L"Définir comme écran principal", L"Copier les informations de l'écran", L"Configuration d'affichage appliquée.",
            L"Modification refusée : ", L"Détection...", L"Écran principal modifié.", L"L'écran principal n'a pas pu être modifié.",
            L"Cette option est déjà active.", L"Effectuer la projection sur un écran connecté"
        },
        {
            L"Projetar", L"Segunda tela", L"Somente tela do PC", L"Duplicar",
            L"Estender", L"Somente segunda tela", L"Mais configurações", L"Detectar monitores",
            L"Conecte uma segunda tela para usar esta opção.", L"Definir como monitor principal", L"Copiar informações do monitor", L"Configuração de exibição aplicada.",
            L"Alteração recusada: ", L"Detectando...", L"Monitor principal alterado.", L"O monitor principal não pôde ser alterado.",
            L"Esta opção já está ativa.", L"Projetar em uma tela conectada"
        },
        {
            L"Projizieren", L"Zweiter Bildschirm", L"Nur PC-Bildschirm", L"Duplizieren",
            L"Erweitern", L"Nur zweiter Bildschirm", L"Weitere Einstellungen", L"Bildschirme erkennen",
            L"Schließen Sie einen zweiten Bildschirm an, um diese Option zu verwenden.", L"Als Hauptbildschirm festlegen", L"Bildschirminformationen kopieren", L"Bildschirmkonfiguration übernommen.",
            L"Änderung abgelehnt: ", L"Erkennung...", L"Hauptbildschirm geändert.", L"Der Hauptbildschirm konnte nicht geändert werden.",
            L"Diese Option ist bereits aktiv.", L"Auf einen angeschlossenen Bildschirm projizieren"
        },
        {
            L"Проецирование", L"Второй экран", L"Только экран компьютера", L"Дублировать",
            L"Расширить", L"Только второй экран", L"Другие параметры", L"Обнаружить экраны",
            L"Подключите второй экран, чтобы использовать этот параметр.", L"Сделать основным экраном", L"Копировать сведения об экране", L"Конфигурация экрана применена.",
            L"Изменение отклонено: ", L"Поиск...", L"Основной экран изменён.", L"Не удалось изменить основной экран.",
            L"Этот параметр уже выбран.", L"Проецирование на подключенный экран"
        },
        {
            L"投影", L"第二块屏幕", L"仅电脑屏幕", L"复制",
            L"扩展", L"仅第二屏幕", L"更多设置", L"检测",
            L"连接第二块屏幕后才能使用此选项。", L"设为主显示器", L"复制显示器信息", L"已应用显示配置。",
            L"更改被拒绝：", L"正在检测...", L"主显示器已更改。", L"无法更改主显示器。",
            L"此选项已经处于活动状态。", L"投影到已连接的屏幕"
        },
        {
            L"プロジェクト", L"セカンド スクリーン", L"PC 画面のみ", L"複製",
            L"延長", L"セカンド スクリーンのみ", L"その他の設定", L"ディスプレイを検出",
            L"このオプションはセカンド ディスプレイを接続すると使えます。", L"メイン ディスプレイに設定", L"ディスプレイ情報をコピー", L"ディスプレイ構成を適用しました。",
            L"変更は拒否されました: ", L"検出中...", L"メイン ディスプレイを変更しました。", L"メイン ディスプレイを変更できませんでした。",
            L"このオプションは既に有効です。", L"接続されている画面に投影"
        },
        {
            L"프로젝션", L"두 번째 화면", L"PC 화면만", L"복제",
            L"확장", L"두 번째 화면만", L"추가 설정", L"디스플레이 검색",
            L"이 옵션은 두 번째 디스플레이를 연결하면 사용할 수 있습니다.", L"기본 디스플레이로 설정", L"디스플레이 정보 복사", L"디스플레이 구성을 적용했습니다.",
            L"변경이 거부되었습니다: ", L"검색 중...", L"기본 디스플레이를 변경했습니다.", L"기본 디스플레이를 변경할 수 없습니다.",
            L"이 옵션은 이미 선택되어 있습니다.", L"연결된 화면으로 프로젝션"
        }
    };
    int l = g_lang; if (l < 0 || l > 9) l = 1;
    if (id < 0 || id >= STR_COUNT) return L"";
    // A missing translation must never hand out a null pointer: the drawing
    // code passes these straight to lstrlenW / DrawText.
    const wchar_t* text = S[l][id];
    return text ? text : L"";
}

static const wchar_t* OptionName(int which) {
    switch (which) {
        case TOPO_CLONE:    return T(STR_OPT_CLONE);
        case TOPO_EXTEND:   return T(STR_OPT_EXTEND);
        case TOPO_EXTERNAL: return T(STR_OPT_EXTERNAL);
        case TOPO_INTERNAL:
        default:            return T(STR_OPT_INTERNAL);
    }
}

static HFONT MkFont(int px, const wchar_t* face, int weight = FW_NORMAL) {
    return CreateFontW(-px, 0, 0, 0, weight, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                       CLEARTYPE_QUALITY, 0, face);
}

static void DeleteFonts() {
    g_fTitle.reset();
    g_fLabel.reset();
    g_fText.reset();
    g_fontScale = -1;
}

static void EnsureFonts() {
    if (g_fontScale == g_scale && g_fTitle.get()) return;
    DeleteFonts();
    // 29 px light title and 15 px body text are the two sizes the Windows 8.1
    // pane uses: its title ink is 26 px tall and its tagline and labels 14 px.
    g_fTitle.reset(MkFont(S(29), L"Segoe UI Light"));
    g_fLabel.reset(MkFont(S(15), L"Segoe UI"));
    g_fText.reset(MkFont(S(14), L"Segoe UI"));
    g_fontScale = g_scale;
}

// ---------------------------------------------------------------- display state
// The four documented projection options, in the flyout order, each mapped to
// the single SDC_TOPOLOGY_xxx flag that requests it.
static const UINT32 kSdcTopology[TOPO_COUNT] = {
    SDC_TOPOLOGY_INTERNAL, SDC_TOPOLOGY_CLONE, SDC_TOPOLOGY_EXTEND, SDC_TOPOLOGY_EXTERNAL
};

static int TopologyFromId(DISPLAYCONFIG_TOPOLOGY_ID id) {
    switch (id) {
        case DISPLAYCONFIG_TOPOLOGY_INTERNAL: return TOPO_INTERNAL;
        case DISPLAYCONFIG_TOPOLOGY_CLONE:    return TOPO_CLONE;
        case DISPLAYCONFIG_TOPOLOGY_EXTEND:   return TOPO_EXTEND;
        case DISPLAYCONFIG_TOPOLOGY_EXTERNAL: return TOPO_EXTERNAL;
        default:                              return TOPO_UNKNOWN;
    }
}

// Enumerates display paths with the pattern documented for QueryDisplayConfig:
// ask GetDisplayConfigBufferSizes for the sizes, query, and retry while the
// configuration changes underneath (ERROR_INSUFFICIENT_BUFFER).
static bool CcdQuery(UINT32 flags,
                     std::vector<DISPLAYCONFIG_PATH_INFO>& paths,
                     std::vector<DISPLAYCONFIG_MODE_INFO>& modes,
                     DISPLAYCONFIG_TOPOLOGY_ID* topology) {
    LONG result = ERROR_SUCCESS;
    DISPLAYCONFIG_TOPOLOGY_ID dummyTopology = (DISPLAYCONFIG_TOPOLOGY_ID)0;
    for (int attempt = 0; attempt < 4; attempt++) {
        UINT32 pathCount = 0, modeCount = 0;
        result = GetDisplayConfigBufferSizes(flags, &pathCount, &modeCount);
        if (result != ERROR_SUCCESS) {
            Wh_Log(L"[Ccd] GetDisplayConfigBufferSizes(0x%X) failed, err=%lu",
                   (unsigned)flags, (unsigned long)result);
            return false;
        }
        paths.resize(std::max<UINT32>(1, pathCount));
        modes.resize(std::max<UINT32>(1, modeCount));
        // currentTopologyId must be NULL for every flag except
        // QDC_DATABASE_CURRENT, and must not be NULL for that one.
        DISPLAYCONFIG_TOPOLOGY_ID* out =
            (flags == QDC_DATABASE_CURRENT) ? (topology ? topology : &dummyTopology) : NULL;
        result = QueryDisplayConfig(flags, &pathCount, paths.data(),
                                    &modeCount, modes.data(), out);
        // The function may return fewer paths and modes than estimated.
        paths.resize(pathCount);
        modes.resize(modeCount);
        if (result != ERROR_INSUFFICIENT_BUFFER) break;
    }
    if (result != ERROR_SUCCESS) {
        static RateLimitedLog log{L"CcdQuery failed", 5};
        if (log.ShouldLog())
            Wh_Log(L"[Ccd] QueryDisplayConfig(0x%X) failed, err=%lu",
                   (unsigned)flags, (unsigned long)result);
        return false;
    }
    return true;
}

// The monitor friendly name, as returned in DISPLAYCONFIG_TARGET_DEVICE_NAME.
static std::wstring CcdTargetName(LUID adapterId, UINT32 targetId) {
    DISPLAYCONFIG_TARGET_DEVICE_NAME n = {};
    n.header.size = sizeof(n);
    n.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
    n.header.adapterId = adapterId;
    n.header.id = targetId;
    if (DisplayConfigGetDeviceInfo(&n.header) != ERROR_SUCCESS) return std::wstring();
    return n.monitorFriendlyDeviceName[0] ? std::wstring(n.monitorFriendlyDeviceName)
                                         : std::wstring();
}

// The GDI device name of a source. The documentation states that this name can
// be used with the EnumDisplaySettings family, which is what the "set as main
// display" command needs.
static std::wstring CcdSourceName(LUID adapterId, UINT32 sourceId) {
    DISPLAYCONFIG_SOURCE_DEVICE_NAME s = {};
    s.header.size = sizeof(s);
    s.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
    s.header.adapterId = adapterId;
    s.header.id = sourceId;
    if (DisplayConfigGetDeviceInfo(&s.header) != ERROR_SUCCESS) return std::wstring();
    return s.viewGdiDeviceName[0] ? std::wstring(s.viewGdiDeviceName) : std::wstring();
}

struct GdiMon {
    std::wstring dev;
    RECT rc = { 0, 0, 0, 0 };
    HMONITOR mon = NULL;
    bool primary = false;
};

static BOOL CALLBACK GdiMonProc(HMONITOR hm, HDC, LPRECT, LPARAM lp) {
    return RunGuardedValue<BOOL>(L"GdiMonProc", TRUE, [&]() -> BOOL {
        // MONITORINFOEX is the documented way to get the GDI device name of a
        // monitor; it starts with the same fields as MONITORINFO, so GetMonitorInfo
        // accepts a pointer to it.
        MONITORINFOEXW mi = {};
        mi.cbSize = sizeof(mi);
        if (GetMonitorInfoW(hm, (MONITORINFO*)&mi)) {
            GdiMon m;
            m.dev = mi.szDevice;
            m.rc = mi.rcMonitor;
            m.mon = hm;
            m.primary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
            ((std::vector<GdiMon>*)lp)->push_back(m);
        }
        return TRUE;
    });
}

static void CollectGdiMonitors(std::vector<GdiMon>& out) {
    EnumDisplayMonitors(NULL, NULL, GdiMonProc, (LPARAM)&out);
}

static int MonitorScalePercent(HMONITOR mon) {
    UINT dpiX = 96, dpiY = 96;
    if (!mon || FAILED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) return 100;
    return (int)(dpiX / 96.0 * 100.0 + 0.5);
}

// The monitor description of a display device, from the monitor device node
// attached to it. Used when the display configuration API has no friendly name
// for a target (no EDID, a fallback driver, or a remote session).
static std::wstring GdiMonitorName(const std::wstring& dev) {
    if (dev.empty()) return std::wstring();
    DISPLAY_DEVICEW mon = {};
    mon.cb = sizeof(mon);
    if (!EnumDisplayDevicesW(dev.c_str(), 0, &mon, 0)) return std::wstring();
    return mon.DeviceString[0] ? std::wstring(mon.DeviceString) : std::wstring();
}

// The mode of a single display device, in physical pixels: the display
// configuration APIs are documented as not taking part in DPI virtualization.
static bool CurrentModeOf(const std::wstring& dev, int& w, int& h, int& hz, int& x, int& y) {
    if (dev.empty()) return false;
    DEVMODEW dm = {};
    dm.dmSize = sizeof(dm);
    if (!EnumDisplaySettingsExW(dev.c_str(), ENUM_CURRENT_SETTINGS, &dm, 0)) return false;
    w = (int)dm.dmPelsWidth;
    h = (int)dm.dmPelsHeight;
    hz = (int)dm.dmDisplayFrequency;
    x = (int)dm.dmPosition.x;
    y = (int)dm.dmPosition.y;
    return true;
}

// Rebuilds the model behind the panel: one row per connected display, the
// current topology, and the connected/active counts. Safe to call from any
// thread; painting reads the result under the shared lock.
static void RefreshState() {
    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;
    DISPLAYCONFIG_TOPOLOGY_ID topoId = (DISPLAYCONFIG_TOPOLOGY_ID)0;

    // QDC_DATABASE_CURRENT describes the currently connected displays, so a
    // monitor that is plugged in but switched off still shows up (and the
    // topology of the saved configuration is returned for it).
    bool haveCcd = CcdQuery(QDC_DATABASE_CURRENT, paths, modes, &topoId);
    if (!haveCcd) {
        paths.clear();
        modes.clear();
        haveCcd = CcdQuery(QDC_ONLY_ACTIVE_PATHS | QDC_VIRTUAL_MODE_AWARE, paths, modes, NULL);
    }

    std::vector<GdiMon> gdis;
    CollectGdiMonitors(gdis);

    std::vector<MonRow> rows;
    int connected = 0, active = 0;
    int topology = TOPO_UNKNOWN;

    if (haveCcd) {
        for (const DISPLAYCONFIG_PATH_INFO& p : paths) {
            // A path whose target is not available is a disconnected connector,
            // not a display: the documentation warns that targetAvailable is
            // what distinguishes the two.
            if (!p.targetInfo.targetAvailable) continue;

            MonRow r;
            r.adapter = p.targetInfo.adapterId;
            r.targetId = p.targetInfo.id;
            r.sourceId = p.sourceInfo.id;
            r.active = (p.flags & DISPLAYCONFIG_PATH_ACTIVE) != 0;
            r.gdi = CcdSourceName(r.adapter, r.sourceId);
            r.name = CcdTargetName(r.adapter, r.targetId);

            bool duplicate = false;
            for (const MonRow& e : rows) {
                // In clone topology several paths share one source (same gdi),
                // so deduplicate physical displays by (adapterId, targetId).
                if (e.adapter.LowPart == r.adapter.LowPart &&
                    e.adapter.HighPart == r.adapter.HighPart &&
                    e.targetId == r.targetId) {
                    duplicate = true;
                    break;
                }
            }
            if (duplicate) continue;

            for (const GdiMon& gm : gdis) {
                if (gm.dev != r.gdi) continue;
                r.primary = gm.primary;
                if (r.active) {
                    r.x = (int)gm.rc.left;
                    r.y = (int)gm.rc.top;
                    r.scale = MonitorScalePercent(gm.mon);
                }
                break;
            }
            if (r.active) {
                int x = r.x, y = r.y;
                CurrentModeOf(r.gdi, r.width, r.height, r.refresh, x, y);
            }
            connected++;
            if (r.active) active++;
            rows.push_back(r);
        }
        topology = TopologyFromId(topoId);
    }

    // Fallback for the cases where the CCD API is unavailable (a non-WDDM
    // driver, a remote session): the classic display devices still describe
    // the desktop, and everything except the exact topology can be shown.
    if (rows.empty() && !gdis.empty()) {
        for (const GdiMon& gm : gdis) {
            MonRow r;
            r.name = gm.dev;
            r.gdi = gm.dev;
            r.active = true;
            r.primary = gm.primary;
            r.x = (int)gm.rc.left;
            r.y = (int)gm.rc.top;
            r.scale = MonitorScalePercent(gm.mon);
            CurrentModeOf(r.gdi, r.width, r.height, r.refresh, r.x, r.y);
            rows.push_back(r);
        }
        connected = (int)rows.size();
        active = (int)rows.size();
    }

    // Name the displays the way the Screen Resolution page numbers them, and
    // fill in the friendly name when the driver could not provide one.
    for (size_t i = 0; i < rows.size(); i++) {
        MonRow& r = rows[i];
        if (r.name.empty() && !r.gdi.empty()) r.name = GdiMonitorName(r.gdi);
        if (r.name.empty()) {
            std::wstring fallback = L"Display ";
            fallback += std::to_wstring((unsigned)(i + 1));
            r.name = fallback;
        }
    }
    std::stable_sort(rows.begin(), rows.end(), [](const MonRow& a, const MonRow& b) {
        if (a.primary != b.primary) return a.primary;
        if (a.y != b.y) return a.y < b.y;
        return a.x < b.x;
    });

    // A fingerprint of everything the panel shows. The state watchdog compares it
    // before and after a refresh so a panel that has nothing new is not repainted
    // every two seconds.
    unsigned stamp = (unsigned)(topology + 1) * 31 + (unsigned)connected * 7 +
                     (unsigned)active;
    for (const MonRow& r : rows) {
        stamp = stamp * 131 + (unsigned)r.name.size() * 3 + (unsigned)r.width * 131 +
                (unsigned)r.height * 17 + (unsigned)r.refresh * 5 + (unsigned)r.scale +
                (r.active ? 1u : 0u) + (r.primary ? 2u : 0u);
    }

    int prevTopology = TOPO_UNKNOWN;
    {
        SrwGuard g(&g_lock, true);
        prevTopology = g_topology;
        g_monitors.swap(rows);
        g_topology = topology;
        g_connected = connected;
        g_active = active;
        g_stateStamp = stamp;
    }
    if (topology != prevTopology && topology >= 0 && topology < TOPO_COUNT)
        g_sel = topology;
    if (g_sel < 0 || g_sel >= TOPO_COUNT) g_sel = TOPO_INTERNAL;
    if (g_debug)
        Wh_Log(L"[State] topology=%d connected=%d active=%d", topology, connected, active);
}

static bool OptionAvailable(int which) {
    if (which == TOPO_INTERNAL) return true;
    SrwGuard g(&g_lock, false);
    return g_disableUnavailable ? g_connected >= 2 : true;
}

static void ReportFailure(LONG code) {
    std::wstring text = WinErrText((DWORD)code);
    if (text.empty()) text = std::wstring(L"error ") + std::to_wstring((unsigned long)code);
    else text += L" (" + std::to_wstring((unsigned long)code) + L")";
    std::wstring line = T(STR_REFUSED);
    line += text;
    SetStatus(line);
    Wh_Log(L"[Apply] refused, err=%lu (%s)", (unsigned long)code, text.c_str());
}

static const wchar_t* DispChangeErrText(LONG code) {
    switch (code) {
    case DISP_CHANGE_SUCCESSFUL:  return L"DISP_CHANGE_SUCCESSFUL";
    case DISP_CHANGE_RESTART:     return L"DISP_CHANGE_RESTART";
    case DISP_CHANGE_FAILED:      return L"DISP_CHANGE_FAILED";
    case DISP_CHANGE_BADMODE:     return L"DISP_CHANGE_BADMODE";
    case DISP_CHANGE_NOTUPDATED:  return L"DISP_CHANGE_NOTUPDATED";
    case DISP_CHANGE_BADFLAGS:    return L"DISP_CHANGE_BADFLAGS";
    case DISP_CHANGE_BADPARAM:    return L"DISP_CHANGE_BADPARAM";
    case DISP_CHANGE_BADDUALVIEW: return L"DISP_CHANGE_BADDUALVIEW";
    default:                      return L"DISP_CHANGE_UNKNOWN";
    }
}

// Applies one of the four documented topologies. The documentation for
// SetDisplayConfig requires NULL arrays and zero counts together with a
// SDC_TOPOLOGY_xxx flag: the system then takes the last stored configuration of
// that topology, or picks the best one if it has never been used.
static void ApplyTopology(int which, const wchar_t* origin) {
    if (which < 0 || which >= TOPO_COUNT || !g_wnd) return;

    const DWORD now = GetTickCount();
    if (now - g_lastApply < (DWORD)g_applyCooldownMs) return;
    g_lastApply = now;

    int current = TOPO_UNKNOWN;
    int connected = 0;
    {
        SrwGuard g(&g_lock, false);
        current = g_topology;
        connected = g_connected;
    }
    if (which != TOPO_INTERNAL && g_disableUnavailable && connected < 2) {
        SetStatus(T(STR_UNAVAILABLE));
        return;
    }
    if (current == which) {
        SetStatus(T(STR_ALREADY));
        if (g_closeAfterApply) ClosePane();
        return;
    }

    const UINT32 base = kSdcTopology[which];

    // SDC_VALIDATE and SDC_APPLY are documented as mutually exclusive, so the
    // check is a separate call. Per SetDisplayConfig documentation, if the
    // stored configuration returns ERROR_BAD_CONFIGURATION, retry with
    // SDC_ALLOW_CHANGES.
    if (g_validateFirst) {
        LONG v = SetDisplayConfig(0, NULL, 0, NULL, SDC_VALIDATE | base);
        if (v == ERROR_BAD_CONFIGURATION) {
            v = SetDisplayConfig(0, NULL, 0, NULL, SDC_VALIDATE | base | SDC_ALLOW_CHANGES);
        }
        if (v != ERROR_SUCCESS) {
            ReportFailure(v);
            return;
        }
    }

    UINT32 flags = SDC_APPLY | base;
    if (g_persistPaths) flags |= SDC_PATH_PERSIST_IF_REQUIRED;

    LONG r = SetDisplayConfig(0, NULL, 0, NULL, flags);
    if (r == ERROR_BAD_CONFIGURATION) {
        r = SetDisplayConfig(0, NULL, 0, NULL, flags | SDC_ALLOW_CHANGES);
    }

    if (r != ERROR_SUCCESS) {
        ReportFailure(r);
        RefreshState();
        InvalidatePanel();
        return;
    }
    Wh_Log(L"[Apply] topology %d applied from %s", which, origin ? origin : L"ui");
    SetStatus(T(STR_APPLIED));
    RefreshState();
    InvalidatePanel();
    if (g_closeAfterApply) ClosePane();
}

// "Detect" as the classic Screen Resolution page implements it: re-apply the
// last saved configuration for the currently connected displays, which is the
// documented meaning of SDC_USE_DATABASE_CURRENT, then re-enumerate.
static void DetectDisplays() {
    SetStatus(T(STR_DETECTING));
    if (g_wnd) UpdateWindow(g_wnd);
    const LONG r = SetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY | SDC_USE_DATABASE_CURRENT);
    if (r != ERROR_SUCCESS) ReportFailure(r);
    else SetStatus(T(STR_APPLIED));
    RefreshState();
    InvalidatePanel();
}

// ChangeDisplaySettingsEx with CDS_SET_PRIMARY is the documented way to make a
// display the primary one. The devices are repositioned relative to the new
// origin and applied in one step, as the CDS_NORESET documentation requires.
static bool SetMainDisplay(int row) {
    MonRow m;
    {
        SrwGuard g(&g_lock, false);
        if (row < 0 || row >= (int)g_monitors.size()) return false;
        m = g_monitors[row];
    }
    if (m.gdi.empty() || !m.active || m.primary) {
        SetStatus(T(STR_MAIN_FAILED));
        return false;
    }

    DEVMODEW cur = {};
    cur.dmSize = sizeof(cur);
    if (!EnumDisplaySettingsExW(m.gdi.c_str(), ENUM_CURRENT_SETTINGS, &cur, 0)) {
        Wh_Log(L"[Main] EnumDisplaySettingsExW(%s) failed", m.gdi.c_str());
        SetStatus(T(STR_MAIN_FAILED));
        return false;
    }
    const LONG dx = cur.dmPosition.x, dy = cur.dmPosition.y;

    DEVMODEW dm = {};
    dm.dmSize = sizeof(dm);
    dm.dmFields = DM_POSITION;
    dm.dmPosition.x = 0;
    dm.dmPosition.y = 0;
    LONG r = ChangeDisplaySettingsExW(m.gdi.c_str(), &dm, NULL,
                                      CDS_UPDATEREGISTRY | CDS_SET_PRIMARY | CDS_NORESET,
                                      NULL);
    if (r != DISP_CHANGE_SUCCESSFUL) {
        Wh_Log(L"[Main] ChangeDisplaySettingsExW(%s, CDS_SET_PRIMARY) failed, code=%ld (%s)",
               m.gdi.c_str(), (long)r, DispChangeErrText(r));
        SetStatus(std::wstring(T(STR_MAIN_FAILED)));
        return false;
    }

    for (UINT i = 0; ; i++) {
        DISPLAY_DEVICEW dd = {};
        dd.cb = sizeof(dd);
        if (!EnumDisplayDevicesW(NULL, i, &dd, 0)) break;
        if (!(dd.StateFlags & DISPLAY_DEVICE_ACTIVE)) continue;
        if (_wcsicmp(dd.DeviceName, m.gdi.c_str()) == 0) continue;
        DEVMODEW other = {};
        other.dmSize = sizeof(other);
        if (!EnumDisplaySettingsExW(dd.DeviceName, ENUM_CURRENT_SETTINGS, &other, 0)) continue;
        DEVMODEW moved = {};
        moved.dmSize = sizeof(moved);
        moved.dmFields = DM_POSITION;
        moved.dmPosition.x = other.dmPosition.x - dx;
        moved.dmPosition.y = other.dmPosition.y - dy;
        ChangeDisplaySettingsExW(dd.DeviceName, &moved, NULL,
                                 CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
    }

    r = ChangeDisplaySettingsExW(NULL, NULL, NULL, 0, NULL);
    if (r != DISP_CHANGE_SUCCESSFUL) {
        Wh_Log(L"[Main] ChangeDisplaySettingsExW(NULL) commit failed, code=%ld (%s)",
               (long)r, DispChangeErrText(r));
        SetStatus(T(STR_MAIN_FAILED));
        RefreshState();
        InvalidatePanel();
        return false;
    }
    Wh_Log(L"[Main] primary display set to %s", m.gdi.c_str());
    SetStatus(T(STR_MAIN_CHANGED));
    RefreshState();
    InvalidatePanel();
    return true;
}

static std::wstring MonitorInfoText(const MonRow& m) {
    std::wstring out = m.name + L"\r\n";
    if (!m.gdi.empty()) out += m.gdi + L"\r\n";
    if (m.width > 0 && m.height > 0) {
        out += std::to_wstring(m.width) + L" x " + std::to_wstring(m.height);
        if (m.refresh > 0) out += L" @ " + std::to_wstring(m.refresh) + L" Hz";
        out += L"\r\n";
    }
    out += std::to_wstring(m.scale) + L"%  ";
    out += L"(" + std::to_wstring(m.x) + L", " + std::to_wstring(m.y) + L")\r\n";
    out += m.active ? L"active" : L"inactive";
    if (m.primary) out += L", main";
    return out;
}

static void OpenUri(PCWSTR uri) {
    // Documented ms-settings: deep links of the Settings app.
    HINSTANCE hr = ShellExecuteW(NULL, L"open", uri, NULL, NULL, SW_SHOWNORMAL);
    if ((INT_PTR)hr <= 32) {
        Wh_Log(L"[Uri] ShellExecuteW(%s) failed, err=%lu", uri, (unsigned long)(INT_PTR)hr);
        SetStatus(WinErrText((DWORD)(INT_PTR)hr));
    }
}

static void OpenMoreSettings() {
    if (g_moreSettings == 1) OpenUri(L"ms-settings:display-advanced");
    else if (g_moreSettings == 2) OpenUri(L"ms-settings:display-advancedgraphics");
    else OpenUri(L"ms-settings:display");
}

// ---------------------------------------------------------------- glyphs
// The four silhouettes of the pane are not drawn by this mod: they are the
// Windows 8.1 bitmaps, cut out of a capture of the real flyout at 100% scaling
// and stored below as base64 PNG alpha masks, one per option, at the original
// 89x45 px size.
//
// Windows 8.1 paints them as white at two alpha levels: the screens that are in
// use are opaque, the screen that is not is at ~50%. That is why a single
// 8-bit mask reproduces the original everywhere: composited over the accent
// background it gives the light gray of the inactive screen, composited over the
// dark selection band it gives the darker gray of the same capture.
//
// How they were cut, for the record (src/extract_glyphs.py of the mod
// repository): crop the icon of each row out of the capture, then invert
// c = a*255 + (1-a)*bg for every pixel to recover its alpha (bg is the accent
// color, or the selection fill inside the band), keep background / 128 / 255,
// and save one grayscale-alpha PNG per option.
//
// At run time the blobs are decoded by the Windows Imaging Component
// (IWICImagingFactory, documented) into a premultiplied 32-bit bitmap, so no
// third-party image decoder is needed.
static const char kGlyphPcScreenOnly[] =
    "iVBORw0KGgoAAAANSUhEUgAAAFkAAAAtCAYAAADFuMd4AAABlklEQVR42u2a7W2DMBRFD1EGoBtkhW6QTlC6Ad2gnQDY"
    "gA2aDUI2yQhsABvQP67URjgfNQae/a7kPygKV0dP1+8ZJ8MwIEQ5sEOOWuAAsFHA3rQzvsVAlgb4j++tQOOlNI8bVN6l"
    "kBWyQlYpZIWskH+1IcNCq4wFcrGgryIkyPcMI8nMnoaY4kKlkBWySiHHAzkHjkAX4mb3aHcxtb4MYK1kD8pM1UYFeE7I"
    "pYmGVDPZH+CCiOU7k7MbgFugAU4K2W2TG1MPVECtlewOOLUAfgHOmsnT9MHRA/YJ2damVbEB9gn51bLJ1UQoX5D3I8+a"
    "WFs4X5DHNrxTrJB9dRcJqtn6ZF8TZHBxMfeXattkKFHtLcjVguYu330QCLo1vkkE3bQPrrvIWO5iy8/KQ4Fsq+SO5c9+"
    "e+BZcB5freS1HK6n2E/xREPOzFqL9iHExmVcdKzvE1EPPIVSyS4x0Zop79o6O8TGMQTIrjHxecdv3h3+f20x9q+4cImJ"
    "Bnh7YCQuYouNLfDhmMMZ89wASo3XOpQ+WTWhvgEGWKW8KNLL9AAAAABJRU5ErkJggg==";

static const char kGlyphDuplicate[] =
    "iVBORw0KGgoAAAANSUhEUgAAAFkAAAAtCAYAAADFuMd4AAACU0lEQVR42u2b223CMBiFvyAGSCdA2QB1gcICNB2BDoDa"
    "vlcNGzABlTpALxMkGxR1gDICbJA+YF6qhtg4vqU+kiUkgvL7cDj/JSap65oIsxgEFGsJ1AGtMjSSS2ASmIAnR6KTQOwi"
    "ZE9LhiEGHZooBkTExBdJjogk+4JhpOBsjIEcuAJGQNaUlCPJasiABXAtXmvbReGwWyo8JHcNfAN3KgS3NSOuG4CkIRbb"
    "dXIBPCnGXKs2I7Y3VXuk3lfhvTHxGUpqJZDGEs4M5sBnVwTLerIru3DhyWNBsG4eUfbk/1SelWd8bgt8AF8+evIcmHGY"
    "u6a4n669KlrEM/ACVD4mvrUg2CcUClXEBriXIdeFJ+fi208l/cyWJ2eiyZBV761iGZoMLCrlrcuM3SEeOybYiZJlOyYX"
    "SpZVsSrBVquLvIXgY2b+cKTihaQH3+rcxLSSdw0WsQeWwMpxnfxN+7BnqpLkbCt5fYLgqVCI69Y5k7CJSvdGJhPf3GOC"
    "j1bWhhefZxdNdfDSE4Lh8ESjrZOrfCZ51hD0yqPSbdTyfmfJ2BTJE5NBd1i+ncJXVzcylfjSAEi2NisZhr6BEBDiqDO0"
    "w4fZQHJTNtdfqAIVcQVsT5G8dBjcsoOuyweCp21tdYThEi7H/d8B5n0huUnJTYMdm9gDl6KJ6Z2SfRmupxyGTL2zi1xy"
    "cGILkz7Yxm+72OHfI6I9cNEXJevYxFZ0eafWRsM23vpAsq5NPEhco/Oz983GzrILHZt4B24kry2QP4baG9sYcjjUnGqq"
    "zEZHk4pYV6GR/APKkBJaUS0AawAAAABJRU5ErkJggg==";

static const char kGlyphExtend[] =
    "iVBORw0KGgoAAAANSUhEUgAAAFkAAAAtCAYAAADFuMd4AAACeElEQVR42u2b/W3aQBjGf0YMQCeoyARpF6i9QEQ7QCUy"
    "QFX1/6pmA9QBHCkLkEwAnSCoA8TKBGGD6x+5SJTi47B9H+/Vj3QSiMO89/jx8773QaaUYoBbjATFugaUoLaWRvIayIUJ"
    "OH8lOhNiF5I9LRtLDFqaKEYMGBLfQPKAgeRYMBYcuwJq4An4BdwB2ygztcASLjOUdTVwD/zUr6OI12QXZcDZUtlycFPg"
    "K/AIVPp91EoOLfGshZKPYdHhpvWiZBuSs0AB9kUy2qs/ebSQ/5JkgB1QeEqO1p6cGibAAzCP0ZNTUfI+3jlWdDJKvgaW"
    "LX127bPyCKnkOXDFy7rr5IhCTyl5Hznw+Uwr2GpFu6/rlVJN7RX03Cp1HKbfVZbx5EqpB2WP0sH4/onXp5JnwI1Wraku"
    "bqPkQ1RnqPrCQWkXxJNLYGUg2IVf31j2/Z6CJ5fAjzNmeH0o+VxF961mr0qenSC41hVC4VDRNqXaF8lKfm6wiJ1eU1h2"
    "qJNtkbO3PW+42RcSlVwZCC4aCHaBjYU/T4FLVwG4JHkeeP1gH7eW1iaK5KZksyDM7sXGIrF9kEbylSHJhcL9ic/fSiM5"
    "bzFI1/ht4cuiqotjFy30Y2v73b6qixD4K15Xu9USiPAGiUcCpB0+nI4sB+WzNVUHErEBahPJi4DBLVr6eWwEF6cS3wDH"
    "JdyM8H8HmKdCcpOSmxZ2fGIHvCfscStnSl5FQDA6hipFu5i5XChpOXMUbxuHdvEciYoPbeNNKkruYhO1nuWZ2raDbaxS"
    "ILmrTXyz6NPlsY/NxlrZRRebuAM+WvYtsdtUTco2xrwcmp50VJmPGc1Ex7qURvIfErVfus8qHAcAAAAASUVORK5CYII=";

static const char kGlyphSecondScreenOnly[] =
    "iVBORw0KGgoAAAANSUhEUgAAAFkAAAAtCAYAAADFuMd4AAAB6ElEQVR42u2aoW4CQRCGPwgJtrKOnEPiKgGJAhwWVFUT"
    "HoCUR6isKgmqihTVusNWtUGhyiv0Da7ijhQBub3bO26Hmz/ZhITldu7L5N+ZZStBEKDKV1VBsfpAIGj40iD7QEdYAncO"
    "oCtC7EKyp1VqEoOWlhRVVLrxKWSVQnZFNUWQWi1gALSBBuCd25QVcjJ5wAPQjz5bZ3KnwAZgEw2X4M6AcdaeXGSH5VJ3"
    "Nwd+0gI2tYt5AS/lSvauIu/VjS+nTc0HbrSEy0dj4CsrwAr5dAa/aJ2crwf7KX63B9bAViHHa5XQIhbA0qTUVMj/FY1p"
    "FfENTJPU8Qo5tInHBNk70Y0vuWZ5AlbIYRaP8wSskMPDHhMPntgsUnbIfYM5U21G7BqPuOPKBRmcBpYZ8sBgzlLbaju1"
    "DTq5jUK2UyPm+3VWC5UZcpwfb7NaqMwd38VuImnHdwFJzGRplw89F//jO6UN8q7OHuLeV2MmFBncsbq4dUXA9B26IOd+"
    "8lV6chMYFRzbG+HhjHids4uhA7H1riWTT0EeAXUHYqtjcWvHZcjNaLjU+rauDfLQwRjF28ZxdTGyyOJf4Clmzj1wm/L5"
    "O+BVeibb2sSHwZxni+e7ZmOpINvYxC4aaZqMJBpKtos7Qb73DnxKg/wHYb95UhuFGcEAAAAASUVORK5CYII=";

static const char* const kGlyphPng[TOPO_COUNT] = {
    kGlyphPcScreenOnly, kGlyphDuplicate, kGlyphExtend, kGlyphSecondScreenOnly
};

// Direct RLE-compressed 89x45 8-bit alpha masks of the same four Windows 8.1
// icons in base64, decoded and resampled in pure C++ so icon rendering never
// depends on COM/WIC apartment state or GDI StretchBlt (which zeroes the alpha
// channel in HALFTONE mode at DPI != 100%).
static const char kGlyphMaskPcScreenOnly[] =
    "qQAAYKqAAGCqAABgrIAAYKkAroCpAIKAqACCgKkAgoCoAIKAqQCCgKgAgoCpAIKAqACCgKkAgoCoAIKAqQCCgKgAgoCp"
    "AIKAqACCgIYAAICd/wCAggCCgKgAgoCGAJ//ggCCgKgAgoCGAIH/mwCB/4IAgoCoAIKAhgCB/5sAgf+CAIKAqACCgIYA"
    "gf+bAIH/ggCCgKgAgoCGAIH/mwCB/4IAgoCoAIKAhgCB/5sAgf+CAIKAqACCgIYAgf+VAANgr+//gQCB/4IAgoCoAIKA"
    "hgCB/5MAAJ+E/4EAgf+CAIKAqACCgIYAgf+RAAFQ74X/gQCB/4IAgoCoAIKAhgCB/5AAAICH/4EAgf+CAIKAqACCgIYA"
    "gf+PAACAiP+BAIH/ggCCgKgAgoCGAIH/jgAAUIb/At+PQIEAgf+CAIKAqACCgIYAgf+OAADvhP8B73CEAIH/ggCCgKgA"
    "goCGAIH/jQAAn4T/Ae8whQCB/4IAgoCoAIKAhgCB/40AhP8B7zCGAIH/ggCCgKgAgoCGAIH/jAAAYIT/AHCHAIH/ggCC"
    "gKgAgoCGAIH/jAAAr4P/AN+IAIH/ggCCgKgAgoCGAIH/jAAA74P/AI+IAIH/ggCCgKgAgoCGAIH/jACE/wBAiACB/4IA"
    "goCoAIKAhgCB/5sAgf+CAIKAqACCgIYAgf+bAIH/ggCugIYAn/+CAABgrIAAYIYAAICd/wCAgwAAYKqAAGDfAABQn/8A"
    "YLYAAO+f/wHvILQAAK+h/wC/swAAUKP/AGCyAADvo/8A77EAAK+N/wDfhwAAz43/AK+vAABQjv8AUIcAADCO/wBQrgAA"
    "743/AK+JAACAjf8A760AAHCp/wBw/wCFAA==";

static const char kGlyphMaskDuplicate[] =
    "/wCCAAC/qv8Av6oAAL+s/wC/qQCu/6kAgv+oAIL/qQCC/6gAgv+pAIL/qACC/6kAgv+oAIL/qQCC/6gAgv+pAIL/oQAE"
    "MIC/3v+BAIL/qQCC/54AAiGP3oT/gQCC/4YAAICd/wCAggCC/5wAASGeh/+BAIL/hgCf/4IAgv+bAACAif+BAIL/hgCB"
    "/5sAgf+CAIL/mQABIc+K/4EAgv+GAIH/mwCB/4IAgv+YAAEw74v/gQCC/4YAgf+bAIH/ggCC/5cAAGGN/4EAgv+GAIH/"
    "mwCB/4IAgv+WAAAwjv+BAIL/hgCB/5sAgf+CAIL/lQABIe+L/wLej2GBAIL/hgCB/5UAA2Gv7/+BAIH/ggCC/5UAAM+K"
    "/wGeQIQAgv+GAIH/kwAAnoT/gQCB/4IAgv+UAACAif8Bz0CGAIL/hgCB/5EAAVDvhf+BAIH/ggCC/5MAACGJ/wCeiACC"
    "/4YAgf+QAACAh/+BAIH/ggCC/5MAAJ6I/wCeiQCC/4YAgf+PAACAiP+BAIH/ggCC/5IAACGI/wCeigCC/4YAgf+OAABQ"
    "hv8C3o9AgQCB/4IAgv+SAACPh/8Az4sAgv+GAIH/jgAA74T/Ae9whACB/4IAgv+SAADeh/8AQIsAgv+GAIH/jQAAnoT/"
    "Ae8whQCB/4IAgv+RAAAwh/8AnowAgv+GAIH/jQCE/wHvMIYAgf+CAIL/kQAAgIf/AECMAIL/hgCB/4wAAGGE/wBwhwCB"
    "/4IAgv+RAAC/hv8A3o0Agv+GAIH/jAAAr4P/AN6IAIH/ggCC/5EAAN6G/wCPjQCC/4YAgf+MAADvg/8Aj4gAgf+CAIL/"
    "kQCH/wBhjQCC/4YAgf+MAIT/AECIAIH/ggCC/6gAgv+GAIH/mwCB/4IAgv+oAIL/hgCB/5sAgf+CAK7/ACGFAJ//ggAA"
    "v6z/AL+GAACAnf8AgIMAAL+q/wC/3wAAUJ//AGG2AADvn/8B7yG0AACvof8Av7MAAFCj/wBhsgAA76P/AO+xAACvjf8A"
    "3ocAAM+N/wCvrwAAUI7/AFCHAAAwjv8AUK4AAO+N/wCviQAAgI3/AO+tAABwqf8AcKwA";

static const char kGlyphMaskExtend[] =
    "/wCCAAC/qv8Av6oAAL+s/wC/qQCu/6kAgv+oAIL/qQCC/6gAgv+pAIL/qACC/6kAgv+oAIL/qQCC/6gAgv+pAIL/gQAE"
    "/96/gDChAIL/qQCC/4EAhP8C3o8hngCC/4YAAICd/wCAggCC/4EAh/8BniGcAIL/hgCf/4IAgv+BAIn/AICbAIL/hgCB"
    "/5sAgf+CAIL/gQCK/wHPIZkAgv+GAIH/mwCB/4IAgv+BAIv/Ae8wmACC/4YAgf+bAIH/ggCC/4EAjf8AYZcAgv+GAIH/"
    "mwCB/4IAgv+BAI7/ADCWAIL/hgCB/5sAgf+CAIL/gQACYY/ei/8B7yGVAIL/hgCB/5UAA2Gv7/+BAIH/ggCC/4QAAUCe"
    "iv8Az5UAgv+GAIH/kwAAnoT/gQCB/4IAgv+GAAFAz4n/AICUAIL/hgCB/5EAAVDvhf+BAIH/ggCC/4gAAJ6J/wAhkwCC"
    "/4YAgf+QAACAh/+BAIH/ggCC/4kAAJ6I/wCekwCC/4YAgf+PAACAiP+BAIH/ggCC/4oAAJ6I/wAhkgCC/4YAgf+OAABQ"
    "hv8C3o9AgQCB/4IAgv+LAADPh/8Aj5IAgv+GAIH/jgAA74T/Ae9whACB/4IAgv+LAABAh/8A3pIAgv+GAIH/jQAAnoT/"
    "Ae8whQCB/4IAgv+MAACeh/8AMJEAgv+GAIH/jQCE/wHvMIYAgf+CAIL/jAAAQIf/AICRAIL/hgCB/4wAAGGE/wBwhwCB"
    "/4IAgv+NAADehv8Av5EAgv+GAIH/jAAAr4P/AN6IAIH/ggCC/40AAI+G/wDekQCC/4YAgf+MAADvg/8Aj4gAgf+CAIL/"
    "jQAAYYf/kQCC/4YAgf+MAIT/AECIAIH/ggCC/6gAgv+GAIH/mwCB/4IAgv+oAIL/hgCB/5sAgf+CAK7/ACGFAJ//ggAA"
    "v6z/AL+GAACAnf8AgIMAAL+q/wC/3wAAUJ//AGG2AADvn/8B7yG0AACvof8Av7MAAFCj/wBhsgAA76P/AO+xAACvjf8A"
    "3ocAAM+N/wCvrwAAUI7/AFCHAAAwjv8AUK4AAO+N/wCviQAAgI3/AO+tAABwqf8AcKwA";

static const char kGlyphMaskSecondScreenOnly[] =
    "/wCCAAC/qv8Av6oAAL+s/wC/qQCu/6kAgv+oAIL/qQCC/6gAgv+pAIL/qACC/6kAgv+oAIL/qQCC/6gAgv+pAIL/oQAE"
    "MIC/3v+BAIL/qQCC/54AAiGP3oT/gQCC/4YAAECdgABAggCC/5wAASGeh/+BAIL/hgCfgIIAgv+bAACAif+BAIL/hgCB"
    "gJsAgYCCAIL/mQABIc+K/4EAgv+GAIGAmwCBgIIAgv+YAAEw74v/gQCC/4YAgYCbAIGAggCC/5cAAGGN/4EAgv+GAIGA"
    "mwCBgIIAgv+WAAAwjv+BAIL/hgCBgJsAgYCCAIL/lQABIe+L/wLej2GBAIL/hgCBgJsAgYCCAIL/lQAAz4r/AZ5AhACC"
    "/4YAgYCbAIGAggCC/5QAAICJ/wHPQIYAgv+GAIGAmwCBgIIAgv+TAAAhif8AnogAgv+GAIGAmwCBgIIAgv+TAACeiP8A"
    "nokAgv+GAIGAmwCBgIIAgv+SAAAhiP8AnooAgv+GAIGAmwCBgIIAgv+SAACPh/8Az4sAgv+GAIGAmwCBgIIAgv+SAADe"
    "h/8AQIsAgv+GAIGAmwCBgIIAgv+RAAAwh/8AnowAgv+GAIGAmwCBgIIAgv+RAACAh/8AQIwAgv+GAIGAmwCBgIIAgv+R"
    "AAC/hv8A3o0Agv+GAIGAmwCBgIIAgv+RAADehv8Aj40Agv+GAIGAmwCBgIIAgv+RAIf/AGGNAIL/hgCBgJsAgYCCAIL/"
    "qACC/4YAgYCbAIGAggCC/6gAgv+GAIGAmwCBgIIArv8AIYUAn4CCAAC/rP8Av4YAAECdgABAgwAAv6r/AL/fAAAon4AA"
    "MLYAAHmfgAB5tQAAWKGAAGGzAAAoo4AAMLIAAHmjgAB5sQAAWI2AAHCHAABojYAAWK8AACiOgAAoiACOgAAorgAAeY2A"
    "AFiJAABAjYAAea0AADepgAA3rAA=";

static const char* const kGlyphMaskB64[TOPO_COUNT] = {
    kGlyphMaskPcScreenOnly, kGlyphMaskDuplicate, kGlyphMaskExtend, kGlyphMaskSecondScreenOnly
};

struct GlyphBitmap {
    GdiObj<HBITMAP> bm;
    std::vector<BYTE> alpha;
    int w = 0, h = 0;
};
static GlyphBitmap g_glyph[TOPO_COUNT];
static double g_glyphScale = -1.0;

static void FreeGlyphs() {
    for (int i = 0; i < TOPO_COUNT; i++) {
        g_glyph[i].bm.reset();
        g_glyph[i].alpha.clear();
        g_glyph[i].w = 0;
        g_glyph[i].h = 0;
    }
    g_glyphScale = -1.0;
}

static bool DecodeGlyphMask(const char* b64, std::vector<BYTE>& alphaOut, int& w, int& h) {
    if (!b64) return false;
    std::vector<BYTE> raw;
    raw.reserve(1024);
    unsigned val = 0;
    int bits = -8;
    for (const unsigned char* p = (const unsigned char*)b64; *p; ++p) {
        unsigned char c = *p;
        int d = -1;
        if (c >= 'A' && c <= 'Z') d = c - 'A';
        else if (c >= 'a' && c <= 'z') d = c - 'a' + 26;
        else if (c >= '0' && c <= '9') d = c - '0' + 52;
        else if (c == '+') d = 62;
        else if (c == '/') d = 63;
        else if (c == '=') break;
        if (d < 0) continue;
        val = (val << 6) | (unsigned)d;
        bits += 6;
        if (bits >= 0) {
            raw.push_back((BYTE)((val >> bits) & 0xFF));
            bits -= 8;
        }
    }
    const size_t expected = (size_t)k81GlyphWidth * k81GlyphHeight;
    alphaOut.clear();
    alphaOut.reserve(expected);
    size_t i = 0;
    while (i < raw.size() && alphaOut.size() < expected) {
        BYTE b = raw[i++];
        if (b & 0x80) {
            size_t count = (size_t)(b & 0x7F) + 1;
            if (i >= raw.size()) return false;
            BYTE v = raw[i++];
            for (size_t k = 0; k < count && alphaOut.size() < expected; ++k)
                alphaOut.push_back(v);
        } else {
            size_t count = (size_t)b + 1;
            if (i + count > raw.size()) return false;
            for (size_t k = 0; k < count && alphaOut.size() < expected; ++k)
                alphaOut.push_back(raw[i++]);
        }
    }
    if (alphaOut.size() != expected) return false;
    w = k81GlyphWidth;
    h = k81GlyphHeight;
    return true;
}

// base64 PNG -> premultiplied 32-bit pixels. Every call here is documented:
// CryptStringToBinaryA, SHCreateMemStream, IWICImagingFactory. All COM objects
// are owned by ScopedComPtr RAII wrappers.
static bool DecodeGlyphPng(const char* b64, std::vector<BYTE>& pixels, int& w, int& h) {
    DWORD cb = 0;
    if (!CryptStringToBinaryA(b64, 0, CRYPT_STRING_BASE64, NULL, &cb, NULL, NULL) || !cb)
        return false;
    std::vector<BYTE> png(cb);
    if (!CryptStringToBinaryA(b64, 0, CRYPT_STRING_BASE64, png.data(), &cb, NULL, NULL))
        return false;

    // The UI thread has no COM apartment of its own yet; WIC needs one.
    ScopedComApartment com(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)));

    ScopedComPtr<IWICImagingFactory> factory;
    ScopedComPtr<IStream> stream;
    ScopedComPtr<IWICBitmapDecoder> decoder;
    ScopedComPtr<IWICBitmapFrameDecode> frame;
    ScopedComPtr<IWICFormatConverter> converter;
    bool ok = SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                                         IID_IWICImagingFactory, (void**)factory.put()));
    if (ok) {
        stream = ScopedComPtr<IStream>(SHCreateMemStream(png.data(), (UINT)png.size()));
        ok = (bool)stream;
    }
    if (ok) ok = SUCCEEDED(factory->CreateDecoderFromStream(stream.get(), NULL,
                          WICDecodeMetadataCacheOnLoad, decoder.put())) && decoder;
    if (ok) ok = SUCCEEDED(decoder->GetFrame(0, frame.put())) && frame;
    if (ok) ok = SUCCEEDED(factory->CreateFormatConverter(converter.put())) && converter;
    if (ok) {
        ok = SUCCEEDED(converter->Initialize(frame.get(), GUID_WICPixelFormat32bppPBGRA,
                                             WICBitmapDitherTypeNone, NULL, 0.0f,
                                             WICBitmapPaletteTypeCustom));
    }
    UINT sw = 0, sh = 0;
    if (ok) ok = SUCCEEDED(converter->GetSize(&sw, &sh)) && sw && sh;
    if (ok) {
        const UINT stride = sw * 4;
        std::vector<BYTE> buf((size_t)stride * sh);
        if (SUCCEEDED(converter->CopyPixels(NULL, stride, (UINT)buf.size(), buf.data()))) {
            pixels.swap(buf);
            w = (int)sw;
            h = (int)sh;
        } else {
            ok = false;
        }
    }
    if (!ok) Wh_Log(L"[Glyph] could not decode the embedded icon");
    return ok;
}

// Builds the four bitmaps and alpha masks at the size the pane needs, and
// rebuilds them when the pane moves to a monitor with another DPI. Resampling
// is performed in C++ directly on the alpha channel so GDI StretchBlt never
// zeroes out the alpha byte at non-100% DPI.
static void EnsureGlyphs() {
    if (g_glyphScale == g_scale && !g_glyph[0].alpha.empty()) return;
    FreeGlyphs();
    const int dstW = std::max(1, S(k81GlyphWidth));
    const int dstH = std::max(1, S(k81GlyphHeight));
    ScopedWindowDc screen(NULL);
    bool allOk = true;
    for (int i = 0; i < TOPO_COUNT; i++) {
        std::vector<BYTE> srcAlpha;
        int sw = 0, sh = 0;
        if (!DecodeGlyphMask(kGlyphMaskB64[i], srcAlpha, sw, sh)) {
            std::vector<BYTE> bgra;
            if (DecodeGlyphPng(kGlyphPng[i], bgra, sw, sh) && sw > 0 && sh > 0) {
                srcAlpha.resize((size_t)sw * sh);
                for (int k = 0; k < sw * sh; ++k) srcAlpha[k] = bgra[k * 4 + 3];
            } else {
                allOk = false;
                continue;
            }
        }

        std::vector<BYTE> resampled((size_t)dstW * dstH, 0);
        if (dstW == sw && dstH == sh) {
            resampled = srcAlpha;
        } else {
            for (int y = 0; y < dstH; y++) {
                double sy = ((y + 0.5) * sh / (double)dstH) - 0.5;
                int y0 = (int)floor(sy);
                double fy = sy - y0;
                int y1 = std::min(sh - 1, std::max(0, y0 + 1));
                y0 = std::min(sh - 1, std::max(0, y0));
                for (int x = 0; x < dstW; x++) {
                    double sx = ((x + 0.5) * sw / (double)dstW) - 0.5;
                    int x0 = (int)floor(sx);
                    double fx = sx - x0;
                    int x1 = std::min(sw - 1, std::max(0, x0 + 1));
                    x0 = std::min(sw - 1, std::max(0, x0));
                    double a00 = srcAlpha[(size_t)y0 * sw + x0];
                    double a10 = srcAlpha[(size_t)y0 * sw + x1];
                    double a01 = srcAlpha[(size_t)y1 * sw + x0];
                    double a11 = srcAlpha[(size_t)y1 * sw + x1];
                    double a0 = a00 + (a10 - a00) * fx;
                    double a1 = a01 + (a11 - a01) * fx;
                    int a = (int)(a0 + (a1 - a0) * fy + 0.5);
                    resampled[(size_t)y * dstW + x] = (BYTE)std::min(255, std::max(0, a));
                }
            }
        }

        g_glyph[i].alpha = std::move(resampled);
        g_glyph[i].w = dstW;
        g_glyph[i].h = dstH;

        if (screen.get()) {
            BITMAPINFO bi = {};
            bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
            bi.bmiHeader.biWidth = dstW;
            bi.bmiHeader.biHeight = -dstH;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            void* dstBits = NULL;
            GdiObj<HBITMAP> dstBm(CreateDIBSection(screen.get(), &bi, DIB_RGB_COLORS, &dstBits, NULL, 0));
            if (dstBm.get() && dstBits) {
                BYTE* p = (BYTE*)dstBits;
                for (size_t k = 0; k < g_glyph[i].alpha.size(); ++k) {
                    BYTE a = g_glyph[i].alpha[k];
                    p[k * 4 + 0] = a;
                    p[k * 4 + 1] = a;
                    p[k * 4 + 2] = a;
                    p[k * 4 + 3] = a;
                }
                g_glyph[i].bm = std::move(dstBm);
            }
        }
    }
    if (allOk) g_glyphScale = g_scale;
}

// Composites one icon over the row behind it. Directly blends the resampled
// alpha mask onto the 32-bpp DIBSection backbuffer when available, with an
// AlphaBlend fallback.
static void DrawGlyph(HDC dc, BYTE* dstBits, int dstW, int dstH, int clipLeft,
                      int x, int y, int index, int alpha) {
    const GlyphBitmap& g = g_glyph[index];
    if (g.w <= 0 || g.h <= 0 || alpha <= 0) return;
    if (dstBits && !g.alpha.empty()) {
        GdiFlush();
        const int clampedAlpha = std::min(255, alpha);
        const int minX = std::max(0, clipLeft);
        for (int gy = 0; gy < g.h; gy++) {
            const int dy = y + gy;
            if (dy < 0 || dy >= dstH) continue;
            const BYTE* srcRow = g.alpha.data() + (size_t)gy * g.w;
            BYTE* dstRow = dstBits + (size_t)dy * dstW * 4;
            for (int gx = 0; gx < g.w; gx++) {
                const int dx = x + gx;
                if (dx < minX || dx >= dstW) continue;
                const int a = (srcRow[gx] * clampedAlpha + 127) / 255;
                if (a <= 0) continue;
                BYTE* p = dstRow + (size_t)dx * 4;
                p[0] = (BYTE)(p[0] + ((255 - p[0]) * a + 127) / 255);
                p[1] = (BYTE)(p[1] + ((255 - p[1]) * a + 127) / 255);
                p[2] = (BYTE)(p[2] + ((255 - p[2]) * a + 127) / 255);
            }
        }
        return;
    }
    if (!g.bm) return;
    DcObj mem(CreateCompatibleDC(dc));
    if (!mem.get()) return;
    ScopedSelectedObject sel(mem.get(), g.bm);
    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = (BYTE)std::min(255, alpha);
    bf.AlphaFormat = AC_SRC_ALPHA;
    AlphaBlend(dc, x, y, g.w, g.h, mem.get(), 0, 0, g.w, g.h, bf);
}

// Draws text so that the top of its capitals lands where the Windows 8.1
// capture has it (`cellTop = inkTop - (tmAscent - cap)`). Uses DT_NOCLIP and
// wraps onto a second line if a localized string exceeds `right - x`, so no
// text is ever cut off with an ellipsis or clipped vertically.
static void DrawTextInkTop(HDC dc, int x, int inkTop, int right, const wchar_t* text) {
    if (!text || !*text || right <= x) return;
    TEXTMETRICW tm;
    if (!GetTextMetricsW(dc, &tm) || !tm.tmAscent) return;
    int cap = 0;
    GLYPHMETRICS gm = {};
    const MAT2 identity = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 1, 0 } };
    if (GetGlyphOutlineW(dc, L'H', GGO_METRICS, &gm, 0, NULL, &identity) != GDI_ERROR &&
        gm.gmBlackBoxY > 0)
        cap = (int)gm.gmBlackBoxY;
    if (cap <= 0 || cap > tm.tmAscent) cap = tm.tmAscent * 3 / 4;

    const int cellTop = inkTop - (tm.tmAscent - cap);
    const int lineStep = tm.tmHeight + tm.tmExternalLeading;
    SIZE sz = {};
    const int len = lstrlenW(text);
    const bool singleFits = GetTextExtentPoint32W(dc, text, len, &sz) && sz.cx <= (right - x);
    RECT rc = { x, cellTop, right, cellTop + lineStep * 2 + S(12) };
    DrawTextW(dc, text, -1, &rc,
              DT_LEFT | DT_TOP | (singleFits ? DT_SINGLELINE : DT_WORDBREAK) |
                  DT_NOCLIP | DT_NOPREFIX);
}

// Draws an option label using the full available row width up to `right`, with
// DT_NOCLIP and word wrapping only when a localized label exceeds the row width
// so no option text is ever cut off or prematurely split.
static void DrawRowLabelInkTop(HDC dc, int x, int inkTop, int right, const wchar_t* text) {
    if (!text || !*text || right <= x) return;
    TEXTMETRICW tm;
    if (!GetTextMetricsW(dc, &tm) || !tm.tmAscent) return;
    int cap = 0;
    GLYPHMETRICS gm = {};
    const MAT2 identity = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 1, 0 } };
    if (GetGlyphOutlineW(dc, L'H', GGO_METRICS, &gm, 0, NULL, &identity) != GDI_ERROR &&
        gm.gmBlackBoxY > 0)
        cap = (int)gm.gmBlackBoxY;
    if (cap <= 0 || cap > tm.tmAscent) cap = tm.tmAscent * 3 / 4;

    const int cellTop = inkTop - (tm.tmAscent - cap);
    const int lineStep = tm.tmHeight + tm.tmExternalLeading;
    SIZE sz = {};
    const int len = lstrlenW(text);
    const bool singleFits = GetTextExtentPoint32W(dc, text, len, &sz) && sz.cx <= (right - x);
    RECT rc = { x, cellTop, right, cellTop + lineStep * 3 + S(12) };
    DrawTextW(dc, text, -1, &rc,
              DT_LEFT | DT_TOP | (singleFits ? DT_SINGLELINE : DT_WORDBREAK) |
                  DT_NOCLIP | DT_NOPREFIX);
}

// ---------------------------------------------------------------- layout (cached)
static Lay Layout() {
    if (g_cachedLayW == g_w && g_cachedLayH == g_h && g_cachedLayScale == g_scale) {
        return g_cachedLay;
    }
    Lay l;
    // All of these are the measurements of the Windows 8.1 pane, at 96 DPI.
    l.mx = S(42);
    l.titleInk = S(39);
    l.subInk = S(89);
    l.listY = S(124);
    l.rowH = S(120);
    l.glyphX = S(41);
    l.glyphY = S(38);
    l.textX = S(142);
    l.rowInk = S(45);
    l.border = S(2);
    l.bandX = S(1);
    g_cachedLay = l;
    g_cachedLayW = g_w;
    g_cachedLayH = g_h;
    g_cachedLayScale = g_scale;
    return l;
}

// ---------------------------------------------------------------- painting
static void DoPaint(HWND h) {
    ScopedPaint dc(h);
    if (!dc.get()) return;
    RECT rc; GetClientRect(h, &rc);
    int W = rc.right, H = rc.bottom;
    if (W <= 0 || H <= 0) return;

    DcObj mem(CreateCompatibleDC(dc.get()));
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = W;
    bi.bmiHeader.biHeight = -H; // top-down 32-bpp DIBSection for direct glyph compositing
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    BYTE* bmpBits = NULL;
    HBITMAP rawBmp = CreateDIBSection(dc.get(), &bi, DIB_RGB_COLORS, (void**)&bmpBits, NULL, 0);
    if (!rawBmp) rawBmp = CreateCompatibleBitmap(dc.get(), W, H);
    GdiObj<HBITMAP> bmp(rawBmp);
    ScopedSelectedObject selBmp(mem.get(), bmp.get());

    { GdiObj<HBRUSH> kb(CreateSolidBrush(KEY)); FillRect(mem.get(), &rc, kb.get()); }

    COLORREF pane = PaneColor();
    int px = std::max(0, g_paneOff);
    if (px < W) {
        RECT pr = { px, 0, W, H };
        { GdiObj<HBRUSH> b(CreateSolidBrush(pane)); FillRect(mem.get(), &pr, b.get()); }
        RECT br = { px, 0, std::min(W, px + std::max(1, S(1))), H };
        { GdiObj<HBRUSH> b(CreateSolidBrush(PaneBorderColor(pane))); FillRect(mem.get(), &br, b.get()); }
    }
    {
        ScopedClipRgn clip(mem.get(), px, 0, W, H);
        ScopedBkMode bkMode(mem.get(), TRANSPARENT);
        EnsureFonts();
        EnsureGlyphs();
        Lay L = Layout();
        int ox = g_paneOff + g_contOff;
        int oxT = g_paneOff + g_contOff * 55 / 100;

        // A snapshot: painting must never block on a refresh from the watchdog
        // thread, and must never see the model change halfway through a frame.
        int topology = TOPO_UNKNOWN, connected = 0;
        {
            SrwGuard g(&g_lock, false);
            topology = g_topology;
            connected = g_connected;
        }

        // The title and the tagline of Windows 8.1. The tagline is always
        // T(STR_SUBTITLE) unless a status message is active, which temporarily replaces it.
        {
            ScopedTextColor tc(mem.get(), RGB(255, 255, 255));
            ScopedSelectedObject so(mem.get(), g_fTitle.get());
            const wchar_t* title = T(g_titleMode == 1 ? STR_TITLE_SECOND : STR_TITLE_PROJECT);
            DrawTextInkTop(mem.get(), L.mx + oxT, L.titleInk, W - S(16) + oxT, title);

            SelectObject(mem.get(), g_fLabel.get());
            std::wstring status;
            {
                SrwGuard sg(&g_lock, false);
                if (g_statusUntil && GetTickCount() < g_statusUntil) status = g_status;
            }
            DrawTextInkTop(mem.get(), L.mx + ox, L.subInk, W - S(16) + ox,
                           status.empty() ? T(STR_SUBTITLE) : status.c_str());
        }

        // The four projection options: the icon of Windows 8.1, then its label.
        // The selected row gets the dark selection rectangle (#2C3030) with the
        // 2 px white focus frame from the Windows 8.1 capture.
        const COLORREF bandColor = SelectFillColor(pane);
        const int selectedRow = (g_sel >= 0 && g_sel < TOPO_COUNT)
                                    ? g_sel
                                    : ((topology >= 0 && topology < TOPO_COUNT) ? topology : TOPO_INTERNAL);
        for (int i = 0; i < TOPO_COUNT; i++) {
            const int y = L.listY + i * L.rowH;
            const bool available = (i == TOPO_INTERNAL) || connected >= 2 || !g_disableUnavailable;
            if (i == selectedRow) {
                RECT r = { L.bandX + ox, y, W + ox, y + L.rowH };
                { GdiObj<HBRUSH> b(CreateSolidBrush(bandColor)); FillRect(mem.get(), &r, b.get()); }
                if (g_state != ST_OUT) {
                    GdiObj<HBRUSH> f(CreateSolidBrush(RGB(255, 255, 255)));
                    if (f.get()) {
                        const RECT edges[4] = {
                            { r.left, r.top, r.right, r.top + L.border },
                            { r.left, r.bottom - L.border, r.right, r.bottom },
                            { r.left, r.top, r.left + L.border, r.bottom },
                            { r.right - L.border, r.top, r.right, r.bottom },
                        };
                        for (const RECT& e : edges) FillRect(mem.get(), &e, f.get());
                    }
                }
            } else if (i == g_hoverOpt) {
                RECT r = { L.bandX + ox, y, W + ox, y + L.rowH };
                GdiObj<HBRUSH> b(CreateSolidBrush(Blend(pane, 18)));
                FillRect(mem.get(), &r, b.get());
            }
            DrawGlyph(mem.get(), bmpBits, W, H, px, L.glyphX + ox, y + L.glyphY, i, available ? 255 : 110);
            ScopedSelectedObject so(mem.get(), g_fLabel.get());
            ScopedTextColor tc(mem.get(), available ? RGB(255, 255, 255) : Blend(pane, 50));
            DrawRowLabelInkTop(mem.get(), L.textX + ox, y + L.rowInk, W - S(16) + ox, OptionName(i));
        }
    }

    BitBlt(dc.get(), 0, 0, W, H, mem.get(), 0, 0, SRCCOPY);
}

// ---------------------------------------------------------------- animation
static LONGLONG Qpc() { LARGE_INTEGER c; QueryPerformanceCounter(&c); return c.QuadPart; }

static double Ease(double t) {
    if (t <= 0) return 0;
    if (t >= 1) return 1;
    double u = 1 - t;
    return 1 - u*u*u;
}

static void StartAnim(bool in) {
    if (in && g_state == ST_IN) return;
    if (!in && g_state == ST_OUT) return;
    g_open = in;
    g_fromPane = g_paneOff;
    g_toPane = in ? 0 : g_w;
    g_fromCont = g_contOff;
    double frac = g_w ? fabs((double)(g_fromPane - g_toPane)) / g_w : 1.0;
    int full = in ? g_openMs : g_closeMs;
    g_dur = std::max(80, (int)(full * frac));
    g_contDur = in ? (int)(g_dur * 1.5) : g_dur;
    g_state = in ? ST_IN : ST_OUT;
    g_t0 = Qpc();
    SetTimer(g_wnd, 1, 8, NULL);
}

static void Tick() {
    double ms = (double)(Qpc() - g_t0) * 1000.0 / (double)g_freq;
    double t = std::min(1.0, ms / g_dur);
    double tc = std::min(1.0, ms / g_contDur);
    double dp = (g_toPane - g_fromPane) * Ease(t);
    if (fabs((g_toPane - g_fromPane) - dp) < 0.75) dp = g_toPane - g_fromPane;
    g_paneOff = g_fromPane + (int)(dp + (dp >= 0 ? 0.5 : -0.5));
    double dc2 = (0 - g_fromCont) * Ease(tc);
    if (fabs(g_fromCont + dc2) < 0.75) dc2 = -g_fromCont;
    g_contOff = g_fromCont + (int)(dc2 + (dc2 >= 0 ? 0.5 : -0.5));
    if (t >= 1.0 && tc >= 1.0) {
        g_paneOff = g_toPane; g_contOff = 0;
        KillTimer(g_wnd, 1);
        if (g_state == ST_OUT) {
            KillTimer(g_wnd, 2);   // the status line is gone with the panel
            ShowWindow(g_wnd, SW_HIDE);
            // The mouse hook is used only while the panel is visible. The
            // Explorer Win+P hook remains active while its setting is enabled.
            SetOutsideClickHook(false);
            SetKeyboardHook(false);
        }
        g_state = ST_IDLE;
    }
    RedrawWindow(g_wnd, NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
}

static void InvalidatePanel() {
    if (g_wnd) InvalidateRect(g_wnd, NULL, FALSE);
}

static void SetStatus(const std::wstring& text) {
    {
        SrwGuard g(&g_lock, true);
        g_status = text;
    }
    g_statusUntil = text.empty() ? 0 : GetTickCount() + kStatusMs;
    InvalidatePanel();
}

// ---------------------------------------------------------------- shell helpers
static void CopyTextToClipboard(HWND owner, const std::wstring& text) {
    ScopedClipboard clip(owner);
    if (!clip.isOpen()) return;
    EmptyClipboard();
    const size_t len = (text.size() + 1) * sizeof(wchar_t);
    ScopedGlobalAlloc g(len, GMEM_MOVEABLE);
    if (g.get()) {
        {
            ScopedGlobalLock lock(g.get());
            if (!lock.get()) return;
            memcpy(lock.get(), text.c_str(), len);
        }
        if (SetClipboardData(CF_UNICODETEXT, g.get())) {
            (void)g.release(); // ownership transferred to the clipboard
        }
    }
}

// ---------------------------------------------------------------- Metro context menu
static int MenuItemHeight() { return S(34); }
static int MenuPadY()       { return S(6);  }
static int MenuPadX()       { return S(14); }

static int MenuWidth() {
    int maxW = S(220);
    ScopedWindowDc dc(NULL);
    if (dc.get()) {
        GdiObj<HFONT> font(CreateFontW(-S(15), 0, 0, 0, FW_NORMAL, 0, 0, 0,
                                       DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI"));
        ScopedSelectedObject selFont(dc.get(), font.get());
        for (const MenuEntry& e : g_menu) {
            if (e.cmd == MC_SEP || e.text.empty()) continue;
            SIZE sz = {};
            if (GetTextExtentPoint32W(dc.get(), e.text.c_str(), (int)e.text.size(), &sz)) {
                maxW = std::max(maxW, (int)sz.cx + MenuPadX() * 2 + S(18));
            }
        }
    }
    return std::min(S(520), maxW);
}

static bool MenuIsSelectable(int i) {
    return i >= 0 && i < (int)g_menu.size() && g_menu[i].cmd != MC_SEP;
}

// Next selectable index in the given direction, wrapping around. from = -1
// starts before the first item (dir > 0) or after the last one (dir < 0).
static int MenuStep(int from, int dir) {
    const int n = (int)g_menu.size();
    if (n == 0) return -1;
    int i = (from < 0 || from >= n) ? (dir > 0 ? -1 : n) : from;
    for (int k = 0; k < n; k++) {
        i += dir;
        if (i < 0) i = n - 1;
        if (i >= n) i = 0;
        if (MenuIsSelectable(i)) return i;
    }
    return -1;
}

static int MenuHitTest(HWND h, int x, int y) {
    RECT rc; GetClientRect(h, &rc);
    if (x < 0 || x >= rc.right) return -1;
    int rel = y - MenuPadY();
    if (rel < 0) return -1;
    int idx = rel / MenuItemHeight();
    return MenuIsSelectable(idx) ? idx : -1;
}

static void MenuSetHover(HWND h, int idx) {
    if (idx != g_menuHover) {
        g_menuHover = idx;
        InvalidateRect(h, NULL, FALSE);
    }
}

// Records why the menu ends (first reason wins) and closes it asynchronously.
// `entry` is the index of the chosen item, -1 for a dismissal without a
// command; the row a command applies to comes from the entry itself.
static void MenuFinish(HWND h, int reason, int entry) {
    if (g_menuEndReason == MENU_END_NONE) {
        g_menuEndReason = reason;
        g_menuResult = MC_NONE;
        g_menuResultRow = -1;
        if (entry >= 0 && entry < (int)g_menu.size()) {
            g_menuResult = g_menu[entry].cmd;
            g_menuResultRow = g_menu[entry].row;
        }
    }
    PostMessageW(h, WM_CLOSE, 0, 0);
}

// The menu lost focus/activation to `to`. Moving to the panel itself counts as
// an inside dismissal; any other window (or another thread's window, reported
// as NULL) is a click outside.
static void MenuDeactivated(HWND h, HWND to) {
    if (to && to == h) return;
    MenuFinish(h, (to && to == g_wnd) ? MENU_END_INSIDE : MENU_END_OUTSIDE, -1);
}

static void MenuPaint(HWND h) {
    ScopedPaint dc(h);
    if (!dc.get()) return;
    RECT rc; GetClientRect(h, &rc);

    GdiObj<HBRUSH> bg(CreateSolidBrush(RGB(255, 255, 255)));
    FillRect(dc.get(), &rc, bg.get());

    GdiObj<HPEN> border(CreatePen(PS_SOLID, 1, RGB(200, 200, 200)));
    {
        ScopedSelectedObject selPen(dc.get(), border.get());
        ScopedSelectedObject selBrush(dc.get(), GetStockObject(NULL_BRUSH));
        Rectangle(dc.get(), rc.left, rc.top, rc.right, rc.bottom);
    }

    GdiObj<HFONT> font(CreateFontW(-S(15), 0, 0, 0, FW_NORMAL, 0, 0, 0,
                                   DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI"));
    ScopedSelectedObject selFont(dc.get(), font.get());
    ScopedBkMode bkMode(dc.get(), TRANSPARENT);

    int itemH = MenuItemHeight();
    int padY = MenuPadY();
    int padX = MenuPadX();

    COLORREF accent = SystemAccentColor();

    for (size_t i = 0; i < g_menu.size(); ++i) {
        RECT r = { 1, padY + (int)i * itemH, rc.right - 1, padY + (int)(i + 1) * itemH };
        const bool isSep = g_menu[i].cmd == MC_SEP;
        const bool hover = ((int)i == g_menuHover) && !isSep;

        if (isSep) {
            GdiObj<HBRUSH> sep(CreateSolidBrush(RGB(224, 224, 224)));
            RECT sr = { padX, r.top + itemH / 2 - 1, rc.right - padX, r.top + itemH / 2 };
            FillRect(dc.get(), &sr, sep.get());
            continue;
        }

        if (hover) {
            GdiObj<HBRUSH> hb(CreateSolidBrush(accent));
            FillRect(dc.get(), &r, hb.get());
        }
        ScopedTextColor tc(dc.get(), hover ? RGB(255, 255, 255) : RGB(32, 32, 32));

        RECT tr = r;
        tr.left += padX;
        tr.right -= padX;
        DrawTextW(dc.get(), g_menu[i].text.c_str(), -1, &tr,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_NOCLIP);
    }
}

static LRESULT CALLBACK MenuWndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    return RunGuardedValue<LRESULT>(L"MenuWndProc", 0, [&]() -> LRESULT {
        switch (msg) {
        case WM_PAINT:
            MenuPaint(h);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEMOVE:
            MenuSetHover(h, MenuHitTest(h, (short)LOWORD(lp), (short)HIWORD(lp)));
            return 0;
        case WM_LBUTTONDOWN: {
            int idx = MenuHitTest(h, (short)LOWORD(lp), (short)HIWORD(lp));
            if (idx >= 0) MenuFinish(h, MENU_END_INSIDE, idx);
            return 0;   // clicks on the padding or on a separator keep the menu open
        }
        case WM_RBUTTONDOWN:
            MenuFinish(h, MENU_END_INSIDE, -1);
            return 0;
        case WM_KEYDOWN:
            switch (wp) {
            case VK_ESCAPE:
                MenuFinish(h, MENU_END_INSIDE, -1);
                break;
            case VK_DOWN:
                MenuSetHover(h, MenuStep(g_menuHover, +1));
                break;
            case VK_UP:
                MenuSetHover(h, MenuStep(g_menuHover, -1));
                break;
            case VK_HOME:
                MenuSetHover(h, MenuStep(-1, +1));
                break;
            case VK_END:
                MenuSetHover(h, MenuStep(-1, -1));
                break;
            case VK_RETURN:
            case VK_SPACE:
                if (MenuIsSelectable(g_menuHover))
                    MenuFinish(h, MENU_END_INSIDE, g_menuHover);
                break;
            case VK_APPS:
                MenuFinish(h, MENU_END_INSIDE, -1);
                break;
            }
            return 0;
        case WM_SYSKEYDOWN:
            // Alt / F10 close the menu like a native popup menu.
            MenuFinish(h, MENU_END_INSIDE, -1);
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE) MenuDeactivated(h, (HWND)lp);
            return 0;
        case WM_KILLFOCUS:
            MenuDeactivated(h, (HWND)wp);
            return 0;
        case WM_CLOSE:
            DestroyWindow(h);
            return 0;
        case WM_DESTROY:
            if (g_menuWnd == h) g_menuWnd = NULL;
            return 0;
        }
        return DefWindowProcW(h, msg, wp, lp);
    });
}

static bool EnsureMenuClass() {
    if (g_menuClassRegistered) return true;

    WNDCLASSW wc = {};
    wc.style = CS_DROPSHADOW;
    wc.lpfnWndProc = MenuWndProc;
    wc.hInstance = (HINSTANCE)&__ImageBase;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = kMenuClass;
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"[Menu] RegisterClassW failed, err=%u", (unsigned)GetLastError());
        return false;
    }

    g_menuClassRegistered = true;
    return true;
}

static void UnregisterMenuClass() {
    if (g_menuClassRegistered) {
        UnregisterClassW(kMenuClass, (HINSTANCE)&__ImageBase);
        g_menuClassRegistered = false;
    }
}

// Keeps the menu inside the work area of the monitor under (x, y), flipping it
// to the left/top of the anchor point when it would overflow, like a native menu.
static void PlaceMenu(int& x, int& y, int w, int hgt) {
    POINT pt = { x, y };
    RECT work = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (mon && GetMonitorInfoW(mon, &mi)) work = mi.rcWork;

    if (x + w > work.right) x -= w;
    if (y + hgt > work.bottom) y -= hgt;
    x = std::max((int)work.left, std::min(x, (int)work.right - w));
    y = std::max((int)work.top, std::min(y, (int)work.bottom - hgt));
}

// Creates the menu in the same z-band as the panel. A window in the default
// band would always be drawn below a panel living in ZBID_UIACCESS.
static HWND CreateMenuWindow(HWND owner, int x, int y, int w, int hgt) {
    const DWORD ex = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
    HINSTANCE hi = (HINSTANCE)&__ImageBase;
    HWND menu = NULL;
    if (g_panelBand != 0 && g_createInBand) {
        menu = g_createInBand(ex, kMenuClass, L"", WS_POPUP, x, y, w, hgt,
                              owner, NULL, hi, NULL, g_panelBand);
        if (!menu)
            Wh_Log(L"[Menu] CreateWindowInBand(%u) failed, err=%u; the menu may appear behind the panel",
                   (unsigned)g_panelBand, (unsigned)GetLastError());
    }
    if (!menu)
        menu = CreateWindowExW(ex, kMenuClass, L"", WS_POPUP, x, y, w, hgt,
                               owner, NULL, hi, NULL);
    return menu;
}

// The pane itself shows only what Windows 8.1 showed, so every command that
// goes beyond the four options lives here. The labels name the documented
// operation they perform, and a "Set as main display" entry is offered for each
// display that is active but not the main one.
static void ShowContextMenu(HWND h, int x, int y, bool fromKeyboard) {
    if (g_inMenu) return;
    std::vector<MonRow> rows;
    {
        SrwGuard g(&g_lock, false);
        rows = g_monitors;
    }
    if (!EnsureMenuClass()) return;

    if (g_menuWnd) { HWND old = g_menuWnd; g_menuWnd = NULL; DestroyWindow(old); }

    g_menu.clear();
    bool anySetMain = false;
    if (g_allowSetPrimary) {
        for (size_t k = 0; k < rows.size(); k++) {
            if (!rows[k].active || rows[k].primary) continue;
            std::wstring label = T(STR_SETMAIN);
            // With more than one candidate the name tells them apart.
            if (!rows[k].name.empty())
                label += L" \u2014 " + rows[k].name;
            g_menu.push_back({ label, MC_SETMAIN, (int)k });
            anySetMain = true;
        }
    }
    if (anySetMain) g_menu.push_back({ L"", MC_SEP, -1 });
    g_menu.push_back({ T(STR_DETECT), MC_DETECT, -1 });
    g_menu.push_back({ T(STR_MORE), MC_MORE, -1 });
    if (!rows.empty()) g_menu.push_back({ T(STR_COPYINFO), MC_COPY, -1 });

    int itemH = MenuItemHeight();
    int padY = MenuPadY();
    int w = MenuWidth();
    int hgt = padY * 2 + (int)g_menu.size() * itemH;
    PlaceMenu(x, y, w, hgt);

    g_inMenu = true;
    g_menuHover = fromKeyboard ? MenuStep(-1, +1) : -1;
    g_menuResult = MC_NONE;
    g_menuResultRow = -1;
    g_menuEndReason = MENU_END_NONE;

    HWND menu = CreateMenuWindow(h, x, y, w, hgt);
    if (!menu) {
        Wh_Log(L"[Menu] could not create the context menu, err=%u", (unsigned)GetLastError());
        g_inMenu = false;
        return;
    }

    g_menuWnd = menu;
    SetWindowPos(menu, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetForegroundWindow(menu);
    SetFocus(menu);

    // Modal loop. GetMessageW returns 0 for WM_QUIT: re-post it so the UI
    // thread's main loop also exits (unload must never hang here).
    bool quitReceived = false;
    MSG msg;
    while (g_menuWnd) {
        BOOL ret = GetMessageW(&msg, NULL, 0, 0);
        if (ret == 0) {
            quitReceived = true;
            PostQuitMessage((int)msg.wParam);
            break;
        }
        if (ret == -1) {
            Wh_Log(L"[Menu] GetMessageW failed, err=%u", (unsigned)GetLastError());
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (g_menuWnd) {
        HWND mm = g_menuWnd;
        g_menuWnd = NULL;
        DestroyWindow(mm);
    }

    g_inMenu = false;
    const int cmd = g_menuResult;
    const int row = g_menuResultRow;
    const int reason = g_menuEndReason;
    g_menuResult = MC_NONE;
    g_menuResultRow = -1;
    g_menuEndReason = MENU_END_NONE;
    g_menuHover = -1;

    if (quitReceived || !g_wnd) return;

    bool paneClosing = false;
    switch (cmd) {
    case MC_SETMAIN:
        // ChangeDisplaySettingsEx with CDS_SET_PRIMARY, see SetMainDisplay().
        SetMainDisplay(row);
        break;
    case MC_MORE:
        // The ms-settings page chosen by the More settings target setting.
        OpenMoreSettings();
        ClosePane();
        paneClosing = true;
        break;
    case MC_COPY: {
        std::wstring all;
        for (const MonRow& m : rows) {
            if (!all.empty()) all += L"\r\n\r\n";
            all += MonitorInfoText(m);
        }
        CopyTextToClipboard(h, all);
        break;
    }
    case MC_DETECT:
        DetectDisplays();
        break;
    default:
        break;
    }

    if (reason == MENU_END_OUTSIDE) {
        // Another window took the focus: never pull it back. Honor the
        // "close when clicking outside" setting that was suspended while the
        // menu was open.
        if (g_open && g_closeOnClickOutside) ClosePane();
        return;
    }
    if (reason == MENU_END_CANCEL || paneClosing || !g_open) return;

    SetForegroundWindow(g_wnd);
    SetFocus(g_wnd);
}

// ---------------------------------------------------------------- dismiss/close
static void DismissContextMenu() {
    if (g_menuWnd) MenuFinish(g_menuWnd, MENU_END_CANCEL, -1);
}

static void ClosePane() {
    if (!g_wnd || !g_open) return;
    Wh_Log(L"[Pane] ClosePane");
    DismissContextMenu();
    StartAnim(false);
}

// ---------------------------------------------------------------- light dismiss
// Closing on "click outside" cannot rely only on WM_ACTIVATE / WM_KILLFOCUS.
// Those messages arrive only if the panel really became the foreground window,
// and SetForegroundWindow can be refused by the system (see its documentation:
// "It is possible for a process to be denied the right to set the foreground
// window even if it meets these conditions"). Clicks on windows that never take
// activation (taskbar areas, overlays, windows in other z-bands) do not
// deactivate the panel either. Two activation-independent detectors are used:
//   1. a WH_MOUSE_LL hook, installed only while the panel is visible, on the
//      dedicated input-hook thread; it just posts the click point here;
//   2. a WinEvent hook for EVENT_SYSTEM_FOREGROUND (out of context, delivered
//      on this UI thread), which catches Alt+Tab, Win key, taskbar clicks, etc.

// Asks the input-hook thread to install/remove the low-level mouse hook.
static void SetOutsideClickHook(bool on) {
    if (g_mouseHookWanted == on) return;
    const DWORD tid = g_keyboardTid;
    if (!tid) return;
    if (PostThreadMessageW(tid, on ? WM_HOOKCTL_MOUSE_ON : WM_HOOKCTL_MOUSE_OFF, 0, 0))
        g_mouseHookWanted = on;
    else
        Wh_Log(L"[Dismiss] PostThreadMessageW failed, err=%u", (unsigned)GetLastError());
}

// The Explorer Win+P hook stays installed while the setting is enabled, even
// when the panel is hidden, because it is also responsible for opening it. The
// on/off parameter is kept for the panel lifecycle callers; the setting is the
// actual lifetime gate.
static void SetKeyboardHook(bool on) {
    (void)on;
    const bool wanted = g_useWinP;
    if (g_keyboardHookWanted == wanted) return;
    const DWORD tid = g_keyboardTid;
    if (!tid) return;
    if (PostThreadMessageW(tid,
                           wanted ? WM_HOOKCTL_KEYBOARD_ON : WM_HOOKCTL_KEYBOARD_OFF,
                           0, 0))
        g_keyboardHookWanted = wanted;
    else
        Wh_Log(L"[Hook] PostThreadMessageW failed, err=%u", (unsigned)GetLastError());
}

// Visible part of the panel: during the slide animation the left part of the
// window is still transparent (color key) and does not count as "inside".
static bool PointInPanel(POINT pt) {
    if (!g_wnd || !IsWindowVisible(g_wnd)) return false;
    RECT r;
    if (!GetWindowRect(g_wnd, &r)) return false;
    r.left += std::max(0, g_paneOff);
    return PtInRect(&r, pt) != FALSE;
}

static bool PointInMenu(POINT pt) {
    if (!g_menuWnd || !IsWindowVisible(g_menuWnd)) return false;
    RECT r;
    return GetWindowRect(g_menuWnd, &r) && PtInRect(&r, pt);
}

static void OnGlobalPointerDown(POINT pt) {
    if (!g_wnd || !g_open) return;
    if (PointInMenu(pt)) return;

    if (g_inMenu) {
        // Clicks on the panel are handled by the menu's own focus logic
        // (inside dismissal). Anything else is outside: the menu code then
        // closes the panel too when "Close when clicking outside" is on.
        if (!PointInPanel(pt) && g_menuWnd)
            MenuFinish(g_menuWnd, MENU_END_OUTSIDE, -1);
        return;
    }
    if (PointInPanel(pt)) return;

    if (g_closeOnClickOutside) {
        ClosePane();
    }
}

static bool IsOurWindow(HWND hwnd) {
    if (!hwnd) return false;
    if (hwnd == g_wnd || (g_menuWnd && hwnd == g_menuWnd)) return true;
    HWND root = GetAncestor(hwnd, GA_ROOTOWNER);
    return root && root == g_wnd;
}

static const DWORD kOpenDismissGraceMs = 450;

static void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                         LONG idObject, LONG, DWORD, DWORD eventTime) {
    RunGuarded(L"ForegroundEventProc", [&] {
        if (event != EVENT_SYSTEM_FOREGROUND || idObject != OBJID_WINDOW || !hwnd) return;

        // If the native DisplaySwitch flyout activates while replacement is enabled,
        // dismiss it immediately and open our pane instead. This is also the fallback
        // for non-keyboard routes such as the Windows 8/8.1 Devices charm and a
        // no-argument DisplaySwitch.exe launch; those routes do not deliver a
        // keyboard event to this hook.
        if (g_useWinP) {
            wchar_t cls[64] = {};
            if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && !wcscmp(cls, L"DisplaySwitchUIWnd")) {
                PostMessageW(hwnd, WM_CLOSE, 0, 0);
                ShowWindow(hwnd, SW_HIDE);
                if (!g_open && g_wnd) PostMessageW(g_wnd, WM_APP_TOGGLE, 0, 0);
                return;
            }
        }

        if (!g_wnd || !g_open || IsOurWindow(hwnd)) return;
        // Ignore foreground bounces during the slide-in animation or within the
        // initial grace window right after Win+P is pressed/released.
        if (g_state == ST_IN) return;
        if ((LONG)(eventTime - g_openTick) < (LONG)kOpenDismissGraceMs) return;
        if ((DWORD)(GetTickCount() - g_openTick) < kOpenDismissGraceMs) return;

        if (g_inMenu) {
            if (g_menuWnd) MenuFinish(g_menuWnd, MENU_END_OUTSIDE, -1);
            return;
        }
        if (g_closeOnClickOutside) ClosePane();
    });
}

// ---------------------------------------------------------------- topmost enforcement
static int  g_raiseCount = 0;
static DWORD g_raiseSince = 0;
static bool g_gaveUp = false;

static void EnforceTopmost() {
    if (!g_wnd || !g_aboveTaskbar) return;

    if (g_gaveUp) {
        SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
        return;
    }

    RECT r; GetWindowRect(g_wnd, &r);
    POINT c = { (r.left + r.right) / 2, (r.top + r.bottom) / 2 };
    bool covered = (WindowFromPoint(c) != g_wnd);

    if (!covered) {
        SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
        g_raiseCount = 0;
        g_raiseSince = 0;
        return;
    }

    DWORD now = GetTickCount();
    if (now - g_raiseSince > 1000) { g_raiseSince = now; g_raiseCount = 0; }
    if (++g_raiseCount > 5) {
        g_gaveUp = true;
        Wh_Log(L"[ZBand] panel keeps getting covered, giving up re-raising (plain topmost only)");
        DebugDump(L"gave up");
        return;
    }
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
}

struct MonCtx { int target; int cur; HMONITOR result; };
static BOOL CALLBACK MonEnumProc(HMONITOR hm, HDC, LPRECT, LPARAM lp) {
    return RunGuardedValue<BOOL>(L"MonEnumProc", TRUE, [&]() -> BOOL {
        auto* c = (MonCtx*)lp;
        if (c->cur == c->target) { c->result = hm; return FALSE; }
        c->cur++;
        return TRUE;
    });
}

static HMONITOR PickMonitor() {
    if (g_monitorChoice <= 0) {
        POINT pt; GetCursorPos(&pt);
        return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    }
    MonCtx c{ g_monitorChoice - 1, 0, nullptr };
    EnumDisplayMonitors(nullptr, nullptr, MonEnumProc, (LPARAM)&c);
    if (c.result) return c.result;
    POINT pt = {0, 0};
    return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
}

static void OpenPane() {
    if (!g_wnd) return;
    const bool wasVisible = IsWindowVisible(g_wnd) != FALSE && g_open;
    if (!wasVisible) {
        HMONITOR target = PickMonitor();
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfoW(target, &mi);
        RECT r = mi.rcMonitor;

        UINT dpiX = 96, dpiY = 96;
        if (FAILED(GetDpiForMonitor(target, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
            dpiX = dpiY = 96;
        }
        g_scale = dpiX / 96.0;

        g_w = S(k81FlyoutWidth);

        // Invalidate the Layout cache since the panel dimensions changed.
        g_cachedLayW = -1;

        // The Windows 8.1 flyout spans the whole right edge of the screen; the
        // compact mode ends the window right below the fourth option.
        const int full = r.bottom - r.top;
        g_h = g_fullHeight ? full : std::min(full, PaneContentHeight());

        // The panel shows the live state, so read it right before animating in.
        RefreshState();
        int topo = TOPO_UNKNOWN, conn = 0;
        {
            SrwGuard g(&g_lock, false);
            topo = g_topology;
            conn = g_connected;
            g_sel = (g_topology >= 0 && g_topology < TOPO_COUNT) ? g_topology : TOPO_INTERNAL;
        }
        g_hoverOpt = -1;
        g_raiseCount = 0; g_raiseSince = 0; g_gaveUp = false;
        SetStatus(L"");
        if (g_state == ST_IDLE || !IsWindowVisible(g_wnd)) {
            g_paneOff = g_w;
            g_contOff = S(160);
        }
        SetWindowPos(g_wnd, HWND_TOPMOST, r.right - g_w, r.top, g_w, g_h,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        InvalidateRect(g_wnd, NULL, FALSE);
        SetTimer(g_wnd, 2, 1000, NULL);
        EnforceTopmost();
        Wh_Log(L"[Pane] OpenPane (x=%d y=%d w=%d h=%d scale=%.2f band=%u topo=%d connected=%d)",
               (int)(r.right - g_w), (int)r.top, g_w, g_h, g_scale,
               (unsigned)g_panelBand, topo, conn);
        if (g_debug) DebugDump(L"after OpenPane");
    }
    g_openTick = GetTickCount();
    StartAnim(true);
    // Light dismiss must work even if activation below is refused. The Explorer
    // Win+P hook is already active while enabled and never forwards ordinary
    // keystrokes to the panel.
    SetOutsideClickHook(true);
    SetKeyboardHook(true);

    // A zero-input SendInput call gives this thread the documented opportunity
    // to set the foreground window without attaching its input queue to another
    // application's thread. Sharing another application's input queue is deliberately
    // avoided: a hung foreground application must not freeze the panel or the display change.
    INPUT input{};
    SendInput(1, &input, sizeof(input));
    SetForegroundWindow(g_wnd);
    if (GetForegroundWindow() == g_wnd)
        SetFocus(g_wnd);
    g_openTick = GetTickCount();

    if (GetForegroundWindow() != g_wnd) {
        // The system may still deny the foreground change. The pane remains
        // usable with the mouse and closes on outside clicks via the hook.
        static RateLimitedLog log{L"foreground denied", 5};
        if (log.ShouldLog())
            Wh_Log(L"[Dismiss] SetForegroundWindow was refused (foreground=%p)",
                   GetForegroundWindow());
    }
}

// ---------------------------------------------------------------- UI helpers
static int OptionAt(int, int y) {
    Lay L = Layout();
    if (y < L.listY) return -1;
    int i = (y - L.listY) / L.rowH;
    return (i >= 0 && i < TOPO_COUNT) ? i : -1;
}

// Bottom of the pane content, so the "not full height" mode can end the window
// right below the fourth option.
static int PaneContentHeight() {
    Lay L = Layout();
    return L.listY + TOPO_COUNT * L.rowH + S(12);
}

// ---------------------------------------------------------------- window procedure
static LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        switch (msg) {
        case WM_PAINT: DoPaint(h); return 0;
        case WM_ERASEBKGND: return 1;
        case WM_CANCELMODE:
            return 0;
        case WM_DISPLAYCHANGE:
            // Documented as sent to all top-level windows when the display
            // settings change, so even a change made by the native shortcut, by
            // the Settings app, or by a monitor hotplug updates the panel.
            RefreshState();
            InvalidateRect(h, NULL, FALSE);
            return 0;
        case WM_TIMER:
            if (wp == 1) Tick();
            else if (wp == 2) {
                if (g_statusUntil && GetTickCount() >= g_statusUntil) {
                    g_statusUntil = 0;
                    {
                        SrwGuard g(&g_lock, true);
                        g_status.clear();
                    }
                    if (IsWindowVisible(h)) InvalidateRect(h, NULL, FALSE);
                }
            } else if (wp == 3) {
                if (g_open && !g_inMenu) EnforceTopmost();
            }
            return 0;
        case WM_APP_CYCLE:
            if (g_open && g_state != ST_OUT) {
                g_sel = (g_sel + 1) % TOPO_COUNT;
                InvalidateRect(h, NULL, FALSE);
            }
            return 0;
        case WM_APP_COMMIT:
            if (g_open && g_state != ST_OUT) ApplyTopology(g_sel, L"Win+P");
            return 0;
        case WM_APP_TOGGLE:
            if (g_open) { DismissContextMenu(); ClosePane(); }
            else OpenPane();
            return 0;
        case WM_APP_POINTER_DOWN: {
            POINT pt = { (LONG)(LONG_PTR)wp, (LONG)(LONG_PTR)lp };
            OnGlobalPointerDown(pt);
            return 0;
        }
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE && g_open && g_closeOnClickOutside &&
                g_state == ST_IDLE && !g_inMenu &&
                (DWORD)(GetTickCount() - g_openTick) >= kOpenDismissGraceMs)
                ClosePane();
            return 0;
        case WM_KILLFOCUS:
            if (g_open && g_closeOnClickOutside && g_state == ST_IDLE && !g_inMenu &&
                (DWORD)(GetTickCount() - g_openTick) >= kOpenDismissGraceMs)
                ClosePane();
            return 0;
        case WM_MOUSEACTIVATE: return MA_ACTIVATE;
        case WM_SETCURSOR: {
            if (LOWORD(lp) == HTCLIENT) {
                POINT pt; GetCursorPos(&pt); ScreenToClient(h, &pt);
                const bool clickable = OptionAt(pt.x, pt.y) >= 0;
                SetCursor(LoadCursorW(NULL, clickable ? IDC_HAND : IDC_ARROW));
                return TRUE;
            }
            break;
        }
        case WM_KEYDOWN: {
            switch (wp) {
            case VK_ESCAPE:
                if (g_inMenu) { DismissContextMenu(); break; }
                ClosePane(); break;
            case VK_DOWN:
            case VK_RIGHT:
                g_sel = (g_sel + 1) % TOPO_COUNT;
                InvalidateRect(h, NULL, FALSE);
                break;
            case VK_UP:
            case VK_LEFT:
                g_sel = (g_sel + TOPO_COUNT - 1) % TOPO_COUNT;
                InvalidateRect(h, NULL, FALSE);
                break;
            case VK_HOME: g_sel = TOPO_INTERNAL; InvalidateRect(h, NULL, FALSE); break;
            case VK_END:  g_sel = TOPO_EXTERNAL; InvalidateRect(h, NULL, FALSE); break;
            case VK_SPACE:
            case VK_RETURN:
                ApplyTopology(g_sel, L"keyboard");
                break;
            case VK_F5:
                DetectDisplays();
                break;
            case '1': case '2': case '3': case '4':
                ApplyTopology((int)wp - '1', L"key");
                break;
            }
            return 0;
        }
        case WM_CONTEXTMENU: {
            // Keyboard invocation (Menu key / Shift+F10) reports (-1, -1).
            // Mouse right-clicks are handled in WM_RBUTTONUP.
            if ((short)LOWORD(lp) != -1 || (short)HIWORD(lp) != -1) return 0;
            if (g_state != ST_IDLE || !g_open || g_inMenu) return 0;
            Lay L = Layout();
            const int i = (g_sel >= 0 && g_sel < TOPO_COUNT) ? g_sel : 0;
            POINT pt = { L.mx + S(40), L.listY + i * L.rowH + L.rowH / 2 };
            ClientToScreen(h, &pt);
            ShowContextMenu(h, pt.x, pt.y, true);
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (g_state != ST_IDLE) return 0;
            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = h;
            TrackMouseEvent(&tme);
            int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
            const int opt = OptionAt(x, y);
            if (opt != g_hoverOpt) {
                g_hoverOpt = opt;
                InvalidateRect(h, NULL, FALSE);
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            if (g_hoverOpt != -1) {
                g_hoverOpt = -1;
                InvalidateRect(h, NULL, FALSE);
            }
            return 0;
        case WM_LBUTTONDOWN: {
            // A click that dismisses the context menu must not also run a command.
            if (g_state != ST_IDLE || !g_open || g_inMenu) return 0;
            const int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
            const int opt = OptionAt(x, y);
            if (opt >= 0) {
                g_sel = opt;
                if (OptionAvailable(opt)) ApplyTopology(opt, L"click");
                else SetStatus(T(STR_UNAVAILABLE));
                return 0;
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            // Right-clicking anywhere in the pane opens the menu: the pane has
            // no per-row commands of its own, unlike the display list it does
            // not have.
            if (g_state != ST_IDLE || !g_open || g_inMenu) return 0;
            POINT pt; GetCursorPos(&pt);
            ShowContextMenu(h, pt.x, pt.y, false);
            return 0;
        }
        case WM_DPICHANGED: {
            g_scale = HIWORD(wp) / 96.0;
            g_fontScale = -1;
            g_cachedLayW = -1;   // invalidate Layout cache
            RECT* r = (RECT*)lp;
            SetWindowPos(h, NULL, r->left, r->top, r->right - r->left, r->bottom - r->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            InvalidateRect(h, NULL, FALSE);
            return 0;
        }
        }
    } catch (const std::exception& e) {
        LogCaughtException(L"WndProc", e.what());
    } catch (...) {
        LogCaughtException(L"WndProc", nullptr);
    }
    return DefWindowProcW(h, msg, wp, lp);
}

// ---------------------------------------------------------------- Explorer Win+P keyboard handling
// Current Windows builds may keep Win+P outside Explorer's WM_HOTKEY path. A
// single, narrow low-level hook in the main Explorer process handles the
// physical sequence without forwarding or consuming any ordinary input. An
// elevated window or the secure desktop can hide the input from Explorer; the
// native Project flyout is intentionally left available in that case.
static bool ExplorerWinPModifiersDown() {
    const bool win = g_lwinDown || g_rwinDown ||
                     ((GetAsyncKeyState(VK_LWIN) |
                       GetAsyncKeyState(VK_RWIN)) & 0x8000) != 0;
    const bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 ||
                      (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0 ||
                      (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
    const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0 ||
                       (GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0 ||
                       (GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0;
    const bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0 ||
                     (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0 ||
                     (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    return win && !ctrl && !shift && !alt;
}

static void ResetExplorerWinPState() {
    g_winPConsumed = false;
    g_winPSequence = false;
    g_winPCycle = false;
    g_lwinDown = false;
    g_rwinDown = false;
}

static void CALLBACK ExplorerDesktopSwitchProc(HWINEVENTHOOK, DWORD event,
                                                HWND, LONG, LONG, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_DESKTOPSWITCH)
        ResetExplorerWinPState();
}

static void MaskExplorerWinKey() {
    // VK_E8 is intentionally unassigned and prevents a consumed Win+P from
    // being interpreted as a bare Win press that opens the Start menu.
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = 0xE8;
    inputs[0].ki.dwExtraInfo = kMagic;
    inputs[1] = inputs[0];
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    const UINT sent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (sent != ARRAYSIZE(inputs))
        Wh_Log(L"[Hook] Win+P menu-mask SendInput sent %u/%u events, err=%u",
               (unsigned)sent, (unsigned)ARRAYSIZE(inputs), (unsigned)GetLastError());
}

static LRESULT CALLBACK ExplorerWinPKeyboardProc(int code, WPARAM wp, LPARAM lp) {
    if (code != HC_ACTION || !lp)
        return CallNextHookEx(g_hook, code, wp, lp);

    const KBDLLHOOKSTRUCT* key = (const KBDLLHOOKSTRUCT*)lp;
    if (key->flags & LLKHF_INJECTED)
        return CallNextHookEx(g_hook, code, wp, lp);

    const bool up = (key->flags & LLKHF_UP) != 0;
    const bool down = !up;

    if (key->vkCode == VK_LWIN)
        g_lwinDown = down;
    else if (key->vkCode == VK_RWIN)
        g_rwinDown = down;

    const bool asyncWin = ((GetAsyncKeyState(VK_LWIN) |
                            GetAsyncKeyState(VK_RWIN)) & 0x8000) != 0;

    // A Win-up or P-up can be missed during a secure-desktop transition. If a
    // later physical P arrives with no async Win state, discard stale state
    // before evaluating it; otherwise a plain P could open the panel.
    if (down && key->vkCode == 'P' && !asyncWin &&
        (g_lwinDown || g_rwinDown || g_winPConsumed || g_winPSequence)) {
        ResetExplorerWinPState();
        return CallNextHookEx(g_hook, code, wp, lp);
    }

    if (up && key->vkCode == 'P' && g_winPConsumed) {
        g_winPConsumed = false;
        return 1;
    }
    // Suppress P auto-repeat while the physical Win+P sequence is active.
    if (down && key->vkCode == 'P' && g_winPConsumed)
        return 1;

    if (up && (key->vkCode == VK_LWIN || key->vkCode == VK_RWIN) &&
        g_winPSequence) {
        const bool commit = g_winPCycle;
        g_winPCycle = false;
        g_winPSequence = false;
        HWND panel = FindWindowW(kClass, NULL);
        if (commit && panel)
            PostMessageW(panel, WM_APP_COMMIT, 0, 0);
        // Repeat the mask immediately before Win-up to cover the shell's
        // key-up timing while still passing the physical Win-up through.
        MaskExplorerWinKey();
        return CallNextHookEx(g_hook, code, wp, lp);
    }

    if (down && key->vkCode == 'P' && !g_winPConsumed &&
        g_useWinP && ExplorerWinPModifiersDown()) {
        HWND panel = FindWindowW(kClass, NULL);
        // If Explorer has no panel, leave native Win+P untouched.
        if (!panel)
            return CallNextHookEx(g_hook, code, wp, lp);

        g_winPConsumed = true;
        g_winPSequence = true;
        const bool visible = IsWindowVisible(panel) != FALSE;
        g_winPCycle = visible;
        if (g_debug)
            Wh_Log(L"[Hook] Win+P intercepted panel=%p visible=%d cycle=%d",
                   panel, visible ? 1 : 0, g_winPCycle ? 1 : 0);
        PostMessageW(panel, g_winPCycle ? WM_APP_CYCLE : WM_APP_TOGGLE, 0, 0);
        MaskExplorerWinKey();
        return 1;
    }

    return CallNextHookEx(g_hook, code, wp, lp);
}

// Low-level mouse hook, active only while the panel is visible. Per the
// LowLevelMouseProc documentation it must return quickly (Windows 10 1709+
// silently removes hooks slower than 1 s), so it only forwards button presses
// to the UI thread and never consumes input: the click still reaches the
// window under the cursor.
static LRESULT CALLBACK MouseProc(int code, WPARAM wp, LPARAM lp) {
    RunGuarded(L"MouseProc", [&] {
        if (code == HC_ACTION) {
            switch (wp) {
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_XBUTTONDOWN: {
                const MSLLHOOKSTRUCT* m = (const MSLLHOOKSTRUCT*)lp;
                HWND panel = g_wnd;
                if (panel && m)
                    PostMessageW(panel, WM_APP_POINTER_DOWN,
                                 (WPARAM)(LONG_PTR)m->pt.x, (LPARAM)(LONG_PTR)m->pt.y);
                break;
            }
            }
        }
    });
    return CallNextHookEx(g_mouseHook, code, wp, lp);
}

static void SignalUiReady(bool started) {
    InterlockedExchange(&g_uiStarted, started ? 1 : 0);
    if (g_uiReady.get()) SetEvent(g_uiReady.get());
}

static void SignalKeyboardReady(bool started) {
    InterlockedExchange(&g_keyboardStarted, started ? 1 : 0);
    if (g_keyboardReady.get()) SetEvent(g_keyboardReady.get());
}

// Keeps the panel honest while it is open: a monitor plugged in, a display
// turned off, or a topology change made through the native shortcut all change
// what the panel must show. WM_DISPLAYCHANGE already covers most of it; this
// timer loop is the safety net for the cases where no message reaches a
// top-level window of this thread.
static DWORD WINAPI StateThread(LPVOID) {
    while (true) {
        const DWORD wait = WaitForSingleObject(g_stopEvent.get(), 2000);
        if (wait != WAIT_TIMEOUT) break;
        if (!g_open) continue;
        const unsigned before = g_stateStamp;
        RunGuarded(L"refreshing the display state", [] { RefreshState(); });
        if (g_stateStamp != before) InvalidatePanel();
    }
    return 0;
}

// This thread owns the Explorer Win+P hook and the on-demand low-level mouse
// hook. It always keeps a message queue so hook callbacks stay off the UI
// thread and the desktop-switch event can reset keyboard state on this thread.
static DWORD WINAPI KeyboardHookThread(LPVOID) {
    MSG queued = {};
    PeekMessageW(&queued, NULL, WM_USER, WM_USER, PM_NOREMOVE); // create the queue first

    ScopedWinEventHook desktopHook(SetWinEventHook(
        EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH,
        NULL, ExplorerDesktopSwitchProc, 0, 0, WINEVENT_OUTOFCONTEXT));
    if (!desktopHook.get())
        Wh_Log(L"[Hook] SetWinEventHook(EVENT_SYSTEM_DESKTOPSWITCH) failed, err=%u",
               (unsigned)GetLastError());

    ScopedHook kbHook;
    if (g_useWinP) {
        HHOOK kh = SetWindowsHookExW(
            WH_KEYBOARD_LL, ExplorerWinPKeyboardProc, (HINSTANCE)&__ImageBase, 0);
        if (!kh) {
            Wh_Log(L"[Hook] SetWindowsHookExW(WH_KEYBOARD_LL) failed, err=%u",
                   (unsigned)GetLastError());
            SignalKeyboardReady(false);
            return 0;
        }
        kbHook.reset(kh);
        g_hook = kbHook.get();
        g_keyboardHookWanted = true;
        Wh_Log(L"[Hook] Explorer Win+P keyboard hook installed: %p", g_hook);
    }
    SignalKeyboardReady(true);

    ScopedHook mouseHook;

    try {
        MSG msg;
        int result = 0;
        while ((result = GetMessageW(&msg, NULL, 0, 0)) > 0) {
            // No window dispatch is needed on this thread. Retrieving messages
            // is enough to service the low-level callbacks and control hooks.
            if (msg.hwnd != NULL) continue;

            if (msg.message == WM_HOOKCTL_KEYBOARD_ON) {
                if (!kbHook.get()) {
                    HHOOK kh = SetWindowsHookExW(
                        WH_KEYBOARD_LL, ExplorerWinPKeyboardProc,
                        (HINSTANCE)&__ImageBase, 0);
                    if (kh) {
                        kbHook.reset(kh);
                        g_hook = kbHook.get();
                        Wh_Log(L"[Hook] Explorer Win+P keyboard hook installed: %p", g_hook);
                    } else {
                        Wh_Log(L"[Hook] SetWindowsHookExW(WH_KEYBOARD_LL) failed, err=%u",
                               (unsigned)GetLastError());
                    }
                }
            } else if (msg.message == WM_HOOKCTL_KEYBOARD_OFF) {
                g_hook = NULL;
                kbHook.reset();
            } else if (msg.message == WM_HOOKCTL_MOUSE_ON) {
                if (!mouseHook.get()) {
                    HHOOK mh = SetWindowsHookExW(
                        WH_MOUSE_LL, MouseProc, (HINSTANCE)&__ImageBase, 0);
                    mouseHook.reset(mh);
                    g_mouseHook = mouseHook.get();
                    if (g_mouseHook)
                        Wh_Log(L"[Hook] on-demand mouse hook installed: %p", g_mouseHook);
                    else
                        Wh_Log(L"[Hook] SetWindowsHookExW(WH_MOUSE_LL) failed, err=%u",
                               (unsigned)GetLastError());
                }
            } else if (msg.message == WM_HOOKCTL_MOUSE_OFF) {
                g_mouseHook = NULL;
                mouseHook.reset();
            }
        }
        if (result == -1)
            Wh_Log(L"[Hook] GetMessageW failed, err=%u", (unsigned)GetLastError());
    } catch (const std::exception& e) {
        LogCaughtException(L"KeyboardHookThread", e.what());
    } catch (...) {
        LogCaughtException(L"KeyboardHookThread", nullptr);
    }

    g_mouseHook = NULL;
    g_hook = NULL;
    return 0;
}

class ScopedWindowClass {
    HINSTANCE instance_;
    const wchar_t* name_;
    bool registered_ = false;
public:
    ScopedWindowClass(HINSTANCE instance, const wchar_t* name)
        : instance_(instance), name_(name) {}
    ~ScopedWindowClass() {
        if (registered_) UnregisterClassW(name_, instance_);
    }
    bool Register(const WNDCLASSW& wc) {
        registered_ = RegisterClassW(&wc) != 0;
        return registered_;
    }
    ScopedWindowClass(const ScopedWindowClass&) = delete;
    ScopedWindowClass& operator=(const ScopedWindowClass&) = delete;
};

// ---------------------------------------------------------------- UI thread
static DWORD WINAPI UiThread(LPVOID) {
    // A queue exists before initialization can fail. This makes WM_QUIT reliable
    // during an early unload and lets Wh_ModInit wait for the panel startup state.
    MSG queued = {};
    PeekMessageW(&queued, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    HINSTANCE hi = (HINSTANCE)&__ImageBase;
    ScopedWindowClass panelClass(hi, kClass);
    bool readySignaled = false;

    try {
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        ScopedComApartment com(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)));

        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32)
            g_createInBand = (CreateWindowInBand_t)(void*)GetProcAddress(user32, "CreateWindowInBand");
        if (!g_createInBand)
            Wh_Log(L"[ZBand] CreateWindowInBand not available, using ordinary topmost windows");

        WNDCLASSW wc = {};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hi;
        wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
        wc.lpszClassName = kClass;
        if (!panelClass.Register(wc)) {
            Wh_Log(L"[UI] RegisterClassW failed, err=%u", (unsigned)GetLastError());
            SignalUiReady(false);
            readySignaled = true;
            return 0;
        }

        g_wnd = CreateOverlayWindow(kClass, 350, 600, &g_panelBand);
        if (!g_wnd) {
            Wh_Log(L"[UI] could not create panel window, err=%u", (unsigned)GetLastError());
            SignalUiReady(false);
            readySignaled = true;
            return 0;
        }
        SetLayeredWindowAttributes(g_wnd, KEY, 255, LWA_COLORKEY);

        // Wh_ModInit waits for this signal before it starts the remaining
        // infrastructure or returns. The window and message queue are ready here.
        SignalUiReady(true);
        readySignaled = true;

        ScopedTimer enforceTimer(g_wnd, 3, 2000);

        // Out-of-context WinEvent hook: delivered on this thread, which has a
        // message loop as SetWinEventHook requires.
        g_foregroundHook.reset(SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                               NULL, ForegroundEventProc, 0, 0,
                                               WINEVENT_OUTOFCONTEXT));
        if (!g_foregroundHook.get())
            Wh_Log(L"[Dismiss] SetWinEventHook(EVENT_SYSTEM_FOREGROUND) failed, err=%u",
                   (unsigned)GetLastError());

        // Prime the model so the very first open has real data to show.
        RunGuarded(L"reading the display state", [] { RefreshState(); });

        MSG msg;
        int result = 0;
        while ((result = GetMessageW(&msg, NULL, 0, 0)) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (result == -1)
            Wh_Log(L"[UI] GetMessageW failed, err=%u", (unsigned)GetLastError());
    } catch (const std::exception& e) {
        LogCaughtException(L"UiThread", e.what());
    } catch (...) {
        LogCaughtException(L"UiThread", nullptr);
    }

    if (!readySignaled) SignalUiReady(false);

    g_foregroundHook.reset();
    g_mouseHookWanted = false;
    g_keyboardHookWanted = false;

    if (g_menuWnd) {
        HWND menu = g_menuWnd;
        g_menuWnd = NULL;
        DestroyWindow(menu);
    }
    g_inMenu = false;
    if (g_wnd) {
        HWND panel = g_wnd;
        g_wnd = NULL;
        DestroyWindow(panel);
    }
    DeleteFonts();
    FreeGlyphs();
    UnregisterMenuClass();
    return 0;
}

// The shell process owns GetShellWindow. If it is in the middle of restarting,
// exclude the known folder-window command lines while waiting for its taskbar.
static bool IsMainExplorerProcess() {
    HWND shellWindow = GetShellWindow();
    if (shellWindow) {
        DWORD shellProcessId = 0;
        GetWindowThreadProcessId(shellWindow, &shellProcessId);
        if (shellProcessId == GetCurrentProcessId()) return true;
    }

    std::wstring commandLine = GetCommandLineW();
    std::transform(commandLine.begin(), commandLine.end(), commandLine.begin(),
                   [](wchar_t ch) { return (wchar_t)towlower(ch); });
    return commandLine.find(L" /factory") == std::wstring::npos &&
           commandLine.find(L" /separate") == std::wstring::npos &&
           commandLine.find(L" -embedding") == std::wstring::npos;
}

static bool WaitForStartup(HANDLE readyEvent, const wchar_t* component) {
    const DWORD wait = WaitForSingleObject(readyEvent, 10000);
    if (wait == WAIT_OBJECT_0) return true;
    Wh_Log(L"[Init] timed out waiting for %s (result=%u)", component, (unsigned)wait);
    return false;
}

// Retry WM_QUIT while a thread is alive. A normal startup creates the message
// queue before signaling readiness; retrying also covers an unload racing an
// unsuccessful startup instead of waiting forever after one failed post.
static void QuitAndJoinThread(WinHandle& thread, DWORD& threadId, const wchar_t* name) {
    HANDLE handle = thread.get();
    if (!handle) return;

    while (WaitForSingleObject(handle, 0) == WAIT_TIMEOUT) {
        if (threadId && PostThreadMessageW(threadId, WM_QUIT, 0, 0)) break;
        Sleep(10);
    }

    if (WaitForSingleObject(handle, 0) == WAIT_TIMEOUT) {
        Wh_Log(L"[Uninit] waiting for %s", name);
        WaitForSingleObject(handle, INFINITE);
    }
    thread.reset();
    threadId = 0;
}

static void StopInfrastructure() {
    // Stop the hook thread first so it cannot post work to a panel that is
    // going away, then the state watchdog, then the panel itself.
    if (g_stopEvent.get()) SetEvent(g_stopEvent.get());

    QuitAndJoinThread(g_keyboardThread, g_keyboardTid, L"keyboard-hook thread");

    if (g_stateThread.get()) {
        WaitForSingleObject(g_stateThread.get(), INFINITE);
        g_stateThread.reset();
    }

    QuitAndJoinThread(g_uiThread, g_uiTid, L"UI thread");

    g_uiReady.reset();
    g_keyboardReady.reset();
    g_stopEvent.reset();
    InterlockedExchange(&g_uiStarted, 0);
    InterlockedExchange(&g_keyboardStarted, 0);
}

void Wh_ModSettingsChanged() {
    RunGuarded(L"applying settings", [] { LoadSettings(); RefreshState(); });
    SetKeyboardHook(g_useWinP);
    InvalidatePanel();
}

BOOL Wh_ModInit() {
    if (!IsMainExplorerProcess()) {
        Wh_Log(L"[Init] not the main Explorer shell process; skipping");
        return FALSE;
    }

    try {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        g_freq = f.QuadPart ? f.QuadPart : 1;

        LoadSettings();

        g_uiReady.reset(CreateEventW(NULL, TRUE, FALSE, NULL));
        g_keyboardReady.reset(CreateEventW(NULL, TRUE, FALSE, NULL));
        g_stopEvent.reset(CreateEventW(NULL, TRUE, FALSE, NULL));
        if (!g_uiReady.get() || !g_keyboardReady.get() || !g_stopEvent.get()) {
            Wh_Log(L"[Init] CreateEventW failed, err=%u", (unsigned)GetLastError());
            StopInfrastructure();
            return FALSE;
        }

        HANDLE rawUi = CreateThread(NULL, 0, UiThread, NULL, 0, &g_uiTid);
        g_uiThread.reset(rawUi);
        if (!g_uiThread.get() || !WaitForStartup(g_uiReady.get(), L"the UI thread") ||
            InterlockedCompareExchange(&g_uiStarted, 0, 0) == 0) {
            Wh_Log(L"[Init] UI thread did not create the panel");
            StopInfrastructure();
            return FALSE;
        }

        HANDLE rawState = CreateThread(NULL, 0, StateThread, NULL, 0, NULL);
        g_stateThread.reset(rawState);
        if (!g_stateThread.get())
            Wh_Log(L"[Init] could not start the state watchdog, err=%u",
                   (unsigned)GetLastError());

        HANDLE rawKeyboard = CreateThread(NULL, 0, KeyboardHookThread, NULL, 0, &g_keyboardTid);
        g_keyboardThread.reset(rawKeyboard);
        if (!g_keyboardThread.get() ||
            !WaitForStartup(g_keyboardReady.get(), L"the keyboard-hook thread") ||
            InterlockedCompareExchange(&g_keyboardStarted, 0, 0) == 0) {
            Wh_Log(L"[Init] keyboard-hook thread was not started");
            StopInfrastructure();
            return FALSE;
        }
        SetKeyboardHook(g_useWinP);

        Wh_Log(L"[Init] initialized in the main Explorer shell process");
        return TRUE;
    } catch (const std::exception& e) {
        LogCaughtException(L"Wh_ModInit", e.what());
        StopInfrastructure();
        return FALSE;
    } catch (...) {
        LogCaughtException(L"Wh_ModInit", nullptr);
        StopInfrastructure();
        return FALSE;
    }
}

void Wh_ModUninit() {
    RunGuarded(L"Wh_ModUninit", [] {
        Wh_Log(L"[Uninit] shutting down, exceptions=%u", g_exceptionCount.load());
        StopInfrastructure();
    });
}
