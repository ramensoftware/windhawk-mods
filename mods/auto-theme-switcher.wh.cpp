// ==WindhawkMod==
// @id              auto-theme-switcher
// @name            Auto Theme Switcher
// @description     Automatically switch between light and dark appearance/wallpapers/themes based on a custom hours/sunset to sunrise with custom script support
// @version         1.1.1
// @author          tinodin
// @github          https://github.com/tinodin
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lwindowsapp -lruntimeobject -lkernel32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Auto Theme Switcher

### Scheduling Modes  
*(default: Set custom hours)*

- Set custom hours for light and dark  
    - *(default: `07:00` for light, `19:00` for dark)*  
- Sunset to sunrise (location service)
- Sunset to sunrise (custom geographic coordinates)
  - *(default: `0` for Latitude, `0` for Longitude)*  
  - Coordinates can be found at [latlong.net](https://www.latlong.net/)

### Switching Modes  
*(default: Switch between light and dark appearance)*

- Switch between light and dark appearance  
- Switch between light and dark appearance + wallpapers  
  - Provide wallpaper paths 
    - *(default: `img0.jpg` for light, `img19.jpg` for dark)*  
- Switch between light and dark themes  
  - Provide paths to `.theme` files 
    - *(default: `aero.theme` for light, `dark.theme` for dark)*  
    - Windows built-in themes: `C:\Windows\Resources\Themes`  
    - Saved/imported themes: `%LOCALAPPDATA%\Microsoft\Windows\Themes`  
    - To create custom `.theme` files, use Windows Personalization settings ([video guide](https://www.youtube.com/watch?v=-QWR6NQZAUg))

### Custom Script
*(default: None)*

- Provide path to a script/executable that runs on every switch with `-light` or `-dark` argument

### Lock Screen
*(default: Enabled)*

- Apply Wallpaper to the Lock Screen
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ScheduleMode: CustomHours
  $name: Scheduling Mode
  $options:
  - CustomHours: Set custom hours
  - LocationService: Sunset to sunrise (location service)
  - CustomCoordinates: Sunset to sunrise (custom geographic coordinates)
- CustomLight: 07:00
  $name: Light Mode Time
  $description: Set custom hours
- CustomDark: 19:00
  $name: Dark Mode Time
  $description: Set custom hours
- Latitude: "0"
  $name: Latitude
  $description: Sunset to sunrise (custom geographic coordinates)
- Longitude: "0"
  $name: Longitude
  $description: Sunset to sunrise (custom geographic coordinates)
- switchMode: Appearance
  $name: Switching Mode
  $options:
  - Appearance: Switch between light and dark appearance
  - Wallpaper: Switch between light and dark appearance + wallpapers
  - Theme: Switch between light and dark themes
- LightWallpaperPath: "C:\\Windows\\Web\\Wallpaper\\Windows\\img0.jpg"
  $name: Light Mode Wallpaper Path
  $description: Switch between light and dark appearance + wallpapers
- DarkWallpaperPath: "C:\\Windows\\Web\\Wallpaper\\Windows\\img19.jpg"
  $name: Dark Mode Wallpaper Path
  $description: Switch between light and dark appearance + wallpapers
- LightThemePath: "C:\\Windows\\Resources\\Themes\\aero.theme"
  $name: Light Mode Theme Path
  $description: Switch between light and dark themes
- DarkThemePath: "C:\\Windows\\Resources\\Themes\\dark.theme"
  $name: Dark Mode Theme Path
  $description: Switch between light and dark themes
- ScriptPath: ""
  $name: Custom Script Path
- LockScreen: true
  $name: Apply Wallpaper to Lock screen
*/
// ==/WindhawkModSettings==

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <fstream>
#include <comdef.h>
#include <winrt/base.h>
#include <winrt/Windows.Devices.Geolocation.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>

using namespace winrt;
using namespace Windows::Devices::Geolocation;
using namespace winrt::Windows::System;

HANDLE g_timer = nullptr;
HANDLE g_timerThread = nullptr;
HANDLE g_wakeEvent = nullptr;
bool g_exitFlag = false;

std::wstring g_scheduleMode;
SYSTEMTIME g_lightTime, g_darkTime;
double g_latitude, g_longitude;

std::wstring g_switchMode;

std::wstring g_lightWallpaperPath, g_darkWallpaperPath;
std::wstring g_lightThemePath, g_darkThemePath;

std::wstring g_scriptPath;

bool g_lockScreen = true;

enum Appearance {
    light,
    dark
};

void ResetWorkingSet() {
    HANDLE hProcess = GetCurrentProcess();
    SetProcessWorkingSetSize(hProcess, SIZE_T(-1), SIZE_T(-1));
}

void RunScript(bool useLightTheme) {
    if (GetFileAttributesW(g_scriptPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return;
    }

    std::wstring params = useLightTheme ? L"-light" : L"-dark";

    if (g_scriptPath.size() >= 4 &&
        _wcsicmp(g_scriptPath.data() + g_scriptPath.size() - 4, L".ps1") == 0)
    {
        std::wstring args = L"-NoProfile -ExecutionPolicy Bypass -File \"" + g_scriptPath + L"\" " + params;
        if ((INT_PTR)ShellExecuteW(nullptr, L"open", L"powershell.exe", args.c_str(), nullptr, SW_HIDE) <= 32) {
            Wh_Log(L"Failed to launch script");
            return;
        } else {
            Wh_Log(L"Successfully launched script: %s", args.c_str());
        }
    }
    else
    {
        if ((INT_PTR)ShellExecuteW(nullptr, L"open", g_scriptPath.c_str(), params.c_str(), nullptr, SW_HIDE) <= 32) {
            Wh_Log(L"Failed to launch script");
            return;
        } else {
            Wh_Log(L"Successfully launched script: %s %s", g_scriptPath.c_str(), params.c_str());
        }
    }
}

void ApplyLockScreen() {
    wchar_t currentWallpaper[MAX_PATH] = {0};
    DWORD size = sizeof(currentWallpaper);

    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallPaper", RRF_RT_REG_SZ, nullptr, currentWallpaper, &size) != ERROR_SUCCESS) {
        Wh_Log(L"Failed to apply lock screen");
        return;
    }

    std::wstring wallpaperPath = currentWallpaper;
    HKEY hKey;

    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\Personalization", 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        if (RegSetValueExW(hKey, L"LockScreenImage", 0, REG_SZ, (const BYTE*)wallpaperPath.c_str(), (DWORD)((wallpaperPath.size() + 1) * sizeof(wchar_t))) != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            Wh_Log(L"Failed to apply lock screen");
            return;
        }
        RegCloseKey(hKey);
    } else {
        Wh_Log(L"Failed to apply lock screen");
        return;
    }

    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\PersonalizationCSP", 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        for (PCWSTR valueName : { L"LockScreenImagePath", L"LockScreenImageUrl" }) {
            if (RegSetValueExW(hKey, valueName, 0, REG_SZ, (const BYTE*)wallpaperPath.c_str(), (DWORD)((wallpaperPath.size() + 1) * sizeof(wchar_t))) != ERROR_SUCCESS) {
                RegCloseKey(hKey);
                Wh_Log(L"Failed to apply lock screen");
                return;
            }
        }
        RegCloseKey(hKey);
    } else {
        Wh_Log(L"Failed to apply lock screen");
        return;
    }

    Wh_Log(L"Successfully applied lock screen");
}

