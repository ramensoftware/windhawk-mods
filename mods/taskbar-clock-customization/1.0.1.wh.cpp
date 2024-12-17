// ==WindhawkMod==
// @id              taskbar-clock-customization
// @name            Taskbar Clock Customization
// @description     Customize the taskbar clock - add seconds, define a custom date/time format, add a news feed, and more
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lwininet
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Clock Customization

Customize the taskbar clock - add seconds, define a custom date/time format, add a news feed, and more.

![Screenshot](https://i.imgur.com/gM9kbH5.png)

Only Windows 10 64-bit and Windows 11 are supported.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ShowSeconds: true
  $name: Show seconds
- TimeFormat: >-
    hh':'mm':'ss tt
  $name: Time format
  $description: >-
    Leave empty for the default format, for syntax see:
    https://docs.microsoft.com/en-us/windows/win32/api/datetimeapi/nf-datetimeapi-gettimeformatex#remarks
- DateFormat: >-
    ddd',' MMM dd yyyy
  $name: Date format
  $description: >-
    Leave empty for the default format, for syntax see:
    https://docs.microsoft.com/en-us/windows/win32/intl/day--month--year--and-era-format-pictures
- WeekdayFormat: dddd
  $name: Week day format
  $description: >-
    Leave empty for the default format, for syntax see:
    https://docs.microsoft.com/en-us/windows/win32/intl/day--month--year--and-era-format-pictures
- TopLine: '%date% | %time%'
  $name: Top line
  $description: >-
    Text to be shown on the first line,
    set to "-" for the default value,
    the following patterns can be used: %time%, %date%, %weekday%, %web%
- BottomLine: '%web%'
  $name: Bottom line
  $description: Only shown if the taskbar is large enough, set to "-" for the default value
- MiddleLine: '%weekday%'
  $name: Middle line (Windows 10 only)
  $description: Only shown if the taskbar is large enough, set to "-" for the default value
- Width: 180
  $name: Clock width (Windows 10 only)
- Height: 60
  $name: Clock height (Windows 10 only)
- WebContentsUrl: https://feeds.bbci.co.uk/news/world/rss.xml
  $name: Web content URL
  $description: Will be used to fetch data displayed in place of the %web% pattern
- WebContentsBlockStart: '<item>'
  $name: Web content block start
  $description: The string in the webpage to start from
- WebContentsStart: '<title><![CDATA['
  $name: Web content start
  $description: The string just before the content
- WebContentsEnd: ']]></title>'
  $name: Web content end
  $description: The string just after the content
- WebContentsMaxLength: 28
  $name: Web content maximum length
  $description: Longer strings will be truncated with ellipsis
- WebContentsUpdateInterval: 10
  $name: Web content update interval
  $description: The update interval, in minutes, of the web content
*/
// ==/WindhawkModSettings==

#include <regex>

#include <wininet.h>

struct {
    bool showSeconds;
    PCWSTR timeFormat;
    PCWSTR dateFormat;
    PCWSTR weekdayFormat;
    PCWSTR topLine;
    PCWSTR bottomLine;
    PCWSTR middleLine;
    int width;
    int height;
    PCWSTR webContentsUrl;
    PCWSTR webContentsBlockStart;
    PCWSTR webContentsStart;
    PCWSTR webContentsEnd;
    int webContentsMaxLength;
    int webContentsUpdateInterval;
} g_settings;

#define FORMATTED_BUFFER_SIZE 256

bool g_isBeforeWin11;
HANDLE g_webContentUpdateThread;
HANDLE g_webContentUpdateStopEvent;
WCHAR g_timeFormatted[FORMATTED_BUFFER_SIZE];
WCHAR g_dateFormatted[FORMATTED_BUFFER_SIZE];
WCHAR g_weekdayFormatted[FORMATTED_BUFFER_SIZE];
WCHAR g_webContent[FORMATTED_BUFFER_SIZE];

typedef UINT (WINAPI *GetDpiForWindow_t)(HWND hwnd);
GetDpiForWindow_t pGetDpiForWindow;

using GetTimeFormatEx_t = decltype(&GetTimeFormatEx);
GetTimeFormatEx_t pOriginalGetTimeFormatEx;

using GetDateFormatEx_t = decltype(&GetDateFormatEx);
GetDateFormatEx_t pOriginalGetDateFormatEx;

LPBYTE GetUrlContent(PCWSTR lpUrl, LPDWORD pdwLength)
{
    HINTERNET hOpenHandle, hUrlHandle;
    LPBYTE pUrlContent;
    DWORD dwLength, dwNumberOfBytesRead;

    hOpenHandle = InternetOpen(L"TaskbarClockCustomization", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (hOpenHandle == nullptr) {
        return nullptr;
    }

    hUrlHandle = InternetOpenUrl(hOpenHandle, lpUrl, nullptr, 0,
        INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES |
        INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0);
    if (hUrlHandle == nullptr) {
        InternetCloseHandle(hOpenHandle);
        return nullptr;
    }

    pUrlContent = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, 0x400);
    if (pUrlContent) {
        InternetReadFile(hUrlHandle, pUrlContent, 0x400, &dwNumberOfBytesRead);
        dwLength = dwNumberOfBytesRead;

        while (dwNumberOfBytesRead) {
            LPBYTE pNewUrlContent = (LPBYTE)HeapReAlloc(GetProcessHeap(), 0, pUrlContent, dwLength + 0x400);
            if (!pNewUrlContent) {
                break;
            }

            pUrlContent = pNewUrlContent;
            InternetReadFile(hUrlHandle, pUrlContent + dwLength, 0x400, &dwNumberOfBytesRead);
            dwLength += dwNumberOfBytesRead;
        }
    }

    InternetCloseHandle(hUrlHandle);
    InternetCloseHandle(hOpenHandle);

    if (pdwLength) {
        *pdwLength = dwLength;
    }

    return pUrlContent;
}

void FreeUrlContent(LPBYTE pUrlContent)
{
    HeapFree(GetProcessHeap(), 0, pUrlContent);
}

int StringCopyTruncated(PWSTR dest, size_t destSize, PCWSTR src, bool* truncated)
{
    if (destSize == 0) {
        *truncated = *src;
        return 0;
    }

    size_t i;
    for (i = 0; i < destSize - 1 && *src; i++) {
        *dest++ = *src++;
    }

    *dest = L'\0';
    *truncated = *src;
    return i;
}

void UpdateWebContent()
{
    DWORD dwLength;
    LPBYTE pUrlContent = GetUrlContent(g_settings.webContentsUrl, &dwLength);
    if (!pUrlContent) {
        return;
    }

    // Assume UTF-8.
    PWSTR unicodeContent = new WCHAR[dwLength + 1];
    DWORD dw = MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent, dwLength, unicodeContent, dwLength * 2);
    unicodeContent[dw] = L'\0';

    FreeUrlContent(pUrlContent);

    PWSTR block = wcsstr(unicodeContent, g_settings.webContentsBlockStart);
    if (block) {
        PWSTR start = wcsstr(block, g_settings.webContentsStart);
        if (start) {
            start += wcslen(g_settings.webContentsStart);

            PWSTR end = wcsstr(block, g_settings.webContentsEnd);
            if (end) {
                *end = L'\0';

                size_t maxLen = ARRAYSIZE(g_webContent) - 1;
                if (g_settings.webContentsMaxLength < maxLen) {
                    maxLen = g_settings.webContentsMaxLength;
                }

                bool truncated;
                StringCopyTruncated(g_webContent, maxLen + 1, start, &truncated);
                if (truncated && maxLen >= 3) {
                    g_webContent[maxLen - 1] = L'.';
                    g_webContent[maxLen - 2] = L'.';
                    g_webContent[maxLen - 3] = L'.';
                }
            }
        }
    }

    delete[] unicodeContent;
}

