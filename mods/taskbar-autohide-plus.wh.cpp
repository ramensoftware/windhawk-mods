// ==WindhawkMod==
// @id              taskbar-autohide-plus
// @name            Taskbar Auto-Hide Plus
// @description     One place for conditional taskbar auto-hide: modes, hide delay, hotkey. Runs as a tool mod.
// @version         1.0.0
// @author          Areeexx
// @github          https://github.com/Areeexx
// @include         windhawk.exe
// @compilerOptions -lshell32 -luser32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==
 
// ==WindhawkModReadme==
/*
# Taskbar Auto-Hide Plus
 
Controls the Windows **"Automatically hide the taskbar"** switch (Settings →
Personalization → Taskbar → Taskbar behaviors) according to simple rules, so
you don't have to toggle it by hand.
 
## How it works
 
* The mod is a **tool mod**: it runs in its own dedicated process, not inside
  `explorer.exe`, so it can't affect shell stability and there is exactly one
  copy no matter how many Explorer processes exist.
* It uses the regular Windows auto-hide switch (`SHAppBarMessage` with
  `ABM_SETSTATE`). It does **not** hook Explorer.
* The mod remembers the state your setting had when it started. Whenever the
  rule says "hide", auto-hide is turned on; otherwise your original state is
  restored. If the rule never requires a change, nothing is written.
* The original state is saved (separately for each Windows user) before the
  first change. If the mod exits uncleanly (for example Windows is shut down
  while the mod is active), the leftover is detected the next time the mod
  starts for that user and the original state is brought back.
* If you change the Windows setting yourself while the mod is running, the mod
  treats your choice as the new "original" state.
* When the mod is disabled or unloaded, your original state is always restored.
* If the mod is removed, or Windhawk is uninstalled or stopped, right after an
  unclean exit (a crash, or Windows shutting down while the mod was active),
  nobody is left to undo the change and auto-hide may stay on. In that case
  turn **Automatically hide the taskbar** off by hand in Settings →
  Personalization → Taskbar → Taskbar behaviors.
 
## Modes
 
* **Always** - the taskbar is always auto-hidden.
* **Only when the active window is maximized** - auto-hide is on while the
  active window is maximized.
* **Only by hotkey** - nothing happens until you press the hotkey.
* **Off** - the mod does not hide the taskbar (the hotkey still works).
 
If auto-hide is already enabled in Windows when the mod starts, the taskbar is
simply always auto-hidden: the mod only adds auto-hide, it never forces it off.
 
## Options
 
* **Hide delay** - how long the rule must hold before auto-hide is turned on.
* **Don't hide while the cursor is over the taskbar** - the taskbar never
  disappears from under the mouse.
* **Toggle hotkey** - registered with `RegisterHotKey`; at least one modifier
  is required. The hotkey inverts the current rule. Note that on keyboard
  layouts with AltGr, AltGr is reported as Ctrl+Alt, so Ctrl+Alt+<key> may also
  be triggered by typing that AltGr character; add Shift or Win to avoid this.
 
## How this differs from existing mods
 
Several existing mods already cover parts of this, for example
*Taskbar auto-hide when maximized*, *Taskbar auto-hide fine tuning*,
*Taskbar auto-hide after a delay* and *Better Taskbar Autohide*. Those mods
work through Explorer hooks or their own logic. This mod is deliberately
simpler: it only drives the standard Windows setting and combines the mode,
the hide delay, the cursor check and the hotkey in a single mod with a single
settings page. If you only need one of those features, one of the existing mods
may be a better fit.
 
The Windows auto-hide setting is shared by all monitors.
*/
// ==/WindhawkModReadme==
 
// ==WindhawkModSettings==
/*
- mode: always
  $name: Mode
  $description: When the taskbar should be auto-hidden
  $options:
  - always: Always
  - maximized: Only when the active window is maximized
  - hotkey: Only by hotkey
  - off: Off
- hideDelay: 0
  $name: Hide delay (ms)
  $description: How long the condition must hold before hiding (0 = immediately)
- waitForCursorLeave: true
  $name: Don't hide while the cursor is over the taskbar
- hotkeyEnabled: true
  $name: Enable toggle hotkey
- hotkeyCtrl: true
  $name: Hotkey - Ctrl
- hotkeyAlt: true
  $name: Hotkey - Alt
- hotkeyShift: false
  $name: Hotkey - Shift
- hotkeyWin: false
  $name: Hotkey - Win
- hotkeyKey: T
  $name: Hotkey - key
  $description: A single letter or digit. At least one modifier must be selected.
*/
// ==/WindhawkModSettings==
 
