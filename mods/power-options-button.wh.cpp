// ==WindhawkMod==
// @id              power-options-button
// @name            Power Options Button
// @description     Tray icon to switch between Windows power plans with the mouse wheel.
// @version         1.0.14
// @author          SilverAmd
// @github          https://github.com/SilverAmd
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32 -lgdi32 -lpowrprof -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Power Options Button

Adds a tray icon for switching between Windows power plans.

## Preview

![Power Options Button preview](https://i.imgur.com/26A3Ilv.gif)

*Shows the tray icon, tooltip, right-click menu, and mouse-wheel switching between power plans.*

## Features

- Tray icon shows the currently active Windows power plan.
- Tooltip shows the active plan.
- Mouse wheel over the tray icon cycles between:
  - Power Saver
  - Balanced
  - High Performance
  - Ultimate Performance
- Right-click opens a context menu.
- Icon color changes depending on the active plan:
  - Green = Power Saver
  - Yellow = Balanced
  - Red = High Performance
  - Violet = Ultimate Performance
- If Ultimate Performance is not available, it is shown as disabled and can be created from the tray menu with administrator approval.
- Automatically restores the tray icon after Explorer/taskbar restart.

## Usage

- Scroll up over the tray icon: next power plan.
- Scroll down over the tray icon: previous power plan.
- Right-click the tray icon: choose a power plan manually.
- Left-click the tray icon: refresh current status.
- Use "Create Ultimate Performance..." from the tray menu to create the Ultimate Performance plan if it is not available yet.
*/
// ==/WindhawkModReadme==

// Important:
// The low-level mouse hook runs on a dedicated message-pumping thread. This
// keeps blocking UI work from delaying the hook long enough for Windows to
// silently remove it because of LowLevelHooksTimeout.

#include <windows.h>
#include <shellapi.h>
#include <powrprof.h>
#include <strsafe.h>

#ifndef NIF_SHOWTIP
#define NIF_SHOWTIP 0x00000080
#endif

#ifndef NIN_SELECT
#define NIN_SELECT (WM_USER + 0)
#endif

#ifndef NIN_KEYSELECT
#define NIN_KEYSELECT (NIN_SELECT | 0x1)
#endif

#define WM_TRAYICON        (WM_USER + 1)
#define WM_TRAY_SCROLL     (WM_APP + 1)
#define WM_APP_EXIT        (WM_APP + 2)

#define ID_TRAYICON               24001
#define ID_MENU_POWER_SAVER       3001
#define ID_MENU_BALANCED          3002
#define ID_MENU_HIGH_PERFORMANCE  3003
#define ID_MENU_ULTIMATE          3004
#define ID_MENU_REFRESH           3005
#define ID_MENU_OPEN_POWER        3006
#define ID_MENU_OPEN_SETTINGS     3007
#define ID_MENU_OPEN_WINDHAWK     3008
#define ID_MENU_CREATE_ULTIMATE   3009

#define HIDDEN_WINDOW_CLASS L"PowerOptionsButtonHiddenWindow"

#define TIMER_TRAY_RECT_REFRESH 4001
#define TIMER_POST_START_ICON_SYNC 4002
#define TIMER_ACTIVE_POWER_PLAN_REFRESH 4003
#define ACTIVE_POWER_PLAN_REFRESH_INTERVAL_MS 2000
#define TRAY_RECT_REFRESH_INTERVAL_MS 500
#define TRAY_RECT_REFRESH_MAX_ATTEMPTS 20
#define POST_START_ICON_SYNC_DELAY_MS 1000
#define WM_APP_POWERCFG_FALLBACK_DONE  (WM_APP + 3)
#define WM_APP_CREATE_ULTIMATE_DONE    (WM_APP + 4)

static HWND g_hwnd = nullptr;
static HICON g_hIcon = nullptr;
static NOTIFYICONDATAW g_nid = {};
static UINT g_taskbarCreatedMessage = 0;

static HHOOK g_mouseHook = nullptr;
static HANDLE g_mouseHookThread = nullptr;
static DWORD g_mouseHookThreadId = 0;

static SRWLOCK g_trayStateLock = SRWLOCK_INIT;
static RECT g_trayIconRect = {};
static RECT g_trayFallbackAreas[2] = {};
static int g_trayFallbackAreaCount = 0;
static DWORD g_lastTrayMouseMoveTime = 0;
static DWORD g_lastScrollTime = 0;
static GUID g_lastSeenPowerSchemeGuid = {};
static bool g_hasLastSeenPowerSchemeGuid = false;

static HANDLE g_uiThread = nullptr;
static DWORD g_uiThreadId = 0;

static WCHAR g_windhawkPath[MAX_PATH] = {};

static const GUID GUID_POWER_SAVER =
    {0xa1841308, 0x3541, 0x4fab, {0xbc, 0x81, 0xf7, 0x15, 0x56, 0xf2, 0x0b, 0x4a}};

static const GUID GUID_BALANCED =
    {0x381b4222, 0xf694, 0x41f0, {0x96, 0x85, 0xff, 0x5b, 0xb2, 0x60, 0xdf, 0x2e}};

static const GUID GUID_HIGH_PERFORMANCE =
    {0x8c5e7fda, 0xe8bf, 0x4a96, {0x9a, 0x85, 0xa6, 0xe2, 0x3a, 0x8c, 0x63, 0x5c}};

static const GUID GUID_ULTIMATE_PERFORMANCE =
    {0xe9a42b02, 0xd5df, 0x448d, {0xaa, 0x00, 0x03, 0xf1, 0x47, 0x49, 0xeb, 0x61}};

struct PowerPlanInfo {
    const WCHAR* name;
    const WCHAR* shortName;
    const WCHAR* guidString;
    GUID guid;
    COLORREF color;
    int menuId;
};

static PowerPlanInfo g_plans[] = {
    {
        L"Power Saver",
        L"P",
        L"a1841308-3541-4fab-bc81-f71556f20b4a",
        GUID_POWER_SAVER,
        RGB(0, 190, 80),
        ID_MENU_POWER_SAVER
    },
    {
        L"Balanced",
        L"B",
        L"381b4222-f694-41f0-9685-ff5bb260df2e",
        GUID_BALANCED,
        RGB(255, 210, 0),
        ID_MENU_BALANCED
    },
    {
        L"High Performance",
        L"H",
        L"8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c",
        GUID_HIGH_PERFORMANCE,
        RGB(230, 40, 40),
        ID_MENU_HIGH_PERFORMANCE
    },
    {
        L"Ultimate Performance",
        L"U",
        L"e9a42b02-d5df-448d-aa00-03f14749eb61",
        GUID_ULTIMATE_PERFORMANCE,
        RGB(160, 70, 255),
        ID_MENU_ULTIMATE
    },
};

static const int g_planCount = ARRAYSIZE(g_plans);
static int g_currentPlanIndex = 1;
static bool g_ultimateAvailable = false;
static GUID g_ultimateRuntimeGuid =
    {0xe9a42b02, 0xd5df, 0x448d, {0xaa, 0x00, 0x03, 0xf1, 0x47, 0x49, 0xeb, 0x61}};
static WCHAR g_ultimateRuntimeName[128] = L"Ultimate Performance";
static WCHAR g_customPowerPlanName[128] = L"Unknown/custom power plan";
static int g_trayRectRefreshAttempts = 0;

static bool g_powercfgFallbackRunning = false;
static bool g_createUltimateRunning = false;

#define MAX_POWER_SCHEME_SNAPSHOT 128
#define ULTIMATE_RUNTIME_GUID_VALUE_NAME L"ultimateRuntimeGuid"

struct PowerSchemeSnapshot {
    GUID guids[MAX_POWER_SCHEME_SNAPSHOT];
    int count;
};

struct CreateUltimateResult {
    bool success;
    bool hasNewGuid;
    GUID newGuid;
};

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

bool GuidEquals(const GUID& a, const GUID& b) {
    return IsEqualGUID(a, b) != FALSE;
}

bool IsValidPlanIndex(int index) {
    return index >= 0 && index < g_planCount;
}

PCWSTR GetPlanDisplayName(int index) {
    if (IsValidPlanIndex(index)) {
        return g_plans[index].name;
    }

    return g_customPowerPlanName;
}

bool GetActivePowerSchemeGuid(GUID* guid) {
    if (!guid) {
        return false;
    }

    GUID* activeGuid = nullptr;

    DWORD result = PowerGetActiveScheme(nullptr, &activeGuid);
    if (result != ERROR_SUCCESS || !activeGuid) {
        Wh_Log(L"PowerGetActiveScheme failed. Error: %u", result);
        return false;
    }

    *guid = *activeGuid;
    LocalFree(activeGuid);

    return true;
}

void RememberActivePowerSchemeGuid() {
    GUID activeGuid = {};

    if (GetActivePowerSchemeGuid(&activeGuid)) {
        g_lastSeenPowerSchemeGuid = activeGuid;
        g_hasLastSeenPowerSchemeGuid = true;
    }
}

void CapturePowerSchemeSnapshot(PowerSchemeSnapshot* snapshot) {
    if (!snapshot) {
        return;
    }

    snapshot->count = 0;

    for (ULONG index = 0;; index++) {
        GUID schemeGuid = {};
        DWORD size = sizeof(schemeGuid);

        DWORD result = PowerEnumerate(
            nullptr,
            nullptr,
            nullptr,
            ACCESS_SCHEME,
            index,
            reinterpret_cast<UCHAR*>(&schemeGuid),
            &size
        );

        if (result == ERROR_NO_MORE_ITEMS) {
            break;
        }

        if (result != ERROR_SUCCESS) {
            Wh_Log(L"PowerEnumerate failed while capturing scheme snapshot. Error: %u", result);
            break;
        }

        if (snapshot->count >= MAX_POWER_SCHEME_SNAPSHOT) {
            Wh_Log(L"Power scheme snapshot limit reached.");
            break;
        }

        snapshot->guids[snapshot->count++] = schemeGuid;
    }
}

bool IsGuidInSnapshot(const PowerSchemeSnapshot* snapshot, const GUID& guid) {
    if (!snapshot) {
        return false;
    }

    for (int i = 0; i < snapshot->count; i++) {
        if (GuidEquals(snapshot->guids[i], guid)) {
            return true;
        }
    }

    return false;
}

bool FindNewPowerSchemeAfterSnapshot(const PowerSchemeSnapshot* before, GUID* newGuid) {
    if (!before || !newGuid) {
        return false;
    }

    for (ULONG index = 0;; index++) {
        GUID schemeGuid = {};
        DWORD size = sizeof(schemeGuid);

        DWORD result = PowerEnumerate(
            nullptr,
            nullptr,
            nullptr,
            ACCESS_SCHEME,
            index,
            reinterpret_cast<UCHAR*>(&schemeGuid),
            &size
        );

        if (result == ERROR_NO_MORE_ITEMS) {
            break;
        }

        if (result != ERROR_SUCCESS) {
            Wh_Log(L"PowerEnumerate failed while searching for new scheme. Error: %u", result);
            break;
        }

        if (!IsGuidInSnapshot(before, schemeGuid)) {
            *newGuid = schemeGuid;
            return true;
        }
    }

    return false;
}

void GuidToStringNoBraces(const GUID& guid, WCHAR* buffer, size_t bufferCount) {
    StringCchPrintfW(
        buffer,
        bufferCount,
        L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        guid.Data1,
        guid.Data2,
        guid.Data3,
        guid.Data4[0],
        guid.Data4[1],
        guid.Data4[2],
        guid.Data4[3],
        guid.Data4[4],
        guid.Data4[5],
        guid.Data4[6],
        guid.Data4[7]
    );
}

bool ReadPowerSchemeFriendlyNameFromRegistry(
    const GUID& guid,
    WCHAR* name,
    DWORD nameCount,
    bool raw
) {
    if (!name || nameCount == 0) {
        return false;
    }

    name[0] = L'\0';

    WCHAR guidText[64] = {};
    GuidToStringNoBraces(guid, guidText, ARRAYSIZE(guidText));

    WCHAR keyPath[256] = {};
    HRESULT hr = StringCchPrintfW(
        keyPath,
        ARRAYSIZE(keyPath),
        L"SYSTEM\\CurrentControlSet\\Control\\Power\\User\\PowerSchemes\\%s",
        guidText
    );

    if (FAILED(hr)) {
        return false;
    }

    // RRF_RT_REG_SZ also accepts REG_EXPAND_SZ and expands it. For the raw
    // comparison path, explicitly allow REG_EXPAND_SZ and suppress expansion.
    DWORD flags = RRF_RT_REG_SZ | RRF_ZEROONFAILURE;
    if (raw) {
        flags |= RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND;
    }

    DWORD type = 0;
    DWORD sizeBytes = nameCount * sizeof(WCHAR);

    LONG result = RegGetValueW(
        HKEY_LOCAL_MACHINE,
        keyPath,
        L"FriendlyName",
        flags,
        &type,
        name,
        &sizeBytes
    );

    if (result != ERROR_SUCCESS && !raw) {
        Wh_Log(
            L"Failed to read custom power plan name from registry. GUID: %s, Error: %ld",
            guidText,
            result
        );
    }

    return result == ERROR_SUCCESS && name[0];
}

bool IsRunningUnderWow64() {
    BOOL isWow64 = FALSE;

    if (!IsWow64Process(GetCurrentProcess(), &isWow64)) {
        return false;
    }

    return isWow64 != FALSE;
}

bool BuildSystemExecutablePath(PCWSTR exeName, WCHAR* buffer, size_t bufferCount) {
    if (!exeName || !buffer || bufferCount == 0) {
        return false;
    }

    buffer[0] = L'\0';

    WCHAR systemPath[MAX_PATH] = {};

    if (IsRunningUnderWow64()) {
        UINT len = GetWindowsDirectoryW(systemPath, ARRAYSIZE(systemPath));
        if (len == 0 || len >= ARRAYSIZE(systemPath)) {
            return false;
        }

        return SUCCEEDED(StringCchPrintfW(
            buffer,
            bufferCount,
            L"%s\\Sysnative\\%s",
            systemPath,
            exeName
        ));
    }

    UINT len = GetSystemDirectoryW(systemPath, ARRAYSIZE(systemPath));
    if (len == 0 || len >= ARRAYSIZE(systemPath)) {
        return false;
    }

    return SUCCEEDED(StringCchPrintfW(
        buffer,
        bufferCount,
        L"%s\\%s",
        systemPath,
        exeName
    ));
}

bool ReadPowerSchemeFriendlyName(const GUID& guid, WCHAR* name, DWORD nameCount) {
    if (!name || nameCount == 0) {
        return false;
    }

    name[0] = L'\0';

    DWORD sizeBytes = nameCount * sizeof(WCHAR);

    DWORD result = PowerReadFriendlyName(
        nullptr,
        &guid,
        nullptr,
        nullptr,
        reinterpret_cast<UCHAR*>(name),
        &sizeBytes
    );

    return result == ERROR_SUCCESS && name[0];
}

void UpdateCustomPowerPlanNameFromGuid(const GUID& guid) {
    WCHAR friendlyName[128] = {};

    if ((ReadPowerSchemeFriendlyName(
             guid,
             friendlyName,
             ARRAYSIZE(friendlyName)) &&
         friendlyName[0]) ||
        (ReadPowerSchemeFriendlyNameFromRegistry(
             guid,
             friendlyName,
             ARRAYSIZE(friendlyName),
             false) &&
         friendlyName[0])) {
        StringCchCopyW(
            g_customPowerPlanName,
            ARRAYSIZE(g_customPowerPlanName),
            friendlyName
        );
    } else {
        StringCchCopyW(
            g_customPowerPlanName,
            ARRAYSIZE(g_customPowerPlanName),
            L"Unknown/custom power plan"
        );
    }
}

bool IsEnumeratedPowerSchemePresent(const GUID& wantedGuid) {
    for (ULONG index = 0;; index++) {
        GUID schemeGuid = {};
        DWORD size = sizeof(schemeGuid);

        DWORD result = PowerEnumerate(
            nullptr,
            nullptr,
            nullptr,
            ACCESS_SCHEME,
            index,
            reinterpret_cast<UCHAR*>(&schemeGuid),
            &size
        );

        if (result == ERROR_NO_MORE_ITEMS) {
            break;
        }

        if (result != ERROR_SUCCESS) {
            Wh_Log(L"PowerEnumerate failed while checking schemes. Error: %u", result);
            break;
        }

        if (GuidEquals(schemeGuid, wantedGuid)) {
            return true;
        }
    }

    return false;
}

void PersistUltimateRuntimeGuid(const GUID& guid) {
    WCHAR guidText[64] = {};
    GuidToStringNoBraces(guid, guidText, ARRAYSIZE(guidText));

    if (!Wh_SetStringValue(ULTIMATE_RUNTIME_GUID_VALUE_NAME, guidText)) {
        Wh_Log(L"Failed to persist Ultimate Performance runtime GUID: %s", guidText);
    }
}

void RefreshUltimatePerformanceAvailability() {
    g_ultimateAvailable = false;
    g_ultimateRuntimeGuid = GUID_ULTIMATE_PERFORMANCE;
    StringCchCopyW(g_ultimateRuntimeName, ARRAYSIZE(g_ultimateRuntimeName), L"Ultimate Performance");

    // Keep the last detected/created runtime GUID as a language-independent
    // safety net. This also lets a duplicated Ultimate plan remain detectable
    // after the user renames it with powercfg -changename.
    WCHAR persistedGuidText[64] = {};
    bool havePersistedGuid =
        Wh_GetStringValue(
            ULTIMATE_RUNTIME_GUID_VALUE_NAME,
            persistedGuidText,
            ARRAYSIZE(persistedGuidText)) != 0;

    // powercfg /duplicatescheme creates a new runtime GUID. The duplicated
    // Ultimate Performance scheme keeps the same raw FriendlyName MUI
    // reference as the built-in Ultimate Performance template. Compare that
    // reference instead of matching localized display-name keywords.
    WCHAR rawReferenceName[256] = {};
    bool haveRawReferenceName = ReadPowerSchemeFriendlyNameFromRegistry(
        GUID_ULTIMATE_PERFORMANCE,
        rawReferenceName,
        ARRAYSIZE(rawReferenceName),
        true
    );

    // Secondary language-independent fallback. PowerReadFriendlyName resolves
    // the MUI resource into the current Windows display language. Use an exact
    // comparison only; never use substring matching here.
    WCHAR localizedReferenceName[128] = {};
    bool haveLocalizedReferenceName = ReadPowerSchemeFriendlyName(
        GUID_ULTIMATE_PERFORMANCE,
        localizedReferenceName,
        ARRAYSIZE(localizedReferenceName)
    );

    if (!haveRawReferenceName) {
        Wh_Log(L"Ultimate Performance raw FriendlyName reference is unavailable.");
    }

    if (!haveLocalizedReferenceName) {
        Wh_Log(L"Ultimate Performance localized FriendlyName reference is unavailable.");
    }

    bool enumerationCompleted = false;

    for (ULONG index = 0;; index++) {
        GUID schemeGuid = {};
        DWORD size = sizeof(schemeGuid);

        DWORD result = PowerEnumerate(
            nullptr,
            nullptr,
            nullptr,
            ACCESS_SCHEME,
            index,
            reinterpret_cast<UCHAR*>(&schemeGuid),
            &size
        );

        if (result == ERROR_NO_MORE_ITEMS) {
            enumerationCompleted = true;
            break;
        }

        if (result != ERROR_SUCCESS) {
            Wh_Log(L"PowerEnumerate failed while searching Ultimate Performance. Error: %u", result);
            break;
        }

        WCHAR schemeGuidText[64] = {};
        GuidToStringNoBraces(schemeGuid, schemeGuidText, ARRAYSIZE(schemeGuidText));

        WCHAR friendlyName[128] = {};
        ReadPowerSchemeFriendlyName(schemeGuid, friendlyName, ARRAYSIZE(friendlyName));

        bool isUltimate = false;
        PCWSTR detectionMethod = nullptr;

        if (havePersistedGuid &&
            _wcsicmp(schemeGuidText, persistedGuidText) == 0) {
            isUltimate = true;
            detectionMethod = L"persisted runtime GUID";
        }

        if (!isUltimate && GuidEquals(schemeGuid, GUID_ULTIMATE_PERFORMANCE)) {
            isUltimate = true;
            detectionMethod = L"template GUID";
        }

        if (!isUltimate && haveRawReferenceName) {
            WCHAR rawFriendlyName[256] = {};

            if (ReadPowerSchemeFriendlyNameFromRegistry(
                    schemeGuid,
                    rawFriendlyName,
                    ARRAYSIZE(rawFriendlyName),
                    true) &&
                _wcsicmp(rawFriendlyName, rawReferenceName) == 0) {
                isUltimate = true;
                detectionMethod = L"raw FriendlyName reference";
            }
        }

        if (!isUltimate &&
            haveLocalizedReferenceName &&
            friendlyName[0] &&
            _wcsicmp(friendlyName, localizedReferenceName) == 0) {
            isUltimate = true;
            detectionMethod = L"localized FriendlyName";
        }

        if (isUltimate) {
            g_ultimateAvailable = true;
            g_ultimateRuntimeGuid = schemeGuid;

            if (friendlyName[0]) {
                StringCchCopyW(
                    g_ultimateRuntimeName,
                    ARRAYSIZE(g_ultimateRuntimeName),
                    friendlyName
                );
            }

            if (!havePersistedGuid ||
                _wcsicmp(schemeGuidText, persistedGuidText) != 0) {
                PersistUltimateRuntimeGuid(g_ultimateRuntimeGuid);
            }

            Wh_Log(
                L"Ultimate Performance detected via %s. Name: %s, GUID: %s",
                detectionMethod,
                g_ultimateRuntimeName,
                schemeGuidText
            );

            return;
        }
    }

    if (havePersistedGuid && enumerationCompleted) {
        Wh_Log(
            L"Persisted Ultimate Performance runtime GUID is no longer present: %s",
            persistedGuidText
        );
        Wh_SetStringValue(ULTIMATE_RUNTIME_GUID_VALUE_NAME, L"");
    }

    Wh_Log(L"Ultimate Performance is not available as an active power scheme.");
}

GUID GetRuntimePlanGuid(int index) {
    if (index == 3 && g_ultimateAvailable) {
        return g_ultimateRuntimeGuid;
    }

    return g_plans[index].guid;
}

int FindPlanIndexByGuid(const GUID& guid);

bool IsPowerPlanAvailable(const GUID& guid) {
    int index = FindPlanIndexByGuid(guid);

    if (index < 0) {
        return false;
    }

    if (index == 3) {
        return g_ultimateAvailable;
    }

    return IsEnumeratedPowerSchemePresent(g_plans[index].guid);
}

bool SetPowerPlanWithPowercfgWorker(int index) {
    if (index < 0 || index >= g_planCount) {
        return false;
    }

    GUID runtimeGuid = GetRuntimePlanGuid(index);

    WCHAR guidText[64] = {};
    GuidToStringNoBraces(runtimeGuid, guidText, ARRAYSIZE(guidText));
    WCHAR powercfgPath[MAX_PATH] = {};
    if (!BuildSystemExecutablePath(L"powercfg.exe", powercfgPath, ARRAYSIZE(powercfgPath))) {
        Wh_Log(L"Failed to build full path for powercfg.exe.");
        return false;
    }

    WCHAR cmdLine[512] = {};
    StringCchPrintfW(
        cmdLine,
        ARRAYSIZE(cmdLine),
        L"\"%s\" /setactive %s",
        powercfgPath,
        guidText
    );

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = {};

    BOOL created = CreateProcessW(
        powercfgPath,
        cmdLine,
        nullptr,
        nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (!created) {
        Wh_Log(
            L"CreateProcessW failed for powercfg fallback: %s. Error: %u",
            g_plans[index].name,
            GetLastError()
        );
        return false;
    }

    WaitForSingleObject(pi.hProcess, 5000);

    DWORD exitCode = 1;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    if (exitCode == 0) {
        Wh_Log(
            L"powercfg fallback changed active power plan to: %s",
            g_plans[index].name
        );
        return true;
    }

    Wh_Log(
        L"powercfg fallback failed for %s. Exit code: %u",
        g_plans[index].name,
        exitCode
    );

    return false;
}

DWORD WINAPI PowercfgFallbackThreadProc(LPVOID param) {
    int index = static_cast<int>(reinterpret_cast<INT_PTR>(param));

    bool success = SetPowerPlanWithPowercfgWorker(index);

    PostMessageW(
        g_hwnd,
        WM_APP_POWERCFG_FALLBACK_DONE,
        success ? TRUE : FALSE,
        index
    );

    return 0;
}

bool StartPowercfgFallbackWorker(int index) {
    if (g_powercfgFallbackRunning) {
        Wh_Log(L"powercfg fallback already running.");
        return false;
    }

    g_powercfgFallbackRunning = true;

    HANDLE thread = CreateThread(
        nullptr,
        0,
        PowercfgFallbackThreadProc,
        reinterpret_cast<LPVOID>(static_cast<INT_PTR>(index)),
        0,
        nullptr
    );

    if (!thread) {
        g_powercfgFallbackRunning = false;
        Wh_Log(L"Failed to start powercfg fallback worker. Error: %u", GetLastError());
        return false;
    }

    CloseHandle(thread);
    return true;
}

int FindPlanIndexByGuid(const GUID& guid) {
    for (int i = 0; i < g_planCount; i++) {
        if (GuidEquals(guid, g_plans[i].guid)) {
            return i;
        }
    }

    if (g_ultimateAvailable && GuidEquals(guid, g_ultimateRuntimeGuid)) {
        return 3;
    }

    return -1;
}

int GetCurrentPowerPlanIndex() {
    GUID* activeGuid = nullptr;

    DWORD result = PowerGetActiveScheme(nullptr, &activeGuid);
    if (result != ERROR_SUCCESS || !activeGuid) {
        Wh_Log(L"PowerGetActiveScheme failed. Error: %u", result);
        return g_currentPlanIndex;
    }

    int index = FindPlanIndexByGuid(*activeGuid);

    if (index < 0) {
        UpdateCustomPowerPlanNameFromGuid(*activeGuid);

        LocalFree(activeGuid);

        Wh_Log(
            L"Active power plan is not one of the configured plans. Name: %s",
            g_customPowerPlanName
        );

        return -1;
    }

    LocalFree(activeGuid);

    return index;
}

bool SetPowerPlanByIndex(int index) {
    if (index < 0 || index >= g_planCount) {
        return false;
    }

    // Ultimate Performance is only a template on this system unless it was
    // created with: powercfg -duplicatescheme e9a42b02-d5df-448d-aa00-03f14749eb61
    // As long as it doesn't appear in "powercfg /list", don't try to activate it.
    if (index == 3 && !g_ultimateAvailable) {
        Wh_Log(L"Ultimate Performance is not available. Use the menu item to create it first.");
        return false;
    }

    if (!IsPowerPlanAvailable(g_plans[index].guid)) {
        Wh_Log(L"Power plan not available: %s", g_plans[index].name);
        return false;
    }

    GUID runtimeGuid = GetRuntimePlanGuid(index);
    DWORD result = PowerSetActiveScheme(nullptr, &runtimeGuid);

    if (result == ERROR_SUCCESS) {
        g_currentPlanIndex = index;
        g_lastSeenPowerSchemeGuid = runtimeGuid;
        g_hasLastSeenPowerSchemeGuid = true;

        Wh_Log(L"Active power plan changed to: %s", g_plans[index].name);

        return true;
    }

    Wh_Log(
        L"PowerSetActiveScheme failed for %s. Error: %u. Trying powercfg fallback.",
        g_plans[index].name,
        result
    );

    StartPowercfgFallbackWorker(index);
    return false;
}

bool CreateUltimatePerformancePlanWorker(
    const PowerSchemeSnapshot* before,
    GUID* newGuid,
    bool* hasNewGuid
) {
    if (hasNewGuid) {
        *hasNewGuid = false;
    }

    Wh_Log(L"Creating Ultimate Performance power scheme via powercfg -duplicatescheme...");

    WCHAR powercfgPath[MAX_PATH] = {};
    if (!BuildSystemExecutablePath(L"powercfg.exe", powercfgPath, ARRAYSIZE(powercfgPath))) {
        Wh_Log(L"Failed to build full path for powercfg.exe.");
        return false;
    }

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";
    sei.lpFile = powercfgPath;
    sei.lpParameters = L"-duplicatescheme e9a42b02-d5df-448d-aa00-03f14749eb61";
    sei.nShow = SW_HIDE;

    if (!ShellExecuteExW(&sei)) {
        DWORD error = GetLastError();

        if (error == ERROR_CANCELLED) {
            Wh_Log(L"User cancelled UAC prompt for creating Ultimate Performance.");
        } else {
            Wh_Log(L"Failed to start elevated powercfg duplicatescheme. Error: %u", error);
        }

        return false;
    }

    if (sei.hProcess) {
        WaitForSingleObject(sei.hProcess, 10000);

        DWORD exitCode = 1;
        GetExitCodeProcess(sei.hProcess, &exitCode);
        CloseHandle(sei.hProcess);

        if (exitCode != 0) {
            Wh_Log(L"powercfg -duplicatescheme failed. Exit code: %u", exitCode);
            return false;
        }
    }

    Sleep(500);

    if (before && newGuid && hasNewGuid &&
        FindNewPowerSchemeAfterSnapshot(before, newGuid)) {
        *hasNewGuid = true;

        WCHAR guidText[64] = {};
        GuidToStringNoBraces(*newGuid, guidText, ARRAYSIZE(guidText));

        Wh_Log(L"New Ultimate Performance scheme detected by GUID: %s", guidText);
    } else {
        Wh_Log(L"Ultimate Performance creation command finished, but no new GUID was detected.");
    }

    return true;
}

DWORD WINAPI CreateUltimateThreadProc(LPVOID param) {
    PowerSchemeSnapshot* before = reinterpret_cast<PowerSchemeSnapshot*>(param);

    CreateUltimateResult* result = new CreateUltimateResult{};
    result->success = false;
    result->hasNewGuid = false;
    result->newGuid = {};

    result->success = CreateUltimatePerformancePlanWorker(
        before,
        &result->newGuid,
        &result->hasNewGuid
    );

    delete before;

    if (!PostMessageW(
            g_hwnd,
            WM_APP_CREATE_ULTIMATE_DONE,
            0,
            reinterpret_cast<LPARAM>(result))) {
        delete result;
    }

    return 0;
}

bool StartCreateUltimateWorker() {
    if (g_createUltimateRunning) {
        Wh_Log(L"Create Ultimate Performance worker already running.");
        return false;
    }

    g_createUltimateRunning = true;
    PowerSchemeSnapshot* before = new PowerSchemeSnapshot{};
    CapturePowerSchemeSnapshot(before);

    HANDLE thread = CreateThread(
        nullptr,
        0,
        CreateUltimateThreadProc,
        before,
        0,
        nullptr
    );

    if (!thread) {
        delete before;
        g_createUltimateRunning = false;
        Wh_Log(L"Failed to start Create Ultimate Performance worker. Error: %u", GetLastError());
        return false;
    }

    CloseHandle(thread);
    return true;
}

void OpenWindhawk() {
    if (!g_windhawkPath[0]) {
        Wh_Log(L"Windhawk path is empty.");
        return;
    }

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.lpFile = g_windhawkPath;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"Failed to open Windhawk. Error: %u", GetLastError());
    }
}

