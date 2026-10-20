// ==WindhawkMod==
// @id              windows-10-action-center-restorer
// @name            Windows 10 Action Center Restorer on Windows 11 24H2+
// @description     This mod restores the Windows 10 Action Center on Windows 11 with the fixed slide animation
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -luser32 -ldwmapi -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 Action Center Restorer on Windows 11 24H2+

This mod restores the Windows 10 Action Center on Windows 11 24H2+ **without
modifying the real Windows registry**.

While the mod is loaded, the following value is exposed virtually, only
inside the processes the mod is injected into:

```
HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Control Center
    UseLiteLayout = 1 (REG_DWORD)
```

## Screenshot

![The restored Windows 10 Action Center](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win10actioncenter.png)

## Requirements

- Windows 11, version 24H2 or 25H2 (x86-64) - the versions this mod is written
  and tested for. It is not meant for Windows 10.
- Windhawk, with mod injection into system processes enabled. The mod is
  injected into `explorer.exe`, `ShellExperienceHost.exe` and `ShellHost.exe`.
- Strongly recommended, but not required: the Windows 10 taskbar, i.e. the mod
  [Win10 taskbar on Win11 24H2 or 25H2](https://windhawk.net/mods/win10-taskbar-on-win11-24h2)
  and, with it,
  [Fake Explorer path](https://windhawk.net/mods/fake-explorer-path), which
  that mod requires. The restored panel is opened and animated by the Windows
  10 shell: the tray button and Win+A, the slide animation and the shortened
  close timer are its paths (the shell's `twinui.pcshell.dll`). The virtual
  value is served in the shell hosts whatever taskbar is running, so the mod
  does not depend on the Windows 10 taskbar, but that is the setup it is
  written for.

## How the virtualization works

Microsoft's built-in registry virtualization (UAC virtualization) is not
available for 64-bit processes, so the value is virtualized at the Win32
registry API level (kernelbase.dll):

- `RegGetValueW` - the API ShellExperienceHost actually uses to read
  `UseLiteLayout` (see ExplorerPatcher's `ShellExperienceHostPatches.cpp`).
- `RegQueryValueExW` - for code that opens the key and queries the value.
- `RegOpenKeyExW` - if the `Control Center` key doesn't exist, a read-only
  handle to an **empty private application hive** (`RegLoadAppKeyW`) is
  returned, so the key behaves exactly like an existing empty key that
  contains only `UseLiteLayout`. No real key is created and no other key's
  values are exposed.
- `RegEnumValueW` / `RegQueryInfoKeyW` - keep enumeration and value counts
  consistent with the virtual value.

Keys are identified by their real kernel path (`NtQueryKey`), so relative
opens (e.g. `CurrentVersion` handle + `Control Center`), different casing
and trailing backslashes are all handled, and there is no global
"last opened handle" state that can be confused by handle reuse.

The application hive is kept in the storage directory Windhawk gives the mod
and removes together with it: the first time the virtual key is needed, an
empty hive file (a few KB) plus the `.LOG1`/`.LOG2` transaction logs the
registry maintains next to a loaded hive are created there. Nothing is
written when the key is never opened.

## Applying changes

`UseLiteLayout` is read when ShellExperienceHost (and parts of Explorer)
start. When the mod is enabled, disabled, or the virtual value setting is
toggled, the mod restarts ShellExperienceHost (like ExplorerPatcher does) -
but only from the shell process that owns the taskbar. Folder windows,
`explorer.exe /factory,... -Embedding` COM servers and the like run in
explorer.exe processes of their own, and they must not kill a
ShellExperienceHost that already reads the virtual value. It is started again
automatically on demand. If the Win+A / tray button still opens the Windows
11 panel after enabling the mod at runtime, restart Explorer once.

## Features

- Virtual `UseLiteLayout=1`, no HKLM modification
- Fast Action Center close timer
- Slide-in / slide-out animations, also on the very first open/close after
  ShellExperienceHost starts (window identified by its thread description,
  system repositioning absorbed into the animation)
- No files are replaced under C:\Windows, and none are left behind either:
  the virtual key's hive lives in Windhawk's storage for this mod

## Credits 
- AdmnistratoX - Fix for the animation of the Windows 10 Action Center 
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- delayMs: 50
  $name: Close delay (ms)
  $description: This setting sets the close delay in milliseconds, replacing the built-in ~2000 ms timer (the valid range is between 1 and 1900)

- slideInMs: 220
  $name: Slide-in duration (ms)
  $description: This setting controls the slide-in animation duration in milliseconds; set it to 0 to disable the animation

- slideOutMs: 140
  $name: Slide-out duration (ms)
  $description: This setting controls the slide-out animation duration in milliseconds; set it to 0 to disable the animation

- virtualLiteLayout: true
  $name: Virtual UseLiteLayout
  $description: This setting virtually reports UseLiteLayout=1 without changing the registry

- restartShellExperienceHost: true
  $name: Restart ShellExperienceHost when needed
  $description: This setting restarts ShellExperienceHost when the mod is loaded or unloaded, or when the virtual value is toggled, so the change takes effect immediately

- lateShowFallback: true
  $name: First-show fallback animation
  $description: This setting enables a fallback animation for the first show when a show bypasses all hooked APIs; the panel may flash for one frame before sliding in
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <tlhelp32.h>
#include <stdint.h>
#include <wchar.h>
#include <string.h>
#include <memory>
#include <atomic>
#include <vector>
#include <climits>

// ---------------------------------------------------------------------------
// RAII helpers
// ---------------------------------------------------------------------------

class ScopedHandle {
public:
    ScopedHandle() : m_h(nullptr) {}
    explicit ScopedHandle(HANDLE h)
        : m_h(h == INVALID_HANDLE_VALUE ? nullptr : h) {}

    ~ScopedHandle() {
        if (m_h)
            CloseHandle(m_h);
    }

    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;

    HANDLE get() const { return m_h; }
    bool valid() const { return m_h != nullptr; }

    HANDLE release() {
        HANDLE h = m_h;
        m_h = nullptr;
        return h;
    }

private:
    HANDLE m_h;
};

class SrwExclusive {
public:
    explicit SrwExclusive(SRWLOCK* l) : m_l(l) { AcquireSRWLockExclusive(m_l); }
    ~SrwExclusive() { ReleaseSRWLockExclusive(m_l); }
    SrwExclusive(const SrwExclusive&) = delete;
    SrwExclusive& operator=(const SrwExclusive&) = delete;

private:
    SRWLOCK* m_l;
};

// ---------------------------------------------------------------------------
// Process type
// ---------------------------------------------------------------------------

enum class ProcessKind {
    Other,
    Explorer,
    ShellExperienceHost,
    ShellHost,
};

static ProcessKind g_processKind = ProcessKind::Other;

static ProcessKind DetectProcessKind() {
    wchar_t path[MAX_PATH] = {};
    DWORD n = GetModuleFileNameW(nullptr, path, _countof(path));
    if (!n || n >= _countof(path))
        return ProcessKind::Other;

    const wchar_t* b = wcsrchr(path, L'\\');
    b = b ? b + 1 : path;

    if (_wcsicmp(b, L"explorer.exe") == 0)
        return ProcessKind::Explorer;
    if (_wcsicmp(b, L"ShellExperienceHost.exe") == 0)
        return ProcessKind::ShellExperienceHost;
    if (_wcsicmp(b, L"ShellHost.exe") == 0)
        return ProcessKind::ShellHost;

    return ProcessKind::Other;
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

static std::atomic<int> g_delayMs{50};
static std::atomic<int> g_slideInMs{220};
static std::atomic<int> g_slideOutMs{140};

static std::atomic<bool> g_virtualLiteLayout{true};
static std::atomic<bool> g_restartSeh{true};
static std::atomic<bool> g_lateShowFallback{true};

static int Clamp(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static void LoadSettings() {
    g_delayMs = Clamp(Wh_GetIntSetting(L"delayMs"), 1, 1900);
    g_slideInMs = Clamp(Wh_GetIntSetting(L"slideInMs"), 0, 1000);
    g_slideOutMs = Clamp(Wh_GetIntSetting(L"slideOutMs"), 0, 1000);

    g_virtualLiteLayout = Wh_GetIntSetting(L"virtualLiteLayout") != 0;
    g_restartSeh = Wh_GetIntSetting(L"restartShellExperienceHost") != 0;
    g_lateShowFallback = Wh_GetIntSetting(L"lateShowFallback") != 0;
}

// ---------------------------------------------------------------------------
// Registry virtualization
//
// 64-bit processes don't get UAC registry virtualization, so the value is
// virtualized at the Win32 API level. The target key is identified by its
// kernel object name, obtained with NtQueryKey(KeyNameInformation).
//
// Target:
//   \REGISTRY\MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Control Center
//       UseLiteLayout = REG_DWORD 1
// ---------------------------------------------------------------------------

static constexpr wchar_t kTargetNtPath[] =
    L"\\REGISTRY\\MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Control Center";
static constexpr wchar_t kHklmNtPath[] = L"\\REGISTRY\\MACHINE";
static constexpr wchar_t kLeafName[] = L"Control Center";
static constexpr wchar_t kUseLiteLayout[] = L"UseLiteLayout";

static constexpr int kTargetNtPathLen = _countof(kTargetNtPath) - 1;
static constexpr int kHklmNtPathLen = _countof(kHklmNtPath) - 1;
static constexpr int kLeafNameLen = _countof(kLeafName) - 1;
static constexpr int kUseLiteLayoutLen = _countof(kUseLiteLayout) - 1;

static constexpr DWORD kLiteLayoutValue = 1;

// NtQueryKey (documented as ZwQueryKey in the WDK).
using NtQueryKey_t = LONG(NTAPI*)(HANDLE KeyHandle,
                                  int KeyInformationClass,
                                  PVOID KeyInformation,
                                  ULONG Length,
                                  PULONG ResultLength);
static constexpr int kKeyNameInformation = 3;
static NtQueryKey_t g_NtQueryKey = nullptr;

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
using RegGetValueW_t = decltype(&RegGetValueW);
using RegEnumValueW_t = decltype(&RegEnumValueW);
using RegQueryInfoKeyW_t = decltype(&RegQueryInfoKeyW);

static RegOpenKeyExW_t RegOpenKeyExW_Original = nullptr;
static RegQueryValueExW_t RegQueryValueExW_Original = nullptr;
static RegGetValueW_t RegGetValueW_Original = nullptr;
static RegEnumValueW_t RegEnumValueW_Original = nullptr;
static RegQueryInfoKeyW_t RegQueryInfoKeyW_Original = nullptr;

static bool EqualsI(const wchar_t* a, int la, const wchar_t* b, int lb) {
    return la == lb &&
           (la == 0 ||
            CompareStringOrdinal(a, la, b, lb, TRUE) == CSTR_EQUAL);
}

static bool IsLiteLayoutValue(LPCWSTR valueName) {
    if (!valueName)
        return false;
    size_t len = wcsnlen(valueName, kUseLiteLayoutLen + 1);
    return EqualsI(valueName, (int)len, kUseLiteLayout, kUseLiteLayoutLen);
}

static bool IsPredefinedKey(HKEY h) {
    // HKEY_CLASSES_ROOT (0x80000000) ... HKEY_PERFORMANCE_NLSTEXT (0x80000060),
    // sign-extended on 64-bit.
    ULONG_PTR v = (ULONG_PTR)h;
    ULONG_PTR base = (ULONG_PTR)HKEY_CLASSES_ROOT;
    return v >= base && v <= base + 0xFF;
}

// Length of a subkey with trailing backslashes removed.
static size_t TrimmedSubKeyLen(LPCWSTR subKey) {
    if (!subKey)
        return 0;
    size_t len = wcslen(subKey);
    while (len && subKey[len - 1] == L'\\')
        len--;
    return len;
}

static bool SubKeyIsEmpty(LPCWSTR subKey) {
    return !subKey || !*subKey;
}

// Cheap pre-check: does the subkey's last component equal "Control Center"?
static bool LeafIsControlCenter(LPCWSTR subKey) {
    size_t len = TrimmedSubKeyLen(subKey);
    if (len < (size_t)kLeafNameLen)
        return false;
    const wchar_t* leaf = subKey + len - kLeafNameLen;
    if (len > (size_t)kLeafNameLen && leaf[-1] != L'\\')
        return false;
    return EqualsI(leaf, kLeafNameLen, kLeafName, kLeafNameLen);
}

// Gets the kernel name of a key handle. Returns the length in characters,
// or -1 on failure. Names that don't fit can't match anything we compare
// against, so failure is treated as "no match".
static int GetKeyNtPath(HKEY h, wchar_t* out, int cchOut) {
    if (!h)
        return -1;

    if (h == HKEY_LOCAL_MACHINE) {
        if (cchOut <= kHklmNtPathLen)
            return -1;
        memcpy(out, kHklmNtPath, kHklmNtPathLen * sizeof(wchar_t));
        return kHklmNtPathLen;
    }

    // Other predefined keys (HKCU, HKCR, ...) or tagged handles never refer
    // to the target key.
    if (IsPredefinedKey(h) || ((ULONG_PTR)h & 3) || !g_NtQueryKey)
        return -1;

    alignas(ULONG) BYTE buffer[sizeof(ULONG) + 512 * sizeof(wchar_t)];
    ULONG resultLength = 0;
    LONG status = g_NtQueryKey(h, kKeyNameInformation, buffer, sizeof(buffer),
                               &resultLength);
    if (status < 0)
        return -1;

    ULONG nameBytes = *(ULONG*)buffer;
    int len = (int)(nameBytes / sizeof(wchar_t));
    if (len <= 0 || len >= cchOut ||
        nameBytes > sizeof(buffer) - sizeof(ULONG)) {
        return -1;
    }

    memcpy(out, buffer + sizeof(ULONG), len * sizeof(wchar_t));
    return len;
}

// ---------------------------------------------------------------------------
// Empty private application hive (RegLoadAppKeyW)
//
// Used as the backing handle when a caller opens the target key and it
// doesn't exist. Being a real empty key, every API that we don't hook
// behaves exactly like for an empty key; only UseLiteLayout is injected.
//
// The hive file is created lazily, in the storage directory Wh_GetModStoragePath
// returns (see LoadAppHiveFile): nothing is written unless a caller really
// opens the missing key, and Windhawk removes the directory together with the
// mod, so no file of the mod is left behind.
// ---------------------------------------------------------------------------

static SRWLOCK g_appHiveLock = SRWLOCK_INIT;
static HKEY g_appHiveRoot = nullptr;
static bool g_appHiveAttempted = false;
static wchar_t g_appHiveNtPath[256] = {};
static std::atomic<int> g_appHiveNtPathLen{0};

// The hive belongs in the mod's storage directory, not in a temp folder: the
// directory is removed by Windhawk together with the mod, while a hive in the
// temp folder - and the .LOG1/.LOG2 transaction logs the registry creates next
// to a loaded hive - would stay there after the mod is disabled or removed.
// Windhawk creates the directory with Modify access for Everyone, All
// Application Packages and All Restricted Application Packages, so
// ShellExperienceHost's AppContainer can create and load the file there too.
static HKEY LoadAppHiveFile(const wchar_t* fileName) {
    wchar_t storageDir[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(storageDir, _countof(storageDir))) {
        Wh_Log(L"Registry virtualization: no mod storage directory, the "
               L"missing key stays missing");
        return nullptr;
    }

    wchar_t path[MAX_PATH] = {};
    if (_snwprintf(path, _countof(path) - 1, L"%s\\%s", storageDir,
                   fileName) <= 0)
        return nullptr;

    HKEY root = nullptr;
    // dwOptions = 0: the hive may be loaded concurrently by other processes
    // (explorer.exe, ShellHost.exe, ShellExperienceHost.exe) without
    // conflicts.
    LSTATUS st = RegLoadAppKeyW(path, &root, KEY_READ, 0, 0);
    if (st != ERROR_SUCCESS) {
        Wh_Log(L"RegLoadAppKeyW(%s) failed: %ld", path, st);
        return nullptr;
    }

    return root;
}

static HKEY GetAppHiveRoot() {
    {
        SrwExclusive lock(&g_appHiveLock);

        if (!g_appHiveAttempted) {
            g_appHiveAttempted = true;

            // One shared file: the hive may be loaded by several processes
            // (explorer.exe, ShellHost.exe, ShellExperienceHost.exe) at once.
            HKEY root =
                LoadAppHiveFile(L"windhawk-w10-action-center-empty.hiv");

            if (root) {
                int len = GetKeyNtPath(root, g_appHiveNtPath,
                                       _countof(g_appHiveNtPath));
                if (len > 0) {
                    g_appHiveRoot = root;
                    // Publish the name after it has been fully written.
                    g_appHiveNtPathLen.store(len, std::memory_order_release);
                    Wh_Log(L"Registry virtualization: empty app hive %.*s",
                           len, g_appHiveNtPath);
                } else {
                    RegCloseKey(root);
                }
            }
        }
    }

    return g_appHiveRoot;
}

// Open a new handle to the (empty) app hive root for a caller.
static bool OpenPhantomControlCenterKey(PHKEY phkResult) {
    HKEY root = GetAppHiveRoot();
    if (!root)
        return false;

    HKEY h = nullptr;
    // Empty subkey: returns a new handle to the same key.
    if (RegOpenKeyExW_Original(root, nullptr, 0, KEY_READ, &h) !=
        ERROR_SUCCESS) {
        return false;
    }

    *phkResult = h;
    return true;
}

// ---------------------------------------------------------------------------
// Key classification
// ---------------------------------------------------------------------------

enum class KeyMatch {
    None,
    Target,   // the real Control Center key (it exists)
    Phantom,  // our empty app hive handle (Control Center didn't exist)
};

// Classifies hKey\subKey. Only called after the cheap value-name checks.
static KeyMatch ClassifyKey(HKEY hKey, LPCWSTR subKey) {
    wchar_t path[600];
    int len = GetKeyNtPath(hKey, path, _countof(path));
    if (len < 0)
        return KeyMatch::None;

    size_t subLen = TrimmedSubKeyLen(subKey);

    if (subLen == 0) {
        int hiveLen = g_appHiveNtPathLen.load(std::memory_order_acquire);
        if (hiveLen > 0 && EqualsI(path, len, g_appHiveNtPath, hiveLen)) {
            return KeyMatch::Phantom;
        }
    } else {
        // A leading backslash is not a valid relative path.
        if (subKey[0] == L'\\')
            return KeyMatch::None;
        if ((size_t)len + 1 + subLen >= _countof(path))
            return KeyMatch::None;
        path[len++] = L'\\';
        memcpy(path + len, subKey, subLen * sizeof(wchar_t));
        len += (int)subLen;
    }

    if (EqualsI(path, len, kTargetNtPath, kTargetNtPathLen))
        return KeyMatch::Target;

    return KeyMatch::None;
}

// ---------------------------------------------------------------------------
// Value emulation helpers (semantics per Microsoft Learn)
// ---------------------------------------------------------------------------

// RegQueryValueExW semantics.
static LSTATUS EmulateQueryValueEx(LPDWORD lpType,
                                   LPBYTE lpData,
                                   LPDWORD lpcbData) {
    if (lpData && !lpcbData)
        return ERROR_INVALID_PARAMETER;

    if (lpType)
        *lpType = REG_DWORD;

    if (!lpcbData)
        return ERROR_SUCCESS;

    if (!lpData) {
        *lpcbData = sizeof(DWORD);
        return ERROR_SUCCESS;
    }

    if (*lpcbData < sizeof(DWORD)) {
        *lpcbData = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }

    memcpy(lpData, &kLiteLayoutValue, sizeof(DWORD));
    *lpcbData = sizeof(DWORD);
    return ERROR_SUCCESS;
}

static void ZeroOnFailure(DWORD dwFlags, PVOID pvData, LPDWORD pcbData) {
    if ((dwFlags & RRF_ZEROONFAILURE) && pvData && pcbData && *pcbData)
        ZeroMemory(pvData, *pcbData);
}

// RegGetValueW semantics. Invalid flag combinations are not handled here;
// the caller forwards them to the original function.
static LSTATUS EmulateGetValue(DWORD dwFlags,
                               LPDWORD pdwType,
                               PVOID pvData,
                               LPDWORD pcbData) {
    if (pvData && !pcbData)
        return ERROR_INVALID_PARAMETER;

    if (!(dwFlags & RRF_RT_REG_DWORD)) {
        ZeroOnFailure(dwFlags, pvData, pcbData);
        return ERROR_UNSUPPORTED_TYPE;
    }

    if (pdwType)
        *pdwType = REG_DWORD;

    if (!pcbData)
        return ERROR_SUCCESS;

    if (!pvData) {
        *pcbData = sizeof(DWORD);
        return ERROR_SUCCESS;
    }

    if (*pcbData < sizeof(DWORD)) {
        ZeroOnFailure(dwFlags, pvData, pcbData);
        *pcbData = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }

    memcpy(pvData, &kLiteLayoutValue, sizeof(DWORD));
    *pcbData = sizeof(DWORD);
    return ERROR_SUCCESS;
}

static bool GetValueFlagsValid(DWORD dwFlags) {
    if (!(dwFlags & RRF_RT_ANY))
        return false;
    if ((dwFlags & RRF_SUBKEY_WOW6464KEY) && (dwFlags & RRF_SUBKEY_WOW6432KEY))
        return false;
    return true;
}

static bool IsReadOnlyAccess(REGSAM sam) {
    const REGSAM writeBits = KEY_SET_VALUE | KEY_CREATE_SUB_KEY |
                             KEY_CREATE_LINK | DELETE | WRITE_DAC |
                             WRITE_OWNER | GENERIC_WRITE | GENERIC_ALL;
    return (sam & writeBits) == 0;
}

// ---------------------------------------------------------------------------
// Registry hooks
// ---------------------------------------------------------------------------

static LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey,
                                         LPCWSTR lpSubKey,
                                         DWORD ulOptions,
                                         REGSAM samDesired,
                                         PHKEY phkResult) {
    LSTATUS status = RegOpenKeyExW_Original(hKey, lpSubKey, ulOptions,
                                            samDesired, phkResult);

    // Only a missing Control Center key needs help: if it exists, the real
    // handle is returned and only the value itself is virtualized.
    if (status != ERROR_FILE_NOT_FOUND || !phkResult ||
        !g_virtualLiteLayout.load(std::memory_order_relaxed) ||
        (ulOptions & REG_OPTION_OPEN_LINK) ||
        (samDesired & KEY_WOW64_32KEY) || !IsReadOnlyAccess(samDesired) ||
        !LeafIsControlCenter(lpSubKey)) {
        return status;
    }

    if (ClassifyKey(hKey, lpSubKey) != KeyMatch::Target)
        return status;

    if (!OpenPhantomControlCenterKey(phkResult))
        return status;

    Wh_Log(L"Registry virtualization: missing Control Center key, "
           L"returned empty virtual key %p",
           *phkResult);
    return ERROR_SUCCESS;
}

static LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey,
                                            LPCWSTR lpValueName,
                                            LPDWORD lpReserved,
                                            LPDWORD lpType,
                                            LPBYTE lpData,
                                            LPDWORD lpcbData) {
    if (!lpReserved && IsLiteLayoutValue(lpValueName) &&
        g_virtualLiteLayout.load(std::memory_order_relaxed) &&
        ClassifyKey(hKey, nullptr) != KeyMatch::None) {
        Wh_Log(L"Registry virtualization: RegQueryValueExW -> UseLiteLayout=1");
        return EmulateQueryValueEx(lpType, lpData, lpcbData);
    }

    return RegQueryValueExW_Original(hKey, lpValueName, lpReserved, lpType,
                                     lpData, lpcbData);
}

static LSTATUS WINAPI RegGetValueW_Hook(HKEY hkey,
                                        LPCWSTR lpSubKey,
                                        LPCWSTR lpValue,
                                        DWORD dwFlags,
                                        LPDWORD pdwType,
                                        PVOID pvData,
                                        LPDWORD pcbData) {
    if (IsLiteLayoutValue(lpValue) &&
        g_virtualLiteLayout.load(std::memory_order_relaxed) &&
        GetValueFlagsValid(dwFlags) &&
        !(!SubKeyIsEmpty(lpSubKey) && (dwFlags & RRF_SUBKEY_WOW6432KEY)) &&
        ClassifyKey(hkey, lpSubKey) != KeyMatch::None) {
        Wh_Log(L"Registry virtualization: RegGetValueW -> UseLiteLayout=1");
        return EmulateGetValue(dwFlags, pdwType, pvData, pcbData);
    }

    return RegGetValueW_Original(hkey, lpSubKey, lpValue, dwFlags, pdwType,
                                 pvData, pcbData);
}

// Makes the virtual value visible in enumerations of the empty virtual key.
// The original runs first; NtQueryKey is only needed when an enumeration at
// index 0 reports an empty key, which is rare.
static LSTATUS WINAPI RegEnumValueW_Hook(HKEY hKey,
                                         DWORD dwIndex,
                                         LPWSTR lpValueName,
                                         LPDWORD lpcchValueName,
                                         LPDWORD lpReserved,
                                         LPDWORD lpType,
                                         LPBYTE lpData,
                                         LPDWORD lpcbData) {
    LSTATUS status =
        RegEnumValueW_Original(hKey, dwIndex, lpValueName, lpcchValueName,
                               lpReserved, lpType, lpData, lpcbData);

    if (status != ERROR_NO_MORE_ITEMS || dwIndex != 0 || lpReserved ||
        g_appHiveNtPathLen.load(std::memory_order_acquire) == 0 ||
        !g_virtualLiteLayout.load(std::memory_order_relaxed) ||
        ClassifyKey(hKey, nullptr) != KeyMatch::Phantom) {
        return status;
    }

    if (!lpValueName || !lpcchValueName)
        return ERROR_INVALID_PARAMETER;

    if (*lpcchValueName <= (DWORD)kUseLiteLayoutLen)
        return ERROR_MORE_DATA;

    if (lpData && !lpcbData)
        return ERROR_INVALID_PARAMETER;

    if (lpData && *lpcbData < sizeof(DWORD)) {
        *lpcbData = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }

    memcpy(lpValueName, kUseLiteLayout,
           (kUseLiteLayoutLen + 1) * sizeof(wchar_t));
    *lpcchValueName = kUseLiteLayoutLen;

    return EmulateQueryValueEx(lpType, lpData, lpcbData);
}

static LSTATUS WINAPI RegQueryInfoKeyW_Hook(HKEY hKey,
                                            LPWSTR lpClass,
                                            LPDWORD lpcchClass,
                                            LPDWORD lpReserved,
                                            LPDWORD lpcSubKeys,
                                            LPDWORD lpcbMaxSubKeyLen,
                                            LPDWORD lpcbMaxClassLen,
                                            LPDWORD lpcValues,
                                            LPDWORD lpcbMaxValueNameLen,
                                            LPDWORD lpcbMaxValueLen,
                                            LPDWORD lpcbSecurityDescriptor,
                                            PFILETIME lpftLastWriteTime) {
    LSTATUS status = RegQueryInfoKeyW_Original(
        hKey, lpClass, lpcchClass, lpReserved, lpcSubKeys, lpcbMaxSubKeyLen,
        lpcbMaxClassLen, lpcValues, lpcbMaxValueNameLen, lpcbMaxValueLen,
        lpcbSecurityDescriptor, lpftLastWriteTime);

    if (status != ERROR_SUCCESS || g_appHiveNtPathLen.load(std::memory_order_acquire) == 0 ||
        !(lpcValues || lpcbMaxValueNameLen || lpcbMaxValueLen) ||
        (lpcValues && *lpcValues != 0) ||
        !g_virtualLiteLayout.load(std::memory_order_relaxed) ||
        ClassifyKey(hKey, nullptr) != KeyMatch::Phantom) {
        return status;
    }

    if (lpcValues)
        *lpcValues = 1;
    if (lpcbMaxValueNameLen)
        *lpcbMaxValueNameLen = kUseLiteLayoutLen;  // in characters
    if (lpcbMaxValueLen)
        *lpcbMaxValueLen = sizeof(DWORD);

    return status;
}

// ---------------------------------------------------------------------------
// Taskbar ownership
// ---------------------------------------------------------------------------

// The taskbar window of this process, or null. FindWindowW is not enough: the
// Windows 10 shell is a private copy of explorer.exe and can have sibling
// processes started from the same folder, and only one of them owns the
// taskbar. Enumerating sends no message to any window, so it is safe to call
// from this mod's threads.
static BOOL CALLBACK FindOwnTaskbarProc(HWND hwnd, LPARAM param) {
    wchar_t className[32] = {};
    if (!GetClassNameW(hwnd, className, _countof(className)) ||
        _wcsicmp(className, L"Shell_TrayWnd") != 0) {
        return TRUE;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId())
        return TRUE;

    *reinterpret_cast<HWND*>(param) = hwnd;
    return FALSE;
}

static HWND FindOwnTaskbarWindow() {
    HWND taskbar = nullptr;
    EnumWindows(FindOwnTaskbarProc, reinterpret_cast<LPARAM>(&taskbar));
    return taskbar;
}

// Only the process that owns the taskbar may restart ShellExperienceHost.
// Windhawk injects this mod into every explorer.exe, and each of those starts
// after the ShellExperienceHost that is already running: a folder window in a
// process of its own, an `explorer.exe /factory,... -Embedding` COM server,
// `explorer.exe /select,...`, and so on. Restarting from one of them would
// kill a ShellExperienceHost that already reads the virtual UseLiteLayout=1,
// losing the open panel and any toast on screen. At shell startup there is
// nothing to restart and the taskbar window doesn't exist yet.
static bool OwnsTaskbarWindow() {
    return FindOwnTaskbarWindow() != nullptr;
}

// ---------------------------------------------------------------------------
// ShellExperienceHost restart (same approach as ExplorerPatcher)
// ---------------------------------------------------------------------------

static void RestartShellExperienceHost() {
    wchar_t winDir[MAX_PATH] = {};
    UINT n = GetSystemWindowsDirectoryW(winDir, _countof(winDir));
    if (!n || n >= _countof(winDir))
        return;

    wchar_t expected[MAX_PATH] = {};
    _snwprintf(expected, _countof(expected) - 1,
               L"%s\\SystemApps\\ShellExperienceHost_cw5n1h2txyewy\\"
               L"ShellExperienceHost.exe",
               winDir);

    DWORD session = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &session))
        return;

    ScopedHandle snap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
    if (!snap.valid())
        return;

    PROCESSENTRY32W pe = {};
    pe.dwSize = sizeof(pe);

    for (BOOL ok = Process32FirstW(snap.get(), &pe); ok;
         ok = Process32NextW(snap.get(), &pe)) {
        if (_wcsicmp(pe.szExeFile, L"ShellExperienceHost.exe") != 0)
            continue;

        DWORD procSession = 0;
        if (!ProcessIdToSessionId(pe.th32ProcessID, &procSession) ||
            procSession != session) {
            continue;
        }

        ScopedHandle p(OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, FALSE,
            pe.th32ProcessID));
        if (!p.valid())
            continue;

        wchar_t path[MAX_PATH] = {};
        DWORD cch = _countof(path);
        if (!QueryFullProcessImageNameW(p.get(), 0, path, &cch) ||
            _wcsicmp(path, expected) != 0) {
            continue;
        }

        if (TerminateProcess(p.get(), 0)) {
            Wh_Log(L"Restarted ShellExperienceHost (pid %lu)",
                   pe.th32ProcessID);
        }
    }
}

// ---------------------------------------------------------------------------
// Action Center animation
//
// How the Action Center is shown/hidden (see m417z's "Shell Flyout
// Positions" and "Action Center on Active Display" Windhawk mods):
//
// - explorer.exe (twinui.pcshell.dll) uncloaks/cloaks the CoreWindow hosted
//   by ShellExperienceHost with DwmSetWindowAttribute(DWMWA_CLOAK).
// - ShellExperienceHost positions its own CoreWindow with SetWindowPos.
// - The window is identified by the description of its thread
//   ("ActionCenter"), which is available from the very first show, unlike
//   the window geometry.
//
// Why the first show/hide wasn't animated in 1.0/1.1:
// - On the first show the CoreWindow isn't docked/sized yet, so the
//   geometry heuristic didn't identify it: neither the opening nor the
//   closing was animated.
// - ShellExperienceHost positions the window right after the first show,
//   overriding the off-screen "park" position.
// - The UI thread is busy with the cold XAML start, so asynchronous moves
//   piled up and were applied after the window was already visible/hidden.
//
// Fixes:
// - Thread-description based identification (+ geometry fallback).
// - SetWindowPos hook: positioning done by the system during a slide-in
//   only updates the animation target instead of making the window jump.
// - ShowWindow / SetWindowPos(SWP_SHOWWINDOW/SWP_HIDEWINDOW) hooks for the
//   first show, which may happen through visibility instead of cloaking.
// - Frames are only posted after the previous one has been applied, and
//   the slide-in waits until the park position has actually been applied.
// - Cross-process coordination (explorer <-> ShellExperienceHost) through
//   window properties, so only one process animates a given transition.
// - Optional WinEvent (EVENT_OBJECT_UNCLOAKED/SHOW) fallback in explorer for
//   shows that bypass all hooked APIs.
// ---------------------------------------------------------------------------

static_assert(sizeof(void*) == 8, "64-bit only (see @architecture)");

static constexpr wchar_t kPropAnimToken[] = L"WhW10AcRestorer.AnimToken";
static constexpr wchar_t kPropShowTick[] = L"WhW10AcRestorer.ShowTick";
static constexpr wchar_t kPropTargetX[] = L"WhW10AcRestorer.TargetX";
static constexpr wchar_t kPropTargetY[] = L"WhW10AcRestorer.TargetY";
// Current animated X during a slide-in (starts at the off-screen X).
static constexpr wchar_t kPropCurX[] = L"WhW10AcRestorer.CurX";

static std::atomic<HWND> g_acHwnd{nullptr};
static std::atomic<LONG> g_gen{0};
static std::atomic<bool> g_open{false};
static std::atomic<bool> g_unloading{false};

static std::atomic<int> g_animCurX{INT_MIN};

static SRWLOCK g_finalLock = SRWLOCK_INIT;
static bool g_haveFinal = false;
static int g_finalX = 0;
static int g_finalY = 0;

using SetWindowPos_t = decltype(&SetWindowPos);
static SetWindowPos_t SetWindowPos_Original = nullptr;

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Original = nullptr;

using Dwm_t = decltype(&DwmSetWindowAttribute);
static Dwm_t Dwm_Original = nullptr;

// --- small helpers ---------------------------------------------------------

static void SetFinal(int x, int y) {
    SrwExclusive lock(&g_finalLock);
    g_finalX = x;
    g_finalY = y;
    g_haveFinal = true;
}

static bool GetFinal(int* x, int* y) {
    SrwExclusive lock(&g_finalLock);
    if (!g_haveFinal)
        return false;
    *x = g_finalX;
    *y = g_finalY;
    return true;
}

static void ResetFinal() {
    SrwExclusive lock(&g_finalLock);
    g_haveFinal = false;
}

// Window properties work across processes (both processes run as the same
// user, explorer at a higher integrity level). Values are stored with bit 32
// set so that 0 can be distinguished from "missing".
static bool PropSetInt(HWND h, const wchar_t* name, int v) {
    return SetPropW(h, name, (HANDLE)(((ULONG_PTR)1 << 32) | (UINT32)v)) !=
           FALSE;
}

static bool PropGetInt(HWND h, const wchar_t* name, int* v) {
    ULONG_PTR p = (ULONG_PTR)GetPropW(h, name);
    if (!(p >> 32))
        return false;
    *v = (int)(UINT32)p;
    return true;
}

static int NewToken() {
    static std::atomic<UINT32> counter{0};
    UINT32 t = (GetCurrentProcessId() << 12) ^ (++counter);
    return t ? (int)t : 1;
}

static bool IsAnimActive(HWND h, int* tokenOut = nullptr) {
    int token = 0;
    int tick = 0;
    if (!PropGetInt(h, kPropAnimToken, &token) || token == 0 ||
        !PropGetInt(h, kPropShowTick, &tick)) {
        return false;
    }

    // Safety net: a stale token (e.g. the animating process died) expires.
    DWORD age = GetTickCount() - (DWORD)tick;
    if (age > (DWORD)g_slideInMs.load() + 3000)
        return false;

    if (tokenOut)
        *tokenOut = token;
    return true;
}

static bool RecentlyShown(HWND h, DWORD withinMs) {
    int tick = 0;
    return PropGetInt(h, kPropShowTick, &tick) &&
           GetTickCount() - (DWORD)tick < withinMs;
}

static void ClearAnimToken(HWND h, int onlyIfToken) {
    int token = 0;
    if (onlyIfToken && (!PropGetInt(h, kPropAnimToken, &token) ||
                        token != onlyIfToken)) {
        return;
    }
    PropSetInt(h, kPropAnimToken, 0);
}

static void SetTarget(HWND h, int x, int y) {
    PropSetInt(h, kPropTargetX, x);
    PropSetInt(h, kPropTargetY, y);
    SetFinal(x, y);
}

static bool GetTarget(HWND h, int* x, int* y) {
    int tx, ty;
    if (PropGetInt(h, kPropTargetX, &tx) && PropGetInt(h, kPropTargetY, &ty)) {
        *x = tx;
        *y = ty;
        return true;
    }
    return GetFinal(x, y);
}

static void SetCurX(HWND h, int x) {
    g_animCurX.store(x);
    PropSetInt(h, kPropCurX, x);
}

static bool GetCurX(HWND h, int* x) {
    if (PropGetInt(h, kPropCurX, x))
        return true;
    int local = g_animCurX.load();
    if (local == INT_MIN)
        return false;
    *x = local;
    return true;
}

static bool IsCloaked(HWND h) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked != 0;
}

