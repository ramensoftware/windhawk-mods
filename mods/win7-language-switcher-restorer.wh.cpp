// ==WindhawkMod==
// @id             win7-language-switcher-restorer
// @name           Windows 7/8.1 Language Switcher Restorer
// @description    This mod restores the classic Windows 7 and Windows 8.1 language switcher on Windows 10 and 11
// @version        1.0.0
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @architecture   x86-64
// @compilerOptions -luser32 -lgdi32 -lgdiplus -ladvapi32 -lshell32 -lole32 -lshlwapi -ldwmapi -luxtheme -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7/8.1 Language Switcher Restorer

## About
This mod restores the classic **Windows 7 and 8.1 language switcher** on Windows 10 and Windows 11, enabling rapid switching between keyboard layouts and input languages.

**Important**: This is a **best-effort visual and functional recreation**. The mod intercepts clicks on the taskbar language indicator, as well as the **Win+Space** and **Alt+Shift** keyboard shortcuts, replacing the modern Windows 10/11 flyout with a fast, lightweight, classic switcher.

The mod has been tested on Windows 10 1809, Windows 10 21H2, and Windows 11 24H2.

## The "Show the language bar" Option in Windows 7

In classic versions of Windows, this option allows the language bar to be displayed on the desktop or taskbar, facilitating convenient switching between languages and layouts.

**What it does:**

- Shows a small language icon (e.g., "EN" or "IT") near the clock.
- Enables changing the input language with a single click.
- Provides quick access to language and keyboard settings.

**Where it can appear:**
- **Docked to the taskbar:** next to the clock in the notification area.
- **Floating on the desktop:** as a small movable toolbar.
- **Hidden:** completely turned off, though keyboard shortcuts like `Alt+Shift` remain active.

## Main Features

- **Two classic styles**: selection between the Windows 7 classic menu and the Windows 8.1 modern flyout, configurable directly from the mod's settings.
- **Light and dark theme support**: automatic adaptation to the system's colors and theme.
- **Quick dismissal**: the switcher closes upon clicking outside it or pressing the `Esc` key.
- **Translated into 27 languages**: fully localized interface for users worldwide.
- **Win+Space support**: holding `Win` and pressing `Space` cycles through installed layouts; releasing the Windows key applies the selection and dismisses the flyout.
- **Alt+Shift and Ctrl+Shift support**: enables fast switching between languages.
- **Multi-monitor and DPI aware**: correct display on any screen and resolution.
- **Completely reversible**: disabling or uninstalling the mod removes all changes and restores the original system behavior.

## Stability and Safety Notes

- The mod is designed for stability and does not interfere with the startup of File Explorer.
- It does not leave permanent traces on the system and does not modify system files.
- All resources are managed carefully to prevent memory issues or performance degradation.
- Every function includes safeguards to avoid crashes or system freezes.

## Known Limitations

- **Best-effort recreation**: this is an in-memory reproduction of the classic interface and does not replace Windows' internal language services.

## Credits

- [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher) - Inspiration for the shell interception techniques
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- switcherStyle: win8
  $name: Switcher Style
  $description: This setting chooses between the Windows 8.1 Modern Flyout or the Windows 7 Classic Menu.
  $options:
    - win8: Windows 8.1 Modern Flyout
    - win7: Windows 7 Classic Menu

- language: auto
  $name: UI Language
  $description: This setting chooses the interface language for options, links, and shortcut tips.
  $options:
    - auto: Automatic (Windows display language)
    - it: Italiano (Italian)
    - en: English (United States)
    - tr: Türkçe (Turkish)
    - fr: Français (French)
    - es: Español (Spanish)
    - pt: Português (Portuguese)
    - zh: 中文 (Simplified Chinese)
    - pl: Polski (Polish)
    - nl: Nederlands (Dutch)
    - de: Deutsch (German)
    - ru: Русский (Russian)
    - ja: 日本語 (Japanese)
    - ko: 한국어 (Korean)
    - ar: العربية (Arabic)
    - sv: Svenska (Swedish)
    - cs: Čeština (Czech)
    - da: Dansk (Danish)
    - fi: Suomi (Finnish)
    - el: Ελληνικά (Greek)
    - he: עברית (Hebrew)
    - hu: Magyar (Hungarian)
    - nb: Norsk (Norwegian)
    - ro: Română (Romanian)
    - sk: Slovenčina (Slovak)
    - uk: Українська (Ukrainian)
    - af: Afrikaans

