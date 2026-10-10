// ==WindhawkMod==
// @id              magic-pointer
// @name            Magic Pointer
// @description     Shake the mouse cursor to summon a Googlebook-style Magic Pointer, then point at or select anything on screen and ask Gemini AI about it
// @version         1.0.1
// @author          Rono
// @github          https://github.com/rono-zeroseven
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -ladvapi32 -lole32 -loleaut32 -lwinhttp -limm32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Magic Pointer

The Magic Pointer from Google's Googlebooks, on Windows: an AI mouse cursor.
Wiggle the mouse, then point at anything on screen, select it, and ask Gemini
about it, search it with Google Lens, copy its text or translate it.

![Magic Pointer with a sample selection and question](https://raw.githubusercontent.com/rono-zeroseven/windhawk-mods/0a3b052e38da866ce5312754be9bec0e5364caea/assets/magic-pointer/preview.png)

*Native preview with a sample selection; browser actions work without an API key.*

## How to use

1. **Shake the mouse**, or swipe back and forth on the touchpad. The keyboard
   shortcut (**Win+Shift+G** by default) works too.
2. **Point and select.** Hover over a button, image, link or paragraph to
   outline it, then click to select it. Or click and drag to select any area.
3. **Ask.** A card opens next to your selection. Type a question and press
   Enter, or pick a suggestion. With a Gemini API key, the **Translate** button
   on the card translates the text in your selection into your Windows
   language, or into the language you pick in the settings.

Press **Esc** or **right-click** to put the Magic Pointer away. It also leaves
on its own when the mouse stays still for a while.

## Gemini

Add a free Gemini API key (https://aistudio.google.com/apikey) in the
settings and answers stream right into the card. You can ask follow-up
questions. Enable **Smart suggestions** to have Gemini suggest up to three
things to do with each selection; this is off by default.
Drag over an answer to select text, then press Ctrl+C or right-click to copy.
The **Copy** button copies the whole answer, including while it is streaming.


It also works without a key: **Send** (or **Ask in Gemini**) opens
Gemini in your browser, types your question into its message box and pastes
your selections there as PNG files. It waits for the new Gemini page to finish
loading, never uses a Gemini tab that was already open, and never presses
Enter: review the draft and send it when ready. If something couldn't be
added, the card comes back and says what to do, usually to click Gemini's
message box and press Ctrl+V. You can also search with Google Lens or copy the
image and its recognized text.

With a key, the **Ask in Gemini** button in the card's top row (the arrow
leaving a window) carries on in Gemini's own app instead of in the card: it
types the question you have started to write, or else the last one you asked
here, and attaches every selection.

Gemini and Google Lens open in your Windows default browser, or in the one you
pick with the **Browser** setting: Chrome, Edge, Firefox, Brave, Zen, Opera,
Vivaldi, LibreWolf, Waterfox, Floorp, or any other browser through the path of
its program. The draft is only typed into a window of that browser, and only
once the browser itself reports that the page is Gemini's chat. Chrome, Edge,
Firefox and Zen have been tried; the others share their engines. Like
Chromium-based browsers, Firefox-based ones turn on their accessibility support
when it's used.

## Text recognition

Every selection is read with the OCR built into Windows 10 and 11, entirely
on your PC. Copy the text with the **Aa** button, and Gemini gets it as extra
context. OCR uses the languages installed in Windows.

## Privacy and temporary files

Summoning the pointer, selecting an area and copying its image or text stay
on your PC with the default settings. The mod contacts Google's services
when you use these features:

* With an API key, **Send**, a suggestion chip or **Translate** sends your
  selected regions as JPEG images (up to 1536 pixels on the longest side),
  their UI Automation names and recognized text, your question and the card's
  conversation to `generativelanguage.googleapis.com` over HTTPS. Follow-up
  questions include the selections and conversation again.
* **Smart suggestions** is off by default. If you enable it with an API key,
  every selection is uploaded as soon as you make or add it, even if you only
  intend to copy its text or search it with Lens. The suggestion request sends
  the selection images and text, without the card's conversation.
* **Ask in Gemini** (also **Send** without an API key) opens
  `gemini.google.com/app` and puts your question and selection PNG files into
  a new draft. Attaching files can upload them before you send the draft; the
  mod leaves sending the message to you.
* **Search with Google Lens** opens a temporary local HTML page containing
  your selection as a PNG (up to 1000 pixels on the longest side). That page
  immediately uploads the image to `lens.google.com/v3/upload` when it opens.

Under Google's [Gemini API terms](https://ai.google.dev/gemini-api/terms),
free-tier submissions and responses, including images, may be used to improve
Google's products and machine learning, and may be read by human reviewers in
most regions. The terms describe regional exceptions and different treatment
for paid services. Avoid sending sensitive, confidential or personal content
through the free tier.

Browser handoff PNGs (`gemini-selection-*.png`) and Lens HTML pages are written
temporarily to the mod's Windhawk storage folder, normally under
`%ProgramData%\Windhawk`. This folder is shared across Windows accounts.
PNGs are scheduled for deletion after 30 minutes and Lens pages after 10
minutes; both are also deleted when the mod is disabled. Files left by an
interrupted process are cleaned up on the next start once they expire. A file held open by another program may delay
deletion. **Copy image** and browser handoff also put the selection on the
Windows clipboard; clipboard history follows your Windows settings.

## Look and feel

* The card and the hint sit on Windows' live frosted glass (acrylic) with
  rounded corners, so they follow whatever is behind them, even as it changes.
  Turning off *Transparency effects* in Windows, or *Glass effect* here,
  switches them to solid colors, as does a Windows older than 10 version 1803.
* Pick any **glow color**: `#4285F4`, `66,133,244`, a name like `purple`,
  `accent` for your Windows accent color, or `rainbow` for Google's colors.
* **Dim outside the selection** darkens the surrounding screen only while you
  drag a rectangle. The area inside stays clear.


## Games and other apps

Shaking is ignored while a game, video or presentation fills the screen, and
in any app on the *Excluded apps* list (for example `RobloxPlayerBeta.exe`,
`C:\Games\*` or a window title).

## Notes

* Runs as a *tool mod* in its own background process. It doesn't inject into
  other programs and never changes your system cursors.
* Uses Windhawk's standard host without requesting UIAccess. Highlighting
  controls and browser handoff are subject to Windows' restrictions on
  interacting with elevated apps.
* Works on every Windows version Windhawk runs on, on x64, ARM64 and 32-bit
  systems, and supports multiple monitors with per-monitor display scaling.
* Shakes are detected from raw mouse input, so nothing runs while the mouse is
  idle and the mouse itself is never slowed down.
* *Highlight what you point at* reads the layout of the app under the pointer
  through UI Automation, the accessibility API. Chromium-based browsers turn
  on their accessibility support when it's used, which costs a little memory.
  Turn it off if you prefer: click and drag always works.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- shake: true
  $name: Shake to activate
  $description: Wiggle the mouse to summon the Magic Pointer
- sensitivity: 5
  $name: Shake sensitivity (1-10)
  $description: >-
    1 needs a large, fast shake. 10 activates with a small, gentle wiggle.
- hotkey: Win+Shift+G
  $name: Keyboard shortcut
  $description: >-
    Also summons the Magic Pointer, for example Win+Shift+G or Ctrl+Alt+Space.
    Leave empty to disable
- excludedApps: [""]
  $name: Excluded apps
  $description: >-
    Shaking never summons the Magic Pointer while one of these is in the
    foreground or under the pointer. Use a program name (game.exe), a path
    with wildcards (C:\Games\*) or a window title
- fullscreen: true
  $name: Ignore shakes in fullscreen apps and games
  $description: Includes borderless fullscreen games, videos and presentations
- glowColor: "#4285F4"
  $name: Glow color
  $description: >-
    Any color, like #4285F4, 66,133,244 or purple. Use "accent" for your
    Windows accent color or "rainbow" for Google's colors
- glass: true
  $name: Glass effect
  $description: >-
    Give the card and hint Windows' live frosted glass, which follows what is
    behind them. Turn it off for solid colors
- ripple: true
  $name: Ripple animation
  $description: Gently bulge and ripple the screen when the Magic Pointer appears
- theme: auto
  $name: Theme
  $options:
  - auto: Follow Windows
  - light: Light
  - dark: Dark
- pointerSize: 100
  $name: Pointer size (%)
- dim: 50
  $name: Dim outside the selection (%)
  $description: >-
    Darken the area outside your rectangle while you drag. The selected area
    stays clear. 0 turns dimming off
- hint: true
  $name: Show hint
  $description: Show the "Select anything to ask Gemini" bubble when the Magic Pointer appears
- timeout: 15
  $name: Put away after (seconds)
  $description: Put the Magic Pointer away when the mouse stays still this long (0 = never)
- highlight: true
  $name: Highlight what you point at
  $description: Outline the button, image or text under the pointer so that one click selects it
- ocr: true
  $name: Recognize text (OCR)
  $description: >-
    Read the text in every selection with the OCR built into Windows, to copy
    it and to give Gemini more context
- browser: default
  $name: Browser
  $description: >-
    The browser that opens Gemini and Google Lens. If the one you pick isn't
    installed, your Windows default browser is used
  $options:
  - default: Windows default browser
  - chrome: Google Chrome
  - edge: Microsoft Edge
  - firefox: Mozilla Firefox
  - brave: Brave
  - zen: Zen Browser
  - opera: Opera
  - vivaldi: Vivaldi
  - librewolf: LibreWolf
  - waterfox: Waterfox
  - floorp: Floorp
  - custom: Other (set its program below)
- browserPath: ""
  $name: Other browser program
  $description: >-
    Only used when Browser is set to Other: the full path of the browser's
    program, such as C:\Apps\MyBrowser\browser.exe
- apiKey: ""
  $name: Gemini API key
  $description: >-
    Optional. Get a free key at https://aistudio.google.com/apikey to see
    answers and suggestions right in the card
- model: gemini-flash-lite-latest
  $name: Gemini model
  $description: >-
    The model that answers your questions. Flash-Lite starts answering in a
    second or two. Flash models think first, so they are smarter but take
    longer to start, and Pro models need a paid Gemini API plan
  $options:
  - gemini-flash-lite-latest: Gemini Flash-Lite (latest) - fastest
  - gemini-3.5-flash-lite: Gemini 3.5 Flash-Lite - fast
  - gemini-3.1-flash-lite: Gemini 3.1 Flash-Lite - fast
  - gemini-3.6-flash: Gemini 3.6 Flash - fast and smarter
  - gemini-3-flash-preview: Gemini 3 Flash (preview) - balanced
  - gemini-3.8-flash: Gemini 3.8 Flash - newest, slow to start
  - gemini-flash-latest: Gemini Flash (latest) - newest Flash, slow to start
  - gemini-3.1-pro-preview: Gemini 3.1 Pro (preview) - most capable, paid plan
  - gemini-pro-latest: Gemini Pro (latest) - most capable, paid plan
- suggestions: false
  $name: Smart suggestions
  $description: >-
    Automatically upload each selection's image and text to Google for up to
    three suggestions (needs an API key). Off by default
- suggestionsModel: gemini-flash-lite-latest
  $name: Suggestions model
  $description: A fast model works best here; suggestions then show up in about a second
  $options:
  - gemini-flash-lite-latest: Gemini Flash-Lite (latest) - fastest
  - gemini-3.5-flash-lite: Gemini 3.5 Flash-Lite - fast
  - gemini-3.1-flash-lite: Gemini 3.1 Flash-Lite - fast
  - gemini-3.6-flash: Gemini 3.6 Flash - fast and smarter
  - gemini-3-flash-preview: Gemini 3 Flash (preview) - balanced
- translateTo: auto
  $name: Translate to
  $description: >-
    The language of the Translate button on the card, which is shown when a
    Gemini API key is set. Gemini reads the text in your selection and
    translates it into this language
  $options:
  - auto: Windows display language
  - English: English
  - Spanish: Spanish
  - French: French
  - German: German
  - Italian: Italian
  - Portuguese: Portuguese
  - Dutch: Dutch
  - Swedish: Swedish
  - Norwegian: Norwegian
  - Danish: Danish
  - Finnish: Finnish
  - Polish: Polish
  - Czech: Czech
  - Hungarian: Hungarian
  - Romanian: Romanian
  - Greek: Greek
  - Turkish: Turkish
  - Russian: Russian
  - Ukrainian: Ukrainian
  - Arabic: Arabic
  - Hebrew: Hebrew
  - Hindi: Hindi
  - Bengali: Bengali
  - Thai: Thai
  - Vietnamese: Vietnamese
  - Indonesian: Indonesian
  - Chinese (Simplified): Chinese (Simplified)
  - Chinese (Traditional): Chinese (Traditional)
  - Japanese: Japanese
  - Korean: Korean
*/
// ==/WindhawkModSettings==

#include <windowsx.h>
#include <shellapi.h>
#include <objbase.h>
#include <oleauto.h>
#include <exdisp.h>
#include <shldisp.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <servprov.h>
#include <uiautomation.h>
#include <wincodec.h>
#include <winhttp.h>
#include <imm.h>
#include <inspectable.h>
#include <winstring.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Basics

constexpr float kPi = 3.14159265358979f;

constexpr UINT WM_APP_SETTINGS = WM_APP + 1;
constexpr UINT WM_APP_PICK = WM_APP + 2;
constexpr UINT WM_APP_GEMINI = WM_APP + 3;
constexpr UINT WM_APP_OCR = WM_APP + 4;
constexpr UINT WM_APP_BROWSER = WM_APP + 5;
constexpr UINT WM_APP_TEMP_FILE = WM_APP + 6;

constexpr int kHotkeyActivate = 1;
constexpr int kHotkeyEscape = 2;

template <typename T>
void PostOwned(HWND hwnd, UINT message, std::unique_ptr<T> object) {
    T* raw = object.release();
    if (!hwnd ||
        !PostMessageW(hwnd, message, 0, reinterpret_cast<LPARAM>(raw))) {
        delete raw;
    }
}

template <typename T>
T GetProc(PCWSTR module, const char* name) {
    HMODULE handle = GetModuleHandleW(module);
    if (!handle) {
        handle = LoadLibraryExW(module, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!handle) {
        return nullptr;
    }
    return reinterpret_cast<T>(
        reinterpret_cast<void*>(GetProcAddress(handle, name)));
}

struct OsVersion {
    DWORD major;
    DWORD minor;
    DWORD build;
};

const OsVersion& GetOsVersion() {
    static const OsVersion version = [] {
        OsVersion result{6, 1, 7601};
        using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
        if (auto rtlGetVersion =
                GetProc<RtlGetVersion_t>(L"ntdll.dll", "RtlGetVersion")) {
            OSVERSIONINFOW info{};
            info.dwOSVersionInfoSize = sizeof(info);
            if (rtlGetVersion(&info) == 0) {
                result = {info.dwMajorVersion, info.dwMinorVersion,
                          info.dwBuildNumber};
            }
        }
        return result;
    }();
    return version;
}

bool IsWindows81OrLater() {
    const OsVersion& v = GetOsVersion();
    return v.major > 6 || (v.major == 6 && v.minor >= 3);
}

bool IsWindows10Build(DWORD build) {
    const OsVersion& v = GetOsVersion();
    return v.major > 10 || (v.major == 10 && v.build >= build);
}

bool IsWindows11() {
    return IsWindows10Build(22000);
}

template <typename T>
class ComPtr {
  public:
    ComPtr() = default;
    ~ComPtr() { Reset(); }
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ComPtr(ComPtr&& other) noexcept : ptr_(std::exchange(other.ptr_, nullptr)) {}
    ComPtr& operator=(ComPtr&& other) noexcept {
        if (this != &other) {
            Reset();
            ptr_ = std::exchange(other.ptr_, nullptr);
        }
        return *this;
    }

    T* Get() const { return ptr_; }
    T* operator->() const { return ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }
    T** Put() {
        Reset();
        return &ptr_;
    }
    void** PutVoid() { return reinterpret_cast<void**>(Put()); }
    void Reset() {
        if (ptr_) {
            ptr_->Release();
            ptr_ = nullptr;
        }
    }

  private:
    T* ptr_ = nullptr;
};

constexpr GUID kClsidCUIAutomation = {
    0xff48dba4, 0x60ef, 0x4201, {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};
constexpr GUID kClsidCUIAutomation8 = {
    0xe22ad333, 0xb25f, 0x460c, {0x83, 0xd0, 0x05, 0x81, 0x10, 0x73, 0x95, 0xc9}};
constexpr GUID kIidIUIAutomation = {
    0x30cbe57d, 0xd9d0, 0x452a, {0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee}};
constexpr GUID kIidIUIAutomation2 = {
    0x34723aff, 0x0c9d, 0x49d0, {0x98, 0x96, 0x7a, 0xb5, 0x2d, 0xf8, 0xcd, 0x8a}};
constexpr GUID kIidIUIAutomationValuePattern = {
    0xa94cd8b1, 0x0844, 0x4cd6, {0x9d, 0x2d, 0x64, 0x05, 0x37, 0xab, 0x39, 0xe9}};
constexpr GUID kClsidWICImagingFactory1 = {
    0xcacaf262, 0x9370, 0x4615, {0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a}};
constexpr GUID kIidIWICImagingFactory = {
    0xec5ec8a9, 0xc395, 0x4314, {0x9c, 0x77, 0x54, 0xd7, 0xa9, 0x35, 0xff, 0x70}};
constexpr GUID kGuidContainerFormatPng = {
    0x1b7cfaf4, 0x713f, 0x473c, {0xbb, 0xcd, 0x61, 0x37, 0x42, 0x5f, 0xae, 0xaf}};
constexpr GUID kGuidWICPixelFormat24bppBGR = {
    0x6fddc324, 0x4e03, 0x4bfe, {0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0c}};
constexpr GUID kClsidShellWindows = {
    0x9ba05972, 0xf6a8, 0x11cf, {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};
constexpr GUID kIidIShellWindows = {
    0x85cb6900, 0x4d95, 0x11cf, {0x96, 0x0c, 0x00, 0x80, 0xc7, 0xf4, 0xee, 0x85}};
constexpr GUID kSidSTopLevelBrowser = {
    0x4c96be40, 0x915c, 0x11cf, {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}};
constexpr GUID kIidIShellBrowser = {
    0x000214e2, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
constexpr GUID kIidIServiceProvider = {
    0x6d5140c1, 0x7436, 0x11ce, {0x80, 0x34, 0x00, 0xaa, 0x00, 0x60, 0x09, 0xfa}};
constexpr GUID kIidIDispatch = {
    0x00020400, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
constexpr GUID kIidIShellFolderViewDual = {
    0xe7a1af80, 0x4d96, 0x11cf, {0x96, 0x0c, 0x00, 0x80, 0xc7, 0xf4, 0xee, 0x85}};
constexpr GUID kIidIShellDispatch2 = {
    0xa4c6892c, 0x3ba9, 0x11d2, {0x9d, 0xea, 0x00, 0xc0, 0x4f, 0xb1, 0x61, 0x62}};
constexpr GUID kGuidContainerFormatJpeg = {
    0x19e4a5aa, 0x5662, 0x4fc5, {0xa0, 0xc0, 0x17, 0x58, 0x02, 0x8e, 0x10, 0x57}};
constexpr GUID kIidIOcrEngineStatics = {
    0x5bffa85a, 0x3384, 0x3540, {0x99, 0x40, 0x69, 0x91, 0x20, 0xd4, 0x28, 0xa8}};
constexpr GUID kIidISoftwareBitmapStatics = {
    0xdf0385db, 0x672f, 0x4a9d, {0x80, 0x6e, 0xc2, 0x44, 0x2f, 0x34, 0x3e, 0x86}};
constexpr GUID kIidICryptographicBufferStatics = {
    0x320b7e22, 0x3cb0, 0x4cdf, {0x86, 0x63, 0x1d, 0x28, 0x91, 0x00, 0x65, 0xeb}};
constexpr GUID kIidIAsyncInfo = {
    0x00000036, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

double NowMs() {
    static const double msPerTick = [] {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return 1000.0 / static_cast<double>(frequency.QuadPart);
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<double>(counter.QuadPart) * msPerTick;
}

inline float Clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

inline double EaseOutCubic(double t) {
    t = std::clamp(t, 0.0, 1.0);
    const double u = 1.0 - t;
    return 1.0 - u * u * u;
}

inline double EaseOutBack(double t) {
    t = std::clamp(t, 0.0, 1.0);
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    const double u = t - 1.0;
    return 1.0 + c3 * u * u * u + c1 * u * u;
}

////////////////////////////////////////////////////////////////////////////////
// Monitors and DPI

UINT GetMonitorDpi(HMONITOR monitor) {
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static auto getDpiForMonitor =
        GetProc<GetDpiForMonitor_t>(L"shcore.dll", "GetDpiForMonitor");
    UINT dpiX = 0;
    UINT dpiY = 0;
    if (getDpiForMonitor && monitor &&
        SUCCEEDED(getDpiForMonitor(monitor, 0 /* MDT_EFFECTIVE_DPI */, &dpiX,
                                   &dpiY)) &&
        dpiX) {
        return dpiX;
    }
    HDC dc = GetDC(nullptr);
    const int dpi = dc ? GetDeviceCaps(dc, LOGPIXELSX) : 96;
    if (dc) {
        ReleaseDC(nullptr, dc);
    }
    return dpi > 0 ? static_cast<UINT>(dpi) : 96;
}

float DpiScaleAt(POINT pt) {
    return GetMonitorDpi(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST)) /
           96.0f;
}

RECT MonitorRectAt(POINT pt, bool workArea) {
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST),
                        &info)) {
        return workArea ? info.rcWork : info.rcMonitor;
    }
    return {0, 0, GetSystemMetrics(SM_CXSCREEN),
            GetSystemMetrics(SM_CYSCREEN)};
}

RECT VirtualScreenRect() {
    const int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    return {x, y, x + GetSystemMetrics(SM_CXVIRTUALSCREEN),
            y + GetSystemMetrics(SM_CYVIRTUALSCREEN)};
}

POINT RectCenter(const RECT& rc) {
    return {rc.left + (rc.right - rc.left) / 2,
            rc.top + (rc.bottom - rc.top) / 2};
}

////////////////////////////////////////////////////////////////////////////////
// Strings

std::string ToUtf8(const wchar_t* text, size_t length) {
    if (!length) {
        return {};
    }
    const int size = WideCharToMultiByte(CP_UTF8, 0, text,
                                         static_cast<int>(length), nullptr, 0,
                                         nullptr, nullptr);
    std::string result(size > 0 ? size : 0, '\0');
    if (size > 0) {
        WideCharToMultiByte(CP_UTF8, 0, text, static_cast<int>(length),
                            result.data(), size, nullptr, nullptr);
    }
    return result;
}

std::string ToUtf8(const std::wstring& text) {
    return ToUtf8(text.data(), text.size());
}

std::wstring FromUtf8(const std::string& text) {
    if (text.empty()) {
        return {};
    }
    const int size = MultiByteToWideChar(CP_UTF8, 0, text.data(),
                                         static_cast<int>(text.size()),
                                         nullptr, 0);
    std::wstring result(size > 0 ? size : 0, L'\0');
    if (size > 0) {
        MultiByteToWideChar(CP_UTF8, 0, text.data(),
                            static_cast<int>(text.size()), result.data(),
                            size);
    }
    return result;
}

std::wstring Trim(const std::wstring& text) {
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && iswspace(text[begin])) {
        begin++;
    }
    while (end > begin && iswspace(text[end - 1])) {
        end--;
    }
    return text.substr(begin, end - begin);
}

bool ContainsNoCase(const std::string& haystack, const char* needle) {
    const size_t needleLength = strlen(needle);
    if (needleLength > haystack.size()) {
        return false;
    }
    for (size_t i = 0; i + needleLength <= haystack.size(); i++) {
        size_t j = 0;
        while (j < needleLength &&
               tolower(static_cast<unsigned char>(haystack[i + j])) ==
                   tolower(static_cast<unsigned char>(needle[j]))) {
            j++;
        }
        if (j == needleLength) {
            return true;
        }
    }
    return false;
}

void AppendJsonString(std::string& out, const std::string& text) {
    out += '"';
    for (unsigned char c : text) {
        switch (c) {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                if (c < 0x20) {
                    char escaped[8];
                    snprintf(escaped, sizeof(escaped), "\\u%04x", c);
                    out += escaped;
                } else {
                    out += static_cast<char>(c);
                }
        }
    }
    out += '"';
}

std::string Base64Encode(const uint8_t* data, size_t size) {
    static const char kTable[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve((size + 2) / 3 * 4);
    size_t i = 0;
    for (; i + 2 < size; i += 3) {
        const uint32_t v = (uint32_t(data[i]) << 16) |
                           (uint32_t(data[i + 1]) << 8) | data[i + 2];
        out += kTable[v >> 18];
        out += kTable[(v >> 12) & 63];
        out += kTable[(v >> 6) & 63];
        out += kTable[v & 63];
    }
    if (i < size) {
        uint32_t v = uint32_t(data[i]) << 16;
        if (i + 1 < size) {
            v |= uint32_t(data[i + 1]) << 8;
        }
        out += kTable[v >> 18];
        out += kTable[(v >> 12) & 63];
        out += i + 1 < size ? kTable[(v >> 6) & 63] : '=';
        out += '=';
    }
    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Minimal JSON reader, enough for Gemini API responses

struct Json;
struct JsonField;

struct Json {
    enum class Type { Null, Bool, Number, String, Array, Object };

    Type type = Type::Null;
    bool boolean = false;
    double number = 0;
    std::string string;
    std::vector<Json> items;
    std::vector<JsonField> fields;

    const Json* Get(const char* key) const;
    const Json* At(size_t index) const {
        return type == Type::Array && index < items.size() ? &items[index]
                                                           : nullptr;
    }
    bool IsString() const { return type == Type::String; }
};

struct JsonField {
    std::string key;
    Json value;
};

const Json* Json::Get(const char* key) const {
    if (type != Type::Object) {
        return nullptr;
    }
    for (const JsonField& field : fields) {
        if (field.key == key) {
            return &field.value;
        }
    }
    return nullptr;
}

class JsonReader {
  public:
    JsonReader(const char* begin, const char* end) : p_(begin), end_(end) {}

    bool ReadDocument(Json& out) {
        if (!ReadValue(out, 0)) {
            return false;
        }
        SkipSpace();
        return p_ == end_;
    }

  private:
    void SkipSpace() {
        while (p_ < end_ &&
               (*p_ == ' ' || *p_ == '\t' || *p_ == '\n' || *p_ == '\r')) {
            p_++;
        }
    }

    bool Literal(const char* word) {
        const size_t length = strlen(word);
        if (static_cast<size_t>(end_ - p_) < length ||
            memcmp(p_, word, length) != 0) {
            return false;
        }
        p_ += length;
        return true;
    }

    bool ReadHex4(uint32_t& out) {
        if (end_ - p_ < 4) {
            return false;
        }
        out = 0;
        for (int i = 0; i < 4; i++) {
            const char c = *p_++;
            out <<= 4;
            if (c >= '0' && c <= '9') {
                out |= c - '0';
            } else if (c >= 'a' && c <= 'f') {
                out |= c - 'a' + 10;
            } else if (c >= 'A' && c <= 'F') {
                out |= c - 'A' + 10;
            } else {
                return false;
            }
        }
        return true;
    }

    static void AppendUtf8(std::string& out, uint32_t cp) {
        if (cp < 0x80) {
            out += static_cast<char>(cp);
        } else if (cp < 0x800) {
            out += static_cast<char>(0xC0 | (cp >> 6));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            out += static_cast<char>(0xE0 | (cp >> 12));
            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        } else {
            out += static_cast<char>(0xF0 | (cp >> 18));
            out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        }
    }

    bool ReadString(std::string& out) {
        p_++;  // Opening quote.
        while (p_ < end_) {
            const char c = *p_++;
            if (c == '"') {
                return true;
            }
            if (c != '\\') {
                out += c;
                continue;
            }
            if (p_ >= end_) {
                return false;
            }
            const char escaped = *p_++;
            switch (escaped) {
                case '"':
                case '\\':
                case '/':
                    out += escaped;
                    break;
                case 'b':
                    out += '\b';
                    break;
                case 'f':
                    out += '\f';
                    break;
                case 'n':
                    out += '\n';
                    break;
                case 'r':
                    out += '\r';
                    break;
                case 't':
                    out += '\t';
                    break;
                case 'u': {
                    uint32_t cp;
                    if (!ReadHex4(cp)) {
                        return false;
                    }
                    if (cp >= 0xD800 && cp <= 0xDBFF && end_ - p_ >= 6 &&
                        p_[0] == '\\' && p_[1] == 'u') {
                        const char* save = p_;
                        p_ += 2;
                        uint32_t low;
                        if (ReadHex4(low) && low >= 0xDC00 && low <= 0xDFFF) {
                            cp = 0x10000 + ((cp - 0xD800) << 10) +
                                 (low - 0xDC00);
                        } else {
                            p_ = save;
                        }
                    }
                    AppendUtf8(out, cp);
                    break;
                }
                default:
                    return false;
            }
        }
        return false;
    }

    bool ReadNumber(double& out) {
        char buffer[64];
        size_t length = 0;
        while (p_ < end_ && length < sizeof(buffer) - 1 &&
               ((*p_ >= '0' && *p_ <= '9') || *p_ == '-' || *p_ == '+' ||
                *p_ == '.' || *p_ == 'e' || *p_ == 'E')) {
            buffer[length++] = *p_++;
        }
        if (!length) {
            return false;
        }
        buffer[length] = '\0';
        out = strtod(buffer, nullptr);
        return true;
    }

    bool ReadValue(Json& out, int depth) {
        if (depth > 64) {
            return false;
        }
        SkipSpace();
        if (p_ >= end_) {
            return false;
        }
        const char c = *p_;
        if (c == '{') {
            p_++;
            out.type = Json::Type::Object;
            SkipSpace();
            if (p_ < end_ && *p_ == '}') {
                p_++;
                return true;
            }
            for (;;) {
                SkipSpace();
                JsonField field;
                if (p_ >= end_ || *p_ != '"' || !ReadString(field.key)) {
                    return false;
                }
                SkipSpace();
                if (p_ >= end_ || *p_ != ':') {
                    return false;
                }
                p_++;
                if (!ReadValue(field.value, depth + 1)) {
                    return false;
                }
                out.fields.push_back(std::move(field));
                SkipSpace();
                if (p_ < end_ && *p_ == ',') {
                    p_++;
                    continue;
                }
                if (p_ < end_ && *p_ == '}') {
                    p_++;
                    return true;
                }
                return false;
            }
        }
        if (c == '[') {
            p_++;
            out.type = Json::Type::Array;
            SkipSpace();
            if (p_ < end_ && *p_ == ']') {
                p_++;
                return true;
            }
            for (;;) {
                Json item;
                if (!ReadValue(item, depth + 1)) {
                    return false;
                }
                out.items.push_back(std::move(item));
                SkipSpace();
                if (p_ < end_ && *p_ == ',') {
                    p_++;
                    continue;
                }
                if (p_ < end_ && *p_ == ']') {
                    p_++;
                    return true;
                }
                return false;
            }
        }
        if (c == '"') {
            out.type = Json::Type::String;
            return ReadString(out.string);
        }
        if (c == 't') {
            out.type = Json::Type::Bool;
            out.boolean = true;
            return Literal("true");
        }
        if (c == 'f') {
            out.type = Json::Type::Bool;
            return Literal("false");
        }
        if (c == 'n') {
            return Literal("null");
        }
        out.type = Json::Type::Number;
        return ReadNumber(out.number);
    }

    const char* p_;
    const char* end_;
};

bool ParseJson(const std::string& text, Json& out) {
    return JsonReader(text.data(), text.data() + text.size()).ReadDocument(out);
}

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class ThemeMode { Auto, Light, Dark };

struct Settings {
    bool shake = true;
    int sensitivity = 5;
    std::wstring hotkey;
    std::vector<std::wstring> excludedApps;
    bool disableInFullscreen = true;
    std::wstring glowColor;
    bool glass = true;
    bool ripple = true;
    ThemeMode theme = ThemeMode::Auto;
    int pointerSize = 100;
    int dim = 50;
    bool hint = true;
    int timeoutSec = 15;
    bool highlight = true;
    bool ocr = true;
    std::wstring browser;
    std::wstring browserPath;
    std::wstring apiKey;
    std::wstring model;
    bool suggestions = false;
    std::wstring suggestionsModel;
    std::wstring translateTo;
};

std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value;
    Wh_FreeStringSetting(value);
    return Trim(result);
}

Settings LoadSettings() {
    Settings s;
    s.shake = Wh_GetIntSetting(L"shake") != 0;
    const int sensitivity = Wh_GetIntSetting(L"sensitivity");
    s.sensitivity = sensitivity <= 0 ? 5 : std::min(sensitivity, 10);
    s.hotkey = GetStringSetting(L"hotkey");
    for (int i = 0; i < 256; i++) {
        const std::wstring app =
            GetStringSetting((L"excludedApps[" + std::to_wstring(i) + L"]").c_str());
        if (app.empty()) {
            continue;
        }
        s.excludedApps.push_back(app);
    }
    s.disableInFullscreen = Wh_GetIntSetting(L"fullscreen") != 0;
    s.glowColor = GetStringSetting(L"glowColor");
    s.glass = Wh_GetIntSetting(L"glass") != 0;
    s.ripple = Wh_GetIntSetting(L"ripple") != 0;

    const std::wstring theme = GetStringSetting(L"theme");
    s.theme = theme == L"light"  ? ThemeMode::Light
              : theme == L"dark" ? ThemeMode::Dark
                                 : ThemeMode::Auto;

    const int pointerSize = Wh_GetIntSetting(L"pointerSize");
    s.pointerSize = pointerSize <= 0 ? 100 : std::clamp(pointerSize, 50, 300);
    s.dim = std::clamp(Wh_GetIntSetting(L"dim"), 0, 80);
    s.hint = Wh_GetIntSetting(L"hint") != 0;
    s.timeoutSec = std::clamp(Wh_GetIntSetting(L"timeout"), 0, 3600);
    s.highlight = Wh_GetIntSetting(L"highlight") != 0;
    s.ocr = Wh_GetIntSetting(L"ocr") != 0;
    s.browser = GetStringSetting(L"browser");
    s.browserPath = GetStringSetting(L"browserPath");
    s.apiKey = GetStringSetting(L"apiKey");
    s.model = GetStringSetting(L"model");
    s.suggestions = Wh_GetIntSetting(L"suggestions") != 0;
    s.suggestionsModel = GetStringSetting(L"suggestionsModel");
    s.translateTo = GetStringSetting(L"translateTo");
    return s;
}

Settings g_settings;

bool SystemUsesDarkTheme() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
            &size) != ERROR_SUCCESS) {
        return false;
    }
    return value == 0;
}

bool UseDarkTheme() {
    switch (g_settings.theme) {
        case ThemeMode::Light:
            return false;
        case ThemeMode::Dark:
            return true;
        case ThemeMode::Auto:
            break;
    }
    return SystemUsesDarkTheme();
}

////////////////////////////////////////////////////////////////////////////////
// Colors

struct Color {
    float r, g, b, a;
};

constexpr Color Rgb(int r, int g, int b, float a = 1.0f) {
    return {r / 255.0f, g / 255.0f, b / 255.0f, a};
}

inline Color WithAlpha(Color c, float a) {
    c.a = a;
    return c;
}

inline Color Mix(const Color& x, const Color& y, float t) {
    return {x.r + (y.r - x.r) * t, x.g + (y.g - x.g) * t,
            x.b + (y.b - x.b) * t, x.a + (y.a - x.a) * t};
}

constexpr Color kWhite = Rgb(255, 255, 255);
constexpr Color kBlack = Rgb(0, 0, 0);
constexpr Color kGoogleBlue = Rgb(66, 133, 244);
constexpr Color kGoogleRed = Rgb(234, 67, 53);
constexpr Color kGoogleYellow = Rgb(251, 188, 4);
constexpr Color kGoogleGreen = Rgb(52, 168, 83);

Color GoogleGradient(float t) {
    static const Color kStops[] = {kGoogleBlue, kGoogleRed, kGoogleYellow,
                                   kGoogleGreen};
    t -= floorf(t);
    const float x = t * 4.0f;
    const int i = static_cast<int>(x);
    float f = x - i;
    f = f * f * (3.0f - 2.0f * f);
    return Mix(kStops[i & 3], kStops[(i + 1) & 3], f);
}

struct Hsl {
    float h, s, l;  // Hue in degrees, saturation and lightness 0..1.
};

Hsl ToHsl(const Color& c) {
    const float mx = std::max({c.r, c.g, c.b});
    const float mn = std::min({c.r, c.g, c.b});
    Hsl out{0, 0, (mx + mn) * 0.5f};
    const float d = mx - mn;
    if (d > 1e-5f) {
        out.s = out.l > 0.5f ? d / (2.0f - mx - mn) : d / (mx + mn);
        if (mx == c.r) {
            out.h = (c.g - c.b) / d + (c.g < c.b ? 6.0f : 0.0f);
        } else if (mx == c.g) {
            out.h = (c.b - c.r) / d + 2.0f;
        } else {
            out.h = (c.r - c.g) / d + 4.0f;
        }
        out.h *= 60.0f;
    }
    return out;
}

Color FromHsl(Hsl hsl, float alpha = 1.0f) {
    hsl.h = fmodf(hsl.h, 360.0f);
    if (hsl.h < 0) {
        hsl.h += 360.0f;
    }
    hsl.s = Clamp01(hsl.s);
    hsl.l = Clamp01(hsl.l);
    const float q = hsl.l < 0.5f ? hsl.l * (1 + hsl.s) : hsl.l + hsl.s - hsl.l * hsl.s;
    const float p = 2 * hsl.l - q;
    auto channel = [&](float t) {
        t -= floorf(t);
        if (t < 1.0f / 6) return p + (q - p) * 6 * t;
        if (t < 0.5f) return q;
        if (t < 2.0f / 3) return p + (q - p) * (2.0f / 3 - t) * 6;
        return p;
    };
    const float h = hsl.h / 360.0f;
    return {channel(h + 1.0f / 3), channel(h), channel(h - 1.0f / 3), alpha};
}

struct GlowPalette {
    bool rainbow = false;
    Color color = kGoogleBlue;

    Color At(float t) const {
        if (rainbow) {
            return GoogleGradient(t);
        }
        return color;
    }

    Color Base() const { return rainbow ? kGoogleBlue : color; }

    Color Accent(bool dark) const {
        Hsl hsl = ToHsl(Base());
        if (dark) {
            hsl.l = std::max(hsl.l, 0.68f);
        } else {
            hsl.l = std::min(hsl.l, 0.42f);
        }
        return FromHsl(hsl);
    }
};

GlowPalette MakeGlowPalette(Color base) {
    GlowPalette palette;
    palette.color = base;
    return palette;
}

GlowPalette ParseGlowColor(std::wstring text) {
    for (wchar_t& c : text) {
        c = towlower(c);
    }
    text = Trim(text);
    const Color fallback = kGoogleBlue;
    if (text == L"rainbow" || text == L"google") {
        GlowPalette palette;
        palette.rainbow = true;
        return palette;
    }
    if (text.empty()) {
        return MakeGlowPalette(fallback);
    }
    if (text == L"accent" || text == L"system" || text == L"windows") {
        DWORD value = 0;
        DWORD size = sizeof(value);
        if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                         L"AccentColor", RRF_RT_REG_DWORD, nullptr, &value,
                         &size) == ERROR_SUCCESS) {
            return MakeGlowPalette(Rgb(value & 0xFF, (value >> 8) & 0xFF,
                                       (value >> 16) & 0xFF));
        }
        return MakeGlowPalette(fallback);
    }
    static const struct {
        PCWSTR name;
        Color color;
    } kNames[] = {
        {L"blue", Rgb(66, 133, 244)},    {L"red", Rgb(234, 67, 53)},
        {L"green", Rgb(52, 168, 83)},    {L"yellow", Rgb(251, 188, 4)},
        {L"orange", Rgb(255, 128, 32)},  {L"purple", Rgb(150, 92, 255)},
        {L"violet", Rgb(150, 92, 255)},  {L"pink", Rgb(255, 92, 170)},
        {L"magenta", Rgb(232, 64, 220)}, {L"cyan", Rgb(0, 200, 230)},
        {L"teal", Rgb(0, 168, 150)},     {L"white", Rgb(240, 244, 255)},
        {L"gold", Rgb(240, 180, 40)},    {L"mint", Rgb(64, 220, 160)},
    };
    for (const auto& entry : kNames) {
        if (text == entry.name) {
            return MakeGlowPalette(entry.color);
        }
    }

    std::wstring hex = text;
    if (!hex.empty() && hex[0] == L'#') {
        hex.erase(0, 1);
    }
    const bool isHex =
        (hex.size() == 3 || hex.size() == 6) &&
        hex.find_first_not_of(L"0123456789abcdef") == std::wstring::npos;
    if (isHex) {
        const unsigned long v = wcstoul(hex.c_str(), nullptr, 16);
        if (hex.size() == 3) {
            return MakeGlowPalette(Rgb(((v >> 8) & 0xF) * 17, ((v >> 4) & 0xF) * 17,
                                       (v & 0xF) * 17));
        }
        return MakeGlowPalette(Rgb((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF));
    }

    int values[3];
    int count = 0;
    for (size_t i = 0; i < text.size() && count < 3;) {
        if (iswdigit(text[i])) {
            values[count++] = std::min(255, _wtoi(text.c_str() + i));
            while (i < text.size() && iswdigit(text[i])) {
                i++;
            }
        } else {
            i++;
        }
    }
    if (count == 3) {
        return MakeGlowPalette(Rgb(values[0], values[1], values[2]));
    }
    return MakeGlowPalette(fallback);
}

GlowPalette g_glow = MakeGlowPalette(kGoogleBlue);

constexpr double kGlowPeriodMs = 3200.0;

float GlowBreath(double now) {
    return 0.62f + 0.38f * sinf(static_cast<float>(
                               fmod(now, kGlowPeriodMs) / kGlowPeriodMs) * 2.0f * kPi);
}

inline uint32_t PackPremultiplied(const Color& c, float coverage) {
    const float a = Clamp01(c.a * coverage);
    if (a <= 0.0f) {
        return 0;
    }
    const uint32_t alpha = static_cast<uint32_t>(a * 255.0f + 0.5f);
    auto channel = [&](float v) {
        return std::min(alpha,
                        static_cast<uint32_t>(Clamp01(v) * a * 255.0f + 0.5f));
    };
    return (alpha << 24) | (channel(c.r) << 16) | (channel(c.g) << 8) |
           channel(c.b);
}

inline void BlendOver(uint32_t& dst, uint32_t src) {
    const uint32_t sa = src >> 24;
    if (sa == 0) {
        return;
    }
    if (sa == 255) {
        dst = src;
        return;
    }
    const uint32_t inv = 255 - sa;
    const uint32_t d = dst;
    uint32_t rb = (d & 0x00FF00FF) * inv + 0x00800080;
    rb = ((rb + ((rb >> 8) & 0x00FF00FF)) >> 8) & 0x00FF00FF;
    uint32_t ag = ((d >> 8) & 0x00FF00FF) * inv + 0x00800080;
    ag = (ag + ((ag >> 8) & 0x00FF00FF)) & 0xFF00FF00;
    dst = src + (rb | ag);
}

inline void BlendColor(uint32_t& dst, const Color& c, float coverage) {
    BlendOver(dst, PackPremultiplied(c, coverage));
}

////////////////////////////////////////////////////////////////////////////////
// Surfaces

class Surface {
  public:
    Surface() = default;
    ~Surface() { Reset(); }
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;

    bool Resize(int width, int height) {
        width = std::max(width, 1);
        height = std::max(height, 1);
        if (!bits_ || width > stride_ || height > capacityHeight_) {
            Reset();
            const int capacityWidth = (width + 63) & ~63;
            const int capacityHeight = (height + 63) & ~63;

            BITMAPINFO info{};
            info.bmiHeader.biSize = sizeof(info.bmiHeader);
            info.bmiHeader.biWidth = capacityWidth;
            info.bmiHeader.biHeight = -capacityHeight;  // Top-down.
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;

            dc_ = CreateCompatibleDC(nullptr);
            if (!dc_) {
                return false;
            }
            void* bits = nullptr;
            bitmap_ = CreateDIBSection(dc_, &info, DIB_RGB_COLORS, &bits,
                                       nullptr, 0);
            if (!bitmap_ || !bits) {
                Reset();
                return false;
            }
            oldBitmap_ = SelectObject(dc_, bitmap_);
            bits_ = static_cast<uint32_t*>(bits);
            stride_ = capacityWidth;
            capacityHeight_ = capacityHeight;
        }
        width_ = width;
        height_ = height;
        Clear();
        return true;
    }

    void Reset() {
        if (dc_) {
            if (oldBitmap_) {
                SelectObject(dc_, oldBitmap_);
            }
            DeleteDC(dc_);
        }
        if (bitmap_) {
            DeleteObject(bitmap_);
        }
        dc_ = nullptr;
        bitmap_ = nullptr;
        oldBitmap_ = nullptr;
        bits_ = nullptr;
        width_ = height_ = stride_ = capacityHeight_ = 0;
    }

    void Clear() {
        for (int y = 0; y < height_; y++) {
            memset(Row(y), 0, static_cast<size_t>(width_) * 4);
        }
    }

    bool Valid() const { return bits_ != nullptr; }
    int Width() const { return width_; }
    int Height() const { return height_; }
    HDC Dc() const { return dc_; }
    uint32_t* Row(int y) { return bits_ + static_cast<size_t>(y) * stride_; }
    const uint32_t* Row(int y) const {
        return bits_ + static_cast<size_t>(y) * stride_;
    }

  private:
    HDC dc_ = nullptr;
    HBITMAP bitmap_ = nullptr;
    HGDIOBJ oldBitmap_ = nullptr;
    uint32_t* bits_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    int stride_ = 0;
    int capacityHeight_ = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Signed distance shapes

struct Vec2 {
    float x, y;
};

inline float SdRoundRect(float px,
                         float py,
                         float left,
                         float top,
                         float right,
                         float bottom,
                         float radius) {
    const float hw = (right - left) * 0.5f;
    const float hh = (bottom - top) * 0.5f;
    const float r = std::max(0.0f, std::min(radius, std::min(hw, hh)));
    const float qx = fabsf(px - (left + hw)) - hw + r;
    const float qy = fabsf(py - (top + hh)) - hh + r;
    const float ox = std::max(qx, 0.0f);
    const float oy = std::max(qy, 0.0f);
    return sqrtf(ox * ox + oy * oy) + std::min(std::max(qx, qy), 0.0f) - r;
}

float SdPolygon(const Vec2* v, int n, Vec2 p) {
    float d = (p.x - v[0].x) * (p.x - v[0].x) + (p.y - v[0].y) * (p.y - v[0].y);
    float sign = 1.0f;
    for (int i = 0, j = n - 1; i < n; j = i, i++) {
        const Vec2 e{v[j].x - v[i].x, v[j].y - v[i].y};
        const Vec2 w{p.x - v[i].x, p.y - v[i].y};
        const float t =
            Clamp01((w.x * e.x + w.y * e.y) / (e.x * e.x + e.y * e.y));
        const Vec2 b{w.x - e.x * t, w.y - e.y * t};
        d = std::min(d, b.x * b.x + b.y * b.y);
        const bool c1 = p.y >= v[i].y;
        const bool c2 = p.y < v[j].y;
        const bool c3 = e.x * w.y > e.y * w.x;
        if ((c1 && c2 && c3) || (!c1 && !c2 && !c3)) {
            sign = -sign;
        }
    }
    return sign * sqrtf(d);
}

inline float SdSegment(Vec2 p, Vec2 a, Vec2 b) {
    const Vec2 pa{p.x - a.x, p.y - a.y};
    const Vec2 ba{b.x - a.x, b.y - a.y};
    const float lengthSq = ba.x * ba.x + ba.y * ba.y;
    const float h =
        lengthSq > 0 ? Clamp01((pa.x * ba.x + pa.y * ba.y) / lengthSq) : 0.0f;
    const float dx = pa.x - ba.x * h;
    const float dy = pa.y - ba.y * h;
    return sqrtf(dx * dx + dy * dy);
}

inline float Erfc(float x) {
    const float z = fabsf(x);
    const float t = 1.0f / (1.0f + 0.3275911f * z);
    const float y =
        t *
        (0.254829592f +
         t * (-0.284496736f +
              t * (1.421413741f + t * (-1.453152027f + t * 1.061405429f)))) *
        expf(-z * z);
    return x >= 0 ? y : 2.0f - y;
}

inline float SoftCoverage(float distance, float sigma) {
    return 0.5f * Erfc(distance / (sigma * 1.41421356f));
}

inline float FastAtan2(float y, float x) {
    const float ax = fabsf(x);
    const float ay = fabsf(y);
    const float mx = std::max(ax, ay);
    const float mn = std::min(ax, ay);
    const float a = mx > 0 ? mn / mx : 0.0f;
    const float s = a * a;
    float r = ((-0.0464964749f * s + 0.15931422f) * s - 0.327622764f) * s * a +
              a;
    if (ay > ax) {
        r = 1.57079637f - r;
    }
    if (x < 0) {
        r = 3.14159274f - r;
    }
    return y < 0 ? -r : r;
}

template <typename DistanceFn, typename ColorFn>
void FillShape(Surface& s,
               float x0,
               float y0,
               float x1,
               float y1,
               DistanceFn distance,
               ColorFn color) {
    const int ix0 = std::max(0, static_cast<int>(floorf(x0)));
    const int iy0 = std::max(0, static_cast<int>(floorf(y0)));
    const int ix1 = std::min(s.Width(), static_cast<int>(ceilf(x1)));
    const int iy1 = std::min(s.Height(), static_cast<int>(ceilf(y1)));
    for (int y = iy0; y < iy1; y++) {
        uint32_t* row = s.Row(y);
        const float py = y + 0.5f;
        for (int x = ix0; x < ix1; x++) {
            const float px = x + 0.5f;
            const float coverage = Clamp01(0.5f - distance(px, py));
            if (coverage > 0.0f) {
                BlendColor(row[x], color(px, py), coverage);
            }
        }
    }
}

void FillRoundRect(Surface& s,
                   float l,
                   float t,
                   float r,
                   float b,
                   float radius,
                   Color c) {
    FillShape(
        s, l - 1, t - 1, r + 1, b + 1,
        [&](float x, float y) { return SdRoundRect(x, y, l, t, r, b, radius); },
        [&](float, float) { return c; });
}

void FillRoundRectGradient(Surface& s,
                           float l,
                           float t,
                           float r,
                           float b,
                           float radius,
                           Color top,
                           Color bottom) {
    const float height = std::max(1.0f, b - t);
    FillShape(
        s, l - 1, t - 1, r + 1, b + 1,
        [&](float x, float y) { return SdRoundRect(x, y, l, t, r, b, radius); },
        [&](float, float y) { return Mix(top, bottom, Clamp01((y - t) / height)); });
}

void StrokeRoundRect(Surface& s,
                     float l,
                     float t,
                     float r,
                     float b,
                     float radius,
                     float width,
                     Color c) {
    FillShape(
        s, l - width, t - width, r + width, b + width,
        [&](float x, float y) {
            return fabsf(SdRoundRect(x, y, l, t, r, b, radius)) - width * 0.5f;
        },
        [&](float, float) { return c; });
}

void StrokeRoundRectDashed(Surface& s,
                           float l,
                           float t,
                           float r,
                           float b,
                           float radius,
                           float width,
                           float dash,
                           Color c) {
    const float cx = (l + r) * 0.5f;
    const float cy = (t + b) * 0.5f;
    const float perimeter = 2.0f * ((r - l) + (b - t));
    const int dashes = std::max(4, static_cast<int>(perimeter / (dash * 2.0f)));
    FillShape(
        s, l - width, t - width, r + width, b + width,
        [&](float x, float y) {
            return fabsf(SdRoundRect(x, y, l, t, r, b, radius)) - width * 0.5f;
        },
        [&](float x, float y) {
            const float angle = FastAtan2(y - cy, x - cx) / (2.0f * kPi) + 0.5f;
            const float phase = angle * dashes;
            return (phase - floorf(phase)) < 0.5f ? c : WithAlpha(c, 0.0f);
        });
}

void StrokeCircle(Surface& s,
                  float cx,
                  float cy,
                  float radius,
                  float width,
                  Color c) {
    FillShape(
        s, cx - radius - width, cy - radius - width, cx + radius + width,
        cy + radius + width,
        [&](float x, float y) {
            return fabsf(sqrtf((x - cx) * (x - cx) + (y - cy) * (y - cy)) -
                         radius) -
                   width * 0.5f;
        },
        [&](float, float) { return c; });
}

void StrokeLine(Surface& s, Vec2 a, Vec2 b, float width, Color c) {
    const float half = width * 0.5f;
    FillShape(
        s, std::min(a.x, b.x) - width, std::min(a.y, b.y) - width,
        std::max(a.x, b.x) + width, std::max(a.y, b.y) + width,
        [&](float x, float y) { return SdSegment({x, y}, a, b) - half; },
        [&](float, float) { return c; });
}

void FillPolygon(Surface& s, const Vec2* points, int count, float round, Color c) {
    float x0 = points[0].x, y0 = points[0].y, x1 = x0, y1 = y0;
    for (int i = 1; i < count; i++) {
        x0 = std::min(x0, points[i].x);
        y0 = std::min(y0, points[i].y);
        x1 = std::max(x1, points[i].x);
        y1 = std::max(y1, points[i].y);
    }
    FillShape(
        s, x0 - round - 1, y0 - round - 1, x1 + round + 1, y1 + round + 1,
        [&](float x, float y) {
            return SdPolygon(points, count, {x, y}) - round;
        },
        [&](float, float) { return c; });
}

void DrawShadow(Surface& s,
                float l,
                float t,
                float r,
                float b,
                float radius,
                float sigma,
                Color c) {
    const float reach = sigma * 3.0f;
    const int ix0 = std::max(0, static_cast<int>(floorf(l - reach)));
    const int iy0 = std::max(0, static_cast<int>(floorf(t - reach)));
    const int ix1 = std::min(s.Width(), static_cast<int>(ceilf(r + reach)));
    const int iy1 = std::min(s.Height(), static_cast<int>(ceilf(b + reach)));
    for (int y = iy0; y < iy1; y++) {
        uint32_t* row = s.Row(y);
        for (int x = ix0; x < ix1; x++) {
            const float d = SdRoundRect(x + 0.5f, y + 0.5f, l, t, r, b, radius);
            const float coverage = SoftCoverage(d, sigma);
            if (coverage > 0.002f) {
                BlendColor(row[x], c, coverage);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// The Gemini sparkle

const Vec2* SparklePolygon(int* count) {
    constexpr int kPoints = 64;
    struct Shape {
        Vec2 points[kPoints];
        Shape() {
            const float exponent = 2.0f / 0.56f;
            for (int i = 0; i < kPoints; i++) {
                const float t = 2.0f * kPi * i / kPoints;
                const float c = cosf(t);
                const float s = sinf(t);
                points[i] = {copysignf(powf(fabsf(c), exponent), c),
                             copysignf(powf(fabsf(s), exponent), s)};
            }
        }
    };
    static const Shape shape;
    *count = kPoints;
    return shape.points;
}

inline float SdSparkle(Vec2 p, float cx, float cy, float radius, float angle) {
    struct Quadrant {
        Vec2 points[16];
        Vec2 edges[16];
        float inverseLength[16];
        Quadrant() {
            int count;
            const Vec2* polygon = SparklePolygon(&count);
            for (int i = 0; i < 16; i++) {
                points[i] = polygon[i];
                edges[i] = {polygon[i + 1].x - polygon[i].x,
                             polygon[i + 1].y - polygon[i].y};
                inverseLength[i] = 1.0f /
                    (edges[i].x * edges[i].x + edges[i].y * edges[i].y);
            }
        }
    };
    static const Quadrant quadrant;
    const float c = cosf(-angle);
    const float s = sinf(-angle);
    const float x = (p.x - cx) / radius;
    const float y = (p.y - cy) / radius;
    const Vec2 q{fabsf(x * c - y * s), fabsf(x * s + y * c)};
    float distanceSq = 1e9f;
    bool inside = false;
    for (int i = 0; i < 16; i++) {
        const Vec2 a = quadrant.points[i];
        const Vec2 e = quadrant.edges[i];
        const Vec2 w{q.x - a.x, q.y - a.y};
        const float t = Clamp01((w.x * e.x + w.y * e.y) * quadrant.inverseLength[i]);
        const Vec2 d{w.x - e.x * t, w.y - e.y * t};
        distanceSq = std::min(distanceSq, d.x * d.x + d.y * d.y);
        if (q.y >= a.y && q.y < a.y + e.y && e.x * w.y > e.y * w.x) {
            inside = true;
        }
    }
    return (inside ? -1.0f : 1.0f) * sqrtf(distanceSq) * radius;
}

void FillSparkleGradient(Surface& s,
                         float cx,
                         float cy,
                         float radius,
                         float angle,
                         float phase) {
    FillShape(
        s, cx - radius - 1, cy - radius - 1, cx + radius + 1, cy + radius + 1,
        [&](float x, float y) {
            return SdSparkle({x, y}, cx, cy, radius, angle);
        },
        [&](float x, float y) {
            const Color color =
                g_glow.At(phase + 0.5f * (x - cx + y - cy) / (radius * 2.0f));
            const float d = sqrtf((x - cx) * (x - cx) + (y - cy) * (y - cy));
            return Mix(color, kWhite, 0.55f * Clamp01(1.0f - d / (radius * 0.5f)));
        });
}

////////////////////////////////////////////////////////////////////////////////
// Images

struct Image {
    int width = 0;
    int height = 0;
    std::vector<uint32_t> pixels;  // Opaque BGRA, top-down.

    bool Empty() const { return pixels.empty(); }
};

Image DownscaleImage(const Image& src, int maxSide) {
    if (src.Empty()) {
        return {};
    }
    const int longest = std::max(src.width, src.height);
    if (longest <= maxSide) {
        return src;
    }
    Image dst;
    dst.width = std::max(1, static_cast<int>(int64_t(src.width) * maxSide / longest));
    dst.height = std::max(1, static_cast<int>(int64_t(src.height) * maxSide / longest));
    dst.pixels.resize(static_cast<size_t>(dst.width) * dst.height);
    for (int y = 0; y < dst.height; y++) {
        const int sy0 = static_cast<int>(int64_t(y) * src.height / dst.height);
        const int sy1 = std::max(
            sy0 + 1, static_cast<int>(int64_t(y + 1) * src.height / dst.height));
        for (int x = 0; x < dst.width; x++) {
            const int sx0 = static_cast<int>(int64_t(x) * src.width / dst.width);
            const int sx1 = std::max(
                sx0 + 1, static_cast<int>(int64_t(x + 1) * src.width / dst.width));
            uint32_t sumR = 0, sumG = 0, sumB = 0, count = 0;
            for (int sy = sy0; sy < sy1; sy++) {
                const uint32_t* row =
                    &src.pixels[static_cast<size_t>(sy) * src.width];
                for (int sx = sx0; sx < sx1; sx++) {
                    const uint32_t p = row[sx];
                    sumR += (p >> 16) & 0xFF;
                    sumG += (p >> 8) & 0xFF;
                    sumB += p & 0xFF;
                    count++;
                }
            }
            dst.pixels[static_cast<size_t>(y) * dst.width + x] =
                0xFF000000 | ((sumR / count) << 16) | ((sumG / count) << 8) |
                (sumB / count);
        }
    }
    return dst;
}

void DrawImageRounded(Surface& s,
                      const Image& image,
                      float l,
                      float t,
                      float r,
                      float b,
                      float radius,
                      Color letterbox) {
    if (image.Empty()) {
        return;
    }
    const float aspect = static_cast<float>(image.width) / image.height;
    const float boxAspect = (r - l) / (b - t);
    if (aspect > boxAspect * 2.0f || aspect < boxAspect * 0.5f) {
        FillRoundRect(s, l, t, r, b, radius, letterbox);
        const float scale = std::min((r - l) / image.width, (b - t) / image.height);
        const float w = image.width * scale;
        const float h = image.height * scale;
        const float x = l + ((r - l) - w) * 0.5f;
        const float y = t + ((b - t) - h) * 0.5f;
        l = x;
        t = y;
        r = x + w;
        b = y + h;
        radius = std::min(radius * 0.35f, std::min(w, h) * 0.5f);
    }
    const float scale = std::max((r - l) / image.width, (b - t) / image.height);
    const float sourceX = (image.width - (r - l) / scale) * 0.5f;
    const float sourceY = (image.height - (b - t) / scale) * 0.5f;
    auto sample = [&](float u, float v) {
        u = std::clamp(u, 0.0f, image.width - 1.0f);
        v = std::clamp(v, 0.0f, image.height - 1.0f);
        const int x0 = static_cast<int>(u);
        const int y0 = static_cast<int>(v);
        const int x1 = std::min(x0 + 1, image.width - 1);
        const int y1 = std::min(y0 + 1, image.height - 1);
        const float fx = u - x0;
        const float fy = v - y0;
        auto px = [&](int x, int y) {
            return image.pixels[static_cast<size_t>(y) * image.width + x];
        };
        auto channel = [&](int shift) {
            const float top = ((px(x0, y0) >> shift) & 0xFF) * (1 - fx) +
                              ((px(x1, y0) >> shift) & 0xFF) * fx;
            const float bottom = ((px(x0, y1) >> shift) & 0xFF) * (1 - fx) +
                                 ((px(x1, y1) >> shift) & 0xFF) * fx;
            return (top * (1 - fy) + bottom * fy) / 255.0f;
        };
        return Color{channel(16), channel(8), channel(0), 1.0f};
    };
    FillShape(
        s, l, t, r, b,
        [&](float x, float y) { return SdRoundRect(x, y, l, t, r, b, radius); },
        [&](float x, float y) {
            return sample(sourceX + (x - l) / scale - 0.5f,
                          sourceY + (y - t) / scale - 0.5f);
        });
}

////////////////////////////////////////////////////////////////////////////////
// Text

class FontCache {
  public:
    void Init() {
        static const PCWSTR kUiFaces[] = {L"Segoe UI Variable Text",
                                          L"Segoe UI", L"Tahoma"};
        static const PCWSTR kMonoFaces[] = {L"Cascadia Mono", L"Consolas",
                                            L"Courier New"};
        static const PCWSTR kIconFaces[] = {L"Segoe Fluent Icons",
                                            L"Segoe MDL2 Assets"};
        face_ = PickFace(kUiFaces, ARRAYSIZE(kUiFaces), true);
        monoFace_ = PickFace(kMonoFaces, ARRAYSIZE(kMonoFaces), true);
        iconFace_ = PickFace(kIconFaces, ARRAYSIZE(kIconFaces), false);
        iconGlyphs_.clear();
    }

    HFONT Get(int px, int weight, bool mono = false) {
        return Find(px, weight, mono ? 1 : 0);
    }

    HFONT Icons(int px) {
        return iconFace_.empty() ? nullptr : Find(px, FW_NORMAL, 2);
    }

    bool HasIcon(wchar_t glyph) {
        if (iconFace_.empty()) {
            return false;
        }
        for (const auto& known : iconGlyphs_) {
            if (known.first == glyph) {
                return known.second;
            }
        }
        bool has = false;
        if (HDC dc = CreateCompatibleDC(nullptr)) {
            HGDIOBJ old = SelectObject(dc, Icons(16));
            WORD index = 0xFFFF;
            has = GetGlyphIndicesW(dc, &glyph, 1, &index,
                                   GGI_MARK_NONEXISTING_GLYPHS) != GDI_ERROR &&
                  index != 0xFFFF;
            SelectObject(dc, old);
            DeleteDC(dc);
        }
        iconGlyphs_.push_back({glyph, has});
        return has;
    }

    void Clear() {
        for (const Entry& entry : entries_) {
            DeleteObject(entry.font);
        }
        entries_.clear();
    }

  private:
    struct Entry {
        int px;
        int weight;
        int kind;  // 0 = text, 1 = monospace, 2 = icons.
        HFONT font;
    };

    HFONT Find(int px, int weight, int kind) {
        px = std::max(px, 6);
        for (const Entry& entry : entries_) {
            if (entry.px == px && entry.weight == weight && entry.kind == kind) {
                return entry.font;
            }
        }
        const std::wstring& face = kind == 2 ? iconFace_ : kind == 1 ? monoFace_ : face_;
        HFONT font = CreateFontW(-px, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_TT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                 DEFAULT_PITCH | FF_DONTCARE, face.c_str());
        if (font) {
            entries_.push_back({px, weight, kind, font});
        }
        return font ? font : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    }

    static int CALLBACK EnumFontProc(const LOGFONTW*,
                                     const TEXTMETRICW*,
                                     DWORD,
                                     LPARAM param) {
        *reinterpret_cast<bool*>(param) = true;
        return 0;
    }

    static std::wstring PickFace(const PCWSTR* faces, size_t count, bool fallbackToLast) {
        HDC dc = GetDC(nullptr);
        for (size_t i = 0; i < count; i++) {
            LOGFONTW lf{};
            lf.lfCharSet = DEFAULT_CHARSET;
            wcsncpy_s(lf.lfFaceName, faces[i], _TRUNCATE);
            bool found = false;
            EnumFontFamiliesExW(dc, &lf, EnumFontProc,
                                reinterpret_cast<LPARAM>(&found), 0);
            if (found) {
                ReleaseDC(nullptr, dc);
                return faces[i];
            }
        }
        ReleaseDC(nullptr, dc);
        return fallbackToLast ? faces[count - 1] : L"";
    }

    std::vector<Entry> entries_;
    std::vector<std::pair<wchar_t, bool>> iconGlyphs_;  // Which glyphs exist.
    std::wstring face_ = L"Segoe UI";
    std::wstring monoFace_ = L"Consolas";
    std::wstring iconFace_;
};

FontCache g_fonts;

struct WrappedTextLine {
    size_t start;
    size_t length;
    std::vector<int> extents;
};

class TextPainter {
  public:
    SIZE Measure(HFONT font, const std::wstring& text, int maxWidth, UINT flags) {
        EnsureMeasureDc();
        HGDIOBJ old = SelectObject(measureDc_, font);
        RECT rc{0, 0, std::max(maxWidth, 1), 0};
        DrawTextW(measureDc_, text.empty() ? L" " : text.c_str(),
                  text.empty() ? 1 : static_cast<int>(text.size()), &rc,
                  flags | DT_CALCRECT | DT_NOPREFIX);
        SelectObject(measureDc_, old);
        return {rc.right - rc.left, rc.bottom - rc.top};
    }

    int Width(HFONT font, const wchar_t* text, int length) {
        if (length <= 0) {
            return 0;
        }
        EnsureMeasureDc();
        HGDIOBJ old = SelectObject(measureDc_, font);
        SIZE size{};
        GetTextExtentPoint32W(measureDc_, text, length, &size);
        SelectObject(measureDc_, old);
        return size.cx;
    }

    void Extents(HFONT font, const std::wstring& text, std::vector<int>& out) {
        out.assign(text.size(), 0);
        if (text.empty()) {
            return;
        }
        EnsureMeasureDc();
        HGDIOBJ old = SelectObject(measureDc_, font);
        SIZE size{};
        GetTextExtentExPointW(measureDc_, text.c_str(),
                              static_cast<int>(text.size()), 0, nullptr,
                              out.data(), &size);
        SelectObject(measureDc_, old);
    }

    std::vector<WrappedTextLine> Wrap(HFONT font, const std::wstring& text, int width) {
        std::vector<WrappedTextLine> lines;
        size_t start = 0;
        while (start < text.size()) {
            const size_t newline = text.find(L'\n', start);
            const size_t end = newline == std::wstring::npos ? text.size() : newline;
            if (start == end) {
                lines.push_back({start, 0, {}});
                start++;
                continue;
            }
            std::vector<int> extents;
            Extents(font, text.substr(start, end - start), extents);
            size_t fit = 0;
            while (fit < extents.size() && extents[fit] <= std::max(width, 1)) {
                fit++;
            }
            if (fit < extents.size()) {
                if (fit > 0 && IS_HIGH_SURROGATE(text[start + fit - 1]) &&
                    IS_LOW_SURROGATE(text[start + fit])) {
                    fit--;
                }
                size_t word = fit;
                while (word > 0 && !iswspace(text[start + word - 1])) {
                    word--;
                }
                if (word > 0) {
                    fit = word;
                }
            }
            if (fit == 0) {
                fit = IS_HIGH_SURROGATE(text[start]) && start + 1 < end &&
                              IS_LOW_SURROGATE(text[start + 1]) ? 2 : 1;
            }
            fit = std::min(fit, end - start);
            extents.resize(fit);
            lines.push_back({start, fit, std::move(extents)});
            start += fit;
            if (start == end && newline != std::wstring::npos) {
                start++;
            }
        }
        return lines;
    }

    void Draw(Surface& dst,
              HFONT font,
              const std::wstring& text,
              RECT layout,
              RECT clip,
              UINT flags,
              Color color) {
        RECT bounds{0, 0, dst.Width(), dst.Height()};
        IntersectRect(&clip, &clip, &bounds);
        const int w = clip.right - clip.left;
        const int h = clip.bottom - clip.top;
        if (w <= 0 || h <= 0 || text.empty() || !mask_.Resize(w, h)) {
            return;
        }
        HDC dc = mask_.Dc();
        HGDIOBJ old = SelectObject(dc, font);
        SetTextColor(dc, RGB(255, 255, 255));
        SetBkMode(dc, TRANSPARENT);
        OffsetRect(&layout, -clip.left, -clip.top);
        DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &layout,
                  flags | DT_NOPREFIX);
        SelectObject(dc, old);
        GdiFlush();

        const float luminance = color.r * 0.3f + color.g * 0.59f + color.b * 0.11f;
        const float gamma = luminance < 0.5f ? 0.8f : 1.0f;
        float lut[256];
        for (int i = 0; i < 256; i++) {
            lut[i] = powf(i / 255.0f, gamma);
        }
        for (int y = 0; y < h; y++) {
            const uint32_t* src = mask_.Row(y);
            uint32_t* row = dst.Row(clip.top + y) + clip.left;
            for (int x = 0; x < w; x++) {
                const uint32_t m = src[x] & 0xFF;
                if (m) {
                    BlendColor(row[x], color, lut[m]);
                }
            }
        }
    }

    void Draw(Surface& dst,
              HFONT font,
              const std::wstring& text,
              RECT layout,
              UINT flags,
              Color color) {
        Draw(dst, font, text, layout, layout, flags, color);
    }

    ~TextPainter() {
        if (measureDc_) {
            DeleteDC(measureDc_);
        }
    }

  private:
    void EnsureMeasureDc() {
        if (!measureDc_) {
            measureDc_ = CreateCompatibleDC(nullptr);
        }
    }

    Surface mask_;
    HDC measureDc_ = nullptr;
};

////////////////////////////////////////////////////////////////////////////////
// Icons, drawn on a 16x16 design grid centered at (cx, cy)

enum class Icon { Close, Copy, Search, External, Send, Plus, Translate };

void DrawIcon(Surface& s, Icon icon, float cx, float cy, float scale, Color c) {
    const float u = scale;  // One design unit.
    auto P = [&](float x, float y) { return Vec2{cx + (x - 8) * u, cy + (y - 8) * u}; };
    const float w = 1.6f * u;
    switch (icon) {
        case Icon::Close:
            StrokeLine(s, P(4, 4), P(12, 12), w, c);
            StrokeLine(s, P(12, 4), P(4, 12), w, c);
            break;
        case Icon::Copy: {
            Vec2 a = P(5.5f, 5.5f);
            Vec2 b = P(14, 14);
            StrokeRoundRect(s, a.x, a.y, b.x, b.y, 2.0f * u, w * 0.9f, c);
            StrokeLine(s, P(2, 10.5f), P(2, 4), w * 0.9f, c);
            StrokeLine(s, P(4, 2), P(10.5f, 2), w * 0.9f, c);
            StrokeLine(s, P(2, 4), P(4, 2), w * 0.9f, c);
            break;
        }
        case Icon::Search: {
            Vec2 center = P(7, 7);
            StrokeCircle(s, center.x, center.y, 4.6f * u, w, c);
            StrokeLine(s, P(10.4f, 10.4f), P(14, 14), w * 1.1f, c);
            break;
        }
        case Icon::External:
            StrokeLine(s, P(4, 12), P(12, 4), w, c);
            StrokeLine(s, P(6, 4), P(12, 4), w, c);
            StrokeLine(s, P(12, 4), P(12, 10), w, c);
            break;
        case Icon::Send: {
            const Vec2 points[] = {P(2.5f, 2.5f), P(14.5f, 8), P(2.5f, 13.5f),
                                   P(5.2f, 8)};
            FillPolygon(s, points, 4, 0.6f * u, c);
            break;
        }
        case Icon::Plus:
            StrokeLine(s, P(8, 2.5f), P(8, 13.5f), w, c);
            StrokeLine(s, P(2.5f, 8), P(13.5f, 8), w, c);
            break;
        case Icon::Translate:
            StrokeLine(s, P(1, 14), P(4.5f, 6), w * 0.85f, c);
            StrokeLine(s, P(4.5f, 6), P(8, 14), w * 0.85f, c);
            StrokeLine(s, P(2.5f, 11), P(6.5f, 11), w * 0.85f, c);
            StrokeLine(s, P(12, 1.8f), P(12, 3.4f), w * 0.85f, c);
            StrokeLine(s, P(9.2f, 4.4f), P(14.8f, 4.4f), w * 0.85f, c);
            StrokeLine(s, P(10.6f, 4.4f), P(13.8f, 9), w * 0.85f, c);
            StrokeLine(s, P(13.4f, 4.4f), P(10.2f, 9), w * 0.85f, c);
            break;
    }
}

enum class Glyph : wchar_t {
    Close = 0xE711,
    Copy = 0xE8C8,
    Search = 0xE721,
    OpenInNew = 0xE8A7,
    Send = 0xE725,
    Add = 0xE710,
    Camera = 0xE722,
    Text = 0xE8D2,
    Translate = 0xE8C1,
};

void DrawGlyph(Surface& s,
               TextPainter& text,
               Glyph glyph,
               float cx,
               float cy,
               float scale,
               Color c) {
    const int px = static_cast<int>(lroundf(16 * scale));
    if (g_fonts.HasIcon(static_cast<wchar_t>(glyph))) {
        HFONT font = g_fonts.Icons(px);
        const LONG x = lroundf(cx);
        const LONG y = lroundf(cy);
        const RECT rc{x - px, y - px, x + px, y + px};
        text.Draw(s, font, std::wstring(1, static_cast<wchar_t>(glyph)), rc,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE, c);
        return;
    }
    Icon icon = Icon::Copy;
    switch (glyph) {
        case Glyph::Close:
            icon = Icon::Close;
            break;
        case Glyph::Search:
        case Glyph::Camera:
            icon = Icon::Search;
            break;
        case Glyph::OpenInNew:
            icon = Icon::External;
            break;
        case Glyph::Send:
            icon = Icon::Send;
            break;
        case Glyph::Add:
            icon = Icon::Plus;
            break;
        case Glyph::Translate:
            icon = Icon::Translate;
            break;
        case Glyph::Copy:
        case Glyph::Text:
            icon = Icon::Copy;
            break;
    }
    DrawIcon(s, icon, cx, cy, scale, c);
}

////////////////////////////////////////////////////////////////////////////////
// Layered windows

HINSTANCE g_instance;

constexpr WCHAR kMessageWindowClass[] = L"MagicPointerMessages_" WH_MOD_ID;
constexpr WCHAR kLayerWindowClass[] = L"MagicPointerLayer_" WH_MOD_ID;
constexpr WCHAR kOverlayWindowClass[] = L"MagicPointerOverlay_" WH_MOD_ID;
constexpr WCHAR kCardWindowClass[] = L"MagicPointerCard_" WH_MOD_ID;

bool g_layersChanged;

int g_styleVersion = 1;

void ConfigurePixelWindow(HWND hwnd, const Color* glass = nullptr);

class LayeredWindow {
  public:
    bool Create(bool clickThrough,
                bool activatable,
                PCWSTR className,
                void* param) {
        DWORD exStyle = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
        if (!activatable) {
            exStyle |= WS_EX_NOACTIVATE;
        }
        if (clickThrough) {
            exStyle |= WS_EX_TRANSPARENT;
        }
        hwnd_ = CreateWindowExW(exStyle, className, L"Magic Pointer", WS_POPUP,
                                0, 0, 1, 1, nullptr, nullptr, g_instance,
                                param);
        if (hwnd_) ConfigurePixelWindow(hwnd_);
        return hwnd_ != nullptr;
    }

    void Destroy() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
        visible_ = false;
    }

    bool Present(Surface& surface, int x, int y, BYTE alpha) {
        if (!hwnd_ || !surface.Valid()) {
            return false;
        }
        POINT dst{x, y};
        SIZE size{surface.Width(), surface.Height()};
        POINT src{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        GdiFlush();  // Finish GDI text/drawing before uploading the DIB pixels.
        if (!UpdateLayeredWindow(hwnd_, nullptr, &dst, &size, surface.Dc(),
                                 &src, 0, &blend, ULW_ALPHA)) {
            return false;
        }
        x_ = x;
        y_ = y;
        width_ = size.cx;
        height_ = size.cy;
        alpha_ = alpha;
        return true;
    }

    void Move(int x, int y) {
        if (hwnd_ && (x != x_ || y != y_)) {
            SetWindowPos(hwnd_, nullptr, x, y, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                             SWP_NOOWNERZORDER);
            x_ = x;
            y_ = y;
        }
    }

    void SetAlpha(BYTE alpha) {
        if (hwnd_ && alpha != alpha_) {
            BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
            UpdateLayeredWindow(hwnd_, nullptr, nullptr, nullptr, nullptr,
                                nullptr, 0, &blend, ULW_ALPHA);
            alpha_ = alpha;
        }
    }

    void Show(bool activate = false) {
        if (hwnd_ && !visible_) {
            SetWindowPos(hwnd_, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW |
                             (activate ? 0 : SWP_NOACTIVATE));
            visible_ = true;
            g_layersChanged = true;
        }
    }

    void Hide() {
        if (hwnd_ && visible_) {
            ShowWindow(hwnd_, SW_HIDE);
            visible_ = false;
        }
    }

    HWND Hwnd() const { return hwnd_; }
    bool Visible() const { return visible_; }
    int X() const { return x_; }
    int Y() const { return y_; }
    int Width() const { return width_; }
    int Height() const { return height_; }
    BYTE Alpha() const { return alpha_; }
    void SetPosition(int x, int y) {
        x_ = x;
        y_ = y;
    }

  private:
    HWND hwnd_ = nullptr;
    bool visible_ = false;
    int x_ = 0;
    int y_ = 0;
    int width_ = 0;
    int height_ = 0;
    BYTE alpha_ = 255;
};

void WaitForComposition() {
    using DwmFlush_t = HRESULT(WINAPI*)();
    static auto dwmFlush = GetProc<DwmFlush_t>(L"dwmapi.dll", "DwmFlush");
    if (dwmFlush && SUCCEEDED(dwmFlush())) {
        dwmFlush();
    } else {
        Sleep(30);
    }
}

inline uint32_t ScalePremultiplied(uint32_t pixel, uint32_t alpha) {
    uint32_t rb = (pixel & 0x00FF00FF) * alpha + 0x00800080;
    rb = ((rb + ((rb >> 8) & 0x00FF00FF)) >> 8) & 0x00FF00FF;
    uint32_t ag = ((pixel >> 8) & 0x00FF00FF) * alpha + 0x00800080;
    ag = (ag + ((ag >> 8) & 0x00FF00FF)) & 0xFF00FF00;
    return rb | ag;
}

////////////////////////////////////////////////////////////////////////////////
// Glass

enum class Backdrop { None, Blur };

struct AccentPolicy {
    int state;
    int flags;
    DWORD gradientColor;  // 0xAABBGGRR.
    int animationId;
};

struct WindowCompositionAttributeData {
    int attribute;
    PVOID data;
    SIZE_T size;
};

using SetWindowCompositionAttribute_t =
    BOOL(WINAPI*)(HWND, WindowCompositionAttributeData*);

SetWindowCompositionAttribute_t GetSetWindowCompositionAttribute() {
    static auto function = GetProc<SetWindowCompositionAttribute_t>(
        L"user32.dll", "SetWindowCompositionAttribute");
    return function;
}

bool TransparencyEnabled() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value != 0;
}

Backdrop ChooseBackdrop() {
    return g_settings.glass && IsWindows10Build(17134) && TransparencyEnabled()
               ? Backdrop::Blur : Backdrop::None;
}

void SetDwmAttribute(HWND hwnd, DWORD attribute, const void* value, DWORD size) {
    using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
    static auto setAttribute = GetProc<DwmSetWindowAttribute_t>(
        L"dwmapi.dll", "DwmSetWindowAttribute");
    if (setAttribute) {
        setAttribute(hwnd, attribute, value, size);
    }
}

void ConfigurePixelWindow(HWND hwnd, const Color* glass) {
    const DWORD nonClient = 1;  // DWMNCRP_DISABLED.
    SetDwmAttribute(hwnd, 2 /* DWMWA_NCRENDERING_POLICY */, &nonClient, sizeof(nonClient));
    const BOOL noTransitions = TRUE;
    SetDwmAttribute(hwnd, 3 /* DWMWA_TRANSITIONS_FORCEDISABLED */, &noTransitions, sizeof(noTransitions));
    if (IsWindows11()) {
        const COLORREF border = 0xFFFFFFFE;  // DWMWA_COLOR_NONE.
        SetDwmAttribute(hwnd, 34 /* DWMWA_BORDER_COLOR */, &border, sizeof(border));
    }
    if (IsWindows10Build(22621)) {
        const DWORD backdrop = 1;  // DWMSBT_NONE.
        SetDwmAttribute(hwnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &backdrop, sizeof(backdrop));
    }
    if (auto setAttribute = GetSetWindowCompositionAttribute()) {
        auto channel = [](float v) { return static_cast<DWORD>(v * 255.0f + 0.5f); };
        AccentPolicy policy{glass ? 4 /* ACCENT_ENABLE_ACRYLICBLURBEHIND */ : 0, 0,
                            glass ? 0x01000000 | channel(glass->b) << 16 |
                                        channel(glass->g) << 8 | channel(glass->r)
                                  : 0,
                            0};
        WindowCompositionAttributeData data{19 /* WCA_ACCENT_POLICY */, &policy,
                                            sizeof(policy)};
        setAttribute(hwnd, &data);
    }
    if (IsWindows11()) {
        const DWORD corners = glass ? 2 : 1;  // DWMWCP_ROUND, DWMWCP_DONOTROUND.
        SetDwmAttribute(hwnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &corners, sizeof(corners));
    }
}

uint32_t SampleOpaqueSurface(const Surface& surface, float x, float y) {
    x = std::clamp(x, 0.0f, static_cast<float>(surface.Width() - 1));
    y = std::clamp(y, 0.0f, static_cast<float>(surface.Height() - 1));
    const int x0 = static_cast<int>(x), y0 = static_cast<int>(y);
    const int x1 = std::min(x0 + 1, surface.Width() - 1);
    const int y1 = std::min(y0 + 1, surface.Height() - 1);
    const uint32_t fx = static_cast<uint32_t>((x - x0) * 256.0f + 0.5f);
    const uint32_t fy = static_cast<uint32_t>((y - y0) * 256.0f + 0.5f);
    auto lerp = [](uint32_t a, uint32_t b, uint32_t weight) {
        const uint32_t rb = (((a & 0x00FF00FF) * (256 - weight) +
                              (b & 0x00FF00FF) * weight) >> 8) & 0x00FF00FF;
        const uint32_t g = (((a & 0x0000FF00) * (256 - weight) +
                             (b & 0x0000FF00) * weight) >> 8) & 0x0000FF00;
        return 0xFF000000 | rb | g;
    };
    return lerp(lerp(surface.Row(y0)[x0], surface.Row(y0)[x1], fx),
                lerp(surface.Row(y1)[x0], surface.Row(y1)[x1], fx), fy);
}

Color GlassTint(bool dark) {
    return dark ? Rgb(25, 26, 32, 0.80f) : Rgb(249, 249, 252, 0.82f);
}

class GlassPanel {
  public:
    bool Create(bool activatable, bool clickThrough, PCWSTR className, void* param) {
        if (panel_.Hwnd()) {
            return true;
        }
        if (!underlay_.Hwnd() &&
            !underlay_.Create(true, false, kLayerWindowClass, nullptr)) {
            return false;
        }
        if (!panel_.Create(clickThrough, activatable, className, param)) {
            return false;
        }
        StyleWindow();
        return true;
    }

    void Destroy() {
        panel_.Destroy();
        underlay_.Destroy();
    }

    void Configure(bool dark) {
        backdrop_ = ChooseBackdrop();
        dark_ = dark;
        if (panel_.Hwnd()) {
            StyleWindow();
        }
    }

    Backdrop Material() const { return backdrop_; }
    bool Dark() const { return dark_; }

    float Radius(float scale) const { return 8.0f * scale; }
    BYTE Alpha() const { return panel_.Alpha(); }

    void Present(Surface& content, int x, int y, BYTE alpha = 255) {
        x_ = x;
        y_ = y;
        panel_.Present(content, x, y, alpha);
        underlay_.Move(x - margin_, y - margin_);
    }

    void PresentUnderlay(Surface& underlay, int margin, BYTE alpha) {
        margin_ = margin;
        underlay_.Present(underlay, x_ - margin, y_ - margin, alpha);
    }

    void SetAlpha(BYTE alpha) { panel_.SetAlpha(alpha); }
    void SetUnderlayAlpha(BYTE alpha) { underlay_.SetAlpha(alpha); }

    void Move(int x, int y) {
        if (x == x_ && y == y_) {
            return;
        }
        x_ = x;
        y_ = y;
        constexpr UINT kFlags = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE;
        HDWP defer = BeginDeferWindowPos(2);
        if (defer) {
            defer = DeferWindowPos(defer, panel_.Hwnd(), nullptr, x, y, 0, 0, kFlags);
        }
        if (defer) {
            defer = DeferWindowPos(defer, underlay_.Hwnd(), nullptr, x - margin_,
                                   y - margin_, 0, 0, kFlags);
        }
        if (defer) {
            EndDeferWindowPos(defer);
        }
        panel_.SetPosition(x, y);
        underlay_.SetPosition(x - margin_, y - margin_);
    }

    void FollowPanel(int x, int y) {
        x_ = x;
        y_ = y;
        panel_.SetPosition(x, y);
        underlay_.Move(x - margin_, y - margin_);
    }

    void Show(bool activate) {
        panel_.Show(activate);
        underlay_.Show();
        SetWindowPos(underlay_.Hwnd(), panel_.Hwnd(), 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    void Hide() {
        panel_.Hide();
        underlay_.Hide();
    }

    bool Visible() const { return panel_.Visible(); }
    HWND Hwnd() const { return panel_.Hwnd(); }
    HWND UnderlayHwnd() const { return underlay_.Hwnd(); }
    int X() const { return x_; }
    int Y() const { return y_; }

  private:
    void StyleWindow() {
        const Color tint = GlassTint(dark_);
        ConfigurePixelWindow(panel_.Hwnd(), backdrop_ == Backdrop::Blur ? &tint : nullptr);
    }

    LayeredWindow panel_;
    LayeredWindow underlay_;
    Backdrop backdrop_ = Backdrop::None;
    bool dark_ = false;
    int x_ = 0;
    int y_ = 0;
    int margin_ = 0;
};

void PaintPanelBase(Surface& s, const GlassPanel& panel, float scale) {
    const float w = static_cast<float>(s.Width());
    const float h = static_cast<float>(s.Height());
    const bool dark = panel.Dark();
    const float radius = panel.Radius(scale);
    if (panel.Material() == Backdrop::Blur) {
        FillRoundRect(s, 0, 0, w, h, radius, GlassTint(dark));
    } else {
        FillRoundRectGradient(s, 0, 0, w, h, radius,
                              dark ? Rgb(40, 41, 47, 0.985f) : Rgb(252, 252, 253, 0.985f),
                              dark ? Rgb(32, 33, 38, 0.985f) : Rgb(244, 245, 248, 0.985f));
    }
    StrokeRoundRect(s, 0.5f, 0.5f, w - 0.5f, h - 0.5f, radius - 0.5f, 0.8f,
                    dark ? WithAlpha(kWhite, 0.14f) : WithAlpha(kBlack, 0.10f));
}

std::vector<float> BlurCoverage(std::vector<float> mask, int width, int height, float sigma);

void PaintPanelUnderlay(Surface& s,
                        int w,
                        int h,
                        int margin,
                        float radius,
                        float scale,
                        bool dark,
                        float glow,
                        float phase) {
    if (!s.Resize(w + margin * 2, h + margin * 2)) return;
    const float l = static_cast<float>(margin);
    const float t = static_cast<float>(margin);
    const float r = l + w;
    const float b = t + h;
    const int width = s.Width(), height = s.Height();
    std::vector<float> body(static_cast<size_t>(width) * height, 0);
    std::vector<float> emission(body.size(), 0);
    const float emitterWidth = 1.8f * scale;
    for (int y = margin; y < margin + h; y++) {
        const float v = Clamp01((y + 0.5f - t) / h);
        const float weight = 0.12f + 0.88f * v * v * (3.0f - 2.0f * v);
        for (int x = margin; x < margin + w; x++) {
            const float distance = SdRoundRect(x + 0.5f, y + 0.5f, l, t, r, b, radius);
            const float coverage = Clamp01(0.5f - distance);
            const size_t index = static_cast<size_t>(y) * width + x;
            body[index] = coverage;
            emission[index] = coverage * Clamp01(emitterWidth + distance + 0.5f) * weight;
        }
    }
    const float tightSigma = 3.8f * scale, softSigma = 9.0f * scale, wideSigma = 17.0f * scale;
    const auto tight = BlurCoverage(emission, width, height, tightSigma);
    const auto soft = BlurCoverage(emission, width, height, softSigma);
    const auto wide = BlurCoverage(emission, width, height, wideSigma);
    const float normalize = 2.506628f / emitterWidth;
    const auto shadow = BlurCoverage(body, width, height, 16.0f * scale);
    const auto contact = BlurCoverage(body, width, height, 2.0f * scale);
    const int shadowOffset = static_cast<int>(lroundf(6 * scale));
    const int contactOffset = static_cast<int>(lroundf(2 * scale));
    const float cx = (l + r) * 0.5f;
    const float cy = (t + b) * 0.5f;
    for (int y = 0; y < height; y++) {
        uint32_t* row = s.Row(y);
        const float py = y + 0.5f;
        for (int x = 0; x < width; x++) {
            const float px = x + 0.5f;
            const size_t index = static_cast<size_t>(y) * width + x;
            const float outside = 1.0f - body[index];
            if (outside <= 0) continue;
            const float light = Clamp01(glow * normalize * (
                0.46f * tightSigma * tight[index] + 0.18f * softSigma * soft[index] +
                0.07f * wideSigma * wide[index]));
            const float shadowAlpha = Clamp01(
                (y >= shadowOffset ? shadow[static_cast<size_t>(y - shadowOffset) * width + x] : 0) *
                    (dark ? 0.26f : 0.16f) +
                (y >= contactOffset ? contact[static_cast<size_t>(y - contactOffset) * width + x] : 0) *
                    (dark ? 0.16f : 0.10f));
            const float a = (light + shadowAlpha * (1 - light)) * outside;
            if (a <= 0) continue;
            const float hue = g_glow.rainbow
                ? FastAtan2(py - cy, (px - cx) * (b - t) / (r - l)) / (2 * kPi) + phase : 0;
            const Color color = g_glow.At(hue);
            uint32_t noise = static_cast<uint32_t>(x) * 0x9E3779B9u ^ static_cast<uint32_t>(y) * 0x85EBCA6Bu;
            noise ^= noise >> 16;
            noise *= 0x7FEB352Du;
            noise ^= noise >> 15;
            const float threshold = ((noise & 65535) + 0.5f) / 65536.0f;
            const uint32_t alpha = static_cast<uint32_t>(a * 255 + threshold);
            auto channel = [&](float c) {
                return std::min(alpha, static_cast<uint32_t>(Clamp01(c * light * outside) * 255 + threshold));
            };
            row[x] = (alpha << 24) | (channel(color.r) << 16) | (channel(color.g) << 8) | channel(color.b);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// The Magic Pointer

struct PointerFrame {
    float scale;    // Pixels per design unit.
    float phase;    // Drift of the glow colors, 0..1.
    float pop;      // Entrance of the sparkle, 0..1.
    float glow;     // Strength of the glow, 0..1.
    float breathe;  // Gentle pulse, 0..1, wraps.
};

struct ArrowGeometry {
    Vec2 points[4];  // Tip, right wing, notch, left wing.
    Vec2 center;
    Vec2 axis;
    float length;
};

const ArrowGeometry& GetArrow() {
    static const ArrowGeometry arrow = [] {
        const float angle = 36.0f * kPi / 180.0f;
        const Vec2 along{sinf(angle), cosf(angle)};
        const Vec2 across{cosf(angle), -sinf(angle)};
        const float length = 22.0f;
        const float halfWidth = 9.6f;
        const float notch = 0.72f;
        ArrowGeometry g;
        g.points[0] = {0, 0};
        g.points[1] = {along.x * length + across.x * halfWidth,
                       along.y * length + across.y * halfWidth};
        g.points[2] = {along.x * length * notch, along.y * length * notch};
        g.points[3] = {along.x * length - across.x * halfWidth,
                       along.y * length - across.y * halfWidth};
        g.center = {along.x * length * 0.55f, along.y * length * 0.55f};
        g.axis = along;
        g.length = length;
        return g;
    }();
    return arrow;
}

constexpr float kArrowRound = 2.3f;
constexpr float kGlowTight = 2.8f;
constexpr float kGlowWide = 8.5f;
constexpr Vec2 kSparkleCenter{11.5f, -9.5f};
constexpr float kSparkleRadius = 7.2f;
constexpr float kSparkleGlowSigma = 4.6f;

std::vector<float> BlurCoverage(std::vector<float> mask, int width, int height, float sigma) {
    int lower = std::max(1, static_cast<int>(floorf(sqrtf(4.0f * sigma * sigma + 1.0f))));
    if (!(lower & 1)) {
        lower--;
    }
    const int lowerPasses = std::clamp(static_cast<int>(lroundf(
        (12.0f * sigma * sigma - 3.0f * lower * lower - 12.0f * lower - 9.0f) /
        (-4.0f * lower - 4.0f))), 0, 3);
    std::vector<float> horizontal(mask.size());
    std::vector<float> columnSums(width);
    for (int pass = 0; pass < 3; pass++) {
        const int diameter = pass < lowerPasses ? lower : lower + 2;
        const int radius = diameter / 2;
        for (int y = 0; y < height; y++) {
            const size_t row = static_cast<size_t>(y) * width;
            float sum = 0;
            for (int x = 0; x <= std::min(radius, width - 1); x++) {
                sum += mask[row + x];
            }
            for (int x = 0; x < width; x++) {
                horizontal[row + x] = std::max(0.0f, sum / diameter);
                if (x - radius >= 0) sum -= mask[row + x - radius];
                if (x + radius + 1 < width) sum += mask[row + x + radius + 1];
            }
        }
        std::fill(columnSums.begin(), columnSums.end(), 0.0f);
        for (int y = 0; y <= std::min(radius, height - 1); y++) {
            const float* row = horizontal.data() + static_cast<size_t>(y) * width;
            for (int x = 0; x < width; x++) columnSums[x] += row[x];
        }
        for (int y = 0; y < height; y++) {
            float* row = mask.data() + static_cast<size_t>(y) * width;
            const float* leaving = y >= radius
                ? horizontal.data() + static_cast<size_t>(y - radius) * width : nullptr;
            const float* entering = y + radius + 1 < height
                ? horizontal.data() + static_cast<size_t>(y + radius + 1) * width : nullptr;
            for (int x = 0; x < width; x++) {
                row[x] = std::max(0.0f, columnSums[x] / diameter);
                if (leaving) columnSums[x] -= leaving[x];
                if (entering) columnSums[x] += entering[x];
            }
        }
    }
    return mask;
}

void RenderPointer(const PointerFrame& f,
                   std::vector<uint32_t>& pixels,
                   int& width,
                   int& height,
                   int& hotX,
                   int& hotY) {
    const ArrowGeometry& arrow = GetArrow();
    const float glowReach = kGlowWide * 3.2f;
    const float sparkleReach = kSparkleRadius * 1.25f + kSparkleGlowSigma * 3.2f;

    float minX = 0, minY = 0, maxX = 0, maxY = 0;
    for (const Vec2& p : arrow.points) {
        minX = std::min(minX, p.x);
        minY = std::min(minY, p.y);
        maxX = std::max(maxX, p.x);
        maxY = std::max(maxY, p.y);
    }
    minX -= kArrowRound + glowReach;
    minY -= kArrowRound + glowReach;
    maxX += kArrowRound + glowReach;
    maxY += kArrowRound + glowReach;
    minX = std::min(minX, kSparkleCenter.x - sparkleReach);
    minY = std::min(minY, kSparkleCenter.y - sparkleReach);
    maxX = std::max(maxX, kSparkleCenter.x + sparkleReach);
    maxY = std::max(maxY, kSparkleCenter.y + sparkleReach);

    const float s = std::min(f.scale, 253.0f / std::max(maxX - minX, maxY - minY));
    hotX = static_cast<int>(ceilf(-minX * s));
    hotY = static_cast<int>(ceilf(-minY * s));
    width = hotX + static_cast<int>(ceilf(maxX * s)) + 1;
    height = hotY + static_cast<int>(ceilf(maxY * s)) + 1;
    pixels.assign(static_cast<size_t>(width) * height, 0);

    const float pulse = sinf(2.0f * kPi * f.breathe);
    const float pop = static_cast<float>(EaseOutBack(f.pop));
    const float sparkleRadius = kSparkleRadius * pop * (1.0f + 0.025f * pulse);
    const float sparkleAngle =
        (1.0f - static_cast<float>(EaseOutCubic(f.pop))) * kPi * 0.5f +
        0.035f * sinf(2.0f * kPi * f.breathe);
    const float sparkleOpacity = Clamp01(f.pop * 2.5f);
    const float glow = f.glow * (0.62f + 0.38f * pulse);
    const float spread = 1.0f + 0.07f * pulse;
    const float tight = kGlowTight * spread;
    const float wide = kGlowWide * spread;
    const float sparkleSigma = kSparkleGlowSigma * spread;
    const Vec2 shadowOffset{0.65f, 1.7f};
    const Color shadow = WithAlpha(kBlack, 0.24f);
    const Color rim = WithAlpha(Mix(g_glow.Base(), kBlack, 0.60f), 0.28f);
    const Color bodyBottom = Rgb(242, 244, 248);

    auto coverage = [&](Vec2 p, float d, auto distance) {
        if (fabsf(d * s) >= 1.0f) {
            return Clamp01(0.5f - d * s);
        }
        float sum = 0;
        for (float oy : {-0.25f, 0.25f}) {
            for (float ox : {-0.25f, 0.25f}) {
                sum += Clamp01(0.5f - distance({p.x + ox / s, p.y + oy / s}) * s * 2.0f);
            }
        }
        return sum * 0.25f;
    };

    std::vector<float> sparkleMask(pixels.size(), 0);
    if (sparkleRadius > 0.05f) {
        const float reach = sparkleRadius * 1.25f + 1.0f / s;
        const int firstX = std::max(0, static_cast<int>(floorf(hotX + (kSparkleCenter.x - reach) * s)));
        const int lastX = std::min(width, static_cast<int>(ceilf(hotX + (kSparkleCenter.x + reach) * s)));
        const int firstY = std::max(0, static_cast<int>(floorf(hotY + (kSparkleCenter.y - reach) * s)));
        const int lastY = std::min(height, static_cast<int>(ceilf(hotY + (kSparkleCenter.y + reach) * s)));
        for (int y = firstY; y < lastY; y++) {
            for (int x = firstX; x < lastX; x++) {
                const Vec2 p{(x + 0.5f - hotX) / s, (y + 0.5f - hotY) / s};
                const float d = SdSparkle(p, kSparkleCenter.x, kSparkleCenter.y,
                                          sparkleRadius, sparkleAngle);
                sparkleMask[static_cast<size_t>(y) * width + x] = sparkleOpacity * coverage(p, d, [&](Vec2 q) {
                    return SdSparkle(q, kSparkleCenter.x, kSparkleCenter.y, sparkleRadius, sparkleAngle);
                });
            }
        }
    }
    const auto sparkleTight = BlurCoverage(sparkleMask, width, height, 1.1f * s);
    const auto sparkleWide = BlurCoverage(sparkleMask, width, height, sparkleSigma * s);
    const auto sparkleShadow = BlurCoverage(sparkleMask, width, height, 1.3f * s);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const Vec2 p{(x + 0.5f - hotX) / s, (y + 0.5f - hotY) / s};
            uint32_t px = 0;

            const float dArrow = SdPolygon(arrow.points, 4, p) - kArrowRound;
            const float dShadow =
                SdPolygon(arrow.points, 4,
                          {p.x - shadowOffset.x, p.y - shadowOffset.y}) - kArrowRound;
            BlendColor(px, shadow, SoftCoverage(dShadow, 1.8f));

            const size_t index = static_cast<size_t>(y) * width + x;
            const int shadowX = x - static_cast<int>(lroundf(0.4f * s));
            const int shadowY = y - static_cast<int>(lroundf(s));
            if (shadowX >= 0 && shadowY >= 0) {
                BlendColor(px, WithAlpha(kBlack, 0.19f),
                           sparkleShadow[static_cast<size_t>(shadowY) * width + shadowX]);
            }

            if (glow > 0.0f && dArrow < glowReach) {
                const float d = std::max(dArrow, 0.0f);
                const float a = glow *
                    (0.38f * expf(-0.5f * d * d / (tight * tight)) +
                     0.68f * SoftCoverage(dArrow, wide));
                const Color color = g_glow.rainbow
                    ? g_glow.At(FastAtan2(p.y - arrow.center.y, p.x - arrow.center.x) /
                                    (2.0f * kPi) + f.phase) : g_glow.Base();
                BlendColor(px, color, a);
            }

            const float starGlow = glow * (0.95f * sparkleTight[index] + 1.45f * sparkleWide[index]);
            if (starGlow > 0.001f) {
                const Color color = g_glow.rainbow
                    ? g_glow.At(FastAtan2(p.y - kSparkleCenter.y, p.x - kSparkleCenter.x) /
                                    (2.0f * kPi) + f.phase + 0.33f) : g_glow.Base();
                BlendColor(px, color, starGlow);
            }

            const float body = coverage(p, dArrow, [&](Vec2 q) {
                return SdPolygon(arrow.points, 4, q) - kArrowRound;
            });
            if (body > 0.0f) {
                const float along = Clamp01((p.x * arrow.axis.x + p.y * arrow.axis.y) /
                                            arrow.length);
                BlendColor(px, Mix(kWhite, bodyBottom, along), body);
                const float inner = coverage(p, dArrow + 0.55f, [&](Vec2 q) {
                    return SdPolygon(arrow.points, 4, q) - kArrowRound + 0.55f;
                });
                BlendColor(px, rim, std::max(0.0f, body - inner));
            }
            if (sparkleMask[index] > 0) {
                const float starCoverage = sparkleMask[index];
                if (starCoverage > 0.0f) {
                    const float r = sqrtf((p.x - kSparkleCenter.x) * (p.x - kSparkleCenter.x) +
                                          (p.y - kSparkleCenter.y) * (p.y - kSparkleCenter.y));
                    const float highlight = expf(-r * r /
                                                  (sparkleRadius * sparkleRadius * 0.23f));
                    const Color tint = Mix(g_glow.At(f.phase), kWhite,
                                           0.80f + 0.16f * highlight);
                    BlendColor(px, tint, starCoverage);
                }
            }
            pixels[static_cast<size_t>(y) * width + x] = px;
        }
    }
}

HCURSOR CreatePointerCursor(const PointerFrame& frame) {
    std::vector<uint32_t> pixels;
    int width, height, hotX, hotY;
    RenderPointer(frame, pixels, width, height, hotX, hotY);

    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP color =
        CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!color || !bits) {
        if (color) {
            DeleteObject(color);
        }
        return nullptr;
    }

    uint32_t* out = static_cast<uint32_t*>(bits);
    for (size_t i = 0; i < pixels.size(); i++) {
        const uint32_t p = pixels[i];
        const uint32_t a = p >> 24;
        if (!a) {
            out[i] = 0;
            continue;
        }
        auto channel = [&](int shift) {
            return std::min<uint32_t>(255, (((p >> shift) & 255) * 255 + a / 2) / a);
        };
        out[i] = (a << 24) | (channel(16) << 16) | (channel(8) << 8) | channel(0);
    }
    GdiFlush();

    const int maskStride = ((width + 15) / 16) * 2;
    std::vector<uint8_t> maskBits(static_cast<size_t>(maskStride) * height, 0);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (!(pixels[static_cast<size_t>(y) * width + x] >> 24)) {
                maskBits[static_cast<size_t>(y) * maskStride + x / 8] |= 0x80 >> (x & 7);
            }
        }
    }
    HBITMAP mask = CreateBitmap(width, height, 1, 1, maskBits.data());
    ICONINFO iconInfo{};
    iconInfo.fIcon = FALSE;
    iconInfo.xHotspot = static_cast<DWORD>(hotX);
    iconInfo.yHotspot = static_cast<DWORD>(hotY);
    iconInfo.hbmMask = mask;
    iconInfo.hbmColor = color;
    HCURSOR cursor = mask ? CreateIconIndirect(&iconInfo) : nullptr;
    DeleteObject(color);
    if (mask) {
        DeleteObject(mask);
    }
    return cursor;
}

class PointerCursors {
  public:
    ~PointerCursors() { Clear(); }

    void SetScale(float scale) {
        if (fabsf(scale - scale_) < 0.01f) {
            return;
        }
        Invalidate();
        scale_ = scale;
    }

    void Invalidate() {
        for (HCURSOR& cursor : loop_) {
            if (cursor) {
                retired_.push_back(cursor);
                cursor = nullptr;
            }
        }
    }

    void StartPop(double now) { popStart_ = now; }

    void ReleaseRetired(HCURSOR onScreen) {
        for (HCURSOR cursor : retired_) {
            if (cursor != onScreen) {
                DestroyCursor(cursor);
            }
        }
        retired_.clear();
    }

    HCURSOR Current(double now) {
        const double loopT = fmod(now, kLoopMs) / kLoopMs;
        const double sincePop = now - popStart_;
        if (sincePop >= 0 && sincePop < kPopMs) {
            PointerFrame frame{scale_, static_cast<float>(loopT),
                               static_cast<float>(sincePop / kPopMs),
                               static_cast<float>(EaseOutCubic(sincePop / 260.0)),
                               static_cast<float>(loopT)};
            if (HCURSOR cursor = CreatePointerCursor(frame)) {
                if (transient_) {
                    retired_.push_back(transient_);
                }
                transient_ = cursor;
                return cursor;
            }
        }

        if (transient_) {
            retired_.push_back(transient_);
            transient_ = nullptr;
        }
        const int index = static_cast<int>(loopT * kLoopFrames) % kLoopFrames;
        if (!loop_[index]) {
            const float t = static_cast<float>(index) / kLoopFrames;
            loop_[index] = CreatePointerCursor({scale_, t, 1.0f, 1.0f, t});
        }
        return loop_[index] ? loop_[index] : LoadCursorW(nullptr, IDC_ARROW);
    }

    double NextChange(double now) const {
        if (now - popStart_ < kPopMs) {
            return 15.0;
        }
        const double frameMs = kLoopMs / kLoopFrames;
        return frameMs - fmod(now, frameMs) + 0.5;
    }

    void Clear() {
        for (HCURSOR& cursor : loop_) {
            if (cursor) {
                DestroyCursor(cursor);
                cursor = nullptr;
            }
        }
        if (transient_) {
            DestroyCursor(transient_);
            transient_ = nullptr;
        }
        for (HCURSOR cursor : retired_) {
            DestroyCursor(cursor);
        }
        retired_.clear();
    }

  private:
    static constexpr int kLoopFrames = 192;
    static constexpr double kLoopMs = kGlowPeriodMs;
    static constexpr double kPopMs = 420.0;

    HCURSOR loop_[kLoopFrames] = {};
    HCURSOR transient_ = nullptr;
    std::vector<HCURSOR> retired_;
    float scale_ = 1.0f;
    double popStart_ = -1e9;
};

////////////////////////////////////////////////////////////////////////////////
// The hint next to the pointer

constexpr WCHAR kHintText[] = L"Select anything to ask Gemini";

class HintPill {
  public:
    void Show(POINT cursor, float scale, bool dark, double now) {
        if (!panel_.Create(false, true, kLayerWindowClass, nullptr)) {
            return;
        }
        if (fabsf(scale - scale_) > 0.01f || dark != panel_.Dark() ||
            styleVersion_ != g_styleVersion || !content_.Valid()) {
            scale_ = scale;
            styleVersion_ = g_styleVersion;
            panel_.Configure(dark);
            Render();
        }
        shownAt_ = now;
        state_ = State::In;
        const POINT pos = Placement(cursor);
        panel_.Present(content_, pos.x, pos.y, 0);
        panel_.PresentUnderlay(underlay_, margin_, 0);
        panel_.Show(false);
    }

    void Follow(POINT cursor) {
        if (state_ != State::Hidden) {
            const POINT pos = Placement(cursor);
            panel_.Move(pos.x, pos.y);
        }
    }

    void FadeOut(double now) {
        if (state_ == State::In || state_ == State::Shown) {
            state_ = State::Out;
            fadeOutAt_ = now;
        }
    }

    void HideNow() {
        state_ = State::Hidden;
        panel_.Hide();
    }

    double Tick(double now) {
        switch (state_) {
            case State::In: {
                const double t = (now - shownAt_) / 180.0;
                const BYTE alpha = static_cast<BYTE>(EaseOutCubic(t) * 255);
                panel_.SetUnderlayAlpha(static_cast<BYTE>(alpha * GlowBreath(now)));
                panel_.SetAlpha(alpha);
                if (t >= 1.0) {
                    state_ = State::Shown;
                    return 1000.0 / 60.0;
                }
                return 0;
            }
            case State::Out: {
                const double t = (now - fadeOutAt_) / 200.0;
                if (t >= 1.0) {
                    HideNow();
                    return 1e9;
                }
                const BYTE alpha = static_cast<BYTE>((1.0 - t) * 255);
                panel_.SetAlpha(alpha);
                panel_.SetUnderlayAlpha(alpha);
                return 0;
            }
            case State::Shown:
                panel_.SetUnderlayAlpha(static_cast<BYTE>(255.0f * GlowBreath(now) + 0.5f));
                return 1000.0 / 60.0;
            default:
                return 1e9;
        }
    }

    bool Visible() const { return state_ != State::Hidden; }
    double ShownAt() const { return shownAt_; }
    HWND Hwnd() const { return panel_.Hwnd(); }
    HWND UnderlayHwnd() const { return panel_.UnderlayHwnd(); }
    void Destroy() { panel_.Destroy(); }

  private:
    enum class State { Hidden, In, Shown, Out };

    void Render() {
        const float s = scale_;
        const bool dark = panel_.Dark();
        HFONT font = g_fonts.Get(static_cast<int>(lroundf(13.5f * s)), FW_SEMIBOLD);
        const int textWidth = text_.Width(font, kHintText,
                                          static_cast<int>(wcslen(kHintText)));
        width_ = static_cast<int>(ceilf((10 + 18 + 8 + 14) * s)) + textWidth;
        height_ = static_cast<int>(ceilf(34 * s));
        margin_ = static_cast<int>(ceilf(84 * s));

        Surface foreground;
        if (!foreground.Resize(width_, height_) || !content_.Resize(width_, height_)) {
            return;
        }
        FillSparkleGradient(foreground, (10 + 9) * s, height_ * 0.5f, 7.5f * s,
                            0.0f, 0.05f);
        const RECT textRect{static_cast<LONG>((10 + 18 + 8) * s), 0, width_, height_};
        text_.Draw(foreground, font, kHintText, textRect,
                   DT_SINGLELINE | DT_VCENTER | DT_LEFT,
                   dark ? WithAlpha(kWhite, 0.96f) : WithAlpha(kBlack, 0.88f));
        PaintPanelBase(content_, panel_, s);
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                BlendOver(content_.Row(y)[x], foreground.Row(y)[x]);
            }
        }
        PaintPanelUnderlay(underlay_, width_, height_, margin_, panel_.Radius(s), s,
                           dark, dark ? 0.85f : 0.65f, 0.0f);
    }

    POINT Placement(POINT cursor) const {
        const RECT monitor = MonitorRectAt(cursor, false);
        int x = cursor.x + static_cast<int>(26 * scale_);
        int y = cursor.y + static_cast<int>(26 * scale_);
        if (x + width_ > monitor.right) {
            x = cursor.x - static_cast<int>(12 * scale_) - width_;
        }
        if (y + height_ > monitor.bottom) {
            y = cursor.y - static_cast<int>(20 * scale_) - height_;
        }
        return {x, y};
    }

    GlassPanel panel_;
    Surface content_;
    Surface underlay_;
    TextPainter text_;
    float scale_ = 0;
    int styleVersion_ = 0;
    int width_ = 0;
    int height_ = 0;
    int margin_ = 0;
    double shownAt_ = 0;
    double fadeOutAt_ = 0;
    State state_ = State::Hidden;
};

////////////////////////////////////////////////////////////////////////////////
// Raster workers and the selection outline
class RasterWorkers {
  public:
    ~RasterWorkers() { Reset(); }
    void Reset() {
        if (work_) {
            WaitForThreadpoolWorkCallbacks(work_, TRUE);
            CloseThreadpoolWork(work_);
            work_ = nullptr;
        }
        if (pool_) {
            DestroyThreadpoolEnvironment(&environment_);
            CloseThreadpool(pool_);
            pool_ = nullptr;
        }
        renderRow_ = {};
        attempted_ = false;
    }

    void ForRows(int first, int last, const std::function<void(int)>& renderRow, bool parallel,
                 int batchSize = 16) {
        if (parallel && !attempted_) {
            attempted_ = true;
            SYSTEM_INFO system;
            GetSystemInfo(&system);
            workerCount_ = std::min<DWORD>(2, system.dwNumberOfProcessors > 1
                                           ? system.dwNumberOfProcessors - 1 : 0u);
            if (workerCount_) {
                pool_ = CreateThreadpool(nullptr);
                if (pool_) {
                    InitializeThreadpoolEnvironment(&environment_);
                    SetThreadpoolCallbackPool(&environment_, pool_);
                    SetThreadpoolThreadMaximum(pool_, workerCount_);
                    if (SetThreadpoolThreadMinimum(pool_, workerCount_)) {
                        work_ = CreateThreadpoolWork(Callback, this, &environment_);
                    }
                }
            }
        }
        if (!parallel || !work_) {
            for (int y = first; y < last; y++) renderRow(y);
            return;
        }
        renderRow_ = renderRow;
        nextRow_ = first;
        lastRow_ = last;
        batchSize_ = std::max(1, batchSize);
        for (unsigned i = 0; i < workerCount_; i++) SubmitThreadpoolWork(work_);
        Run();
        WaitForThreadpoolWorkCallbacks(work_, FALSE);
        renderRow_ = {};
    }

  private:
    static void CALLBACK Callback(PTP_CALLBACK_INSTANCE, void* context, PTP_WORK) {
        static_cast<RasterWorkers*>(context)->Run();
    }
    void Run() {
        for (;;) {
            const int first = nextRow_.fetch_add(batchSize_, std::memory_order_relaxed);
            if (first >= lastRow_) return;
            for (int y = first; y < std::min(first + batchSize_, lastRow_); y++) renderRow_(y);
        }
    }
    PTP_POOL pool_ = nullptr;
    PTP_WORK work_ = nullptr;
    TP_CALLBACK_ENVIRON environment_{};
    std::function<void(int)> renderRow_;
    std::atomic<int> nextRow_{0};
    int lastRow_ = 0;
    int batchSize_ = 16;
    unsigned workerCount_ = 0;
    bool attempted_ = false;
};

enum class FrameStyle { Hover, Drag, Selected };

float OutlineCoverage(float x, float y, float l, float t, float r, float b,
                      float radius, float border) {
    auto distance = [&](float px, float py) {
        return fabsf(SdRoundRect(px, py, l, t, r, b, radius)) - border * 0.5f;
    };
    const float d = distance(x, y);
    if (fabsf(d) >= 1.0f) return Clamp01(0.5f - d);
    float coverage = 0;
    for (float dy : {-0.25f, 0.25f}) {
        for (float dx : {-0.25f, 0.25f}) {
            coverage += Clamp01(0.5f - distance(x + dx, y + dy) * 2.0f);
        }
    }
    return coverage * 0.25f;
}

class GlowFrame {
  public:
    void Show(const RECT& rect, FrameStyle style, float scale, double now, bool animate) {
        if (!window_.Hwnd() && !window_.Create(true, false, kLayerWindowClass, nullptr)) return;
        const float target[4] = {static_cast<float>(rect.left),
                                 static_cast<float>(rect.top),
                                 static_cast<float>(rect.right),
                                 static_cast<float>(rect.bottom)};
        const bool wasVisible = visible_ && opacityTo_ > 0;
        if (animate && wasVisible && style == style_) {
            for (int i = 0; i < 4; i++) {
                from_[i] = current_[i];
                to_[i] = target[i];
            }
            moveStart_ = now;
            moveDuration_ = 150.0;
        } else {
            for (int i = 0; i < 4; i++) {
                from_[i] = to_[i] = current_[i] = target[i];
            }
            moveDuration_ = 0;
        }
        const bool restyle =
            !visible_ || style != style_ || fabsf(scale - scale_) > 0.01f;
        if (style == FrameStyle::Selected && style_ != FrameStyle::Selected) {
            flashStart_ = now;
        }
        style_ = style;
        scale_ = scale;
        if (!wasVisible) {
            StartFade(now, 1.0f, style == FrameStyle::Hover ? 140.0 : 0.0);
        }
        visible_ = true;
        if (restyle) {
            Render(now);
        }
    }

    void FadeOut(double now) {
        if (visible_ && opacityTo_ > 0) {
            StartFade(now, 0.0f, style_ == FrameStyle::Hover ? 140.0 : 380.0);
        }
    }

    void HideNow() {
        visible_ = false;
        opacity_ = opacityTo_ = 0;
        window_.Hide();
    }

    void Conceal() { window_.Hide(); }

    double Tick(double now) {
        if (!visible_) {
            return 1e9;
        }
        if (moveDuration_ > 0) {
            const double t = (now - moveStart_) / moveDuration_;
            const float eased = static_cast<float>(EaseOutCubic(t));
            for (int i = 0; i < 4; i++) {
                current_[i] = from_[i] + (to_[i] - from_[i]) * eased;
            }
            if (t >= 1.0) {
                moveDuration_ = 0;
            }
        }
        if (fadeDuration_ > 0) {
            const double t = (now - fadeStart_) / fadeDuration_;
            opacity_ = opacityFrom_ +
                       (opacityTo_ - opacityFrom_) *
                           static_cast<float>(EaseOutCubic(t));
            if (t >= 1.0) {
                fadeDuration_ = 0;
                opacity_ = opacityTo_;
                if (opacityTo_ <= 0) {
                    HideNow();
                    return 1e9;
                }
            }
        }
        Render(now);
        return 1000.0 / 60.0;
    }

    bool Visible() const { return visible_; }
    FrameStyle Style() const { return style_; }
    HWND Hwnd() const { return window_.Hwnd(); }

    void Destroy() {
        window_.Destroy();
        composite_.Reset();
        workers_.Reset();
        for (Bloom& bloom : blooms_) bloom = {};
        masksReady_ = false;
        visible_ = false;
    }

  private:
    struct Look {
        float border;
        float tightSigma, tightAlpha;
        float wideSigma, wideAlpha;
    };

    Look CurrentLook(double now) const {
        Look look;
        switch (style_) {
            case FrameStyle::Hover:
                look = {1.1f, 3.0f, 0.34f, 12.0f, 0.16f};
                break;
            case FrameStyle::Drag:
                look = {1.3f, 3.6f, 0.42f, 14.0f, 0.20f};
                break;
            case FrameStyle::Selected:
            default:
                look = {1.4f, 4.0f, 0.46f, 16.0f, 0.22f};
                break;
        }
        const double sinceFlash = now - flashStart_;
        if (style_ == FrameStyle::Selected && sinceFlash < 500.0) {
            const float flash = static_cast<float>(1.0 - EaseOutCubic(sinceFlash / 500.0));
            look.tightAlpha = std::min(1.0f, look.tightAlpha + 0.35f * flash);
            look.wideAlpha = std::min(1.0f, look.wideAlpha + 0.30f * flash);
        }
        const float pulse = GlowBreath(now);
        look.tightAlpha *= pulse;
        look.wideAlpha *= pulse;
        return look;
    }

    void StartFade(double now, float to, double duration) {
        opacityFrom_ = opacity_;
        opacityTo_ = to;
        fadeStart_ = now;
        fadeDuration_ = duration;
        if (duration <= 0) {
            opacity_ = to;
        }
    }

    BYTE AlphaByte() const {
        return static_cast<BYTE>(Clamp01(opacity_) * 255.0f + 0.5f);
    }

    struct Bloom {
        std::vector<float> core, tight, wide;
    };

    static void BuildBloom(Bloom& bloom, RECT part, float l, float t, float r, float b,
                    float radius, float border, float tight, float wide) {
        const int padding = static_cast<int>(ceilf(wide * 3.0f)) + 6;
        const int width = part.right - part.left, height = part.bottom - part.top;
        const int workWidth = width + padding * 2, workHeight = height + padding * 2;
        const int left = part.left - padding, top = part.top - padding;
        std::vector<float> outline(static_cast<size_t>(workWidth) * workHeight, 0);
        const float reach = border * 0.5f + 1.0f;
        const int firstX = std::max(0, static_cast<int>(floorf(l - reach)) - left);
        const int lastX = std::min(workWidth, static_cast<int>(ceilf(r + reach)) - left);
        for (int y = 0; y < workHeight; y++) {
            const float py = top + y + 0.5f;
            if (py < t - reach || py > b + reach) continue;
            auto span = [&](int first, int last) {
                for (int x = first; x < last; x++) {
                    outline[static_cast<size_t>(y) * workWidth + x] = OutlineCoverage(
                        left + x + 0.5f, py, l, t, r, b, radius, border);
                }
            };
            if (py > t + radius + reach && py < b - radius - reach) {
                span(firstX, std::min(lastX, static_cast<int>(ceilf(l + reach)) - left));
                span(std::max(firstX, static_cast<int>(floorf(r - reach)) - left), lastX);
            } else {
                span(firstX, lastX);
            }
        }
        const auto tightBlur = BlurCoverage(outline, workWidth, workHeight, tight);
        const auto wideBlur = BlurCoverage(outline, workWidth, workHeight, wide);
        const size_t pixels = static_cast<size_t>(width) * height;
        bloom.core.resize(pixels);
        bloom.tight.resize(pixels);
        bloom.wide.resize(pixels);
        const float tightGain = 2.506628f * tight / border;
        const float wideGain = 2.506628f * wide / border;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                const size_t src = static_cast<size_t>(y + padding) * workWidth + x + padding;
                const size_t dst = static_cast<size_t>(y) * width + x;
                bloom.core[dst] = outline[src];
                bloom.tight[dst] = tightBlur[src] * tightGain;
                bloom.wide[dst] = wideBlur[src] * wideGain;
            }
        }
    }

    void Render(double now) {
        const float l = current_[0];
        const float t = current_[1];
        const float r = std::max(current_[2], l + 1);
        const float b = std::max(current_[3], t + 1);
        const float s = scale_;
        const Look look = CurrentLook(now);

        const float radius = std::min(12.0f * s, std::min(r - l, b - t) * 0.5f);
        const float border = look.border * s;
        const float tight = look.tightSigma * s;
        const float wide = look.wideSigma * s;
        const float phase = style_ == FrameStyle::Hover
                                ? 0.0f
                                : static_cast<float>(fmod(now / 2800.0, 1.0));

        const int outer = static_cast<int>(ceilf(wide * 4.0f + border));
        const int inner = outer;
        const bool rebuild = !masksReady_ || maskScale_ != s || maskStyle_ != style_ ||
                             !std::equal(std::begin(current_), std::end(current_), std::begin(maskRect_));

        const RECT outerRect{static_cast<LONG>(floorf(l)) - outer,
                             static_cast<LONG>(floorf(t)) - outer,
                             static_cast<LONG>(ceilf(r)) + outer,
                             static_cast<LONG>(ceilf(b)) + outer};
        const RECT innerRect{static_cast<LONG>(ceilf(l)) + inner,
                             static_cast<LONG>(ceilf(t)) + inner,
                             static_cast<LONG>(floorf(r)) - inner,
                             static_cast<LONG>(floorf(b)) - inner};
        RECT parts[4];
        int count;
        if (innerRect.right <= innerRect.left || innerRect.bottom <= innerRect.top) {
            parts[0] = outerRect;
            count = 1;
        } else {
            parts[0] = {outerRect.left, outerRect.top, outerRect.right, innerRect.top};
            parts[1] = {outerRect.left, innerRect.bottom, outerRect.right, outerRect.bottom};
            parts[2] = {outerRect.left, innerRect.top, innerRect.left, innerRect.bottom};
            parts[3] = {innerRect.right, innerRect.top, outerRect.right, innerRect.bottom};
            count = 4;
        }

        uint32_t colors[64];
        uint32_t lines[64];
        for (int i = 0; i < 64; i++) {
            const Color c = g_glow.At(i / 64.0f);
            colors[i] = PackPremultiplied(c, 1.0f);
            lines[i] = PackPremultiplied(Mix(c, kWhite, 0.65f), 1.0f);
        }

        const float cx = (l + r) * 0.5f;
        const float cy = (t + b) * 0.5f;
        if (rebuild) {
            workers_.ForRows(0, count, [&](int i) {
                BuildBloom(blooms_[i], parts[i], l, t, r, b, radius, border, tight, wide);
            }, count > 1 &&
               static_cast<uint64_t>(outerRect.right - outerRect.left) *
                   (outerRect.bottom - outerRect.top) >= 300000, 1);
        }
        for (int i = 0; i < 4; i++) {
            if (i >= count) {
                stripUsed_[i] = false;
                continue;
            }
            const RECT& part = parts[i];
            Surface& surface = surfaces_[i];
            stripUsed_[i] = false;
            if (!surface.Resize(part.right - part.left, part.bottom - part.top)) {
                masksReady_ = false;
                continue;
            }
            Bloom& bloom = blooms_[i];
            for (int y = 0; y < surface.Height(); y++) {
                uint32_t* row = surface.Row(y);
                const float py = part.top + y + 0.5f;
                for (int x = 0; x < surface.Width(); x++) {
                    const float px = part.left + x + 0.5f;
                    const size_t index = static_cast<size_t>(y) * surface.Width() + x;
                    const float glow = Clamp01(look.tightAlpha * bloom.tight[index] +
                                               look.wideAlpha * bloom.wide[index]);
                    const float edge = bloom.core[index];
                    if (glow < 0.5f / 255.0f && edge <= 0.0f) {
                        continue;
                    }
                    int c = 0;
                    if (g_glow.rainbow) {
                        float hue = FastAtan2(py - cy, px - cx) / (2.0f * kPi) + phase;
                        hue -= floorf(hue);
                        c = static_cast<int>(hue * 64.0f) & 63;
                    }
                    uint32_t pixel = ScalePremultiplied(
                        colors[c], static_cast<uint32_t>(glow * 255.0f + 0.5f));
                    if (edge > 0.0f) {
                        BlendOver(pixel, ScalePremultiplied(
                                             lines[c], static_cast<uint32_t>(
                                                           edge * 222.0f + 0.5f)));
                    }
                    row[x] = pixel;
                }
            }
            stripUsed_[i] = true;
            partRects_[i] = part;
        }
        masksReady_ = std::all_of(stripUsed_, stripUsed_ + count, [](bool used) { return used; });
        if (masksReady_) {
            std::copy_n(current_, 4, maskRect_);
            maskScale_ = s;
            maskStyle_ = style_;
        }
        if (!composite_.Resize(outerRect.right - outerRect.left, outerRect.bottom - outerRect.top)) return;
        for (int i = 0; i < count; i++) {
            if (!stripUsed_[i]) continue;
            const Surface& source = surfaces_[i];
            const int offsetX = partRects_[i].left - outerRect.left;
            const int offsetY = partRects_[i].top - outerRect.top;
            for (int y = 0; y < source.Height(); y++) {
                memcpy(composite_.Row(offsetY + y) + offsetX, source.Row(y),
                       static_cast<size_t>(source.Width()) * sizeof(uint32_t));
            }
        }
        window_.Present(composite_, outerRect.left, outerRect.top, AlphaByte());
        window_.Show();
    }

    LayeredWindow window_;
    Surface composite_;
    Surface surfaces_[4];
    RECT partRects_[4] = {};
    bool stripUsed_[4] = {};
    Bloom blooms_[4];
    RasterWorkers workers_;
    float maskRect_[4]{};
    float maskScale_ = 0;
    FrameStyle maskStyle_ = FrameStyle::Hover;
    bool masksReady_ = false;
    float from_[4] = {};
    float to_[4] = {};
    float current_[4] = {};
    double moveStart_ = 0;
    double moveDuration_ = 0;
    float opacity_ = 0;
    float opacityFrom_ = 0;
    float opacityTo_ = 0;
    double fadeStart_ = 0;
    double fadeDuration_ = 0;
    double flashStart_ = -1e9;
    FrameStyle style_ = FrameStyle::Hover;
    float scale_ = 1.0f;
    bool visible_ = false;
};

////////////////////////////////////////////////////////////////////////////////
// The ripple

struct RippleWave {
    float displacement;
    float opacity;
    float light;
};

RippleWave EvaluateRippleWave(float distance, float sigma, float strength) {
    const float u = distance / sigma;
    const float envelope = expf(-0.5f * u * u);
    return {-strength * u * envelope,
            Clamp01(envelope * 4.0f),
            0.055f * envelope};
}

class Ripple {
  public:
    static constexpr double kDurationMs = 1650.0;

    void Start(POINT center, float scale, double now) {
        Stop();
        if (!window_.Hwnd() && !window_.Create(true, false, kLayerWindowClass, nullptr)) {
            return;
        }
        monitor_ = MonitorRectAt(center, false);
        const int w = monitor_.right - monitor_.left;
        const int h = monitor_.bottom - monitor_.top;
        if (w <= 0 || h <= 0 || !surface_.Resize(w, h)) {
            Stop();
            return;
        }
        if (snapshot_.Resize(w, h)) {
            HDC screen = GetDC(nullptr);
            const bool captured = screen && BitBlt(snapshot_.Dc(), 0, 0, w, h,
                                                   screen, monitor_.left, monitor_.top,
                                                   SRCCOPY | CAPTUREBLT);
            GdiFlush();
            if (screen) {
                ReleaseDC(nullptr, screen);
            }
            if (!captured) {
                snapshot_.Reset();
            }
        }
        center_ = {static_cast<float>(center.x - monitor_.left),
                   static_cast<float>(center.y - monitor_.top)};
        scale_ = scale;
        float farthest = 0;
        for (const Vec2 corner : {Vec2{0, 0}, Vec2{static_cast<float>(w), 0},
                                  Vec2{0, static_cast<float>(h)},
                                  Vec2{static_cast<float>(w), static_cast<float>(h)}}) {
            farthest = std::max(farthest, hypotf(corner.x - center_.x, corner.y - center_.y));
        }
        maxRadius_ = farthest + 100.0f * scale;
        start_ = std::max(now, NowMs());
        active_ = true;
        Render(start_);
        window_.Show();
    }

    double Tick(double now) {
        if (!active_) {
            return 1e9;
        }
        if (now - start_ >= kDurationMs) {
            Stop();
            return 1e9;
        }
        Render(now);
        return 1000.0 / 60.0;
    }

    void Stop() {
        workers_.Reset();
        active_ = false;
        window_.Hide();
        surface_.Reset();
        snapshot_.Reset();
        wave_.clear();
    }

    bool Active() const { return active_; }
    HWND Hwnd() const { return window_.Hwnd(); }
    void Destroy() {
        Stop();
        window_.Destroy();
    }

  private:
    uint32_t SampleDesktop(float x, float y) const {
        return SampleOpaqueSurface(snapshot_, x, y);
    }

    struct RasterWave {
        float displacement;
        uint32_t opacity;
        uint32_t lightAlpha;
    };

    void Render(double now) {
        const float t = static_cast<float>(std::clamp((now - start_) / kDurationMs, 0.0, 1.0));
        const float elapsed = static_cast<float>(now - start_);
        const int w = surface_.Width();
        const int h = surface_.Height();
        surface_.Clear();

        const float travel = t * t * (3.0f - 2.0f * t);
        const float radius = travel * maxRadius_;
        const float sigma = (40.0f + 38.0f * travel) * scale_;
        const float in = Clamp01(elapsed / 180.0f);
        const float out = Clamp01((t - 0.55f) / 0.45f);
        const float fadeIn = in * in * (3.0f - 2.0f * in);
        const float fadeOut = 1.0f - out * out * (3.0f - 2.0f * out);
        const float strength = 14.0f * scale_ * fadeIn * fadeOut;
        const float alpha = fadeIn * fadeOut;

        const float reach = sigma * 3.8f;
        const int offset = static_cast<int>(ceilf(reach * 4.0f));
        wave_.resize(static_cast<size_t>(offset) * 2 + 2);
        for (size_t i = 0; i < wave_.size(); i++) {
            const RippleWave wave = EvaluateRippleWave((static_cast<int>(i) - offset) / 4.0f,
                                                        sigma, strength);
            wave_[i] = {wave.displacement,
                         static_cast<uint32_t>(wave.opacity * alpha * 255.0f + 0.5f),
                         static_cast<uint32_t>(wave.light * alpha * 255.0f + 0.5f)};
        }
        const float inner = std::max(0.0f, radius - reach);
        const float outer = radius + reach;
        const int firstY = std::max(0, static_cast<int>(floorf(center_.y - outer)));
        const int lastY = std::min(h, static_cast<int>(ceilf(center_.y + outer)));
        const float edgeWidth = 24.0f * scale_;
        uint32_t colors[64];
        for (int i = 0; i < 64; i++) {
            colors[i] = PackPremultiplied(g_glow.At(i / 64.0f + t * 0.2f), 1.0f);
        }
        workers_.ForRows(firstY, lastY, [&](int y) {
            const float dy = y - center_.y;
            if (fabsf(dy) >= outer) {
                return;
            }
            const float xo = sqrtf(std::max(0.0f, outer * outer - dy * dy));
            const float xi = fabsf(dy) < inner
                                 ? sqrtf(std::max(0.0f, inner * inner - dy * dy)) : 0.0f;
            const float spans[2][2] = {{center_.x - xo, center_.x - xi},
                                       {center_.x + xi, center_.x + xo}};
            uint32_t* row = surface_.Row(y);
            const float rowEdge = Clamp01(std::min(y, h - 1 - y) / edgeWidth);
            for (int span = 0; span < 2; span++) {
                const int x0 = std::max(0, static_cast<int>(ceilf(spans[span][0])));
                const int x1 = std::min(w, static_cast<int>(ceilf(spans[span][1])));
                for (int x = x0; x < x1; x++) {
                    const float dx = x - center_.x;
                    const float distance = sqrtf(dx * dx + dy * dy);
                    const int index = static_cast<int>((distance - radius) * 4.0f) + offset;
                    if (index < 0 || index >= static_cast<int>(wave_.size())) {
                        continue;
                    }
                    const RasterWave& wave = wave_[index];
                    if (snapshot_.Valid() && wave.opacity > 1) {
                        float edge = rowEdge;
                        if (x < edgeWidth || w - 1 - x < edgeWidth) {
                            edge = std::min(edge, Clamp01(std::min(x, w - 1 - x) / edgeWidth));
                        }
                        const float displacement = wave.displacement * edge * edge *
                            (3.0f - 2.0f * edge) / std::max(distance, 1.0f);
                        uint32_t sample = SampleDesktop(x + dx * displacement,
                                                         y + dy * displacement);
                        row[x] = ScalePremultiplied(sample, wave.opacity);
                    }
                    int hue = 0;
                    if (g_glow.rainbow) {
                        const float phase = FastAtan2(dy, dx) / (2.0f * kPi);
                        hue = static_cast<int>((phase - floorf(phase)) * 64.0f) & 63;
                    }
                    BlendOver(row[x], ScalePremultiplied(colors[hue], wave.lightAlpha));
                }
            }
        }, static_cast<uint64_t>(w) * h >= 500000);
        window_.Present(surface_, monitor_.left, monitor_.top, 255);
    }

    LayeredWindow window_;
    Surface surface_;
    Surface snapshot_;
    RECT monitor_{};
    Vec2 center_{};
    float scale_ = 1.0f;
    float maxRadius_ = 0;
    double start_ = 0;
    bool active_ = false;
    std::vector<RasterWave> wave_;
    RasterWorkers workers_;
};

////////////////////////////////////////////////////////////////////////////////
// The input overlay

void OnOverlayMouse(UINT message, POINT screenPoint);
HCURSOR OverlayCursor();
void OnDisplayChanged();

void OutsideRectangles(RECT screen, RECT opening, RECT (&parts)[4]) {
    RECT hole;
    if (!IntersectRect(&hole, &screen, &opening)) {
        parts[0] = screen;
        parts[1] = parts[2] = parts[3] = {};
        return;
    }
    parts[0] = {screen.left, screen.top, screen.right, hole.top};
    parts[1] = {screen.left, hole.bottom, screen.right, screen.bottom};
    parts[2] = {screen.left, hole.top, hole.left, hole.bottom};
    parts[3] = {hole.right, hole.top, screen.right, hole.bottom};
}

bool RenderSelectionDimMask(Surface& mask, RECT screen, RECT opening, float feather) {
    if (!mask.Resize(screen.right - screen.left, screen.bottom - screen.top)) return false;
    for (int y = 0; y < mask.Height(); y++) {
        std::fill_n(mask.Row(y), mask.Width(), 0xFF000000u);
    }
    RECT hole;
    if (IntersectRect(&hole, &screen, &opening)) {
        for (int y = hole.top; y < hole.bottom; y++) {
            std::fill_n(mask.Row(y - screen.top) + hole.left - screen.left,
                        hole.right - hole.left, 0u);
        }
    }
    feather = std::max(feather, 1.0f);
    const LONG padding = static_cast<LONG>(ceilf(feather)) + 1;
    RECT bounds{opening.left - padding, opening.top - padding,
                opening.right + padding, opening.bottom + padding};
    if (IntersectRect(&bounds, &bounds, &screen)) {
        RECT parts[4];
        OutsideRectangles(bounds, opening, parts);
        for (RECT part : parts) {
            for (int y = part.top; y < part.bottom; y++) {
                uint32_t* row = mask.Row(y - screen.top);
                for (int x = part.left; x < part.right; x++) {
                    const float distance = SdRoundRect(x + 0.5f, y + 0.5f,
                        opening.left, opening.top, opening.right, opening.bottom, 0);
                    const float t = Clamp01(distance / feather);
                    const uint32_t alpha = static_cast<uint32_t>(t * t * (3 - 2 * t) * 255 + 0.5f);
                    row[x - screen.left] = alpha << 24;
                }
            }
        }
    }
    return true;
}

class InputOverlay {
  public:
    bool Create() {
        hwnd_ = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            kOverlayWindowClass, L"Magic Pointer", WS_POPUP, 0, 0, 1, 1,
            nullptr, nullptr, g_instance, nullptr);
        if (hwnd_) {
            SetLayeredWindowAttributes(hwnd_, 0, 1, LWA_ALPHA);
            if (!dimWindow_.Create(true, false, kLayerWindowClass, nullptr)) {
                Destroy();
                return false;
            }
        }
        return hwnd_ != nullptr;
    }

    void Show(int dimPercent) {
        if (!hwnd_) {
            return;
        }
        const RECT screen = VirtualScreenRect();
        ClearSelection();
        dimPercent_ = std::clamp(dimPercent, 0, 80);
        SetLayeredWindowAttributes(hwnd_, 0, 1, LWA_ALPHA);
        SetWindowPos(hwnd_, HWND_TOPMOST, screen.left, screen.top,
                     screen.right - screen.left, screen.bottom - screen.top,
                     SWP_SHOWWINDOW | SWP_NOACTIVATE);
        visible_ = true;
        g_layersChanged = true;
    }

    void SetSelection(RECT rect, double now) {
        opening_ = rect;
        dimDirty_ = true;
        if (!dimming_ && dimPercent_ > 0) {
            dimming_ = true;
            dimStart_ = now;
        }
    }

    void ClearSelection() {
        dimming_ = false;
        dimDirty_ = false;
        dimAlpha_ = 0;
        dimWindow_.Hide();
        dimMask_.Reset();
    }

    void Hide() {
        ClearSelection();
        if (hwnd_ && visible_) {
            ShowWindow(hwnd_, SW_HIDE);
        }
        visible_ = false;
    }

    double Tick(double now) {
        if (!visible_ || !dimming_) {
            return 1e9;
        }
        const double t = (now - dimStart_) / 160.0;
        const BYTE alpha = static_cast<BYTE>(dimPercent_ * 255.0 / 100.0 * EaseOutCubic(t) + 0.5);
        if (alpha != dimAlpha_ || dimDirty_) {
            if (dimDirty_) {
                const RECT screen = VirtualScreenRect();
                if (RenderSelectionDimMask(dimMask_, screen, opening_,
                        6.0f * DpiScaleAt(RectCenter(opening_)))) {
                    dimWindow_.Present(dimMask_, screen.left, screen.top, alpha);
                    dimDirty_ = false;
                }
            } else {
                dimWindow_.SetAlpha(alpha);
            }
            if (alpha) dimWindow_.Show();
            else dimWindow_.Hide();
            dimAlpha_ = alpha;
        }
        return t >= 1.0 ? 1e9 : 0;
    }

    bool Visible() const { return visible_; }
    HWND Hwnd() const { return hwnd_; }
    HWND DimHwnd() const { return dimWindow_.Hwnd(); }

    void Destroy() {
        dimWindow_.Destroy();
        dimMask_.Reset();
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
        visible_ = false;
    }

    static LRESULT CALLBACK WndProc(HWND hwnd,
                                    UINT message,
                                    WPARAM wParam,
                                    LPARAM lParam) {
        switch (message) {
            case WM_MOUSEACTIVATE:
                return MA_NOACTIVATE;

            case WM_SETCURSOR:
                SetCursor(OverlayCursor());
                return TRUE;

            case WM_ERASEBKGND: {
                RECT rc;
                GetClientRect(hwnd, &rc);
                FillRect(reinterpret_cast<HDC>(wParam), &rc,
                         static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
                return 1;
            }

            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_CAPTURECHANGED: {
                POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                if (message == WM_CAPTURECHANGED) {
                    GetCursorPos(&pt);
                } else {
                    ClientToScreen(hwnd, &pt);
                }
                OnOverlayMouse(message, pt);
                return 0;
            }

            case WM_DISPLAYCHANGE:
                OnDisplayChanged();
                break;
        }
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

  private:
    HWND hwnd_ = nullptr;
    bool visible_ = false;
    LayeredWindow dimWindow_;
    Surface dimMask_;
    RECT opening_{};
    int dimPercent_ = 0;
    BYTE dimAlpha_ = 0;
    bool dimming_ = false;
    bool dimDirty_ = false;
    double dimStart_ = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Shake detection

class ShakeDetector {
  public:
    void Configure(int sensitivity) {
        minDistance_ = 1300.0 - 100.0 * sensitivity;
        minRatio_ = 5.0 - 0.3 * sensitivity;
        Clear();
    }

    void Clear() {
        samples_.clear();
        pathLength_ = 0;
    }

    bool AddPoint(double now, POINT pt, float dpiScale) {
        if (!samples_.empty() && samples_.back().x == pt.x &&
            samples_.back().y == pt.y) {
            return false;
        }

        if (samples_.size() >= 2 && now - samples_.back().time < 4.0) {
            Sample& last = samples_.back();
            const Sample& previous = samples_[samples_.size() - 2];
            pathLength_ -= last.segment;
            last.x = pt.x;
            last.y = pt.y;
            last.segment = Distance(previous.x, previous.y, pt.x, pt.y);
            pathLength_ += last.segment;
        } else {
            const double segment =
                samples_.empty() ? 0.0
                                 : Distance(samples_.back().x,
                                            samples_.back().y, pt.x, pt.y);
            samples_.push_back({now, pt.x, pt.y, segment});
            pathLength_ += segment;
        }

        while (samples_.size() > 1 && now - samples_.front().time > kWindowMs) {
            samples_.pop_front();
            pathLength_ -= samples_.front().segment;
            samples_.front().segment = 0;
        }
        if (samples_.size() <= 1) {
            pathLength_ = 0;
        }

        if (samples_.size() < 6 || pathLength_ < minDistance_ * dpiScale) {
            return false;
        }

        LONG minX = samples_.front().x, maxX = minX;
        LONG minY = samples_.front().y, maxY = minY;
        for (const Sample& sample : samples_) {
            minX = std::min(minX, sample.x);
            maxX = std::max(maxX, sample.x);
            minY = std::min(minY, sample.y);
            maxY = std::max(maxY, sample.y);
        }
        const double diagonal = Distance(minX, minY, maxX, maxY);
        return diagonal >= 1.0 && pathLength_ >= diagonal * minRatio_;
    }

  private:
    struct Sample {
        double time;
        LONG x;
        LONG y;
        double segment;  // Distance from the previous sample.
    };

    static double Distance(LONG x1, LONG y1, LONG x2, LONG y2) {
        return std::hypot(static_cast<double>(x2 - x1),
                          static_cast<double>(y2 - y1));
    }

    static constexpr double kWindowMs = 1000.0;
    std::deque<Sample> samples_;
    double pathLength_ = 0;
    double minDistance_ = 800.0;
    double minRatio_ = 3.5;
};

bool AnyMouseButtonDown() {
    return (GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON) |
            GetAsyncKeyState(VK_MBUTTON) | GetAsyncKeyState(VK_XBUTTON1) |
            GetAsyncKeyState(VK_XBUTTON2)) &
           0x8000;
}

bool ForegroundIsFullscreen();

bool CanSummonNow() {
    CURSORINFO cursorInfo{};
    cursorInfo.cbSize = sizeof(cursorInfo);
    if (!GetCursorInfo(&cursorInfo) || !(cursorInfo.flags & CURSOR_SHOWING) ||
        (cursorInfo.flags & CURSOR_SUPPRESSED)) {
        return false;
    }

    RECT clip;
    if (GetClipCursor(&clip)) {
        const RECT monitor = MonitorRectAt(cursorInfo.ptScreenPos, false);
        if (clip.left > monitor.left || clip.top > monitor.top ||
            clip.right < monitor.right || clip.bottom < monitor.bottom) {
            return false;
        }
    }

    if (g_settings.disableInFullscreen) {
        QUERY_USER_NOTIFICATION_STATE state;
        if (SUCCEEDED(SHQueryUserNotificationState(&state)) &&
            (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN ||
             state == QUNS_PRESENTATION_MODE)) {
            return false;
        }
        if (ForegroundIsFullscreen()) {
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Apps where shaking the mouse is part of using them

bool WildcardMatch(const wchar_t* pattern, const wchar_t* text) {
    const wchar_t* star = nullptr;
    const wchar_t* resume = nullptr;
    while (*text) {
        if (*pattern == L'*') {
            star = pattern++;
            resume = text;
        } else if (*pattern == L'?' || towlower(*pattern) == towlower(*text)) {
            pattern++;
            text++;
        } else if (star) {
            pattern = star + 1;
            text = ++resume;
        } else {
            return false;
        }
    }
    while (*pattern == L'*') {
        pattern++;
    }
    return !*pattern;
}

std::wstring ProcessPathOfWindow(HWND hwnd) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (!processId) {
        return {};
    }
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return {};
    }
    WCHAR path[MAX_PATH * 2];
    DWORD size = ARRAYSIZE(path);
    std::wstring result;
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        result.assign(path, size);
    }
    CloseHandle(process);
    return result;
}

bool IsAppExcluded(HWND hwnd) {
    if (!hwnd || g_settings.excludedApps.empty()) {
        return false;
    }
    hwnd = GetAncestor(hwnd, GA_ROOT);
    if (!hwnd) {
        return false;
    }
    const std::wstring path = ProcessPathOfWindow(hwnd);
    const size_t slash = path.find_last_of(L"\\/");
    const std::wstring name =
        slash == std::wstring::npos ? path : path.substr(slash + 1);
    WCHAR title[256] = L"";
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));
    for (const std::wstring& pattern : g_settings.excludedApps) {
        if (pattern.find_first_of(L"\\/:") != std::wstring::npos) {
            if (!path.empty() && WildcardMatch(pattern.c_str(), path.c_str())) {
                return true;
            }
        } else if (!name.empty() &&
                   (WildcardMatch(pattern.c_str(), name.c_str()) ||
                    (pattern.find(L'.') == std::wstring::npos &&
                     WildcardMatch((pattern + L".exe").c_str(), name.c_str())))) {
            return true;
        }
        if (title[0] && WildcardMatch(pattern.c_str(), title)) {
            return true;
        }
    }
    return false;
}

bool IsWindowCloaked(HWND hwnd);

bool ForegroundIsFullscreen() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd || !IsWindowVisible(hwnd) || IsWindowCloaked(hwnd)) {
        return false;
    }
    WCHAR className[64] = L"";
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    for (PCWSTR shell : {L"Progman", L"WorkerW", L"Shell_TrayWnd",
                         L"Shell_SecondaryTrayWnd"}) {
        if (wcscmp(className, shell) == 0) {
            return false;
        }
    }
    const LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    if ((style & WS_CAPTION) == WS_CAPTION) {
        return false;
    }
    RECT rect;
    MONITORINFO monitor{};
    monitor.cbSize = sizeof(monitor);
    if (!GetWindowRect(hwnd, &rect) ||
        !GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                         &monitor)) {
        return false;
    }
    return rect.left <= monitor.rcMonitor.left &&
           rect.top <= monitor.rcMonitor.top &&
           rect.right >= monitor.rcMonitor.right &&
           rect.bottom >= monitor.rcMonitor.bottom;
}

bool ShakeExcludedAt(POINT pt) {
    if (g_settings.excludedApps.empty()) {
        return false;
    }
    return IsAppExcluded(GetForegroundWindow()) || IsAppExcluded(WindowFromPoint(pt));
}

////////////////////////////////////////////////////////////////////////////////
// Keyboard shortcut parsing, for strings like "Win+Shift+G"

bool ParseHotkey(const std::wstring& text, UINT& modifiers, UINT& key) {
    modifiers = 0;
    key = 0;
    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find(L'+', start);
        if (end == std::wstring::npos) {
            end = text.size();
        }
        std::wstring token = Trim(text.substr(start, end - start));
        start = end + 1;
        if (token.empty()) {
            if (end == text.size()) {
                break;
            }
            continue;
        }
        for (wchar_t& c : token) {
            c = towlower(c);
        }

        if (token == L"ctrl" || token == L"control") {
            modifiers |= MOD_CONTROL;
        } else if (token == L"alt") {
            modifiers |= MOD_ALT;
        } else if (token == L"shift") {
            modifiers |= MOD_SHIFT;
        } else if (token == L"win" || token == L"windows" ||
                   token == L"meta" || token == L"super") {
            modifiers |= MOD_WIN;
        } else if (token.size() == 1 &&
                   ((token[0] >= L'a' && token[0] <= L'z') ||
                    (token[0] >= L'0' && token[0] <= L'9'))) {
            key = towupper(token[0]);
        } else if (token.size() >= 2 && token.size() <= 3 && token[0] == L'f' &&
                   iswdigit(token[1])) {
            const int number = _wtoi(token.c_str() + 1);
            if (number >= 1 && number <= 24) {
                key = VK_F1 + number - 1;
            }
        } else {
            static const struct {
                PCWSTR name;
                UINT vk;
            } kKeys[] = {
                {L"space", VK_SPACE},     {L"enter", VK_RETURN},
                {L"return", VK_RETURN},   {L"tab", VK_TAB},
                {L"backspace", VK_BACK},  {L"insert", VK_INSERT},
                {L"ins", VK_INSERT},      {L"delete", VK_DELETE},
                {L"del", VK_DELETE},      {L"home", VK_HOME},
                {L"end", VK_END},         {L"pageup", VK_PRIOR},
                {L"pgup", VK_PRIOR},      {L"pagedown", VK_NEXT},
                {L"pgdn", VK_NEXT},       {L"up", VK_UP},
                {L"down", VK_DOWN},       {L"left", VK_LEFT},
                {L"right", VK_RIGHT},     {L"printscreen", VK_SNAPSHOT},
                {L"prtsc", VK_SNAPSHOT},  {L"pause", VK_PAUSE},
                {L"`", VK_OEM_3},         {L"-", VK_OEM_MINUS},
                {L"=", VK_OEM_PLUS},      {L"[", VK_OEM_4},
                {L"]", VK_OEM_6},         {L"\\", VK_OEM_5},
                {L";", VK_OEM_1},         {L"'", VK_OEM_7},
                {L",", VK_OEM_COMMA},     {L".", VK_OEM_PERIOD},
                {L"/", VK_OEM_2},
            };
            for (const auto& entry : kKeys) {
                if (token == entry.name) {
                    key = entry.vk;
                    break;
                }
            }
            if (!key) {
                return false;
            }
        }
    }
    return key != 0;
}

////////////////////////////////////////////////////////////////////////////////
// Finding what's under the pointer with UI Automation

struct PickResult {
    uint32_t id = 0;
    bool found = false;
    RECT rect{};
    std::wstring text;
};

bool IsWindowCloaked(HWND hwnd) {
    using DwmGetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, PVOID, DWORD);
    static auto getAttribute = GetProc<DwmGetWindowAttribute_t>(
        L"dwmapi.dll", "DwmGetWindowAttribute");
    DWORD cloaked = 0;
    return getAttribute &&
           SUCCEEDED(getAttribute(hwnd, 14 /* DWMWA_CLOAKED */, &cloaked,
                                  sizeof(cloaked))) &&
           cloaked;
}

HWND TopLevelWindowAt(POINT pt) {
    const DWORD self = GetCurrentProcessId();
    for (HWND hwnd = GetTopWindow(nullptr); hwnd;
         hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
        if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
            continue;
        }
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == self) {
            continue;
        }
        const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (exStyle & WS_EX_TRANSPARENT) {
            continue;
        }
        RECT rc;
        if (!GetWindowRect(hwnd, &rc) || !PtInRect(&rc, pt)) {
            continue;
        }
        if (exStyle & WS_EX_LAYERED) {
            BYTE alpha = 255;
            DWORD flags = 0;
            COLORREF key;
            if (GetLayeredWindowAttributes(hwnd, &key, &alpha, &flags) &&
                (flags & LWA_ALPHA) && alpha == 0) {
                continue;
            }
        }
        if (IsWindowCloaked(hwnd)) {
            continue;
        }
        HRGN region = CreateRectRgn(0, 0, 0, 0);
        const int regionType = region ? GetWindowRgn(hwnd, region) : ERROR;
        const bool inRegion =
            regionType == ERROR ||
            (regionType != NULLREGION &&
             PtInRegion(region, pt.x - rc.left, pt.y - rc.top));
        if (region) {
            DeleteObject(region);
        }
        if (inRegion) {
            return hwnd;
        }
    }
    return nullptr;
}

HWND DeepestChildAt(HWND hwnd, POINT pt) {
    for (int depth = 0; depth < 32; depth++) {
        POINT client = pt;
        ScreenToClient(hwnd, &client);
        HWND child = ChildWindowFromPointEx(
            hwnd, client, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);
        if (!child || child == hwnd) {
            break;
        }
        hwnd = child;
    }
    return hwnd;
}

class ElementPicker {
  public:
    bool Start(HWND notify) {
        if (thread_) {
            return true;
        }
        notify_ = notify;
        stop_ = false;
        event_ = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!event_) {
            return false;
        }
        thread_ = CreateThread(nullptr, 0, ThreadProc, this, 0, nullptr);
        if (!thread_) {
            CloseHandle(event_);
            event_ = nullptr;
            return false;
        }
        return true;
    }

    void Stop() {
        if (!thread_) {
            return;
        }
        stop_ = true;
        SetEvent(event_);
        if (WaitForSingleObject(thread_, 3000) == WAIT_OBJECT_0) {
            CloseHandle(thread_);
            CloseHandle(event_);
        }
        thread_ = nullptr;
        event_ = nullptr;
    }

    uint32_t Request(POINT pt) {
        if (!thread_) {
            return 0;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        pendingPoint_ = pt;
        pendingId_ = ++lastId_;
        hasPending_ = true;
        SetEvent(event_);
        return pendingId_;
    }

    uint32_t LatestId() {
        std::lock_guard<std::mutex> lock(mutex_);
        return lastId_;
    }

  private:
    static DWORD WINAPI ThreadProc(LPVOID param) {
        static_cast<ElementPicker*>(param)->Run();
        return 0;
    }

    void Run() {
        const HRESULT coInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        ComPtr<IUIAutomation> automation;
        if (FAILED(CoCreateInstance(kClsidCUIAutomation8, nullptr,
                                    CLSCTX_INPROC_SERVER, kIidIUIAutomation,
                                    automation.PutVoid()))) {
            CoCreateInstance(kClsidCUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                             kIidIUIAutomation, automation.PutVoid());
        }

        ComPtr<IUIAutomationCacheRequest> cache;
        ComPtr<IUIAutomationCondition> condition;
        if (automation) {
            ComPtr<IUIAutomation2> automation2;
            if (SUCCEEDED(automation->QueryInterface(kIidIUIAutomation2,
                                                     automation2.PutVoid()))) {
                automation2->put_ConnectionTimeout(1500);
                automation2->put_TransactionTimeout(1500);
            }
            if (SUCCEEDED(automation->CreateCacheRequest(cache.Put()))) {
                cache->AddProperty(UIA_BoundingRectanglePropertyId);
                cache->AddProperty(UIA_ControlTypePropertyId);
                cache->AddProperty(UIA_IsOffscreenPropertyId);
                cache->AddProperty(UIA_NamePropertyId);
            }
            automation->get_ControlViewCondition(condition.Put());
        }
        if (!automation || !cache || !condition) {
            Wh_Log(L"UI Automation is unavailable, outlining windows only");
        }

        while (!stop_) {
            WaitForSingleObject(event_, INFINITE);
            if (stop_) {
                break;
            }
            POINT pt;
            uint32_t id;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (!hasPending_) {
                    continue;
                }
                pt = pendingPoint_;
                id = pendingId_;
                hasPending_ = false;
            }

            auto result = std::make_unique<PickResult>();
            result->id = id;
            Pick(automation.Get(), cache.Get(), condition.Get(), pt, *result);
            if (stop_) {
                break;
            }
            PostOwned(notify_, WM_APP_PICK, std::move(result));
        }

        condition.Reset();
        cache.Reset();
        automation.Reset();
        if (SUCCEEDED(coInit)) {
            CoUninitialize();
        }
    }

    static bool IsMeaningfulType(CONTROLTYPEID type) {
        switch (type) {
            case UIA_ButtonControlTypeId:
            case UIA_CheckBoxControlTypeId:
            case UIA_ComboBoxControlTypeId:
            case UIA_EditControlTypeId:
            case UIA_HyperlinkControlTypeId:
            case UIA_ImageControlTypeId:
            case UIA_ListItemControlTypeId:
            case UIA_MenuItemControlTypeId:
            case UIA_RadioButtonControlTypeId:
            case UIA_SliderControlTypeId:
            case UIA_SplitButtonControlTypeId:
            case UIA_TabItemControlTypeId:
            case UIA_TreeItemControlTypeId:
            case UIA_DataItemControlTypeId:
            case UIA_HeaderItemControlTypeId:
                return true;
        }
        return false;
    }

    static bool IsLabelContainer(CONTROLTYPEID type) {
        return type == UIA_ButtonControlTypeId ||
               type == UIA_HyperlinkControlTypeId ||
               type == UIA_ListItemControlTypeId ||
               type == UIA_MenuItemControlTypeId ||
               type == UIA_TabItemControlTypeId ||
               type == UIA_TreeItemControlTypeId ||
               type == UIA_DataItemControlTypeId ||
               type == UIA_HeaderItemControlTypeId ||
               type == UIA_SplitButtonControlTypeId;
    }

    void Pick(IUIAutomation* automation,
              IUIAutomationCacheRequest* cache,
              IUIAutomationCondition* condition,
              POINT pt,
              PickResult& out) {
        HWND top = TopLevelWindowAt(pt);
        if (!top) {
            return;
        }
        HWND target = DeepestChildAt(top, pt);

        const RECT monitor = MonitorRectAt(pt, false);
        const double monitorArea =
            double(monitor.right - monitor.left) * (monitor.bottom - monitor.top);
        auto area = [](const RECT& rc) {
            return double(rc.right - rc.left) * (rc.bottom - rc.top);
        };
        auto reasonable = [&](const RECT& rc, double maxShare) {
            const LONG w = rc.right - rc.left;
            const LONG h = rc.bottom - rc.top;
            return w >= 6 && h >= 6 && area(rc) <= monitorArea * maxShare;
        };

        auto fallbackToWindow = [&] {
            RECT rc;
            if (target != top && GetWindowRect(target, &rc) &&
                reasonable(rc, 0.6) && PtInRect(&rc, pt)) {
                out.found = true;
                out.rect = rc;
            }
        };

        if (!automation || !cache || !condition) {
            fallbackToWindow();
            return;
        }

        struct Node {
            IUIAutomationElement* element;
            RECT rect;
            CONTROLTYPEID type;
        };
        std::vector<Node> path;
        auto release = [&] {
            for (Node& node : path) {
                node.element->Release();
            }
            path.clear();
        };

        IUIAutomationElement* element = nullptr;
        if (FAILED(automation->ElementFromHandleBuildCache(target, cache,
                                                           &element)) ||
            !element) {
            fallbackToWindow();
            return;
        }
        Node root{element, {}, 0};
        element->get_CachedBoundingRectangle(&root.rect);
        element->get_CachedControlType(&root.type);
        path.push_back(root);

        for (int depth = 0; depth < 40 && !stop_; depth++) {
            IUIAutomationElementArray* children = nullptr;
            if (FAILED(path.back().element->FindAllBuildCache(
                    TreeScope_Children, condition, cache, &children)) ||
                !children) {
                break;
            }
            int length = 0;
            children->get_Length(&length);
            Node best{nullptr, {}, 0};
            double bestArea = 0;
            for (int i = 0; i < length && i < 2000; i++) {
                IUIAutomationElement* child = nullptr;
                if (FAILED(children->GetElement(i, &child)) || !child) {
                    continue;
                }
                RECT rc{};
                BOOL offscreen = FALSE;
                CONTROLTYPEID type = 0;
                child->get_CachedBoundingRectangle(&rc);
                child->get_CachedIsOffscreen(&offscreen);
                child->get_CachedControlType(&type);
                const double childArea = area(rc);
                if (!offscreen && childArea > 0 && PtInRect(&rc, pt) &&
                    (!best.element || childArea <= bestArea)) {
                    if (best.element) {
                        best.element->Release();
                    }
                    best = {child, rc, type};
                    bestArea = childArea;
                } else {
                    child->Release();
                }
            }
            children->Release();
            if (!best.element) {
                break;
            }
            path.push_back(best);
        }

        int chosen = -1;
        int fallback = -1;
        for (int i = static_cast<int>(path.size()) - 1; i >= 0; i--) {
            const Node& node = path[i];
            if (!reasonable(node.rect, 0.85)) {
                if (area(node.rect) > monitorArea * 0.85) {
                    break;
                }
                continue;
            }
            if (node.type == UIA_TextControlTypeId) {
                if (i > 0 && IsLabelContainer(path[i - 1].type) &&
                    reasonable(path[i - 1].rect, 0.85)) {
                    continue;
                }
                chosen = i;
                break;
            }
            if (IsMeaningfulType(node.type)) {
                chosen = i;
                break;
            }
            if (fallback < 0 && i > 0 && reasonable(node.rect, 0.6) &&
                node.rect.right - node.rect.left >= 24 &&
                node.rect.bottom - node.rect.top >= 16) {
                fallback = i;
            }
        }
        if (chosen < 0) {
            chosen = fallback;
        }

        if (chosen >= 0) {
            out.found = true;
            out.rect = path[chosen].rect;
            BSTR name = nullptr;
            if (SUCCEEDED(path[chosen].element->get_CachedName(&name)) && name) {
                out.text = Trim(std::wstring(name, SysStringLen(name)));
                SysFreeString(name);
            }
            if (out.text.size() > 4000) {
                out.text.resize(4000);
            }
        } else {
            fallbackToWindow();
        }
        release();
    }

    HANDLE thread_ = nullptr;
    HANDLE event_ = nullptr;
    HWND notify_ = nullptr;
    std::atomic<bool> stop_{false};
    std::mutex mutex_;
    POINT pendingPoint_{};
    uint32_t pendingId_ = 0;
    uint32_t lastId_ = 0;
    bool hasPending_ = false;
};

////////////////////////////////////////////////////////////////////////////////
// Screen capture, PNG and the clipboard

bool CaptureScreen(const RECT& rect, Image& out) {
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) {
        return false;
    }
    HDC screen = GetDC(nullptr);
    if (!screen) {
        return false;
    }
    HDC memory = CreateCompatibleDC(screen);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap =
        CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    bool ok = false;
    if (memory && bitmap && bits) {
        HGDIOBJ old = SelectObject(memory, bitmap);
        ok = BitBlt(memory, 0, 0, width, height, screen, rect.left, rect.top,
                    SRCCOPY | CAPTUREBLT);
        GdiFlush();
        SelectObject(memory, old);
        if (ok) {
            const uint32_t* pixels = static_cast<const uint32_t*>(bits);
            out.width = width;
            out.height = height;
            out.pixels.assign(pixels, pixels + static_cast<size_t>(width) * height);
            for (uint32_t& pixel : out.pixels) {
                pixel |= 0xFF000000;
            }
        }
    }
    if (bitmap) {
        DeleteObject(bitmap);
    }
    if (memory) {
        DeleteDC(memory);
    }
    ReleaseDC(nullptr, screen);
    return ok;
}

bool EncodeImage(const Image& image,
                 std::vector<uint8_t>& out,
                 bool jpeg,
                 float quality = 0.9f) {
    if (image.Empty()) {
        return false;
    }
    ComPtr<IWICImagingFactory> factory;
    ComPtr<IStream> stream;
    ComPtr<IWICBitmapEncoder> encoder;
    ComPtr<IWICBitmapFrameEncode> frame;
    ComPtr<IPropertyBag2> properties;
    if (FAILED(CoCreateInstance(kClsidWICImagingFactory1, nullptr,
                                CLSCTX_INPROC_SERVER, kIidIWICImagingFactory,
                                factory.PutVoid())) ||
        FAILED(CreateStreamOnHGlobal(nullptr, TRUE, stream.Put())) ||
        FAILED(factory->CreateEncoder(
            jpeg ? kGuidContainerFormatJpeg : kGuidContainerFormatPng, nullptr,
            encoder.Put())) ||
        FAILED(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache)) ||
        FAILED(encoder->CreateNewFrame(frame.Put(), properties.Put()))) {
        return false;
    }
    if (jpeg && properties) {
        PROPBAG2 option{};
        option.pstrName = const_cast<LPOLESTR>(L"ImageQuality");
        VARIANT value;
        VariantInit(&value);
        value.vt = VT_R4;
        value.fltVal = quality;
        properties->Write(1, &option, &value);
    }
    if (FAILED(frame->Initialize(properties.Get())) ||
        FAILED(frame->SetSize(image.width, image.height))) {
        return false;
    }
    WICPixelFormatGUID format = kGuidWICPixelFormat24bppBGR;
    if (FAILED(frame->SetPixelFormat(&format)) ||
        !IsEqualGUID(format, kGuidWICPixelFormat24bppBGR)) {
        return false;
    }
    const UINT stride = (static_cast<UINT>(image.width) * 3 + 3) & ~3u;
    std::vector<BYTE> rows(static_cast<size_t>(stride) * image.height);
    for (int y = 0; y < image.height; y++) {
        BYTE* dst = &rows[static_cast<size_t>(y) * stride];
        const uint32_t* src = &image.pixels[static_cast<size_t>(y) * image.width];
        for (int x = 0; x < image.width; x++) {
            dst[x * 3] = src[x] & 0xFF;
            dst[x * 3 + 1] = (src[x] >> 8) & 0xFF;
            dst[x * 3 + 2] = (src[x] >> 16) & 0xFF;
        }
    }
    if (FAILED(frame->WritePixels(image.height, stride,
                                  static_cast<UINT>(rows.size()), rows.data())) ||
        FAILED(frame->Commit()) || FAILED(encoder->Commit())) {
        return false;
    }

    HGLOBAL global = nullptr;
    STATSTG stat{};
    if (FAILED(GetHGlobalFromStream(stream.Get(), &global)) ||
        FAILED(stream->Stat(&stat, STATFLAG_NONAME))) {
        return false;
    }
    const size_t size = static_cast<size_t>(stat.cbSize.QuadPart);
    const void* data = GlobalLock(global);
    if (!data) {
        return false;
    }
    out.assign(static_cast<const uint8_t*>(data),
               static_cast<const uint8_t*>(data) + size);
    GlobalUnlock(global);
    return !out.empty();
}

bool EncodePng(const Image& image, std::vector<uint8_t>& out) {
    return EncodeImage(image, out, false);
}

bool OpenClipboardWithRetry(HWND owner) {
    for (int attempt = 0; attempt < 10; attempt++) {
        if (OpenClipboard(owner)) {
            return true;
        }
        Sleep(15);
    }
    return false;
}

HGLOBAL GlobalFromBytes(const void* data, size_t size) {
    HGLOBAL global = GlobalAlloc(GMEM_MOVEABLE, size);
    if (global) {
        void* dst = GlobalLock(global);
        if (dst) {
            memcpy(dst, data, size);
            GlobalUnlock(global);
        } else {
            GlobalFree(global);
            global = nullptr;
        }
    }
    return global;
}

struct ClipboardImageData {
    std::vector<BYTE> dib, dibV5;
    std::vector<uint8_t> png;
};

bool BuildClipboardImageData(const Image& image, ClipboardImageData& data) {
    data = {};
    if (image.width <= 0 || image.height <= 0 ||
        static_cast<uint64_t>(image.width) * image.height > UINT_MAX / 4 ||
        image.pixels.size() < static_cast<uint64_t>(image.width) * image.height) return false;
    const UINT stride = (static_cast<UINT>(image.width) * 3 + 3) & ~3u;
    data.dib.resize(sizeof(BITMAPINFOHEADER) + static_cast<size_t>(stride) * image.height);
    BITMAPINFOHEADER* header = reinterpret_cast<BITMAPINFOHEADER*>(data.dib.data());
    header->biSize = sizeof(BITMAPINFOHEADER);
    header->biWidth = image.width;
    header->biHeight = image.height;
    header->biPlanes = 1;
    header->biBitCount = 24;
    header->biCompression = BI_RGB;
    header->biSizeImage = stride * image.height;
    BYTE* pixels = data.dib.data() + sizeof(BITMAPINFOHEADER);
    for (int y = 0; y < image.height; y++) {
        BYTE* dst = pixels + static_cast<size_t>(image.height - 1 - y) * stride;
        const uint32_t* src = &image.pixels[static_cast<size_t>(y) * image.width];
        for (int x = 0; x < image.width; x++) {
            dst[x * 3] = src[x] & 0xFF;
            dst[x * 3 + 1] = (src[x] >> 8) & 0xFF;
            dst[x * 3 + 2] = (src[x] >> 16) & 0xFF;
        }
    }

    const size_t pixelBytes = static_cast<size_t>(image.width) * image.height * 4;
    data.dibV5.resize(sizeof(BITMAPV5HEADER) + pixelBytes);
    auto* v5 = reinterpret_cast<BITMAPV5HEADER*>(data.dibV5.data());
    v5->bV5Size = sizeof(BITMAPV5HEADER);
    v5->bV5Width = image.width;
    v5->bV5Height = -image.height;
    v5->bV5Planes = 1;
    v5->bV5BitCount = 32;
    v5->bV5Compression = BI_BITFIELDS;
    v5->bV5SizeImage = static_cast<DWORD>(pixelBytes);
    v5->bV5RedMask = 0x00FF0000;
    v5->bV5GreenMask = 0x0000FF00;
    v5->bV5BlueMask = 0x000000FF;
    v5->bV5AlphaMask = 0xFF000000;
    v5->bV5CSType = LCS_sRGB;
    v5->bV5Intent = LCS_GM_IMAGES;
    auto* opaquePixels = reinterpret_cast<uint32_t*>(data.dibV5.data() + sizeof(BITMAPV5HEADER));
    for (size_t i = 0; i < pixelBytes / 4; i++) opaquePixels[i] = image.pixels[i] | 0xFF000000u;
    EncodePng(image, data.png);
    return true;
}

bool CopyImageToClipboard(HWND owner, const Image& image) {
    ClipboardImageData data;
    if (!BuildClipboardImageData(image, data)) return false;
    struct Format {
        UINT type;
        HGLOBAL memory;
    } formats[]{
        {RegisterClipboardFormatW(L"PNG"), data.png.empty() ? nullptr : GlobalFromBytes(data.png.data(), data.png.size())},
        {CF_DIBV5, GlobalFromBytes(data.dibV5.data(), data.dibV5.size())},
        {CF_DIB, GlobalFromBytes(data.dib.data(), data.dib.size())},
    };
    bool ok = false;
    if (OpenClipboardWithRetry(owner)) {
        if (EmptyClipboard()) {
            for (Format& format : formats) {
                if (format.type && format.memory && SetClipboardData(format.type, format.memory)) {
                    format.memory = nullptr;  // Ownership passed to Windows.
                    ok = true;
                }
            }
        }
        CloseClipboard();
    }
    for (Format& format : formats) if (format.memory) GlobalFree(format.memory);
    return ok;
}

std::vector<BYTE> BuildClipboardFileDrop(const std::vector<std::wstring>& paths) {
    size_t characters = 1;  // The list ends with one more terminator.
    for (const std::wstring& path : paths) {
        if (path.empty() || path.find(L'\0') != std::wstring::npos) return {};
        characters += path.size() + 1;
    }
    if (paths.empty()) return {};
    std::vector<BYTE> bytes(sizeof(DROPFILES) + characters * sizeof(wchar_t), 0);
    auto* drop = reinterpret_cast<DROPFILES*>(bytes.data());
    drop->pFiles = sizeof(DROPFILES);
    drop->fWide = TRUE;
    auto* names = reinterpret_cast<wchar_t*>(bytes.data() + sizeof(DROPFILES));
    for (const std::wstring& path : paths) {
        memcpy(names, path.data(), path.size() * sizeof(wchar_t));
        names += path.size() + 1;  // Its terminator is already zero.
    }
    return bytes;
}

bool CopyImageFileToClipboard(HWND owner, const std::vector<std::wstring>& paths,
                              DWORD expectedSequence = 0) {
    for (const std::wstring& path : paths) {
        const DWORD attributes = GetFileAttributesW(path.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY)) return false;
    }
    const auto bytes = BuildClipboardFileDrop(paths);
    HGLOBAL drop = bytes.empty() ? nullptr : GlobalFromBytes(bytes.data(), bytes.size());
    const DWORD effect = DROPEFFECT_COPY;
    HGLOBAL copyEffect = GlobalFromBytes(&effect, sizeof(effect));
    const UINT effectFormat = RegisterClipboardFormatW(L"Preferred DropEffect");
    bool ok = false;
    if (drop && OpenClipboardWithRetry(owner)) {
        if ((!expectedSequence || GetClipboardSequenceNumber() == expectedSequence) &&
            EmptyClipboard() && SetClipboardData(CF_HDROP, drop)) {
            drop = nullptr;
            ok = true;
            if (effectFormat && copyEffect && SetClipboardData(effectFormat, copyEffect)) copyEffect = nullptr;
        }
        CloseClipboard();
    }
    if (drop) GlobalFree(drop);
    if (copyEffect) GlobalFree(copyEffect);
    return ok;
}

std::wstring g_storageDir;
extern std::atomic<bool> g_stopping;
constexpr double kGeminiFileLifetimeMs = 30.0 * 60 * 1000;
struct TemporaryGeminiFile { std::wstring path; double expires; };
std::mutex g_geminiFilesMutex;
std::vector<TemporaryGeminiFile> g_geminiFiles;

double CleanupGeminiFiles(double now) {
    double next = 1e9;
    std::lock_guard<std::mutex> lock(g_geminiFilesMutex);
    for (auto it = g_geminiFiles.begin(); it != g_geminiFiles.end();) {
        if (now >= it->expires) {
            if (DeleteFileW(it->path.c_str()) || GetLastError() == ERROR_FILE_NOT_FOUND) {
                it = g_geminiFiles.erase(it);
                continue;
            }
            it->expires = now + 60000;  // An upload may still have the file open.
        }
        next = std::min(next, it->expires - now);
        ++it;
    }
    return next;
}

void DeleteExpiredGeminiFiles(bool all = false) {
    std::lock_guard<std::mutex> lock(g_geminiFilesMutex);
    if (g_storageDir.empty()) return;
    WIN32_FIND_DATAW data{};
    HANDLE find = FindFirstFileW((g_storageDir + L"\\gemini-selection-*.png").c_str(), &data);
    if (find == INVALID_HANDLE_VALUE) return;
    FILETIME current{};
    GetSystemTimeAsFileTime(&current);
    auto ticks = [](FILETIME time) { return (static_cast<uint64_t>(time.dwHighDateTime) << 32) | time.dwLowDateTime; };
    do {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            (all || (ticks(current) >= ticks(data.ftLastWriteTime) &&
             ticks(current) - ticks(data.ftLastWriteTime) >= static_cast<uint64_t>(kGeminiFileLifetimeMs * 10000)))) {
            DeleteFileW((g_storageDir + L"\\" + data.cFileName).c_str());
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
}

std::wstring WriteGeminiImageFile(const Image& image, HWND notify) {
    if (g_storageDir.empty() || image.width <= 0 || image.height <= 0 ||
        image.pixels.size() != static_cast<size_t>(image.width) * image.height) return {};
    std::vector<uint8_t> png;
    if (!EncodePng(image, png) || png.size() > MAXDWORD) return {};
    // Serialize file creation with shutdown cleanup so an in-flight handoff
    // cannot leave a new screenshot behind after the mod is disabled.
    std::lock_guard<std::mutex> lock(g_geminiFilesMutex);
    if (g_stopping) return {};
    static std::atomic<unsigned> sequence{0};
    const std::wstring path = g_storageDir + L"\\gemini-selection-" +
        std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(GetTickCount64()) +
        L"-" + std::to_wstring(++sequence) + L".png";
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                              CREATE_NEW, FILE_ATTRIBUTE_TEMPORARY, nullptr);
    if (file == INVALID_HANDLE_VALUE) return {};
    DWORD written = 0;
    const bool ok = WriteFile(file, png.data(), static_cast<DWORD>(png.size()), &written, nullptr) && written == png.size();
    CloseHandle(file);  // Close before Chrome opens the file.
    if (!ok) { DeleteFileW(path.c_str()); return {}; }
    g_geminiFiles.push_back({path, NowMs() + kGeminiFileLifetimeMs});
    if (notify) PostMessageW(notify, WM_APP_TEMP_FILE, 0, 0);
    return path;
}

bool CopyTextToClipboard(HWND owner, const std::wstring& text) {
    HGLOBAL global =
        GlobalFromBytes(text.c_str(), (text.size() + 1) * sizeof(wchar_t));
    if (!global) {
        return false;
    }
    if (!OpenClipboardWithRetry(owner)) {
        GlobalFree(global);
        return false;
    }
    EmptyClipboard();
    const bool ok = SetClipboardData(CF_UNICODETEXT, global) != nullptr;
    if (!ok) {
        GlobalFree(global);
    }
    CloseClipboard();
    return ok;
}

std::wstring ReadClipboardText(HWND owner) {
    std::wstring text;
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT) ||
        !OpenClipboardWithRetry(owner)) {
        return text;
    }
    if (HANDLE data = GetClipboardData(CF_UNICODETEXT)) {
        if (const wchar_t* chars = static_cast<const wchar_t*>(GlobalLock(data))) {
            text = chars;
            GlobalUnlock(data);
        }
    }
    CloseClipboard();
    return text;
}

////////////////////////////////////////////////////////////////////////////////
// Opening the browser

std::atomic<int> g_helperThreads{0};

std::wstring Lowercase(std::wstring text) {
    for (wchar_t& c : text) c = towlower(c);
    return text;
}

std::wstring FileNameOf(const std::wstring& path) {
    const size_t slash = path.find_last_of(L"\\/");
    return Lowercase(slash == std::wstring::npos ? path : path.substr(slash + 1));
}

bool IsProcessElevated() {
    static const bool elevated = [] {
        HANDLE token;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            return false;
        }
        TOKEN_ELEVATION elevation{};
        DWORD size = sizeof(elevation);
        const bool result = GetTokenInformation(token, TokenElevation,
                                                &elevation, sizeof(elevation),
                                                &size) &&
                            elevation.TokenIsElevated;
        CloseHandle(token);
        return result;
    }();
    return elevated;
}

bool ShellExecuteThroughExplorer(const std::wstring& target,
                                 const std::wstring& parameters = {}) {
    ComPtr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(kClsidShellWindows, nullptr,
                                CLSCTX_LOCAL_SERVER, kIidIShellWindows,
                                shellWindows.PutVoid()))) {
        return false;
    }
    VARIANT empty;
    VariantInit(&empty);
    long hwnd = 0;
    ComPtr<IDispatch> dispatch;
    if (shellWindows->FindWindowSW(&empty, &empty, SWC_DESKTOP, &hwnd,
                                   SWFO_NEEDDISPATCH, dispatch.Put()) != S_OK ||
        !dispatch) {
        return false;
    }
    ComPtr<IServiceProvider> provider;
    ComPtr<IShellBrowser> browser;
    ComPtr<IShellView> view;
    ComPtr<IDispatch> background;
    ComPtr<IShellFolderViewDual> folderView;
    ComPtr<IDispatch> application;
    ComPtr<IShellDispatch2> shell;
    if (FAILED(dispatch->QueryInterface(kIidIServiceProvider,
                                        provider.PutVoid())) ||
        FAILED(provider->QueryService(kSidSTopLevelBrowser, kIidIShellBrowser,
                                      browser.PutVoid())) ||
        FAILED(browser->QueryActiveShellView(view.Put())) ||
        FAILED(view->GetItemObject(SVGIO_BACKGROUND, kIidIDispatch,
                                   background.PutVoid())) ||
        FAILED(background->QueryInterface(kIidIShellFolderViewDual,
                                          folderView.PutVoid())) ||
        FAILED(folderView->get_Application(application.Put())) ||
        FAILED(application->QueryInterface(kIidIShellDispatch2,
                                           shell.PutVoid()))) {
        return false;
    }
    BSTR file = SysAllocString(target.c_str());
    VARIANT none;
    VariantInit(&none);
    VARIANT arguments;
    VariantInit(&arguments);
    if (!parameters.empty()) {
        arguments.vt = VT_BSTR;
        arguments.bstrVal = SysAllocString(parameters.c_str());
    }
    VARIANT show;
    VariantInit(&show);
    show.vt = VT_I4;
    show.lVal = SW_SHOWNORMAL;
    const HRESULT hr = shell->ShellExecute(file, arguments, none, none, show);
    SysFreeString(file);
    VariantClear(&arguments);
    return SUCCEEDED(hr);
}

////////////////////////////////////////////////////////////////////////////////
// Choosing the browser

struct KnownBrowser {
    PCWSTR id;         // The value of the Browser setting.
    PCWSTR process;    // The program whose windows are the browser's.
    PCWSTR program;    // What gets started; differs when a launcher starts it.
    PCWSTR places[4];  // Where it installs when Windows has no record of it.
};

constexpr KnownBrowser kBrowsers[] = {
    {L"chrome", L"chrome.exe", L"chrome.exe",
     {L"%ProgramFiles%\\Google\\Chrome\\Application",
      L"%ProgramFiles(x86)%\\Google\\Chrome\\Application",
      L"%LOCALAPPDATA%\\Google\\Chrome\\Application",
      L"%ProgramW6432%\\Google\\Chrome\\Application"}},
    {L"edge", L"msedge.exe", L"msedge.exe",
     {L"%ProgramFiles(x86)%\\Microsoft\\Edge\\Application",
      L"%ProgramFiles%\\Microsoft\\Edge\\Application",
      L"%ProgramW6432%\\Microsoft\\Edge\\Application", nullptr}},
    {L"firefox", L"firefox.exe", L"firefox.exe",
     {L"%ProgramFiles%\\Mozilla Firefox", L"%ProgramFiles(x86)%\\Mozilla Firefox",
      L"%ProgramW6432%\\Mozilla Firefox", nullptr}},
    {L"brave", L"brave.exe", L"brave.exe",
     {L"%ProgramFiles%\\BraveSoftware\\Brave-Browser\\Application",
      L"%LOCALAPPDATA%\\BraveSoftware\\Brave-Browser\\Application",
      L"%ProgramFiles(x86)%\\BraveSoftware\\Brave-Browser\\Application",
      L"%ProgramW6432%\\BraveSoftware\\Brave-Browser\\Application"}},
    {L"zen", L"zen.exe", L"zen.exe",
     {L"%ProgramFiles%\\Zen Browser", L"%LOCALAPPDATA%\\Zen Browser",
      L"%LOCALAPPDATA%\\Programs\\Zen Browser", L"%ProgramW6432%\\Zen Browser"}},
    {L"opera", L"opera.exe", L"launcher.exe",
     {L"%LOCALAPPDATA%\\Programs\\Opera", L"%ProgramFiles%\\Opera",
      L"%ProgramW6432%\\Opera", nullptr}},
    {L"vivaldi", L"vivaldi.exe", L"vivaldi.exe",
     {L"%LOCALAPPDATA%\\Vivaldi\\Application", L"%ProgramFiles%\\Vivaldi\\Application",
      L"%ProgramW6432%\\Vivaldi\\Application", nullptr}},
    {L"librewolf", L"librewolf.exe", L"librewolf.exe",
     {L"%ProgramFiles%\\LibreWolf", L"%LOCALAPPDATA%\\LibreWolf",
      L"%ProgramW6432%\\LibreWolf", nullptr}},
    {L"waterfox", L"waterfox.exe", L"waterfox.exe",
     {L"%ProgramFiles%\\Waterfox", L"%LOCALAPPDATA%\\Waterfox",
      L"%ProgramW6432%\\Waterfox", nullptr}},
    {L"floorp", L"floorp.exe", L"floorp.exe",
     {L"%ProgramFiles%\\Ablaze Floorp", L"%LOCALAPPDATA%\\Ablaze Floorp",
      L"%ProgramW6432%\\Ablaze Floorp", nullptr}},
};

bool IsKnownBrowserName(const std::wstring& name) {
    for (const KnownBrowser& browser : kBrowsers) {
        if (name == browser.process) return true;
    }
    for (PCWSTR other : {L"browser.exe", L"chromium.exe", L"thorium.exe"}) {
        if (name == other) return true;
    }
    return false;
}

struct BrowserTarget {
    std::wstring file;       // What to start. Empty: the Windows default browser.
    std::wstring process;    // Name of the program whose windows are the browser's.
    bool anyKnown = false;   // Windows of well-known browsers count as well.
};

bool BrowserOwnsProcess(const std::wstring& name, const BrowserTarget& browser) {
    if (!browser.process.empty() && name == browser.process) return true;
    return (browser.process.empty() || browser.anyKnown) && IsKnownBrowserName(name);
}

std::wstring ProgramOfCommand(const std::wstring& line) {
    const std::wstring command = Trim(line);
    if (command.empty()) return {};
    if (command[0] == L'"') {
        const size_t close = command.find(L'"', 1);
        return close == std::wstring::npos ? command.substr(1) : command.substr(1, close - 1);
    }
    const size_t exe = Lowercase(command).find(L".exe");
    return exe != std::wstring::npos ? command.substr(0, exe + 4)
                                     : command.substr(0, command.find(L' '));
}

bool IsFile(const std::wstring& path) {
    const DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::wstring ExpandVariables(const std::wstring& text) {
    WCHAR expanded[MAX_PATH * 2];
    const DWORD size = ExpandEnvironmentStringsW(text.c_str(), expanded, ARRAYSIZE(expanded));
    return size > 0 && size <= ARRAYSIZE(expanded) ? std::wstring(expanded) : text;
}

bool ReadRegistryString(HKEY root, const std::wstring& subkey, PCWSTR name, REGSAM view,
                        std::wstring& value) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(root, subkey.c_str(), 0, KEY_READ | view, &key) != ERROR_SUCCESS) {
        return false;
    }
    WCHAR buffer[1024];
    DWORD size = sizeof(buffer);
    const LSTATUS status = RegGetValueW(key, nullptr, name, RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ,
                                        nullptr, buffer, &size);
    RegCloseKey(key);
    value = status == ERROR_SUCCESS ? buffer : L"";
    return !value.empty();
}

std::wstring FindBrowserProgram(const KnownBrowser& browser) {
    struct Hive {
        HKEY root;
        REGSAM view;
    };
    const Hive hives[] = {{HKEY_CURRENT_USER, 0},
                          {HKEY_LOCAL_MACHINE, KEY_WOW64_64KEY},
                          {HKEY_LOCAL_MACHINE, KEY_WOW64_32KEY}};
    for (PCWSTR exe : {browser.process, browser.program}) {
        for (const Hive& hive : hives) {
            std::wstring path;
            if (ReadRegistryString(
                    hive.root,
                    std::wstring(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\") + exe,
                    nullptr, hive.view, path)) {
                path = ProgramOfCommand(path);
                if (IsFile(path)) return path;
            }
        }
    }
    for (const Hive& hive : hives) {
        HKEY clients = nullptr;
        if (RegOpenKeyExW(hive.root, L"SOFTWARE\\Clients\\StartMenuInternet", 0,
                          KEY_READ | hive.view, &clients) != ERROR_SUCCESS) {
            continue;
        }
        WCHAR name[256];
        for (DWORD i = 0;; i++) {
            DWORD length = ARRAYSIZE(name);
            if (RegEnumKeyExW(clients, i, name, &length, nullptr, nullptr, nullptr, nullptr) !=
                ERROR_SUCCESS) {
                break;
            }
            std::wstring command;
            if (!ReadRegistryString(
                    hive.root,
                    std::wstring(L"SOFTWARE\\Clients\\StartMenuInternet\\") + name +
                        L"\\shell\\open\\command",
                    nullptr, hive.view, command)) {
                continue;
            }
            const std::wstring path = ProgramOfCommand(command);
            const std::wstring file = FileNameOf(path);
            if ((file == browser.process || file == browser.program) && IsFile(path)) {
                RegCloseKey(clients);
                return path;
            }
        }
        RegCloseKey(clients);
    }
    for (PCWSTR place : browser.places) {
        if (!place) continue;
        for (PCWSTR exe : {browser.program, browser.process}) {
            const std::wstring path = ExpandVariables(place) + L"\\" + exe;
            if (IsFile(path)) return path;
        }
    }
    return {};
}

std::wstring DefaultBrowserProcess() {
    std::wstring progId;
    std::wstring command;
    if (!ReadRegistryString(HKEY_CURRENT_USER,
                            L"Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\https\\UserChoice",
                            L"ProgId", 0, progId) ||
        !ReadRegistryString(HKEY_CLASSES_ROOT, progId + L"\\shell\\open\\command", nullptr, 0,
                            command)) {
        return {};
    }
    const std::wstring program = FileNameOf(ProgramOfCommand(command));
    return program == L"launcher.exe" ? std::wstring(L"opera.exe") : program;
}

BrowserTarget DefaultBrowser() {
    BrowserTarget browser;
    browser.process = DefaultBrowserProcess();
    browser.anyKnown = !browser.process.empty() && !IsKnownBrowserName(browser.process);
    return browser;
}

BrowserTarget ResolveBrowser(const std::wstring& choice, const std::wstring& customPath) {
    const std::wstring id = Lowercase(Trim(choice));
    if (id == L"custom") {
        std::wstring path = Trim(customPath);
        if (path.size() > 1 && path.front() == L'"' && path.back() == L'"') {
            path = Trim(path.substr(1, path.size() - 2));
        }
        path = ExpandVariables(path);
        if (IsFile(path)) {
            BrowserTarget browser;
            browser.file = path;
            browser.process = FileNameOf(path);
            browser.anyKnown = true;
            return browser;
        }
        return DefaultBrowser();
    }
    for (const KnownBrowser& known : kBrowsers) {
        if (id == known.id) {
            const std::wstring file = FindBrowserProgram(known);
            if (file.empty()) break;
            BrowserTarget browser;
            browser.file = file;
            browser.process = known.process;
            return browser;
        }
    }
    return DefaultBrowser();  // "default", or a browser that isn't installed.
}

BrowserTarget SelectedBrowser() {
    return ResolveBrowser(g_settings.browser, g_settings.browserPath);
}

bool OpenWithBrowser(const std::wstring& target, BrowserTarget& browser) {
    if (!browser.file.empty()) {
        const std::wstring parameters = L'"' + target + L'"';
        if ((IsProcessElevated() && ShellExecuteThroughExplorer(browser.file, parameters)) ||
            reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, L"open", browser.file.c_str(),
                                                    parameters.c_str(), nullptr,
                                                    SW_SHOWNORMAL)) > 32) {
            return true;
        }
        Wh_Log(L"Couldn't start %s", browser.file.c_str());
        browser = DefaultBrowser();
    }
    return (IsProcessElevated() && ShellExecuteThroughExplorer(target)) ||
           reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, L"open", target.c_str(), nullptr,
                                                   nullptr, SW_SHOWNORMAL)) > 32;
}

struct OpenJob {
    std::wstring target;
    BrowserTarget browser;
};

DWORD WINAPI OpenThreadProc(LPVOID param) {
    std::unique_ptr<OpenJob> job(static_cast<OpenJob*>(param));
    const HRESULT coInit =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (!OpenWithBrowser(job->target, job->browser)) {
        Wh_Log(L"Couldn't open %s", job->target.c_str());
    }
    if (SUCCEEDED(coInit)) {
        CoUninitialize();
    }
    g_helperThreads--;
    return 0;
}

void OpenInBrowser(const std::wstring& target, const BrowserTarget& browser) {
    auto* param = new OpenJob{target, browser};
    g_helperThreads++;
    HANDLE thread = CreateThread(nullptr, 0, OpenThreadProc, param, 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        g_helperThreads--;
        delete param;
    }
}

extern std::atomic<bool> g_stopping;

struct BrowserHandoffJob {
    uint32_t id = 0;
    HWND notify = nullptr;
    DWORD clipboardSequence = 0;
    std::wstring prompt;
    std::vector<std::shared_ptr<const Image>> images;  // Everything the user selected.
    std::vector<std::wstring> imagePaths;              // The PNG files made from them.
    std::vector<std::wstring> onClipboard;             // The files the clipboard should hold right now.
    BrowserTarget browser;  // Where Gemini opens, and whose windows get the draft.
    std::atomic<bool> canceled{false};
};

struct BrowserHandoffMessage {
    uint32_t id = 0;
    bool pasted = false;
    std::wstring status;
};

bool IsGeminiChatUrl(std::wstring value) {
    value = Lowercase(Trim(value));
    if (value.rfind(L"https://", 0) == 0) value.erase(0, 8);
    constexpr PCWSTR host = L"gemini.google.com/";
    if (value.rfind(host, 0) != 0) return false;
    size_t i = wcslen(host);
    if (value.compare(i, 2, L"u/") == 0) {
        i += 2;
        const size_t digits = i;
        while (i < value.size() && iswdigit(value[i])) i++;
        if (i == digits || i >= value.size() || value[i] != L'/') return false;
        i++;
    }
    if (value.compare(i, 3, L"app") != 0) return false;
    i += 3;
    return i == value.size() || value[i] == L'/' || value[i] == L'?' || value[i] == L'#';
}

bool PastesSeveralFiles(const std::wstring& process) {
    for (PCWSTR name : {L"chrome.exe", L"msedge.exe", L"brave.exe", L"opera.exe", L"vivaldi.exe",
                        L"browser.exe", L"chromium.exe", L"thorium.exe"}) {
        if (process == name) return true;
    }
    return false;
}

bool IsTargetBrowserWindow(HWND hwnd, const BrowserTarget& browser) {
    return BrowserOwnsProcess(FileNameOf(ProcessPathOfWindow(hwnd)), browser);
}

std::wstring CachedElementString(IUIAutomationElement* element, PROPERTYID property) {
    VARIANT value;
    VariantInit(&value);
    std::wstring text;
    if (SUCCEEDED(element->GetCachedPropertyValue(property, &value)) && value.vt == VT_BSTR && value.bstrVal) {
        text.assign(value.bstrVal, SysStringLen(value.bstrVal));
    }
    VariantClear(&value);
    return text;
}

std::wstring CurrentElementString(IUIAutomationElement* element, PROPERTYID property) {
    VARIANT value;
    VariantInit(&value);
    std::wstring text;
    if (SUCCEEDED(element->GetCurrentPropertyValue(property, &value)) && value.vt == VT_BSTR && value.bstrVal) {
        text.assign(value.bstrVal, SysStringLen(value.bstrVal));
    }
    VariantClear(&value);
    return text;
}

ComPtr<IUIAutomationElement> ContainingDocument(IUIAutomationTreeWalker* walker,
                                               IUIAutomationElement* element) {
    ComPtr<IUIAutomationElement> current;
    element->AddRef();
    *current.Put() = element;
    for (int depth = 0; depth < 32 && current; depth++) {
        CONTROLTYPEID type = 0;
        if (SUCCEEDED(current->get_CurrentControlType(&type)) && type == UIA_DocumentControlTypeId) {
            return current;
        }
        ComPtr<IUIAutomationElement> parent;
        if (FAILED(walker->GetParentElement(current.Get(), parent.Put()))) break;
        current = std::move(parent);
    }
    return {};
}

ComPtr<IUIAutomationElement> FindGeminiComposer(IUIAutomation* automation,
                                               HWND browser, bool& verifiedPage,
                                               bool* sawDocument = nullptr) {
    verifiedPage = false;
    if (sawDocument) *sawDocument = false;
    ComPtr<IUIAutomationElement> root;
    ComPtr<IUIAutomationTreeWalker> walker;
    ComPtr<IUIAutomationCondition> editCondition, documentCondition;
    ComPtr<IUIAutomationCacheRequest> cache;
    VARIANT type;
    VariantInit(&type);
    type.vt = VT_I4;
    if (FAILED(automation->ElementFromHandle(browser, root.Put())) || !root ||
        FAILED(automation->get_ControlViewWalker(walker.Put())) || !walker ||
        FAILED(automation->CreateCacheRequest(cache.Put()))) return {};
    type.lVal = UIA_EditControlTypeId;
    if (FAILED(automation->CreatePropertyCondition(UIA_ControlTypePropertyId, type, editCondition.Put()))) return {};
    type.lVal = UIA_DocumentControlTypeId;
    if (FAILED(automation->CreatePropertyCondition(UIA_ControlTypePropertyId, type, documentCondition.Put()))) return {};
    for (PROPERTYID property : {UIA_NamePropertyId, UIA_ClassNamePropertyId,
                                UIA_ValueValuePropertyId, UIA_IsOffscreenPropertyId,
                                UIA_IsEnabledPropertyId, UIA_IsKeyboardFocusablePropertyId}) {
        cache->AddProperty(property);
    }

    ComPtr<IUIAutomationElementArray> documents;
    if (SUCCEEDED(root->FindAllBuildCache(TreeScope_Descendants, documentCondition.Get(), cache.Get(), documents.Put())) &&
        documents) {
        int count = 0;
        documents->get_Length(&count);
        if (sawDocument) *sawDocument = count > 0;
        for (int i = 0; i < std::min(count, 64); i++) {
            ComPtr<IUIAutomationElement> document;
            BOOL offscreen = TRUE;
            if (SUCCEEDED(documents->GetElement(i, document.Put())) && document &&
                SUCCEEDED(document->get_CachedIsOffscreen(&offscreen)) && !offscreen &&
                IsGeminiChatUrl(CachedElementString(document.Get(), UIA_ValueValuePropertyId))) {
                verifiedPage = true;
            }
        }
    }

    ComPtr<IUIAutomationElementArray> edits;
    if (FAILED(root->FindAllBuildCache(TreeScope_Descendants, editCondition.Get(), cache.Get(), edits.Put())) || !edits) {
        return {};
    }
    int count = 0;
    edits->get_Length(&count);
    struct Candidate {
        ComPtr<IUIAutomationElement> edit;
        int score;
    };
    std::vector<Candidate> candidates;
    bool addressBar = false;
    for (int i = 0; i < std::min(count, 128); i++) {
        ComPtr<IUIAutomationElement> edit;
        if (FAILED(edits->GetElement(i, edit.Put())) || !edit) continue;
        BOOL offscreen = TRUE, enabled = FALSE, focusable = FALSE;
        edit->get_CachedIsOffscreen(&offscreen);
        edit->get_CachedIsEnabled(&enabled);
        edit->get_CachedIsKeyboardFocusable(&focusable);
        if (offscreen || !enabled) continue;
        if (IsGeminiChatUrl(CachedElementString(edit.Get(), UIA_ValueValuePropertyId)) &&
            !ContainingDocument(walker.Get(), edit.Get())) {
            addressBar = true;
            continue;
        }
        if (!focusable) continue;
        const std::wstring name = Lowercase(CachedElementString(edit.Get(), UIA_NamePropertyId));
        const std::wstring className = Lowercase(CachedElementString(edit.Get(), UIA_ClassNamePropertyId));
        if (name.find(L"search") != std::wstring::npos) continue;
        const int score = name.find(L"gemini") != std::wstring::npos ? 30
                          : name.find(L"prompt") != std::wstring::npos ? 20
                          : className.find(L"ql-editor") != std::wstring::npos ? 20 : 0;
        if (score) candidates.push_back({std::move(edit), score});
    }
    if (addressBar) verifiedPage = true;

    ComPtr<IUIAutomationElement> best;
    int bestScore = -1;
    bool tied = false;
    for (Candidate& candidate : candidates) {
        const auto document = ContainingDocument(walker.Get(), candidate.edit.Get());
        if (!document) continue;  // Not in a web page.
        const std::wstring address = CurrentElementString(document.Get(), UIA_ValueValuePropertyId);
        if (address.empty() ? !addressBar : !IsGeminiChatUrl(address)) continue;
        if (!address.empty()) verifiedPage = true;
        if (candidate.score > bestScore) {
            bestScore = candidate.score;
            best = std::move(candidate.edit);
            tied = false;
        } else if (candidate.score == bestScore) {
            tied = true;
        }
    }
    if (tied) return {};
    return best;
}

struct BrowserAttachmentSnapshot {
    bool readable = false;
    std::vector<std::wstring> items;
};

std::wstring ElementRuntimeKey(IUIAutomationElement* element) {
    SAFEARRAY* id = nullptr;
    if (FAILED(element->GetRuntimeId(&id)) || !id) return {};
    LONG first = 0, last = -1;
    LONG* values = nullptr;
    std::wstring key;
    if (SUCCEEDED(SafeArrayGetLBound(id, 1, &first)) &&
        SUCCEEDED(SafeArrayGetUBound(id, 1, &last)) && last >= first && last - first < 64 &&
        SUCCEEDED(SafeArrayAccessData(id, reinterpret_cast<void**>(&values)))) {
        for (LONG i = 0; i <= last - first; i++) key += std::to_wstring(values[i]) + L":";
        SafeArrayUnaccessData(id);
    }
    SafeArrayDestroy(id);
    return key;
}

BrowserAttachmentSnapshot ReadBrowserAttachments(IUIAutomation* automation, HWND browser,
                                                 const RECT& editor) {
    BrowserAttachmentSnapshot result;
    ComPtr<IUIAutomationElement> root;
    ComPtr<IUIAutomationCondition> condition;
    ComPtr<IUIAutomationCacheRequest> cache;
    ComPtr<IUIAutomationElementArray> elements;
    VARIANT type;
    VariantInit(&type);
    type.vt = VT_I4;
    type.lVal = UIA_ImageControlTypeId;
    int count = 0;
    if (FAILED(automation->ElementFromHandle(browser, root.Put())) || !root ||
        FAILED(automation->CreatePropertyCondition(UIA_ControlTypePropertyId, type, condition.Put())) ||
        FAILED(automation->CreateCacheRequest(cache.Put()))) return result;
    cache->AddProperty(UIA_IsOffscreenPropertyId);
    cache->AddProperty(UIA_BoundingRectanglePropertyId);
    if (FAILED(root->FindAllBuildCache(TreeScope_Descendants, condition.Get(), cache.Get(), elements.Put())) ||
        !elements || FAILED(elements->get_Length(&count))) return result;
    result.readable = true;
    const float scale = DpiScaleAt(RectCenter(editor));
    const RECT around{editor.left - static_cast<LONG>(140 * scale), editor.top - static_cast<LONG>(360 * scale),
                      editor.right + static_cast<LONG>(440 * scale), editor.bottom + static_cast<LONG>(60 * scale)};
    for (int i = 0; i < std::min(count, 256); i++) {
        ComPtr<IUIAutomationElement> element;
        BOOL offscreen = TRUE;
        RECT bounds{}, overlap;
        if (FAILED(elements->GetElement(i, element.Put())) || !element ||
            FAILED(element->get_CachedIsOffscreen(&offscreen)) || offscreen ||
            FAILED(element->get_CachedBoundingRectangle(&bounds)) ||
            !IntersectRect(&overlap, &bounds, &around)) continue;
        const auto key = ElementRuntimeKey(element.Get());
        if (!key.empty()) result.items.push_back(key);
    }
    return result;
}

bool HasNewBrowserAttachment(const BrowserAttachmentSnapshot& before,
                             const BrowserAttachmentSnapshot& after) {
    if (!before.readable || !after.readable) return false;
    return std::any_of(after.items.begin(), after.items.end(), [&](const std::wstring& item) {
        return std::find(before.items.begin(), before.items.end(), item) == before.items.end();
    });
}

std::vector<INPUT> BrowserImagePasteInputs() {
    std::vector<INPUT> inputs(4);
    for (INPUT& input : inputs) input.type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;
    inputs[1].ki.wVk = 'V';
    inputs[2].ki.wVk = 'V';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    return inputs;
}

std::wstring BrowserDraftText(const std::wstring& prompt) {
    std::wstring text = prompt.substr(0, 4000);
    for (wchar_t& c : text) {
        if (c < L' ' || c == 0x7F) c = L' ';
    }
    return text;
}

std::vector<INPUT> BrowserDraftInputs(const std::wstring& prompt) {
    std::vector<INPUT> inputs;
    const std::wstring text = BrowserDraftText(prompt);
    inputs.reserve(text.size() * 2);
    for (wchar_t c : text) {
        INPUT down{};
        down.type = INPUT_KEYBOARD;
        down.ki.wScan = c;
        down.ki.dwFlags = KEYEVENTF_UNICODE;
        inputs.push_back(down);
        down.ki.dwFlags |= KEYEVENTF_KEYUP;
        inputs.push_back(down);
    }
    return inputs;
}

std::wstring CollapseSpacing(const std::wstring& text) {
    std::wstring out;
    bool pending = false;
    for (wchar_t c : text) {
        if (c == 0x200B) continue;
        if (iswspace(c) || c == 0xA0) {
            pending = !out.empty();
            continue;
        }
        if (pending) out += L' ';
        pending = false;
        out += c;
    }
    return out;
}

bool ContainsIgnoringSpacing(const std::wstring& text, const std::wstring& part) {
    const std::wstring needle = CollapseSpacing(part);
    return needle.empty() || CollapseSpacing(text).find(needle) != std::wstring::npos;
}

bool BrowserStillTarget(HWND expectedBrowser, const BrowserHandoffJob& job) {
    return !g_stopping && !job.canceled && expectedBrowser &&
           GetForegroundWindow() == expectedBrowser;
}

bool BrowserKeysAllowed(HWND expectedBrowser, const BrowserHandoffJob& job) {
    return BrowserStillTarget(expectedBrowser, job) &&
           !(GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
           !(GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
           !(GetAsyncKeyState(VK_MENU) & 0x8000) &&
           !(GetAsyncKeyState(VK_LWIN) & 0x8000) &&
           !(GetAsyncKeyState(VK_RWIN) & 0x8000);
}

bool BrowserPasteStillValid(HWND expectedBrowser, DWORD clipboardSequence,
                            const BrowserHandoffJob& job) {
    return BrowserKeysAllowed(expectedBrowser, job) &&
           GetClipboardSequenceNumber() == clipboardSequence;
}

bool ClipboardHoldsFiles(HWND owner, const std::vector<std::wstring>& paths) {
    bool holds = false;
    if (paths.empty() || !OpenClipboardWithRetry(owner)) return false;
    if (HANDLE drop = GetClipboardData(CF_HDROP)) {
        const HDROP files = static_cast<HDROP>(drop);
        holds = DragQueryFileW(files, 0xFFFFFFFF, nullptr, 0) == paths.size();
        for (UINT i = 0; holds && i < paths.size(); i++) {
            WCHAR name[MAX_PATH * 2]{};
            holds = DragQueryFileW(files, i, name, ARRAYSIZE(name)) &&
                    _wcsicmp(name, paths[i].c_str()) == 0;
        }
    }
    CloseClipboard();
    return holds;
}

bool ClipboardStillOurs(BrowserHandoffJob& job) {
    const DWORD sequence = GetClipboardSequenceNumber();
    if (sequence == job.clipboardSequence) return true;
    if (!ClipboardHoldsFiles(job.notify, job.onClipboard)) return false;
    job.clipboardSequence = sequence;
    return true;
}

bool BrowserComposerStillCurrent(IUIAutomation* automation, IUIAutomationElement* composer,
                                 HWND browser, const BrowserHandoffJob& job, bool forPaste,
                                 bool requireFocus) {
    auto allowed = [&] {
        return forPaste ? BrowserPasteStillValid(browser, job.clipboardSequence, job)
                        : BrowserKeysAllowed(browser, job);
    };
    if (!allowed()) return false;
    bool verified = false;
    auto current = FindGeminiComposer(automation, browser, verified);
    BOOL same = FALSE;
    if (!verified || !current ||
        FAILED(automation->CompareElements(composer, current.Get(), &same)) || !same) return false;
    if (requireFocus) {
        ComPtr<IUIAutomationElement> focused;
        same = FALSE;
        if (FAILED(automation->GetFocusedElement(focused.Put())) || !focused ||
            FAILED(automation->CompareElements(composer, focused.Get(), &same)) || !same) return false;
    }
    return allowed();
}

bool FocusBrowserComposer(IUIAutomation* automation, IUIAutomationElement* composer,
                          HWND browser, const BrowserHandoffJob& job, bool forPaste) {
    auto allowed = [&] {
        return forPaste ? BrowserPasteStillValid(browser, job.clipboardSequence, job)
                        : BrowserKeysAllowed(browser, job);
    };
    if (!BrowserComposerStillCurrent(automation, composer, browser, job, forPaste, false) ||
        FAILED(composer->SetFocus())) return false;
    for (int attempt = 0; attempt < 8; attempt++) {
        ComPtr<IUIAutomationElement> focused;
        BOOL same = FALSE;
        if (SUCCEEDED(automation->GetFocusedElement(focused.Put())) && focused &&
            SUCCEEDED(automation->CompareElements(composer, focused.Get(), &same)) && same) {
            return allowed();
        }
        if (!BrowserStillTarget(browser, job)) return false;
        Sleep(50);
    }
    return false;
}

bool ReadElementText(IUIAutomationElement* element, std::wstring& text) {
    ComPtr<IUIAutomationValuePattern> value;
    BSTR raw = nullptr;
    if (FAILED(element->GetCurrentPatternAs(UIA_ValuePatternId, kIidIUIAutomationValuePattern,
                                            value.PutVoid())) || !value ||
        FAILED(value->get_CurrentValue(&raw)) || !raw) return false;
    text.assign(raw, SysStringLen(raw));
    SysFreeString(raw);
    return true;
}

class SteadyCandidate {
  public:
    static constexpr double kSteadyMs = 800.0;

    bool Update(const std::wstring& key, double now) {
        if (key.empty() || key != key_) {
            key_ = key;
            since_ = now;
            return false;
        }
        return now - since_ >= kSteadyMs;
    }

  private:
    std::wstring key_;
    double since_ = 0;
};

struct BrowserWindowSearch {
    const BrowserTarget* browser;
    std::vector<HWND> windows;
    bool minimized = false;  // Minimized windows count too.
};

BOOL CALLBACK CollectBrowserWindow(HWND hwnd, LPARAM param) {
    auto* search = reinterpret_cast<BrowserWindowSearch*>(param);
    if (IsWindowVisible(hwnd) && (search->minimized || !IsIconic(hwnd)) &&
        IsTargetBrowserWindow(hwnd, *search->browser)) {
        search->windows.push_back(hwnd);
    }
    return TRUE;
}

void BringToFront(HWND window) {
    if (IsIconic(window)) ShowWindow(window, SW_RESTORE);
    const HWND foreground = GetForegroundWindow();
    const DWORD foregroundThread = foreground ? GetWindowThreadProcessId(foreground, nullptr) : 0;
    const DWORD thisThread = GetCurrentThreadId();
    const bool joined = foregroundThread && foregroundThread != thisThread &&
                        AttachThreadInput(thisThread, foregroundThread, TRUE);
    BringWindowToTop(window);
    SetForegroundWindow(window);
    if (joined) AttachThreadInput(thisThread, foregroundThread, FALSE);
}

std::vector<std::wstring> OpenGeminiComposerKeys(IUIAutomation* automation,
                                                 const BrowserTarget& browser) {
    BrowserWindowSearch search{&browser, {}};
    EnumWindows(CollectBrowserWindow, reinterpret_cast<LPARAM>(&search));
    const std::vector<HWND>& windows = search.windows;
    std::vector<std::wstring> keys;
    for (int attempt = 0; attempt < 6; attempt++) {
        keys.clear();
        bool waiting = false;
        for (HWND window : windows) {
            bool verified = false;
            if (auto composer = FindGeminiComposer(automation, window, verified)) {
                const std::wstring key = ElementRuntimeKey(composer.Get());
                if (!key.empty()) keys.push_back(key);
            } else if (verified) {
                waiting = true;
            }
        }
        if (!waiting) break;
        Sleep(150);
    }
    return keys;
}

HWND FindNewGeminiWindow(IUIAutomation* automation, const BrowserTarget& browser,
                         const std::vector<std::wstring>& alreadyOpen) {
    BrowserWindowSearch search{&browser, {}, true};
    EnumWindows(CollectBrowserWindow, reinterpret_cast<LPARAM>(&search));
    HWND minimized = nullptr;
    for (HWND window : search.windows) {
        if (IsIconic(window)) {  // Its page can't be looked into: the title has to do.
            WCHAR title[256]{};
            GetWindowTextW(window, title, ARRAYSIZE(title));
            if (!minimized && Lowercase(title).find(L"gemini") != std::wstring::npos) minimized = window;
            continue;
        }
        bool verified = false;
        auto composer = FindGeminiComposer(automation, window, verified);
        if (!composer) continue;
        const std::wstring key = ElementRuntimeKey(composer.Get());
        if (!key.empty() && std::find(alreadyOpen.begin(), alreadyOpen.end(), key) == alreadyOpen.end()) {
            return window;
        }
    }
    return minimized;
}

std::wstring BrowserDraftStatus(size_t total, size_t added, bool questionTyped, bool clipboardOurs) {
    const bool several = total > 1;
    const std::wstring images = several ? L"images" : L"image";
    if (added >= total) return L"The " + images + (several ? L" were" : L" was") + L" added. Type your question in Gemini.";
    const std::wstring typed = questionTyped ? L"Your question is in Gemini. " : L"";
    if (added > 0) {
        return typed + L"Only " + std::to_wstring(added) + L" of " + std::to_wstring(total) +
               L" images could be confirmed. Check Gemini, and add the rest with Ctrl+V in its message box.";
    }
    if (!clipboardOurs) {
        return typed + (several ? L"The clipboard changed before the images could be pasted. Ask in Gemini again to retry."
                                : L"The clipboard changed before the image could be pasted: use Copy image, then press Ctrl+V in Gemini.");
    }
    if (questionTyped) {
        return typed + L"If the " + images + (several ? L" aren't" : L" isn't") +
               L" attached, click Gemini's message box and press Ctrl+V.";
    }
    return L"Click Gemini's message box and press Ctrl+V to attach the " + images + L". Your question is kept here.";
}

enum class DraftOutcome { Added, Partial };

DraftOutcome FillBrowserDraft(IUIAutomation* automation, IUIAutomationElement* composer,
                              HWND browser, BrowserHandoffJob& job, std::wstring& status) {
    RECT editor{};
    composer->get_CurrentBoundingRectangle(&editor);
    const std::wstring question = BrowserDraftText(job.prompt);
    bool questionAdded = question.empty();
    bool keysSent = false;
    if (!questionAdded) {
        auto keys = BrowserDraftInputs(question);
        if (!BrowserComposerStillCurrent(automation, composer, browser, job, false, true)) {
            status = BrowserDraftStatus(job.images.size(), 0, false,
                                       GetClipboardSequenceNumber() == job.clipboardSequence);
            return DraftOutcome::Partial;
        }
        if (SendInput(static_cast<UINT>(keys.size()), keys.data(), sizeof(INPUT)) == keys.size()) {
            keysSent = true;
            const double typedAt = NowMs();
            while (!questionAdded && NowMs() - typedAt < 3000 && BrowserStillTarget(browser, job)) {
                Sleep(50);
                std::wstring shown;
                const bool readable = ReadElementText(composer, shown);
                if (!BrowserComposerStillCurrent(automation, composer, browser, job, false, true)) break;
                questionAdded = readable ? ContainsIgnoringSpacing(shown, question)
                                         : NowMs() - typedAt > 300;
            }
        }
    }

    const size_t total = job.images.size();
    const bool oneByOne = job.imagePaths.size() > 1 &&
                          !PastesSeveralFiles(FileNameOf(ProcessPathOfWindow(browser)));
    const size_t pastes = oneByOne ? job.imagePaths.size() : 1;
    size_t confirmed = 0;
    bool clipboardOurs = true;
    for (size_t paste = 0; paste < pastes && BrowserStillTarget(browser, job); paste++) {
        if (!ClipboardStillOurs(job)) {
            clipboardOurs = false;
            break;
        }
        bool focused = false;
        for (int attempt = 0; attempt < 20 && !focused && BrowserStillTarget(browser, job); attempt++) {
            if (!BrowserKeysAllowed(browser, job)) {
                Sleep(100);
                continue;
            }
            focused = BrowserComposerStillCurrent(automation, composer, browser, job, true, true);
            break;
        }
        if (!focused) break;
        if (oneByOne) {
            const std::vector<std::wstring> next{job.imagePaths[paste]};
            if (!CopyImageFileToClipboard(job.notify, next, job.clipboardSequence)) {
                clipboardOurs = false;
                break;
            }
            job.clipboardSequence = GetClipboardSequenceNumber();
            job.onClipboard = next;
        }
        auto before = ReadBrowserAttachments(automation, browser, editor);
        for (int retry = 0; retry < 3 && !before.readable &&
             BrowserPasteStillValid(browser, job.clipboardSequence, job); retry++) {
            Sleep(100);
            before = ReadBrowserAttachments(automation, browser, editor);
        }
        auto keys = BrowserImagePasteInputs();
        if (!BrowserComposerStillCurrent(automation, composer, browser, job, true, true)) {
            clipboardOurs = GetClipboardSequenceNumber() == job.clipboardSequence;
            break;
        }
        if (SendInput(static_cast<UINT>(keys.size()), keys.data(), sizeof(INPUT)) != keys.size()) {
            INPUT release[2]{};
            for (INPUT& input : release) { input.type = INPUT_KEYBOARD; input.ki.dwFlags = KEYEVENTF_KEYUP; }
            release[0].ki.wVk = 'V';
            release[1].ki.wVk = VK_CONTROL;
            SendInput(2, release, sizeof(INPUT));
            break;
        }
        const double deadline = NowMs() + (before.readable ? 12000 : 1500);
        bool added = false;
        while (!added && NowMs() < deadline && BrowserStillTarget(browser, job)) {
            Sleep(150);
            const auto after = ReadBrowserAttachments(automation, browser, editor);
            if (!BrowserComposerStillCurrent(automation, composer, browser, job, false, true)) break;
            added = HasNewBrowserAttachment(before, after);
        }
        if (!added) break;
        confirmed++;
    }
    const size_t imagesAdded = confirmed < pastes ? (oneByOne ? confirmed : 0)
                               : oneByOne ? job.imagePaths.size()
                               : job.imagePaths.empty() ? std::min<size_t>(1, total) : total;
    if (keysSent && !questionAdded &&
        BrowserComposerStillCurrent(automation, composer, browser, job, false, true)) {
        std::wstring shown;
        questionAdded = ReadElementText(composer, shown) &&
                        ContainsIgnoringSpacing(shown, question) &&
                        BrowserComposerStillCurrent(automation, composer, browser, job, false, true);
    }
    if (questionAdded && imagesAdded >= total) return DraftOutcome::Added;
    status = BrowserDraftStatus(total, imagesAdded, questionAdded && !question.empty(), clipboardOurs);
    return DraftOutcome::Partial;
}

ComPtr<IUIAutomation> CreateAutomation() {
    ComPtr<IUIAutomation> automation;
    if (FAILED(CoCreateInstance(kClsidCUIAutomation8, nullptr, CLSCTX_INPROC_SERVER,
                                kIidIUIAutomation, automation.PutVoid()))) {
        CoCreateInstance(kClsidCUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                         kIidIUIAutomation, automation.PutVoid());
    }
    if (automation) {
        ComPtr<IUIAutomation2> automation2;
        if (SUCCEEDED(automation->QueryInterface(kIidIUIAutomation2, automation2.PutVoid()))) {
            automation2->put_ConnectionTimeout(700);
            automation2->put_TransactionTimeout(700);
        }
    }
    return automation;
}

DWORD WINAPI BrowserHandoffThreadProc(LPVOID param) {
    std::unique_ptr<std::shared_ptr<BrowserHandoffJob>> holder(
        static_cast<std::shared_ptr<BrowserHandoffJob>*>(param));
    const std::shared_ptr<BrowserHandoffJob> job = *holder;
    auto result = std::make_unique<BrowserHandoffMessage>();
    result->id = job->id;
    const HRESULT coInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (!job->images.empty() && SUCCEEDED(coInit) && !job->canceled && !g_stopping) {
        for (const auto& image : job->images) {
            const std::wstring path = WriteGeminiImageFile(*image, job->notify);
            if (path.empty()) {  // All of them or none: a part of the pictures would mislead.
                job->imagePaths.clear();
                break;
            }
            job->imagePaths.push_back(path);
        }
        if (!job->canceled && !g_stopping && GetClipboardSequenceNumber() == job->clipboardSequence && !job->imagePaths.empty() &&
            CopyImageFileToClipboard(job->notify, job->imagePaths, job->clipboardSequence)) {
            job->clipboardSequence = GetClipboardSequenceNumber();
            job->onClipboard = job->imagePaths;
        }
    }
    ComPtr<IUIAutomation> automation;
    if (SUCCEEDED(coInit)) automation = CreateAutomation();
    const std::vector<std::wstring> alreadyOpen =
        automation && !job->canceled ? OpenGeminiComposerKeys(automation.Get(), job->browser)
                                     : std::vector<std::wstring>();
    const std::wstring url = L"https://gemini.google.com/app";
    const bool launched = !job->canceled && !g_stopping && OpenWithBrowser(url, job->browser);
    HWND verifiedBrowser = nullptr;
    SteadyCandidate steady;
    double blankSince = -1;  // When the window began to show no page.
    double lastRestart = 0;  // When the UI Automation client last started over.
    bool everSawDocument = false;
    const double launchedAt = NowMs();
    double lastPush = launchedAt;
    int pushes = 0;
    const double deadline = launchedAt + 20000;
    auto pushForward = [&] {
        const double now = NowMs();
        if (verifiedBrowser || pushes >= 6 || now - launchedAt < 1200 || now - lastPush < 700) return;
        lastPush = now;
        pushes++;
        if (HWND window = FindNewGeminiWindow(automation.Get(), job->browser, alreadyOpen)) BringToFront(window);
    };
    while (launched && automation && !job->canceled && !g_stopping && NowMs() < deadline) {
        const HWND foreground = GetForegroundWindow();
        const bool inBrowser = IsWindowVisible(foreground) && IsTargetBrowserWindow(foreground, job->browser);
        if (verifiedBrowser && !inBrowser) break;
        if (!inBrowser) {
            steady.Update(L"", NowMs());
            pushForward();
            Sleep(100);
            continue;
        }
        bool verifiedPage = false;
        bool sawDocument = false;
        auto composer = FindGeminiComposer(automation.Get(), foreground, verifiedPage, &sawDocument);
        if (!verifiedPage) pushForward();  // This window isn't the one with Gemini.
        if (sawDocument) {
            blankSince = -1;
            everSawDocument = true;
        } else {
            const double now = NowMs();
            if (blankSince < 0) {
                blankSince = now;
                lastRestart = now;
            }
            if (now - lastRestart > 1200) {
                if (ComPtr<IUIAutomation> fresh = CreateAutomation()) automation = std::move(fresh);
                lastRestart = now;
            }
        }
        if (verifiedPage) verifiedBrowser = foreground;
        std::wstring key = composer ? ElementRuntimeKey(composer.Get()) : std::wstring();
        if (std::find(alreadyOpen.begin(), alreadyOpen.end(), key) != alreadyOpen.end()) key.clear();
        if (steady.Update(key, NowMs()) &&
            FocusBrowserComposer(automation.Get(), composer.Get(), foreground, *job, false)) {
            const DraftOutcome outcome = FillBrowserDraft(automation.Get(), composer.Get(),
                                                         foreground, *job, result->status);
            result->pasted = outcome == DraftOutcome::Added;
            break;
        }
        Sleep(100);
    }
    if (!launched) {
        result->status = job->images.size() > 1 ? L"Couldn't open the browser. The images are copied; paste them into Gemini with Ctrl+V."
                                                : L"Couldn't open the browser. The image is copied; paste it into Gemini with Ctrl+V.";
    }
    if (!result->pasted && result->status.empty()) {
        result->status = everSawDocument
            ? BrowserDraftStatus(job->images.size(), 0, false, true)
            : L"Magic Pointer couldn't read the page in your browser. If a notice is open over it, close it and click Ask in Gemini again.";
    }
    automation.Reset();
    if (SUCCEEDED(coInit)) CoUninitialize();
    if (!job->canceled && !g_stopping) PostOwned(job->notify, WM_APP_BROWSER, std::move(result));
    g_helperThreads--;
    return 0;
}

bool StartBrowserHandoff(const std::shared_ptr<BrowserHandoffJob>& job) {
    auto* holder = new std::shared_ptr<BrowserHandoffJob>(job);
    g_helperThreads++;
    HANDLE thread = CreateThread(nullptr, 0, BrowserHandoffThreadProc, holder, 0, nullptr);
    if (!thread) {
        delete holder;
        g_helperThreads--;
        return false;
    }
    CloseHandle(thread);
    return true;
}

std::wstring UiLanguage() {
    WCHAR locale[LOCALE_NAME_MAX_LENGTH] = L"en";
    GetUserDefaultLocaleName(locale, LOCALE_NAME_MAX_LENGTH);
    std::wstring language = locale;
    const size_t dash = language.find(L'-');
    if (dash != std::wstring::npos) {
        language.resize(dash);
    }
    return language.empty() ? L"en" : language;
}

constexpr double kLensPageLifetimeMs = 10.0 * 60 * 1000;
extern HWND g_messageWindow;
struct TemporaryLensPage { std::wstring path; double expires; };
std::mutex g_lensPagesMutex;
std::vector<TemporaryLensPage> g_lensPages;

void TrackLensPage(const std::wstring& path, double expires) {
    std::lock_guard<std::mutex> lock(g_lensPagesMutex);
    const auto found = std::find_if(g_lensPages.begin(), g_lensPages.end(),
        [&](const TemporaryLensPage& page) { return page.path == path; });
    if (found == g_lensPages.end()) {
        g_lensPages.push_back({path, expires});
    } else {
        found->expires = expires;
    }
}

double CleanupLensPages(double now) {
    double next = 1e9;
    std::lock_guard<std::mutex> lock(g_lensPagesMutex);
    for (auto it = g_lensPages.begin(); it != g_lensPages.end();) {
        if (now >= it->expires) {
            if (DeleteFileW(it->path.c_str()) || GetLastError() == ERROR_FILE_NOT_FOUND ||
                GetLastError() == ERROR_PATH_NOT_FOUND) {
                it = g_lensPages.erase(it);
                continue;
            }
            it->expires = now + 60000;  // A browser may still have the page open.
        }
        next = std::min(next, it->expires - now);
        ++it;
    }
    return next;
}

void DeleteOldLensPages(bool all) {
    if (g_storageDir.empty()) {
        return;
    }
    WIN32_FIND_DATAW data{};
    HANDLE find = FindFirstFileW((g_storageDir + L"\\lens-*.html").c_str(), &data);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }
    FILETIME current{};
    GetSystemTimeAsFileTime(&current);
    const ULONGLONG nowTicks = (ULONGLONG(current.dwHighDateTime) << 32) | current.dwLowDateTime;
    const double now = NowMs();
    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const std::wstring path = g_storageDir + L"\\" + data.cFileName;
        const ULONGLONG written = (ULONGLONG(data.ftLastWriteTime.dwHighDateTime) << 32) |
                                  data.ftLastWriteTime.dwLowDateTime;
        const double age = nowTicks >= written ? (nowTicks - written) / 10000.0 : 0.0;
        if (all || age >= kLensPageLifetimeMs) {
            if (!DeleteFileW(path.c_str()) && GetLastError() != ERROR_FILE_NOT_FOUND &&
                GetLastError() != ERROR_PATH_NOT_FOUND) {
                TrackLensPage(path, now + 60000);
            }
        } else {
            TrackLensPage(path, now + kLensPageLifetimeMs - age);
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
}

struct LensJob {
    std::shared_ptr<const Image> image;
    std::wstring language;
    BrowserTarget browser;
    HWND notify = nullptr;
};

std::string BuildLensPage(const std::vector<uint8_t>& png,
                          const std::wstring& language) {
    std::string page =
        "<!DOCTYPE html>\n<html><head><meta charset=\"utf-8\">"
        "<meta name=\"referrer\" content=\"no-referrer\">"
        "<title>Google Lens</title><style>"
        "html,body{height:100%;margin:0;background:#1f1f1f;color:#e3e3e3;"
        "font:15px 'Segoe UI',system-ui,sans-serif}"
        "body{display:flex;flex-direction:column;align-items:center;"
        "justify-content:center;gap:14px}"
        "img{max-width:360px;max-height:220px;border-radius:14px}"
        "button{font:inherit;padding:9px 20px;border:0;border-radius:20px;"
        "background:#a8c7fa;color:#062e6f;cursor:pointer}"
        "</style></head><body><img id=\"p\" alt=\"\">"
        "<div id=\"s\">Searching with Google Lens&hellip;</div>"
        "<button id=\"b\" hidden>Search with Google Lens</button>"
        "<form id=\"f\" method=\"POST\" enctype=\"multipart/form-data\" "
        "action=\"https://lens.google.com/v3/upload?hl=";
    page += ToUtf8(language);
    page += "&amp;st=";
    page += std::to_string(GetTickCount64());
    page +=
        "\"><input type=\"file\" name=\"encoded_image\" id=\"i\" hidden>"
        "</form><script>\nconst data=\"";
    page += Base64Encode(png.data(), png.size());
    page +=
        "\";\n"
        "document.getElementById('p').src='data:image/png;base64,'+data;\n"
        "const button=document.getElementById('b');\n"
        "function go(){try{\n"
        "const bytes=Uint8Array.from(atob(data),c=>c.charCodeAt(0));\n"
        "const transfer=new DataTransfer();\n"
        "transfer.items.add(new File([bytes],'selection.png',{type:'image/png'}));\n"
        "document.getElementById('i').files=transfer.files;\n"
        "try{sessionStorage.setItem('sent','1');}catch(e){}\n"
        "document.getElementById('f').submit();\n"
        "}catch(e){document.getElementById('s').textContent="
        "\"Couldn't upload automatically. The image is on your clipboard: "
        "paste it at lens.google.com.\";button.hidden=false;}}\n"
        "button.onclick=go;\n"
        "let sent=false;try{sent=sessionStorage.getItem('sent')==='1';}catch(e){}\n"
        "if(sent){document.getElementById('s').textContent='';button.hidden=false;}"
        "else{setTimeout(()=>{button.hidden=false;},5000);go();}\n"
        "</script></body></html>\n";
    return page;
}

DWORD WINAPI LensThreadProc(LPVOID param) {
    std::unique_ptr<LensJob> job(static_cast<LensJob*>(param));
    const HRESULT coInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    std::vector<uint8_t> png;
    const bool encoded = EncodePng(DownscaleImage(*job->image, 1000), png);
    if (SUCCEEDED(coInit)) {
        CoUninitialize();
    }
    if (encoded && !g_storageDir.empty() && !g_stopping) {
        const std::string page = BuildLensPage(png, job->language);
        static std::atomic<unsigned> sequence{0};
        const std::wstring path = g_storageDir + L"\\lens-" +
            std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(GetTickCount64()) +
            L"-" + std::to_wstring(++sequence) + L".html";
        HANDLE file = page.size() <= MAXDWORD ?
            CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                        CREATE_NEW, FILE_ATTRIBUTE_TEMPORARY, nullptr) : INVALID_HANDLE_VALUE;
        if (file != INVALID_HANDLE_VALUE) {
            DWORD written = 0;
            const BOOL ok = WriteFile(file, page.data(),
                                      static_cast<DWORD>(page.size()), &written,
                                      nullptr);
            CloseHandle(file);
            if (ok && written == page.size()) {
                TrackLensPage(path, NowMs() + kLensPageLifetimeMs);
                if (!g_stopping && job->notify) PostMessageW(job->notify, WM_APP_TEMP_FILE, 0, 0);
                if (!g_stopping) OpenInBrowser(path, job->browser);
            } else if (!DeleteFileW(path.c_str())) {
                TrackLensPage(path, NowMs() + 60000);
                if (!g_stopping && job->notify) PostMessageW(job->notify, WM_APP_TEMP_FILE, 0, 0);
            }
        }
    }
    g_helperThreads--;
    return 0;
}

void SearchWithLens(std::shared_ptr<const Image> image, const BrowserTarget& browser) {
    auto* job = new LensJob{std::move(image), UiLanguage(), browser, g_messageWindow};
    g_helperThreads++;
    HANDLE thread = CreateThread(nullptr, 0, LensThreadProc, job, 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        g_helperThreads--;
        delete job;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Text recognition with the OCR engine built into Windows 10 and later

namespace abi {

struct IAsyncInfo : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Id(UINT32* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Status(int* status) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ErrorCode(HRESULT* error) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel() = 0;
    virtual HRESULT STDMETHODCALLTYPE Close() = 0;
};

struct IOcrLine : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Words(IInspectable** words) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Text(HSTRING* text) = 0;
};

struct IOcrLineList : IInspectable {  // IVectorView<OcrLine>
    virtual HRESULT STDMETHODCALLTYPE GetAt(UINT32 index, IOcrLine** line) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Size(UINT32* size) = 0;
};

struct IOcrResult : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Lines(IOcrLineList** lines) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TextAngle(IInspectable** angle) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Text(HSTRING* text) = 0;
};

struct IOcrOperation : IInspectable {  // IAsyncOperation<OcrResult>
    virtual HRESULT STDMETHODCALLTYPE put_Completed(IUnknown* handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Completed(IUnknown** handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResults(IOcrResult** result) = 0;
};

struct IOcrEngine : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE RecognizeAsync(IInspectable* bitmap,
                                                     IOcrOperation** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RecognizerLanguage(
        IInspectable** language) = 0;
};

struct IOcrEngineStatics : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_MaxImageDimension(UINT32* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AvailableRecognizerLanguages(
        IInspectable** languages) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsLanguageSupported(IInspectable* language,
                                                          boolean* supported) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryCreateFromLanguage(IInspectable* language,
                                                            IOcrEngine** engine) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryCreateFromUserProfileLanguages(
        IOcrEngine** engine) = 0;
};

struct ISoftwareBitmapStatics : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Copy(IInspectable* source,
                                           IInspectable** copy) = 0;
    virtual HRESULT STDMETHODCALLTYPE Convert(IInspectable* source,
                                              int format,
                                              IInspectable** converted) = 0;
    virtual HRESULT STDMETHODCALLTYPE ConvertWithAlpha(IInspectable* source,
                                                       int format,
                                                       int alpha,
                                                       IInspectable** converted) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCopyFromBuffer(IInspectable* buffer,
                                                           int format,
                                                           INT32 width,
                                                           INT32 height,
                                                           IInspectable** bitmap) = 0;
};

struct ICryptographicBufferStatics : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Compare(IInspectable* a,
                                              IInspectable* b,
                                              boolean* equal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GenerateRandom(UINT32 length,
                                                     IInspectable** buffer) = 0;
    virtual HRESULT STDMETHODCALLTYPE GenerateRandomNumber(UINT32* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFromByteArray(UINT32 size,
                                                          BYTE* bytes,
                                                          IInspectable** buffer) = 0;
};

constexpr int kBitmapPixelFormatBgra8 = 87;
constexpr int kAsyncStarted = 0;
constexpr int kAsyncCompleted = 1;

}  // namespace abi

class WinRt {
  public:
    static const WinRt& Get() {
        static const WinRt instance;
        return instance;
    }

    bool Available() const {
        return getActivationFactory_ && createString_ && deleteString_ &&
               rawBuffer_;
    }

    template <typename T>
    bool Factory(PCWSTR className, const GUID& iid, ComPtr<T>& out) const {
        HSTRING name = nullptr;
        if (!Available() ||
            FAILED(createString_(className,
                                 static_cast<UINT32>(wcslen(className)), &name))) {
            return false;
        }
        const HRESULT hr = getActivationFactory_(name, iid, out.PutVoid());
        deleteString_(name);
        return SUCCEEDED(hr) && out;
    }

    std::wstring Take(HSTRING text) const {
        if (!text) {
            return {};
        }
        UINT32 length = 0;
        PCWSTR raw = rawBuffer_(text, &length);
        std::wstring result(raw ? raw : L"", raw ? length : 0);
        deleteString_(text);
        return result;
    }

  private:
    using RoGetActivationFactory_t = HRESULT(WINAPI*)(HSTRING, REFIID, void**);
    using WindowsCreateString_t = HRESULT(WINAPI*)(PCWSTR, UINT32, HSTRING*);
    using WindowsDeleteString_t = HRESULT(WINAPI*)(HSTRING);
    using WindowsGetStringRawBuffer_t = PCWSTR(WINAPI*)(HSTRING, UINT32*);

    WinRt() {
        if (!IsWindows10Build(10240)) {
            return;
        }
        getActivationFactory_ = GetProc<RoGetActivationFactory_t>(
            L"combase.dll", "RoGetActivationFactory");
        createString_ = GetProc<WindowsCreateString_t>(L"combase.dll",
                                                       "WindowsCreateString");
        deleteString_ = GetProc<WindowsDeleteString_t>(L"combase.dll",
                                                       "WindowsDeleteString");
        rawBuffer_ = GetProc<WindowsGetStringRawBuffer_t>(
            L"combase.dll", "WindowsGetStringRawBuffer");
    }

    RoGetActivationFactory_t getActivationFactory_ = nullptr;
    WindowsCreateString_t createString_ = nullptr;
    WindowsDeleteString_t deleteString_ = nullptr;
    WindowsGetStringRawBuffer_t rawBuffer_ = nullptr;
};

Image UpscaleImage2x(const Image& src) {
    Image dst;
    dst.width = src.width * 2;
    dst.height = src.height * 2;
    dst.pixels.resize(static_cast<size_t>(dst.width) * dst.height);
    auto channel = [](uint32_t p, int shift) {
        return static_cast<float>((p >> shift) & 0xFF);
    };
    for (int y = 0; y < dst.height; y++) {
        const float sy = std::max(0.0f, (y + 0.5f) * 0.5f - 0.5f);
        const int y0 = std::min(static_cast<int>(sy), src.height - 1);
        const int y1 = std::min(y0 + 1, src.height - 1);
        const float fy = sy - y0;
        const uint32_t* row0 = &src.pixels[static_cast<size_t>(y0) * src.width];
        const uint32_t* row1 = &src.pixels[static_cast<size_t>(y1) * src.width];
        for (int x = 0; x < dst.width; x++) {
            const float sx = std::max(0.0f, (x + 0.5f) * 0.5f - 0.5f);
            const int x0 = std::min(static_cast<int>(sx), src.width - 1);
            const int x1 = std::min(x0 + 1, src.width - 1);
            const float fx = sx - x0;
            uint32_t out = 0xFF000000;
            for (int shift = 0; shift < 24; shift += 8) {
                const float top = channel(row0[x0], shift) +
                                  (channel(row0[x1], shift) - channel(row0[x0], shift)) * fx;
                const float bottom = channel(row1[x0], shift) +
                                     (channel(row1[x1], shift) - channel(row1[x0], shift)) * fx;
                out |= static_cast<uint32_t>(top + (bottom - top) * fy + 0.5f) << shift;
            }
            dst.pixels[static_cast<size_t>(y) * dst.width + x] = out;
        }
    }
    return dst;
}

std::atomic<bool> g_stopping{false};

std::wstring RecognizeText(const Image& source) {
    const WinRt& winrt = WinRt::Get();
    ComPtr<abi::IOcrEngineStatics> ocrStatics;
    ComPtr<abi::ISoftwareBitmapStatics> bitmapStatics;
    ComPtr<abi::ICryptographicBufferStatics> bufferStatics;
    if (source.Empty() ||
        !winrt.Factory(L"Windows.Media.Ocr.OcrEngine", kIidIOcrEngineStatics,
                       ocrStatics) ||
        !winrt.Factory(L"Windows.Graphics.Imaging.SoftwareBitmap",
                       kIidISoftwareBitmapStatics, bitmapStatics) ||
        !winrt.Factory(L"Windows.Security.Cryptography.CryptographicBuffer",
                       kIidICryptographicBufferStatics, bufferStatics)) {
        return {};
    }
    ComPtr<abi::IOcrEngine> engine;
    if (FAILED(ocrStatics->TryCreateFromUserProfileLanguages(engine.Put())) ||
        !engine) {
        Wh_Log(L"No OCR language is installed");
        return {};
    }

    UINT32 maxDimension = 0;
    if (FAILED(ocrStatics->get_MaxImageDimension(&maxDimension)) ||
        !maxDimension) {
        maxDimension = 2600;
    }
    const int maxSide = static_cast<int>(maxDimension);
    const Image* image = &source;
    Image scaled;
    const int longest = std::max(source.width, source.height);
    if (longest * 2 <= maxSide &&
        static_cast<int64_t>(source.width) * source.height <= 1500 * 1000) {
        scaled = UpscaleImage2x(source);
        image = &scaled;
    } else if (longest > maxSide) {
        scaled = DownscaleImage(source, maxSide);
        image = &scaled;
    }
    if (image->width < 40 || image->height < 40) {
        Image padded;
        padded.width = std::max(image->width, 40);
        padded.height = std::max(image->height, 40);
        padded.pixels.assign(static_cast<size_t>(padded.width) * padded.height,
                             image->pixels[0]);
        for (int y = 0; y < image->height; y++) {
            std::copy_n(&image->pixels[static_cast<size_t>(y) * image->width],
                        image->width,
                        &padded.pixels[static_cast<size_t>(y) * padded.width]);
        }
        scaled = std::move(padded);
        image = &scaled;
    }

    ComPtr<IInspectable> buffer;
    ComPtr<IInspectable> bitmap;
    ComPtr<abi::IOcrOperation> operation;
    ComPtr<abi::IAsyncInfo> info;
    if (FAILED(bufferStatics->CreateFromByteArray(
            static_cast<UINT32>(image->pixels.size() * 4),
            reinterpret_cast<BYTE*>(const_cast<uint32_t*>(image->pixels.data())),
            buffer.Put())) ||
        FAILED(bitmapStatics->CreateCopyFromBuffer(
            buffer.Get(), abi::kBitmapPixelFormatBgra8, image->width,
            image->height, bitmap.Put())) ||
        FAILED(engine->RecognizeAsync(bitmap.Get(), operation.Put())) ||
        FAILED(operation->QueryInterface(kIidIAsyncInfo, info.PutVoid()))) {
        return {};
    }
    int status = abi::kAsyncStarted;
    const double deadline = NowMs() + 15000;
    while (SUCCEEDED(info->get_Status(&status)) &&
           status == abi::kAsyncStarted) {
        if (g_stopping || NowMs() > deadline) {
            info->Cancel();
            return {};
        }
        Sleep(10);
    }
    ComPtr<abi::IOcrResult> result;
    ComPtr<abi::IOcrLineList> lines;
    UINT32 count = 0;
    if (status != abi::kAsyncCompleted ||
        FAILED(operation->GetResults(result.Put())) || !result ||
        FAILED(result->get_Lines(lines.Put())) || !lines ||
        FAILED(lines->get_Size(&count))) {
        return {};
    }
    std::wstring text;
    for (UINT32 i = 0; i < count; i++) {
        ComPtr<abi::IOcrLine> line;
        HSTRING lineText = nullptr;
        if (SUCCEEDED(lines->GetAt(i, line.Put())) && line &&
            SUCCEEDED(line->get_Text(&lineText))) {
            const std::wstring value = winrt.Take(lineText);
            if (!value.empty()) {
                if (!text.empty()) {
                    text += L'\n';
                }
                text += value;
            }
        }
    }
    return text;
}

bool OcrAvailable() {
    return WinRt::Get().Available();
}

struct OcrJob {
    HWND notify;
    uint32_t attachmentId;
    std::shared_ptr<const Image> image;
};

struct OcrResultMessage {
    uint32_t attachmentId;
    std::wstring text;
};

DWORD WINAPI OcrThreadProc(LPVOID param) {
    std::unique_ptr<OcrJob> job(static_cast<OcrJob*>(param));
    const HRESULT coInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const double start = NowMs();
    auto message = std::make_unique<OcrResultMessage>();
    message->attachmentId = job->attachmentId;
    message->text = RecognizeText(*job->image);
    Wh_Log(L"OCR: %d characters in %d ms",
           static_cast<int>(message->text.size()),
           static_cast<int>(NowMs() - start));
    if (SUCCEEDED(coInit)) {
        CoUninitialize();
    }
    if (!g_stopping) {
        PostOwned(job->notify, WM_APP_OCR, std::move(message));
    }
    g_helperThreads--;
    return 0;
}

bool StartOcr(HWND notify,
              uint32_t attachmentId,
              std::shared_ptr<const Image> image) {
    if (!OcrAvailable() || !image || image->Empty()) {
        return false;
    }
    auto* job = new OcrJob{notify, attachmentId, std::move(image)};
    g_helperThreads++;
    HANDLE thread = CreateThread(nullptr, 0, OcrThreadProc, job, 0, nullptr);
    if (thread) {
        CloseHandle(thread);
        return true;
    }
    g_helperThreads--;
    delete job;
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Gemini API client

#ifndef WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
#define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY 4
#endif
#ifndef WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3
#define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3 0x00002000
#endif
#ifndef WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL
#define WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL 133
#endif
#ifndef WINHTTP_PROTOCOL_FLAG_HTTP2
#define WINHTTP_PROTOCOL_FLAG_HTTP2 0x1
#endif

enum class GeminiEvent { Chunk, Done, Error, Suggestions };

struct GeminiMessage {
    uint32_t requestId = 0;
    GeminiEvent event = GeminiEvent::Done;
    bool partial = false;  // More suggestions are on the way.
    std::wstring text;
    std::vector<std::wstring> suggestions;
};

struct ChatTurn {
    bool fromUser;
    std::string text;  // UTF-8.
};

struct GeminiJob {
    uint32_t id = 0;
    bool suggestions = false;
    std::wstring apiKey;
    std::wstring model;
    std::wstring fallbackModel;  // When `model` doesn't exist.
    std::vector<std::shared_ptr<const Image>> images;
    std::string context;  // Text of the selection, UTF-8.
    std::vector<ChatTurn> history;  // Ends with the new question.
    HWND notify = nullptr;
    std::atomic<bool> canceled{false};
    std::atomic<HINTERNET> request{nullptr};

    void Cancel() {
        canceled = true;
        if (HINTERNET handle = request.exchange(nullptr)) {
            WinHttpCloseHandle(handle);  // Aborts a blocking read.
        }
    }
};

constexpr WCHAR kDefaultModel[] = L"gemini-flash-lite-latest";

std::wstring SanitizeModel(std::wstring model) {
    if (model.rfind(L"models/", 0) == 0) {
        model.erase(0, 7);
    }
    std::wstring result;
    for (wchar_t c : model) {
        if (iswalnum(c) || c == L'-' || c == L'.' || c == L'_') {
            result += c;
        }
    }
    return result;
}

enum class Thinking { Minimal, Low, LimitedBudget, Default };

struct ModelQuirks {
    std::wstring model;
    bool suggestions;
    Thinking thinking;
};

std::mutex g_quirksMutex;
std::vector<ModelQuirks> g_quirks;
std::vector<std::wstring> g_missingModels;

Thinking FirstThinking(const std::wstring& model, bool suggestions) {
    // Gemini 3.8 Flash and Pro reject "minimal". Latest aliases can also
    // move to a model with different thinking support, so prefer "low".
    const bool supportsMinimal =
        model == L"gemini-flash-lite-latest" ||
        model == L"gemini-3.5-flash-lite" ||
        model == L"gemini-3.1-flash-lite" ||
        model == L"gemini-3.6-flash" ||
        model == L"gemini-3-flash-preview";
    return suggestions && supportsMinimal ? Thinking::Minimal : Thinking::Low;
}

Thinking GetThinking(const std::wstring& model, bool suggestions) {
    std::lock_guard<std::mutex> lock(g_quirksMutex);
    for (const ModelQuirks& quirks : g_quirks) {
        if (quirks.model == model && quirks.suggestions == suggestions) {
            return quirks.thinking;
        }
    }
    return FirstThinking(model, suggestions);
}

Thinking NextThinking(Thinking thinking) {
    switch (thinking) {
        case Thinking::Minimal:
            return Thinking::Low;
        case Thinking::Low:
            return Thinking::LimitedBudget;
        case Thinking::LimitedBudget:
        case Thinking::Default:
            break;
    }
    return Thinking::Default;
}

void SetThinking(const std::wstring& model, bool suggestions, Thinking thinking) {
    std::lock_guard<std::mutex> lock(g_quirksMutex);
    for (ModelQuirks& quirks : g_quirks) {
        if (quirks.model == model && quirks.suggestions == suggestions) {
            quirks.thinking = thinking;
            return;
        }
    }
    g_quirks.push_back({model, suggestions, thinking});
}

bool IsModelMissing(const std::wstring& model) {
    std::lock_guard<std::mutex> lock(g_quirksMutex);
    return std::find(g_missingModels.begin(), g_missingModels.end(), model) !=
           g_missingModels.end();
}

void SetModelMissing(const std::wstring& model) {
    std::lock_guard<std::mutex> lock(g_quirksMutex);
    if (std::find(g_missingModels.begin(), g_missingModels.end(), model) ==
        g_missingModels.end()) {
        g_missingModels.push_back(model);
    }
}

std::string SystemInstruction(const GeminiJob& job) {
    std::string text;
    if (job.suggestions) {
        text =
            "You suggest what someone might want to ask or do with a part of "
            "their screen they just selected.";
    } else {
        text =
            "You are Magic Pointer, the Gemini assistant built into the "
            "user's Windows PC. The user pointed at part of their screen and "
            "selected it; the attached image is exactly what they selected. "
            "Answer their request about it directly and concisely, in the "
            "language they write in. Prefer short paragraphs and bullet "
            "points, use bold sparingly and no tables. If the request is "
            "vague, briefly say what the selection shows and the most useful "
            "thing you can tell them about it. When asked to translate, "
            "translate all the text in the selection into the language they "
            "name and reply with only that translation, written in that "
            "language whatever language the request is in, keeping its line "
            "breaks.";
    }
    if (!job.context.empty()) {
        text += "\n\nText in the selection, from the app and from text "
                "recognition (it may contain recognition errors):\n\"\"\"\n";
        text += job.context;
        text += "\n\"\"\"";
    }
    return text;
}

constexpr char kSuggestionPrompt[] =
    "Suggest exactly three short, specific things I might want to ask or do "
    "with this selection next. Rules: each at most 45 characters; written "
    "from my point of view (use I or me); cover different intents such as "
    "understanding it, transforming it, getting ideas from it or acting on "
    "it; nothing generic like \"Tell me more\" or \"Explain this\"; no "
    "emojis; nothing that needs other software. Reply with only a JSON array "
    "of three strings.";

std::string BuildRequestBody(const GeminiJob& job,
                             const std::vector<std::string>& images,
                             Thinking thinking) {
    std::string body = "{\"systemInstruction\":{\"parts\":[{\"text\":";
    AppendJsonString(body, SystemInstruction(job));
    body += "}]},\"contents\":[";

    auto appendImages = [&] {
        for (const std::string& image : images) {
            body += "{\"inlineData\":{\"mimeType\":\"image/jpeg\",\"data\":\"";
            body += image;
            body += "\"}},";
        }
    };

    if (job.suggestions) {
        body += "{\"role\":\"user\",\"parts\":[";
        appendImages();
        body += "{\"text\":";
        AppendJsonString(body, kSuggestionPrompt);
        body += "}]}";
    } else {
        for (size_t i = 0; i < job.history.size(); i++) {
            const ChatTurn& turn = job.history[i];
            if (i > 0) {
                body += ',';
            }
            body += turn.fromUser ? "{\"role\":\"user\",\"parts\":["
                                  : "{\"role\":\"model\",\"parts\":[";
            if (i == 0) {
                appendImages();
            }
            body += "{\"text\":";
            AppendJsonString(body,
                             turn.text.empty() ? std::string(" ") : turn.text);
            body += "}]}";
        }
    }
    body += "],\"generationConfig\":{";
    switch (thinking) {
        case Thinking::Minimal:
            body += "\"thinkingConfig\":{\"thinkingLevel\":\"minimal\"},";
            break;
        case Thinking::Low:
            body += "\"thinkingConfig\":{\"thinkingLevel\":\"low\"},";
            break;
        case Thinking::LimitedBudget:
            // Positive budgets also work with models that cannot turn off
            // thinking; never retry a Gemini 3 request with a zero budget.
            body += "\"thinkingConfig\":{\"thinkingBudget\":1024},";
            break;
        case Thinking::Default:
            break;
    }
    if (job.suggestions) {
        body += "\"responseMimeType\":\"application/json\",";
        // This limit includes thought tokens as well as the three chips.
        body += "\"maxOutputTokens\":2048,";
    }
    if (body.back() == ',') {
        body.pop_back();
    }
    body += "}}";
    return body;
}

void ExtractResponseText(const Json& root,
                         std::string& text,
                         std::string& finishReason) {
    if (const Json* candidates = root.Get("candidates")) {
        if (const Json* candidate = candidates->At(0)) {
            if (const Json* content = candidate->Get("content")) {
                if (const Json* parts = content->Get("parts")) {
                    for (const Json& part : parts->items) {
                        const Json* thought = part.Get("thought");
                        if (thought && thought->boolean) {
                            continue;
                        }
                        if (const Json* partText = part.Get("text")) {
                            text += partText->string;
                        }
                    }
                }
            }
            if (const Json* reason = candidate->Get("finishReason")) {
                finishReason = reason->string;
            }
        }
    }
    if (const Json* feedback = root.Get("promptFeedback")) {
        if (const Json* reason = feedback->Get("blockReason")) {
            finishReason = reason->string;
        }
    }
}

std::string ApiErrorMessage(const std::string& body) {
    Json root;
    if (ParseJson(body, root)) {
        if (const Json* error = root.Get("error")) {
            if (const Json* message = error->Get("message")) {
                return message->string;
            }
        }
        if (const Json* first = root.At(0)) {  // Streamed errors are arrays.
            if (const Json* error = first->Get("error")) {
                if (const Json* message = error->Get("message")) {
                    return message->string;
                }
            }
        }
    }
    return {};
}

std::wstring FriendlyError(DWORD status,
                           const std::string& apiMessage,
                           const std::wstring& model) {
    if (status == 400 && ContainsNoCase(apiMessage, "API key")) {
        return L"Your Gemini API key isn't valid. Check it in the mod's "
               L"settings.";
    }
    if (status == 403) {
        return L"This API key can't use Gemini (403). Check the key in the "
               L"mod's settings.";
    }
    if (status == 404) {
        return L"The model \"" + model + L"\" isn't available to your key. "
               L"Pick another model in the mod's settings.";
    }
    if (status == 429) {
        if (ContainsNoCase(apiMessage, "limit: 0")) {
            return L"Your Gemini plan has no free quota for \"" + model +
                   L"\". Pick a Flash-Lite or Flash model in the mod's "
                   L"settings, or add billing to your key.";
        }
        return L"You've reached Gemini's rate limit for now. Try again in a "
               L"moment.";
    }
    if (status >= 500) {
        return L"Gemini is having trouble right now (" +
               std::to_wstring(status) + L"). Try again in a moment.";
    }
    if (!apiMessage.empty()) {
        return FromUtf8(apiMessage);
    }
    return L"Gemini returned an error (" + std::to_wstring(status) + L").";
}

class SseParser {
  public:
    template <typename OnEvent>
    void Feed(const char* data, size_t size, OnEvent onEvent) {
        buffer_.append(data, size);
        for (;;) {
            size_t separator = std::string::npos;
            size_t separatorLength = 0;
            const size_t lf = buffer_.find("\n\n");
            const size_t crlf = buffer_.find("\r\n\r\n");
            if (lf != std::string::npos &&
                (crlf == std::string::npos || lf < crlf)) {
                separator = lf;
                separatorLength = 2;
            } else if (crlf != std::string::npos) {
                separator = crlf;
                separatorLength = 4;
            }
            if (separator == std::string::npos) {
                break;
            }
            Emit(buffer_.substr(0, separator), onEvent);
            buffer_.erase(0, separator + separatorLength);
        }
    }

    template <typename OnEvent>
    void Finish(OnEvent onEvent) {
        if (!buffer_.empty()) {
            Emit(buffer_, onEvent);
            buffer_.clear();
        }
    }

  private:
    template <typename OnEvent>
    static void Emit(const std::string& event, OnEvent onEvent) {
        std::string data;
        size_t start = 0;
        while (start < event.size()) {
            size_t end = event.find('\n', start);
            if (end == std::string::npos) {
                end = event.size();
            }
            std::string line = event.substr(start, end - start);
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.rfind("data:", 0) == 0) {
                if (!data.empty()) {
                    data += '\n';
                }
                data += line.substr(line.size() > 5 && line[5] == ' ' ? 6 : 5);
            }
            start = end + 1;
        }
        if (!data.empty()) {
            onEvent(data);
        }
    }

    std::string buffer_;
};

std::mutex g_sessionMutex;
HINTERNET g_session;

HINTERNET GeminiSession() {
    std::lock_guard<std::mutex> lock(g_sessionMutex);
    if (g_session) {
        return g_session;
    }
    HINTERNET session = WinHttpOpen(
        L"MagicPointer/1.1 (Windhawk)",
        IsWindows81OrLater() ? WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
                             : WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) {
        session = WinHttpOpen(L"MagicPointer/1.1 (Windhawk)",
                              WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                              WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    }
    if (!session) {
        return nullptr;
    }
    DWORD protocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2 |
                      WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3;
    if (!WinHttpSetOption(session, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols,
                          sizeof(protocols))) {
        protocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
        WinHttpSetOption(session, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols,
                         sizeof(protocols));
    }
    DWORD http2 = WINHTTP_PROTOCOL_FLAG_HTTP2;
    WinHttpSetOption(session, WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL, &http2,
                     sizeof(http2));
    WinHttpSetTimeouts(session, 15000, 15000, 30000, 90000);
    g_session = session;
    return session;
}

void CloseGeminiSession() {
    std::lock_guard<std::mutex> lock(g_sessionMutex);
    if (g_session) {
        WinHttpCloseHandle(g_session);
        g_session = nullptr;
    }
}

struct HttpResult {
    DWORD status = 0;
    DWORD error = 0;
    std::string body;  // Only kept for error responses.
};

template <typename OnData>
HttpResult SendToGemini(PCWSTR verb,
                        const std::wstring& path,
                        const std::wstring& apiKey,
                        const std::string& body,
                        std::atomic<bool>* canceled,
                        std::atomic<HINTERNET>* requestSlot,
                        DWORD receiveTimeout,
                        OnData onData) {
    HttpResult result;
    HINTERNET session = GeminiSession();
    if (!session) {
        result.error = GetLastError();
        return result;
    }
    HINTERNET connection = WinHttpConnect(
        session, L"generativelanguage.googleapis.com",
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    HINTERNET request =
        connection
            ? WinHttpOpenRequest(connection, verb, path.c_str(), nullptr,
                                 WINHTTP_NO_REFERER,
                                 WINHTTP_DEFAULT_ACCEPT_TYPES,
                                 WINHTTP_FLAG_SECURE)
            : nullptr;
    if (!request) {
        result.error = GetLastError();
        if (connection) {
            WinHttpCloseHandle(connection);
        }
        return result;
    }
    WinHttpSetTimeouts(request, 15000, 15000, 30000,
                       static_cast<int>(receiveTimeout));

    if (requestSlot) {
        requestSlot->store(request);
        if (*canceled) {
            if (HINTERNET handle = requestSlot->exchange(nullptr)) {
                WinHttpCloseHandle(handle);
            }
        }
    }

    const std::wstring headers = L"Content-Type: application/json\r\n"
                                 L"x-goog-api-key: " +
                                 apiKey + L"\r\n";
    bool ok = !(canceled && *canceled) &&
              WinHttpSendRequest(request, headers.c_str(), static_cast<DWORD>(-1),
                                 const_cast<char*>(body.data()),
                                 static_cast<DWORD>(body.size()),
                                 static_cast<DWORD>(body.size()), 0) &&
              WinHttpReceiveResponse(request, nullptr);
    if (ok) {
        DWORD status = 0;
        DWORD size = sizeof(status);
        ok = WinHttpQueryHeaders(
                 request,
                 WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                 WINHTTP_HEADER_NAME_BY_INDEX, &status, &size,
                 WINHTTP_NO_HEADER_INDEX) != FALSE;
        result.status = status;
        char buffer[8192];
        while (ok) {
            DWORD available = 0;
            DWORD read = 0;
            if ((canceled && *canceled) ||
                !WinHttpQueryDataAvailable(request, &available) ||
                (available > 0 &&
                 !WinHttpReadData(request, buffer,
                                  std::min<DWORD>(available, sizeof(buffer)),
                                  &read))) {
                ok = false;
                break;
            }
            if (read == 0) {
                break;
            }
            if (status == 200) {
                onData(buffer, static_cast<size_t>(read));
            } else if (result.body.size() < 65536) {
                result.body.append(buffer, read);
            }
        }
    }
    if (!ok && !result.error) {
        result.error = GetLastError();
        if (!result.error) {
            result.error = ERROR_WINHTTP_CONNECTION_ERROR;
        }
    }

    if (requestSlot) {
        if (HINTERNET handle = requestSlot->exchange(nullptr)) {
            WinHttpCloseHandle(handle);
        }
    } else {
        WinHttpCloseHandle(request);
    }
    WinHttpCloseHandle(connection);
    return result;
}

void PostGeminiMessage(GeminiJob& job,
                       GeminiEvent event,
                       std::wstring text,
                       std::vector<std::wstring> suggestions = {},
                       bool partial = false) {
    if (job.canceled) {
        return;
    }
    auto message = std::make_unique<GeminiMessage>();
    message->requestId = job.id;
    message->event = event;
    message->partial = partial;
    message->text = std::move(text);
    message->suggestions = std::move(suggestions);
    PostOwned(job.notify, WM_APP_GEMINI, std::move(message));
}

std::wstring CleanSuggestion(std::string item) {
    while (!item.empty() && (item.back() == ' ' || item.back() == ',' ||
                             item.back() == '"' || item.back() == '.')) {
        item.pop_back();
    }
    size_t start = 0;
    while (start < item.size() &&
           (item[start] == ' ' || item[start] == '"' || item[start] == '-' ||
            item[start] == '*' || item[start] == '[')) {
        start++;
    }
    size_t digits = start;
    while (digits < item.size() &&
           isdigit(static_cast<unsigned char>(item[digits]))) {
        digits++;
    }
    if (digits > start && digits + 1 < item.size() &&
        (item[digits] == '.' || item[digits] == ')') && item[digits + 1] == ' ') {
        start = digits + 2;
    }
    item.erase(0, start);
    std::wstring wide = Trim(FromUtf8(item));
    return wide.size() <= 80 ? wide : std::wstring();
}

std::vector<std::wstring> ParsePartialSuggestions(const std::string& text) {
    std::vector<std::wstring> result;
    size_t i = text.find('[');
    if (i == std::string::npos) {
        return result;
    }
    for (i++; i < text.size() && result.size() < 3; i++) {
        if (text[i] == ']') {
            break;
        }
        if (text[i] != '"') {
            continue;
        }
        size_t end = i + 1;
        while (end < text.size() && text[end] != '"') {
            end += text[end] == '\\' ? 2 : 1;
        }
        if (end >= text.size()) {
            break;
        }
        Json value;
        if (ParseJson(text.substr(i, end - i + 1), value) && value.IsString()) {
            std::wstring item = CleanSuggestion(value.string);
            if (!item.empty()) {
                result.push_back(std::move(item));
            }
        }
        i = end;
    }
    return result;
}

std::vector<std::wstring> ParseSuggestions(const std::string& text) {
    std::vector<std::wstring> result;
    auto add = [&](const std::string& item) {
        std::wstring wide = CleanSuggestion(item);
        if (!wide.empty() && result.size() < 3) {
            result.push_back(std::move(wide));
        }
    };

    std::string json = text;
    const size_t open = json.find('[');
    const size_t close = json.rfind(']');
    if (open != std::string::npos && close != std::string::npos && close > open) {
        json = json.substr(open, close - open + 1);
    }
    Json root;
    if (ParseJson(json, root) && root.type == Json::Type::Array) {
        for (const Json& item : root.items) {
            if (item.IsString()) {
                add(item.string);
            }
        }
        return result;
    }
    size_t start = 0;
    while (start < text.size()) {
        size_t end = text.find('\n', start);
        if (end == std::string::npos) {
            end = text.size();
        }
        add(text.substr(start, end - start));
        start = end + 1;
    }
    return result;
}

constexpr int kSuggestionImageSize = 768;
constexpr int kAnswerImageSize = 1536;

DWORD WINAPI GeminiThreadProc(LPVOID param) {
    std::unique_ptr<std::shared_ptr<GeminiJob>> holder(
        static_cast<std::shared_ptr<GeminiJob>*>(param));
    GeminiJob& job = **holder;

    std::vector<std::string> images;
    const HRESULT coInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    for (const auto& image : job.images) {
        std::vector<uint8_t> jpeg;
        const Image scaled = DownscaleImage(
            *image, job.suggestions ? kSuggestionImageSize : kAnswerImageSize);
        if (!job.canceled &&
            EncodeImage(scaled, jpeg, true, job.suggestions ? 0.8f : 0.9f)) {
            images.push_back(Base64Encode(jpeg.data(), jpeg.size()));
        }
    }
    if (SUCCEEDED(coInit)) {
        CoUninitialize();
    }

    std::wstring model = SanitizeModel(job.model);
    const std::wstring fallback = SanitizeModel(job.fallbackModel);
    if ((model.empty() || IsModelMissing(model)) && !fallback.empty()) {
        model = fallback;
    }
    if (model.empty()) {
        model = kDefaultModel;
    }
    Thinking thinking = GetThinking(model, job.suggestions);

    // At most four thinking settings for each of the two possible models.
    constexpr int kMaxAttempts = 8;
    for (int attempt = 0; attempt < kMaxAttempts && !job.canceled; attempt++) {
        const std::wstring path =
            L"/v1beta/models/" + model + L":streamGenerateContent?alt=sse";
        const std::string body = BuildRequestBody(job, images, thinking);
        std::string answer;
        std::string finishReason;
        std::string streamError;
        bool invalidEvent = false;
        size_t shownSuggestions = 0;
        const double start = NowMs();
        double firstText = -1;
        SseParser sse;
        auto onEvent = [&](const std::string& data) {
            Json root;
            if (!ParseJson(data, root) || root.type != Json::Type::Object) {
                const std::string error = ApiErrorMessage(data);
                if (!error.empty()) {
                    streamError = error;
                }
                invalidEvent = true;
                return;
            }
            if (const Json* error = root.Get("error")) {
                if (const Json* message = error->Get("message")) {
                    streamError = message->string;
                }
                if (streamError.empty()) {
                    streamError = "Gemini returned a streaming error.";
                }
                return;
            }
            std::string text;
            ExtractResponseText(root, text, finishReason);
            if (text.empty()) {
                return;
            }
            if (firstText < 0) {
                firstText = NowMs() - start;
            }
            answer += text;
            if (!job.suggestions) {
                PostGeminiMessage(job, GeminiEvent::Chunk, FromUtf8(text));
                return;
            }
            std::vector<std::wstring> partial = ParsePartialSuggestions(answer);
            if (partial.size() > shownSuggestions && partial.size() < 3) {
                shownSuggestions = partial.size();
                PostGeminiMessage(job, GeminiEvent::Suggestions, L"",
                                  std::move(partial), true);
            }
        };

        HttpResult result = SendToGemini(
            L"POST", path, job.apiKey, body, &job.canceled, &job.request,
            job.suggestions ? 20000 : 90000,
            [&](const char* data, size_t size) { sse.Feed(data, size, onEvent); });
        if (job.canceled) {
            break;
        }

        if (result.status == 200) {
            SetThinking(model, job.suggestions, thinking);
            sse.Finish(onEvent);
            Wh_Log(L"%s %s: first text after %d ms, done after %d ms",
                   model.c_str(), job.suggestions ? L"suggestions" : L"answer",
                   static_cast<int>(firstText), static_cast<int>(NowMs() - start));
            std::wstring note;
            if (result.error) {
                note = answer.empty()
                           ? L"Couldn't receive Gemini's answer. Check your "
                             L"internet connection (error "
                           : L"The connection to Gemini was interrupted. This "
                             L"answer is incomplete (error ";
                note += std::to_wstring(result.error) + L"). Try asking again.";
            } else if (!streamError.empty()) {
                note = answer.empty() ? FromUtf8(streamError)
                                      : L"Gemini interrupted this answer: " +
                                            FromUtf8(streamError);
            } else if (invalidEvent) {
                note = L"Gemini returned an unreadable response. Try asking "
                       L"again.";
            } else if (finishReason == "MAX_TOKENS") {
                note = L"Gemini reached its output limit. This answer is "
                       L"incomplete. Try asking for a shorter answer.";
            } else if (!finishReason.empty() && finishReason != "STOP") {
                note = L"Gemini stopped this answer (" +
                       FromUtf8(finishReason) + L"). Try asking another way.";
            } else if (finishReason.empty()) {
                note = L"Gemini stopped before confirming that the answer was "
                       L"complete. Try asking again.";
            } else if (answer.empty()) {
                note = L"Gemini didn't return an answer. Try asking another "
                       L"way.";
            }
            if (job.suggestions) {
                PostGeminiMessage(job, GeminiEvent::Suggestions, note,
                                  note.empty() ? ParseSuggestions(answer)
                                               : ParsePartialSuggestions(answer));
                break;
            }
            PostGeminiMessage(job,
                              note.empty() ? GeminiEvent::Done : GeminiEvent::Error,
                              note);
            break;
        }

        const std::string apiMessage = ApiErrorMessage(result.body);
        if (attempt + 1 < kMaxAttempts && result.status == 400 &&
            thinking != Thinking::Default &&
            (ContainsNoCase(apiMessage, "thinking") ||
             ContainsNoCase(apiMessage, "budget"))) {
            thinking = NextThinking(thinking);
            Wh_Log(L"%s: trying another thinking setting (%d)", model.c_str(),
                   static_cast<int>(thinking));
            continue;
        }
        if (attempt + 1 < kMaxAttempts && result.status == 404 &&
            !fallback.empty() && model != fallback) {
            Wh_Log(L"%s doesn't exist, using %s", model.c_str(), fallback.c_str());
            SetModelMissing(model);
            model = fallback;
            thinking = GetThinking(model, job.suggestions);
            continue;
        }

        std::wstring error;
        if (result.status == 0) {
            error = L"Couldn't reach Gemini. Check your internet connection "
                    L"(error " +
                    std::to_wstring(result.error) + L").";
        } else {
            error = FriendlyError(result.status, apiMessage, model);
        }
        Wh_Log(L"Gemini request failed: %u %u %S", result.status, result.error,
               apiMessage.c_str());
        PostGeminiMessage(job,
                          job.suggestions ? GeminiEvent::Suggestions
                                          : GeminiEvent::Error,
                          error);
        break;
    }

    g_helperThreads--;
    return 0;
}

std::shared_ptr<GeminiJob> StartGeminiJob(std::shared_ptr<GeminiJob> job) {
    auto* param = new std::shared_ptr<GeminiJob>(job);
    g_helperThreads++;
    HANDLE thread = CreateThread(nullptr, 0, GeminiThreadProc, param, 0, nullptr);
    if (!thread) {
        g_helperThreads--;
        delete param;
        return nullptr;
    }
    CloseHandle(thread);
    return job;
}

////////////////////////////////////////////////////////////////////////////////
// The card that opens next to a selection

HWND g_messageWindow;
uint32_t g_lastRequestId;
uint32_t g_lastAttachmentId;

void BeginAddSelection();
void OnCardClosed();
void RequestTick();

struct Attachment {
    std::shared_ptr<const Image> image;  // As captured.
    Image thumb;                         // Downscaled for the card.
    std::wstring text;     // The element's own text, when one was selected.
    std::wstring ocrText;  // Text recognized in the image.
    uint32_t id = 0;
    bool ocrPending = false;
};

struct Palette {
    bool dark;
    Color text, secondary, tertiary, divider, controlFill, controlFillHover,
        controlFillFocus, controlBorder, controlStrong, subtleHover,
        subtlePressed, accent, onAccent, selection, error, userBubble, codeBg,
        tooltipBg, tooltipBorder;
};

Palette MakePalette(bool dark) {
    Palette p{};
    p.dark = dark;
    const Color ink = dark ? kWhite : kBlack;
    p.text = WithAlpha(ink, dark ? 1.0f : 0.894f);
    p.secondary = WithAlpha(ink, dark ? 0.786f : 0.606f);
    p.tertiary = WithAlpha(ink, dark ? 0.544f : 0.446f);
    p.divider = WithAlpha(ink, dark ? 0.0837f : 0.0803f);
    p.controlFill = dark ? WithAlpha(kWhite, 0.0605f) : WithAlpha(kWhite, 0.70f);
    p.controlFillHover =
        dark ? WithAlpha(kWhite, 0.0837f) : Rgb(249, 249, 249, 0.50f);
    p.controlFillFocus = dark ? Rgb(30, 30, 30, 0.70f) : kWhite;
    p.controlBorder = WithAlpha(ink, dark ? 0.07f : 0.0578f);
    p.controlStrong = WithAlpha(ink, dark ? 0.544f : 0.446f);
    p.subtleHover = WithAlpha(ink, dark ? 0.0605f : 0.0373f);
    p.subtlePressed = WithAlpha(ink, dark ? 0.0419f : 0.0241f);
    p.accent = g_glow.Accent(dark);
    const float luminance =
        p.accent.r * 0.2126f + p.accent.g * 0.7152f + p.accent.b * 0.0722f;
    p.onAccent = luminance > 0.45f ? Rgb(0, 0, 0, 0.9f) : kWhite;
    p.selection = WithAlpha(p.accent, 0.38f);
    p.error = dark ? Rgb(255, 153, 164) : Rgb(196, 43, 28);
    p.userBubble = WithAlpha(p.accent, dark ? 0.22f : 0.14f);
    p.codeBg = WithAlpha(kBlack, dark ? 0.24f : 0.045f);
    p.tooltipBg = dark ? Rgb(44, 44, 44, 0.98f) : Rgb(249, 249, 249, 0.98f);
    p.tooltipBorder = WithAlpha(kBlack, dark ? 0.45f : 0.10f);
    return p;
}

std::wstring WindowsLanguageName() {
    WCHAR locale[LOCALE_NAME_MAX_LENGTH]{};
    if (!LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT),
                          locale, ARRAYSIZE(locale), 0)) {
        return L"English";
    }
    if (_wcsnicmp(locale, L"zh", 2) == 0) {  // Chinese comes in two scripts.
        const bool traditional = wcsstr(locale, L"TW") || wcsstr(locale, L"HK") ||
                                 wcsstr(locale, L"MO") || wcsstr(locale, L"Hant");
        return traditional ? L"Chinese (Traditional)" : L"Chinese (Simplified)";
    }
    WCHAR name[80]{};
    return GetLocaleInfoEx(locale, LOCALE_SENGLISHLANGUAGENAME, name,
                           ARRAYSIZE(name)) > 0
               ? std::wstring(name)
               : std::wstring(L"English");
}

std::wstring TranslationLanguage() {
    return g_settings.translateTo.empty() || g_settings.translateTo == L"auto"
               ? WindowsLanguageName()
               : g_settings.translateTo;
}

std::wstring TranslatePrompt(const std::wstring& language) {
    return L"Translate all the text in this selection into " + language +
           L". Reply with only the translation in " + language +
           L", keeping its line breaks. Don't describe the picture or explain "
           L"anything.";
}

struct CardTurn {
    bool fromUser = false;
    std::wstring text;
    bool streaming = false;
    bool error = false;
    std::wstring errorText = {};  // Status after a preserved partial answer.
};

enum class ChipAction { Ask, AskInBrowser, Lens, CopyImage, CopyText };

struct Chip {
    std::wstring label;
    ChipAction action;
};

struct TextBlock {
    enum class Kind { Paragraph, Heading, Bullet, Code };
    Kind kind;
    std::wstring bullet;
    std::wstring text;
};

std::wstring StripInlineMarkdown(const std::wstring& line) {
    std::wstring out;
    out.reserve(line.size());
    for (size_t i = 0; i < line.size(); i++) {
        const wchar_t c = line[i];
        if ((c == L'*' || c == L'_') && i + 1 < line.size() && line[i + 1] == c) {
            i++;
            continue;
        }
        if (c == L'`') {
            continue;
        }
        if (c == L']' && i + 1 < line.size() && line[i + 1] == L'(') {
            const size_t close = line.find(L')', i + 2);
            const size_t open = out.rfind(L'[');
            if (close != std::wstring::npos && open != std::wstring::npos) {
                out.erase(open, 1);
                i = close;
                continue;
            }
        }
        out += c;
    }
    return out;
}

std::vector<TextBlock> ParseMarkdown(const std::wstring& text) {
    std::vector<TextBlock> blocks;
    bool inCode = false;
    std::wstring code;
    size_t start = 0;
    for (;;) {
        size_t end = text.find(L'\n', start);
        const bool last = end == std::wstring::npos;
        if (last) {
            end = text.size();
        }
        std::wstring line = text.substr(start, end - start);
        start = end + 1;
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }
        const std::wstring trimmed = Trim(line);

        if (trimmed.rfind(L"```", 0) == 0) {
            if (inCode) {
                if (!code.empty() && code.back() == L'\n') {
                    code.pop_back();
                }
                blocks.push_back({TextBlock::Kind::Code, L"", code});
                code.clear();
            }
            inCode = !inCode;
        } else if (inCode) {
            code += line + L"\n";
        } else if (trimmed.empty()) {
            // Blank lines only separate blocks.
        } else if (trimmed[0] == L'#') {
            size_t level = 0;
            while (level < trimmed.size() && trimmed[level] == L'#') {
                level++;
            }
            blocks.push_back({TextBlock::Kind::Heading, L"",
                              StripInlineMarkdown(Trim(trimmed.substr(level)))});
        } else if (trimmed.size() > 2 &&
                   (trimmed[0] == L'-' || trimmed[0] == L'*' ||
                    trimmed[0] == L'\x2022') &&
                   trimmed[1] == L' ') {
            blocks.push_back({TextBlock::Kind::Bullet, L"\x2022",
                              StripInlineMarkdown(Trim(trimmed.substr(2)))});
        } else {
            size_t digits = 0;
            while (digits < trimmed.size() && iswdigit(trimmed[digits])) {
                digits++;
            }
            if (digits > 0 && digits < 4 && digits + 1 < trimmed.size() &&
                (trimmed[digits] == L'.' || trimmed[digits] == L')') &&
                trimmed[digits + 1] == L' ') {
                blocks.push_back(
                    {TextBlock::Kind::Bullet, trimmed.substr(0, digits + 1),
                     StripInlineMarkdown(Trim(trimmed.substr(digits + 2)))});
            } else {
                std::wstring paragraph = trimmed;
                if (paragraph.rfind(L"> ", 0) == 0) {
                    paragraph.erase(0, 2);
                }
                blocks.push_back({TextBlock::Kind::Paragraph, L"",
                                  StripInlineMarkdown(paragraph)});
            }
        }
        if (last) {
            break;
        }
    }
    if (inCode && !code.empty()) {
        if (code.back() == L'\n') {
            code.pop_back();
        }
        blocks.push_back({TextBlock::Kind::Code, L"", code});
    }
    return blocks;
}

class AskCard {
  public:
    bool Create() { return panel_.Create(true, false, kCardWindowClass, this); }

    void Destroy() {
        Close();
        panel_.Destroy();
        chrome_.Reset();
        surface_.Reset();
        convo_.Reset();
        underlay_.Reset();
    }

    void Open(Attachment attachment, const RECT& selection, double now) {
        Close();
        if (!Create()) {
            return;
        }
        open_ = true;
        suspended_ = false;
        dragged_ = false;
        selection_ = selection;
        placed_ = false;
        openTime_ = now;
        scale_ = DpiScaleAt(RectCenter(selection));
        Restyle();
        Attach(std::move(attachment));
        ResetChips();
        StartSuggestions(now);
        Update(now);
        panel_.Show(true);
        SetForegroundWindow(panel_.Hwnd());
        SetFocus(panel_.Hwnd());
    }

    void AddAttachment(Attachment attachment, double now) {
        if (!open_) {
            return;
        }
        Attach(std::move(attachment));
        ResetChips();
        StartSuggestions(now);
        Resume(now);
    }

    void Suspend() {
        if (open_ && !suspended_) {
            suspended_ = true;
            panel_.Hide();
        }
    }

    void Resume(double now, bool activate = true) {
        if (open_ && suspended_) {
            suspended_ = false;
            openTime_ = now;
            Update(now);
            panel_.Show(activate);
            if (activate) {
                SetForegroundWindow(panel_.Hwnd());
                SetFocus(panel_.Hwnd());
            }
        }
    }

    void Close() {
        if (!open_) {
            return;
        }
        CancelChat();
        CancelSuggestions();
        if (browserJob_) {
            browserJob_->canceled = true;
            browserJob_.reset();
        }
        open_ = false;
        suspended_ = false;
        if (GetCapture() == panel_.Hwnd()) {
            ReleaseCapture();
        }
        panel_.Hide();
        attachments_.clear();
        turns_.clear();
        chips_.clear();
        convoItems_.clear();
        answerLines_.clear();
        answerText_.clear();
        answerAnchor_ = answerCaret_ = 0;
        answerFocused_ = selectingAnswer_ = false;
        input_.clear();
        caret_ = anchor_ = 0;
        inputScroll_ = 0;
        scroll_ = 0;
        status_.clear();
        hover_ = pressed_ = kHitNone;
        OnCardClosed();
    }

    bool IsOpen() const { return open_; }
    bool CanAddMore() const {
        return open_ && turns_.empty() && attachments_.size() < 3;
    }
    bool IsWaiting() const {
        return chipsLoading_ || (!turns_.empty() && turns_.back().streaming &&
                                 turns_.back().text.empty());
    }
    HWND Hwnd() const { return panel_.Hwnd(); }
    HWND UnderlayHwnd() const { return panel_.UnderlayHwnd(); }

    void OnSettingsChanged(double now) {
        if (!open_) {
            return;
        }
        Restyle();
        if (turns_.empty() && !chipsLoading_) {
            ResetChips();
        }
        Update(now);
    }

    double Tick(double now) {
        if (!open_ || suspended_) {
            return 1e9;
        }
        double next = 1e9;
        bool render = false;

        if (now - openTime_ < kAppearMs + 20) {
            render = true;
            next = 0;
        }
        if (!status_.empty()) {
            if (now >= statusUntil_) {
                status_.clear();
                Update(now);
            } else {
                next = std::min(next, statusUntil_ - now);
            }
        }
        if (focused_ && !answerFocused_) {
            const double blink = std::max<double>(GetCaretBlinkTime(), 200);
            const double phase = fmod(now - caretStart_, blink * 2);
            const bool on = phase < blink;
            if (on != caretOn_) {
                caretOn_ = on;
                render = true;
            }
            next = std::min(next, blink - fmod(phase, blink) + 1);
        }
        if (chipsLoading_) {
            const double waited = now - suggestionsStart_;
            if ((chips_.empty() && waited > kSuggestionsPatienceMs) ||
                waited > kSuggestionsLimitMs) {
                Wh_Log(L"Suggestions took too long");
                CancelSuggestions();
                if (chips_.empty()) {
                    ResetChips();
                }
                Update(now);
            }
        }
        if (IsWaiting() || HasPendingOcr() ||
            (!turns_.empty() && turns_.back().streaming)) {
            render = true;
            next = std::min(next, 33.0);
        }
        panel_.SetUnderlayAlpha(static_cast<BYTE>(
            EaseOutCubic((now - openTime_) / kAppearMs) * GlowBreath(now) * 255.0f + 0.5f));
        next = std::min(next, 1000.0 / 60.0);
        if (selectingAnswer_ && (answerDragPoint_.y < convoRect_.top ||
                                answerDragPoint_.y >= convoRect_.bottom)) {
            ScrollBy(answerDragPoint_.y < convoRect_.top ? -SI(8) : SI(8));
            answerCaret_ = AnswerIndexAt(answerDragPoint_);
            render = true;
        }
        if (hover_ != kHitNone && hover_ <= kHitAdd && !tooltipShown_) {
            const double wait = kTooltipDelayMs - (now - hoverStart_);
            if (wait <= 0) {
                tooltipShown_ = true;
                render = true;
            } else {
                next = std::min(next, wait);
            }
        }
        if (render) {
            Present(now);
        }
        return next;
    }

    void OnGeminiMessage(std::unique_ptr<GeminiMessage> message, double now) {
        if (!open_) {
            return;
        }
        if (message->requestId == chatId_ && chatId_ && !turns_.empty()) {
            CardTurn& turn = turns_.back();
            switch (message->event) {
                case GeminiEvent::Chunk:
                    turn.text += message->text;
                    break;
                case GeminiEvent::Done:
                    turn.streaming = false;
                    chatJob_.reset();
                    break;
                case GeminiEvent::Error:
                    turn.streaming = false;
                    turn.error = true;
                    if (turn.text.empty()) {
                        turn.text = message->text;
                    } else {
                        turn.errorText = message->text;
                    }
                    chatJob_.reset();
                    break;
                case GeminiEvent::Suggestions:
                    break;
            }
            convoDirty_ = true;
            Update(now);
        } else if (message->requestId == suggestionsId_ && suggestionsId_ &&
                   message->event == GeminiEvent::Suggestions) {
            if (turns_.empty()) {
                if (!message->suggestions.empty()) {
                    chips_.clear();
                    for (const std::wstring& suggestion : message->suggestions) {
                        chips_.push_back({suggestion, ChipAction::Ask});
                    }
                } else if (!message->partial) {
                    ResetChips();  // Nothing came back: the usual questions.
                }
            }
            if (!message->partial) {
                chipsLoading_ = false;
                suggestionsJob_.reset();
                suggestionsId_ = 0;
            }
            Update(now);
        }
    }

    void OnBrowserHandoff(std::unique_ptr<BrowserHandoffMessage> message, double now) {
        if (!open_ || !browserJob_ || message->id != browserJob_->id) return;
        browserJob_.reset();
        if (message->pasted) {
            Close();
        } else {
            SetStatus(message->status, now);
            Resume(now, false);  // Keep focus in Gemini or its upload dialog.
            Update(now);
        }
    }

    void OnOcrResult(uint32_t attachmentId, std::wstring text, double now) {
        bool found = false;
        for (Attachment& attachment : attachments_) {
            if (attachment.id == attachmentId) {
                attachment.ocrPending = false;
                attachment.ocrText = Trim(text);
                found = true;
            }
        }
        if (!found || !open_) {
            return;
        }
        if (turns_.empty() && !chipsLoading_ && g_settings.apiKey.empty()) {
            ResetChips();  // Offers to copy the text.
        }
        Update(now);
    }

    static LRESULT CALLBACK WndProc(HWND hwnd,
                                    UINT message,
                                    WPARAM wParam,
                                    LPARAM lParam) {
        if (message == WM_NCCREATE) {
            auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(create->lpCreateParams));
        }
        auto* card =
            reinterpret_cast<AskCard*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (card) {
            LRESULT result = 0;
            if (card->HandleMessage(hwnd, message, wParam, lParam, result)) {
                return result;
            }
        }
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

  private:
    static constexpr double kAppearMs = 220.0;
    static constexpr double kTooltipDelayMs = 450.0;
    static constexpr double kSuggestionsPatienceMs = 8000.0;
    static constexpr double kSuggestionsLimitMs = 15000.0;

    enum Hit {
        kHitNone = 0,
        kHitClose,
        kHitCopy,
        kHitCopyText,
        kHitLens,
        kHitGemini,
        kHitTranslate,
        kHitAdd,
        kHitSend,
        kHitInput,
        kHitConvo,
        kHitChip = 100,        // + chip index.
        kHitCopyAnswer = 200,  // + turn index.
    };

    struct HitRect {
        RECT rect;
        int id;
    };

    struct PanelKey {
        int width = 0;
        int height = 0;
        bool dark = false;
        Backdrop material = Backdrop::None;
        int style = 0;
        int scale = 0;
        bool operator==(const PanelKey&) const = default;
    };

    struct ConvoItem {
        enum class Kind { UserBubble, Text, Thinking, CopyButton, Sparkle };
        Kind kind;
        RECT rect;  // In conversation coordinates.
        RECT textRect;
        std::wstring text;
        std::wstring bullet;
        HFONT font;
        Color color;
        int turn;
        bool code;
    };

    struct AnswerLine {
        RECT rect;  // Conversation coordinates, independent of scrolling.
        size_t start;
        size_t length;
        std::vector<int> extents;
        HFONT font;
        Color color;
        int turn;
    };

    float S(float v) const { return v * scale_; }
    int SI(float v) const { return static_cast<int>(lroundf(v * scale_)); }

    HFONT BodyFont() { return g_fonts.Get(SI(14), FW_NORMAL); }
    HFONT HeadingFont() { return g_fonts.Get(SI(15), FW_SEMIBOLD); }
    HFONT SmallFont() { return g_fonts.Get(SI(12), FW_NORMAL); }
    HFONT CodeFont() { return g_fonts.Get(SI(13), FW_NORMAL, true); }
    HFONT InputFont() { return g_fonts.Get(SI(14), FW_NORMAL); }
    HFONT ChipFont() { return g_fonts.Get(SI(14), FW_NORMAL); }

    void Restyle() {
        palette_ = MakePalette(UseDarkTheme());
        panel_.Configure(palette_.dark);
        chromeKey_ = {};
        underlayKey_ = {};
        convoDirty_ = true;
    }

    void Attach(Attachment attachment) {
        attachment.id = ++g_lastAttachmentId;
        attachment.ocrPending =
            g_settings.ocr && StartOcr(g_messageWindow, attachment.id, attachment.image);
        attachments_.push_back(std::move(attachment));
    }

    bool HasPendingOcr() const {
        for (const Attachment& attachment : attachments_) {
            if (attachment.ocrPending) {
                return true;
            }
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////
    // Actions

    void SetStatus(const std::wstring& text, double now) {
        status_ = text;
        statusUntil_ = now + 4500;
    }

    const Attachment* MainAttachment() const {
        return attachments_.empty() ? nullptr : &attachments_.front();
    }

    std::wstring CopyableText() const {
        std::wstring text;
        for (const Attachment& attachment : attachments_) {
            const std::wstring& part =
                attachment.text.empty() ? attachment.ocrText : attachment.text;
            if (!part.empty()) {
                if (!text.empty()) {
                    text += L"\n\n";
                }
                text += part;
            }
        }
        return text;
    }

    std::wstring ContextText() const {
        std::wstring text;
        auto add = [&](const std::wstring& part) {
            if (part.empty() || text.find(part) != std::wstring::npos) {
                return;
            }
            if (!text.empty()) {
                text += L"\n\n";
            }
            text += part;
        };
        for (const Attachment& attachment : attachments_) {
            add(attachment.text);
            add(attachment.ocrText);
        }
        if (text.size() > 8000) {
            text.resize(8000);
        }
        return text;
    }

    void CopyImage(double now) {
        if (const Attachment* a = MainAttachment()) {
            SetStatus(CopyImageToClipboard(panel_.Hwnd(), *a->image)
                          ? L"Image copied"
                          : L"Couldn't copy the image",
                      now);
        }
    }

    void CopyText(double now) {
        const std::wstring text = CopyableText();
        if (text.empty()) {
            SetStatus(HasPendingOcr() ? L"Still reading the text\x2026"
                                      : L"No text found in the selection",
                      now);
            return;
        }
        SetStatus(CopyTextToClipboard(panel_.Hwnd(), text)
                      ? L"Text copied"
                      : L"Couldn't copy the text",
                  now);
    }

    void OpenLens(double now) {
        if (const Attachment* a = MainAttachment()) {
            CopyImageToClipboard(panel_.Hwnd(), *a->image);
            SearchWithLens(a->image, SelectedBrowser());
            SetStatus(L"Opening Google Lens in your browser\x2026", now);
        }
    }

    std::wstring BrowserQuestion() const {
        const std::wstring typed = Trim(input_);
        if (!typed.empty()) {
            return typed;
        }
        for (size_t i = turns_.size(); i-- > 0;) {
            if (turns_[i].fromUser) {
                return Trim(turns_[i].text);
            }
        }
        return {};
    }

    void AskInBrowser(double now, const std::wstring& prompt) {
        if (const Attachment* a = MainAttachment()) {
            if (!CopyImageToClipboard(panel_.Hwnd(), *a->image)) {
                SetStatus(L"Couldn't copy the image. Try again.", now);
                return;
            }
            if (browserJob_) browserJob_->canceled = true;
            auto job = std::make_shared<BrowserHandoffJob>();
            job->id = ++g_lastRequestId;
            job->notify = g_messageWindow;
            job->clipboardSequence = GetClipboardSequenceNumber();
            job->prompt = prompt;
            for (const Attachment& attachment : attachments_) {
                job->images.push_back(attachment.image);
            }
            job->browser = SelectedBrowser();
            browserJob_ = job;
            CancelSuggestions();
            const bool several = attachments_.size() > 1;
            if (StartBrowserHandoff(job)) {
                SetStatus(several ? L"Opening Gemini and attaching your images\x2026"
                                  : L"Opening Gemini and attaching your image\x2026",
                          now);
                Suspend();  // Let the browser receive focus and the paste.
            } else {
                browserJob_.reset();
                OpenInBrowser(L"https://gemini.google.com/app", job->browser);
                SetStatus(L"Click Gemini's message box, then press Ctrl+V to attach the image.", now);
            }
        }
    }

    void ResetChips() {
        chips_.clear();
        if (g_settings.apiKey.empty()) {
            chips_.push_back({L"Ask in Gemini", ChipAction::AskInBrowser});
            chips_.push_back({L"Search with Google Lens", ChipAction::Lens});
            if (!CopyableText().empty()) {
                chips_.push_back({L"Copy text", ChipAction::CopyText});
            }
            chips_.push_back({L"Copy image", ChipAction::CopyImage});
        } else {
            chips_.push_back({L"What is this?", ChipAction::Ask});
            chips_.push_back({L"Explain this to me", ChipAction::Ask});
            chips_.push_back({L"Summarize this for me", ChipAction::Ask});
        }
    }

    void StartSuggestions(double now) {
        CancelSuggestions();
        if (g_settings.apiKey.empty() || !g_settings.suggestions ||
            attachments_.empty()) {
            return;
        }
        auto job = std::make_shared<GeminiJob>();
        job->id = ++g_lastRequestId;
        job->suggestions = true;
        job->apiKey = g_settings.apiKey;
        job->model = g_settings.suggestionsModel.empty() ? g_settings.model
                                                         : g_settings.suggestionsModel;
        job->fallbackModel = g_settings.model;
        for (const Attachment& attachment : attachments_) {
            job->images.push_back(attachment.image);
        }
        job->context = ToUtf8(ContextText());
        job->notify = g_messageWindow;
        suggestionsJob_ = StartGeminiJob(job);
        if (suggestionsJob_) {
            suggestionsId_ = job->id;
            suggestionsStart_ = now;
            chipsLoading_ = true;
            chips_.clear();
        }
    }

    void CancelSuggestions() {
        if (suggestionsJob_) {
            suggestionsJob_->Cancel();
            suggestionsJob_.reset();
        }
        suggestionsId_ = 0;
        chipsLoading_ = false;
    }

    void CancelChat() {
        if (chatJob_) {
            chatJob_->Cancel();
            chatJob_.reset();
        }
        chatId_ = 0;
        if (!turns_.empty() && turns_.back().streaming) {
            turns_.back().streaming = false;
            turns_.back().error = true;
            if (turns_.back().text.empty()) {
                turns_.back().text = L"Stopped.";
            } else {
                turns_.back().errorText = L"Stopped before the answer finished.";
            }
            convoDirty_ = true;
        }
    }

    void Submit(std::wstring question, double now, bool keepInput = false,
                std::wstring prompt = {}) {
        question = Trim(question);
        if (question.empty() || attachments_.empty()) {
            return;
        }
        if (prompt.empty()) {
            prompt = question;
        }
        if (g_settings.apiKey.empty()) {
            AskInBrowser(now, prompt);
            Update(now);
            return;
        }
        CancelChat();
        CancelSuggestions();
        chips_.clear();

        auto job = std::make_shared<GeminiJob>();
        job->id = ++g_lastRequestId;
        job->apiKey = g_settings.apiKey;
        job->model = g_settings.model;
        for (const Attachment& attachment : attachments_) {
            job->images.push_back(attachment.image);
        }
        job->context = ToUtf8(ContextText());
        job->notify = g_messageWindow;
        for (size_t i = 0; i + 1 < turns_.size(); i += 2) {
            if (turns_[i].fromUser && !turns_[i + 1].error) {
                job->history.push_back({true, ToUtf8(turns_[i].text)});
                job->history.push_back({false, ToUtf8(turns_[i + 1].text)});
            }
        }
        job->history.push_back({true, ToUtf8(prompt)});

        turns_.push_back({true, question, false, false});
        turns_.push_back({false, L"", true, false});
        if (!keepInput) {
            input_.clear();
            caret_ = anchor_ = 0;
            inputScroll_ = 0;
        }
        stickToBottom_ = true;
        convoDirty_ = true;

        chatJob_ = StartGeminiJob(job);
        if (chatJob_) {
            chatId_ = job->id;
        } else {
            turns_.back().streaming = false;
            turns_.back().error = true;
            turns_.back().text = L"Couldn't start the request.";
        }
        Update(now);
    }

    void Translate(double now) {
        const std::wstring language = TranslationLanguage();
        Submit(L"Translate to " + language, now, true, TranslatePrompt(language));
    }

    void Activate(int hit, double now) {
        if (hit >= kHitCopyAnswer) {
            const size_t turn = static_cast<size_t>(hit - kHitCopyAnswer);
            if (turn < turns_.size()) {
                SetStatus(CopyTextToClipboard(panel_.Hwnd(), turns_[turn].text)
                              ? L"Answer copied"
                              : L"Couldn't copy the answer",
                          now);
            }
        } else if (hit >= kHitChip) {
            const size_t index = static_cast<size_t>(hit - kHitChip);
            if (index >= chips_.size()) {
                return;
            }
            const Chip chip = chips_[index];
            switch (chip.action) {
                case ChipAction::Ask:
                    Submit(chip.label, now);
                    return;
                case ChipAction::AskInBrowser:
                    AskInBrowser(now, BrowserQuestion());
                    break;
                case ChipAction::Lens:
                    OpenLens(now);
                    break;
                case ChipAction::CopyImage:
                    CopyImage(now);
                    break;
                case ChipAction::CopyText:
                    CopyText(now);
                    break;
            }
        } else {
            switch (hit) {
                case kHitClose:
                    Close();
                    return;
                case kHitCopy:
                    CopyImage(now);
                    break;
                case kHitCopyText:
                    CopyText(now);
                    break;
                case kHitLens:
                    OpenLens(now);
                    break;
                case kHitGemini:
                    AskInBrowser(now, BrowserQuestion());
                    break;
                case kHitTranslate:
                    Translate(now);
                    return;
                case kHitAdd:
                    if (CanAddMore()) {
                        BeginAddSelection();
                    }
                    return;
                case kHitSend:
                    Submit(input_, now);
                    return;
                default:
                    return;
            }
        }
        Update(now);
    }

    ////////////////////////////////////////////////////////////////////////
    // Text input

    const std::vector<int>& InputExtents() {
        if (extentsText_ != input_ || extentsFont_ != InputFont()) {
            extentsText_ = input_;
            extentsFont_ = InputFont();
            text_.Extents(extentsFont_, input_, extents_);
        }
        return extents_;
    }

    int CaretX(int index) {
        const std::vector<int>& extents = InputExtents();
        index = std::clamp(index, 0, static_cast<int>(extents.size()));
        return index == 0 ? 0 : extents[index - 1];
    }

    int PrevChar(int i) const {
        if (i <= 0) {
            return 0;
        }
        i--;
        if (i > 0 && IS_LOW_SURROGATE(input_[i]) &&
            IS_HIGH_SURROGATE(input_[i - 1])) {
            i--;
        }
        return i;
    }

    int NextChar(int i) const {
        const int size = static_cast<int>(input_.size());
        if (i >= size) {
            return size;
        }
        i++;
        if (i < size && IS_LOW_SURROGATE(input_[i]) &&
            IS_HIGH_SURROGATE(input_[i - 1])) {
            i++;
        }
        return i;
    }

    int PrevWord(int i) const {
        while (i > 0 && iswspace(input_[i - 1])) {
            i--;
        }
        while (i > 0 && !iswspace(input_[i - 1])) {
            i--;
        }
        return i;
    }

    int NextWord(int i) const {
        const int size = static_cast<int>(input_.size());
        while (i < size && !iswspace(input_[i])) {
            i++;
        }
        while (i < size && iswspace(input_[i])) {
            i++;
        }
        return i;
    }

    void DeleteSelection() {
        if (anchor_ != caret_) {
            const int a = std::min(anchor_, caret_);
            const int b = std::max(anchor_, caret_);
            input_.erase(a, b - a);
            caret_ = anchor_ = a;
        }
    }

    void InsertText(std::wstring text) {
        for (wchar_t& c : text) {
            if (c == L'\r' || c == L'\n' || c == L'\t') {
                c = L' ';
            }
        }
        DeleteSelection();
        const size_t room = input_.size() < 4000 ? 4000 - input_.size() : 0;
        if (text.size() > room) {
            text.resize(room);
        }
        input_.insert(caret_, text);
        caret_ += static_cast<int>(text.size());
        anchor_ = caret_;
    }

    void MoveCaret(int position, bool extend) {
        caret_ = std::clamp(position, 0, static_cast<int>(input_.size()));
        if (!extend) {
            anchor_ = caret_;
        }
    }

    int InputIndexAt(int x) {
        const std::vector<int>& extents = InputExtents();
        const int offset = x - (inputText_.left - inputScroll_);
        int previous = 0;
        for (int i = 0; i < static_cast<int>(extents.size()); i = NextChar(i)) {
            const int next = NextChar(i);
            const int width = extents[next - 1];
            if (offset < (previous + width) / 2) {
                return i;
            }
            previous = width;
        }
        return static_cast<int>(input_.size());
    }

    bool HandleKey(WPARAM key, double now) {
        const bool ctrl = GetKeyState(VK_CONTROL) & 0x8000;
        const bool shift = GetKeyState(VK_SHIFT) & 0x8000;
        if (answerFocused_) {
            if (ctrl && key == 'A') {
                answerAnchor_ = 0;
                answerCaret_ = answerText_.size();
                Present(now);
                return true;
            }
            if (ctrl && (key == 'C' || key == 'X')) {
                CopyAnswerSelection(now);
                return true;
            }
            if (key == VK_ESCAPE) {
                Close();
                return true;
            }
            if (key == VK_UP || key == VK_DOWN || key == VK_PRIOR || key == VK_NEXT) {
                const int step = key == VK_UP || key == VK_DOWN ? SI(48)
                    : std::max(SI(48), static_cast<int>(convoRect_.bottom - convoRect_.top) - SI(40));
                ScrollBy(key == VK_UP || key == VK_PRIOR ? -step : step);
                Present(now);
                return true;
            }
            if (key == VK_LEFT || key == VK_RIGHT || key == VK_HOME || key == VK_END) {
                const size_t size = answerText_.size();
                if (!shift && answerAnchor_ != answerCaret_ && (key == VK_LEFT || key == VK_RIGHT)) {
                    answerCaret_ = key == VK_LEFT ? std::min(answerAnchor_, answerCaret_)
                                                  : std::max(answerAnchor_, answerCaret_);
                } else if (key == VK_HOME) {
                    answerCaret_ = 0;
                } else if (key == VK_END) {
                    answerCaret_ = size;
                } else if (key == VK_LEFT && answerCaret_ > 0) {
                    answerCaret_--;
                    if (answerCaret_ > 0 && IS_LOW_SURROGATE(answerText_[answerCaret_])) {
                        answerCaret_--;
                    }
                } else if (key == VK_RIGHT && answerCaret_ < size) {
                    answerCaret_++;
                    if (answerCaret_ < size && IS_LOW_SURROGATE(answerText_[answerCaret_])) {
                        answerCaret_++;
                    }
                }
                if (!shift) {
                    answerAnchor_ = answerCaret_;
                }
                Present(now);
                return true;
            }
            if (key == VK_RETURN || (ctrl && key == 'V')) {
                ClearAnswerSelection();
            } else {
                return false;
            }
        }
        switch (key) {
            case VK_ESCAPE:
                Close();
                return true;
            case VK_RETURN:
                Submit(input_, now);
                return true;
            case VK_LEFT:
                if (!shift && anchor_ != caret_) {
                    MoveCaret(std::min(anchor_, caret_), false);
                } else {
                    MoveCaret(ctrl ? PrevWord(caret_) : PrevChar(caret_), shift);
                }
                break;
            case VK_RIGHT:
                if (!shift && anchor_ != caret_) {
                    MoveCaret(std::max(anchor_, caret_), false);
                } else {
                    MoveCaret(ctrl ? NextWord(caret_) : NextChar(caret_), shift);
                }
                break;
            case VK_HOME:
                MoveCaret(0, shift);
                break;
            case VK_END:
                MoveCaret(static_cast<int>(input_.size()), shift);
                break;
            case VK_BACK:
                if (anchor_ == caret_) {
                    anchor_ = ctrl ? PrevWord(caret_) : PrevChar(caret_);
                }
                DeleteSelection();
                break;
            case VK_DELETE:
                if (anchor_ == caret_) {
                    anchor_ = ctrl ? NextWord(caret_) : NextChar(caret_);
                }
                DeleteSelection();
                break;
            case VK_UP:
            case VK_DOWN:
            case VK_PRIOR:
            case VK_NEXT: {
                const int page = std::max(SI(48), static_cast<int>(convoRect_.bottom - convoRect_.top) - SI(40));
                const int step = (key == VK_UP || key == VK_DOWN) ? SI(48) : page;
                ScrollBy((key == VK_UP || key == VK_PRIOR) ? -step : step);
                Present(now);
                return true;
            }
            case 'A':
                if (!ctrl) {
                    return false;
                }
                anchor_ = 0;
                caret_ = static_cast<int>(input_.size());
                break;
            case 'C':
            case 'X':
                if (!ctrl) {
                    return false;
                }
                if (anchor_ != caret_) {
                    const int a = std::min(anchor_, caret_);
                    const int b = std::max(anchor_, caret_);
                    CopyTextToClipboard(panel_.Hwnd(), input_.substr(a, b - a));
                    if (key == 'X') {
                        DeleteSelection();
                    }
                }
                break;
            case 'V':
                if (!ctrl) {
                    return false;
                }
                InsertText(ReadClipboardText(panel_.Hwnd()));
                break;
            default:
                return false;
        }
        caretStart_ = now;
        caretOn_ = true;
        Update(now);
        return true;
    }

    void ScrollBy(int delta) {
        const int viewport = convoRect_.bottom - convoRect_.top;
        const int maxScroll = std::max(0, contentHeight_ - viewport);
        scroll_ = std::clamp(scroll_ + delta, 0, maxScroll);
        stickToBottom_ = !answerFocused_ && scroll_ >= maxScroll;
        UpdateCopyHits();
    }

    void ClearAnswerSelection() {
        answerFocused_ = selectingAnswer_ = false;
        answerAnchor_ = answerCaret_ = 0;
    }

    void CopyAnswerSelection(double now) {
        const size_t a = std::min(answerAnchor_, answerCaret_);
        const size_t b = std::max(answerAnchor_, answerCaret_);
        if (a < b && b <= answerText_.size()) {
            SetStatus(CopyTextToClipboard(panel_.Hwnd(), answerText_.substr(a, b - a))
                          ? L"Text copied" : L"Couldn't copy the text", now);
            Update(now);
        }
    }

    size_t AnswerIndexAt(POINT pt) const {
        if (answerLines_.empty()) {
            return 0;
        }
        const int y = pt.y - convoRect_.top + scroll_;
        const int x = pt.x - convoRect_.left;
        const AnswerLine* nearest = &answerLines_.front();
        int best = INT_MAX;
        for (const AnswerLine& line : answerLines_) {
            const int gap = y < line.rect.top ? line.rect.top - y
                           : y >= line.rect.bottom ? y - line.rect.bottom + 1 : 0;
            if (gap < best) {
                nearest = &line;
                best = gap;
            }
        }
        int previous = 0;
        for (size_t i = 0; i < nearest->length;) {
            size_t next = i + 1;
            if (next < nearest->length && IS_HIGH_SURROGATE(answerText_[nearest->start + i]) &&
                IS_LOW_SURROGATE(answerText_[nearest->start + next])) {
                next++;
            }
            const int end = nearest->extents[next - 1];
            if (x - nearest->rect.left < (previous + end) / 2) {
                return nearest->start + i;
            }
            previous = end;
            i = next;
        }
        return nearest->start + nearest->length;
    }

    void SelectAnswerAt(POINT pt, bool extend, bool word) {
        answerFocused_ = true;
        stickToBottom_ = false;
        answerCaret_ = AnswerIndexAt(pt);
        if (!extend) {
            answerAnchor_ = answerCaret_;
        }
        if (word && !answerText_.empty()) {
            size_t start = std::min(answerCaret_, answerText_.size() - 1);
            size_t end = start;
            while (start > 0 && !iswspace(answerText_[start - 1])) {
                start--;
            }
            while (end < answerText_.size() && !iswspace(answerText_[end])) {
                end++;
            }
            answerAnchor_ = start;
            answerCaret_ = end;
        }
    }


    ////////////////////////////////////////////////////////////////////////
    // Layout

    void LayoutConversation() {
        convoItems_.clear();
        const int width = convoWidth_;
        const int textLeft = SI(26);
        int y = 0;
        for (size_t t = 0; t < turns_.size(); t++) {
            const CardTurn& turn = turns_[t];
            const int turnIndex = static_cast<int>(t);
            if (turn.fromUser) {
                HFONT font = BodyFont();
                const int maxWidth = static_cast<int>(width * 0.82f) - SI(24);
                const SIZE size =
                    text_.Measure(font, turn.text, maxWidth, DT_WORDBREAK);
                const int bubbleWidth =
                    std::min(maxWidth, static_cast<int>(size.cx) + SI(3)) + SI(24);
                const int bubbleHeight = size.cy + SI(16);
                convoItems_.push_back(
                    {ConvoItem::Kind::UserBubble,
                     {width - bubbleWidth, y, width, y + bubbleHeight},
                     {width - bubbleWidth + SI(12), y + SI(8), width - SI(12),
                      y + SI(8) + size.cy},
                     turn.text, L"", font, palette_.text, turnIndex, false});
                y += bubbleHeight + SI(14);
                continue;
            }

            convoItems_.push_back({ConvoItem::Kind::Sparkle,
                                   {0, y + SI(1), SI(18), y + SI(19)},
                                   {}, L"", L"", nullptr, palette_.text,
                                   turnIndex, false});
            if (turn.streaming && turn.text.empty()) {
                const RECT rc{textLeft, y - SI(1), width, y + SI(21)};
                convoItems_.push_back({ConvoItem::Kind::Thinking, rc, rc,
                                       L"Thinking\x2026", L"", BodyFont(),
                                       palette_.secondary, turnIndex, false});
                y += SI(20) + SI(14);
                continue;
            }
            if (turn.error && turn.errorText.empty()) {
                HFONT font = BodyFont();
                const auto lines = text_.Wrap(font, turn.text, width - textLeft);
                const SIZE size{width - textLeft, static_cast<LONG>(lines.size()) *
                    text_.Measure(font, L"Mg", width - textLeft, DT_SINGLELINE).cy};
                const RECT rc{textLeft, y, width, y + size.cy};
                convoItems_.push_back({ConvoItem::Kind::Text, rc, rc, turn.text,
                                       L"", font, palette_.error, turnIndex,
                                       false});
                y += size.cy + SI(14);
                continue;
            }

            bool first = true;
            for (const TextBlock& block : ParseMarkdown(turn.text)) {
                if (block.text.empty()) {
                    continue;
                }
                if (!first) {
                    y += block.kind == TextBlock::Kind::Heading ? SI(10) : SI(6);
                }
                first = false;
                const bool code = block.kind == TextBlock::Kind::Code;
                HFONT font = block.kind == TextBlock::Kind::Heading ? HeadingFont()
                             : code                                  ? CodeFont()
                                                                     : BodyFont();
                int left = textLeft;
                int right = width;
                if (block.kind == TextBlock::Kind::Bullet) {
                    left += SI(block.bullet.size() > 1 ? 24 : 16);
                } else if (code) {
                    left += SI(10);
                    right -= SI(10);
                }
                std::wstring display;
                int column = 0;
                for (wchar_t c : block.text) {
                    if (c == L'\t') {
                        const int spaces = 4 - column % 4;
                        display.append(spaces, L' ');
                        column += spaces;
                    } else {
                        display += c;
                        column = c == L'\n' ? 0 : column + 1;
                    }
                }
                const auto lines = text_.Wrap(font, display, right - left);
                const SIZE size{right - left, static_cast<LONG>(lines.size()) *
                    text_.Measure(font, L"Mg", right - left, DT_SINGLELINE).cy};
                ConvoItem item{ConvoItem::Kind::Text,
                               {textLeft, y, width, y + size.cy},
                               {left, y, right, y + size.cy},
                               display, block.bullet, font, palette_.text,
                               turnIndex, code};
                if (code) {
                    item.rect.bottom += SI(16);
                    OffsetRect(&item.textRect, 0, SI(8));
                    y += SI(16);
                }
                convoItems_.push_back(item);
                y += size.cy;
            }
            y += SI(8);
            if (turn.error && !turn.errorText.empty()) {
                HFONT font = BodyFont();
                const auto lines = text_.Wrap(font, turn.errorText, width - textLeft);
                const int height = static_cast<int>(lines.size()) *
                    text_.Measure(font, L"Mg", width - textLeft, DT_SINGLELINE).cy;
                const RECT rc{textLeft, y, width, y + height};
                convoItems_.push_back({ConvoItem::Kind::Text, rc, rc, turn.errorText,
                                       L"", font, palette_.error, turnIndex, false});
                y += height + SI(8);
            }
            convoItems_.push_back({ConvoItem::Kind::CopyButton,
                                   {textLeft - SI(8), y, textLeft + SI(64), y + SI(28)},
                                   {}, L"Copy", L"", SmallFont(), palette_.secondary,
                                   turnIndex, false});
            y += SI(28) + SI(14);
        }
        contentHeight_ = std::max(0, y - SI(14));
        BuildAnswerLines();
    }

    void BuildAnswerLines() {
        answerLines_.clear();
        answerText_.clear();
        for (const ConvoItem& item : convoItems_) {
            if (item.kind != ConvoItem::Kind::Text) {
                continue;
            }
            if (!answerText_.empty()) {
                answerText_ += L"\n\n";
            }
            if (!item.bullet.empty()) {
                answerText_ += item.bullet + L" ";
            }
            const size_t base = answerText_.size();
            answerText_ += item.text;
            const int height = text_.Measure(item.font, L"Mg", 1, DT_SINGLELINE).cy;
            int y = item.textRect.top;
            for (WrappedTextLine& line : text_.Wrap(item.font, item.text,
                                                   item.textRect.right - item.textRect.left)) {
                answerLines_.push_back({{item.textRect.left, y, item.textRect.right, y + height},
                                         base + line.start, line.length, std::move(line.extents),
                                         item.font, item.color, item.turn});
                y += height;
            }
        }
        answerAnchor_ = std::min(answerAnchor_, answerText_.size());
        answerCaret_ = std::min(answerCaret_, answerText_.size());
    }

    bool ConversationCacheCoversViewport() const {
        const int viewport = std::max(1, static_cast<int>(convoRect_.bottom - convoRect_.top));
        return convo_.Valid() && convo_.Width() == std::max(1, convoWidth_) &&
               scroll_ >= convoCacheTop_ &&
               scroll_ + viewport <= convoCacheTop_ + convo_.Height();
    }

    void RenderConversation() {
        const int viewport = std::max(1, static_cast<int>(convoRect_.bottom - convoRect_.top));
        const int cacheHeight = std::min(std::max(1, contentHeight_), viewport * 3);
        convoCacheTop_ = std::clamp(scroll_ - viewport, 0,
                                   std::max(0, contentHeight_ - cacheHeight));
        if (!convo_.Resize(std::max(1, convoWidth_), cacheHeight)) {
            return;
        }
        const int cacheBottom = convoCacheTop_ + cacheHeight;
        for (const ConvoItem& item : convoItems_) {
            if (item.rect.bottom <= convoCacheTop_ || item.rect.top >= cacheBottom) {
                continue;
            }
            RECT textRect = item.textRect;
            OffsetRect(&textRect, 0, -convoCacheTop_);
            const float l = static_cast<float>(item.rect.left);
            const float t = static_cast<float>(item.rect.top - convoCacheTop_);
            const float r = static_cast<float>(item.rect.right);
            const float b = static_cast<float>(item.rect.bottom - convoCacheTop_);
            switch (item.kind) {
                case ConvoItem::Kind::UserBubble:
                    FillRoundRect(convo_, l, t, r, b, S(8), palette_.userBubble);
                    text_.Draw(convo_, item.font, item.text, textRect,
                               DT_WORDBREAK, item.color);
                    break;
                case ConvoItem::Kind::Text:
                    if (item.code) {
                        FillRoundRect(convo_, l, t, r, b, S(6), palette_.codeBg);
                    }
                    if (!item.bullet.empty()) {
                        const RECT bulletRect{item.rect.left, textRect.top,
                                              textRect.left, textRect.bottom};
                        text_.Draw(convo_, item.font, item.bullet, bulletRect,
                                   DT_LEFT | DT_SINGLELINE, palette_.secondary);
                    }
                    break;
                default:
                    break;
            }
        }
        for (const AnswerLine& line : answerLines_) {
            if (line.rect.bottom <= convoCacheTop_ || line.rect.top >= cacheBottom) {
                continue;
            }
            RECT rc = line.rect;
            OffsetRect(&rc, 0, -convoCacheTop_);
            text_.Draw(convo_, line.font, answerText_.substr(line.start, line.length),
                       rc, DT_SINGLELINE | DT_LEFT, line.color);
        }
        convoDirty_ = false;
    }

    void UpdateCopyHits() {
        hits_.erase(std::remove_if(hits_.begin(), hits_.end(),
                                   [](const HitRect& hit) {
                                       return hit.id >= kHitCopyAnswer;
                                   }),
                    hits_.end());
        for (const ConvoItem& item : convoItems_) {
            if (item.kind != ConvoItem::Kind::CopyButton) {
                continue;
            }
            RECT rc = item.rect;
            OffsetRect(&rc, convoRect_.left, convoRect_.top - scroll_);
            RECT visible;
            if (IntersectRect(&visible, &rc, &convoRect_)) {
                hits_.insert(hits_.begin(), HitRect{visible, kHitCopyAnswer + item.turn});
            }
        }
    }

    void Layout() {
        width_ = SI(400);
        const int pad = SI(12);
        hits_.clear();

        int x = pad;
        const int headerTop = pad;
        const int thumbSize = SI(56);
        thumbRects_.clear();
        for (size_t i = 0; i < attachments_.size(); i++) {
            thumbRects_.push_back({x, headerTop, x + thumbSize, headerTop + thumbSize});
            x += thumbSize + SI(8);
        }
        addRect_ = {};
        if (CanAddMore()) {
            addRect_ = {x, headerTop, x + thumbSize, headerTop + thumbSize};
            hits_.push_back({addRect_, kHitAdd});
        }
        const bool hasKey = !g_settings.apiKey.empty();
        std::vector<int> buttons{kHitClose};
        if (hasKey) {
            buttons.push_back(kHitGemini);
        }
        buttons.push_back(kHitLens);
        if (hasKey) {
            buttons.push_back(kHitTranslate);
        }
        buttons.push_back(kHitCopy);
        if (!CopyableText().empty()) {
            buttons.push_back(kHitCopyText);
        }
        const int count = static_cast<int>(buttons.size());
        const int right = width_ - pad + SI(4);
        const int used = std::max<int>(x - SI(8), addRect_.right);
        const int gaps = SI(6) + SI(2) * (count - 2);
        const int buttonSize =
            std::clamp((right - used - SI(6) - gaps) / count, SI(26), SI(32));
        int bx = right - buttonSize;
        const int by = headerTop - SI(4);
        for (RECT& rc : buttonRects_) {
            rc = {};
        }
        for (int id : buttons) {
            buttonRects_[id] = {bx, by, bx + buttonSize, by + buttonSize};
            hits_.push_back({buttonRects_[id], id});
            bx -= buttonSize + (id == kHitClose ? SI(6) : SI(2));
        }
        int y = headerTop + thumbSize;

        convoWidth_ = width_ - pad * 2;
        convoRect_ = {};
        if (!turns_.empty()) {
            if (convoDirty_ || convoLayoutWidth_ != convoWidth_) {
                LayoutConversation();
                convoLayoutWidth_ = convoWidth_;
                convoDirty_ = true;
            }
            y += SI(14);
            const RECT work = MonitorRectAt(RectCenter(selection_), true);
            const int maxHeight = std::clamp(
                static_cast<int>((work.bottom - work.top) * 0.5f), SI(140), SI(560));
            const int height = std::min(contentHeight_, maxHeight);
            convoRect_ = {pad, y, width_ - pad, y + height};
            const int maxScroll = std::max(0, contentHeight_ - height);
            if (stickToBottom_) {
                scroll_ = maxScroll;
            }
            scroll_ = std::clamp(scroll_, 0, maxScroll);
            y += height;
        }

        statusText_ = status_;
        statusIsHint_ = false;
        if (statusText_.empty() && g_settings.apiKey.empty() && turns_.empty()) {
            statusText_ =
                L"Add a Gemini API key in the mod's settings to get answers "
                L"right here.";
            statusIsHint_ = true;
        }
        statusRect_ = {};
        if (!statusText_.empty()) {
            const SIZE size = text_.Measure(SmallFont(), statusText_,
                                            width_ - pad * 2, DT_WORDBREAK);
            y += SI(10);
            statusRect_ = {pad, y, width_ - pad, y + size.cy};
            y += size.cy;
        }

        y += SI(12);
        inputRect_ = {pad, y, width_ - pad, y + SI(40)};
        const int sendSize = SI(30);
        sendRect_ = {inputRect_.right - SI(5) - sendSize, inputRect_.top + SI(5),
                     inputRect_.right - SI(5), inputRect_.top + SI(5) + sendSize};
        inputText_ = {inputRect_.left + SI(12), inputRect_.top,
                      sendRect_.left - SI(8), inputRect_.bottom};
        hits_.push_back({sendRect_, kHitSend});
        hits_.push_back({inputRect_, kHitInput});
        if (convoRect_.bottom > convoRect_.top) {
            hits_.push_back({convoRect_, kHitConvo});
        }
        y = inputRect_.bottom;

        chipRects_.clear();
        dividerY_ = -1;
        if (turns_.empty() && (chipsLoading_ || !chips_.empty())) {
            y += SI(10);
            dividerY_ = y;
            y += SI(5);
            const size_t count = chipsLoading_ ? std::max<size_t>(3, chips_.size())
                                               : chips_.size();
            for (size_t i = 0; i < count; i++) {
                const RECT rc{SI(5), y, width_ - SI(5), y + SI(36)};
                chipRects_.push_back(rc);
                if (i < chips_.size()) {
                    hits_.push_back({rc, kHitChip + static_cast<int>(i)});
                }
                y += SI(36);
            }
            y += SI(5);
        } else {
            y += pad;
        }
        height_ = y;
        UpdateCopyHits();

        const int caretX = CaretX(caret_);
        const int visibleWidth =
            std::max(1, static_cast<int>(inputText_.right - inputText_.left) - SI(2));
        if (caretX - inputScroll_ > visibleWidth) {
            inputScroll_ = caretX - visibleWidth;
        } else if (caretX < inputScroll_) {
            inputScroll_ = caretX;
        }
        inputScroll_ = std::max(0, inputScroll_);
    }

    void Position() {
        const POINT anchor = dragged_ ? POINT{cardPos_.x + width_ / 2, cardPos_.y + SI(24)}
                                      : RectCenter(selection_);
        const RECT work = MonitorRectAt(anchor, true);
        const int gap = SI(12);
        const int edge = SI(8);
        if (!placed_) {
            const POINT candidates[] = {
                {selection_.left, selection_.bottom + gap},
                {selection_.left, selection_.top - gap - height_},
                {selection_.right + gap, selection_.top},
                {selection_.left - gap - width_, selection_.top},
                {selection_.right - gap - width_, selection_.bottom - gap - height_},
            };
            cardPos_ = candidates[0];
            for (const POINT& candidate : candidates) {
                if (candidate.y >= work.top + edge &&
                    candidate.y + height_ <= work.bottom - edge &&
                    candidate.x + width_ / 2 >= work.left &&
                    candidate.x + width_ / 2 <= work.right) {
                    cardPos_ = candidate;
                    break;
                }
            }
            placed_ = true;
        }
        const int minX = work.left + edge;
        const int maxX = std::max(minX, static_cast<int>(work.right) - edge - width_);
        const int x = std::clamp(static_cast<int>(cardPos_.x), minX, maxX);
        int y = std::min(static_cast<int>(cardPos_.y),
                         static_cast<int>(work.bottom) - edge - height_);
        y = std::max(y, static_cast<int>(work.top) + edge);
        windowPos_ = {x, y};
    }

    void Update(double now) {
        if (!open_) {
            return;
        }
        Layout();
        if (convoDirty_) {
            RenderConversation();
        }
        Position();
        Present(now);
    }

    ////////////////////////////////////////////////////////////////////////
    // Rendering

    PanelKey Key() const {
        return {width_, height_, palette_.dark, panel_.Material(), g_styleVersion, SI(100)};
    }

    void RenderBase() {
        if (chrome_.Valid() && Key() == chromeKey_) {
            return;
        }
        chromeKey_ = Key();
        chrome_.Resize(width_, height_);
        PaintPanelBase(chrome_, panel_, scale_);
    }

    bool RenderUnderlay() {
        const PanelKey key = Key();
        if (underlay_.Valid() && key == underlayKey_) {
            return false;
        }
        underlayKey_ = key;
        underlayMargin_ = SI(84);
        PaintPanelUnderlay(underlay_, width_, height_, underlayMargin_,
                           panel_.Radius(scale_), scale_, palette_.dark,
                           palette_.dark ? 0.95f : 0.70f, 0.0f);
        return true;
    }

    void DrawButton(int id, Glyph glyph) {
        const RECT& rc = buttonRects_[id];
        if (rc.right <= rc.left) {
            return;
        }
        const bool hovered = hover_ == id;
        if (hovered) {
            FillRoundRect(surface_, static_cast<float>(rc.left),
                          static_cast<float>(rc.top), static_cast<float>(rc.right),
                          static_cast<float>(rc.bottom), S(4),
                          pressed_ == id ? palette_.subtlePressed : palette_.subtleHover);
        }
        DrawGlyph(surface_, text_, glyph, (rc.left + rc.right) * 0.5f,
                  (rc.top + rc.bottom) * 0.5f, scale_,
                  hovered ? palette_.text : palette_.secondary);
    }

    void DrawThumbnails(double now) {
        for (size_t i = 0; i < attachments_.size() && i < thumbRects_.size(); i++) {
            const RECT& rc = thumbRects_[i];
            Attachment& attachment = attachments_[i];
            const int wanted =
                std::min(static_cast<int>(rc.right - rc.left) * 2,
                         std::max(attachment.image->width, attachment.image->height));
            if (attachment.thumb.Empty() ||
                std::max(attachment.thumb.width, attachment.thumb.height) < wanted) {
                attachment.thumb = DownscaleImage(*attachment.image, std::max(1, wanted));
            }
            const float l = static_cast<float>(rc.left);
            const float t = static_cast<float>(rc.top);
            const float r = static_cast<float>(rc.right);
            const float b = static_cast<float>(rc.bottom);
            DrawImageRounded(surface_, attachment.thumb, l, t, r, b, S(6),
                             palette_.controlFill);
            StrokeRoundRect(surface_, l + 0.5f, t + 0.5f, r - 0.5f, b - 0.5f, S(6),
                            1.0f, palette_.controlBorder);
            if (attachment.ocrPending) {
                const float sweep = static_cast<float>(fmod(now / 1300.0, 1.0));
                const float lineY = t - S(8) + (b - t + S(16)) * sweep;
                const float sigma = S(5);
                const Color glow = Mix(g_glow.At(static_cast<float>(now / 3000.0)),
                                       kWhite, 0.35f);
                FillShape(
                    surface_, l, std::max(t, lineY - sigma * 3), r,
                    std::min(b, lineY + sigma * 3),
                    [&](float x, float y) {
                        return SdRoundRect(x, y, l, t, r, b, S(6));
                    },
                    [&](float, float y) {
                        const float d = (y - lineY) / sigma;
                        return WithAlpha(glow, 0.75f * expf(-d * d));
                    });
            }
        }
        if (addRect_.right > addRect_.left) {
            const bool hovered = hover_ == kHitAdd;
            const Color color = hovered ? palette_.text : palette_.secondary;
            const float l = static_cast<float>(addRect_.left);
            const float t = static_cast<float>(addRect_.top);
            const float r = static_cast<float>(addRect_.right);
            const float b = static_cast<float>(addRect_.bottom);
            if (hovered) {
                FillRoundRect(surface_, l, t, r, b, S(6), palette_.subtleHover);
            }
            StrokeRoundRectDashed(surface_, l + S(1), t + S(1), r - S(1), b - S(1),
                                  S(6), S(1.2f), S(4), WithAlpha(color, 0.75f));
            DrawGlyph(surface_, text_, Glyph::Add, (l + r) * 0.5f, (t + b) * 0.5f,
                      scale_, color);
        }
    }

    void DrawConversation(double now) {
        if (!ConversationCacheCoversViewport()) {
            RenderConversation();
        }
        const int viewport = convoRect_.bottom - convoRect_.top;
        const int fade = SI(18);
        const bool fadeTop = scroll_ > 0;
        const bool fadeBottom = scroll_ + viewport < contentHeight_;
        for (int y = 0; y < viewport; y++) {
            const int sourceY = y + scroll_ - convoCacheTop_;
            if (sourceY < 0 || sourceY >= convo_.Height()) {
                continue;
            }
            float factor = 1.0f;
            if (fadeTop && y < fade) {
                factor = std::min(factor, static_cast<float>(y) / fade);
            }
            if (fadeBottom && viewport - y < fade) {
                factor = std::min(factor, static_cast<float>(viewport - y) / fade);
            }
            const uint32_t* src = convo_.Row(sourceY);
            uint32_t* dst = surface_.Row(convoRect_.top + y) + convoRect_.left;
            const uint32_t k = static_cast<uint32_t>(factor * 255.0f + 0.5f);
            for (int x = 0; x < convoWidth_ && x < convo_.Width(); x++) {
                uint32_t pixel = src[x];
                if (!pixel) {
                    continue;
                }
                if (k < 255) {
                    pixel = ScalePremultiplied(pixel, k);
                }
                BlendOver(dst[x], pixel);
            }
        }

        const size_t selectionStart = std::min(answerAnchor_, answerCaret_);
        const size_t selectionEnd = std::max(answerAnchor_, answerCaret_);
        if (selectionStart < selectionEnd) {
            for (const AnswerLine& line : answerLines_) {
                const size_t a = std::max(selectionStart, line.start);
                const size_t b = std::min(selectionEnd, line.start + line.length);
                if (a >= b) {
                    continue;
                }
                const size_t first = a - line.start;
                const size_t last = b - line.start;
                RECT layout = line.rect;
                OffsetRect(&layout, convoRect_.left, convoRect_.top - scroll_);
                RECT highlight = layout;
                highlight.left += first ? line.extents[first - 1] : 0;
                highlight.right = layout.left + (last ? line.extents[last - 1] : 0);
                RECT clip;
                if (IntersectRect(&clip, &highlight, &convoRect_)) {
                    FillRoundRect(surface_, static_cast<float>(clip.left),
                                  static_cast<float>(clip.top), static_cast<float>(clip.right),
                                  static_cast<float>(clip.bottom), S(2),
                                  WithAlpha(palette_.accent, focused_ ? 0.28f : 0.15f));
                    text_.Draw(surface_, line.font, answerText_.substr(line.start, line.length),
                               layout, clip, DT_SINGLELINE | DT_LEFT, line.color);
                }
            }
        }

        for (const ConvoItem& item : convoItems_) {
            RECT rc = item.rect;
            OffsetRect(&rc, convoRect_.left, convoRect_.top - scroll_);
            RECT visible;
            if (!IntersectRect(&visible, &rc, &convoRect_)) {
                continue;
            }
            switch (item.kind) {
                case ConvoItem::Kind::Sparkle: {
                    const bool working = item.turn == static_cast<int>(turns_.size()) - 1 &&
                                         turns_.back().streaming;
                    const float angle =
                        working ? static_cast<float>(fmod(now / 900.0, 1.0)) * 2.0f * kPi
                                : 0.0f;
                    FillSparkleGradient(surface_, (rc.left + rc.right) * 0.5f,
                                        (rc.top + rc.bottom) * 0.5f, S(8.5f), angle,
                                        working ? static_cast<float>(now / 2000.0) : 0.1f);
                    break;
                }
                case ConvoItem::Kind::Thinking: {
                    text_.Draw(surface_, item.font, item.text, rc, convoRect_,
                               DT_SINGLELINE | DT_VCENTER | DT_LEFT, palette_.secondary);
                    const int width = text_.Width(item.font, item.text.c_str(),
                                                  static_cast<int>(item.text.size()));
                    const float sweep = static_cast<float>(fmod(now / 1300.0, 1.0));
                    const float center = rc.left + (width + S(60)) * sweep - S(30);
                    const RECT shine{static_cast<LONG>(center - S(22)), rc.top,
                                     static_cast<LONG>(center + S(22)), rc.bottom};
                    RECT clip;
                    if (IntersectRect(&clip, &shine, &visible)) {
                        text_.Draw(surface_, item.font, item.text, rc, clip,
                                   DT_SINGLELINE | DT_VCENTER | DT_LEFT, palette_.accent);
                    }
                    break;
                }
                case ConvoItem::Kind::CopyButton: {
                    const bool hovered = hover_ == kHitCopyAnswer + item.turn;
                    if (hovered) {
                        FillRoundRect(surface_, static_cast<float>(visible.left),
                                      static_cast<float>(visible.top),
                                      static_cast<float>(visible.right),
                                      static_cast<float>(visible.bottom), S(4),
                                      palette_.subtleHover);
                    }
                    const Color color = hovered ? palette_.text : palette_.secondary;
                    const float cy = (rc.top + rc.bottom) * 0.5f;
                    if (cy > convoRect_.top && cy < convoRect_.bottom) {
                        DrawGlyph(surface_, text_, Glyph::Copy, rc.left + S(18), cy,
                                  scale_ * 0.875f, color);
                    }
                    const RECT label{rc.left + SI(32), rc.top, rc.right, rc.bottom};
                    text_.Draw(surface_, item.font, item.text, label, convoRect_,
                               DT_SINGLELINE | DT_VCENTER | DT_LEFT, color);
                    break;
                }
                default:
                    break;
            }
        }
    }

    void DrawInput() {
        const bool inputFocused = focused_ && !answerFocused_;
        const float l = static_cast<float>(inputRect_.left);
        const float t = static_cast<float>(inputRect_.top);
        const float r = static_cast<float>(inputRect_.right);
        const float b = static_cast<float>(inputRect_.bottom);
        const float radius = S(4);
        const bool hovered = hover_ == kHitInput || hover_ == kHitSend;
        FillRoundRect(surface_, l, t, r, b, radius,
                      inputFocused ? palette_.controlFillFocus
                      : hovered ? palette_.controlFillHover
                                : palette_.controlFill);
        StrokeRoundRect(surface_, l + 0.5f, t + 0.5f, r - 0.5f, b - 0.5f, radius,
                        1.0f, palette_.controlBorder);
        const float line = inputFocused ? std::max(2.0f, S(2)) : 1.0f;
        const Color lineColor = inputFocused ? palette_.accent : palette_.controlStrong;
        FillShape(
            surface_, l, b - line - 1, r, b,
            [&](float x, float y) {
                return std::max(SdRoundRect(x, y, l, t, r, b, radius), b - line - y);
            },
            [&](float, float) { return lineColor; });

        HFONT font = InputFont();
        const int textX = inputText_.left - inputScroll_;
        if (anchor_ != caret_) {
            const int x0 = textX + CaretX(std::min(anchor_, caret_));
            const int x1 = textX + CaretX(std::max(anchor_, caret_));
            const float left = static_cast<float>(std::max(x0, static_cast<int>(inputText_.left)));
            const float right = static_cast<float>(std::min(x1, static_cast<int>(inputText_.right)));
            if (right > left) {
                FillRoundRect(surface_, left, t + S(10), right, b - S(10), S(2),
                              palette_.selection);
            }
        }
        if (input_.empty()) {
            text_.Draw(surface_, font,
                       turns_.empty() ? L"Ask about your selection\x2026" : L"Ask a follow-up\x2026",
                       inputText_, DT_SINGLELINE | DT_VCENTER | DT_LEFT,
                       palette_.secondary);
        } else {
            const RECT layout{textX, inputText_.top, textX + 100000, inputText_.bottom};
            text_.Draw(surface_, font, input_, layout, inputText_,
                       DT_SINGLELINE | DT_VCENTER | DT_LEFT, palette_.text);
        }
        if (inputFocused && caretOn_) {
            const float x = textX + CaretX(caret_) + 0.5f;
            if (x >= inputText_.left - 1 && x <= inputText_.right + 1) {
                StrokeLine(surface_, {x, t + S(11)}, {x, b - S(11)}, std::max(1.0f, S(1)),
                           palette_.text);
            }
        }

        const float sl = static_cast<float>(sendRect_.left);
        const float st = static_cast<float>(sendRect_.top);
        const float sr = static_cast<float>(sendRect_.right);
        const float sb = static_cast<float>(sendRect_.bottom);
        const bool enabled = !Trim(input_).empty();
        if (enabled) {
            FillRoundRect(surface_, sl, st, sr, sb, S(4),
                          hover_ == kHitSend ? WithAlpha(palette_.accent, 0.9f)
                                             : palette_.accent);
        } else if (hover_ == kHitSend) {
            FillRoundRect(surface_, sl, st, sr, sb, S(4), palette_.subtleHover);
        }
        DrawGlyph(surface_, text_, Glyph::Send, (sl + sr) * 0.5f, (st + sb) * 0.5f,
                  scale_ * 0.875f, enabled ? palette_.onAccent : palette_.tertiary);
    }

    void DrawChips(double now) {
        if (chipRects_.empty()) {
            return;
        }
        if (dividerY_ >= 0) {
            FillRoundRect(surface_, static_cast<float>(SI(12)),
                          static_cast<float>(dividerY_),
                          static_cast<float>(width_ - SI(12)),
                          static_cast<float>(dividerY_) + std::max(1.0f, S(1)), 0,
                          palette_.divider);
        }
        for (size_t i = 0; i < chipRects_.size(); i++) {
            const RECT& rc = chipRects_[i];
            const float l = static_cast<float>(rc.left);
            const float t = static_cast<float>(rc.top);
            const float r = static_cast<float>(rc.right);
            const float b = static_cast<float>(rc.bottom);
            const float cy = (t + b) * 0.5f;
            const float iconX = l + S(21);
            if (i >= chips_.size()) {
                FillSparkleGradient(surface_, iconX, cy, S(6.5f),
                                    static_cast<float>(fmod(now / 1100.0, 1.0)) * 2.0f * kPi,
                                    static_cast<float>(now / 2000.0));
                const float sweep = static_cast<float>(fmod(now / 1200.0 + i * 0.15, 1.0));
                const float barL = l + S(41);
                const float barR = barL + (r - S(16) - barL) * (i % 3 == 0   ? 0.62f
                                                                 : i % 3 == 1 ? 0.80f
                                                                              : 0.52f);
                const float barT = cy - S(5);
                const float barB = cy + S(5);
                const float center = barL + (barR - barL + S(80)) * sweep - S(40);
                FillShape(
                    surface_, barL, barT, barR, barB,
                    [&](float x, float y) {
                        return SdRoundRect(x, y, barL, barT, barR, barB, S(5));
                    },
                    [&](float x, float) {
                        const float k = Clamp01(1.0f - fabsf(x - center) / S(48));
                        return Mix(WithAlpha(palette_.text, 0.08f),
                                   WithAlpha(palette_.accent, 0.45f), k);
                    });
                continue;
            }
            const int id = kHitChip + static_cast<int>(i);
            const bool hovered = hover_ == id;
            if (hovered) {
                FillRoundRect(surface_, l, t, r, b, S(4),
                              pressed_ == id ? palette_.subtlePressed : palette_.subtleHover);
            }
            const Chip& chip = chips_[i];
            const Color iconColor = hovered ? palette_.text : palette_.secondary;
            switch (chip.action) {
                case ChipAction::Ask:
                    FillSparkleGradient(surface_, iconX, cy, S(6.5f), 0.0f,
                                        0.1f + 0.2f * static_cast<float>(i));
                    break;
                case ChipAction::AskInBrowser:
                    DrawGlyph(surface_, text_, Glyph::OpenInNew, iconX, cy, scale_, iconColor);
                    break;
                case ChipAction::Lens:
                    DrawGlyph(surface_, text_, Glyph::Camera, iconX, cy, scale_, iconColor);
                    break;
                case ChipAction::CopyImage:
                    DrawGlyph(surface_, text_, Glyph::Copy, iconX, cy, scale_, iconColor);
                    break;
                case ChipAction::CopyText:
                    DrawGlyph(surface_, text_, Glyph::Text, iconX, cy, scale_, iconColor);
                    break;
            }
            const RECT label{static_cast<LONG>(l + S(41)), rc.top,
                             static_cast<LONG>(r - S(12)), rc.bottom};
            text_.Draw(surface_, ChipFont(), chip.label, label,
                       DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS,
                       palette_.text);
        }
    }

    void DrawTooltip() {
        std::wstring tooltip;
        switch (hover_) {
            case kHitClose:
                tooltip = L"Close";
                break;
            case kHitGemini:
                tooltip = L"Ask in Gemini";
                break;
            case kHitLens:
                tooltip = L"Search with Google Lens";
                break;
            case kHitTranslate:
                tooltip = L"Translate to " + TranslationLanguage();
                break;
            case kHitCopy:
                tooltip = L"Copy image";
                break;
            case kHitCopyText:
                tooltip = L"Copy text";
                break;
            case kHitAdd:
                tooltip = L"Add another selection";
                break;
        }
        if (tooltip.empty() || !tooltipShown_) {
            return;
        }
        const RECT& target = hover_ == kHitAdd ? addRect_ : buttonRects_[hover_];
        HFONT font = SmallFont();
        const int width =
            text_.Width(font, tooltip.c_str(), static_cast<int>(tooltip.size())) + SI(18);
        const int height = SI(28);
        const int minLeft = SI(4);
        const int maxLeft = std::max(minLeft, width_ - SI(4) - width);
        const int left = std::clamp(
            static_cast<int>((target.left + target.right) / 2 - width / 2), minLeft, maxLeft);
        const int top = target.bottom + SI(6);
        const float l = static_cast<float>(left);
        const float t = static_cast<float>(top);
        const float r = static_cast<float>(left + width);
        const float b = static_cast<float>(top + height);
        DrawShadow(surface_, l, t + S(2), r, b + S(2), S(4), S(4),
                   WithAlpha(kBlack, palette_.dark ? 0.35f : 0.12f));
        FillRoundRect(surface_, l, t, r, b, S(4), palette_.tooltipBg);
        StrokeRoundRect(surface_, l + 0.5f, t + 0.5f, r - 0.5f, b - 0.5f, S(4), 1.0f,
                        palette_.tooltipBorder);
        const RECT rc{left, top, left + width, top + height};
        text_.Draw(surface_, font, tooltip, rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER,
                   palette_.text);
    }

    void Present(double now) {
        if (!open_ || suspended_ || width_ <= 0) {
            return;
        }
        const double eased = EaseOutCubic((now - openTime_) / kAppearMs);
        const int rise = static_cast<int>((1.0 - eased) * S(8));
        RenderBase();
        if (!surface_.Resize(width_, height_)) {
            return;
        }
        for (int y = 0; y < height_; y++) {
            memcpy(surface_.Row(y), chrome_.Row(y), static_cast<size_t>(width_) * 4);
        }

        DrawThumbnails(now);
        DrawButton(kHitClose, Glyph::Close);
        DrawButton(kHitGemini, Glyph::OpenInNew);
        DrawButton(kHitLens, Glyph::Camera);
        DrawButton(kHitTranslate, Glyph::Translate);
        DrawButton(kHitCopy, Glyph::Copy);
        DrawButton(kHitCopyText, Glyph::Text);

        if (!turns_.empty() && convo_.Valid()) {
            DrawConversation(now);
        }
        if (!statusText_.empty()) {
            text_.Draw(surface_, SmallFont(), statusText_, statusRect_, DT_WORDBREAK,
                       statusIsHint_ ? palette_.secondary : palette_.text);
        }
        DrawInput();
        DrawChips(now);
        DrawTooltip();

        panel_.Present(surface_, windowPos_.x, windowPos_.y + rise,
                       static_cast<BYTE>(eased * 255.0 + 0.5));

        const float glow = static_cast<float>(eased) * GlowBreath(now);
        const BYTE underlayAlpha = static_cast<BYTE>(glow * 255.0f + 0.5f);
        if (RenderUnderlay()) {
            panel_.PresentUnderlay(underlay_, underlayMargin_, underlayAlpha);
        } else {
            panel_.SetUnderlayAlpha(underlayAlpha);
        }
    }

    ////////////////////////////////////////////////////////////////////////
    // Window messages

    int HitTest(POINT pt) const {
        for (const HitRect& hit : hits_) {
            if (PtInRect(&hit.rect, pt)) {
                return hit.id;
            }
        }
        return kHitNone;
    }

    void SetHover(int hit, double now) {
        if (hit != hover_) {
            hover_ = hit;
            hoverStart_ = now;
            tooltipShown_ = false;
            Present(now);
        }
    }

    bool HandleMessage(HWND hwnd,
                       UINT message,
                       WPARAM wParam,
                       LPARAM lParam,
                       LRESULT& result) {
        const double now = NowMs();
        switch (message) {
            case WM_NCHITTEST: {
                POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ScreenToClient(hwnd, &pt);
                result = HitTest(pt) != kHitNone ? HTCLIENT : HTCAPTION;
                return true;
            }

            case WM_NCRBUTTONDOWN:
            case WM_NCRBUTTONUP:
            case WM_NCLBUTTONDBLCLK:
                result = 0;
                return true;

            case WM_CONTEXTMENU: {
                POINT screen{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                if (screen.x == -1 && screen.y == -1) {
                    screen = {convoRect_.left + SI(32), convoRect_.top + SI(16)};
                    ClientToScreen(hwnd, &screen);
                }
                POINT pt = screen;
                ScreenToClient(hwnd, &pt);
                if (PtInRect(&convoRect_, pt) && !answerText_.empty()) {
                    SetFocus(hwnd);
                    if (!answerFocused_ || answerAnchor_ == answerCaret_) {
                        SelectAnswerAt(pt, false, true);
                        Present(now);
                    }
                    HMENU menu = CreatePopupMenu();
                    if (menu) {
                        AppendMenuW(menu, MF_STRING, 1, L"Copy\tCtrl+C");
                        AppendMenuW(menu, MF_STRING, 2, L"Select all\tCtrl+A");
                        const UINT action = TrackPopupMenu(menu,
                            TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON,
                            screen.x, screen.y, 0, hwnd, nullptr);
                        DestroyMenu(menu);
                        if (action == 1) {
                            CopyAnswerSelection(now);
                        } else if (action == 2) {
                            answerAnchor_ = 0;
                            answerCaret_ = answerText_.size();
                            Present(now);
                        }
                    }
                }
                result = 0;
                return true;
            }

            case WM_SETCURSOR:
                if (LOWORD(lParam) == HTCLIENT) {
                    POINT pt;
                    GetCursorPos(&pt);
                    ScreenToClient(hwnd, &pt);
                    const int hit = HitTest(pt);
                    PCWSTR cursor = IDC_ARROW;
                    if (hit == kHitInput || (hit == kHitConvo && !answerLines_.empty())) {
                        cursor = IDC_IBEAM;
                    } else if (hit != kHitNone && hit != kHitConvo) {
                        cursor = IDC_HAND;
                    }
                    SetCursor(LoadCursorW(nullptr, cursor));
                    result = TRUE;
                    return true;
                }
                return false;

            case WM_MOUSEMOVE: {
                const POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                if (!trackingMouse_) {
                    TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd, 0};
                    trackingMouse_ = TrackMouseEvent(&track);
                }
                if (selectingAnswer_) {
                    answerDragPoint_ = pt;
                    if (pt.y < convoRect_.top || pt.y >= convoRect_.bottom) {
                        ScrollBy(pt.y < convoRect_.top ? -SI(12) : SI(12));
                    }
                    answerCaret_ = AnswerIndexAt(pt);
                    Present(now);
                    RequestTick();
                } else if (selectingText_) {
                    MoveCaret(InputIndexAt(pt.x), true);
                    Update(now);
                } else {
                    SetHover(HitTest(pt), now);
                }
                result = 0;
                return true;
            }

            case WM_NCMOUSEMOVE:
                SetHover(kHitNone, now);
                return false;

            case WM_MOUSELEAVE:
                trackingMouse_ = false;
                SetHover(kHitNone, now);
                result = 0;
                return true;

            case WM_LBUTTONDOWN:
            case WM_LBUTTONDBLCLK: {
                const POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                SetFocus(hwnd);
                pressed_ = HitTest(pt);
                if (pressed_ == kHitInput) {
                    ClearAnswerSelection();
                    const bool shift = GetKeyState(VK_SHIFT) & 0x8000;
                    MoveCaret(InputIndexAt(pt.x), shift);
                    if (message == WM_LBUTTONDBLCLK && !input_.empty()) {
                        int begin = caret_;
                        int end = caret_;
                        while (begin > 0 && !iswspace(input_[begin - 1])) {
                            begin--;
                        }
                        while (end < static_cast<int>(input_.size()) &&
                               !iswspace(input_[end])) {
                            end++;
                        }
                        anchor_ = begin;
                        caret_ = end;
                    } else {
                        selectingText_ = true;
                        SetCapture(hwnd);
                    }
                    caretStart_ = now;
                    caretOn_ = true;
                    Update(now);
                } else if (pressed_ == kHitConvo && !answerLines_.empty()) {
                    SelectAnswerAt(pt, GetKeyState(VK_SHIFT) & 0x8000,
                                   message == WM_LBUTTONDBLCLK);
                    answerDragPoint_ = pt;
                    selectingAnswer_ = message != WM_LBUTTONDBLCLK;
                    if (selectingAnswer_) {
                        SetCapture(hwnd);
                    }
                    Present(now);
                    RequestTick();
                } else if (pressed_ != kHitNone) {
                    Present(now);
                }
                result = 0;
                return true;
            }

            case WM_LBUTTONUP: {
                const POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                if (selectingAnswer_) {
                    answerCaret_ = AnswerIndexAt(pt);
                    selectingAnswer_ = false;
                    ReleaseCapture();
                    Present(now);
                } else if (selectingText_) {
                    selectingText_ = false;
                    ReleaseCapture();
                } else if (pressed_ != kHitNone && pressed_ != kHitInput &&
                           pressed_ == HitTest(pt)) {
                    const int hit = pressed_;
                    pressed_ = kHitNone;
                    Activate(hit, now);
                }
                if (pressed_ != kHitNone) {
                    pressed_ = kHitNone;
                    Present(now);
                }
                result = 0;
                return true;
            }

            case WM_CAPTURECHANGED:
                selectingText_ = false;
                selectingAnswer_ = false;
                return false;

            case WM_MOUSEWHEEL:
                if (!turns_.empty()) {
                    ScrollBy(-GET_WHEEL_DELTA_WPARAM(wParam) * SI(48) / WHEEL_DELTA);
                    Present(now);
                }
                result = 0;
                return true;

            case WM_KEYDOWN:
                if (HandleKey(wParam, now)) {
                    result = 0;
                    return true;
                }
                return false;

            case WM_CHAR:
                if (wParam >= 0x20 && wParam != 0x7F) {
                    ClearAnswerSelection();
                    InsertText(std::wstring(1, static_cast<wchar_t>(wParam)));
                    caretStart_ = now;
                    caretOn_ = true;
                    Update(now);
                }
                result = 0;
                return true;

            case WM_SETFOCUS:
                focused_ = true;
                caretStart_ = now;
                caretOn_ = true;
                Present(now);
                return false;

            case WM_KILLFOCUS:
                focused_ = false;
                Present(now);
                return false;

            case WM_IME_STARTCOMPOSITION:
                if (HIMC context = ImmGetContext(hwnd)) {
                    COMPOSITIONFORM form{};
                    form.dwStyle = CFS_POINT;
                    form.ptCurrentPos.x = inputText_.left - inputScroll_ + CaretX(caret_);
                    form.ptCurrentPos.y = inputRect_.top + SI(10);
                    ImmSetCompositionWindow(context, &form);
                    ImmReleaseContext(hwnd, context);
                }
                return false;

            case WM_WINDOWPOSCHANGED: {
                const WINDOWPOS* pos = reinterpret_cast<const WINDOWPOS*>(lParam);
                if (!(pos->flags & SWP_NOMOVE) && open_ &&
                    (pos->x != panel_.X() || pos->y != panel_.Y())) {
                    panel_.FollowPanel(pos->x, pos->y);
                    cardPos_ = {pos->x, pos->y};
                    windowPos_ = cardPos_;
                    dragged_ = true;
                    openTime_ = std::min(openTime_, now - kAppearMs - 1);
                    Present(now);
                }
                return false;
            }

            case WM_DPICHANGED:
                scale_ = HIWORD(wParam) / 96.0f;
                chromeKey_ = {};
                underlayKey_ = {};
                convoDirty_ = true;
                Update(now);
                result = 0;
                return true;

            case WM_CLOSE:
                Close();
                result = 0;
                return true;
        }
        return false;
    }

    GlassPanel panel_;
    Surface surface_;
    Surface chrome_;
    Surface convo_;
    Surface underlay_;
    TextPainter text_;
    Palette palette_ = MakePalette(false);
    float scale_ = 1.0f;

    bool open_ = false;
    bool suspended_ = false;
    bool placed_ = false;
    bool dragged_ = false;
    double openTime_ = 0;
    RECT selection_{};
    POINT cardPos_{};
    POINT windowPos_{};

    std::vector<Attachment> attachments_;
    std::vector<CardTurn> turns_;
    std::vector<Chip> chips_;
    bool chipsLoading_ = false;
    double suggestionsStart_ = 0;
    std::shared_ptr<GeminiJob> chatJob_;
    std::shared_ptr<GeminiJob> suggestionsJob_;
    std::shared_ptr<BrowserHandoffJob> browserJob_;
    uint32_t chatId_ = 0;
    uint32_t suggestionsId_ = 0;

    std::wstring input_;
    int caret_ = 0;
    int anchor_ = 0;  // The other end of the text selection.
    int inputScroll_ = 0;
    bool selectingText_ = false;
    std::wstring extentsText_;
    HFONT extentsFont_ = nullptr;
    std::vector<int> extents_;

    std::wstring status_;
    double statusUntil_ = 0;
    std::wstring statusText_;
    bool statusIsHint_ = false;

    int width_ = 0;
    int height_ = 0;
    std::vector<RECT> thumbRects_;
    RECT addRect_{};
    RECT buttonRects_[kHitTranslate + 1] = {};
    RECT convoRect_{};
    RECT statusRect_{};
    RECT inputRect_{};
    RECT inputText_{};
    RECT sendRect_{};
    int dividerY_ = -1;
    std::vector<RECT> chipRects_;
    std::vector<HitRect> hits_;

    PanelKey chromeKey_, underlayKey_;
    int underlayMargin_ = 0;

    std::vector<ConvoItem> convoItems_;
    std::vector<AnswerLine> answerLines_;
    std::wstring answerText_;
    size_t answerAnchor_ = 0;
    size_t answerCaret_ = 0;
    bool answerFocused_ = false;
    bool selectingAnswer_ = false;
    POINT answerDragPoint_{};
    int convoWidth_ = 0;
    int convoLayoutWidth_ = 0;
    int convoCacheTop_ = 0;
    int contentHeight_ = 0;
    bool convoDirty_ = true;
    int scroll_ = 0;
    bool stickToBottom_ = true;

    int hover_ = kHitNone;
    int pressed_ = kHitNone;
    double hoverStart_ = 0;
    bool tooltipShown_ = false;
    bool trackingMouse_ = false;
    bool focused_ = false;
    bool caretOn_ = true;
    double caretStart_ = 0;
};

////////////////////////////////////////////////////////////////////////////////
// The Magic Pointer's state, input and animation

enum class PointerState { Off, Pointing, Pressed, Dragging };

InputOverlay g_overlay;
PointerCursors g_cursors;
HintPill g_pill;
GlowFrame g_frame;
Ripple g_ripple;
AskCard g_card;
ElementPicker g_picker;
ShakeDetector g_shake;

PointerState g_state = PointerState::Off;
HCURSOR g_currentCursor;
bool g_addingToCard;
bool g_releasingCapture;
bool g_escapeRegistered;
bool g_hotkeyRegistered;
POINT g_pressPoint;
double g_lastActivity;
double g_selectedAt;
float g_pointerScale = 1.0f;
HMONITOR g_pointerMonitor;
PickResult g_hover;
bool g_hoverValid;

POINT g_lastRawPoint{LONG_MIN, LONG_MIN};
HMONITOR g_shakeMonitor;
float g_shakeDpiScale = 1.0f;

HANDLE g_timer;
bool g_timerArmed;
double g_timerDue;
double g_lastTick = -1e9;
constexpr double kFrameMs = 1000.0 / 60.0;
constexpr double kHintMs = 4500.0;

float PointerScaleAt(POINT pt) {
    DWORD baseSize = 32;
    DWORD size = sizeof(baseSize);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Cursors",
                     L"CursorBaseSize", RRF_RT_REG_DWORD, nullptr, &baseSize,
                     &size) != ERROR_SUCCESS ||
        baseSize < 32 || baseSize > 256) {
        baseSize = 32;
    }
    const float scale =
        DpiScaleAt(pt) * (baseSize / 32.0f) * g_settings.pointerSize / 100.0f;
    return std::clamp(scale, 0.75f, 3.5f);
}

HCURSOR OverlayCursor() {
    return g_currentCursor ? g_currentCursor : LoadCursorW(nullptr, IDC_ARROW);
}

void UpdatePointerCursor(double now) {
    HCURSOR cursor = g_cursors.Current(now);
    if (cursor != g_currentCursor) {
        g_currentCursor = cursor;
        SetCursor(cursor);
    }
    g_cursors.ReleaseRetired(g_currentCursor);
}

void ArmTimer(double due, double now) {
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = -std::max<LONGLONG>(1, static_cast<LONGLONG>((due - now) * 10000.0));
    SetWaitableTimer(g_timer, &dueTime, 0, nullptr, nullptr, FALSE);
    g_timerArmed = true;
    g_timerDue = due;
}

void RequestTick() {
    const double now = NowMs();
    const double due = std::max(now, g_lastTick + kFrameMs);
    if (g_timerArmed && g_timerDue <= due) {
        return;
    }
    ArmTimer(due, now);
}

bool AnythingActive() {
    return g_state != PointerState::Off || g_card.IsOpen() ||
           g_frame.Visible() || g_pill.Visible() || g_overlay.Visible() ||
           g_ripple.Active();
}

void ArrangeLayers() {
    if (!g_layersChanged) {
        return;
    }
    g_layersChanged = false;
    const HWND order[] = {
        g_overlay.Hwnd(),     g_overlay.DimHwnd(), g_ripple.Hwnd(), g_frame.Hwnd(),
        g_pill.UnderlayHwnd(), g_pill.Hwnd(),       g_card.UnderlayHwnd(),
        g_card.Hwnd(),
    };
    for (HWND hwnd : order) {
        if (hwnd && IsWindowVisible(hwnd)) {
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_NOOWNERZORDER);
        }
    }
}

void ShowHint(POINT pt, double now) {
    if (g_settings.hint) {
        g_pill.Show(pt, DpiScaleAt(pt), UseDarkTheme(), now);
    }
}

void ActivatePointer(POINT pt, double now) {
    if (g_state != PointerState::Off) {
        g_cursors.StartPop(now);
        if (!g_pill.Visible()) {
            ShowHint(pt, now);
        }
        g_lastActivity = now;
        RequestTick();
        return;
    }
    if (g_card.IsOpen() && !g_addingToCard) {
        g_card.Close();
    }
    g_frame.HideNow();
    g_pointerScale = PointerScaleAt(pt);
    g_pointerMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    g_cursors.SetScale(g_pointerScale);
    if (g_settings.ripple) {
        g_ripple.Start(pt, DpiScaleAt(pt), now);
    }
    now = NowMs();
    g_cursors.StartPop(now);
    g_overlay.Show(g_settings.dim);
    g_state = PointerState::Pointing;
    UpdatePointerCursor(now);
    g_escapeRegistered =
        RegisterHotKey(g_messageWindow, kHotkeyEscape, MOD_NOREPEAT, VK_ESCAPE);
    g_lastActivity = now;
    g_hoverValid = false;
    ShowHint(pt, now);
    if (g_settings.highlight && g_picker.Start(g_messageWindow)) {
        g_picker.Request(pt);
    }
    RequestTick();
}

void DeactivatePointer(double now, bool keepFrame) {
    if (g_state == PointerState::Off) {
        return;
    }
    g_state = PointerState::Off;
    if (GetCapture() == g_overlay.Hwnd()) {
        g_releasingCapture = true;
        ReleaseCapture();
        g_releasingCapture = false;
    }
    if (g_escapeRegistered) {
        UnregisterHotKey(g_messageWindow, kHotkeyEscape);
        g_escapeRegistered = false;
    }
    g_pill.FadeOut(now);
    g_ripple.Stop();
    if (!keepFrame) {
        g_frame.FadeOut(now);
    }
    SetCursor(LoadCursorW(nullptr, IDC_ARROW));
    g_overlay.Hide();
    g_currentCursor = nullptr;
    g_cursors.Clear();
    POINT pt;
    if (GetCursorPos(&pt)) {
        SetCursorPos(pt.x, pt.y);
    }
    g_hoverValid = false;
    if (g_addingToCard) {
        g_addingToCard = false;
        g_card.Resume(now);
    }
    RequestTick();
}

void OnCardClosed() {
    if (g_frame.Visible() && g_frame.Style() == FrameStyle::Selected) {
        g_frame.FadeOut(NowMs());
        RequestTick();
    }
}

void BeginAddSelection() {
    if (!g_card.CanAddMore()) {
        return;
    }
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }
    g_card.Suspend();
    g_addingToCard = true;
    ActivatePointer(pt, NowMs());
}

void CompleteSelection(RECT rect, const std::wstring& text, double now) {
    const POINT center = RectCenter(rect);
    const RECT monitor = MonitorRectAt(center, false);
    IntersectRect(&rect, &rect, &monitor);
    const bool adding = g_addingToCard && g_card.IsOpen();
    g_addingToCard = false;
    if (IsRectEmpty(&rect)) {
        DeactivatePointer(now, false);
        if (adding) {
            g_card.Resume(now);
        }
        return;
    }

    g_frame.Conceal();
    g_pill.HideNow();
    g_ripple.Stop();
    DeactivatePointer(now, true);
    WaitForComposition();

    auto image = std::make_shared<Image>();
    if (!CaptureScreen(rect, *image)) {
        Wh_Log(L"Screen capture failed");
        g_frame.HideNow();
        if (adding) {
            g_card.Resume(now);
        }
        return;
    }
    now = NowMs();
    g_frame.Show(rect, FrameStyle::Selected, DpiScaleAt(center), now, false);
    g_selectedAt = now;

    Attachment attachment;
    attachment.image = std::move(image);
    attachment.text = text;
    if (adding) {
        g_card.AddAttachment(std::move(attachment), now);
    } else {
        g_card.Open(std::move(attachment), rect, now);
    }
    RequestTick();
}

RECT DragRect(POINT pt) {
    return {std::min(g_pressPoint.x, pt.x), std::min(g_pressPoint.y, pt.y),
            std::max(g_pressPoint.x, pt.x) + 1, std::max(g_pressPoint.y, pt.y) + 1};
}

void OnOverlayMouse(UINT message, POINT pt) {
    if (g_state == PointerState::Off) {
        return;
    }
    const double now = NowMs();
    g_lastActivity = now;
    switch (message) {
        case WM_MOUSEMOVE: {
            HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            if (monitor != g_pointerMonitor) {
                g_pointerMonitor = monitor;
                const float scale = PointerScaleAt(pt);
                if (fabsf(scale - g_pointerScale) > 0.01f) {
                    g_pointerScale = scale;
                    g_cursors.SetScale(scale);
                    UpdatePointerCursor(now);
                }
            }
            g_pill.Follow(pt);
            if (g_state == PointerState::Pressed) {
                const LONG threshold = static_cast<LONG>(5 * DpiScaleAt(pt));
                if (labs(pt.x - g_pressPoint.x) > threshold ||
                    labs(pt.y - g_pressPoint.y) > threshold) {
                    g_state = PointerState::Dragging;
                    g_ripple.Stop();
                    g_pill.FadeOut(now);
                    g_hoverValid = false;
                }
            }
            if (g_state == PointerState::Dragging) {
                const RECT rect = DragRect(pt);
                g_overlay.SetSelection(rect, now);
                g_frame.Show(rect, FrameStyle::Drag, DpiScaleAt(pt), now, false);
            } else if (g_state == PointerState::Pointing && g_settings.highlight) {
                g_picker.Request(pt);
            }
            break;
        }

        case WM_LBUTTONDOWN:
            if (g_state == PointerState::Pointing) {
                g_state = PointerState::Pressed;
                g_pressPoint = pt;
                SetCapture(g_overlay.Hwnd());
            }
            break;

        case WM_LBUTTONUP: {
            const PointerState state = g_state;
            if (GetCapture() == g_overlay.Hwnd()) {
                g_releasingCapture = true;
                ReleaseCapture();
                g_releasingCapture = false;
            }
            if (state == PointerState::Dragging) {
                const RECT rect = DragRect(pt);
                if (rect.right - rect.left >= 8 && rect.bottom - rect.top >= 8) {
                    CompleteSelection(rect, L"", now);
                } else {
                    g_state = PointerState::Pointing;
                    g_overlay.ClearSelection();
                    g_frame.FadeOut(now);
                }
            } else if (state == PointerState::Pressed) {
                if (g_hoverValid && PtInRect(&g_hover.rect, pt)) {
                    const PickResult hover = g_hover;
                    CompleteSelection(hover.rect, hover.text, now);
                } else {
                    DeactivatePointer(now, false);
                }
            }
            break;
        }

        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            DeactivatePointer(now, false);
            break;

        case WM_CAPTURECHANGED:
            if (!g_releasingCapture && (g_state == PointerState::Pressed ||
                                        g_state == PointerState::Dragging)) {
                g_state = PointerState::Pointing;
                g_overlay.ClearSelection();
                g_frame.FadeOut(now);
            }
            break;
    }
}

void OnPickResult(std::unique_ptr<PickResult> result) {
    if (g_state != PointerState::Pointing) {
        return;
    }
    const double now = NowMs();
    POINT pt;
    GetCursorPos(&pt);
    if (result->found && PtInRect(&result->rect, pt)) {
        RECT rect = result->rect;
        const RECT monitor = MonitorRectAt(pt, false);
        IntersectRect(&rect, &rect, &monitor);
        if (IsRectEmpty(&rect)) {
            return;
        }
        const bool same = g_hoverValid && EqualRect(&rect, &g_hover.rect);
        g_hover = *result;
        g_hover.rect = rect;
        g_hoverValid = true;
        if (!same || !g_frame.Visible()) {
            g_frame.Show(rect, FrameStyle::Hover, DpiScaleAt(pt), now, true);
        }
    } else if (result->id == g_picker.LatestId()) {
        g_hoverValid = false;
        g_frame.FadeOut(now);
    }
}

void OnShake(POINT pt) {
    const double now = NowMs();
    if (g_state == PointerState::Off) {
        if (!CanSummonNow()) {
            return;
        }
        if (ShakeExcludedAt(pt)) {
            Wh_Log(L"Shake ignored: excluded app");
            return;
        }
    }
    Wh_Log(L"Shake at %d,%d", pt.x, pt.y);
    ActivatePointer(pt, now);
}

void OnRawMouseInput() {
    if (!g_settings.shake) {
        return;
    }
    POINT pt;
    if (!GetCursorPos(&pt) ||
        (pt.x == g_lastRawPoint.x && pt.y == g_lastRawPoint.y)) {
        return;  // Clicks, the wheel, and movement that didn't move the cursor.
    }
    g_lastRawPoint = pt;
    if (AnyMouseButtonDown()) {
        g_shake.Clear();
        return;
    }
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (monitor != g_shakeMonitor) {
        g_shakeMonitor = monitor;
        g_shakeDpiScale = GetMonitorDpi(monitor) / 96.0f;
    }
    if (g_shake.AddPoint(NowMs(), pt, g_shakeDpiScale)) {
        g_shake.Clear();
        OnShake(pt);
    }
}

void OnHotkey(int id) {
    const double now = NowMs();
    if (id == kHotkeyEscape) {
        DeactivatePointer(now, false);
        return;
    }
    if (id != kHotkeyActivate) {
        return;
    }
    if (g_state != PointerState::Off) {
        DeactivatePointer(now, false);
        return;
    }
    CURSORINFO cursorInfo{};
    cursorInfo.cbSize = sizeof(cursorInfo);
    if (GetCursorInfo(&cursorInfo) && (cursorInfo.flags & CURSOR_SHOWING)) {
        ActivatePointer(cursorInfo.ptScreenPos, now);
    }
}

void OnDisplayChanged() {
    const double now = NowMs();
    DeactivatePointer(now, false);
    g_frame.HideNow();
}

void RegisterActivationHotkey() {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(g_messageWindow, kHotkeyActivate);
        g_hotkeyRegistered = false;
    }
    if (g_settings.hotkey.empty()) {
        return;
    }
    UINT modifiers;
    UINT key;
    if (!ParseHotkey(g_settings.hotkey, modifiers, key)) {
        Wh_Log(L"Couldn't understand the shortcut \"%s\"", g_settings.hotkey.c_str());
        return;
    }
    g_hotkeyRegistered = RegisterHotKey(g_messageWindow, kHotkeyActivate,
                                        modifiers | MOD_NOREPEAT, key);
    if (!g_hotkeyRegistered) {
        Wh_Log(L"The shortcut %s is taken by another app (error %u)",
               g_settings.hotkey.c_str(), GetLastError());
    }
}

void ApplySettings(const Settings& settings) {
    const double now = NowMs();
    DeactivatePointer(now, false);
    g_ripple.Stop();
    g_settings = settings;
    g_glow = ParseGlowColor(g_settings.glowColor);
    g_styleVersion++;
    g_cursors.Clear();
    g_shake.Configure(g_settings.sensitivity);
    RegisterActivationHotkey();
    g_card.OnSettingsChanged(now);
}

double TickAll(double now) {
    double next = std::min(CleanupGeminiFiles(now), CleanupLensPages(now));
    if (g_state != PointerState::Off) {
        UpdatePointerCursor(now);
        next = std::min(next, g_cursors.NextChange(now));
        if (g_pill.Visible() && now - g_pill.ShownAt() >= kHintMs) {
            g_pill.FadeOut(now);
        } else if (g_pill.Visible()) {
            next = std::min(next, kHintMs - (now - g_pill.ShownAt()));
        }
        if (g_settings.timeoutSec > 0 && g_state == PointerState::Pointing) {
            const double timeout = g_settings.timeoutSec * 1000.0;
            const double idle = now - g_lastActivity;
            if (idle >= timeout) {
                DeactivatePointer(now, false);
            } else {
                next = std::min(next, timeout - idle);
            }
        }
    }

    if (g_state == PointerState::Off && g_frame.Visible() &&
        g_frame.Style() == FrameStyle::Selected) {
        const double age = now - g_selectedAt;
        const bool working = g_card.IsOpen() && g_card.IsWaiting();
        if (age > 1200.0 && (!working || age > 12000.0)) {
            g_frame.FadeOut(now);
        }
    }

    next = std::min(next, g_overlay.Tick(now));
    next = std::min(next, g_ripple.Tick(now));
    next = std::min(next, g_pill.Tick(now));
    next = std::min(next, g_frame.Tick(now));
    next = std::min(next, g_card.Tick(now));
    return next;
}

void OnTimer() {
    g_timerArmed = false;
    const double now = NowMs();
    g_lastTick = now;
    const double next = TickAll(now);
    ArrangeLayers();
    if (next < 1e8) {
        const double finished = NowMs();
        ArmTimer(std::max(now + std::max(next, kFrameMs), finished), finished);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Worker thread

HANDLE g_workerThread;
HANDLE g_workerReadyEvent;

LRESULT CALLBACK LayerWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_MOUSEACTIVATE) {
        return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void Shutdown() {
    const double now = NowMs();
    DeactivatePointer(now, false);
    g_card.Destroy();
    g_frame.Destroy();
    g_ripple.Destroy();
    g_pill.Destroy();
    g_overlay.Destroy();
    g_picker.Stop();
    g_cursors.Clear();
    if (g_hotkeyRegistered) {
        UnregisterHotKey(g_messageWindow, kHotkeyActivate);
        g_hotkeyRegistered = false;
    }
    RAWINPUTDEVICE device{};
    device.usUsagePage = 0x01;  // HID_USAGE_PAGE_GENERIC
    device.usUsage = 0x02;      // HID_USAGE_GENERIC_MOUSE
    device.dwFlags = RIDEV_REMOVE;
    device.hwndTarget = nullptr;
    RegisterRawInputDevices(&device, 1, sizeof(device));
}

LRESULT CALLBACK MessageWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INPUT:
            OnRawMouseInput();
            break;  // DefWindowProc still has to clean up the input.

        case WM_HOTKEY:
            OnHotkey(static_cast<int>(wParam));
            return 0;

        case WM_APP_SETTINGS: {
            std::unique_ptr<Settings> settings(reinterpret_cast<Settings*>(lParam));
            ApplySettings(*settings);
            return 0;
        }

        case WM_APP_PICK:
            OnPickResult(std::unique_ptr<PickResult>(reinterpret_cast<PickResult*>(lParam)));
            return 0;

        case WM_APP_GEMINI:
            g_card.OnGeminiMessage(
                std::unique_ptr<GeminiMessage>(reinterpret_cast<GeminiMessage*>(lParam)),
                NowMs());
            return 0;

        case WM_APP_BROWSER:
            g_card.OnBrowserHandoff(
                std::unique_ptr<BrowserHandoffMessage>(reinterpret_cast<BrowserHandoffMessage*>(lParam)),
                NowMs());
            return 0;

        case WM_APP_TEMP_FILE:
            RequestTick();
            return 0;

        case WM_APP_OCR: {
            std::unique_ptr<OcrResultMessage> result(
                reinterpret_cast<OcrResultMessage*>(lParam));
            g_card.OnOcrResult(result->attachmentId, std::move(result->text), NowMs());
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            Shutdown();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool RegisterWindowClasses() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = g_instance;

    wc.lpfnWndProc = MessageWndProc;
    wc.lpszClassName = kMessageWindowClass;
    if (!RegisterClassExW(&wc)) {
        return false;
    }

    wc.lpfnWndProc = LayerWndProc;
    wc.lpszClassName = kLayerWindowClass;
    if (!RegisterClassExW(&wc)) {
        return false;
    }

    wc.lpfnWndProc = InputOverlay::WndProc;
    wc.lpszClassName = kOverlayWindowClass;
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    if (!RegisterClassExW(&wc)) {
        return false;
    }

    wc.lpfnWndProc = AskCard::WndProc;
    wc.lpszClassName = kCardWindowClass;
    wc.style = CS_DBLCLKS;
    wc.hbrBackground = nullptr;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    return RegisterClassExW(&wc) != 0;
}

void UnregisterWindowClasses() {
    UnregisterClassW(kMessageWindowClass, g_instance);
    UnregisterClassW(kLayerWindowClass, g_instance);
    UnregisterClassW(kOverlayWindowClass, g_instance);
    UnregisterClassW(kCardWindowClass, g_instance);
}

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

DWORD WINAPI WorkerThreadProc(LPVOID) {
    const HRESULT coInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_fonts.Init();
    g_glow = ParseGlowColor(g_settings.glowColor);

    auto fail = [&](PCWSTR what) {
        Wh_Log(L"%s failed: %u", what, GetLastError());
        if (g_messageWindow) {
            DestroyWindow(g_messageWindow);
            g_messageWindow = nullptr;
        }
        UnregisterWindowClasses();
        if (SUCCEEDED(coInit)) {
            CoUninitialize();
        }
        SetEvent(g_workerReadyEvent);
        return 1;
    };

    if (!RegisterWindowClasses()) {
        return fail(L"RegisterClassExW");
    }
    g_messageWindow = CreateWindowExW(0, kMessageWindowClass, L"", 0, 0, 0, 0, 0,
                                      HWND_MESSAGE, nullptr, g_instance, nullptr);
    if (!g_messageWindow) {
        return fail(L"CreateWindowExW");
    }

    g_timer = CreateWaitableTimerExW(nullptr, nullptr,
                                     CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                     TIMER_ALL_ACCESS);
    if (!g_timer) {
        g_timer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
    }
    if (!g_timer) {
        return fail(L"CreateWaitableTimerExW");
    }

    RAWINPUTDEVICE device{};
    device.usUsagePage = 0x01;  // HID_USAGE_PAGE_GENERIC
    device.usUsage = 0x02;      // HID_USAGE_GENERIC_MOUSE
    device.dwFlags = RIDEV_INPUTSINK;
    device.hwndTarget = g_messageWindow;
    if (!RegisterRawInputDevices(&device, 1, sizeof(device))) {
        CloseHandle(g_timer);
        g_timer = nullptr;
        return fail(L"RegisterRawInputDevices");
    }

    if (!g_overlay.Create()) {
        Wh_Log(L"Couldn't create the overlay: %u", GetLastError());
    }
    g_shake.Configure(g_settings.sensitivity);
    RegisterActivationHotkey();
    SetEvent(g_workerReadyEvent);

    bool running = true;
    while (running) {
        const DWORD result = MsgWaitForMultipleObjectsEx(
            1, &g_timer, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0) {
            OnTimer();
            continue;
        }
        if (result != WAIT_OBJECT_0 + 1) {
            Wh_Log(L"MsgWaitForMultipleObjectsEx failed: %u", GetLastError());
            break;
        }
        bool tick = false;
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            if (msg.message != WM_INPUT) {
                tick = true;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (running && tick && AnythingActive()) {
            RequestTick();
        }
        if (running) {
            ArrangeLayers();
        }
    }

    if (g_messageWindow && IsWindow(g_messageWindow)) {
        DestroyWindow(g_messageWindow);  // Only if the loop failed.
    }
    g_messageWindow = nullptr;
    g_fonts.Clear();
    CloseHandle(g_timer);
    g_timer = nullptr;
    UnregisterWindowClasses();
    if (SUCCEEDED(coInit)) {
        CoUninitialize();
    }
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    g_instance = GetModuleHandleW(nullptr);
    g_settings = LoadSettings();

    WCHAR storage[MAX_PATH];
    if (Wh_GetModStoragePath(storage, ARRAYSIZE(storage))) {
        g_storageDir = storage;
    }
    if (!g_storageDir.empty()) {
        CreateDirectoryW(g_storageDir.c_str(), nullptr);
        DeleteExpiredGeminiFiles();
        DeleteOldLensPages(false);
    }

    g_workerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReadyEvent) {
        return FALSE;
    }
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        return FALSE;
    }
    HANDLE handles[] = {g_workerReadyEvent, g_workerThread};
    WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, INFINITE);
    if (g_messageWindow) PostMessageW(g_messageWindow, WM_APP_TEMP_FILE, 0, 0);
    return g_messageWindow != nullptr;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    PostOwned(g_messageWindow, WM_APP_SETTINGS,
              std::make_unique<Settings>(LoadSettings()));
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    g_stopping = true;
    if (g_messageWindow) {
        PostMessageW(g_messageWindow, WM_CLOSE, 0, 0);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    for (int i = 0; i < 40 && g_helperThreads > 0; i++) {
        Sleep(50);
    }
    if (g_helperThreads == 0) {
        CloseGeminiSession();
    }
    DeleteExpiredGeminiFiles(true);
    DeleteOldLensPages(true);
}

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
