// ==WindhawkMod==
// @id              phone-panel-for-nectarlink
// @name            Phone panel for NectarLink
// @description     Unofficial. Shows your NectarLink-paired Android phone in the Windows 11 Start menu, like Phone Link
// @version         1.0.0
// @author          vampirakos
// @github          https://github.com/vampirakos
// @homepage        https://github.com/vampirakos/phone-panel-for-nectarlink
// @license         MIT
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lwinhttp -lshell32 -ladvapi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Phone panel for NectarLink

**Unofficial.** This mod is not made by, affiliated with or endorsed by the
NectarLink project. It works with [NectarLink](https://github.com/thtbee/nectarlink),
the free, open-source app that links an Android phone with a Windows PC.

It puts your phone in the Start menu's phone panel (the one Phone Link uses):

* your phone's name, connection and battery
* quick actions: Find phone, Clipboard, Take photo, Scan doc, Play on phone, Send files
* **Recent**: the last items from NectarLink's Timeline (click to open)
* **Clipboard**: the last clipboard items (click to copy again)
* Open NectarLink and NectarLink settings

## Requirements

* Windows 11 (x64) and the NectarLink app for Windows, paired with your phone.
* In NectarLink, turn on **Settings → Local automation API**. Without it,
  Find phone / Clipboard / Take photo / Send files and the footer buttons still
  work; status, Recent, Clipboard, Scan doc and Play on phone need it.
* Keep **Show mobile device in Start** turned on in
  Windows Settings → Personalization → Start.

## How it works

The panel shows NectarLink instead of Phone Link. By default it only does so
while NectarLink is installed and has a paired phone; otherwise Phone Link's
panel is left alone (the "Replace Phone Link's panel" setting can force it).
The phone button next to the search box shows and hides it.

## Privacy and safety

* The mod only talks to NectarLink on this PC (`127.0.0.1`), using the token
  NectarLink writes to `%LOCALAPPDATA%\Nectarlink\api-token.json`. Nothing is
  sent anywhere else.
* Clicking a Recent file opens it only if it's a common photo, video, audio or
  document type. Anything else (programs, scripts, shortcuts...) is shown in
  File Explorer instead of being opened. Links open only if they're http(s).
* Turning the mod off restores the Start menu and Phone Link's panel.

## If the panel doesn't appear

Open Windhawk's log for this mod (Details → Log). If it can't find where to
attach, it prints the named elements of the Start menu.

NectarLink is a trademark of its project; the name is used here only to say
what this mod works with.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- takeOver: auto
  $name: Replace Phone Link's panel
  $description: Auto = only while NectarLink is installed and has a paired phone
  $options:
  - auto: Auto
  - always: Always
- recentCount: 4
  $name: Recent items to show (0 hides the section)
- clipCount: 3
  $name: Clipboard items to show (0 hides the section)
- deviceName: ""
  $name: Phone name
  $description: Which paired phone to show. Empty = the first connected one
- exePath: ""
  $name: NectarLink exe path
  $description: Leave empty to find nectarlink-desktop.exe automatically
- refreshSeconds: 5
  $name: Refresh interval (seconds)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <winhttp.h>

#include <algorithm>
#include <atomic>
#include <ctime>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#undef GetCurrentTime
#undef GetObject  // windows.h macro clashes with IJsonValue::GetObject

#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace json = winrt::Windows::Data::Json;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct Settings {
    bool takeOverAlways = false;
    int recentCount = 4;
    int clipCount = 3;
    std::wstring deviceName;
    std::wstring exePath;
    int refreshSeconds = 5;
} g_settings;

static std::wstring GetStr(PCWSTR name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring s = v ? v : L"";
    Wh_FreeStringSetting(v);
    return s;
}

static void LoadSettings() {
    g_settings.takeOverAlways = GetStr(L"takeOver") == L"always";
    g_settings.recentCount = std::clamp(Wh_GetIntSetting(L"recentCount"), 0, 10);
    g_settings.clipCount = std::clamp(Wh_GetIntSetting(L"clipCount"), 0, 10);
    g_settings.deviceName = GetStr(L"deviceName");
    g_settings.exePath = GetStr(L"exePath");
    g_settings.refreshSeconds = Wh_GetIntSetting(L"refreshSeconds");
    if (g_settings.refreshSeconds < 1) g_settings.refreshSeconds = 5;
    g_settings.refreshSeconds = std::min(g_settings.refreshSeconds, 3600);  // also keeps *1000 from overflowing
}

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

static std::wstring Utf8ToWide(const std::string& s) {
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), n);
    return w;
}

static std::wstring ExpandEnv(PCWSTR s) {
    wchar_t buf[MAX_PATH * 2];
    DWORD n = ExpandEnvironmentStringsW(s, buf, ARRAYSIZE(buf));
    return (n && n <= ARRAYSIZE(buf)) ? std::wstring(buf) : std::wstring();
}

static bool ReadFileUtf8(const std::wstring& path, std::string& out) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    char buf[4096];
    DWORD read = 0;
    out.clear();
    while (ReadFile(h, buf, sizeof(buf), &read, nullptr) && read > 0) out.append(buf, read);
    CloseHandle(h);
    return true;
}

static bool IsType(const json::JsonObject& o, PCWSTR key, json::JsonValueType t) {
    return o.HasKey(key) && o.GetNamedValue(key).ValueType() == t;
}

static std::wstring Str(const json::JsonObject& o, PCWSTR key) {
    return IsType(o, key, json::JsonValueType::String) ? std::wstring(o.GetNamedString(key)) : std::wstring();
}

static int64_t Num(const json::JsonObject& o, PCWSTR key) {
    return IsType(o, key, json::JsonValueType::Number) ? (int64_t)o.GetNamedNumber(key) : 0;
}

static std::wstring Ago(int64_t ts) {
    if (ts <= 0) return L"";
    int64_t d = (int64_t)time(nullptr) - ts;
    if (d < 60) return L"just now";
    if (d < 3600) return std::to_wstring(d / 60) + L" min ago";
    if (d < 86400) return std::to_wstring(d / 3600) + L" h ago";
    return std::to_wstring(d / 86400) + L" d ago";
}