- themeMode: win8_purple
  $name: Color Theme
  $description: This setting chooses the color scheme used for the selected item and flyout styling.
  $options:
    - win8_purple: Windows 8.1 Purple (#5B2C82)
    - auto: Follow Windows Accent & Theme
    - dark: Dark Mode
    - light: Light Mode
    - custom: Custom Accent Color

- customAccentColor: "#5B2C82"
  $name: Custom Accent Color (Hex)
  $description: This setting sets the hex color used when theme is set to 'Custom' (e.g. #5B2C82 or #0078D7).

- enableWinSpace: true
  $name: Enable Win+Space Cycling
  $description: This setting intercepts Win+Space to cycle through keyboard layouts, applying the selection upon releasing the Windows key.

- enableAltShift: true
  $name: Enable Alt+Shift Toggle
  $description: This setting intercepts Alt+Shift (and Ctrl+Shift) to toggle through keyboard layouts.

- hookTrayClicks: true
  $name: Intercept Taskbar Language Button
  $description: This setting intercepts clicks on the taskbar language button / input indicator to show this classic switcher.

- showShortcutHint: true
  $name: Show Shortcut Hint in Footer
  $description: This setting displays 'To switch, press Windows key + Space' at the bottom of the flyout.

- customPreferencesCmd: "ms-settings:regionlanguage"
  $name: Language Preferences Command
  $description: This setting sets the command or URL executed when clicking 'Language preferences' (e.g. 'ms-settings:regionlanguage' or 'control.exe /name Microsoft.Language').

- enableCustomHotkey: false
  $name: Enable Ctrl+Shift+L Shortcut
  $description: This setting intercepts Ctrl+Shift+L to open the switcher without changing the current layout. Disabled by default to avoid any risk of colliding with a shortcut already in use in other apps.
*/
// ==/WindhawkModSettings==

#ifndef WINVER
#define WINVER 0x0602
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <msctf.h>
#include <objbase.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <atomic>
#include <mutex>

#if __has_include(<windhawk_api.h>)
#include <windhawk_api.h>
#include <windhawk_utils.h>
#else
// Fallback declarations when building outside Windhawk environment for testing
#ifdef __cplusplus
extern "C" {
#endif
void Wh_Log(const wchar_t* format, ...);
int Wh_GetIntSetting(const wchar_t* name);
const wchar_t* Wh_GetStringSetting(const wchar_t* name);
void Wh_FreeStringSetting(const wchar_t* string);
BOOL Wh_SetFunctionHook(void* target, void* hook, void** original);
int Wh_GetModStoragePath(wchar_t* buffer, int max_len);
#ifdef __cplusplus
}
namespace WindhawkUtils {
    template <typename T>
    inline BOOL SetFunctionHook(T target, T hook, T* original) {
        return Wh_SetFunctionHook(reinterpret_cast<void*>(target), reinterpret_cast<void*>(hook), reinterpret_cast<void**>(original));
    }
    using WH_SUBCLASSPROC = LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
    inline BOOL SetWindowSubclassFromAnyThread(HWND hWnd, WH_SUBCLASSPROC pfnSubclass, DWORD_PTR dwRefData = 0) {
        return SetWindowSubclass(hWnd, reinterpret_cast<SUBCLASSPROC>(reinterpret_cast<void*>(pfnSubclass)), reinterpret_cast<UINT_PTR>(pfnSubclass), dwRefData);
    }
    inline BOOL RemoveWindowSubclassFromAnyThread(HWND hWnd, WH_SUBCLASSPROC pfnSubclass) {
        return RemoveWindowSubclass(hWnd, reinterpret_cast<SUBCLASSPROC>(reinterpret_cast<void*>(pfnSubclass)), reinterpret_cast<UINT_PTR>(pfnSubclass));
    }
}
#endif
#endif

// =============================================================================
// Comprehensive RAII Resource Management Infrastructure
// =============================================================================

// RAII wrapper for generic Win32 HANDLE (Events, Mutexes, Threads, Files).
class WinHandle {
public:
    WinHandle() noexcept = default;
    explicit WinHandle(HANDLE h) noexcept : h_(h) {}
    ~WinHandle() { reset(); }

    WinHandle(const WinHandle&) = delete;
    WinHandle& operator=(const WinHandle&) = delete;

    WinHandle(WinHandle&& o) noexcept : h_(o.release()) {}
    WinHandle& operator=(WinHandle&& o) noexcept {
        if (this != &o) { reset(o.release()); }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept {
        return h_ != nullptr && h_ != INVALID_HANDLE_VALUE;
    }
    [[nodiscard]] HANDLE get() const noexcept { return h_; }
    [[nodiscard]] explicit operator bool() const noexcept { return valid(); }

    HANDLE release() noexcept {
        HANDLE tmp = h_;
        h_ = INVALID_HANDLE_VALUE;
        return tmp;
    }

    void reset(HANDLE h = INVALID_HANDLE_VALUE) noexcept {
        if (valid()) { CloseHandle(h_); }
        h_ = h;
    }

private:
    HANDLE h_ = INVALID_HANDLE_VALUE;
};

// RAII wrapper for HMODULE loaded via LoadLibrary / GetModuleHandle.
class LoadedModule {
public:
    LoadedModule() noexcept = default;
    explicit LoadedModule(HMODULE h, bool owned = true) noexcept : h_(h), owned_(owned) {}
    ~LoadedModule() { reset(); }

    LoadedModule(const LoadedModule&) = delete;
    LoadedModule& operator=(const LoadedModule&) = delete;

    LoadedModule(LoadedModule&& o) noexcept : h_(o.h_), owned_(o.owned_) {
        o.h_ = nullptr; o.owned_ = false;
    }
    LoadedModule& operator=(LoadedModule&& o) noexcept {
        if (this != &o) { reset(); h_ = o.h_; owned_ = o.owned_; o.h_ = nullptr; o.owned_ = false; }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept { return h_ != nullptr; }
    [[nodiscard]] HMODULE get() const noexcept { return h_; }

    HMODULE release() noexcept {
        HMODULE tmp = h_; h_ = nullptr; owned_ = false; return tmp;
    }

    void reset() noexcept {
        if (h_ && owned_) { FreeLibrary(h_); }
        h_ = nullptr; owned_ = false;
    }

private:
    HMODULE h_ = nullptr;
    bool owned_ = false;
};

// RAII template wrapper for GDI objects (HFONT, HBRUSH, HPEN, HBITMAP).
template <typename T>
class ScopedGdiObject {
public:
    ScopedGdiObject() noexcept : obj_(nullptr) {}
    explicit ScopedGdiObject(T obj) noexcept : obj_(obj) {}
    ~ScopedGdiObject() { reset(); }

    ScopedGdiObject(const ScopedGdiObject&) = delete;
    ScopedGdiObject& operator=(const ScopedGdiObject&) = delete;

    ScopedGdiObject(ScopedGdiObject&& o) noexcept : obj_(o.release()) {}
    ScopedGdiObject& operator=(ScopedGdiObject&& o) noexcept {
        if (this != &o) { reset(o.release()); }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept { return obj_ != nullptr; }
    [[nodiscard]] T get() const noexcept { return obj_; }
    [[nodiscard]] explicit operator bool() const noexcept { return valid(); }

    T release() noexcept {
        T tmp = obj_;
        obj_ = nullptr;
        return tmp;
    }

    void reset(T obj = nullptr) noexcept {
        if (obj_) { DeleteObject(obj_); }
        obj_ = obj;
    }

private:
    T obj_ = nullptr;
};

using ScopedFont = ScopedGdiObject<HFONT>;
using ScopedBrush = ScopedGdiObject<HBRUSH>;
using ScopedPen = ScopedGdiObject<HPEN>;
using ScopedBitmap = ScopedGdiObject<HBITMAP>;

// RAII wrapper for DC selection (SelectObject / restore on exit).
class ScopedSelectObject {
public:
    ScopedSelectObject(HDC hdc, HGDIOBJ newObj) noexcept : hdc_(hdc), oldObj_(nullptr) {
        if (hdc_ && newObj) {
            oldObj_ = SelectObject(hdc_, newObj);
            if (oldObj_ == HGDI_ERROR) {
                oldObj_ = nullptr;
            }
        }
    }
    ~ScopedSelectObject() {
        if (hdc_ && oldObj_ && oldObj_ != HGDI_ERROR) {
            SelectObject(hdc_, oldObj_);
        }
    }

    ScopedSelectObject(const ScopedSelectObject&) = delete;
    ScopedSelectObject& operator=(const ScopedSelectObject&) = delete;

private:
    HDC hdc_ = nullptr;
    HGDIOBJ oldObj_ = nullptr;
};

// RAII wrapper for Device Contexts (GetDC / ReleaseDC or CreateCompatibleDC / DeleteDC).
class ScopedDC {
public:
    ScopedDC() noexcept : hdc_(nullptr), hwnd_(nullptr), isCompatible_(false) {}
    
    static ScopedDC FromWindow(HWND hwnd) noexcept {
        ScopedDC dc;
        dc.hwnd_ = hwnd;
        dc.hdc_ = GetDC(hwnd);
        dc.isCompatible_ = false;
        return dc;
    }

    static ScopedDC CreateCompatible(HDC hdcRef) noexcept {
        ScopedDC dc;
        dc.hwnd_ = nullptr;
        dc.hdc_ = (hdcRef ? CreateCompatibleDC(hdcRef) : nullptr);
        dc.isCompatible_ = true;
        return dc;
    }

    ~ScopedDC() { reset(); }

    ScopedDC(const ScopedDC&) = delete;
    ScopedDC& operator=(const ScopedDC&) = delete;

    ScopedDC(ScopedDC&& o) noexcept 
        : hdc_(o.hdc_), hwnd_(o.hwnd_), isCompatible_(o.isCompatible_) {
        o.hdc_ = nullptr; o.hwnd_ = nullptr; o.isCompatible_ = false;
    }
    ScopedDC& operator=(ScopedDC&& o) noexcept {
        if (this != &o) {
            reset();
            hdc_ = o.hdc_; hwnd_ = o.hwnd_; isCompatible_ = o.isCompatible_;
            o.hdc_ = nullptr; o.hwnd_ = nullptr; o.isCompatible_ = false;
        }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept { return hdc_ != nullptr; }
    [[nodiscard]] HDC get() const noexcept { return hdc_; }

    void reset() noexcept {
        if (hdc_) {
            if (isCompatible_) {
                DeleteDC(hdc_);
            } else {
                ReleaseDC(hwnd_, hdc_);
            }
        }
        hdc_ = nullptr;
        hwnd_ = nullptr;
        isCompatible_ = false;
    }

private:
    HDC hdc_ = nullptr;
    HWND hwnd_ = nullptr;
    bool isCompatible_ = false;
};

// RAII wrapper for BeginPaint / EndPaint.
class ScopedPaintDC {
public:
    explicit ScopedPaintDC(HWND hwnd) noexcept : hwnd_(hwnd) {
        ZeroMemory(&ps_, sizeof(ps_));
        if (hwnd_ && IsWindow(hwnd_)) {
            hdc_ = BeginPaint(hwnd_, &ps_);
        }
    }
    ~ScopedPaintDC() {
        if (hwnd_ && hdc_) {
            EndPaint(hwnd_, &ps_);
        }
    }

    ScopedPaintDC(const ScopedPaintDC&) = delete;
    ScopedPaintDC& operator=(const ScopedPaintDC&) = delete;

    [[nodiscard]] bool valid() const noexcept { return hdc_ != nullptr; }
    [[nodiscard]] HDC get() const noexcept { return hdc_; }
    [[nodiscard]] const PAINTSTRUCT& ps() const noexcept { return ps_; }

private:
    HWND hwnd_ = nullptr;
    HDC hdc_ = nullptr;
    PAINTSTRUCT ps_{};
};

// RAII wrapper for HKEY handles.
class ScopedRegKey {
public:
    ScopedRegKey() noexcept = default;
    explicit ScopedRegKey(HKEY k) noexcept : key_(k) {}
    ~ScopedRegKey() { reset(); }

    ScopedRegKey(const ScopedRegKey&) = delete;
    ScopedRegKey& operator=(const ScopedRegKey&) = delete;

    ScopedRegKey(ScopedRegKey&& o) noexcept : key_(o.release()) {}
    ScopedRegKey& operator=(ScopedRegKey&& o) noexcept {
        if (this != &o) { reset(o.release()); }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept { return key_ != nullptr; }
    [[nodiscard]] HKEY get() const noexcept { return key_; }

    HKEY release() noexcept {
        HKEY tmp = key_;
        key_ = nullptr;
        return tmp;
    }

    void reset(HKEY k = nullptr) noexcept {
        if (key_) { RegCloseKey(key_); }
        key_ = k;
    }

private:
    HKEY key_ = nullptr;
};

// RAII wrapper for HHOOK handles.
class ScopedHook {
public:
    ScopedHook() noexcept = default;
    explicit ScopedHook(HHOOK h) noexcept : hook_(h) {}
    ~ScopedHook() { reset(); }

    ScopedHook(const ScopedHook&) = delete;
    ScopedHook& operator=(const ScopedHook&) = delete;

    ScopedHook(ScopedHook&& o) noexcept : hook_(o.release()) {}
    ScopedHook& operator=(ScopedHook&& o) noexcept {
        if (this != &o) { reset(o.release()); }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept { return hook_ != nullptr; }
    [[nodiscard]] HHOOK get() const noexcept { return hook_; }

    HHOOK release() noexcept {
        HHOOK tmp = hook_;
        hook_ = nullptr;
        return tmp;
    }

    void reset(HHOOK h = nullptr) noexcept {
        if (hook_) { UnhookWindowsHookEx(hook_); }
        hook_ = h;
    }

private:
    HHOOK hook_ = nullptr;
};

// RAII wrapper for COM initialization.
class ScopedCoInit {
public:
    explicit ScopedCoInit(DWORD dwCoInit = COINIT_APARTMENTTHREADED) noexcept : hr_(CoInitializeEx(nullptr, dwCoInit)) {}
    ~ScopedCoInit() {
        if (SUCCEEDED(hr_)) {
            CoUninitialize();
        }
    }
    ScopedCoInit(const ScopedCoInit&) = delete;
    ScopedCoInit& operator=(const ScopedCoInit&) = delete;
    [[nodiscard]] bool succeeded() const noexcept { return SUCCEEDED(hr_); }
    [[nodiscard]] HRESULT result() const noexcept { return hr_; }

private:
    HRESULT hr_ = E_FAIL;
};

// RAII wrapper for HMENU handles.
class ScopedMenu {
public:
    ScopedMenu() noexcept = default;
    explicit ScopedMenu(HMENU h) noexcept : menu_(h) {}
    ~ScopedMenu() { reset(); }

    ScopedMenu(const ScopedMenu&) = delete;
    ScopedMenu& operator=(const ScopedMenu&) = delete;

    ScopedMenu(ScopedMenu&& o) noexcept : menu_(o.release()) {}
    ScopedMenu& operator=(ScopedMenu&& o) noexcept {
        if (this != &o) { reset(o.release()); }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept { return menu_ != nullptr; }
    [[nodiscard]] HMENU get() const noexcept { return menu_; }
    [[nodiscard]] explicit operator bool() const noexcept { return valid(); }

    HMENU release() noexcept {
        HMENU tmp = menu_;
        menu_ = nullptr;
        return tmp;
    }

    void reset(HMENU h = nullptr) noexcept {
        if (menu_) { DestroyMenu(menu_); }
        menu_ = h;
    }

private:
    HMENU menu_ = nullptr;
};

// RAII wrapper for HTHEME handles.
class ScopedTheme {
public:
    ScopedTheme() noexcept = default;
    explicit ScopedTheme(HTHEME h) noexcept : theme_(h) {}
    ~ScopedTheme() { reset(); }

    ScopedTheme(const ScopedTheme&) = delete;
    ScopedTheme& operator=(const ScopedTheme&) = delete;

    ScopedTheme(ScopedTheme&& o) noexcept : theme_(o.release()) {}
    ScopedTheme& operator=(ScopedTheme&& o) noexcept {
        if (this != &o) { reset(o.release()); }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept { return theme_ != nullptr; }
    [[nodiscard]] HTHEME get() const noexcept { return theme_; }

    HTHEME release() noexcept {
        HTHEME tmp = theme_;
        theme_ = nullptr;
        return tmp;
    }

    void reset(HTHEME h = nullptr) noexcept {
        if (theme_) { CloseThemeData(theme_); }
        theme_ = h;
    }

private:
    HTHEME theme_ = nullptr;
};

// RAII wrapper for temporary cursor changes.
class ScopedCursor {
public:
    explicit ScopedCursor(HCURSOR hNew) noexcept : hOld_(SetCursor(hNew)) {}
    ~ScopedCursor() {
        if (hOld_) {
            SetCursor(hOld_);
        }
    }
    ScopedCursor(const ScopedCursor&) = delete;
    ScopedCursor& operator=(const ScopedCursor&) = delete;

private:
    HCURSOR hOld_ = nullptr;
};

// RAII wrapper for GdiplusStartup / GdiplusShutdown. Owns the process-wide
// GDI+ token so a missed Uninit (or an exception during Init) cannot leak it.
class ScopedGdiplus {
public:
    ScopedGdiplus() noexcept = default;
    ~ScopedGdiplus() { reset(); }

    ScopedGdiplus(const ScopedGdiplus&) = delete;
    ScopedGdiplus& operator=(const ScopedGdiplus&) = delete;

    ScopedGdiplus(ScopedGdiplus&& o) noexcept : token_(o.token_), ready_(o.ready_) {
        o.token_ = 0;
        o.ready_ = false;
    }
    ScopedGdiplus& operator=(ScopedGdiplus&& o) noexcept {
        if (this != &o) {
            reset();
            token_ = o.token_;
            ready_ = o.ready_;
            o.token_ = 0;
            o.ready_ = false;
        }
        return *this;
    }

    [[nodiscard]] bool ready() const noexcept { return ready_; }
    [[nodiscard]] explicit operator bool() const noexcept { return ready_; }

    bool startup() noexcept {
        if (ready_) {
            return true;
        }
        try {
            Gdiplus::GdiplusStartupInput input;
            const Gdiplus::Status st = Gdiplus::GdiplusStartup(&token_, &input, nullptr);
            ready_ = (st == Gdiplus::Ok && token_ != 0);
            if (!ready_) {
                token_ = 0;
            }
            return ready_;
        } catch (...) {
            token_ = 0;
            ready_ = false;
            return false;
        }
    }

    void reset() noexcept {
        if (!ready_) {
            token_ = 0;
            return;
        }
        try {
            Gdiplus::GdiplusShutdown(token_);
        } catch (...) {}
        token_ = 0;
        ready_ = false;
    }

private:
    ULONG_PTR token_ = 0;
    bool ready_ = false;
};

// =============================================================================
// Dark Theme Context Menu Infrastructure (inspired by win7-network-flyout)
// =============================================================================
namespace DarkContextMenu {
enum class AppMode {
    Default,
    AllowDark,
    ForceDark,
    ForceLight,
    Max
};

using FlushMenuThemes_T = void(WINAPI*)();
using SetPreferredAppMode_T = AppMode(WINAPI*)(AppMode);
using AllowDarkModeForWindow_T = bool(WINAPI*)(HWND, bool);

static HMODULE g_hUxtheme = nullptr;
static FlushMenuThemes_T pFlushMenuThemes = nullptr;
static SetPreferredAppMode_T pSetPreferredAppMode = nullptr;
static AllowDarkModeForWindow_T pAllowDarkModeForWindow = nullptr;
static AppMode g_initialAppMode = AppMode::Default;

static void Init() {
    g_hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_hUxtheme) {
        pSetPreferredAppMode = reinterpret_cast<SetPreferredAppMode_T>(reinterpret_cast<void*>(GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(135))));
        pFlushMenuThemes = reinterpret_cast<FlushMenuThemes_T>(reinterpret_cast<void*>(GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(136))));
        pAllowDarkModeForWindow = reinterpret_cast<AllowDarkModeForWindow_T>(reinterpret_cast<void*>(GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(133))));
        if (pSetPreferredAppMode) {
            g_initialAppMode = pSetPreferredAppMode(AppMode::Default);
            pSetPreferredAppMode(g_initialAppMode);
        }
    }
}

static void AllowDarkModeForWindow(HWND hwnd, bool allow) {
    if (pAllowDarkModeForWindow && hwnd && IsWindow(hwnd)) {
        pAllowDarkModeForWindow(hwnd, allow);
    }
}

static void Apply(BOOL dark) {
    if (!g_hUxtheme || !pSetPreferredAppMode || !pFlushMenuThemes) return;
    pSetPreferredAppMode(dark ? AppMode::ForceDark : AppMode::Default);
    pFlushMenuThemes();
}

static void Restore() {
    if (!g_hUxtheme || !pSetPreferredAppMode || !pFlushMenuThemes) return;
    pSetPreferredAppMode(g_initialAppMode);
    pFlushMenuThemes();
}

static void Uninit() {
    if (pSetPreferredAppMode) {
        pSetPreferredAppMode(g_initialAppMode);
        if (pFlushMenuThemes) pFlushMenuThemes();
    }
    if (g_hUxtheme) {
        FreeLibrary(g_hUxtheme);
        g_hUxtheme = nullptr;
    }
    pSetPreferredAppMode = nullptr;
    pFlushMenuThemes = nullptr;
    pAllowDarkModeForWindow = nullptr;
}
} // namespace DarkContextMenu

// =============================================================================
// Language Abbreviations Table (65+ Languages)
// =============================================================================
struct LangAbbrevEntry {
    WORD langId;
    const wchar_t* abbrev;
};

static const LangAbbrevEntry g_LangAbbrevs[] = {
    { LANG_AFRIKAANS,   L"AFR" },
    { LANG_ALBANIAN,    L"ALB" },
    { LANG_ARABIC,      L"ARA" },
    { LANG_ARMENIAN,    L"ARM" },
    { LANG_ASSAMESE,    L"ASM" },
    { LANG_AZERI,       L"AZE" },
    { LANG_BASQUE,      L"BSQ" },
    { LANG_BELARUSIAN,  L"BEL" },
    { LANG_BENGALI,     L"BEN" },
    { LANG_BULGARIAN,   L"BGR" },
    { LANG_CATALAN,     L"CAT" },
    { LANG_CHINESE,     L"CHS" },
    { LANG_CROATIAN,    L"HRV" },
    { LANG_CZECH,       L"CSY" },
    { LANG_DANISH,      L"DAN" },
    { LANG_DUTCH,       L"NLD" },
    { LANG_ENGLISH,     L"ENG" },
    { LANG_ESTONIAN,    L"ETI" },
    { LANG_FAEROESE,    L"FRO" },
    { LANG_FARSI,       L"FAR" },
    { LANG_FINNISH,     L"FIN" },
    { LANG_FRENCH,      L"FRA" },
    { LANG_GEORGIAN,    L"GEO" },
    { LANG_GERMAN,      L"DEU" },
    { LANG_GREEK,       L"ELL" },
    { LANG_GUJARATI,    L"GUJ" },
    { LANG_HEBREW,      L"HEB" },
    { LANG_HINDI,       L"HIN" },
    { LANG_HUNGARIAN,   L"HUN" },
    { LANG_ICELANDIC,   L"ISL" },
    { LANG_INDONESIAN,  L"IND" },
    { LANG_ITALIAN,     L"ITA" },
    { LANG_JAPANESE,    L"JPN" },
    { LANG_KANNADA,     L"KAN" },
    { LANG_KASHMIRI,    L"KSH" },
    { LANG_KAZAK,       L"KAZ" },
    { LANG_KONKANI,     L"KOK" },
    { LANG_KOREAN,      L"KOR" },
    { LANG_LATVIAN,     L"LVI" },
    { LANG_LITHUANIAN,  L"LTH" },
    { LANG_MACEDONIAN,  L"MKI" },
    { LANG_MALAY,       L"MSL" },
    { LANG_MALAYALAM,   L"MAL" },
    { LANG_MANIPURI,    L"MPI" },
    { LANG_MARATHI,     L"MAR" },
    { LANG_NEPALI,      L"NEP" },
    { LANG_NORWEGIAN,   L"NOR" },
    { LANG_ORIYA,       L"ORI" },
    { LANG_POLISH,      L"PLK" },
    { LANG_PORTUGUESE,  L"PTG" },
    { LANG_PUNJABI,     L"PAN" },
    { LANG_ROMANIAN,    L"ROM" },
    { LANG_RUSSIAN,     L"RUS" },
    { LANG_SANSKRIT,    L"SAN" },
    { LANG_SERBIAN,     L"SRB" },
    { LANG_SLOVAK,      L"SLK" },
    { LANG_SLOVENIAN,   L"SLV" },
    { LANG_SPANISH,     L"ESP" },
    { LANG_SWAHILI,     L"SWK" },
    { LANG_SWEDISH,     L"SVE" },
    { LANG_TAMIL,       L"TAM" },
    { LANG_TATAR,       L"TTT" },
    { LANG_TELUGU,      L"TEL" },
    { LANG_THAI,        L"THA" },
    { LANG_TURKISH,     L"TRK" },
    { LANG_UKRAINIAN,   L"UKR" },
    { LANG_URDU,        L"URD" },
    { LANG_UZBEK,       L"UZB" },
    { LANG_VIETNAMESE,  L"VIT" },
};

// =============================================================================
// Localized UI String Catalog (27 Languages)
// =============================================================================
struct LocalizedUiText {
    const wchar_t* langTag;
    const wchar_t* preferences;
    const wchar_t* shortcutHint;
    const wchar_t* showLanguageBar;
};

static const LocalizedUiText kLocalizedStrings[] = {
    { L"it", L"Preferenze lingua", L"Per passare da una lingua all'altra, premi tasto Windows + Spazio", L"Mostra barra della lingua" },
    { L"en", L"Language preferences", L"To switch, press Windows key + Space", L"Show the Language bar" },
    { L"tr", L"Dil tercihleri", L"Geçiş yapmak için Windows tuşu + Boşluk tuşuna basın", L"Dil çubuğunu göster" },
    { L"fr", L"Préférences linguistiques", L"Pour basculer, appuyez sur la touche Windows + Espace", L"Afficher la barre des langues" },
    { L"es", L"Preferencias de idioma", L"Para cambiar, presione la tecla Windows + Barra espaciadora", L"Mostrar la barra de idioma" },
    { L"pt", L"Preferências de idioma", L"Para alternar, pressione a tecla Windows + Espaço", L"Mostrar a barra de idiomas" },
    { L"zh", L"语言首选项", L"若要切换，请按 Windows 徽标键 + 空格键", L"显示语言栏" },
    { L"pl", L"Preferencje językowe", L"Aby przełączyć, naciśnij klawisz Windows + Spacja", L"Pokaż pasek języka" },
    { L"nl", L"Taalvoorkeuren", L"Druk op Windows-toets + Spatiebalk om te wisselen", L"Taalbalk weergeven" },
    { L"de", L"Spracheinstellungen", L"Drücken Sie Windows-Taste + Leertaste, um zu wechseln", L"Sprachenleiste anzeigen" },
    { L"ru", L"Настройки языка", L"Для переключения нажмите клавишу Windows + Пробел", L"Отобразить языковую панель" },
    { L"ja", L"言語の設定", L"切り替えるには、Windows ロゴ キー + Space キーを押します", L"言語バーを表示" },
    { L"ko", L"언어 기본 설정", L"전환하려면 Windows 키 + 스페이스바를 누르세요", L"언어 표시줄 표시" },
    { L"ar", L"تفضيلات اللغة", L"للتبديل، اضغط على مفتاح Windows + المسافة", L"إظهار شريط اللغة" },
    { L"sv", L"Språkinställningar", L"Tryck på Windows-tangenten + Blanksteg för att växla", L"Visa språkfältet" },
    { L"cs", L"Jazykové předvolby", L"Chcete-li přepnout, stiskněte klávesu Windows + Mezerník", L"Zobrazit panel jazyků" },
    { L"da", L"Sprogindstillinger", L"Tryk på Windows-tasten + Mellemrum for at skifte", L"Vis proceslinjen Sprog" },
    { L"fi", L"Kieliasetukset", L"Vaihda painamalla Windows-näppäintä + välilyöntiä", L"Näytä kielipalkki" },
    { L"el", L"Προτιμήσεις γλώσσας", L"Για εναλλαγή, πατήστε το πλήκτρο Windows + Διαστήματος", L"Εμφάνιση της γραμμής γλώσσας" },
    { L"he", L"העדפות שפה", L"כדי לעבור, לחץ על מקש Windows + רווח", L"הצג את סרגל השפה" },
    { L"hu", L"Nyelvi beállítások", L"A váltáshoz nyomja le a Windows billentyű + Szóköz billentyűt", L"Nyelvi sáv megjelenítése" },
    { L"nb", L"Språkinnstillinger", L"Trykk på Windows-tasten + Mellomrom for å bytte", L"Vis språklinjen" },
    { L"ro", L"Preferințe de limbă", L"Pentru a comuta, apăsați tasta Windows + Spațiu", L"Afișare bară de limbă" },
    { L"sk", L"Jazykové predvoľby", L"Ak chcete prepnúť, stlačte kláves s logom Windows + Medzerník", L"Zobraziť panel jazykov" },
    { L"uk", L"Мовні параметри", L"Щоб переключити, натисніть клавішу Windows + Пробіл", L"Відобразити мовну панель" },
    { L"af", L"Taalvoorkeure", L"Vir maklike wisseling, druk Windows-sleutel + Spasie", L"Wys die taalbalk" }
};

// =============================================================================
// Keyboard Layout & Input Profile Information
// =============================================================================
struct KeyboardLayoutItem {
    HKL hkl = nullptr;
    LANGID langId = 0;
    std::wstring langName;      // e.g. "Afrikaans", "English (United States)"
    std::wstring langAbbrev;    // e.g. "AFR", "ENG", "ITA"
    std::wstring subAbbrev;     // e.g. "US", "INTL", "DV"
    std::wstring layoutName;    // e.g. "US keyboard", "Italian"
    std::wstring klid;          // e.g. "00000409"
    bool isCurrent = false;
};

// =============================================================================
// Global State & Configuration
// =============================================================================
static const wchar_t* kFlyoutClassName = L"Windhawk_Win78LanguageFlyout";

static std::atomic<bool> g_shuttingDown{false};
static std::atomic<bool> g_unloading{false};
static std::atomic<int> g_inFlightSubclasses{0};

static std::mutex g_stateMutex;
// Thread ID of the thread that ran Wh_ModInit (normally explorer's main UI
// thread, the one pumping the taskbar/tray message loop). CreateWindowExW_Hook
// and ShowWindow_Hook are process-wide hooks, so they also fire for windows
// that Explorer/DWM create from other threads (e.g. during Aero Peek, which
// creates and shows many windows in quick succession, sometimes off the main
// thread). Touching our own HWND/state machinery from the wrong thread is
// unsafe, so those hooks only run their special-casing when called on this
// thread and otherwise fall straight through to the original function.
static DWORD g_mainThreadId = 0;
static std::vector<KeyboardLayoutItem> g_layouts;
static size_t g_selectedIndex = 0;
static int g_hoveredIndex = -1;
static bool g_hoveredFooter = false;
static int g_hoveredWin7Index = -1;

static HWND g_hFlyoutWnd = nullptr;
static HWND g_targetWindow = nullptr;
static bool g_isWinSpaceCycling = false;
static DWORD g_lastTriggerTick = 0;

static ScopedHook g_keyboardHook;
static ScopedHook g_mouseHook;
static ScopedGdiplus g_gdiplus;

// Subclassed window tracking
static std::mutex g_subclassedWindowsMutex;
static std::unordered_set<HWND> g_subclassedWindows;
static BOOL g_flyoutWasVisibleOnDown = FALSE;

// Settings
static std::wstring g_switcherStyle = L"win8";          // "win8" or "win7"
static std::wstring g_uiLanguage = L"auto";             // "auto", "it", "en", "tr", "fr", "es", etc.
static std::wstring g_themeMode = L"win8_purple";        // "win8_purple", "auto", "dark", "light", "custom"
static COLORREF g_customAccentColor = RGB(91, 44, 130); // Default Win 8.1 purple (#5B2C82)
static bool g_enableWinSpace = true;
static bool g_enableAltShift = true;
static bool g_hookTrayClicks = true;
static bool g_showShortcutHint = true;
static bool g_enableCustomHotkey = false;
static std::wstring g_customPreferencesCmd = L"ms-settings:regionlanguage";

// =============================================================================
// Color & Utility Helpers
// =============================================================================
static COLORREF ParseHexColor(const std::wstring& hexStr, COLORREF fallback) {
    try {
        if (hexStr.empty()) return fallback;
        const wchar_t* p = hexStr.c_str();
        while (*p == L' ' || *p == L'\t') p++;
        if (*p == L'#') p++;
        if (!*p) return fallback;

        wchar_t* endPtr = nullptr;
        unsigned long val = wcstoul(p, &endPtr, 16);
        if (endPtr == p) return fallback;
        
        BYTE r = static_cast<BYTE>((val >> 16) & 0xFF);
        BYTE g = static_cast<BYTE>((val >> 8) & 0xFF);
        BYTE b = static_cast<BYTE>(val & 0xFF);
        return RGB(r, g, b);
    } catch (...) {
        return fallback;
    }
}

static COLORREF GetSystemAccentColor() {
    try {
        DWORD color = 0;
        BOOL opaque = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
            BYTE r = static_cast<BYTE>((color >> 16) & 0xFF);
            BYTE g = static_cast<BYTE>((color >> 8) & 0xFF);
            BYTE b = static_cast<BYTE>(color & 0xFF);
            return RGB(r, g, b);
        }
    } catch (...) {}
    return RGB(91, 44, 130);
}

static bool IsDarkModeActive() {
    try {
        ScopedRegKey key;
        HKEY h = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                          0, KEY_READ, &h) == ERROR_SUCCESS) {
            key.reset(h);
            DWORD appsUseLightTheme = 1;
            DWORD size = sizeof(appsUseLightTheme);
            if (RegQueryValueExW(key.get(), L"AppsUseLightTheme", nullptr, nullptr,
                                 reinterpret_cast<LPBYTE>(&appsUseLightTheme), &size) == ERROR_SUCCESS) {
                return appsUseLightTheme == 0;
            }
        }
    } catch (...) {}
    return false;
}

static int ScaleForDpi(int value, UINT dpi) {
    if (dpi == 0) dpi = 96;
    return MulDiv(value, static_cast<int>(dpi), 96);
}

static UINT GetWindowDpi(HWND hwnd) {
    try {
        LoadedModule hUser(GetModuleHandleW(L"user32.dll"), false);
        if (hUser.valid()) {
            auto pGetDpi = reinterpret_cast<UINT(WINAPI*)(HWND)>(reinterpret_cast<void*>(GetProcAddress(hUser.get(), "GetDpiForWindow")));
            if (pGetDpi && hwnd && IsWindow(hwnd)) {
                UINT dpi = pGetDpi(hwnd);
                if (dpi > 0) return dpi;
            }
        }
    } catch (...) {}
    
    try {
        ScopedDC dc = ScopedDC::FromWindow(nullptr);
        if (dc.valid()) {
            int dpi = GetDeviceCaps(dc.get(), LOGPIXELSY);
            if (dpi > 0) return static_cast<UINT>(dpi);
        }
    } catch (...) {}
    return 96;
}

// Get localized UI strings for footer across 27 languages
static void GetLocalizedFooterStrings(std::wstring& outPreferences, std::wstring& outHint, std::wstring* outShowBar = nullptr) {
    try {
        std::wstring targetTag = g_uiLanguage;
        if (targetTag.empty() || targetTag == L"auto") {
            wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {};
            if (GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName)) > 0) {
                targetTag = localeName;
            } else {
                targetTag = L"en";
            }
        }

        for (const auto& item : kLocalizedStrings) {
            if (item.langTag && wcsncmp(targetTag.c_str(), item.langTag, wcslen(item.langTag)) == 0) {
                outPreferences = item.preferences ? item.preferences : L"Language preferences";
                outHint = item.shortcutHint ? item.shortcutHint : L"To switch, press Windows key + Space";
                if (outShowBar) *outShowBar = item.showLanguageBar ? item.showLanguageBar : L"Show the Language bar";
                return;
            }
        }
    } catch (...) {}
    outPreferences = L"Language preferences";
    outHint = L"To switch, press Windows key + Space";
    if (outShowBar) *outShowBar = L"Show the Language bar";
}

// =============================================================================
// Keyboard Layout Query & MUI Resolution
// =============================================================================

static std::wstring GetLangAbbrev(LANGID langId) {
    WORD priLang = PRIMARYLANGID(langId);
    for (const auto& item : g_LangAbbrevs) {
        if (item.langId == priLang) {
            return item.abbrev;
        }
    }
    wchar_t abbrBuf[16] = {};
    if (GetLocaleInfoW(langId, LOCALE_SABBREVLANGNAME, abbrBuf, ARRAYSIZE(abbrBuf)) > 0) {
        for (wchar_t* p = abbrBuf; *p; ++p) *p = towupper(*p);
        return abbrBuf;
    }
    return L"ENG";
}

static std::wstring GetLayoutDisplayName(const std::wstring& klid) {
    try {
        if (klid.empty()) return L"";
        std::wstring regPath = L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\" + klid;
        ScopedRegKey key;
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            key.reset(hKey);

            wchar_t displayBuf[512] = {};
            DWORD cbData = sizeof(displayBuf);
            if (RegQueryValueExW(key.get(), L"Layout Display Name", nullptr, nullptr,
                                 reinterpret_cast<LPBYTE>(displayBuf), &cbData) == ERROR_SUCCESS && displayBuf[0]) {
                wchar_t resolvedBuf[512] = {};
                if (SUCCEEDED(SHLoadIndirectString(displayBuf, resolvedBuf, ARRAYSIZE(resolvedBuf), nullptr)) && resolvedBuf[0]) {
                    return resolvedBuf;
                }
            }

            cbData = sizeof(displayBuf);
            if (RegQueryValueExW(key.get(), L"Layout Text", nullptr, nullptr,
                                 reinterpret_cast<LPBYTE>(displayBuf), &cbData) == ERROR_SUCCESS && displayBuf[0]) {
                return displayBuf;
            }
        }
    } catch (...) {}
    return L"";
}

