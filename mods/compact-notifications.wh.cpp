// ==WindhawkMod==
// @id              compact-notifications
// @name            Compact Notifications
// @description     Replaces Windows notification banners with compact chat-style cards: avatar, sender and a few lines of text
// @version         1.0
// @author          Wheatlay
// @github          https://github.com/Wheatlay
// @license         GPL-3.0
// @include         ShellExperienceHost.exe
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -ldwmapi -ld2d1 -ldwrite -lshcore -lgdi32 -lwindowscodecs -ladvapi32 -lurlmon
// ==/WindhawkMod==

// The tool-mod boilerplate and the banner detection idea come from the "Toast auto-dismiss" mod by nikitor32 (GPL-3.0),
// the way the banner window is told apart from other shell windows from "Notifications placement" by m417z (GPL-3.0),
// so this file is GPL-3.0 as well.

// ==WindhawkModReadme==
/*
# Compact Notifications

Replaces Windows toast banners with compact chat-style cards: black (or white) rounded
cards with a round avatar, the sender in bold and up to a few lines of text, sliding in from the screen edge.

## What works

* **Click** a card to do what clicking the original notification does (open the app or the conversation).
* **×** (on hover) closes the card; the notification stays in the notification center.
* **Buttons** from the notification (for example *Reply*, *Snooze*, *Dismiss*) are shown under the text.
* **Reply box**: click it and type, then press the app's send button. The text goes to the original reply
  box, so the app receives it as usual.
* **Avatars**: the notification's image (local files, web images and images from Store apps), or a thumbnail of
  its large picture (e.g. a screenshot); otherwise the app's registered icon (e.g. Firefox), and the first
  letter of the app as the last resort.
* **Priority notifications** (reminders, alarms, incoming calls) get a colored outline and stay until handled.

## How long a card stays

A card stays exactly as long as the original notification would. Windows sets this time in
*Settings > Accessibility > Visual effects > Dismiss notifications after this amount of time*.

## How it works

Windows still creates its own banner, but the mod moves it off the screen and reads it (UI Automation and
the notification database). Clicks and typing on a card go to the hidden banner, so every app action
works as before. Turning the mod off brings the original banners back.

## Limitations

* Text styles, progress bars and large (hero) images are not shown.
* While a full-screen app or *Do not disturb* is on, Windows doesn't show notifications at all, so there are
  no cards either.
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
  $name: Appearance
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
  - highlightPriority: true
    $name: Highlight priority notifications
    $description: Colored outline for reminders, alarms and incoming calls.
  - priorityColor: "#F0A020"
    $name: Priority color
    $description: Hex color, for example #F0A020 (orange) or #E0102F (red).
  $name: Behavior
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <shellscalingapi.h>
#include <uiautomation.h>
#include <wincodec.h>
#include <appmodel.h>
#include <urlmon.h>

#include <wchar.h>

#include <algorithm>
#include <atomic>
#include <memory>
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

BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if ((uFlags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOMOVE | SWP_NOSIZE) && IsBannerWindow(hWnd)) {
        if (uFlags & SWP_NOMOVE) {
            RECT rc{};
            GetWindowRect(hWnd, &rc);
            Y = rc.top;
            uFlags &= ~SWP_NOMOVE;
        }
        X = OffscreenX();
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

void HideExistingBanner() {
    HWND hwnd = nullptr;
    while ((hwnd = FindWindowExW(HWND_DESKTOP, hwnd, L"Windows.UI.Core.CoreWindow", nullptr)) != nullptr) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId() && IsBannerWindow(hwnd)) {
            RECT rc{};
            GetWindowRect(hwnd, &rc);
            SetWindowPos_Original(hwnd, nullptr, OffscreenX(), rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Reading: a worker thread finds the banner and reads its toasts through UI Automation.

struct ToastInfo {
    // App, title and text (plus a counter for identical toasts). UIA runtime ids change while the banner lives.
    std::wstring id;
    std::wstring app;
    std::wstring title;
    std::wstring text;
    std::wstring image;  // local file of the avatar (appLogoOverride), empty if none
    std::vector<std::wstring> buttons;
    bool hasInput = false;
    std::wstring placeholder;
    std::wstring inputValue;  // what has been typed into the (hidden) banner's reply box
    bool inputFocused = false;
    bool priority = false;  // reminders, alarms, calls: they stay until handled
};

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

// The avatar file from a toast payload: the appLogoOverride image as a local file. Web images are downloaded to
// the internet cache; ms-appx and ms-appdata point into the app's package and data folders.
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
            return src;  // downloaded in the background (StartAvatarDownload), the card shows a letter until then
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
            return after(L"file:///");
        }
        std::replace(src.begin(), src.end(), L'/', L'\\');
        return src.size() > 2 && src[1] == L':' ? src : std::wstring();
    }
    return {};
}

// Finds the newest stored toast whose text contains both the title and the message, and returns its avatar.
std::wstring FindAvatar(const std::wstring& title, const std::wstring& text) {
    if (!LoadSqlite()) {
        return {};
    }
    WCHAR localAppData[MAX_PATH];
    if (!GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, ARRAYSIZE(localAppData))) {
        return {};
    }
    std::string path = ToUtf8(std::wstring(localAppData) + L"\\Microsoft\\Windows\\Notifications\\wpndatabase.db");
    sqlite3* db = nullptr;
    if (g_sqlite.open_v2(path.c_str(), &db, SQLITE_OPEN_READONLY, nullptr) != 0) {
        Log(L"Opening the notification database failed");
        if (db) {
            g_sqlite.close(db);
        }
        return {};
    }
    // Only the start of the message: the banner can show it with other line breaks or cut off (Firefox, Gmail).
    std::wstring start = text.substr(0, std::min(text.find_first_of(L"\r\n"), (size_t)24));
    std::string wantTitle = ToUtf8(title), wantText = ToUtf8(start);
    std::string payload, aumid;
    sqlite3_stmt* stmt = nullptr;
    if (g_sqlite.prepare_v2(db,
                            "SELECT n.Payload, h.PrimaryId FROM Notification n JOIN NotificationHandler h "
                            "ON h.RecordId = n.HandlerId WHERE n.Type = 'toast' ORDER BY n.ArrivalTime DESC LIMIT 30",
                            -1, &stmt, nullptr) == 0) {
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
    g_sqlite.close(db);
    // The download can take a moment, so it happens after the database is closed.
    return payload.empty() ? std::wstring() : AvatarFromPayload(payload, FromUtf8(aumid));
}

// The icon a desktop app registered for its notifications (HKCU\Software\Classes\AppUserModelId\<id>, IconUri),
// found by the app name the banner shows. Used when the toast itself has no image or isn't in the database
// (Firefox doesn't store its toasts there).
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
            FileExists(value)) {
            icon = value;
        }
    }
    RegCloseKey(root);
    return icon;
}

enum class Action { Open, Dismiss, Button, FocusInput };

struct Command {
    std::wstring id;
    Action action;
    int index = 0;  // which button
};

std::mutex g_mutex;
std::vector<ToastInfo> g_snapshot;  // guarded by g_mutex
std::vector<Command> g_commands;    // guarded by g_mutex

HANDLE g_stopEvent = nullptr;
HANDLE g_wakeEvent = nullptr;  // set when the banner window shows or moves, or a card is clicked
HANDLE g_workerThread = nullptr;
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
HWND g_window = nullptr;
std::atomic<DWORD> g_shellPid{0};  // ShellExperienceHost, whose window events wake the worker
HWINEVENTHOOK g_eventHook = nullptr;  // owned by the card thread

constexpr UINT WM_APP_SNAPSHOT = WM_APP + 1;
constexpr UINT WM_APP_SETTINGS = WM_APP + 2;
constexpr UINT WM_APP_REHOOK = WM_APP + 3;  // ShellExperienceHost restarted: watch the new process
constexpr UINT WM_APP_AVATAR = WM_APP + 4;  // a downloaded avatar is ready (lParam: AvatarReady*)
// While a banner is up its toasts are read this often (typing, removed toasts). With no banner the worker sleeps
// until a window event and only checks every kIdleMs in case one was missed.
constexpr DWORD kPollMs = 100, kIdleMs = 2000;

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

struct AvatarReady {
    std::wstring id;    // toast
    std::wstring path;  // downloaded file
};

// Web avatars download on their own thread, so a slow server never holds up the cards. The thread keeps the mod's
// DLL loaded until it finishes, so unloading the mod in the meantime is safe.
DWORD WINAPI AvatarDownloadThread(LPVOID param) {
    auto job = static_cast<AvatarReady*>(param);
    WCHAR file[MAX_PATH];
    HRESULT hr = URLDownloadToCacheFileW(nullptr, job->path.c_str(), file, ARRAYSIZE(file), 0, nullptr);
    if (SUCCEEDED(hr)) {
        job->path = file;
        HWND window = g_window;
        if (!window || !PostMessage(window, WM_APP_AVATAR, 0, (LPARAM)job)) {
            delete job;
        }
    } else {
        Log(L"Downloading the avatar failed (%08lX): %s", hr, job->path.c_str());
        delete job;
    }
    HMODULE self = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&AvatarDownloadThread, &self);
    FreeLibraryAndExitThread(self, 0);
}

void StartAvatarDownload(const std::wstring& id, const std::wstring& url) {
    HMODULE self = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)&AvatarDownloadThread, &self)) {
        return;
    }
    HANDLE thread = CreateThread(nullptr, 0, AvatarDownloadThread, new AvatarReady{id, url}, 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        FreeLibrary(self);
    }
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

Com<IUIAutomationCondition>* NewIdCondition(IUIAutomation* uia, const wchar_t* automationId) {
    auto condition = new Com<IUIAutomationCondition>();
    VARIANT v{};
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocString(automationId);
    uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, v, condition->put());
    VariantClear(&v);
    return condition;
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

DWORD WINAPI WorkerThread(LPVOID) {
    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
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
        Com<IUIAutomationCondition> toastCondition;
        {
            VARIANT v{};
            v.vt = VT_BSTR;
            v.bstrVal = SysAllocString(L"FlexibleToastView");
            uia->CreatePropertyCondition(UIA_ClassNamePropertyId, v, toastCondition.put());
            VariantClear(&v);
        }
        Com<IUIAutomationCondition>* senderCondition = NewIdCondition(uia.p, L"SenderName");
        Com<IUIAutomationCondition>* titleCondition = NewIdCondition(uia.p, L"Title");
        Com<IUIAutomationCondition>* textCondition = NewIdCondition(uia.p, L"MessageText");
        Com<IUIAutomationCondition>* dismissCondition = NewIdCondition(uia.p, L"DismissButton");
        Com<IUIAutomationCondition>* verbCondition = NewIdCondition(uia.p, L"VerbButton");
        Com<IUIAutomationCondition>* editCondition = NewIdCondition(uia.p, L"Edit");

        // Toasts on the banner. The element is needed to click, dismiss or type; runtimeId only saves re-reading
        // the texts while it stays the same.
        struct Known {
            ToastInfo info;
            std::wstring runtimeId;
            IUIAutomationElement* element;
        };
        std::vector<Known> known;
        std::vector<ToastInfo> lastSent;

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

        bool busy = true;  // a banner was up on the last pass (or this is the first pass)
        HANDLE waits[] = {g_stopEvent, g_wakeEvent};
        while (WaitForMultipleObjects(2, waits, FALSE, busy ? kPollMs : kIdleMs) != WAIT_OBJECT_0) {
            if (!busy) {
                // ShellExperienceHost restarts (or starts late): its window events need a new hook.
                DWORD pid = BannerProcess();
                if (pid && pid != g_shellPid && g_window) {
                    PostMessage(g_window, WM_APP_REHOOK, 0, 0);
                }
            }
            std::vector<Command> commands;
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                commands.swap(g_commands);
            }
            for (auto& command : commands) {
                for (auto& k : known) {
                    if (k.info.id != command.id) {
                        continue;
                    }
                    IUIAutomationCondition* condition = command.action == Action::Dismiss  ? dismissCondition->p
                                                        : command.action == Action::Button ? verbCondition->p
                                                        : command.action == Action::FocusInput ? editCondition->p
                                                                                               : nullptr;
                    if (!condition) {
                        InvokeElement(k.element);
                        continue;
                    }
                    auto targets = findChildren(k.element, condition);
                    int index = command.action == Action::Button ? command.index : 0;
                    if (index < (int)targets.size()) {
                        if (command.action == Action::FocusInput) {
                            // Typing then goes straight to the hidden banner's reply box; the card shows its text.
                            targets[index]->SetFocus();
                        } else {
                            InvokeElement(targets[index]);
                        }
                    }
                    release(targets);
                }
            }

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
                    for (int i = 0; i < count; i++) {
                        IUIAutomationElement* element = nullptr;
                        if (FAILED(toasts->GetElement(i, &element)) || !element) {
                            continue;
                        }
                        std::wstring runtimeId = RuntimeId(element);
                        auto same = std::find_if(known.begin(), known.end(),
                                                 [&](const Known& k) { return k.runtimeId == runtimeId; });
                        ToastInfo info;
                        if (same != known.end()) {
                            info = same->info;
                        } else {
                            info.app = ChildName(element, senderCondition->p);
                            info.title = ChildName(element, titleCondition->p);
                            info.text = ChildName(element, textCondition->p);
                            std::wstring key = info.app + L"\n" + info.title + L"\n" + info.text + L"#";
                            int n = (int)std::count_if(current.begin(), current.end(), [&](const Known& k) {
                                return k.info.id.rfind(key, 0) == 0;
                            });
                            info.id = key + std::to_wstring(n);
                            auto old = std::find_if(known.begin(), known.end(),
                                                    [&](const Known& k) { return k.info.id == info.id; });
                            if (old != known.end()) {
                                info = old->info;
                            } else {
                                info.image = FindAvatar(info.title, info.text);
                                if (info.image.rfind(L"http", 0) == 0) {
                                    StartAvatarDownload(info.id, info.image);
                                    info.image.clear();
                                }
                                if (info.image.empty()) {
                                    info.image = AppIcon(info.app);
                                }
                                BSTR automationId = nullptr;
                                element->get_CurrentAutomationId(&automationId);
                                info.priority = automationId && wcsncmp(automationId, L"PriorityToastView", 17) == 0;
                                SysFreeString(automationId);
                                auto buttons = findChildren(element, verbCondition->p);
                                for (auto button : buttons) {
                                    BSTR name = nullptr;
                                    button->get_CurrentName(&name);
                                    info.buttons.push_back(name ? name : L"");
                                    SysFreeString(name);
                                }
                                release(buttons);
                                auto edits = findChildren(element, editCondition->p);
                                info.hasInput = !edits.empty();
                                release(edits);
                                info.placeholder = ChildName(element, editCondition->p);
                                Log(L"Toast %s: %s / %s [%s] %d buttons%s", info.app.c_str(), info.title.c_str(),
                                    info.text.c_str(), info.image.c_str(), (int)info.buttons.size(),
                                    info.hasInput ? L", reply box" : L"");
                            }
                        }
                        // The reply box changes while typing: its text and focus are read on every pass.
                        if (info.hasInput) {
                            auto edits = findChildren(element, editCondition->p);
                            if (!edits.empty()) {
                                Com<IUIAutomationValuePattern> value;
                                BSTR text = nullptr;
                                if (SUCCEEDED(edits[0]->GetCurrentPatternAs(UIA_ValuePatternId,
                                                                            __uuidof(IUIAutomationValuePattern),
                                                                            reinterpret_cast<void**>(value.put()))) &&
                                    value && SUCCEEDED(value->get_CurrentValue(&text))) {
                                    info.inputValue = text ? text : L"";
                                    SysFreeString(text);
                                }
                                BOOL focused = FALSE;
                                edits[0]->get_CurrentHasKeyboardFocus(&focused);
                                info.inputFocused = focused != FALSE;
                            }
                            release(edits);
                        }
                        current.push_back({info, runtimeId, element});
                    }
                }
            }
            for (auto& k : known) {
                k.element->Release();
            }
            known.swap(current);

            std::vector<ToastInfo> infos;
            for (auto& k : known) {
                infos.push_back(k.info);
            }
            bool changed = infos.size() != lastSent.size() ||
                           !std::equal(infos.begin(), infos.end(), lastSent.begin(), [](const ToastInfo& a, const ToastInfo& b) {
                               return a.id == b.id && a.inputValue == b.inputValue && a.inputFocused == b.inputFocused;
                           });
            if (changed) {
                {
                    std::lock_guard<std::mutex> lock(g_mutex);
                    g_snapshot = infos;
                }
                lastSent = infos;
                if (g_window) {
                    PostMessage(g_window, WM_APP_SNAPSHOT, 0, 0);
                }
            }
        }
        for (auto& k : known) {
            k.element->Release();
        }
        delete senderCondition;
        delete titleCondition;
        delete textCondition;
        delete dismissCondition;
        delete verbCondition;
        delete editCondition;
    }
    CoUninitialize();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Drawing: one layered, never-activated window in a bottom corner with a stack of cards:
// newest at the bottom, slides in from the right, fades out. Buttons and the reply box sit under the text.

constexpr float kWidth = 360;  // window width in DIPs; cards are kWidth - 2 * kPad wide
constexpr float kPad = 8;      // room around the stack (shadow) and gap between cards
constexpr float kCardPadX = 12, kCardPadY = 10, kAvatar = 40, kGap = 10, kRadius = 12;
constexpr float kFontSize = 13, kLine = kFontSize * 1.35f;
constexpr float kFieldHeight = 28, kRowGap = 8, kFieldRadius = 7, kButtonGap = 6;
constexpr DWORD kInMs = 280, kOutMs = 400;

// Mod settings (WindhawkModSettings). Sizes above are in CSS pixels; "size" scales all of them.
enum class ThemeChoice { Auto, Dark, Light };
struct Settings {
    ThemeChoice theme = ThemeChoice::Auto;
    float zoom = 1.3f;
    float maxLines = 4;  // longer messages make the card taller up to this; the rest is cut with an ellipsis
    bool cursorMonitor = true;
    bool top = false, left = false;  // corner
    bool showActions = true;
    bool highlightPriority = true;
    UINT32 priorityColor = 0xF0A020;  // reminders, alarms, calls
} g_settings;

std::wstring StringSetting(const wchar_t* name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

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
    std::wstring color = StringSetting(L"behavior.priorityColor");
    if (!color.empty() && color[0] == L'#') {
        color.erase(0, 1);
    }
    if (color.size() == 6 && color.find_first_not_of(L"0123456789abcdefABCDEF") == std::wstring::npos) {
        s.priorityColor = wcstoul(color.c_str(), nullptr, 16);
    }
    g_settings = s;
    Log(L"Settings: theme %d, size %d%%, %d lines, %s monitor, corner %s%s, actions %d, priority %d #%06X", (int)s.theme,
        (int)(s.zoom * 100 + 0.5f), (int)s.maxLines, s.cursorMonitor ? L"cursor" : L"primary", s.top ? L"top" : L"bottom",
        s.left ? L"Left" : L"Right", s.showActions, s.highlightPriority, s.priorityColor);
}

// Black cards in dark mode, white ones in light mode.
struct Theme {
    UINT32 card, cardHover, border, ink, field, fieldHover;
    float borderAlpha;
};
constexpr Theme kDark{0x000000, 0x121212, 0x3a3b3c, 0xe4e6eb, 0x1c1c1d, 0x2a2b2c, 0.9f};
constexpr Theme kLight{0xffffff, 0xf2f2f2, 0x808080, 0x050505, 0xf0f2f5, 0xe4e6eb, 0.25f};

bool SystemUsesLightTheme() {
    DWORD value = 0, size = sizeof(value);
    return RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                        L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS &&
           value != 0;
}

enum class Part { None, Body, Close, Button, Input };

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
    float contentHeight = 0;  // avatar and texts, without buttons and the reply box
    float height = 0;
    ULONGLONG shownAt = 0;
    ULONGLONG goneAt = 0;  // 0 while the system banner still has the toast
    float top = 0;         // last drawn position, for hit testing
};

std::vector<Card> g_cards;
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

float EaseOut(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return 1 - (1 - t) * (1 - t) * (1 - t);
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
    IWICFormatConverter* converter = nullptr;
    if (SUCCEEDED(g_wic->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad,
                                                   &decoder)) &&
        SUCCEEDED(decoder->GetFrame(0, &frame)) && SUCCEEDED(g_wic->CreateFormatConverter(&converter)) &&
        SUCCEEDED(converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0,
                                        WICBitmapPaletteTypeMedianCut))) {
        g_target->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap);
    }
    if (!bitmap) {
        Log(L"Loading the avatar failed: %s", path.c_str());
    }
    for (IUnknown* object : std::initializer_list<IUnknown*>{converter, frame, decoder}) {
        if (object) {
            object->Release();
        }
    }
    return bitmap;
}

void LayoutCard(Card& card) {
    float textWidth = kWidth - 2 * kPad - 2 * kCardPadX - kAvatar - kGap;
    std::wstring title = card.info.title.empty() ? card.info.app : card.info.title;
    g_dwrite->CreateTextLayout(title.c_str(), (UINT32)title.size(), g_titleFormat, textWidth, kLine, &card.title);
    g_dwrite->CreateTextLayout(card.info.text.c_str(), (UINT32)card.info.text.size(), g_textFormat, textWidth,
                               g_settings.maxLines * kLine, &card.text);
    DWRITE_TEXT_METRICS metrics{};
    float textHeight = 0;
    if (card.text && SUCCEEDED(card.text->GetMetrics(&metrics))) {
        textHeight = std::min(metrics.height, g_settings.maxLines * kLine);
    }
    card.contentHeight = std::max(kAvatar, kLine + textHeight);
    card.height = card.contentHeight + 2 * kCardPadY;
    if (card.info.hasInput) {
        card.height += kRowGap + kFieldHeight;
    }
    if (!card.info.buttons.empty()) {
        card.height += kRowGap + kFieldHeight;
    }
    card.avatar = LoadBitmap(card.info.image);
}

// Rows under the text, in card coordinates (left edge 0): the reply box, then the buttons side by side.
D2D1_RECT_F InputRect(const Card& card) {
    float top = kCardPadY + card.contentHeight + kRowGap;
    return {kCardPadX, top, kWidth - 2 * kPad - kCardPadX, top + kFieldHeight};
}

D2D1_RECT_F ButtonRect(const Card& card, int index) {
    float top = kCardPadY + card.contentHeight + kRowGap + (card.info.hasInput ? kFieldHeight + kRowGap : 0);
    int count = (int)card.info.buttons.size();
    float width = (kWidth - 2 * kPad - 2 * kCardPadX - (count - 1) * kButtonGap) / count;
    float left = kCardPadX + index * (width + kButtonGap);
    return {left, top, left + width, top + kFieldHeight};
}

void ApplySnapshot() {
    std::vector<ToastInfo> snapshot;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        snapshot = g_snapshot;
    }
    ULONGLONG now = GetTickCount64();
    for (auto& card : g_cards) {
        auto live = std::find_if(snapshot.begin(), snapshot.end(), [&](const ToastInfo& t) { return t.id == card.info.id; });
        if (live == snapshot.end()) {
            if (!card.goneAt) {
                card.goneAt = now;
            }
        } else {
            card.info.inputValue = live->inputValue;
            card.info.inputFocused = live->inputFocused;
        }
    }
    for (auto& info : snapshot) {
        bool have = std::any_of(g_cards.begin(), g_cards.end(), [&](const Card& c) { return c.info.id == info.id; });
        if (!have) {
            // A new stack follows the cursor's monitor and the current theme; a growing one stays where it is.
            if (std::all_of(g_cards.begin(), g_cards.end(), [](const Card& c) { return c.goneAt; })) {
                POINT cursor{};
                GetCursorPos(&cursor);
                g_monitor = g_settings.cursorMonitor ? MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY) : nullptr;
                bool light = g_settings.theme == ThemeChoice::Light ||
                             (g_settings.theme == ThemeChoice::Auto && SystemUsesLightTheme());
                g_theme = light ? kLight : kDark;
            }
            Card card;
            card.info = info;
            if (!g_settings.showActions) {
                card.info.buttons.clear();
                card.info.hasInput = false;
            }
            card.info.priority = card.info.priority && g_settings.highlightPriority;
            card.shownAt = now;
            LayoutCard(card);
            g_cards.push_back(card);
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
    float cardWidth = kWidth - 2 * kPad;
    D2D1_RECT_F rect{x, card.top, x + cardWidth, card.top + card.height};
    bool live = !card.goneAt;
    bool hovered = live && g_hover.card == index;

    // Soft shadow: a few widening, fading layers (box-shadow: 0 4px 14px rgba(0,0,0,.35)).
    for (int layer = 4; layer >= 1; layer--) {
        float grow = layer * 2.0f;
        FillRounded(D2D1_RECT_F{rect.left - grow, rect.top - grow + 4, rect.right + grow, rect.bottom + grow + 4},
                    kRadius + grow, 0x000000, 0.06f * alpha);
    }
    FillRounded(rect, kRadius, hovered && g_hover.part == Part::Body ? theme.cardHover : theme.card, alpha);
    if (card.info.priority) {
        // Reminders, alarms and calls: accent outline and a bar along the left edge.
        StrokeRounded(rect, kRadius, g_settings.priorityColor, alpha, 1.5f);
        FillRounded(D2D1_RECT_F{rect.left + 4, rect.top + kRadius, rect.left + 7, rect.bottom - kRadius}, 1.5f,
                    g_settings.priorityColor, alpha);
    } else {
        StrokeRounded(rect, kRadius, theme.border, theme.borderAlpha * alpha, 1);
    }

    // Avatar: the toast's image cropped to a circle (object-fit: cover), otherwise the app's first letter.
    float cx = rect.left + kCardPadX + kAvatar / 2, cy = rect.top + kCardPadY + card.contentHeight / 2;
    ID2D1BitmapBrush* avatarBrush = nullptr;
    if (card.avatar) {
        D2D1_SIZE_F size = card.avatar->GetSize();
        float fit = kAvatar / std::min(size.width, size.height);
        D2D1_BITMAP_BRUSH_PROPERTIES brushProps{D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP,
                                                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR};
        D2D1_BRUSH_PROPERTIES common{alpha, D2D1::Matrix3x2F::Scale(fit, fit) *
                                                D2D1::Matrix3x2F::Translation(cx - size.width * fit / 2,
                                                                              cy - size.height * fit / 2)};
        g_target->CreateBitmapBrush(card.avatar, &brushProps, &common, &avatarBrush);
    }
    D2D1_ELLIPSE circle{{cx, cy}, kAvatar / 2, kAvatar / 2};
    if (avatarBrush) {
        g_target->FillEllipse(&circle, avatarBrush);
        avatarBrush->Release();
    } else {
        if (ID2D1SolidColorBrush* fill = Brush(card.info.priority ? g_settings.priorityColor : 0x808080, 0.3f * alpha)) {
            g_target->FillEllipse(&circle, fill);
            fill->Release();
        }
        std::wstring source = card.info.app.empty() ? card.info.title : card.info.app;
        if (!source.empty()) {
            std::wstring letter(1, (wchar_t)towupper(source[0]));
            DrawString(letter, g_letterFormat, {cx - kAvatar / 2, cy - kAvatar / 2, cx + kAvatar / 2, cy + kAvatar / 2},
                       theme.ink, alpha);
        }
    }

    float textLeft = rect.left + kCardPadX + kAvatar + kGap;
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

    // Reply box: shows what is typed into the hidden banner's box, with a caret while it has the keyboard.
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
        FillRounded(button, kFieldRadius, over ? theme.fieldHover : theme.field, alpha);
        DrawString(card.info.buttons[i], g_buttonFormat, button, theme.ink, alpha);
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
    }
}

// Returns whether anything is still animating.
bool Render() {
    ULONGLONG now = GetTickCount64();
    g_cards.erase(std::remove_if(g_cards.begin(), g_cards.end(),
                                 [&](Card& c) {
                                     bool done = c.goneAt && now - c.goneAt >= kOutMs;
                                     if (done) {
                                         ReleaseCard(c);
                                     }
                                     return done;
                                 }),
                  g_cards.end());
    if (g_cards.empty()) {
        ShowWindow(g_window, SW_HIDE);
        g_hover = {};
        return false;
    }

    float scale = Scale();
    float heightDip = kPad;
    for (auto& card : g_cards) {
        heightDip += card.height + kPad;
    }
    SIZE size{(LONG)(kWidth * scale + 0.5f), (LONG)(heightDip * scale + 0.5f)};
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

    bool animating = false;
    float cardWidth = kWidth - 2 * kPad;
    float y = kPad;
    // Newest card next to the screen edge: at the bottom in bottom corners, at the top in top corners.
    for (size_t n = 0; n < g_cards.size(); n++) {
        size_t i = g_settings.top ? g_cards.size() - 1 - n : n;
        Card& card = g_cards[i];
        card.top = y;
        float in = EaseOut((now - card.shownAt) / (float)kInMs);
        float alpha = card.goneAt ? 1 - std::clamp((now - card.goneAt) / (float)kOutMs, 0.0f, 1.0f) : 1.0f;
        animating |= in < 1 || card.goneAt;
        float slide = (1 - in) * cardWidth * 1.1f;
        DrawCard(card, (int)i, kPad + (g_settings.left ? -slide : slide), alpha);
        y += card.height + kPad;
    }
    g_target->EndDraw();

    RECT work = WorkArea();
    POINT dst{g_settings.left ? work.left : work.right - size.cx, g_settings.top ? work.top : work.bottom - size.cy};
    POINT src{0, 0};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_window, nullptr, &dst, &size, g_memDC, &src, 0, &blend, ULW_ALPHA);
    if (!IsWindowVisible(g_window)) {
        ShowWindow(g_window, SW_SHOWNOACTIVATE);
    }
    SetWindowPos(g_window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    return animating;
}

void Redraw() {
    if (Render()) {
        SetTimer(g_window, 1, 15, nullptr);
    } else {
        KillTimer(g_window, 1);
    }
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
        if (card.goneAt || x < kPad || x >= kWidth - kPad || y < card.top || y >= card.top + card.height) {
            continue;
        }
        float cardX = x - kPad, cardY = y - card.top;
        float bx = kWidth - 2 * kPad - 14, by = 14;
        if ((cardX - bx) * (cardX - bx) + (cardY - by) * (cardY - by) <= 12 * 12) {
            return {(int)i, Part::Close};
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

void SendCommand(const std::wstring& id, Action action, int index = 0) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_commands.push_back({id, action, index});
    SetEvent(g_wakeEvent);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_APP_SNAPSHOT:
            ApplySnapshot();
            Redraw();
            return 0;
        case WM_TIMER:
            Redraw();
            return 0;
        case WM_APP_AVATAR: {
            std::unique_ptr<AvatarReady> ready(reinterpret_cast<AvatarReady*>(lParam));
            for (auto& card : g_cards) {
                if (card.info.id == ready->id && !card.avatar) {
                    card.info.image = ready->path;
                    card.avatar = LoadBitmap(ready->path);
                }
            }
            Redraw();
            return 0;
        }
        case WM_APP_REHOOK:
            if (g_eventHook) {
                UnhookWinEvent(g_eventHook);
            }
            g_eventHook = HookBannerEvents();
            Log(L"Watching ShellExperienceHost %lu", g_shellPid.load());
            return 0;
        case WM_APP_SETTINGS:
            // Size and lines change the layout of the cards already shown.
            LoadSettings();
            for (auto& card : g_cards) {
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
            if (!(hit == g_hover)) {
                g_hover = hit;
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                Redraw();
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            g_hover = {};
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
                case Part::Close:
                    SendCommand(card.info.id, hit.part == Part::Body ? Action::Open : Action::Dismiss);
                    card.goneAt = GetTickCount64();
                    break;
                case Part::Button:
                    // The card goes when the banner drops the toast (a "snooze" may keep it).
                    SendCommand(card.info.id, Action::Button, hit.index);
                    break;
                case Part::Input:
                    SendCommand(card.info.id, Action::FocusInput);
                    break;
                case Part::None:
                    break;
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
    for (IUnknown* object : std::initializer_list<IUnknown*>{g_target, g_fieldFormat, g_buttonFormat, g_letterFormat,
                                                             g_textFormat, g_titleFormat, g_dwrite, g_wic, g_d2d}) {
        if (object) {
            object->Release();
        }
    }
}

DWORD WINAPI UiThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (CreateResources()) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = L"CompactNotifications";
        RegisterClassW(&wc);
        g_window = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, wc.lpszClassName,
                                   L"Compact Notifications", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);
        Log(L"Card window %p (error %lu)", g_window, g_window ? 0 : GetLastError());
        // Out-of-context window events arrive through this thread's message loop.
        g_eventHook = HookBannerEvents();
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0) > 0) {
            DispatchMessage(&msg);
        }
        if (g_eventHook) {
            UnhookWinEvent(g_eventHook);
            g_eventHook = nullptr;
        }
        DestroyWindow(g_window);
        g_window = nullptr;
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
    } else {
        Log(L"Direct2D/DirectWrite unavailable");
    }
    ReleaseResources();
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_uiThread = CreateThread(nullptr, 0, UiThread, nullptr, 0, &g_uiThreadId);
    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    return g_stopEvent && g_wakeEvent && g_uiThread && g_workerThread;
}

void WhTool_ModSettingsChanged() {}

void WhTool_ModUninit() {
    SetEvent(g_stopEvent);
    // The card window goes first, so nothing frozen stays on the screen even if the worker is stuck.
    PostThreadMessage(g_uiThreadId, WM_QUIT, 0, 0);
    if (WaitForSingleObject(g_uiThread, 3000) != WAIT_OBJECT_0) {
        Log(L"The card thread didn't stop");
    }
    if (WaitForSingleObject(g_workerThread, 5000) != WAIT_OBJECT_0) {
        Log(L"The reading thread didn't stop");
    }
}


////////////////////////////////////////////////////////////////////////////////
// Process roles: ShellExperienceHost only hides the banner; explorer hides it too (some builds move it from there)
// and one explorer process also reads the toasts and draws the cards. Windhawk's separate "tool mod" process didn't
// survive reloads, and explorer is always running anyway.

enum class Role { None, Shell, ShellWithCards };
Role g_role = Role::None;
HANDLE g_cardsMutex = nullptr;

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
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook, (void**)&SetWindowPos_Original);
    g_role = Role::Shell;
    // With "open folders in a separate process" there are several explorers: only the first one draws.
    if (explorer) {
        g_cardsMutex = CreateMutex(nullptr, TRUE, L"windhawk_" WH_MOD_ID L"_cards");
        if (g_cardsMutex && GetLastError() != ERROR_ALREADY_EXISTS) {
            g_role = Role::ShellWithCards;
        } else if (g_cardsMutex) {
            CloseHandle(g_cardsMutex);
            g_cardsMutex = nullptr;
        }
    }
    Log(L"Loaded in %s, %s", base, g_role == Role::ShellWithCards ? L"drawing cards" : L"hiding only");
    return TRUE;
}

void Wh_ModAfterInit() {
    HideExistingBanner();
    LoadSettings();
    if (g_role == Role::ShellWithCards && !WhTool_ModInit()) {
        Log(L"Starting the cards failed");
    }
}

void Wh_ModSettingsChanged() {
    if (g_window) {
        PostMessage(g_window, WM_APP_SETTINGS, 0, 0);
    }
}

void Wh_ModUninit() {
    if (g_role == Role::ShellWithCards) {
        WhTool_ModUninit();
        ReleaseMutex(g_cardsMutex);
        CloseHandle(g_cardsMutex);
    }
    // The hook goes away with the mod and Windows places the next banner on the screen again.
}