// ---------------------------------------------------------------------------
// NectarLink's Local API
// ---------------------------------------------------------------------------

struct ApiConfig {
    int port = 0;
    std::wstring token;
};

// Reads %LOCALAPPDATA%\Nectarlink\api-token.json. Needs an initialized apartment (uses WinRT JSON).
static bool LoadApiConfig(ApiConfig& cfg, std::wstring& problem) {
    std::string raw;
    if (!ReadFileUtf8(ExpandEnv(L"%LOCALAPPDATA%\\Nectarlink\\api-token.json"), raw)) {
        problem = L"Turn on Settings → Local automation API in NectarLink to see live status here.";
        return false;
    }
    json::JsonObject o{nullptr};
    if (!json::JsonObject::TryParse(Utf8ToWide(raw), o)) {
        problem = L"Couldn't read NectarLink's api-token.json.";
        return false;
    }
    cfg.port = (int)o.GetNamedNumber(L"port", 53320);
    if (cfg.port < 1 || cfg.port > 65535) cfg.port = 53320;  // port 0 would mean "default HTTP port" to WinHTTP
    cfg.token = Str(o, L"token");
    return true;
}

// Returns the HTTP status, or 0 if NectarLink isn't reachable.
static DWORD HttpRequest(const ApiConfig& cfg, PCWSTR method, PCWSTR path, const std::string& body,
                         std::string& out) {
    DWORD status = 0;
    HINTERNET session = WinHttpOpen(L"PhonePanelForNectarLink/1.0", WINHTTP_ACCESS_TYPE_NO_PROXY,
                                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) return 0;
    WinHttpSetTimeouts(session, 1000, 1000, 2000, 4000);
    HINTERNET conn = WinHttpConnect(session, L"127.0.0.1", (INTERNET_PORT)cfg.port, 0);
    HINTERNET req = conn ? WinHttpOpenRequest(conn, method, path, nullptr, WINHTTP_NO_REFERER,
                                              WINHTTP_DEFAULT_ACCEPT_TYPES, 0)
                         : nullptr;
    if (req) {
        std::wstring headers = L"Authorization: Bearer " + cfg.token + L"\r\n";
        if (!body.empty()) headers += L"Content-Type: application/json\r\n";
        if (WinHttpSendRequest(req, headers.c_str(), (DWORD)-1L, body.empty() ? WINHTTP_NO_REQUEST_DATA : (LPVOID)body.data(),
                               (DWORD)body.size(), (DWORD)body.size(), 0) &&
            WinHttpReceiveResponse(req, nullptr)) {
            DWORD size = sizeof(status);
            WinHttpQueryHeaders(req, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX);
            out.clear();
            DWORD avail = 0;
            while (WinHttpQueryDataAvailable(req, &avail) && avail > 0) {
                std::string chunk(avail, '\0');
                DWORD got = 0;
                if (!WinHttpReadData(req, chunk.data(), avail, &got) || got == 0) break;
                out.append(chunk.data(), got);
            }
        }
        WinHttpCloseHandle(req);
    }
    if (conn) WinHttpCloseHandle(conn);
    WinHttpCloseHandle(session);
    return status;
}

static bool GetJson(const ApiConfig& cfg, PCWSTR path, json::JsonObject& root) {
    std::string body;
    if (HttpRequest(cfg, L"GET", path, "", body) != 200) return false;
    return json::JsonObject::TryParse(Utf8ToWide(body), root);
}

// ---------------------------------------------------------------------------
// Status (filled by the worker thread, shown by the XAML thread)
// ---------------------------------------------------------------------------

struct RecentItem {
    std::wstring kind, direction, title, detail, target;
    int64_t ts = 0;
};

struct ClipItem {
    std::wstring kind, text, device;
    bool incoming = false;
    int64_t ts = 0;
};

struct PhoneStatus {
    bool apiOk = false;
    bool hasDevice = false;
    std::wstring name;
    bool online = false;
    std::wstring link;
    int battery = -1;
    bool charging = false;
    std::wstring problem;
    std::vector<RecentItem> recent;
    std::vector<ClipItem> clips;
};

std::mutex g_statusMutex;
PhoneStatus g_status;
std::wstring g_flash;          // short message after an action ("Copied", errors)
ULONGLONG g_flashUntil = 0;
std::atomic<bool> g_statusDirty{true};
std::atomic<bool> g_takeOverAllowed{false};  // may we replace Phone Link's panel?

static void Flash(const std::wstring& msg) {
    {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        g_flash = msg;
        g_flashUntil = GetTickCount64() + 4000;
    }
    g_statusDirty = true;
}

