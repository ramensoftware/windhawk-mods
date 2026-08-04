// ==WindhawkMod==
// @id              startup-app-delay-fix
// @name            Startup App Delay Fix
// @description     Removes the delay Windows applies to startup applications after sign-in
// @version         1.3
// @author          meteoni
// @github          https://github.com/Meteony
// @include         explorer.exe
// @compilerOptions -ladvapi32 -lntdll
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
Windows intentionally delays startup applications after sign-in to reduce
system load, which can cause apps to open minutes after startup.

This mod makes Explorer see both `WaitforIdleState` and `StartupDelayInMSec` as
zero when it reads the startup-item serialization settings. It leaves no
persistent registry changes behind.

If the `Serialize` key is missing, the mod creates it as a volatile, in-memory
key. The creating instance deletes it on a normal disable; otherwise it is
discarded when the user registry hive unloads at sign-out or shutdown.

The settings are read when Explorer starts, so enabling or disabling the mod
takes effect after the next sign-in or Explorer restart.

This only affects the delay Explorer applies to startup-item processing. It
doesn't change delays configured by other systems, such as Task Scheduler.
*/
// ==/WindhawkModReadme==

#include <ntdef.h>
#include <ntstatus.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cstring>
#include <cwchar>
#include <string>
#include <string_view>
#include <vector>

constexpr wchar_t kRegistryPath[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Serialize";
constexpr wchar_t kKeyPathSuffix[] =
    L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Serialize";
constexpr wchar_t kUserHivePrefix[] = L"\\REGISTRY\\USER\\";
constexpr const wchar_t* kValueNames[] = {
    L"WaitforIdleState",
    L"StartupDelayInMSec",
};

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

HKEY g_createdSerializeKey;

bool GetKeyPath(HANDLE key, std::wstring* path) {
    ULONG size = 0;
    NTSTATUS status = NtQueryKey(key, KeyNameInformation, nullptr, 0, &size);
    if (status != STATUS_BUFFER_TOO_SMALL &&
        status != STATUS_BUFFER_OVERFLOW) {
        return false;
    }

    std::vector<BYTE> buffer(size);
    auto info = reinterpret_cast<KEY_NAME_INFORMATION*>(buffer.data());
    status = NtQueryKey(key, KeyNameInformation, info, size, &size);
    if (status < 0) {
        return false;
    }

    path->assign(info->Name, info->NameLength / sizeof(wchar_t));
    return true;
}

bool IsTargetValueName(const UNICODE_STRING* valueName) {
    if (!valueName || !valueName->Buffer) {
        return false;
    }

    for (const wchar_t* targetName : kValueNames) {
        size_t targetLength = wcslen(targetName);
        if (valueName->Length == targetLength * sizeof(wchar_t) &&
            _wcsnicmp(valueName->Buffer, targetName, targetLength) == 0) {
            return true;
        }
    }
    return false;
}

bool IsSerializePath(std::wstring_view path) {
    constexpr size_t userHivePrefixLength = ARRAYSIZE(kUserHivePrefix) - 1;
    constexpr size_t suffixLength = ARRAYSIZE(kKeyPathSuffix) - 1;
    return path.length() >= userHivePrefixLength + suffixLength &&
           _wcsnicmp(path.data(), kUserHivePrefix, userHivePrefixLength) == 0 &&
           _wcsnicmp(path.data() + path.length() - suffixLength,
                      kKeyPathSuffix, suffixLength) == 0;
}

bool IsTargetQuery(HANDLE key, const UNICODE_STRING* valueName) {
    if (!IsTargetValueName(valueName)) {
        return false;
    }

    std::wstring path;
    return GetKeyPath(key, &path) && IsSerializePath(path);
}

ULONG AlignUp(ULONG value, ULONG alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

NTSTATUS FillValueInformation(const UNICODE_STRING* valueName,
                              KEY_VALUE_INFORMATION_CLASS informationClass,
                              void* buffer, ULONG bufferLength,
                              ULONG* resultLength,
                              NTSTATUS unsupportedStatus) {
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
                std::min<ULONG>(valueName->Length, bufferLength - nameOffset);
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
                std::min<ULONG>(valueName->Length, bufferLength - nameOffset);
            std::memcpy(info->Name, valueName->Buffer, copiedNameLength);
            if (bufferLength < requiredLength) {
                return STATUS_BUFFER_OVERFLOW;
            }
            *reinterpret_cast<DWORD*>(static_cast<BYTE*>(buffer) + dataOffset) =
                0;
            return STATUS_SUCCESS;
        }

        case KeyValuePartialInformation: {
            dataOffset = FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION_LOCAL,
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

    return unsupportedStatus;
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

    if (status != STATUS_SUCCESS && status != STATUS_OBJECT_NAME_NOT_FOUND &&
        status != STATUS_OBJECT_PATH_NOT_FOUND &&
        status != STATUS_BUFFER_TOO_SMALL &&
        status != STATUS_BUFFER_OVERFLOW) {
        return status;
    }

    status = FillValueInformation(valueName, keyValueInformationClass,
                                  keyValueInformation, length, resultLength,
                                  status);
    if (status == STATUS_SUCCESS) {
        Wh_Log(L"Reporting %.*s=0",
               static_cast<int>(valueName->Length / sizeof(wchar_t)),
               valueName->Buffer);
    }
    return status;
}

BOOL Wh_ModInit() {
    DWORD disposition;
    HKEY serializeKey;
    LSTATUS createStatus = RegCreateKeyExW(
        HKEY_CURRENT_USER, kRegistryPath, 0, nullptr, REG_OPTION_VOLATILE,
        KEY_READ, nullptr, &serializeKey, &disposition);
    if (createStatus != ERROR_SUCCESS) {
        Wh_Log(L"Failed to open or create volatile Serialize key: %ld",
               createStatus);
        return FALSE;
    } else if (disposition == REG_CREATED_NEW_KEY) {
        g_createdSerializeKey = serializeKey;
        Wh_Log(L"Created volatile Serialize key");
    } else {
        RegCloseKey(serializeKey);
    }

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto ntQueryValueKey = reinterpret_cast<NtQueryValueKey_t>(
        GetProcAddress(ntdll, "NtQueryValueKey"));
    if (!ntQueryValueKey) {
        Wh_Log(L"Failed to find NtQueryValueKey");
        if (g_createdSerializeKey) {
            RegCloseKey(g_createdSerializeKey);
            g_createdSerializeKey = nullptr;
            RegDeleteKeyW(HKEY_CURRENT_USER, kRegistryPath);
        }
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(ntQueryValueKey, NtQueryValueKey_hook,
                                   &NtQueryValueKey_orig);
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_createdSerializeKey) {
        return;
    }

    RegCloseKey(g_createdSerializeKey);
    g_createdSerializeKey = nullptr;
    LSTATUS status = RegDeleteKeyW(HKEY_CURRENT_USER, kRegistryPath);
    if (status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND) {
        Wh_Log(L"Failed to delete volatile Serialize key: %ld", status);
    }
}
