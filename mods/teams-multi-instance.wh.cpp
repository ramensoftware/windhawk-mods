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
rewrite names, and keeps that decision — and the settings in effect at the
time — for the life of the process. A Teams instance that was already
running when the mod was enabled is left untouched; an instance started with
the mod active keeps its salt across mod updates and Windhawk restarts; and
settings changes only apply to Teams processes started afterwards.

## How it works
Hooks `NtCreateMutant` / `NtOpenMutant` in ntdll and appends `_<pid>` to
object names matching the configured patterns. Only mutexes are hooked —
sections and events are deliberately left alone, since those fire on the
DLL-loader path and hooking them destabilises the process.

Each `ms-teams.exe` process uses its own PID as the salt. Only the top-level
Teams process creates or opens the `Teams-Tfw-*` objects; child
`ms-teams.exe` processes never touch them, so they need no shared salt.

Teams registers its tray icon with a fixed GUID, and Windows allows only one
icon per GUID, so a second instance would get no icon at all. The mod hooks
`Shell_NotifyIconW` in salted processes, drops the GUID so the icon is
identified by its window instead, and appends the PID to the tooltip.

## Limitations
- **All instances share the same Teams profile**
  (`%LOCALAPPDATA%\Packages\MSTeams_8wekyb3d8bbwe\LocalCache`, including the
  WebView2 user-data folder). Two Teams processes writing it concurrently is
  exactly what the single-instance lock exists to prevent. This can corrupt
  the profile, which means signing in again and rebuilding the cache. Avoid
  signing in/out or changing settings in more than one instance, and use at
  your own risk.
- Notification clicks and `teams://` links are routed by Windows activation
  and may land in a different instance than the one you expect.
- Helpers that are not `ms-teams.exe` (the Teams Meeting Add-in in Outlook,
  the updater) are not hooked and cannot reach a salted instance's objects;
  meeting-join from Outlook may target the wrong instance or none.
- Not supported by Microsoft.

## Troubleshooting
Turn on **Log only**, then quit and restart Teams. The Windhawk log lists
every named mutex the new Teams process creates or opens without modifying
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
  $description: Comma-separated substrings. Named mutexes matching any of these get salted. Applies to Teams processes started after the change.
- fixTrayIcon: true
  $name: Separate tray icon per instance
  $description: Drop the fixed tray-icon GUID so every instance gets its own tray icon, and show the process ID in the tooltip. Applies to Teams processes started after the change.
- logOnly: false
  $name: Log only
  $description: Log every named mutex Teams creates or opens without modifying anything. For troubleshooting; restart Teams after changing.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <winternl.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <string>
#include <vector>

// Written to the process environment once this process is committed to
// salting, i.e. only after every hook is installed. Format:
//   <pid>|<fixTrayIcon 0/1>|<namePatterns>
// It carries the PID so a child process that inherits the variable sees a PID
// that isn't its own and ignores it, and it carries the effective settings so
// a mod reload inside the same process (mod update, Windhawk restart)
// restores exactly the configuration the process started with instead of
// re-reading settings that may have changed since. Never cleared on uninit —
// an unload is exactly the reload case it needs to survive.
constexpr PCWSTR kSaltMarker = L"WH_TEAMS_MI_SALTED";

// Settings are read once per process and never updated live: changing them
// mid-life would desynchronise instances that already created their objects
// under the old rules.
struct {
    bool logOnly;
    bool fixTrayIcon;
    std::wstring patternsRaw;
    std::vector<std::wstring> patterns;
} g_settings;

// Empty means "do not salt in this process" (log-only, or the mod was loaded
// into a process that was already running and never salted).
std::wstring g_salt;

// ---------------------------------------------------------------------------
// Named-object salting
// ---------------------------------------------------------------------------

