// ==WindhawkMod==
// @id              zoneshift
// @name            ZoneShift - Per-App Timezone Spoofer
// @description     Spoof timezone and local time for specific apps without altering the Windows system clock.
// @version         1.0.0
// @author          Dev-StudioCode
// @github          https://github.com/Dev-StudioCode
// @include         wallpaper32.exe
// @include         wallpaper64.exe
// @include         webwallpaper32.exe
// @include         webwallpaper64.exe
// @include         edgewallpaper32.exe
// @include         edgewallpaper64.exe
// @include         ui32.exe
// @include         wallpaperui.exe
// @compilerOptions -ladvapi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ZoneShift - Per-App Timezone Spoofer

**ZoneShift** is a lightweight, high-performance Windhawk mod that spoofs the time zone and local time for individual Windows applications without changing the system clock or affecting other running programs.

---

### Why Per-App Timezone Spoofing?

1. **Wallpaper Engine & Desktop Customization**:
   - Web wallpapers (Chromium/CEF), Scene wallpapers (DirectX/native), and Edge wallpapers can display the local clock of any chosen city (e.g. Tokyo, London, New York) or simulate day/night lighting cycles for a different hemisphere without altering your PC's real clock.

2. **International Outreach & Client Work**:
   - Localize browser profiles, CRM tools, scheduling calendars, Slack, Discord, or web apps to match client timezones, avoiding timezone math errors while keeping your taskbar clock accurate.

3. **Software Testing & QA**:
   - Verify application behavior across global regions (e.g., date-boundary logic, DST shifts, calendar schedulers, daily reset intervals) on demand.

4. **Privacy & Anti-Fingerprinting**:
   - Mask your real physical timezone from web tracking scripts and telemetry engines that cross-check timezone offsets.

---

### Important Usage Notes

- **Adding Custom Applications**:
  By default, ZoneShift targets Wallpaper Engine processes to keep other apps unaffected out of the box. To target any other application (browsers, chat apps, games, etc.):
  1. Open the **Windhawk UI**.
  2. Navigate to **ZoneShift** -> **Details** -> **Advanced** tab.
  3. Under **Process inclusion list**, add the executable name (e.g., `chrome.exe`, `slack.exe`, `yourapp.exe`).
  4. Click **Save**.

- **UTC Time Shifting in Browsers**:
  Enabling `Shift UTC Time` shifts `GetSystemTime`. Note that shifting UTC time by more than a few minutes in web browsers may cause HTTPS security certificate validation failures, expired cookies, or invalid TOTP two-factor authentication codes. Keep this option disabled unless specifically required.

- **Client Games & Anti-Cheat**:
  Many multiplayer games employ anti-cheat systems that inspect or block process hooks. Use caution when injecting mods into protected games, and note that server-authoritative games determine daily resets on the game server.

---

### How It Works

Windows apps query time through multiple Win32, NT, ICU, CRT, and registry APIs. ZoneShift intercepts all critical layers at the process level:
- **`GetDynamicTimeZoneInformation` & `GetTimeZoneInformation`**: Modern Win32 APIs used by Chromium, Electron, CEF, Node.js, and ICU to resolve IANA and Windows timezones.
- **`GetLocalTime`**: Native API called by C++/DirectX apps, desktop widgets, and Wallpaper Engine scene wallpapers.
- **`SystemTimeToTzSpecificLocalTime` / `TzSpecificLocalTimeToSystemTime`**: Local-to-UTC and UTC-to-local conversions with full Daylight Saving Time (DST) support.
- **`FileTimeToLocalFileTime` / `LocalFileTimeToFileTime`**: File and timestamp conversions.
- **`NtQuerySystemInformation` (`SystemTimeOfDayInformation`)**: Low-level NT kernel time-of-day bias.
- **`RegQueryValueExW`**: Intercepts `TimeZoneKeyName` and `ActiveTimeBias` queries.
- **Windows ICU Integration**: Automatically converts any IANA identifier (e.g. `Europe/Warsaw`, `America/Mexico_City`, `Asia/Tokyo`) to Windows time zones using `icu.dll`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetTimezone: Tokyo Standard Time
  $name: Target Time Zone
  $description: 'Select the spoofed time zone. Popular presets are provided. Choose "Custom" to enter any Windows or IANA time zone below.'
  $options:
    - Tokyo Standard Time: 'Tokyo, Japan (UTC+09:00)'
    - GMT Standard Time: 'London, UK (GMT / BST, UTC+00:00 / UTC+01:00)'
    - UTC: 'UTC (Coordinated Universal Time, UTC+00:00)'
    - W. Europe Standard Time: 'Berlin, Paris, Rome, Madrid (CET / CEST, UTC+01:00 / UTC+02:00)'
    - Eastern Standard Time: 'New York, Toronto (US Eastern, UTC-05:00 / UTC-04:00)'
    - Central Standard Time: 'Chicago, Dallas (US Central, UTC-06:00 / UTC-05:00)'
    - Mountain Standard Time: 'Denver (US Mountain, UTC-07:00 / UTC-06:00)'
    - Pacific Standard Time: 'Los Angeles, Seattle (US Pacific, UTC-08:00 / UTC-07:00)'
    - Hawaiian Standard Time: 'Honolulu, Hawaii (UTC-10:00)'
    - India Standard Time: 'New Delhi, Mumbai, Kolkata (IST, UTC+05:30)'
    - China Standard Time: 'Beijing, Shanghai, Hong Kong (UTC+08:00)'
    - Singapore Standard Time: 'Singapore (UTC+08:00)'
    - Korea Standard Time: 'Seoul, South Korea (UTC+09:00)'
    - AUS Eastern Standard Time: 'Sydney, Melbourne (AEST / AEDT, UTC+10:00 / UTC+11:00)'
    - New Zealand Standard Time: 'Auckland, Wellington (NZST / NZDT, UTC+12:00 / UTC+13:00)'
    - Arabian Standard Time: 'Dubai, Abu Dhabi (UTC+04:00)'
    - Custom: 'Custom (Specified below)'