DWORD WINAPI WebContentUpdateThread(LPVOID lpThreadParameter)
{
    while (true) {
        UpdateWebContent();

        DWORD dwWaitResult = WaitForSingleObject(g_webContentUpdateStopEvent, g_settings.webContentsUpdateInterval * 60 * 1000);
        if (dwWaitResult != WAIT_TIMEOUT) {
            break;
        }
    }

    return 0;
}

void WebContentUpdateThreadInit()
{
    *g_webContent = L'\0';
    if (*g_settings.webContentsUrl) {
        g_webContentUpdateStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        g_webContentUpdateThread = CreateThread(nullptr, 0, WebContentUpdateThread, nullptr, 0, nullptr);
    }
}

void WebContentUpdateThreadUninit()
{
    if (g_webContentUpdateThread) {
        SetEvent(g_webContentUpdateStopEvent);
        WaitForSingleObject(g_webContentUpdateThread, INFINITE);
        CloseHandle(g_webContentUpdateThread);
        g_webContentUpdateThread = nullptr;
        CloseHandle(g_webContentUpdateStopEvent);
        g_webContentUpdateStopEvent = nullptr;
    }
}

void InitializeFormattedStrings(const SYSTEMTIME* time)
{
    pOriginalGetTimeFormatEx(nullptr, g_settings.showSeconds ? 0 : TIME_NOSECONDS, time,
        *g_settings.timeFormat ? g_settings.timeFormat : nullptr, g_timeFormatted, ARRAYSIZE(g_timeFormatted));
    pOriginalGetDateFormatEx(nullptr, DATE_AUTOLAYOUT, time,
        *g_settings.dateFormat ? g_settings.dateFormat : nullptr, g_dateFormatted, ARRAYSIZE(g_dateFormatted), nullptr);
    pOriginalGetDateFormatEx(nullptr, DATE_AUTOLAYOUT, time,
        *g_settings.weekdayFormat ? g_settings.weekdayFormat : nullptr, g_weekdayFormatted, ARRAYSIZE(g_weekdayFormatted), nullptr);
}

