// ==WindhawkMod==
// @id              rdp-icon-refresh
// @name            Sharp Taskbar Icons After Remote Desktop
// @description     Re-renders tray and taskbar button icons when you return to the local screen after a Remote Desktop session with a different scale, so they don't stay blurry
// @version         1.0
// @author          Solo_mag
// @github          https://github.com/Solomag
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -lwtsapi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Sharp Taskbar Icons After Remote Desktop

Connecting with Remote Desktop from a device with a different display scale (for example
the Microsoft Remote Desktop app on a phone, at 200–300%) switches the session to that
scale, and Explorer renders the tray icons for it. When you come back to the local screen,
Explorer doesn't render them again from the programs' icons — it rescales the copies it
already made, so they stay blurry until the program or Explorer is restarted.

This mod asks Explorer to render the icons again. For the tray it uses the copy of each
icon that Explorer itself keeps for the tray button; as a fallback (for icons it can't
find there), it remembers the icon each program gives the tray, by watching the
`WM_COPYDATA` messages that `Shell_NotifyIcon` sends to the taskbar and Explorer's
answers to them. When the session returns to the local console, it waits until the
scale has stayed the same for a few seconds (Explorer may pass through an intermediate
scale, and turns tray requests down while it rebuilds the taskbar) and then:

* asks Explorer to render every tray icon again (`NIM_MODIFY` with the icon Explorer
  itself keeps for that tray button, or the program's own icon handle for icons that
  aren't in the tray toolbars — what happens when the program is restarted, but
  without restarting anything). Explorer's copy is used because many programs destroy
  the handle they give the tray right after giving it;
* asks the taskbar to re-read the icon of every top-level window (`HSHELL_REDRAW`
  through the taskbar's shell hook message). Buttons of pinned programs that aren't
  running are not refreshed.

If Explorer turns most tray icons down because it is still busy, or the scale changes
again, the tray is refreshed again (for about a minute after returning).

While the remote session is active, the mod checks the taskbar's scale every few
seconds. If it never differed from the local screen's scale, nothing is done. The
Windhawk log shows the scales and how many icons were refreshed.

Icons that existed before the mod was enabled are handled too: their copies are read
from the tray at refresh time. The exception is an icon a program registered by GUID
before the mod was loaded: Explorer only accepts it by its GUID, which the mod learns
when the program registers or updates the icon. Nothing is sent to other programs.

Made for the Windows 10 taskbar (tested on Windows 10 22H2), and for Windows 11 versions
whose taskbar still behaves like it; not tested on the newer Windows 11 taskbar. The
newer Windows 11 (XAML) notification area has no tray toolbars, so there only icons
registered while the mod was running are refreshed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- delaySeconds: 3
  $name: Delay after returning to the local screen (seconds)
  $description: >-
    How long the display scale must stay the same before the icons are refreshed. The
    scale is watched for a minute: if it changes again, or Explorer is still busy and
    turns the icons down, they are refreshed again.
- logToFile: false
  $name: Log refreshes to a file
  $description: >-
    Also append the refresh reports to refresh.log in the mod's storage folder (its full
    path is written to the Windhawk log). The reports always go to the Windhawk log.
- refreshOnLoad: false
  $name: Refresh once when the mod is loaded
  $description: Useful to test the refresh, or to fix icons right after enabling the mod.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <wtsapi32.h>
#include <windhawk_utils.h>

#include <atomic>
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
// structure, so it is only used as an extra upper bound next to cbData.
constexpr DWORD kGuidEnd = offsetof(NotifyIconData32, guidItem) + sizeof(GUID);

struct TrayIcon {
    HWND hwnd;
    UINT id;
    bool byGuid;
    GUID guid;
    HICON icon;  // the last icon the program gave the tray, or null
};

constexpr UINT_PTR kRefreshTimer = 1;    // back to the local screen
constexpr UINT_PTR kRemoteDpiTimer = 2;  // sample the scale of the remote session
constexpr UINT_PTR kLoadTimer = 3;       // "refresh once on load" setting
constexpr UINT_PTR kWtsTimer = 4;        // retry the session notification registration

constexpr UINT kRemoteDpiPeriodMs = 5000;

SRWLOCK g_lock = SRWLOCK_INIT;  // guards g_icons and g_logFile
std::vector<TrayIcon> g_icons;
WCHAR g_logFile[MAX_PATH];