- customTimezoneKey: ""
  $name: Custom Time Zone Key / Name
  $description: 'If "Custom" is selected above, enter any Windows Time Zone Key Name (e.g. Alaskan Standard Time) or an IANA identifier (e.g. Asia/Tokyo, Europe/Warsaw).'
- timeShiftMinutes: 0
  $name: Custom Time Shift (Minutes)
  $description: 'Additional time offset in minutes added to local time (positive or negative). Set to 0 for normal time for the selected zone (e.g. 120 adds 2 hours).'
- spoofUtcTime: false
  $name: Shift UTC Time (GetSystemTime)
  $description: 'Applies the custom time shift to GetSystemTime. Warning: Shifting UTC time in browsers may break HTTPS certificates, cookie expiration, and TOTP two-factor codes.'
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <winternl.h>
#include <cwchar>
#include <string>

extern "C" DWORD WINAPI EnumDynamicTimeZoneInformation(
    const DWORD dwIndex,
    PDYNAMIC_TIME_ZONE_INFORMATION lpTimeZoneInformation
);

// ---------------------------------------------------------------------------
// Structs & Prototypes
// ---------------------------------------------------------------------------

#ifndef SystemTimeOfDayInformation
#define SystemTimeOfDayInformation 3
#endif

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

#ifndef STATUS_NOT_IMPLEMENTED
#define STATUS_NOT_IMPLEMENTED ((NTSTATUS)0xC0000002L)
#endif

typedef struct _SYSTEM_TIMEOFDAY_INFORMATION_LOCAL {
    LARGE_INTEGER BootTime;
    LARGE_INTEGER CurrentTime;
    LARGE_INTEGER TimeZoneBias;
    ULONG TimeZoneId;
    ULONG Reserved;
    ULONGLONG BootId;
} SYSTEM_TIMEOFDAY_INFORMATION_LOCAL;

// Function pointer types
typedef DWORD (WINAPI *pfn_GetDynamicTimeZoneInformation)(PDYNAMIC_TIME_ZONE_INFORMATION pdtzi);
typedef DWORD (WINAPI *pfn_GetTimeZoneInformation)(LPTIME_ZONE_INFORMATION ptzi);
typedef BOOL (WINAPI *pfn_GetTimeZoneInformationForYear)(USHORT wYear, PDYNAMIC_TIME_ZONE_INFORMATION pdtzi, LPTIME_ZONE_INFORMATION ptzi);
typedef VOID (WINAPI *pfn_GetLocalTime)(LPSYSTEMTIME lpSystemTime);
typedef BOOL (WINAPI *pfn_SystemTimeToTzSpecificLocalTime)(const TIME_ZONE_INFORMATION *lpTimeZoneInformation, const SYSTEMTIME *lpUniversalTime, LPSYSTEMTIME lpLocalTime);
typedef BOOL (WINAPI *pfn_SystemTimeToTzSpecificLocalTimeEx)(const DYNAMIC_TIME_ZONE_INFORMATION *lpTimeZoneInformation, const SYSTEMTIME *lpUniversalTime, LPSYSTEMTIME lpLocalTime);
typedef BOOL (WINAPI *pfn_TzSpecificLocalTimeToSystemTime)(const TIME_ZONE_INFORMATION *lpTimeZoneInformation, const SYSTEMTIME *lpLocalTime, LPSYSTEMTIME lpUniversalTime);
typedef BOOL (WINAPI *pfn_TzSpecificLocalTimeToSystemTimeEx)(const DYNAMIC_TIME_ZONE_INFORMATION *lpTimeZoneInformation, const SYSTEMTIME *lpLocalTime, LPSYSTEMTIME lpUniversalTime);
typedef BOOL (WINAPI *pfn_FileTimeToLocalFileTime)(const FILETIME *lpFileTime, LPFILETIME lpLocalFileTime);
typedef BOOL (WINAPI *pfn_LocalFileTimeToFileTime)(const FILETIME *lpLocalFileTime, LPFILETIME lpFileTime);
typedef VOID (WINAPI *pfn_GetSystemTime)(LPSYSTEMTIME lpSystemTime);
typedef VOID (WINAPI *pfn_GetSystemTimeAsFileTime)(LPFILETIME lpSystemTimeAsFileTime);
typedef VOID (WINAPI *pfn_GetSystemTimePreciseAsFileTime)(LPFILETIME lpSystemTimeAsFileTime);
typedef NTSTATUS (NTAPI *pfn_NtQuerySystemInformation)(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);
typedef LSTATUS (WINAPI *pfn_RegQueryValueExW)(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);

