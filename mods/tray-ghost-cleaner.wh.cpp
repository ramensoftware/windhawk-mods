// ==WindhawkMod==
// @id              tray-ghost-cleaner
// @name            Tray Ghost Icon Cleaner
// @description     Removes leftover tray icons of programs that crashed or were killed, the moment they exit. For Windows 10 and older Windows 11 versions; newer Windows 11 already does this itself
// @version         1.0
// @author          Solo_mag
// @github          https://github.com/Solomag
// @include         explorer.exe
// @compilerOptions -luser32 -lshell32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tray Ghost Icon Cleaner

**For Windows 10 and older Windows 11 versions.** Newer Windows 11 versions with the
new (XAML) notification area already track the owner processes and remove these icons
themselves, so the mod isn't needed there. See [Compatibility](#compatibility).

When a program crashes or is terminated, it never removes its notification area (tray)
icon. Explorer only notices that the owner is gone when the mouse passes over the icon,
so "ghost" icons and duplicates pile up until you hover over the tray.

![Demo](https://raw.githubusercontent.com/Solomag/windhawk-assets/main/tray-ghost-cleaner/demo.gif)

## How it works

Programs add and remove tray icons with `Shell_NotifyIcon`, which delivers each request
to the taskbar window as a `WM_COPYDATA` message. The mod watches those messages inside
Explorer, together with Explorer's answer to each of them, so it knows exactly which icon
belongs to which process — including icons identified by GUID — without reading any
internal Explorer data.

* For every process that owns an icon, the mod registers a wait on the process handle
  (no polling).
* When the process exits, the mod waits 1.5 s and then removes the icons that are still
  there with a regular `Shell_NotifyIcon(NIM_DELETE)`, using the same identity (window +
  ID, or GUID) the program registered them with. An icon is only removed when the window
  it belongs to is gone as well.
* A light 30 s check catches owners the mod couldn't open or wait on: it only looks at
  its own list, not at the tray.
* When the mod is loaded together with Explorer (the usual case), it sees every icon
  from the start. When it is enabled while Explorer is already running, it learns the
  existing icons the next time their program updates them (tooltip, image or state).
  Optionally, it can send the documented `TaskbarCreated` notification once so that
  running programs re-register their icons right away. Every program receives it as if
  Explorer had restarted, so it's off by default.
* Every removed ghost is logged — program path, PID and tooltip — to the Windhawk log
  and, optionally, to `ghosts.log` in the mod's storage folder (its full path is written
  to the Windhawk log), so you can find out which program keeps leaving them.

## Compared to other approaches

| | This mod | Tray refresh utilities | Tray reset tools | Restarting Explorer |
|---|---|---|---|---|
| Runs as | part of Explorer, no extra process | separate background program | one-off program | manual |
| Trigger | the owner process exiting | timer or on demand | when you run it | when you do it |
| Method | `NIM_DELETE` with the icon's own identity | simulated mouse moves over the tray, or reading Explorer's toolbar memory from another process | resets the tray settings in the registry | full restart |
| Icons registered by GUID | removed (if the mod was enabled while Explorer was running: once their program has updated them) | depends on the tool | — | removed |
| Tells you who left the icon | yes, program and PID | no | no | no |
| Side effects | none by default; the optional `TaskbarCreated` on load reaches every program, as if Explorer had restarted | depends on the tool; may lag behind | clears your icon visibility settings | taskbar flickers, open Explorer windows close |

## Compatibility

* Windows 10 — tested on 22H2.
* Older Windows 11 versions whose notification area still behaves like Windows 10's,
  and Windows 11 with the Windows 10 taskbar restored.
* Not needed on newer Windows 11 versions with the XAML notification area: Explorer
  there tracks the processes that own tray icons and removes the icons itself.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- logToFile: false
  $name: Log removed icons to a file
  $description: >-
    Also append removed icons to ghosts.log (UTF-8) in the mod's storage folder. The
    file's full path is written to the Windhawk log.
- refreshOnLoad: false
  $name: Learn existing icons on load
  $description: >-
    When the mod is enabled while Explorer is already running, send TaskbarCreated
    once so that running programs re-register their icons. Every program receives it
    as if Explorer had restarted, and a few re-initialize or misbehave. Without it,
    icons that existed before the mod was loaded are learned only when their program
    updates them.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <windhawk_utils.h>

#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace {

// ---- What Shell_NotifyIcon sends to the taskbar in WM_COPYDATA (dwData == 1). ----
// Handles are 32-bit so that 32- and 64-bit programs share one format.
constexpr ULONG_PTR kCopyDataNotifyIcon = 1;
constexpr DWORD kTraySignature = 0x34753423;

