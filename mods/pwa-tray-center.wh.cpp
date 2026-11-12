// ==WindhawkMod==
// @id              pwa-tray-center
// @name            PWA Tray Center
// @description     One tray icon for all your Chrome PWAs: hide windows to the PWA Tray Center, control media, see notifications and launch installed apps from a Fluent flyout
// @version         0.5.790
// @author          Proponent
// @github          https://github.com/JJProponent
// @include         chrome.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -luser32 -lgdi32 -ladvapi32 -ldwmapi -ld2d1 -ldwrite -lwindowscodecs -lruntimeobject -ld3d11 -ldxgi -ldcomp
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- simpleMode: false
  $name: Simple mode
  $description: No panel, no shortcut, no media polling. Hidden PWAs go to their own tray icons instead of the PWA Tray Center.
- simpleStyle: separate
  $name: Simple mode tray style
  $description: Works only when Simple mode is on. Separate means each hidden PWA gets its own tray icon (left click shows it, right click gives a menu). Single means one tray icon, right click gives a menu of all PWAs (left click does nothing).
  $options:
  - separate: Separate icon per PWA
  - single: One icon with a menu
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
## PWA Tray Center

Adds a **"Hide to PWA Tray Center"** item to the window menu of installed Chrome PWA windows (right-click title bar).

![Hide to PWA Tray Center in the window menu](https://raw.githubusercontent.com/JJProponent/Image-s-/main/RMB%20Place.jpg)

- Hidden windows keep running (music plays, etc.)
- One tray icon for all PWAs. By default it stays in the tray. With *Always show tray icon* turned off (Settings → Advanced), it appears only while PWAs are hidden and disappears when the last one is restored

![PWA Tray Center flyout](https://raw.githubusercontent.com/JJProponent/Image-s-/main/Main%20app.jpg)

### Tray Icon

![PWA Tray Center tray icon](https://raw.githubusercontent.com/JJProponent/Image-s-/main/Tray%20place.jpg)

- **Left click** → opens PWA Tray Center (Fluent flyout, rounded corners, Mica or solid backdrop, chosen in Settings)
- **Right click** → menu: *Show all* / *Quit all*
- **Center mode** (Settings → Panel): *Hermes (Quick)* closes on a click outside, *Titan (Pinned)* stays open
- **Simple mode** (Windhawk settings): no panel and no shortcut. A hidden PWA gets its own tray icon, or with the *single* style one tray icon lists all PWAs in its right-click menu

### Tabs

| Tab | Description |
|-----|-------------|
| **Media** | Cards for each Chrome media session: cover, title, artist, PWA source, prev/play-pause/next, draggable progress bar. Right-click for controls. Cover style (small, full or none) and compact view (3 covers per row) are set in Settings. |
| **PWAs** | All open PWAs. Hidden ones are bright, visible ones are translucent. Left-click a translucent one to hide it to the tray, left-click a hidden one to show it. Right-click for Show or Hide, and Quit. Badge shows unread counter. |
| **Notifications** | Toast notifications from Windows notification center. Falls back to unread counter from window title (e.g. `(3) Inbox`). |
| **Library** | All installed Chrome PWAs. Left-click to launch/show. Drag to reorder. Right-click for Open/Quit/categories. Search field + category chips for filtering. |
| **Settings** | All mod options (Panel, Media, PWAs, Notifications, Advanced). Two settings are in Windhawk (Simple mode and its tray style); everything else is here. |

> Tabs, cards, and tiles are **drag-to-reorder**; order is remembered.

> **Notifications are not verified yet and may not work.**

### Technical Notes
- Only affects PWA windows (identified by `AppUserModelID` with `_crx_<app id>`)
- Regular browser windows and other programs are untouched
- Library reads from Chrome's Web Applications folders (default or `--user-data-dir`)

### Chrome Volume
Volume bar in Media tab controls Chrome's system-wide volume (same as Windows volume mixer). Dims when no audio session; changes apply when Chrome starts playing. It can be turned on or off in Settings. The level stays as set after the mod is disabled.

### Limitations
- Does not keep PWA alive after browser process exits
- Audio may stop if browser freezes or discards background pages (Memory Saver / Efficiency mode)
*/
// ==/WindhawkModReadme==


#include <windhawk_utils.h>

#include <commctrl.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <audiopolicy.h>
#include <dwrite.h>
#include <mmdeviceapi.h>
#include <propsys.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <wincodec.h>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Notifications.h>

// DXGI / Direct3D / DirectComposition: after WinRT, which must come first.
#include <d2d1_1.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dxgi1_2.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

constexpr wchar_t kChromeWindowClass[] = L"Chrome_WidgetWin_1";
constexpr wchar_t kHostClass[] = L"Windhawk_PwaCenter_Host";
constexpr wchar_t kPanelClass[] = L"Windhawk_PwaCenter_Panel";
// Chromium builds the AppUserModelID of app windows as
// "<BaseAppId>.<app_name>[.<profile>]", and app_name is "_crx_<app id>".
constexpr wchar_t kPwaMarker[] = L"_crx_";
constexpr UINT kTrayIconId = 1;
constexpr int kHotkeyId = 1;  // RegisterHotKey id of the shortcut that opens the panel

// Command id of the item added to the window menu. User-defined window menu
// ids must be below 0xF000 and the low 4 bits are reserved by the system, so
// the id is a multiple of 16 and WM_SYSCOMMAND is compared with 0xFFF0 mask.
constexpr UINT kMenuIdHideToCenter = 0x0B80;

constexpr UINT kMsgListChanged = WM_APP + 1;
constexpr UINT kMsgMouseHotkey = WM_APP + 4;  // mouse button pressed while the shortcut is chosen (wParam vk, lParam mods)
constexpr UINT kMsgHookSync = WM_APP + 5;     // hook thread: make the hooks match the settings
constexpr UINT kMsgWindowHidden = WM_APP + 8;  // a PWA window was hidden: show its note if the center is open
constexpr UINT kMsgPlainClick = WM_APP + 7;   // plain press while a shortcut is chosen (point in wParam/lParam)
constexpr UINT kMsgOutsidePress = WM_APP + 6; // a mouse press seen by a hook (Hermes closes on an outside press)

constexpr UINT_PTR kHostTimer = 1;
constexpr UINT kHostTimerMs = 1000;
constexpr UINT_PTR kTimerData = 1;
constexpr UINT kTimerDataMs = 250;
constexpr UINT_PTR kTimerCaret = 3;
constexpr UINT kTimerCaretMs = 500;  // caret blink half period

enum : UINT {
    kTrayMenuShowAll = 1,
    kTrayMenuQuitAll = 2,
};

// PKEY_AppUserModel_ID, defined by hand to avoid depending on propkey.h/libs.
const PROPERTYKEY kPkeyAppUserModelId = {
    {0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}},
    5};

// GUIDs written by hand, so no extra import libraries are needed.
const GUID kClsidWicFactory = {
    0xcacaf262, 0x9370, 0x4615, {0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a}};
const GUID kIidWicFactory = {
    0xec5ec8a9, 0xc395, 0x4314, {0x9c, 0x77, 0x54, 0xd7, 0xa9, 0x35, 0xff, 0x70}};
const GUID kWicPixelFormat32bppPBGRA = {
    0x6fddc324, 0x4e03, 0x4bfe, {0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10}};
const GUID kIidD2DFactory = {
    0x06152247, 0x6f50, 0x465a, {0x92, 0x45, 0x11, 0x8b, 0xfd, 0x3b, 0x60, 0x07}};
const GUID kIidDWriteFactory = {
    0xb859ee5a, 0xd838, 0x4b5b, {0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48}};

enum class Backdrop { Mica, Solid };
enum class AppDisplay { Both, Icons, Names };
enum class PanelMode { Hermes, Titan };

struct WindowState {
    bool hiddenByUs = false;
    ULONGLONG hiddenTick = 0;
    std::wstring aumid;  // cached, resolved lazily
};

std::mutex g_mutex;
std::unordered_map<HWND, WindowState> g_windows;
std::atomic<int> g_showDiag{0};
std::atomic<int> g_backdrop{(int)Backdrop::Mica};
std::atomic<int> g_appDisplay{(int)AppDisplay::Both};
std::atomic<int> g_showVolume{1};
std::atomic<int> g_mediaCompact{0};
std::atomic<int> g_cardCover{0};  // 1 = the cover fills the media card, 0 = small cover on the left
std::atomic<int> g_panelMode{(int)PanelMode::Titan};
// Shortcut that opens the panel: virtual key in the low 16 bits, MOD_* flags above. 0 = none.
std::atomic<int> g_hotkey{0};

enum class NotifPos { TopLeft, TopRight, BottomLeft, BottomRight, Top, Bottom };
enum class NotifDismiss { Swipe, Click };
std::atomic<int> g_notifPos{(int)NotifPos::BottomRight};
std::atomic<int> g_notifDismiss{(int)NotifDismiss::Swipe};
std::atomic<int> g_showNotes{1};
bool g_simpleMode = false;   // Windhawk setting: no panel, hidden PWAs go to tray icons
bool g_simpleSingle = false;  // Windhawk setting (simple mode only): one icon with a menu
std::atomic<int> g_trayAlways{1};  // 1 = the tray icon stays even with nothing hidden

UINT g_trayMsg = 0;
UINT g_taskbarCreatedMsg = 0;
UINT g_restoreMsg = 0;
UINT g_hideMsg = 0;
UINT g_quitMsg = 0;

ULONGLONG NowMs() {
    return (ULONGLONG)std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

// [pure:begin]
std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = static_cast<wchar_t>(std::towlower(c));
    }
    return s;
}

std::wstring Trim(const std::wstring& s) {
    size_t b = s.find_first_not_of(L" \t");
    if (b == std::wstring::npos) {
        return L"";
    }
    size_t e = s.find_last_not_of(L" \t");
    return s.substr(b, e - b + 1);
}

std::vector<std::wstring> Split(const std::wstring& s, wchar_t sep) {
    std::vector<std::wstring> out;
    size_t start = 0;
    while (true) {
        size_t e = s.find(sep, start);
        if (e == std::wstring::npos) {
            out.push_back(s.substr(start));
            break;
        }
        out.push_back(s.substr(start, e - start));
        start = e + 1;
    }
    return out;
}


// Looks for a leading "(N)" or "[N]" in a window title, the usual way web apps
// show unread counters. Returns the index right after the closing bracket (0 if
// there is no counter) and the number in `count`.
size_t UnreadPrefixEnd(const std::wstring& title, int& count) {
    count = 0;
    size_t i = 0;
    while (i < title.size() && (title[i] == L' ' || title[i] == L'\t')) {
        ++i;
    }
    if (i >= title.size()) {
        return 0;
    }
    wchar_t open = title[i];
    wchar_t close = open == L'(' ? L')' : (open == L'[' ? L']' : 0);
    if (!close) {
        return 0;
    }
    size_t j = i + 1;
    long long n = 0;
    size_t digits = 0;
    while (j < title.size() && title[j] >= L'0' && title[j] <= L'9' &&
           digits < 6) {
        n = n * 10 + (title[j] - L'0');
        ++j;
        ++digits;
    }
    if (digits == 0 || j >= title.size() || title[j] != close) {
        return 0;
    }
    count = (int)n;
    return j + 1;
}

int ParseUnreadFromTitle(const std::wstring& title) {
    int count = 0;
    UnreadPrefixEnd(title, count);
    return count;
}

std::wstring StripUnreadPrefix(const std::wstring& title) {
    int count = 0;
    size_t end = UnreadPrefixEnd(title, count);
    if (!end) {
        return Trim(title);
    }
    return Trim(title.substr(end));
}

// "m:ss", or "h:mm:ss" for an hour and more.
std::wstring FormatTime(long long ms) {
    if (ms < 0) {
        ms = 0;
    }
    long long total = ms / 1000;
    long long h = total / 3600;
    long long m = (total / 60) % 60;
    long long s = total % 60;
    auto two = [](long long v) {
        return (v < 10 ? std::wstring(L"0") : std::wstring()) +
               std::to_wstring(v);
    };
    if (h > 0) {
        return std::to_wstring(h) + L":" + two(m) + L":" + two(s);
    }
    return std::to_wstring(m) + L":" + two(s);
}

// The part of the app id after "_crx_", up to the next dot (lowercase).
std::wstring ExtractCrxId(const std::wstring& aumid) {
    size_t p = ToLower(aumid).find(L"_crx_");
    if (p == std::wstring::npos) {
        return L"";
    }
    size_t b = p + 5;
    size_t e = aumid.find(L'.', b);
    if (e == std::wstring::npos) {
        e = aumid.size();
    }
    return ToLower(aumid.substr(b, e - b));
}

// What follows the app id after a dot, if anything (the profile part).
std::wstring ExtractProfile(const std::wstring& aumid) {
    size_t p = ToLower(aumid).find(L"_crx_");
    if (p == std::wstring::npos) {
        return L"";
    }
    size_t e = aumid.find(L'.', p + 5);
    if (e == std::wstring::npos) {
        return L"";
    }
    return aumid.substr(e + 1);
}

double Clamp01(double v) {
    return v < 0 ? 0 : (v > 1 ? 1 : v);
}

// Value of a CSS style cubic-bezier(x1, y1, x2, y2) at progress x.
double CubicBezierY(double x1, double y1, double x2, double y2, double x) {
    if (x <= 0) {
        return 0;
    }
    if (x >= 1) {
        return 1;
    }
    double lo = 0, hi = 1, s = x;
    for (int i = 0; i < 24; ++i) {
        double mt = 1 - s;
        double bx = 3 * mt * mt * s * x1 + 3 * mt * s * s * x2 + s * s * s;
        if (bx < x) {
            lo = s;
        } else {
            hi = s;
        }
        s = (lo + hi) / 2;
    }
    double mt = 1 - s;
    return 3 * mt * mt * s * y1 + 3 * mt * s * s * y2 + s * s * s;
}

struct Category {
    std::wstring name;
    std::vector<std::wstring> ids;  // lowercase app ids
};

std::wstring SanitizeCategoryName(std::wstring s) {
    for (auto& c : s) {
        if (c == L'\t' || c == L'\n' || c == L'\r') {
            c = L' ';
        }
    }
    s = Trim(s);
    if (s.size() > 40) {
        s.resize(40);
    }
    return Trim(s);
}

std::vector<Category> ParseCategories(const std::wstring& text) {
    std::vector<Category> out;
    // Categories are separated by \x1e (names never contain control characters).
    // Older saves used line breaks, they are read the same way.
    std::wstring norm = text;
    std::replace(norm.begin(), norm.end(), L'\n', L'\x1e');
    for (const auto& line : Split(norm, L'\x1e')) {
        if (line.empty()) {
            continue;
        }
        size_t tab = line.find(L'\t');
        Category c;
        c.name = SanitizeCategoryName(tab == std::wstring::npos
                                          ? line
                                          : line.substr(0, tab));
        if (c.name.empty()) {
            continue;
        }
        if (tab != std::wstring::npos) {
            for (const auto& id : Split(line.substr(tab + 1), L',')) {
                std::wstring t = ToLower(Trim(id));
                if (!t.empty()) {
                    c.ids.push_back(t);
                }
            }
        }
        out.push_back(std::move(c));
    }
    return out;
}

std::wstring SerializeCategories(const std::vector<Category>& cats) {
    std::wstring s;
    for (const auto& c : cats) {
        s += c.name + L"\t";
        for (size_t i = 0; i < c.ids.size(); ++i) {
            s += (i ? L"," : L"") + c.ids[i];
        }
        s += L"\x1e";
    }
    return s;
}

enum { kTabMedia = 0, kTabApps = 1, kTabNotifs = 2, kTabLibrary = 3, kTabSettings = 4, kTabCount = 5 };

// Tab ids: 0 media, 1 apps, 2 notifications, 3 library, 4 settings.
void ParseTabOrder(const std::wstring& s, int order[kTabCount]) {
    bool used[kTabCount] = {};
    int n = 0;
    for (const auto& tok : Split(s, L',')) {
        int id = (tok.size() == 1 && tok[0] >= L'0' && tok[0] < L'0' + kTabCount)
                     ? tok[0] - L'0'
                     : -1;
        if (id >= 0 && !used[id]) {
            used[id] = true;
            order[n++] = id;
        }
    }
    for (int i = 0; i < kTabCount; ++i) {
        if (!used[i]) {
            order[n++] = i;
        }
    }
}

std::wstring SerializeTabOrder(const int order[kTabCount]) {
    std::wstring s;
    for (int i = 0; i < kTabCount; ++i) {
        s += (i ? L"," : L"") + std::to_wstring(order[i]);
    }
    return s;
}
// [pure:end]

// Minimal COM smart pointer (the mod does not depend on WRL).
template <typename T>
struct Cp {
    T* p = nullptr;
    Cp() = default;
    Cp(const Cp&) = delete;
    Cp& operator=(const Cp&) = delete;
    ~Cp() { Reset(); }
    void Reset() {
        if (p) {
            p->Release();
            p = nullptr;
        }
    }
    T** Put() {
        Reset();
        return &p;
    }
    T* operator->() const { return p; }
    explicit operator bool() const { return p != nullptr; }
};

// ---------------------------------------------------------------------------
// Preferences: kept in the mod storage, changed in the Settings tab (and in
// the media card menu). The globals above are what the rest of the mod reads.
// ---------------------------------------------------------------------------

enum PrefId {
    kPrefPanelMode = 0,
    kPrefBackdrop,
    kPrefMediaCompact,
    kPrefCardCover,
    kPrefShowVolume,
    kPrefAppDisplay,
    kPrefNotes,
    kPrefNotifPos,
    kPrefNotifDismiss,
    kPrefTrayAlways,
    kPrefDiag,
    kPrefCount,
};

// Option labels, in the order of the values of their enum.
const wchar_t* const kOptPanelMode[] = {L"Hermes (Quick)", L"Titan (Pinned)"};
const wchar_t* const kOptBackdrop[] = {L"Mica", L"Solid color"};
const wchar_t* const kOptAppDisplay[] = {L"Icon and name", L"Icons only", L"Names only"};
const wchar_t* const kOptNotifPos[] = {L"Top left", L"Top right", L"Bottom left",
                                       L"Bottom right", L"Top center", L"Bottom center"};
const wchar_t* const kOptNotifDismiss[] = {L"Swipe", L"Click"};
const wchar_t* const kOptCardCover[] = {L"Small cover", L"Full cover", L"No cover"};

template <size_t N>
int OptionCount(const wchar_t* const (&)[N]) {
    return (int)N;
}

std::atomic<int>* PrefValue(int id) {
    switch (id) {
        case kPrefPanelMode: return &g_panelMode;
        case kPrefBackdrop: return &g_backdrop;
        case kPrefMediaCompact: return &g_mediaCompact;
        case kPrefCardCover: return &g_cardCover;
        case kPrefShowVolume: return &g_showVolume;
        case kPrefAppDisplay: return &g_appDisplay;
        case kPrefNotes: return &g_showNotes;
        case kPrefNotifPos: return &g_notifPos;
        case kPrefNotifDismiss: return &g_notifDismiss;
        case kPrefTrayAlways: return &g_trayAlways;
        case kPrefDiag: return &g_showDiag;
    }
    return nullptr;
}

PCWSTR PrefKey(int id) {
    switch (id) {
        case kPrefPanelMode: return L"prefPanelMode";
        case kPrefBackdrop: return L"prefBackdrop";
        case kPrefMediaCompact: return L"prefMediaCompact";
        case kPrefCardCover: return L"prefCardCover";
        case kPrefShowVolume: return L"prefShowVolume";
        case kPrefAppDisplay: return L"prefAppDisplay";
        case kPrefNotes: return L"prefShowNotes";
        case kPrefNotifPos: return L"prefNotifPos";
        case kPrefNotifDismiss: return L"prefNotifDismiss";
        case kPrefTrayAlways: return L"prefTrayAlways";
        case kPrefDiag: return L"prefDiagnostics";
    }
    return L"";
}

int PrefDefault(int id) {
    switch (id) {
        case kPrefPanelMode: return (int)PanelMode::Titan;
        case kPrefBackdrop: return (int)Backdrop::Mica;
        case kPrefAppDisplay: return (int)AppDisplay::Both;
        case kPrefNotifPos: return (int)NotifPos::BottomRight;
        case kPrefNotifDismiss: return (int)NotifDismiss::Swipe;
        case kPrefNotes: return 1;
        case kPrefTrayAlways: return 1;
        default: return 0;  // toggles: media compact off, Chrome volume and diagnostics below
    }
}

bool PrefIsToggle(int id) {
    return id == kPrefMediaCompact || id == kPrefShowVolume || id == kPrefNotes ||
           id == kPrefTrayAlways || id == kPrefDiag;
}

int PrefOptionCount(int id) {
    switch (id) {
        case kPrefPanelMode: return OptionCount(kOptPanelMode);
        case kPrefBackdrop: return OptionCount(kOptBackdrop);
        case kPrefAppDisplay: return OptionCount(kOptAppDisplay);
        case kPrefNotifPos: return OptionCount(kOptNotifPos);
        case kPrefNotifDismiss: return OptionCount(kOptNotifDismiss);
        case kPrefCardCover: return OptionCount(kOptCardCover);
        default: return 2;  // toggles
    }
}

void LoadSettings() {
    for (int id = 0; id < kPrefCount; ++id) {
        const int v = Wh_GetIntValue(PrefKey(id), PrefDefault(id));
        PrefValue(id)->store(v >= 0 && v < PrefOptionCount(id) ? v : PrefDefault(id));
    }
}

// Stores a preference. The caller updates what is on screen.
void SetPref(int id, int value) {
    PrefValue(id)->store(value);
    Wh_SetIntValue(PrefKey(id), value);
}

// Mod local storage (shared by all instances of the mod).
std::wstring GetStored(PCWSTR name) {
    std::vector<wchar_t> buf(32768, 0);
    Wh_GetStringValue(name, buf.data(), buf.size());
    buf.back() = 0;
    return std::wstring(buf.data());
}

DWORD GetWindowsBuild() {
    using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
    static DWORD build = 0;
    if (!build) {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        auto fn = ntdll ? (RtlGetVersion_t)GetProcAddress(ntdll, "RtlGetVersion")
                        : nullptr;
        OSVERSIONINFOW vi = {};
        vi.dwOSVersionInfoSize = sizeof(vi);
        if (fn && fn(&vi) == 0) {
            build = vi.dwBuildNumber;
        }
    }
    return build;
}

// ---------------------------------------------------------------------------
// Window helpers
// ---------------------------------------------------------------------------

bool GetWindowAppUserModelId(HWND hWnd, std::wstring& out) {
    using SHGetPropertyStoreForWindow_t =
        HRESULT(WINAPI*)(HWND, REFIID, void**);

    static SHGetPropertyStoreForWindow_t fn = []() {
        HMODULE shell32 = LoadLibraryW(L"shell32.dll");
        return shell32 ? (SHGetPropertyStoreForWindow_t)GetProcAddress(
                             shell32, "SHGetPropertyStoreForWindow")
                       : nullptr;
    }();
    if (!fn) {
        return false;
    }

    IPropertyStore* store = nullptr;
    HRESULT hr = fn(hWnd, __uuidof(IPropertyStore), (void**)&store);
    if (FAILED(hr) || !store) {
        return false;
    }

    PROPVARIANT pv;
    PropVariantInit(&pv);
    out.clear();
    if (SUCCEEDED(store->GetValue(kPkeyAppUserModelId, &pv)) &&
        pv.vt == VT_LPWSTR && pv.pwszVal) {
        out = pv.pwszVal;
    }
    PropVariantClear(&pv);
    store->Release();

    return !out.empty();
}

// Title of a window, read without sending it a message: InternalGetWindowText
// does not wait for the window's thread, so a busy browser cannot block the caller.
// Falls back to a message with a timeout if the export is missing.
std::wstring GetTitleSafe(HWND hWnd) {
    using InternalGetWindowText_t = int(WINAPI*)(HWND, LPWSTR, int);
    static InternalGetWindowText_t internalGetText = []() {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        return user32 ? (InternalGetWindowText_t)GetProcAddress(user32, "InternalGetWindowText")
                      : nullptr;
    }();
    wchar_t buf[512] = {};
    if (internalGetText) {
        internalGetText(hWnd, buf, ARRAYSIZE(buf));
        buf[ARRAYSIZE(buf) - 1] = 0;
        return buf;
    }
    DWORD_PTR res = 0;
    if (!SendMessageTimeoutW(hWnd, WM_GETTEXT, ARRAYSIZE(buf), (LPARAM)buf,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 150, &res)) {
        return L"";
    }
    buf[ARRAYSIZE(buf) - 1] = 0;
    return buf;
}

HICON GetWindowIconSafe(HWND hWnd, bool big) {
    auto ask = [&](WPARAM kind) -> HICON {
        DWORD_PTR res = 0;
        if (SendMessageTimeoutW(hWnd, WM_GETICON, kind, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 150, &res)) {
            return (HICON)res;
        }
        return nullptr;
    };
    HICON icon = nullptr;
    if (big) {
        icon = ask(ICON_BIG);
    }
    if (!icon) {
        icon = ask(ICON_SMALL);
    }
    if (!icon) {
        icon = ask(ICON_BIG);
    }
    if (!icon) {
        icon = (HICON)GetClassLongPtrW(hWnd, big ? GCLP_HICON : GCLP_HICONSM);
    }
    if (!icon) {
        icon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICONSM);
    }
    if (!icon) {
        icon = LoadIconW(nullptr, IDI_APPLICATION);
    }
    return icon;
}

void SetWindowFlags(HWND hWnd,
                    const bool* hiddenByUs,
                    const ULONGLONG* hiddenTick = nullptr) {
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_windows.find(hWnd);
    if (it == g_windows.end()) {
        return;
    }
    if (hiddenByUs) {
        it->second.hiddenByUs = *hiddenByUs;
    }
    if (hiddenTick) {
        it->second.hiddenTick = *hiddenTick;
    }
}

bool IsTracked(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_windows.find(hWnd) != g_windows.end();
}

// AppUserModelID of a tracked window, cached. Chromium sets it after the window
// is created, so an empty result means "not known yet".
std::wstring ResolveAumid(HWND hWnd) {
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_windows.find(hWnd);
        if (it == g_windows.end()) {
            return L"";
        }
        if (!it->second.aumid.empty()) {
            return it->second.aumid;
        }
    }
    std::wstring aumid;
    if (!GetWindowAppUserModelId(hWnd, aumid)) {
        return L"";
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_windows.find(hWnd);
    if (it != g_windows.end()) {
        it->second.aumid = aumid;
    }
    return aumid;
}

// True for AppUserModelIDs of installed Chrome PWAs.
bool IsPwaAumid(const std::wstring& aumid) {
    return ToLower(aumid).find(kPwaMarker) != std::wstring::npos;
}

// True for installed PWA windows. Decided lazily
// (when the menu is opened or the window is closed): Chromium sets the
// AppUserModelID of a window after the window is created, so it can't be
// checked in the creation hook.
bool IsTargetPwaWindow(HWND hWnd) {
    if (!IsTracked(hWnd)) {
        return false;
    }
    std::wstring aumid = ResolveAumid(hWnd);
    if (aumid.empty()) {
        return false;
    }
    return IsPwaAumid(aumid);
}

// Messages for a PWA window. They are handled on the window's own thread.
void RequestRestore(HWND hWnd) {
    PostMessageW(hWnd, g_restoreMsg, 0, 0);
}

void RequestHide(HWND hWnd) {
    PostMessageW(hWnd, g_hideMsg, 0, 0);
}

void RequestQuit(HWND hWnd) {
    PostMessageW(hWnd, g_quitMsg, 0, 0);
}

// ---------------------------------------------------------------------------
// Chrome volume: the audio sessions of this browser in the Windows mixer
// ---------------------------------------------------------------------------

const GUID kClsidMMDevEnum = {
    0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const GUID kIidMMDevEnum = {
    0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const GUID kIidSessionMgr2 = {
    0x77AA99A0, 0x1BD6, 0x484F, {0x8B, 0xC7, 0x2C, 0x65, 0x4C, 0x9A, 0x9B, 0x6F}};
const GUID kIidSessionCtl2 = {
    0xBFB7FF88, 0x7239, 0x4FC9, {0x8F, 0xA2, 0x07, 0xC9, 0x50, 0xBE, 0x9C, 0x6D}};
const GUID kIidSimpleVol = {
    0x87CE5498, 0x68D6, 0x44E5, {0x92, 0x15, 0x6D, 0xA4, 0x7E, 0xF8, 0x83, 0xD8}};

bool SameExeAsSelf(DWORD pid, const wchar_t* self) {
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) {
        return false;
    }
    wchar_t path[MAX_PATH] = {};
    DWORD len = ARRAYSIZE(path);
    bool same = QueryFullProcessImageNameW(h, 0, path, &len) &&
                _wcsicmp(path, self) == 0;
    CloseHandle(h);
    return same;
}

// Calls fn(ISimpleAudioVolume*) for every audio session of the default
// playback device that belongs to this browser (all its processes). Returns the
// number of sessions. Needs COM on the calling thread.
template <typename F>
int ForEachChromeSession(F fn) {
    Cp<IMMDeviceEnumerator> en;
    if (FAILED(CoCreateInstance(kClsidMMDevEnum, nullptr, CLSCTX_ALL,
                                kIidMMDevEnum, (void**)en.Put()))) {
        return 0;
    }
    Cp<IMMDevice> dev;
    if (FAILED(en->GetDefaultAudioEndpoint(eRender, eMultimedia, dev.Put()))) {
        return 0;
    }
    Cp<IAudioSessionManager2> mgr;
    if (FAILED(dev->Activate(kIidSessionMgr2, CLSCTX_ALL, nullptr,
                             (void**)mgr.Put()))) {
        return 0;
    }
    Cp<IAudioSessionEnumerator> list;
    if (FAILED(mgr->GetSessionEnumerator(list.Put()))) {
        return 0;
    }
    int count = 0;
    if (FAILED(list->GetCount(&count))) {
        return 0;
    }
    wchar_t self[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, self, ARRAYSIZE(self));

    int found = 0;
    for (int i = 0; i < count; ++i) {
        Cp<IAudioSessionControl> ctl;
        if (FAILED(list->GetSession(i, ctl.Put()))) {
            continue;
        }
        Cp<IAudioSessionControl2> ctl2;
        if (FAILED(ctl->QueryInterface(kIidSessionCtl2, (void**)ctl2.Put()))) {
            continue;
        }
        DWORD pid = 0;
        if (FAILED(ctl2->GetProcessId(&pid)) || !pid ||
            !SameExeAsSelf(pid, self)) {
            continue;
        }
        Cp<ISimpleAudioVolume> vol;
        if (FAILED(ctl->QueryInterface(kIidSimpleVol, (void**)vol.Put()))) {
            continue;
        }
        fn(vol.p);
        ++found;
    }
    return found;
}

bool ReadChromeVolume(float& level, bool& mute) {
    bool got = false;
    ForEachChromeSession([&](ISimpleAudioVolume* v) {
        if (got) {
            return;
        }
        float l = 1.0f;
        BOOL m = FALSE;
        if (SUCCEEDED(v->GetMasterVolume(&l)) && SUCCEEDED(v->GetMute(&m))) {
            level = l;
            mute = m != FALSE;
            got = true;
        }
    });
    return got;
}

// False when the browser has no audio session right now (nothing played yet).
bool WriteChromeVolume(float level, bool mute) {
    return ForEachChromeSession([&](ISimpleAudioVolume* v) {
               v->SetMasterVolume(level < 0 ? 0 : (level > 1 ? 1 : level),
                                  nullptr);
               v->SetMute(mute ? TRUE : FALSE, nullptr);
           }) > 0;
}

// ---------------------------------------------------------------------------
// Library: installed Chrome PWAs and user categories
// ---------------------------------------------------------------------------

struct LibApp {
    std::wstring crxId;
    std::wstring profileDir;
    std::wstring name;
    std::wstring icoPath;
};

std::vector<LibApp> g_library;  // center thread only
ULONGLONG g_libraryTick = 0;
std::vector<Category> g_categories;  // center thread only

bool IsValidCrxId(const std::wstring& id) {
    if (id.size() != 32) {
        return false;
    }
    for (wchar_t c : id) {
        if (c < L'a' || c > L'p') {
            return false;
        }
    }
    return true;
}

bool EnumNames(const std::wstring& pattern,
               bool wantDirs,
               std::vector<std::wstring>& out) {
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }
    do {
        bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if (isDir != wantDirs || wcscmp(fd.cFileName, L".") == 0 ||
            wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }
        out.push_back(fd.cFileName);
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return true;
}

bool DirExists(const std::wstring& path) {
    DWORD a = GetFileAttributesW(path.c_str());
    return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY);
}

// The value of --user-data-dir= from the command line of this browser, if any.
std::wstring ExplicitUserDataDir() {
    std::wstring cmd = GetCommandLineW();
    const std::wstring key = L"--user-data-dir=";
    size_t p = cmd.find(key);
    if (p == std::wstring::npos) {
        return L"";
    }
    p += key.size();
    if (p < cmd.size() && cmd[p] == L'"') {
        size_t e = cmd.find(L'"', p + 1);
        return cmd.substr(p + 1, e == std::wstring::npos ? std::wstring::npos
                                                          : e - p - 1);
    }
    size_t e = cmd.find(L' ', p);
    return cmd.substr(p, e == std::wstring::npos ? std::wstring::npos : e - p);
}

std::wstring GetUserDataDir() {
    std::wstring dir = ExplicitUserDataDir();
    if (!dir.empty()) {
        return dir;
    }
    wchar_t local[MAX_PATH] = {};
    if (!GetEnvironmentVariableW(L"LOCALAPPDATA", local, MAX_PATH)) {
        return L"";
    }
    for (PCWSTR name : {L"Chrome", L"Chrome Beta", L"Chrome Dev", L"Chrome SxS"}) {
        std::wstring d = std::wstring(local) + L"\\Google\\" + name + L"\\User Data";
        if (DirExists(d)) {
            return d;
        }
    }
    return L"";
}

// Chrome keeps every installed PWA in <profile>\Web Applications\_crx_<id>,
// with an .ico named after the app.
// Identifies an installed app in the saved order. The profile is part of the
// key, so the same app in two profiles keeps two places.
std::wstring LibKey(const LibApp& a) {
    return a.crxId + L"@" + ToLower(a.profileDir);
}

std::vector<std::wstring> ParseLibOrder(const std::wstring& text) {
    std::vector<std::wstring> out;
    for (const auto& t : Split(text, L',')) {
        std::wstring k = Trim(t);
        if (!k.empty()) {
            out.push_back(k);
        }
    }
    return out;
}

// Stores the current order of g_library (the library tab's order).
void SaveLibOrder() {
    std::wstring s;
    for (size_t i = 0; i < g_library.size(); ++i) {
        s += (i ? L"," : L"") + LibKey(g_library[i]);
    }
    Wh_SetStringValue(L"libraryOrder", s.c_str());
}

std::vector<LibApp> ScanLibrary() {
    std::vector<LibApp> out;
    std::wstring root = GetUserDataDir();
    if (root.empty()) {
        return out;
    }
    std::vector<std::wstring> profiles;
    EnumNames(root + L"\\*", true, profiles);
    for (const auto& prof : profiles) {
        std::wstring web = root + L"\\" + prof + L"\\Web Applications";
        std::vector<std::wstring> dirs;
        if (!EnumNames(web + L"\\*", true, dirs)) {
            continue;
        }
        for (const auto& dir : dirs) {
            std::wstring id = ToLower(dir);
            if (id.compare(0, 5, L"_crx_") == 0) {
                id = id.substr(5);
            }
            if (!IsValidCrxId(id)) {
                continue;
            }
            LibApp a;
            a.crxId = id;
            a.profileDir = prof;
            std::vector<std::wstring> icos;
            EnumNames(web + L"\\" + dir + L"\\*.ico", false, icos);
            if (!icos.empty()) {
                a.icoPath = web + L"\\" + dir + L"\\" + icos[0];
                a.name = icos[0].substr(0, icos[0].size() - 4);
            }
            if (a.name.empty()) {
                a.name = id;
            }
            out.push_back(std::move(a));
        }
    }
    for (auto& a : out) {
        int same = 0;
        for (const auto& b : out) {
            if (b.crxId == a.crxId) {
                ++same;
            }
        }
        if (same > 1 && (a.name.empty() || a.name.back() != L')')) {
            a.name += L" (" + a.profileDir + L")";
        }
    }
    std::sort(out.begin(), out.end(), [](const LibApp& x, const LibApp& y) {
        return ToLower(x.name) < ToLower(y.name);
    });
    // Apps the user has moved keep their saved places; new ones stay in the
    // alphabetical order after them.
    const std::vector<std::wstring> order =
        ParseLibOrder(GetStored(L"libraryOrder"));
    auto posOf = [&](const LibApp& a) -> size_t {
        const std::wstring key = LibKey(a);
        auto it = std::find(order.begin(), order.end(), key);
        return it == order.end() ? order.size()
                                 : (size_t)(it - order.begin());
    };
    std::stable_sort(out.begin(), out.end(),
                     [&](const LibApp& x, const LibApp& y) {
                         return posOf(x) < posOf(y);
                     });
    return out;
}

const LibApp* FindLib(const std::wstring& crxId, const std::wstring& profile) {
    const LibApp* any = nullptr;
    for (const auto& l : g_library) {
        if (l.crxId != crxId) {
            continue;
        }
        if (!profile.empty() && ToLower(l.profileDir) == ToLower(profile)) {
            return &l;
        }
        if (!any) {
            any = &l;
        }
    }
    return any;
}

void LaunchApp(const LibApp& a) {
    wchar_t exe[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exe, ARRAYSIZE(exe));
    std::wstring params =
        L"--profile-directory=\"" + a.profileDir + L"\" --app-id=" + a.crxId;
    std::wstring udd = ExplicitUserDataDir();
    if (!udd.empty()) {
        params += L" --user-data-dir=\"" + udd + L"\"";
    }
    ShellExecuteW(nullptr, L"open", exe, params.c_str(), nullptr, SW_SHOWNORMAL);
}

void LoadCategories() {
    g_categories = ParseCategories(GetStored(L"categories"));
}

void SaveCategories() {
    Wh_SetStringValue(L"categories", SerializeCategories(g_categories).c_str());
}

bool InCategory(const Category& c, const std::wstring& crxId) {
    return std::find(c.ids.begin(), c.ids.end(), crxId) != c.ids.end();
}

// ---------------------------------------------------------------------------
// Running PWA windows
// ---------------------------------------------------------------------------

struct AppInfo {
    HWND hwnd = nullptr;
    std::wstring aumid;
    std::wstring crxId;
    std::wstring profile;
    std::wstring title;
    std::wstring name;
    int unread = 0;
    bool hidden = false;
    ULONGLONG hiddenTick = 0;
};

std::wstring AppDisplayName(const AppInfo& a) {
    if (const LibApp* l = FindLib(a.crxId, a.profile)) {
        return l->name;
    }
    std::wstring n = StripUnreadPrefix(a.title);
    return n.empty() ? L"PWA" : n;
}

// Hidden windows, oldest first. A window the browser showed again by itself is
// not hidden any more.
std::vector<HWND> HiddenSnapshot(bool* changed = nullptr) {
    struct S {
        HWND h;
        ULONGLONG t;
    };
    std::vector<S> found;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (const auto& kv : g_windows) {
            if (kv.second.hiddenByUs) {
                found.push_back({kv.first, kv.second.hiddenTick});
            }
        }
    }
    std::vector<S> keep;
    for (const auto& s : found) {
        if (!IsWindow(s.h)) {
            continue;
        }
        if (IsWindowVisible(s.h)) {
            const bool no = false;
            SetWindowFlags(s.h, &no);
            if (changed) {
                *changed = true;
            }
            continue;
        }
        keep.push_back(s);
    }
    std::sort(keep.begin(), keep.end(),
              [](const S& a, const S& b) { return a.t < b.t; });
    std::vector<HWND> out;
    for (const auto& s : keep) {
        out.push_back(s.h);
    }
    return out;
}

// The PWA windows as they are now. Runs on the worker thread: no names and no
// order here, TakeWindowSnapshot adds them on the panel side.
std::vector<AppInfo> CollectWindowsRaw() {
    struct Snap {
        HWND h;
        bool hidden;
        ULONGLONG tick;
    };
    std::vector<Snap> snaps;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (const auto& kv : g_windows) {
            snaps.push_back({kv.first, kv.second.hiddenByUs, kv.second.hiddenTick});
        }
    }
    std::vector<AppInfo> out;
    for (const auto& s : snaps) {
        if (!IsWindow(s.h)) {
            continue;
        }
        bool hidden = s.hidden;
        if (hidden && IsWindowVisible(s.h)) {
            const bool no = false;
            SetWindowFlags(s.h, &no);
            hidden = false;
        }
        std::wstring aumid = ResolveAumid(s.h);
        if (aumid.empty()) {
            continue;
        }
        if (!IsPwaAumid(aumid)) {
            continue;
        }
        std::wstring title = GetTitleSafe(s.h);
        AppInfo a;
        a.hwnd = s.h;
        a.aumid = aumid;
        a.crxId = ExtractCrxId(aumid);
        a.profile = ExtractProfile(aumid);
        a.title = title;
        a.unread = ParseUnreadFromTitle(title);
        a.hidden = hidden;
        a.hiddenTick = s.tick;
        out.push_back(std::move(a));
    }
    return out;
}

// ---------------------------------------------------------------------------
// Worker thread: media sessions (SMTC) and toast notifications
// ---------------------------------------------------------------------------

using winrt::Windows::Foundation::AsyncStatus;
using winrt::Windows::Media::Control::
    GlobalSystemMediaTransportControlsSession;
using winrt::Windows::Media::Control::
    GlobalSystemMediaTransportControlsSessionManager;
using winrt::Windows::Media::Control::
    GlobalSystemMediaTransportControlsSessionPlaybackStatus;

constexpr int kCoverPx = 320;

struct MediaInfo {
    std::wstring key;         // "<source aumid>#<ordinal>"
    std::wstring matchedCrx;  // app id of the PWA the session belongs to, if any
    bool playing = false;
    bool canPrev = false;
    bool canNext = false;
    bool canToggle = false;
    bool canSeek = false;
    std::wstring title;
    std::wstring artist;
    long long posMs = 0;
    long long durMs = 0;
    ULONGLONG posTick = 0;  // GetTickCount64() at the moment posMs was valid
    uint64_t coverId = 0;
    std::shared_ptr<const std::vector<uint8_t>> cover;  // kCoverPx^2*4 PBGRA
};

struct ToastInfo {
    std::wstring aumid;
    std::wstring title;
    std::wstring body;
};

enum class MediaCmd { Toggle, Next, Prev, Seek };

struct MediaCommand {
    MediaCmd cmd;
    std::wstring key;
    long long arg;  // milliseconds for Seek
};

std::mutex g_workMutex;
std::vector<MediaInfo> g_mediaList;
std::vector<ToastInfo> g_toastList;
std::vector<AppInfo> g_windowSnap;  // PWA windows, raw (no names yet)
bool g_volKnown = false;            // Chrome has an audio session
float g_volLevel = 1.0f;            // its volume, 0 to 1
bool g_volMute = false;
std::vector<std::wstring> g_knownCrx;     // lowercase app ids of known PWAs
std::vector<std::wstring> g_knownAumids;  // AppUserModelIDs of known PWAs
std::vector<MediaCommand> g_commands;

std::mutex g_workThreadMutex;
HANDLE g_workThread = nullptr;
HANDLE g_workWake = nullptr;
std::atomic<bool> g_workStop{false};
std::atomic<bool> g_stopping{false};  // set when the mod unloads: no thread is started after this
std::vector<std::pair<HWND, ULONGLONG>> g_pendingHidden;  // hidden windows waiting for a note (window, tick); guarded by g_mutex
std::atomic<ULONGLONG> g_pollUntil{0};

template <typename Op>
bool WaitFor(Op const& op, int ms = 3000) {
    // wait_for sets the Completed handler, which an operation takes only once,
    // so the status is polled instead. A stop request (unload) ends the wait.
    const ULONGLONG until = GetTickCount64() + ms;
    while (op.Status() == AsyncStatus::Started) {
        if (g_workStop || GetTickCount64() >= until) {
            op.Cancel();
            return false;
        }
        Sleep(20);
    }
    return op.Status() == AsyncStatus::Completed;
}

std::wstring ToWide(winrt::hstring const& h) {
    return std::wstring(h.c_str());
}

// Decodes an image into a kCoverPx x kCoverPx center-cropped bitmap.
bool DecodeCover(const std::vector<uint8_t>& bytes, std::vector<uint8_t>& out) {
    Cp<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(kClsidWicFactory, nullptr, CLSCTX_INPROC_SERVER,
                                kIidWicFactory, (void**)factory.Put()))) {
        return false;
    }

    Cp<IWICStream> stream;
    if (FAILED(factory->CreateStream(stream.Put())) ||
        FAILED(stream->InitializeFromMemory((BYTE*)bytes.data(),
                                            (DWORD)bytes.size()))) {
        return false;
    }

    Cp<IWICBitmapDecoder> decoder;
    if (FAILED(factory->CreateDecoderFromStream(
            stream.p, nullptr, WICDecodeMetadataCacheOnDemand,
            decoder.Put()))) {
        return false;
    }

    Cp<IWICBitmapFrameDecode> frame;
    if (FAILED(decoder->GetFrame(0, frame.Put()))) {
        return false;
    }

    UINT w = 0, h = 0;
    if (FAILED(frame->GetSize(&w, &h)) || !w || !h) {
        return false;
    }

    Cp<IWICFormatConverter> converter;
    if (FAILED(factory->CreateFormatConverter(converter.Put())) ||
        FAILED(converter->Initialize(frame.p, kWicPixelFormat32bppPBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0.0,
                                     WICBitmapPaletteTypeCustom))) {
        return false;
    }

    double scale = (std::max)((double)kCoverPx / w, (double)kCoverPx / h);
    UINT sw = (std::max)((UINT)kCoverPx, (UINT)std::ceil(w * scale));
    UINT sh = (std::max)((UINT)kCoverPx, (UINT)std::ceil(h * scale));

    Cp<IWICBitmapScaler> scaler;
    if (FAILED(factory->CreateBitmapScaler(scaler.Put())) ||
        FAILED(scaler->Initialize(converter.p, sw, sh,
                                  WICBitmapInterpolationModeFant))) {
        return false;
    }

    WICRect rc = {(INT)((sw - kCoverPx) / 2), (INT)((sh - kCoverPx) / 2),
                  kCoverPx, kCoverPx};
    out.assign((size_t)kCoverPx * kCoverPx * 4, 0);
    if (FAILED(scaler->CopyPixels(&rc, kCoverPx * 4, (UINT)out.size(),
                                  out.data()))) {
        out.clear();
        return false;
    }
    return true;
}

bool LoadCover(
    winrt::Windows::Storage::Streams::IRandomAccessStreamReference const& ref,
    std::vector<uint8_t>& out) {
    auto open = ref.OpenReadAsync();
    if (!WaitFor(open)) {
        return false;
    }
    auto stream = open.GetResults();
    uint64_t size = stream.Size();
    if (size == 0 || size > (16u << 20)) {
        return false;
    }

    winrt::Windows::Storage::Streams::DataReader reader(
        stream.GetInputStreamAt(0));
    auto load = reader.LoadAsync((uint32_t)size);
    if (!WaitFor(load)) {
        return false;
    }
    load.GetResults();

    std::vector<uint8_t> bytes((size_t)size);
    reader.ReadBytes(winrt::array_view<uint8_t>(bytes));
    return DecodeCover(bytes, out);
}

struct CoverState {
    std::wstring coverKey;
    int attempts = 0;
    uint64_t id = 0;
    std::shared_ptr<const std::vector<uint8_t>> pixels;
};

// Lives on the worker thread only, so WinRT objects are released before the
// apartment is uninitialized.
struct Worker {
    GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
    std::unordered_map<std::wstring, GlobalSystemMediaTransportControlsSession>
        live;
    std::unordered_map<std::wstring, CoverState> covers;
    uint64_t coverCounter = 0;
    ULONGLONG lastToast = 0;

    bool EnsureManager() {
        if (manager) {
            return true;
        }
        auto op = GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
        if (!WaitFor(op)) {
            return false;
        }
        manager = op.GetResults();
        return (bool)manager;
    }

    static void Publish(std::vector<MediaInfo>&& list) {
        std::lock_guard<std::mutex> lock(g_workMutex);
        g_mediaList = std::move(list);
    }

    // Collects every media session that belongs to Chrome: either the session
    // id contains the app id of a known PWA, or it mentions "chrome".
    void PollMedia(const std::vector<std::wstring>& knownCrx) {
        std::vector<MediaInfo> list;
        try {
            if (!EnsureManager()) {
                Publish({});
                return;
            }

            auto sessions = manager.GetSessions();
            std::unordered_map<std::wstring, int> ordinals;
            std::unordered_map<std::wstring,
                               GlobalSystemMediaTransportControlsSession>
                newLive;

            for (auto const& s : sessions) {
                if (g_workStop) {
                    break;
                }
                std::wstring id = ToWide(s.SourceAppUserModelId());
                std::wstring lower = ToLower(id);
                std::wstring crx;
                for (const auto& c : knownCrx) {
                    if (!c.empty() && lower.find(c) != std::wstring::npos) {
                        crx = c;
                        break;
                    }
                }
                bool chromeLike = lower.find(L"chrome") != std::wstring::npos;
                if (crx.empty() && !chromeLike) {
                    continue;
                }

                auto playbackInfo = s.GetPlaybackInfo();
                auto status = playbackInfo.PlaybackStatus();
                bool playing =
                    status == GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                                  Playing;
                bool paused =
                    status == GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                                  Paused;
                if (!playing && !paused) {
                    continue;
                }

                auto propsOp = s.TryGetMediaPropertiesAsync();
                if (!WaitFor(propsOp)) {
                    continue;
                }
                auto props = propsOp.GetResults();
                std::wstring title = ToWide(props.Title());
                if (title.empty()) {
                    continue;
                }
                std::wstring artist = ToWide(props.Artist());
                if (artist.empty()) {
                    artist = ToWide(props.AlbumArtist());
                }

                MediaInfo m;
                int ord = ordinals[id]++;
                m.key = id + L"#" + std::to_wstring(ord);
                m.matchedCrx = crx;
                m.playing = playing;
                m.title = title;
                m.artist = artist;

                auto controls = playbackInfo.Controls();
                m.canPrev = controls.IsPreviousEnabled();
                m.canNext = controls.IsNextEnabled();
                m.canToggle =
                    controls.IsPlayEnabled() || controls.IsPauseEnabled();
                m.canSeek = controls.IsPlaybackPositionEnabled();

                auto timeline = s.GetTimelineProperties();
                using std::chrono::duration_cast;
                using std::chrono::milliseconds;
                long long startMs =
                    duration_cast<milliseconds>(timeline.StartTime()).count();
                long long endMs =
                    duration_cast<milliseconds>(timeline.EndTime()).count();
                long long posMs =
                    duration_cast<milliseconds>(timeline.Position()).count() -
                    startMs;
                long long durMs = endMs - startMs;
                if (playing) {
                    long long age =
                        duration_cast<milliseconds>(
                            winrt::clock::now() - timeline.LastUpdatedTime())
                            .count();
                    if (age > 0 && age < 12LL * 3600 * 1000) {
                        posMs += age;
                    }
                }
                if (durMs < 0) {
                    durMs = 0;
                }
                if (posMs < 0) {
                    posMs = 0;
                }
                if (durMs > 0 && posMs > durMs) {
                    posMs = durMs;
                }
                m.posMs = posMs;
                m.durMs = durMs;
                m.posTick = GetTickCount64();
                if (durMs == 0) {
                    m.canSeek = false;
                }

                CoverState& cs = covers[m.key];
                std::wstring ck = title + L"\x1f" + artist;
                if (ck != cs.coverKey) {
                    cs.coverKey = ck;
                    cs.attempts = 0;
                    cs.pixels.reset();
                    cs.id = ++coverCounter;
                }
                if (!cs.pixels && cs.attempts < 6) {
                    auto thumb = props.Thumbnail();
                    if (thumb) {
                        ++cs.attempts;
                        std::vector<uint8_t> px;
                        if (LoadCover(thumb, px)) {
                            cs.pixels = std::make_shared<
                                const std::vector<uint8_t>>(std::move(px));
                            cs.id = ++coverCounter;
                        }
                    }
                }
                m.coverId = cs.id;
                m.cover = cs.pixels;

                newLive.insert_or_assign(m.key, s);
                list.push_back(std::move(m));
            }

            for (auto it = covers.begin(); it != covers.end();) {
                if (newLive.find(it->first) == newLive.end()) {
                    it = covers.erase(it);
                } else {
                    ++it;
                }
            }
            live = std::move(newLive);

        } catch (winrt::hresult_error const&) {
            manager = nullptr;
            live.clear();
            list.clear();
        } catch (...) {
            manager = nullptr;
            live.clear();
            list.clear();
        }
        Publish(std::move(list));
    }

    // What Chrome put into the Windows notification center, per PWA.
    void PollToasts(const std::vector<std::wstring>& aumids) {
        std::vector<ToastInfo> out;
        for (const auto& a : aumids) {
            try {
                auto history =
                    winrt::Windows::UI::Notifications::ToastNotificationManager::
                        History();
                auto list = history.GetHistory(winrt::hstring(a));
                int n = 0;
                for (auto const& toast : list) {
                    if (n >= 6) {
                        break;
                    }
                    auto doc = toast.Content();
                    auto texts = doc.GetElementsByTagName(L"text");
                    std::wstring title, body;
                    for (uint32_t i = 0; i < texts.Length(); ++i) {
                        std::wstring s = ToWide(texts.Item(i).InnerText());
                        if (s.empty()) {
                            continue;
                        }
                        if (title.empty()) {
                            title = s;
                        } else if (body.empty()) {
                            body = s;
                        }
                    }
                    if (title.empty() && body.empty()) {
                        continue;
                    }
                    out.push_back({a, title, body});
                    ++n;
                }
            } catch (winrt::hresult_error const&) {
            } catch (...) {
            }
        }
        std::lock_guard<std::mutex> lock(g_workMutex);
        g_toastList = std::move(out);
    }

    void Execute(const MediaCommand& c) {
        auto it = live.find(c.key);
        if (it == live.end()) {
            return;
        }
        auto session = it->second;
        try {
            switch (c.cmd) {
                case MediaCmd::Toggle: {
                    auto op = session.TryTogglePlayPauseAsync();
                    WaitFor(op);
                    break;
                }
                case MediaCmd::Next: {
                    auto op = session.TrySkipNextAsync();
                    WaitFor(op);
                    break;
                }
                case MediaCmd::Prev: {
                    auto op = session.TrySkipPreviousAsync();
                    WaitFor(op);
                    break;
                }
                case MediaCmd::Seek: {
                    long long startTicks =
                        session.GetTimelineProperties().StartTime().count();
                    auto op = session.TryChangePlaybackPositionAsync(
                        startTicks + c.arg * 10000);
                    WaitFor(op);
                    break;
                }
            }
        } catch (...) {
        }
    }
};

void CollectWindowsSnapshot() {
    std::vector<AppInfo> raw = CollectWindowsRaw();
    std::lock_guard<std::mutex> lock(g_workMutex);
    g_windowSnap = std::move(raw);
}

void SetVolumeSnapshot(float level, bool mute) {
    std::lock_guard<std::mutex> lock(g_workMutex);
    g_volKnown = true;
    g_volLevel = level;
    g_volMute = mute;
}

// Returns false when Chrome has no audio session.
bool GetVolumeSnapshot(float& level, bool& mute) {
    std::lock_guard<std::mutex> lock(g_workMutex);
    level = g_volLevel;
    mute = g_volMute;
    return g_volKnown;
}

void CollectVolumeSnapshot() {
    float level = 1.0f;
    bool mute = false;
    if (ReadChromeVolume(level, mute)) {
        SetVolumeSnapshot(level, mute);
    } else {
        std::lock_guard<std::mutex> lock(g_workMutex);
        g_volKnown = false;
    }
}

DWORD WINAPI WorkThreadProc(LPVOID) {
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (...) {
    }

    {
        Worker worker;
        while (!g_workStop) {
            bool active = GetTickCount64() < g_pollUntil;
            WaitForSingleObject(g_workWake, active ? 250 : INFINITE);
            if (g_workStop) {
                break;
            }

            std::vector<MediaCommand> commands;
            std::vector<std::wstring> crx;
            std::vector<std::wstring> aumids;
            {
                std::lock_guard<std::mutex> lock(g_workMutex);
                commands.swap(g_commands);
                crx = g_knownCrx;
                aumids = g_knownAumids;
            }

            for (const auto& c : commands) {
                if (g_workStop) {
                    break;
                }
                worker.Execute(c);
            }
            if (!commands.empty()) {
                // Give the page a moment to publish the new state.
                Sleep(120);
            }

            if (GetTickCount64() < g_pollUntil || !commands.empty()) {
                CollectWindowsSnapshot();
                CollectVolumeSnapshot();
                worker.PollMedia(crx);
                ULONGLONG now = GetTickCount64();
                if (now - worker.lastToast >= 1500) {
                    worker.lastToast = now;
                    worker.PollToasts(aumids);
                }
            }
        }
    }

    try {
        winrt::uninit_apartment();
    } catch (...) {
    }
    return 0;
}

void EnsureWorkThread() {
    std::lock_guard<std::mutex> lock(g_workThreadMutex);
    if (g_stopping) {
        return;
    }
    if (g_workThread) {
        return;
    }
    g_workStop = false;
    g_workWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_workWake) {
        return;
    }
    g_workThread = CreateThread(nullptr, 0, WorkThreadProc, nullptr, 0, nullptr);
    if (!g_workThread) {
        CloseHandle(g_workWake);
        g_workWake = nullptr;
    }
}

void StopWorkThread() {
    std::lock_guard<std::mutex> lock(g_workThreadMutex);
    if (!g_workThread) {
        return;
    }
    g_workStop = true;
    SetEvent(g_workWake);
    WaitForSingleObject(g_workThread, INFINITE);
    CloseHandle(g_workThread);
    CloseHandle(g_workWake);
    g_workThread = nullptr;
    g_workWake = nullptr;
}

// Keeps the worker polling for the next `ms` milliseconds. `wake` makes it
// poll right now instead of at its next tick.
void RequestMediaPoll(unsigned ms, bool wake = true) {
    ULONGLONG until = GetTickCount64() + ms;
    ULONGLONG cur = g_pollUntil;
    while (cur < until && !g_pollUntil.compare_exchange_weak(cur, until)) {
    }
    EnsureWorkThread();
    // StopWorkThread closes the event under this mutex, so it cannot be closed meanwhile.
    std::lock_guard<std::mutex> lock(g_workThreadMutex);
    if (wake && g_workWake) {
        SetEvent(g_workWake);
    }
}

void SendMediaCommand(MediaCmd cmd, const std::wstring& key, long long arg) {
    {
        std::lock_guard<std::mutex> lock(g_workMutex);
        g_commands.push_back({cmd, key, arg});
    }
    RequestMediaPoll(3000);
}

void SetKnownApps(const std::vector<AppInfo>& apps) {
    std::vector<std::wstring> crx;
    std::vector<std::wstring> aumids;
    for (const auto& a : apps) {
        if (!a.crxId.empty() &&
            std::find(crx.begin(), crx.end(), a.crxId) == crx.end()) {
            crx.push_back(a.crxId);
        }
        aumids.push_back(a.aumid);
    }
    // Installed apps too, so a session of a not yet listed app still matches.
    for (const auto& l : g_library) {
        if (std::find(crx.begin(), crx.end(), l.crxId) == crx.end()) {
            crx.push_back(l.crxId);
        }
    }
    std::lock_guard<std::mutex> lock(g_workMutex);
    g_knownCrx = std::move(crx);
    g_knownAumids = std::move(aumids);
}

void GetWorkSnapshot(std::vector<MediaInfo>& media,
                     std::vector<ToastInfo>& toasts) {
    std::lock_guard<std::mutex> lock(g_workMutex);
    media = g_mediaList;
    toasts = g_toastList;
}

// ---------------------------------------------------------------------------
// Animation
// ---------------------------------------------------------------------------

enum Ease { kEaseLinear = 0, kEaseOut = 1, kEaseIn = 2 };

// Fluent motion: "fast out, slow in" is cubic-bezier(0,0,0,1) for things that
// enter. Everything else moves linearly.
float ApplyEase(int ease, float x) {
    switch (ease) {
        case kEaseOut:
            return (float)CubicBezierY(0, 0, 0, 1, x);
        case kEaseIn:
            return (float)CubicBezierY(1, 0, 1, 1, x);
        default:
            return x;
    }
}

struct Tween {
    float v = 0;
    float from = 0;
    float to = 0;
    ULONGLONG t0 = 0;
    int dur = 0;
    int ease = kEaseLinear;
};

void TweenJump(Tween& t, float v) {
    t.v = t.from = t.to = v;
    t.dur = 0;
}

void TweenSet(Tween& t, float to, int dur, int ease) {
    t.from = t.v;
    t.to = to;
    t.t0 = NowMs();
    t.dur = dur;
    t.ease = ease;
    if (dur <= 0) {
        t.v = to;
    }
}

// Returns true while the tween is still running.
bool TweenStep(Tween& t, ULONGLONG now) {
    if (t.dur <= 0) {
        t.v = t.to;
        return false;
    }
    float x = (float)(now - t.t0) / (float)t.dur;
    if (x >= 1.0f) {
        t.v = t.to;
        t.dur = 0;
        return false;
    }
    t.v = t.from + (t.to - t.from) * ApplyEase(t.ease, x < 0 ? 0 : x);
    return true;
}

// ---------------------------------------------------------------------------
// Panel: a Fluent style flyout drawn with Direct2D
// ---------------------------------------------------------------------------

// Segoe Fluent Icons glyphs.
constexpr wchar_t kGlyphPlay[] = L"\xE768";
constexpr wchar_t kGlyphPause[] = L"\xE769";
constexpr wchar_t kGlyphPrev[] = L"\xE892";
constexpr wchar_t kGlyphNext[] = L"\xE893";
constexpr wchar_t kGlyphOpen[] = L"\xE8A7";
constexpr wchar_t kGlyphAdd[] = L"\xE710";
constexpr wchar_t kGlyphDelete[] = L"\xE74D";
constexpr wchar_t kGlyphRename[] = L"\xE8AC";
constexpr wchar_t kGlyphCheck[] = L"\xE73E";
constexpr wchar_t kGlyphAllApps[] = L"\xE9A4";
constexpr wchar_t kGlyphChevL[] = L"\xE76B";
constexpr wchar_t kGlyphChevR[] = L"\xE76C";
constexpr wchar_t kGlyphChevDown[] = L"\xE70D";
constexpr wchar_t kGlyphFolder[] = L"\xE8B7";
constexpr wchar_t kGlyphClose[] = L"\xE8BB";  // ChromeClose for title bar
constexpr wchar_t kGlyphVolMute[] = L"\xE74F";
constexpr wchar_t kGlyphVol1[] = L"\xE993";
constexpr wchar_t kGlyphVol2[] = L"\xE994";
constexpr wchar_t kGlyphVol3[] = L"\xE995";

// Custom title bar
constexpr float kTitleBarH = 40.0f;
constexpr float kCloseButtonW = 46.0f;
constexpr wchar_t kAppVersion[] = L"v" WH_MOD_VERSION;

constexpr float kPanelW = 400.0f, kPanelH = 480.0f;  // 440 content + 40 title bar
constexpr float kPad = 12.0f;
constexpr float kRadiusCtl = 4.0f;      // buttons, list items, cards
constexpr float kRadiusOverlay = 8.0f;  // flyouts, menus, dialogs
constexpr float kMenuW = 232.0f;        // context menu width
constexpr float kMenuItemH = 36.0f;     // Windows 11 style menu row
constexpr float kMenuSepH = 9.0f;
constexpr float kMenuPad = 4.0f;
constexpr float kTabsY = kTitleBarH + 4.0f;  // Tabs right after title bar with small gap
constexpr float kTabsH = 52.0f;
constexpr float kContentTop = kTabsY + kTabsH + 8.0f;  // Content starts after tabs
constexpr float kContentBottom = kPanelH - kPad;
constexpr float kDiagH = 150.0f;
constexpr float kVolBarH = 48.0f;  // Chrome volume bar of the Media tab

// Library search field, pinned to the bottom edge of the panel (same kind of field as the category dialog).
constexpr float kSearchH = 30.0f;
constexpr float kSearchBottomGap = 6.0f;
constexpr float kSearchTop = kPanelH - kSearchBottomGap - kSearchH;  // 444
constexpr float kLibGridBottom = kSearchTop - 8.0f;                  // the tiles stop above the field


struct TabDef {
    PCWSTR label;
    PCWSTR glyph;
    PCWSTR empty;
};

const TabDef kTabDefs[kTabCount] = {
    {L"Media", L"\xE90B", L"No media playing"},
    {L"PWAs", L"\xE8F9", L"No PWA windows"},
    {L"Notifications", L"\xE8BD", L"No notifications"},
    {L"Library", L"\xE71D", L"No installed apps found"},
    {L"Settings", L"\xE713", L""},
};

enum HitKind {
    kHitNone = 0,
    kHitTab = 1,
    kHitTile,
    kHitRow,
    kHitMediaPrev,
    kHitMediaPlay,
    kHitMediaNext,
    kHitMediaSeek,
    kHitChip,
    kHitMenuItem,
    kHitDlgOk,
    kHitDlgCancel,
    kHitMediaCard,  // whole media card (right click target)
    kHitVolSeek,    // Chrome volume slider
    kHitVolMute,    // Chrome mute button
    kHitHiddenBadge,  // Hidden apps badge in title bar
    kHitNote,         // in-panel notification card
    kHitTileMedia,    // speaker badge on a PWA tile
    kHitMediaCover,   // fade of a compact cover (not a click target)
    kHitSetting,      // row of the Settings tab (arg = row index)
    kHitDiag,         // the Diagnostics area (right click copies the report)
};

inline int MakeId(int kind, int idx) { return (kind << 16) | (idx & 0xFFFF); }
inline int KindOf(int id) { return id >> 16; }
inline int IndexOf(int id) { return id & 0xFFFF; }

enum MenuCmd {
    kCmdNone = 0,
    kCmdShow,
    kCmdHide,
    kCmdQuit,
    kCmdLaunch,
    kCmdToggleCat,
    kCmdNewCat,
    kCmdRenameCat,
    kCmdDeleteCat,
    kCmdMoveLeft,
    kCmdMoveRight,
    kCmdShowPWA,      // Show PWA window from media card
    kCmdClosePWA,     // Close PWA app from media card
    kCmdMediaPlay,    // media card: play / pause (arg = media index)
    kCmdMediaPrev,
    kCmdMediaNext,
    kCmdToggleMediaView,
    kCmdSetPref,      // Settings dropdown option (arg = pref * 16 + option)
    kCmdCopyLogs,     // copy the Diagnostics report to the clipboard
};

struct MenuItem {
    int cmd = kCmdNone;
    int arg = 0;
    std::wstring text;
    PCWSTR glyph = nullptr;
    bool checked = false;
    bool enabled = true;
    bool sep = false;
    bool header = false;           // info row: icon and text, not clickable
    ID2D1Bitmap* bmp = nullptr;    // icon of a header row
};

struct MenuCtx {
    HWND hwnd = nullptr;
    std::wstring crxId;
    int libIndex = -1;
    int tab = -1;
};

struct Hit {
    int id;
    D2D1_RECT_F r;
};

struct Theme {
    bool dark = true;
    D2D1_COLOR_F text, textSecondary, card, cardBorder, hover, press, divider,
        track, bgSolid, accent, accentHover, accentPress, onAccent, menuBg, menuBgMica,
        menuBorder, fieldBg, scrim;
};

struct NotifRow {
    int appIdx = 0;  // index into Panel::all
    std::wstring title;
    std::wstring body;
};

struct TileData {
    std::wstring name;
    ID2D1Bitmap* bmp = nullptr;
    int unread = 0;
    bool running = false;
    bool hasMedia = false;  // the app has a media session
    bool dim = false;       // the PWA is visible (not hidden): drawn translucent
    std::wstring key;       // identity of the item, for the reorder glide
};

struct Optim {
    bool playing = false;
    long long posMs = 0;
    ULONGLONG posTick = 0;
    ULONGLONG until = 0;
};

struct CoverEntry {
    uint64_t id = (uint64_t)-1;
    ID2D1Bitmap* bmp = nullptr;
};

enum class NotifSeverity { Info, Success, Warning, Error };

constexpr float kNoteW = 280.0f;
constexpr float kNoteMinH = 48.0f;  // a note is at least this tall
constexpr float kNoteMaxH = 140.0f; // and at most this tall (longer text is cut)
constexpr float kNoteGap = 8.0f;
constexpr float kNoteHideMs = 4000.0f;
constexpr size_t kNoteMax = 3;

// A notification drawn inside the panel. One card style for every severity.
struct Note {
    float h = 48.0f;  // height, from the text (see NoteHeight)
    int id = 0;
    NotifSeverity sev = NotifSeverity::Info;
    PCWSTR glyph = nullptr;  // custom icon, else the severity icon
    ID2D1Bitmap* icon = nullptr;  // app icon, shown instead of the glyph
    std::wstring title;
    std::wstring message;
    float x = 0;          // resting x, DIP
    float y = 0;          // current y, DIP (eases to targetY)
    float targetY = 0;
    float dragDx = 0;     // swipe offset, DIP
    float dragStartX = 0;
    bool dragging = false;
    bool fresh = true;    // first layout: no easing from 0
    bool leaving = false;
    int leaveDir = 1;
    int swipeDir = 0;     // -1 left, +1 right, 0 both ways
    float ttl = kNoteHideMs;
    Tween enter;          // slide-in progress, 0 to 1
    Tween leave;          // leaving: accelerates out to the edge, 0 to 1
    Tween yT;             // glide to the place in the stack
    float leaveFrom = 0;  // swipe offset when the leave began
    bool leaveStarted = false;
};

// Press spring of one pressable element: its scale now, and its speed.
struct PressSpring {
    float scale = 1.0f;
    float vel = 0.0f;
};

// Drawing transform of the panel, uniform: x' = s * x + tx, y' = s * y + ty.
struct Xf {
    float s = 1.0f;
    float tx = 0.0f;
    float ty = 0.0f;
};

// The element that was just dropped: it glides from the pointer to its slot.
struct Settle {
    bool active = false;
    bool placed = false;        // its slot is known: it has been drawn at its place
    int id = 0;                 // hit id of the element at its slot
    bool settleX = false;       // glides along x
    bool settleY = false;       // glides along y
    float gx = 0, gy = 0;       // centre of the ghost at the drop (DIP)
    float offX = 0, offY = 0;   // from the ghost to the slot, set when placed
    ULONGLONG tDrop = 0;        // when it was dropped
    ULONGLONG t0 = 0;           // when the glide started (once placed)
};

// Glide duration, Material 3 emphasized decelerate curve.
constexpr float kSettleMs = 220.0f;

// One edge of the tab indicator. It moves from where it is now to a target, with a
// start delay: the leading edge goes at once, the trailing edge follows.
struct Edge {
    float from = 0.0f;
    float to = 0.0f;
    ULONGLONG t0 = 0;
    int delay = 0;
    int dur = 0;

    float Value(ULONGLONG now) const {
        if (dur <= 0) {
            return to;
        }
        if (now < t0 + (ULONGLONG)delay) {
            return from;
        }
        const float x = Clamp01((float)(now - t0 - (ULONGLONG)delay) / (float)dur);
        return from + (to - from) * (float)CubicBezierY(0, 0, 0, 1, x);
    }
    bool Running(ULONGLONG now) const {
        return dur > 0 && now < t0 + (ULONGLONG)delay + (ULONGLONG)dur;
    }
    void Start(ULONGLONG now, float target, int durMs, int delayMs) {
        from = Value(now);
        to = target;
        t0 = now;
        dur = durMs;
        delay = delayMs;
    }
    void Jump(ULONGLONG now, float target) {
        from = to = target;
        t0 = now;
        dur = 0;
        delay = 0;
    }
};

// Reorder glide of one item: where it was drawn last frame (scroll-free), and how far
// it still has to slide to its new place.
struct MoveAnim {
    bool seen = false;
    float lastRx = 0.0f;
    float lastRy = 0.0f;
    Edge ex;
    Edge ey;
};

struct Panel {
    HWND hwnd = nullptr;
    int dpi = 96;
    bool solid = false;  // opaque background (no Mica)
    Theme th;

    // Placement.
    POINT cursor = {};
    RECT work = {};
    RECT anchor = {};  // tray icon rectangle, if known
    bool hasAnchor = false;
    bool bottomAnchored = true;
    int pxW = 0;
    int pxH = 0;
    float hDip = 0;

    // Direct2D on a DXGI composition swap chain (premultiplied alpha), shown by
    // DirectComposition. The swap chain's transparent pixels let Mica through.
    Cp<ID3D11Device> d3d;
    Cp<IDXGIDevice> dxgi;
    Cp<ID2D1Device> d2dDevice;
    Cp<ID2D1RenderTarget> rt;          // owns the device context
    ID2D1DeviceContext* dc = nullptr;  // the same object, borrowed
    Cp<ID2D1SolidColorBrush> brush;
    Cp<IDXGISwapChain1> swap;
    Cp<ID2D1Bitmap1> target;           // back buffer of the swap chain, as target
    Cp<IDCompositionDevice> dcomp;
    Cp<IDCompositionTarget> dcTarget;
    Cp<IDCompositionVisual> dcVisual;
    // Direct2D / DirectWrite.
    Cp<IDWriteTextFormat> fName, fBody, fBodyWrap, fSmall, fTimeRight, fButton, fIcon,
        fIconBig, fIconHuge, fTab, fCenter, fBadge, fLetter, fDiag, fMenu,
        fMenuIcon;
    std::unordered_map<std::wstring, ID2D1Bitmap*> bmps;  // owned, may be null
    std::unordered_map<std::wstring, CoverEntry> covers;  // owned

    // Back buffer: size and DPI of the swap chain's target.
    int bbW = 0;
    int bbH = 0;
    float bbDpi = 0;

    // Data.
    std::vector<AppInfo> all;       // matching PWA windows
    std::vector<int> hiddenIdx;     // indexes into `all`, hidden ones
    std::vector<int> appsIdx;       // every PWA window in the order of the PWAs tab
    std::vector<MediaInfo> media;
    std::vector<ToastInfo> toasts;
    std::vector<NotifRow> rows;
    std::unordered_map<std::wstring, Optim> optim;

    // Tabs.
    int order[kTabCount] = {0, 1, 2, 3, 4};
    int selectedTab = kTabMedia;
    Tween tabPos[kTabCount];
    Tween slideX, slideA, openDy, openAlpha, menuT, menuDy;
    Tween exitX, exitA;                   // the tab being left: slides out and fades
    int exitTab = -1;
    Edge indL, indR;                      // left and right edges of the tab indicator
    std::unordered_map<std::wstring, MoveAnim> moves;  // reorder glide state, by item key
    Tween barA;                           // alpha of the scrollbar
    ULONGLONG barTick = 0;                // last scroll, or pointer move over the list
    Tween scrollTw[kTabCount];            // drawn scroll of each tab
    Tween chipTw;                         // drawn scroll of the category chips
    int dragTab = -1;
    bool dragging = false;
    float dragStartX = 0;
    float dragOrigX = 0;

    // Scrolling.
    float scrollT[kTabCount] = {};
    float scrollC[kTabCount] = {};
    float contentH[kTabCount] = {};
    float viewH[kTabCount] = {};
    float chipScroll = 0;   // drawn offset of the chip row
    float chipScrollT = 0;  // where the chip row is heading (smooth scroll)
    float chipGhostW = 0;       // width of the dragged category chip
    float chipContentW = 0;
    int chipSel = 0;  // 0 = All, k = category k - 1

    // Library: drag a tile to another place, like on a phone home screen.
    int libDownIdx = -1;               // tile pressed (display index), -1 none
    float libDownX = 0, libDownY = 0;  // where the press began, DIP
    bool libDragging = false;          // the press moved far enough to drag
    float libDragX = 0, libDragY = 0;  // pointer while dragging, DIP
    int libTargetIdx = -1;             // tile under the pointer while dragging
    float gridTileW = 0, gridTileH = 0;  // size of the last drawn grid tile
    int libDragTab = -1;                 // tab the drag started on
    int libDragKind = kHitNone;          // hit kind of the dragged item
    int dragPressId = 0;                 // hit id of the dragged element, it stays pressed during the drag
    float grabX = 0, grabY = 0;          // grab point, from the centre of the dragged element (DIP)
    // Saved orders of the PWAs, Media and Notifications tabs (keys of items).
    std::vector<std::wstring> orderApps, orderMedia, orderNotifs;
    std::wstring stateSig;  // what was on screen last time, to skip redraws

    // Interaction.
    std::unordered_map<int, Tween> hoverT;
    std::unordered_map<int, Tween> toggleT;  // Settings toggles: thumb position, by pref
    std::vector<D2D1_RECT_F> settingRects;   // rows of the Settings tab, as drawn
    int togglePressRow = -1;      // Settings row whose toggle is pressed, -1 none
    bool toggleDragging = false;  // the press moved far enough to drag the thumb
    float toggleDownX = 0;        // where the press began
    float toggleStartV = 0;       // thumb position when the press began
    float toggleLastX = 0;
    int toggleDir = 0;            // last direction of the drag, -1 or +1
    std::wstring diagReport;      // the Diagnostics report, rebuilt once a second
    ULONGLONG diagTick = 0;
    float diagScroll = 0;         // scroll of the Diagnostics area
    float diagMax = 0;            // its largest scroll, from the last draw
    int hovered = kHitNone;
    int pressed = kHitNone;
    int seekingIdx = -1;
    double seekFrac = 0;
    std::vector<D2D1_RECT_F> trackRects;
    D2D1_RECT_F volTrack = {};
    double volFrac = 1.0;
    bool volMute = false;
    bool volDrag = false;
    bool volKnown = false;
    bool volPending = false;
    ULONGLONG volTick = 0;
    ULONGLONG volSetTick = 0;
    bool tracking = false;
    bool nativeMenuActive = false;  // a native popup menu is running its loop
    bool hotkeyListening = false;   // waiting for a key combination in Settings
    bool closeAfterMenu = false;    // close requested while that menu was open
    Xf xf;                                         // current drawing transform (press springs)
    std::unordered_map<int, PressSpring> springs;  // press springs of pressable elements, by hit id
    Settle settle;                                 // glide of the element just dropped

    // Title bar.
    bool closeHover = false;
    bool closePressed = false;
    Tween closeT;                         // hover fade of the close button, 0 to 1
    POINT dragStartPos = {};
    bool titleDragging = false;

    // Context menu and dialog.
    bool menuOpen = false;
    std::vector<MenuItem> menuItems;
    MenuCtx menuCtx;
    D2D1_RECT_F menuRect = {};
    bool dlgOpen = false;
    bool dlgRename = false;
    int dlgCat = -1;
    std::wstring dlgAddCrx;
    std::wstring dlgText;  // text of the category dialog, drawn by the panel
    std::vector<std::wstring> dlgLog;  // chat-style warnings, oldest first
    std::wstring dlgDupKey;  // lowercase name of the repeated duplicate try
    int dlgDupCount = 0;     // consecutive tries of that name in this dialog
    struct DlgSnap {
        std::wstring text;
        int caret = 0;
    };
    int dlgSelAnchor = -1;         // selection start, -1 = no selection
    bool dlgSelDragging = false;   // mouse selection in progress
    bool dlgTyping = false;        // last edit was typing (undo steps are grouped)
    std::vector<DlgSnap> dlgUndo;
    std::vector<DlgSnap> dlgRedo;
    int dlgCaret = 0;      // caret position in dlgText
    float ui = 1.0f;          // UI scale on top of the DPI (the panel is 1.5x)
    std::vector<Note> notes;  // in-panel notifications, oldest first
    int noteSeq = 0;
    int noteDown = -1;        // note under a pressed button
    bool dlgFocused = false;  // the text field has the focus
    bool dlgCaretOn = true;   // caret blink phase
    D2D1_RECT_F dlgRect = {}, dlgField = {};

    // Library search field.
    std::wstring searchText;
    int searchCaret = 0;
    int searchSelAnchor = -1;        // -1 = no selection
    bool searchSelDragging = false;  // mouse selection in progress
    bool searchFocused = false;
    bool searchCaretOn = true;       // caret blink phase

    // Per frame state.
    std::vector<Hit> hits;
    D2D1_RECT_F hitClip = {};
    bool hitsOn = true;
    float ox = 0, oy = 0, alpha = 1;
    float viewTop = 0, viewBottom = 0;
    ULONGLONG lastFrame = 0;
    bool animating = false;
    std::vector<HWND> frameHwnds;  // Apps tiles / notification rows
    std::vector<bool> frameHidden;  // per tile of the PWAs tab: hidden or visible
    std::vector<int> frameLib;     // Library tiles -> g_library index
    std::vector<std::wstring> frameMediaKeys;  // PWAs tiles -> media card key (or empty)
    std::wstring hlKey;            // media card to highlight after a jump
    ULONGLONG hlTick = 0;          // when the jump happened
};

// True while the drag that started on `tab` is in progress.
bool DragOn(const Panel& p, int tab) {
    return p.libDragging && p.libDragTab == tab;
}

// Which press starts a drag on the current tab.
bool CanDragHit(const Panel& p, int hit, float fy) {
    switch (p.selectedTab) {
        case kTabLibrary:
            if (KindOf(hit) == kHitChip) {
                // Only the categories can move: not "All", not the "+" chip.
                return IndexOf(hit) >= 1 &&
                       IndexOf(hit) <= (int)g_categories.size() &&
                       fy < kContentTop + 34;
            }
            return KindOf(hit) == kHitTile && fy >= kContentTop + 40;
        case kTabApps:
            return KindOf(hit) == kHitTile && fy >= kContentTop;
        case kTabMedia:
            return KindOf(hit) == kHitMediaCard;
        case kTabNotifs:
            return KindOf(hit) == kHitRow;
        default:
            return false;
    }
}

HHOOK g_mouseHook = nullptr;        // Hermes outside press (hook thread only)
HHOOK g_mouseHotkeyHook = nullptr;  // mouse shortcut (hook thread only)
Panel* g_panel = nullptr;
ULONGLONG g_panelClosedTick = 0;
ATOM g_panelClass = 0;
std::atomic<HWND> g_hostHwnd{nullptr};

ID2D1Factory* g_d2d = nullptr;
IDWriteFactory* g_dwrite = nullptr;
IWICImagingFactory* g_wic = nullptr;

bool EnsureFactories() {
    if (!g_d2d &&
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,
                                 kIidD2DFactory, nullptr, (void**)&g_d2d))) {
        return false;
    }
    if (!g_dwrite &&
        FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, kIidDWriteFactory,
                                   (IUnknown**)&g_dwrite))) {
        return false;
    }
    if (!g_wic &&
        FAILED(CoCreateInstance(kClsidWicFactory, nullptr, CLSCTX_INPROC_SERVER,
                                kIidWicFactory, (void**)&g_wic))) {
        return false;
    }
    return true;
}

void ReleaseFactories() {
    if (g_wic) {
        g_wic->Release();
        g_wic = nullptr;
    }
    if (g_dwrite) {
        g_dwrite->Release();
        g_dwrite = nullptr;
    }
    if (g_d2d) {
        g_d2d->Release();
        g_d2d = nullptr;
    }
}

D2D1_COLOR_F Col(uint32_t argb) {
    return D2D1_COLOR_F{((argb >> 16) & 0xFF) / 255.0f,
                        ((argb >> 8) & 0xFF) / 255.0f, (argb & 0xFF) / 255.0f,
                        ((argb >> 24) & 0xFF) / 255.0f};
}

D2D1_COLOR_F WithAlpha(D2D1_COLOR_F c, float a) {
    c.a *= a;
    return c;
}

D2D1_RECT_F Rc(float x, float y, float w, float h) {
    return D2D1_RECT_F{x, y, x + w, y + h};
}

bool InRect(const D2D1_RECT_F& r, float x, float y) {
    return x >= r.left && x < r.right && y >= r.top && y < r.bottom;
}

bool IsSystemDark() {
    DWORD v = 1;
    DWORD size = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                     L"Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &v,
                     &size) != ERROR_SUCCESS) {
        v = 1;
    }
    return v == 0;
}

uint32_t GetAccentArgb() {
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        return 0xFF000000u | (color & 0xFFFFFFu);
    }
    return 0xFF0078D4u;
}

uint32_t Mix(uint32_t argb, uint32_t withRgb, float t) {
    auto ch = [&](int shift) {
        float a = (float)((argb >> shift) & 0xFF);
        float b = (float)((withRgb >> shift) & 0xFF);
        return (uint32_t)(a + (b - a) * t + 0.5f) & 0xFF;
    };
    return 0xFF000000u | (ch(16) << 16) | (ch(8) << 8) | ch(0);
}

void InitTheme(Panel& p) {
    Theme& t = p.th;
    t.dark = IsSystemDark();
    uint32_t accent = GetAccentArgb();
    uint32_t r = (accent >> 16) & 0xFF;
    uint32_t g = (accent >> 8) & 0xFF;
    uint32_t b = accent & 0xFF;
    bool lightAccent = (299 * r + 587 * g + 114 * b) / 1000 > 150;

    if (t.dark) {
        t.text = Col(0xFFFFFFFF);
        t.textSecondary = Col(0xC5FFFFFF);
        t.card = Col(0x0DFFFFFF);
        t.cardBorder = Col(0x14FFFFFF);
        t.hover = Col(0x1AFFFFFF);
        t.press = Col(0x0FFFFFFF);
        t.divider = Col(0x1FFFFFFF);
        t.track = Col(0x45FFFFFF);
        t.bgSolid = Col(0xFF2B2B2B);
        t.menuBg = Col(0xFF2C2C2C);
        t.menuBgMica = Col(0x9E2C2C2C);
        t.menuBorder = Col(0x33FFFFFF);
        t.fieldBg = Col(0xFF393939);
        t.scrim = Col(0x66000000);
    } else {
        t.text = Col(0xE4000000);
        t.textSecondary = Col(0x9E000000);
        t.card = Col(0x99FFFFFF);
        t.cardBorder = Col(0x0F000000);
        t.hover = Col(0x10000000);
        t.press = Col(0x08000000);
        t.divider = Col(0x14000000);
        t.track = Col(0x5C000000);
        t.bgSolid = Col(0xFFF3F3F3);
        t.menuBg = Col(0xFFFCFCFC);
        t.menuBgMica = Col(0x9EFCFCFC);
        t.menuBorder = Col(0x24000000);
        t.fieldBg = Col(0xFFFFFFFF);
        t.scrim = Col(0x33000000);
    }
    t.accent = Col(accent);
    t.accentHover = Col(Mix(accent, lightAccent ? 0x000000u : 0xFFFFFFu, 0.12f));
    t.accentPress = Col(Mix(accent, lightAccent ? 0x000000u : 0xFFFFFFu, 0.24f));
    t.onAccent = Col(lightAccent ? 0xFF000000u : 0xFFFFFFFFu);
}

bool FontExists(PCWSTR family) {
    Cp<IDWriteFontCollection> collection;
    if (FAILED(g_dwrite->GetSystemFontCollection(collection.Put(), FALSE))) {
        return false;
    }
    UINT32 index = 0;
    BOOL exists = FALSE;
    return SUCCEEDED(collection->FindFamilyName(family, &index, &exists)) &&
           exists;
}

bool MakeFormat(Cp<IDWriteTextFormat>& out,
                PCWSTR family,
                float size,
                DWRITE_FONT_WEIGHT weight,
                DWRITE_TEXT_ALIGNMENT hAlign,
                bool trim,
                bool wrap = false) {
    if (FAILED(g_dwrite->CreateTextFormat(
            family, nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, size, L"en-us", out.Put()))) {
        return false;
    }
    out->SetTextAlignment(hAlign);
    out->SetParagraphAlignment(wrap ? DWRITE_PARAGRAPH_ALIGNMENT_NEAR
                                    : DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    out->SetWordWrapping(wrap ? DWRITE_WORD_WRAPPING_WRAP
                              : DWRITE_WORD_WRAPPING_NO_WRAP);
    if (trim) {
        Cp<IDWriteInlineObject> sign;
        if (SUCCEEDED(g_dwrite->CreateEllipsisTrimmingSign(out.p, sign.Put()))) {
            DWRITE_TRIMMING trimming = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0,
                                        0};
            out->SetTrimming(&trimming, sign.p);
        }
    }
    return true;
}

PCWSTR TextFontFamily() {
    return FontExists(L"Segoe UI Variable Text") ? L"Segoe UI Variable Text"
                                                 : L"Segoe UI";
}

bool CreateTextFormats(Panel& p) {
    PCWSTR text = TextFontFamily();
    PCWSTR icons = FontExists(L"Segoe Fluent Icons") ? L"Segoe Fluent Icons"
                                                     : L"Segoe MDL2 Assets";
    const auto normal = DWRITE_FONT_WEIGHT_NORMAL;
    const auto semi = DWRITE_FONT_WEIGHT_SEMI_BOLD;
    const auto left = DWRITE_TEXT_ALIGNMENT_LEADING;
    const auto center = DWRITE_TEXT_ALIGNMENT_CENTER;

    return MakeFormat(p.fName, text, 14.0f, semi, left, true) &&
           MakeFormat(p.fBody, text, 12.0f, normal, left, true) &&
           MakeFormat(p.fBodyWrap, text, 12.0f, normal, left, true, true) &&
           MakeFormat(p.fSmall, text, 11.0f, normal, left, false) &&
           MakeFormat(p.fTimeRight, text, 11.0f, normal,
                      DWRITE_TEXT_ALIGNMENT_TRAILING, false) &&
           MakeFormat(p.fButton, text, 13.0f, normal, left, true) &&
           MakeFormat(p.fMenu, text, 13.0f, normal, left, true) &&
           MakeFormat(p.fTab, text, 11.0f, normal, center, false) &&
           MakeFormat(p.fCenter, text, 13.0f, normal, center, false) &&
           MakeFormat(p.fBadge, text, 10.0f, semi, center, false) &&
           MakeFormat(p.fLetter, text, 14.0f, semi, center, false) &&
           MakeFormat(p.fDiag, text, 10.0f, normal, left, false, true) &&
           MakeFormat(p.fIcon, icons, 16.0f, normal, center, false) &&
           MakeFormat(p.fMenuIcon, icons, 13.0f, normal, center, false) &&
           MakeFormat(p.fIconBig, icons, 20.0f, normal, center, false) &&
           MakeFormat(p.fIconHuge, icons, 34.0f, normal, center, false);
}

// Rounds the corners of a square premultiplied BGRA image.
void RoundCorners(std::vector<uint8_t>& px, int size, float radius) {
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            float dx = (std::max)({radius - (x + 0.5f),
                                   (x + 0.5f) - (size - radius), 0.0f});
            float dy = (std::max)({radius - (y + 0.5f),
                                   (y + 0.5f) - (size - radius), 0.0f});
            if (dx <= 0 || dy <= 0) {
                continue;
            }
            float cov = radius - std::sqrt(dx * dx + dy * dy) + 0.5f;
            cov = cov < 0 ? 0 : (cov > 1 ? 1 : cov);
            uint8_t* q = &px[((size_t)y * size + x) * 4];
            for (int c = 0; c < 4; ++c) {
                q[c] = (uint8_t)(q[c] * cov + 0.5f);
            }
        }
    }
}

ID2D1Bitmap* CreateCoverBitmap(Panel& p, const std::vector<uint8_t>& src) {
    if (src.size() != (size_t)kCoverPx * kCoverPx * 4) {
        return nullptr;
    }
    std::vector<uint8_t> px = src;
    RoundCorners(px, kCoverPx, kCoverPx * kRadiusCtl / 64.0f);

    D2D1_BITMAP_PROPERTIES props = {};
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    props.dpiX = 96.0f;
    props.dpiY = 96.0f;
    D2D1_SIZE_U size = {(UINT32)kCoverPx, (UINT32)kCoverPx};
    ID2D1Bitmap* out = nullptr;
    if (FAILED(p.rt->CreateBitmap(size, px.data(), kCoverPx * 4, &props, &out))) {
        return nullptr;
    }
    return out;
}

// Direct2D bitmap from an icon handle. The caller owns the icon.
ID2D1Bitmap* BitmapFromHicon(Panel& p, HICON icon) {
    if (!icon || !g_wic) {
        return nullptr;
    }
    Cp<IWICBitmap> bitmap;
    if (FAILED(g_wic->CreateBitmapFromHICON(icon, bitmap.Put()))) {
        return nullptr;
    }
    Cp<IWICFormatConverter> converter;
    if (FAILED(g_wic->CreateFormatConverter(converter.Put())) ||
        FAILED(converter->Initialize(bitmap.p, kWicPixelFormat32bppPBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0.0,
                                     WICBitmapPaletteTypeCustom))) {
        return nullptr;
    }
    ID2D1Bitmap* out = nullptr;
    if (FAILED(p.rt->CreateBitmapFromWicBitmap(converter.p, nullptr, &out))) {
        return nullptr;
    }
    return out;
}

ID2D1Bitmap* LibBitmap(Panel& p, const LibApp& l) {
    std::wstring key = L"lib:" + l.crxId + L"|" + l.profileDir;
    auto it = p.bmps.find(key);
    if (it != p.bmps.end()) {
        return it->second;
    }
    ID2D1Bitmap* bmp = nullptr;
    if (!l.icoPath.empty()) {
        HICON icon = (HICON)LoadImageW(nullptr, l.icoPath.c_str(), IMAGE_ICON, 64,
                                       64, LR_LOADFROMFILE);
        if (icon) {
            bmp = BitmapFromHicon(p, icon);
            DestroyIcon(icon);
        }
    }
    p.bmps[key] = bmp;
    return bmp;
}

ID2D1Bitmap* AppBitmap(Panel& p, const AppInfo& a) {
    if (const LibApp* l = FindLib(a.crxId, a.profile)) {
        if (ID2D1Bitmap* b = LibBitmap(p, *l)) {
            return b;
        }
    }
    std::wstring key =
        L"hw:" + std::to_wstring((unsigned long long)(uintptr_t)a.hwnd);
    auto it = p.bmps.find(key);
    if (it != p.bmps.end()) {
        return it->second;
    }
    ID2D1Bitmap* bmp = BitmapFromHicon(p, GetWindowIconSafe(a.hwnd, true));
    p.bmps[key] = bmp;
    return bmp;
}

// Text layouts made for drawing and measuring, reused between frames. Cleared
// when the panel closes, the next open makes them again.
struct LayoutEntry {
    IDWriteTextFormat* format;
    float width;
    float height;
    IDWriteTextLayout* layout;
};
std::unordered_map<std::wstring, std::vector<LayoutEntry>> g_layoutCache;
int g_layoutCount = 0;

void ClearLayoutCache() {
    for (auto& kv : g_layoutCache) {
        for (const LayoutEntry& e : kv.second) {
            e.layout->Release();
        }
    }
    g_layoutCache.clear();
    g_layoutCount = 0;
}

void ReleasePanelBitmaps(Panel& p) {
    ClearLayoutCache();
    for (auto& kv : p.bmps) {
        if (kv.second) {
            kv.second->Release();
        }
    }
    p.bmps.clear();
    for (auto& kv : p.covers) {
        if (kv.second.bmp) {
            kv.second.bmp->Release();
        }
    }
    p.covers.clear();
}

bool CreateGfx(Panel& p) {
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                   D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                                   D3D11_SDK_VERSION, p.d3d.Put(), nullptr, nullptr);
    if (FAILED(hr)) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                               D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                               D3D11_SDK_VERSION, p.d3d.Put(), nullptr, nullptr);
    }
    if (FAILED(hr)) {
        return false;
    }
    hr = p.d3d->QueryInterface(__uuidof(IDXGIDevice), (void**)p.dxgi.Put());
    if (FAILED(hr)) {
        return false;
    }

    // Same factory as g_d2d (its geometries must stay usable with this target).
    Cp<ID2D1Factory1> factory1;
    hr = g_d2d->QueryInterface(__uuidof(ID2D1Factory1), (void**)factory1.Put());
    if (FAILED(hr)) {
        return false;
    }
    hr = factory1->CreateDevice(p.dxgi.p, p.d2dDevice.Put());
    if (FAILED(hr)) {
        return false;
    }
    ID2D1DeviceContext* dc = nullptr;
    hr = p.d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &dc);
    if (FAILED(hr)) {
        return false;
    }
    p.rt.p = dc;
    p.dc = dc;

    D2D1_COLOR_F white = {1, 1, 1, 1};
    hr = p.rt->CreateSolidColorBrush(&white, nullptr, p.brush.Put());
    if (FAILED(hr)) {
        return false;
    }

    // Swap chain for composition: premultiplied alpha, so that transparent
    // pixels are see-through and the Mica backdrop shows behind them.
    Cp<IDXGIAdapter> adapter;
    Cp<IDXGIFactory2> factory2;
    hr = p.dxgi->GetAdapter(adapter.Put());
    if (SUCCEEDED(hr)) {
        hr = adapter->GetParent(__uuidof(IDXGIFactory2), (void**)factory2.Put());
    }
    if (FAILED(hr)) {
        return false;
    }
    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = (UINT)(std::max)(p.pxW, 1);
    sd.Height = (UINT)(std::max)(p.pxH, 1);
    sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.SampleDesc.Count = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.Scaling = DXGI_SCALING_STRETCH;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    sd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    hr = factory2->CreateSwapChainForComposition(p.d3d.p, &sd, nullptr, p.swap.Put());
    if (FAILED(hr)) {
        return false;
    }
    return CreateTextFormats(p);
}

// Connects the swap chain to the window with DirectComposition. Needs the window.
bool AttachComposition(Panel& p) {
    HRESULT hr = DCompositionCreateDevice(p.dxgi.p, __uuidof(IDCompositionDevice),
                                          (void**)p.dcomp.Put());
    if (FAILED(hr)) {
        return false;
    }
    hr = p.dcomp->CreateTargetForHwnd(p.hwnd, TRUE, p.dcTarget.Put());
    if (FAILED(hr)) {
        return false;
    }
    hr = p.dcomp->CreateVisual(p.dcVisual.Put());
    if (FAILED(hr)) {
        return false;
    }
    hr = p.dcVisual->SetContent(p.swap.p);
    if (FAILED(hr)) {
        return false;
    }
    hr = p.dcTarget->SetRoot(p.dcVisual.p);
    if (FAILED(hr)) {
        return false;
    }
    hr = p.dcomp->Commit();
    return SUCCEEDED(hr);
}

void FreeBackBuffer(Panel& p) {
    if (p.dc) {
        p.dc->SetTarget(nullptr);
    }
    p.target.Reset();
    p.bbW = p.bbH = 0;
    p.bbDpi = 0;
}

bool EnsureBackBuffer(Panel& p) {
    const int w = (std::max)(p.pxW, 1);
    const int h = (std::max)(p.pxH, 1);
    const float dpi = (float)(p.dpi * p.ui);
    if (p.target && p.bbW == w && p.bbH == h && p.bbDpi == dpi) {
        return true;
    }
    FreeBackBuffer(p);  // no references to the old buffer are left before a resize
    if (!p.swap || !p.dc) {
        return false;
    }
    HRESULT hr = p.swap->ResizeBuffers(0, (UINT)w, (UINT)h, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) {
        return false;
    }
    Cp<IDXGISurface> surface;
    hr = p.swap->GetBuffer(0, __uuidof(IDXGISurface), (void**)surface.Put());
    if (FAILED(hr)) {
        return false;
    }
    D2D1_BITMAP_PROPERTIES1 bp = {};
    bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bp.dpiX = dpi;
    bp.dpiY = dpi;
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    hr = p.dc->CreateBitmapFromDxgiSurface(surface.p, &bp, p.target.Put());
    if (FAILED(hr)) {
        return false;
    }
    p.dc->SetTarget(p.target.p);
    p.bbW = w;
    p.bbH = h;
    p.bbDpi = dpi;
    return true;
}

// Gets the screen rectangle of the tray icon (also works for icons inside the
// "hidden icons" flyout). Returns false if the shell doesn't know it.
bool GetTrayIconRect(HWND hWnd, RECT& out) {
    struct NotifyIconId {
        DWORD cbSize;
        HWND hWnd;
        UINT uID;
        GUID guidItem;
    };
    using GetRect_t = HRESULT(WINAPI*)(const NotifyIconId*, RECT*);

    HMODULE shell32 = LoadLibraryW(L"shell32.dll");
    if (!shell32) {
        return false;
    }
    auto fn = (GetRect_t)GetProcAddress(shell32, "Shell_NotifyIconGetRect");
    if (!fn) {
        return false;
    }
    NotifyIconId id = {};
    id.cbSize = sizeof(id);
    id.hWnd = hWnd;
    id.uID = kTrayIconId;
    RECT r = {};
    if (FAILED(fn(&id, &r)) || r.right <= r.left || r.bottom <= r.top) {
        return false;
    }
    out = r;
    return true;
}

// Puts the panel right above (or below, for a top taskbar) the tray icon, so it
// never covers the tray itself.
void Place(Panel& p, int& x, int& y) {
    int margin = (int)std::lround(12.0f * p.dpi / 96.0f);
    int centerX = p.cursor.x;
    int iconTop = p.cursor.y;
    int iconBottom = p.cursor.y;
    if (p.hasAnchor) {
        centerX = (p.anchor.left + p.anchor.right) / 2;
        iconTop = p.anchor.top;
        iconBottom = p.anchor.bottom;
    }

    x = centerX - p.pxW / 2;
    x = (std::max)((int)p.work.left + margin,
                   (std::min)(x, (int)p.work.right - p.pxW - margin));

    p.bottomAnchored = (iconTop + iconBottom) / 2 >=
                       (p.work.top + p.work.bottom) / 2;
    if (p.bottomAnchored) {
        int bottom = (std::min)(iconTop - margin, (int)p.work.bottom - margin);
        y = bottom - p.pxH;
    } else {
        y = (std::max)(iconBottom + margin, (int)p.work.top + margin);
    }
    if (y < p.work.top) {
        y = p.work.top;
    }
}

int GetMonitorDpi(HMONITOR monitor) {
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    HMODULE shcore = LoadLibraryW(L"shcore.dll");
    if (shcore) {
        auto fn = (GetDpiForMonitor_t)GetProcAddress(shcore, "GetDpiForMonitor");
        UINT x = 0, y = 0;
        if (fn && SUCCEEDED(fn(monitor, 0 /*MDT_EFFECTIVE_DPI*/, &x, &y)) && x) {
            return (int)x;
        }
    }
    UINT dpi = GetDpiForSystem();
    return dpi ? (int)dpi : 96;
}

// Brings a window to the front. A process may take the foreground only after an
// input event: the empty SendInput below is one (it moves nothing). AttachThreadInput
// is not used, it would share the input queue with another app. `topmost` also lifts
// the window above the others (used for the panel).
void ActivateWindow(HWND hwnd, bool topmost) {
    INPUT input = {};
    SendInput(1, &input, sizeof(INPUT));
    if (topmost) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    BringWindowToTop(hwnd);
    SetForegroundWindow(hwnd);
    SetActiveWindow(hwnd);
}

void ApplyBackdrop(Panel& p) {
    BOOL dark = p.th.dark ? TRUE : FALSE;
    DwmSetWindowAttribute(p.hwnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &dark,
                          sizeof(dark));
    int corner = 2;  // DWMWCP_ROUND
    DwmSetWindowAttribute(p.hwnd, 33 /*DWMWA_WINDOW_CORNER_PREFERENCE*/, &corner,
                          sizeof(corner));

    p.solid = true;
    Backdrop b = (Backdrop)g_backdrop.load();
    if (b != Backdrop::Solid && GetWindowsBuild() >= 22621) {
        const int type = 2;  // DWMSBT_MAINWINDOW (Mica)
        if (SUCCEEDED(DwmSetWindowAttribute(
                p.hwnd, 38 /*DWMWA_SYSTEMBACKDROP_TYPE*/, &type, sizeof(type)))) {
            MARGINS margins = {-1, -1, -1, -1};
            if (SUCCEEDED(DwmExtendFrameIntoClientArea(p.hwnd, &margins))) {
                p.solid = false;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Panel: drawing primitives (all of them honor the animation offset/opacity)
// ---------------------------------------------------------------------------

D2D1_RECT_F Off(const Panel& p, D2D1_RECT_F r) {
    r.left += p.ox;
    r.right += p.ox;
    r.top += p.oy;
    r.bottom += p.oy;
    return r;
}

void SetBrush(Panel& p, D2D1_COLOR_F c) {
    c.a *= p.alpha;
    p.brush->SetColor(&c);
}

void FillRR(Panel& p, const D2D1_RECT_F& r, float radius, const D2D1_COLOR_F& c) {
    SetBrush(p, c);
    D2D1_ROUNDED_RECT rr = {Off(p, r), radius, radius};
    p.rt->FillRoundedRectangle(&rr, p.brush.p);
}

void FillRect(Panel& p, const D2D1_RECT_F& r, const D2D1_COLOR_F& c) {
    SetBrush(p, c);
    D2D1_RECT_F o = Off(p, r);
    p.rt->FillRectangle(&o, p.brush.p);
}

void StrokeRR(Panel& p, const D2D1_RECT_F& r, float radius, const D2D1_COLOR_F& c) {
    SetBrush(p, c);
    D2D1_RECT_F o = Off(p, r);
    D2D1_RECT_F in = {o.left + 0.5f, o.top + 0.5f, o.right - 0.5f, o.bottom - 0.5f};
    D2D1_ROUNDED_RECT rr = {in, radius, radius};
    p.rt->DrawRoundedRectangle(&rr, p.brush.p, 1.0f, nullptr);
}

void FillCircle(Panel& p, float cx, float cy, float radius, const D2D1_COLOR_F& c) {
    SetBrush(p, c);
    D2D1_ELLIPSE e = {{cx + p.ox, cy + p.oy}, radius, radius};
    p.rt->FillEllipse(&e, p.brush.p);
}

void DrawLine(Panel& p, float x1, float y1, float x2, float y2, const D2D1_COLOR_F& c) {
    SetBrush(p, c);
    p.rt->DrawLine(D2D1_POINT_2F{x1 + p.ox, y1 + p.oy},
                   D2D1_POINT_2F{x2 + p.ox, y2 + p.oy}, p.brush.p, 1.0f, nullptr);
}

// A text layout for a box, from the cache when it was made before.
IDWriteTextLayout* CachedLayout(const std::wstring& s,
                                IDWriteTextFormat* f,
                                float width,
                                float height) {
    const float w = (std::max)(0.0f, width);
    const float h = (std::max)(0.0f, height);
    auto it = g_layoutCache.find(s);
    if (it != g_layoutCache.end()) {
        for (const LayoutEntry& e : it->second) {
            if (e.format == f && e.width == w && e.height == h) {
                return e.layout;
            }
        }
    }
    if (g_layoutCount >= 1024) {
        ClearLayoutCache();
    }
    IDWriteTextLayout* layout = nullptr;
    if (FAILED(g_dwrite->CreateTextLayout(s.c_str(), (UINT32)s.size(), f, w, h, &layout))) {
        return nullptr;
    }
    g_layoutCache[s].push_back({f, w, h, layout});
    ++g_layoutCount;
    return layout;
}

void DrawTxt(Panel& p,
             const std::wstring& s,
             IDWriteTextFormat* format,
             const D2D1_RECT_F& r,
             const D2D1_COLOR_F& c) {
    if (s.empty()) {
        return;
    }
    SetBrush(p, c);
    D2D1_RECT_F o = Off(p, r);
    IDWriteTextLayout* layout =
        CachedLayout(s, format, o.right - o.left, o.bottom - o.top);
    if (!layout) {
        return;
    }
    p.rt->DrawTextLayout(D2D1_POINT_2F{o.left, o.top}, layout, p.brush.p,
                         D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void DrawBmp(Panel& p, ID2D1Bitmap* bmp, const D2D1_RECT_F& r, float opacity = 1.0f) {
    if (!bmp) {
        return;
    }
    D2D1_RECT_F o = Off(p, r);
    p.rt->DrawBitmap(bmp, &o, p.alpha * opacity,
                     D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, nullptr);
}

float MeasureH(const std::wstring& s, IDWriteTextFormat* f, float width) {
    if (s.empty()) {
        return 0;
    }
    IDWriteTextLayout* layout = CachedLayout(s, f, width, 100000.0f);
    if (!layout) {
        return 0;
    }
    DWRITE_TEXT_METRICS m = {};
    layout->GetMetrics(&m);
    return m.height;
}

float MeasureW(const std::wstring& s, IDWriteTextFormat* f) {
    if (s.empty()) {
        return 0;
    }
    IDWriteTextLayout* layout = CachedLayout(s, f, 1000.0f, 40.0f);
    if (!layout) {
        return 60.0f;
    }
    DWRITE_TEXT_METRICS m = {};
    layout->GetMetrics(&m);
    return m.widthIncludingTrailingWhitespace;
}

void PushClip(Panel& p, const D2D1_RECT_F& r) {
    p.rt->PushAxisAlignedClip(&r, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void PopClip(Panel& p) {
    p.rt->PopAxisAlignedClip();
}

void AddHit(Panel& p, int id, D2D1_RECT_F r) {
    if (!p.hitsOn) {
        return;
    }
    r.left = (std::max)(r.left, p.hitClip.left);
    r.top = (std::max)(r.top, p.hitClip.top);
    r.right = (std::min)(r.right, p.hitClip.right);
    r.bottom = (std::min)(r.bottom, p.hitClip.bottom);
    if (r.right <= r.left || r.bottom <= r.top) {
        return;
    }
    p.hits.push_back({id, r});
}

int HitTest(const Panel& p, float x, float y) {
    for (size_t i = p.hits.size(); i-- > 0;) {
        if (InRect(p.hits[i].r, x, y)) {
            return p.hits[i].id;
        }
    }
    return kHitNone;
}

float Hover(const Panel& p, int id) {
    auto it = p.hoverT.find(id);
    return it == p.hoverT.end() ? 0.0f : it->second.v;
}

// Hover/press highlight with the Fluent "faster" fade (83 ms).
void DrawHoverBg(Panel& p, int id, const D2D1_RECT_F& r, float radius) {
    bool press = p.pressed == id && p.hovered == id;
    float h = Hover(p, id);
    if (press) {
        FillRR(p, r, radius, p.th.press);
    } else if (h > 0.01f) {
        FillRR(p, r, radius, WithAlpha(p.th.hover, h));
    }
}

// Frames are paced by the display. While the panel animates, a small thread
// waits for each vertical blank (DwmFlush) and asks the panel for one redraw.
// Nothing is requested while the panel is idle.
constexpr UINT kMsgFrame = WM_APP + 2;
std::atomic<HWND> g_frameHwnd{nullptr};   // the open panel, null when closed
std::atomic<bool> g_framePending{false};  // a redraw request is in the queue
HANDLE g_frameWake = nullptr;            // set while a panel is open, wakes the frame thread
std::atomic<bool> g_frameStop{false};
HANDLE g_frameThread = nullptr;

DWORD WINAPI FrameThreadProc(LPVOID) {
    LARGE_INTEGER freq = {};
    QueryPerformanceFrequency(&freq);
    while (!g_frameStop) {
        HWND h = g_frameHwnd.load();
        if (!h) {
            // No panel open: sleep until ShowPanel wakes this thread.
            WaitForSingleObject(g_frameWake, INFINITE);
            continue;
        }
        LARGE_INTEGER t0 = {};
        LARGE_INTEGER t1 = {};
        QueryPerformanceCounter(&t0);
        DwmFlush();  // returns at the next vertical blank while DWM composes
        QueryPerformanceCounter(&t1);
        // Without composition DwmFlush returns at once: do not spin.
        if ((t1.QuadPart - t0.QuadPart) * 1000 < freq.QuadPart) {
            Sleep(4);
        }
        if (!g_framePending.exchange(true)) {
            PostMessageW(h, kMsgFrame, 0, 0);
        }
    }
    return 0;
}

void EnsureFrameThread() {
    if (g_stopping) {
        return;
    }
    if (!g_frameWake) {
        g_frameWake = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    }
    if (!g_frameThread) {
        g_frameStop = false;
        g_frameThread = CreateThread(nullptr, 0, FrameThreadProc, nullptr, 0, nullptr);
    }
}

void StopFrameThread() {
    g_frameStop = true;
    if (g_frameWake) {
        SetEvent(g_frameWake);
    }
    if (g_frameThread) {
        WaitForSingleObject(g_frameThread, INFINITE);
        CloseHandle(g_frameThread);
        g_frameThread = nullptr;
    }
    if (g_frameWake) {
        CloseHandle(g_frameWake);
        g_frameWake = nullptr;
    }
}

void StartAnim(Panel& p) {
    p.animating = true;
    InvalidateRect(p.hwnd, nullptr, FALSE);
}

void SetHover(Panel& p, int id) {
    if (id == p.hovered) {
        return;
    }
    if (p.hovered != kHitNone) {
        TweenSet(p.hoverT[p.hovered], 0.0f, 83, kEaseLinear);
    }
    if (id != kHitNone) {
        TweenSet(p.hoverT[id], 1.0f, 83, kEaseLinear);
    }
    p.hovered = id;
    StartAnim(p);
}

// ---------------------------------------------------------------------------
// Panel: data
// ---------------------------------------------------------------------------

int SlotOf(const Panel& p, int tab) {
    for (int i = 0; i < kTabCount; ++i) {
        if (p.order[i] == tab) {
            return i;
        }
    }
    return 0;
}

float TabW() {
    return (kPanelW - 2 * kPad) / (float)kTabCount;
}

float SlotX(int slot) {
    return kPad + slot * TabW();
}

long long CurrentPosMs(const MediaInfo& m) {
    long long pos = m.posMs;
    if (m.playing && m.posTick) {
        pos += (long long)(GetTickCount64() - m.posTick);
    }
    if (m.durMs > 0 && pos > m.durMs) {
        pos = m.durMs;
    }
    return pos < 0 ? 0 : pos;
}

const AppInfo* FindRunning(const Panel& p, const LibApp& l) {
    const AppInfo* any = nullptr;
    for (const auto& a : p.all) {
        if (a.crxId != l.crxId) {
            continue;
        }
        if (!a.profile.empty() && ToLower(a.profile) == ToLower(l.profileDir)) {
            return &a;
        }
        if (!any) {
            any = &a;
        }
    }
    return any;
}

std::wstring MediaSourceName(const Panel& p, const MediaInfo& m) {
    if (!m.matchedCrx.empty()) {
        for (const auto& a : p.all) {
            if (a.crxId == m.matchedCrx) {
                return a.name;
            }
        }
        if (const LibApp* l = FindLib(m.matchedCrx, L"")) {
            return l->name;
        }
    }
    return L"Google Chrome";
}

// Copies text to the clipboard as Unicode.
void SetClipboardText(HWND owner, const std::wstring& text) {
    if (!OpenClipboard(owner)) {
        return;
    }
    EmptyClipboard();
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(wchar_t));
    if (mem) {
        void* dst = GlobalLock(mem);
        if (dst) {
            memcpy(dst, text.c_str(), (text.size() + 1) * sizeof(wchar_t));
            GlobalUnlock(mem);
            SetClipboardData(CF_UNICODETEXT, mem);  // the system owns it now
        } else {
            GlobalFree(mem);
        }
    }
    CloseClipboard();
}

// Keys that identify an item across refreshes, for the saved orders.
std::wstring AppKey(const AppInfo& a) {
    return a.crxId + L"@" + ToLower(a.profile);
}

// A media session key without its ordinal ("<source>#<n>" -> "<source>").
std::wstring MediaBaseKey(const MediaInfo& m) {
    const size_t h = m.key.rfind(L'#');
    return h == std::wstring::npos ? m.key : m.key.substr(0, h);
}

// Position of a key in a saved order; unknown keys go last.
size_t OrderPos(const std::vector<std::wstring>& order, const std::wstring& key) {
    auto it = std::find(order.begin(), order.end(), key);
    return it == order.end() ? order.size() : (size_t)(it - order.begin());
}

std::vector<std::wstring> ParseOrderList(const std::wstring& text) {
    std::vector<std::wstring> out;
    for (const auto& t : Split(text, L',')) {
        std::wstring k = Trim(t);
        if (!k.empty()) {
            out.push_back(k);
        }
    }
    return out;
}

void SaveOrderList(PCWSTR name, const std::vector<std::wstring>& keys) {
    std::wstring s;
    for (size_t i = 0; i < keys.size(); ++i) {
        s += (i ? L"," : L"") + keys[i];
    }
    Wh_SetStringValue(name, s.c_str());
}

// The keys in `front`, then the keys of `old` that are not in `front`, so an
// item that is away for a while keeps its place.
std::vector<std::wstring> MergeOrder(std::vector<std::wstring> front,
                                     const std::vector<std::wstring>& old) {
    for (const auto& k : old) {
        if (std::find(front.begin(), front.end(), k) == front.end()) {
            front.push_back(k);
        }
    }
    return front;
}

// Everything the panel shows that changes between refreshes, as one string.
// Playback position is left out: a playing track animates on its own.
std::wstring PanelStateSignature(const Panel& p) {
    std::wstring s;
    for (const auto& a : p.all) {
        s += a.aumid + L"|" + a.title + L"|" + std::to_wstring(a.unread) +
             (a.hidden ? L"h" : L"v") + L";";
    }
    for (const auto& m : p.media) {
        s += m.key + L"|" + m.title + L"|" + m.artist + L"|" +
             std::to_wstring(m.coverId) + (m.playing ? L"p" : L"s") +
             (m.canPrev ? L"1" : L"0") + (m.canNext ? L"1" : L"0") +
             (m.canToggle ? L"1" : L"0") + L";";
    }
    for (const auto& r : p.rows) {
        s += r.title + L"|" + r.body + L";";
    }
    s += L"#" + std::to_wstring((int)std::lround(p.volFrac * 1000.0)) +
         (p.volMute ? L"m" : L"-") + (p.volKnown ? L"k" : L"-");
    return s;
}

// The PWA windows from the worker's last snapshot, with their names, their
// current hidden state and the order of the PWAs tab.
std::vector<AppInfo> TakeWindowSnapshot() {
    std::vector<AppInfo> raw;
    {
        std::lock_guard<std::mutex> lock(g_workMutex);
        raw = g_windowSnap;
    }
    std::vector<AppInfo> out;
    for (auto& a : raw) {
        if (!IsWindow(a.hwnd)) {
            continue;
        }
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            auto it = g_windows.find(a.hwnd);
            if (it == g_windows.end()) {
                continue;
            }
            a.hidden = it->second.hiddenByUs && !IsWindowVisible(a.hwnd);
            a.hiddenTick = it->second.hiddenTick;
        }
        a.name = AppDisplayName(a);
        out.push_back(std::move(a));
    }
    std::sort(out.begin(), out.end(), [](const AppInfo& a, const AppInfo& b) {
        if (a.hidden != b.hidden) {
            return a.hidden;
        }
        if (a.hidden) {
            return a.hiddenTick < b.hiddenTick;
        }
        return ToLower(a.name) < ToLower(b.name);
    });
    return out;
}

// Pulls the PWA windows, media sessions and notifications. Returns false when
// there is nothing hidden any more.
bool RefreshData(Panel& p) {
    p.all = TakeWindowSnapshot();
    p.hiddenIdx.clear();
    for (size_t i = 0; i < p.all.size(); ++i) {
        if (p.all[i].hidden) {
            p.hiddenIdx.push_back((int)i);
        }
    }
    // The PWAs tab keeps its saved order (the hidden ones).
    std::stable_sort(p.hiddenIdx.begin(), p.hiddenIdx.end(),
                     [&](int x, int y) {
                         return OrderPos(p.orderApps, AppKey(p.all[x])) <
                                OrderPos(p.orderApps, AppKey(p.all[y]));
                     });
    // The PWAs tab lists every PWA window: hidden ones first, the rest translucent.
    // Saved order is kept, unsaved windows keep the base order.
    p.appsIdx.resize(p.all.size());
    for (size_t i = 0; i < p.all.size(); ++i) {
        p.appsIdx[i] = (int)i;
    }
    std::stable_sort(p.appsIdx.begin(), p.appsIdx.end(), [&](int x, int y) {
        return OrderPos(p.orderApps, AppKey(p.all[x])) < OrderPos(p.orderApps, AppKey(p.all[y]));
    });
    SetKnownApps(p.all);

    // Media: keep the optimistic play/pause state for a moment.
    std::vector<MediaInfo> media;
    GetWorkSnapshot(media, p.toasts);
    ULONGLONG tick = GetTickCount64();
    for (auto& m : media) {
        auto it = p.optim.find(m.key);
        if (it != p.optim.end() && tick < it->second.until) {
            m.playing = it->second.playing;
            m.posMs = it->second.posMs;
            m.posTick = it->second.posTick;
        }
    }
    for (auto it = p.optim.begin(); it != p.optim.end();) {
        if (tick >= it->second.until) {
            it = p.optim.erase(it);
        } else {
            ++it;
        }
    }
    // The Media tab keeps its saved order.
    std::stable_sort(media.begin(), media.end(),
                     [&](const MediaInfo& x, const MediaInfo& y) {
                         return OrderPos(p.orderMedia, MediaBaseKey(x)) <
                                OrderPos(p.orderMedia, MediaBaseKey(y));
                     });
    p.media = std::move(media);

    // Covers.
    for (const auto& m : p.media) {
        CoverEntry& ce = p.covers[m.key];
        if (ce.id != m.coverId) {
            if (ce.bmp) {
                ce.bmp->Release();
                ce.bmp = nullptr;
            }
            ce.id = m.coverId;
            if (m.cover) {
                ce.bmp = CreateCoverBitmap(p, *m.cover);
            }
        }
    }
    for (auto it = p.covers.begin(); it != p.covers.end();) {
        bool present = false;
        for (const auto& m : p.media) {
            if (m.key == it->first) {
                present = true;
                break;
            }
        }
        if (!present) {
            if (it->second.bmp) {
                it->second.bmp->Release();
            }
            it = p.covers.erase(it);
        } else {
            ++it;
        }
    }

    // Notification rows: toasts of the notification center, or the unread
    // counter from the window title when there are none.
    p.rows.clear();
    // The Notifications tab keeps its saved order of apps.
    std::vector<size_t> rowOrder(p.all.size());
    for (size_t k = 0; k < rowOrder.size(); ++k) {
        rowOrder[k] = k;
    }
    std::stable_sort(rowOrder.begin(), rowOrder.end(), [&](size_t x, size_t y) {
        return OrderPos(p.orderNotifs, AppKey(p.all[x])) <
               OrderPos(p.orderNotifs, AppKey(p.all[y]));
    });
    for (size_t i : rowOrder) {
        const AppInfo& a = p.all[i];
        int found = 0;
        for (const auto& t : p.toasts) {
            if (ToLower(t.aumid) != ToLower(a.aumid)) {
                continue;
            }
            NotifRow r;
            r.appIdx = (int)i;
            r.title = t.title.empty() ? a.name : t.title;
            r.body = t.body;
            p.rows.push_back(std::move(r));
            ++found;
        }
        if (!found && a.unread > 0) {
            NotifRow r;
            r.appIdx = (int)i;
            r.title = a.name;
            r.body = std::to_wstring(a.unread) + L" unread";
            p.rows.push_back(std::move(r));
        }
    }

    return !p.all.empty();
}

void ClampScroll(Panel& p, int tab) {
    float maxScroll = (std::max)(0.0f, p.contentH[tab] - p.viewH[tab]);
    p.scrollT[tab] = (std::max)(0.0f, (std::min)(p.scrollT[tab], maxScroll));
}

// Press spring, shared by every pressable element of the panel: transport buttons,
// PWA and library tiles, the speaker badge and the hidden counter. Pressing scales
// the element to kPressScale, releasing bounces it back. Material 3 Expressive
// spatial values (stiffness and damping ratio, mass 1): pressing settles without
// bounce, release bounces a little.
constexpr float kPressScale = 0.9f;
constexpr float kPressStiffness = 1400.0f;   // standard spatial fast
constexpr float kPressDamping = 0.9f;
constexpr float kReleaseStiffness = 800.0f;  // expressive spatial fast
constexpr float kReleaseDamping = 0.6f;

// Elements that get a press spring.
bool IsSpringKind(int kind) {
    return kind == kHitMediaPlay || kind == kHitMediaPrev || kind == kHitMediaNext ||
           kind == kHitMediaCard || kind == kHitTile || kind == kHitTileMedia ||
           kind == kHitHiddenBadge || kind == kHitTab;
}

// Scale of an element's spring, 1 when it has none.
float SpringScale(const Panel& p, int id) {
    auto it = p.springs.find(id);
    return it == p.springs.end() ? 1.0f : it->second.scale;
}

// Advances every spring by dt seconds. A press starts a spring for the element
// under it; a spring that has settled back to 1 is dropped. Returns true while a
// spring is still moving.
bool StepSprings(Panel& p, float dt) {
    if (p.pressed != kHitNone && p.hovered == p.pressed && IsSpringKind(KindOf(p.pressed))) {
        p.springs.try_emplace(p.pressed);
    }
    // A drag keeps the dragged element pressed, wherever the pointer goes.
    if (p.libDragging && IsSpringKind(KindOf(p.dragPressId))) {
        p.springs.try_emplace(p.dragPressId);
    }
    bool active = false;
    for (auto it = p.springs.begin(); it != p.springs.end();) {
        PressSpring& s = it->second;
        const bool pressing = p.libDragging
                                  ? it->first == p.dragPressId
                                  : (p.pressed == it->first && p.hovered == it->first);
        const float target = pressing ? kPressScale : 1.0f;
        const float k = pressing ? kPressStiffness : kReleaseStiffness;
        const float zeta = pressing ? kPressDamping : kReleaseDamping;
        const float c = 2.0f * zeta * std::sqrt(k);  // damping coefficient
        const float h = 0.002f;  // fixed sub-step keeps the integration stable
        float t = dt;
        while (t > 0) {
            const float step = t < h ? t : h;
            const float acc = -k * (s.scale - target) - c * s.vel;
            s.vel += acc * step;
            s.scale += s.vel * step;
            t -= step;
        }
        const bool settled = std::fabs(s.scale - target) < 0.0005f &&
                             std::fabs(s.vel) < 0.01f;
        if (settled) {
            s.scale = target;
            s.vel = 0.0f;
        } else {
            active = true;
        }
        if (settled && !pressing) {
            it = p.springs.erase(it);
        } else {
            ++it;
        }
    }
    return active;
}

// Sends the panel's current transform to Direct2D.
void ApplyXf(Panel& p) {
    D2D1_MATRIX_3X2_F m = {};
    m.m11 = p.xf.s;
    m.m22 = p.xf.s;
    m.dx = p.xf.tx;
    m.dy = p.xf.ty;
    p.rt->SetTransform(&m);
}

// Reorder glide: an item drawn somewhere new slides there from where it was drawn last
// frame. Positions are passed scroll-free, so scrolling alone glides nothing. Draws
// without hits (exit of a tab, overlays) do not count.
Xf BeginMove(Panel& p, const std::wstring& key, int id, const D2D1_RECT_F& r) {
    const Xf prev = p.xf;
    if (!p.hitsOn) {
        return prev;
    }
    MoveAnim& m = p.moves[key];
    const ULONGLONG now = NowMs();
    const bool dropped = p.settle.active && id == p.settle.id;  // the dropped item glides by settle
    if (m.seen && !dropped && (m.lastRx != r.left || m.lastRy != r.top)) {
        // Start where it is drawn now, so a move during a glide carries on smoothly.
        const float visX = m.lastRx + m.ex.Value(now);
        const float visY = m.lastRy + m.ey.Value(now);
        m.ex.Jump(now, visX - r.left);
        m.ey.Jump(now, visY - r.top);
        m.ex.Start(now, 0.0f, 250, 0);
        m.ey.Start(now, 0.0f, 250, 0);
    }
    m.seen = true;
    m.lastRx = r.left;
    m.lastRy = r.top;
    p.xf = {prev.s, prev.tx + m.ex.Value(now), prev.ty + m.ey.Value(now)};
    ApplyXf(p);
    return prev;
}

// Offset (DIP) that is still left for the element `id` drawn in rectangle r. The
// first draw after the drop fixes its slot; the offset then shrinks to 0 along the curve.
void SettleOffset(Panel& p, int id, const D2D1_RECT_F& r, float& dx, float& dy) {
    dx = 0;
    dy = 0;
    Settle& s = p.settle;
    if (!s.active || id != s.id) {
        return;
    }
    const ULONGLONG now = NowMs();
    if (!s.placed) {
        if (now - s.tDrop > 250) {
            s.active = false;  // never drawn at its place: no glide
            return;
        }
        s.offX = s.settleX ? s.gx - (r.left + r.right) / 2 : 0;
        s.offY = s.settleY ? s.gy - (r.top + r.bottom) / 2 : 0;
        s.placed = true;
        s.t0 = now;
    }
    const float x = Clamp01((float)(now - s.t0) / kSettleMs);
    const float k = 1.0f - (float)CubicBezierY(0.05, 0.7, 0.1, 1.0, x);
    dx = s.offX * k;
    dy = s.offY * k;
    if (x >= 1.0f) {
        s.active = false;
    }
}

// Scales what is drawn next by the press spring of `id`, about the centre of `r`.
// Returns the transform before it; EndSpring restores it, so springs nest (the
// speaker badge of a tile scales on its own, on top of the tile's scale).
Xf BeginSpring(Panel& p, int id, const D2D1_RECT_F& r) {
    const Xf prev = p.xf;
    const float s = SpringScale(p, id);
    // The panel offsets are part of the drawn coordinates, so they are part of the centre.
    const float cx = (r.left + r.right) / 2 + p.ox;
    const float cy = (r.top + r.bottom) / 2 + p.oy;
    float dx = 0, dy = 0;
    SettleOffset(p, id, r, dx, dy);
    const Xf inner = {s, cx * (1 - s) + dx, cy * (1 - s) + dy};
    p.xf = {inner.s * prev.s, prev.s * inner.tx + prev.tx, prev.s * inner.ty + prev.ty};
    ApplyXf(p);
    return prev;
}

void EndSpring(Panel& p, const Xf& prev) {
    p.xf = prev;
    ApplyXf(p);
}

// Starts the glide of the element just dropped. `index` is where it is now, or where
// it came from if it was dropped outside. Its press spring moves along with it.
void BeginSettle(Panel& p, int kind, int index) {
    const int oldKey = p.dragPressId;
    const int newKey = MakeId(kind, index);
    if (oldKey != newKey) {
        auto it = p.springs.find(oldKey);
        if (it != p.springs.end()) {
            const PressSpring s = it->second;
            p.springs.erase(it);
            p.springs[newKey] = s;
        }
    }
    Settle& s = p.settle;
    s.active = true;
    s.placed = false;
    s.id = newKey;
    // Compact covers move in a grid, so they glide along both axes; full cards
    // only up and down, chips only sideways.
    s.settleX = kind == kHitTile || kind == kHitChip ||
                (kind == kHitMediaCard && g_mediaCompact);
    s.settleY = kind != kHitChip;
    s.gx = p.libDragX - p.grabX;
    s.gy = p.libDragY - p.grabY;
    s.tDrop = NowMs();
    s.offX = 0;
    s.offY = 0;
}

// DIP per device pixel of the panel (the drawing is scaled by ui).
inline float DipPerDevicePx(const Panel& p) {
    return 96.0f / (p.dpi * p.ui);
}

D2D1_COLOR_F SeverityColor(const Theme& th, NotifSeverity s) {
    switch (s) {
        case NotifSeverity::Success:
            return th.dark ? Col(0xFF6CCB5F) : Col(0xFF0F7B0F);
        case NotifSeverity::Warning:
            return th.dark ? Col(0xFFFCE100) : Col(0xFF9D5D00);
        case NotifSeverity::Error:
            return th.dark ? Col(0xFFFF99A4) : Col(0xFFC42B1C);
        default:
            return th.dark ? Col(0xFF60CDFF) : Col(0xFF005FB8);
    }
}

PCWSTR SeverityGlyph(NotifSeverity s) {
    switch (s) {
        case NotifSeverity::Success:
            return kGlyphCheck;
        case NotifSeverity::Warning:
            return L"\xE7BA";
        case NotifSeverity::Error:
            return L"\xE783";
        default:
            return L"\xE946";
    }
}

// Swipe direction is the way toward the edge the notes come from.
int SwipeDirFor(NotifPos pos) {
    switch (pos) {
        case NotifPos::TopLeft:
        case NotifPos::BottomLeft:
            return -1;
        case NotifPos::TopRight:
        case NotifPos::BottomRight:
            return 1;
        default:
            return 0;
    }
}

int EdgeDir() {
    return SwipeDirFor((NotifPos)g_notifPos.load()) < 0 ? -1 : 1;
}

void LayoutNotes(Panel& p) {
    const NotifPos pos = (NotifPos)g_notifPos.load();
    const bool top = pos == NotifPos::TopLeft || pos == NotifPos::TopRight ||
                     pos == NotifPos::Top;
    const bool left = pos == NotifPos::TopLeft || pos == NotifPos::BottomLeft;
    const bool center = pos == NotifPos::Top || pos == NotifPos::Bottom;
    const float x = center ? (kPanelW - kNoteW) / 2
                           : (left ? kPad : kPanelW - kPad - kNoteW);
    // Stacked from the chosen edge, the newest nearest to it. Every note has its own height.
    float used = 0;
    for (size_t i = p.notes.size(); i-- > 0;) {
        Note& n = p.notes[i];
        if (n.leaving) {
            continue;
        }
        n.x = x;
        n.targetY = top ? kTitleBarH + kPad + used : kPanelH - kPad - n.h - used;
        if (n.fresh) {
            n.y = n.targetY;
            n.fresh = false;
            TweenJump(n.yT, n.targetY);
        }
        used += n.h + kNoteGap;
    }
}

// Moves the notes one frame. Returns true while something still moves.
bool StepNotes(Panel& p, float dt) {
    if (p.notes.empty()) {
        return false;
    }
    LayoutNotes(p);
    const float ms = dt * 1000.0f;
    const float k = 1.0f - std::exp(-dt * 16.0f);
    const float away = kNoteW + 60.0f;
    const ULONGLONG now = NowMs();
    for (auto& n : p.notes) {
        // Hovering and swiping pause the auto hide.
        const bool hover = p.hovered == MakeId(kHitNote, n.id);
        if (!n.leaving && !hover && !n.dragging) {
            n.ttl -= ms;
            if (n.ttl <= 0) {
                n.leaving = true;
                n.leaveDir = EdgeDir();
            }
        }
        // A leaving note starts from where it is (also after a swipe) and accelerates out.
        if (n.leaving && !n.leaveStarted) {
            n.leaveStarted = true;
            n.leaveFrom = n.dragDx;
            TweenJump(n.leave, 0.0f);
            TweenSet(n.leave, 1.0f, 167, kEaseIn);
        }
        // The stack glides to the new places, decelerating (Fluent enter curve).
        if (!n.leaving && n.yT.to != n.targetY) {
            TweenSet(n.yT, n.targetY, 167, kEaseOut);
        }
        TweenStep(n.leave, now);
        TweenStep(n.enter, now);
        TweenStep(n.yT, now);
        n.y = n.yT.v;
        if (n.leaving) {
            n.dragDx = n.leaveFrom + (n.leaveDir * away - n.leaveFrom) * n.leave.v;
        } else if (!n.dragging) {
            n.dragDx += (0.0f - n.dragDx) * k;  // springs back after a swipe
        }
    }
    p.notes.erase(std::remove_if(p.notes.begin(), p.notes.end(),
                                 [](const Note& n) {
                                     return n.leaving && n.leaveStarted &&
                                            n.leave.dur == 0 && n.leave.v >= 1.0f;
                                 }),
                  p.notes.end());
    return true;  // keep frames coming so the auto hide keeps counting
}

void DrawNotes(Panel& p) {
    if (p.notes.empty()) {
        return;
    }
    const Theme& th = p.th;
    const int edge = SwipeDirFor((NotifPos)g_notifPos.load());
    const float saveAlpha = p.alpha;
    const float saveOx = p.ox;
    const float saveOy = p.oy;
    for (const Note& n : p.notes) {
        const float e = n.enter.v;
        const float x = n.x + n.dragDx + edge * (1.0f - e) * (kNoteW + 24.0f);
        float fade = e;
        if (n.leaving) {
            fade *= 1.0f - n.leave.v;
        }
        AddHit(p, MakeId(kHitNote, n.id), Rc(x, n.y, kNoteW, n.h));
        if (fade <= 0.01f) {
            continue;
        }
        p.alpha = saveAlpha * fade;
        p.ox = x;
        p.oy = n.y;
        FillRR(p, Rc(0, 0, kNoteW, n.h), kRadiusOverlay, th.menuBg);
        StrokeRR(p, Rc(0, 0, kNoteW, n.h), kRadiusOverlay, th.menuBorder);
        if (n.icon) {
            DrawBmp(p, n.icon, Rc(12, 11, 28, 28));
        } else {
            DrawTxt(p, n.glyph ? n.glyph : SeverityGlyph(n.sev), p.fIconBig.p,
                    Rc(12, 1, 36, n.h), SeverityColor(th, n.sev));
        }
        DrawTxt(p, n.title, p.fName.p, Rc(56, 10, kNoteW - 70, 20), th.text);
        DrawTxt(p, n.message, p.fBodyWrap.p, Rc(56, 30, kNoteW - 70, n.h - 36),
                th.textSecondary);
    }
    p.alpha = saveAlpha;
    p.ox = saveOx;
    p.oy = saveOy;
}

// Shows a notification inside the panel.
// Height of a note: the title line, then the message wrapped to the text width.
float NoteHeight(Panel& p, const std::wstring& message) {
    const float bodyH = message.empty() ? 0.0f : MeasureH(message, p.fBodyWrap.p, kNoteW - 70.0f);
    return (std::min)(kNoteMaxH, (std::max)(kNoteMinH, 30.0f + bodyH + 12.0f));
}

// Icon of a running PWA window, for the notes about it.
ID2D1Bitmap* WindowBitmap(Panel& p, HWND hwnd) {
    for (const AppInfo& a : p.all) {
        if (a.hwnd == hwnd) {
            return AppBitmap(p, a);
        }
    }
    return nullptr;
}

void PushNote(Panel& p,
              NotifSeverity sev,
              const std::wstring& title,
              const std::wstring& message,
              PCWSTR glyph = nullptr,
              ID2D1Bitmap* icon = nullptr) {
    if (g_panelMode == (int)PanelMode::Hermes) {
        return;  // Hermes shows no notifications
    }
    if (!g_showNotes) {
        return;  // notes are switched off in Settings
    }
    size_t live = 0;
    Note* oldest = nullptr;
    for (auto& n : p.notes) {
        if (!n.leaving) {
            ++live;
            if (!oldest) {
                oldest = &n;
            }
        }
    }
    if (live >= kNoteMax && oldest) {
        oldest->leaving = true;
        oldest->leaveDir = EdgeDir();
    }

    Note n;
    n.id = ++p.noteSeq;
    n.sev = sev;
    n.title = title;
    n.message = message;
    n.swipeDir = SwipeDirFor((NotifPos)g_notifPos.load());
    n.glyph = glyph;
    n.h = NoteHeight(p, message);
    n.icon = icon;
    TweenJump(n.enter, 0.0f);
    TweenSet(n.enter, 1.0f, 167, kEaseOut);
    p.notes.push_back(n);
    StartAnim(p);
}

// Steps every running animation. Returns true if another frame is needed.
bool UpdateAnims(Panel& p, ULONGLONG now) {
    bool active = false;
    active |= TweenStep(p.openDy, now);
    active |= TweenStep(p.openAlpha, now);
    active |= TweenStep(p.slideX, now);
    active |= TweenStep(p.slideA, now);
    active |= TweenStep(p.menuT, now);
    active |= TweenStep(p.menuDy, now);
    active |= TweenStep(p.exitX, now);
    active |= TweenStep(p.exitA, now);
    active |= p.indL.Running(now) || p.indR.Running(now);
    active |= TweenStep(p.closeT, now);
    for (auto& kv : p.moves) {
        active |= kv.second.ex.Running(now) || kv.second.ey.Running(now);
    }
    active |= TweenStep(p.barA, now);
    for (int i = 0; i < kTabCount; ++i) {
        active |= TweenStep(p.tabPos[i], now);
    }
    for (auto& kv : p.toggleT) {
        active |= TweenStep(kv.second, now);
    }
    for (auto it = p.hoverT.begin(); it != p.hoverT.end();) {
        bool run = TweenStep(it->second, now);
        active |= run;
        if (!run && it->second.v <= 0.001f && it->first != p.hovered) {
            it = p.hoverT.erase(it);
        } else {
            ++it;
        }
    }

    float dt = p.lastFrame ? (float)(now - p.lastFrame) / 1000.0f : 0.016f;
    p.lastFrame = now;
    if (dt > 0.05f) {
        dt = 0.05f;
    }
    active |= StepSprings(p, dt);
    active |= p.settle.active;
    active |= StepNotes(p, dt);
    if (!p.hlKey.empty() && now - p.hlTick < 1450) {
        active = true;
    }
    // Scrolling follows its target along the Fluent decelerate curve, 167 ms. A new
    // target starts from where the list is now, so repeated wheel notches blend.
    {
        const float maxChip = (std::max)(0.0f, p.chipContentW - kPanelW);
        p.chipScrollT = (std::max)(0.0f, (std::min)(p.chipScrollT, maxChip));
        if (p.chipTw.to != p.chipScrollT) {
            TweenSet(p.chipTw, p.chipScrollT, 167, kEaseOut);
        }
        active |= TweenStep(p.chipTw, now);
        p.chipScroll = p.chipTw.v;
    }
    for (int i = 0; i < kTabCount; ++i) {
        ClampScroll(p, i);
        Tween& tw = p.scrollTw[i];
        if (tw.to != p.scrollT[i]) {
            TweenSet(tw, p.scrollT[i], 167, kEaseOut);
        }
        active |= TweenStep(tw, now);
        p.scrollC[i] = tw.v;
    }
    return active;
}

// ---------------------------------------------------------------------------
// Panel: drawing of the tabs and their contents
// ---------------------------------------------------------------------------

void DrawEmpty(Panel& p, PCWSTR glyph, PCWSTR text, float top, float bottom) {
    const Theme& t = p.th;
    float cy = (top + bottom) / 2;
    DrawTxt(p, glyph, p.fIconHuge.p, Rc(0, cy - 44, kPanelW, 44),
            WithAlpha(t.textSecondary, 0.4f));
    DrawTxt(p, text, p.fCenter.p, Rc(0, cy + 6, kPanelW, 22),
            WithAlpha(t.textSecondary, 0.7f));
}

void DrawTitleBar(Panel& p) {
    const Theme& t = p.th;
    
    // Background - use card background for title bar
    FillRect(p, Rc(0, 0, kPanelW, kTitleBarH), t.card);
    
    // Left side: App icon + title
    float iconSize = 16.0f;
    float iconY = (kTitleBarH - iconSize) / 2;
    DrawTxt(p, kGlyphAllApps, p.fIcon.p, Rc(kPad, iconY, iconSize, iconSize), t.accent);
    
    float titleX = kPad + iconSize + 8.0f;
    const std::wstring titleText = L"PWA Tray Center";
    float titleW = MeasureW(titleText, p.fBody.p);
    DrawTxt(p, titleText, p.fBody.p, Rc(titleX, 0, titleW + 4, kTitleBarH), t.text);
    
    // Version text right after the title, with a small gap
    std::wstring versionText = kAppVersion;
    float versionX = titleX + titleW + 4.0f;
    DrawTxt(p, versionText.c_str(), p.fSmall.p, Rc(versionX, 0, 60, kTitleBarH), 
            WithAlpha(t.textSecondary, 0.55f));
    
    // Right side: Hidden count in pill badge + close button
    size_t n = p.hiddenIdx.size();
    std::wstring hiddenText =
        n == 0 ? std::wstring(L"\x00AF\\_(\x30C4)_/\x00AF") : std::to_wstring(n) + (n == 1 ? L" hidden PWA" : L" hidden PWAs");
    
    float pillW = (std::max)(64.0f, MeasureW(hiddenText, p.fBadge.p) + 16.0f);
    float pillH = 18.0f;
    float pillX = kPanelW - kCloseButtonW - pillW - 8.0f;
    float pillY = (kTitleBarH - pillH) / 2;
    D2D1_RECT_F hiddenPill = Rc(pillX, pillY, pillW, pillH);
    
    // Add hit area for hidden badge (clickable button)
    int hiddenId = MakeId(kHitHiddenBadge, 0);
    AddHit(p, hiddenId, hiddenPill);
    
    // Draw hidden count pill badge (Win11 style) with hover effect
    const Xf prevPill = BeginSpring(p, hiddenId, hiddenPill);
    FillRR(p, hiddenPill, 9.0f, WithAlpha(t.textSecondary, 0.15f));
    DrawHoverBg(p, hiddenId, hiddenPill, 9.0f);  // Fluent hover effect
    DrawTxt(p, hiddenText.c_str(), p.fBadge.p, hiddenPill, WithAlpha(t.textSecondary, 0.75f));
    EndSpring(p, prevPill);
    
    // Close button on the far right
    D2D1_RECT_F closeRect = Rc(kPanelW - kCloseButtonW, 0, kCloseButtonW, kTitleBarH);
    
    // Windows 11 Fluent Design close button colors
    // Hover fades the red in and out over 150 ms. The button draws no glyph.
    const float ch = p.closeT.v;
    if (ch > 0.0f) {
        D2D1_COLOR_F closeBg = p.closePressed ? Col(0xFFA02010) : Col(0xFFC42B1C);
        FillRect(p, closeRect, WithAlpha(closeBg, ch));
    }
    
    // Bottom border divider
    DrawLine(p, 0, kTitleBarH, kPanelW, kTitleBarH, t.divider);
}

void DrawTabs(Panel& p) {
    const Theme& t = p.th;
    const float tw = TabW();

    int drawOrder[kTabCount];
    int n = 0;
    for (int i = 0; i < kTabCount; ++i) {
        if (p.order[i] != p.dragTab) {
            drawOrder[n++] = p.order[i];
        }
    }
    if (p.dragTab >= 0) {
        drawOrder[n++] = p.dragTab;  // on top
    }

    for (int k = 0; k < kTabCount; ++k) {
        int tab = drawOrder[k];
        float x = p.tabPos[tab].v;
        D2D1_RECT_F r = Rc(x, kTabsY, tw, kTabsH);
        int id = MakeId(kHitTab, tab);
        AddHit(p, id, r);
        const Xf prevTab = BeginSpring(p, id, r);
        if (p.dragTab == tab && p.dragging) {
            FillRR(p, r, kRadiusCtl, t.hover);
        } else {
            DrawHoverBg(p, id, r, kRadiusCtl);
        }

        bool sel = tab == p.selectedTab;
        D2D1_COLOR_F c = sel ? t.text : t.textSecondary;
        DrawTxt(p, kTabDefs[tab].glyph, p.fIcon.p, Rc(x, kTabsY + 6, tw, 24), c);
        DrawTxt(p, kTabDefs[tab].label, p.fTab.p, Rc(x, kTabsY + 30, tw, 16), c);

        // A dot on the icon: something is playing / there are notifications.
        bool dot = false;
        if (tab == kTabMedia) {
            for (const auto& m : p.media) {
                dot |= m.playing;
            }
        } else if (tab == kTabNotifs) {
            dot = !p.rows.empty();
        }
        if (dot) {
            FillCircle(p, x + tw / 2 + 10, kTabsY + 10, 3.5f, t.accent);
        }
        EndSpring(p, prevTab);
    }

    // Selected tab indicator.
    const ULONGLONG nowI = NowMs();
    const float indLeft = p.indL.Value(nowI);
    const float indRight = p.indR.Value(nowI);
    FillRR(p, Rc(indLeft, kTabsY + kTabsH - 4, indRight - indLeft, 3), 1.5f, t.accent);
}

// Highlight after a jump from the PWAs tab: short wait, 400 ms light up, 100 ms fade out.
float MediaHighlight(const Panel& p, const std::wstring& key) {
    if (p.hlKey.empty() || key != p.hlKey) {
        return 0.0f;
    }
    const long long t = (long long)(NowMs() - p.hlTick);
    if (t < 150) {
        return 0.0f;
    }
    if (t < 550) {
        return (float)(t - 150) / 400.0f;
    }
    if (t < 1250) {
        return 1.0f;
    }
    if (t < 1350) {
        return 1.0f - (float)(t - 1250) / 100.0f;
    }
    return 0.0f;
}

// Draws one line in a box. Text that does not fit scrolls slowly: a pause at
// the start of every loop, then a steady move. Two copies with a gap make the
// loop seamless. Text that fits stays still.
void DrawScrollingLine(Panel& p, const std::wstring& s, IDWriteTextFormat* fmt,
                       const D2D1_RECT_F& box, const D2D1_COLOR_F& c,
                       bool moving) {
    if (s.empty()) {
        return;
    }
    const float boxW = box.right - box.left;
    const float textW = MeasureW(s, fmt);
    if (textW <= boxW) {
        DrawTxt(p, s, fmt, box, c);
        return;
    }
    const float gap = 40.0f;        // space between two copies of the text
    const float speed = 30.0f;      // DIP per second
    const double pauseMs = 1500.0;  // rest at the start of every loop
    const double step = textW + gap;
    const double cycleMs = pauseMs + step / speed * 1000.0;
    const double phase = std::fmod((double)GetTickCount64(), cycleMs);
    const float offset =
        !moving || phase < pauseMs ? 0.0f
                                   : (float)((phase - pauseMs) / 1000.0 * speed);

    if (moving) {
        p.animating = true;  // keep frames coming while the text moves
    }
    const float h = box.bottom - box.top;
    PushClip(p, Off(p, box));
    DrawTxt(p, s, fmt, Rc(box.left - offset, box.top, textW + 4.0f, h), c);
    DrawTxt(p, s, fmt,
            Rc(box.left - offset + (float)step, box.top, textW + 4.0f, h), c);
    PopClip(p);
}

// Progress bar (Material 3 Expressive, Pixel style). The played part is a sine wave
// that travels along the track, the rest is a straight line. Both stop a little short
// of a thick vertical separator that marks the position. The wave flattens near 0%
// and 100%, and it stops while the track is paused.
constexpr float kWaveLength = 24.0f;   // DIP per wave
constexpr float kWaveAmp = 0.0f;       // half height, DIP (0 = straight bar)
constexpr float kWaveStroke = 6.0f;    // DIP, the bar's thickness
constexpr float kSeparatorW = 5.0f;    // DIP, the vertical separator
constexpr float kSeparatorH = 20.0f;   // DIP
constexpr float kSeparatorGap = 5.0f;  // visible gap between the bar and the separator
constexpr float kSeparatorDotR = 3.0f;  // DIP, radius of the solid dot that replaces the separator in the compact view
constexpr float kSeparatorDotGap = 2.0f;  // DIP, gap on each side of that dot

// Strokes an open polyline with round caps. Points are in panel coordinates.
void StrokePolyline(Panel& p, const std::vector<D2D1_POINT_2F>& pts,
                    const D2D1_COLOR_F& c, ID2D1StrokeStyle* style) {
    if (pts.size() < 2) {
        return;
    }
    ID2D1PathGeometry* geom = nullptr;
    if (FAILED(g_d2d->CreatePathGeometry(&geom))) {
        return;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (SUCCEEDED(geom->Open(&sink))) {
        sink->BeginFigure(pts[0], D2D1_FIGURE_BEGIN_HOLLOW);
        for (size_t i = 1; i < pts.size(); ++i) {
            sink->AddLine(pts[i]);
        }
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();
        sink->Release();
        SetBrush(p, c);
        p.rt->DrawGeometry(geom, p.brush.p, kWaveStroke, style);
    }
    geom->Release();
}

// `track` is the bar's rectangle in panel coordinates before the scroll offset;
// its vertical centre is the bar's centre line.
void DrawWavyTrack(Panel& p, const D2D1_RECT_F& track, double frac, bool playing,
                   bool separator, bool dot = false) {
    const float cy = (track.top + track.bottom) / 2.0f;
    const float left = track.left;
    const float right = track.right;
    const float xEnd = left + (right - left) * (float)frac;

    // The wave is static: its phase does not move, so no frames are requested for it.

    // Full height in the middle, flat near both ends.
    const double edge = (std::min)(frac / 0.1, (1.0 - frac) / 0.05);
    const float amp = kWaveAmp * (float)Clamp01(edge);

    // Both parts stop short of the separator: the distance from its centre to the
    // end of each part is half its width, the gap, and the round cap of the bar.
    const float reach = (dot ? kSeparatorDotR + kSeparatorDotGap
                             : kSeparatorW / 2.0f + kSeparatorGap) +
                        kWaveStroke / 2.0f;
    const float xA = xEnd - reach;  // end of the played part
    const float xB = xEnd + reach;  // start of the unplayed part

    D2D1_STROKE_STYLE_PROPERTIES sp = {};
    sp.startCap = D2D1_CAP_STYLE_ROUND;
    sp.endCap = D2D1_CAP_STYLE_ROUND;
    sp.dashCap = D2D1_CAP_STYLE_ROUND;
    sp.lineJoin = D2D1_LINE_JOIN_ROUND;
    ID2D1StrokeStyle* style = nullptr;
    if (FAILED(g_d2d->CreateStrokeStyle(&sp, nullptr, 0, &style))) {
        return;
    }

    // Played part: the wave, from the start of the track to the separator.
    std::vector<D2D1_POINT_2F> wave;
    for (float x = left; x < xA; x += 1.5f) {
        const float phase = x - left;
        wave.push_back({x + p.ox,
                        cy + p.oy + amp * (float)std::sin(2.0 * 3.14159265 * phase / kWaveLength)});
    }
    if (xA > left) {
        const float phase = xA - left;
        wave.push_back({xA + p.ox,
                        cy + p.oy + amp * (float)std::sin(2.0 * 3.14159265 * phase / kWaveLength)});
    }
    StrokePolyline(p, wave, p.th.accent, style);

    // Unplayed part: a straight line.
    if (xB < right) {
        StrokePolyline(p, {{xB + p.ox, cy + p.oy}, {right + p.ox, cy + p.oy}}, p.th.track,
                       style);
    }
    style->Release();

    // The thick separator: always visible while the track can be seeked (Pixel style).
    if (separator) {
        if (dot) {
            FillCircle(p, xEnd, cy, kSeparatorDotR, p.th.accent);
        } else {
            FillRR(p, Rc(xEnd - kSeparatorW / 2.0f, cy - kSeparatorH / 2.0f, kSeparatorW,
                         kSeparatorH),
                   kSeparatorW / 2.0f, p.th.accent);
        }
    }
}

// Play / pause button of a media card. The same in both views.
void DrawPlayButton(Panel& p, int i, const MediaInfo& m, const D2D1_RECT_F& rPlay) {
    const Theme& t = p.th;
    const int idPlay = MakeId(kHitMediaPlay, i);
    // Play / pause: accent filled circle.
    const float cx = (rPlay.left + rPlay.right) / 2;
    const float cy = (rPlay.top + rPlay.bottom) / 2;
    D2D1_COLOR_F fill = t.accent;
    if (!m.canToggle) {
        fill = t.track;
    } else {
        AddHit(p, idPlay, rPlay);
        if (p.hovered == idPlay && p.pressed == idPlay) {
            fill = t.accentPress;
        } else if (p.hovered == idPlay) {
            fill = t.accentHover;
        }
    }
    const Xf prev = BeginSpring(p, idPlay, rPlay);
    FillCircle(p, cx, cy, 20.0f, fill);
    DrawTxt(p, m.playing ? kGlyphPause : kGlyphPlay, p.fIconBig.p, rPlay, t.onAccent);
    EndSpring(p, prev);
}

// Previous / next button of a media card. Its look is the same as before (hover
// fill with the fade); it has the press spring of play / pause.
void DrawSkipButton(Panel& p, int i, int kind, const D2D1_RECT_F& r, PCWSTR glyph,
                    bool enabled) {
    const Theme& t = p.th;
    const int id = MakeId(kind, i);
    if (enabled) {
        AddHit(p, id, r);
    }
    const Xf prev = BeginSpring(p, id, r);
    if (enabled) {
        DrawHoverBg(p, id, r, kRadiusCtl);
    }
    DrawTxt(p, glyph, p.fIcon.p, r, enabled ? t.text : WithAlpha(t.textSecondary, 0.5f));
    EndSpring(p, prev);
}

ID2D1Bitmap* MediaSourceBitmap(Panel& p, const MediaInfo& m);

// Normal card: the cover and text, then the transport buttons.
constexpr float kMediaCardH = 172.0f;
// Opacity of the cover that fills a media card (1 = opaque).
constexpr float kCardCoverOpacity = 0.7f;

// Draws a picture over a rounded rectangle: the picture fills the rectangle (centre
// crop, no stretching) at the given opacity. The mask follows the panel's transform,
// so a pressed card shrinks its cover with it.
void DrawCoverFill(Panel& p, ID2D1Bitmap* bmp, const D2D1_RECT_F& r, float radius,
                   float opacity) {
    if (!bmp) {
        return;
    }
    // The press spring scales the panel's drawing, so the rectangle is worked out in
    // device space here. The layer and the picture are then drawn with no transform,
    // so the mask and the picture always have the same size.
    const D2D1_RECT_F o = Off(p, r);
    const float s = p.xf.s;
    const D2D1_RECT_F dst = {o.left * s + p.xf.tx, o.top * s + p.xf.ty,
                             o.right * s + p.xf.tx, o.bottom * s + p.xf.ty};
    const float w = dst.right - dst.left;
    const float h = dst.bottom - dst.top;
    if (w <= 0 || h <= 0) {
        return;
    }

    D2D1_MATRIX_3X2_F identity = {};
    identity.m11 = 1.0f;
    identity.m22 = 1.0f;
    p.rt->SetTransform(&identity);

    D2D1_ROUNDED_RECT rr = {dst, radius * s, radius * s};
    ID2D1RoundedRectangleGeometry* mask = nullptr;
    if (SUCCEEDED(g_d2d->CreateRoundedRectangleGeometry(&rr, &mask))) {
        D2D1_LAYER_PARAMETERS lp = {};
        lp.contentBounds = D2D1_RECT_F{-1e6f, -1e6f, 1e6f, 1e6f};
        lp.geometricMask = mask;
        lp.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
        lp.maskTransform = identity;
        lp.opacity = p.alpha * opacity;
        lp.layerOptions = D2D1_LAYER_OPTIONS_NONE;
        p.rt->PushLayer(&lp, nullptr);

        // Centre crop of the square picture, so that it covers the whole rectangle.
        const D2D1_SIZE_F sz = bmp->GetSize();
        float srcW = sz.width;
        float srcH = sz.height;
        if (w / h > sz.width / sz.height) {
            srcH = sz.width * h / w;
        } else {
            srcW = sz.height * w / h;
        }
        const D2D1_RECT_F src = {(sz.width - srcW) / 2, (sz.height - srcH) / 2,
                                 (sz.width + srcW) / 2, (sz.height + srcH) / 2};
        // Cubic filter (16 samples) for the stretch; the layer carries the opacity.
        // The transform maps the cropped part of the picture onto the card.
        const float k = w / srcW;
        D2D1_MATRIX_3X2_F scale = {};
        scale.m11 = k;
        scale.m22 = k;
        scale.dx = dst.left;
        scale.dy = dst.top;
        p.rt->SetTransform(&scale);
        const D2D1_POINT_2F at = {0.0f, 0.0f};
        p.dc->DrawImage(bmp, &at, &src, D2D1_INTERPOLATION_MODE_CUBIC,
                        D2D1_COMPOSITE_MODE_SOURCE_OVER);
        p.rt->SetTransform(&identity);
        p.rt->PopLayer();
        mask->Release();
    }

    ApplyXf(p);  // back to the panel's transform for what is drawn next
}

void DrawMediaCard(Panel& p, int i, const MediaInfo& m, float y) {
    const Theme& t = p.th;
    const float W = kPanelW;
    D2D1_RECT_F card = Rc(kPad, y, W - 2 * kPad, kMediaCardH);
    // Added first, so the buttons on top of it win the hit test.
    AddHit(p, MakeId(kHitMediaCard, i), card);
    const Xf prevCard = BeginSpring(p, MakeId(kHitMediaCard, i), card);
    FillRR(p, card, kRadiusCtl, t.card);
    // Cover style: the cover fills the card, slightly transparent, under the text and
    // the controls; or it is a small square on the left.
    {
        auto it = p.covers.find(m.key);
        const bool hasCover = it != p.covers.end() && it->second.bmp;
        if (g_cardCover == 1) {
            if (hasCover) {
                DrawCoverFill(p, it->second.bmp, card, kRadiusCtl, kCardCoverOpacity);
            } else {
                DrawTxt(p, L"\xE90B", p.fIconHuge.p, card, WithAlpha(t.textSecondary, 0.25f));
            }
        } else if (g_cardCover == 0) {
            const D2D1_RECT_F small = Rc(kPad + 12, y + 12, 64, 64);
            if (hasCover) {
                DrawBmp(p, it->second.bmp, small);
            } else {
                FillRR(p, small, kRadiusCtl, t.hover);
                DrawTxt(p, L"\xE90B", p.fIconBig.p, small, t.textSecondary);
            }
        }
    }
    StrokeRR(p, card, kRadiusCtl, t.cardBorder);
    const float hl = MediaHighlight(p, m.key);
    if (hl > 0.0f) {
        FillRR(p, card, kRadiusCtl, WithAlpha(t.accent, 0.25f * hl));
        StrokeRR(p, card, kRadiusCtl, WithAlpha(t.accent, hl));
        StrokeRR(p, card, kRadiusCtl, WithAlpha(t.accent, hl));
    }

    float L = kPad + 12;
    float tx = g_cardCover ? L : L + 76;
    float tw = W - kPad - 12 - tx;
    DrawScrollingLine(p, m.title, p.fName.p, Rc(tx, y + 12, tw, 22), t.text, m.playing);
    DrawScrollingLine(p, m.artist, p.fBody.p, Rc(tx, y + 34, tw, 18), t.textSecondary,
                      m.playing);

    // Source: the small icon of the PWA, then its name.
    const float srcY = y + 54;
    if (ID2D1Bitmap* icon = MediaSourceBitmap(p, m)) {
        DrawBmp(p, icon, Rc(tx, srcY + 1, 14, 14), 0.8f);
    }
    DrawTxt(p, MediaSourceName(p, m), p.fSmall.p, Rc(tx + 18, srcY, tw - 18, 16),
            WithAlpha(t.textSecondary, 0.8f));

    // Progress bar: redrawn on the panel's timer tick while a track plays.
    float trackW = W - 2 * kPad - 24;
    D2D1_RECT_F track = Rc(L, y + 92, trackW, 4);
    if ((size_t)i < p.trackRects.size()) {
        p.trackRects[i] = track;
    }
    long long dur = m.durMs;
    long long pos = CurrentPosMs(m);
    bool seeking = p.seekingIdx == i;
    double frac = seeking ? p.seekFrac
                          : (dur > 0 ? Clamp01((double)pos / dur) : 0.0);
    if (seeking && dur > 0) {
        pos = (long long)(p.seekFrac * dur);
    }
    int seekId = MakeId(kHitMediaSeek, i);
    if (m.canSeek) {
        AddHit(p, seekId, Rc(L, y + 84, trackW, 20));
    }
    DrawWavyTrack(p, track, frac, m.playing, m.canSeek);
    DrawTxt(p, FormatTime(pos), p.fSmall.p, Rc(L, y + 104, 70, 16), t.textSecondary);
    if (dur > 0) {
        DrawTxt(p, FormatTime(dur), p.fTimeRight.p,
                Rc(L + trackW - 70, y + 104, 70, 16), t.textSecondary);
    }

    // Transport buttons.
    float x0 = (W - 144) / 2;
    D2D1_RECT_F rPrev = Rc(x0, y + 124, 40, 40);
    D2D1_RECT_F rPlay = Rc(x0 + 52, y + 124, 40, 40);
    D2D1_RECT_F rNext = Rc(x0 + 104, y + 124, 40, 40);
    DrawSkipButton(p, i, kHitMediaPrev, rPrev, kGlyphPrev, m.canPrev);
    DrawSkipButton(p, i, kHitMediaNext, rNext, kGlyphNext, m.canNext);
    DrawPlayButton(p, i, m, rPlay);
    EndSpring(p, prevCard);
}

// Chrome volume bar, pinned to the bottom of the Media tab.
void DrawVolumeBar(Panel& p, float y) {
    const Theme& t = p.th;
    const float W = kPanelW;
    D2D1_RECT_F bar = Rc(kPad, y, W - 2 * kPad, kVolBarH);
    FillRR(p, bar, kRadiusCtl, t.card);
    StrokeRR(p, bar, kRadiusCtl, t.cardBorder);

    float savedAlpha = p.alpha;
    if (!p.volKnown) {
        p.alpha *= 0.6f;  // the browser has no audio session yet
    }
    double shown = p.volMute ? 0.0 : p.volFrac;

    int idMute = MakeId(kHitVolMute, 0);
    int idVol = MakeId(kHitVolSeek, 0);
    D2D1_RECT_F rMute = Rc(kPad + 8, y + 8, 32, 32);
    AddHit(p, idMute, rMute);
    DrawHoverBg(p, idMute, rMute, kRadiusCtl);
    PCWSTR vg = shown <= 0.0 ? kGlyphVolMute
                : (shown < 0.34 ? kGlyphVol1
                                : (shown < 0.67 ? kGlyphVol2 : kGlyphVol3));
    DrawTxt(p, vg, p.fIcon.p, rMute, t.text);

    float x = kPad + 50;
    float w = W - kPad - 54 - x;
    DrawTxt(p, L"Chrome", p.fSmall.p, Rc(x, y + 5, 120, 14),
            t.textSecondary);
    D2D1_RECT_F track = Rc(x, y + 28, w, 4);
    p.volTrack = track;
    AddHit(p, idVol, Rc(x - 6, y + 14, w + 12, 28));
    FillRR(p, track, 2.0f, t.track);
    if (shown > 0) {
        D2D1_RECT_F fill = track;
        fill.right = fill.left + (float)(w * shown);
        FillRR(p, fill, 2.0f, t.accent);
    }
    if (p.hovered == idVol || p.volDrag) {
        float cx = track.left + (float)(w * shown);
        float cy = track.top + 2;
        FillCircle(p, cx, cy, 9.0f, t.dark ? Col(0xFF454545) : Col(0xFFFFFFFF));
        FillCircle(p, cx, cy, p.volDrag ? 4.0f : 5.0f, t.accent);
    }
    DrawTxt(p, std::to_wstring((int)std::lround(shown * 100.0)) + L"%",
            p.fTimeRight.p, Rc(W - kPad - 52, y, 44, kVolBarH),
            t.textSecondary);
    p.alpha = savedAlpha;
}

// Compact view, on hover: elapsed time on the left, remaining on the right, and
// the normal seekable progress bar along the bottom of the cover.
void DrawCoverProgress(Panel& p, int i, const MediaInfo& m, const D2D1_RECT_F& r) {
    const Theme& t = p.th;
    const float trackW = (r.right - r.left) - 12.0f;
    const float L = r.left + 6.0f;
    const D2D1_RECT_F track = Rc(L, r.bottom - 12.0f, trackW, 4.0f);
    if ((size_t)i < p.trackRects.size()) {
        p.trackRects[i] = track;
    }
    const long long dur = m.durMs;
    long long pos = CurrentPosMs(m);
    const bool seeking = p.seekingIdx == i;
    const double frac =
        seeking ? p.seekFrac : (dur > 0 ? Clamp01((double)pos / dur) : 0.0);
    if (seeking && dur > 0) {
        pos = (long long)(p.seekFrac * dur);
    }
    const int seekId = MakeId(kHitMediaSeek, i);
    if (m.canSeek) {
        AddHit(p, seekId, Rc(L, track.top - 8.0f, trackW, 20.0f));
    }
    const D2D1_COLOR_F labelCol = t.text;
    const float labelTop = r.bottom - 30.0f;
    DrawTxt(p, FormatTime(pos), p.fSmall.p,
            Rc(L, labelTop, trackW / 2, 14.0f), labelCol);
    if (dur > 0) {
        DrawTxt(p, L"-" + FormatTime((std::max)(0LL, dur - pos)), p.fTimeRight.p,
                Rc(L + trackW / 2, labelTop, trackW / 2, 14.0f), labelCol);
    }
    DrawWavyTrack(p, track, frac, m.playing, m.canSeek, true);
}

// Compact view: only the cover, with the normal card's border. Hover darkens it
// and shows the play/pause button.
void DrawMediaCover(Panel& p, int i, const MediaInfo& m, const D2D1_RECT_F& r) {
    const Theme& t = p.th;
    const int cardId = MakeId(kHitMediaCard, i);
    const int playId = MakeId(kHitMediaPlay, i);
    const int seekId = MakeId(kHitMediaSeek, i);
    AddHit(p, cardId, r);  // right click and hover target
    const Xf prevCover = BeginSpring(p, cardId, r);

    // One fade for the whole tile: on while the cursor is over the tile, its button
    // or its progress bar. Moving between them keeps it steady.
    const int coverId = MakeId(kHitMediaCover, i);
    const bool over =
        p.hovered == cardId || p.hovered == playId || p.hovered == seekId;
    Tween& fade = p.hoverT[coverId];
    if ((fade.to > 0.5f) != over) {
        TweenSet(fade, over ? 1.0f : 0.0f, 83, kEaseLinear);
    }
    if (fade.dur > 0) {
        p.animating = true;
    }
    const float hv = fade.v;
    const float baseAlpha = p.alpha;

    // The cover fades out, a grey panel fades in under it.
    p.alpha = baseAlpha * (1.0f - hv);
    auto ce = p.covers.find(m.key);
    if (ce != p.covers.end() && ce->second.bmp && g_cardCover != 2) {
        DrawBmp(p, ce->second.bmp, r);
    } else {
        FillRR(p, r, kRadiusCtl, t.hover);
        DrawTxt(p, L"\xE90B", p.fIconBig.p, r, t.textSecondary);
    }
    p.alpha = baseAlpha;
    StrokeRR(p, r, kRadiusCtl, t.cardBorder);

    if (hv > 0.0f) {
        const D2D1_COLOR_F grey = t.dark ? Col(0xFF383838) : Col(0xFFE0E0E0);
        p.alpha = baseAlpha * hv;
        FillRR(p, r, kRadiusCtl, grey);
        // Title at the top, the artist under it in a smaller font. Both scroll
        // when they are too long for the cover.
        const float textW = (r.right - r.left) - 16;
        DrawScrollingLine(p, m.title, p.fBody.p,
                          Rc(r.left + 8, r.top + 6, textW, 16), t.text, m.playing);
        DrawScrollingLine(p, m.artist, p.fSmall.p,
                          Rc(r.left + 8, r.top + 22, textW, 14),
                          WithAlpha(t.textSecondary, 0.9f), m.playing);
        if (m.canToggle) {
            const float cx = (r.left + r.right) / 2;
            const float cy = (r.top + r.bottom) / 2;
            DrawPlayButton(p, i, m, Rc(cx - 20, cy - 20, 40, 40));
        }
        p.alpha = baseAlpha;
    }

    const float showA = p.seekingIdx == i ? 1.0f : hv;
    if (showA > 0.0f) {
        p.alpha = baseAlpha * showA;
        DrawCoverProgress(p, i, m, r);
        p.alpha = baseAlpha;
    }

    // Jump highlight, same as the normal card: accent frame and a light tint.
    const float hl = MediaHighlight(p, m.key);
    if (hl > 0.0f) {
        FillRR(p, r, kRadiusCtl, WithAlpha(t.accent, 0.25f * hl));
        StrokeRR(p, r, kRadiusCtl, WithAlpha(t.accent, hl));
        StrokeRR(p, r, kRadiusCtl, WithAlpha(t.accent, hl));
    }
    EndSpring(p, prevCover);
}

void DrawMediaTab(Panel& p, float top, float bottom) {
    const float viewBottom =
        g_showVolume ? bottom - kVolBarH - 8.0f : bottom;
    p.viewH[kTabMedia] = viewBottom - top;
    p.trackRects.assign(p.media.size(), D2D1_RECT_F{});

    D2D1_RECT_F clip = Rc(0, top, kPanelW, viewBottom - top);
    PushClip(p, clip);
    D2D1_RECT_F saveClip = p.hitClip;
    p.hitClip = clip;
    if (p.media.empty()) {
        p.contentH[kTabMedia] = 0;
        DrawEmpty(p, kTabDefs[kTabMedia].glyph, kTabDefs[kTabMedia].empty, top,
                  viewBottom);
    } else if (g_mediaCompact) {
        // Three covers per row, square, with an 8 px gap.
        const float gap = 8.0f;
        const float cell = (kPanelW - 2 * kPad - 2 * gap) / 3.0f;
        const bool drag = DragOn(p, kTabMedia);
        for (size_t i = 0; i < p.media.size(); ++i) {
            const float x = kPad + (float)(i % 3) * (cell + gap);
            const float row = std::floor(static_cast<float>(i) / 3.0f);
            const float y = top - p.scrollC[kTabMedia] + row * (cell + gap);
            const D2D1_RECT_F cr = Rc(x, y, cell, cell);
            if (drag && (int)i == p.libDownIdx) {
                continue;  // drawn under the pointer instead
            }
            const Xf prevMove = BeginMove(p, L"m:" + p.media[i].key,
                                          MakeId(kHitMediaCard, (int)i),
                                          Rc(x, y + p.scrollC[kTabMedia], cell, cell));
            DrawMediaCover(p, (int)i, p.media[i], cr);
            EndSpring(p, prevMove);
            if (drag && (int)i == p.libTargetIdx) {
                FillRR(p, cr, kRadiusCtl, WithAlpha(p.th.accent, 0.25f));
                StrokeRR(p, cr, kRadiusCtl, p.th.accent);
            }
        }
        if (drag && p.libDownIdx < (int)p.media.size()) {
            const float baseAlpha = p.alpha;
            const bool saveHits = p.hitsOn;
            p.alpha = baseAlpha * 0.85f;
            p.hitsOn = false;
            DrawMediaCover(p, p.libDownIdx, p.media[p.libDownIdx],
                           Rc(p.libDragX - p.grabX - cell / 2, p.libDragY - p.grabY - cell / 2, cell, cell));
            p.hitsOn = saveHits;
            p.alpha = baseAlpha;
        }
        const size_t rows = (p.media.size() + 2) / 3;
        p.contentH[kTabMedia] = (float)rows * (cell + gap) - gap;
    } else {
        float y = top - p.scrollC[kTabMedia];
        const bool drag = DragOn(p, kTabMedia);
        for (size_t i = 0; i < p.media.size(); ++i) {
            if (!(drag && (int)i == p.libDownIdx)) {
                const Xf prevMove = BeginMove(p, L"m:" + p.media[i].key,
                                              MakeId(kHitMediaCard, (int)i),
                                              Rc(kPad, y + p.scrollC[kTabMedia], kPanelW - 2 * kPad, kMediaCardH));
                DrawMediaCard(p, (int)i, p.media[i], y);
                EndSpring(p, prevMove);
            }
            if (drag && (int)i == p.libTargetIdx) {
                const D2D1_RECT_F tr = Rc(kPad, y, kPanelW - 2 * kPad, kMediaCardH);
                FillRR(p, tr, kRadiusCtl, WithAlpha(p.th.accent, 0.15f));
                StrokeRR(p, tr, kRadiusCtl, p.th.accent);
            }
            y += kMediaCardH + 8;
        }
        if (drag && p.libDownIdx < (int)p.media.size()) {
            const float baseAlpha = p.alpha;
            const bool saveHits = p.hitsOn;
            p.alpha = baseAlpha * 0.85f;
            p.hitsOn = false;
            DrawMediaCard(p, p.libDownIdx, p.media[p.libDownIdx], p.libDragY - p.grabY - kMediaCardH / 2);
            p.hitsOn = saveHits;
            p.alpha = baseAlpha;
        }
        p.contentH[kTabMedia] = p.media.size() * (kMediaCardH + 8.0f) - 8.0f;
    }
    p.hitClip = saveClip;
    PopClip(p);

    if (g_showVolume) {
        DrawVolumeBar(p, bottom - kVolBarH);
    }
}

// One tile of the Apps / Library grids.
// Alpha of a visible (not hidden) PWA tile.
constexpr float kVisibleTileAlpha = 0.45f;

void DrawTileBody(Panel& p, int id, const D2D1_RECT_F& r, const TileData& d, int springId);

void DrawTile(Panel& p, int id, const D2D1_RECT_F& r, const TileData& d, int springId = -1) {
    const float baseAlpha = p.alpha;
    if (d.dim) {
        p.alpha = baseAlpha * kVisibleTileAlpha;
    }
    DrawTileBody(p, id, r, d, springId < 0 ? id : springId);
    p.alpha = baseAlpha;
}

void DrawTileBody(Panel& p, int id, const D2D1_RECT_F& r, const TileData& d, int springId) {
    const Theme& t = p.th;
    float w = r.right - r.left;
    float h = r.bottom - r.top;
    AddHit(p, id, r);
    const Xf prevTile = BeginSpring(p, springId, r);
    FillRR(p, r, kRadiusCtl, t.card);
    StrokeRR(p, r, kRadiusCtl, t.cardBorder);
    DrawHoverBg(p, id, r, kRadiusCtl);

    AppDisplay mode = (AppDisplay)g_appDisplay.load();
    D2D1_RECT_F ir;
    if (mode == AppDisplay::Icons) {
        ir = Rc(r.left + (w - 32) / 2, r.top + (h - 32) / 2, 32, 32);
    } else {
        ir = Rc(r.left + 10, r.top + (h - 28) / 2, 28, 28);
    }
    if (mode != AppDisplay::Names) {
        if (d.bmp) {
            DrawBmp(p, d.bmp, ir);
        } else {
            FillRR(p, ir, kRadiusCtl, t.hover);
            std::wstring letter = d.name.empty() ? L"?" : d.name.substr(0, 1);
            DrawTxt(p, letter, p.fLetter.p, ir, t.text);
        }
        if (d.running) {
            float dx = mode == AppDisplay::Icons ? r.left + w / 2
                                                 : ir.right - 2;
            float dy = mode == AppDisplay::Icons ? r.bottom - 5 : ir.bottom - 2;
            FillCircle(p, dx, dy, 3.0f, t.card);
            FillCircle(p, dx, dy, 2.5f, t.accent);
        }
    }

    // Room on the right for the speaker badge.
    const float padR = (d.hasMedia && mode != AppDisplay::Icons) ? 26.0f : 0.0f;
    float badgeW = 0;
    std::wstring badge;
    if (d.unread > 0) {
        badge = d.unread > 99 ? L"99+" : std::to_wstring(d.unread);
        badgeW = (std::max)(18.0f, MeasureW(badge, p.fBadge.p) + 10);
    }
    if (mode != AppDisplay::Icons) {
        float tx = mode == AppDisplay::Both ? r.left + 46 : r.left + 12;
        float tw = r.right - tx - 10 - padR - (badgeW > 0 ? badgeW + 4 : 0);
        DrawTxt(p, d.name, p.fButton.p, Rc(tx, r.top, tw, h), t.text);
    }
    if (badgeW > 0) {
        D2D1_RECT_F br;
        if (mode == AppDisplay::Icons) {
            br = Rc(r.right - badgeW - 2, r.top + 2, badgeW, 16);
        } else {
            br = Rc(r.right - badgeW - 8 - padR, r.top + (h - 16) / 2, badgeW, 16);
        }
        FillRR(p, br, 8.0f, t.accent);
        DrawTxt(p, badge, p.fBadge.p, br, t.onAccent);
    }

    if (d.hasMedia) {
        // Speaker badge: jumps to this app's media card. Hover is a 4 px
        // rounded subtle fill, the icon keeps its color.
        const int iconId = MakeId(kHitTileMedia, IndexOf(id));
        const D2D1_RECT_F ib = mode == AppDisplay::Icons
                                   ? Rc(r.right - 19, r.bottom - 19, 18, 18)
                                   : Rc(r.right - 26, r.top + (h - 20) / 2, 20, 20);
        AddHit(p, iconId, ib);
        const Xf prevIcon = BeginSpring(p, iconId, ib);
        DrawHoverBg(p, iconId, ib, kRadiusCtl);
        DrawTxt(p, kGlyphVol3, p.fMenuIcon.p, ib, t.text);
        EndSpring(p, prevIcon);
    }
    EndSpring(p, prevTile);
}

// Lays the tiles out in a grid starting at `top` (already scrolled). Returns
// the total height of the grid.
float DrawGrid(Panel& p,
               const std::vector<TileData>& tiles,
               float top,
               int hideIdx = -1,
               int targetIdx = -1) {
    if (tiles.empty()) {
        return 0;
    }
    const float availW = kPanelW - 2 * kPad;
    const float gap = 8;
    AppDisplay mode = (AppDisplay)g_appDisplay.load();
    int cols;
    float tw, th;
    if (mode == AppDisplay::Icons) {
        tw = th = 56;
        cols = (std::max)(1, (int)((availW + gap) / (tw + gap)));
    } else {
        cols = 2;
        tw = (availW - gap) / 2;
        th = mode == AppDisplay::Names ? 36.0f : 48.0f;
    }
    p.gridTileW = tw;
    p.gridTileH = th;
    float gridW = cols * tw + (cols - 1) * gap;
    float x0 = kPad + (availW - gridW) / 2;
    for (size_t i = 0; i < tiles.size(); ++i) {
        int row = (int)i / cols;
        int col = (int)i % cols;
        float x = x0 + col * (tw + gap);
        float y = top + row * (th + gap);
        if (y + th < p.viewTop || y > p.viewBottom) {
            continue;
        }
        if ((int)i == hideIdx) {
            continue;  // this tile is drawn under the pointer instead
        }
        const Xf prevMove = BeginMove(p, tiles[i].key, MakeId(kHitTile, (int)i),
                                      Rc(x, y + p.scrollC[p.selectedTab], tw, th));
        DrawTile(p, MakeId(kHitTile, (int)i), Rc(x, y, tw, th), tiles[i]);
        EndSpring(p, prevMove);
        if ((int)i == targetIdx) {
            const D2D1_RECT_F tr = Rc(x, y, tw, th);
            FillRR(p, tr, kRadiusCtl, WithAlpha(p.th.accent, 0.15f));
            StrokeRR(p, tr, kRadiusCtl, p.th.accent);
        }
    }
    int rows = ((int)tiles.size() + cols - 1) / cols;
    return rows * th + (rows - 1) * gap;
}

// The dragged tile, drawn under the pointer, a little transparent.
void DrawDragGhost(Panel& p, const TileData& d) {
    const float tw = p.gridTileW;
    const float th = p.gridTileH;
    const float baseAlpha = p.alpha;
    const bool saveHits = p.hitsOn;
    p.alpha = baseAlpha * 0.85f;
    p.hitsOn = false;  // the ghost must not become a drop target
    DrawTile(p, MakeId(kHitTile, 0xFFFE),
             Rc(p.libDragX - p.grabX - tw / 2, p.libDragY - p.grabY - th / 2, tw, th), d, p.dragPressId);
    p.hitsOn = saveHits;
    p.alpha = baseAlpha;
}

// A tile grid of a tab, with the dragged tile left out and the drop target marked.
float DrawGridWithDrag(Panel& p, const std::vector<TileData>& tiles, float top, int tab) {
    const bool drag = DragOn(p, tab) && p.libDragKind == kHitTile &&
                      p.libDownIdx < (int)tiles.size();
    const float h = DrawGrid(p, tiles, top, drag ? p.libDownIdx : -1,
                             drag ? p.libTargetIdx : -1);
    if (drag) {
        DrawDragGhost(p, tiles[p.libDownIdx]);
    }
    return h;
}

void DrawAppsTab(Panel& p, float top, float bottom) {
    p.viewH[kTabApps] = bottom - top;
    p.frameHwnds.clear();
    p.frameMediaKeys.clear();
    p.frameHidden.clear();
    std::vector<TileData> tiles;
    for (int idx : p.appsIdx) {
        const AppInfo& a = p.all[idx];
        TileData d;
        d.name = a.name;
        d.bmp = AppBitmap(p, a);
        d.unread = a.unread;
        d.dim = !a.hidden;
        d.key = L"a:" + std::to_wstring((uintptr_t)a.hwnd);
        std::wstring key;
        for (const auto& m : p.media) {
            if (!m.matchedCrx.empty() && m.matchedCrx == a.crxId) {
                key = m.key;  // first matching session
                break;
            }
        }
        d.hasMedia = !key.empty();
        tiles.push_back(d);
        p.frameHwnds.push_back(a.hwnd);
        p.frameMediaKeys.push_back(key);
        p.frameHidden.push_back(a.hidden);
    }
    if (tiles.empty()) {
        p.contentH[kTabApps] = 0;
        DrawEmpty(p, kTabDefs[kTabApps].glyph, kTabDefs[kTabApps].empty, top,
                  bottom);
        return;
    }
    p.viewTop = top;
    p.viewBottom = bottom;
    p.contentH[kTabApps] = DrawGridWithDrag(p, tiles, top - p.scrollC[kTabApps], kTabApps);
}

void DrawNotifsTab(Panel& p, float top, float bottom) {
    const Theme& t = p.th;
    p.viewH[kTabNotifs] = bottom - top;
    p.frameHwnds.clear();
    if (p.rows.empty()) {
        p.contentH[kTabNotifs] = 0;
        DrawEmpty(p, kTabDefs[kTabNotifs].glyph, kTabDefs[kTabNotifs].empty, top,
                  bottom);
        return;
    }
    float y = top - p.scrollC[kTabNotifs];
    std::unordered_map<std::wstring, int> seenRows;  // keeps keys unique for equal toasts
    const bool drag = DragOn(p, kTabNotifs);
    for (size_t i = 0; i < p.rows.size(); ++i) {
        const NotifRow& row = p.rows[i];
        const AppInfo& rowApp = p.all[row.appIdx];
        std::wstring rowKey = AppKey(rowApp) + L"|" + row.title;
        rowKey += L"#" + std::to_wstring(seenRows[rowKey]++);
        const AppInfo& a = p.all[row.appIdx];
        p.frameHwnds.push_back(a.hwnd);
        D2D1_RECT_F r = Rc(kPad, y, kPanelW - 2 * kPad, 64);
        y += 72;
        if (drag && (int)i == p.libDownIdx) {
            continue;  // drawn under the pointer instead
        }
        if (r.bottom < top || r.top > bottom) {
            continue;
        }
        int id = MakeId(kHitRow, (int)i);
        AddHit(p, id, r);
        const Xf prevMove = BeginMove(p, rowKey, id,
                                      Rc(r.left, r.top + p.scrollC[kTabNotifs], r.right - r.left, r.bottom - r.top));
        const Xf prevRow = BeginSpring(p, id, r);
        FillRR(p, r, kRadiusCtl, t.card);
        StrokeRR(p, r, kRadiusCtl, t.cardBorder);
        DrawHoverBg(p, id, r, kRadiusCtl);
        if (drag && (int)i == p.libTargetIdx) {
            FillRR(p, r, kRadiusCtl, WithAlpha(t.accent, 0.15f));
            StrokeRR(p, r, kRadiusCtl, t.accent);
        }

        D2D1_RECT_F ir = Rc(r.left + 10, r.top + 13, 28, 28);
        if (ID2D1Bitmap* b = AppBitmap(p, a)) {
            DrawBmp(p, b, ir);
        } else {
            FillRR(p, ir, kRadiusCtl, t.hover);
        }
        float tx = r.left + 48;
        float tw = r.right - tx - 10;
        DrawTxt(p, row.title, p.fName.p, Rc(tx, r.top + 6, tw, 20), t.text);
        DrawTxt(p, row.body, p.fBodyWrap.p, Rc(tx, r.top + 27, tw, 34),
                t.textSecondary);
        EndSpring(p, prevRow);
        EndSpring(p, prevMove);
    }
    if (drag && p.libDownIdx < (int)p.rows.size()) {
        const NotifRow& row = p.rows[p.libDownIdx];
        const AppInfo& a = p.all[row.appIdx];
        const float baseAlpha = p.alpha;
        const bool saveHits = p.hitsOn;
        p.alpha = baseAlpha * 0.85f;
        p.hitsOn = false;
        const D2D1_RECT_F gr = Rc(kPad, p.libDragY - p.grabY - 32, kPanelW - 2 * kPad, 64);
        FillRR(p, gr, kRadiusCtl, t.card);
        StrokeRR(p, gr, kRadiusCtl, t.cardBorder);
        if (ID2D1Bitmap* b = AppBitmap(p, a)) {
            DrawBmp(p, b, Rc(gr.left + 10, gr.top + 12, 28, 28));
        }
        const float tx = gr.left + 48;
        const float tw = gr.right - tx - 10;
        DrawTxt(p, row.title, p.fName.p, Rc(tx, gr.top + 6, tw, 20), t.text);
        DrawTxt(p, row.body, p.fBodyWrap.p, Rc(tx, gr.top + 27, tw, 34), t.textSecondary);
        p.alpha = baseAlpha;
        p.hitsOn = saveHits;
    }
    p.contentH[kTabNotifs] = p.rows.size() * 72.0f - 8.0f;
}

void DrawChips(Panel& p, float y) {
    const Theme& t = p.th;
    const float h = 30;
    const float gap = 6;
    const float W = kPanelW;
    size_t n = g_categories.size();
    if ((size_t)p.chipSel > n) {
        p.chipSel = 0;
    }

    D2D1_RECT_F clip = Rc(0, y - 1, W, h + 2);
    PushClip(p, clip);
    D2D1_RECT_F saveClip = p.hitClip;
    p.hitClip = clip;

    float x = kPad - p.chipScroll;
    float startX = x;
    const bool dragChip = DragOn(p, kTabLibrary) && p.libDragKind == kHitChip;
    for (size_t i = 0; i <= n + 1; ++i) {
        bool isAdd = i == n + 1;
        std::wstring label = isAdd ? L"" : (i == 0 ? L"All" : g_categories[i - 1].name);
        float w = isAdd ? 30.0f : (std::max)(44.0f, MeasureW(label, p.fCenter.p) + 24);
        D2D1_RECT_F r = Rc(x, y, w, h);
        int id = MakeId(kHitChip, (int)i);
        if (dragChip && (int)i == p.libDownIdx) {
            p.chipGhostW = w;
            x += w + gap;
            continue;  // the dragged chip is drawn under the pointer
        }
        AddHit(p, id, r);
        const Xf prevMove = BeginMove(p, L"c:" + label, id,
                                      Rc(r.left + p.chipScroll, r.top, r.right - r.left, r.bottom - r.top));
        const Xf prevChip = BeginSpring(p, id, r);
        bool sel = !isAdd && (int)i == p.chipSel;
        if (sel) {
            FillRR(p, r, kRadiusCtl, t.accent);
        } else {
            FillRR(p, r, kRadiusCtl, t.card);
            StrokeRR(p, r, kRadiusCtl, t.cardBorder);
            DrawHoverBg(p, id, r, kRadiusCtl);
        }
        if (isAdd) {
            DrawTxt(p, kGlyphAdd, p.fIcon.p, r, t.text);
        } else {
            DrawTxt(p, label, p.fCenter.p, r, sel ? t.onAccent : t.text);
        }
        if (dragChip && (int)i == p.libTargetIdx && i >= 1 && i <= n) {
            FillRR(p, r, kRadiusCtl, WithAlpha(t.accent, 0.25f));
            StrokeRR(p, r, kRadiusCtl, t.accent);
        }
        EndSpring(p, prevChip);
        EndSpring(p, prevMove);
        x += w + gap;
    }
    p.chipContentW = (x - gap) - startX + 2 * kPad;
    float maxScroll = (std::max)(0.0f, p.chipContentW - W);
    p.chipScrollT = (std::max)(0.0f, (std::min)(p.chipScrollT, maxScroll));
    p.chipScroll = (std::max)(0.0f, (std::min)(p.chipScroll, maxScroll));

    p.hitClip = saveClip;
    PopClip(p);

    // The dragged chip follows the pointer sideways, a little transparent.
    if (dragChip && p.libDownIdx >= 1 && p.libDownIdx <= (int)n) {
        const float baseAlpha = p.alpha;
        const bool saveHits = p.hitsOn;
        p.alpha = baseAlpha * 0.85f;
        p.hitsOn = false;
        const D2D1_RECT_F gr = Rc(p.libDragX - p.grabX - p.chipGhostW / 2, y, p.chipGhostW, h);
        FillRR(p, gr, kRadiusCtl, t.accent);
        DrawTxt(p, g_categories[p.libDownIdx - 1].name, p.fCenter.p, gr, t.onAccent);
        p.alpha = baseAlpha;
        p.hitsOn = saveHits;
    }

    // Horizontal scrollbar under the chips, the same look as the vertical one.
    if (maxScroll > 0) {
        float trackW = W - 2 * kPad;
        float thumbW = (std::max)(24.0f, trackW * W / p.chipContentW);
        float frac = p.chipScroll / maxScroll;
        float tx = kPad + (trackW - thumbW) * frac;
        FillRR(p, Rc(tx, y + h + 4, thumbW, 3), 1.5f, WithAlpha(t.track, 0.9f));
    }
}

// The text the apps are filtered by: trimmed and lowercase.
std::wstring SearchQuery(const Panel& p) {
    return ToLower(Trim(p.searchText));
}

void DrawLibraryTab(Panel& p, float top, float bottom) {
    const float W = kPanelW;
    bottom = kLibGridBottom;  // the search field takes the space below
    const std::wstring query = SearchQuery(p);
    DrawChips(p, top);
    float vtop = top + 40;
    p.viewH[kTabLibrary] = bottom - vtop;

    std::vector<TileData> tiles;
    p.frameLib.clear();
    for (size_t i = 0; i < g_library.size(); ++i) {
        const LibApp& l = g_library[i];
        if (p.chipSel > 0 && !InCategory(g_categories[p.chipSel - 1], l.crxId)) {
            continue;
        }
        if (!query.empty() && ToLower(l.name).find(query) == std::wstring::npos) {
            continue;
        }
        TileData d;
        d.name = l.name;
        d.key = L"l:" + LibKey(l);
        d.bmp = LibBitmap(p, l);
        if (const AppInfo* run = FindRunning(p, l)) {
            d.running = true;
            d.unread = run->unread;
            if (!d.bmp) {
                d.bmp = AppBitmap(p, *run);
            }
        }
        tiles.push_back(d);
        p.frameLib.push_back((int)i);
    }

    D2D1_RECT_F clip = Rc(0, vtop, W, bottom - vtop);
    PushClip(p, clip);
    D2D1_RECT_F saveClip = p.hitClip;
    p.hitClip = clip;
    if (tiles.empty()) {
        p.contentH[kTabLibrary] = 0;
        DrawEmpty(p, kTabDefs[kTabLibrary].glyph,
                  !query.empty() ? L"Nothing found"
                                 : (p.chipSel > 0 ? L"No apps in this category"
                                                  : kTabDefs[kTabLibrary].empty),
                  vtop, bottom);
    } else {
        p.viewTop = vtop;
        p.viewBottom = bottom;
        p.contentH[kTabLibrary] =
            DrawGridWithDrag(p, tiles, vtop - p.scrollC[kTabLibrary], kTabLibrary);
    }
    p.hitClip = saveClip;
    PopClip(p);
}

// ---------------------------------------------------------------------------
// Settings tab: groups of toggles (Fluent switch) and dropdowns
// ---------------------------------------------------------------------------

// One row of the Settings tab: a toggle, or a dropdown with `options`.
struct SettingRow {
    int pref;
    bool toggle;
    PCWSTR label;
    PCWSTR glyph;
    const wchar_t* const* options;
    bool hotkey;  // the shortcut field, not a preference
};

struct SettingGroup {
    PCWSTR glyph;
    PCWSTR title;
    int first;  // index of its first row in kSettingRows
    int count;
};

const SettingRow kSettingRows[] = {
    {kPrefPanelMode, false, L"Center mode", L"\xE718", kOptPanelMode, false},
    {kPrefBackdrop, false, L"Backdrop", L"\xE771", kOptBackdrop, false},
    {-1, false, L"Open shortcut", kGlyphRename, nullptr, true},
    {kPrefCardCover, false, L"Cover style", L"\xE8B9", kOptCardCover, false},
    {kPrefMediaCompact, true, L"Compact view", L"\xF232", nullptr, false},
    {kPrefShowVolume, true, L"Chrome volume", L"\xE767", nullptr, false},
    {kPrefAppDisplay, false, L"App view", L"\xF207", kOptAppDisplay, false},
    {kPrefNotes, true, L"Pop-up notifications", L"\xE7ED", nullptr, false},
    {kPrefNotifPos, false, L"Position", L"\xE707", kOptNotifPos, false},
    {kPrefNotifDismiss, false, L"Dismiss with", L"\xE8B0", kOptNotifDismiss, false},
    {kPrefTrayAlways, true, L"Always show tray icon", L"\xE7E7", nullptr, false},
    {kPrefDiag, true, L"Diagnostics", L"\xF196", nullptr, false},
};

const SettingGroup kSettingGroups[] = {
    {L"\xE7C4", L"Panel", 0, 3},
    {L"\xE768", L"Media", 3, 3},
    {L"\xF2CE", L"PWAs", 6, 1},
    {L"\xE7E7", L"Notifications", 7, 3},
    {L"\xE756", L"Advanced", 10, 2},
};

constexpr int kSettingRowCount = (int)(sizeof(kSettingRows) / sizeof(kSettingRows[0]));
constexpr float kSetHeadH = 30.0f;
constexpr float kSetRowH = 40.0f;
constexpr float kSetRowGap = 4.0f;
constexpr float kSetGroupGap = 8.0f;

// Thumb position of a toggle. Created at the state the pref has now.
Tween& ToggleTween(Panel& p, int id) {
    auto it = p.toggleT.find(id);
    if (it == p.toggleT.end()) {
        Tween t;
        TweenJump(t, PrefValue(id)->load() ? 1.0f : 0.0f);
        it = p.toggleT.emplace(id, t).first;
    }
    return it->second;
}

// How far the thumb travels from off to on, in DIP (its centers: 10 px in, 10 px from the far edge).
constexpr float kToggleTravel = 20.0f;

// Fluent toggle: a 40 x 20 track. `on` is the position, 0 to 1, and follows a drag.
// `on` is the animated state, 0 to 1.
void DrawToggle(Panel& p, const D2D1_RECT_F& r, float on, bool pressed) {
    const Theme& t = p.th;
    const float h = r.bottom - r.top;
    if (on < 1.0f) {
        StrokeRR(p, r, h / 2, WithAlpha(t.track, 1.0f - on));
    }
    if (on > 0.0f) {
        FillRR(p, r, h / 2, WithAlpha(t.accent, on));
    }
    // Pressed, the thumb swells a little (as on Apple's switches).
    const float radius = 5.0f + 2.0f * on + (pressed ? 1.5f : 0.0f);
    const float cx = r.left + 10.0f + kToggleTravel * on;
    const float cy = r.top + h / 2;
    FillCircle(p, cx, cy, radius, on > 0.5f ? t.onAccent : t.textSecondary);
}

// Dropdown box: the current option and a chevron.
void DrawDropdown(Panel& p, const D2D1_RECT_F& r, PCWSTR value) {
    const Theme& t = p.th;
    FillRR(p, r, kRadiusCtl, t.fieldBg);
    StrokeRR(p, r, kRadiusCtl, t.cardBorder);
    DrawTxt(p, value, p.fButton.p,
            Rc(r.left + 10, r.top, r.right - r.left - 34, r.bottom - r.top), t.text);
    DrawTxt(p, kGlyphChevDown, p.fMenuIcon.p,
            Rc(r.right - 24, r.top, 16, r.bottom - r.top), t.textSecondary);
}

// Names of the mouse buttons a shortcut can use (their Virtual-Key codes).
PCWSTR MouseButtonName(UINT vk) {
    switch (vk) {
        case VK_LBUTTON: return L"Left click";
        case VK_RBUTTON: return L"Right click";
        case VK_MBUTTON: return L"Middle click";
        case VK_XBUTTON1: return L"Mouse 4";
        case VK_XBUTTON2: return L"Mouse 5";
        default: return nullptr;
    }
}

// Text of a shortcut, such as "Ctrl+Alt+P".
std::wstring HotkeyText(int value) {
    if (value == 0) {
        return L"None";
    }
    const UINT vk = (UINT)value & 0xFFFF;
    const UINT mods = ((UINT)value >> 16) & 0xFFFF;
    std::wstring s;
    if (mods & MOD_CONTROL) s += L"Ctrl+";
    if (mods & MOD_ALT) s += L"Alt+";
    if (mods & MOD_SHIFT) s += L"Shift+";
    if (mods & MOD_WIN) s += L"Win+";
    if (PCWSTR name = MouseButtonName(vk)) {
        s += name;
        return s;
    }
    const UINT sc = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC_EX);
    LONG lp = (LONG)((sc & 0xFF) << 16);
    if (sc & 0xFF00) {
        lp |= (1 << 24);  // extended key
    }
    wchar_t name[64] = {};
    if (GetKeyNameTextW(lp, name, ARRAYSIZE(name)) > 0) {
        s += name;
    } else {
        wchar_t hex[16] = {};
        wsprintfW(hex, L"0x%02X", vk);
        s += hex;
    }
    return s;
}

// The field of the shortcut row: its text, outlined in the accent color while it waits.
void DrawValueBox(Panel& p, const D2D1_RECT_F& r, const std::wstring& value, bool active) {
    const Theme& t = p.th;
    FillRR(p, r, kRadiusCtl, t.fieldBg);
    StrokeRR(p, r, kRadiusCtl, active ? t.accent : t.cardBorder);
    DrawTxt(p, value, p.fButton.p,
            Rc(r.left + 10, r.top, r.right - r.left - 20, r.bottom - r.top),
            t.text);
}

void DrawSettingsTab(Panel& p, float top, float bottom) {
    const Theme& t = p.th;
    p.viewH[kTabSettings] = bottom - top;
    p.viewTop = top;
    p.viewBottom = bottom;
    p.settingRects.clear();

    const float x0 = kPad;
    const float w = kPanelW - 2 * kPad;
    float y = top - p.scrollC[kTabSettings];
    for (const SettingGroup& g : kSettingGroups) {
        DrawTxt(p, g.glyph, p.fIcon.p, Rc(x0 + 2, y, 16, kSetHeadH), t.textSecondary);
        DrawTxt(p, g.title, p.fBody.p, Rc(x0 + 26, y, w - 26, kSetHeadH), t.textSecondary);
        y += kSetHeadH;
        for (int i = g.first; i < g.first + g.count; ++i) {
            const SettingRow& row = kSettingRows[i];
            const D2D1_RECT_F rr = Rc(x0, y, w, kSetRowH);
            p.settingRects.push_back(rr);
            if (rr.bottom >= top && rr.top <= bottom) {
                const int id = MakeId(kHitSetting, i);
                AddHit(p, id, rr);
                FillRR(p, rr, kRadiusCtl, t.card);
                StrokeRR(p, rr, kRadiusCtl, t.cardBorder);
                DrawHoverBg(p, id, rr, kRadiusCtl);
                DrawTxt(p, row.glyph, p.fIcon.p,
                        Rc(rr.left + 12, rr.top, 16, kSetRowH), t.text);
                DrawTxt(p, row.label, p.fButton.p,
                        Rc(rr.left + 40, rr.top, w - 40 - 130, kSetRowH), t.text);
                const float right = rr.right - 12;
                if (row.toggle) {
                    const float top20 = rr.top + (kSetRowH - 20) / 2;
                    DrawToggle(p, Rc(right - 40, top20, 40, 20),
                               ToggleTween(p, row.pref).v, p.togglePressRow == i);
                } else if (row.hotkey) {
                    DrawValueBox(p, Rc(right - 130, rr.top + 7, 130, 26),
                                 p.hotkeyListening ? std::wstring(L"Press a key")
                                                   : HotkeyText(g_hotkey.load()),
                                 p.hotkeyListening);
                } else {
                    DrawDropdown(p, Rc(right - 130, rr.top + 7, 130, 26),
                                 row.options[PrefValue(row.pref)->load()]);
                }
            }
            y += kSetRowH + kSetRowGap;
        }
        y += kSetGroupGap;
    }
    p.contentH[kTabSettings] = y - top + p.scrollC[kTabSettings];
}

void DrawScrollbar(Panel& p, float top, float bottom) {
    int tab = p.selectedTab;
    float viewH = p.viewH[tab];
    float contentH = p.contentH[tab];
    if (contentH <= viewH + 1 || viewH <= 0) {
        return;
    }
    // Shown while the pointer is over the list or it was scrolled, then it fades out.
    const bool show = NowMs() - p.barTick < 1000;
    if ((p.barA.to > 0.5f) != show) {
        TweenSet(p.barA, show ? 1.0f : 0.0f, show ? 83 : 167, kEaseLinear);
    }
    if (p.barA.v <= 0.001f) {
        return;
    }
    float trackTop = tab == kTabMedia ? top : bottom - viewH;
    float thumbH = (std::max)(24.0f, viewH * viewH / contentH);
    float maxScroll = contentH - viewH;
    float frac = maxScroll > 0 ? p.scrollC[tab] / maxScroll : 0;
    frac = frac < 0 ? 0 : (frac > 1 ? 1 : frac);
    float y = trackTop + 2 + frac * (viewH - thumbH - 4);
    FillRR(p, Rc(kPanelW - 6, y, 3, thumbH), 1.5f, WithAlpha(p.th.track, 0.9f * p.barA.v));
}

void DrawMenu(Panel& p) {
    const Theme& t = p.th;
    float prevAlpha = p.alpha;
    float prevOy = p.oy;
    p.alpha = p.menuT.v;
    p.oy = p.menuDy.v;
    D2D1_RECT_F r = p.menuRect;
    // With Mica the area under the menu was left transparent (see DrawPanel),
    // so a translucent tint shows the backdrop through.
    FillRR(p, r, kRadiusOverlay, p.solid ? t.menuBg : t.menuBgMica);
    StrokeRR(p, r, kRadiusOverlay, t.menuBorder);

    const float px = DipPerDevicePx(p);  // one device pixel in DIPs
    float y = r.top + kMenuPad;
    for (size_t i = 0; i < p.menuItems.size(); ++i) {
        const MenuItem& it = p.menuItems[i];
        if (it.sep) {
            // Hairline: exactly one device pixel, snapped to the pixel grid.
            float sy = std::floor((y + 4) / px) * px;
            FillRect(p, Rc(r.left + 1, sy, r.right - r.left - 2, px),
                     WithAlpha(t.divider, 0.8f));
            y += kMenuSepH;
            continue;
        }
        D2D1_RECT_F ir = Rc(r.left + kMenuPad, y, r.right - r.left - 2 * kMenuPad,
                            kMenuItemH);
        if (it.header) {
            DrawBmp(p, it.bmp, Rc(ir.left + 10, y + 10, 16, 16));
            DrawTxt(p, it.text, p.fMenu.p,
                    Rc(ir.left + 34, y, ir.right - ir.left - 42, kMenuItemH), t.text);
            y += kMenuItemH;
            continue;
        }
        int id = MakeId(kHitMenuItem, (int)i);
        if (it.enabled) {
            AddHit(p, id, ir);
            DrawHoverBg(p, id, Rc(ir.left, ir.top + 2, ir.right - ir.left,
                                  kMenuItemH - 4),
                        kRadiusCtl);
        }
        D2D1_COLOR_F c = it.enabled ? t.text : WithAlpha(t.textSecondary, 0.5f);
        if (it.checked) {
            DrawTxt(p, kGlyphCheck, p.fMenuIcon.p, Rc(ir.left + 8, y, 20, kMenuItemH), c);
        } else if (it.glyph) {
            DrawTxt(p, it.glyph, p.fMenuIcon.p, Rc(ir.left + 8, y, 20, kMenuItemH), c);
        }
        DrawTxt(p, it.text, p.fMenu.p,
                Rc(ir.left + 38, y, ir.right - ir.left - 46, kMenuItemH), c);
        y += kMenuItemH;
    }
    p.alpha = prevAlpha;
    p.oy = prevOy;
}

bool HasDlgSel(const Panel& p) {
    return p.dlgSelAnchor >= 0 && p.dlgSelAnchor != p.dlgCaret;
}

void DlgSelRange(const Panel& p, int& a, int& b) {
    if (HasDlgSel(p)) {
        a = (std::min)(p.dlgSelAnchor, p.dlgCaret);
        b = (std::max)(p.dlgSelAnchor, p.dlgCaret);
    } else {
        a = b = p.dlgCaret;
    }
}

// Screen x of the character boundary `idx` in the dialog text (DIPs).
float DlgTextX(const Panel& p, int idx) {
    return p.dlgField.left + 11.0f +
           MeasureW(p.dlgText.substr(0, (size_t)idx), p.fButton.p);
}

// The character boundary nearest to x (DIPs).
int CaretFromX(const Panel& p, float x) {
    int best = 0;
    float bestD = 1e9f;
    const int len = (int)p.dlgText.size();
    for (int i = 0; i <= len; ++i) {
        float d = std::fabs(DlgTextX(p, i) - x);
        if (d < bestD) {
            bestD = d;
            best = i;
        }
    }
    return best;
}

// Character classes for word jumps: 0 space, 1 word (letters, digits, _), 2 other.
int CharClassOf(wchar_t c) {
    if (std::iswspace(c)) {
        return 0;
    }
    if (std::iswalnum(c) || c == L'_') {
        return 1;
    }
    return 2;
}

// Ctrl+Left: start of the previous word.
int PrevWordStart(const std::wstring& t, int i) {
    while (i > 0 && CharClassOf(t[(size_t)i - 1]) == 0) {
        --i;
    }
    if (i == 0) {
        return 0;
    }
    const int cls = CharClassOf(t[(size_t)i - 1]);
    while (i > 0 && CharClassOf(t[(size_t)i - 1]) == cls) {
        --i;
    }
    return i;
}

// Ctrl+Right: start of the next word.
int NextWordStart(const std::wstring& t, int i) {
    const int n = (int)t.size();
    if (i >= n) {
        return n;
    }
    const int cls = CharClassOf(t[(size_t)i]);
    if (cls != 0) {
        while (i < n && CharClassOf(t[(size_t)i]) == cls) {
            ++i;
        }
    }
    while (i < n && CharClassOf(t[(size_t)i]) == 0) {
        ++i;
    }
    return i;
}

constexpr size_t kDialogMaxLen = 40;

// Clipboard text as Unicode, or empty.
std::wstring ReadClipboardText(HWND owner) {
    std::wstring out;
    if (!OpenClipboard(owner)) {
        return out;
    }
    if (HANDLE h = GetClipboardData(CF_UNICODETEXT)) {
        if (const wchar_t* text = (const wchar_t*)GlobalLock(h)) {
            out = text;
            GlobalUnlock(h);
        }
    }
    CloseClipboard();
    return out;
}

// One editable text. The category dialog field and the search field both edit
// through these functions, so keys and selection behave the same in both.
struct TextEdit {
    std::wstring& text;
    int& caret;
    int& anchor;  // -1 = no selection
    size_t maxLen;
};

TextEdit DlgEdit(Panel& p) {
    return TextEdit{p.dlgText, p.dlgCaret, p.dlgSelAnchor, kDialogMaxLen};
}

bool TEHasSel(const TextEdit& e) {
    return e.anchor >= 0 && e.anchor != e.caret;
}

void TESelRange(const TextEdit& e, int& a, int& b) {
    if (TEHasSel(e)) {
        a = (std::min)(e.anchor, e.caret);
        b = (std::max)(e.anchor, e.caret);
    } else {
        a = b = e.caret;
    }
}

void TEDeleteSel(TextEdit e) {
    int a = 0, b = 0;
    TESelRange(e, a, b);
    e.text.erase((size_t)a, (size_t)(b - a));
    e.caret = a;
    e.anchor = -1;
}

// Replaces the selection with `in`: printable characters only, up to the length limit.
void TEInsert(TextEdit e, const std::wstring& in) {
    if (TEHasSel(e)) {
        TEDeleteSel(e);
    }
    for (wchar_t ch : in) {
        if (ch < 32 || ch == 127 || e.text.size() >= e.maxLen) {
            continue;
        }
        e.text.insert((size_t)e.caret, 1, ch);
        ++e.caret;
    }
    e.anchor = -1;
}

void TEBackspace(TextEdit e) {
    if (TEHasSel(e)) {
        TEDeleteSel(e);
    } else if (e.caret > 0) {
        e.text.erase((size_t)(e.caret - 1), 1);
        --e.caret;
        e.anchor = -1;
    }
}

void TEDeleteFwd(TextEdit e) {
    if (TEHasSel(e)) {
        TEDeleteSel(e);
    } else if (e.caret < (int)e.text.size()) {
        e.text.erase((size_t)e.caret, 1);
        e.anchor = -1;
    }
}

void TESelectAll(TextEdit e) {
    if (!e.text.empty()) {
        e.anchor = 0;
        e.caret = (int)e.text.size();
    }
}

// Double click: selects the run of one character class around idx.
void TESelectWord(TextEdit e, int idx) {
    if (e.text.empty()) {
        return;
    }
    const int n = (int)e.text.size();
    const int i = idx < n ? idx : n - 1;
    const int cls = CharClassOf(e.text[(size_t)i]);
    int a = i;
    int b = i + 1;
    while (a > 0 && CharClassOf(e.text[(size_t)a - 1]) == cls) {
        --a;
    }
    while (b < n && CharClassOf(e.text[(size_t)b]) == cls) {
        ++b;
    }
    e.anchor = a;
    e.caret = b;
}

// Arrows, Home and End. Shift extends the selection, Ctrl jumps by word.
bool TEMove(TextEdit e, WPARAM vk, bool shift, bool ctrl) {
    if (vk != VK_LEFT && vk != VK_RIGHT && vk != VK_HOME && vk != VK_END) {
        return false;
    }
    const int len = (int)e.text.size();
    int target = e.caret;
    if (vk == VK_LEFT) {
        target = ctrl ? PrevWordStart(e.text, e.caret) : (e.caret > 0 ? e.caret - 1 : 0);
    } else if (vk == VK_RIGHT) {
        target = ctrl ? NextWordStart(e.text, e.caret) : (e.caret < len ? e.caret + 1 : len);
    } else if (vk == VK_HOME) {
        target = 0;
    } else {
        target = len;
    }
    // Without Shift an existing selection collapses to its edge.
    if (!shift && TEHasSel(e) && (vk == VK_LEFT || vk == VK_RIGHT)) {
        int a = 0, b = 0;
        TESelRange(e, a, b);
        target = vk == VK_LEFT ? a : b;
    }
    if (shift) {
        if (e.anchor < 0) {
            e.anchor = e.caret;
        }
    } else {
        e.anchor = -1;
    }
    e.caret = target;
    if (e.anchor == e.caret) {
        e.anchor = -1;
    }
    return true;
}

void TECopy(TextEdit e, HWND owner) {
    if (!TEHasSel(e)) {
        return;
    }
    int a = 0, b = 0;
    TESelRange(e, a, b);
    SetClipboardText(owner, e.text.substr((size_t)a, (size_t)(b - a)));
}

void TEPaste(TextEdit e, HWND owner) {
    const std::wstring text = ReadClipboardText(owner);
    if (!text.empty()) {
        TEInsert(e, text);
    }
}


void SelectDlgWordAt(Panel& p, int idx) {
    TESelectWord(DlgEdit(p), idx);
    p.dlgTyping = false;
}

void DrawDialog(Panel& p) {
    const Theme& t = p.th;
    FillRR(p, Rc(0, 0, kPanelW, p.hDip), 0.0f, t.scrim);
    // Swallow the clicks outside of the dialog card.
    AddHit(p, MakeId(kHitNone, 1), Rc(0, 0, kPanelW, p.hDip));

    D2D1_RECT_F r = p.dlgRect;
    FillRR(p, r, kRadiusOverlay, t.menuBg);
    StrokeRR(p, r, kRadiusOverlay, t.menuBorder);
    DrawTxt(p, p.dlgRename ? L"Rename category" : L"New category", p.fName.p,
            Rc(r.left + 16, r.top + 12, r.right - r.left - 32, 24), t.text);

    // The active field fill is a little lighter (dark theme only, the light
    // field is already white).
    D2D1_COLOR_F fieldFill = t.fieldBg;
    if (p.dlgFocused && t.dark) {
        fieldFill.r += (1.0f - fieldFill.r) * 0.06f;
        fieldFill.g += (1.0f - fieldFill.g) * 0.06f;
        fieldFill.b += (1.0f - fieldFill.b) * 0.06f;
    }
    FillRR(p, p.dlgField, kRadiusCtl, fieldFill);
    StrokeRR(p, p.dlgField, kRadiusCtl, t.cardBorder);
    if (p.dlgFocused) {
        // Active field: the bottom corners are square, so the accent rule
        // runs flush into the sides. Repaint the bottom corner area square and
        // restore the side borders above the rule.
        const D2D1_RECT_F f0 = p.dlgField;
        const float corner = kRadiusCtl;
        FillRect(p, Rc(f0.left, f0.bottom - corner, f0.right - f0.left, corner),
                 fieldFill);
        FillRect(p, Rc(f0.left, f0.bottom - corner, 1.0f, corner - 2.0f),
                 t.cardBorder);
        FillRect(p, Rc(f0.right - 1.0f, f0.bottom - corner, 1.0f, corner - 2.0f),
                 t.cardBorder);
    }

    const D2D1_RECT_F f = p.dlgField;
    const float textX = f.left + 11.0f;
    const float textW = f.right - f.left - 22.0f;
    if (HasDlgSel(p)) {
        int a = 0, b = 0;
        DlgSelRange(p, a, b);
        const float x0 = DlgTextX(p, a);
        const float x1 = DlgTextX(p, b);
        FillRect(p, Rc(x0, f.top + 8, x1 - x0, f.bottom - f.top - 14),
                 WithAlpha(t.accent, 0.4f));
    }
    DrawTxt(p, p.dlgText, p.fButton.p,
            Rc(textX, f.top, textW, f.bottom - f.top - 2), t.text);
    if (p.dlgFocused && p.dlgCaretOn) {
        const std::wstring beforeCaret =
            p.dlgText.substr(0, static_cast<size_t>(p.dlgCaret));
        float caretX = textX + MeasureW(beforeCaret, p.fButton.p);
        FillRect(p, Rc(caretX, f.top + 10, 1.0f, 16.0f), t.text);
    }

    if (p.dlgFocused) {
        // Active field: 2px rule, the accent a bit lighter.
        D2D1_COLOR_F lit = t.accent;
        lit.r += (1.0f - lit.r) * 0.25f;
        lit.g += (1.0f - lit.g) * 0.25f;
        lit.b += (1.0f - lit.b) * 0.25f;
        FillRect(p, Rc(f.left, f.bottom - 2, f.right - f.left, 2), lit);
    }

    for (size_t i = 0; i < p.dlgLog.size(); ++i) {
        const float y = r.top + 88.0f + 16.0f * (float)i;
        const std::wstring text =
            i == 0 ? p.dlgLog[i] : L"?: " + p.dlgLog[i];
        DrawTxt(p, text, p.fBody.p,
                Rc(r.left + 16, y, r.right - r.left - 32, 16),
                i == 0 ? (t.dark ? Col(0xFFFFB347) : Col(0xFFB35C00))
                       : SeverityColor(t, NotifSeverity::Error));
    }

    float bw = (r.right - r.left - 40) / 2;
    D2D1_RECT_F ok = Rc(r.left + 16, r.bottom - 48, bw, 32);
    D2D1_RECT_F cancel = Rc(r.left + 24 + bw, r.bottom - 48, bw, 32);
    int idOk = MakeId(kHitDlgOk, 0);
    int idCancel = MakeId(kHitDlgCancel, 0);
    AddHit(p, idOk, ok);
    AddHit(p, idCancel, cancel);

    D2D1_COLOR_F fill = t.accent;
    if (p.hovered == idOk && p.pressed == idOk) {
        fill = t.accentPress;
    } else if (p.hovered == idOk) {
        fill = t.accentHover;
    }
    FillRR(p, ok, kRadiusCtl, fill);
    DrawTxt(p, p.dlgRename ? L"Save" : L"Create", p.fCenter.p, ok, t.onAccent);

    FillRR(p, cancel, kRadiusCtl, t.card);
    StrokeRR(p, cancel, kRadiusCtl, t.cardBorder);
    DrawHoverBg(p, idCancel, cancel, kRadiusCtl);
    DrawTxt(p, L"Cancel", p.fCenter.p, cancel, t.text);
}

// ---------------------------------------------------------------------------
// Library search field: the same field as in the category dialog, pinned to
// the bottom of the Library tab. It filters the apps by name.
// ---------------------------------------------------------------------------

constexpr size_t kSearchMaxLen = 40;
constexpr float kSearchTextLeft = 30.0f;  // the text starts after the search icon

D2D1_RECT_F SearchRect() {
    return Rc(kPad, kSearchTop, kPanelW - 2 * kPad, kSearchH);
}

bool HasSearchSel(const Panel& p) {
    return p.searchSelAnchor >= 0 && p.searchSelAnchor != p.searchCaret;
}

void SearchSelRange(const Panel& p, int& a, int& b) {
    if (HasSearchSel(p)) {
        a = (std::min)(p.searchSelAnchor, p.searchCaret);
        b = (std::max)(p.searchSelAnchor, p.searchCaret);
    } else {
        a = b = p.searchCaret;
    }
}

// Screen x of the character boundary idx in the search text (DIPs).
float SearchTextX(const Panel& p, int idx) {
    return SearchRect().left + kSearchTextLeft +
           MeasureW(p.searchText.substr(0, (size_t)idx), p.fButton.p);
}

// The character boundary nearest to x (DIPs).
int SearchCaretFromX(const Panel& p, float x) {
    int best = 0;
    float bestD = 1e9f;
    const int len = (int)p.searchText.size();
    for (int i = 0; i <= len; ++i) {
        float d = std::fabs(SearchTextX(p, i) - x);
        if (d < bestD) {
            bestD = d;
            best = i;
        }
    }
    return best;
}

// The caret blink timer runs while a text field is open (category dialog or search).
void SyncCaretTimer(Panel& p) {
    if (p.dlgOpen || p.searchFocused) {
        SetTimer(p.hwnd, kTimerCaret, kTimerCaretMs, nullptr);
    } else {
        KillTimer(p.hwnd, kTimerCaret);
    }
}

void SetSearchFocus(Panel& p, bool on) {
    p.searchFocused = on;
    p.searchCaretOn = true;
    if (!on) {
        p.searchSelAnchor = -1;
        p.searchSelDragging = false;
    }
    SyncCaretTimer(p);
    StartAnim(p);
}

TextEdit SearchEdit(Panel& p) {
    return TextEdit{p.searchText, p.searchCaret, p.searchSelAnchor, kSearchMaxLen};
}

// The text changed: back to the top of the list, the caret shows.
void SearchEdited(Panel& p) {
    p.searchCaretOn = true;
    p.scrollT[kTabLibrary] = 0;
    p.scrollC[kTabLibrary] = 0;
    StartAnim(p);
}





void SelectSearchWordAt(Panel& p, int idx) {
    TESelectWord(SearchEdit(p), idx);
    StartAnim(p);
}

void SearchChar(Panel& p, wchar_t c) {
    if (!p.searchFocused) {
        return;
    }
    p.searchCaretOn = true;
    if (c >= 32 && c != 127) {
        TEInsert(SearchEdit(p), std::wstring(1, c));
        SearchEdited(p);
        return;
    }
    switch (c) {
        case 1:  // Ctrl+A
            TESelectAll(SearchEdit(p));
            break;
        case 3:  // Ctrl+C
            TECopy(SearchEdit(p), p.hwnd);
            break;
        case 24:  // Ctrl+X
            if (TEHasSel(SearchEdit(p))) {
                TECopy(SearchEdit(p), p.hwnd);
                TEDeleteSel(SearchEdit(p));
                SearchEdited(p);
                return;
            }
            break;
        case 22:  // Ctrl+V
            TEPaste(SearchEdit(p), p.hwnd);
            SearchEdited(p);
            return;
        case 8:  // Backspace
            if (HasSearchSel(p) || p.searchCaret > 0) {
                TEBackspace(SearchEdit(p));
                SearchEdited(p);
                return;
            }
            break;
        default:
            break;
    }
    StartAnim(p);
}

bool SearchKeyDown(Panel& p, WPARAM vk) {
    if (!p.searchFocused) {
        return false;
    }
    p.searchCaretOn = true;
    switch (vk) {
        case VK_ESCAPE:
            // While the field has the focus, Escape clears the text, then leaves the field.
            // It does not close the panel.
            if (!p.searchText.empty()) {
                p.searchText.clear();
                p.searchCaret = 0;
                p.searchSelAnchor = -1;
                SearchEdited(p);
            } else {
                SetSearchFocus(p, false);
            }
            return true;
        case VK_RETURN:
            return true;
        case VK_DELETE:
            TEDeleteFwd(SearchEdit(p));
            SearchEdited(p);
            return true;
        case VK_LEFT:
        case VK_RIGHT:
        case VK_HOME:
        case VK_END:
            TEMove(SearchEdit(p), vk, (GetKeyState(VK_SHIFT) & 0x8000) != 0,
                   (GetKeyState(VK_CONTROL) & 0x8000) != 0);
            StartAnim(p);
            return true;
        default:
            return false;
    }
}

void DrawSearchField(Panel& p) {
    const Theme& t = p.th;
    const D2D1_RECT_F f = SearchRect();
    const float textLeft = f.left + kSearchTextLeft;

    // Same field look as the category dialog: a slightly lighter fill while focused (dark theme).
    D2D1_COLOR_F fill = t.fieldBg;
    if (p.searchFocused && t.dark) {
        fill.r += (1.0f - fill.r) * 0.06f;
        fill.g += (1.0f - fill.g) * 0.06f;
        fill.b += (1.0f - fill.b) * 0.06f;
    }
    FillRR(p, f, kRadiusCtl, fill);
    StrokeRR(p, f, kRadiusCtl, t.cardBorder);
    if (p.searchFocused) {
        // Square bottom corners, so the accent rule runs flush into the sides.
        const float corner = kRadiusCtl;
        FillRect(p, Rc(f.left, f.bottom - corner, f.right - f.left, corner), fill);
        FillRect(p, Rc(f.left, f.bottom - corner, 1.0f, corner - 2.0f), t.cardBorder);
        FillRect(p, Rc(f.right - 1.0f, f.bottom - corner, 1.0f, corner - 2.0f), t.cardBorder);
    }

    DrawTxt(p, L"\xE721", p.fIcon.p, Rc(f.left + 6, f.top, 20, f.bottom - f.top),
            t.textSecondary);

    if (HasSearchSel(p)) {
        int a = 0, b = 0;
        SearchSelRange(p, a, b);
        const float x0 = SearchTextX(p, a);
        const float x1 = SearchTextX(p, b);
        FillRect(p, Rc(x0, f.top + 7, x1 - x0, f.bottom - f.top - 14),
                 WithAlpha(t.accent, 0.4f));
    }
    const D2D1_RECT_F textBox =
        Rc(textLeft, f.top, f.right - textLeft - 10, f.bottom - f.top - 2);
    if (p.searchText.empty()) {
        DrawTxt(p, L"Search apps", p.fButton.p, textBox, WithAlpha(t.textSecondary, 0.8f));
    } else {
        DrawTxt(p, p.searchText, p.fButton.p, textBox, t.text);
    }
    if (p.searchFocused && p.searchCaretOn) {
        const float cx = SearchTextX(p, p.searchCaret);
        FillRect(p, Rc(cx, f.top + 7, 1.0f, f.bottom - f.top - 14), t.text);
    }

    if (p.searchFocused) {
        // Accent rule at the bottom, a bit lighter, as in the dialog field.
        D2D1_COLOR_F lit = t.accent;
        lit.r += (1.0f - lit.r) * 0.25f;
        lit.g += (1.0f - lit.g) * 0.25f;
        lit.b += (1.0f - lit.b) * 0.25f;
        FillRect(p, Rc(f.left, f.bottom - 2, f.right - f.left, 2), lit);
    }
}

// Text of the Diagnostics strip and of "Copy all logs". Built from the current
// state, and rebuilt about once a second while the strip is shown.
void BuildDiagReport(Panel& p) {
    std::wstring s;
    auto line = [&s](const std::wstring& t) {
        s += t;
        s += L"\n";
    };
    line(std::wstring(L"PWA Tray Center ") + kAppVersion);
    line(L"Windows build " + std::to_wstring(GetWindowsBuild()));
    line(std::wstring(L"Center: ") + kOptPanelMode[g_panelMode.load()] + L", backdrop " +
         kOptBackdrop[g_backdrop.load()]);
    line(L"Shortcut: " + HotkeyText(g_hotkey.load()));
    line(L"Library: " + std::to_wstring(g_library.size()) + L" apps, " +
         std::to_wstring(g_categories.size()) + L" categories");
    line(L"PWA windows: " + std::to_wstring(p.all.size()) + L" (" +
         std::to_wstring(p.hiddenIdx.size()) + L" hidden)");
    for (const AppInfo& a : p.all) {
        std::wstring row = std::wstring(L"  ") + (a.hidden ? L"hidden " : L"shown  ") +
                           a.name + L" [" + a.crxId;
        if (!a.profile.empty()) {
            row += L", " + a.profile;
        }
        row += L"]";
        if (a.unread > 0) {
            row += L" unread " + std::to_wstring(a.unread);
        }
        line(row);
    }
    line(L"Media sessions: " + std::to_wstring(p.media.size()));
    for (const MediaInfo& m : p.media) {
        line(std::wstring(L"  ") + (m.playing ? L"playing " : L"paused  ") +
             MediaSourceName(p, m) + L": " + m.title);
    }
    line(L"Notifications: " + std::to_wstring(p.toasts.size()) + L" toasts, " +
         std::to_wstring(p.rows.size()) + L" rows");
    line(std::wstring(L"Chrome volume: ") +
         (p.volKnown ? std::to_wstring((int)std::lround(p.volFrac * 100.0)) + L"%" +
                           (p.volMute ? L", muted" : L"")
                     : std::wstring(L"no audio session yet")));
    line(std::wstring(L"Volume bar: ") + (g_showVolume ? L"on" : L"off"));

    p.diagReport = s;
    p.diagTick = GetTickCount64();
}

// Draws the content of one tab.
void DrawTabContent(Panel& p, int tab, float top, float bottom) {
    switch (tab) {
        case kTabMedia:
            DrawMediaTab(p, top, bottom);
            break;
        case kTabApps:
            DrawAppsTab(p, top, bottom);
            break;
        case kTabNotifs:
            DrawNotifsTab(p, top, bottom);
            break;
        case kTabLibrary:
            DrawLibraryTab(p, top, bottom);
            break;
        case kTabSettings:
            DrawSettingsTab(p, top, bottom);
            break;
    }
}

void DrawPanel(Panel& p) {
    ID2D1RenderTarget* rt = p.rt.p;
    if (!p.target) {
        return;
    }
    rt->SetDpi((float)(p.dpi * p.ui), (float)(p.dpi * p.ui));
    rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    p.animating = UpdateAnims(p, NowMs());

    const Theme& t = p.th;
    p.hits.clear();
    p.hitClip = Rc(0, 0, kPanelW, p.hDip);
    p.hitsOn = !p.menuOpen && !p.dlgOpen;

    rt->BeginDraw();
    D2D1_COLOR_F bg = p.solid ? t.bgSolid : D2D1_COLOR_F{0, 0, 0, 0};
    rt->Clear(&bg);

    // Mica context menu: everything except a rounded hole under the menu goes
    // through a layer, so the hole stays transparent and the backdrop shows through it.
    ID2D1RectangleGeometry* gOuter = nullptr;
    ID2D1RoundedRectangleGeometry* gHole = nullptr;
    ID2D1GeometryGroup* gGroup = nullptr;
    bool layerOn = false;
    if (p.menuOpen && !p.solid) {
        D2D1_RECT_F hr = p.menuRect;
        hr.top += p.menuDy.v;
        hr.bottom += p.menuDy.v;
        D2D1_ROUNDED_RECT hrr = {hr, kRadiusOverlay, kRadiusOverlay};
        D2D1_RECT_F outer = Rc(-50, -50, kPanelW + 100, p.hDip + 100);
        if (SUCCEEDED(g_d2d->CreateRectangleGeometry(&outer, &gOuter)) &&
            SUCCEEDED(g_d2d->CreateRoundedRectangleGeometry(&hrr, &gHole))) {
            ID2D1Geometry* arr[2] = {gOuter, gHole};
            if (SUCCEEDED(g_d2d->CreateGeometryGroup(D2D1_FILL_MODE_ALTERNATE,
                                                     arr, 2, &gGroup))) {
                D2D1_LAYER_PARAMETERS lp = {};
                lp.contentBounds = D2D1_RECT_F{-1e6f, -1e6f, 1e6f, 1e6f};
                lp.geometricMask = gGroup;
                lp.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
                lp.maskTransform.m11 = 1.0f;
                lp.maskTransform.m22 = 1.0f;
                lp.opacity = 1.0f;
                lp.opacityBrush = nullptr;
                lp.layerOptions = D2D1_LAYER_OPTIONS_NONE;
                rt->PushLayer(&lp, nullptr);
                layerOn = true;
            }
        }
    }

    p.ox = 0;
    p.oy = p.openDy.v;
    p.alpha = p.openAlpha.v;

    DrawTitleBar(p);
    DrawTabs(p);

    // Content of the selected tab.
    const float top = kContentTop;
    const float bottom = kContentBottom;
    D2D1_RECT_F clip = Rc(0, top, kPanelW, bottom - top);
    PushClip(p, clip);
    D2D1_RECT_F saveClip = p.hitClip;
    p.hitClip = clip;
    // The tab being left slides out and fades (no input); the new one slides in.
    if (p.exitTab >= 0 && p.exitTab != p.selectedTab && p.exitA.v > 0.001f) {
        p.ox = p.exitX.v;
        p.alpha = p.openAlpha.v * p.exitA.v;
        const bool saveHits = p.hitsOn;
        p.hitsOn = false;
        DrawTabContent(p, p.exitTab, top, bottom);
        p.hitsOn = saveHits;
    }
    p.ox = p.slideX.v;
    p.alpha = p.openAlpha.v * p.slideA.v;
    DrawTabContent(p, p.selectedTab, top, bottom);
    ClampScroll(p, p.selectedTab);
    DrawScrollbar(p, top, p.selectedTab == kTabLibrary ? kLibGridBottom : bottom);
    p.ox = 0;
    p.alpha = p.openAlpha.v;
    p.hitClip = saveClip;
    PopClip(p);

    // Library search field, at the bottom edge. It slides with the tab like the content.
    if (p.selectedTab == kTabLibrary) {
        p.ox = p.slideX.v;
        p.alpha = p.openAlpha.v * p.slideA.v;
        DrawSearchField(p);
        p.ox = 0;
        p.alpha = p.openAlpha.v;
    }

    // Diagnostics: the report, scrolled with the wheel, copied from the menu.
    if (g_showDiag) {
        if (GetTickCount64() - p.diagTick >= 1000) {
            BuildDiagReport(p);
        }
        const D2D1_RECT_F box = Rc(kPad, kPanelH + 4, kPanelW - 2 * kPad, kDiagH - 8);
        DrawLine(p, kPad, kPanelH, kPanelW - kPad, kPanelH, t.divider);
        AddHit(p, MakeId(kHitDiag, 0), box);
        const float textH = MeasureH(p.diagReport, p.fDiag.p, box.right - box.left);
        p.diagMax = (std::max)(0.0f, textH - (box.bottom - box.top));
        p.diagScroll = (std::max)(0.0f, (std::min)(p.diagScroll, p.diagMax));
        PushClip(p, box);
        DrawTxt(p, p.diagReport, p.fDiag.p,
                Rc(box.left, box.top - p.diagScroll, box.right - box.left, textH + 4),
                t.textSecondary);
        PopClip(p);
    }

    if (layerOn) {
        rt->PopLayer();
    }

    // Overlays.
    p.oy = 0;
    p.alpha = 1;
    if (p.menuOpen) {
        p.hitsOn = true;
        AddHit(p, MakeId(kHitNone, 2), Rc(0, 0, kPanelW, p.hDip));  // outside
        DrawMenu(p);
    }
    if (p.dlgOpen) {
        p.hitsOn = true;
        DrawDialog(p);
    }
    p.hitsOn = true;
    DrawNotes(p);
    p.oy = p.openDy.v;
    p.alpha = p.openAlpha.v;

    rt->EndDraw(nullptr, nullptr);
    // Present: the composition swap chain is shown with its alpha.
    p.swap->Present(0, 0);
    if (p.dcomp) {
        p.dcomp->Commit();
    }
    if (gGroup) {
        gGroup->Release();
    }
    if (gHole) {
        gHole->Release();
    }
    if (gOuter) {
        gOuter->Release();
    }
}

// ---------------------------------------------------------------------------
// Panel: actions
// ---------------------------------------------------------------------------

void RequestClosePanel() {
    if (g_panel && g_panel->hwnd) {
        PostMessageW(g_panel->hwnd, WM_CLOSE, 0, 0);
    }
}

bool IsHermes() {
    return g_panelMode == (int)PanelMode::Hermes;
}

// Hermes closes the panel after a show/open action, Titan never does.
void CloseIfHermes() {
    if (IsHermes()) {
        RequestClosePanel();
    }
}

// Closes the panel right now. Only for the host window procedure: the panel
// must not be used afterwards.
void ClosePanelLocal() {
    if (g_panel && g_panel->hwnd && IsWindow(g_panel->hwnd)) {
        if (g_panel->nativeMenuActive) {
            // The native menu is inside TrackPopupMenu and still uses the
            // panel. Dismiss it, the panel is destroyed right after it returns.
            g_panel->closeAfterMenu = true;
            EndMenu();
            return;
        }
        DestroyWindow(g_panel->hwnd);
    }
}


// Moves the tab indicator under the selected tab, animated or at once.
void SyncIndicator(Panel& p, bool animate) {
    const float c = SlotX(SlotOf(p, p.selectedTab)) + TabW() / 2;
    const float L = c - 10.0f;
    const float R = c + 10.0f;
    const ULONGLONG now = NowMs();
    if (!animate) {
        p.indL.Jump(now, L);
        p.indR.Jump(now, R);
        return;
    }
    // The indicator stretches: the leading edge goes at once, the trailing edge follows
    // 50 ms later, so it grows while it travels and then shortens to its usual 20 px.
    const float oldC = (p.indL.Value(now) + p.indR.Value(now)) / 2;
    if (c < oldC - 0.5f) {
        p.indL.Start(now, L, 250, 0);
        p.indR.Start(now, R, 250, 50);
    } else {
        p.indR.Start(now, R, 250, 0);
        p.indL.Start(now, L, 250, 50);
    }
}

void SaveTabOrder(const Panel& p) {
    Wh_SetStringValue(L"tabOrder", SerializeTabOrder(p.order).c_str());
}

// Moves `tab` to `slot` in the order and animates the others out of the way.
void MoveTabToSlot(Panel& p, int tab, int slot) {
    int from = SlotOf(p, tab);
    if (from == slot) {
        return;
    }
    int tmp[kTabCount];
    int n = 0;
    for (int i = 0; i < kTabCount; ++i) {
        if (p.order[i] != tab) {
            tmp[n++] = p.order[i];
        }
    }
    int out[kTabCount];
    int k = 0;
    for (int i = 0; i < kTabCount; ++i) {
        out[i] = i == slot ? tab : tmp[k++];
    }
    for (int i = 0; i < kTabCount; ++i) {
        p.order[i] = out[i];
    }
    for (int t = 0; t < kTabCount; ++t) {
        if (t == p.dragTab && p.dragging) {
            continue;
        }
        TweenSet(p.tabPos[t], SlotX(SlotOf(p, t)), 167, kEaseOut);
    }
    if (!(p.dragging && tab == p.selectedTab)) {
        SyncIndicator(p, true);
    }
    StartAnim(p);
}

void SelectTab(Panel& p, int tab) {
    if (tab == p.selectedTab) {
        return;
    }
    int dir = SlotOf(p, tab) > SlotOf(p, p.selectedTab) ? 1 : -1;
    p.exitTab = p.selectedTab;
    TweenJump(p.exitX, 0.0f);
    TweenSet(p.exitX, -28.0f * dir, 83, kEaseIn);
    TweenJump(p.exitA, 1.0f);
    TweenSet(p.exitA, 0.0f, 83, kEaseIn);
    p.selectedTab = tab;
    SyncIndicator(p, true);
    TweenJump(p.slideX, 28.0f * dir);
    TweenSet(p.slideX, 0.0f, 167, kEaseOut);
    TweenJump(p.slideA, 0.0f);
    TweenSet(p.slideA, 1.0f, 167, kEaseOut);
    Wh_SetIntValue(L"lastTab", tab);
    if (tab == kTabLibrary && GetTickCount64() - g_libraryTick > 3000) {
        g_library = ScanLibrary();
        g_libraryTick = GetTickCount64();
    }
    StartAnim(p);
}

void ShowNativeMenu(Panel& p, std::vector<MenuItem> items, const MenuCtx& ctx);

// The part of the panel that is really on screen (the panel can be dragged
// close to, or partly over, the edge of a monitor), in panel DIPs and with a
// margin. (px, py) is a point of the panel, it selects the monitor.
bool GetMenuArea(const Panel& p, float px, float py, D2D1_RECT_F& area) {
    RECT wr = {};
    if (!p.hwnd || !GetWindowRect(p.hwnd, &wr)) {
        return false;
    }
    const float s = DipPerDevicePx(p);  // DIPs per device pixel
    POINT pt = {wr.left + (LONG)std::lround(px / s),
                wr.top + (LONG)std::lround(py / s)};
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(mon, &mi)) {
        return false;
    }
    const float m = 8.0f;
    area.left = (std::max)(0.0f, (mi.rcMonitor.left - wr.left) * s) + m;
    area.top = (std::max)(0.0f, (mi.rcMonitor.top - wr.top) * s) + m;
    area.right = (std::min)(kPanelW, (mi.rcMonitor.right - wr.left) * s) - m;
    area.bottom = (std::min)(p.hDip, (mi.rcMonitor.bottom - wr.top) * s) - m;
    return true;
}

// Position of a menu along one axis: where the click is if it fits, else
// flipped to the other side of the click, else pushed into [lo, hi].
float PlaceMenuAxis(float pos, float size, float lo, float hi) {
    if (pos >= lo && pos + size <= hi) {
        return pos;
    }
    float flipped = pos - size;
    if (flipped >= lo && flipped + size <= hi) {
        return flipped;
    }
    return (std::max)(lo, (std::min)(pos, hi - size));
}

void OpenMenu(Panel& p, std::vector<MenuItem> items, const MenuCtx& ctx, float x, float y) {
    float w = kMenuW;
    float h = 2 * kMenuPad;
    for (const auto& it : items) {
        h += it.sep ? kMenuSepH : kMenuItemH;
    }
    D2D1_RECT_F area = {};
    if (!GetMenuArea(p, x, y, area) || area.right - area.left < w ||
        area.bottom - area.top < h) {
        // Does not fit into the visible part of the panel at all.
        ShowNativeMenu(p, std::move(items), ctx);
        return;
    }
    x = PlaceMenuAxis(x, w, area.left, area.right);
    y = PlaceMenuAxis(y, h, area.top, area.bottom);
    p.menuItems = std::move(items);
    p.menuCtx = ctx;
    p.menuRect = Rc(x, y, w, h);
    p.menuOpen = true;
    // Windows 11 style: fade in and slide down a little (Fluent "enter").
    TweenJump(p.menuT, 0.0f);
    TweenSet(p.menuT, 1.0f, 167, kEaseOut);
    TweenJump(p.menuDy, -10.0f);
    TweenSet(p.menuDy, 0.0f, 167, kEaseOut);
    SetHover(p, kHitNone);
    StartAnim(p);
}

void CloseMenu(Panel& p) {
    p.menuOpen = false;
    p.menuItems.clear();
    p.pressed = kHitNone;
    SetHover(p, kHitNone);
    StartAnim(p);
}

// Dialog height: the fields and buttons plus one line per warning (at least one).
float DialogH(size_t lines) {
    return 156.0f + 16.0f * (float)(lines > 1 ? lines : 1);
}

void CloseDialog(Panel& p) {
    p.dlgOpen = false;
    p.dlgText.clear();
    p.dlgLog.clear();
    p.dlgSelAnchor = -1;
    p.dlgSelDragging = false;
    p.dlgTyping = false;
    p.dlgUndo.clear();
    p.dlgRedo.clear();
    p.dlgDupKey.clear();
    p.dlgDupCount = 0;
    p.dlgCaret = 0;
    p.dlgFocused = false;
    SyncCaretTimer(p);
    p.dlgAddCrx.clear();
    p.pressed = kHitNone;
    SetFocus(p.hwnd);
    StartAnim(p);
}

void OpenDialog(Panel& p, bool rename, int catIdx, const std::wstring& addCrx) {
    if (p.dlgOpen) {
        return;
    }
    p.dlgOpen = true;
    p.dlgRename = rename;
    p.dlgCat = catIdx;
    p.dlgAddCrx = addCrx;
    float w = kPanelW - 48;
    float h = DialogH(0);
    p.dlgRect = Rc(24, (kPanelH - h) / 2 - 20, w, h);
    p.dlgField = Rc(p.dlgRect.left + 16, p.dlgRect.top + 48, w - 32, 36);

    p.dlgText = rename && catIdx >= 0 && catIdx < (int)g_categories.size()
                    ? g_categories[catIdx].name
                    : L"";
    p.dlgCaret = (int)p.dlgText.size();
    p.dlgSelAnchor = -1;
    p.dlgFocused = true;
    p.dlgCaretOn = true;
    SyncCaretTimer(p);
    SetFocus(p.hwnd);
    SetHover(p, kHitNone);
    StartAnim(p);
}

bool CategoryNameTaken(const std::wstring& name, int exceptIdx) {
    for (size_t i = 0; i < g_categories.size(); ++i) {
        if ((int)i != exceptIdx && ToLower(g_categories[i].name) == ToLower(name)) {
            return true;
        }
    }
    return false;
}

// Tries of the same existing name in one open dialog. Attempts 1-9 show the plain
// warning. "Загадка" (the riddle) starts on attempt 10 (four messages), and
// attempt 14 closes the dialog and makes the "142857" category.
constexpr int kRiddleStart = 10;
constexpr int kRiddleEnd = 14;

void DialogCommit(Panel& p) {
    if (!p.dlgOpen) {
        return;
    }
    std::wstring name = SanitizeCategoryName(p.dlgText);
    if (name.empty()) {
        return;
    }
    if (CategoryNameTaken(name, p.dlgRename ? p.dlgCat : -1)) {
        // Same name again in this open dialog: count it. A different name starts over.
        const std::wstring key = ToLower(name);
        if (key == p.dlgDupKey) {
            ++p.dlgDupCount;
        } else {
            p.dlgDupKey = key;
            p.dlgDupCount = 1;
            p.dlgLog.clear();
        }

        // The riddle only runs while "142857" can still be created.
        const bool riddleOn = !CategoryNameTaken(L"142857", -1);

        if (riddleOn && p.dlgDupCount >= kRiddleEnd) {
            // The last try of the riddle: the panel makes its own category instead.
            const std::wstring fallback = L"142857";
            if (!CategoryNameTaken(fallback, -1)) {
                Category c;
                c.name = fallback;
                g_categories.push_back(std::move(c));
                SaveCategories();
                PushNote(p, NotifSeverity::Success, L"Category created",
                         L"\"" + fallback + L"\" was added to the list.", kGlyphAdd);
                p.chipSel = (int)g_categories.size();
                p.scrollT[kTabLibrary] = 0;
                p.chipScrollT = 1e6f;
            }
            CloseDialog(p);
            return;
        }

        if (p.dlgDupCount == 1) {
            p.dlgLog.push_back(L"\"" + name + L"\" already exists.");
        } else if (riddleOn && p.dlgDupCount >= kRiddleStart) {
            static const wchar_t* kRiddleLines[] = {
                L"Serious?",
                L"Are you going to stop?",
                L"...",
                L"One more and I'll do it for you.",
            };
            p.dlgLog.push_back(kRiddleLines[p.dlgDupCount - kRiddleStart]);
        }
        p.dlgRect.bottom = p.dlgRect.top + DialogH(p.dlgLog.size());
        StartAnim(p);
        return;
    }
    if (p.dlgRename) {
        if (p.dlgCat >= 0 && p.dlgCat < (int)g_categories.size()) {
            g_categories[p.dlgCat].name = name;
            PushNote(p, NotifSeverity::Success, L"Category renamed",
                     L"Now called \"" + name + L"\".", kGlyphRename);
        }
    } else {
        Category c;
        c.name = name;
        if (!p.dlgAddCrx.empty()) {
            c.ids.push_back(p.dlgAddCrx);
        }
        g_categories.push_back(std::move(c));
        PushNote(p, NotifSeverity::Success, L"Category created",
                 L"\"" + name + L"\" was added to the list.", kGlyphAdd);
        p.chipSel = (int)g_categories.size();
        p.scrollT[kTabLibrary] = 0;
        p.chipScrollT = 1e6f;  // clamped to the real end in DrawChips
    }
    SaveCategories();
    CloseDialog(p);
}

// Text input of the dialog: typed characters, Backspace, Ctrl+V.

void PushDlgUndo(Panel& p) {
    p.dlgUndo.push_back({p.dlgText, p.dlgCaret});
    if (p.dlgUndo.size() > 100) {
        p.dlgUndo.erase(p.dlgUndo.begin());
    }
    p.dlgRedo.clear();
}

void DeleteDlgSelection(Panel& p) {
    TEDeleteSel(DlgEdit(p));
}

void InsertDlgText(Panel& p, const std::wstring& in) {
    TEInsert(DlgEdit(p), in);
}

void CopyDlgSelection(Panel& p) {
    TECopy(DlgEdit(p), p.hwnd);
}

void PasteDlg(Panel& p) {
    const std::wstring text = ReadClipboardText(p.hwnd);
    if (!text.empty()) {
        PushDlgUndo(p);
        InsertDlgText(p, text);
    }
}

void DlgUndo(Panel& p) {
    if (p.dlgUndo.empty()) {
        return;
    }
    p.dlgRedo.push_back({p.dlgText, p.dlgCaret});
    Panel::DlgSnap s = p.dlgUndo.back();
    p.dlgUndo.pop_back();
    p.dlgText = s.text;
    p.dlgCaret = s.caret;
    p.dlgSelAnchor = -1;
}

void DlgRedo(Panel& p) {
    if (p.dlgRedo.empty()) {
        return;
    }
    p.dlgUndo.push_back({p.dlgText, p.dlgCaret});
    Panel::DlgSnap s = p.dlgRedo.back();
    p.dlgRedo.pop_back();
    p.dlgText = s.text;
    p.dlgCaret = s.caret;
    p.dlgSelAnchor = -1;
}

void DialogChar(Panel& p, wchar_t c) {
    if (!p.dlgFocused) {
        return;
    }
    p.dlgCaretOn = true;

    if (c >= 32 && c != 127) {
        // Typing. A run of typed characters is one undo step.
        if (!p.dlgTyping || HasDlgSel(p)) {
            PushDlgUndo(p);
        }
        InsertDlgText(p, std::wstring(1, c));
        p.dlgTyping = true;
    } else {
        p.dlgTyping = false;
        switch (c) {
            case 1:  // Ctrl+A
                TESelectAll(DlgEdit(p));
                break;
            case 3:  // Ctrl+C
                CopyDlgSelection(p);
                break;
            case 24:  // Ctrl+X
                if (HasDlgSel(p)) {
                    CopyDlgSelection(p);
                    PushDlgUndo(p);
                    DeleteDlgSelection(p);
                }
                break;
            case 22:  // Ctrl+V
                PasteDlg(p);
                break;
            case 26:  // Ctrl+Z
                DlgUndo(p);
                break;
            case 25:  // Ctrl+Y
                DlgRedo(p);
                break;
            case 8:  // Backspace
                if (HasDlgSel(p) || p.dlgCaret > 0) {
                    PushDlgUndo(p);
                    TEBackspace(DlgEdit(p));
                }
                break;
            default:
                break;
        }
    }
    StartAnim(p);
}

bool DialogKeyDown(Panel& p, WPARAM vk) {
    if (vk != VK_RETURN && vk != VK_ESCAPE && !p.dlgFocused) {
        return false;
    }
    p.dlgCaretOn = true;
    switch (vk) {
        case VK_RETURN:
            DialogCommit(p);
            return true;
        case VK_ESCAPE:
            CloseDialog(p);
            return true;
        case VK_LEFT:
        case VK_RIGHT:
        case VK_HOME:
        case VK_END:
            p.dlgTyping = false;
            TEMove(DlgEdit(p), vk, (GetKeyState(VK_SHIFT) & 0x8000) != 0,
                   (GetKeyState(VK_CONTROL) & 0x8000) != 0);
            break;
        case VK_DELETE:
            p.dlgTyping = false;
            if (HasDlgSel(p) || p.dlgCaret < (int)p.dlgText.size()) {
                PushDlgUndo(p);
                TEDeleteFwd(DlgEdit(p));
            }
            break;
        default:
            return false;
    }
    StartAnim(p);
    return true;
}

void ToggleMember(int catIdx, const std::wstring& crxId) {
    if (catIdx < 0 || catIdx >= (int)g_categories.size() || crxId.empty()) {
        return;
    }
    auto& ids = g_categories[catIdx].ids;
    auto it = std::find(ids.begin(), ids.end(), crxId);
    if (it == ids.end()) {
        ids.push_back(crxId);
    } else {
        ids.erase(it);
    }
    SaveCategories();
}

std::wstring PanelAppName(const Panel& p, HWND hwnd) {
    for (const auto& a : p.all) {
        if (a.hwnd == hwnd) {
            return a.name;
        }
    }
    return L"PWA";
}

void LaunchOrRestore(Panel& p, int libIdx) {
    if (libIdx < 0 || libIdx >= (int)g_library.size()) {
        return;
    }
    const LibApp& l = g_library[libIdx];
    PushNote(p, NotifSeverity::Info, L"Opening app", l.name, nullptr, LibBitmap(p, l));
    if (const AppInfo* run = FindRunning(p, l)) {
        RequestRestore(run->hwnd);
    } else {
        LaunchApp(l);
    }
}

void Activate(Panel& p, int hit);
void ApplyUserVolume(Panel& p, bool persist);

// Forward declaration: the hook procedure is defined further down.
LRESULT CALLBACK MouseHookProc(int code, WPARAM wParam, LPARAM lParam);

// Hermes needs the low-level mouse hook (it closes on an outside press), Titan
// does not. Follows the mode, also while the panel is open.
// ---------------------------------------------------------------------------
// Mouse hooks run on their own thread, which only pumps messages. A hook proc
// never waits for anything: it posts to the host window, and the panel's thread
// decides. So a busy panel or browser cannot make Windows drop the hooks.
// ---------------------------------------------------------------------------

bool IsMouseVk(UINT vk);
LRESULT CALLBACK HotkeyMouseHookProc(int code, WPARAM wParam, LPARAM lParam);

std::atomic<bool> g_wantHermesHook{false};  // the panel is open in Hermes mode
std::atomic<bool> g_hookListening{false};   // the open shortcut is being chosen
std::atomic<DWORD> g_hookThreadId{0};
HANDLE g_hookThread = nullptr;
std::mutex g_hookThreadMutex;

// Makes the hooks match the settings. Hook thread only.
void SyncHooks() {
    const bool wantHermes = g_wantHermesHook.load();
    if (wantHermes && !g_mouseHook) {
        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc,
                                        GetModuleHandleW(nullptr), 0);
    } else if (!wantHermes && g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }

    const bool wantHotkey =
        IsMouseVk((UINT)g_hotkey.load() & 0xFFFF) || g_hookListening.load();
    if (wantHotkey && !g_mouseHotkeyHook) {
        g_mouseHotkeyHook = SetWindowsHookExW(WH_MOUSE_LL, HotkeyMouseHookProc,
                                              GetModuleHandleW(nullptr), 0);
    } else if (!wantHotkey && g_mouseHotkeyHook) {
        UnhookWindowsHookEx(g_mouseHotkeyHook);
        g_mouseHotkeyHook = nullptr;
    }
}

DWORD WINAPI HookThreadProc(LPVOID) {
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // creates the message queue
    g_hookThreadId = GetCurrentThreadId();
    if (!g_stopping) {
        SyncHooks();
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            if (msg.message == kMsgHookSync) {
                SyncHooks();
            }
        }
    }
    // WM_QUIT: the hooks go with this thread.
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    if (g_mouseHotkeyHook) {
        UnhookWindowsHookEx(g_mouseHotkeyHook);
        g_mouseHotkeyHook = nullptr;
    }
    g_hookThreadId = 0;
    return 0;
}

void EnsureHookThread() {
    std::lock_guard<std::mutex> lock(g_hookThreadMutex);
    if (g_stopping || g_hookThread) {
        return;
    }
    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, nullptr);
}

// Asks the hook thread to make its hooks match the settings. Any thread.
void RequestHookSync() {
    EnsureHookThread();
    if (DWORD tid = g_hookThreadId.load()) {
        PostThreadMessageW(tid, kMsgHookSync, 0, 0);
    }
}

void StopHookThread() {
    std::lock_guard<std::mutex> lock(g_hookThreadMutex);
    if (!g_hookThread) {
        return;
    }
    if (DWORD tid = g_hookThreadId.load()) {
        PostThreadMessageW(tid, WM_QUIT, 0, 0);
    }
    WaitForSingleObject(g_hookThread, INFINITE);
    CloseHandle(g_hookThread);
    g_hookThread = nullptr;
}

// Hermes needs the outside-press hook while its panel is open. Panel thread.
void UpdateModeHook() {
    g_wantHermesHook = g_panel && IsHermes();
    RequestHookSync();
}

// Resizes the open panel for the diagnostics strip, keeping its bottom edge.
void ResizePanelForDiag(Panel& p) {
    p.hDip = kPanelH + (g_showDiag ? kDiagH : 0.0f);
    p.diagTick = 0;
    const int newH = (int)std::lround(p.hDip * p.dpi * p.ui / 96.0f);
    RECT wr = {};
    GetWindowRect(p.hwnd, &wr);
    const int top = p.bottomAnchored ? wr.bottom - newH : wr.top;
    p.pxH = newH;
    SetWindowPos(p.hwnd, nullptr, wr.left, top, p.pxW, newH,
                 SWP_NOZORDER | SWP_NOACTIVATE);
}

// Changes a preference from the Settings tab or a menu, and updates the panel.
void ApplyPref(Panel& p, int id, int value) {
    value = (std::clamp)(value, 0, PrefOptionCount(id) - 1);
    if (PrefIsToggle(id)) {
        TweenSet(ToggleTween(p, id), value ? 1.0f : 0.0f, 167, kEaseOut);
    }
    SetPref(id, value);
    switch (id) {
        case kPrefPanelMode:
            UpdateModeHook();
            break;
        case kPrefBackdrop:
            ApplyBackdrop(p);
            break;
        case kPrefMediaCompact:
            p.scrollT[kTabMedia] = 0;
            p.scrollC[kTabMedia] = 0;
            break;
        case kPrefTrayAlways:
            if (HWND host = g_hostHwnd.load()) {
                PostMessageW(host, kMsgListChanged, 0, 0);
            }
            break;
        case kPrefDiag:
            ResizePanelForDiag(p);
            break;
        default:
            break;
    }
    StartAnim(p);
}

void ExecMenuCmd(Panel& p, const MenuItem& it, const MenuCtx& ctx) {
    switch (it.cmd) {
        case kCmdMediaPlay:
            Activate(p, MakeId(kHitMediaPlay, it.arg));
            break;
        case kCmdMediaPrev:
            Activate(p, MakeId(kHitMediaPrev, it.arg));
            break;
        case kCmdMediaNext:
            Activate(p, MakeId(kHitMediaNext, it.arg));
            break;
        case kCmdCopyLogs:
            BuildDiagReport(p);
            SetClipboardText(p.hwnd, p.diagReport);
            PushNote(p, NotifSeverity::Success, L"Logs copied",
                     L"The diagnostics report is on the clipboard.");
            break;
        case kCmdToggleMediaView:
            ApplyPref(p, kPrefMediaCompact, g_mediaCompact ? 0 : 1);
            break;
        case kCmdSetPref:
            ApplyPref(p, it.arg / 16, it.arg % 16);
            break;
        case kCmdHide:
            RequestHide(ctx.hwnd);
            break;
        case kCmdShow:
        case kCmdShowPWA:
            PushNote(p, NotifSeverity::Info, L"Showing window",
                     PanelAppName(p, ctx.hwnd), nullptr, WindowBitmap(p, ctx.hwnd));
            RequestRestore(ctx.hwnd);
            CloseIfHermes();
            break;
        case kCmdQuit:
            RequestQuit(ctx.hwnd);
            break;
        case kCmdClosePWA:
            // Close PWA app from media card context menu
            RequestQuit(ctx.hwnd);
            break;
        case kCmdLaunch:
            LaunchOrRestore(p, ctx.libIndex);
            CloseIfHermes();
            break;
        case kCmdToggleCat:
            ToggleMember(it.arg, ctx.crxId);
            break;
        case kCmdNewCat:
            OpenDialog(p, false, -1, ctx.crxId);
            break;
        case kCmdRenameCat:
            OpenDialog(p, true, it.arg, L"");
            break;
        case kCmdDeleteCat:
            if (it.arg >= 0 && it.arg < (int)g_categories.size()) {
                const std::wstring removed = g_categories[it.arg].name;
                g_categories.erase(g_categories.begin() + it.arg);
                PushNote(p, NotifSeverity::Error, L"Category deleted",
                         L"\"" + removed + L"\" was removed. Apps were not affected.",
                         kGlyphDelete);
                if (p.chipSel == it.arg + 1) {
                    p.chipSel = 0;
                } else if (p.chipSel > it.arg + 1) {
                    --p.chipSel;
                }
                SaveCategories();
            }
            break;
        case kCmdMoveLeft:
        case kCmdMoveRight: {
            int slot = SlotOf(p, ctx.tab) + (it.cmd == kCmdMoveLeft ? -1 : 1);
            if (slot >= 0 && slot < kTabCount) {
                MoveTabToSlot(p, ctx.tab, slot);
                SaveTabOrder(p);
            }
            break;
        }
    }
}

// Fallback for a menu that does not fit into the visible part of the panel
// (the panel is next to a screen edge, or the menu is taller than the panel):
// a native popup menu, which the system keeps on screen by itself.
void ShowNativeMenu(Panel& p, std::vector<MenuItem> items, const MenuCtx& ctx) {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }
    for (size_t i = 0; i < items.size(); ++i) {
        const MenuItem& it = items[i];
        if (it.sep) {
            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            continue;
        }
        std::wstring text;
        for (wchar_t c : it.text) {
            text += c;
            if (c == L'&') {
                text += L'&';  // no accelerator from a category name
            }
        }
        UINT flags = MF_STRING | (it.checked ? MF_CHECKED : MF_UNCHECKED) |
                     (it.enabled && !it.header ? MF_ENABLED : MF_GRAYED);
        AppendMenuW(menu, flags, (UINT_PTR)(i + 1), text.c_str());
    }

    POINT pt = {};
    GetCursorPos(&pt);
    HWND hwnd = p.hwnd;
    p.pressed = kHitNone;
    SetHover(p, kHitNone);
    p.nativeMenuActive = true;
    p.closeAfterMenu = false;
    SetForegroundWindow(hwnd);
    UINT cmd = TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y,
                              0, hwnd, nullptr);
    PostMessageW(hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);
    p.nativeMenuActive = false;

    if (p.closeAfterMenu) {
        // Destroyed by the message loop, after this call chain has returned.
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
        return;
    }
    if (cmd >= 1 && cmd <= items.size()) {
        ExecMenuCmd(p, items[cmd - 1], ctx);
    }
    StartAnim(p);
}

MenuItem MakeItem(int cmd, const wchar_t* text, PCWSTR glyph = nullptr, int arg = 0) {
    MenuItem m;
    m.cmd = cmd;
    m.text = text;
    m.glyph = glyph;
    m.arg = arg;
    return m;
}

MenuItem MakeSep() {
    MenuItem m;
    m.sep = true;
    return m;
}

// Icon of the PWA a media session belongs to (or of the browser for the rest).
ID2D1Bitmap* MediaSourceBitmap(Panel& p, const MediaInfo& m) {
    if (m.matchedCrx.empty()) {
        return nullptr;
    }
    for (const AppInfo& a : p.all) {
        if (a.crxId == m.matchedCrx) {
            return AppBitmap(p, a);
        }
    }
    if (const LibApp* l = FindLib(m.matchedCrx, L"")) {
        return LibBitmap(p, *l);
    }
    return nullptr;
}

// The info row at the top of a menu: the icon and the name of a PWA window.
MenuItem WindowHeader(Panel& p, HWND hwnd) {
    MenuItem h;
    h.header = true;
    h.text = L"PWA";
    for (const AppInfo& a : p.all) {
        if (a.hwnd == hwnd) {
            h.text = a.name;
            h.bmp = AppBitmap(p, a);
            break;
        }
    }
    return h;
}

// The dropdown of a Settings row: its options as a menu, under the row (above
// it if there is no room below).
void OpenPrefMenu(Panel& p, int rowIdx) {
    if (rowIdx < 0 || rowIdx >= (int)p.settingRects.size()) {
        return;
    }
    const SettingRow& row = kSettingRows[rowIdx];
    const int cur = PrefValue(row.pref)->load();
    const int n = PrefOptionCount(row.pref);
    std::vector<MenuItem> items;
    for (int v = 0; v < n; ++v) {
        MenuItem m = MakeItem(kCmdSetPref, row.options[v], nullptr, row.pref * 16 + v);
        m.checked = v == cur;
        items.push_back(m);
    }
    const D2D1_RECT_F rr = p.settingRects[rowIdx];
    const float h = 2 * kMenuPad + kMenuItemH * n;
    float y = rr.bottom;
    D2D1_RECT_F area = {};
    if (GetMenuArea(p, rr.right, rr.bottom, area) && rr.bottom + h > area.bottom) {
        y = rr.top - h;
    }
    OpenMenu(p, std::move(items), MenuCtx(), rr.right - kMenuW, y);
}

void OnContextMenu(Panel& p, int hit, float x, float y) {
    int kind = KindOf(hit);
    int idx = IndexOf(hit);
    MenuCtx ctx;
    std::vector<MenuItem> items;

    if (kind == kHitTab) {
        ctx.tab = idx;
        int slot = SlotOf(p, idx);
        MenuItem l = MakeItem(kCmdMoveLeft, L"Move left", kGlyphChevL);
        l.enabled = slot > 0;
        MenuItem r = MakeItem(kCmdMoveRight, L"Move right", kGlyphChevR);
        r.enabled = slot < kTabCount - 1;
        items.push_back(l);
        items.push_back(r);
    } else if (kind == kHitTile && p.selectedTab == kTabApps) {
        if (idx >= (int)p.frameHwnds.size()) {
            return;
        }
        ctx.hwnd = p.frameHwnds[idx];
        items.push_back(WindowHeader(p, ctx.hwnd));
        if (p.frameHidden[idx]) {
            items.push_back(MakeItem(kCmdShow, L"Show", kGlyphOpen));
        } else {
            items.push_back(MakeItem(kCmdHide, L"Hide to PWA Tray Center"));
        }
        items.push_back(MakeSep());
        items.push_back(MakeItem(kCmdQuit, L"Quit", kGlyphClose));
    } else if (kind == kHitTile && p.selectedTab == kTabLibrary) {
        if (idx >= (int)p.frameLib.size()) {
            return;
        }
        ctx.libIndex = p.frameLib[idx];
        const LibApp& l = g_library[ctx.libIndex];
        ctx.crxId = l.crxId;
        const AppInfo* run = FindRunning(p, l);
        MenuItem head;
        head.header = true;
        head.text = l.name;
        head.bmp = LibBitmap(p, l);
        if (!head.bmp && run) {
            head.bmp = AppBitmap(p, *run);
        }
        items.push_back(head);
        items.push_back(MakeItem(kCmdLaunch, run ? L"Show" : L"Open", kGlyphOpen));
        if (run) {
            ctx.hwnd = run->hwnd;
            items.push_back(MakeItem(kCmdQuit, L"Quit", kGlyphClose));
        }
        items.push_back(MakeSep());
        for (size_t i = 0; i < g_categories.size(); ++i) {
            MenuItem m = MakeItem(kCmdToggleCat, g_categories[i].name.c_str(),
                                  kGlyphFolder, (int)i);
            m.checked = InCategory(g_categories[i], l.crxId);
            items.push_back(m);
        }
        items.push_back(MakeItem(kCmdNewCat, L"New category\x2026", kGlyphAdd));
    } else if (kind == kHitRow) {
        if (idx >= (int)p.frameHwnds.size()) {
            return;
        }
        ctx.hwnd = p.frameHwnds[idx];
        items.push_back(WindowHeader(p, ctx.hwnd));
        items.push_back(MakeItem(kCmdShow, L"Show", kGlyphOpen));
        items.push_back(MakeSep());
        items.push_back(MakeItem(kCmdQuit, L"Quit", kGlyphClose));
    } else if (kind == kHitDiag) {
        items.push_back(MakeItem(kCmdCopyLogs, L"Copy all logs"));
    } else if (kind == kHitChip) {
        if (idx < 1 || idx > (int)g_categories.size()) {
            return;
        }
        items.push_back(MakeItem(kCmdRenameCat, L"Rename", kGlyphRename, idx - 1));
        items.push_back(MakeSep());
        items.push_back(MakeItem(kCmdDeleteCat, L"Delete category", kGlyphDelete,
                                 idx - 1));
    } else if ((kind >= kHitMediaPrev && kind <= kHitMediaSeek) ||
               kind == kHitMediaCard) {
        if (idx < 0 || idx >= (int)p.media.size()) {
            return;
        }
        const MediaInfo& m = p.media[idx];
        // The source PWA as an info row with its icon, in both views.
        MenuItem src;
        src.header = true;
        src.text = MediaSourceName(p, m);
        src.bmp = MediaSourceBitmap(p, m);
        items.push_back(src);
        items.push_back(MakeSep());
        // The PWA window that owns this media session.
        HWND target = nullptr;
        if (!m.matchedCrx.empty()) {
            for (const AppInfo& app : p.all) {
                if (app.crxId == m.matchedCrx) {
                    target = app.hwnd;
                    if (app.hidden) {
                        break;
                    }
                }
            }
        }
        MenuItem mp = MakeItem(kCmdMediaPlay, m.playing ? L"Pause" : L"Play",
                               m.playing ? kGlyphPause : kGlyphPlay, idx);
        mp.enabled = m.canToggle;
        MenuItem mprev = MakeItem(kCmdMediaPrev, L"Previous", kGlyphPrev, idx);
        mprev.enabled = m.canPrev;
        MenuItem mnext = MakeItem(kCmdMediaNext, L"Next", kGlyphNext, idx);
        mnext.enabled = m.canNext;
        items.push_back(mp);
        items.push_back(mprev);
        items.push_back(mnext);
        if (target) {
            ctx.hwnd = target;
            items.push_back(MakeSep());
            items.push_back(MakeItem(kCmdShowPWA, L"Show window", kGlyphOpen));
            items.push_back(MakeItem(kCmdClosePWA, L"Quit app", kGlyphClose));
        }
    } else {
        return;
    }
    OpenMenu(p, std::move(items), ctx, x, y);
}

// Jumps to the Media tab and highlights the card (after 150 ms).
void ShowMediaHighlight(Panel& p, const std::wstring& key) {
    SelectTab(p, kTabMedia);
    p.hlKey = key;
    p.hlTick = NowMs();
    StartAnim(p);
}

// Mouse buttons cannot be registered with RegisterHotKey. A shortcut that is a
// mouse button is kept in the same value as keys (the button's Virtual-Key code
// in the low 16 bits, MOD_* flags above) and watched by a low-level mouse hook.
bool IsMouseVk(UINT vk) {
    return vk == VK_LBUTTON || vk == VK_RBUTTON || vk == VK_MBUTTON ||
           vk == VK_XBUTTON1 || vk == VK_XBUTTON2;
}

UINT CurrentMods() {
    UINT mods = 0;
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) mods |= MOD_CONTROL;
    if (GetAsyncKeyState(VK_MENU) & 0x8000) mods |= MOD_ALT;
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) mods |= MOD_SHIFT;
    if ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) mods |= MOD_WIN;
    return mods;
}

UINT g_swallowUp = 0;               // button whose press was used: its release is swallowed too

LRESULT CALLBACK HotkeyMouseHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    const MSLLHOOKSTRUCT* ms = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
    UINT vk = 0;
    bool down = false;
    switch (wParam) {
        case WM_LBUTTONDOWN: vk = VK_LBUTTON; down = true; break;
        case WM_LBUTTONUP: vk = VK_LBUTTON; break;
        case WM_RBUTTONDOWN: vk = VK_RBUTTON; down = true; break;
        case WM_RBUTTONUP: vk = VK_RBUTTON; break;
        case WM_MBUTTONDOWN: vk = VK_MBUTTON; down = true; break;
        case WM_MBUTTONUP: vk = VK_MBUTTON; break;
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            vk = HIWORD(ms->mouseData) == XBUTTON2 ? VK_XBUTTON2 : VK_XBUTTON1;
            down = wParam == WM_XBUTTONDOWN;
            break;
        default:
            break;
    }
    if (!vk) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    if (!down) {
        if (vk == g_swallowUp) {
            g_swallowUp = 0;
            return 1;
        }
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    HWND host = g_hostHwnd.load();
    if (!host) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    // While the shortcut is being chosen, a press with a modifier is the new shortcut.
    // A plain press goes through to the app; the panel decides what it means.
    if (g_hookListening.load()) {
        const UINT mods = CurrentMods();
        if (mods & (MOD_CONTROL | MOD_ALT | MOD_WIN)) {
            PostMessageW(host, kMsgMouseHotkey, (WPARAM)vk, (LPARAM)mods);
            g_swallowUp = vk;
            return 1;
        }
        // A plain press goes through; the panel checks whether it was outside.
        PostMessageW(host, kMsgPlainClick, (WPARAM)(uint32_t)ms->pt.x,
                     (LPARAM)(uint32_t)ms->pt.y);
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    // Otherwise, the shortcut that opens the panel.
    const UINT cur = (UINT)g_hotkey.load();
    if (IsMouseVk(cur & 0xFFFF) && vk == (cur & 0xFFFF) &&
        CurrentMods() == ((cur >> 16) & 0xFFFF)) {
        PostMessageW(host, WM_HOTKEY, kHotkeyId, 0);
        g_swallowUp = vk;
        return 1;
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Called on the panel's thread when the shortcut or the capture state changes.
void UpdateMouseHotkeyHook() {
    g_hookListening = g_panel && g_panel->hotkeyListening;
    RequestHookSync();
}

// Registers the shortcut on the host window (0 removes it). False if the system
// or another program already uses the combination. Mouse buttons are not
// registered: they go through the mouse hook.
bool ApplyHotkey(HWND host, int value) {
    UnregisterHotKey(host, kHotkeyId);
    if (value == 0 || IsMouseVk((UINT)value & 0xFFFF)) {
        return true;
    }
    const UINT vk = (UINT)value & 0xFFFF;
    const UINT mods = ((UINT)value >> 16) & 0xFFFF;
    return RegisterHotKey(host, kHotkeyId, mods | MOD_NOREPEAT, vk) != FALSE;
}

void CancelHotkeyCapture(Panel& p) {
    p.hotkeyListening = false;
    if (HWND host = g_hostHwnd.load()) {
        ApplyHotkey(host, g_hotkey.load());  // the old shortcut works again
    }
    UpdateMouseHotkeyHook();
    StartAnim(p);
}

void BeginHotkeyCapture(Panel& p) {
    p.hotkeyListening = true;
    UpdateMouseHotkeyHook();
    // Off while it is being chosen, so the old combination can be pressed.
    if (HWND host = g_hostHwnd.load()) {
        UnregisterHotKey(host, kHotkeyId);
    }
    StartAnim(p);
}

void ApplyHotkeyPref(Panel& p, int value) {
    p.hotkeyListening = false;
    UpdateMouseHotkeyHook();
    HWND host = g_hostHwnd.load();
    if (host && !ApplyHotkey(host, value)) {
        ApplyHotkey(host, g_hotkey.load());
        PushNote(p, NotifSeverity::Error, L"Shortcut is in use",
                 L"Pick another combination.");
        StartAnim(p);
        return;
    }
    g_hotkey = value;
    Wh_SetIntValue(L"prefHotkey", value);
    UpdateMouseHotkeyHook();
    PushNote(p, NotifSeverity::Success,
             value == 0 ? L"Shortcut removed" : L"Shortcut saved",
             value == 0 ? L"" : HotkeyText(value), kGlyphRename);
    StartAnim(p);
}

// Takes a key while the shortcut is being chosen. True if the key was used.
bool HandleHotkeyKey(Panel& p, UINT vk) {
    if (!p.hotkeyListening) {
        return false;
    }
    if (vk == VK_CONTROL || vk == VK_MENU || vk == VK_SHIFT || vk == VK_LWIN ||
        vk == VK_RWIN) {
        return true;  // only modifiers so far
    }
    if (vk == VK_ESCAPE) {
        CancelHotkeyCapture(p);
        return true;
    }
    UINT mods = 0;
    if (GetKeyState(VK_CONTROL) & 0x8000) mods |= MOD_CONTROL;
    if (GetKeyState(VK_MENU) & 0x8000) mods |= MOD_ALT;
    if (GetKeyState(VK_SHIFT) & 0x8000) mods |= MOD_SHIFT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) mods |= MOD_WIN;
    if (vk == VK_BACK && mods == 0) {
        ApplyHotkeyPref(p, 0);
        return true;
    }
    if (!(mods & (MOD_CONTROL | MOD_ALT | MOD_WIN))) {
        PushNote(p, NotifSeverity::Warning, L"Add Ctrl, Alt or Win",
                 L"A shortcut needs a modifier key.");
        return true;
    }
    ApplyHotkeyPref(p, (int)(vk | (mods << 16)));
    return true;
}

void Activate(Panel& p, int hit) {
    int kind = KindOf(hit);
    int idx = IndexOf(hit);
    switch (kind) {
        case kHitHiddenBadge:
            // Switch to PWAs tab when clicking hidden badge
            SelectTab(p, kTabApps);
            break;
        case kHitSetting: {
            if (idx >= 0 && idx < kSettingRowCount) {
                const SettingRow& row = kSettingRows[idx];
                if (row.hotkey) {
                    if (p.hotkeyListening) {
                        CancelHotkeyCapture(p);
                    } else {
                        BeginHotkeyCapture(p);
                    }
                } else if (row.toggle) {
                    ApplyPref(p, row.pref, PrefValue(row.pref)->load() ? 0 : 1);
                } else {
                    OpenPrefMenu(p, idx);
                }
            }
            break;
        }
        case kHitTile:
            if (p.selectedTab == kTabApps && idx < (int)p.frameHwnds.size()) {
                HWND h = p.frameHwnds[idx];
                if (!p.frameHidden[idx]) {
                    RequestHide(h);  // a translucent (visible) PWA goes to the tray
                } else {
                    PushNote(p, NotifSeverity::Info, L"Showing window",
                             PanelAppName(p, h), nullptr, WindowBitmap(p, h));
                    RequestRestore(h);
                    CloseIfHermes();
                }
            } else if (p.selectedTab == kTabLibrary && idx < (int)p.frameLib.size()) {
                LaunchOrRestore(p, p.frameLib[idx]);
                CloseIfHermes();
            }
            break;
        case kHitTileMedia:
            if (idx < (int)p.frameMediaKeys.size() && !p.frameMediaKeys[idx].empty()) {
                ShowMediaHighlight(p, p.frameMediaKeys[idx]);
            }
            break;
        case kHitRow:
            if (idx < (int)p.frameHwnds.size()) {
                PushNote(p, NotifSeverity::Info, L"Showing window",
                         PanelAppName(p, p.frameHwnds[idx]), nullptr, WindowBitmap(p, p.frameHwnds[idx]));
                RequestRestore(p.frameHwnds[idx]);
                CloseIfHermes();
            }
            break;
        case kHitMediaPrev:
        case kHitMediaNext:
        case kHitMediaPlay: {
            if (idx >= (int)p.media.size()) {
                break;
            }
            MediaInfo& m = p.media[idx];
            if (kind == kHitMediaPrev) {
                SendMediaCommand(MediaCmd::Prev, m.key, 0);
            } else if (kind == kHitMediaNext) {
                SendMediaCommand(MediaCmd::Next, m.key, 0);
            } else {
                // Show the new state right away, the next poll confirms it.
                Optim o;
                o.posMs = CurrentPosMs(m);
                o.posTick = GetTickCount64();
                o.playing = !m.playing;
                o.until = GetTickCount64() + 900;
                p.optim[m.key] = o;
                m.posMs = o.posMs;
                m.posTick = o.posTick;
                m.playing = o.playing;
                SendMediaCommand(MediaCmd::Toggle, m.key, 0);
            }
            break;
        }
        case kHitVolMute:
            p.volMute = !p.volMute;
            if (!p.volMute && p.volFrac <= 0.0) {
                p.volFrac = 0.5;
            }
            ApplyUserVolume(p, true);
            break;
        case kHitChip: {
            int n = (int)g_categories.size();
            if (idx == n + 1) {
                OpenDialog(p, false, -1, L"");
            } else {
                p.chipSel = idx;
                p.scrollT[kTabLibrary] = 0;
            }
            break;
        }
        case kHitMenuItem: {
            if (idx < (int)p.menuItems.size()) {
                MenuItem it = p.menuItems[idx];
                MenuCtx ctx = p.menuCtx;
                CloseMenu(p);
                ExecMenuCmd(p, it, ctx);
            }
            break;
        }
        case kHitDlgOk:
            DialogCommit(p);
            break;
        case kHitDlgCancel:
            CloseDialog(p);
            break;
    }
    StartAnim(p);
}

double SeekFracFromX(const Panel& p, int idx, float xDip) {
    if (idx < 0 || (size_t)idx >= p.trackRects.size()) {
        return 0;
    }
    const D2D1_RECT_F& r = p.trackRects[idx];
    float w = r.right - r.left;
    return w > 0 ? Clamp01((xDip - r.left) / w) : 0.0;
}

void CommitSeek(Panel& p) {
    int idx = p.seekingIdx;
    p.seekingIdx = -1;
    if (idx < 0 || idx >= (int)p.media.size()) {
        return;
    }
    MediaInfo& m = p.media[idx];
    if (m.durMs > 0) {
        long long ms = (long long)(p.seekFrac * m.durMs);
        Optim o;
        o.playing = m.playing;
        o.posMs = ms;
        o.posTick = GetTickCount64();
        o.until = GetTickCount64() + 900;
        p.optim[m.key] = o;
        m.posMs = ms;
        m.posTick = o.posTick;
        SendMediaCommand(MediaCmd::Seek, m.key, ms);
    }
}

void SaveVolumeState(const Panel& p) {
    Wh_SetIntValue(L"chromeVol", (int)std::lround(p.volFrac * 100.0));
    Wh_SetIntValue(L"chromeMute", p.volMute ? 1 : 0);
}

// Sends the panel's volume to Chrome. During a drag it is throttled, the last
// value is always sent on release (persist).
void ApplyUserVolume(Panel& p, bool persist) {
    ULONGLONG now = NowMs();
    if (!persist && now - p.volSetTick < 30) {
        return;
    }
    p.volSetTick = now;
    if (WriteChromeVolume((float)p.volFrac, p.volMute)) {
        p.volPending = false;
        p.volKnown = true;
        SetVolumeSnapshot((float)p.volFrac, p.volMute);
    } else {
        p.volPending = true;  // applied as soon as Chrome has an audio session
    }
    if (persist) {
        SaveVolumeState(p);
    }
}

// Reads the volume Chrome has now (the user can also change it in the Windows
// mixer). Applies a pending change once a session exists.
void RefreshVolume(Panel& p, bool force) {
    if (!g_showVolume || p.volDrag) {
        return;
    }
    ULONGLONG now = NowMs();
    if (!force && now - p.volTick < 600) {
        return;
    }
    p.volTick = now;
    if (p.volPending) {
        if (WriteChromeVolume((float)p.volFrac, p.volMute)) {
            p.volPending = false;
            p.volKnown = true;
            SetVolumeSnapshot((float)p.volFrac, p.volMute);
        }
        return;
    }
    float level = 1.0f;
    bool mute = false;
    if (GetVolumeSnapshot(level, mute)) {
        p.volFrac = level;
        p.volMute = mute;
        p.volKnown = true;
    } else {
        p.volKnown = false;
    }
}

double VolFracFromX(const Panel& p, float xDip) {
    float w = p.volTrack.right - p.volTrack.left;
    return w > 0 ? Clamp01((xDip - p.volTrack.left) / w) : 0.0;
}

// Moves an installed app to another place in the library and remembers it.
void MoveLibraryTile(Panel& p, int fromIdx, int toIdx) {
    const int n = (int)g_library.size();
    if (fromIdx < 0 || toIdx < 0 || fromIdx >= n || toIdx >= n ||
        fromIdx == toIdx) {
        return;
    }
    LibApp item = g_library[fromIdx];
    g_library.erase(g_library.begin() + fromIdx);
    g_library.insert(g_library.begin() + toIdx, item);
    SaveLibOrder();
    StartAnim(p);
}

// Moves a category to another chip place. Chip i is category i - 1.
void ReorderCategories(Panel& p, int from, int to) {
    const int n = (int)g_categories.size();
    if (from < 1 || to < 1 || from > n || to > n || from == to) {
        return;
    }
    Category item = std::move(g_categories[from - 1]);
    g_categories.erase(g_categories.begin() + (from - 1));
    g_categories.insert(g_categories.begin() + (to - 1), std::move(item));
    // The selected chip follows its category.
    if (p.chipSel == from) {
        p.chipSel = to;
    } else if (from < p.chipSel && p.chipSel <= to) {
        --p.chipSel;
    } else if (to <= p.chipSel && p.chipSel < from) {
        ++p.chipSel;
    }
    SaveCategories();
    StartAnim(p);
}

// Puts a dragged item at its new place in its tab and saves the order.
void ReorderDragged(Panel& p, int tab, int from, int to) {
    if (from < 0 || to < 0 || from == to) {
        return;
    }
    switch (tab) {
        case kTabLibrary:
            if (from < (int)p.frameLib.size() && to < (int)p.frameLib.size()) {
                // Display indexes -> indexes in the whole library.
                MoveLibraryTile(p, p.frameLib[from], p.frameLib[to]);
            }
            break;
        case kTabApps: {
            const int n = (int)p.appsIdx.size();
            if (from >= n || to >= n) {
                break;
            }
            std::vector<int> ord = p.appsIdx;
            const int item = ord[from];
            ord.erase(ord.begin() + from);
            ord.insert(ord.begin() + to, item);
            p.appsIdx = ord;
            std::vector<std::wstring> keys;
            for (int idx : ord) {
                keys.push_back(AppKey(p.all[idx]));
            }
            p.orderApps = MergeOrder(keys, p.orderApps);
            SaveOrderList(L"appsOrder", p.orderApps);
            break;
        }
        case kTabMedia: {
            const int n = (int)p.media.size();
            if (from >= n || to >= n) {
                break;
            }
            const MediaInfo item = p.media[from];
            p.media.erase(p.media.begin() + from);
            p.media.insert(p.media.begin() + to, item);
            std::vector<std::wstring> keys;
            for (const auto& m : p.media) {
                keys.push_back(MediaBaseKey(m));
            }
            p.orderMedia = MergeOrder(keys, p.orderMedia);
            SaveOrderList(L"mediaOrder", p.orderMedia);
            break;
        }
        case kTabNotifs: {
            const int n = (int)p.rows.size();
            if (from >= n || to >= n) {
                break;
            }
            // Moves the whole block of the app the row belongs to.
            const std::wstring ka = AppKey(p.all[p.rows[from].appIdx]);
            const std::wstring kb = AppKey(p.all[p.rows[to].appIdx]);
            if (ka == kb) {
                break;
            }
            std::vector<std::wstring> ord;
            for (const auto& r : p.rows) {
                std::wstring k = AppKey(p.all[r.appIdx]);
                if (std::find(ord.begin(), ord.end(), k) == ord.end()) {
                    ord.push_back(k);
                }
            }
            auto ia = std::find(ord.begin(), ord.end(), ka);
            if (ia != ord.end()) {
                ord.erase(ia);
            }
            auto ib = std::find(ord.begin(), ord.end(), kb);
            if (ib == ord.end()) {
                break;
            }
            ord.insert(from < to ? ib + 1 : ib, ka);
            p.orderNotifs = MergeOrder(ord, p.orderNotifs);
            SaveOrderList(L"notifOrder", p.orderNotifs);
            break;
        }
    }
    StartAnim(p);
}

void UpdateTabDrag(Panel& p, float fx) {

    if (!p.dragging) {
        if (std::fabs(fx - p.dragStartX) <= 6.0f) {
            return;
        }
        p.dragging = true;
    }
    float tw = TabW();
    float nx = p.dragOrigX + (fx - p.dragStartX);
    nx = (std::max)(kPad, (std::min)(nx, kPanelW - kPad - tw));
    TweenJump(p.tabPos[p.dragTab], nx);
    if (p.dragTab == p.selectedTab) {
        p.indL.Jump(NowMs(), nx + tw / 2 - 10.0f);
        p.indR.Jump(NowMs(), nx + tw / 2 + 10.0f);
    }
    int slot = (int)std::lround((nx - kPad) / tw);
    slot = (std::max)(0, (std::min)(slot, kTabCount - 1));
    MoveTabToSlot(p, p.dragTab, slot);
    StartAnim(p);
}

// Ends a press on a Settings toggle. A drag snaps the thumb to the nearer state,
// a flick decides by its direction, and the change is applied. Returns true if
// the press was a drag; a plain tap is left to the click.
bool EndToggleDrag(Panel& p) {
    const int row = p.togglePressRow;
    const bool wasDrag = p.toggleDragging;
    p.togglePressRow = -1;
    p.toggleDragging = false;
    if (!wasDrag || row < 0 || row >= kSettingRowCount) {
        return wasDrag;
    }
    const int id = kSettingRows[row].pref;
    const float v = ToggleTween(p, id).v;
    int target = v > 0.5f ? 1 : 0;
    if (p.toggleDir > 0 && v > 0.3f) {
        target = 1;
    }
    if (p.toggleDir < 0 && v < 0.7f) {
        target = 0;
    }
    ApplyPref(p, id, target);
    return true;
}

// Hermes: a mouse press outside the panel closes it. A low-level hook gets
// every press as an event, however short it is, unlike polling the buttons.
LRESULT CALLBACK MouseHookProc(int code, WPARAM wParam, LPARAM lParam) {
    // Runs on the hook thread: it never blocks, it only tells the host about the press.
    if (code == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
                              wParam == WM_MBUTTONDOWN)) {
        if (HWND host = g_hostHwnd.load()) {
            const MSLLHOOKSTRUCT* ms = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
            PostMessageW(host, kMsgOutsidePress, (WPARAM)(uint32_t)ms->pt.x,
                         (LPARAM)(uint32_t)ms->pt.y);
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void ShowPendingHiddenNotes(Panel& p);

// Remembers where the press grabbed the dragged element, measured from its centre.
// The ghost keeps that point under the pointer, so the element does not jump to
// the pointer when the drag starts.
void SetGrab(Panel& p, int id) {
    for (size_t i = p.hits.size(); i-- > 0;) {
        if (p.hits[i].id == id) {
            const D2D1_RECT_F& r = p.hits[i].r;
            p.grabX = p.libDownX - (r.left + r.right) / 2;
            p.grabY = p.libDownY - (r.top + r.bottom) / 2;
            return;
        }
    }
    p.grabX = 0;
    p.grabY = 0;
}

LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE && g_panel) {
        g_panel->hwnd = hwnd;
    }
    Panel* p = g_panel;
    if (!p || p->hwnd != hwnd) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    auto toDipX = [&](LPARAM l) { return (float)(short)LOWORD(l) * DipPerDevicePx(*p); };
    auto toDipY = [&](LPARAM l) { return (float)(short)HIWORD(l) * DipPerDevicePx(*p); };

    switch (msg) {
        case WM_ERASEBKGND:
            return 1;

        case WM_NCCALCSIZE:
            // No frame or caption is drawn: the client area is the whole window.
            if (wParam) {
                return 0;
            }
            break;

        case WM_NCHITTEST:
            // The whole window is client area: clicks go to the panel, not to the caption.
            return HTCLIENT;

        case WM_GETMINMAXINFO: {
            // Fixed size: the window can be neither resized nor maximized by the user.
            // The frame style stays, so Mica keeps working.
            MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
            mmi->ptMinTrackSize.x = mmi->ptMaxTrackSize.x = p->pxW;
            mmi->ptMinTrackSize.y = mmi->ptMaxTrackSize.y = p->pxH;
            mmi->ptMaxSize.x = p->pxW;
            mmi->ptMaxSize.y = p->pxH;
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            if (EnsureBackBuffer(*p)) {
                DrawPanel(*p);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case kMsgFrame:
            g_framePending = false;
            if (p->animating) {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_ACTIVATE:
            if (p->hotkeyListening && LOWORD(wParam) == WA_INACTIVE) {
                CancelHotkeyCapture(*p);
            }
            // Re-assert the backdrop on every activation change.
            ApplyBackdrop(*p);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_MOUSEMOVE: {
            float fx = toDipX(lParam);
            float fy = toDipY(lParam);
            if (fy >= kContentTop && fy < kContentBottom) {
                if (NowMs() - p->barTick > 1000) {
                    InvalidateRect(hwnd, nullptr, FALSE);  // wakes the scrollbar
                }
                p->barTick = NowMs();
            }
            if (!p->tracking) {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                p->tracking = true;
            }
            
            // Dragging over the search text: extend the selection
            if (p->searchSelDragging && (wParam & MK_LBUTTON)) {
                p->searchCaret = SearchCaretFromX(*p, fx);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }

            // Dragging over the dialog text: extend the selection
            if (p->dlgSelDragging && (wParam & MK_LBUTTON)) {
                p->dlgCaret = CaretFromX(*p, fx);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }

            // Swiping a notification
            if (p->noteDown >= 0 && (wParam & MK_LBUTTON) &&
                g_notifDismiss == (int)NotifDismiss::Swipe) {
                for (auto& n : p->notes) {
                    if (n.id == p->noteDown) {
                        float dx = fx - n.dragStartX;
                        bool allowed = n.swipeDir == 0 ||
                                       (dx != 0 && (dx > 0) == (n.swipeDir > 0));
                        n.dragDx = allowed ? dx : dx * 0.25f;
                    }
                }
                StartAnim(*p);
                return 0;
            }


            // Title bar dragging
            if (p->titleDragging && (wParam & MK_LBUTTON)) {
                POINT pt;
                GetCursorPos(&pt);
                int dx = pt.x - p->dragStartPos.x;
                int dy = pt.y - p->dragStartPos.y;
                RECT wr;
                GetWindowRect(hwnd, &wr);
                SetWindowPos(hwnd, nullptr, wr.left + dx, wr.top + dy, 0, 0,
                            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                p->dragStartPos = pt;
                return 0;
            }
            
            // Close button hover
            bool wasCloseHover = p->closeHover;
            D2D1_RECT_F closeRect = Rc(kPanelW - kCloseButtonW, 0, kCloseButtonW, kTitleBarH);
            p->closeHover = InRect(closeRect, fx, fy);
            
            if (p->closeHover != wasCloseHover) {
                TweenSet(p->closeT, p->closeHover ? 1.0f : 0.0f, 150, kEaseOut);
                StartAnim(*p);
            }
            
            if (p->seekingIdx >= 0) {
                p->seekFrac = SeekFracFromX(*p, p->seekingIdx, fx);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (p->volDrag) {
                p->volFrac = VolFracFromX(*p, fx);
                p->volMute = false;
                ApplyUserVolume(*p, false);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (p->togglePressRow >= 0 && (wParam & MK_LBUTTON)) {
                const float dx = fx - p->toggleDownX;
                if (!p->toggleDragging && std::fabs(dx) > 4.0f) {
                    p->toggleDragging = true;
                    p->pressed = kHitNone;  // the release must not click it
                }
                if (p->toggleDragging) {
                    const int id = kSettingRows[p->togglePressRow].pref;
                    if (fx != p->toggleLastX) {
                        p->toggleDir = fx > p->toggleLastX ? 1 : -1;
                    }
                    p->toggleLastX = fx;
                    const float v =
                        (std::clamp)(p->toggleStartV + dx / kToggleTravel, 0.0f, 1.0f);
                    TweenJump(ToggleTween(*p, id), v);
                    StartAnim(*p);
                    return 0;
                }
            }
            if (p->libDownIdx >= 0 && (wParam & MK_LBUTTON)) {
                const float dx = fx - p->libDownX;
                const float dy = fy - p->libDownY;
                if (!p->libDragging && dx * dx + dy * dy > 36.0f) {
                    p->libDragging = true;
                    p->dragPressId = MakeId(p->libDragKind, p->libDownIdx);
                    SetGrab(*p, p->dragPressId);
                    p->pressed = kHitNone;  // the release must not launch the app
                }
                if (p->libDragging) {
                    p->libDragX = fx;
                    p->libDragY = fy;
                    const int hit = HitTest(*p, fx, fy);
                    p->libTargetIdx =
                        KindOf(hit) == p->libDragKind ? IndexOf(hit) : -1;
                    StartAnim(*p);
                    return 0;
                }
            }
            if (p->dragTab >= 0) {
                UpdateTabDrag(*p, fx);
                return 0;
            }
            SetHover(*p, HitTest(*p, fx, fy));
            return 0;
        }

        case WM_MOUSELEAVE:
            p->tracking = false;
            p->closeHover = false;
            TweenSet(p->closeT, 0.0f, 150, kEaseOut);
            StartAnim(*p);
            p->closePressed = false;
            if (p->seekingIdx < 0 && p->dragTab < 0 && !p->volDrag) {
                SetHover(*p, kHitNone);
            }
            return 0;

        case WM_LBUTTONDOWN: {
            float fx = toDipX(lParam);
            float fy = toDipY(lParam);
            p->settle.active = false;
            
            // Check if clicking close button
            if (p->closeHover && fy < kTitleBarH) {
                p->closePressed = true;
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            
            // Check if clicking title bar for dragging
            if (!p->menuOpen && fy < kTitleBarH && fx < kPanelW - kCloseButtonW &&
                KindOf(HitTest(*p, fx, fy)) != kHitHiddenBadge) {
                GetCursorPos(&p->dragStartPos);
                p->titleDragging = true;
                SetCapture(hwnd);
                return 0;
            }
            
            // Library search field: a press on it takes the focus, any other press gives it up.
            if (!p->dlgOpen && !p->menuOpen && p->selectedTab == kTabLibrary &&
                InRect(SearchRect(), fx, fy)) {
                SetSearchFocus(*p, true);
                p->searchCaret = SearchCaretFromX(*p, fx);
                p->searchSelAnchor = p->searchCaret;
                p->searchSelDragging = true;
                SetCapture(hwnd);
                return 0;
            }
            if (p->searchFocused) {
                SetSearchFocus(*p, false);
            }
            if (p->dlgOpen) {
                p->dlgFocused = InRect(p->dlgField, fx, fy);
                p->dlgCaretOn = true;
                if (p->dlgFocused) {
                    p->dlgCaret = CaretFromX(*p, fx);
                    p->dlgSelAnchor = p->dlgCaret;
                    p->dlgSelDragging = true;
                    p->dlgTyping = false;
                    SetCapture(hwnd);
                }
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            int hit = HitTest(*p, fx, fy);
            if (p->menuOpen) {
                if (KindOf(hit) != kHitMenuItem) {
                    CloseMenu(*p);
                } else {
                    p->pressed = hit;
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (KindOf(hit) == kHitNote && !p->menuOpen) {
                for (auto& n : p->notes) {
                    if (n.id == IndexOf(hit)) {
                        n.dragging = true;
                        n.dragStartX = fx;
                        p->noteDown = n.id;
                        SetCapture(hwnd);
                    }
                }
                StartAnim(*p);
                return 0;
            }
            p->pressed = hit;
            if (!p->dlgOpen && !p->menuOpen && CanDragHit(*p, hit, fy)) {
                p->libDownIdx = IndexOf(hit);
                p->libDragKind = KindOf(hit);
                p->libDragTab = p->selectedTab;
                p->libDownX = fx;
                p->libDownY = fy;
                p->libDragging = false;
                SetCapture(hwnd);
            }
            if (!p->dlgOpen && KindOf(hit) == kHitSetting) {
                const int row = IndexOf(hit);
                if (row >= 0 && row < kSettingRowCount && kSettingRows[row].toggle) {
                    p->togglePressRow = row;
                    p->toggleDragging = false;
                    p->toggleDownX = fx;
                    p->toggleLastX = fx;
                    p->toggleDir = 0;
                    p->toggleStartV = ToggleTween(*p, kSettingRows[row].pref).v;
                    SetCapture(hwnd);
                }
            }
            if (!p->dlgOpen) {
                if (KindOf(hit) == kHitMediaSeek) {
                    p->seekingIdx = IndexOf(hit);
                    p->seekFrac = SeekFracFromX(*p, p->seekingIdx, fx);
                    SetCapture(hwnd);
                } else if (KindOf(hit) == kHitVolSeek) {
                    p->volDrag = true;
                    p->volFrac = VolFracFromX(*p, fx);
                    p->volMute = false;
                    ApplyUserVolume(*p, false);
                    SetCapture(hwnd);
                } else if (KindOf(hit) == kHitTab) {
                    p->dragTab = IndexOf(hit);
                    p->dragging = false;
                    p->dragStartX = fx;
                    p->dragOrigX = p->tabPos[p->dragTab].v;
                    SetCapture(hwnd);
                }
            }
            StartAnim(*p);
            return 0;
        }

        case WM_LBUTTONDBLCLK: {
            // The second press of a double click. Handled as a normal press
            // first, so tiles and buttons keep working, then the word is selected.
            LRESULT r = PanelProc(hwnd, WM_LBUTTONDOWN, wParam, lParam);
            if (p->dlgOpen && p->dlgFocused) {
                SelectDlgWordAt(*p, CaretFromX(*p, toDipX(lParam)));
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            if (p->searchFocused && InRect(SearchRect(), toDipX(lParam), toDipY(lParam))) {
                SelectSearchWordAt(*p, SearchCaretFromX(*p, toDipX(lParam)));
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return r;
        }

        case WM_LBUTTONUP: {
            float fx = toDipX(lParam);
            float fy = toDipY(lParam);

            if (p->libDownIdx >= 0) {
                const bool dragged = p->libDragging;
                const int tab = p->libDragTab;
                const int kind = p->libDragKind;
                const int from = p->libDownIdx;
                const int to = p->libTargetIdx;
                p->libDownIdx = -1;
                p->libDragging = false;
                p->libTargetIdx = -1;
                ReleaseCapture();
                if (dragged) {
                    p->pressed = kHitNone;
                    // The element glides from the pointer to its slot. Dropped outside,
                    // it glides back to where it came from.
                    BeginSettle(*p, kind, to >= 0 ? to : from);
                    if (to >= 0) {
                        if (kind == kHitChip) {
                            ReorderCategories(*p, from, to);
                        } else {
                            ReorderDragged(*p, tab, from, to);
                        }
                    }
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
                // Not dragged: a plain click, handled below.
            }
            if (p->togglePressRow >= 0) {
                const bool dragged = EndToggleDrag(*p);
                ReleaseCapture();
                if (dragged) {
                    p->pressed = kHitNone;
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
            }

            if (p->searchSelDragging) {
                p->searchSelDragging = false;
                ReleaseCapture();
                if (p->searchSelAnchor == p->searchCaret) {
                    p->searchSelAnchor = -1;
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (p->dlgSelDragging) {
                p->dlgSelDragging = false;
                ReleaseCapture();
                if (p->dlgSelAnchor == p->dlgCaret) {
                    p->dlgSelAnchor = -1;
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            
            if (p->noteDown >= 0) {
                const int down = p->noteDown;
                p->noteDown = -1;
                ReleaseCapture();
                for (auto& n : p->notes) {
                    if (n.id != down) {
                        continue;
                    }
                    n.dragging = false;
                    const int dir = n.dragDx > 0 ? 1 : -1;
                    const bool allowed = n.swipeDir == 0 || dir == n.swipeDir;
                    if (g_notifDismiss == (int)NotifDismiss::Click) {
                        n.leaving = true;
                        n.leaveDir = EdgeDir();
                    } else if (std::fabs(n.dragDx) > kNoteW * 0.3f && allowed) {
                        n.leaving = true;
                        n.leaveDir = dir;
                    }
                }
                p->pressed = kHitNone;
                StartAnim(*p);
                return 0;
            }

            // Handle close button release
            if (p->closePressed) {
                p->closePressed = false;
                if (p->closeHover) {
                    PostMessageW(hwnd, WM_CLOSE, 0, 0);
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            
            // Handle title bar dragging release
            if (p->titleDragging) {
                p->titleDragging = false;
                ReleaseCapture();
                return 0;
            }
            
            if (p->volDrag) {
                p->volFrac = VolFracFromX(*p, fx);
                p->volMute = false;
                p->volDrag = false;
                ApplyUserVolume(*p, true);
                p->pressed = kHitNone;
                ReleaseCapture();
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (p->seekingIdx >= 0) {
                p->seekFrac = SeekFracFromX(*p, p->seekingIdx, fx);
                CommitSeek(*p);
                p->pressed = kHitNone;
                ReleaseCapture();
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (p->dragTab >= 0) {
                int tab = p->dragTab;
                bool wasDrag = p->dragging;
                p->dragTab = -1;
                p->dragging = false;
                p->pressed = kHitNone;
                ReleaseCapture();
                if (wasDrag) {
                    TweenSet(p->tabPos[tab], SlotX(SlotOf(*p, tab)), 167, kEaseOut);
                    SyncIndicator(*p, true);
                    SaveTabOrder(*p);
                    StartAnim(*p);
                } else {
                    SelectTab(*p, tab);
                    StartAnim(*p);  // the same tab: SelectTab does nothing, the spring must still release
                }
                return 0;
            }
            int hit = HitTest(*p, fx, fy);
            int pressed = p->pressed;
            p->pressed = kHitNone;
            InvalidateRect(hwnd, nullptr, FALSE);
            if (hit != kHitNone && hit == pressed) {
                Activate(*p, hit);  // may post WM_CLOSE
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            float fx = toDipX(lParam);
            float fy = toDipY(lParam);
            if (p->menuOpen) {
                CloseMenu(*p);
                return 0;
            }
            if (p->dlgOpen) {
                return 0;
            }
            int hit = HitTest(*p, fx, fy);
            if (hit != kHitNone) {
                OnContextMenu(*p, hit, fx, fy);
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            if (p->menuOpen || p->dlgOpen) {
                return 0;
            }
            POINT pt = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
            ScreenToClient(hwnd, &pt);
            float fx = (float)pt.x * DipPerDevicePx(*p);
            float fy = (float)pt.y * DipPerDevicePx(*p);
            float notches = (float)(short)HIWORD(wParam) / 120.0f;
            p->barTick = NowMs();
            if (g_showDiag && fy >= kPanelH) {
                p->diagScroll = (std::max)(0.0f, (std::min)(p->diagScroll - notches * 48.0f, p->diagMax));
                StartAnim(*p);
                return 0;
            }
            if (g_showVolume && p->selectedTab == kTabMedia &&
                fy >= kContentBottom - kVolBarH - 4) {
                p->volFrac = Clamp01(p->volFrac + notches * 0.05);
                if (p->volFrac > 0) {
                    p->volMute = false;
                }
                ApplyUserVolume(*p, true);
                StartAnim(*p);
                return 0;
            }
            if (p->selectedTab == kTabLibrary && fy >= kContentTop &&
                fy < kContentTop + 34) {
                p->chipScrollT -= notches * 64.0f;
            } else {
                p->scrollT[p->selectedTab] -= notches * 64.0f;
                ClampScroll(*p, p->selectedTab);
            }
            (void)fx;
            StartAnim(*p);
            return 0;
        }

        case WM_MOUSEHWHEEL: {
            if (p->menuOpen || p->dlgOpen || p->selectedTab != kTabLibrary) {
                return 0;
            }
            POINT pt = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
            ScreenToClient(hwnd, &pt);
            float fy = (float)pt.y * DipPerDevicePx(*p);
            if (fy >= kContentTop && fy < kContentTop + 34) {
                // Positive wheel tilt means to the right, the row moves left.
                p->chipScrollT += (float)(short)HIWORD(wParam) / 120.0f * 64.0f;
                StartAnim(*p);
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            p->libDownIdx = -1;
            p->libDragging = false;
            p->libTargetIdx = -1;
            if (p->togglePressRow >= 0) {
                EndToggleDrag(*p);
            }
            p->dlgSelDragging = false;
            p->searchSelDragging = false;
            p->seekingIdx = -1;
            p->noteDown = -1;
            for (auto& n : p->notes) {
                n.dragging = false;
            }
            if (p->volDrag) {
                p->volDrag = false;
                ApplyUserVolume(*p, true);
            }
            if (p->dragTab >= 0) {
                p->dragTab = -1;
                p->dragging = false;
                SyncIndicator(*p, true);
                for (int t = 0; t < kTabCount; ++t) {
                    TweenSet(p->tabPos[t], SlotX(SlotOf(*p, t)), 167, kEaseOut);
                }
                StartAnim(*p);
            }
            return 0;

        case WM_SYSCOMMAND:
            switch (wParam & 0xFFF0) {
                case SC_MINIMIZE:
                case SC_MAXIMIZE:
                case SC_SIZE:
                case SC_MOVE:
                case SC_KEYMENU:
                    return 0;
                default:
                    break;
            }
            break;

        case WM_SYSKEYDOWN:
            // Alt combinations arrive here.
            if (HandleHotkeyKey(*p, (UINT)wParam)) {
                return 0;
            }
            break;

        case WM_KEYDOWN:
            if (HandleHotkeyKey(*p, (UINT)wParam)) {
                return 0;
            }
            if (p->dlgOpen && DialogKeyDown(*p, wParam)) {
                return 0;
            }
            if (SearchKeyDown(*p, wParam)) {
                return 0;
            }
            if (wParam == VK_ESCAPE) {
                if (p->menuOpen) {
                    CloseMenu(*p);
                } else if (p->dlgOpen) {
                    CloseDialog(*p);
                } else if (IsHermes()) {
                    PostMessageW(hwnd, WM_CLOSE, 0, 0);
                }
                return 0;
            }
            if ((wParam == VK_LEFT || wParam == VK_RIGHT) && !p->dlgOpen &&
                !p->menuOpen) {
                int slot = SlotOf(*p, p->selectedTab) + (wParam == VK_LEFT ? -1 : 1);
                if (slot >= 0 && slot < kTabCount) {
                    SelectTab(*p, p->order[slot]);
                }
                return 0;
            }
            break;

        case WM_CHAR:
            if (p->dlgOpen) {
                DialogChar(*p, (wchar_t)wParam);
                return 0;
            }
            if (p->searchFocused) {
                SearchChar(*p, (wchar_t)wParam);
                return 0;
            }
            break;

        case WM_TIMER:
            if (wParam == kTimerCaret) {
                if (p->dlgOpen) {
                    p->dlgCaretOn = !p->dlgCaretOn;
                }
                if (p->searchFocused) {
                    p->searchCaretOn = !p->searchCaretOn;
                }
                if (p->dlgOpen || p->searchFocused) {
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }
            if (wParam == kTimerData) {
                RequestMediaPoll(1500, false);
                RefreshVolume(*p, false);
                if (!RefreshData(*p) && IsHermes()) {
                    PostMessageW(hwnd, WM_CLOSE, 0, 0);
                    return 0;
                }
                ShowPendingHiddenNotes(*p);
                if (p->barA.v > 0.0f || p->barA.to > 0.5f) {
                    InvalidateRect(hwnd, nullptr, FALSE);  // lets the scrollbar fade out
                }
                // Redraw only when something on screen has changed.
                std::wstring sig = PanelStateSignature(*p);
                bool playing = false;
                for (const auto& m : p->media) {
                    playing |= m.playing;
                }
                if (sig != p->stateSig || playing) {
                    p->stateSig = sig;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }
            break;

        case WM_SETCURSOR:
            // Over the text field of the category dialog: the text (I-beam)
            // cursor. lParam's low word is the hit-test code here, not a point,
            // so the position is taken from the cursor.
            if (p->dlgOpen && LOWORD(lParam) == HTCLIENT) {
                POINT pt = {};
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                float fx = (float)pt.x * DipPerDevicePx(*p);
                float fy = (float)pt.y * DipPerDevicePx(*p);
                if (InRect(p->dlgField, fx, fy)) {
                    SetCursor(LoadCursorW(nullptr, IDC_IBEAM));
                    return TRUE;
                }
            }
            if (LOWORD(lParam) == HTCLIENT && !p->dlgOpen && p->selectedTab == kTabLibrary) {
                POINT pt = {};
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                if (InRect(SearchRect(), (float)pt.x * DipPerDevicePx(*p),
                           (float)pt.y * DipPerDevicePx(*p))) {
                    SetCursor(LoadCursorW(nullptr, IDC_IBEAM));
                    return TRUE;
                }
            }
            break;

        case WM_CLOSE:
            if (p->nativeMenuActive) {
                p->closeAfterMenu = true;
                EndMenu();
                return 0;
            }
            DestroyWindow(hwnd);
            return 0;

        case WM_NCDESTROY: {
            KillTimer(hwnd, kTimerData);
            g_frameHwnd = nullptr;
            if (g_frameWake) {
                ResetEvent(g_frameWake);
            }
            g_framePending = false;
            if (p->hotkeyListening) {
                if (HWND host = g_hostHwnd.load()) {
                    ApplyHotkey(host, g_hotkey.load());
                }
            }
            g_panelClosedTick = GetTickCount64();
            g_panel = nullptr;
            UpdateMouseHotkeyHook();
            UpdateModeHook();
            ReleasePanelBitmaps(*p);
            FreeBackBuffer(*p);
            delete p;
            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void ShowTrayMenu(HWND host);

// Notes about PWA windows hidden to the center. Checked on every panel tick, so a
// note appears while the center is open; a window the worker has not listed yet
// stays pending for up to 10 seconds.
void ShowPendingHiddenNotes(Panel& p) {
    std::vector<std::pair<HWND, ULONGLONG>> pending;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_pendingHidden.empty()) {
            return;
        }
        pending.swap(g_pendingHidden);
    }
    const ULONGLONG now = GetTickCount64();
    std::vector<std::pair<HWND, ULONGLONG>> keep;
    for (const auto& item : pending) {
        if (now - item.second > 10000) {
            continue;  // too old: the window was probably shown again
        }
        const AppInfo* found = nullptr;
        for (const AppInfo& a : p.all) {
            if (a.hwnd == item.first) {
                found = &a;
                break;
            }
        }
        if (found && found->hidden) {
            PushNote(p, NotifSeverity::Info, L"Hidden to PWA Tray Center", found->name,
                     nullptr, AppBitmap(p, *found));
        } else if (IsWindow(item.first)) {
            keep.push_back(item);
        }
    }
    if (!keep.empty()) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_pendingHidden.insert(g_pendingHidden.begin(), keep.begin(), keep.end());
    }
}

void ShowPanel(HWND host) {
    if (g_simpleMode) {
        return;
    }
    if (g_panel) {
        ClosePanelLocal();
        if (g_panel) {
            return;  // still closing (a native menu is open)
        }
    }

    if (!EnsureFactories()) {
        ShowTrayMenu(host);
        return;
    }

    HINSTANCE hInstance = GetModuleHandleW(nullptr);
    if (!g_panelClass) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.style = CS_DROPSHADOW | CS_DBLCLKS;
        wc.lpfnWndProc = PanelProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kPanelClass;
        g_panelClass = RegisterClassExW(&wc);
        if (!g_panelClass) {
            ShowTrayMenu(host);
            return;
        }
    }

    Panel* p = new Panel();
    GetCursorPos(&p->cursor);
    p->hasAnchor = GetTrayIconRect(host, p->anchor);
    POINT monitorPt = p->hasAnchor ? POINT{(p->anchor.left + p->anchor.right) / 2,
                                           (p->anchor.top + p->anchor.bottom) / 2}
                                   : p->cursor;
    HMONITOR monitor = MonitorFromPoint(monitorPt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(monitor, &mi);
    p->work = mi.rcWork;
    p->dpi = GetMonitorDpi(monitor);
    InitTheme(*p);

    p->hDip = kPanelH + (g_showDiag ? kDiagH : 0.0f);
    p->ui = 1.0f;  // real size: kPanelW x kPanelH DIPs, scaled only by DPI
    p->pxW = (int)std::lround(kPanelW * p->dpi * p->ui / 96.0f);
    p->pxH = (int)std::lround(p->hDip * p->dpi * p->ui / 96.0f);

    // Saved state.
    LoadCategories();
    p->orderApps = ParseOrderList(GetStored(L"appsOrder"));
    p->orderMedia = ParseOrderList(GetStored(L"mediaOrder"));
    p->orderNotifs = ParseOrderList(GetStored(L"notifOrder"));
    ParseTabOrder(GetStored(L"tabOrder"), p->order);
    int last = Wh_GetIntValue(L"lastTab", -1);
    p->selectedTab = (last >= 0 && last < kTabCount) ? last : p->order[0];
    for (int slot = 0; slot < kTabCount; ++slot) {
        TweenJump(p->tabPos[p->order[slot]], SlotX(slot));
    }
    SyncIndicator(*p, false);
    TweenJump(p->slideX, 0.0f);
    TweenJump(p->slideA, 1.0f);
    TweenJump(p->openDy, 0.0f);
    TweenJump(p->openAlpha, 1.0f);
    TweenJump(p->menuT, 1.0f);
    TweenJump(p->menuDy, 0.0f);

    g_library = ScanLibrary();
    g_libraryTick = GetTickCount64();

    if (!CreateGfx(*p)) {
        delete p;
        ShowTrayMenu(host);
        return;
    }

    RequestMediaPoll(2500);
    CollectWindowsSnapshot();
    if (!RefreshData(*p) && IsHermes()) {
        // Hermes is a quick flyout for hidden windows: nothing hidden, nothing to show.
        ReleasePanelBitmaps(*p);
        delete p;
        return;
    }
    p->volFrac = Wh_GetIntValue(L"chromeVol", 100) / 100.0;
    p->volMute = Wh_GetIntValue(L"chromeMute", 0) != 0;
    RefreshVolume(*p, true);

    int x = 0, y = 0;
    Place(*p, x, y);

    // Same trick as for a native tray menu: let the (hidden) host window take
    // the foreground first, so that the panel can be activated and then gets
    // deactivated when the user clicks elsewhere.
    SetForegroundWindow(host);

    g_panel = p;
    // Overlapped window with no GDI surface: DirectComposition shows the swap chain,
    // and DWM draws the system backdrop (Mica) behind its transparent pixels.
    // Overlapped style, as before 0.5.590: Mica is applied to it. The window menu,
    // minimize, maximize, size and move commands are swallowed in PanelProc.
    const DWORD kPanelStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
    // WS_EX_TOOLWINDOW keeps the panel out of Alt+Tab and the taskbar, as it was when it was a popup.
    HWND hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOREDIRECTIONBITMAP, kPanelClass,
                                L"", kPanelStyle, x, y, p->pxW, p->pxH, nullptr, nullptr,
                                hInstance, nullptr);
    if (!hwnd) {
        g_panel = nullptr;
        ReleasePanelBitmaps(*p);
        delete p;
        ShowTrayMenu(host);
        return;
    }
    p->hwnd = hwnd;

    AttachComposition(*p);

    ApplyBackdrop(*p);

    // Makes Windows send WM_NCCALCSIZE right away: until it does, the standard frame and
    // caption stay on the window (Microsoft's custom-frame note).
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    // Turns off DWM's own non-client drawing: no caption buttons or frame. Mica stays.
    DWMNCRENDERINGPOLICY ncPolicy = DWMNCRP_DISABLED;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &ncPolicy, sizeof(ncPolicy));

    // Open animation: the content slides in and fades (Fluent "enter").
    TweenJump(p->openDy, p->bottomAnchored ? 20.0f : -20.0f);
    TweenSet(p->openDy, 0.0f, 250, kEaseOut);
    TweenJump(p->openAlpha, 0.0f);
    TweenSet(p->openAlpha, 1.0f, 167, kEaseOut);

    SetTimer(hwnd, kTimerData, kTimerDataMs, nullptr);
    g_framePending = false;
    EnsureFrameThread();
    g_frameHwnd = hwnd;
    if (g_frameWake) {
        SetEvent(g_frameWake);
    }
    // Only presses that start after the panel opened count, so the tray click
    // that opened it does not close it. Titan does not need the hook.
    UpdateModeHook();
    ShowWindow(hwnd, SW_SHOW);
    StartAnim(*p);
    ShowPendingHiddenNotes(*p);

    ActivateWindow(hwnd, true);
    SetFocus(hwnd);
}

// ---------------------------------------------------------------------------
// Tray icon (one for all hidden PWAs) and the host window
// ---------------------------------------------------------------------------

HICON g_trayIcon = nullptr;
bool g_trayAdded = false;
bool g_trayBadge = false;
bool g_trayDark = false;
std::wstring g_trayTip;

// A tray icon drawn from the "all apps" glyph of Segoe Fluent Icons, in the
// color of the taskbar, with a dot in the accent color for unread items.
HICON MakeTrayIcon(bool dark, bool badge) {
    int s = GetSystemMetrics(SM_CXSMICON);
    if (s < 16) {
        s = 16;
    }
    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = s;
    bi.bmiHeader.biHeight = -s;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        DeleteDC(dc);
        ReleaseDC(nullptr, screen);
        return LoadIconW(nullptr, IDI_APPLICATION);
    }
    HGDIOBJ oldBmp = SelectObject(dc, dib);
    memset(bits, 0, (size_t)s * s * 4);

    PCWSTR family =
        GetWindowsBuild() >= 22000 ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";
    HFONT font = CreateFontW(-(s * 88 / 100), 0, 0, 0, FW_NORMAL, FALSE, FALSE,
                             FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                             DEFAULT_PITCH, family);
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    RECT rc = {0, 0, s, s};
    DrawTextW(dc, kGlyphAllApps, 1, &rc,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(dc, oldFont);
    DeleteObject(font);
    GdiFlush();

    // White on black -> color with alpha.
    uint32_t* px = (uint32_t*)bits;
    uint32_t fr = dark ? 255 : 0;
    uint32_t accent = GetAccentArgb();
    float badgeR = s * 0.24f;
    float bcx = s - badgeR - 0.5f;
    float bcy = s - badgeR - 0.5f;
    for (int y = 0; y < s; ++y) {
        for (int x = 0; x < s; ++x) {
            uint32_t v = px[y * s + x];
            uint32_t a = (std::max)({(v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF});
            float cr = (float)fr, cg = (float)fr, cb = (float)fr;
            float af = a / 255.0f;
            if (badge) {
                float dx = x + 0.5f - bcx;
                float dy = y + 0.5f - bcy;
                float d = std::sqrt(dx * dx + dy * dy) - badgeR;
                float cov = 0.5f - d;
                cov = cov < 0 ? 0 : (cov > 1 ? 1 : cov);
                if (cov > 0) {
                    float na = cov + af * (1 - cov);
                    float ar = (float)((accent >> 16) & 0xFF);
                    float ag = (float)((accent >> 8) & 0xFF);
                    float ab = (float)(accent & 0xFF);
                    cr = (ar * cov + cr * af * (1 - cov)) / na;
                    cg = (ag * cov + cg * af * (1 - cov)) / na;
                    cb = (ab * cov + cb * af * (1 - cov)) / na;
                    af = na;
                }
            }
            uint32_t ia = (uint32_t)(af * 255.0f + 0.5f);
            px[y * s + x] = (ia << 24) | ((uint32_t)(cr + 0.5f) << 16) |
                            ((uint32_t)(cg + 0.5f) << 8) | (uint32_t)(cb + 0.5f);
        }
    }

    SelectObject(dc, oldBmp);
    HBITMAP mask = CreateBitmap(s, s, 1, 1, nullptr);
    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmMask = mask;
    ii.hbmColor = dib;
    HICON icon = CreateIconIndirect(&ii);
    DeleteObject(mask);
    DeleteObject(dib);
    DeleteDC(dc);
    ReleaseDC(nullptr, screen);
    return icon ? icon : LoadIconW(nullptr, IDI_APPLICATION);
}

void RemoveTrayIcon(HWND host) {
    if (g_trayAdded) {
        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(nid);
        nid.hWnd = host;
        nid.uID = kTrayIconId;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        g_trayAdded = false;
    }
    if (g_trayIcon) {
        DestroyIcon(g_trayIcon);
        g_trayIcon = nullptr;
    }
    g_trayTip.clear();
}

// ---------------------------------------------------------------------------
// Simple mode: no panel. A hidden PWA gets its own tray icon ("separate"), or the one
// tray icon lists all PWAs in its menu ("single"). Runs on the center thread, which
// owns the tray icons.
// ---------------------------------------------------------------------------

constexpr UINT kSimpleIconBase = 100;   // uID of the first icon of a PWA
constexpr UINT kSimpleMenuBase = 2000;  // command id of the first item of the PWA menu

struct SimpleIcon {
    UINT uid = 0;
    HWND hwnd = nullptr;
    HICON icon = nullptr;  // our own copy, destroyed when the icon goes
    bool badge = false;
    std::wstring tip;
};
std::vector<SimpleIcon> g_simpleIcons;  // center thread only

// A copy of the icon, with an accent dot in the corner when there is something unread.
HICON MakeBadgedIcon(HICON src, bool badge) {
    if (!src) {
        return nullptr;
    }
    if (!badge) {
        return CopyIcon(src);
    }
    int s = GetSystemMetrics(SM_CXSMICON);
    if (s < 16) {
        s = 16;
    }
    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = s;
    bi.bmiHeader.biHeight = -s;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        if (dib) {
            DeleteObject(dib);
        }
        DeleteDC(dc);
        ReleaseDC(nullptr, screen);
        return CopyIcon(src);
    }
    HGDIOBJ oldBmp = SelectObject(dc, dib);
    memset(bits, 0, (size_t)s * s * 4);
    DrawIconEx(dc, 0, 0, src, s, s, 0, nullptr, DI_NORMAL);
    GdiFlush();
    uint32_t* px = (uint32_t*)bits;
    for (int i = 0; i < s * s; ++i) {
        // Drawn pixels become opaque, the rest stays fully transparent.
        px[i] = (px[i] & 0x00FFFFFFu) ? (px[i] | 0xFF000000u) : 0u;
    }
    // Accent dot in the bottom right corner.
    const uint32_t accent = GetAccentArgb();
    const float r = s * 0.24f;
    const float cx = s - r - 0.5f;
    const float cy = s - r - 0.5f;
    for (int y = 0; y < s; ++y) {
        for (int x = 0; x < s; ++x) {
            const float dx = x + 0.5f - cx;
            const float dy = y + 0.5f - cy;
            if (dx * dx + dy * dy <= r * r) {
                px[y * s + x] = accent;
            }
        }
    }
    SelectObject(dc, oldBmp);
    HBITMAP mask = CreateBitmap(s, s, 1, 1, nullptr);
    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmMask = mask;
    ii.hbmColor = dib;
    HICON icon = CreateIconIndirect(&ii);
    DeleteObject(mask);
    DeleteObject(dib);
    DeleteDC(dc);
    ReleaseDC(nullptr, screen);
    return icon ? icon : CopyIcon(src);
}

// Removes the icons of the PWAs (the single icon goes with RemoveTrayIcon).
void RemoveSimpleIcons(HWND host) {
    for (const auto& ic : g_simpleIcons) {
        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(nid);
        nid.hWnd = host;
        nid.uID = ic.uid;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        if (ic.icon) {
            DestroyIcon(ic.icon);
        }
    }
    g_simpleIcons.clear();
}

UINT FreeSimpleUid() {
    for (UINT uid = kSimpleIconBase;; ++uid) {
        bool used = false;
        for (const auto& ic : g_simpleIcons) {
            if (ic.uid == uid) {
                used = true;
                break;
            }
        }
        if (!used) {
            return uid;
        }
    }
}

// Menu text: '&' is doubled, so that it is not taken as an accelerator.
std::wstring MenuText(const std::wstring& s) {
    std::wstring out;
    for (wchar_t c : s) {
        out += c;
        if (c == L'&') {
            out += L'&';
        }
    }
    return out;
}

// The icon as a 32-bit bitmap for a menu item: transparent where the icon is.
HBITMAP IconToMenuBitmap(HICON icon, int s) {
    if (!icon) {
        return nullptr;
    }
    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = s;
    bi.bmiHeader.biHeight = -s;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (dib && bits) {
        HGDIOBJ old = SelectObject(dc, dib);
        memset(bits, 0, (size_t)s * s * 4);
        DrawIconEx(dc, 0, 0, icon, s, s, 0, nullptr, DI_NORMAL);
        GdiFlush();
        uint32_t* px = (uint32_t*)bits;
        for (int i = 0; i < s * s; ++i) {
            px[i] = (px[i] & 0x00FFFFFFu) ? (px[i] | 0xFF000000u) : 0u;
        }
        SelectObject(dc, old);
    }
    DeleteDC(dc);
    ReleaseDC(nullptr, screen);
    return dib;
}

// Right click on the single icon: every PWA window, open ones and hidden ones,
// each with its own icon on the left.
void ShowSimpleMenu(HWND host) {
    std::vector<AppInfo> apps = CollectWindowsRaw();
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }
    if (apps.empty()) {
        AppendMenuW(menu, MF_STRING | MF_GRAYED, 0, L"No PWA windows");
    }
    int s = GetSystemMetrics(SM_CXSMICON);
    if (s < 16) {
        s = 16;
    }
    std::vector<HBITMAP> bitmaps;
    for (size_t i = 0; i < apps.size(); ++i) {
        std::wstring text = AppDisplayName(apps[i]);
        text += apps[i].hidden ? L"  (hidden)" : L"  (open)";
        text = MenuText(text);
        HBITMAP bmp = IconToMenuBitmap(GetWindowIconSafe(apps[i].hwnd, false), s);
        bitmaps.push_back(bmp);
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_ID | MIIM_STRING | MIIM_BITMAP | MIIM_STATE;
        mii.fState = MFS_ENABLED;
        mii.wID = (UINT)(kSimpleMenuBase + i);
        mii.dwTypeData = text.data();
        mii.cch = (UINT)text.size();
        mii.hbmpItem = bmp;
        InsertMenuItemW(menu, (UINT)i, TRUE, &mii);
    }
    POINT pt = {};
    GetCursorPos(&pt);
    SetForegroundWindow(host);
    const UINT cmd = TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y,
                                    0, host, nullptr);
    PostMessageW(host, WM_NULL, 0, 0);
    DestroyMenu(menu);
    for (HBITMAP bmp : bitmaps) {
        if (bmp) {
            DeleteObject(bmp);
        }
    }
    if (cmd >= kSimpleMenuBase && cmd - kSimpleMenuBase < apps.size()) {
        const AppInfo& a = apps[cmd - kSimpleMenuBase];
        if (a.hidden) {
            RequestRestore(a.hwnd);
        } else {
            RequestHide(a.hwnd);
        }
    }
}

// Right click on the icon of one PWA: show it or quit it.
void ShowSimpleIconMenu(HWND host, HWND target) {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }
    AppendMenuW(menu, MF_STRING, 1, L"Show");
    AppendMenuW(menu, MF_STRING, 2, L"Quit");
    POINT pt = {};
    GetCursorPos(&pt);
    SetForegroundWindow(host);
    const UINT cmd = TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y,
                                    0, host, nullptr);
    PostMessageW(host, WM_NULL, 0, 0);
    DestroyMenu(menu);
    if (cmd == 1) {
        RequestRestore(target);
    } else if (cmd == 2) {
        RequestQuit(target);
    }
}

// Tray messages in simple mode: the uID tells which icon was used.
void SimpleTrayMessage(HWND host, UINT uid, UINT msg) {
    if (uid == kTrayIconId) {
        if (msg == WM_RBUTTONUP) {
            ShowSimpleMenu(host);  // the single icon: left click does nothing
        }
        return;
    }
    for (const auto& ic : g_simpleIcons) {
        if (ic.uid != uid) {
            continue;
        }
        if (msg == WM_LBUTTONUP) {
            RequestRestore(ic.hwnd);
        } else if (msg == WM_RBUTTONUP) {
            ShowSimpleIconMenu(host, ic.hwnd);
        }
        return;
    }
}

// Brings the tray icons in line with the PWA windows (simple mode).
void SyncSimpleTray(HWND host) {
    std::vector<AppInfo> apps = CollectWindowsRaw();
    if (g_simpleSingle) {
        RemoveSimpleIcons(host);
        int unread = 0;
        for (const auto& a : apps) {
            unread += a.unread;
        }
        const bool dark = IsSystemDark();
        const bool badge = unread > 0;
        std::wstring tip = L"PWA Tray Center";
        if (!apps.empty()) {
            tip += L" \x00B7 " + std::to_wstring(apps.size()) + (apps.size() == 1 ? L" app" : L" apps");
        }
        if (!g_trayAdded || badge != g_trayBadge || dark != g_trayDark || tip != g_trayTip) {
            HICON icon = MakeTrayIcon(dark, badge);
            NOTIFYICONDATAW nid = {};
            nid.cbSize = sizeof(nid);
            nid.hWnd = host;
            nid.uID = kTrayIconId;
            nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
            nid.uCallbackMessage = g_trayMsg;
            nid.hIcon = icon;
            wcsncpy(nid.szTip, tip.c_str(), ARRAYSIZE(nid.szTip) - 1);
            if (Shell_NotifyIconW(g_trayAdded ? NIM_MODIFY : NIM_ADD, &nid)) {
                if (g_trayIcon) {
                    DestroyIcon(g_trayIcon);
                }
                g_trayIcon = icon;
                g_trayAdded = true;
                g_trayBadge = badge;
                g_trayDark = dark;
                g_trayTip = tip;
            } else {
                DestroyIcon(icon);
            }
        }
    } else {
        if (g_trayAdded) {
            RemoveTrayIcon(host);
        }
        // Icons of PWAs that are no longer hidden go.
        for (size_t i = g_simpleIcons.size(); i-- > 0;) {
            const UINT uid = g_simpleIcons[i].uid;
            const HWND hwnd = g_simpleIcons[i].hwnd;
            bool stillHidden = false;
            for (const auto& a : apps) {
                if (a.hwnd == hwnd && a.hidden) {
                    stillHidden = true;
                }
            }
            if (!stillHidden) {
                NOTIFYICONDATAW nid = {};
                nid.cbSize = sizeof(nid);
                nid.hWnd = host;
                nid.uID = uid;
                Shell_NotifyIconW(NIM_DELETE, &nid);
                if (g_simpleIcons[i].icon) {
                    DestroyIcon(g_simpleIcons[i].icon);
                }
                g_simpleIcons.erase(g_simpleIcons.begin() + i);
            }
        }
        // A new icon for each hidden PWA; an icon is updated when its unread or name changes.
        for (const auto& a : apps) {
            if (!a.hidden) {
                continue;
            }
            const bool badge = a.unread > 0;
            std::wstring tip = AppDisplayName(a);
            if (badge) {
                tip += L" \x00B7 " + std::to_wstring(a.unread) + L" unread";
            }
            SimpleIcon* found = nullptr;
            for (auto& ic : g_simpleIcons) {
                if (ic.hwnd == a.hwnd) {
                    found = &ic;
                    break;
                }
            }
            if (found && found->badge == badge && found->tip == tip) {
                continue;
            }
            HICON icon = MakeBadgedIcon(GetWindowIconSafe(a.hwnd, false), badge);
            NOTIFYICONDATAW nid = {};
            nid.cbSize = sizeof(nid);
            nid.hWnd = host;
            nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
            nid.uCallbackMessage = g_trayMsg;
            nid.hIcon = icon;
            wcsncpy(nid.szTip, tip.c_str(), ARRAYSIZE(nid.szTip) - 1);
            if (found) {
                nid.uID = found->uid;
                if (Shell_NotifyIconW(NIM_MODIFY, &nid) || Shell_NotifyIconW(NIM_ADD, &nid)) {
                    if (found->icon) {
                        DestroyIcon(found->icon);
                    }
                    found->icon = icon;
                    found->badge = badge;
                    found->tip = tip;
                } else {
                    // Neither worked: drop the entry, the next sync adds the icon again.
                    DestroyIcon(icon);
                    const UINT deadUid = found->uid;
                    for (size_t k = 0; k < g_simpleIcons.size(); ++k) {
                        if (g_simpleIcons[k].uid == deadUid) {
                            if (g_simpleIcons[k].icon) {
                                DestroyIcon(g_simpleIcons[k].icon);
                            }
                            g_simpleIcons.erase(g_simpleIcons.begin() + k);
                            break;
                        }
                    }
                }
            } else {
                SimpleIcon ic;
                ic.uid = FreeSimpleUid();
                ic.hwnd = a.hwnd;
                nid.uID = ic.uid;
                if (Shell_NotifyIconW(NIM_ADD, &nid)) {
                    ic.icon = icon;
                    ic.badge = badge;
                    ic.tip = tip;
                    g_simpleIcons.push_back(ic);
                } else {
                    DestroyIcon(icon);
                }
            }
        }
    }
    SetTimer(host, kHostTimer, kHostTimerMs, nullptr);
}

// Adds, updates or removes the tray icon according to the hidden windows.
void SyncTray(HWND host) {
    if (g_simpleMode) {
        SyncSimpleTray(host);
        return;
    }
    std::vector<HWND> hidden = HiddenSnapshot();
    if (hidden.empty() && !g_trayAlways) {
        RemoveTrayIcon(host);
        KillTimer(host, kHostTimer);
        return;
    }

    int unread = 0;
    for (HWND h : hidden) {
        unread += ParseUnreadFromTitle(GetTitleSafe(h));
    }
    bool badge = unread > 0;
    bool dark = IsSystemDark();
    std::wstring tip = L"PWA Tray Center";
    if (!hidden.empty()) {
        tip += L" \x00B7 " + std::to_wstring(hidden.size()) +
               (hidden.size() == 1 ? L" app" : L" apps");
    }
    if (unread > 0) {
        tip += L" \x00B7 " + std::to_wstring(unread) + L" unread";
    }

    bool needIcon = !g_trayIcon || badge != g_trayBadge || dark != g_trayDark;
    if (!g_trayAdded || needIcon || tip != g_trayTip) {
        HICON icon = g_trayIcon;
        if (needIcon) {
            icon = MakeTrayIcon(dark, badge);
        }
        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(nid);
        nid.hWnd = host;
        nid.uID = kTrayIconId;
        nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        nid.uCallbackMessage = g_trayMsg;
        nid.hIcon = icon;
        wcsncpy(nid.szTip, tip.c_str(), ARRAYSIZE(nid.szTip) - 1);
        BOOL ok = Shell_NotifyIconW(g_trayAdded ? NIM_MODIFY : NIM_ADD, &nid);
        if (ok) {
            g_trayAdded = true;
            if (icon != g_trayIcon) {
                if (g_trayIcon) {
                    DestroyIcon(g_trayIcon);
                }
                g_trayIcon = icon;
            }
            g_trayBadge = badge;
            g_trayDark = dark;
            g_trayTip = tip;
        } else if (icon != g_trayIcon) {
            DestroyIcon(icon);
        }
    }
    SetTimer(host, kHostTimer, kHostTimerMs, nullptr);
}

// Right click on the tray icon: a small classic menu.
void ShowTrayMenu(HWND host) {
    if (IsHermes()) {
        ClosePanelLocal();
    }
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }
    AppendMenuW(menu, MF_STRING, kTrayMenuShowAll, L"Show all");
    AppendMenuW(menu, MF_STRING, kTrayMenuQuitAll, L"Quit all");

    POINT pt = {};
    GetCursorPos(&pt);

    // Required so that the menu closes when the user clicks elsewhere.
    SetForegroundWindow(host);
    UINT cmd = TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y,
                              0, host, nullptr);
    PostMessageW(host, WM_NULL, 0, 0);
    DestroyMenu(menu);

    if (cmd == kTrayMenuShowAll || cmd == kTrayMenuQuitAll) {
        for (HWND h : HiddenSnapshot()) {
            if (cmd == kTrayMenuShowAll) {
                RequestRestore(h);
            } else {
                RequestQuit(h);
            }
        }
    }
}

LRESULT CALLBACK HostProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_trayMsg && msg == g_trayMsg) {
        if (g_simpleMode) {
            SimpleTrayMessage(hwnd, (UINT)wParam, (UINT)LOWORD(lParam));
            return 0;
        }
        switch (LOWORD(lParam)) {
            case WM_LBUTTONUP:
                if (g_panel) {
                    if (IsHermes()) {
                        ClosePanelLocal();
                    }
                } else if (GetTickCount64() - g_panelClosedTick >= 300) {
                    // (A click that just closed the panel by deactivation must
                    // not reopen it.)
                    ShowPanel(hwnd);
                }
                break;
            case WM_RBUTTONUP:
                ShowTrayMenu(hwnd);
                break;
            case WM_MOUSEMOVE: {
                // Hovering the icon is a good hint that the panel is next.
                static ULONGLONG lastPrewarm = 0;
                ULONGLONG now = GetTickCount64();
                if (now - lastPrewarm > 1500 && !g_panel) {
                    lastPrewarm = now;
                    RequestMediaPoll(4000);
                }
                break;
            }
        }
        return 0;
    }

    if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {
        // Explorer was restarted: the new taskbar knows none of our icons.
        g_trayAdded = false;
        RemoveSimpleIcons(hwnd);  // the NIM_DELETE calls fail harmlessly; the icons are freed
        SyncTray(hwnd);           // every hidden PWA gets its icon again
        return 0;
    }

    switch (msg) {
        case kMsgListChanged:
            SyncTray(hwnd);
            return 0;

        case kMsgOutsidePress:
            if (g_panel && g_panel->hwnd && IsHermes() && !g_panel->nativeMenuActive) {
                POINT pt = {(LONG)(int32_t)(uint32_t)wParam, (LONG)(int32_t)(uint32_t)lParam};
                RECT wr = {};
                if (GetWindowRect(g_panel->hwnd, &wr) && !PtInRect(&wr, pt)) {
                    PostMessageW(g_panel->hwnd, WM_CLOSE, 0, 0);
                }
            }
            return 0;

        case kMsgWindowHidden:
            if (g_panel) {
                ShowPendingHiddenNotes(*g_panel);
            }
            return 0;

        case kMsgPlainClick:
            if (g_panel && g_panel->hotkeyListening) {
                POINT pt = {(LONG)(int32_t)(uint32_t)wParam, (LONG)(int32_t)(uint32_t)lParam};
                RECT wr = {};
                if (GetWindowRect(g_panel->hwnd, &wr) && !PtInRect(&wr, pt)) {
                    CancelHotkeyCapture(*g_panel);
                } else {
                    // Inside the panel: the click goes on, and the note says what is needed.
                    PushNote(*g_panel, NotifSeverity::Warning, L"Add Ctrl, Alt or Win",
                             L"Mouse buttons need a modifier key.");
                    StartAnim(*g_panel);
                }
            }
            return 0;

        case kMsgMouseHotkey:
            // Only presses with a modifier come here (see HotkeyMouseHookProc).
            if (g_panel && g_panel->hotkeyListening) {
                ApplyHotkeyPref(*g_panel, (int)((UINT)wParam | ((UINT)lParam << 16)));
            }
            return 0;

        case WM_TIMER:
            if (wParam == kHostTimer) {
                SyncTray(hwnd);
            }
            return 0;

        case WM_HOTKEY:
            if (wParam == kHotkeyId) {
                if (g_panel) {
                    ClosePanelLocal();
                } else {
                    ShowPanel(hwnd);
                }
            }
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            ClosePanelLocal();
            KillTimer(hwnd, kHostTimer);
            RemoveTrayIcon(hwnd);
            RemoveSimpleIcons(hwnd);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// The center lives on its own thread, so that it keeps working (and animating)
// even when the browser's UI thread is busy.
HANDLE g_centerThread = nullptr;
HANDLE g_centerReady = nullptr;
std::mutex g_centerMutex;

DWORD WINAPI CenterThreadProc(LPVOID) {
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = HostProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = kHostClass;
    // A class left over from an earlier instance would keep its old window procedure.
    const bool registered = RegisterClassExW(&wc) != 0;

    HWND host = registered
                    ? CreateWindowExW(WS_EX_TOOLWINDOW, kHostClass, L"PWA Tray Center",
                                      WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                      hInstance, nullptr)
                    : nullptr;
    g_hostHwnd = host;
    if (host) {
        ApplyHotkey(host, g_hotkey.load());
        UpdateMouseHotkeyHook();
    }
    if (host) {
        PostMessageW(host, kMsgListChanged, 0, 0);  // shows the tray icon if it is always on
    }
    SetEvent(g_centerReady);

    if (host) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    g_hostHwnd = nullptr;
    if (g_panelClass) {
        UnregisterClassW(kPanelClass, hInstance);
        g_panelClass = 0;
    }
    UnregisterClassW(kHostClass, hInstance);
    // The factories were made on this thread, so they are released before its COM apartment ends.
    ReleaseFactories();
    if (SUCCEEDED(hrCom)) {
        CoUninitialize();
    }
    return 0;
}

bool EnsureCenter() {
    std::lock_guard<std::mutex> lock(g_centerMutex);
    if (g_stopping) {
        return false;
    }
    HWND host = g_hostHwnd.load();
    if (host && IsWindow(host)) {
        return true;
    }
    if (!g_centerThread) {
        g_centerReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_centerReady) {
            return false;
        }
        g_centerThread =
            CreateThread(nullptr, 0, CenterThreadProc, nullptr, 0, nullptr);
        if (!g_centerThread) {
            CloseHandle(g_centerReady);
            g_centerReady = nullptr;
            return false;
        }
    }
    WaitForSingleObject(g_centerReady, 3000);
    return g_hostHwnd.load() != nullptr;
}

void StopCenter() {
    HANDLE thread;
    HANDLE ready;
    HWND host;
    {
        std::lock_guard<std::mutex> lock(g_centerMutex);
        thread = g_centerThread;
        ready = g_centerReady;
        host = g_hostHwnd.load();
        g_centerThread = nullptr;
        g_centerReady = nullptr;
    }
    if (!thread) {
        return;
    }
    if (host) {
        PostMessageW(host, WM_CLOSE, 0, 0);
    }
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    if (ready) {
        CloseHandle(ready);
    }
}

// ---------------------------------------------------------------------------
// Window menu item and window management (runs on the browser's UI thread)
// ---------------------------------------------------------------------------


void NotifyListChanged() {
    HWND host = g_hostHwnd.load();
    if (host) {
        PostMessageW(host, kMsgListChanged, 0, 0);
    }
}

bool HideToCenter(HWND hWnd) {
    if (!EnsureCenter()) {
        return false;
    }
    ShowWindow(hWnd, SW_HIDE);
    const bool hidden = true;
    const ULONGLONG tick = GetTickCount64();
    SetWindowFlags(hWnd, &hidden, &tick);
    if (!g_simpleMode) {
        RequestMediaPoll(4000);
    }
    NotifyListChanged();
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_pendingHidden.emplace_back(hWnd, GetTickCount64());
        if (g_pendingHidden.size() > 16) {
            g_pendingHidden.erase(g_pendingHidden.begin());
        }
    }
    if (HWND host = g_hostHwnd.load()) {
        PostMessageW(host, kMsgWindowHidden, 0, 0);
    }
    return true;
}

// From the window menu: if the panel could not start, say so and keep the window visible.
void HideFromMenu(HWND hWnd) {
    // Runs inside the browser's window procedure: no modal dialog here, it would
    // keep the browser's thread busy while the mod unloads.
    if (g_stopping) {
        return;
    }
    if (!HideToCenter(hWnd)) {
        MessageBeep(MB_ICONWARNING);
        Wh_Log(L"PWA Tray Center could not start, so the window stays visible.");
    }
}


void RestoreWindow(HWND hWnd) {
    const bool hidden = false;
    SetWindowFlags(hWnd, &hidden);
    ShowWindow(hWnd, IsIconic(hWnd) ? SW_RESTORE : SW_SHOW);
    ActivateWindow(hWnd, false);
    NotifyListChanged();
}

void QuitWindow(HWND hWnd) {
    const bool hidden = false;
    SetWindowFlags(hWnd, &hidden);
    PostMessageW(hWnd, WM_CLOSE, 0, 0);
    NotifyListChanged();
}

// Adds the "Hide to PWA Tray Center" item right above "Close" (SC_CLOSE). Does
// nothing if the item is already there. Called every time the window menu is
// opened, since the browser can rebuild the menu items on its own.
void EnsureWindowMenuItem(HWND hWnd, HMENU menu) {
    if (!menu || !IsMenu(menu)) {
        return;
    }

    if (GetMenuState(menu, kMenuIdHideToCenter, MF_BYCOMMAND) != (UINT)-1) {
        return;
    }

    PCWSTR text = L"Hide to PWA Tray Center";

    BOOL ok = InsertMenuW(menu, SC_CLOSE, MF_BYCOMMAND | MF_STRING,
                          kMenuIdHideToCenter, text);
    if (!ok) {
        // No SC_CLOSE item (unexpected menu layout): put it at the end.
        AppendMenuW(menu, MF_STRING, kMenuIdHideToCenter, text);
    }
}

void RemoveWindowMenuItem(HWND hWnd) {
    HMENU menu = GetSystemMenu(hWnd, FALSE);
    if (menu) {
        DeleteMenu(menu, kMenuIdHideToCenter, MF_BYCOMMAND);
    }
}

// Signature of WindhawkUtils::SetWindowSubclassFromAnyThread callbacks: there
// is no uIdSubclass parameter. The wrapper removes itself on WM_NCDESTROY.
LRESULT CALLBACK WindowSubclassProc(HWND hWnd,
                                    UINT uMsg,
                                    WPARAM wParam,
                                    LPARAM lParam,
                                    DWORD_PTR /*dwRefData*/) {
    if (g_restoreMsg && uMsg == g_restoreMsg) {
        RestoreWindow(hWnd);
        return 0;
    }
    if (g_quitMsg && uMsg == g_quitMsg) {
        QuitWindow(hWnd);
        return 0;
    }
    if (g_hideMsg && uMsg == g_hideMsg) {
        HideFromMenu(hWnd);
        return 0;
    }

    switch (uMsg) {
        case WM_INITMENU:
        case WM_INITMENUPOPUP: {
            // Let the browser prepare its menu first, and add the item after
            // that, so that a menu rebuilt by the browser can't remove it.
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            HMENU menu = (HMENU)wParam;
            bool isWindowMenu =
                uMsg == WM_INITMENUPOPUP
                    ? HIWORD(lParam) != 0  // TRUE for the window menu
                    : menu == GetSystemMenu(hWnd, FALSE);
            if (isWindowMenu && IsTargetPwaWindow(hWnd)) {
                EnsureWindowMenuItem(hWnd, menu);
            }
            return result;
        }

        case WM_SYSCOMMAND: {
            UINT command = (UINT)(wParam & 0xFFF0);
            if (command == kMenuIdHideToCenter) {
                HideFromMenu(hWnd);
                return 0;
            }

            break;
        }

        case WM_COMMAND:
            // Fallback in case the menu command arrives as WM_COMMAND.
            if (HIWORD(wParam) == 0 && lParam == 0 &&
                LOWORD(wParam) == kMenuIdHideToCenter) {
                HideFromMenu(hWnd);
                return 0;
            }
            break;

        case WM_NCDESTROY: {
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                g_windows.erase(hWnd);
            }
            NotifyListChanged();
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void TrySubclass(HWND hWnd) {
    wchar_t className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)) ||
        wcscmp(className, kChromeWindowClass) != 0) {
        return;
    }

    // Menus, tooltips and other popups are owned windows.
    if (GetWindow(hWnd, GW_OWNER)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (!g_windows.emplace(hWnd, WindowState{}).second) {
            return;
        }
    }

    // Called outside the lock: the helper may send a message to the window's
    // thread, and the subclass procedure takes the same lock.
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, WindowSubclassProc,
                                                       0)) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_windows.erase(hWnd);
        return;
    }
    // Only the real browser gets here: the processes that just forward a command line
    // never create a top-level Chrome window, so they start no tray icon to leave behind.
    EnsureCenter();
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        TrySubclass(hWnd);
    }
    return TRUE;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && !hWndParent) {
        TrySubclass(hWnd);
    }
    return hWnd;
}

}  // namespace

BOOL Wh_ModInit() {
    // The mod is injected into every process of the browser. Only the main
    // browser process (the one without --type=) owns the top-level windows.
    PCWSTR commandLine = GetCommandLineW();
    if (commandLine && wcsstr(commandLine, L"--type=")) {
        return FALSE;
    }

    LoadSettings();
    g_hotkey = Wh_GetIntValue(L"prefHotkey", 0);
    g_simpleMode = Wh_GetIntSetting(L"simpleMode") != 0;
    PCWSTR simpleStyle = Wh_GetStringSetting(L"simpleStyle");
    g_simpleSingle = simpleStyle && wcscmp(simpleStyle, L"single") == 0;
    Wh_FreeStringSetting(simpleStyle);
    if (g_simpleMode) {
        g_hotkey = 0;  // no panel, so no shortcut
    }

    g_trayMsg = RegisterWindowMessageW(L"Windhawk_PwaCenter_Tray");
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    g_restoreMsg = RegisterWindowMessageW(L"Windhawk_PwaCenter_Restore");
    g_hideMsg = RegisterWindowMessageW(L"Windhawk_PwaCenter_Hide");
    g_quitMsg = RegisterWindowMessageW(L"Windhawk_PwaCenter_Quit");

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        return FALSE;
    }

    return TRUE;
}

// Settings changed in Windhawk: reload the mod, so that the new values (Simple mode and
// its style) apply at once. Windhawk restarts the mod, which is what happened when the
// mod was turned off and on again by hand.
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumWindowsProc, 0);
}


void Wh_ModUninit() {
    g_stopping = true;
    std::vector<HWND> windows;
    std::vector<HWND> hidden;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (const auto& [hWnd, state] : g_windows) {
            windows.push_back(hWnd);
            if (state.hiddenByUs) {
                hidden.push_back(hWnd);
            }
        }
    }

    // Never leave windows hidden without a way to bring them back.
    for (HWND hWnd : hidden) {
        if (IsWindow(hWnd)) {
            ShowWindowAsync(hWnd, SW_SHOW);
        }
    }

    // The center (panel, tray icon) and the worker go before the windows, they
    // use them.
    StopCenter();
    StopFrameThread();
    StopWorkThread();
    StopHookThread();

    // Outside the lock, the helper is a cross-thread SendMessage.
    for (HWND hWnd : windows) {
        if (IsWindow(hWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                             WindowSubclassProc);
        }
    }

    // Take the added item out of the window menus.
    for (HWND hWnd : windows) {
        if (IsWindow(hWnd)) {
            RemoveWindowMenuItem(hWnd);
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_windows.clear();
    }

}