std::atomic<int> g_delaySeconds{3};
std::atomic<bool> g_refreshOnLoad{false};

HANDLE g_thread = nullptr;
HANDLE g_ready = nullptr;
HWND g_window = nullptr;           // our message window
std::atomic<HWND> g_tray{nullptr};  // taskbar of this process (subclassed)
std::atomic<bool> g_subclassed{false};  // the subclass of g_tray is installed
std::atomic<HICON> g_sendingIcon{nullptr};  // icon of the mod's own NIM_MODIFY in flight

// Scales seen during the remote session, and the scale when it started (window thread
// only).
UINT g_remoteDpiMin = 0;
UINT g_remoteDpiMax = 0;
UINT g_localDpi = 0;
// The session was remote (or disconnected) since the last return to the console; a
// console connect without that is a local user switch, with nothing to refresh.
bool g_awayFromConsole = false;

HWND FromHandle32(DWORD h) {
    return (HWND)(LONG_PTR)(LONG)h;  // sign-extended, as WOW64 does
}

// This DLL, so that the window class belongs to the mod and goes away with it.
HINSTANCE ThisModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&ThisModule, &module);
    return module;
}

void LoadSettings() {
    int delay = Wh_GetIntSetting(L"delaySeconds");
    g_delaySeconds = delay < 0 ? 0 : (delay > 60 ? 60 : delay);
    g_refreshOnLoad = Wh_GetIntSetting(L"refreshOnLoad") != 0;
    // The log file lives in the mod's storage folder, which is removed with the mod.
    WCHAR path[MAX_PATH] = L"";
    WCHAR dir[MAX_PATH - 16];
    if (Wh_GetIntSetting(L"logToFile") && Wh_GetModStoragePath(dir, ARRAYSIZE(dir))) {
        SHCreateDirectoryExW(nullptr, dir, nullptr);  // may not exist yet
        wsprintfW(path, L"%s\\refresh.log", dir);
        Wh_Log(L"Logging refreshes to %s", path);
    }
    AcquireSRWLockExclusive(&g_lock);
    lstrcpynW(g_logFile, path, MAX_PATH);
    ReleaseSRWLockExclusive(&g_lock);
}

void Report(const WCHAR* line) {
    Wh_Log(L"%s", line);
    WCHAR path[MAX_PATH];
    AcquireSRWLockShared(&g_lock);
    lstrcpynW(path, g_logFile, MAX_PATH);
    ReleaseSRWLockShared(&g_lock);
    if (!path[0])
        return;
    SYSTEMTIME t;
    GetLocalTime(&t);
    WCHAR full[512];
    wsprintfW(full, L"%02d.%02d.%04d %02d:%02d:%02d | %s\r\n", t.wDay, t.wMonth,
              t.wYear, t.wHour, t.wMinute, t.wSecond, line);
    char utf8[1536];
    int n = WideCharToMultiByte(CP_UTF8, 0, full, -1, utf8, sizeof(utf8), nullptr, nullptr);
    HANDLE f = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_ALWAYS, 0, nullptr);
    if (f != INVALID_HANDLE_VALUE) {
        DWORD written;
        if (n > 1)
            WriteFile(f, utf8, n - 1, &written, nullptr);
        CloseHandle(f);
    }
}

// Reading Explorer's copies of the tray icons (below): the mod's thread sends
// g_collectMsg to the taskbar, so that the toolbars are read on the thread that owns
// them (their button data can't change or be freed while it is read).
struct ExplorerIcons;
void CollectAllExplorerIcons(HWND tray, ExplorerIcons& out);
UINT g_collectMsg = 0;
// The request the mod's thread is waiting for. Held while the icons are collected, so
// that a request that timed out can't be written to afterwards.
SRWLOCK g_collectLock = SRWLOCK_INIT;
ExplorerIcons* g_collectTarget = nullptr;

