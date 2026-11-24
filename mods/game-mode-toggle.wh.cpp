// ==WindhawkMod==
// @id              game-mode-toggle
// @name            Game Mode Toggle
// @description     One hotkey to get ready to play: closes the apps you don't need and switches the power plan. Press it again to bring everything back exactly as it was.
// @version         1.0.0
// @author          theshindesahil
// @github          https://github.com/theshindesahil
// @include         windhawk.exe
// @compilerOptions -lpowrprof -lshell32 -lole32 -ldwmapi -ladvapi32 -luser32 -lgdi32 -lversion
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
2. **A checklist shows what will be closed**: untick anything to leave it alone
   this time, then Enter confirms and Esc cancels. Turn the checklist off in the
   settings if you prefer.
3. **Apps are closed politely**, all at once: every window of the app gets a
   normal close request, exactly like clicking the X. An app that asks "save
   changes?" stays open and is left alone. Background apps that have no window
   to close (tray apps) are ended, but only the ones you left ticked in the
   checklist. Apps you list under *Apps to force close* are ended too.
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
- **Close every other app too** (on by default): every other program of yours
  that is not needed for gaming, whether it has a taskbar window or only runs in
  the background (tray apps, launchers, sync clients). Helper processes are
  grouped with the app that started them. Never touched: Windows itself,
  Explorer, Windhawk and its editor, console tools (git, node, shells), drivers
  and GPU software in the *Folders to keep* list, anything under *Apps to keep*,
  anything running from your *Game folders*, and administrator apps, which a
  normal program cannot close anyway.
- Store apps (Phone Link, WhatsApp, Files, ...) are reopened through Windows
  with their app identity, other apps with their original command line. A web
  link a browser was started with is not replayed.

## Safety

- An app with a window is never force-closed unless you list it for that. A
  background app with no window is only ended if you left it ticked in the
  checklist, and with the checklist off it is only asked to close.
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
  $name: Show the checklist before turning on and off
  $description: >-
    Shows the apps as a checklist first. Untick an app to leave it alone this
    time. Enter confirms, Esc cancels. With this off nothing is ended, only
    closed politely.
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
    Also close every other app that is not needed for gaming, except Windows
    itself, Windhawk and the lists below. The checklist shows the full list.
- closeBackground: true
  $name: Include background and tray apps
  $description: >-
    Also list apps that only run in the background without a window. They are
    ended (not just asked to close) when you leave them ticked.
- keepFolders: [\NVIDIA Corporation\, \NVIDIA\, \AMD\, \Intel\, .IntelGraphicsExperience, \Realtek, \Windhawk\, \WindowsApps\MicrosoftWindows., \WindowsApps\Microsoft.Widgets, \WindowsApps\Microsoft.DesktopAppInstaller, \EdgeWebView\]
  $name: Folders to keep
  $description: >-
    Programs whose path contains any of these pieces of text are never closed.
    Use it for drivers, GPU software and tools you always want running.
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
    bool closeBackground = true;
    bool boostGames = true;
    bool notify = true;
    std::wstring plan = L"ultimate";
    int timeoutSec = 8;
    std::vector<std::wstring> closeApps;
    std::vector<std::wstring> keepApps;
    std::vector<std::wstring> keepFolders;  // pieces of text a path may contain
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
    std::wstring lowerPath = Lower(path);
    for (const auto& piece : s.keepFolders) {
        std::wstring p = Lower(Trim(piece));
        if (!p.empty() && lowerPath.find(p) != std::wstring::npos) {
            return L"in Folders to keep";
        }
    }
    return nullptr;
}

