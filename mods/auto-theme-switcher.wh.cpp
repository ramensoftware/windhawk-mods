// ==WindhawkMod==
// @id              auto-theme-switcher
// @name            Auto Theme Switcher
// @description     Automatically switch between light and dark appearance/wallpapers/themes based on a custom hours/sunset to sunrise with custom script support
// @version         1.2.0
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
    - *(default: `07:00:00` for light, `19:00:00` for dark)*  
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
    - Windows built-in wallpapers: `C:\Windows\Web\Wallpaper\Windows`  
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

- Apply current wallpaper to the Lock Screen
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
- CustomLight: 07:00:00
  $name: Light Mode Time
  $description: Set custom hours (HH:mm:ss or HH:mm)
- CustomDark: 19:00:00
  $name: Dark Mode Time
  $description: Set custom hours (HH:mm:ss or HH:mm)
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

#include <comdef.h>
#include <winrt/base.h>
#include <winrt/Windows.Devices.Geolocation.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.System.UserProfile.h>
#include <winrt/Windows.Storage.h>

using namespace winrt;
using namespace Windows::Devices::Geolocation;
using namespace winrt::Windows::System;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::System::UserProfile;
using namespace winrt::Windows::Foundation;

enum class ScheduleMode {
    CustomHours,
    LocationService,
    CustomCoordinates,
};

enum class SwitchMode {
    Appearance,
    Wallpaper,
    Theme,
};

struct {
    ScheduleMode scheduleMode;
    SYSTEMTIME lightTime;
    SYSTEMTIME darkTime;
    double latitude;
    double longitude;
    SwitchMode switchMode;
    std::wstring lightWallpaperPath;
    std::wstring darkWallpaperPath;
    std::wstring lightThemePath;
    std::wstring darkThemePath;
    std::wstring scriptPath;
    bool lockScreen;
} g_settings;

HANDLE g_timer = nullptr;
HANDLE g_timerThread = nullptr;
HANDLE g_wakeEvent = nullptr;
bool g_exitFlag = false;
HPOWERNOTIFY g_hPowerNotify = nullptr;

enum Appearance {
    light,
    dark
};

SYSTEMTIME ParseScheduleTime(PCWSTR timeStr) {
    SYSTEMTIME st = {};
    if (timeStr) {
        if (swscanf_s(timeStr, L"%hu:%hu:%hu", &st.wHour, &st.wMinute, &st.wSecond) < 3) {
            st.wSecond = 0;
        }
    }
    return st;
}