#include <windows.h>
#include <shellapi.h>
#include <sddl.h>
#include <wchar.h>
 
enum Mode { MODE_ALWAYS, MODE_MAXIMIZED, MODE_HOTKEY, MODE_OFF };
 
struct Settings {
    Mode mode = MODE_ALWAYS;
    int  hideDelay = 0;
    bool waitForCursorLeave = true;
    bool hotkeyEnabled = true;
    bool ctrl = true, alt = true, shift = false, win = false;
    UINT vk = 'T';
};
 
#define WM_APP_SETTINGS (WM_APP + 1)
#define HOTKEY_ID       1
 
// All of the state below is owned by the worker thread, except the handles
// that WhTool_ModInit / WhTool_ModUninit use to start and stop it.
static Settings        g_settings;
static HANDLE          g_thread = nullptr;
static HANDLE          g_readyEvent = nullptr;
static DWORD           g_threadId = 0;
static HWINEVENTHOOK   g_hookForeground = nullptr;
static HWINEVENTHOOK   g_hookLocation = nullptr;
static UINT_PTR        g_timer = 0;
static bool            g_hotkeyRegistered = false;
static bool            g_manualToggle = false;  // flipped by the hotkey
static bool            g_ready = false;         // taskbar found, original state captured
static bool            g_orig = false;          // user's auto-hide state
static int             g_lastApplied = -1;      // state last written by the mod, -1 = none
static ULONGLONG       g_pendingSince = 0;      // when a pending "hide" started
static int             g_lastMax = -1;          // last observed "active window maximized"
static WCHAR           g_dirtyName[256] = L"dirty";         // per-user value names
static WCHAR           g_origName[256]  = L"origAutoHide";
 
// ---------------------------------------------------------------- settings
 
static void LoadSettings() {
    Settings s;
 
    PCWSTR m = Wh_GetStringSetting(L"mode");
    if (!wcscmp(m, L"maximized"))    s.mode = MODE_MAXIMIZED;
    else if (!wcscmp(m, L"hotkey"))  s.mode = MODE_HOTKEY;
    else if (!wcscmp(m, L"off"))     s.mode = MODE_OFF;
    else                             s.mode = MODE_ALWAYS;
    Wh_FreeStringSetting(m);
 
    s.hideDelay          = Wh_GetIntSetting(L"hideDelay");
    s.waitForCursorLeave = Wh_GetIntSetting(L"waitForCursorLeave") != 0;
    s.hotkeyEnabled      = Wh_GetIntSetting(L"hotkeyEnabled") != 0;
    s.ctrl               = Wh_GetIntSetting(L"hotkeyCtrl") != 0;
    s.alt                = Wh_GetIntSetting(L"hotkeyAlt") != 0;
    s.shift              = Wh_GetIntSetting(L"hotkeyShift") != 0;
    s.win                = Wh_GetIntSetting(L"hotkeyWin") != 0;
 
    if (s.hideDelay < 0)     s.hideDelay = 0;
    if (s.hideDelay > 60000) s.hideDelay = 60000;
 
    PCWSTR k = Wh_GetStringSetting(L"hotkeyKey");
    WCHAR c = k[0] ? k[0] : L'T';
    if (c >= L'a' && c <= L'z') c -= 32;
    s.vk = c;  // for A-Z and 0-9 the virtual-key code equals the character
    Wh_FreeStringSetting(k);
 
    g_settings = s;
}
 
// ---------------------------------------------------------- taskbar control
 
static HWND GetTaskbar() {
    return FindWindowW(L"Shell_TrayWnd", nullptr);
}
 
static bool GetAutoHide() {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd   = GetTaskbar();
    UINT st = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    return (st & ABS_AUTOHIDE) != 0;
}
 
static void SetAutoHide(bool enable) {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd   = GetTaskbar();
    UINT st = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    st = enable ? (st | ABS_AUTOHIDE) : (st & ~(UINT)ABS_AUTOHIDE);
    abd.lParam = (LPARAM)st;
    SHAppBarMessage(ABM_SETSTATE, &abd);
}
 
static bool CursorOverTaskbar() {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd   = GetTaskbar();
    if (!SHAppBarMessage(ABM_GETTASKBARPOS, &abd)) return false;
    RECT r = abd.rc;
    InflateRect(&r, 4, 4);
    POINT p;
    GetCursorPos(&p);
    return PtInRect(&r, p) != FALSE;
}
 