#pragma pack(push, 1)
struct NotifyIconData32 {
    DWORD cbSize;
    DWORD hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    DWORD hIcon;
    WCHAR szTip[128];
    DWORD dwState;
    DWORD dwStateMask;
    WCHAR szInfo[256];
    UINT uVersion;
    WCHAR szInfoTitle[64];
    DWORD dwInfoFlags;
    GUID guidItem;
    DWORD hBalloonIcon;
};
struct TrayNotifyData {
    DWORD dwSignature;
    DWORD dwMessage;
    NotifyIconData32 nid;
};
#pragma pack(pop)

// cbSize is the size the program declared, which may refer to its own (64-bit or older)
// structure, so it is only used as an extra upper bound next to cbData. Programs that
// use a structure without guidItem never set NIF_GUID.
constexpr DWORD kGuidEnd = offsetof(NotifyIconData32, guidItem) + sizeof(GUID);

constexpr DWORD kPeriodicMs = 30000;
constexpr DWORD kGraceMs = 1500;

// Program path of an owner, shared by its icons so that a ghost is always reported with
// the program that added it, even if its pid has been reused since.
using ExePtr = std::shared_ptr<std::wstring>;

struct TrackedIcon {
    HWND hwnd;
    UINT id;
    bool byGuid;
    GUID guid;
    DWORD pid;
    ExePtr exe;
    std::wstring tip;
};

struct Owner {
    HANDLE process = nullptr;
    HANDLE wait = nullptr;
    bool pending = true;  // the worker hasn't opened the process yet
    FILETIME seen = {};   // when its first icon was seen (detects pid reuse)
    ExePtr exe = std::make_shared<std::wstring>(L"?");
};

SRWLOCK g_lock = SRWLOCK_INIT;            // guards everything below
std::vector<TrackedIcon> g_icons;
std::map<DWORD, Owner> g_owners;          // pid -> wait on its process
std::vector<Owner> g_retired;             // replaced owners; the worker closes them
std::vector<DWORD> g_exited;              // pids reported by the wait callbacks
std::vector<DWORD> g_newPids;             // pids that still need a wait
std::wstring g_logFile;
bool g_refreshOnLoad = false;

HANDLE g_stop = nullptr;
HANDLE g_wake = nullptr;
std::atomic<HANDLE> g_thread{nullptr};    // started with the first taskbar
std::atomic<bool> g_workerStarted{false};
std::atomic<HWND> g_tray{nullptr};        // subclassed taskbar of this process

HWND FromHandle32(DWORD h) {
    return (HWND)(LONG_PTR)(LONG)h;  // sign-extended, as WOW64 does
}

bool SameIdentity(const TrackedIcon& a, bool byGuid, const GUID& guid, HWND hwnd, UINT id) {
    if (byGuid || a.byGuid)
        return byGuid && a.byGuid && IsEqualGUID(a.guid, guid);
    return a.hwnd == hwnd && a.id == id;
}

void ReleaseOwner(const Owner& owner) {
    if (owner.wait)
        UnregisterWaitEx(owner.wait, INVALID_HANDLE_VALUE);  // waits for the callback
    if (owner.process)
        CloseHandle(owner.process);
}

void Report(const std::wstring& line) {
    Wh_Log(L"%s", line.c_str());

    AcquireSRWLockShared(&g_lock);
    std::wstring path = g_logFile;
    ReleaseSRWLockShared(&g_lock);
    if (path.empty())
        return;

    SYSTEMTIME t;
    GetLocalTime(&t);
    wchar_t stamp[32];
    wsprintfW(stamp, L"%02d.%02d.%04d %02d:%02d:%02d | ", t.wDay, t.wMonth,
              t.wYear, t.wHour, t.wMinute, t.wSecond);
    std::wstring full = stamp + line + L"\r\n";
    int n = WideCharToMultiByte(CP_UTF8, 0, full.c_str(), (int)full.size(),
                                nullptr, 0, nullptr, nullptr);
    if (n <= 0)
        return;
    std::string utf8(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, full.c_str(), (int)full.size(), &utf8[0], n,
                        nullptr, nullptr);
    HANDLE f = CreateFileW(path.c_str(), FILE_APPEND_DATA,
                           FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                           OPEN_ALWAYS, 0, nullptr);
    if (f != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(f, utf8.data(), (DWORD)utf8.size(), &written, nullptr);
        CloseHandle(f);
    }
}