static BOOL RawSetWindowPos(HWND h, int x, int y, UINT flags) {
    SetWindowPos_t fn = SetWindowPos_Original ? SetWindowPos_Original
                                              : SetWindowPos;
    return fn(h, nullptr, x, y, 0, 0,
              flags | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

static void MoveAsync(HWND h, int x, int y) {
    RawSetWindowPos(h, x, y, SWP_ASYNCWINDOWPOS);
}

// Synchronous on the window's own thread, asynchronous otherwise (never
// blocks on another, possibly busy or hung, thread).
static void MoveForAnim(HWND h, int x, int y) {
    bool ownThread = GetWindowThreadProcessId(h, nullptr) == GetCurrentThreadId();
    RawSetWindowPos(h, x, y, ownThread ? 0 : SWP_ASYNCWINDOWPOS);
}

static double Ease(double t) {
    double u = 1.0 - t;
    return 1.0 - u * u * u;
}

static ULONGLONG NowMs() {
    LARGE_INTEGER f;
    LARGE_INTEGER c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (ULONGLONG)(c.QuadPart * 1000 / f.QuadPart);
}

// Waits for the next DWM present (DwmFlush), with a minimum delay so that an
// idle compositor doesn't turn this into a busy loop.
static void FrameWait() {
    ULONGLONG t0 = NowMs();
    if (FAILED(DwmFlush()) || NowMs() - t0 < 4)
        Sleep(6);
}

static bool GetWindowMonitorInfo(HWND h, MONITORINFO* mi) {
    HMONITOR mon = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
    mi->cbSize = sizeof(*mi);
    return mon && GetMonitorInfoW(mon, mi);
}

static bool GetRectMonitorInfo(const RECT& r, MONITORINFO* mi) {
    HMONITOR mon = MonitorFromRect(&r, MONITOR_DEFAULTTONEAREST);
    mi->cbSize = sizeof(*mi);
    return mon && GetMonitorInfoW(mon, mi);
}

static UINT GetWindowDpi(HWND h) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    static GetDpiForWindow_t fn = (GetDpiForWindow_t)(void*)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
    UINT dpi = fn ? fn(h) : 0;
    return dpi ? dpi : 96;
}

// --- identification --------------------------------------------------------

static bool PlausibleActionCenterWidth(int w) {
    return w >= 250 && w <= 700;
}

static bool IsCoreWindow(HWND h) {
    if (!h)
        return false;

    wchar_t cls[64] = L"";
    if (!GetClassNameW(h, cls, _countof(cls)))
        return false;

    return wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
}

static std::atomic<DWORD> g_cachedShellPid{0};

static bool IsShellExpWindow(HWND h) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!pid)
        return false;

    if (g_processKind == ProcessKind::ShellExperienceHost)
        return pid == GetCurrentProcessId();

    if (pid == g_cachedShellPid.load(std::memory_order_relaxed))
        return true;

    ScopedHandle p(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid));
    if (!p.valid())
        return false;

    wchar_t path[MAX_PATH] = {};
    DWORD n = MAX_PATH;
    if (!QueryFullProcessImageNameW(p.get(), 0, path, &n))
        return false;

    const wchar_t* b = wcsrchr(path, L'\\');
    b = b ? b + 1 : path;

    if (_wcsicmp(b, L"ShellExperienceHost.exe") != 0)
        return false;

    g_cachedShellPid.store(pid, std::memory_order_relaxed);
    return true;
}

