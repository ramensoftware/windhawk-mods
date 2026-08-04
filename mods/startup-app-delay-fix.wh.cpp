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
startup-item serialization settings. It leaves no persistent registry changes
behind.

Only `WaitforIdleState` is overridden. The related `StartupDelayInMSec` value
is left unchanged.

The setting is read when Explorer starts, so enabling or disabling the mod
takes effect after the next sign-in or Explorer restart.
*/
// ==/WindhawkModReadme==

#include <ntdef.h>
#include <ntstatus.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

constexpr wchar_t kKeyPathSuffix[] =
    L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Serialize";
constexpr wchar_t kUserHivePrefix[] = L"\\REGISTRY\\USER\\";
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

struct KEY_FULL_INFORMATION_LOCAL {
    LARGE_INTEGER LastWriteTime;
    ULONG TitleIndex;
    ULONG ClassOffset;
    ULONG ClassLength;
    ULONG SubKeys;
    ULONG MaxNameLen;
    ULONG MaxClassLen;
    ULONG Values;
    ULONG MaxValueNameLen;
    ULONG MaxValueDataLen;
    WCHAR Class[1];
};

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

using NtEnumerateValueKey_t = NTSTATUS(NTAPI*)(
    HANDLE keyHandle, ULONG index,
    KEY_VALUE_INFORMATION_CLASS keyValueInformationClass,
    PVOID keyValueInformation, ULONG length, PULONG resultLength);
NtEnumerateValueKey_t NtEnumerateValueKey_orig;

using NtOpenKey_t = NTSTATUS(NTAPI*)(PHANDLE keyHandle,
                                     ACCESS_MASK desiredAccess,
                                     POBJECT_ATTRIBUTES objectAttributes);
NtOpenKey_t NtOpenKey_orig;

using NtOpenKeyEx_t = NTSTATUS(NTAPI*)(PHANDLE keyHandle,
                                       ACCESS_MASK desiredAccess,
                                       POBJECT_ATTRIBUTES objectAttributes,
                                       ULONG openOptions);
NtOpenKeyEx_t NtOpenKeyEx_orig;

using NtClose_t = NTSTATUS(NTAPI*)(HANDLE handle);
NtClose_t NtClose_orig;

SRWLOCK g_virtualKeyHandlesLock = SRWLOCK_INIT;
std::unordered_set<HANDLE> g_virtualKeyHandles;
std::atomic<size_t> g_virtualKeyHandleCount{0};

bool IsVirtualKey(HANDLE key) {
    if (g_virtualKeyHandleCount.load(std::memory_order_relaxed) == 0) {
        return false;
    }
    AcquireSRWLockShared(&g_virtualKeyHandlesLock);
    bool found = g_virtualKeyHandles.find(key) != g_virtualKeyHandles.end();
    ReleaseSRWLockShared(&g_virtualKeyHandlesLock);
    return found;
}

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
    constexpr size_t valueNameLength = ARRAYSIZE(kValueName) - 1;
    return valueName && valueName->Buffer &&
           valueName->Length == valueNameLength * sizeof(wchar_t) &&
           _wcsnicmp(valueName->Buffer, kValueName, valueNameLength) == 0;
}

bool IsSerializePath(std::wstring_view path) {
    constexpr size_t userHivePrefixLength = ARRAYSIZE(kUserHivePrefix) - 1;
    constexpr size_t suffixLength = ARRAYSIZE(kKeyPathSuffix) - 1;
    return path.length() >= userHivePrefixLength + suffixLength &&
           _wcsnicmp(path.data(), kUserHivePrefix, userHivePrefixLength) == 0 &&
           _wcsnicmp(path.data() + path.length() - suffixLength,
                      kKeyPathSuffix, suffixLength) == 0;
}

bool IsSerializeKey(HANDLE key) {
    if (IsVirtualKey(key)) {
        return true;
    }

    std::wstring path;
    return GetKeyPath(key, &path) && IsSerializePath(path);
}

bool IsTargetQuery(HANDLE key, const UNICODE_STRING* valueName) {
    if (!IsTargetValueName(valueName)) {
        return false;
    }

    return IsSerializeKey(key);
}

