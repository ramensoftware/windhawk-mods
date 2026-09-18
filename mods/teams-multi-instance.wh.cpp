// ==WindhawkMod==
// @id              teams-multi-instance
// @name            Teams Multi-Instance
// @description     Allows running multiple independent instances of the new Microsoft Teams desktop app
// @version         1.0
// @author          Sam Mahdi
// @github          https://github.com/TSA3000
// @include         ms-teams.exe
// @compilerOptions -lshlwapi -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Teams Multi-Instance

The new Microsoft Teams desktop app only allows one instance per Windows
session — launching it again just activates the existing window. Teams
enforces this with two named mutexes, `Teams-Tfw-instance` and
`Teams-Tfw-server`. This mod appends the process ID to those names so each
launch believes it is the first one, letting you run e.g. your own tenant and
a client tenant side by side in two full Teams windows.

## Usage
1. Enable the mod.
2. Quit Teams completely (tray icon → Quit) and start it again, so the first
   instance is launched with the mod active.
3. Start Teams again. Every additional launch opens a new independent
   instance. Sign in to a different account/tenant in each one.

Each instance gets its own tray icon, with the process ID in the tooltip
(e.g. `Microsoft Teams [20680]`) so they can be told apart. Additional icons
usually land in the tray overflow (the `^` arrow). Quit each instance from
its own tray icon or from inside its window — closing a window only
minimises it.

**Restart Teams after enabling, disabling, or changing the settings of the
mod.** The mod decides once, when a Teams process starts, whether and how to
rewrite names, and keeps that decision for the life of the process. A Teams
instance that was already running when the mod was enabled is left untouched;
an instance started with the mod active keeps its salt across mod updates and
Windhawk restarts; and settings changes only apply to Teams processes started
afterwards.

## How it works
Hooks `NtCreateMutant` / `NtOpenMutant` (and the semaphore equivalents) in
ntdll and appends `_<pid>` to object names matching the configured patterns.
Only mutexes and semaphores are hooked — sections and events are deliberately
left alone, since those fire on the DLL-loader path and hooking them
destabilises the process.

Each `ms-teams.exe` process uses its own PID as the salt. Only the top-level
Teams process creates or opens the `Teams-Tfw-*` objects; child
`ms-teams.exe` processes never touch them, so they need no shared salt.

Teams registers its tray icon with a fixed GUID, and Windows allows only one
icon per GUID, so a second instance would get no icon at all. The mod hooks
`Shell_NotifyIconW` in salted processes, drops the GUID so the icon is
identified by window + per-process id instead, and appends the PID to the
tooltip.

## Limitations
- **All instances share the same Teams profile**
  (`%LOCALAPPDATA%\Packages\MSTeams_8wekyb3d8bbwe\LocalCache`, including the
  WebView2 user-data folder). Two Teams processes writing it concurrently is
  exactly what the single-instance lock exists to prevent. This can corrupt
  the profile, which means signing in again and rebuilding the cache. Avoid
  signing in/out or changing settings in more than one instance, and use at
  your own risk.
- Without the GUID, Windows does not remember each instance's tray icon
  position (pinned vs. overflow) across restarts.
- Notification clicks and `teams://` links are routed by Windows activation
  and may land in a different instance than the one you expect.
- Not supported by Microsoft.

## Troubleshooting
Turn on **Log only**, then quit and restart Teams. The Windhawk log lists
every named mutex/semaphore the new Teams process creates without modifying
any of them (the interesting objects are created during startup, so a running
instance shows nothing useful). If a future Teams build renames the
single-instance objects, add the new name to **Object name patterns**, turn
Log only off, and restart Teams again.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- namePatterns: "Teams-Tfw-"
  $name: Object name patterns
  $description: Comma-separated substrings. Named mutexes/semaphores matching any of these get salted. Applies to Teams processes started after the change.
- fixTrayIcon: true
  $name: Separate tray icon per instance
  $description: Drop the fixed tray-icon GUID so every instance gets its own tray icon, and show the process ID in the tooltip. Applies to Teams processes started after the change.
- logOnly: false
  $name: Log only
  $description: Log every named mutex/semaphore Teams creates without modifying anything. For troubleshooting; restart Teams after changing.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winternl.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <string>
#include <vector>

// Set in the process environment once a process has been salted. Carries the
// PID so that (a) a mod reload inside the same process (mod update, Windhawk
// restart) restores the same salt instead of treating the process as
// "already running", and (b) a child process that inherits the variable sees
// a PID that isn't its own and ignores it. Never cleared on uninit — an
// unload is exactly the reload case it needs to survive.
constexpr PCWSTR kSaltMarker = L"WH_TEAMS_MI_SALTED_PID";

// Settings are read once per process in Wh_ModInit and never updated live:
// changing them mid-life would desynchronise instances that already created
// their objects under the old rules.
struct {
    bool logOnly;
    bool fixTrayIcon;
    std::vector<std::wstring> patterns;
} g_settings;

// Empty means "do not salt in this process" (log-only, or the mod was loaded
// into a process that was already running).
std::wstring g_salt;