static std::wstring GetSubstituteKlid(const std::wstring& klid) {
    try {
        if (klid.empty()) return klid;
        ScopedRegKey key;
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Keyboard Layout\\Substitutes", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            key.reset(hKey);
            wchar_t subBuf[64] = {};
            DWORD cb = sizeof(subBuf);
            if (RegQueryValueExW(key.get(), klid.c_str(), nullptr, nullptr,
                                 reinterpret_cast<LPBYTE>(subBuf), &cb) == ERROR_SUCCESS && subBuf[0]) {
                return subBuf;
            }
        }
    } catch (...) {}
    return klid;
}

static void RefreshKeyboardLayouts() {
    try {
        UINT count = GetKeyboardLayoutList(0, nullptr);
        if (count == 0 || count > 64) {
            Wh_Log(L"Win78LangSwitcher: Invalid or empty keyboard layouts count (%u)", count);
            return;
        }

        std::vector<HKL> hkls(count);
        UINT fetched = GetKeyboardLayoutList(count, hkls.data());
        if (fetched == 0) return;
        hkls.resize(fetched);

        HWND hFore = g_targetWindow ? g_targetWindow : GetForegroundWindow();
        if (hFore == g_hFlyoutWnd) hFore = g_targetWindow;
        if (hFore && !IsWindow(hFore)) hFore = nullptr;

        DWORD dwTid = (hFore ? GetWindowThreadProcessId(hFore, nullptr) : GetCurrentThreadId());
        HKL activeHkl = GetKeyboardLayout(dwTid);

        std::vector<KeyboardLayoutItem> newLayouts;
        newLayouts.reserve(hkls.size());
        size_t foundActiveIndex = 0;

        for (size_t i = 0; i < hkls.size(); ++i) {
            HKL hkl = hkls[i];
            if (!hkl) continue;

            KeyboardLayoutItem item;
            item.hkl = hkl;
            item.langId = LOWORD(reinterpret_cast<uintptr_t>(hkl));
            item.isCurrent = (hkl == activeHkl);

            if (item.isCurrent) {
                foundActiveIndex = i;
            }

            wchar_t klidBuf[16] = {};
            DWORD devId = static_cast<DWORD>(HIWORD(reinterpret_cast<uintptr_t>(hkl)));
            if ((devId & 0xF000) == 0xF000) {
                wsprintfW(klidBuf, L"%08X", devId);
            } else {
                wsprintfW(klidBuf, L"%08X", static_cast<UINT>(item.langId));
            }
            item.klid = klidBuf;

            std::wstring effectiveKlid = GetSubstituteKlid(item.klid);

            wchar_t langNameBuf[256] = {};
            if (GetLocaleInfoW(item.langId, LOCALE_SLOCALIZEDDISPLAYNAME, langNameBuf, ARRAYSIZE(langNameBuf)) > 0) {
                item.langName = langNameBuf;
            } else if (GetLocaleInfoW(item.langId, LOCALE_SENGLISHDISPLAYNAME, langNameBuf, ARRAYSIZE(langNameBuf)) > 0) {
                item.langName = langNameBuf;
            } else {
                item.langName = L"Language";
            }

            if (!item.langName.empty() && iswlower(item.langName[0])) {
                item.langName[0] = towupper(item.langName[0]);
            }

            item.langAbbrev = GetLangAbbrev(item.langId);

            std::wstring layoutDesc = GetLayoutDisplayName(effectiveKlid);
            if (layoutDesc.empty()) {
                layoutDesc = GetLayoutDisplayName(item.klid);
            }
            if (layoutDesc.empty()) {
                layoutDesc = item.langName;
            }
            item.layoutName = layoutDesc;

            newLayouts.push_back(std::move(item));
        }

        for (size_t i = 0; i < newLayouts.size(); ++i) {
            int sameLangCount = 0;
            for (size_t j = 0; j < newLayouts.size(); ++j) {
                if (newLayouts[i].langAbbrev == newLayouts[j].langAbbrev) {
                    sameLangCount++;
                }
            }
            if (sameLangCount > 1) {
                std::wstring upperLayout = newLayouts[i].layoutName;
                for (auto& c : upperLayout) c = towupper(c);
                
                if (upperLayout.find(L"INTERNATIONAL") != std::wstring::npos || upperLayout.find(L"INTL") != std::wstring::npos) {
                    newLayouts[i].subAbbrev = L"INTL";
                } else if (upperLayout.find(L"UNITED STATES") != std::wstring::npos || upperLayout.find(L"US") != std::wstring::npos) {
                    newLayouts[i].subAbbrev = L"US";
                } else if (upperLayout.find(L"DVORAK") != std::wstring::npos) {
                    newLayouts[i].subAbbrev = L"DV";
                } else if (upperLayout.find(L"UNITED KINGDOM") != std::wstring::npos || upperLayout.find(L"UK") != std::wstring::npos) {
                    newLayouts[i].subAbbrev = L"UK";
                } else {
                    std::wstring tag;
                    bool newWord = true;
                    for (wchar_t ch : newLayouts[i].layoutName) {
                        if (iswalpha(ch)) {
                            if (newWord && tag.size() < 4) {
                                tag.push_back(towupper(ch));
                                newWord = false;
                            }
                        } else {
                            newWord = true;
                        }
                    }
                    newLayouts[i].subAbbrev = tag.empty() ? L"1" : tag;
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            g_layouts = std::move(newLayouts);
            g_selectedIndex = (foundActiveIndex < g_layouts.size()) ? foundActiveIndex : 0;
        }
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in RefreshKeyboardLayouts: %hs", e.what());
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in RefreshKeyboardLayouts");
    }
}

static void SwitchToLayout(size_t index) {
    HKL targetHkl = nullptr;
    HWND hTarget = nullptr;
    std::wstring logLang, logLayout;

    try {
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            if (index >= g_layouts.size()) return;
            targetHkl = g_layouts[index].hkl;
            logLang = g_layouts[index].langName;
            logLayout = g_layouts[index].layoutName;
            g_selectedIndex = index;
            for (size_t i = 0; i < g_layouts.size(); ++i) {
                g_layouts[i].isCurrent = (i == index);
            }
        }

        if (!targetHkl) return;

        // The layout list snapshot above may be stale by the time we act on it
        // (e.g. the user uninstalled a keyboard layout between opening the
        // switcher and clicking it). Re-check against the live system list so
        // we never hand a dangling HKL to ActivateKeyboardLayout/PostMessage.
        {
            UINT count = GetKeyboardLayoutList(0, nullptr);
            bool stillValid = false;
            if (count > 0 && count < 64) {
                std::vector<HKL> hkls(count);
                UINT fetched = GetKeyboardLayoutList(count, hkls.data());
                for (UINT i = 0; i < fetched; ++i) {
                    if (hkls[i] == targetHkl) {
                        stillValid = true;
                        break;
                    }
                }
            }
            if (!stillValid) {
                Wh_Log(L"Win78LangSwitcher: Layout [%s - %s] is no longer installed, skipping switch",
                       logLang.c_str(), logLayout.c_str());
                return;
            }
        }

        hTarget = g_targetWindow ? g_targetWindow : GetForegroundWindow();
        if (hTarget == g_hFlyoutWnd) hTarget = nullptr;

        Wh_Log(L"Win78LangSwitcher: Switching to layout [%s - %s]", logLang.c_str(), logLayout.c_str());

        if (hTarget && IsWindow(hTarget)) {
            PostMessageW(hTarget, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(targetHkl));
        }

        ActivateKeyboardLayout(targetHkl, KLF_SETFORPROCESS);
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in SwitchToLayout: %hs", e.what());
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in SwitchToLayout");
    }
}

// Forward declarations
static void HideFlyout();
static void ShowSwitcher(HWND hAnchorWnd = nullptr, POINT* ptAnchor = nullptr);
static void TriggerSwitcher(HWND hAnchorWnd = nullptr, POINT* ptAnchor = nullptr);

// =============================================================================
// Unified Custom Window Renderer (Supports Win8.1 Modern Flyout & Win7 Classic Menu)
// =============================================================================

static void PaintWin8Flyout(HWND hwnd, HDC hdc) {
    RECT clientRect{};
    if (!GetClientRect(hwnd, &clientRect)) return;

    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    if (width <= 0 || height <= 0) return;

    UINT dpi = GetWindowDpi(hwnd);
    const int itemHeight = ScaleForDpi(58, dpi);
    const int badgeWidth = ScaleForDpi(62, dpi);
    const int paddingX = ScaleForDpi(16, dpi);
    const int separatorHeight = ScaleForDpi(1, dpi);

    ScopedDC memDC = ScopedDC::CreateCompatible(hdc);
    if (!memDC.valid()) return;

    ScopedBitmap memBmp(CreateCompatibleBitmap(hdc, width, height));
    if (!memBmp.valid()) return;
    ScopedSelectObject selBmp(memDC.get(), memBmp.get());

    SetBkMode(memDC.get(), TRANSPARENT);

    bool isDark = (g_themeMode == L"dark") || (g_themeMode == L"auto" && IsDarkModeActive());
    
    COLORREF colBg = isDark ? RGB(32, 32, 32) : RGB(242, 242, 242);
    COLORREF colTextNormal = isDark ? RGB(240, 240, 240) : RGB(0, 0, 0);
    // Native Windows input-switch flyout renders the secondary line (keyboard
    // layout name) and the footer hint in a subdued medium gray, not near-black.
    // Matching that here is purely a color tweak -- no layout/logic change.
    COLORREF colTextSub = isDark ? RGB(160, 160, 160) : RGB(96, 96, 96);
    COLORREF colHoverBg = isDark ? RGB(50, 50, 50) : RGB(220, 220, 220);
    COLORREF colBorder = isDark ? RGB(60, 60, 60) : RGB(190, 190, 190);
    COLORREF colSeparator = isDark ? RGB(55, 55, 55) : RGB(200, 200, 200);

    COLORREF colSelectedBg = RGB(91, 44, 130);
    if (g_themeMode == L"auto") {
        colSelectedBg = GetSystemAccentColor();
    } else if (g_themeMode == L"custom") {
        colSelectedBg = g_customAccentColor;
    }
    COLORREF colSelectedText = RGB(255, 255, 255);
    COLORREF colSelectedSubText = RGB(235, 235, 235);

    COLORREF colLink = (g_themeMode == L"win8_purple") ? RGB(91, 44, 130) : 
                       (isDark ? RGB(100, 160, 255) : RGB(0, 102, 204));
    // Native flyout renders "For easy switching, press..." in the same
    // subdued gray as the secondary line above, not solid black.
    COLORREF colTipText = isDark ? RGB(160, 160, 160) : RGB(96, 96, 96);

    ScopedBrush bgBrush(CreateSolidBrush(colBg));
    FillRect(memDC.get(), &clientRect, bgBrush.valid() ? bgBrush.get() : reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

    ScopedFont fontAbbr(CreateFontW(
        ScaleForDpi(18, dpi), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    ));
    ScopedFont fontSubAbbr(CreateFontW(
        ScaleForDpi(11, dpi), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    ));
    ScopedFont fontTitle(CreateFontW(
        ScaleForDpi(14, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    ));
    ScopedFont fontSub(CreateFontW(
        ScaleForDpi(12, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    ));
    ScopedFont fontLink(CreateFontW(
        ScaleForDpi(13, dpi), 0, 0, 0, FW_NORMAL, FALSE, g_hoveredFooter ? TRUE : FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    ));
    ScopedFont fontHint(CreateFontW(
        ScaleForDpi(11, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    ));

    std::vector<KeyboardLayoutItem> layoutsCopy;
    size_t selIndex = 0;
    int hovIndex = -1;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        layoutsCopy = g_layouts;
        selIndex = g_selectedIndex;
        hovIndex = g_hoveredIndex;
    }

    int currentY = 0;
    for (size_t i = 0; i < layoutsCopy.size(); ++i) {
        RECT itemRect{0, currentY, width, currentY + itemHeight};
        bool isSelected = (i == selIndex);
        bool isHovered = (static_cast<int>(i) == hovIndex);

        if (isSelected) {
            ScopedBrush selBrush(CreateSolidBrush(colSelectedBg));
            FillRect(memDC.get(), &itemRect, selBrush.valid() ? selBrush.get() : reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
        } else if (isHovered) {
            ScopedBrush hovBrush(CreateSolidBrush(colHoverBg));
            FillRect(memDC.get(), &itemRect, hovBrush.valid() ? hovBrush.get() : reinterpret_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH)));
        }

        COLORREF itemTextColor = isSelected ? colSelectedText : colTextNormal;
        COLORREF itemSubTextColor = isSelected ? colSelectedSubText : colTextSub;

        if (layoutsCopy[i].subAbbrev.empty()) {
            ScopedSelectObject selFont(memDC.get(), fontAbbr.valid() ? fontAbbr.get() : GetStockObject(DEFAULT_GUI_FONT));
            SetTextColor(memDC.get(), itemTextColor);
            RECT abbrRect{paddingX, currentY, badgeWidth + paddingX, currentY + itemHeight};
            DrawTextW(memDC.get(), layoutsCopy[i].langAbbrev.c_str(), -1, &abbrRect,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        } else {
            ScopedSelectObject selFont(memDC.get(), fontAbbr.valid() ? fontAbbr.get() : GetStockObject(DEFAULT_GUI_FONT));
            SetTextColor(memDC.get(), itemTextColor);
            RECT abbrRect1{paddingX, currentY + ScaleForDpi(6, dpi), badgeWidth + paddingX, currentY + ScaleForDpi(30, dpi)};
            DrawTextW(memDC.get(), layoutsCopy[i].langAbbrev.c_str(), -1, &abbrRect1,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

            ScopedSelectObject selSubFont(memDC.get(), fontSubAbbr.valid() ? fontSubAbbr.get() : GetStockObject(DEFAULT_GUI_FONT));
            RECT abbrRect2{paddingX, currentY + ScaleForDpi(28, dpi), badgeWidth + paddingX, currentY + ScaleForDpi(48, dpi)};
            DrawTextW(memDC.get(), layoutsCopy[i].subAbbrev.c_str(), -1, &abbrRect2,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
        }

        int rightX = badgeWidth + paddingX + ScaleForDpi(6, dpi);
        int textRight = width - paddingX;

        {
            ScopedSelectObject selFont(memDC.get(), fontTitle.valid() ? fontTitle.get() : GetStockObject(DEFAULT_GUI_FONT));
            SetTextColor(memDC.get(), itemTextColor);
            RECT textRect1{rightX, currentY + ScaleForDpi(8, dpi), textRight, currentY + ScaleForDpi(30, dpi)};
            DrawTextW(memDC.get(), layoutsCopy[i].langName.c_str(), -1, &textRect1,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
        }

        {
            ScopedSelectObject selFont(memDC.get(), fontSub.valid() ? fontSub.get() : GetStockObject(DEFAULT_GUI_FONT));
            SetTextColor(memDC.get(), itemSubTextColor);
            RECT textRect2{rightX, currentY + ScaleForDpi(30, dpi), textRight, currentY + ScaleForDpi(50, dpi)};
            DrawTextW(memDC.get(), layoutsCopy[i].layoutName.c_str(), -1, &textRect2,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
        }

        currentY += itemHeight;
    }

    RECT sepRect{paddingX, currentY + ScaleForDpi(4, dpi), width - paddingX, currentY + ScaleForDpi(4, dpi) + separatorHeight};
    ScopedBrush sepBrush(CreateSolidBrush(colSeparator));
    FillRect(memDC.get(), &sepRect, sepBrush.valid() ? sepBrush.get() : reinterpret_cast<HBRUSH>(GetStockObject(GRAY_BRUSH)));
    currentY += ScaleForDpi(8, dpi);

    std::wstring prefStr, hintStr;
    GetLocalizedFooterStrings(prefStr, hintStr);

    {
        ScopedSelectObject selFont(memDC.get(), fontLink.valid() ? fontLink.get() : GetStockObject(DEFAULT_GUI_FONT));
        SetTextColor(memDC.get(), colLink);
        RECT linkRect{paddingX, currentY, width - paddingX, currentY + ScaleForDpi(22, dpi)};
        DrawTextW(memDC.get(), prefStr.c_str(), -1, &linkRect,
                  DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
    }

    if (g_showShortcutHint) {
        ScopedSelectObject selFont(memDC.get(), fontHint.valid() ? fontHint.get() : GetStockObject(DEFAULT_GUI_FONT));
        SetTextColor(memDC.get(), colTipText);
        RECT tipRect{paddingX, currentY + ScaleForDpi(22, dpi), width - paddingX, currentY + ScaleForDpi(44, dpi)};
        DrawTextW(memDC.get(), hintStr.c_str(), -1, &tipRect,
                  DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
    }

    ScopedPen borderPen(CreatePen(PS_SOLID, 1, colBorder));
    ScopedSelectObject selPen(memDC.get(), borderPen.valid() ? borderPen.get() : GetStockObject(BLACK_PEN));
    ScopedSelectObject selNullBrush(memDC.get(), GetStockObject(NULL_BRUSH));
    Rectangle(memDC.get(), 0, 0, width, height);

    BitBlt(hdc, 0, 0, width, height, memDC.get(), 0, 0, SRCCOPY);
}

// GDI+ startup token. Needed so the Win7 checkmark can be drawn with
// anti-aliased filled paths instead of aliased GDI LineTo strokes.
// Classic Windows 7 language-bar checkmark: a filled, anti-aliased ✓
// matching the MENU_POPUPCHECK glyph. GDI+ objects (Graphics / Path /
// Brush / Pen) are stack-RAII; startup/shutdown lives in g_gdiplus.
// This glyph is Win7-classic only -- the Win8.1 flyout never calls it.
static void DrawWin7GdiFallbackCheck(HDC hdc, const RECT& gutter, COLORREF color, UINT dpi) {
    try {
        if (!hdc) return;
        int thickness = ScaleForDpi(2, dpi);
        if (thickness < 2) thickness = 2;
        LOGBRUSH lb{};
        lb.lbStyle = BS_SOLID;
        lb.lbColor = color;
        ScopedPen pen(ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_ROUND | PS_JOIN_MITER,
                                   thickness, &lb, 0, nullptr));
        if (!pen.valid()) return;
        ScopedSelectObject selPen(hdc, pen.get());
        const int cx = (gutter.left + gutter.right) / 2;
        const int cy = (gutter.top + gutter.bottom) / 2 + ScaleForDpi(1, dpi);
        MoveToEx(hdc, cx - ScaleForDpi(5, dpi), cy, nullptr);
        LineTo(hdc, cx - ScaleForDpi(1, dpi), cy + ScaleForDpi(4, dpi));
        LineTo(hdc, cx + ScaleForDpi(6, dpi), cy - ScaleForDpi(5, dpi));
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in DrawWin7GdiFallbackCheck: %hs", e.what());
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in DrawWin7GdiFallbackCheck");
    }
}

static void DrawWin7MenuCheckmark(HDC hdc, const RECT& gutter, COLORREF color, UINT dpi) {
    try {
        if (!hdc) return;

        // Hard gate: the checkmark exists only on the Windows 7 classic menu.
        // The Win8.1 flyout highlights the active row with the accent fill.
        if (g_switcherStyle != L"win7") {
            return;
        }

        const BYTE r = GetRValue(color);
        const BYTE gch = GetGValue(color);
        const BYTE b = GetBValue(color);

        if (!g_gdiplus.ready() && !g_gdiplus.startup()) {
            DrawWin7GdiFallbackCheck(hdc, gutter, color, dpi);
            return;
        }

        try {
            Gdiplus::Graphics graphics(hdc);
            if (graphics.GetLastStatus() != Gdiplus::Ok) {
                DrawWin7GdiFallbackCheck(hdc, gutter, color, dpi);
                return;
            }

            graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
            graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
            graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

            const float boxL = static_cast<float>(gutter.left);
            const float boxT = static_cast<float>(gutter.top);
            const float boxW = static_cast<float>(gutter.right - gutter.left);
            const float boxH = static_cast<float>(gutter.bottom - gutter.top);
            if (boxW < 4.0f || boxH < 4.0f) return;

            float size = boxW * 0.70f;
            if (size > boxH * 0.50f) size = boxH * 0.50f;
            if (size < 8.0f) size = (boxH < boxW ? boxH : boxW) * 0.48f;

            const float originX = boxL + (boxW - size) * 0.42f;
            const float originY = boxT + (boxH - size) * 0.54f;

            const Gdiplus::PointF pts[] = {
                { originX + size * 0.06f, originY + size * 0.50f },
                { originX + size * 0.18f, originY + size * 0.38f },
                { originX + size * 0.38f, originY + size * 0.62f },
                { originX + size * 0.82f, originY + size * 0.08f },
                { originX + size * 0.96f, originY + size * 0.20f },
                { originX + size * 0.38f, originY + size * 0.90f },
            };

            Gdiplus::GraphicsPath path;
            if (path.GetLastStatus() != Gdiplus::Ok ||
                path.AddPolygon(pts, 6) != Gdiplus::Ok) {
                DrawWin7GdiFallbackCheck(hdc, gutter, color, dpi);
                return;
            }

            const Gdiplus::Color fillColor(255, r, gch, b);
            Gdiplus::SolidBrush brush(fillColor);
            if (brush.GetLastStatus() != Gdiplus::Ok ||
                graphics.FillPath(&brush, &path) != Gdiplus::Ok) {
                DrawWin7GdiFallbackCheck(hdc, gutter, color, dpi);
                return;
            }

            const float outlineW = (dpi >= 144) ? 0.90f * (static_cast<float>(dpi) / 96.0f) : 0.70f;
            Gdiplus::Pen outline(fillColor, outlineW);
            if (outline.GetLastStatus() == Gdiplus::Ok) {
                outline.SetLineJoin(Gdiplus::LineJoinRound);
                outline.SetStartCap(Gdiplus::LineCapRound);
                outline.SetEndCap(Gdiplus::LineCapRound);
                graphics.DrawPath(&outline, &path);
            }
        } catch (const std::exception& e) {
            Wh_Log(L"Win78LangSwitcher: std::exception in DrawWin7MenuCheckmark(GDI+): %hs", e.what());
            DrawWin7GdiFallbackCheck(hdc, gutter, color, dpi);
        } catch (...) {
            Wh_Log(L"Win78LangSwitcher: Unknown exception in DrawWin7MenuCheckmark(GDI+)");
            DrawWin7GdiFallbackCheck(hdc, gutter, color, dpi);
        }
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in DrawWin7MenuCheckmark: %hs", e.what());
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in DrawWin7MenuCheckmark");
    }
}

static void PaintWin7Menu(HWND hwnd, HDC hdc) {
    try {
    RECT clientRect{};
    if (!GetClientRect(hwnd, &clientRect)) return;

    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    if (width <= 0 || height <= 0) return;

    UINT dpi = GetWindowDpi(hwnd);
    const int itemHeight = ScaleForDpi(26, dpi);
    const int paddingLeft = ScaleForDpi(28, dpi);
    const int paddingRight = ScaleForDpi(16, dpi);

    ScopedDC memDC = ScopedDC::CreateCompatible(hdc);
    if (!memDC.valid()) return;

    ScopedBitmap memBmp(CreateCompatibleBitmap(hdc, width, height));
    if (!memBmp.valid()) return;
    ScopedSelectObject selBmp(memDC.get(), memBmp.get());

    SetBkMode(memDC.get(), TRANSPARENT);

    bool isDark = (g_themeMode == L"dark") || (g_themeMode == L"auto" && IsDarkModeActive());
    
    COLORREF colBg = isDark ? RGB(36, 36, 36) : RGB(242, 242, 242);
    COLORREF colTextNormal = isDark ? RGB(245, 245, 245) : RGB(0, 0, 0);
    COLORREF colHoverBg = isDark ? RGB(56, 56, 56) : RGB(185, 215, 251);
    COLORREF colHoverBorder = isDark ? RGB(80, 80, 80) : RGB(125, 162, 206);
    COLORREF colBorder = isDark ? RGB(70, 70, 70) : RGB(160, 160, 160);
    COLORREF colSeparator = isDark ? RGB(55, 55, 55) : RGB(210, 210, 210);

    ScopedBrush bgBrush(CreateSolidBrush(colBg));
    FillRect(memDC.get(), &clientRect, bgBrush.valid() ? bgBrush.get() : reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

    ScopedFont fontMenu(CreateFontW(
        ScaleForDpi(13, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    ));

    std::vector<KeyboardLayoutItem> layoutsCopy;
    size_t activeIdx = 0;
    int hovIdx = -1;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        layoutsCopy = g_layouts;
        activeIdx = g_selectedIndex;
        hovIdx = g_hoveredWin7Index;
    }

    int currentY = ScaleForDpi(3, dpi);

    for (size_t i = 0; i < layoutsCopy.size(); ++i) {
        RECT itemRect{ScaleForDpi(2, dpi), currentY, width - ScaleForDpi(2, dpi), currentY + itemHeight};
        bool isHovered = (static_cast<int>(i) == hovIdx);
        bool isActive = (i == activeIdx);

        if (isHovered) {
            ScopedBrush hovBrush(CreateSolidBrush(colHoverBg));
            FillRect(memDC.get(), &itemRect, hovBrush.get());
            ScopedPen hovPen(CreatePen(PS_SOLID, 1, colHoverBorder));
            ScopedSelectObject selPen(memDC.get(), hovPen.get());
            ScopedSelectObject selNull(memDC.get(), GetStockObject(NULL_BRUSH));
            Rectangle(memDC.get(), itemRect.left, itemRect.top, itemRect.right, itemRect.bottom);
        }

        if (isActive && g_switcherStyle == L"win7") {
            try {
                RECT checkRect{ScaleForDpi(4, dpi), currentY, paddingLeft, currentY + itemHeight};
                DrawWin7MenuCheckmark(memDC.get(), checkRect, colTextNormal, dpi);
            } catch (const std::exception& e) {
                Wh_Log(L"Win78LangSwitcher: std::exception drawing Win7 checkmark: %hs", e.what());
            } catch (...) {
                Wh_Log(L"Win78LangSwitcher: Unknown exception drawing Win7 checkmark");
            }
        }

        std::wstring itemText = layoutsCopy[i].langAbbrev + L"  " + layoutsCopy[i].langName;
        if (!layoutsCopy[i].layoutName.empty() && layoutsCopy[i].layoutName != layoutsCopy[i].langName) {
            itemText += L" (" + layoutsCopy[i].layoutName + L")";
        }

        ScopedSelectObject selF(memDC.get(), fontMenu.get());
        SetTextColor(memDC.get(), colTextNormal);
        RECT textRect{paddingLeft, currentY, width - paddingRight, currentY + itemHeight};
        DrawTextW(memDC.get(), itemText.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

        currentY += itemHeight;
    }

    currentY += ScaleForDpi(2, dpi);
    RECT sepRect{ScaleForDpi(6, dpi), currentY + ScaleForDpi(3, dpi), width - ScaleForDpi(6, dpi), currentY + ScaleForDpi(4, dpi)};
    ScopedBrush sepBrush(CreateSolidBrush(colSeparator));
    FillRect(memDC.get(), &sepRect, sepBrush.get());
    currentY += ScaleForDpi(7, dpi);

    std::wstring prefStr, hintStr, showBarStr;
    GetLocalizedFooterStrings(prefStr, hintStr, &showBarStr);

    int footer1Index = static_cast<int>(layoutsCopy.size());
    int footer2Index = footer1Index + 1;

    // Option 1: Show the Language bar
    {
        RECT itemRect{ScaleForDpi(2, dpi), currentY, width - ScaleForDpi(2, dpi), currentY + itemHeight};
        if (hovIdx == footer1Index) {
            ScopedBrush hovBrush(CreateSolidBrush(colHoverBg));
            FillRect(memDC.get(), &itemRect, hovBrush.get());
            ScopedPen hovPen(CreatePen(PS_SOLID, 1, colHoverBorder));
            ScopedSelectObject selPen(memDC.get(), hovPen.get());
            ScopedSelectObject selNull(memDC.get(), GetStockObject(NULL_BRUSH));
            Rectangle(memDC.get(), itemRect.left, itemRect.top, itemRect.right, itemRect.bottom);
        }
        ScopedSelectObject selF(memDC.get(), fontMenu.get());
        SetTextColor(memDC.get(), colTextNormal);
        RECT textRect{paddingLeft, currentY, width - paddingRight, currentY + itemHeight};
        DrawTextW(memDC.get(), showBarStr.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        currentY += itemHeight;
    }

    // Option 2: Language preferences...
    {
        RECT itemRect{ScaleForDpi(2, dpi), currentY, width - ScaleForDpi(2, dpi), currentY + itemHeight};
        if (hovIdx == footer2Index) {
            ScopedBrush hovBrush(CreateSolidBrush(colHoverBg));
            FillRect(memDC.get(), &itemRect, hovBrush.get());
            ScopedPen hovPen(CreatePen(PS_SOLID, 1, colHoverBorder));
            ScopedSelectObject selPen(memDC.get(), hovPen.get());
            ScopedSelectObject selNull(memDC.get(), GetStockObject(NULL_BRUSH));
            Rectangle(memDC.get(), itemRect.left, itemRect.top, itemRect.right, itemRect.bottom);
        }
        ScopedSelectObject selF(memDC.get(), fontMenu.get());
        SetTextColor(memDC.get(), colTextNormal);
        RECT textRect{paddingLeft, currentY, width - paddingRight, currentY + itemHeight};
        std::wstring prefWithDots = prefStr + L"...";
        DrawTextW(memDC.get(), prefWithDots.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }

    ScopedPen borderPen(CreatePen(PS_SOLID, 1, colBorder));
    ScopedSelectObject selPen(memDC.get(), borderPen.valid() ? borderPen.get() : GetStockObject(BLACK_PEN));
    ScopedSelectObject selNullBrush(memDC.get(), GetStockObject(NULL_BRUSH));
    Rectangle(memDC.get(), 0, 0, width, height);

    BitBlt(hdc, 0, 0, width, height, memDC.get(), 0, 0, SRCCOPY);
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in PaintWin7Menu: %hs", e.what());
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in PaintWin7Menu");
    }
}

static void PaintSwitcher(HWND hwnd, HDC hdc) {
    static thread_local bool s_inPaint = false;
    if (s_inPaint || !hwnd || !hdc) return;
    s_inPaint = true;

    try {
        if (g_switcherStyle == L"win7") {
            PaintWin7Menu(hwnd, hdc);
        } else {
            PaintWin8Flyout(hwnd, hdc);
        }
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in PaintSwitcher: %hs", e.what());
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in PaintSwitcher");
    }

    s_inPaint = false;
}

static LRESULT CALLBACK FlyoutWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    try {
        switch (uMsg) {
        case WM_PAINT: {
            ScopedPaintDC dc(hwnd);
            if (dc.valid()) {
                PaintSwitcher(hwnd, dc.get());
            }
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            UINT dpi = GetWindowDpi(hwnd);

            size_t count = 0;
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                count = g_layouts.size();
            }

            bool needsRepaint = false;

            if (g_switcherStyle == L"win7") {
                const int itemHeight = ScaleForDpi(26, dpi);
                const int sepHeight = ScaleForDpi(9, dpi);
                int newHov = -1;

                if (y >= ScaleForDpi(3, dpi) && y < ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight) {
                    newHov = (y - ScaleForDpi(3, dpi)) / itemHeight;
                } else if (y >= ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight + sepHeight) {
                    int footerY = y - (ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight + sepHeight);
                    int fIndex = footerY / itemHeight;
                    if (fIndex == 0) newHov = static_cast<int>(count);
                    else if (fIndex == 1) newHov = static_cast<int>(count) + 1;
                }

                {
                    std::lock_guard<std::mutex> lock(g_stateMutex);
                    if (newHov != g_hoveredWin7Index) {
                        g_hoveredWin7Index = newHov;
                        needsRepaint = true;
                    }
                }
            } else {
                const int itemHeight = ScaleForDpi(58, dpi);
                const int paddingX = ScaleForDpi(16, dpi);
                int newHoveredIndex = -1;
                bool newHoveredFooter = false;

                if (itemHeight > 0 && y >= 0 && y < static_cast<int>(count) * itemHeight) {
                    newHoveredIndex = y / itemHeight;
                    if (newHoveredIndex >= static_cast<int>(count)) {
                        newHoveredIndex = -1;
                    }
                } else if (y >= static_cast<int>(count) * itemHeight) {
                    int linkY = static_cast<int>(count) * itemHeight + ScaleForDpi(8, dpi);
                    if (y >= linkY && y <= linkY + ScaleForDpi(24, dpi) && x >= paddingX && x <= ScaleForDpi(220, dpi)) {
                        newHoveredFooter = true;
                    }
                }

                {
                    std::lock_guard<std::mutex> lock(g_stateMutex);
                    if (newHoveredIndex != g_hoveredIndex || newHoveredFooter != g_hoveredFooter) {
                        g_hoveredIndex = newHoveredIndex;
                        g_hoveredFooter = newHoveredFooter;
                        needsRepaint = true;
                    }
                }
            }

            if (needsRepaint && IsWindow(hwnd)) {
                InvalidateRect(hwnd, nullptr, FALSE);
            }

            TRACKMOUSEEVENT tme{};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE: {
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                g_hoveredIndex = -1;
                g_hoveredFooter = false;
                g_hoveredWin7Index = -1;
            }
            if (IsWindow(hwnd)) {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_SETCURSOR: {
            bool isFooter = false;
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                isFooter = g_hoveredFooter;
            }
            if (isFooter && g_switcherStyle != L"win7") {
                SetCursor(LoadCursor(nullptr, IDC_HAND));
                return TRUE;
            }
            break;
        }

        case WM_LBUTTONUP: {
            int y = GET_Y_LPARAM(lParam);
            int x = GET_X_LPARAM(lParam);
            UINT dpi = GetWindowDpi(hwnd);

            size_t count = 0;
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                count = g_layouts.size();
            }

            if (g_switcherStyle == L"win7") {
                const int itemHeight = ScaleForDpi(26, dpi);
                const int sepHeight = ScaleForDpi(9, dpi);

                if (y >= ScaleForDpi(3, dpi) && y < ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight) {
                    size_t clickedIndex = static_cast<size_t>((y - ScaleForDpi(3, dpi)) / itemHeight);
                    if (clickedIndex < count) {
                        SwitchToLayout(clickedIndex);
                    }
                    HideFlyout();
                    return 0;
                } else if (y >= ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight + sepHeight) {
                    int footerY = y - (ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight + sepHeight);
                    int fIndex = footerY / itemHeight;
                    HideFlyout();
                    if (fIndex == 0) {
                        ShellExecuteW(nullptr, L"open", L"control.exe", L"/name Microsoft.Language", nullptr, SW_SHOWNORMAL);
                    } else if (fIndex == 1) {
                        std::wstring cmd = g_customPreferencesCmd;
                        if (cmd.empty()) cmd = L"ms-settings:regionlanguage";
                        ShellExecuteW(nullptr, L"open", cmd.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                    }
                    return 0;
                }
            } else {
                int itemHeight = ScaleForDpi(58, dpi);
                int paddingX = ScaleForDpi(16, dpi);

                if (itemHeight > 0 && y >= 0 && y < static_cast<int>(count) * itemHeight) {
                    size_t clickedIndex = static_cast<size_t>(y / itemHeight);
                    if (clickedIndex < count) {
                        SwitchToLayout(clickedIndex);
                    }
                    HideFlyout();
                    return 0;
                } else if (y >= static_cast<int>(count) * itemHeight) {
                    int linkY = static_cast<int>(count) * itemHeight + ScaleForDpi(8, dpi);
                    if (y >= linkY && y <= linkY + ScaleForDpi(26, dpi) && x >= paddingX && x <= ScaleForDpi(250, dpi)) {
                        HideFlyout();
                        std::wstring cmd = g_customPreferencesCmd;
                        if (cmd.empty()) cmd = L"ms-settings:regionlanguage";
                        ShellExecuteW(nullptr, L"open", cmd.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                        return 0;
                    }
                }
            }
            break;
        }

        case WM_KEYDOWN: {
            size_t count = 0;
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                count = g_layouts.size();
            }
            if (count == 0) break;

            if (wParam == VK_ESCAPE) {
                HideFlyout();
                return 0;
            } else if (wParam == VK_DOWN || wParam == VK_TAB) {
                {
                    std::lock_guard<std::mutex> lock(g_stateMutex);
                    g_selectedIndex = (g_selectedIndex + 1) % count;
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            } else if (wParam == VK_UP) {
                {
                    std::lock_guard<std::mutex> lock(g_stateMutex);
                    g_selectedIndex = (g_selectedIndex + count - 1) % count;
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            } else if (wParam == VK_HOME) {
                {
                    std::lock_guard<std::mutex> lock(g_stateMutex);
                    g_selectedIndex = 0;
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            } else if (wParam == VK_END) {
                {
                    std::lock_guard<std::mutex> lock(g_stateMutex);
                    g_selectedIndex = count - 1;
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            } else if (wParam == VK_RETURN || wParam == VK_SPACE) {
                size_t sel = 0;
                {
                    std::lock_guard<std::mutex> lock(g_stateMutex);
                    sel = g_selectedIndex;
                }
                SwitchToLayout(sel);
                HideFlyout();
                return 0;
            }
            break;
        }

        case WM_ACTIVATE: {
            if (LOWORD(wParam) == WA_INACTIVE) {
                if (!g_isWinSpaceCycling) {
                    HideFlyout();
                }
            }
            break;
        }

        case WM_DESTROY:
            g_hFlyoutWnd = nullptr;
            return 0;
        }
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in FlyoutWndProc: %hs", e.what());
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in FlyoutWndProc");
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

// Register flyout window class (clean styles, no legacy GDI drop shadow artifacts)
static bool RegisterFlyoutClass() {
    try {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wc.lpfnWndProc = FlyoutWndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszClassName = kFlyoutClassName;
        
        RegisterClassExW(&wc);
        return true;
    } catch (...) {
        return false;
    }
}

// Unregister flyout window class
static void UnregisterFlyoutClass() {
    try {
        UnregisterClassW(kFlyoutClassName, GetModuleHandleW(nullptr));
    } catch (...) {}
}

static void HideFlyout() {
    try {
        g_isWinSpaceCycling = false;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            g_hoveredIndex = -1;
            g_hoveredFooter = false;
            g_hoveredWin7Index = -1;
        }
        if (g_hFlyoutWnd && IsWindow(g_hFlyoutWnd)) {
            ShowWindow(g_hFlyoutWnd, SW_HIDE);
            SetWindowPos(g_hFlyoutWnd, nullptr, -10000, -10000, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
        }
    } catch (...) {}
}

static void ShowSwitcher(HWND hAnchorWnd, POINT* ptAnchor) {
    (void)hAnchorWnd;
    try {
        RefreshKeyboardLayouts();

        size_t count = 0;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            count = g_layouts.size();
        }
        if (count == 0) return;

        if (!g_hFlyoutWnd || !IsWindow(g_hFlyoutWnd)) {
            g_hFlyoutWnd = CreateWindowExW(
                WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                kFlyoutClassName,
                L"Windows Language Switcher",
                WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                0, 0, 100, 100,
                nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
            );
        }

        if (!g_hFlyoutWnd || !IsWindow(g_hFlyoutWnd)) return;

        bool isDark = (g_themeMode == L"dark") || (g_themeMode == L"auto" && IsDarkModeActive());
        BOOL useDarkMode = isDark ? TRUE : FALSE;
        DwmSetWindowAttribute(g_hFlyoutWnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &useDarkMode, sizeof(useDarkMode));
        DwmSetWindowAttribute(g_hFlyoutWnd, 19 /* DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 */, &useDarkMode, sizeof(useDarkMode));
        DarkContextMenu::AllowDarkModeForWindow(g_hFlyoutWnd, isDark);
        DarkContextMenu::Apply(isDark ? TRUE : FALSE);

        enum { DWMWA_WINDOW_CORNER_PREFERENCE_LOCAL = 33, DWMWCP_ROUND_LOCAL = 2 };
        DWORD cornerPref = DWMWCP_ROUND_LOCAL;
        DwmSetWindowAttribute(g_hFlyoutWnd, DWMWA_WINDOW_CORNER_PREFERENCE_LOCAL, &cornerPref, sizeof(cornerPref));

        UINT dpi = GetWindowDpi(g_hFlyoutWnd);
        int flyoutWidth = 0;
        int totalHeight = 0;

        if (g_switcherStyle == L"win7") {
            int itemHeight = ScaleForDpi(26, dpi);
            int sepHeight = ScaleForDpi(9, dpi);
            int footerHeight = itemHeight * 2;
            totalHeight = ScaleForDpi(6, dpi) + static_cast<int>(count) * itemHeight + sepHeight + footerHeight;
            flyoutWidth = ScaleForDpi(270, dpi);
        } else {
            int itemHeight = ScaleForDpi(58, dpi);
            int footerHeight = ScaleForDpi(62, dpi);
            totalHeight = static_cast<int>(count) * itemHeight + footerHeight;
            flyoutWidth = ScaleForDpi(330, dpi);
        }

        POINT pt{};
        if (ptAnchor) {
            pt = *ptAnchor;
        } else {
            GetCursorPos(&pt);
        }

        HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        GetMonitorInfoW(hMon, &mi);
        RECT rcWork = mi.rcWork;

        APPBARDATA abd{};
        abd.cbSize = sizeof(abd);
        SHAppBarMessage(ABM_GETTASKBARPOS, &abd);

        if (totalHeight > (rcWork.bottom - rcWork.top)) {
            totalHeight = rcWork.bottom - rcWork.top;
        }

        int posX = rcWork.right - flyoutWidth - ScaleForDpi(8, dpi);
        int posY = rcWork.bottom - totalHeight - ScaleForDpi(8, dpi);

        if (abd.uEdge == ABE_TOP) {
            posY = abd.rc.bottom + ScaleForDpi(8, dpi);
        } else if (abd.uEdge == ABE_LEFT) {
            posX = abd.rc.right + ScaleForDpi(8, dpi);
        } else if (abd.uEdge == ABE_RIGHT) {
            posX = abd.rc.left - flyoutWidth - ScaleForDpi(8, dpi);
        }

        if (posX + flyoutWidth > rcWork.right - 2) posX = rcWork.right - flyoutWidth - 2;
        if (posX < rcWork.left + 2) posX = rcWork.left + 2;
        if (posY + totalHeight > rcWork.bottom - 2) posY = rcWork.bottom - totalHeight - 2;
        if (posY < rcWork.top + 2) posY = rcWork.top + 2;

        Wh_Log(L"Win78LangSwitcher: Displaying %s switcher at (%d, %d)", g_switcherStyle.c_str(), posX, posY);

        SetWindowPos(g_hFlyoutWnd, HWND_TOPMOST, posX, posY, flyoutWidth, totalHeight,
                     SWP_SHOWWINDOW | SWP_NOACTIVATE);

        InvalidateRect(g_hFlyoutWnd, nullptr, TRUE);
        UpdateWindow(g_hFlyoutWnd);
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in ShowSwitcher: %hs", e.what());
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in ShowSwitcher");
    }
}

static void TriggerSwitcher(HWND hAnchorWnd, POINT* ptAnchor) {
    DWORD now = GetTickCount();
    if (now - g_lastTriggerTick < 250) {
        return;
    }
    g_lastTriggerTick = now;

    ShowSwitcher(hAnchorWnd, ptAnchor);
}

// =============================================================================
// Taskbar Tray Input Indicator Subclass Procedures (Safe & Selective)
// =============================================================================

static LRESULT CALLBACK InputIndicatorButtonSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData
) {
    (void)dwRefData;
    try {
        if (uMsg == WM_NCDESTROY) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, InputIndicatorButtonSubclassProc);
            {
                std::lock_guard<std::mutex> lock(g_subclassedWindowsMutex);
                g_subclassedWindows.erase(hWnd);
            }
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        if (g_unloading.load(std::memory_order_acquire)) {
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        g_inFlightSubclasses.fetch_add(1, std::memory_order_acq_rel);
        struct InFlightGuard {
            ~InFlightGuard() { g_inFlightSubclasses.fetch_sub(1, std::memory_order_acq_rel); }
        } guard;

        if (g_hookTrayClicks && !g_shuttingDown.load(std::memory_order_acquire)) {
            if (uMsg == WM_LBUTTONDOWN) {
                g_flyoutWasVisibleOnDown = (g_hFlyoutWnd && IsWindow(g_hFlyoutWnd) && IsWindowVisible(g_hFlyoutWnd));
                return 0;
            } else if (uMsg == WM_LBUTTONUP) {
                if (g_flyoutWasVisibleOnDown) {
                    HideFlyout();
                } else {
                    g_targetWindow = GetForegroundWindow();
                    if (g_targetWindow == hWnd || g_targetWindow == g_hFlyoutWnd) {
                        g_targetWindow = nullptr;
                    }
                    TriggerSwitcher(hWnd, nullptr);
                }
                return 0;
            } else if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_MOUSEACTIVATE) {
                return 0;
            } else if (uMsg == WM_KEYDOWN && (wParam == VK_RETURN || wParam == VK_SPACE)) {
                g_targetWindow = GetForegroundWindow();
                TriggerSwitcher(hWnd, nullptr);
                return 0;
            }
        }
    } catch (...) {}

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Subclass a taskbar window safely (only dedicated input indicator windows)
static void SubclassTaskbarWindow(HWND hWnd) {
    try {
        if (!hWnd || !IsWindow(hWnd)) return;

        DWORD dwPid = 0;
        GetWindowThreadProcessId(hWnd, &dwPid);
        if (dwPid != GetCurrentProcessId()) return;

        wchar_t className[128] = {};
        if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) <= 0) return;

        std::lock_guard<std::mutex> lock(g_subclassedWindowsMutex);
        if (g_subclassedWindows.find(hWnd) != g_subclassedWindows.end()) return;

        if (wcsicmp(className, L"TrayInputIndicatorWClass") == 0 ||
            wcsicmp(className, L"InputIndicatorButton") == 0 ||
            wcsicmp(className, L"InputIndicator") == 0 ||
            wcsicmp(className, L"CiceroUIWndFrame") == 0 ||
            wcsicmp(className, L"TipBandNotificationArea") == 0) {
            
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, InputIndicatorButtonSubclassProc, 0)) {
                g_subclassedWindows.insert(hWnd);
                Wh_Log(L"Win78LangSwitcher: Subclassed Input Indicator window %s (0x%p)", className, hWnd);
            }
        }
    } catch (...) {}
}

static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam) {
    (void)lParam;
    SubclassTaskbarWindow(hWnd);
    return TRUE;
}

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    (void)lParam;
    wchar_t className[128] = {};
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) > 0) {
        if (wcsicmp(className, L"Shell_TrayWnd") == 0 ||
            wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
            SubclassTaskbarWindow(hWnd);
            EnumChildWindows(hWnd, EnumChildProc, 0);
        }
    }
    return TRUE;
}

static void EnumerateAndSubclassTaskbars() {
    try {
        EnumWindows(EnumWindowsProc, 0);
    } catch (...) {}
}

// =============================================================================
// Hooking Engine (Safe, Re-entrancy Protected ShowWindow & CreateWindowExW)
// =============================================================================

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t CreateWindowExW_Original = nullptr;

static HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam
) {
    static thread_local bool s_inCreateWindow = false;
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    if (s_inCreateWindow || g_shuttingDown.load(std::memory_order_acquire) || g_unloading.load(std::memory_order_acquire) ||
        (g_mainThreadId != 0 && GetCurrentThreadId() != g_mainThreadId)) {
        return hWnd;
    }
    s_inCreateWindow = true;

    try {
        if (hWnd && lpClassName && (((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0)) {
            if (wcsicmp(lpClassName, L"TrayInputIndicatorWClass") == 0 ||
                wcsicmp(lpClassName, L"InputIndicatorButton") == 0 ||
                wcsicmp(lpClassName, L"InputIndicator") == 0 ||
                wcsicmp(lpClassName, L"CiceroUIWndFrame") == 0 ||
                wcsicmp(lpClassName, L"TipBandNotificationArea") == 0) {
                SubclassTaskbarWindow(hWnd);
            }
        }
    } catch (...) {}

    s_inCreateWindow = false;
    return hWnd;
}

using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
static ShowWindow_t ShowWindow_Original = nullptr;

static BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    static thread_local bool s_inShowWindow = false;
    if (s_inShowWindow || g_shuttingDown.load(std::memory_order_acquire) || g_unloading.load(std::memory_order_acquire) ||
        (g_mainThreadId != 0 && GetCurrentThreadId() != g_mainThreadId)) {
        return ShowWindow_Original ? ShowWindow_Original(hWnd, nCmdShow) : ShowWindow(hWnd, nCmdShow);
    }
    s_inShowWindow = true;

    try {
        if (g_hookTrayClicks && hWnd && IsWindow(hWnd) && nCmdShow != SW_HIDE) {
            wchar_t className[128] = {};
            if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) > 0) {
                if (wcsicmp(className, L"Shell_InputSwitchTopLevelWindow") == 0 ||
                    wcsicmp(className, L"Shell_InputSwitchDismissOverlay") == 0 ||
                    wcsicmp(className, L"Shell_InputSwitch") == 0 ||
                    wcsicmp(className, L"InputSwitch") == 0) {
                    
                    Wh_Log(L"Win78LangSwitcher: Intercepted modern %s show -> Displaying %s switcher",
                           className, g_switcherStyle.c_str());
                    TriggerSwitcher(nullptr, nullptr);
                    s_inShowWindow = false;
                    return TRUE;
                }
            }
        }
    } catch (...) {}

    s_inShowWindow = false;
    return ShowWindow_Original(hWnd, nCmdShow);
}

// =============================================================================
// Keyboard & Mouse Hooks (Win+Space, Alt+Shift, Escape & Outside Clicks)
// =============================================================================

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || !lParam || g_shuttingDown.load(std::memory_order_acquire)) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    static thread_local bool s_inKbdHook = false;
    if (s_inKbdHook) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    s_inKbdHook = true;

    try {
        auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (isKeyDown && kbd->vkCode == VK_ESCAPE) {
            if (g_hFlyoutWnd && IsWindow(g_hFlyoutWnd) && IsWindowVisible(g_hFlyoutWnd)) {
                HideFlyout();
                s_inKbdHook = false;
                return 1;
            }
        }

        bool isWinDown = ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) || ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0);
        bool isAltDown = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
        bool isCtrlDown = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);

        if (g_enableWinSpace && kbd->vkCode == VK_SPACE && isWinDown) {
            if (isKeyDown) {
                if (g_switcherStyle == L"win7") {
                    g_targetWindow = GetForegroundWindow();
                    RefreshKeyboardLayouts();
                    size_t count = 0;
                    {
                        std::lock_guard<std::mutex> lock(g_stateMutex);
                        count = g_layouts.size();
                        if (count > 0) {
                            g_selectedIndex = (g_selectedIndex + 1) % count;
                        }
                    }
                    TriggerSwitcher();
                } else {
                    if (!g_isWinSpaceCycling) {
                        g_isWinSpaceCycling = true;
                        g_targetWindow = GetForegroundWindow();
                        RefreshKeyboardLayouts();
                        
                        size_t count = 0;
                        {
                            std::lock_guard<std::mutex> lock(g_stateMutex);
                            count = g_layouts.size();
                            if (count > 0) {
                                g_selectedIndex = (g_selectedIndex + 1) % count;
                            }
                        }
                        ShowSwitcher();
                    } else {
                        size_t count = 0;
                        {
                            std::lock_guard<std::mutex> lock(g_stateMutex);
                            count = g_layouts.size();
                            if (count > 0) {
                                g_selectedIndex = (g_selectedIndex + 1) % count;
                            }
                        }
                        if (g_hFlyoutWnd && IsWindow(g_hFlyoutWnd)) {
                            InvalidateRect(g_hFlyoutWnd, nullptr, FALSE);
                        }
                    }
                }
                s_inKbdHook = false;
                return 1;
            }
        }

        if (g_isWinSpaceCycling && isKeyUp && (kbd->vkCode == VK_LWIN || kbd->vkCode == VK_RWIN)) {
            size_t sel = 0;
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                sel = g_selectedIndex;
            }
            SwitchToLayout(sel);
            HideFlyout();
            s_inKbdHook = false;
            return 0;
        }

        if (g_enableAltShift && isKeyDown && isAltDown && (kbd->vkCode == VK_SHIFT || kbd->vkCode == VK_LSHIFT || kbd->vkCode == VK_RSHIFT)) {
            g_targetWindow = GetForegroundWindow();
            RefreshKeyboardLayouts();
            size_t count = 0;
            size_t sel = 0;
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                count = g_layouts.size();
                if (count > 0) {
                    g_selectedIndex = (g_selectedIndex + 1) % count;
                    sel = g_selectedIndex;
                }
            }
            if (count > 0) {
                SwitchToLayout(sel);
                TriggerSwitcher();
            }
        }

        // Custom hotkey (Ctrl+Shift+L): opens the switcher without changing the
        // current layout, so pressing it is always non-destructive. Ctrl+Shift
        // is used instead of Ctrl+Alt because on many European keyboard layouts
        // (including Italian) AltGr generates a synthetic Left-Ctrl+Right-Alt
        // combination, which would make a Ctrl+Alt hotkey fire accidentally
        // while typing AltGr characters. Guarded with a static "already active"
        // flag so holding the keys down doesn't repeatedly reopen the flyout
        // on key-repeat.
        static bool s_customHotkeyActive = false;
        if (g_enableCustomHotkey && kbd->vkCode == 'L') {
            bool isShiftDown = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);
            if (isKeyDown && isCtrlDown && isShiftDown && !isAltDown) {
                if (!s_customHotkeyActive) {
                    s_customHotkeyActive = true;
                    g_targetWindow = GetForegroundWindow();
                    RefreshKeyboardLayouts();
                    TriggerSwitcher();
                }
                s_inKbdHook = false;
                return 1;
            }
            if (isKeyUp) {
                s_customHotkeyActive = false;
            }
        }
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in LowLevelKeyboardProc: %hs", e.what());
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in LowLevelKeyboardProc");
    }

    s_inKbdHook = false;
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || !lParam || g_shuttingDown.load(std::memory_order_acquire)) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    static thread_local bool s_inMouseHook = false;
    if (s_inMouseHook) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    s_inMouseHook = true;

    try {
        if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || 
            wParam == WM_MBUTTONDOWN || wParam == WM_NCLBUTTONDOWN || wParam == WM_NCRBUTTONDOWN) {
            if (g_hFlyoutWnd && IsWindow(g_hFlyoutWnd) && IsWindowVisible(g_hFlyoutWnd)) {
                auto* mouse = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
                RECT rcFlyout{};
                if (GetWindowRect(g_hFlyoutWnd, &rcFlyout)) {
                    if (!PtInRect(&rcFlyout, mouse->pt)) {
                        if (!g_isWinSpaceCycling) {
                            HideFlyout();
                        }
                    }
                }
            }
        }
    } catch (...) {}

    s_inMouseHook = false;
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// =============================================================================
// Settings Loading & Mod Lifecycle
// =============================================================================
static void LoadModSettings() {
    try {
        const wchar_t* styleSetting = Wh_GetStringSetting(L"switcherStyle");
        if (styleSetting) {
            g_switcherStyle = styleSetting;
            Wh_FreeStringSetting(styleSetting);
        } else {
            g_switcherStyle = L"win8";
        }

        const wchar_t* langSetting = Wh_GetStringSetting(L"language");
        if (langSetting) {
            g_uiLanguage = langSetting;
            Wh_FreeStringSetting(langSetting);
        } else {
            g_uiLanguage = L"auto";
        }

        const wchar_t* themeSetting = Wh_GetStringSetting(L"themeMode");
        if (themeSetting) {
            g_themeMode = themeSetting;
            Wh_FreeStringSetting(themeSetting);
        } else {
            g_themeMode = L"win8_purple";
        }

        const wchar_t* customColorSetting = Wh_GetStringSetting(L"customAccentColor");
        if (customColorSetting) {
            g_customAccentColor = ParseHexColor(customColorSetting, RGB(91, 44, 130));
            Wh_FreeStringSetting(customColorSetting);
        }

        const wchar_t* prefCmdSetting = Wh_GetStringSetting(L"customPreferencesCmd");
        if (prefCmdSetting) {
            g_customPreferencesCmd = prefCmdSetting;
            Wh_FreeStringSetting(prefCmdSetting);
        } else {
            g_customPreferencesCmd = L"ms-settings:regionlanguage";
        }

        g_enableWinSpace = (Wh_GetIntSetting(L"enableWinSpace") != 0);
        g_enableAltShift = (Wh_GetIntSetting(L"enableAltShift") != 0);
        g_hookTrayClicks = (Wh_GetIntSetting(L"hookTrayClicks") != 0);
        g_showShortcutHint = (Wh_GetIntSetting(L"showShortcutHint") != 0);
        g_enableCustomHotkey = (Wh_GetIntSetting(L"enableCustomHotkey") != 0);

        Wh_Log(L"Win78LangSwitcher: Settings loaded (style=%s, lang=%s, theme=%s, winSpace=%d, altShift=%d)",
               g_switcherStyle.c_str(), g_uiLanguage.c_str(), g_themeMode.c_str(), g_enableWinSpace ? 1 : 0, g_enableAltShift ? 1 : 0);
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in LoadModSettings: %hs", e.what());
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in LoadModSettings");
    }
}