bool ShouldSalt(LPCWSTR name) {
    if (!name || g_salt.empty())
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

// ---------------------------------------------------------------------------
// Tray icon: one icon per instance
// ---------------------------------------------------------------------------

using Shell_NotifyIconW_t = decltype(&Shell_NotifyIconW);
Shell_NotifyIconW_t Shell_NotifyIconW_Original;
BOOL WINAPI Shell_NotifyIconW_Hook(DWORD dwMessage, PNOTIFYICONDATAW lpData) {
    if (!g_settings.fixTrayIcon || g_salt.empty() || !lpData)
        return Shell_NotifyIconW_Original(dwMessage, lpData);

    // Work on a copy sized by the caller's cbSize; never mutate the caller's
    // struct and never read past the version it actually passed.
    NOTIFYICONDATAW copy = {};
    DWORD cb = lpData->cbSize;
    if (cb > sizeof(copy))
        cb = sizeof(copy);
    memcpy(&copy, lpData, cb);

    // Windows allows only one tray icon per GUID. Drop it so the icon is
    // identified by (hWnd, uID) instead; hWnd is already unique per process,
    // so Teams's own uID can stay as it is.
    if (copy.cbSize >= NOTIFYICONDATAW_V3_SIZE && (copy.uFlags & NIF_GUID)) {
        copy.uFlags &= ~NIF_GUID;
        copy.guidItem = {};
    }

    // Append the PID to the tooltip so instances can be told apart.
    if (copy.uFlags & NIF_TIP) {
        // szTip is 64 chars in the V1 layout, 128 from V2 on.
        size_t maxLen = (copy.cbSize >= NOTIFYICONDATAW_V2_SIZE) ? 127 : 63;
        std::wstring tip(copy.szTip, wcsnlen(copy.szTip, maxLen));
        std::wstring suffix = L" [" + g_salt + L"]";
        if (tip.find(suffix) == std::wstring::npos) {
            tip += suffix;
            if (tip.size() > maxLen)
                tip.resize(maxLen);
            wcscpy_s(copy.szTip, maxLen + 1, tip.c_str());
        }
    }

    return Shell_NotifyIconW_Original(dwMessage, &copy);
}

// ---------------------------------------------------------------------------
// Settings / init
// ---------------------------------------------------------------------------

void SetPatterns(const std::wstring& raw) {
    g_settings.patternsRaw = raw;
    g_settings.patterns.clear();

    size_t pos = 0;
    while (pos != std::wstring::npos) {
        size_t next = raw.find(L',', pos);
        std::wstring token = raw.substr(pos, next == std::wstring::npos ? next : next - pos);
        size_t a = token.find_first_not_of(L' ');
        size_t b = token.find_last_not_of(L' ');
        if (a != std::wstring::npos)
            g_settings.patterns.push_back(token.substr(a, b - a + 1));
        pos = (next == std::wstring::npos) ? next : next + 1;
    }
}

void LoadSettings() {
    g_settings.logOnly = Wh_GetIntSetting(L"logOnly");
    g_settings.fixTrayIcon = Wh_GetIntSetting(L"fixTrayIcon");
    SetPatterns(WindhawkUtils::StringSetting::make(L"namePatterns").get());
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

    if (IsInitialThread()) {
        // Fresh process: decide now, once, from the current settings.
        if (g_settings.logOnly) {
            Wh_Log(L"Log only; not salting");
            return;
        }
        g_salt = pid;
        Wh_Log(L"Salt: %s", pid.c_str());
        return;
    }

    // Loaded into a running process. Keep salting only if an earlier load of
    // the mod already committed this process to it (mod update, Windhawk
    // restart). The marker — not the current settings — is authoritative, so
    // the process continues with exactly the configuration it started with.
    WCHAR marker[1024];
    DWORD n = GetEnvironmentVariableW(kSaltMarker, marker, ARRAYSIZE(marker));
    if (n > 0 && n < ARRAYSIZE(marker)) {
        std::wstring m(marker);
        size_t p1 = m.find(L'|');
        size_t p2 = (p1 == std::wstring::npos) ? p1 : m.find(L'|', p1 + 1);
        if (p1 != std::wstring::npos && p2 != std::wstring::npos &&
            m.compare(0, p1, pid) == 0) {
            g_salt = pid;
            g_settings.logOnly = false;
            g_settings.fixTrayIcon = (m[p1 + 1] == L'1');
            SetPatterns(m.substr(p2 + 1));
            Wh_Log(L"Mod reloaded in an already salted process; keeping salt %s", pid.c_str());
            return;
        }
    }

    Wh_Log(L"Loaded into a running process; object names left untouched");
}

template <typename T>
bool HookExport(HMODULE mod, PCSTR name, T* hook, T** original) {
    auto* target = (T*)GetProcAddress(mod, name);
    if (!target || !WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"Hook FAILED: %S", name);
        return false;
    }
    return true;
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

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    ok &= HookExport(ntdll, "NtCreateMutant", NtCreateMutant_Hook, &NtCreateMutant_Original);
    ok &= HookExport(ntdll, "NtOpenMutant", NtOpenMutant_Hook, &NtOpenMutant_Original);

    if (g_settings.fixTrayIcon && !g_salt.empty()) {
        if (!WindhawkUtils::SetFunctionHook(Shell_NotifyIconW, Shell_NotifyIconW_Hook,
                                            &Shell_NotifyIconW_Original)) {
            Wh_Log(L"Hook FAILED: Shell_NotifyIconW");
            ok = false;
        }
    }

    // A partial hook set is worse than none: objects could be created under
    // salted names and looked up under unsalted ones. Bail out entirely.
    if (!ok)
        return FALSE;

    // Stamp the process only now that every hook is in place, so a failed
    // init is not mistaken for a salted process on the next load.
    if (!g_salt.empty()) {
        std::wstring marker = g_salt + L"|" + (g_settings.fixTrayIcon ? L"1" : L"0") +
                              L"|" + g_settings.patternsRaw;
        SetEnvironmentVariableW(kSaltMarker, marker.c_str());
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    // kSaltMarker is intentionally left in the environment so a reload of the
    // mod in this process restores the same salt and settings.
}

void Wh_ModSettingsChanged() {
    // Deliberately not reloading settings: they apply to Teams processes
    // started after the change. See the README.
    Wh_Log(L"Settings changed; they apply to Teams processes started from now on");
}