bool IsTargetOpen(const OBJECT_ATTRIBUTES* objectAttributes) {
    if (!objectAttributes || !objectAttributes->ObjectName ||
        !objectAttributes->ObjectName->Buffer) {
        return false;
    }

    const UNICODE_STRING* objectName = objectAttributes->ObjectName;
    std::wstring_view name(objectName->Buffer,
                           objectName->Length / sizeof(wchar_t));
    constexpr std::wstring_view serializeName = L"Serialize";
    if (name.length() < serializeName.length() ||
        _wcsnicmp(name.data() + name.length() - serializeName.length(),
                   serializeName.data(), serializeName.length()) != 0) {
        return false;
    }

    std::wstring path(objectName->Buffer,
                      objectName->Length / sizeof(wchar_t));
    if (objectAttributes->RootDirectory &&
        (path.empty() || path.front() != L'\\')) {
        std::wstring rootPath;
        if (!GetKeyPath(objectAttributes->RootDirectory, &rootPath)) {
            return false;
        }
        if (!path.empty()) {
            rootPath += L'\\';
            rootPath += path;
        }
        path = rootPath;
    }

    return IsSerializePath(path);
}

void RememberVirtualKey(HANDLE key) {
    AcquireSRWLockExclusive(&g_virtualKeyHandlesLock);
    if (g_virtualKeyHandles.insert(key).second) {
        g_virtualKeyHandleCount.fetch_add(1, std::memory_order_relaxed);
    }
    ReleaseSRWLockExclusive(&g_virtualKeyHandlesLock);
}

// A tracked handle is only a read-only backing handle for value queries. It
// must never be used as a real registry key for child opens or other namespace
// operations, because it actually refers to the parent Explorer key.
NTSTATUS OpenParentAsVirtualKey(PHANDLE keyHandle,
                                POBJECT_ATTRIBUTES objectAttributes,
                                ULONG openOptions, bool useNtOpenKeyEx,
                                NTSTATUS originalStatus) {
    const UNICODE_STRING* objectName = objectAttributes->ObjectName;
    std::wstring parentName(objectName->Buffer,
                            objectName->Length / sizeof(wchar_t));
    constexpr wchar_t serializeComponent[] = L"\\Serialize";
    constexpr size_t serializeComponentLength =
        ARRAYSIZE(serializeComponent) - 1;

    if (parentName.length() >= serializeComponentLength &&
        _wcsicmp(parentName.c_str() + parentName.length() -
                     serializeComponentLength,
                 serializeComponent) == 0) {
        parentName.resize(parentName.length() - serializeComponentLength);
    } else if (objectAttributes->RootDirectory &&
               _wcsicmp(parentName.c_str(), L"Serialize") == 0) {
        HANDLE duplicate = nullptr;
        if (!DuplicateHandle(GetCurrentProcess(), objectAttributes->RootDirectory,
                             GetCurrentProcess(), &duplicate, KEY_QUERY_VALUE,
                             FALSE, 0)) {
            return originalStatus;
        }
        *keyHandle = duplicate;
        RememberVirtualKey(duplicate);
        return STATUS_SUCCESS;
    } else {
        return originalStatus;
    }

    UNICODE_STRING parentObjectName{
        .Length = static_cast<USHORT>(parentName.length() * sizeof(wchar_t)),
        .MaximumLength =
            static_cast<USHORT>(parentName.length() * sizeof(wchar_t)),
        .Buffer = parentName.data(),
    };
    OBJECT_ATTRIBUTES parentAttributes = *objectAttributes;
    parentAttributes.Length = sizeof(parentAttributes);
    parentAttributes.ObjectName = &parentObjectName;

    NTSTATUS status = useNtOpenKeyEx
                          ? NtOpenKeyEx_orig(keyHandle, KEY_QUERY_VALUE,
                                             &parentAttributes, openOptions)
                          : NtOpenKey_orig(keyHandle, KEY_QUERY_VALUE,
                                           &parentAttributes);
    if (status == STATUS_SUCCESS) {
        RememberVirtualKey(*keyHandle);
    }
    return status;
}

bool IsMissingStatus(NTSTATUS status) {
    return status == STATUS_OBJECT_NAME_NOT_FOUND ||
           status == STATUS_OBJECT_PATH_NOT_FOUND;
}

constexpr ACCESS_MASK kNonReadAccess =
    KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_CREATE_LINK | DELETE | WRITE_DAC |
    WRITE_OWNER | GENERIC_WRITE | GENERIC_ALL | MAXIMUM_ALLOWED;

