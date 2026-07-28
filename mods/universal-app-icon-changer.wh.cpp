// ==WindhawkMod==
// @id              universal-app-icon-changer
// @name            Universal App Icon Changer
// @description     Change runtime window, taskbar and Alt+Tab icons for selected applications
// @version         1.0.0
// @author          Parwin
// @github          https://github.com/parwin889-ui
// @include         *
// @exclude         Windhawk.exe
// @exclude         WindhawkUI.exe
// @exclude         WindhawkEngine.exe
// @exclude         WindhawkCompiler.exe
// @exclude         dwm.exe
// @exclude         csrss.exe
// @exclude         winlogon.exe
// @exclude         services.exe
// @exclude         lsass.exe
// @exclude         smss.exe
// @exclude         svchost.exe
// @exclude         conhost.exe
// @license         MIT
// @compilerOptions -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Universal App Icon Changer

Change runtime icons for selected applications.

## What it can change

- Window title bar icon
- Taskbar running window icon
- Alt + Tab window icon
- Some dock tools that read the running window icon

## What it does not permanently change

- The exe file icon itself
- Desktop shortcut icon
- Start menu pinned icon
- Some self-drawn app icons, such as certain Chromium, Electron, UWP or Store apps

## Usage

1. Prepare icon files, for example:

   D:\AppIcons\chrome.ico  
   D:\AppIcons\notepad.ico  
   D:\AppIcons\vscode.ico

2. Open this mod's settings.

3. Edit Rules:

   Process: chrome.exe  
   IconPath: D:\AppIcons\chrome.ico

4. Restart the target application.

## IconPath examples

Use an ico file:

D:\AppIcons\chrome.ico

Use an exe icon:

C:\Program Files\Google\Chrome\Application\chrome.exe,0

Use an environment variable:

%USERPROFILE%\Pictures\Icons\chrome.ico

Use IconDirectory:

IconDirectory: D:\AppIcons  
IconPath: chrome.ico
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Enabled: true
  $name: Enabled
  $description: Enable or disable the mod.

- ApplyIntervalMs: 1000
  $name: Apply interval
  $description: How often icons are re-applied to existing windows, in milliseconds. Recommended value is 1000.

- BigIconSize: 32
  $name: Big icon size
  $description: Big icon size. Use 32 or 48. Use 0 for the system default.

- SmallIconSize: 16
  $name: Small icon size
  $description: Small icon size. Use 16 or 24. Use 0 for the system default.

- OnlyTopLevelWindows: true
  $name: Only top-level windows
  $description: Recommended. Apply only to top-level windows.

- ChangeClassIcon: true
  $name: Change class icon
  $description: Also change the window class icon when possible.

- IconDirectory: ""
  $name: Icon directory
  $description: Optional base folder for relative icon paths, for example D:\AppIcons.

- DefaultIconPath: ""
  $name: Default icon path
  $description: Optional. If set, unmatched processes will use this icon. Leave empty for safer behavior.

- Rules:
    - - Process: notepad.exe
      - IconPath: ""
    - - Process: chrome.exe
      - IconPath: ""
    - - Process: msedge.exe
      - IconPath: ""
    - - Process: Code.exe
      - IconPath: ""
    - - Process: explorer.exe
      - IconPath: ""
    - - Process: Cursor.exe
      - IconPath: ""
    - - Process: Discord.exe
      - IconPath: ""
    - - Process: Spotify.exe
      - IconPath: ""
    - - Process: Telegram.exe
      - IconPath: ""
    - - Process: WeChat.exe
      - IconPath: ""
    - - Process: QQ.exe
      - IconPath: ""
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <cwchar>
#include <cwctype>

#ifndef ICON_SMALL2
#define ICON_SMALL2 2
#endif

using CreateWindowExW_t = HWND(WINAPI*)(
    DWORD,
    LPCWSTR,
    LPCWSTR,
    DWORD,
    int,
    int,
    int,
    int,
    HWND,
    HMENU,
    HINSTANCE,
    LPVOID
);