// ---------------------------------------------------------------------------
// Named-object salting
// ---------------------------------------------------------------------------

bool ShouldSalt(LPCWSTR name) {
    if (!name || g_settings.logOnly || g_salt.empty())
        return false;
    for (const auto& p : g_settings.patterns) {
        if (StrStrIW(name, p.c_str()))
            return true;
    }
    return false;
}

std::wstring GetOaName(POBJECT_ATTRIBUTES oa) {
    if (oa && oa->ObjectName && oa->ObjectName->Buffer && oa->ObjectName->Length)
        return std::wstring(oa->ObjectName->Buffer,
                            oa->ObjectName->Length / sizeof(WCHAR));
    return {};
}

// Logs the name in log-only mode, and if it matches a pattern, builds a
// salted copy of OBJECT_ATTRIBUTES and points _poa at it. The copy honours
// the caller's Length field so a shorter struct is never over-read, and only
// the copy's ObjectName is re-pointed — the caller's struct is not touched.
#define NT_PROLOG(tag)                                                     \
    std::wstring _name = GetOaName(ObjectAttributes);                      \
    if (g_settings.logOnly && !_name.empty())                              \
        Wh_Log(L"[" tag L"] name: %s", _name.c_str());                     \
    alignas(OBJECT_ATTRIBUTES) BYTE _buf[sizeof(OBJECT_ATTRIBUTES)] = {};  \
    UNICODE_STRING _us;                                                    \
    std::wstring _salted;                                                  \
    POBJECT_ATTRIBUTES _poa = ObjectAttributes;                            \
    if (!_name.empty() && ShouldSalt(_name.c_str())) {                     \
        _salted = _name + L"_" + g_salt;                                   \
        Wh_Log(L"[" tag L"] %s -> %s", _name.c_str(), _salted.c_str());    \
        ULONG _len = ObjectAttributes->Length;                             \
        if (_len > sizeof(_buf))                                           \
            _len = sizeof(_buf);                                           \
        memcpy(_buf, ObjectAttributes, _len);                              \
        auto* _copy = reinterpret_cast<POBJECT_ATTRIBUTES>(_buf);          \
        _us.Buffer = _salted.data();                                       \
        _us.Length = (USHORT)(_salted.size() * sizeof(WCHAR));             \
        _us.MaximumLength = _us.Length + sizeof(WCHAR);                    \
        _copy->ObjectName = &_us;                                          \
        _poa = _copy;                                                      \
    }

using NtCreateMutant_t = NTSTATUS(NTAPI*)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, BOOLEAN);
NtCreateMutant_t NtCreateMutant_Original;
NTSTATUS NTAPI NtCreateMutant_Hook(PHANDLE Handle, ACCESS_MASK Access,
                                   POBJECT_ATTRIBUTES ObjectAttributes, BOOLEAN Owner) {
    NT_PROLOG(L"create-mutant")
    return NtCreateMutant_Original(Handle, Access, _poa, Owner);
}

using NtOpenMutant_t = NTSTATUS(NTAPI*)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
NtOpenMutant_t NtOpenMutant_Original;
NTSTATUS NTAPI NtOpenMutant_Hook(PHANDLE Handle, ACCESS_MASK Access,
                                 POBJECT_ATTRIBUTES ObjectAttributes) {
    NT_PROLOG(L"open-mutant")
    return NtOpenMutant_Original(Handle, Access, _poa);
}

using NtCreateSemaphore_t = NTSTATUS(NTAPI*)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, LONG, LONG);
NtCreateSemaphore_t NtCreateSemaphore_Original;
NTSTATUS NTAPI NtCreateSemaphore_Hook(PHANDLE Handle, ACCESS_MASK Access,
                                      POBJECT_ATTRIBUTES ObjectAttributes,
                                      LONG Initial, LONG Max) {
    NT_PROLOG(L"create-semaphore")
    return NtCreateSemaphore_Original(Handle, Access, _poa, Initial, Max);
}

using NtOpenSemaphore_t = NTSTATUS(NTAPI*)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
NtOpenSemaphore_t NtOpenSemaphore_Original;
NTSTATUS NTAPI NtOpenSemaphore_Hook(PHANDLE Handle, ACCESS_MASK Access,
                                    POBJECT_ATTRIBUTES ObjectAttributes) {
    NT_PROLOG(L"open-semaphore")
    return NtOpenSemaphore_Original(Handle, Access, _poa);
}

// ---------------------------------------------------------------------------
// Tray icon: one icon per instance
// ---------------------------------------------------------------------------