NTSTATUS NTAPI NtOpenKey_hook(PHANDLE keyHandle, ACCESS_MASK desiredAccess,
                              POBJECT_ATTRIBUTES objectAttributes) {
    if (objectAttributes && objectAttributes->RootDirectory &&
        IsVirtualKey(objectAttributes->RootDirectory)) {
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    NTSTATUS status =
        NtOpenKey_orig(keyHandle, desiredAccess, objectAttributes);
    if (IsMissingStatus(status) && !(desiredAccess & kNonReadAccess) &&
        IsTargetOpen(objectAttributes)) {
        NTSTATUS virtualStatus = OpenParentAsVirtualKey(
            keyHandle, objectAttributes, 0, false, status);
        if (virtualStatus == STATUS_SUCCESS) {
            Wh_Log(L"Substituting missing Serialize key (access=0x%08X)",
                   desiredAccess);
        }
        return virtualStatus;
    }
    return status;
}

NTSTATUS NTAPI NtOpenKeyEx_hook(PHANDLE keyHandle, ACCESS_MASK desiredAccess,
                                POBJECT_ATTRIBUTES objectAttributes,
                                ULONG openOptions) {
    if (objectAttributes && objectAttributes->RootDirectory &&
        IsVirtualKey(objectAttributes->RootDirectory)) {
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    NTSTATUS status = NtOpenKeyEx_orig(keyHandle, desiredAccess,
                                       objectAttributes, openOptions);
    if (IsMissingStatus(status) && !(desiredAccess & kNonReadAccess) &&
        IsTargetOpen(objectAttributes)) {
        NTSTATUS virtualStatus = OpenParentAsVirtualKey(
            keyHandle, objectAttributes, openOptions, true, status);
        if (virtualStatus == STATUS_SUCCESS) {
            Wh_Log(L"Substituting missing Serialize key (access=0x%08X)",
                   desiredAccess);
        }
        return virtualStatus;
    }
    return status;
}

NTSTATUS NTAPI NtClose_hook(HANDLE handle) {
    if (g_virtualKeyHandleCount.load(std::memory_order_relaxed) != 0 &&
        IsVirtualKey(handle)) {
        AcquireSRWLockExclusive(&g_virtualKeyHandlesLock);
        if (g_virtualKeyHandles.erase(handle)) {
            g_virtualKeyHandleCount.fetch_sub(1, std::memory_order_relaxed);
        }
        ReleaseSRWLockExclusive(&g_virtualKeyHandlesLock);
    }
    return NtClose_orig(handle);
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

    return STATUS_INVALID_INFO_CLASS;
}

bool IsSupportedValueInformationClass(
    KEY_VALUE_INFORMATION_CLASS informationClass) {
    switch (informationClass) {
        case KeyValueBasicInformation:
        case KeyValueFullInformation:
        case KeyValuePartialInformation:
        case KeyValueFullInformationAlign64:
            return true;
        default:
            return false;
    }
}

UNICODE_STRING GetTargetValueName() {
    return {
        .Length = static_cast<USHORT>((ARRAYSIZE(kValueName) - 1) *
                                      sizeof(wchar_t)),
        .MaximumLength = static_cast<USHORT>(sizeof(kValueName)),
        .Buffer = const_cast<PWSTR>(kValueName),
    };
}

bool IsTargetValueAtIndex(HANDLE keyHandle, ULONG index) {
    alignas(KEY_VALUE_BASIC_INFORMATION_LOCAL) BYTE buffer[128];
    ULONG resultLength = 0;
    NTSTATUS status = NtEnumerateValueKey_orig(
        keyHandle, index, KeyValueBasicInformation, buffer, sizeof(buffer),
        &resultLength);
    if (status != STATUS_SUCCESS && status != STATUS_BUFFER_OVERFLOW) {
        return false;
    }

    auto info = reinterpret_cast<KEY_VALUE_BASIC_INFORMATION_LOCAL*>(buffer);
    constexpr ULONG targetNameLength =
        (ARRAYSIZE(kValueName) - 1) * sizeof(wchar_t);
    constexpr ULONG nameOffset =
        FIELD_OFFSET(KEY_VALUE_BASIC_INFORMATION_LOCAL, Name);
    return info->NameLength == targetNameLength &&
           resultLength >= nameOffset + targetNameLength &&
           _wcsnicmp(info->Name, kValueName, ARRAYSIZE(kValueName) - 1) == 0;
}

bool GetValueCount(HANDLE keyHandle, ULONG* valueCount) {
    KEY_FULL_INFORMATION_LOCAL info{};
    ULONG resultLength = 0;
    NTSTATUS status = NtQueryKey(keyHandle, KeyFullInformation, &info,
                                 sizeof(info), &resultLength);
    if (status != STATUS_SUCCESS && status != STATUS_BUFFER_OVERFLOW) {
        return false;
    }
    *valueCount = info.Values;
    return true;
}

bool ContainsTargetValue(HANDLE keyHandle, ULONG valueCount) {
    for (ULONG index = 0; index < valueCount; index++) {
        if (IsTargetValueAtIndex(keyHandle, index)) {
            return true;
        }
    }
    return false;
}

NTSTATUS NTAPI NtQueryValueKey_hook(
    HANDLE keyHandle, PUNICODE_STRING valueName,
    KEY_VALUE_INFORMATION_CLASS keyValueInformationClass,
    PVOID keyValueInformation, ULONG length, PULONG resultLength) {
    bool virtualKey = IsVirtualKey(keyHandle);
    NTSTATUS status;
    if (virtualKey) {
        if (!IsTargetValueName(valueName)) {
            return STATUS_OBJECT_NAME_NOT_FOUND;
        }
        status = STATUS_OBJECT_NAME_NOT_FOUND;
    } else {
        status = NtQueryValueKey_orig(
            keyHandle, valueName, keyValueInformationClass,
            keyValueInformation, length, resultLength);
        if (!IsTargetQuery(keyHandle, valueName)) {
            return status;
        }
    }

    if (status != STATUS_SUCCESS && status != STATUS_OBJECT_NAME_NOT_FOUND &&
        status != STATUS_OBJECT_PATH_NOT_FOUND &&
        status != STATUS_BUFFER_TOO_SMALL &&
        status != STATUS_BUFFER_OVERFLOW) {
        return status;
    }

    if (!IsSupportedValueInformationClass(keyValueInformationClass)) {
        return status;
    }

    status = FillValueInformation(valueName, keyValueInformationClass,
                                  keyValueInformation, length, resultLength);
    if (status == STATUS_SUCCESS) {
        Wh_Log(L"Reporting WaitforIdleState=0");
    }
    return status;
}

NTSTATUS NTAPI NtEnumerateValueKey_hook(
    HANDLE keyHandle, ULONG index,
    KEY_VALUE_INFORMATION_CLASS keyValueInformationClass,
    PVOID keyValueInformation, ULONG length, PULONG resultLength) {
    bool virtualKey = IsVirtualKey(keyHandle);
    if (virtualKey) {
        if (index != 0) {
            return STATUS_NO_MORE_ENTRIES;
        }
        if (!IsSupportedValueInformationClass(keyValueInformationClass)) {
            return STATUS_INVALID_INFO_CLASS;
        }
        UNICODE_STRING valueName = GetTargetValueName();
        return FillValueInformation(&valueName, keyValueInformationClass,
                                    keyValueInformation, length, resultLength);
    }

    NTSTATUS status = NtEnumerateValueKey_orig(
        keyHandle, index, keyValueInformationClass, keyValueInformation, length,
        resultLength);
    if (!IsSerializeKey(keyHandle) ||
        !IsSupportedValueInformationClass(keyValueInformationClass)) {
        return status;
    }

    bool targetAtIndex = IsTargetValueAtIndex(keyHandle, index);
    if (!targetAtIndex && status != STATUS_NO_MORE_ENTRIES) {
        return status;
    }

    if (!targetAtIndex) {
        ULONG valueCount;
        if (!GetValueCount(keyHandle, &valueCount) || index != valueCount ||
            ContainsTargetValue(keyHandle, valueCount)) {
            return status;
        }
    }

    UNICODE_STRING valueName = GetTargetValueName();
    return FillValueInformation(&valueName, keyValueInformationClass,
                                keyValueInformation, length, resultLength);
}

BOOL Wh_ModInit() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto ntQueryValueKey = reinterpret_cast<NtQueryValueKey_t>(
        GetProcAddress(ntdll, "NtQueryValueKey"));
    auto ntOpenKey = reinterpret_cast<NtOpenKey_t>(
        GetProcAddress(ntdll, "NtOpenKey"));
    auto ntEnumerateValueKey = reinterpret_cast<NtEnumerateValueKey_t>(
        GetProcAddress(ntdll, "NtEnumerateValueKey"));
    auto ntOpenKeyEx = reinterpret_cast<NtOpenKeyEx_t>(
        GetProcAddress(ntdll, "NtOpenKeyEx"));
    auto ntClose =
        reinterpret_cast<NtClose_t>(GetProcAddress(ntdll, "NtClose"));
    if (!ntQueryValueKey || !ntEnumerateValueKey || !ntOpenKey ||
        !ntOpenKeyEx || !ntClose) {
        Wh_Log(L"Failed to find required ntdll registry functions");
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(ntQueryValueKey, NtQueryValueKey_hook,
                                   &NtQueryValueKey_orig);
    WindhawkUtils::SetFunctionHook(ntEnumerateValueKey,
                                   NtEnumerateValueKey_hook,
                                   &NtEnumerateValueKey_orig);
    WindhawkUtils::SetFunctionHook(ntOpenKey, NtOpenKey_hook, &NtOpenKey_orig);
    WindhawkUtils::SetFunctionHook(ntOpenKeyEx, NtOpenKeyEx_hook,
                                   &NtOpenKeyEx_orig);
    WindhawkUtils::SetFunctionHook(ntClose, NtClose_hook, &NtClose_orig);
    return TRUE;
}
