// ==WindhawkMod==
// @id              game-mode-toggle
// @name            Game Mode Toggle
// @description     One hotkey to get ready to play: closes the apps you don't need and switches the power plan. Press it again to bring everything back exactly as it was.
// @version         1.0.0
// @author          theshindesahil
// @github          https://github.com/theshindesahil
// @include         windhawk.exe
// @compilerOptions -lpowrprof -lshell32 -lole32 -ldwmapi -ladvapi32 -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Game Mode Toggle

Press **Ctrl+Alt+G** before you play: the mod shows what it is about to close,
you confirm, and your PC is ready. Press the hotkey again and everything comes
back: the apps are reopened, the power plan is restored, game priorities are put
back.

Nothing runs in the background while Game Mode is off. The mod only waits for
its hotkey, in a small separate process started by Windhawk.

## What happens when you turn it on

1. **A fresh snapshot is taken**: the active power plan, every app it is about
   to close, and the priority of any running game.
2. **A confirmation shows the list**: Enter confirms, Esc cancels. Turn the
   confirmation off in the settings if you prefer.
3. **Apps are closed politely**, all at once: every window of the app gets a
   normal close request, exactly like clicking the X. An app that asks "save
   changes?" stays open and is left alone. Nothing is force-closed unless you
   list it under *Apps to force close*.
4. **The power plan is switched** (Ultimate Performance by default, if your PC
   has it).
5. **Games that are already running** (from your game folders) get High
   priority. Realtime is never used.

## What happens when you turn it off

Exactly the snapshot is restored, then deleted: apps that were closed are
reopened, the power plan goes back to what it was, priorities are reset. There
is no stored "original": changes you make while Game Mode is off simply stay.
Apps you reopened yourself in the meantime are not started twice.

## Which apps are closed

- **Apps to close**: process names you list (for example `Rainmeter.exe`). Every
  process with that name is asked to close, tray apps included.
- **Close every other app too** (on by default): any other normal app that has
  a window on your taskbar. Never touched: Windows itself, Explorer, Windhawk,
  anything under *Apps to keep*, and anything running from your *Game folders*.
- Store apps (Phone Link, WhatsApp, Files, ...) are reopened through Windows
  with their app identity, other apps with their original command line. A web
  link a browser was started with is not replayed.

## Safety

- The mod never force-closes an app you did not list for it.
- An unreadable or half-written snapshot is discarded with a notification
  instead of blocking the hotkey.
- If the PC restarted or crashed while Game Mode was on, the leftover snapshot
  is restored about 90 seconds after the mod starts, so Windows startup apps
  come up first and nothing opens twice.
- Elevated (administrator) apps cannot be closed by a normal-privilege tool;
  they are left running.

## Limits

- Windows services (a local database, for instance) need administrator rights
  and are not touched.
- The GPU mode (hybrid or discrete-only) is a firmware/driver setting and is not
  changed.
- Do not run it from inside an app that is on the close list: that app is
  closed, with whatever it was doing.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey: Ctrl+Alt+G
  $name: Hotkey
  $description: >-
    Press it once to turn Game Mode on, again to turn it off. Use Ctrl, Alt,
    Shift or Win plus a letter, digit or F1-F24, for example Ctrl+Alt+G.
- confirm: true
  $name: Ask before turning on and off
  $description: >-
    Show the list of apps first. Enter confirms, Esc cancels.
- gamePowerPlan: ultimate
  $name: Power plan while playing
  $options:
    - none: Leave the power plan alone
    - ultimate: Ultimate Performance
    - high: High performance
  $description: >-
    If the plan does not exist on this PC, the power plan is left alone. The
    plan that was active is restored when Game Mode is turned off.
- closeApps: [""]
  $name: Apps to close
  $description: >-
    Process names, one per row, for example Rainmeter.exe. All processes with
    that name are asked to close, including tray apps without a window.
- closeOthers: true
  $name: Close every other app too
  $description: >-
    Also close any other app that has a window on the taskbar, except Windows
    itself, Windhawk and the lists below. The confirmation shows the full list.
- keepApps: [steam.exe, EpicGamesLauncher.exe, Battle.net.exe, GalaxyClient.exe, Discord.exe, obs64.exe, WindowsTerminal.exe]
  $name: Apps to keep
  $description: >-
    Process names that are never closed, even with "Close every other app too".
    Add your game launchers, voice chat and recording tools here.
- forceCloseApps: [""]
  $name: Apps to force close
  $description: >-
    Listed apps that ignore the normal close request can be ended after the
    wait below. Unsaved data in these apps is lost. Leave empty to never force.
- closeTimeoutSec: 8
  $name: Seconds to wait for apps to close
  $description: >-
    How long apps get to close on their own before the remaining ones are
    reported as still running (or ended, if they are in the force list).
- gameFolders: [""]
  $name: Game folders
  $description: >-
    Programs running from these folders are never closed and get High
    priority, for example D:\SteamLibrary\steamapps\common. Subfolders count.
- boostGames: true
  $name: High priority for running games
  $description: >-
    Only applies to games that are already running when you turn Game Mode on.
- notify: true
  $name: Show a notification when done
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <powrprof.h>
#include <shellapi.h>
#include <tlhelp32.h>

#include <algorithm>
#include <cstdlib>
#include <cwctype>
#include <map>
#include <set>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Pure logic. No Windows calls: this part is unit-tested outside Windhawk.

struct Settings {
    UINT hotMods = MOD_CONTROL | MOD_ALT;
    UINT hotVk = 'G';
    bool hotkeyValid = true;
    bool confirm = true;
    bool closeOthers = true;
    bool boostGames = true;
    bool notify = true;
    std::wstring plan = L"ultimate";
    int timeoutSec = 8;
    std::vector<std::wstring> closeApps;
    std::vector<std::wstring> keepApps;
    std::vector<std::wstring> forceApps;
    std::vector<std::wstring> gameFolders;
};

static std::wstring Lower(std::wstring s) {
    for (auto& c : s) {
        c = (wchar_t)towlower(c);
    }
    return s;
}

static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n\"");
    if (a == std::wstring::npos) {
        return L"";
    }
    size_t b = s.find_last_not_of(L" \t\r\n\"");
    return s.substr(a, b - a + 1);
}

