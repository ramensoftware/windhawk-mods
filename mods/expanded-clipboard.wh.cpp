// ==WindhawkMod==
// @id              expanded-clipboard
// @name            Expanded Clipboard (Win+V)
// @description     Raises the 25-item cap of Windows clipboard history (Win+V) and lifts the per-item / total buffer size limit so large images stick.
// @version         0.7.5
// @author          takattowo
// @github          https://github.com/takattowo
// @include         svchost.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Expanded Clipboard (Win+V)

Raises the 25-entry cap on Windows 11 clipboard history (the list
shown by **Win+V**) and lifts the per-item size cap so large image
entries are kept.

## Targeting

The clipboard history service (`cbdhsvc.dll`) is hosted in a
per-user `svchost.exe`. Windhawk's `@include` has no service-level
targeting, so the engine injects this mod into every svchost
instance — but the mod only installs anything in the instance that
hosts the clipboard service, identified from the command line
(`svchost.exe -k <Group> [-p] [-s <Service>]`): an exact
`-s cbdhsvc[_*]` match, or, for unsplit instances, the `-k`
group's member list in the registry. Every other svchost instance
bails out in `Wh_ModInit` without installing anything. This avoids
breaking unrelated svchost-hosted services
(https://github.com/ramensoftware/windhawk-mods/issues/5060).

`cbdhsvc.dll` is loaded lazily, so it usually isn't loaded yet
when `Wh_ModInit` runs. In that case the mod hooks
`kernelbase!LoadLibraryExW` — only in the hosting instance — and
applies the symbol hooks as soon as the DLL appears, before the
service starts using it.

## Hook targets

Hosted in `cbdhsvc.dll`:

- `ClipboardSettingsImpl::get_ClipboardHistoryMaxItemsCount`:
  entry-count cap (default 25).
- `ClipboardSettingsImpl::get_ClipboardHistoryMaxItemSizeInBytes`:
  per-item byte cap (default 4 MiB).
- `ClipboardSettingsImpl::get_ClipboardHistoryMaxSizeInBytes`:
  total in-memory buffer cap.
- `DataPackageHelper::CanIncludeInClipboardHistory`: opt-in
  override of the per-item allow check.

## Configuration

- `maxItems` (default 100, range 25..5000): entry cap.
- `maxItemSizeMB` (default 256, range 4..4096): per-item and
  total buffer byte cap, in MB.
- `forceIncludeAll` (default off): when on, force the allow check
  to true.

## Logs

Open Windhawk Logs and filter by the mod name.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxItems: 100
  $name: Max clipboard history items
  $description: Cap of entries kept in Win+V history. Native Windows default is 25. Range 25..5000.
- maxItemSizeMB: 256
  $name: Max per-item / total buffer size (MB)
  $description: Largest single clipboard entry and total in-memory buffer. Native Windows defaults are around 4 MB each. Range 4..4096.
- forceIncludeAll: false
  $name: Force-include every clipboard item
  $description: Force DataPackageHelper::CanIncludeInClipboardHistory to return true. Useful when an app marks content as "do not include in history".
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>

#include <atomic>
#include <vector>

struct Settings {
    int maxItems = 100;
    int maxItemSizeMB = 256;
    bool forceIncludeAll = false;
};
static Settings g_settings;

static void LoadSettings() {
    g_settings.maxItems = (int)Wh_GetIntSetting(L"maxItems");
    if (g_settings.maxItems < 25)   g_settings.maxItems = 25;
    if (g_settings.maxItems > 5000) g_settings.maxItems = 5000;

    g_settings.maxItemSizeMB = (int)Wh_GetIntSetting(L"maxItemSizeMB");
    if (g_settings.maxItemSizeMB < 4)    g_settings.maxItemSizeMB = 4;
    if (g_settings.maxItemSizeMB > 4096) g_settings.maxItemSizeMB = 4096;

    g_settings.forceIncludeAll = Wh_GetIntSetting(L"forceIncludeAll") != 0;
}

using PFN_GetMaxItemsCount = HRESULT(WINAPI*)(void*, unsigned int*);
static PFN_GetMaxItemsCount g_origGetMaxItemsCount = nullptr;

static HRESULT WINAPI Hook_GetMaxItemsCount(void* thisPtr, unsigned int* out) {
    HRESULT hr = g_origGetMaxItemsCount(thisPtr, out);
    if (SUCCEEDED(hr) && out) {
        *out = (unsigned int)g_settings.maxItems;
    }
    return hr;
}

using PFN_GetMaxSize = HRESULT(WINAPI*)(void*, unsigned __int64*);
static PFN_GetMaxSize g_origGetMaxItemSize = nullptr;
static PFN_GetMaxSize g_origGetMaxSize     = nullptr;

static HRESULT WINAPI Hook_GetMaxItemSize(void* thisPtr,
                                          unsigned __int64* out) {
    HRESULT hr = g_origGetMaxItemSize(thisPtr, out);
    if (SUCCEEDED(hr) && out) {
        *out = (unsigned __int64)g_settings.maxItemSizeMB * 1024ull * 1024ull;
    }
    return hr;
}

static HRESULT WINAPI Hook_GetMaxSize(void* thisPtr, unsigned __int64* out) {
    HRESULT hr = g_origGetMaxSize(thisPtr, out);
    if (SUCCEEDED(hr) && out) {
        *out = (unsigned __int64)g_settings.maxItemSizeMB * 1024ull * 1024ull;
    }
    return hr;
}

using PFN_CanInclude = unsigned char(WINAPI*)(void*);
static PFN_CanInclude g_origCanInclude = nullptr;

static unsigned char WINAPI Hook_CanInclude(void* dataPackageView) {
    if (g_settings.forceIncludeAll) {
        return 1;
    }
    return g_origCanInclude(dataPackageView);
}

static std::atomic<bool> g_cbdhsvcHooked = false;

static bool HookCbdhsvcSymbols(HMODULE hMod) {
    WindhawkUtils::SYMBOL_HOOK cbdhsvcDllHooks[] = {
        {
            { L"public: virtual long __cdecl ClipboardSettingsImpl::get_ClipboardHistoryMaxItemsCount(unsigned int *)" },
            &g_origGetMaxItemsCount,
            Hook_GetMaxItemsCount,
        },
        {
            { L"public: virtual long __cdecl ClipboardSettingsImpl::get_ClipboardHistoryMaxItemSizeInBytes(unsigned __int64 *)" },
            &g_origGetMaxItemSize,
            Hook_GetMaxItemSize,
        },
        {
            { L"public: virtual long __cdecl ClipboardSettingsImpl::get_ClipboardHistoryMaxSizeInBytes(unsigned __int64 *)" },
            &g_origGetMaxSize,
            Hook_GetMaxSize,
        },
        {
            { L"public: static unsigned char __cdecl Windows::ApplicationModel::Internal::DataTransfer::DataPackageHelper::CanIncludeInClipboardHistory(struct Windows::ApplicationModel::DataTransfer::IDataPackageView *)" },
            &g_origCanInclude,
            Hook_CanInclude,
        },
    };

    if (!WindhawkUtils::HookSymbols(hMod, cbdhsvcDllHooks,
                                      ARRAYSIZE(cbdhsvcDllHooks))) {
        Wh_Log(L"HookSymbols failed "
               L"(PDB/symbol mismatch on this Windows build?)");
        return false;
    }

    return true;
}

// Returns true when this svchost instance hosts the clipboard
// history service (cbdhsvc). Windhawk injects the mod into every
// svchost, so this check is what confines the mod to the one
// instance that matters.
static bool SvchostHostsClipboardService() {
    // svchost.exe is launched as: svchost.exe -k <Group> [-p]
    // [-s <Service>]. The command line contains no quoted
    // arguments, so splitting a copy on whitespace is enough
    // (and avoids pulling shell32.dll into every svchost).
    wchar_t cmdLine[1024];
    wcsncpy_s(cmdLine, GetCommandLineW(), _TRUNCATE);

    wchar_t* args[16] = {};
    int argc = 0;
    wchar_t* context = nullptr;
    for (wchar_t* token = wcstok_s(cmdLine, L" \t", &context);
         token && argc < _countof(args);
         token = wcstok_s(nullptr, L" \t", &context)) {
        args[argc++] = token;
    }

    // "-s <Service>" identifies the service exactly when svchost
    // splits the group into per-service instances. It comes
    // after -k, so it must be checked first.
    for (int i = 1; i + 1 < argc; ++i) {
        if (_wcsicmp(args[i], L"-s") == 0) {
            const wchar_t* service = args[i + 1];
            return _wcsnicmp(service, L"cbdhsvc", 7) == 0 &&
                   (service[7] == L'\0' || service[7] == L'_');
        }
    }

    // Without -s, the instance hosts every service in the -k
    // group, whose member list is a REG_MULTI_SZ value under
    // HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Svchost.
    for (int i = 1; i + 1 < argc; ++i) {
        if (_wcsicmp(args[i], L"-k") == 0) {
            HKEY hKey;
            if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                    L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost",
                    0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                bool hosts = false;
                DWORD type = REG_NONE, size = 0;
                if (RegQueryValueExW(hKey, args[i + 1], nullptr,
                        &type, nullptr, &size) == ERROR_SUCCESS
                    && type == REG_MULTI_SZ && size != 0) {
                    // Pad the buffer so the double-null
                    // terminator is guaranteed even if the
                    // registry data isn't properly terminated.
                    std::vector<wchar_t> services(
                        size / sizeof(wchar_t) + 2);
                    if (RegQueryValueExW(hKey, args[i + 1], nullptr,
                            nullptr,
                            reinterpret_cast<LPBYTE>(services.data()),
                            &size) == ERROR_SUCCESS) {
                        for (const wchar_t* service = services.data();
                             *service; service += wcslen(service) + 1) {
                            if (_wcsicmp(service, L"cbdhsvc") == 0) {
                                hosts = true;
                                break;
                            }
                        }
                    }
                }
                RegCloseKey(hKey);
                return hosts;
            }
            return false;
        }
    }

    return false;
}