bool IsAppearanceApplied(Appearance appearance) {
    DWORD val = (appearance == light) ? 1 : 0, current = 1, size = sizeof(DWORD);

    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &current, &size);
    if (current != val) return false;

    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &current, &size);
    return current == val;
}

bool IsWallpaperApplied(PCWSTR wallpaperPath) {
    wchar_t currentWallpaper[MAX_PATH];
    DWORD size = sizeof(currentWallpaper);

    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallPaper", RRF_RT_REG_SZ, nullptr, currentWallpaper, &size) != ERROR_SUCCESS)
    {
        return false;
    }

    return _wcsicmp(currentWallpaper, wallpaperPath) == 0;
}

bool IsThemeApplied(PCWSTR themePath) {
    wchar_t currentTheme[MAX_PATH] = {0};
    DWORD size = sizeof(currentTheme);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes", L"CurrentTheme", RRF_RT_REG_SZ, nullptr, currentTheme, &size) != ERROR_SUCCESS)
        return false;
    if (_wcsicmp(currentTheme, themePath) != 0)
        return false;

    DWORD appsLight = 1, systemLight = 1, dataSize = sizeof(DWORD);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &appsLight, &dataSize) != ERROR_SUCCESS)
        return false;
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &systemLight, &dataSize) != ERROR_SUCCESS)
        return false;

    std::wifstream file(themePath);
    if (!file)
        return false;

    bool inVisualStyles = false;
    std::wstring line, systemMode, appMode;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == L';') continue;
        if (line[0] == L'[') {
            inVisualStyles = (line == L"[VisualStyles]");
            continue;
        }
        if (inVisualStyles) {
            if (line.find(L"SystemMode=") == 0)
                systemMode = line.substr(11);
            else if (line.find(L"AppMode=") == 0)
                appMode = line.substr(8);
            if (!systemMode.empty() && !appMode.empty())
                break;
        }
    }

    auto isLight = [](const std::wstring& s) { return _wcsicmp(s.c_str(), L"Light") == 0; };
    bool themeLight = isLight(systemMode) && isLight(appMode);

    return (appsLight == (themeLight ? 1 : 0)) && (systemLight == (themeLight ? 1 : 0));
}