void OpenClassicPowerOptions() {
    WCHAR controlPath[MAX_PATH] = {};

    if (!BuildSystemExecutablePath(L"control.exe", controlPath, ARRAYSIZE(controlPath))) {
        Wh_Log(L"Failed to build full path for control.exe.");
        return;
    }

    ShellExecuteW(
        nullptr,
        L"open",
        controlPath,
        L"powercfg.cpl",
        nullptr,
        SW_SHOWNORMAL
    );
}

void OpenModernPowerSettings() {
    ShellExecuteW(
        nullptr,
        L"open",
        L"ms-settings:powersleep",
        nullptr,
        nullptr,
        SW_SHOWNORMAL
    );
}

// ------------------------------------------------------------
// Tray icon drawing
// ------------------------------------------------------------

HICON CreateFallbackTrayIcon() {
    HICON sharedIcon = LoadIconW(nullptr, IDI_APPLICATION);

    if (!sharedIcon) {
        return nullptr;
    }

    // LoadIconW with IDI_APPLICATION returns a shared icon.
    // Copy it so the returned HICON is owned by this mod and can be safely
    // destroyed with DestroyIcon later.
    return CopyIcon(sharedIcon);
}

HICON CreatePowerTrayIcon(int planIndex) {
    const int size = 32;

    COLORREF color = RGB(160, 160, 160);
    const WCHAR* text = L"?";

    if (IsValidPlanIndex(planIndex)) {
        color = g_plans[planIndex].color;
        text = g_plans[planIndex].shortName;
    }

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    DWORD* pixels = nullptr;

    HDC hdc = GetDC(nullptr);
    HBITMAP hbmColor = CreateDIBSection(
        hdc,
        &bi,
        DIB_RGB_COLORS,
        reinterpret_cast<void**>(&pixels),
        nullptr,
        0
    );
    ReleaseDC(nullptr, hdc);

    if (!hbmColor || !pixels) {
        return CreateFallbackTrayIcon();
    }

    for (int i = 0; i < size * size; i++) {
        pixels[i] = 0x00000000;
    }

    HDC iconHdc = CreateCompatibleDC(nullptr);
    if (!iconHdc) {
        DeleteObject(hbmColor);
        return CreateFallbackTrayIcon();
    }

    HBITMAP oldBitmap = (HBITMAP)SelectObject(iconHdc, hbmColor);

    HBRUSH bgBrush = CreateSolidBrush(RGB(18, 18, 18));
    HBRUSH oldBrush = (HBRUSH)SelectObject(iconHdc, bgBrush);

    HPEN borderPen = CreatePen(PS_SOLID, 3, color);
    HPEN oldPen = (HPEN)SelectObject(iconHdc, borderPen);

    RoundRect(iconHdc, 1, 1, 31, 31, 8, 8);

    SelectObject(iconHdc, oldPen);
    DeleteObject(borderPen);

    SelectObject(iconHdc, oldBrush);
    DeleteObject(bgBrush);

    SetBkMode(iconHdc, TRANSPARENT);

    HFONT hFont = CreateFontW(
        25,
        0,
        0,
        0,
        FW_HEAVY,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Segoe UI"
    );

    HFONT oldFont = (HFONT)SelectObject(iconHdc, hFont);

    RECT textRect = {0, 0, 32, 31};

    RECT shadowRect = textRect;
    OffsetRect(&shadowRect, 1, 1);

    SetTextColor(iconHdc, RGB(1, 1, 1));
    DrawTextW(
        iconHdc,
        text,
        -1,
        &shadowRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    SetTextColor(iconHdc, color);
    DrawTextW(
        iconHdc,
        text,
        -1,
        &textRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    SelectObject(iconHdc, oldFont);
    DeleteObject(hFont);

    SelectObject(iconHdc, oldBitmap);
    DeleteDC(iconHdc);

    for (int i = 0; i < size * size; i++) {
        if ((pixels[i] & 0x00FFFFFF) != 0) {
            pixels[i] |= 0xFF000000;
        }
    }

    HBITMAP hbmMask = CreateBitmap(size, size, 1, 1, nullptr);
    if (!hbmMask) {
        DeleteObject(hbmColor);
        return CreateFallbackTrayIcon();
    }

    ICONINFO iconInfo = {};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmColor = hbmColor;
    iconInfo.hbmMask = hbmMask;

    HICON hIcon = CreateIconIndirect(&iconInfo);

    DeleteObject(hbmColor);
    DeleteObject(hbmMask);

    if (!hIcon) {
        return CreateFallbackTrayIcon();
    }

    return hIcon;
}

// ------------------------------------------------------------
// Tray icon handling
// ------------------------------------------------------------

void SetTrayIconRectSnapshot(const RECT& rect) {
    AcquireSRWLockExclusive(&g_trayStateLock);
    g_trayIconRect = rect;
    ReleaseSRWLockExclusive(&g_trayStateLock);
}

void SetTrayFallbackAreaSnapshot(const RECT* areas, int count) {
    AcquireSRWLockExclusive(&g_trayStateLock);

    g_trayFallbackAreaCount = 0;
    ZeroMemory(g_trayFallbackAreas, sizeof(g_trayFallbackAreas));

    if (areas && count > 0) {
        int copyCount = count;
        if (copyCount > static_cast<int>(ARRAYSIZE(g_trayFallbackAreas))) {
            copyCount = static_cast<int>(ARRAYSIZE(g_trayFallbackAreas));
        }

        for (int i = 0; i < copyCount; i++) {
            g_trayFallbackAreas[i] = areas[i];
        }

        g_trayFallbackAreaCount = copyCount;
    }

    ReleaseSRWLockExclusive(&g_trayStateLock);
}

RECT GetTrayIconRectSnapshot() {
    RECT rect = {};

    AcquireSRWLockShared(&g_trayStateLock);
    rect = g_trayIconRect;
    ReleaseSRWLockShared(&g_trayStateLock);

    return rect;
}

void GetTrayMouseStateSnapshot(
    RECT* rect,
    DWORD* lastMouseMoveTime,
    RECT* fallbackAreas,
    int* fallbackAreaCount
) {
    AcquireSRWLockShared(&g_trayStateLock);

    if (rect) {
        *rect = g_trayIconRect;
    }

    if (lastMouseMoveTime) {
        *lastMouseMoveTime = g_lastTrayMouseMoveTime;
    }

    if (fallbackAreaCount) {
        int copyCount = g_trayFallbackAreaCount;

        if (!fallbackAreas) {
            copyCount = 0;
        } else if (copyCount > 2) {
            copyCount = 2;
        }

        for (int i = 0; i < copyCount; i++) {
            fallbackAreas[i] = g_trayFallbackAreas[i];
        }

        *fallbackAreaCount = copyCount;
    }

    ReleaseSRWLockShared(&g_trayStateLock);
}

void SetLastTrayMouseMoveTime(DWORD value) {
    AcquireSRWLockExclusive(&g_trayStateLock);
    g_lastTrayMouseMoveTime = value;
    ReleaseSRWLockExclusive(&g_trayStateLock);
}

void RefreshTrayFallbackAreas() {
    RECT areas[2] = {};
    int count = 0;

    const PCWSTR classes[] = {
        L"NotifyIconOverflowWindow",
        L"TopLevelWindowForOverflowXamlIsland",
    };

    for (size_t i = 0; i < ARRAYSIZE(classes); i++) {
        HWND hwnd = FindWindowW(classes[i], nullptr);
        RECT rect = {};

        if (hwnd && GetWindowRect(hwnd, &rect) && !IsRectEmpty(&rect)) {
            areas[count++] = rect;

            if (count >= static_cast<int>(ARRAYSIZE(areas))) {
                break;
            }
        }
    }

    SetTrayFallbackAreaSnapshot(areas, count);
}

void RefreshTrayIconRect() {
    if (!g_hwnd) {
        return;
    }

    NOTIFYICONIDENTIFIER nii = {};
    nii.cbSize = sizeof(nii);
    nii.hWnd = g_hwnd;
    nii.uID = ID_TRAYICON;

    RECT rect = {};
    HRESULT hr = Shell_NotifyIconGetRect(&nii, &rect);
    if (FAILED(hr)) {
        SetRectEmpty(&rect);
        Wh_Log(L"Shell_NotifyIconGetRect failed. HRESULT: 0x%08X", hr);
    }

    SetTrayIconRectSnapshot(rect);
    RefreshTrayFallbackAreas();
}

void StartTrayRectRefreshTimer() {
    if (!g_hwnd) {
        return;
    }

    g_trayRectRefreshAttempts = 0;

    SetTimer(
        g_hwnd,
        TIMER_TRAY_RECT_REFRESH,
        TRAY_RECT_REFRESH_INTERVAL_MS,
        nullptr
    );

    Wh_Log(L"Tray icon rect refresh timer started.");
}

void UpdateTrayTooltip() {
    if (!g_hwnd) {
        return;
    }

    WCHAR tip[128] = {};
    StringCchPrintfW(
        tip,
        ARRAYSIZE(tip),
        L"Power Options Button\nActive: %s\nMouse wheel: switch\nRight-click: menu",
        GetPlanDisplayName(g_currentPlanIndex)
    );

    StringCchCopyW(g_nid.szTip, ARRAYSIZE(g_nid.szTip), tip);

    g_nid.uFlags = NIF_TIP | NIF_SHOWTIP;
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);
}

void UpdateTrayIcon(bool refreshCurrentPlan = true) {
    if (!g_hwnd) {
        return;
    }

    if (refreshCurrentPlan) {
        g_currentPlanIndex = GetCurrentPowerPlanIndex();
        RememberActivePowerSchemeGuid();
    }

    HICON newIcon = CreatePowerTrayIcon(g_currentPlanIndex);
    if (!newIcon) {
        return;
    }

    HICON oldIcon = g_hIcon;
    g_hIcon = newIcon;

    g_nid.uFlags = NIF_ICON;
    g_nid.hIcon = g_hIcon;
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);

    UpdateTrayTooltip();
    RefreshTrayIconRect();

    if (oldIcon) {
        DestroyIcon(oldIcon);
    }
}

