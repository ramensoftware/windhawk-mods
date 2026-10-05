// ==WindhawkMod==
// @id              spoof-machine-guid
// @name            Spoof MachineGuid
// @name:zh-CN      伪造 MachineGuid
// @description     Replaces the MachineGuid returned by registry queries, for the target process only
// @description:zh-CN 替换目标进程查询注册表时返回的 MachineGuid，系统真实值不受影响
// @version         1.0
// @author          loliri
// @github          https://github.com/loliri
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Spoof MachineGuid

Replaces the `MachineGuid` value that registry queries return, for the target
process only. The real value on the system is left untouched, and other
processes keep seeing it.

## Intended use

`MachineGuid`, stored at
`HKLM\SOFTWARE\Microsoft\Cryptography\MachineGuid`, is a per-installation
identifier that is assigned when Windows is installed. Programs commonly read
it to recognise a particular machine: license activation and trial checks,
telemetry and device fingerprinting, and some anti-cheat and ban systems.

This mod is for making a single program see a different value, so it stops
recognising the machine. Typical reasons are testing software that keys off this
value, or keeping one specific application from correlating the machine across
sessions.

Note that `MachineGuid` is only one of several identifiers a program can use —
the SMBIOS UUID, volume serial numbers, MAC addresses and the Windows product ID
are others. Changing this one value will not make every program see a different
machine.

## Choosing the target process

The mod targets nothing by default, so it does nothing until you tell it which
process to apply to. Open the mod in Windhawk, go to the **Advanced** tab, and
put the executable name of your target in the **Custom process inclusion list**.
The change applies as soon as you save, and takes effect the next time the
program starts.

## Settings

- **Fake MachineGuid**: the GUID to return in place of the real one. Both the
  plain `11111111-2222-3333-4444-555555555555` form and the braced
  `{11111111-2222-3333-4444-555555555555}` form are accepted. Any other value
  leaves the mod inactive, and a note is written to the log.

## How it works

The mod hooks `NtQueryValueKey` in ntdll, which every Win32 registry read goes
through, so `RegQueryValueExA/W` and `RegGetValueA/W` are all covered. Only a
`REG_SZ` value named `MachineGuid` in the
`\REGISTRY\MACHINE\SOFTWARE\Microsoft\Cryptography` key is replaced.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fakeGuid: "11111111-2222-3333-4444-555555555555"
  $name: Fake MachineGuid
  $name:zh-CN: 伪造的 MachineGuid
  $description: >-
    The GUID to return in place of the real one. Braces around it are optional.
  $description:zh-CN: 填入你想伪造的 GUID，两侧的花括号可有可无
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <winternl.h>

#include <string>
#include <vector>

// ============================================================
//  Native API types
// ============================================================