// Original function trampolines
static pfn_GetDynamicTimeZoneInformation pfnOrig_GetDynamicTimeZoneInformation = nullptr;
static pfn_GetTimeZoneInformation pfnOrig_GetTimeZoneInformation = nullptr;
static pfn_GetTimeZoneInformationForYear pfnOrig_GetTimeZoneInformationForYear = nullptr;
static pfn_GetLocalTime pfnOrig_GetLocalTime = nullptr;
static pfn_SystemTimeToTzSpecificLocalTime pfnOrig_SystemTimeToTzSpecificLocalTime = nullptr;
static pfn_SystemTimeToTzSpecificLocalTimeEx pfnOrig_SystemTimeToTzSpecificLocalTimeEx = nullptr;
static pfn_TzSpecificLocalTimeToSystemTime pfnOrig_TzSpecificLocalTimeToSystemTime = nullptr;
static pfn_TzSpecificLocalTimeToSystemTimeEx pfnOrig_TzSpecificLocalTimeToSystemTimeEx = nullptr;
static pfn_FileTimeToLocalFileTime pfnOrig_FileTimeToLocalFileTime = nullptr;
static pfn_LocalFileTimeToFileTime pfnOrig_LocalFileTimeToFileTime = nullptr;
static pfn_GetSystemTime pfnOrig_GetSystemTime = nullptr;
static pfn_GetSystemTimeAsFileTime pfnOrig_GetSystemTimeAsFileTime = nullptr;
static pfn_GetSystemTimePreciseAsFileTime pfnOrig_GetSystemTimePreciseAsFileTime = nullptr;
static pfn_NtQuerySystemInformation pfnOrig_NtQuerySystemInformation = nullptr;
static pfn_RegQueryValueExW pfnOrig_RegQueryValueExW = nullptr;

// Active spoofed configuration protected by SRWLock
static SRWLOCK g_tzLock = SRWLOCK_INIT;
static DYNAMIC_TIME_ZONE_INFORMATION g_targetDtzi = {};
static TIME_ZONE_INFORMATION g_targetTzi = {};
static int g_nTimeShiftMinutes = 0;
static bool g_bSpoofUtcTime = false;
static bool g_bInitialized = false;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void ShiftSystemTime(LPSYSTEMTIME pst, int minutes) {
    if (!pst || minutes == 0) return;
    FILETIME ft;
    if (SystemTimeToFileTime(pst, &ft)) {
        ULARGE_INTEGER ul;
        ul.LowPart = ft.dwLowDateTime;
        ul.HighPart = ft.dwHighDateTime;
        ul.QuadPart += (LONGLONG)minutes * 600000000LL;
        ft.dwLowDateTime = ul.LowPart;
        ft.dwHighDateTime = ul.HighPart;
        FileTimeToSystemTime(&ft, pst);
    }
}

static DWORD GetCurrentTimeZoneState(const DYNAMIC_TIME_ZONE_INFORMATION* pDtzi) {
    if (!pDtzi) return TIME_ZONE_ID_UNKNOWN;

    if (pDtzi->DynamicDaylightTimeDisabled || pDtzi->StandardDate.wMonth == 0 || pDtzi->DaylightDate.wMonth == 0) {
        return TIME_ZONE_ID_STANDARD;
    }

    TIME_ZONE_INFORMATION tzi = {};
    memcpy(&tzi, pDtzi, sizeof(TIME_ZONE_INFORMATION));

    DWORD res = GetTimeZoneInformation(&tzi);
    return (res != TIME_ZONE_ID_INVALID) ? res : TIME_ZONE_ID_STANDARD;
}

// ---------------------------------------------------------------------------
// Time Zone Resolution via Windows ICU & Direct Registry Enumeration
// ---------------------------------------------------------------------------

typedef int32_t (WINAPI *pfn_ucal_getWindowsTimeZoneID)(
    const WCHAR* id, int32_t len, WCHAR* winid, int32_t winidCapacity, int32_t* status);