// Whitespace only: unlike Trim, it keeps quotes, which are part of arguments.
static std::wstring TrimSpace(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos) {
        return L"";
    }
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}

static std::wstring BaseName(const std::wstring& path) {
    size_t i = path.find_last_of(L"\\/");
    return i == std::wstring::npos ? path : path.substr(i + 1);
}

// "Rainmeter" and "Rainmeter.exe" both name the same process.
static std::wstring ExeKey(const std::wstring& name) {
    std::wstring n = Lower(Trim(name));
    if (n.size() > 4 && n.compare(n.size() - 4, 4, L".exe") == 0) {
        n.resize(n.size() - 4);
    }
    return n;
}

static bool InNameList(const std::vector<std::wstring>& list,
                       const std::wstring& exeName) {
    std::wstring key = ExeKey(exeName);
    for (const auto& item : list) {
        if (ExeKey(item) == key) {
            return true;
        }
    }
    return false;
}

static bool InFolder(const std::wstring& path, const std::wstring& folder) {
    std::wstring f = Lower(Trim(folder));
    while (!f.empty() && (f.back() == L'\\' || f.back() == L'/')) {
        f.pop_back();
    }
    if (f.empty()) {
        return false;
    }
    std::wstring p = Lower(path);
    return p.size() > f.size() && p.compare(0, f.size(), f) == 0 &&
           (p[f.size()] == L'\\' || p[f.size()] == L'/');
}

static bool InAnyFolder(const std::wstring& path,
                        const std::vector<std::wstring>& folders) {
    for (const auto& f : folders) {
        if (InFolder(path, f)) {
            return true;
        }
    }
    return false;
}

// "Ctrl+Alt+G" -> modifiers + virtual key. At least one modifier and exactly
// one key are required, so a bare letter can never hijack typing.
static bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    UINT m = 0;
    UINT k = 0;
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t plus = text.find(L'+', pos);
        std::wstring tok = Lower(Trim(text.substr(
            pos, plus == std::wstring::npos ? std::wstring::npos : plus - pos)));
        pos = plus == std::wstring::npos ? text.size() + 1 : plus + 1;
        if (tok.empty()) {
            return false;
        }
        if (tok == L"ctrl" || tok == L"control") {
            m |= MOD_CONTROL;
        } else if (tok == L"alt") {
            m |= MOD_ALT;
        } else if (tok == L"shift") {
            m |= MOD_SHIFT;
        } else if (tok == L"win" || tok == L"windows") {
            m |= MOD_WIN;
        } else if (k != 0) {
            return false;
        } else if (tok.size() == 1 && ((tok[0] >= L'a' && tok[0] <= L'z') ||
                                       (tok[0] >= L'0' && tok[0] <= L'9'))) {
            k = (UINT)towupper(tok[0]);
        } else if (tok.size() >= 2 && tok.size() <= 3 && tok[0] == L'f') {
            int n = _wtoi(tok.c_str() + 1);
            if (n < 1 || n > 24) {
                return false;
            }
            k = (UINT)(VK_F1 + n - 1);
        } else if (tok == L"space") {
            k = VK_SPACE;
        } else {
            return false;
        }
    }
    if (m == 0 || k == 0) {
        return false;
    }
    *mods = m;
    *vk = k;
    return true;
}

// What follows the executable in a full command line. A link handler start
// ("firefox.exe -url https://...") is not a way to start the app, so such
// arguments are dropped.
static std::wstring ArgsFromCommandLine(const std::wstring& cl) {
    const std::wstring& s = cl;
    size_t i = 0;
    while (i < s.size() && s[i] == L' ') {
        i++;
    }
    size_t end;
    if (i < s.size() && s[i] == L'"') {
        end = s.find(L'"', i + 1);
        if (end == std::wstring::npos) {
            return L"";
        }
        end++;
    } else {
        end = s.find(L' ', i);
        if (end == std::wstring::npos) {
            return L"";
        }
    }
    std::wstring args = TrimSpace(s.substr(end));
    if (args.find(L"://") != std::wstring::npos) {
        return L"";
    }
    return args;
}

// Processes that are part of the desktop itself.
static bool IsShellProcess(const std::wstring& exeName) {
    static const wchar_t* const kShell[] = {
        L"explorer",
        L"windhawk",
        L"applicationframehost",
        L"shellexperiencehost",
        L"startmenuexperiencehost",
        L"searchhost",
        L"textinputhost",
        L"lockapp",
        L"securityhealthsystray",
        L"msedgewebview2",
        L"widgets",
        L"ctfmon",
        L"dwm",
    };
    std::wstring key = ExeKey(exeName);
    for (const wchar_t* k : kShell) {
        if (key == k) {
            return true;
        }
    }
    return false;
}

// Why an app found through its window must not be closed, or nullptr if it may.
static const wchar_t* WhyKeep(const std::wstring& exeName,
                              const std::wstring& path,
                              const std::wstring& windowsDir,
                              const Settings& s) {
    if (IsShellProcess(exeName)) {
        return L"part of Windows";
    }
    if (!windowsDir.empty() && InFolder(path, windowsDir)) {
        return L"part of Windows";
    }
    if (InNameList(s.keepApps, exeName)) {
        return L"in Apps to keep";
    }
    if (InAnyFolder(path, s.gameFolders)) {
        return L"in a game folder";
    }
    return nullptr;
}

// ---- snapshot text format --------------------------------------------------

struct SavedApp {
    std::wstring name, path, args, aumid;
};

struct SavedPriority {
    DWORD pid = 0;
    std::wstring exe;
    DWORD priorityClass = 0;
};

struct Snapshot {
    unsigned long long startedAt = 0;  // FILETIME, 100 ns ticks
    std::wstring plan;                 // GUID text, empty = not changed
    std::vector<SavedApp> apps;
    std::vector<SavedPriority> prios;
};

static std::wstring Esc(const std::wstring& s) {
    std::wstring o;
    for (wchar_t c : s) {
        if (c == L'\\') {
            o += L"\\\\";
        } else if (c == L'\t') {
            o += L"\\t";
        } else if (c == L'\n') {
            o += L"\\n";
        } else if (c == L'\r') {
            o += L"\\r";
        } else {
            o += c;
        }
    }
    return o;
}