void ApplyAppearance(Appearance appearance) {
    DWORD val = (appearance == light) ? 1 : 0;

    if (
        RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme", REG_DWORD, &val, sizeof(val)) == ERROR_SUCCESS &&
        RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"SystemUsesLightTheme", REG_DWORD, &val, sizeof(val)) == ERROR_SUCCESS
    ) {
        Wh_Log(L"Successfully applied %s appearance", appearance == light ? L"light" : L"dark");
    } else {
        Wh_Log(L"Failed to apply %s appearance", appearance == light ? L"light" : L"dark");
        return;
    }

    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 100, nullptr);
}

void ApplyWallpaper(PCWSTR wallpaperPath) {
    if (SystemParametersInfoW(
            SPI_SETDESKWALLPAPER,
            0,
            (PVOID)wallpaperPath,
            SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE)) {
        Wh_Log(L"Successfully applied wallpaper");
    } else {
        Wh_Log(L"Failed to apply wallpaper");
    }
}

// Based on:
// https://github.com/qwerty12/AutoHotkeyScripts/blob/a9423f59c945a3a031cb38b25cf461a34de9a6d3/SetThemeFromdotThemeFile.ahk
void ApplyTheme(PCWSTR themePath) {
    // {C04B329E-5823-4415-9C93-BA44688947B0}
    constexpr winrt::guid CLSID_IThemeManager{
        0xC04B329E,
        0x5823,
        0x4415,
        {0x9C, 0x93, 0xBA, 0x44, 0x68, 0x89, 0x47, 0xB0}};

    // {0646EBBE-C1B7-4045-8FD0-FFD65D3FC792}
    constexpr winrt::guid IID_IThemeManager{
        0x0646EBBE,
        0xC1B7,
        0x4045,
        {0x8F, 0xD0, 0xFF, 0xD6, 0x5D, 0x3F, 0xC7, 0x92}};

    winrt::com_ptr<IUnknown> pThemeManager;
    HRESULT hr =
        CoCreateInstance(CLSID_IThemeManager, nullptr, CLSCTX_INPROC_SERVER,
                        IID_IThemeManager, pThemeManager.put_void());
    if (FAILED(hr) || !pThemeManager) {
        Wh_Log(L"Failed to apply theme");
        return;
    }

    _bstr_t bstrTheme(themePath);
    void** vtable = *(void***)pThemeManager.get();
    using ApplyThemeFunc = HRESULT(WINAPI*)(IUnknown*, BSTR);
    ApplyThemeFunc ApplyThemeMethod = (ApplyThemeFunc)vtable[4];
    hr = ApplyThemeMethod(pThemeManager.get(), bstrTheme);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to apply theme");
        return;
    }

    Wh_Log(L"Successfully applied theme");

    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 100, nullptr);
}