static bool WindowsZoneFromIana(PCWSTR ianaId, WCHAR* outWinId, int32_t outCapacity) {
    if (!ianaId || !*ianaId || !outWinId || outCapacity <= 0) return false;

    HMODULE hIcu = LoadLibraryExW(L"icu.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hIcu) return false;

    auto pfn = (pfn_ucal_getWindowsTimeZoneID)GetProcAddress(hIcu, "ucal_getWindowsTimeZoneID");
    if (!pfn) {
        FreeLibrary(hIcu);
        return false;
    }

    int32_t status = 0;
    int32_t len = pfn(ianaId, -1, outWinId, outCapacity, &status);
    FreeLibrary(hIcu);

    return (status <= 0 && len > 0);
}

static BOOL ResolveTargetTimeZone(PCWSTR tzQuery, DYNAMIC_TIME_ZONE_INFORMATION* pOut) {
    if (!tzQuery || !*tzQuery || !pOut) return FALSE;

    WCHAR winKeyBuf[128] = {};
    PCWSTR targetKey = tzQuery;

    // Resolve IANA identifier to Windows Time Zone Key Name via icu.dll
    if (WindowsZoneFromIana(tzQuery, winKeyBuf, ARRAYSIZE(winKeyBuf))) {
        targetKey = winKeyBuf;
    }

    // 1. Exact match by TimeZoneKeyName
    DWORD idx = 0;
    DYNAMIC_TIME_ZONE_INFORMATION dtzi;
    while (EnumDynamicTimeZoneInformation(idx, &dtzi) == ERROR_SUCCESS) {
        if (_wcsicmp(dtzi.TimeZoneKeyName, targetKey) == 0) {
            *pOut = dtzi;
            return TRUE;
        }
        idx++;
    }

    // 2. Exact match by StandardName
    idx = 0;
    while (EnumDynamicTimeZoneInformation(idx, &dtzi) == ERROR_SUCCESS) {
        if (_wcsicmp(dtzi.StandardName, targetKey) == 0) {
            *pOut = dtzi;
            return TRUE;
        }
        idx++;
    }

    return FALSE;
}

// ---------------------------------------------------------------------------
// Detour Functions
// ---------------------------------------------------------------------------

static DWORD WINAPI Hook_GetDynamicTimeZoneInformation(PDYNAMIC_TIME_ZONE_INFORMATION pdtzi) {
    if (!g_bInitialized || !pdtzi) {
        return pfnOrig_GetDynamicTimeZoneInformation ? pfnOrig_GetDynamicTimeZoneInformation(pdtzi) : TIME_ZONE_ID_INVALID;
    }

    AcquireSRWLockShared(&g_tzLock);
    *pdtzi = g_targetDtzi;
    DWORD state = GetCurrentTimeZoneState(&g_targetDtzi);
    ReleaseSRWLockShared(&g_tzLock);

    return state;
}

static DWORD WINAPI Hook_GetTimeZoneInformation(LPTIME_ZONE_INFORMATION ptzi) {
    if (!g_bInitialized || !ptzi) {
        return pfnOrig_GetTimeZoneInformation ? pfnOrig_GetTimeZoneInformation(ptzi) : TIME_ZONE_ID_INVALID;
    }

    AcquireSRWLockShared(&g_tzLock);
    *ptzi = g_targetTzi;
    DWORD state = GetCurrentTimeZoneState(&g_targetDtzi);
    ReleaseSRWLockShared(&g_tzLock);

    return state;
}

static BOOL WINAPI Hook_GetTimeZoneInformationForYear(USHORT wYear, PDYNAMIC_TIME_ZONE_INFORMATION pdtzi, LPTIME_ZONE_INFORMATION ptzi) {
    if (!g_bInitialized) {
        return pfnOrig_GetTimeZoneInformationForYear ? pfnOrig_GetTimeZoneInformationForYear(wYear, pdtzi, ptzi) : FALSE;
    }

    DYNAMIC_TIME_ZONE_INFORMATION localDtzi;
    AcquireSRWLockShared(&g_tzLock);
    localDtzi = g_targetDtzi;
    ReleaseSRWLockShared(&g_tzLock);

    const DYNAMIC_TIME_ZONE_INFORMATION* pSource = pdtzi ? pdtzi : &localDtzi;
    return pfnOrig_GetTimeZoneInformationForYear ?
        pfnOrig_GetTimeZoneInformationForYear(wYear, (PDYNAMIC_TIME_ZONE_INFORMATION)pSource, ptzi) : FALSE;
}

static VOID WINAPI Hook_GetLocalTime(LPSYSTEMTIME lpSystemTime) {
    if (!g_bInitialized || !lpSystemTime) {
        if (pfnOrig_GetLocalTime) pfnOrig_GetLocalTime(lpSystemTime);
        return;
    }

    SYSTEMTIME stUtc;
    GetSystemTime(&stUtc);

    TIME_ZONE_INFORMATION localTzi;
    int nShift = 0;
    bool bSpoofUtc = false;

    AcquireSRWLockShared(&g_tzLock);
    localTzi = g_targetTzi;
    nShift = g_nTimeShiftMinutes;
    bSpoofUtc = g_bSpoofUtcTime;
    ReleaseSRWLockShared(&g_tzLock);

    if (pfnOrig_SystemTimeToTzSpecificLocalTime) {
        pfnOrig_SystemTimeToTzSpecificLocalTime(&localTzi, &stUtc, lpSystemTime);
    } else {
        SystemTimeToTzSpecificLocalTime(&localTzi, &stUtc, lpSystemTime);
    }

    // Only apply time shift here if UTC was NOT already shifted by Hook_GetSystemTime
    if (!bSpoofUtc && nShift != 0) {
        ShiftSystemTime(lpSystemTime, nShift);
    }
}

static BOOL WINAPI Hook_SystemTimeToTzSpecificLocalTime(
    const TIME_ZONE_INFORMATION *lpTimeZoneInformation,
    const SYSTEMTIME *lpUniversalTime,
    LPSYSTEMTIME lpLocalTime
) {
    if (!g_bInitialized) {
        return pfnOrig_SystemTimeToTzSpecificLocalTime ?
            pfnOrig_SystemTimeToTzSpecificLocalTime(lpTimeZoneInformation, lpUniversalTime, lpLocalTime) : FALSE;
    }

    TIME_ZONE_INFORMATION localTzi;
    int nShift = 0;
    bool bSpoofUtc = false;

    AcquireSRWLockShared(&g_tzLock);
    localTzi = g_targetTzi;
    nShift = g_nTimeShiftMinutes;
    bSpoofUtc = g_bSpoofUtcTime;
    ReleaseSRWLockShared(&g_tzLock);

    const TIME_ZONE_INFORMATION* pTziToUse = lpTimeZoneInformation ? lpTimeZoneInformation : &localTzi;

    BOOL result = pfnOrig_SystemTimeToTzSpecificLocalTime ?
        pfnOrig_SystemTimeToTzSpecificLocalTime(pTziToUse, lpUniversalTime, lpLocalTime) : FALSE;

    if (result && !lpTimeZoneInformation && nShift != 0 && !bSpoofUtc) {
        ShiftSystemTime(lpLocalTime, nShift);
    }

    return result;
}

static BOOL WINAPI Hook_SystemTimeToTzSpecificLocalTimeEx(
    const DYNAMIC_TIME_ZONE_INFORMATION *lpTimeZoneInformation,
    const SYSTEMTIME *lpUniversalTime,
    LPSYSTEMTIME lpLocalTime
) {
    if (!g_bInitialized) {
        return pfnOrig_SystemTimeToTzSpecificLocalTimeEx ?
            pfnOrig_SystemTimeToTzSpecificLocalTimeEx(lpTimeZoneInformation, lpUniversalTime, lpLocalTime) : FALSE;
    }

    DYNAMIC_TIME_ZONE_INFORMATION localDtzi;
    int nShift = 0;
    bool bSpoofUtc = false;

    AcquireSRWLockShared(&g_tzLock);
    localDtzi = g_targetDtzi;
    nShift = g_nTimeShiftMinutes;
    bSpoofUtc = g_bSpoofUtcTime;
    ReleaseSRWLockShared(&g_tzLock);

    const DYNAMIC_TIME_ZONE_INFORMATION* pDtziToUse = lpTimeZoneInformation ? lpTimeZoneInformation : &localDtzi;

    BOOL result = pfnOrig_SystemTimeToTzSpecificLocalTimeEx ?
        pfnOrig_SystemTimeToTzSpecificLocalTimeEx(pDtziToUse, lpUniversalTime, lpLocalTime) : FALSE;

    if (result && !lpTimeZoneInformation && nShift != 0 && !bSpoofUtc) {
        ShiftSystemTime(lpLocalTime, nShift);
    }

    return result;
}

static BOOL WINAPI Hook_TzSpecificLocalTimeToSystemTime(
    const TIME_ZONE_INFORMATION *lpTimeZoneInformation,
    const SYSTEMTIME *lpLocalTime,
    LPSYSTEMTIME lpUniversalTime
) {
    if (!g_bInitialized) {
        return pfnOrig_TzSpecificLocalTimeToSystemTime ?
            pfnOrig_TzSpecificLocalTimeToSystemTime(lpTimeZoneInformation, lpLocalTime, lpUniversalTime) : FALSE;
    }

    TIME_ZONE_INFORMATION localTzi;
    int nShift = 0;
    bool bSpoofUtc = false;

    AcquireSRWLockShared(&g_tzLock);
    localTzi = g_targetTzi;
    nShift = g_nTimeShiftMinutes;
    bSpoofUtc = g_bSpoofUtcTime;
    ReleaseSRWLockShared(&g_tzLock);

    const TIME_ZONE_INFORMATION* pTziToUse = lpTimeZoneInformation ? lpTimeZoneInformation : &localTzi;

    SYSTEMTIME stAdjusted = *lpLocalTime;
    if (!lpTimeZoneInformation && nShift != 0 && !bSpoofUtc) {
        ShiftSystemTime(&stAdjusted, -nShift);
    }

    return pfnOrig_TzSpecificLocalTimeToSystemTime ?
        pfnOrig_TzSpecificLocalTimeToSystemTime(pTziToUse, &stAdjusted, lpUniversalTime) : FALSE;
}

static BOOL WINAPI Hook_TzSpecificLocalTimeToSystemTimeEx(
    const DYNAMIC_TIME_ZONE_INFORMATION *lpTimeZoneInformation,
    const SYSTEMTIME *lpLocalTime,
    LPSYSTEMTIME lpUniversalTime
) {
    if (!g_bInitialized) {
        return pfnOrig_TzSpecificLocalTimeToSystemTimeEx ?
            pfnOrig_TzSpecificLocalTimeToSystemTimeEx(lpTimeZoneInformation, lpLocalTime, lpUniversalTime) : FALSE;
    }

    DYNAMIC_TIME_ZONE_INFORMATION localDtzi;
    int nShift = 0;
    bool bSpoofUtc = false;

    AcquireSRWLockShared(&g_tzLock);
    localDtzi = g_targetDtzi;
    nShift = g_nTimeShiftMinutes;
    bSpoofUtc = g_bSpoofUtcTime;
    ReleaseSRWLockShared(&g_tzLock);

    const DYNAMIC_TIME_ZONE_INFORMATION* pDtziToUse = lpTimeZoneInformation ? lpTimeZoneInformation : &localDtzi;

    SYSTEMTIME stAdjusted = *lpLocalTime;
    if (!lpTimeZoneInformation && nShift != 0 && !bSpoofUtc) {
        ShiftSystemTime(&stAdjusted, -nShift);
    }

    return pfnOrig_TzSpecificLocalTimeToSystemTimeEx ?
        pfnOrig_TzSpecificLocalTimeToSystemTimeEx(pDtziToUse, &stAdjusted, lpUniversalTime) : FALSE;
}

static BOOL WINAPI Hook_FileTimeToLocalFileTime(const FILETIME *lpFileTime, LPFILETIME lpLocalFileTime) {
    if (!g_bInitialized || !lpFileTime || !lpLocalFileTime) {
        return pfnOrig_FileTimeToLocalFileTime ? pfnOrig_FileTimeToLocalFileTime(lpFileTime, lpLocalFileTime) : FALSE;
    }

    SYSTEMTIME stUtc, stLocal;
    if (!FileTimeToSystemTime(lpFileTime, &stUtc)) return FALSE;

    TIME_ZONE_INFORMATION localTzi;
    int nShift = 0;
    bool bSpoofUtc = false;

    AcquireSRWLockShared(&g_tzLock);
    localTzi = g_targetTzi;
    nShift = g_nTimeShiftMinutes;
    bSpoofUtc = g_bSpoofUtcTime;
    ReleaseSRWLockShared(&g_tzLock);

    if (pfnOrig_SystemTimeToTzSpecificLocalTime) {
        pfnOrig_SystemTimeToTzSpecificLocalTime(&localTzi, &stUtc, &stLocal);
    } else {
        SystemTimeToTzSpecificLocalTime(&localTzi, &stUtc, &stLocal);
    }

    if (nShift != 0 && !bSpoofUtc) {
        ShiftSystemTime(&stLocal, nShift);
    }

    return SystemTimeToFileTime(&stLocal, lpLocalFileTime);
}

static BOOL WINAPI Hook_LocalFileTimeToFileTime(const FILETIME *lpLocalFileTime, LPFILETIME lpFileTime) {
    if (!g_bInitialized || !lpLocalFileTime || !lpFileTime) {
        return pfnOrig_LocalFileTimeToFileTime ? pfnOrig_LocalFileTimeToFileTime(lpLocalFileTime, lpFileTime) : FALSE;
    }

    SYSTEMTIME stLocal, stUtc;
    if (!FileTimeToSystemTime(lpLocalFileTime, &stLocal)) return FALSE;

    TIME_ZONE_INFORMATION localTzi;
    int nShift = 0;
    bool bSpoofUtc = false;

    AcquireSRWLockShared(&g_tzLock);
    localTzi = g_targetTzi;
    nShift = g_nTimeShiftMinutes;
    bSpoofUtc = g_bSpoofUtcTime;
    ReleaseSRWLockShared(&g_tzLock);

    if (nShift != 0 && !bSpoofUtc) {
        ShiftSystemTime(&stLocal, -nShift);
    }

    if (pfnOrig_TzSpecificLocalTimeToSystemTime) {
        pfnOrig_TzSpecificLocalTimeToSystemTime(&localTzi, &stLocal, &stUtc);
    } else {
        TzSpecificLocalTimeToSystemTime(&localTzi, &stLocal, &stUtc);
    }

    return SystemTimeToFileTime(&stUtc, lpFileTime);
}

static VOID WINAPI Hook_GetSystemTime(LPSYSTEMTIME lpSystemTime) {
    if (pfnOrig_GetSystemTime) pfnOrig_GetSystemTime(lpSystemTime);
    else GetSystemTime(lpSystemTime);

    if (!g_bInitialized) return;

    int nShift = 0;
    bool bSpoofUtc = false;

    AcquireSRWLockShared(&g_tzLock);
    nShift = g_nTimeShiftMinutes;
    bSpoofUtc = g_bSpoofUtcTime;
    ReleaseSRWLockShared(&g_tzLock);

    if (bSpoofUtc && nShift != 0) {
        ShiftSystemTime(lpSystemTime, nShift);
    }
}

static VOID WINAPI Hook_GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime) {
    if (pfnOrig_GetSystemTimeAsFileTime) pfnOrig_GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
    else GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);

    if (!g_bInitialized || !lpSystemTimeAsFileTime) return;

    int nShift = 0;
    bool bSpoofUtc = false;

    AcquireSRWLockShared(&g_tzLock);
    nShift = g_nTimeShiftMinutes;
    bSpoofUtc = g_bSpoofUtcTime;
    ReleaseSRWLockShared(&g_tzLock);

    if (bSpoofUtc && nShift != 0) {
        ULARGE_INTEGER ul;
        ul.LowPart = lpSystemTimeAsFileTime->dwLowDateTime;
        ul.HighPart = lpSystemTimeAsFileTime->dwHighDateTime;
        ul.QuadPart += (LONGLONG)nShift * 600000000LL;
        lpSystemTimeAsFileTime->dwLowDateTime = ul.LowPart;
        lpSystemTimeAsFileTime->dwHighDateTime = ul.HighPart;
    }
}