static std::wstring Unesc(const std::wstring& s) {
    std::wstring o;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == L'\\' && i + 1 < s.size()) {
            wchar_t n = s[++i];
            o += n == L't' ? L'\t' : n == L'n' ? L'\n' : n == L'r' ? L'\r' : n;
        } else {
            o += s[i];
        }
    }
    return o;
}

static std::wstring SerializeSnapshot(const Snapshot& sn) {
    std::wstring o = L"GMT1\n";
    o += L"started\t" + std::to_wstring(sn.startedAt) + L"\n";
    o += L"plan\t" + Esc(sn.plan) + L"\n";
    for (const auto& a : sn.apps) {
        o += L"app\t" + Esc(a.name) + L"\t" + Esc(a.path) + L"\t" + Esc(a.args) +
             L"\t" + Esc(a.aumid) + L"\n";
    }
    for (const auto& p : sn.prios) {
        o += L"prio\t" + std::to_wstring(p.pid) + L"\t" + Esc(p.exe) + L"\t" +
             std::to_wstring(p.priorityClass) + L"\n";
    }
    o += L"end\n";
    return o;
}

static std::vector<std::wstring> SplitTabs(const std::wstring& line) {
    std::vector<std::wstring> f;
    size_t pos = 0;
    while (true) {
        size_t t = line.find(L'\t', pos);
        if (t == std::wstring::npos) {
            f.push_back(line.substr(pos));
            break;
        }
        f.push_back(line.substr(pos, t - pos));
        pos = t + 1;
    }
    return f;
}

// Fails on anything that is not a complete, current-format snapshot, so a
// half-written file can never be half-applied.
static bool ParseSnapshot(const std::wstring& text, Snapshot* out) {
    Snapshot sn;
    size_t pos = 0;
    bool first = true;
    bool sawStarted = false;
    bool sawEnd = false;
    while (pos < text.size()) {
        size_t nl = text.find(L'\n', pos);
        std::wstring line = text.substr(
            pos, nl == std::wstring::npos ? std::wstring::npos : nl - pos);
        pos = nl == std::wstring::npos ? text.size() : nl + 1;
        if (sawEnd) {
            return false;  // data after the end marker
        }
        if (first) {
            if (line != L"GMT1") {
                return false;
            }
            first = false;
            continue;
        }
        auto f = SplitTabs(line);
        if (f[0] == L"started" && f.size() == 2) {
            wchar_t* e = nullptr;
            sn.startedAt = _wcstoui64(f[1].c_str(), &e, 10);
            if (!e || *e || sn.startedAt == 0) {
                return false;
            }
            sawStarted = true;
        } else if (f[0] == L"plan" && f.size() == 2) {
            sn.plan = Unesc(f[1]);
        } else if (f[0] == L"app" && f.size() == 5) {
            sn.apps.push_back(
                {Unesc(f[1]), Unesc(f[2]), Unesc(f[3]), Unesc(f[4])});
        } else if (f[0] == L"prio" && f.size() == 4) {
            SavedPriority p;
            p.pid = (DWORD)wcstoul(f[1].c_str(), nullptr, 10);
            p.exe = Unesc(f[2]);
            p.priorityClass = (DWORD)wcstoul(f[3].c_str(), nullptr, 10);
            sn.prios.push_back(p);
        } else if (f[0] == L"end" && f.size() == 1) {
            sawEnd = true;
        } else {
            return false;
        }
    }
    if (first || !sawStarted || !sawEnd) {
        return false;
    }
    *out = sn;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Windows side.

namespace {

constexpr UINT WM_APP_RELOAD = WM_APP + 1;
constexpr UINT WM_APP_STOP = WM_APP + 2;
constexpr int kHotkeyId = 0x6D47;
constexpr UINT_PTR kRecoverTimer = 0x6D48;
constexpr DWORD kRecoverDelayMs = 90 * 1000;
const wchar_t kGuidBalanced[] = L"381b4222-f694-41f0-9685-ff5bb260df2e";
const wchar_t kGuidHigh[] = L"8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c";

Settings g_settings;
std::wstring g_windowsDir;
HANDLE g_thread;
DWORD g_threadId;
HANDLE g_threadReady;
bool g_hotkeyRegistered;
bool g_busy;

// ---- settings --------------------------------------------------------------

std::vector<std::wstring> ReadList(PCWSTR name) {
    std::vector<std::wstring> list;
    for (int i = 0; i < 200; i++) {
        PCWSTR v = Wh_GetStringSetting(name, i);
        std::wstring s = v ? v : L"";
        Wh_FreeStringSetting(v);
        s = Trim(s);
        if (s.empty()) {
            break;
        }
        list.push_back(s);
    }
    return list;
}

void LoadSettings() {
    Settings s;
    PCWSTR hk = Wh_GetStringSetting(L"hotkey");
    std::wstring hotkeyText = hk ? hk : L"";
    Wh_FreeStringSetting(hk);
    s.hotkeyValid = ParseHotkey(hotkeyText, &s.hotMods, &s.hotVk);
    if (!s.hotkeyValid) {
        Wh_Log(L"Invalid hotkey \"%s\"", hotkeyText.c_str());
    }
    s.confirm = Wh_GetIntSetting(L"confirm") != 0;
    s.closeOthers = Wh_GetIntSetting(L"closeOthers") != 0;
    s.boostGames = Wh_GetIntSetting(L"boostGames") != 0;
    s.notify = Wh_GetIntSetting(L"notify") != 0;
    PCWSTR plan = Wh_GetStringSetting(L"gamePowerPlan");
    s.plan = plan ? Lower(plan) : L"none";
    Wh_FreeStringSetting(plan);
    s.timeoutSec = std::clamp(Wh_GetIntSetting(L"closeTimeoutSec"), 1, 60);
    s.closeApps = ReadList(L"closeApps[%d]");
    s.keepApps = ReadList(L"keepApps[%d]");
    s.forceApps = ReadList(L"forceCloseApps[%d]");
    s.gameFolders = ReadList(L"gameFolders[%d]");
    g_settings = s;
}

// ---- small helpers ---------------------------------------------------------

unsigned long long NowFileTime() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}

std::wstring GuidToString(const GUID& g) {
    WCHAR buf[64];
    StringFromGUID2(g, buf, ARRAYSIZE(buf));
    std::wstring s = Lower(buf);
    return s.substr(1, s.size() - 2);  // without braces
}

bool GuidFromString(const std::wstring& text, GUID* g) {
    std::wstring braced = L"{" + text + L"}";
    return SUCCEEDED(CLSIDFromString(braced.c_str(), g));
}

std::wstring SnapshotPath() {
    WCHAR dir[MAX_PATH];
    if (Wh_GetModStoragePath(dir, ARRAYSIZE(dir)) == 0) {
        return L"";
    }
    CreateDirectoryW(dir, nullptr);
    return std::wstring(dir) + L"\\snapshot.txt";
}

bool ReadFileText(const std::wstring& path, std::wstring* text) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }
    LARGE_INTEGER size;
    std::string bytes;
    bool ok = GetFileSizeEx(h, &size) && size.QuadPart < (16 << 20);
    if (ok) {
        bytes.resize((size_t)size.QuadPart);
        DWORD got = 0;
        ok = ReadFile(h, bytes.data(), (DWORD)bytes.size(), &got, nullptr) &&
             got == bytes.size();
    }
    CloseHandle(h);
    if (!ok) {
        return false;
    }
    int n = MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                                nullptr, 0);
    text->assign(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                        text->data(), n);
    return true;
}