void Apply(bool useLightTheme) {
    PCWSTR wallpaperPath = useLightTheme ? g_lightWallpaperPath.c_str() : g_darkWallpaperPath.c_str();
    PCWSTR themePath = useLightTheme ? g_lightThemePath.c_str() : g_darkThemePath.c_str();
    bool changed = false;

    if (g_switchMode == L"Theme") {
        if (*themePath) {
            for (;;) {
                if (g_exitFlag) return;
                HWND progman = FindWindowW(L"Progman", nullptr);
                HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
                if (progman && tray && IsWindowVisible(tray))
                    break;
                Sleep(500);
            }
            if (!IsThemeApplied(themePath)) {
                CoInitialize(nullptr);
                ApplyTheme(themePath);
                CoUninitialize();
                changed = true;
            } else {
                wchar_t currentWallpaper[MAX_PATH] = {};
                DWORD size = sizeof(currentWallpaper);
                bool wallpaperMismatch = false;

                if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallPaper", RRF_RT_REG_SZ, nullptr, currentWallpaper, &size) == ERROR_SUCCESS) {
                    std::wifstream file(themePath);
                    if (file) {
                        std::wstring line, wallpaperInTheme;
                        bool inDesktop = false;
                        while (std::getline(file, line)) {
                            if (line.empty() || line[0] == L';') continue;
                            if (line[0] == L'[') {
                                inDesktop = (line == L"[Control Panel\\Desktop]");
                                continue;
                            }
                            if (inDesktop && line.find(L"Wallpaper=") == 0) {
                                wallpaperInTheme = line.substr(10);
                                break;
                            }
                        }
                        if (!wallpaperInTheme.empty()) {
                            wchar_t expanded[MAX_PATH] = {};
                            ExpandEnvironmentStringsW(wallpaperInTheme.c_str(), expanded, MAX_PATH);
                            if (_wcsicmp(currentWallpaper, expanded) != 0) {
                                ApplyWallpaper(expanded);
                                changed = true;
                                wallpaperMismatch = true;
                            }
                        }
                    }
                }

                if (wallpaperMismatch)
                {
                    Wh_Log(L"Successfully applied theme");
                }
                else {
                    Wh_Log(L"Theme already applied");
                }   
            }
        }
    } else {
        if (g_switchMode == L"Wallpaper" && *wallpaperPath) {
            if (!IsWallpaperApplied(wallpaperPath)) {
                ApplyWallpaper(wallpaperPath);
                changed = true;
            } else {
                Wh_Log(L"Wallpaper already applied");
            }
        }

        if (!IsAppearanceApplied(useLightTheme ? light : dark)) {
            ApplyAppearance(useLightTheme ? light : dark);
            changed = true;
        } else {
            Wh_Log(L"Appearance already applied");
        }
    }

    if (changed && Wh_GetIntSetting(L"LockScreen")) {
        ApplyLockScreen();
    }

    if (changed && !g_scriptPath.empty()) {
        RunScript(useLightTheme);
    }

    ResetWorkingSet();
}

void ApplyCurrent() {
    time_t now = time(nullptr);
    struct tm local;
    localtime_s(&local, &now);

    auto makeTime = [&](const SYSTEMTIME& st) {
        struct tm t = local;
        t.tm_hour = st.wHour; t.tm_min = st.wMinute; t.tm_sec = 0;
        return mktime(&t);
    };

    time_t lightT = makeTime(g_lightTime);
    time_t darkT = makeTime(g_darkTime);

    bool isLightNow;
    if (lightT < darkT)
        isLightNow = now >= lightT && now < darkT;
    else
        isLightNow = now >= lightT || now < darkT;

    Apply(isLightNow);
}

SYSTEMTIME ParseScheduleTime(PCWSTR timeStr) {
    SYSTEMTIME st = {};
    swscanf_s(timeStr, L"%hu:%hu", &st.wHour, &st.wMinute);
    return st;
}