using CreateWindowExA_t = HWND(WINAPI*)(
    DWORD,
    LPCSTR,
    LPCSTR,
    DWORD,
    int,
    int,
    int,
    int,
    HWND,
    HMENU,
    HINSTANCE,
    LPVOID
);

struct Settings {
    bool enabled;
    int applyIntervalMs;
    int bigIconSize;
    int smallIconSize;
    bool onlyTopLevelWindows;
    bool changeClassIcon;
    std::wstring iconDirectory;
    std::wstring defaultIconPath;
};

struct Rule {
    std::wstring process;
    std::wstring iconPath;
};

struct IconSet {
    HICON big;
    HICON small;
};

CreateWindowExW_t CreateWindowExW_Original = nullptr;
CreateWindowExA_t CreateWindowExA_Original = nullptr;

Settings g_settings = {};
std::vector<Rule> g_rules;
std::wstring g_processPath;
std::wstring g_processFile;
std::wstring g_selectedIconSpec;
IconSet g_icons = {};
HANDLE g_stopEvent = nullptr;
HANDLE g_workerThread = nullptr;
bool g_active = false;
bool g_hooksInstalled = false;
std::vector<HICON> g_ownedIcons;

std::wstring Trim(const std::wstring& value) {
    size_t start = 0;
    size_t end = value.size();

    while (start < end && iswspace(value[start])) {
        start++;
    }

    while (end > start && iswspace(value[end - 1])) {
        end--;
    }

    return value.substr(start, end - start);
}

std::wstring GetStringSettingValue(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result;

    if (value) {
        result = value;
        Wh_FreeStringSetting(value);
    }

    return result;
}

std::wstring GetStringSettingValue(PCWSTR format, int index) {
    PCWSTR value = Wh_GetStringSetting(format, index);
    std::wstring result;

    if (value) {
        result = value;
        Wh_FreeStringSetting(value);
    }

    return result;
}

std::wstring ExpandEnvironment(const std::wstring& value) {
    if (value.empty()) {
        return value;
    }

    DWORD required = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);

    if (required == 0) {
        return value;
    }

    std::wstring buffer;
    buffer.resize(required);

    DWORD written = ExpandEnvironmentStringsW(value.c_str(), buffer.data(), required);

    if (written == 0 || written > required) {
        return value;
    }

    if (!buffer.empty() && buffer.back() == L'\0') {
        buffer.pop_back();
    }

    return buffer;
}

bool IsAbsolutePath(const std::wstring& path) {
    if (path.size() >= 3 && iswalpha(path[0]) && path[1] == L':' && (path[2] == L'\\' || path[2] == L'/')) {
        return true;
    }

    if (path.size() >= 2 && path[0] == L'\\' && path[1] == L'\\') {
        return true;
    }

    return false;
}

std::wstring JoinPath(const std::wstring& directory, const std::wstring& file) {
    if (directory.empty()) {
        return file;
    }

    if (file.empty()) {
        return directory;
    }

    wchar_t last = directory.back();

    if (last == L'\\' || last == L'/') {
        return directory + file;
    }

    return directory + L"\\" + file;
}

std::wstring GetBaseName(const std::wstring& path) {
    size_t pos1 = path.find_last_of(L'\\');
    size_t pos2 = path.find_last_of(L'/');
    size_t pos = std::wstring::npos;

    if (pos1 != std::wstring::npos && pos2 != std::wstring::npos) {
        pos = pos1 > pos2 ? pos1 : pos2;
    } else if (pos1 != std::wstring::npos) {
        pos = pos1;
    } else {
        pos = pos2;
    }

    if (pos == std::wstring::npos) {
        return path;
    }

    return path.substr(pos + 1);
}

std::wstring GetExtensionLower(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    size_t dot = path.find_last_of(L'.');

    if (dot == std::wstring::npos) {
        return L"";
    }

    if (slash != std::wstring::npos && dot < slash) {
        return L"";
    }

    std::wstring ext = path.substr(dot);

    for (wchar_t& ch : ext) {
        ch = static_cast<wchar_t>(towlower(ch));
    }

    return ext;
}

