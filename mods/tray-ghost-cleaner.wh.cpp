// ==WindhawkMod==
// @id              tray-ghost-cleaner
// @name            Tray Ghost Icon Cleaner
// @description     Removes leftover tray icons of programs that crashed or were killed, the moment they exit, without hovering the mouse over the tray
// @version         1.0
// @author          Solo_mag
// @github          https://github.com/Solomag
// @include         explorer.exe
// @compilerOptions -luser32 -lshell32 -lkernel32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tray Ghost Icon Cleaner

When a program crashes or is terminated, it never removes its notification area (tray)
icon. Explorer only notices that the owner is gone when the mouse passes over the icon,
so "ghost" icons and duplicates pile up until you hover over the tray.

![Demo](https://raw.githubusercontent.com/Solomag/windhawk-assets/main/tray-ghost-cleaner/demo.gif)

## How it works

Programs add and remove tray icons with `Shell_NotifyIcon`, which delivers each request
to the taskbar window as a `WM_COPYDATA` message. The mod watches those messages inside
Explorer, so it knows exactly which icon belongs to which process — including icons
identified by GUID — without reading any internal Explorer data.

* For every process that owns an icon, the mod registers a wait on the process handle
  (no polling).
* When the process exits, the mod waits 1.5 s (time for a well-behaved program to clean
  up after itself) and removes the icons that are still there with a regular
  `Shell_NotifyIcon(NIM_DELETE)`, using the same identity (window + ID, or GUID) the
  program registered them with.
* A light 30 s check catches owners the mod can't wait on (for example elevated
  processes): it only looks at its own list, not at the tray.
* When the mod is enabled while Explorer is already running, it sends the documented
  `TaskbarCreated` notification once, so running programs re-register their icons and
  the mod learns about them. (Explorer itself sends it on every start; programs are
  required to handle it.)
* Every removed ghost is logged — program path, PID and tooltip — to the Windhawk log
  and, optionally, to a text file, so you can find out which program keeps leaving them.

## Compared to other tools

| | This mod | System Tray Refresh, NoVirusThanks SysTray Refresh, LP TrayIconBuster | Notification Area Cleaner | Restarting Explorer |
|---|---|---|---|---|
| Runs as | part of Explorer, no extra process | separate background program | one-off program | manual |
| Trigger | the owner process exiting | timer: every N seconds or on demand | when you run it | when you do it |
| Method | `NIM_DELETE` with the icon's own identity | simulated mouse moves over the tray, or reading Explorer's toolbar memory from outside | resets the tray settings in the registry | full restart |
| Icons registered by GUID | removed | depends on the hover check | — | removed |
| Tells you who left the icon | yes, program and PID | no | no | no |
| Side effects | none | brief cursor-free hover messages; may lag behind | clears your icon visibility settings | taskbar flickers, open Explorer windows close |

## Compatibility

Windows 10 taskbar, and Windows 11 with the classic (Windows 10) taskbar. The new
Windows 11 taskbar also receives icons through `WM_COPYDATA`, so tracking works there,
but this has not been tested.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- logFile: ""
  $name: Log file
  $description: >-
    Optional path of a text file to append removed icons to (UTF-8).
    Leave empty to log only to the Windhawk log.
- refreshOnLoad: true
  $name: Learn existing icons on load
  $description: >-
    When the mod is enabled while Explorer is already running, send TaskbarCreated
    once so that running programs re-register their icons. Without it, only icons
    added after the mod was loaded are tracked.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <windhawk_utils.h>

#include <map>
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

constexpr DWORD kGuidEnd = offsetof(NotifyIconData32, guidItem) + sizeof(GUID);

struct TrackedIcon {
    HWND hwnd;
    UINT id;
    bool byGuid;
    GUID guid;
    DWORD pid;
    std::wstring tip;
};

struct Owner {
    HANDLE process = nullptr;
    HANDLE wait = nullptr;
    std::wstring exe;
};

SRWLOCK g_lock = SRWLOCK_INIT;            // guards everything below
std::vector<TrackedIcon> g_icons;
std::map<DWORD, Owner> g_owners;          // pid -> wait on its process
std::vector<DWORD> g_exited;              // pids reported by the wait callbacks
std::vector<DWORD> g_newPids;             // pids that still need a wait
std::wstring g_logFile;
bool g_refreshOnLoad = true;