using Shell_NotifyIconW_t = decltype(&Shell_NotifyIconW);
Shell_NotifyIconW_t Shell_NotifyIconW_Original;
BOOL WINAPI Shell_NotifyIconW_Hook(DWORD dwMessage, PNOTIFYICONDATAW lpData) {
    if (!g_settings.fixTrayIcon || g_salt.empty() || !lpData)
        return Shell_NotifyIconW_Original(dwMessage, lpData);

    // Work on a copy; never mutate the caller's struct.
    NOTIFYICONDATAW copy = *lpData;

    // Windows allows only one tray icon per GUID. Drop it so the icon is
    // identified by (hwnd, uID) instead, which is unique per process.
    if (copy.uFlags & NIF_GUID) {
        copy.uFlags &= ~NIF_GUID;
        copy.guidItem = {};
    }
    copy.uID = (UINT)(GetCurrentProcessId() & 0xFFFF);

    // Append the PID to the tooltip so instances can be told apart.
    if (copy.uFlags & NIF_TIP) {
        std::wstring tip = copy.szTip;
        std::wstring suffix = L" [" + g_salt + L"]";
        if (tip.find(suffix) == std::wstring::npos) {
            tip += suffix;
            size_t maxLen = ARRAYSIZE(copy.szTip) - 1;
            if (tip.size() > maxLen)
                tip.resize(maxLen);
            wcscpy_s(copy.szTip, tip.c_str());
        }
    }

    return Shell_NotifyIconW_Original(dwMessage, &copy);
}

// ---------------------------------------------------------------------------
// Settings / init
// ---------------------------------------------------------------------------

void LoadSettings() {
    g_settings.logOnly = Wh_GetIntSetting(L"logOnly");
    g_settings.fixTrayIcon = Wh_GetIntSetting(L"fixTrayIcon");

    g_settings.patterns.clear();
    PCWSTR raw = Wh_GetStringSetting(L"namePatterns");
    std::wstring s(raw ? raw : L"");
    Wh_FreeStringSetting(raw);

    size_t pos = 0;
    while (pos != std::wstring::npos) {
        size_t next = s.find(L',', pos);
        std::wstring token = s.substr(pos, next == std::wstring::npos ? next : next - pos);
        size_t a = token.find_first_not_of(L' ');
        size_t b = token.find_last_not_of(L' ');
        if (a != std::wstring::npos)
            g_settings.patterns.push_back(token.substr(a, b - a + 1));
        pos = (next == std::wstring::npos) ? next : next + 1;
    }
}

// True when Wh_ModInit runs on the process's initial thread, i.e. Windhawk
// loaded the mod before the process started executing. When the mod is
// loaded into an already-running process, Wh_ModInit runs on the Windhawk
// engine thread instead. See Windhawk's "Development tips".
bool IsInitialThread() {
#ifdef _WIN64
    const size_t OFFSET_SAME_TEB_FLAGS = 0x17EE;
#else
    const size_t OFFSET_SAME_TEB_FLAGS = 0x0FCA;
#endif
    return *(USHORT*)((BYTE*)NtCurrentTeb() + OFFSET_SAME_TEB_FLAGS) & 0x0400;
}

void InitSalt() {
    std::wstring pid = std::to_wstring(GetCurrentProcessId());

    WCHAR marker[16];
    DWORD n = GetEnvironmentVariableW(kSaltMarker, marker, ARRAYSIZE(marker));
    bool alreadySalted = n > 0 && n < ARRAYSIZE(marker) && pid == marker;

    if (alreadySalted) {
        Wh_Log(L"Mod reloaded in an already salted process; keeping salt %s", pid.c_str());
    } else if (!IsInitialThread()) {
        Wh_Log(L"Loaded into a running process; object names left untouched");
        return;  // g_salt stays empty -> ShouldSalt() is a no-op
    } else {
        Wh_Log(L"Salt: %s", pid.c_str());
    }

    g_salt = pid;
    SetEnvironmentVariableW(kSaltMarker, pid.c_str());
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();
    InitSalt();

    if (g_salt.empty() && !g_settings.logOnly) {
        Wh_Log(L"Nothing to do in this process");
        return FALSE;
    }

    bool ok = true;

#define HOOK(mod, fn)                                                       \
    do {                                                                    \
        FARPROC p = GetProcAddress(mod, #fn);                               \
        if (!p || !Wh_SetFunctionHook((void*)p, (void*)fn##_Hook,           \
                                      (void**)&fn##_Original)) {            \
            Wh_Log(L"Hook FAILED: %S", #fn);                                \
            ok = false;                                                     \
        }                                                                   \
    } while (0)

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    HOOK(ntdll, NtCreateMutant);
    HOOK(ntdll, NtOpenMutant);
    HOOK(ntdll, NtCreateSemaphore);
    HOOK(ntdll, NtOpenSemaphore);

    if (g_settings.fixTrayIcon && !g_salt.empty()) {
        HMODULE shell32 = LoadLibraryW(L"shell32.dll");
        if (shell32) {
            HOOK(shell32, Shell_NotifyIconW);
        } else {
            Wh_Log(L"LoadLibrary shell32.dll failed");
            ok = false;
        }
    }
#undef HOOK

    // A partial hook set is worse than none: objects could be created under
    // salted names and looked up under unsalted ones. Bail out entirely.
    if (!ok)
        return FALSE;

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    // kSaltMarker is intentionally left in the environment so a reload of the
    // mod in this process restores the same salt.
}

void Wh_ModSettingsChanged() {
    // Deliberately not reloading settings: they apply to Teams processes
    // started after the change. See the README.
    Wh_Log(L"Settings changed; they apply to Teams processes started from now on");
}
