// ==WindhawkMod==
// @id              zoneshift
// @name            ZoneShift - Per-App Timezone Spoofer
// @description     Spoof timezone and local time for specific apps (Wallpaper Engine, browsers, games, clients) without altering the Windows system clock.
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
// @include         chrome.exe
// @include         msedge.exe
// @include         brave.exe
// @include         firefox.exe
// @include         Discord.exe
// @include         Slack.exe
// @include         Telegram.exe
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

5. **Gaming & Daily Resets**:
   - Trigger region-specific daily login rewards, server refresh timers, or in-game events in client-based games.

---

### How It Works

Windows apps query time through multiple Win32, NT, ICU, CRT, and registry APIs. ZoneShift intercepts all critical layers at the process level:
- **`GetDynamicTimeZoneInformation` & `GetTimeZoneInformation`**: Modern Win32 APIs used by Chromium, Electron, CEF, Node.js, and ICU to resolve IANA/Windows timezones.
- **`GetLocalTime`**: Native API called by C++/DirectX apps, desktop widgets, and Wallpaper Engine scene wallpapers.
- **`SystemTimeToTzSpecificLocalTime` / `TzSpecificLocalTimeToSystemTime`**: Handles local-to-UTC and UTC-to-local conversions with full Daylight Saving Time (DST) support.
- **`FileTimeToLocalFileTime` / `LocalFileTimeToFileTime`**: File and timestamp conversions.
- **`NtQuerySystemInformation` (`SystemTimeOfDayInformation`)**: Low-level NT kernel time-of-day bias.
- **`RegQueryValueExW`**: Registry queries targeting `HKLM\SYSTEM\CurrentControlSet\Control\TimeZoneInformation`.

---

### Adding Any Application

By default, ZoneShift targets Wallpaper Engine, common browsers, and messaging apps. To target **any custom application**:
1. Open the **Windhawk UI**.
2. Go to **ZoneShift** -> **Details** -> **Advanced** tab.
3. In the **Process inclusion list**, add the executable name of your app (e.g., `yourapp.exe`, `game.exe`, `steam.exe`).
4. Click **Save**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetTimezone: Tokyo Standard Time
  $name: Target Time Zone
  $description: Select the spoofed time zone. Popular presets are provided. Choose "Custom" to enter any Windows or IANA time zone below.
  $options:
    - Tokyo Standard Time: "Tokyo, Japan (UTC+09:00)"
    - GMT Standard Time: "London, UK (GMT / BST, UTC+00:00 / UTC+01:00)"
    - UTC: "UTC (Coordinated Universal Time, UTC+00:00)"
    - W. Europe Standard Time: "Berlin, Paris, Rome, Madrid (CET / CEST, UTC+01:00 / UTC+02:00)"
    - Eastern Standard Time: "New York, Toronto (US Eastern, UTC-05:00 / UTC-04:00)"
    - Central Standard Time: "Chicago, Dallas (US Central, UTC-06:00 / UTC-05:00)"
    - Mountain Standard Time: "Denver (US Mountain, UTC-07:00 / UTC-06:00)"
    - Pacific Standard Time: "Los Angeles, Seattle (US Pacific, UTC-08:00 / UTC-07:00)"
    - Hawaiian Standard Time: "Honolulu, Hawaii (UTC-10:00)"
    - India Standard Time: "New Delhi, Mumbai, Kolkata (IST, UTC+05:30)"
    - China Standard Time: "Beijing, Shanghai, Hong Kong (UTC+08:00)"
    - Singapore Standard Time: "Singapore (UTC+08:00)"
    - Korea Standard Time: "Seoul, South Korea (UTC+09:00)"
    - AUS Eastern Standard Time: "Sydney, Melbourne (AEST / AEDT, UTC+10:00 / UTC+11:00)"
    - New Zealand Standard Time: "Auckland, Wellington (NZST / NZDT, UTC+12:00 / UTC+13:00)"
    - Arabian Standard Time: "Dubai, Abu Dhabi (UTC+04:00)"
    - Custom: "Custom (Specified below)"
- customTimezoneKey: ""
  $name: Custom Time Zone Key / Name
  $description: If "Custom" is selected above, enter any Windows Time Zone Key Name (e.g. "Alaskan Standard Time") or an IANA identifier (e.g. "Asia/Tokyo", "Europe/London").
- timeShiftMinutes: 0
  $name: Custom Time Shift (Minutes)
  $description: Additional time offset in minutes added to local time (positive or negative). 0 = normal time for the selected zone. Example: 120 adds 2 hours.
