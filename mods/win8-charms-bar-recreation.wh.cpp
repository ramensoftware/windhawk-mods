// ==WindhawkMod==
// @id              win8-charms-bar-recreation
// @name            Windows 8/8.1 Charms Bar Recreation
// @description     This mod recreates the Windows 8/8.1 Charms Bar on Windows 10 and Windows 11
// @version         1.0.0
// @author 	        babamohammed
// @github          https://github.com/babamohammed2022
// @license         GPL-3.0
// @include         explorer.exe
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lgdi32 -luser32 -lshell32 -lole32 -loleaut32 -lwbemuuid -lshlwapi -ldwmapi -lshcore -lgdiplus -lmsimg32 -lwlanapi -liphlpapi -lwininet -lwindowsapp -luuid -ladvapi32 -lpowrprof -lcrypt32 -lwindowscodecs
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Windows 8/8.1 Charms Bar Recreation

This mod recreates the classic Windows 8/8.1 Charms Bar on Windows 10 and 11.

## Screenshots 
### Windows 8/8.1-styled Charms Bar

![win8charmsnormal](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win8charmsnormal.PNG)

### Windows 8 beta-styled Charms Bar 

![win8betacharms](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win8betacharms.PNG)

## About the mod

It recreates the five charms (Search, Share, Start, Devices, Settings) with Win+C and edge activation, keyboard and touch input, multi-monitor placement, and a clock/status flyout. Suggestions are welcome. Tested on Windows 10 21H2, Windows 11 24H2 and Windows 11 25H2.
Note: This is a best effort modification. Layout, colors, icons and animations are heavily inspired by Windows 8/8.1. Share, Devices and the Second screen panel are recreations on Windows 10/11, not the original code.

## Keyboard shortcuts

| Shortcut | Action |
| --- | --- |
| Win+C | Opens the Charms Bar |
| Right-edge mouse gesture | Opens the Charms Bar |
| Esc | Closes the rail or an open pane |
| Click outside | Closes the rail or an open pane |

While a charm pane is open:

| Shortcut | Action |
| --- | --- |
| Win+C | Closes the current pane first |
| Esc | Closes the current pane |
| Click outside | Closes the current pane |

## Settings

- **Charms Bar enabled**: turns the Charms Bar on or off. Share is always included.
- **In-mod language**: English, Italian, German, French, Spanish, Japanese, Portuguese, Russian, Chinese
  (Simplified) or Korean. The choice applies to the rail, the charm panes and the Project panel alike.
- **Activation mode**: full right edge, top-right corner, Win+C, or combinations.
- **Hide after choosing a charm**: closes the Charms Bar after a charm is activated. Turn off to leave it open.
- **Close on outside click or focus change**: light-dismiss behavior.
- **Animate show and hide**: enables pane motion. Windows' own *Settings > Accessibility > Visual effects >
  Animation effects* switch is always honoured: when it is off, the rail and the panes appear and disappear
  instantly even if this setting is on.
- **Start logo style**: Windows 8/8.1 logo (system accent) or the Windows 8 Build 8102 beta logo.
- **Charms Bar icons**: Windows 8/8.1 icons, or the Windows 8 Build 8102 beta set for Search, Share, Devices and Settings. The Start logo is set separately.
- **Enable rail transparency**: optional subtle rail transparency.
- **Show clock and system status flyout**: shows or hides the clock and status flyout.
- **Settings pane color**: system accent or dark background for the Settings, Devices and Share panes.
- **Settings charm**: recreated Windows 8-style pane or native Win+I.
- **Search charm target**: automatic or Windows search. Automatic opens the companion
  *Windows 8/8.1 Search Charm Recreation* panel when that mod is installed, otherwise it sends the
  Windows shortcut.
- **Search hand-off shortcut**: automatic, Win+S, or Win+Q. Used only when the target is Windows search.
- **Share fallback behavior**: automatic, Windows Share UI, Win+H, or mail/clipboard.
- **Display Switch panel title, color and custom color**: see the project flyout settings at the end of the settings page.

## Performance

Live system data (volume, brightness, network, battery) is read only while a pane or flyout is open. Rendering falls back to native GDI if GDI+ startup fails.

## Requirements

- Windows 10 version 1809 or later (64-bit), with DWM enabled (default on Windows)

## Notes

- The Charms Bar appears on the main monitor, where all five charms are displayed.

## Credits

- Icepenguins101 - Technical Inspiration 
- AdministratoX - Testing on Windows 11 25H2 and providing feedback
- Kiyaku - Providing feedback
- CharmsBarReloaded project - Wi-Fi PNGs and battery silhouettes (GPL-3.0, license-compatible)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- charmsEnabled: true
  $name: Charms Bar enabled
  $description: "This setting turns the whole Charms Bar on or off. When it is off, the rail and its five charms are not shown. The Share charm is always included and this mod does not hide it."
- uiLanguage: system
  $name: In-mod language
  $description: "This setting chooses the language of the rail captions, the clock and status labels, the charm panes, the Share fallback text and the Project panel. Date and time still follow the Windows regional settings."
  $options:
    - system: Follow Windows display language
    - it: Italiano
    - en: English
    - de: Deutsch
    - ja: 日本語
    - fr: Français
    - es: Español
    - pt: Português
    - ru: Русский
    - zh: 中文(简体)
    - ko: 한국어
- activationMode: edgeAndWinC
  $name: Activation mode
  $description: "This setting chooses how the Charms Bar opens: from the full right edge, from the top-right corner, with Win+C, or with a combination of these."
  $options:
    - edgeAndWinC: Full right edge + Win+C
    - edgeOnly: Full right edge only
    - topRightAndWinC: Top-right corner + Win+C
    - topRightOnly: Top-right corner only
    - winCOnly: Win+C only
- hideAfterClick: true
  $name: Hide after choosing a charm
  $description: "This setting closes the Charms Bar after you activate a charm. Turn it off to leave the rail open."
- closeOnClickOutside: true
  $name: Close on outside click or focus change
  $description: "This setting closes the Charms Bar when you click outside it or when another window takes the focus."
- useAnimations: true
  $name: Animate show and hide
  $description: "This setting animates the Charms Bar and its panes when they open and close. Turn it off for instant show and hide. The Windows animation setting (Settings > Accessibility > Visual effects > Animation effects) is always honoured: when Windows animations are off, the Charms Bar always appears and disappears instantly."
- startLogoStyle: windows8
  $name: Start logo style
  $description: "This setting chooses the Start logo in the rail: the Windows 8/8.1 logo, drawn with the system accent color, or the Windows 8 Build 8102 beta logo."
  $options:
    - windows8: Windows 8/8.1 (system accent)
    - build8102: Windows 8 Build 8102 beta (logo)
- charmIconSet: windows81
  $name: Charms Bar icons
  $description: "This setting chooses the icons for Search, Share, Devices and Settings: the regular Windows 8/8.1 icons, or the Windows 8 Build 8102 beta icons. The Start logo is set by Start logo style."
  $options:
    - windows81: Windows 8/8.1 (regular)
    - build8102: Windows 8 Build 8102 beta
- enableTransparency: false
  $name: Enable rail transparency
  $description: "This setting adds a subtle fixed transparency to the rail. It is off by default, and no other opacity setting is needed."
- showClock: true
  $name: Show clock and system status flyout
  $description: "This setting shows the clock and system status flyout next to the Charms Bar when it opens."
- paneColorMode: accent
  $name: Settings pane color
  $description: "This setting chooses the background of the Settings, Devices and Share panes: the system accent color, or a dark background. It does not change the hover colors of the rail."
  $options:
    - accent: System accent
    - dark: Dark
- settingsCharmMode: pane
  $name: Settings charm
  $description: "This setting chooses what the Settings charm opens: the recreated Windows 8-style Settings pane, or the native Windows Win+I experience."
  $options:
    - pane: Windows 8-style pane
    - native: Native Windows Win+I
- searchHotkey: auto
  $name: Search hand-off shortcut
  $description: "This setting chooses the shortcut the Search charm sends to Windows: automatic for the current app, Win+S, or Win+Q. It is used only when the Windows search UI is the target; the Search Charm Recreation mod is opened through its own window instead."
  $options:
    - auto: Automatic for Windows/app context
    - winS: Win+S
    - winQ: Win+Q
- searchTarget: auto
  $name: Search charm target
  $description: "Automatic opens the Windows 8/8.1 Search Charm Recreation panel when that mod is installed, and otherwise sends the Windows shortcut. The companion panel is preferred because it is a real Windows 8 search surface and it opens without toggling, so a second click never closes it. When that mod is installed its keyboard hook owns Win+S and Win+Q, so choosing Windows search may not reach the native search UI; the search target log line reports what was found."
  $options:
    - auto: Automatic (companion mod if installed, otherwise Windows)
    - windows: Windows search (Win+S / Win+Q)
- shareBehavior: auto
  $name: Share fallback behavior
  $description: "This setting chooses what the Share charm does when the recreated panel cannot open, and on Windows 8.x. The Windows 10/11 Share row starts Snipping Tool without capture arguments; this fallback may share text, not a screenshot."
  $options:
    - auto: Automatic (recommended)
    - winrt: Windows Share UI (title/text fallback)
    - winH: Win+H (Windows 8.x only; later Windows uses voice typing)
    - email: Mail/clipboard text fallback
- dsPaneTitle: secondScreen
  $name: Display Switch panel title
  $description: "This setting chooses the title of the project flyout, which is the display switch panel that opens from Devices > Second screen. Windows 8 called it \"Second screen\" and Windows 8.1 renamed it \"Project\". The default, \"Second screen\", follows the Windows 8 reference screenshot."
  $options:
    - secondScreen: Second screen (Windows 8)
    - project: Project (Windows 8.1)
- dsColorMode: accent
  $name: Display Switch panel color
  $description: "This setting chooses the background of the project flyout (the display switch panel): the system accent color, as in Windows 8, charcoal, or a custom color."
  $options:
    - accent: Accent color
    - charcoal: Charcoal
    - custom: Custom color
- dsCustomColor: "#1D1D1D"
  $name: Custom color (hex)
  $description: "This setting sets the background of the project flyout when Display Switch panel color is Custom. Enter a six-digit hex color, with or without #, for example 087900 or #1D1D1D."
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0601
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#if !defined(WINVER) || WINVER < 0x0601
#undef WINVER
#define WINVER 0x0601
#endif
#if !defined(NTDDI_VERSION) || NTDDI_VERSION < 0x06010000
#undef NTDDI_VERSION
#define NTDDI_VERSION 0x06010000
#endif
// winsock2.h has to be included, not skipped: iphlpapi.h builds IP_ADAPTER_ADDRESSES and the GAA_FLAG_*
// constants on the Winsock 2 definitions (ws2def.h/ws2ipdef.h), and without them GetAdaptersAddresses does
// not even have a type to take. With -DWIN32_LEAN_AND_MEAN in @compilerOptions, the preincluded windows.h
// no longer pulls in winsock.h, so winsock2.h can be included without the old include-order warning.
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <powrprof.h>
#include <windhawk_api.h>
#include <objbase.h>
#include <shellscalingapi.h>
#include <wincrypt.h>
#include <initguid.h>
#include <wincodec.h>
#include <math.h>
#include <exception>
#include <winstring.h>
#include <windows.foundation.h>
#include <inspectable.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <wlanapi.h>
#include <wininet.h>
#include <roapi.h>
#include <windows.applicationmodel.datatransfer.h>
#include <wrl.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <wbemidl.h>
#include <new>
#include <cstdlib>
#include <wrl/wrappers/corewrappers.h>
#include <atomic>
#include <algorithm>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <cwchar>
#include <cstring>
#include <cmath>

extern "C" IMAGE_DOS_HEADER __ImageBase;

// Defined below, after the Charms Bar UI messages exist: shows the main Charms Bar again.
void ReturnToCharmsBarFromDisplaySwitch();

// ---------------------------------------------------------------- Display Switch (Second screen / Project)
// Built-in copy of "Windows 8/8.1 Display Switch Recreation" by babamohammed (GPL-3.0).
namespace DisplaySwitchMod {
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
// The four documented projection options of the Windows 8/8.1 pane, in the order the flyout shows them. Each
// one maps to exactly one SDC_TOPOLOGY_xxx flag.
enum Topo {
    TOPO_INTERNAL = 0,   // PC screen only      (DISPLAYCONFIG_TOPOLOGY_INTERNAL)
    TOPO_CLONE = 1,      // Duplicate           (DISPLAYCONFIG_TOPOLOGY_CLONE)
    TOPO_EXTEND = 2,     // Extend              (DISPLAYCONFIG_TOPOLOGY_EXTEND)
    TOPO_EXTERNAL = 3,   // Second screen only  (DISPLAYCONFIG_TOPOLOGY_EXTERNAL)
    TOPO_COUNT = 4, TOPO_UNKNOWN = -1
};

// One connected display. Everything here comes from a documented API: the name from
// DISPLAYCONFIG_TARGET_DEVICE_NAME, the GDI device name from DISPLAYCONFIG_SOURCE_DEVICE_NAME, the mode from
// EnumDisplaySettingsEx, the rectangle and primary flag from GetMonitorInfo, the scale from GetDpiForMonitor.
// A row can be connected but inactive (for example the built-in panel while "Second screen only" is active).
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

// Layout struct: needs to be fully defined before the g_cachedLay global. Every value is a Windows 8.1
// measurement from the capture, in physical pixels at 96 DPI, scaled by S() for the monitor the pane is shown
// on.
struct Lay {
    int mx;              // left edge of the tagline and the subtitle
    int titleX;          // left edge of the title, moved right to make room for the back button
    int titleInk;        // ink top of the title
    int subInk;          // ink top of the tagline
    int listY;           // top of the first option row
    int rowH;            // row height, and the height of the selection band
    int glyphX, glyphY;  // icon position inside the row
    int textX;           // left edge of the option label
    int rowInk;          // ink top of the option label, relative to the row
    int backX, backY;    // top left of the back button ring
    int backD;           // diameter of the back button ring
    int bandX;           // left inset of the selection band
};

static const UINT WM_APP_TOGGLE       = WM_APP + 1;
// Posted by the low-level mouse hook for every button press while the panel is visible. wParam/lParam carry
// the screen point (per-monitor-aware coordinates).
static const UINT WM_APP_POINTER_DOWN = WM_APP + 2;
// Thread messages for the input-hook thread (msg.hwnd == NULL).
static const UINT WM_HOOKCTL_MOUSE_ON     = WM_APP + 20;
static const UINT WM_HOOKCTL_MOUSE_OFF    = WM_APP + 21;
// Own class names, so this copy and the separate mod can be loaded together without a class clash.
static const wchar_t* kClass = L"CharmsBarDisplaySwitch";
static const int k81FlyoutWidth = 346;
static const int k81GlyphWidth = 89, k81GlyphHeight = 45;
static const wchar_t* kMenuClass = L"CharmsBarDisplaySwitchMenu";
static const COLORREF KEY = RGB(255, 0, 255);
static const UINT_PTR kStatusMs = 6000;
static std::vector<MonRow> g_monitors;
static SRWLOCK g_lock = SRWLOCK_INIT;
static int g_topology = TOPO_UNKNOWN;
static int g_connected = 0;
static int g_active = 0;
static int g_sel = 0;
static int g_hoverOpt = -1;
static bool g_backHover = false;   // mouse is over the back arrow
static std::wstring g_status;
static DWORD g_statusUntil = 0;
// Bumped by RefreshState whenever the reported state changes; the state watchdog uses it to avoid repainting
// a panel that has nothing new to show.
static volatile unsigned g_stateStamp = 0;
static HWND g_wnd = NULL;
static WinHandle g_uiThread, g_keyboardThread, g_stateThread;
static WinHandle g_stopEvent;
static WinHandle g_uiReady, g_keyboardReady;
static DWORD g_uiTid = 0, g_keyboardTid = 0;
static HHOOK g_mouseHook = NULL;           // owned by the input-hook thread
static bool g_mouseHookWanted = false;     // UI-thread view of the requested state
static HWINEVENTHOOK g_foregroundHook = nullptr;    // owned by the UI thread; unhooked explicitly before exit
static DWORD g_openTick = 0;               // GetTickCount() of the last OpenPane
static volatile LONG g_uiStarted = 0, g_keyboardStarted = 0;
static DWORD g_lastApply = 0;
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
    MENU_END_NONE = 0, MENU_END_INSIDE,    // command chosen, Esc, or focus moved to the panel itself
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
               c->idx, w, cls, (unsigned)pid, (unsigned)GetWindowLongW(w, GWL_EXSTYLE),
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

// The system accent color: what Windows 8.1 painted its flyout with, and what DwmGetColorizationColor reports
// on Windows 10 and 11. Falls back to the Metro blue (#0074CB) when DWM does not provide a color.
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
    return RGB(GetRValue(pane) * 78 / 100, GetGValue(pane) * 78 / 100,
               GetBValue(pane) * 83 / 100);
}

// The selection band uses a lighter shade of the panel color (such as #008CE0 on the #0074CB Metro blue
// panel, or a +18% lighten on any other color). The selection band is a lighter version of the panel color.
// Matched to the Windows 8 capture: on the teal panel #004D61 the band is about #008287, so the green channel
// moves 30 % toward 255 and the blue channel 24 %.
static COLORREF SelectFillColor(COLORREF pane) {
    const int r = GetRValue(pane), g = GetGValue(pane), b = GetBValue(pane);
    return RGB(r, g + (255 - g) * 30 / 100, b + (255 - b) * 24 / 100);
}

// The English system message for a Win32 error code (falling back to the neutral language only when English
// message resources are absent), so all Windhawk log entries remain in English.
static std::wstring WinErrText(DWORD err) {
    ScopedLocalFree buf;
    DWORD len = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPWSTR)buf.put(), 0, NULL);
    if (!len || !buf.asWstr()) {
        len = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, 0, (LPWSTR)buf.put(), 0, NULL);
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

// Only the settings a Windows 8 user expects are exposed: the panel title, and the panel color. Everything
// else keeps the Windows 8.1 behaviour of the reference capture.
static void LoadSettings() {
    {
        ScopedWhString cm(L"dsColorMode");
        g_colorMode = (cm && !wcscmp(cm, L"charcoal")) ? 1 : (cm && !wcscmp(cm, L"custom")) ? 2 : 0;
    }
    {
        ScopedWhString cc(L"dsCustomColor");
        g_custom = ParseHex(cc, RGB(29, 29, 29));
    }
    {
        // Windows 8 named the project flyout "Second screen"; Windows 8.1 renamed it "Project". The default
        // is "secondScreen" because the reference capture is Windows 8.
        ScopedWhString pt(L"dsPaneTitle");
        g_titleMode = (pt && !wcscmp(pt, L"project")) ? 0 : 1;
    }
    g_moreSettings = 0;   // "More settings" opens the display page
    g_openMs = 300;
    g_closeMs = 250;
    {
        // The rail follows the mod's "In-mod language" setting; this panel used to follow the Windows
        // display language only, so a fixed language setting left the two halves of the mod in different
        // languages. Honour the same setting here and fall back to the display language on "system".
        ScopedWhString uiLanguage(Wh_GetStringSetting(L"uiLanguage"));
        int chosen = -1;
        if (uiLanguage.get()) {
            if (!wcscmp(uiLanguage.get(), L"it")) chosen = 0;
            else if (!wcscmp(uiLanguage.get(), L"en")) chosen = 1;
            else if (!wcscmp(uiLanguage.get(), L"es")) chosen = 2;
            else if (!wcscmp(uiLanguage.get(), L"fr")) chosen = 3;
            else if (!wcscmp(uiLanguage.get(), L"pt")) chosen = 4;
            else if (!wcscmp(uiLanguage.get(), L"de")) chosen = 5;
            else if (!wcscmp(uiLanguage.get(), L"ru")) chosen = 6;
            else if (!wcscmp(uiLanguage.get(), L"zh")) chosen = 7;
            else if (!wcscmp(uiLanguage.get(), L"ja")) chosen = 8;
            else if (!wcscmp(uiLanguage.get(), L"ko")) chosen = 9;
        }
        if (chosen >= 0) {
            g_lang = chosen;
        } else {
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
    g_fullHeight = true;
    g_validateFirst = true;
    g_persistPaths = false;
    g_disableUnavailable = false;
    g_allowSetPrimary = true;
    // The panel stays open after a choice. It closes with Esc, the back arrow, or a click outside.
    g_closeAfterApply = false;
    g_closeOnClickOutside = true;
    g_aboveTaskbar = true;
    g_useZBand = true;
    g_applyCooldownMs = 700;
    g_hotkeyCooldownMs = 400;
    g_monitorChoice = 0;
    g_debug = false;
    Wh_Log(L"[Settings] lang=%d colorMode=%d titleMode=%d", g_lang, g_colorMode, g_titleMode);
}

// Strings: 0=it,1=en,2=es,3=fr,4=pt,5=de,6=ru,7=zh,8=ja,9=ko
enum {
    STR_TITLE_PROJECT, STR_TITLE_SECOND, STR_OPT_INTERNAL, STR_OPT_CLONE, STR_OPT_EXTEND, STR_OPT_EXTERNAL,
    STR_MORE, STR_DETECT, STR_UNAVAILABLE, STR_SETMAIN, STR_COPYINFO,
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
    // A missing translation must never hand out a null pointer: the drawing code passes these straight to
    // lstrlenW / DrawText.
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
    return CreateFontW(-px, 0, 0, 0, weight, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, face);
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
    // 29 px light title and 15 px body text are the two sizes the Windows 8.1 pane uses: its title ink is 26
    // px tall and its tagline and labels 14 px.
    g_fTitle.reset(MkFont(S(29), L"Segoe UI Light"));
    g_fLabel.reset(MkFont(S(15), L"Segoe UI"));
    g_fText.reset(MkFont(S(14), L"Segoe UI"));
    g_fontScale = g_scale;
}

// ---------------------------------------------------------------- display state
// The four documented projection options, in the flyout order, each mapped to the single SDC_TOPOLOGY_xxx
// flag that requests it.
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

// Enumerates display paths with the pattern documented for QueryDisplayConfig: ask
// GetDisplayConfigBufferSizes for the sizes, query, and retry while the configuration changes underneath
// (ERROR_INSUFFICIENT_BUFFER).
static bool CcdQuery(UINT32 flags, std::vector<DISPLAYCONFIG_PATH_INFO>& paths,
                     std::vector<DISPLAYCONFIG_MODE_INFO>& modes, DISPLAYCONFIG_TOPOLOGY_ID* topology) {
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
        // currentTopologyId must be NULL for every flag except QDC_DATABASE_CURRENT, and must not be NULL for
        // that one.
        DISPLAYCONFIG_TOPOLOGY_ID* out =
            (flags == QDC_DATABASE_CURRENT) ? (topology ? topology : &dummyTopology) : NULL;
        result = QueryDisplayConfig(flags, &pathCount, paths.data(), &modeCount, modes.data(), out);
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

// The GDI device name of a source. The documentation states that this name can be used with the
// EnumDisplaySettings family, which is what the "set as main display" command needs.
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
        // MONITORINFOEX is the documented way to get the GDI device name of a monitor; it starts with the
        // same fields as MONITORINFO, so GetMonitorInfo accepts a pointer to it.
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

// The monitor description of a display device, from the monitor device node attached to it. Used when the
// display configuration API has no friendly name for a target (no EDID, a fallback driver, or a remote
// session).
static std::wstring GdiMonitorName(const std::wstring& dev) {
    if (dev.empty()) return std::wstring();
    DISPLAY_DEVICEW mon = {};
    mon.cb = sizeof(mon);
    if (!EnumDisplayDevicesW(dev.c_str(), 0, &mon, 0)) return std::wstring();
    return mon.DeviceString[0] ? std::wstring(mon.DeviceString) : std::wstring();
}

// The mode of a single display device, in physical pixels: the display configuration APIs are documented as
// not taking part in DPI virtualization.
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

// Rebuilds the model behind the panel: one row per connected display, the current topology, and the
// connected/active counts. Safe to call from any thread; painting reads the result under the shared lock.
static void RefreshState() {
    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;
    DISPLAYCONFIG_TOPOLOGY_ID topoId = (DISPLAYCONFIG_TOPOLOGY_ID)0;

    // QDC_DATABASE_CURRENT describes the currently connected displays, so a monitor that is plugged in but
    // switched off still shows up (and the topology of the saved configuration is returned for it).
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
            // A path whose target is not available is a disconnected connector, not a display: the
            // documentation warns that targetAvailable is what distinguishes the two.
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
                // In clone topology several paths share one source (same gdi), so deduplicate physical
                // displays by (adapterId, targetId).
                if (e.adapter.LowPart == r.adapter.LowPart && e.adapter.HighPart == r.adapter.HighPart &&
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

    // Fallback for the cases where the CCD API is unavailable (a non-WDDM driver, a remote session): the
    // classic display devices still describe the desktop, and everything except the exact topology can be
    // shown.
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

    // Name the displays the way the Screen Resolution page numbers them, and fill in the friendly name when
    // the driver could not provide one.
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

    // A fingerprint of everything the panel shows. The state watchdog compares it before and after a refresh
    // so a panel that has nothing new is not repainted every two seconds.
    unsigned stamp = (unsigned)(topology + 1) * 31 + (unsigned)connected * 7 + (unsigned)active;
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

// Applies one of the four documented topologies. The documentation for SetDisplayConfig requires NULL arrays
// and zero counts together with a SDC_TOPOLOGY_xxx flag: the system then takes the last stored configuration
// of that topology, or picks the best one if it has never been used.
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

    // SDC_VALIDATE and SDC_APPLY are documented as mutually exclusive, so the check is a separate call. Per
    // SetDisplayConfig documentation, if the stored configuration returns ERROR_BAD_CONFIGURATION, retry with
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
    // Applying a display layout makes Windows move focus and resize windows. Restart the foreground grace
    // period so that change does not dismiss the panel the user is using.
    g_openTick = GetTickCount();
    if (g_closeAfterApply) ClosePane();
}

// "Detect" as the classic Screen Resolution page implements it: re-apply the last saved configuration for the
// currently connected displays, which is the documented meaning of SDC_USE_DATABASE_CURRENT, then
// re-enumerate.
static void DetectDisplays() {
    SetStatus(T(STR_DETECTING));
    if (g_wnd) UpdateWindow(g_wnd);
    const LONG r = SetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY | SDC_USE_DATABASE_CURRENT);
    if (r != ERROR_SUCCESS) ReportFailure(r);
    else SetStatus(T(STR_APPLIED));
    RefreshState();
    InvalidatePanel();
}

// ChangeDisplaySettingsEx with CDS_SET_PRIMARY is the documented way to make a display the primary one. The
// devices are repositioned relative to the new origin and applied in one step, as the CDS_NORESET
// documentation requires.
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
                                      CDS_UPDATEREGISTRY | CDS_SET_PRIMARY | CDS_NORESET, NULL);
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
        ChangeDisplaySettingsExW(dd.DeviceName, &moved, NULL, CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
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
// The four silhouettes of the pane are not drawn by this mod: they are the Windows 8.1 bitmaps, cut out of a
// capture of the real flyout at 100% scaling and stored below as base64 PNG alpha masks, one per option, at
// the original 89x45 px size.
//
// Windows 8.1 paints them as white at two alpha levels: the screens that are in use are opaque, the screen
// that is not is at ~50%. That is why a single 8-bit mask reproduces the original everywhere: composited over
// the accent background it gives the light gray of the inactive screen, composited over the dark selection
// band it gives the darker gray of the same capture.
//
// How they were cut, for the record (src/extract_glyphs.py of the mod repository): crop the icon of each row
// out of the capture, then invert c = a*255 + (1-a)*bg for every pixel to recover its alpha (bg is the accent
// color, or the selection fill inside the band), keep background / 128 / 255, and save one grayscale-alpha
// PNG per option.
//
// At run time the blobs are decoded by the Windows Imaging Component (IWICImagingFactory, documented) into a
// premultiplied 32-bit bitmap, so no third-party image decoder is needed.
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

// Direct RLE-compressed 89x45 8-bit alpha masks of the same four Windows 8.1 icons in base64, decoded and
// resampled in pure C++ so icon rendering never depends on COM/WIC apartment state or GDI StretchBlt (which
// zeroes the alpha channel in HALFTONE mode at DPI != 100%).
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

// base64 PNG -> premultiplied 32-bit pixels. Every call here is documented: CryptStringToBinaryA,
// SHCreateMemStream, IWICImagingFactory. All COM objects are owned by ScopedComPtr RAII wrappers.
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

// Builds the four bitmaps and alpha masks at the size the pane needs, and rebuilds them when the pane moves
// to a monitor with another DPI. Resampling is performed in C++ directly on the alpha channel so GDI
// StretchBlt never zeroes out the alpha byte at non-100% DPI.
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

// Composites one icon over the row behind it. Directly blends the resampled alpha mask onto the 32-bpp
// DIBSection backbuffer when available, with an AlphaBlend fallback.
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

// Draws text so that the top of its capitals lands where the Windows 8.1 capture has it (`cellTop = inkTop -
// (tmAscent - cap)`). Uses DT_NOCLIP and wraps onto a second line if a localized string exceeds `right - x`,
// so no text is ever cut off with an ellipsis or clipped vertically.
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
    DrawTextW(dc, text, -1, &rc, DT_LEFT | DT_TOP | (singleFits ? DT_SINGLELINE : DT_WORDBREAK) |
                  DT_NOCLIP | DT_NOPREFIX);
}

// Draws an option label using the full available row width up to `right`, with DT_NOCLIP and word wrapping
// only when a localized label exceeds the row width so no option text is ever cut off or prematurely split.
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
    DrawTextW(dc, text, -1, &rc, DT_LEFT | DT_TOP | (singleFits ? DT_SINGLELINE : DT_WORDBREAK) |
                  DT_NOCLIP | DT_NOPREFIX);
}

// ---------------------------------------------------------------- layout (cached)
static Lay Layout() {
    if (g_cachedLayW == g_w && g_cachedLayH == g_h && g_cachedLayScale == g_scale) {
        return g_cachedLay;
    }
    // Measured on the Windows 8 "Second screen" capture (uploads/image-1.png), which is at 75 % scale: each
    // value is the capture pixel offset from the pane's left edge or top, divided by 0.75, in logical (96
    // DPI) pixels. S() applies the scale of the monitor the pane is on, so the layout stays DPI aware.
    Lay l;
    l.mx = S(45);
    l.titleX = S(91);
    l.titleInk = S(35);
    l.subInk = S(89);
    l.listY = S(129);
    l.rowH = S(133);
    l.glyphX = S(46);
    l.glyphY = S(43);
    l.textX = S(156);
    l.rowInk = S(49);
    l.backX = S(46);
    l.backY = S(28);
    l.backD = S(36);
    l.bandX = 0;
    g_cachedLay = l;
    g_cachedLayW = g_w;
    g_cachedLayH = g_h;
    g_cachedLayScale = g_scale;
    return l;
}

// ---------------------------------------------------------------- painting
// Back button: a ring with a left-pointing arrow in the top left corner of the pane. It leaves this panel and
// returns to the main Charms Bar (ReturnToCharmsBarFromDisplaySwitch). RAII owner of this panel's GDI+
// session. Started once, shut down when the mod unloads.
class PanelGdiplusSession {
public:
    PanelGdiplusSession() {
        Gdiplus::GdiplusStartupInput input;
        if (Gdiplus::GdiplusStartup(&token_, &input, nullptr) != Gdiplus::Ok) token_ = 0;
    }
    ~PanelGdiplusSession() {
        if (token_) Gdiplus::GdiplusShutdown(token_);
    }
    PanelGdiplusSession(const PanelGdiplusSession&) = delete;
    PanelGdiplusSession& operator=(const PanelGdiplusSession&) = delete;
    bool ok() const { return token_ != 0; }
private:
    ULONG_PTR token_ = 0;
};
// g_panelGdiplus: no_destroy prevents the destructor from running at process exit under the loader lock.
[[clang::no_destroy]] static std::unique_ptr<PanelGdiplusSession> g_panelGdiplus;
static bool BackButtonAt(int x, int y) {
    const Lay L = Layout();
    const int cx = L.backX + L.backD / 2, cy = L.backY + L.backD / 2;
    const int dx = x - cx, dy = y - cy, r = L.backD / 2 + S(3);
    return dx * dx + dy * dy <= r * r;
}

// hover: the Second screen option hover fill (panel colour, 18 % toward white) sits inside the ring, so the
// ring stroke covers its edge and nothing bleeds outside the 36 px circle.
static void DrawBackButton(HDC dc, const Lay& L, int ox, COLORREF pane, bool hover) {
    try {
        Gdiplus::Graphics g(dc);
        if (g.GetLastStatus() != Gdiplus::Ok) return;
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        const float stroke = (float)S(2);
        const float x = (float)(L.backX + ox), y = (float)L.backY, d = (float)L.backD;
        if (hover) {
            const COLORREF fill = Blend(pane, 18);
            Gdiplus::SolidBrush fillBrush(Gdiplus::Color(255, GetRValue(fill), GetGValue(fill), GetBValue(fill)));
            g.FillEllipse(&fillBrush, x + stroke / 2, y + stroke / 2, d - stroke, d - stroke);
        }
        Gdiplus::Pen ring(Gdiplus::Color(255, 255, 255, 255), stroke);
        g.DrawEllipse(&ring, x + stroke / 2, y + stroke / 2, d - stroke, d - stroke);

        const float cx = x + d / 2, cy = y + d / 2;
        const float tipX = cx - (float)S(9);
        Gdiplus::Pen arrow(Gdiplus::Color(255, 255, 255, 255), stroke);
        arrow.SetStartCap(Gdiplus::LineCapRound);
        arrow.SetEndCap(Gdiplus::LineCapRound);
        g.DrawLine(&arrow, tipX, cy, cx + (float)S(9), cy);
        g.DrawLine(&arrow, tipX, cy, tipX + (float)S(8), cy - (float)S(6));
        g.DrawLine(&arrow, tipX, cy, tipX + (float)S(8), cy + (float)S(6));
    } catch (...) {
        LogCaughtException(L"back button", nullptr);
    }
}

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

        // A snapshot: painting must never block on a refresh from the watchdog thread, and must never see the
        // model change halfway through a frame.
        int topology = TOPO_UNKNOWN, connected = 0;
        {
            SrwGuard g(&g_lock, false);
            topology = g_topology;
            connected = g_connected;
        }

        // The title and the tagline of Windows 8.1. The tagline is always T(STR_SUBTITLE) unless a status
        // message is active, which temporarily replaces it.
        {
            ScopedTextColor tc(mem.get(), RGB(255, 255, 255));
            ScopedSelectedObject so(mem.get(), g_fTitle.get());
            const wchar_t* title = T(g_titleMode == 1 ? STR_TITLE_SECOND : STR_TITLE_PROJECT);
            DrawTextInkTop(mem.get(), L.titleX + oxT, L.titleInk, W - S(16) + oxT, title);

            SelectObject(mem.get(), g_fLabel.get());
            std::wstring status;
            {
                SrwGuard sg(&g_lock, false);
                if (g_statusUntil && GetTickCount() < g_statusUntil) status = g_status;
            }
            DrawTextInkTop(mem.get(), L.mx + ox, L.subInk, W - S(16) + ox,
                           status.empty() ? T(STR_SUBTITLE) : status.c_str());
        }

        DrawBackButton(mem.get(), L, ox, pane, g_backHover);

        // The four projection options: the icon, then its label. The selected row gets the lighter band of
        // the capture; the Windows 8 capture has no white frame around it.
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
            // The mouse hook is used only while the panel is visible.
            SetOutsideClickHook(false);
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

// Next selectable index in the given direction, wrapping around. from = -1 starts before the first item (dir
// > 0) or after the last one (dir < 0).
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

// Records why the menu ends (first reason wins) and closes it asynchronously. `entry` is the index of the
// chosen item, -1 for a dismissal without a command; the row a command applies to comes from the entry
// itself.
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

// The menu lost focus/activation to `to`. Moving to the panel itself counts as an inside dismissal; any other
// window (or another thread's window, reported as NULL) is a click outside.
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

// Keeps the menu inside the work area of the monitor under (x, y), flipping it to the left/top of the anchor
// point when it would overflow, like a native menu.
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

// Creates the menu in the same z-band as the panel. A window in the default band would always be drawn below
// a panel living in ZBID_UIACCESS.
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

// The pane itself shows only what Windows 8.1 showed, so every command that goes beyond the four options
// lives here. The labels name the documented operation they perform, and a "Set as main display" entry is
// offered for each display that is active but not the main one.
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

    // Modal loop. GetMessageW returns 0 for WM_QUIT: re-post it so the UI thread's main loop also exits
    // (unload must never hang here).
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
        // Another window took the focus: never pull it back. Honor the "close when clicking outside" setting
        // that was suspended while the menu was open.
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
// Closing on "click outside" cannot rely only on WM_ACTIVATE / WM_KILLFOCUS. Those messages arrive only if
// the panel really became the foreground window, and SetForegroundWindow can be refused by the system (see
// its documentation: "It is possible for a process to be denied the right to set the foreground window even
// if it meets these conditions"). Clicks on windows that never take activation (taskbar areas, overlays,
// windows in other z-bands) do not deactivate the panel either. Two activation-independent detectors are
// used:
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

// Visible part of the panel: during the slide animation the left part of the window is still transparent
// (color key) and does not count as "inside".
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
        // Clicks on the panel are handled by the menu's own focus logic (inside dismissal). Anything else is
        // outside: the menu code then closes the panel too when "Close when clicking outside" is on.
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

        if (!g_wnd || !g_open || IsOurWindow(hwnd)) return;
        // Ignore foreground bounces during the slide-in animation or within the initial grace window right
        // after the panel opens.
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
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
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

        // The Windows 8.1 flyout spans the whole right edge of the screen; the compact mode ends the window
        // right below the fourth option.
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
        SetWindowPos(g_wnd, HWND_TOPMOST, r.right - g_w, r.top, g_w, g_h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        InvalidateRect(g_wnd, NULL, FALSE);
        SetTimer(g_wnd, 2, 1000, NULL);
        EnforceTopmost();
        Wh_Log(L"[Pane] OpenPane (x=%d y=%d w=%d h=%d scale=%.2f band=%u topo=%d connected=%d)",
               (int)(r.right - g_w), (int)r.top, g_w, g_h, g_scale, (unsigned)g_panelBand, topo, conn);
        if (g_debug) DebugDump(L"after OpenPane");
    }
    g_openTick = GetTickCount();
    StartAnim(true);
    // Light dismiss must work even if activation below is refused.
    SetOutsideClickHook(true);

    // A zero-input SendInput call gives this thread the documented opportunity to set the foreground window
    // without attaching its input queue to another application's thread. Sharing another application's input
    // queue is deliberately avoided: a hung foreground application must not freeze the panel or the display
    // change.
    INPUT input{};
    SendInput(1, &input, sizeof(input));
    SetForegroundWindow(g_wnd);
    if (GetForegroundWindow() == g_wnd)
        SetFocus(g_wnd);
    g_openTick = GetTickCount();

    if (GetForegroundWindow() != g_wnd) {
        // The system may still deny the foreground change. The pane remains usable with the mouse and closes
        // on outside clicks via the hook.
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

// Bottom of the pane content, so the "not full height" mode can end the window right below the fourth option.
static int PaneContentHeight() {
    Lay L = Layout();
    return L.listY + TOPO_COUNT * L.rowH + S(12);
}

// ---------------------------------------------------------------- window procedure
static LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        switch (msg) {
        case WM_PAINT:
            // A paint failure must not unwind into the window procedure of Explorer.
            try {
                DoPaint(h);
            } catch (...) {
                LogCaughtException(L"paint", nullptr);
                ValidateRect(h, NULL);
            }
            return 0;
        case WM_ERASEBKGND: return 1;
        case WM_CANCELMODE:
            return 0;
        case WM_DISPLAYCHANGE:
            // Documented as sent to all top-level windows when the display settings change, so even a change
            // made by the native shortcut, by the Settings app, or by a monitor hotplug updates the panel.
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
            // Keyboard invocation (Menu key / Shift+F10) reports (-1, -1). Mouse right-clicks are handled in
            // WM_RBUTTONUP.
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
            const bool backHover = BackButtonAt(x, y);
            if (opt != g_hoverOpt || backHover != g_backHover) {
                g_hoverOpt = opt;
                g_backHover = backHover;
                InvalidateRect(h, NULL, FALSE);
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            if (g_hoverOpt != -1 || g_backHover) {
                g_hoverOpt = -1;
                g_backHover = false;
                InvalidateRect(h, NULL, FALSE);
            }
            return 0;
        case WM_LBUTTONDOWN: {
            // A click that dismisses the context menu must not also run a command.
            if (g_state != ST_IDLE || !g_open || g_inMenu) return 0;
            const int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
            if (BackButtonAt(x, y)) {
                ClosePane();
                ReturnToCharmsBarFromDisplaySwitch();
                return 0;
            }
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
            // Right-clicking anywhere in the pane opens the menu: the pane has no per-row commands of its
            // own, unlike the display list it does not have.
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

// Keeps the panel honest while it is open: a monitor plugged in, a display turned off, or a topology change
// made through the native shortcut all change what the panel must show. WM_DISPLAYCHANGE already covers most
// of it; this timer loop is the safety net for the cases where no message reaches a top-level window of this
// thread.
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

// This thread owns the on-demand low-level mouse hook used for light dismiss. It always keeps a message queue
// so hook callbacks stay off the UI thread and the desktop-switch event can reset keyboard state on this
// thread.
static DWORD WINAPI KeyboardHookThread(LPVOID) {
    MSG queued = {};
    PeekMessageW(&queued, NULL, WM_USER, WM_USER, PM_NOREMOVE); // create the queue first

    SignalKeyboardReady(true);

    ScopedHook mouseHook;

    try {
        MSG msg;
        int result = 0;
        while ((result = GetMessageW(&msg, NULL, 0, 0)) > 0) {
            // No window dispatch is needed on this thread. Retrieving messages is enough to service the
            // low-level callbacks and control hooks.
            if (msg.hwnd != NULL) continue;

            if (msg.message == WM_HOOKCTL_MOUSE_ON) {
                if (!mouseHook.get()) {
                    HHOOK mh = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, (HINSTANCE)&__ImageBase, 0);
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
    // A queue exists before initialization can fail. This makes WM_QUIT reliable during an early unload and
    // lets Wh_ModInit wait for the panel startup state.
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

        // Wh_ModInit waits for this signal before it starts the remaining infrastructure or returns. The
        // window and message queue are ready here.
        SignalUiReady(true);
        readySignaled = true;

        ScopedTimer enforceTimer(g_wnd, 3, 2000);

        // Out-of-context WinEvent hook: delivered on this thread, which has a message loop as SetWinEventHook
        // requires.
        g_foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                           NULL, ForegroundEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        if (!g_foregroundHook)
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

    if (g_foregroundHook) { UnhookWinEvent(g_foregroundHook); g_foregroundHook = nullptr; }
    g_mouseHookWanted = false;

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

// The shell process owns GetShellWindow. If it is in the middle of restarting, exclude the known
// folder-window command lines while waiting for its taskbar.
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

// Retry WM_QUIT while a thread is alive. A normal startup creates the message queue before signaling
// readiness; retrying also covers an unload racing an unsuccessful startup instead of waiting forever after
// one failed post.
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
    // Stop the hook thread first so it cannot post work to a panel that is going away, then the state
    // watchdog, then the panel itself.
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

void SettingsChanged() {
    RunGuarded(L"applying settings", [] { LoadSettings(); RefreshState(); });
    InvalidatePanel();
}

BOOL Init() {
    try {
        if (!g_panelGdiplus) g_panelGdiplus = std::make_unique<PanelGdiplusSession>();
    } catch (...) {
        LogCaughtException(L"GDI+ session", nullptr);
    }
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

void Uninit() {
    RunGuarded(L"Wh_ModUninit", [] {
        Wh_Log(L"[Uninit] shutting down, exceptions=%u", g_exceptionCount.load());
        StopInfrastructure();
    });
    g_panelGdiplus.reset();
}

// ---------------------------------------------------------------- Charms Bar integration
// Opens or closes the Second screen / Project panel. First tries to hand off to the companion
// Windows 8/8.1 Display Switch Recreation mod (which may be installed alongside this one), then
// falls back to the built-in panel. If neither is available, returns false so the caller can
// fall back to Win+P.
bool TogglePanel() {
    // 1. Try the companion Display Switch mod (win8-display-switch-recreation).
    HWND externalPanel = FindWindowW(L"Win81DisplaySwitchMod", NULL);
    if (externalPanel) {
        PostMessageW(externalPanel, WM_APP + 1, 0, 0);
        return true;
    }
    // 2. Fall back to the built-in panel – lazy initialization.
    HWND panel = FindWindowW(kClass, NULL);
    if (!panel) {
        if (!Init()) {
            return false;
        }
        panel = FindWindowW(kClass, NULL);
        if (!panel) return false;
    }
    PostMessageW(panel, WM_APP_TOGGLE, 0, 0);
    return true;
}
} // namespace DisplaySwitchMod

namespace {
using Microsoft::WRL::ComPtr;
using Microsoft::WRL::Wrappers::HStringReference;
namespace awf = ABI::Windows::Foundation;
namespace dt = ABI::Windows::ApplicationModel::DataTransfer;

// Desktop WinRT interop contract for invoking the system Share UI.
MIDL_INTERFACE("3A3DCD6C-3EAB-43DC-BCDE-45671CE800C8")
IDataTransferManagerInteropLocal : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetForWindow(HWND appWindow, REFIID riid, void** result) = 0;
    virtual HRESULT STDMETHODCALLTYPE ShowShareUIForWindow(HWND appWindow) = 0;
};
// Public Windows.UI.ViewManagement.IUISettings3 ABI surface for the documented GetColorValue method; a local
// declaration keeps this single-file mod independent of optional C++/WinRT projection headers.
struct UiSettingsColor { BYTE A; BYTE R; BYTE G; BYTE B; };
MIDL_INTERFACE("03021BE4-5254-4781-8194-5168F7D06D7B")
IUISettings3AccentLocal : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE GetColorValue(INT32 desiredColor, UiSettingsColor* value) = 0;
};
static const GUID kIidUISettings3Accent =
    {0x03021be4,0x5254,0x4781,{0x81,0x94,0x51,0x68,0xf7,0xd0,0x6d,0x7b}};
// Explicit IIDs avoid MinGW/LLD's unresolved __mingw_uuidof references.
static const GUID kIidDataTransferManagerInterop =
    {0x3a3dcd6c,0x3eab,0x43dc,{0xbc,0xde,0x45,0x67,0x1c,0xe8,0x00,0xc8}};
static const GUID kIidShareEventHandler =
    {0xec6f9cc8,0x46d0,0x5e0e,{0xb4,0xd2,0x7d,0x77,0x73,0xae,0x37,0xa0}};
// Windows.Media.PlayTo.PlayToManager, the public WinRT surface behind the Windows 8 Devices > Play charm.
// ShowPlayToUI is a plain static method, so the activation factory for the runtime class hands us the statics
// interface directly and no projection headers are needed. The API is marked deprecated but still ships and
// works on Windows 10/11, which makes it the simplest faithful stand-in for the original charm.
MIDL_INTERFACE("64E6A887-3982-4F3B-BA20-6155E435325B")
IPlayToManagerStaticsLocal : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE GetForCurrentView(IInspectable** result) = 0;
    virtual HRESULT STDMETHODCALLTYPE ShowPlayToUI() = 0;
};
static const GUID kIidPlayToManagerStatics =
    {0x64e6a887,0x3982,0x4f3b,{0xba,0x20,0x61,0x55,0xe4,0x35,0x32,0x5b}};

constexpr UINT WM_PORT_OPEN = WM_APP + 0x461;
constexpr UINT WM_PORT_CLOSE = WM_APP + 0x462;
constexpr UINT WM_PORT_REFRESH = WM_APP + 0x463;
constexpr UINT WM_PORT_MENU = WM_APP + 0x464;
constexpr UINT WM_PORT_NAVIGATE = WM_APP + 0x465;
constexpr UINT WM_PORT_ACTIVATE = WM_APP + 0x466;
constexpr UINT_PTR TIMER_ANIMATION = 1;
constexpr UINT_PTR TIMER_STATUS = 2;
constexpr UINT_PTR TIMER_CLOCK_STATUS = 3;
constexpr UINT_PTR TIMER_PANE = 4;
constexpr UINT_PTR TIMER_VOLUME = 5;
constexpr UINT_PTR TIMER_BRIGHTNESS_APPLY = 6;
constexpr UINT_PTR TIMER_PANE_METRICS = 7;
constexpr UINT_PTR TIMER_SEARCH_VERIFY = 8; // 700 ms after a Win chord: did the Windows search UI appear?
constexpr UINT_PTR TIMER_START_VERIFY = 9;  // 900 ms after handing Start over: did that window appear?
constexpr UINT_PTR TIMER_STARTTIP_DWELL = 10; // is the pointer still parked in the corner?
constexpr UINT_PTR TIMER_STARTTIP_FRAME = 11; // 15 ms frame tick for the fade and the glint
constexpr UINT_PTR TIMER_STARTTIP_POLL = 12;  // 500 ms: is the Start screen still there?
constexpr UINT_PTR TIMER_STARTTIP_CLOSE = 13;  // 450 ms after a click: did the Start screen really close?
constexpr UINT_PTR TIMER_STARTTIP_EXIT = 14;   // grace expired: has the pointer really left for good?
// The companion does not snap its window shut: it slides the Start screen out over its own closeSlideMs,
// 600 ms unless that setting was changed. So the close is watched rather than timed: a fixed check would
// either fire into the middle of the slide or wait far too long for it.
constexpr int kStartTipClosePollMs = 150;      // how often the slide-out is looked at
constexpr int kStartTipCloseTimeoutMs = 4000;  // stop watching after this; nothing is ever forced
constexpr int kStartTipSuppressAfterClickMs = 900; // the tip also stays away this long after a click
constexpr UINT WM_PORT_PANECLOSE = WM_APP + 0x467;
constexpr UINT WM_PORT_MENU_COMMAND = WM_APP + 0x468;
constexpr UINT WM_PORT_STARTTIPHIDE = WM_APP + 0x469; // the pointer left the corner
constexpr UINT WM_PORT_STARTTIPARM = WM_APP + 0x46A;  // start the dwell timer for the corner
constexpr UINT WM_PORT_STARTTIPLEAVE = WM_APP + 0x46B; // pointer left: run the grace down on a timer
// The Windows 8.1 "Start tip": a 48 px black square in the bottom-left corner holding the 30 px Windows
// flag, all at 96 dpi. Windows showed it as soon as the pointer reached the corner - practically at once
// there, after a short pause when the pointer only brushed the edge - so the corner has its own dwell.
constexpr int kStartTipSize = 48;
constexpr int kStartTipLogoSize = 30;
// The hot area is deliberately generous: an 8 px strip is what Windows used, and it is easy to miss by a
// few pixels and lose the tip while moving towards it. The strip is wider and the bottom-left corner is a
// square of its own, so resting "at the left, near the bottom" always lands in it.
constexpr int kStartTipEdgeWidth = 12;      // strip along the left edge, full height
constexpr int kStartTipCornerSize = 140;    // square in the bottom-left corner
constexpr int kStartTipLeaveGraceMs = 300;  // time to travel from the edge to the square
constexpr int kStartTipHitPadding = 12;     // the square is easier to hit than it looks
constexpr ULONGLONG kStartTipCornerDwellMs = 120;
constexpr ULONGLONG kStartTipDwellMs = 400;
constexpr int kStartTipFadeMs = 150;   // the square fades in and grows a touch, it does not pop
constexpr int kStartTipGlintMs = 420;  // the gloss that sweeps across the flag on hover
constexpr int kStartTipHoverMs = 120;  // white -> accent, instead of an instant swap
constexpr int kStartTipRevealPercent = 94; // the flag starts at 94 % and settles at 100 %
constexpr wchar_t kStartTipClass[] = L"WhCharmsBarPortStartTip";
enum CharmPaneKind : int { PANE_SETTINGS=0, PANE_DEVICES=1, PANE_SHARE=2 };
enum MetroMenuPurpose : int { METRO_MENU_NONE=0, METRO_MENU_QUICK=1, METRO_MENU_POWER=2 };
struct MetroMenuItem { std::wstring text; int command; bool separator; bool enabled; };
constexpr int kPaneWidth = 346;
// TwinUI UIFILE 8801 (Windows 8 RTM 6.2.9200.16384 and Windows 8.1 6.3.9600.17041) lays out hardware flyouts
// as a 65×100rp slider plus a 65×40rp status/glyph area. DUI70 6.2.9200.16384 / 6.3.9600.17031's vertical
// TouchSlider metrics are 51rp wide with a 13rp track and 11rp thumb. These values affect only the mod-owned
// popup; the native Charms are untouched.
constexpr int kTwinUiHardwareSliderHeight = 100;
constexpr int kTwinUiHardwareStatusHeight = 40;
constexpr int kVolumeFlyoutWidth = 65;
constexpr int kVolumeFlyoutHeight = kTwinUiHardwareSliderHeight + kTwinUiHardwareStatusHeight;
// Geometry of the hardware flyout (volume and brightness share the screenshot geometry).
struct HwFlyoutMetrics { int width,height,trackTop,trackBottom,barWidth,thumbHeight; };
HwFlyoutMetrics HwMetrics();
constexpr int kMetroMenuWidth = 220;
constexpr int kMetroMenuItemHeight = 34;
constexpr int kMetroMenuPadY = 6;
constexpr int kMetroMenuPadX = 14;
static const GUID kGuidCurrentMonitorBrightness={0x8ffee2c6,0x2d01,0x46be,{0xad,0xb9,0x39,0x8a,0xdd,0xc5,0xb4,0xff}};
static const GUID kGuidVideoPolicyBrightness={0xaded5e82,0xb909,0x4619,{0x99,0x49,0xf5,0xd7,0x1d,0xac,0x0b,0xcb}};
constexpr wchar_t kPaneClass[] = L"WhCharmsBarPortSettingsPane";
constexpr wchar_t kVolumeFlyoutClass[] = L"WhCharmsBarPortVolumeFlyout";
constexpr wchar_t kVolumeTipClass[] = L"WhCharmsBarPortVolumeTip";
// TwinUI UIFILE 7001 in Windows 8 RTM 6.2.9200.16384 and Windows 8.1 6.3.9600.17041 uses an 86rp rail and
// 100rp BaseCharm cells; these dimensions apply only to this mod-owned rail.
constexpr int kRailWidth = 86;
constexpr int kCharmCount = 5;
constexpr int kGridCell = 100;
constexpr int kClockWidth = 475;
constexpr int kClockHeight = 140;
// ---------------------------------------------------------------- z-band surfaces
// Windows 8 draws the Charms Bar over the Start screen. A plain WS_EX_TOPMOST window cannot do that once
// another window sits in a higher z-band, which is exactly what the Windows 8 (8045) Start Screen mod does
// with its full-screen surface. Every Charms Bar window is therefore created through CreateWindowInBand in
// the same band chain the companion mods use: band 2 (UIAccess) first because that is the band the companion
// Start Screen and Search Charm mods actually get, band 16 (system tools) second, and an ordinary topmost
// window as the fallback when the undocumented API is missing. Staying in the same band matters: two surfaces
// in band 2 can be re-ordered with SetWindowPos, two different bands cannot.
typedef HWND (WINAPI *CreateWindowInBandCharms_t)(DWORD, LPCWSTR, LPCWSTR, DWORD,
                                                  int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID, DWORD);
static CreateWindowInBandCharms_t g_charmsCreateInBand = nullptr;
static int g_charmsBand = -1; // band that worked here; -1 = not tried yet, 0 = none available

static HWND CreateCharmsSurface(DWORD exStyle,PCWSTR className,PCWSTR title,DWORD style,
                                int x,int y,int w,int h,HWND parent,HINSTANCE instance) {
    if(g_charmsCreateInBand) {
        const DWORD bands[2]={2,16};
        for(const DWORD band : bands) {
            if(g_charmsBand>0 && (DWORD)g_charmsBand!=band) continue; // reuse the band already proven here
            const HWND hwnd=g_charmsCreateInBand(exStyle,className,title,style,x,y,w,h,
                                                 parent,nullptr,instance,nullptr,band);
            if(hwnd) {
                if(g_charmsBand!=(int)band) {
                    g_charmsBand=(int)band;
                    Wh_Log(L"%s created in z-band %u (above the Start screen and fullscreen apps)",
                           className,(unsigned)band);
                }
                return hwnd;
            }
        }
        if(g_charmsBand<0) {
            g_charmsBand=0;
            Wh_Log(L"no z-band available; the Charms Bar falls back to ordinary topmost windows");
        }
    }
    return CreateWindowExW(exStyle,className,title,style,x,y,w,h,parent,nullptr,instance,nullptr);
}

// Re-raise one of our surfaces. Both this mod and the Start Screen mod live in band 2, so this puts the
// Charms Bar above the full-screen Start screen, the way Windows 8 did - and never steals focus.
static void RaiseCharmsSurface(HWND hwnd) {
    if(hwnd) SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
}

constexpr wchar_t kRailClass[] = L"WhCharmsBarPortRail";
constexpr wchar_t kClockClass[] = L"WhCharmsBarPortClock";
constexpr wchar_t kMenuClass[] = L"WhCharmsBarPortMenu";
struct CharmDef { int id; const wchar_t* en; const wchar_t* it; const wchar_t* de; const wchar_t* ja; const wchar_t* fr; const wchar_t* es;
                  const wchar_t* pt; const wchar_t* ru; const wchar_t* zh; const wchar_t* ko; };

// Small non-throwing ownership wrappers keep GDI, process, registry, Native Wi-Fi and COM resources balanced
// across early returns and C++ exceptions.
class ScopedDC {
public:
    explicit ScopedDC(HDC dc = nullptr) noexcept : value_(dc) {}
    ~ScopedDC() noexcept {
        if (value_ && !DeleteDC(value_))
            Wh_Log(L"DeleteDC failed (%lu)",GetLastError());
    }
    ScopedDC(const ScopedDC&) = delete;
    ScopedDC& operator=(const ScopedDC&) = delete;
    HDC get() const noexcept { return value_; }
    explicit operator bool() const noexcept { return value_ != nullptr; }
private:
    HDC value_;
};

class ScopedHandle {
public:
    explicit ScopedHandle(HANDLE handle=nullptr) noexcept : value_(handle) {}
    ~ScopedHandle() noexcept { reset(); }
    ScopedHandle(const ScopedHandle&)=delete;
    ScopedHandle& operator=(const ScopedHandle&)=delete;
    HANDLE get() const noexcept { return value_; }
    HANDLE* put() noexcept { reset(); return &value_; }
    explicit operator bool() const noexcept { return value_ && value_!=INVALID_HANDLE_VALUE; }
    void reset(HANDLE handle=nullptr) noexcept {
        if(value_ && value_!=INVALID_HANDLE_VALUE && !CloseHandle(value_))
            Wh_Log(L"CloseHandle failed (%lu)",GetLastError());
        value_=handle;
    }
private:
    HANDLE value_=nullptr;
};

class ScopedGuidMemory {
public:
    ~ScopedGuidMemory() noexcept {
        if(value_ && LocalFree(value_))
            Wh_Log(L"freeing power-scheme GUID failed (%lu)",GetLastError());
    }
    ScopedGuidMemory() noexcept = default;
    ScopedGuidMemory(const ScopedGuidMemory&)=delete;
    ScopedGuidMemory& operator=(const ScopedGuidMemory&)=delete;
    GUID** put() noexcept {
        if(value_) LocalFree(value_);
        value_=nullptr;
        return &value_;
    }
    GUID* get() const noexcept { return value_; }
private:
    GUID* value_=nullptr;
};

class ScopedGdiObject {
public:
    explicit ScopedGdiObject(HGDIOBJ object = nullptr) noexcept : value_(object) {}
    ~ScopedGdiObject() noexcept {
        if (value_ && !DeleteObject(value_))
            Wh_Log(L"DeleteObject failed (%lu)",GetLastError());
    }
    ScopedGdiObject(const ScopedGdiObject&) = delete;
    ScopedGdiObject& operator=(const ScopedGdiObject&) = delete;
    HGDIOBJ get() const noexcept { return value_; }
    HGDIOBJ release() noexcept { HGDIOBJ result=value_; value_=nullptr; return result; }
    explicit operator bool() const noexcept { return value_ != nullptr; }
private:
    HGDIOBJ value_;
};

class ScopedSelectObject {
public:
    ScopedSelectObject(HDC dc, HGDIOBJ object) noexcept : dc_(dc) {
        if (dc_ && object) {
            previous_=SelectObject(dc_,object);
            selected_=previous_ && previous_!=HGDI_ERROR;
        }
    }
    ~ScopedSelectObject() noexcept {
        if (selected_) {
            HGDIOBJ restored=SelectObject(dc_,previous_);
            if (!restored || restored==HGDI_ERROR)
                Wh_Log(L"restoring selected GDI object failed (%lu)",GetLastError());
        }
    }
    ScopedSelectObject(const ScopedSelectObject&) = delete;
    ScopedSelectObject& operator=(const ScopedSelectObject&) = delete;
    explicit operator bool() const noexcept { return selected_; }
private:
    HDC dc_=nullptr;
    HGDIOBJ previous_=nullptr;
    bool selected_=false;
};

class ScopedRegKey {
public:
    ~ScopedRegKey() noexcept {
        if (value_) {
            const LONG rc=RegCloseKey(value_);
            if (rc!=ERROR_SUCCESS) Wh_Log(L"RegCloseKey failed (%ld)",rc);
        }
    }
    ScopedRegKey(const ScopedRegKey&) = delete;
    ScopedRegKey& operator=(const ScopedRegKey&) = delete;
    ScopedRegKey() noexcept = default;
    HKEY* put() noexcept { return &value_; }
    HKEY get() const noexcept { return value_; }
private:
    HKEY value_=nullptr;
};

class ScopedWlanHandle {
public:
    ~ScopedWlanHandle() noexcept {
        if (value_) {
            const DWORD rc=WlanCloseHandle(value_,nullptr);
            if (rc!=ERROR_SUCCESS) Wh_Log(L"WlanCloseHandle failed (%lu)",rc);
        }
    }
    ScopedWlanHandle(const ScopedWlanHandle&) = delete;
    ScopedWlanHandle& operator=(const ScopedWlanHandle&) = delete;
    ScopedWlanHandle() noexcept = default;
    HANDLE* put() noexcept { return &value_; }
    HANDLE get() const noexcept { return value_; }
private:
    HANDLE value_=nullptr;
};

class ScopedShutdownPrivilege {
public:
    ~ScopedShutdownPrivilege() noexcept { Restore(); }
    ScopedShutdownPrivilege() noexcept = default;
    ScopedShutdownPrivilege(const ScopedShutdownPrivilege&)=delete;
    ScopedShutdownPrivilege& operator=(const ScopedShutdownPrivilege&)=delete;
    bool Enable(HANDLE token) noexcept {
        if(!token || active_) return false;
        LUID luid{};
        if(!LookupPrivilegeValueW(nullptr,SE_SHUTDOWN_NAME,&luid)) return false;
        TOKEN_PRIVILEGES enable{};
        enable.PrivilegeCount=1;
        enable.Privileges[0].Luid=luid;
        enable.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
        DWORD previousSize=sizeof(previous_);
        SetLastError(ERROR_SUCCESS);
        const BOOL adjusted=AdjustTokenPrivileges(token,FALSE,&enable,sizeof(previous_),
                                                   &previous_,&previousSize);
        const DWORD error=GetLastError();
        if(!adjusted || error==ERROR_NOT_ALL_ASSIGNED) {
            if(adjusted && previous_.PrivilegeCount>0)
                AdjustTokenPrivileges(token,FALSE,&previous_,0,nullptr,nullptr);
            return false;
        }
        token_=token;
        active_=true;
        return true;
    }
private:
    void Restore() noexcept {
        if(active_ && !AdjustTokenPrivileges(token_,FALSE,&previous_,0,nullptr,nullptr))
            Wh_Log(L"restoring the shutdown privilege failed (%lu)",GetLastError());
        active_=false;
    }
    HANDLE token_=nullptr;
    TOKEN_PRIVILEGES previous_{};
    bool active_=false;
};

class ScopedWlanMemory {
public:
    ~ScopedWlanMemory() noexcept { if (value_) WlanFreeMemory(value_); }
    ScopedWlanMemory(const ScopedWlanMemory&) = delete;
    ScopedWlanMemory& operator=(const ScopedWlanMemory&) = delete;
    ScopedWlanMemory() noexcept = default;
    PVOID* put() noexcept { return &value_; }
    PVOID get() const noexcept { return value_; }
private:
    PVOID value_=nullptr;
};

class ScopedWhString {
public:
    explicit ScopedWhString(PCWSTR value=nullptr) noexcept : value_(value) {}
    ~ScopedWhString() noexcept { if (value_) Wh_FreeStringSetting(value_); }
    ScopedWhString(const ScopedWhString&) = delete;
    ScopedWhString& operator=(const ScopedWhString&) = delete;
    PCWSTR get() const noexcept { return value_; }
private:
    PCWSTR value_;
};

class ScopedHString {
public:
    ScopedHString() noexcept = default;
    ~ScopedHString() noexcept { if (value_) WindowsDeleteString(value_); }
    ScopedHString(const ScopedHString&) = delete;
    ScopedHString& operator=(const ScopedHString&) = delete;
    HSTRING* put() noexcept { if (value_) { WindowsDeleteString(value_); value_=nullptr; } return &value_; }
    HSTRING get() const noexcept { return value_; }
private:
    HSTRING value_=nullptr;
};

class ScopedPaint {
public:
    explicit ScopedPaint(HWND hwnd) noexcept : hwnd_(hwnd) {
        dc_=BeginPaint(hwnd_,&paint_);
        begun_=true;
    }
    ~ScopedPaint() noexcept {
        if (begun_ && !EndPaint(hwnd_,&paint_))
            Wh_Log(L"EndPaint failed (%lu)",GetLastError());
    }
    ScopedPaint(const ScopedPaint&) = delete;
    ScopedPaint& operator=(const ScopedPaint&) = delete;
    HDC get() const noexcept { return dc_; }
private:
    HWND hwnd_;
    PAINTSTRUCT paint_{};
    HDC dc_=nullptr;
    bool begun_=false;
};

const CharmDef kCharmDefs[kCharmCount] = {
    {0, L"Search", L"Ricerca", L"Suchen", L"検索", L"Rechercher", L"Buscar",
        L"Pesquisar", L"Поиск", L"搜索", L"검색"},
    {1, L"Share", L"Condividi", L"Freigeben", L"共有", L"Partager", L"Compartir",
        L"Compartilhar", L"Поделиться", L"共享", L"공유"},
    {2, L"Start", L"Start", L"Start", L"スタート", L"Accueil", L"Inicio",
        L"Iniciar", L"Пуск", L"开始", L"시작"},
    {3, L"Devices", L"Dispositivi", L"Geräte", L"デバイス", L"Périphériques", L"Dispositivos",
        L"Dispositivos", L"Устройства", L"设备", L"장치"},
    {4, L"Settings", L"Impostazioni", L"Einstellungen", L"設定", L"Paramètres", L"Configuración",
        L"Configurações", L"Параметры", L"设置", L"설정"}
};

HANDLE g_thread = nullptr, g_ready = nullptr;
HANDLE g_hookThread = nullptr, g_hookReady = nullptr;
DWORD g_threadId = 0, g_hookThreadId = 0;
bool g_clockStatusTimerActive = false;
HWND g_rail = nullptr, g_clock = nullptr, g_menu = nullptr, g_pane = nullptr;
HWND g_volumeFlyout = nullptr, g_volumeTip = nullptr;
std::vector<MetroMenuItem> g_metroMenuItems;
int g_metroMenuPurpose=METRO_MENU_NONE, g_metroMenuHover=-1;
bool g_metroMenuActive=false;
HHOOK g_keyboardHook = nullptr, g_mouseHook = nullptr;
HWINEVENTHOOK g_foregroundHook = nullptr;
HPOWERNOTIFY g_brightnessNotifyCurrent=nullptr, g_brightnessNotifyPolicy=nullptr;
HMONITOR g_monitor = nullptr;
RECT g_monitorRect = {}, g_workRect = {};
// Atomics are assigned explicitly in Wh_ModInit so the code also works with standard-library builds that omit
// value-taking atomic constructors.
std::atomic<bool> g_stop;
std::atomic<bool> g_useWinC, g_edgeActivation, g_topRightCornerOnly, g_useAnimations;
std::atomic<bool> g_charmsEnabled, g_hideAfterClick;
std::atomic<bool> g_showClock, g_showQuickMenu, g_fullHeight;
std::atomic<bool> g_closeOnClickOutside;
std::atomic<int> g_opacity, g_openMs, g_closeMs, g_edgeDelay, g_startLogoStyle;
// 0 = Windows 8/8.1 regular icons, 1 = Windows 8 Build 8102 beta icon set (Start logo excluded).
std::atomic<int> g_charmIconSet{0};
std::atomic<bool> g_enableTransparency;
std::atomic<bool> g_settingsPane{true}, g_paneVisible{false};
std::atomic<int> g_paneColorMode{0}, g_paneKind{PANE_SETTINGS};

std::atomic<int> g_searchHotkey, g_shareBehavior, g_clockFormat, g_uiScalePercent, g_uiDpi;
// 0 = automatic (the companion search mod when it is installed, otherwise the Windows shortcut),
// 1 = always use the Windows shortcut, ignoring the companion mod.
std::atomic<int> g_searchTarget;
// True while the native search UI is believed to be open. Set by the foreground hook and by every chord
// we send, cleared when another window takes the foreground or when the 700 ms verification fails.
std::atomic<bool> g_searchKnownOpen;
std::atomic<DWORD> g_searchVerifyTick;
// GetTickCount() until which the Start Screen hand-off is parked after an unanswered request.
std::atomic<DWORD> g_companionStartRetryAfter;
// Start tip window, its hover latching (hook thread) and its animation state (UI thread).
HWND g_startTip = nullptr;
std::atomic<bool> g_startTipShown{false};
std::atomic<bool> g_startTipArmed{false}; // hook thread writes, UI thread clears on hide
// GetTickCount() until which the tip must not reappear: set when it is clicked, so a pointer that stays
// in the corner cannot bring it back during the moment the Start screen takes to slide away.
std::atomic<DWORD> g_startTipSuppressUntil;
ULONGLONG g_startTipLeaveSince = 0; // hook thread only: when the pointer left the hot area
ULONGLONG g_startTipShownAt = 0;   // UI thread: fade start
ULONGLONG g_startTipGlintAt = 0;   // UI thread: gloss start, 0 when idle
ULONGLONG g_startTipLastFrame = 0; // UI thread: previous animation frame
ULONGLONG g_startTipCloseDeadline = 0; // UI thread: when to stop watching a close that never happens
double g_startTipHoverT = 0.0;     // UI thread: 0 = white flag, 1 = accent flag
bool g_startTipHover = false;      // UI thread: pointer over the square
bool g_startTipPressed = false;    // UI thread: left button held on the square
bool g_startTipTracking = false;   // UI thread: WM_MOUSELEAVE armed
std::atomic<int> g_uiLanguage;
std::atomic<int> g_charmSlotCount;
std::atomic<int> g_actionMap[kCharmCount];

std::atomic<bool> g_threadPerMonitorDpi;
std::atomic<DWORD> g_openTick, g_noFocusDismissUntil;
std::atomic<bool> g_hidden[kCharmCount];
std::atomic<int> g_hover, g_pressed, g_keyboardSelection;
std::atomic<bool> g_visible, g_keyboardOpened, g_shareReady;
std::atomic<bool> g_startSucceeded, g_disableStartFade, g_hooksSucceeded;
std::atomic<bool> g_disableTRCorner, g_disableBRCorner, g_disableCharmsHint, g_registryQuickMenu;
std::atomic<int> g_desktopEdgeDelay, g_immersiveEdgeDelay;
std::atomic<COLORREF> g_accent;
std::atomic<int> g_networkState, g_wifiSignal, g_batteryLevel;
std::atomic<bool> g_batteryCharging, g_adapterDetected, g_airplane, g_wifiRadioOff;
std::atomic<int> g_animStep, g_animTarget, g_slideOffset;
int g_animFromAlpha = 0, g_animFromSlide = 0, g_animToSlide = 0;
int g_animDurationMs = 0;
LONGLONG g_animStartQpc = 0, g_animQpcFrequency = 0;
std::atomic<bool> g_edgeLatched, g_menuEdgeLatched, g_edgePosted;
std::atomic<ULONGLONG> g_edgeSince;
std::wstring g_shareTitle, g_shareText;

void HideVolumeFlyout();
void HideVolumeTip();
void RepositionVisibleVolumeFlyout();
void HideMetroContextMenu(bool restoreFocus);
void ShowPowerMetroMenu(HWND hwnd);
void ShowQuickMetroMenu(POINT anchor);
void DispatchMetroMenuCommand(int purpose,int command);

BYTE GetRailWindowAlpha(int animationAlpha) noexcept {
    animationAlpha=(std::max)(0,(std::min)(255,animationAlpha));
    if(!g_enableTransparency.load()) return animationAlpha>0?255:0;
    return (BYTE)(animationAlpha*g_opacity.load()/255);
}
ULONG_PTR g_gdiplusToken = 0;
// g_dtmInterop, g_dtm, g_dataRequested: no_destroy prevents COM destructors from running at process exit
// under the loader lock after the STA thread is already gone.
[[clang::no_destroy]] ComPtr<IDataTransferManagerInteropLocal> g_dtmInterop;
[[clang::no_destroy]] ComPtr<dt::IDataTransferManager> g_dtm;
using ShareEventHandler = awf::ITypedEventHandler<dt::DataTransferManager*, dt::DataRequestedEventArgs*>;
[[clang::no_destroy]] ComPtr<ShareEventHandler> g_dataRequested;
EventRegistrationToken g_dataToken{};
HWND g_shareOwner=nullptr;

PCWSTR LocalizedShareDescription();
PCWSTR LocalizedShareFallbackTitle();
PCWSTR LocalizedShareFallbackNotice();
PCWSTR LocalizedShareClipboardFailure();

// Minimal COM implementation of the WinRT DataRequested delegate, avoiding an optional WRL callback helper
// not shipped by Windhawk's bundled compiler.
class ShareDataRequestedHandler final : public ShareEventHandler {
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if(!object) return E_POINTER;
        *object=nullptr;
        if(IsEqualIID(iid,IID_IUnknown) || IsEqualIID(iid,kIidShareEventHandler)) {
            *object=static_cast<ShareEventHandler*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return (ULONG)InterlockedIncrement(&refs_); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG remaining=(ULONG)InterlockedDecrement(&refs_);
        if(!remaining) delete this;
        return remaining;
    }
    HRESULT STDMETHODCALLTYPE Invoke(dt::IDataTransferManager*,dt::IDataRequestedEventArgs* args) override {
        try {
        if(!args) return E_POINTER;
        ComPtr<dt::IDataRequest> request;
        HRESULT hr=args->get_Request(&request);
        if(FAILED(hr)) return hr;
        ComPtr<dt::IDataPackage> package;
        hr=request->get_Data(&package);
        if(FAILED(hr)) return hr;
        ComPtr<dt::IDataPackagePropertySet> props;
        hr=package->get_Properties(&props);
        if(FAILED(hr)) return hr;
        std::wstring title=g_shareTitle.empty()?L"Windows desktop":g_shareTitle;
        std::wstring text=g_shareText.empty()?title:g_shareText;
        const std::wstring description=LocalizedShareDescription();
        HStringReference titleRef(title.c_str());
        HStringReference descRef(description.c_str());
        HStringReference textRef(text.c_str());
        hr=props->put_Title(titleRef.Get());
        if(FAILED(hr)) return hr;
        hr=props->put_Description(descRef.Get());
        if(FAILED(hr)) return hr;
        return package->SetText(textRef.Get());
        } catch (...) {
            return E_OUTOFMEMORY;
        }
    }
private:
    volatile LONG refs_=1;
};

int ReadRegDWORD(PCWSTR name, int fallback = -1) {
    ScopedRegKey key;
    DWORD value = 0, type = 0, size = sizeof(value);
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\ImmersiveShell\\EdgeUI",
        0, KEY_QUERY_VALUE, key.put()) != ERROR_SUCCESS) return fallback;
    const LONG rc = RegQueryValueExW(key.get(), name, nullptr, &type, reinterpret_cast<BYTE*>(&value), &size);
    return (rc == ERROR_SUCCESS && type == REG_DWORD) ? (int)value : fallback;
}

int GetUiPrimaryLanguage() noexcept {
    switch(g_uiLanguage.load()) {
        case 1: return LANG_ITALIAN;
        case 2: return LANG_ENGLISH;
        case 3: return LANG_GERMAN;
        case 4: return LANG_JAPANESE;
        case 5: return LANG_FRENCH;
        case 6: return LANG_SPANISH;
        case 7: return LANG_PORTUGUESE;
        case 8: return LANG_RUSSIAN;
        case 9: return LANG_CHINESE;
        case 10: return LANG_KOREAN;
        default: return PRIMARYLANGID(GetUserDefaultUILanguage());
    }
}

// Windows exposes a single accessibility switch for this: Impostazioni > Accessibilità > Schermo >
// "Mostra animazioni in Windows" (SPI_GETCLIENTAREAANIMATION). Windows 8 honoured it for the Charms Bar, so
// the rail and its panes have to honour it as well: with the switch off they appear and disappear instantly
// even when the mod's own "Animate show and hide" setting is on. The value is queried live, so toggling it
// takes effect on the next show or hide without any WM_SETTINGCHANGE plumbing.
bool SystemAnimationsEnabled() {
    BOOL enabled = TRUE;
    if (!SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0)) return true;
    return enabled != FALSE;
}

// Single gate used by every open/close path (rail, Settings pane, Devices pane, Share pane).
bool AnimationsEnabled() {
    return g_useAnimations.load() && SystemAnimationsEnabled();
}

void ReadWindhawkSettings() {
    bool useWinC=true;
    bool edgeActivation=true;
    bool topRightCornerOnly=false;
    ScopedWhString activationMode(Wh_GetStringSetting(L"activationMode"));
    if(activationMode.get()) {
        if(wcscmp(activationMode.get(),L"edgeAndWinC")==0) {
            useWinC=true; edgeActivation=true;
        } else if(wcscmp(activationMode.get(),L"edgeOnly")==0) {
            useWinC=false; edgeActivation=true;
        } else if(wcscmp(activationMode.get(),L"topRightAndWinC")==0) {
            useWinC=true; edgeActivation=true; topRightCornerOnly=true;
        } else if(wcscmp(activationMode.get(),L"topRightOnly")==0) {
            useWinC=false; edgeActivation=true; topRightCornerOnly=true;
        } else if(wcscmp(activationMode.get(),L"winCOnly")==0) {
            useWinC=true; edgeActivation=false;
        }
    }
    g_useWinC.store(useWinC);
    g_edgeActivation.store(edgeActivation);
    g_topRightCornerOnly.store(topRightCornerOnly);
    g_charmsEnabled.store(Wh_GetIntSetting(L"charmsEnabled")!=0);
    g_hideAfterClick.store(Wh_GetIntSetting(L"hideAfterClick")!=0);

    g_charmSlotCount.store(kCharmCount);
    for(int i=0;i<kCharmCount;i++) g_actionMap[i].store(i);

    ScopedWhString uiLanguage(Wh_GetStringSetting(L"uiLanguage"));
    int languageIndex=0;
    if(uiLanguage.get() && wcscmp(uiLanguage.get(),L"it")==0) languageIndex=1;
    else if(uiLanguage.get() && wcscmp(uiLanguage.get(),L"en")==0) languageIndex=2;
    else if(uiLanguage.get() && wcscmp(uiLanguage.get(),L"de")==0) languageIndex=3;
    else if(uiLanguage.get() && wcscmp(uiLanguage.get(),L"ja")==0) languageIndex=4;
    else if(uiLanguage.get() && wcscmp(uiLanguage.get(),L"fr")==0) languageIndex=5;
    else if(uiLanguage.get() && wcscmp(uiLanguage.get(),L"es")==0) languageIndex=6;
    else if(uiLanguage.get() && wcscmp(uiLanguage.get(),L"pt")==0) languageIndex=7;
    else if(uiLanguage.get() && wcscmp(uiLanguage.get(),L"ru")==0) languageIndex=8;
    else if(uiLanguage.get() && wcscmp(uiLanguage.get(),L"zh")==0) languageIndex=9;
    else if(uiLanguage.get() && wcscmp(uiLanguage.get(),L"ko")==0) languageIndex=10;
    g_uiLanguage.store(languageIndex);

    g_useAnimations.store(Wh_GetIntSetting(L"useAnimations")!=0);
    g_showClock.store(Wh_GetIntSetting(L"showClock")!=0);
    g_showQuickMenu.store(false); // The legacy menu is intentionally dormant.
    g_fullHeight.store(true);
    g_closeOnClickOutside.store(Wh_GetIntSetting(L"closeOnClickOutside")!=0);

    ScopedWhString searchHotkey(Wh_GetStringSetting(L"searchHotkey"));
    int searchHotkeyMode=0;
    if(searchHotkey.get() && wcscmp(searchHotkey.get(),L"winS")==0) searchHotkeyMode=1;
    else if(searchHotkey.get() && wcscmp(searchHotkey.get(),L"winQ")==0) searchHotkeyMode=2;
    g_searchHotkey.store(searchHotkeyMode);

    ScopedWhString searchTarget(Wh_GetStringSetting(L"searchTarget"));
    g_searchTarget.store(searchTarget.get() && wcscmp(searchTarget.get(),L"windows")==0 ? 1 : 0);

    ScopedWhString shareBehavior(Wh_GetStringSetting(L"shareBehavior"));
    int shareMode=0;
    if(shareBehavior.get() && wcscmp(shareBehavior.get(),L"winrt")==0) shareMode=1;
    else if(shareBehavior.get() && wcscmp(shareBehavior.get(),L"winH")==0) shareMode=2;
    else if(shareBehavior.get() && wcscmp(shareBehavior.get(),L"email")==0) shareMode=3;
    g_shareBehavior.store(shareMode);

    ScopedWhString settingsMode(Wh_GetStringSetting(L"settingsCharmMode"));
    g_settingsPane.store(!(settingsMode.get() && wcscmp(settingsMode.get(),L"native")==0));
    ScopedWhString paneColor(Wh_GetStringSetting(L"paneColorMode"));
    g_paneColorMode.store(paneColor.get() && wcscmp(paneColor.get(),L"dark")==0 ? 1 : 0);

    // These controls were intentionally removed from the simplified settings UI. Keep the established
    // behavior with safe, predictable defaults.
    g_clockFormat.store(0);       // Windows regional time format
    g_uiScalePercent.store(0);    // Automatic monitor DPI
    g_enableTransparency.store(Wh_GetIntSetting(L"enableTransparency")!=0);
    g_opacity.store(g_enableTransparency.load()?210:255);
    g_openMs.store(300);
    g_closeMs.store(250);
    g_edgeDelay.store(100);

    ScopedWhString startStyle(Wh_GetStringSetting(L"startLogoStyle"));
    int startStyleIndex=0;
    if(startStyle.get() && wcscmp(startStyle.get(),L"build8102")==0) startStyleIndex=1;
    g_startLogoStyle.store(startStyleIndex);

    ScopedWhString charmIconSet(Wh_GetStringSetting(L"charmIconSet"));
    g_charmIconSet.store(charmIconSet.get() && wcscmp(charmIconSet.get(),L"build8102")==0 ? 1 : 0);

    if(!g_charmsEnabled.load() && g_visible.load() && g_threadId)
        PostThreadMessageW(g_threadId,WM_PORT_CLOSE,0,0);
    if(g_rail) {
        SetLayeredWindowAttributes(g_rail,0,GetRailWindowAlpha(g_animStep.load()),LWA_ALPHA);
        InvalidateRect(g_rail,nullptr,TRUE);
    }
    if(g_clock) InvalidateRect(g_clock,nullptr,TRUE);
    if(g_volumeFlyout) InvalidateRect(g_volumeFlyout,nullptr,TRUE);
    if(g_threadId) PostThreadMessageW(g_threadId,WM_PORT_REFRESH,0,0);
}

// Read the system accent using the documented WinRT UISettings.GetColorValue API. DWM colorization is
// retained as a documented fallback on older Windows builds.
bool TryGetUiSettingsAccent(COLORREF* accent) noexcept {
    if(!accent) return false;
    const HRESULT apartment=RoInitialize(RO_INIT_MULTITHREADED);
    if(FAILED(apartment) && apartment!=RPC_E_CHANGED_MODE) return false;

    bool succeeded=false;
    do {
        ScopedHString className;
        constexpr PCWSTR runtimeClass=L"Windows.UI.ViewManagement.UISettings";
        HRESULT hr=WindowsCreateString(runtimeClass,(UINT32)wcslen(runtimeClass),className.put());
        if(FAILED(hr)) break;

        ComPtr<IInspectable> instance;
        hr=RoActivateInstance(className.get(),instance.GetAddressOf());
        if(FAILED(hr)) break;

        ComPtr<IUISettings3AccentLocal> settings;
        hr=instance->QueryInterface(kIidUISettings3Accent, reinterpret_cast<void**>(settings.GetAddressOf()));
        if(FAILED(hr)) break;

        UiSettingsColor color{};
        constexpr INT32 accentType=5; // UIColorType.Accent
        hr=settings->GetColorValue(accentType,&color);
        if(FAILED(hr)) break;
        *accent=RGB(color.R,color.G,color.B);
        succeeded=true;
    } while(false);

    if(SUCCEEDED(apartment)) RoUninitialize();
    return succeeded;
}

void RefreshSystemAccent() {
    COLORREF accent=GetSysColor(COLOR_HIGHLIGHT);
    if(TryGetUiSettingsAccent(&accent)) {
        g_accent.store(accent);
        return;
    }

    DWORD colorization=0;
    BOOL opaque=FALSE;
    if(SUCCEEDED(DwmGetColorizationColor(&colorization,&opaque))) {
        g_accent.store(RGB((colorization>>16)&0xff, (colorization>>8)&0xff,
                           colorization&0xff));
    } else {
        g_accent.store(accent);
    }
}

COLORREF GetRailBackground() {
    return RGB(17,17,17);
}

COLORREF GetRailHighlight(bool pressed) {
    return pressed ? RGB(62,62,62) : RGB(43,43,43);
}

// Relative luminance (WCAG) of an sRGB colour, 0..1.
double RelativeLuminance(double r, double g, double b) {
    auto lin = [](double v) { v /= 255.0; return v <= 0.04045 ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4); };
    return 0.2126 * lin(r) + 0.7152 * lin(g) + 0.0722 * lin(b);
}

COLORREF GetStartLogoColor() {
    return g_accent.load();
}

constexpr int kReferenceDpi = 96;

int DpiScale(int logical) noexcept {
    return MulDiv(logical, (std::max)(kReferenceDpi, g_uiDpi.load()), kReferenceDpi);
}

int DpiUnscale(int physical) noexcept {
    return MulDiv(physical, kReferenceDpi, (std::max)(kReferenceDpi, g_uiDpi.load()));
}

using SetThreadDpiAwarenessContextFn = HANDLE (WINAPI*)(HANDLE);
using GetDpiForWindowFn = UINT (WINAPI*)(HWND);
using GetDpiForMonitorFn = HRESULT (WINAPI*)(HMONITOR, int, UINT*, UINT*);

SetThreadDpiAwarenessContextFn GetSetThreadDpiAwarenessContext() noexcept {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    return user32 ? reinterpret_cast<SetThreadDpiAwarenessContextFn>(
        GetProcAddress(user32, "SetThreadDpiAwarenessContext")) : nullptr;
}

HANDLE EnterBestEffortPerMonitorDpiContext() noexcept {
    SetThreadDpiAwarenessContextFn setContext = GetSetThreadDpiAwarenessContext();
    if (!setContext) return nullptr;
    HANDLE previous = setContext(reinterpret_cast<HANDLE>((LONG_PTR)-4)); // Per-monitor V2.
    if (!previous) previous = setContext(reinterpret_cast<HANDLE>((LONG_PTR)-3)); // Per-monitor V1.
    if (previous) g_threadPerMonitorDpi.store(true);
    return previous;
}

void RestoreThreadDpiContext(HANDLE previous) noexcept {
    if (!previous) return;
    SetThreadDpiAwarenessContextFn setContext = GetSetThreadDpiAwarenessContext();
    if (setContext) setContext(previous);
}

UINT QueryMonitorDpi(HMONITOR monitor) noexcept {
    UINT dpi = 0;
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto getDpiForWindow = user32 ? reinterpret_cast<GetDpiForWindowFn>(
        GetProcAddress(user32, "GetDpiForWindow")) : nullptr;
    if (getDpiForWindow && g_rail && IsWindow(g_rail) &&
        MonitorFromWindow(g_rail, MONITOR_DEFAULTTONEAREST) == monitor) {
        dpi = getDpiForWindow(g_rail);
    }
    // GetDpiForMonitor is appropriate only when this thread has not opted into per-monitor awareness. Under
    // PMv1/PMv2, let WM_DPICHANGED provide the new monitor DPI after the hidden rail is moved to that
    // monitor.
    if (!dpi && monitor && !g_threadPerMonitorDpi.load()) {
        HMODULE shcore = GetModuleHandleW(L"shcore.dll");
        auto getDpiForMonitor = shcore ? reinterpret_cast<GetDpiForMonitorFn>(
            GetProcAddress(shcore, "GetDpiForMonitor")) : nullptr;
        UINT dpiX = 0, dpiY = 0;
        if (getDpiForMonitor && SUCCEEDED(getDpiForMonitor(monitor, 0, &dpiX, &dpiY)))
            dpi = dpiX;
    }
    if (!dpi) {
        HDC screen = GetDC(nullptr);
        if (screen) {
            dpi = (UINT)GetDeviceCaps(screen, LOGPIXELSX);
            ReleaseDC(nullptr, screen);
        }
    }
    return dpi >= 48 && dpi <= 768 ? dpi : kReferenceDpi;
}

bool ConfigureDpiCanvas(HDC dc) noexcept {
    const int dpi = (std::max)(kReferenceDpi, g_uiDpi.load());
    if (!dc || !SetMapMode(dc, MM_ANISOTROPIC)) return false;
    if (!SetWindowExtEx(dc, kReferenceDpi, kReferenceDpi, nullptr) ||
        !SetViewportExtEx(dc, dpi, dpi, nullptr)) {
        SetMapMode(dc, MM_TEXT);
        return false;
    }
    return true;
}

// Where this explorer.exe stands relative to the shell window. Explorer restarts and cold boots inject the
// mod before the new shell window exists, and for a short while GetShellWindow() still returns the window of
// the old, already exiting Explorer. That transient state used to be mistaken for "this is a secondary
// Explorer process", so the mod went idle for the whole restarted process; the three states below keep the
// difference between "not ready yet" (keep waiting) and "another live Explorer owns the shell" (give up).
enum class ShellHostState { Host, OtherLive, NotReady };

ShellHostState QueryShellHostState() {
    const DWORD currentPid=GetCurrentProcessId();
    const HWND shell=GetShellWindow();
    if(!shell || !IsWindow(shell)) return ShellHostState::NotReady;
    DWORD shellPid=0;
    if(!GetWindowThreadProcessId(shell,&shellPid) || !shellPid) return ShellHostState::NotReady;
    if(shellPid==currentPid) return ShellHostState::Host;
    // A live owner means this process will never host the shell in this session. An exited owner is the
    // stale window of a restart in progress, so keep waiting for the new shell window instead of bailing out.
    ScopedHandle shellProcess(OpenProcess(SYNCHRONIZE,FALSE,shellPid));
    if(shellProcess && WaitForSingleObject(shellProcess.get(),0)==WAIT_TIMEOUT)
        return ShellHostState::OtherLive;
    return ShellHostState::NotReady;
}

PCWSTR ShellHostStateName(ShellHostState state) {
    switch(state) {
        case ShellHostState::Host:return L"host";
        case ShellHostState::OtherLive:return L"other-live-explorer";
        default:return L"shell-not-ready";
    }
}

int GetCharmAction(int slot) noexcept {
    if(slot<0 || slot>=kCharmCount) return -1;
    const int action=g_actionMap[slot].load();
    return action>=0 && action<kCharmCount?action:slot;
}

int GetCharmSlotCount() noexcept {
    return (std::max)(1,(std::min)(kCharmCount,g_charmSlotCount.load()));
}

int GetCharmTop(int id, int height) {
    const int contentHeight = GetCharmSlotCount() * kGridCell;
    return (height > contentHeight ? (height - contentHeight) / 2 : 0) + id * kGridCell;
}

// Keep the familiar Windows shell captions localized instead of hard-coding English into the
// screenshot-matched rail. Invalid indices safely fall back.
std::wstring LocalizedCharm(int id) {
    if (id < 0 || id >= kCharmCount) return L"";
    const CharmDef& charm = kCharmDefs[id];
    switch (GetUiPrimaryLanguage()) {
        case LANG_ITALIAN:    return charm.it;
        case LANG_GERMAN:     return charm.de;
        case LANG_JAPANESE:   return charm.ja;
        case LANG_FRENCH:     return charm.fr;
        case LANG_SPANISH:    return charm.es;
        case LANG_PORTUGUESE: return charm.pt;
        case LANG_RUSSIAN:    return charm.ru;
        case LANG_CHINESE:    return charm.zh;
        case LANG_KOREAN:     return charm.ko;
        default:              return charm.en;
    }
}

bool IsImmersiveWindow(HWND hwnd) {
    if (!hwnd) return false;
    wchar_t cls[128]{}; GetClassNameW(hwnd,cls,128);
    return wcscmp(cls,L"Windows.UI.Core.CoreWindow")==0 || wcscmp(cls,L"ApplicationFrameWindow")==0 || wcscmp(cls,L"Windows.UI.Composition.DesktopWindowContentBridge")==0;
}

bool QueryWindowsVersion(DWORD* major,DWORD* minor) {
    using RtlGetVersionFn=LONG (WINAPI*)(OSVERSIONINFOW*);
    HMODULE ntdll=GetModuleHandleW(L"ntdll.dll");
    auto rtl=ntdll?(RtlGetVersionFn)GetProcAddress(ntdll,"RtlGetVersion"):nullptr;
    if(!rtl) return false;
    OSVERSIONINFOW version{}; version.dwOSVersionInfoSize=sizeof(version);
    if(rtl(&version)!=0) return false;
    if(major) *major=version.dwMajorVersion;
    if(minor) *minor=version.dwMinorVersion;
    return true;
}

bool IsWindows10OrLater() {
    DWORD major=10,minor=0;
    return !QueryWindowsVersion(&major,&minor) || major>=10;
}

bool IsWindows8OrLater() {
    DWORD major=10,minor=0;
    if(!QueryWindowsVersion(&major,&minor)) return true;
    return major>6 || (major==6 && minor>=2);
}

void RefreshEdgeUiRegistry() {
    g_hidden[0]=ReadRegDWORD(L"DisableSearchCharm",0)!=0;
    g_hidden[1]=false; // Keep Share visible and usable, independent of the legacy shell flag.
    g_hidden[2]=ReadRegDWORD(L"DisableStartCharm",0)!=0;
    g_hidden[3]=ReadRegDWORD(L"DisableDevicesCharm",0)!=0;
    g_hidden[4]=ReadRegDWORD(L"DisableSettingsCharm",0)!=0;
    g_disableStartFade.store(ReadRegDWORD(L"DisableStartFade", 0) == 1);
    g_disableTRCorner.store(ReadRegDWORD(L"DisableTRCorner", 0) != 0);
    g_disableBRCorner.store(ReadRegDWORD(L"DisableBRCorner", 0) != 0);
    g_disableCharmsHint.store(ReadRegDWORD(L"DisableCharmsHint", 0) != 0);
    g_registryQuickMenu.store(false); // Disabled for now; legacy menu renderer remains available in source.
    int desktopDelay=ReadRegDWORD(L"CharmsBarDesktopDelay",g_edgeDelay.load());
    int immersiveDelay=ReadRegDWORD(L"CharmsBarImmersiveDelay",g_edgeDelay.load());
    if(desktopDelay<0||desktopDelay>2000)desktopDelay=g_edgeDelay.load();
    if(immersiveDelay<0||immersiveDelay>2000)immersiveDelay=g_edgeDelay.load();
    g_desktopEdgeDelay.store(desktopDelay);
    g_immersiveEdgeDelay.store(immersiveDelay);
}

void DrawTextBox(HDC dc, PCWSTR text, RECT r, int size, COLORREF color,
                 int weight=FW_NORMAL, UINT flags=DT_LEFT|DT_VCENTER|DT_SINGLELINE,
                 PCWSTR face=L"Segoe UI", bool fitWidth=false) {
    if (!dc || !text || size <= 0) return;
    int drawSize=size;
    if(fitWidth) {
        bool fitFound=false;
        const int textLength=lstrlenW(text);
        for(int candidate=size;candidate>=48;candidate-=2) {
            ScopedGdiObject measureFont(CreateFontW(-candidate,0,0,0,weight,FALSE,FALSE,FALSE,
                DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
                DEFAULT_PITCH|FF_DONTCARE,face));
            if(!measureFont) break;
            ScopedSelectObject measureSelected(dc,measureFont.get());
            if(!measureSelected) break;
            SIZE extent{};
            if(GetTextExtentPoint32W(dc,text,textLength,&extent) && extent.cx<=r.right-r.left) {
                drawSize=candidate;
                fitFound=true;
                break;
            }
        }
        if(!fitFound) drawSize=48;
    }
    ScopedGdiObject font(CreateFontW(-drawSize,0,0,0,weight,FALSE,FALSE,FALSE,
        DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE,face));
    if (!font) return;
    ScopedSelectObject selected(dc, font.get());
    if (!selected) return;
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextW(dc, text, -1, &r, flags);
}

void CaptureForeground() {
    HWND active=GetForegroundWindow();
    if(active && active!=g_rail && active!=g_clock && active!=g_menu) {
        wchar_t title[512]{}; GetWindowTextW(active,title,512);
        try {
            g_shareTitle = title[0] ? title : L"Windows desktop";
            g_shareText = g_shareTitle;
        } catch (...) {
            g_shareTitle.clear();
            g_shareText.clear();
        }
    }
}

void PositionWindowsOnCurrentMonitor() {
    if(g_monitorRect.right<=g_monitorRect.left || g_monitorRect.bottom<=g_monitorRect.top) return;
    const int railTop=g_fullHeight?g_monitorRect.top:g_workRect.top;
    const int railHeight=g_fullHeight?(g_monitorRect.bottom-g_monitorRect.top):
                                      (g_workRect.bottom-g_workRect.top);
    const int railWidth=DpiScale(kRailWidth);
    const int targetX=g_monitorRect.right-railWidth+DpiScale(g_slideOffset.load());
    if(g_rail) SetWindowPos(g_rail,HWND_TOPMOST,targetX,railTop,railWidth,railHeight,SWP_NOACTIVATE);
    if(g_clock) SetWindowPos(g_clock,HWND_TOPMOST,
        g_monitorRect.left+DpiScale(51),g_monitorRect.bottom-DpiScale(188),
        DpiScale(kClockWidth),DpiScale(kClockHeight),SWP_NOACTIVATE);
    if(g_menu) SetWindowPos(g_menu,HWND_TOPMOST,g_monitorRect.left,
        g_monitorRect.bottom-DpiScale(250),DpiScale(300),DpiScale(220),SWP_NOACTIVATE);
    RepositionVisibleVolumeFlyout();
}

void PlaceWindows(POINT pt) {
    HMONITOR mon=MonitorFromPoint(pt,MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if(!mon || !GetMonitorInfoW(mon,&mi)) {
        mon=MonitorFromWindow(GetDesktopWindow(),MONITOR_DEFAULTTOPRIMARY);
        mi={sizeof(mi)};
        if(!mon || !GetMonitorInfoW(mon,&mi)) return;
    }
    UINT dpi=QueryMonitorDpi(mon);
    const int scalePercent=g_uiScalePercent.load();
    if(scalePercent>0) dpi=(UINT)MulDiv(kReferenceDpi,scalePercent,100);
    g_uiDpi.store((std::max)(kReferenceDpi,(int)dpi));
    g_monitor=mon; g_monitorRect=mi.rcMonitor; g_workRect=mi.rcWork;
    PositionWindowsOnCurrentMonitor();
}

void HandleDpiChanged(HWND hwnd,WPARAM wParam) {
    const UINT suggestedDpi=LOWORD(wParam);
    const int scalePercent=g_uiScalePercent.load();
    const UINT dpi=scalePercent>0?(UINT)MulDiv(kReferenceDpi,scalePercent,100):
                   (suggestedDpi>=48&&suggestedDpi<=768?suggestedDpi:kReferenceDpi);
    g_uiDpi.store((std::max)(kReferenceDpi,(int)dpi));
    HMONITOR monitor=MonitorFromWindow(hwnd,MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if(monitor && GetMonitorInfoW(monitor,&mi)) {
        g_monitor=monitor; g_monitorRect=mi.rcMonitor; g_workRect=mi.rcWork;
    }
    PositionWindowsOnCurrentMonitor();
    if(g_rail) InvalidateRect(g_rail,nullptr,TRUE);
    if(g_clock) InvalidateRect(g_clock,nullptr,TRUE);
    if(g_menu) InvalidateRect(g_menu,nullptr,TRUE);
}

void UpdateStatus();

void ApplyRailFrame(int alpha, int slideOffset) {
    alpha = (std::max)(0, (std::min)(255, alpha));
    slideOffset = (std::max)(0, (std::min)(kRailWidth, slideOffset));
    g_animStep.store(alpha);
    g_slideOffset.store(slideOffset);
    if (g_rail) {
        SetLayeredWindowAttributes(g_rail, 0, GetRailWindowAlpha(alpha), LWA_ALPHA);
        const int railTop = g_fullHeight ? g_monitorRect.top : g_workRect.top;
        SetWindowPos(g_rail, nullptr,
            g_monitorRect.right - DpiScale(kRailWidth) + DpiScale(slideOffset), railTop, 0, 0,
            SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
    }
    if (g_clock) SetLayeredWindowAttributes(g_clock, 0, (BYTE)alpha, LWA_ALPHA);
}

void StartRailAnimation(bool opening) {
    g_animFromAlpha = g_animStep.load();
    g_animFromSlide = g_slideOffset.load();
    const int targetAlpha = opening ? 255 : 0;
    g_animTarget.store(targetAlpha);
    g_animToSlide = opening ? 0 : kRailWidth;
    const int fullDuration = opening ? g_openMs.load() : g_closeMs.load();
    const double alphaFraction = (double)std::abs(targetAlpha - g_animFromAlpha) / 255.0;
    const double slideFraction = (double)std::abs(g_animToSlide - g_animFromSlide) / kRailWidth;
    const double distanceFraction = (std::max)(alphaFraction, slideFraction);
    if (distanceFraction < 0.001) {
        ApplyRailFrame(targetAlpha, g_animToSlide);
        if (!opening) {
            if (g_clock) ShowWindow(g_clock, SW_HIDE);
            ShowWindow(g_rail, SW_HIDE);
        }
        KillTimer(g_rail, TIMER_ANIMATION);
        return;
    }
    g_animDurationMs = (std::max)(80, (int)(fullDuration * distanceFraction));

    LARGE_INTEGER now{}, frequency{};
    QueryPerformanceCounter(&now);
    QueryPerformanceFrequency(&frequency);
    g_animStartQpc = now.QuadPart;
    g_animQpcFrequency = frequency.QuadPart;
    if (g_animDurationMs <= 0 || g_animQpcFrequency <= 0) {
        ApplyRailFrame(opening ? 255 : 0, g_animToSlide);
        if (!opening) {
            if (g_clock) ShowWindow(g_clock, SW_HIDE);
            if (g_rail) ShowWindow(g_rail, SW_HIDE);
        }
        KillTimer(g_rail, TIMER_ANIMATION);
        return;
    }
    if (!SetTimer(g_rail, TIMER_ANIMATION, 8, nullptr)) {
        Wh_Log(L"animation timer failed (%lu); applying final frame", GetLastError());
        ApplyRailFrame(opening ? 255 : 0, g_animToSlide);
        if (!opening) {
            if (g_clock) ShowWindow(g_clock, SW_HIDE);
            ShowWindow(g_rail, SW_HIDE);
        }
    }
}

void ClosePaneWindow(bool immediate);

// Defined with the companion Start Screen helpers further down; the rail calls them on every show and
// from its 1 s status timer.
static bool CompanionStartScreenVisible();
static void RaiseCharmsOverCompanionStartScreen();

void ShowCharms(POINT pt, bool byKeyboard) {
    if (!g_rail || !g_charmsEnabled.load()) return;
    if(g_paneVisible.load() || (g_pane && IsWindowVisible(g_pane))) {
        if(!byKeyboard) return;
        ClosePaneWindow(true);
    }
    const bool alreadyVisible = IsWindowVisible(g_rail) != FALSE;
    if (!alreadyVisible) {
        const bool animate = AnimationsEnabled() && g_openMs.load() > 0;
        g_animStep.store(animate ? 0 : 255);
        g_slideOffset.store(animate ? kRailWidth : 0);
    }
    CaptureForeground();
    PlaceWindows(pt);
    RefreshEdgeUiRegistry();
    g_keyboardOpened = byKeyboard;
    const int slotCount=GetCharmSlotCount();
    int startSlot=(std::min)(2,slotCount-1);
    g_keyboardSelection.store(startSlot);
    if(byKeyboard && g_hidden[GetCharmAction(startSlot)]) {
        for(int step=1;step<=slotCount;step++) {
            const int candidate=(startSlot+step)%slotCount;
            if(!g_hidden[GetCharmAction(candidate)]) { g_keyboardSelection.store(candidate); break; }
        }
    }
    g_hover = -1;
    g_pressed = -1;
    g_visible = true;
    g_noFocusDismissUntil.store(0);
    g_openTick.store(GetTickCount());
    if (g_showClock && ReadRegDWORD(L"DisableCharmsClock") != 1 && g_clock) {
        UpdateStatus();
        SetLayeredWindowAttributes(g_clock, 0, (BYTE)g_animStep.load(), LWA_ALPHA);
        ShowWindow(g_clock, SW_SHOWNOACTIVATE);
    } else if (g_clock) {
        ShowWindow(g_clock, SW_HIDE);
    }
    SetLayeredWindowAttributes(g_rail, 0, GetRailWindowAlpha(g_animStep.load()), LWA_ALPHA);
    ShowWindow(g_rail, SW_SHOWNOACTIVATE);
    RaiseCharmsOverCompanionStartScreen();
    if (g_rail) SetTimer(g_rail, TIMER_STATUS, 1000, nullptr);

    if (AnimationsEnabled() && g_openMs.load() > 0) {
        StartRailAnimation(true);
    } else {
        KillTimer(g_rail, TIMER_ANIMATION);
        ApplyRailFrame(255, 0);
    }
    InvalidateRect(g_rail, nullptr, TRUE);
    static bool firstShowLogged = false;
    if (!firstShowLogged) {
        firstShowLogged = true;
        Wh_Log(L"Charms Bar shown for the first time in this process (pid %lu)",
               GetCurrentProcessId());
    }
}

void HideCharms(bool immediate = false) {
    if(!g_rail) return;
    const bool wasVisible=g_visible.exchange(false);
    if(!wasVisible && !immediate) return;
    g_noFocusDismissUntil.store(0);
    g_keyboardOpened = false;
    g_hover = -1;
    g_pressed = -1;
    HideMetroContextMenu(false);

    if (immediate || !wasVisible || !AnimationsEnabled() || g_closeMs.load() == 0) {
        KillTimer(g_rail, TIMER_ANIMATION);
        KillTimer(g_rail, TIMER_STATUS);
        ApplyRailFrame(0, kRailWidth);
        if (g_clock) ShowWindow(g_clock, SW_HIDE);
        ShowWindow(g_rail, SW_HIDE);
        return;
    }
    StartRailAnimation(false);
}

void CompleteCharmAction() {
    if(g_hideAfterClick.load()) {
        HideCharms(true);
    } else {
        // Suppress only the activation event caused by the chosen command; explicit outside clicks remain
        // dismissive through the mouse hook.
        g_noFocusDismissUntil.store(GetTickCount()+2500);
    }
}

void InjectKeyUp(WORD vk) {
    INPUT release{};
    release.type = INPUT_KEYBOARD;
    release.ki.wVk = vk;
    release.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &release, sizeof(release));
}

bool SendWinChord(WORD vk) {
    INPUT inputs[4]{};
    for (auto& input : inputs) input.type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_LWIN;
    inputs[1].ki.wVk = vk;
    inputs[2].ki.wVk = vk; inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].ki.wVk = VK_LWIN; inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    const UINT sent = SendInput((UINT)_countof(inputs), inputs, sizeof(INPUT));
    if (sent != _countof(inputs)) {
        Wh_Log(L"incomplete Win chord injection (%u/4, error %lu)", sent, GetLastError());
        if (sent >= 2 && sent < 3) InjectKeyUp(vk);
        if (sent >= 1 && sent < 4) InjectKeyUp(VK_LWIN);
        return false;
    }
    return true;
}

void SendWinTap() {
    INPUT inputs[2]{};
    inputs[0].type = inputs[1].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = inputs[1].ki.wVk = VK_LWIN;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    const UINT sent = SendInput((UINT)_countof(inputs), inputs, sizeof(INPUT));
    if (sent == 1) InjectKeyUp(VK_LWIN);
}


HRESULT SetupShareManager(HWND shareWindow) {
    if(!shareWindow || !IsWindow(shareWindow)) return E_HANDLE;
    try {
        if(g_shareReady.load() && g_shareOwner==shareWindow &&
           g_dtm && g_dtmInterop && g_dataRequested) return S_OK;
        if(g_dtm && g_dataRequested) {
            const HRESULT removeResult=g_dtm->remove_DataRequested(g_dataToken);
            if(FAILED(removeResult))
                Wh_Log(L"removing previous Share event handler failed (0x%08X)",
                       (unsigned)removeResult);
        }
        g_shareReady=false;
        g_shareOwner=nullptr;
        g_dataRequested.Reset();
        g_dtm.Reset();
        g_dtmInterop.Reset();
        // The UI thread initialized the Windows Runtime before creating its HWNDs.
        HRESULT hr=S_OK;
        ScopedHString className;
        constexpr PCWSTR runtimeClass=L"Windows.ApplicationModel.DataTransfer.DataTransferManager";
        hr=WindowsCreateString(runtimeClass,(UINT32)wcslen(runtimeClass),className.put());
        if(FAILED(hr)) return hr;
        hr=RoGetActivationFactory(className.get(),kIidDataTransferManagerInterop,
                                  (void**)g_dtmInterop.GetAddressOf());
        if(FAILED(hr)) return hr;
        static const GUID iidDtm={0xa5caee9b,0x8708,0x49d1,{0x8d,0x36,0x67,0xd2,0x5a,0x8d,0xa0,0x0c}};
        hr=g_dtmInterop->GetForWindow(shareWindow,iidDtm,(void**)g_dtm.GetAddressOf());
        if(FAILED(hr)) return hr;

        ComPtr<ShareDataRequestedHandler> handler;
        handler.Attach(new(std::nothrow) ShareDataRequestedHandler());
        if(!handler) return E_OUTOFMEMORY;
        hr=handler->QueryInterface(kIidShareEventHandler, (void**)g_dataRequested.GetAddressOf());
        if(FAILED(hr)) return hr;
        hr=g_dtm->add_DataRequested(g_dataRequested.Get(),&g_dataToken);
        if(SUCCEEDED(hr)) {
            g_shareOwner=shareWindow;
            g_shareReady=true;
        } else {
            g_dataRequested.Reset(); g_dtm.Reset(); g_dtmInterop.Reset();
            g_shareOwner=nullptr;
        }
        return hr;
    } catch (...) {
        Wh_Log(L"C++ exception while preparing Share UI");
        g_shareReady=false;
        g_shareOwner=nullptr;
        g_dataRequested.Reset();
        g_dtm.Reset();
        g_dtmInterop.Reset();
        return E_OUTOFMEMORY;
    }
}

std::wstring EncodeMailComponent(PCWSTR text) {
    if(!text) return {};
    const int chars=(std::min)(lstrlenW(text),2048);
    if(chars<=0) return {};
    const int byteCount=WideCharToMultiByte(CP_UTF8,0,text,chars,nullptr,0,nullptr,nullptr);
    if(byteCount<=0) return {};
    std::vector<char> utf8((size_t)byteCount);
    if(!WideCharToMultiByte(CP_UTF8,0,text,chars,utf8.data(),byteCount,nullptr,nullptr)) return {};
    static constexpr wchar_t hex[]=L"0123456789ABCDEF";
    std::wstring encoded;
    encoded.reserve((size_t)byteCount*3);
    for(unsigned char ch:utf8) {
        const bool safe=(ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||
                        (ch>='0'&&ch<='9')||ch=='-'||ch=='_'||ch=='.'||ch=='~';
        if(safe) encoded.push_back((wchar_t)ch);
        else { encoded.push_back(L'%'); encoded.push_back(hex[ch>>4]); encoded.push_back(hex[ch&15]); }
    }
    return encoded;
}

bool CopyShareTextToClipboard(PCWSTR text) noexcept {
    if(!text) return false;
    const SIZE_T bytes=((SIZE_T)lstrlenW(text)+1)*sizeof(wchar_t);
    HGLOBAL memory=GlobalAlloc(GMEM_MOVEABLE,bytes);
    if(!memory) return false;
    if(!OpenClipboard(nullptr)) { GlobalFree(memory); return false; }
    bool copied=false;
    EmptyClipboard();
    void* destination=GlobalLock(memory);
    if(destination) {
        memcpy(destination,text,bytes);
        GlobalUnlock(memory);
        if(SetClipboardData(CF_UNICODETEXT,memory)) { copied=true; memory=nullptr; }
    }
    if(memory) GlobalFree(memory);
    CloseClipboard();
    return copied;
}

void OpenMailFallback() noexcept {
    try {
        const std::wstring title=g_shareTitle.empty()?L"Windows desktop":g_shareTitle;
        const std::wstring text=g_shareText.empty()?title:g_shareText;
        const std::wstring uri=L"mailto:?subject="+EncodeMailComponent(title.c_str())+
                               L"&body="+EncodeMailComponent(text.c_str());
        const HINSTANCE launched=ShellExecuteW(nullptr,L"open",uri.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
        if((INT_PTR)launched>32) return;
        Wh_Log(L"mail fallback failed (ShellExecute code %ld)",(LONG)(INT_PTR)launched);
        const bool copied=CopyShareTextToClipboard(text.c_str());
        const PCWSTR notice=copied?LocalizedShareFallbackNotice():LocalizedShareClipboardFailure();
        MessageBoxW(nullptr,notice,LocalizedShareFallbackTitle(), MB_OK|MB_ICONINFORMATION|MB_TOPMOST);
    } catch (...) {
        Wh_Log(L"exception in mail/clipboard Share fallback");
    }
}

void LaunchShare(HWND requestedOwner=nullptr) {
    HWND shareWindow=requestedOwner?requestedOwner:g_rail;
    const bool fromPane=requestedOwner && requestedOwner==g_pane && g_paneVisible.load();
    try {
        const int mode=g_shareBehavior.load();
        const bool nativeWinH=IsWindows8OrLater() && !IsWindows10OrLater();
        const bool useWinH=nativeWinH && (mode==2 || mode==0);
        if(mode==3) {
            if(fromPane) ClosePaneWindow(true);
            CompleteCharmAction();
            OpenMailFallback();
            return;
        }
        if(useWinH) {
            if(fromPane) ClosePaneWindow(true);
            CompleteCharmAction();
            if(IsWindows8OrLater()) SendWinChord('H');
            else OpenMailFallback();
            return;
        }

        // Use the active pane HWND when invoked from the recreated Share surface; DataTransferManager must be
        // associated with the window that owns the UI.
        HRESULT hr=SetupShareManager(shareWindow);
        if(SUCCEEDED(hr)) {
            if(g_dtmInterop) hr=g_dtmInterop->ShowShareUIForWindow(shareWindow);
            else hr=E_NOINTERFACE;
        }
        CompleteCharmAction();
        if(fromPane) ClosePaneWindow(FAILED(hr));
        if(FAILED(hr)) {
            Wh_Log(L"native Share UI unavailable (0x%08X); using fallback",
                   (unsigned)hr);
            OpenMailFallback();
        }
    } catch (...) {
        Wh_Log(L"exception while launching Share UI");
        if(fromPane) ClosePaneWindow(true);
        CompleteCharmAction();
        OpenMailFallback();
    }
}

bool ShowSettingsPane();
bool ShowDevicesPane();
bool ShowSharePane();

// Open Shell replaces the Start menu, and it does not always react to a synthetic Win tap. When its
// StartMenu.exe is installed, ask it to open the menu directly. Returns false if it could not be started.
static bool OpenOpenShellStartMenu() {
    wchar_t programFiles[MAX_PATH]{};
    if (!GetEnvironmentVariableW(L"ProgramFiles", programFiles, _countof(programFiles))) return false;
    const std::wstring exe = std::wstring(programFiles) + L"\\Open-Shell\\StartMenu.exe";
    if (GetFileAttributesW(exe.c_str()) == INVALID_FILE_ATTRIBUTES) return false;
    const HINSTANCE launched = ShellExecuteW(nullptr, L"open", exe.c_str(), L"-open", nullptr, SW_SHOWNORMAL);
    return reinterpret_cast<INT_PTR>(launched) > 32;
}

// Windows 10/11 toggle the search UI on Win+S/Win+Q: sending the chord while search is already open closes
// it. That made every second click of the Search charm look dead (open, close, open, close...). Look for a
// visible top-level window owned by the search host process so the charm can be made idempotent.
static bool IsSearchHostProcess(DWORD processId) {
    if(!processId) return false;
    ScopedHandle process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,processId));
    if(!process) return false;
    wchar_t path[MAX_PATH]{};
    DWORD size=_countof(path);
    if(!QueryFullProcessImageNameW(process.get(),0,path,&size)) return false;
    PCWSTR name=wcsrchr(path,L'\\');
    name=name?name+1:path;
    return _wcsicmp(name,L"SearchApp.exe")==0 ||
           _wcsicmp(name,L"SearchHost.exe")==0 ||
           _wcsicmp(name,L"SearchUI.exe")==0;
}

// A dismissed UWP/Search surface is not hidden: DWM cloaks it, so IsWindowVisible stays TRUE forever and the
// window keeps being reported after search is closed. DWMWA_CLOAKED is the state that actually tracks whether
// the surface is on screen; this is what made search open only twice and then never again.
static bool IsWindowCloaked(HWND hwnd) {
    if(!hwnd) return true;
    constexpr DWORD kDwmwaCloaked=14; // DWMWA_CLOAKED; declared locally so older SDK headers still build
    DWORD cloaked=0;
    if(SUCCEEDED(DwmGetWindowAttribute(hwnd,kDwmwaCloaked,&cloaked,sizeof(cloaked))))
        return cloaked!=0;
    return false;
}

static bool IsSearchWindowOnScreen(HWND hwnd) {
    if(!hwnd || !IsWindow(hwnd)) return false;
    if(!IsWindowVisible(hwnd) || IsIconic(hwnd)) return false;
    if(IsWindowCloaked(hwnd)) return false; // present in the window list, but dismissed
    RECT rect{};
    if(!GetWindowRect(hwnd,&rect)) return false;
    return rect.right>rect.left && rect.bottom>rect.top;
}

static BOOL CALLBACK FindSearchWindowProc(HWND hwnd,LPARAM lParam) {
    if(!IsSearchWindowOnScreen(hwnd)) return TRUE;
    DWORD processId=0;
    GetWindowThreadProcessId(hwnd,&processId);
    if(!IsSearchHostProcess(processId)) return TRUE;
    *(HWND*)lParam=hwnd;
    return FALSE; // first on-screen search UI surface wins
}

static HWND FindWindowsSearchWindow() {
    // Prefer the foreground window when it belongs to the search host: the rail is a no-activate overlay, so
    // a search UI that is really open keeps the focus and is reported here before the enumeration below.
    const HWND foreground=GetForegroundWindow();
    DWORD foregroundPid=0;
    if(foreground) GetWindowThreadProcessId(foreground,&foregroundPid);
    if(IsSearchHostProcess(foregroundPid) && IsSearchWindowOnScreen(foreground)) return foreground;
    HWND found=nullptr;
    EnumWindows(FindSearchWindowProc,(LPARAM)&found);
    return found;
}

// ---------------------------------------------------------------- companion Search Charm mod
// The Windows 8/8.1 Search Charm Recreation mod (win8-search-charm-recreation) hosts a real Windows 8
// search panel in a window of class "Win81SearchCharmMod" and opens or closes it on WM_APP + 1. Posting that
// message is a plain inter-mod hand-off inside the same Explorer process: no shared code, no keystrokes, and
// no way to desync, because the panel's own window tells us whether it is already open. It is also the only
// correct target when that mod is installed: its keyboard hook swallows Win+S/Win+Q before Windows or this mod
// ever see them, so an injected chord would be either lost or turn into a second toggle that closes the panel.
static constexpr PCWSTR kCompanionSearchClasses[] = {
    L"Win81SearchCharmMod", // current class (win8-search-charm-recreation)
    L"Win8SearchCharmMod",  // short form without the 8.1 marker
};
static constexpr UINT kCompanionSearchToggle = WM_APP + 1;

static HWND FindCompanionSearchPanel() {
    for(const PCWSTR className : kCompanionSearchClasses) {
        const HWND panel=FindWindowW(className,nullptr);
        if(panel && IsWindow(panel)) return panel;
    }
    return nullptr;
}

// The companion panel is created once and hidden, not destroyed, when it closes, so IsWindowVisible is the
// authoritative open/closed state. It is a normal layered window, never DWM-cloaked, unlike UWP surfaces.
static bool CompanionSearchPanelOpen(HWND panel) {
    return panel && IsWindowVisible(panel)!=FALSE && !IsIconic(panel);
}

// Returns true when the companion mod handled the request, in which case no chord must be sent.
static bool TryOpenCompanionSearchPanel(const wchar_t* caller) {
    if(g_searchTarget.load()!=0) return false; // "Windows search" selected: never use the companion panel
    const HWND panel=FindCompanionSearchPanel();
    if(!panel) return false;
    if(CompanionSearchPanelOpen(panel)) {
        Wh_Log(L"%s: the Windows 8 Search Charm panel is already open (hwnd 0x%p); leaving it open "
               L"instead of toggling it closed",caller,(void*)panel);
        return true;
    }
    if(!PostMessageW(panel,kCompanionSearchToggle,0,0)) {
        Wh_Log(L"%s: posting WM_APP_TOGGLE to the Windows 8 Search Charm panel failed (%lu); "
               L"falling back to the Windows shortcut",caller,GetLastError());
        return false;
    }
    Wh_Log(L"%s: opened the Windows 8/8.1 Search Charm Recreation panel (hwnd 0x%p, WM_APP+1)",
           caller,(void*)panel);
    return true;
}

// ---------------------------------------------------------------- companion Start Screen mod
// The "Windows 8 (8045) Start Screen" mod (win8-start-8045) owns the full-screen Start screen in a window of
// class "Win8StartScreenMod8045" and toggles it on WM_APP + 1 (WM_APP + 2 only closes it). When that mod is
// installed the Start charm hands the request over instead of tapping the Windows key: tapping would run
// through that mod's own Win-key hook and its 250 ms toggle debounce, so the key press could be swallowed or
// toggle the screen shut again. The window is created hidden and shown while the screen is up, so
// IsWindowVisible is a reliable open/closed test - except during the ~600 ms closing slide, where the screen
// is on its way out and the charm deliberately leaves it alone.
// Discovery is deliberately a list rather than one hard-coded name: a rename in that mod is the most likely
// change to break the hand-off, and trying the old and the short form costs one FindWindowW each.
static constexpr PCWSTR kCompanionStartClasses[] = {
    L"Win8StartScreenMod8045", // current class (win8-start-8045)
    L"Win8StartScreenMod",     // short form, in case the build number is ever dropped from the name
    L"Win8StartScreen8045",    // word order variant
};
static constexpr UINT kCompanionStartToggle = WM_APP + 1; // WM_APP+2 closes without toggling
static constexpr UINT kCompanionStartClose = WM_APP + 2;

// Optional opt-ins from the other side. A surface that sets this property and answers the registered
// message gets the idempotent open path; anything else keeps the plain WM_APP+1 toggle.
static constexpr PCWSTR kCompanionInteropProperty = L"Win8Charms.InteropVersion";
static constexpr int kCompanionInteropMessageVersion = 2;

static UINT CompanionOpenMessage() {
    static const UINT message=RegisterWindowMessageW(L"Windhawk.Win8Charms.OpenSurface");
    return message;
}

static int CompanionInteropVersion(HWND panel) {
    if(!panel) return 0;
    return (int)(INT_PTR)GetPropW(panel,kCompanionInteropProperty);
}

static HWND FindCompanionStartScreen() {
    for(const PCWSTR className : kCompanionStartClasses) {
        const HWND screen=FindWindowW(className,nullptr);
        if(screen && IsWindow(screen)) return screen;
    }
    return nullptr;
}

static bool CompanionStartScreenOpen(HWND screen) {
    return screen && IsWindowVisible(screen)!=FALSE;
}

// Returns true when the Start Screen mod handled the request, so the caller must not tap the Windows key.
static bool TryOpenCompanionStartScreen(const wchar_t* caller) {
    const HWND screen=FindCompanionStartScreen();
    if(!screen) return false;
    if(CompanionStartScreenOpen(screen)) {
        Wh_Log(L"%s: the Windows 8 Start Screen is already open (hwnd 0x%p); leaving it as it is",
               caller,(void*)screen);
        return true;
    }
    // A previous request that produced nothing parks the hand-off for a few minutes: the window still exists
    // but it ignored the message, which is what a changed protocol looks like from here. Falling back to the
    // Windows key is then better than clicking into the void.
    const DWORD retryAfter=g_companionStartRetryAfter.load();
    if(retryAfter && (LONG)(retryAfter-GetTickCount())>0) {
        Wh_Log(L"%s: the Windows 8 Start Screen did not answer the last request; using the Windows "
               L"key for now (fallback clears in %lu s)",caller,(unsigned)((retryAfter-GetTickCount())/1000));
        return false;
    }
    const UINT message=CompanionOpenMessage();
    const int version=CompanionInteropVersion(screen);
    const bool idempotent=message && version>=kCompanionInteropMessageVersion;
    const UINT posted=idempotent?message:kCompanionStartToggle;
    if(!PostMessageW(screen,posted,0,0)) {
        Wh_Log(L"%s: posting to the Windows 8 Start Screen failed (%lu); "
               L"falling back to the Windows key",caller,GetLastError());
        return false;
    }
    Wh_Log(L"%s: opened the Windows 8 Start Screen (hwnd 0x%p, %s, interop v%d)",caller,(void*)screen,
           idempotent?L"registered open message":L"WM_APP+1",version);
    if(g_rail) {
        KillTimer(g_rail,TIMER_START_VERIFY);
        if(!SetTimer(g_rail,TIMER_START_VERIFY,1500,nullptr))
            Wh_Log(L"could not arm the Start screen verification timer (%lu)",GetLastError());
    }
    return true;
}

// The Start Screen mod ignores a toggle that lands within 250 ms of its own Win-key toggle, so a press that
// arrives right after the Windows key can be dropped. Never retry (a second toggle would close the screen);
// just say so in the log, which makes that case obvious instead of looking like a dead click.
static void VerifyCompanionStartScreen() {
    const HWND screen=FindCompanionStartScreen();
    if(screen && CompanionStartScreenOpen(screen)) return;
    // Nothing appeared. Do not send more messages to a surface that is not answering: park the hand-off so the
    // next presses use the Windows key, and say why.
    g_companionStartRetryAfter.store(GetTickCount()+5*60*1000);
    Wh_Log(L"the Windows 8 Start Screen did not appear after the open request; falling back to the "
           L"Windows key for 5 minutes (a swallowed 250 ms toggle debounce also looks like this)");
}

// True when the companion Start Screen is on screen right now, i.e. when our surfaces may be covered by it.
static bool CompanionStartScreenVisible() {
    return CompanionStartScreenOpen(FindCompanionStartScreen());
}

// Brings the rail and the clock back over the full-screen Start screen. Both windows live in the same z-band
// as that screen, so re-raising is enough; it is repeated from the 1 s status timer because the Start Screen
// mod raises its own window every time it opens.
static void RaiseCharmsOverCompanionStartScreen() {
    if(!CompanionStartScreenVisible()) return;
    RaiseCharmsSurface(g_rail);
    if(g_clock && IsWindowVisible(g_clock)) RaiseCharmsSurface(g_clock);
    static bool logged=false;
    if(!logged) {
        logged=true;
        Wh_Log(L"Charms Bar raised above the Windows 8 (8045) Start Screen (same z-band, re-raised)");
    }
}

// ---------------------------------------------------------------- Windows 8.1 Start tip
// Windows 8.1 replaced the Windows 8 "desktop thumbnail" in the corner with the Start tip: the familiar
// Windows logo in a black square that appears whenever the pointer goes to the bottom-left corner of the
// Start screen, and clicking it leaves the Start screen for whatever is underneath - normally the desktop.
// The square is drawn the way the classic-shell skinning notes describe the original: a black box with the
// flag cut out of it, the cut-out filled with the Start screen accent colour, plus a gloss that sweeps
// across the flag while it is hovered. Microsoft's own wording for the change was that the Start "tip" is
// now "the familiar Windows logo"; it is not a desktop preview, which is what Windows 8.0 had.
//
// Two deliberate differences from the original: the tip is armed along the whole left edge as well, since
// that is the gesture this mod offers (Windows 8.1 reacted in the corner only), and it stays up while the
// pointer is on the square itself so that it can actually be clicked. Everything else follows the original:
// same corner, same black square, same accent flag, same short fade-in and hover gloss; both animations are
// skipped when "Show animations in Windows" is off, and the Build 8102 beta logo is drawn as a still image
// with no effects at all, because that is how the beta's corner logo behaved.
static HWND CompanionStartScreenCached();
static HMONITOR CompanionStartScreenMonitor();
static bool CompanionStartScreenVisibleFast();
static bool DrawStartGlyphWithBrush(Gdiplus::Graphics& g, Gdiplus::Brush* brush);
static bool Draw8102StartGlyph(HDC dc,int x,int y,int width,int height);
bool DrawCharmIconGdiplus(HDC dc,int id,int x,int y,int width,int height,COLORREF color);
void DrawFallbackCharmIcon(HDC dc,int id,int x,int y,int width,int height);
COLORREF BlendColor(COLORREF from,COLORREF to,int percent);

// Bottom-left corner square, counted separately because the corner gets the shorter dwell.
static bool InStartTipCornerBox(const RECT& monitor, POINT p) {
    const int corner=DpiScale(kStartTipCornerSize);
    return p.x>=monitor.left && p.x<monitor.left+corner &&
           p.y>=monitor.bottom-corner && p.y<monitor.bottom;
}

// The hover target: the left edge, full height, plus the corner square. The strip covers the whole left
// side, the square covers "somewhere near the bottom-left", which is where the pointer really ends up.
static bool InStartTipZone(const RECT& monitor, POINT p) {
    if(p.y<monitor.top || p.y>=monitor.bottom) return false;
    if(p.x>=monitor.left && p.x<monitor.left+DpiScale(kStartTipEdgeWidth)) return true;
    return InStartTipCornerBox(monitor,p);
}

// The mouse hook runs system-wide for every pointer move, so it must not enumerate windows here. FindWindowW
// results are cached and only resolved again when the cached window is gone: soon when a window that existed
// disappeared, rarely on machines without the companion mod. The cache is thread_local on purpose: the hook
// thread and the UI thread both call this, and two threads must never share these two plain variables.
static HWND CompanionStartScreenCached() {
    thread_local HWND cached=nullptr;
    thread_local ULONGLONG checkedAt=0;
    if(cached && IsWindow(cached)) return cached;
    const ULONGLONG now=GetTickCount64();
    if(now-checkedAt<(cached?2000:10000)) return nullptr;
    checkedAt=now;
    cached=FindCompanionStartScreen();
    return cached;
}

// The Start screen owns one monitor, and the tip belongs on that monitor only: a second screen keeps its own
// left edge, where no Start screen is covering anything.
static HMONITOR CompanionStartScreenMonitor() {
    const HWND screen=CompanionStartScreenCached();
    return screen ? MonitorFromWindow(screen,MONITOR_DEFAULTTONULL) : nullptr;
}

static bool CompanionStartScreenVisibleFast() {
    const HWND screen=CompanionStartScreenCached();
    return screen && IsWindowVisible(screen)!=FALSE;
}

static Gdiplus::Color StartTipColor(COLORREF base, double mixWhite) {
    const double r=GetRValue(base)+(255.0-GetRValue(base))*mixWhite;
    const double g=GetGValue(base)+(255.0-GetGValue(base))*mixWhite;
    const double b=GetBValue(base)+(255.0-GetBValue(base))*mixWhite;
    return Gdiplus::Color(255,(BYTE)(r+0.5),(BYTE)(g+0.5),(BYTE)(b+0.5));
}

// The Build 8102 beta drew its corner logo as a flat four-colour mark on black, with no motion of any kind.
// The fade-in, the couple of percent of growth, the gloss sweep and the white-to-accent hover blend are all
// Windows 8.1 button behaviour, so they run only for the regular logo: choosing the beta style turns the
// tip into a still image that simply appears, holds and goes.
static bool StartTipEffectsEnabled() {
    return g_startLogoStyle.load()!=1 && SystemAnimationsEnabled();
}

static void PaintStartTip(HWND hwnd) {
    PAINTSTRUCT ps{};
    HDC dc=BeginPaint(hwnd,&ps);
    if(!dc) return;
    RECT rc{}; GetClientRect(hwnd,&rc);
    const int w=rc.right, h=rc.bottom;
    ScopedGdiObject background(CreateSolidBrush(RGB(0,0,0)));
    if(background) FillRect(dc,&rc,(HBRUSH)background.get());

    const ULONGLONG now=GetTickCount64();
    const bool animated=StartTipEffectsEnabled();
    double glint=-1.0;
    if(g_startTipGlintAt && animated) {
        const double elapsed=(double)(now-g_startTipGlintAt);
        if(elapsed<(double)kStartTipGlintMs) glint=elapsed/(double)kStartTipGlintMs;
    }
    // The reveal: Metro flyouts faded in while growing a couple of percent, and the flag settles the same
    // way. With animations off the button is simply there at full size.
    double reveal=1.0;
    if(animated) {
        const double elapsed=(double)(now-g_startTipShownAt)/(double)kStartTipFadeMs;
        const double t=(std::max)(0.0,(std::min)(1.0,elapsed));
        const double eased=1.0-(1.0-t)*(1.0-t);
        reveal=(double)kStartTipRevealPercent/100.0+(1.0-(double)kStartTipRevealPercent/100.0)*eased;
    }
    // Windows 8.1 drew the flag white and turned it into the accent colour while the button was hovered;
    // the swap is animated instead of instant. Pressing the button darkens it a little, like the original.
    COLORREF base=BlendColor(RGB(255,255,255),GetStartLogoColor(),
                             (int)((std::max)(0.0,(std::min)(1.0,g_startTipHoverT))*100.0+0.5));
    if(g_startTipPressed) base=BlendColor(base,RGB(0,0,0),22);
    const int box=(std::min)(w,h);
    const int fullLogo=(std::max)(1,(std::min)(DpiScale(kStartTipLogoSize),(std::max)(1,box-2*DpiScale(2))));
    const int logo=(std::max)(1,(int)(fullLogo*reveal+0.5));
    const int logoX=(w-logo)/2, logoY=(h-logo)/2;

    if(g_startLogoStyle.load()==1) {
        // The Build 8102 flag keeps its four colours and is not tinted; the hover shows as the pressed
        // darkening only, and there is no gloss over a four-colour logo. If that path fails for any
        // reason, the accent flag below still paints instead of leaving an empty black square.
        if(DrawCharmIconGdiplus(dc,2,logoX,logoY,logo,logo,RGB(255,255,255))) {
            EndPaint(hwnd,&ps);
            return;
        }
        Wh_Log(L"Build 8102 Start tip glyph failed; drawing the regular flag");
    }
    bool painted=false;
    if(g_gdiplusToken) {
        try {
            Gdiplus::Graphics g(dc);
            if(g.GetLastStatus()!=Gdiplus::Ok) {
                Wh_Log(L"GDI+ Graphics unavailable for the Start tip; using the GDI flag");
            } else {
                g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
                g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
                // DrawStartGlyphWithBrush works in the 44-unit box the other charm icons use.
                g.TranslateTransform((Gdiplus::REAL)logoX,(Gdiplus::REAL)logoY);
                g.ScaleTransform((Gdiplus::REAL)logo/44.0f,(Gdiplus::REAL)logo/44.0f);
                if(glint>=0.0) {
                    // A narrow bright band travelling across the flag, diagonally, the way the original gloss
                    // moved. GDI+ wants strictly increasing stops, so the three inner ones are pushed inside
                    // (0,1) and kept apart even while the band is still off the edge of the flag.
                    Gdiplus::RectF bounds(0.0f,0.0f,44.0f,44.0f);
                    Gdiplus::LinearGradientBrush brush(bounds,StartTipColor(base,0.0),
                                                       StartTipColor(base,0.0),20.0f);
                    Gdiplus::REAL center=(Gdiplus::REAL)(-0.35+1.7*glint);
                    Gdiplus::REAL p1=(Gdiplus::REAL)(center-0.13), p2=center, p3=(Gdiplus::REAL)(center+0.13);
                    p1=(std::max)(0.001f,(std::min)(p1,0.994f));
                    p2=(std::max)(p1+0.002f,(std::min)(p2,0.996f));
                    p3=(std::max)(p2+0.002f,(std::min)(p3,0.998f));
                    Gdiplus::Color colors[5]={StartTipColor(base,0.0),StartTipColor(base,0.05),
                                              StartTipColor(base,0.85),StartTipColor(base,0.05),
                                              StartTipColor(base,0.0)};
                    Gdiplus::REAL positions[5]={0.0f,p1,p2,p3,1.0f};
                    brush.SetInterpolationColors(colors,positions,5);
                    painted=DrawStartGlyphWithBrush(g,&brush);
                } else {
                    Gdiplus::SolidBrush brush(StartTipColor(base,0.0));
                    painted=DrawStartGlyphWithBrush(g,&brush);
                }
            }
        } catch (...) {
            Wh_Log(L"C++ exception while drawing the Start tip");
        }
    }
    if(!painted) {
        // Last resort, with GDI+ gone or refusing: the aliased flag the charms rail uses on that path.
        DrawFallbackCharmIcon(dc,2,logoX,logoY,logo,logo);
    }
    EndPaint(hwnd,&ps);
}

// Close the companion Start screen: that is exactly "go back to the desktop", because the desktop is what the
// full-screen surface is covering. WM_APP+2 is the close-only message in the Start Screen mod (WM_APP+1 is the
// toggle, which would open a screen that is already closed).
//
// Nothing is forced here. The companion animates its close, so its window is still visible for a while after
// it has accepted the request; sending Win+D on top of that - which is what an earlier version did - does not
// "return to the desktop", it minimises every open window behind a close that was working perfectly. The
// slide is watched instead, and if the screen really never goes the click is logged and left alone: the user
// can still press Esc or the Windows key, and no window is disturbed by a guess.
static void ReturnToDesktopFromStartScreen() {
    const HWND screen=FindCompanionStartScreen();
    // The button goes away at once: the Start screen is about to close and nothing should linger over the
    // desktop for the moment it takes to slide out.
    if(g_startTip) {
        KillTimer(g_startTip,TIMER_STARTTIP_FRAME);
        KillTimer(g_startTip,TIMER_STARTTIP_POLL);
        KillTimer(g_startTip,TIMER_STARTTIP_EXIT);
        if(IsWindowVisible(g_startTip)) ShowWindow(g_startTip,SW_HIDE);
        g_startTipShown.store(false);
        g_startTipGlintAt=0;
    }
    g_startTipArmed.store(false);
    g_startTipSuppressUntil.store(GetTickCount()+kStartTipSuppressAfterClickMs);
    if(!screen) {
        // The tip only appears over that screen, so this means it has just gone: nothing left to close.
        Wh_Log(L"Start tip used, but the Windows 8 Start Screen is no longer there");
        return;
    }
    if(!PostMessageW(screen,kCompanionStartClose,0,0)) {
        Wh_Log(L"posting the close message to the Start Screen failed (%lu)",GetLastError());
        return;
    }
    Wh_Log(L"Start tip used; the Windows 8 Start Screen was asked to close");
    g_startTipCloseDeadline=GetTickCount64()+(ULONGLONG)kStartTipCloseTimeoutMs;
    if(g_startTip && !SetTimer(g_startTip,TIMER_STARTTIP_CLOSE,(UINT)kStartTipClosePollMs,nullptr))
        Wh_Log(L"could not watch the Start-screen close (%lu)",GetLastError());
}

// True while a click is still being acted on: the tip must not pop back in that moment.
static bool StartTipSuppressed() {
    const DWORD until=g_startTipSuppressUntil.load();
    return until && (LONG)(until-GetTickCount())>0;
}

// One 15 ms frame timer drives the reveal, the gloss and the hover colour, so the three never fight over it.
static void StartStartTipFrames(HWND hwnd) {
    if(!StartTipEffectsEnabled()) return;
    if(!g_startTipLastFrame) g_startTipLastFrame=GetTickCount64();
    if(!SetTimer(hwnd,TIMER_STARTTIP_FRAME,15,nullptr))
        Wh_Log(L"Start tip frame timer failed (%lu)",GetLastError());
}

static void StartStartTipGlint(HWND hwnd) {
    if(!StartTipEffectsEnabled()) return;
    g_startTipGlintAt=GetTickCount64();
    StartStartTipFrames(hwnd);
}

// The square itself, padded: once it is up, pointing roughly at it must keep it there.
static bool StartTipHitTest(POINT p) {
    if(!g_startTip || !IsWindow(g_startTip)) return false;
    RECT rect{};
    if(!GetWindowRect(g_startTip,&rect)) return false;
    InflateRect(&rect,DpiScale(kStartTipHitPadding),DpiScale(kStartTipHitPadding));
    return PtInRect(&rect,p)!=FALSE;
}

// True while the pointer may still keep the square up: on the square itself or anywhere in the hot area of
// the Start screen's monitor. The exit timer uses it, because a pointer that stops moving the instant it
// leaves the area never produces another mouse hook callback to run the grace down.
static bool StartTipPointerInside() {
    POINT p{};
    if(!GetCursorPos(&p)) return true; // no cursor to judge by: keep the square rather than flicker
    // While the square is still hidden its rectangle sits in the corner, so it must not count as a hit.
    if(g_startTipShown.load() && StartTipHitTest(p)) return true;
    const HMONITOR monitor=CompanionStartScreenMonitor();
    MONITORINFO mi{sizeof(mi)};
    if(monitor && MonitorFromPoint(p,MONITOR_DEFAULTTONEAREST)==monitor && GetMonitorInfoW(monitor,&mi))
        return InStartTipZone(mi.rcMonitor,p);
    return false;
}

static void HideStartTip() {
    if(g_startTip) {
        KillTimer(g_startTip,TIMER_STARTTIP_FRAME);
        KillTimer(g_startTip,TIMER_STARTTIP_DWELL);
        KillTimer(g_startTip,TIMER_STARTTIP_POLL);
        KillTimer(g_startTip,TIMER_STARTTIP_CLOSE);
        KillTimer(g_startTip,TIMER_STARTTIP_EXIT);
        if(g_startTipPressed) {
            g_startTipPressed=false;
            if(GetCapture()==g_startTip) ReleaseCapture();
        }
    }
    if(g_startTip && IsWindowVisible(g_startTip)) ShowWindow(g_startTip,SW_HIDE);
    g_startTipShown.store(false);
    g_startTipGlintAt=0;
    g_startTipLastFrame=0;
    g_startTipHoverT=0.0;
    g_startTipHover=false;
    g_startTipTracking=false; // hiding the window cancels the pending WM_MOUSELEAVE
    // Clearing the latch is what lets the very same corner bring the tip back after the Start screen
    // has been closed and reopened without the pointer ever leaving it. The leave timestamp belongs to
    // the hook thread, which clears it when it arms again.
    g_startTipArmed.store(false);
}

static void ShowStartTip(); // defined below the window procedure

// One animation frame: the fade-in alpha and, while it runs, a repaint for the gloss.
static void UpdateStartTipAnimation(HWND hwnd) {
    const ULONGLONG now=GetTickCount64();
    const bool animated=StartTipEffectsEnabled();
    int alpha=255;
    if(animated) {
        const ULONGLONG elapsed=now-g_startTipShownAt;
        if(elapsed<(ULONGLONG)kStartTipFadeMs) {
            const double t=(double)elapsed/(double)kStartTipFadeMs;
            const double eased=1.0-(1.0-t)*(1.0-t); // ease-out, like the original appearing
            alpha=(int)(255.0*eased+0.5);
        }
    }
    SetLayeredWindowAttributes(hwnd,0,(BYTE)alpha,LWA_ALPHA);
    // Hover colour: white and accent are blended over 120 ms rather than swapped in one frame.
    const double target=g_startTipHover?1.0:0.0;
    if(!animated) {
        g_startTipHoverT=target;
    } else if(g_startTipLastFrame && now>g_startTipLastFrame) {
        const double step=(double)(now-g_startTipLastFrame)/(double)kStartTipHoverMs;
        if(g_startTipHoverT<target) g_startTipHoverT=(std::min)(target,g_startTipHoverT+step);
        else if(g_startTipHoverT>target) g_startTipHoverT=(std::max)(target,g_startTipHoverT-step);
        if(std::fabs(g_startTipHoverT-target)<0.01) g_startTipHoverT=target; // land exactly on the colour
    }
    g_startTipLastFrame=now;
    InvalidateRect(hwnd,nullptr,FALSE);
    const bool fading=animated && (now-g_startTipShownAt)<(ULONGLONG)kStartTipFadeMs;
    const bool glinting=g_startTipGlintAt && animated &&
                         (now-g_startTipGlintAt)<(ULONGLONG)kStartTipGlintMs;
    const bool stillMoving=animated && g_startTipHoverT!=target;
    if(!fading && !glinting && !stillMoving) {
        g_startTipGlintAt=0;
        g_startTipLastFrame=0;
        KillTimer(hwnd,TIMER_STARTTIP_FRAME);
    }
}

LRESULT CALLBACK StartTipProcImpl(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    switch(msg) {
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: PaintStartTip(hwnd); return 0;
        case WM_TIMER:
            if(wp==TIMER_STARTTIP_DWELL) {
                KillTimer(hwnd,TIMER_STARTTIP_DWELL);
                // The dwell only counts if the pointer stopped in the corner and the Start screen is still
                // the thing on screen: passing through on the way somewhere else must not raise the tip.
                POINT cursor{};
                if(!GetCursorPos(&cursor)) return 0;
                if(!CompanionStartScreenVisible()) return 0;
                HMONITOR monitor=MonitorFromPoint(cursor,MONITOR_DEFAULTTONEAREST);
                if(!monitor || monitor!=CompanionStartScreenMonitor()) return 0;
                MONITORINFO mi{sizeof(mi)};
                if(!GetMonitorInfoW(monitor,&mi)) return 0;
                if(g_menu && IsWindowVisible(g_menu)) return 0;
                if(!InStartTipZone(mi.rcMonitor,cursor)) return 0;
                if(StartTipSuppressed()) return 0;
                ShowStartTip();
                return 0;
            }
            if(wp==TIMER_STARTTIP_FRAME) {
                UpdateStartTipAnimation(hwnd);
                return 0;
            }
            if(wp==TIMER_STARTTIP_CLOSE) {
                // Repeating while the slide runs: the screen is gone when the window stops being visible, and
                // that is the only thing worth waiting for. Nothing is sent on top of a close in progress.
                if(!CompanionStartScreenVisible()) {
                    KillTimer(hwnd,TIMER_STARTTIP_CLOSE);
                    return 0;
                }
                if(GetTickCount64()>=g_startTipCloseDeadline) {
                    KillTimer(hwnd,TIMER_STARTTIP_CLOSE);
                    Wh_Log(L"the Start Screen is still open %d s after the close request; "
                           L"leaving it alone (Esc or the Windows key still closes it)",
                           kStartTipCloseTimeoutMs/1000);
                }
                return 0;
            }
            if(wp==TIMER_STARTTIP_EXIT) {
                KillTimer(hwnd,TIMER_STARTTIP_EXIT);
                // The pointer may have stopped moving the instant it left, in which case the hook never
                // runs again; the decision is taken here, on real cursor state. It also covers the case
                // where the square had not appeared yet: a dwell still waiting must be dropped, otherwise
                // the latch would stay set and the next visit to the corner would never re-arm it.
                if(StartTipPointerInside()) return 0;
                if(g_startTipShown.load()) {
                    HideStartTip();
                } else {
                    KillTimer(hwnd,TIMER_STARTTIP_DWELL);
                    g_startTipArmed.store(false);
                }
                return 0;
            }
            if(wp==TIMER_STARTTIP_POLL) {
                if(!CompanionStartScreenVisible()) {
                    HideStartTip();
                    return 0;
                }
                // A monitor or DPI change moves the corner: follow it instead of leaving a stray square.
                MONITORINFO mi{sizeof(mi)};
                const HMONITOR monitor=CompanionStartScreenMonitor();
                RECT current{};
                if(monitor && GetMonitorInfoW(monitor,&mi) && GetWindowRect(hwnd,&current)) {
                    const int size=DpiScale(kStartTipSize);
                    if(current.left!=mi.rcMonitor.left || current.bottom!=mi.rcMonitor.bottom ||
                       current.right-current.left!=size || current.bottom-current.top!=size) {
                        SetWindowPos(hwnd,HWND_TOPMOST,mi.rcMonitor.left,mi.rcMonitor.bottom-size,
                                     size,size,SWP_NOACTIVATE|SWP_NOOWNERZORDER);
                    }
                }
                RaiseCharmsSurface(hwnd);
                return 0;
            }
            break;
        case WM_MOUSEMOVE:
            if(!g_startTipTracking) {
                TRACKMOUSEEVENT track{sizeof(track),TME_LEAVE,hwnd,0};
                if(TrackMouseEvent(&track)) g_startTipTracking=true;
            }
            // Under the beta logo there is no hover tint and no gloss, so there is nothing to repaint for.
            if(!g_startTipHover && StartTipEffectsEnabled()) {
                g_startTipHover=true;
                StartStartTipGlint(hwnd); // accent fade and gloss start together
                StartStartTipFrames(hwnd);
            }
            return 0;
        case WM_MOUSELEAVE:
            g_startTipTracking=false;
            g_startTipHover=false;
            StartStartTipFrames(hwnd); // fade back to white
            return 0;
        case WM_LBUTTONDOWN:
            // Act like a button: the press is shown, the capture keeps the release, and letting go
            // outside the square cancels instead of activating.
            g_startTipPressed=true;
            SetCapture(hwnd);
            InvalidateRect(hwnd,nullptr,FALSE);
            return 0;
        case WM_CAPTURECHANGED:
            if(g_startTipPressed) {
                g_startTipPressed=false;
                InvalidateRect(hwnd,nullptr,FALSE);
            }
            return 0;
        case WM_LBUTTONUP: {
            const bool wasPressed=g_startTipPressed;
            g_startTipPressed=false;
            if(GetCapture()==hwnd) ReleaseCapture();
            POINT cursor{};
            RECT client{};
            const bool stillOnButton=wasPressed && GetCursorPos(&cursor) &&
                ScreenToClient(hwnd,&cursor) && GetClientRect(hwnd,&client) &&
                PtInRect(&client,cursor)!=FALSE;
            if(stillOnButton) {
                ReturnToDesktopFromStartScreen();
            } else {
                InvalidateRect(hwnd,nullptr,FALSE);
            }
            return 0;
        }
        case WM_RBUTTONUP:
            // Right-clicking the Start button has always opened the Win+X style menu.
            HideStartTip();
            if(!g_threadId || !PostThreadMessageW(g_threadId,WM_PORT_MENU,0,0))
                Wh_Log(L"could not ask the UI thread for the Start tip menu (%lu)",GetLastError());
            return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

LRESULT CALLBACK StartTipProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    try {
        return StartTipProcImpl(hwnd,msg,wp,lp);
    } catch (...) {
        Wh_Log(L"C++ exception while handling the Start tip");
        return DefWindowProcW(hwnd,msg,wp,lp);
    }
}

static void ShowStartTip() {
    if(g_startTipShown.load() || StartTipSuppressed()) return;
    if(!g_startTip || !IsWindow(g_startTip)) {
        if(g_startTip) Wh_Log(L"the Start tip window is gone; not showing it");
        return;
    }
    HMONITOR monitor=CompanionStartScreenMonitor();
    if(!monitor) {
        POINT cursor{}; GetCursorPos(&cursor);
        monitor=MonitorFromPoint(cursor,MONITOR_DEFAULTTONEAREST);
    }
    MONITORINFO mi{sizeof(mi)};
    if(!monitor || !GetMonitorInfoW(monitor,&mi)) return;
    const int size=DpiScale(kStartTipSize);
    const ULONGLONG now=GetTickCount64();
    const bool animated=StartTipEffectsEnabled();
    g_startTipShownAt=now;
    g_startTipGlintAt=0;
    g_startTipLastFrame=now;
    g_startTipHoverT=0.0; // the flag arrives white and turns accent under the pointer
    g_startTipPressed=false;
    g_startTipTracking=false;
    // The pointer may already be on the square - in the corner it always is - and then the gloss starts
    // together with the fade, exactly as the original glinted under the cursor that had just summoned it.
    POINT cursor{};
    g_startTipHover=GetCursorPos(&cursor) &&
        cursor.x>=mi.rcMonitor.left && cursor.x<mi.rcMonitor.left+size &&
        cursor.y>=mi.rcMonitor.bottom-size && cursor.y<mi.rcMonitor.bottom;
    // Transparency is set before the window appears: with the beta logo, which is a still image, the
    // square has to be fully opaque in its very first frame instead of inheriting the alpha a previous
    // fade happened to stop at.
    SetLayeredWindowAttributes(g_startTip,0,(BYTE)(animated?0:255),LWA_ALPHA);
    SetWindowPos(g_startTip,HWND_TOPMOST,
                 mi.rcMonitor.left,mi.rcMonitor.bottom-size,size,size,
                 SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOOWNERZORDER);
    // Same z-band as the Start screen, but the Start Screen mod re-raises its own window whenever it
    // opens; re-raising here and on every poll keeps the square whole instead of half covered.
    RaiseCharmsSurface(g_startTip);
    g_startTipShown.store(true);
    if(animated) {
        if(g_startTipHover) g_startTipGlintAt=now;
        if(!SetTimer(g_startTip,TIMER_STARTTIP_FRAME,15,nullptr))
            Wh_Log(L"Start tip frame timer failed (%lu)",GetLastError());
    }
    InvalidateRect(g_startTip,nullptr,FALSE);
    // A 500 ms poll retires the tip if the Start screen closes by other means (the Windows key, the Charms
    // Bar Start charm, the Start Screen mod itself).
    SetTimer(g_startTip,TIMER_STARTTIP_POLL,500,nullptr);
    static bool logged=false;
    if(!logged) {
        logged=true;
        Wh_Log(L"Start tip shown in the bottom-left corner of the Start screen");
    }
}

// ---------------------------------------------------------------- native search state
// DWM cloaking is not portable across Windows builds (the same window reported cloaked/open differently on
// 21H2 and 24H2), so the native fallback no longer depends on it alone. It tracks the hand-off instead: the
// Windows search flyout takes the foreground when it opens and gives it back when it is dismissed, which the
// foreground hook below already observes. Every chord we send is verified 700 ms later, so a chord that did
// nothing cannot leave the charm stuck in a falsely "open" state.
static void NoteSearchChordSent() {
    g_searchKnownOpen.store(true);
    g_searchVerifyTick.store(GetTickCount());
    if(g_rail) {
        KillTimer(g_rail,TIMER_SEARCH_VERIFY);
        if(!SetTimer(g_rail,TIMER_SEARCH_VERIFY,700,nullptr))
            Wh_Log(L"could not arm the search verification timer (%lu)",GetLastError());
    }
}

// Called from RailProcImpl 700 ms after a chord: confirms the search UI appeared, or clears the state so the
// next press sends the chord again. Without this, one chord that Windows ignored would make every following
// click look dead, which is the failure the log showed.
static void VerifySearchChord() {
    if(FindWindowsSearchWindow()) {
        Wh_Log(L"search UI confirmed open after the Win chord");
        return;
    }
    const bool wasOpen=g_searchKnownOpen.exchange(false);
    if(wasOpen)
        Wh_Log(L"no search window was found after the Win chord; the next Search press will send "
               L"it again instead of assuming the search UI is open");
}

void ActivateCharm(int slot) {
    if(slot<0 || slot>=GetCharmSlotCount()) return;
    const int id=GetCharmAction(slot);
    if(id<0 || id>=kCharmCount || g_hidden[id]) return;
    HWND target=GetForegroundWindow();
    if(!target || target==g_rail || target==g_clock || target==g_menu)
        target=GetDesktopWindow();
    const bool immersive=IsImmersiveWindow(target);
    if(id==0) {
        // 1. The companion Search Charm mod, when installed, owns the search UI: hand the request over with a
        //    message instead of a keystroke. Never toggles, so a second click cannot close it.
        if(TryOpenCompanionSearchPanel(L"Search charm")) {
            CompleteCharmAction();
            return;
        }
        // 2. Native Windows search: leave it alone when it is already open.
        const HWND searchWindow=FindWindowsSearchWindow();
        if(searchWindow || g_searchKnownOpen.load()) {
            if(searchWindow) {
                wchar_t searchClass[64]{};
                GetClassNameW(searchWindow,searchClass,_countof(searchClass));
                Wh_Log(L"Search charm pressed while the search UI is already open "
                       L"(hwnd 0x%p, class \"%s\"); leaving it open instead of toggling it closed",
                       (void*)searchWindow,searchClass);
            } else {
                Wh_Log(L"Search charm pressed while the search UI is already open (tracked "
                       L"state); leaving it open instead of toggling it closed");
            }
            CompleteCharmAction();
            return;
        }
        // 3. Send the Windows shortcut and arm the 700 ms verification.
        const int mode=g_searchHotkey.load();
        const WORD key=mode==1?'S':mode==2?'Q':(immersive?'Q':'S');
        // Deliver the chord before hiding the no-activate rail; this avoids racing the foreground hand-off.
        auto sendSearchChord=[&]() {
            Wh_Log(L"Search charm opening the Windows search UI with Win+%c",key);
            bool sent=SendWinChord(key);
            if(!sent && mode==0)
                sent=SendWinChord(key=='S'?'Q':'S');
            if(!sent)
                Wh_Log(L"Search Win+%c injection failed; nothing was sent",key);
            else
                NoteSearchChordSent();
        };
        if(g_hideAfterClick.load()) {
            sendSearchChord();
            CompleteCharmAction();
        } else {
            CompleteCharmAction();
            sendSearchChord();
        }
        return;
    }
    if(id==1) {
        if(IsWindows10OrLater()) {
            HideCharms(true);
            if(ShowSharePane()) return;
        }
        LaunchShare();
        return;
    }
    if(id==3 && IsWindows10OrLater()) {
        HideCharms(true);
        if(ShowDevicesPane()) return;
    }
    if(id==4 && g_settingsPane.load() && (IsWindows10OrLater() || !IsWindows8OrLater())) {
        HideCharms(true);
        if(ShowSettingsPane()) return;
    }
    CompleteCharmAction();
    switch(id) {
        case 2:
            // The Windows 8 (8045) Start Screen mod, when installed, owns the Start screen.
            if(TryOpenCompanionStartScreen(L"Start charm")) break;
            // If that mod is installed but did not take the message, its own Win-key hook still owns the
            // screen, so a Win tap is the right fallback. Open-Shell is only used when the mod is absent.
            if(FindCompanionStartScreen() || !OpenOpenShellStartMenu()) SendWinTap();
            break;
        case 3:
            if(IsWindows8OrLater()) {
                // Native Devices/Connect surface; no fabricated generic target list.
                SendWinChord('K');
            } else {
                ShellExecuteW(nullptr,L"open",L"control.exe",
                              L"/name Microsoft.DevicesAndPrinters",nullptr,SW_SHOWNORMAL);
            }
            break;
        case 4:
            if(IsWindows10OrLater() && !immersive) {
                HINSTANCE result=ShellExecuteW(nullptr,L"open",L"ms-settings:",nullptr,nullptr,SW_SHOWNORMAL);
                if((INT_PTR)result<=32) ShellExecuteW(nullptr,L"open",L"control.exe",nullptr,nullptr,SW_SHOWNORMAL);
            } else if(IsWindows8OrLater()) {
                SendWinChord('I'); // Native Windows 8.x Settings charm / Win+I.
            } else {
                ShellExecuteW(nullptr,L"open",L"control.exe",nullptr,nullptr,SW_SHOWNORMAL);
            }
            break;
    }
}

int ReadRadioState() {
    ScopedRegKey key;
    DWORD value=0,type=0,size=sizeof(value);
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Control\\RadioManagement",
        0,KEY_QUERY_VALUE,key.put())!=ERROR_SUCCESS) return 0;
    const LONG rc=RegQueryValueExW(key.get(),L"SystemRadioState",nullptr,&type,
                                   reinterpret_cast<BYTE*>(&value),&size);
    return rc==ERROR_SUCCESS && type==REG_DWORD?(int)value:0;
}

static bool HasActiveEthernetAdapter() {
    // Adapter enumeration is synchronous and relatively expensive; the status surface refreshes once a
    // second, so cache this independent link-state query.
    static ULONGLONG lastCheck=0;
    static bool cachedResult=false;
    const ULONGLONG now=GetTickCount64();
    if(lastCheck && now-lastCheck<3000) return cachedResult;
    bool detected=false;
    try {
        ULONG bufferSize=16*1024;
        std::vector<BYTE> storage(bufferSize);
        for(int attempt=0;attempt<3;attempt++) {
            ULONG required=(ULONG)storage.size();
            const ULONG result=GetAdaptersAddresses(AF_UNSPEC,
                GAA_FLAG_SKIP_ANYCAST|GAA_FLAG_SKIP_MULTICAST|GAA_FLAG_SKIP_DNS_SERVER,
                nullptr,reinterpret_cast<PIP_ADAPTER_ADDRESSES>(storage.data()),&required);
            if(result==ERROR_BUFFER_OVERFLOW && required>storage.size()) {
                storage.resize(required);
                continue;
            }
            if(result!=NO_ERROR) break;
            for(auto* adapter=reinterpret_cast<PIP_ADAPTER_ADDRESSES>(storage.data());
                adapter;adapter=adapter->Next) {
                if(adapter->IfType==IF_TYPE_ETHERNET_CSMACD && adapter->OperStatus==IfOperStatusUp) {
                    detected=true;
                    break;
                }
            }
            break;
        }
    } catch (...) {
        // A transient allocation/API failure should not break status updates.
    }
    cachedResult=detected;
    lastCheck=now;
    return cachedResult;
}

void RefreshStatusData() {
    try {
    SYSTEM_POWER_STATUS power{};
    if (GetSystemPowerStatus(&power) && power.BatteryLifePercent != 255) {
        g_batteryLevel.store(power.BatteryLifePercent);
        g_batteryCharging.store(power.BatteryFlag != 255 && (power.BatteryFlag & BATTERY_FLAG_CHARGING) != 0);
    } else {
        g_batteryLevel.store(-1);
        g_batteryCharging.store(false);
    }

    int network=0, signal=-1;
    bool adapterDetected=false, radioStateKnown=false, anyRadioOn=false;
    ScopedWlanHandle wlan;
    DWORD negotiated=0;
    if (WlanOpenHandle(2,nullptr,&negotiated,wlan.put()) == ERROR_SUCCESS) {
        ScopedWlanMemory listMemory;
        if (WlanEnumInterfaces(wlan.get(),nullptr,
                reinterpret_cast<PWLAN_INTERFACE_INFO_LIST*>(listMemory.put())) == ERROR_SUCCESS &&
            listMemory.get()) {
            auto* list=static_cast<PWLAN_INTERFACE_INFO_LIST>(listMemory.get());
            adapterDetected=list->dwNumberOfItems>0;
            for (DWORD i=0;i<list->dwNumberOfItems;i++) {
                const GUID* interfaceGuid=&list->InterfaceInfo[i].InterfaceGuid;

                ScopedWlanMemory radioMemory;
                DWORD radioSize=0;
                WLAN_OPCODE_VALUE_TYPE radioType{};
                if (WlanQueryInterface(wlan.get(),interfaceGuid,wlan_intf_opcode_radio_state,
                        nullptr,&radioSize,radioMemory.put(),&radioType)==ERROR_SUCCESS &&
                    radioMemory.get()) {
                    auto* radioState=static_cast<WLAN_RADIO_STATE*>(radioMemory.get());
                    const DWORD phyCount=(std::min)(radioState->dwNumberOfPhys, (DWORD)WLAN_MAX_PHY_INDEX);
                    if (phyCount>0) radioStateKnown=true;
                    for (DWORD phy=0;phy<phyCount;phy++) {
                        const WLAN_PHY_RADIO_STATE& item=radioState->PhyRadioState[phy];
                        if (item.dot11SoftwareRadioState!=dot11_radio_state_off &&
                            item.dot11HardwareRadioState!=dot11_radio_state_off) anyRadioOn=true;
                    }
                }

                ScopedWlanMemory connectionMemory;
                DWORD connectionSize=0;
                WLAN_OPCODE_VALUE_TYPE connectionType{};
                if (WlanQueryInterface(wlan.get(),interfaceGuid,
                        wlan_intf_opcode_current_connection,nullptr,&connectionSize,
                        connectionMemory.put(),&connectionType)==ERROR_SUCCESS && connectionMemory.get()) {
                    auto* attributes=static_cast<WLAN_CONNECTION_ATTRIBUTES*>(connectionMemory.get());
                    if (attributes->isState==wlan_interface_state_connected) {
                        network=2;
                        signal=(int)attributes->wlanAssociationAttributes.wlanSignalQuality;
                    }
                }
            }
        }
    }

    DWORD internetFlags=0;
    const BOOL internet=InternetGetConnectedState(&internetFlags,0);
    // A live WLAN association takes priority over any other active adapter. When there is no associated Wi-Fi
    // link, use IP Helper to detect wired Ethernet.
    const bool wiredEthernet=HasActiveEthernetAdapter();
    if(wiredEthernet && network!=2) {
        network=1;
        signal=-1;
        adapterDetected=true;
    } else if(network==0 && internet && (internetFlags&INTERNET_CONNECTION_LAN) && !adapterDetected) {
        // Fallback for systems where WLAN enumeration is unavailable.
        network=1;
        adapterDetected=true;
    }
    if(network==2 && internet) network=3;

    g_networkState.store(network);
    g_wifiSignal.store(signal);
    g_adapterDetected.store(adapterDetected);
    g_wifiRadioOff.store(adapterDetected && radioStateKnown && !anyRadioOn);
    g_airplane.store(ReadRadioState()==1);
    } catch (...) {
        Wh_Log(L"C++ exception while refreshing system status");
        g_networkState.store(0);
        g_wifiSignal.store(-1);
        g_adapterDetected.store(false);
        g_wifiRadioOff.store(false);
        // Retain the last valid battery reading rather than replacing it with a guess.
    }
}
std::wstring LocalizedNetworkStatus();
std::wstring BuildAccessibleStatus();

void UpdateStatus() {
    RefreshStatusData();
    if (!g_clock) return;
    try {
        const std::wstring status = BuildAccessibleStatus();
        SetWindowTextW(g_clock, status.c_str()); // Exposes network and power state to accessibility clients.
    } catch (...) {
        SetWindowTextW(g_clock, L"Charms status");
    }
    InvalidateRect(g_clock, nullptr, TRUE);
}

// The Build 8102 logo is a four-pane flag of about 331 x 294 units, four flat colours, straight-ish
// sides with a slight bow - the same shapes as the 331 x 294 sprite this mod used to embed as a
// base64 PNG. These outlines were traced from that sprite once (marching squares on each pane's
// saturated mask, then a light smoothing pass and Douglas-Peucker at 0.15 px) and are now the source
// artwork: 898 points in total, about 18 KB of source instead of 528 KB of base64 pixels, and sharp at
// any DPI because GDI+ fills them with anti-aliasing instead of scaling a bitmap.
constexpr int kStart8102DesignWidth=331;
constexpr int kStart8102DesignHeight=294;

// The largest pane, used to size the GDI fallback's point buffer. The static assertions below turn a future
// edit of these arrays into a compile error instead of a silently skipped pane at runtime.
constexpr int kStart8102MaxPoints=236;

static const Gdiplus::PointF kStart8102_red[]={
    {159.37f,129.85f},{158.63f,129.85f},{156.37f,128.13f},{153.01f,126.49f},{151.37f,125.13f},{146.63f,122.87f},
    {144.37f,121.13f},{138.36f,118.14f},{136.64f,117.86f},{131.36f,115.14f},{129.64f,114.86f},{126.36f,113.14f},
    {124.64f,112.86f},{123.36f,112.14f},{121.64f,111.86f},{120.36f,111.14f},{117.64f,110.86f},{116.36f,110.14f},
    {112.64f,109.86f},{111.36f,109.14f},{110.49f,109.01f},{93.51f,109.01f},{92.64f,109.14f},{91.36f,109.86f},
    {86.64f,110.14f},{85.36f,110.86f},{81.64f,111.14f},{80.36f,111.86f},{76.64f,112.14f},{75.36f,112.86f},
    {72.64f,113.14f},{71.36f,113.86f},{68.64f,114.14f},{67.36f,114.86f},{64.64f,115.14f},{63.36f,115.86f},
    {60.64f,116.14f},{59.36f,116.86f},{57.64f,117.14f},{56.36f,117.86f},{54.64f,118.14f},{53.36f,118.86f},
    {50.64f,119.14f},{49.36f,119.86f},{47.64f,120.14f},{46.36f,120.86f},{44.64f,121.14f},{43.36f,121.86f},
    {41.64f,122.14f},{40.36f,122.86f},{38.64f,123.14f},{37.51f,123.72f},{37.15f,123.35f},{37.15f,122.62f},
    {37.86f,121.36f},{38.14f,118.64f},{38.86f,117.36f},{39.14f,115.64f},{39.86f,114.36f},{40.14f,111.64f},
    {40.86f,110.36f},{41.14f,108.64f},{41.86f,107.36f},{42.14f,104.64f},{42.86f,103.36f},{43.14f,101.64f},
    {43.86f,100.36f},{44.14f,98.64f},{44.86f,97.36f},{45.14f,94.64f},{45.86f,93.36f},{46.14f,91.64f},
    {46.86f,90.36f},{47.14f,87.64f},{47.86f,86.36f},{48.14f,84.64f},{48.86f,83.36f},{49.14f,80.64f},
    {49.86f,79.36f},{50.14f,77.64f},{50.86f,76.36f},{51.14f,73.64f},{51.86f,72.36f},{52.14f,70.64f},
    {52.86f,69.36f},{53.14f,66.64f},{53.86f,65.36f},{54.14f,63.64f},{54.86f,62.36f},{55.14f,59.64f},
    {55.86f,58.36f},{56.14f,56.64f},{56.86f,55.36f},{57.14f,53.64f},{57.86f,52.36f},{58.14f,49.64f},
    {58.86f,48.36f},{59.14f,46.64f},{59.86f,45.36f},{60.14f,42.64f},{60.86f,41.36f},{61.14f,39.64f},
    {61.86f,38.36f},{62.14f,35.64f},{62.86f,34.36f},{63.14f,32.64f},{63.86f,31.36f},{64.14f,28.64f},
    {64.86f,27.36f},{65.14f,25.64f},{65.86f,24.36f},{66.14f,21.64f},{66.86f,20.36f},{67.14f,18.64f},
    {67.86f,17.36f},{68.14f,14.64f},{68.51f,14.01f},{69.64f,13.14f},{71.36f,12.86f},{78.64f,9.14f},
    {80.36f,8.86f},{81.64f,8.14f},{83.36f,7.86f},{84.64f,7.14f},{86.36f,6.86f},{87.64f,6.14f},
    {89.36f,5.86f},{90.64f,5.14f},{93.36f,4.86f},{94.64f,4.14f},{97.36f,3.86f},{98.64f,3.14f},
    {101.36f,2.86f},{102.64f,2.14f},{107.36f,1.86f},{108.64f,1.14f},{109.51f,1.01f},{114.49f,0.99f},
    {115.36f,0.86f},{116.64f,0.14f},{117.51f,0.01f},{137.49f,0.01f},{138.36f,0.14f},{139.64f,0.86f},
    {145.49f,1.01f},{146.36f,1.14f},{147.64f,1.86f},{151.36f,2.14f},{152.64f,2.86f},{155.36f,3.14f},
    {156.64f,3.86f},{158.36f,4.14f},{159.64f,4.86f},{161.36f,5.14f},{162.64f,5.86f},{164.36f,6.14f},
    {165.64f,6.86f},{167.36f,7.14f},{181.37f,14.13f},{183.01f,15.49f},{184.99f,16.51f},{186.01f,17.49f},
    {187.99f,18.51f},{189.01f,19.49f},{190.36f,20.14f},{190.85f,20.65f},{190.99f,23.49f},{190.86f,24.36f},
    {190.14f,25.64f},{189.86f,28.36f},{189.14f,29.64f},{188.86f,31.36f},{188.14f,32.64f},{187.86f,35.36f},
    {187.14f,36.64f},{186.86f,38.36f},{186.14f,39.64f},{185.86f,42.36f},{185.14f,43.64f},{184.86f,45.36f},
    {184.14f,46.64f},{183.86f,48.36f},{183.14f,49.64f},{182.86f,52.36f},{182.14f,53.64f},{181.86f,55.36f},
    {181.14f,56.64f},{180.86f,59.36f},{180.14f,60.64f},{179.86f,62.36f},{179.14f,63.64f},{178.86f,66.36f},
    {178.14f,67.64f},{177.86f,69.36f},{177.14f,70.64f},{176.86f,73.36f},{176.14f,74.64f},{175.86f,76.36f},
    {175.14f,77.64f},{174.86f,79.36f},{174.14f,80.64f},{173.86f,83.36f},{173.14f,84.64f},{172.86f,86.36f},
    {172.14f,87.64f},{171.86f,90.36f},{171.14f,91.64f},{170.86f,93.36f},{170.14f,94.64f},{169.86f,97.36f},
    {169.14f,98.64f},{168.86f,100.36f},{168.14f,101.64f},{167.86f,104.36f},{167.14f,105.64f},{166.86f,107.36f},
    {166.14f,108.64f},{165.86f,110.36f},{165.14f,111.64f},{164.86f,114.36f},{164.14f,115.64f},{163.86f,117.36f},
    {163.14f,118.64f},{162.86f,121.36f},{162.14f,122.64f},{161.86f,124.36f},{161.14f,125.64f},{160.86f,128.36f},
    {159.99f,129.49f},
};
constexpr COLORREF kStart8102RedColor=RGB(236,28,35);

static const Gdiplus::PointF kStart8102_green[]={
    {237.36f,164.86f},{231.51f,164.99f},{230.64f,164.86f},{229.36f,164.14f},{228.49f,164.01f},{221.64f,163.86f},
    {220.36f,163.14f},{216.64f,162.86f},{215.36f,162.14f},{213.64f,161.86f},{212.36f,161.14f},{209.64f,160.86f},
    {206.36f,159.14f},{204.64f,158.86f},{201.36f,157.14f},{199.64f,156.86f},{195.63f,154.87f},{193.37f,153.13f},
    {190.63f,151.87f},{188.99f,150.51f},{187.01f,149.49f},{185.99f,148.51f},{184.01f,147.49f},{182.99f,146.51f},
    {181.01f,145.49f},{177.51f,141.99f},{177.14f,141.36f},{177.14f,137.64f},{177.86f,136.36f},{178.14f,134.64f},
    {178.86f,133.36f},{179.14f,131.64f},{179.86f,130.36f},{180.14f,127.64f},{180.86f,126.36f},{181.14f,124.64f},
    {181.86f,123.36f},{182.14f,120.64f},{182.86f,119.36f},{183.14f,117.64f},{183.86f,116.36f},{184.14f,114.64f},
    {184.86f,113.36f},{185.14f,110.64f},{185.86f,109.36f},{186.14f,107.64f},{186.86f,106.36f},{187.14f,103.64f},
    {187.86f,102.36f},{188.14f,100.64f},{188.86f,99.36f},{189.14f,97.64f},{189.86f,96.36f},{190.14f,93.64f},
    {190.86f,92.36f},{191.14f,90.64f},{191.86f,89.36f},{192.14f,86.64f},{192.86f,85.36f},{193.14f,83.64f},
    {193.86f,82.36f},{194.14f,79.64f},{194.86f,78.36f},{195.14f,76.64f},{195.86f,75.36f},{196.14f,73.64f},
    {196.86f,72.36f},{197.14f,69.64f},{197.86f,68.36f},{198.14f,66.64f},{198.86f,65.36f},{199.14f,62.64f},
    {199.86f,61.36f},{200.14f,59.64f},{200.86f,58.36f},{201.14f,56.64f},{201.86f,55.36f},{202.14f,52.64f},
    {202.86f,51.36f},{203.14f,49.64f},{203.86f,48.36f},{204.14f,45.64f},{204.86f,44.36f},{205.14f,42.64f},
    {205.86f,41.36f},{206.14f,39.64f},{206.86f,38.36f},{207.14f,35.64f},{208.51f,33.28f},{210.99f,34.51f},
    {212.01f,35.49f},{213.99f,36.51f},{215.01f,37.49f},{216.99f,38.51f},{218.63f,39.87f},{232.64f,46.86f},
    {234.36f,47.14f},{237.64f,48.86f},{239.36f,49.14f},{240.64f,49.86f},{242.36f,50.14f},{243.64f,50.86f},
    {246.36f,51.14f},{247.64f,51.86f},{251.36f,52.14f},{252.64f,52.86f},{256.36f,53.14f},{257.64f,53.86f},
    {258.51f,53.99f},{265.36f,54.14f},{266.64f,54.86f},{267.51f,54.99f},{271.36f,54.86f},{272.64f,54.14f},
    {273.51f,54.01f},{281.49f,53.99f},{282.36f,53.86f},{283.64f,53.14f},{287.49f,52.99f},{288.36f,52.86f},
    {289.64f,52.14f},{293.36f,51.86f},{294.64f,51.14f},{297.36f,50.86f},{298.64f,50.14f},{301.36f,49.86f},
    {302.64f,49.14f},{305.36f,48.86f},{306.64f,48.14f},{308.36f,47.86f},{309.64f,47.14f},{311.36f,46.86f},
    {312.64f,46.14f},{314.36f,45.86f},{315.64f,45.14f},{317.36f,44.86f},{318.64f,44.14f},{320.36f,43.86f},
    {321.64f,43.14f},{323.36f,42.86f},{324.64f,42.14f},{326.36f,41.86f},{329.62f,40.15f},{330.36f,40.16f},
    {330.84f,40.64f},{330.85f,41.38f},{330.14f,42.64f},{329.86f,45.36f},{329.14f,46.64f},{328.86f,48.36f},
    {328.14f,49.64f},{327.86f,51.36f},{327.14f,52.64f},{326.86f,55.36f},{326.14f,56.64f},{325.86f,58.36f},
    {325.14f,59.64f},{324.86f,62.36f},{324.14f,63.64f},{323.86f,65.36f},{323.14f,66.64f},{322.86f,69.36f},
    {322.14f,70.64f},{321.86f,72.36f},{321.14f,73.64f},{320.86f,76.36f},{320.14f,77.64f},{319.86f,79.36f},
    {319.14f,80.64f},{318.86f,83.36f},{318.14f,84.64f},{317.86f,86.36f},{317.14f,87.64f},{316.86f,90.36f},
    {316.14f,91.64f},{315.86f,93.36f},{315.14f,94.64f},{314.86f,97.36f},{314.14f,98.64f},{313.86f,100.36f},
    {313.14f,101.64f},{312.86f,103.36f},{312.14f,104.64f},{311.86f,107.36f},{311.14f,108.64f},{310.86f,110.36f},
    {310.14f,111.64f},{309.86f,114.36f},{309.14f,115.64f},{308.86f,117.36f},{308.14f,118.64f},{307.86f,121.36f},
    {307.14f,122.64f},{306.86f,124.36f},{306.14f,125.64f},{305.86f,128.36f},{305.14f,129.64f},{304.86f,131.36f},
    {304.14f,132.64f},{303.86f,135.36f},{303.14f,136.64f},{302.86f,138.36f},{302.14f,139.64f},{301.86f,142.36f},
    {301.14f,143.64f},{300.86f,145.36f},{300.14f,146.64f},{299.86f,148.36f},{299.49f,148.99f},{298.37f,149.87f},
    {296.36f,150.86f},{294.64f,151.14f},{289.36f,153.86f},{287.64f,154.14f},{286.36f,154.86f},{284.64f,155.14f},
    {281.36f,156.86f},{278.64f,157.14f},{277.36f,157.86f},{275.64f,158.14f},{274.36f,158.86f},{271.64f,159.14f},
    {270.36f,159.86f},{267.64f,160.14f},{266.36f,160.86f},{263.64f,161.14f},{262.36f,161.86f},{257.64f,162.14f},
    {256.36f,162.86f},{251.51f,163.01f},{250.64f,163.14f},{249.36f,163.86f},{248.49f,163.99f},{239.51f,164.01f},
    {238.64f,164.14f},{238.00f,164.50f},
};
constexpr COLORREF kStart8102GreenColor=RGB(122,169,52);

static const Gdiplus::PointF kStart8102_blue[]={
    {122.36f,259.84f},{121.63f,259.85f},{119.37f,258.13f},{114.63f,255.87f},{112.37f,254.13f},{102.36f,249.14f},
    {100.64f,248.86f},{95.36f,246.14f},{93.64f,245.86f},{90.36f,244.14f},{88.64f,243.86f},{87.36f,243.14f},
    {85.64f,242.86f},{84.36f,242.14f},{81.64f,241.86f},{80.36f,241.14f},{77.64f,240.86f},{76.36f,240.14f},
    {71.64f,239.86f},{70.36f,239.14f},{69.49f,239.01f},{60.51f,239.01f},{59.64f,239.14f},{58.36f,239.86f},
    {57.49f,239.99f},{50.51f,240.01f},{49.64f,240.14f},{48.36f,240.86f},{43.51f,241.01f},{42.64f,241.14f},
    {41.36f,241.86f},{36.51f,242.01f},{35.64f,242.14f},{34.36f,242.86f},{29.64f,243.14f},{28.36f,243.86f},
    {25.64f,244.14f},{24.36f,244.86f},{20.64f,245.14f},{19.36f,245.86f},{17.64f,246.14f},{16.36f,246.86f},
    {13.64f,247.14f},{12.36f,247.86f},{10.64f,248.14f},{9.36f,248.86f},{7.64f,249.14f},{0.51f,252.72f},
    {0.15f,252.35f},{0.15f,251.62f},{0.86f,250.36f},{1.14f,247.64f},{1.86f,246.36f},{2.14f,244.64f},
    {2.86f,243.36f},{3.14f,240.64f},{3.86f,239.36f},{4.14f,237.64f},{4.86f,236.36f},{5.14f,233.64f},
    {5.86f,232.36f},{6.14f,230.64f},{6.86f,229.36f},{7.14f,226.64f},{7.86f,225.36f},{8.14f,223.64f},
    {8.86f,222.36f},{9.14f,219.64f},{9.86f,218.36f},{10.14f,216.64f},{10.86f,215.36f},{11.14f,212.64f},
    {11.86f,211.36f},{12.14f,209.64f},{12.86f,208.36f},{13.14f,205.64f},{13.86f,204.36f},{14.14f,202.64f},
    {14.86f,201.36f},{15.14f,198.64f},{15.86f,197.36f},{16.14f,195.64f},{16.86f,194.36f},{17.14f,191.64f},
    {17.86f,190.36f},{18.14f,188.64f},{18.86f,187.36f},{19.14f,185.64f},{19.86f,184.36f},{20.14f,181.64f},
    {20.86f,180.36f},{21.14f,178.64f},{21.86f,177.36f},{22.14f,174.64f},{22.86f,173.36f},{23.14f,171.64f},
    {23.86f,170.36f},{24.14f,167.64f},{24.86f,166.36f},{25.14f,164.64f},{25.86f,163.36f},{26.14f,160.64f},
    {26.86f,159.36f},{27.14f,157.64f},{27.86f,156.36f},{28.14f,153.64f},{28.86f,152.36f},{29.14f,150.64f},
    {29.86f,149.36f},{30.14f,146.64f},{30.86f,145.36f},{31.14f,143.64f},{32.01f,142.51f},{32.64f,142.14f},
    {34.36f,141.86f},{37.64f,140.14f},{39.36f,139.86f},{42.64f,138.14f},{44.36f,137.86f},{45.64f,137.14f},
    {47.36f,136.86f},{48.64f,136.14f},{50.36f,135.86f},{51.64f,135.14f},{53.36f,134.86f},{54.64f,134.14f},
    {57.36f,133.86f},{58.64f,133.14f},{60.36f,132.86f},{61.64f,132.14f},{65.36f,131.86f},{66.64f,131.14f},
    {69.36f,130.86f},{70.64f,130.14f},{75.36f,129.86f},{76.64f,129.14f},{77.51f,129.01f},{83.49f,128.99f},
    {84.36f,128.86f},{85.64f,128.14f},{86.51f,128.01f},{95.49f,128.01f},{96.36f,128.14f},{97.64f,128.86f},
    {103.49f,129.01f},{104.36f,129.14f},{105.64f,129.86f},{109.36f,130.14f},{110.64f,130.86f},{114.36f,131.14f},
    {115.64f,131.86f},{117.36f,132.14f},{118.64f,132.86f},{120.36f,133.14f},{121.64f,133.86f},{123.36f,134.14f},
    {124.64f,134.86f},{126.36f,135.14f},{127.64f,135.86f},{129.36f,136.14f},{147.37f,145.13f},{149.01f,146.49f},
    {150.99f,147.51f},{152.01f,148.49f},{153.37f,149.13f},{154.48f,150.00f},{154.73f,150.50f},{154.14f,151.64f},
    {153.86f,153.36f},{153.14f,154.64f},{152.86f,157.36f},{152.14f,158.64f},{151.86f,160.36f},{151.14f,161.64f},
    {150.86f,164.36f},{150.14f,165.64f},{149.86f,167.36f},{149.14f,168.64f},{148.86f,170.36f},{148.14f,171.64f},
    {147.86f,174.36f},{147.14f,175.64f},{146.86f,177.36f},{146.14f,178.64f},{145.86f,181.36f},{145.14f,182.64f},
    {144.86f,184.36f},{144.14f,185.64f},{143.86f,188.36f},{143.14f,189.64f},{142.86f,191.36f},{142.14f,192.64f},
    {141.86f,195.36f},{141.14f,196.64f},{140.86f,198.36f},{140.14f,199.64f},{139.86f,201.36f},{139.14f,202.64f},
    {138.86f,205.36f},{138.14f,206.64f},{137.86f,208.36f},{137.14f,209.64f},{136.86f,212.36f},{136.14f,213.64f},
    {135.86f,215.36f},{135.14f,216.64f},{134.86f,219.36f},{134.14f,220.64f},{133.86f,222.36f},{133.14f,223.64f},
    {132.86f,225.36f},{132.14f,226.64f},{131.86f,229.36f},{131.14f,230.64f},{130.86f,232.36f},{130.14f,233.64f},
    {129.86f,236.36f},{129.14f,237.64f},{128.86f,239.36f},{128.14f,240.64f},{127.86f,243.36f},{127.14f,244.64f},
    {126.86f,246.36f},{126.14f,247.64f},{125.86f,250.36f},{125.14f,251.64f},{124.86f,253.36f},{124.14f,254.64f},
    {123.86f,256.36f},{123.14f,257.64f},{122.85f,259.35f},
};
constexpr COLORREF kStart8102BlueColor=RGB(0,173,237);

static const Gdiplus::PointF kStart8102_orange[]={
    {232.36f,287.86f},{173.64f,287.86f},{172.36f,287.14f},{170.64f,286.86f},{169.36f,286.14f},{167.64f,285.86f},
    {153.63f,278.87f},{151.99f,277.51f},{150.01f,276.49f},{148.99f,275.51f},{145.63f,273.87f},{143.99f,272.51f},
    {142.01f,271.49f},{140.14f,269.36f},{140.14f,267.64f},{140.86f,266.36f},{141.14f,263.64f},{141.86f,262.36f},
    {142.14f,260.64f},{142.86f,259.36f},{143.14f,257.64f},{143.86f,256.36f},{144.14f,253.64f},{144.86f,252.36f},
    {145.14f,250.64f},{145.86f,249.36f},{146.14f,246.64f},{146.86f,245.36f},{147.14f,243.64f},{147.86f,242.36f},
    {148.14f,239.64f},{148.86f,238.36f},{149.14f,236.64f},{149.86f,235.36f},{150.14f,232.64f},{150.86f,231.36f},
    {151.14f,229.64f},{151.86f,228.36f},{152.14f,225.64f},{152.86f,224.36f},{153.14f,222.64f},{153.86f,221.36f},
    {154.14f,218.64f},{154.86f,217.36f},{155.14f,215.64f},{155.86f,214.36f},{156.14f,212.64f},{156.86f,211.36f},
    {157.14f,208.64f},{157.86f,207.36f},{158.14f,205.64f},{158.86f,204.36f},{159.14f,201.64f},{159.86f,200.36f},
    {160.14f,198.64f},{160.86f,197.36f},{161.14f,194.64f},{161.86f,193.36f},{162.14f,191.64f},{162.86f,190.36f},
    {163.14f,187.64f},{163.86f,186.36f},{164.14f,184.64f},{164.86f,183.36f},{165.14f,180.64f},{165.86f,179.36f},
    {166.14f,177.64f},{166.86f,176.36f},{167.14f,173.64f},{167.86f,172.36f},{168.14f,170.64f},{168.86f,169.36f},
    {169.14f,167.64f},{169.86f,166.36f},{170.14f,163.64f},{171.51f,161.28f},{175.37f,163.13f},{177.63f,164.87f},
    {182.37f,167.13f},{184.63f,168.87f},{192.64f,172.86f},{194.36f,173.14f},{199.64f,175.86f},{201.36f,176.14f},
    {204.64f,177.86f},{206.36f,178.14f},{207.64f,178.86f},{209.36f,179.14f},{210.64f,179.86f},{213.36f,180.14f},
    {214.64f,180.86f},{217.36f,181.14f},{218.64f,181.86f},{219.51f,181.99f},{241.49f,181.99f},{242.36f,181.86f},
    {243.64f,181.14f},{244.51f,181.01f},{251.36f,180.86f},{252.64f,180.14f},{256.36f,179.86f},{257.64f,179.14f},
    {261.36f,178.86f},{262.64f,178.14f},{265.36f,177.86f},{266.64f,177.14f},{269.36f,176.86f},{270.64f,176.14f},
    {272.36f,175.86f},{273.64f,175.14f},{276.36f,174.86f},{277.64f,174.14f},{279.36f,173.86f},{280.64f,173.14f},
    {282.36f,172.86f},{285.64f,171.14f},{287.36f,170.86f},{288.64f,170.14f},{290.36f,169.86f},{293.49f,168.28f},
    {293.85f,168.65f},{293.85f,169.38f},{293.14f,170.64f},{292.86f,172.36f},{292.14f,173.64f},{291.86f,175.36f},
    {291.14f,176.64f},{290.86f,179.36f},{290.14f,180.64f},{289.86f,182.36f},{289.14f,183.64f},{288.86f,186.36f},
    {288.14f,187.64f},{287.86f,189.36f},{287.14f,190.64f},{286.86f,192.36f},{286.14f,193.64f},{285.86f,196.36f},
    {285.14f,197.64f},{284.86f,199.36f},{284.14f,200.64f},{283.86f,203.36f},{283.14f,204.64f},{282.86f,206.36f},
    {282.14f,207.64f},{281.86f,210.36f},{281.14f,211.64f},{280.86f,213.36f},{280.14f,214.64f},{279.86f,217.36f},
    {279.14f,218.64f},{278.86f,220.36f},{278.14f,221.64f},{277.86f,224.36f},{277.14f,225.64f},{276.86f,227.36f},
    {276.14f,228.64f},{275.86f,230.36f},{275.14f,231.64f},{274.86f,234.36f},{274.14f,235.64f},{273.86f,237.36f},
    {273.14f,238.64f},{272.86f,241.36f},{272.14f,242.64f},{271.86f,244.36f},{271.14f,245.64f},{270.86f,248.36f},
    {270.14f,249.64f},{269.86f,251.36f},{269.14f,252.64f},{268.86f,255.36f},{268.14f,256.64f},{267.86f,258.36f},
    {267.14f,259.64f},{266.86f,262.36f},{266.14f,263.64f},{265.86f,265.36f},{265.14f,266.64f},{264.86f,269.36f},
    {264.14f,270.64f},{263.86f,272.36f},{263.14f,273.64f},{262.86f,276.36f},{262.49f,276.99f},{261.36f,277.86f},
    {259.64f,278.14f},{256.36f,279.86f},{254.64f,280.14f},{251.36f,281.86f},{249.64f,282.14f},{248.36f,282.86f},
    {246.64f,283.14f},{245.36f,283.86f},{243.64f,284.14f},{242.36f,284.86f},{240.64f,285.14f},{239.36f,285.86f},
    {236.64f,286.14f},{235.36f,286.86f},{233.64f,287.14f},{233.00f,287.50f},
};
constexpr COLORREF kStart8102OrangeColor=RGB(250,175,64);
static_assert(_countof(kStart8102_red)<=kStart8102MaxPoints,"traced red pane outgrew the point buffer");
static_assert(_countof(kStart8102_green)<=kStart8102MaxPoints,"traced green pane outgrew the point buffer");
static_assert(_countof(kStart8102_blue)<=kStart8102MaxPoints,"traced blue pane outgrew the point buffer");
static_assert(_countof(kStart8102_orange)<=kStart8102MaxPoints,"traced orange pane outgrew the point buffer");
static_assert(kStart8102DesignWidth>0 && kStart8102DesignHeight>0,"Build 8102 design box must be positive");

// Draws the Build 8102 flag from those outlines instead of scaling a bitmap. The aspect fit is the one the
// sprite path used: at most 36 x 32 logical px (DPI scaled), centred in the caller's box. GDI+ fills the
// polygons with anti-aliasing and high-quality pixel offsets, so the edges stay clean at any size; a plain
// GDI polygon fill is kept as a degraded fallback if GDI+ is unavailable.
static bool Draw8102StartGlyph(HDC dc,int x,int y,int width,int height) {
    if(!dc || width<=0 || height<=0) return false;
    const int maxWidth=(std::min)(width,DpiScale(36));
    const int maxHeight=(std::min)(height,DpiScale(32));
    const Gdiplus::REAL scale=(std::min)((Gdiplus::REAL)maxWidth/kStart8102DesignWidth,
                                         (Gdiplus::REAL)maxHeight/kStart8102DesignHeight);
    if(scale<=0.0f) return false;
    const Gdiplus::REAL drawWidth=kStart8102DesignWidth*scale;
    const Gdiplus::REAL drawHeight=kStart8102DesignHeight*scale;
    const Gdiplus::REAL left=(Gdiplus::REAL)x+(width-drawWidth)/2.0f;
    const Gdiplus::REAL top=(Gdiplus::REAL)y+(height-drawHeight)/2.0f;

    struct Pane { const Gdiplus::PointF* points; int count; COLORREF color; };
    const Pane panes[4]={
        {kStart8102_red,   (int)_countof(kStart8102_red),   kStart8102RedColor},
        {kStart8102_green, (int)_countof(kStart8102_green), kStart8102GreenColor},
        {kStart8102_blue,  (int)_countof(kStart8102_blue),  kStart8102BlueColor},
        {kStart8102_orange,(int)_countof(kStart8102_orange),kStart8102OrangeColor}};

    const int saved=SaveDC(dc);
    if(!saved) return false;
    SetMapMode(dc,MM_TEXT);
    bool drawn=false;
    if(g_gdiplusToken) {
        Gdiplus::Graphics graphics(dc);
        if(graphics.GetLastStatus()==Gdiplus::Ok) {
            graphics.SetPageUnit(Gdiplus::UnitPixel);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
            graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
            graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
            graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            graphics.TranslateTransform(left,top);
            graphics.ScaleTransform(scale,scale);
            drawn=true;
            for(const Pane& pane : panes) {
                Gdiplus::GraphicsPath path(Gdiplus::FillModeAlternate);
                if(path.AddPolygon(pane.points,pane.count)!=Gdiplus::Ok || path.GetLastStatus()!=Gdiplus::Ok) {
                    drawn=false;
                    break;
                }
                Gdiplus::SolidBrush brush(Gdiplus::Color(255,GetRValue(pane.color),
                                                          GetGValue(pane.color),GetBValue(pane.color)));
                if(graphics.FillPath(&brush,&path)!=Gdiplus::Ok) {
                    drawn=false;
                    break;
                }
            }
        }
    }
    RestoreDC(dc,saved);
    if(drawn) return true;

    // Fallback: the same polygons through GDI, aliased but complete. The buffer size is a compile-time
    // constant and every pane is asserted against it, so no pane can be dropped here.
    POINT devicePoints[kStart8102MaxPoints];
    for(const Pane& pane : panes) {
        if(pane.count<3 || pane.count>kStart8102MaxPoints) continue;
        for(int i=0;i<pane.count;i++) {
            devicePoints[i].x=(LONG)std::lround(left+pane.points[i].X*scale);
            devicePoints[i].y=(LONG)std::lround(top+pane.points[i].Y*scale);
        }
        ScopedGdiObject brush(CreateSolidBrush(pane.color));
        if(!brush) continue;
        ScopedSelectObject selectedBrush(dc,brush.get());
        ScopedSelectObject selectedPen(dc,GetStockObject(NULL_PEN));
        if(selectedBrush) {
            Polygon(dc,devicePoints,pane.count);
            drawn=true;
        }
    }
    return drawn;
}

// GDI fallback used only if GDI+ cannot be initialized.
void DrawFallbackCharmIcon(HDC dc, int id, int x, int y, int width, int height) {
    if (!dc || id < 0 || id >= kCharmCount || width < 20 || height < 20) return;
    if(id==2 && g_startLogoStyle.load()==1 && Draw8102StartGlyph(dc,x,y,width,height)) return;
    const COLORREF color = id == 2 ? GetStartLogoColor() : RGB(255, 255, 255);
    ScopedGdiObject pen(CreatePen(PS_SOLID, 3, color));
    ScopedGdiObject fill(CreateSolidBrush(color));
    if (!pen || !fill) return;
    ScopedSelectObject selectedPen(dc,pen.get());
    if (!selectedPen) return;
    ScopedSelectObject selectedBrush(dc,id == 2 ? fill.get() : GetStockObject(HOLLOW_BRUSH));
    if (!selectedBrush) return;
    const int cx=x+width/2, cy=y+height/2;
    switch(id) {
        case 0:
            Ellipse(dc,x+11,y+2,x+41,y+32);
            MoveToEx(dc,x+18,y+26,nullptr); LineTo(dc,x+2,y+42);
            break;
        case 1: {
            const POINT nodes[3]={{x+6,y+22},{x+32,y+7},{x+32,y+37}};
            MoveToEx(dc,nodes[0].x,nodes[0].y,nullptr); LineTo(dc,nodes[1].x,nodes[1].y);
            MoveToEx(dc,nodes[0].x,nodes[0].y,nullptr); LineTo(dc,nodes[2].x,nodes[2].y);
            for(const POINT& n:nodes) Ellipse(dc,n.x-6,n.y-6,n.x+6,n.y+6);
            break;
        }
        case 2: {
            const POINT p0[4]={{x+1,y+8},{x+18,y+5},{x+18,y+21},{x+1,y+21}};
            const POINT p1[4]={{x+21,y+4},{x+43,y+1},{x+43,y+21},{x+21,y+21}};
            const POINT p2[4]={{x+1,y+24},{x+18,y+24},{x+18,y+39},{x+1,y+36}};
            const POINT p3[4]={{x+21,y+24},{x+43,y+24},{x+43,y+43},{x+21,y+40}};
            Polygon(dc,p0,4); Polygon(dc,p1,4); Polygon(dc,p2,4); Polygon(dc,p3,4);
            break;
        }
        case 3:
            MoveToEx(dc,x+12,y+5,nullptr); LineTo(dc,x+30,y+5);
            Arc(dc,x+28,y+4,x+42,y+18,x+39,y+9,x+42,y+15);
            LineTo(dc,x+40,y+29); Arc(dc,x+28,y+27,x+42,y+41,x+42,y+34,x+37,y+40);
            LineTo(dc,x+12,y+39);
            Rectangle(dc,x+1,y+15,x+16,y+29);
            break;
        default: {
            static const int radii[32]={15,15,20,20,15,15,15,15, 20,20,15,15,15,15,20,20,
                                        15,15,15,15,20,20,15,15, 15,15,20,20,15,15,15,15};
            POINT pts[32]{};
            for(int i=0;i<32;i++) {
                const double angle=(-90.0+i*11.25)*3.14159265358979323846/180.0;
                pts[i]={cx+(int)std::lround(std::cos(angle)*radii[i]),
                        cy+(int)std::lround(std::sin(angle)*radii[i])};
            }
            Polygon(dc,pts,32);
            ScopedSelectObject hollow(dc,GetStockObject(HOLLOW_BRUSH));
            Ellipse(dc,cx-8,cy-8,cx+9,cy+9);
            break;
        }
    }
}

// High-quality, manually traced Windows 8 glyphs. The paths are drawn at logical icon dimensions and scaled
// with the active monitor's DPI.
struct VectorContourData { const Gdiplus::PointF* points; int count; };
struct VectorLayerData { const VectorContourData* contours; int count; COLORREF color; bool useTint; };
struct VectorGlyphData { const VectorLayerData* layers; int count; };
static const Gdiplus::PointF kGlyphDevices_white_contour0[]={ {3.58f,15.86f}, {3.58f,26.60f}, {4.09f,27.12f}, {18.93f,27.12f}, {19.44f,26.60f}, {19.44f,15.86f}, {18.93f,15.35f}, {4.09f,15.35f} };
static const Gdiplus::PointF kGlyphDevices_white_contour1[]={ {12.28f,8.19f}, {10.74f,9.72f}, {10.23f,11.26f}, {30.70f,11.26f}, {32.74f,12.28f}, {34.79f,15.35f}, {34.79f,27.12f}, {33.77f,29.16f}, {30.70f,31.21f}, {10.23f,31.21f}, {10.74f,32.74f}, {12.28f,34.28f}, {14.84f,35.30f}, {32.74f,35.30f}, {34.79f,34.28f}, {37.86f,31.21f}, {38.88f,28.65f}, {38.88f,13.81f}, {37.86f,11.26f}, {34.79f,8.19f}, {32.74f,7.16f}, {14.84f,7.16f} };
static const Gdiplus::PointF kGlyphBattery0_white_contour0[]={ {26.00f,0.00f}, {21.00f,0.00f}, {17.00f,4.00f}, {12.00f,4.00f}, {12.00f,40.00f}, {19.00f,40.00f}, {20.00f,39.00f}, {14.00f,39.00f}, {13.00f,38.00f}, {13.00f,6.00f}, {14.00f,5.00f}, {31.00f,5.00f}, {32.00f,6.00f}, {32.00f,26.00f}, {33.00f,26.00f}, {33.00f,4.00f}, {30.00f,4.00f} };
static const VectorContourData kGlyphBattery0_white_contours[]={ {kGlyphBattery0_white_contour0,_countof(kGlyphBattery0_white_contour0)} };
static const VectorLayerData kGlyphBattery0_white_layer={ kGlyphBattery0_white_contours,_countof(kGlyphBattery0_white_contours),RGB(255,255,255),false };
static const Gdiplus::PointF kGlyphBattery0_red_contour0[]={ {24.00f,28.00f}, {30.00f,34.00f}, {24.00f,40.00f}, {24.00f,41.00f}, {25.00f,42.00f}, {27.00f,42.00f}, {32.00f,37.00f}, {33.00f,37.00f}, {37.00f,41.00f}, {37.00f,42.00f}, {39.00f,42.00f}, {40.00f,41.00f}, {41.00f,41.00f}, {41.00f,40.00f}, {35.00f,34.00f}, {41.00f,28.00f}, {39.00f,26.00f}, {38.00f,26.00f}, {33.00f,31.00f}, {32.00f,31.00f}, {27.00f,26.00f}, {26.00f,26.00f} };
static const VectorContourData kGlyphBattery0_red_contours[]={ {kGlyphBattery0_red_contour0,_countof(kGlyphBattery0_red_contour0)} };
static const VectorLayerData kGlyphBattery0_red_layer={ kGlyphBattery0_red_contours,_countof(kGlyphBattery0_red_contours),RGB(234,38,39),false };
static const VectorLayerData kGlyphBattery0_layers[]={ kGlyphBattery0_white_layer, kGlyphBattery0_red_layer };
static const VectorGlyphData kGlyphBattery0={ kGlyphBattery0_layers,_countof(kGlyphBattery0_layers) };
static const Gdiplus::PointF kGlyphBattery1_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBattery1_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const VectorContourData kGlyphBattery1_white_contours[]={ {kGlyphBattery1_white_contour0,_countof(kGlyphBattery1_white_contour0)}, {kGlyphBattery1_white_contour1,_countof(kGlyphBattery1_white_contour1)} };
static const VectorLayerData kGlyphBattery1_white_layer={ kGlyphBattery1_white_contours,_countof(kGlyphBattery1_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBattery1_layers[]={ kGlyphBattery1_white_layer };
static const VectorGlyphData kGlyphBattery1={ kGlyphBattery1_layers,_countof(kGlyphBattery1_layers) };
static const Gdiplus::PointF kGlyphBattery5_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBattery5_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const Gdiplus::PointF kGlyphBattery5_white_contour2[]={ {17.00f,35.00f}, {17.00f,37.00f}, {30.00f,37.00f}, {30.00f,35.00f} };
static const VectorContourData kGlyphBattery5_white_contours[]={ {kGlyphBattery5_white_contour0,_countof(kGlyphBattery5_white_contour0)}, {kGlyphBattery5_white_contour1,_countof(kGlyphBattery5_white_contour1)}, {kGlyphBattery5_white_contour2,_countof(kGlyphBattery5_white_contour2)} };
static const VectorLayerData kGlyphBattery5_white_layer={ kGlyphBattery5_white_contours,_countof(kGlyphBattery5_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBattery5_layers[]={ kGlyphBattery5_white_layer };
static const VectorGlyphData kGlyphBattery5={ kGlyphBattery5_layers,_countof(kGlyphBattery5_layers) };
static const Gdiplus::PointF kGlyphBattery10_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBattery10_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const Gdiplus::PointF kGlyphBattery10_white_contour2[]={ {17.00f,32.00f}, {17.00f,37.00f}, {30.00f,37.00f}, {30.00f,32.00f} };
static const VectorContourData kGlyphBattery10_white_contours[]={ {kGlyphBattery10_white_contour0,_countof(kGlyphBattery10_white_contour0)}, {kGlyphBattery10_white_contour1,_countof(kGlyphBattery10_white_contour1)}, {kGlyphBattery10_white_contour2,_countof(kGlyphBattery10_white_contour2)} };
static const VectorLayerData kGlyphBattery10_white_layer={ kGlyphBattery10_white_contours,_countof(kGlyphBattery10_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBattery10_layers[]={ kGlyphBattery10_white_layer };
static const VectorGlyphData kGlyphBattery10={ kGlyphBattery10_layers,_countof(kGlyphBattery10_layers) };
static const Gdiplus::PointF kGlyphBattery20_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBattery20_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const Gdiplus::PointF kGlyphBattery20_white_contour2[]={ {17.00f,29.00f}, {17.00f,37.00f}, {30.00f,37.00f}, {30.00f,29.00f} };
static const VectorContourData kGlyphBattery20_white_contours[]={ {kGlyphBattery20_white_contour0,_countof(kGlyphBattery20_white_contour0)}, {kGlyphBattery20_white_contour1,_countof(kGlyphBattery20_white_contour1)}, {kGlyphBattery20_white_contour2,_countof(kGlyphBattery20_white_contour2)} };
static const VectorLayerData kGlyphBattery20_white_layer={ kGlyphBattery20_white_contours,_countof(kGlyphBattery20_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBattery20_layers[]={ kGlyphBattery20_white_layer };
static const VectorGlyphData kGlyphBattery20={ kGlyphBattery20_layers,_countof(kGlyphBattery20_layers) };
static const Gdiplus::PointF kGlyphBattery30_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBattery30_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const Gdiplus::PointF kGlyphBattery30_white_contour2[]={ {17.00f,26.00f}, {17.00f,37.00f}, {30.00f,37.00f}, {30.00f,26.00f} };
static const VectorContourData kGlyphBattery30_white_contours[]={ {kGlyphBattery30_white_contour0,_countof(kGlyphBattery30_white_contour0)}, {kGlyphBattery30_white_contour1,_countof(kGlyphBattery30_white_contour1)}, {kGlyphBattery30_white_contour2,_countof(kGlyphBattery30_white_contour2)} };
static const VectorLayerData kGlyphBattery30_white_layer={ kGlyphBattery30_white_contours,_countof(kGlyphBattery30_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBattery30_layers[]={ kGlyphBattery30_white_layer };
static const VectorGlyphData kGlyphBattery30={ kGlyphBattery30_layers,_countof(kGlyphBattery30_layers) };
static const Gdiplus::PointF kGlyphBattery40_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBattery40_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const Gdiplus::PointF kGlyphBattery40_white_contour2[]={ {17.00f,23.00f}, {17.00f,37.00f}, {30.00f,37.00f}, {30.00f,23.00f} };
static const VectorContourData kGlyphBattery40_white_contours[]={ {kGlyphBattery40_white_contour0,_countof(kGlyphBattery40_white_contour0)}, {kGlyphBattery40_white_contour1,_countof(kGlyphBattery40_white_contour1)}, {kGlyphBattery40_white_contour2,_countof(kGlyphBattery40_white_contour2)} };
static const VectorLayerData kGlyphBattery40_white_layer={ kGlyphBattery40_white_contours,_countof(kGlyphBattery40_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBattery40_layers[]={ kGlyphBattery40_white_layer };
static const VectorGlyphData kGlyphBattery40={ kGlyphBattery40_layers,_countof(kGlyphBattery40_layers) };
static const Gdiplus::PointF kGlyphBattery50_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBattery50_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const Gdiplus::PointF kGlyphBattery50_white_contour2[]={ {17.00f,20.00f}, {17.00f,37.00f}, {30.00f,37.00f}, {30.00f,20.00f} };
static const VectorContourData kGlyphBattery50_white_contours[]={ {kGlyphBattery50_white_contour0,_countof(kGlyphBattery50_white_contour0)}, {kGlyphBattery50_white_contour1,_countof(kGlyphBattery50_white_contour1)}, {kGlyphBattery50_white_contour2,_countof(kGlyphBattery50_white_contour2)} };
static const VectorLayerData kGlyphBattery50_white_layer={ kGlyphBattery50_white_contours,_countof(kGlyphBattery50_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBattery50_layers[]={ kGlyphBattery50_white_layer };
static const VectorGlyphData kGlyphBattery50={ kGlyphBattery50_layers,_countof(kGlyphBattery50_layers) };
static const Gdiplus::PointF kGlyphBattery60_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBattery60_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const Gdiplus::PointF kGlyphBattery60_white_contour2[]={ {17.00f,17.00f}, {17.00f,37.00f}, {30.00f,37.00f}, {30.00f,17.00f} };
static const VectorContourData kGlyphBattery60_white_contours[]={ {kGlyphBattery60_white_contour0,_countof(kGlyphBattery60_white_contour0)}, {kGlyphBattery60_white_contour1,_countof(kGlyphBattery60_white_contour1)}, {kGlyphBattery60_white_contour2,_countof(kGlyphBattery60_white_contour2)} };
static const VectorLayerData kGlyphBattery60_white_layer={ kGlyphBattery60_white_contours,_countof(kGlyphBattery60_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBattery60_layers[]={ kGlyphBattery60_white_layer };
static const VectorGlyphData kGlyphBattery60={ kGlyphBattery60_layers,_countof(kGlyphBattery60_layers) };
static const Gdiplus::PointF kGlyphBattery70_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBattery70_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const Gdiplus::PointF kGlyphBattery70_white_contour2[]={ {17.00f,14.00f}, {17.00f,37.00f}, {30.00f,37.00f}, {30.00f,14.00f} };
static const VectorContourData kGlyphBattery70_white_contours[]={ {kGlyphBattery70_white_contour0,_countof(kGlyphBattery70_white_contour0)}, {kGlyphBattery70_white_contour1,_countof(kGlyphBattery70_white_contour1)}, {kGlyphBattery70_white_contour2,_countof(kGlyphBattery70_white_contour2)} };
static const VectorLayerData kGlyphBattery70_white_layer={ kGlyphBattery70_white_contours,_countof(kGlyphBattery70_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBattery70_layers[]={ kGlyphBattery70_white_layer };
static const VectorGlyphData kGlyphBattery70={ kGlyphBattery70_layers,_countof(kGlyphBattery70_layers) };
static const Gdiplus::PointF kGlyphBattery80_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBattery80_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const Gdiplus::PointF kGlyphBattery80_white_contour2[]={ {17.00f,11.00f}, {17.00f,37.00f}, {30.00f,37.00f}, {30.00f,11.00f} };
static const VectorContourData kGlyphBattery80_white_contours[]={ {kGlyphBattery80_white_contour0,_countof(kGlyphBattery80_white_contour0)}, {kGlyphBattery80_white_contour1,_countof(kGlyphBattery80_white_contour1)}, {kGlyphBattery80_white_contour2,_countof(kGlyphBattery80_white_contour2)} };
static const VectorLayerData kGlyphBattery80_white_layer={ kGlyphBattery80_white_contours,_countof(kGlyphBattery80_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBattery80_layers[]={ kGlyphBattery80_white_layer };
static const VectorGlyphData kGlyphBattery80={ kGlyphBattery80_layers,_countof(kGlyphBattery80_layers) };
static const Gdiplus::PointF kGlyphBattery90_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBattery90_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const Gdiplus::PointF kGlyphBattery90_white_contour2[]={ {17.00f,11.00f}, {17.00f,37.00f}, {30.00f,37.00f}, {30.00f,11.00f} };
static const VectorContourData kGlyphBattery90_white_contours[]={ {kGlyphBattery90_white_contour0,_countof(kGlyphBattery90_white_contour0)}, {kGlyphBattery90_white_contour1,_countof(kGlyphBattery90_white_contour1)}, {kGlyphBattery90_white_contour2,_countof(kGlyphBattery90_white_contour2)} };
static const VectorLayerData kGlyphBattery90_white_layer={ kGlyphBattery90_white_contours,_countof(kGlyphBattery90_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBattery90_layers[]={ kGlyphBattery90_white_layer };
static const VectorGlyphData kGlyphBattery90={ kGlyphBattery90_layers,_countof(kGlyphBattery90_layers) };
static const Gdiplus::PointF kGlyphBatteryFull_white_contour0[]={ {21.00f,0.00f}, {17.00f,4.00f}, {13.00f,4.00f}, {13.00f,41.00f}, {34.00f,41.00f}, {34.00f,4.00f}, {30.00f,4.00f}, {26.00f,0.00f} };
static const Gdiplus::PointF kGlyphBatteryFull_white_contour1[]={ {15.00f,5.00f}, {32.00f,5.00f}, {33.00f,6.00f}, {33.00f,39.00f}, {32.00f,40.00f}, {15.00f,40.00f}, {14.00f,39.00f}, {14.00f,6.00f} };
static const Gdiplus::PointF kGlyphBatteryFull_white_contour2[]={ {17.00f,8.00f}, {17.00f,37.00f}, {30.00f,37.00f}, {30.00f,8.00f} };
static const VectorContourData kGlyphBatteryFull_white_contours[]={ {kGlyphBatteryFull_white_contour0,_countof(kGlyphBatteryFull_white_contour0)}, {kGlyphBatteryFull_white_contour1,_countof(kGlyphBatteryFull_white_contour1)}, {kGlyphBatteryFull_white_contour2,_countof(kGlyphBatteryFull_white_contour2)} };
static const VectorLayerData kGlyphBatteryFull_white_layer={ kGlyphBatteryFull_white_contours,_countof(kGlyphBatteryFull_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphBatteryFull_layers[]={ kGlyphBatteryFull_white_layer };
static const VectorGlyphData kGlyphBatteryFull={ kGlyphBatteryFull_layers,_countof(kGlyphBatteryFull_layers) };
static const Gdiplus::PointF kGlyphCharging_white_contour0[]={ {0.00f,14.00f}, {0.00f,22.00f}, {2.00f,24.00f}, {3.00f,24.00f}, {4.00f,25.00f}, {4.00f,41.00f}, {6.00f,41.00f}, {6.00f,25.00f}, {7.00f,24.00f}, {8.00f,24.00f}, {10.00f,22.00f}, {10.00f,20.00f}, {11.00f,19.00f}, {10.00f,18.00f}, {10.00f,14.00f} };
static const Gdiplus::PointF kGlyphCharging_white_contour1[]={ {7.00f,5.00f}, {7.00f,11.00f}, {9.00f,11.00f}, {9.00f,5.00f} };
static const Gdiplus::PointF kGlyphCharging_white_contour2[]={ {1.00f,5.00f}, {1.00f,11.00f}, {3.00f,11.00f}, {3.00f,5.00f} };
static const VectorContourData kGlyphCharging_white_contours[]={ {kGlyphCharging_white_contour0,_countof(kGlyphCharging_white_contour0)}, {kGlyphCharging_white_contour1,_countof(kGlyphCharging_white_contour1)}, {kGlyphCharging_white_contour2,_countof(kGlyphCharging_white_contour2)} };
static const VectorLayerData kGlyphCharging_white_layer={ kGlyphCharging_white_contours,_countof(kGlyphCharging_white_contours),RGB(255,255,255),false };
static const VectorLayerData kGlyphCharging_layers[]={ kGlyphCharging_white_layer };
static const VectorGlyphData kGlyphCharging={ kGlyphCharging_layers,_countof(kGlyphCharging_layers) };

template<typename DrawFunction>
bool WithGdiplusCanvas(HDC dc,int x,int y,int width,int height,
                       Gdiplus::REAL sourceWidth,Gdiplus::REAL sourceHeight, DrawFunction draw) {
    if(!dc || width<=0 || height<=0 || sourceWidth<=0 || sourceHeight<=0 || !g_gdiplusToken)
        return false;
    const int saved=SaveDC(dc);
    if(!saved) return false;
    Gdiplus::REAL scaleX=1.0f,scaleY=1.0f;
    const int mapMode=GetMapMode(dc);
    if(mapMode==MM_ANISOTROPIC || mapMode==MM_ISOTROPIC) {
        SIZE windowExtent{},viewportExtent{};
        if(GetWindowExtEx(dc,&windowExtent) && GetViewportExtEx(dc,&viewportExtent) &&
           windowExtent.cx && windowExtent.cy) {
            scaleX=(Gdiplus::REAL)viewportExtent.cx/windowExtent.cx;
            scaleY=(Gdiplus::REAL)viewportExtent.cy/windowExtent.cy;
        }
    }
    SetMapMode(dc,MM_TEXT);
    bool drawn=false;
    {
        Gdiplus::Graphics graphics(dc);
        if(graphics.GetLastStatus()==Gdiplus::Ok) {
            graphics.SetPageUnit(Gdiplus::UnitPixel);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
            graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
            graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
            graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
            graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            const Gdiplus::Matrix transform(scaleX*width/sourceWidth,0,0,scaleY*height/sourceHeight,
                scaleX*(Gdiplus::REAL)x,scaleY*(Gdiplus::REAL)y);
            if(graphics.SetTransform(&transform)==Gdiplus::Ok)
                drawn=draw(graphics);
        }
    }
    RestoreDC(dc,saved);
    return drawn;
}

bool DrawVectorGlyphGdiplus(HDC dc,const VectorGlyphData& glyph,
                            int x,int y,int width,int height,COLORREF tint) {
    return WithGdiplusCanvas(dc,x,y,width,height,44.0f,44.0f,
        [&glyph,tint](Gdiplus::Graphics& graphics)->bool {
            bool drawn=false;
            for(int layerIndex=0;layerIndex<glyph.count;layerIndex++) {
                const VectorLayerData& layer=glyph.layers[layerIndex];
                Gdiplus::GraphicsPath path(Gdiplus::FillModeAlternate);
                bool hasGeometry=false;
                for(int contourIndex=0;contourIndex<layer.count;contourIndex++) {
                    const VectorContourData& contour=layer.contours[contourIndex];
                    if(contour.count>=3 && path.AddPolygon(contour.points,contour.count)==Gdiplus::Ok)
                        hasGeometry=true;
                }
                if(!hasGeometry) continue;
                const COLORREF color=layer.useTint?tint:layer.color;
                Gdiplus::SolidBrush brush(Gdiplus::Color(255,GetRValue(color),
                                                          GetGValue(color),GetBValue(color)));
                if(graphics.FillPath(&brush,&path)==Gdiplus::Ok) drawn=true;
            }
            return drawn;
        });
}

// The Share charm is deliberately a single opaque white fill. It uses the supplied Share contours but does
// not add a grey/secondary layer or tint. Share, Windows 8/8.1 regular: three nodes on a ring joined by three
// arcs. Redrawn from the Windows 8/8.1 Share charm (charms bar reference capture) in the 44 logical px design
// box, so it is a reconstruction of that look, not a pixel copy of the original glyph.
static bool DrawShareGlyphMonochrome(HDC dc,int x,int y,int width,int height) {
    return WithGdiplusCanvas(dc,x,y,width,height,44.0f,44.0f,
        [](Gdiplus::Graphics& g)->bool {
            const float cx = 22.0f, cy = 22.0f;  // centre of the nodes
            const float ring = 13.9f;            // radius of the node circle
            const float node = 4.6f;             // radius of each node dot
            const float stroke = 3.0f;           // width of each arc
            const float nodeAngle[3] = { -60.0f, 60.0f, 180.0f };   // degrees, screen space
            Gdiplus::SolidBrush white(Gdiplus::Color(255, 255, 255, 255));
            Gdiplus::Pen pen(Gdiplus::Color(255, 255, 255, 255), stroke);
            bool ok = true;
            // Each arc joins two nodes and stops 18 degrees short of each one.
            for (float a : nodeAngle) {
                ok = (g.DrawArc(&pen, cx - ring, cy - ring, 2.0f * ring, 2.0f * ring,
                                a + 18.0f, 84.0f) == Gdiplus::Ok) && ok;
            }
            const float pi = 3.14159265f;
            for (float a : nodeAngle) {
                const float rad = a * pi / 180.0f;
                ok = (g.FillEllipse(&white, cx + ring * cosf(rad) - node, cy + ring * sinf(rad) - node,
                                    2.0f * node, 2.0f * node) == Gdiplus::Ok) && ok;
            }
            return ok;
        });
}

static Gdiplus::Color GdipColor(COLORREF c) {
    return Gdiplus::Color(255, GetRValue(c), GetGValue(c), GetBValue(c));
}

// Search: true circular lens ring + round-capped handle (no polygon facets).
static bool DrawSearchGlyph(Gdiplus::Graphics& g, COLORREF color) {
    Gdiplus::SolidBrush brush(GdipColor(color));
    Gdiplus::GraphicsPath ring(Gdiplus::FillModeAlternate);
    ring.AddEllipse(14.8f, 4.1f, 24.6f, 24.6f);
    ring.AddEllipse(17.9f, 7.2f, 18.4f, 18.4f);
    bool ok = g.FillPath(&brush, &ring) == Gdiplus::Ok;
    Gdiplus::Pen handle(GdipColor(color), 4.4f);
    handle.SetStartCap(Gdiplus::LineCapRound);
    handle.SetEndCap(Gdiplus::LineCapFlat);
    ok = (g.DrawLine(&handle, 5.8f, 37.8f, 18.8f, 24.8f) == Gdiplus::Ok) && ok;
    return ok;
}

// Start: the Windows 8 four-pane flag with clean straight edges and the perspective of the original mark.
// Colour = system accent by default.
static bool DrawStartGlyphWithBrush(Gdiplus::Graphics& g, Gdiplus::Brush* brush) {
    static const Gdiplus::PointF panes[4][4] = {
        {{2.6f, 7.7f},  {17.4f, 5.6f},  {17.4f, 20.5f}, {2.6f, 20.5f}},
        {{19.95f, 5.1f},{40.4f, 2.6f},  {40.4f, 20.5f}, {19.95f, 20.5f}},
        {{2.6f, 23.0f}, {17.4f, 23.0f}, {17.4f, 37.9f}, {2.6f, 35.8f}},
        {{19.95f, 23.0f},{40.4f, 23.0f},{40.4f, 40.9f}, {19.95f, 38.4f}}};
    if(!brush) return false;
    Gdiplus::GraphicsPath path(Gdiplus::FillModeAlternate);
    for (const auto& pane : panes) { path.StartFigure(); path.AddPolygon(pane, 4); }
    return g.FillPath(brush, &path) == Gdiplus::Ok;
}

static bool DrawStartGlyph(Gdiplus::Graphics& g, COLORREF color) {
    Gdiplus::SolidBrush brush(GdipColor(color));
    return DrawStartGlyphWithBrush(g, &brush);
}

// Settings: eight-tooth gear generated geometrically with a true circular hub hole.
static bool DrawSettingsGlyph(Gdiplus::Graphics& g, COLORREF color) {
    const double pi = 3.14159265358979323846;
    const int teeth = 8;
    const double step = 2.0 * pi / teeth, cx = 21.75, cy = 21.75;
    const double rOut = 17.3, rRoot = 13.5, wOut = step * 0.15, wRoot = step * 0.24;
    Gdiplus::PointF pts[teeth * 8];
    int n = 0;
    auto at = [&](double radius, double a) {
        return Gdiplus::PointF((Gdiplus::REAL)(cx + std::cos(a) * radius),
                               (Gdiplus::REAL)(cy + std::sin(a) * radius));
    };
    for (int t = 0; t < teeth; ++t) {
        const double ac = -pi / 2 + t * step;
        const double nextRootStart = ac + step - wRoot;
        pts[n++] = at(rRoot, ac - wRoot);
        pts[n++] = at(rOut, ac - wOut);
        pts[n++] = at(rOut, ac);
        pts[n++] = at(rOut, ac + wOut);
        pts[n++] = at(rRoot, ac + wRoot);
        for (int k = 1; k <= 3; ++k)
            pts[n++] = at(rRoot, ac + wRoot + (nextRootStart - (ac + wRoot)) * k / 4.0);
    }
    Gdiplus::SolidBrush brush(GdipColor(color));
    Gdiplus::GraphicsPath path(Gdiplus::FillModeAlternate);
    path.AddPolygon(pts, n);
    path.AddEllipse((Gdiplus::REAL)(cx - 8.9), (Gdiplus::REAL)(cy - 8.9), 17.8f, 17.8f);
    return g.FillPath(&brush, &path) == Gdiplus::Ok;
}

// Devices: rounded display outline with a small device overlapping its left edge.
static bool DrawDevicesGlyph(Gdiplus::Graphics& g, COLORREF color) {
    Gdiplus::SolidBrush brush(GdipColor(color));
    Gdiplus::GraphicsPath ring(Gdiplus::FillModeAlternate);
    auto rounded=[](Gdiplus::GraphicsPath& p,float x,float y,float w,float h,float r) {
        p.AddArc(x,y,2*r,2*r,180,90); p.AddArc(x+w-2*r,y,2*r,2*r,270,90);
        p.AddArc(x+w-2*r,y+h-2*r,2*r,2*r,0,90); p.AddArc(x,y+h-2*r,2*r,2*r,90,90);
        p.CloseFigure();
    };
    rounded(ring,10.2f,7.2f,28.7f,28.1f,4.5f);
    rounded(ring,14.3f,11.3f,20.5f,19.9f,1.5f);
    g.SetClip(Gdiplus::RectF(6.8f,12.8f,15.4f,17.0f),Gdiplus::CombineModeExclude);
    bool ok=g.FillPath(&brush,&ring)==Gdiplus::Ok;
    g.ResetClip();
    ok=(g.FillRectangle(&brush,Gdiplus::RectF(3.6f,15.4f,15.8f,11.7f))==Gdiplus::Ok) && ok;
    return ok;
}

// ---------------------------------------------------------------- Windows 8 Build 8102 beta charm icons
// Search, Share, Devices and Settings icons from the Windows 8 Build 8102 desktop capture (75 % scale). The
// capture is too small to draw crisply as a bitmap, so each icon was traced once into vector outlines
// (sub-pixel contours of its alpha mask, simplified to ~0.03 px) and is now filled with GDI+ anti-aliasing at
// the active DPI, like the other charm icons. The outlines use the same 44 logical px box as the regular
// icons. The Start icon is the logo and is not included.

static const Gdiplus::PointF kBuild8102Search_contour0[]={ {6.208f,42.133f},{5.375f,42.172f},{4.292f,42.004f},{3.641f,41.708f},{3.126f,41.292f},{2.825f,40.792f},{2.391f,39.542f},{2.362f,38.875f},{2.523f,38.208f},{2.794f,37.542f},{3.471f,36.458f},{4.817f,34.875f},{5.542f,34.150f},{6.560f,33.292f},{7.975f,31.875f},{8.894f,30.792f},{9.542f,30.150f},{10.560f,29.292f},{12.988f,26.792f},{13.458f,26.393f},{13.875f,26.187f},{14.792f,25.922f},{15.625f,25.822f},{15.958f,25.711f},{16.375f,25.376f},{16.605f,25.042f},{16.813f,24.292f},{16.745f,23.625f},{16.497f,23.042f},{15.874f,22.125f},{15.405f,21.208f},{14.724f,18.458f},{14.637f,16.958f},{14.681f,13.958f},{14.932f,12.542f},{15.440f,10.708f},{15.925f,9.792f},{16.437f,9.042f},{16.850f,8.125f},{17.790f,6.792f},{18.958f,5.676f},{21.708f,3.584f},{24.625f,2.371f},{25.625f,2.105f},{26.875f,2.005f},{30.625f,2.014f},{31.792f,2.123f},{32.792f,2.406f},{35.708f,3.636f},{38.542f,5.839f},{39.527f,6.792f},{39.875f,7.244f},{40.467f,8.125f},{40.882f,9.042f},{41.339f,9.708f},{41.833f,10.625f},{42.356f,12.458f},{42.627f,14.042f},{42.668f,16.958f},{42.609f,18.208f},{42.375f,19.460f},{42.116f,20.292f},{41.964f,21.042f},{41.485f,22.042f},{40.882f,22.958f},{40.467f,23.875f},{39.527f,25.208f},{38.458f,26.232f},{35.708f,28.357f},{32.875f,29.565f},{31.625f,29.905f},{30.292f,30.001f},{27.208f,30.002f},{25.542f,29.881f},{24.458f,29.573f},{23.625f,29.175f},{22.875f,28.915f},{20.625f,27.808f},{20.208f,27.730f},{19.875f,27.800f},{19.542f,28.032f},{19.356f,28.292f},{18.542f,30.397f},{17.938f,31.375f},{16.415f,33.208f},{15.792f,33.824f},{14.741f,34.708f},{13.327f,36.125f},{12.415f,37.208f},{11.792f,37.824f},{10.741f,38.708f},{8.226f,41.292f},{7.875f,41.589f},{7.542f,41.773f},{6.208f,42.133f} };
static const Gdiplus::PointF kBuild8102Search_contour1[]={ {30.207f,25.792f},{31.542f,25.445f},{33.208f,24.687f},{34.542f,23.844f},{35.958f,22.555f},{36.493f,21.958f},{37.743f,19.875f},{38.224f,18.208f},{38.439f,17.042f},{38.412f,14.708f},{37.812f,12.292f},{36.981f,10.792f},{36.213f,9.708f},{34.458f,8.092f},{33.042f,7.226f},{31.625f,6.586f},{30.542f,6.258f},{28.708f,6.149f},{26.875f,6.242f},{25.542f,6.649f},{24.208f,7.252f},{22.958f,7.990f},{21.375f,9.430f},{20.792f,10.087f},{19.577f,12.125f},{19.093f,13.792f},{18.879f,14.958f},{18.906f,17.292f},{19.509f,19.708f},{20.285f,21.125f},{21.028f,22.208f},{22.792f,23.894f},{24.292f,24.788f},{25.708f,25.420f},{26.708f,25.722f},{28.458f,25.849f},{30.207f,25.792f} };
static const VectorContourData kBuild8102Search_contours[]={ {kBuild8102Search_contour0,95}, {kBuild8102Search_contour1,37} };
static const VectorLayerData kBuild8102Search_layer={ kBuild8102Search_contours,_countof(kBuild8102Search_contours),RGB(255,255,255),false };
static const VectorLayerData kBuild8102Search_layers[]={ kBuild8102Search_layer };
static const VectorGlyphData kBuild8102Search={ kBuild8102Search_layers,_countof(kBuild8102Search_layers) };
static const Gdiplus::PointF kBuild8102Share_contour0[]={ {3.542f,19.126f},{2.875f,19.140f},{2.375f,18.962f},{1.983f,18.625f},{1.817f,18.208f},{1.723f,17.042f},{1.704f,13.625f},{1.902f,12.792f},{2.208f,12.286f},{2.558f,12.042f},{2.958f,11.933f},{6.375f,12.031f},{7.338f,11.875f},{7.792f,11.732f},{8.208f,11.396f},{8.875f,10.440f},{9.375f,9.913f},{10.280f,9.208f},{10.875f,8.658f},{12.292f,7.688f},{14.958f,6.275f},{16.875f,5.741f},{17.792f,5.386f},{19.042f,5.070f},{20.208f,4.924f},{23.792f,4.893f},{24.958f,5.076f},{27.875f,5.964f},{28.876f,6.375f},{29.625f,6.920f},{30.458f,7.343f},{31.792f,8.262f},{32.317f,8.792f},{33.208f,9.876f},{33.553f,10.375f},{33.740f,10.792f},{33.803f,11.375f},{33.696f,11.958f},{33.230f,12.875f},{32.792f,13.483f},{32.391f,13.875f},{31.958f,14.136f},{31.375f,14.321f},{30.625f,14.438f},{29.708f,14.345f},{28.708f,13.961f},{28.208f,13.672f},{26.458f,12.309f},{24.958f,11.615f},{23.958f,11.337f},{22.125f,11.253f},{20.625f,11.294f},{20.125f,11.350f},{19.208f,11.577f},{17.625f,12.111f},{16.292f,12.799f},{14.125f,14.396f},{13.517f,14.958f},{12.919f,15.792f},{12.695f,16.208f},{12.451f,17.125f},{12.231f,17.458f},{11.708f,17.985f},{10.458f,18.849f},{10.042f,18.974f},{9.042f,19.107f},{3.542f,19.126f} };
static const Gdiplus::PointF kBuild8102Share_contour1[]={ {23.958f,27.964f},{23.458f,28.021f},{21.625f,28.005f},{20.708f,27.784f},{20.097f,27.542f},{18.754f,26.625f},{17.458f,25.328f},{17.055f,24.708f},{16.719f,23.792f},{16.578f,22.125f},{16.659f,20.625f},{16.790f,19.958f},{17.193f,19.042f},{18.125f,17.918f},{18.958f,17.170f},{20.208f,16.400f},{21.375f,16.029f},{21.875f,15.975f},{23.708f,15.994f},{24.625f,16.220f},{25.208f,16.451f},{26.562f,17.375f},{27.792f,18.604f},{28.236f,19.292f},{28.571f,20.208f},{28.675f,21.875f},{28.608f,23.542f},{28.458f,24.162f},{28.058f,25.042f},{27.276f,25.958f},{26.292f,26.826f},{25.125f,27.593f},{23.958f,27.964f} };
static const Gdiplus::PointF kBuild8102Share_contour2[]={ {25.375f,39.049f},{21.708f,39.117f},{20.792f,39.017f},{19.019f,38.542f},{18.375f,38.279f},{16.732f,37.792f},{16.292f,37.595f},{15.625f,37.115f},{14.792f,36.642f},{13.458f,35.689f},{12.438f,34.542f},{11.813f,33.708f},{11.556f,33.125f},{11.529f,32.458f},{11.674f,31.875f},{12.129f,30.958f},{12.708f,30.248f},{13.375f,29.834f},{14.375f,29.594f},{15.208f,29.584f},{15.875f,29.735f},{16.792f,30.131f},{18.792f,31.641f},{20.542f,32.451f},{21.458f,32.685f},{21.875f,32.726f},{23.708f,32.749f},{25.042f,32.628f},{26.375f,32.325f},{27.792f,31.839f},{29.125f,31.129f},{30.542f,30.123f},{31.642f,29.208f},{32.176f,28.542f},{32.590f,27.875f},{32.875f,26.853f},{33.221f,26.375f},{33.708f,25.927f},{34.958f,25.112f},{36.375f,24.891f},{41.375f,24.907f},{42.292f,24.846f},{42.875f,25.000f},{43.292f,25.366f},{43.456f,25.792f},{43.546f,26.875f},{43.569f,30.375f},{43.345f,31.292f},{43.164f,31.625f},{42.958f,31.840f},{42.708f,31.987f},{42.375f,32.068f},{39.042f,31.968f},{37.625f,32.218f},{37.125f,32.574f},{36.611f,33.292f},{35.958f,34.023f},{34.542f,35.198f},{33.625f,35.784f},{32.958f,36.296f},{31.875f,36.920f},{30.125f,37.778f},{28.375f,38.277f},{27.542f,38.607f},{26.292f,38.925f},{25.375f,39.049f} };
static const VectorContourData kBuild8102Share_contours[]={ {kBuild8102Share_contour0,67}, {kBuild8102Share_contour1,33}, {kBuild8102Share_contour2,66} };
static const VectorLayerData kBuild8102Share_layer={ kBuild8102Share_contours,_countof(kBuild8102Share_contours),RGB(255,255,255),false };
static const VectorLayerData kBuild8102Share_layers[]={ kBuild8102Share_layer };
static const VectorGlyphData kBuild8102Share={ kBuild8102Share_layers,_countof(kBuild8102Share_layers) };
static const Gdiplus::PointF kBuild8102Devices_contour0[]={ {25.458f,41.960f},{20.208f,41.989f},{19.042f,41.878f},{17.625f,41.593f},{15.792f,40.876f},{14.958f,40.625f},{13.542f,40.001f},{12.208f,39.255f},{9.792f,37.588f},{8.458f,36.364f},{6.792f,34.444f},{5.760f,33.125f},{5.011f,31.875f},{4.590f,30.708f},{3.642f,28.958f},{3.327f,27.792f},{2.733f,25.042f},{2.642f,23.875f},{2.571f,21.042f},{2.724f,19.042f},{2.951f,17.875f},{3.674f,14.958f},{4.590f,13.292f},{4.965f,12.208f},{5.760f,10.875f},{6.792f,9.556f},{8.718f,7.375f},{9.792f,6.412f},{12.125f,4.800f},{13.542f,3.992f},{14.958f,3.375f},{15.792f,3.124f},{17.625f,2.407f},{18.958f,2.135f},{20.458f,2.003f},{24.958f,2.007f},{26.292f,2.127f},{27.625f,2.392f},{29.542f,3.132f},{30.375f,3.377f},{31.792f,3.999f},{33.125f,4.754f},{35.375f,6.305f},{36.792f,7.573f},{38.542f,9.577f},{39.550f,10.875f},{40.308f,12.125f},{40.728f,13.292f},{41.701f,15.125f},{41.993f,16.208f},{42.564f,18.875f},{42.669f,20.292f},{42.667f,23.792f},{42.588f,24.958f},{42.362f,26.125f},{42.136f,26.958f},{41.993f,27.792f},{41.640f,29.042f},{41.089f,29.958f},{40.728f,30.708f},{40.349f,31.792f},{39.492f,33.208f},{38.000f,34.958f},{37.292f,35.891f},{36.592f,36.625f},{35.458f,37.634f},{33.125f,39.246f},{31.792f,39.992f},{30.458f,40.581f},{29.542f,40.868f},{27.792f,41.560f},{26.375f,41.860f},{25.458f,41.960f} };
static const Gdiplus::PointF kBuild8102Devices_contour1[]={ {23.910f,38.792f},{25.125f,38.678f},{26.952f,38.208f},{27.958f,38.019f},{28.792f,37.743f},{29.625f,37.245f},{30.458f,36.918f},{31.875f,36.124f},{34.542f,33.928f},{35.447f,33.042f},{36.372f,31.792f},{36.858f,30.792f},{37.975f,29.208f},{38.994f,26.208f},{39.280f,25.042f},{39.392f,23.708f},{39.397f,20.458f},{39.313f,19.208f},{38.970f,17.708f},{38.511f,16.292f},{38.249f,15.708f},{38.012f,14.875f},{37.659f,14.292f},{36.858f,13.208f},{36.372f,12.208f},{35.217f,10.708f},{34.458f,10.001f},{31.792f,7.805f},{30.458f,7.082f},{29.625f,6.755f},{28.670f,6.208f},{27.875f,5.958f},{26.952f,5.792f},{25.208f,5.340f},{23.625f,5.198f},{21.625f,5.199f},{20.542f,5.273f},{17.542f,5.932f},{16.708f,6.179f},{16.292f,6.363f},{15.708f,6.736f},{14.875f,7.078f},{13.792f,7.633f},{10.875f,9.980f},{10.010f,10.792f},{8.939f,12.208f},{8.458f,13.205f},{7.657f,14.292f},{7.309f,14.875f},{7.069f,15.708f},{6.803f,16.292f},{6.368f,17.625f},{6.073f,18.792f},{5.865f,20.625f},{5.860f,21.125f},{5.964f,21.625f},{6.170f,22.042f},{6.458f,22.258f},{6.792f,22.283f},{7.292f,22.064f},{8.196f,21.375f},{9.542f,20.505f},{10.958f,19.932f},{12.542f,19.448f},{13.625f,19.231f},{15.042f,19.120f},{19.958f,19.092f},{20.958f,19.135f},{21.792f,18.961f},{22.360f,18.625f},{22.564f,18.208f},{22.673f,16.958f},{22.591f,14.958f},{22.774f,14.375f},{23.042f,14.103f},{23.458f,14.009f},{23.958f,14.104f},{24.375f,14.380f},{25.458f,15.741f},{26.264f,16.458f},{27.020f,17.292f},{28.292f,18.406f},{29.458f,19.741f},{30.642f,20.792f},{31.089f,21.375f},{31.204f,21.708f},{31.232f,22.042f},{31.089f,22.625f},{30.642f,23.208f},{29.458f,24.259f},{28.292f,25.594f},{27.020f,26.708f},{26.257f,27.542f},{25.458f,28.259f},{24.673f,29.292f},{24.292f,29.695f},{23.875f,29.926f},{23.458f,29.991f},{23.042f,29.897f},{22.792f,29.654f},{22.591f,29.042f},{22.671f,26.958f},{22.515f,25.625f},{22.360f,25.375f},{22.042f,25.147f},{21.542f,24.961f},{21.042f,24.868f},{17.542f,24.887f},{15.708f,25.249f},{13.375f,25.210f},{12.292f,25.365f},{9.708f,26.045f},{9.208f,26.315f},{8.663f,26.875f},{7.970f,28.042f},{7.796f,28.792f},{7.895f,29.542f},{8.205f,30.375f},{8.884f,31.708f},{9.865f,33.042f},{10.875f,34.020f},{13.708f,36.314f},{14.875f,36.922f},{15.708f,37.264f},{16.458f,37.722f},{17.458f,38.048f},{20.542f,38.727f},{21.625f,38.801f},{23.910f,38.792f} };
static const VectorContourData kBuild8102Devices_contours[]={ {kBuild8102Devices_contour0,73}, {kBuild8102Devices_contour1,129} };
static const VectorLayerData kBuild8102Devices_layer={ kBuild8102Devices_contours,_countof(kBuild8102Devices_contours),RGB(255,255,255),false };
static const VectorLayerData kBuild8102Devices_layers[]={ kBuild8102Devices_layer };
static const VectorGlyphData kBuild8102Devices={ kBuild8102Devices_layers,_countof(kBuild8102Devices_layers) };
static const Gdiplus::PointF kBuild8102Settings_contour0[]={ {28.125f,40.709f},{27.458f,40.570f},{26.875f,40.161f},{26.577f,39.792f},{26.228f,38.875f},{25.669f,37.708f},{25.292f,37.145f},{25.042f,36.890f},{24.708f,36.671f},{23.875f,36.402f},{22.708f,36.509f},{20.625f,36.480f},{20.125f,36.627f},{19.871f,36.875f},{19.483f,37.458f},{18.408f,39.875f},{17.875f,40.395f},{17.375f,40.638f},{16.958f,40.680f},{16.458f,40.589f},{13.458f,39.448f},{13.042f,39.131f},{12.823f,38.875f},{12.664f,38.542f},{12.610f,38.125f},{12.667f,37.458f},{12.844f,36.708f},{13.029f,36.292f},{13.589f,35.375f},{13.773f,34.792f},{13.788f,34.375f},{13.727f,34.042f},{13.588f,33.708f},{13.295f,33.292f},{11.208f,31.080f},{10.833f,30.792f},{10.458f,30.641f},{9.958f,30.607f},{9.375f,30.721f},{7.542f,31.545f},{6.875f,31.752f},{6.208f,31.762f},{5.542f,31.544f},{5.168f,31.208f},{4.813f,30.625f},{4.488f,29.542f},{3.633f,27.625f},{3.547f,27.208f},{3.558f,26.875f},{3.684f,26.542f},{3.902f,26.292f},{4.208f,26.081f},{4.958f,25.790f},{5.625f,25.392f},{6.458f,25.047f},{7.375f,24.510f},{7.766f,24.125f},{7.940f,23.625f},{7.895f,21.375f},{8.003f,20.292f},{7.958f,20.016f},{7.792f,19.646f},{7.394f,19.208f},{6.458f,18.571f},{4.285f,17.708f},{3.947f,17.375f},{3.711f,16.875f},{3.640f,16.292f},{3.749f,15.625f},{3.955f,15.125f},{4.438f,14.292f},{5.005f,12.792f},{5.219f,12.458f},{5.542f,12.182f},{6.042f,11.985f},{6.542f,11.912f},{7.269f,12.042f},{8.625f,12.713f},{9.792f,13.029f},{10.542f,12.996f},{10.875f,12.839f},{11.208f,12.588f},{11.958f,11.893f},{13.210f,10.458f},{13.710f,9.792f},{13.871f,9.375f},{13.868f,9.042f},{13.771f,8.708f},{13.042f,7.469f},{12.730f,6.542f},{12.577f,5.625f},{12.727f,5.042f},{13.056f,4.625f},{13.542f,4.255f},{14.375f,3.985f},{15.125f,3.607f},{16.125f,3.257f},{16.708f,3.129f},{17.208f,3.129f},{17.708f,3.277f},{18.125f,3.546f},{18.625f,4.149f},{18.925f,5.042f},{19.638f,6.625f},{20.125f,7.107f},{20.625f,7.239f},{23.708f,7.335f},{24.345f,7.208f},{24.875f,6.997f},{25.261f,6.708f},{25.512f,6.375f},{26.469f,4.042f},{26.997f,3.458f},{27.625f,3.160f},{28.125f,3.099f},{28.708f,3.199f},{29.542f,3.552f},{30.458f,3.832f},{31.708f,4.382f},{32.042f,4.619f},{32.292f,4.919f},{32.458f,5.292f},{32.504f,5.625f},{32.466f,6.042f},{32.346f,6.458f},{31.376f,8.542f},{31.218f,9.042f},{31.209f,9.375f},{31.292f,9.673f},{31.751f,10.375f},{33.708f,12.509f},{34.208f,12.890f},{34.708f,13.087f},{35.208f,13.086f},{35.792f,12.936f},{36.792f,12.552f},{37.760f,12.042f},{38.208f,11.899f},{38.708f,11.882f},{39.292f,12.098f},{39.792f,12.539f},{40.387f,13.458f},{41.196f,15.708f},{41.300f,16.208f},{41.273f,16.708f},{41.117f,17.208f},{40.875f,17.542f},{40.292f,17.922f},{38.958f,18.403f},{37.542f,19.141f},{37.033f,19.708f},{36.875f,20.042f},{36.813f,20.375f},{36.874f,23.375f},{36.980f,23.875f},{37.178f,24.208f},{37.625f,24.631f},{38.375f,24.932f},{38.958f,25.273f},{40.875f,26.195f},{41.181f,26.458f},{41.326f,26.708f},{41.396f,27.125f},{41.319f,27.625f},{40.340f,30.375f},{39.875f,31.011f},{39.458f,31.445f},{39.042f,31.708f},{38.625f,31.817f},{38.292f,31.807f},{37.875f,31.703f},{35.625f,30.747f},{35.125f,30.587f},{34.542f,30.607f},{33.936f,30.958f},{32.625f,32.401f},{31.641f,33.375f},{31.468f,33.625f},{31.348f,33.958f},{31.311f,34.625f},{31.448f,35.292f},{31.653f,35.792f},{32.240f,36.875f},{32.455f,37.542f},{32.498f,38.292f},{32.343f,38.792f},{32.042f,39.159f},{31.708f,39.371f},{30.958f,39.624f},{30.375f,39.920f},{29.542f,40.183f},{28.708f,40.589f},{28.125f,40.709f} };
static const Gdiplus::PointF kBuild8102Settings_contour1[]={ {23.354f,31.708f},{25.042f,31.380f},{26.458f,30.883f},{27.125f,30.459f},{27.958f,30.042f},{28.542f,29.639f},{29.042f,29.193f},{30.256f,27.875f},{31.310f,26.208f},{31.832f,25.042f},{32.254f,23.792f},{32.480f,22.292f},{32.423f,20.875f},{32.323f,20.208f},{31.972f,19.042f},{31.298f,17.542f},{30.458f,16.108f},{29.292f,14.784f},{28.779f,14.292f},{27.792f,13.614f},{26.292f,12.763f},{25.625f,12.597f},{24.792f,12.284f},{23.792f,12.042f},{21.625f,11.999f},{20.547f,12.208f},{19.792f,12.475f},{18.875f,12.694f},{17.708f,13.327f},{16.042f,14.466f},{15.539f,14.958f},{14.512f,16.208f},{13.675f,17.625f},{13.047f,18.875f},{12.937f,19.208f},{12.695f,20.708f},{12.656f,22.125f},{12.724f,23.375f},{12.794f,23.875f},{13.075f,24.875f},{13.914f,26.542f},{14.769f,27.875f},{16.042f,29.240f},{16.458f,29.609f},{18.792f,30.989f},{19.125f,31.125f},{20.375f,31.468f},{21.625f,31.707f},{22.292f,31.752f},{23.354f,31.708f} };
static const Gdiplus::PointF kBuild8102Settings_contour2[]={ {23.875f,25.972f},{21.708f,26.005f},{21.292f,25.934f},{20.875f,25.777f},{20.093f,25.208f},{19.208f,24.189f},{18.894f,23.625f},{18.663f,22.542f},{18.621f,21.958f},{18.677f,21.375f},{18.922f,20.292f},{19.283f,19.708f},{20.125f,18.759f},{20.625f,18.363f},{21.125f,18.120f},{21.792f,17.990f},{23.458f,17.989f},{23.938f,18.042f},{24.375f,18.192f},{24.792f,18.431f},{25.224f,18.792f},{26.094f,19.792f},{26.421f,20.375f},{26.649f,21.458f},{26.690f,22.042f},{26.543f,23.125f},{26.357f,23.792f},{25.962f,24.375f},{25.125f,25.304f},{24.542f,25.726f},{23.875f,25.972f} };
static const VectorContourData kBuild8102Settings_contours[]={ {kBuild8102Settings_contour0,193}, {kBuild8102Settings_contour1,50}, {kBuild8102Settings_contour2,31} };
static const VectorLayerData kBuild8102Settings_layer={ kBuild8102Settings_contours,_countof(kBuild8102Settings_contours),RGB(255,255,255),false };
static const VectorLayerData kBuild8102Settings_layers[]={ kBuild8102Settings_layer };
static const VectorGlyphData kBuild8102Settings={ kBuild8102Settings_layers,_countof(kBuild8102Settings_layers) };

// Charm id -> Build 8102 outline: Search 0, Share 1, Devices 3, Settings 4. Start (2) is excluded.
static bool DrawBuild8102CharmIcon(HDC dc, int charm, int x, int y, int width, int height) {
    const VectorGlyphData* glyph = nullptr;
    switch (charm) {
        case 0: glyph = &kBuild8102Search; break;
        case 1: glyph = &kBuild8102Share; break;
        case 3: glyph = &kBuild8102Devices; break;
        case 4: glyph = &kBuild8102Settings; break;
        default: return false;
    }
    try {
        return DrawVectorGlyphGdiplus(dc, *glyph, x, y, width, height, RGB(255, 255, 255));
    } catch (const std::exception& e) {
        Wh_Log(L"[Charms] Build 8102 icon not drawn: %S", e.what());
        return false;
    } catch (...) {
        Wh_Log(L"[Charms] Build 8102 icon not drawn");
        return false;
    }
}

bool DrawCharmIconGdiplus(HDC dc,int id,int x,int y,int width,int height,COLORREF color) {
    if(id<0 || id>=kCharmCount) return false;
    if(id==2 && g_startLogoStyle.load()==1)
        return Draw8102StartGlyph(dc,x,y,width,height);
    if(id!=2 && g_charmIconSet.load()==1 && DrawBuild8102CharmIcon(dc,id,x,y,width,height))
        return true;
    switch(id) {
        case 0: return WithGdiplusCanvas(dc,x,y,width,height,44.0f,44.0f,
                    [color](Gdiplus::Graphics& g)->bool { return DrawSearchGlyph(g,color); });
        case 2: return WithGdiplusCanvas(dc,x,y,width,height,44.0f,44.0f,
                    [color](Gdiplus::Graphics& g)->bool { return DrawStartGlyph(g,color); });
        case 4: return WithGdiplusCanvas(dc,x,y,width,height,44.0f,44.0f,
                    [color](Gdiplus::Graphics& g)->bool { return DrawSettingsGlyph(g,color); });
        case 1: return DrawShareGlyphMonochrome(dc,x,y,width,height);
        default: return WithGdiplusCanvas(dc,x,y,width,height,44.0f,44.0f,
                    [color](Gdiplus::Graphics& g)->bool { return DrawDevicesGlyph(g,color); });
    }
}

bool DrawRailCaptionGdiplus(HDC dc,PCWSTR text,RECT rect,int fontSize,COLORREF color) {
    if(!text || !*text || rect.right<=rect.left || rect.bottom<=rect.top || fontSize<=0)
        return false;
    const Gdiplus::REAL width=(Gdiplus::REAL)(rect.right-rect.left);
    const Gdiplus::REAL height=(Gdiplus::REAL)(rect.bottom-rect.top);
    return WithGdiplusCanvas(dc,rect.left,rect.top,(int)width,(int)height,width,height,
        [text,fontSize,color,width,height](Gdiplus::Graphics& graphics)->bool {
            Gdiplus::FontFamily semilight(L"Segoe UI Semilight");
            Gdiplus::FontFamily regular(L"Segoe UI");
            const Gdiplus::FontFamily& family=semilight.GetLastStatus()==Gdiplus::Ok?semilight:regular;
            if(family.GetLastStatus()!=Gdiplus::Ok) return false;
            Gdiplus::Font font(&family,(Gdiplus::REAL)fontSize,Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
            format.SetAlignment(Gdiplus::StringAlignmentCenter);
            format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
            format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
            Gdiplus::SolidBrush brush(Gdiplus::Color(255,GetRValue(color),
                                                      GetGValue(color),GetBValue(color)));
            graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
            const Gdiplus::RectF bounds(0,0,width,height);
            return graphics.DrawString(text,-1,&font,bounds,&format,&brush)==Gdiplus::Ok;
        });
}

// Byte-for-byte 44x44 clock Wi-Fi PNGs from Sub-Bubble/CharmsBarReloaded. Source:
// src/CharmsBarReloaded/Assets/CharmsClock; GPL-3.0.
enum WifiClockPngAsset {
    kWifiPngNoInternet=0, kWifiPngWifiInternet=1,
    kWifiPngWifiInternetMax=2, kWifiPngWifiInternetStrong=3,
    kWifiPngWifiInternetMedium=4, kWifiPngWifiInternetWeak=5,
    kWifiPngWifiInternetWeakest=6, kWifiPngWifiNoInternet=7,
    kWifiPngWifiNoInternetMax=8, kWifiPngWifiNoInternetStrong=9,
    kWifiPngWifiNoInternetMedium=10, kWifiPngWifiNoInternetWeak=11,
    kWifiPngWifiNoInternetWeakest=12, kWifiClockPngAssetCount=13
};
static const char* const kWifiClockPngBase64[kWifiClockPngAssetCount]={
    // NoInternet.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAMAAAApWqozAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAABIUExURQAAAP///////////////////////9ghKNghKNgh"
    "KP///9ghKP///////////////9ghKN09Q9ovNed1ef3y8t9LUOJZXvCsrxjFjv8AAAAPdFJOUwBg"
    "cDDwEIAgkNDQELBAUE6/ILQAAAABYktHRAH/Ai3eAAAAB3RJTUUH6AIFDAQns6EZFgAAAQFJREFU"
    "OMvl09sSgiAQBuBVPKBWi2Hy/m8a50Q55IxXtRdFM1/M/rAAnCs0dTn2Si8wWdUZjM6g+TiLM7n+"
    "Boe3cx0Ojj2PK2/rMgYgjVw2BOAbDEQu83aDAYvT/PO47cq4pwOTNdAeSID3b25sgTJfFHJ4bKeB"
    "bWqYMpgoOzv6lDrRs/zdqR44cmP58vKdRHDbM7bKb6054iL37hP4psMJo7n9E03gGnQ6rZ21XR/x"
    "HUyzwqS1rRew0dbusH9jHvM0xj3mxzYeTbXb2QbU2UQYMFaU+TMTwdFFB25zKUrP7lJSWwvXrF6k"
    "NwZQg7S6QVpzHTsdGdFcyMPwp1N+npWqN1SSK3wuka65AAAAJXRFWHRkYXRlOmNyZWF0ZQAyMDI0"
    "LTAyLTA1VDEyOjA0OjM4KzAwOjAwFwiT1gAAACV0RVh0ZGF0ZTptb2RpZnkAMjAyNC0wMi0wNVQx"
    "MjowNDozOCswMDowMGZVK2oAAAAodEVYdGRhdGU6dGltZXN0YW1wADIwMjQtMDItMDVUMTI6MDQ6"
    "MzkrMDA6MDCXNwEBAAAAAElFTkSuQmCC",
    // WifiInternet.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAMAAAApWqozAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAABFUExURQAAAPz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Ngh"
    "KNghKNghKNghKPz8/NghKN09Q9ovNed1ef////3y8t9LUOJZXvCsr3v09RcAAAAOdFJOUwBAZgYa"
    "U0YtMyCQ0BATv+7e7wAAAAFiS0dEEnu8bAAAAAAHdElNRQfoAgETCR+2FazfAAAA8UlEQVQ4y+3T"
    "2xLDEBAGYEFOtFLSeP9HLWFzQHDVw0z/m3YmX2V3bRH651vSYIybWoxtfgX7zuqwVx/DhFJKanFr"
    "nra1+Kzej8POup4iRPsuic+dDWQ7ZChPdmRcmHA2Foo1L2BiC8vilty4OITfMvhu7QT0YfSuztuI"
    "8WBrkEo6K+fnsZJwsmQUYlHKaanUbM4eL3C3Nqedlv5H7AL3aO1u1WD3qgNMkSvWarBCFLDT3hax"
    "TGE/OvgALJNlROFgpY4ajMLENjMdji7K8VKsng6XkjxaQ7Hrl+uDEbKLtMAiLbmKQWdXNG4ys/xh"
    "l+e/1QsypBbGo4iiRgAAACV0RVh0ZGF0ZTpjcmVhdGUAMjAyNC0wMi0wMVQxOTowOTozMCswMDow"
    "MOLhpSUAAAAldEVYdGRhdGU6bW9kaWZ5ADIwMjQtMDItMDFUMTk6MDk6MzArMDA6MDCTvB2ZAAAA"
    "KHRFWHRkYXRlOnRpbWVzdGFtcAAyMDI0LTAyLTAxVDE5OjA5OjMxKzAwOjAwYt438gAAAABJRU5E"
    "rkJggg==",
    // WifiInternetMax.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAQAAAC0jZKKAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAAACYktHRAD/h4/MvwAAAAd0SU1FB+gCARMICCzdGFkAAACJ"
    "SURBVEjH7ZHLDYAgEAWfhgIsxZKxI0qxgJesB5EENUYQOcDOhU9gAhlAURSPWNmxx85QSCxB6I3j"
    "X3/oSHzNFZMdL87VQjzz/ignzAAAZ9aib6CjUCh0ACCB6wpIisdwyQzdxauUy8+FN7kS48W5Wo9X"
    "S0zrA9mv4lO8p1war1vxEo25K0VphA1Qc4rmLDnD/AAAACV0RVh0ZGF0ZTpjcmVhdGUAMjAyNC0w"
    "Mi0wMVQxOTowODowOCswMDowMLBDh58AAAAldEVYdGRhdGU6bW9kaWZ5ADIwMjQtMDItMDFUMTk6"
    "MDg6MDgrMDA6MDDBHj8jAAAAKHRFWHRkYXRlOnRpbWVzdGFtcAAyMDI0LTAyLTAxVDE5OjA4OjA4"
    "KzAwOjAwlgse/AAAAABJRU5ErkJggg==",
    // WifiInternetStrong.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAQAAAC0jZKKAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAAACYktHRAD/h4/MvwAAAAd0SU1FB+gCARMIB7xiBcgAAACf"
    "SURBVEjH7ZTBDcIwDEVfUQboFL2WEdiE1cImHQGumYIBLIUDvTQ9pAYLGvC7WImiF+tbMjhO+3Q2"
    "GjkxAJDC9Lw5GDU4FNVMvOKPxBIlS5Ys0brjc1F3H0WNsP2p9BwBuIa7qZiJEYDb/IFZFGNRzcQq"
    "vjM83bgUYt24NFGoxqURv0Hz4toyfL3jyjLcXxQu/oD4sqjbT2k+JRznh3gA6gMosoSHBN4AAAAl"
    "dEVYdGRhdGU6Y3JlYXRlADIwMjQtMDItMDFUMTk6MDg6MDcrMDA6MDBGC/d2AAAAJXRFWHRkYXRl"
    "Om1vZGlmeQAyMDI0LTAyLTAxVDE5OjA4OjA3KzAwOjAwN1ZPygAAACh0RVh0ZGF0ZTp0aW1lc3Rh"
    "bXAAMjAyNC0wMi0wMVQxOTowODowNyswMDowMGBDbhUAAAAASUVORK5CYII=",
    // WifiInternetMedium.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAQAAAC0jZKKAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAAACYktHRAD/h4/MvwAAAAd0SU1FB+gCARMICCzdGFkAAACf"
    "SURBVEjH7ZHBDYMwDEVfqwzQKbjSEboJq8EmjNBeM0UHsBQOcCFBldxaQCq/i+VIPKz/wXHq52Kj"
    "kQcNADGM88vV6MAmm2bigvrE4dsPy7qsLi7q2imKM2QsN+4APMPbVMxIC8Br+YFZFG02zcQqjilP"
    "V5dCrKtLE4WqLo34B6oXSy9JkiTprS/usnn6KFy8g3hYzU9bXLa4sTnOnzABzqoii7imWjgAAAAl"
    "dEVYdGRhdGU6Y3JlYXRlADIwMjQtMDItMDFUMTk6MDg6MDcrMDA6MDBGC/d2AAAAJXRFWHRkYXRl"
    "Om1vZGlmeQAyMDI0LTAyLTAxVDE5OjA4OjA3KzAwOjAwN1ZPygAAACh0RVh0ZGF0ZTp0aW1lc3Rh"
    "bXAAMjAyNC0wMi0wMVQxOTowODowOCswMDowMJYLHvwAAAAASUVORK5CYII=",
    // WifiInternetWeak.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAQAAAC0jZKKAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAAACYktHRAD/h4/MvwAAAAd0SU1FB+gCARMICCzdGFkAAACs"
    "SURBVEjH7ZGxDcIwEEUfyKJGCjOkDSOwADOwWjIAO2QEaD0DSNToJFOQJnaBgK8QI7/m6yzl5fQP"
    "CoX8WWg0tqMGwLv++bIULVhHKRMn5Cd2n36Ynku1cXKuiaqYQ8e2YgPAxd2lYvZUAFw5aquoopSJ"
    "3+I3x7M1WwBO7iYV09MAcB5+IKuiiVIm/oLsxdZasGDBWvXGhyhnX0URTyDuRjme/DD5l1Oh8Cc8"
    "AJPaIs8Lb4MeAAAAJXRFWHRkYXRlOmNyZWF0ZQAyMDI0LTAyLTAxVDE5OjA4OjA3KzAwOjAwRgv3"
    "dgAAACV0RVh0ZGF0ZTptb2RpZnkAMjAyNC0wMi0wMVQxOTowODowNyswMDowMDdWT8oAAAAodEVY"
    "dGRhdGU6dGltZXN0YW1wADIwMjQtMDItMDFUMTk6MDg6MDgrMDA6MDCWCx78AAAAAElFTkSuQmCC",
    // WifiInternetWeakest.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAQAAAC0jZKKAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAAACYktHRAD/h4/MvwAAAAd0SU1FB+gCARMICCzdGFkAAACd"
    "SURBVEjH7ZHBDYMwDEVfUdQFYAbmYAFm6Gp0gO7AHJmhXQBZCof20uTk1gKC/C6WI+XJ+h8cp34u"
    "NhoZ6AGIYX6/NEYH9tk0ExfUJw6/fizrsrq4qGujKI6QsVzpAHiGxVTMSAvAi4dtFG02zcQq9ilP"
    "V5dCrKtLE4WqLo34D6oXyyRJkiSZrC++ZfPwUbh4A/H9a8bPFpWb45yEFay4IxPOItyTAAAAJXRF"
    "WHRkYXRlOmNyZWF0ZQAyMDI0LTAyLTAxVDE5OjA4OjA3KzAwOjAwRgv3dgAAACV0RVh0ZGF0ZTpt"
    "b2RpZnkAMjAyNC0wMi0wMVQxOTowODowNyswMDowMDdWT8oAAAAodEVYdGRhdGU6dGltZXN0YW1w"
    "ADIwMjQtMDItMDFUMTk6MDg6MDgrMDA6MDCWCx78AAAAAElFTkSuQmCC",
    // WifiNoInternet.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAQAAAC0jZKKAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAAACYktHRAD/h4/MvwAAAAd0SU1FB+gCARMIB7xiBcgAAAF1"
    "SURBVEjH7ZWxbcMwEEVfDBXpktJgk2xgbWBqA4+gDawRNILTpfQI2UD0BJY3iJtDSpUpBCiFTZmS"
    "5cSUnCAJ/Is7HI58oP4RIjQkWiqpRDNYo+GI88ChEy8KvnfiMLDEYhxPH50IiBYjcS+yFFJJZeFi"
    "dlUNraSSop8VKQBTMjGirRWixZAxBdiv8NQNSMiSyYn+hljlPcEAsmDe0X1SyXmYcra/SXnwYq0A"
    "QCVEbBtrt0TnYjlcUJsb1+2+zyef0qi2IuOu0Xkgk8UgsISSdzoMc8kl9APWYElY7+/EiogNABsi"
    "VgBMWEvStbGclWmZlmk5O3Xi1EKVVoYCgEIZpWt42rkzbOUjcFJDAV4PsYYn9FCgliyd2gHv4Oh+"
    "HgetunBi29VbxgC8Be/+4NyJbcUWzPPXYJ8XZNzKPidWxv49hurb3rzgs6bfuDzAfuPyscJrXD7g"
    "Afp74Nbwjt+uS504bOXfZ8UV/PPgvJH7Vldd9U/0AUBDhTTP/KOkAAAAJXRFWHRkYXRlOmNyZWF0"
    "ZQAyMDI0LTAyLTAxVDE5OjA4OjA3KzAwOjAwRgv3dgAAACV0RVh0ZGF0ZTptb2RpZnkAMjAyNC0w"
    "Mi0wMVQxOTowODowNyswMDowMDdWT8oAAAAodEVYdGRhdGU6dGltZXN0YW1wADIwMjQtMDItMDFU"
    "MTk6MDg6MDcrMDA6MDBgQ24VAAAAAElFTkSuQmCC",
    // WifiNoInternetMax.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAMAAAApWqozAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAAA/UExURQAAAPz8/Pz8/Pz8/Pz8/Pz8/P/CCv/CCv/CCv/C"
    "Cv/CCvz8/P/CCq+JFpR2Gl9QIioqKsqcEtelEDczKP///wiewLAAAAALdFJOUwCg0OAQYGAgoPCA"
    "2cG8bAAAAAFiS0dEFJLfyTUAAAAHdElNRQfoAgETCR+2FazfAAAAwklEQVQ4y+2RyxKDMAhF0fhA"
    "QyLG///XikmnjWlLljrTu2Fz5swFAP65ShprbVMLW8ld4LRZHZyoe8D5ZgqcUxeHy81agPYLXChN"
    "B9CZSriHYYC+DjYwEo1gquAekAhFrf4siqNaPZaInfdO1Cos4oV5EbUKS+MD3tX6z/bGntlLax3G"
    "J4w6LKeIcK5Op8vHLqaVeaWz+kOOGyf41LoMvsNYIX5l1MVEIcSJFeKwbUFXJ7Fjdrp6Sl1dYmn6"
    "Ac90ygwP9WkY3ctw8NcAAAAldEVYdGRhdGU6Y3JlYXRlADIwMjQtMDItMDFUMTk6MDk6MzErMDA6"
    "MDBElq6RAAAAJXRFWHRkYXRlOm1vZGlmeQAyMDI0LTAyLTAxVDE5OjA5OjMxKzAwOjAwNcsWLQAA"
    "ACh0RVh0ZGF0ZTp0aW1lc3RhbXAAMjAyNC0wMi0wMVQxOTowOTozMSswMDowMGLeN/IAAAAASUVO"
    "RK5CYII=",
    // WifiNoInternetStrong.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAMAAAApWqozAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAABRUExURQAAAPz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8"
    "/Pz8/P/CCvz8/P/CCv/CCv/CCv/CCvz8/P/CCq+JFpR2Gl9QIioqKsqcEtelEDczKP///6h6H4EA"
    "AAARdFJOUwBQgKDQaOAQCHBgYDAgoPCAOVQQpgAAAAFiS0dEGnVn5DIAAAAHdElNRQfoAgETCAlb"
    "2ijPAAAAzElEQVQ4y+2QQRKCMAxFQ1EUkELTEsr9L2oDdbSopEuZ8S+SzZs3PwH451dSKKWKXFhx"
    "jgKXxpgyFzaco8Dxsjw4UseA08t4ngDOX+BUGUZ1AbjWmXADbQu3PLiCDrGDOgtuQCNqVu9fxmsR"
    "r+rdsssKYuucZbUIs3gkGlktwtx4gYNahLmxI3LcWob1A9YyzK9Y4VQdX5euIMaJaMKt+kOWH0d4"
    "0/o9+hXWGeJnOlmM6P26dYbYz7OX1VFsiays7mNXG1nsd+ABNxngDqLMIQTAhbgFAAAAJXRFWHRk"
    "YXRlOmNyZWF0ZQAyMDI0LTAyLTAxVDE5OjA4OjA5KzAwOjAwFjSMKwAAACV0RVh0ZGF0ZTptb2Rp"
    "ZnkAMjAyNC0wMi0wMVQxOTowODowOSswMDowMGdpNJcAAAAodEVYdGRhdGU6dGltZXN0YW1wADIw"
    "MjQtMDItMDFUMTk6MDg6MDkrMDA6MDAwfBVIAAAAAElFTkSuQmCC",
    // WifiNoInternetMedium.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAMAAAApWqozAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAABFUExURQAAAPz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/P/CCv/C"
    "Cv/CCv/CCv/CCvz8/P/CCq+JFpR2Gl9QIioqKsqcEtelEDczKP///6U8cVgAAAANdFJOUwBQgKBo"
    "cAgwYCCg8IBqiAFiAAAAAWJLR0QWfNGoGQAAAAd0SU1FB+gCARMJH7YVrN8AAADJSURBVDjL7dBJ"
    "DoMwDEBRM4QhECeYcP+rFoMRQ0vjZZHqTTZPX3YA/vMrk+V5nmlxzvMULJfpsKhn4MI5V2ix43kK"
    "lssOuAQob7CoHZsKoDJKXEPTQK3DBlrEFowK12ARLafvL9vwEl7T98kNz2Efgud0EnN4IBo4ncS8"
    "8YLndBLzxoEo8NZpbDds05i/YsXntHzd+ZnDOBKNeE1/mOWPBV+2fh97xFYR3qdNhxFjXF+rCMdp"
    "ium0hD2RT6c72dWLxe4L7vEyPbwAqoIW1pmUHmkAAAAldEVYdGRhdGU6Y3JlYXRlADIwMjQtMDIt"
    "MDFUMTk6MDk6MzErMDA6MDBElq6RAAAAJXRFWHRkYXRlOm1vZGlmeQAyMDI0LTAyLTAxVDE5OjA5"
    "OjMxKzAwOjAwNcsWLQAAACh0RVh0ZGF0ZTp0aW1lc3RhbXAAMjAyNC0wMi0wMVQxOTowOTozMSsw"
    "MDowMGLeN/IAAAAASUVORK5CYII=",
    // WifiNoInternetWeak.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAMAAAApWqozAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAABFUExURQAAAPz8/Pz8/Pz8/Pz8/Pz8/Pz8/Pz8/P/CCv/C"
    "Cv/CCv/CCv/CCvz8/P/CCq+JFpR2Gl9QIioqKsqcEtelEDczKP///6U8cVgAAAANdFJOUwBQgKBo"
    "cAgwYCCg8IBqiAFiAAAAAWJLR0QWfNGoGQAAAAd0SU1FB+gCARMICVvaKM8AAADLSURBVDjL7dLB"
    "DoMgDIDhijIVpSDi+7/qrHaZMLdy1GS9cPnypxAA/nOVqZRSVSlWNHfBfLMyzOoeOL2ZgFN1cVxb"
    "a+tENQDNF2xpjlg/AB66ELfQddCWYQ09Yg+6CLdgEA2lT26W4S28p0+SGV7DzntHaRFTeAphorSI"
    "aeMNr2kR08Y+BE9by9i8sJExPcWO0zQ/HR/8+dcwziHMmKdPZntjxtnWn2OO2BSE39PLYcQY99MU"
    "hOOyRDnNYReCk9MD7+rY4vADj5jNCE+MohMcpoG3yAAAACV0RVh0ZGF0ZTpjcmVhdGUAMjAyNC0w"
    "Mi0wMVQxOTowODowOSswMDowMBY0jCsAAAAldEVYdGRhdGU6bW9kaWZ5ADIwMjQtMDItMDFUMTk6"
    "MDg6MDkrMDA6MDBnaTSXAAAAKHRFWHRkYXRlOnRpbWVzdGFtcAAyMDI0LTAyLTAxVDE5OjA4OjA5"
    "KzAwOjAwMHwVSAAAAABJRU5ErkJggg==",
    // WifiNoInternetWeakest.png
    "iVBORw0KGgoAAAANSUhEUgAAACwAAAAsCAMAAAApWqozAAAAIGNIUk0AAHomAACAhAAA+gAAAIDo"
    "AAB1MAAA6mAAADqYAAAXcJy6UTwAAABFUExURQAAAPz8/Pz8/Pz8/Pz8/Pz8/Pz8/P/CCv/CCv/C"
    "Cvz8/P/CCv/CCv/CCvz8/K+JFpR2Gl9QIioqKsqcEtelEDczKP///9rSjPIAAAANdFJOUwBQgGhw"
    "CDBgIKCg8IBL5SjLAAAAAWJLR0QWfNGoGQAAAAd0SU1FB+gCARMICVvaKM8AAADLSURBVDjL7dLB"
    "DoMgDIDhikxFrWDF93/VWe0ylW3lqMl64fLlTyEA/OcqUxhjilxseO6C5WZ5WNQ98PFmCj6qi+P0"
    "ZiVA+QUnSfsAeNhMXEFdQ5WHLTSIDdgsXIFDdJzeq3YYhjbBa3hL7/HAk+Al7EPwnFYxh0eikdMq"
    "5o1XvKRVzBsHosBb69i9sNMxP8WGj2l5OvkbcixhnIgmPKc/zPrGgk9bp+P22GWE39PoYcQYt9Nl"
    "hOM8Rz0tYU/k9XQnu3qx2P3APZ6mhydU6A//frZt3AAAACV0RVh0ZGF0ZTpjcmVhdGUAMjAyNC0w"
    "Mi0wMVQxOTowODowOSswMDowMBY0jCsAAAAldEVYdGRhdGU6bW9kaWZ5ADIwMjQtMDItMDFUMTk6"
    "MDg6MDkrMDA6MDBnaTSXAAAAKHRFWHRkYXRlOnRpbWVzdGFtcAAyMDI0LTAyLTAxVDE5OjA4OjA5"
    "KzAwOjAwMHwVSAAAAABJRU5ErkJggg==",
};

static bool DecodeEmbeddedBase64Png(const char* encoded,std::vector<BYTE>& bytes) {
    if(!encoded) return false;
    bytes.clear();
    unsigned accumulator=0;
    int bits=0;
    for(const unsigned char* p=reinterpret_cast<const unsigned char*>(encoded);*p;p++) {
        if(*p=='=') break;
        int value=-1;
        if(*p>='A'&&*p<='Z') value=*p-'A';
        else if(*p>='a'&&*p<='z') value=*p-'a'+26;
        else if(*p>='0'&&*p<='9') value=*p-'0'+52;
        else if(*p=='+') value=62;
        else if(*p=='/') value=63;
        else return false;
        accumulator=(accumulator<<6)|(unsigned)value;
        bits+=6;
        if(bits>=8) {
            bits-=8;
            bytes.push_back((BYTE)((accumulator>>bits)&0xff));
            accumulator &= bits?((1u<<bits)-1u):0u;
        }
    }
    static const BYTE pngSignature[]={0x89,'P','N','G',0x0d,0x0a,0x1a,0x0a};
    return bytes.size()>=sizeof(pngSignature) && memcmp(bytes.data(),pngSignature,sizeof(pngSignature))==0;
}

static bool DrawWifiClockPngGdiplus(HDC dc,int x,int y,int width,int height,int asset) {
    if(!dc || width<=0 || height<=0 || asset<0 || asset>=kWifiClockPngAssetCount) return false;
    std::vector<BYTE> png;
    if(!DecodeEmbeddedBase64Png(kWifiClockPngBase64[asset],png) || png.size()>MAXDWORD) return false;
    IStream* stream=SHCreateMemStream(png.data(),(UINT)png.size());
    if(!stream) return false;
    const bool drawn=WithGdiplusCanvas(dc,x,y,width,height,44.0f,44.0f,
        [stream](Gdiplus::Graphics& graphics)->bool {
            Gdiplus::Bitmap source(stream,FALSE);
            if(source.GetLastStatus()!=Gdiplus::Ok) return false;
            graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
            graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
            graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
            const Gdiplus::RectF destination(0.0f,0.0f,44.0f,44.0f);
            return graphics.DrawImage(&source,destination,0.0f,0.0f,
                (Gdiplus::REAL)source.GetWidth(),(Gdiplus::REAL)source.GetHeight(),
                Gdiplus::UnitPixel)==Gdiplus::Ok;
        });
    stream->Release();
    return drawn;
}

static int WifiStrengthPngAsset(int quality,bool noInternet) {
    if(quality<0) return noInternet?kWifiPngWifiNoInternet:kWifiPngWifiInternetMedium;
    if(quality<=20) return noInternet?kWifiPngWifiNoInternetWeakest:kWifiPngWifiInternetWeakest;
    if(quality<=40) return noInternet?kWifiPngWifiNoInternetWeak:kWifiPngWifiInternetWeak;
    if(quality<=60) return noInternet?kWifiPngWifiNoInternetMedium:kWifiPngWifiInternetMedium;
    if(quality<=80) return noInternet?kWifiPngWifiNoInternetStrong:kWifiPngWifiInternetStrong;
    return noInternet?kWifiPngWifiNoInternetMax:kWifiPngWifiInternetMax;
}

void DrawWifiNetworkIconGdiplus(HDC dc,int x,int y,int width,int height,
                               int signalQuality,bool connected,bool noInternet,
                               bool radioDisabled,bool adapterDetected) {
    int asset=kWifiPngNoInternet;
    if(connected) {
        asset=WifiStrengthPngAsset(signalQuality,noInternet);
    } else if(radioDisabled) {
        asset=kWifiPngWifiInternet;
    } else if(adapterDetected) {
        asset=kWifiPngWifiNoInternet;
    }
    DrawWifiClockPngGdiplus(dc,x,y,width,height,asset);
}

void DrawEthernetIconGdiplus(HDC dc,int x,int y,int width,int height) {
    // Match the small Windows 8 wired-network glyph in the supplied clock reference: a centered latch,
    // squared socket and three contacts, all white.
    WithGdiplusCanvas(dc,x,y,width,height,22.0f,20.0f,
        [](Gdiplus::Graphics& graphics)->bool {
            Gdiplus::Pen white(Gdiplus::Color(255,255,255,255),1.65f);
            white.SetStartCap(Gdiplus::LineCapRound);
            white.SetEndCap(Gdiplus::LineCapRound);
            Gdiplus::GraphicsPath jack;
            jack.AddLine(8.0f,2.0f,14.0f,2.0f);
            jack.AddLine(14.0f,2.0f,14.0f,6.0f);
            jack.AddLine(14.0f,6.0f,18.0f,6.0f);
            jack.AddLine(18.0f,6.0f,20.0f,8.0f);
            jack.AddLine(20.0f,8.0f,20.0f,15.5f);
            jack.AddLine(20.0f,15.5f,18.0f,17.5f);
            jack.AddLine(18.0f,17.5f,4.0f,17.5f);
            jack.AddLine(4.0f,17.5f,2.0f,15.5f);
            jack.AddLine(2.0f,15.5f,2.0f,8.0f);
            jack.AddLine(2.0f,8.0f,4.0f,6.0f);
            jack.AddLine(4.0f,6.0f,8.0f,6.0f);
            jack.AddLine(8.0f,6.0f,8.0f,2.0f);
            if(graphics.DrawPath(&white,&jack)!=Gdiplus::Ok) return false;
            for(int i=0;i<3;i++) {
                const Gdiplus::REAL contactX=7.0f+i*4.0f;
                graphics.DrawLine(&white,contactX,9.5f,contactX,15.2f);
            }
            return true;
        });
}

void DrawAirplaneIconGdiplus(HDC dc,int x,int y,int width,int height) {
    static const Gdiplus::PointF plane[]={
        {1.0f,8.0f},{7.0f,7.0f},{14.0f,1.0f},{17.0f,2.0f},
        {13.0f,8.0f},{19.0f,8.0f},{19.0f,10.0f},{12.0f,10.0f},
        {9.0f,16.0f},{7.0f,16.0f},{8.0f,10.0f},{1.0f,10.0f}
    };
    WithGdiplusCanvas(dc,x,y,width,height,22.0f,20.0f,
        [](Gdiplus::Graphics& graphics)->bool {
            Gdiplus::SolidBrush white(Gdiplus::Color(255,245,245,245));
            Gdiplus::GraphicsPath path;
            path.AddPolygon(plane,_countof(plane));
            return graphics.FillPath(&white,&path)==Gdiplus::Ok;
        });
}

void DrawNetworkStatusIcon(HDC dc,int x,int y,int width,int height) {
    const int state=g_networkState.load();
    if(state==1) { DrawEthernetIconGdiplus(dc,x,y,width,height); return; }
    if(g_airplane.load()) { DrawAirplaneIconGdiplus(dc,x,y,width,height); return; }
    const bool connected=state==2 || state==3;
    DrawWifiNetworkIconGdiplus(dc,x,y,width,height, connected?g_wifiSignal.load():-1,connected,state==2,
        g_wifiRadioOff.load(),g_adapterDetected.load());
}

void DrawRail(HDC dc, RECT rc) {
    // Paint the rail in physical pixels. GDI+ ignores some HDC mapping modes, so mixing a mapped GDI canvas
    // with GDI+ was shifting/scaling glyphs differently from captions. Scale every logical coordinate exactly
    // once.
    const int width=rc.right;
    const int logicalHeight=DpiUnscale(rc.bottom);
    ScopedGdiObject background(CreateSolidBrush(GetRailBackground()));
    if(background) FillRect(dc,&rc,(HBRUSH)background.get());

    // UIFILE 7001 reserves an 86×62rp icon area with 19rp/14rp side/top padding; the existing 44×44 glyphs
    // centered inside it land at x=21,y=16.
    static const int iconWidths[kCharmCount]={44,44,44,44,44};
    static const int iconHeights[kCharmCount]={44,44,44,44,44};
    static const int iconOffsets[kCharmCount]={16,16,16,16,16};
    const int slotCount=GetCharmSlotCount();
    for(int i=0;i<slotCount;i++) {
        const int charm=GetCharmAction(i);
        if(charm<0 || g_hidden[charm]) continue;
        const int rowTop=DpiScale(GetCharmTop(i,logicalHeight));
        const int rowHeight=DpiScale(kGridCell);
        const bool active=g_hover.load()==i || (g_keyboardOpened.load() && g_keyboardSelection.load()==i);
        const bool pressed=g_pressed.load()==i;
        if(active || pressed) {
            RECT row{0,rowTop,width,rowTop+rowHeight};
            ScopedGdiObject highlight(CreateSolidBrush(GetRailHighlight(pressed)));
            if(highlight) FillRect(dc,&row,(HBRUSH)highlight.get());
        }
        const int iconWidth=DpiScale(iconWidths[charm]);
        const int iconHeight=DpiScale(iconHeights[charm]);
        const int iconX=(width-iconWidth)/2;
        const int iconY=rowTop+DpiScale(iconOffsets[charm]);
        const COLORREF iconColor=charm==2?GetStartLogoColor():RGB(255,255,255);
        if(!DrawCharmIconGdiplus(dc,charm,iconX,iconY,iconWidth,iconHeight,iconColor))
            DrawFallbackCharmIcon(dc,charm,(width-DpiScale(44))/2,
                                  rowTop+DpiScale(16),DpiScale(44),DpiScale(44));
        try {
            const std::wstring label=LocalizedCharm(charm);
            RECT text{DpiScale(2),rowTop+DpiScale(64), width-DpiScale(2),rowTop+DpiScale(86)};
            if(!DrawRailCaptionGdiplus(dc,label.c_str(),text,DpiScale(12),RGB(255,255,255)))
                DrawTextBox(dc,label.c_str(),text,DpiScale(12),RGB(255,255,255),FW_NORMAL,
                            DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
        } catch (...) {
            // A caption allocation failure must not take down Explorer's UI thread.
        }
    }
}

PCWSTR LocalizedShareDescription() {
    switch(GetUiPrimaryLanguage()) {
        case LANG_ITALIAN: return L"Condiviso dalla Charms Bar di Windows";
        case LANG_GERMAN: return L"Über die Windows-Charms-Leiste geteilt";
        case LANG_FRENCH: return L"Partagé depuis la barre des charmes Windows";
        case LANG_SPANISH: return L"Compartido desde la barra de accesos de Windows";
        case LANG_JAPANESE: return L"Windows チャーム バーから共有";
        case LANG_PORTUGUESE: return L"Compartilhado da Barra de Charms do Windows";
        case LANG_RUSSIAN: return L"Отправлено из панели чудо-кнопок Windows";
        case LANG_CHINESE: return L"从 Windows 超级按钮栏共享";
        case LANG_KOREAN: return L"Windows 참 메뉴 모음에서 공유함";
        default: return L"Shared from the Windows Charms Bar";
    }
}

PCWSTR LocalizedQuickMenuItem(int index) {
    static const PCWSTR labels[10][3] = {
        {L"Network settings",L"Display settings",L"All settings"},
        {L"Impostazioni rete",L"Impostazioni schermo",L"Tutte le impostazioni"},
        {L"Netzwerkeinstellungen",L"Anzeigeeinstellungen",L"Alle Einstellungen"},
        {L"Paramètres réseau",L"Paramètres d'affichage",L"Tous les paramètres"},
        {L"Configuración de red",L"Configuración de pantalla",L"Toda la configuración"},
        {L"ネットワーク設定",L"ディスプレイ設定",L"すべての設定"},
        {L"Configurações de rede",L"Configurações de vídeo",L"Todas as configurações"},
        {L"Параметры сети",L"Параметры экрана",L"Все параметры"},
        {L"网络设置",L"显示设置",L"所有设置"},
        {L"네트워크 설정",L"디스플레이 설정",L"모든 설정"} };
    const int lang=GetUiPrimaryLanguage();
    int row=0;
    if(lang==LANG_ITALIAN) row=1;
    else if(lang==LANG_GERMAN) row=2;
    else if(lang==LANG_FRENCH) row=3;
    else if(lang==LANG_SPANISH) row=4;
    else if(lang==LANG_JAPANESE) row=5;
    else if(lang==LANG_PORTUGUESE) row=6;
    else if(lang==LANG_RUSSIAN) row=7;
    else if(lang==LANG_CHINESE) row=8;
    else if(lang==LANG_KOREAN) row=9;
    return index>=0&&index<3?labels[row][index]:L"";
}

PCWSTR LocalizedShareFallbackTitle() {
    switch(GetUiPrimaryLanguage()) {
        case LANG_ITALIAN: return L"Condivisione Charms";
        case LANG_GERMAN: return L"Charms-Freigabe";
        case LANG_FRENCH: return L"Partage Charms";
        case LANG_SPANISH: return L"Compartir Charms";
        case LANG_JAPANESE: return L"チャームで共有";
        case LANG_PORTUGUESE: return L"Compartilhamento Charms";
        case LANG_RUSSIAN: return L"Общий доступ Charms";
        case LANG_CHINESE: return L"超级按钮共享";
        case LANG_KOREAN: return L"참 공유";
        default: return L"Charms Share";
    }
}

PCWSTR LocalizedShareFallbackNotice() {
    switch(GetUiPrimaryLanguage()) {
        case LANG_ITALIAN: return L"L’app di posta non è disponibile. Il testo da condividere è stato copiato negli Appunti.";
        case LANG_GERMAN: return L"Keine Mail-App verfügbar. Der Freigabetext wurde in die Zwischenablage kopiert.";
        case LANG_FRENCH: return L"Aucune application de courrier. Le texte à partager a été copié dans le presse-papiers.";
        case LANG_SPANISH: return L"No hay una aplicación de correo. El texto se copió al portapapeles.";
        case LANG_JAPANESE: return L"メール アプリがありません。共有テキストをクリップボードにコピーしました。";
        case LANG_PORTUGUESE: return L"Nenhum aplicativo de email está disponível. O texto compartilhado foi copiado para a área de transferência.";
        case LANG_RUSSIAN: return L"Почтовое приложение недоступно. Текст для общего доступа скопирован в буфер обмена.";
        case LANG_CHINESE: return L"没有可用的邮件应用。要共享的文本已复制到剪贴板。";
        case LANG_KOREAN: return L"사용할 수 있는 메일 앱이 없습니다. 공유할 텍스트를 클립보드에 복사했습니다.";
        default: return L"No mail app is available. The share text was copied to the clipboard.";
    }
}

PCWSTR LocalizedShareClipboardFailure() {
    switch(GetUiPrimaryLanguage()) {
        case LANG_ITALIAN: return L"L’app di posta non è disponibile e non è stato possibile copiare il testo negli Appunti.";
        case LANG_GERMAN: return L"Keine Mail-App verfügbar; der Freigabetext konnte nicht in die Zwischenablage kopiert werden.";
        case LANG_FRENCH: return L"Aucune application de courrier et impossible de copier le texte dans le presse-papiers.";
        case LANG_SPANISH: return L"No hay una aplicación de correo y no se pudo copiar el texto al portapapeles.";
        case LANG_JAPANESE: return L"メール アプリがなく、共有テキストをクリップボードにコピーできませんでした。";
        case LANG_PORTUGUESE: return L"Nenhum aplicativo de email está disponível e não foi possível copiar o texto para a área de transferência.";
        case LANG_RUSSIAN: return L"Почтовое приложение недоступно, и не удалось скопировать текст в буфер обмена.";
        case LANG_CHINESE: return L"没有可用的邮件应用，并且无法将文本复制到剪贴板。";
        case LANG_KOREAN: return L"사용할 수 있는 메일 앱이 없고 텍스트를 클립보드에 복사할 수 없습니다.";
        default: return L"No mail app is available, and the share text could not be copied to the clipboard.";
    }
}

std::wstring LocalizedNetworkStatus() {
    int lang=GetUiPrimaryLanguage();
    int state=g_networkState.load();
    const wchar_t* airplane=L"Airplane mode";
    const wchar_t* wifiOff=L"Wi-Fi radio off";
    const wchar_t* internet=L"Internet";
    const wchar_t* ethernet=L"Ethernet";
    const wchar_t* disconnected=L"Network disconnected";
    const wchar_t* wifi=L"Wi-Fi  %d%%";
    const wchar_t* wifiUnknown=L"Wi-Fi connected";
    if(lang==LANG_ITALIAN){airplane=L"Modalità aereo";wifiOff=L"Wi-Fi disattivato";internet=L"Internet";ethernet=L"Ethernet";disconnected=L"Rete non connessa";wifi=L"Wi-Fi  %d%%";wifiUnknown=L"Wi-Fi connesso";}
    else if(lang==LANG_GERMAN){airplane=L"Flugzeugmodus";wifiOff=L"WLAN deaktiviert";internet=L"Internet";ethernet=L"Ethernet";disconnected=L"Nicht verbunden";wifi=L"WLAN  %d%%";wifiUnknown=L"WLAN verbunden";}
    else if(lang==LANG_FRENCH){airplane=L"Mode avion";wifiOff=L"Wi-Fi désactivé";internet=L"Internet";ethernet=L"Ethernet";disconnected=L"Réseau déconnecté";wifi=L"Wi-Fi  %d%%";wifiUnknown=L"Wi-Fi connecté";}
    else if(lang==LANG_SPANISH){airplane=L"Modo avión";wifiOff=L"Wi-Fi desactivado";internet=L"Internet";ethernet=L"Ethernet";disconnected=L"Red desconectada";wifi=L"Wi-Fi  %d%%";wifiUnknown=L"Wi-Fi conectado";}
    else if(lang==LANG_JAPANESE){airplane=L"機内モード";wifiOff=L"Wi-Fi オフ";internet=L"インターネット";ethernet=L"イーサネット";disconnected=L"ネットワーク未接続";wifi=L"Wi-Fi  %d%%";wifiUnknown=L"Wi-Fi 接続済み";}
    else if(lang==LANG_PORTUGUESE){airplane=L"Modo avião";wifiOff=L"Wi-Fi desativado";internet=L"Internet";ethernet=L"Ethernet";disconnected=L"Rede desconectada";wifi=L"Wi-Fi  %d%%";wifiUnknown=L"Wi-Fi conectado";}
    else if(lang==LANG_RUSSIAN){airplane=L"Режим полета";wifiOff=L"Wi-Fi выключен";internet=L"Интернет";ethernet=L"Ethernet";disconnected=L"Сеть не подключена";wifi=L"Wi-Fi  %d%%";wifiUnknown=L"Wi-Fi подключен";}
    else if(lang==LANG_CHINESE){airplane=L"飞行模式";wifiOff=L"Wi-Fi 已关闭";internet=L"Internet";ethernet=L"以太网";disconnected=L"网络未连接";wifi=L"Wi-Fi  %d%%";wifiUnknown=L"Wi-Fi 已连接";}
    else if(lang==LANG_KOREAN){airplane=L"비행기 모드";wifiOff=L"Wi-Fi 꺼짐";internet=L"인터넷";ethernet=L"이더넷";disconnected=L"네트워크 연결 안 됨";wifi=L"Wi-Fi  %d%%";wifiUnknown=L"Wi-Fi 연결됨";}
    if(state==1) return ethernet;
    if(g_airplane.load()) return airplane;
    if(g_wifiRadioOff.load()) return wifiOff;
    if(state==3) return internet;
    if(state==2){
        const int quality = g_wifiSignal.load();
        if (quality < 0) return wifiUnknown;
        wchar_t text[64]{};swprintf_s(text,64,wifi,quality);return text;
    }
    return disconnected;
}

// Build a concise, localized accessible name for the otherwise purely visual clock/status flyout. This also
// keeps battery charge information available without adding extra text that would diverge from the supplied
// reference.
std::wstring BuildAccessibleStatus() {
    const LANGID language = GetUiPrimaryLanguage();
    const int level = g_batteryLevel.load();
    const bool charging = g_batteryCharging.load();
    std::wstring result = LocalizedNetworkStatus();
    if (level < 0) return result;

    const wchar_t* battery = L"Battery";
    const wchar_t* chargingText = L"charging";
    const wchar_t* idleText = L"not charging";
    if (language == LANG_ITALIAN) {
        battery = L"Batteria"; chargingText = L"in carica"; idleText = L"non in carica";
    } else if (language == LANG_GERMAN) {
        battery = L"Akku"; chargingText = L"wird geladen"; idleText = L"wird nicht geladen";
    } else if (language == LANG_FRENCH) {
        battery = L"Batterie"; chargingText = L"en charge"; idleText = L"hors charge";
    } else if (language == LANG_SPANISH) {
        battery = L"Batería"; chargingText = L"cargando"; idleText = L"sin cargar";
    } else if (language == LANG_JAPANESE) {
        battery = L"バッテリー"; chargingText = L"充電中"; idleText = L"充電していません";
    } else if (language == LANG_PORTUGUESE) {
        battery = L"Bateria"; chargingText = L"carregando"; idleText = L"não carregando";
    } else if (language == LANG_RUSSIAN) {
        battery = L"Батарея"; chargingText = L"заряжается"; idleText = L"не заряжается";
    } else if (language == LANG_CHINESE) {
        battery = L"电池"; chargingText = L"正在充电"; idleText = L"未充电";
    } else if (language == LANG_KOREAN) {
        battery = L"배터리"; chargingText = L"충전 중"; idleText = L"충전 안 함";
    }

    result += L"; ";
    result += battery;
    result += L" ";
    result += std::to_wstring((std::max)(0, (std::min)(100, level)));
    result += L"% — ";
    result += charging ? chargingText : idleText;
    return result;
}

// Draw the battery from current SYSTEM_POWER_STATUS data. Unlike the old screenshot crop, this reflects the
// real level and only draws a bolt when Windows reports BATTERY_FLAG_CHARGING; no battery is shown on
// desktops.
void DrawBatteryStatusIcon(HDC dc,int x,int y,int width,int height) {
    if(!dc) return;
    const int level=g_batteryLevel.load();
    if(level<0) return;

    const VectorGlyphData* battery=&kGlyphBatteryFull;
    if(level==0) battery=&kGlyphBattery0;
    else if(level<=4) battery=&kGlyphBattery1;
    else if(level<=9) battery=&kGlyphBattery5;
    else if(level<100) {
        const int decile=(level/10)*10;
        switch(decile) {
            case 10:battery=&kGlyphBattery10;break;
            case 20:battery=&kGlyphBattery20;break;
            case 30:battery=&kGlyphBattery30;break;
            case 40:battery=&kGlyphBattery40;break;
            case 50:battery=&kGlyphBattery50;break;
            case 60:battery=&kGlyphBattery60;break;
            case 70:battery=&kGlyphBattery70;break;
            case 80:battery=&kGlyphBattery80;break;
            default:battery=&kGlyphBattery90;break;
        }
    }

    // Exact 44x44 CharmsBarReloaded state silhouettes, vector-traced and redrawn by GDI+; the original
    // charging-plug glyph overlays the battery exactly as it does in that project's clock XAML.
    const COLORREF white=RGB(255,255,255);
    DrawVectorGlyphGdiplus(dc,*battery,x,y,width,height,white);
    if(g_batteryCharging.load())
        DrawVectorGlyphGdiplus(dc,kGlyphCharging,x,y,width,height,white);
}

void DrawClock(HDC dc, RECT rc) {
    ScopedGdiObject background(CreateSolidBrush(RGB(16,27,31)));
    if (background) FillRect(dc, &rc, (HBRUSH)background.get());

    SYSTEMTIME now{};
    GetLocalTime(&now);
    wchar_t timeText[64]{}, weekday[128]{}, monthDay[128]{};
    const int clockFormat=g_clockFormat.load();
    const PCWSTR timePattern=clockFormat==1?L"h:mm tt":clockFormat==2?L"HH:mm":nullptr;
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT,TIME_NOSECONDS,
                    &now,timePattern,timeText,(int)_countof(timeText));
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &now, L"dddd", weekday,
                    (int)_countof(weekday), nullptr);
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &now, L"MMMM d", monthDay,
                    (int)_countof(monthDay), nullptr);

    DrawNetworkStatusIcon(dc,40,38,22,20);
    DrawBatteryStatusIcon(dc,40,82,22,20);
    const COLORREF textColor = RGB(245,245,245);
    // Compact live-status glyphs at left, prominent time in the center, and a two-line date column at right.
    // The 203px time cell prevents the final digit of 24-hour times being clipped.
    RECT timeRect{76, 0, 279, kClockHeight};
    DrawTextBox(dc,timeText,timeRect,76,textColor,FW_LIGHT,
                DT_LEFT|DT_VCENTER|DT_SINGLELINE,L"Segoe UI Light",true);
    RECT weekdayRect{289,8,kClockWidth-12,68};
    DrawTextBox(dc,weekday,weekdayRect,29,textColor,FW_LIGHT,
                DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI Light");
    RECT dateRect{289,69,kClockWidth-12,132};
    DrawTextBox(dc,monthDay,dateRect,29,textColor,FW_LIGHT,
                DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI Light");
}

// ---------------------------------------------------------------------------
// Windows 8-style Settings pane (346 px, right edge).
// ---------------------------------------------------------------------------
PCWSTR LocalizedPaneText(int index) {
    static const PCWSTR t[10][17] = {
        {L"Settings",L"Desktop",L"Control Panel",L"Personalization",L"PC info",L"Help",L"Network",L"Volume",L"Brightness",L"Notifications",L"Power",L"Keyboard",L"Change PC settings",L"Sleep",L"Shut down",L"Restart",L"Unavailable"},
        {L"Impostazioni",L"Desktop",L"Pannello di controllo",L"Personalizzazione",L"Informazioni PC",L"Guida",L"Rete",L"Volume",L"Luminosità",L"Notifiche",L"Alimentazione",L"Tastiera",L"Cambia impostazioni PC",L"Sospendi",L"Arresta il sistema",L"Riavvia il sistema",L"Non disponibile"},
        {L"Einstellungen",L"Desktop",L"Systemsteuerung",L"Anpassung",L"PC-Info",L"Hilfe",L"Netzwerk",L"Lautstärke",L"Helligkeit",L"Benachrichtigungen",L"Ein/Aus",L"Tastatur",L"PC-Einstellungen ändern",L"Energie sparen",L"Herunterfahren",L"Neu starten",L"Nicht verfügbar"},
        {L"Paramètres",L"Bureau",L"Panneau de configuration",L"Personnalisation",L"Infos sur le PC",L"Aide",L"Réseau",L"Volume",L"Luminosité",L"Notifications",L"Alimentation",L"Clavier",L"Modifier les paramètres du PC",L"Mettre en veille",L"Arrêter",L"Redémarrer",L"Indisponible"},
        {L"Configuración",L"Escritorio",L"Panel de control",L"Personalización",L"Información del PC",L"Ayuda",L"Red",L"Volumen",L"Brillo",L"Notificaciones",L"Inicio/apagado",L"Teclado",L"Cambiar configuración del equipo",L"Suspender",L"Apagar",L"Reiniciar",L"No disponible"},
        {L"設定",L"デスクトップ",L"コントロール パネル",L"個人設定",L"PC 情報",L"ヘルプ",L"ネットワーク",L"音量",L"明るさ",L"通知",L"電源",L"キーボード",L"PC 設定の変更",L"スリープ",L"シャットダウン",L"再起動",L"利用不可"},
        {L"Configurações",L"Área de Trabalho",L"Painel de Controle",L"Personalização",L"Informações do PC",L"Ajuda",L"Rede",L"Volume",L"Brilho",L"Notificações",L"Energia",L"Teclado",L"Alterar configurações do PC",L"Suspender",L"Desligar",L"Reiniciar",L"Não disponível"},
        {L"Параметры",L"Рабочий стол",L"Панель управления",L"Персонализация",L"Сведения о компьютере",L"Справка",L"Сеть",L"Громкость",L"Яркость",L"Уведомления",L"Электропитание",L"Клавиатура",L"Изменение параметров компьютера",L"Спящий режим",L"Завершение работы",L"Перезагрузка",L"Недоступно"},
        {L"设置",L"桌面",L"控制面板",L"个性化",L"电脑信息",L"帮助",L"网络",L"音量",L"亮度",L"通知",L"电源",L"键盘",L"更改电脑设置",L"睡眠",L"关机",L"重启",L"不可用"},
        {L"설정",L"바탕 화면",L"제어판",L"개인 설정",L"PC 정보",L"도움말",L"네트워크",L"볼륨",L"밝기",L"알림",L"전원",L"키보드",L"PC 설정 변경",L"절전",L"시스템 종료",L"다시 시작",L"사용할 수 없음"}};
    int row=0;
    switch(GetUiPrimaryLanguage()) {
        case LANG_ITALIAN: row=1; break; case LANG_GERMAN: row=2; break;
        case LANG_FRENCH: row=3; break; case LANG_SPANISH: row=4; break;
        case LANG_JAPANESE: row=5; break;
        case LANG_PORTUGUESE: row=6; break; case LANG_RUSSIAN: row=7; break;
        case LANG_CHINESE: row=8; break; case LANG_KOREAN: row=9; break;
        default: row=0; break;
    }
    return index>=0&&index<17?t[row][index]:L"";
}

PCWSTR LocalizedCharmPaneTitle(int kind) {
    static const PCWSTR titles[10][3]={
        {L"Settings",L"Devices",L"Share"},
        {L"Impostazioni",L"Dispositivi",L"Condividi"},
        {L"Einstellungen",L"Geräte",L"Teilen"},
        {L"Paramètres",L"Périphériques",L"Partager"},
        {L"Configuración",L"Dispositivos",L"Compartir"},
        {L"設定",L"デバイス",L"共有"},
        {L"Configurações",L"Dispositivos",L"Compartilhar"},
        {L"Параметры",L"Устройства",L"Поделиться"},
        {L"设置",L"设备",L"共享"},
        {L"설정",L"장치",L"공유"}};
    int row=0;
    switch(GetUiPrimaryLanguage()) {
        case LANG_ITALIAN:row=1;break; case LANG_GERMAN:row=2;break;
        case LANG_FRENCH:row=3;break; case LANG_SPANISH:row=4;break;
        case LANG_JAPANESE:row=5;break;
        case LANG_PORTUGUESE:row=6;break; case LANG_RUSSIAN:row=7;break;
        case LANG_CHINESE:row=8;break; case LANG_KOREAN:row=9;break;
        default:row=0;break;
    }
    return kind>=0&&kind<3?titles[row][kind]:L"";
}

PCWSTR LocalizedSharePaneText(int index) {
    static const PCWSTR text[10][2]={
        {L"You can only share a screenshot from desktop apps.",L"Share a screenshot"},
        {L"Dalle app desktop puoi condividere solo uno screenshot.",L"Condividi uno screenshot"},
        {L"In Desktop-Apps können Sie nur einen Screenshot teilen.",L"Screenshot teilen"},
        {L"Dans les applications de bureau, seul un screenshot peut être partagé.",L"Partager une capture d’écran"},
        {L"En las aplicaciones de escritorio solo se puede compartir una captura.",L"Compartir una captura de pantalla"},
        {L"デスクトップ アプリから共有できるのはスクリーンショットのみです。",L"スクリーンショットを共有"},
        {L"Nos aplicativos da área de trabalho você só pode compartilhar uma captura de tela.",L"Compartilhar uma captura de tela"},
        {L"Из классических приложений можно поделиться только снимком экрана.",L"Поделиться снимком экрана"},
        {L"在桌面应用中，只能共享屏幕截图。",L"共享屏幕截图"},
        {L"데스크톱 앱에서는 스크린샷만 공유할 수 있습니다.",L"스크린샷 공유"}};
    int row=0;
    switch(GetUiPrimaryLanguage()) {
        case LANG_ITALIAN:row=1;break; case LANG_GERMAN:row=2;break;
        case LANG_FRENCH:row=3;break; case LANG_SPANISH:row=4;break;
        case LANG_JAPANESE:row=5;break;
        case LANG_PORTUGUESE:row=6;break; case LANG_RUSSIAN:row=7;break;
        case LANG_CHINESE:row=8;break; case LANG_KOREAN:row=9;break;
        default:row=0;break;
    }
    return index>=0&&index<2?text[row][index]:L"";
}

COLORREF BlendColor(COLORREF from,COLORREF to,int percent) {
    auto mix=[percent](int x,int y){ return x+(y-x)*percent/100; };
    return RGB(mix(GetRValue(from),GetRValue(to)),mix(GetGValue(from),GetGValue(to)),mix(GetBValue(from),GetBValue(to)));
}

double ColorContrastRatio(COLORREF first,COLORREF second) {
    const double firstLuminance=RelativeLuminance(GetRValue(first),GetGValue(first),GetBValue(first));
    const double secondLuminance=RelativeLuminance(GetRValue(second),GetGValue(second),GetBValue(second));
    return ((std::max)(firstLuminance,secondLuminance)+0.05)/
           ((std::min)(firstLuminance,secondLuminance)+0.05);
}

// Keep body text legible while preserving the dimmer Metro secondary-text hierarchy wherever the background
// already provides sufficient contrast.
COLORREF PaneSecondaryTextColor(COLORREF background) {
    const COLORREF white=RGB(255,255,255);
    for(int blend=50;blend<=100;blend+=5) {
        const COLORREF candidate=BlendColor(background,white,blend);
        if(ColorContrastRatio(candidate,background)>=4.5) return candidate;
    }
    return white;
}

// Disabled-state text: the darkest neutral grey that still reaches the 4.5:1 text contrast Microsoft
// recommends, so "Non disponibile" stays readable while looking greyed out.
COLORREF DisabledGreyColor(COLORREF background) {
    for(int v=96;v<=255;v++) {
        const COLORREF candidate=RGB(v,v,v);
        if(ColorContrastRatio(candidate,background)>=4.5) return candidate;
    }
    return RGB(255,255,255);
}

// The tile that only exists to say "Unavailable" must read as inactive next to the live tiles. The plain
// disabled grey still clears 4.5:1 and looked white on a dark pane, so pull it a further 30 % towards the
// background. That keeps it legible (>= 3:1) while making icon and caption visibly dimmer than their
// neighbours, the way a disabled Charm tile looked in Windows 8.
COLORREF UnavailableGreyColor(COLORREF background) {
    const COLORREF base=DisabledGreyColor(background);
    COLORREF muted=BlendColor(base,background,30);
    for(int percent=30;percent>=0;percent-=5) {
        const COLORREF candidate=BlendColor(base,background,percent);
        if(ColorContrastRatio(candidate,background)>=3.0) return candidate;
    }
    return muted;
}

COLORREF PaneInteractionFill(COLORREF background,bool pressed) {
    // A consistent, clearer hover/pressed response for Settings, Devices, Share.
    return BlendColor(background,RGB(255,255,255),pressed?28:16);
}

// Preserve white-text contrast on bright system accents by lowering only the pane/menu surface (never
// user-facing text or the separate Charms rail).
COLORREF EnsureReadableWithWhite(COLORREF c) {
    double r=GetRValue(c),g=GetGValue(c),b=GetBValue(c);
    for(int i=0;i<24 && RelativeLuminance(r,g,b)>0.18;++i) { r*=0.92; g*=0.92; b*=0.92; }
    return RGB((int)std::lround(r),(int)std::lround(g),(int)std::lround(b));
}

COLORREF GetPaneBackground() {
    return g_paneColorMode.load()==1 ? RGB(31,31,31) :
           EnsureReadableWithWhite(g_accent.load());
}

bool GetDefaultAudioEndpointVolume(ComPtr<IAudioEndpointVolume>& volume) {
    ComPtr<IMMDeviceEnumerator> enumerator;
    if(FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,
                               IID_PPV_ARGS(&enumerator)))) return false;
    ComPtr<IMMDevice> device;
    if(FAILED(enumerator->GetDefaultAudioEndpoint(eRender,eMultimedia,&device))) return false;
    return SUCCEEDED(device->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,nullptr,
                    reinterpret_cast<void**>(volume.GetAddressOf())));
}

bool QueryMasterAudioState(int* percent,bool* muted) {
    if(!percent) return false;
    try {
        ComPtr<IAudioEndpointVolume> volume;
        if(!GetDefaultAudioEndpointVolume(volume)) return false;
        float level=0.0f;
        if(FAILED(volume->GetMasterVolumeLevelScalar(&level))) return false;
        *percent=(int)std::lround((std::max)(0.0f,(std::min)(1.0f,level))*100.0f);
        if(muted) {
            BOOL value=FALSE;
            *muted=SUCCEEDED(volume->GetMute(&value)) && value!=FALSE;
        }
        return true;
    } catch (...) { return false; }
}

bool SetMasterVolumePercent(int percent) {
    try {
        ComPtr<IAudioEndpointVolume> volume;
        if(!GetDefaultAudioEndpointVolume(volume)) return false;
        percent=(std::max)(0,(std::min)(100,percent));
        const HRESULT result=volume->SetMasterVolumeLevelScalar((float)percent/100.0f,nullptr);
        if(FAILED(result)) return false;
        if(percent>0 && FAILED(volume->SetMute(FALSE,nullptr))) return false;
        return true;
    } catch (...) { return false; }
}

bool SetMasterMute(bool muted) {
    try {
        ComPtr<IAudioEndpointVolume> volume;
        if(!GetDefaultAudioEndpointVolume(volume)) return false;
        return SUCCEEDED(volume->SetMute(muted?TRUE:FALSE,nullptr));
    } catch (...) { return false; }
}

// WMI resources are scoped so every BSTR, VARIANT and COM apartment is released on every early-return path.
// The provider is queried only when the user opens or adjusts the brightness flyout; unsupported displays
// fall back to Display Settings.
class ScopedBstr {
public:
    explicit ScopedBstr(PCWSTR text) noexcept : value_(text?SysAllocString(text):nullptr) {}
    ~ScopedBstr() { if(value_) SysFreeString(value_); }
    ScopedBstr(const ScopedBstr&)=delete;
    ScopedBstr& operator=(const ScopedBstr&)=delete;
    BSTR get() const noexcept { return value_; }
    explicit operator bool() const noexcept { return value_!=nullptr; }
private:
    BSTR value_=nullptr;
};

class ScopedVariant {
public:
    ScopedVariant() noexcept { VariantInit(&value_); }
    ~ScopedVariant() { VariantClear(&value_); }
    ScopedVariant(const ScopedVariant&)=delete;
    ScopedVariant& operator=(const ScopedVariant&)=delete;
    VARIANT* get() noexcept { return &value_; }
    const VARIANT& value() const noexcept { return value_; }
private:
    VARIANT value_{};
};

class ScopedComApartment {
public:
    ScopedComApartment() noexcept : result_(CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED)),
        owns_(SUCCEEDED(result_)) {}
    ~ScopedComApartment() { if(owns_) CoUninitialize(); }
    ScopedComApartment(const ScopedComApartment&)=delete;
    ScopedComApartment& operator=(const ScopedComApartment&)=delete;
    bool ready() const noexcept { return SUCCEEDED(result_) || result_==RPC_E_CHANGED_MODE; }
private:
    HRESULT result_;
    bool owns_;
};

class WmiServicesClient {
public:
    WmiServicesClient() noexcept = default;
    WmiServicesClient(const WmiServicesClient&)=delete;
    WmiServicesClient& operator=(const WmiServicesClient&)=delete;
    bool Connect() noexcept {
        if(!apartment_.ready()) return false;
        ComPtr<IWbemLocator> locator;
        if(FAILED(CoCreateInstance(CLSID_WbemLocator,nullptr,CLSCTX_INPROC_SERVER, IID_IWbemLocator,
                                   reinterpret_cast<void**>(locator.GetAddressOf())))) return false;
        ScopedBstr nameSpace(L"ROOT\\WMI");
        if(!nameSpace || FAILED(locator->ConnectServer(nameSpace.get(),nullptr,nullptr,
            nullptr,0,nullptr,nullptr,services_.GetAddressOf()))) return false;
        return SUCCEEDED(CoSetProxyBlanket(services_.Get(),RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,nullptr,RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,nullptr,EOAC_NONE));
    }
    IWbemServices* Get() const noexcept { return services_.Get(); }
private:
    ScopedComApartment apartment_;
    ComPtr<IWbemServices> services_;
};

bool VariantToLong(const VARIANT& value,LONG* result) noexcept {
    if(!result) return false;
    switch(value.vt) {
        case VT_I1:*result=value.cVal;return true;
        case VT_UI1:*result=value.bVal;return true;
        case VT_I2:*result=value.iVal;return true;
        case VT_UI2:*result=value.uiVal;return true;
        case VT_I4:case VT_INT:*result=value.lVal;return true;
        case VT_UI4:case VT_UINT:
            if(value.ulVal>(ULONG)MAXLONG) return false;
            *result=(LONG)value.ulVal;return true;
        default:return false;
    }
}

bool QueryWmiBrightness(int* percent) noexcept {
    if(!percent) return false;
    try {
        WmiServicesClient client;
        if(!client.Connect()) return false;
        ScopedBstr language(L"WQL");
        ScopedBstr query(L"SELECT CurrentBrightness FROM WmiMonitorBrightness WHERE Active = TRUE");
        if(!language || !query) return false;
        ComPtr<IEnumWbemClassObject> enumerator;
        HRESULT hr=client.Get()->ExecQuery(language.get(),query.get(),
            WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,nullptr, enumerator.GetAddressOf());
        if(FAILED(hr)) return false;
        ComPtr<IWbemClassObject> instance;
        ULONG count=0;
        hr=enumerator->Next(2000,1,instance.GetAddressOf(),&count);
        if(FAILED(hr) || count==0 || !instance) return false;
        ScopedVariant current;
        if(FAILED(instance->Get(L"CurrentBrightness",0,current.get(),nullptr,nullptr))) return false;
        LONG value=0;
        if(!VariantToLong(current.value(),&value) || value<0 || value>100) return false;
        *percent=(int)value;
        return true;
    } catch (...) {
        Wh_Log(L"C++ exception querying WMI display brightness");
        return false;
    }
}

bool SetWmiBrightnessPercent(int percent) noexcept {
    percent=(std::max)(0,(std::min)(100,percent));
    try {
        WmiServicesClient client;
        if(!client.Connect()) return false;
        ScopedBstr language(L"WQL");
        ScopedBstr query(L"SELECT * FROM WmiMonitorBrightnessMethods WHERE Active = TRUE");
        if(!language || !query) return false;
        ComPtr<IEnumWbemClassObject> enumerator;
        HRESULT hr=client.Get()->ExecQuery(language.get(),query.get(),
            WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,nullptr, enumerator.GetAddressOf());
        if(FAILED(hr)) return false;
        ComPtr<IWbemClassObject> instance;
        ULONG count=0;
        hr=enumerator->Next(2000,1,instance.GetAddressOf(),&count);
        if(FAILED(hr) || count==0 || !instance) return false;
        ScopedVariant objectPath;
        if(FAILED(instance->Get(L"__PATH",0,objectPath.get(),nullptr,nullptr)) ||
           objectPath.value().vt!=VT_BSTR || !objectPath.value().bstrVal) return false;

        ComPtr<IWbemClassObject> classObject,inputDefinition,inputParameters,outputParameters;
        ScopedBstr className(L"WmiMonitorBrightnessMethods");
        ScopedBstr methodName(L"WmiSetBrightness");
        if(!className || !methodName ||
           FAILED(client.Get()->GetObject(className.get(),0,nullptr,classObject.GetAddressOf(),nullptr)) ||
           FAILED(classObject->GetMethod(methodName.get(),0,inputDefinition.GetAddressOf(),nullptr)) ||
           !inputDefinition || FAILED(inputDefinition->SpawnInstance(0,inputParameters.GetAddressOf())) ||
           !inputParameters) return false;

        ScopedVariant timeoutValue,brightnessValue;
        timeoutValue.get()->vt=VT_UI4;
        timeoutValue.get()->ulVal=1; // WMI timeout in seconds.
        brightnessValue.get()->vt=VT_UI1;
        brightnessValue.get()->bVal=(BYTE)percent;
        if(FAILED(inputParameters->Put(L"Timeout",0,timeoutValue.get(),0)) ||
           FAILED(inputParameters->Put(L"Brightness",0,brightnessValue.get(),0))) return false;
        hr=client.Get()->ExecMethod(objectPath.value().bstrVal,methodName.get(),0,nullptr,
            inputParameters.Get(),outputParameters.GetAddressOf(),nullptr);
        if(FAILED(hr) || !outputParameters) return false;
        ScopedVariant returnValue;
        if(FAILED(outputParameters->Get(L"ReturnValue",0,returnValue.get(),nullptr,nullptr))) return false;
        LONG code=1;
        return VariantToLong(returnValue.value(),&code) && code==0;
    } catch (...) {
        Wh_Log(L"C++ exception setting WMI display brightness");
        return false;
    }
}

enum SettingsFlyoutKind : int { FLYOUT_VOLUME=0, FLYOUT_BRIGHTNESS=1 };
int g_paneVolume=-1;
int g_paneBrightness=-1;
int g_brightnessPending=-1;
int g_flyoutKind=FLYOUT_VOLUME;
int g_flyoutTileId=11;
bool g_paneMuted=false;
bool g_volumeDragging=false;
bool g_volumeHoverSettings=false;
bool g_paneBrightnessAvailable=false;
// The brightness tile stays usable until the first probe has answered: a fresh session must not show it
// greyed out while the deferred WMI query is still on its way.
bool g_paneBrightnessProbed=false;
bool PaneBrightnessUnavailable() { return g_paneBrightnessProbed && !g_paneBrightnessAvailable; }

// Live data for the Settings tiles. This is the only charm pane that reads system state, and it does so with
// an MMDevice COM query plus two WMI round trips (the no-op brightness write probe). Running that before the
// pane was positioned made the Settings slide-in start late and stutter, while Share and Devices, which do no
// such work, glided in smoothly. The refresh now runs once the slide has finished.
void RefreshSettingsPaneMetrics() {
    int audioPercent=-1;
    bool audioMuted=false;
    if(QueryMasterAudioState(&audioPercent,&audioMuted)) {
        g_paneVolume=audioPercent;
        g_paneMuted=audioMuted;
    }
    int brightnessNow=-1;
    // A display is treated as supporting brightness only if it accepts a write; the current level written
    // back is a no-op probe.
    g_paneBrightnessAvailable=QueryWmiBrightness(&brightnessNow) && SetWmiBrightnessPercent(brightnessNow);
    g_paneBrightnessProbed=true;
}

struct PaneItem { int id; RECT r; };

// Geometry measured from Windows 8 reference screenshots (1:1 at 96 DPI). ids: 0-3 desktop links, 10-15
// system tiles, 20 "Change PC settings".
int PaneLayout(int w,int h,PaneItem* out) {
    int n=0;
    const int margin=DpiScale(40);
    for(int i=0;i<4;i++) {
        const int top=DpiScale(118)+i*DpiScale(50);
        out[n++]={i,{0,top,w,top+DpiScale(50)}};
    }
    const int changeTop=h-DpiScale(77);
    out[n++]={20,{0,changeTop,w,changeTop+DpiScale(44)}};
    const int tileW=(w-2*margin)/3, tileH=DpiScale(90);
    for(int i=0;i<6;i++) {
        const int col=i%3,row=i/3;
        const int top=h-DpiScale(280)+row*tileH;
        out[n++]={10+i,{margin+col*tileW,top,margin+(col+1)*tileW,top+tileH}};
    }
    return n;
}

int SecondaryPaneLayout(int kind,int w,int h,PaneItem* out) {
    if(kind!=PANE_DEVICES && kind!=PANE_SHARE) return 0;
    const int margin=DpiScale(32);
    if(kind==PANE_SHARE) {
        const int top=DpiScale(190);
        const int rowHeight=DpiScale(50); // same row height as the Settings pane list
        out[0]={200,{0,top,w,top+rowHeight}}; // full width, like the Settings rows
        return 1;
    }
    // Win8.1 Devices pane (reference screenshot): Play, Print and Project, in that order, each a full-width row
    // like the Settings list. Play shows the system Play To picker, Print opens Devices and Printers, and Project
    // (Windows 8 called it Second screen) opens the built-in display switch panel.
    (void)h; (void)margin;
    static const int ids[3]={100,101,102};
    const int rowTop=DpiScale(184);
    for(int i=0;i<3;i++)
        out[i]={ids[i],{0,rowTop+i*DpiScale(50),w,rowTop+(i+1)*DpiScale(50)}};
    return 3;
}

void PositionVolumeFlyout() {
    if(!g_volumeFlyout || !g_pane) return;
    RECT paneRect{};
    if(!GetWindowRect(g_pane,&paneRect)) return;
    const int paneWidth=paneRect.right-paneRect.left;
    const int paneHeight=paneRect.bottom-paneRect.top;
    PaneItem items[16]{};
    const int count=PaneLayout(paneWidth,paneHeight,items);
    RECT targetTile{};
    bool found=false;
    for(int i=0;i<count;i++) if(items[i].id==g_flyoutTileId) {
        targetTile=items[i].r;
        found=true;
        break;
    }
    if(!found) return;
    const HwFlyoutMetrics hw=HwMetrics();
    const int width=DpiScale(hw.width),height=DpiScale(hw.height);
    const int gap=DpiScale(6);
    const int paneLeft=static_cast<int>(paneRect.left);
    const int paneTop=static_cast<int>(paneRect.top);
    const int paneRight=static_cast<int>(paneRect.right);
    const int tileCenterX=(targetTile.left+targetTile.right)/2;
    int x=paneLeft+tileCenterX-width/2;
    const int iconSize=DpiScale(30);
    const int iconTop=targetTile.top+DpiScale(42)-iconSize/2;
    int y=paneTop+iconTop-height-gap;
    const int paneInset=DpiScale(6);
    x=(std::max)(paneLeft+paneInset, (std::min)(paneRight-width-paneInset,x));
    y=(std::max)(paneTop+paneInset,y);
    SetWindowPos(g_volumeFlyout,HWND_TOPMOST,x,y,width,height,SWP_NOACTIVATE);
}

void RepositionVisibleVolumeFlyout() {
    if(g_volumeFlyout && IsWindowVisible(g_volumeFlyout)) PositionVolumeFlyout();
}

void HideVolumeFlyout() {
    if(!g_volumeFlyout) return;
    KillTimer(g_volumeFlyout,TIMER_VOLUME);
    KillTimer(g_volumeFlyout,TIMER_BRIGHTNESS_APPLY);
    if(GetCapture()==g_volumeFlyout) ReleaseCapture();
    g_volumeDragging=false;
    g_brightnessPending=-1;
    g_volumeHoverSettings=false;
    HideVolumeTip();
    ShowWindow(g_volumeFlyout,SW_HIDE);
}

int g_paneHover=-1, g_panePressed=-1;
bool g_paneAnimOpening=true, g_paneMenuOpen=false;
// Pane animation clock: QueryPerformanceCounter and an 8 ms frame timer, the same timing the Display Switch
// panel uses. GetTickCount steps in ~15.6 ms, which made the slide look choppy.
constexpr UINT kPaneFrameMs=8;
LONGLONG g_paneAnimStart=0;
static LONGLONG PaneAnimNow() {
    LARGE_INTEGER c{};
    QueryPerformanceCounter(&c);
    return c.QuadPart;
}
static double PaneAnimElapsedMs(LONGLONG start) {
    LARGE_INTEGER f{};
    QueryPerformanceFrequency(&f);
    if(f.QuadPart<=0) return 0.0;
    return (double)(PaneAnimNow()-start)*1000.0/(double)f.QuadPart;
}
int g_paneAnimOffset=0; // physical px the pane is pushed off-screen
int g_paneAnimFromOffset=0, g_paneAnimToOffset=0, g_paneAnimDurationMs=0;
int g_paneContentOffset=0; // physical px of the delayed inner-content parallax
int g_paneContentFromOffset=0, g_paneContentToOffset=0, g_paneContentDurationMs=0;

// Share and Devices repaint a handful of rows per frame, so they slide smoothly. The Settings pane paints six
// GDI+ tiles, ten text runs and the header on every frame, which at 125 fps is enough work to drop frames. To
// give it the same motion as the other charms, the resting layout is rendered once into a cached bitmap when
// the slide starts and every frame is then one BitBlt shifted by the content parallax. The cache only lives
// while the pane slides; the pane is drawn the normal way before and after.
constexpr UINT kPaneMetricsDelayMs=60;
HBITMAP g_paneSlideBitmap=nullptr;
int g_paneSlideWidth=0,g_paneSlideHeight=0;
COLORREF g_paneSlideBackground=0;

void DrawPane(HDC dc,RECT rc);

void DiscardPaneSlideCache() {
    if(g_paneSlideBitmap) {
        if(!DeleteObject(g_paneSlideBitmap))
            Wh_Log(L"pane slide cache cleanup failed (%lu)",GetLastError());
        g_paneSlideBitmap=nullptr;
    }
    g_paneSlideWidth=g_paneSlideHeight=0;
}

void BuildPaneSlideCache(HWND hwnd) {
    DiscardPaneSlideCache();
    if(!hwnd || g_paneKind.load()!=PANE_SETTINGS) return;
    RECT rc{};
    if(!GetClientRect(hwnd,&rc) || rc.right<=0 || rc.bottom<=0) return;
    const HDC windowDc=GetDC(hwnd);
    if(!windowDc) return;
    ScopedDC memory(CreateCompatibleDC(windowDc));
    ScopedGdiObject bitmap(CreateCompatibleBitmap(windowDc,rc.right,rc.bottom));
    ReleaseDC(hwnd,windowDc);
    if(!memory || !bitmap) return;
    ScopedSelectObject selected(memory.get(),bitmap.get());
    if(!selected) return;
    // Cache the resting layout (content parallax at zero, no hover); each slide frame is that frame shifted.
    const int savedShift=g_paneContentOffset;
    const int savedHover=g_paneHover,savedPressed=g_panePressed;
    g_paneContentOffset=0;
    g_paneHover=g_panePressed=-1;
    DrawPane(memory.get(),rc);
    g_paneContentOffset=savedShift;
    g_paneHover=savedHover;
    g_panePressed=savedPressed;
    g_paneSlideBitmap=(HBITMAP)bitmap.release();
    g_paneSlideWidth=rc.right;
    g_paneSlideHeight=rc.bottom;
    g_paneSlideBackground=GetPaneBackground();
}

// One blit per animation frame while the Settings pane slides. Any mismatch (DPI change, accent change,
// resized pane) simply falls back to the regular double-buffered drawing path.
bool BlitPaneSlideFrame(HDC dc,const RECT& device) {
    if(!dc || !g_paneSlideBitmap || g_paneKind.load()!=PANE_SETTINGS) return false;
    if(device.right!=g_paneSlideWidth || device.bottom!=g_paneSlideHeight) return false;
    if(GetPaneBackground()!=g_paneSlideBackground) return false;
    // A hover or press highlight changes the content, so let the regular path draw those frames.
    if(g_paneHover>=0 || g_panePressed>=0) return false;
    ScopedDC memory(CreateCompatibleDC(dc));
    if(!memory) return false;
    ScopedSelectObject selected(memory.get(),g_paneSlideBitmap);
    if(!selected) return false;
    ScopedGdiObject background(CreateSolidBrush(g_paneSlideBackground));
    if(!background) return false;
    const int shift=(std::max)(0,(std::min)(g_paneSlideWidth,g_paneContentOffset));
    FillRect(dc,&device,(HBRUSH)background.get());
    if(shift<g_paneSlideWidth)
        BitBlt(dc,shift,0,g_paneSlideWidth-shift,g_paneSlideHeight,memory.get(),0,0,SRCCOPY);
    return true;
}

void PositionPane(int offset) {
    if(!g_pane) return;
    const int width=DpiScale(kPaneWidth);
    const int top=g_monitorRect.top, height=g_monitorRect.bottom-g_monitorRect.top;
    SetWindowPos(g_pane,HWND_TOPMOST,g_monitorRect.right-width+offset,top,width,height,
                 SWP_NOACTIVATE|SWP_SHOWWINDOW);
}

void ClosePaneWindow(bool immediate) {
    HideMetroContextMenu(false);
    HideVolumeFlyout();
    if(g_pane) KillTimer(g_pane,TIMER_PANE_METRICS);
    DiscardPaneSlideCache();
    if(g_paneKind.load()==PANE_SHARE && g_clock) ShowWindow(g_clock,SW_HIDE);
    if(!g_pane) return;
    if(immediate) {
        g_paneVisible.store(false);
        g_paneHover=g_panePressed=-1;
        KillTimer(g_pane,TIMER_PANE);
        g_paneAnimOffset=DpiScale(kPaneWidth);
        g_paneContentOffset=0;
        ShowWindow(g_pane,SW_HIDE);
        return;
    }
    if(!g_paneVisible.exchange(false)) return; // an existing close animation is already running
    g_paneHover=g_panePressed=-1;
    KillTimer(g_pane,TIMER_PANE);
    if(!AnimationsEnabled() || g_closeMs.load()==0) {
        g_paneAnimOffset=DpiScale(kPaneWidth);
        g_paneContentOffset=0;
        ShowWindow(g_pane,SW_HIDE);
        return;
    }
    const int full=DpiScale(kPaneWidth);
    g_paneAnimOpening=false;
    g_paneAnimStart=PaneAnimNow();
    g_paneAnimFromOffset=g_paneAnimOffset;
    g_paneAnimToOffset=full;
    const int distance=std::abs(g_paneAnimToOffset-g_paneAnimFromOffset);
    g_paneAnimDurationMs=(std::max)(80,(int)std::lround(g_closeMs.load()*(double)distance/(std::max)(1,full)));
    g_paneContentFromOffset=g_paneContentOffset;
    g_paneContentToOffset=DpiScale(16);
    g_paneContentDurationMs=g_paneAnimDurationMs;
    // Same cached-frame slide as the opening animation, so leaving Settings is as smooth as entering it.
    BuildPaneSlideCache(g_pane);
    SetTimer(g_pane,TIMER_PANE,kPaneFrameMs,nullptr);
}

bool ShowCharmPane(int kind) {
    if(!g_pane || (kind!=PANE_SETTINGS && kind!=PANE_DEVICES && kind!=PANE_SHARE)) return false;
    const bool currentPaneActive=g_paneVisible.load();
    const bool currentPaneWindowVisible=IsWindowVisible(g_pane)!=FALSE;
    if(currentPaneActive || currentPaneWindowVisible)
        return currentPaneActive && g_paneKind.load()==kind;
    if(g_visible.load() || (g_rail && IsWindowVisible(g_rail))) HideCharms(true);
    if(g_monitorRect.right<=g_monitorRect.left) {
        POINT pt{}; GetCursorPos(&pt); PlaceWindows(pt);
        if(g_monitorRect.right<=g_monitorRect.left) return false;
    }
    HideVolumeFlyout();
    KillTimer(g_pane,TIMER_PANE);
    KillTimer(g_pane,TIMER_PANE_METRICS);
    g_paneKind.store(kind);
    g_paneHover=g_panePressed=-1;
    g_paneMenuOpen=false;
    RefreshSystemAccent();
    PCWSTR title=kind==PANE_SETTINGS?LocalizedPaneText(0):LocalizedCharmPaneTitle(kind);
    SetWindowTextW(g_pane,title);
    g_paneVisible.store(true);
    const bool animate=AnimationsEnabled() && g_openMs.load()>0;
    const int full=DpiScale(kPaneWidth);
    g_paneAnimOpening=true;
    g_paneAnimStart=PaneAnimNow();
    g_paneAnimFromOffset=full;
    g_paneAnimToOffset=0;
    g_paneAnimDurationMs=(std::max)(80,(int)g_openMs.load());
    g_paneContentFromOffset=animate?DpiScale(16):0;
    g_paneContentToOffset=0;
    g_paneContentDurationMs=animate?(std::max)(80,g_paneAnimDurationMs*3/2):0;
    g_paneAnimOffset=animate?g_paneAnimFromOffset:0;
    g_paneContentOffset=animate?g_paneContentFromOffset:0;
    PositionPane(g_paneAnimOffset);
    // The Settings pane slides with cached frames; the live volume/brightness read that used to run here is
    // now deferred until the slide is over, so the first frame is on screen immediately, exactly as it is
    // for the Share and Devices panes.
    if(animate) BuildPaneSlideCache(g_pane);
    InvalidateRect(g_pane,nullptr,TRUE);
    if(kind==PANE_SHARE && g_showClock.load() &&
       ReadRegDWORD(L"DisableCharmsClock")!=1 && g_clock) {
        UpdateStatus();
        SetLayeredWindowAttributes(g_clock,0,255,LWA_ALPHA);
        ShowWindow(g_clock,SW_SHOWNOACTIVATE);
    } else if(g_clock) {
        ShowWindow(g_clock,SW_HIDE);
    }
    SetForegroundWindow(g_pane);
    if(animate) SetTimer(g_pane,TIMER_PANE,kPaneFrameMs,nullptr);
    if(kind==PANE_SETTINGS) {
        const UINT delay=animate?(UINT)((std::max)(80,g_paneAnimDurationMs)+kPaneMetricsDelayMs)
                                 :kPaneMetricsDelayMs;
        if(!SetTimer(g_pane,TIMER_PANE_METRICS,delay,nullptr)) {
            Wh_Log(L"pane metrics timer failed (%lu); refreshing now",GetLastError());
            RefreshSettingsPaneMetrics();
        }
    }
    return true;
}

bool ShowSettingsPane() { return ShowCharmPane(PANE_SETTINGS); }
bool ShowDevicesPane() { return ShowCharmPane(PANE_DEVICES); }
bool ShowSharePane() { return ShowCharmPane(PANE_SHARE); }

static void PaneStrokeIcon(Gdiplus::Graphics& g,int icon,COLORREF color,COLORREF bg) {
    Gdiplus::Pen pen(GdipColor(color),2.4f);
    pen.SetStartCap(Gdiplus::LineCapRound); pen.SetEndCap(Gdiplus::LineCapRound);
    Gdiplus::SolidBrush fill(GdipColor(color));
    switch(icon) {
        case 1: { // volume
            const Gdiplus::PointF sp[]={{3,12},{9,12},{17,5.5f},{17,26.5f},{9,20},{3,20}};
            g.FillPolygon(&fill,sp,6);
            g.DrawArc(&pen,12.0f,10.0f,10.0f,12.0f,-55.0f,110.0f);
            g.DrawArc(&pen,9.0f,5.5f,18.0f,21.0f,-55.0f,110.0f);
            break; }
        case 2: { // brightness
            g.DrawEllipse(&pen,10.5f,10.5f,11.0f,11.0f);
            for(int i=0;i<8;i++) {
                const double a=i*3.14159265358979/4.0;
                g.DrawLine(&pen,(Gdiplus::REAL)(16+std::cos(a)*9.0),(Gdiplus::REAL)(16+std::sin(a)*9.0),
                                (Gdiplus::REAL)(16+std::cos(a)*13.0),(Gdiplus::REAL)(16+std::sin(a)*13.0));
            }
            break; }
        case 3: { // notifications: solid card with text lines
            g.FillRectangle(&fill,Gdiplus::RectF(3.5f,7.0f,25.0f,18.0f));
            Gdiplus::Pen line(GdipColor(bg),1.8f);
            g.DrawLine(&line,8.0f,12.0f,24.0f,12.0f);
            g.DrawLine(&line,8.0f,16.0f,24.0f,16.0f);
            g.DrawLine(&line,8.0f,20.0f,19.0f,20.0f);
            break; }
        case 4: { // power
            g.DrawArc(&pen,6.0f,7.0f,20.0f,20.0f,-55.0f,290.0f);
            g.DrawLine(&pen,16.0f,3.5f,16.0f,16.0f);
            break; }
        default: { // keyboard
            Gdiplus::Pen thin(GdipColor(color),1.6f);
            g.DrawRectangle(&thin,3.8f,8.8f,24.4f,15.4f);
            for(int r=0;r<3;r++) for(int c=0;c<6;c++)
                g.FillRectangle(&fill,6.2f+c*3.6f,11.4f+r*3.4f,2.0f,2.0f);
            break; }
    }
}

void DrawPane(HDC dc,RECT rc) {
    const COLORREF bg=GetPaneBackground();
    ScopedGdiObject background(CreateSolidBrush(bg));
    if(background) FillRect(dc,&rc,(HBRUSH)background.get());
    const int w=rc.right,h=rc.bottom;
    const COLORREF white=RGB(255,255,255);
    const COLORREF dim=PaneSecondaryTextColor(bg);
    const int contentShift=g_paneContentOffset;
    const int margin=DpiScale(40)+contentShift;

    RECT title{margin,DpiScale(24),w-DpiScale(20)+contentShift,DpiScale(74)};
    DrawTextBox(dc,LocalizedPaneText(0),title,DpiScale(28),white,FW_LIGHT,
                DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI Light");
    RECT sub{margin,DpiScale(79),w-DpiScale(20)+contentShift,DpiScale(109)};
    DrawTextBox(dc,LocalizedPaneText(1),sub,DpiScale(15),dim,FW_NORMAL,
                DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI");

    // Thin separator above the system-wide tiles.
    {
        ScopedGdiObject line(CreateSolidBrush(BlendColor(bg,white,28)));
        RECT sep{margin,h-DpiScale(281),w-margin,h-DpiScale(281)+(std::max)(1,DpiScale(1))};
        if(line) FillRect(dc,&sep,(HBRUSH)line.get());
    }

    PaneItem items[16]{};
    const int count=PaneLayout(w,h,items);
    for(int i=0;i<count;i++) {
        PaneItem item=items[i];
        OffsetRect(&item.r,contentShift,0);
        const bool greyedOut=(item.id==12 && PaneBrightnessUnavailable());
        if(!greyedOut && (g_paneHover==item.id || g_panePressed==item.id)) {
            ScopedGdiObject hl(CreateSolidBrush(PaneInteractionFill(bg,g_panePressed==item.id)));
            if(hl) FillRect(dc,&item.r,(HBRUSH)hl.get());
        }
        if(item.id<4) {
            RECT text{margin,item.r.top,w-DpiScale(20),item.r.bottom};
            DrawTextBox(dc,LocalizedPaneText(2+item.id),text,DpiScale(15),white,FW_NORMAL,
                        DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI");
        } else if(item.id==20) {
            RECT text{margin,item.r.top,w-margin,item.r.bottom};
            DrawTextBox(dc,LocalizedPaneText(12),text,DpiScale(15),white,FW_NORMAL,
                        DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI");
        } else {
            const int idx=item.id-10;
            const bool unavailable=(idx==2 && PaneBrightnessUnavailable());
            const COLORREF ink=unavailable?UnavailableGreyColor(bg):white; // clearly dimmer than the live tiles
            const int iconSize=DpiScale(30);
            const int ix=(item.r.left+item.r.right-iconSize)/2;
            const int iy=item.r.top+DpiScale(42)-iconSize/2;
            if(idx==0) DrawNetworkStatusIcon(dc,ix-DpiScale(2),iy,DpiScale(34),DpiScale(31));
            else WithGdiplusCanvas(dc,ix,iy,iconSize,iconSize,32.0f,32.0f,
                    [idx,ink,bg](Gdiplus::Graphics& g)->bool { PaneStrokeIcon(g,idx,ink,bg); return true; });
            std::wstring label;
            if(idx==1 && g_paneVolume>=0) label=std::to_wstring(g_paneVolume);
            else if(idx==2 && unavailable) label=LocalizedPaneText(16);
            else label=LocalizedPaneText(6+idx);
            const int labelCenter=item.r.top+DpiScale(72);
            RECT cap{item.r.left+DpiScale(2),labelCenter-DpiScale(11),item.r.right-DpiScale(2),labelCenter+DpiScale(11)};
            if(!DrawRailCaptionGdiplus(dc,label.c_str(),cap,DpiScale(12),ink))
                DrawTextBox(dc,label.c_str(),cap,DpiScale(12),ink,FW_NORMAL,
                            DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
        }
    }
}

// GDI+ has no rounded-rectangle path primitive; the two glyphs traced from the Devices pane need one.
static void AddRoundRectPath(Gdiplus::GraphicsPath& path,const Gdiplus::RectF& r,Gdiplus::REAL radius) {
    const Gdiplus::REAL d=radius*2.0f;
    if(radius<=0.0f || d>r.Width || d>r.Height) { path.AddRectangle(r); return; }
    path.AddArc(r.X,r.Y,d,d,180.0f,90.0f);
    path.AddArc(r.GetRight()-d,r.Y,d,d,270.0f,90.0f);
    path.AddArc(r.GetRight()-d,r.GetBottom()-d,d,d,0.0f,90.0f);
    path.AddArc(r.X,r.GetBottom()-d,d,d,90.0f,90.0f);
    path.CloseFigure();
}

static bool DrawCharmActionGlyph(HDC dc,int kind,int x,int y,int size,COLORREF color) {
    if(kind==3)
        return DrawShareGlyphMonochrome(dc,x,y,size,size);
    return WithGdiplusCanvas(dc,x,y,size,size,32.0f,32.0f,
        [kind,color](Gdiplus::Graphics& g)->bool {
            Gdiplus::Pen pen(GdipColor(color),2.1f);
            pen.SetStartCap(Gdiplus::LineCapRound);
            pen.SetEndCap(Gdiplus::LineCapRound);
            switch(kind) {
                case 0: { // Play media on a compatible target.
                    Gdiplus::SolidBrush play(GdipColor(color));
                    const Gdiplus::PointF triangle[]={{10.0f,5.0f},{25.0f,16.0f},{10.0f,27.0f}};
                    return g.FillPolygon(&play,triangle,_countof(triangle))==Gdiplus::Ok;
                }
                case 1: { // Play, traced from the Windows 8.1 Devices pane: a rounded playback screen with a
                          // media triangle inside, the glyph the original charm used for Play To.
                    Gdiplus::GraphicsPath screen;
                    AddRoundRectPath(screen,Gdiplus::RectF(3.4f,6.6f,25.2f,18.8f),2.2f);
                    g.DrawPath(&pen,&screen);
                    Gdiplus::SolidBrush arrow(GdipColor(color));
                    const Gdiplus::PointF triangle[]={{12.7f,10.7f},{22.3f,16.0f},{12.7f,21.3f}};
                    return g.FillPolygon(&arrow,triangle,_countof(triangle))==Gdiplus::Ok;
                }
                case 2: { // Print, traced 1:1 from the Windows 8.1 Devices pane screenshot: a rounded
                          // printer body with a status dot and a status LED, a sheet feeding in from the top
                          // with three text rows, and the output tray in front of the body.
                    Gdiplus::SolidBrush ink(GdipColor(color));
                    Gdiplus::GraphicsPath body;
                    AddRoundRectPath(body,Gdiplus::RectF(2.4f,12.2f,27.4f,10.4f),1.8f);
                    g.DrawPath(&pen,&body);
                    Gdiplus::GraphicsPath sheet;
                    AddRoundRectPath(sheet,Gdiplus::RectF(9.0f,3.2f,14.0f,9.4f),0.6f);
                    g.DrawPath(&pen,&sheet);
                    g.FillEllipse(&ink,5.1f,16.5f,1.9f,1.9f);   // status dot on the body
                    g.FillEllipse(&ink,11.6f,5.2f,1.7f,1.7f);   // text row bullet on the sheet
                    g.FillRectangle(&ink,14.3f,5.9f,6.2f,1.0f); // text row
                    g.FillRectangle(&ink,11.4f,8.1f,9.3f,1.0f); // text row
                    g.FillRectangle(&ink,11.4f,10.3f,9.3f,1.0f); // text row
                    g.FillRectangle(&ink,10.4f,22.9f,11.4f,5.7f); // output tray
                    return true;
                }
                case 4: { // Mail envelope.
                    Gdiplus::GraphicsPath envelope;
                    envelope.AddRectangle(Gdiplus::RectF(4.0f,7.0f,24.0f,18.0f));
                    g.DrawPath(&pen,&envelope);
                    g.DrawLine(&pen,4.5f,8.0f,16.0f,17.0f);
                    g.DrawLine(&pen,27.5f,8.0f,16.0f,17.0f);
                    return true;
                }
                case 5: { // Project, traced from the Windows 8.1 Devices pane: a monitor behind and a
                          // smaller display with its stand in front. The front displays are clipped out of the
                          // back one so the two outlines read as separate objects, as in the original glyph.
                    Gdiplus::SolidBrush ink(GdipColor(color));
                    auto monitorFrame=[](Gdiplus::GraphicsPath& path,float x,float y,float w,float h) {
                        AddRoundRectPath(path,Gdiplus::RectF(x,y,w,h),1.1f);
                    };
                    const Gdiplus::RectF frontArea(3.2f,9.2f,17.0f,16.2f);
                    Gdiplus::Region frontGap(frontArea);
                    // Back monitor: frame plus its stand, with the area the front display occupies removed.
                    g.ExcludeClip(&frontGap);
                    Gdiplus::GraphicsPath backFrame;
                    monitorFrame(backFrame,10.6f,2.8f,16.6f,12.4f);
                    g.DrawPath(&pen,&backFrame); // the back monitor's stand is hidden behind the front one
                    g.ResetClip();
                    // Front display with its stand.
                    Gdiplus::GraphicsPath frontFrame;
                    monitorFrame(frontFrame,4.2f,10.2f,15.0f,11.6f);
                    g.DrawPath(&pen,&frontFrame);
                    const Gdiplus::PointF frontStand[]={{9.1f,21.6f},{14.3f,21.6f},{15.9f,24.2f},{7.5f,24.2f}};
                    g.FillPolygon(&ink,frontStand,_countof(frontStand));
                    return true;
                }
                default: { // Copy text/document.
                    g.DrawRectangle(&pen,9.0f,4.5f,17.0f,22.0f);
                    g.DrawLine(&pen,13.0f,11.0f,22.0f,11.0f);
                    g.DrawLine(&pen,13.0f,16.0f,22.0f,16.0f);
                    g.DrawLine(&pen,13.0f,21.0f,19.0f,21.0f);
                    g.DrawLine(&pen,5.0f,9.0f,5.0f,28.0f);
                    g.DrawLine(&pen,5.0f,28.0f,21.0f,28.0f);
                    return true;
                }
            }
        });
}

// Windows 8.1 Devices pane strings: index 0 is the empty-state sentence, 1..3 are the Play, Print and
// Project row captions. Print uses the standard Windows wording for the Devices and Printers folder.
PCWSTR LocalizedDevicesWin8Text(int index) {
    static const PCWSTR text[10][4]={
        {L"There's nothing to send right now.",L"Play",L"Print",L"Project"},
        {L"Al momento non c'è niente da inviare.",L"Riproduci",L"Stampa",L"Proietta"},
        {L"Zurzeit gibt es nichts zu senden.",L"Wiedergeben",L"Drucken",L"Projizieren"},
        {L"Il n'y a rien à envoyer pour le moment.",L"Lire",L"Imprimer",L"Projeter"},
        {L"No hay nada que enviar en este momento.",L"Reproducir",L"Imprimir",L"Proyectar"},
        {L"現在、送信するものはありません。",L"再生",L"印刷",L"投影"},
        {L"Não há nada para enviar no momento.",L"Reproduzir",L"Imprimir",L"Projetar"},
        {L"Сейчас нечего отправлять.",L"Воспроизвести",L"Печать",L"Проецировать"},
        {L"当前没有可发送的内容。",L"播放",L"打印",L"投影"},
        {L"지금은 보낼 항목이 없습니다.",L"재생",L"인쇄",L"프로젝트"}};
    int row=0;
    switch(GetUiPrimaryLanguage()) {
        case LANG_ITALIAN:row=1;break; case LANG_GERMAN:row=2;break;
        case LANG_FRENCH:row=3;break; case LANG_SPANISH:row=4;break;
        case LANG_JAPANESE:row=5;break;
        case LANG_PORTUGUESE:row=6;break; case LANG_RUSSIAN:row=7;break;
        case LANG_CHINESE:row=8;break; case LANG_KOREAN:row=9;break;
        default:row=0;break;
    }
    return index>=0&&index<4?text[row][index]:L"";
}

static void DrawDevicesPane(HDC dc,RECT rc) {
    const int w=rc.right;
    const int shift=g_paneContentOffset;
    const int margin=DpiScale(32)+shift;
    const COLORREF bg=GetPaneBackground();
    const COLORREF white=RGB(255,255,255);
    ScopedGdiObject background(CreateSolidBrush(bg));
    if(background) FillRect(dc,&rc,(HBRUSH)background.get());
    RECT titleRect{margin,DpiScale(24),w-DpiScale(20)+shift,DpiScale(74)};
    RECT subtitleRect{margin,DpiScale(79),w-DpiScale(20)+shift,DpiScale(109)};
    RECT messageRect{margin,DpiScale(126),w-DpiScale(20)+shift,DpiScale(156)};
    DrawTextBox(dc,LocalizedCharmPaneTitle(PANE_DEVICES),titleRect,DpiScale(28),white,FW_LIGHT,
                DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI Light");
    DrawTextBox(dc,LocalizedPaneText(1),subtitleRect,DpiScale(15),PaneSecondaryTextColor(bg),FW_NORMAL,
                DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI");
    DrawTextBox(dc,LocalizedDevicesWin8Text(0),messageRect,DpiScale(15),white,FW_NORMAL,
                DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI");

    PaneItem items[3]{};
    const int count=SecondaryPaneLayout(PANE_DEVICES,rc.right,rc.bottom,items);
    const int iconSize=DpiScale(30);
    for(int i=0;i<count;i++) {
        PaneItem item=items[i];
        OffsetRect(&item.r,shift,0);
        if(g_paneHover==item.id || g_panePressed==item.id) {
            ScopedGdiObject highlight(CreateSolidBrush(PaneInteractionFill(bg,g_panePressed==item.id)));
            if(highlight) FillRect(dc,&item.r,(HBRUSH)highlight.get());
        }
        // Row order matches Windows 8.1: Play, Print, Project. Glyph kinds 1, 2 and 5 are vector fac-similes
        // traced from the Windows 8.1 Devices pane reference screenshot.
        const int glyph=item.id==100?1:item.id==101?2:5;
        const int caption=item.id==100?1:item.id==101?2:3;
        const int iconX=item.r.left+DpiScale(34); // same absolute position as before the row was widened
        const int iconY=item.r.top+((item.r.bottom-item.r.top)-iconSize)/2;
        DrawCharmActionGlyph(dc,glyph,iconX,iconY,iconSize,white);
        RECT textRect{item.r.left+DpiScale(84),item.r.top,item.r.right-DpiScale(12),item.r.bottom};
        DrawTextBox(dc,LocalizedDevicesWin8Text(caption),textRect,DpiScale(15),white,FW_NORMAL,
                    DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI");
    }
}

static void DrawSharePane(HDC dc,RECT rc) {
    const int w=rc.right,h=rc.bottom;
    const int shift=g_paneContentOffset;
    const COLORREF bg=GetPaneBackground();
    const COLORREF white=RGB(255,255,255);
    ScopedGdiObject background(CreateSolidBrush(bg));
    if(background) FillRect(dc,&rc,(HBRUSH)background.get());

    // Share uses the same single pane color as Settings and Devices.

    const int margin=DpiScale(40)+shift;
    RECT title{margin,DpiScale(24),w-DpiScale(20)+shift,DpiScale(74)};
    DrawTextBox(dc,LocalizedCharmPaneTitle(PANE_SHARE),title,DpiScale(28),white,FW_LIGHT,
                DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI Light");
    RECT subtitle{margin,DpiScale(79),w-DpiScale(20)+shift,DpiScale(145)};
    DrawTextBox(dc,LocalizedSharePaneText(0),subtitle,DpiScale(14),white,FW_NORMAL,
                DT_LEFT|DT_TOP|DT_WORDBREAK|DT_NOPREFIX,L"Segoe UI");

    PaneItem items[1]{};
    if(SecondaryPaneLayout(PANE_SHARE,w,h,items)==1) {
        PaneItem item=items[0];
        OffsetRect(&item.r,shift,0);
        if(g_paneHover==item.id || g_panePressed==item.id) {
            ScopedGdiObject highlight(CreateSolidBrush(PaneInteractionFill(bg,g_panePressed==item.id)));
            if(highlight) FillRect(dc,&item.r,(HBRUSH)highlight.get());
        }
        const int iconSize=DpiScale(26);
        const int iconX=DpiScale(48)+shift;
        const int iconY=(item.r.top+item.r.bottom-iconSize)/2;
        const bool diamondDrawn=WithGdiplusCanvas(dc,iconX,iconY,iconSize,iconSize,32.0f,32.0f,
            [](Gdiplus::Graphics& g)->bool {
                Gdiplus::SolidBrush diamond(GdipColor(white));
                const Gdiplus::PointF points[]={{16.0f,1.0f},{31.0f,16.0f},
                                                {16.0f,31.0f},{1.0f,16.0f}};
                return g.FillPolygon(&diamond,points,_countof(points))==Gdiplus::Ok;
            });
        if(!diamondDrawn) {
            const POINT points[]={{iconX+iconSize/2,iconY},
                                  {iconX+iconSize,iconY+iconSize/2},
                                  {iconX+iconSize/2,iconY+iconSize},
                                  {iconX,iconY+iconSize/2}};
            ScopedGdiObject brush(CreateSolidBrush(white));
            if(brush) {
                ScopedSelectObject noPen(dc,GetStockObject(NULL_PEN));
                ScopedSelectObject selected(dc,brush.get());
                if(noPen && selected) Polygon(dc,points,_countof(points));
            }
        }
        RECT action{DpiScale(96)+shift,item.r.top,w-DpiScale(20)+shift,item.r.bottom};
        DrawTextBox(dc,LocalizedSharePaneText(1),action,DpiScale(16),white,FW_NORMAL,
                    DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,L"Segoe UI");
    }
}

static void DrawActiveCharmPane(HDC dc,RECT rc) {
    switch(g_paneKind.load()) {
        case PANE_DEVICES: DrawDevicesPane(dc,rc); break;
        case PANE_SHARE: DrawSharePane(dc,rc); break;
        default: DrawPane(dc,rc); break;
    }
}

// Win8 hardware flyout, copied from the reference screenshot: glyph on top, a wide vertical bar below it
// filled with the accent colour under a black rectangular thumb. While the thumb is dragged a Metro
// context-menu style box follows it on the left and shows the value (see VolumeTipProc).
constexpr int kVolTipWidth = 46;
constexpr int kVolTipHeight = 30;

// ---------------------------------------------------------------------------
// Volume flyout: pixel-exact copy of the Windows 8 reference screenshot. Screenshot pixels are used as
// logical pixels (96 DPI). Every value goes through DpiScale(), so the flyout keeps the same proportions at
// any DPI. Window origin = screenshot (8,8).
// ---------------------------------------------------------------------------
constexpr int kVolShotWidth = 37;          // border to border: screenshot x 8..44
constexpr int kVolShotHeight = 139;        // screenshot y 8..146
constexpr int kVolShotTrackX = 15;         // bar and thumb: columns 15..20
constexpr int kVolShotBarWidth = 6;
constexpr int kVolShotTrackTop = 40;       // thumb top at 100 %
constexpr int kVolShotTrackBottom = 129;   // bar bottom (exclusive)
constexpr int kVolShotThumbSize = 6;
constexpr int kVolShotRuleX = 6;           // light rule under the glyph
constexpr int kVolShotRuleY = 30;
constexpr int kVolShotRuleWidth = 25;
constexpr int kVolShotRuleHeight = 2;
constexpr int kVolShotGlyphX = 11;         // glyph bitmap origin in the window
constexpr int kVolShotGlyphY = 10;
constexpr int kVolShotGlyphCols = 14;
constexpr int kVolShotGlyphRows = 12;
// Glyph darkness per pixel: 0 = white ... 9 = black, sampled from the screenshot.
static const char* const kVolSpeakerNormal[kVolShotGlyphRows] = {
    "00000003510000",
    "00000037600210",
    "01110389600541",
    "15742799611262",
    "16943999615244",
    "16943999615325",
    "17943999604325",
    "16943999615244",
    "15732799612262",
    "02210379601541",
    "00000038600210",
    "00000003500000"
};
static const char* const kVolSpeakerMuted[kVolShotGlyphRows] = {
    "00000000000000",
    "00000037990009",
    "01110389990099",
    "15742799099099",
    "16943999099990",
    "16943999009990",
    "17943999009900",
    "16943999009990",
    "15732799099990",
    "02210379099099",
    "00000038990099",
    "00000000990009"
};

static const char* const kVolSunGlyph[kVolShotGlyphRows] = {
    "00000090000000",
    "00620090026000",
    "00281131182000",
    "00014878410000",
    "00018101810000",
    "49937000739940",
    "00018101810000",
    "00014878410000",
    "00281131182000",
    "00620090026000",
    "00000090000000",
    "00000040000000"
};

// Volume and brightness share the same flyout geometry (screenshot copy).
HwFlyoutMetrics HwMetrics() {
    return HwFlyoutMetrics{kVolShotWidth,kVolShotHeight,kVolShotTrackTop,
                           kVolShotTrackBottom,kVolShotBarWidth,kVolShotThumbSize};
}

void DrawVolumeFlyout(HDC dc,RECT rc) {
    if(!dc) return;
    const COLORREF white=RGB(255,255,255);
    const COLORREF black=RGB(0,0,0);
    const COLORREF charm=GetPaneBackground(); // Settings charm colour: accent or dark
    const COLORREF borderColor=RGB(170,164,172);
    const COLORREF ruleColor=RGB(241,241,241);
    const bool brightness=g_flyoutKind==FLYOUT_BRIGHTNESS;
    const bool muted=!brightness && (g_paneMuted || g_paneVolume==0);
    const int value=brightness?g_paneBrightness:g_paneVolume;
    const int percent=(std::max)(0,(std::min)(100,value));

    // Background and 1 device-pixel border on the physical window edges.
    ScopedGdiObject background(CreateSolidBrush(white));
    if(background) FillRect(dc,&rc,(HBRUSH)background.get());
    ScopedGdiObject border(CreateSolidBrush(borderColor));
    if(border) FrameRect(dc,&rc,(HBRUSH)border.get());

    // Every logical rectangle is converted edge by edge, so 1 logical px stays 1 px at 100 %.
    auto fillLogical=[dc](int x,int y,int w,int h,COLORREF color){
        RECT r{DpiScale(x),DpiScale(y),DpiScale(x+w),DpiScale(y+h)};
        ScopedGdiObject brush(CreateSolidBrush(color));
        if(brush) FillRect(dc,&r,(HBRUSH)brush.get());
    };

    fillLogical(kVolShotRuleX,kVolShotRuleY,kVolShotRuleWidth,kVolShotRuleHeight,ruleColor);

    // Glyph: speaker for volume (muted variant), sun for brightness; one cell per bitmap pixel.
    const char* const* glyph=brightness?kVolSunGlyph:(muted?kVolSpeakerMuted:kVolSpeakerNormal);
    for(int r=0;r<kVolShotGlyphRows;++r) {
        for(int c=0;c<kVolShotGlyphCols;++c) {
            const int level=glyph[r][c]-'0';
            if(level<=0) continue;
            const int v=255*(9-level)/9;
            fillLogical(kVolShotGlyphX+c,kVolShotGlyphY+r,1,1,RGB(v,v,v));
        }
    }

    // Thumb travels from the track top (100 %) to the track bottom (0 %).
    const int travelTop=kVolShotTrackTop;
    const int travelBottom=kVolShotTrackBottom-kVolShotThumbSize;
    const int thumbTop=travelBottom-(travelBottom-travelTop)*percent/100;
    // The screenshot's bar has anti-aliased edges (left column ~65 % covered, right column ~27 %, last row
    // ~35 %); the same blends are reproduced here.
    if(percent>0) {
        const int barH=kVolShotTrackBottom-thumbTop;
        fillLogical(kVolShotTrackX,thumbTop,1,barH,BlendColor(white,charm,65));
        fillLogical(kVolShotTrackX+1,thumbTop,kVolShotBarWidth-1,barH,charm);
        fillLogical(kVolShotTrackX+kVolShotBarWidth,thumbTop,1,barH,BlendColor(white,charm,27));
        fillLogical(kVolShotTrackX+1,kVolShotTrackBottom,kVolShotBarWidth-1,1,BlendColor(white,charm,35));
    }
    // Thumb: black, same cells as the screenshot (outer columns ~60 % covered).
    const COLORREF thumbEdge=BlendColor(white,black,60);
    fillLogical(kVolShotTrackX,thumbTop,1,kVolShotThumbSize,thumbEdge);
    fillLogical(kVolShotTrackX+1,thumbTop,kVolShotThumbSize-2,kVolShotThumbSize,black);
    fillLogical(kVolShotTrackX+kVolShotThumbSize-1,thumbTop,1,kVolShotThumbSize,thumbEdge);
}

// Metro context-menu style value box that follows the dragged thumb on the left (Windows 8 shows the new
// volume/brightness value there for every change).
LRESULT CALLBACK VolumeTipProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    switch(msg) {
        case WM_ERASEBKGND:return 1;
        case WM_MOUSEACTIVATE:return MA_NOACTIVATE;
        case WM_NCHITTEST:return HTTRANSPARENT;
        case WM_PAINT: {
            ScopedPaint paint(hwnd);
            HDC dc=paint.get();
            if(!dc) return 0;
            RECT rc{}; GetClientRect(hwnd,&rc);
            ScopedGdiObject background(CreateSolidBrush(RGB(255,255,255)));
            if(background) FillRect(dc,&rc,(HBRUSH)background.get());
            ScopedGdiObject frame(CreateSolidBrush(RGB(200,200,200)));
            if(frame) FrameRect(dc,&rc,(HBRUSH)frame.get());
            const int value=g_flyoutKind==FLYOUT_BRIGHTNESS?g_paneBrightness:g_paneVolume;
            const std::wstring text=value>=0?std::to_wstring(value):std::wstring(L"--");
            DrawTextBox(dc,text.c_str(),rc,DpiScale(15),RGB(32,32,32),FW_NORMAL,
                        DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX,L"Segoe UI");
            return 0; }
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

void HideVolumeTip() {
    if(g_volumeTip && IsWindowVisible(g_volumeTip)) ShowWindow(g_volumeTip,SW_HIDE);
}

void UpdateVolumeTip() {
    if(!g_volumeTip || !g_volumeFlyout || !g_volumeDragging || !IsWindowVisible(g_volumeFlyout)) {
        HideVolumeTip();
        return;
    }
    RECT flyoutRect{};
    if(!GetWindowRect(g_volumeFlyout,&flyoutRect)) return;
    const HwFlyoutMetrics hw=HwMetrics();
    const int value=g_flyoutKind==FLYOUT_BRIGHTNESS?g_paneBrightness:g_paneVolume;
    const int percent=(std::max)(0,(std::min)(100,value));
    const int radius=hw.thumbHeight/2;
    const int travelTop=hw.trackTop+radius;
    const int travelBottom=hw.trackBottom-radius;
    const int centerY=travelBottom-(travelBottom-travelTop)*percent/100;
    const int tipW=DpiScale(kVolTipWidth),tipH=DpiScale(kVolTipHeight);
    const int x=flyoutRect.left-tipW-DpiScale(6);
    const int y=flyoutRect.top+DpiScale(centerY)-tipH/2;
    SetWindowPos(g_volumeTip,HWND_TOPMOST,x,y,tipW,tipH,SWP_NOACTIVATE|SWP_SHOWWINDOW);
    InvalidateRect(g_volumeTip,nullptr,FALSE);
}

int VolumeFlyoutHitTest(HWND hwnd,int physicalX,int physicalY) {
    RECT rc{}; GetClientRect(hwnd,&rc);
    const int x=DpiUnscale(physicalX),y=DpiUnscale(physicalY),w=DpiUnscale(rc.right);
    const HwFlyoutMetrics hw=HwMetrics();
    const RECT mute{w/2-24,0,w/2+24,hw.trackTop};
    const RECT slider{0,hw.trackTop-4,w,hw.height};
    const RECT settings{0,0,0,0}; // link removed (see DrawVolumeFlyout)
    if(g_flyoutKind==FLYOUT_VOLUME && PtInRect(&mute,POINT{x,y})) return 1;
    if(PtInRect(&slider,POINT{x,y})) return 2;
    if(PtInRect(&settings,POINT{x,y})) return 3;
    return 0;
}
void UpdateVolumeFromPoint(HWND hwnd,int physicalY) {
    const int y=DpiUnscale(physicalY);
    const HwFlyoutMetrics hw=HwMetrics();
    const int top=hw.trackTop+hw.thumbHeight/2;
    const int bottom=hw.trackBottom-hw.thumbHeight/2;
    const int percent=(std::max)(0,(std::min)(100,(bottom-y)*100/(bottom-top)));
    if(g_flyoutKind==FLYOUT_BRIGHTNESS) {
        g_paneBrightness=percent;
        g_brightnessPending=percent;
        KillTimer(hwnd,TIMER_BRIGHTNESS_APPLY);
        if(!SetTimer(hwnd,TIMER_BRIGHTNESS_APPLY,100,nullptr)) {
            if(!SetWmiBrightnessPercent(percent) && !g_volumeDragging) {
                int current=-1;
                g_paneBrightness=QueryWmiBrightness(&current)?current:-1;
            }
            g_brightnessPending=-1;
        }
        InvalidateRect(hwnd,nullptr,FALSE);
        if(g_pane) InvalidateRect(g_pane,nullptr,FALSE);
        UpdateVolumeTip();
        return;
    }
    if(SetMasterVolumePercent(percent)) {
        g_paneVolume=percent;
        if(percent>0) g_paneMuted=false;
        InvalidateRect(hwnd,nullptr,FALSE);
        if(g_pane) InvalidateRect(g_pane,nullptr,FALSE);
        UpdateVolumeTip();
    }
}

void PositionVolumeFlyout();

bool ShowVolumeFlyout() {
    if(!g_volumeFlyout || !g_paneVisible.load()) return false;
    if(IsWindowVisible(g_volumeFlyout)) {
        if(g_flyoutKind==FLYOUT_VOLUME) { HideVolumeFlyout(); return true; }
        HideVolumeFlyout();
    }
    g_flyoutKind=FLYOUT_VOLUME;
    g_flyoutTileId=11;
    int percent=-1; bool muted=false;
    if(QueryMasterAudioState(&percent,&muted)) { g_paneVolume=percent; g_paneMuted=muted; }
    else { g_paneVolume=-1; g_paneMuted=false; }
    PositionVolumeFlyout();
    SetTimer(g_volumeFlyout,TIMER_VOLUME,500,nullptr);
    g_volumeHoverSettings=false;
    ShowWindow(g_volumeFlyout,SW_SHOWNOACTIVATE);
    InvalidateRect(g_volumeFlyout,nullptr,TRUE);
    if(g_pane) InvalidateRect(g_pane,nullptr,FALSE);
    return true;
}

bool ShowBrightnessFlyout() {
    if(!g_volumeFlyout || !g_paneVisible.load()) return false;
    if(IsWindowVisible(g_volumeFlyout)) {
        if(g_flyoutKind==FLYOUT_BRIGHTNESS) { HideVolumeFlyout(); return true; }
        HideVolumeFlyout();
    }
    int percent=-1;
    if(!QueryWmiBrightness(&percent)) return false;
    g_flyoutKind=FLYOUT_BRIGHTNESS;
    g_flyoutTileId=12;
    g_paneBrightness=percent;
    g_brightnessPending=-1;
    KillTimer(g_volumeFlyout,TIMER_VOLUME);
    KillTimer(g_volumeFlyout,TIMER_BRIGHTNESS_APPLY);
    PositionVolumeFlyout();
    g_volumeHoverSettings=false;
    ShowWindow(g_volumeFlyout,SW_SHOWNOACTIVATE);
    InvalidateRect(g_volumeFlyout,nullptr,TRUE);
    if(g_pane) InvalidateRect(g_pane,nullptr,FALSE);
    return true;
}

static void ApplyPendingBrightness(HWND hwnd) noexcept {
    KillTimer(hwnd,TIMER_BRIGHTNESS_APPLY);
    const int pending=g_brightnessPending;
    g_brightnessPending=-1;
    // While the thumb is dragged, keep the requested position even if this write is rejected; the real level
    // is read back only after the button is released.
    if(pending>=0 && !SetWmiBrightnessPercent(pending) && !g_volumeDragging) {
        int actual=-1;
        g_paneBrightness=QueryWmiBrightness(&actual)?actual:-1;
    }
    InvalidateRect(hwnd,nullptr,FALSE);
    if(g_pane) InvalidateRect(g_pane,nullptr,FALSE);
}

LRESULT CALLBACK VolumeFlyoutProcImpl(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    switch(msg) {
        case WM_ERASEBKGND:return 1;
        case WM_MOUSEACTIVATE:return MA_NOACTIVATE;
        case WM_PAINT: {
            ScopedPaint paint(hwnd);
            HDC dc=paint.get();
            if(!dc) return 0;
            RECT rc{}; GetClientRect(hwnd,&rc);
            DrawVolumeFlyout(dc,rc);
            return 0; }
        case WM_MOUSEMOVE: {
            if(g_volumeDragging) { UpdateVolumeFromPoint(hwnd,GET_Y_LPARAM(lp)); return 0; }
            const bool hover=VolumeFlyoutHitTest(hwnd,GET_X_LPARAM(lp),GET_Y_LPARAM(lp))==2;
            if(hover!=g_volumeHoverSettings) { g_volumeHoverSettings=hover; InvalidateRect(hwnd,nullptr,FALSE); }
            TRACKMOUSEEVENT track{sizeof(track),TME_LEAVE,hwnd,0};
            TrackMouseEvent(&track);
            return 0; }
        case WM_MOUSELEAVE:
            if(g_volumeHoverSettings) { g_volumeHoverSettings=false; InvalidateRect(hwnd,nullptr,FALSE); }
            return 0;
        case WM_SETCURSOR:
            if(LOWORD(lp)==HTCLIENT) {
                SetCursor(LoadCursorW(nullptr,IDC_ARROW));
                return TRUE;
            }
            break;
        case WM_LBUTTONDOWN: {
            const int hit=VolumeFlyoutHitTest(hwnd,GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
            if(hit==1) {
                const bool mute=!g_paneMuted;
                if(SetMasterMute(mute)) {
                    g_paneMuted=mute;
                    InvalidateRect(hwnd,nullptr,FALSE);
                    if(g_pane) InvalidateRect(g_pane,nullptr,FALSE);
                }
                return 0;
            }
            if(hit==2) {
                g_volumeDragging=true; SetCapture(hwnd);
                UpdateVolumeFromPoint(hwnd,GET_Y_LPARAM(lp));
                return 0;
            }
            if(hit==3) {
                const bool brightness=g_flyoutKind==FLYOUT_BRIGHTNESS;
                ClosePaneWindow(true);
                ShellExecuteW(nullptr,L"open",brightness?L"ms-settings:display":L"ms-settings:sound",
                              nullptr,nullptr,SW_SHOWNORMAL);
                return 0;
            }
            return 0; }
        case WM_LBUTTONUP:
            if(g_volumeDragging) {
                UpdateVolumeFromPoint(hwnd,GET_Y_LPARAM(lp));
                g_volumeDragging=false;
                if(GetCapture()==hwnd) ReleaseCapture();
                UpdateVolumeTip();
                if(g_flyoutKind==FLYOUT_BRIGHTNESS && g_brightnessPending>=0) {
                    KillTimer(hwnd,TIMER_BRIGHTNESS_APPLY);
                    if(!SetTimer(hwnd,TIMER_BRIGHTNESS_APPLY,1,nullptr))
                        ApplyPendingBrightness(hwnd);
                }
                return 0;
            }
            return 0;
        case WM_CAPTURECHANGED:
            g_volumeDragging=false; UpdateVolumeTip(); return 0;
        case WM_TIMER:
            if(wp==TIMER_BRIGHTNESS_APPLY) {
                ApplyPendingBrightness(hwnd);
                return 0;
            }
            if(wp==TIMER_VOLUME && g_flyoutKind==FLYOUT_VOLUME && !g_volumeDragging) {
                int percent=-1; bool muted=false;
                if(QueryMasterAudioState(&percent,&muted) && (percent!=g_paneVolume || muted!=g_paneMuted)) {
                    g_paneVolume=percent; g_paneMuted=muted;
                    InvalidateRect(hwnd,nullptr,FALSE);
                    if(g_pane) InvalidateRect(g_pane,nullptr,FALSE);
                }
                return 0;
            }
            break;
        case WM_POWERBROADCAST:
            if(wp==PBT_POWERSETTINGCHANGE && lp) {
                const auto* setting=reinterpret_cast<const POWERBROADCAST_SETTING*>(lp);
                if((IsEqualGUID(setting->PowerSetting,kGuidCurrentMonitorBrightness) ||
                    IsEqualGUID(setting->PowerSetting,kGuidVideoPolicyBrightness)) &&
                   setting->DataLength>=sizeof(DWORD)) {
                    DWORD level=0;
                    memcpy(&level,setting->Data,sizeof(level));
                    if(level<=100 && !g_volumeDragging && g_brightnessPending<0) {
                        g_paneBrightness=(int)level;
                        if(g_flyoutKind==FLYOUT_BRIGHTNESS) InvalidateRect(hwnd,nullptr,FALSE);
                        if(g_pane) InvalidateRect(g_pane,nullptr,FALSE);
                    }
                }
            }
            return TRUE;
        case WM_CLOSE:HideVolumeFlyout();return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

LRESULT CALLBACK VolumeFlyoutProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    try { return VolumeFlyoutProcImpl(hwnd,msg,wp,lp); }
    catch (...) { Wh_Log(L"C++ exception in volume/brightness flyout"); return DefWindowProcW(hwnd,msg,wp,lp); }
}

static void PaneRun(PCWSTR file,PCWSTR params=nullptr) {
    ShellExecuteW(nullptr,L"open",file,params,nullptr,SW_SHOWNORMAL);
}

static bool PaneTryOpen(PCWSTR target,PCWSTR parameters=nullptr) {
    const HINSTANCE result=ShellExecuteW(nullptr,L"open",target,parameters,nullptr,SW_SHOWNORMAL);
    return (INT_PTR)result>32;
}

static bool EnterSystemSleepState() noexcept {
    ScopedHandle token;
    if(!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES,token.put())) {
        Wh_Log(L"opening the process token for Sleep failed (%lu)",GetLastError());
        return false;
    }
    ScopedShutdownPrivilege privilege;
    if(!privilege.Enable(token.get())) {
        Wh_Log(L"enabling the shutdown privilege for Sleep failed (%lu)",GetLastError());
        return false;
    }
    const BOOLEAN slept=SetSuspendState(FALSE,FALSE,FALSE);
    const DWORD sleepError=GetLastError();
    if(!slept)
        Wh_Log(L"entering the Sleep state failed (%lu)",sleepError);
    return slept!=FALSE;
}

static bool MetroMenuItemSelectable(int index) noexcept {
    return index>=0 && index<(int)g_metroMenuItems.size() && !g_metroMenuItems[(size_t)index].separator &&
           g_metroMenuItems[(size_t)index].enabled;
}

static int MetroMenuStep(int from,int direction) noexcept {
    const int count=(int)g_metroMenuItems.size();
    if(count<=0) return -1;
    int index=(from<0 || from>=count)?(direction>0?-1:count):from;
    for(int step=0;step<count;step++) {
        index+=direction;
        if(index<0) index=count-1;
        if(index>=count) index=0;
        if(MetroMenuItemSelectable(index)) return index;
    }
    return -1;
}

static void MetroMenuSetHover(HWND hwnd,int index) noexcept {
    if(index!=g_metroMenuHover) {
        g_metroMenuHover=index;
        InvalidateRect(hwnd,nullptr,FALSE);
    }
}

static int MetroMenuHitTest(HWND hwnd,int x,int y) noexcept {
    RECT rc{};
    if(!GetClientRect(hwnd,&rc) || x<0 || x>=DpiUnscale(rc.right)) return -1;
    const int relative=y-kMetroMenuPadY;
    if(relative<0) return -1;
    const int index=relative/kMetroMenuItemHeight;
    return MetroMenuItemSelectable(index)?index:-1;
}

static void PlaceMetroMenu(POINT& point,int width,int height) noexcept {
    RECT work{0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)};
    const HMONITOR monitor=MonitorFromPoint(point,MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{sizeof(info)};
    if(monitor && GetMonitorInfoW(monitor,&info)) work=info.rcWork;
    if(point.x+width>work.right) point.x-=width;
    if(point.y+height>work.bottom) point.y-=height;
    const LONG maxX=work.right-static_cast<LONG>(width);
    const LONG maxY=work.bottom-static_cast<LONG>(height);
    point.x=(std::max)(work.left,(std::min)(point.x,maxX));
    point.y=(std::max)(work.top,(std::min)(point.y,maxY));
}

static bool ShowMetroContextMenu(int purpose,POINT anchor,bool fromKeyboard,HWND owner) {
    if(!g_menu || !IsWindow(g_menu) || g_metroMenuItems.empty()) return false;
    if(g_metroMenuActive) {
        std::vector<MetroMenuItem> newItems;
        newItems.swap(g_metroMenuItems);
        HideMetroContextMenu(false);
        g_metroMenuItems.swap(newItems);
    }
    const int width=DpiScale(kMetroMenuWidth);
    const int height=DpiScale(2*kMetroMenuPadY+ (int)g_metroMenuItems.size()*kMetroMenuItemHeight);
    PlaceMetroMenu(anchor,width,height);
    g_metroMenuPurpose=purpose;
    g_metroMenuHover=fromKeyboard?MetroMenuStep(-1,1):-1;
    g_metroMenuActive=true;
    g_paneMenuOpen=purpose==METRO_MENU_POWER;
    SetWindowLongPtrW(g_menu,GWLP_HWNDPARENT,(LONG_PTR)owner);
    if(!SetWindowPos(g_menu,HWND_TOPMOST,anchor.x,anchor.y,width,height,SWP_SHOWWINDOW)) {
        HideMetroContextMenu(false);
        Wh_Log(L"positioning the Metro context menu failed (%lu)",GetLastError());
        return false;
    }
    SetLayeredWindowAttributes(g_menu,0,255,LWA_ALPHA);
    InvalidateRect(g_menu,nullptr,TRUE);
    if(!SetForegroundWindow(g_menu))
        Wh_Log(L"foreground activation of the Metro menu was declined");
    SetFocus(g_menu);
    return true;
}

void HideMetroContextMenu(bool restoreFocus) {
    if(!g_metroMenuActive) {
        g_paneMenuOpen=false;
        if(g_menu && IsWindowVisible(g_menu)) ShowWindow(g_menu,SW_HIDE);
        if(g_menu) SetWindowLongPtrW(g_menu,GWLP_HWNDPARENT,0);
        g_metroMenuItems.clear();
        return;
    }
    const int oldPurpose=g_metroMenuPurpose;
    g_metroMenuActive=false;
    g_metroMenuPurpose=METRO_MENU_NONE;
    g_metroMenuHover=-1;
    if(oldPurpose==METRO_MENU_POWER) g_paneMenuOpen=false;
    if(g_menu) {
        ShowWindow(g_menu,SW_HIDE);
        SetWindowLongPtrW(g_menu,GWLP_HWNDPARENT,0);
        InvalidateRect(g_menu,nullptr,TRUE);
    }
    g_metroMenuItems.clear();
    if(restoreFocus && oldPurpose==METRO_MENU_POWER && g_pane && g_paneVisible.load()) {
        SetForegroundWindow(g_pane);
        SetFocus(g_pane);
    }
}

static void FinishMetroMenu(HWND hwnd,int command,bool outside,bool restoreFocus) {
    if(!g_metroMenuActive) return;
    const int purpose=g_metroMenuPurpose;
    HideMetroContextMenu(false);
    if(command>0) {
        if(g_threadId) PostThreadMessageW(g_threadId,WM_PORT_MENU_COMMAND, (WPARAM)purpose,(LPARAM)command);
    } else if(outside && purpose==METRO_MENU_POWER) {
        if(g_threadId) PostThreadMessageW(g_threadId,WM_PORT_PANECLOSE,0,0);
    } else if(restoreFocus && purpose==METRO_MENU_POWER && g_pane && g_paneVisible.load()) {
        SetForegroundWindow(g_pane);
        SetFocus(g_pane);
    }
    (void)hwnd;
}

static bool PointInsideWindow(HWND hwnd,POINT point) noexcept {
    RECT rect{};
    return hwnd && GetWindowRect(hwnd,&rect) && PtInRect(&rect,point)!=FALSE;
}

void ShowPowerMetroMenu(HWND hwnd) {
    try {
        g_metroMenuItems.clear();
        g_metroMenuItems.push_back({LocalizedPaneText(13),1,false,true});
        g_metroMenuItems.push_back({LocalizedPaneText(14),2,false,true});
        g_metroMenuItems.push_back({LocalizedPaneText(15),3,false,true});
        RECT rc{};
        if(!GetClientRect(hwnd,&rc)) return;
        PaneItem items[16]{};
        const int count=PaneLayout(rc.right,rc.bottom,items);
        POINT anchor{rc.left+DpiScale(40),rc.bottom-DpiScale(145)};
        for(int i=0;i<count;i++) if(items[i].id==14) {
            anchor.x=items[i].r.left-DpiScale(2);
            anchor.y=items[i].r.top+DpiScale(10);
            break;
        }
        ClientToScreen(hwnd,&anchor);
        if(!ShowMetroContextMenu(METRO_MENU_POWER,anchor,false,hwnd)) g_paneMenuOpen=false;
    } catch (...) {
        g_paneMenuOpen=false;
        HideMetroContextMenu(false);
        Wh_Log(L"exception while preparing the Power context menu");
    }
}

void ShowQuickMetroMenu(POINT anchor) {
    try {
        g_metroMenuItems.clear();
        for(int i=0;i<3;i++)
            g_metroMenuItems.push_back({LocalizedQuickMenuItem(i),i+1,false,true});
        ShowMetroContextMenu(METRO_MENU_QUICK,anchor,false,g_rail);
    } catch (...) {
        HideMetroContextMenu(false);
        Wh_Log(L"exception while preparing the quick context menu");
    }
}

void DispatchMetroMenuCommand(int purpose,int command) {
    if(purpose==METRO_MENU_QUICK) {
        if(command==1) ShellExecuteW(nullptr,L"open",L"ms-settings:network-status",nullptr,nullptr,SW_SHOWNORMAL);
        else if(command==2) ShellExecuteW(nullptr,L"open",L"ms-settings:display",nullptr,nullptr,SW_SHOWNORMAL);
        else if(command==3) ShellExecuteW(nullptr,L"open",L"ms-settings:",nullptr,nullptr,SW_SHOWNORMAL);
        return;
    }
    if(purpose!=METRO_MENU_POWER || command<1 || command>3) return;
    ClosePaneWindow(true);
    if(command==1) EnterSystemSleepState();
    else if(command==2) PaneRun(L"shutdown.exe",L"/s /t 0");
    else PaneRun(L"shutdown.exe",L"/r /t 0");
}

// Windows 8 Devices > Play opened the system Play To picker. The public WinRT entry point for that picker is
// PlayToManager.ShowPlayToUI; it is deprecated but shipped on every Windows 10/11 build, and calling it needs
// no projection headers, no focus forgery and no Win+K chord. If it fails we fall back to the modern Connect
// chord, which on Windows 10/11 is the closest surviving surface.
static bool ShowPlayToPicker() {
    // PlayToManager is a UWP contract: ShowPlayToUI needs a CoreWindow for the calling app, so from
    // explorer.exe it normally fails with 0x80070578 (ERROR_INVALID_WINDOW_HANDLE). Trying it costs a
    // round trip on every click, so the first failure is remembered for this process and later clicks go
    // straight to the Connect surface.
    static bool playToUnavailable=false;
    if(playToUnavailable) return false;
    bool shown=false;
    try {
        ScopedHString runtimeClass;
        constexpr PCWSTR className=L"Windows.Media.PlayTo.PlayToManager";
        HRESULT hr=WindowsCreateString(className,(UINT32)wcslen(className),runtimeClass.put());
        if(SUCCEEDED(hr)) {
            ComPtr<IPlayToManagerStaticsLocal> statics;
            hr=RoGetActivationFactory(runtimeClass.get(),kIidPlayToManagerStatics,
                                      (void**)statics.GetAddressOf());
            if(SUCCEEDED(hr) && statics) {
                hr=statics->ShowPlayToUI();
                if(SUCCEEDED(hr)) {
                    shown=true;
                    Wh_Log(L"Play To picker shown through PlayToManager.ShowPlayToUI");
                } else {
                    Wh_Log(L"PlayToManager.ShowPlayToUI failed (0x%08X); "
                           L"using the Connect surface for the rest of this session",(unsigned)hr);
                    playToUnavailable=true;
                }
            } else {
                Wh_Log(L"Play To activation factory unavailable (0x%08X); "
                       L"using the Connect surface for the rest of this session",(unsigned)hr);
                playToUnavailable=true;
            }
        }
    } catch (...) {
        Wh_Log(L"C++ exception while opening the Play To picker");
        playToUnavailable=true;
    }
    return shown;
}

// Devices and Printers, the shell folder the Windows 8 Print charm opened.
constexpr wchar_t kDevicesAndPrintersClsid[]=L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}";

static void ActivateDevicesPaneItem(int id) {
    ClosePaneWindow(true);
    CompleteCharmAction();
    if(id==100) {
        if(!ShowPlayToPicker()) {
            Wh_Log(L"Devices > Play opened through Win+K (Connect)");
            SendWinChord('K'); // Windows 10/11 Connect surface, the modern Play To picker.
        }
    } else if(id==101) {
        if(!PaneTryOpen(kDevicesAndPrintersClsid))
            PaneRun(L"control.exe",L"/name Microsoft.DevicesAndPrinters");
    } else if(id==102) {
        // The Second screen / Project panel: first try the companion Display Switch mod, then
        // the built-in panel, then fall back to Win+P.
        if(!DisplaySwitchMod::TogglePanel())
            SendWinChord('P'); // native Windows Project flyout
    }
}

static void ActivateSharePaneItem(int id) {
    if(id!=200) return;
    ClosePaneWindow(true);
    HideCharms(true); // Keep the recreated pane, rail and clock out of the selected desktop area.
    HINSTANCE launched=ShellExecuteW(nullptr,L"open",L"SnippingTool.exe",nullptr,
                                      nullptr,SW_SHOWNORMAL);
    if((INT_PTR)launched<=32)
        Wh_Log(L"SnippingTool.exe launch failed (%ld)",(LONG)(INT_PTR)launched);
}

void ActivatePaneItem(HWND hwnd,int id) {
    const int kind=g_paneKind.load();
    if(kind==PANE_DEVICES) { ActivateDevicesPaneItem(id); return; }
    if(kind==PANE_SHARE) { ActivateSharePaneItem(id); return; }
    switch(id) {
        case 0: ClosePaneWindow(true); PaneRun(L"control.exe"); break;
        case 1: ClosePaneWindow(true); PaneRun(L"explorer.exe",L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"); break;
        case 2: ClosePaneWindow(true); PaneRun(L"control.exe",L"/name Microsoft.System"); break;
        case 3: ClosePaneWindow(true); PaneRun(L"https://support.microsoft.com/windows"); break;
        case 10: ClosePaneWindow(true); PaneRun(L"ms-settings:network-status"); break;
        case 11:
            if(!ShowVolumeFlyout()) { ClosePaneWindow(true); PaneRun(L"ms-settings:sound"); }
            break;
        case 12:
            if(PaneBrightnessUnavailable()) break; // greyed out: nothing to do
            if(!ShowBrightnessFlyout()) { ClosePaneWindow(true); PaneRun(L"ms-settings:display"); }
            break;
        case 13: ClosePaneWindow(true); PaneRun(L"ms-settings:notifications"); break;
        case 15:
            ClosePaneWindow(true);
            PaneRun(L"explorer.exe",L"shell:::{725BE8F7-668E-4C7B-8F90-46BDB0936430}");
            break;
        case 20: ClosePaneWindow(true); PaneRun(L"ms-settings:"); break;
        case 14: ShowPowerMetroMenu(hwnd); break;
    }
}

// A tile that only reads "Unavailable" is drawn dimmer and must not behave like a live tile: it is skipped
// by the hit test, so it gets no hover highlight, no hand cursor and no click.
bool PaneItemSelectable(int id) {
    return !(g_paneKind.load()==PANE_SETTINGS && id==12 && PaneBrightnessUnavailable());
}

int PaneHitTest(HWND hwnd,int x,int y) {
    RECT rc{}; GetClientRect(hwnd,&rc);
    x-=g_paneContentOffset;
    PaneItem items[16]{};
    const int kind=g_paneKind.load();
    const int count=kind==PANE_SETTINGS?PaneLayout(rc.right,rc.bottom,items):
                    SecondaryPaneLayout(kind,rc.right,rc.bottom,items);
    for(int i=0;i<count;i++) {
        const RECT& r=items[i].r;
        if(x>=r.left&&x<r.right&&y>=r.top&&y<r.bottom)
            return PaneItemSelectable(items[i].id)?items[i].id:-1;
    }
    return -1;
}

LRESULT CALLBACK PaneProcImpl(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    switch(msg) {
        case WM_ERASEBKGND: return 1;
        case WM_MOUSEACTIVATE: return MA_ACTIVATE;
        case WM_PAINT: {
            ScopedPaint paint(hwnd);
            HDC dc=paint.get();
            if(!dc) return 0;
            RECT device{};
            if(!GetClientRect(hwnd,&device) || device.right<=0 || device.bottom<=0) return 0;
            // Cheap slide frames: the cached Settings layout, shifted by the content parallax.
            if(BlitPaneSlideFrame(dc,device)) return 0;
            ScopedDC memory(CreateCompatibleDC(dc));
            ScopedGdiObject bitmap(CreateCompatibleBitmap(dc,device.right,device.bottom));
            if(!memory || !bitmap) return 0;
            ScopedSelectObject selected(memory.get(),bitmap.get());
            if(!selected) return 0;
            DrawActiveCharmPane(memory.get(),device);
            BitBlt(dc,0,0,device.right,device.bottom,memory.get(),0,0,SRCCOPY);
            return 0; }
        case WM_MOUSEMOVE: {
            const int id=PaneHitTest(hwnd,GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
            if(id!=g_paneHover) {
                g_paneHover=id; InvalidateRect(hwnd,nullptr,FALSE);
                TRACKMOUSEEVENT track{sizeof(track),TME_LEAVE,hwnd,0};
                TrackMouseEvent(&track);
            }
            return 0; }
        case WM_MOUSELEAVE:
            g_paneHover=-1; InvalidateRect(hwnd,nullptr,FALSE); return 0;
        case WM_SETCURSOR:
            if(LOWORD(lp)==HTCLIENT && g_paneHover>=0) { SetCursor(LoadCursorW(nullptr,IDC_HAND)); return TRUE; }
            break;
        case WM_LBUTTONDOWN:
            g_panePressed=PaneHitTest(hwnd,GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
            if(g_panePressed!=11) HideVolumeFlyout();
            InvalidateRect(hwnd,nullptr,FALSE); SetCapture(hwnd); return 0;
        case WM_LBUTTONUP: {
            ReleaseCapture();
            const int id=PaneHitTest(hwnd,GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
            const int pressed=g_panePressed;
            g_panePressed=-1; InvalidateRect(hwnd,nullptr,FALSE);
            if(id>=0 && id==pressed) ActivatePaneItem(hwnd,id);
            return 0; }
        case WM_KEYDOWN:
            if(wp==VK_ESCAPE) { ClosePaneWindow(false); return 0; }
            break;
        case WM_ACTIVATE:
            if(LOWORD(wp)==WA_INACTIVE && !g_paneMenuOpen && g_paneVisible.load())
                PostThreadMessageW(g_threadId,WM_PORT_PANECLOSE,0,0);
            return 0;
        case WM_TIMER:
            if(wp==TIMER_PANE_METRICS) {
                KillTimer(hwnd,TIMER_PANE_METRICS);
                RefreshSettingsPaneMetrics();
                InvalidateRect(hwnd,nullptr,TRUE);
                return 0;
            }
            if(wp==TIMER_PANE) {
                const bool opening=g_paneAnimOpening;
                const double elapsedMs=PaneAnimElapsedMs(g_paneAnimStart);
                const int paneDuration=(std::max)(80,g_paneAnimDurationMs);
                const int contentDuration=(std::max)(80,g_paneContentDurationMs);
                const double t=(std::min)(1.0,elapsedMs/paneDuration);
                const double tc=(std::min)(1.0,elapsedMs/contentDuration);
                const double eased=1.0-std::pow(1.0-t,3.0);
                const double contentEased=1.0-std::pow(1.0-tc,3.0);
                g_paneAnimOffset=(int)std::lround(g_paneAnimFromOffset+
                    (g_paneAnimToOffset-g_paneAnimFromOffset)*eased);
                g_paneContentOffset=(int)std::lround(g_paneContentFromOffset+
                    (g_paneContentToOffset-g_paneContentFromOffset)*contentEased);
                if(t>=1.0) g_paneAnimOffset=g_paneAnimToOffset;
                if(tc>=1.0) g_paneContentOffset=g_paneContentToOffset;
                PositionPane(g_paneAnimOffset);
                InvalidateRect(hwnd,nullptr,FALSE);
                UpdateWindow(hwnd); // draw this frame now instead of waiting for WM_PAINT
                if(t>=1.0 && tc>=1.0) {
                    KillTimer(hwnd,TIMER_PANE);
                    DiscardPaneSlideCache();
                    // Repaint once through the regular path so hover states and live labels are current.
                    if(opening) InvalidateRect(hwnd,nullptr,TRUE);
                    else ShowWindow(hwnd,SW_HIDE);
                }
                return 0;
            }
            break;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

LRESULT CALLBACK PaneProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    try { return PaneProcImpl(hwnd,msg,wp,lp); }
    catch (...) { Wh_Log(L"C++ exception in recreated charm pane"); return DefWindowProcW(hwnd,msg,wp,lp); }
}

LRESULT PaintRailWindow(HWND hwnd) {
    try {
        ScopedPaint paint(hwnd);
        HDC dc=paint.get();
        if(!dc) return 0;
        RECT device{};
        if(!GetClientRect(hwnd,&device) || device.right<=0 || device.bottom<=0) return 0;
        ScopedDC memory(CreateCompatibleDC(dc));
        ScopedGdiObject bitmap(CreateCompatibleBitmap(dc,device.right,device.bottom));
        if(!memory || !bitmap) return 0;
        ScopedSelectObject selected(memory.get(),bitmap.get());
        if(!selected) return 0;
        RECT physical{0,0,device.right,device.bottom};
        DrawRail(memory.get(),physical);
        BitBlt(dc,0,0,device.right,device.bottom,memory.get(),0,0,SRCCOPY);
        return 0;
    } catch (...) {
        Wh_Log(L"C++ exception while painting rail");
        return 0;
    }
}

LRESULT PaintClockWindow(HWND hwnd) {
    try {
        ScopedPaint paint(hwnd);
        HDC dc=paint.get();
        if(!dc) return 0;
        RECT device{};
        if(!GetClientRect(hwnd,&device) || device.right<=0 || device.bottom<=0) return 0;
        ScopedDC memory(CreateCompatibleDC(dc));
        ScopedGdiObject bitmap(CreateCompatibleBitmap(dc,device.right,device.bottom));
        if(!memory || !bitmap) return 0;
        ScopedSelectObject selected(memory.get(),bitmap.get());
        if(!selected) return 0;
        const bool mapped=ConfigureDpiCanvas(memory.get());
        RECT logical{0,0,mapped?DpiUnscale(device.right):device.right,
                          mapped?DpiUnscale(device.bottom):device.bottom};
        DrawClock(memory.get(),logical);
        if(mapped)
            StretchBlt(dc,0,0,device.right,device.bottom,memory.get(),
                       0,0,logical.right,logical.bottom,SRCCOPY);
        else
            BitBlt(dc,0,0,device.right,device.bottom,memory.get(),0,0,SRCCOPY);
        return 0;
    } catch (...) {
        Wh_Log(L"C++ exception while painting clock/status");
        return 0;
    }
}

void DrawMetroMenu(HDC dc,RECT rc) {
    if(!dc) return;
    ScopedGdiObject background(CreateSolidBrush(RGB(255,255,255)));
    if(background) FillRect(dc,&rc,(HBRUSH)background.get());
    ScopedGdiObject border(CreateSolidBrush(RGB(200,200,200)));
    if(border) FrameRect(dc,&rc,(HBRUSH)border.get());

    const COLORREF accent=EnsureReadableWithWhite(g_accent.load());
    for(size_t i=0;i<g_metroMenuItems.size();++i) {
        const MetroMenuItem& item=g_metroMenuItems[i];
        RECT row{1,kMetroMenuPadY+(int)i*kMetroMenuItemHeight,
                 rc.right-1,kMetroMenuPadY+(int)(i+1)*kMetroMenuItemHeight};
        if(item.separator) {
            const int mid=(row.top+row.bottom)/2;
            RECT line{kMetroMenuPadX,mid,rc.right-kMetroMenuPadX,mid+1};
            ScopedGdiObject separator(CreateSolidBrush(RGB(224,224,224)));
            if(separator) FillRect(dc,&line,(HBRUSH)separator.get());
            continue;
        }
        const bool hovered=(int)i==g_metroMenuHover && item.enabled;
        if(hovered) {
            ScopedGdiObject highlight(CreateSolidBrush(accent));
            if(highlight) FillRect(dc,&row,(HBRUSH)highlight.get());
        }
        RECT text=row;
        text.left+=kMetroMenuPadX;
        text.right-=kMetroMenuPadX;
        const COLORREF color=!item.enabled?RGB(150,150,150):
            (hovered?RGB(255,255,255):RGB(32,32,32));
        DrawTextBox(dc,item.text.c_str(),text,15,color,FW_NORMAL,
                    DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS,
                    L"Segoe UI");
    }
}

LRESULT PaintMenuWindow(HWND hwnd) {
    try {
        ScopedPaint paint(hwnd);
        HDC dc=paint.get();
        if(!dc) return 0;
        RECT device{};
        if(!GetClientRect(hwnd,&device) || device.right<=0 || device.bottom<=0) return 0;
        ScopedDC memory(CreateCompatibleDC(dc));
        ScopedGdiObject bitmap(CreateCompatibleBitmap(dc,device.right,device.bottom));
        if(!memory || !bitmap) {
            const bool mapped=ConfigureDpiCanvas(dc);
            RECT logical{0,0,mapped?DpiUnscale(device.right):device.right,
                              mapped?DpiUnscale(device.bottom):device.bottom};
            DrawMetroMenu(dc,logical);
            return 0;
        }
        ScopedSelectObject selected(memory.get(),bitmap.get());
        if(!selected) return 0;
        const bool mapped=ConfigureDpiCanvas(memory.get());
        RECT logical{0,0,mapped?DpiUnscale(device.right):device.right,
                          mapped?DpiUnscale(device.bottom):device.bottom};
        DrawMetroMenu(memory.get(),logical);
        if(mapped)
            StretchBlt(dc,0,0,device.right,device.bottom,memory.get(),
                       0,0,logical.right,logical.bottom,SRCCOPY);
        else
            BitBlt(dc,0,0,device.right,device.bottom,memory.get(),0,0,SRCCOPY);
        return 0;
    } catch (...) {
        Wh_Log(L"C++ exception while painting Metro context menu");
        return 0;
    }
}

void UpdateAnimation() {
    if (!g_rail || g_animQpcFrequency <= 0 || g_animDurationMs <= 0) return;
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);
    const double elapsedMs = (double)(now.QuadPart - g_animStartQpc) * 1000.0 /
                             (double)g_animQpcFrequency;
    const double t = (std::min)(1.0, elapsedMs / g_animDurationMs);
    const double remaining = 1.0 - t;
    const double eased = 1.0 - remaining * remaining * remaining; // cubic ease-out
    const int targetAlpha = g_animTarget.load();
    const int alpha = t >= 1.0 ? targetAlpha : (int)(g_animFromAlpha +
        (targetAlpha - g_animFromAlpha) * eased + 0.5);
    const int slide = t >= 1.0 ? g_animToSlide : (int)(g_animFromSlide +
        (g_animToSlide - g_animFromSlide) * eased + 0.5);
    ApplyRailFrame(alpha, slide);

    if (t >= 1.0) {
        KillTimer(g_rail, TIMER_ANIMATION);
        if (targetAlpha == 0) {
            KillTimer(g_rail, TIMER_STATUS);
            if (g_clock) ShowWindow(g_clock, SW_HIDE);
            ShowWindow(g_rail, SW_HIDE);
        }
    }
}

LRESULT CALLBACK RailProcImpl(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    switch(msg) {
        case WM_ERASEBKGND:return 1;
        case 0x02E0: HandleDpiChanged(hwnd,wp); return 0; // WM_DPICHANGED (Win8.1+).
        case WM_PAINT: return PaintRailWindow(hwnd);
        case WM_TIMER:
            if (wp == TIMER_ANIMATION) UpdateAnimation();
            else if (wp == TIMER_SEARCH_VERIFY) {
                KillTimer(hwnd,TIMER_SEARCH_VERIFY);
                VerifySearchChord();
            }
            else if (wp == TIMER_START_VERIFY) {
                KillTimer(hwnd,TIMER_START_VERIFY);
                VerifyCompanionStartScreen();
            }
            else if (wp == TIMER_STATUS) {
                // Only poll registry, accent and status while the rail (or clock) is actually visible.
                // When hidden, these values are refreshed on ShowCharms / ShowCharmPane instead.
                if (!g_visible.load() && (!g_clock || !IsWindowVisible(g_clock))) break;
                RefreshEdgeUiRegistry(); RefreshSystemAccent();
                if (!g_clockStatusTimerActive && g_clock && IsWindowVisible(g_clock)) UpdateStatus();
                if (g_visible.load()) RaiseCharmsOverCompanionStartScreen();
                InvalidateRect(g_rail, nullptr, FALSE);
            }
            return 0;
        case WM_MOUSEMOVE:{
            const int y=DpiUnscale(GET_Y_LPARAM(lp));
            const int x=DpiUnscale(GET_X_LPARAM(lp));
            int id=-1; RECT rc{}; GetClientRect(hwnd,&rc);
            const int width=DpiUnscale(rc.right),height=DpiUnscale(rc.bottom);
            for(int i=0;i<GetCharmSlotCount();i++) {
                const int top=GetCharmTop(i,height);
                const int charm=GetCharmAction(i);
                if(charm>=0&&!g_hidden[charm]&&x>=0&&x<width&&y>=top&&y<top+kGridCell) id=i;
            }
            g_keyboardOpened=false;
            if(g_hover.exchange(id)!=id) InvalidateRect(hwnd,nullptr,FALSE);
            return 0;
        }
        case WM_LBUTTONDOWN:{
            const int y=DpiUnscale(GET_Y_LPARAM(lp));
            RECT rc{}; GetClientRect(hwnd,&rc);
            const int height=DpiUnscale(rc.bottom);
            for(int i=0;i<GetCharmSlotCount();i++) {
                const int top=GetCharmTop(i,height);
                const int charm=GetCharmAction(i);
                if(charm>=0&&!g_hidden[charm]&&y>=top&&y<top+kGridCell) {
                    g_pressed=i; SetCapture(hwnd); InvalidateRect(hwnd,nullptr,FALSE); break;
                }
            }
            return 0;
        }
        case WM_LBUTTONUP:{
            ReleaseCapture(); const int id=g_pressed.load(); g_pressed=-1;
            InvalidateRect(hwnd,nullptr,FALSE);
            if(id>=0) {
                const int y=DpiUnscale(GET_Y_LPARAM(lp));
                RECT rc{}; GetClientRect(hwnd,&rc);
                const int top=GetCharmTop(id,DpiUnscale(rc.bottom));
                if(y>=top&&y<top+kGridCell) ActivateCharm(id);
            }
            return 0;
        }
        case WM_RBUTTONUP:{
            if(g_showQuickMenu||g_registryQuickMenu.load()) {
                POINT point{}; GetCursorPos(&point);
                if(g_monitor) PlaceWindows(point);
                ShowQuickMetroMenu(point);
            }
            return 0; }
        case WM_KEYDOWN:if(wp==VK_ESCAPE){HideCharms();return 0;}return 0;
        case WM_TOUCH: {
            const UINT count = LOWORD(wp);
            try {
                std::vector<TOUCHINPUT> inputs(count);
                if (GetTouchInputInfo((HTOUCHINPUT)lp, count, inputs.data(), sizeof(TOUCHINPUT))) {
                    RECT rc{}; GetClientRect(hwnd,&rc);
                    const int width=DpiUnscale(rc.right),height=DpiUnscale(rc.bottom);
                    for (const auto& touch : inputs) {
                        POINT point{TOUCH_COORD_TO_PIXEL(touch.x),TOUCH_COORD_TO_PIXEL(touch.y)};
                        ScreenToClient(hwnd,&point);
                        point.x=DpiUnscale(point.x); point.y=DpiUnscale(point.y);
                        if(point.x<0 || point.x>=width) continue;
                        int id=-1;
                        for(int i=0;i<GetCharmSlotCount();i++) {
                            const int top=GetCharmTop(i,height);
                            const int charm=GetCharmAction(i);
                            if(charm>=0&&!g_hidden[charm]&&point.y>=top&&point.y<top+kGridCell) id=i;
                        }
                        if (touch.dwFlags & TOUCHEVENTF_DOWN) {
                            g_keyboardOpened = false; g_hover = id; g_pressed = id;
                            InvalidateRect(hwnd, nullptr, FALSE);
                        } else if (touch.dwFlags & TOUCHEVENTF_UP) {
                            const int pressed = g_pressed.exchange(-1);
                            g_hover = id; InvalidateRect(hwnd, nullptr, FALSE);
                            if (id >= 0 && pressed == id) ActivateCharm(id);
                        } else if (touch.dwFlags & TOUCHEVENTF_MOVE) {
                            g_hover = id; InvalidateRect(hwnd, nullptr, FALSE);
                        }
                    }
                }
            } catch (...) {
                Wh_Log(L"could not allocate touch input buffer");
            }
            CloseTouchInputHandle((HTOUCHINPUT)lp);
            return 0;
        }
        case WM_CLOSE:HideCharms(true);return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

LRESULT CALLBACK RailProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    try {
        return RailProcImpl(hwnd,msg,wp,lp);
    } catch (...) {
        Wh_Log(L"C++ exception in rail window procedure");
        if (msg == WM_TOUCH) CloseTouchInputHandle((HTOUCHINPUT)lp);
        return DefWindowProcW(hwnd,msg,wp,lp);
    }
}

LRESULT CALLBACK ClockProcImpl(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    if(msg==0x02E0) { HandleDpiChanged(hwnd,wp); return 0; } // WM_DPICHANGED.
    if (msg == WM_TIMER && wp == TIMER_CLOCK_STATUS) {
        if (IsWindowVisible(hwnd)) UpdateStatus();
        return 0;
    }
    if(msg==WM_ERASEBKGND)return 1;
    if(msg==WM_PAINT)return PaintClockWindow(hwnd);
    if(msg==WM_LBUTTONUP) {
        ShellExecuteW(nullptr,L"open",L"ms-settings:dateandtime",nullptr,nullptr,SW_SHOWNORMAL);
        return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

LRESULT CALLBACK ClockProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    try {
        return ClockProcImpl(hwnd,msg,wp,lp);
    } catch (...) {
        Wh_Log(L"C++ exception in clock window procedure");
        return DefWindowProcW(hwnd,msg,wp,lp);
    }
}

LRESULT CALLBACK MenuProcImpl(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    if(msg==0x02E0) { HandleDpiChanged(hwnd,wp); return 0; } // WM_DPICHANGED.
    switch(msg) {
        case WM_ERASEBKGND:return 1;
        case WM_PAINT:return PaintMenuWindow(hwnd);
        case WM_MOUSEACTIVATE:return MA_ACTIVATE;
        case WM_GETDLGCODE:return DLGC_WANTARROWS|DLGC_WANTCHARS|DLGC_WANTALLKEYS;
        case WM_MOUSEMOVE: {
            const int x=DpiUnscale(GET_X_LPARAM(lp));
            const int y=DpiUnscale(GET_Y_LPARAM(lp));
            MetroMenuSetHover(hwnd,MetroMenuHitTest(hwnd,x,y));
            TRACKMOUSEEVENT track{sizeof(track),TME_LEAVE,hwnd,0};
            TrackMouseEvent(&track);
            return 0; }
        case WM_MOUSELEAVE:
            MetroMenuSetHover(hwnd,-1);
            return 0;
        case WM_LBUTTONUP: {
            const int x=DpiUnscale(GET_X_LPARAM(lp));
            const int y=DpiUnscale(GET_Y_LPARAM(lp));
            const int index=MetroMenuHitTest(hwnd,x,y);
            if(MetroMenuItemSelectable(index))
                FinishMetroMenu(hwnd,g_metroMenuItems[(size_t)index].command,false,false);
            return 0; }
        case WM_RBUTTONDOWN:
            FinishMetroMenu(hwnd,0,false,true);
            return 0;
        case WM_KEYDOWN:
            switch(wp) {
                case VK_ESCAPE: case VK_APPS:
                    FinishMetroMenu(hwnd,0,false,true); break;
                case VK_DOWN:MetroMenuSetHover(hwnd,MetroMenuStep(g_metroMenuHover,+1));break;
                case VK_UP:MetroMenuSetHover(hwnd,MetroMenuStep(g_metroMenuHover,-1));break;
                case VK_HOME:MetroMenuSetHover(hwnd,MetroMenuStep(-1,+1));break;
                case VK_END:MetroMenuSetHover(hwnd,MetroMenuStep(-1,-1));break;
                case VK_RETURN: case VK_SPACE:
                    if(MetroMenuItemSelectable(g_metroMenuHover))
                        FinishMetroMenu(hwnd,g_metroMenuItems[(size_t)g_metroMenuHover].command,false,false);
                    break;
            }
            return 0;
        case WM_SYSKEYDOWN:
            FinishMetroMenu(hwnd,0,false,true);
            return 0;
        case WM_ACTIVATE:
            if(LOWORD(wp)==WA_INACTIVE && g_metroMenuActive) {
                const HWND target=(HWND)lp;
                const bool owner=g_metroMenuPurpose==METRO_MENU_POWER?
                    target==g_pane:target==g_rail;
                FinishMetroMenu(hwnd,0,!owner,owner && g_metroMenuPurpose==METRO_MENU_POWER);
            }
            return 0;
        case WM_KILLFOCUS:
            if(g_metroMenuActive) {
                const HWND target=(HWND)wp;
                const bool owner=g_metroMenuPurpose==METRO_MENU_POWER?
                    target==g_pane:target==g_rail;
                if(target!=hwnd)
                    FinishMetroMenu(hwnd,0,!owner,owner && g_metroMenuPurpose==METRO_MENU_POWER);
            }
            return 0;
        case WM_CLOSE: {
            POINT point{}; GetCursorPos(&point);
            const bool owner=g_metroMenuPurpose==METRO_MENU_POWER?
                PointInsideWindow(g_pane,point):PointInsideWindow(g_rail,point);
            FinishMetroMenu(hwnd,0,!owner,owner && g_metroMenuPurpose==METRO_MENU_POWER);
            return 0; }
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

LRESULT CALLBACK MenuProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    try {
        return MenuProcImpl(hwnd,msg,wp,lp);
    } catch (...) {
        Wh_Log(L"C++ exception in Metro context-menu window procedure");
        HideMetroContextMenu(false);
        return DefWindowProcW(hwnd,msg,wp,lp);
    }
}

void MoveKeyboardSelection(int direction) {
    const int slotCount=GetCharmSlotCount();
    int current=g_keyboardSelection.load();
    if(current<0 || current>=slotCount) current=0;
    for(int step=1;step<=slotCount;step++) {
        const int candidate=(current+direction*step+slotCount*2)%slotCount;
        const int charm=GetCharmAction(candidate);
        if(charm>=0&&!g_hidden[charm]) { g_keyboardSelection=candidate; InvalidateRect(g_rail,nullptr,FALSE); return; }
    }
}

LRESULT CALLBACK KeyboardProcImpl(int code, WPARAM wp, LPARAM lp) {
    if (code != HC_ACTION || !lp)
        return CallNextHookEx(g_keyboardHook, code, wp, lp);
    const KBDLLHOOKSTRUCT* key = (const KBDLLHOOKSTRUCT*)lp;
    if (key->flags & LLKHF_INJECTED)
        return CallNextHookEx(g_keyboardHook, code, wp, lp);

    const bool down = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
    const bool up = wp == WM_KEYUP || wp == WM_SYSKEYUP;
    // Track whether we consumed a Win+C chord so the matching C-up is also swallowed.
    // The Win key itself always passes through untouched, so Win+drag, GetAsyncKeyState
    // queries and elevated windows all keep working.
    static bool charmChordConsumed = false;

    // When Win+C activation is off or the Charms Bar is disabled, do not touch the Win key
    // at all.  This prevents the previous hold-and-replay logic from breaking Win shortcuts,
    // Start taps and Win+mouse combinations when the mod is not using Win+C.
    const bool winCActive = g_useWinC.load() && g_charmsEnabled.load();

    // Win+C detection: swallow only the C key when Win is physically held. A masking key
    // (0xE8, unassigned) is injected down+up together before the C-down so the Win release
    // does not open Start. Using 0xE8 instead of VK_F23 avoids conflict with Copilot, which
    // sends Win+Shift+F23 on some keyboards.
    if (down && key->vkCode == 'C' && winCActive) {
        if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) || (GetAsyncKeyState(VK_CONTROL) & 0x8000) || (GetAsyncKeyState(VK_MENU) & 0x8000)) {
            return CallNextHookEx(g_keyboardHook, code, wp, lp);
        }
        if ((GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000)) {
            if (!charmChordConsumed) {
                charmChordConsumed = true;
                // Mask the Win release so it does not open Start after we swallow C.
                INPUT mask[2]{};
                mask[0].type = mask[1].type = INPUT_KEYBOARD;
                mask[0].ki.wVk = mask[1].ki.wVk = 0xE8;  // unassigned: keeps Win-up from opening Start
                mask[1].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(2, mask, sizeof(INPUT));
                const bool paneOpen=g_paneVisible.load() || (g_pane && IsWindowVisible(g_pane));
                PostThreadMessageW(g_threadId, WM_PORT_OPEN, paneOpen?2:1, 0);
            }
            return 1;
        }
    }
    if (up && key->vkCode == 'C' && charmChordConsumed) {
        charmChordConsumed = false;
        return 1;
    }
    // No Win-up handling: let it through naturally.

    // Visible-panel navigation is posted to the UI thread: never run actions such as ShellExecute or WinRT
    // Share synchronously inside a low-level hook.
    if (g_visible.load() && down) {
        if (key->vkCode == VK_ESCAPE) {
            PostThreadMessageW(g_threadId, WM_PORT_CLOSE, 0, 0);
            return 1;
        }
        if (key->vkCode == VK_LEFT || key->vkCode == VK_UP) {
            PostThreadMessageW(g_threadId, WM_PORT_NAVIGATE, (WPARAM)-1, 0);
            return 1;
        }
        if (key->vkCode == VK_RIGHT || key->vkCode == VK_DOWN) {
            PostThreadMessageW(g_threadId, WM_PORT_NAVIGATE, 1, 0);
            return 1;
        }
        if (key->vkCode == VK_RETURN || key->vkCode == VK_SPACE) {
            PostThreadMessageW(g_threadId, WM_PORT_ACTIVATE, 0, 0);
            return 1;
        }
    }

    return CallNextHookEx(g_keyboardHook, code, wp, lp);
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wp, LPARAM lp) {
    try {
        return KeyboardProcImpl(code,wp,lp);
    } catch (...) {
        Wh_Log(L"C++ exception in low-level keyboard callback");
        return CallNextHookEx(g_keyboardHook,code,wp,lp);
    }
}

LRESULT CALLBACK MouseProcImpl(int code,WPARAM wp,LPARAM lp) {
    if(code==HC_ACTION && lp) {
        const MSLLHOOKSTRUCT* m=(const MSLLHOOKSTRUCT*)lp;
        POINT p=m->pt;
        if(wp==WM_MOUSEMOVE) {
            HMONITOR mon=MonitorFromPoint(p,MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi{sizeof(mi)};
            if(mon && GetMonitorInfoW(mon,&mi)) {
                if(g_edgeActivation.load() && g_charmsEnabled.load()) {
                    const bool atRight=p.x>=mi.rcMonitor.right-DpiScale(2);
                    const bool atTopEdge=p.y<=mi.rcMonitor.top+DpiScale(2);
                    const bool inTopRightZone=p.y<=mi.rcMonitor.top+DpiScale(40);
                    const bool atBottom=p.y>=mi.rcMonitor.bottom-DpiScale(2);
                    const bool cornerOnly=g_topRightCornerOnly.load();
                    const bool validActivationZone=!cornerOnly||inTopRightZone;
                    const bool blockedCorner=((cornerOnly?inTopRightZone:atTopEdge) && g_disableTRCorner.load()) ||
                                             (atBottom && g_disableBRCorner.load());
                    const bool noHint=g_disableCharmsHint.load();
                    const ULONGLONG now=GetTickCount64();
                    HWND foreground=GetForegroundWindow();
                    const int delay=IsImmersiveWindow(foreground) ?
                        g_immersiveEdgeDelay.load() : g_desktopEdgeDelay.load();
                    if(atRight && validActivationZone && !blockedCorner) {
                        if(!g_edgeLatched.exchange(true)) g_edgeSince=now;
                        if(!noHint && now-g_edgeSince.load()>=(ULONGLONG)delay && !g_visible.load() && !g_edgePosted.exchange(true))
                            PostThreadMessageW(g_threadId,WM_PORT_OPEN,0,MAKELPARAM((SHORT)p.x,(SHORT)p.y));
                    } else if(g_edgeLatched.exchange(false)) {
                        // With DisableCharmsHint, require an inward swipe from the selected edge zone.
                        const int inward=mi.rcMonitor.right-p.x;
                        if(validActivationZone && !blockedCorner && inward>=0 && inward<=DpiScale(48) &&
                           now-g_edgeSince.load()>=(ULONGLONG)delay && !g_visible.load() && !g_edgePosted.exchange(true))
                            PostThreadMessageW(g_threadId,WM_PORT_OPEN,0,MAKELPARAM((SHORT)p.x,(SHORT)p.y));
                    }
                    if((!atRight && mi.rcMonitor.right-p.x>DpiScale(48)) || blockedCorner || !validActivationZone)
                        g_edgePosted=false;
                } else { g_edgeLatched=false; g_edgePosted=false; }

                bool menuEnabled=g_showQuickMenu.load() || g_registryQuickMenu.load();
                bool atBottomLeft=p.x>=mi.rcMonitor.left && p.x<mi.rcMonitor.left+DpiScale(136) &&
                                  p.y>=mi.rcMonitor.bottom-DpiScale(2);
                // On the Start screen the corner belongs to the Start tip; the menu stays
                // reachable there through the right button (Win+X), see the pointerDown block below.
                const bool onStartScreen=CompanionStartScreenVisibleFast();
                if(menuEnabled && atBottomLeft && !onStartScreen) {
                    if(!g_menuEdgeLatched.exchange(true)) PostThreadMessageW(g_threadId,WM_PORT_MENU,0,0);
                } else if(p.y<mi.rcMonitor.bottom-DpiScale(24) || p.x>=mi.rcMonitor.left+DpiScale(150)) {
                    g_menuEdgeLatched=false;
                }

                // Windows 8.1 Start tip. Armed only while the Windows 8 Start Screen is the surface on
                // screen: over the desktop the same gesture would be meaningless. The tip stays up while
                // the pointer is on the strip, anywhere in the corner square, or on the square itself (with
                // a little padding), and it survives a short exit so that travelling from the edge to the
                // square - which means moving away from the edge - does not make it vanish on the way.
                // Nothing below costs anything on a machine without the companion mod: the monitor lookup
                // and the hit test only run once the Start screen is known to be on screen.
                bool keepTip=false;
                if(onStartScreen) {
                    const bool onStartMonitor=
                        CompanionStartScreenMonitor()==MonitorFromPoint(p,MONITOR_DEFAULTTONEAREST);
                    keepTip=onStartMonitor && (InStartTipZone(mi.rcMonitor,p) ||
                        (g_startTipShown.load() && StartTipHitTest(p)));
                }
                if(keepTip && !StartTipSuppressed()) {
                    g_startTipLeaveSince=0;
                    if(!g_startTipArmed.load()) {
                        g_startTipArmed=true;
                        PostThreadMessageW(g_threadId,WM_PORT_STARTTIPARM,0,0);
                    }
                } else if(g_startTipArmed) {
                    const ULONGLONG now=GetTickCount64();
                    if(!g_startTipLeaveSince) {
                        g_startTipLeaveSince=now;
                        // The pointer can stop dead the moment it leaves, and then this hook is never
                        // called again; the UI thread runs the grace down on a timer instead. The
                        // message is posted even before the square appears, because a dwell that is
                        // still pending has to be called off too.
                        PostThreadMessageW(g_threadId,WM_PORT_STARTTIPLEAVE,0,0);
                    }
                    if(now-g_startTipLeaveSince>=kStartTipLeaveGraceMs) {
                        g_startTipArmed=false;
                        g_startTipLeaveSince=0;
                        PostThreadMessageW(g_threadId,WM_PORT_STARTTIPHIDE,0,0);
                    }
                }
            }
        }
        const bool pointerDown=wp==WM_LBUTTONDOWN || wp==WM_RBUTTONDOWN ||
                               wp==WM_MBUTTONDOWN || wp==WM_XBUTTONDOWN;
        if(pointerDown) {
            RECT rect{};
            const bool inRail=g_rail && IsWindowVisible(g_rail) &&
                              GetWindowRect(g_rail,&rect) && PtInRect(&rect,p);
            const bool inClock=g_clock && IsWindowVisible(g_clock) &&
                               GetWindowRect(g_clock,&rect) && PtInRect(&rect,p);
            const bool inMenu=g_menu && IsWindowVisible(g_menu) &&
                              GetWindowRect(g_menu,&rect) && PtInRect(&rect,p);

            // Right button in the bottom-left corner of the Start screen is the Win+X menu, exactly as on
            // the desktop; the hover latch above is suspended there so the Start tip can own that corner.
            if(wp==WM_RBUTTONDOWN && CompanionStartScreenVisibleFast()) {
                HMONITOR mon=MonitorFromPoint(p,MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi{sizeof(mi)};
                if(mon && GetMonitorInfoW(mon,&mi) &&
                   p.x>=mi.rcMonitor.left && p.x<mi.rcMonitor.left+DpiScale(136) &&
                   p.y>=mi.rcMonitor.bottom-DpiScale(2)) {
                    HideStartTip();
                    PostThreadMessageW(g_threadId,WM_PORT_MENU,0,0);
                }
            }
            // Any click somewhere else dismisses the Start tip: it is a transient hint, not a fixture.
            // Clicks on the square itself reach its own window and are handled there.
            if(g_startTipShown.load() && !StartTipHitTest(p))
                PostThreadMessageW(g_threadId,WM_PORT_STARTTIPHIDE,0,0);

            // Treat every mod-owned surface as inside so clicking it cannot dismiss the rail underneath.
            if(g_menu && IsWindowVisible(g_menu) && !inMenu)
                PostMessageW(g_menu,WM_CLOSE,0,0);
            if(g_visible.load() && g_closeOnClickOutside.load() &&
               !inRail && !inClock && !inMenu) {
                // Post to the UI queue rather than relying on activation or a WM_CLOSE delivery to a possibly
                // non-activating overlay HWND.
                PostThreadMessageW(g_threadId,WM_PORT_CLOSE,0,0);
            }
        }

    }
    return CallNextHookEx(g_mouseHook,code,wp,lp);
}

LRESULT CALLBACK MouseProc(int code,WPARAM wp,LPARAM lp) {
    try {
        return MouseProcImpl(code,wp,lp);
    } catch (...) {
        Wh_Log(L"C++ exception in low-level mouse callback");
        return CallNextHookEx(g_mouseHook,code,wp,lp);
    }
}

void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                  LONG idObject, LONG, DWORD, DWORD eventTime) {
    // Track the native search flyout even while the rail is closed: it takes the foreground when it opens
    // and gives it back when it is dismissed, which is the most portable open/closed signal we have. The
    // 1 s grace keeps the optimistic state set by NoteSearchChordSent() from being cleared by foreground
    // bounces while the flyout is still coming up.
    if(event==EVENT_SYSTEM_FOREGROUND && idObject==OBJID_WINDOW && hwnd) {
        // One-process cache: switching between windows of the same app is the common case and would
        // otherwise re-open the process handle on every activation.
        static DWORD lastPid=0;
        static bool lastWasSearch=false;
        DWORD foregroundPid=0;
        GetWindowThreadProcessId(hwnd,&foregroundPid);
        bool isSearch=false;
        if(foregroundPid && foregroundPid==lastPid) isSearch=lastWasSearch;
        else {
            isSearch=IsSearchHostProcess(foregroundPid);
            lastPid=foregroundPid;
            lastWasSearch=isSearch;
        }
        if(isSearch) g_searchKnownOpen.store(true);
        else if((DWORD)(GetTickCount()-g_searchVerifyTick.load())>1000) g_searchKnownOpen.store(false);
    }
    if(event==EVENT_SYSTEM_FOREGROUND && idObject==OBJID_WINDOW && hwnd &&
       g_paneVisible.load() && hwnd!=g_pane && GetAncestor(hwnd,GA_ROOTOWNER)!=g_pane) {
        PostThreadMessageW(g_threadId,WM_PORT_PANECLOSE,0,0);
        return;
    }
    if(event!=EVENT_SYSTEM_FOREGROUND || idObject!=OBJID_WINDOW || !hwnd ||
       !g_visible.load() || !g_closeOnClickOutside.load()) return;
    if(eventTime && (LONG)(eventTime-g_openTick.load())<0) return;
    const DWORD suppressUntil=g_noFocusDismissUntil.load();
    if(suppressUntil && (LONG)(suppressUntil-GetTickCount())>0) return;
    if(hwnd==g_rail || hwnd==g_clock || hwnd==g_menu) return;
    HWND root=GetAncestor(hwnd,GA_ROOTOWNER);
    if(root==g_rail || root==g_clock || root==g_menu) return;
    PostThreadMessageW(g_threadId,WM_PORT_CLOSE,0,0);
}

DWORD WINAPI InputHookThreadProc(LPVOID) {
    HANDLE previousDpiContext=nullptr;
    try {
        MSG queued{};
        PeekMessageW(&queued, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
        previousDpiContext=EnterBestEffortPerMonitorDpiContext();
        g_hookThreadId = GetCurrentThreadId();

        g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, (HINSTANCE)&__ImageBase, 0);
        if (!g_keyboardHook)
            Wh_Log(L"WH_KEYBOARD_LL install failed (%lu)", GetLastError());
        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, (HINSTANCE)&__ImageBase, 0);
        if (!g_mouseHook)
            Wh_Log(L"WH_MOUSE_LL install failed (%lu)", GetLastError());

        const bool installed = g_keyboardHook && g_mouseHook;
        g_hooksSucceeded.store(installed);
        if (g_hookReady) SetEvent(g_hookReady);

        MSG message{};
        while (!g_stop.load()) {
            const BOOL result = GetMessageW(&message, nullptr, 0, 0);
            if (result <= 0) break;
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    } catch (...) {
        Wh_Log(L"C++ exception in input-hook thread");
        g_hooksSucceeded.store(false);
        g_stop.store(true);
        if (g_hookReady) SetEvent(g_hookReady);
        if (g_threadId) PostThreadMessageW(g_threadId,WM_QUIT,0,0);
    }

    if (g_mouseHook) {
        if (!UnhookWindowsHookEx(g_mouseHook))
            Wh_Log(L"mouse hook removal failed (%lu)",GetLastError());
        g_mouseHook = nullptr;
    }
    if (g_keyboardHook) {
        if (!UnhookWindowsHookEx(g_keyboardHook))
            Wh_Log(L"keyboard hook removal failed (%lu)",GetLastError());
        g_keyboardHook = nullptr;
    }
    RestoreThreadDpiContext(previousDpiContext);
    return 0;
}

DWORD WINAPI UiThreadProc(LPVOID) {
    HRESULT roInit=E_FAIL;
    HANDLE previousDpiContext=nullptr;
    try {
        MSG queued{};
        PeekMessageW(&queued,nullptr,WM_USER,WM_USER,PM_NOREMOVE);
        previousDpiContext=EnterBestEffortPerMonitorDpiContext();
        g_threadId=GetCurrentThreadId();

        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        if(Gdiplus::GdiplusStartup(&g_gdiplusToken,&gdiplusStartupInput,nullptr)!=Gdiplus::Ok) {
            g_gdiplusToken=0;
            Wh_Log(L"GDI+ startup failed; using vector GDI fallback");
        }

        roInit=RoInitialize(RO_INIT_SINGLETHREADED);
        if(FAILED(roInit))
            Wh_Log(L"RoInitialize failed (0x%08X)",(unsigned)roInit);

        RefreshSystemAccent();

        WNDCLASSEXW wc{sizeof(wc)};
        // Resolve the undocumented band creator once, before any surface exists. The Charms Bar has to draw
        // over the Start screen, and a plain WS_EX_TOPMOST window cannot do that once another window lives in a
        // higher z-band - which is what the Windows 8 (8045) Start Screen mod and this mod's own Project panel do.
        {
            HMODULE user32=GetModuleHandleW(L"user32.dll");
            if(user32) g_charmsCreateInBand=reinterpret_cast<CreateWindowInBandCharms_t>(
                reinterpret_cast<void*>(GetProcAddress(user32,"CreateWindowInBand")));
            if(!g_charmsCreateInBand)
                Wh_Log(L"CreateWindowInBand not available; using ordinary topmost windows");
        }
        wc.hInstance=(HINSTANCE)&__ImageBase;
        wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
        wc.lpfnWndProc=RailProc;
        wc.lpszClassName=kRailClass;
        const bool railClassRegistered = RegisterClassExW(&wc) != 0;
        if(!railClassRegistered)
            Wh_Log(L"critical: rail window class registration failed (%lu)",GetLastError());

        WNDCLASSEXW cc{sizeof(cc)};
        cc.hInstance=wc.hInstance; cc.hCursor=wc.hCursor;
        cc.lpfnWndProc=ClockProc; cc.lpszClassName=kClockClass;
        const bool clockClassRegistered = RegisterClassExW(&cc) != 0;
        if(!clockClassRegistered)
            Wh_Log(L"critical: clock window class registration failed (%lu)",GetLastError());

        WNDCLASSEXW mc{sizeof(mc)};
        mc.style=CS_DROPSHADOW;
        mc.hInstance=wc.hInstance; mc.hCursor=wc.hCursor;
        mc.lpfnWndProc=MenuProc; mc.lpszClassName=kMenuClass;
        const bool menuClassRegistered = RegisterClassExW(&mc) != 0;
        if(!menuClassRegistered)
            Wh_Log(L"optional: menu window class registration failed (%lu)",GetLastError());

        WNDCLASSEXW pc{sizeof(pc)};
        pc.hInstance=wc.hInstance; pc.hCursor=wc.hCursor;
        pc.lpfnWndProc=PaneProc; pc.lpszClassName=kPaneClass;
        const bool paneClassRegistered = RegisterClassExW(&pc) != 0;
        if(!paneClassRegistered)
            Wh_Log(L"critical: settings pane class registration failed (%lu)",GetLastError());
        if(paneClassRegistered) {
            g_pane=CreateCharmsSurface(WS_EX_TOPMOST|WS_EX_TOOLWINDOW,kPaneClass,L"Settings",WS_POPUP,
                0,0,DpiScale(kPaneWidth),DpiScale(600),nullptr,wc.hInstance);
        }
        WNDCLASSEXW vc{sizeof(vc)};
        vc.hInstance=wc.hInstance; vc.hCursor=wc.hCursor;
        vc.style=CS_DROPSHADOW;
        vc.lpfnWndProc=VolumeFlyoutProc; vc.lpszClassName=kVolumeFlyoutClass;
        const bool volumeClassRegistered = RegisterClassExW(&vc) != 0;
        if(!volumeClassRegistered)
            Wh_Log(L"optional: settings flyout class registration failed (%lu)",GetLastError());
        // Owned by the pane, so it has to share the pane's band or it would be drawn behind its own owner.
        if(volumeClassRegistered && g_pane) {
            g_volumeFlyout=CreateCharmsSurface(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,
                kVolumeFlyoutClass,L"Volume",WS_POPUP,0,0,DpiScale(kVolumeFlyoutWidth),
                DpiScale(kVolumeFlyoutHeight),g_pane,wc.hInstance);
        }
        WNDCLASSEXW tc{sizeof(tc)};
        tc.hInstance=wc.hInstance; tc.hCursor=wc.hCursor;
        tc.lpfnWndProc=VolumeTipProc; tc.lpszClassName=kVolumeTipClass;
        const bool tipClassRegistered = RegisterClassExW(&tc) != 0;
        if(!tipClassRegistered)
            Wh_Log(L"optional: value tip class registration failed (%lu)",GetLastError());
        if(tipClassRegistered && g_pane) {
            g_volumeTip=CreateCharmsSurface(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,
                kVolumeTipClass,L"Value",WS_POPUP,0,0,DpiScale(kVolTipWidth),
                DpiScale(kVolTipHeight),g_pane,wc.hInstance);
        }
        if(!g_volumeTip)
            Wh_Log(L"optional value tip window not created (%lu)",GetLastError());
        if(!g_volumeFlyout) {
            Wh_Log(L"optional settings flyout window not created (%lu)",GetLastError());
        } else {
            g_brightnessNotifyCurrent=RegisterPowerSettingNotification(
                g_volumeFlyout,&kGuidCurrentMonitorBrightness,DEVICE_NOTIFY_WINDOW_HANDLE);
            g_brightnessNotifyPolicy=RegisterPowerSettingNotification(
                g_volumeFlyout,&kGuidVideoPolicyBrightness,DEVICE_NOTIFY_WINDOW_HANDLE);
        }
        if(railClassRegistered) {
            g_rail=CreateCharmsSurface(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_LAYERED|WS_EX_NOACTIVATE,
                kRailClass,L"Charms",WS_POPUP,0,0,DpiScale(kRailWidth),DpiScale(600),
                nullptr,wc.hInstance);
        }
        if(clockClassRegistered) {
            g_clock=CreateCharmsSurface(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_LAYERED|WS_EX_NOACTIVATE,
                kClockClass,L"Charms Clock",WS_POPUP,0,0,DpiScale(kClockWidth),DpiScale(kClockHeight),
                nullptr,wc.hInstance);
        }
        if(menuClassRegistered) {
            g_menu=CreateCharmsSurface(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_LAYERED,
                kMenuClass,L"Charms Menu",WS_POPUP,0,0,DpiScale(300),DpiScale(220),
                nullptr,wc.hInstance);
        }
        WNDCLASSEXW tipClass{sizeof(tipClass)};
        tipClass.hInstance=wc.hInstance; tipClass.hCursor=wc.hCursor;
        tipClass.lpfnWndProc=StartTipProc; tipClass.lpszClassName=kStartTipClass;
        const bool startTipClassRegistered = RegisterClassExW(&tipClass) != 0;
        if(!startTipClassRegistered)
            Wh_Log(L"optional: Start tip class registration failed (%lu)",GetLastError());
        if(startTipClassRegistered) {
            g_startTip=CreateCharmsSurface(
                WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_LAYERED,
                kStartTipClass,L"Start",WS_POPUP,0,0,DpiScale(kStartTipSize),DpiScale(kStartTipSize),
                nullptr,wc.hInstance);
        }
        if(!g_startTip)
            Wh_Log(L"optional Start tip window not created (%lu)",GetLastError());

        if(g_rail) {
            SetLayeredWindowAttributes(g_rail,0,0,LWA_ALPHA);
            RegisterTouchWindow(g_rail,TWF_WANTPALM);
            // TIMER_STATUS is started lazily in ShowCharms and stopped in HideCharms
        }
        if(g_clock) SetLayeredWindowAttributes(g_clock,0,255,LWA_ALPHA);
        if(g_menu) SetLayeredWindowAttributes(g_menu,0,255,LWA_ALPHA);
        g_clockStatusTimerActive = g_clock && SetTimer(g_clock,TIMER_CLOCK_STATUS,1000,nullptr) != 0;
        if(g_clock && !g_clockStatusTimerActive)
            Wh_Log(L"clock status timer failed (%lu); using rail timer fallback",GetLastError());
        if(!g_menu)
            Wh_Log(L"optional context-menu window creation failed (%lu)",GetLastError());
        if(!g_rail || !g_clock)
            Wh_Log(L"required rail/clock window creation failed (%lu)",GetLastError());

        RefreshEdgeUiRegistry();
        g_foregroundHook=SetWinEventHook(EVENT_SYSTEM_FOREGROUND,EVENT_SYSTEM_FOREGROUND,
            nullptr,ForegroundEventProc,0,0,WINEVENT_OUTOFCONTEXT);
        if(!g_foregroundHook)
            Wh_Log(L"foreground light-dismiss hook failed (%lu)",GetLastError());
        g_startSucceeded.store(g_rail!=nullptr && g_clock!=nullptr);
        if(g_ready) SetEvent(g_ready);

        MSG message{};
        while(!g_stop.load() && GetMessageW(&message,nullptr,0,0)>0) {
            if(message.hwnd==nullptr && message.message==WM_PORT_OPEN) {
                const bool keyboard=message.wParam==1 || message.wParam==2;
                const bool paneWasOpen=message.wParam==2;
                POINT point{(LONG)(SHORT)LOWORD(message.lParam), (LONG)(SHORT)HIWORD(message.lParam)};
                if(keyboard) GetCursorPos(&point);
                if(paneWasOpen) {
                    ClosePaneWindow(true);
                    HideCharms(true);
                    if(IsWindows8OrLater() && !IsWindows10OrLater() && SendWinChord('C'))
                        continue; // let Windows 8/8.1 open its native Charms surface
                }
                ShowCharms(point,keyboard);
                continue;
            }
            if(message.hwnd==nullptr && message.message==WM_PORT_REFRESH) {
                POINT point{};
                if(g_monitorRect.right>g_monitorRect.left && g_monitorRect.bottom>g_monitorRect.top) {
                    point.x=(g_monitorRect.left+g_monitorRect.right)/2;
                    point.y=(g_monitorRect.top+g_monitorRect.bottom)/2;
                } else GetCursorPos(&point);
                PlaceWindows(point);
                if(g_rail) InvalidateRect(g_rail,nullptr,TRUE);
                if(g_clock) InvalidateRect(g_clock,nullptr,TRUE);
                if(g_menu) InvalidateRect(g_menu,nullptr,TRUE);
                continue;
            }
            if(message.hwnd==nullptr && message.message==WM_PORT_CLOSE) {
                HideCharms();
                continue;
            }
            if(message.hwnd==nullptr && message.message==WM_PORT_PANECLOSE) {
                ClosePaneWindow(false);
                continue;
            }
            if(message.hwnd==nullptr && message.message==WM_PORT_MENU) {
                HideStartTip();
                POINT point{};
                GetCursorPos(&point);
                PlaceWindows(point);
                ShowQuickMetroMenu(point);
                continue;
            }
            if(message.hwnd==nullptr && message.message==WM_PORT_STARTTIPARM) {
                if(g_startTip && CompanionStartScreenVisible()) {
                    ULONGLONG dwell=kStartTipDwellMs;
                    POINT cursor{};
                    HMONITOR monitor=CompanionStartScreenMonitor();
                    MONITORINFO mi{sizeof(mi)};
                    if(GetCursorPos(&cursor) && monitor==MonitorFromPoint(cursor,MONITOR_DEFAULTTONEAREST) &&
                       GetMonitorInfoW(monitor,&mi) &&
                       InStartTipCornerBox(mi.rcMonitor,cursor)) {
                        dwell=kStartTipCornerDwellMs;
                    }
                    if(!SetTimer(g_startTip,TIMER_STARTTIP_DWELL,(UINT)dwell,nullptr))
                        Wh_Log(L"Start tip dwell timer failed (%lu)",GetLastError());
                }
                continue;
            }
            if(message.hwnd==nullptr && message.message==WM_PORT_STARTTIPHIDE) {
                HideStartTip();
                continue;
            }
            if(message.hwnd==nullptr && message.message==WM_PORT_STARTTIPLEAVE) {
                // Grace before the square goes, whether or not the pointer ever moves again. One
                // decision point on the timer also covers a dwell that is still waiting: if the
                // pointer does not come back, that pending dwell must not raise the square later.
                if(g_startTip && !SetTimer(g_startTip,TIMER_STARTTIP_EXIT,(UINT)kStartTipLeaveGraceMs,nullptr))
                    Wh_Log(L"Start tip exit timer failed (%lu)",GetLastError());
                continue;
            }
            if(message.hwnd==nullptr && message.message==WM_PORT_MENU_COMMAND) {
                DispatchMetroMenuCommand((int)message.wParam,(int)message.lParam);
                continue;
            }
            if(message.hwnd==nullptr && message.message==WM_PORT_NAVIGATE) {
                MoveKeyboardSelection((int)(INT_PTR)message.wParam);
                continue;
            }
            if(message.hwnd==nullptr && message.message==WM_PORT_ACTIVATE) {
                ActivateCharm(g_keyboardSelection.load());
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    } catch (...) {
        Wh_Log(L"C++ exception in UI thread; shutting down safely");
        g_startSucceeded.store(false);
        g_stop.store(true);
        if(g_ready) SetEvent(g_ready);
        if(g_hookThreadId) PostThreadMessageW(g_hookThreadId,WM_QUIT,0,0);
    }

    if(g_foregroundHook) {
        if(!UnhookWinEvent(g_foregroundHook))
            Wh_Log(L"foreground event hook removal failed (%lu)",GetLastError());
        g_foregroundHook=nullptr;
    }

    try {
        if(g_dtm && g_dataRequested) {
            const HRESULT removeResult=g_dtm->remove_DataRequested(g_dataToken);
            if(FAILED(removeResult))
                Wh_Log(L"removing Share event handler failed (0x%08X)",
                       (unsigned)removeResult);
        }
    } catch (...) {
        Wh_Log(L"exception while removing Share event handler");
    }
    g_dataRequested.Reset();
    g_dtm.Reset();
    g_dtmInterop.Reset();
    g_shareOwner=nullptr;
    g_shareReady.store(false);

    HideMetroContextMenu(false);
    if(g_volumeTip) {
        HideVolumeTip();
        if(!DestroyWindow(g_volumeTip)) Wh_Log(L"value tip destruction failed (%lu)",GetLastError());
        g_volumeTip=nullptr;
    }
    if(g_volumeFlyout) {
        HideVolumeFlyout();
        if(g_brightnessNotifyCurrent && !UnregisterPowerSettingNotification(g_brightnessNotifyCurrent))
            Wh_Log(L"brightness notification cleanup failed (%lu)",GetLastError());
        g_brightnessNotifyCurrent=nullptr;
        if(g_brightnessNotifyPolicy && !UnregisterPowerSettingNotification(g_brightnessNotifyPolicy))
            Wh_Log(L"brightness policy notification cleanup failed (%lu)",GetLastError());
        g_brightnessNotifyPolicy=nullptr;
        if(!DestroyWindow(g_volumeFlyout)) Wh_Log(L"volume flyout destruction failed (%lu)",GetLastError());
        g_volumeFlyout=nullptr;
    }
    // The Start tip lives in the same band and on the same thread as the other surfaces, so it has to be
    // destroyed here as well: a window left behind would keep a pointer to an unloaded WndProc and would
    // also make the class registration fail on the next load.
    if(g_startTip) {
        KillTimer(g_startTip,TIMER_STARTTIP_FRAME);
        KillTimer(g_startTip,TIMER_STARTTIP_DWELL);
        KillTimer(g_startTip,TIMER_STARTTIP_POLL);
        KillTimer(g_startTip,TIMER_STARTTIP_CLOSE);
        KillTimer(g_startTip,TIMER_STARTTIP_EXIT);
        if(IsWindowVisible(g_startTip)) ShowWindow(g_startTip,SW_HIDE);
        g_startTipShown.store(false);
        if(!DestroyWindow(g_startTip)) Wh_Log(L"Start tip destruction failed (%lu)",GetLastError());
        g_startTip=nullptr;
    }
    if(g_pane) {
        g_paneVisible.store(false);
        KillTimer(g_pane,TIMER_PANE_METRICS);
        if(!DestroyWindow(g_pane)) Wh_Log(L"settings pane destruction failed (%lu)",GetLastError());
        g_pane=nullptr;
    }
    DiscardPaneSlideCache();
    if(g_menu) {
        if(!DestroyWindow(g_menu)) Wh_Log(L"menu window destruction failed (%lu)",GetLastError());
        g_menu=nullptr;
    }
    if(g_clock) {
        if(!DestroyWindow(g_clock)) Wh_Log(L"clock window destruction failed (%lu)",GetLastError());
        g_clock=nullptr;
    }
    if(g_rail) {
        if(!DestroyWindow(g_rail)) Wh_Log(L"rail window destruction failed (%lu)",GetLastError());
        g_rail=nullptr;
    }
    g_clockStatusTimerActive=false;
    if(!UnregisterClassW(kVolumeTipClass,(HINSTANCE)&__ImageBase) && GetLastError()!=ERROR_CLASS_DOES_NOT_EXIST)
        Wh_Log(L"value tip class cleanup failed (%lu)",GetLastError());
    if(!UnregisterClassW(kVolumeFlyoutClass,(HINSTANCE)&__ImageBase) && GetLastError()!=ERROR_CLASS_DOES_NOT_EXIST)
        Wh_Log(L"volume flyout class cleanup failed (%lu)",GetLastError());
    if(!UnregisterClassW(kPaneClass,(HINSTANCE)&__ImageBase) && GetLastError()!=ERROR_CLASS_DOES_NOT_EXIST)
        Wh_Log(L"settings pane class cleanup failed (%lu)",GetLastError());
    if(!UnregisterClassW(kMenuClass,(HINSTANCE)&__ImageBase) && GetLastError()!=ERROR_CLASS_DOES_NOT_EXIST)
        Wh_Log(L"menu window class cleanup failed (%lu)",GetLastError());
    if(!UnregisterClassW(kClockClass,(HINSTANCE)&__ImageBase) && GetLastError()!=ERROR_CLASS_DOES_NOT_EXIST)
        Wh_Log(L"clock window class cleanup failed (%lu)",GetLastError());
    if(!UnregisterClassW(kRailClass,(HINSTANCE)&__ImageBase) && GetLastError()!=ERROR_CLASS_DOES_NOT_EXIST)
        Wh_Log(L"rail window class cleanup failed (%lu)",GetLastError());
    if(!UnregisterClassW(kStartTipClass,(HINSTANCE)&__ImageBase) && GetLastError()!=ERROR_CLASS_DOES_NOT_EXIST)
        Wh_Log(L"Start tip class cleanup failed (%lu)",GetLastError());

    if(SUCCEEDED(roInit)) RoUninitialize();
    if(g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken=0;
    }
    RestoreThreadDpiContext(previousDpiContext);
    return 0;
}

void StopAndJoinWorkers() noexcept {
    g_stop.store(true);
    if (g_hookThreadId) PostThreadMessageW(g_hookThreadId,WM_QUIT,0,0);
    if (g_threadId) PostThreadMessageW(g_threadId,WM_QUIT,0,0);
    if (g_hookThread) {
        WaitForSingleObject(g_hookThread,INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread=nullptr;
    }
    if (g_thread) {
        WaitForSingleObject(g_thread,INFINITE);
        CloseHandle(g_thread);
        g_thread=nullptr;
    }
    if (g_hookReady) { CloseHandle(g_hookReady); g_hookReady=nullptr; }
    if (g_ready) { CloseHandle(g_ready); g_ready=nullptr; }
    g_hookThreadId=g_threadId=0;
    g_clockStatusTimerActive=false;
}
} // namespace

void ReturnToCharmsBarFromDisplaySwitch() {
    // wParam 1 = open the Charms Bar at the cursor (the same message the Charms chord posts)
    if (g_threadId) PostThreadMessageW(g_threadId, WM_PORT_OPEN, 1, 0);
}

// -----------------------------------------------------------------------------------------------------
// Startup, persistence and logging
//
// Windhawk injects a mod at process creation, and Wh_ModInit runs before the target process starts
// executing (see the Windhawk "Creating a new mod" documentation). At a cold boot or after an Explorer
// restart there is no shell window yet, and for a moment GetShellWindow() still returns the window of the
// old, exiting Explorer. Both cases are transient, so the mod no longer decides at init time that it is a
// "secondary" Explorer process and stays idle: it starts a supervisor thread instead, which waits for this
// process to own the shell window and then starts the Charms Bar. The supervisor also retries a failed
// startup instead of giving up, so the mod recovers by itself after an Explorer restart.
constexpr PCWSTR kModVersion=L"1.0.0";
// Assigned explicitly in Wh_ModInit/StartSupervisorIfNeeded, like the other atomics in this file, so builds
// whose standard library omits value-taking atomic constructors keep working.
std::atomic<bool> g_charmsServicesStarted;
std::atomic<bool> g_supervisorStop;
HANDLE g_supervisorThread=nullptr;

bool StartCharmsServices(); // defined below; the body of the former Wh_ModInit

DWORD WINAPI ShellHostSupervisorProc(LPVOID) {
    constexpr int kMaxStartAttempts=8;
    int startAttempts=0;
    unsigned readyPolls=0;
    ShellHostState lastState=ShellHostState::NotReady;
    Wh_Log(L"supervisor started for pid %lu",GetCurrentProcessId());
    for(;;) {
        if(g_supervisorStop.load()) break;
        const ShellHostState state=QueryShellHostState();
        if(state!=lastState) {
            DWORD shellPid=0;
            const HWND shell=GetShellWindow();
            if(shell) GetWindowThreadProcessId(shell,&shellPid);
            Wh_Log(L"shell host state is now %s (shell pid %lu, this pid %lu)",
                   ShellHostStateName(state),(unsigned long)shellPid,GetCurrentProcessId());
            lastState=state;
        }
        if(state==ShellHostState::OtherLive) {
            Wh_Log(L"another live Explorer process owns the shell window; "
                   L"the Charms Bar stays idle in this process (this is the normal case for extra explorer.exe instances)");
            break;
        }
        if(state==ShellHostState::Host) {
            if(g_charmsServicesStarted.load()) break;
            ++startAttempts;
            Wh_Log(L"shell window is ready, starting the Charms Bar (attempt %d of %d)",
                   startAttempts,kMaxStartAttempts);
            if(StartCharmsServices()) {
                g_charmsServicesStarted.store(true);
                Wh_Log(L"Charms Bar initialized and running in pid %lu",GetCurrentProcessId());
                break;
            }
            Wh_Log(L"startup attempt %d failed",startAttempts);
            if(startAttempts>=kMaxStartAttempts) {
                Wh_Log(L"giving up after %d startup attempts; the mod stays loaded but idle",
                       kMaxStartAttempts);
                break;
            }
            Sleep((DWORD)(std::min)(5000,500*startAttempts)); // linear backoff, 0.5 s .. 4 s
            continue;
        }
        // Shell window not available yet (cold boot, Explorer restart, or the stale window of the old
        // Explorer). Poll every 250 ms and log the wait every 30 s so an idle log still shows the mod is
        // alive and what it is waiting for.
        if(readyPolls==0 || readyPolls%120==0)
            Wh_Log(L"waiting for this process to own the shell window (poll %u)",readyPolls);
        ++readyPolls;
        Sleep(250);
    }
    Wh_Log(L"supervisor exiting for pid %lu",GetCurrentProcessId());
    return 0;
}

bool StartSupervisorIfNeeded() {
    if(g_supervisorThread) return true;
    g_supervisorStop.store(false);
    g_supervisorThread=CreateThread(nullptr,0,ShellHostSupervisorProc,nullptr,0,nullptr);
    if(!g_supervisorThread) {
        Wh_Log(L"supervisor thread creation failed (%lu)",GetLastError());
        return false;
    }
    return true;
}

void StopSupervisor() {
    g_supervisorStop.store(true);
    if(!g_supervisorThread) return;
    // The supervisor may be inside a startup attempt, which can take up to ~15 s, so wait
    // without a bounded timeout.  Returning while the thread is still alive would let
    // Windhawk unload the image under a running thread and crash Explorer.
    WaitForSingleObject(g_supervisorThread,INFINITE);
    CloseHandle(g_supervisorThread);
    g_supervisorThread=nullptr;
}

// The original Wh_ModInit body, now callable both at init time and from the supervisor retry loop.
bool StartCharmsServices() {
    try {
    g_stop.store(false);
    g_threadId = g_hookThreadId = 0;
    g_clockStatusTimerActive = false;
    g_keyboardHook = g_mouseHook = nullptr;
    g_hover.store(-1); g_pressed.store(-1); g_keyboardSelection.store(2);
    g_visible.store(false); g_keyboardOpened.store(false); g_shareReady.store(false);
    g_startSucceeded.store(false); g_hooksSucceeded.store(false);
    g_foregroundHook=nullptr;
    g_openTick.store(0); g_noFocusDismissUntil.store(0);
    g_charmSlotCount.store(kCharmCount);
    for(int i=0;i<kCharmCount;i++) {
        g_actionMap[i].store(i);
        g_hidden[i].store(false);
    }
    g_uiDpi.store(kReferenceDpi);
    g_threadPerMonitorDpi.store(false);
    g_topRightCornerOnly.store(false);
    g_enableTransparency.store(false);
    g_startLogoStyle.store(0);
    g_charmIconSet.store(0);
    g_paneKind.store(PANE_SETTINGS);
    g_paneBrightnessProbed=false;
    g_paneContentOffset=0;
    g_disableStartFade.store(false);
    g_disableTRCorner.store(false); g_disableBRCorner.store(false);
    g_disableCharmsHint.store(false); g_registryQuickMenu.store(false);
    g_desktopEdgeDelay.store(100); g_immersiveEdgeDelay.store(100);
    g_accent.store(GetSysColor(COLOR_HIGHLIGHT));
    g_networkState.store(0); g_wifiSignal.store(-1); g_batteryLevel.store(-1);
    g_batteryCharging.store(false); g_adapterDetected.store(false); g_airplane.store(false); g_wifiRadioOff.store(false);
    g_animStep.store(0); g_animTarget.store(0); g_slideOffset.store(0);
    g_animFromAlpha=g_animFromSlide=g_animToSlide=g_animDurationMs=0;
    g_animStartQpc=g_animQpcFrequency=0;
    g_edgeLatched.store(false); g_menuEdgeLatched.store(false); g_edgePosted.store(false);
    g_edgeSince.store(0);
    g_searchTarget.store(0); g_searchKnownOpen.store(false); g_searchVerifyTick.store(0);
    g_companionStartRetryAfter.store(0);

    ReadWindhawkSettings();
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hookReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_ready || !g_hookReady) {
        if (g_ready) CloseHandle(g_ready);
        if (g_hookReady) CloseHandle(g_hookReady);
        g_ready = g_hookReady = nullptr;
        Wh_Log(L"startup event creation failed (%lu)", GetLastError());
        return false;
    }

    g_thread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"UI thread creation failed (%lu)", GetLastError());
        CloseHandle(g_ready); CloseHandle(g_hookReady);
        g_ready = g_hookReady = nullptr;
        return false;
    }
    if (WaitForSingleObject(g_ready, 15000) != WAIT_OBJECT_0 || !g_startSucceeded.load()) {
        Wh_Log(L"UI startup timed out or required windows failed");
        g_stop.store(true);
        if (g_threadId) PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread); g_thread = nullptr;
        CloseHandle(g_ready); CloseHandle(g_hookReady);
        g_ready = g_hookReady = nullptr;
        return false;
    }

    g_hookThread = CreateThread(nullptr, 0, InputHookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread || WaitForSingleObject(g_hookReady, 8000) != WAIT_OBJECT_0 ||
        !g_hooksSucceeded.load() || !g_startSucceeded.load() || g_stop.load()) {
        Wh_Log(L"input hook thread failed to start");
        g_stop.store(true);
        if (g_hookThreadId) PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        if (g_threadId) PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        if (g_hookThread) { WaitForSingleObject(g_hookThread, INFINITE); CloseHandle(g_hookThread); g_hookThread=nullptr; }
        if (g_thread) { WaitForSingleObject(g_thread, INFINITE); CloseHandle(g_thread); g_thread=nullptr; }
        CloseHandle(g_ready); CloseHandle(g_hookReady);
        g_ready = g_hookReady = nullptr;
        return false;
    }
    // DisplaySwitchMod::Init() removed from eager startup – lazy init on first Devices > Project use.
    Wh_Log(L"v%s services started (rail=0x%p clock=0x%p menu=0x%p pane=0x%p volume=0x%p, "
           L"keyboard hook=%s, mouse hook=%s, foreground hook=%s)",
           kModVersion,(void*)g_rail,(void*)g_clock,(void*)g_menu,(void*)g_pane,(void*)g_volumeFlyout,
           g_keyboardHook?L"ok":L"failed",g_mouseHook?L"ok":L"failed",g_foregroundHook?L"ok":L"failed");
    return true;
    } catch (...) {
        Wh_Log(L"C++ exception during mod initialization; cleaning up");
        StopAndJoinWorkers();
        return false;
    }
}

// Entry point used by Wh_ModInit. Starts immediately when this process already owns the shell window;
// otherwise arms the supervisor so a cold boot or an Explorer restart cannot leave the mod dormant.
BOOL InitializeCharmsMod() {
    // A fresh init after a disable/enable cycle in the same process must not inherit the previous run's state.
    g_charmsServicesStarted.store(false);
    DWORD sessionId=0;
    ProcessIdToSessionId(GetCurrentProcessId(),&sessionId);
    const ShellHostState state=QueryShellHostState();
    const HWND shell=GetShellWindow();
    DWORD shellPid=0;
    if(shell) GetWindowThreadProcessId(shell,&shellPid);
    Wh_Log(L"v%s, Wh_ModInit: pid=%lu, session=%lu, shell pid=%lu, state=%s",
           kModVersion,GetCurrentProcessId(),(unsigned long)sessionId,(unsigned long)shellPid,
           ShellHostStateName(state));

    if(state==ShellHostState::Host) {
        if(StartCharmsServices()) {
            g_charmsServicesStarted.store(true);
            Wh_Log(L"v%s initialized in pid %lu",kModVersion,GetCurrentProcessId());
            return TRUE;
        }
        Wh_Log(L"immediate start failed; handing over to the supervisor");
    } else if(state==ShellHostState::OtherLive) {
        Wh_Log(L"another live Explorer process owns the shell window; this process stays idle");
        return TRUE;
    } else {
        Wh_Log(L"shell window not ready yet (the usual state right after an Explorer restart); "
               L"startup is deferred to the supervisor");
    }
    // Returning TRUE keeps the mod loaded so the supervisor can finish the job once the shell is up. A
    // FALSE return would make Windhawk unload the mod for this Explorer process until it is restarted again.
    if(!StartSupervisorIfNeeded())
        Wh_Log(L"WARNING supervisor could not be started; the Charms Bar will not initialize in this process");
    return TRUE;
}

BOOL WhTool_ModInit() {
    try {
        // Tool host: explorer.exe -tool-mod "win8-charms-bar-recreation"
        // Runs in a dedicated explorer.exe tool process, isolated from the shell.
        // Bypass the ShellHostSupervisor (which is for injected explorer.exe) and
        // start the Charms services directly. Supervisor stays dormant (no thread).
        g_charmsServicesStarted.store(false);
        DWORD sessionId = 0;
        ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
        Wh_Log(L"v%s, WhTool_ModInit (tool host): pid=%lu, session=%lu", kModVersion, GetCurrentProcessId(), (unsigned long)sessionId);
        if (StartCharmsServices()) {
            g_charmsServicesStarted.store(true);
            Wh_Log(L"v%s initialized in tool host pid %lu", kModVersion, GetCurrentProcessId());
            // Also log the former Wh_ModAfterInit info for parity
            Wh_Log(L"v%s, WhTool_ModAfterInit: pid=%lu, services started=%s, supervisor=%s", kModVersion, GetCurrentProcessId(), g_charmsServicesStarted.load() ? L"yes" : L"no", g_supervisorThread ? L"yes" : L"no");
            return TRUE;
        }
        Wh_Log(L"tool host startup failed");
        return FALSE;
    } catch (...) {
        Wh_Log(L"C++ exception during WhTool_ModInit");
        return FALSE;
    }
}

// Optional Windhawk callback: runs after Wh_ModInit and after the engine finished applying hooks.
void WhTool_ModAfterInit() {
    Wh_Log(L"v%s, Wh_ModAfterInit: pid=%lu, services started=%s, supervisor=%s",
           kModVersion,GetCurrentProcessId(),
           g_charmsServicesStarted.load()?L"yes":L"no",g_supervisorThread?L"yes":L"no");
}

void WhTool_ModUninit() {
    Wh_Log(L"v%s, Wh_ModUninit: pid=%lu, services started=%s",
           kModVersion,GetCurrentProcessId(),g_charmsServicesStarted.load()?L"yes":L"no");
    StopSupervisor();
    DisplaySwitchMod::Uninit();
    try {
    g_stop.store(true);
    if (g_hookThreadId) PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    if (g_threadId) PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread); g_hookThread = nullptr;
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread); g_thread = nullptr;
    }
    if (g_hookReady) { CloseHandle(g_hookReady); g_hookReady = nullptr; }
    if (g_ready) { CloseHandle(g_ready); g_ready = nullptr; }
    g_hookThreadId = g_threadId = 0;
    } catch (...) {
        Wh_Log(L"C++ exception during mod uninitialization; forcing worker cleanup");
        StopAndJoinWorkers();
    }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"settings changed (pid %lu, services started=%s)",
           GetCurrentProcessId(),g_charmsServicesStarted.load()?L"yes":L"no");
    DisplaySwitchMod::SettingsChanged();
    try {
        ReadWindhawkSettings();
        RefreshSystemAccent();
        Wh_Log(L"settings applied (charms=%s, language=%d, animation=%s, system animations=%s)",
               g_charmsEnabled.load()?L"on":L"off",g_uiLanguage.load(),g_useAnimations.load()?L"on":L"off",
               SystemAnimationsEnabled()?L"on":L"off");
        Wh_Log(L"search target=%s, Windows 8 Search Charm panel=%s",
               g_searchTarget.load()==0?L"automatic":L"windows search only",
               FindCompanionSearchPanel()?L"present (opened through its own window)":L"not found (Win+S fallback)");
        {
            const HWND startScreen=FindCompanionStartScreen();
            const int interop=CompanionInteropVersion(startScreen);
            Wh_Log(L"Windows 8 Start Screen mod=%s, interop v%d (%s)",
                   startScreen?L"present":L"not found (Start charm uses the Windows key)",interop,
                   interop>=kCompanionInteropMessageVersion?L"registered open message"
                                                          :L"WM_APP+1 toggle");
        }
    } catch (...) {
        Wh_Log(L"C++ exception while applying settings");
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