// =============================================================================
// Windhawk Entry Points
// =============================================================================

BOOL Wh_ModInit(void) {
    try {
        Wh_Log(L"Win78LangSwitcher: Initializing Windows 7/8.1 Language Switcher Restorer");

        g_mainThreadId = GetCurrentThreadId();
        g_unloading.store(false, std::memory_order_release);
        g_shuttingDown.store(false, std::memory_order_release);

        DarkContextMenu::Init();
        if (!g_gdiplus.startup()) {
            Wh_Log(L"Win78LangSwitcher: GDI+ startup failed, Win7 checkmark will use GDI fallback");
        }
        LoadModSettings();
        RegisterFlyoutClass();

        HHOOK hKbd = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandleW(nullptr), 0);
        if (hKbd) {
            g_keyboardHook.reset(hKbd);
            Wh_Log(L"Win78LangSwitcher: Low-level keyboard hook installed successfully");
        } else {
            Wh_Log(L"Win78LangSwitcher: ❌ Failed to install keyboard hook (error=%lu)", GetLastError());
        }

        HHOOK hMouse = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandleW(nullptr), 0);
        if (hMouse) {
            g_mouseHook.reset(hMouse);
            Wh_Log(L"Win78LangSwitcher: Low-level mouse hook installed successfully");
        }

        LoadedModule hUser(GetModuleHandleW(L"user32.dll"), false);
        if (hUser.valid()) {
            void* pCreateWindowExW = reinterpret_cast<void*>(reinterpret_cast<FARPROC>(GetProcAddress(hUser.get(), "CreateWindowExW")));
            if (pCreateWindowExW) {
                WindhawkUtils::SetFunctionHook(reinterpret_cast<CreateWindowExW_t>(pCreateWindowExW),
                                               CreateWindowExW_Hook, &CreateWindowExW_Original);
                Wh_Log(L"Win78LangSwitcher: CreateWindowExW hook installed successfully");
            }

            void* pShowWindow = reinterpret_cast<void*>(reinterpret_cast<FARPROC>(GetProcAddress(hUser.get(), "ShowWindow")));
            if (pShowWindow) {
                WindhawkUtils::SetFunctionHook(reinterpret_cast<ShowWindow_t>(pShowWindow),
                                               ShowWindow_Hook, &ShowWindow_Original);
                Wh_Log(L"Win78LangSwitcher: ShowWindow hook installed successfully");
            }
        }

        EnumerateAndSubclassTaskbars();

        return TRUE;
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in Wh_ModInit: %hs", e.what());
        return FALSE;
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in Wh_ModInit");
        return FALSE;
    }
}