bool GetLocation(double& lat, double& lng)
{
    try {
        auto accessOp = Geolocator::RequestAccessAsync();
        if (accessOp.get() != GeolocationAccessStatus::Allowed) return false;

        Geolocator locator;
        locator.DesiredAccuracy(PositionAccuracy::Default);

        Geoposition location = locator.GetGeopositionAsync().get();
        auto pos = location.Coordinate().Point().Position();

        lat = pos.Latitude;
        lng = pos.Longitude;

        wchar_t buf[32];
        swprintf_s(buf, L"%.6f", lat);
        Wh_SetStringValue(L"CachedLat", buf);
        swprintf_s(buf, L"%.6f", lng);
        Wh_SetStringValue(L"CachedLng", buf);

        return true;
    } catch (...) {
        return false;
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
    struct tm now_tm;
    gmtime_s(&now_tm, &now);

    double tsunrise, tsunset;
    Sunriset::SunriseSunset(now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday, latitude, longitude, tsunrise, tsunset);

    auto convertToLocalSystemTime = [&](double timeVal) -> SYSTEMTIME
    {
        struct tm utc_midnight = now_tm;
        utc_midnight.tm_hour = 0;
        utc_midnight.tm_min = 0;
        utc_midnight.tm_sec = 0;
        
        time_t utc_time = _mkgmtime(&utc_midnight) + (time_t)(timeVal * 3600.0);
        
        struct tm local_tm;
        localtime_s(&local_tm, &utc_time);
        
        SYSTEMTIME st = { (WORD)(local_tm.tm_year + 1900), (WORD)(local_tm.tm_mon + 1), 0, (WORD)local_tm.tm_mday, (WORD)local_tm.tm_hour, (WORD)local_tm.tm_min, (WORD)local_tm.tm_sec, 0 };
        return st;
    };

    sunrise = convertToLocalSystemTime(tsunrise);
    sunset = convertToLocalSystemTime(tsunset);
}

void RunScript(const std::wstring& scriptPath, bool useLightTheme) {
    if (scriptPath.empty() || GetFileAttributesW(scriptPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return;
    }

    std::wstring params = useLightTheme ? L"-light" : L"-dark";

    if (scriptPath.size() >= 4 &&
        _wcsicmp(scriptPath.data() + scriptPath.size() - 4, L".ps1") == 0)
    {
        std::wstring args = L"-NoProfile -ExecutionPolicy Bypass -File \"" + scriptPath + L"\" " + params;
        if ((INT_PTR)ShellExecuteW(nullptr, L"open", L"powershell.exe", args.c_str(), nullptr, SW_HIDE) <= 32) {
            Wh_Log(L"Failed to launch script");
            return;
        } else {
            Wh_Log(L"Successfully launched script: %s", args.c_str());
        }
    }
    else
    {
        if ((INT_PTR)ShellExecuteW(nullptr, L"open", scriptPath.c_str(), params.c_str(), nullptr, SW_HIDE) <= 32) {
            Wh_Log(L"Failed to launch script");
            return;
        } else {
            Wh_Log(L"Successfully launched script: %s %s", scriptPath.c_str(), params.c_str());
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

    try {
        StorageFile file = StorageFile::GetFileFromPathAsync(currentWallpaper).get();
        IAsyncAction action = LockScreen::SetImageFileAsync(file);
        action.get();
        Wh_Log(L"Successfully applied lock screen");
    } catch (...) {
        Wh_Log(L"Failed to apply lock screen.");
    }
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
        return false;
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

    wchar_t systemMode[64] = {}, appMode[64] = {};
    GetPrivateProfileStringW(L"VisualStyles", L"SystemMode", L"", systemMode, ARRAYSIZE(systemMode), themePath);
    GetPrivateProfileStringW(L"VisualStyles", L"AppMode", L"", appMode, ARRAYSIZE(appMode), themePath);

    auto isLight = [](PCWSTR s) { return _wcsicmp(s, L"Light") == 0; };
    bool themeLight = isLight(systemMode) && isLight(appMode);

    return (appsLight == (themeLight ? 1 : 0)) && (systemLight == (themeLight ? 1 : 0));
}

void ApplyAppearance(Appearance appearance) {
    DWORD val = (appearance == light) ? 1 : 0;
    if (RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme", REG_DWORD, &val, sizeof(val)) == ERROR_SUCCESS &&
        RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", REG_DWORD, &val, sizeof(val)) == ERROR_SUCCESS) {
        Wh_Log(L"Applied %s appearance", appearance == light ? L"light" : L"dark");
    }
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 100, nullptr);
}

void ApplyWallpaper(PCWSTR wallpaperPath) {
    if (SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)wallpaperPath, SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE))
        Wh_Log(L"Applied wallpaper: %s", wallpaperPath);
}

struct IThemeManager : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Reserved1() = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyTheme(BSTR themePath) = 0;
};

void ApplyTheme(PCWSTR themePath) {
    constexpr winrt::guid CLSID_IThemeManager{ 0xC04B329E, 0x5823, 0x4415, {0x9C, 0x93, 0xBA, 0x44, 0x68, 0x89, 0x47, 0xB0}};
    constexpr winrt::guid IID_IThemeManager{ 0x0646EBBE, 0xC1B7, 0x4045, {0x8F, 0xD0, 0xFF, 0xD6, 0x5D, 0x3F, 0xC7, 0x92}};

    winrt::com_ptr<IThemeManager> pThemeManager;
    if (FAILED(CoCreateInstance(CLSID_IThemeManager, nullptr, CLSCTX_INPROC_SERVER, IID_IThemeManager, pThemeManager.put_void()))) return;

    _bstr_t bstrTheme(themePath);
    if (SUCCEEDED(pThemeManager->ApplyTheme(bstrTheme)))
        Wh_Log(L"Applied theme: %s", themePath);
}