// Written to a temp file and renamed over the real one, so a crash in the
// middle leaves the old snapshot or the new one, never half of one.
bool WriteSnapshot(const Snapshot& sn) {
    std::wstring path = SnapshotPath();
    if (path.empty()) {
        return false;
    }
    std::wstring text = SerializeSnapshot(sn);
    int n = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(),
                                nullptr, 0, nullptr, nullptr);
    std::string bytes(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), bytes.data(),
                        n, nullptr, nullptr);
    std::wstring tmp = path + L".tmp";
    HANDLE h = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD wrote = 0;
    bool ok =
        WriteFile(h, bytes.data(), (DWORD)bytes.size(), &wrote, nullptr) &&
        wrote == bytes.size() && FlushFileBuffers(h);
    CloseHandle(h);
    if (!ok) {
        DeleteFileW(tmp.c_str());
        return false;
    }
    return MoveFileExW(tmp.c_str(), path.c_str(),
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
}

enum class LoadResult { None, Ok, Corrupt };

LoadResult LoadSnapshot(Snapshot* sn) {
    std::wstring path = SnapshotPath();
    if (path.empty() ||
        GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return LoadResult::None;
    }
    std::wstring text;
    if (!ReadFileText(path, &text) || !ParseSnapshot(text, sn)) {
        return LoadResult::Corrupt;
    }
    return LoadResult::Ok;
}

void DeleteSnapshot() {
    std::wstring path = SnapshotPath();
    if (!path.empty()) {
        DeleteFileW(path.c_str());
    }
}

// ---- notification and confirmation -----------------------------------------

HWND g_notifyWnd;
constexpr UINT_PTR kNotifyTimer = 1;

LRESULT CALLBACK NotifyProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_TIMER && wp == kNotifyTimer) {
        KillTimer(hwnd, kNotifyTimer);
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd = hwnd;
        nid.uID = 1;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void Notify(const std::wstring& text) {
    if (!g_settings.notify) {
        return;
    }
    if (!g_notifyWnd) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = NotifyProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"GameModeToggleNotify";
        RegisterClassW(&wc);
        g_notifyWnd =
            CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0,
                            HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
        if (!g_notifyWnd) {
            return;
        }
    }
    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd = g_notifyWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO;
    nid.hIcon = LoadIconW(nullptr, IDI_INFORMATION);
    wcsncpy(nid.szTip, L"Game Mode", ARRAYSIZE(nid.szTip) - 1);
    wcsncpy(nid.szInfoTitle, L"Game Mode", ARRAYSIZE(nid.szInfoTitle) - 1);
    wcsncpy(nid.szInfo, text.c_str(), ARRAYSIZE(nid.szInfo) - 1);
    nid.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND;
    Shell_NotifyIconW(NIM_DELETE, &nid);
    Shell_NotifyIconW(NIM_ADD, &nid);
    SetTimer(g_notifyWnd, kNotifyTimer, 8000, nullptr);
}

bool Ask(const std::wstring& text) {
    if (!g_settings.confirm) {
        return true;
    }
    return MessageBoxW(nullptr, text.c_str(), L"Game Mode",
                       MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1 |
                           MB_SETFOREGROUND | MB_TOPMOST) == IDOK;
}

// ---- processes -------------------------------------------------------------

struct ProcInfo {
    DWORD pid = 0;
    DWORD parent = 0;
    std::wstring exe;   // file name as reported by the snapshot
    std::wstring path;  // full image path
    unsigned long long created = 0;
};

std::wstring QueryPath(HANDLE h) {
    WCHAR buf[32768];
    DWORD len = ARRAYSIZE(buf);
    if (QueryFullProcessImageNameW(h, 0, buf, &len)) {
        return std::wstring(buf, len);
    }
    return L"";
}

struct UnicodeString {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
};

std::wstring QueryCommandLine(HANDLE h) {
    using NtQueryInformationProcess_t =
        LONG(NTAPI*)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    static auto fn = (NtQueryInformationProcess_t)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"), "NtQueryInformationProcess");
    if (!fn) {
        return L"";
    }
    constexpr ULONG kProcessCommandLineInformation = 60;
    ULONG len = 0;
    fn(h, kProcessCommandLineInformation, nullptr, 0, &len);
    if (len == 0 || len > (1 << 20)) {
        return L"";
    }
    std::vector<BYTE> buf(len + 16);
    if (fn(h, kProcessCommandLineInformation, buf.data(), (ULONG)buf.size(),
           &len) < 0) {
        return L"";
    }
    auto* us = (UnicodeString*)buf.data();
    return std::wstring(us->Buffer, us->Length / sizeof(WCHAR));
}