// GetThreadDescription: Windows 10 1607+, documented on Microsoft Learn.
static bool GetThreadDescriptionById(DWORD tid, wchar_t* out, size_t cch) {
    using GetThreadDescription_t = HRESULT(WINAPI*)(HANDLE, PWSTR*);
    static GetThreadDescription_t fn = []() {
        HMODULE kb = GetModuleHandleW(L"kernelbase.dll");
        void* p = kb ? (void*)GetProcAddress(kb, "GetThreadDescription")
                     : nullptr;
        if (!p)
            p = (void*)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),
                                      "GetThreadDescription");
        return (GetThreadDescription_t)p;
    }();

    if (!fn || !tid || !cch)
        return false;

    ScopedHandle t(OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid));
    if (!t.valid())
        return false;

    PWSTR desc = nullptr;
    if (FAILED(fn(t.get(), &desc)) || !desc)
        return false;

    wcsncpy(out, desc, cch - 1);
    out[cch - 1] = L'\0';
    LocalFree(desc);
    return out[0] != L'\0';
}

static bool LooksLikeActionCenter(const RECT& r, const MONITORINFO& mi) {
    int w = r.right - r.left;
    return r.top <= mi.rcWork.top + 2 && r.bottom >= mi.rcWork.bottom - 2 &&
           r.right >= mi.rcWork.right - 2 &&
           r.right <= mi.rcMonitor.right + 2 && PlausibleActionCenterWidth(w);
}

