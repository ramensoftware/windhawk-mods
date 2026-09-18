// ==WindhawkMod==
// @id              teams-multi-instance
// @name            Teams Multi-Instance
// @description     Allows running multiple independent instances of the new Microsoft Teams desktop app
// @version         1.0
// @author          Sam Mahdi
// @github          https://github.com/TSA3000
// @include         ms-teams.exe
// @compilerOptions -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Teams Multi-Instance

The new Microsoft Teams desktop app only allows one instance per Windows
session — launching it again just activates the existing window. Teams
enforces this with two named mutexes, `Teams-Tfw-instance` and
`Teams-Tfw-server`. This mod appends a per-launch salt to those names so each
launch believes it is the first one, letting you run e.g. your own tenant and
a client tenant side by side in two full Teams windows.

## Usage
Enable the mod and simply start Teams again while it is already running.
Each launch opens a new independent instance. Sign in to a different
account/tenant in each one.

Each instance has its own tray icon; the second one is usually hidden in the
tray overflow (the `^` arrow). Quit each instance separately from its own
tray icon or from inside its window.

## How it works
Hooks `NtCreateMutant` / `NtOpenMutant` in ntdll and rewrites object names
matching the configured patterns. Child processes (WebView2 etc.) inherit the
same salt via an environment variable so objects shared inside one Teams
process tree keep working. Only mutexes and semaphores are hooked — sections
and events are deliberately left alone, since those fire on the DLL-loader
path and hooking them destabilises the process.

## Limitations
- All instances share the same profile directory
  (`%LOCALAPPDATA%\Packages\MSTeams_8wekyb3d8bbwe\LocalCache`). Reading is
  fine, but avoid signing in/out or changing settings in one instance while
  another is running — treat one instance as "primary" for that.
- Notification clicks and `teams://` links are routed by Windows activation
  and may land in a different instance than the one you expect.
- Not supported by Microsoft. Use at your own risk.

## Troubleshooting
Turn on **Log only** and check the Windhawk log: it lists every named
mutex/semaphore Teams creates without modifying any of them. If a future
Teams build renames the single-instance objects, add the new name to
**Object name patterns**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- namePatterns: "Teams-Tfw-"
  $name: Object name patterns
  $description: Comma-separated substrings. Named mutexes/semaphores matching any of these get salted.
- logOnly: false
  $name: Log only
  $description: Log every named mutex/semaphore Teams creates without modifying anything. For troubleshooting.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winternl.h>
#include <shlwapi.h>
#include <string>
#include <vector>

const wchar_t* kSaltEnvVar = L"WH_TEAMS_MI_SALT";

struct {
    bool logOnly;
    std::vector<std::wstring> patterns;
} g_settings;

std::wstring g_salt;

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
// salted copy of OBJECT_ATTRIBUTES and points _poa at it.
#define NT_PROLOG(tag)                                                     \
    std::wstring _name = GetOaName(ObjectAttributes);                      \
    if (g_settings.logOnly && !_name.empty())                              \
        Wh_Log(L"[" tag L"] name: %s", _name.c_str());                     \
    OBJECT_ATTRIBUTES _oa;                                                 \
    UNICODE_STRING _us;                                                    \
    std::wstring _salted;                                                  \
    POBJECT_ATTRIBUTES _poa = ObjectAttributes;                            \
    if (!_name.empty() && ShouldSalt(_name.c_str())) {                     \
        _salted = _name + L"_" + g_salt;                                   \
        Wh_Log(L"[" tag L"] %s -> %s", _name.c_str(), _salted.c_str());    \
        _oa = *ObjectAttributes;                                           \
        _us.Buffer = _salted.data();                                       \
        _us.Length = (USHORT)(_salted.size() * sizeof(WCHAR));             \
        _us.MaximumLength = _us.Length + sizeof(WCHAR);                    \
        _oa.ObjectName = &_us;                                             \
        _poa = &_oa;                                                       \
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

void LoadSettings() {
    g_settings.logOnly = Wh_GetIntSetting(L"logOnly");

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

void InitSalt() {
    WCHAR buf[64];
    // Inherit the salt from the parent (WebView2 children etc.) so one Teams
    // process tree shares objects; otherwise create one for this launch.
    if (GetEnvironmentVariableW(kSaltEnvVar, buf, ARRAYSIZE(buf)) > 0) {
        g_salt = buf;
        Wh_Log(L"Inherited salt: %s", g_salt.c_str());
    } else {
        swprintf_s(buf, L"%lu", GetCurrentProcessId());
        SetEnvironmentVariableW(kSaltEnvVar, buf);
        g_salt = buf;
        Wh_Log(L"Created salt: %s", g_salt.c_str());
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();
    InitSalt();

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");

#define HOOK(fn)                                                            \
    do {                                                                    \
        FARPROC p = GetProcAddress(ntdll, #fn);                             \
        if (!p || !Wh_SetFunctionHook((void*)p, (void*)fn##_Hook,           \
                                      (void**)&fn##_Original))              \
            Wh_Log(L"Hook FAILED: %S", #fn);                                \
    } while (0)

    HOOK(NtCreateMutant);
    HOOK(NtOpenMutant);
    HOOK(NtCreateSemaphore);
    HOOK(NtOpenSemaphore);
#undef HOOK

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
