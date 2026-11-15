// ==WindhawkMod==
// @id              fix-windows-tools-view
// @name            Fix Windows Tools Folder View
// @description     Saves/restores view mode for Windows Tools folder
// @version         2.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod makes the `Windows Tools` (formerly `Administrative Tools`) folder to save and restore its view properties, like any other folder
instead of using the hardcoded tile view.
Particularly, this works around the issue that in the default, tile mode the elements of this folders have other content
instead of names displayed when using SysListView32.

The version 2.0 is based on the registry findings by the WinClassic user 04mc.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <winternl.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>

// --- Определение структур и функций NTAPI для получения пути ключа реестра ---
typedef enum _KEY_INFORMATION_CLASS_EXT {
    KeyNameInformationClass = 3
} KEY_INFORMATION_CLASS_EXT;

typedef struct _KEY_NAME_INFORMATION {
    ULONG NameLength;
    WCHAR Name[1];
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

typedef NTSTATUS(WINAPI* PNtQueryKey)(
    HANDLE                KeyHandle,
    ULONG                 KeyInformationClass,
    PVOID                 KeyInformation,
    ULONG                 Length,
    PULONG                ResultLength
);

PNtQueryKey pNtQueryKey = nullptr;

std::wstring GetKeyPath(HKEY hKey) {
    if (!pNtQueryKey) return L"";

    ULONG resultLength = 0;
    pNtQueryKey(hKey, KeyNameInformationClass, nullptr, 0, &resultLength);
    if (resultLength == 0) return L"";

    std::vector<BYTE> buffer(resultLength);
    if (pNtQueryKey(hKey, KeyNameInformationClass, buffer.data(), resultLength, &resultLength) == 0 /* STATUS_SUCCESS */) {
        auto info = (PKEY_NAME_INFORMATION)buffer.data();
        return std::wstring(info->Name, info->NameLength / sizeof(WCHAR));
    }
    return L"";
}

// --- Логика подмены ---

#define TARGET_GUID L"{D20EA4E1-3957-11D2-A40B-0C5020524153}"
#define TARGET_PATH_INSTANCE L"CLASSES\\CLSID\\" TARGET_GUID L"\\INSTANCE"
#define TARGET_PATH_INITPROP L"CLASSES\\CLSID\\" TARGET_GUID L"\\INSTANCE\\INITPROPERTYBAG"

// Быстрый фильтр (fast-path), чтобы не нагружать нецелевые запросы к реестру
bool IsCandidateValueName(LPCWSTR valueName) {
    return valueName && (_wcsicmp(valueName, L"CLSID") == 0 ||
                         _wcsicmp(valueName, L"TargetKnownFolder") == 0);
}

// Медленный путь, вызывается только для целевых параметров
bool TrySpoofRegistryValue(
    HKEY hKey, 
    LPCWSTR lpValueName, 
    LPDWORD lpType, 
    LPBYTE lpData, 
    LPDWORD lpcbData, 
    LSTATUS& statusOut
) {
    LPCWSTR fakeStr = nullptr;

    if (_wcsicmp(lpValueName, L"CLSID") == 0) {
        std::wstring path = GetKeyPath(hKey);
        std::transform(path.begin(), path.end(), path.begin(), ::towupper);
        
        // Проверяем \Instance, но исключаем подпапки (например, \InitPropertyBag)
        if (path.find(TARGET_PATH_INSTANCE) != std::wstring::npos && 
            path.find(L"INITPROPERTYBAG") == std::wstring::npos) 
        {
            fakeStr = L"{0E5AAE11-A475-4c5b-AB00-C66DE400274E}";
        }
    }
    else if (_wcsicmp(lpValueName, L"TargetKnownFolder") == 0) {
        std::wstring path = GetKeyPath(hKey);
        std::transform(path.begin(), path.end(), path.begin(), ::towupper);
        
        if (path.find(TARGET_PATH_INITPROP) != std::wstring::npos) {
            fakeStr = L"{D0384E7D-BAC3-4797-8F14-CBA229B392B5}";
        }
    }

    if (fakeStr) {
        DWORD requiredSize = (DWORD)(wcslen(fakeStr) + 1) * sizeof(WCHAR);
        
        if (lpType) {
            *lpType = REG_SZ;
        }

        if (lpData) {
            if (!lpcbData || *lpcbData < requiredSize) {
                if (lpcbData) *lpcbData = requiredSize;
                statusOut = ERROR_MORE_DATA;
                return true;
            }
            wcscpy_s((LPWSTR)lpData, *lpcbData / sizeof(WCHAR), fakeStr);
        }
        
        if (lpcbData) {
            *lpcbData = requiredSize;
        }

        statusOut = ERROR_SUCCESS;
        return true;
    }

    return false;
}

// --- Хуки ---

using RegQueryValueExW_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
RegQueryValueExW_t RegQueryValueExW_orig;

LSTATUS WINAPI RegQueryValueExW_hook(
    HKEY hKey, 
    LPCWSTR lpValueName, 
    LPDWORD lpReserved, 
    LPDWORD lpType, 
    LPBYTE lpData, 
    LPDWORD lpcbData
) {
    if (IsCandidateValueName(lpValueName)) {
        LSTATUS spoofStatus;
        if (TrySpoofRegistryValue(hKey, lpValueName, lpType, lpData, lpcbData, spoofStatus)) {
            Wh_Log(L"Spoofed RegQueryValueExW for value: %s", lpValueName);
            return spoofStatus;
        }
    }
    return RegQueryValueExW_orig(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

using RegGetValueW_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD, PVOID, LPDWORD);
RegGetValueW_t RegGetValueW_orig;

LSTATUS WINAPI RegGetValueW_hook(
    HKEY hKey, 
    LPCWSTR lpSubKey, 
    LPCWSTR lpValue, 
    DWORD dwFlags, 
    LPDWORD pdwType, 
    PVOID pvData, 
    LPDWORD pcbData
) {
    if (!IsCandidateValueName(lpValue)) {
        return RegGetValueW_orig(hKey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    }

    // Проверяем, позволяет ли вызывающий код вернуть строковый тип. 
    // Если флаг ограничивает типы (например, строго REG_DWORD), сразу отдаём оригиналу.
    if (!(dwFlags & RRF_RT_REG_SZ)) {
        return RegGetValueW_orig(hKey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    }

    HKEY hTargetKey = hKey;
    bool closedKey = false;
    LSTATUS spoofStatus;

    // Резолв ключа делаем только на медленном пути, когда точно знаем, что ключ целевой
    if (lpSubKey && lpSubKey[0] != L'\0') {
        if (RegOpenKeyExW(hKey, lpSubKey, 0, KEY_QUERY_VALUE, &hTargetKey) == ERROR_SUCCESS) {
            closedKey = true;
        } else {
            return RegGetValueW_orig(hKey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
        }
    }

    bool spoofed = TrySpoofRegistryValue(hTargetKey, lpValue, pdwType, (LPBYTE)pvData, pcbData, spoofStatus);

    if (closedKey) {
        RegCloseKey(hTargetKey);
    }

    if (spoofed) {
        Wh_Log(L"Spoofed RegGetValueW for value: %s", lpValue);
        return spoofStatus;
    }

    return RegGetValueW_orig(hKey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

// --- Инициализация мода ---
BOOL Wh_ModInit() {
    Wh_Log(L"Init Explorer Registry Spoofer");

    HMODULE hNtDll = GetModuleHandleW(L"ntdll.dll");
    if (hNtDll) {
        pNtQueryKey = (PNtQueryKey)GetProcAddress(hNtDll, "NtQueryKey");
    }

    if (!pNtQueryKey) {
        Wh_Log(L"Failed to locate NtQueryKey in ntdll.dll");
        return FALSE;
    }

    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        void* pRegQueryValueExW = (void*)GetProcAddress(hKernelBase, "RegQueryValueExW");
        if (pRegQueryValueExW) {
            Wh_SetFunctionHook(pRegQueryValueExW, (void*)RegQueryValueExW_hook, (void**)&RegQueryValueExW_orig);
        }

        void* pRegGetValueW = (void*)GetProcAddress(hKernelBase, "RegGetValueW");
        if (pRegGetValueW) {
            Wh_SetFunctionHook(pRegGetValueW, (void*)RegGetValueW_hook, (void**)&RegGetValueW_orig);
        }
    } else {
        Wh_Log(L"Failed to get handle for kernelbase.dll");
        return FALSE;
    }

    return TRUE;
}