bool EqualsIgnoreCase(const std::wstring& a, const std::wstring& b) {
    return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

bool HasWildcard(const std::wstring& value) {
    return value.find(L'*') != std::wstring::npos || value.find(L'?') != std::wstring::npos;
}

bool WildcardMatchIgnoreCase(const wchar_t* pattern, const wchar_t* text) {
    while (*pattern) {
        if (*pattern == L'*') {
            pattern++;

            if (!*pattern) {
                return true;
            }

            while (*text) {
                if (WildcardMatchIgnoreCase(pattern, text)) {
                    return true;
                }

                text++;
            }

            return false;
        }

        if (*pattern == L'?') {
            if (!*text) {
                return false;
            }

            pattern++;
            text++;
            continue;
        }

        if (towlower(*pattern) != towlower(*text)) {
            return false;
        }

        pattern++;
        text++;
    }

    return *text == L'\0';
}

bool ProcessPatternMatches(const std::wstring& pattern, const std::wstring& processFile, const std::wstring& processPath) {
    std::wstring p = Trim(ExpandEnvironment(pattern));

    if (p.empty()) {
        return false;
    }

    if (HasWildcard(p)) {
        return WildcardMatchIgnoreCase(p.c_str(), processFile.c_str()) || WildcardMatchIgnoreCase(p.c_str(), processPath.c_str());
    }

    return EqualsIgnoreCase(p, processFile) || EqualsIgnoreCase(p, processPath);
}

void LoadProcessIdentity() {
    wchar_t path[32768] = {};
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));

    if (length == 0) {
        g_processPath.clear();
        g_processFile.clear();
        return;
    }

    g_processPath = path;
    g_processFile = GetBaseName(g_processPath);
}

void LoadSettings() {
    g_settings.enabled = Wh_GetIntSetting(L"Enabled") != 0;

    g_settings.applyIntervalMs = Wh_GetIntSetting(L"ApplyIntervalMs");

    if (g_settings.applyIntervalMs < 250) {
        g_settings.applyIntervalMs = 1000;
    }

    if (g_settings.applyIntervalMs > 30000) {
        g_settings.applyIntervalMs = 30000;
    }

    g_settings.bigIconSize = Wh_GetIntSetting(L"BigIconSize");
    g_settings.smallIconSize = Wh_GetIntSetting(L"SmallIconSize");

    if (g_settings.bigIconSize < 0) {
        g_settings.bigIconSize = 0;
    }

    if (g_settings.smallIconSize < 0) {
        g_settings.smallIconSize = 0;
    }

    g_settings.onlyTopLevelWindows = Wh_GetIntSetting(L"OnlyTopLevelWindows") != 0;
    g_settings.changeClassIcon = Wh_GetIntSetting(L"ChangeClassIcon") != 0;
    g_settings.iconDirectory = Trim(ExpandEnvironment(GetStringSettingValue(L"IconDirectory")));
    g_settings.defaultIconPath = Trim(GetStringSettingValue(L"DefaultIconPath"));

    g_rules.clear();

    for (int i = 0; i < 200; i++) {
        std::wstring process = Trim(GetStringSettingValue(L"Rules[%d].Process", i));
        std::wstring iconPath = Trim(GetStringSettingValue(L"Rules[%d].IconPath", i));

        if (process.empty() && iconPath.empty()) {
            if (i >= 20) {
                break;
            }

            continue;
        }

        if (process.empty()) {
            continue;
        }

        Rule rule;
        rule.process = process;
        rule.iconPath = iconPath;
        g_rules.push_back(rule);
    }
}