std::wstring QueryAumid(HANDLE h) {
    using GetAumid_t = LONG(WINAPI*)(HANDLE, UINT32*, PWSTR);
    static auto fn = (GetAumid_t)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "GetApplicationUserModelId");
    if (!fn) {
        return L"";
    }
    WCHAR buf[256];
    UINT32 len = ARRAYSIZE(buf);
    if (fn(h, &len, buf) != ERROR_SUCCESS || len == 0) {
        return L"";
    }
    return std::wstring(buf, len - 1);
}

unsigned long long QueryCreated(HANDLE h) {
    FILETIME c, e, k, u;
    if (!GetProcessTimes(h, &c, &e, &k, &u)) {
        return 0;
    }
    return ((unsigned long long)c.dwHighDateTime << 32) | c.dwLowDateTime;
}

// Every process of this logon session whose image path can be read.
std::vector<ProcInfo> ListProcesses() {
    std::vector<ProcInfo> list;
    DWORD mySession = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &mySession);
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return list;
    }
    PROCESSENTRY32W pe = {sizeof(pe)};
    for (BOOL ok = Process32FirstW(snap, &pe); ok;
         ok = Process32NextW(snap, &pe)) {
        DWORD session = 0;
        if (!ProcessIdToSessionId(pe.th32ProcessID, &session) ||
            session != mySession) {
            continue;
        }
        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                               pe.th32ProcessID);
        if (!h) {
            continue;
        }
        ProcInfo p;
        p.pid = pe.th32ProcessID;
        p.parent = pe.th32ParentProcessID;
        p.exe = pe.szExeFile;
        p.path = QueryPath(h);
        p.created = QueryCreated(h);
        CloseHandle(h);
        if (!p.path.empty()) {
            list.push_back(p);
        }
    }
    CloseHandle(snap);
    return list;
}

// `created` 0 skips the check that the PID was not reused by another process.
bool PidRunning(DWORD pid, unsigned long long created) {
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
                           FALSE, pid);
    if (!h) {
        return false;
    }
    bool running = WaitForSingleObject(h, 0) == WAIT_TIMEOUT;
    if (running && created != 0 && QueryCreated(h) != created) {
        running = false;
    }
    CloseHandle(h);
    return running;
}

std::set<std::wstring> RunningPaths() {
    std::set<std::wstring> paths;
    for (const auto& p : ListProcesses()) {
        paths.insert(Lower(p.path));
    }
    return paths;
}

// ---- windows ---------------------------------------------------------------

struct WindowInfo {
    HWND hwnd;
    DWORD pid;
    bool taskbar;  // an app window as shown in Alt+Tab
};

bool IsTaskbarWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER)) {
        return false;
    }
    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) {
        return false;
    }
    if (GetWindowTextLengthW(hwnd) == 0) {
        return false;
    }
    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return false;
    }
    return true;
}

// Window callbacks are named CALLBACK functions, not lambdas: on 32-bit builds
// WNDENUMPROC is stdcall, which a lambda does not convert to.
BOOL CALLBACK CollectWindowProc(HWND hwnd, LPARAM lp) {
    auto* out = (std::vector<WindowInfo>*)lp;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    out->push_back({hwnd, pid, IsTaskbarWindow(hwnd)});
    return TRUE;
}

std::vector<WindowInfo> ListTopLevelWindows() {
    std::vector<WindowInfo> list;
    EnumWindows(CollectWindowProc, (LPARAM)&list);
    return list;
}

struct FrameSearch {
    DWORD host;
    DWORD found;
};

BOOL CALLBACK FindAppChildProc(HWND child, LPARAM lp) {
    auto* c = (FrameSearch*)lp;
    DWORD pid = 0;
    GetWindowThreadProcessId(child, &pid);
    if (pid && pid != c->host) {
        c->found = pid;
        return FALSE;
    }
    return TRUE;
}

// A Store app's window is a frame owned by ApplicationFrameHost; the app's
// own process owns a CoreWindow child. Returns that process id, or 0.
DWORD StoreAppPidOfFrame(HWND frame, DWORD hostPid) {
    FrameSearch ctx = {hostPid, 0};
    EnumChildWindows(frame, FindAppChildProc, (LPARAM)&ctx);
    return ctx.found;
}

// ---- app groups ------------------------------------------------------------

// All processes of one program (same image path), closed and reopened as one.
struct AppGroup {
    std::wstring name;  // shown in the list
    std::wstring path;
    std::wstring exe;
    std::wstring args;
    std::wstring aumid;
    std::map<DWORD, unsigned long long> pids;  // pid -> creation time
    std::vector<HWND> windows;  // every top-level window to send WM_CLOSE to
    bool forced = false;
};

std::wstring DisplayName(const std::wstring& path) {
    std::wstring n = BaseName(path);
    if (n.size() > 4 && Lower(n.substr(n.size() - 4)) == L".exe") {
        n.resize(n.size() - 4);
    }
    return n;
}