void LoadSettings() {
    // The log file lives in the mod's storage folder, which is removed with the mod.
    std::wstring path;
    WCHAR dir[MAX_PATH];
    if (Wh_GetIntSetting(L"logToFile") && Wh_GetModStoragePath(dir, ARRAYSIZE(dir))) {
        path = std::wstring(dir) + L"\\ghosts.log";
        Wh_Log(L"Logging removed icons to %s", path.c_str());
    }
    bool refresh = Wh_GetIntSetting(L"refreshOnLoad") != 0;
    AcquireSRWLockExclusive(&g_lock);
    g_logFile = path;
    g_refreshOnLoad = refresh;
    ReleaseSRWLockExclusive(&g_lock);
}

// The owner entry of a pid, created (and queued for a wait) if needed. Lock held.
Owner& OwnerFor(DWORD pid) {
    auto it = g_owners.find(pid);
    if (it != g_owners.end()) {
        // An owner whose process already exited means the pid now belongs to a new
        // process: retire the old entry (its icons keep their program path).
        HANDLE process = it->second.process;
        if (!process || WaitForSingleObject(process, 0) != WAIT_OBJECT_0)
            return it->second;
        g_retired.push_back(it->second);
        g_owners.erase(it);
    }
    Owner& owner = g_owners[pid];
    GetSystemTimePreciseAsFileTime(&owner.seen);
    g_newPids.push_back(pid);
    SetEvent(g_wake);
    return owner;
}

// ---- Runs on Explorer's taskbar thread, after Explorer handled the request. ----
void OnNotifyIcon(const COPYDATASTRUCT* cds, bool accepted) {
    if (cds->cbData < offsetof(TrayNotifyData, nid) + offsetof(NotifyIconData32, szTip))
        return;
    const auto* data = (const TrayNotifyData*)cds->lpData;
    if (data->dwSignature != kTraySignature)
        return;
    const NotifyIconData32& nid = data->nid;
    DWORD size = cds->cbData - offsetof(TrayNotifyData, nid);
    if (nid.cbSize < size)
        size = nid.cbSize;

    bool byGuid = (nid.uFlags & NIF_GUID) && size >= kGuidEnd;
    // A rejected request with a GUID may come from another program (a second
    // instance, or a copy at another path): it says nothing about the icon's owner.
    if (byGuid && !accepted)
        return;
    GUID guid = byGuid ? nid.guidItem : GUID{};
    HWND hwnd = FromHandle32(nid.hWnd);

    AcquireSRWLockExclusive(&g_lock);
    auto it = g_icons.begin();
    for (; it != g_icons.end(); ++it)
        if (SameIdentity(*it, byGuid, guid, hwnd, nid.uID))
            break;

    switch (data->dwMessage) {
        case NIM_ADD:
        case NIM_MODIFY: {
            // An accepted request proves the icon exists, even if it was added before
            // the mod was loaded. A rejected NIM_ADD by window + ID usually means the
            // icon already exists (a program re-adding it after TaskbarCreated); if it
            // doesn't, the later NIM_DELETE simply fails.
            if (!accepted && data->dwMessage == NIM_MODIFY)
                break;
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (!pid)
                break;
            Owner& owner = OwnerFor(pid);
            if (it == g_icons.end()) {
                g_icons.push_back(TrackedIcon{hwnd, nid.uID, byGuid, guid, pid, owner.exe, L""});
                it = g_icons.end() - 1;
            } else if (it->pid != pid || it->exe != owner.exe) {
                it->pid = pid;
                it->exe = owner.exe;
            }
            it->hwnd = hwnd;
            if ((nid.uFlags & NIF_TIP) && size >= offsetof(NotifyIconData32, dwState)) {
                WCHAR tip[128];
                lstrcpynW(tip, nid.szTip, 128);
                for (WCHAR* c = tip; *c; c++)
                    if (*c == L'\r' || *c == L'\n')
                        *c = L' ';
                it->tip = tip;
            }
            break;
        }
        case NIM_DELETE:
            // A rejected delete by window + ID means the icon isn't there either.
            if (it != g_icons.end())
                g_icons.erase(it);
            break;
    }
    ReleaseSRWLockExclusive(&g_lock);
}

LRESULT CALLBACK TraySubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                              DWORD_PTR) {
    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    if (uMsg == WM_COPYDATA) {
        // The result is what Shell_NotifyIcon returns to the program.
        const auto* cds = (const COPYDATASTRUCT*)lParam;
        if (cds && cds->dwData == kCopyDataNotifyIcon && cds->lpData)
            OnNotifyIcon(cds, result != 0);
    }
    return result;
}

