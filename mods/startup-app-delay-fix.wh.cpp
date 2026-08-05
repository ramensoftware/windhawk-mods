// ==WindhawkMod==
// @id              startup-app-delay-fix
// @name            Startup App Delay Fix
// @description     Removes the delay Windows applies to startup applications after sign-in
// @version         1.4
// @author          meteoni
// @github          https://github.com/Meteony
// @include         explorer.exe
// @compilerOptions -ladvapi32 -lntdll
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
Windows intentionally delays startup applications after sign-in to reduce
system load, which can cause apps to open minutes after startup.

![Key](https://raw.githubusercontent.com/Meteony/meteoni-assets/main/startup-app-delay-fix/startup-app-delay-fix.png)

This mod redirects Explorer's startup-serialization key to a dedicated,
volatile key where both `WaitforIdleState` and `StartupDelayInMSec` are zero.
It never creates, modifies, or deletes the user's real `Serialize` key.

The redirect key is in-memory only. The mod deletes it on a normal disable,
and Windows discards it when the user registry hive unloads at sign-out or
shutdown if Explorer exits unexpectedly.

The settings are read when Explorer starts, so enabling or disabling the mod
takes effect after the next sign-in or Explorer restart.

This only affects the delay Explorer applies to startup-item processing. It
doesn't change delays configured by other systems, such as Task Scheduler.
*/
// ==/WindhawkModReadme==

#include <ntdef.h>
#include <ntstatus.h>

#include <cwchar>
#include <string>
#include <string_view>
#include <vector>

constexpr wchar_t kRedirectRegistryPath[] =
    L"Software\\Windhawk_" WH_MOD_ID L"_Redirect";
constexpr wchar_t kOwnerValueName[] = L"Owner";
constexpr wchar_t kOwnerValue[] = WH_MOD_ID;
constexpr wchar_t kSerializePathSuffix[] =
    L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Serialize";
constexpr wchar_t kUserHivePrefix[] = L"\\REGISTRY\\USER\\";
constexpr const wchar_t* kDelayValueNames[] = {
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

EXTERN_C NTSYSAPI NTSTATUS NTAPI NtQueryKey(
    HANDLE keyHandle, KEY_INFORMATION_CLASS keyInformationClass,
    PVOID keyInformation, ULONG length, PULONG resultLength);

using NtOpenKey_t = NTSTATUS(NTAPI*)(PHANDLE keyHandle,
                                     ACCESS_MASK desiredAccess,
                                     POBJECT_ATTRIBUTES objectAttributes);
NtOpenKey_t NtOpenKey_orig;

using NtOpenKeyEx_t = NTSTATUS(NTAPI*)(PHANDLE keyHandle,
                                       ACCESS_MASK desiredAccess,
                                       POBJECT_ATTRIBUTES objectAttributes,
                                       ULONG openOptions);
NtOpenKeyEx_t NtOpenKeyEx_orig;

HKEY g_redirectKey;
bool g_ownsRedirectKey;

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

bool IsSerializePath(std::wstring_view path) {
    constexpr size_t userHivePrefixLength = ARRAYSIZE(kUserHivePrefix) - 1;
    constexpr size_t suffixLength = ARRAYSIZE(kSerializePathSuffix) - 1;
    return path.length() >= userHivePrefixLength + suffixLength &&
           _wcsnicmp(path.data(), kUserHivePrefix, userHivePrefixLength) == 0 &&
           _wcsnicmp(path.data() + path.length() - suffixLength,
                      kSerializePathSuffix, suffixLength) == 0;
}

bool IsSerializeOpen(const OBJECT_ATTRIBUTES* objectAttributes) {
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

    std::wstring path(name);
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

NTSTATUS DuplicateRedirectKeyHandle(PHANDLE keyHandle,
                                    ACCESS_MASK desiredAccess) {
    HANDLE duplicate = nullptr;
    DWORD options = desiredAccess & MAXIMUM_ALLOWED ? DUPLICATE_SAME_ACCESS : 0;
    if (!DuplicateHandle(GetCurrentProcess(), g_redirectKey,
                         GetCurrentProcess(), &duplicate, desiredAccess, FALSE,
                         options)) {
        switch (GetLastError()) {
            case ERROR_ACCESS_DENIED:
                return STATUS_ACCESS_DENIED;
            case ERROR_INVALID_HANDLE:
                return STATUS_INVALID_HANDLE;
            case ERROR_NOT_ENOUGH_MEMORY:
            case ERROR_OUTOFMEMORY:
                return STATUS_NO_MEMORY;
            default:
                return STATUS_UNSUCCESSFUL;
        }
    }

    *keyHandle = duplicate;
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI NtOpenKey_hook(PHANDLE keyHandle, ACCESS_MASK desiredAccess,
                              POBJECT_ATTRIBUTES objectAttributes) {
    if (IsSerializeOpen(objectAttributes)) {
        return DuplicateRedirectKeyHandle(keyHandle, desiredAccess);
    }
    return NtOpenKey_orig(keyHandle, desiredAccess, objectAttributes);
}

NTSTATUS NTAPI NtOpenKeyEx_hook(PHANDLE keyHandle, ACCESS_MASK desiredAccess,
                                POBJECT_ATTRIBUTES objectAttributes,
                                ULONG openOptions) {
    if (IsSerializeOpen(objectAttributes)) {
        return DuplicateRedirectKeyHandle(keyHandle, desiredAccess);
    }
    return NtOpenKeyEx_orig(keyHandle, desiredAccess, objectAttributes,
                            openOptions);
}

bool VerifyOwner(HKEY key) {
    wchar_t owner[ARRAYSIZE(kOwnerValue)];
    DWORD type;
    DWORD size = sizeof(owner);
    LSTATUS status = RegQueryValueExW(
        key, kOwnerValueName, nullptr, &type, reinterpret_cast<BYTE*>(owner),
        &size);
    return status == ERROR_SUCCESS && type == REG_SZ &&
           size == sizeof(kOwnerValue) &&
           wcscmp(owner, kOwnerValue) == 0;
}

bool InitializeRedirectKey() {
    DWORD disposition;
    LSTATUS status = RegCreateKeyExW(
        HKEY_CURRENT_USER, kRedirectRegistryPath, 0, nullptr,
        REG_OPTION_VOLATILE, KEY_ALL_ACCESS, nullptr, &g_redirectKey,
        &disposition);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Failed to create redirect key: %ld", status);
        return false;
    }

    bool created = disposition == REG_CREATED_NEW_KEY;
    if (!created && !VerifyOwner(g_redirectKey)) {
        Wh_Log(L"Existing redirect key isn't owned by this mod");
        RegCloseKey(g_redirectKey);
        g_redirectKey = nullptr;
        return false;
    }

    if (created) {
        status = RegSetValueExW(
            g_redirectKey, kOwnerValueName, 0, REG_SZ,
            reinterpret_cast<const BYTE*>(kOwnerValue), sizeof(kOwnerValue));
        if (status != ERROR_SUCCESS) {
            Wh_Log(L"Failed to mark redirect-key ownership: %ld", status);
            RegCloseKey(g_redirectKey);
            g_redirectKey = nullptr;
            RegDeleteKeyW(HKEY_CURRENT_USER, kRedirectRegistryPath);
            return false;
        }
    }

    DWORD zero = 0;
    for (const wchar_t* valueName : kDelayValueNames) {
        status = RegSetValueExW(
            g_redirectKey, valueName, 0, REG_DWORD,
            reinterpret_cast<const BYTE*>(&zero), sizeof(zero));
        if (status != ERROR_SUCCESS) {
            Wh_Log(L"Failed to initialize %s: %ld", valueName, status);
            RegCloseKey(g_redirectKey);
            g_redirectKey = nullptr;
            if (created) {
                RegDeleteKeyW(HKEY_CURRENT_USER, kRedirectRegistryPath);
            }
            return false;
        }
    }

    g_ownsRedirectKey = true;
    Wh_Log(L"Initialized volatile Serialize redirect key");
    return true;
}

void DeleteRedirectKey() {
    if (g_redirectKey) {
        RegCloseKey(g_redirectKey);
        g_redirectKey = nullptr;
    }
    if (!g_ownsRedirectKey) {
        return;
    }

    g_ownsRedirectKey = false;
    LSTATUS status = RegDeleteKeyW(HKEY_CURRENT_USER, kRedirectRegistryPath);
    if (status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND) {
        Wh_Log(L"Failed to delete volatile redirect key: %ld", status);
    }
}

BOOL Wh_ModInit() {
    if (!InitializeRedirectKey()) {
        return FALSE;
    }

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto ntOpenKey = reinterpret_cast<NtOpenKey_t>(
        GetProcAddress(ntdll, "NtOpenKey"));
    auto ntOpenKeyEx = reinterpret_cast<NtOpenKeyEx_t>(
        GetProcAddress(ntdll, "NtOpenKeyEx"));
    if (!ntOpenKey || !ntOpenKeyEx) {
        Wh_Log(L"Failed to find required ntdll registry functions");
        DeleteRedirectKey();
        return FALSE;
    }

    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(ntOpenKey),
                            reinterpret_cast<void*>(NtOpenKey_hook),
                            reinterpret_cast<void**>(&NtOpenKey_orig)) ||
        !Wh_SetFunctionHook(reinterpret_cast<void*>(ntOpenKeyEx),
                            reinterpret_cast<void*>(NtOpenKeyEx_hook),
                            reinterpret_cast<void**>(&NtOpenKeyEx_orig))) {
        Wh_Log(L"Failed to register registry-open hooks");
        DeleteRedirectKey();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    DeleteRedirectKey();
}