static VOID WINAPI Hook_GetSystemTimePreciseAsFileTime(LPFILETIME lpSystemTimeAsFileTime) {
    if (pfnOrig_GetSystemTimePreciseAsFileTime) pfnOrig_GetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
    else if (pfnOrig_GetSystemTimeAsFileTime) pfnOrig_GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
    else GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);

    if (!g_bInitialized || !lpSystemTimeAsFileTime) return;

    int nShift = 0;
    bool bSpoofUtc = false;

    AcquireSRWLockShared(&g_tzLock);
    nShift = g_nTimeShiftMinutes;
    bSpoofUtc = g_bSpoofUtcTime;
    ReleaseSRWLockShared(&g_tzLock);

    if (bSpoofUtc && nShift != 0) {
        ULARGE_INTEGER ul;
        ul.LowPart = lpSystemTimeAsFileTime->dwLowDateTime;
        ul.HighPart = lpSystemTimeAsFileTime->dwHighDateTime;
        ul.QuadPart += (LONGLONG)nShift * 600000000LL;
        lpSystemTimeAsFileTime->dwLowDateTime = ul.LowPart;
        lpSystemTimeAsFileTime->dwHighDateTime = ul.HighPart;
    }
}

static NTSTATUS NTAPI Hook_NtQuerySystemInformation(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
) {
    NTSTATUS status = pfnOrig_NtQuerySystemInformation ?
        pfnOrig_NtQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength) :
        STATUS_NOT_IMPLEMENTED;

    if (NT_SUCCESS(status) && g_bInitialized && SystemInformationClass == SystemTimeOfDayInformation &&
        SystemInformationLength >= sizeof(SYSTEM_TIMEOFDAY_INFORMATION_LOCAL) && SystemInformation) {

        DYNAMIC_TIME_ZONE_INFORMATION localDtzi;
        AcquireSRWLockShared(&g_tzLock);
        localDtzi = g_targetDtzi;
        ReleaseSRWLockShared(&g_tzLock);

        auto* pTod = (SYSTEM_TIMEOFDAY_INFORMATION_LOCAL*)SystemInformation;
        DWORD state = GetCurrentTimeZoneState(&localDtzi);
        LONG bias = localDtzi.Bias + (state == TIME_ZONE_ID_DAYLIGHT ? localDtzi.DaylightBias : localDtzi.StandardBias);

        pTod->TimeZoneBias.QuadPart = (LONGLONG)bias * 600000000LL;
        pTod->TimeZoneId = state;
    }

    return status;
}