// ---- Runs on Explorer's taskbar thread, after Explorer handled the request. ----
void OnNotifyIcon(const COPYDATASTRUCT* cds, bool accepted) {
    if (cds->cbData < offsetof(TrayNotifyData, nid) + offsetof(NotifyIconData32, szTip))
        return;
    const auto* data = (const TrayNotifyData*)cds->lpData;
    if (data->dwSignature != kTraySignature)
        return;
    const NotifyIconData32& nid = data->nid;
    // The mod's own refresh: Explorer's copy of the icon, not the program's.
    HICON own = g_sendingIcon;
    if (own && (nid.uFlags & NIF_ICON) && (HICON)FromHandle32(nid.hIcon) == own)
        return;
    DWORD size = cds->cbData - offsetof(TrayNotifyData, nid);
    if (nid.cbSize < size)
        size = nid.cbSize;

    bool byGuid = (nid.uFlags & NIF_GUID) && size >= kGuidEnd;
    // A rejected request with a GUID may come from another program (a second
    // instance, or a copy at another path): it says nothing about the icon.
    if (byGuid && !accepted)
        return;
    GUID guid = byGuid ? nid.guidItem : GUID{};
    HWND hwnd = FromHandle32(nid.hWnd);

    AcquireSRWLockExclusive(&g_lock);
    auto it = g_icons.begin();
    for (; it != g_icons.end(); ++it) {
        bool same = (byGuid || it->byGuid)
                        ? (byGuid && it->byGuid && IsEqualGUID(it->guid, guid))
                        : (it->hwnd == hwnd && it->id == nid.uID);
        if (same)
            break;
    }
    switch (data->dwMessage) {
        case NIM_ADD:
        case NIM_MODIFY:
            // A rejected NIM_ADD by window + ID usually means the icon already exists
            // (a program re-adding it after TaskbarCreated); a rejected NIM_MODIFY
            // means it doesn't.
            if (!accepted && data->dwMessage == NIM_MODIFY)
                break;
            if (it == g_icons.end()) {
                g_icons.push_back(TrayIcon{hwnd, nid.uID, byGuid, guid, nullptr});
                it = g_icons.end() - 1;
            }
            // A GUID icon can move to another window without changing its image.
            it->hwnd = hwnd;
            if (nid.uFlags & NIF_ICON)
                it->icon = (HICON)FromHandle32(nid.hIcon);
            break;
        case NIM_DELETE:
            if (it != g_icons.end())
                g_icons.erase(it);
            break;
    }
    ReleaseSRWLockExclusive(&g_lock);
}

LRESULT CALLBACK TraySubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                              DWORD_PTR) {
    if (g_collectMsg && uMsg == g_collectMsg) {
        // Only the request the mod's own thread is waiting for: anyone can send a
        // registered message, with any lParam.
        auto* out = (ExplorerIcons*)lParam;
        LRESULT collected = 0;
        AcquireSRWLockExclusive(&g_collectLock);
        if (out && out == g_collectTarget) {
            CollectAllExplorerIcons(hWnd, *out);
            collected = 1;
        }
        ReleaseSRWLockExclusive(&g_collectLock);
        return collected;
    }
    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    if (uMsg == WM_COPYDATA) {
        // The result is what Shell_NotifyIcon returns to the program.
        const auto* cds = (const COPYDATASTRUCT*)lParam;
        if (cds && cds->dwData == kCopyDataNotifyIcon && cds->lpData)
            OnNotifyIcon(cds, result != 0);
    }
    return result;
}