// Helper programs of an installer or updater are never the app itself.
static bool LooksLikeHelper(const std::wstring& exeName) {
    std::wstring n = Lower(exeName);
    for (const wchar_t* part : {L"update", L"crash", L"setup", L"install", L"uninst"}) {
        if (n.find(part) != std::wstring::npos) {
            return true;
        }
    }
    return false;
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
    s.closeBackground = Wh_GetIntSetting(L"closeBackground") != 0;
    s.boostGames = Wh_GetIntSetting(L"boostGames") != 0;
    s.notify = Wh_GetIntSetting(L"notify") != 0;
    PCWSTR plan = Wh_GetStringSetting(L"gamePowerPlan");
    s.plan = plan ? Lower(plan) : L"none";
    Wh_FreeStringSetting(plan);
    s.timeoutSec = std::clamp(Wh_GetIntSetting(L"closeTimeoutSec"), 1, 60);
    s.closeApps = ReadList(L"closeApps[%d]");
    s.keepApps = ReadList(L"keepApps[%d]");
    s.keepFolders = ReadList(L"keepFolders[%d]");
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

// ---- checklist dialog ------------------------------------------------------
//
// A real window with real controls (check boxes and push buttons), so keyboard
// use and screen readers work. It follows the Windows app theme (dark or light)
// and is per-monitor DPI aware: windhawk.exe is not, so the dialog thread opts
// in while the dialog is open. Enter confirms, Esc cancels, Tab moves, Space
// ticks.

struct ChecklistItem {
    std::wstring text;
    std::wstring note;  // shown after the name, e.g. "background"
};

struct ChecklistSpec {
    std::wstring title;
    std::wstring verb;               // "Closes" or "Reopens"
    std::vector<std::wstring> info;  // muted lines under the summary
    std::wstring hint;
    std::wstring okText;
    std::vector<ChecklistItem> items;
    bool startExpanded = false;
};

struct ChecklistResult {
    bool ok = false;
    std::vector<bool> checked;
};

constexpr int kIdAdv = 1001;
constexpr int kIdAll = 1002;
constexpr int kIdItem0 = 2000;
const wchar_t kDlgClass[] = L"GameModeToggleDialog";

struct DlgState {
    HWND hwnd = nullptr;
    int dpi = 96;
    bool dark = false;
    bool expanded = false;
    bool done = false;
    bool result = false;
    bool stopRequested = false;
    bool reloadRequested = false;
    HFONT font = nullptr;
    HFONT fontTitle = nullptr;
    HBRUSH bgBrush = nullptr;
    COLORREF bg = 0, foot = 0, text = 0, muted = 0, line = 0;
    int footTop = 0;
    ChecklistSpec spec;
    HWND titleWnd = nullptr, summaryWnd = nullptr, hintWnd = nullptr;
    HWND advBtn = nullptr, allBtn = nullptr, okBtn = nullptr, cancelBtn = nullptr;
    std::vector<HWND> infoWnds;
    std::vector<HWND> itemWnds;
};

DlgState* g_dlg = nullptr;

int Sd(int v) {
    return MulDiv(v, g_dlg->dpi, 96);
}

bool IsDarkTheme() {
    DWORD v = 1;
    DWORD size = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &v,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return v == 0;
}

template <typename Fn>
Fn UserProc(const char* name) {
    return (Fn)GetProcAddress(GetModuleHandleW(L"user32.dll"), name);
}

void MakeFonts() {
    DlgState& d = *g_dlg;
    NONCLIENTMETRICSW ncm = {sizeof(ncm)};
    using SpiForDpi_t = BOOL(WINAPI*)(UINT, UINT, PVOID, UINT, UINT);
    auto spiForDpi = UserProc<SpiForDpi_t>("SystemParametersInfoForDpi");
    if (!(spiForDpi && spiForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, d.dpi))) {
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
        ncm.lfMessageFont.lfHeight = -MulDiv(12, d.dpi, 96);
    }
    if (d.font) {
        DeleteObject(d.font);
    }
    if (d.fontTitle) {
        DeleteObject(d.fontTitle);
    }
    d.font = CreateFontIndirectW(&ncm.lfMessageFont);
    LOGFONTW title = ncm.lfMessageFont;
    title.lfHeight = -MulDiv(20, d.dpi, 96);
    title.lfWeight = FW_SEMIBOLD;
    d.fontTitle = CreateFontIndirectW(&title);
}

HWND MakeChild(const wchar_t* cls, const std::wstring& text, DWORD style, int id) {
    DlgState& d = *g_dlg;
    HWND h = CreateWindowExW(0, cls, text.c_str(), WS_CHILD | WS_VISIBLE | style,
                             0, 0, 10, 10, d.hwnd, (HMENU)(INT_PTR)id,
                             GetModuleHandleW(nullptr), nullptr);
    SendMessageW(h, WM_SETFONT, (WPARAM)d.font, TRUE);
    return h;
}

void PlaceChild(HWND h, int x, int y, int w, int hgt) {
    SetWindowPos(h, nullptr, x, y, w, hgt, SWP_NOZORDER | SWP_NOACTIVATE);
}

int TextWidth(HDC dc, const std::wstring& s) {
    SIZE sz = {};
    GetTextExtentPoint32W(dc, s.c_str(), (int)s.size(), &sz);
    return sz.cx;
}

// Cuts a name that does not fit and ends it with an ellipsis.
std::wstring FitText(HDC dc, const std::wstring& s, int maxWidth) {
    if (TextWidth(dc, s) <= maxWidth) {
        return s;
    }
    std::wstring t = s;
    while (t.size() > 1 && TextWidth(dc, t + L"…") > maxWidth) {
        t.pop_back();
    }
    return t + L"…";
}