static bool LooksLikeStartupActionCenter(const RECT& r, const MONITORINFO& mi) {
    int w = r.right - r.left;
    int h = r.bottom - r.top;
    int workH = mi.rcWork.bottom - mi.rcWork.top;
    return PlausibleActionCenterWidth(w) && h >= workH - 20;
}

// Other ShellExperienceHost flyouts (see m417z's mods); never animate them.
static bool IsKnownOtherFlyoutThread(const wchar_t* desc) {
    static const wchar_t* const kOthers[] = {
        L"QuickActions", L"Connect", L"Cast", L"Project", L"Display",
    };
    if (!desc)
        return false;
    for (const wchar_t* o : kOthers) {
        if (_wcsicmp(desc, o) == 0)
            return true;
    }
    return false;
}

static bool IsActionCenterWindow(HWND h) {
    if (!h)
        return false;

    HWND cached = g_acHwnd.load();
    if (h == cached)
        return true;

    if (!IsCoreWindow(h) || !IsShellExpWindow(h))
        return false;

    // The cached window died (e.g. ShellExperienceHost restarted).
    if (cached && !IsWindow(cached)) {
        g_acHwnd.store(nullptr);
        ResetFinal();
        cached = nullptr;
    }

    DWORD tid = GetWindowThreadProcessId(h, nullptr);
    wchar_t desc[64] = L"";
    bool haveDesc = GetThreadDescriptionById(tid, desc, _countof(desc));

    bool match = false;
    const wchar_t* reason = L"";

    // "ActionCenter" also hosts the Windows 11 Notification Center; only
    // trust it when the Windows 10 layout is requested.
    if (haveDesc && _wcsnicmp(desc, L"ActionCenter", 12) == 0 &&
        g_virtualLiteLayout.load()) {
        match = true;
        reason = L"thread description";
    } else if (!IsKnownOtherFlyoutThread(haveDesc ? desc : nullptr)) {
        RECT r = {};
        MONITORINFO mi = {};
        if (GetWindowRect(h, &r) && GetWindowMonitorInfo(h, &mi) &&
            (LooksLikeActionCenter(r, mi) ||
             (!cached && LooksLikeStartupActionCenter(r, mi)))) {
            match = true;
            reason = L"geometry";
        }
    }

    if (!match)
        return false;

    if (h != cached) {
        g_acHwnd.store(h);
        ResetFinal();
        Wh_Log(L"%llu IDENTIFIED Action Center hwnd=%p by %s (thread '%s')",
               GetTickCount64(), h, reason, haveDesc ? desc : L"");
    }

    return true;
}

