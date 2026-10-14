// ==WindhawkMod==
// @id              disable-audio-limiter
// @name            Disable Windows Audio Limiter
// @description     Disables built-in Windows Audio Limiter (CAudioLimiter)
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         audiodg.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Windows Audio Limiter

Windows has a built-in audio adjustment mechanism that decreases the volume for a few seconds after a loud click or increases if a video or audio has dropped volume.
This mod disables this adjustment.

After mod start, restart the Windows audio service, executing with admin privileges:

`net stop audiosrv`

`net start audiosrv`

or reboot.
*/
// ==/WindhawkModReadme==

#include <string>
#include <vector>
#include <algorithm>

namespace {

constexpr wchar_t kTargetPathSuffix[] =
    L"classes\\clsid\\{d69e0717-dd4b-4b25-997a-da813833b8ac}\\inprocserver32";

using NtQueryKey_t = NTSTATUS(NTAPI*)(HANDLE, int, PVOID, ULONG, PULONG);
NtQueryKey_t pNtQueryKey = nullptr;

std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    return s;
}

std::wstring AnsiToWide(LPCSTR str) {
    if (!str || !*str) return L"";
    int len = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
    std::wstring result(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, str, -1, result.data(), len);
    if (!result.empty() && result.back() == L'\0') result.pop_back();
    return result;
}

std::wstring GetKeyFullPath(HKEY hKey) {
    if (!pNtQueryKey || !hKey) return L"";

    ULONG size = 0;
    pNtQueryKey(hKey, 3 /* KeyNameInformation */, nullptr, 0, &size);
    if (!size) return L"";

    std::vector<BYTE> buffer(size);
    if (pNtQueryKey(hKey, 3, buffer.data(), size, &size) != 0) return L"";

    ULONG nameLength = *reinterpret_cast<ULONG*>(buffer.data());
    auto* namePtr = reinterpret_cast<wchar_t*>(buffer.data() + sizeof(ULONG));
    return std::wstring(namePtr, nameLength / sizeof(wchar_t));
}

bool PathMatchesTarget(const std::wstring& pathLower) {
    constexpr size_t suffixLen = std::size(kTargetPathSuffix) - 1;
    return pathLower.size() >= suffixLen &&
           pathLower.compare(pathLower.size() - suffixLen, suffixLen,
                              kTargetPathSuffix) == 0;
}

// Builds the full lowercase path for hKey (+ optional subkey) and, if it
// matches the target, logs and returns true (caller should block the call).
bool ShouldBlock(const wchar_t* apiName, HKEY hKey,
                  const std::wstring& subKey = L"") {
    std::wstring path = ToLower(GetKeyFullPath(hKey));
    if (!subKey.empty()) {
        path += L"\\" + ToLower(subKey);
    }

    if (!PathMatchesTarget(path)) return false;

    Wh_Log(L"[BLOCKED] %s (Default) on %s (PID: %lu)", apiName, path.c_str(),
           GetCurrentProcessId());
    SetLastError(ERROR_ACCESS_DENIED);
    return true;
}

bool IsDefaultValueName(LPCWSTR name) { return !name || !*name; }
bool IsDefaultValueNameA(LPCSTR name) { return !name || !*name; }

}  // namespace

// ---- RegQueryValueExW ----
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;

LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey, LPCWSTR lpValueName,
                                      LPDWORD lpReserved, LPDWORD lpType,
                                      LPBYTE lpData, LPDWORD lpcbData) {
    if (IsDefaultValueName(lpValueName) &&
        ShouldBlock(L"RegQueryValueExW", hKey)) {
        return ERROR_ACCESS_DENIED;
    }
    return RegQueryValueExW_Original(hKey, lpValueName, lpReserved, lpType,
                                      lpData, lpcbData);
}

// ---- RegQueryValueExA ----
using RegQueryValueExA_t = decltype(&RegQueryValueExA);
RegQueryValueExA_t RegQueryValueExA_Original;

LSTATUS WINAPI RegQueryValueExA_Hook(HKEY hKey, LPCSTR lpValueName,
                                      LPDWORD lpReserved, LPDWORD lpType,
                                      LPBYTE lpData, LPDWORD lpcbData) {
    if (IsDefaultValueNameA(lpValueName) &&
        ShouldBlock(L"RegQueryValueExA", hKey)) {
        return ERROR_ACCESS_DENIED;
    }
    return RegQueryValueExA_Original(hKey, lpValueName, lpReserved, lpType,
                                      lpData, lpcbData);
}

// ---- Legacy RegQueryValueW/A (also read the (Default) value) ----
using RegQueryValueW_t = decltype(&RegQueryValueW);
RegQueryValueW_t RegQueryValueW_Original;

LSTATUS WINAPI RegQueryValueW_Hook(HKEY hKey, LPCWSTR lpSubKey,
                                    LPWSTR lpValue, PLONG lpcbValue) {
    if (ShouldBlock(L"RegQueryValueW", hKey, lpSubKey ? lpSubKey : L"")) {
        return ERROR_ACCESS_DENIED;
    }
    return RegQueryValueW_Original(hKey, lpSubKey, lpValue, lpcbValue);
}

using RegQueryValueA_t = decltype(&RegQueryValueA);
RegQueryValueA_t RegQueryValueA_Original;

LSTATUS WINAPI RegQueryValueA_Hook(HKEY hKey, LPCSTR lpSubKey, LPSTR lpValue,
                                    PLONG lpcbValue) {
    if (ShouldBlock(L"RegQueryValueA", hKey, AnsiToWide(lpSubKey))) {
        return ERROR_ACCESS_DENIED;
    }
    return RegQueryValueA_Original(hKey, lpSubKey, lpValue, lpcbValue);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init: Block CLSID InProcServer32 default value read mod");

    pNtQueryKey = (NtQueryKey_t)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryKey");

    Wh_SetFunctionHook((void*)RegQueryValueExW, (void*)RegQueryValueExW_Hook,
                        (void**)&RegQueryValueExW_Original);
    Wh_SetFunctionHook((void*)RegQueryValueExA, (void*)RegQueryValueExA_Hook,
                        (void**)&RegQueryValueExA_Original);
    Wh_SetFunctionHook((void*)RegQueryValueW, (void*)RegQueryValueW_Hook,
                        (void**)&RegQueryValueW_Original);
    Wh_SetFunctionHook((void*)RegQueryValueA, (void*)RegQueryValueA_Hook,
                        (void**)&RegQueryValueA_Original);

    return TRUE;
}
