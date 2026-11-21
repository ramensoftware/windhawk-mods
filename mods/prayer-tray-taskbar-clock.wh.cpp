// ==WindhawkMod==
// @id              prayer-tray-taskbar-clock
// @name            Prayer Tray: Next Prayer in the Taskbar Clock
// @description     Shows the next prayer time next to the Windows 11 taskbar clock, using the times saved by the Prayer Tray app
// @version         1.0
// @author          aitoolstohelp
// @github          https://github.com/qamarkadii
// @homepage        https://aitoolstohelp.github.io/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Prayer Tray: Next Prayer in the Taskbar Clock

Adds the next prayer to the Windows 11 taskbar clock, for example
`21/11/2026 · Asr 3:08`. It switches to the next prayer on its own, and after
Isha it shows tomorrow's Fajr.

The times come from [Prayer Tray](https://aitoolstohelp.github.io/), a free
tray app with UAE prayer times from Awqaf. **Prayer Tray must be installed**:
this mod only reads the times Prayer Tray saves on your PC
(`%LOCALAPPDATA%\PrayerTray\data.json`). The mod itself never goes online.

Windows 11 only.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- line: date
  $name: Show it next to
  $options:
  - date: The date
  - time: The time
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <mutex>
#include <string>

bool g_onTimeLine;
std::atomic<bool> g_moduleHooked;
thread_local bool g_inRefreshIcon;
thread_local bool g_inToolTip;

// ---------------------------------------------------------------- prayer times from Prayer Tray's data.json

std::mutex g_dataMutex;
std::wstring g_dataPath;
std::string g_json;
FILETIME g_jsonTime;

// Re-reads data.json only when it changed. Keeps the last copy while Prayer
// Tray is replacing the file.
void LoadJsonIfChanged() {
    WIN32_FILE_ATTRIBUTE_DATA attr;
    if (g_dataPath.empty() ||
        !GetFileAttributesExW(g_dataPath.c_str(), GetFileExInfoStandard, &attr) ||
        (!g_json.empty() && CompareFileTime(&attr.ftLastWriteTime, &g_jsonTime) == 0)) {
        return;
    }
    HANDLE file = CreateFileW(g_dataPath.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }
    DWORD size = GetFileSize(file, nullptr), read = 0;
    if (size != INVALID_FILE_SIZE && size < 8 * 1024 * 1024) {
        std::string buffer(size, '\0');
        if (ReadFile(file, buffer.data(), size, &read, nullptr)) {
            buffer.resize(read);
            g_json = std::move(buffer);
            g_jsonTime = attr.ftLastWriteTime;
        }
    }
    CloseHandle(file);
}

// Minutes after midnight of prayer `key` on `day` ("yyyy-mm-dd"), or -1.
// data.json looks like {"Days":{"2026-11-21":{"fajr":"05:19",...},...},...}
int PrayerMinutes(const char* day, const char* key) {
    size_t start = g_json.find(std::string("\"") + day + "\":{");
    if (start == std::string::npos) {
        return -1;
    }
    size_t end = g_json.find('}', start);
    std::string tag = std::string("\"") + key + "\":\"";
    size_t pos = g_json.find(tag, start);
    if (pos == std::string::npos || pos > end) {
        return -1;
    }
    int hour, minute;
    if (sscanf_s(g_json.c_str() + pos + tag.size(), "%d:%d", &hour, &minute) != 2) {
        return -1;
    }
    return hour * 60 + minute;
}

// "Asr 3:08", or empty when Prayer Tray has no times for today.
std::wstring NextPrayerText() {
    static const char* keys[] = {"fajr", "dhuhr", "asr", "maghrib", "isha"};
    static const wchar_t* names[] = {L"Fajr", L"Dhuhr", L"Asr", L"Maghrib", L"Isha"};

    SYSTEMTIME now;
    GetLocalTime(&now);
    char today[16];
    sprintf_s(today, "%04u-%02u-%02u", now.wYear, now.wMonth, now.wDay);
    int nowMinutes = now.wHour * 60 + now.wMinute;

    std::lock_guard<std::mutex> lock(g_dataMutex);
    LoadJsonIfChanged();

    const wchar_t* name = nullptr;
    int minutes = -1;
    for (int i = 0; i < 5 && !name; i++) {
        int m = PrayerMinutes(today, keys[i]);
        if (m > nowMinutes) {
            name = names[i];
            minutes = m;
        }
    }
    if (!name) {
        // after Isha: tomorrow's Fajr
        FILETIME ft;
        SystemTimeToFileTime(&now, &ft);
        ULARGE_INTEGER u{{ft.dwLowDateTime, ft.dwHighDateTime}};
        u.QuadPart += 24ULL * 60 * 60 * 10000000;
        ft = {u.LowPart, u.HighPart};
        SYSTEMTIME tomorrow;
        FileTimeToSystemTime(&ft, &tomorrow);
        char day[16];
        sprintf_s(day, "%04u-%02u-%02u", tomorrow.wYear, tomorrow.wMonth, tomorrow.wDay);
        minutes = PrayerMinutes(day, "fajr");
        name = L"Fajr";
    }
    if (minutes < 0) {
        return L"";
    }
    int hour12 = (minutes / 60) % 12;
    WCHAR text[64];
    swprintf_s(text, L" \x00B7 %s %d:%02d", name, hour12 ? hour12 : 12, minutes % 60);
    return text;
}

// Appends the prayer text to a clock line that the original function wrote.
// `ret` is the original result (characters including the terminator, or the
// needed size when `cch` is 0).
int AppendPrayer(LPWSTR out, int cch, int ret) {
    if (ret <= 0) {
        return ret;
    }
    std::wstring extra = NextPrayerText();
    int extraLen = (int)extra.size();
    if (!extraLen) {
        return ret;
    }
    if (cch == 0) {
        return ret + extraLen;   // size query
    }
    if (ret + extraLen > cch) {
        return ret;              // no room: leave the clock as it was
    }
    wcscpy_s(out + ret - 1, cch - (ret - 1), extra.c_str());
    return ret + extraLen;
}

// ---------------------------------------------------------------- hooks

using GetDateFormatEx_t = decltype(GetDateFormatEx);
GetDateFormatEx_t* GetDateFormatEx_Original;
int WINAPI GetDateFormatEx_Hook(LPCWSTR locale, DWORD flags, CONST SYSTEMTIME* date, LPCWSTR format,
                                LPWSTR out, int cch, LPCWSTR calendar) {
    int ret = GetDateFormatEx_Original(locale, flags, date, format, out, cch, calendar);
    if (g_inRefreshIcon && !g_inToolTip && !g_onTimeLine && !(flags & DATE_LONGDATE)) {
        return AppendPrayer(out, cch, ret);
    }
    return ret;
}

using GetTimeFormatEx_t = decltype(GetTimeFormatEx);
GetTimeFormatEx_t* GetTimeFormatEx_Original;
int WINAPI GetTimeFormatEx_Hook(LPCWSTR locale, DWORD flags, CONST SYSTEMTIME* time, LPCWSTR format,
                                LPWSTR out, int cch) {
    int ret = GetTimeFormatEx_Original(locale, flags, time, format, out, cch);
    if (g_inRefreshIcon && !g_inToolTip && g_onTimeLine) {
        return AppendPrayer(out, cch, ret);
    }
    return ret;
}

// The clock formats its two lines inside RefreshIcon (about once a minute).
using RefreshIcon_t = void WINAPI(void* pThis, void* clockUpdate);
RefreshIcon_t* RefreshIcon_Original;
RefreshIcon_t* RefreshIcon2_Original;
void WINAPI RefreshIcon_Hook(void* pThis, void* clockUpdate) {
    g_inRefreshIcon = true;
    RefreshIcon_Original(pThis, clockUpdate);
    g_inRefreshIcon = false;
}
void WINAPI RefreshIcon2_Hook(void* pThis, void* clockUpdate) {
    g_inRefreshIcon = true;
    RefreshIcon2_Original(pThis, clockUpdate);
    g_inRefreshIcon = false;
}

// The hover tooltip is also built inside RefreshIcon; leave it untouched.
// (Returns an hstring, so on x64 the hidden return pointer is the 2nd argument.)
using ToolTip_t = void* WINAPI(void* pThis, void* ret, void* a, void* b, void* c);
ToolTip_t* ToolTip_Original;
ToolTip_t* ToolTip2_Original;
void* WINAPI ToolTip_Hook(void* pThis, void* ret, void* a, void* b, void* c) {
    g_inToolTip = true;
    void* result = ToolTip_Original(pThis, ret, a, b, c);
    g_inToolTip = false;
    return result;
}
void* WINAPI ToolTip2_Hook(void* pThis, void* ret, void* a, void* b, void* c) {
    g_inToolTip = true;
    void* result = ToolTip2_Original(pThis, ret, a, b, c);
    g_inToolTip = false;
    return result;
}

WORD ModuleMajorVersion(HMODULE module) {
    WCHAR path[MAX_PATH];
    if (!GetModuleFileNameW(module, path, MAX_PATH)) {
        return 0;
    }
    DWORD size = GetFileVersionInfoSizeW(path, nullptr);
    std::string data(size, '\0');
    VS_FIXEDFILEINFO* info;
    UINT len;
    if (!size || !GetFileVersionInfoW(path, 0, size, data.data()) ||
        !VerQueryValueW(data.data(), L"\\", (void**)&info, &len)) {
        return 0;
    }
    return HIWORD(info->dwFileVersionMS);
}

// Where the Windows 11 clock code lives, depending on the Windows build.
HMODULE GetSystemTrayModule() {
    if (HMODULE module = GetModuleHandleW(L"SystemTray.dll")) {
        return module;
    }
    if (HMODULE module = GetModuleHandleW(L"Taskbar.View.dll")) {
        // From Taskbar.View.dll 2604 on, the clock moved to SystemTray.dll.
        WORD major = ModuleMajorVersion(module);
        if (major && major < 2604) {
            return module;
        }
    }
    return GetModuleHandleW(L"ExplorerExtensions.dll");
}

bool HookSystemTray(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
            &RefreshIcon_Original,
            RefreshIcon_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
            &RefreshIcon2_Original,
            RefreshIcon2_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ToolTip_Original,
            ToolTip_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::GetTimeToolTipString(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ToolTip2_Original,
            ToolTip2_Hook,
            true,
        },
    };
    return WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

void TryHookSystemTray(HMODULE module, bool apply) {
    if (module && module == GetSystemTrayModule() && !g_moduleHooked.exchange(true)) {
        if (HookSystemTray(module)) {
            if (apply) {
                Wh_ApplyHookOperations();
            }
        } else {
            Wh_Log(L"Couldn't find the clock functions");
        }
    }
}

// The clock module can load after the mod; hook it when it does.
using LoadLibraryExW_t = decltype(LoadLibraryExW);
LoadLibraryExW_t* LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module && !g_moduleHooked) {
        TryHookSystemTray(module, true);
    }
    return module;
}