void RefreshActivePowerPlanStatusIfChanged() {
    GUID activeGuid = {};

    if (!GetActivePowerSchemeGuid(&activeGuid)) {
        return;
    }

    if (g_hasLastSeenPowerSchemeGuid &&
        GuidEquals(activeGuid, g_lastSeenPowerSchemeGuid)) {
        return;
    }

    g_lastSeenPowerSchemeGuid = activeGuid;
    g_hasLastSeenPowerSchemeGuid = true;

    int newIndex = FindPlanIndexByGuid(activeGuid);

    // If an Ultimate Performance plan was created or activated outside this
    // mod, refresh detection before treating it as a custom power plan.
    if (newIndex < 0) {
        RefreshUltimatePerformanceAvailability();
        newIndex = FindPlanIndexByGuid(activeGuid);
    }

    if (newIndex == g_currentPlanIndex) {
        return;
    }

    g_currentPlanIndex = newIndex;

    if (IsValidPlanIndex(g_currentPlanIndex)) {
        Wh_Log(
            L"Active power plan changed externally to: %s",
            g_plans[g_currentPlanIndex].name
        );
    } else {
        UpdateCustomPowerPlanNameFromGuid(activeGuid);

        Wh_Log(
            L"Active power plan changed externally to custom power plan: %s",
            g_customPowerPlanName
        );
    }

    UpdateTrayIcon(false);
}