// ---- Thread pool: a tracked process exited. ----
VOID CALLBACK OnProcessExit(PVOID param, BOOLEAN) {
    AcquireSRWLockExclusive(&g_lock);
    g_exited.push_back((DWORD)(ULONG_PTR)param);
    ReleaseSRWLockExclusive(&g_lock);
    SetEvent(g_wake);
}

// ---- Worker thread. ----
void StartWaits() {
    AcquireSRWLockExclusive(&g_lock);
    std::vector<DWORD> pids;
    pids.swap(g_newPids);
    std::vector<Owner> retired;
    retired.swap(g_retired);
    ReleaseSRWLockExclusive(&g_lock);

    for (const Owner& owner : retired)
        ReleaseOwner(owner);

    for (DWORD pid : pids) {
        AcquireSRWLockShared(&g_lock);
        auto it = g_owners.find(pid);
        bool pending = it != g_owners.end() && it->second.pending;
        ExePtr entry = pending ? it->second.exe : nullptr;  // identifies this entry
        FILETIME seen = pending ? it->second.seen : FILETIME{};
        ReleaseSRWLockShared(&g_lock);
        if (!pending)
            continue;

        HANDLE process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        HANDLE info = process ? process : OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        std::wstring exe = L"?";
        if (info) {
            // A process created after the icon was seen reuses the pid of the owner,
            // which has already exited.
            FILETIME created, exited, kernel, user;
            if (GetProcessTimes(info, &created, &exited, &kernel, &user) &&
                CompareFileTime(&created, &seen) > 0) {
                CloseHandle(info);
                process = info = nullptr;
            } else {
                wchar_t path[MAX_PATH];
                DWORD n = MAX_PATH;
                if (QueryFullProcessImageNameW(info, 0, path, &n))
                    exe = path;
            }
        }
        if (info && info != process)
            CloseHandle(info);  // can't be waited on; the periodic check covers it
        HANDLE wait = nullptr;
        if (process &&
            !RegisterWaitForSingleObject(&wait, process, OnProcessExit,
                                         (PVOID)(ULONG_PTR)pid, INFINITE,
                                         WT_EXECUTEONLYONCE)) {
            wait = nullptr;
            CloseHandle(process);
            process = nullptr;
        }

        AcquireSRWLockExclusive(&g_lock);
        it = g_owners.find(pid);
        if (it != g_owners.end() && it->second.exe == entry && it->second.pending) {
            it->second.process = process;
            it->second.wait = wait;
            it->second.pending = false;
            *it->second.exe = exe;
            process = wait = nullptr;
        }
        ReleaseSRWLockExclusive(&g_lock);
        ReleaseOwner(Owner{process, wait});
    }
}

// The owner of an icon is gone if its window is gone or now belongs to another process.
bool OwnerGone(const TrackedIcon& icon) {
    DWORD pid = 0;
    return !IsWindow(icon.hwnd) ||
           !GetWindowThreadProcessId(icon.hwnd, &pid) || pid != icon.pid;
}

// Removes ghost icons of the given pids (or of any pid whose owner is gone, if `all`).
void Clean(const std::vector<DWORD>& pids, bool all, const wchar_t* reason) {
    std::vector<std::pair<TrackedIcon, std::wstring>> ghosts;
    std::vector<Owner> finished;

    AcquireSRWLockExclusive(&g_lock);
    for (auto it = g_icons.begin(); it != g_icons.end();) {
        bool listed = false;
        for (DWORD pid : pids)
            if (pid == it->pid)
                listed = true;
        if ((listed || all) && OwnerGone(*it)) {
            ghosts.emplace_back(*it, *it->exe);
            it = g_icons.erase(it);
        } else {
            ++it;
        }
    }
    // Forget owners that have no icons left and whose process is gone (or unknown).
    for (auto it = g_owners.begin(); it != g_owners.end();) {
        bool hasIcons = false;
        for (auto& icon : g_icons)
            if (icon.pid == it->first)
                hasIcons = true;
        bool exited = it->second.process &&
                      WaitForSingleObject(it->second.process, 0) == WAIT_OBJECT_0;
        if (!hasIcons && (exited || !it->second.process)) {
            finished.push_back(it->second);
            it = g_owners.erase(it);
        } else {
            ++it;
        }
    }
    ReleaseSRWLockExclusive(&g_lock);

    for (const Owner& owner : finished)
        ReleaseOwner(owner);

    for (const auto& [g, exe] : ghosts) {
        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(nid);
        nid.hWnd = g.hwnd;
        nid.uID = g.id;
        if (g.byGuid) {
            nid.uFlags = NIF_GUID;
            nid.guidItem = g.guid;
        }
        // Succeeds only if the icon really was still in the tray, i.e. a ghost.
        if (Shell_NotifyIconW(NIM_DELETE, &nid)) {
            Report(L"ghost | " + exe + L" | pid " + std::to_wstring(g.pid) +
                   L" | \"" + g.tip + L"\" | " + (g.byGuid ? L"by GUID" : L"by window") +
                   L" | " + reason);
        }
    }
}