time_t GetNextSwitch(const SYSTEMTIME& light, const SYSTEMTIME& dark, bool& nextLight) {
    time_t now = time(nullptr);
    struct tm local;
    localtime_s(&local, &now);

    auto makeTime = [&](const SYSTEMTIME& st) {
        struct tm t = local;
        t.tm_hour = st.wHour; t.tm_min = st.wMinute; t.tm_sec = 0;
        return mktime(&t);
    };

    time_t lightT = makeTime(light);
    time_t darkT = makeTime(dark);

    bool isLightNow;
    if (lightT < darkT)
        isLightNow = now >= lightT && now < darkT;
    else
        isLightNow = now >= lightT || now < darkT;

    if (isLightNow) {
        nextLight = false;
        if (darkT <= now) darkT += 86400;
        return darkT;
    } else {
        nextLight = true;
        if (lightT <= now) lightT += 86400;
        return lightT;
    }
}

DWORD WINAPI ThemeScheduler(LPVOID) {
    LARGE_INTEGER dueTime;
    HANDLE handles[] = { g_wakeEvent, g_timer };

    while (!g_exitFlag) {
        bool nextLight;
        time_t nextSwitch = GetNextSwitch(g_lightTime, g_darkTime, nextLight);
        time_t now = time(nullptr);
        int secondsToWait = (int)(nextSwitch - now);
        if (secondsToWait < 0) secondsToWait = 0;

        dueTime.QuadPart = -((LONGLONG)secondsToWait * 10000000LL);

        SetWaitableTimer(g_timer, &dueTime, 0, nullptr, nullptr, TRUE);

        DWORD res = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        if (res == WAIT_OBJECT_0) {
            if (g_exitFlag) break;
            continue;
        }

        Apply(nextLight);
    }

    return 0;
}

void StartScheduler() {
    ApplyCurrent();

    if (g_timerThread) {
        SetEvent(g_wakeEvent);
        return;
    }

    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_timer = CreateWaitableTimerW(nullptr, TRUE, nullptr);

    g_timerThread = CreateThread(nullptr, 0, ThemeScheduler, nullptr, 0, nullptr);
}

BasicGeoposition GetLocation()
{
    auto permission = Geolocator::RequestAccessAsync().get();

    if (permission == GeolocationAccessStatus::Allowed)
    {
        Geolocator locator;
        locator.DesiredAccuracy(PositionAccuracy::Default);

        Geoposition location = locator.GetGeopositionAsync().get();

        return location.Coordinate().Point().Position();
    }
    else
    {
        if (Geolocator::DefaultGeoposition())
        {
            return Geolocator::DefaultGeoposition().Value();
        }
        else
        {
            Wh_Log(L"Location services are disabled");

            winrt::Windows::System::Launcher::LaunchUriAsync(winrt::Windows::Foundation::Uri(L"ms-settings:privacy-location"));

            return {};
        }
    }
}

// Based on:
// https://github.com/aureldussauge/SunriseSunset
// Licensed under Apache License 2.0
class Sunriset {
public:
    static void SunriseSunset(int year, int month, int day, double lat, double lng, double& tsunrise, double& tsunset) {
        SunriseSunsetCore(year, month, day, lng, lat, SunriseSunsetAltitude, true, tsunrise, tsunset);
    }

private:
    static constexpr double SunriseSunsetAltitude = -35.0 / 60.0;
    static constexpr double RadDeg = 180.0 / M_PI;
    static constexpr double DegRad = M_PI / 180.0;
    static constexpr double INV360 = 1.0 / 360.0;

    static long daysSince2000Jan0(int y, int m, int d) {
        return (367L * y - ((7 * (y + ((m + 9) / 12))) / 4) + ((275 * m) / 9) + d - 730530L);
    }

    static double sind(double x) { return sin(x * DegRad); }
    static double cosd(double x) { return cos(x * DegRad); }
    static double tand(double x) { return tan(x * DegRad); }
    static double atand(double x) { return RadDeg * atan(x); }
    static double asind(double x) { return RadDeg * asin(x); }
    static double acosd(double x) { return RadDeg * acos(x); }
    static double atan2d(double y, double x) { return RadDeg * atan2(y, x); }

    static double revolution(double x) {
        return (x - 360.0 * floor(x * INV360));
    }

    static double rev180(double x) {
        return (x - 360.0 * floor(x * INV360 + 0.5));
    }