void Apply(bool useLightTheme, SwitchMode switchMode, const std::wstring& lightWallpaper, const std::wstring& darkWallpaper, const std::wstring& lightTheme, const std::wstring& darkTheme, const std::wstring& scriptPath, bool lockScreen) {
    PCWSTR wallpaperPath = useLightTheme ? lightWallpaper.c_str() : darkWallpaper.c_str();
    PCWSTR themePath = useLightTheme ? lightTheme.c_str() : darkTheme.c_str();

    bool changed = false;

    if (switchMode == SwitchMode::Theme && *themePath) {
        while (true) {
            if (g_exitFlag) return;
            HWND shellWnd = GetShellWindow();
            HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
            if (shellWnd && tray && IsWindowVisible(tray)) break;
            Sleep(100);
        }

        if (!IsThemeApplied(themePath)) {
            ApplyTheme(themePath);
            changed = true;
        } else {
            wchar_t currentWallpaper[MAX_PATH] = {};
            DWORD size = sizeof(currentWallpaper);
            if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallPaper", RRF_RT_REG_SZ, nullptr, currentWallpaper, &size) == ERROR_SUCCESS) {
                wchar_t wallpaperInTheme[MAX_PATH] = {};
                GetPrivateProfileStringW(L"Control Panel\\Desktop", L"Wallpaper", L"", wallpaperInTheme, ARRAYSIZE(wallpaperInTheme), themePath);
                if (wallpaperInTheme[0]) {
                    wchar_t expanded[MAX_PATH] = {};
                    ExpandEnvironmentStringsW(wallpaperInTheme, expanded, MAX_PATH);
                    if (_wcsicmp(currentWallpaper, expanded) != 0) {
                        ApplyWallpaper(expanded);
                        changed = true;
                    }
                }
            }
        }
    } else {
        if (switchMode == SwitchMode::Wallpaper && *wallpaperPath && !IsWallpaperApplied(wallpaperPath)) {
            ApplyWallpaper(wallpaperPath);
            changed = true;
        }
        if (!IsAppearanceApplied(useLightTheme ? light : dark)) {
            ApplyAppearance(useLightTheme ? light : dark);
            changed = true;
        }
    }

    if (changed) {
        if (lockScreen) ApplyLockScreen();
        if (!scriptPath.empty()) RunScript(scriptPath, useLightTheme);
    }
}

time_t UpdateThemeState() {
    time_t now = time(nullptr);
    struct tm now_tm;
    localtime_s(&now_tm, &now);

    SYSTEMTIME lightST = g_settings.lightTime;
    SYSTEMTIME darkST = g_settings.darkTime;

    if (g_settings.scheduleMode == ScheduleMode::LocationService || 
        g_settings.scheduleMode == ScheduleMode::CustomCoordinates) {
        
        double lat = g_settings.latitude;
        double lng = g_settings.longitude;

        bool locSuccess = false;
        if (g_settings.scheduleMode == ScheduleMode::LocationService) {
            locSuccess = GetLocation(lat, lng);
            if (locSuccess) {
                g_settings.latitude = lat;
                g_settings.longitude = lng;
            } else {
                wchar_t latBuf[32] = {0}, lngBuf[32] = {0};
                if (Wh_GetStringValue(L"CachedLat", latBuf, 32) > 0 && 
                    Wh_GetStringValue(L"CachedLng", lngBuf, 32) > 0) {
                    lat = _wtof(latBuf);
                    lng = _wtof(lngBuf);
                    Wh_Log(L"Location fetch failed, using coordinates from local storage: %f, %f", lat, lng);
                } else {
                    Wh_Log(L"Location fetch failed and no cache found, using settings coordinates.");
                    lat = g_settings.latitude;
                    lng = g_settings.longitude;
                }
            }
        }
        
        GetSunriseSunsetTimes(lat, lng, lightST, darkST);

        if (locSuccess || g_settings.scheduleMode == ScheduleMode::CustomCoordinates) {
            PCWSTR sourceStr = (g_settings.scheduleMode == ScheduleMode::CustomCoordinates) ? L"CustomCoordinates" : (locSuccess ? L"LocationService (Fresh)" : L"LocationService (Cached)");
            Wh_Log(L"Active Schedule (%s): %f, %f", sourceStr, lat, lng);
            Wh_Log(L"Active Schedule (%s): Light: %02d:%02d:%02d, Dark: %02d:%02d:%02d",
                sourceStr,
                lightST.wHour, lightST.wMinute, lightST.wSecond,
                darkST.wHour, darkST.wMinute, darkST.wSecond);
        }


        if (g_settings.scheduleMode == ScheduleMode::LocationService && !locSuccess) {

            auto getAbsInternal = [&](const SYSTEMTIME& st, int dayOffset) {
                struct tm t = now_tm;
                t.tm_hour = st.wHour; t.tm_min = st.wMinute; t.tm_sec = st.wSecond;
                t.tm_mday += dayOffset; t.tm_isdst = -1;
                return mktime(&t);
            };
            time_t lT = getAbsInternal(lightST, 0);
            time_t dT = getAbsInternal(darkST, 0);
            bool isLight = (lT < dT) ? (now >= lT && now < dT) : (now >= lT || now < dT);
            Apply(isLight, g_settings.switchMode, g_settings.lightWallpaperPath, g_settings.darkWallpaperPath, g_settings.lightThemePath, g_settings.darkThemePath, g_settings.scriptPath, g_settings.lockScreen);


            while (true) {
                if (g_exitFlag) return 0;
                HWND shellWnd = GetShellWindow();
                HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
                if (shellWnd && tray && IsWindowVisible(tray)) break;
                Sleep(100);
            }

            if (GetLocation(lat, lng)) {
                g_settings.latitude = lat;
                g_settings.longitude = lng;
                GetSunriseSunsetTimes(lat, lng, lightST, darkST);
                
                PCWSTR sourceStr = L"LocationService (Fresh)";
                Wh_Log(L"Active Schedule (%s): %f, %f", sourceStr, lat, lng);
                Wh_Log(L"Active Schedule (%s): Light: %02d:%02d:%02d, Dark: %02d:%02d:%02d",
                    sourceStr,
                    lightST.wHour, lightST.wMinute, lightST.wSecond,
                    darkST.wHour, darkST.wMinute, darkST.wSecond);
            }
        }
    }

    auto getAbs = [&](const SYSTEMTIME& st, int dayOffset) {
        struct tm t = now_tm;
        t.tm_hour = st.wHour; t.tm_min = st.wMinute; t.tm_sec = st.wSecond;
        t.tm_mday += dayOffset; t.tm_isdst = -1;
        return mktime(&t);
    };

    time_t lightT = getAbs(lightST, 0);
    time_t darkT = getAbs(darkST, 0);
    bool isLightNow = (lightT < darkT) ? (now >= lightT && now < darkT) : (now >= lightT || now < darkT);

    Apply(isLightNow, g_settings.switchMode, g_settings.lightWallpaperPath, g_settings.darkWallpaperPath, g_settings.lightThemePath, g_settings.darkThemePath, g_settings.scriptPath, g_settings.lockScreen);

    time_t candidates[] = { lightT, darkT, getAbs(lightST, 1), getAbs(darkST, 1) };
    time_t next = 0;
    for (time_t c : candidates) {
        if (c > now && (next == 0 || c < next)) next = c;
    }
    return next;
}