// Remembers the taskbar of this process and subclasses it to learn the tray icons.
bool AttachToTray(HWND tray) {
    g_tray = tray;  // the taskbar button refresh works even without the subclass
    bool subclassed = WindhawkUtils::SetWindowSubclassFromAnyThread(tray, TraySubclass, 0);
    g_subclassed = subclassed;
    if (!subclassed)
        Wh_Log(L"Failed to subclass the taskbar");
    return subclassed;
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

UINT TaskbarDpi() {
    HWND tray = g_tray;
    UINT dpi = tray ? GetDpiForWindow(tray) : 0;
    return dpi ? dpi : GetDpiForSystem();
}

bool IconAlive(HICON icon) {
    ICONINFO ii;
    if (!icon || !GetIconInfo(icon, &ii))
        return false;
    if (ii.hbmColor)
        DeleteObject(ii.hbmColor);
    if (ii.hbmMask)
        DeleteObject(ii.hbmMask);
    return true;
}

// The start of the data Explorer keeps for each tray button (TBBUTTON::dwData of the
// notification area toolbars). Explorer's own copy of the icon is in it.
struct TrayButtonData {
    HWND hwnd;
    UINT uID;
    UINT uCallbackMessage;
    DWORD reserved[2];
    HICON hIcon;
};

// Whether [p, p + size) is committed, readable memory of this process.
bool Readable(const void* p, size_t size) {
    const char* start = (const char*)p;
    const char* end = start + size;
    if (end < start)
        return false;
    while (start < end) {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(start, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT)
            return false;
        DWORD prot = mbi.Protect & 0xFF;
        if ((mbi.Protect & PAGE_GUARD) || prot == PAGE_NOACCESS || prot == PAGE_EXECUTE)
            return false;
        start = (const char*)mbi.BaseAddress + mbi.RegionSize;
    }
    return true;
}

struct ExplorerIcon {
    HWND hwnd;
    UINT id;
    HICON icon;
};

struct ExplorerIcons {
    std::vector<ExplorerIcon> icons;
    bool toolbarFound = false;  // the notification area toolbar exists
};

// Explorer's copies of the tray icons in one notification area toolbar of this process.
// Runs on the taskbar thread.
void CollectExplorerIcons(HWND toolbar, ExplorerIcons& out) {
    DWORD pid = 0;
    if (!toolbar || !GetWindowThreadProcessId(toolbar, &pid) || pid != GetCurrentProcessId())
        return;
    out.toolbarFound = true;
    int count = (int)SendMessageW(toolbar, TB_BUTTONCOUNT, 0, 0);
    for (int i = 0; i < count; i++) {
        TBBUTTON button = {};
        if (!SendMessageW(toolbar, TB_GETBUTTON, i, (LPARAM)&button) || !button.dwData)
            continue;
        // Checked, in case a Windows version lays this data out differently.
        if (!Readable((const void*)button.dwData, sizeof(TrayButtonData)))
            continue;
        const auto* data = (const TrayButtonData*)button.dwData;
        if (IsWindow(data->hwnd) && IconAlive(data->hIcon))
            out.icons.push_back(ExplorerIcon{data->hwnd, data->uID, data->hIcon});
    }
}

// The taskbar's toolbar and the overflow window's toolbar. Runs on the taskbar thread.
void CollectAllExplorerIcons(HWND tray, ExplorerIcons& out) {
    HWND notify = FindWindowExW(tray, nullptr, L"TrayNotifyWnd", nullptr);
    HWND pager = FindWindowExW(notify, nullptr, L"SysPager", nullptr);
    CollectExplorerIcons(FindWindowExW(pager, nullptr, L"ToolbarWindow32", nullptr), out);
    HWND overflow = FindWindowW(L"NotifyIconOverflowWindow", nullptr);
    CollectExplorerIcons(FindWindowExW(overflow, nullptr, L"ToolbarWindow32", nullptr), out);
}

bool ModifyIcon(HWND hwnd, UINT id, const GUID* guid, HICON icon) {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = id;
    nid.uFlags = NIF_ICON;
    nid.hIcon = icon;
    if (guid) {
        nid.uFlags |= NIF_GUID;
        nid.guidItem = *guid;
    }
    // Explorer handles the request before Shell_NotifyIconW returns, so the subclass
    // can tell it apart from the programs' own requests.
    g_sendingIcon = icon;
    bool ok = Shell_NotifyIconW(NIM_MODIFY, &nid);
    g_sendingIcon = nullptr;
    return ok;
}

// Re-renders every tray icon. Explorer's own copy of an icon is the reliable source:
// many programs destroy the handle they give the tray right after giving it. The
// program's handle is the fallback for icons that aren't in the toolbars.
int RerenderTrayIcons(int* total, bool* toolbarFound, bool* collected) {
    // Forget icons whose window is gone (crashed or restarted programs); for a GUID
    // icon, sending a dead window would also re-point the icon at it.
    AcquireSRWLockExclusive(&g_lock);
    for (auto it = g_icons.begin(); it != g_icons.end();)
        it = IsWindow(it->hwnd) ? it + 1 : g_icons.erase(it);
    std::vector<TrayIcon> icons = g_icons;
    ReleaseSRWLockExclusive(&g_lock);

    // Handled by the subclass on the taskbar thread. Waiting while the taskbar is busy is
    // fine (this thread has nothing else to do), but not if it stopped responding: the
    // mod couldn't be unloaded then.
    ExplorerIcons copies;
    HWND tray = g_tray;
    DWORD_PTR answer = 0;
    AcquireSRWLockExclusive(&g_collectLock);
    g_collectTarget = &copies;
    ReleaseSRWLockExclusive(&g_collectLock);
    // Without the subclass nobody answers, and waiting for an answer wouldn't help.
    *collected = !g_subclassed ||
                 (tray &&
                  SendMessageTimeoutW(tray, g_collectMsg, 0, (LPARAM)&copies,
                                      SMTO_ABORTIFHUNG | SMTO_NOTIMEOUTIFNOTHUNG, 5000,
                                      &answer) &&
                  answer);
    AcquireSRWLockExclusive(&g_collectLock);
    g_collectTarget = nullptr;
    ReleaseSRWLockExclusive(&g_collectLock);
    *toolbarFound = copies.toolbarFound;

    std::vector<bool> handled(icons.size(), false);
    int done = 0;
    *total = 0;
    for (const ExplorerIcon& c : copies.icons) {
        (*total)++;
        bool ok = ModifyIcon(c.hwnd, c.id, nullptr, c.icon);
        // An icon registered by GUID is found by its GUID only; use the one the mod
        // learned for this window, if there is exactly one.
        const GUID* guid = nullptr;
        size_t guidIndex = 0;
        int guidCount = 0;
        bool knownById = false;
        for (size_t i = 0; i < icons.size(); i++) {
            if (icons[i].hwnd != c.hwnd)
                continue;
            if (!icons[i].byGuid && icons[i].id == c.id) {
                handled[i] = true;
                knownById = true;
            }
            if (icons[i].byGuid) {
                guid = &icons[i].guid;
                guidIndex = i;
                guidCount++;
            }
        }
        if (!ok && guidCount == 1) {
            ok = ModifyIcon(c.hwnd, c.id, guid, c.icon);
            if (ok)
                handled[guidIndex] = true;
        }
        if (ok)
            done++;
        else if (guidCount > 1 && !knownById)
            (*total)--;  // one of this window's GUID icons: counted by the fallback below
    }

    for (size_t i = 0; i < icons.size(); i++) {
        const TrayIcon& t = icons[i];
        if (handled[i] || !IconAlive(t.icon))
            continue;  // done above, or the program already replaced or destroyed it
        (*total)++;
        if (ModifyIcon(t.hwnd, t.id, t.byGuid ? &t.guid : nullptr, t.icon))
            done++;
    }
    return done;
}

struct RedrawContext {
    HWND taskSwitch;
    UINT shellHookMsg;
    int count;
};

// Windows that can have a taskbar button: visible, not tool windows, unowned or app
// windows. Windows on other virtual desktops are included.
BOOL CALLBACK RedrawButton(HWND hwnd, LPARAM param) {
    auto* ctx = (RedrawContext*)param;
    if (!IsWindowVisible(hwnd))
        return TRUE;
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW)
        return TRUE;
    if (GetWindow(hwnd, GW_OWNER) && !(ex & WS_EX_APPWINDOW))
        return TRUE;
    PostMessageW(ctx->taskSwitch, ctx->shellHookMsg, HSHELL_REDRAW, (LPARAM)hwnd);
    ctx->count++;
    return TRUE;
}

