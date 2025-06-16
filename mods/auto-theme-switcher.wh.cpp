// ==WindhawkMod==
// @id              auto-theme-switcher
// @name            Auto Theme Switcher
// @description     Automatically switch between light and dark appearance/wallpaper/theme based on a set schedule with custom script support
// @version         1.0.4
// @author          tinodin
// @github          https://github.com/tinodin
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Auto Theme Switcher

Configure the schedule for light and dark (By default: `07:00` for light, `19:00` for dark)

Choose between 3 different modes (By default: `Switch between light and dark appearance`)
- Switch between light and dark appearance
- Switch between light and dark appearance + wallpapers
    - Provide paths to wallpapers (By default: `img0.jpg` for light, `img19.jpg` for dark)
- Switch between light and dark themes
    - Provide paths to `.theme` files (By default: `aero.theme` for light, `dark.theme` for dark)

    - Themes shipped with Windows are located in: `C:\Windows\Resources\Themes`

    - Saved or imported themes are located in: `%LOCALAPPDATA%\Microsoft\Windows\Themes`

    - To create custom `.theme` files, use the Personalization settings in Windows [Video guide](https://www.youtube.com/watch?v=-QWR6NQZAUg)

Provide path to custom script
- The script will be executed on every switch with -light/-dark argument

If you do not want the wallpaper to be applied to the lock screen, disable *Apply Wallpaper to Lock screen*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Light: 07:00
  $name: Light mode time
- Dark: 19:00
  $name: Dark mode time
- mode: appearance
  $name: Mode
  $options:
  - appearance: Switch between light and dark appearance
  - wallpaper: Switch between light and dark appearance + wallpapers
  - theme: Switch between light and dark themes
- LightWallpaperPath: "C:\\Windows\\Web\\Wallpaper\\Windows\\img0.jpg"
  $name: Light mode wallpaper path
- DarkWallpaperPath: "C:\\Windows\\Web\\Wallpaper\\Windows\\img19.jpg"
  $name: Dark mode wallpaper path
- LightThemePath: "C:\\Windows\\Resources\\Themes\\aero.theme"
  $name: Light mode theme path
- DarkThemePath: "C:\\Windows\\Resources\\Themes\\dark.theme"
  $name: Dark mode theme path
- ScriptPath: ""
  $name: Custom script path
- LockScreen: true
  $name: Apply Wallpaper to Lock screen
*/
// ==/WindhawkModSettings==

#include <fstream>
#include <comdef.h>
#include <winrt/base.h>

HANDLE g_timer = nullptr;
HANDLE g_timerThread = nullptr;
HANDLE g_wakeEvent = nullptr;
bool g_exitFlag = false;
SYSTEMTIME g_lightTime, g_darkTime;
std::wstring g_mode;
std::wstring g_lightWallpaperPath, g_darkWallpaperPath;
std::wstring g_lightThemePath, g_darkThemePath;
std::wstring g_scriptPath;

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
        } else {
            Wh_Log(L"Successfully launched script: %s", args.c_str());
        }
    }
    else
    {
        if ((INT_PTR)ShellExecuteW(nullptr, L"open", g_scriptPath.c_str(), params.c_str(), nullptr, SW_HIDE) <= 32) {
            Wh_Log(L"Failed to launch script");
        } else {
            Wh_Log(L"Successfully launched script: %s %s", g_scriptPath.c_str(), params.c_str());
        }
    }
}

void ApplyLockScreen() {
    wchar_t currentWallpaper[MAX_PATH] = {0};
    DWORD size = sizeof(currentWallpaper);

    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallPaper", RRF_RT_REG_SZ, nullptr, currentWallpaper, &size) != ERROR_SUCCESS) {
        Wh_Log(L"Failed to apply lock Screen");
        return;
    }

    std::wstring wallpaperPath = currentWallpaper;
    HKEY hKey;

    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\Personalization", 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        if (RegSetValueExW(hKey, L"LockScreenImage", 0, REG_SZ, (const BYTE*)wallpaperPath.c_str(), (DWORD)((wallpaperPath.size() + 1) * sizeof(wchar_t))) != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            Wh_Log(L"Failed to apply lock Screen");
            return;
        }
        RegCloseKey(hKey);
    } else {
        Wh_Log(L"Failed to apply lock Screen");
        return;
    }

    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\PersonalizationCSP", 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        for (PCWSTR valueName : { L"LockScreenImagePath", L"LockScreenImageUrl" }) {
            if (RegSetValueExW(hKey, valueName, 0, REG_SZ, (const BYTE*)wallpaperPath.c_str(), (DWORD)((wallpaperPath.size() + 1) * sizeof(wchar_t))) != ERROR_SUCCESS) {
                RegCloseKey(hKey);
                Wh_Log(L"Failed to apply lock Screen");
                return;
            }
        }
        RegCloseKey(hKey);
    } else {
        Wh_Log(L"Failed to apply lock Screen");
        return;
    }

    Wh_Log(L"Successfully applied lock Screen");
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
    }

    Wh_Log(L"Successfully applied theme");

    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 100, nullptr);
}

void Apply(bool useLightTheme) {
    PCWSTR wallpaperPath = useLightTheme ? g_lightWallpaperPath.c_str() : g_darkWallpaperPath.c_str();
    PCWSTR themePath = useLightTheme ? g_lightThemePath.c_str() : g_darkThemePath.c_str();
    bool changed = false;

    if (g_mode == L"theme") {
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
        if (g_mode == L"wallpaper" && *wallpaperPath) {
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

std::wstring TrimQuotes(const std::wstring& str) {
    size_t start = 0;
    size_t end = str.length();
    if (!str.empty() && str.front() == L'"') start = 1;
    if (end > start && str[end - 1] == L'"') end--;
    return str.substr(start, end - start);
}

void LoadSettings() {
    auto rawLight = Wh_GetStringSetting(L"Light");
    g_lightTime = ParseScheduleTime(rawLight);
    if (rawLight) Wh_FreeStringSetting(rawLight);

    auto rawDark = Wh_GetStringSetting(L"Dark");
    g_darkTime = ParseScheduleTime(rawDark);
    if (rawDark) Wh_FreeStringSetting(rawDark);

    auto rawMode = Wh_GetStringSetting(L"mode");
    g_mode = rawMode ? std::wstring(rawMode) : L"appearance";
    if (rawMode) Wh_FreeStringSetting(rawMode);

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
