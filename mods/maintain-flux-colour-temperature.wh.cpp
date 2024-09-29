// ==WindhawkMod==
// @id              maintain-flux-colour-temperature
// @name            Maintain colour temperature of f.lux
// @description     Keeps your preferred f.lux colour temperature settings, eliminating automatic changes
// @version         1.0
// @author          Roland Pihlakas
// @github          https://github.com/levitation
// @homepage        https://www.simplify.ee/
// @compilerOptions -lkernel32
// @include         flux.exe
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/levitation-opensource/my-windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/levitation-opensource/my-windhawk-mods/

// ==WindhawkModReadme==
/*
# Maintain colour temperature of f.lux

Keeps your preferred f.lux colour temperature settings, eliminating automatic changes. You can choose the desired colour temperature from f.lux dropdown menu, and f.lux will not change it later automatically.

The mod operates by providing f.lux always with a fixed day and time. This fixed day and time can be adjusted by the user in the settings panel of this mod, if necessary, though the default setting should be usually suitable. By default the noon of approximately midsummer day of Northern Hemisphere is chosen as the fixed day and time. The particular day and time are not very important, what matters is that the fixed time is not near the time points where f.lux would usually get triggered to adjust the screen colour on its own.

There are many posts in f.lux forum asking for this functionality since 2016. Unfortunately there were no solutions provided so far:
* [Disable f.lux's automatic adjustments based on time/location](https://forum.justgetflux.com/topic/3012/disable-f-lux-s-automatic-adjustments-based-on-time-location/)
* [Can automatic timer settings be disabled?](https://forum.justgetflux.com/topic/6162/can-automatic-timer-settings-be-disabled)
* [Disabling all automatic functions.](https://forum.justgetflux.com/topic/6228/disabling-all-automatic-functions)
* [How To Disable Automatic Change Based On Time / Location ?](https://forum.justgetflux.com/topic/7904/how-to-disable-automatic-change-based-on-time-location)
* [How do I stop flux to stop changing colors arbitrary / Force always on mode / Force coupling day and night colors?](https://forum.justgetflux.com/topic/8404/how-do-i-stop-flux-to-stop-changing-colors-arbitrary-force-always-on-mode-force-coupling-day-and-night-colors)
* [Sleeping during the day](https://forum.justgetflux.com/topic/8431/sleeping-during-the-day)
* [How do I make my own Permanent preset?](https://forum.justgetflux.com/topic/8475/how-do-i-make-my-own-permanent-preset)
* [Disable Circadian Response Only](https://forum.justgetflux.com/topic/8502/disable-circadian-response-only/)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Hour: 12
  $name: Hour in local time
- Day: 21
  $name: Day
- Month: 6
  $name: Month
*/
// ==/WindhawkModSettings==


#include <windowsx.h>


template <typename T>
BOOL Wh_SetFunctionHookT(
    FARPROC targetFunction,
    T hookFunction,
    T* originalFunction
) {
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction, (void**)originalFunction);
}


int g_hour;
int g_day;
int g_month;


typedef void(WINAPI* GetSystemTimeAsFileTime_t)(LPFILETIME);
GetSystemTimeAsFileTime_t pOriginalGetSystemTimeAsFileTime;
typedef void(WINAPI* GetSystemTime_t)(LPSYSTEMTIME);
GetSystemTime_t pOriginalGetSystemTime;
typedef void(WINAPI* GetLocalTime_t)(LPSYSTEMTIME);
GetLocalTime_t pOriginalGetLocalTime;


/*
https://learn.microsoft.com/en-gb/windows/win32/api/minwinbase/ns-minwinbase-systemtime?redirectedfrom=MSDN
It is not recommended that you add and subtract values from the SYSTEMTIME structure to obtain relative times. Instead, you should:
Convert the SYSTEMTIME structure to a FILETIME structure.
Copy the resulting FILETIME structure to a ULARGE_INTEGER structure.
Use normal 64-bit arithmetic on the ULARGE_INTEGER value.
*/
//code adapted from https://stackoverflow.com/questions/8699069/difference-between-two-systemtime-variable
__int64 Subtract(bool* error, const SYSTEMTIME& pSl, const SYSTEMTIME& pSr) {

    SYSTEMTIME t_res;
    FILETIME v_ftime;
    ULARGE_INTEGER v_ui;
    __int64 v_left, v_right, v_res;

    if (SystemTimeToFileTime(&pSl, &v_ftime)) {
        v_ui.LowPart = v_ftime.dwLowDateTime;
        v_ui.HighPart = v_ftime.dwHighDateTime;
        v_left = v_ui.QuadPart;

        if (SystemTimeToFileTime(&pSr, &v_ftime)) {
            v_ui.LowPart = v_ftime.dwLowDateTime;
            v_ui.HighPart = v_ftime.dwHighDateTime;
            v_right = v_ui.QuadPart;

            v_res = v_left - v_right;
            return v_res;
        }
        else {
            *error = true;
            Wh_Log(L"Error: SystemTimeToFileTime failed on pSl");
        }
    }
    else {
        *error = true;
        Wh_Log(L"Error: SystemTimeToFileTime failed on pSr");
    }

    return 0;
}

