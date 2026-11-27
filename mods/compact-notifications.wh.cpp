// ==WindhawkMod==
// @id              compact-notifications
// @name            Compact Notifications
// @description     Replaces Windows notification banners with compact chat-style cards: avatar, sender and a few lines of text
// @version         1.2
// @author          Wheatlay
// @github          https://github.com/Wheatlay
// @license         GPL-3.0
// @include         ShellExperienceHost.exe
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -ld2d1 -ldwrite -lshcore -lgdi32 -lwindowscodecs -ladvapi32 -lshell32 -ldwmapi -lwinmm
// ==/WindhawkMod==

// The banner detection idea comes from the "Toast auto-dismiss" mod by nikitor32 (GPL-3.0),
// the way the banner window is told apart from other shell windows from "Notifications placement" by m417z (GPL-3.0),
// so this file is GPL-3.0 as well.

// ==WindhawkModReadme==
/*
# Compact Notifications

Replaces Windows toast banners with compact chat-style cards: black (or white) rounded
cards with a round avatar, the sender in bold and up to a few lines of text, sliding in from the screen edge.

![Cards](https://raw.githubusercontent.com/Wheatlay/compact-notifications/master/docs/screenshot.png)

## What works

* **Click** a card to do what clicking the original notification does (open the app or the conversation).
* **×** (on hover) closes the card; the notification stays in the notification center.
* **Moon** (on hover, top left) mutes the app for 15 min, 1 h, 8 h, 24 h or for good (∞): its cards go and its
  next notifications only go to the notification center. Timed mutes are kept in memory, so restarting Explorer or
  the mod ends them; ∞ is saved and lasts until you flip *Unmute apps muted for good* in the settings.
* **Buttons** from the notification (for example *Reply*, *Snooze*, *Dismiss*) are shown under the text.
* **Codes**: a notification with a one-time code (e.g. an SMS from Phone Link) gets a *Copy 123456* button,
  unless the app has its own copy button. It says "Copy" in the language of Windows (or in English, see the settings).
* **Reply box**: click it and type, then press the app's send button (or Enter). The text goes into the
  original reply box together with the button press, so the app receives it as usual.
* **Avatars**: the notification's image (local files and images from Store apps), or a thumbnail of its
  large picture (e.g. a screenshot); otherwise the app's registered icon (e.g. Firefox), and the first
  letter of the app as the last resort. Web images aren't downloaded.
* **Priority notifications** (reminders, alarms, incoming calls) get a colored outline and stay until handled;
  the card limit never pushes them out.
* **Chat formatting**: \*\*bold\*\*, \*italic\*, \_\_underline\_\_, \~\~strike\~\~ and \`code\` left in the text by
  chat apps are shown formatted (can be turned off).
* **Sound** plays when the card shows, not when Windows gets to the notification's banner later, so a few
  messages in a row don't keep chiming after their cards are gone. It is the sound Windows would play (or your
  own .wav), and it follows the Windows settings: no sound for silent notifications or apps with sound turned off.

## Settings

* **Appearance**: theme, size, lines of text, card width, corner radius, background opacity, your own card
  and text colors, distance from the screen edge.
* **Avatars**: on or off, circle, rounded square or square, the app's name before the sender.
* **Animation**: on or off, playful or plain, speed, how many cards show at once.
* **Position**: monitor and corner.
* **Sound**: when the card shows, as Windows plays it, or none; your own sound file.
* **Per-app rules**: for a single app (by the name shown on its notifications), hide the buttons, the reply
  box or the avatar, or don't show cards for it at all; its notifications still go to the notification center.

## How long a card stays

A card stays as long as the original notification would, counted from when it arrives. Windows sets this time in
*Settings > Accessibility > Visual effects > Dismiss notifications after this amount of time*. Reminders, alarms
and calls stay until handled.

## How it works

Windows shows one banner at a time and makes the next ones wait (about 7 seconds each). The mod makes cards
from the notification database as soon as a notification arrives, so messages sent in a row show together.
Windows still creates its own banner, but the mod moves it off the screen. Clicks and typing on a card go to
the hidden banner; for a notification still waiting for its banner, the mod opens the notification center off
the screen and acts there, so every app action works as before. Turning the mod off brings the original
banners back.

## Limitations

* Text styles, progress bars and large (hero) images are not shown.
* Pressing a button on a card whose notification is still waiting opens the notification center off the screen
  for a moment: it takes the keyboard focus from the app you're in (a full-screen game may minimize), and the
  action takes up to a second or two.
* While a full-screen app or *Do not disturb* is on, Windows doesn't show notifications at all, so there are
  no cards either.
* The reply box on a card is simple: typing, Backspace, Ctrl+Backspace, Ctrl+V, Enter to send and Esc; no
  caret movement or selection.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- appearance:
  - theme: auto
    $name: Theme
    $options:
    - auto: Follow Windows (black in dark mode, white in light mode)
    - dark: Always black
    - light: Always white
  - size: 130
    $name: Size (%)
    $description: 100 is the base size. From 80 to 200.
  - maxLines: 4
    $name: Lines of text
    $description: Longer messages make the card taller up to this many lines; the rest is cut off with "…". From 1 to 10.
  - width: 360
    $name: Card width
    $description: In pixels at 100% size. From 280 to 600.
  - cornerRadius: 12
    $name: Corner radius
    $description: Roundness of the cards, in pixels. 0 makes square corners. From 0 to 24.
  - opacity: 100
    $name: Background opacity (%)
    $description: Below 100 the desktop shows through the cards. From 30 to 100.
  - cardColor: ""
    $name: Card color
    $description: Hex color like #202020. Empty uses the theme's color.
  - textColor: ""
    $name: Text color
    $description: Hex color like #FFFFFF. Empty uses the theme's color.
  - margin: 0
    $name: Distance from the screen edge
    $description: Extra space between the cards and the screen edge, in pixels. From 0 to 200.
  $name: Appearance
- avatars:
  - show: true
    $name: Show avatars
    $description: When off, the text starts at the left edge of the card.
  - shape: circle
    $name: Shape
    $options:
    - circle: Circle
    - rounded: Rounded square
    - square: Square
  - appName: false
    $name: Show the app name
    $description: Adds the app's name before the sender, e.g. "Discord · Anna".
  $name: Avatars
- animation:
  - enabled: true
    $name: Animate cards
    $description: Slide in and fade out. When off, cards appear and disappear at once.
  - playful: true
    $name: Playful animations
    $description: Cards pop out of the corner like a bubble and shrink away, the other cards glide to their new place, pressed buttons bounce. When off, or when Windows animation effects are off, cards just slide in and fade out.
  - speed: 100
    $name: Animation speed (%)
    $description: 200 is twice as fast, 50 half as fast. From 25 to 400.
  - maxCards: 5
    $name: Most cards at once
    $description: When more notifications come, the oldest cards fade out (the notifications stay in the notification center). From 1 to 10.
  $name: Animation
- position:
  - monitor: cursor
    $name: Monitor
    $options:
    - cursor: The one with the mouse pointer
    - primary: The primary monitor
  - corner: bottomRight
    $name: Corner
    $options:
    - bottomRight: Bottom right
    - topRight: Top right
    - bottomLeft: Bottom left
    - topLeft: Top left
  $name: Position
- behavior:
  - showActions: true
    $name: Show buttons and reply boxes
    $description: When off, cards only show the text; use the notification center to reply.
  - copyCodes: true
    $name: Copy button for codes
    $description: A notification with a one-time code (for example an SMS from Phone Link) gets a button that copies the code, unless the app already has its own copy button.
  - copyInSystemLanguage: true
    $name: Copy button in the Windows language
    $description: The button says "Copy" in the language of Windows (the word Windows uses in its own menus). Turn off for English.
  - markdown: true
    $name: Format chat text
    $description: Shows **bold**, *italic*, __underline__, ~~strike~~ and `code` as chat apps (Discord, Slack, Teams) mean them, instead of the raw marks. Turn off if an app's text loses characters (e.g. file names like __init__.py).
  - highlightPriority: true
    $name: Highlight priority notifications
    $description: Colored outline for reminders, alarms and incoming calls.
  - unmuteAll: false
    $name: Unmute apps muted for good
    $description: The moon button's ∞ mutes an app until you change this switch (either way) and save.
  - priorityColor: "#F0A020"
    $name: Priority color
    $description: Hex color, for example #F0A020 (orange) or #E0102F (red).
  $name: Behavior
- sound:
  - mode: card
    $name: Notification sound
    $options:
    - card: When the card shows
    - windows: As Windows plays it (when its banner would show, can come after the card is gone)
    - off: No sound
    $description: Reminders, alarms and calls keep their looping sound from Windows either way.
  - file: ""
    $name: Sound file
    $description: Full path to a .wav file played instead of the Windows sound, e.g. C:\Users\Me\ding.wav. Empty uses the sound from the Windows sound scheme. Only with "When the card shows".
  $name: Sound
- appRules:
  - - app: ""
      $name: App name
      $description: As shown on the notification or in the notification center, e.g. "Discord" or "Mozilla Firefox". Not case-sensitive.
    - show: card
      $name: Notifications from this app
      $options:
      - card: Show a card
      - mute: Don't show (they still go to the notification center)
    - buttons: true
      $name: Show buttons
    - replyBox: true
      $name: Show the reply box
    - avatar: true
      $name: Show the avatar
  $name: Per-app rules
  $description: Settings for single apps; they override the ones above. Add one entry per app.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <shellscalingapi.h>
#include <uiautomation.h>
#include <wincodec.h>
#include <appmodel.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <dwmapi.h>
#include <mmsystem.h>

#include <windhawk_utils.h>

#include <climits>
#include <wchar.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

#ifndef Log
#define Log(format, ...) Wh_Log(format, ##__VA_ARGS__)
#endif

// The banner window title in every Windows display language (String resource \ActionCenter\AC_ToastCenter_Title in
// Windows.UI.ShellCommon.<locale>.pri). The list comes from the "Notifications placement" mod by m417z, which collected
// it from https://github.com/m417z/windows-language-files.
// clang-format off
const wchar_t* const kBannerTitles[] = {
    L"Nuwe kennisgewing", // AF-ZA
    L"አዲስ ማሳወቂያ", // AM-ET
    L"নতুন জাননী", // AS-IN
    L"Yeni bildiriş", // AZ-LATN-AZ
    L"Новае апавяшчэнне", // BE-BY
    L"নতুন বিজ্ঞপ্তি", // BN-IN
    L"Novo obavještenje", // BS-LATN-BA
    L"Notificació nova", // CA-ES-VALENCIA
    L"ᎢᏤᎢ ᎧᏃᎮᏓ", // CHR-CHER-US
    L"Hysbysiad newydd", // CY-GB
    L"اعلان جدید", // FA-IR
    L"Bagong notification", // FIL-PH
    L"Fógra nua", // GA-IE
    L"Brath ùr", // GD-GB
    L"નવી સૂચના", // GU-IN
    L"नई अधिसूचना", // HI-IN
    L"Նոր ծանուցում", // HY-AM
    L"Ný tilkynning", // IS-IS
    L"ახალი შეტყობინება", // KA-GE
    L"Жаңа хабарландыру", // KK-KZ
    L"ការ\u200bជូន\u200bដំណឹង\u200bថ្មី", // KM-KH
    L"ಹೊಸ ಪ್ರಕಟಣೆ", // KN-IN
    L"नवी अधिसुचोवणी", // KOK-IN
    L"Nei Notifikatioun", // LB-LU
    L"ການແຈ້ງເຕືອນໃໝ່", // LO-LA
    L"Whakamōhiotanga hōu", // MI-NZ
    L"Ново известување", // MK-MK
    L"പുതിയ അറിയിപ്പ്", // ML-IN
    L"नवीन सूचना", // MR-IN
    L"Pemberitahuan baharu", // MS-MY
    L"Notifika ġdida", // MT-MT
    L"नयाँ सूचना", // NE-NP
    L"Nytt varsel", // NN-NO
    L"ନୂତନ ବିଜ୍ଞପ୍ତି", // OR-IN
    L"ਨਵੀਂ ਸੂਚਨਾ", // PA-IN
    L"Musuq willana", // QUZ-PE
    L"Njoftim i ri", // SQ-AL
    L"Ново обавјештење", // SR-CYRL-BA
    L"Ново обавештење", // SR-CYRL-RS
    L"புதிய அறிவிப்பு", // TA-IN
    L"కొత్త నోటిఫికేషన్", // TE-IN
    L"Яңа белдерү", // TT-RU
    L"يېڭى ئۇقتۇرۇش", // UG-CN
    L"نئی اطلاع", // UR-PK
    L"Yangi xabarnoma", // UZ-LATN-UZ
    L"\u200f\u200fإعلام جديد", // AR-SA
    L"Ново известие", // BG-BG
    L"Notificació nova", // CA-ES
    L"Nové oznámení", // CS-CZ
    L"Ny meddelelse", // DA-DK
    L"Neue Benachrichtigung", // DE-DE
    L"Νέα ειδοποίηση", // EL-GR
    L"New notification", // EN-GB
    L"New notification", // EN-US
    L"Notificación nueva", // ES-ES
    L"Nueva notificación", // ES-MX
    L"Uus teatis", // ET-EE
    L"Jakinarazpen berria", // EU-ES
    L"Uusi ilmoitus", // FI-FI
    L"Nouvelle notification", // FR-CA
    L"Nouvelle notification", // FR-FR
    L"Nova notificación", // GL-ES
    L"הודעה חדשה", // HE-IL
    L"Nova obavijesti", // HR-HR
    L"Új értesítés", // HU-HU
    L"Pemberitahuan baru", // ID-ID
    L"Nuova notifica", // IT-IT
    L"新しい通知", // JA-JP
    L"새 알림", // KO-KR
    L"Naujas pranešimas", // LT-LT
    L"Jauns paziņojums", // LV-LV
    L"Ny varsling", // NB-NO
    L"Nieuwe melding", // NL-NL
    L"Nowe powiadomienie", // PL-PL
    L"Nova notificação", // PT-BR
    L"Nova notificação", // PT-PT
    L"Notificare nouă", // RO-RO
    L"Новое уведомление", // RU-RU
    L"Nové oznámenie", // SK-SK
    L"Novo obvestilo", // SL-SI
    L"Novo obaveštenje", // SR-LATN-RS
    L"Nytt meddelande", // SV-SE
    L"การแจ้งให้ทราบใหม่", // TH-TH
    L"Yeni bildirim", // TR-TR
    L"Нове сповіщення", // UK-UA
    L"Thông báo mới", // VI-VN
    L"新通知", // ZH-CN
    L"新通知", // ZH-TW
};
// clang-format on

bool IsBannerTitle(HWND hwnd) {
    WCHAR title[64];
    if (!InternalGetWindowText(hwnd, title, ARRAYSIZE(title))) {
        return false;
    }
    for (auto known : kBannerTitles) {
        if (wcscmp(title, known) == 0) {
            return true;
        }
    }
    return false;
}

bool IsCoreWindowClass(HWND hwnd) {
    WCHAR className[32];
    return GetClassNameW(hwnd, className, ARRAYSIZE(className)) && wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0;
}

bool ProcessIsShellExperienceHost(DWORD pid) {
    bool result = false;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (h) {
        WCHAR path[MAX_PATH] = {};
        DWORD size = ARRAYSIZE(path);
        if (QueryFullProcessImageNameW(h, 0, path, &size)) {
            const WCHAR* base = wcsrchr(path, L'\\');
            result = _wcsicmp(base ? base + 1 : path, L"ShellExperienceHost.exe") == 0;
        }
        CloseHandle(h);
    }
    return result;
}

// The banner window, cached by handle: a window never changes process or class.
bool IsBannerWindow(HWND hwnd) {
    static std::atomic<HWND> lastYes{nullptr};
    if (!hwnd || !IsCoreWindowClass(hwnd) || !IsBannerTitle(hwnd)) {
        return false;
    }
    if (lastYes.load() == hwnd) {
        return true;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!ProcessIsShellExperienceHost(pid)) {
        return false;
    }
    lastYes.store(hwnd);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Hiding: inside ShellExperienceHost and explorer, every move of the banner window goes off the screen.

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;

int OffscreenX() {
    return GetSystemMetrics(SM_XVIRTUALSCREEN) + GetSystemMetrics(SM_CXVIRTUALSCREEN) + 200;
}

// The card window (in the shell's explorer). The banner is hidden only while it exists, so if nothing draws cards
// (explorer restarting, Direct2D unavailable) the normal banners show.
constexpr wchar_t kWindowClass[] = L"CompactNotifications";

// The card window's title while the mod opens the notification center itself (see OpenCenter): the center is kept
// off the screen then. A window title is the flag because ShellExperienceHost, an app container, can't open the
// named objects of explorer, but can find its windows.
constexpr wchar_t kCardTitle[] = L"Compact Notifications";
constexpr wchar_t kCenterTitle[] = L"Compact Notifications (center)";

bool g_explorer = false;  // this copy runs in explorer (else in ShellExperienceHost)

std::atomic<int> g_bannerX{INT_MIN};  // where Windows last put the banner, for RestoreBanner

// The notification center: ShellExperienceHost's other core window.
bool IsCenterWindow(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return !g_explorer && pid == GetCurrentProcessId() && IsCoreWindowClass(hwnd) && !IsBannerTitle(hwnd);
}

BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if ((uFlags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOMOVE | SWP_NOSIZE) &&
        ((IsBannerWindow(hWnd) && FindWindowW(kWindowClass, nullptr)) ||
         (IsCenterWindow(hWnd) && FindWindowW(kWindowClass, kCenterTitle)))) {
        if (uFlags & SWP_NOMOVE) {
            RECT rc{};
            GetWindowRect(hWnd, &rc);
            Y = rc.top;
            uFlags &= ~SWP_NOMOVE;
        } else if (IsBannerTitle(hWnd)) {
            g_bannerX = X;
        }
        X = OffscreenX();
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// This process's banner window, if it has one (ShellExperienceHost).
HWND OwnBanner() {
    HWND hwnd = nullptr;
    while ((hwnd = FindWindowExW(HWND_DESKTOP, hwnd, L"Windows.UI.Core.CoreWindow", nullptr)) != nullptr) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId() && IsBannerWindow(hwnd)) {
            return hwnd;
        }
    }
    return nullptr;
}

void HideExistingBanner() {
    HWND hwnd = OwnBanner();
    RECT rc{};
    if (hwnd && FindWindowW(kWindowClass, nullptr) && GetWindowRect(hwnd, &rc)) {
        SetWindowPos_Original(hwnd, nullptr, OffscreenX(), rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void RestoreBanner() {
    HWND hwnd = OwnBanner();
    RECT rc{};
    int x = g_bannerX;
    if (hwnd && x != INT_MIN && GetWindowRect(hwnd, &rc) && rc.left >= OffscreenX()) {
        SetWindowPos_Original(hwnd, nullptr, x, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Reading: a worker thread finds the banner and reads its toasts through UI Automation.

struct ToastInfo {
    // App, title and text (plus a counter for identical toasts). UIA runtime ids change while the banner lives.
    std::wstring id;
    long long arrival = 0;  // a stored toast's ArrivalTime (its row in the database), 0 for one read from the banner
    std::wstring app;
    std::wstring title;
    std::wstring text;
    std::wstring image;  // local file of the avatar (appLogoOverride), empty if none
    std::vector<std::wstring> buttons;
    bool hasInput = false;
    std::wstring placeholder;
    std::wstring inputValue;  // the reply typed on the card (the card thread's own state)
    bool inputFocused = false;
    bool priority = false;  // reminders, alarms, calls: they stay until handled
    std::wstring aumid;     // the app's id (stored toasts only), for its sound setting
    bool silent = false;    // <audio silent="true"/>
    std::wstring sound;     // sound alias from <audio src="ms-winsoundevent:...">, empty for the default
    bool appSound = false;  // <audio src> is the app's own file: Windows plays it (it isn't under %WINDIR%\Media)
};

// The <audio> element of a toast: silent, a Windows sound event or the app's own file.
void ReadAudio(const std::string& payload, const std::wstring& aumid, ToastInfo& info);

////////////////////////////////////////////////////////////////////////////////
// Avatar: UI Automation doesn't expose images, so the toast XML is read from the notification database
// (%LOCALAPPDATA%\Microsoft\Windows\Notifications\wpndatabase.db) through the SQLite that ships with Windows.

struct sqlite3;
struct sqlite3_stmt;
struct Sqlite {
    int (*open_v2)(const char*, sqlite3**, int, const char*);
    int (*prepare_v2)(sqlite3*, const char*, int, sqlite3_stmt**, const char**);
    int (*step)(sqlite3_stmt*);
    const void* (*column_blob)(sqlite3_stmt*, int);
    int (*column_bytes)(sqlite3_stmt*, int);
    int (*finalize)(sqlite3_stmt*);
    int (*close)(sqlite3*);
    bool loaded = false;
} g_sqlite;

constexpr int SQLITE_OPEN_READONLY = 0x1;
constexpr int SQLITE_ROW = 100;
constexpr int SQLITE_DONE = 101;

bool LoadSqlite() {
    if (g_sqlite.loaded) {
        return true;
    }
    HMODULE module = LoadLibraryExW(L"winsqlite3.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Log(L"winsqlite3.dll unavailable");
        return false;
    }
    auto get = [module](const char* name) { return (void*)GetProcAddress(module, name); };
    *(void**)&g_sqlite.open_v2 = get("sqlite3_open_v2");
    *(void**)&g_sqlite.prepare_v2 = get("sqlite3_prepare_v2");
    *(void**)&g_sqlite.step = get("sqlite3_step");
    *(void**)&g_sqlite.column_blob = get("sqlite3_column_blob");
    *(void**)&g_sqlite.column_bytes = get("sqlite3_column_bytes");
    *(void**)&g_sqlite.finalize = get("sqlite3_finalize");
    *(void**)&g_sqlite.close = get("sqlite3_close");
    g_sqlite.loaded = g_sqlite.open_v2 && g_sqlite.prepare_v2 && g_sqlite.step && g_sqlite.column_blob &&
                      g_sqlite.column_bytes && g_sqlite.finalize && g_sqlite.close;
    return g_sqlite.loaded;
}

std::string ToUtf8(const std::wstring& text) {
    int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), nullptr, 0, nullptr, nullptr);
    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), result.data(), size, nullptr, nullptr);
    return result;
}

std::wstring FromUtf8(const std::string& text) {
    int size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (int)text.size(), nullptr, 0);
    std::wstring result(size, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (int)text.size(), result.data(), size);
    return result;
}

std::string XmlUnescape(std::string s) {
    const std::pair<const char*, const char*> entities[] = {
        {"&lt;", "<"}, {"&gt;", ">"}, {"&quot;", "\""}, {"&apos;", "'"}, {"&#xA;", "\n"}, {"&amp;", "&"}};
    for (auto [from, to] : entities) {
        for (size_t at = 0; (at = s.find(from, at)) != std::string::npos; at += strlen(to)) {
            s.replace(at, strlen(from), to);
        }
    }
    return s;
}

std::string PercentDecode(const std::string& s) {
    std::string result;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '%' && i + 2 < s.size() && isxdigit((unsigned char)s[i + 1]) && isxdigit((unsigned char)s[i + 2])) {
            result += (char)strtol(s.substr(i + 1, 2).c_str(), nullptr, 16);
            i += 2;
        } else {
            result += s[i];
        }
    }
    return result;
}

// Value of an attribute inside one tag, still XML-escaped.
std::string Attribute(const std::string& tag, const char* name) {
    std::string key = std::string(" ") + name + "=\"";
    size_t at = tag.find(key);
    if (at == std::string::npos) {
        return {};
    }
    at += key.size();
    size_t end = tag.find('"', at);
    return end == std::string::npos ? std::string() : tag.substr(at, end - at);
}

bool FileExists(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

// Install folder of a Store app from its package family name (the part of the AUMID before "!").
std::wstring PackagePath(const std::wstring& family) {
    UINT32 count = 0, bufferLength = 0;
    if (GetPackagesByPackageFamily(family.c_str(), &count, nullptr, &bufferLength, nullptr) != ERROR_INSUFFICIENT_BUFFER ||
        !count) {
        return {};
    }
    std::vector<PWSTR> names(count);
    std::vector<WCHAR> buffer(bufferLength);
    if (GetPackagesByPackageFamily(family.c_str(), &count, names.data(), &bufferLength, buffer.data()) != ERROR_SUCCESS) {
        return {};
    }
    UINT32 length = 0;
    GetPackagePathByFullName(names[0], &length, nullptr);
    std::wstring path(length, L'\0');
    if (!length || GetPackagePathByFullName(names[0], &length, path.data()) != ERROR_SUCCESS) {
        return {};
    }
    path.resize(wcslen(path.c_str()));
    return path;
}

// Store apps ship images in several scales (logo.scale-200.png) and refer to them without one (logo.png).
std::wstring WithScale(const std::wstring& path) {
    if (FileExists(path)) {
        return path;
    }
    size_t dot = path.rfind(L'.');
    if (dot == std::wstring::npos) {
        return {};
    }
    for (const wchar_t* scale : {L".scale-200", L".scale-150", L".scale-125", L".scale-100", L".scale-400"}) {
        std::wstring candidate = path.substr(0, dot) + scale + path.substr(dot);
        if (FileExists(candidate)) {
            return candidate;
        }
    }
    return {};
}

// The avatar file from a toast payload: the appLogoOverride image as a local file (web images are skipped);
// ms-appx and ms-appdata point into the app's package and data folders.
// Without one, any other image (hero or inline) stands in, e.g. the screenshot Snipping Tool attaches.
std::wstring ImageFromPayload(const std::string& payload, const std::wstring& aumid, bool logoOnly);
std::wstring AvatarFromPayload(const std::string& payload, const std::wstring& aumid) {
    std::wstring logo = ImageFromPayload(payload, aumid, true);
    return logo.empty() ? ImageFromPayload(payload, aumid, false) : logo;
}

std::wstring ImageFromPayload(const std::string& payload, const std::wstring& aumid, bool logoOnly) {
    for (size_t at = 0; (at = payload.find("<image", at)) != std::string::npos; at++) {
        size_t end = payload.find('>', at);
        if (end == std::string::npos) {
            break;
        }
        std::string tag = payload.substr(at, end - at);
        if ((Attribute(tag, "placement") == "appLogoOverride") != logoOnly) {
            continue;
        }
        std::wstring src = FromUtf8(XmlUnescape(Attribute(tag, "src")));
        std::wstring family = aumid.substr(0, aumid.find(L'!'));
        auto after = [&](const wchar_t* prefix) {
            std::wstring rest = FromUtf8(PercentDecode(ToUtf8(src.substr(wcslen(prefix)))));
            std::replace(rest.begin(), rest.end(), L'/', L'\\');
            return rest;
        };
        auto starts = [&](const wchar_t* prefix) { return _wcsnicmp(src.c_str(), prefix, wcslen(prefix)) == 0; };
        if (starts(L"http://") || starts(L"https://")) {
            continue;  // web images aren't fetched (no network access); the app icon or the letter is used instead
        }
        if (starts(L"ms-appx:///")) {
            std::wstring root = PackagePath(family);
            return root.empty() ? std::wstring() : WithScale(root + L"\\" + after(L"ms-appx:///"));
        }
        if (starts(L"ms-appdata:///")) {
            // ms-appdata:///local/x.png -> %LOCALAPPDATA%\Packages\<family>\LocalState\x.png (also roaming, temp).
            std::wstring rest = after(L"ms-appdata:///");
            const std::pair<const wchar_t*, const wchar_t*> folders[] = {
                {L"local\\", L"LocalState\\"}, {L"roaming\\", L"RoamingState\\"}, {L"temp\\", L"TempState\\"}};
            WCHAR localAppData[MAX_PATH];
            if (!GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, ARRAYSIZE(localAppData))) {
                return {};
            }
            for (auto [from, to] : folders) {
                if (_wcsnicmp(rest.c_str(), from, wcslen(from)) == 0) {
                    return WithScale(std::wstring(localAppData) + L"\\Packages\\" + family + L"\\" + to +
                                     rest.substr(wcslen(from)));
                }
            }
            return {};
        }
        if (starts(L"file:///")) {
            // Local drives only: a network path (file://///server/share) would be opened over SMB.
            std::wstring path = after(L"file:///");
            return path.size() > 2 && path[1] == L':' ? path : std::wstring();
        }
        std::replace(src.begin(), src.end(), L'/', L'\\');
        return src.size() > 2 && src[1] == L':' ? src : std::wstring();
    }
    return {};
}

std::wstring NotificationFolder() {
    WCHAR localAppData[MAX_PATH];
    if (!GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, ARRAYSIZE(localAppData))) {
        return {};
    }
    return std::wstring(localAppData) + L"\\Microsoft\\Windows\\Notifications";
}

// One read-only connection, opened when first needed and kept by the worker thread (the only one that reads the
// database) until it ends: the worker reads every pass while cards are up. Closed and opened again after an error.
sqlite3* g_db = nullptr;

sqlite3* OpenNotificationDb() {
    if (g_db) {
        return g_db;
    }
    std::wstring folder = NotificationFolder();
    if (folder.empty() || !LoadSqlite()) {
        return nullptr;
    }
    std::string path = ToUtf8(folder + L"\\wpndatabase.db");
    if (g_sqlite.open_v2(path.c_str(), &g_db, SQLITE_OPEN_READONLY, nullptr) != 0) {
        Log(L"Opening the notification database failed");
        if (g_db) {
            g_sqlite.close(g_db);
            g_db = nullptr;
        }
    }
    return g_db;
}

void CloseNotificationDb() {
    if (g_db) {
        g_sqlite.close(g_db);
        g_db = nullptr;
    }
}

// Runs a query on the shared connection; on a failed prepare the connection is dropped, so the next read reopens it.
sqlite3_stmt* Prepare(const std::string& sql) {
    sqlite3* db = OpenNotificationDb();
    sqlite3_stmt* stmt = nullptr;
    if (db && g_sqlite.prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != 0) {
        stmt = nullptr;
        CloseNotificationDb();
    }
    return stmt;
}

// Finds the newest stored toast whose text contains both the title and the message, and returns its avatar.
// A card made from the banner also takes the toast's sound details from it (into audio, when given).
std::wstring FindAvatar(const std::wstring& title, const std::wstring& text, ToastInfo* audio = nullptr) {
    // Only the start of the message: the banner can show it with other line breaks or cut off (Firefox, Gmail).
    std::wstring start = text.substr(0, std::min(text.find_first_of(L"\r\n"), (size_t)24));
    std::string wantTitle = ToUtf8(title), wantText = ToUtf8(start);
    std::string payload, aumid;
    if (sqlite3_stmt* stmt =
            Prepare("SELECT n.Payload, h.PrimaryId FROM Notification n JOIN NotificationHandler h "
                    "ON h.RecordId = n.HandlerId WHERE n.Type = 'toast' ORDER BY n.ArrivalTime DESC LIMIT 30")) {
        while (g_sqlite.step(stmt) == SQLITE_ROW) {
            auto data = (const char*)g_sqlite.column_blob(stmt, 0);
            int size = g_sqlite.column_bytes(stmt, 0);
            if (!data || size <= 0) {
                continue;
            }
            std::string candidate(data, size);
            std::string plain = XmlUnescape(candidate);
            if (plain.find(wantTitle) != std::string::npos && plain.find(wantText) != std::string::npos) {
                payload = candidate;
                if (auto id = (const char*)g_sqlite.column_blob(stmt, 1)) {
                    aumid.assign(id, g_sqlite.column_bytes(stmt, 1));
                }
                break;
            }
        }
        g_sqlite.finalize(stmt);
    }
    if (payload.empty()) {
        return std::wstring();
    }
    if (audio) {
        ReadAudio(payload, FromUtf8(aumid), *audio);
    }
    return AvatarFromPayload(payload, FromUtf8(aumid));
}

// The icon a desktop app registered for its notifications (HKCU\Software\Classes\AppUserModelId\<id>, IconUri),
// found by the app name the banner shows. Used when the toast itself has no image or isn't in the database
// (older Firefox versions didn't store their toasts there).
std::wstring AppIcon(const std::wstring& app) {
    HKEY root;
    if (app.empty() || RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Classes\\AppUserModelId", 0, KEY_READ, &root) !=
                           ERROR_SUCCESS) {
        return {};
    }
    std::wstring icon;
    WCHAR name[512];
    for (DWORD i = 0; icon.empty(); i++) {
        DWORD nameLength = ARRAYSIZE(name);
        if (RegEnumKeyExW(root, i, name, &nameLength, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) {
            break;
        }
        WCHAR value[MAX_PATH];
        DWORD size = sizeof(value);
        if (RegGetValueW(root, name, L"DisplayName", RRF_RT_REG_SZ, nullptr, value, &size) != ERROR_SUCCESS ||
            app != value) {
            continue;
        }
        size = sizeof(value);
        if (RegGetValueW(root, name, L"IconUri", RRF_RT_REG_SZ, nullptr, value, &size) == ERROR_SUCCESS &&
            value[1] == L':' && FileExists(value)) {  // local drives only, no network paths
            icon = value;
        }
    }
    RegCloseKey(root);
    return icon;
}

////////////////////////////////////////////////////////////////////////////////
// Stored toasts: Windows shows one banner at a time and queues the rest (about 7 s each), but writes every toast
// to the database at once. Cards are made from there, so toasts that come in a row show together.

// Every <name ...>content</name> (self-closing ones with empty content): the opening tag and the content,
// both still XML-escaped.
std::vector<std::pair<std::string, std::string>> Elements(const std::string& xml, const char* name) {
    std::vector<std::pair<std::string, std::string>> result;
    std::string open = std::string("<") + name, close = std::string("</") + name + ">";
    for (size_t at = 0; (at = xml.find(open, at)) != std::string::npos; at += open.size()) {
        char next = at + open.size() < xml.size() ? xml[at + open.size()] : 0;
        if (next != ' ' && next != '>' && next != '/') {
            continue;  // <actions> when looking for <action>
        }
        size_t end = xml.find('>', at);
        if (end == std::string::npos) {
            break;
        }
        std::string content;
        if (xml[end - 1] != '/') {
            size_t stop = xml.find(close, end);
            if (stop == std::string::npos) {
                break;
            }
            content = xml.substr(end + 1, stop - end - 1);
        }
        result.push_back({xml.substr(at, end - at), content});
    }
    return result;
}

// The app's name as the banner shows it: what Start shows for the AppUserModelID, else the name the app registered
// for its notifications (HKCU\Software\Classes\AppUserModelId\<id>, e.g. Firefox's per-install id).
std::wstring AppName(const std::wstring& aumid) {
    std::wstring name;
    IShellItem* item = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName((L"shell:AppsFolder\\" + aumid).c_str(), nullptr, IID_PPV_ARGS(&item)))) {
        PWSTR display = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &display)) && display) {
            name = display;
            CoTaskMemFree(display);
        }
        item->Release();
    }
    if (name.empty()) {
        WCHAR value[256];
        DWORD size = sizeof(value);
        if (RegGetValueW(HKEY_CURRENT_USER, (L"Software\\Classes\\AppUserModelId\\" + aumid).c_str(), L"DisplayName",
                         RRF_RT_REG_SZ, nullptr, value, &size) == ERROR_SUCCESS) {
            name = value;
        }
    }
    return name;
}

DWORD UserDword(const std::wstring& key, const wchar_t* name, DWORD missing) {
    DWORD value = 0, size = sizeof(value);
    return RegGetValueW(HKEY_CURRENT_USER, key.c_str(), name, RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS
               ? value
               : missing;
}

// Do not disturb (Focus assist): SHQueryUserNotificationState doesn't report it on Windows 11. The shell publishes the
// active profile as the WNF state WNF_SHEL_QUIETHOURS_ACTIVE_PROFILE_CHANGED (0 off, 1 priority only, 2 alarms only),
// read through ntdll's NtQueryWnfStateData (undocumented; if it's missing or fails, the check is skipped).
bool QuietHours() {
    using Query = LONG(NTAPI*)(const ULONGLONG*, const void*, const void*, ULONG*, void*, ULONG*);
    static Query query = (Query)(void*)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryWnfStateData");
    if (!query) {
        return false;
    }
    const ULONGLONG name = 0x0D83063EA3BF1C75;
    ULONG changeStamp = 0, profile = 0, size = sizeof(profile);
    return query(&name, nullptr, nullptr, &changeStamp, &profile, &size) >= 0 && size == sizeof(profile) && profile;
}

// Whether Windows shows a banner for the app's toasts: notifications on, the app's banners on, no Do not disturb
// or full-screen app. Otherwise the toast only goes to the notification center, and gets no card either. (With Do not
// disturb, apps allowed through it still get cards: from their banner, which Windows does show.)
bool BannerAllowed(const std::wstring& aumid) {
    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state)) && state != QUNS_ACCEPTS_NOTIFICATIONS && state != QUNS_APP) {
        return false;
    }
    if (QuietHours()) {
        return false;
    }
    const std::wstring settings = L"Software\\Microsoft\\Windows\\CurrentVersion\\Notifications\\Settings";
    return UserDword(L"Software\\Microsoft\\Windows\\CurrentVersion\\PushNotifications", L"ToastEnabled", 1) &&
           UserDword(settings, L"NOC_GLOBAL_SETTING_TOASTS_ENABLED", 1) &&
           UserDword(settings + L"\\" + aumid, L"Enabled", 1) && UserDword(settings + L"\\" + aumid, L"ShowBanner", 1);
}

// The sound for a new card, as Windows would play it for the banner (see CreateFileW_Hook): not for silent toasts,
// with notification sounds turned off (for all apps or for this one), or for a toast that isn't fresh (cards made
// for toasts already there when the mod starts). Looping sounds stay with the banner. Returns whether it played.
void ReadAudio(const std::string& payload, const std::wstring& aumid, ToastInfo& info) {
    info.aumid = aumid;
    for (auto& [tag, content] : Elements(payload, "audio")) {
        info.silent = Attribute(tag, "silent") == "true";
        std::string src = Attribute(tag, "src");
        if (src.rfind("ms-winsoundevent:", 0) == 0) {
            info.sound = FromUtf8(src.substr(17));
        } else {
            info.appSound = !src.empty();
        }
        break;
    }
}

bool PlayCardSound(const ToastInfo& info, const std::wstring& file) {
    if (info.silent || info.appSound || info.sound.find(L"Looping") != std::wstring::npos) {
        return false;
    }
    const std::wstring settings = L"Software\\Microsoft\\Windows\\CurrentVersion\\Notifications\\Settings";
    if (!UserDword(settings, L"NOC_GLOBAL_SETTING_ALLOW_NOTIFICATION_SOUND", 1) ||
        (!info.aumid.empty() && !UserDword(settings + L"\\" + info.aumid, L"SoundFile", 1))) {
        return false;
    }
    if (info.arrival) {
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        long long now = ((long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
        if (now - info.arrival > 30LL * 10000000) {
            return false;
        }
    }
    // On the system sounds volume, without blocking the card thread, and nothing when the sound is missing.
    constexpr DWORD kFlags = SND_ASYNC | SND_NODEFAULT | SND_SYSTEM;
    // Own file from the settings; a missing or broken file falls back to the sound from the Windows sound scheme.
    if (!file.empty() && PlaySoundW(file.c_str(), nullptr, kFlags | SND_FILENAME)) {
        Log(L"Card sound played (own file)");
        return true;
    }
    std::wstring alias = info.sound.empty() ? L"Notification.Default" : info.sound;
    if (!PlaySoundW(alias.c_str(), nullptr, kFlags | SND_ALIAS) && alias != L"Notification.Default") {
        PlaySoundW(L"Notification.Default", nullptr, kFlags | SND_ALIAS);
    }
    Log(L"Card sound played");
    return true;
}

ToastInfo ToastFromPayload(const std::string& payload, const std::wstring& aumid) {
    ToastInfo info;
    std::vector<std::wstring> texts;
    for (auto& [tag, content] : Elements(payload, "text")) {
        if (!content.empty() && Attribute(tag, "placement") != "attribution") {
            texts.push_back(FromUtf8(XmlUnescape(content)));
        }
    }
    for (size_t i = 0; i < texts.size(); i++) {
        if (i == 0) {
            info.title = texts[0];
        } else {
            info.text += (i > 1 ? L"\n" : L"") + texts[i];
        }
    }
    for (auto& [tag, content] : Elements(payload, "action")) {
        if (_stricmp(Attribute(tag, "placement").c_str(), "contextMenu") != 0) {  // Firefox writes "contextmenu"
            info.buttons.push_back(FromUtf8(XmlUnescape(Attribute(tag, "content"))));
        }
    }
    for (auto& [tag, content] : Elements(payload, "input")) {
        if (Attribute(tag, "type") == "text") {
            info.hasInput = true;
            info.placeholder = FromUtf8(XmlUnescape(Attribute(tag, "placeHolderContent")));
            break;
        }
    }
    std::string scenario = Attribute(payload.substr(0, payload.find('>')), "scenario");
    info.priority = scenario == "reminder" || scenario == "alarm" || scenario == "incomingCall" || scenario == "urgent";
    ReadAudio(payload, aumid, info);
    info.app = AppName(aumid);
    info.image = AvatarFromPayload(payload, aumid);
    if (info.image.empty()) {
        info.image = AppIcon(info.app);
    }
    return info;
}

std::string Column(sqlite3_stmt* stmt, int index) {
    auto data = (const char*)g_sqlite.column_blob(stmt, index);
    int size = g_sqlite.column_bytes(stmt, index);
    return data && size > 0 ? std::string(data, size) : std::string();
}

// Toasts that arrived after lastArrival (ArrivalTime, a FILETIME). Not [Order]: it is the row id, and when an app
// removes its newest toasts (Discord does once a message is read) the next ones get the same numbers again.
// The first call (lastArrival -1) only finds the newest one: older toasts came before the mod.
std::vector<ToastInfo> NewStoredToasts(long long& lastArrival) {
    std::vector<ToastInfo> result;
    bool first = lastArrival < 0;
    bool read = false;  // a row came back: only then the first call sets where reading starts
    std::string sql = first ? "SELECT MAX(ArrivalTime) FROM Notification"
                            : "SELECT n.ArrivalTime, n.Payload, h.PrimaryId FROM Notification n JOIN NotificationHandler h "
                              "ON h.RecordId = n.HandlerId WHERE n.Type = 'toast' AND n.ArrivalTime > " +
                                  std::to_string(lastArrival) + " ORDER BY n.ArrivalTime";
    if (sqlite3_stmt* stmt = Prepare(sql)) {
        while (g_sqlite.step(stmt) == SQLITE_ROW) {
            read = true;
            long long arrival = atoll(Column(stmt, 0).c_str());
            lastArrival = std::max(lastArrival, arrival);
            if (first) {
                break;
            }
            std::wstring aumid = FromUtf8(Column(stmt, 2));
            if (!BannerAllowed(aumid)) {
                continue;
            }
            ToastInfo info = ToastFromPayload(Column(stmt, 1), aumid);
            if (info.title.empty() && info.text.empty()) {
                continue;
            }
            info.id = L"stored#" + std::to_wstring(arrival);
            info.arrival = arrival;
            result.push_back(info);
        }
        g_sqlite.finalize(stmt);
    }
    // A failed first read (database busy or missing) tries again next time; starting from 0 would make a card of
    // every toast in the database.
    if (!first || read) {
        lastArrival = std::max(lastArrival, 0LL);
    }
    return result;
}

// Which of the stored toasts (by ArrivalTime) the apps took back: an app removes its toast once handled in the app
// (an answered call). One query for all of them; nothing counts as gone when the database can't be read.
std::vector<long long> StoredToastsGone(const std::vector<long long>& arrivals) {
    if (arrivals.empty()) {
        return {};
    }
    std::string sql = "SELECT ArrivalTime FROM Notification WHERE ArrivalTime IN (";
    for (size_t i = 0; i < arrivals.size(); i++) {
        sql += (i ? "," : "") + std::to_string(arrivals[i]);
    }
    sql += ")";
    sqlite3_stmt* stmt = Prepare(sql);
    if (!stmt) {
        return {};
    }
    std::vector<long long> gone = arrivals;
    int rc;
    while ((rc = g_sqlite.step(stmt)) == SQLITE_ROW) {
        std::erase(gone, atoll(Column(stmt, 0).c_str()));
    }
    g_sqlite.finalize(stmt);
    return rc == SQLITE_DONE ? gone : std::vector<long long>();  // a read cut short (busy) proves nothing
}

// Texts as compared between the database, the banner and the notification center: without the direction marks some
// apps (Discord) put around names, and only the start of the message (the banner may cut or rewrap it).
std::wstring Plain(const std::wstring& s) {
    std::wstring r;
    for (wchar_t c : s) {
        if (c < 0x2066 || c > 0x2069) {
            r += iswspace(c) ? L' ' : c;
        }
    }
    size_t a = r.find_first_not_of(L' '), b = r.find_last_not_of(L' ');
    return a == std::wstring::npos ? std::wstring() : r.substr(a, b - a + 1);
}

bool SameToast(const ToastInfo& info, const std::wstring& title, const std::wstring& text) {
    if (Plain(info.title) != Plain(title)) {
        return false;
    }
    std::wstring a = Plain(info.text), b = Plain(text);
    size_t n = std::min({a.size(), b.size(), (size_t)24});
    return a.compare(0, n, b, 0, n) == 0;
}

enum class Action { Open, Dismiss, Button };

struct Command {
    ToastInfo info;  // the card's toast: found by id on the banner, by title and text in the notification center
    Action action;
    int index = 0;      // which button
    std::wstring reply;  // typed on the card: put into the toast's reply box before the button is pressed
};

std::mutex g_mutex;
std::vector<ToastInfo> g_snapshot;  // guarded by g_mutex
std::vector<Command> g_commands;    // guarded by g_mutex

HANDLE g_stopEvent = nullptr;
HANDLE g_wakeEvent = nullptr;  // set when the banner window shows or moves, or a card is clicked
HANDLE g_workerReady = nullptr;  // set once the worker can read toasts (UI Automation works)
HANDLE g_workerThread = nullptr;
HANDLE g_uiThread = nullptr;
std::atomic<HWND> g_window{nullptr};  // created by the card thread, posted to by the worker
// Cards don't run out under the mouse (the banner's own timer doesn't see the mouse on a card): until this tick
// (GetTickCount64), set by the card thread, the worker keeps every card. Max while hovering, a short grace after.
std::atomic<ULONGLONG> g_holdUntil{0};
constexpr ULONGLONG kHoverGraceMs = 2000;
std::atomic<DWORD> g_shellPid{0};  // ShellExperienceHost, whose window events wake the worker
HWINEVENTHOOK g_eventHook = nullptr;  // owned by the card thread

constexpr UINT WM_APP_SNAPSHOT = WM_APP + 1;
constexpr UINT WM_APP_SETTINGS = WM_APP + 2;
constexpr UINT WM_APP_REHOOK = WM_APP + 3;  // ShellExperienceHost restarted: watch the new process
constexpr UINT WM_APP_CENTER = WM_APP + 4;  // wParam 1: the mod opens the notification center (keep it off the screen)
// While a banner or a stored toast's card is up, the worker passes every kBannerMs (removed toasts, new stored ones,
// cards running out). Showing, hiding and resizing the banner wake the worker at once through window events. With
// nothing up it sleeps and only checks every kIdleMs in case something was missed.
constexpr DWORD kBannerMs = 500, kIdleMs = 2000;

// The banner window: the persistent one (any size) or only while it shows a toast.
HWND FindBanner(bool showing) {
    HWND hwnd = nullptr;
    while ((hwnd = FindWindowExW(HWND_DESKTOP, hwnd, L"Windows.UI.Core.CoreWindow", nullptr)) != nullptr) {
        if (!IsBannerWindow(hwnd)) {
            continue;
        }
        if (!showing) {
            return hwnd;
        }
        if (!IsWindowVisible(hwnd)) {
            continue;
        }
        // At rest the window shrinks to a sliver; a real banner is at least ~80 px tall.
        RECT rc;
        UINT dpi = GetDpiForWindow(hwnd);
        if (GetWindowRect(hwnd, &rc) && rc.bottom - rc.top >= MulDiv(80, dpi ? dpi : 96, 96)) {
            return hwnd;
        }
    }
    return nullptr;
}

HWND FindVisibleBanner() {
    return FindBanner(true);
}

DWORD BannerProcess() {
    DWORD pid = 0;
    if (HWND banner = FindBanner(false)) {
        GetWindowThreadProcessId(banner, &pid);
    }
    return pid;
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG, DWORD, DWORD) {
    if (idObject == OBJID_WINDOW && hwnd && IsBannerWindow(hwnd)) {
        SetEvent(g_wakeEvent);
    }
}

// Window shown, hidden or moved/resized in ShellExperienceHost (the banner grows from a sliver when a toast comes).
HWINEVENTHOOK HookBannerEvents() {
    DWORD pid = BannerProcess();
    g_shellPid = pid;
    if (!pid) {
        return nullptr;  // not running yet: the worker's idle check hooks it later
    }
    return SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_LOCATIONCHANGE, nullptr, WinEventProc, pid, 0,
                           WINEVENT_OUTOFCONTEXT);
}

bool Stopping() {
    return WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0;
}

// The cards belong to the shell's explorer, the one that owns the desktop window. Others (folder windows in their
// own process, "Launch folder windows in a separate process") come and go, so they don't draw. At logon the
// desktop window may not exist yet. False: another process is the shell, or the mod is unloading.
bool IsShellProcess() {
    for (;;) {
        if (HWND shell = GetShellWindow()) {
            DWORD pid = 0;
            GetWindowThreadProcessId(shell, &pid);
            return pid == GetCurrentProcessId();
        }
        if (WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) {
            return false;
        }
    }
}

template <typename T>
struct Com {
    T* p = nullptr;
    Com() = default;
    Com(const Com&) = delete;
    Com& operator=(const Com&) = delete;
    ~Com() {
        if (p) {
            p->Release();
        }
    }
    T** put() {
        if (p) {
            p->Release();
            p = nullptr;
        }
        return &p;
    }
    T* operator->() const { return p; }
    explicit operator bool() const { return p != nullptr; }
};

std::wstring RuntimeId(IUIAutomationElement* element) {
    SAFEARRAY* array = nullptr;
    std::wstring id;
    if (SUCCEEDED(element->GetRuntimeId(&array)) && array) {
        LONG lo = 0, hi = -1;
        SafeArrayGetLBound(array, 1, &lo);
        SafeArrayGetUBound(array, 1, &hi);
        for (LONG i = lo; i <= hi; i++) {
            int value = 0;
            SafeArrayGetElement(array, &i, &value);
            id += std::to_wstring(value) + L".";
        }
        SafeArrayDestroy(array);
    }
    return id;
}

void NewIdCondition(IUIAutomation* uia, const wchar_t* automationId, Com<IUIAutomationCondition>& condition) {
    VARIANT v{};
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocString(automationId);
    uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, v, condition.put());
    VariantClear(&v);
}

std::wstring ChildName(IUIAutomationElement* toast, IUIAutomationCondition* condition) {
    Com<IUIAutomationElement> child;
    std::wstring result;
    if (condition && SUCCEEDED(toast->FindFirst(TreeScope_Descendants, condition, child.put())) && child) {
        BSTR name = nullptr;
        if (SUCCEEDED(child->get_CurrentName(&name)) && name) {
            result = name;
            SysFreeString(name);
        }
    }
    return result;
}


void InvokeElement(IUIAutomationElement* element) {
    Com<IUIAutomationInvokePattern> invoke;
    if (SUCCEEDED(element->GetCurrentPatternAs(UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern),
                                               reinterpret_cast<void**>(invoke.put()))) &&
        invoke) {
        invoke->Invoke();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Notification center: a toast still waiting in the banner's queue has no banner to click, but it is already in the
// notification center, with the same buttons and reply box. The mod opens the center off the screen (Win+N; the
// ShellExperienceHost copy moves it away, see SetWindowPos_Hook), acts there and closes it.

void PressKeys(std::initializer_list<WORD> keys) {
    std::vector<INPUT> inputs;
    for (WORD key : keys) {
        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = key;
        inputs.push_back(input);
    }
    for (size_t i = keys.size(); i-- > 0;) {
        INPUT input = inputs[i];
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        inputs.push_back(input);
    }
    SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
}

void SetCenterHidden(bool hidden) {
    if (HWND window = g_window) {
        SendMessageTimeoutW(window, WM_APP_CENTER, hidden, 0, SMTO_ABORTIFHUNG, 500, nullptr);
    }
}

// The center takes the foreground while it is open.
HWND ForegroundCenter() {
    HWND hwnd = GetForegroundWindow();
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return hwnd && IsCoreWindowClass(hwnd) && !IsBannerTitle(hwnd) && ProcessIsShellExperienceHost(pid) ? hwnd
                                                                                                         : nullptr;
}

// Windows keeps the center where it last was and doesn't place it again when it opens, so the mod moves it back after
// using it. Where it belongs: its last on-screen spot, or the right edge of the taskbar's monitor.
int g_centerX = INT_MIN;

int CenterHomeX(HWND hwnd) {
    if (g_centerX != INT_MIN) {
        return g_centerX;
    }
    RECT rc{};
    GetWindowRect(hwnd, &rc);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfo(MonitorFromWindow(FindWindowW(L"Shell_TrayWnd", nullptr), MONITOR_DEFAULTTOPRIMARY), &mi);
    return mi.rcWork.right - (rc.right - rc.left);
}

// Puts the center back on the screen if it was left off it (also at load, after a crash or an old version).
void RestoreCenter(HWND hwnd) {
    RECT rc;
    if (hwnd && GetWindowRect(hwnd, &rc) && rc.left >= OffscreenX() - 200) {
        // Async: a hung ShellExperienceHost must not block the worker (and with it the mod's unload).
        SetWindowPos(hwnd, nullptr, CenterHomeX(hwnd), rc.top, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    }
}

// At load: a center left off the screen (Explorer crashed while the mod used it) goes back.
void RestoreLostCenter(DWORD shellPid) {
    HWND hwnd = nullptr;
    while ((hwnd = FindWindowExW(HWND_DESKTOP, hwnd, L"Windows.UI.Core.CoreWindow", nullptr)) != nullptr) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == shellPid && !IsBannerTitle(hwnd)) {
            RestoreCenter(hwnd);
        }
    }
}

HWND OpenCenter() {
    if (HWND hwnd = ForegroundCenter()) {
        return hwnd;
    }
    SetCenterHidden(true);
    PressKeys({VK_LWIN, 'N'});
    for (int i = 0; i < 75 && !Stopping(); i++) {
        if (HWND hwnd = ForegroundCenter()) {
            RECT rc;
            if (GetWindowRect(hwnd, &rc) && rc.left < OffscreenX() - 200) {
                g_centerX = rc.left;
                SetWindowPos(hwnd, nullptr, OffscreenX(), rc.top, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
            }
            return hwnd;
        }
        Sleep(20);
    }
    Log(L"The notification center didn't open");
    SetCenterHidden(false);
    RestoreLostCenter(BannerProcess());  // the hook may have moved it off the screen meanwhile
    return nullptr;
}

void CloseCenter(HWND hwnd) {
    // Opening an item closes the center by itself (and the app takes the foreground): Esc only for a center that is
    // still open after that, so it never reaches the app.
    for (int i = 0; i < 25 && ForegroundCenter() == hwnd; i++) {
        Sleep(20);
    }
    if (ForegroundCenter() == hwnd) {
        PressKeys({VK_ESCAPE});
        Sleep(300);  // closing animation, still off the screen
    }
    SetCenterHidden(false);
    RestoreCenter(hwnd);
}

DWORD WINAPI WorkerThread(LPVOID) {
    if (!IsShellProcess() || FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        return 0;
    }
    {
        Com<IUIAutomation> uia;
        if (FAILED(CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation),
                                    reinterpret_cast<void**>(uia.put())))) {
            Log(L"UI Automation unavailable");
            CoUninitialize();
            return 0;
        }
        // The banner's process can stall; without limits a call could hang and block unloading the mod.
        {
            Com<IUIAutomation2> uia2;
            if (SUCCEEDED(uia->QueryInterface(__uuidof(IUIAutomation2), reinterpret_cast<void**>(uia2.put())))) {
                uia2->put_ConnectionTimeout(1000);
                uia2->put_TransactionTimeout(1000);
            }
        }
        // Only now the card window may come (and with it the banner hiding): without a reader, banners stay.
        SetEvent(g_workerReady);
        auto classCondition = [&](const wchar_t* className, Com<IUIAutomationCondition>& condition) {
            VARIANT v{};
            v.vt = VT_BSTR;
            v.bstrVal = SysAllocString(className);
            uia->CreatePropertyCondition(UIA_ClassNamePropertyId, v, condition.put());
            VariantClear(&v);
        };
        Com<IUIAutomationCondition> toastCondition;
        classCondition(L"FlexibleToastView", toastCondition);
        // The notification center: a list item per toast under a header per app.
        Com<IUIAutomationCondition> itemCondition;
        classCondition(L"ListViewItem", itemCondition);
        Com<IUIAutomationCondition> headerCondition;
        classCondition(L"ListViewHeaderItem", headerCondition);
        Com<IUIAutomationCondition> contentCondition;
        NewIdCondition(uia.p, L"Content", contentCondition);
        Com<IUIAutomationCondition> moreCondition;
        NewIdCondition(uia.p, L"SeeMoreLessButton", moreCondition);
        Com<IUIAutomationCondition> expandCondition;
        NewIdCondition(uia.p, L"ExpandButton", expandCondition);
        Com<IUIAutomationCondition> senderCondition;
        NewIdCondition(uia.p, L"SenderName", senderCondition);
        Com<IUIAutomationCondition> titleCondition;
        NewIdCondition(uia.p, L"Title", titleCondition);
        Com<IUIAutomationCondition> textCondition;
        NewIdCondition(uia.p, L"MessageText", textCondition);
        Com<IUIAutomationCondition> dismissCondition;
        NewIdCondition(uia.p, L"DismissButton", dismissCondition);
        Com<IUIAutomationCondition> verbCondition;
        NewIdCondition(uia.p, L"VerbButton", verbCondition);
        Com<IUIAutomationCondition> editCondition;
        NewIdCondition(uia.p, L"Edit", editCondition);

        // Toasts on the banner. The element is needed to click, dismiss or type; runtimeId only saves re-reading
        // the texts while it stays the same.
        struct Known {
            ToastInfo info;
            std::wstring runtimeId;
            IUIAutomationElement* element;
            bool hidden = false;  // its stored card already came and went: the banner shows it late, no second card
            ULONGLONG until = 0;  // a stored toast's card time, kept when the banner takes it over (0: banner's time)
        };
        std::vector<Known> known;
        std::vector<ToastInfo> lastSent;

        // Toasts from the database that the banner hasn't shown yet. Their cards last as long as a banner would;
        // a toast the banner shows meanwhile continues the card and lives on with the banner.
        struct Pending {
            ToastInfo info;
            ULONGLONG until;
        };
        std::vector<Pending> pending;
        std::vector<std::pair<ToastInfo, ULONGLONG>> expired;  // card gone, banner not seen yet (and when)
        long long lastArrival = -1;
        NewStoredToasts(lastArrival);
        ULONGLONG lastDbRead = GetTickCount64();
        auto cardMs = [] {
            UINT seconds = 5;
            SystemParametersInfoW(SPI_GETMESSAGEDURATION, 0, &seconds, 0);
            return (ULONGLONG)seconds * 1000 + 1500;
        };
        // The database changes when a toast comes: that wakes the worker at once.
        HANDLE dbChange = FindFirstChangeNotificationW(NotificationFolder().c_str(), FALSE,
                                                       FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE);
        auto findChildren = [](IUIAutomationElement* toast, IUIAutomationCondition* condition) {
            std::vector<IUIAutomationElement*> result;
            Com<IUIAutomationElementArray> array;
            if (condition && SUCCEEDED(toast->FindAll(TreeScope_Descendants, condition, array.put())) && array) {
                int count = 0;
                array->get_Length(&count);
                for (int i = 0; i < count; i++) {
                    IUIAutomationElement* element = nullptr;
                    if (SUCCEEDED(array->GetElement(i, &element)) && element) {
                        result.push_back(element);
                    }
                }
            }
            return result;
        };
        auto release = [](std::vector<IUIAutomationElement*>& elements) {
            for (auto element : elements) {
                element->Release();
            }
        };

        // The toast's item in the open notification center (AddRef'd), or nullptr.
        auto centerItem = [&](IUIAutomationElement* root, const ToastInfo& info) {
            IUIAutomationElement* found = nullptr;
            auto items = findChildren(root, itemCondition.p);
            for (auto item : items) {
                if (!found && SameToast(info, ChildName(item, titleCondition.p), ChildName(item, contentCondition.p))) {
                    found = item;
                    found->AddRef();
                }
            }
            release(items);
            return found;
        };
        // Older toasts of an app are folded under its newest one ("Notifications (+3)").
        auto unfold = [&](IUIAutomationElement* root, const std::wstring& app) {
            auto headers = findChildren(root, headerCondition.p);
            for (auto header : headers) {
                if (Plain(ChildName(header, titleCondition.p)) == Plain(app)) {
                    auto more = findChildren(header, moreCondition.p);
                    for (auto button : more) {
                        InvokeElement(button);
                    }
                    release(more);
                }
            }
            release(headers);
        };
        // Presses a button of a toast (banner or notification center), first putting the reply typed on the card
        // into the toast's reply box.
        auto pressButton = [&](IUIAutomationElement* toast, const Command& command) {
            if (!command.reply.empty()) {
                auto edits = findChildren(toast, editCondition.p);
                Com<IUIAutomationValuePattern> value;
                bool filled = false;
                if (!edits.empty() &&
                    SUCCEEDED(edits[0]->GetCurrentPatternAs(UIA_ValuePatternId, __uuidof(IUIAutomationValuePattern),
                                                            reinterpret_cast<void**>(value.put()))) &&
                    value) {
                    BSTR text = SysAllocString(command.reply.c_str());
                    value->SetValue(text);
                    SysFreeString(text);
                    BSTR now = nullptr;
                    filled = SUCCEEDED(value->get_CurrentValue(&now)) && now && command.reply == now;
                    SysFreeString(now);
                }
                Log(L"Reply %s", filled ? L"put into the toast's reply box" : L"NOT put into the reply box");
                release(edits);
            }
            auto buttons = findChildren(toast, verbCondition.p);
            bool done = command.index < (int)buttons.size();
            if (done) {
                InvokeElement(buttons[command.index]);
            }
            release(buttons);
            return done;
        };
        // Does the card's action in the notification center. False if the toast wasn't found there.
        auto centerCommand = [&](const ToastInfo& info, const Command& command) {
            HWND hwnd = OpenCenter();
            if (!hwnd) {
                return false;
            }
            bool done = false;
            Com<IUIAutomationElement> root;
            if (SUCCEEDED(uia->ElementFromHandle(hwnd, root.put())) && root) {
                IUIAutomationElement* item = nullptr;
                // The list fills in a moment after opening.
                for (int attempt = 0; attempt < 8 && !item && !Stopping(); attempt++) {
                    if (attempt) {
                        Sleep(150);
                    }
                    if (attempt == 3) {
                        unfold(root.p, info.app);
                        Sleep(250);
                    }
                    item = centerItem(root.p, info);
                }
                if (item && command.action == Action::Open) {
                    InvokeElement(item);
                    done = true;
                } else if (item && command.action == Action::Dismiss) {
                    auto targets = findChildren(item, dismissCondition.p);
                    if (!targets.empty()) {
                        InvokeElement(targets[0]);
                        done = true;
                    }
                    release(targets);
                } else if (item) {
                    // Buttons and the reply box show only when the item is expanded, which builds it anew.
                    auto verbs = findChildren(item, verbCondition.p);
                    if (verbs.empty()) {
                        auto expand = findChildren(item, expandCondition.p);
                        if (!expand.empty()) {
                            InvokeElement(expand[0]);
                            Sleep(300);
                            if (IUIAutomationElement* fresh = centerItem(root.p, info)) {
                                item->Release();
                                item = fresh;
                            }
                        }
                        release(expand);
                    }
                    release(verbs);
                    done = pressButton(item, command);
                }
                if (item) {
                    item->Release();
                }
            }
            Log(L"Center %s %d: %s", done ? L"did" : L"didn't find", (int)command.action, info.title.c_str());
            CloseCenter(hwnd);
            return done;
        };

        bool busy = true;  // a banner was up on the last pass (or this is the first pass)
        bool centerChecked = false;
        HANDLE waits[] = {g_stopEvent, g_wakeEvent, dbChange};
        DWORD waitCount = dbChange != INVALID_HANDLE_VALUE ? 3 : 2;
        DWORD woke;
        while ((woke = WaitForMultipleObjects(waitCount, waits, FALSE,
                                              busy || !pending.empty() ? kBannerMs : kIdleMs)) != WAIT_OBJECT_0) {
            if (woke == WAIT_OBJECT_0 + 2) {
                FindNextChangeNotification(dbChange);
            }
            if (!centerChecked && g_shellPid) {
                RestoreLostCenter(g_shellPid);
                centerChecked = true;
            }
            if (!busy) {
                // ShellExperienceHost restarts (or starts late): its window events need a new hook.
                DWORD pid = BannerProcess();
                HWND window = g_window;
                if (pid && pid != g_shellPid && window) {
                    PostMessage(window, WM_APP_REHOOK, 0, 0);
                }
            }
            std::vector<Command> commands;
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                commands.swap(g_commands);
            }
            for (auto& command : commands) {
                if (Stopping()) {
                    break;
                }
                const std::wstring& id = command.info.id;
                auto onBanner = std::find_if(known.begin(), known.end(), [&](const Known& k) { return k.info.id == id; });
                if (onBanner != known.end()) {
                    onBanner->until = 0;  // handled: its card goes with the banner
                    if (command.action == Action::Button) {
                        pressButton(onBanner->element, command);
                    } else if (command.action == Action::Dismiss) {
                        auto targets = findChildren(onBanner->element, dismissCondition.p);
                        if (!targets.empty()) {
                            InvokeElement(targets[0]);
                        }
                        release(targets);
                    } else {
                        InvokeElement(onBanner->element);
                    }
                    continue;
                }
                // Not on the banner (still queued, or the banner already let it go): through the notification
                // center. × only closes the card, the toast stays there (as for banner toasts).
                if (command.action != Action::Dismiss) {
                    centerCommand(command.info, command);
                }
                auto waiting = std::find_if(pending.begin(), pending.end(),
                                            [&](const Pending& p) { return p.info.id == id; });
                if (waiting != pending.end()) {
                    expired.push_back({waiting->info, GetTickCount64()});
                    pending.erase(waiting);
                }
            }

            // New stored toasts (the same pass reads the banner after this, so a toast shown at once is matched).
            // The folder change notification comes late or not at all for the database's writes, so it is read on
            // every pass while toasts are on the screen (every kBannerMs), and otherwise when something woke the
            // worker (a banner showing, the folder changing) or every kIdleMs.
            ULONGLONG now = GetTickCount64();
            std::vector<ToastInfo> stored;
            if (woke != WAIT_TIMEOUT || busy || !pending.empty() || now - lastDbRead >= kIdleMs) {
                stored = NewStoredToasts(lastArrival);
                lastDbRead = now;
            }
            for (auto& info : stored) {
                bool shown = std::any_of(known.begin(), known.end(),
                                         [&](const Known& k) { return SameToast(info, k.info.title, k.info.text); });
                Log(L"Stored toast %s: %s (%zu chars)%s", info.app.c_str(), info.title.c_str(), info.text.size(),
                    shown ? L" (already on the banner)" : L"");
                if (!shown) {
                    pending.push_back({info, now + cardMs()});
                }
            }
            std::vector<long long> arrivals;
            for (auto& p : pending) {
                if (p.info.arrival) {
                    arrivals.push_back(p.info.arrival);
                }
            }
            std::vector<long long> taken = StoredToastsGone(arrivals);
            for (auto it = pending.begin(); it != pending.end();) {
                // The app took its toast back (a call answered or declined in the app, a message read): no card.
                if (it->info.arrival && std::find(taken.begin(), taken.end(), it->info.arrival) != taken.end()) {
                    Log(L"Stored toast removed by the app: %s", it->info.title.c_str());
                    it = pending.erase(it);
                    continue;
                }
                // Priority toasts (reminders, calls) wait for the banner, which keeps them until handled.
                if (now < it->until || now < g_holdUntil || (it->info.priority && now < it->until + 10 * 60 * 1000)) {
                    ++it;
                    continue;
                }
                expired.push_back({it->info, now});
                it = pending.erase(it);
            }
            std::erase_if(expired, [&](const auto& e) { return now - e.second > 2 * 60 * 1000; });

            std::vector<Known> current;
            HWND banner = FindVisibleBanner();
            busy = banner != nullptr;
            if (banner) {
                Com<IUIAutomationElement> root;
                Com<IUIAutomationElementArray> toasts;
                if (SUCCEEDED(uia->ElementFromHandle(banner, root.put())) && root &&
                    SUCCEEDED(root->FindAll(TreeScope_Descendants, toastCondition.p, toasts.put())) && toasts) {
                    int count = 0;
                    toasts->get_Length(&count);
                    // Each UIA call can take up to its timeout when the banner's process hangs, so unloading
                    // the mod doesn't wait for the whole pass.
                    for (int i = 0; i < count && !Stopping(); i++) {
                        IUIAutomationElement* element = nullptr;
                        if (FAILED(toasts->GetElement(i, &element)) || !element) {
                            continue;
                        }
                        std::wstring runtimeId = RuntimeId(element);
                        auto same = std::find_if(known.begin(), known.end(),
                                                 [&](const Known& k) { return k.runtimeId == runtimeId; });
                        ToastInfo info;
                        // An app can update a toast in place (Discord does for messages sent in a row): the
                        // element and its runtimeId stay, only the texts change, so they are compared every pass.
                        std::wstring title = ChildName(element, titleCondition.p);
                        std::wstring text = ChildName(element, textCondition.p);
                        bool hidden = false;
                        ULONGLONG until = 0;
                        if (same != known.end() && same->info.title == title && same->info.text == text) {
                            info = same->info;
                            hidden = same->hidden;
                            until = same->until;
                        } else {
                            info.app = same != known.end() ? same->info.app : ChildName(element, senderCondition.p);
                            info.title = title;
                            info.text = text;
                            if (Stopping()) {
                                element->Release();  // a new toast takes many UIA calls; don't make unloading wait
                                break;
                            }
                            std::wstring key = info.app + L"\n" + info.title + L"\n" + info.text + L"#";
                            int n = (int)std::count_if(current.begin(), current.end(), [&](const Known& k) {
                                return k.info.id.rfind(key, 0) == 0;
                            });
                            info.id = key + std::to_wstring(n);
                            // A stored toast reaching the banner: the card it already has goes on (or, if that card
                            // is gone, none comes back).
                            std::wstring image;
                            auto waiting = std::find_if(pending.begin(), pending.end(), [&](const Pending& p) {
                                return SameToast(p.info, title, text);
                            });
                            auto gone = std::find_if(expired.begin(), expired.end(), [&](const auto& e) {
                                return SameToast(e.first, title, text);
                            });
                            if (waiting != pending.end()) {
                                info.id = waiting->info.id;
                                info.arrival = waiting->info.arrival;
                                image = waiting->info.image;
                                info.aumid = waiting->info.aumid;
                                info.silent = waiting->info.silent;
                                info.sound = waiting->info.sound;
                                info.appSound = waiting->info.appSound;
                                until = waiting->until;
                                pending.erase(waiting);
                            } else if (gone != expired.end()) {
                                info.id = gone->first.id;
                                info.arrival = gone->first.arrival;
                                hidden = true;
                                expired.erase(gone);
                            }
                            auto old = std::find_if(known.begin(), known.end(),
                                                    [&](const Known& k) { return k.info.id == info.id; });
                            if (old != known.end()) {
                                info = old->info;
                                hidden = old->hidden;
                                until = old->until;
                            } else {
                                info.image = !image.empty() ? image : FindAvatar(info.title, info.text, &info);
                                if (info.image.empty()) {
                                    info.image = AppIcon(info.app);
                                }
                                BSTR automationId = nullptr;
                                element->get_CurrentAutomationId(&automationId);
                                info.priority = automationId && wcsncmp(automationId, L"PriorityToastView", 17) == 0;
                                SysFreeString(automationId);
                                auto buttons = findChildren(element, verbCondition.p);
                                for (auto button : buttons) {
                                    BSTR name = nullptr;
                                    button->get_CurrentName(&name);
                                    info.buttons.push_back(name ? name : L"");
                                    SysFreeString(name);
                                }
                                release(buttons);
                                auto edits = findChildren(element, editCondition.p);
                                info.hasInput = !edits.empty();
                                release(edits);
                                info.placeholder = ChildName(element, editCondition.p);
                                Log(L"Toast %s: %s (%zu chars) [%s] %d buttons%s%s", info.app.c_str(), info.title.c_str(),
                                    info.text.size(), info.image.c_str(), (int)info.buttons.size(),
                                    info.hasInput ? L", reply box" : L"",
                                    hidden                              ? L" (its card already went)"
                                    : info.id.rfind(L"stored#", 0) == 0 ? L" (continues its card)"
                                                                        : L"");
                            }
                        }
                        // Its card ends on time unless it waits to be handled (the card keeps itself while the user
                        // types in it).
                        if (until && GetTickCount64() > until && GetTickCount64() >= g_holdUntil && !info.priority) {
                            hidden = true;
                        }
                        current.push_back({info, runtimeId, element, hidden, until});
                    }
                }
            }
            for (auto& k : known) {
                // The banner drops a toast before its card's time is up when the notification center opens (the
                // mod's own Win+N too): the card stays, now acting through the center.
                bool kept = std::any_of(current.begin(), current.end(),
                                        [&](const Known& c) { return c.info.id == k.info.id; });
                // Also when its time ran out under the mouse: the card waits for the mouse to leave.
                ULONGLONG tick = GetTickCount64();
                if (!kept && !k.hidden && ((k.until && tick < k.until) || (tick < g_holdUntil && !k.info.priority))) {
                    pending.push_back({k.info, k.until});
                }
                k.element->Release();
            }
            known.swap(current);
            if (Stopping()) {
                break;  // the pass may be incomplete
            }

            std::vector<ToastInfo> infos;
            for (auto& k : known) {
                if (!k.hidden) {
                    infos.push_back(k.info);
                }
            }
            for (auto& p : pending) {
                infos.push_back(p.info);
            }
            bool changed = infos.size() != lastSent.size() ||
                           !std::equal(infos.begin(), infos.end(), lastSent.begin(),
                                       [](const ToastInfo& a, const ToastInfo& b) { return a.id == b.id; });
            if (changed) {
                {
                    std::lock_guard<std::mutex> lock(g_mutex);
                    g_snapshot = infos;
                }
                lastSent = infos;
                if (HWND window = g_window) {
                    PostMessage(window, WM_APP_SNAPSHOT, 0, 0);
                }
            }
        }
        for (auto& k : known) {
            k.element->Release();
        }
        if (dbChange != INVALID_HANDLE_VALUE) {
            FindCloseChangeNotification(dbChange);
        }
        CloseNotificationDb();
    }
    CoUninitialize();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Drawing: one layered, never-activated window in a bottom corner with a stack of cards:
// newest at the bottom, slides in from the right, fades out. Buttons and the reply box sit under the text.

constexpr float kPad = 8;  // room around the stack (shadow) and gap between cards
constexpr float kCardPadX = 12, kCardPadY = 10, kAvatar = 40, kGap = 10;
constexpr float kFontSize = 13, kLine = kFontSize * 1.35f;
constexpr float kFieldHeight = 28, kRowGap = 8, kFieldRadius = 7, kButtonGap = 6;
constexpr DWORD kInMs = 280, kOutMs = 400;
// Playful style: pop in from the corner, shrink away, glide to a new place, bounce a pressed button.
constexpr DWORD kPopInMs = 380, kPopOutMs = 260, kShiftMs = 420, kPressMs = 380;

// Mod settings (WindhawkModSettings). Sizes are in CSS pixels; "size" scales all of them.
enum class ThemeChoice { Auto, Dark, Light };
enum class AvatarShape { Circle, Rounded, Square };
constexpr UINT32 kNoColor = 0xFFFFFFFF;

struct AppRule {
    std::wstring app;
    bool mute = false;
    bool buttons = true, replyBox = true, avatar = true;
};

struct Settings {
    ThemeChoice theme = ThemeChoice::Auto;
    float zoom = 1.3f;
    float maxLines = 4;  // longer messages make the card taller up to this; the rest is cut with an ellipsis
    float width = 360;   // window width; cards are width - 2 * kPad wide
    float radius = 12;
    float opacity = 1;
    UINT32 cardColor = kNoColor, textColor = kNoColor;
    float margin = 0;
    bool showAvatars = true;
    AvatarShape avatarShape = AvatarShape::Circle;
    bool appName = false;
    DWORD inMs = kInMs, outMs = kOutMs;  // 0 without animation
    bool playful = true;                 // never with animation off
    DWORD popInMs = kPopInMs, popOutMs = kPopOutMs, shiftMs = kShiftMs, pressMs = kPressMs;
    int maxCards = 5;
    bool cursorMonitor = true;
    bool top = false, left = false;  // corner
    bool showActions = true;
    bool highlightPriority = true;
    bool copyCodes = true;
    bool copyInSystemLanguage = true;
    bool markdown = true;
    int soundMode = 0;  // 0 when the card shows, 1 as Windows plays it, 2 none
    std::wstring soundFile;
    UINT32 priorityColor = 0xF0A020;  // reminders, alarms, calls
    std::vector<AppRule> rules;
} g_settings;

// The sound mode for CreateFileW_Hook, which runs on ShellExperienceHost's threads while settings load on another.
std::atomic<int> g_soundMode{0};

std::wstring StringSetting(const wchar_t* name) {
    return WindhawkUtils::StringSetting(Wh_GetStringSetting(name)).get();
}

// "#RRGGBB" or "RRGGBB"; kNoColor when empty or invalid.
UINT32 ColorSetting(const wchar_t* name) {
    std::wstring color = StringSetting(name);
    if (!color.empty() && color[0] == L'#') {
        color.erase(0, 1);
    }
    if (color.size() == 6 && color.find_first_not_of(L"0123456789abcdefABCDEF") == std::wstring::npos) {
        return wcstoul(color.c_str(), nullptr, 16);
    }
    return kNoColor;
}

// The rule for an app, or the default one (everything on).
AppRule RuleFor(const std::wstring& app) {
    for (const AppRule& rule : g_settings.rules) {
        if (_wcsicmp(rule.app.c_str(), app.c_str()) == 0) {
            return rule;
        }
    }
    return {};
}

void LoadMutedForever();

void LoadSettings() {
    Settings s;
    std::wstring theme = StringSetting(L"appearance.theme");
    s.theme = theme == L"dark" ? ThemeChoice::Dark : theme == L"light" ? ThemeChoice::Light : ThemeChoice::Auto;
    if (int size = Wh_GetIntSetting(L"appearance.size")) {
        s.zoom = std::clamp(size, 80, 200) / 100.0f;
    }
    if (int lines = Wh_GetIntSetting(L"appearance.maxLines")) {
        s.maxLines = (float)std::clamp(lines, 1, 10);
    }
    s.cursorMonitor = StringSetting(L"position.monitor") != L"primary";
    std::wstring corner = StringSetting(L"position.corner");
    s.top = corner.rfind(L"top", 0) == 0;
    s.left = corner.find(L"Left") != std::wstring::npos;
    s.showActions = Wh_GetIntSetting(L"behavior.showActions") != 0;
    s.highlightPriority = Wh_GetIntSetting(L"behavior.highlightPriority") != 0;
    s.copyCodes = Wh_GetIntSetting(L"behavior.copyCodes") != 0;
    s.copyInSystemLanguage = Wh_GetIntSetting(L"behavior.copyInSystemLanguage") != 0;
    s.markdown = Wh_GetIntSetting(L"behavior.markdown") != 0;
    std::wstring soundMode = StringSetting(L"sound.mode");
    s.soundMode = soundMode == L"windows" ? 1 : soundMode == L"off" ? 2 : 0;
    g_soundMode = s.soundMode;
    s.soundFile = StringSetting(L"sound.file");
    if (s.soundFile.size() >= 2 && s.soundFile.front() == L'"' && s.soundFile.back() == L'"') {
        s.soundFile = s.soundFile.substr(1, s.soundFile.size() - 2);  // "Copy as path" adds quotes
    }
    if (UINT32 color = ColorSetting(L"behavior.priorityColor"); color != kNoColor) {
        s.priorityColor = color;
    }
    // A missing number reads as 0, so only values in range replace the defaults.
    int width = Wh_GetIntSetting(L"appearance.width");
    if (width) {
        s.width = (float)std::clamp(width, 280, 600);
    }
    s.radius = (float)std::clamp(Wh_GetIntSetting(L"appearance.cornerRadius"), 0, 24);
    if (int opacity = Wh_GetIntSetting(L"appearance.opacity")) {
        s.opacity = std::clamp(opacity, 30, 100) / 100.0f;
    }
    s.cardColor = ColorSetting(L"appearance.cardColor");
    s.textColor = ColorSetting(L"appearance.textColor");
    s.margin = (float)std::clamp(Wh_GetIntSetting(L"appearance.margin"), 0, 200);
    s.showAvatars = Wh_GetIntSetting(L"avatars.show") != 0;
    std::wstring shape = StringSetting(L"avatars.shape");
    s.avatarShape = shape == L"rounded" ? AvatarShape::Rounded : shape == L"square" ? AvatarShape::Square
                                                                                   : AvatarShape::Circle;
    s.appName = Wh_GetIntSetting(L"avatars.appName") != 0;
    if (!Wh_GetIntSetting(L"animation.enabled")) {
        s.inMs = s.outMs = 0;
        s.playful = false;
    } else {
        s.playful = Wh_GetIntSetting(L"animation.playful") != 0;
        if (int speed = Wh_GetIntSetting(L"animation.speed")) {
            speed = std::clamp(speed, 25, 400);
            s.inMs = kInMs * 100 / speed;
            s.outMs = kOutMs * 100 / speed;
            s.popInMs = kPopInMs * 100 / speed;
            s.popOutMs = kPopOutMs * 100 / speed;
            s.shiftMs = kShiftMs * 100 / speed;
            s.pressMs = kPressMs * 100 / speed;
        }
    }
    if (int maxCards = Wh_GetIntSetting(L"animation.maxCards")) {
        s.maxCards = std::clamp(maxCards, 1, 10);
    }
    for (int i = 0; i < 100; i++) {
        AppRule rule;
        rule.app = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"appRules[%d].app", i)).get();
        if (rule.app.empty()) {
            break;
        }
        rule.mute = std::wstring(WindhawkUtils::StringSetting(Wh_GetStringSetting(L"appRules[%d].show", i)).get()) ==
                    L"mute";
        rule.buttons = Wh_GetIntSetting(L"appRules[%d].buttons", i) != 0;
        rule.replyBox = Wh_GetIntSetting(L"appRules[%d].replyBox", i) != 0;
        rule.avatar = Wh_GetIntSetting(L"appRules[%d].avatar", i) != 0;
        s.rules.push_back(rule);
    }
    g_settings = s;
    if (g_explorer) {  // only the shell's Explorer draws cards; the other process needs no list
        LoadMutedForever();
    }
    Log(L"Settings: theme %d, size %d%%, %d lines, width %d, %s monitor, corner %s%s, actions %d, priority %d #%06X, "
        L"%d rules",
        (int)s.theme, (int)(s.zoom * 100 + 0.5f), (int)s.maxLines, (int)s.width,
        s.cursorMonitor ? L"cursor" : L"primary", s.top ? L"top" : L"bottom", s.left ? L"Left" : L"Right",
        s.showActions, s.highlightPriority, s.priorityColor, (int)s.rules.size());
}

// Black cards in dark mode, white ones in light mode.
struct Theme {
    UINT32 card, cardHover, border, ink, field, fieldHover;
    float borderAlpha;
};
constexpr Theme kDark{0x000000, 0x121212, 0x3a3b3c, 0xe4e6eb, 0x1c1c1d, 0x2a2b2c, 0.9f};
constexpr Theme kLight{0xffffff, 0xf2f2f2, 0x808080, 0x050505, 0xf0f2f5, 0xe4e6eb, 0.25f};

// a + (b - a) * t per channel.
UINT32 Mix(UINT32 a, UINT32 b, float t) {
    UINT32 result = 0;
    for (int shift = 0; shift <= 16; shift += 8) {
        float x = (float)((a >> shift) & 0xff), y = (float)((b >> shift) & 0xff);
        result |= (UINT32)(x + (y - x) * t + 0.5f) << shift;
    }
    return result;
}

// The theme with the colors from the settings; hover and field shades are derived from the card color.
Theme CustomTheme(Theme theme) {
    if (g_settings.textColor != kNoColor) {
        theme.ink = g_settings.textColor;
    }
    if (g_settings.cardColor != kNoColor) {
        theme.card = g_settings.cardColor;
        theme.cardHover = Mix(theme.card, theme.ink, 0.07f);
        theme.field = Mix(theme.card, theme.ink, 0.11f);
        theme.fieldHover = Mix(theme.card, theme.ink, 0.17f);
    }
    return theme;
}

bool SystemUsesLightTheme() {
    DWORD value = 0, size = sizeof(value);
    return RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                        L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS &&
           value != 0;
}

Theme CurrentTheme() {
    bool light = g_settings.theme == ThemeChoice::Light ||
                 (g_settings.theme == ThemeChoice::Auto && SystemUsesLightTheme());
    return CustomTheme(light ? kLight : kDark);
}

enum class Part { None, Body, Close, Mute, MuteChoice, Button, Input };

// The moon button's list of times (an in-card list: a popup menu in explorer.exe comes up empty).
struct MuteTime {
    const wchar_t* label;
    ULONGLONG minutes;
};
constexpr ULONGLONG kMuteForever = 0;  // minutes of the last choice (∞): kept in the mod's storage
constexpr MuteTime kMuteTimes[] = {
    {L"15 min", 15}, {L"1 h", 60}, {L"8 h", 8 * 60}, {L"24 h", 24 * 60}, {L"∞", kMuteForever}};
constexpr int kMuteTimeCount = (int)(sizeof(kMuteTimes) / sizeof(kMuteTimes[0]));
constexpr float kMuteLabelLeft = 30, kMuteLabelWidth = 64;

// Apps muted from a card's moon button for a while, until a tick count (GetTickCount64). Only in memory: a restart
// clears it.
std::vector<std::pair<std::wstring, ULONGLONG>> g_muted;
// Apps muted for good (∞), saved in the mod's storage as one value per app (mutedForever0, 1, ...) and a count: the
// portable version keeps storage in an INI file, which can't hold line breaks. Flipping the "unmuteAll" setting
// (either way) empties the list: its last seen value is stored too, and LoadSettings compares them.
std::vector<std::wstring> g_mutedForever;
constexpr wchar_t kMutedForeverCountKey[] = L"mutedForeverCount";
constexpr wchar_t kUnmuteSeenKey[] = L"unmuteAllSeen";

void SaveMutedForever() {
    int count = 0;
    for (const std::wstring& app : g_mutedForever) {
        WCHAR name[32];
        swprintf_s(name, L"mutedForever%d", count++);
        Wh_SetStringValue(name, app.c_str());
    }
    Wh_SetIntValue(kMutedForeverCountKey, count);
}

void LoadMutedForever() {
    g_mutedForever.clear();
    int unmuteAll = Wh_GetIntSetting(L"behavior.unmuteAll") != 0;
    if (Wh_GetIntValue(kUnmuteSeenKey, 0) != unmuteAll) {
        Wh_SetIntValue(kUnmuteSeenKey, unmuteAll);
        Wh_SetIntValue(kMutedForeverCountKey, 0);
        Log(L"Unmuted all apps muted for good");
        return;
    }
    int count = Wh_GetIntValue(kMutedForeverCountKey, 0);
    for (int i = 0; i < count; i++) {
        WCHAR name[32], app[512] = L"";
        swprintf_s(name, L"mutedForever%d", i);
        if (Wh_GetStringValue(name, app, ARRAYSIZE(app)) && *app) {
            g_mutedForever.push_back(app);
        }
    }
}

bool AppMuted(const std::wstring& app) {
    auto same = [&](const std::wstring& other) { return _wcsicmp(other.c_str(), app.c_str()) == 0; };
    if (std::any_of(g_mutedForever.begin(), g_mutedForever.end(), same)) {
        return true;
    }
    ULONGLONG now = GetTickCount64();
    std::erase_if(g_muted, [&](const auto& m) { return m.second <= now; });
    return std::any_of(g_muted.begin(), g_muted.end(), [&](const auto& m) { return same(m.first); });
}

bool PolishUI() {
    return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_POLISH;
}

struct Hit {
    int card = -1;
    Part part = Part::None;
    int index = 0;
    bool operator==(const Hit&) const = default;
};

struct Card {
    ToastInfo info;
    IDWriteTextLayout* title = nullptr;
    IDWriteTextLayout* text = nullptr;
    ID2D1Bitmap* avatar = nullptr;
    bool showAvatar = true;   // the avatars setting and the app's rule
    bool waitsForUser = false;  // a reminder, alarm or call (with or without the highlight setting)
    float contentHeight = 0;  // avatar and texts, without buttons and the reply box
    float height = 0;
    ULONGLONG shownAt = 0;
    ULONGLONG goneAt = 0;  // 0 while the system banner still has the toast
    float top = 0;         // last drawn position, for hit testing
    std::wstring code;     // one-time code found in the text, copied by the mod's own last button
    int copyButton = -1;   // its index in info.buttons (after the app's buttons), -1 if none
    ULONGLONG copiedAt = 0;  // the copy button was pressed: it reads "Copied" for a moment, then the card goes
    // Playful style: distance of the card's top from the corner's edge (glides between edgeFrom and edgeTo).
    float edgeFrom = -1, edgeTo = -1;
    ULONGLONG shiftAt = 0;
    int pressedButton = -1;  // bounces for a moment after a press
    ULONGLONG pressedAt = 0;
    bool muteOpen = false;  // the moon button's list of times covers the card
    ULONGLONG mutedAt = 0;  // a time was picked: the card says until when for a moment, then goes
    std::wstring mutedNote;
};

constexpr ULONGLONG kMutedMs = 1800;

// A time in the open mute list, in card coordinates: one row, between the label and the close button.
D2D1_RECT_F MuteChoiceRect(const Card& card, int index, float cardWidth) {
    float left = kMuteLabelLeft + kMuteLabelWidth, right = cardWidth - 30;
    float width = (right - left - kButtonGap * (kMuteTimeCount - 1)) / kMuteTimeCount;
    float x = left + index * (width + kButtonGap), y = card.height / 2 - kFieldHeight / 2;
    return {x, y, x + width, y + kFieldHeight};
}

constexpr ULONGLONG kCopiedMs = 900;

std::vector<Card> g_cards;
std::vector<std::wstring> g_skipped;  // ids of toasts shown without a card (see ApplySnapshot)
Hit g_hover;
Theme g_theme = kDark;
HMONITOR g_monitor = nullptr;  // where the current stack is shown

ID2D1Factory* g_d2d = nullptr;
IDWriteFactory* g_dwrite = nullptr;
IWICImagingFactory* g_wic = nullptr;
IDWriteTextFormat* g_titleFormat = nullptr;
IDWriteTextFormat* g_textFormat = nullptr;
IDWriteTextFormat* g_letterFormat = nullptr;
IDWriteTextFormat* g_buttonFormat = nullptr;  // centered, one line
IDWriteTextFormat* g_iconFormat = nullptr;    // Segoe Fluent Icons (MDL2 Assets on older systems), centered
IDWriteTextFormat* g_bigIconFormat = nullptr; // the same, avatar-sized
IDWriteTextFormat* g_fieldFormat = nullptr;   // reply box: left, vertically centered, one line
ID2D1DCRenderTarget* g_target = nullptr;
HDC g_memDC = nullptr;
HBITMAP g_bitmap = nullptr;
SIZE g_bitmapSize{};

HMONITOR Monitor() {
    return g_monitor ? g_monitor : MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
}

float Scale() {
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(Monitor(), MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    return dpiX / 96.0f * g_settings.zoom;
}

RECT WorkArea() {
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfo(Monitor(), &mi);
    return mi.rcWork;
}

// Milliseconds for animations. GetTickCount64 only moves every ~15.6 ms, so on a 144 Hz screen several frames in a
// row would draw the same step.
ULONGLONG NowMs() {
    static LARGE_INTEGER frequency = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (ULONGLONG)(counter.QuadPart * 1000 / frequency.QuadPart);
}

// How far an animation of ms milliseconds is after elapsed ones, 0 to 1 (done at once when ms is 0).
float Progress(ULONGLONG elapsed, DWORD ms) {
    return ms ? std::clamp(elapsed / (float)ms, 0.0f, 1.0f) : 1.0f;
}

float EaseOut(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return 1 - (1 - t) * (1 - t) * (1 - t);
}

// Ease-out that overshoots a little before settling.
float EaseOutBack(float t) {
    t = std::clamp(t, 0.0f, 1.0f) - 1;
    const float c = 1.2f;
    return 1 + (c + 1) * t * t * t + c * t * t;
}

// A value through keyframes {time 0..1, value}, smoothed within each segment.
float Keyframes(float t, std::initializer_list<std::pair<float, float>> keys) {
    auto prev = *keys.begin();
    for (auto key : keys) {
        if (t <= key.first) {
            if (key.first <= prev.first) {
                return key.second;
            }
            float u = (t - prev.first) / (key.first - prev.first);
            u = u * u * (3 - 2 * u);
            return prev.second + (key.second - prev.second) * u;
        }
        prev = key;
    }
    return prev.second;
}

// Playful animations: the setting, unless Windows animation effects are off (checked at most once a second).
bool Playful() {
    static ULONGLONG checkedAt = 0;
    static BOOL systemOn = TRUE;
    ULONGLONG now = NowMs();
    if (!checkedAt || now - checkedAt > 1000) {
        checkedAt = now;
        if (!SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &systemOn, 0)) {
            systemOn = TRUE;
        }
    }
    return g_settings.playful && systemOn;
}

DWORD OutMs() {
    return Playful() ? g_settings.popOutMs : g_settings.outMs;
}

void ReleaseCard(Card& card) {
    for (IUnknown* object : std::initializer_list<IUnknown*>{card.title, card.text, card.avatar}) {
        if (object) {
            object->Release();
        }
    }
}

// Decodes an image file into a bitmap for the render target (premultiplied BGRA, as Direct2D wants it).
ID2D1Bitmap* LoadBitmap(const std::wstring& path) {
    if (path.empty() || !g_wic) {
        return nullptr;
    }
    ID2D1Bitmap* bitmap = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    IWICFormatConverter* converter = nullptr;
    if (SUCCEEDED(g_wic->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad,
                                                   &decoder)) &&
        SUCCEEDED(decoder->GetFrame(0, &frame))) {
        // Only the avatar's size is kept (a screenshot would take tens of MB for a 40 px circle): the shorter side
        // is scaled down to the avatar's size in screen pixels, as the image fills the avatar (cover).
        IWICBitmapSource* source = frame;
        UINT width = 0, height = 0;
        UINT side = (UINT)(kAvatar * Scale() + 1);
        if (SUCCEEDED(frame->GetSize(&width, &height)) && width && height && std::min(width, height) > side &&
            SUCCEEDED(g_wic->CreateBitmapScaler(&scaler))) {
            double fit = (double)side / std::min(width, height);
            UINT w = std::max(1u, (UINT)(width * fit + 0.5)), h = std::max(1u, (UINT)(height * fit + 0.5));
            if (SUCCEEDED(scaler->Initialize(frame, w, h, WICBitmapInterpolationModeFant))) {
                source = scaler;
            }
        }
        if (SUCCEEDED(g_wic->CreateFormatConverter(&converter)) &&
            SUCCEEDED(converter->Initialize(source, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0,
                                            WICBitmapPaletteTypeMedianCut))) {
            g_target->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap);
        }
    }
    if (!bitmap) {
        Log(L"Loading the avatar failed: %s", path.c_str());
    }
    for (IUnknown* object : std::initializer_list<IUnknown*>{converter, scaler, frame, decoder}) {
        if (object) {
            object->Release();
        }
    }
    return bitmap;
}

// Room the avatar takes left of the texts.
float AvatarSpace(const Card& card) {
    return card.showAvatar ? kAvatar + kGap : 0;
}

// Simple Markdown as chat apps (Slack, Discord, Teams) leave it in toasts: **bold**, __underline__ (as Discord
// shows it), *italic*, ~~strike~~, `code`. Marks are removed; each pair becomes a styled range. An unpaired mark stays
// as text. The "Format chat text" setting turns it off.
struct MarkRange {
    UINT32 start, length;
    int style;  // 0 bold, 1 italic, 2 strike, 3 code, 4 underline
};

std::wstring StripMarkdown(const std::wstring& in, std::vector<MarkRange>& ranges) {
    static const struct { const wchar_t* mark; int style; } kMarks[] = {
        {L"**", 0}, {L"__", 4}, {L"~~", 2}, {L"`", 3}, {L"*", 1}};
    std::wstring out;
    size_t i = 0;
    while (i < in.size()) {
        bool matched = false;
        for (auto& m : kMarks) {
            size_t len = wcslen(m.mark);
            if (in.compare(i, len, m.mark) != 0) continue;
            size_t close = in.find(m.mark, i + len);
            // "***" closing "**...*x***": the inner mark closes first, the outer one is the last two.
            while (close != std::wstring::npos && close + len < in.size() && in[close + len] == m.mark[0]) close++;
            // Content must be non-empty and not start with a space ("2 * 3 * 4" stays as is).
            if (close == std::wstring::npos || close == i + len || iswspace(in[i + len]) ||
                iswspace(in[close - 1])) {
                continue;
            }
            std::vector<MarkRange> inner;
            std::wstring content = m.style == 3 ? in.substr(i + len, close - i - len)
                                                : StripMarkdown(in.substr(i + len, close - i - len), inner);
            UINT32 base = (UINT32)out.size();
            for (auto& r : inner) ranges.push_back({base + r.start, r.length, r.style});
            ranges.push_back({base, (UINT32)content.size(), m.style});
            out += content;
            i = close + len;
            matched = true;
            break;
        }
        if (!matched) out += in[i++];
    }
    return out;
}

void LayoutCard(Card& card) {
    float textWidth = g_settings.width - 2 * kPad - 2 * kCardPadX - AvatarSpace(card);
    std::wstring title = card.info.title.empty() ? card.info.app : card.info.title;
    if (g_settings.appName && !card.info.app.empty() && title != card.info.app) {
        title = card.info.app + L" · " + title;
    }
    g_dwrite->CreateTextLayout(title.c_str(), (UINT32)title.size(), g_titleFormat, textWidth, kLine, &card.title);
    std::vector<MarkRange> ranges;
    std::wstring text = g_settings.markdown ? StripMarkdown(card.info.text, ranges) : card.info.text;
    // Keep one-letter words and numbers with the next word ("za 1 h", "w domu"): a no-break space after them.
    for (size_t i = 1; i < text.size(); i++) {
        if (text[i] != L' ') continue;
        size_t start = i;
        while (start > 0 && !iswspace(text[start - 1])) start--;
        bool number = start < i && std::all_of(text.begin() + start, text.begin() + i, iswdigit);
        if (i - start == 1 || number) text[i] = 0x00A0;
    }
    g_dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(), g_textFormat, textWidth,
                               g_settings.maxLines * kLine, &card.text);
    if (card.text) {
        for (auto& r : ranges) {
            DWRITE_TEXT_RANGE range{r.start, r.length};
            if (r.style == 0) card.text->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, range);
            if (r.style == 1) card.text->SetFontStyle(DWRITE_FONT_STYLE_ITALIC, range);
            if (r.style == 2) card.text->SetStrikethrough(TRUE, range);
            if (r.style == 3) card.text->SetFontFamilyName(L"Consolas", range);
            if (r.style == 4) card.text->SetUnderline(TRUE, range);
        }
    }
    DWRITE_TEXT_METRICS metrics{};
    float textHeight = 0;
    if (card.text && SUCCEEDED(card.text->GetMetrics(&metrics))) {
        textHeight = std::min(metrics.height, g_settings.maxLines * kLine);
    }
    card.contentHeight = std::max(card.showAvatar ? kAvatar : 0, kLine + textHeight);
    card.height = card.contentHeight + 2 * kCardPadY;
    if (card.info.hasInput) {
        card.height += kRowGap + kFieldHeight;
    }
    if (!card.info.buttons.empty()) {
        card.height += kRowGap + kFieldHeight;
    }
    card.avatar = card.showAvatar ? LoadBitmap(card.info.image) : nullptr;
}

// Rows under the text, in card coordinates (left edge 0): the reply box, then the buttons side by side.
D2D1_RECT_F InputRect(const Card& card) {
    float top = kCardPadY + card.contentHeight + kRowGap;
    return {kCardPadX, top, g_settings.width - 2 * kPad - kCardPadX, top + kFieldHeight};
}

D2D1_RECT_F ButtonRect(const Card& card, int index) {
    float top = kCardPadY + card.contentHeight + kRowGap + (card.info.hasInput ? kFieldHeight + kRowGap : 0);
    int count = (int)card.info.buttons.size();
    float width = (g_settings.width - 2 * kPad - 2 * kCardPadX - (count - 1) * kButtonGap) / count;
    float left = kCardPadX + index * (width + kButtonGap);
    return {left, top, left + width, top + kFieldHeight};
}

// A one-time code in a notification: 4-8 digits (groups like "123 456" or "123-456" are joined), only when the
// texts mention a code, so phone numbers, amounts and dates in ordinary messages don't count. The digits nearest
// after the keyword win.
std::wstring FindCode(const std::wstring& title, const std::wstring& body) {
    std::wstring all = title + L"\n" + body;
    std::wstring lower = all;
    for (auto& c : lower) {
        c = towlower(c);
    }
    static const wchar_t* keywords[] = {L"code", L"kod", L"otp", L"pin", L"passcode", L"password", L"hasło",
                                        L"haslo", L"verif", L"weryfik", L"blik", L"2fa", L"token", L"código", L"sign in", L"sign-in",
                                        L"log in", L"login", L"security", L"logowa", L"zaloguj"};
    size_t keyword = std::wstring::npos;
    for (auto k : keywords) {
        keyword = std::min(keyword, lower.find(k));
    }
    if (keyword == std::wstring::npos) {
        return L"";
    }
    auto digit = [&](size_t i) { return i < all.size() && all[i] >= L'0' && all[i] <= L'9'; };
    auto word = [&](size_t i) { return i < all.size() && (iswalnum(all[i]) || all[i] == L'%'); };
    std::vector<std::pair<size_t, std::wstring>> found;
    size_t i = 0;
    while (i < all.size()) {
        // A number starts after a non-word character that isn't part of another number (+48, 12.50, 1,5).
        bool starts = digit(i) && (i == 0 || !(word(i - 1) || all[i - 1] == L'+' ||
                                               ((all[i - 1] == L'.' || all[i - 1] == L',') && digit(i - 2))));
        if (!starts) {
            i++;
            continue;
        }
        size_t start = i;
        std::wstring digits;
        while (true) {
            size_t group = i;
            while (digit(i)) {
                digits += all[i++];
            }
            // Groups of three are joined: "123 456", "123-456".
            if (i - group == 3 && (all[i] == L' ' || all[i] == L'-') && digit(i + 1) && digit(i + 3) && !digit(i + 4)) {
                i++;
                continue;
            }
            break;
        }
        // Ends at a non-word character, not in the middle of 12.50, 10:30 or 25%.
        bool ends = !word(i) && !((all[i] == L'.' || all[i] == L',' || all[i] == L':') && digit(i + 1));
        if (ends && digits.size() >= 4 && digits.size() <= 8) {
            found.push_back({start, digits});
        }
    }
    for (auto& [at, digits] : found) {
        if (at > keyword) return digits;
    }
    return found.size() == 1 ? found[0].second : L"";
}

// "Copy" in the Windows display language: the Copy item of the standard edit-box context menu
// (user32.dll menu 1, item id WM_COPY), without the '&' and the "\tCtrl+C". English if not found.
const std::wstring& SystemCopyWord() {
    static const std::wstring word = [] {
        std::wstring result = L"Copy";
        if (HMENU menu = LoadMenuW(GetModuleHandleW(L"user32.dll"), MAKEINTRESOURCEW(1))) {
            wchar_t buf[64] = {};
            HMENU sub = GetSubMenu(menu, 0);
            if (sub && GetMenuStringW(sub, WM_COPY, buf, ARRAYSIZE(buf), MF_BYCOMMAND) > 0) {
                std::wstring text;
                for (auto c : std::wstring(buf)) {
                    if (c == L'\t') break;
                    if (c != L'&') text += c;
                }
                while (!text.empty() && iswspace(text.back())) text.pop_back();
                if (!text.empty()) result = text;
            }
            DestroyMenu(menu);
        }
        return result;
    }();
    return word;
}

std::wstring CopyWord() {
    return g_settings.copyInSystemLanguage ? SystemCopyWord() : L"Copy";
}

// The app's own copy button (Phone Link has one for codes): then the mod adds none.
bool HasCopyButton(const std::vector<std::wstring>& buttons, const std::wstring& code) {
    for (auto name : buttons) {
        for (auto& c : name) c = towlower(c);
        std::wstring digits;
        for (auto c : name) if (iswdigit(c)) digits += c;
        std::wstring word = SystemCopyWord();
        for (auto& c : word) c = towlower(c);
        if (name.find(L"cop") != std::wstring::npos || name.find(L"kopi") != std::wstring::npos ||
            name.find(word) != std::wstring::npos ||
            (!code.empty() && digits.find(code) != std::wstring::npos)) {
            return true;
        }
    }
    return false;
}

bool CopyText(HWND hwnd, const std::wstring& text) {
    if (!OpenClipboard(hwnd)) return false;
    EmptyClipboard();
    bool ok = false;
    if (HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(wchar_t))) {
        memcpy(GlobalLock(memory), text.c_str(), (text.size() + 1) * sizeof(wchar_t));
        GlobalUnlock(memory);
        ok = SetClipboardData(CF_UNICODETEXT, memory) != nullptr;
        if (!ok) GlobalFree(memory);
    }
    CloseClipboard();
    return ok;
}

void ApplySnapshot() {
    std::vector<ToastInfo> snapshot;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        snapshot = g_snapshot;
    }
    ULONGLONG now = NowMs();
    for (auto& card : g_cards) {
        // A card being typed into stays until the typing ends (EndTyping applies the snapshot again).
        // A just-muted card keeps its note for kMutedMs (Render closes it).
        bool live = card.info.inputFocused || card.mutedAt ||
                    std::any_of(snapshot.begin(), snapshot.end(), [&](const ToastInfo& t) { return t.id == card.info.id; });
        if (!live && !card.goneAt) {
            card.goneAt = now;
        }
    }
    // Toasts that get no card (muted apps, cards pushed out by "Most cards at once") are remembered while the
    // banner still has them, so they don't come back on the next snapshot.
    std::erase_if(g_skipped, [&](const std::wstring& id) {
        return std::none_of(snapshot.begin(), snapshot.end(), [&](const ToastInfo& t) { return t.id == id; });
    });
    bool played = false;
    for (auto& info : snapshot) {
        bool have = std::any_of(g_cards.begin(), g_cards.end(), [&](const Card& c) { return c.info.id == info.id; });
        if (have || std::find(g_skipped.begin(), g_skipped.end(), info.id) != g_skipped.end()) {
            continue;
        }
        AppRule rule = RuleFor(info.app);
        if (rule.mute || AppMuted(info.app)) {
            g_skipped.push_back(info.id);
            continue;
        }
        // A new stack follows the cursor's monitor and the current theme; a growing one stays where it is.
        if (std::all_of(g_cards.begin(), g_cards.end(), [](const Card& c) { return c.goneAt; })) {
            POINT cursor{};
            GetCursorPos(&cursor);
            g_monitor = g_settings.cursorMonitor ? MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY) : nullptr;
            g_theme = CurrentTheme();
        }
        Card card;
        card.info = info;
        if (!g_settings.showActions || !rule.buttons) {
            card.info.buttons.clear();
        }
        if (!g_settings.showActions || !rule.replyBox) {
            card.info.hasInput = false;
        }
        if (g_settings.showActions && rule.buttons && g_settings.copyCodes) {
            std::wstring code = FindCode(info.title, info.text);
            if (!code.empty() && !HasCopyButton(card.info.buttons, code)) {
                card.code = code;
                card.copyButton = (int)card.info.buttons.size();
                card.info.buttons.push_back(CopyWord() + L" " + code);
                Log(L"Code found: copy button");
            }
        }
        card.showAvatar = g_settings.showAvatars && rule.avatar;
        card.waitsForUser = card.info.priority;
        card.info.priority = card.info.priority && g_settings.highlightPriority;  // drawn with the outline
        card.shownAt = now;
        LayoutCard(card);
        g_cards.push_back(card);
        if (!played && g_settings.soundMode == 0) {
            played = PlayCardSound(info, g_settings.soundFile);  // one sound for cards that come together
        }
    }
    // Over the limit, the oldest cards still showing fade out. Not reminders, alarms and calls: their banner is hidden
    // too, so they would vanish while still waiting to be handled.
    int showing = (int)std::count_if(g_cards.begin(), g_cards.end(), [](const Card& c) { return !c.goneAt; });
    for (auto& card : g_cards) {
        if (showing <= g_settings.maxCards) {
            break;
        }
        if (!card.goneAt && !card.waitsForUser) {
            card.goneAt = now;
            g_skipped.push_back(card.info.id);
            showing--;
        }
    }
}

ID2D1SolidColorBrush* Brush(UINT32 rgb, float alpha) {
    ID2D1SolidColorBrush* brush = nullptr;
    D2D1_COLOR_F color{((rgb >> 16) & 0xff) / 255.0f, ((rgb >> 8) & 0xff) / 255.0f, (rgb & 0xff) / 255.0f, alpha};
    g_target->CreateSolidColorBrush(&color, nullptr, &brush);
    return brush;
}

void FillRounded(D2D1_RECT_F rect, float radius, UINT32 rgb, float alpha) {
    if (ID2D1SolidColorBrush* brush = Brush(rgb, alpha)) {
        D2D1_ROUNDED_RECT rounded{rect, radius, radius};
        g_target->FillRoundedRectangle(&rounded, brush);
        brush->Release();
    }
}

void StrokeRounded(D2D1_RECT_F rect, float radius, UINT32 rgb, float alpha, float width) {
    if (ID2D1SolidColorBrush* brush = Brush(rgb, alpha)) {
        D2D1_RECT_F inner{rect.left + width / 2, rect.top + width / 2, rect.right - width / 2, rect.bottom - width / 2};
        D2D1_ROUNDED_RECT rounded{inner, radius, radius};
        g_target->DrawRoundedRectangle(&rounded, brush, width);
        brush->Release();
    }
}

void DrawString(const std::wstring& text, IDWriteTextFormat* format, D2D1_RECT_F box, UINT32 rgb, float alpha) {
    if (ID2D1SolidColorBrush* ink = Brush(rgb, alpha)) {
        g_target->DrawText(text.c_str(), (UINT32)text.size(), format, &box, ink, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        ink->Release();
    }
}

D2D1_RECT_F Offset(D2D1_RECT_F rect, float x, float y) {
    return {rect.left + x, rect.top + y, rect.right + x, rect.bottom + y};
}

void DrawCard(Card& card, int index, float x, float alpha) {
    const Theme& theme = g_theme;
    float cardWidth = g_settings.width - 2 * kPad;
    D2D1_RECT_F rect{x, card.top, x + cardWidth, card.top + card.height};
    bool live = !card.goneAt;
    bool hovered = live && !card.mutedAt && g_hover.card == index;

    // Soft shadow: a few widening, fading layers (box-shadow: 0 4px 14px rgba(0,0,0,.35)).
    for (int layer = 4; layer >= 1; layer--) {
        float grow = layer * 2.0f;
        FillRounded(D2D1_RECT_F{rect.left - grow, rect.top - grow + 4, rect.right + grow, rect.bottom + grow + 4},
                    g_settings.radius + grow, 0x000000, 0.06f * g_settings.opacity * alpha);
    }
    FillRounded(rect, g_settings.radius, hovered && g_hover.part == Part::Body ? theme.cardHover : theme.card,
                g_settings.opacity * alpha);
    if (card.info.priority) {
        // Reminders, alarms and calls: accent outline and a bar along the left edge.
        StrokeRounded(rect, g_settings.radius, g_settings.priorityColor, alpha, 1.5f);
        FillRounded(D2D1_RECT_F{rect.left + 4, rect.top + g_settings.radius, rect.left + 7, rect.bottom - g_settings.radius}, 1.5f,
                    g_settings.priorityColor, alpha);
    } else {
        StrokeRounded(rect, g_settings.radius, theme.border, theme.borderAlpha * alpha, 1);
    }

    // Avatar: the toast's image cropped to the avatar shape (object-fit: cover), otherwise the app's first letter.
    float cx = rect.left + kCardPadX + kAvatar / 2, cy = rect.top + kCardPadY + card.contentHeight / 2;
    auto fillAvatar = [&](ID2D1Brush* brush) {
        if (g_settings.avatarShape == AvatarShape::Circle) {
            D2D1_ELLIPSE circle{{cx, cy}, kAvatar / 2, kAvatar / 2};
            g_target->FillEllipse(&circle, brush);
        } else {
            float r = g_settings.avatarShape == AvatarShape::Rounded ? kAvatar * 0.22f : 0;
            D2D1_ROUNDED_RECT square{{cx - kAvatar / 2, cy - kAvatar / 2, cx + kAvatar / 2, cy + kAvatar / 2}, r, r};
            g_target->FillRoundedRectangle(&square, brush);
        }
    };
    ID2D1BitmapBrush* avatarBrush = nullptr;
    if (card.showAvatar && card.avatar) {
        D2D1_SIZE_F size = card.avatar->GetSize();
        float fit = kAvatar / std::min(size.width, size.height);
        D2D1_BITMAP_BRUSH_PROPERTIES brushProps{D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP,
                                                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR};
        D2D1_BRUSH_PROPERTIES common{alpha, D2D1::Matrix3x2F::Scale(fit, fit) *
                                                D2D1::Matrix3x2F::Translation(cx - size.width * fit / 2,
                                                                              cy - size.height * fit / 2)};
        g_target->CreateBitmapBrush(card.avatar, &brushProps, &common, &avatarBrush);
    }
    if (avatarBrush) {
        fillAvatar(avatarBrush);
        avatarBrush->Release();
    } else if (card.showAvatar) {
        if (ID2D1SolidColorBrush* fill = Brush(card.info.priority ? g_settings.priorityColor : 0x808080, 0.3f * alpha)) {
            fillAvatar(fill);
            fill->Release();
        }
        std::wstring source = card.info.app.empty() ? card.info.title : card.info.app;
        if (!source.empty()) {
            std::wstring letter(1, (wchar_t)towupper(source[0]));
            DrawString(letter, g_letterFormat, {cx - kAvatar / 2, cy - kAvatar / 2, cx + kAvatar / 2, cy + kAvatar / 2},
                       theme.ink, alpha);
        }
    }

    float textLeft = rect.left + kCardPadX + AvatarSpace(card);
    DWRITE_TEXT_METRICS metrics{};
    float textHeight = card.text && SUCCEEDED(card.text->GetMetrics(&metrics)) ? std::min(metrics.height, g_settings.maxLines * kLine) : 0;
    float textTop = rect.top + kCardPadY + (card.contentHeight - kLine - textHeight) / 2;
    if (ID2D1SolidColorBrush* ink = Brush(theme.ink, alpha)) {
        if (card.title) {
            g_target->DrawTextLayout(D2D1_POINT_2F{textLeft, textTop}, card.title, ink, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        ink->Release();
    }
    if (ID2D1SolidColorBrush* ink = Brush(theme.ink, 0.8f * alpha)) {
        if (card.text) {
            g_target->DrawTextLayout(D2D1_POINT_2F{textLeft, textTop + kLine}, card.text, ink, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        ink->Release();
    }

    // Reply box: what is typed on the card, with a caret while it has the keyboard.
    if (card.info.hasInput) {
        D2D1_RECT_F box = Offset(InputRect(card), rect.left, rect.top);
        bool over = hovered && g_hover.part == Part::Input;
        FillRounded(box, kFieldRadius, over || card.info.inputFocused ? theme.fieldHover : theme.field, alpha);
        if (card.info.inputFocused) {
            StrokeRounded(box, kFieldRadius, card.info.priority ? g_settings.priorityColor : 0x0866ff, alpha, 1.5f);
        }
        D2D1_RECT_F textBox{box.left + 10, box.top, box.right - 10, box.bottom};
        bool empty = card.info.inputValue.empty();
        DrawString(empty ? card.info.placeholder : card.info.inputValue, g_fieldFormat, textBox, theme.ink,
                   (empty ? 0.5f : 1.0f) * alpha);
        if (card.info.inputFocused) {
            float caretX = textBox.left;
            IDWriteTextLayout* layout = nullptr;
            if (!empty && SUCCEEDED(g_dwrite->CreateTextLayout(card.info.inputValue.c_str(),
                                                              (UINT32)card.info.inputValue.size(), g_fieldFormat,
                                                              10000, kFieldHeight, &layout))) {
                DWRITE_TEXT_METRICS m{};
                layout->GetMetrics(&m);
                caretX = std::min(textBox.left + m.widthIncludingTrailingWhitespace + 1, textBox.right);
                layout->Release();
            }
            if (ID2D1SolidColorBrush* ink = Brush(theme.ink, alpha)) {
                g_target->DrawLine({caretX, box.top + 7}, {caretX, box.bottom - 7}, ink, 1);
                ink->Release();
            }
        }
    }

    for (int i = 0; i < (int)card.info.buttons.size(); i++) {
        D2D1_RECT_F button = Offset(ButtonRect(card, i), rect.left, rect.top);
        bool over = hovered && g_hover.part == Part::Button && g_hover.index == i;
        // A pressed button squeezes and springs back (playful style).
        D2D1_MATRIX_3X2_F before;
        g_target->GetTransform(&before);
        bool bounce = i == card.pressedButton && card.pressedAt && Playful();
        if (bounce) {
            float t = Progress(NowMs() - card.pressedAt, g_settings.pressMs);
            float s = Keyframes(t, {{0, 1}, {0.3f, 0.92f}, {0.65f, 1.06f}, {1, 1}});
            D2D1_POINT_2F center{(button.left + button.right) / 2, (button.top + button.bottom) / 2};
            g_target->SetTransform(D2D1::Matrix3x2F::Scale(s, s, center) * before);
        }
        FillRounded(button, kFieldRadius, over ? theme.fieldHover : theme.field, alpha);
        DrawString(card.info.buttons[i], g_buttonFormat, button, theme.ink, alpha);
        if (bounce) {
            g_target->SetTransform(before);
        }
    }

    // Just muted: a moon in the avatar's place and until when, over the card's content.
    if (card.mutedAt) {
        FillRounded(rect, g_settings.radius, theme.card, alpha);
        float cx = rect.left + kCardPadX + kAvatar / 2, cy = rect.top + card.height / 2;
        FillRounded({cx - kAvatar / 2, cy - kAvatar / 2, cx + kAvatar / 2, cy + kAvatar / 2}, kAvatar / 2, theme.field,
                    alpha);
        if (g_bigIconFormat) {
            DrawString(L"\xE708", g_bigIconFormat, {cx - kAvatar / 2, cy - kAvatar / 2, cx + kAvatar / 2, cy + kAvatar / 2},
                       theme.ink, alpha);
        }
        float textLeft = rect.left + kCardPadX + kAvatar + kGap;
        DrawString(card.mutedNote, g_fieldFormat, {textLeft, rect.top, rect.right - kCardPadX, rect.bottom}, theme.ink,
                   alpha);
    }

    // The open mute list covers the card's content.
    if (hovered && card.muteOpen) {
        FillRounded(rect, g_settings.radius, theme.card, alpha);
        DrawString(PolishUI() ? L"Wycisz na" : L"Mute for", g_fieldFormat,
                   {rect.left + kMuteLabelLeft, rect.top, rect.left + kMuteLabelLeft + kMuteLabelWidth, rect.bottom},
                   theme.ink, alpha);
        for (int i = 0; i < kMuteTimeCount; i++) {
            D2D1_RECT_F chip = Offset(MuteChoiceRect(card, i, cardWidth), rect.left, rect.top);
            bool over = g_hover.part == Part::MuteChoice && g_hover.index == i;
            FillRounded(chip, kFieldRadius, over ? theme.fieldHover : theme.field, alpha);
            DrawString(kMuteTimes[i].label, g_buttonFormat, chip, theme.ink, alpha);
        }
    }

    // Close button on hover, top right.
    if (hovered) {
        float bx = rect.right - 14, by = rect.top + 14;
        if (g_hover.part == Part::Close) {
            if (ID2D1SolidColorBrush* fill = Brush(0x808080, 0.3f)) {
                D2D1_ELLIPSE closeCircle{{bx, by}, 10, 10};
                g_target->FillEllipse(&closeCircle, fill);
                fill->Release();
            }
        }
        if (ID2D1SolidColorBrush* ink = Brush(theme.ink, 0.8f)) {
            g_target->DrawLine({bx - 4, by - 4}, {bx + 4, by + 4}, ink, 1.4f);
            g_target->DrawLine({bx + 4, by - 4}, {bx - 4, by + 4}, ink, 1.4f);
            ink->Release();
        }
        // Mute button on hover, top left: a moon (the Do not disturb icon), opens the list of times.
        float mx = rect.left + 14, my = rect.top + 14;
        D2D1_ELLIPSE muteCircle{{mx, my}, 10, 10};
        if (ID2D1SolidColorBrush* fill = Brush(g_hover.part == Part::Mute ? 0x808080 : theme.card,
                                               g_hover.part == Part::Mute ? 0.3f : 0.85f)) {
            g_target->FillEllipse(&muteCircle, fill);
            fill->Release();
        }
        if (g_iconFormat) {
            DrawString(L"\xE708", g_iconFormat, {mx - 10, my - 10, mx + 10, my + 10}, theme.ink, 0.8f);
        }
    }
}

// Returns whether anything is still animating.
bool Render() {
    ULONGLONG now = NowMs();
    for (auto& c : g_cards) {
        if (c.copiedAt && !c.goneAt && now - c.copiedAt >= kCopiedMs) {
            c.goneAt = now;
        }
        if (c.mutedAt && !c.goneAt && now - c.mutedAt >= kMutedMs) {
            c.goneAt = now;
        }
    }
    g_cards.erase(std::remove_if(g_cards.begin(), g_cards.end(),
                                 [&](Card& c) {
                                     bool done = c.goneAt && now - c.goneAt >= OutMs();
                                     if (done) {
                                         ReleaseCard(c);
                                     }
                                     return done;
                                 }),
                  g_cards.end());
    if (g_cards.empty()) {
        ShowWindow(g_window, SW_HIDE);
        g_hover = {};
        // The window hid under the mouse: no WM_MOUSELEAVE may come, and the next cards mustn't wait for one.
        if (g_holdUntil == ULLONG_MAX) {
            g_holdUntil = GetTickCount64() + kHoverGraceMs;
        }
        return false;
    }

    float scale = Scale();
    bool playful = Playful();
    float layoutHeight = kPad;
    for (auto& card : g_cards) {
        layoutHeight += card.height + kPad;
    }
    // Newest card next to the screen edge: at the bottom in bottom corners, at the top in top corners. Positions are
    // kept as the distance of a card's top from that edge, so a card that changes place (playful style) glides there
    // while the window grows or shrinks under it.
    std::vector<float> edges(g_cards.size());
    float heightDip = layoutHeight;
    float y = kPad;
    bool animating = false;
    for (size_t n = 0; n < g_cards.size(); n++) {
        size_t i = g_settings.top ? g_cards.size() - 1 - n : n;
        Card& card = g_cards[i];
        float target = g_settings.top ? y : layoutHeight - y;
        y += card.height + kPad;
        float edge = target;
        if (!playful || card.edgeTo < 0) {
            card.edgeFrom = card.edgeTo = target;
        } else {
            if (target != card.edgeTo) {
                // Start the glide from wherever the card is now (also in the middle of an earlier one).
                float glide = EaseOutBack(Progress(now - card.shiftAt, g_settings.shiftMs));
                card.edgeFrom += (card.edgeTo - card.edgeFrom) * glide;
                card.edgeTo = target;
                card.shiftAt = now;
            }
            float t = Progress(now - card.shiftAt, g_settings.shiftMs);
            edge = card.edgeFrom + (card.edgeTo - card.edgeFrom) * EaseOutBack(t);
            animating |= t < 1;
        }
        edges[i] = edge;
        heightDip = std::max(heightDip, g_settings.top ? edge + card.height + kPad : edge + kPad);
    }
    SIZE size{(LONG)(g_settings.width * scale + 0.5f), (LONG)(heightDip * scale + 0.5f)};
    if (size.cx != g_bitmapSize.cx || size.cy != g_bitmapSize.cy) {
        if (g_bitmap) {
            DeleteObject(g_bitmap);
        }
        BITMAPINFO bi{};
        bi.bmiHeader = {sizeof(BITMAPINFOHEADER), size.cx, -size.cy, 1, 32, BI_RGB};
        void* bits = nullptr;
        g_bitmap = CreateDIBSection(g_memDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        SelectObject(g_memDC, g_bitmap);
        g_bitmapSize = size;
    }
    RECT bindRect{0, 0, size.cx, size.cy};
    g_target->BindDC(g_memDC, &bindRect);
    g_target->SetDpi(96 * scale, 96 * scale);
    g_target->BeginDraw();
    D2D1_COLOR_F transparent{0, 0, 0, 0};
    g_target->Clear(&transparent);

    float cardWidth = g_settings.width - 2 * kPad;
    for (size_t i = 0; i < g_cards.size(); i++) {
        Card& card = g_cards[i];
        card.top = g_settings.top ? edges[i] : heightDip - edges[i];
        animating |= card.goneAt || card.copiedAt || card.mutedAt || (card.pressedAt && now - card.pressedAt < g_settings.pressMs);
        if (!playful) {
            float in = EaseOut(Progress(now - card.shownAt, g_settings.inMs));
            float alpha = card.goneAt ? 1 - Progress(now - card.goneAt, g_settings.outMs) : 1.0f;
            animating |= in < 1;
            float slide = (1 - in) * cardWidth * 1.1f;
            DrawCard(card, (int)i, kPad + (g_settings.left ? -slide : slide), alpha);
            continue;
        }
        // Bubble: grows out of the corner nearest the screen corner with a little tilt, bursts back into it.
        float pop, turn, alpha;
        float in = Progress(now - card.shownAt, g_settings.popInMs);
        animating |= in < 1;
        if (card.goneAt) {
            float out = Progress(now - card.goneAt, g_settings.popOutMs);
            pop = Keyframes(out, {{0, 1}, {0.35f, 1.05f}, {1, 0}});
            alpha = Keyframes(out, {{0, 1}, {0.35f, 1}, {1, 0}});
            turn = 0;
        } else {
            pop = Keyframes(in, {{0, 0.2f}, {0.6f, 1.06f}, {0.82f, 0.98f}, {1, 1}});
            turn = Keyframes(in, {{0, 8}, {0.6f, -1.5f}, {1, 0}});
            alpha = Keyframes(in, {{0, 0}, {0.6f, 1}, {1, 1}});
        }
        if (g_settings.left != g_settings.top) {
            turn = -turn;  // tilt away from the screen corner on either side
        }
        D2D1_POINT_2F origin{g_settings.left ? kPad : kPad + cardWidth, g_settings.top ? card.top : card.top + card.height};
        g_target->SetTransform(D2D1::Matrix3x2F::Scale(pop, pop, origin) * D2D1::Matrix3x2F::Rotation(turn, origin));
        DrawCard(card, (int)i, kPad, std::clamp(alpha, 0.0f, 1.0f));
        g_target->SetTransform(D2D1::Matrix3x2F::Identity());
    }
    g_target->EndDraw();

    RECT work = WorkArea();
    LONG margin = (LONG)(g_settings.margin * Scale() / g_settings.zoom + 0.5f);  // screen pixels, not zoomed
    POINT dst{g_settings.left ? work.left + margin : work.right - size.cx - margin,
              g_settings.top ? work.top + margin : work.bottom - size.cy - margin};
    POINT src{0, 0};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_window, nullptr, &dst, &size, g_memDC, &src, 0, &blend, ULW_ALPHA);
    if (!IsWindowVisible(g_window)) {
        ShowWindow(g_window, SW_SHOWNOACTIVATE);
    }
    SetWindowPos(g_window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    return animating;
}

// While cards move or fade, the card thread draws a frame per screen refresh (see UiThread).
bool g_animating = false;

void Redraw() {
    g_animating = Render();
}

bool Inside(D2D1_RECT_F rect, float x, float y) {
    return x >= rect.left && x < rect.right && y >= rect.top && y < rect.bottom;
}

// What is under a point in window pixels.
Hit HitTest(POINT pt) {
    float scale = Scale();
    float x = pt.x / scale, y = pt.y / scale;
    for (size_t i = 0; i < g_cards.size(); i++) {
        const Card& card = g_cards[i];
        if (card.goneAt || card.mutedAt || x < kPad || x >= g_settings.width - kPad || y < card.top || y >= card.top + card.height) {
            continue;
        }
        float cardX = x - kPad, cardY = y - card.top;
        float bx = g_settings.width - 2 * kPad - 14, by = 14;
        if ((cardX - bx) * (cardX - bx) + (cardY - by) * (cardY - by) <= 12 * 12) {
            return {(int)i, Part::Close};
        }
        if ((cardX - 14) * (cardX - 14) + (cardY - 14) * (cardY - 14) <= 12 * 12) {
            return {(int)i, Part::Mute};
        }
        if (card.muteOpen) {  // the list covers the buttons and the reply box
            for (int t = 0; t < kMuteTimeCount; t++) {
                if (Inside(MuteChoiceRect(card, t, g_settings.width - 2 * kPad), cardX, cardY)) {
                    return {(int)i, Part::MuteChoice, t};
                }
            }
            return {(int)i, Part::Body};
        }
        if (card.info.hasInput && Inside(InputRect(card), cardX, cardY)) {
            return {(int)i, Part::Input};
        }
        for (int b = 0; b < (int)card.info.buttons.size(); b++) {
            if (Inside(ButtonRect(card, b), cardX, cardY)) {
                return {(int)i, Part::Button, b};
            }
        }
        return {(int)i, Part::Body};
    }
    return {};
}

void CloseMuteLists() {
    for (auto& card : g_cards) {
        card.muteOpen = false;
    }
}

void MutedUntilNote(Card& clicked, ULONGLONG minutes);

// Mutes an app for a while (only in memory) or for good (kMuteForever, saved): its cards go (the notifications stay
// in the notification center) and its next toasts get none until the time is up.
// The clicked card says until when for a moment (kMutedMs), then goes; the app's other cards go at once.
void MuteApp(Card& clicked, ULONGLONG minutes) {
    std::wstring app = clicked.info.app;
    std::erase_if(g_muted, [&](const auto& m) { return _wcsicmp(m.first.c_str(), app.c_str()) == 0; });
    ULONGLONG now = NowMs();
    if (minutes == kMuteForever) {
        g_mutedForever.push_back(app);
        SaveMutedForever();
        Log(L"Muted %s for good", app.c_str());
        clicked.mutedNote = PolishUI() ? L"Wyciszono na stałe" : L"Muted for good";
    } else {
        g_muted.push_back({app, GetTickCount64() + minutes * 60 * 1000});
        Log(L"Muted %s for %llu min", app.c_str(), minutes);
        MutedUntilNote(clicked, minutes);
    }
    clicked.mutedAt = now;
    clicked.muteOpen = false;
    clicked.info.inputFocused = false;
    g_skipped.push_back(clicked.info.id);
    for (auto& card : g_cards) {
        if (!card.goneAt && !card.mutedAt && _wcsicmp(card.info.app.c_str(), app.c_str()) == 0) {
            card.info.inputFocused = false;
            card.muteOpen = false;
            card.goneAt = now;
            g_skipped.push_back(card.info.id);
        }
    }
}

// "Muted until 13:45" (in the user's clock format), with "tomorrow" for a day.
void MutedUntilNote(Card& clicked, ULONGLONG minutes) {
    // The end time in the user's clock format (13:45 or 1:45 PM).
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER t{{ft.dwLowDateTime, ft.dwHighDateTime}};
    t.QuadPart += minutes * 60 * 10000000ULL;
    ft = {t.LowPart, t.HighPart};
    FILETIME localFt;
    SYSTEMTIME local{};
    wchar_t clock[32] = L"";
    if (FileTimeToLocalFileTime(&ft, &localFt) && FileTimeToSystemTime(&localFt, &local)) {
        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &local, nullptr, clock, ARRAYSIZE(clock));
    }
    clicked.mutedNote = (PolishUI() ? L"Wyciszono do " : L"Muted until ") + std::wstring(clock);
    if (minutes >= 24 * 60) {
        clicked.mutedNote += PolishUI() ? L" jutro" : L" tomorrow";
    }
}

void SendCommand(const Card& card, Action action, int index = 0) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_commands.push_back({card.info, action, index, action == Action::Button ? card.info.inputValue : L""});
    SetEvent(g_wakeEvent);
}

// The reply is typed on the card itself: the card window takes the keyboard while its reply box is focused, and the
// text goes to the toast only with the button (see Command::reply). So typing is instant and needs no hidden window.
Card* TypingCard() {
    auto it = std::find_if(g_cards.begin(), g_cards.end(), [](const Card& c) { return c.info.inputFocused && !c.goneAt; });
    return it == g_cards.end() ? nullptr : &*it;
}

void EndTyping() {
    bool was = false;
    for (auto& card : g_cards) {
        was = was || card.info.inputFocused;
        card.info.inputFocused = false;
    }
    if (was) {
        ApplySnapshot();  // a card whose toast went meanwhile goes now
        Redraw();
    }
}

void StartTyping(HWND hwnd, Card& card) {
    for (auto& other : g_cards) {
        other.info.inputFocused = false;
    }
    card.info.inputFocused = true;
    // Never activated by a click (WM_MOUSEACTIVATE): only now, for the keyboard. Clicking elsewhere ends it.
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
}

// A pressed button goes at once; the worker presses it on the banner or in the notification center meanwhile.
// Skipped, so the toast doesn't get a card again while Windows still has it (e.g. after Snooze).
void SendReply(Card& card, int button) {
    if (!card.copiedAt) {
        card.pressedButton = button;
        card.pressedAt = NowMs();
    }
    if (button == card.copyButton) {
        // The mod's own button: the code goes to the clipboard, the button says so for a moment, then the card
        // closes (Render); the toast stays in the center.
        if (card.copiedAt) {
            return;
        }
        bool ok = CopyText(g_window, card.code);
        card.info.buttons[button] = ok ? std::wstring(L"✓") : std::wstring(L"✗");
        card.info.inputFocused = false;
        card.copiedAt = NowMs();
        EndTyping();
        return;
    }
    SendCommand(card, Action::Button, button);
    card.info.inputValue.clear();
    card.info.inputFocused = false;
    card.goneAt = NowMs();
    g_skipped.push_back(card.info.id);
    EndTyping();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_APP_SNAPSHOT:
            ApplySnapshot();
            Redraw();
            return 0;
        case WM_APP_CENTER:
            SetWindowTextW(hwnd, wParam ? kCenterTitle : kCardTitle);
            return 0;
        case WM_APP_REHOOK:
            if (g_eventHook) {
                UnhookWinEvent(g_eventHook);
            }
            g_eventHook = HookBannerEvents();
            Log(L"Watching ShellExperienceHost %lu", g_shellPid.load());
            return 0;
        case WM_APP_SETTINGS:
            // Size, lines, colors and avatars change the cards already shown.
            LoadSettings();
            g_theme = CurrentTheme();
            for (auto& card : g_cards) {
                card.showAvatar = g_settings.showAvatars && RuleFor(card.info.app).avatar;
                ReleaseCard(card);
                card.title = card.text = nullptr;
                card.avatar = nullptr;
                LayoutCard(card);
            }
            Redraw();
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_MOUSEMOVE: {
            Hit hit = HitTest(POINT{(short)LOWORD(lParam), (short)HIWORD(lParam)});
            g_holdUntil = hit.card >= 0 ? ULLONG_MAX : GetTickCount64() + kHoverGraceMs;
            if (!(hit == g_hover)) {
                if (hit.card != g_hover.card) {
                    CloseMuteLists();
                }
                g_hover = hit;
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                Redraw();
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            g_holdUntil = GetTickCount64() + kHoverGraceMs;
            g_hover = {};
            CloseMuteLists();
            Redraw();
            return 0;
        case WM_LBUTTONUP: {
            Hit hit = HitTest(POINT{(short)LOWORD(lParam), (short)HIWORD(lParam)});
            if (hit.card < 0) {
                return 0;
            }
            Card& card = g_cards[hit.card];
            switch (hit.part) {
                case Part::Body:
                    if (card.muteOpen) {  // a click beside the list's times only closes it
                        card.muteOpen = false;
                        break;
                    }
                    [[fallthrough]];
                case Part::Close:
                    SendCommand(card, hit.part == Part::Body ? Action::Open : Action::Dismiss);
                    card.info.inputFocused = false;
                    card.goneAt = NowMs();
                    g_skipped.push_back(card.info.id);  // the banner may still list it: no new card for it
                    break;
                case Part::Mute:
                    card.muteOpen = !card.muteOpen;
                    break;
                case Part::MuteChoice:
                    MuteApp(card, kMuteTimes[hit.index].minutes);
                    break;
                case Part::Button:
                    SendReply(card, hit.index);
                    break;
                case Part::Input:
                    StartTyping(hwnd, card);
                    break;
                case Part::None:
                    break;
            }
            Redraw();
            return 0;
        }
        case WM_KILLFOCUS:
            EndTyping();
            return 0;
        case WM_CHAR: {
            Card* card = TypingCard();
            if (!card) {
                return 0;
            }
            std::wstring& text = card->info.inputValue;
            wchar_t c = (wchar_t)wParam;
            if (c == L'\r') {
                if (!card->info.buttons.empty() && card->copyButton != 0) {
                    SendReply(*card, 0);  // Enter sends with the first button, like the toast's reply box
                }
            } else if (c == 27) {
                EndTyping();
            } else if (c == L'\b') {
                if (!text.empty()) {
                    text.pop_back();
                    if (!text.empty() && IS_HIGH_SURROGATE(text.back())) {
                        text.pop_back();
                    }
                }
            } else if (c == 0x7F) {  // Ctrl+Backspace: the last word
                while (!text.empty() && iswspace(text.back())) text.pop_back();
                while (!text.empty() && !iswspace(text.back())) text.pop_back();
            } else if (c == 0x16) {  // Ctrl+V
                if (OpenClipboard(hwnd)) {
                    if (HANDLE data = GetClipboardData(CF_UNICODETEXT)) {
                        if (auto clip = (const wchar_t*)GlobalLock(data)) {
                            for (const wchar_t* p = clip; *p; p++) {
                                text += *p == L'\r' || *p == L'\n' || *p == L'\t' ? L' ' : *p;
                            }
                            GlobalUnlock(data);
                        }
                    }
                    CloseClipboard();
                }
            } else if (c >= 0x20) {
                text += c;
            }
            Redraw();
            return 0;
        }
        case WM_SETCURSOR: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            SetCursor(LoadCursor(nullptr, HitTest(pt).part == Part::Input ? IDC_IBEAM : IDC_HAND));
            return TRUE;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool CreateResources() {
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr,
                                 reinterpret_cast<void**>(&g_d2d))) ||
        FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                   reinterpret_cast<IUnknown**>(&g_dwrite)))) {
        return false;
    }
    // Without WIC the cards just show letters instead of avatars.
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_wic));
    auto format = [](DWRITE_FONT_WEIGHT weight, float size, IDWriteTextFormat** out) {
        g_dwrite->CreateTextFormat(L"Segoe UI", nullptr, weight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                                   size, L"", out);
    };
    format(DWRITE_FONT_WEIGHT_SEMI_BOLD, kFontSize, &g_titleFormat);
    format(DWRITE_FONT_WEIGHT_NORMAL, kFontSize, &g_textFormat);
    format(DWRITE_FONT_WEIGHT_SEMI_BOLD, 17, &g_letterFormat);
    format(DWRITE_FONT_WEIGHT_SEMI_BOLD, kFontSize, &g_buttonFormat);
    format(DWRITE_FONT_WEIGHT_NORMAL, kFontSize, &g_fieldFormat);
    if (!g_titleFormat || !g_textFormat || !g_letterFormat || !g_buttonFormat || !g_fieldFormat) {
        return false;
    }
    // Name on one line, text on at most maxLines lines, both cut with an ellipsis (text-overflow, -webkit-line-clamp).
    DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
    for (IDWriteTextFormat* f : {g_titleFormat, g_textFormat, g_buttonFormat, g_fieldFormat}) {
        IDWriteInlineObject* ellipsis = nullptr;
        g_dwrite->CreateEllipsisTrimmingSign(f, &ellipsis);
        f->SetTrimming(&trimming, ellipsis);
        if (ellipsis) {
            ellipsis->Release();
        }
    }
    for (IDWriteTextFormat* f : {g_titleFormat, g_textFormat}) {
        f->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, kLine, kLine * 0.8f);
    }
    g_titleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    g_textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK);
    for (IDWriteTextFormat* f : {g_buttonFormat, g_fieldFormat}) {
        f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    g_buttonFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    g_letterFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    g_letterFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    // Icon font for the mute button; without one the button is an empty circle.
    IDWriteFontCollection* fonts = nullptr;
    if (SUCCEEDED(g_dwrite->GetSystemFontCollection(&fonts, FALSE))) {
        for (const wchar_t* family : {L"Segoe Fluent Icons", L"Segoe MDL2 Assets"}) {
            UINT32 at = 0;
            BOOL exists = FALSE;
            if (SUCCEEDED(fonts->FindFamilyName(family, &at, &exists)) && exists) {
                g_dwrite->CreateTextFormat(family, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                           DWRITE_FONT_STRETCH_NORMAL, 11, L"", &g_iconFormat);
                g_dwrite->CreateTextFormat(family, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                           DWRITE_FONT_STRETCH_NORMAL, 18, L"", &g_bigIconFormat);
                break;
            }
        }
        fonts->Release();
    }
    for (IDWriteTextFormat* f : {g_iconFormat, g_bigIconFormat}) {
        if (f) {
            f->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
    }

    D2D1_RENDER_TARGET_PROPERTIES props{D2D1_RENDER_TARGET_TYPE_DEFAULT,
                                        {DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED},
                                        0,
                                        0,
                                        D2D1_RENDER_TARGET_USAGE_NONE,
                                        D2D1_FEATURE_LEVEL_DEFAULT};
    if (FAILED(g_d2d->CreateDCRenderTarget(&props, &g_target))) {
        return false;
    }
    g_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    g_memDC = CreateCompatibleDC(nullptr);
    return true;
}

void ReleaseResources() {
    for (auto& card : g_cards) {
        ReleaseCard(card);
    }
    g_cards.clear();
    if (g_bitmap) {
        DeleteObject(g_bitmap);
    }
    if (g_memDC) {
        DeleteDC(g_memDC);
    }
    for (IUnknown* object : std::initializer_list<IUnknown*>{g_target, g_iconFormat, g_bigIconFormat, g_fieldFormat, g_buttonFormat, g_letterFormat,
                                                             g_textFormat, g_titleFormat, g_dwrite, g_wic, g_d2d}) {
        if (object) {
            object->Release();
        }
    }
}

extern "C" IMAGE_DOS_HEADER __ImageBase;

DWORD WINAPI UiThread(LPVOID) {
    if (!IsShellProcess()) {
        return 0;
    }
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    WNDCLASSW wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = (HINSTANCE)&__ImageBase;
    wc.lpszClassName = kWindowClass;
    // The card window hides the banners (SetWindowPos_Hook), so it comes only once the worker can read toasts; if the
    // worker ends first (no UI Automation), the normal banners stay.
    HANDLE waits[] = {g_stopEvent, g_workerReady, g_workerThread};
    bool workerReady = WaitForMultipleObjects(3, waits, FALSE, INFINITE) == WAIT_OBJECT_0 + 1;
    if (!workerReady) {
        Log(L"No card window: the worker didn't start");
    } else if (!CreateResources()) {
        Log(L"Direct2D/DirectWrite unavailable");
    } else if (!RegisterClassW(&wc)) {
        Log(L"Registering the window class failed (%lu)", GetLastError());
    } else {
        g_window = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kWindowClass,
                                   kCardTitle, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);
        Log(L"Card window %p (error %lu)", g_window.load(), g_window ? 0 : GetLastError());
        // Out-of-context window events arrive through this thread's message loop.
        g_eventHook = HookBannerEvents();
        // Waiting on the stop event directly: a WM_QUIT posted before this thread had a message queue would be lost.
        // While animating it doesn't wait: after the messages, DwmFlush waits for the next screen refresh and a frame
        // is drawn, so animations run at the monitor's rate (60, 144 Hz...) instead of a ~64 Hz timer.
        for (;;) {
            DWORD woke = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, g_animating ? 0 : INFINITE, QS_ALLINPUT);
            if (woke == WAIT_OBJECT_0 || woke == WAIT_FAILED) {
                break;
            }
            MSG msg;
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);  // keys to WM_CHAR for the reply typed on a card
                DispatchMessage(&msg);
            }
            if (g_animating) {
                if (FAILED(DwmFlush())) {
                    Sleep(8);
                }
                Redraw();
            }
        }
        if (g_eventHook) {
            UnhookWinEvent(g_eventHook);
            g_eventHook = nullptr;
        }
        HWND window = g_window.exchange(nullptr);
        DestroyWindow(window);
        UnregisterClassW(kWindowClass, wc.hInstance);
    }
    ReleaseResources();
    CoUninitialize();
    return 0;
}

bool StartCards() {
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_workerReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent || !g_wakeEvent || !g_workerReady) {
        return false;
    }
    // The worker first: the card thread waits for it (see UiThread).
    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_workerThread) {
        return false;
    }
    g_uiThread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    return g_uiThread != nullptr;
}

// Both threads end soon after the stop event: the card thread waits on it, the worker checks it between toasts
// and while reading a new one.
// Neither sends messages to the thread unloading the mod, so waiting without a timeout can't deadlock, and the mod's
// code is never freed while one of them still runs.
void StopCards() {
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    for (HANDLE* handle : {&g_uiThread, &g_workerThread, &g_wakeEvent, &g_workerReady, &g_stopEvent}) {
        if (*handle) {
            if (handle == &g_uiThread || handle == &g_workerThread) {
                WaitForSingleObject(*handle, INFINITE);
            }
            CloseHandle(*handle);
            *handle = nullptr;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Process roles: ShellExperienceHost only hides the banner; explorer hides it too (some builds move it from there),
// and the shell's explorer also reads the toasts and draws the cards (IsShellProcess). Windhawk's separate "tool mod"
// process didn't survive reloads, and explorer is always running anyway.

// Sound: ShellExperienceHost plays a toast's sound by opening its .wav when the banner comes up, and banners come one
// at a time (about 7 s each), so with the cards the sounds would keep coming after the cards are gone. While the card
// window exists, opening a sound from %WINDIR%\Media fails there, and the card thread plays one sound per new batch
// of cards (PlayCardSound). Looping alarm and call sounds (Alarm*, Ring*) stay with Windows: those toasts wait for
// their banner anyway.
using CreateFileW_t = decltype(&CreateFileW);
CreateFileW_t CreateFileW_Original;
HANDLE WINAPI CreateFileW_Hook(LPCWSTR name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sa, DWORD disposition,
                               DWORD flags, HANDLE templateFile) {
    static const std::wstring media = [] {
        WCHAR dir[MAX_PATH];
        UINT n = GetWindowsDirectoryW(dir, ARRAYSIZE(dir));
        return n && n < ARRAYSIZE(dir) ? std::wstring(dir) + L"\\Media\\" : std::wstring();
    }();
    if (name && !media.empty() && g_soundMode != 1) {
        size_t n = wcslen(name);
        const WCHAR* file = name + std::min(n, media.size());
        if (n > media.size() + 4 && _wcsnicmp(name, media.c_str(), media.size()) == 0 &&
            _wcsicmp(name + n - 4, L".wav") == 0 && !wcschr(file, L'\\') && _wcsnicmp(file, L"Alarm", 5) != 0 &&
            _wcsnicmp(file, L"Ring", 4) != 0 && FindWindowW(kWindowClass, nullptr)) {
            Log(L"Banner sound muted");
            SetLastError(ERROR_FILE_NOT_FOUND);
            return INVALID_HANDLE_VALUE;
        }
    }
    return CreateFileW_Original(name, access, share, sa, disposition, flags, templateFile);
}

BOOL Wh_ModInit() {
    WCHAR path[MAX_PATH];
    if (!GetModuleFileName(nullptr, path, ARRAYSIZE(path))) {
        return FALSE;
    }
    const WCHAR* base = wcsrchr(path, L'\\');
    base = base ? base + 1 : path;
    bool explorer = _wcsicmp(base, L"explorer.exe") == 0;
    if (!explorer && _wcsicmp(base, L"ShellExperienceHost.exe") != 0) {
        return FALSE;
    }
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original);
    if (!explorer) {
        // kernelbase's export: kernel32's CreateFileW is only a stub, and most Windows DLLs call kernelbase directly.
        if (auto createFile = (CreateFileW_t)(void*)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"), "CreateFileW")) {
            WindhawkUtils::SetFunctionHook(createFile, CreateFileW_Hook, &CreateFileW_Original);
        }
    }
    g_explorer = explorer;
    Log(L"Loaded in %s", base);
    return TRUE;
}

void Wh_ModAfterInit() {
    HideExistingBanner();
    LoadSettings();
    if (g_explorer && !StartCards()) {
        Log(L"Starting the cards failed");
    }
}

void Wh_ModSettingsChanged() {
    if (!g_explorer) {
        LoadSettings();  // ShellExperienceHost: only the sound mode matters here (see g_soundMode)
    } else if (HWND window = g_window) {
        PostMessage(window, WM_APP_SETTINGS, 0, 0);
    }
}

void Wh_ModBeforeUninit() {
    // A banner that stays up (a reminder, an alarm) would otherwise stay off the screen until Windows moves it.
    RestoreBanner();
}

void Wh_ModUninit() {
    if (g_explorer) {
        StopCards();
    }
}