- spoofUtcTime: false
  $name: Shift UTC Time (GetSystemTime)
  $description: Applies the custom time shift to GetSystemTime / GetSystemTimeAsFileTime as well. Keep OFF for standard timezone spoofing.
- debugLogging: false
  $name: Enable Debug Logging
  $description: Output verbose hook messages to the Windhawk Mod Manager log console.
*/
// ==/WindhawkModSettings==

#ifndef WH_MOD_ID
#define WH_MOD_ID L"zoneshift"
#endif

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winternl.h>
#include <cwchar>

// ---------------------------------------------------------------------------
// Structs & Prototypes
// ---------------------------------------------------------------------------

#ifndef SystemTimeOfDayInformation
#define SystemTimeOfDayInformation 3
#endif

typedef struct _SYSTEM_TIMEOFDAY_INFORMATION_LOCAL {
    LARGE_INTEGER BootTime;
    LARGE_INTEGER CurrentTime;
    LARGE_INTEGER TimeZoneBias;
    ULONG TimeZoneId;
    ULONG Reserved;
    ULONGLONG BootId;
} SYSTEM_TIMEOFDAY_INFORMATION_LOCAL;

// Original function pointer types
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

// Active spoofed configuration
static DYNAMIC_TIME_ZONE_INFORMATION g_targetDtzi = {};
static TIME_ZONE_INFORMATION g_targetTzi = {};
static int g_nTimeShiftMinutes = 0;
static bool g_bSpoofUtcTime = false;
static bool g_bDebugLogging = false;
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

// Determines if Daylight Saving Time (DST) is active for the target timezone
static DWORD GetCurrentTimeZoneState(const DYNAMIC_TIME_ZONE_INFORMATION* pDtzi) {
    if (!pDtzi) return TIME_ZONE_ID_UNKNOWN;

    if (pDtzi->DynamicDaylightTimeDisabled || pDtzi->StandardDate.wMonth == 0 || pDtzi->DaylightDate.wMonth == 0) {
        return TIME_ZONE_ID_UNKNOWN;
    }

    SYSTEMTIME stUtc, stLocal;
    GetSystemTime(&stUtc);

    TIME_ZONE_INFORMATION tzi;
    memcpy(&tzi, pDtzi, sizeof(TIME_ZONE_INFORMATION));

    if (pfnOrig_SystemTimeToTzSpecificLocalTime) {
        if (!pfnOrig_SystemTimeToTzSpecificLocalTime(&tzi, &stUtc, &stLocal)) {
            return TIME_ZONE_ID_STANDARD;
        }
    } else {
        if (!SystemTimeToTzSpecificLocalTime(&tzi, &stUtc, &stLocal)) {
            return TIME_ZONE_ID_STANDARD;
        }
    }

    FILETIME ftUtc, ftLocal;
    SystemTimeToFileTime(&stUtc, &ftUtc);
    SystemTimeToFileTime(&stLocal, &ftLocal);

    ULARGE_INTEGER uUtc, uLocal;
    uUtc.LowPart = ftUtc.dwLowDateTime;
    uUtc.HighPart = ftUtc.dwHighDateTime;
    uLocal.LowPart = ftLocal.dwLowDateTime;
    uLocal.HighPart = ftLocal.dwHighDateTime;

    LONGLONG diff100ns = (LONGLONG)uUtc.QuadPart - (LONGLONG)uLocal.QuadPart;
    LONG diffMinutes = (LONG)(diff100ns / 600000000LL);

    if (diffMinutes == (pDtzi->Bias + pDtzi->DaylightBias)) {
        return TIME_ZONE_ID_DAYLIGHT;
    } else if (diffMinutes == (pDtzi->Bias + pDtzi->StandardBias)) {
        return TIME_ZONE_ID_STANDARD;
    }

    return TIME_ZONE_ID_STANDARD;
}