static LSTATUS WINAPI Hook_RegQueryValueExW(
    HKEY hKey,
    LPCWSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
) {
    if (!g_bInitialized || !lpValueName) {
        return pfnOrig_RegQueryValueExW ?
            pfnOrig_RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData) :
            ERROR_CALL_NOT_IMPLEMENTED;
    }

    if (_wcsicmp(lpValueName, L"TimeZoneKeyName") == 0) {
        DYNAMIC_TIME_ZONE_INFORMATION localDtzi;
        AcquireSRWLockShared(&g_tzLock);
        localDtzi = g_targetDtzi;
        ReleaseSRWLockShared(&g_tzLock);

        DWORD cbNeeded = (DWORD)((wcslen(localDtzi.TimeZoneKeyName) + 1) * sizeof(WCHAR));
        if (lpType) *lpType = REG_SZ;
        if (!lpData) {
            if (lpcbData) *lpcbData = cbNeeded;
            return ERROR_SUCCESS;
        }
        if (lpcbData && *lpcbData < cbNeeded) {
            *lpcbData = cbNeeded;
            return ERROR_MORE_DATA;
        }
        memcpy(lpData, localDtzi.TimeZoneKeyName, cbNeeded);
        if (lpcbData) *lpcbData = cbNeeded;
        return ERROR_SUCCESS;
    }

    if (_wcsicmp(lpValueName, L"ActiveTimeBias") == 0) {
        DYNAMIC_TIME_ZONE_INFORMATION localDtzi;
        AcquireSRWLockShared(&g_tzLock);
        localDtzi = g_targetDtzi;
        ReleaseSRWLockShared(&g_tzLock);

        DWORD state = GetCurrentTimeZoneState(&localDtzi);
        LONG activeBias = localDtzi.Bias + (state == TIME_ZONE_ID_DAYLIGHT ? localDtzi.DaylightBias : localDtzi.StandardBias);

        if (lpType) *lpType = REG_DWORD;
        if (!lpData) {
            if (lpcbData) *lpcbData = sizeof(DWORD);
            return ERROR_SUCCESS;
        }
        if (lpcbData && *lpcbData < sizeof(DWORD)) {
            *lpcbData = sizeof(DWORD);
            return ERROR_MORE_DATA;
        }
        *(DWORD*)lpData = (DWORD)activeBias;
        if (lpcbData) *lpcbData = sizeof(DWORD);
        return ERROR_SUCCESS;
    }

    return pfnOrig_RegQueryValueExW ?
        pfnOrig_RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData) :
        ERROR_CALL_NOT_IMPLEMENTED;
}