// Asks running programs to re-register their tray icons. Explorer sends this on every
// start and programs are required to handle it.
void BroadcastTaskbarCreated() {
    UINT taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    if (taskbarCreated)
        PostMessageW(HWND_BROADCAST, taskbarCreated, 0, 0);
}

DWORD WINAPI Worker(LPVOID) {
    HANDLE handles[] = {g_stop, g_wake};
    ULONGLONG nextCheck = GetTickCount64() + kPeriodicMs;
    for (;;) {
        // The periodic check runs on a fixed schedule, however often the event fires.
        ULONGLONG now = GetTickCount64();
        DWORD timeout = now >= nextCheck ? 0 : (DWORD)(nextCheck - now);
        if (WaitForMultipleObjects(2, handles, FALSE, timeout) == WAIT_OBJECT_0)
            break;

        StartWaits();

        AcquireSRWLockShared(&g_lock);
        bool anyExited = !g_exited.empty();
        ReleaseSRWLockShared(&g_lock);
        if (anyExited) {
            // Short grace period, in case the program is restarted and takes its
            // icon back.
            if (WaitForSingleObject(g_stop, kGraceMs) == WAIT_OBJECT_0)
                break;
            AcquireSRWLockExclusive(&g_lock);
            std::vector<DWORD> exited;
            exited.swap(g_exited);
            ReleaseSRWLockExclusive(&g_lock);
            Clean(exited, false, L"owner exited");
        }

        if (GetTickCount64() >= nextCheck) {
            Clean({}, true, L"periodic check");
            nextCheck = GetTickCount64() + kPeriodicMs;
        }
    }
    return 0;
}

// Other explorer.exe processes (separate folder windows) never create a taskbar, so the
// worker is only started once this process has one.
void StartWorker() {
    if (g_workerStarted.exchange(true))
        return;
    HANDLE thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!thread)
        Wh_Log(L"Failed to start the worker thread: %u", GetLastError());
    g_thread = thread;
}

bool AttachToTray(HWND tray) {
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(tray, TraySubclass, 0)) {
        Wh_Log(L"Failed to subclass the taskbar");
        return false;
    }
    g_tray = tray;
    StartWorker();
    return true;
}

// Explorer creating (or re-creating) its taskbar: subclass it right away, before
// programs get the chance to register their icons with it.
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName,
                                 DWORD style, int x, int y, int width, int height,
                                 HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(exStyle, className, windowName, style, x, y,
                                         width, height, parent, menu, instance, param);
    if (hwnd && !IS_INTRESOURCE(className) && lstrcmpiW(className, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar created");
        AttachToTray(hwnd);
    }
    return hwnd;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original))
        return FALSE;
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stop || !g_wake) {
        if (g_wake)
            CloseHandle(g_wake);
        if (g_stop)
            CloseHandle(g_stop);
        return FALSE;
    }
    return TRUE;
}

// Hooks are active now: a taskbar created from here on is caught by the hook.
void Wh_ModAfterInit() {
    // The taskbar must belong to this process: other explorer.exe processes (separate
    // folder windows) only see the main process's taskbar.
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    if (!tray || !GetWindowThreadProcessId(tray, &pid) || pid != GetCurrentProcessId())
        return;
    if (g_tray == tray || !AttachToTray(tray))
        return;

    // Explorer was already running: ask programs to register their icons again.
    AcquireSRWLockShared(&g_lock);
    bool refresh = g_refreshOnLoad;
    ReleaseSRWLockShared(&g_lock);
    if (refresh)
        BroadcastTaskbarCreated();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    // Remove the subclass first: it runs on the taskbar thread, so no tray message is
    // being handled once it returns. Then stop the worker; every wait in it is
    // interruptible, so this is quick.
    HWND tray = g_tray;
    if (tray)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(tray, TraySubclass);
    SetEvent(g_stop);
    if (HANDLE thread = g_thread) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }

    // Cancel the process waits and wait for any running callback to finish.
    for (auto& o : g_owners)
        ReleaseOwner(o.second);
    g_owners.clear();
    for (auto& o : g_retired)
        ReleaseOwner(o);
    g_retired.clear();
    CloseHandle(g_wake);
    CloseHandle(g_stop);
}