int FormatLine(PWSTR buffer, size_t bufferSize, PCWSTR format)
{
    if (bufferSize == 0) {
        return 0;
    }

    PWSTR bufferStart = buffer;
    PWSTR bufferEnd = bufferStart + bufferSize;
    while (*format && bufferEnd - buffer > 1) {
        if (*format == L'%') {
            PCWSTR srcStr = nullptr;
            size_t formatTokenLen;

            if (wcsncmp(L"%time%", format, sizeof("%time%") - 1) == 0) {
                srcStr = g_timeFormatted;
                formatTokenLen = sizeof("%time%") - 1;
            }
            else if (wcsncmp(L"%date%", format, sizeof("%date%") - 1) == 0) {
                srcStr = g_dateFormatted;
                formatTokenLen = sizeof("%date%") - 1;
            }
            else if (wcsncmp(L"%weekday%", format, sizeof("%weekday%") - 1) == 0) {
                srcStr = g_weekdayFormatted;
                formatTokenLen = sizeof("%weekday%") - 1;
            }
            else if (wcsncmp(L"%web%", format, sizeof("%web%") - 1) == 0) {
                srcStr = *g_webContent ? g_webContent : L"Loading...";
                formatTokenLen = sizeof("%web%") - 1;
            }

            if (srcStr) {
                bool truncated;
                buffer += StringCopyTruncated(buffer, bufferEnd - buffer, srcStr, &truncated);
                if (truncated) {
                    break;
                }

                format += formatTokenLen;
                continue;
            }
        }

        *buffer++ = *format++;
    }

    if (*format && bufferSize >= 4) {
        buffer[-1] = L'.';
        buffer[-2] = L'.';
        buffer[-3] = L'.';
    }

    *buffer = L'\0';

    return buffer - bufferStart;
}

#pragma region Win11Hooks

DWORD g_refreshIconThreadId;
DWORD g_ScheduleNextUpdateThreadId;