bool AddTrayIcon() {
    if (!g_hwnd) {
        return false;
    }

    g_currentPlanIndex = GetCurrentPowerPlanIndex();

    if (!g_hIcon) {
        g_hIcon = CreatePowerTrayIcon(g_currentPlanIndex);
    }

    if (!g_hIcon) {
        Wh_Log(L"Failed to create tray icon.");
        return false;
    }

    ZeroMemory(&g_nid, sizeof(g_nid));

    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd = g_hwnd;
    g_nid.uID = ID_TRAYICON;
    g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = g_hIcon;

    StringCchPrintfW(
        g_nid.szTip,
        ARRAYSIZE(g_nid.szTip),
        L"Power Options Button\nActive: %s\nMouse wheel: switch\nRight-click: menu",
        GetPlanDisplayName(g_currentPlanIndex)
    );

    BOOL result = Shell_NotifyIconW(NIM_ADD, &g_nid);

    if (result) {
        NOTIFYICONDATAW nidVer = {};
        nidVer.cbSize = sizeof(nidVer);
        nidVer.hWnd = g_hwnd;
        nidVer.uID = ID_TRAYICON;
        nidVer.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nidVer);

        Wh_Log(L"Power Options Button tray icon added.");
        RefreshTrayIconRect();
        StartTrayRectRefreshTimer();

        SetTimer(
            g_hwnd,
            TIMER_POST_START_ICON_SYNC,
            POST_START_ICON_SYNC_DELAY_MS,
            nullptr
        );

        Wh_Log(L"Post-start tray icon sync timer started.");

        RememberActivePowerSchemeGuid();

        SetTimer(
            g_hwnd,
            TIMER_ACTIVE_POWER_PLAN_REFRESH,
            ACTIVE_POWER_PLAN_REFRESH_INTERVAL_MS,
            nullptr
        );

        Wh_Log(L"Active power plan refresh timer started.");

    } else {
        Wh_Log(L"Failed to add tray icon. Error: %u", GetLastError());
    }

    return result != FALSE;
}