#ifndef DEVICE_NOTIFY_CALLBACK
#define DEVICE_NOTIFY_CALLBACK 2
#endif

#ifndef PBT_APMRESUMESUSPEND
#define PBT_APMRESUMESUSPEND 0x0007
#endif

typedef struct _DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS {
    PVOID Callback;
    PVOID Context;
} DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS;

static ULONG WINAPI PowerResumeCallback(PVOID Context, ULONG Type, PVOID Setting) {
    if (Type == PBT_APMRESUMESUSPEND) {
        if (Context) SetEvent((HANDLE)Context);
    }
    return 0;
}

DWORD WINAPI ThemeScheduler(LPVOID) {
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    Wh_Log(L"Theme scheduler started.");

    HANDLE handles[] = { g_wakeEvent, g_timer };
    
    HMODULE hPowrProf = GetModuleHandle(L"powrprof.dll");
    if (hPowrProf) {
        typedef HPOWERNOTIFY (WINAPI *RegisterEx_t)(DWORD, PVOID, PHPOWERNOTIFY);
        auto RegisterEx = (RegisterEx_t)GetProcAddress(hPowrProf, "PowerRegisterSuspendResumeNotification");
        if (RegisterEx) {
            static DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS params;
            params.Callback = (PVOID)PowerResumeCallback;
            params.Context = g_wakeEvent;
            RegisterEx(DEVICE_NOTIFY_CALLBACK, &params, &g_hPowerNotify);
        }
    }

    while (!g_exitFlag) {
        time_t next = UpdateThemeState();
        if (g_exitFlag) break;

        if (next > 0) {
            LARGE_INTEGER due;
            due.QuadPart = (LONGLONG)next * 10000000LL + 116444736000000000LL; 
            SetWaitableTimer(g_timer, &due, 0, nullptr, nullptr, FALSE);
            struct tm n_tm; localtime_s(&n_tm, &next);
            Wh_Log(L"Waiting until %02d:%02d:%02d for next switch.", n_tm.tm_hour, n_tm.tm_min, n_tm.tm_sec);
        }

        WaitForMultipleObjects(2, handles, FALSE, INFINITE);
    }

    Wh_Log(L"Theme scheduler stopped.");
    return 0;
}

void StartScheduler() {
    if (g_timerThread) {
        SetEvent(g_wakeEvent);
        return;
    }

    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_timer = CreateWaitableTimerW(nullptr, TRUE, nullptr);

    g_timerThread = CreateThread(nullptr, 0, ThemeScheduler, nullptr, 0, nullptr);
}