// --- Late-load handling ------------------------------------------------
//
// cbdhsvc.dll is loaded lazily by the service control manager, so
// it usually isn't loaded yet when Wh_ModInit runs. The
// LoadLibraryExW hook below catches the load — but only after the
// service-group check above confirmed this svchost is the
// clipboard host, so no other svchost instance is ever touched.
// The work runs after the original call returns, so the loader
// lock is released and the hooks are applied before the service
// starts using the DLL.

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original;

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                          HANDLE hFile,
                                          DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile,
                                               dwFlags);
    DWORD lastError = GetLastError();

    if (module && !g_cbdhsvcHooked.load() &&
        GetModuleHandleW(L"cbdhsvc.dll") == module &&
        !g_cbdhsvcHooked.exchange(true)) {
        if (HookCbdhsvcSymbols(module)) {
            Wh_ApplyHookOperations();
            Wh_Log(L"hooks active (caught late load)");
        }
    }

    SetLastError(lastError);
    return module;
}

// ----------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    // Only the svchost instance hosting the clipboard history
    // service is relevant. Bail out everywhere else without
    // installing anything — hooks in unrelated svchost
    // instances break other services (issue #5060).
    HMODULE hMod = GetModuleHandleW(L"cbdhsvc.dll");
    if (!hMod && !SvchostHostsClipboardService()) {
        return FALSE;
    }

    Wh_Log(L"Init. maxItems=%d, maxItemSizeMB=%d, "
           L"forceIncludeAll=%d",
           g_settings.maxItems, g_settings.maxItemSizeMB,
           g_settings.forceIncludeAll ? 1 : 0);

    if (hMod) {
        // Hook operations are applied implicitly when
        // Wh_ModInit returns TRUE.
        g_cbdhsvcHooked = true;
        return HookCbdhsvcSymbols(hMod);
    }

    // This svchost hosts the clipboard service, but cbdhsvc.dll
    // hasn't been loaded yet. Catch the load.
    HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW =
        (decltype(&LoadLibraryExW))GetProcAddress(
            kernelBaseModule, "LoadLibraryExW");
    if (pKernelBaseLoadLibraryExW) {
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    // Covers a load that happened between Wh_ModInit and the
    // LoadLibraryExW hook becoming active.
    HMODULE hMod = GetModuleHandleW(L"cbdhsvc.dll");
    if (hMod && !g_cbdhsvcHooked.exchange(true)) {
        if (HookCbdhsvcSymbols(hMod)) {
            Wh_ApplyHookOperations();
            Wh_Log(L"hooks active (ModAfterInit)");
        }
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings reloaded. maxItems=%d, maxItemSizeMB=%d, "
           L"forceIncludeAll=%d",
           g_settings.maxItems, g_settings.maxItemSizeMB,
           g_settings.forceIncludeAll ? 1 : 0);
}

void Wh_ModUninit() {
}