SYSTEMTIME Subtract(bool* error, const SYSTEMTIME& pSl, const __int64& pSr) {

    SYSTEMTIME t_res;
    FILETIME v_ftime;
    ULARGE_INTEGER v_ui;
    __int64 v_left, v_res;

    if (SystemTimeToFileTime(&pSl, &v_ftime)) {
        v_ui.LowPart = v_ftime.dwLowDateTime;
        v_ui.HighPart = v_ftime.dwHighDateTime;
        v_left = v_ui.QuadPart;

        v_res = v_left - pSr;

        v_ui.QuadPart = v_res;
        v_ftime.dwLowDateTime = v_ui.LowPart;
        v_ftime.dwHighDateTime = v_ui.HighPart;
        if (FileTimeToSystemTime(&v_ftime, &t_res)) {
            return t_res;
        }
        else {
            *error = true;
            Wh_Log(L"Error: FileTimeToSystemTime failed");
        }
    }
    else {
        *error = true;
        Wh_Log(L"Error: SystemTimeToFileTime failed");
    }

    return pSl;
}

FILETIME Subtract(const FILETIME& pSl, const __int64& pSr) {

    FILETIME v_ftime;
    ULARGE_INTEGER v_ui;
    __int64 v_left, v_res;

    v_ui.LowPart = pSl.dwLowDateTime;
    v_ui.HighPart = pSl.dwHighDateTime;
    v_left = v_ui.QuadPart;

    v_res = v_left - pSr;

    v_ui.QuadPart = v_res;
    v_ftime.dwLowDateTime = v_ui.LowPart;
    v_ftime.dwHighDateTime = v_ui.HighPart;
    return v_ftime;
}

void FillLocalTime(SYSTEMTIME* systemTime, const SYSTEMTIME& actualSystemTime) {

    systemTime->wMilliseconds = 0;
    systemTime->wSecond = 0;
    systemTime->wMinute = 0;
    systemTime->wHour = g_hour;   //assume local timezone
    systemTime->wDay = g_day;
    systemTime->wMonth = g_month;
    systemTime->wYear = actualSystemTime.wYear;     //Use the current year. This has two benefits. First, the daylight saving laws may change across years, so this code ensures that current daylight saving law is considered. Secondly, the user does not need to enter one more number for the year, which would not be essential for specifying the f.lux time.
}

void WINAPI GetSystemTimeAsFileTimeHook(OUT LPFILETIME lpSystemTimeAsFileTime) {

    if (lpSystemTimeAsFileTime) {       //let the original function handle invalid arguments

        Wh_Log(L"GetSystemTimeAsFileTime");

        SYSTEMTIME actualSystemTime;
        pOriginalGetSystemTime(&actualSystemTime);

        SYSTEMTIME actualLocalTime;
        pOriginalGetLocalTime(&actualLocalTime);

        bool error = false;
        __int64 offset = Subtract(&error, actualLocalTime, actualSystemTime);
        Wh_Log(L"offset: %lli", (long long)offset);

        if (!error) {

            SYSTEMTIME localTime;
            FillLocalTime(&localTime, actualLocalTime);
            //The wDayOfWeek member of the SYSTEMTIME structure is ignored in SystemTimeToFileTime() function - https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-systemtimetofiletime

            FILETIME localTimeAsFileTime;
            if (SystemTimeToFileTime(
                &localTime,
                &localTimeAsFileTime
            )) {
                //NB! write only once to the target variable in order to not cause any side effects by changing it twice
                *lpSystemTimeAsFileTime = Subtract(localTimeAsFileTime, offset);     //local time to system time
                return;
            }
            else {
                Wh_Log(L"Error: SystemTimeToFileTime failed");
            }
        }
    }
    
    pOriginalGetSystemTimeAsFileTime(lpSystemTimeAsFileTime);    
}

