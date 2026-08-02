// ==WindhawkMod==
// @id              startup-app-delay-fix
// @name            Startup App Delay Fix
// @description     Removes the delay Windows applies to startup applications after sign-in
// @version         1.1
// @author          meteoni
// @github          https://github.com/Meteony
// @include         explorer.exe
// @compilerOptions -lntdll -lshell32
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

The setting is read when Explorer starts. Enabling or disabling the mod during
a session takes effect after the next sign-in or Explorer restart.
*/
// ==/WindhawkModReadme==

#include <ntdef.h>
#include <ntstatus.h>
#include <shlobj.h>
#include <strsafe.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cstring>
#include <string>
#include <unordered_set>

constexpr wchar_t kKeyPathSuffix[] =
    L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Serialize";
constexpr wchar_t kExplorerPathSuffix[] =
    L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer";
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
wchar_t g_debugLogPath[MAX_PATH];

// Temporary diagnostics. Revert the dedicated debug commit before submission.
void InitializeDebugLog() {
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr,
                                SHGFP_TYPE_CURRENT, g_debugLogPath)) ||
        FAILED(StringCchCatW(g_debugLogPath, ARRAYSIZE(g_debugLogPath),
                             L"\\startup-app-delay-fix-hook.log"))) {
        g_debugLogPath[0] = L'\0';
    }
}

