// ==WindhawkMod==
// @id              auto-theme-switcher
// @name            Auto Theme Switcher
// @description     Automatically changes between light and dark mode based on a set schedule
// @version         1.0
// @author          tinodin
// @github          https://github.com/tinodin
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- Light: 07:00
- Dark: 19:00
*/
// ==/WindhawkModSettings==

#include <sec_api/wchar_s.h>
#include <windows.h>
#include <ctime>

HANDLE g_timerThread = nullptr;
HANDLE g_wakeEvent = nullptr;
bool g_exitThread = false;
SYSTEMTIME g_lightTime, g_darkTime;

void SetTheme(bool light) {
    DWORD value = light ? 1 : 0;
    Wh_Log(L"[Theme] Switching to %s mode", light ? L"Light" : L"Dark");

    RegSetKeyValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", REG_DWORD, &value, sizeof(DWORD));
    RegSetKeyValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme", REG_DWORD, &value, sizeof(DWORD));

    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
        (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 100, nullptr);
}

SYSTEMTIME ParseTime(const wchar_t* timeStr) {
    SYSTEMTIME st = {};
    swscanf_s(timeStr, L"%hu:%hu", &st.wHour, &st.wMinute);
    return st;
}

time_t GetNextSwitchTime(const SYSTEMTIME& light, const SYSTEMTIME& dark, bool& nextIsLight) {
    time_t now = time(nullptr);
    struct tm localTime;
    localtime_s(&localTime, &now);

    auto MakeTodayTime = [&](const SYSTEMTIME& st) {
        struct tm t = localTime;
        t.tm_hour = st.wHour;
        t.tm_min = st.wMinute;
        t.tm_sec = 0;
        return mktime(&t);
    };

    time_t lightTime = MakeTodayTime(light);
    time_t darkTime = MakeTodayTime(dark);

    if (now < lightTime) {
        nextIsLight = true;
        return lightTime;
    } else if (now < darkTime) {
        nextIsLight = false;
        return darkTime;
    } else {
        nextIsLight = true;
        lightTime += 86400;
        return lightTime;
    }
}

bool IsLightTime(const SYSTEMTIME& light, const SYSTEMTIME& dark) {
    time_t now = time(nullptr);
    struct tm t;
    localtime_s(&t, &now);

    struct tm lt = t;
    lt.tm_hour = light.wHour;
    lt.tm_min = light.wMinute;
    lt.tm_sec = 0;
    time_t lightTime = mktime(&lt);

    lt.tm_hour = dark.wHour;
    lt.tm_min = dark.wMinute;
    time_t darkTime = mktime(&lt);

    if (now < lightTime)
        return false;
    if (now < darkTime)
        return true;
    return false;
}

DWORD WINAPI TimerLoop(LPVOID) {
    while (!g_exitThread) {
        bool nextIsLight;
        time_t now = time(nullptr);
        time_t target = GetNextSwitchTime(g_lightTime, g_darkTime, nextIsLight);
        int sleepSec = (int)(target - now);

        Wh_Log(L"[Scheduler] Next switch to %s in %d seconds", nextIsLight ? L"Light" : L"Dark", sleepSec);

        DWORD result = WaitForSingleObject(g_wakeEvent, sleepSec * 1000);
        if (g_exitThread)
            break;

        if (result == WAIT_OBJECT_0) {
            Wh_Log(L"[Scheduler] Woken up early for reschedule");
            continue;
        }

        SetTheme(nextIsLight);
    }
    return 0;
}

void StartOrRestartThread() {
    if (g_timerThread) {
        SetEvent(g_wakeEvent);
        return;
    }

    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_timerThread = CreateThread(nullptr, 0, TimerLoop, nullptr, 0, nullptr);
}

void ApplyCurrentThemeAndSchedule() {
    bool useLight = IsLightTime(g_lightTime, g_darkTime);
    SetTheme(useLight);
    StartOrRestartThread();
}

void LoadSettingsAndApply() {
    const wchar_t* lightStr = Wh_GetStringSetting(L"Light");
    const wchar_t* darkStr = Wh_GetStringSetting(L"Dark");
    Wh_Log(L"[Settings] Light=%s, Dark=%s", lightStr, darkStr);

    g_lightTime = ParseTime(lightStr);
    g_darkTime = ParseTime(darkStr);

    ApplyCurrentThemeAndSchedule();
}

BOOL Wh_ModInit() {
    LoadSettingsAndApply();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettingsAndApply();
}

void Wh_ModUninit() {
    g_exitThread = true;
    if (g_wakeEvent)
        SetEvent(g_wakeEvent);
    if (g_timerThread) {
        WaitForSingleObject(g_timerThread, INFINITE);
        CloseHandle(g_timerThread);
    }
    if (g_wakeEvent)
        CloseHandle(g_wakeEvent);
}