std::wstring ResolveIconSpec(const std::wstring& spec) {
    std::wstring value = Trim(ExpandEnvironment(spec));

    if (value.empty()) {
        return L"";
    }

    size_t comma = value.find_last_of(L',');

    std::wstring pathPart = value;
    std::wstring indexPart;

    if (comma != std::wstring::npos) {
        std::wstring maybeIndex = Trim(value.substr(comma + 1));
        bool numeric = !maybeIndex.empty();

        size_t start = 0;

        if (!maybeIndex.empty() && (maybeIndex[0] == L'-' || maybeIndex[0] == L'+')) {
            start = 1;
        }

        for (size_t i = start; i < maybeIndex.size(); i++) {
            if (!iswdigit(maybeIndex[i])) {
                numeric = false;
                break;
            }
        }

        if (numeric) {
            pathPart = Trim(value.substr(0, comma));
            indexPart = maybeIndex;
        }
    }

    if (!IsAbsolutePath(pathPart) && !g_settings.iconDirectory.empty()) {
        pathPart = JoinPath(g_settings.iconDirectory, pathPart);
    }

    pathPart = ExpandEnvironment(pathPart);

    if (!indexPart.empty()) {
        return pathPart + L"," + indexPart;
    }

    return pathPart;
}

bool SelectIconForCurrentProcess() {
    g_selectedIconSpec.clear();

    for (const Rule& rule : g_rules) {
        if (!ProcessPatternMatches(rule.process, g_processFile, g_processPath)) {
            continue;
        }

        std::wstring resolved = ResolveIconSpec(rule.iconPath);

        if (resolved.empty()) {
            Wh_Log(L"Matched process %s, but IconPath is empty", g_processFile.c_str());
            return false;
        }

        g_selectedIconSpec = resolved;
        return true;
    }

    std::wstring resolvedDefault = ResolveIconSpec(g_settings.defaultIconPath);

    if (!resolvedDefault.empty()) {
        g_selectedIconSpec = resolvedDefault;
        return true;
    }

    return false;
}

bool ParseIconSpec(const std::wstring& spec, std::wstring& path, int& index) {
    path = spec;
    index = 0;

    size_t comma = spec.find_last_of(L',');

    if (comma == std::wstring::npos) {
        return true;
    }

    std::wstring maybeIndex = Trim(spec.substr(comma + 1));

    if (maybeIndex.empty()) {
        return true;
    }

    bool numeric = true;
    size_t start = 0;

    if (maybeIndex[0] == L'-' || maybeIndex[0] == L'+') {
        start = 1;
    }

    if (start >= maybeIndex.size()) {
        numeric = false;
    }

    for (size_t i = start; i < maybeIndex.size(); i++) {
        if (!iswdigit(maybeIndex[i])) {
            numeric = false;
            break;
        }
    }

    if (!numeric) {
        return true;
    }

    path = Trim(spec.substr(0, comma));
    index = _wtoi(maybeIndex.c_str());
    return true;
}

HICON ResizeIconIfNeeded(HICON icon, int width, int height) {
    if (!icon) {
        return nullptr;
    }

    if (width <= 0 || height <= 0) {
        return icon;
    }

    HICON resized = reinterpret_cast<HICON>(
        CopyImage(icon, IMAGE_ICON, width, height, LR_COPYRETURNORG)
    );

    if (!resized) {
        return icon;
    }

    return resized;
}

bool LoadIconFromIco(const std::wstring& path, IconSet& icons) {
    int bigSize = g_settings.bigIconSize > 0 ? g_settings.bigIconSize : GetSystemMetrics(SM_CXICON);
    int smallSize = g_settings.smallIconSize > 0 ? g_settings.smallIconSize : GetSystemMetrics(SM_CXSMICON);

    HICON big = reinterpret_cast<HICON>(
        LoadImageW(nullptr, path.c_str(), IMAGE_ICON, bigSize, bigSize, LR_LOADFROMFILE | LR_DEFAULTCOLOR)
    );

    HICON small = reinterpret_cast<HICON>(
        LoadImageW(nullptr, path.c_str(), IMAGE_ICON, smallSize, smallSize, LR_LOADFROMFILE | LR_DEFAULTCOLOR)
    );

    if (!big && !small) {
        return false;
    }

    if (!big) {
        big = small;
    }

    if (!small) {
        small = big;
    }

    icons.big = big;
    icons.small = small;

    if (big) {
        g_ownedIcons.push_back(big);
    }

    if (small && small != big) {
        g_ownedIcons.push_back(small);
    }

    return true;
}