// Makes the clock redraw now (same trick as the Taskbar Clock Customization mod).
void RefreshClock() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    if (!taskbar || !GetWindowThreadProcessId(taskbar, &pid) || pid != GetCurrentProcessId()) {
        return;
    }
    HKEY key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\TimeDate\\AdditionalClocks", 0, KEY_WRITE, &key) == ERROR_SUCCESS) {
        constexpr WCHAR kTemp[] = L"_temp_windhawk_prayer-tray-taskbar-clock";
        if (RegSetValueExW(key, kTemp, 0, REG_SZ, (const BYTE*)L"", sizeof(WCHAR)) == ERROR_SUCCESS) {
            RegDeleteValueW(key, kTemp);
        }
        RegCloseKey(key);
    }
}

void LoadSettings() {
    g_onTimeLine = wcscmp(WindhawkUtils::StringSetting::make(L"line"), L"time") == 0;
}

// ---------------------------------------------------------------- Windhawk entry points

BOOL Wh_ModInit() {
    LoadSettings();

    WCHAR path[MAX_PATH];
    if (ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\PrayerTray\\data.json", path, MAX_PATH)) {
        g_dataPath = path;
    }

    TryHookSystemTray(GetSystemTrayModule(), false);

    // Use kernelbase directly, otherwise the kernel32 stubs get hooked.
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    WindhawkUtils::SetFunctionHook((GetDateFormatEx_t*)GetProcAddress(kernelBase, "GetDateFormatEx"),
                                   GetDateFormatEx_Hook, &GetDateFormatEx_Original);
    WindhawkUtils::SetFunctionHook((GetTimeFormatEx_t*)GetProcAddress(kernelBase, "GetTimeFormatEx"),
                                   GetTimeFormatEx_Hook, &GetTimeFormatEx_Original);
    WindhawkUtils::SetFunctionHook((LoadLibraryExW_t*)GetProcAddress(kernelBase, "LoadLibraryExW"),
                                   LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_moduleHooked) {
        TryHookSystemTray(GetSystemTrayModule(), true);
    }
    RefreshClock();
}

void Wh_ModUninit() {
    RefreshClock();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    LoadSettings();
    RefreshClock();
    return TRUE;
}