void LoadSettings() {
    auto getString = [](PCWSTR name, PCWSTR defaultValue = L"") {
        PCWSTR value = Wh_GetStringSetting(name);
        std::wstring result = value ? value : defaultValue;
        Wh_FreeStringSetting(value);
        if (result.size() >= 2 && result.front() == L'"' && result.back() == L'"') {
            result = result.substr(1, result.size() - 2);
        }
        return result;
    };

    g_settings.lightWallpaperPath = getString(L"LightWallpaperPath", L"C:\\Windows\\Web\\Wallpaper\\Windows\\img0.jpg");
    g_settings.darkWallpaperPath = getString(L"DarkWallpaperPath", L"C:\\Windows\\Web\\Wallpaper\\Windows\\img19.jpg");
    g_settings.lightThemePath = getString(L"LightThemePath", L"C:\\Windows\\Resources\\Themes\\aero.theme");
    g_settings.darkThemePath = getString(L"DarkThemePath", L"C:\\Windows\\Resources\\Themes\\dark.theme");
    g_settings.scriptPath = getString(L"ScriptPath");

    PCWSTR scheduleMode = Wh_GetStringSetting(L"ScheduleMode");
    g_settings.scheduleMode = ScheduleMode::CustomHours;
    if (scheduleMode) {
        if (wcscmp(scheduleMode, L"LocationService") == 0) g_settings.scheduleMode = ScheduleMode::LocationService;
        else if (wcscmp(scheduleMode, L"CustomCoordinates") == 0) g_settings.scheduleMode = ScheduleMode::CustomCoordinates;
        Wh_FreeStringSetting(scheduleMode);
    }

    PCWSTR switchMode = Wh_GetStringSetting(L"switchMode");
    g_settings.switchMode = SwitchMode::Appearance;
    if (switchMode) {
        if (wcscmp(switchMode, L"Wallpaper") == 0) g_settings.switchMode = SwitchMode::Wallpaper;
        else if (wcscmp(switchMode, L"Theme") == 0) g_settings.switchMode = SwitchMode::Theme;
        Wh_FreeStringSetting(switchMode);
    }

    PCWSTR latitude = Wh_GetStringSetting(L"Latitude");
    g_settings.latitude = latitude ? _wtof(latitude) : 0.0;
    Wh_FreeStringSetting(latitude);

    PCWSTR longitude = Wh_GetStringSetting(L"Longitude");
    g_settings.longitude = longitude ? _wtof(longitude) : 0.0;
    Wh_FreeStringSetting(longitude);

    PCWSTR customLight = Wh_GetStringSetting(L"CustomLight");
    g_settings.lightTime = ParseScheduleTime(customLight ? customLight : L"07:00:00");
    Wh_FreeStringSetting(customLight);

    PCWSTR customDark = Wh_GetStringSetting(L"CustomDark");
    g_settings.darkTime = ParseScheduleTime(customDark ? customDark : L"19:00:00");
    Wh_FreeStringSetting(customDark);

    g_settings.lockScreen = Wh_GetIntSetting(L"LockScreen", 1) != 0;

    if (g_settings.scheduleMode == ScheduleMode::CustomHours) {
        Wh_Log(L"Active Schedule (CustomHours): Light: %02d:%02d:%02d, Dark: %02d:%02d:%02d",
            g_settings.lightTime.wHour, g_settings.lightTime.wMinute, g_settings.lightTime.wSecond,
            g_settings.darkTime.wHour, g_settings.darkTime.wMinute, g_settings.darkTime.wSecond);
    }
}

BOOL WhTool_ModInit() {
    LoadSettings();
    StartScheduler();
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    StartScheduler();
}

void WhTool_ModUninit() {
    g_exitFlag = true;
    if (g_wakeEvent) SetEvent(g_wakeEvent);
    if (g_timerThread) {
        WaitForSingleObject(g_timerThread, INFINITE);
        CloseHandle(g_timerThread);
    }

    if (g_hPowerNotify) {
        HMODULE hModule = GetModuleHandle(L"powrprof.dll");
        if (hModule) {
            typedef BOOL (WINAPI *Unregister_t)(HPOWERNOTIFY);
            auto Unregister = (Unregister_t)GetProcAddress(hModule, "PowerUnregisterSuspendResumeNotification");
            if (Unregister) Unregister(g_hPowerNotify);
        }
        g_hPowerNotify = nullptr;
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