// Returns false if Explorer turned down most tray icons (it is still busy re-creating
// the taskbar for the new scale), so that the caller tries again. The taskbar buttons
// are only redrawn when `buttons` is set (the first attempt for a scale).
bool Refresh(const wchar_t* reason, bool buttons) {
    // Only the explorer.exe that owns the taskbar refreshes; other explorer.exe
    // processes (separate folder windows) would just repeat the work.
    HWND tray = g_tray;
    if (!tray || !IsWindow(tray))
        return true;
    UINT dpi = TaskbarDpi();

    // 1. Tray: Explorer renders each icon again.
    int trayTotal = 0;
    bool toolbarFound = false;
    bool collected = false;
    int trayIcons = RerenderTrayIcons(&trayTotal, &toolbarFound, &collected);

    // 2. Taskbar buttons: the taskbar re-reads each window's icon, the same way it
    //    does when a program changes its icon.
    RedrawContext ctx{nullptr, RegisterWindowMessageW(L"SHELLHOOK"), 0};
    if (buttons) {
        HWND rebar = FindWindowExW(tray, nullptr, L"ReBarWindow32", nullptr);
        ctx.taskSwitch = FindWindowExW(rebar, nullptr, L"MSTaskSwWClass", nullptr);
        if (ctx.taskSwitch && ctx.shellHookMsg)
            EnumWindows(RedrawButton, (LPARAM)&ctx);
    }

    WCHAR line[256];
    wsprintfW(line, L"refreshed (%s): taskbar DPI %u, remote session DPI %u-%u, tray icons %d of %d, windows %d%s",
              reason, dpi, g_remoteDpiMin, g_remoteDpiMax, trayIcons, trayTotal, ctx.count,
              collected ? L"" : L", the taskbar didn't answer");
    Report(line);
    if (!collected)
        return false;
    // An empty notification area toolbar means it is being re-created. Without the
    // toolbar (a taskbar that keeps the tray elsewhere), retrying wouldn't help.
    if (trayTotal == 0)
        return !toolbarFound;
    return trayIcons * 2 >= trayTotal;
}