void UpdateSummary() {
    DlgState& d = *g_dlg;
    size_t n = d.itemWnds.size();
    size_t on = 0;
    for (HWND h : d.itemWnds) {
        on += SendMessageW(h, BM_GETCHECK, 0, 0) == BST_CHECKED;
    }
    std::wstring s = n == 0 ? d.spec.verb + L" nothing"
                            : d.spec.verb + L" " + std::to_wstring(on) + L" of " +
                                  std::to_wstring(n) + L" apps";
    SetWindowTextW(d.summaryWnd, s.c_str());
    if (d.allBtn) {
        SendMessageW(d.allBtn, BM_SETCHECK,
                     on == n ? BST_CHECKED : on == 0 ? BST_UNCHECKED : BST_INDETERMINATE, 0);
    }
}

void Layout() {
    DlgState& d = *g_dlg;
    const int W = Sd(500);
    const int M = Sd(24);
    HDC dc = GetDC(d.hwnd);
    HFONT old = (HFONT)SelectObject(dc, d.font);
    int y = M;

    PlaceChild(d.titleWnd, M, y, W - 2 * M, Sd(30));
    y += Sd(38);
    PlaceChild(d.summaryWnd, M, y, W - 2 * M, Sd(22));
    y += Sd(24);
    for (HWND h : d.infoWnds) {
        PlaceChild(h, M, y, W - 2 * M, Sd(20));
        y += Sd(20);
    }
    y += Sd(6);

    const bool hasItems = !d.itemWnds.empty();
    ShowWindow(d.advBtn, hasItems ? SW_SHOW : SW_HIDE);
    if (hasItems) {
        PlaceChild(d.advBtn, M, y, Sd(200), Sd(30));
        SetWindowTextW(d.advBtn, d.expanded ? L"Advanced options (hide list)" : L"Advanced options");
        y += Sd(42);
    }

    const bool showList = hasItems && d.expanded;
    ShowWindow(d.allBtn, showList ? SW_SHOW : SW_HIDE);
    for (HWND h : d.itemWnds) {
        ShowWindow(h, showList ? SW_SHOW : SW_HIDE);
    }
    if (showList) {
        PlaceChild(d.allBtn, M, y, W - 2 * M, Sd(28));
        y += Sd(32);
        const int cols = d.itemWnds.size() > 24 ? 3 : 2;
        const int rowH = Sd(28);
        const int colW = (W - 2 * M) / cols;
        const int avail = colW - Sd(34);
        for (size_t i = 0; i < d.itemWnds.size(); i++) {
            const ChecklistItem& it = d.spec.items[i];
            std::wstring full = it.text + (it.note.empty() ? L"" : L"  (" + it.note + L")");
            std::wstring shown = FitText(dc, full, avail);
            SetWindowTextW(d.itemWnds[i], shown.c_str());
            int r = (int)i / cols;
            int c = (int)i % cols;
            PlaceChild(d.itemWnds[i], M + c * colW, y + r * rowH, colW - Sd(6), rowH);
        }
        int rows = ((int)d.itemWnds.size() + cols - 1) / cols;
        y += rows * rowH + Sd(10);
    }

    RECT hr = {0, 0, W - 2 * M, 0};
    DrawTextW(dc, d.spec.hint.c_str(), -1, &hr, DT_CALCRECT | DT_WORDBREAK);
    PlaceChild(d.hintWnd, M, y, W - 2 * M, hr.bottom + Sd(2));
    y += hr.bottom + M;

    d.footTop = y;
    const int bw = Sd(130);
    const int bh = Sd(34);
    PlaceChild(d.cancelBtn, W - M - bw, y + Sd(17), bw, bh);
    PlaceChild(d.okBtn, W - M - 2 * bw - Sd(12), y + Sd(17), bw, bh);
    int clientH = y + Sd(68);

    SelectObject(dc, old);
    ReleaseDC(d.hwnd, dc);

    RECT r = {0, 0, W, clientH};
    DWORD style = (DWORD)GetWindowLongPtrW(d.hwnd, GWL_STYLE);
    DWORD ex = (DWORD)GetWindowLongPtrW(d.hwnd, GWL_EXSTYLE);
    using AdjustForDpi_t = BOOL(WINAPI*)(LPRECT, DWORD, BOOL, DWORD, UINT);
    auto adjust = UserProc<AdjustForDpi_t>("AdjustWindowRectExForDpi");
    if (adjust) {
        adjust(&r, style, FALSE, ex, d.dpi);
    } else {
        AdjustWindowRectEx(&r, style, FALSE, ex);
    }
    RECT cur;
    GetWindowRect(d.hwnd, &cur);
    SetWindowPos(d.hwnd, nullptr, cur.left, cur.top, r.right - r.left,
                 r.bottom - r.top, SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(d.hwnd, nullptr, TRUE);
}

void CenterOnMonitor() {
    DlgState& d = *g_dlg;
    POINT pt;
    GetCursorPos(&pt);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
    RECT r;
    GetWindowRect(d.hwnd, &r);
    int w = r.right - r.left;
    int h = r.bottom - r.top;
    int x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - w) / 2;
    int y = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - h) / 2;
    SetWindowPos(d.hwnd, HWND_TOPMOST, x, std::max<int>(y, mi.rcWork.top), 0, 0,
                 SWP_NOSIZE | SWP_NOACTIVATE);
}

LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    DlgState* d = g_dlg;
    if (!d || !d->hwnd) {
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    switch (msg) {
        case WM_ERASEBKGND: {
            HDC dc = (HDC)wp;
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(dc, &rc, d->bgBrush);
            RECT f = {0, d->footTop, rc.right, rc.bottom};
            HBRUSH fb = CreateSolidBrush(d->foot);
            FillRect(dc, &f, fb);
            DeleteObject(fb);
            RECT ln = {0, d->footTop, rc.right, d->footTop + 1};
            HBRUSH lb = CreateSolidBrush(d->line);
            FillRect(dc, &ln, lb);
            DeleteObject(lb);
            return 1;
        }
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN: {
            HDC dc = (HDC)wp;
            HWND c = (HWND)lp;
            bool dim = c == d->hintWnd ||
                       std::find(d->infoWnds.begin(), d->infoWnds.end(), c) != d->infoWnds.end();
            SetTextColor(dc, dim ? d->muted : d->text);
            SetBkMode(dc, TRANSPARENT);
            return (LRESULT)d->bgBrush;
        }
        case WM_COMMAND: {
            int id = LOWORD(wp);
            if (id == IDOK) {
                d->result = true;
                d->done = true;
            } else if (id == IDCANCEL) {
                d->result = false;
                d->done = true;
            } else if (id == kIdAdv) {
                d->expanded = !d->expanded;
                Layout();
                CenterOnMonitor();
            } else if (id == kIdAll) {
                bool on = SendMessageW(d->allBtn, BM_GETCHECK, 0, 0) != BST_CHECKED;
                for (HWND h : d->itemWnds) {
                    SendMessageW(h, BM_SETCHECK, on ? BST_CHECKED : BST_UNCHECKED, 0);
                }
                UpdateSummary();
            } else if (id >= kIdItem0 && id < kIdItem0 + (int)d->itemWnds.size()) {
                UpdateSummary();
            }
            return 0;
        }
        case DM_GETDEFID:
            return MAKELONG(IDOK, DC_HASDEFID);
        case DM_SETDEFID:
            return TRUE;
        case WM_CLOSE:
            d->result = false;
            d->done = true;
            return 0;
        case WM_DPICHANGED: {
            d->dpi = HIWORD(wp);
            MakeFonts();
            for (HWND h : {d->titleWnd, d->summaryWnd, d->hintWnd, d->advBtn, d->allBtn, d->okBtn, d->cancelBtn}) {
                SendMessageW(h, WM_SETFONT, (WPARAM)(h == d->titleWnd ? d->fontTitle : d->font), TRUE);
            }
            for (HWND h : d->infoWnds) {
                SendMessageW(h, WM_SETFONT, (WPARAM)d->font, TRUE);
            }
            for (HWND h : d->itemWnds) {
                SendMessageW(h, WM_SETFONT, (WPARAM)d->font, TRUE);
            }
            Layout();
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// Check boxes and buttons only look modern with common controls 6, which
// windhawk.exe may not enable for itself. Activate a manifest that asks for it.
HANDLE CreateComctl6Context() {
    static const char kManifest[] =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">"
        "<dependency><dependentAssembly><assemblyIdentity type=\"win32\" "
        "name=\"Microsoft.Windows.Common-Controls\" version=\"6.0.0.0\" "
        "processorArchitecture=\"*\" publicKeyToken=\"6595b64144ccf1df\" "
        "language=\"*\"/></dependentAssembly></dependency></assembly>";
    std::wstring path;
    WCHAR dir[MAX_PATH];
    if (Wh_GetModStoragePath(dir, ARRAYSIZE(dir)) != 0) {
        CreateDirectoryW(dir, nullptr);
        path = std::wstring(dir) + L"\\comctl6.manifest";
    } else {
        GetTempPathW(ARRAYSIZE(dir), dir);
        path = std::wstring(dir) + L"gmt-comctl6.manifest";
    }
    HANDLE f = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        return nullptr;
    }
    DWORD wrote = 0;
    WriteFile(f, kManifest, (DWORD)sizeof(kManifest) - 1, &wrote, nullptr);
    CloseHandle(f);
    ACTCTXW ac = {sizeof(ac)};
    ac.lpSource = path.c_str();
    HANDLE h = CreateActCtxW(&ac);
    return h == INVALID_HANDLE_VALUE ? nullptr : h;
}

ChecklistResult ShowChecklist(const ChecklistSpec& spec) {
    ChecklistResult res;
    res.checked.assign(spec.items.size(), true);
    if (g_dlg) {
        return res;
    }
    DlgState d;
    g_dlg = &d;
    d.spec = spec;
    d.expanded = spec.startExpanded;

    using SetDpiCtx_t = HANDLE(WINAPI*)(HANDLE);
    auto setDpiCtx = UserProc<SetDpiCtx_t>("SetThreadDpiAwarenessContext");
    HANDLE oldDpiCtx = setDpiCtx ? setDpiCtx((HANDLE)-4) : nullptr;  // per-monitor v2

    HANDLE actCtx = CreateComctl6Context();
    ULONG_PTR cookie = 0;
    bool actCtxOn = actCtx && ActivateActCtx(actCtx, &cookie);

    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW wc = {sizeof(wc)};
        wc.lpfnWndProc = DlgProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hIcon = LoadIconW(nullptr, IDI_QUESTION);
        wc.lpszClassName = kDlgClass;
        classRegistered = RegisterClassExW(&wc) != 0;
    }

    d.dark = IsDarkTheme();
    d.bg = d.dark ? RGB(0x2B, 0x2B, 0x2B) : RGB(0xF9, 0xF9, 0xF9);
    d.foot = d.dark ? RGB(0x20, 0x20, 0x20) : RGB(0xF0, 0xF0, 0xF0);
    d.text = d.dark ? RGB(0xFF, 0xFF, 0xFF) : RGB(0x1A, 0x1A, 0x1A);
    d.muted = d.dark ? RGB(0xB5, 0xB5, 0xB5) : RGB(0x5F, 0x5F, 0x5F);
    d.line = d.dark ? RGB(0x45, 0x45, 0x45) : RGB(0xE3, 0xE3, 0xE3);
    d.bgBrush = CreateSolidBrush(d.bg);

    POINT pt;
    GetCursorPos(&pt);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
    d.hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_APPWINDOW, kDlgClass,
                             L"Game Mode",
                             WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN,
                             mi.rcWork.left + 100, mi.rcWork.top + 100, 100, 100,
                             nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (d.hwnd) {
        using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
        auto getDpi = UserProc<GetDpiForWindow_t>("GetDpiForWindow");
        d.dpi = getDpi ? (int)getDpi(d.hwnd) : 96;
        if (d.dpi < 96) {
            d.dpi = 96;
        }
        MakeFonts();

        d.titleWnd = MakeChild(L"STATIC", spec.title, SS_LEFT | SS_NOPREFIX, 0);
        SendMessageW(d.titleWnd, WM_SETFONT, (WPARAM)d.fontTitle, TRUE);
        d.summaryWnd = MakeChild(L"STATIC", L"", SS_LEFT | SS_NOPREFIX, 0);
        for (const auto& line : spec.info) {
            d.infoWnds.push_back(MakeChild(L"STATIC", line, SS_LEFT | SS_NOPREFIX, 0));
        }
        d.advBtn = MakeChild(L"BUTTON", L"Advanced options", WS_TABSTOP | BS_PUSHBUTTON, kIdAdv);
        d.allBtn = MakeChild(L"BUTTON", L"Select all", WS_TABSTOP | BS_3STATE, kIdAll);
        for (size_t i = 0; i < spec.items.size(); i++) {
            HWND h = MakeChild(L"BUTTON", spec.items[i].text, WS_TABSTOP | BS_AUTOCHECKBOX,
                               kIdItem0 + (int)i);
            SendMessageW(h, BM_SETCHECK, BST_CHECKED, 0);
            d.itemWnds.push_back(h);
        }
        d.hintWnd = MakeChild(L"STATIC", spec.hint, SS_LEFT | SS_NOPREFIX, 0);
        d.okBtn = MakeChild(L"BUTTON", spec.okText, WS_TABSTOP | WS_GROUP | BS_DEFPUSHBUTTON, IDOK);
        d.cancelBtn = MakeChild(L"BUTTON", L"Cancel", WS_TABSTOP | BS_PUSHBUTTON, IDCANCEL);

        if (d.dark) {
            using SetWindowTheme_t = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);
            auto setTheme = (SetWindowTheme_t)GetProcAddress(LoadLibraryW(L"uxtheme.dll"), "SetWindowTheme");
            if (setTheme) {
                for (HWND h : {d.advBtn, d.allBtn, d.okBtn, d.cancelBtn}) {
                    setTheme(h, L"DarkMode_Explorer", nullptr);
                }
                for (HWND h : d.itemWnds) {
                    setTheme(h, L"DarkMode_Explorer", nullptr);
                }
            }
        }
        BOOL dark = d.dark;
        DwmSetWindowAttribute(d.hwnd, 20, &dark, sizeof(dark));
        COLORREF cap = d.bg;
        DwmSetWindowAttribute(d.hwnd, 35, &cap, sizeof(cap));

        UpdateSummary();
        Layout();
        CenterOnMonitor();
        ShowWindow(d.hwnd, SW_SHOW);
        SetForegroundWindow(d.hwnd);
        SetFocus(d.okBtn);

        MSG m;
        while (!d.done) {
            BOOL r = GetMessageW(&m, nullptr, 0, 0);
            if (r <= 0) {
                if (r == 0) {
                    PostQuitMessage((int)m.wParam);
                }
                break;
            }
            if (!m.hwnd) {  // messages meant for the hotkey loop that is waiting for us
                if (m.message == WM_APP_STOP) {
                    d.stopRequested = true;
                    d.done = true;
                } else if (m.message == WM_APP_RELOAD) {
                    d.reloadRequested = true;
                }
                continue;
            }
            if (!IsDialogMessageW(d.hwnd, &m)) {
                TranslateMessage(&m);
                DispatchMessageW(&m);
            }
        }

        res.ok = d.result && !d.stopRequested;
        for (size_t i = 0; i < d.itemWnds.size(); i++) {
            res.checked[i] = SendMessageW(d.itemWnds[i], BM_GETCHECK, 0, 0) == BST_CHECKED;
        }
        DestroyWindow(d.hwnd);
    }
    if (d.font) {
        DeleteObject(d.font);
    }
    if (d.fontTitle) {
        DeleteObject(d.fontTitle);
    }
    DeleteObject(d.bgBrush);
    if (actCtxOn) {
        DeactivateActCtx(0, cookie);
    }
    if (actCtx) {
        ReleaseActCtx(actCtx);
    }
    if (setDpiCtx && oldDpiCtx) {
        setDpiCtx(oldDpiCtx);
    }
    if (d.stopRequested) {
        PostThreadMessageW(GetCurrentThreadId(), WM_APP_STOP, 0, 0);
    }
    if (d.reloadRequested) {
        PostThreadMessageW(GetCurrentThreadId(), WM_APP_RELOAD, 0, 0);
    }
    g_dlg = nullptr;
    return res;
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
    bool forced = false;        // listed under "Apps to force close"
    bool background = false;    // no taskbar window: a tray or background app
    bool windowless = false;    // no window at all: it cannot be asked to close
};