typedef void (WINAPI *ClockSystemTrayIconDataModelRefreshIcon_t)(
    LPVOID pThis,
    LPVOID // SystemTrayTelemetry::ClockUpdate&
);
typedef void (WINAPI *ClockSystemTrayIconDataModelScheduleNextUpdate_t)(
    LPVOID pThis,
    LPVOID // SystemTrayTelemetry::ClockUpdate&
);
typedef int (WINAPI *ICalendarSecond_t)(
    LPVOID pThis
);

ClockSystemTrayIconDataModelRefreshIcon_t pOriginalClockSystemTrayIconDataModelRefreshIcon;
ClockSystemTrayIconDataModelScheduleNextUpdate_t pOriginalClockSystemTrayIconDataModelScheduleNextUpdate;
ICalendarSecond_t pOriginalICalendarSecond;

void WINAPI ClockSystemTrayIconDataModelRefreshIconHook(
    LPVOID pThis,
    LPVOID param1
)
{
    Wh_Log(L">");

    g_refreshIconThreadId = GetCurrentThreadId();

    pOriginalClockSystemTrayIconDataModelRefreshIcon(
        pThis,
        param1
    );

    g_refreshIconThreadId = 0;
}

void WINAPI ClockSystemTrayIconDataModelScheduleNextUpdateHook(
    LPVOID pThis,
    LPVOID param1
)
{
    Wh_Log(L">");

    g_ScheduleNextUpdateThreadId = GetCurrentThreadId();

    pOriginalClockSystemTrayIconDataModelScheduleNextUpdate(
        pThis,
        param1
    );

    g_ScheduleNextUpdateThreadId = 0;
}

int WINAPI ICalendarSecondHook(
    LPVOID pThis
)
{
    Wh_Log(L">");

    if (g_ScheduleNextUpdateThreadId == GetCurrentThreadId()) {
        if (g_settings.showSeconds || !*g_webContent) {
            // Make the next refresh happen in a second.
            return 59;
        }
    }

    int ret = pOriginalICalendarSecond(
        pThis
    );

    return ret;
}

int WINAPI GetTimeFormatExHook_Win11(
    LPCWSTR lpLocaleName,
    DWORD dwFlags,
    CONST SYSTEMTIME* lpTime,
    LPCWSTR lpFormat,
    LPWSTR lpTimeStr,
    int cchTime)
{
    if (g_refreshIconThreadId == GetCurrentThreadId()) {
        if (wcscmp(g_settings.topLine, L"-") != 0) {
            if (!cchTime) {
                // Hopefully a large enough buffer size.
                return FORMATTED_BUFFER_SIZE;
            }

            return FormatLine(lpTimeStr, cchTime, g_settings.topLine) + 1;
        }
    }

    int ret = pOriginalGetTimeFormatEx(
        lpLocaleName,
        dwFlags,
        lpTime,
        lpFormat,
        lpTimeStr,
        cchTime
    );

    return ret;
}

int WINAPI GetDateFormatExHook_Win11(
    LPCWSTR lpLocaleName,
    DWORD dwFlags,
    CONST SYSTEMTIME* lpDate,
    LPCWSTR lpFormat,
    LPWSTR lpDateStr,
    int cchDate,
    LPCWSTR lpCalendar)
{
    if (g_refreshIconThreadId == GetCurrentThreadId() && (dwFlags & DATE_SHORTDATE)) {
        if (!cchDate) {
            // First call, initialize strings.
            InitializeFormattedStrings(lpDate);
        }

        if (wcscmp(g_settings.bottomLine, L"-") != 0) {
            if (!cchDate) {
                // Hopefully a large enough buffer size.
                return FORMATTED_BUFFER_SIZE;
            }

            return FormatLine(lpDateStr, cchDate, g_settings.bottomLine) + 1;
        }
    }

    int ret = pOriginalGetDateFormatEx(
        lpLocaleName,
        dwFlags,
        lpDate,
        lpFormat,
        lpDateStr,
        cchDate,
        lpCalendar
    );

    return ret;
}

#pragma endregion Win11Hooks

#pragma region Win10Hooks