void RemoveTrayIcon() {
    if (!g_hwnd) {
        return;
    }

    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwnd;
    nid.uID = ID_TRAYICON;

    Shell_NotifyIconW(NIM_DELETE, &nid);

    if (g_hIcon) {
        DestroyIcon(g_hIcon);
        g_hIcon = nullptr;
    }

    Wh_Log(L"Power Options Button tray icon removed.");
}

// ------------------------------------------------------------
// Mouse wheel over tray icon
// ------------------------------------------------------------

bool IsPointNearTrayIcon(POINT pt, const RECT& cachedRect) {
    RECT rect = cachedRect;

    if (IsRectEmpty(&rect)) {
        return false;
    }

    // Small tolerance for DPI/scaling/border edge cases.
    InflateRect(&rect, 4, 4);

    return PtInRect(&rect, pt) != FALSE;
}

bool IsPointInTrayFallbackAreas(
    POINT pt,
    const RECT* fallbackAreas,
    int fallbackAreaCount
) {
    if (!fallbackAreas || fallbackAreaCount <= 0) {
        return false;
    }

    for (int i = 0; i < fallbackAreaCount; i++) {
        if (PtInRect(&fallbackAreas[i], pt)) {
            return true;
        }
    }

    return false;
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL) {
        const MSLLHOOKSTRUCT* ms =
            reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);

        short delta = static_cast<short>(HIWORD(ms->mouseData));
        int direction = (delta > 0) ? 1 : -1;
        DWORD now = GetTickCount();

        RECT trayRect = {};
        RECT fallbackAreas[2] = {};
        int fallbackAreaCount = 0;
        DWORD lastTrayMouseMoveTime = 0;
        GetTrayMouseStateSnapshot(
            &trayRect,
            &lastTrayMouseMoveTime,
            fallbackAreas,
            &fallbackAreaCount
        );

        BOOL inside = IsPointNearTrayIcon(ms->pt, trayRect);

        // Fallback for cases where Shell_NotifyIconGetRect can't resolve the
        // icon in the notification overflow flyout. The hook only checks
        // cached rectangles; it never calls WindowFromPoint or sends messages
        // to Explorer from inside the low-level callback.
        if (!inside &&
            IsRectEmpty(&trayRect) &&
            lastTrayMouseMoveTime != 0 &&
            now - lastTrayMouseMoveTime < 500 &&
            IsPointInTrayFallbackAreas(
                ms->pt,
                fallbackAreas,
                fallbackAreaCount)) {
            inside = TRUE;
        }

        if (inside && g_hwnd) {
            PostMessageW(
                g_hwnd,
                WM_TRAY_SCROLL,
                static_cast<WPARAM>(direction),
                0
            );

            return 1;
        }
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