// --- worker threads --------------------------------------------------------

static SRWLOCK g_threadsLock = SRWLOCK_INIT;
static std::vector<HANDLE> g_threads;

static void TrackThread(HANDLE h) {
    SrwExclusive lock(&g_threadsLock);

    for (size_t i = 0; i < g_threads.size();) {
        if (WaitForSingleObject(g_threads[i], 0) == WAIT_OBJECT_0) {
            CloseHandle(g_threads[i]);
            g_threads[i] = g_threads.back();
            g_threads.pop_back();
        } else {
            i++;
        }
    }

    g_threads.push_back(h);
}

static void WaitForAllThreads() {
    std::vector<HANDLE> threads;
    {
        SrwExclusive lock(&g_threadsLock);
        threads.swap(g_threads);
    }

    for (HANDLE h : threads) {
        WaitForSingleObject(h, 5000);
        CloseHandle(h);
    }
}

// --- animation core --------------------------------------------------------

static bool Aborted(HWND h, LONG gen, int token) {
    if (gen != g_gen.load(std::memory_order_relaxed) ||
        g_unloading.load(std::memory_order_relaxed) || !IsWindow(h)) {
        return true;
    }

    if (token) {
        int current = 0;
        if (!PropGetInt(h, kPropAnimToken, &current) || current != token)
            return true;
    }

    return false;
}

// Moves the window horizontally from fromX to the target. A new frame is
// only posted once the previous one has been applied by the window's thread
// (or after 50 ms), so moves never pile up behind a busy UI thread.
// Returns false if aborted.
static bool SlideFrames(HWND h,
                        int fromX,
                        bool targetFromProps,
                        int toX,
                        int y,
                        int ms,
                        LONG gen,
                        int token,
                        DWORD hardTimeoutMs) {
    ULONGLONG t0 = NowMs();
    int lastX = INT_MIN;
    ULONGLONG lastPost = 0;

    for (;;) {
        if (Aborted(h, gen, token))
            return false;

        ULONGLONG now = NowMs();
        if (now - t0 > hardTimeoutMs)
            return true;

        if (lastX != INT_MIN && now - lastPost < 50) {
            RECT r = {};
            if (GetWindowRect(h, &r) && r.left != lastX) {
                Sleep(1);
                continue;
            }
        }

        if (targetFromProps) {
            int tx, ty;
            if (GetTarget(h, &tx, &ty)) {
                toX = tx;
                y = ty;
            }
        }

        double t = ms <= 0 ? 1.0
                           : ((now - t0) >= (ULONGLONG)ms
                                  ? 1.0
                                  : (double)(now - t0) / (double)ms);

        int x = fromX + (int)((double)(toX - fromX) * Ease(t));
        if (targetFromProps)
            SetCurX(h, x);
        MoveForAnim(h, x, y);
        lastX = x;
        lastPost = now;

        if (t >= 1.0)
            return true;

        FrameWait();
    }
}

struct SlideIn {
    HWND h;
    int offX;
    int ms;
    LONG gen;
    int token;
};

