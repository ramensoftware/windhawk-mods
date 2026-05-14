// ==WindhawkMod==
// @id              expanded-clipboard
// @name            Expanded Clipboard (Win+V)
// @description     Raises the 25-item cap of Windows clipboard history (Win+V) and lifts the per-item / total buffer size limit so large images stick.
// @version         0.7.1
// @author          takattowo
// @github          https://github.com/takattowo
// @include         svchost.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Expanded Clipboard (Win+V)

Raises the 25-entry cap on Windows 11 clipboard history (the list shown by
**Win+V**) and lifts the per-item size cap so large image entries are kept.

## Hook targets

Hosted in `cbdhsvc.dll` (per-user `svchost.exe`, service group
`UnistackSvcGroup`). All hooks are virtual COM methods on `ClipboardSettingsImpl`
plus one static helper:

- `get_ClipboardHistoryMaxItemsCount`: entry-count cap (default 25).
- `get_ClipboardHistoryMaxItemSizeInBytes`: per-item byte cap (default 4 MiB).
- `get_ClipboardHistoryMaxSizeInBytes`: total in-memory buffer cap. Empirically
  this is the gate the inner add path checks first.
- `DataPackageHelper::CanIncludeInClipboardHistory`: opt-in override of the
  per-item allow check.

## Configuration

- `maxItems` (default 100, range 25..5000): entry cap.
- `maxItemSizeMB` (default 256, range 4..4096): per-item and total buffer byte
  cap, in MB. Both caps share this value.
- `forceIncludeAll` (default off): when on, force the allow check to true so
  items marked "do not include in history" by their source app still land in
  Win+V.

## Logs

Open Windhawk Logs. Filter `[ExpandedClipboard]`.
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

static std::atomic<bool> g_cbdhsvcDllHooked = false;

static bool HookCbdhsvcDllSymbols(HMODULE hMod) {
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

    if (!WindhawkUtils::HookSymbols(hMod, cbdhsvcDllHooks, ARRAYSIZE(cbdhsvcDllHooks))) {
        Wh_Log(L"[ExpandedClipboard] HookSymbols failed");
        return false;
    }

    return true;
}

static void HandleLoadedModuleIfCbdhsvc(HMODULE module, LPCWSTR lpLibFileName) {
    if (g_cbdhsvcDllHooked) {
        return;
    }

    if (GetModuleHandleW(L"cbdhsvc.dll") != module) {
        return;
    }

    if (g_cbdhsvcDllHooked.exchange(true)) {
        return;
    }

    Wh_Log(L"[ExpandedClipboard] cbdhsvc.dll loaded (%s)",
           lpLibFileName ? lpLibFileName : L"?");

    if (HookCbdhsvcDllSymbols(module)) {
        Wh_ApplyHookOperations();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original;
static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                          HANDLE hFile,
                                          DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfCbdhsvc(module, lpLibFileName);
    }
    return module;
}

BOOL Wh_ModInit() {
    LoadSettings();

    Wh_Log(L"[ExpandedClipboard] Init. maxItems=%d, maxItemSizeMB=%d, "
           L"forceIncludeAll=%d",
           g_settings.maxItems, g_settings.maxItemSizeMB,
           g_settings.forceIncludeAll ? 1 : 0);

    if (HMODULE hMod = GetModuleHandleW(L"cbdhsvc.dll")) {
        g_cbdhsvcDllHooked = true;
        if (!HookCbdhsvcDllSymbols(hMod)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"[ExpandedClipboard] cbdhsvc.dll not loaded yet, waiting");
    }

    HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW =
        (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                  "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_cbdhsvcDllHooked) {
        return;
    }

    if (HMODULE hMod = GetModuleHandleW(L"cbdhsvc.dll")) {
        if (!g_cbdhsvcDllHooked.exchange(true)) {
            Wh_Log(L"[ExpandedClipboard] cbdhsvc.dll loaded post-init");
            if (HookCbdhsvcDllSymbols(hMod)) {
                Wh_ApplyHookOperations();
            }
        }
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"[ExpandedClipboard] Settings reloaded. "
           L"maxItems=%d, maxItemSizeMB=%d, forceIncludeAll=%d",
           g_settings.maxItems, g_settings.maxItemSizeMB,
           g_settings.forceIncludeAll ? 1 : 0);
}

void Wh_ModUninit() {
}
