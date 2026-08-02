// ==WindhawkMod==
// @id              startup-app-delay-fix
// @name            Startup App Delay Fix
// @description     Removes the delay Windows applies to startup applications after sign-in
// @version         1.1
// @author          meteoni
// @github          https://github.com/Meteony
// @include         explorer.exe
// @compilerOptions -lntdll
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
Windows intentionally delays startup applications after sign-in to reduce
system load, which can cause apps to open minutes after startup.

This mod makes Explorer see `WaitforIdleState` as zero when it reads the
startup-item serialization settings. It doesn't create or modify registry
values, so disabling the mod restores Windows' normal behavior immediately.

Only `WaitforIdleState` is overridden. The related `StartupDelayInMSec` value
is left unchanged.
*/
// ==/WindhawkModReadme==

#include <ntdef.h>
#include <ntstatus.h>

#include <cstring>
#include <string>

constexpr wchar_t kKeyPathSuffix[] =
    L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Serialize";
constexpr wchar_t kValueName[] = L"WaitforIdleState";

typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation,
    KeyNameInformation,
} KEY_INFORMATION_CLASS;

typedef struct _KEY_NAME_INFORMATION {
    ULONG NameLength;
    WCHAR Name[1];
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64,
} KEY_VALUE_INFORMATION_CLASS;

struct KEY_VALUE_BASIC_INFORMATION_LOCAL {
    ULONG TitleIndex;
    ULONG Type;
    ULONG NameLength;
    WCHAR Name[1];
};

struct KEY_VALUE_FULL_INFORMATION_LOCAL {
    ULONG TitleIndex;
    ULONG Type;
    ULONG DataOffset;
    ULONG DataLength;
    ULONG NameLength;
    WCHAR Name[1];
};

struct KEY_VALUE_PARTIAL_INFORMATION_LOCAL {
    ULONG TitleIndex;
    ULONG Type;
    ULONG DataLength;
    UCHAR Data[1];
};

EXTERN_C NTSYSAPI NTSTATUS NTAPI NtQueryKey(
    HANDLE keyHandle, KEY_INFORMATION_CLASS keyInformationClass,
    PVOID keyInformation, ULONG length, PULONG resultLength);

using NtQueryValueKey_t = NTSTATUS(NTAPI*)(
    HANDLE keyHandle, PUNICODE_STRING valueName,
    KEY_VALUE_INFORMATION_CLASS keyValueInformationClass,
    PVOID keyValueInformation, ULONG length, PULONG resultLength);
NtQueryValueKey_t NtQueryValueKey_orig;

bool GetKeyPath(HANDLE key, std::wstring* path) {
    ULONG size = 0;
    NTSTATUS status = NtQueryKey(key, KeyNameInformation, nullptr, 0, &size);
    if (status != STATUS_BUFFER_TOO_SMALL &&
        status != STATUS_BUFFER_OVERFLOW) {
        return false;
    }

    std::wstring buffer((size + sizeof(wchar_t) - 1) / sizeof(wchar_t), L'\0');
    auto info = reinterpret_cast<KEY_NAME_INFORMATION*>(buffer.data());
    status = NtQueryKey(key, KeyNameInformation, info, size, &size);
    if (status < 0) {
        return false;
    }

    path->assign(info->Name, info->NameLength / sizeof(wchar_t));
    return true;
}

bool IsTargetQuery(HANDLE key, const UNICODE_STRING* valueName) {
    constexpr size_t valueNameLength = ARRAYSIZE(kValueName) - 1;
    if (!valueName || !valueName->Buffer ||
        valueName->Length != valueNameLength * sizeof(wchar_t) ||
        _wcsnicmp(valueName->Buffer, kValueName, valueNameLength) != 0) {
        return false;
    }

    std::wstring path;
    if (!GetKeyPath(key, &path)) {
        return false;
    }

    size_t suffixLength = ARRAYSIZE(kKeyPathSuffix) - 1;
    return path.length() >= suffixLength &&
           _wcsnicmp(path.c_str() + path.length() - suffixLength,
                      kKeyPathSuffix, suffixLength) == 0;
}

