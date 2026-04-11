// ==WindhawkMod==
// @id              auto-theme-switcher
// @name            Auto Theme Switcher
// @description     Automatically switch between light and dark appearance/wallpapers/themes based on a schedule with hotkey and custom script support
// @version         1.3.0
// @author          tinodin
// @github          https://github.com/tinodin
// @include         explorer.exe
// @license         GPL-3.0
// @compilerOptions -lole32 -loleaut32 -lwindowsapp -lruntimeobject -lkernel32 -luser32 -lshell32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For pull requests, development takes place here:
// https://github.com/tinodin/windhawk-mods/tree/main

// ==WindhawkModReadme==
/*
# Auto Theme Switcher

### Scheduling Modes  
*(default: Custom)*

- Custom
    - *(default: `07:00:00` for light, `19:00:00` for dark)*  
- Sunset to sunrise (location service)
- Sunset to sunrise (custom geographic coordinates)
  - *(default: `0` for Latitude, `0` for Longitude)*  
  - Coordinates can be found at [latlong.net](https://www.latlong.net/)
- Hotkey only

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

### Hotkey (Toggle Theme)
*(default: None)*

- Provide a hotkey to manually toggle the current theme

Hotkeys are specified in the format: `Modifier+Modifier+Key`

**Modifiers:** Alt, Ctrl, Shift, Win

**Keys:**
- Letters: A-Z
- Numbers: 0-9
- Function keys: F1-F24
- Navigation: Home, End, PageUp, PageDown, Insert, Delete, Left, Right, Up, Down
- Common: Enter, Tab, Space, Backspace, Escape (or Esc), CapsLock, PrintScreen,
  Pause
- Numpad: Numpad0-Numpad9 (or Num0-Num9), NumLock, Add, Subtract, Multiply,
  Divide, Decimal
- Media: VolumeMute, VolumeUp, VolumeDown, MediaPlayPause, MediaNext, MediaPrev,
  MediaStop
- Modifier keys (for Virtual key press): LWin, RWin, LShift, RShift, LCtrl,
  RCtrl, LAlt, RAlt
- Some of the VK_* codes from [Virtual-Key
  Codes](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)

**Examples:**
- `Ctrl+Alt+D` - Ctrl + Alt + D
- `Ctrl+Shift+F1` - Ctrl + Shift + F1
- `Alt+Home` - Alt + Home
- `Win+Numpad1` - Windows key + Numpad 1

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
- ScheduleMode: Custom
  $name: Scheduling Mode
  $options:
  - Custom: Custom
  - LocationService: Sunset to sunrise (location service)
  - CustomCoordinates: Sunset to sunrise (custom geographic coordinates)
  - Hotkey: Hotkey only
- CustomLight: 07:00:00
  $name: Light Mode Time
  $description: Custom (HH:mm:ss or HH:mm)
- CustomDark: 19:00:00
  $name: Dark Mode Time
  $description: Custom (HH:mm:ss or HH:mm)
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
- Hotkey: ""
  $name: Hotkey (Toggle Theme)
  $description: >-
    Hotkey in format: Modifier+Key (e.g., Ctrl+Alt+D).
    Modifiers: Alt, Ctrl, Shift, Win.
    Keys: A-Z, 0-9, F1-F24, Enter, Space, Home, End, Insert, Delete, etc.
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
    Custom,
    LocationService,
    CustomCoordinates,
    Hotkey,
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
    std::wstring hotkeyString;
    std::wstring scriptPath;
    bool lockScreen;
} g_settings;

HANDLE g_timer = nullptr;
HANDLE g_timerThread = nullptr;
HANDLE g_wakeEvent = nullptr;
bool g_exitFlag = false;
HPOWERNOTIFY g_hPowerNotify = nullptr;

static const int kToggleHotkeyId = 1774952533;
static bool g_hotkeyRegistered = false;

enum Appearance {
    light,
    dark
};

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

time_t UpdateThemeState(bool applyNow) {
    time_t now = time(nullptr);
    struct tm now_tm;
    localtime_s(&now_tm, &now);

    if (g_settings.scheduleMode == ScheduleMode::Hotkey) {
        return 0;
    }

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
                    if (applyNow) {
                        Wh_Log(L"Location fetch failed, using coordinates from local storage: %f, %f", lat, lng);
                    }
                } else {
                    if (applyNow) {
                        Wh_Log(L"Location fetch failed and no cache found, using settings coordinates.");
                    }
                    lat = g_settings.latitude;
                    lng = g_settings.longitude;
                }
            }
        }

        GetSunriseSunsetTimes(lat, lng, lightST, darkST);

        if (locSuccess || g_settings.scheduleMode == ScheduleMode::CustomCoordinates) {
            PCWSTR sourceStr = (g_settings.scheduleMode == ScheduleMode::CustomCoordinates) ? L"CustomCoordinates" : (locSuccess ? L"LocationService (Fresh)" : L"LocationService (Cached)");
            Wh_Log(L"Active Schedule (%s): %f, %f", sourceStr, lat, lng);
            Wh_Log(L"Active Schedule (%s): Light: %02d:%02d:%02d, Dark: %02d:%02d:%02d", sourceStr, lightST.wHour, lightST.wMinute, lightST.wSecond, darkST.wHour, darkST.wMinute, darkST.wSecond);
        }

        if (g_settings.scheduleMode == ScheduleMode::LocationService && !locSuccess) {
            if (applyNow) {
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

    if (applyNow) {
        Apply(isLightNow, g_settings.switchMode, g_settings.lightWallpaperPath, g_settings.darkWallpaperPath, g_settings.lightThemePath, g_settings.darkThemePath, g_settings.scriptPath, g_settings.lockScreen);
    }

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

// Based on:
// https://windhawk.net/mods/keyboard-shortcut-actions
// Licensed under GNU General Public License v3.0
bool FromStringHotKey(std::wstring_view hotkeyString,
                      UINT* modifiersOut,
                      UINT* vkOut) {
    static const std::unordered_map<std::wstring_view, UINT> modifiersMap = {
        {L"ALT", MOD_ALT},           {L"CTRL", MOD_CONTROL},
        {L"NOREPEAT", MOD_NOREPEAT}, {L"SHIFT", MOD_SHIFT},
        {L"WIN", MOD_WIN},
    };

    static const std::unordered_map<std::wstring_view, UINT> vkMap = {
        // Letters A-Z
        {L"A", 0x41},
        {L"B", 0x42},
        {L"C", 0x43},
        {L"D", 0x44},
        {L"E", 0x45},
        {L"F", 0x46},
        {L"G", 0x47},
        {L"H", 0x48},
        {L"I", 0x49},
        {L"J", 0x4A},
        {L"K", 0x4B},
        {L"L", 0x4C},
        {L"M", 0x4D},
        {L"N", 0x4E},
        {L"O", 0x4F},
        {L"P", 0x50},
        {L"Q", 0x51},
        {L"R", 0x52},
        {L"S", 0x53},
        {L"T", 0x54},
        {L"U", 0x55},
        {L"V", 0x56},
        {L"W", 0x57},
        {L"X", 0x58},
        {L"Y", 0x59},
        {L"Z", 0x5A},
        // Numbers 0-9
        {L"0", 0x30},
        {L"1", 0x31},
        {L"2", 0x32},
        {L"3", 0x33},
        {L"4", 0x34},
        {L"5", 0x35},
        {L"6", 0x36},
        {L"7", 0x37},
        {L"8", 0x38},
        {L"9", 0x39},
        // Function keys F1-F24
        {L"F1", 0x70},
        {L"F2", 0x71},
        {L"F3", 0x72},
        {L"F4", 0x73},
        {L"F5", 0x74},
        {L"F6", 0x75},
        {L"F7", 0x76},
        {L"F8", 0x77},
        {L"F9", 0x78},
        {L"F10", 0x79},
        {L"F11", 0x7A},
        {L"F12", 0x7B},
        {L"F13", 0x7C},
        {L"F14", 0x7D},
        {L"F15", 0x7E},
        {L"F16", 0x7F},
        {L"F17", 0x80},
        {L"F18", 0x81},
        {L"F19", 0x82},
        {L"F20", 0x83},
        {L"F21", 0x84},
        {L"F22", 0x85},
        {L"F23", 0x86},
        {L"F24", 0x87},
        // Common keys (friendly names)
        {L"BACKSPACE", 0x08},
        {L"TAB", 0x09},
        {L"ENTER", 0x0D},
        {L"RETURN", 0x0D},
        {L"PAUSE", 0x13},
        {L"CAPSLOCK", 0x14},
        {L"ESCAPE", 0x1B},
        {L"ESC", 0x1B},
        {L"SPACE", 0x20},
        {L"SPACEBAR", 0x20},
        {L"PAGEUP", 0x21},
        {L"PAGEDOWN", 0x22},
        {L"END", 0x23},
        {L"HOME", 0x24},
        {L"LEFT", 0x25},
        {L"UP", 0x26},
        {L"RIGHT", 0x27},
        {L"DOWN", 0x28},
        {L"PRINTSCREEN", 0x2C},
        {L"PRTSC", 0x2C},
        {L"INSERT", 0x2D},
        {L"INS", 0x2D},
        {L"DELETE", 0x2E},
        {L"DEL", 0x2E},
        {L"HELP", 0x2F},
        {L"SLEEP", 0x5F},
        {L"APPS", 0x5D},
        {L"MENU", 0x5D},
        // Numpad keys
        {L"NUMPAD0", 0x60},
        {L"NUMPAD1", 0x61},
        {L"NUMPAD2", 0x62},
        {L"NUMPAD3", 0x63},
        {L"NUMPAD4", 0x64},
        {L"NUMPAD5", 0x65},
        {L"NUMPAD6", 0x66},
        {L"NUMPAD7", 0x67},
        {L"NUMPAD8", 0x68},
        {L"NUMPAD9", 0x69},
        {L"NUM0", 0x60},
        {L"NUM1", 0x61},
        {L"NUM2", 0x62},
        {L"NUM3", 0x63},
        {L"NUM4", 0x64},
        {L"NUM5", 0x65},
        {L"NUM6", 0x66},
        {L"NUM7", 0x67},
        {L"NUM8", 0x68},
        {L"NUM9", 0x69},
        {L"MULTIPLY", 0x6A},
        {L"ADD", 0x6B},
        {L"SUBTRACT", 0x6D},
        {L"DECIMAL", 0x6E},
        {L"DIVIDE", 0x6F},
        {L"NUMLOCK", 0x90},
        {L"SCROLLLOCK", 0x91},
        // Media keys
        {L"VOLUMEMUTE", 0xAD},
        {L"VOLUMEDOWN", 0xAE},
        {L"VOLUMEUP", 0xAF},
        {L"MEDIANEXT", 0xB0},
        {L"MEDIAPREV", 0xB1},
        {L"MEDIASTOP", 0xB2},
        {L"MEDIAPLAYPAUSE", 0xB3},
        // Browser keys
        {L"BROWSERBACK", 0xA6},
        {L"BROWSERFORWARD", 0xA7},
        {L"BROWSERREFRESH", 0xA8},
        {L"BROWSERSTOP", 0xA9},
        {L"BROWSERSEARCH", 0xAA},
        {L"BROWSERFAVORITES", 0xAB},
        {L"BROWSERHOME", 0xAC},
        // Modifier keys (for use in Virtual key press action)
        {L"LWIN", 0x5B},
        {L"RWIN", 0x5C},
        {L"LSHIFT", 0xA0},
        {L"RSHIFT", 0xA1},
        {L"LCTRL", 0xA2},
        {L"RCTRL", 0xA3},
        {L"LALT", 0xA4},
        {L"RALT", 0xA5},
        // VK_ prefixed versions (only keys without friendly aliases)
        {L"VK_LBUTTON", 0x01},
        {L"VK_RBUTTON", 0x02},
        {L"VK_CANCEL", 0x03},
        {L"VK_MBUTTON", 0x04},
        {L"VK_XBUTTON1", 0x05},
        {L"VK_XBUTTON2", 0x06},
        {L"VK_CLEAR", 0x0C},
        {L"VK_SHIFT", 0x10},
        {L"VK_CONTROL", 0x11},
        {L"VK_MENU", 0x12},
        {L"VK_KANA", 0x15},
        {L"VK_HANGUL", 0x15},
        {L"VK_IME_ON", 0x16},
        {L"VK_JUNJA", 0x17},
        {L"VK_FINAL", 0x18},
        {L"VK_HANJA", 0x19},
        {L"VK_KANJI", 0x19},
        {L"VK_IME_OFF", 0x1A},
        {L"VK_CONVERT", 0x1C},
        {L"VK_NONCONVERT", 0x1D},
        {L"VK_ACCEPT", 0x1E},
        {L"VK_MODECHANGE", 0x1F},
        {L"VK_SELECT", 0x29},
        {L"VK_PRINT", 0x2A},
        {L"VK_EXECUTE", 0x2B},
        {L"VK_SEPARATOR", 0x6C},
        {L"VK_LAUNCH_MAIL", 0xB4},
        {L"VK_LAUNCH_MEDIA_SELECT", 0xB5},
        {L"VK_LAUNCH_APP1", 0xB6},
        {L"VK_LAUNCH_APP2", 0xB7},
        {L"VK_OEM_1", 0xBA},
        {L"VK_OEM_PLUS", 0xBB},
        {L"VK_OEM_COMMA", 0xBC},
        {L"VK_OEM_MINUS", 0xBD},
        {L"VK_OEM_PERIOD", 0xBE},
        {L"VK_OEM_2", 0xBF},
        {L"VK_OEM_3", 0xC0},
        {L"VK_OEM_4", 0xDB},
        {L"VK_OEM_5", 0xDC},
        {L"VK_OEM_6", 0xDD},
        {L"VK_OEM_7", 0xDE},
        {L"VK_OEM_8", 0xDF},
        {L"VK_OEM_102", 0xE2},
        {L"VK_PROCESSKEY", 0xE5},
        {L"VK_PACKET", 0xE7},
        {L"VK_ATTN", 0xF6},
        {L"VK_CRSEL", 0xF7},
        {L"VK_EXSEL", 0xF8},
        {L"VK_EREOF", 0xF9},
        {L"VK_PLAY", 0xFA},
        {L"VK_ZOOM", 0xFB},
        {L"VK_NONAME", 0xFC},
        {L"VK_PA1", 0xFD},
        {L"VK_OEM_CLEAR", 0xFE},
    };

    auto splitStringView = [](std::wstring_view s, WCHAR delimiter) {
        size_t pos_start = 0, pos_end;
        std::wstring_view token;
        std::vector<std::wstring_view> res;

        while ((pos_end = s.find(delimiter, pos_start)) !=
               std::wstring_view::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + 1;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    };

    auto trimStringView = [](std::wstring_view s) {
        s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
        s.remove_suffix(std::min(
            s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
        return s;
    };

    UINT modifiers = 0;
    UINT vk = 0;

    auto hotkeyParts = splitStringView(hotkeyString, '+');
    for (auto hotkeyPart : hotkeyParts) {
        hotkeyPart = trimStringView(hotkeyPart);
        std::wstring hotkeyPartUpper{hotkeyPart};
        std::transform(hotkeyPartUpper.begin(), hotkeyPartUpper.end(),
                       hotkeyPartUpper.begin(), ::toupper);

        if (auto it = modifiersMap.find(hotkeyPartUpper);
            it != modifiersMap.end()) {
            modifiers |= it->second;
            continue;
        }

        if (vk) {
            // Only one key is allowed.
            return false;
        }

        if (auto it = vkMap.find(hotkeyPartUpper); it != vkMap.end()) {
            vk = it->second;
            continue;
        }

        size_t pos;
        try {
            vk = std::stoi(hotkeyPartUpper, &pos, 0);
            if (hotkeyPartUpper[pos] != L'\0' || !vk) {
                return false;
            }
        } catch (const std::exception&) {
            return false;
        }
    }

    if (!vk) {
        return false;
    }

    *modifiersOut = modifiers;
    *vkOut = vk;
    return true;
}

static void RegisterHotkey(HWND hWnd) {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(hWnd, kToggleHotkeyId);
        g_hotkeyRegistered = false;
        Wh_Log(L"Hotkey unregistered.");
    }

    if (g_settings.hotkeyString.empty()) {
        return;
    }

    UINT modifiers = 0;
    UINT vk = 0;
    if (!FromStringHotKey(g_settings.hotkeyString, &modifiers, &vk)) {
        Wh_Log(L"Failed to parse hotkey: %s", g_settings.hotkeyString.c_str());
        return;
    }

    if (!RegisterHotKey(hWnd, kToggleHotkeyId, modifiers, vk)) {
        Wh_Log(L"Failed to register hotkey: %s (error=%d)", g_settings.hotkeyString.c_str(), GetLastError());
        return;
    }

    g_hotkeyRegistered = true;
    Wh_Log(L"Registered hotkey: %s", g_settings.hotkeyString.c_str());
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

    RegisterHotkey(nullptr);

    time_t next = UpdateThemeState(true);
    if (g_exitFlag) {
        if (g_hotkeyRegistered) {
            UnregisterHotKey(nullptr, kToggleHotkeyId);
            g_hotkeyRegistered = false;
            Wh_Log(L"Hotkey unregistered.");
        }
        Wh_Log(L"Theme scheduler stopped.");
        return 0;
    }

    if (next > 0 && g_settings.scheduleMode != ScheduleMode::Hotkey) {
        LARGE_INTEGER due;
        due.QuadPart = (LONGLONG)next * 10000000LL + 116444736000000000LL;
        SetWaitableTimer(g_timer, &due, 0, nullptr, nullptr, FALSE);
        struct tm n_tm;
        localtime_s(&n_tm, &next);
        Wh_Log(L"Waiting until %02d:%02d:%02d for next switch.", n_tm.tm_hour, n_tm.tm_min, n_tm.tm_sec);
    } else {
        if (g_timer) CancelWaitableTimer(g_timer);
    }

    while (!g_exitFlag) {
        const DWORD waitRes = MsgWaitForMultipleObjects(2, handles, FALSE, INFINITE, QS_ALLEVENTS);
        if (g_exitFlag) break;

        if (waitRes == WAIT_OBJECT_0) {
            RegisterHotkey(nullptr);
            next = UpdateThemeState(true);
            if (g_exitFlag) break;
            if (next > 0 && g_settings.scheduleMode != ScheduleMode::Hotkey) {
                LARGE_INTEGER due;
                due.QuadPart =
                    (LONGLONG)next * 10000000LL + 116444736000000000LL;
                SetWaitableTimer(g_timer, &due, 0, nullptr, nullptr, FALSE);
                struct tm n_tm;
                localtime_s(&n_tm, &next);
                Wh_Log(L"Waiting until %02d:%02d:%02d for next switch.", n_tm.tm_hour, n_tm.tm_min, n_tm.tm_sec);
            } else {
                if (g_timer) CancelWaitableTimer(g_timer);
            }
        } else if (waitRes == WAIT_OBJECT_0 + 1) {
            next = UpdateThemeState(true);
            if (g_exitFlag) break;
            if (next > 0 && g_settings.scheduleMode != ScheduleMode::Hotkey) {
                LARGE_INTEGER due;
                due.QuadPart = (LONGLONG)next * 10000000LL + 116444736000000000LL;
                SetWaitableTimer(g_timer, &due, 0, nullptr, nullptr, FALSE);
                struct tm n_tm;
                localtime_s(&n_tm, &next);
                Wh_Log(L"Waiting until %02d:%02d:%02d for next switch.", n_tm.tm_hour, n_tm.tm_min, n_tm.tm_sec);
            } else {
                if (g_timer) CancelWaitableTimer(g_timer);
            }
        } else if (waitRes == WAIT_OBJECT_0 + 2) {
            bool hotkeyPressed = false;
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_HOTKEY &&
                    static_cast<int>(msg.wParam) == kToggleHotkeyId) {
                    Wh_Log(L"Hotkey pressed: %s", g_settings.hotkeyString.c_str());
                    Apply(!IsAppearanceApplied(light), g_settings.switchMode, g_settings.lightWallpaperPath, g_settings.darkWallpaperPath, g_settings.lightThemePath, g_settings.darkThemePath, g_settings.scriptPath, g_settings.lockScreen);
                    hotkeyPressed = true;
                }
            }

            if (hotkeyPressed) {
                next = UpdateThemeState(false);
                if (next > 0 && g_settings.scheduleMode != ScheduleMode::Hotkey) {
                    LARGE_INTEGER due;
                    due.QuadPart = (LONGLONG)next * 10000000LL + 116444736000000000LL;
                    SetWaitableTimer(g_timer, &due, 0, nullptr, nullptr, FALSE);
                    struct tm n_tm;
                    localtime_s(&n_tm, &next);
                    Wh_Log(L"Waiting until %02d:%02d:%02d for next switch.", n_tm.tm_hour, n_tm.tm_min, n_tm.tm_sec);
                } else {
                    if (g_timer) CancelWaitableTimer(g_timer);
                }
            }
        } else if (waitRes == WAIT_FAILED) {
            Wh_Log(L"MsgWaitForMultipleObjects failed (error=%d)", GetLastError());
            break;
        }
    }

    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, kToggleHotkeyId);
        g_hotkeyRegistered = false;
        Wh_Log(L"Hotkey unregistered.");
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

SYSTEMTIME ParseScheduleTime(PCWSTR timeStr) {
    SYSTEMTIME st = {};
    if (timeStr) {
        if (swscanf_s(timeStr, L"%hu:%hu:%hu", &st.wHour, &st.wMinute, &st.wSecond) < 3) {
            st.wSecond = 0;
        }
    }
    return st;
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
    g_settings.hotkeyString = getString(L"Hotkey");
    g_settings.lockScreen = Wh_GetIntSetting(L"LockScreen", 1) != 0;

    PCWSTR scheduleMode = Wh_GetStringSetting(L"ScheduleMode");
    g_settings.scheduleMode = ScheduleMode::Custom;
    if (scheduleMode) {
        if (wcscmp(scheduleMode, L"LocationService") == 0) g_settings.scheduleMode = ScheduleMode::LocationService;
        else if (wcscmp(scheduleMode, L"CustomCoordinates") == 0) g_settings.scheduleMode = ScheduleMode::CustomCoordinates;
        else if (wcscmp(scheduleMode, L"Hotkey") == 0) g_settings.scheduleMode = ScheduleMode::Hotkey;
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

    if (g_settings.scheduleMode == ScheduleMode::Custom) {
        Wh_Log(L"Active Schedule (Custom): Light: %02d:%02d:%02d, Dark: %02d:%02d:%02d", g_settings.lightTime.wHour, g_settings.lightTime.wMinute, g_settings.lightTime.wSecond, g_settings.darkTime.wHour, g_settings.darkTime.wMinute, g_settings.darkTime.wSecond);
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