bool LoadIconFromModuleFile(const std::wstring& path, int index, IconSet& icons) {
    HICON big = nullptr;
    HICON small = nullptr;

    UINT count = ExtractIconExW(path.c_str(), index, &big, &small, 1);

    if (count == 0 || (!big && !small)) {
        return false;
    }

    if (!big) {
        big = small;
    }

    if (!small) {
        small = big;
    }

    int bigSize = g_settings.bigIconSize > 0 ? g_settings.bigIconSize : 0;
    int smallSize = g_settings.smallIconSize > 0 ? g_settings.smallIconSize : 0;

    if (bigSize > 0 && big) {
        HICON resizedBig = ResizeIconIfNeeded(big, bigSize, bigSize);

        if (resizedBig && resizedBig != big) {
            DestroyIcon(big);
            big = resizedBig;
        }
    }

    if (smallSize > 0 && small) {
        HICON resizedSmall = ResizeIconIfNeeded(small, smallSize, smallSize);

        if (resizedSmall && resizedSmall != small) {
            if (small != big) {
                DestroyIcon(small);
            }

            small = resizedSmall;
        }
    }

    icons.big = big;
    icons.small = small;

    if (big) {
        g_ownedIcons.push_back(big);
    }

    if (small && small != big) {
        g_ownedIcons.push_back(small);
    }

    return true;
}

bool LoadConfiguredIcons() {
    g_icons = {};

    std::wstring iconPath;
    int iconIndex = 0;

    if (!ParseIconSpec(g_selectedIconSpec, iconPath, iconIndex)) {
        return false;
    }

    iconPath = Trim(ExpandEnvironment(iconPath));

    if (iconPath.empty()) {
        return false;
    }

    std::wstring extension = GetExtensionLower(iconPath);

    IconSet icons = {};

    bool loaded = false;

    if (extension == L".ico") {
        loaded = LoadIconFromIco(iconPath, icons);
    } else {
        loaded = LoadIconFromModuleFile(iconPath, iconIndex, icons);
    }

    if (!loaded) {
        Wh_Log(L"Failed to load icon for %s from %s", g_processFile.c_str(), g_selectedIconSpec.c_str());
        return false;
    }

    g_icons = icons;
    return true;
}

bool ShouldApplyToWindow(HWND hwnd) {
    if (!g_active) {
        return false;
    }

    if (!IsWindow(hwnd)) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    if (processId != GetCurrentProcessId()) {
        return false;
    }

    if (g_settings.onlyTopLevelWindows) {
        if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
            return false;
        }
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);

    if (!(style & WS_CAPTION)) {
        return false;
    }

    if (!IsWindowVisible(hwnd)) {
        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

        if (exStyle & WS_EX_TOOLWINDOW) {
            return false;
        }
    }

    return true;
}

void ApplyIconToWindow(HWND hwnd) {
    if (!ShouldApplyToWindow(hwnd)) {
        return;
    }

    if (g_icons.big) {
        SendMessageTimeoutW(
            hwnd,
            WM_SETICON,
            ICON_BIG,
            reinterpret_cast<LPARAM>(g_icons.big),
            SMTO_ABORTIFHUNG,
            100,
            nullptr
        );

        if (g_settings.changeClassIcon) {
            SetClassLongPtrW(hwnd, GCLP_HICON, reinterpret_cast<LONG_PTR>(g_icons.big));
        }
    }

    if (g_icons.small) {
        SendMessageTimeoutW(
            hwnd,
            WM_SETICON,
            ICON_SMALL,
            reinterpret_cast<LPARAM>(g_icons.small),
            SMTO_ABORTIFHUNG,
            100,
            nullptr
        );

        SendMessageTimeoutW(
            hwnd,
            WM_SETICON,
            ICON_SMALL2,
            reinterpret_cast<LPARAM>(g_icons.small),
            SMTO_ABORTIFHUNG,
            100,
            nullptr
        );

        if (g_settings.changeClassIcon) {
            SetClassLongPtrW(hwnd, GCLP_HICONSM, reinterpret_cast<LONG_PTR>(g_icons.small));
        }
    }

    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM) {
    ApplyIconToWindow(hwnd);
    return TRUE;
}