std::vector<AppGroup> CollectGroups(const Settings& s,
                                    std::vector<std::wstring>* kept) {
    std::vector<ProcInfo> procs = ListProcesses();
    std::vector<WindowInfo> windows = ListTopLevelWindows();
    DWORD self = GetCurrentProcessId();
    std::map<std::wstring, AppGroup> groups;  // key: lower-case image path
    std::map<DWORD, const ProcInfo*> byPid;
    for (const auto& p : procs) {
        byPid[p.pid] = &p;
    }

    auto addPid = [&](const ProcInfo& p) {
        AppGroup& g = groups[Lower(p.path)];
        if (g.path.empty()) {
            g.path = p.path;
            g.exe = BaseName(p.path);
            g.name = DisplayName(p.path);
        }
        g.pids[p.pid] = p.created;
    };

    // 1) Listed apps: every process with a listed name.
    for (const auto& p : procs) {
        if (p.pid != self && InNameList(s.closeApps, p.exe) &&
            !IsShellProcess(p.exe)) {
            addPid(p);
        }
    }

    // 2) Every other app that has a taskbar window.
    if (s.closeOthers) {
        for (const auto& w : windows) {
            if (!w.taskbar) {
                continue;
            }
            auto it = byPid.find(w.pid);
            if (it == byPid.end()) {
                continue;  // elevated or protected: cannot be closed anyway
            }
            const ProcInfo* p = it->second;
            if (ExeKey(p->exe) == L"applicationframehost") {
                DWORD appPid = StoreAppPidOfFrame(w.hwnd, w.pid);
                auto at = byPid.find(appPid);
                if (!appPid || at == byPid.end()) {
                    continue;
                }
                p = at->second;
            }
            if (p->pid == self) {
                continue;
            }
            if (InNameList(s.closeApps, p->exe)) {
                continue;  // already collected as a listed app
            }
            const wchar_t* why = WhyKeep(p->exe, p->path, g_windowsDir, s);
            if (why) {
                if (kept) {
                    kept->push_back(DisplayName(p->path) + L" (" + why + L")");
                }
                continue;
            }
            addPid(*p);
        }
    }

    std::vector<AppGroup> result;
    for (auto& kv : groups) {
        AppGroup& g = kv.second;
        // Copy the command line from the oldest process of the program.
        const ProcInfo* root = nullptr;
        for (const auto& pc : g.pids) {
            const ProcInfo* p = byPid[pc.first];
            if (p && (!root || p->created < root->created)) {
                root = p;
            }
        }
        if (!root) {
            continue;
        }
        for (const auto& w : windows) {
            if (g.pids.count(w.pid)) {
                g.windows.push_back(w.hwnd);
                continue;
            }
            auto it = byPid.find(w.pid);
            if (it != byPid.end() &&
                ExeKey(it->second->exe) == L"applicationframehost" &&
                g.pids.count(StoreAppPidOfFrame(w.hwnd, w.pid))) {
                g.windows.push_back(w.hwnd);  // the Store app's frame window
            }
        }
        g.forced = InNameList(s.forceApps, g.exe);
        HANDLE h =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, root->pid);
        if (h) {
            g.aumid = QueryAumid(h);
            if (g.aumid.empty()) {
                g.args = ArgsFromCommandLine(QueryCommandLine(h));
            }
            CloseHandle(h);
        }
        result.push_back(g);
    }
    std::sort(result.begin(), result.end(),
              [](const AppGroup& a, const AppGroup& b) {
                  return Lower(a.name) < Lower(b.name);
              });
    return result;
}

bool GroupRunning(const AppGroup& g) {
    for (const auto& pc : g.pids) {
        if (PidRunning(pc.first, pc.second)) {
            return true;
        }
    }
    return false;
}

// ---- power plan ------------------------------------------------------------

std::vector<std::pair<std::wstring, std::wstring>> ListPowerPlans() {
    std::vector<std::pair<std::wstring, std::wstring>> plans;
    for (ULONG i = 0; i < 64; i++) {
        GUID g;
        DWORD size = sizeof(g);
        if (PowerEnumerate(nullptr, nullptr, nullptr, ACCESS_SCHEME, i,
                           (UCHAR*)&g, &size) != ERROR_SUCCESS) {
            break;
        }
        WCHAR name[256] = {};
        DWORD nameSize = sizeof(name);
        PowerReadFriendlyName(nullptr, &g, nullptr, nullptr, (PUCHAR)name,
                              &nameSize);
        plans.push_back({GuidToString(g), name});
    }
    return plans;
}

std::wstring ActivePlan() {
    GUID* g = nullptr;
    if (PowerGetActiveScheme(nullptr, &g) != ERROR_SUCCESS || !g) {
        return L"";
    }
    std::wstring s = GuidToString(*g);
    LocalFree(g);
    return s;
}

bool SetPlan(const std::wstring& guidText) {
    GUID g;
    return GuidFromString(guidText, &g) &&
           PowerSetActiveScheme(nullptr, &g) == ERROR_SUCCESS;
}

// The plan to switch to for the setting, or empty when it does not exist.
std::wstring ResolveGamePlan(const std::wstring& setting, std::wstring* label) {
    auto plans = ListPowerPlans();
    if (setting == L"high") {
        for (const auto& p : plans) {
            if (p.first == kGuidHigh) {
                *label = L"High performance";
                return p.first;
            }
        }
    } else if (setting == L"ultimate") {
        for (const auto& p : plans) {
            if (Lower(p.second).find(L"ultimate") != std::wstring::npos) {
                *label = p.second;
                return p.first;
            }
        }
    }
    return L"";
}

// ---- turning on ------------------------------------------------------------

std::wstring Bullets(const std::vector<std::wstring>& names, size_t max) {
    std::wstring o;
    for (size_t i = 0; i < names.size() && i < max; i++) {
        o += L"    " + names[i] + L"\n";
    }
    if (names.size() > max) {
        o += L"    ...and " + std::to_wstring(names.size() - max) + L" more\n";
    }
    return o;
}

std::wstring OnDialogText(const std::vector<std::wstring>& names,
                          bool changePlan,
                          bool planNotSet,
                          const std::wstring& planLabel,
                          size_t gameCount) {
    std::wstring text = L"Turn on Game Mode?\n\n";
    text += names.empty()
                ? L"Nothing to close.\n"
                : L"Close " + std::to_wstring(names.size()) + L" app(s):\n" +
                      Bullets(names, 18);
    text += L"\n";
    text += changePlan ? L"Power plan: switch to " + planLabel + L".\n"
            : planNotSet
                ? L"Power plan: not changed.\n"
                : L"Power plan: already " + planLabel + L".\n";
    if (gameCount) {
        text += L"High priority for " + std::to_wstring(gameCount) +
                L" running game process(es).\n";
    }
    text += L"\nPress the hotkey again to restore everything.";
    return text;
}

std::wstring OffDialogText(const std::vector<std::wstring>& names,
                           bool planRestored) {
    std::wstring text = L"Turn off Game Mode?\n\n";
    text += names.empty() ? L"No apps to reopen.\n"
                          : L"Reopen " + std::to_wstring(names.size()) +
                                L" app(s):\n" + Bullets(names, 18);
    text += planRestored ? L"\nPower plan: restore the one active before."
                         : L"\nPower plan: not changed by Game Mode.";
    return text;
}