HANDLE g_stop = nullptr;
HANDLE g_wake = nullptr;
HANDLE g_thread = nullptr;
HWND g_tray = nullptr;

HWND FromHandle32(DWORD h) {
    return (HWND)(LONG_PTR)(LONG)h;  // sign-extended, as WOW64 does
}

bool SameIdentity(const TrackedIcon& a, bool byGuid, const GUID& guid, HWND hwnd, UINT id) {
    if (byGuid || a.byGuid)
        return byGuid && a.byGuid && IsEqualGUID(a.guid, guid);
    return a.hwnd == hwnd && a.id == id;
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
    PCWSTR s = Wh_GetStringSetting(L"logFile");
    bool refresh = Wh_GetIntSetting(L"refreshOnLoad") != 0;
    AcquireSRWLockExclusive(&g_lock);
    g_logFile = s ? s : L"";
    g_refreshOnLoad = refresh;
    ReleaseSRWLockExclusive(&g_lock);
    Wh_FreeStringSetting(s);
}

// ---- Runs on Explorer's taskbar thread: must stay short. ----
void OnNotifyIcon(const COPYDATASTRUCT* cds) {
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
            if (it == g_icons.end() && data->dwMessage == NIM_MODIFY)
                break;  // an icon we never saw being added; nothing to learn
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (!pid)
                break;
            if (it == g_icons.end()) {
                g_icons.push_back(TrackedIcon{hwnd, nid.uID, byGuid, guid, pid, L""});
                it = g_icons.end() - 1;
            }
            it->hwnd = hwnd;
            it->pid = pid;
            if ((nid.uFlags & NIF_TIP) && size >= offsetof(NotifyIconData32, dwState)) {
                WCHAR tip[128];
                lstrcpynW(tip, nid.szTip, 128);
                for (WCHAR* c = tip; *c; c++)
                    if (*c == L'\r' || *c == L'\n')
                        *c = L' ';
                it->tip = tip;
            }
            if (!g_owners.count(pid)) {
                g_owners[pid] = Owner{};
                g_newPids.push_back(pid);
                SetEvent(g_wake);
            }
            break;
        }
        case NIM_DELETE:
            if (it != g_icons.end())
                g_icons.erase(it);
            break;
    }
    ReleaseSRWLockExclusive(&g_lock);
}