DWORD g_updateTextStringThreadId;
int g_GetDateFormatExCounter;

typedef unsigned int (WINAPI *ClockButtonUpdateTextStringsIfNecessary_t)(
    LPVOID pThis,
    bool*
);
typedef LPSIZE (WINAPI *ClockButtonCalculateMinimumSize_t)(
    LPVOID pThis,
    LPSIZE,
    SIZE
);
typedef void (WINAPI *ClockButtonv_OnDisplayStateChange_t)(
    LPVOID pThis,
    bool
);

ClockButtonUpdateTextStringsIfNecessary_t pOriginalClockButtonUpdateTextStringsIfNecessary;
ClockButtonCalculateMinimumSize_t pOriginalClockButtonCalculateMinimumSize;
ClockButtonv_OnDisplayStateChange_t pOriginalClockButtonv_OnDisplayStateChange;

unsigned int WINAPI ClockButtonUpdateTextStringsIfNecessaryHook(
    LPVOID pThis,
    bool* param1
)
{
    Wh_Log(L">");

    g_updateTextStringThreadId = GetCurrentThreadId();
    g_GetDateFormatExCounter = 0;

    unsigned int ret = pOriginalClockButtonUpdateTextStringsIfNecessary(
        pThis,
        param1
    );

    g_updateTextStringThreadId = 0;

    if (g_settings.showSeconds || !*g_webContent) {
        // Return the time-out value for the time of the next update.
        SYSTEMTIME time;
        GetLocalTime(&time);
        return 1000 - time.wMilliseconds;
    }

    return ret;
}

LPSIZE WINAPI ClockButtonCalculateMinimumSizeHook(
    LPVOID pThis,
    LPSIZE param1,
    SIZE param2
)
{
    Wh_Log(L">");

    LPSIZE ret = pOriginalClockButtonCalculateMinimumSize(
        pThis,
        param1,
        param2
    );

    HWND hWnd = *((HWND*)pThis + 1);
    UINT windowDpi = pGetDpiForWindow ? pGetDpiForWindow(hWnd) : 0;

    if (g_settings.width > 0) {
        ret->cx = g_settings.width;
        if (windowDpi) {
            ret->cx = MulDiv(ret->cx, windowDpi, 96);
        }
    }

    if (g_settings.height > 0) {
        ret->cy = g_settings.height;
        if (windowDpi) {
            ret->cy = MulDiv(ret->cy, windowDpi, 96);
        }
    }

    return ret;
}

int WINAPI GetTimeFormatExHook_Win10(
    LPCWSTR lpLocaleName,
    DWORD dwFlags,
    CONST SYSTEMTIME* lpTime,
    LPCWSTR lpFormat,
    LPWSTR lpTimeStr,
    int cchTime)
{
    if (g_updateTextStringThreadId == GetCurrentThreadId()) {
        InitializeFormattedStrings(lpTime);

        if (wcscmp(g_settings.topLine, L"-") != 0) {
            return FormatLine(lpTimeStr, cchTime, g_settings.topLine) + 1;
        }
    }

    int ret = pOriginalGetTimeFormatEx(
        lpLocaleName,
        dwFlags,
        lpTime,
        lpFormat,
        lpTimeStr,
        cchTime
    );

    return ret;
}

int WINAPI GetDateFormatExHook_Win10(
    LPCWSTR lpLocaleName,
    DWORD dwFlags,
    CONST SYSTEMTIME* lpDate,
    LPCWSTR lpFormat,
    LPWSTR lpDateStr,
    int cchDate,
    LPCWSTR lpCalendar)
{
    if (g_updateTextStringThreadId == GetCurrentThreadId()) {
        g_GetDateFormatExCounter++;
        PCWSTR format = g_GetDateFormatExCounter > 1 ? g_settings.middleLine : g_settings.bottomLine;
        if (wcscmp(format, L"-") != 0) {
            return FormatLine(lpDateStr, cchDate, format) + 1;
        }
    }

    int ret = pOriginalGetDateFormatEx(
        lpLocaleName,
        dwFlags,
        lpDate,
        lpFormat,
        lpDateStr,
        cchDate,
        lpCalendar
    );

    return ret;
}