// The name the program calls itself ("Google Chrome"), else the file name.
std::wstring DisplayName(const std::wstring& path) {
    DWORD ignored = 0;
    DWORD size = GetFileVersionInfoSizeW(path.c_str(), &ignored);
    if (size) {
        std::vector<BYTE> buf(size);
        struct Translation {
            WORD lang;
            WORD codepage;
        }* tr = nullptr;
        UINT len = 0;
        if (GetFileVersionInfoW(path.c_str(), 0, size, buf.data()) &&
            VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation", (LPVOID*)&tr, &len) &&
            len >= sizeof(Translation)) {
            WCHAR sub[64];
            swprintf_s(sub, L"\\StringFileInfo\\%04x%04x\\FileDescription", tr[0].lang,
                       tr[0].codepage);
            PWSTR value = nullptr;
            UINT vlen = 0;
            if (VerQueryValueW(buf.data(), sub, (LPVOID*)&value, &vlen) && value &&
                vlen > 1) {
                std::wstring name = Trim(value);
                if (!name.empty()) {
                    return name;
                }
            }
        }
    }
    std::wstring n = BaseName(path);
    if (n.size() > 4 && Lower(n.substr(n.size() - 4)) == L".exe") {
        n.resize(n.size() - 4);
    }
    return n;
}