typedef struct _KEY_NAME_INFORMATION {
    ULONG NameLength;
    WCHAR Name[1];
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

typedef enum _KEY_INFORMATION_CLASS_LOCAL {
    KeyNameInformationLocal = 3
} KEY_INFORMATION_CLASS_LOCAL;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION_LOCAL {
    ULONG TitleIndex;
    ULONG Type;
    ULONG DataLength;
    UCHAR Data[1];
} KEY_VALUE_PARTIAL_INFORMATION_LOCAL, *PKEY_VALUE_PARTIAL_INFORMATION_LOCAL;

typedef enum _KEY_VALUE_INFORMATION_CLASS_LOCAL {
    KeyValuePartialInformationLocal = 2,
    KeyValuePartialInformationAlign64Local = 4
} KEY_VALUE_INFORMATION_CLASS_LOCAL;

typedef NTSTATUS(NTAPI* NtQueryKey_t)(HANDLE, KEY_INFORMATION_CLASS_LOCAL,
                                      PVOID, ULONG, PULONG);

typedef NTSTATUS(NTAPI* NtQueryValueKey_t)(
    HANDLE, PUNICODE_STRING, KEY_VALUE_INFORMATION_CLASS_LOCAL, PVOID, ULONG,
    PULONG);

NtQueryKey_t NtQueryKey_Func = nullptr;
NtQueryValueKey_t NtQueryValueKey_Original = nullptr;

// The real MachineGuid is 36 characters plus a terminating null. The fake value
// is validated to the same length, so the two are always interchangeable and
// the size-query paths need no special handling. A fixed-size buffer also means
// the hook never reads freed memory when the setting changes.
WCHAR g_fakeGuid[37] = {};

// ============================================================
//  Helpers
// ============================================================

// The full registry path of an open key, via NtQueryKey.
bool GetKeyPath(HANDLE hKey, std::wstring& path) {
    ULONG size = 0;
    NtQueryKey_Func(hKey, KeyNameInformationLocal, nullptr, 0, &size);
    if (size == 0) {
        return false;
    }

    std::vector<BYTE> buffer(size);
    ULONG resultSize = 0;
    NTSTATUS status = NtQueryKey_Func(hKey, KeyNameInformationLocal,
                                      buffer.data(), size, &resultSize);
    if (!NT_SUCCESS(status)) {
        return false;
    }

    auto* info = reinterpret_cast<PKEY_NAME_INFORMATION>(buffer.data());
    path.assign(info->Name, info->NameLength / sizeof(WCHAR));
    return true;
}

// The key MachineGuid lives in. Compared case-insensitively, and with the
// trailing separator the native path may carry removed.
bool IsCryptographyKey(HANDLE hKey) {
    std::wstring path;
    if (!GetKeyPath(hKey, path)) {
        return false;
    }

    while (!path.empty() && path.back() == L'\\') {
        path.pop_back();
    }

    static const std::wstring kExpected =
        L"\\REGISTRY\\MACHINE\\SOFTWARE\\Microsoft\\Cryptography";

    return CompareStringOrdinal(path.c_str(), (int)path.size(),
                                kExpected.c_str(), (int)kExpected.size(),
                                TRUE) == CSTR_EQUAL;
}

bool IsMachineGuidName(PUNICODE_STRING valueName) {
    if (!valueName || !valueName->Buffer || valueName->Length == 0) {
        return false;
    }

    static const WCHAR kName[] = L"MachineGuid";
    static const int kNameLength = ARRAYSIZE(kName) - 1;

    if (valueName->Length / sizeof(WCHAR) != (ULONG)kNameLength) {
        return false;
    }

    return CompareStringOrdinal(valueName->Buffer, kNameLength, kName,
                                kNameLength, TRUE) == CSTR_EQUAL;
}

// ============================================================
//  Hook
// ============================================================

NTSTATUS NTAPI NtQueryValueKey_Hook(HANDLE KeyHandle,
                                    PUNICODE_STRING ValueName,
                                    KEY_VALUE_INFORMATION_CLASS_LOCAL InfoClass,
                                    PVOID Info,
                                    ULONG Length,
                                    PULONG ResultLength) {
    NTSTATUS status = NtQueryValueKey_Original(KeyHandle, ValueName, InfoClass,
                                               Info, Length, ResultLength);

    if (!NT_SUCCESS(status) || !g_fakeGuid[0] || !Info) {
        return status;
    }

    if (InfoClass != KeyValuePartialInformationLocal) {
        return status;
    }

    if (!IsMachineGuidName(ValueName) || !IsCryptographyKey(KeyHandle)) {
        return status;
    }

    auto* info = reinterpret_cast<PKEY_VALUE_PARTIAL_INFORMATION_LOCAL>(Info);

    // Only a plain string of exactly the real value's size is replaced, so a
    // same-named value of another type in another key is never touched.
    if (info->Type != REG_SZ || info->DataLength != sizeof(g_fakeGuid)) {
        return status;
    }

    memcpy(info->Data, g_fakeGuid, sizeof(g_fakeGuid));
    return status;
}

// ============================================================
//  Settings
// ============================================================

// Accepts the plain 36-character form and the braced 38-character form.
// Anything else turns the spoofing off, with a note in the log.
void LoadSettings() {
    WindhawkUtils::StringSetting setting =
        WindhawkUtils::StringSetting::make(L"fakeGuid");

    std::wstring guid = setting.get();

    if (guid.size() == 38 && guid.front() == L'{' && guid.back() == L'}') {
        guid = guid.substr(1, 36);
    }

    bool valid = guid.size() == 36;
    if (valid) {
        for (size_t i = 0; i < guid.size(); i++) {
            if (i == 8 || i == 13 || i == 18 || i == 23) {
                if (guid[i] != L'-') {
                    valid = false;
                    break;
                }
            } else if (!iswxdigit(guid[i])) {
                valid = false;
                break;
            }
        }
    }

    if (!valid) {
        Wh_Log(L"Invalid MachineGuid '%s', spoofing is off", guid.c_str());
        g_fakeGuid[0] = L'\0';
        return;
    }

    memcpy(g_fakeGuid, guid.c_str(), sizeof(g_fakeGuid));
    Wh_Log(L"Spoofing MachineGuid as %s", g_fakeGuid);
}

// ============================================================
//  Mod entry points
// ============================================================

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) {
        Wh_Log(L"Failed to get ntdll.dll");
        return FALSE;
    }

    NtQueryKey_Func = (NtQueryKey_t)GetProcAddress(ntdll, "NtQueryKey");
    NtQueryValueKey_t pNtQueryValueKey =
        (NtQueryValueKey_t)GetProcAddress(ntdll, "NtQueryValueKey");

    if (!NtQueryKey_Func || !pNtQueryValueKey) {
        Wh_Log(L"Failed to resolve the registry functions");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(pNtQueryValueKey, NtQueryValueKey_Hook,
                                        &NtQueryValueKey_Original)) {
        Wh_Log(L"Failed to hook NtQueryValueKey");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