#pragma endregion Win10Hooks

void LoadSettings()
{
    g_settings.showSeconds = Wh_GetIntSetting(L"ShowSeconds");
    g_settings.timeFormat = Wh_GetStringSetting(L"TimeFormat");
    g_settings.dateFormat = Wh_GetStringSetting(L"DateFormat");
    g_settings.weekdayFormat = Wh_GetStringSetting(L"WeekdayFormat");
    g_settings.topLine = Wh_GetStringSetting(L"TopLine");
    g_settings.bottomLine = Wh_GetStringSetting(L"BottomLine");
    g_settings.middleLine = Wh_GetStringSetting(L"MiddleLine");
    g_settings.width = Wh_GetIntSetting(L"Width");
    g_settings.height = Wh_GetIntSetting(L"Height");
    g_settings.webContentsUrl = Wh_GetStringSetting(L"WebContentsUrl");
    g_settings.webContentsBlockStart = Wh_GetStringSetting(L"WebContentsBlockStart");
    g_settings.webContentsStart = Wh_GetStringSetting(L"WebContentsStart");
    g_settings.webContentsEnd = Wh_GetStringSetting(L"WebContentsEnd");
    g_settings.webContentsMaxLength = Wh_GetIntSetting(L"WebContentsMaxLength");
    g_settings.webContentsUpdateInterval = Wh_GetIntSetting(L"WebContentsUpdateInterval");
}

void FreeSettings()
{
    Wh_FreeStringSetting(g_settings.timeFormat);
    Wh_FreeStringSetting(g_settings.dateFormat);
    Wh_FreeStringSetting(g_settings.weekdayFormat);
    Wh_FreeStringSetting(g_settings.topLine);
    Wh_FreeStringSetting(g_settings.bottomLine);
    Wh_FreeStringSetting(g_settings.middleLine);
    Wh_FreeStringSetting(g_settings.webContentsUrl);
    Wh_FreeStringSetting(g_settings.webContentsBlockStart);
    Wh_FreeStringSetting(g_settings.webContentsStart);
    Wh_FreeStringSetting(g_settings.webContentsEnd);
}

void ApplySettingsWin11()
{
    DWORD dwProcessId;
    DWORD dwCurrentProcessId = GetCurrentProcessId();

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbarWnd && GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId) &&
        dwProcessId == dwCurrentProcessId) {
        // Trigger a time change notification. Do so only if the current explorer.exe
        // instance owns the taskbar.
        WCHAR szTimeFormat[80];
        if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, szTimeFormat, ARRAYSIZE(szTimeFormat))) {
            SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, szTimeFormat);
        }
    }
}