void LogDebugEvent(const char* eventName, NTSTATUS status = STATUS_SUCCESS,
                   LONG detail = 0) {
    if (!g_debugLogPath[0]) {
        return;
    }

    HANDLE file = CreateFileW(g_debugLogPath, FILE_APPEND_DATA,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    SYSTEMTIME time;
    GetLocalTime(&time);
    char line[192];
    int length = sprintf_s(
        line, sizeof(line),
        "%04u-%02u-%02u %02u:%02u:%02u.%03u  %s  status=0x%08lX detail=%ld\r\n",
        time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute,
        time.wSecond, time.wMilliseconds, eventName,
        static_cast<unsigned long>(status), detail);
    if (length > 0) {
        DWORD written;
        WriteFile(file, line, static_cast<DWORD>(length), &written, nullptr);
    }
    CloseHandle(file);
}

bool IsVirtualKey(HANDLE key) {
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

    if (IsVirtualKey(key)) {
        return true;
    }

    std::wstring path;
    if (!GetKeyPath(key, &path)) {
        return false;
    }
    size_t suffixLength = ARRAYSIZE(kKeyPathSuffix) - 1;
    constexpr size_t userHivePrefixLength = ARRAYSIZE(kUserHivePrefix) - 1;
    return path.length() >= userHivePrefixLength + suffixLength &&
           _wcsnicmp(path.c_str(), kUserHivePrefix, userHivePrefixLength) == 0 &&
           _wcsnicmp(path.c_str() + path.length() - suffixLength,
                      kKeyPathSuffix, suffixLength) == 0;
}

bool IsTargetOpen(const OBJECT_ATTRIBUTES* objectAttributes) {
    if (!objectAttributes || !objectAttributes->ObjectName ||
        !objectAttributes->ObjectName->Buffer) {
        return false;
    }

    const UNICODE_STRING* objectName = objectAttributes->ObjectName;
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

    constexpr size_t userHivePrefixLength = ARRAYSIZE(kUserHivePrefix) - 1;
    constexpr size_t suffixLength = ARRAYSIZE(kKeyPathSuffix) - 1;
    return path.length() >= userHivePrefixLength + suffixLength &&
           _wcsnicmp(path.c_str(), kUserHivePrefix, userHivePrefixLength) == 0 &&
           _wcsnicmp(path.c_str() + path.length() - suffixLength,
                      kKeyPathSuffix, suffixLength) == 0;
}

void RememberVirtualKey(HANDLE key) {
    AcquireSRWLockExclusive(&g_virtualKeyHandlesLock);
    g_virtualKeyHandles.insert(key);
    ReleaseSRWLockExclusive(&g_virtualKeyHandlesLock);
}

NTSTATUS OpenParentAsVirtualKey(PHANDLE keyHandle, ACCESS_MASK desiredAccess,
                                POBJECT_ATTRIBUTES objectAttributes,
                                ULONG openOptions, bool useNtOpenKeyEx) {
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
        DWORD options = desiredAccess & MAXIMUM_ALLOWED ? DUPLICATE_SAME_ACCESS
                                                       : 0;
        if (!DuplicateHandle(GetCurrentProcess(), objectAttributes->RootDirectory,
                             GetCurrentProcess(), &duplicate, desiredAccess,
                             FALSE, options)) {
            return STATUS_UNSUCCESSFUL;
        }
        *keyHandle = duplicate;
        RememberVirtualKey(duplicate);
        LogDebugEvent("Substituted missing Serialize key", STATUS_SUCCESS,
                      useNtOpenKeyEx);
        return STATUS_SUCCESS;
    } else {
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    UNICODE_STRING parentObjectName{
        .Length = static_cast<USHORT>(parentName.length() * sizeof(wchar_t)),
        .MaximumLength =
            static_cast<USHORT>(parentName.length() * sizeof(wchar_t)),
        .Buffer = parentName.data(),
    };
    OBJECT_ATTRIBUTES parentAttributes = *objectAttributes;
    parentAttributes.ObjectName = &parentObjectName;

    NTSTATUS status = useNtOpenKeyEx
                          ? NtOpenKeyEx_orig(keyHandle, desiredAccess,
                                             &parentAttributes, openOptions)
                          : NtOpenKey_orig(keyHandle, desiredAccess,
                                           &parentAttributes);
    if (status == STATUS_SUCCESS) {
        RememberVirtualKey(*keyHandle);
        LogDebugEvent("Substituted missing Serialize key", status,
                      useNtOpenKeyEx);
    }
    return status;
}

bool IsMissingStatus(NTSTATUS status) {
    return status == STATUS_OBJECT_NAME_NOT_FOUND ||
           status == STATUS_OBJECT_PATH_NOT_FOUND;
}

NTSTATUS NTAPI NtOpenKey_hook(PHANDLE keyHandle, ACCESS_MASK desiredAccess,
                              POBJECT_ATTRIBUTES objectAttributes) {
    NTSTATUS status =
        NtOpenKey_orig(keyHandle, desiredAccess, objectAttributes);
    if (IsMissingStatus(status) && IsTargetOpen(objectAttributes)) {
        return OpenParentAsVirtualKey(keyHandle, desiredAccess, objectAttributes,
                                      0, false);
    }
    return status;
}

NTSTATUS NTAPI NtOpenKeyEx_hook(PHANDLE keyHandle, ACCESS_MASK desiredAccess,
                                POBJECT_ATTRIBUTES objectAttributes,
                                ULONG openOptions) {
    NTSTATUS status = NtOpenKeyEx_orig(keyHandle, desiredAccess,
                                       objectAttributes, openOptions);
    if (IsMissingStatus(status) && IsTargetOpen(objectAttributes)) {
        return OpenParentAsVirtualKey(keyHandle, desiredAccess, objectAttributes,
                                      openOptions, true);
    }
    return status;
}

NTSTATUS NTAPI NtClose_hook(HANDLE handle) {
    AcquireSRWLockExclusive(&g_virtualKeyHandlesLock);
    g_virtualKeyHandles.erase(handle);
    ReleaseSRWLockExclusive(&g_virtualKeyHandlesLock);
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

NTSTATUS NTAPI NtQueryValueKey_hook(
    HANDLE keyHandle, PUNICODE_STRING valueName,
    KEY_VALUE_INFORMATION_CLASS keyValueInformationClass,
    PVOID keyValueInformation, ULONG length, PULONG resultLength) {
    NTSTATUS status = NtQueryValueKey_orig(
        keyHandle, valueName, keyValueInformationClass, keyValueInformation,
        length, resultLength);

    if (IsVirtualKey(keyHandle) && !IsTargetQuery(keyHandle, valueName)) {
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    if (!IsTargetQuery(keyHandle, valueName)) {
        return status;
    }

    if (status != STATUS_SUCCESS && status != STATUS_OBJECT_NAME_NOT_FOUND &&
        status != STATUS_OBJECT_PATH_NOT_FOUND &&
        status != STATUS_BUFFER_TOO_SMALL &&
        status != STATUS_BUFFER_OVERFLOW) {
        return status;
    }

    switch (keyValueInformationClass) {
        case KeyValueBasicInformation:
        case KeyValueFullInformation:
        case KeyValuePartialInformation:
        case KeyValueFullInformationAlign64:
            break;
        default:
            return status;
    }

    status = FillValueInformation(valueName, keyValueInformationClass,
                                  keyValueInformation, length, resultLength);
    if (status == STATUS_SUCCESS) {
        Wh_Log(L"Reporting WaitforIdleState=0");
        LogDebugEvent("Reported WaitforIdleState=0", status,
                      keyValueInformationClass);
    }
    return status;
}

BOOL Wh_ModInit() {
    InitializeDebugLog();
    LogDebugEvent("Wh_ModInit");
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto ntQueryValueKey = reinterpret_cast<NtQueryValueKey_t>(
        GetProcAddress(ntdll, "NtQueryValueKey"));
    auto ntOpenKey = reinterpret_cast<NtOpenKey_t>(
        GetProcAddress(ntdll, "NtOpenKey"));
    auto ntOpenKeyEx = reinterpret_cast<NtOpenKeyEx_t>(
        GetProcAddress(ntdll, "NtOpenKeyEx"));
    auto ntClose =
        reinterpret_cast<NtClose_t>(GetProcAddress(ntdll, "NtClose"));
    if (!ntQueryValueKey || !ntOpenKey || !ntOpenKeyEx || !ntClose) {
        Wh_Log(L"Failed to find required ntdll registry functions");
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(ntQueryValueKey, NtQueryValueKey_hook,
                                   &NtQueryValueKey_orig);
    WindhawkUtils::SetFunctionHook(ntOpenKey, NtOpenKey_hook, &NtOpenKey_orig);
    WindhawkUtils::SetFunctionHook(ntOpenKeyEx, NtOpenKeyEx_hook,
                                   &NtOpenKeyEx_orig);
    WindhawkUtils::SetFunctionHook(ntClose, NtClose_hook, &NtClose_orig);
    return TRUE;
}