bool InstallMouseWheelHook() {
    if (g_mouseHook) {
        return true;
    }

    g_mouseHook = SetWindowsHookExW(
        WH_MOUSE_LL,
        LowLevelMouseProc,
        GetModuleHandleW(nullptr),
        0
    );

    if (!g_mouseHook) {
        Wh_Log(L"Failed to install mouse wheel hook. Error: %u", GetLastError());
        return false;
    }

    Wh_Log(
        L"Mouse wheel hook installed. HHOOK=0x%p, thread=%lu",
        g_mouseHook,
        GetCurrentThreadId()
    );

    return true;
}

void UninstallMouseWheelHook() {
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
        Wh_Log(L"Mouse wheel hook uninstalled.");
    }
}

DWORD WINAPI MouseHookThreadProc(LPVOID param) {
    HANDLE readyEvent = reinterpret_cast<HANDLE>(param);

    // Force creation of this thread's message queue before the owner can post
    // WM_QUIT during teardown.
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    bool installed = InstallMouseWheelHook();

    if (readyEvent) {
        SetEvent(readyEvent);
    }

    if (!installed) {
        return 0;
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UninstallMouseWheelHook();
    return 0;
}

bool StartMouseHookThread() {
    if (g_mouseHookThread) {
        return true;
    }

    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        Wh_Log(L"Failed to create mouse hook ready event. Error: %u", GetLastError());
        return false;
    }

    g_mouseHookThread = CreateThread(
        nullptr,
        0,
        MouseHookThreadProc,
        readyEvent,
        0,
        &g_mouseHookThreadId
    );

    if (!g_mouseHookThread) {
        Wh_Log(L"Failed to create mouse hook thread. Error: %u", GetLastError());
        CloseHandle(readyEvent);
        g_mouseHookThreadId = 0;
        return false;
    }

    DWORD waitResult = WaitForSingleObject(readyEvent, 3000);
    CloseHandle(readyEvent);

    if (waitResult != WAIT_OBJECT_0 || !g_mouseHook) {
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"Mouse hook thread did not initialize within timeout.");
        } else if (waitResult == WAIT_FAILED) {
            Wh_Log(L"Waiting for mouse hook thread initialization failed. Error: %u", GetLastError());
        } else {
            Wh_Log(L"Mouse hook thread started, but the hook was not installed.");
        }

        return false;
    }

    Wh_Log(L"Mouse hook thread started. Thread=%lu", g_mouseHookThreadId);
    return true;
}