static DWORD WINAPI SlideInThread(LPVOID p) {
    std::unique_ptr<SlideIn> s(static_cast<SlideIn*>(p));
    if (!s)
        return 0;

    HWND h = s->h;

    // Wait until the park position has really been applied. On the first
    // show the UI thread can be busy for a while (cold XAML start).
    ULONGLONG t0 = NowMs();
    bool parked = false;
    while (!Aborted(h, s->gen, s->token)) {
        RECT r = {};
        if (GetWindowRect(h, &r) && r.left >= s->offX - 4) {
            parked = true;
            break;
        }
        if (NowMs() - t0 > 1500)
            break;
        Sleep(4);
    }

    bool completed = false;
    if (parked) {
        completed = SlideFrames(h, s->offX, true, 0, 0, s->ms, s->gen,
                                s->token, (DWORD)s->ms + 1500);
    }

    // Unless a hide took over (gen/token changed while still running
    // normally), make sure the panel ends exactly at its final position.
    bool hideTookOver = !completed && parked &&
                        !g_unloading.load(std::memory_order_relaxed) &&
                        Aborted(h, s->gen, s->token);
    if (!hideTookOver && IsWindow(h)) {
        int tx, ty;
        if (GetTarget(h, &tx, &ty))
            MoveAsync(h, tx, ty);
    }

    ClearAnimToken(h, s->token);
    g_animCurX.store(INT_MIN);
    RemovePropW(h, kPropCurX);

    Wh_Log(L"%llu SLIDE-IN %s", GetTickCount64(),
           completed ? L"done" : (parked ? L"aborted" : L"park timeout"));
    return 0;
}

// Computes the final (docked) position. hint: explicit position requested by
// the caller (SetWindowPos with SWP_SHOWWINDOW), if any.
static void ComputeFinal(HWND h,
                         const POINT* hint,
                         int* finalX,
                         int* finalY,
                         int* offX) {
    RECT r = {};
    GetWindowRect(h, &r);
    int w = r.right - r.left;

    if (hint) {
        RECT hr = {hint->x, hint->y, hint->x + w, hint->y + (r.bottom - r.top)};
        MONITORINFO mi = {};
        GetRectMonitorInfo(hr, &mi);
        *finalX = hint->x;
        *finalY = hint->y;
        *offX = mi.rcMonitor.right;
        return;
    }

    MONITORINFO mi = {};
    GetWindowMonitorInfo(h, &mi);
    *offX = mi.rcMonitor.right;

    bool docked = PlausibleActionCenterWidth(w) &&
                  r.left < mi.rcMonitor.right - 4 &&
                  r.right >= mi.rcWork.right - 8 &&
                  r.right <= mi.rcMonitor.right + 8;
    if (docked) {
        *finalX = r.left;
        *finalY = r.top;
        return;
    }

    if (GetFinal(finalX, finalY))
        return;

    int fbW = PlausibleActionCenterWidth(w)
                  ? w
                  : MulDiv(396, (int)GetWindowDpi(h), 96);
    *finalX = mi.rcWork.right - fbW;
    *finalY = mi.rcWork.top;
}

// Per-process timestamp of the last show handled by a hook (used together
// with the window property, in case properties can't be set).
static std::atomic<DWORD> g_lastHookShowTick{0};

// Marks the start of a show animation (visible to both processes). Returns
// the animation token, or 0 if the window properties couldn't be set (the
// animation then relies on the per-process generation counter only).
static int BeginShowState(HWND h, int finalX, int finalY, int offX) {
    int token = NewToken();
    SetTarget(h, finalX, finalY);
    SetCurX(h, offX);
    g_lastHookShowTick.store(GetTickCount() | 1);
    g_open.store(true);

    int check = 0;
    if (!PropSetInt(h, kPropShowTick, (int)GetTickCount()) ||
        !PropSetInt(h, kPropAnimToken, token) ||
        !PropGetInt(h, kPropAnimToken, &check) || check != token) {
        Wh_Log(L"Window properties unavailable, animating without "
               L"cross-process coordination");
        return 0;
    }

    return token;
}

static void StartSlideInThread(HWND h, int offX, int token) {
    LONG gen = g_gen.load();
    auto s = std::make_unique<SlideIn>(
        SlideIn{h, offX, g_slideInMs.load(), gen, token});

    HANDLE th = CreateThread(nullptr, 0, SlideInThread, s.get(), 0, nullptr);
    if (th) {
        // Ownership is transferred to the thread; the raw pointer returned
        // by release() is intentionally not used.
        [[maybe_unused]] auto* released = s.release();
        TrackThread(th);
        return;
    }

    // No thread: just make sure the panel is visible where it belongs.
    int tx, ty;
    if (GetTarget(h, &tx, &ty))
        MoveAsync(h, tx, ty);
    ClearAnimToken(h, token);
}
// Parks the window off-screen. On another thread the move is asynchronous;
// wait up to waitMs for it to be applied so that the uncloak doesn't flash
// the panel at its final position.
static void Park(HWND h, int offX, int y, DWORD waitMs) {
    // Synchronous, also across processes: explorer itself positions this
    // window with synchronous SetWindowPos calls right after the uncloak, so
    // this doesn't add a new kind of wait. On the first show the UI thread
    // of ShellExperienceHost is busy and an asynchronous move would only be
    // applied after the panel is already visible (on the left, where the
    // not yet positioned window lives). Hung windows get an async move.
    if (IsHungAppWindow(h))
        MoveForAnim(h, offX, y);
    else
        RawSetWindowPos(h, offX, y, 0);

    ULONGLONG t0 = NowMs();
    for (;;) {
        RECT r = {};
        if (GetWindowRect(h, &r) && r.left >= offX - 4)
            return;
        if (NowMs() - t0 >= waitMs)
            return;
        Sleep(2);
    }
}

// Slide-out, blocking (bounded). Must be called before the hide happens.
static void RunHideAnimation(HWND h) {
    g_open.store(false);
    LONG gen = g_gen.fetch_add(1) + 1;

    // Abort a slide-in running in any process, and remember where the panel
    // belongs so it can be restored while hidden.
    bool wasAnimating = IsAnimActive(h);
    ClearAnimToken(h, 0);

    RECT r = {};
    MONITORINFO mi = {};
    if (!GetWindowRect(h, &r) || !GetWindowMonitorInfo(h, &mi))
        return;

    if (!wasAnimating && PlausibleActionCenterWidth(r.right - r.left) &&
        r.left < mi.rcMonitor.right - 4 && r.right >= mi.rcWork.right - 8) {
        SetTarget(h, r.left, r.top);
    }

    int ms = g_slideOutMs.load();
    if (ms <= 0 || r.left >= mi.rcMonitor.right)
        return;

    SlideFrames(h, r.left, false, mi.rcMonitor.right, r.top, ms, gen, 0,
                (DWORD)ms + 250);

    // Give the last frame a moment to be applied before the hide.
    ULONGLONG t0 = NowMs();
    while (NowMs() - t0 < 40) {
        RECT rr = {};
        if (!GetWindowRect(h, &rr) || rr.left >= mi.rcMonitor.right - 4)
            break;
        Sleep(2);
    }
}

static void RestoreFinalWhileHidden(HWND h) {
    int x, y;
    if (GetTarget(h, &x, &y))
        MoveAsync(h, x, y);
}

static bool AnimationsWanted(bool show) {
    return !g_unloading.load() &&
           (show ? g_slideInMs.load() > 0 : g_slideOutMs.load() > 0);
}

// --- DWM cloak hook (explorer uncloaks/cloaks the SEH window) --------------

static HRESULT WINAPI Dwm_Hook(HWND h, DWORD attr, LPCVOID v, DWORD cb) {
    if (attr != DWMWA_CLOAK || !v || cb < sizeof(BOOL) ||
        g_unloading.load(std::memory_order_relaxed) ||
        !IsActionCenterWindow(h)) {
        return Dwm_Original(h, attr, v, cb);
    }

    BOOL cloak = *(const BOOL*)v;
    bool visible = IsWindowVisible(h) != FALSE;
    bool cloaked = IsCloaked(h);

    if (!cloak) {
        // Show transition only if this call is what makes it visible.
        if (!visible || !cloaked) {
            if (visible)
                g_open.store(true);
            return Dwm_Original(h, attr, v, cb);
        }

        g_open.store(true);
        g_gen.fetch_add(1);

        if (!AnimationsWanted(true) || IsAnimActive(h))
            return Dwm_Original(h, attr, v, cb);

        int fx, fy, offX;
        ComputeFinal(h, nullptr, &fx, &fy, &offX);
        int token = BeginShowState(h, fx, fy, offX);

        Park(h, offX, fy, 120);
        HRESULT hr = Dwm_Original(h, attr, v, cb);
        StartSlideInThread(h, offX, token);

        Wh_Log(L"%llu SHOW (uncloak) final=%d,%d", GetTickCount64(), fx, fy);
        return hr;
    }

    // Hide transition only if the panel is currently visible.
    if (!visible || cloaked) {
        g_open.store(false);
        return Dwm_Original(h, attr, v, cb);
    }

    if (AnimationsWanted(false))
        RunHideAnimation(h);
    else {
        g_open.store(false);
        g_gen.fetch_add(1);
        ClearAnimToken(h, 0);
    }

    HRESULT hr = Dwm_Original(h, attr, v, cb);
    RestoreFinalWhileHidden(h);

    Wh_Log(L"%llu HIDE (cloak)", GetTickCount64());
    return hr;
}

// --- ShowWindow / SetWindowPos hooks ---------------------------------------

static bool IsShowCommand(int cmd) {
    switch (cmd) {
        case SW_SHOWNORMAL:
        case SW_SHOW:
        case SW_SHOWNA:
        case SW_SHOWNOACTIVATE:
        case SW_RESTORE:
        case SW_SHOWDEFAULT:
            return true;
    }
    return false;
}