static PhoneStatus FetchStatus() {
    PhoneStatus s;
    ApiConfig cfg;
    if (!LoadApiConfig(cfg, s.problem)) return s;

    std::string body;
    DWORD code = HttpRequest(cfg, L"GET", L"/v1/devices", "", body);
    if (code == 0) {
        s.problem = L"NectarLink isn't running.";
        return s;
    }
    json::JsonObject root{nullptr};
    if (code != 200 || !json::JsonObject::TryParse(Utf8ToWide(body), root) ||
        !IsType(root, L"devices", json::JsonValueType::Array)) {
        s.problem = L"NectarLink's API answered " + std::to_wstring(code) + L".";
        return s;
    }
    s.apiOk = true;

    json::JsonObject pick{nullptr};
    for (auto const& v : root.GetNamedArray(L"devices")) {
        if (v.ValueType() != json::JsonValueType::Object) continue;
        auto d = v.GetObject();
        std::wstring name = Str(d, L"name");
        if (!g_settings.deviceName.empty()) {
            if (_wcsicmp(name.c_str(), g_settings.deviceName.c_str()) == 0) { pick = d; break; }
            continue;
        }
        if (!pick) pick = d;
        if (d.GetNamedBoolean(L"online", false)) { pick = d; break; }
    }
    if (pick) {
        s.hasDevice = true;
        s.name = Str(pick, L"name");
        if (s.name.empty()) s.name = L"Phone";
        s.online = pick.GetNamedBoolean(L"online", false);
        s.link = Str(pick, L"link");
        if (IsType(pick, L"battery", json::JsonValueType::Object)) {
            auto b = pick.GetNamedObject(L"battery");
            if (IsType(b, L"level", json::JsonValueType::Number)) s.battery = (int)b.GetNamedNumber(L"level");
            s.charging = b.GetNamedBoolean(L"charging", false);
        }
    }

    if (g_settings.recentCount > 0) {
        std::wstring path = L"/v1/timeline?limit=" + std::to_wstring(g_settings.recentCount);
        json::JsonObject t{nullptr};
        if (GetJson(cfg, path.c_str(), t) && IsType(t, L"items", json::JsonValueType::Array)) {
            for (auto const& v : t.GetNamedArray(L"items")) {
                if (v.ValueType() != json::JsonValueType::Object) continue;
                auto o = v.GetObject();
                RecentItem r;
                r.kind = Str(o, L"kind");
                r.direction = Str(o, L"direction");
                r.title = Str(o, L"title");
                r.detail = Str(o, L"detail");
                r.target = Str(o, L"target");
                r.ts = Num(o, L"timestamp");
                s.recent.push_back(r);
                if ((int)s.recent.size() >= g_settings.recentCount) break;
            }
        }
    }

    if (g_settings.clipCount > 0) {
        json::JsonObject c{nullptr};
        if (GetJson(cfg, L"/v1/clipboard", c) && IsType(c, L"history", json::JsonValueType::Array)) {
            for (auto const& v : c.GetNamedArray(L"history")) {
                if (v.ValueType() != json::JsonValueType::Object) continue;
                auto o = v.GetObject();
                ClipItem ci;
                ci.kind = Str(o, L"kind");
                ci.text = Str(o, L"preview");
                ci.device = Str(o, L"deviceName");
                ci.incoming = o.GetNamedBoolean(L"incoming", false);
                ci.ts = Num(o, L"timestamp");
                s.clips.push_back(ci);
                if ((int)s.clips.size() >= g_settings.clipCount) break;
            }
        }
    }
    return s;
}

// ---------------------------------------------------------------------------
// Run code on the Start menu's XAML thread
// ---------------------------------------------------------------------------

using RunProc = void(WINAPI*)(PVOID);
struct RunParam { RunProc proc; PVOID param; };
static UINT g_runMsg = 0;

static LRESULT CALLBACK RunHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_runMsg) {
            auto* p = (RunParam*)cwp->lParam;
            p->proc(p->param);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool RunFromWindowThread(HWND hWnd, RunProc proc, PVOID param) {
    if (!g_runMsg) g_runMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_phone-panel-for-nectarlink");
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) { proc(param); return true; }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunHookProc, nullptr, tid);
    if (!hook) return false;
    RunParam p{proc, param};
    SendMessageW(hWnd, g_runMsg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

static HWND FindStartCoreWindow() {
    struct Ctx { DWORD pid; HWND found; } ctx{GetCurrentProcessId(), nullptr};
    EnumWindows(
        [](HWND h, LPARAM lp) -> BOOL {
            auto* c = (Ctx*)lp;
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (pid != c->pid) return TRUE;
            wchar_t cls[64];
            if (GetClassNameW(h, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
                c->found = h;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&ctx);
    return ctx.found;
}

static HWND g_coreWnd = nullptr;

// ---------------------------------------------------------------------------
// Actions
// ---------------------------------------------------------------------------

static std::wstring ResolveExe() {
    if (!g_settings.exePath.empty()) return g_settings.exePath;
    wchar_t dir[MAX_PATH];
    DWORD size = sizeof(dir);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, L"Software\\Nectarlink", L"InstallDir", RRF_RT_REG_SZ, nullptr, dir,
                     &size) == ERROR_SUCCESS) {
        return std::wstring(dir) + L"\\nectarlink-desktop.exe";
    }
    return ExpandEnv(L"%ProgramFiles%\\Nectarlink\\nectarlink-desktop.exe");
}

static void ShellOpen(const std::wstring& file, const std::wstring& args = L"") {
    AllowSetForegroundWindow(ASFW_ANY);  // let the opened app come to the front
    SHELLEXECUTEINFOW sei{sizeof(sei)};
    sei.fMask = SEE_MASK_FLAG_NO_UI;
    sei.lpFile = file.c_str();
    sei.lpParameters = args.empty() ? nullptr : args.c_str();
    sei.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&sei)) Wh_Log(L"Couldn't open %s (error %u)", file.c_str(), GetLastError());
}

// task empty = just show NectarLink's window; otherwise a Jump List task (--task).
static void RunTask(const std::wstring& task) {
    ShellOpen(ResolveExe(), task.empty() ? L"" : L"--task " + task);
}

std::atomic<int> g_actionsRunning{0};

// POST to the Local API on a background thread; shows a short message on failure.
static void PostAction(std::wstring path, std::string body) {
    g_actionsRunning++;
    std::thread([path, body] {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        ApiConfig cfg;
        std::wstring problem;
        if (!LoadApiConfig(cfg, problem)) {
            Flash(problem);
        } else {
            std::string out;
            DWORD code = HttpRequest(cfg, L"POST", path.c_str(), body, out);
            if (code == 0) Flash(L"NectarLink isn't running.");
            else if (out.find("nothing_playing") != std::string::npos) Flash(L"Nothing is playing on this PC.");
            else if (out.find("no_phone") != std::string::npos || code == 409) Flash(L"Your phone isn't connected.");
            else if (code != 200) Flash(L"That didn't work (error " + std::to_wstring(code) + L").");
        }
        winrt::uninit_apartment();
        g_actionsRunning--;
    }).detach();
}

