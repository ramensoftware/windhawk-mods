// ==WindhawkMod==
// @id              win7-open-with-dialog
// @name            Windows 7 Open With Dialog
// @description     This mod restores the classic Windows 7 "Open with" dialog on Windows 10 and 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         MIT
// @include         explorer.exe
// @include         OpenWith.exe
// @include         rundll32.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshell32 -lshlwapi -lversion -ladvapi32 -lcomctl32 -luxtheme -ldwmapi -luser32 -lgdi32 -luuid -lwinpthread
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Open With Dialog Recreation

This mod restores the classic Windows 7 **Open with** dialog on Windows 10 and
11, replacing (when the mod is active) the modern picker with an accurate recreation of the original Windows 7 one while
keeping the original file and application paths untouched.

The mod has been tested primarily on **Windows 10 21H2** and **Windows 11 25H2**. The public API and
classic context-menu paths are designed to fail safely on unsupported builds.
## Screenshot

![openwith](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/openwith.PNG)

## Features

- **Windows 7-style dialog**: Recreates the classic layout with recommended and
  other-program groups, Browse, Web search and the Always use checkbox.
- **Open with context menu**: Redirects the canonical `openas` command through
  the stable `CLSID_OpenWithMenu` context-menu contract.
- **Unknown-file double click**: Hooks the real OpenWith.exe COM server methods
  without substituting mod-owned COM objects.
- **Properties → Change**: Recognizes association-only launcher requests and
  selects a default without opening the file.
- **Persistent defaults**: Uses `IAssocHandler::MakeDefault` and can open
  Windows Default Apps if the protected system association rejects the change.
- **Direct selected-program launch**: Uses the selected ProgID/class key or the
  selected executable directly, avoiding recursive re-entry into Open With.
- **Browse application registration**: Programs selected through Browse are
  added to the current user's `Applications`, `SupportedTypes`, `OpenWithList`,
  `MRUList` and `OpenWithProgids` keys and appear immediately in the dialog.
- **Extensionless files**: Enumerates registered applications through a
   `HKCR\Applications` fallback.
- **Localized interface**: 120 languages (English, Italian, Spanish,
  French, German, Portuguese, Russian, Chinese, Japanese, Korean, Turkish,
  Dutch, Polish and 107 more), with automatic language detection.
- **Short file display**: Shows `File: photo.png` instead of the full path. The
  full path is retained internally for execution.
- **Optional description box**: The "Type a description to use for this kind of
  file" label and edit can be hidden from the settings (off by default), which
  also shrinks the dialog by the space they used.
- **Custom icon**: Uses the supplied document-and-magnifier artwork, embedded as
  transparent 32×32 BGRA Base64.
- **DPI aware**: Scales the window and controls for the owner monitor.
- **Defensive lifetime management**: Uses RAII for COM pointers, registry keys,
  handles, windows, icons, image lists, fonts and worker-owned dialogs, with
  exception containment around hook boundaries.

## Requirements

- **Windows 10 or Windows 11 x64**
- **Windhawk** with injection enabled for `explorer.exe`, `OpenWith.exe` and
  `rundll32.exe`
- The mod setting **Enable the Windows 7-style picker** must be enabled

## Note

When the user explicitly selects **Always use**, the mod attempts to persist an
association. All other Open With selections open the file once without changing
the default.

## Known limitations

- **One file at a time**: Multiple-selection Open With requests aren't handled.
- **No default for extensionless files**: They can be opened, but there is no
  extension to associate persistently.
- **Composite extensions**: Windows normally associates `archive.tar.gz` by
  `.gz`; the mod follows the same final-suffix behavior.
- **Store/UWP handlers**: Some AppX handlers don't expose a filesystem
  executable. They rely on their registered ProgID and may fail open if the
  registration is incomplete.
- **Protected associations**: Windows can reject changes to a protected
  association; use Windows Default Apps when that happens.

## Credits

- **ReactOS** — Inspiration
- **aubymori** - Inspiration
- **Image supplied by the user** — document-and-magnifier dialog icon.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- replaceSystemDialog: true
  $name: Enable the Windows 7-style picker
  $description: When enabled, replace Open With through the stable CLSID_OpenWithMenu context-menu contract, SHOpenWithDialog, the exact openas ShellExecute verb, or detours on the real OpenWith.exe server. When disabled, requests are passed to Windows.
- darkMode: auto
  $name: Window appearance theme
  $description: Visual theme for the dialog window.
  $options:
    - auto: Follow the Windows app theme
    - light: Always light (classic Windows 7)
    - dark: Always dark
- showWebLink: true
  $name: Show the Web search link
  $description: Show the Windows 7-style Web link. Only the sanitized file extension is sent to the browser search.
- hideDescriptionField: false
  $name: Hide the description box
  $description: Hide the "Type a description to use for this kind of file" label and its text box, and shrink the dialog by the space they used. The file type keeps its existing description when this is enabled.
- defaultAssociationBehavior: disabled
  $name: Always use checkbox behavior
  $description: The checkbox now uses IAssocHandler::MakeDefault. This setting controls only the fallback used when Windows rejects the association request.
  $options:
    - disabled: Don't open Settings on failure
    - openSettings: Open Windows Default Apps on failure
- language: auto
  $name: Language
  $description: Language used by the recreated picker.
  $options:
    - auto: Automatic
    - en: English
    - it: Italiano
    - es: Español
    - fr: Français
    - de: Deutsch
    - pt-BR: Português (Brasil)
    - pt-PT: Português (Portugal)
    - ru: Русский
    - zh-CN: 简体中文
    - zh-TW: 繁體中文
    - ja: 日本語
    - ko: 한국어
    - tr: Türkçe
    - nl: Nederlands
    - pl: Polski
    - sv: Svenska
    - da: Dansk
    - nb: Norsk
    - fi: Suomi
    - cs: Čeština
    - hu: Magyar
    - el: Ελληνικά
    - ar: العربية
    - he: עברית
    - ro: Română
    - uk: Українська
    - bg: Български
    - sk: Slovenčina
    - hr: Hrvatski
    - id: Bahasa Indonesia
    - af: Afrikaans
    - sq: Shqip
    - am: አማርኛ
    - hy: Հայերեն
    - as: অসমীয়া
    - az: Azərbaycan dili
    - ba: Башҡортса
    - eu: Euskara
    - be: Беларуская
    - bn: বাংলা
    - bs: Bosanski
    - ca: Català
    - chr: ᏣᎳᎩ
    - prs: دری
    - dv: ދިވެހި
    - et: Eesti
    - fil: Filipino
    - fy: Frysk
    - gl: Galego
    - ka: ქართული
    - gu: ગુજરાતી
    - ha: Hausa
    - hi: हिन्दी
    - is: Íslenska
    - ig: Igbo
    - ga: Gaeilge
    - xh: isiXhosa
    - zu: isiZulu
    - kn: ಕನ್ನಡ
    - kk: Қазақ тілі
    - km: ខ្មែរ
    - rw: Kinyarwanda
    - sw: Kiswahili
    - kok: कोंकणी
    - ky: Кыргызча
    - lo: ລາວ
    - lv: Latviešu
    - lt: Lietuvių
    - lb: Lëtzebuergesch
    - mk: Македонски
    - ms: Bahasa Melayu
    - ml: മലയാളം
    - mt: Malti
    - mi: te reo Māori
    - mr: मराठी
    - mn: Монгол
    - my: မြန်မာ
    - ne: नेपाली
    - nn: Norsk nynorsk
    - or: ଓଡ଼ିଆ
    - ps: پښتو
    - fa: فارسی
    - pa: ਪੰਜਾਬੀ
    - quz: Runasimi
    - gd: Gàidhlig
    - sr-Latn: Srpski (latinica)
    - sr-Cyrl: Српски (ћирилица)
    - nso: Sesotho sa Leboa
    - tn: Setswana
    - si: සිංහල
    - sl: Slovenščina
    - tg: Тоҷикӣ
    - ta: தமிழ்
    - tt: Татарча
    - te: తెలుగు
    - th: ไทย
    - ti: ትግርኛ
    - tk: Türkmen dili
    - ur: اردو
    - ug: ئۇيغۇرچە
    - uz: Oʻzbekcha
    - vi: Tiếng Việt
    - cy: Cymraeg
    - wo: Wolof
    - yo: Yorùbá
    - iu: ᐃᓄᒃᑎᑐᑦ
    - ckb: کوردیی ناوەندی
    - sd: سنڌي
    - ks: کٲشُر
    - sa: संस्कृतम्
    - bo: བོད་ཡིག
    - wa: Walon
    - sah: Саха тыла
    - hsb: Hornjoserbšćina
    - dsb: Dolnoserbšćina
    - br: Brezhoneg
    - oc: Occitan
    - co: Corsu
    - kl: Kalaallisut
    - fo: Føroyskt
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <objidl.h>
#include <ocidl.h>
#include <oleidl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <dwmapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1
#define DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 19
#endif
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <cwctype>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <vector>

// -----------------------------------------------------------------------------
// Undocumented dark-mode activation APIs.
//
// SetWindowTheme(hwnd, L"DarkMode_Explorer", ...) alone only recolors a
// handful of surfaces. Without also calling into these ordinal-only uxtheme
// exports, comctl32 keeps drawing ListView group headers, selection
// highlight, WS_EX_CLIENTEDGE borders and scrollbars using their light-theme
// colors, which is why those pieces stay light/illegible even though the
// rest of the dialog is dark.
// -----------------------------------------------------------------------------
namespace DarkModeActivation {
enum class AppMode { Default, AllowDark, ForceDark, ForceLight, Max };
using SetPreferredAppMode_t = AppMode(WINAPI*)(AppMode);
using AllowDarkModeForWindow_t = bool(WINAPI*)(HWND, bool);

static HMODULE g_hUxtheme = nullptr;
static SetPreferredAppMode_t pSetPreferredAppMode = nullptr;
static AllowDarkModeForWindow_t pAllowDarkModeForWindow = nullptr;
static bool g_resolved = false;

// SetPreferredAppMode is a PER-PROCESS setting and the picker's worker
// thread lives inside explorer.exe, which sets AllowDark for itself at
// startup. Passing Default here would turn Explorer's own dark rendering
// off process-wide (shell context menus, controls, and any other mod that
// had set AllowDark/ForceDark), and the change would outlive the mod.
// So: capture the host's mode once, never pass Default as a "light"
// request, and put the captured mode back when the picker closes and in
// Wh_ModUninit. FlushMenuThemes is not called at all - it re-themes the
// whole process and the picker has no menus.
static AppMode g_initialAppMode = AppMode::Default;
static bool g_initialAppModeCaptured = false;
static bool g_appModeOverridden = false;

static void Resolve() {
    if (g_resolved) return;
    g_resolved = true;
    g_hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr,
                                LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hUxtheme) return;
    pSetPreferredAppMode = reinterpret_cast<SetPreferredAppMode_t>(
        GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(135)));
    pAllowDarkModeForWindow = reinterpret_cast<AllowDarkModeForWindow_t>(
        GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(133)));
}

// Reads the host process's current preferred app mode without changing
// it: the ordinal returns the mode that was in effect, so calling it with
// that same value both queries and leaves the process untouched.
static void CaptureInitialAppMode() {
    if (g_initialAppModeCaptured || !pSetPreferredAppMode) return;
    g_initialAppMode = pSetPreferredAppMode(AppMode::Default);
    pSetPreferredAppMode(g_initialAppMode);
    g_initialAppModeCaptured = true;
}

// Restores the host's captured mode. Called when the picker closes and
// from Wh_ModUninit, so nothing the mod did to the process survives it.
static void RestoreAppMode() {
    if (!g_appModeOverridden) return;
    g_appModeOverridden = false;
    if (pSetPreferredAppMode && g_initialAppModeCaptured)
        pSetPreferredAppMode(g_initialAppMode);
}

// Recursively opts a window and every descendant into (or out of) dark
// rendering, then asks it to re-pull its theme. Safe to call repeatedly;
// harmless if the ordinals weren't resolved (e.g. older/locked-down builds).
static void Apply(HWND window, bool dark) {
    Resolve();
    if (!window) return;
    if (pSetPreferredAppMode) {
        CaptureInitialAppMode();
        if (dark) {
            // Only ever widen the process mode, and only while a dark
            // picker actually needs it. Hosts already running AllowDark
            // or ForceDark are left alone.
            if (g_initialAppMode != AppMode::AllowDark &&
                g_initialAppMode != AppMode::ForceDark) {
                pSetPreferredAppMode(AppMode::AllowDark);
                g_appModeOverridden = true;
            }
        } else {
            // Light picker: put back whatever the host had. Never force
            // Default, that would disable dark mode process-wide.
            RestoreAppMode();
        }
    }
    if (pAllowDarkModeForWindow) {
        pAllowDarkModeForWindow(window, dark);
        for (HWND child = GetWindow(window, GW_CHILD); child;
             child = GetWindow(child, GW_HWNDNEXT)) {
            pAllowDarkModeForWindow(child, dark);
        }
    }
    SendMessageW(window, WM_THEMECHANGED, 0, 0);
    for (HWND child = GetWindow(window, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        SendMessageW(child, WM_THEMECHANGED, 0, 0);
    }
}
}  // namespace DarkModeActivation

// -----------------------------------------------------------------------------
// Move-only owners.
// -----------------------------------------------------------------------------

template <typename T>
class ComPtr {
   public:
    ComPtr() = default;
    explicit ComPtr(T* value) : value_(value) {}
    ~ComPtr() { Reset(); }
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ComPtr(ComPtr&& other) noexcept : value_(other.Detach()) {}
    ComPtr& operator=(ComPtr&& other) noexcept {
        if (this != &other) Reset(other.Detach());
        return *this;
    }
    T* Get() const { return value_; }
    T* operator->() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    T** Put() {
        Reset();
        return &value_;
    }
    T* Detach() {
        T* value = value_;
        value_ = nullptr;
        return value;
    }
    void Reset(T* value = nullptr) {
        if (value_) value_->Release();
        value_ = value;
    }

   private:
    T* value_ = nullptr;
};

class IconOwner {
   public:
    IconOwner() = default;
    explicit IconOwner(HICON value) : value_(value) {}
    ~IconOwner() { Reset(); }
    IconOwner(const IconOwner&) = delete;
    IconOwner& operator=(const IconOwner&) = delete;
    IconOwner(IconOwner&& other) noexcept : value_(other.Detach()) {}
    IconOwner& operator=(IconOwner&& other) noexcept {
        if (this != &other) Reset(other.Detach());
        return *this;
    }
    HICON Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    HICON Detach() {
        HICON value = value_;
        value_ = nullptr;
        return value;
    }
    void Reset(HICON value = nullptr) {
        if (value_) DestroyIcon(value_);
        value_ = value;
    }

   private:
    HICON value_ = nullptr;
};

class ImageListOwner {
   public:
    ~ImageListOwner() { Reset(); }
    ImageListOwner() = default;
    ImageListOwner(const ImageListOwner&) = delete;
    ImageListOwner& operator=(const ImageListOwner&) = delete;
    HIMAGELIST Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    void Reset(HIMAGELIST value = nullptr) {
        if (value_) ImageList_Destroy(value_);
        value_ = value;
    }

   private:
    HIMAGELIST value_ = nullptr;
};

class FontOwner {
   public:
    ~FontOwner() { Reset(); }
    FontOwner() = default;
    FontOwner(const FontOwner&) = delete;
    FontOwner& operator=(const FontOwner&) = delete;
    HFONT Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    void Reset(HFONT value = nullptr) {
        if (value_) DeleteObject(value_);
        value_ = value;
    }

   private:
    HFONT value_ = nullptr;
};

class BrushOwner {
   public:
    ~BrushOwner() { Reset(); }
    BrushOwner() = default;
    explicit BrushOwner(HBRUSH value) : value_(value) {}
    BrushOwner(const BrushOwner&) = delete;
    BrushOwner& operator=(const BrushOwner&) = delete;
    HBRUSH Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    void Reset(HBRUSH value = nullptr) {
        if (value_) DeleteObject(value_);
        value_ = value;
    }

   private:
    HBRUSH value_ = nullptr;
};

class PenOwner {
   public:
    PenOwner() = default;
    explicit PenOwner(HPEN value) : value_(value) {}
    ~PenOwner() { Reset(); }
    PenOwner(const PenOwner&) = delete;
    PenOwner& operator=(const PenOwner&) = delete;
    HPEN Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    void Reset(HPEN value = nullptr) {
        if (value_) DeleteObject(value_);
        value_ = value;
    }

   private:
    HPEN value_ = nullptr;
};

class RegionOwner {
   public:
    RegionOwner() = default;
    explicit RegionOwner(HRGN value) : value_(value) {}
    ~RegionOwner() { Reset(); }
    RegionOwner(const RegionOwner&) = delete;
    RegionOwner& operator=(const RegionOwner&) = delete;
    HRGN Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    void Reset(HRGN value = nullptr) {
        if (value_) DeleteObject(value_);
        value_ = value;
    }

   private:
    HRGN value_ = nullptr;
};

class BitmapOwner {
   public:
    BitmapOwner() = default;
    explicit BitmapOwner(HBITMAP value) : value_(value) {}
    ~BitmapOwner() { Reset(); }
    BitmapOwner(const BitmapOwner&) = delete;
    BitmapOwner& operator=(const BitmapOwner&) = delete;
    HBITMAP Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    HBITMAP Detach() {
        HBITMAP value = value_;
        value_ = nullptr;
        return value;
    }
    void Reset(HBITMAP value = nullptr) {
        if (value_) DeleteObject(value_);
        value_ = value;
    }

   private:
    HBITMAP value_ = nullptr;
};

// Window DC obtained with GetDC: released against the window it came
// from, unlike a memory DC, which is deleted (see MemoryDcOwner).
class WindowDcOwner {
   public:
    WindowDcOwner(HWND window, HDC dc) : window_(window), dc_(dc) {}
    explicit WindowDcOwner(HWND window)
        : window_(window), dc_(GetDC(window)) {}
    ~WindowDcOwner() {
        if (dc_) ReleaseDC(window_, dc_);
    }
    WindowDcOwner(const WindowDcOwner&) = delete;
    WindowDcOwner& operator=(const WindowDcOwner&) = delete;
    HDC Get() const { return dc_; }
    explicit operator bool() const { return dc_ != nullptr; }

   private:
    HWND window_ = nullptr;
    HDC dc_ = nullptr;
};

class MemoryDcOwner {
   public:
    MemoryDcOwner() = default;
    explicit MemoryDcOwner(HDC dc) : dc_(dc) {}
    ~MemoryDcOwner() {
        if (dc_) DeleteDC(dc_);
    }
    MemoryDcOwner(const MemoryDcOwner&) = delete;
    MemoryDcOwner& operator=(const MemoryDcOwner&) = delete;
    HDC Get() const { return dc_; }
    explicit operator bool() const { return dc_ != nullptr; }

   private:
    HDC dc_ = nullptr;
};

// Restores the object a SelectObject call displaced. Selecting into a DC
// is a swap, so the previous object has to go back before the DC is
// released or the replacement is destroyed.
class SelectedObject {
   public:
    SelectedObject() = default;
    SelectedObject(HDC dc, HGDIOBJ object)
        : dc_(dc), previous_(object ? SelectObject(dc, object) : nullptr) {}
    ~SelectedObject() {
        if (dc_ && previous_) SelectObject(dc_, previous_);
    }
    SelectedObject(const SelectedObject&) = delete;
    SelectedObject& operator=(const SelectedObject&) = delete;

   private:
    HDC dc_ = nullptr;
    HGDIOBJ previous_ = nullptr;
};

class ThemeOwner {
   public:
    ThemeOwner() = default;
    explicit ThemeOwner(HTHEME value) : value_(value) {}
    ~ThemeOwner() { Reset(); }
    ThemeOwner(const ThemeOwner&) = delete;
    ThemeOwner& operator=(const ThemeOwner&) = delete;
    HTHEME Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    void Reset(HTHEME value = nullptr) {
        if (value_) CloseThemeData(value_);
        value_ = value;
    }

   private:
    HTHEME value_ = nullptr;
};

// CommandLineToArgvW returns a single LocalAlloc block holding both the
// pointer array and the strings.
class ArgvOwner {
   public:
    explicit ArgvOwner(LPCWSTR commandLine)
        : value_(CommandLineToArgvW(commandLine, &count_)) {
        if (!value_) count_ = 0;
    }
    ~ArgvOwner() {
        if (value_) LocalFree(value_);
    }
    ArgvOwner(const ArgvOwner&) = delete;
    ArgvOwner& operator=(const ArgvOwner&) = delete;
    LPWSTR* Get() const { return value_; }
    int Count() const { return count_; }
    explicit operator bool() const { return value_ != nullptr; }
    LPCWSTR operator[](int index) const { return value_[index]; }

   private:
    LPWSTR* value_ = nullptr;
    int count_ = 0;
};

static std::atomic<HWND> g_activeBrowseHwnd{nullptr};
static std::atomic<bool> g_shuttingDown{false};

struct HandleDeleter {
    void operator()(HANDLE handle) const noexcept {
        if (handle && handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
    }
};
using WinHandle = std::unique_ptr<
    std::remove_pointer<HANDLE>::type, HandleDeleter>;

class RegKeyOwner {
   public:
    RegKeyOwner() = default;
    explicit RegKeyOwner(HKEY value) : value_(value) {}
    ~RegKeyOwner() { Reset(); }
    RegKeyOwner(const RegKeyOwner&) = delete;
    RegKeyOwner& operator=(const RegKeyOwner&) = delete;
    RegKeyOwner(RegKeyOwner&& other) noexcept : value_(other.Detach()) {}
    RegKeyOwner& operator=(RegKeyOwner&& other) noexcept {
        if (this != &other) Reset(other.Detach());
        return *this;
    }
    HKEY Get() const { return value_; }
    HKEY* Put() {
        Reset();
        return &value_;
    }
    explicit operator bool() const { return value_ != nullptr; }
    HKEY Detach() {
        HKEY value = value_;
        value_ = nullptr;
        return value;
    }
    void Reset(HKEY value = nullptr) {
        if (value_) RegCloseKey(value_);
        value_ = value;
    }

   private:
    HKEY value_ = nullptr;
};

class WindowOwner {
   public:
    WindowOwner() = default;
    explicit WindowOwner(HWND value) : value_(value) {}
    ~WindowOwner() { Reset(); }
    WindowOwner(const WindowOwner&) = delete;
    WindowOwner& operator=(const WindowOwner&) = delete;
    HWND Get() const { return value_; }
    explicit operator bool() const { return value_ && IsWindow(value_); }
    void Reset(HWND value = nullptr) {
        if (value_ && IsWindow(value_)) DestroyWindow(value_);
        value_ = value;
    }

   private:
    HWND value_ = nullptr;
};

class ComApartment {
   public:
    explicit ComApartment(DWORD flags)
        : result_(CoInitializeEx(nullptr, flags)) {}
    ~ComApartment() {
        if (SUCCEEDED(result_)) CoUninitialize();
    }
    HRESULT Result() const { return result_; }
    bool Ready() const { return SUCCEEDED(result_); }

   private:
    HRESULT result_;
};

// -----------------------------------------------------------------------------
// Association interfaces. These are the SDK/Shell association-handler ABI used
// by SHAssocEnumHandlers; explicit local names avoid dependence on header age.
// -----------------------------------------------------------------------------

MIDL_INTERFACE("F04061AC-1659-4A3F-A954-775AA57FC083")
StandaloneAssocHandler : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE GetName(PWSTR* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUIName(PWSTR* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIconLocation(PWSTR* path,
                                                       int* index) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsRecommended() = 0;
    virtual HRESULT STDMETHODCALLTYPE MakeDefault(PCWSTR description) = 0;
    virtual HRESULT STDMETHODCALLTYPE Invoke(IDataObject* dataObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateInvoker(IDataObject* dataObject,
                                                     IUnknown** invoker) = 0;
};

MIDL_INTERFACE("973810AE-9599-4B88-9E4D-6EE98C9552DA")
StandaloneEnumAssocHandlers : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG count,
                                           StandaloneAssocHandler** handlers,
                                           ULONG* fetched) = 0;
};

using SHAssocEnumHandlers_t = HRESULT(WINAPI*)(
    PCWSTR extension, DWORD filter, StandaloneEnumAssocHandlers** result);

// -----------------------------------------------------------------------------
// Settings and complete localization catalog.
// -----------------------------------------------------------------------------

enum LocaleStringId {
    STR_TITLE,
    STR_INSTRUCTION,
    STR_FILE_LABEL,
    STR_RECOMMENDED,
    STR_OTHER,
    STR_DESCRIPTION,
    STR_ALWAYS_USE,
    STR_BROWSE,
    STR_WEB_LINK,
    STR_BROWSE_TITLE,
    STR_PROGRAMS,
    STR_ALL_FILES,
    STR_OK,
    STR_CANCEL,
    STR_NO_HANDLERS,
    STR_OPEN_FAILED,
    STR_COUNT,
};

struct LocalePack {
    LANGID langId;
    PCWSTR strings[STR_COUNT];
};

static const LocalePack g_Locales[] = {
    {0x0409, {  // English
        L"Open with",
        L"Choose the program you want to use to open this file:",
        L"File:",
        L"Recommended Programs",
        L"Other Programs",
        L"Type a description that you want to use for this kind of file:",
        L"&Always use the selected program to open this kind of file",
        L"&Browse...",
        L"If the program you want is not in the list or on your computer, you can <A ID=\"WebSearch\">look for the appropriate program on the Web</A>.",
        L"Open with...",
        L"Programs",
        L"All Files",
        L"OK",
        L"Cancel",
        L"No registered program can open this kind of file.",
        L"The selected program could not open the file.",
    }},
    {0x0410, {  // Italian
        L"Apri con",
        L"Scegliere il programma da utilizzare per aprire il file:",
        L"File:",
        L"Programmi consigliati",
        L"Altri programmi",
        L"Digitare una descrizione da utilizzare per questo tipo di file:",
        L"&Usa sempre il programma selezionato per questo tipo di file",
        L"&Sfoglia...",
        L"Se il programma desiderato non è presente nell'elenco o nel computer, è possibile <A ID=\"WebSearch\">cercare il programma appropriato nel Web</A>.",
        L"Apri con...",
        L"Programmi",
        L"Tutti i file",
        L"OK",
        L"Annulla",
        L"Nessun programma registrato può aprire questo tipo di file.",
        L"Impossibile aprire il file con il programma selezionato.",
    }},
    {0x040A, {  // Spanish
        L"Abrir con",
        L"Elija el programa que desea usar para abrir este archivo:",
        L"Archivo:",
        L"Programas recomendados",
        L"Otros programas",
        L"Escriba una descripción que desee usar para este tipo de archivo:",
        L"&Usar siempre el programa seleccionado para abrir este tipo de archivos",
        L"&Examinar...",
        L"Si el programa que desea no está en la lista o en el equipo, puede <A ID=\"WebSearch\">buscar el programa adecuado en la Web</A>.",
        L"Abrir con...",
        L"Programas",
        L"Todos los archivos",
        L"Aceptar",
        L"Cancelar",
        L"Ningún programa registrado puede abrir este tipo de archivo.",
        L"El programa seleccionado no pudo abrir el archivo.",
    }},
    {0x040C, {  // French
        L"Ouvrir avec",
        L"Choisissez le programme à utiliser pour ouvrir ce fichier :",
        L"Fichier :",
        L"Programmes recommandés",
        L"Autres programmes",
        L"Tapez une description que vous souhaitez utiliser pour ce type de fichier :",
        L"&Toujours utiliser le programme sélectionné pour ouvrir ce type de fichier",
        L"&Parcourir...",
        L"Si le programme souhaité ne figure pas dans la liste ou sur votre ordinateur, vous pouvez <A ID=\"WebSearch\">rechercher le programme approprié sur le Web</A>.",
        L"Ouvrir avec...",
        L"Programmes",
        L"Tous les fichiers",
        L"OK",
        L"Annuler",
        L"Aucun programme enregistré ne peut ouvrir ce type de fichier.",
        L"Le programme sélectionné n'a pas pu ouvrir le fichier.",
    }},
    {0x0407, {  // German
        L"Öffnen mit",
        L"Wählen Sie das Programm aus, das Sie zum Öffnen dieser Datei verwenden möchten:",
        L"Dateiname:",
        L"Empfohlene Programme",
        L"Andere Programme",
        L"Geben Sie eine Beschreibung für diesen Dateityp ein:",
        L"&Dateityp immer mit dem ausgewählten Programm öffnen",
        L"&Durchsuchen...",
        L"Falls das gewünschte Programm nicht aufgeführt ist, können Sie <A ID=\"WebSearch\">im Web nach dem entsprechenden Programm suchen</A>.",
        L"Öffnen mit...",
        L"Programme",
        L"Alle Dateien",
        L"OK",
        L"Abbrechen",
        L"Kein registriertes Programm kann diesen Dateityp öffnen.",
        L"Die Datei konnte nicht mit dem ausgewählten Programm geöffnet werden.",
    }},
    {0x0416, {  // Portuguese (Brazil)
        L"Abrir com",
        L"Escolha o programa que você deseja usar para abrir este arquivo:",
        L"Arquivo:",
        L"Programas Recomendados",
        L"Outros Programas",
        L"Digite uma descrição que você deseja usar para este tipo de arquivo:",
        L"&Sempre usar o programa selecionado para abrir este tipo de arquivo",
        L"&Procurar...",
        L"Se o programa desejado não estiver na lista ou no computador, você pode <A ID=\"WebSearch\">procurar o programa apropriado na Web</A>.",
        L"Abrir com...",
        L"Programas",
        L"Todos os Arquivos",
        L"OK",
        L"Cancelar",
        L"Nenhum programa registrado pode abrir este tipo de arquivo.",
        L"O programa selecionado não pôde abrir o arquivo.",
    }},
    {0x0816, {  // Portuguese (Portugal)
        L"Abrir com",
        L"Escolha o programa que pretende utilizar para abrir este ficheiro:",
        L"Ficheiro:",
        L"Programas Recomendados",
        L"Outros Programas",
        L"Escreva uma descrição que pretende utilizar para este tipo de ficheiro:",
        L"&Utilizar sempre o programa selecionado para abrir este tipo de ficheiro",
        L"&Procurar...",
        L"Se o programa pretendido não constar da lista, pode <A ID=\"WebSearch\">procurar o programa adequado na Web</A>.",
        L"Abrir com...",
        L"Programas",
        L"Todos os Ficheiros",
        L"OK",
        L"Cancelar",
        L"Nenhum programa registado pode abrir este tipo de ficheiro.",
        L"O programa selecionado não conseguiu abrir o ficheiro.",
    }},
    {0x0419, {  // Russian
        L"Выбор программы",
        L"Выберите программу для открытия этого файла:",
        L"Файл:",
        L"Рекомендуемые программы",
        L"Другие программы",
        L"Введите описание для этого типа файлов:",
        L"&Использовать выбранную программу для всех файлов этого типа",
        L"&Обзор...",
        L"Если нужной программы нет в списке, можно <A ID=\"WebSearch\">найти ее в Интернете</A>.",
        L"Выбор программы...",
        L"Программы",
        L"Все файлы",
        L"ОК",
        L"Отмена",
        L"Нет зарегистрированных программ для этого типа файлов.",
        L"Выбранная программа не может открыть этот файл.",
    }},
    {0x0804, {  // Chinese (Simplified)
        L"打开方式",
        L"选择您想用来打开此文件的程序:",
        L"文件:",
        L"推荐的程序",
        L"其他程序",
        L"键入您想用于此类文件的描述:",
        L"始终使用选择的程序打开这种文件(&A)",
        L"浏览(&B)...",
        L"如果您需要的程序不在列表中或计算机上，您可以<A ID=\"WebSearch\">在 Web 上查找适当的程序</A>。",
        L"打开方式...",
        L"程序",
        L"所有文件",
        L"确定",
        L"取消",
        L"没有已注册的程序可以打开此类文件。",
        L"所选程序无法打开该文件。",
    }},
    {0x0404, {  // Chinese (Traditional)
        L"開啟檔案",
        L"選擇要用來開啟此檔案的程式:",
        L"檔案:",
        L"建議的程式",
        L"其他程式",
        L"輸入要用於此類檔案的描述:",
        L"永遠使用選取的程式開啟這種檔案(&A)",
        L"瀏覽(&B)...",
        L"如果要使用的程式不在清單或電腦中，您可以<A ID=\"WebSearch\">在網路上尋找適當的程式</A>。",
        L"開啟檔案...",
        L"程式",
        L"所有檔案",
        L"確定",
        L"取消",
        L"沒有已註冊的程式可以開啟此類檔案。",
        L"選取的程式無法開啟檔案。",
    }},
    {0x0411, {  // Japanese
        L"ファイルを開くプログラムの選択",
        L"このファイルを開くプログラムを選択してください:",
        L"ファイル:",
        L"推奨されたプログラム",
        L"ほかのプログラム",
        L"このファイルの種類に使用する説明を入力してください:",
        L"この種類のファイルを開くときは、いつもこのプログラムを使う(&A)",
        L"参照(&B)...",
        L"使いたいプログラムが一覧にない場合は、<A ID=\"WebSearch\">Web で適切なプログラムを探す</A>ことができます。",
        L"ファイルを開くプログラムの選択...",
        L"プログラム",
        L"すべてのファイル",
        L"OK",
        L"キャンセル",
        L"この種類のファイルを開くことができる登録されたプログラムはありません。",
        L"選択したプログラムでファイルを開くことができませんでした。",
    }},
    {0x0412, {  // Korean
        L"연결 프로그램",
        L"이 파일을 열 때 사용할 프로그램을 선택하십시오:",
        L"파일:",
        L"권장하는 프로그램",
        L"기타 프로그램",
        L"이 파일 형식에 사용할 설명을 입력하십시오:",
        L"이 종류의 파일을 열 때 항상 선택된 프로그램 사용(&A)",
        L"찾아보기(&B)...",
        L"원하는 프로그램이 목록이나 컴퓨터에 없으면 <A ID=\"WebSearch\">웹에서 적절한 프로그램을 검색</A>할 수 있습니다.",
        L"연결 프로그램...",
        L"프로그램",
        L"모든 파일",
        L"확인",
        L"취소",
        L"이 파일 형식을 열 수 있는 등록된 프로그램이 없습니다.",
        L"선택한 프로그램으로 파일을 열 수 없습니다.",
    }},
    {0x041F, {  // Turkish
        L"Birlikte Aç",
        L"Bu dosyayı açmak için kullanmak istediğiniz programı seçin:",
        L"Dosya:",
        L"Önerilen Programlar",
        L"Diğer Programlar",
        L"Bu dosya türü için kullanmak istediğiniz bir açıklama yazın:",
        L"Bu tür dosyaları açmak için &her zaman seçili programı kullan",
        L"&Gözat...",
        L"İstediğiniz program listede veya bilgisayarınızda yoksa, <A ID=\"WebSearch\">Web'de uygun programı arayabilirsiniz</A>.",
        L"Birlikte aç...",
        L"Programlar",
        L"Tüm Dosyalar",
        L"Tamam",
        L"İptal",
        L"Kayıtlı hiçbir program bu tür dosyayı açamaz.",
        L"Seçilen program dosyayı açamadı.",
    }},
    {0x0413, {  // Dutch
        L"Openen met",
        L"Kies het programma dat u wilt gebruiken om dit bestand te openen:",
        L"Bestand:",
        L"Aanbevolen programma's",
        L"Andere programma's",
        L"Typ een beschrijving die u wilt gebruiken voor dit type bestand:",
        L"Dit type bestand &altijd met dit programma openen",
        L"&Bladeren...",
        L"Als het gewenste programma niet in de lijst of op de computer staat, kunt u <A ID=\"WebSearch\">op internet naar het juiste programma zoeken</A>.",
        L"Openen met...",
        L"Programma's",
        L"Alle bestanden",
        L"OK",
        L"Annuleren",
        L"Er is geen geregistreerd programma waarmee dit type bestand kan worden geopend.",
        L"Het geselecteerde programma kan het bestand niet openen.",
    }},
    {0x0415, {  // Polish
        L"Otwórz za pomocą",
        L"Wybierz program, którego chcesz użyć do otwarcia tego pliku:",
        L"Plik:",
        L"Polecane programy",
        L"Inne programy",
        L"Wpisz opis, którego chcesz użyć dla tego typu pliku:",
        L"&Zawsze używaj wybranego programu do otwierania tego typu plików",
        L"&Przeglądaj...",
        L"Jeśli żądanego programu nie ma na liście ani na komputerze, możesz <A ID=\"WebSearch\">poszukać odpowiedniego programu w sieci Web</A>.",
        L"Otwórz za pomocą...",
        L"Programy",
        L"Wszystkie pliki",
        L"OK",
        L"Anuluj",
        L"Żaden zarejestrowany program nie może otworzyć tego typu pliku.",
        L"Wybrany program nie mógł otworzyć pliku.",
    }},
    {0x041D, {  // Swedish
        L"Öppna med",
        L"Välj det program som du vill använda för att öppna den här filen:",
        L"Fil:",
        L"Rekommenderade program",
        L"Andra program",
        L"Ange en beskrivning som du vill använda för den här filtypen:",
        L"Använd &alltid detta program för att öppna den här filtypen",
        L"&Bläddra...",
        L"Om programmet inte finns i listan kan du <A ID=\"WebSearch\">söka efter lämpligt program på webben</A>.",
        L"Öppna med...",
        L"Program",
        L"Alla filer",
        L"OK",
        L"Avbryt",
        L"Det finns inget registrerat program som kan öppna den här filtypen.",
        L"Det markerade programmet kunde inte öppna filen.",
    }},
    {0x0406, {  // Danish
        L"Åbn med",
        L"Vælg det program, du vil bruge til at åbne filen med:",
        L"Fil:",
        L"Anbefalede programmer",
        L"Andre programmer",
        L"Skriv en beskrivelse, du vil bruge til denne type fil:",
        L"Brug &altid det valgte program til at åbne denne type fil",
        L"&Gennemse...",
        L"Hvis programmet ikke er på listen, kan du <A ID=\"WebSearch\">søge efter et program på internettet</A>.",
        L"Åbn med...",
        L"Programmer",
        L"Alle filer",
        L"OK",
        L"Annuller",
        L"Intet registreret program kan åbne denne filtype.",
        L"Det valgte program kunne ikke åbne filen.",
    }},
    {0x0414, {  // Norwegian
        L"Åpne i",
        L"Velg programmet du vil bruke til å åpne denne filen:",
        L"Fil:",
        L"Anbefalte programmer",
        L"Andre programmer",
        L"Skriv inn en beskrivelse du vil bruke for denne filtypen:",
        L"Bruk &alltid det valgte programmet til å åpne denne filtypen",
        L"&Bla gjennom...",
        L"Hvis programmet ikke finnes i listen, kan du <A ID=\"WebSearch\">søke etter et passende program på Internett</A>.",
        L"Åpne i...",
        L"Programmer",
        L"Alle filer",
        L"OK",
        L"Avbryt",
        L"Ingen registrerte programmer kan åpne denne filtypen.",
        L"Det valgte programmet kunne ikke åpne filen.",
    }},
    {0x040B, {  // Finnish
        L"Avaa sovelluksessa",
        L"Valitse sovellus, jolla haluat avata tämän tiedoston:",
        L"Tiedosto:",
        L"Suositellut sovellukset",
        L"Muut sovellukset",
        L"Kirjoita kuvaus, jota haluat käyttää tälle tiedostotyypille:",
        L"Käytä &aina valittua sovellusta tämän tiedostotyypin avaamiseen",
        L"&Selaa...",
        L"Jos haluamasi sovellus ei ole luettelossa, voit <A ID=\"WebSearch\">hakea sopivaa sovellusta verkosta</A>.",
        L"Avaa sovelluksessa...",
        L"Sovellukset",
        L"Kaikki tiedostot",
        L"OK",
        L"Peruuta",
        L"Yksikään rekisteröity sovellus ei voi avata tätä tiedostotyyppiä.",
        L"Valittu sovellus ei voinut avata tiedostoa.",
    }},
    {0x0405, {  // Czech
        L"Otevřít v programu",
        L"Vyberte program, který chcete použít k otevření tohoto souboru:",
        L"Soubor:",
        L"Doporučené programy",
        L"Jiné programy",
        L"Zadejte popis, který chcete použít pro tento typ souboru:",
        L"K otevření tohoto typu souboru &vždycky použít vybraný program",
        L"&Procházet...",
        L"Pokud požadovaný program není v seznamu, můžete <A ID=\"WebSearch\">vyhledat vhodný program na webu</A>.",
        L"Otevřít v programu...",
        L"Programy",
        L"Všechny soubory",
        L"OK",
        L"Storno",
        L"Žádný registrovaný program nemůže otevřít tento typ souboru.",
        L"Vybraný program nemohl soubor otevřít.",
    }},
    {0x040E, {  // Hungarian
        L"Társítás",
        L"Válassza ki a fájl megnyitásához használni kívánt programot:",
        L"Fájl:",
        L"Ajánlott programok",
        L"Egyéb programok",
        L"Írja be a fájltípushoz használni kívánt leírást:",
        L"&Mindig a kijelölt programmal nyissa meg ezt a fájltípust",
        L"&Tallózás...",
        L"Ha a kívánt program nem található a listában, <A ID=\"WebSearch\">megkeresheti a megfelelő programot a weben</A>.",
        L"Társítás...",
        L"Programok",
        L"Minden fájl",
        L"OK",
        L"Mégse",
        L"Nincs regisztrált program a fájltípus megnyitásához.",
        L"A kijelölt program nem tudta megnyitni a fájlt.",
    }},
    {0x0408, {  // Greek
        L"Άνοιγμα με",
        L"Επιλέξτε το πρόγραμμα που θέλετε να χρησιμοποιήσετε για το άνοιγμα αυτού του αρχείου:",
        L"Αρχείο:",
        L"Προτεινόμενα προγράμματα",
        L"Άλλα προγράμματα",
        L"Πληκτρολογήστε μια περιγραφή για αυτόν τον τύπο αρχείου:",
        L"&Να χρησιμοποιείται πάντα το επιλεγμένο πρόγραμμα για αυτόν τον τύπο αρχείου",
        L"&Αναζήτηση...",
        L"Εάν το πρόγραμμα δεν υπάρχει στη λίστα, μπορείτε να <A ID=\"WebSearch\">αναζητήσετε το κατάλληλο πρόγραμμα στο Web</A>.",
        L"Άνοιγμα με...",
        L"Προγράμματα",
        L"Όλα τα αρχεία",
        L"OK",
        L"Άκυρο",
        L"Κανένα καταχωρημένο πρόγραμμα δεν μπορεί να ανοίξει αυτόν τον τύπο αρχείου.",
        L"Δεν ήταν δυνατό το άνοιγμα του αρχείου από το επιλεγμένο πρόγραμμα.",
    }},
    {0x0401, {  // Arabic
        L"فتح باستخدام",
        L"اختر البرنامج الذي تريد استخدامه لفتح هذا الملف:",
        L"الملف:",
        L"البرامج الموصى بها",
        L"برامج أخرى",
        L"اكتب وصفاً تريد استخدامه لهذا النوع من الملفات:",
        L"استخدام البرنامج المحدد &دائماً لفتح هذا النوع من الملفات",
        L"&استعراض...",
        L"إذا لم يكن البرنامج المطلوب في القائمة، يمكنك <A ID=\"WebSearch\">البحث عن البرنامج المناسب على الويب</A>.",
        L"فتح باستخدام...",
        L"البرامج",
        L"كافة الملفات",
        L"موافق",
        L"إلغاء الأمر",
        L"لا يوجد برنامج مسجل يمكنه فتح هذا النوع من الملفات.",
        L"تعذر فتح الملف باستخدام البرنامج المحدد.",
    }},
    {0x040D, {  // Hebrew
        L"פתח באמצעות",
        L"בחר את התוכנית שברצונך להשתמש בה כדי לפתוח קובץ זה:",
        L"קובץ:",
        L"תוכניות מומלצות",
        L"תוכניות אחרות",
        L"הקלד תיאור שברצונך להשתמש בו עבור סוג קובץ זה:",
        L"השתמש &תמיד בתוכנית שנבחרה לפתיחת סוג קובץ זה",
        L"&עיון...",
        L"אם התוכנית הרצויה אינה מופיעה ברשימה, באפשרותך <A ID=\"WebSearch\">לחפש את התוכנית המתאימה באינטרנט</A>.",
        L"פתח באמצעות...",
        L"תוכניות",
        L"כל הקבצים",
        L"אישור",
        L"ביטול",
        L"אין תוכנית רשומה היכולה לפתוח סוג קובץ זה.",
        L"התוכנית שנבחרה לא הצליחה לפתוח את הקובץ.",
    }},
    {0x0418, {  // Romanian
        L"Deschidere cu",
        L"Alegeți programul pe care doriți să îl utilizați pentru a deschide acest fișier:",
        L"Fișier:",
        L"Programe recomandate",
        L"Alte programe",
        L"Tastați o descriere pe care doriți să o utilizați pentru acest tip de fișier:",
        L"Se utilizează &întotdeauna programul selectat pentru acest tip de fișier",
        L"&Răsfoire...",
        L"Dacă programul dorit nu este în listă, puteți <A ID=\"WebSearch\">căuta programul corespunzător pe Web</A>.",
        L"Deschidere cu...",
        L"Programe",
        L"Toate fișierele",
        L"OK",
        L"Anulare",
        L"Niciun program înregistrat nu poate deschide acest tip de fișier.",
        L"Programul selectat nu a putut deschide fișierul.",
    }},
    {0x0422, {  // Ukrainian
        L"Вибір програми",
        L"Виберіть програму для відкриття цього файлу:",
        L"Файл:",
        L"Рекомендовані програми",
        L"Інші програми",
        L"Введіть опис для цього типу файлів:",
        L"&Завжди використовувати вибрану програму для цього типу файлів",
        L"&Огляд...",
        L"Якщо потрібної програми немає у списку, можна <A ID=\"WebSearch\">знайти її в Інтернеті</A>.",
        L"Вибір програми...",
        L"Програми",
        L"Усі файли",
        L"ОК",
        L"Скасувати",
        L"Немає зареєстрованих програм для цього типу файлів.",
        L"Вибрана програма не змогла відкрити файл.",
    }},
    {0x0402, {  // Bulgarian
        L"Отваряне с",
        L"Изберете програмата, която искате да използвате за отваряне на този файл:",
        L"Файл:",
        L"Препоръчани програми",
        L"Други програми",
        L"Въведете описание за този тип файл:",
        L"&Винаги използвай избраната програма за този тип файл",
        L"&Преглед...",
        L"Ако желаната програма не е в списъка, можете да <A ID=\"WebSearch\">поърсите подходяща програма в интернет</A>.",
        L"Отваряне с...",
        L"Програми",
        L"Всички файлове",
        L"ОК",
        L"Отказ",
        L"Няма регистрирана програма за отваряне на този тип файл.",
        L"Избраната програма не може да отвори файла.",
    }},
    {0x041B, {  // Slovak
        L"Otvoriť v programe",
        L"Vyberte program, ktorý chcete použiť na otvorenie tohto súboru:",
        L"Súbor:",
        L"Odporúčané programy",
        L"Iné programy",
        L"Zadajte popis, ktorý chcete použiť pre tento typ súboru:",
        L"Na otvorenie tohto typu súboru &vždy použiť vybratý program",
        L"&Prehľadávať...",
        L"Ak požadovaný program nie je v zozname, môžete <A ID=\"WebSearch\">vyhľadať vhodný program na webe</A>.",
        L"Otvoriť v programe...",
        L"Programy",
        L"Všetky súbory",
        L"OK",
        L"Zrušiť",
        L"Žiadny zaregistrovaný program nemôže otvoriť tento typ súboru.",
        L"Vybratý program nemohol otvoriť súbor.",
    }},
    {0x041A, {  // Croatian
        L"Otvori pomoću",
        L"Odaberite program kojim želite otvoriti ovu datoteku:",
        L"Datoteka:",
        L"Preporučeni programi",
        L"Ostali programi",
        L"Upišite opis koji želite koristiti za ovu vrstu datoteke:",
        L"&Uvijek koristi odabrani program za otvaranje ove vrste datoteka",
        L"&Pregledaj...",
        L"Ako željeni program nije na popisu, možete <A ID=\"WebSearch\">potražiti odgovarajući program na webu</A>.",
        L"Otvori pomoću...",
        L"Programi",
        L"Sve datoteke",
        L"U redu",
        L"Odustani",
        L"Nijedan registrirani program ne može otvoriti ovu vrstu datoteke.",
        L"Odabrani program nije mogao otvoriti datoteku.",
    }},
    {0x0421, {  // Indonesian
        L"Buka dengan",
        L"Pilih program yang ingin Anda gunakan untuk membuka file ini:",
        L"File:",
        L"Program yang Disarankan",
        L"Program Lainnya",
        L"Ketik deskripsi yang ingin Anda gunakan untuk jenis file ini:",
        L"&Selalu gunakan program yang dipilih untuk membuka jenis file ini",
        L"&Telusuri...",
        L"Jika program yang Anda inginkan tidak ada dalam daftar, Anda dapat <A ID=\"WebSearch\">mencari program yang sesuai di Web</A>.",
        L"Buka dengan...",
        L"Program",
        L"Semua File",
        L"OK",
        L"Batal",
        L"Tidak ada program terdaftar yang dapat membuka jenis file ini.",
        L"Program yang dipilih tidak dapat membuka file.",
    }},
    {0x0436, {  // Afrikaans
        L"Maak oop met",
        L"Kies die program wat u wil gebruik om hierdie lêer te open:",
        L"Lêer:",
        L"Aanbevole programme",
        L"Ander programme",
        L"Tik 'n beskrywing wat u vir hierdie soort lêer wil gebruik:",
        L"&Gebruik altyd die geselekteerde program om hierdie soort lêer te open",
        L"&Blaai...",
        L"As die program wat u wil hê nie in die lys of op u rekenaar is nie, kan u <A ID=\"WebSearch\">op die web na die gepaste program soek</A>.",
        L"Maak oop met...",
        L"Programme",
        L"Alle lêers",
        L"OK",
        L"Kanselleer",
        L"Geen geregistreerde program kan hierdie soort lêer open nie.",
        L"Die geselekteerde program kon nie die lêer open nie.",
    }},
    {0x041C, {  // Albanian
        L"Hap me",
        L"Zgjidhni programin që dëshironi të përdorni për të hapur këtë skedar:",
        L"Skedar:",
        L"Programe të rekomanduara",
        L"Programe të tjera",
        L"Shkruani një përshkrim që dëshironi të përdorni për këtë lloj skedari:",
        L"&Përdor gjithmonë programin e zgjedhur për të hapur këtë lloj skedari",
        L"&Shfleto...",
        L"Nëse programi që dëshironi nuk është në listë ose në kompjuterin tuaj, mund të <A ID=\"WebSearch\">kërkoni programin e duhur në internet</A>.",
        L"Hap me...",
        L"Programe",
        L"Të gjithë skedarët",
        L"OK",
        L"Anulo",
        L"Asnjë program i regjistruar nuk mund ta hapë këtë lloj skedari.",
        L"Programi i zgjedhur nuk mundi ta hapte skedarin.",
    }},
    {0x045E, {  // Amharic
        L"በ... ክፈት",
        L"ይህን ፋይል ለመክፈት መጠቀም የሚፈልጉትን ፕሮግራም ይምረጡ:",
        L"ፋይል:",
        L"የሚመከሩ ፕሮግራሞች",
        L"ሌሎች ፕሮግራሞች",
        L"ለዚህ ዓይነት ፋይል መጠቀም የሚፈልጉትን መግለጫ ይጻፉ:",
        L"&የተመረጠውን ፕሮግራም ይህን ዓይነት ፋይል ለመክፈት ሁልጊዜ ተጠቀም",
        L"&አስስ...",
        L"የሚፈልጉት ፕሮግራም በዝርዝሩ ውስጥ ወይም በኮምፒውተርዎ ላይ ከሌለ፣ ተገቢውን ፕሮግራም <A ID=\"WebSearch\">በድር ላይ መፈለግ ይችላሉ</A>።",
        L"በ... ክፈት...",
        L"ፕሮግራሞች",
        L"ሁሉም ፋይሎች",
        L"እሺ",
        L"ተወው",
        L"ይህን ዓይነት ፋይል መክፈት የሚችል የተመዘገበ ፕሮግራም የለም።",
        L"የተመረጠው ፕሮግራም ፋይሉን መክፈት አልቻለም።",
    }},
    {0x042B, {  // Armenian
        L"Բացել հետ",
        L"Ընտրեք ծրագիրը, որը ցանկանում եք օգտագործել այս ֆայլը բացելու համար.",
        L"Ֆայլ՝",
        L"Խորհուրդ տրվող ծրագրեր",
        L"Այլ ծրագրեր",
        L"Մուտքագրեք նկարագրություն, որը ցանկանում եք օգտագործել այս տեսակի ֆայլի համար.",
        L"&Միշտ օգտագործել ընտրված ծրագիրը այս տեսակի ֆայլը բացելու համար",
        L"&Թերթել...",
        L"Եթե ցանկալի ծրագիրը ցանկում կամ ձեր համակարգչում չկա, կարող եք <A ID=\"WebSearch\">որոնել համապատասխան ծրագիրը համացանցում</A>։",
        L"Բացել հետ...",
        L"Ծրագրեր",
        L"Բոլոր ֆայլերը",
        L"Լավ",
        L"Չեղարկել",
        L"Ոչ մի գրանցված ծրագիր չի կարող բացել այս տեսակի ֆայլ։",
        L"Ընտրված ծրագիրը չկարողացավ բացել ֆայլը։",
    }},
    {0x044D, {  // Assamese
        L"সৈতে খোলক",
        L"এই ফাইলটো খুলিবলৈ ব্যৱহাৰ কৰিব বিচৰা প্ৰগ্ৰামটো বাছক:",
        L"ফাইল:",
        L"পৰামৰ্শিত প্ৰগ্ৰামসমূহ",
        L"অন্যান্য প্ৰগ্ৰামসমূহ",
        L"এই ধৰণৰ ফাইলৰ বাবে ব্যৱহাৰ কৰিব বিচৰা বিৱৰণ টাইপ কৰক:",
        L"&এই ধৰণৰ ফাইল খুলিবলৈ সদায় নিৰ্বাচিত প্ৰগ্ৰামটো ব্যৱহাৰ কৰক",
        L"&ব্ৰাউজ...",
        L"আপুনি বিচৰা প্ৰগ্ৰামটো তালিকাত বা আপোনাৰ কমপিউটাৰত নাথাকিলে, আপুনি <A ID=\"WebSearch\">ৱেবত উপযুক্ত প্ৰগ্ৰামটো বিচাৰিব পাৰে</A>।",
        L"সৈতে খোলক...",
        L"প্ৰগ্ৰামসমূহ",
        L"সকলো ফাইল",
        L"ঠিক আছে",
        L"বাতিল",
        L"এই ধৰণৰ ফাইল কোনো পঞ্জীয়নভুক্ত প্ৰগ্ৰামে খুলিব নোৱাৰে।",
        L"নিৰ্বাচিত প্ৰগ্ৰামটোৱে ফাইলটো খুলিব নোৱাৰিলে।",
    }},
    {0x042C, {  // Azerbaijani
        L"Birlikdə aç",
        L"Bu faylı açmaq üçün istifadə etmək istədiyiniz proqramı seçin:",
        L"Fayl:",
        L"Tövsiyə olunan proqramlar",
        L"Digər proqramlar",
        L"Bu fayl növü üçün istifadə etmək istədiyiniz təsviri yazın:",
        L"&Bu fayl növünü açmaq üçün həmişə seçilmiş proqramdan istifadə et",
        L"&Gözdən keçir...",
        L"İstədiyiniz proqram siyahıda və ya kompüterinizdə yoxdursa, <A ID=\"WebSearch\">internetdə uyğun proqram axtara bilərsiniz</A>.",
        L"Birlikdə aç...",
        L"Proqramlar",
        L"Bütün fayllar",
        L"OK",
        L"Ləğv et",
        L"Heç bir qeydiyyatlı proqram bu fayl növünü aça bilmir.",
        L"Seçilmiş proqram faylı aça bilmədi.",
    }},
    {0x046D, {  // Bashkir
        L"Менән асыу",
        L"Был файлды асыу өсөн ҡулланырға теләгән программаны һайлағыҙ:",
        L"Файл:",
        L"Тәҡдим ителгән программалар",
        L"Башҡа программалар",
        L"Был файл төрө өсөн ҡулланырға теләгән тасуирламаны яҙығыҙ:",
        L"&Был файл төрөн асыу өсөн һәр саҡ һайланған программаны ҡулланырға",
        L"&Күҙәтеү...",
        L"Теләгән программа исемлектә йәки компьютерҙа юҡ икән, <A ID=\"WebSearch\">Интернетта кәрәкле программаны эҙләй алаһығыҙ</A>.",
        L"Менән асыу...",
        L"Программалар",
        L"Бөтә файлдар",
        L"Ярай",
        L"Кире алыу",
        L"Был файл төрөн асырлыҡ теркәлгән программа юҡ.",
        L"Һайланған программа файлды аса алманы.",
    }},
    {0x042D, {  // Basque
        L"Ireki honekin",
        L"Aukeratu fitxategi hau irekitzeko erabili nahi duzun programa:",
        L"Fitxategia:",
        L"Gomendatutako programak",
        L"Beste programa batzuk",
        L"Idatzi fitxategi mota honetarako erabili nahi duzun deskribapena:",
        L"&Erabili beti hautatutako programa fitxategi mota hau irekitzeko",
        L"&Arakatu...",
        L"Nahi duzun programa zerrendan edo zure ordenagailuan ez badago, <A ID=\"WebSearch\">bilatu dezakezu programa egokia webean</A>.",
        L"Ireki honekin...",
        L"Programak",
        L"Fitxategi guztiak",
        L"Ados",
        L"Utzi",
        L"Erregistratutako programarik ezin du fitxategi mota hau ireki.",
        L"Hautatutako programak ezin izan du fitxategia ireki.",
    }},
    {0x0423, {  // Belarusian
        L"Адкрыць з дапамогай",
        L"Выберыце праграму, якую хочаце выкарыстоўваць для адкрыцця гэтага файла:",
        L"Файл:",
        L"Рэкамендаваныя праграмы",
        L"Іншыя праграмы",
        L"Увядзіце апісанне, якое хочаце выкарыстоўваць для гэтага тыпу файлаў:",
        L"&Заўсёды выкарыстоўваць выбраную праграму для адкрыцця гэтага тыпу файлаў",
        L"&Агляд...",
        L"Калі патрэбнай праграмы няма ў спісе або на вашым камп'ютары, вы можаце <A ID=\"WebSearch\">знайсці адпаведную праграму ў Інтэрнэце</A>.",
        L"Адкрыць з дапамогай...",
        L"Праграмы",
        L"Усе файлы",
        L"ОК",
        L"Адмена",
        L"Ніводная зарэгістраваная праграма не можа адкрыць гэты тып файлаў.",
        L"Выбраная праграма не змагла адкрыць файл.",
    }},
    {0x0845, {  // Bengali
        L"যা দিয়ে খুলুন",
        L"এই ফাইলটি খুলতে আপনি যে প্রোগ্রামটি ব্যবহার করতে চান তা চয়ন করুন:",
        L"ফাইল:",
        L"প্রস্তাবিত প্রোগ্রাম",
        L"অন্যান্য প্রোগ্রাম",
        L"এই ধরনের ফাইলের জন্য আপনি যে বিবরণ ব্যবহার করতে চান তা টাইপ করুন:",
        L"&এই ধরনের ফাইল খুলতে সর্বদা নির্বাচিত প্রোগ্রামটি ব্যবহার করুন",
        L"&ব্রাউজ করুন...",
        L"আপনার পছন্দের প্রোগ্রামটি তালিকায় বা আপনার কম্পিউটারে না থাকলে, আপনি <A ID=\"WebSearch\">ওয়েবে উপযুক্ত প্রোগ্রামটি খুঁজতে পারেন</A>।",
        L"যা দিয়ে খুলুন...",
        L"প্রোগ্রামসমূহ",
        L"সব ফাইল",
        L"ঠিক আছে",
        L"বাতিল",
        L"কোনো নিবন্ধিত প্রোগ্রাম এই ধরনের ফাইল খুলতে পারে না।",
        L"নির্বাচিত প্রোগ্রামটি ফাইলটি খুলতে পারেনি।",
    }},
    {0x141A, {  // Bosnian
        L"Otvori pomoću",
        L"Odaberite program koji želite koristiti za otvaranje ove datoteke:",
        L"Datoteka:",
        L"Preporučeni programi",
        L"Ostali programi",
        L"Upišite opis koji želite koristiti za ovu vrstu datoteke:",
        L"&Uvijek koristi odabrani program za otvaranje ove vrste datoteke",
        L"&Pregledaj...",
        L"Ako željeni program nije na listi ili na vašem računaru, možete <A ID=\"WebSearch\">potražiti odgovarajući program na webu</A>.",
        L"Otvori pomoću...",
        L"Programi",
        L"Sve datoteke",
        L"U redu",
        L"Otkaži",
        L"Nijedan registrovani program ne može otvoriti ovu vrstu datoteke.",
        L"Odabrani program nije mogao otvoriti datoteku.",
    }},
    {0x0403, {  // Catalan
        L"Obre amb",
        L"Trieu el programa que voleu utilitzar per obrir aquest fitxer:",
        L"Fitxer:",
        L"Programes recomanats",
        L"Altres programes",
        L"Escriviu una descripció que vulgueu utilitzar per a aquest tipus de fitxer:",
        L"&Utilitza sempre el programa seleccionat per obrir aquest tipus de fitxer",
        L"&Navega...",
        L"Si el programa que voleu no és a la llista ni a l'ordinador, podeu <A ID=\"WebSearch\">cercar el programa adequat al web</A>.",
        L"Obre amb...",
        L"Programes",
        L"Tots els fitxers",
        L"D'acord",
        L"Cancel·la",
        L"Cap programa registrat pot obrir aquest tipus de fitxer.",
        L"El programa seleccionat no ha pogut obrir el fitxer.",
    }},
    {0x045C, {  // Cherokee
        L"ᎠᏍᏚᎢᏍᏗ",
        L"ᎪᏪᎵ ᎠᏍᏚᎢᏍᏗ ᎠᏍᏆᏗᏍᎩ ᏗᏑᏰᏍᏗ:",
        L"ᎪᏪᎵ:",
        L"ᎠᏟᏍᏗᏍᎬ ᎠᏍᏆᏗᏍᎩ",
        L"ᏐᎢ ᎠᏍᏆᏗᏍᎩ",
        L"ᎯᎠ ᏗᎧᏃᏗ ᏕᎭᏬᏪᎳᏁᏗ:",
        L"&ᏂᎪᎯᎸ ᎯᎠ ᎠᏍᏆᏗᏍᎩ ᏧᏑᏰᏛ ᎪᏪᎵ ᎠᏍᏚᎢᏍᏗᏱ",
        L"&ᎠᏑᏰᏍᏗ...",
        L"ᎣᏍᏛ ᎠᏍᏆᏗᏍᎩ Ꮭ ᏱᎬᏩᏂᎸᏍᎦ, <A ID=\"WebSearch\">ᏫᏂᎦᏛᏗᏍᏗ</A>.",
        L"ᎠᏍᏚᎢᏍᏗ...",
        L"ᎠᏍᏆᏗᏍᎩ",
        L"ᏂᎦᏛ ᎪᏪᎵ",
        L"ᎰᏩ",
        L"ᏫᎦᏅᏓᏗᏍᏗ",
        L"ᎪᏪᎵ ᎯᎠ ᏗᎧᏃᏗ ᎠᏍᏚᎢᏍᏗᏱ ᎠᏍᏆᏗᏍᎩ Ꮭ ᏱᎨᏒ",
        L"ᎯᎠ ᎠᏍᏆᏗᏍᎩ ᏧᏑᏰᏛ ᎪᏪᎵ ᎠᏍᏚᎢᏍᏗᏱ Ꮭ ᏱᎬᏩᎵᏍᏓᏁᎸᎦ",
    }},
    {0x048C, {  // Dari
        L"باز کردن با",
        L"برنامه\u200cای را که می\u200cخواهید برای باز کردن این فایل استفاده کنید انتخاب کنید:",
        L"فایل:",
        L"برنامه\u200cهای پیشنهادشده",
        L"برنامه\u200cهای دیگر",
        L"توضیحی را که می\u200cخواهید برای این نوع فایل استفاده کنید تایپ کنید:",
        L"&همیشه از برنامه انتخاب\u200cشده برای باز کردن این نوع فایل استفاده کن",
        L"&مرور...",
        L"اگر برنامه\u200cای که می\u200cخواهید در فهرست یا در رایانه شما نیست، می\u200cتوانید <A ID=\"WebSearch\">برنامه مناسب را در وب جستجو کنید</A>.",
        L"باز کردن با...",
        L"برنامه\u200cها",
        L"همه فایل\u200cها",
        L"تأیید",
        L"لغو",
        L"هیچ برنامه ثبت\u200cشده\u200cای نمی\u200cتواند این نوع فایل را باز کند.",
        L"برنامه انتخاب\u200cشده نتوانست فایل را باز کند.",
    }},
    {0x0465, {  // Divehi
        L"ހުޅުވާލުން",
        L"މި ފައިލް ހުޅުވުމަށް ބޭނުންވާ ޕްރޮގްރާމް އިޚްތިޔާރުކުރައްވާ:",
        L"ފައިލް:",
        L"ލަފާދޭ ޕްރޮގްރާމްތައް",
        L"އެހެން ޕްރޮގްރާމްތައް",
        L"މި ވައްތަރުގެ ފައިލަށް ބޭނުންކުރަން އެދޭ ތަފްސީލު ލިޔުއްވާ:",
        L"&މި ވައްތަރުގެ ފައިލް ހުޅުވުމަށް އަބަދުވެސް އިޚްތިޔާރުކުރެވުނު ޕްރޮގްރާމް ބޭނުންކުރޭ",
        L"&ބްރައުޒް...",
        L"އެދޭ ޕްރޮގްރާމް ލިސްޓުގައި ނުވަތަ ކޮމްޕިއުޓަރުގައި ނެތްނަމަ، <A ID=\"WebSearch\">ވެބްގައި އެކަށޭނެ ޕްރޮގްރާމެއް ހޯދެވިދާނެ</A>ެވެ.",
        L"ހުޅުވާލުން...",
        L"ޕްރޮގްރާމްތައް",
        L"ހުރިހާ ފައިލްތައް",
        L"އޯކޭ",
        L"ބާތިލުކުރޭ",
        L"މި ވައްތަރުގެ ފައިލް ހުޅުވޭނެ ރަޖިސްޓްރީކޮށްފައިވާ ޕްރޮގްރާމެއް ނެތެވެ.",
        L"އިޚްތިޔާރުކުރެވުނު ޕްރޮގްރާމަކަށް ފައިލް ހުޅުވޭގޮތެއް ނުވިއެވެ.",
    }},
    {0x0425, {  // Estonian
        L"Ava rakendusega",
        L"Valige programm, millega soovite selle faili avada:",
        L"Fail:",
        L"Soovitatavad programmid",
        L"Muud programmid",
        L"Tippige kirjeldus, mida soovite seda tüüpi faili puhul kasutada:",
        L"&Kasuta alati valitud programmi seda tüüpi faili avamiseks",
        L"&Sirvi...",
        L"Kui soovitud programm ei ole loendis ega teie arvutis, saate <A ID=\"WebSearch\">otsida sobivat programmi veebist</A>.",
        L"Ava rakendusega...",
        L"Programmid",
        L"Kõik failid",
        L"OK",
        L"Loobu",
        L"Ükski registreeritud programm ei saa seda tüüpi faili avada.",
        L"Valitud programm ei suutnud faili avada.",
    }},
    {0x0464, {  // Filipino
        L"Buksan gamit",
        L"Piliin ang program na gusto mong gamitin para buksan ang file na ito:",
        L"File:",
        L"Mga Inirerekomendang Program",
        L"Iba Pang Mga Program",
        L"I-type ang paglalarawang gusto mong gamitin para sa ganitong uri ng file:",
        L"&Palaging gamitin ang napiling program para buksan ang ganitong uri ng file",
        L"&Mag-browse...",
        L"Kung wala sa listahan o sa iyong computer ang program na gusto mo, maaari kang <A ID=\"WebSearch\">maghanap sa Web ng angkop na program</A>.",
        L"Buksan gamit...",
        L"Mga Program",
        L"Lahat ng File",
        L"OK",
        L"Kanselahin",
        L"Walang nakarehistrong program na makakapagbukas ng ganitong uri ng file.",
        L"Hindi mabuksan ng napiling program ang file.",
    }},
    {0x0462, {  // Western Frisian
        L"Iepenet mei",
        L"Kies it programma dat jo brûke wolle om dit bestân te iepenjen:",
        L"Bestân:",
        L"Oanrikkemandearre programma's",
        L"Oare programma's",
        L"Typ in beskriuwing dy't jo brûke wolle foar dit soarte bestân:",
        L"&Brûk altyd it selektearre programma om dit soarte bestân te iepenjen",
        L"&Blêdzje...",
        L"As it winske programma net yn 'e list of op jo kompjûter stiet, kinne jo <A ID=\"WebSearch\">op it web sykje nei it passende programma</A>.",
        L"Iepenet mei...",
        L"Programma's",
        L"Alle bestannen",
        L"OK",
        L"Annulearje",
        L"Gjin registrearre programma kin dit soarte bestân iepenje.",
        L"It selektearre programma koe it bestân net iepenje.",
    }},
    {0x0456, {  // Galician
        L"Abrir con",
        L"Escolla o programa que quere usar para abrir este ficheiro:",
        L"Ficheiro:",
        L"Programas recomendados",
        L"Outros programas",
        L"Escriba unha descrición que queira usar para este tipo de ficheiro:",
        L"&Usar sempre o programa seleccionado para abrir este tipo de ficheiro",
        L"&Examinar...",
        L"Se o programa que quere non está na lista nin no seu computador, pode <A ID=\"WebSearch\">buscar o programa axeitado na web</A>.",
        L"Abrir con...",
        L"Programas",
        L"Todos os ficheiros",
        L"Aceptar",
        L"Cancelar",
        L"Ningún programa rexistrado pode abrir este tipo de ficheiro.",
        L"O programa seleccionado non puido abrir o ficheiro.",
    }},
    {0x0437, {  // Georgian
        L"გახსნა პროგრამით",
        L"აირჩიეთ პროგრამა, რომლითაც გსურთ ამ ფაილის გახსნა:",
        L"ფაილი:",
        L"რეკომენდებული პროგრამები",
        L"სხვა პროგრამები",
        L"შეიყვანეთ აღწერა, რომლის გამოყენებაც გსურთ ამ ტიპის ფაილისთვის:",
        L"&ყოველთვის გამოიყენე არჩეული პროგრამა ამ ტიპის ფაილის გასახსნელად",
        L"&დათვალიერება...",
        L"თუ სასურველი პროგრამა არ არის სიაში ან თქვენს კომპიუტერში, შეგიძლიათ <A ID=\"WebSearch\">მოძებნოთ შესაფერისი პროგრამა ინტერნეტში</A>.",
        L"გახსნა პროგრამით...",
        L"პროგრამები",
        L"ყველა ფაილი",
        L"კარგი",
        L"გაუქმება",
        L"ვერცერთი რეგისტრირებული პროგრამა ხსნის ამ ტიპის ფაილს.",
        L"არჩეულმა პროგრამამ ვერ გახსნა ფაილი.",
    }},
    {0x0447, {  // Gujarati
        L"આનાથી ખોલો",
        L"આ ફાઇલ ખોલવા માટે તમે જે પ્રોગ્રામનો ઉપયોગ કરવા માંગો છો તે પસંદ કરો:",
        L"ફાઇલ:",
        L"ભલામણ કરેલા પ્રોગ્રામ્સ",
        L"અન્ય પ્રોગ્રામ્સ",
        L"આ પ્રકારની ફાઇલ માટે તમે જે વર્ણનનો ઉપયોગ કરવા માંગો છો તે લખો:",
        L"&આ પ્રકારની ફાઇલ ખોલવા માટે હંમેશાં પસંદ કરેલા પ્રોગ્રામનો ઉપયોગ કરો",
        L"&બ્રાઉઝ કરો...",
        L"તમને જોઈતો પ્રોગ્રામ સૂચિમાં કે તમારા કમ્પ્યુટર પર ન હોય તો, તમે <A ID=\"WebSearch\">વેબ પર યોગ્ય પ્રોગ્રામ શોધી શકો છો</A>.",
        L"આનાથી ખોલો...",
        L"પ્રોગ્રામ્સ",
        L"બધી ફાઇલો",
        L"ઠીક છે",
        L"રદ કરો",
        L"કોઈ નોંધાયેલ પ્રોગ્રામ આ પ્રકારની ફાઇલ ખોલી શકતો નથી.",
        L"પસંદ કરેલો પ્રોગ્રામ ફાઇલ ખોલી શક્યો નહીં.",
    }},
    {0x0468, {  // Hausa
        L"Buɗe da",
        L"Zaɓi shirin da kake so ka yi amfani da shi don buɗe wannan fayil:",
        L"Fayil:",
        L"Shirye-shiryen da aka ba da shawarar",
        L"Sauran shirye-shirye",
        L"Buga bayanin da kake so ka yi amfani da shi ga irin wannan fayil:",
        L"&Kullum yi amfani da shirin da aka zaɓa don buɗe irin wannan fayil",
        L"&Bincika...",
        L"Idan shirin da kake so ba ya cikin jerin ko a kan kwamfutarka, za ka iya <A ID=\"WebSearch\">neman shirin da ya dace a yanar gizo</A>.",
        L"Buɗe da...",
        L"Shirye-shirye",
        L"Duk fayiloli",
        L"OK",
        L"Soke",
        L"Babu wani shiri mai rijista da zai iya buɗe irin wannan fayil.",
        L"Shirin da aka zaɓa bai iya buɗe fayil ɗin ba.",
    }},
    {0x0439, {  // Hindi
        L"इसके साथ खोलें",
        L"इस फ़ाइल को खोलने के लिए आप जिस प्रोग्राम का उपयोग करना चाहते हैं उसे चुनें:",
        L"फ़ाइल:",
        L"अनुशंसित प्रोग्राम",
        L"अन्य प्रोग्राम",
        L"इस प्रकार की फ़ाइल के लिए आप जो विवरण उपयोग करना चाहते हैं उसे टाइप करें:",
        L"&इस प्रकार की फ़ाइल खोलने के लिए हमेशा चयनित प्रोग्राम का उपयोग करें",
        L"&ब्राउज़ करें...",
        L"अगर आपका इच्छित प्रोग्राम सूची में या आपके कंप्यूटर पर नहीं है, तो आप <A ID=\"WebSearch\">वेब पर उपयुक्त प्रोग्राम खोज सकते हैं</A>।",
        L"इसके साथ खोलें...",
        L"प्रोग्राम",
        L"सभी फ़ाइलें",
        L"ठीक है",
        L"रद्द करें",
        L"कोई पंजीकृत प्रोग्राम इस प्रकार की फ़ाइल नहीं खोल सकता।",
        L"चयनित प्रोग्राम फ़ाइल नहीं खोल सका।",
    }},
    {0x040F, {  // Icelandic
        L"Opna með",
        L"Veldu forritið sem þú vilt nota til að opna þessa skrá:",
        L"Skrá:",
        L"Mælt er með þessum forritum",
        L"Önnur forrit",
        L"Sláðu inn lýsingu sem þú vilt nota fyrir þessa tegund skráa:",
        L"&Nota alltaf valið forrit til að opna þessa tegund skráa",
        L"&Fletta...",
        L"Ef forritið sem þú vilt er ekki á listanum eða í tölvunni þinni geturðu <A ID=\"WebSearch\">leitað að viðeigandi forriti á vefnum</A>.",
        L"Opna með...",
        L"Forrit",
        L"Allar skrár",
        L"Í lagi",
        L"Hætta við",
        L"Ekkert skráð forrit getur opnað þessa tegund skráa.",
        L"Valið forrit gat ekki opnað skrána.",
    }},
    {0x0470, {  // Igbo
        L"Jiri mepee",
        L"Họrọ mmemme ị chọrọ iji mepee faịlụ a:",
        L"Faịlụ:",
        L"Mmemme ndị atụrụ aro",
        L"Mmemme ndị ọzọ",
        L"Dee nkọwa ị chọrọ iji maka ụdị faịlụ a:",
        L"&Jiri mmemme ahọpụtara mepee ụdị faịlụ a mgbe niile",
        L"&Chọgharịa...",
        L"Ọ bụrụ na mmemme ị chọrọ anọghị na ndepụta ma ọ bụ na kọmputa gị, ị nwere ike <A ID=\"WebSearch\">chọọ mmemme kwesịrị ekwesị na Weebụ</A>.",
        L"Jiri mepee...",
        L"Mmemme",
        L"Faịlụ niile",
        L"Ọ dị mma",
        L"Kagbuo",
        L"Ọ dịghị mmemme edebanyere aha nwere ike imepe ụdị faịlụ a.",
        L"Mmemme ahọpụtara enweghị ike imepe faịlụ ahụ.",
    }},
    {0x083C, {  // Irish
        L"Oscail le",
        L"Roghnaigh an clár is mian leat a úsáid chun an comhad seo a oscailt:",
        L"Comhad:",
        L"Cláir Mholta",
        L"Cláir Eile",
        L"Clóscríobh cur síos is mian leat a úsáid don chineál seo comhaid:",
        L"&Úsáid an clár roghnaithe i gcónaí chun an cineál seo comhaid a oscailt",
        L"&Brabhsáil...",
        L"Mura bhfuil an clár atá uait ar an liosta nó ar do ríomhaire, is féidir leat <A ID=\"WebSearch\">cuardach a dhéanamh ar an nGréasán don chlár cuí</A>.",
        L"Oscail le...",
        L"Cláir",
        L"Gach Comhad",
        L"OK",
        L"Cealaigh",
        L"Ní féidir le haon chlár cláraithe an cineál seo comhaid a oscailt.",
        L"Níorbh fhéidir leis an gclár roghnaithe an comhad a oscailt.",
    }},
    {0x0434, {  // Xhosa
        L"Vula nge",
        L"Khetha inkqubo ofuna ukuyisebenzisa ukuvula le fayile:",
        L"Ifayile:",
        L"Iinkqubo ezicetyiswayo",
        L"Ezinye iinkqubo",
        L"Chwetheza inkcazo ofuna ukuyisebenzisa kolu hlobo lwefayile:",
        L"&Soloko usebenzisa inkqubo ekhethiweyo ukuvula olu hlobo lwefayile",
        L"&Khangela...",
        L"Ukuba inkqubo oyifunayo ayikho kuluhlu okanye kwikhompyutha yakho, unako <A ID=\"WebSearch\">ukukhangela inkqubo efanelekileyo kwiWebhu</A>.",
        L"Vula nge...",
        L"Iinkqubo",
        L"Zonke iifayile",
        L"Kulungile",
        L"Rhoxisa",
        L"Akukho nkqubo ibhalisiweyo inokuvula olu hlobo lwefayile.",
        L"Inkqubo ekhethiweyo ayikwazanga ukuvula ifayile.",
    }},
    {0x0435, {  // Zulu
        L"Vula nge",
        L"Khetha uhlelo ofuna ukulusebenzisa ukuze uvule le fayela:",
        L"Ifayela:",
        L"Izinhlelo ezinconywayo",
        L"Ezinye izinhlelo",
        L"Thayipha incazelo ofuna ukuyisebenzisa kulolu hlobo lwefayela:",
        L"&Sebenzisa njalo uhlelo olukhethiwe ukuze uvule lolu hlobo lwefayela",
        L"&Phequlula...",
        L"Uma uhlelo olufunayo lungekho ohlwini noma kukhompyutha yakho, ungakwazi <A ID=\"WebSearch\">ukufuna uhlelo olufanele kuwebhu</A>.",
        L"Vula nge...",
        L"Izinhlelo",
        L"Wonke amafayela",
        L"Kulungile",
        L"Khansela",
        L"Alukho uhlelo olubhalisiwe olungavula lolu hlobo lwefayela.",
        L"Uhlelo olukhethiwe alukwazanga ukuvula ifayela.",
    }},
    {0x044B, {  // Kannada
        L"ಇದರೊಂದಿಗೆ ತೆರೆಯಿರಿ",
        L"ಈ ಫೈಲ್ ತೆರೆಯಲು ನೀವು ಬಳಸಲು ಬಯಸುವ ಪ್ರೋಗ್ರಾಂ ಅನ್ನು ಆಯ್ಕೆಮಾಡಿ:",
        L"ಫೈಲ್:",
        L"ಶಿಫಾರಸು ಮಾಡಲಾದ ಪ್ರೋಗ್ರಾಂಗಳು",
        L"ಇತರ ಪ್ರೋಗ್ರಾಂಗಳು",
        L"ಈ ರೀತಿಯ ಫೈಲ್\u200cಗಾಗಿ ನೀವು ಬಳಸಲು ಬಯಸುವ ವಿವರಣೆಯನ್ನು ಟೈಪ್ ಮಾಡಿ:",
        L"&ಈ ರೀತಿಯ ಫೈಲ್ ತೆರೆಯಲು ಯಾವಾಗಲೂ ಆಯ್ಕೆಮಾಡಿದ ಪ್ರೋಗ್ರಾಂ ಬಳಸಿ",
        L"&ಬ್ರೌಸ್ ಮಾಡಿ...",
        L"ನಿಮಗೆ ಬೇಕಾದ ಪ್ರೋಗ್ರಾಂ ಪಟ್ಟಿಯಲ್ಲಿ ಅಥವಾ ನಿಮ್ಮ ಕಂಪ್ಯೂಟರ್\u200cನಲ್ಲಿ ಇಲ್ಲದಿದ್ದರೆ, ನೀವು <A ID=\"WebSearch\">ವೆಬ್\u200cನಲ್ಲಿ ಸೂಕ್ತ ಪ್ರೋಗ್ರಾಂ ಹುಡುಕಬಹುದು</A>.",
        L"ಇದರೊಂದಿಗೆ ತೆರೆಯಿರಿ...",
        L"ಪ್ರೋಗ್ರಾಂಗಳು",
        L"ಎಲ್ಲಾ ಫೈಲ್\u200cಗಳು",
        L"ಸರಿ",
        L"ರದ್ದುಮಾಡಿ",
        L"ಯಾವುದೇ ನೋಂದಾಯಿತ ಪ್ರೋಗ್ರಾಂ ಈ ರೀತಿಯ ಫೈಲ್ ತೆರೆಯಲು ಸಾಧ್ಯವಿಲ್ಲ.",
        L"ಆಯ್ಕೆಮಾಡಿದ ಪ್ರೋಗ್ರಾಂ ಫೈಲ್ ತೆರೆಯಲು ಸಾಧ್ಯವಾಗಲಿಲ್ಲ.",
    }},
    {0x043F, {  // Kazakh
        L"Көмегімен ашу",
        L"Осы файлды ашу үшін пайдаланғыңыз келетін бағдарламаны таңдаңыз:",
        L"Файл:",
        L"Ұсынылатын бағдарламалар",
        L"Басқа бағдарламалар",
        L"Файлдың осы түрі үшін пайдаланғыңыз келетін сипаттаманы енгізіңіз:",
        L"&Файлдың осы түрін ашу үшін әрқашан таңдалған бағдарламаны пайдалану",
        L"&Шолу...",
        L"Қалаған бағдарлама тізімде немесе компьютеріңізде болмаса, <A ID=\"WebSearch\">Интернеттен тиісті бағдарламаны іздей аласыз</A>.",
        L"Көмегімен ашу...",
        L"Бағдарламалар",
        L"Барлық файлдар",
        L"Жарайды",
        L"Бас тарту",
        L"Тіркелген ешбір бағдарлама файлдың осы түрін аша алмайды.",
        L"Таңдалған бағдарлама файлды аша алмады.",
    }},
    {0x0453, {  // Khmer
        L"បើកជាមួយ",
        L"ជ្រើសរើសកម្មវិធីដែលអ្នកចង់ប្រើដើម្បីបើកឯកសារនេះ៖",
        L"ឯកសារ៖",
        L"កម្មវិធីដែលបានណែនាំ",
        L"កម្មវិធីផ្សេងទៀត",
        L"វាយបញ្ចូលការពិពណ៌នាដែលអ្នកចង់ប្រើសម្រាប់ឯកសារប្រភេទនេះ៖",
        L"&តែងតែប្រើកម្មវិធីដែលបានជ្រើសរើសដើម្បីបើកឯកសារប្រភេទនេះ",
        L"&រកមើល...",
        L"ប្រសិនបើកម្មវិធីដែលអ្នកចង់បានមិននៅក្នុងបញ្ជី ឬនៅលើកុំព្យូទ័ររបស់អ្នក អ្នកអាច <A ID=\"WebSearch\">ស្វែងរកកម្មវិធីដែលសមរម្យនៅលើវេប</A>។",
        L"បើកជាមួយ...",
        L"កម្មវិធី",
        L"ឯកសារទាំងអស់",
        L"យល់ព្រម",
        L"បោះបង់",
        L"គ្មានកម្មវិធីដែលបានចុះឈ្មោះអាចបើកឯកសារប្រភេទនេះទេ។",
        L"កម្មវិធីដែលបានជ្រើសរើសមិនអាចបើកឯកសារបានទេ។",
    }},
    {0x0487, {  // Kinyarwanda
        L"Fungura ukoresheje",
        L"Hitamo porogaramu ushaka gukoresha kugira ngo ufungure iyi dosiye:",
        L"Dosiye:",
        L"Porogaramu zasabwe",
        L"Izindi porogaramu",
        L"Andika ibisobanuro ushaka gukoresha kuri ubu bwoko bwa dosiye:",
        L"&Ukoreshe buri gihe porogaramu wahisemo kugira ngo ufungure ubu bwoko bwa dosiye",
        L"&Shakisha...",
        L"Niba porogaramu ushaka itari ku rutonde cyangwa kuri mudasobwa yawe, ushobora <A ID=\"WebSearch\">gushaka porogaramu ikwiriye kuri interineti</A>.",
        L"Fungura ukoresheje...",
        L"Porogaramu",
        L"Dosiye zose",
        L"Sawa",
        L"Hagarika",
        L"Nta porogaramu yanditswe ishobora gufungura ubu bwoko bwa dosiye.",
        L"Porogaramu wahisemo ntiyashoboye gufungura dosiye.",
    }},
    {0x0441, {  // Kiswahili
        L"Fungua kwa",
        L"Chagua programu unayotaka kutumia kufungua faili hii:",
        L"Faili:",
        L"Programu zinazopendekezwa",
        L"Programu nyingine",
        L"Andika maelezo unayotaka kutumia kwa aina hii ya faili:",
        L"&Tumia kila wakati programu iliyochaguliwa kufungua aina hii ya faili",
        L"&Vinjari...",
        L"Ikiwa programu unayotaka haiko kwenye orodha au kwenye kompyuta yako, unaweza <A ID=\"WebSearch\">kutafuta programu sahihi kwenye wavuti</A>.",
        L"Fungua kwa...",
        L"Programu",
        L"Faili zote",
        L"Sawa",
        L"Ghairi",
        L"Hakuna programu iliyosajiliwa inayoweza kufungua aina hii ya faili.",
        L"Programu iliyochaguliwa haikuweza kufungua faili.",
    }},
    {0x0457, {  // Konkani
        L"हाचे सयत उगड",
        L"ही फायल उगडपाक तुमकां जो प्रोग्राम वापरपाचो आसा तो वेंचात:",
        L"फायल:",
        L"शिफारस केल्ले प्रोग्राम",
        L"हेर प्रोग्राम",
        L"ह्या प्रकारचे फायले खातीर तुमकां वापरपाचें वर्णन बरयात:",
        L"&ह्या प्रकारची फायल उगडपाक सदांच वेंचिल्लो प्रोग्राम वापरात",
        L"&न्याळात...",
        L"तुमकां जाय तो प्रोग्राम यादींत वा तुमच्या कॉम्प्युटराचेर ना जाल्यार, तुमी <A ID=\"WebSearch\">वेबाचेर योग्य प्रोग्राम सोदूंक शकतात</A>.",
        L"हाचे सयत उगड...",
        L"प्रोग्राम",
        L"सगळ्यो फायली",
        L"बरो",
        L"रद्द करात",
        L"ह्या प्रकारची फायल उगडपाक कोणोच नोंदणीकृत प्रोग्राम ना.",
        L"वेंचिल्ल्या प्रोग्रामाक फायल उगडपाक जमलें ना.",
    }},
    {0x0440, {  // Kyrgyz
        L"Менен ачуу",
        L"Бул файлды ачуу үчүн колдонгуңуз келген программаны тандаңыз:",
        L"Файл:",
        L"Сунушталган программалар",
        L"Башка программалар",
        L"Файлдын бул түрү үчүн колдонгуңуз келген сүрөттөмөнү жазыңыз:",
        L"&Файлдын бул түрүн ачуу үчүн ар дайым тандалган программаны колдонуу",
        L"&Карап чыгуу...",
        L"Каалаган программа тизмеде же компьютериңизде жок болсо, <A ID=\"WebSearch\">Интернеттен ылайыктуу программаны издей аласыз</A>.",
        L"Менен ачуу...",
        L"Программалар",
        L"Бардык файлдар",
        L"Макул",
        L"Жокко чыгаруу",
        L"Файлдын бул түрүн эч бир катталган программа ача албайт.",
        L"Тандалган программа файлды ача алган жок.",
    }},
    {0x0454, {  // Lao
        L"ເປີດດ້ວຍ",
        L"ເລືອກໂປຣແກຣມທີ່ທ່ານຕ້ອງການໃຊ້ເພື່ອເປີດໄຟລ໌ນີ້:",
        L"ໄຟລ໌:",
        L"ໂປຣແກຣມທີ່ແນະນຳ",
        L"ໂປຣແກຣມອື່ນໆ",
        L"ພິມຄຳອະທິບາຍທີ່ທ່ານຕ້ອງການໃຊ້ສຳລັບໄຟລ໌ປະເພດນີ້:",
        L"&ໃຊ້ໂປຣແກຣມທີ່ເລືອກໄວ້ສະເໝີເພື່ອເປີດໄຟລ໌ປະເພດນີ້",
        L"&ຊອກຫາ...",
        L"ຖ້າໂປຣແກຣມທີ່ທ່ານຕ້ອງການບໍ່ຢູ່ໃນລາຍຊື່ ຫຼື ຢູ່ໃນຄອມພິວເຕີຂອງທ່ານ, ທ່ານສາມາດ <A ID=\"WebSearch\">ຊອກຫາໂປຣແກຣມທີ່ເໝາະສົມໃນເວັບ</A> ໄດ້.",
        L"ເປີດດ້ວຍ...",
        L"ໂປຣແກຣມ",
        L"ໄຟລ໌ທັງໝົດ",
        L"ຕົກລົງ",
        L"ຍົກເລີກ",
        L"ບໍ່ມີໂປຣແກຣມທີ່ລົງທະບຽນໄວ້ສາມາດເປີດໄຟລ໌ປະເພດນີ້ໄດ້.",
        L"ໂປຣແກຣມທີ່ເລືອກໄວ້ບໍ່ສາມາດເປີດໄຟລ໌ໄດ້.",
    }},
    {0x0426, {  // Latvian
        L"Atvērt ar",
        L"Izvēlieties programmu, kuru vēlaties izmantot, lai atvērtu šo failu:",
        L"Fails:",
        L"Ieteicamās programmas",
        L"Citas programmas",
        L"Ierakstiet aprakstu, kuru vēlaties izmantot šāda veida failam:",
        L"&Vienmēr izmantot atlasīto programmu, lai atvērtu šāda veida failu",
        L"&Pārlūkot...",
        L"Ja vēlamā programma nav sarakstā vai datorā, varat <A ID=\"WebSearch\">meklēt piemērotu programmu tīmeklī</A>.",
        L"Atvērt ar...",
        L"Programmas",
        L"Visi faili",
        L"Labi",
        L"Atcelt",
        L"Neviena reģistrēta programma nevar atvērt šāda veida failu.",
        L"Atlasītā programma nevarēja atvērt failu.",
    }},
    {0x0427, {  // Lithuanian
        L"Atidaryti naudojant",
        L"Pasirinkite programą, kurią norite naudoti šiam failui atidaryti:",
        L"Failas:",
        L"Rekomenduojamos programos",
        L"Kitos programos",
        L"Įveskite aprašą, kurį norite naudoti šio tipo failui:",
        L"&Visada naudoti pasirinktą programą šio tipo failui atidaryti",
        L"&Naršyti...",
        L"Jei norimos programos nėra sąraše ar kompiuteryje, galite <A ID=\"WebSearch\">ieškoti tinkamos programos internete</A>.",
        L"Atidaryti naudojant...",
        L"Programos",
        L"Visi failai",
        L"Gerai",
        L"Atšaukti",
        L"Nė viena registruota programa negali atidaryti šio tipo failo.",
        L"Pasirinkta programa negalėjo atidaryti failo.",
    }},
    {0x046E, {  // Luxembourgish
        L"Oppe mat",
        L"Wielt de Programm, deen Dir benotze wëllt, fir dëse Fichier opzemaachen:",
        L"Fichier:",
        L"Empfohlene Programmer",
        L"Aner Programmer",
        L"Gitt eng Beschreiwung an, déi Dir fir dës Zort Fichier benotze wëllt:",
        L"&Benotzt ëmmer de gewielte Programm, fir dës Zort Fichier opzemaachen",
        L"&Bliederen...",
        L"Wann de gewënschte Programm net an der Lëscht oder op Ärem Computer ass, kënnt Dir <A ID=\"WebSearch\">um Web no dem passende Programm sichen</A>.",
        L"Oppe mat...",
        L"Programmer",
        L"All Fichieren",
        L"OK",
        L"Ofbriechen",
        L"Kee registréierte Programm kann dës Zort Fichier opmaachen.",
        L"De gewielte Programm konnt de Fichier net opmaachen.",
    }},
    {0x042F, {  // Macedonian
        L"Отвори со",
        L"Изберете ја програмата што сакате да ја користите за да ја отворите оваа датотека:",
        L"Датотека:",
        L"Препорачани програми",
        L"Други програми",
        L"Внесете опис што сакате да го користите за овој вид датотека:",
        L"&Секогаш користи ја избраната програма за отворање на овој вид датотека",
        L"&Прегледај...",
        L"Ако саканата програма не е во списокот или на вашиот компјутер, можете да <A ID=\"WebSearch\">ја побарате соодветната програма на интернет</A>.",
        L"Отвори со...",
        L"Програми",
        L"Сите датотеки",
        L"Во ред",
        L"Откажи",
        L"Ниту една регистрирана програма не може да го отвори овој вид датотека.",
        L"Избраната програма не можеше да ја отвори датотеката.",
    }},
    {0x043E, {  // Malay
        L"Buka dengan",
        L"Pilih program yang anda mahu gunakan untuk membuka fail ini:",
        L"Fail:",
        L"Program Disyorkan",
        L"Program Lain",
        L"Taip perihalan yang anda mahu gunakan untuk jenis fail ini:",
        L"&Sentiasa gunakan program yang dipilih untuk membuka jenis fail ini",
        L"&Semak imbas...",
        L"Jika program yang anda mahu tiada dalam senarai atau pada komputer anda, anda boleh <A ID=\"WebSearch\">mencari program yang sesuai di Web</A>.",
        L"Buka dengan...",
        L"Program",
        L"Semua Fail",
        L"OK",
        L"Batal",
        L"Tiada program berdaftar boleh membuka jenis fail ini.",
        L"Program yang dipilih tidak dapat membuka fail.",
    }},
    {0x044C, {  // Malayalam
        L"ഇതുപയോഗിച്ച് തുറക്കുക",
        L"ഈ ഫയൽ തുറക്കാൻ ഉപയോഗിക്കേണ്ട പ്രോഗ്രാം തിരഞ്ഞെടുക്കുക:",
        L"ഫയൽ:",
        L"ശുപാർശ ചെയ്യുന്ന പ്രോഗ്രാമുകൾ",
        L"മറ്റ് പ്രോഗ്രാമുകൾ",
        L"ഇത്തരം ഫയലിനായി ഉപയോഗിക്കേണ്ട വിവരണം ടൈപ്പ് ചെയ്യുക:",
        L"&ഇത്തരം ഫയൽ തുറക്കാൻ എല്ലായ്പ്പോഴും തിരഞ്ഞെടുത്ത പ്രോഗ്രാം ഉപയോഗിക്കുക",
        L"&ബ്രൗസ് ചെയ്യുക...",
        L"നിങ്ങൾക്ക് വേണ്ട പ്രോഗ്രാം പട്ടികയിലോ നിങ്ങളുടെ കമ്പ്യൂട്ടറിലോ ഇല്ലെങ്കിൽ, <A ID=\"WebSearch\">വെബിൽ അനുയോജ്യമായ പ്രോഗ്രാം തിരയാം</A>.",
        L"ഇതുപയോഗിച്ച് തുറക്കുക...",
        L"പ്രോഗ്രാമുകൾ",
        L"എല്ലാ ഫയലുകളും",
        L"ശരി",
        L"റദ്ദാക്കുക",
        L"ഇത്തരം ഫയൽ തുറക്കാൻ കഴിയുന്ന രജിസ്റ്റർ ചെയ്ത പ്രോഗ്രാമില്ല.",
        L"തിരഞ്ഞെടുത്ത പ്രോഗ്രാമിന് ഫയൽ തുറക്കാൻ കഴിഞ്ഞില്ല.",
    }},
    {0x043A, {  // Maltese
        L"Iftaħ ma'",
        L"Agħżel il-programm li tixtieq tuża biex tiftaħ dan il-fajl:",
        L"Fajl:",
        L"Programmi Rakkomandati",
        L"Programmi Oħra",
        L"Ittajpja deskrizzjoni li tixtieq tuża għal dan it-tip ta' fajl:",
        L"&Uża dejjem il-programm magħżul biex tiftaħ dan it-tip ta' fajl",
        L"&Fittex...",
        L"Jekk il-programm li tixtieq mhuwiex fil-lista jew fuq il-kompjuter tiegħek, tista' <A ID=\"WebSearch\">tfittex il-programm xieraq fuq il-Web</A>.",
        L"Iftaħ ma'...",
        L"Programmi",
        L"Il-Fajls Kollha",
        L"OK",
        L"Ikkanċella",
        L"Ebda programm irreġistrat ma jista' jiftaħ dan it-tip ta' fajl.",
        L"Il-programm magħżul ma setax jiftaħ il-fajl.",
    }},
    {0x0481, {  // Maori
        L"Huaki me",
        L"Kōwhiria te papatono e hiahia ana koe ki te whakamahi hei huaki i tēnei kōnae:",
        L"Kōnae:",
        L"Ngā Papatono Tūtohutia",
        L"Ētahi Atu Papatono",
        L"Patohia he whakaahuatanga e hiahia ana koe mō tēnei momo kōnae:",
        L"&Whakamahia tonutia te papatono kua kōwhiria hei huaki i tēnei momo kōnae",
        L"&Tirotiro...",
        L"Ki te kore te papatono e hiahiatia ana kei te rārangi, kei tō rorohiko rānei, ka taea e koe te <A ID=\"WebSearch\">rapu i te papatono tika i te Tukutuku</A>.",
        L"Huaki me...",
        L"Ngā Papatono",
        L"Ngā Kōnae Katoa",
        L"Āe",
        L"Whakakore",
        L"Kāore he papatono rēhita e taea te huaki i tēnei momo kōnae.",
        L"Kīhai i taea e te papatono kua kōwhiria te kōnae te huaki.",
    }},
    {0x044E, {  // Marathi
        L"याद्वारे उघडा",
        L"ही फाईल उघडण्यासाठी तुम्हाला वापरायचा असलेला प्रोग्राम निवडा:",
        L"फाईल:",
        L"शिफारस केलेले प्रोग्राम",
        L"इतर प्रोग्राम",
        L"या प्रकारच्या फाईलसाठी तुम्हाला वापरायचे असलेले वर्णन टाइप करा:",
        L"&या प्रकारची फाईल उघडण्यासाठी नेहमी निवडलेला प्रोग्राम वापरा",
        L"&ब्राउझ करा...",
        L"तुम्हाला हवा असलेला प्रोग्राम सूचीमध्ये किंवा तुमच्या संगणकावर नसल्यास, तुम्ही <A ID=\"WebSearch\">वेबवर योग्य प्रोग्राम शोधू शकता</A>.",
        L"याद्वारे उघडा...",
        L"प्रोग्राम",
        L"सर्व फाईल्स",
        L"ठीक आहे",
        L"रद्द करा",
        L"कोणताही नोंदणीकृत प्रोग्राम या प्रकारची फाईल उघडू शकत नाही.",
        L"निवडलेला प्रोग्राम फाईल उघडू शकला नाही.",
    }},
    {0x0450, {  // Mongolian
        L"Тусламжтайгаар нээх",
        L"Энэ файлыг нээхийн тулд ашиглахыг хүсэж буй програмаа сонгоно уу:",
        L"Файл:",
        L"Санал болгосон програмууд",
        L"Бусад програмууд",
        L"Энэ төрлийн файлд ашиглахыг хүсэж буй тайлбараа бичнэ үү:",
        L"&Энэ төрлийн файлыг нээхийн тулд сонгосон програмаа үргэлж ашиглах",
        L"&Хайх...",
        L"Хүссэн програм жагсаалтад эсвэл таны компьютерт байхгүй бол <A ID=\"WebSearch\">вэбээс тохирох програмыг хайж болно</A>.",
        L"Тусламжтайгаар нээх...",
        L"Програмууд",
        L"Бүх файлууд",
        L"За",
        L"Цуцлах",
        L"Энэ төрлийн файлыг нээж чадах бүртгэлтэй програм байхгүй.",
        L"Сонгосон програм файлыг нээж чадсангүй.",
    }},
    {0x0455, {  // Burmese
        L"ဖြင့်ဖွင့်ပါ",
        L"ဤဖိုင်ကိုဖွင့်ရန် သင်အသုံးပြုလိုသော ပရိုဂရမ်ကို ရွေးပါ။",
        L"ဖိုင်:",
        L"အကြံပြုထားသော ပရိုဂရမ်များ",
        L"အခြား ပရိုဂရမ်များ",
        L"ဤဖိုင်အမျိုးအစားအတွက် သင်အသုံးပြုလိုသော ဖော်ပြချက်ကို ရိုက်ထည့်ပါ။",
        L"&ဤဖိုင်အမျိုးအစားကိုဖွင့်ရန် ရွေးထားသော ပရိုဂရမ်ကို အမြဲသုံးပါ",
        L"&ရှာဖွေပါ...",
        L"သင်လိုချင်သော ပရိုဂရမ်သည် စာရင်းထဲ သို့မဟုတ် သင့်ကွန်ပျူတာတွင် မရှိပါက <A ID=\"WebSearch\">ဝဘ်ပေါ်တွင် သင့်လျော်သော ပရိုဂရမ်ကို ရှာဖွေနိုင်ပါသည်</A>။",
        L"ဖြင့်ဖွင့်ပါ...",
        L"ပရိုဂရမ်များ",
        L"ဖိုင်အားလုံး",
        L"အိုကေ",
        L"မလုပ်တော့ပါ",
        L"ဤဖိုင်အမျိုးအစားကို ဖွင့်နိုင်သော မှတ်ပုံတင်ထားသည့် ပရိုဂရမ် မရှိပါ။",
        L"ရွေးထားသော ပရိုဂရမ်သည် ဖိုင်ကို မဖွင့်နိုင်ပါ။",
    }},
    {0x0461, {  // Nepali
        L"यसद्वारा खोल्नुहोस्",
        L"यो फाइल खोल्न तपाईंले प्रयोग गर्न चाहनुभएको प्रोग्राम छान्नुहोस्:",
        L"फाइल:",
        L"सिफारिस गरिएका प्रोग्रामहरू",
        L"अन्य प्रोग्रामहरू",
        L"यस प्रकारको फाइलका लागि तपाईंले प्रयोग गर्न चाहनुभएको विवरण टाइप गर्नुहोस्:",
        L"&यस प्रकारको फाइल खोल्न सधैं चयन गरिएको प्रोग्राम प्रयोग गर्नुहोस्",
        L"&ब्राउज गर्नुहोस्...",
        L"तपाईंले चाहनुभएको प्रोग्राम सूचीमा वा तपाईंको कम्प्युटरमा छैन भने, तपाईंले <A ID=\"WebSearch\">वेबमा उपयुक्त प्रोग्राम खोज्न सक्नुहुन्छ</A>।",
        L"यसद्वारा खोल्नुहोस्...",
        L"प्रोग्रामहरू",
        L"सबै फाइलहरू",
        L"ठीक छ",
        L"रद्द गर्नुहोस्",
        L"कुनै पनि दर्ता गरिएको प्रोग्रामले यस प्रकारको फाइल खोल्न सक्दैन।",
        L"चयन गरिएको प्रोग्रामले फाइल खोल्न सकेन।",
    }},
    {0x0814, {  // Norwegian Nynorsk
        L"Opne med",
        L"Vel programmet du vil bruke for å opne denne fila:",
        L"Fil:",
        L"Tilrådde program",
        L"Andre program",
        L"Skriv ei skildring du vil bruke for denne typen fil:",
        L"&Bruk alltid det valde programmet til å opne denne typen fil",
        L"&Bla gjennom...",
        L"Dersom programmet du vil ha ikkje er i lista eller på datamaskina, kan du <A ID=\"WebSearch\">leite etter eit høveleg program på nettet</A>.",
        L"Opne med...",
        L"Program",
        L"Alle filer",
        L"OK",
        L"Avbryt",
        L"Ingen registrerte program kan opne denne typen fil.",
        L"Det valde programmet kunne ikkje opne fila.",
    }},
    {0x0448, {  // Odia
        L"ଏହାଦ୍ୱାରା ଖୋଲନ୍ତୁ",
        L"ଏହି ଫାଇଲ୍ ଖୋଲିବା ପାଇଁ ଆପଣ ବ୍ୟବହାର କରିବାକୁ ଚାହୁଁଥିବା ପ୍ରୋଗ୍ରାମ୍ ବାଛନ୍ତୁ:",
        L"ଫାଇଲ୍:",
        L"ସୁପାରିଶ କରାଯାଇଥିବା ପ୍ରୋଗ୍ରାମ୍",
        L"ଅନ୍ୟ ପ୍ରୋଗ୍ରାମ୍",
        L"ଏହି ପ୍ରକାରର ଫାଇଲ୍ ପାଇଁ ଆପଣ ବ୍ୟବହାର କରିବାକୁ ଚାହୁଁଥିବା ବର୍ଣ୍ଣନା ଟାଇପ୍ କରନ୍ତୁ:",
        L"&ଏହି ପ୍ରକାରର ଫାଇଲ୍ ଖୋଲିବା ପାଇଁ ସର୍ବଦା ବଛା ଯାଇଥିବା ପ୍ରୋଗ୍ରାମ୍ ବ୍ୟବହାର କରନ୍ତୁ",
        L"&ବ୍ରାଉଜ୍ କରନ୍ତୁ...",
        L"ଆପଣ ଚାହୁଁଥିବା ପ୍ରୋଗ୍ରାମ୍ ତାଲିକାରେ କିମ୍ବା ଆପଣଙ୍କ କମ୍ପ୍ୟୁଟରରେ ନ ଥିଲେ, ଆପଣ <A ID=\"WebSearch\">ୱେବ୍\u200cରେ ଉପଯୁକ୍ତ ପ୍ରୋଗ୍ରାମ୍ ଖୋଜି ପାରିବେ</A>।",
        L"ଏହାଦ୍ୱାରା ଖୋଲନ୍ତୁ...",
        L"ପ୍ରୋଗ୍ରାମ୍",
        L"ସମସ୍ତ ଫାଇଲ୍",
        L"ଠିକ୍ ଅଛି",
        L"ବାତିଲ୍ କରନ୍ତୁ",
        L"ଏହି ପ୍ରକାରର ଫାଇଲ୍ ଖୋଲିପାରୁଥିବା କୌଣସି ପଞ୍ଜୀକୃତ ପ୍ରୋଗ୍ରାମ୍ ନାହିଁ।",
        L"ବଛା ଯାଇଥିବା ପ୍ରୋଗ୍ରାମ୍ ଫାଇଲ୍ ଖୋଲିପାରିଲା ନାହିଁ।",
    }},
    {0x0463, {  // Pashto
        L"په دې سره پرانیزئ",
        L"د دې فایل د پرانیستلو لپاره هغه پروګرام وټاکئ چې کارول یې غواړئ:",
        L"فایل:",
        L"وړاندیز شوي پروګرامونه",
        L"نور پروګرامونه",
        L"هغه توضیح ولیکئ چې د دې ډول فایل لپاره یې کارول غواړئ:",
        L"&د دې ډول فایل د پرانیستلو لپاره تل ټاکل شوی پروګرام وکاروئ",
        L"&لټون...",
        L"که هغه پروګرام چې غواړئ په نوملړ کې یا ستاسو په کمپیوټر کې نه وي، تاسو کولی شئ <A ID=\"WebSearch\">په وېب کې مناسب پروګرام ولټوئ</A>.",
        L"په دې سره پرانیزئ...",
        L"پروګرامونه",
        L"ټول فایلونه",
        L"ښه",
        L"لغوه",
        L"هېڅ ثبت شوی پروګرام نشي کولی دا ډول فایل پرانیزي.",
        L"ټاکل شوي پروګرام ونه شو کولی فایل پرانیزي.",
    }},
    {0x0429, {  // Persian
        L"باز کردن با",
        L"برنامه\u200cای را که می\u200cخواهید برای باز کردن این فایل استفاده کنید انتخاب کنید:",
        L"فایل:",
        L"برنامه\u200cهای پیشنهادی",
        L"برنامه\u200cهای دیگر",
        L"توضیحی را که می\u200cخواهید برای این نوع فایل استفاده کنید تایپ کنید:",
        L"&همیشه از برنامه انتخاب\u200cشده برای باز کردن این نوع فایل استفاده کن",
        L"&مرور...",
        L"اگر برنامه مورد نظر در فهرست یا روی رایانه شما نیست، می\u200cتوانید <A ID=\"WebSearch\">برنامه مناسب را در وب جستجو کنید</A>.",
        L"باز کردن با...",
        L"برنامه\u200cها",
        L"همه فایل\u200cها",
        L"تأیید",
        L"لغو",
        L"هیچ برنامه ثبت\u200cشده\u200cای نمی\u200cتواند این نوع فایل را باز کند.",
        L"برنامه انتخاب\u200cشده نتوانست فایل را باز کند.",
    }},
    {0x0446, {  // Punjabi
        L"ਨਾਲ ਖੋਲ੍ਹੋ",
        L"ਇਸ ਫ਼ਾਈਲ ਨੂੰ ਖੋਲ੍ਹਣ ਲਈ ਉਹ ਪ੍ਰੋਗਰਾਮ ਚੁਣੋ ਜੋ ਤੁਸੀਂ ਵਰਤਣਾ ਚਾਹੁੰਦੇ ਹੋ:",
        L"ਫ਼ਾਈਲ:",
        L"ਸਿਫ਼ਾਰਸ਼ ਕੀਤੇ ਪ੍ਰੋਗਰਾਮ",
        L"ਹੋਰ ਪ੍ਰੋਗਰਾਮ",
        L"ਇਸ ਕਿਸਮ ਦੀ ਫ਼ਾਈਲ ਲਈ ਉਹ ਵੇਰਵਾ ਟਾਈਪ ਕਰੋ ਜੋ ਤੁਸੀਂ ਵਰਤਣਾ ਚਾਹੁੰਦੇ ਹੋ:",
        L"&ਇਸ ਕਿਸਮ ਦੀ ਫ਼ਾਈਲ ਖੋਲ੍ਹਣ ਲਈ ਹਮੇਸ਼ਾ ਚੁਣਿਆ ਪ੍ਰੋਗਰਾਮ ਵਰਤੋ",
        L"&ਬ੍ਰਾਊਜ਼ ਕਰੋ...",
        L"ਜੇ ਤੁਹਾਡਾ ਲੋੜੀਂਦਾ ਪ੍ਰੋਗਰਾਮ ਸੂਚੀ ਵਿੱਚ ਜਾਂ ਤੁਹਾਡੇ ਕੰਪਿਊਟਰ ਉੱਤੇ ਨਹੀਂ ਹੈ, ਤਾਂ ਤੁਸੀਂ <A ID=\"WebSearch\">ਵੈੱਬ ਉੱਤੇ ਢੁਕਵਾਂ ਪ੍ਰੋਗਰਾਮ ਲੱਭ ਸਕਦੇ ਹੋ</A>।",
        L"ਨਾਲ ਖੋਲ੍ਹੋ...",
        L"ਪ੍ਰੋਗਰਾਮ",
        L"ਸਾਰੀਆਂ ਫ਼ਾਈਲਾਂ",
        L"ਠੀਕ ਹੈ",
        L"ਰੱਦ ਕਰੋ",
        L"ਕੋਈ ਵੀ ਰਜਿਸਟਰਡ ਪ੍ਰੋਗਰਾਮ ਇਸ ਕਿਸਮ ਦੀ ਫ਼ਾਈਲ ਨਹੀਂ ਖੋਲ੍ਹ ਸਕਦਾ।",
        L"ਚੁਣਿਆ ਪ੍ਰੋਗਰਾਮ ਫ਼ਾਈਲ ਨਹੀਂ ਖੋਲ੍ਹ ਸਕਿਆ।",
    }},
    {0x046B, {  // Quechua
        L"Kaywan kichay",
        L"Kay willañiqita kichanaykipaq llamk'achiyta munasqayki wakichita akllay:",
        L"Willañiqi:",
        L"Amañakusqa wakichikuna",
        L"Huk wakichikuna",
        L"Kay rikch'aq willañiqipaq llamk'achiyta munasqayki sut'inchayta qillqay:",
        L"&Kay rikch'aq willañiqita kichanaykipaq akllasqa wakichita hayk'aqpas llamk'achiy",
        L"&Maskay...",
        L"Munasqayki wakichi sutisuyupi icha antañiqiqiykipi mana kaptinqa, <A ID=\"WebSearch\">Webpi kaqllaq wakichita maskayta atinki</A>.",
        L"Kaywan kichay...",
        L"Wakichikuna",
        L"Llapam willañiqikuna",
        L"Allinmi",
        L"Tatichiy",
        L"Manam mayqin qillqasqa wakichipas kay rikch'aq willañiqita kichayta atinchu.",
        L"Akllasqa wakichi manam willañiqita kichayta atirqanchu.",
    }},
    {0x0491, {  // Scottish Gaelic
        L"Fosgail le",
        L"Tagh am prògram a tha thu airson a chleachdadh gus am faidhle seo fhosgladh:",
        L"Faidhle:",
        L"Prògraman air am moladh",
        L"Prògraman eile",
        L"Sgrìobh tuairisgeul a tha thu airson a chleachdadh airson an seòrsa faidhle seo:",
        L"&Cleachd an-còmhnaidh am prògram a thagh thu gus an seòrsa faidhle seo fhosgladh",
        L"&Brabhsaich...",
        L"Mura bheil am prògram a tha thu ag iarraidh air an liosta no air a' choimpiutair agad, 's urrainn dhut <A ID=\"WebSearch\">coimhead airson prògram freagarrach air an lìon</A>.",
        L"Fosgail le...",
        L"Prògraman",
        L"Gach faidhle",
        L"Ceart ma-thà",
        L"Sguir dheth",
        L"Chan urrainn do phrògram clàraichte sam bith an seòrsa faidhle seo fhosgladh.",
        L"Cha b' urrainn dhan phrògram a thagh thu am faidhle fhosgladh.",
    }},
    {0x241A, {  // Serbian (Latin)
        L"Otvori pomoću",
        L"Izaberite program koji želite da koristite za otvaranje ove datoteke:",
        L"Datoteka:",
        L"Preporučeni programi",
        L"Ostali programi",
        L"Unesite opis koji želite da koristite za ovu vrstu datoteke:",
        L"&Uvek koristi izabrani program za otvaranje ove vrste datoteke",
        L"&Pregledaj...",
        L"Ako program koji želite nije na listi ili na računaru, možete da <A ID=\"WebSearch\">potražite odgovarajući program na vebu</A>.",
        L"Otvori pomoću...",
        L"Programi",
        L"Sve datoteke",
        L"U redu",
        L"Otkaži",
        L"Nijedan registrovani program ne može da otvori ovu vrstu datoteke.",
        L"Izabrani program nije mogao da otvori datoteku.",
    }},
    {0x281A, {  // Serbian (Cyrillic)
        L"Отвори помоћу",
        L"Изаберите програм који желите да користите за отварање ове датотеке:",
        L"Датотека:",
        L"Препоручени програми",
        L"Остали програми",
        L"Унесите опис који желите да користите за ову врсту датотеке:",
        L"&Увек користи изабрани програм за отварање ове врсте датотеке",
        L"&Прегледај...",
        L"Ако програм који желите није на листи или на рачунару, можете да <A ID=\"WebSearch\">потражите одговарајући програм на вебу</A>.",
        L"Отвори помоћу...",
        L"Програми",
        L"Све датотеке",
        L"У реду",
        L"Откажи",
        L"Ниједан регистровани програм не може да отвори ову врсту датотеке.",
        L"Изабрани програм није могао да отвори датотеку.",
    }},
    {0x046C, {  // Northern Sotho
        L"Bula ka",
        L"Kgetha lenaneo leo o nyakago go le diriša go bula faele ye:",
        L"Faele:",
        L"Mananeo a eletšwago",
        L"Mananeo a mangwe",
        L"Ngwala tlhaloso yeo o nyakago go e diriša mohuta wo wa faele:",
        L"&Diriša kamehla lenaneo leo le kgethilwego go bula mohuta wo wa faele",
        L"&Sekaseka...",
        L"Ge eba lenaneo leo o le nyakago le sego lenaneng goba khomphutheng ya gago, o ka kgona go <A ID=\"WebSearch\">nyaka lenaneo la maleba Webeng</A>.",
        L"Bula ka...",
        L"Mananeo",
        L"Difaele ka moka",
        L"Go lokile",
        L"Khansela",
        L"Ga go lenaneo le le ngwadišitšwego leo le ka bulago mohuta wo wa faele.",
        L"Lenaneo leo le kgethilwego le paletšwe go bula faele.",
    }},
    {0x0432, {  // Tswana
        L"Bula ka",
        L"Tlhopha lenaneo le o batlang go le dirisa go bula faela eno:",
        L"Faele:",
        L"Mananeo a a atlanegisitsweng",
        L"Mananeo a mangwe",
        L"Kwala tlhaloso e o batlang go e dirisa mo mofuteng ono wa faela:",
        L"&Nna o dirisa lenaneo le le tlhophilweng go bula mofuta ono wa faela",
        L"&Batlisisa...",
        L"Fa lenaneo le o le batlang le se mo lenaaneng kgotsa mo khomphiutheng ya gago, o ka kgona go <A ID=\"WebSearch\">batla lenaneo le le tshwanelang mo Webeng</A>.",
        L"Bula ka...",
        L"Mananeo",
        L"Difaele tsotlhe",
        L"Go siame",
        L"Khansela",
        L"Ga go na lenaneo le le kwadisitsweng le le ka bulang mofuta ono wa faela.",
        L"Lenaneo le le tlhophilweng ga le a kgona go bula faela.",
    }},
    {0x045B, {  // Sinhala
        L"සමඟ විවෘත කරන්න",
        L"මෙම ගොනුව විවෘත කිරීමට ඔබට භාවිතා කිරීමට අවශ්\u200dය වැඩසටහන තෝරන්න:",
        L"ගොනුව:",
        L"නිර්දේශිත වැඩසටහන්",
        L"වෙනත් වැඩසටහන්",
        L"මෙවැනි ගොනු සඳහා ඔබට භාවිතා කිරීමට අවශ්\u200dය විස්තරය ටයිප් කරන්න:",
        L"&මෙවැනි ගොනු විවෘත කිරීමට සැමවිටම තෝරාගත් වැඩසටහන භාවිතා කරන්න",
        L"&බ්\u200dරවුස් කරන්න...",
        L"ඔබට අවශ්\u200dය වැඩසටහන ලැයිස්තුවේ හෝ ඔබේ පරිගණකයේ නොමැති නම්, ඔබට <A ID=\"WebSearch\">වෙබයේ සුදුසු වැඩසටහන සෙවිය හැක</A>.",
        L"සමඟ විවෘත කරන්න...",
        L"වැඩසටහන්",
        L"සියලුම ගොනු",
        L"හරි",
        L"අවලංගු කරන්න",
        L"මෙවැනි ගොනු විවෘත කළ හැකි ලියාපදිංචි වැඩසටහනක් නොමැත.",
        L"තෝරාගත් වැඩසටහනට ගොනුව විවෘත කිරීමට නොහැකි විය.",
    }},
    {0x0424, {  // Slovenian
        L"Odpri z",
        L"Izberite program, ki ga želite uporabiti za odpiranje te datoteke:",
        L"Datoteka:",
        L"Priporočeni programi",
        L"Drugi programi",
        L"Vnesite opis, ki ga želite uporabiti za to vrsto datoteke:",
        L"&Vedno uporabi izbrani program za odpiranje te vrste datoteke",
        L"&Prebrskaj...",
        L"Če programa, ki ga želite, ni na seznamu ali v računalniku, lahko <A ID=\"WebSearch\">na spletu poiščete ustrezen program</A>.",
        L"Odpri z...",
        L"Programi",
        L"Vse datoteke",
        L"V redu",
        L"Prekliči",
        L"Noben registriran program ne more odpreti te vrste datoteke.",
        L"Izbrani program ni mogel odpreti datoteke.",
    }},
    {0x0428, {  // Tajik
        L"Кушодан бо",
        L"Барои кушодани ин файл барномаеро, ки мехоҳед истифода баред, интихоб кунед:",
        L"Файл:",
        L"Барномаҳои тавсияшуда",
        L"Барномаҳои дигар",
        L"Тавсиферо, ки барои ин намуди файл истифода кардан мехоҳед, ворид кунед:",
        L"&Барои кушодани ин намуди файл ҳамеша барномаи интихобшударо истифода баред",
        L"&Мурур...",
        L"Агар барномаи дилхоҳ дар рӯйхат ё дар компютери шумо набошад, шумо метавонед <A ID=\"WebSearch\">барномаи муносибро дар интернет ҷустуҷӯ кунед</A>.",
        L"Кушодан бо...",
        L"Барномаҳо",
        L"Ҳамаи файлҳо",
        L"Хуб",
        L"Бекор кардан",
        L"Ягон барномаи сабтшуда ин намуди файлро кушода наметавонад.",
        L"Барномаи интихобшуда файлро кушода натавонист.",
    }},
    {0x0449, {  // Tamil
        L"இதனுடன் திற",
        L"இந்தக் கோப்பைத் திறக்க நீங்கள் பயன்படுத்த விரும்பும் நிரலைத் தேர்ந்தெடுக்கவும்:",
        L"கோப்பு:",
        L"பரிந்துரைக்கப்பட்ட நிரல்கள்",
        L"பிற நிரல்கள்",
        L"இந்த வகையான கோப்பிற்கு நீங்கள் பயன்படுத்த விரும்பும் விளக்கத்தைத் தட்டச்சு செய்யவும்:",
        L"&இந்த வகையான கோப்பைத் திறக்க எப்போதும் தேர்ந்தெடுத்த நிரலைப் பயன்படுத்து",
        L"&உலாவு...",
        L"நீங்கள் விரும்பும் நிரல் பட்டியலில் அல்லது உங்கள் கணினியில் இல்லை என்றால், <A ID=\"WebSearch\">வலையில் பொருத்தமான நிரலைத் தேடலாம்</A>.",
        L"இதனுடன் திற...",
        L"நிரல்கள்",
        L"எல்லாக் கோப்புகளும்",
        L"சரி",
        L"ரத்து",
        L"இந்த வகையான கோப்பைத் திறக்கக்கூடிய பதிவுசெய்யப்பட்ட நிரல் எதுவும் இல்லை.",
        L"தேர்ந்தெடுத்த நிரலால் கோப்பைத் திறக்க முடியவில்லை.",
    }},
    {0x0444, {  // Tatar
        L"Ярдәмендә ачу",
        L"Бу файлны ачу өчен кулланырга теләгән программаны сайлагыз:",
        L"Файл:",
        L"Тәкъдим ителгән программалар",
        L"Башка программалар",
        L"Файлның бу төре өчен кулланырга теләгән тасвирламаны языгыз:",
        L"&Файлның бу төрен ачу өчен һәрвакыт сайланган программаны кулланырга",
        L"&Күзәтү...",
        L"Теләгән программа исемлектә яки санагыгызда юк икән, <A ID=\"WebSearch\">Интернетта тиешле программаны эзли аласыз</A>.",
        L"Ярдәмендә ачу...",
        L"Программалар",
        L"Барлык файллар",
        L"Ярар",
        L"Баш тарту",
        L"Файлның бу төрен ача алырлык теркәлгән программа юк.",
        L"Сайланган программа файлны ача алмады.",
    }},
    {0x044A, {  // Telugu
        L"దీనితో తెరవండి",
        L"ఈ ఫైల్\u200cను తెరవడానికి మీరు ఉపయోగించాలనుకుంటున్న ప్రోగ్రామ్\u200cను ఎంచుకోండి:",
        L"ఫైల్:",
        L"సిఫార్సు చేసిన ప్రోగ్రామ్\u200cలు",
        L"ఇతర ప్రోగ్రామ్\u200cలు",
        L"ఈ రకమైన ఫైల్ కోసం మీరు ఉపయోగించాలనుకుంటున్న వివరణను టైప్ చేయండి:",
        L"&ఈ రకమైన ఫైల్ తెరవడానికి ఎల్లప్పుడూ ఎంచుకున్న ప్రోగ్రామ్\u200cను ఉపయోగించు",
        L"&బ్రౌజ్ చేయండి...",
        L"మీకు కావలసిన ప్రోగ్రామ్ జాబితాలో లేదా మీ కంప్యూటర్\u200cలో లేకపోతే, మీరు <A ID=\"WebSearch\">వెబ్\u200cలో తగిన ప్రోగ్రామ్ కోసం వెతకవచ్చు</A>.",
        L"దీనితో తెరవండి...",
        L"ప్రోగ్రామ్\u200cలు",
        L"అన్ని ఫైల్\u200cలు",
        L"సరే",
        L"రద్దు చేయి",
        L"ఈ రకమైన ఫైల్\u200cను తెరవగల నమోదిత ప్రోగ్రామ్ ఏదీ లేదు.",
        L"ఎంచుకున్న ప్రోగ్రామ్ ఫైల్\u200cను తెరవలేకపోయింది.",
    }},
    {0x041E, {  // Thai
        L"เปิดด้วย",
        L"เลือกโปรแกรมที่คุณต้องการใช้เปิดแฟ้มนี้:",
        L"แฟ้ม:",
        L"โปรแกรมที่แนะนำ",
        L"โปรแกรมอื่นๆ",
        L"พิมพ์คำอธิบายที่คุณต้องการใช้สำหรับแฟ้มชนิดนี้:",
        L"&ใช้โปรแกรมที่เลือกไว้เปิดแฟ้มชนิดนี้เสมอ",
        L"&เรียกดู...",
        L"ถ้าโปรแกรมที่คุณต้องการไม่อยู่ในรายการหรือในคอมพิวเตอร์ของคุณ คุณสามารถ <A ID=\"WebSearch\">ค้นหาโปรแกรมที่เหมาะสมบนเว็บ</A> ได้",
        L"เปิดด้วย...",
        L"โปรแกรม",
        L"แฟ้มทั้งหมด",
        L"ตกลง",
        L"ยกเลิก",
        L"ไม่มีโปรแกรมที่ลงทะเบียนไว้สามารถเปิดแฟ้มชนิดนี้ได้",
        L"โปรแกรมที่เลือกไว้ไม่สามารถเปิดแฟ้มได้",
    }},
    {0x0473, {  // Tigrinya
        L"ኣብዚ ክፈት",
        L"ነዚ ፋይል ንምኽፋት ክትጥቀመሉ እትደልዮ ፕሮግራም ምረጽ:",
        L"ፋይል:",
        L"ዝተመኽሩ ፕሮግራማት",
        L"ካልኦት ፕሮግራማት",
        L"ነዚ ዓይነት ፋይል ክትጥቀመሉ እትደልዮ መግለጺ ጽሓፍ:",
        L"&ነዚ ዓይነት ፋይል ንምኽፋት ኵሉ ግዜ እቲ ዝተመረጸ ፕሮግራም ተጠቐም",
        L"&ኣስስ...",
        L"እቲ እትደልዮ ፕሮግራም ኣብ ዝርዝር ወይ ኣብ ኮምፒውተርካ እንተዘየለ፣ <A ID=\"WebSearch\">ኣብ ዌብ ግቡእ ፕሮግራም ክትደሊ ትኽእል</A>።",
        L"ኣብዚ ክፈት...",
        L"ፕሮግራማት",
        L"ኵሎም ፋይላት",
        L"ሕራይ",
        L"ሰርዝ",
        L"ነዚ ዓይነት ፋይል ክኸፍት ዝኽእል ዝተመዝገበ ፕሮግራም የለን።",
        L"እቲ ዝተመረጸ ፕሮግራም ነቲ ፋይል ክኸፍቶ ኣይከኣለን።",
    }},
    {0x0442, {  // Turkmen
        L"Bilen aç",
        L"Bu faýly açmak üçin ulanmak isleýän programmaňyzy saýlaň:",
        L"Faýl:",
        L"Maslahat berilýän programmalar",
        L"Beýleki programmalar",
        L"Faýlyň bu görnüşi üçin ulanmak isleýän beýanyňyzy ýazyň:",
        L"&Faýlyň bu görnüşini açmak üçin hemişe saýlanan programmany ulanyň",
        L"&Gözden geçir...",
        L"Isleýän programmaňyz sanawda ýa-da kompýuteriňizde ýok bolsa, <A ID=\"WebSearch\">Internetde laýyk programmany gözläp bilersiňiz</A>.",
        L"Bilen aç...",
        L"Programmalar",
        L"Ähli faýllar",
        L"Bolýar",
        L"Ýatyr",
        L"Faýlyň bu görnüşini açyp biljek bellige alnan programma ýok.",
        L"Saýlanan programma faýly açyp bilmedi.",
    }},
    {0x0420, {  // Urdu
        L"کے ساتھ کھولیں",
        L"اس فائل کو کھولنے کے لیے جو پروگرام آپ استعمال کرنا چاہتے ہیں اسے منتخب کریں:",
        L"فائل:",
        L"تجویز کردہ پروگرام",
        L"دیگر پروگرام",
        L"اس قسم کی فائل کے لیے جو وضاحت آپ استعمال کرنا چاہتے ہیں وہ ٹائپ کریں:",
        L"&اس قسم کی فائل کھولنے کے لیے ہمیشہ منتخب پروگرام استعمال کریں",
        L"&براؤز کریں...",
        L"اگر مطلوبہ پروگرام فہرست میں یا آپ کے کمپیوٹر پر نہیں ہے تو آپ <A ID=\"WebSearch\">ویب پر مناسب پروگرام تلاش کر سکتے ہیں</A>۔",
        L"کے ساتھ کھولیں...",
        L"پروگرامز",
        L"تمام فائلیں",
        L"ٹھیک ہے",
        L"منسوخ کریں",
        L"اس قسم کی فائل کوئی رجسٹرڈ پروگرام نہیں کھول سکتا۔",
        L"منتخب پروگرام فائل نہیں کھول سکا۔",
    }},
    {0x0480, {  // Uyghur
        L"بىلەن ئېچىش",
        L"بۇ ھۆججەتنى ئېچىش ئۈچۈن ئىشلەتمەكچى بولغان پروگراممىنى تاللاڭ:",
        L"ھۆججەت:",
        L"تەۋسىيە قىلىنغان پروگراممىلار",
        L"باشقا پروگراممىلار",
        L"ھۆججەتنىڭ بۇ تۈرى ئۈچۈن ئىشلەتمەكچى بولغان چۈشەندۈرۈشنى يېزىڭ:",
        L"&ھۆججەتنىڭ بۇ تۈرىنى ئېچىش ئۈچۈن ھەمىشە تاللانغان پروگراممىنى ئىشلىتىش",
        L"&كۆرۈش...",
        L"خالىغان پروگرامما تىزىملىكتە ياكى كومپيۇتېرىڭىزدا بولمىسا، <A ID=\"WebSearch\">تور بەتتە مۇناسىپ پروگراممىنى ئىزدەپ تاپالايسىز</A>.",
        L"بىلەن ئېچىش...",
        L"پروگراممىلار",
        L"بارلىق ھۆججەتلەر",
        L"جەزملە",
        L"بىكار قىلىش",
        L"ھۆججەتنىڭ بۇ تۈرىنى ئاچالايدىغان تىزىملاتقان پروگرامما يوق.",
        L"تاللانغان پروگرامما ھۆججەتنى ئاچالمىدى.",
    }},
    {0x0443, {  // Uzbek
        L"Yordamida ochish",
        L"Ushbu faylni ochish uchun foydalanmoqchi bo'lgan dasturni tanlang:",
        L"Fayl:",
        L"Tavsiya etilgan dasturlar",
        L"Boshqa dasturlar",
        L"Faylning ushbu turi uchun foydalanmoqchi bo'lgan tavsifni kiriting:",
        L"&Faylning ushbu turini ochish uchun har doim tanlangan dasturdan foydalanish",
        L"&Ko'rib chiqish...",
        L"Agar kerakli dastur ro'yxatda yoki kompyuteringizda bo'lmasa, <A ID=\"WebSearch\">internetdan mos dasturni izlashingiz mumkin</A>.",
        L"Yordamida ochish...",
        L"Dasturlar",
        L"Barcha fayllar",
        L"OK",
        L"Bekor qilish",
        L"Faylning ushbu turini ocha oladigan ro'yxatdan o'tgan dastur yo'q.",
        L"Tanlangan dastur faylni ocha olmadi.",
    }},
    {0x042A, {  // Vietnamese
        L"Mở bằng",
        L"Chọn chương trình bạn muốn dùng để mở tệp này:",
        L"Tệp:",
        L"Chương trình được khuyến nghị",
        L"Chương trình khác",
        L"Nhập mô tả bạn muốn dùng cho loại tệp này:",
        L"&Luôn dùng chương trình đã chọn để mở loại tệp này",
        L"&Duyệt...",
        L"Nếu chương trình bạn muốn không có trong danh sách hoặc trên máy tính, bạn có thể <A ID=\"WebSearch\">tìm chương trình phù hợp trên Web</A>.",
        L"Mở bằng...",
        L"Chương trình",
        L"Tất cả tệp",
        L"OK",
        L"Hủy bỏ",
        L"Không có chương trình đã đăng ký nào có thể mở loại tệp này.",
        L"Chương trình đã chọn không thể mở tệp.",
    }},
    {0x0452, {  // Welsh
        L"Agor gyda",
        L"Dewiswch y rhaglen rydych chi am ei defnyddio i agor y ffeil hon:",
        L"Ffeil:",
        L"Rhaglenni a Argymhellir",
        L"Rhaglenni Eraill",
        L"Teipiwch ddisgrifiad rydych chi am ei ddefnyddio ar gyfer y math hwn o ffeil:",
        L"&Defnyddiwch y rhaglen a ddewiswyd bob amser i agor y math hwn o ffeil",
        L"&Pori...",
        L"Os nad yw'r rhaglen rydych chi ei heisiau ar y rhestr nac ar eich cyfrifiadur, gallwch <A ID=\"WebSearch\">chwilio am y rhaglen briodol ar y We</A>.",
        L"Agor gyda...",
        L"Rhaglenni",
        L"Pob Ffeil",
        L"Iawn",
        L"Canslo",
        L"Ni all unrhyw raglen gofrestredig agor y math hwn o ffeil.",
        L"Ni allai'r rhaglen a ddewiswyd agor y ffeil.",
    }},
    {0x0488, {  // Wolof
        L"Ubbi ak",
        L"Tànnul progaraam bi nga bëgg a jëfandikoo ngir ubbi bii dencukaay:",
        L"Dencukaay:",
        L"Progaraam yi ñu digal",
        L"Progaraam yineen",
        L"Bindal faramfàcce bu nga bëgg a jëfandikoo ngir wii xeet dencukaay:",
        L"&Jëfandikoo bépp yoon progaraam bi ñu tànn ngir ubbi wii xeet dencukaay",
        L"&Xoolal...",
        L"Su progaraam bi nga bëgg nekkul ci limu bi wala ci nosukaay bi, mën nga <A ID=\"WebSearch\">seet progaraam bu wuute ci Web bi</A>.",
        L"Ubbi ak...",
        L"Progaraam yi",
        L"Dencukaay yépp",
        L"Baax na",
        L"Faral",
        L"Amul progaraam bu ñu nàndal mën a ubbi wii xeet dencukaay.",
        L"Progaraam bi ñu tànn mënul a ubbi dencukaay bi.",
    }},
    {0x046A, {  // Yoruba
        L"Ṣii pẹlu",
        L"Yan eto ti o fẹ lo lati ṣii faili yii:",
        L"Faili:",
        L"Awọn Eto Ti A Ṣeduro",
        L"Awọn Eto Miiran",
        L"Tẹ àpèjúwe ti o fẹ lo fun iru faili yii:",
        L"&Máa lo eto ti a yàn nigbagbogbo lati ṣii iru faili yii",
        L"&Ṣàwárí...",
        L"Ti eto ti o fẹ ko ba si ninu akojọ tabi lori kọmputa rẹ, o le <A ID=\"WebSearch\">wa eto ti o yẹ lori Ayélujára</A>.",
        L"Ṣii pẹlu...",
        L"Awọn Eto",
        L"Gbogbo Faili",
        L"O dara",
        L"Fagilee",
        L"Ko si eto ti a forukọsilẹ ti o le ṣii iru faili yii.",
        L"Eto ti a yàn ko le ṣii faili naa.",
    }},
    {0x085D, {  // Inuktitut
        L"Uijjaq",
        L"Programmi toqqarlugu fili una uijjarlagu:",
        L"Fili:",
        L"Programmit atugaksarijaujut",
        L"Programmit asingit",
        L"Nalunaijaut allattuq filimut taassumunnga aturumajatsi:",
        L"&Tamarmik programmi toqqakkak aturlugu fili taassuma uijjarniarlugu",
        L"&Qiniq...",
        L"Programmi pigumajait titiqqanut iluaniittuq imaluunniit qarasaujanniittuq, <A ID=\"WebSearch\">qaritaujakkut programmi naleqquttuq qiniqtuinnarusukkaluaq</A>.",
        L"Uijjaq...",
        L"Programmit",
        L"Filit tamaita",
        L"Ii",
        L"Nungutaaq",
        L"Programmi atiliurutausimajuq inngittuq fili taassuma uijjarniaqtuni.",
        L"Programmi toqqakkak fili uijjarniaqtuq pijunnaanngittuq.",
    }},
    {0x0492, {  // Central Kurdish
        L"پێی بکەرەوە",
        L"ئەو پرۆگرامە هەڵبژێرە کە دەتەوێت بەکاربهێنیت بۆ کردنەوەی ئەم فایلە:",
        L"فایل:",
        L"پرۆگرامە پێشنیارکراوەکان",
        L"پرۆگرامەکانی تر",
        L"ئەو وەسفە بنووسە کە دەتەوێت بۆ ئەم جۆرە فایلە بەکاریبهێنیت:",
        L"&هەمیشە پرۆگرامە هەڵبژێردراوەکە بەکاربهێنە بۆ کردنەوەی ئەم جۆرە فایلە",
        L"&گەڕان...",
        L"ئەگەر ئەو پرۆگرامەی دەتەوێت لە لیستەکە یان لە کۆمپیوتەرەکەتدا نییە، دەتوانیت <A ID=\"WebSearch\">لە وێبدا بەدوای پرۆگرامی گونجاودا بگەڕێیت</A>.",
        L"پێی بکەرەوە...",
        L"پرۆگرامەکان",
        L"هەموو فایلەکان",
        L"باشە",
        L"هەڵوەشاندنەوە",
        L"هیچ پرۆگرامێکی تۆمارکراو ناتوانێت ئەم جۆرە فایلە بکاتەوە.",
        L"پرۆگرامە هەڵبژێردراوەکە نەیتوانی فایلەکە بکاتەوە.",
    }},
    {0x0459, {  // Sindhi
        L"سان کوليو",
        L"هن فائل کي کولڻ لاءِ جيڪو پروگرام استعمال ڪرڻ چاهيو ٿا سو چونڊيو:",
        L"فائل:",
        L"تجويز ڪيل پروگرام",
        L"ٻيا پروگرام",
        L"هن قسم جي فائل لاءِ جيڪا وضاحت استعمال ڪرڻ چاهيو ٿا سا ٽائيپ ڪريو:",
        L"&هن قسم جي فائل کولڻ لاءِ هميشه چونڊيل پروگرام استعمال ڪريو",
        L"&برائوز ڪريو...",
        L"جيڪڏهن گهربل پروگرام فهرست ۾ يا توهان جي ڪمپيوٽر تي نه آهي ته توهان <A ID=\"WebSearch\">ويب تي مناسب پروگرام ڳولي سگهو ٿا</A>.",
        L"سان کوليو...",
        L"پروگرام",
        L"سڀ فائلون",
        L"ٺيڪ آهي",
        L"رد ڪريو",
        L"هن قسم جي فائل کي ڪو به رجسٽرڊ پروگرام نٿو کولي سگهي.",
        L"چونڊيل پروگرام فائل نه کولي سگهيو.",
    }},
    {0x0860, {  // Kashmiri
        L"سٟتؠ کھولٕو",
        L"یہِ فایل کھولنہٕ خٲطرٕ تُہؠ یُس پروگرام اِستِمال کرُن یژھان چھِو سُہ ژٲرِو:",
        L"فایل:",
        L"تجویٖز کرنہٕ آمٕتؠ پروگرام",
        L"بیٚیہِ پروگرام",
        L"یہِ قٕسمٕچ فایلہٕ خٲطرٕ تُہؠ یُس وَضاحَتھ اِستِمال کرُن یژھان چھِو سُہ لیکھِو:",
        L"&یہِ قٕسمٕچ فایل کھولنہٕ خٲطرٕ ہمیشہٕ مُنتخَب پروگرام اِستِمال کٔرِو",
        L"&براؤز کٔرِو...",
        L"اگر یژھان پروگرام فہرستَس منٛز یا تُہنٛدِس کمپیوٗٹرَس پؠٹھ چھُنہٕ، تیٚلہِ تُہؠ ہیٚکِو <A ID=\"WebSearch\">ویبَس پؠٹھ موزوں پروگرام ژھٲنٛڈِتھ</A>۔",
        L"سٟتؠ کھولٕو...",
        L"پروگرام",
        L"سٲری فایلہٕ",
        L"ٹھیٖکھ",
        L"مُنسوٗخ کٔرِو",
        L"یہِ قٕسمٕچ فایل کھولنہٕ خٲطرٕ کانٛہہ رجِسٹرڈ پروگرام چھُنہٕ۔",
        L"مُنتخَب پروگرامَس فایل کھولنہٕ نہٕ آے۔",
    }},
    {0x044F, {  // Sanskrit
        L"अनेन उद्घाटयतु",
        L"एतत् सञ्चिकाम् उद्घाटयितुम् इच्छितं कार्यक्रमं चिनोतु:",
        L"सञ्चिका:",
        L"अनुशंसितानि कार्यक्रमाणि",
        L"अन्यानि कार्यक्रमाणि",
        L"एतादृशायै सञ्चिकायै इच्छितं वर्णनं लिखतु:",
        L"&एतादृशीं सञ्चिकाम् उद्घाटयितुं सर्वदा चितं कार्यक्रमं प्रयुङ्क्ताम्",
        L"&अन्वेषयतु...",
        L"इच्छितं कार्यक्रमं सूच्यां वा भवतः सङ्गणके वा नास्ति चेत्, <A ID=\"WebSearch\">जालपुटे उपयुक्तं कार्यक्रमम् अन्वेष्टुं शक्नोति</A>।",
        L"अनेन उद्घाटयतु...",
        L"कार्यक्रमाणि",
        L"सर्वाः सञ्चिकाः",
        L"अस्तु",
        L"निवर्तयतु",
        L"एतादृशीं सञ्चिकाम् उद्घाटयितुं न कोऽपि पञ्जीकृतः कार्यक्रमः शक्नोति।",
        L"चितं कार्यक्रमं सञ्चिकाम् उद्घाटयितुं नाशक्नोत्।",
    }},
    {0x0451, {  // Tibetan
        L"འདིས་ཁ་ཕྱེ",
        L"ཡིག་ཆ་འདི་ཁ་ཕྱེ་བར་བེད་སྤྱོད་བྱེད་འདོད་པའི་མཉེན་ཆས་འདེམས་རོགས་གནང་།",
        L"ཡིག་ཆ།",
        L"འོས་སྦྱོར་མཉེན་ཆས།",
        L"མཉེན་ཆས་གཞན།",
        L"ཡིག་ཆའི་རིགས་འདིར་བེད་སྤྱོད་བྱེད་འདོད་པའི་གསལ་བཤད་འབྲི་རོགས་གནང་།",
        L"&ཡིག་ཆའི་རིགས་འདི་ཁ་ཕྱེ་བར་རྟག་པར་འདེམས་པའི་མཉེན་ཆས་བེད་སྤྱོད་བྱེད།",
        L"&འཚོལ་ཞིབ།...",
        L"གལ་ཏེ་ཁྱེད་ཀྱིས་འདོད་པའི་མཉེན་ཆས་ཐོ་ཡིག་ནང་དང་ཁྱེད་ཀྱི་རྩིས་འཁོར་ནང་མེད་ན། ཁྱེད་ཀྱིས་<A ID=\"WebSearch\">དྲ་བའི་ནང་དུ་འོས་འཚམས་ཀྱི་མཉེན་ཆས་འཚོལ་ཆོག</A>།",
        L"འདིས་ཁ་ཕྱེ...",
        L"མཉེན་ཆས།",
        L"ཡིག་ཆ་ཡོངས།",
        L"འགྲིག་འདུག",
        L"ཕྱིར་འཐེན།",
        L"ཡིག་ཆའི་རིགས་འདི་ཁ་ཕྱེ་ཐུབ་པའི་ཐོ་འགོད་བྱས་པའི་མཉེན་ཆས་མེད།",
        L"འདེམས་པའི་མཉེན་ཆས་ཀྱིས་ཡིག་ཆ་དེ་ཁ་ཕྱེ་མ་ཐུབ།",
    }},
    {0x0490, {  // Walloon
        L"Drovi avou",
        L"Tchoezixhoz li programe ki vos vloz eployî po drovi ci fitchî ci:",
        L"Fitchî:",
        L"Programes ricmandés",
        L"Ôtes programes",
        L"Tapez ene discrijhaedje ki vos vloz eployî po cisse sôre di fitchî:",
        L"&Eployî todi li programe tchoezi po drovi cisse sôre di fitchî",
        L"&Foyter...",
        L"Si l' programe ki vos vloz n' est nén dins l' djivêye ou so vosse copiutrece, vos ploz <A ID=\"WebSearch\">cweri après on programe ki convént sol Daegntoele</A>.",
        L"Drovi avou...",
        L"Programes",
        L"Tos les fitchîs",
        L"Oyi",
        L"Rinoncî",
        L"Nou programe eredjistré ni sait drovi cisse sôre di fitchî.",
        L"Li programe tchoezi n' a savou drovi l' fitchî.",
    }},
    {0x0485, {  // Sakha
        L"Көмөтүнэн ас",
        L"Бу билэни аһарга туттуоххун баҕарар бырагыраамаҕын тал:",
        L"Билэ:",
        L"Сүбэлэнэр бырагыраамалар",
        L"Атын бырагыраамалар",
        L"Билэ бу көрүҥэр туттуоххун баҕарар быһаарыыгын суруй:",
        L"&Билэ бу көрүҥүн аһарга куруук талбыт бырагыраамаҕын туттун",
        L"&Көрүү...",
        L"Баҕарар бырагыраамаҥ тиһиккэ эбэтэр көмпүүтэргэр суох буоллаҕына, <A ID=\"WebSearch\">Интэриниэккэ сөптөөх бырагырааманы көрдүөххүн сөп</A>.",
        L"Көмөтүнэн ас...",
        L"Бырагыраамалар",
        L"Бары билэлэр",
        L"Сөп",
        L"Тохтот",
        L"Билэ бу көрүҥүн аһар кыахтаах бэлиэтэммит бырагыраама суох.",
        L"Талбыт бырагыраамаҥ билэни аспата.",
    }},
    {0x042E, {  // Upper Sorbian
        L"Wočinić z",
        L"Wubjerće program, kotryž chceće wužiwać, zo byšće tutu dataju wočinili:",
        L"Dataja:",
        L"Doporučene programy",
        L"Druhe programy",
        L"Zapisajće wopisanje, kotrež chceće za tutón typ dataje wužiwać:",
        L"&Přeco wubrany program wužiwać, zo by so tutón typ dataje wočinił",
        L"&Přehladać...",
        L"Jeli požadany program njeje w lisćinje abo na wašim ličaku, móžeće <A ID=\"WebSearch\">w interneće za přihódnym programom pytać</A>.",
        L"Wočinić z...",
        L"Programy",
        L"Wšě dataje",
        L"W porjadku",
        L"Přetorhnyć",
        L"Žadyn registrowany program njemóže tutón typ dataje wočinić.",
        L"Wubrany program njemóžeše dataju wočinić.",
    }},
    {0x082E, {  // Lower Sorbian
        L"Wócyniś z",
        L"Wubjeŕśo program, kótaryž cośo wužywaś, aby toś tu dataju wócynił:",
        L"Dataja:",
        L"Dopórucone programy",
        L"Druge programy",
        L"Zapišćo wopisanje, kótarež cośo za toś ten typ dataje wužywaś:",
        L"&Pśecej wubrany program wužywaś, aby toś ten typ dataje wócynił",
        L"&Pśeglědaś...",
        L"Jolic požedany program njejo w lisćinje abo na wašom licaku, móžośo <A ID=\"WebSearch\">w interneśe za pśigódnym programom pytaś</A>.",
        L"Wócyniś z...",
        L"Programy",
        L"Wšykne dataje",
        L"W pórěźe",
        L"Pśetergnuś",
        L"Žeden registrěrowany program njamóžo toś ten typ dataje wócyniś.",
        L"Wubrany program njamóžašo dataju wócyniś.",
    }},
    {0x047E, {  // Breton
        L"Digeriñ gant",
        L"Dibabit ar goulev a fell deoc'h implijout evit digeriñ ar restr-mañ:",
        L"Restr:",
        L"Goulevioù erbedet",
        L"Goulevioù all",
        L"Skrivit un deskrivadur a fell deoc'h implijout evit ar seurt restr-mañ:",
        L"&Implijout bepred ar goulev dibabet evit digeriñ ar seurt restr-mañ",
        L"&Furchal...",
        L"Ma n'emañ ket ar goulev a fell deoc'h el listenn pe war hoc'h urzhiataer e c'hallit <A ID=\"WebSearch\">klask ur goulev dereat war ar Gwiad</A>.",
        L"Digeriñ gant...",
        L"Goulevioù",
        L"An holl restroù",
        L"Mat eo",
        L"Nullañ",
        L"N'eus goulev enrollet ebet a c'hall digeriñ ar seurt restr-mañ.",
        L"N'eus ket deuet a-benn ar goulev dibabet da zigeriñ ar restr.",
    }},
    {0x0482, {  // Occitan
        L"Dobrir amb",
        L"Causissètz lo programa que volètz utilizar per dobrir aqueste fichièr:",
        L"Fichièr:",
        L"Programas recomandats",
        L"Autres programas",
        L"Picatz una descripcion que volètz utilizar per aqueste tipe de fichièr:",
        L"&Utilizar totjorn lo programa causit per dobrir aqueste tipe de fichièr",
        L"&Percórrer...",
        L"Se lo programa que volètz es pas dins la lista ni sus vòstre ordenador, podètz <A ID=\"WebSearch\">cercar lo programa apropriat sul Web</A>.",
        L"Dobrir amb...",
        L"Programas",
        L"Totes los fichièrs",
        L"D'acòrdi",
        L"Anullar",
        L"Cap de programa enregistrat pòt pas dobrir aqueste tipe de fichièr.",
        L"Lo programa causit a pas pogut dobrir lo fichièr.",
    }},
    {0x0483, {  // Corsican
        L"Apre cù",
        L"Sceglite u prugramma chì vulete usà per apre stu schedariu:",
        L"Schedariu:",
        L"Prugrammi cunsigliati",
        L"Altri prugrammi",
        L"Scrivite una discrizzione chì vulete usà per stu tipu di schedariu:",
        L"&Usà sempre u prugramma sceltu per apre stu tipu di schedariu",
        L"&Sfuglià...",
        L"S'è u prugramma chì vulete ùn hè micca in a lista o nant'à u vostru urdinatore, pudete <A ID=\"WebSearch\">circà u prugramma adattu nant'à u Web</A>.",
        L"Apre cù...",
        L"Prugrammi",
        L"Tutti i schedarii",
        L"Và bè",
        L"Annullà",
        L"Nisun prugramma arregistratu pò apre stu tipu di schedariu.",
        L"U prugramma sceltu ùn hà pussutu apre u schedariu.",
    }},
    {0x046F, {  // Greenlandic
        L"Ammagaat",
        L"Toqqaruk programmi atorumanerit fili una ammarniarlugu:",
        L"Fili:",
        L"Programmit innersuunneqartut",
        L"Programmit allat",
        L"Allaguk nassuiaat atorumanerit fili taama ittunut:",
        L"&Tamatiinnarpiit programmi toqqakkak atoruk fili taama ittunut ammarniarlugit",
        L"&Ujarlerit...",
        L"Programmi pigumanerit allattorsimaffimmi imaluunniit qarasaatianni inngippat, <A ID=\"WebSearch\">Internettimi programmi naleqquttoq ujarlerisinnaavat</A>.",
        L"Ammagaat...",
        L"Programmit",
        L"Filit tamaasa",
        L"Aap",
        L"Atorunnaarsiguk",
        L"Programmi allattorsimasoq ataaseq inngilaq fili taama ittunik ammarniarsinnaasoq.",
        L"Programmi toqqakkak fili ammarniarsinnaanngilai.",
    }},
    {0x0438, {  // Faroese
        L"Lat upp við",
        L"Vel forritið, sum tú vilt brúka at lata hesa fílu upp:",
        L"Fíla:",
        L"Tilmælt forrit",
        L"Onnur forrit",
        L"Skriva eina frágreiðing, sum tú vilt brúka til hesa fíluslag:",
        L"&Brúka altíð valda forritið at lata hesa fíluslag upp",
        L"&Kaga...",
        L"Um forritið, sum tú vilt hava, ikki er á listanum ella á tínari teldu, kanst tú <A ID=\"WebSearch\">leita eftir hóskandi forriti á netinum</A>.",
        L"Lat upp við...",
        L"Forrit",
        L"Allar fílur",
        L"Í lagi",
        L"Angra",
        L"Einki skrásett forrit kann lata hesa fíluslag upp.",
        L"Valda forritið kundi ikki lata fíluna upp.",
    }},
};

static std::atomic<const LocalePack*> g_CurrentLocalePack{&g_Locales[0]};
#define LOC(id) (g_CurrentLocalePack.load(std::memory_order_acquire)->strings[id])

enum class DefaultBehavior {
    Disabled,
    OpenSettings,
};

enum class ThemeMode {
    Auto,
    Light,
    Dark,
};

static std::atomic<bool> g_replaceSystemDialog{true};
static std::atomic<bool> g_showWebLink{true};
static std::atomic<bool> g_hideDescriptionField{false};

// Vertical space the description block occupies in the dialog layout, in
// logical pixels: from the top of its label (y=300) to the top of the
// "Always use" checkbox (y=354). When the block is hidden, the window
// and every control below it shrink/move up by exactly this much.
constexpr int kDescriptionBlockHeight = 54;

static int DescriptionLayoutShift() {
    return g_hideDescriptionField.load(std::memory_order_acquire)
               ? kDescriptionBlockHeight
               : 0;
}
static std::atomic<ThemeMode> g_themeMode{ThemeMode::Auto};
static std::atomic<DefaultBehavior> g_defaultBehavior{DefaultBehavior::Disabled};

static const LocalePack* TryFindLocalePack(LANGID langId) {
    const LANGID primary = PRIMARYLANGID(langId);
    for (const LocalePack& locale : g_Locales) {
        if (locale.langId == langId) return &locale;
    }
    for (const LocalePack& locale : g_Locales) {
        if (PRIMARYLANGID(locale.langId) == primary) return &locale;
    }
    return nullptr;
}

static const LocalePack* FindLocalePack(LANGID langId) {
    const LocalePack* locale = TryFindLocalePack(langId);
    return locale ? locale : &g_Locales[0];
}

static const LocalePack* LocalePackFromName(PCWSTR localeName) {
    if (!localeName || !*localeName) return nullptr;
    const LCID lcid = LocaleNameToLCID(localeName, 0);
    if (lcid) {
        if (const LocalePack* locale = TryFindLocalePack(LANGIDFROMLCID(lcid))) {
            return locale;
        }
    }
    return nullptr;
}

static const LocalePack* DetectAutomaticLocale() {
    ULONG languageCount = 0;
    ULONG bufferChars = 0;
    if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &languageCount, nullptr, &bufferChars) &&
        bufferChars > 1 && bufferChars < 32768) {
        try {
            std::vector<wchar_t> languages(bufferChars);
            if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &languageCount, languages.data(), &bufferChars)) {
                for (PCWSTR cursor = languages.data(); *cursor; cursor += wcslen(cursor) + 1) {
                    if (const LocalePack* locale = LocalePackFromName(cursor)) {
                        return locale;
                    }
                }
            }
        } catch (...) {}
    }

    wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {};
    if (GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName))) {
        if (const LocalePack* locale = LocalePackFromName(localeName)) {
            return locale;
        }
    }
    if (const LocalePack* locale = TryFindLocalePack(GetUserDefaultUILanguage())) {
        return locale;
    }
    if (GetSystemDefaultLocaleName(localeName, ARRAYSIZE(localeName))) {
        if (const LocalePack* locale = LocalePackFromName(localeName)) {
            return locale;
        }
    }
    if (const LocalePack* locale = TryFindLocalePack(GetSystemDefaultUILanguage())) {
        return locale;
    }
    return &g_Locales[0];
}

static std::wstring g_configuredLanguage = L"auto";
static std::mutex g_languageMutex;

static void DetermineLocale() {
    std::wstring requested;
    {
        std::lock_guard<std::mutex> lock(g_languageMutex);
        requested = g_configuredLanguage;
    }

    const LocalePack* selected = nullptr;
    if (!requested.empty() && _wcsicmp(requested.c_str(), L"auto") != 0 && _wcsicmp(requested.c_str(), L"0") != 0) {
        struct LangMapping {
            PCWSTR key;
            LANGID id;
        };
        static const LangMapping kMap[] = {
            {L"en", 0x0409}, {L"english", 0x0409},
            {L"it", 0x0410}, {L"italiano", 0x0410}, {L"italian", 0x0410},
            {L"es", 0x040A}, {L"español", 0x040A}, {L"spanish", 0x040A},
            {L"fr", 0x040C}, {L"français", 0x040C}, {L"french", 0x040C},
            {L"de", 0x0407}, {L"deutsch", 0x0407}, {L"german", 0x0407},
            {L"pt-br", 0x0416}, {L"pt_br", 0x0416}, {L"português (brasil)", 0x0416},
            {L"pt-pt", 0x0816}, {L"pt_pt", 0x0816}, {L"pt", 0x0416}, {L"portuguese", 0x0416},
            {L"ru", 0x0419}, {L"русский", 0x0419}, {L"russian", 0x0419},
            {L"zh-cn", 0x0804}, {L"zh-hans", 0x0804}, {L"zh_cn", 0x0804}, {L"zh", 0x0804}, {L"简体中文", 0x0804},
            {L"zh-tw", 0x0404}, {L"zh-hant", 0x0404}, {L"zh_tw", 0x0404}, {L"繁體中文", 0x0404},
            {L"ja", 0x0411}, {L"日本語", 0x0411}, {L"japanese", 0x0411},
            {L"ko", 0x0412}, {L"한국어", 0x0412}, {L"korean", 0x0412},
            {L"tr", 0x041F}, {L"türkçe", 0x041F}, {L"turkish", 0x041F},
            {L"nl", 0x0413}, {L"nederlands", 0x0413}, {L"dutch", 0x0413},
            {L"pl", 0x0415}, {L"polski", 0x0415}, {L"polish", 0x0415},
            {L"sv", 0x041D}, {L"svenska", 0x041D}, {L"swedish", 0x041D},
            {L"da", 0x0406}, {L"dansk", 0x0406}, {L"danish", 0x0406},
            {L"nb", 0x0414}, {L"no", 0x0414}, {L"norsk", 0x0414}, {L"norwegian", 0x0414},
            {L"fi", 0x040B}, {L"suomi", 0x040B}, {L"finnish", 0x040B},
            {L"cs", 0x0405}, {L"čeština", 0x0405}, {L"czech", 0x0405},
            {L"hu", 0x040E}, {L"magyar", 0x040E}, {L"hungarian", 0x040E},
            {L"el", 0x0408}, {L"ελληνικά", 0x0408}, {L"greek", 0x0408},
            {L"ar", 0x0401}, {L"العربية", 0x0401}, {L"arabic", 0x0401},
            {L"he", 0x040D}, {L"עברית", 0x040D}, {L"hebrew", 0x040D},
            {L"ro", 0x0418}, {L"română", 0x0418}, {L"romanian", 0x0418},
            {L"uk", 0x0422}, {L"українська", 0x0422}, {L"ukrainian", 0x0422},
            {L"bg", 0x0402}, {L"български", 0x0402}, {L"bulgarian", 0x0402},
            {L"sk", 0x041B}, {L"slovenčina", 0x041B}, {L"slovak", 0x041B},
            {L"hr", 0x041A}, {L"hrvatski", 0x041A}, {L"croatian", 0x041A},
            {L"id", 0x0421}, {L"bahasa indonesia", 0x0421}, {L"indonesian", 0x0421},
            {L"af", 0x0436}, {L"afrikaans", 0x0436},
            {L"sq", 0x041C}, {L"albanian", 0x041C},
            {L"am", 0x045E}, {L"amharic", 0x045E},
            {L"hy", 0x042B}, {L"armenian", 0x042B},
            {L"as", 0x044D}, {L"assamese", 0x044D},
            {L"az", 0x042C}, {L"azerbaijani", 0x042C},
            {L"ba", 0x046D}, {L"bashkir", 0x046D},
            {L"eu", 0x042D}, {L"basque", 0x042D},
            {L"be", 0x0423}, {L"belarusian", 0x0423},
            {L"bn", 0x0845}, {L"bengali", 0x0845},
            {L"bs", 0x141A}, {L"bosnian", 0x141A},
            {L"ca", 0x0403}, {L"catalan", 0x0403},
            {L"chr", 0x045C}, {L"cherokee", 0x045C},
            {L"prs", 0x048C}, {L"dari", 0x048C},
            {L"dv", 0x0465}, {L"divehi", 0x0465},
            {L"et", 0x0425}, {L"estonian", 0x0425},
            {L"fil", 0x0464}, {L"filipino", 0x0464},
            {L"fy", 0x0462}, {L"western frisian", 0x0462},
            {L"gl", 0x0456}, {L"galician", 0x0456},
            {L"ka", 0x0437}, {L"georgian", 0x0437},
            {L"gu", 0x0447}, {L"gujarati", 0x0447},
            {L"ha", 0x0468}, {L"hausa", 0x0468},
            {L"hi", 0x0439}, {L"hindi", 0x0439},
            {L"is", 0x040F}, {L"icelandic", 0x040F},
            {L"ig", 0x0470}, {L"igbo", 0x0470},
            {L"ga", 0x083C}, {L"irish", 0x083C},
            {L"xh", 0x0434}, {L"xhosa", 0x0434},
            {L"zu", 0x0435}, {L"zulu", 0x0435},
            {L"kn", 0x044B}, {L"kannada", 0x044B},
            {L"kk", 0x043F}, {L"kazakh", 0x043F},
            {L"km", 0x0453}, {L"khmer", 0x0453},
            {L"rw", 0x0487}, {L"kinyarwanda", 0x0487},
            {L"sw", 0x0441}, {L"kiswahili", 0x0441},
            {L"kok", 0x0457}, {L"konkani", 0x0457},
            {L"ky", 0x0440}, {L"kyrgyz", 0x0440},
            {L"lo", 0x0454}, {L"lao", 0x0454},
            {L"lv", 0x0426}, {L"latvian", 0x0426},
            {L"lt", 0x0427}, {L"lithuanian", 0x0427},
            {L"lb", 0x046E}, {L"luxembourgish", 0x046E},
            {L"mk", 0x042F}, {L"macedonian", 0x042F},
            {L"ms", 0x043E}, {L"malay", 0x043E},
            {L"ml", 0x044C}, {L"malayalam", 0x044C},
            {L"mt", 0x043A}, {L"maltese", 0x043A},
            {L"mi", 0x0481}, {L"maori", 0x0481},
            {L"mr", 0x044E}, {L"marathi", 0x044E},
            {L"mn", 0x0450}, {L"mongolian", 0x0450},
            {L"my", 0x0455}, {L"burmese", 0x0455},
            {L"ne", 0x0461}, {L"nepali", 0x0461},
            {L"nn", 0x0814}, {L"norwegian nynorsk", 0x0814},
            {L"or", 0x0448}, {L"odia", 0x0448},
            {L"ps", 0x0463}, {L"pashto", 0x0463},
            {L"fa", 0x0429}, {L"persian", 0x0429},
            {L"pa", 0x0446}, {L"punjabi", 0x0446},
            {L"quz", 0x046B}, {L"quechua", 0x046B},
            {L"gd", 0x0491}, {L"scottish gaelic", 0x0491},
            {L"sr-Latn", 0x241A}, {L"serbian (latin)", 0x241A},
            {L"sr-Cyrl", 0x281A}, {L"serbian (cyrillic)", 0x281A},
            {L"nso", 0x046C}, {L"northern sotho", 0x046C},
            {L"tn", 0x0432}, {L"tswana", 0x0432},
            {L"si", 0x045B}, {L"sinhala", 0x045B},
            {L"sl", 0x0424}, {L"slovenian", 0x0424},
            {L"tg", 0x0428}, {L"tajik", 0x0428},
            {L"ta", 0x0449}, {L"tamil", 0x0449},
            {L"tt", 0x0444}, {L"tatar", 0x0444},
            {L"te", 0x044A}, {L"telugu", 0x044A},
            {L"th", 0x041E}, {L"thai", 0x041E},
            {L"ti", 0x0473}, {L"tigrinya", 0x0473},
            {L"tk", 0x0442}, {L"turkmen", 0x0442},
            {L"ur", 0x0420}, {L"urdu", 0x0420},
            {L"ug", 0x0480}, {L"uyghur", 0x0480},
            {L"uz", 0x0443}, {L"uzbek", 0x0443},
            {L"vi", 0x042A}, {L"vietnamese", 0x042A},
            {L"cy", 0x0452}, {L"welsh", 0x0452},
            {L"wo", 0x0488}, {L"wolof", 0x0488},
            {L"yo", 0x046A}, {L"yoruba", 0x046A},
            {L"iu", 0x085D}, {L"inuktitut", 0x085D},
            {L"ckb", 0x0492}, {L"central kurdish", 0x0492},
            {L"sd", 0x0459}, {L"sindhi", 0x0459},
            {L"ks", 0x0860}, {L"kashmiri", 0x0860},
            {L"sa", 0x044F}, {L"sanskrit", 0x044F},
            {L"bo", 0x0451}, {L"tibetan", 0x0451},
            {L"wa", 0x0490}, {L"walloon", 0x0490},
            {L"sah", 0x0485}, {L"sakha", 0x0485},
            {L"hsb", 0x042E}, {L"upper sorbian", 0x042E},
            {L"dsb", 0x082E}, {L"lower sorbian", 0x082E},
            {L"br", 0x047E}, {L"breton", 0x047E},
            {L"oc", 0x0482}, {L"occitan", 0x0482},
            {L"co", 0x0483}, {L"corsican", 0x0483},
            {L"kl", 0x046F}, {L"greenlandic", 0x046F},
            {L"fo", 0x0438}, {L"faroese", 0x0438},
        };

        for (const auto& entry : kMap) {
            if (!_wcsicmp(requested.c_str(), entry.key)) {
                selected = FindLocalePack(entry.id);
                break;
            }
        }
        if (!selected) {
            for (const auto& entry : kMap) {
                size_t len = wcslen(entry.key);
                if (!_wcsnicmp(requested.c_str(), entry.key, len)) {
                    selected = FindLocalePack(entry.id);
                    break;
                }
            }
        }
    }

    if (!selected) selected = DetectAutomaticLocale();
    if (!selected) selected = &g_Locales[0];
    g_CurrentLocalePack.store(selected, std::memory_order_release);
    Wh_Log(L"Standalone Open With locale: requested=%s selected=%04X userUI=%04X systemUI=%04X",
           requested.c_str(), selected->langId, GetUserDefaultUILanguage(), GetSystemDefaultUILanguage());
}

static bool AppsUseDarkTheme() {
    DWORD appsUseLightTheme = 1;
    DWORD bytes = sizeof(appsUseLightTheme);
    const LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr,
        &appsUseLightTheme, &bytes);
    return status == ERROR_SUCCESS && appsUseLightTheme == 0;
}

static bool ResolveDarkMode() {
    switch (g_themeMode.load(std::memory_order_acquire)) {
        case ThemeMode::Dark:
            return true;
        case ThemeMode::Light:
            return false;
        case ThemeMode::Auto:
        default:
            return AppsUseDarkTheme();
    }
}

static void LoadSettings() {
    const bool replace = Wh_GetIntSetting(L"replaceSystemDialog") != 0;
    const bool web = Wh_GetIntSetting(L"showWebLink") != 0;
    const bool hideDescription =
        Wh_GetIntSetting(L"hideDescriptionField") != 0;

    WindhawkUtils::StringSetting language =
        WindhawkUtils::StringSetting::make(L"language");
    {
        std::lock_guard<std::mutex> lock(g_languageMutex);
        g_configuredLanguage =
            (language.get() && *language.get()) ? language.get() : L"auto";
    }

    WindhawkUtils::StringSetting darkMode =
        WindhawkUtils::StringSetting::make(L"darkMode");
    ThemeMode theme = ThemeMode::Auto;
    if (darkMode.get() && !_wcsicmp(darkMode.get(), L"dark")) {
        theme = ThemeMode::Dark;
    } else if (darkMode.get() && !_wcsicmp(darkMode.get(), L"light")) {
        theme = ThemeMode::Light;
    }

    WindhawkUtils::StringSetting defaultBehavior =
        WindhawkUtils::StringSetting::make(L"defaultAssociationBehavior");
    const DefaultBehavior behavior =
        defaultBehavior.get() &&
        !_wcsicmp(defaultBehavior.get(), L"openSettings")
            ? DefaultBehavior::OpenSettings
            : DefaultBehavior::Disabled;

    g_defaultBehavior.store(behavior, std::memory_order_release);
    g_themeMode.store(theme, std::memory_order_release);
    g_showWebLink.store(web, std::memory_order_release);
    g_hideDescriptionField.store(hideDescription,
                                 std::memory_order_release);
    g_replaceSystemDialog.store(replace, std::memory_order_release);
    DetermineLocale();
}

// -----------------------------------------------------------------------------
// Picker state and association enumeration.
// -----------------------------------------------------------------------------

struct HandlerEntry {
    ComPtr<StandaloneAssocHandler> handler;
    std::wstring displayName;
    std::wstring internalName;
    std::wstring progId;
    std::wstring companyName;
    bool recommended = false;
    bool browsed = false;
    int imageIndex = -1;
};

struct PickerRequest {
    std::wstring path;
    HWND owner = nullptr;
    // Optional caller-owned event, signaled after the picker request finishes.
    HANDLE completionEvent = nullptr;
    // Properties -> Change selects a default but must not execute the file.
    bool setDefaultOnly = false;
    // Optional caller-owned output flag: true when the user accepted a
    // program, false when the dialog was cancelled. The API contract of
    // SHOpenWithDialog / ShellExecuteExW distinguishes the two (Cancel is
    // HRESULT_FROM_WIN32(ERROR_CANCELLED) / FALSE + ERROR_CANCELLED), so
    // the waiting hooks need more than "the picker finished".
    std::atomic<bool>* acceptedOut = nullptr;
};

struct PickerState {
    PickerRequest request;
    std::vector<HandlerEntry> handlers;
    ImageListOwner images;
    IconOwner headerIcon;
    FontOwner font;
    BrushOwner darkBgBrush;
    BrushOwner darkCardBrush;
    HWND window = nullptr;
    HWND list = nullptr;
    HWND description = nullptr;
    HWND alwaysUse = nullptr;
    // Dark mode runs the "Always use" checkbox as BS_OWNERDRAW, and an
    // owner-drawn button does not keep a reliable BM_GETCHECK state, so
    // the check state lives here. Light mode keeps using BM_GETCHECK on
    // the untouched BS_AUTOCHECKBOX control.
    bool alwaysUseChecked = false;
    int hoverButton = 0;
    // Index of the list row under the pointer, for the Windows 7 flyout
    // style hover frame (dark theme only, -1 when nothing is hovered).
    int hoverRow = -1;
    // Physical pixel size of the program icons in the list image list,
    // resolved from the window DPI in InitializeList.
    int listIconSize = 32;
    // Logical pixels every control below the description box moves up by
    // when the description box is hidden. Captured once in
    // BuildPickerControls so a setting change mid-dialog cannot leave the
    // window and its controls disagreeing about the layout.
    int descriptionOffsetY = 0;
    bool finished = false;
    bool accepted = false;
    bool makeDefaultRequested = false;
    std::wstring associationDescription;
    bool openDefaultSettings = false;
    bool listUsesGroups = false;
    bool hasOtherGroup = false;
    bool isDarkMode = false;
    int chosenIndex = -1;
};

static SHAssocEnumHandlers_t ResolveHandlerEnumerator() {
    static SHAssocEnumHandlers_t function = [] {
        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        return shell32 ? reinterpret_cast<SHAssocEnumHandlers_t>(
                             GetProcAddress(shell32, "SHAssocEnumHandlers"))
                       : nullptr;
    }();
    return function;
}

static std::wstring TakeTaskString(PWSTR value) {
    if (!value) return {};
    std::wstring result;
    try { result = value; } catch (...) {}
    CoTaskMemFree(value);
    return result;
}

static std::wstring ExtensionOf(const std::wstring& path) {
    const wchar_t* fileName = PathFindFileNameW(path.c_str());
    const wchar_t* extension = fileName ? PathFindExtensionW(fileName) : nullptr;
    return extension ? extension : L"";
}

static bool IsSupportedFile(const std::wstring& path) {
    if (path.empty() || path.size() >= 32767) return false;
    const DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

static std::wstring ApplicationProgIdForExecutable(
    const std::wstring& executable) {
    PCWSTR name = PathFindFileNameW(executable.c_str());
    if (!name || !*name) return {};
    try {
        return std::wstring(L"Applications\\") + name;
    } catch (...) {
        return {};
    }
}

static std::wstring ResolveHandlerProgId(const std::wstring& internalName) {
    if (internalName.empty()) return {};
    if (!_wcsnicmp(internalName.c_str(), L"Applications\\", 13) ||
        !_wcsnicmp(internalName.c_str(), L"AppX", 4)) {
        return internalName;
    }
    if (GetFileAttributesW(internalName.c_str()) != INVALID_FILE_ATTRIBUTES)
        return ApplicationProgIdForExecutable(internalName);

    // Some handlers expose a bare executable name.
    if (PathFindExtensionW(internalName.c_str()) &&
        !_wcsicmp(PathFindExtensionW(internalName.c_str()), L".exe")) {
        try {
            return std::wstring(L"Applications\\") +
                   PathFindFileNameW(internalName.c_str());
        } catch (...) {
        }
    }
    // Otherwise GetName commonly returned the ProgID itself.
    return internalName;
}

static HRESULT AddExecutableToOpenWithMru(PCWSTR extension,
                                          PCWSTR executableName) {
    if (!extension || extension[0] != L'.' || !extension[1] ||
        !executableName || !*executableName) {
        return E_INVALIDARG;
    }
    wchar_t keyPath[1024] = {};
    if (swprintf_s(
            keyPath,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
            L"FileExts\\%s\\OpenWithList",
            extension) < 0) {
        return E_FAIL;
    }

    RegKeyOwner key;
    LSTATUS status = RegCreateKeyExW(
        HKEY_CURRENT_USER, keyPath, 0, nullptr, 0, KEY_READ | KEY_WRITE,
        nullptr, key.Put(), nullptr);
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    wchar_t mru[64] = {};
    DWORD bytes = sizeof(mru);
    RegGetValueW(key.Get(), nullptr, L"MRUList", RRF_RT_REG_SZ, nullptr,
                 mru, &bytes);

    wchar_t selectedLetter = 0;
    for (wchar_t letter = L'a'; letter <= L'z'; ++letter) {
        wchar_t name[2] = {letter, 0};
        wchar_t value[MAX_PATH] = {};
        bytes = sizeof(value);
        if (RegGetValueW(key.Get(), nullptr, name, RRF_RT_REG_SZ, nullptr,
                         value, &bytes) == ERROR_SUCCESS) {
            if (!_wcsicmp(value, executableName)) {
                selectedLetter = letter;
                break;
            }
        } else if (!selectedLetter) {
            selectedLetter = letter;
        }
    }
    if (!selectedLetter) return HRESULT_FROM_WIN32(ERROR_TOO_MANY_NAMES);

    wchar_t valueName[2] = {selectedLetter, 0};
    status = RegSetValueExW(
        key.Get(), valueName, 0, REG_SZ,
        reinterpret_cast<const BYTE*>(executableName),
        static_cast<DWORD>((wcslen(executableName) + 1) * sizeof(wchar_t)));
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    std::wstring newMru(1, selectedLetter);
    for (PCWSTR cursor = mru; *cursor; ++cursor) {
        if (*cursor != selectedLetter) newMru.push_back(*cursor);
    }
    status = RegSetValueExW(
        key.Get(), L"MRUList", 0, REG_SZ,
        reinterpret_cast<const BYTE*>(newMru.c_str()),
        static_cast<DWORD>((newMru.size() + 1) * sizeof(wchar_t)));
    return HRESULT_FROM_WIN32(status);
}

static HRESULT EnsureUserApplicationRegistration(
    const std::wstring& executable, const std::wstring& extension,
    std::wstring* progIdOut) {
    if (!IsSupportedFile(executable)) return E_INVALIDARG;
    PCWSTR executableName = PathFindFileNameW(executable.c_str());
    if (!executableName || !*executableName) return E_INVALIDARG;

    std::wstring appPath;
    try {
        appPath = std::wstring(L"Software\\Classes\\Applications\\") +
                  executableName;
    } catch (...) {
        return E_OUTOFMEMORY;
    }

    RegKeyOwner appKey;
    LSTATUS status = RegCreateKeyExW(
        HKEY_CURRENT_USER, appPath.c_str(), 0, nullptr, 0,
        KEY_READ | KEY_WRITE, nullptr, appKey.Put(), nullptr);
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    RegKeyOwner commandKey;
    status = RegCreateKeyExW(appKey.Get(), L"shell\\open\\command", 0,
                             nullptr, 0, KEY_READ | KEY_WRITE, nullptr,
                             commandKey.Put(), nullptr);
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    std::wstring command;
    try {
        command = L"\"" + executable + L"\" \"%1\"";
    } catch (...) {
        return E_OUTOFMEMORY;
    }
    status = RegSetValueExW(
        commandKey.Get(), nullptr, 0, REG_SZ,
        reinterpret_cast<const BYTE*>(command.c_str()),
        static_cast<DWORD>((command.size() + 1) * sizeof(wchar_t)));
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    if (extension.size() > 1 && extension[0] == L'.') {
        RegKeyOwner supportedTypes;
        if (RegCreateKeyExW(appKey.Get(), L"SupportedTypes", 0, nullptr, 0,
                            KEY_READ | KEY_WRITE, nullptr,
                            supportedTypes.Put(), nullptr) == ERROR_SUCCESS) {
            const wchar_t empty[] = L"";
            RegSetValueExW(supportedTypes.Get(), extension.c_str(), 0,
                           REG_SZ, reinterpret_cast<const BYTE*>(empty),
                           sizeof(empty));
        }
        AddExecutableToOpenWithMru(extension.c_str(), executableName);

        std::wstring progId = ApplicationProgIdForExecutable(executable);
        wchar_t progIdsPath[1024] = {};
        if (!progId.empty() &&
            swprintf_s(
                progIdsPath,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
                L"FileExts\\%s\\OpenWithProgids",
                extension.c_str()) >= 0) {
            RegKeyOwner progIds;
            if (RegCreateKeyExW(HKEY_CURRENT_USER, progIdsPath, 0, nullptr,
                                0, KEY_READ | KEY_WRITE, nullptr,
                                progIds.Put(), nullptr) == ERROR_SUCCESS) {
                RegSetValueExW(progIds.Get(), progId.c_str(), 0, REG_NONE,
                               nullptr, 0);
            }
        }
    }

    if (progIdOut)
        *progIdOut = ApplicationProgIdForExecutable(executable);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

static bool HandlerExecutableExists(const std::wstring& executable, const std::wstring& progId) {
    if (!executable.empty()) {
        if (GetFileAttributesW(executable.c_str()) != INVALID_FILE_ATTRIBUTES) return true;
        wchar_t found[MAX_PATH] = {};
        if (SearchPathW(nullptr, executable.c_str(), L".exe", ARRAYSIZE(found), found, nullptr)) return true;
    }
    if (!progId.empty()) {
        wchar_t subKey[512] = {};
        if (swprintf_s(subKey, L"%s\\shell\\open\\command", progId.c_str()) > 0) {
            RegKeyOwner key;
            if (RegOpenKeyExW(HKEY_CLASSES_ROOT, subKey, 0, KEY_READ,
                              key.Put()) == ERROR_SUCCESS) {
                return true;
            }
        }
    }
    return false;
}

static bool IsOpenWithExecutable(const std::wstring& executable) {
    if (executable.empty()) return false;
    PCWSTR fileName = PathFindFileNameW(executable.c_str());
    return fileName && !_wcsicmp(fileName, L"OpenWith.exe");
}

static bool IsOpenWithHandlerName(const std::wstring& internalName,
                                  const std::wstring& progId) {
    if (IsOpenWithExecutable(internalName)) return true;
    if (!_wcsicmp(progId.c_str(), L"Applications\\OpenWith.exe")) return true;
    return StrStrIW(internalName.c_str(), L"\\OpenWith.exe") != nullptr;
}

static bool RegistryValueExists(HKEY key, PCWSTR name) {
    return key && RegQueryValueExW(key, name, nullptr, nullptr, nullptr,
                                   nullptr) == ERROR_SUCCESS;
}

static std::wstring ExecutableFromCommand(PCWSTR command) {
    if (!command || !*command) return {};
    DWORD required = ExpandEnvironmentStringsW(command, nullptr, 0);
    std::wstring expanded;
    try {
        if (required > 1 && required < 32768) {
            std::vector<wchar_t> buffer(required);
            if (ExpandEnvironmentStringsW(command, buffer.data(), required))
                expanded.assign(buffer.data());
        }
        if (expanded.empty()) expanded.assign(command);
    } catch (...) {
        return {};
    }

    ArgvOwner argv(expanded.c_str());
    if (!argv || argv.Count() < 1) return {};
    std::wstring executable;
    try {
        executable.assign(argv[0]);
    } catch (...) {
    }
    if (executable.empty()) return {};

    if (GetFileAttributesW(executable.c_str()) == INVALID_FILE_ATTRIBUTES) {
        wchar_t resolved[MAX_PATH] = {};
        if (SearchPathW(nullptr, executable.c_str(), nullptr,
                        ARRAYSIZE(resolved), resolved, nullptr)) {
            executable.assign(resolved);
        }
    }
    return GetFileAttributesW(executable.c_str()) != INVALID_FILE_ATTRIBUTES
               ? executable
               : std::wstring{};
}

static std::wstring ApplicationDisplayName(HKEY applicationKey,
                                            PCWSTR executableName,
                                            const std::wstring& executable) {
    wchar_t friendly[512] = {};
    DWORD bytes = sizeof(friendly);
    if (applicationKey &&
        RegGetValueW(applicationKey, nullptr, L"FriendlyAppName",
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr,
                     friendly, &bytes) == ERROR_SUCCESS && friendly[0]) {
        return friendly;
    }

    DWORD ignored = 0;
    const DWORD versionBytes =
        GetFileVersionInfoSizeW(executable.c_str(), &ignored);
    if (versionBytes && versionBytes < 16 * 1024 * 1024) {
        try {
            std::vector<BYTE> version(versionBytes);
            if (GetFileVersionInfoW(executable.c_str(), 0, versionBytes,
                                    version.data())) {
                struct Translation { WORD language; WORD codePage; };
                Translation* translations = nullptr;
                UINT translationBytes = 0;
                if (VerQueryValueW(version.data(),
                                   L"\\VarFileInfo\\Translation",
                                   reinterpret_cast<void**>(&translations),
                                   &translationBytes) &&
                    translationBytes >= sizeof(Translation)) {
                    wchar_t query[128] = {};
                    swprintf_s(query,
                               L"\\StringFileInfo\\%04x%04x\\FileDescription",
                               translations[0].language,
                               translations[0].codePage);
                    PWSTR description = nullptr;
                    UINT chars = 0;
                    if (VerQueryValueW(version.data(), query,
                                       reinterpret_cast<void**>(&description),
                                       &chars) && description && chars > 1) {
                        return std::wstring(description, chars - 1);
                    }
                }
            }
        } catch (...) {
        }
    }

    std::wstring display = executableName ? executableName : L"";
    if (display.size() > 4 &&
        !_wcsicmp(display.c_str() + display.size() - 4, L".exe")) {
        display.resize(display.size() - 4);
    }
    return display;
}

// Resolves a possibly-bare executable reference (no directory, relying on
// PATH lookup) to a real file path, the same way ExecutableFromCommand
// does for registry command lines. Returns an empty string if the file
// can't be found at all.
static std::wstring ResolveExecutablePath(const std::wstring& executable) {
    if (executable.empty()) return {};
    if (GetFileAttributesW(executable.c_str()) != INVALID_FILE_ATTRIBUTES)
        return executable;
    wchar_t resolved[MAX_PATH] = {};
    if (SearchPathW(nullptr, executable.c_str(), L".exe",
                    ARRAYSIZE(resolved), resolved, nullptr)) {
        return resolved;
    }
    return {};
}

// Reads a single string from the executable's VERSIONINFO resource
// (\StringFileInfo\<lang><codepage>\<key>, using the file's first
// translation block). Returns an empty string if the resource, the
// translation table or the requested key is missing.
static std::wstring ExecutableVersionString(const std::wstring& executable,
                                            PCWSTR key) {
    DWORD ignored = 0;
    const DWORD versionBytes =
        GetFileVersionInfoSizeW(executable.c_str(), &ignored);
    if (!versionBytes || versionBytes >= 16 * 1024 * 1024) return {};
    try {
        std::vector<BYTE> version(versionBytes);
        if (!GetFileVersionInfoW(executable.c_str(), 0, versionBytes,
                                 version.data())) {
            return {};
        }
        struct Translation { WORD language; WORD codePage; };
        Translation* translations = nullptr;
        UINT translationBytes = 0;
        if (!VerQueryValueW(version.data(), L"\\VarFileInfo\\Translation",
                            reinterpret_cast<void**>(&translations),
                            &translationBytes) ||
            translationBytes < sizeof(Translation)) {
            return {};
        }
        wchar_t query[128] = {};
        swprintf_s(query, L"\\StringFileInfo\\%04x%04x\\%s",
                   translations[0].language, translations[0].codePage, key);
        PWSTR value = nullptr;
        UINT chars = 0;
        if (VerQueryValueW(version.data(), query,
                           reinterpret_cast<void**>(&value), &chars) &&
            value && chars > 1) {
            return std::wstring(value, chars - 1);
        }
    } catch (...) {
    }
    return {};
}

// Publisher/company subtitle shown under the program name, matching the
// second line the real Windows 7 dialog draws under each tile (e.g.
// "BitTorrent, Inc." under "uTorrent"). IAssocHandler::GetName() often
// returns a bare name or an identifier rather than a full path (unlike
// the registry-fallback path, which always has a real executable), so
// this also falls back to resolving the ProgID's own shell\open\command
// the same way HandlerExecutableExists validates it, before giving up.
static std::wstring ExecutableFromProgId(const std::wstring& progId) {
    if (progId.empty()) return {};
    wchar_t subKey[512] = {};
    if (swprintf_s(subKey, L"%s\\shell\\open\\command", progId.c_str()) <= 0)
        return {};
    wchar_t command[32768] = {};
    DWORD bytes = sizeof(command);
    if (RegGetValueW(HKEY_CLASSES_ROOT, subKey, nullptr,
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr, command,
                     &bytes) != ERROR_SUCCESS) {
        return {};
    }
    return ExecutableFromCommand(command);
}

static std::wstring ExecutableCompanyName(const std::wstring& internalName,
                                          const std::wstring& progId = L"") {
    std::wstring resolved = ResolveExecutablePath(internalName);
    if (resolved.empty() && !progId.empty()) resolved = ExecutableFromProgId(progId);
    if (resolved.empty()) return {};
    return ExecutableVersionString(resolved, L"CompanyName");
}

// ReactOS first enumerates HKCR\\Applications, then marks extension-specific
// entries as recommended. This minimal fallback is especially important for a
// file with no extension, for which SHAssocEnumHandlers can return no object.
static bool EnumerateRegistryApplications(PickerState& state) {
    RegKeyOwner applications;
    if (RegOpenKeyExW(HKEY_CLASSES_ROOT, L"Applications", 0, KEY_READ,
                      applications.Put()) != ERROR_SUCCESS) {
        return false;
    }

    DWORD index = 0;
    wchar_t executableName[512] = {};
    for (;;) {
        DWORD nameChars = ARRAYSIZE(executableName);
        const LSTATUS enumeration = RegEnumKeyExW(
            applications.Get(), index++, executableName, &nameChars, nullptr,
            nullptr, nullptr, nullptr);
        if (enumeration == ERROR_NO_MORE_ITEMS) break;
        if (enumeration != ERROR_SUCCESS) continue;

        RegKeyOwner application;
        if (RegOpenKeyExW(applications.Get(), executableName, 0, KEY_READ,
                          application.Put()) != ERROR_SUCCESS) {
            continue;
        }
        const bool hidden =
            RegistryValueExists(application.Get(), L"NoOpenWith") ||
            RegistryValueExists(application.Get(), L"NoStartPage");
        if (hidden) continue;

        wchar_t command[32768] = {};
        DWORD bytes = sizeof(command);
        if (RegGetValueW(application.Get(), L"shell\\open\\command", nullptr,
                         RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr,
                         command, &bytes) != ERROR_SUCCESS) {
            continue;
        }
        std::wstring executable = ExecutableFromCommand(command);
        if (executable.empty() || IsOpenWithExecutable(executable)) continue;

        bool duplicate = false;
        for (const HandlerEntry& existing : state.handlers) {
            if (!existing.internalName.empty() &&
                !_wcsicmp(existing.internalName.c_str(), executable.c_str())) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate && HandlerExecutableExists(executable, ApplicationProgIdForExecutable(executable))) {
            HandlerEntry entry;
            // Registry-enumerated applications are NOT flagged browsed:
            // they already have a registered ProgID with a command
            // template, so they launch through InvokeSelectedHandler
            // with their own template and arguments. Flagging them
            // browsed would write HKCU\...\Applications keys the user
            // never asked for and launch them as a bare
            // "app.exe" "file" command line. Only entries the user
            // picked through the Browse dialog carry browsed = true.
            entry.internalName = executable;
            entry.progId = ApplicationProgIdForExecutable(executable);
            entry.displayName = ApplicationDisplayName(
                application.Get(), executableName, executable);
            entry.companyName = ExecutableCompanyName(executable, entry.progId);
            if (!entry.displayName.empty())
                state.handlers.push_back(std::move(entry));
        }
    }

    std::stable_sort(state.handlers.begin(), state.handlers.end(),
        [](const HandlerEntry& a, const HandlerEntry& b) {
            if (a.recommended != b.recommended)
                return a.recommended > b.recommended;
            return _wcsicmp(a.displayName.c_str(), b.displayName.c_str()) < 0;
        });
    Wh_Log(L"Standalone Open With: registry application fallback count=%u",
           static_cast<unsigned int>(state.handlers.size()));
    return !state.handlers.empty();
}

static bool EnumerateHandlers(PickerState& state) {
    SHAssocEnumHandlers_t enumerate = ResolveHandlerEnumerator();
    ComPtr<StandaloneEnumAssocHandlers> e;
    if (enumerate) {
        enumerate(ExtensionOf(state.request.path).c_str(), 0, e.Put());
    }

    while (e) {
        StandaloneAssocHandler* raw = nullptr;
        ULONG fetched = 0;
        if (e->Next(1, &raw, &fetched) != S_OK || fetched != 1 || !raw) break;
        HandlerEntry entry;
        entry.handler.Reset(raw);
        PWSTR value = nullptr;
        if (SUCCEEDED(entry.handler->GetUIName(&value)))
            entry.displayName = TakeTaskString(value);
        else if (value) CoTaskMemFree(value);
        value = nullptr;
        if (SUCCEEDED(entry.handler->GetName(&value)))
            entry.internalName = TakeTaskString(value);
        else if (value) CoTaskMemFree(value);
        if (entry.displayName.empty()) entry.displayName = entry.internalName;
        if (entry.displayName.empty()) continue;
        entry.progId = ResolveHandlerProgId(entry.internalName);
        if (IsOpenWithHandlerName(entry.internalName, entry.progId))
            continue;
        if (!HandlerExecutableExists(entry.internalName, entry.progId))
            continue;
        entry.recommended = entry.handler->IsRecommended() == S_OK;
entry.companyName = ExecutableCompanyName(entry.internalName);
        state.handlers.push_back(std::move(entry));
    }


    std::stable_sort(state.handlers.begin(), state.handlers.end(),
        [](const HandlerEntry& a, const HandlerEntry& b) {
            if (a.recommended != b.recommended) return a.recommended > b.recommended;
            return _wcsicmp(a.displayName.c_str(), b.displayName.c_str()) < 0;
        });

    if (state.handlers.empty() || ExtensionOf(state.request.path).empty())
        EnumerateRegistryApplications(state);
    return !state.handlers.empty();
}

static HICON DefaultAppIcon(int size = 0) {
    if (size > 0) {
        HICON scaled = static_cast<HICON>(
            LoadImageW(nullptr, IDI_APPLICATION, IMAGE_ICON, size, size,
                       LR_DEFAULTCOLOR));
        if (scaled) return scaled;
    }
    HICON shared = LoadIconW(nullptr, IDI_APPLICATION);
    return shared ? CopyIcon(shared) : nullptr;
}

// Loads a program icon at an explicit pixel size.
//
// ExtractIconExW and SHGFI_LARGEICON both hand back whatever the system
// large-icon size happens to be (32 pixels at 96 DPI) and the image list
// then stretch-blits that into its own cell. Asking the extractor for the
// exact size instead lets it pick the right frame out of the executable's
// icon group, which is what keeps the icons sharp once the list icons are
// DPI-scaled past 32 pixels.
static HICON EntryIcon(const HandlerEntry& entry, int iconSize) {
    if (iconSize <= 0) iconSize = 32;
    if (entry.handler) {
        PWSTR raw = nullptr;
        int index = 0;
        if (SUCCEEDED(entry.handler->GetIconLocation(&raw, &index)) && raw &&
            *raw && *raw != L'@') {
            const std::wstring location = TakeTaskString(raw);
            HICON icon = nullptr;
            if (SUCCEEDED(SHDefExtractIconW(location.c_str(), index, 0, &icon,
                                            nullptr,
                                            static_cast<UINT>(iconSize))) &&
                icon) {
                return icon;
            }
            icon = nullptr;
            if (ExtractIconExW(location.c_str(), index, &icon, nullptr, 1) > 0 && icon)
                return icon;
        } else if (raw) {
            CoTaskMemFree(raw);
        }
    }
    if (!entry.internalName.empty()) {
        HICON icon = nullptr;
        if (SUCCEEDED(SHDefExtractIconW(entry.internalName.c_str(), 0, 0,
                                        &icon, nullptr,
                                        static_cast<UINT>(iconSize))) &&
            icon) {
            return icon;
        }
        SHFILEINFOW info{};
        UINT flags = SHGFI_ICON | SHGFI_LARGEICON;
        DWORD attributes = GetFileAttributesW(entry.internalName.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            attributes = FILE_ATTRIBUTE_NORMAL;
            flags |= SHGFI_USEFILEATTRIBUTES;
        }
        if (SHGetFileInfoW(entry.internalName.c_str(), attributes, &info,
                           sizeof(info), flags) && info.hIcon)
            return info.hIcon;
    }
    return DefaultAppIcon(iconSize);
}

// 32x32 BGRA artwork derived from the user-supplied transparent PNG
// (document + magnifier), Lanczos-resampled at build time and encoded as raw
// top-down BGRA Base64 so the mod remains a single source file.
static const char kStandaloneIconBase64[] =
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAQAAAAEAAAABAAAAAQAAAAEAAAABAAAAAQAAAAEAAAABAAAA"
    "AQAAAAEAAAABAAAAAgAAAAAAAAAA////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAKqq"
    "qgMAAAAAqKSgQdPRztTR0M3Qz87M0s/NzNLOzcrSzszJ0s3KydLMycfTzMnH0szJxdLJx8TTyMbD08vIxdHIxMHTurWzsoiIiA8A"
    "AAAAf39/AgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAv7+/BAAAAACppqNZ////////////////"
    "//////////////////////////////////////////////////////r49v/k39z/zcbCw1xcRQsBAQEAf39/AgAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC/v38EAAAAAKilolP49/b9////+fr6+vz7+/z8+/v8/Pv7+/z7+/v7+/r5+/v6"
    "+fv7+fj7+/n3+/r49vv8+vn78O3r+9bT0fjz7ur/xL+6vT8fHwj///8Bf39/AgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAL9/fwQAAAAApqOgVPj39//////7+/v7/vz8/P77+/v+/Pv7/fz7+f38+/n9/Pr5/fv5+P37+ff9+vj2/f37+f3w7ev9"
    "2tfU/fj18vjw6ub/xsG8tQAAAAX///8Bf39/AgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAv39/BAAAAACjoJ1U+Pf3"
    "//////v7+/v+/Pz8/fz7+v38+/r9/Pv5/fz6+P37+fj9+/n3/fr49v36+PX9/Pn3/fDt6v3b2NX9//z6/fXx7/jx7Of/w725sAAA"
    "AAP///8B////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC/f38EAAAAAKKfnFX49/b/////+/v7/P38+/r9/Pv5/fz6+f38"
    "+fj9+/n4/fv59/37+ff9+/j2/fr39f37+fb97+zq/dvY1f3////9//37/fr39fj28Oz/xL66qlVVVQP///8CAAAAAQAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAH9/fwQAAAAAo52aVPf29v/////6+/r5/fz7+f38+vn9+/n3/fv5+P37+ff9+/j2/fr39f359/X9+fb0"
    "/fr29P318vD90s7K/dLNyf3Tzsr90s3J/dDLx/jZ0sz/o56ZlAAAAACqqqoDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAf39/BAAA"
    "AACgnZdU9vX0//////r7+vj9+/n4/fz6+P3//Pr9/fv5/fz69/37+PX9+/j1/fr39f349fP9+PXy/fn18/3z7+397Onm/e3p5v3s"
    "6Ob97ern+/Xx7/2ppKHUAAAAAP///wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB/f38EAAAAAJ+ZllX29PP////9+vr49/3//fv9"
    "+vj4/evp6f3l4d795d/a/ejk4/3r6uv98/Du/fr28/349fL99/Tx/fn18/379/X9+vf0/fr39P369/T7//36/6ynpMoAAAAA////"
    "AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH9/fwQAAAAAnJmWVfb08v////36//37/eXi4f3Jv7b93MSp/enJof3ryJz96ceb/d/E"
    "o/3c08v96ebm/ffz7/349fL99/Tx/ff08f339PH99/Tx/fj08vv++vf/qqWizQAAAAD///8BAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAf39/BAAAAACdmpRU9fPx//////rV0dD9uqiU/e7Qqv3/58n9/erS/f3q1P3858/9+d27/fDNpP3XyLj93trZ/fn08f338/H9"
    "9/Tx/ff08f339PH99/Tx+/369/+qpaHNAAAAAP///wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB/f38EAAAAAJmWk1X49fP/7evr"
    "+ruplP321Kz9//Xm/fv6+P37+PP9++7f/fvo0/387t79/e7e/fLWs/3YzcD96OTi/fj08f328vD99/Tx/fbz8P339PH6/fn2/6um"
    "oswAAAAA////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH9/fwQAAAAAnJmWVfLv7v/d1c766cWY/f/u2v36+vf9/Pn1/fzx5P37"
    "58/9++nU/fvs2v377+L9//Pl/erStf3NyMT98+/s/ff08f328u/99fLv/ff08fr9+fb/q6aizAAAAAD///8BAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAf39/BAAAAACmoJpU4uDg/8myl/r31a79/PHk/fvt3P3769n9++TJ/fvn0P377Nr9++/g/fvx5f389/H9+uXO"
    "/b2xpP3f3Nr9+vbz/fXx7/z18u/89/Pw+vz49f+qpaHNAAAAAP///wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB/f38EAAAAAK2m"
    "oFTOy8r/xaaB+v/jxP3669j9+uTL/frjyP3758/9++rW/fvt3f378OP9+/Pp/fv49P3/8N39uqqX/cnGxf38+PX99PDt/fXx7v32"
    "8u/7/Pf0/6qloM0AAAAA////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH9/fwQAAAAAqKWfVcrGxf/JqYP6/+PD/frlzf365Mv9"
    "++bP/fvp1f377Nr9++7g/fvx5f379Oz9+/v8/f/v2/2un439v7y6/f359v3z7+z89fHt/fbx7vv89/T/qKOgzAAAAAD///8BAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAf39/BAAAAACjnZdU29nZ/9q+oPr63Ln9++bP/fvmz/366dX9++zb/fvu4P378eT9+/Po/fz4"
    "9P38+ff9++TL/ZKIfP3Fwr/9+/j1/fPu6/318Oz99fDt+/r28v+oo57MAAAAAP///wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB/"
    "f38EAAAAAKCal1Tm4+L/z7+u+vDNpP3969b9+unV/fvt3f378OL9+/Lm/fvz6f389/H9+fn6/f/67/3Wwaj9fHhz/eDc2P349PH9"
    "9O/r/fPv6/307+z7+vXx/6mkn80AAAAA////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH9/fwQAAAAAl5SRVPDs6f/Hwr/62r6e"
    "/f3kyP368OP9+u/i/fvy5/379Ov9+/fy/fv7+/3/+O/99OLN/Y+Gev2gnJb98u/t/fTv6/3z7uv98+7q/fTv7Pr69fH/qKKezAAA"
    "AAD///8BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAf39/BAAAAACUkYtU9PHv/+nl4vrQycP98Na6/f/t1/3/+e/9//jx/f338f3/"
    "9+/9/+/e/e7Yvv2jmIz9jomC/dnW0v318e798+7q/fPu6v3y7en99O/r+vn07/+nop3MAAAAAP///wEAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAB/f38EAAAAAZmWkFX18u//497Y+ru2s/2oop39xrWj/d/KtP3038X99uHH/fDbwv3cy7j9rKSb/ZaQi/3LyMX98/Dt"
    "/fPv6/3z7+v98+7q/fLt6f3z7un7+fTv/6einc0AAAAA////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGZmZgUAAAAAjImCUNHL"
    "xv/Z1NH6eHJs/YB4cP2rpaH9kol+/aKXi/21q5/9vLOq/bOspP2xq6T919TR/fTw7f307+z99O/r/fTv6/3z7ur98u3p/fPt6fv4"
    "8u7/pqGczQAAAAD///8BAAAAAAAAAAAAAAAAAAAAAAAAAAD///8B////AhwAAAmhm5TCzMfE/oN8ePt6cmf94d/e/fDt6/3a1tP9"
    "xsK//cK+uv3Hw7791tLP/evp5/328u/99O/s/fPu6v3z7ur98u3p/fPt6f3y7Of98ezn+/nz7v+moZzNAAAAAP///wEAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAaqqqgMAAAAAmJGJrtbT0P+GhYT6PjYt/bGtqP308vD99/Pw/fn28/359vP9+PXz/fj18/359fL89vHt/fTv"
    "7P3z7ur98+7q/fLu6v3y7en98ezn/fHr5v3x6+b79/Ht/6ehncwAAAAA////AQAAAAAAAAAAAAAAAAAAAAC/v78EAAAAAI2Gf5LQ"
    "zcr/lJOT+EQ+Nv2BdGb91dPS/ff08f318u/99vLv/fby7v328e799fDs/fXw7P307+v98+7q/fPu6v3y7en98u3o/fHr5/3x6+b9"
    "8evm/fLs5/r38uz/pqGczAAAAAD///8BAAAAAAAAAAAAAAAAqlVVAwAAAACFfnVzx8PA/6SjovdHQz38eGxa/cPAv/zw7ev89fHu"
    "/PTw7f318e789PDt/PTv6/z07+v88+7q/PLt6fzy7en88ezn/PDr5vzw6uX88Orl/PDq5fzw6uT87+nk+/bw6v6noZzMAAAAAP//"
    "/wEAAAAAAAAAAH9/fwIAAAAAioF1Vbeyrv+urKz5UExI/2pdS/++u7f98vDv/vv49P78+PX9/Pj1/fz49f38+PT9+/fz/vr28v76"
    "9fD++vXw/vj07/738u3+9/Hs/vfx7P738ez+9/Hr/vfx6/748ev8//nz/6mkn80AAAAA////AQAAAAAAAAAA////AQAAAACGem/c"
    "xLu1/2NeWv5fUj/ihXtvfLi0sv3EwLv7wr66/MG9ufzBvbn8wby4/MG8uPy/urb8wLu2/L+6tvy/urb8vrm1/L+5tfy+uLP8vriz"
    "/L24s/y+uLP8vriz/L24s/rCvbf/l5KMyAAAAAD///8BAAAAAAAAAABVVVUDAAAAAHJiVFtqW0z5bl9L+YNuWTwAAAAAZ2FVKlVO"
    "RydVTkcnVU5HJ1VORydVTkcnVU5HJ1VORydVTkcnTkdHJ05ORydVTkcnVU5HJ05HRydOR0cnTkdHJ05HRydVTkcnTkdHJ1JMRihn"
    "Xl4bAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAEY4IyRdSjcpAAAAAP8AAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAB/f38CAAAAAAAAAAB/f38CAAAAAH9/fwJ/f38Cf39/An9/fwJ/f38Cf39/An9/fwJ/f38Cf39/An9/fwJ/f38Cf39/An9/"
    "fwJ/f38Cf39/An9/fwJ/f38Cf39/An9/fwJ/f38Cf39/AgAAAAAAAAAAAAAAAA==";

static int Base64Digit(unsigned char value) {
    if (value >= 'A' && value <= 'Z') return value - 'A';
    if (value >= 'a' && value <= 'z') return value - 'a' + 26;
    if (value >= '0' && value <= '9') return value - '0' + 52;
    if (value == '+') return 62;
    if (value == '/') return 63;
    return -1;
}

static bool DecodeBase64(PCSTR encoded, std::vector<BYTE>& output) {
    output.clear();
    if (!encoded) return false;
    try {
        output.reserve(4096);
        unsigned int accumulator = 0;
        int bits = 0;
        for (const unsigned char* cursor =
                 reinterpret_cast<const unsigned char*>(encoded);
             *cursor; ++cursor) {
            if (*cursor == '=') break;
            const int digit = Base64Digit(*cursor);
            if (digit < 0) {
                if (*cursor == ' ' || *cursor == '\r' || *cursor == '\n' ||
                    *cursor == '\t') {
                    continue;
                }
                return false;
            }
            accumulator = (accumulator << 6) | static_cast<unsigned int>(digit);
            bits += 6;
            if (bits >= 8) {
                bits -= 8;
                output.push_back(static_cast<BYTE>(accumulator >> bits));
                accumulator &= (1u << bits) - 1u;
            }
        }
    } catch (...) {
        output.clear();
        return false;
    }
    return output.size() == 32u * 32u * 4u;
}

// Bilinear resampler for the embedded 32x32 BGRA artwork.
//
// The header icon used to be handed to a 34x34 SS_REALSIZECONTROL static,
// which made USER32 stretch a 32x32 icon by a fractional factor with
// nearest-neighbour sampling - that is what made the artwork look squashed
// and blurry, and it got dramatically worse at non-100% DPI. Producing the
// icon at exactly the pixel size the control will use, with proper
// filtering on premultiplied alpha, keeps every edge crisp instead.
static bool ResampleIconPixels(const std::vector<BYTE>& source, int sourceSize,
                               int targetSize, std::vector<BYTE>& target) {
    if (sourceSize <= 0 || targetSize <= 0) return false;
    if (source.size() !=
        static_cast<size_t>(sourceSize) * sourceSize * 4u) {
        return false;
    }
    try {
        target.assign(static_cast<size_t>(targetSize) * targetSize * 4u, 0);
    } catch (...) {
        return false;
    }
    if (targetSize == sourceSize) {
        target = source;
        return true;
    }

    // Premultiply once up front: interpolating straight (non-premultiplied)
    // BGRA bleeds the color of fully transparent pixels into the edges and
    // leaves a dark halo around the artwork.
    std::vector<float> premultiplied;
    try {
        premultiplied.assign(
            static_cast<size_t>(sourceSize) * sourceSize * 4u, 0.0f);
    } catch (...) {
        return false;
    }
    for (size_t i = 0; i < premultiplied.size(); i += 4) {
        const float alpha = source[i + 3] / 255.0f;
        premultiplied[i + 0] = source[i + 0] * alpha;
        premultiplied[i + 1] = source[i + 1] * alpha;
        premultiplied[i + 2] = source[i + 2] * alpha;
        premultiplied[i + 3] = source[i + 3];
    }

    const float scale = static_cast<float>(sourceSize) / targetSize;
    for (int y = 0; y < targetSize; ++y) {
        // Sample at pixel centers so the result stays centered rather than
        // drifting half a pixel toward the top-left corner.
        const float sourceY = (y + 0.5f) * scale - 0.5f;
        int y0 = static_cast<int>(floorf(sourceY));
        float weightY = sourceY - y0;
        int y1 = y0 + 1;
        y0 = std::clamp(y0, 0, sourceSize - 1);
        y1 = std::clamp(y1, 0, sourceSize - 1);
        for (int x = 0; x < targetSize; ++x) {
            const float sourceX = (x + 0.5f) * scale - 0.5f;
            int x0 = static_cast<int>(floorf(sourceX));
            float weightX = sourceX - x0;
            int x1 = x0 + 1;
            x0 = std::clamp(x0, 0, sourceSize - 1);
            x1 = std::clamp(x1, 0, sourceSize - 1);

            const size_t i00 =
                (static_cast<size_t>(y0) * sourceSize + x0) * 4u;
            const size_t i01 =
                (static_cast<size_t>(y0) * sourceSize + x1) * 4u;
            const size_t i10 =
                (static_cast<size_t>(y1) * sourceSize + x0) * 4u;
            const size_t i11 =
                (static_cast<size_t>(y1) * sourceSize + x1) * 4u;
            const size_t out =
                (static_cast<size_t>(y) * targetSize + x) * 4u;
            for (int channel = 0; channel < 4; ++channel) {
                const float top =
                    premultiplied[i00 + channel] * (1.0f - weightX) +
                    premultiplied[i01 + channel] * weightX;
                const float bottom =
                    premultiplied[i10 + channel] * (1.0f - weightX) +
                    premultiplied[i11 + channel] * weightX;
                const float value =
                    top * (1.0f - weightY) + bottom * weightY;
                target[out + channel] = static_cast<BYTE>(
                    std::clamp(value + 0.5f, 0.0f, 255.0f));
            }
        }
    }
    return true;
}

// Builds the header icon at an exact pixel size. size <= 0 means "native
// 32x32". The returned icon is 32-bit BGRA with a real alpha channel.
static HICON LoadStandaloneIcon(int size = 0) {
    std::vector<BYTE> pixels;
    if (!DecodeBase64(kStandaloneIconBase64, pixels))
        return DefaultAppIcon(size);

    constexpr int kNativeSize = 32;
    int targetSize = size > 0 ? size : kNativeSize;
    // Guard rail against a nonsensical DPI value producing a huge bitmap.
    targetSize = std::clamp(targetSize, 8, 256);
    if (targetSize != kNativeSize) {
        std::vector<BYTE> scaled;
        if (ResampleIconPixels(pixels, kNativeSize, targetSize, scaled)) {
            pixels.swap(scaled);
        } else {
            targetSize = kNativeSize;
        }
    }

    BITMAPV5HEADER header{};
    header.bV5Size = sizeof(header);
    header.bV5Width = targetSize;
    header.bV5Height = -targetSize;  // top-down, matching the byte order
    header.bV5Planes = 1;
    header.bV5BitCount = 32;
    header.bV5Compression = BI_BITFIELDS;
    header.bV5RedMask = 0x00FF0000;
    header.bV5GreenMask = 0x0000FF00;
    header.bV5BlueMask = 0x000000FF;
    header.bV5AlphaMask = 0xFF000000;

    void* colorBits = nullptr;
    BitmapOwner color;
    {
        WindowDcOwner screen(nullptr);
        if (screen) {
            color.Reset(CreateDIBSection(
                screen.Get(), reinterpret_cast<BITMAPINFO*>(&header),
                DIB_RGB_COLORS, &colorBits, nullptr, 0));
        }
    }
    if (!color || !colorBits) return DefaultAppIcon(targetSize);
    memcpy(colorBits, pixels.data(), pixels.size());

    // Mask scanlines are DWORD aligned; an all-zero mask means "take the
    // alpha channel from the color bitmap".
    const size_t maskStride = ((static_cast<size_t>(targetSize) + 31) / 32) * 4u;
    std::vector<BYTE> maskBits;
    try {
        maskBits.assign(maskStride * targetSize, 0);
    } catch (...) {
        return DefaultAppIcon(targetSize);
    }
    BitmapOwner mask(
        CreateBitmap(targetSize, targetSize, 1, 1, maskBits.data()));
    if (!mask) return DefaultAppIcon(targetSize);

    // CreateIconIndirect copies both bitmaps, so the owners here still
    // release the originals when they go out of scope.
    ICONINFO info{};
    info.fIcon = TRUE;
    info.hbmColor = color.Get();
    info.hbmMask = mask.Get();
    HICON icon = CreateIconIndirect(&info);
    return icon ? icon : DefaultAppIcon(targetSize);
}

// -----------------------------------------------------------------------------
// Native window and layout.
// -----------------------------------------------------------------------------

enum : int {
    IDC_SOW_ICON = 100,
    IDC_SOW_INSTRUCTION,
    IDC_SOW_FILE_LABEL,
    IDC_SOW_FILE_NAME,
    IDC_SOW_PROGRAMS,
    IDC_SOW_DESCRIPTION_LABEL,
    IDC_SOW_DESCRIPTION,
    IDC_SOW_ALWAYS_USE,
    IDC_SOW_BROWSE,
    IDC_SOW_WEB,
};

enum : int { GROUP_RECOMMENDED = 1, GROUP_OTHER = 2 };
static constexpr UINT WM_SOW_ACTIVATE = WM_APP + 0x217;
static constexpr UINT WM_SOW_SETTINGS_CHANGED = WM_APP + 0x218;
static const wchar_t kWindowClass[] = L"WindhawkStandaloneWin7OpenWith";
static std::atomic<HWND> g_currentWindow{nullptr};

static HINSTANCE ModInstance() {
    static HINSTANCE instance = [] {
        HMODULE module = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&ModInstance), &module);
        return reinterpret_cast<HINSTANCE>(module);
    }();
    return instance;
}

static UINT WindowDpi(HWND owner) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    using GetDpiForSystem_t = UINT(WINAPI*)();
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto getWindowDpi = user32 ? reinterpret_cast<GetDpiForWindow_t>(
                                     GetProcAddress(user32, "GetDpiForWindow"))
                               : nullptr;
    auto getSystemDpi = user32 ? reinterpret_cast<GetDpiForSystem_t>(
                                     GetProcAddress(user32, "GetDpiForSystem"))
                               : nullptr;
    UINT dpi = 96;
    if (owner && getWindowDpi) dpi = getWindowDpi(owner);
    else if (getSystemDpi) dpi = getSystemDpi();
    return dpi >= 48 && dpi <= 768 ? dpi : 96;
}

static int DpiScale(int value, UINT dpi) { return MulDiv(value, dpi, 96); }

// Logical size of the program icons in the list. Windows 7 shows large
// (32 pixel) shell icons here at 96 DPI; the value is DPI-scaled at use.
static constexpr int kListIconSize = 32;

static void ApplyFont(HWND window, HFONT font) {
    if (window && font) SendMessageW(window, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
}

static HWND Child(HWND parent, DWORD exStyle, PCWSTR cls, PCWSTR text,
                  DWORD style, int x, int y, int width, int height, int id,
                  UINT dpi, HFONT font) {
    HWND window = CreateWindowExW(exStyle, cls, text, WS_CHILD | WS_VISIBLE | style,
        DpiScale(x, dpi), DpiScale(y, dpi), DpiScale(width, dpi),
        DpiScale(height, dpi), parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        ModInstance(), nullptr);
    ApplyFont(window, font);
    return window;
}

// Forward declarations: used by the window procedure and by
// ApplyPickerTheme before their definitions further down.
static void EnsureAlwaysUseCheckbox(PickerState& state);
static LRESULT CALLBACK PickerListSubclassProc(HWND window, UINT message,
                                               WPARAM wParam, LPARAM lParam,
                                               UINT_PTR idSubclass,
                                               DWORD_PTR referenceData);
static bool AlwaysUseChecked(PickerState& state);
static void SetAlwaysUseChecked(PickerState& state, bool checked);
static bool SelectedHandlerLaunchable(const HandlerEntry& entry);

static void RefreshPickerThemeResources(PickerState& state) {
    state.isDarkMode = ResolveDarkMode();
    if (state.isDarkMode) {
        if (!state.darkBgBrush)
            state.darkBgBrush.Reset(CreateSolidBrush(RGB(32, 32, 32)));
        if (!state.darkCardBrush)
            state.darkCardBrush.Reset(CreateSolidBrush(RGB(45, 45, 45)));
    } else {
        state.darkBgBrush.Reset();
        state.darkCardBrush.Reset();
    }
}

static void SetImmersiveDarkTitleBar(HWND window, bool enabled) {
    if (!window) return;
    BOOL useDark = enabled ? TRUE : FALSE;
    HRESULT hr = DwmSetWindowAttribute(
        window, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
    if (FAILED(hr) &&
        DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 !=
            DWMWA_USE_IMMERSIVE_DARK_MODE) {
        DwmSetWindowAttribute(
            window, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1,
            &useDark, sizeof(useDark));
    }
}

static void ApplyPickerTheme(PickerState& state) {
    RefreshPickerThemeResources(state);
    SetImmersiveDarkTitleBar(state.window, state.isDarkMode);
    // Must run before the per-control SetWindowTheme calls below: without
    // this, comctl32 never switches its internal group-header, selection,
    // client-edge and scrollbar palettes to the dark variants, even though
    // SetWindowTheme(..., L"DarkMode_Explorer", ...) is applied further down.
    DarkModeActivation::Apply(state.window, state.isDarkMode);

    if (state.list) {
        if (state.isDarkMode) {
            SetWindowTheme(state.list, L"DarkMode_Explorer", nullptr);
            ListView_SetBkColor(state.list, RGB(32, 32, 32));
            ListView_SetTextBkColor(state.list, RGB(32, 32, 32));
            ListView_SetTextColor(state.list, RGB(240, 240, 240));
        } else {
            SetWindowTheme(state.list, L"Explorer", nullptr);
            ListView_SetBkColor(state.list, RGB(255, 255, 255));
            ListView_SetTextBkColor(state.list, RGB(255, 255, 255));
            ListView_SetTextColor(state.list, RGB(0, 0, 0));
        }
    }

    const bool dark = state.isDarkMode;

    // "Always use" checkbox. Dark mode needs a BS_OWNERDRAW control so
    // the glyph can be painted as a dark square with a gray border and
    // an azure check (see WM_DRAWITEM), including its own label text:
    // no companion static is used anymore, so nothing can repaint the
    // text with a dark color while hovering. The button type style
    // cannot be switched reliably after creation, so the control is
    // recreated whenever the theme requires a different type. Light
    // mode keeps the classic BS_AUTOCHECKBOX control untouched.
    EnsureAlwaysUseCheckbox(state);

    // Buttons: owner-drawn in dark mode with the network flyout dark
    // palette (see WM_DRAWITEM); light mode keeps the standard button
    // styles untouched.
    static const int kOwnerDrawButtons[] = {IDOK, IDCANCEL, IDC_SOW_BROWSE};
    for (const int id : kOwnerDrawButtons) {
        HWND button = GetDlgItem(state.window, id);
        if (!button) continue;
        LONG_PTR style = GetWindowLongPtrW(button, GWL_STYLE);
        if (dark) {
            style |= BS_OWNERDRAW;
            SetWindowTheme(button, L"DarkMode_Explorer", nullptr);
        } else {
            style &= ~BS_OWNERDRAW;
            SetWindowTheme(button, nullptr, nullptr);
        }
        SetWindowLongPtrW(button, GWL_STYLE, style);
        SetWindowPos(button, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                         SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    // The description edit drops its light 3D client edge in dark mode:
    // the sunken white border would otherwise stay visible against the
    // dark background. WS_BORDER is not used because the non-client
    // border is painted with the black COLOR_WINDOWFRAME, which is
    // invisible on the dark background; instead the dialog paints a
    // 1 pixel gray frame around the control in WM_PAINT (see
    // PaintDarkControlFrames).
    if (state.description) {
        LONG_PTR exStyle = GetWindowLongPtrW(state.description, GWL_EXSTYLE);
        LONG_PTR style = GetWindowLongPtrW(state.description, GWL_STYLE);
        if (dark) {
            exStyle &= ~WS_EX_CLIENTEDGE;
            style &= ~WS_BORDER;
        } else {
            exStyle |= WS_EX_CLIENTEDGE;
            style &= ~WS_BORDER;
        }
        SetWindowLongPtrW(state.description, GWL_EXSTYLE, exStyle);
        SetWindowLongPtrW(state.description, GWL_STYLE, style);
        SetWindowPos(state.description, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                         SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    for (HWND child = state.window ? GetWindow(state.window, GW_CHILD) : nullptr;
         child; child = GetWindow(child, GW_HWNDNEXT)) {
        if (child == state.list || child == state.alwaysUse) {
            continue;
        }
        wchar_t className[32] = {};
        GetClassNameW(child, className, ARRAYSIZE(className));
        // Static labels stay unthemed in dark mode so WM_CTLCOLORSTATIC
        // can draw them with the light text color: themed statics paint
        // their text with the theme's own color, ignoring SetTextColor(),
        // which would leave dark text on the dark background. Light mode
        // already left them unthemed, so nothing changes there.
        if (_wcsicmp(className, WC_STATICW) == 0) {
            SetWindowTheme(child, nullptr, nullptr);
            continue;
        }
        SetWindowTheme(child, dark ? L"DarkMode_Explorer" : nullptr,
                       nullptr);
    }
    if (state.window) {
        RedrawWindow(state.window, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_FRAME |
                         RDW_ALLCHILDREN);
    }
}

static int SelectedIndex(PickerState& state) {
    const int item = state.list ? ListView_GetNextItem(state.list, -1, LVNI_SELECTED) : -1;
    if (item < 0) return -1;
    LVITEMW value{};
    value.mask = LVIF_PARAM;
    value.iItem = item;
    return ListView_GetItem(state.list, &value) ? static_cast<int>(value.lParam) : -1;
}

static void UpdateSelectionUi(PickerState& state) {
    const int index = SelectedIndex(state);
    const bool valid =
        index >= 0 && static_cast<size_t>(index) < state.handlers.size();
    EnableWindow(GetDlgItem(state.window, IDOK), valid);

    // A default association only makes sense for a real extension and for an
    // IAssocHandler capable of MakeDefault. Extensionless files can be opened,
    // but there is no extension to persist as a default.
    const std::wstring extension = ExtensionOf(state.request.path);
    const bool hasAssociableExtension =
        extension.size() > 1 && extension[0] == L'.';
    const bool handlerCanBeDefault =
        valid &&
        (state.handlers[static_cast<size_t>(index)].handler ||
         !state.handlers[static_cast<size_t>(index)].progId.empty());

    if (state.request.setDefaultOnly) {
        EnableWindow(state.alwaysUse, FALSE);
        SetAlwaysUseChecked(state, true);
    } else {
        const bool enableAssociation =
            handlerCanBeDefault && hasAssociableExtension;
        EnableWindow(state.alwaysUse, enableAssociation);
        if (!enableAssociation) SetAlwaysUseChecked(state, false);
    }
    if (state.isDarkMode && state.alwaysUse)
        InvalidateRect(state.alwaysUse, nullptr, TRUE);
}

static void AddGroup(HWND list, int id, PCWSTR title, bool collapsed) {
    LVGROUP group{};
    group.cbSize = sizeof(group);
    group.mask = LVGF_HEADER | LVGF_GROUPID | LVGF_STATE;
    group.pszHeader = const_cast<PWSTR>(title);
    group.iGroupId = id;
    group.stateMask = LVGS_COLLAPSIBLE | LVGS_COLLAPSED;
    group.state = LVGS_COLLAPSIBLE | (collapsed ? LVGS_COLLAPSED : 0);
    SendMessageW(list, LVM_INSERTGROUP, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(&group));
}

static void SetListGroupCollapsed(HWND list, int groupId, bool collapsed) {
    if (!list) return;
    LVGROUP group{};
    group.cbSize = sizeof(group);
    group.mask = LVGF_STATE;
    group.stateMask = LVGS_COLLAPSED;
    group.state = collapsed ? LVGS_COLLAPSED : 0;
    SendMessageW(list, LVM_SETGROUPINFO, groupId,
                 reinterpret_cast<LPARAM>(&group));
}

static int AddListItem(PickerState& state, size_t index) {
    HandlerEntry& entry = state.handlers[index];
    if (state.images && entry.imageIndex < 0) {
        IconOwner icon(EntryIcon(entry, state.listIconSize));
        if (icon) entry.imageIndex = ImageList_AddIcon(state.images.Get(), icon.Get());
    }
    LVITEMW item{};
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    if (state.listUsesGroups) item.mask |= LVIF_GROUPID;
    item.iItem = static_cast<int>(index);
    item.iImage = entry.imageIndex;
    item.iGroupId = entry.recommended ? GROUP_RECOMMENDED : GROUP_OTHER;
    item.lParam = static_cast<LPARAM>(index);
    item.pszText = entry.displayName.data();
    const int inserted = ListView_InsertItem(state.list, &item);
    // Publisher subtitle, shown as the tile's second line (subitem 1),
    // matching the "Company, Inc." line under each program in the real
    // Windows 7 dialog. Left unset (and so blank) when unknown.
    if (inserted >= 0 && !entry.companyName.empty()) {
        ListView_SetItemText(state.list, inserted, 1,
                             entry.companyName.data());
    }
    return inserted;
}

static int FindListItemForHandler(PickerState& state, size_t handlerIndex) {
    if (!state.list) return -1;
    const int count = ListView_GetItemCount(state.list);
    for (int itemIndex = 0; itemIndex < count; ++itemIndex) {
        LVITEMW item{};
        item.mask = LVIF_PARAM;
        item.iItem = itemIndex;
        if (ListView_GetItem(state.list, &item) &&
            static_cast<size_t>(item.lParam) == handlerIndex) {
            return itemIndex;
        }
    }
    return -1;
}

static void InitializeList(PickerState& state) {
    ApplyPickerTheme(state);
    ListView_SetExtendedListViewStyle(state.list,
        LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    ListView_SetView(state.list, LV_VIEW_TILE);
    // The image list used to be a hardcoded 32x32, which is the Windows 7
    // size only at 96 DPI - on a scaled display the program icons stayed
    // physically 32 pixels while everything around them grew, so they
    // looked shrunken. Scale the icon size with the DPI instead, and let
    // the tile height follow it.
    const UINT listDpi = WindowDpi(state.window);
    const int listIconSize = std::max(32, DpiScale(kListIconSize, listDpi));
    state.listIconSize = listIconSize;
    state.images.Reset(ImageList_Create(listIconSize, listIconSize,
                                        ILC_COLOR32 | ILC_MASK, 8, 8));
    if (state.images) {
        ListView_SetImageList(state.list, state.images.Get(), LVSIL_NORMAL);
        ListView_SetImageList(state.list, state.images.Get(), LVSIL_SMALL);
    }
    bool recommended = false, other = false;
    for (const auto& entry : state.handlers) {
        recommended |= entry.recommended;
        other |= !entry.recommended;
    }
    if (!state.handlers.empty()) {
        state.listUsesGroups = true;
        ListView_EnableGroupView(state.list, TRUE);
        if (recommended) AddGroup(state.list, GROUP_RECOMMENDED, LOC(STR_RECOMMENDED), false);
        if (other) {
            AddGroup(state.list, GROUP_OTHER, LOC(STR_OTHER), recommended);
            state.hasOtherGroup = true;
        }
    }
    LVCOLUMNW column{};
    column.mask = LVCF_WIDTH;
    column.cx = 420;
    ListView_InsertColumn(state.list, 0, &column);
    // Second column, shown as the tile's subtitle line (see cLines below
    // and AddListItem's ListView_SetItemText(..., 1, ...) call).
    LVCOLUMNW subtitleColumn{};
    subtitleColumn.mask = LVCF_WIDTH;
    subtitleColumn.cx = 420;
    ListView_InsertColumn(state.list, 1, &subtitleColumn);
    for (size_t i = 0; i < state.handlers.size(); ++i) AddListItem(state, i);
    if (!state.handlers.empty()) {
        ListView_SetItemState(state.list, 0, LVIS_SELECTED | LVIS_FOCUSED,
                              LVIS_SELECTED | LVIS_FOCUSED);
    }
    {
        // The real Windows 7 dialog lays its tiles out two per row.
        // comctl32's tile view otherwise auto-sizes tiles to the widest
        // label and packs as many as fit, which for this list's fixed
        // width collapses to a single column; pin an explicit tile size
        // instead so two columns always fit, matching the original. This
        // has to run AFTER group view is enabled and the groups/columns/
        // items are inserted above: those calls reset a tile size set
        // any earlier back to comctl32's auto-sizing default, the same
        // way they reset the custom colors re-applied below.
        const UINT dpi = listDpi;
        RECT rcList{};
        GetClientRect(state.list, &rcList);
        const int available = (rcList.right - rcList.left) > 0
                                   ? (rcList.right - rcList.left)
                                   : DpiScale(532, dpi);
        const int gap = DpiScale(4, dpi);
        const int tileWidth = (available - gap) / 2;

        // Tile height is derived from what the row actually has to hold
        // rather than from a fixed 48 logical pixels. A hardcoded height
        // leaves slack that comctl32 distributes as vertical padding,
        // which is what made the rows look too tall; sizing to
        // max(icon, two text lines) plus a small margin gives the tight
        // Windows 7 row instead, and it adapts to the user's font size
        // and DPI on its own.
        int textHeight = 0;
        {
            WindowDcOwner listDc(state.list);
            if (listDc) {
                HFONT listFont = reinterpret_cast<HFONT>(
                    SendMessageW(state.list, WM_GETFONT, 0, 0));
                SelectedObject fontSelection(listDc.Get(), listFont);
                TEXTMETRICW metrics{};
                if (GetTextMetricsW(listDc.Get(), &metrics))
                    textHeight = metrics.tmHeight * 2;
            }
        }
        const int contentHeight = std::max(listIconSize, textHeight);
        const int tileHeight = contentHeight + DpiScale(6, dpi);

        LVTILEVIEWINFO tileInfo{};
        tileInfo.cbSize = sizeof(tileInfo);
        tileInfo.dwMask = LVTVIM_TILESIZE | LVTVIM_COLUMNS;
        tileInfo.dwFlags = LVTVIF_FIXEDSIZE;
        tileInfo.sizeTile.cx = tileWidth;
        tileInfo.sizeTile.cy = tileHeight;
        // One extra line for the publisher subtitle (subitem 1), under
        // the program name.
        tileInfo.cLines = 1;
        SendMessageW(state.list, LVM_SETTILEVIEWINFO, 0,
                    reinterpret_cast<LPARAM>(&tileInfo));
    }
    // Switching to tile view and inserting groups/items can reset the
    // custom colors to theme defaults, which left the first (selected)
    // tile with a white background in dark mode. Re-apply the palette
    // after the view is fully built and repaint. The light values are
    // identical to the ones set in ApplyPickerTheme, so light rendering
    // is unchanged.
    if (state.isDarkMode) {
        ListView_SetBkColor(state.list, RGB(32, 32, 32));
        ListView_SetTextBkColor(state.list, RGB(32, 32, 32));
        ListView_SetTextColor(state.list, RGB(240, 240, 240));
    } else {
        ListView_SetBkColor(state.list, RGB(255, 255, 255));
        ListView_SetTextBkColor(state.list, RGB(255, 255, 255));
        ListView_SetTextColor(state.list, RGB(0, 0, 0));
    }
    // LVM_SETTILEVIEWINFO alone doesn't always re-flow items already in
    // the list; force a relayout so the new tile size takes effect
    // immediately instead of only after the next resize/scroll.
    ListView_Arrange(state.list, LVA_DEFAULT);
    RedrawWindow(state.list, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    UpdateSelectionUi(state);
}

class BrowseDialogEvents final : public IFileDialogEvents {
   public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (!IsEqualIID(iid, IID_IUnknown) &&
            !IsEqualIID(iid, IID_IFileDialogEvents)) {
            return E_NOINTERFACE;
        }
        *object = static_cast<IFileDialogEvents*>(this);
        AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return references_.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG value =
            references_.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (!value) delete this;
        return value;
    }

    HRESULT STDMETHODCALLTYPE OnFileOk(IFileDialog* dialog) override {
        CaptureWindow(dialog);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnFolderChanging(IFileDialog* dialog,
                                                IShellItem*) override {
        CaptureWindow(dialog);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnFolderChange(IFileDialog* dialog) override {
        CaptureWindow(dialog);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnSelectionChange(IFileDialog* dialog) override {
        CaptureWindow(dialog);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnShareViolation(
        IFileDialog* dialog, IShellItem*,
        FDE_SHAREVIOLATION_RESPONSE* response) override {
        CaptureWindow(dialog);
        if (response) *response = FDESVR_DEFAULT;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnTypeChange(IFileDialog* dialog) override {
        CaptureWindow(dialog);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnOverwrite(
        IFileDialog* dialog, IShellItem*,
        FDE_OVERWRITE_RESPONSE* response) override {
        CaptureWindow(dialog);
        if (response) *response = FDEOR_DEFAULT;
        return S_OK;
    }

   private:
    void CaptureWindow(IFileDialog* dialog) {
        if (!dialog) return;
        ComPtr<IOleWindow> oleWindow;
        if (FAILED(dialog->QueryInterface(
                IID_IOleWindow,
                reinterpret_cast<void**>(oleWindow.Put()))) ||
            !oleWindow) {
            return;
        }
        HWND window = nullptr;
        if (SUCCEEDED(oleWindow->GetWindow(&window)) && window) {
            g_activeBrowseHwnd.store(window, std::memory_order_release);
            if (g_shuttingDown.load(std::memory_order_acquire))
                PostMessageW(window, WM_CLOSE, 0, 0);
        }
    }

    std::atomic<ULONG> references_{1};
};

static void Browse(PickerState& state) {
    ComPtr<IFileOpenDialog> dialog;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(dialog.Put()))) || !dialog)
        return;
    DWORD options = 0;
    if (SUCCEEDED(dialog->GetOptions(&options)))
        dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST);
    const COMDLG_FILTERSPEC filters[] = {
        {LOC(STR_PROGRAMS), L"*.exe;*.com;*.bat;*.cmd"},
        {LOC(STR_ALL_FILES), L"*.*"},
    };
    dialog->SetFileTypes(ARRAYSIZE(filters), filters);
    dialog->SetTitle(LOC(STR_BROWSE_TITLE));

    BrowseDialogEvents* rawEvents = nullptr;
    try {
        rawEvents = new BrowseDialogEvents();
    } catch (...) {
        return;
    }
    ComPtr<IFileDialogEvents> events(rawEvents);
    DWORD eventsCookie = 0;
    const bool advised = SUCCEEDED(dialog->Advise(events.Get(), &eventsCookie));
    if (g_shuttingDown.load(std::memory_order_acquire)) {
        if (advised) dialog->Unadvise(eventsCookie);
        return;
    }
    const HRESULT showHr = dialog->Show(state.window);
    if (advised) dialog->Unadvise(eventsCookie);
    g_activeBrowseHwnd.store(nullptr, std::memory_order_release);
    if (FAILED(showHr) ||
        g_shuttingDown.load(std::memory_order_acquire)) {
        return;
    }
    ComPtr<IShellItem> item;
    if (FAILED(dialog->GetResult(item.Put())) || !item) return;
    PWSTR raw = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &raw)) || !raw) return;
    const std::wstring executable = TakeTaskString(raw);
    if (executable.empty()) return;
    if (IsOpenWithExecutable(executable)) {
        SetWindowTextW(GetDlgItem(state.window, IDC_SOW_INSTRUCTION),
                       LOC(STR_OPEN_FAILED));
        MessageBeep(MB_ICONWARNING);
        return;
    }
    for (size_t i = 0; i < state.handlers.size(); ++i) {
        if (!_wcsicmp(state.handlers[i].internalName.c_str(), executable.c_str())) {
            if (!state.handlers[i].recommended)
                SetListGroupCollapsed(state.list, GROUP_OTHER, false);
            const int itemIndex = FindListItemForHandler(state, i);
            if (itemIndex >= 0) {
                ListView_SetItemState(state.list, itemIndex,
                                      LVIS_SELECTED | LVIS_FOCUSED,
                                      LVIS_SELECTED | LVIS_FOCUSED);
                ListView_EnsureVisible(state.list, itemIndex, FALSE);
            }
            UpdateSelectionUi(state);
            return;
        }
    }
    HandlerEntry entry;
    entry.browsed = true;
    entry.internalName = executable;
    entry.progId = ApplicationProgIdForExecutable(executable);
entry.companyName = ExecutableCompanyName(executable, entry.progId);
    PCWSTR name = PathFindFileNameW(executable.c_str());
    entry.displayName = name && *name ? name : executable;
    if (entry.displayName.size() > 4 &&
        !_wcsicmp(entry.displayName.c_str() + entry.displayName.size() - 4, L".exe"))
        entry.displayName.resize(entry.displayName.size() - 4);
    state.handlers.push_back(std::move(entry));
    if (!state.listUsesGroups) {
        state.listUsesGroups = true;
        ListView_EnableGroupView(state.list, TRUE);
    }
    if (!state.hasOtherGroup) {
        AddGroup(state.list, GROUP_OTHER, LOC(STR_OTHER), false);
        state.hasOtherGroup = true;
    }
    // A pre-existing Other Programs group is initially collapsed when there
    // are recommended handlers. Expand it before selecting the browsed app so
    // the newly added item is immediately visible.
    if (state.listUsesGroups)
        SetListGroupCollapsed(state.list, GROUP_OTHER, false);

    const size_t index = state.handlers.size() - 1;
    const int insertedItem = AddListItem(state, index);
    if (insertedItem >= 0) {
        ListView_SetItemState(state.list, insertedItem,
                              LVIS_SELECTED | LVIS_FOCUSED,
                              LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(state.list, insertedItem, FALSE);
        SetFocus(state.list);
        RedrawWindow(state.list, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }
    UpdateSelectionUi(state);
}

static void WebSearch(const std::wstring& path) {
    std::wstring query;
    for (wchar_t c : ExtensionOf(path)) {
        if ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') ||
            (c >= L'0' && c <= L'9')) query.push_back(c);
    }
    if (query.empty()) query = L"unknown";
    const std::wstring url = L"https://www.bing.com/search?q=program+to+open+" + query + L"+file";
    ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static void SetGroupTitle(HWND list, int groupId, PCWSTR title) {
    if (!list) return;
    LVGROUP group{};
    group.cbSize = sizeof(group);
    group.mask = LVGF_HEADER;
    group.pszHeader = const_cast<PWSTR>(title);
    SendMessageW(list, LVM_SETGROUPINFO, groupId,
                 reinterpret_cast<LPARAM>(&group));
}

static void ApplyLocalizedText(PickerState& state) {
    if (!state.window || !IsWindow(state.window)) return;
    SetWindowTextW(state.window, LOC(STR_TITLE));
    SetWindowTextW(GetDlgItem(state.window, IDC_SOW_INSTRUCTION),
                   state.handlers.empty() ? LOC(STR_NO_HANDLERS)
                                          : LOC(STR_INSTRUCTION));
    SetWindowTextW(GetDlgItem(state.window, IDC_SOW_FILE_LABEL),
                   LOC(STR_FILE_LABEL));
    // Absent when the description box is hidden by its setting.
    HWND descriptionLabel = GetDlgItem(state.window,
                                       IDC_SOW_DESCRIPTION_LABEL);
    if (descriptionLabel)
        SetWindowTextW(descriptionLabel, LOC(STR_DESCRIPTION));
    // Both themes keep the label on the checkbox itself: in dark mode
    // the owner-draw handler reads it back with GetWindowText and paints
    // it with the light text color.
    SetWindowTextW(GetDlgItem(state.window, IDC_SOW_ALWAYS_USE),
                   LOC(STR_ALWAYS_USE));
    SetWindowTextW(GetDlgItem(state.window, IDC_SOW_BROWSE), LOC(STR_BROWSE));
    HWND web = GetDlgItem(state.window, IDC_SOW_WEB);
    SetWindowTextW(web, LOC(STR_WEB_LINK));
    ShowWindow(web, g_showWebLink.load(std::memory_order_acquire)
                        ? SW_SHOW : SW_HIDE);
    SetWindowTextW(GetDlgItem(state.window, IDOK), LOC(STR_OK));
    SetWindowTextW(GetDlgItem(state.window, IDCANCEL), LOC(STR_CANCEL));
    SetGroupTitle(state.list, GROUP_RECOMMENDED, LOC(STR_RECOMMENDED));
    SetGroupTitle(state.list, GROUP_OTHER, LOC(STR_OTHER));
}

// Reading the "Always use" state: light mode asks the real
// BS_AUTOCHECKBOX control (unchanged behavior), dark mode uses the state
// mirror, because an owner-drawn button has no reliable check state.
static bool AlwaysUseChecked(PickerState& state) {
    if (!state.alwaysUse || !IsWindow(state.alwaysUse))
        return state.alwaysUseChecked;
    if (state.isDarkMode) return state.alwaysUseChecked;
    return Button_GetCheck(state.alwaysUse) == BST_CHECKED;
}

static void SetAlwaysUseChecked(PickerState& state, bool checked) {
    state.alwaysUseChecked = checked;
    if (!state.alwaysUse || !IsWindow(state.alwaysUse)) return;
    Button_SetCheck(state.alwaysUse, checked ? BST_CHECKED : BST_UNCHECKED);
    if (state.isDarkMode) InvalidateRect(state.alwaysUse, nullptr, TRUE);
}

// Creates the "Always use" checkbox with the type style the current
// theme needs, recreating it when the theme changed at runtime: the
// button type (BS_AUTOCHECKBOX vs BS_OWNERDRAW) cannot be swapped
// reliably through SetWindowLongPtr once the control exists, which is
// why the dark glyph used to stay light. Light mode always ends up with
// exactly the same BS_AUTOCHECKBOX control as before.
static void EnsureAlwaysUseCheckbox(PickerState& state) {
    if (!state.window || !IsWindow(state.window)) return;
    const bool dark = state.isDarkMode;
    const LONG_PTR wantedType = dark ? BS_OWNERDRAW : BS_AUTOCHECKBOX;

    bool enabled = true;
    if (state.alwaysUse && IsWindow(state.alwaysUse)) {
        const LONG_PTR style = GetWindowLongPtrW(state.alwaysUse, GWL_STYLE);
        enabled = IsWindowEnabled(state.alwaysUse) != FALSE;
        if (!dark)
            state.alwaysUseChecked =
                Button_GetCheck(state.alwaysUse) == BST_CHECKED;
        if ((style & BS_TYPEMASK) == wantedType) {
            SetWindowTextW(state.alwaysUse, LOC(STR_ALWAYS_USE));
            SetWindowTheme(state.alwaysUse, nullptr, nullptr);
            if (dark) InvalidateRect(state.alwaysUse, nullptr, TRUE);
            return;
        }
        DestroyWindow(state.alwaysUse);
        state.alwaysUse = nullptr;
    }

    const UINT dpi = WindowDpi(state.window);
    HFONT font = state.font ? state.font.Get()
                            : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    // Same y offset BuildPickerControls used, so a checkbox recreated on
    // a theme switch lands back where the rest of the layout expects it.
    state.alwaysUse = Child(state.window, 0, WC_BUTTONW, LOC(STR_ALWAYS_USE),
                            WS_TABSTOP | static_cast<DWORD>(wantedType),
                            14, 354 - state.descriptionOffsetY, 405, 22,
                            IDC_SOW_ALWAYS_USE, dpi, font);
    if (!state.alwaysUse) return;
    SetWindowTheme(state.alwaysUse, nullptr, nullptr);
    // Keep the original tab order: a freshly created child lands at the
    // end of the sibling chain, so move it right behind the control it
    // was created after - the description edit, or the list when the
    // description box is hidden.
    HWND tabPredecessor =
        (state.description && IsWindow(state.description))
            ? state.description
            : ((state.list && IsWindow(state.list)) ? state.list : nullptr);
    if (tabPredecessor) {
        SetWindowPos(state.alwaysUse, tabPredecessor, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    Button_SetCheck(state.alwaysUse,
                    state.alwaysUseChecked ? BST_CHECKED : BST_UNCHECKED);
    EnableWindow(state.alwaysUse, enabled);
    InvalidateRect(state.alwaysUse, nullptr, TRUE);
}

// Windows 7 row highlight palette. Selection and hover are the blue
// gradient Explorer uses in Windows 7 (Aero "listview item" states): a
// rounded rectangle with a vertical gradient fill and a saturated blue
// border. The light values are the stock Windows 7 ones; the dark values
// are the same hues pulled down to sit on the RGB(32,32,32) background
// while still reading as blue.
struct Win7RowColors {
    COLORREF fillTop;
    COLORREF fillBottom;
    COLORREF border;
};

// Selection: the Windows 7 blue gradient (light values are the stock
// Aero ones, dark values the same hues darkened to sit on RGB(32,32,32)).
static constexpr Win7RowColors kWin7RowSelectedLight = {
    RGB(220, 235, 252), RGB(193, 219, 252), RGB(125, 162, 206)};
static constexpr Win7RowColors kWin7RowSelectedDark = {
    RGB(38, 66, 104), RGB(28, 52, 86), RGB(74, 132, 198)};

// Hover: the win7-network-flyout-recreation palette, so a row lights up
// exactly the way that mod's rows do. Light mode uses the Aero hot-item
// wash, which is the same shape of effect - a pale fill with a slightly
// stronger border - just in the light key.
static constexpr Win7RowColors kWin7RowHoverLight = {
    RGB(252, 253, 254), RGB(241, 248, 253), RGB(184, 214, 251)};
static constexpr Win7RowColors kWin7RowHoverDark = {
    RGB(35, 35, 45), RGB(35, 35, 45), RGB(50, 70, 100)};

static const Win7RowColors& Win7RowPalette(bool dark, bool selected) {
    if (dark) return selected ? kWin7RowSelectedDark : kWin7RowHoverDark;
    return selected ? kWin7RowSelectedLight : kWin7RowHoverLight;
}

// Native UxTheme draw for the row highlight.
//
// This is the primary path for the hover state: it asks the system for
// its own list item states, so a hovered row lights up exactly like a
// row in any other Explorer list on the running OS, in whichever theme
// is active. Returns false when the themed draw could not be done (no
// visual style loaded, or the list part not defined by the theme), so
// the caller can fall back to the hand-drawn palette.
static bool PaintThemedRowBackground(HWND list, HDC hdc, const RECT& rc,
                                     bool selected, bool focusedList) {
    if (!list || !hdc) return false;
    if (!IsThemeActive()) return false;
    // Plain "ListView" first, on purpose: ApplyPickerTheme has already
    // put the list on "DarkMode_Explorer" or "Explorer" through
    // SetWindowTheme, and the unqualified class honors that assignment,
    // so dark mode gets the dark item states. Naming a class explicitly
    // would override it and force the light Explorer artwork.
    ThemeOwner theme(OpenThemeData(list, L"ListView"));
    if (!theme) theme.Reset(OpenThemeData(list, L"Explorer::ListView"));
    if (!theme) return false;

    const int stateId = selected
                            ? (focusedList ? LISS_SELECTED
                                           : LISS_SELECTEDNOTFOCUS)
                            : LISS_HOT;
    if (!IsThemePartDefined(theme.Get(), LVP_LISTITEM, stateId)) return false;
    RECT rcTheme = rc;
    rcTheme.right += 1;
    rcTheme.bottom += 1;
    return SUCCEEDED(DrawThemeBackground(theme.Get(), hdc, LVP_LISTITEM,
                                         stateId, &rcTheme, nullptr));
}

// Paints one list row highlight the way Windows 7 paints its list rows:
// a rounded rectangle with a 3 pixel corner radius, a vertical gradient
// fill and a matching border.
//
// The gradient is drawn as clipped scanlines rather than with GradientFill
// so the mod keeps linking against nothing but uxtheme and dwmapi.
static bool PaintWin7RowBackground(HDC hdc, const RECT& rc, bool dark,
                                   bool selected) {
    if (!hdc) return false;
    // Callers pass an inclusive rectangle (right/bottom are the last
    // painted pixel), so the region and the fill both run one past them.
    const int width = rc.right - rc.left + 1;
    const int height = rc.bottom - rc.top + 1;
    if (width <= 0 || height <= 0) return false;

    const Win7RowColors& colors = Win7RowPalette(dark, selected);
    RegionOwner region(CreateRoundRectRgn(rc.left, rc.top, rc.right + 1,
                                          rc.bottom + 1, 4, 4));
    if (!region) return false;

    const int savedDc = SaveDC(hdc);
    if (savedDc) SelectClipRgn(hdc, region.Get());

    for (int y = 0; y < height; ++y) {
        // Integer interpolation between the two stops, rounded.
        const int numerator = height > 1 ? y : 0;
        const int denominator = height > 1 ? height - 1 : 1;
        auto blend = [&](int from, int to) {
            return from + (to - from) * numerator / denominator;
        };
        const COLORREF line =
            RGB(blend(GetRValue(colors.fillTop), GetRValue(colors.fillBottom)),
                blend(GetGValue(colors.fillTop), GetGValue(colors.fillBottom)),
                blend(GetBValue(colors.fillTop), GetBValue(colors.fillBottom)));
        BrushOwner brush(CreateSolidBrush(line));
        if (!brush) break;
        RECT rcLine{rc.left, rc.top + y, rc.right + 1, rc.top + y + 1};
        FillRect(hdc, &rcLine, brush.Get());
    }

    if (savedDc) RestoreDC(hdc, savedDc);

    BrushOwner borderBrush(CreateSolidBrush(colors.border));
    if (borderBrush) FrameRgn(hdc, region.Get(), borderBrush.Get(), 1, 1);
    return true;
}

// How a row highlight ended up being painted, so the caller knows which
// text color goes on top of it.
enum class RowHighlight { None, Custom, Themed, System };

// Paints the hover/selection highlight for one row.
//
// The two states deliberately use opposite strategies:
//
// - Hover: the native UxTheme item state comes first, so a hovered row
//   looks exactly like a hovered row in any other Explorer list on the
//   running OS. The hand-drawn flyout palette is only the fallback, for
//   when the theme cannot provide the part (classic theme, or a theme
//   without LVP_LISTITEM).
// - Selection: the hand-drawn Windows 7 blue gradient comes first,
//   because that is the look this dialog is recreating and the modern
//   system draw would be the flat Windows 10/11 grey rectangle.
//
// A flat COLOR_HIGHLIGHT fill closes the chain for selected rows when
// neither draw is available, which is also the only case where the row
// text has to switch to COLOR_HIGHLIGHTTEXT.
static RowHighlight PaintRowHighlight(HWND list, HDC hdc, const RECT& rc,
                                      bool dark, bool selected,
                                      bool focusedList) {
    if (!selected) {
        if (PaintThemedRowBackground(list, hdc, rc, false, focusedList))
            return RowHighlight::Themed;
        if (PaintWin7RowBackground(hdc, rc, dark, false))
            return RowHighlight::Custom;
        return RowHighlight::None;
    }

    if (PaintWin7RowBackground(hdc, rc, dark, true))
        return RowHighlight::Custom;
    if (PaintThemedRowBackground(list, hdc, rc, true, focusedList))
        return RowHighlight::Themed;
    RECT rcFill = rc;
    rcFill.right += 1;
    rcFill.bottom += 1;
    FillRect(hdc, &rcFill, GetSysColorBrush(COLOR_HIGHLIGHT));
    return RowHighlight::System;
}

// Draws a complete program row the Windows 7 way: rounded blue
// hover/selection frame, then the application icon and its two text
// lines. Used in both themes through CDRF_SKIPDEFAULT - the light theme
// needs it too, because the stock comctl32 highlight on a modern OS is
// the flat Windows 10/11 grey rectangle rather than the Windows 7 blue
// gradient this dialog is recreating.
static void PaintWin7ListRow(PickerState& state,
                             const NMLVCUSTOMDRAW& listDraw) {
    HDC hdc = listDraw.nmcd.hdc;
    HWND list = state.list;
    const int row = static_cast<int>(listDraw.nmcd.dwItemSpec);
    if (!hdc || !list || row < 0) return;

    RECT rcBounds{};
    if (!ListView_GetItemRect(list, row, &rcBounds, LVIR_BOUNDS)) return;
    RECT rcIcon{};
    const bool hasIconRect =
        ListView_GetItemRect(list, row, &rcIcon, LVIR_ICON) != FALSE;
    RECT rcLabel{};
    const bool hasLabelRect =
        ListView_GetItemRect(list, row, &rcLabel, LVIR_LABEL) != FALSE;

    const UINT itemState =
        ListView_GetItemState(list, row, LVIS_SELECTED | LVIS_FOCUSED);
    const bool selected = (itemState & LVIS_SELECTED) != 0;
    const bool focused = (itemState & LVIS_FOCUSED) != 0;
    const bool hovered = row == state.hoverRow;

    const bool dark = state.isDarkMode;

    // Plain rows are erased with the list background first, so a frame
    // left over from a previous hover never survives a partial repaint.
    if (dark) {
        if (state.darkBgBrush) FillRect(hdc, &rcBounds, state.darkBgBrush.Get());
    } else {
        COLORREF background = ListView_GetBkColor(list);
        if (background == CLR_NONE || background == CLR_DEFAULT)
            background = GetSysColor(COLOR_WINDOW);
        BrushOwner light(CreateSolidBrush(background));
        if (light) FillRect(hdc, &rcBounds, light.Get());
    }
    const bool listFocused = GetFocus() == list;
    RowHighlight highlight = RowHighlight::None;
    if (selected || hovered) {
        RECT rcFrame = rcBounds;
        // One pixel of breathing room, so adjacent rounded frames never
        // touch each other, exactly like the real dialog's rows.
        rcFrame.right -= 1;
        rcFrame.bottom -= 1;
        highlight = PaintRowHighlight(list, hdc, rcFrame, dark, selected,
                                      listFocused);
    }

    LVITEMW item{};
    item.mask = LVIF_IMAGE;
    item.iItem = row;
    if (hasIconRect && state.images && ListView_GetItem(list, &item) &&
        item.iImage >= 0) {
        ImageList_Draw(state.images.Get(), item.iImage, hdc, rcIcon.left,
                       rcIcon.top, ILD_TRANSPARENT);
    }

    if (hasLabelRect) {
        wchar_t label[512] = {};
        ListView_GetItemText(list, row, 0, label, ARRAYSIZE(label));
        wchar_t subtitle[512] = {};
        ListView_GetItemText(list, row, 1, subtitle, ARRAYSIZE(subtitle));
        const bool hasSubtitle = subtitle[0] != L'\0';

        SetBkMode(hdc, TRANSPARENT);
        SelectedObject fontSelection(
            hdc, reinterpret_cast<HFONT>(SendMessageW(list, WM_GETFONT, 0, 0)));

        RECT rcText = rcLabel;
        rcText.left += 2;

        // The two lines are stacked as one block of exactly the height
        // the font needs, then that block is centered in the label rect.
        //
        // Splitting the label rect in half (the old approach) padded the
        // program name and its publisher apart by whatever slack the tile
        // had, which is what made the rows look airy and too tall. Using
        // the real line height keeps them on consecutive baselines the
        // way Windows 7 does, and it means an unsigned program - one with
        // no publisher line - ends up genuinely centered against its
        // icon instead of sitting in the top half of the tile.
        TEXTMETRICW metrics{};
        const int lineHeight = GetTextMetricsW(hdc, &metrics)
                                   ? metrics.tmHeight
                                   : (rcText.bottom - rcText.top) / 2;
        const int labelHeight = rcText.bottom - rcText.top;
        const int blockHeight = hasSubtitle ? lineHeight * 2 : lineHeight;
        int lineTop = rcText.top + (labelHeight - blockHeight) / 2;
        if (lineTop < rcText.top) lineTop = rcText.top;

        // Only the flat system highlight forces its own text color; the
        // Windows 7 and themed fills are pale enough to keep the normal
        // one, exactly as the real dialog does.
        const bool systemFill = highlight == RowHighlight::System;
        if (label[0]) {
            SetTextColor(hdc, systemFill
                                  ? GetSysColor(COLOR_HIGHLIGHTTEXT)
                                  : (dark ? RGB(240, 240, 240)
                                          : RGB(0, 0, 0)));
            RECT rcTitle{rcText.left, lineTop, rcText.right,
                         lineTop + lineHeight};
            DrawTextW(hdc, label, -1, &rcTitle,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS |
                          DT_NOPREFIX);
        }
        if (hasSubtitle) {
            RECT rcSubtitle{rcText.left, lineTop + lineHeight, rcText.right,
                            lineTop + lineHeight * 2};
            SetTextColor(hdc, systemFill
                                  ? GetSysColor(COLOR_HIGHLIGHTTEXT)
                                  : (dark ? RGB(150, 150, 165)
                                          : RGB(112, 112, 112)));
            DrawTextW(hdc, subtitle, -1, &rcSubtitle,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS |
                          DT_NOPREFIX);
        }
    }

    // Keyboard focus marker, same dotted rectangle the flyout uses for
    // its focused row.
    if (focused && !selected && listFocused) {
        RECT rcFocus = rcBounds;
        rcFocus.right -= 1;
        rcFocus.bottom -= 1;
        SetTextColor(hdc, dark ? RGB(150, 150, 165) : RGB(0, 0, 0));
        DrawFocusRect(hdc, &rcFocus);
    }
}

// The DarkMode_Explorer visual style paints group header labels itself
// via UxTheme, and it ignores NMLVCUSTOMDRAW::clrText entirely for that
// draw, so there is no way to simply recolor the "Other Programs"
// header. Instead the native label is covered with the list background
// and an identical string is redrawn on top in the accent color, which
// reads as a recolor as long as the clone lands exactly where the
// original was.
//
// Getting that alignment right means measuring with the same font the
// theme used: group headers are drawn with the theme's own header font
// (TMT_FONT on LVP_GROUPHEADER), which is usually larger than the
// list's WM_GETFONT font. Measuring with the wrong one leaves an erase
// rectangle too small for the native glyphs, and their edges survive
// around the clone.
static void RepaintOtherProgramsHeaderText(PickerState& state,
                                           const NMLVCUSTOMDRAW& listDraw) {
    HWND list = state.list;
    HDC hdc = listDraw.nmcd.hdc;
    if (!list || !hdc) return;

    // LVGGR_LABEL is the label's own rectangle, so the clone inherits
    // the native indent and vertical placement instead of recomputing
    // them. It is not supported on every comctl32 build; the wider
    // LVGGR_HEADER band is the fallback.
    bool haveLabelRect = true;
    RECT rcLabel{};
    rcLabel.top = LVGGR_LABEL;
    if (!SendMessageW(list, LVM_GETGROUPRECT, static_cast<WPARAM>(GROUP_OTHER),
                      reinterpret_cast<LPARAM>(&rcLabel)) ||
        IsRectEmpty(&rcLabel)) {
        haveLabelRect = false;
        rcLabel = {};
        rcLabel.top = LVGGR_HEADER;
        if (!SendMessageW(list, LVM_GETGROUPRECT,
                          static_cast<WPARAM>(GROUP_OTHER),
                          reinterpret_cast<LPARAM>(&rcLabel)) ||
            IsRectEmpty(&rcLabel)) {
            return;
        }
    }

    wchar_t header[256] = {};
    LVGROUP group{};
    group.cbSize = sizeof(group);
    group.mask = LVGF_HEADER | LVGF_STATE;
    group.pszHeader = header;
    group.cchHeader = ARRAYSIZE(header);
    group.stateMask = LVGS_COLLAPSED;
    if (SendMessageW(list, LVM_GETGROUPINFO, static_cast<WPARAM>(GROUP_OTHER),
                     reinterpret_cast<LPARAM>(&group)) == -1 ||
        !header[0]) {
        return;
    }
    const bool collapsed = (group.state & LVGS_COLLAPSED) != 0;

    // The font the theme used for the native label, falling back to the
    // list font when the theme cannot supply one.
    ThemeOwner theme(OpenThemeData(list, L"ListView"));
    FontOwner themeFont;
    if (theme) {
        LOGFONTW logFont{};
        if (SUCCEEDED(GetThemeFont(theme.Get(), hdc, LVP_GROUPHEADER,
                                   collapsed ? LVGH_CLOSE : LVGH_OPEN,
                                   TMT_FONT, &logFont))) {
            themeFont.Reset(CreateFontIndirectW(&logFont));
        }
        theme.Reset();
    }
    HFONT font = themeFont
                     ? themeFont.Get()
                     : reinterpret_cast<HFONT>(
                           SendMessageW(list, WM_GETFONT, 0, 0));
    SelectedObject fontSelection(hdc, font);

    const int length = static_cast<int>(wcslen(header));
    SIZE extent{};
    GetTextExtentPoint32W(hdc, header, length, &extent);

    RECT rcTextArea = rcLabel;
    if (!haveLabelRect) {
        // Header band fallback: reproduce the native label box inside it
        // (same left indent, vertically centered).
        rcTextArea.left += 2;
        const int centerY = (rcLabel.top + rcLabel.bottom) / 2;
        rcTextArea.top = centerY - extent.cy / 2;
        rcTextArea.bottom = rcTextArea.top + extent.cy;
    }
    // The text area keeps the native rectangle so DT_VCENTER puts the
    // clone on the same baseline the native label used, but the erase is
    // sized from the glyph run itself, plus 2 pixels for antialiased
    // edges. Erasing the whole label rectangle instead would also wipe
    // whatever sits beside the text in the header band - the divider
    // line the Explorer styles draw to the right of the label, and the
    // collapse chevron.
    const int centerY = (rcTextArea.top + rcTextArea.bottom) / 2;
    RECT rcErase{rcTextArea.left, centerY - extent.cy / 2,
                 rcTextArea.left + extent.cx, centerY + extent.cy / 2};
    InflateRect(&rcErase, 2, 2);
    if (rcErase.right > rcTextArea.right && !haveLabelRect)
        rcErase.right = rcTextArea.right;
    // Never paint outside the group's own header band.
    RECT rcBand{};
    rcBand.top = LVGGR_HEADER;
    if (SendMessageW(list, LVM_GETGROUPRECT, static_cast<WPARAM>(GROUP_OTHER),
                     reinterpret_cast<LPARAM>(&rcBand)) &&
        !IsRectEmpty(&rcBand)) {
        if (rcErase.top < rcBand.top) rcErase.top = rcBand.top;
        if (rcErase.bottom > rcBand.bottom) rcErase.bottom = rcBand.bottom;
        if (rcErase.right > rcBand.right) rcErase.right = rcBand.right;
    }

    // Erase with the list's own background rather than the dialog brush:
    // it is the color the native label was actually drawn on, so the
    // patch stays invisible even if the two ever diverge.
    COLORREF background = ListView_GetBkColor(list);
    if (background == CLR_NONE || background == CLR_DEFAULT)
        background = GetSysColor(COLOR_WINDOW);
    BrushOwner eraseBrush;
    eraseBrush.Reset(CreateSolidBrush(background));
    if (eraseBrush) {
        FillRect(hdc, &rcErase, eraseBrush.Get());
    } else if (state.darkBgBrush) {
        FillRect(hdc, &rcErase, state.darkBgBrush.Get());
    }

    const int savedBkMode = SetBkMode(hdc, TRANSPARENT);
    const COLORREF savedColor =
        SetTextColor(hdc, RGB(0, 146, 214));  // Azzurro Napoli.
    // DT_NOCLIP: rcTextArea can be reported tight around the glyphs, and
    // clipping to it would shave the antialiased edges the erase pass
    // just cleared.
    RECT rcText = rcTextArea;
    DrawTextW(hdc, header, length, &rcText,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX |
                  DT_NOCLIP);

    SetTextColor(hdc, savedColor);
    SetBkMode(hdc, savedBkMode);
}

// Dark mode only: paints a 1 pixel gray frame around the controls that
// lost their light 3D client edge, so the description bar keeps a
// visible border on the dark background.
static void PaintDarkControlFrames(PickerState& state, HDC hdc) {
    if (!state.isDarkMode || !state.window || !hdc) return;
    HWND targets[] = {state.description};
    PenOwner pen(CreatePen(PS_SOLID, 1, RGB(110, 110, 120)));
    if (!pen) return;
    SelectedObject penSelection(hdc, pen.Get());
    SelectedObject brushSelection(hdc, GetStockObject(NULL_BRUSH));
    for (HWND target : targets) {
        if (!target || !IsWindow(target) || !IsWindowVisible(target)) continue;
        RECT rc{};
        if (!GetWindowRect(target, &rc)) continue;
        MapWindowPoints(nullptr, state.window,
                        reinterpret_cast<LPPOINT>(&rc), 2);
        InflateRect(&rc, 1, 1);
        Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
    }
}

// Recomputes which list row is hovered and repaints the rows that
// changed state. Both themes need it now that the light theme also
// paints its own Windows 7 blue hover frame.
static void UpdateListHover(PickerState& state, POINT clientPoint) {
    if (!state.list || !IsWindow(state.list)) return;
    LVHITTESTINFO hit{};
    hit.pt = clientPoint;
    const int row = ListView_HitTest(state.list, &hit);
    if (row == state.hoverRow) return;
    const int previous = state.hoverRow;
    state.hoverRow = row;
    RECT rc{};
    if (previous >= 0 &&
        ListView_GetItemRect(state.list, previous, &rc, LVIR_BOUNDS)) {
        InvalidateRect(state.list, &rc, FALSE);
    }
    if (row >= 0 && ListView_GetItemRect(state.list, row, &rc, LVIR_BOUNDS))
        InvalidateRect(state.list, &rc, FALSE);
    if (row >= 0) {
        TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, state.list, 0};
        TrackMouseEvent(&tme);
    }
}

static void ClearListHover(PickerState& state) {
    if (state.hoverRow < 0) return;
    const int previous = state.hoverRow;
    state.hoverRow = -1;
    if (!state.list || !IsWindow(state.list)) return;
    RECT rc{};
    if (ListView_GetItemRect(state.list, previous, &rc, LVIR_BOUNDS))
        InvalidateRect(state.list, &rc, FALSE);
}

// Subclass of the program list: the hover frame needs mouse messages
// that the list would otherwise consume without notifying the dialog.
// Active in both themes, since both now paint their own hover frame.
static LRESULT CALLBACK PickerListSubclassProc(HWND window, UINT message,
                                               WPARAM wParam, LPARAM lParam,
                                               UINT_PTR idSubclass,
                                               DWORD_PTR referenceData) {
    auto state = reinterpret_cast<PickerState*>(referenceData);
    switch (message) {
        case WM_MOUSEMOVE:
            if (state) {
                POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                UpdateListHover(*state, pt);
            }
            break;
        case WM_MOUSELEAVE:
        case WM_MOUSEWHEEL:
            if (state) ClearListHover(*state);
            break;
        case WM_NCDESTROY:
            RemoveWindowSubclass(window, PickerListSubclassProc, idSubclass);
            break;
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

static void UpdateButtonHover(PickerState& state, POINT screenPoint) {
    int target = 0;
    // The checkbox is tracked too: its owner-drawn glyph has a hover
    // state and needs the same enter/leave invalidation as the buttons.
    static const int kHoverTracked[] = {IDOK, IDCANCEL, IDC_SOW_BROWSE,
                                        IDC_SOW_ALWAYS_USE};
    for (const int id : kHoverTracked) {
        HWND button = GetDlgItem(state.window, id);
        if (!button) continue;
        RECT rect{};
        if (GetWindowRect(button, &rect) && PtInRect(&rect, screenPoint)) {
            target = id;
            break;
        }
    }
    if (target == state.hoverButton) return;
    HWND previous =
        state.hoverButton ? GetDlgItem(state.window, state.hoverButton) : nullptr;
    state.hoverButton = target;
    if (previous) InvalidateRect(previous, nullptr, FALSE);
    if (target) InvalidateRect(GetDlgItem(state.window, target), nullptr, FALSE);
    TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, state.window, 0};
    TrackMouseEvent(&tme);
}

static void BuildPickerControls(PickerState& state) {
    const UINT dpi = WindowDpi(state.window);
    NONCLIENTMETRICSW metrics{sizeof(metrics)};
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0))
        state.font.Reset(CreateFontIndirectW(&metrics.lfMessageFont));
    HFONT font = state.font ? state.font.Get() : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));

    // The theme must be known before creating the controls: dark mode
    // uses owner-draw buttons and an emptied checkbox with a companion
    // label (network flyout approach); light mode is unchanged.
    state.isDarkMode = ResolveDarkMode();
    const bool dark = state.isDarkMode;

    // Windows 7 shows a large header glyph next to the instruction text.
    // kHeaderIconSize is logical pixels; the icon itself is rendered at the
    // matching physical size below so nothing has to be stretched.
    constexpr int kHeaderIconSize = 40;
    HWND icon = Child(state.window, 0, WC_STATICW, L"", SS_ICON | SS_REALSIZECONTROL,
                      12, 12, kHeaderIconSize, kHeaderIconSize, IDC_SOW_ICON,
                      dpi, font);
    Child(state.window, 0, WC_STATICW, LOC(STR_INSTRUCTION), SS_LEFT,
          62, 15, 476, 20, IDC_SOW_INSTRUCTION, dpi, font);
    Child(state.window, 0, WC_STATICW, LOC(STR_FILE_LABEL), SS_LEFT,
          62, 40, 40, 18, IDC_SOW_FILE_LABEL, dpi, font);
    PCWSTR displayedFileName = PathFindFileNameW(state.request.path.c_str());
    if (!displayedFileName || !*displayedFileName)
        displayedFileName = state.request.path.c_str();
    Child(state.window, 0, WC_STATICW, displayedFileName,
          SS_LEFT | SS_PATHELLIPSIS | SS_NOPREFIX,
          102, 40, 431, 18, IDC_SOW_FILE_NAME, dpi, font);
    state.list = Child(state.window, WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
          WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS |
              LVS_SHAREIMAGELISTS | LVS_NOCOLUMNHEADER,
          14, 65, 532, 225, IDC_SOW_PROGRAMS, dpi, font);
    if (state.list) {
        SetWindowSubclass(state.list, PickerListSubclassProc, 1,
                          reinterpret_cast<DWORD_PTR>(&state));
    }
    // Optional description box. When hidden, the label and the edit are
    // not created at all (rather than created and hidden) so they stay
    // out of the tab order, and everything below them slides up by the
    // vertical space they occupied.
    //
    // The offset is read back from the state rather than from the
    // setting: CreatePickerWindow already sized the window with it, and
    // re-reading a setting that changed in between would leave the
    // controls and the window disagreeing about the layout.
    const int shift = state.descriptionOffsetY;
    const bool hideDescription = shift != 0;

    if (!hideDescription) {
        Child(state.window, 0, WC_STATICW, LOC(STR_DESCRIPTION), SS_LEFT,
              14, 300, 532, 18, IDC_SOW_DESCRIPTION_LABEL, dpi, font);
        // Dark mode gets no 3D client edge: the dialog paints its own gray
        // frame around this edit (see PaintDarkControlFrames).
        state.description = Child(state.window,
              static_cast<DWORD>(dark ? 0 : WS_EX_CLIENTEDGE), WC_EDITW, L"",
              WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
              14, 320, 520, 24, IDC_SOW_DESCRIPTION, dpi, font);
    }
    // The checkbox is created directly with the type style the theme
    // needs: BS_OWNERDRAW in dark mode (custom square glyph plus its own
    // label text), plain BS_AUTOCHECKBOX in light mode.
    state.alwaysUse = Child(state.window, 0, WC_BUTTONW, LOC(STR_ALWAYS_USE),
          WS_TABSTOP | (dark ? BS_OWNERDRAW : BS_AUTOCHECKBOX),
          14, 354 - shift, 405, 22, IDC_SOW_ALWAYS_USE, dpi, font);
    // Win7 dialog buttons are 23 logical pixels tall, not 27. The y values
    // are nudged down by the 4 pixels the height loses so the bottom edges
    // stay where the rest of the layout expects them.
    Child(state.window, 0, WC_BUTTONW, LOC(STR_BROWSE),
          WS_TABSTOP | (dark ? BS_OWNERDRAW : BS_PUSHBUTTON),
          452, 354 - shift, 94, 23, IDC_SOW_BROWSE, dpi, font);
    HWND web = Child(state.window, 0, WC_LINK, LOC(STR_WEB_LINK), WS_TABSTOP,
                     14, 386 - shift, 532, 38, IDC_SOW_WEB, dpi, font);
    ShowWindow(web, g_showWebLink.load(std::memory_order_acquire)
                        ? SW_SHOW : SW_HIDE);
    Child(state.window, 0, WC_BUTTONW, LOC(STR_OK),
          WS_TABSTOP | WS_DISABLED | (dark ? BS_OWNERDRAW : BS_DEFPUSHBUTTON),
          391, 430 - shift, 75, 23, IDOK, dpi, font);
    Child(state.window, 0, WC_BUTTONW, LOC(STR_CANCEL),
          WS_TABSTOP | (dark ? BS_OWNERDRAW : BS_PUSHBUTTON),
          471, 430 - shift, 75, 23, IDCANCEL, dpi, font);

    // Rendered at the control's physical size so SS_REALSIZECONTROL has
    // nothing to stretch - a 32x32 icon forced into a DPI-scaled 40x40
    // control is exactly what made the artwork look squashed.
    state.headerIcon.Reset(LoadStandaloneIcon(DpiScale(kHeaderIconSize, dpi)));
    if (state.headerIcon) {
        SendMessageW(icon, STM_SETICON,
                     reinterpret_cast<WPARAM>(state.headerIcon.Get()), 0);
        // Deliberately no WM_SETICON: the real Windows 7 Open With dialog
        // has a plain caption with no icon in it. WS_EX_DLGMODALFRAME
        // suppresses the system menu icon, and leaving the window iconless
        // keeps Alt+Tab/taskbar consistent with the host process.
    }
    InitializeList(state);
    ApplyLocalizedText(state);
    if (state.request.setDefaultOnly && state.alwaysUse) {
        SetAlwaysUseChecked(state, true);
        EnableWindow(state.alwaysUse, FALSE);
    }
}

static void ActivatePickerWindow(HWND window) {
    if (!window || !IsWindow(window)) return;
    ShowWindow(window, SW_RESTORE);
    // A brief topmost pulse reliably raises an owned window without leaving it
    // permanently above unrelated applications.
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetForegroundWindow(window);
}

static LRESULT PickerWndProcBody(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    PickerState* state = reinterpret_cast<PickerState*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    switch (message) {
        case WM_NCCREATE: {
            auto create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            state = static_cast<PickerState*>(create->lpCreateParams);
            SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
            if (state) { state->window = window; g_currentWindow.store(window, std::memory_order_release); }
            return TRUE;
        }
        case WM_CREATE:
            if (!state) return -1;
            BuildPickerControls(*state);
            return 0;
        case WM_ERASEBKGND: {
            RECT client{};
            GetClientRect(window, &client);
            HBRUSH brush = state && state->isDarkMode && state->darkBgBrush
                               ? state->darkBgBrush.Get()
                               : GetSysColorBrush(COLOR_3DFACE);
            FillRect(reinterpret_cast<HDC>(wParam), &client, brush);
            return 1;
        }
        case WM_PAINT: {
            if (!state || !state->isDarkMode) break;
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(window, &ps);
            // EndPaint must run even if the DC is unavailable, otherwise
            // the update region stays invalid and WM_PAINT repeats.
            if (hdc) PaintDarkControlFrames(*state, hdc);
            EndPaint(window, &ps);
            return 0;
        }
        case WM_DRAWITEM: {
            if (!state || !state->isDarkMode) break;
            auto item = reinterpret_cast<LPDRAWITEMSTRUCT>(lParam);
            if (!item) break;
            if (item->CtlID == IDC_SOW_ALWAYS_USE) {
                // Dark-mode recreation of the checkbox: a square glyph
                // with a gray border, a dark fill and an azure check
                // mark, followed by the label text painted with the
                // light text color. Everything (glyph plus text) is
                // drawn by this control, so no sibling static can be
                // repainted with a dark color while hovering. The light
                // theme keeps the classic BS_AUTOCHECKBOX rendering
                // untouched and never reaches this code.
                const UINT dpi = WindowDpi(state->window);
                RECT rc = item->rcItem;
                const int glyphSize = DpiScale(14, dpi);
                RECT glyph = rc;
                glyph.top = rc.top + ((rc.bottom - rc.top) - glyphSize) / 2;
                glyph.right = glyph.left + glyphSize;
                glyph.bottom = glyph.top + glyphSize;
                const bool checked = state->alwaysUseChecked;
                const bool disabled =
                    (item->itemState & ODS_DISABLED) != 0;
                const bool focused = (item->itemState & ODS_FOCUS) != 0;
                POINT cursor{};
                GetCursorPos(&cursor);
                RECT hotScreen = rc;
                MapWindowPoints(item->hwndItem, nullptr,
                                reinterpret_cast<LPPOINT>(&hotScreen), 2);
                const bool hovering =
                    !disabled && PtInRect(&hotScreen, cursor);
                // Repaint the whole item rect with the dialog
                // background: the owner-draw button erases itself with
                // COLOR_BTNFACE before WM_DRAWITEM, which would leave a
                // light patch behind the glyph and the text.
                FillRect(item->hDC, &rc, state->darkBgBrush.Get());
                BrushOwner glyphBrush(CreateSolidBrush(
                    disabled ? RGB(38, 38, 42)
                             : (hovering ? RGB(48, 48, 58)
                                         : RGB(32, 32, 32))));
                if (glyphBrush) FillRect(item->hDC, &glyph, glyphBrush.Get());
                COLORREF borderColor = disabled   ? RGB(70, 70, 75)
                                       : hovering ? RGB(175, 175, 185)
                                                  : RGB(130, 130, 140);
                PenOwner borderPen(CreatePen(PS_SOLID, 1, borderColor));
                {
                    SelectedObject brushSelection(item->hDC,
                                                  GetStockObject(NULL_BRUSH));
                    SelectedObject penSelection(item->hDC, borderPen.Get());
                    Rectangle(item->hDC, glyph.left, glyph.top, glyph.right,
                              glyph.bottom);
                }
                if (checked) {
                    COLORREF checkColor =
                        disabled ? RGB(90, 90, 100) : RGB(0, 168, 255);
                    const int scaled = DpiScale(2, dpi);
                    const int penWidth = scaled > 2 ? scaled : 2;
                    PenOwner checkPen(
                        CreatePen(PS_SOLID, penWidth, checkColor));
                    SelectedObject penSelection(item->hDC, checkPen.Get());
                    const int u = glyphSize;
                    MoveToEx(item->hDC, glyph.left + u * 3 / 14,
                             glyph.top + u * 7 / 14, nullptr);
                    LineTo(item->hDC, glyph.left + u * 6 / 14,
                           glyph.top + u * 10 / 14);
                    LineTo(item->hDC, glyph.left + u * 11 / 14,
                           glyph.top + u * 4 / 14);
                }
                // Label text: always painted with the light color, in
                // every state (normal, hover, focus, pressed), which is
                // what the removed companion static could not guarantee.
                WCHAR label[256];
                const int labelLen =
                    GetWindowTextW(item->hwndItem, label, ARRAYSIZE(label));
                RECT rcText = rc;
                rcText.left = glyph.right + DpiScale(7, dpi);
                if (labelLen > 0) {
                    SetBkMode(item->hDC, TRANSPARENT);
                    SetTextColor(item->hDC, disabled ? RGB(140, 140, 140)
                                                     : RGB(240, 240, 240));
                    SelectedObject labelFontSelection(
                        item->hDC, reinterpret_cast<HFONT>(SendMessageW(
                                       item->hwndItem, WM_GETFONT, 0, 0)));
                    DrawTextW(item->hDC, label, labelLen, &rcText,
                              DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                    if (focused) {
                        RECT rcFocus = rcText;
                        DrawTextW(item->hDC, label, labelLen, &rcFocus,
                                  DT_LEFT | DT_VCENTER | DT_SINGLELINE |
                                      DT_CALCRECT);
                        rcFocus.top = rc.top + 1;
                        rcFocus.bottom = rc.bottom - 1;
                        rcFocus.right += DpiScale(2, dpi);
                        InflateRect(&rcFocus, 1, 0);
                        SetTextColor(item->hDC, RGB(150, 150, 165));
                        DrawFocusRect(item->hDC, &rcFocus);
                    }
                }
                return TRUE;
            }
            if (item->CtlID != IDOK && item->CtlID != IDCANCEL &&
                item->CtlID != IDC_SOW_BROWSE) {
                break;
            }
            // Same dark button painting as the network flyout mod.
            const bool pressed = (item->itemState & ODS_SELECTED) != 0;
            const bool disabled = (item->itemState & ODS_DISABLED) != 0;
            const bool focused = (item->itemState & ODS_FOCUS) != 0;
            const bool hovering =
                state->hoverButton == static_cast<int>(item->CtlID) &&
                !pressed && !disabled;
            HDC hdcReal = item->hDC;
            RECT rc = item->rcItem;
            const int w = rc.right - rc.left;
            const int h = rc.bottom - rc.top;
            if (w <= 0 || h <= 0) break;
            WCHAR text[128];
            const int textLen =
                GetWindowTextW(item->hwndItem, text, ARRAYSIZE(text));
            COLORREF bgColor = disabled   ? RGB(50, 50, 58)
                               : pressed  ? RGB(35, 35, 45)
                               : hovering ? RGB(70, 70, 85)
                                          : RGB(60, 60, 72);
            COLORREF lightColor =
                pressed ? RGB(25, 25, 32)
                        : (hovering ? RGB(95, 95, 115) : RGB(85, 85, 100));
            COLORREF darkColor =
                pressed ? RGB(60, 60, 72)
                        : (hovering ? RGB(35, 35, 45) : RGB(25, 25, 32));
            COLORREF textColor =
                disabled ? RGB(130, 130, 140) : RGB(255, 255, 255);
            COLORREF hoverBorder = hovering ? RGB(90, 90, 120) : RGB(0, 0, 0);
            MemoryDcOwner memoryDc(CreateCompatibleDC(hdcReal));
            if (!memoryDc) return TRUE;
            HDC hdcMem = memoryDc.Get();
            BitmapOwner bitmapMem(CreateCompatibleBitmap(hdcReal, w, h));
            if (!bitmapMem) return TRUE;
            SelectedObject bitmapSelection(hdcMem, bitmapMem.Get());
            RECT rcLocal{0, 0, w, h};
            BrushOwner bgBrush(CreateSolidBrush(bgColor));
            if (bgBrush) FillRect(hdcMem, &rcLocal, bgBrush.Get());
            PenOwner penLight(CreatePen(PS_SOLID, 1, lightColor));
            PenOwner penDark(CreatePen(PS_SOLID, 1, darkColor));
            {
                SelectedObject penSelection(hdcMem, penLight.Get());
                MoveToEx(hdcMem, 0, h - 1, nullptr);
                LineTo(hdcMem, 0, 0);
                LineTo(hdcMem, w - 1, 0);
                SelectObject(hdcMem, penDark.Get());
                MoveToEx(hdcMem, w - 1, 0, nullptr);
                LineTo(hdcMem, w - 1, h - 1);
                LineTo(hdcMem, 0, h - 1);
                if (hovering) {
                    PenOwner penHover(CreatePen(PS_SOLID, 1, hoverBorder));
                    if (penHover) {
                        SelectObject(hdcMem, penHover.Get());
                        MoveToEx(hdcMem, 1, 1, nullptr);
                        LineTo(hdcMem, w - 2, 1);
                        LineTo(hdcMem, w - 2, h - 2);
                        LineTo(hdcMem, 1, h - 2);
                        LineTo(hdcMem, 1, 1);
                        // Deselect before penHover is destroyed: a pen
                        // still selected into a DC cannot be deleted.
                        SelectObject(hdcMem, penDark.Get());
                    }
                }
            }
            if (focused) {
                RECT rcFocus = rcLocal;
                InflateRect(&rcFocus, -3, -3);
                SelectedObject brushSelection(hdcMem,
                                              GetStockObject(NULL_BRUSH));
                SetTextColor(hdcMem, RGB(150, 150, 165));
                DrawFocusRect(hdcMem, &rcFocus);
            }
            SetBkMode(hdcMem, TRANSPARENT);
            SetTextColor(hdcMem, textColor);
            {
                SelectedObject fontSelection(
                    hdcMem, reinterpret_cast<HFONT>(SendMessageW(
                                item->hwndItem, WM_GETFONT, 0, 0)));
                RECT rcText = rcLocal;
                if (pressed) {
                    rcText.left += 1;
                    rcText.top += 1;
                }
                DrawTextW(hdcMem, text, textLen, &rcText,
                          DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            BitBlt(hdcReal, rc.left, rc.top, w, h, hdcMem, 0, 0, SRCCOPY);
            return TRUE;
        }
        case WM_MOUSEMOVE: {
            if (!state || !state->isDarkMode) break;
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ClientToScreen(window, &pt);
            UpdateButtonHover(*state, pt);
            break;
        }
        case WM_SETCURSOR: {
            // Children that don't set their own cursor (the owner-draw
            // buttons) bubble WM_SETCURSOR up to the dialog, so hover
            // tracking keeps working while the pointer is over them.
            if (!state || !state->isDarkMode) break;
            POINT pt{};
            GetCursorPos(&pt);
            UpdateButtonHover(*state, pt);
            SetCursor(LoadCursorW(nullptr, IDC_ARROW));
            return TRUE;
        }
        case WM_MOUSELEAVE: {
            if (!state || !state->isDarkMode) break;
            if (state->hoverButton != 0) {
                HWND button = GetDlgItem(window, state->hoverButton);
                state->hoverButton = 0;
                if (button) InvalidateRect(button, nullptr, FALSE);
            }
            break;
        }
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX: {
            if (state && state->isDarkMode && state->darkCardBrush) {
                HDC hdc = reinterpret_cast<HDC>(wParam);
                SetBkMode(hdc, OPAQUE);
                SetBkColor(hdc, RGB(45, 45, 45));
                SetTextColor(hdc, RGB(240, 240, 240));
                return reinterpret_cast<LRESULT>(state->darkCardBrush.Get());
            }
            break;
        }
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORDLG: {
            if (state && state->isDarkMode && state->darkBgBrush) {
                HDC hdc = reinterpret_cast<HDC>(wParam);
                HWND control = reinterpret_cast<HWND>(lParam);
                SetBkMode(hdc, TRANSPARENT);
                const bool disabled =
                    control && !IsWindowEnabled(control);
                SetTextColor(hdc, disabled ? RGB(140, 140, 140)
                                            : RGB(240, 240, 240));
                return reinterpret_cast<LRESULT>(state->darkBgBrush.Get());
            }
            break;
        }
        case WM_CTLCOLORBTN: {
            if (state && state->isDarkMode && state->darkBgBrush) {
                HDC hdc = reinterpret_cast<HDC>(wParam);
                HWND control = reinterpret_cast<HWND>(lParam);
                // The owner-drawn checkbox needs an opaque background
                // matching the dialog, otherwise the button control
                // erases itself with the light COLOR_BTNFACE.
                if (control == state->alwaysUse) {
                    SetBkMode(hdc, OPAQUE);
                    SetBkColor(hdc, RGB(32, 32, 32));
                    return reinterpret_cast<LRESULT>(state->darkBgBrush.Get());
                }
                SetBkMode(hdc, TRANSPARENT);
                const bool disabled =
                    control && !IsWindowEnabled(control);
                SetTextColor(hdc, disabled ? RGB(140, 140, 140)
                                            : RGB(240, 240, 240));
                return reinterpret_cast<LRESULT>(state->darkBgBrush.Get());
            }
            break;
        }
        case WM_COMMAND:
            if (!state) break;
            if (LOWORD(wParam) == IDOK) {
                const int index = SelectedIndex(*state);
                if (index >= 0 && static_cast<size_t>(index) < state->handlers.size()) {
                    // Surface an unlaunchable selection while the dialog
                    // is still open instead of only beeping after it
                    // closes.
                    if (!SelectedHandlerLaunchable(
                            state->handlers[static_cast<size_t>(index)])) {
                        MessageBoxW(window, LOC(STR_OPEN_FAILED),
                                    LOC(STR_TITLE), MB_ICONERROR | MB_OK);
                        return 0;
                    }
                    state->chosenIndex = index;
                    state->accepted = true;
                    const std::wstring extension =
                        ExtensionOf(state->request.path);
                    state->makeDefaultRequested =
                        extension.size() > 1 && extension[0] == L'.' &&
                        (state->request.setDefaultOnly ||
                         AlwaysUseChecked(*state));
                    if (state->description) {
                        const int chars =
                            GetWindowTextLengthW(state->description);
                        if (chars > 0 && chars < 32767) {
                            try {
                                std::vector<wchar_t> text(chars + 1);
                                if (GetWindowTextW(state->description,
                                                   text.data(),
                                                   static_cast<int>(
                                                       text.size()))) {
                                    state->associationDescription.assign(
                                        text.data());
                                }
                            } catch (...) {
                            }
                        }
                    }
                    DestroyWindow(window);
                }
                return 0;
            }
            if (LOWORD(wParam) == IDCANCEL) { DestroyWindow(window); return 0; }
            if (LOWORD(wParam) == IDC_SOW_BROWSE) { Browse(*state); return 0; }
            if (LOWORD(wParam) == IDC_SOW_ALWAYS_USE) {
                // Dark mode runs the checkbox as owner-drawn: clicks do
                // not auto-toggle, so toggle the mirrored state and
                // repaint. Light mode keeps BS_AUTOCHECKBOX, which
                // toggles on its own; only the mirror is refreshed.
                if (state->isDarkMode) {
                    SetAlwaysUseChecked(*state, !state->alwaysUseChecked);
                } else {
                    state->alwaysUseChecked =
                        Button_GetCheck(state->alwaysUse) == BST_CHECKED;
                }
                return 0;
            }
            break;
        case WM_NOTIFY: {
            if (!state) break;
            auto header = reinterpret_cast<NMHDR*>(lParam);
            if (header && header->idFrom == IDC_SOW_PROGRAMS) {
                if (header->code == NM_CUSTOMDRAW) {
                    // Both themes paint their own rows: a rounded
                    // rectangle with a blue gradient fill and a blue
                    // border for the hovered and the selected row,
                    // instead of the themed highlight. In dark mode that
                    // replaces the DarkMode_Explorer highlight (and
                    // fixes the first entry showing a white background
                    // right after the dialog opens); in light mode it
                    // replaces the host OS highlight, which on Windows
                    // 10/11 is a flat grey rectangle rather than the
                    // Windows 7 blue this dialog recreates.
                    auto listDraw = reinterpret_cast<NMLVCUSTOMDRAW*>(lParam);
                    if (!listDraw) break;
                    if (listDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
                        return CDRF_NOTIFYITEMDRAW;
                    if (listDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
                        // Group headers ("Recommended", "Other Programs")
                        // are reported here too, with dwItemSpec holding
                        // the group id rather than a row index. Only the
                        // "Other Programs" header gets a custom color;
                        // the themed draw ignores clrText for group
                        // labels, so ask for a post-paint pass instead and
                        // redraw the label there (see
                        // RepaintOtherProgramsHeaderText). Everything
                        // else, including the "Recommended" header, keeps
                        // the plain themed draw.
                        if (listDraw->dwItemType == LVCDI_GROUP) {
                            if (state->isDarkMode &&
                                static_cast<int>(listDraw->nmcd.dwItemSpec) ==
                                    GROUP_OTHER) {
                                return CDRF_NOTIFYPOSTPAINT;
                            }
                            break;
                        }
                        // The row is drawn entirely here (background,
                        // icon and label) and the default drawing is
                        // skipped: letting comctl32 draw would repaint
                        // the themed selection rectangle on top of the
                        // Windows 7 frame and put a dark text box over
                        // the hover fill.
                        PaintWin7ListRow(*state, *listDraw);
                        return CDRF_SKIPDEFAULT;
                    }
                    if (listDraw->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT &&
                        state->isDarkMode &&
                        listDraw->dwItemType == LVCDI_GROUP &&
                        static_cast<int>(listDraw->nmcd.dwItemSpec) ==
                            GROUP_OTHER) {
                        RepaintOtherProgramsHeaderText(*state, *listDraw);
                    }
                    break;
                }
                if (header->code == LVN_ITEMCHANGED) UpdateSelectionUi(*state);
                if (header->code == NM_DBLCLK && SelectedIndex(*state) >= 0)
                    SendMessageW(window, WM_COMMAND, IDOK, 0);
                return 0;
            }
            if (header && header->idFrom == IDC_SOW_WEB &&
                (header->code == NM_CLICK || header->code == NM_RETURN)) {
                WebSearch(state->request.path);
                return 0;
            }
            if (header && header->idFrom == IDC_SOW_WEB &&
                header->code == NM_CUSTOMDRAW) {
                auto customDraw = reinterpret_cast<NMCUSTOMDRAW*>(lParam);
                if (customDraw->dwDrawStage == CDDS_PREPAINT) {
                    // SysLink doesn't route through WM_CTLCOLORSTATIC, so its
                    // plain (non-hyperlink) text keeps the default color and
                    // looks inconsistent against a dark background unless we
                    // recolor it here. The hyperlink portion itself keeps its
                    // usual accent color, matching native Explorer dark mode.
                    SetTextColor(customDraw->hdc,
                                 state->isDarkMode ? RGB(240, 240, 240)
                                                    : RGB(0, 0, 0));
                    return CDRF_DODEFAULT;
                }
                break;
            }
            break;
        }
        case WM_SOW_ACTIVATE:
            ActivatePickerWindow(window);
            return 0;
        case WM_SOW_SETTINGS_CHANGED:
            if (state) {
                ApplyPickerTheme(*state);
                ApplyLocalizedText(*state);
            }
            return 0;
        case WM_KEYDOWN: {
            if (wParam == VK_RETURN) {
                SendMessageW(window, WM_COMMAND, IDOK, 0);
                return 0;
            }
            if (wParam == VK_ESCAPE) {
                SendMessageW(window, WM_COMMAND, IDCANCEL, 0);
                return 0;
            }
            break;
        }
        case WM_CLOSE: DestroyWindow(window); return 0;
        case WM_NCDESTROY:
            if (state) { state->finished = true; state->window = nullptr; }
            g_currentWindow.store(nullptr, std::memory_order_release);
            SetWindowLongPtrW(window, GWLP_USERDATA, 0);
            break;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

static LRESULT CALLBACK PickerWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    try { return PickerWndProcBody(window, message, wParam, lParam); }
    catch (...) {
        Wh_Log(L"Standalone Open With: exception contained in window proc");
        if (message != WM_NCDESTROY && IsWindow(window)) DestroyWindow(window);
        return 0;
    }
}

// -----------------------------------------------------------------------------
// Invocation and modeless worker-owned picker loop.
// -----------------------------------------------------------------------------

static std::wstring ExecutableForProgId(const std::wstring& progId) {
    if (progId.empty()) return {};
    wchar_t subKey[2048] = {};
    if (swprintf_s(subKey, L"%s\\shell\\open\\command",
                   progId.c_str()) < 0) {
        return {};
    }
    wchar_t command[32768] = {};
    DWORD bytes = sizeof(command);
    if (RegGetValueW(HKEY_CLASSES_ROOT, subKey, nullptr,
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr,
                     command, &bytes) != ERROR_SUCCESS) {
        return {};
    }
    return ExecutableFromCommand(command);
}

static std::wstring CommandTemplateForProgId(
    const std::wstring& progId) {
    if (progId.empty()) return {};
    wchar_t subKey[2048] = {};
    if (swprintf_s(subKey, L"%s\\shell\\open\\command",
                   progId.c_str()) < 0) {
        return {};
    }
    DWORD bytes = 0;
    if (RegGetValueW(HKEY_CLASSES_ROOT, subKey, nullptr,
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr,
                     nullptr, &bytes) != ERROR_SUCCESS || !bytes ||
        bytes > 65534 * sizeof(wchar_t)) {
        return {};
    }
    try {
        std::vector<wchar_t> command(bytes / sizeof(wchar_t) + 2);
        if (RegGetValueW(HKEY_CLASSES_ROOT, subKey, nullptr,
                         RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr,
                         command.data(), &bytes) != ERROR_SUCCESS) {
            return {};
        }
        return command.data();
    } catch (...) {
        return {};
    }
}

static std::wstring ExecutableForHandler(const HandlerEntry& handler) {
    if (IsSupportedFile(handler.internalName)) return handler.internalName;
    std::wstring executable = ExecutableForProgId(handler.progId);
    if (IsSupportedFile(executable)) return executable;
    if (!_wcsnicmp(handler.progId.c_str(), L"Applications\\", 13)) {
        PCWSTR executableName = handler.progId.c_str() + 13;
        wchar_t resolved[MAX_PATH] = {};
        if (SearchPathW(nullptr, executableName, nullptr, ARRAYSIZE(resolved),
                        resolved, nullptr)) {
            return resolved;
        }
    }
    return {};
}

static std::wstring QuoteCommandLineArgument(const std::wstring& value) {
    std::wstring result;
    try {
        result.push_back(L'"');
        size_t backslashes = 0;
        for (wchar_t c : value) {
            if (c == L'\\') {
                ++backslashes;
                continue;
            }
            if (c == L'"') {
                result.append(backslashes * 2 + 1, L'\\');
                result.push_back(L'"');
                backslashes = 0;
                continue;
            }
            result.append(backslashes, L'\\');
            backslashes = 0;
            result.push_back(c);
        }
        // Backslashes before the closing quote must be doubled.
        result.append(backslashes * 2, L'\\');
        result.push_back(L'"');
    } catch (...) {
        return {};
    }
    return result;
}

static bool ReplaceCommandPlaceholder(std::wstring& command,
                                      const std::wstring& placeholder,
                                      const std::wstring& replacement) {
    bool replaced = false;
    size_t position = 0;
    while ((position = command.find(placeholder, position)) !=
           std::wstring::npos) {
        command.replace(position, placeholder.size(), replacement);
        position += replacement.size();
        replaced = true;
    }
    return replaced;
}

static std::wstring BuildCommandLineFromTemplate(
    const std::wstring& commandTemplate, const std::wstring& path) {
    if (commandTemplate.empty() || path.empty()) return {};
    std::wstring command = commandTemplate;
    const std::wstring quotedPath = QuoteCommandLineArgument(path);
    if (quotedPath.empty()) return {};
    try {
        bool replaced = false;
        // Replace already-quoted placeholders first to avoid producing
        // doubled quotes such as ""C:\\file"".
        replaced |= ReplaceCommandPlaceholder(command, L"\"%1\"", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"\"%L\"", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"\"%l\"", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"\"%*\"", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"%1", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"%L", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"%l", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"%*", quotedPath);
        if (!replaced) {
            command.push_back(L' ');
            command.append(quotedPath);
        }
        return command;
    } catch (...) {
        return {};
    }
}

static HRESULT CreateProcessFromCommandLine(const std::wstring& commandLine,
                                            PCWSTR currentDirectory = nullptr) {
    if (commandLine.empty()) return E_INVALIDARG;
    try {
        std::vector<wchar_t> mutableCommand(commandLine.begin(),
                                            commandLine.end());
        mutableCommand.push_back(L'\0');
        STARTUPINFOW startup{sizeof(startup)};
        PROCESS_INFORMATION process{};
        if (!CreateProcessW(nullptr, mutableCommand.data(), nullptr, nullptr,
                            FALSE, 0, nullptr, currentDirectory, &startup,
                            &process)) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
        WinHandle processHandle(process.hProcess);
        WinHandle threadHandle(process.hThread);
        return S_OK;
    } catch (...) {
        return E_OUTOFMEMORY;
    }
}

static HRESULT InvokeCommandTemplate(const std::wstring& commandTemplate,
                                     const std::wstring& path) {
    const std::wstring commandLine =
        BuildCommandLineFromTemplate(commandTemplate, path);
    if (commandLine.empty()) return E_FAIL;
    const std::wstring executable = ExecutableFromCommand(commandLine.c_str());
    if (IsOpenWithExecutable(executable))
        return HRESULT_FROM_WIN32(ERROR_NO_ASSOCIATION);
    const HRESULT hr = CreateProcessFromCommandLine(commandLine);
    Wh_Log(L"Standalone Open With: direct command launch command=%s "
           L"hr=0x%08X", commandLine.c_str(),
           static_cast<unsigned int>(hr));
    return hr;
}

static HRESULT InvokeExecutableWithFile(const std::wstring& executable,
                                        const std::wstring& path) {
    if (!IsSupportedFile(executable) || !IsSupportedFile(path) ||
        IsOpenWithExecutable(executable)) {
        return E_INVALIDARG;
    }
    const std::wstring commandLine =
        QuoteCommandLineArgument(executable) + L" " +
        QuoteCommandLineArgument(path);
    const HRESULT hr = CreateProcessFromCommandLine(commandLine);
    Wh_Log(L"Standalone Open With: direct executable launch exe=%s file=%s "
           L"hr=0x%08X", executable.c_str(), path.c_str(),
           static_cast<unsigned int>(hr));
    return hr;
}

// AppX / Store handlers carry only a DelegateExecute value in their
// shell\open\command key: they have neither a usable command template nor a
// direct executable path. Such handlers are launched through their own
// IAssocHandler::Invoke - unlike generic association resolution, invoking
// the specific handler the user picked does not re-resolve the association,
// so it cannot re-enter Open With.
static HRESULT InvokeHandlerWithDataObject(StandaloneAssocHandler* handler,
                                           const std::wstring& path) {
    if (!handler || !IsSupportedFile(path)) return E_INVALIDARG;
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0,
                                  nullptr)) ||
        !pidl) {
        return E_FAIL;
    }
    ComPtr<IDataObject> dataObject;
    // SHCreateDataObject takes an array of const PIDLs.
    LPCITEMIDLIST pidls[1] = {pidl};
    const HRESULT createHr = SHCreateDataObject(
        nullptr, 1, pidls, nullptr, IID_PPV_ARGS(dataObject.Put()));
    ILFree(pidl);
    if (FAILED(createHr) || !dataObject) return createHr;
    const HRESULT invokeHr = handler->Invoke(dataObject.Get());
    Wh_Log(L"Standalone Open With: handler invoker fallback hr=0x%08X",
           static_cast<unsigned int>(invokeHr));
    return invokeHr;
}

// Whether the given entry has at least one working launch route (direct
// executable, command template or handler invoker). Used to surface an
// unlaunchable selection while the dialog is still open instead of beeping
// only after it has closed.
static bool SelectedHandlerLaunchable(const HandlerEntry& entry) {
    if (entry.handler) return true;
    if (!entry.internalName.empty() && IsSupportedFile(entry.internalName))
        return true;
    if (!entry.progId.empty()) {
        if (!CommandTemplateForProgId(entry.progId).empty()) return true;
        if (!ExecutableForProgId(entry.progId).empty()) return true;
    }
    return false;
}

static HRESULT InvokeSelectedHandler(const PickerState& state,
                                     HandlerEntry& selected) {
    try {
        const std::wstring executable = ExecutableForHandler(selected);
        if (IsOpenWithExecutable(executable) ||
            IsOpenWithHandlerName(selected.internalName, selected.progId)) {
            return HRESULT_FROM_WIN32(ERROR_NO_ASSOCIATION);
        }

        // Never pass a user-selected Win32 program back through generic Shell
        // association resolution: on an unknown type that can re-enter
        // OpenWith. Resolve and launch the command template directly instead.
        HRESULT directHr = E_FAIL;
        if (!selected.progId.empty()) {
            const std::wstring commandTemplate =
                CommandTemplateForProgId(selected.progId);
            if (!commandTemplate.empty()) {
                directHr = InvokeCommandTemplate(commandTemplate,
                                                 state.request.path);
            }
        }
        if (FAILED(directHr)) {
            directHr = InvokeExecutableWithFile(executable,
                                                state.request.path);
        }
        if (FAILED(directHr) && selected.handler) {
            directHr = InvokeHandlerWithDataObject(selected.handler.Get(),
                                                   state.request.path);
        }
        Wh_Log(L"Standalone Open With: selected handler direct result "
               L"progId=%s exe=%s hr=0x%08X",
               selected.progId.c_str(), executable.c_str(),
               static_cast<unsigned int>(directHr));
        return directHr;
    } catch (...) {
        return E_FAIL;
    }
}

static HRESULT InvokeBrowsed(const PickerState& state,
                             const std::wstring& executable) {
    return InvokeExecutableWithFile(executable, state.request.path);
}

static HRESULT MakeSelectedDefault(PickerState& state,
                                   HandlerEntry& selected) {
    const std::wstring extension = ExtensionOf(state.request.path);
    if (extension.size() <= 1) return E_INVALIDARG;

    if (selected.progId.empty())
        selected.progId = ResolveHandlerProgId(selected.internalName);
    if (selected.progId.empty() && IsSupportedFile(selected.internalName)) {
        EnsureUserApplicationRegistration(selected.internalName, extension,
                                          &selected.progId);
    }
    if (selected.progId.empty()) return E_FAIL;

    HRESULT shellHr = E_NOTIMPL;
    if (selected.handler) {
        PCWSTR description = !state.associationDescription.empty()
            ? state.associationDescription.c_str()
            : extension.c_str();
        shellHr = selected.handler->MakeDefault(description);
        Wh_Log(L"Standalone Open With: IAssocHandler::MakeDefault handler=%s "
               L"progId=%s hr=0x%08X", selected.displayName.c_str(),
               selected.progId.c_str(),
               static_cast<unsigned int>(shellHr));
    }

    Wh_Log(L"Standalone Open With: association result extension=%s "
           L"progId=%s shellHr=0x%08X",
           extension.c_str(), selected.progId.c_str(),
           static_cast<unsigned int>(shellHr));

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST | SHCNF_FLUSH,
                   nullptr, nullptr);
    return shellHr;
}

static WinHandle g_stopEvent;
static WinHandle g_requestEvent;
static WinHandle g_workerReadyEvent;
static std::atomic<bool> g_workerReady{false};
static std::mutex g_requestMutex;
static std::optional<PickerRequest> g_pendingRequest;
#if defined(__clang__)
[[clang::no_destroy]]
#endif
static std::optional<std::thread> g_worker;
// Set by the worker thread just before it returns, so a worker that
// stopped (a failed startup, or a stop/start across a mod reload) can be
// joined and replaced instead of blocking every future request.
static std::atomic<bool> g_workerExited{false};

static bool RegisterPickerClass() {
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = PickerWndProc;
    wc.hInstance = ModInstance();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    // The per-window Base64 icon is assigned after creation and owned by the
    // picker state; the class itself owns no icon handle.
    wc.hIcon = nullptr;
    wc.hIconSm = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszClassName = kWindowClass;
    if (RegisterClassExW(&wc)) return true;

    // Recompiling the mod unloads and reloads the DLL, but a window class
    // is owned by USER32 and outlives the module that registered it. The
    // stale registration still points lpfnWndProc at the previous image,
    // so registration fails here with ERROR_CLASS_ALREADY_EXISTS and the
    // worker used to give up - leaving every request to fall through to
    // the system dialog until the host process restarted. Drop the stale
    // class and register again.
    if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;
    if (!UnregisterClassW(kWindowClass, ModInstance())) {
        Wh_Log(L"Standalone Open With: stale window class still in use, "
               L"cannot re-register (error %u)", GetLastError());
        return false;
    }
    if (RegisterClassExW(&wc)) {
        Wh_Log(L"Standalone Open With: re-registered picker class after "
               L"a previous module instance");
        return true;
    }
    Wh_Log(L"Standalone Open With: picker class registration failed "
           L"(error %u)", GetLastError());
    return false;
}

class PickerClassRegistration {
   public:
    bool Register() {
        if (registered_) return true;
        registered_ = RegisterPickerClass();
        return registered_;
    }
    ~PickerClassRegistration() {
        if (registered_) UnregisterClassW(kWindowClass, ModInstance());
    }
    PickerClassRegistration(const PickerClassRegistration&) = delete;
    PickerClassRegistration& operator=(const PickerClassRegistration&) = delete;
    PickerClassRegistration() = default;

   private:
    bool registered_ = false;
};

static HWND CreatePickerWindow(PickerState& state) {
    const UINT dpi = WindowDpi(state.request.owner);
    // Decided here, before CreateWindowExW dispatches WM_CREATE, so that
    // BuildPickerControls lays the controls out against the same offset
    // this window was sized with.
    state.descriptionOffsetY = DescriptionLayoutShift();
    int width = DpiScale(560, dpi),
        height = DpiScale(465 - state.descriptionOffsetY, dpi);
    RECT rect{0, 0, width, height};
    AdjustWindowRectEx(&rect, WS_POPUP | WS_CAPTION | WS_SYSMENU, FALSE,
                       WS_EX_DLGMODALFRAME | WS_EX_CONTROLPARENT);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    HMONITOR monitor = MonitorFromWindow(state.request.owner, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(monitor, &mi);
    const int x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - width) / 2;
    const int y = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - height) / 2;
    return CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_CONTROLPARENT,
        kWindowClass, LOC(STR_TITLE), WS_POPUP | WS_CAPTION | WS_SYSMENU,
        x, y, width, height, state.request.owner, nullptr, ModInstance(), &state);
}

// Puts the host process's preferred app mode back the way it was as soon
// as the picker is gone, on every exit path out of ShowPicker.
class AppModeRestorer {
   public:
    AppModeRestorer() = default;
    ~AppModeRestorer() { DarkModeActivation::RestoreAppMode(); }
    AppModeRestorer(const AppModeRestorer&) = delete;
    AppModeRestorer& operator=(const AppModeRestorer&) = delete;
};

// Signals the waiting caller that the picker finished, and publishes
// whether the user actually accepted a program (OK / double click) or
// dismissed the dialog (Cancel, Escape, close box). The accepted flag is
// written before the event is set, so a caller released by the event is
// guaranteed to observe the final value.
class PickerCompletionSignal {
   public:
    PickerCompletionSignal(HANDLE event, std::atomic<bool>* acceptedOut)
        : event_(event), acceptedOut_(acceptedOut) {}
    ~PickerCompletionSignal() {
        if (acceptedOut_)
            acceptedOut_->store(accepted_, std::memory_order_release);
        if (event_) SetEvent(event_);
    }
    PickerCompletionSignal(const PickerCompletionSignal&) = delete;
    PickerCompletionSignal& operator=(const PickerCompletionSignal&) = delete;

    void SetAccepted(bool accepted) { accepted_ = accepted; }

   private:
    HANDLE event_;
    std::atomic<bool>* acceptedOut_;
    bool accepted_ = false;
};

static void ShowPicker(PickerRequest request) {
    DetermineLocale();
    // Destruction order matters: the restorer is declared after the
    // completion signal, so it runs FIRST and the host process's app mode
    // is already back to normal by the time the waiting caller is released.
    PickerCompletionSignal completion(request.completionEvent,
                                      request.acceptedOut);
    AppModeRestorer appModeRestorer;
    if (g_shuttingDown.load(std::memory_order_acquire) || !IsSupportedFile(request.path)) return;
    PickerState state;
    state.request = std::move(request);
    RefreshPickerThemeResources(state);
    EnumerateHandlers(state);
    HWND window = CreatePickerWindow(state);
    if (!window) return;
    WindowOwner windowOwner(window);
    ShowWindow(window, SW_SHOWNORMAL);
    ActivatePickerWindow(window);
    // Give the list the focus so keyboard navigation starts on the
    // pre-selected first entry, which is painted with the Windows 7 blue
    // selection frame. Both themes do this now that both paint that
    // frame themselves.
    if (state.list && IsWindow(state.list)) SetFocus(state.list);

    HANDLE stopHandle = g_stopEvent.get();
    while (!state.finished) {
        const DWORD wait = MsgWaitForMultipleObjects(1, &stopHandle, FALSE,
                                                      INFINITE, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) {
            if (IsWindow(window)) DestroyWindow(window);
        }
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (!IsWindow(window)) break;
            if (!IsDialogMessageW(window, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        if (!IsWindow(window)) state.finished = true;
    }
    // The user pressed OK (or double clicked a program) rather than
    // cancelling: report that to the waiting hook, which turns it into
    // S_OK / TRUE instead of ERROR_CANCELLED.
    completion.SetAccepted(state.accepted);

    if (!state.accepted || state.chosenIndex < 0 ||
        static_cast<size_t>(state.chosenIndex) >= state.handlers.size() ||
        g_shuttingDown.load(std::memory_order_acquire)) return;
    HandlerEntry& selected =
        state.handlers[static_cast<size_t>(state.chosenIndex)];

    if (selected.browsed) {
        EnsureUserApplicationRegistration(selected.internalName,
                                          ExtensionOf(state.request.path),
                                          &selected.progId);
    }

    HRESULT defaultHr = S_OK;
    if (state.makeDefaultRequested)
        defaultHr = MakeSelectedDefault(state, selected);

    HRESULT invokeHr = S_OK;
    if (!state.request.setDefaultOnly) {
        invokeHr = selected.browsed
            ? InvokeBrowsed(state, selected.internalName)
            : InvokeSelectedHandler(state, selected);
    }

    if (FAILED(defaultHr) &&
        g_defaultBehavior.load(std::memory_order_acquire) ==
            DefaultBehavior::OpenSettings) {
        state.openDefaultSettings = true;
    }
    if (FAILED(defaultHr) || FAILED(invokeHr)) {
        Wh_Log(L"Standalone Open With: request failed default=0x%08X "
               L"invoke=0x%08X",
               static_cast<unsigned int>(defaultHr),
               static_cast<unsigned int>(invokeHr));
        MessageBeep(MB_ICONERROR);
    }

    if (state.openDefaultSettings) {
        ShellExecuteW(nullptr, L"open", L"ms-settings:defaultapps", nullptr,
                      nullptr, SW_SHOWNORMAL);
    }
}

static void WorkerMain() {
    ComApartment apartment(COINIT_APARTMENTTHREADED);
    if (!apartment.Ready()) {
        Wh_Log(L"Standalone Open With: worker COM initialization failed (0x%08X)",
               static_cast<unsigned int>(apartment.Result()));
        return;
    }
    INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_LISTVIEW_CLASSES | ICC_LINK_CLASS | ICC_STANDARD_CLASSES};
    if (!InitCommonControlsEx(&controls)) {
        Wh_Log(L"Standalone Open With: common-controls initialization failed");
        return;
    }
    MSG createQueue{};
    PeekMessageW(&createQueue, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    PickerClassRegistration classRegistration;
    if (!classRegistration.Register()) {
        Wh_Log(L"Standalone Open With: window class registration failed");
        return;
    }
    g_workerReady.store(true, std::memory_order_release);
    if (g_workerReadyEvent) SetEvent(g_workerReadyEvent.get());
    HANDLE handles[] = {g_stopEvent.get(), g_requestEvent.get()};
    for (;;) {
        const DWORD wait = MsgWaitForMultipleObjects(2, handles, FALSE, INFINITE, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) break;
        if (wait == WAIT_OBJECT_0 + 1) {
            std::optional<PickerRequest> request;
            {
                std::lock_guard<std::mutex> lock(g_requestMutex);
                request.swap(g_pendingRequest);
            }
            if (request) {
                try { ShowPicker(std::move(*request)); }
                catch (...) { Wh_Log(L"Standalone Open With: picker request failed"); }
            }
        }
        if (wait == WAIT_OBJECT_0 + 2) {
            MSG message{};
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    }
    g_workerReady.store(false, std::memory_order_release);
    if (HWND window = g_currentWindow.load(std::memory_order_acquire))
        SendMessageW(window, WM_CLOSE, 0, 0);
}

static void WorkerMainNoexcept() {
    try {
        WorkerMain();
    } catch (...) {
        g_workerReady.store(false, std::memory_order_release);
        Wh_Log(L"Standalone Open With: worker exception contained");
    }
    // Mark the thread as finished so StartWorkerIfNeeded can reap it and
    // try again. Without this a worker that failed to start up (COM,
    // common controls or the window class) would leave g_worker engaged
    // but never ready, and every later request would fall through to the
    // system dialog for the lifetime of the process.
    g_workerExited.store(true, std::memory_order_release);
    // Also release a waiter if initialization failed before Ready was set.
    if (g_workerReadyEvent) SetEvent(g_workerReadyEvent.get());
}

static bool WaitForPickerWorker(DWORD timeoutMilliseconds) {
    if (g_workerReady.load(std::memory_order_acquire)) return true;
    if (!g_workerReadyEvent) return false;
    WaitForSingleObject(g_workerReadyEvent.get(), timeoutMilliseconds);
    return g_workerReady.load(std::memory_order_acquire);
}

// The worker thread (COM STA, common controls, picker window class and
// message loop) is started lazily on the first picker request instead of
// at injection time. @include rundll32.exe injects the mod into every
// rundll32 process - Control Panel applets, printer/device UI,
// shell32 Control_RunDLL and dozens of other hosts that never show an
// Open With dialog - so those processes must not pay for the worker at
// startup. The two QueuePicker entry points are the only users of the
// worker, so the start lives here.
//
// Deliberately a mutex and not std::call_once: a once_flag is consumed
// for the lifetime of the loaded module, and a mod recompile reloads the
// DLL into a host process that is still running. StopWorker joins and
// clears g_worker on unload, but the spent once_flag meant the reloaded
// module could never start a second worker, so every subsequent request
// fell through to the system dialog. Keying the start off g_worker
// itself makes it repeatable.
static std::mutex g_workerStartMutex;
static bool StartWorkerIfNeeded() {
    {
        std::lock_guard<std::mutex> lock(g_workerStartMutex);
        // Reap a worker that already returned before deciding whether one
        // has to be started.
        if (g_worker && g_workerExited.load(std::memory_order_acquire)) {
            if (g_worker->joinable()) g_worker->join();
            g_worker.reset();
        }
        if (!g_worker && !g_shuttingDown.load(std::memory_order_acquire)) {
            g_workerExited.store(false, std::memory_order_release);
            g_workerReady.store(false, std::memory_order_release);
            if (g_workerReadyEvent) ResetEvent(g_workerReadyEvent.get());
            try {
                g_worker.emplace(WorkerMainNoexcept);
            } catch (...) {
                Wh_Log(L"Standalone Open With: worker thread creation failed");
            }
        }
    }
    return g_worker.has_value() && WaitForPickerWorker(3000);
}

static bool QueuePicker(HWND owner, PCWSTR path,
                        bool setDefaultOnly = false) {
    if (!g_replaceSystemDialog.load(std::memory_order_acquire) ||
        g_shuttingDown.load(std::memory_order_acquire) || !path) {
        Wh_Log(L"Standalone Open With: QueuePicker rejected early (replace=%d "
               L"shuttingDown=%d hasPath=%d)",
               g_replaceSystemDialog.load(std::memory_order_acquire),
               g_shuttingDown.load(std::memory_order_acquire), path ? 1 : 0);
        return false;
    }
    // Start the worker lazily on the first request (see
    // StartWorkerIfNeeded); hosts that never show a picker never pay
    // for the thread.
    if (!StartWorkerIfNeeded()) {
        Wh_Log(L"Standalone Open With: QueuePicker rejected, worker unavailable");
        return false;
    }
    std::wstring copy;
    try { copy = path; } catch (...) { return false; }
    if (!IsSupportedFile(copy)) {
        Wh_Log(L"Standalone Open With: QueuePicker rejected, unsupported file "
               L"(path=%s)", copy.c_str());
        return false;
    }

    if (HWND current = g_currentWindow.load(std::memory_order_acquire)) {
        PostMessageW(current, WM_SOW_ACTIVATE, 0, 0);
        return false;
    }

    std::lock_guard<std::mutex> lock(g_requestMutex);
    if (g_pendingRequest) {
        return false;
    }
    if (HWND current = g_currentWindow.load(std::memory_order_acquire)) {
        PostMessageW(current, WM_SOW_ACTIVATE, 0, 0);
        return false;
    }
    try {
        g_pendingRequest.emplace(PickerRequest{std::move(copy), owner, nullptr,
                                               setDefaultOnly, nullptr});
    } catch (...) {
        return false;
    }
    if (!SetEvent(g_requestEvent.get())) { g_pendingRequest.reset(); return false; }
    return true;
}

// Outcome of a synchronous picker request. The three states are distinct
// because the hooked APIs report them differently: only Accepted is a
// success, Cancelled must surface as ERROR_CANCELLED, and NotHandled means
// the mod declined the request and the original API has to run.
enum class PickerOutcome { NotHandled, Cancelled, Accepted };

static PickerOutcome QueuePickerAndWait(HWND owner, PCWSTR path,
                                        bool setDefaultOnly = false) {
    if (!path ||
        !g_replaceSystemDialog.load(std::memory_order_acquire) ||
        g_shuttingDown.load(std::memory_order_acquire) ||
        !StartWorkerIfNeeded()) {
        return PickerOutcome::NotHandled;
    }

    std::wstring copy;
    try {
        copy = path;
    } catch (...) {
        return PickerOutcome::NotHandled;
    }
    if (!IsSupportedFile(copy)) return PickerOutcome::NotHandled;

    WinHandle completion(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!completion) return PickerOutcome::NotHandled;
    // Lives on this frame for the whole wait below: the worker writes the
    // OK/Cancel result into it just before signaling the completion event.
    std::atomic<bool> accepted{false};
    {
        std::lock_guard<std::mutex> lock(g_requestMutex);
        if (g_pendingRequest ||
            g_currentWindow.load(std::memory_order_acquire)) {
            return PickerOutcome::NotHandled;
        }
        try {
            g_pendingRequest.emplace(
                PickerRequest{std::move(copy), owner, completion.get(),
                              setDefaultOnly, &accepted});
        } catch (...) {
            return PickerOutcome::NotHandled;
        }
        if (!SetEvent(g_requestEvent.get())) {
            g_pendingRequest.reset();
            return PickerOutcome::NotHandled;
        }
    }

    HANDLE waits[] = {completion.get(), g_stopEvent.get()};
    DWORD completedIndex = 0;
    HRESULT waitResult = CoWaitForMultipleHandles(
        COWAIT_DISPATCH_CALLS | COWAIT_DISPATCH_WINDOW_MESSAGES,
        INFINITE, ARRAYSIZE(waits), waits, &completedIndex);
    if (FAILED(waitResult)) {
        // The calling thread may not have COM initialized
        // (CO_E_NOTINITIALIZED) or may be an MTA thread (no window-message
        // dispatch). Blocking it on a bare WaitForMultipleObjects would
        // freeze its windows ("Not Responding") and deadlock the worker:
        // the picker is owned by a window of this thread, and USER32
        // needs it to pump messages to deactivate/activate that owner.
        // Fall back to a message-pumping wait instead.
        for (;;) {
            const DWORD wait = MsgWaitForMultipleObjects(
                ARRAYSIZE(waits), waits, FALSE, INFINITE, QS_ALLINPUT);
            if (wait == WAIT_OBJECT_0 || wait == WAIT_OBJECT_0 + 1) {
                completedIndex = wait - WAIT_OBJECT_0;
                break;
            }
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }
    // completedIndex 1 is g_stopEvent: the mod is unloading, the picker
    // never reached a decision, so let the original API handle it.
    if (completedIndex != 0) return PickerOutcome::NotHandled;
    return accepted.load(std::memory_order_acquire) ? PickerOutcome::Accepted
                                                    : PickerOutcome::Cancelled;
}

// -----------------------------------------------------------------------------
// Stable Open-With context-menu boundary (ReactOS-inspired).
// -----------------------------------------------------------------------------

// ReactOS implements Open With as the CLSID_OpenWithMenu shell extension. The
// stable boundary is IShellExtInit::Initialize (receives the selected item) plus
// IContextMenu::InvokeCommand (receives the canonical "openas" verb), before any
// Windows-version-specific picker implementation is involved.
static const CLSID kClsidOpenWithMenu = {
    0x09799AFB,
    0xAD67,
    0x11D1,
    {0xAB, 0xCD, 0x00, 0xC0, 0x4F, 0xC3, 0x09, 0x36}};

struct OpenWithMenuState {
    std::wstring path;
    int openAsOffset = -1;
};

static std::mutex g_openWithMenuStateMutex;
static std::unordered_map<void*, OpenWithMenuState> g_openWithMenuStates;

static void* ComIdentity(IUnknown* object) {
    if (!object) return nullptr;
    IUnknown* identity = nullptr;
    if (SUCCEEDED(object->QueryInterface(IID_IUnknown,
                                         reinterpret_cast<void**>(&identity))) &&
        identity) {
        void* key = identity;
        identity->Release();
        return key;
    }
    return object;
}

using SHCreateShellItemArrayFromDataObject_t = HRESULT(WINAPI*)(
    IDataObject*, REFIID, void**);

static std::wstring PathFromDataObject(IDataObject* dataObject) {
    if (!dataObject) return {};

    // Prefer the documented Shell item-array conversion. Resolve it at runtime
    // so an older SDK or OS doesn't become a hard dependency.
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    auto createArray = shell32
        ? reinterpret_cast<SHCreateShellItemArrayFromDataObject_t>(
              GetProcAddress(shell32,
                             "SHCreateShellItemArrayFromDataObject"))
        : nullptr;
    if (createArray) {
        ComPtr<IShellItemArray> array;
        if (SUCCEEDED(createArray(dataObject, IID_IShellItemArray,
                                  reinterpret_cast<void**>(array.Put()))) &&
            array) {
            DWORD count = 0;
            if (SUCCEEDED(array->GetCount(&count)) && count == 1) {
                ComPtr<IShellItem> item;
                if (SUCCEEDED(array->GetItemAt(0, item.Put())) && item) {
                    PWSTR raw = nullptr;
                    if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH,
                                                       &raw)) && raw) {
                        return TakeTaskString(raw);
                    }
                    if (raw) CoTaskMemFree(raw);
                }
            }
        }
    }

    // CF_HDROP fallback for callers which expose only a classic data object.
    FORMATETC format{};
    format.cfFormat = CF_HDROP;
    format.dwAspect = DVASPECT_CONTENT;
    format.lindex = -1;
    format.tymed = TYMED_HGLOBAL;
    STGMEDIUM medium{};
    if (FAILED(dataObject->GetData(&format, &medium))) return {};

    std::wstring result;
    void* locked = medium.hGlobal ? GlobalLock(medium.hGlobal) : nullptr;
    if (locked) {
        HDROP drop = reinterpret_cast<HDROP>(locked);
        if (DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0) == 1) {
            const UINT chars = DragQueryFileW(drop, 0, nullptr, 0);
            if (chars && chars < 32767) {
                try {
                    std::vector<wchar_t> path(chars + 1);
                    if (DragQueryFileW(drop, 0, path.data(),
                                       static_cast<UINT>(path.size()))) {
                        result.assign(path.data());
                    }
                } catch (...) {
                }
            }
        }
        GlobalUnlock(medium.hGlobal);
    }
    ReleaseStgMedium(&medium);
    return result;
}

static bool CanonicalVerbIsOpenAs(IContextMenu* menu, UINT_PTR offset) {
    if (!menu) return false;

    wchar_t wide[64] = {};
    if (SUCCEEDED(menu->GetCommandString(
            offset, GCS_VERBW, nullptr, reinterpret_cast<LPSTR>(wide),
            ARRAYSIZE(wide))) &&
        !_wcsicmp(wide, L"openas")) {
        return true;
    }

    char narrow[64] = {};
    return SUCCEEDED(menu->GetCommandString(offset, GCS_VERBA, nullptr,
                                            narrow, ARRAYSIZE(narrow))) &&
           !_stricmp(narrow, "openas");
}

using OpenWithMenuInitialize_t = HRESULT(STDMETHODCALLTYPE*)(
    IShellExtInit*, PCIDLIST_ABSOLUTE, IDataObject*, HKEY);
using OpenWithMenuQueryContextMenu_t = HRESULT(STDMETHODCALLTYPE*)(
    IContextMenu*, HMENU, UINT, UINT, UINT, UINT);
using OpenWithMenuInvokeCommand_t = HRESULT(STDMETHODCALLTYPE*)(
    IContextMenu*, LPCMINVOKECOMMANDINFO);

static OpenWithMenuInitialize_t OpenWithMenuInitializeOriginal = nullptr;
static OpenWithMenuQueryContextMenu_t OpenWithMenuQueryContextMenuOriginal =
    nullptr;
static OpenWithMenuInvokeCommand_t OpenWithMenuInvokeCommandOriginal = nullptr;

static HRESULT STDMETHODCALLTYPE OpenWithMenuInitializeHook(
    IShellExtInit* self, PCIDLIST_ABSOLUTE folder, IDataObject* dataObject,
    HKEY classKey) {
    const HRESULT hr = OpenWithMenuInitializeOriginal
                           ? OpenWithMenuInitializeOriginal(
                                 self, folder, dataObject, classKey)
                           : E_FAIL;
    try {
        std::wstring path = PathFromDataObject(dataObject);
        void* identity = ComIdentity(static_cast<IUnknown*>(self));
        if (identity) {
            std::lock_guard<std::mutex> lock(g_openWithMenuStateMutex);
            if (IsSupportedFile(path)) {
                if (g_openWithMenuStates.size() > 256)
                    g_openWithMenuStates.clear();
                g_openWithMenuStates[identity] =
                    OpenWithMenuState{std::move(path), -1};
                Wh_Log(L"Standalone Open With: captured CLSID_OpenWithMenu item");
            } else {
                g_openWithMenuStates.erase(identity); // Clean up stale state
            }
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: context-menu Initialize hook exception");
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE OpenWithMenuQueryContextMenuHook(
    IContextMenu* self, HMENU menu, UINT index, UINT first, UINT last,
    UINT flags) {
    const HRESULT hr = OpenWithMenuQueryContextMenuOriginal
                           ? OpenWithMenuQueryContextMenuOriginal(
                                 self, menu, index, first, last, flags)
                           : E_FAIL;
    try {
        if (SUCCEEDED(hr)) {
            const UINT count = HRESULT_CODE(hr);
            int openAsOffset = -1;
            for (UINT offset = 0; offset < count; ++offset) {
                if (CanonicalVerbIsOpenAs(self, offset)) {
                    openAsOffset = static_cast<int>(offset);
                    break;
                }
            }
            if (openAsOffset >= 0) {
                void* identity = ComIdentity(static_cast<IUnknown*>(self));
                std::lock_guard<std::mutex> lock(g_openWithMenuStateMutex);
                auto found = g_openWithMenuStates.find(identity);
                if (found != g_openWithMenuStates.end())
                    found->second.openAsOffset = openAsOffset;
                Wh_Log(L"Standalone Open With: openas command offset=%d",
                       openAsOffset);
            }
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: QueryContextMenu hook exception");
    }
    return hr;
}

static bool InvocationUsesOpenAs(IContextMenu* self,
                                 LPCMINVOKECOMMANDINFO info,
                                 int knownOffset) {
    if (!info) return false;

    // MAKEINTRESOURCE(0) is a null pointer. It is nevertheless the valid
    // numeric command offset zero, which is exactly what CLSID_OpenWithMenu
    // reports on the affected Windows 10 build.
    if (IS_INTRESOURCE(info->lpVerb)) {
        const UINT_PTR offset = LOWORD(reinterpret_cast<ULONG_PTR>(info->lpVerb));
        if ((knownOffset >= 0 && offset == static_cast<UINT_PTR>(knownOffset)) ||
            CanonicalVerbIsOpenAs(self, offset)) {
            return true;
        }
    } else if (info->lpVerb && !_stricmp(info->lpVerb, "openas")) {
        return true;
    }

    if (info->cbSize >= sizeof(CMINVOKECOMMANDINFOEX) &&
        (info->fMask & CMIC_MASK_UNICODE)) {
        auto extended =
            reinterpret_cast<const CMINVOKECOMMANDINFOEX*>(info);
        if (extended->lpVerbW && !IS_INTRESOURCE(extended->lpVerbW) &&
            !_wcsicmp(extended->lpVerbW, L"openas")) {
            return true;
        }
    }
    return false;
}

static HRESULT STDMETHODCALLTYPE OpenWithMenuInvokeCommandHook(
    IContextMenu* self, LPCMINVOKECOMMANDINFO info) {
    try {
        std::wstring path;
        int openAsOffset = -1;
        void* identity = ComIdentity(static_cast<IUnknown*>(self));
        {
            std::lock_guard<std::mutex> lock(g_openWithMenuStateMutex);
            auto found = g_openWithMenuStates.find(identity);
            if (found != g_openWithMenuStates.end()) {
                path = found->second.path;
                openAsOffset = found->second.openAsOffset;
            }
        }

        const bool openAs = InvocationUsesOpenAs(self, info, openAsOffset);
        const bool numericVerb = info && IS_INTRESOURCE(info->lpVerb);
        const UINT_PTR invokedOffset = numericVerb
            ? LOWORD(reinterpret_cast<ULONG_PTR>(info->lpVerb))
            : static_cast<UINT_PTR>(-1);
        Wh_Log(L"Standalone Open With: IContextMenu::InvokeCommand "
               L"openas=%d hasPath=%d numeric=%d offset=%llu known=%d",
               openAs, path.empty() ? 0 : 1, numericVerb,
               static_cast<unsigned long long>(invokedOffset), openAsOffset);
        if (openAs && !path.empty() &&
            QueuePicker(info ? info->hwnd : nullptr, path.c_str())) {
            std::lock_guard<std::mutex> lock(g_openWithMenuStateMutex);
            g_openWithMenuStates.erase(identity);
            return S_OK;
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: InvokeCommand hook exception; "
               L"falling back to Windows");
    }

    return OpenWithMenuInvokeCommandOriginal
               ? OpenWithMenuInvokeCommandOriginal(self, info)
               : E_FAIL;
}

template <typename Interface>
static void* InterfaceMethod(Interface* object, size_t index) {
    return object ? (*reinterpret_cast<void***>(object))[index] : nullptr;
}

static bool InstallOpenWithMenuMethodHooks() {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool uninitialize = SUCCEEDED(comResult);
    if (FAILED(comResult) && comResult != RPC_E_CHANGED_MODE) {
        Wh_Log(L"Standalone Open With: COM probe initialization failed "
               L"(0x%08X)", static_cast<unsigned int>(comResult));
        return false;
    }

    bool result = false;
    try {
        ComPtr<IUnknown> unknown;
        HRESULT hr = CoCreateInstance(
            kClsidOpenWithMenu, nullptr, CLSCTX_INPROC_SERVER, IID_IUnknown,
            reinterpret_cast<void**>(unknown.Put()));
        ComPtr<IContextMenu> contextMenu;
        ComPtr<IShellExtInit> shellExtInit;
        if (SUCCEEDED(hr) && unknown) {
            unknown->QueryInterface(IID_IContextMenu,
                                    reinterpret_cast<void**>(contextMenu.Put()));
            unknown->QueryInterface(IID_IShellExtInit,
                                    reinterpret_cast<void**>(shellExtInit.Put()));
        }

        if (contextMenu && shellExtInit) {
            // InterfaceMethod dereferences the object's raw vtable at a
            // fixed slot index. If the installed CLSID_OpenWithMenu
            // implementation ever changes its vtable layout, these reads
            // (or the SetFunctionHook calls below, which patch at that
            // address) could fault; contain that in this probe instead of
            // letting it take down the host process.
            auto initialize =
                reinterpret_cast<OpenWithMenuInitialize_t>(
                    InterfaceMethod(shellExtInit.Get(), 3));
            auto query =
                reinterpret_cast<OpenWithMenuQueryContextMenu_t>(
                    InterfaceMethod(contextMenu.Get(), 3));
            auto invoke =
                reinterpret_cast<OpenWithMenuInvokeCommand_t>(
                    InterfaceMethod(contextMenu.Get(), 4));

            bool initHook = initialize && WindhawkUtils::SetFunctionHook(
                initialize, OpenWithMenuInitializeHook,
                &OpenWithMenuInitializeOriginal);
            bool queryHook = query && WindhawkUtils::SetFunctionHook(
                query, OpenWithMenuQueryContextMenuHook,
                &OpenWithMenuQueryContextMenuOriginal);
            bool invokeHook = invoke && WindhawkUtils::SetFunctionHook(
                invoke, OpenWithMenuInvokeCommandHook,
                &OpenWithMenuInvokeCommandOriginal);
            result = initHook && invokeHook;
            Wh_Log(L"Standalone Open With: CLSID_OpenWithMenu vtable probe "
                   L"init=%p query=%p invoke=%p hooks=%d/%d/%d",
                   reinterpret_cast<void*>(initialize),
                   reinterpret_cast<void*>(query),
                   reinterpret_cast<void*>(invoke), initHook, queryHook,
                   invokeHook);
        } else {
            Wh_Log(L"Standalone Open With: CLSID_OpenWithMenu probe failed "
                   L"hr=0x%08X context=%d init=%d",
                   static_cast<unsigned int>(hr), contextMenu ? 1 : 0,
                   shellExtInit ? 1 : 0);
        }
    } catch (...) {
        result = false;
        Wh_Log(L"Standalone Open With: CLSID_OpenWithMenu vtable probe "
               L"threw, hooks not installed");
    }

    if (uninitialize) CoUninitialize();
    return result;
}

// -----------------------------------------------------------------------------
// OpenWith.exe COM-server method detours.
//
// Unknown-file double click and Properties -> Change arrive at the local COM
// server without crossing an exported Shell function in Explorer. Hook the real
// class factory and the real object's methods; never return a mod-owned COM
// object, so no external reference can outlive the mod image.
// -----------------------------------------------------------------------------

static constexpr DWORD kImmersiveOpenWithDoNotExec = 0x00000004;

static bool ClassNameEquals(HWND window, PCWSTR expected) {
    if (!window || !expected) return false;
    wchar_t className[128] = {};
    return GetClassNameW(window, className, ARRAYSIZE(className)) &&
           !_wcsicmp(className, expected);
}

static bool IsFilePropertiesOwner(HWND owner) {
    if (!owner) return false;
    HWND root = GetAncestor(owner, GA_ROOT);
    if (!root) root = owner;
    if (!ClassNameEquals(root, L"#32770")) return false;
    return FindWindowExW(root, nullptr, WC_TABCONTROLW, nullptr) != nullptr;
}

static bool ShouldSetDefaultOnly(HWND owner, DWORD flags) {
    const bool properties =
        (flags & kImmersiveOpenWithDoNotExec) && IsFilePropertiesOwner(owner);
    Wh_Log(L"Standalone Open With: launcher intent flags=0x%08X owner=%p "
           L"setDefaultOnly=%d", flags, owner, properties);
    return properties;
}

MIDL_INTERFACE("6A283FE2-ECFA-4599-91C4-E80957137B26")
StandaloneOpenWithLauncher : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE Launch(HWND owner, PCWSTR path,
                                             DWORD flags) = 0;
};

static const CLSID kClsidExecuteUnknown = {
    0xE44E9428,
    0xBDBC,
    0x4987,
    {0xA0, 0x99, 0x40, 0xDC, 0x8F, 0xD2, 0x55, 0xE7}};
static const IID kIidOpenWithLauncher = {
    0x6A283FE2,
    0xECFA,
    0x4599,
    {0x91, 0xC4, 0xE8, 0x09, 0x57, 0x13, 0x7B, 0x26}};

struct ServerOpenWithState {
    std::wstring selectionPath;
    std::wstring parameterPath;
    HWND owner = nullptr;
};

static std::mutex g_serverHookMutex;
static std::mutex g_serverStateMutex;
static std::unordered_map<void*, ServerOpenWithState> g_serverStates;
static bool g_serverFactoryHookInstalled = false;

using CoRegisterClassObject_t = HRESULT(WINAPI*)(
    REFCLSID, IUnknown*, DWORD, DWORD, LPDWORD);
using ServerFactoryCreateInstance_t = HRESULT(STDMETHODCALLTYPE*)(
    IClassFactory*, IUnknown*, REFIID, void**);
using ServerLauncherLaunch_t = HRESULT(STDMETHODCALLTYPE*)(
    StandaloneOpenWithLauncher*, HWND, PCWSTR, DWORD);
using ServerExecute_t = HRESULT(STDMETHODCALLTYPE*)(IExecuteCommand*);
using ServerSetParameters_t = HRESULT(STDMETHODCALLTYPE*)(
    IExecuteCommand*, PCWSTR);
using ServerSetSelection_t = HRESULT(STDMETHODCALLTYPE*)(
    IObjectWithSelection*, IShellItemArray*);
using ServerSetSite_t = HRESULT(STDMETHODCALLTYPE*)(IObjectWithSite*, IUnknown*);

static CoRegisterClassObject_t CoRegisterClassObjectOriginal = nullptr;
static ServerFactoryCreateInstance_t ServerFactoryCreateInstanceOriginal =
    nullptr;
static ServerLauncherLaunch_t ServerLauncherLaunchOriginal = nullptr;
static ServerExecute_t ServerExecuteOriginal = nullptr;
static ServerSetParameters_t ServerSetParametersOriginal = nullptr;
static ServerSetSelection_t ServerSetSelectionOriginal = nullptr;
static ServerSetSite_t ServerSetSiteOriginal = nullptr;

static std::wstring PathFromShellItemArray(IShellItemArray* selection) {
    if (!selection) return {};
    DWORD count = 0;
    if (FAILED(selection->GetCount(&count)) || count != 1) return {};
    ComPtr<IShellItem> item;
    if (FAILED(selection->GetItemAt(0, item.Put())) || !item) return {};
    PWSTR raw = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &raw)) || !raw)
        return {};
    return TakeTaskString(raw);
}

static std::wstring PathFromExecuteParameters(PCWSTR parameters) {
    if (!parameters || !*parameters) return {};
    try {
        std::wstring candidate = parameters;
        const size_t first = candidate.find_first_not_of(L" \t\r\n");
        if (first == std::wstring::npos) return {};
        const size_t last = candidate.find_last_not_of(L" \t\r\n");
        candidate = candidate.substr(first, last - first + 1);
        if (candidate.size() >= 2 && candidate.front() == L'"' &&
            candidate.back() == L'"') {
            candidate = candidate.substr(1, candidate.size() - 2);
        }
        if (IsSupportedFile(candidate)) return candidate;

        ArgvOwner argv(parameters);
        if (!argv) return {};
        std::wstring result;
        if (argv.Count() == 1 && IsSupportedFile(argv[0])) result = argv[0];
        return result;
    } catch (...) {
        return {};
    }
}

static HWND WindowFromSite(IUnknown* site) {
    if (!site) return nullptr;
    ComPtr<IOleWindow> oleWindow;
    if (FAILED(site->QueryInterface(
            IID_IOleWindow,
            reinterpret_cast<void**>(oleWindow.Put()))) || !oleWindow) {
        return nullptr;
    }
    HWND window = nullptr;
    return SUCCEEDED(oleWindow->GetWindow(&window)) ? window : nullptr;
}

template <typename Interface>
static void* ServerIdentity(Interface* object) {
    return object ? ComIdentity(static_cast<IUnknown*>(object)) : nullptr;
}

static void UpdateServerSelectionPath(void* identity, std::wstring path) {
    if (!identity) return;
    std::lock_guard<std::mutex> lock(g_serverStateMutex);
    if (g_serverStates.size() > 256) g_serverStates.clear();
    g_serverStates[identity].selectionPath =
        IsSupportedFile(path) ? std::move(path) : std::wstring{};
}

static void UpdateServerParameterPath(void* identity, std::wstring path) {
    if (!identity) return;
    std::lock_guard<std::mutex> lock(g_serverStateMutex);
    if (g_serverStates.size() > 256) g_serverStates.clear();
    g_serverStates[identity].parameterPath =
        IsSupportedFile(path) ? std::move(path) : std::wstring{};
}

static void UpdateServerOwner(void* identity, HWND owner) {
    if (!identity) return;
    std::lock_guard<std::mutex> lock(g_serverStateMutex);
    if (g_serverStates.size() > 256) g_serverStates.clear();
    g_serverStates[identity].owner = owner;
}

static ServerOpenWithState TakeServerState(void* identity) {
    ServerOpenWithState state;
    if (!identity) return state;
    std::lock_guard<std::mutex> lock(g_serverStateMutex);
    auto found = g_serverStates.find(identity);
    if (found != g_serverStates.end()) {
        state = std::move(found->second);
        g_serverStates.erase(found);
    }
    return state;
}

static HRESULT STDMETHODCALLTYPE ServerSetSelectionHook(
    IObjectWithSelection* self, IShellItemArray* selection) {
    const HRESULT hr = ServerSetSelectionOriginal
                           ? ServerSetSelectionOriginal(self, selection)
                           : E_FAIL;
    if (SUCCEEDED(hr)) {
        try {
            UpdateServerSelectionPath(ServerIdentity(self),
                                      PathFromShellItemArray(selection));
        } catch (...) {
        }
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE ServerSetParametersHook(
    IExecuteCommand* self, PCWSTR parameters) {
    const HRESULT hr = ServerSetParametersOriginal
                           ? ServerSetParametersOriginal(self, parameters)
                           : E_FAIL;
    if (SUCCEEDED(hr)) {
        try {
            UpdateServerParameterPath(
                ServerIdentity(self), PathFromExecuteParameters(parameters));
        } catch (...) {
        }
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE ServerSetSiteHook(IObjectWithSite* self,
                                                    IUnknown* site) {
    const HRESULT hr = ServerSetSiteOriginal
                           ? ServerSetSiteOriginal(self, site)
                           : E_FAIL;
    if (SUCCEEDED(hr)) {
        try {
            UpdateServerOwner(ServerIdentity(self), WindowFromSite(site));
        } catch (...) {
        }
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE ServerExecuteHook(IExecuteCommand* self) {
    try {
        ServerOpenWithState state = TakeServerState(ServerIdentity(self));
        if (!state.owner) state.owner = GetForegroundWindow();
        const std::wstring& path = IsSupportedFile(state.selectionPath)
                                       ? state.selectionPath
                                       : state.parameterPath;
        Wh_Log(L"Standalone Open With: server Execute path=%s owner=%p",
               path.empty() ? L"(empty)" : path.c_str(), state.owner);
        // IExecuteCommand::Execute has no "user cancelled" channel - the
        // real server simply exits either way - so both Accepted and
        // Cancelled count as handled and only NotHandled falls through.
        if (g_replaceSystemDialog.load(std::memory_order_acquire) &&
            IsSupportedFile(path) &&
            QueuePickerAndWait(state.owner, path.c_str()) !=
                PickerOutcome::NotHandled) {
            return S_OK;
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: server Execute hook exception");
    }
    return ServerExecuteOriginal ? ServerExecuteOriginal(self) : E_FAIL;
}

static HRESULT STDMETHODCALLTYPE ServerLauncherLaunchHook(
    StandaloneOpenWithLauncher* self, HWND owner, PCWSTR path, DWORD flags) {
    try {
        const bool setDefaultOnly = ShouldSetDefaultOnly(owner, flags);
        Wh_Log(L"Standalone Open With: server Launch path=%s owner=%p "
               L"flags=0x%08X", path ? path : L"(null)", owner, flags);
        if (g_replaceSystemDialog.load(std::memory_order_acquire) && path &&
            IsSupportedFile(path) &&
            QueuePickerAndWait(owner, path, setDefaultOnly) !=
                PickerOutcome::NotHandled) {
            return S_OK;
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: server Launch hook exception");
    }
    return ServerLauncherLaunchOriginal
               ? ServerLauncherLaunchOriginal(self, owner, path, flags)
               : E_FAIL;
}

static bool InstallServerObjectMethodHooks(IUnknown* object) {
    if (!object || g_shuttingDown.load(std::memory_order_acquire)) return false;

    ComPtr<StandaloneOpenWithLauncher> launcher;
    object->QueryInterface(kIidOpenWithLauncher,
                           reinterpret_cast<void**>(launcher.Put()));
    ComPtr<IExecuteCommand> execute;
    object->QueryInterface(IID_IExecuteCommand,
                           reinterpret_cast<void**>(execute.Put()));
    ComPtr<IObjectWithSelection> selection;
    object->QueryInterface(IID_IObjectWithSelection,
                           reinterpret_cast<void**>(selection.Put()));
    ComPtr<IObjectWithSite> site;
    object->QueryInterface(IID_IObjectWithSite,
                           reinterpret_cast<void**>(site.Put()));

    std::lock_guard<std::mutex> lock(g_serverHookMutex);
    bool registered = false;
    if (launcher && !ServerLauncherLaunchOriginal) {
        auto method = reinterpret_cast<ServerLauncherLaunch_t>(
            InterfaceMethod(launcher.Get(), 3));
        registered |= method && WindhawkUtils::SetFunctionHook(
            method, ServerLauncherLaunchHook, &ServerLauncherLaunchOriginal);
    }
    if (execute) {
        if (!ServerSetParametersOriginal) {
            auto method = reinterpret_cast<ServerSetParameters_t>(
                InterfaceMethod(execute.Get(), 4));
            registered |= method && WindhawkUtils::SetFunctionHook(
                method, ServerSetParametersHook,
                &ServerSetParametersOriginal);
        }
        if (!ServerExecuteOriginal) {
            auto method = reinterpret_cast<ServerExecute_t>(
                InterfaceMethod(execute.Get(), 9));
            registered |= method && WindhawkUtils::SetFunctionHook(
                method, ServerExecuteHook, &ServerExecuteOriginal);
        }
    }
    if (selection && !ServerSetSelectionOriginal) {
        auto method = reinterpret_cast<ServerSetSelection_t>(
            InterfaceMethod(selection.Get(), 3));
        registered |= method && WindhawkUtils::SetFunctionHook(
            method, ServerSetSelectionHook, &ServerSetSelectionOriginal);
    }
    if (site && !ServerSetSiteOriginal) {
        auto method = reinterpret_cast<ServerSetSite_t>(
            InterfaceMethod(site.Get(), 3));
        registered |= method && WindhawkUtils::SetFunctionHook(
            method, ServerSetSiteHook, &ServerSetSiteOriginal);
    }

    if (registered && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Standalone Open With: failed to apply server object hooks");
        return false;
    }
    Wh_Log(L"Standalone Open With: server object methods launch=%d "
           L"parameters=%d execute=%d selection=%d site=%d",
           ServerLauncherLaunchOriginal != nullptr,
           ServerSetParametersOriginal != nullptr,
           ServerExecuteOriginal != nullptr,
           ServerSetSelectionOriginal != nullptr,
           ServerSetSiteOriginal != nullptr);
    return registered || ServerLauncherLaunchOriginal || ServerExecuteOriginal;
}

static HRESULT STDMETHODCALLTYPE ServerFactoryCreateInstanceHook(
    IClassFactory* self, IUnknown* outer, REFIID iid, void** object) {
    const HRESULT hr = ServerFactoryCreateInstanceOriginal
                           ? ServerFactoryCreateInstanceOriginal(
                                 self, outer, iid, object)
                           : E_FAIL;
    if (SUCCEEDED(hr) && !outer && object && *object) {
        try {
            InstallServerObjectMethodHooks(
                reinterpret_cast<IUnknown*>(*object));
        } catch (...) {
            Wh_Log(L"Standalone Open With: server object probe exception");
        }
    }
    return hr;
}

static bool InstallServerFactoryHook(IUnknown* classObject) {
    if (!classObject) return false;
    ComPtr<IClassFactory> factory;
    if (FAILED(classObject->QueryInterface(
            IID_IClassFactory,
            reinterpret_cast<void**>(factory.Put()))) || !factory) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_serverHookMutex);
    if (g_serverFactoryHookInstalled) return true;
    auto createInstance = reinterpret_cast<ServerFactoryCreateInstance_t>(
        InterfaceMethod(factory.Get(), 3));
    if (!createInstance || !WindhawkUtils::SetFunctionHook(
                               createInstance,
                               ServerFactoryCreateInstanceHook,
                               &ServerFactoryCreateInstanceOriginal)) {
        Wh_Log(L"Standalone Open With: failed to register server factory hook");
        return false;
    }
    if (!Wh_ApplyHookOperations()) {
        Wh_Log(L"Standalone Open With: failed to apply server factory hook");
        return false;
    }
    g_serverFactoryHookInstalled = true;
    Wh_Log(L"Standalone Open With: server class factory hooked at %p",
           reinterpret_cast<void*>(createInstance));
    return true;
}

static HRESULT WINAPI CoRegisterClassObjectHook(
    REFCLSID clsid, IUnknown* classObject, DWORD context, DWORD flags,
    LPDWORD registration) {
    try {
        if (IsEqualCLSID(clsid, kClsidExecuteUnknown)) {
            Wh_Log(L"Standalone Open With: ExecuteUnknown class registered");
            InstallServerFactoryHook(classObject);
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: class registration hook exception");
    }
    return CoRegisterClassObjectOriginal
               ? CoRegisterClassObjectOriginal(clsid, classObject, context,
                                               flags, registration)
               : E_FAIL;
}

// -----------------------------------------------------------------------------
// Direct OpenWith.exe command-line interception.
// -----------------------------------------------------------------------------

// Direct OpenWith.exe command lines (drag/drop or explicit launch) don't use
// the COM server. If a real file is present in argv, replace the executable's
// entry point and run the standalone picker instead.
using ProcessEntryPoint_t = void(WINAPI*)();
static ProcessEntryPoint_t OpenWithEntryPointOriginal = nullptr;
static std::wstring g_directOpenWithPath;

static void WINAPI OpenWithEntryPointHook() {
    Wh_Log(L"Standalone Open With: intercepted direct OpenWith.exe entry "
           L"path=%s", g_directOpenWithPath.c_str());
    bool handled = false;
    if (IsSupportedFile(g_directOpenWithPath) &&
        g_replaceSystemDialog.load(std::memory_order_acquire)) {
        // The process was launched purely to show this dialog, so a
        // cancelled picker is still a completed run: exit rather than
        // letting the real OpenWith.exe put a second dialog on screen.
        handled = QueuePickerAndWait(nullptr, g_directOpenWithPath.c_str()) !=
                  PickerOutcome::NotHandled;
    }
    if (handled) {
        ExitProcess(0);
    } else if (OpenWithEntryPointOriginal) {
        OpenWithEntryPointOriginal();
    } else {
        ExitProcess(0);
    }
}

static bool InstallDirectOpenWithEntryHook(PCWSTR path) {
    if (!path || !IsSupportedFile(path)) return false;
    try {
        g_directOpenWithPath = path;
    } catch (...) {
        return false;
    }

    // Walks the loaded module's own PE headers to locate its entry point.
    // A malformed or unexpected image layout (e_lfanew pointing outside
    // the mapped module, an unusual header size, etc.) would otherwise
    // dereference bad memory and crash OpenWith.exe outright; treat any
    // fault here as "can't hook this build" instead of taking the process
    // down.
    try {
        HMODULE executable = GetModuleHandleW(nullptr);
        if (!executable) return false;
        auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(executable);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE) return false;
        auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
            reinterpret_cast<const BYTE*>(executable) + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE ||
            !nt->OptionalHeader.AddressOfEntryPoint) {
            return false;
        }
        auto entry = reinterpret_cast<ProcessEntryPoint_t>(
            reinterpret_cast<BYTE*>(executable) +
            nt->OptionalHeader.AddressOfEntryPoint);
        const bool hooked = WindhawkUtils::SetFunctionHook(
            entry, OpenWithEntryPointHook, &OpenWithEntryPointOriginal);
        Wh_Log(L"Standalone Open With: direct entry hook=%d entry=%p path=%s",
               hooked, reinterpret_cast<void*>(entry), path);
        return hooked;
    } catch (...) {
        Wh_Log(L"Standalone Open With: direct entry hook setup threw, "
               L"falling back to Windows");
        return false;
    }
}

// Extracts the file path argument from an "OpenWith.exe -c <path>"-style
// command line. Returns an empty string if nothing usable is found.
static std::wstring ExtractOpenWithTargetPath(LPCWSTR commandLine) {
    if (!commandLine || !*commandLine) return L"";
    ArgvOwner argv(commandLine);
    if (!argv) return L"";
    const int argc = argv.Count();
    std::wstring result;
    try {
        for (int i = 0; i < argc; ++i) {
            if (!_wcsicmp(argv[i], L"-c") && i + 1 < argc) {
                result = argv[i + 1];
                break;
            }
        }
        if (result.empty() && argc > 1) {
            // Fall back to the last argument if no explicit -c flag is found.
            result = argv[argc - 1];
        }
    } catch (...) {
        result.clear();
    }
    return result;
}

using SHOpenWithDialog_t = HRESULT(WINAPI*)(HWND, const OPENASINFO*);
using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR,
                                           LPCWSTR, INT);
static SHOpenWithDialog_t SHOpenWithDialogOriginal = nullptr;
static ShellExecuteExW_t ShellExecuteExWOriginal = nullptr;
static ShellExecuteW_t ShellExecuteWOriginal = nullptr;

static bool IsOpenAsVerb(PCWSTR verb) {
    return verb && !_wcsicmp(verb, L"openas");
}

static HRESULT WINAPI SHOpenWithDialogHook(HWND owner, const OPENASINFO* info) {
    try {
        if (info && (info->oaifInFlags & OAIF_EXEC) &&
            g_replaceSystemDialog.load(std::memory_order_acquire) &&
            info->pcszFile && IsSupportedFile(info->pcszFile)) {
            // Windows returns HRESULT_FROM_WIN32(ERROR_CANCELLED) when the
            // user dismisses the dialog. Callers branch on it - to retry,
            // to fall back to another handler, or to delete a temp file
            // they only keep when a program was actually chosen - so the
            // Cancel path must not be reported as S_OK.
            switch (QueuePickerAndWait(owner, info->pcszFile)) {
                case PickerOutcome::Accepted:
                    return S_OK;
                case PickerOutcome::Cancelled:
                    return HRESULT_FROM_WIN32(ERROR_CANCELLED);
                case PickerOutcome::NotHandled:
                    break;
            }
        }
        Wh_Log(L"Standalone Open With: SHOpenWithDialog falling back to system "
               L"dialog (hasInfo=%d flags=0x%X file=%s replace=%d workerReady=%d "
               L"shuttingDown=%d)",
               info ? 1 : 0, info ? info->oaifInFlags : 0,
               info && info->pcszFile ? info->pcszFile : L"(null)",
               g_replaceSystemDialog.load(std::memory_order_acquire),
               g_workerReady.load(std::memory_order_acquire),
               g_shuttingDown.load(std::memory_order_acquire));
    } catch (...) {}
    return SHOpenWithDialogOriginal ? SHOpenWithDialogOriginal(owner, info) : E_FAIL;
}

static BOOL WINAPI ShellExecuteExWHook(SHELLEXECUTEINFOW* info) {
    try {
        if (info && IsOpenAsVerb(info->lpVerb) &&
            g_replaceSystemDialog.load(std::memory_order_acquire) &&
            info->lpFile && IsSupportedFile(info->lpFile)) {
            // Same contract as above: Windows fails the call with
            // ERROR_CANCELLED when the picker is dismissed, so returning
            // TRUE for a cancel would tell the caller a program was
            // launched when none was.
            switch (QueuePickerAndWait(info->hwnd, info->lpFile)) {
                case PickerOutcome::Accepted:
                    info->hInstApp = reinterpret_cast<HINSTANCE>(33);
                    info->hProcess = nullptr;
                    return TRUE;
                case PickerOutcome::Cancelled:
                    info->hInstApp =
                        reinterpret_cast<HINSTANCE>(SE_ERR_NOASSOC);
                    info->hProcess = nullptr;
                    SetLastError(ERROR_CANCELLED);
                    return FALSE;
                case PickerOutcome::NotHandled:
                    break;
            }
        }
        if (info && IsOpenAsVerb(info->lpVerb)) {
            Wh_Log(L"Standalone Open With: ShellExecuteExW(openas) falling back "
                   L"to system dialog (file=%s replace=%d workerReady=%d "
                   L"shuttingDown=%d)",
                   info->lpFile ? info->lpFile : L"(null)",
                   g_replaceSystemDialog.load(std::memory_order_acquire),
                   g_workerReady.load(std::memory_order_acquire),
                   g_shuttingDown.load(std::memory_order_acquire));
        }
    } catch (...) {}
    return ShellExecuteExWOriginal ? ShellExecuteExWOriginal(info) : FALSE;
}

static HINSTANCE WINAPI ShellExecuteWHook(HWND owner, LPCWSTR verb, LPCWSTR file,
                                          LPCWSTR parameters, LPCWSTR directory,
                                          INT show) {
    try {
        // Waiting form, like the two sibling hooks: ShellExecuteW is
        // synchronous for "openas" on Windows, so returning as soon as the
        // request was queued told the caller the dialog was done while it
        // was still on screen.
        if (IsOpenAsVerb(verb) &&
            g_replaceSystemDialog.load(std::memory_order_acquire) && file &&
            IsSupportedFile(file)) {
            switch (QueuePickerAndWait(owner, file)) {
                case PickerOutcome::Accepted:
                    return reinterpret_cast<HINSTANCE>(33);
                case PickerOutcome::Cancelled:
                    // ShellExecuteW reports failure through its return
                    // value; the matching error code goes to GetLastError.
                    SetLastError(ERROR_CANCELLED);
                    return reinterpret_cast<HINSTANCE>(SE_ERR_NOASSOC);
                case PickerOutcome::NotHandled:
                    break;
            }
        }
        if (IsOpenAsVerb(verb)) {
            Wh_Log(L"Standalone Open With: ShellExecuteW(openas) falling back "
                   L"to system dialog (file=%s replace=%d workerReady=%d "
                   L"shuttingDown=%d)",
                   file ? file : L"(null)",
                   g_replaceSystemDialog.load(std::memory_order_acquire),
                   g_workerReady.load(std::memory_order_acquire),
                   g_shuttingDown.load(std::memory_order_acquire));
        }
    } catch (...) {}
    return ShellExecuteWOriginal
        ? ShellExecuteWOriginal(owner, verb, file, parameters, directory, show)
        : reinterpret_cast<HINSTANCE>(SE_ERR_ACCESSDENIED);
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle.
// -----------------------------------------------------------------------------

static std::atomic<bool> g_isExplorerProcess{false};

static void StopWorker() {
    g_shuttingDown.store(true, std::memory_order_release);
    if (g_stopEvent) SetEvent(g_stopEvent.get());
    if (HWND browse = g_activeBrowseHwnd.load(std::memory_order_acquire))
        PostMessageW(browse, WM_CLOSE, 0, 0);
    if (HWND window = g_currentWindow.load(std::memory_order_acquire))
        PostMessageW(window, WM_CLOSE, 0, 0);
    {
        std::lock_guard<std::mutex> lock(g_workerStartMutex);
        if (g_worker) {
            if (g_worker->joinable()) g_worker->join();
            g_worker.reset();
        }
        g_workerExited.store(false, std::memory_order_release);
    }
    g_activeBrowseHwnd.store(nullptr, std::memory_order_release);
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t LoadLibraryExWOriginal = nullptr;
static std::mutex g_shell32HookMutex;

// @include rundll32.exe matches EVERY rundll32 invocation on the system:
// Control Panel applets, printer and device UI, shell32.dll,Control_RunDLL
// and dozens more. Only shell32.dll,OpenAs_RunDLL can ever show an Open
// With picker, and it needs nothing but SHOpenWithDialog. So hosts that
// aren't Explorer or OpenWith.exe take the single narrow hook and skip
// ShellExecuteW / ShellExecuteExW, two hot general-purpose APIs that would
// otherwise be detoured in processes that can never show the dialog.
static std::atomic<bool> g_hookGeneralShellExecute{false};

static bool HookShell32Exports(HMODULE shell32) {
    if (!shell32 || g_shuttingDown.load(std::memory_order_acquire))
        return false;

    std::lock_guard<std::mutex> lock(g_shell32HookMutex);
    bool registered = false;
    auto openWith = reinterpret_cast<SHOpenWithDialog_t>(
        GetProcAddress(shell32, "SHOpenWithDialog"));
    if (openWith && !SHOpenWithDialogOriginal) {
        registered |= WindhawkUtils::SetFunctionHook(
            openWith, SHOpenWithDialogHook, &SHOpenWithDialogOriginal);
    }
    if (!g_hookGeneralShellExecute.load(std::memory_order_acquire))
        return registered;

    auto executeEx = reinterpret_cast<ShellExecuteExW_t>(
        GetProcAddress(shell32, "ShellExecuteExW"));
    auto execute = reinterpret_cast<ShellExecuteW_t>(
        GetProcAddress(shell32, "ShellExecuteW"));
    if (executeEx && !ShellExecuteExWOriginal) {
        registered |= WindhawkUtils::SetFunctionHook(
            executeEx, ShellExecuteExWHook, &ShellExecuteExWOriginal);
    }
    if (execute && !ShellExecuteWOriginal) {
        registered |= WindhawkUtils::SetFunctionHook(
            execute, ShellExecuteWHook, &ShellExecuteWOriginal);
    }
    return registered;
}

static HMODULE WINAPI LoadLibraryExWHook(LPCWSTR name, HANDLE file,
                                         DWORD flags) {
    HMODULE result = LoadLibraryExWOriginal
                         ? LoadLibraryExWOriginal(name, file, flags)
                         : nullptr;
    constexpr DWORD kDataOnly =
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
        LOAD_LIBRARY_AS_IMAGE_RESOURCE;
    if (result && name && !(flags & kDataOnly) &&
        !_wcsicmp(PathFindFileNameW(name), L"shell32.dll") &&
        HookShell32Exports(result) && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Standalone Open With: failed to apply late shell32 hooks");
    }
    return result;
}

static bool VerifySystemBinariesExist() {
    try {
        wchar_t winDir[MAX_PATH] = {};
        if (!GetWindowsDirectoryW(winDir, ARRAYSIZE(winDir))) return false;

        wchar_t sysDir[MAX_PATH] = {};
        if (!GetSystemDirectoryW(sysDir, ARRAYSIZE(sysDir))) return false;

        wchar_t explorerPath[MAX_PATH] = {};
        if (swprintf_s(explorerPath, L"%s\\explorer.exe", winDir) <= 0)
            return false;

        wchar_t openWithPath[MAX_PATH] = {};
        if (swprintf_s(openWithPath, L"%s\\OpenWith.exe", sysDir) <= 0)
            return false;

        const DWORD explorerAttrs = GetFileAttributesW(explorerPath);
        if (explorerAttrs == INVALID_FILE_ATTRIBUTES ||
            (explorerAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
            Wh_Log(L"Standalone Open With: explorer.exe verification failed "
                   L"at %s", explorerPath);
            return false;
        }

        const DWORD openWithAttrs = GetFileAttributesW(openWithPath);
        if (openWithAttrs == INVALID_FILE_ATTRIBUTES ||
            (openWithAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
            Wh_Log(L"Standalone Open With: OpenWith.exe verification failed "
                   L"at %s", openWithPath);
            return false;
        }

        return true;
    } catch (...) {
        return false;
    }
}

// @include rundll32.exe exists only for the legacy rundll32.exe
// shell32.dll,OpenAs_RunDLL entry point: OpenAs_RunDLL calls
// SHOpenWithDialog inside shell32.dll, so the SHOpenWithDialog export
// hook installed by HookShell32Exports (including late loads via the
// LoadLibraryExW hook) covers it without any rundll32-specific code.
// Because that include also matches every unrelated rundll32 host,
// those processes install SHOpenWithDialog and nothing else: the
// ShellExecuteW/ShellExecuteExW detours are gated on Explorer or
// OpenWith.exe (g_hookGeneralShellExecute), and the LoadLibraryExW
// detour is skipped whenever shell32 is already loaded, which is the
// normal case in rundll32.exe since it imports shell32 statically.
// With @architecture x86-64 only the 64-bit rundll32 is covered: a
// 64-bit caller's OpenAs_RunDLL uses the 64-bit rundll32, so that route
// works; 32-bit callers fall outside this mod's architecture scope.
BOOL Wh_ModInit() {
    try {
        if (!VerifySystemBinariesExist()) {
            Wh_Log(L"Standalone Open With: required system binaries not found, "
                   L"aborting initialization");
            return FALSE;
        }

        wchar_t moduleName[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, moduleName, ARRAYSIZE(moduleName));
        const PCWSTR processFileName = PathFindFileNameW(moduleName);
        const bool isExplorer = processFileName &&
            !_wcsicmp(processFileName, L"explorer.exe");
        const bool isOpenWith = processFileName &&
            !_wcsicmp(processFileName, L"OpenWith.exe");
        g_isExplorerProcess.store(isExplorer, std::memory_order_release);

        std::wstring directOpenWithPath;
        if (isOpenWith) {
            directOpenWithPath =
                ExtractOpenWithTargetPath(GetCommandLineW());
        }

        LoadSettings();
        g_shuttingDown.store(false, std::memory_order_release);
        g_stopEvent.reset(CreateEventW(nullptr, TRUE, FALSE, nullptr));
        g_requestEvent.reset(CreateEventW(nullptr, FALSE, FALSE, nullptr));
        g_workerReadyEvent.reset(CreateEventW(nullptr, TRUE, FALSE, nullptr));
        if (!g_stopEvent || !g_requestEvent || !g_workerReadyEvent)
            return FALSE;
        // The picker worker thread is started lazily on the first
        // request (see StartWorkerIfNeeded below).

        // ShellExecuteW / ShellExecuteExW are only worth detouring where
        // an Open With picker can actually originate. Explorer raises the
        // context-menu "openas" verb and OpenWith.exe is the dialog host;
        // every other @include rundll32.exe host (Control Panel applets,
        // printer and device UI, Control_RunDLL, ...) gets SHOpenWithDialog
        // alone, which is all shell32.dll,OpenAs_RunDLL needs.
        g_hookGeneralShellExecute.store(isExplorer || isOpenWith,
                                        std::memory_order_release);

        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");

        // The LoadLibraryExW detour exists only to catch a shell32 that
        // gets loaded later. When shell32 is already in the process -
        // which is the normal case in rundll32.exe, since it imports
        // shell32 statically, and in Explorer - there is nothing left to
        // catch, so don't detour a hot loader API for nothing.
        bool hookedLoadLibrary = false;
        if (!shell32) {
            HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
            if (!kernelbase) kernelbase = GetModuleHandleW(L"kernel32.dll");
            if (kernelbase) {
                auto loadLibraryExW = reinterpret_cast<LoadLibraryExW_t>(
                    GetProcAddress(kernelbase, "LoadLibraryExW"));
                if (loadLibraryExW) {
                    hookedLoadLibrary = WindhawkUtils::SetFunctionHook(
                        loadLibraryExW, LoadLibraryExWHook,
                        &LoadLibraryExWOriginal);
                }
            }
        }

        const bool hookedShellExports = HookShell32Exports(shell32);

        bool hookedServerRegistration = false;
        bool hookedDirectEntry = false;
        if (isOpenWith) {
            HMODULE combase = GetModuleHandleW(L"combase.dll");
            auto registerClassObject = combase
                ? reinterpret_cast<CoRegisterClassObject_t>(
                      GetProcAddress(combase, "CoRegisterClassObject"))
                : nullptr;
            if (!registerClassObject) {
                HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
                registerClassObject = ole32
                    ? reinterpret_cast<CoRegisterClassObject_t>(
                          GetProcAddress(ole32, "CoRegisterClassObject"))
                    : nullptr;
            }
            if (registerClassObject) {
                hookedServerRegistration = WindhawkUtils::SetFunctionHook(
                    registerClassObject, CoRegisterClassObjectHook,
                    &CoRegisterClassObjectOriginal);
            }
            if (IsSupportedFile(directOpenWithPath)) {
                hookedDirectEntry = InstallDirectOpenWithEntryHook(
                    directOpenWithPath.c_str());
            }
        }

        // The stable context-menu vtable probe is intentionally deferred to
        // Wh_ModAfterInit so Explorer's main thread is not forced to activate a
        // shell extension during process initialization.
        const bool anyHook = hookedLoadLibrary || hookedShellExports ||
                             hookedServerRegistration || hookedDirectEntry ||
                             isExplorer;
        Wh_Log(L"Standalone Open With: init process=%s pid=%u shell32=%p "
               L"loadLibrary=%d shellExports=%d generalShellExecute=%d "
               L"serverRegistration=%d directEntry=%d deferredMenuProbe=%d",
               moduleName, GetCurrentProcessId(), shell32,
               hookedLoadLibrary, hookedShellExports,
               g_hookGeneralShellExecute.load(std::memory_order_acquire),
               hookedServerRegistration, hookedDirectEntry,
               isExplorer);
        if (!anyHook) {
            StopWorker();
            return FALSE;
        }
        return TRUE;
    } catch (...) {
        StopWorker();
        return FALSE;
    }
}

void Wh_ModAfterInit() {
    if (!g_isExplorerProcess.load(std::memory_order_acquire) ||
        g_shuttingDown.load(std::memory_order_acquire)) {
        return;
    }
    try {
        const bool hooked = InstallOpenWithMenuMethodHooks();
        if (!Wh_ApplyHookOperations()) {
            Wh_Log(L"Standalone Open With: failed to apply deferred "
                   L"CLSID_OpenWithMenu hooks");
        } else if (!hooked) {
            Wh_Log(L"Standalone Open With: deferred context-menu hooks "
                   L"weren't available");
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: deferred context-menu probe failed");
    }
}

void Wh_ModSettingsChanged() {
    try {
        LoadSettings();
        if (HWND window = g_currentWindow.load(std::memory_order_acquire)) {
            PostMessageW(window, WM_SOW_SETTINGS_CHANGED, 0, 0);
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: settings reload failed");
    }
}

void Wh_ModUninit() {
    StopWorker();
    // Last resort: if the worker was torn down while a dark picker still
    // held AllowDark, put the host process's preferred app mode back. The
    // mod must not leave Explorer's theme state changed behind it.
    DarkModeActivation::RestoreAppMode();
    {
        std::lock_guard<std::mutex> lock(g_requestMutex);
        g_pendingRequest.reset();
    }
    {
        std::lock_guard<std::mutex> lock(g_openWithMenuStateMutex);
        g_openWithMenuStates.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_serverStateMutex);
        g_serverStates.clear();
    }
    g_workerReadyEvent.reset();
    g_requestEvent.reset();
    g_stopEvent.reset();
}