static BOOL WINAPI ShowWindow_Hook(HWND h, int cmd) {
    bool isShow = IsShowCommand(cmd);
    bool isHide = cmd == SW_HIDE;

    if ((!isShow && !isHide) || g_unloading.load(std::memory_order_relaxed) ||
        !IsActionCenterWindow(h)) {
        return ShowWindow_Original(h, cmd);
    }

    bool visible = IsWindowVisible(h) != FALSE;
    bool cloaked = IsCloaked(h);

    if (isShow) {
        // While cloaked, the uncloak (explorer) is the real show.
        if (visible || cloaked || !AnimationsWanted(true) || IsAnimActive(h))
            return ShowWindow_Original(h, cmd);

        g_gen.fetch_add(1);
        int fx, fy, offX;
        ComputeFinal(h, nullptr, &fx, &fy, &offX);
        int token = BeginShowState(h, fx, fy, offX);

        Park(h, offX, fy, 120);
        BOOL ret = ShowWindow_Original(h, cmd);
        StartSlideInThread(h, offX, token);

        Wh_Log(L"%llu SHOW (ShowWindow) final=%d,%d", GetTickCount64(), fx,
               fy);
        return ret;
    }

    if (!visible || cloaked)
        return ShowWindow_Original(h, cmd);

    if (AnimationsWanted(false))
        RunHideAnimation(h);
    else {
        g_open.store(false);
        g_gen.fetch_add(1);
        ClearAnimToken(h, 0);
    }

    BOOL ret = ShowWindow_Original(h, cmd);
    RestoreFinalWhileHidden(h);

    Wh_Log(L"%llu HIDE (ShowWindow)", GetTickCount64());
    return ret;
}

static BOOL WINAPI SetWindowPos_Hook(HWND h,
                                     HWND after,
                                     int X,
                                     int Y,
                                     int cx,
                                     int cy,
                                     UINT flags) {
    // Fast path: SetWindowPos is called very often.
    bool showHide = (flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW)) != 0;
    if (!h || (h != g_acHwnd.load(std::memory_order_relaxed) && !showHide) ||
        g_unloading.load(std::memory_order_relaxed) ||
        !IsActionCenterWindow(h)) {
        return SetWindowPos_Original(h, after, X, Y, cx, cy, flags);
    }

    bool visible = IsWindowVisible(h) != FALSE;
    bool cloaked = IsCloaked(h);
    bool moves = !(flags & SWP_NOMOVE);

    // Show through SWP_SHOWWINDOW.
    if ((flags & SWP_SHOWWINDOW) && !visible && !cloaked &&
        AnimationsWanted(true) && !IsAnimActive(h)) {
        g_gen.fetch_add(1);

        POINT hint = {X, Y};
        int fx, fy, offX;
        ComputeFinal(h, moves ? &hint : nullptr, &fx, &fy, &offX);
        int token = BeginShowState(h, fx, fy, offX);

        if (!moves)
            Park(h, offX, fy, 120);

        BOOL ret = SetWindowPos_Original(h, after, moves ? offX : X, Y, cx, cy,
                                         flags);
        StartSlideInThread(h, offX, token);

        Wh_Log(L"%llu SHOW (SetWindowPos) final=%d,%d", GetTickCount64(), fx,
               fy);
        return ret;
    }

    // Hide through SWP_HIDEWINDOW.
    if ((flags & SWP_HIDEWINDOW) && visible && !cloaked) {
        if (AnimationsWanted(false))
            RunHideAnimation(h);
        else {
            g_open.store(false);
            g_gen.fetch_add(1);
            ClearAnimToken(h, 0);
        }

        BOOL ret = SetWindowPos_Original(h, after, X, Y, cx, cy, flags);
        RestoreFinalWhileHidden(h);
        Wh_Log(L"%llu HIDE (SetWindowPos)", GetTickCount64());
        return ret;
    }

    // The system positions the panel while a slide-in is running (this is
    // what used to break the first show): take the requested position as
    // the new animation target and keep the current animated X.
    if (moves && IsAnimActive(h)) {
        RECT r = {};
        if (GetWindowRect(h, &r)) {
            int w = (flags & SWP_NOSIZE) ? (r.right - r.left) : cx;
            RECT req = {X, Y, X + w, Y + (r.bottom - r.top)};
            MONITORINFO mi = {};
            GetRectMonitorInfo(req, &mi);

            // Explorer first parks the panel at the monitor's right edge,
            // then moves it to its docked position. Only an on-screen
            // position is a valid animation target.
            bool onScreen = PlausibleActionCenterWidth(w) &&
                            X < mi.rcMonitor.right - 4 &&
                            X + w > mi.rcMonitor.left;

            int tx = X, ty = Y;
            if (onScreen)
                SetTarget(h, X, Y);
            else
                GetTarget(h, &tx, &ty);

            // Keep the window where the animation currently has it (the
            // off-screen start before the first frame), never at its old,
            // not yet positioned, location.
            int curX;
            if (!GetCurX(h, &curX))
                curX = mi.rcMonitor.right;

            Wh_Log(L"%llu Repositioning during slide-in: requested %d,%d -> "
                   L"%s, kept at x=%d",
                   GetTickCount64(), X, Y,
                   onScreen ? L"new target" : L"ignored (off-screen)", curX);

            X = curX;
            Y = ty;
        }
    }

    return SetWindowPos_Original(h, after, X, Y, cx, cy, flags);
}

// --- WinEvent fallback (explorer only) -------------------------------------

static HANDLE g_winEventThread = nullptr;
static DWORD g_winEventThreadId = 0;

static void LateShowAnimation(HWND h) {
    RECT r = {};
    MONITORINFO mi = {};
    if (!GetWindowRect(h, &r) || !GetWindowMonitorInfo(h, &mi) ||
        r.left >= mi.rcMonitor.right - 4) {
        return;
    }

    g_gen.fetch_add(1);
    int fx, fy, offX;
    ComputeFinal(h, nullptr, &fx, &fy, &offX);
    int token = BeginShowState(h, fx, fy, offX);

    MoveAsync(h, offX, fy);
    StartSlideInThread(h, offX, token);

    Wh_Log(L"%llu SHOW (late, WinEvent fallback) final=%d,%d",
           GetTickCount64(), fx, fy);
}

static void CALLBACK WinEventProc(HWINEVENTHOOK,
                                  DWORD event,
                                  HWND h,
                                  LONG idObject,
                                  LONG idChild,
                                  DWORD,
                                  DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !h ||
        g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    if (h != g_acHwnd.load() && (!IsCoreWindow(h) || !IsActionCenterWindow(h)))
        return;

    bool shown = IsWindowVisible(h) && !IsCloaked(h);

    if (event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_UNCLOAKED) {
        if (!shown)
            return;

        g_open.store(true);

        // Already handled synchronously by a hook (in either process)?
        DWORD lastLocal = g_lastHookShowTick.load();
        if (IsAnimActive(h) || RecentlyShown(h, 1000) ||
            (lastLocal && GetTickCount() - lastLocal < 1000)) {
            return;
        }

        if (g_lateShowFallback.load() && AnimationsWanted(true))
            LateShowAnimation(h);
    } else if (!shown) {
        g_open.store(false);
    }
}

static DWORD WINAPI WinEventThread(LPVOID readyEvent) {
    MSG msg;
    // Create the message queue before signaling readiness.
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent((HANDLE)readyEvent);

    HWINEVENTHOOK h1 = SetWinEventHook(
        EVENT_OBJECT_SHOW, EVENT_OBJECT_HIDE, nullptr, WinEventProc, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    HWINEVENTHOOK h2 = SetWinEventHook(
        EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr, WinEventProc, 0,
        0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (h1)
        UnhookWinEvent(h1);
    if (h2)
        UnhookWinEvent(h2);
    return 0;
}

static void StartWinEventThread() {
    ScopedHandle ready(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!ready.valid())
        return;

    g_winEventThread = CreateThread(nullptr, 0, WinEventThread, ready.get(), 0,
                                    &g_winEventThreadId);
    if (g_winEventThread)
        WaitForSingleObject(ready.get(), 2000);
}

static void StopWinEventThread() {
    if (!g_winEventThread)
        return;

    PostThreadMessageW(g_winEventThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_winEventThread, 5000);
    CloseHandle(g_winEventThread);
    g_winEventThread = nullptr;
    g_winEventThreadId = 0;
}

// ---------------------------------------------------------------------------
// Close timer (explorer.exe / twinui.pcshell.dll only)
// ---------------------------------------------------------------------------

__attribute__((noinline)) static bool TwinuiInStack() {
    static HMODULE s_twinui = nullptr;
    if (!s_twinui)
        s_twinui = GetModuleHandleW(L"twinui.pcshell.dll");
    if (!s_twinui)
        return false;

    void* fr[12] = {};
    USHORT c = CaptureStackBackTrace(1, 12, fr, nullptr);

    for (USHORT i = 0; i < c; i++) {
        HMODULE m = nullptr;
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                   GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                               (LPCWSTR)fr[i], &m) &&
            m == s_twinui) {
            return true;
        }
    }

    return false;
}

static bool IsActionCenterShownNow() {
    HWND h = g_acHwnd.load();
    return h && IsWindow(h) && IsWindowVisible(h) && !IsCloaked(h);
}

static bool MatchTimer(const FILETIME* due) {
    if (!due)
        return false;

    ULARGE_INTEGER u;
    u.LowPart = due->dwLowDateTime;
    u.HighPart = due->dwHighDateTime;
    LONGLONG v = (LONGLONG)u.QuadPart;

    // Only relative due times (negative values) of ~2 s.
    if (v >= 0)
        return false;

    LONGLONG ms = (-v) / 10000;
    if (ms < 1900 || ms > 2100)
        return false;

    // The panel may have been shown through a path that explorer doesn't
    // see synchronously (e.g. on the first show), so also check its state.
    if (!g_open.load(std::memory_order_relaxed) && !IsActionCenterShownNow())
        return false;

    return TwinuiInStack();
}