void WINAPI GetSystemTimeHook(OUT LPSYSTEMTIME lpSystemTime) {

    if (lpSystemTime) {       //let the original function handle invalid arguments

        Wh_Log(L"GetSystemTime");

        SYSTEMTIME actualSystemTime;
        pOriginalGetSystemTime(&actualSystemTime);

        SYSTEMTIME actualLocalTime;
        pOriginalGetLocalTime(&actualLocalTime);

        bool error = false;
        __int64 offset = Subtract(&error, actualLocalTime, actualSystemTime);
        Wh_Log(L"offset: %lli", (long long)offset);

        if (!error) {

            SYSTEMTIME localTime;
            FillLocalTime(&localTime, actualLocalTime);

            //calculate wDayOfWeek - https://stackoverflow.com/questions/3017745/given-date-get-day-of-week-systemtime
            //The wDayOfWeek member of the SYSTEMTIME structure is ignored in SystemTimeToFileTime() function - https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-systemtimetofiletime
            //the offset subtraction in Subtract() will also compute the wDayOfWeek as a side effect

            //NB! write only once to the target variable in order to not cause any side effects by changing it twice
            *lpSystemTime = Subtract(&error, localTime, offset);     //local time to system time
            if (!error)
                return;
        }
    }

    pOriginalGetSystemTime(lpSystemTime);
}

void WINAPI GetLocalTimeHook(OUT LPSYSTEMTIME lpLocalTime) {

    if (lpLocalTime) {       //let the original function handle invalid arguments

        Wh_Log(L"GetLocalTime");

        SYSTEMTIME actualLocalTime;
        pOriginalGetLocalTime(&actualLocalTime);

        SYSTEMTIME localTime;
        FillLocalTime(&localTime, actualLocalTime);

        //calculate wDayOfWeek - https://stackoverflow.com/questions/3017745/given-date-get-day-of-week-systemtime
        //The wDayOfWeek member of the SYSTEMTIME structure is ignored in SystemTimeToFileTime() function - https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-systemtimetofiletime
        
        FILETIME localTimeAsFileTime;
        if (SystemTimeToFileTime(
            &localTime,
            &localTimeAsFileTime
        )) {
            if (FileTimeToSystemTime(
                &localTimeAsFileTime,
                lpLocalTime     //NB! write only once to the target variable in order to not cause any side effects by changing it twice
            )) {
                return;
            }
            else {
                Wh_Log(L"Error: FileTimeToSystemTime failed");
            }
        }
        else {
            Wh_Log(L"Error: SystemTimeToFileTime failed");
        }
    }
    
    pOriginalGetLocalTime(lpLocalTime);
}

void LoadSettings() {

    g_hour = Wh_GetIntSetting(L"Hour");
    g_day = Wh_GetIntSetting(L"Day");
    g_month = Wh_GetIntSetting(L"Month");
}

BOOL Wh_ModInit() {

    LoadSettings();     //need to load settings first, else there would be no logging config in case settings contain logging config

    Wh_Log(L"Init");

    HMODULE hKernel32 = GetModuleHandle(L"kernel32.dll");
    if (!hKernel32) {
        Wh_Log(L"Loading kernel32.dll failed");
        return FALSE;
    }

    FARPROC pGetSystemTimeAsFileTime = GetProcAddress(hKernel32, "GetSystemTimeAsFileTime");
    FARPROC pGetSystemTime = GetProcAddress(hKernel32, "GetSystemTime");
    FARPROC pGetLocalTime = GetProcAddress(hKernel32, "GetLocalTime");
    if (
        !pGetSystemTimeAsFileTime
        || !pGetSystemTime
        || !pGetLocalTime
    ) {
        Wh_Log(L"Finding hookable functions from kernel32.dll failed");
        return FALSE;
    }

    Wh_SetFunctionHookT(pGetSystemTimeAsFileTime, GetSystemTimeAsFileTimeHook, &pOriginalGetSystemTimeAsFileTime);
    Wh_SetFunctionHookT(pGetSystemTime, GetSystemTimeHook, &pOriginalGetSystemTime);
    Wh_SetFunctionHookT(pGetLocalTime, GetLocalTimeHook, &pOriginalGetLocalTime);

    return TRUE;
}

void Wh_ModSettingsChanged() {

    Wh_Log(L"SettingsChanged");

    LoadSettings();
}

void Wh_ModUninit() {

    Wh_Log(L"Uninit");
}