// ---- After returning to the local screen (window thread only). ----
// Explorer re-creates the taskbar for the local scale, sometimes through an
// intermediate scale, and turns down tray requests while it does. So the scale is
// watched for a while: the icons are refreshed once it has been the same for
// delaySeconds, again if it changes later, and again if Explorer turned them down.
constexpr UINT kSettlePeriodMs = 1000;
constexpr DWORD kSettleWatchMs = 60000;
constexpr int kMaxRetries = 5;

UINT g_settleDpi = 0;      // the scale seen on the last tick
int g_settleTicks = 0;     // ticks it has stayed the same
UINT g_refreshedDpi = 0;   // the scale the icons were last refreshed for
int g_retries = 0;
DWORD g_settleStart = 0;

void StartSettling(HWND hwnd) {
    g_settleDpi = 0;
    g_settleTicks = 0;
    g_refreshedDpi = 0;
    g_retries = 0;
    g_settleStart = GetTickCount();
    SetTimer(hwnd, kRefreshTimer, kSettlePeriodMs, nullptr);
}

void OnSettleTick(HWND hwnd) {
    UINT dpi = TaskbarDpi();
    if (dpi != g_settleDpi) {
        g_settleDpi = dpi;
        g_settleTicks = 0;
        g_retries = 0;  // every scale gets its own attempts
    } else {
        g_settleTicks++;
    }
    DWORD elapsed = GetTickCount() - g_settleStart;
    bool watchOver = elapsed >= kSettleWatchMs;
    int wait = g_delaySeconds < 1 ? 1 : (int)g_delaySeconds;
    // Right after returning, the taskbar may stay at a scale of the remote session for a
    // few seconds. Such a scale is only taken as final if it is the local scale seen
    // when the session started, or when the watch ends without a change.
    bool remoteScale = g_remoteDpiMax && dpi >= g_remoteDpiMin && dpi <= g_remoteDpiMax;
    bool lingering = !watchOver && remoteScale && dpi != g_localDpi;
    if (dpi != g_refreshedDpi && !lingering && g_settleTicks >= wait) {
        if (!g_refreshedDpi && g_remoteDpiMin == dpi && g_remoteDpiMax == dpi) {
            WCHAR line[128];
            wsprintfW(line, L"back to the local screen: same scale as the remote session (DPI %u), nothing to do", dpi);
            Report(line);
            g_refreshedDpi = dpi;
        } else if (Refresh(L"back to the local screen", g_retries == 0) ||
                   ++g_retries > kMaxRetries) {
            g_refreshedDpi = dpi;
        } else {
            g_settleTicks = 0;  // Explorer is still busy: try again later
        }
    }
    if (watchOver && (g_refreshedDpi == dpi || elapsed >= 2 * kSettleWatchMs)) {
        KillTimer(hwnd, kRefreshTimer);
        g_remoteDpiMin = g_remoteDpiMax = 0;  // this remote session is handled
    }
}