// Map common IANA zone strings to Windows Time Zone Keys
static PCWSTR MapIanaToWindows(PCWSTR iana) {
    if (!iana || !*iana) return nullptr;

    struct IanaMapping {
        PCWSTR iana;
        PCWSTR winKey;
    };

    static const IanaMapping mappings[] = {
        { L"Asia/Tokyo", L"Tokyo Standard Time" },
        { L"Asia/Seoul", L"Korea Standard Time" },
        { L"Asia/Shanghai", L"China Standard Time" },
        { L"Asia/Hong_Kong", L"China Standard Time" },
        { L"Asia/Taipei", L"Taipei Standard Time" },
        { L"Asia/Singapore", L"Singapore Standard Time" },
        { L"Asia/Kolkata", L"India Standard Time" },
        { L"Asia/Calcutta", L"India Standard Time" },
        { L"Asia/Dubai", L"Arabian Standard Time" },
        { L"Asia/Bangkok", L"SE Asia Standard Time" },
        { L"Asia/Jakarta", L"SE Asia Standard Time" },
        { L"Europe/London", L"GMT Standard Time" },
        { L"Europe/Dublin", L"GMT Standard Time" },
        { L"Europe/Paris", L"W. Europe Standard Time" },
        { L"Europe/Berlin", L"W. Europe Standard Time" },
        { L"Europe/Rome", L"W. Europe Standard Time" },
        { L"Europe/Madrid", L"Romance Standard Time" },
        { L"Europe/Amsterdam", L"W. Europe Standard Time" },
        { L"Europe/Athens", L"GTB Standard Time" },
        { L"Europe/Bucharest", L"GTB Standard Time" },
        { L"Europe/Helsinki", L"FLE Standard Time" },
        { L"Europe/Kyiv", L"FLE Standard Time" },
        { L"Europe/Moscow", L"Russian Standard Time" },
        { L"America/New_York", L"Eastern Standard Time" },
        { L"America/Toronto", L"Eastern Standard Time" },
        { L"America/Chicago", L"Central Standard Time" },
        { L"America/Denver", L"Mountain Standard Time" },
        { L"America/Phoenix", L"US Mountain Standard Time" },
        { L"America/Los_Angeles", L"Pacific Standard Time" },
        { L"America/Vancouver", L"Pacific Standard Time" },
        { L"America/Anchorage", L"Alaskan Standard Time" },
        { L"Pacific/Honolulu", L"Hawaiian Standard Time" },
        { L"America/Sao_Paulo", L"E. South America Standard Time" },
        { L"Australia/Sydney", L"AUS Eastern Standard Time" },
        { L"Australia/Melbourne", L"AUS Eastern Standard Time" },
        { L"Australia/Brisbane", L"E. Australia Standard Time" },
        { L"Australia/Adelaide", L"Cen. Australia Standard Time" },
        { L"Australia/Perth", L"W. Australia Standard Time" },
        { L"Pacific/Auckland", L"New Zealand Standard Time" },
        { L"UTC", L"UTC" },
        { L"Etc/UTC", L"UTC" },
        { L"Etc/GMT", L"UTC" },
    };

    for (const auto& item : mappings) {
        if (_wcsicmp(item.iana, iana) == 0) {
            return item.winKey;
        }
    }

    return nullptr;
}