// 2 = Windows GUI program, 3 = console program. Console tools (git, node,
// shells) are never closed; unreadable files are treated as not-GUI.
bool IsGuiExe(const std::wstring& path) {
    static std::map<std::wstring, int> cache;
    std::wstring key = Lower(path);
    auto it = cache.find(key);
    if (it != cache.end()) {
        return it->second == 2;
    }
    int subsystem = 0;
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           nullptr, OPEN_EXISTING, 0, nullptr);
    if (h != INVALID_HANDLE_VALUE) {
        BYTE dos[64];
        DWORD got = 0;
        if (ReadFile(h, dos, sizeof(dos), &got, nullptr) && got == sizeof(dos) &&
            dos[0] == 'M' && dos[1] == 'Z') {
            LONG pe = *(LONG*)(dos + 0x3C);
            BYTE hdr[96];
            if (pe > 0 && pe < (1 << 20) &&
                SetFilePointer(h, pe, nullptr, FILE_BEGIN) != INVALID_SET_FILE_POINTER &&
                ReadFile(h, hdr, sizeof(hdr), &got, nullptr) && got == sizeof(hdr) &&
                hdr[0] == 'P' && hdr[1] == 'E') {
                subsystem = *(WORD*)(hdr + 4 + 20 + 68);
            }
        }
        CloseHandle(h);
    }
    cache[key] = subsystem;
    return subsystem == 2;
}