void TurnOn() {
    const Settings s = g_settings;
    std::vector<AppGroup> groups = CollectGroups(s, nullptr);

    std::wstring planLabel;
    std::wstring current = ActivePlan();
    std::wstring target =
        s.plan == L"none" ? L"" : ResolveGamePlan(s.plan, &planLabel);
    bool changePlan = !target.empty() && target != current;

    // Games that are running now.
    std::vector<ProcInfo> games;
    if (s.boostGames && !s.gameFolders.empty()) {
        for (const auto& p : ListProcesses()) {
            if (InAnyFolder(p.path, s.gameFolders)) {
                games.push_back(p);
            }
        }
    }

    std::vector<std::wstring> names;
    for (const auto& g : groups) {
        names.push_back(g.name + (g.forced ? L"  (force close)" : L""));
    }
    if (!Ask(OnDialogText(names, changePlan, target.empty(), planLabel,
                          games.size()))) {
        Wh_Log(L"ON cancelled");
        return;
    }

    // Snapshot first: if anything below is interrupted, a restore knows what
    // to do. Apps that fail to close are dropped from it afterwards.
    Snapshot sn;
    sn.startedAt = NowFileTime();
    sn.plan = changePlan ? current : L"";
    for (const auto& g : groups) {
        sn.apps.push_back({g.name, g.path, g.args, g.aumid});
    }
    for (const auto& p : games) {
        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, p.pid);
        if (!h) {
            continue;
        }
        DWORD cls = GetPriorityClass(h);
        CloseHandle(h);
        if (cls) {
            sn.prios.push_back({p.pid, p.exe, cls});
        }
    }
    if (!WriteSnapshot(sn)) {
        Wh_Log(L"Could not write the snapshot; nothing was changed");
        Notify(L"Could not save the snapshot, so nothing was changed.");
        return;
    }

    if (changePlan) {
        Wh_Log(L"Power plan %s -> %s", current.c_str(), target.c_str());
        if (!SetPlan(target)) {
            Wh_Log(L"SetPlan failed");
            sn.plan = L"";
        }
    }

    // Close everything at once: one round of WM_CLOSE, one shared wait.
    for (const auto& g : groups) {
        Wh_Log(L"Closing %s (%zu process(es), %zu window(s))", g.name.c_str(),
               g.pids.size(), g.windows.size());
        for (HWND w : g.windows) {
            PostMessageW(w, WM_CLOSE, 0, 0);
        }
    }
    ULONGLONG deadline = GetTickCount64() + (ULONGLONG)s.timeoutSec * 1000;
    std::vector<bool> done(groups.size(), false);
    while (true) {
        bool pending = false;
        for (size_t i = 0; i < groups.size(); i++) {
            if (!done[i] && !GroupRunning(groups[i])) {
                done[i] = true;
            }
            pending = pending || !done[i];
        }
        if (!pending || GetTickCount64() >= deadline) {
            break;
        }
        Sleep(250);
    }
    int closed = 0;
    int left = 0;
    Snapshot after = sn;
    after.apps.clear();
    for (size_t i = 0; i < groups.size(); i++) {
        const AppGroup& g = groups[i];
        if (!done[i] && g.forced) {
            Wh_Log(L"Force closing %s", g.name.c_str());
            for (const auto& pc : g.pids) {
                HANDLE h =
                    OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pc.first);
                if (h) {
                    TerminateProcess(h, 1);
                    WaitForSingleObject(h, 2000);
                    CloseHandle(h);
                }
            }
            done[i] = !GroupRunning(g);
        }
        if (done[i]) {
            closed++;
            after.apps.push_back({g.name, g.path, g.args, g.aumid});
        } else {
            left++;
            Wh_Log(L"%s did not close (it may be asking to save); left running",
                   g.name.c_str());
        }
    }

    // Priority last, on the games that were running.
    int boosted = 0;
    for (const auto& pr : after.prios) {
        HANDLE h = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pr.pid);
        if (h) {
            boosted += SetPriorityClass(h, HIGH_PRIORITY_CLASS) ? 1 : 0;
            CloseHandle(h);
        }
    }
    WriteSnapshot(after);

    std::wstring msg = L"On: closed " + std::to_wstring(closed) + L" app(s)";
    if (left) {
        msg += L", " + std::to_wstring(left) + L" left running";
    }
    msg += changePlan ? L", power plan set." : L".";
    if (boosted) {
        msg += L" " + std::to_wstring(boosted) + L" game process(es) boosted.";
    }
    Wh_Log(L"%s", msg.c_str());
    Notify(msg);
}

// ---- turning off -----------------------------------------------------------