// Files from the phone open directly only if they're a plain media or document type.
static bool IsSafeToOpen(const std::wstring& path) {
    static const wchar_t* kSafe[] = {
        L".jpg", L".jpeg", L".png", L".gif", L".webp", L".heic", L".heif", L".bmp", L".tif", L".tiff", L".avif",
        L".mp4", L".mov", L".mkv", L".webm", L".3gp", L".avi", L".m4v",
        L".mp3", L".m4a", L".aac", L".wav", L".ogg", L".opus", L".flac", L".amr",
        L".pdf", L".txt", L".md", L".csv", L".rtf", L".odt", L".ods", L".odp",
        L".doc", L".docx", L".xls", L".xlsx", L".ppt", L".pptx", L".zip",
    };
    size_t dot = path.find_last_of(L'.');
    size_t slash = path.find_last_of(L"\\/");
    if (dot == std::wstring::npos || (slash != std::wstring::npos && dot < slash)) return false;
    std::wstring ext = path.substr(dot);
    for (auto e : kSafe) {
        if (_wcsicmp(ext.c_str(), e) == 0) return true;
    }
    return false;
}

static void OpenRecent(const RecentItem& r) {
    std::wstring target = r.target.substr(0, r.target.find(L'\n'));  // several paths are newline-separated
    while (!target.empty() && (target.back() == L'\r' || target.back() == L' ')) target.pop_back();
    if (target.rfind(L"http://", 0) == 0 || target.rfind(L"https://", 0) == 0) {
        ShellOpen(target);
        return;
    }
    DWORD attrs = target.empty() ? INVALID_FILE_ATTRIBUTES : GetFileAttributesW(target.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        RunTask(L"open-timeline");
    } else if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
        ShellOpen(L"explorer.exe", L"\"" + target + L"\"");
    } else if (IsSafeToOpen(target)) {
        ShellOpen(target);
    } else {
        ShellOpen(L"explorer.exe", L"/select,\"" + target + L"\"");  // show it, don't run it
    }
}

static void CopyText(const std::wstring& text) {
    if (text.empty() || !OpenClipboard(g_coreWnd)) return;
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    if (HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, bytes)) {
        void* mem = GlobalLock(h);
        if (mem) {
            memcpy(mem, text.c_str(), bytes);
            GlobalUnlock(h);
            if (SetClipboardData(CF_UNICODETEXT, h)) h = nullptr;  // the clipboard owns it now
        }
        if (h) GlobalFree(h);
    }
    CloseClipboard();
    Flash(L"Copied to clipboard");
}

// ---------------------------------------------------------------------------
// The panel (XAML thread only)
// ---------------------------------------------------------------------------

enum class ActKind { Task, Api };
struct ActionDef { PCWSTR name; PCWSTR glyph; PCWSTR label; ActKind kind; PCWSTR arg; PCWSTR body; };
static const ActionDef kActions[] = {
    {L"NlA0", L"&#xEA8F;", L"Find phone", ActKind::Task, L"find-phone", nullptr},
    {L"NlA1", L"&#xE77F;", L"Clipboard", ActKind::Task, L"send-clipboard", nullptr},
    {L"NlA2", L"&#xE722;", L"Take photo", ActKind::Task, L"take-photo", nullptr},
    {L"NlA3", L"&#xE8FE;", L"Scan doc", ActKind::Api, L"/v1/camera", L"{\"mode\":\"scan\"}"},
    {L"NlA4", L"&#xE768;", L"Play on phone", ActKind::Api, L"/v1/play-on-phone", L"{}"},
    {L"NlA5", L"&#xE898;", L"Send files", ActKind::Task, L"send-files", nullptr},
};

static const wchar_t kNs[] =
    L" xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\""
    L" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\"";
static const wchar_t kIconFont[] = L"Segoe Fluent Icons,Segoe MDL2 Assets";