static bool ActiveWindowMaximized() {
    HWND fg = GetForegroundWindow();
    if (!fg || !IsWindowVisible(fg) || IsIconic(fg)) return false;
 
    WCHAR cls[64] = L"";
    GetClassNameW(fg, cls, ARRAYSIZE(cls));
    if (!wcscmp(cls, L"Shell_TrayWnd") || !wcscmp(cls, L"Shell_SecondaryTrayWnd") ||
        !wcscmp(cls, L"Progman")       || !wcscmp(cls, L"WorkerW"))
        return false;
 
    return IsZoomed(fg) != FALSE;
}
 
// -------------------------------------------------------------- state logic
 
static void ArmTimer(UINT ms) {
    if (g_timer) KillTimer(nullptr, g_timer);
    if (ms < USER_TIMER_MINIMUM) ms = USER_TIMER_MINIMUM;
    g_timer = SetTimer(nullptr, 0, ms, nullptr);
}
 
static void DisarmTimer() {
    if (g_timer) {
        KillTimer(nullptr, g_timer);
        g_timer = 0;
    }
}
 
// Mod storage is machine-wide, while the taskbar setting is per user, so the
// value names carry the SID of the current user.
static void InitValueNames() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return;
 
    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    if (size) {
        BYTE* buf = (BYTE*)LocalAlloc(LMEM_FIXED, size);
        if (buf) {
            if (GetTokenInformation(token, TokenUser, buf, size, &size)) {
                PWSTR sid = nullptr;
                if (ConvertSidToStringSidW(((TOKEN_USER*)buf)->User.Sid, &sid)) {
                    if (wcslen(sid) < 200) {
                        swprintf_s(g_dirtyName, L"dirty_%s", sid);
                        swprintf_s(g_origName, L"origAutoHide_%s", sid);
                    }
                    LocalFree(sid);
                }
            }
            LocalFree(buf);
        }
    }
    CloseHandle(token);
}
 
// Waits until the taskbar exists, undoes a leftover from an unclean exit and
// captures the user's original state. Returns false while the taskbar is
// not available yet.
static bool EnsureReady() {
    if (g_ready) return true;
    if (!GetTaskbar()) return false;
 
    if (Wh_GetIntValue(g_dirtyName, 0)) {
        // The previous run ended without restoring the setting. Adopt the
        // leftover as the state the mod itself applied and remember the real
        // original; the next Update either keeps it (if the rules still want
        // auto-hide) or restores the original with a single write.
        bool saved = Wh_GetIntValue(g_origName, 0) != 0;
        bool cur = GetAutoHide();
        Wh_Log(L"Leftover from previous run: original=%d current=%d",
               (int)saved, (int)cur);
        g_orig = saved;
        if (cur != saved) {
            g_lastApplied = cur ? 1 : 0;
        } else {
            Wh_SetIntValue(g_dirtyName, 0);
        }
        g_ready = true;
        return true;
    }
 
    g_orig = GetAutoHide();
    g_ready = true;
    Wh_Log(L"Original auto-hide state: %d", (int)g_orig);
    return true;
}
 
// Whether the rules currently ask for auto-hide.
static bool DesiredHide() {
    bool base = false;
    switch (g_settings.mode) {
        case MODE_ALWAYS:
            base = true;
            break;
        case MODE_MAXIMIZED: {
            bool m = ActiveWindowMaximized();
            g_lastMax = m ? 1 : 0;
            base = m;
            break;
        }
        case MODE_HOTKEY:
        case MODE_OFF:
            base = false;
            break;
    }
    return g_manualToggle ? !base : base;
}
 
static void ApplyAutoHide(bool value) {
    // Persist the original state before the first change away from it.
    if (value != g_orig) {
        Wh_SetIntValue(g_origName, g_orig ? 1 : 0);
        Wh_SetIntValue(g_dirtyName, 1);
    }
    SetAutoHide(value);
    g_lastApplied = value ? 1 : 0;
    if (value == g_orig) {
        Wh_SetIntValue(g_dirtyName, 0);
    }
}
 
static void Update() {
    DisarmTimer();
 
    if (!EnsureReady()) {
        ArmTimer(1000);  // taskbar not created yet, try again later
        return;
    }
 
    bool cur = GetAutoHide();
 
    // The setting differs from what the mod last wrote: the user changed it,
    // so take their choice as the new original state.
    if (g_lastApplied != -1 && cur != (g_lastApplied != 0)) {
        Wh_Log(L"Setting changed externally -> %d", (int)cur);
        g_orig = cur;
        g_lastApplied = -1;
        Wh_SetIntValue(g_dirtyName, 0);
    }
 
    bool want   = DesiredHide();
    bool target = want ? true : g_orig;
 
    if (target == cur) {
        g_pendingSince = 0;
        return;
    }
 
    if (target) {
        // Turning auto-hide on: honor the delay and the cursor check.
        ULONGLONG now = GetTickCount64();
        if (!g_pendingSince) g_pendingSince = now;
        ULONGLONG elapsed = now - g_pendingSince;
        if (elapsed < (ULONGLONG)g_settings.hideDelay) {
            ArmTimer((UINT)((ULONGLONG)g_settings.hideDelay - elapsed));
            return;
        }
        if (g_settings.waitForCursorLeave && CursorOverTaskbar()) {
            ArmTimer(100);
            return;
        }
    }
 
    g_pendingSince = 0;
    ApplyAutoHide(target);
}
 