static FILETIME MakeDue(int ms) {
    LARGE_INTEGER li;
    li.QuadPart = -(LONGLONG)ms * 10000;

    FILETIME ft;
    ft.dwLowDateTime = li.LowPart;
    ft.dwHighDateTime = (DWORD)li.HighPart;
    return ft;
}

using TpTimer_t = decltype(&SetThreadpoolTimer);
static TpTimer_t TpTimer_Original = nullptr;

static VOID WINAPI TpTimer_Hook(PTP_TIMER t,
                                PFILETIME due,
                                DWORD period,
                                DWORD window) {
    if (MatchTimer(due)) {
        int delayMs = g_delayMs.load(std::memory_order_relaxed);
        FILETIME ft = MakeDue(delayMs);
        Wh_Log(L"%llu PATCHED SetThreadpoolTimer 2000 -> %d ms",
               GetTickCount64(), delayMs);
        TpTimer_Original(t, &ft, period, window);
        return;
    }

    TpTimer_Original(t, due, period, window);
}

using TpTimerEx_t = decltype(&SetThreadpoolTimerEx);
static TpTimerEx_t TpTimerEx_Original = nullptr;

static BOOL WINAPI TpTimerEx_Hook(PTP_TIMER t,
                                  PFILETIME due,
                                  DWORD period,
                                  DWORD window) {
    if (MatchTimer(due)) {
        int delayMs = g_delayMs.load(std::memory_order_relaxed);
        FILETIME ft = MakeDue(delayMs);
        Wh_Log(L"%llu PATCHED SetThreadpoolTimerEx 2000 -> %d ms",
               GetTickCount64(), delayMs);
        return TpTimerEx_Original(t, &ft, period, window);
    }

    return TpTimerEx_Original(t, due, period, window);
}

// ---------------------------------------------------------------------------
// Hook helpers
// ---------------------------------------------------------------------------

static void* GetRegistryFunction(HMODULE kernelBase,
                                 HMODULE advapi,
                                 const char* name) {
    void* p = kernelBase ? (void*)GetProcAddress(kernelBase, name) : nullptr;
    if (!p && advapi)
        p = (void*)GetProcAddress(advapi, name);
    return p;
}

template <typename T>
static bool HookFunction(void* target, T* hook, T** original, const wchar_t* name) {
    if (!target) {
        Wh_Log(L"Hook target not found: %s", name);
        return false;
    }

    if (!Wh_SetFunctionHook(target, (void*)hook, (void**)original)) {
        Wh_Log(L"Failed to hook %s", name);
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    g_processKind = DetectProcessKind();

    Wh_Log(L"%llu Windows 10 Action Center Restorer loaded in process kind %d "
           L"delay=%d in=%d out=%d virtualLite=%d restartSeh=%d lateShow=%d",
           GetTickCount64(), (int)g_processKind, g_delayMs.load(),
           g_slideInMs.load(), g_slideOutMs.load(),
           (int)g_virtualLiteLayout.load(), (int)g_restartSeh.load(),
           (int)g_lateShowFallback.load());

    if (g_processKind == ProcessKind::Other)
        return FALSE;

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll)
        g_NtQueryKey = (NtQueryKey_t)(void*)GetProcAddress(ntdll, "NtQueryKey");

    // -----------------------------------------------------------------------
    // Registry virtualization (all target processes).
    // -----------------------------------------------------------------------

    if (g_NtQueryKey) {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        HMODULE advapi = LoadLibraryW(L"advapi32.dll");

        HookFunction(GetRegistryFunction(kernelBase, advapi, "RegOpenKeyExW"),
                     RegOpenKeyExW_Hook, &RegOpenKeyExW_Original,
                     L"RegOpenKeyExW");
        HookFunction(GetRegistryFunction(kernelBase, advapi, "RegQueryValueExW"),
                     RegQueryValueExW_Hook, &RegQueryValueExW_Original,
                     L"RegQueryValueExW");
        HookFunction(GetRegistryFunction(kernelBase, advapi, "RegGetValueW"),
                     RegGetValueW_Hook, &RegGetValueW_Original,
                     L"RegGetValueW");
        HookFunction(GetRegistryFunction(kernelBase, advapi, "RegEnumValueW"),
                     RegEnumValueW_Hook, &RegEnumValueW_Original,
                     L"RegEnumValueW");
        HookFunction(GetRegistryFunction(kernelBase, advapi, "RegQueryInfoKeyW"),
                     RegQueryInfoKeyW_Hook, &RegQueryInfoKeyW_Original,
                     L"RegQueryInfoKeyW");
    } else {
        Wh_Log(L"NtQueryKey not found, registry virtualization disabled");
    }

    // ShellHost.exe: only the registry value is needed there.
    if (g_processKind == ProcessKind::ShellHost)
        return TRUE;

    // -----------------------------------------------------------------------
    // Action Center animation hooks (explorer.exe + ShellExperienceHost.exe)
    // -----------------------------------------------------------------------

    HMODULE dwmapi =
        LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    void* pDwm = dwmapi ? (void*)GetProcAddress(dwmapi, "DwmSetWindowAttribute")
                        : nullptr;
    HookFunction(pDwm ? pDwm : (void*)DwmSetWindowAttribute, Dwm_Hook,
                 &Dwm_Original, L"DwmSetWindowAttribute");

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    HookFunction(user32 ? (void*)GetProcAddress(user32, "SetWindowPos")
                        : (void*)SetWindowPos,
                 SetWindowPos_Hook, &SetWindowPos_Original, L"SetWindowPos");
    HookFunction(user32 ? (void*)GetProcAddress(user32, "ShowWindow")
                        : (void*)ShowWindow,
                 ShowWindow_Hook, &ShowWindow_Original, L"ShowWindow");

    // The close timer lives in twinui.pcshell.dll, loaded only in explorer.
    if (g_processKind == ProcessKind::Explorer) {
        HookFunction((void*)SetThreadpoolTimer, TpTimer_Hook,
                     &TpTimer_Original, L"SetThreadpoolTimer");
        HookFunction((void*)SetThreadpoolTimerEx, TpTimerEx_Hook,
                     &TpTimerEx_Original, L"SetThreadpoolTimerEx");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_processKind != ProcessKind::Explorer)
        return;

    StartWinEventThread();

    // Injected into a running shell: the taskbar already exists in this
    // process, so the mod was enabled while the shell was up, and the
    // ShellExperienceHost that is running has read the real (missing/0)
    // UseLiteLayout value. At shell startup there is nothing to restart and
    // no taskbar window yet, and the other explorer.exe instances of this
    // session don't own the taskbar.
    if (g_restartSeh.load() && g_virtualLiteLayout.load() &&
        OwnsTaskbarWindow()) {
        RestartShellExperienceHost();
    }
}

// ---------------------------------------------------------------------------
// Settings changed
// ---------------------------------------------------------------------------

void Wh_ModSettingsChanged() {
    bool prevVirtual = g_virtualLiteLayout.load();

    LoadSettings();

    Wh_Log(L"Settings changed: delay=%d in=%d out=%d virtualLite=%d "
           L"restartSeh=%d lateShow=%d",
           g_delayMs.load(), g_slideInMs.load(), g_slideOutMs.load(),
           (int)g_virtualLiteLayout.load(), (int)g_restartSeh.load(),
           (int)g_lateShowFallback.load());

    if (prevVirtual != g_virtualLiteLayout.load()) {
        // Re-identify the window with the new setting.
        g_acHwnd.store(nullptr);
        ResetFinal();

        if (g_processKind == ProcessKind::Explorer) {
            // Only the taskbar owner restarts ShellExperienceHost: the other
            // explorer.exe instances must not kill the running one.
            if (g_restartSeh.load() && OwnsTaskbarWindow())
                RestartShellExperienceHost();
            Wh_Log(L"UseLiteLayout virtualization toggled; restart Explorer "
                   L"if the tray button still opens the previous panel");
        }
    }
}

// ---------------------------------------------------------------------------
// Uninitialization
// ---------------------------------------------------------------------------

void Wh_ModBeforeUninit() {
    // Stop running animations before hooks are removed.
    g_unloading.store(true);
    g_gen.fetch_add(1);

    if (g_processKind == ProcessKind::Explorer)
        StopWinEventThread();
}

void Wh_ModUninit() {
    // Hooks are already removed here. Make sure no worker thread is still
    // running mod code before the DLL is unloaded.
    WaitForAllThreads();

    HWND h = g_acHwnd.load();
    if (h && IsWindow(h)) {
        int x, y;
        if (GetTarget(h, &x, &y))
            MoveAsync(h, x, y);

        PropSetInt(h, kPropAnimToken, 0);
        RemovePropW(h, kPropAnimToken);
        RemovePropW(h, kPropShowTick);
        RemovePropW(h, kPropTargetX);
        RemovePropW(h, kPropTargetY);
        RemovePropW(h, kPropCurX);
    }

    // Handles already given to callers stay valid (real empty key); after
    // unload they simply no longer report UseLiteLayout, like the real
    // registry. Our own root handle can be closed.
    {
        SrwExclusive lock(&g_appHiveLock);
        if (g_appHiveRoot) {
            RegCloseKey(g_appHiveRoot);
            g_appHiveRoot = nullptr;
        }
    }

    // Bring the Windows 11 notification center back without a reboot. Only
    // the process that owns the taskbar does it, so the other explorer.exe
    // instances of the session don't kill the ShellExperienceHost in use.
    if (g_processKind == ProcessKind::Explorer && g_restartSeh.load() &&
        g_virtualLiteLayout.load() && OwnsTaskbarWindow()) {
        RestartShellExperienceHost();
    }

    Wh_Log(L"Windows 10 Action Center Restorer unloaded. "
           L"No registry changes were made.");
}