static std::wstring BuildContentXaml(bool withNs) {
    std::wstring font = kIconFont;
    std::wstring tiles;
    for (int i = 0; i < (int)ARRAYSIZE(kActions); i++) {
        auto& a = kActions[i];
        tiles += L"<Button x:Name=\"" + std::wstring(a.name) + L"\" Grid.Row=\"" + std::to_wstring(i / 3) +
                 L"\" Grid.Column=\"" + std::to_wstring(i % 3) +
                 L"\" HorizontalAlignment=\"Stretch\" Height=\"64\" CornerRadius=\"6\" Padding=\"2\">"
                 L"<StackPanel Spacing=\"6\" HorizontalAlignment=\"Center\">"
                 L"<FontIcon Glyph=\"" + a.glyph + L"\" FontSize=\"17\" FontFamily=\"" + font + L"\"/>"
                 L"<TextBlock Text=\"" + a.label + L"\" FontSize=\"11\" HorizontalAlignment=\"Center\" "
                 L"TextTrimming=\"CharacterEllipsis\"/>"
                 L"</StackPanel></Button>";
    }
    return L"<Grid" + std::wstring(withNs ? kNs : L"") + LR"xaml( Background="Transparent">
  <Grid.RowDefinitions>
    <RowDefinition Height="*"/>
    <RowDefinition Height="Auto"/>
  </Grid.RowDefinitions>
  <StackPanel Margin="12,22,12,8">
    <FontIcon x:Name="NlPhoneIcon" Glyph="&#xE8EA;" FontSize="48" HorizontalAlignment="Center"
              Foreground="{ThemeResource SystemControlForegroundAccentBrush}" FontFamily=")xaml" + font + LR"xaml("/>
    <TextBlock x:Name="NlName" Text="NectarLink" FontSize="14" FontWeight="SemiBold" Margin="0,10,0,0"
               HorizontalAlignment="Center" TextTrimming="CharacterEllipsis"/>
    <StackPanel Orientation="Horizontal" HorizontalAlignment="Center" Spacing="6" Opacity="0.75" Height="24" Margin="0,2,0,0">
      <TextBlock x:Name="NlStatus" Text="Loading..." FontSize="12" VerticalAlignment="Center" TextLineBounds="Tight"
                 TextTrimming="CharacterEllipsis"/>
      <FontIcon x:Name="NlBattIcon" FontSize="22" VerticalAlignment="Center" Visibility="Collapsed"
                FontFamily=")xaml" + font + LR"xaml("/>
      <TextBlock x:Name="NlBattText" FontSize="12" VerticalAlignment="Center" TextLineBounds="Tight" Visibility="Collapsed"/>
    </StackPanel>
    <TextBlock x:Name="NlHint" FontSize="11" Opacity="0.7" TextWrapping="Wrap" TextAlignment="Center"
               HorizontalAlignment="Center" Margin="12,8,12,0" Visibility="Collapsed"/>
    <Grid RowSpacing="6" ColumnSpacing="6" Margin="0,16,0,0">
      <Grid.ColumnDefinitions>
        <ColumnDefinition Width="*"/><ColumnDefinition Width="*"/><ColumnDefinition Width="*"/>
      </Grid.ColumnDefinitions>
      <Grid.RowDefinitions>
        <RowDefinition Height="Auto"/><RowDefinition Height="Auto"/>
      </Grid.RowDefinitions>
      )xaml" + tiles + LR"xaml(
    </Grid>
    <StackPanel x:Name="NlRecentSection" Visibility="Collapsed" Margin="0,20,0,0">
      <Grid Margin="4,0,4,4">
        <TextBlock Text="Recent" FontSize="13" FontWeight="SemiBold" VerticalAlignment="Center"/>
        <HyperlinkButton x:Name="NlTimeline" Content="Timeline" FontSize="12" Padding="4,2"
                         Foreground="{ThemeResource SystemControlForegroundBaseMediumBrush}"
                         HorizontalAlignment="Right" VerticalAlignment="Center"/>
      </Grid>
      <StackPanel x:Name="NlRecentList" Spacing="2"/>
    </StackPanel>
    <StackPanel x:Name="NlClipSection" Visibility="Collapsed" Margin="0,16,0,0">
      <Grid Margin="4,0,4,6">
        <TextBlock Text="Clipboard" FontSize="13" FontWeight="SemiBold"/>
        <TextBlock Text="Click to copy" FontSize="11" Opacity="0.7" HorizontalAlignment="Right"
                   VerticalAlignment="Center"/>
      </Grid>
      <StackPanel x:Name="NlClipList" Spacing="4"/>
    </StackPanel>
  </StackPanel>
  <Grid Grid.Row="1" Margin="12" ColumnSpacing="8">
    <Grid.ColumnDefinitions>
      <ColumnDefinition Width="*"/>
      <ColumnDefinition Width="Auto"/>
    </Grid.ColumnDefinitions>
    <Button x:Name="NlOpen" HorizontalAlignment="Stretch" Height="36" CornerRadius="4"
            Background="{ThemeResource SystemControlBackgroundAccentBrush}"
            Foreground="{ThemeResource SystemControlForegroundChromeWhiteBrush}">
      <StackPanel Orientation="Horizontal" Spacing="8">
        <FontIcon Glyph="&#xE8A7;" FontSize="14" FontFamily=")xaml" + font + LR"xaml("/>
        <TextBlock Text="Open NectarLink" FontSize="13" FontWeight="SemiBold"/>
      </StackPanel>
    </Button>
    <Button x:Name="NlSettings" Grid.Column="1" Width="36" Height="36" Padding="0" CornerRadius="4"
            ToolTipService.ToolTip="NectarLink settings" AutomationProperties.Name="NectarLink settings">
      <FontIcon Glyph="&#xE713;" FontSize="15" FontFamily=")xaml" + font + LR"xaml("/>
    </Button>
  </Grid>
</Grid>)xaml";
}

static std::wstring BuildPanelXaml() {
    return BuildContentXaml(true);
}

// One row of Recent (tile with icon) or Clipboard (small icon, button background).
static std::wstring ItemXaml(bool clip) {
    std::wstring font = kIconFont;
    if (clip) {
        return L"<Button" + std::wstring(kNs) + LR"xaml( HorizontalAlignment="Stretch" HorizontalContentAlignment="Stretch"
        Padding="10,6" CornerRadius="4">
  <Grid ColumnSpacing="10">
    <Grid.ColumnDefinitions><ColumnDefinition Width="Auto"/><ColumnDefinition Width="*"/></Grid.ColumnDefinitions>
    <FontIcon x:Name="Icon" FontSize="14" Opacity="0.7" VerticalAlignment="Center" FontFamily=")xaml" + font + LR"xaml("/>
    <StackPanel Grid.Column="1">
      <TextBlock x:Name="Title" FontSize="12" TextTrimming="CharacterEllipsis" MaxLines="1"/>
      <TextBlock x:Name="Detail" FontSize="11" Opacity="0.7" TextTrimming="CharacterEllipsis"/>
    </StackPanel>
  </Grid>
</Button>)xaml";
    }
    return L"<Button" + std::wstring(kNs) + LR"xaml( HorizontalAlignment="Stretch" HorizontalContentAlignment="Stretch"
        Background="Transparent" BorderThickness="0" Padding="6,5" CornerRadius="4">
  <Grid ColumnSpacing="12">
    <Grid.ColumnDefinitions><ColumnDefinition Width="Auto"/><ColumnDefinition Width="*"/></Grid.ColumnDefinitions>
    <Border x:Name="Tile" Width="34" Height="34" CornerRadius="4">
      <FontIcon x:Name="Icon" FontSize="16" Foreground="{ThemeResource SystemControlForegroundAccentBrush}" FontFamily=")xaml" + font + LR"xaml("/>
    </Border>
    <StackPanel Grid.Column="1" VerticalAlignment="Center">
      <TextBlock x:Name="Title" FontSize="13" TextTrimming="CharacterEllipsis"/>
      <TextBlock x:Name="Detail" FontSize="11" Opacity="0.7" TextTrimming="CharacterEllipsis"/>
    </StackPanel>
  </Grid>
</Button>)xaml";
}

struct UiState {
    wux::FrameworkElement root{nullptr};
    winrt::weak_ref<wuxc::Grid> companionHost;
    winrt::weak_ref<wux::FrameworkElement> hiddenContent;
    wuxc::TextBlock nameText{nullptr}, statusText{nullptr}, hintText{nullptr}, battText{nullptr};
    wuxc::FontIcon battIcon{nullptr}, phoneIcon{nullptr};
    wuxc::StackPanel recentSection{nullptr}, recentList{nullptr}, clipSection{nullptr}, clipList{nullptr};
    std::vector<wuxc::Button> actionButtons;
    std::vector<std::pair<wux::Controls::Primitives::ButtonBase, winrt::event_token>> clicks;
    std::wstring recentSig, clipSig;
    wux::DispatcherTimer timer{nullptr};
    winrt::event_token tickToken{};
    bool dumped = false;
};
static UiState* g_ui = nullptr;
static bool g_uiInitOk = false;