void ApplySettingsWin10()
{
    DWORD dwProcessId;
    DWORD dwCurrentProcessId = GetCurrentProcessId();

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbarWnd && GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId) &&
        dwProcessId == dwCurrentProcessId) {
        // Apply size.
        RECT rc;
        if (GetClientRect(hTaskbarWnd, &rc)) {
            SendMessage(hTaskbarWnd, WM_SIZE, SIZE_RESTORED,
                MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));
        }

        // Apply text.
        HWND hTrayNotifyWnd = FindWindowEx(hTaskbarWnd, nullptr, L"TrayNotifyWnd", nullptr);
        if (hTrayNotifyWnd) {
            HWND hTrayClockWWnd = FindWindowEx(hTrayNotifyWnd, nullptr, L"TrayClockWClass", nullptr);
            if (hTrayClockWWnd) {
                LONG_PTR lpTrayClockWClassLongPtr = GetWindowLongPtr(hTrayClockWWnd, 0);
                if (lpTrayClockWClassLongPtr) {
                    pOriginalClockButtonv_OnDisplayStateChange((LPVOID)lpTrayClockWClassLongPtr, true);
                }
            }
        }
    }

    HWND hSecondaryTaskbarWnd = FindWindow(L"Shell_SecondaryTrayWnd", nullptr);
    while (hSecondaryTaskbarWnd && GetWindowThreadProcessId(hSecondaryTaskbarWnd, &dwProcessId) &&
        dwProcessId == dwCurrentProcessId) {
        // Apply size.
        RECT rc;
        if (GetClientRect(hSecondaryTaskbarWnd, &rc)) {
            WINDOWPOS windowpos;
            windowpos.hwnd = hSecondaryTaskbarWnd;
            windowpos.hwndInsertAfter = nullptr;
            windowpos.x = 0;
            windowpos.y = 0;
            windowpos.cx = rc.right - rc.left;
            windowpos.cy = rc.bottom - rc.top;
            windowpos.flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE;

            SendMessage(hSecondaryTaskbarWnd, WM_WINDOWPOSCHANGED, 0, (LPARAM)&windowpos);
        }

        // Apply text.
        HWND hClockButtonWnd = FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"ClockButton", nullptr);
        if (hClockButtonWnd) {
            LONG_PTR lpClockButtonLongPtr = GetWindowLongPtr(hClockButtonWnd, 0);
            if (lpClockButtonLongPtr) {
                pOriginalClockButtonv_OnDisplayStateChange((LPVOID)lpClockButtonLongPtr, true);
            }
        }

        hSecondaryTaskbarWnd = FindWindowEx(nullptr, hSecondaryTaskbarWnd, L"Shell_SecondaryTrayWnd", nullptr);
    }
}

void ApplySettings()
{
    if (!g_isBeforeWin11) {
        ApplySettingsWin11();
    }
    else {
        ApplySettingsWin10();
    }
}

struct SYMBOLHOOKS {
    std::wregex symbolRegex;
    void* hookFunction;
    void** pOriginalFunction;
};