    static void sunpos(double d, double& lon, double& r) {
        double M = revolution(356.0470 + 0.9856002585 * d);
        double w = 282.9404 + 4.70935E-5 * d;
        double e = 0.016709 - 1.151E-9 * d;
        double E = M + e * RadDeg * sind(M) * (1.0 + e * cosd(M));
        double x = cosd(E) - e;
        double y = sqrt(1.0 - e * e) * sind(E);
        r = sqrt(x * x + y * y);
        double v = atan2d(y, x);
        lon = v + w;
        if (lon >= 360.0) lon -= 360.0;
    }

    static void sun_RA_dec(double d, double& RA, double& dec, double& r) {
        double lon, obl_ecl, x, y, z;
        sunpos(d, lon, r);
        x = r * cosd(lon);
        y = r * sind(lon);
        obl_ecl = 23.4393 - 3.563E-7 * d;
        z = y * sind(obl_ecl);
        y = y * cosd(obl_ecl);
        RA = atan2d(y, x);
        dec = atan2d(z, sqrt(x * x + y * y));
    }

    static double GMST0(double d) {
        return revolution((180.0 + 356.0470 + 282.9404) + (0.9856002585 + 4.70935E-5) * d);
    }

    static int SunriseSunsetCore(int year, int month, int day, double lon, double lat,
        double altit, bool upper_limb, double& trise, double& tset) {
        double d = daysSince2000Jan0(year, month, day) + 0.5 - lon / 360.0;
        double sidtime = revolution(GMST0(d) + 180.0 + lon);
        double sRA, sdec, sr;
        sun_RA_dec(d, sRA, sdec, sr);
        double tsouth = 12.0 - rev180(sidtime - sRA) / 15.0;
        double sradius = 0.2666 / sr;
        if (upper_limb) altit -= sradius;
        double cost = (sind(altit) - sind(lat) * sind(sdec)) / (cosd(lat) * cosd(sdec));
        double t;
        int rc = 0;
        if (cost >= 1.0) {
            rc = -1; t = 0.0;
        }
        else if (cost <= -1.0) {
            rc = +1; t = 12.0;
        }
        else {
            t = acosd(cost) / 15.0;
        }
        trise = tsouth - t;
        tset = tsouth + t;
        return rc;
    }
};

void GetSunriseSunsetTimes(double latitude, double longitude, SYSTEMTIME& sunrise, SYSTEMTIME& sunset)
{
    time_t now = time(nullptr);
    struct tm* now_tm = gmtime(&now);
    int year = now_tm->tm_year + 1900;
    int month = now_tm->tm_mon + 1;
    int day = now_tm->tm_mday;

    double tsunrise, tsunset;
    Sunriset::SunriseSunset(year, month, day, latitude, longitude, tsunrise, tsunset);

    auto convertToLocalSystemTime = [](double timeVal, SYSTEMTIME& localST)
    {
        SYSTEMTIME utcST = {};
        utcST.wYear = 2000;
        utcST.wMonth = 1;
        utcST.wDay = 1;
        utcST.wHour = static_cast<WORD>(timeVal);
        utcST.wMinute = static_cast<WORD>((timeVal - utcST.wHour) * 60);
        utcST.wSecond = static_cast<WORD>((((timeVal - utcST.wHour) * 60) - utcST.wMinute) * 60);

        FILETIME utcFT, localFT;
        SystemTimeToFileTime(&utcST, &utcFT);
        FileTimeToLocalFileTime(&utcFT, &localFT);
        FileTimeToSystemTime(&localFT, &localST);
    };

    convertToLocalSystemTime(tsunrise, sunrise);
    convertToLocalSystemTime(tsunset, sunset);
}

std::wstring TrimQuotes(const std::wstring& str) {
    size_t start = 0;
    size_t end = str.length();
    if (!str.empty() && str.front() == L'"') start = 1;
    if (end > start && str[end - 1] == L'"') end--;
    return str.substr(start, end - start);
}