// On unload: put the user's setting back, unless the user changed it meanwhile.
static void RestoreOriginal() {
    if (!g_ready) return;
    if (g_lastApplied != -1) {
        bool cur = GetAutoHide();
        if (cur == (g_lastApplied != 0) && cur != g_orig) {
            SetAutoHide(g_orig);
        }
        g_lastApplied = -1;
    }
    Wh_SetIntValue(g_dirtyName, 0);
}
 
// ----------------------------------------------------------------- hotkey
 
static void RegisterHotkeyFromSettings() {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, HOTKEY_ID);
        g_hotkeyRegistered = false;
    }
 
    const Settings& s = g_settings;
    if (!s.hotkeyEnabled) return;
 
    UINT mods = MOD_NOREPEAT;
    if (s.ctrl)  mods |= MOD_CONTROL;
    if (s.alt)   mods |= MOD_ALT;
    if (s.shift) mods |= MOD_SHIFT;
    if (s.win)   mods |= MOD_WIN;
 
    if (!(s.ctrl || s.alt || s.shift || s.win)) {
        Wh_Log(L"Hotkey needs at least one modifier");
        return;
    }
 
    if (RegisterHotKey(nullptr, HOTKEY_ID, mods, s.vk)) {
        g_hotkeyRegistered = true;
    } else {
        Wh_Log(L"RegisterHotKey failed (%u)", GetLastError());
    }
}
 
// ------------------------------------------------------- window state events
 
static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                  LONG idObject, LONG idChild, DWORD, DWORD) {
    if (g_settings.mode != MODE_MAXIMIZED) return;
 
    if (event == EVENT_OBJECT_LOCATIONCHANGE &&
        (idObject != OBJID_WINDOW || idChild != CHILDID_SELF ||
         hwnd != GetForegroundWindow())) {
        return;
    }
 
    int m = ActiveWindowMaximized() ? 1 : 0;
    if (m == g_lastMax) return;
    Update();
}
 
// ------------------------------------------------------------ worker thread
 
static DWORD WINAPI WorkerThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // create the queue
 
    InitValueNames();
    LoadSettings();
    RegisterHotkeyFromSettings();
 
    g_hookForeground = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_hookLocation = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
 
    SetEvent(g_readyEvent);
    Update();
 
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        switch (msg.message) {
            case WM_HOTKEY:
                if (msg.wParam == HOTKEY_ID) {
                    g_manualToggle = !g_manualToggle;
                    Wh_Log(L"Hotkey toggle -> %d", (int)g_manualToggle);
                    Update();
                }
                break;
 
            case WM_TIMER:
                if (msg.wParam == g_timer) {
                    Update();
                }
                break;
 
            case WM_APP_SETTINGS:
                LoadSettings();
                RegisterHotkeyFromSettings();
                g_manualToggle = false;
                g_lastMax = -1;
                Update();
                break;
 
            default:
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
                break;
        }
    }
 
    if (g_hookForeground) UnhookWinEvent(g_hookForeground);
    if (g_hookLocation)   UnhookWinEvent(g_hookLocation);
    g_hookForeground = g_hookLocation = nullptr;
    DisarmTimer();
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, HOTKEY_ID);
        g_hotkeyRegistered = false;
    }
    RestoreOriginal();
    return 0;
}
 
// ------------------------------------------------------------ tool mod hooks
 
BOOL WhTool_ModInit() {
    Wh_Log(L"Init");
 
    g_readyEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_readyEvent) return FALSE;
 
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }
 
    // Wait until the worker has a message queue, so that later posts succeed.
    HANDLE handles[2] = {g_readyEvent, g_thread};
    if (WaitForMultipleObjects(2, handles, FALSE, INFINITE) != WAIT_OBJECT_0) {
        CloseHandle(g_thread);
        CloseHandle(g_readyEvent);
        g_thread = g_readyEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}
 
void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_APP_SETTINGS, 0, 0);
    }
}
 
void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        // Wait without a timeout: the worker must finish (and restore the
        // user's setting) before the process exits.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}
 
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
 