template <typename T>
static T Named(const wux::FrameworkElement& root, PCWSTR name) {
    return root.FindName(name).as<T>();
}

static void OnClick(wuxc::Primitives::ButtonBase const& b, std::function<void()> fn) {
    auto token = b.Click([fn](wf::IInspectable const&, wux::RoutedEventArgs const&) { fn(); });
    g_ui->clicks.emplace_back(b, token);
}

static wchar_t BatteryGlyph(int level, bool charging) {
    int step = std::clamp((level + 5) / 10, 0, 10);
    if (charging) {
        if (step == 10) return 0xEA93;
        if (step == 9) return 0xE83E;
        return (wchar_t)(0xE85A + step);
    }
    if (step == 10) return 0xE83F;
    return (wchar_t)(0xE850 + step);
}

static std::wstring KindLabel(const std::wstring& kind) {
    if (kind == L"photo") return L"Photo";
    if (kind == L"file") return L"File";
    if (kind == L"link") return L"Link";
    if (kind == L"clip") return L"Clipboard";
    if (kind == L"recording") return L"Recording";
    if (kind == L"session") return L"Session";
    return L"Item";
}

static wchar_t KindGlyph(const std::wstring& kind) {
    if (kind == L"photo") return 0xE8B9;
    if (kind == L"file") return 0xE8A5;
    if (kind == L"link") return 0xE71B;
    if (kind == L"clip") return 0xE77F;
    if (kind == L"recording") return 0xE714;
    return 0xE8EA;
}

// Neutral tile background on purpose; the accent color goes on the icons and the Open button.
static winrt::Windows::UI::Color KindTint(const std::wstring&) {
    return {0x40, 0x80, 0x80, 0x80};
}

static wuxc::Button MakeItem(bool clip, wchar_t glyph, const std::wstring& title, const std::wstring& detail) {
    auto b = wux::Markup::XamlReader::Load(ItemXaml(clip)).as<wuxc::Button>();
    wchar_t g[2] = {glyph, 0};
    Named<wuxc::FontIcon>(b, L"Icon").Glyph(g);
    Named<wuxc::TextBlock>(b, L"Title").Text(title);
    Named<wuxc::TextBlock>(b, L"Detail").Text(detail);
    return b;
}

static void ClearList(wuxc::StackPanel const& list) {
    // Drop the click registrations of the rows being removed.
    auto& c = g_ui->clicks;
    c.erase(std::remove_if(c.begin(), c.end(),
                           [&](std::pair<wuxc::Primitives::ButtonBase, winrt::event_token>& p) {
                               auto parent = wuxm::VisualTreeHelper::GetParent(p.first).try_as<wuxc::StackPanel>();
                               if (parent && parent == list) { p.first.Click(p.second); return true; }
                               return false;
                           }),
            c.end());
    list.Children().Clear();
}