void LoadSettings() {
    auto rawScheduleMode = Wh_GetStringSetting(L"ScheduleMode");
    g_scheduleMode = rawScheduleMode ? std::wstring(rawScheduleMode) : L"CustomHours";
    if (rawScheduleMode) Wh_FreeStringSetting(rawScheduleMode);

    auto rawLight = Wh_GetStringSetting(L"CustomLight");
    auto rawDark = Wh_GetStringSetting(L"CustomDark");

    auto rawLatitude = Wh_GetStringSetting(L"Latitude");
    g_latitude = rawLatitude ? _wtof(rawLatitude) : 0.0;
    if (rawLatitude) Wh_FreeStringSetting(rawLatitude);

    auto rawLongitude = Wh_GetStringSetting(L"Longitude");
    g_longitude = rawLongitude ? _wtof(rawLongitude) : 0.0;
    if (rawLongitude) Wh_FreeStringSetting(rawLongitude);

    if (g_scheduleMode == L"CustomHours") {
        g_lightTime = ParseScheduleTime(rawLight);
        g_darkTime = ParseScheduleTime(rawDark);
    } else if (g_scheduleMode == L"LocationService") {
        BasicGeoposition pos = GetLocation();

        SYSTEMTIME sunriseTime, sunsetTime;
        GetSunriseSunsetTimes(pos.Latitude, pos.Longitude, sunriseTime, sunsetTime);

        g_lightTime = sunriseTime;
        g_darkTime = sunsetTime;

    } else if (g_scheduleMode == L"CustomCoordinates") {
        SYSTEMTIME sunriseTime, sunsetTime;
        GetSunriseSunsetTimes(g_latitude, g_longitude, sunriseTime, sunsetTime);

        g_lightTime = sunriseTime;
        g_darkTime = sunsetTime;
    }
    
    Wh_Log(L"Light: %02d:%02d:%02d, Dark: %02d:%02d:%02d", g_lightTime.wHour, g_lightTime.wMinute, g_lightTime.wSecond, g_darkTime.wHour, g_darkTime.wMinute, g_darkTime.wSecond);

    if (rawLight) Wh_FreeStringSetting(rawLight);
    if (rawDark) Wh_FreeStringSetting(rawDark);

    auto rawSwitchMode = Wh_GetStringSetting(L"SwitchMode");
    g_switchMode = rawSwitchMode ? std::wstring(rawSwitchMode) : L"Appearance";
    if (rawSwitchMode) Wh_FreeStringSetting(rawSwitchMode);

    auto rawLightWallpaperPath = Wh_GetStringSetting(L"LightWallpaperPath");
    g_lightWallpaperPath = rawLightWallpaperPath ? TrimQuotes(rawLightWallpaperPath) : L"";
    if (rawLightWallpaperPath) Wh_FreeStringSetting(rawLightWallpaperPath);

    auto rawDarkWallpaperPath = Wh_GetStringSetting(L"DarkWallpaperPath");
    g_darkWallpaperPath = rawDarkWallpaperPath ? TrimQuotes(rawDarkWallpaperPath) : L"";
    if (rawDarkWallpaperPath) Wh_FreeStringSetting(rawDarkWallpaperPath);

    auto rawLightThemePath = Wh_GetStringSetting(L"LightThemePath");
    g_lightThemePath = rawLightThemePath ? TrimQuotes(rawLightThemePath) : L"";
    if (rawLightThemePath) Wh_FreeStringSetting(rawLightThemePath);

    auto rawDarkThemePath = Wh_GetStringSetting(L"DarkThemePath");
    g_darkThemePath = rawDarkThemePath ? TrimQuotes(rawDarkThemePath) : L"";
    if (rawDarkThemePath) Wh_FreeStringSetting(rawDarkThemePath);

    auto rawScriptPath = Wh_GetStringSetting(L"ScriptPath");
    g_scriptPath = rawScriptPath ? TrimQuotes(rawScriptPath) : L"";
    if (rawScriptPath) Wh_FreeStringSetting(rawScriptPath);

    g_lockScreen = Wh_GetIntSetting(L"LockScreen", 1) != 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    StartScheduler();
    ResetWorkingSet();
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    StartScheduler();
    ResetWorkingSet();
}

void WhTool_ModUninit() {
    g_exitFlag = true;
    if (g_wakeEvent) SetEvent(g_wakeEvent);
    if (g_timerThread) {
        WaitForSingleObject(g_timerThread, INFINITE);
        CloseHandle(g_timerThread);
    }
    if (g_timer) CloseHandle(g_timer);
    if (g_wakeEvent) CloseHandle(g_wakeEvent);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk-mods/pull/1916
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
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
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

    if (isService) {
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

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
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