ULONG AlignUp(ULONG value, ULONG alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

NTSTATUS FillValueInformation(const UNICODE_STRING* valueName,
                              KEY_VALUE_INFORMATION_CLASS informationClass,
                              void* buffer, ULONG bufferLength,
                              ULONG* resultLength) {
    ULONG dataOffset;
    ULONG requiredLength;

    switch (informationClass) {
        case KeyValueBasicInformation: {
            ULONG nameOffset = FIELD_OFFSET(KEY_VALUE_BASIC_INFORMATION_LOCAL,
                                            Name);
            requiredLength = nameOffset + valueName->Length;
            if (resultLength) {
                *resultLength = requiredLength;
            }
            if (!buffer || bufferLength < nameOffset) {
                return STATUS_BUFFER_TOO_SMALL;
            }
            auto info = static_cast<KEY_VALUE_BASIC_INFORMATION_LOCAL*>(buffer);
            info->TitleIndex = 0;
            info->Type = REG_DWORD;
            info->NameLength = valueName->Length;
            ULONG copiedNameLength =
                min(valueName->Length, bufferLength - nameOffset);
            std::memcpy(info->Name, valueName->Buffer, copiedNameLength);
            return bufferLength < requiredLength ? STATUS_BUFFER_OVERFLOW
                                                 : STATUS_SUCCESS;
        }

        case KeyValueFullInformation:
        case KeyValueFullInformationAlign64: {
            ULONG nameOffset = FIELD_OFFSET(KEY_VALUE_FULL_INFORMATION_LOCAL,
                                            Name);
            ULONG alignment = informationClass == KeyValueFullInformationAlign64
                                  ? 8
                                  : 4;
            dataOffset = AlignUp(nameOffset + valueName->Length, alignment);
            requiredLength = dataOffset + sizeof(DWORD);
            if (resultLength) {
                *resultLength = requiredLength;
            }
            if (!buffer || bufferLength < nameOffset) {
                return STATUS_BUFFER_TOO_SMALL;
            }
            auto info = static_cast<KEY_VALUE_FULL_INFORMATION_LOCAL*>(buffer);
            info->TitleIndex = 0;
            info->Type = REG_DWORD;
            info->DataOffset = dataOffset;
            info->DataLength = sizeof(DWORD);
            info->NameLength = valueName->Length;
            ULONG copiedNameLength =
                min(valueName->Length, bufferLength - nameOffset);
            std::memcpy(info->Name, valueName->Buffer, copiedNameLength);
            if (bufferLength < requiredLength) {
                return STATUS_BUFFER_OVERFLOW;
            }
            *reinterpret_cast<DWORD*>(static_cast<BYTE*>(buffer) + dataOffset) =
                0;
            return STATUS_SUCCESS;
        }

        case KeyValuePartialInformation:
        case KeyValuePartialInformationAlign64: {
            dataOffset = informationClass == KeyValuePartialInformationAlign64
                             ? 16
                             : FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION_LOCAL,
                                            Data);
            requiredLength = dataOffset + sizeof(DWORD);
            if (resultLength) {
                *resultLength = requiredLength;
            }
            constexpr ULONG headerLength =
                FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION_LOCAL, Data);
            if (!buffer || bufferLength < headerLength) {
                return STATUS_BUFFER_TOO_SMALL;
            }
            auto info =
                static_cast<KEY_VALUE_PARTIAL_INFORMATION_LOCAL*>(buffer);
            info->TitleIndex = 0;
            info->Type = REG_DWORD;
            info->DataLength = sizeof(DWORD);
            if (bufferLength < requiredLength) {
                return STATUS_BUFFER_OVERFLOW;
            }
            *reinterpret_cast<DWORD*>(static_cast<BYTE*>(buffer) + dataOffset) =
                0;
            return STATUS_SUCCESS;
        }
    }

    return STATUS_INVALID_INFO_CLASS;
}

NTSTATUS NTAPI NtQueryValueKey_hook(
    HANDLE keyHandle, PUNICODE_STRING valueName,
    KEY_VALUE_INFORMATION_CLASS keyValueInformationClass,
    PVOID keyValueInformation, ULONG length, PULONG resultLength) {
    NTSTATUS status = NtQueryValueKey_orig(
        keyHandle, valueName, keyValueInformationClass, keyValueInformation,
        length, resultLength);

    if (!IsTargetQuery(keyHandle, valueName)) {
        return status;
    }

    return FillValueInformation(valueName, keyValueInformationClass,
                                keyValueInformation, length, resultLength);
}

BOOL Wh_ModInit() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    void* ntQueryValueKey =
        reinterpret_cast<void*>(GetProcAddress(ntdll, "NtQueryValueKey"));
    if (!ntQueryValueKey) {
        Wh_Log(L"Failed to find NtQueryValueKey");
        return FALSE;
    }

    Wh_SetFunctionHook(ntQueryValueKey,
                       reinterpret_cast<void*>(NtQueryValueKey_hook),
                       reinterpret_cast<void**>(&NtQueryValueKey_orig));
    return TRUE;
}