bool RegisterSessionNotification(HWND hwnd) {
    return WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_WTSSESSION_CHANGE:
            // Only the explorer.exe that owns the taskbar (not separate folder-window
            // processes) follows the session.
            if (!g_tray)
                return 0;
            if (wParam == WTS_REMOTE_CONNECT) {
                // Sampled while the session lasts: the remote scale may be applied
                // only some time after connecting. When the remote session follows a
                // local one, the scale now is still the local one.
                if (!g_remoteDpiMax)
                    g_localDpi = TaskbarDpi();
                g_awayFromConsole = true;
                g_remoteDpiMin = g_remoteDpiMax = 0;
                KillTimer(hwnd, kRefreshTimer);
                SetTimer(hwnd, kRemoteDpiTimer, kRemoteDpiPeriodMs, nullptr);
            } else if (wParam == WTS_REMOTE_DISCONNECT) {
                KillTimer(hwnd, kRemoteDpiTimer);
            } else if (wParam == WTS_CONSOLE_CONNECT) {
                KillTimer(hwnd, kRemoteDpiTimer);
                if (g_awayFromConsole) {
                    g_awayFromConsole = false;
                    StartSettling(hwnd);
                }
            }
            return 0;
        case WM_TIMER:
            if (wParam == kRemoteDpiTimer) {
                UINT dpi = TaskbarDpi();
                if (!g_remoteDpiMax || dpi < g_remoteDpiMin || dpi > g_remoteDpiMax) {
                    if (!g_remoteDpiMax || dpi < g_remoteDpiMin)
                        g_remoteDpiMin = dpi;
                    if (dpi > g_remoteDpiMax)
                        g_remoteDpiMax = dpi;
                    WCHAR line[64];
                    wsprintfW(line, L"remote session: taskbar DPI %u", dpi);
                    Report(line);
                }
                return 0;
            }
            if (wParam == kWtsTimer) {
                if (RegisterSessionNotification(hwnd)) {
                    KillTimer(hwnd, kWtsTimer);
                    Wh_Log(L"Registered for session notifications");
                }
                return 0;
            }
            if (wParam == kRefreshTimer) {
                OnSettleTick(hwnd);
                return 0;
            }
            if (wParam == kLoadTimer) {
                // Explorer may still be setting up the tray when the mod loads with it.
                static int loadRetries = 0;
                if (Refresh(L"mod loaded", loadRetries == 0) || ++loadRetries > kMaxRetries)
                    KillTimer(hwnd, kLoadTimer);
                return 0;
            }
            KillTimer(hwnd, wParam);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            WTSUnRegisterSessionNotification(hwnd);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI Worker(LPVOID) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = ThisModule();
    wc.lpszClassName = L"Windhawk_" WH_MOD_ID;
    ATOM atom = RegisterClassW(&wc);
    if (atom)
        g_window = CreateWindowExW(0, wc.lpszClassName, nullptr, 0, 0, 0, 0, 0,
                                   HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!g_window)
        Wh_Log(L"Failed to create the notification window: %u", GetLastError());
    SetEvent(g_ready);  // Wh_ModUninit only needs g_window

    if (g_window) {
        // Loaded during a remote session, or while it is disconnected.
        WTS_CONNECTSTATE_CLASS* state = nullptr;
        DWORD bytes = 0;
        if (GetSystemMetrics(SM_REMOTESESSION))
            g_awayFromConsole = true;
        else if (WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION,
                                             WTSConnectState, (LPWSTR*)&state, &bytes)) {
            g_awayFromConsole = bytes >= sizeof(*state) && *state != WTSActive;
            WTSFreeMemory(state);
        }
        // Early in a session the service may not be ready yet: keep retrying.
        if (!RegisterSessionNotification(g_window)) {
            Wh_Log(L"WTSRegisterSessionNotification failed: %u, retrying", GetLastError());
            SetTimer(g_window, kWtsTimer, 2000, nullptr);
        }
        if (g_refreshOnLoad)
            SetTimer(g_window, kLoadTimer, 2000, nullptr);
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    if (atom)
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    g_collectMsg = RegisterWindowMessageW(L"Windhawk_" WH_MOD_ID L"_collect");
    if (!g_collectMsg)
        return FALSE;
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original))
        return FALSE;
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_ready)
        return FALSE;
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_ready);
        return FALSE;
    }
    // The window must exist (or have failed) before Wh_ModUninit can be called.
    WaitForSingleObject(g_ready, INFINITE);
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
    // Icons that already exist are taken from Explorer's own copies at refresh time.
    if (g_tray != tray)
        AttachToTray(tray);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    // Remove the subclass first (it runs on the taskbar thread, so no tray message is
    // being handled once it returns), then stop the window thread.
    HWND tray = g_tray;
    if (tray)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(tray, TraySubclass);
    if (g_window)
        PostMessageW(g_window, WM_CLOSE, 0, 0);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_ready);
}