LRESULT CALLBACK TraySubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                              DWORD_PTR) {
    if (uMsg == WM_COPYDATA) {
        const auto* cds = (const COPYDATASTRUCT*)lParam;
        if (cds && cds->dwData == kCopyDataNotifyIcon && cds->lpData)
            OnNotifyIcon(cds);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
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
    ReleaseSRWLockExclusive(&g_lock);

    for (DWORD pid : pids) {
        Owner owner;
        owner.process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!owner.process)
            owner.process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        owner.exe = L"?";
        if (owner.process) {
            wchar_t path[MAX_PATH];
            DWORD n = MAX_PATH;
            if (QueryFullProcessImageNameW(owner.process, 0, path, &n))
                owner.exe = path;
            if (!RegisterWaitForSingleObject(&owner.wait, owner.process, OnProcessExit,
                                             (PVOID)(ULONG_PTR)pid, INFINITE,
                                             WT_EXECUTEONLYONCE))
                owner.wait = nullptr;  // the periodic check covers it
        }
        AcquireSRWLockExclusive(&g_lock);
        auto it = g_owners.find(pid);
        if (it != g_owners.end() && !it->second.process) {
            it->second = owner;
            owner = Owner{};
        }
        ReleaseSRWLockExclusive(&g_lock);
        if (owner.wait)
            UnregisterWaitEx(owner.wait, INVALID_HANDLE_VALUE);
        if (owner.process)
            CloseHandle(owner.process);
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
    std::vector<TrackedIcon> ghosts;
    std::vector<std::pair<DWORD, Owner>> finished;

    AcquireSRWLockExclusive(&g_lock);
    for (auto it = g_icons.begin(); it != g_icons.end();) {
        bool listed = false;
        for (DWORD pid : pids)
            if (pid == it->pid)
                listed = true;
        if ((listed || all) && OwnerGone(*it)) {
            ghosts.push_back(*it);
            it = g_icons.erase(it);
        } else {
            ++it;
        }
    }
    std::map<DWORD, std::wstring> exes;
    for (auto& g : ghosts)
        exes[g.pid] = g_owners.count(g.pid) ? g_owners[g.pid].exe : L"?";
    // Forget owners that have no icons left and whose process is gone.
    for (auto it = g_owners.begin(); it != g_owners.end();) {
        bool hasIcons = false;
        for (auto& icon : g_icons)
            if (icon.pid == it->first)
                hasIcons = true;
        bool exited = it->second.process &&
                      WaitForSingleObject(it->second.process, 0) == WAIT_OBJECT_0;
        if (!hasIcons && (exited || !it->second.process)) {
            finished.push_back(*it);
            it = g_owners.erase(it);
        } else {
            ++it;
        }
    }
    ReleaseSRWLockExclusive(&g_lock);

    for (auto& f : finished) {
        if (f.second.wait)
            UnregisterWaitEx(f.second.wait, INVALID_HANDLE_VALUE);
        if (f.second.process)
            CloseHandle(f.second.process);
    }

    for (const TrackedIcon& g : ghosts) {
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
            Report(L"ghost | " + exes[g.pid] + L" | pid " + std::to_wstring(g.pid) +
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
    // The taskbar may not exist yet if Explorer is just starting.
    bool explorerWasRunning = true;
    for (;;) {
        g_tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (g_tray)
            break;
        explorerWasRunning = false;
        if (WaitForSingleObject(g_stop, 500) == WAIT_OBJECT_0)
            return 0;
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(g_tray, TraySubclass, 0)) {
        Wh_Log(L"Failed to subclass the taskbar");
        g_tray = nullptr;
        return 0;
    }

    AcquireSRWLockShared(&g_lock);
    bool refresh = g_refreshOnLoad;
    ReleaseSRWLockShared(&g_lock);
    if (explorerWasRunning && refresh)
        BroadcastTaskbarCreated();

    for (;;) {
        HANDLE handles[] = {g_stop, g_wake};
        DWORD r = WaitForMultipleObjects(2, handles, FALSE, 30000);
        if (r == WAIT_OBJECT_0)
            break;

        StartWaits();

        if (r == WAIT_TIMEOUT) {
            // The taskbar window can be re-created without restarting Explorer;
            // follow it, or the mod would silently stop seeing new icons.
            HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
            if (tray && tray != g_tray &&
                WindhawkUtils::SetWindowSubclassFromAnyThread(tray, TraySubclass, 0)) {
                if (IsWindow(g_tray))
                    WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_tray, TraySubclass);
                g_tray = tray;
                Wh_Log(L"Taskbar window re-created, following it");
                BroadcastTaskbarCreated();
            }
            Clean({}, true, L"periodic check");
            continue;
        }

        AcquireSRWLockShared(&g_lock);
        bool anyExited = !g_exited.empty();
        ReleaseSRWLockShared(&g_lock);
        if (!anyExited)
            continue;

        // Give a well-behaved program time to remove its own icon.
        if (WaitForSingleObject(g_stop, 1500) == WAIT_OBJECT_0)
            break;
        AcquireSRWLockExclusive(&g_lock);
        std::vector<DWORD> exited;
        exited.swap(g_exited);
        ReleaseSRWLockExclusive(&g_lock);
        Clean(exited, false, L"owner exited");
    }
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_stop && g_wake)
        g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_thread) {
        if (g_wake)
            CloseHandle(g_wake);
        if (g_stop)
            CloseHandle(g_stop);
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    // Stop the worker first (it is the one that installs the subclass), then remove
    // the subclass. Every wait in the worker is interruptible, so this is quick.
    SetEvent(g_stop);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    if (g_tray)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_tray, TraySubclass);

    // Cancel the process waits and wait for any running callback to finish.
    for (auto& o : g_owners) {
        if (o.second.wait)
            UnregisterWaitEx(o.second.wait, INVALID_HANDLE_VALUE);
        if (o.second.process)
            CloseHandle(o.second.process);
    }
    g_owners.clear();
    CloseHandle(g_wake);
    CloseHandle(g_stop);
}