void ApplyIconToAllWindows() {
    if (!g_active) {
        return;
    }

    EnumWindows(EnumWindowsProc, 0);
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    while (WaitForSingleObject(g_stopEvent, g_settings.applyIntervalMs) == WAIT_TIMEOUT) {
        ApplyIconToAllWindows();
    }

    return 0;
}

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
) {
    HWND hwnd = CreateWindowExW_Original(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam
    );

    if (hwnd) {
        ApplyIconToWindow(hwnd);
    }

    return hwnd;
}

HWND WINAPI CreateWindowExA_Hook(
    DWORD dwExStyle,
    LPCSTR lpClassName,
    LPCSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
) {
    HWND hwnd = CreateWindowExA_Original(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam
    );

    if (hwnd) {
        ApplyIconToWindow(hwnd);
    }

    return hwnd;
}

bool InstallHooks() {
    if (g_hooksInstalled) {
        return true;
    }

    HMODULE user32 = GetModuleHandleW(L"user32.dll");

    if (!user32) {
        return false;
    }

    void* createWindowExW = reinterpret_cast<void*>(GetProcAddress(user32, "CreateWindowExW"));
    void* createWindowExA = reinterpret_cast<void*>(GetProcAddress(user32, "CreateWindowExA"));

    bool installedAny = false;

    if (createWindowExW) {
        if (Wh_SetFunctionHook(
            createWindowExW,
            reinterpret_cast<void*>(CreateWindowExW_Hook),
            reinterpret_cast<void**>(&CreateWindowExW_Original)
        )) {
            installedAny = true;
        }
    }

    if (createWindowExA) {
        if (Wh_SetFunctionHook(
            createWindowExA,
            reinterpret_cast<void*>(CreateWindowExA_Hook),
            reinterpret_cast<void**>(&CreateWindowExA_Original)
        )) {
            installedAny = true;
        }
    }

    g_hooksInstalled = installedAny;
    return installedAny;
}

void StartWorkerThread() {
    if (g_workerThread) {
        return;
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_stopEvent) {
        return;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
}

void StopWorkerThread() {
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

bool ReloadConfiguration() {
    g_active = false;
    g_selectedIconSpec.clear();
    g_icons = {};

    LoadSettings();

    if (!g_settings.enabled) {
        Wh_Log(L"Mod disabled for %s", g_processFile.c_str());
        return false;
    }

    if (!SelectIconForCurrentProcess()) {
        Wh_Log(L"No matching icon rule for %s", g_processFile.c_str());
        return false;
    }

    if (!LoadConfiguredIcons()) {
        return false;
    }

    g_active = true;
    ApplyIconToAllWindows();

    Wh_Log(L"Icon rule active for %s: %s", g_processFile.c_str(), g_selectedIconSpec.c_str());

    return true;
}

BOOL Wh_ModInit() {
    LoadProcessIdentity();

    if (g_processFile.empty()) {
        return FALSE;
    }

    if (!ReloadConfiguration()) {
        return FALSE;
    }

    if (!InstallHooks()) {
        return FALSE;
    }

    StartWorkerThread();

    return TRUE;
}

void Wh_ModAfterInit() {
    ApplyIconToAllWindows();
}

void Wh_ModSettingsChanged() {
    ReloadConfiguration();
}

void Wh_ModBeforeUninit() {
    g_active = false;
    StopWorkerThread();
}

void Wh_ModUninit() {
}