void StopMouseHookThread() {
    if (g_mouseHookThreadId) {
        if (!PostThreadMessageW(g_mouseHookThreadId, WM_QUIT, 0, 0)) {
            DWORD error = GetLastError();

            if (!g_mouseHookThread ||
                WaitForSingleObject(g_mouseHookThread, 0) != WAIT_OBJECT_0) {
                Wh_Log(L"Failed to post WM_QUIT to mouse hook thread. Error: %u", error);
            }
        }
    }

    if (g_mouseHookThread) {
        DWORD waitResult = WaitForSingleObject(g_mouseHookThread, 3000);

        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"Mouse hook thread did not exit within timeout.");
        } else if (waitResult == WAIT_FAILED) {
            Wh_Log(L"WaitForSingleObject for mouse hook thread failed. Error: %u", GetLastError());
        }

        CloseHandle(g_mouseHookThread);
        g_mouseHookThread = nullptr;
    }

    g_mouseHookThreadId = 0;
}

void CyclePowerPlan(int direction) {
    g_currentPlanIndex = GetCurrentPowerPlanIndex();

    int nextIndex = g_currentPlanIndex;

    for (int attempt = 0; attempt < g_planCount; attempt++) {
        nextIndex += direction;

        if (nextIndex >= g_planCount) {
            nextIndex = 0;
        } else if (nextIndex < 0) {
            nextIndex = g_planCount - 1;
        }

        if (IsPowerPlanAvailable(g_plans[nextIndex].guid)) {
            break;
        }
    }

    if (SetPowerPlanByIndex(nextIndex)) {
        UpdateTrayIcon();
    }
}

// ------------------------------------------------------------
// Context menu
// ------------------------------------------------------------

void ShowContextMenu() {
    if (!g_hwnd) {
        return;
    }

    g_currentPlanIndex = GetCurrentPowerPlanIndex();
    RefreshUltimatePerformanceAvailability();

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }

    for (int i = 0; i < g_planCount; i++) {
        UINT flags = MF_STRING;

        if (!IsPowerPlanAvailable(g_plans[i].guid)) {
            flags |= MF_GRAYED;
        }

        if (i == g_currentPlanIndex) {
            flags |= MF_CHECKED;
        }

        AppendMenuW(
            menu,
            flags,
            g_plans[i].menuId,
            g_plans[i].name
        );
    }

    if (!g_ultimateAvailable) {
        AppendMenuW(menu, MF_STRING, ID_MENU_CREATE_ULTIMATE, L"Create Ultimate Performance...");
    }

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, ID_MENU_REFRESH, L"Refresh");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, ID_MENU_OPEN_POWER, L"Open classic Power Options");
    AppendMenuW(menu, MF_STRING, ID_MENU_OPEN_SETTINGS, L"Open Windows power settings");
    AppendMenuW(menu, MF_STRING, ID_MENU_OPEN_WINDHAWK, L"Open Windhawk");

    POINT pt;
    GetCursorPos(&pt);

    SetForegroundWindow(g_hwnd);

    TrackPopupMenu(
        menu,
        TPM_RIGHTBUTTON,
        pt.x,
        pt.y,
        0,
        g_hwnd,
        nullptr
    );

    PostMessageW(g_hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);
}

// ------------------------------------------------------------
// Window procedure
// ------------------------------------------------------------