// Resolves a timezone name, alias, or substring to full DYNAMIC_TIME_ZONE_INFORMATION
static BOOL ResolveTargetTimeZone(PCWSTR tzQuery, DYNAMIC_TIME_ZONE_INFORMATION* pOut) {
    if (!tzQuery || !*tzQuery || !pOut) return FALSE;

    HMODULE hAdvapi32 = LoadLibraryW(L"advapi32.dll");
    if (!hAdvapi32) return FALSE;

    typedef DWORD (WINAPI *EnumDynamicTimeZoneInformation_t)(DWORD, PDYNAMIC_TIME_ZONE_INFORMATION);
    auto pfnEnum = (EnumDynamicTimeZoneInformation_t)GetProcAddress(hAdvapi32, "EnumDynamicTimeZoneInformation");
    if (!pfnEnum) return FALSE;

    // Check IANA mapping first
    PCWSTR winKey = MapIanaToWindows(tzQuery);
    if (!winKey) winKey = tzQuery;

    // 1. Exact match by TimeZoneKeyName
    DWORD idx = 0;
    DYNAMIC_TIME_ZONE_INFORMATION dtzi;
    while (pfnEnum(idx, &dtzi) == ERROR_SUCCESS) {
        if (_wcsicmp(dtzi.TimeZoneKeyName, winKey) == 0) {
            *pOut = dtzi;
            return TRUE;
        }
        idx++;
    }

    // 2. Case-insensitive substring match
    idx = 0;
    WCHAR lowerQuery[128] = {};
    wcsncpy(lowerQuery, winKey, 127);
    _wcslwr(lowerQuery);

    while (pfnEnum(idx, &dtzi) == ERROR_SUCCESS) {
        WCHAR lowerKey[128] = {};
        wcsncpy(lowerKey, dtzi.TimeZoneKeyName, 127);
        _wcslwr(lowerKey);

        if (wcsstr(lowerKey, lowerQuery) != nullptr) {
            *pOut = dtzi;
            return TRUE;
        }
        idx++;
    }

    // 3. Substring match on StandardName
    idx = 0;
    while (pfnEnum(idx, &dtzi) == ERROR_SUCCESS) {
        WCHAR lowerStd[64] = {};
        wcsncpy(lowerStd, dtzi.StandardName, 63);
        _wcslwr(lowerStd);

        if (wcsstr(lowerStd, lowerQuery) != nullptr) {
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

    *pdtzi = g_targetDtzi;
    DWORD state = GetCurrentTimeZoneState(&g_targetDtzi);

    if (g_bDebugLogging) {
        Wh_Log(L"GetDynamicTimeZoneInformation called -> %ls, state=%u", g_targetDtzi.TimeZoneKeyName, state);
    }

    return state;
}

static DWORD WINAPI Hook_GetTimeZoneInformation(LPTIME_ZONE_INFORMATION ptzi) {
    if (!g_bInitialized || !ptzi) {
        return pfnOrig_GetTimeZoneInformation ? pfnOrig_GetTimeZoneInformation(ptzi) : TIME_ZONE_ID_INVALID;
    }

    *ptzi = g_targetTzi;
    DWORD state = GetCurrentTimeZoneState(&g_targetDtzi);

    if (g_bDebugLogging) {
        Wh_Log(L"GetTimeZoneInformation called -> %ls, state=%u", g_targetTzi.StandardName, state);
    }

    return state;
}

static BOOL WINAPI Hook_GetTimeZoneInformationForYear(USHORT wYear, PDYNAMIC_TIME_ZONE_INFORMATION pdtzi, LPTIME_ZONE_INFORMATION ptzi) {
    // If application passes NULL, Windows uses the current timezone. Use our target timezone instead.
    const DYNAMIC_TIME_ZONE_INFORMATION* pSource = pdtzi ? pdtzi : &g_targetDtzi;
    if (pfnOrig_GetTimeZoneInformationForYear) {
        return pfnOrig_GetTimeZoneInformationForYear(wYear, (PDYNAMIC_TIME_ZONE_INFORMATION)pSource, ptzi);
    }
    return FALSE;
}

static VOID WINAPI Hook_GetLocalTime(LPSYSTEMTIME lpSystemTime) {
    if (!g_bInitialized || !lpSystemTime) {
        if (pfnOrig_GetLocalTime) pfnOrig_GetLocalTime(lpSystemTime);
        return;
    }

    SYSTEMTIME stUtc;
    GetSystemTime(&stUtc);

    if (g_bSpoofUtcTime && g_nTimeShiftMinutes != 0) {
        ShiftSystemTime(&stUtc, g_nTimeShiftMinutes);
    }

    if (pfnOrig_SystemTimeToTzSpecificLocalTime) {
        pfnOrig_SystemTimeToTzSpecificLocalTime(&g_targetTzi, &stUtc, lpSystemTime);
    } else {
        SystemTimeToTzSpecificLocalTime(&g_targetTzi, &stUtc, lpSystemTime);
    }

    if (!g_bSpoofUtcTime && g_nTimeShiftMinutes != 0) {
        ShiftSystemTime(lpSystemTime, g_nTimeShiftMinutes);
    }

    if (g_bDebugLogging) {
        Wh_Log(L"GetLocalTime -> %02u:%02u:%02u", lpSystemTime->wHour, lpSystemTime->wMinute, lpSystemTime->wSecond);
    }
}

static BOOL WINAPI Hook_SystemTimeToTzSpecificLocalTime(
    const TIME_ZONE_INFORMATION *lpTimeZoneInformation,
    const SYSTEMTIME *lpUniversalTime,
    LPSYSTEMTIME lpLocalTime
) {
    const TIME_ZONE_INFORMATION* pTziToUse = lpTimeZoneInformation ? lpTimeZoneInformation : &g_targetTzi;

    BOOL result = FALSE;
    if (pfnOrig_SystemTimeToTzSpecificLocalTime) {
        result = pfnOrig_SystemTimeToTzSpecificLocalTime(pTziToUse, lpUniversalTime, lpLocalTime);
    }

    if (result && !lpTimeZoneInformation && g_nTimeShiftMinutes != 0 && !g_bSpoofUtcTime) {
        ShiftSystemTime(lpLocalTime, g_nTimeShiftMinutes);
    }

    return result;
}

static BOOL WINAPI Hook_SystemTimeToTzSpecificLocalTimeEx(
    const DYNAMIC_TIME_ZONE_INFORMATION *lpTimeZoneInformation,
    const SYSTEMTIME *lpUniversalTime,
    LPSYSTEMTIME lpLocalTime
) {
    const DYNAMIC_TIME_ZONE_INFORMATION* pDtziToUse = lpTimeZoneInformation ? lpTimeZoneInformation : &g_targetDtzi;

    BOOL result = FALSE;
    if (pfnOrig_SystemTimeToTzSpecificLocalTimeEx) {
        result = pfnOrig_SystemTimeToTzSpecificLocalTimeEx(pDtziToUse, lpUniversalTime, lpLocalTime);
    }

    if (result && !lpTimeZoneInformation && g_nTimeShiftMinutes != 0 && !g_bSpoofUtcTime) {
        ShiftSystemTime(lpLocalTime, g_nTimeShiftMinutes);
    }

    return result;
}

static BOOL WINAPI Hook_TzSpecificLocalTimeToSystemTime(
    const TIME_ZONE_INFORMATION *lpTimeZoneInformation,
    const SYSTEMTIME *lpLocalTime,
    LPSYSTEMTIME lpUniversalTime
) {
    const TIME_ZONE_INFORMATION* pTziToUse = lpTimeZoneInformation ? lpTimeZoneInformation : &g_targetTzi;

    SYSTEMTIME stAdjusted = *lpLocalTime;
    if (!lpTimeZoneInformation && g_nTimeShiftMinutes != 0 && !g_bSpoofUtcTime) {
        ShiftSystemTime(&stAdjusted, -g_nTimeShiftMinutes);
    }

    if (pfnOrig_TzSpecificLocalTimeToSystemTime) {
        return pfnOrig_TzSpecificLocalTimeToSystemTime(pTziToUse, &stAdjusted, lpUniversalTime);
    }
    return FALSE;
}

static BOOL WINAPI Hook_TzSpecificLocalTimeToSystemTimeEx(
    const DYNAMIC_TIME_ZONE_INFORMATION *lpTimeZoneInformation,
    const SYSTEMTIME *lpLocalTime,
    LPSYSTEMTIME lpUniversalTime
) {
    const DYNAMIC_TIME_ZONE_INFORMATION* pDtziToUse = lpTimeZoneInformation ? lpTimeZoneInformation : &g_targetDtzi;

    SYSTEMTIME stAdjusted = *lpLocalTime;
    if (!lpTimeZoneInformation && g_nTimeShiftMinutes != 0 && !g_bSpoofUtcTime) {
        ShiftSystemTime(&stAdjusted, -g_nTimeShiftMinutes);
    }

    if (pfnOrig_TzSpecificLocalTimeToSystemTimeEx) {
        return pfnOrig_TzSpecificLocalTimeToSystemTimeEx(pDtziToUse, &stAdjusted, lpUniversalTime);
    }
    return FALSE;
}

static BOOL WINAPI Hook_FileTimeToLocalFileTime(const FILETIME *lpFileTime, LPFILETIME lpLocalFileTime) {
    if (!g_bInitialized || !lpFileTime || !lpLocalFileTime) {
        return pfnOrig_FileTimeToLocalFileTime ? pfnOrig_FileTimeToLocalFileTime(lpFileTime, lpLocalFileTime) : FALSE;
    }

    SYSTEMTIME stUtc, stLocal;
    if (!FileTimeToSystemTime(lpFileTime, &stUtc)) return FALSE;

    if (pfnOrig_SystemTimeToTzSpecificLocalTime) {
        pfnOrig_SystemTimeToTzSpecificLocalTime(&g_targetTzi, &stUtc, &stLocal);
    } else {
        SystemTimeToTzSpecificLocalTime(&g_targetTzi, &stUtc, &stLocal);
    }

    if (g_nTimeShiftMinutes != 0) {
        ShiftSystemTime(&stLocal, g_nTimeShiftMinutes);
    }

    return SystemTimeToFileTime(&stLocal, lpLocalFileTime);
}

static BOOL WINAPI Hook_LocalFileTimeToFileTime(const FILETIME *lpLocalFileTime, LPFILETIME lpFileTime) {
    if (!g_bInitialized || !lpLocalFileTime || !lpFileTime) {
        return pfnOrig_LocalFileTimeToFileTime ? pfnOrig_LocalFileTimeToFileTime(lpLocalFileTime, lpFileTime) : FALSE;
    }

    SYSTEMTIME stLocal, stUtc;
    if (!FileTimeToSystemTime(lpLocalFileTime, &stLocal)) return FALSE;

    if (g_nTimeShiftMinutes != 0) {
        ShiftSystemTime(&stLocal, -g_nTimeShiftMinutes);
    }

    if (pfnOrig_TzSpecificLocalTimeToSystemTime) {
        pfnOrig_TzSpecificLocalTimeToSystemTime(&g_targetTzi, &stLocal, &stUtc);
    } else {
        TzSpecificLocalTimeToSystemTime(&g_targetTzi, &stLocal, &stUtc);
    }

    return SystemTimeToFileTime(&stUtc, lpFileTime);
}

static VOID WINAPI Hook_GetSystemTime(LPSYSTEMTIME lpSystemTime) {
    if (pfnOrig_GetSystemTime) pfnOrig_GetSystemTime(lpSystemTime);
    if (g_bSpoofUtcTime && g_nTimeShiftMinutes != 0 && lpSystemTime) {
        ShiftSystemTime(lpSystemTime, g_nTimeShiftMinutes);
    }
}

static VOID WINAPI Hook_GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime) {
    if (pfnOrig_GetSystemTimeAsFileTime) pfnOrig_GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
    if (g_bSpoofUtcTime && g_nTimeShiftMinutes != 0 && lpSystemTimeAsFileTime) {
        ULARGE_INTEGER ul;
        ul.LowPart = lpSystemTimeAsFileTime->dwLowDateTime;
        ul.HighPart = lpSystemTimeAsFileTime->dwHighDateTime;
        ul.QuadPart += (LONGLONG)g_nTimeShiftMinutes * 600000000LL;
        lpSystemTimeAsFileTime->dwLowDateTime = ul.LowPart;
        lpSystemTimeAsFileTime->dwHighDateTime = ul.HighPart;
    }
}

static VOID WINAPI Hook_GetSystemTimePreciseAsFileTime(LPFILETIME lpSystemTimeAsFileTime) {
    if (pfnOrig_GetSystemTimePreciseAsFileTime) pfnOrig_GetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
    if (g_bSpoofUtcTime && g_nTimeShiftMinutes != 0 && lpSystemTimeAsFileTime) {
        ULARGE_INTEGER ul;
        ul.LowPart = lpSystemTimeAsFileTime->dwLowDateTime;
        ul.HighPart = lpSystemTimeAsFileTime->dwHighDateTime;
        ul.QuadPart += (LONGLONG)g_nTimeShiftMinutes * 600000000LL;
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
        0xC0000002; // STATUS_NOT_IMPLEMENTED

    if (status >= 0 && SystemInformationClass == SystemTimeOfDayInformation &&
        SystemInformation && SystemInformationLength >= sizeof(SYSTEM_TIMEOFDAY_INFORMATION_LOCAL) && g_bInitialized) {
        auto* pInfo = (SYSTEM_TIMEOFDAY_INFORMATION_LOCAL*)SystemInformation;
        DWORD state = GetCurrentTimeZoneState(&g_targetDtzi);
        LONG activeBiasMinutes = g_targetDtzi.Bias + (state == TIME_ZONE_ID_DAYLIGHT ? g_targetDtzi.DaylightBias : g_targetDtzi.StandardBias);
        pInfo->TimeZoneBias.QuadPart = (LONGLONG)activeBiasMinutes * 60 * 10000000LL;
        pInfo->TimeZoneId = (state == TIME_ZONE_ID_DAYLIGHT) ? 2 : 1;
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
    if (g_bInitialized && lpValueName) {
        if (_wcsicmp(lpValueName, L"TimeZoneKeyName") == 0) {
            DWORD cbNeeded = (DWORD)((wcslen(g_targetDtzi.TimeZoneKeyName) + 1) * sizeof(WCHAR));
            if (lpType) *lpType = REG_SZ;
            if (lpcbData) {
                if (!lpData || *lpcbData < cbNeeded) {
                    *lpcbData = cbNeeded;
                    return lpData ? ERROR_MORE_DATA : ERROR_SUCCESS;
                }
                *lpcbData = cbNeeded;
                memcpy(lpData, g_targetDtzi.TimeZoneKeyName, cbNeeded);
                return ERROR_SUCCESS;
            }
        } else if (_wcsicmp(lpValueName, L"ActiveTimeBias") == 0) {
            DWORD cbNeeded = sizeof(DWORD);
            if (lpType) *lpType = REG_DWORD;
            if (lpcbData) {
                if (!lpData || *lpcbData < cbNeeded) {
                    *lpcbData = cbNeeded;
                    return lpData ? ERROR_MORE_DATA : ERROR_SUCCESS;
                }
                *lpcbData = cbNeeded;
                DWORD state = GetCurrentTimeZoneState(&g_targetDtzi);
                LONG activeBias = g_targetDtzi.Bias + (state == TIME_ZONE_ID_DAYLIGHT ? g_targetDtzi.DaylightBias : g_targetDtzi.StandardBias);
                memcpy(lpData, &activeBias, sizeof(DWORD));
                return ERROR_SUCCESS;
            }
        }
    }

    return pfnOrig_RegQueryValueExW ?
        pfnOrig_RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData) :
        ERROR_CALL_NOT_IMPLEMENTED;
}

// ---------------------------------------------------------------------------
// Settings & Lifecycle
// ---------------------------------------------------------------------------

static void LoadSettings() {
    auto targetTz = WindhawkUtils::StringSetting::make(L"targetTimezone");
    auto customTz = WindhawkUtils::StringSetting::make(L"customTimezoneKey");
    g_nTimeShiftMinutes = Wh_GetIntSetting(L"timeShiftMinutes");
    g_bSpoofUtcTime = Wh_GetIntSetting(L"spoofUtcTime") != 0;
    g_bDebugLogging = Wh_GetIntSetting(L"debugLogging") != 0;

    PCWSTR tzQuery = targetTz.get();
    if (_wcsicmp(tzQuery, L"Custom") == 0) {
        tzQuery = customTz.get();
    }

    if (!tzQuery || !*tzQuery) {
        tzQuery = L"Tokyo Standard Time";
    }

    DYNAMIC_TIME_ZONE_INFORMATION resolvedDtzi = {};
    if (ResolveTargetTimeZone(tzQuery, &resolvedDtzi)) {
        g_targetDtzi = resolvedDtzi;
        memcpy(&g_targetTzi, &resolvedDtzi, sizeof(TIME_ZONE_INFORMATION));
        g_bInitialized = true;
        Wh_Log(L"ZoneShift active: %ls (Bias: %d min, Shift: %d min)",
               g_targetDtzi.TimeZoneKeyName, g_targetDtzi.Bias, g_nTimeShiftMinutes);
    } else {
        Wh_Log(L"ZoneShift warning: Failed to resolve time zone '%ls'. Keeping previous configuration.", tzQuery);
    }
}

// Broadcasts setting change to current process top-level windows to refresh Chromium/CEF/UI caches
static BOOL CALLBACK EnumWindowsNotifyProc(HWND hWnd, LPARAM lParam) {
    (void)lParam;
    PostMessageW(hWnd, WM_SETTINGCHANGE, 0, (LPARAM)L"intl");
    return TRUE;
}

static void NotifyProcessWindows() {
    EnumThreadWindows(GetCurrentThreadId(), EnumWindowsNotifyProc, 0);
}

BOOL Wh_ModInit() {
    Wh_Log(L"ZoneShift initializing in process %u...", GetCurrentProcessId());

    LoadSettings();

    HMODULE hKernelBase = GetModuleHandleW(L"KernelBase.dll");
    if (!hKernelBase) hKernelBase = GetModuleHandleW(L"kernel32.dll");
    if (!hKernelBase) {
        Wh_Log(L"Failed to obtain handle to KernelBase.dll / kernel32.dll");
        return FALSE;
    }

    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    HMODULE hAdvapi32 = GetModuleHandleW(L"advapi32.dll");

    // Hook GetDynamicTimeZoneInformation
    if (auto pfn = (pfn_GetDynamicTimeZoneInformation)GetProcAddress(hKernelBase, "GetDynamicTimeZoneInformation")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetDynamicTimeZoneInformation, &pfnOrig_GetDynamicTimeZoneInformation);
    }

    // Hook GetTimeZoneInformation
    if (auto pfn = (pfn_GetTimeZoneInformation)GetProcAddress(hKernelBase, "GetTimeZoneInformation")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetTimeZoneInformation, &pfnOrig_GetTimeZoneInformation);
    }

    // Hook GetTimeZoneInformationForYear
    if (auto pfn = (pfn_GetTimeZoneInformationForYear)GetProcAddress(hKernelBase, "GetTimeZoneInformationForYear")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetTimeZoneInformationForYear, &pfnOrig_GetTimeZoneInformationForYear);
    }

    // Hook GetLocalTime
    if (auto pfn = (pfn_GetLocalTime)GetProcAddress(hKernelBase, "GetLocalTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetLocalTime, &pfnOrig_GetLocalTime);
    }

    // Hook SystemTimeToTzSpecificLocalTime
    if (auto pfn = (pfn_SystemTimeToTzSpecificLocalTime)GetProcAddress(hKernelBase, "SystemTimeToTzSpecificLocalTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_SystemTimeToTzSpecificLocalTime, &pfnOrig_SystemTimeToTzSpecificLocalTime);
    }

    // Hook SystemTimeToTzSpecificLocalTimeEx
    if (auto pfn = (pfn_SystemTimeToTzSpecificLocalTimeEx)GetProcAddress(hKernelBase, "SystemTimeToTzSpecificLocalTimeEx")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_SystemTimeToTzSpecificLocalTimeEx, &pfnOrig_SystemTimeToTzSpecificLocalTimeEx);
    }

    // Hook TzSpecificLocalTimeToSystemTime
    if (auto pfn = (pfn_TzSpecificLocalTimeToSystemTime)GetProcAddress(hKernelBase, "TzSpecificLocalTimeToSystemTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_TzSpecificLocalTimeToSystemTime, &pfnOrig_TzSpecificLocalTimeToSystemTime);
    }

    // Hook TzSpecificLocalTimeToSystemTimeEx
    if (auto pfn = (pfn_TzSpecificLocalTimeToSystemTimeEx)GetProcAddress(hKernelBase, "TzSpecificLocalTimeToSystemTimeEx")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_TzSpecificLocalTimeToSystemTimeEx, &pfnOrig_TzSpecificLocalTimeToSystemTimeEx);
    }

    // Hook FileTimeToLocalFileTime
    if (auto pfn = (pfn_FileTimeToLocalFileTime)GetProcAddress(hKernelBase, "FileTimeToLocalFileTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_FileTimeToLocalFileTime, &pfnOrig_FileTimeToLocalFileTime);
    }

    // Hook LocalFileTimeToFileTime
    if (auto pfn = (pfn_LocalFileTimeToFileTime)GetProcAddress(hKernelBase, "LocalFileTimeToFileTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_LocalFileTimeToFileTime, &pfnOrig_LocalFileTimeToFileTime);
    }

    // Hook GetSystemTime & GetSystemTimeAsFileTime
    if (auto pfn = (pfn_GetSystemTime)GetProcAddress(hKernelBase, "GetSystemTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetSystemTime, &pfnOrig_GetSystemTime);
    }
    if (auto pfn = (pfn_GetSystemTimeAsFileTime)GetProcAddress(hKernelBase, "GetSystemTimeAsFileTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetSystemTimeAsFileTime, &pfnOrig_GetSystemTimeAsFileTime);
    }
    if (auto pfn = (pfn_GetSystemTimePreciseAsFileTime)GetProcAddress(hKernelBase, "GetSystemTimePreciseAsFileTime")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_GetSystemTimePreciseAsFileTime, &pfnOrig_GetSystemTimePreciseAsFileTime);
    }

    // Hook NtQuerySystemInformation
    if (hNtdll) {
        if (auto pfn = (pfn_NtQuerySystemInformation)GetProcAddress(hNtdll, "NtQuerySystemInformation")) {
            WindhawkUtils::SetFunctionHook(pfn, Hook_NtQuerySystemInformation, &pfnOrig_NtQuerySystemInformation);
        }
    }

    // Hook RegQueryValueExW
    if (auto pfn = (pfn_RegQueryValueExW)GetProcAddress(hKernelBase, "RegQueryValueExW")) {
        WindhawkUtils::SetFunctionHook(pfn, Hook_RegQueryValueExW, &pfnOrig_RegQueryValueExW);
    } else if (hAdvapi32) {
        if (auto pfnAdv = (pfn_RegQueryValueExW)GetProcAddress(hAdvapi32, "RegQueryValueExW")) {
            WindhawkUtils::SetFunctionHook(pfnAdv, Hook_RegQueryValueExW, &pfnOrig_RegQueryValueExW);
        }
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