BOOL Wh_ModInit(void)
{
    Wh_Log(L">");

    LoadSettings();

    HMODULE hUser32 = LoadLibrary(L"user32.dll");
    if (hUser32) {
        pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(hUser32, "GetDpiForWindow");
    }

    SYMBOLHOOKS taskbarHooks11[] = {
        {
            std::wregex(LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::RefreshIcon\(class SystemTrayTelemetry::ClockUpdate &\s*(__ptr64)?\)\s*(__ptr64)?)"),
            (void*)ClockSystemTrayIconDataModelRefreshIconHook,
            (void**)&pOriginalClockSystemTrayIconDataModelRefreshIcon
        },
        {
            std::wregex(LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::ScheduleNextUpdate\(class SystemTrayTelemetry::ClockUpdate &\s*(__ptr64)?\)\s*(__ptr64)?)"),
            (void*)ClockSystemTrayIconDataModelScheduleNextUpdateHook,
            (void**)&pOriginalClockSystemTrayIconDataModelScheduleNextUpdate
        },
        {
            std::wregex(LR"(public: int __cdecl winrt::impl::consume_Windows_Globalization_ICalendar<struct winrt::Windows::Globalization::ICalendar>::Second\(void\)const\s*(__ptr64)?)"),
            (void*)ICalendarSecondHook,
            (void**)&pOriginalICalendarSecond
        }
    };
    size_t taskbarHooks11Size = ARRAYSIZE(taskbarHooks11);

    SYMBOLHOOKS taskbarHooks10[] = {
        {
            std::wregex(LR"(private: unsigned int __cdecl ClockButton::UpdateTextStringsIfNecessary\(bool \* __ptr64\) __ptr64)"),
            (void*)ClockButtonUpdateTextStringsIfNecessaryHook,
            (void**)&pOriginalClockButtonUpdateTextStringsIfNecessary
        },
        {
            std::wregex(LR"(public: struct tagSIZE __cdecl ClockButton::CalculateMinimumSize\(struct tagSIZE\) __ptr64)"),
            (void*)ClockButtonCalculateMinimumSizeHook,
            (void**)&pOriginalClockButtonCalculateMinimumSize
        },
        {
            std::wregex(LR"(protected: virtual void __cdecl ClockButton::v_OnDisplayStateChange\(bool\) __ptr64)"),
            nullptr,
            (void**)&pOriginalClockButtonv_OnDisplayStateChange
        }
    };
    size_t taskbarHooks10Size = ARRAYSIZE(taskbarHooks10);

    SYMBOLHOOKS* taskbarHooks = taskbarHooks11;
    size_t taskbarHooksSize = taskbarHooks11Size;
    HMODULE module = LoadLibrary(L"explorerextensions.dll");
    if (!module) {
        g_isBeforeWin11 = true;
        taskbarHooks = taskbarHooks10;
        taskbarHooksSize = taskbarHooks10Size;
        module = GetModuleHandle(nullptr);
    }

    WH_FIND_SYMBOL symbol;
    HANDLE find_symbol = Wh_FindFirstSymbol(module, nullptr, &symbol);
    if (find_symbol) {
        do {
            for (size_t i = 0; i < taskbarHooksSize; i++) {
                if (!*taskbarHooks[i].pOriginalFunction && std::regex_match(symbol.symbol, taskbarHooks[i].symbolRegex)) {
                    if (taskbarHooks[i].hookFunction) {
                        Wh_SetFunctionHook(symbol.address, taskbarHooks[i].hookFunction, taskbarHooks[i].pOriginalFunction);
                        Wh_Log(L"Hooked %p (%s)", symbol.address, symbol.symbol);
                    }
                    else {
                        *taskbarHooks[i].pOriginalFunction = symbol.address;
                        Wh_Log(L"Found %p (%s)", symbol.address, symbol.symbol);
                    }
                    break;
                }
            }
        } while (Wh_FindNextSymbol(find_symbol, &symbol));

        Wh_FindCloseSymbol(find_symbol);
    }

    for (size_t i = 0; i < taskbarHooksSize; i++) {
        if (!*taskbarHooks[i].pOriginalFunction) {
            Wh_Log(L"Missing symbol: %d", i);
            return FALSE;
        }
    }

    // Must use GetProcAddress for the functions below, otherwise the stubs
    // in kernel32.dll are being hooked.
    HMODULE hKernelBase = GetModuleHandle(L"kernelbase.dll");
    if (!hKernelBase) {
        return FALSE;
    }

    FARPROC pGetTimeFormatEx = GetProcAddress(hKernelBase, "GetTimeFormatEx");
    if (!pGetTimeFormatEx) {
        return FALSE;
    }

    FARPROC pGetDateFormatEx = GetProcAddress(hKernelBase, "GetDateFormatEx");
    if (!pGetDateFormatEx) {
        return FALSE;
    }

    if (!g_isBeforeWin11) {
        Wh_SetFunctionHook((void*)pGetTimeFormatEx, (void*)GetTimeFormatExHook_Win11, (void**)&pOriginalGetTimeFormatEx);
        Wh_SetFunctionHook((void*)pGetDateFormatEx, (void*)GetDateFormatExHook_Win11, (void**)&pOriginalGetDateFormatEx);
    }
    else {
        Wh_SetFunctionHook((void*)pGetTimeFormatEx, (void*)GetTimeFormatExHook_Win10, (void**)&pOriginalGetTimeFormatEx);
        Wh_SetFunctionHook((void*)pGetDateFormatEx, (void*)GetDateFormatExHook_Win10, (void**)&pOriginalGetDateFormatEx);
    }

    WebContentUpdateThreadInit();

    return TRUE;
}

void Wh_ModAfterInit(void)
{
    Wh_Log(L">");

    ApplySettings();
}

void Wh_ModUninit(void)
{
    Wh_Log(L">");

    WebContentUpdateThreadUninit();

    FreeSettings();
    ApplySettings();
}

void Wh_ModSettingsChanged(void)
{
    Wh_Log(L">");

    WebContentUpdateThreadUninit();

    FreeSettings();
    LoadSettings();

    WebContentUpdateThreadInit();

    ApplySettings();
}