// ---------------------------------------------------------------------------
// Settings Loader & Notifications
// ---------------------------------------------------------------------------

static void LoadSettings() {
    PCWSTR targetTzSetting = Wh_GetStringSetting(L"targetTimezone");
    PCWSTR customKeySetting = Wh_GetStringSetting(L"customTimezoneKey");

    std::wstring tzQuery;
    if (targetTzSetting && wcscmp(targetTzSetting, L"Custom") == 0) {
        if (customKeySetting && *customKeySetting) {
            tzQuery = customKeySetting;
        } else {
            tzQuery = L"UTC";
        }
    } else if (targetTzSetting && *targetTzSetting) {
        tzQuery = targetTzSetting;
    } else {
        tzQuery = L"Tokyo Standard Time";
    }

    DYNAMIC_TIME_ZONE_INFORMATION newDtzi = {};
    if (ResolveTargetTimeZone(tzQuery.c_str(), &newDtzi)) {
        TIME_ZONE_INFORMATION newTzi = {};
        memcpy(&newTzi, &newDtzi, sizeof(TIME_ZONE_INFORMATION));

        int newShift = Wh_GetIntSetting(L"timeShiftMinutes");
        bool newSpoofUtc = Wh_GetIntSetting(L"spoofUtcTime") != 0;

        AcquireSRWLockExclusive(&g_tzLock);
        g_targetDtzi = newDtzi;
        g_targetTzi = newTzi;
        g_nTimeShiftMinutes = newShift;
        g_bSpoofUtcTime = newSpoofUtc;
        g_bInitialized = true;
        ReleaseSRWLockExclusive(&g_tzLock);

        Wh_Log(L"Time zone active: %ls (Bias: %d min, Shift: %d min, SpoofUTC: %d)",
               newDtzi.TimeZoneKeyName, newDtzi.Bias, newShift, newSpoofUtc ? 1 : 0);
    } else {
        Wh_Log(L"Failed to resolve time zone: %ls", tzQuery.c_str());
    }
}

static BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == static_cast<DWORD>(lParam)) {
        PostMessageW(hwnd, WM_TIMECHANGE, 0, 0);
    }
    return TRUE;
}

static void NotifyProcessWindows() {
    EnumWindows(EnumWindowsCallback, static_cast<LPARAM>(GetCurrentProcessId()));
}

// ---------------------------------------------------------------------------
// Windhawk Lifecycle Entry Points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing ZoneShift...");

    LoadSettings();
    if (!g_bInitialized) {
        Wh_Log(L"Unknown time zone, not loading");
        return FALSE;
    }

    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!hKernelBase) {
        Wh_Log(L"Failed to obtain handle to kernelbase.dll");
        return FALSE;
    }

    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");

    // Hook time zone APIs
    if (auto pfn = (pfn_GetDynamicTimeZoneInformation)GetProcAddress(hKernelBase, "GetDynamicTimeZoneInformation")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetDynamicTimeZoneInformation, &pfnOrig_GetDynamicTimeZoneInformation);
    }
    if (auto pfn = (pfn_GetTimeZoneInformation)GetProcAddress(hKernelBase, "GetTimeZoneInformation")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetTimeZoneInformation, &pfnOrig_GetTimeZoneInformation);
    }
    if (auto pfn = (pfn_GetTimeZoneInformationForYear)GetProcAddress(hKernelBase, "GetTimeZoneInformationForYear")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetTimeZoneInformationForYear, &pfnOrig_GetTimeZoneInformationForYear);
    }

    // Hook local time APIs
    if (auto pfn = (pfn_GetLocalTime)GetProcAddress(hKernelBase, "GetLocalTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetLocalTime, &pfnOrig_GetLocalTime);
    }
    if (auto pfn = (pfn_SystemTimeToTzSpecificLocalTime)GetProcAddress(hKernelBase, "SystemTimeToTzSpecificLocalTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_SystemTimeToTzSpecificLocalTime, &pfnOrig_SystemTimeToTzSpecificLocalTime);
    }
    if (auto pfn = (pfn_SystemTimeToTzSpecificLocalTimeEx)GetProcAddress(hKernelBase, "SystemTimeToTzSpecificLocalTimeEx")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_SystemTimeToTzSpecificLocalTimeEx, &pfnOrig_SystemTimeToTzSpecificLocalTimeEx);
    }
    if (auto pfn = (pfn_TzSpecificLocalTimeToSystemTime)GetProcAddress(hKernelBase, "TzSpecificLocalTimeToSystemTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_TzSpecificLocalTimeToSystemTime, &pfnOrig_TzSpecificLocalTimeToSystemTime);
    }
    if (auto pfn = (pfn_TzSpecificLocalTimeToSystemTimeEx)GetProcAddress(hKernelBase, "TzSpecificLocalTimeToSystemTimeEx")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_TzSpecificLocalTimeToSystemTimeEx, &pfnOrig_TzSpecificLocalTimeToSystemTimeEx);
    }

    // Hook file time APIs
    if (auto pfn = (pfn_FileTimeToLocalFileTime)GetProcAddress(hKernelBase, "FileTimeToLocalFileTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_FileTimeToLocalFileTime, &pfnOrig_FileTimeToLocalFileTime);
    }
    if (auto pfn = (pfn_LocalFileTimeToFileTime)GetProcAddress(hKernelBase, "LocalFileTimeToFileTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_LocalFileTimeToFileTime, &pfnOrig_LocalFileTimeToFileTime);
    }

    // Hook system time APIs
    if (auto pfn = (pfn_GetSystemTime)GetProcAddress(hKernelBase, "GetSystemTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetSystemTime, &pfnOrig_GetSystemTime);
    }
    if (auto pfn = (pfn_GetSystemTimeAsFileTime)GetProcAddress(hKernelBase, "GetSystemTimeAsFileTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetSystemTimeAsFileTime, &pfnOrig_GetSystemTimeAsFileTime);
    }
    if (auto pfn = (pfn_GetSystemTimePreciseAsFileTime)GetProcAddress(hKernelBase, "GetSystemTimePreciseAsFileTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetSystemTimePreciseAsFileTime, &pfnOrig_GetSystemTimePreciseAsFileTime);
    }

    // Hook NT and registry queries
    if (hNtdll) {
        if (auto pfn = (pfn_NtQuerySystemInformation)GetProcAddress(hNtdll, "NtQuerySystemInformation")) {
            WindhawkUtils::SetFunctionHook(pfn, Hook_NtQuerySystemInformation, &pfnOrig_NtQuerySystemInformation);
        }
    }

    if (auto pfn = (pfn_RegQueryValueExW)GetProcAddress(hKernelBase, "RegQueryValueExW")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_RegQueryValueExW, &pfnOrig_RegQueryValueExW);
    }

    Wh_Log(L"ZoneShift initialized successfully.");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"ZoneShift settings changed, updating...");
    LoadSettings();
    NotifyProcessWindows();
}

void Wh_ModUninit() {
    Wh_Log(L"ZoneShift uninitialized.");
}
