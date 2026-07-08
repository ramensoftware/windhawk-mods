// ==WindhawkMod==
// @id              replace-buggy-sdhc-icon
// @name            Replace the buggy SDHC card drive icon
// @description     Replace the buggy SDHC card drive icon which MS does not bother to fix
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         *
// @compilerOptions -lntdll
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Replace the buggy SDHC card drive icon

Replace the buggy SDHC card drive icon which MS does not bother to fix since before Windows 8.

![Screenshot](https://i.imgur.com/6BKSRQR.png)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <winternl.h>
#include <string>
#include <algorithm>

// ============================================================
//  Native API type definitions
// ============================================================

typedef struct _KEY_NAME_INFORMATION {
    ULONG NameLength;
    WCHAR Name[1];
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

typedef enum _KEY_INFORMATION_CLASS {
    KeyNameInformation = 3
} KEY_INFORMATION_CLASS;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG TitleIndex;
    ULONG Type;
    ULONG DataLength;
    UCHAR Data[1];
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation           = 0,
    KeyValueFullInformation            = 1,
    KeyValuePartialInformation         = 2,
    KeyValueFullInformationAlign64     = 3,
    KeyValuePartialInformationAlign64  = 4
} KEY_VALUE_INFORMATION_CLASS;

typedef NTSTATUS (NTAPI *NtQueryKey_t)(
    HANDLE                  KeyHandle,
    KEY_INFORMATION_CLASS   KeyInformationClass,
    PVOID                   KeyInformation,
    ULONG                   Length,
    PULONG                  ResultLength
);

typedef NTSTATUS (NTAPI *NtQueryValueKey_t)(
    HANDLE                      KeyHandle,
    PUNICODE_STRING             ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID                       KeyValueInformation,
    ULONG                       Length,
    PULONG                      ResultLength
);

// ============================================================
//  Globals
// ============================================================

NtQueryKey_t      NtQueryKey_Func         = nullptr;
NtQueryValueKey_t NtQueryValueKey_Original = nullptr;

static const std::wstring kTargetValue  = L"%systemroot%\\syswow64\\rscricon.dll,50";
static const std::wstring kReplacedValue = L"%SystemRoot%\\system32\\shell32.dll,193";

// ============================================================
//  Helpers
// ============================================================

// Retrieves the full registry path of an open key handle using NtQueryKey.
bool GetKeyPath(HANDLE hKey, std::wstring& path) {
    ULONG size = 0;
    NtQueryKey_Func(hKey, KeyNameInformation, nullptr, 0, &size);
    if (size == 0) {
        return false;
    }

    BYTE* buffer = new BYTE[size];
    NTSTATUS status = NtQueryKey_Func(hKey, KeyNameInformation, buffer, size, &size);
    if (NT_SUCCESS(status)) {
        auto* info = reinterpret_cast<KEY_NAME_INFORMATION*>(buffer);
        path.assign(info->Name, info->NameLength / sizeof(WCHAR));
        delete[] buffer;
        return true;
    }

    delete[] buffer;
    return false;
}

// Returns true if the key's path ends with "\DefaultIcon" (case-insensitive).
bool IsDefaultIconKey(HANDLE hKey) {
    std::wstring path;
    if (!GetKeyPath(hKey, path)) {
        return false;
    }

    std::transform(path.begin(), path.end(), path.begin(), ::towlower);

    static const std::wstring kSuffix = L"\\defaulticon";
    if (path.length() < kSuffix.length()) {
        return false;
    }

    return path.compare(path.length() - kSuffix.length(), kSuffix.length(), kSuffix) == 0;
}

// ============================================================
//  Hook
// ============================================================

// Intercepts NtQueryValueKey to redirect a specific DefaultIcon value.
NTSTATUS NTAPI NtQueryValueKey_Hook(
    HANDLE                      KeyHandle,
    PUNICODE_STRING             ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID                       KeyValueInformation,
    ULONG                       Length,
    PULONG                      ResultLength
) {
    NTSTATUS status = NtQueryValueKey_Original(
        KeyHandle, ValueName, KeyValueInformationClass,
        KeyValueInformation, Length, ResultLength);

    // Only process successful calls.
    if (!NT_SUCCESS(status)) {
        return status;
    }

    // Only handle KeyValuePartialInformation requests.
    if (KeyValueInformationClass != KeyValuePartialInformation &&
        KeyValueInformationClass != KeyValuePartialInformationAlign64) {
        return status;
    }

    // Only intercept reads of the default value (empty or null name).
    bool isDefaultValue = !ValueName || !ValueName->Buffer || ValueName->Length == 0;
    if (!isDefaultValue) {
        return status;
    }

    // Only act on keys named DefaultIcon.
    if (!IsDefaultIconKey(KeyHandle)) {
        return status;
    }

    auto* info = reinterpret_cast<PKEY_VALUE_PARTIAL_INFORMATION>(KeyValueInformation);

    // Only handle plain string values.
    if (info->Type != REG_SZ || info->DataLength == 0) {
        return status;
    }

    // Read the current value and strip the trailing null if present.
    std::wstring value(reinterpret_cast<WCHAR*>(info->Data), info->DataLength / sizeof(WCHAR));
    if (!value.empty() && value.back() == L'\0') {
        value.pop_back();
    }

    // Compare case-insensitively against the target string.
    std::wstring valueLower = value;
    std::transform(valueLower.begin(), valueLower.end(), valueLower.begin(), ::towlower);

    if (valueLower != kTargetValue) {
        return status;
    }

    // Ensure the output buffer is large enough for the replacement string.
    size_t newDataSize = (kReplacedValue.length() + 1) * sizeof(WCHAR);
    size_t available   = Length - FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data);
    if (newDataSize > available) {
        return status;
    }

    // Write the replacement value into the caller's buffer.
    wcscpy_s(reinterpret_cast<WCHAR*>(info->Data), available / sizeof(WCHAR),
             kReplacedValue.c_str());
    info->DataLength = static_cast<ULONG>(newDataSize);

    if (ResultLength) {
        *ResultLength = FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data)
                      + static_cast<ULONG>(newDataSize);
    }

    return status;
}

// ============================================================
//  Mod entry points
// ============================================================

BOOL Wh_ModInit() {

HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
if (!ntdll) {
    return FALSE;
}

NtQueryKey_Func    = (NtQueryKey_t)     GetProcAddress(ntdll, "NtQueryKey");
void* pNtQueryValueKey = (void*)        GetProcAddress(ntdll, "NtQueryValueKey");

if (!NtQueryKey_Func || !pNtQueryValueKey) {
    return FALSE;
}

return Wh_SetFunctionHook(pNtQueryValueKey, (void*)NtQueryValueKey_Hook, (void**)&NtQueryValueKey_Original);

}