bool CanControl(DWORD pid) {
    HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!h) {
        return false;  // elevated or protected: it cannot be closed from here
    }
    CloseHandle(h);
    return true;
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

    // Processes that own a taskbar window (a Store app's window is owned by
    // ApplicationFrameHost, so it is mapped back to the app's own process).
    std::set<DWORD> taskbarPids;
    for (const auto& w : windows) {
        if (!w.taskbar) {
            continue;
        }
        DWORD pid = w.pid;
        auto it = byPid.find(pid);
        if (it != byPid.end() && ExeKey(it->second->exe) == L"applicationframehost") {
            pid = StoreAppPidOfFrame(w.hwnd, w.pid);
        }
        if (pid) {
            taskbarPids.insert(pid);
        }
    }

    // A program started by an app you keep (Steam's web helper, a launcher's
    // overlay) stays with it. Windows itself does not count as a keeper here,
    // or everything started by Explorer would be kept.
    auto startedByKeptApp = [&](const ProcInfo& p) {
        DWORD cur = p.pid;
        for (int hop = 0; hop < 12; hop++) {
            auto child = byPid.find(cur);
            if (child == byPid.end()) {
                return false;
            }
            auto parent = byPid.find(child->second->parent);
            if (parent == byPid.end() || parent->second->created >= child->second->created) {
                return false;
            }
            const wchar_t* why = WhyKeep(parent->second->exe, parent->second->path, g_windowsDir, s);
            if (why && (wcscmp(why, L"in Apps to keep") == 0 || wcscmp(why, L"in Folders to keep") == 0 ||
                        wcscmp(why, L"in a game folder") == 0)) {
                return true;
            }
            cur = parent->first;
        }
        return false;
    };

    // 1) Listed apps, then 2) every other program that is not needed for gaming.
    std::set<std::wstring> keptNames;
    for (const auto& p : procs) {
        if (p.pid == self || !CanControl(p.pid)) {
            continue;
        }
        const bool listed = InNameList(s.closeApps, p.exe) && !IsShellProcess(p.exe);
        if (!listed) {
            if (!s.closeOthers || IsShellProcess(p.exe) || LooksLikeHelper(p.exe)) {
                continue;
            }
            if (!taskbarPids.count(p.pid) && !s.closeBackground) {
                continue;
            }
            const wchar_t* why = WhyKeep(p.exe, p.path, g_windowsDir, s);
            if (why) {
                keptNames.insert(DisplayName(p.path) + L" (" + why + L")");
                continue;
            }
            if (!IsGuiExe(p.path)) {
                continue;
            }
            if (startedByKeptApp(p)) {
                keptNames.insert(DisplayName(p.path) + L" (started by an app to keep)");
                continue;
            }
        }
        addPid(p);
    }
    if (kept) {
        kept->assign(keptNames.begin(), keptNames.end());
    }

    // A helper that was started by another program on the list belongs to it
    // (PowerToys' modules, a browser's crash handler): it is listed once, under
    // the program that started it. The parent must be older than the child, so
    // a reused process id never links two unrelated programs.
    for (bool merged = true; merged;) {
        merged = false;
        std::map<DWORD, std::wstring> keyOfPid;
        for (const auto& kv : groups) {
            for (const auto& pc : kv.second.pids) {
                keyOfPid[pc.first] = kv.first;
            }
        }
        for (auto it = groups.begin(); it != groups.end(); ++it) {
            std::wstring parentKey;
            for (const auto& pc : it->second.pids) {
                DWORD cur = pc.first;
                for (int hop = 0; hop < 12 && parentKey.empty(); hop++) {
                    auto child = byPid.find(cur);
                    if (child == byPid.end()) {
                        break;
                    }
                    auto parent = byPid.find(child->second->parent);
                    if (parent == byPid.end() ||
                        parent->second->created >= child->second->created) {
                        break;
                    }
                    auto owner = keyOfPid.find(parent->first);
                    if (owner != keyOfPid.end() && owner->second != it->first) {
                        parentKey = owner->second;
                    }
                    cur = parent->first;
                }
                if (!parentKey.empty()) {
                    break;
                }
            }
            if (!parentKey.empty()) {
                for (const auto& pc : it->second.pids) {
                    groups[parentKey].pids[pc.first] = pc.second;
                }
                groups.erase(it);
                merged = true;
                break;  // the iterator is gone: rebuild the pid map and start over
            }
        }
    }

    std::vector<AppGroup> result;
    for (auto& kv : groups) {
        AppGroup& g = kv.second;
        g.background = true;
        for (const auto& pc : g.pids) {
            if (taskbarPids.count(pc.first)) {
                g.background = false;
            }
        }
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
        g.windowless = g.windows.empty();
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

ChecklistSpec OnChecklist(const std::vector<ChecklistItem>& items,
                          bool changePlan,
                          bool planNotSet,
                          const std::wstring& planLabel,
                          size_t gameCount) {
    ChecklistSpec spec;
    spec.title = L"Turn on Game Mode?";
    spec.verb = L"Closes";
    spec.okText = L"Turn on";
    spec.items = items;
    spec.info.push_back(changePlan ? L"Switch power plan to " + planLabel
                        : planNotSet
                            ? L"Power plan: not changed"
                            : L"Power plan: already " + planLabel + L" (no change)");
    spec.info.push_back(gameCount ? L"High priority for " + std::to_wstring(gameCount) +
                                        L" running game process(es)"
                                  : L"No game running yet. Launch it after this.");
    spec.hint = L"Press the hotkey again to restore everything exactly as it is now.";
    return spec;
}

ChecklistSpec OffChecklist(const std::vector<ChecklistItem>& items,
                           bool planRestored) {
    ChecklistSpec spec;
    spec.title = L"Turn off Game Mode?";
    spec.verb = L"Reopens";
    spec.okText = L"Turn off";
    spec.items = items;
    spec.info.push_back(planRestored ? L"Restores the power plan from before"
                                     : L"Power plan: not changed by Game Mode");
    spec.hint = L"Everything ticked above is put back as it was before.";
    return spec;
}

// What the checklist shows next to an app name.
std::wstring NoteFor(const AppGroup& g) {
    if (g.forced) {
        return L"force close";
    }
    if (g.windowless) {
        return L"ends process";
    }
    return g.background ? L"background" : L"";
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

    if (s.confirm) {
        std::vector<ChecklistItem> items;
        for (const auto& g : groups) {
            items.push_back({g.name, NoteFor(g)});
        }
        ChecklistResult choice = ShowChecklist(
            OnChecklist(items, changePlan, target.empty(), planLabel, games.size()));
        if (!choice.ok) {
            Wh_Log(L"ON cancelled");
            return;
        }
        std::vector<AppGroup> picked;  // only what was left ticked
        for (size_t i = 0; i < groups.size(); i++) {
            if (choice.checked[i]) {
                picked.push_back(groups[i]);
            } else {
                Wh_Log(L"Left alone this time: %s", groups[i].name.c_str());
            }
        }
        groups = picked;
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

    // An app may be ended (not just asked to close) when it is on the force
    // list, or when it has no window to ask and the user saw it in the checklist
    // and left it ticked.
    auto mayEnd = [&](const AppGroup& g) {
        return g.forced || (g.windowless && s.confirm);
    };
    auto endProcesses = [](const AppGroup& g) {
        for (const auto& pc : g.pids) {
            HANDLE h = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pc.first);
            if (h) {
                TerminateProcess(h, 1);
                WaitForSingleObject(h, 2000);
                CloseHandle(h);
            }
        }
    };

    // Close everything at once: one round of WM_CLOSE, one shared wait.
    for (const auto& g : groups) {
        Wh_Log(L"Closing %s (%zu process(es), %zu window(s))", g.name.c_str(),
               g.pids.size(), g.windows.size());
        for (HWND w : g.windows) {
            PostMessageW(w, WM_CLOSE, 0, 0);
        }
        if (g.windowless && mayEnd(g)) {
            Wh_Log(L"Ending %s (no window to close)", g.name.c_str());
            endProcesses(g);
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
            endProcesses(g);
        }
        if (!done[i]) {
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
    std::set<std::wstring> skip;  // apps the user unticked: not reopened
    if (interactive && g_settings.confirm) {
        std::vector<ChecklistItem> items;
        std::vector<std::wstring> itemPaths;
        for (const auto& a : sn.apps) {
            if (!running.count(Lower(a.path))) {
                items.push_back({a.name, L""});
                itemPaths.push_back(Lower(a.path));
            }
        }
        ChecklistResult choice = ShowChecklist(OffChecklist(items, !sn.plan.empty()));
        if (!choice.ok) {
            Wh_Log(L"OFF cancelled");
            return;
        }
        for (size_t i = 0; i < itemPaths.size(); i++) {
            if (!choice.checked[i]) {
                skip.insert(itemPaths[i]);
            }
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
        if (skip.count(Lower(a.path))) {
            Wh_Log(L"Not reopened (your choice): %s", a.name.c_str());
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