bool Relaunch(const SavedApp& a) {
    if (!a.aumid.empty()) {
        std::wstring target = L"shell:AppsFolder\\" + a.aumid;
        return (INT_PTR)ShellExecuteW(nullptr, L"open", L"explorer.exe",
                                      target.c_str(), nullptr,
                                      SW_SHOWNORMAL) > 32;
    }
    if (GetFileAttributesW(a.path.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    std::wstring cmd =
        L"\"" + a.path + L"\"" + (a.args.empty() ? L"" : L" " + a.args);
    std::wstring dir = a.path.substr(0, a.path.find_last_of(L'\\'));
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi = {};
    if (!CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, FALSE, 0, nullptr,
                        dir.c_str(), &si, &pi)) {
        return false;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

// `interactive` false = silent recovery after a crash or restart.
void TurnOff(const Snapshot& sn, bool interactive) {
    std::set<std::wstring> running = RunningPaths();
    std::vector<std::wstring> names;
    for (const auto& a : sn.apps) {
        if (!running.count(Lower(a.path))) {
            names.push_back(a.name);
        }
    }
    if (interactive) {
        if (!Ask(OffDialogText(names, !sn.plan.empty()))) {
            Wh_Log(L"OFF cancelled");
            return;
        }
    }

    for (const auto& p : sn.prios) {
        if (!PidRunning(p.pid, 0)) {
            continue;
        }
        HANDLE h = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_INFORMATION, FALSE,
            p.pid);
        if (!h) {
            continue;
        }
        if (Lower(BaseName(QueryPath(h))) == Lower(p.exe)) {
            SetPriorityClass(h, p.priorityClass);
        }
        CloseHandle(h);
    }

    bool failed = false;
    if (!sn.plan.empty() && ActivePlan() != sn.plan) {
        std::wstring target = sn.plan;
        bool exists = false;
        for (const auto& p : ListPowerPlans()) {
            exists = exists || p.first == target;
        }
        if (!exists) {
            Wh_Log(L"Saved power plan %s no longer exists; using Balanced",
                   target.c_str());
            target = kGuidBalanced;
        }
        if (!SetPlan(target)) {
            Wh_Log(L"Could not restore the power plan");
            failed = true;
        }
    }

    // Apps first started again by Windows or by the user are left alone.
    running = RunningPaths();
    int reopened = 0;
    Snapshot rest = sn;
    rest.apps.clear();
    rest.prios.clear();
    for (const auto& a : sn.apps) {
        if (running.count(Lower(a.path))) {
            Wh_Log(L"%s is already running", a.name.c_str());
            continue;
        }
        if (Relaunch(a)) {
            Wh_Log(L"Reopened %s", a.name.c_str());
            reopened++;
        } else {
            Wh_Log(L"Could not reopen %s", a.name.c_str());
            rest.apps.push_back(a);
            failed = true;
        }
    }

    if (failed) {
        // Keep only what is left, so the next press retries just that.
        if (sn.plan.empty() || ActivePlan() == sn.plan) {
            rest.plan.clear();
        }
        WriteSnapshot(rest);
        Notify(L"Off, but not everything could be restored. Press the hotkey "
               L"to retry.");
        return;
    }
    DeleteSnapshot();
    std::wstring msg = L"Off: reopened " + std::to_wstring(reopened) + L" app(s)";
    msg += sn.plan.empty() ? L"." : L", power plan restored.";
    Wh_Log(L"%s", msg.c_str());
    Notify(msg);
}

void Toggle() {
    if (g_busy) {
        return;
    }
    g_busy = true;
    Snapshot sn;
    switch (LoadSnapshot(&sn)) {
        case LoadResult::None:
            TurnOn();
            break;
        case LoadResult::Ok:
            KillTimer(nullptr, kRecoverTimer);  // the user acted before recovery
            TurnOff(sn, true);
            break;
        case LoadResult::Corrupt:
            Wh_Log(L"The snapshot is unreadable; discarding it");
            DeleteSnapshot();
            Notify(L"The saved snapshot was unreadable and was discarded. "
                   L"Nothing was restored.");
            break;
    }
    g_busy = false;
}

// After a restart or crash: restore quietly once Windows startup apps are up.
void Recover() {
    if (g_busy) {
        return;
    }
    g_busy = true;
    Snapshot sn;
    LoadResult r = LoadSnapshot(&sn);
    if (r == LoadResult::Ok) {
        Wh_Log(L"Recovering the snapshot left over from before the restart");
        TurnOff(sn, false);
    } else if (r == LoadResult::Corrupt) {
        DeleteSnapshot();
        Notify(L"A leftover Game Mode snapshot was unreadable and was "
               L"discarded.");
    }
    g_busy = false;
}

// True when a snapshot exists that was written before the last restart.
bool SnapshotIsLeftOver() {
    Snapshot sn;
    switch (LoadSnapshot(&sn)) {
        case LoadResult::Corrupt:
            return true;  // Recover() discards it
        case LoadResult::None:
            return false;
        case LoadResult::Ok:
            break;
    }
    unsigned long long boot =
        NowFileTime() - (unsigned long long)GetTickCount64() * 10000ULL;
    return sn.startedAt + 50000000ULL < boot;  // began over 5 s before boot
}

// ---- hotkey thread ---------------------------------------------------------

void RegisterHotkey() {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    if (!g_settings.hotkeyValid) {
        Notify(L"The hotkey in the settings is not valid, so Game Mode has no "
               L"hotkey.");
        return;
    }
    if (RegisterHotKey(nullptr, kHotkeyId, g_settings.hotMods | MOD_NOREPEAT,
                       g_settings.hotVk)) {
        g_hotkeyRegistered = true;
    } else {
        Wh_Log(L"RegisterHotKey failed (%lu): the hotkey is probably used by "
               L"another program",
               GetLastError());
        Notify(L"The hotkey is already used by another program. Pick another "
               L"one in the mod settings.");
    }
}

DWORD WINAPI WorkerThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // make the queue
    SetEvent(g_threadReady);

    LoadSettings();
    RegisterHotkey();
    if (SnapshotIsLeftOver()) {
        SetTimer(nullptr, kRecoverTimer, kRecoverDelayMs, nullptr);
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == (WPARAM)kHotkeyId) {
            Toggle();
            MSG drop;
            while (PeekMessageW(&drop, nullptr, WM_HOTKEY, WM_HOTKEY,
                                PM_REMOVE)) {
            }  // presses that piled up while a dialog was open
        } else if (msg.message == WM_TIMER && msg.wParam == kRecoverTimer) {
            KillTimer(nullptr, kRecoverTimer);
            Recover();
        } else if (msg.message == WM_APP_RELOAD) {
            LoadSettings();
            RegisterHotkey();
        } else if (msg.message == WM_APP_STOP) {
            break;
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, kHotkeyId);
    }
    if (g_notifyWnd) {
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd = g_notifyWnd;
        nid.uID = 1;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        DestroyWindow(g_notifyWnd);
    }
    return 0;
}

}  // namespace

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

BOOL WhTool_ModInit() {
    WCHAR windir[MAX_PATH];
    UINT n = GetWindowsDirectoryW(windir, ARRAYSIZE(windir));
    g_windowsDir = n && n < ARRAYSIZE(windir) ? windir : L"C:\\Windows";

    g_threadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }
    WaitForSingleObject(g_threadReady, 5000);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    PostThreadMessageW(g_threadId, WM_APP_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    PostThreadMessageW(g_threadId, WM_APP_STOP, 0, 0);
    WaitForSingleObject(g_thread, 3000);
    CloseHandle(g_thread);
    CloseHandle(g_threadReady);
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