LRESULT CALLBACK HiddenWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == g_taskbarCreatedMessage) {
        Wh_Log(L"TaskbarCreated received. Re-adding tray icon.");
        AddTrayIcon();
        UpdateTrayIcon();
        return 0;
    }

    switch (msg) {
    case WM_APP_CREATE_ULTIMATE_DONE: {
        g_createUltimateRunning = false;

        CreateUltimateResult* result =
            reinterpret_cast<CreateUltimateResult*>(lParam);

        bool success = false;

        if (result) {
            success = result->success;

            if (result->success && result->hasNewGuid) {
                g_ultimateAvailable = true;
                g_ultimateRuntimeGuid = result->newGuid;
                PersistUltimateRuntimeGuid(g_ultimateRuntimeGuid);

                WCHAR friendlyName[128] = {};
                if (ReadPowerSchemeFriendlyName(
                        g_ultimateRuntimeGuid,
                        friendlyName,
                        ARRAYSIZE(friendlyName)) &&
                    friendlyName[0]) {
                    StringCchCopyW(
                        g_ultimateRuntimeName,
                        ARRAYSIZE(g_ultimateRuntimeName),
                        friendlyName
                    );
                } else {
                    StringCchCopyW(
                        g_ultimateRuntimeName,
                        ARRAYSIZE(g_ultimateRuntimeName),
                        L"Ultimate Performance"
                    );
                }

                WCHAR guidText[64] = {};
                GuidToStringNoBraces(g_ultimateRuntimeGuid, guidText, ARRAYSIZE(guidText));

                Wh_Log(
                    L"Create Ultimate Performance worker captured new GUID. Name: %s, GUID: %s",
                    g_ultimateRuntimeName,
                    guidText
                );
            } else {
                RefreshUltimatePerformanceAvailability();
            }

            delete result;
        } else {
            RefreshUltimatePerformanceAvailability();
        }

        UpdateTrayIcon();

        Wh_Log(
            L"Create Ultimate Performance worker completed. Success=%d",
            success
        );

        return 0;
    }

    case WM_TIMER:
        if (wParam == TIMER_TRAY_RECT_REFRESH) {
            RefreshTrayIconRect();

            g_trayRectRefreshAttempts++;

            RECT trayRect = GetTrayIconRectSnapshot();
            if (!IsRectEmpty(&trayRect)) {
                KillTimer(hwnd, TIMER_TRAY_RECT_REFRESH);

                Wh_Log(
                    L"Tray icon rect detected after %d attempt(s): left=%ld top=%ld right=%ld bottom=%ld",
                    g_trayRectRefreshAttempts,
                    trayRect.left,
                    trayRect.top,
                    trayRect.right,
                    trayRect.bottom
                );

                return 0;
            }

            if (g_trayRectRefreshAttempts >= TRAY_RECT_REFRESH_MAX_ATTEMPTS) {
                KillTimer(hwnd, TIMER_TRAY_RECT_REFRESH);

                Wh_Log(L"Tray icon rect refresh timer stopped. Tray rect still empty.");

                return 0;
            }

            return 0;
        }

        if (wParam == TIMER_POST_START_ICON_SYNC) {
            KillTimer(hwnd, TIMER_POST_START_ICON_SYNC);

            Wh_Log(L"Post-start tray icon sync running.");

            UpdateTrayIcon();
            RefreshTrayIconRect();

            Wh_Log(L"Post-start tray icon sync completed.");

            return 0;
        }

        if (wParam == TIMER_ACTIVE_POWER_PLAN_REFRESH) {
            RefreshActivePowerPlanStatusIfChanged();
            return 0;
        }

        break;

    case WM_APP_POWERCFG_FALLBACK_DONE: {
        g_powercfgFallbackRunning = false;

        bool success = wParam != FALSE;
        int index = static_cast<int>(lParam);
        PCWSTR planName = IsValidPlanIndex(index) ? g_plans[index].name : L"(unknown)";

        if (success) {
            g_currentPlanIndex = GetCurrentPowerPlanIndex();
            UpdateTrayIcon();

            Wh_Log(
                L"powercfg fallback worker completed successfully for: %s",
                planName
            );
        } else {
            Wh_Log(
                L"powercfg fallback worker failed for: %s",
                planName
            );
        }

        return 0;
    }

    case WM_CREATE:
        g_hwnd = hwnd;
        Wh_Log(L"Hidden window created. HWND: 0x%p", hwnd);
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_TRAY_RECT_REFRESH);
        KillTimer(hwnd, TIMER_POST_START_ICON_SYNC);
        KillTimer(hwnd, TIMER_ACTIVE_POWER_PLAN_REFRESH);
        RemoveTrayIcon();
        PostQuitMessage(0);
        return 0;

    case WM_TRAYICON: {
        UINT event = LOWORD(lParam);

        if (event == WM_RBUTTONUP || event == WM_CONTEXTMENU) {
            ShowContextMenu();
            return 0;
        }

        if (event == WM_LBUTTONUP || event == NIN_SELECT || event == NIN_KEYSELECT) {
            UpdateTrayIcon();
            return 0;
        }

        if (event == WM_MOUSEMOVE) {
            SetLastTrayMouseMoveTime(GetTickCount());
            RefreshTrayIconRect();
            return 0;
        }

        return 0;
    }

    case WM_TRAY_SCROLL: {
        DWORD now = GetTickCount();
        DWORD diff = now - g_lastScrollTime;

        int direction = static_cast<int>(wParam);

        if (diff > 150) {
            g_lastScrollTime = now;
            CyclePowerPlan(direction);
        }

        return 0;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);

        for (int i = 0; i < g_planCount; i++) {
            if (id == g_plans[i].menuId) {
                if (SetPowerPlanByIndex(i)) {
                    UpdateTrayIcon();
                }
                return 0;
            }
        }

        switch (id) {
        case ID_MENU_CREATE_ULTIMATE:
            StartCreateUltimateWorker();
            return 0;

        case ID_MENU_REFRESH:
            Wh_Log(L"Manual refresh requested from tray menu.");
            RefreshUltimatePerformanceAvailability();
            UpdateTrayIcon();

            {
                PCWSTR currentName = GetPlanDisplayName(g_currentPlanIndex);

                Wh_Log(
                    L"Manual refresh completed. Current plan: %s",
                    currentName
                );
            }

            return 0;

        case ID_MENU_OPEN_POWER:
            OpenClassicPowerOptions();
            return 0;

        case ID_MENU_OPEN_SETTINGS:
            OpenModernPowerSettings();
            return 0;

        case ID_MENU_OPEN_WINDHAWK:
            OpenWindhawk();
            return 0;
        }

        return 0;
    }

    case WM_APP_EXIT:
        DestroyWindow(hwnd);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ------------------------------------------------------------
// UI thread
// ------------------------------------------------------------

DWORD WINAPI UiThreadProc(LPVOID) {
    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = HiddenWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = HIDDEN_WINDOW_CLASS;

    ATOM classAtom = RegisterClassW(&wc);
    if (!classAtom) {
        Wh_Log(L"RegisterClassW failed. Error: %u", GetLastError());
        return 0;
    }

    HWND hwnd = CreateWindowExW(
        0,
        HIDDEN_WINDOW_CLASS,
        L"Power Options Button",
        WS_OVERLAPPED,
        0,
        0,
        0,
        0,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd) {
        Wh_Log(L"Failed to create hidden window. Error: %u", GetLastError());
        UnregisterClassW(HIDDEN_WINDOW_CLASS, hInstance);
        return 0;
    }

    g_hwnd = hwnd;

    if (AddTrayIcon()) {
        RefreshTrayIconRect();
    } else {
        Wh_Log(L"AddTrayIcon returned false after CreateWindowExW.");
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_hwnd = nullptr;

    if (!UnregisterClassW(HIDDEN_WINDOW_CLASS, hInstance)) {
        Wh_Log(L"UnregisterClassW failed. Error: %u", GetLastError());
    }

    return 0;
}

// ------------------------------------------------------------
// Windhawk lifecycle
// ------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Power Options Button...");

    GetModuleFileNameW(nullptr, g_windhawkPath, ARRAYSIZE(g_windhawkPath));

    RefreshUltimatePerformanceAvailability();
    g_currentPlanIndex = GetCurrentPowerPlanIndex();

    if (!StartMouseHookThread()) {
        Wh_Log(
            L"Failed to start dedicated mouse hook thread. "
            L"Continuing without mouse-wheel support."
        );
        StopMouseHookThread();
    }

    g_uiThread = CreateThread(
        nullptr,
        0,
        UiThreadProc,
        nullptr,
        0,
        &g_uiThreadId
    );

    if (!g_uiThread) {
        Wh_Log(L"Failed to create UI thread. Error: %u", GetLastError());
        StopMouseHookThread();
        return FALSE;
    }

    Wh_Log(L"Power Options Button initialized.");
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninitializing Power Options Button...");

    StopMouseHookThread();

    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_APP_EXIT, 0, 0);
    } else if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    }

    if (g_uiThread) {
        DWORD waitResult = WaitForSingleObject(g_uiThread, 3000);

        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"UI thread did not exit within timeout.");
        } else if (waitResult == WAIT_FAILED) {
            Wh_Log(L"WaitForSingleObject for UI thread failed. Error: %u", GetLastError());
        }

        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }

    g_uiThreadId = 0;

    if (g_hIcon) {
        DestroyIcon(g_hIcon);
        g_hIcon = nullptr;
    }

    Wh_Log(L"Power Options Button uninitialized.");
}

void WhTool_ModSettingsChanged() {
    // No Windhawk settings yet.
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;

            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }

            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);

        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);

        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[
        MAX_PATH + 2 +
        (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1
    ];

    swprintf_s(
        commandLine,
        ARRAYSIZE(commandLine),
        L"\"%s\" -tool-mod \"%s\"",
        currentProcessPath,
        WH_MOD_ID
    );

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken,
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        WINBOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken
    );

    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(
            kernelModule,
            "CreateProcessInternalW"
        );

    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };

    PROCESS_INFORMATION pi;

    if (!pCreateProcessInternalW(
            nullptr,
            currentProcessPath,
            commandLine,
            nullptr,
            nullptr,
            FALSE,
            NORMAL_PRIORITY_CLASS,
            nullptr,
            nullptr,
            &si,
            &pi,
            nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}