static void RebuildRecent(const PhoneStatus& s) {
    bool show = s.apiOk && g_settings.recentCount > 0 && !s.recent.empty();
    g_ui->recentSection.Visibility(show ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    std::wstring sig;
    std::vector<std::pair<std::wstring, std::wstring>> rows;
    for (auto& r : s.recent) {
        std::wstring title = !r.title.empty() ? r.title : r.detail;
        std::wstring detail = KindLabel(r.kind) + (r.direction == L"incoming" ? L" from phone" : L" sent to phone");
        std::wstring ago = Ago(r.ts);
        if (!ago.empty()) detail += L" · " + ago;
        rows.emplace_back(title, detail);
        sig += r.kind + L"|" + title + L"|" + detail + L"\n";
    }
    if (sig == g_ui->recentSig) return;
    g_ui->recentSig = sig;
    ClearList(g_ui->recentList);
    for (size_t i = 0; i < s.recent.size(); i++) {
        auto& r = s.recent[i];
        auto b = MakeItem(false, KindGlyph(r.kind), rows[i].first, rows[i].second);
        Named<wuxc::Border>(b, L"Tile").Background(wuxm::SolidColorBrush(KindTint(r.kind)));
        RecentItem copy = r;
        OnClick(b, [copy] { OpenRecent(copy); });
        g_ui->recentList.Children().Append(b);
    }
}

static void RebuildClips(const PhoneStatus& s) {
    bool show = s.apiOk && g_settings.clipCount > 0 && !s.clips.empty();
    g_ui->clipSection.Visibility(show ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    std::wstring sig;
    std::vector<std::pair<std::wstring, std::wstring>> rows;
    for (auto& c : s.clips) {
        bool image = c.kind == L"image";
        std::wstring title = image ? L"Image" : c.text;
        std::replace(title.begin(), title.end(), L'\n', L' ');
        std::replace(title.begin(), title.end(), L'\r', L' ');
        std::wstring detail = c.incoming ? (L"From " + (c.device.empty() ? std::wstring(L"phone") : c.device))
                                         : std::wstring(L"From PC");
        std::wstring ago = Ago(c.ts);
        if (!ago.empty()) detail += L" · " + ago;
        rows.emplace_back(title, detail);
        sig += c.kind + L"|" + title + L"|" + detail + L"\n";
    }
    if (sig == g_ui->clipSig) return;
    g_ui->clipSig = sig;
    ClearList(g_ui->clipList);
    for (size_t i = 0; i < s.clips.size(); i++) {
        auto& c = s.clips[i];
        bool image = c.kind == L"image";
        auto b = MakeItem(true, image ? 0xE8B9 : (c.text.rfind(L"http", 0) == 0 ? 0xE71B : 0xE8C8), rows[i].first,
                          rows[i].second);
        if (image) {
            b.IsEnabled(false);  // the API doesn't give image data
        } else {
            std::wstring text = c.text;
            OnClick(b, [text] { CopyText(text); });
        }
        g_ui->clipList.Children().Append(b);
    }
}

static void ApplyStatus() {
    PhoneStatus s;
    std::wstring flash;
    {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        s = g_status;
        if (!g_flash.empty() && GetTickCount64() < g_flashUntil) flash = g_flash;
    }
    auto vis = [](bool on) { return on ? wux::Visibility::Visible : wux::Visibility::Collapsed; };

    std::wstring name = L"NectarLink", line, hint;
    bool showBattery = false;
    if (!s.apiOk) {
        line = L"Live status off";
        hint = s.problem;
    } else if (!s.hasDevice) {
        line = g_settings.deviceName.empty() ? L"No paired phone" : L"\"" + g_settings.deviceName + L"\" isn't paired";
    } else {
        name = s.name;
        if (!s.online) {
            line = L"Not connected";
            hint = L"Open NectarLink on your phone or check that both are on the same network.";
        } else {
            line = L"Connected";
            if (s.link == L"lan") line += L" · Wi-Fi";
            else if (s.link == L"usb") line += L" · USB";
            else if (s.link == L"relay") line += L" · Internet";
        }
        showBattery = s.battery >= 0;
    }
    if (!flash.empty()) hint = flash;

    g_ui->nameText.Text(name);
    g_ui->statusText.Text(showBattery ? line + L" ·" : line);
    if (showBattery) {
        wchar_t glyph[2] = {BatteryGlyph(s.battery, s.charging), 0};
        g_ui->battIcon.Glyph(glyph);
        g_ui->battText.Text(std::to_wstring(s.battery) + L"%");
    }
    g_ui->battIcon.Visibility(vis(showBattery));
    g_ui->battText.Visibility(vis(showBattery));
    g_ui->hintText.Text(hint);
    g_ui->hintText.Visibility(vis(!hint.empty()));

    // Without the API we can't tell if the phone is there, so let the --task actions try.
    bool phoneOk = s.apiOk ? (s.hasDevice && s.online) : true;
    g_ui->phoneIcon.Opacity(phoneOk ? 1.0 : 0.45);
    for (size_t i = 0; i < g_ui->actionButtons.size(); i++) {
        bool enabled = phoneOk && (kActions[i].kind == ActKind::Task || s.apiOk);
        g_ui->actionButtons[i].IsEnabled(enabled);
    }

    RebuildRecent(s);
    RebuildClips(s);
}

static wux::FrameworkElement FindByName(wux::DependencyObject const& node, std::wstring_view name, int depth) {
    if (!node) return nullptr;
    if (auto fe = node.try_as<wux::FrameworkElement>(); fe && std::wstring_view(fe.Name()) == name) return fe;
    if (depth > 60) return nullptr;
    int n = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; i++) {
        if (auto r = FindByName(wuxm::VisualTreeHelper::GetChild(node, i), name, depth + 1)) return r;
    }
    return nullptr;
}

static void DumpNames(wux::DependencyObject const& node, int depth, int& budget) {
    if (!node || budget <= 0 || depth > 16) return;
    if (auto fe = node.try_as<wux::FrameworkElement>(); fe && !fe.Name().empty()) {
        Wh_Log(L"%*s%s  (%s, %.0fx%.0f)", depth * 2, L"", fe.Name().c_str(), winrt::get_class_name(fe).c_str(),
               fe.ActualWidth(), fe.ActualHeight());
        budget--;
    }
    int n = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; i++) DumpNames(wuxm::VisualTreeHelper::GetChild(node, i), depth + 1, budget);
}

static void DetachRoot() {
    if (!g_ui || !g_ui->root) return;
    if (auto parent = wuxm::VisualTreeHelper::GetParent(g_ui->root).try_as<wuxc::Panel>()) {
        uint32_t idx = 0;
        if (parent.Children().IndexOf(g_ui->root, idx)) parent.Children().RemoveAt(idx);
    }
}

static void RestorePhoneLink() {
    if (!g_ui) return;
    if (auto mc = g_ui->hiddenContent.get()) mc.Visibility(wux::Visibility::Visible);
    g_ui->hiddenContent = {};
}

// Put the panel inside Start's phone panel, in place of Phone Link's content.
static void TickPanel() {
    if (!g_takeOverAllowed) {
        if (g_ui->companionHost.get()) {
            DetachRoot();
            RestorePhoneLink();
            g_ui->companionHost = {};
            Wh_Log(L"NectarLink not installed or not paired: Phone Link's panel restored");
        }
        return;
    }
    auto host = g_ui->companionHost.get();
    if (!host || !host.IsLoaded()) {
        host = nullptr;
        auto window = wux::Window::Current();
        auto content = window ? window.Content() : nullptr;
        wux::FrameworkElement companion{nullptr};
        if (content) {
            companion = FindByName(content, L"RightCompanion", 0);
            if (!companion) companion = FindByName(content, L"LeftCompanion", 0);
        }
        if (companion) host = FindByName(companion, L"CompanionRoot", 0).try_as<wuxc::Grid>();
        if (!host) {
            if (!g_ui->dumped && content) {
                g_ui->dumped = true;
                Wh_Log(L"Start's phone panel (RightCompanion) not found yet. Is \"Show mobile device in Start\" on? "
                       L"Named elements in Start:");
                int budget = 400;
                DumpNames(content, 0, budget);
            }
            return;
        }
        RestorePhoneLink();
        DetachRoot();
        host.Children().Append(g_ui->root);
        wuxc::Grid::SetRowSpan(g_ui->root, 99);
        wuxc::Grid::SetColumnSpan(g_ui->root, 99);
        g_ui->root.Margin(wux::Thickness{1, 1, 1, 1});
        g_ui->companionHost = winrt::make_weak(host);
        if (auto mc = FindByName(companion, L"MainContent", 0)) g_ui->hiddenContent = winrt::make_weak(mc);
        Wh_Log(L"Attached to Start's phone panel");
    }
    // Start can recreate Phone Link's MainContent while our host stays loaded. Our root is
    // transparent, so a new one would show through: look for it again.
    auto mc = g_ui->hiddenContent.get();
    if (!mc) {
        mc = FindByName(host, L"MainContent", 0);
        if (mc) g_ui->hiddenContent = winrt::make_weak(mc);
    }
    if (mc && mc.Visibility() != wux::Visibility::Collapsed) mc.Visibility(wux::Visibility::Collapsed);
}