void Wh_ModAfterInit(void) {
    try {
        Wh_Log(L"Win78LangSwitcher: Initialization complete. Classic language switcher is active.");
        RefreshKeyboardLayouts();
    } catch (...) {}
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    try {
        LoadModSettings();
        HideFlyout();
        EnumerateAndSubclassTaskbars();
        if (reload) *reload = FALSE;
        return TRUE;
    } catch (...) {
        if (reload) *reload = TRUE;
        return TRUE;
    }
}

void Wh_ModBeforeUninit(void) {
    try {
        Wh_Log(L"Win78LangSwitcher: Wh_ModBeforeUninit - cleaning up subclasses and active windows");
        g_unloading.store(true, std::memory_order_release);

        HideFlyout();

        {
            std::lock_guard<std::mutex> lock(g_subclassedWindowsMutex);
            for (HWND hWnd : g_subclassedWindows) {
                if (hWnd && IsWindow(hWnd)) {
                    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, InputIndicatorButtonSubclassProc);
                }
            }
            g_subclassedWindows.clear();
        }
    } catch (...) {}
}

void Wh_ModUninit(void) {
    try {
        Wh_Log(L"Win78LangSwitcher: Unloading mod...");
        g_shuttingDown.store(true, std::memory_order_release);
        g_unloading.store(true, std::memory_order_release);

        DarkContextMenu::Restore();
        DarkContextMenu::Uninit();
        g_gdiplus.reset();

        for (int i = 0; i < 50 && g_inFlightSubclasses.load(std::memory_order_acquire) > 0; ++i) {
            Sleep(1);
        }

        g_keyboardHook.reset();
        g_mouseHook.reset();

        HideFlyout();
        if (g_hFlyoutWnd && IsWindow(g_hFlyoutWnd)) {
            DestroyWindow(g_hFlyoutWnd);
            g_hFlyoutWnd = nullptr;
        }

        UnregisterFlyoutClass();

        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            g_layouts.clear();
        }

        Wh_Log(L"Win78LangSwitcher: Unloaded successfully");
    } catch (const std::exception& e) {
        Wh_Log(L"Win78LangSwitcher: std::exception in Wh_ModUninit: %hs", e.what());
    } catch (...) {
        Wh_Log(L"Win78LangSwitcher: Unknown exception in Wh_ModUninit");
    }
}