static void OnTick() {
    if (!g_ui) return;
    try {
        {
            std::lock_guard<std::mutex> lock(g_statusMutex);
            if (!g_flash.empty() && GetTickCount64() >= g_flashUntil) {
                g_flash.clear();
                g_statusDirty = true;
            }
        }
        if (g_statusDirty.exchange(false)) ApplyStatus();
        TickPanel();
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Tick error 0x%08X: %s", (unsigned)e.code(), e.message().c_str());
    }
}

static void WINAPI InitUi(PVOID) {
    if (g_ui) { g_uiInitOk = true; return; }
    try {
        if (!wux::Window::Current()) { g_uiInitOk = false; return; }
        g_ui = new UiState();
        auto root = wux::Markup::XamlReader::Load(BuildPanelXaml()).as<wux::FrameworkElement>();
        g_ui->root = root;
        g_ui->nameText = Named<wuxc::TextBlock>(root, L"NlName");
        g_ui->statusText = Named<wuxc::TextBlock>(root, L"NlStatus");
        g_ui->hintText = Named<wuxc::TextBlock>(root, L"NlHint");
        g_ui->battText = Named<wuxc::TextBlock>(root, L"NlBattText");
        g_ui->battIcon = Named<wuxc::FontIcon>(root, L"NlBattIcon");
        g_ui->phoneIcon = Named<wuxc::FontIcon>(root, L"NlPhoneIcon");
        g_ui->recentSection = Named<wuxc::StackPanel>(root, L"NlRecentSection");
        g_ui->recentList = Named<wuxc::StackPanel>(root, L"NlRecentList");
        g_ui->clipSection = Named<wuxc::StackPanel>(root, L"NlClipSection");
        g_ui->clipList = Named<wuxc::StackPanel>(root, L"NlClipList");

        for (auto& a : kActions) {
            auto btn = Named<wuxc::Button>(root, a.name);
            g_ui->actionButtons.push_back(btn);
            std::wstring arg = a.arg;
            if (a.kind == ActKind::Task) {
                OnClick(btn, [arg] { RunTask(arg); });
            } else {
                std::string body;
                for (const wchar_t* p = a.body; p && *p; p++) body += (char)*p;  // ASCII JSON
                OnClick(btn, [arg, body] { PostAction(arg, body); });
            }
        }
        OnClick(Named<wuxc::Button>(root, L"NlOpen"), [] { RunTask(L""); });
        OnClick(Named<wuxc::Button>(root, L"NlSettings"), [] { RunTask(L"open-settings"); });
        OnClick(Named<wuxc::HyperlinkButton>(root, L"NlTimeline"), [] { RunTask(L"open-timeline"); });

        g_ui->timer = wux::DispatcherTimer();
        g_ui->timer.Interval(std::chrono::milliseconds(500));
        g_ui->tickToken = g_ui->timer.Tick([](wf::IInspectable const&, wf::IInspectable const&) { OnTick(); });
        g_ui->timer.Start();
        g_statusDirty = true;
        g_uiInitOk = true;
        Wh_Log(L"Panel created");
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"InitUi error 0x%08X: %s", (unsigned)e.code(), e.message().c_str());
        g_uiInitOk = true;  // don't retry a broken XAML forever
    }
}

static void WINAPI UninitUi(PVOID) {
    if (!g_ui) return;
    try {
        if (g_ui->timer) {
            g_ui->timer.Stop();
            g_ui->timer.Tick(g_ui->tickToken);
        }
        for (auto& [btn, token] : g_ui->clicks) btn.Click(token);
        g_ui->clicks.clear();
        if (g_ui->recentList) g_ui->recentList.Children().Clear();
        if (g_ui->clipList) g_ui->clipList.Children().Clear();
        DetachRoot();
        RestorePhoneLink();
    } catch (...) {
    }
    delete g_ui;
    g_ui = nullptr;
}

// ---------------------------------------------------------------------------
// Worker: wait for Start's window, create the panel, then poll NectarLink
// ---------------------------------------------------------------------------

static HANDLE g_stopEvent = nullptr;
static std::thread g_worker;

static void Worker() {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    while (true) {
        if (!g_coreWnd) g_coreWnd = FindStartCoreWindow();
        if (g_coreWnd) {
            g_uiInitOk = false;
            RunFromWindowThread(g_coreWnd, InitUi, nullptr);
            if (g_uiInitOk) break;
        }
        if (WaitForSingleObject(g_stopEvent, 1000) != WAIT_TIMEOUT) goto done;
    }

    do {
        PhoneStatus s;
        try {
            s = FetchStatus();
        } catch (winrt::hresult_error const& e) {
            s.problem = L"Status error: " + std::wstring(e.message());
        }
        {
            std::lock_guard<std::mutex> lock(g_statusMutex);
            g_status = std::move(s);
        }
        g_statusDirty = true;
        // Leave Phone Link alone unless NectarLink is installed and (as far as we can tell) paired.
        bool installed = GetFileAttributesW(ResolveExe().c_str()) != INVALID_FILE_ATTRIBUTES;
        bool unpaired = g_status.apiOk && !g_status.hasDevice;
        g_takeOverAllowed = g_settings.takeOverAlways || (installed && !unpaired);
    } while (WaitForSingleObject(g_stopEvent, g_settings.refreshSeconds * 1000) == WAIT_TIMEOUT);

done:
    winrt::uninit_apartment();
}

static void StartMod() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_worker = std::thread(Worker);
}

static void StopMod() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_worker.joinable()) g_worker.join();
    if (g_coreWnd && IsWindow(g_coreWnd)) RunFromWindowThread(g_coreWnd, UninitUi, nullptr);
    for (int i = 0; i < 50 && g_actionsRunning > 0; i++) Sleep(100);  // let running API calls finish
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
}

BOOL Wh_ModInit() {
    Wh_Log(L"NectarLink in Start: init");
    LoadSettings();
    return TRUE;
}

void Wh_ModAfterInit() {
    StartMod();
}

void Wh_ModUninit() {
    Wh_Log(L"NectarLink in Start: uninit");
    StopMod();
}

void Wh_ModSettingsChanged() {
    StopMod();
    LoadSettings();
    StartMod();
}
