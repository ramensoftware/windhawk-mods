// ==WindhawkMod==
// @id              taskbar-app-volume-mixer
// @name            Taskbar App Volume Mixer
// @description     Scroll over the taskbar to open a mixer of all apps and control the volume of each app separately
// @version         1.0.0
// @author          Sanekx Arcs
// @github          https://github.com/SanekxArcs
// @license         GPL-3.0-only
// @include         explorer.exe
// @compilerOptions -lcomctl32 -ldwmapi -lgdi32 -lole32 -lversion -lshell32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The taskbar scroll hooking is based on the "Taskbar Volume Control" mod by
// m417z: https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar App Volume Mixer

Scroll the mouse wheel over the taskbar to open a small mixer and set the volume
of each app separately, without opening the Windows volume mixer.

This mod is a fork of
[Taskbar Volume Control](https://windhawk.net/mods/taskbar-volume-control) by
m417z, which changes only the system volume. Don't enable both mods at once:
they react to the same scroll.

## Controls

* **Scroll over the taskbar**: opens the popup and changes the system volume.
* **Ctrl + scroll over the taskbar**: opens the popup and changes the volume of
  the app in the active window.
* **Scroll over a row in the popup**: changes the volume of that app only.
* **Click or drag on a bar**: sets the volume directly.
* **Click the icon, click the percentage, or middle-click a row**: mutes or
  unmutes the row.

Raising the volume of a muted row unmutes it. The popup closes by itself
shortly after the mouse leaves it. Both taskbar scroll actions can be changed in
the settings.

## Features

* The popup lists the system volume and every app that has an audio session,
  on all playback devices, and follows the light or dark taskbar theme.
* Apps that use several processes, such as browsers, are shown as one row.
* New apps and volume changes made elsewhere show up while the popup is open.
* While the popup is closed, the mod runs no timers and does no polling.

## Snap to every 5th value

When the value gets within 1 of a multiple of 5, or would pass over one, it
lands exactly on that multiple. With a step of 1, scrolling up from 5 gives 6,
7, 8, 10. The snap applies to steps smaller than 5 and to dragging a bar.

## Logarithmic app volume

Windows treats an app's volume as a plain multiplier, so most of the audible
change is squeezed into the lowest part of the slider: 50% is only 6 dB quieter
than 100%. With this option on, the slider position maps to decibels instead,
so each step changes the loudness by the same amount. The range setting is the
distance in decibels between 1% and 100%.

With the option on, an app's percentage in the popup no longer matches the
Windows volume mixer. The system volume row is never affected: Windows already
applies its own curve there.

## Differences from Taskbar Volume Control

Added: the mixer popup, per-app volume and mute, the active app shortcut, the
snap and the logarithmic curve.

Removed: scrolling anywhere with modifier keys, scrolling in full-screen apps,
middle-clicking the tray volume icon to mute, and the choice of Windows volume
indicators.

## Limitations

* Only apps that have an audio session are listed (playing now or played
  recently), the same as in the Windows volume mixer.
* An app running as administrator is shown as "Process 1234" with a generic
  icon.
* Scrolling inside the popup relies on the Windows setting "Scroll inactive
  windows when hovering over them", which is on by default.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- scrollTarget: master
  $name: Scrolling over the taskbar changes
  $options:
  - master: System volume
  - activeApp: Volume of the app in the active window
  - openOnly: Nothing, only open the mixer popup
- ctrlScrollTarget: activeApp
  $name: Ctrl + scrolling over the taskbar changes
  $options:
  - activeApp: Volume of the app in the active window
  - master: System volume
  - openOnly: Nothing, only open the mixer popup
  - disabled: Disabled (the taskbar handles the scroll)
- volumeChangeStep: 2
  $name: Volume change step
  $description: Percentage points per wheel notch.
- snapToFive: true
  $name: Snap to every 5th value
  $description: >-
    When the value gets within 1 of a multiple of 5 (or passes over it), it
    lands exactly on that multiple. Works for steps smaller than 5.
- logarithmic: false
  $name: Logarithmic app volume
  $description: >-
    Use a decibel curve for the app sliders, so that equal steps sound like
    equal changes in loudness. The system volume row is not affected.
- logRangeDb: 40
  $name: Logarithmic range (dB)
  $description: >-
    How many decibels the slider covers from 1% to 100%. Larger values make
    the lower part of the slider quieter. Typical values are 30 to 60.
- showOnlyActive: false
  $name: Show only apps that are playing sound right now
- popupTimeout: 1500
  $name: Popup timeout (ms)
  $description: How long the popup stays open after the mouse leaves it.
- scrollArea: taskbar
  $name: Scroll area
  $options:
  - taskbar: The taskbar
  - notification_area: The notification area
  - taskbarWithoutNotificationArea: The taskbar without the notification area
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option if you're using ExplorerPatcher or a similar tool that
    restores the old taskbar on Windows 11.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <audiopolicy.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <objbase.h>
#include <psapi.h>
#include <shellapi.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class ScrollTarget {
    master,
    activeApp,
    openOnly,
    disabled,
};

enum class ScrollArea {
    taskbar,
    notificationArea,
    taskbarWithoutNotificationArea,
};

struct {
    ScrollTarget scrollTarget;
    ScrollTarget ctrlScrollTarget;
    int volumeChangeStep;
    bool snapToFive;
    bool logarithmic;
    int logRangeDb;
    bool showOnlyActive;
    int popupTimeout;
    ScrollArea scrollArea;
    bool oldTaskbarOnWin11;
} g_settings;

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI->return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t
#endif

#ifndef WM_POINTERWHEEL
#define WM_POINTERWHEEL 0x024E
#endif

std::atomic<bool> g_initialized;
bool g_inputSiteProcHooked;
std::unordered_set<HWND> g_secondaryTaskbarWindows;

// True if the taskbar is the new XAML-based Windows 11 taskbar.
bool g_newTaskbar;
HWND g_hTaskbarWnd;
DWORD g_dwTaskbarThreadId;

UINT g_uninitMsg = RegisterWindowMessage(L"Windhawk_Uninit_" WH_MOD_ID);

#pragma region taskbar_functions

UINT GetDpiForWindowWithFallback(HWND hWnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
    static GetDpiForWindow_t pGetDpiForWindow = []() {
        HMODULE hUser32 = GetModuleHandle(L"user32.dll");
        if (hUser32) {
            return (GetDpiForWindow_t)GetProcAddress(hUser32,
                                                     "GetDpiForWindow");
        }

        return (GetDpiForWindow_t) nullptr;
    }();

    int iDpi = 96;
    if (pGetDpiForWindow) {
        iDpi = pGetDpiForWindow(hWnd);
    } else {
        HDC hdc = GetDC(NULL);
        if (hdc) {
            iDpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(NULL, hdc);
        }
    }

    return iDpi ? iDpi : 96;
}

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

bool GetNotificationAreaRect(HWND hMMTaskbarWnd, RECT* rcResult) {
    if (hMMTaskbarWnd == g_hTaskbarWnd) {
        HWND hTrayNotifyWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"TrayNotifyWnd", NULL);
        if (hTrayNotifyWnd && GetWindowRect(hTrayNotifyWnd, rcResult) &&
            !IsRectEmpty(rcResult)) {
            return true;
        }
    } else if (g_newTaskbar) {
        RECT rcTaskbar;
        if (GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
            HWND hBridgeWnd = FindWindowEx(
                hMMTaskbarWnd, NULL,
                L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
            while (hBridgeWnd) {
                RECT rcBridge;
                if (!GetWindowRect(hBridgeWnd, &rcBridge)) {
                    break;
                }

                if (!EqualRect(&rcBridge, &rcTaskbar)) {
                    if (IsRectEmpty(&rcBridge)) {
                        break;
                    }

                    CopyRect(rcResult, &rcBridge);
                    return true;
                }

                hBridgeWnd = FindWindowEx(
                    hMMTaskbarWnd, hBridgeWnd,
                    L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
            }
        }
    } else {
        HWND hClockButtonWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"ClockButton", NULL);
        if (hClockButtonWnd && GetWindowRect(hClockButtonWnd, rcResult) &&
            !IsRectEmpty(rcResult)) {
            return true;
        }
    }

    RECT rcTaskbar;
    if (!GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
        return false;
    }

    // Just consider the last pixels as a fallback, not accurate, but better
    // than nothing.
    int lastPixels = MulDiv(50, GetDpiForWindowWithFallback(hMMTaskbarWnd), 96);
    CopyRect(rcResult, &rcTaskbar);
    if (rcResult->right - rcResult->left > lastPixels) {
        if (GetWindowLong(hMMTaskbarWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
            rcResult->right = rcResult->left + lastPixels;
        } else {
            rcResult->left = rcResult->right - lastPixels;
        }
    }

    return true;
}

bool IsPointInsideScrollArea(HWND hMMTaskbarWnd, POINT pt) {
    RECT rc;
    switch (g_settings.scrollArea) {
        case ScrollArea::taskbar:
            return GetWindowRect(hMMTaskbarWnd, &rc) && PtInRect(&rc, pt);

        case ScrollArea::notificationArea:
            return GetNotificationAreaRect(hMMTaskbarWnd, &rc) &&
                   PtInRect(&rc, pt);

        case ScrollArea::taskbarWithoutNotificationArea:
            return GetWindowRect(hMMTaskbarWnd, &rc) && PtInRect(&rc, pt) &&
                   (!GetNotificationAreaRect(hMMTaskbarWnd, &rc) ||
                    !PtInRect(&rc, pt));
    }

    return false;
}

bool IsWindows11() {
    HRSRC hResource =
        FindResource(nullptr, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (!hResource) {
        return false;
    }

    HGLOBAL hGlobal = LoadResource(nullptr, hResource);
    void* pData = hGlobal ? LockResource(hGlobal) : nullptr;
    if (!pData) {
        return false;
    }

    VS_FIXEDFILEINFO* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;
    if (!VerQueryValue(pData, L"\\", (void**)&pFixedFileInfo, &uPtrLen) ||
        !uPtrLen) {
        return false;
    }

    WORD nMajor = HIWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nBuild = HIWORD(pFixedFileInfo->dwFileVersionLS);
    return nMajor > 10 || (nMajor == 10 && nBuild >= 22000);
}

#pragma endregion  // taskbar_functions

#pragma region audio

struct MixerEntry {
    // "*master", "*system", a lowercase executable path, or "pid:N".
    std::wstring key;
    std::wstring name;
    HICON icon = nullptr;  // Owned by g_appInfoCache.
    WCHAR glyph = 0;       // Used if there's no icon.
    IAudioEndpointVolume* endpoint = nullptr;
    std::vector<ISimpleAudioVolume*> sessions;
    bool active = false;
    int pos = 0;  // Slider position, 0-100.
    bool muted = false;
};

struct AppInfo {
    std::wstring path;
    std::wstring name;
    HICON icon = nullptr;
};

// All of the following are only used from the taskbar thread.
IMMDeviceEnumerator* g_pDeviceEnumerator;
std::vector<MixerEntry> g_entries;
// Kept until the mod is unloaded, so each app is read from disk only once.
std::map<std::wstring, AppInfo> g_appInfoCache;
// Apps whose description and icon weren't read from disk yet.
std::vector<std::wstring> g_pendingAppInfo;
// Process paths, valid while the popup is open.
std::unordered_map<DWORD, std::wstring> g_pidPathCache;

const WCHAR kMasterKey[] = L"*master";
const WCHAR kSystemSoundsKey[] = L"*system";
const WCHAR kGlyphVolume = 0xE767;
const WCHAR kGlyphMute = 0xE74F;
const WCHAR kGlyphSystemSounds = 0xEA8F;

// Slider position (0-100) to the linear amplitude that the session API uses.
float PosToAmp(int pos) {
    pos = std::clamp(pos, 0, 100);
    if (!g_settings.logarithmic) {
        return pos / 100.0f;
    }

    if (pos == 0) {
        return 0.0f;
    }

    float db = g_settings.logRangeDb * (pos / 100.0f - 1.0f);
    return powf(10.0f, db / 20.0f);
}

int AmpToPos(float amp) {
    if (amp <= 0.0f) {
        return 0;
    }

    float p = amp;
    if (g_settings.logarithmic) {
        p = 1.0f + 20.0f * log10f(amp) / g_settings.logRangeDb;
    }

    return std::clamp((int)lroundf(p * 100.0f), 0, 100);
}

void ReadEntry(MixerEntry& entry) {
    float volume = 0.0f;
    BOOL muted = FALSE;
    if (entry.endpoint) {
        entry.endpoint->GetMasterVolumeLevelScalar(&volume);
        entry.endpoint->GetMute(&muted);
        entry.pos = std::clamp((int)lroundf(volume * 100.0f), 0, 100);
    } else if (!entry.sessions.empty()) {
        entry.sessions[0]->GetMasterVolume(&volume);
        entry.sessions[0]->GetMute(&muted);
        entry.pos = AmpToPos(volume);
    }

    entry.muted = muted;
}

void SetEntryMute(MixerEntry& entry, bool mute) {
    if (entry.endpoint) {
        entry.endpoint->SetMute(mute, nullptr);
    }

    for (ISimpleAudioVolume* session : entry.sessions) {
        session->SetMute(mute, nullptr);
    }

    entry.muted = mute;
}

void SetEntryPos(MixerEntry& entry, int pos) {
    pos = std::clamp(pos, 0, 100);

    if (entry.endpoint) {
        entry.endpoint->SetMasterVolumeLevelScalar(pos / 100.0f, nullptr);
    }

    float amp = PosToAmp(pos);
    for (ISimpleAudioVolume* session : entry.sessions) {
        session->SetMasterVolume(amp, nullptr);
    }

    entry.pos = pos;

    if (pos > 0 && entry.muted) {
        SetEntryMute(entry, false);
    }
}

// Moves the position by the given amount of wheel notches, applying the
// optional snap to multiples of 5.
int StepPos(int pos, int clicks) {
    int dir = clicks > 0 ? 1 : -1;
    int step = std::max(1, g_settings.volumeChangeStep);

    for (int i = 0; i < abs(clicks); i++) {
        int target = pos + dir * step;

        if (g_settings.snapToFive && step < 5) {
            if (dir > 0) {
                int next = (pos / 5 + 1) * 5;
                if (target >= next - 1) {
                    target = next;
                }
            } else if (pos > 0) {
                int next = ((pos - 1) / 5) * 5;
                if (target <= next + 1) {
                    target = next;
                }
            }
        }

        pos = std::clamp(target, 0, 100);
    }

    return pos;
}

MixerEntry* FindEntry(const std::wstring& key) {
    for (MixerEntry& entry : g_entries) {
        if (entry.key == key) {
            return &entry;
        }
    }

    return nullptr;
}

void ReleaseEntries() {
    for (MixerEntry& entry : g_entries) {
        if (entry.endpoint) {
            entry.endpoint->Release();
        }

        for (ISimpleAudioVolume* session : entry.sessions) {
            session->Release();
        }
    }

    g_entries.clear();
}

std::wstring GetProcessPath(DWORD pid) {
    auto it = g_pidPathCache.find(pid);
    if (it != g_pidPathCache.end()) {
        return it->second;
    }

    std::wstring result;
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProcess) {
        WCHAR path[MAX_PATH * 2];
        DWORD size = ARRAYSIZE(path);
        if (QueryFullProcessImageName(hProcess, 0, path, &size)) {
            result.assign(path, size);
        }

        CloseHandle(hProcess);
    }

    g_pidPathCache.emplace(pid, result);
    return result;
}

std::wstring ToLower(std::wstring s) {
    if (!s.empty()) {
        CharLowerBuff(s.data(), (DWORD)s.size());
    }

    return s;
}

std::wstring GetFileDescription(const std::wstring& path) {
    DWORD handle;
    DWORD size = GetFileVersionInfoSize(path.c_str(), &handle);
    if (!size) {
        return {};
    }

    std::vector<BYTE> buffer(size);
    if (!GetFileVersionInfo(path.c_str(), 0, size, buffer.data())) {
        return {};
    }

    struct LangAndCodePage {
        WORD language;
        WORD codePage;
    }* translation;
    UINT len;
    if (!VerQueryValue(buffer.data(), L"\\VarFileInfo\\Translation",
                       (void**)&translation, &len) ||
        len < sizeof(LangAndCodePage)) {
        return {};
    }

    WCHAR query[64];
    wsprintf(query, L"\\StringFileInfo\\%04x%04x\\FileDescription",
             translation->language, translation->codePage);

    WCHAR* description;
    if (!VerQueryValue(buffer.data(), query, (void**)&description, &len) ||
        !len) {
        return {};
    }

    std::wstring result = description;
    while (!result.empty() && result.back() == L' ') {
        result.pop_back();
    }

    return result;
}

const AppInfo& GetAppInfo(const std::wstring& key, const std::wstring& path) {
    auto it = g_appInfoCache.find(key);
    if (it != g_appInfoCache.end()) {
        return it->second;
    }

    // Start with the file name, which needs no disk access. The description
    // and the icon are read later, one app at a time, so that the taskbar
    // thread is never blocked for long.
    AppInfo info;
    info.path = path;
    size_t slash = path.find_last_of(L'\\');
    info.name = slash == std::wstring::npos ? path : path.substr(slash + 1);
    size_t dot = info.name.find_last_of(L'.');
    if (dot != std::wstring::npos && dot > 0) {
        info.name.resize(dot);
    }

    g_pendingAppInfo.push_back(key);
    return g_appInfoCache.emplace(key, std::move(info)).first->second;
}

// Reads the description and the icon of one pending app. Returns true if a
// visible row was updated.
bool LoadOnePendingAppInfo() {
    if (g_pendingAppInfo.empty()) {
        return false;
    }

    std::wstring key = std::move(g_pendingAppInfo.back());
    g_pendingAppInfo.pop_back();

    auto it = g_appInfoCache.find(key);
    if (it == g_appInfoCache.end()) {
        return false;
    }

    AppInfo& info = it->second;

    std::wstring description = GetFileDescription(info.path);
    if (!description.empty()) {
        info.name = std::move(description);
    }

    SHFILEINFO sfi{};
    if (SHGetFileInfo(info.path.c_str(), 0, &sfi, sizeof(sfi),
                      SHGFI_ICON | SHGFI_LARGEICON)) {
        info.icon = sfi.hIcon;
    }

    MixerEntry* entry = FindEntry(key);
    if (!entry) {
        return false;
    }

    entry->name = info.name;
    entry->icon = info.icon;
    return true;
}

// Sorting uses the file name and not the description, so that rows don't move
// when a description is loaded.
PCWSTR EntrySortName(const MixerEntry& entry) {
    size_t slash = entry.key.find_last_of(L'\\');
    return slash == std::wstring::npos ? entry.name.c_str()
                                       : entry.key.c_str() + slash + 1;
}

// A cheap fingerprint of what the popup shows, to skip needless repaints.
std::wstring EntriesSignature() {
    std::wstring signature;
    for (const MixerEntry& entry : g_entries) {
        signature += entry.key;
        signature += entry.muted ? L'm' : L'u';
        signature += (WCHAR)(L'0' + entry.pos);
    }

    return signature;
}

void AddDeviceSessions(IMMDevice* device, std::vector<MixerEntry>& apps) {
    IAudioSessionManager2* manager = nullptr;
    if (FAILED(device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                                nullptr, (void**)&manager))) {
        return;
    }

    IAudioSessionEnumerator* enumerator = nullptr;
    if (SUCCEEDED(manager->GetSessionEnumerator(&enumerator))) {
        int count = 0;
        enumerator->GetCount(&count);

        for (int i = 0; i < count; i++) {
            IAudioSessionControl* control = nullptr;
            if (FAILED(enumerator->GetSession(i, &control))) {
                continue;
            }

            IAudioSessionControl2* control2 = nullptr;
            ISimpleAudioVolume* volume = nullptr;
            AudioSessionState state = AudioSessionStateExpired;
            control->GetState(&state);

            if (state != AudioSessionStateExpired &&
                SUCCEEDED(control->QueryInterface(
                    __uuidof(IAudioSessionControl2), (void**)&control2)) &&
                SUCCEEDED(control->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                  (void**)&volume))) {
                std::wstring key;
                std::wstring name;
                HICON icon = nullptr;
                WCHAR glyph = 0;

                DWORD pid = 0;
                if (control2->IsSystemSoundsSession() == S_OK) {
                    key = kSystemSoundsKey;
                    name = L"System sounds";
                    glyph = kGlyphSystemSounds;
                } else if (SUCCEEDED(control2->GetProcessId(&pid)) && pid) {
                    std::wstring path = GetProcessPath(pid);
                    if (!path.empty()) {
                        key = ToLower(path);
                        const AppInfo& info = GetAppInfo(key, path);
                        name = info.name;
                        icon = info.icon;
                    } else {
                        // Most likely an elevated process.
                        key = L"pid:" + std::to_wstring(pid);

                        LPWSTR displayName = nullptr;
                        if (SUCCEEDED(control->GetDisplayName(&displayName)) &&
                            displayName) {
                            if (*displayName && *displayName != L'@') {
                                name = displayName;
                            }

                            CoTaskMemFree(displayName);
                        }

                        if (name.empty()) {
                            name = L"Process " + std::to_wstring(pid);
                        }
                    }

                    if (!icon) {
                        glyph = kGlyphVolume;
                    }
                }

                if (!key.empty()) {
                    auto it = std::find_if(
                        apps.begin(), apps.end(),
                        [&key](const MixerEntry& e) { return e.key == key; });
                    if (it == apps.end()) {
                        MixerEntry entry;
                        entry.key = std::move(key);
                        entry.name = std::move(name);
                        entry.icon = icon;
                        entry.glyph = glyph;
                        apps.push_back(std::move(entry));
                        it = apps.end() - 1;
                    }

                    it->sessions.push_back(volume);
                    volume = nullptr;

                    if (state == AudioSessionStateActive) {
                        it->active = true;
                    }
                }
            }

            if (volume) {
                volume->Release();
            }

            if (control2) {
                control2->Release();
            }

            control->Release();
        }

        enumerator->Release();
    }

    manager->Release();
}

void RebuildEntries() {
    ReleaseEntries();

    if (!g_pDeviceEnumerator) {
        HRESULT hr = CoCreateInstance(
            __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IMMDeviceEnumerator), (void**)&g_pDeviceEnumerator);
        if (FAILED(hr)) {
            Wh_Log(L"CoCreateInstance failed: %08X", hr);
            g_pDeviceEnumerator = nullptr;
            return;
        }
    }

    IMMDevice* defaultDevice = nullptr;
    if (SUCCEEDED(g_pDeviceEnumerator->GetDefaultAudioEndpoint(
            eRender, eConsole, &defaultDevice))) {
        IAudioEndpointVolume* endpoint = nullptr;
        if (SUCCEEDED(defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
                                              CLSCTX_ALL, nullptr,
                                              (void**)&endpoint))) {
            MixerEntry entry;
            entry.key = kMasterKey;
            entry.name = L"System volume";
            entry.glyph = kGlyphVolume;
            entry.endpoint = endpoint;
            entry.active = true;
            g_entries.push_back(std::move(entry));
        }

        defaultDevice->Release();
    }

    std::vector<MixerEntry> apps;

    IMMDeviceCollection* devices = nullptr;
    if (SUCCEEDED(g_pDeviceEnumerator->EnumAudioEndpoints(
            eRender, DEVICE_STATE_ACTIVE, &devices))) {
        UINT count = 0;
        devices->GetCount(&count);

        for (UINT i = 0; i < count; i++) {
            IMMDevice* device = nullptr;
            if (SUCCEEDED(devices->Item(i, &device))) {
                AddDeviceSessions(device, apps);
                device->Release();
            }
        }

        devices->Release();
    }

    std::sort(apps.begin(), apps.end(),
              [](const MixerEntry& a, const MixerEntry& b) {
                  bool aSystem = a.key == kSystemSoundsKey;
                  bool bSystem = b.key == kSystemSoundsKey;
                  if (aSystem != bSystem) {
                      return bSystem;
                  }

                  int cmp = _wcsicmp(EntrySortName(a), EntrySortName(b));
                  return cmp ? cmp < 0 : a.key < b.key;
              });

    for (MixerEntry& app : apps) {
        if (g_settings.showOnlyActive && !app.active) {
            for (ISimpleAudioVolume* session : app.sessions) {
                session->Release();
            }

            continue;
        }

        g_entries.push_back(std::move(app));
    }

    for (MixerEntry& entry : g_entries) {
        ReadEntry(entry);
    }
}

// Returns the mixer entry of the app that owns the foreground window.
MixerEntry* FindForegroundEntry() {
    HWND hForegroundWnd = GetForegroundWindow();
    if (!hForegroundWnd) {
        return nullptr;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hForegroundWnd, &pid);

    // Store apps are hosted in a frame window of another process, the real app
    // process owns one of the child windows.
    WCHAR szClassName[64];
    if (GetClassName(hForegroundWnd, szClassName, ARRAYSIZE(szClassName)) &&
        _wcsicmp(szClassName, L"ApplicationFrameWindow") == 0) {
        struct Param {
            DWORD hostPid;
            DWORD appPid;
        } param = {pid, 0};
        EnumChildWindows(
            hForegroundWnd,
            [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
                Param& param = *(Param*)lParam;
                DWORD childPid = 0;
                GetWindowThreadProcessId(hWnd, &childPid);
                if (childPid && childPid != param.hostPid) {
                    param.appPid = childPid;
                    return FALSE;
                }

                return TRUE;
            },
            (LPARAM)&param);
        if (param.appPid) {
            pid = param.appPid;
        }
    }

    if (!pid) {
        return nullptr;
    }

    std::wstring path = GetProcessPath(pid);
    if (path.empty()) {
        return FindEntry(L"pid:" + std::to_wstring(pid));
    }

    return FindEntry(ToLower(path));
}

#pragma endregion  // audio

#pragma region mixer_popup

const WCHAR kMixerClassName[] = L"Windhawk_AppVolumeMixer_" WH_MOD_ID;

// Layout, in device-independent pixels.
constexpr int kWidth = 380;
constexpr int kRowHeight = 44;
constexpr int kPadX = 14;
constexpr int kPadY = 8;
constexpr int kIconSize = 24;
constexpr int kNameWidth = 120;
constexpr int kPercentWidth = 42;
constexpr int kGap = 10;

constexpr UINT_PTR kMixerTimerId = 1;
constexpr int kMixerTimerInterval = 100;
// Volumes are re-read twice a second, the list of apps once a second.
constexpr int kReadEveryTicks = 5;
constexpr int kRebuildEveryTicks = 10;

bool g_mixerClassRegistered;
HWND g_mixerWnd;
UINT g_mixerDpi = 96;
POINT g_anchorPt;
HWND g_anchorTaskbarWnd;
int g_hoverRow = -1;
bool g_trackingMouse;
int g_idleMs;
int g_tickCount;
bool g_darkTheme;
HFONT g_textFont;
HFONT g_glyphFont;
// The row that was last changed by scrolling over the taskbar.
std::wstring g_highlightKey;
// The row whose bar is being dragged.
std::wstring g_dragKey;

DWORD g_lastWheelTime;
int g_wheelRemainder;

int Scale(int value) {
    return MulDiv(value, g_mixerDpi, 96);
}

// Converts a wheel delta to whole notches, keeping the remainder for
// high-resolution wheels and touchpads.
int WheelDeltaToClicks(short delta) {
    int total = delta;
    if (GetTickCount() - g_lastWheelTime < 1000) {
        total += g_wheelRemainder;
    }

    g_lastWheelTime = GetTickCount();
    g_wheelRemainder = total % WHEEL_DELTA;
    return total / WHEEL_DELTA;
}

int MixerRowCount() {
    return std::max(1, (int)g_entries.size());
}

int MixerHeight() {
    return Scale(kPadY) * 2 + Scale(kRowHeight) * MixerRowCount();
}

RECT MixerRowRect(int row) {
    RECT rc;
    rc.left = 0;
    rc.right = Scale(kWidth);
    rc.top = Scale(kPadY) + Scale(kRowHeight) * row;
    rc.bottom = rc.top + Scale(kRowHeight);
    return rc;
}

// Horizontal extent of the volume bar.
void MixerBarExtent(int* left, int* right) {
    *left = Scale(kPadX + kIconSize + kGap + kNameWidth + kGap);
    *right = Scale(kWidth - kPadX - kPercentWidth - kGap);
}

int MixerRowFromPoint(POINT ptClient) {
    int y = ptClient.y - Scale(kPadY);
    if (y < 0 || ptClient.x < 0 || ptClient.x >= Scale(kWidth)) {
        return -1;
    }

    int row = y / Scale(kRowHeight);
    return row < (int)g_entries.size() ? row : -1;
}

void PositionMixer(bool show) {
    int width = Scale(kWidth);
    int height = MixerHeight();

    HMONITOR monitor = MonitorFromPoint(g_anchorPt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(monitor, &mi);

    // The area next to the taskbar. The work area isn't enough, since it
    // includes the taskbar if it's set to hide automatically.
    RECT avail = mi.rcWork;
    RECT rcTaskbar;
    if (g_anchorTaskbarWnd && GetWindowRect(g_anchorTaskbarWnd, &rcTaskbar)) {
        bool horizontal = (rcTaskbar.right - rcTaskbar.left) >=
                          (rcTaskbar.bottom - rcTaskbar.top);
        if (horizontal) {
            if (rcTaskbar.top + rcTaskbar.bottom >
                mi.rcMonitor.top + mi.rcMonitor.bottom) {
                avail.bottom = std::min(avail.bottom, rcTaskbar.top);
            } else {
                avail.top = std::max(avail.top, rcTaskbar.bottom);
            }
        } else {
            if (rcTaskbar.left + rcTaskbar.right >
                mi.rcMonitor.left + mi.rcMonitor.right) {
                avail.right = std::min(avail.right, rcTaskbar.left);
            } else {
                avail.left = std::max(avail.left, rcTaskbar.right);
            }
        }
    }

    InflateRect(&avail, -Scale(8), -Scale(8));

    int x = g_anchorPt.x - width / 2;
    int y = g_anchorPt.y - height / 2;
    x = std::max((int)avail.left, std::min(x, (int)avail.right - width));
    y = std::max((int)avail.top, std::min(y, (int)avail.bottom - height));

    SetWindowPos(g_mixerWnd, HWND_TOPMOST, x, y, width, height,
                 SWP_NOACTIVATE | (show ? SWP_SHOWWINDOW : 0));
}

bool IsTaskbarDarkTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValue(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
            &size) != ERROR_SUCCESS) {
        return true;
    }

    return value == 0;
}

void HideMixer() {
    if (!g_mixerWnd) {
        return;
    }

    KillTimer(g_mixerWnd, kMixerTimerId);

    if (GetCapture() == g_mixerWnd) {
        ReleaseCapture();
    }

    ShowWindow(g_mixerWnd, SW_HIDE);

    g_dragKey.clear();
    g_highlightKey.clear();
    g_hoverRow = -1;
    ReleaseEntries();
    g_pidPathCache.clear();

    if (g_textFont) {
        DeleteObject(g_textFont);
        g_textFont = nullptr;
    }

    if (g_glyphFont) {
        DeleteObject(g_glyphFont);
        g_glyphFont = nullptr;
    }
}

void FillSolidRect(HDC hdc, const RECT& rc, COLORREF color) {
    SetDCBrushColor(hdc, color);
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(DC_BRUSH));
}

void PaintMixer(HDC hdc, const RECT& rcClient) {
    COLORREF clrBackground = g_darkTheme ? RGB(44, 44, 44) : RGB(249, 249, 249);
    COLORREF clrBorder = g_darkTheme ? RGB(70, 70, 70) : RGB(208, 208, 208);
    COLORREF clrHover = g_darkTheme ? RGB(60, 60, 60) : RGB(234, 234, 234);
    COLORREF clrText = g_darkTheme ? RGB(255, 255, 255) : RGB(27, 27, 27);
    COLORREF clrDimText = g_darkTheme ? RGB(160, 160, 160) : RGB(110, 110, 110);
    COLORREF clrTrack = g_darkTheme ? RGB(95, 95, 95) : RGB(190, 190, 190);
    COLORREF clrAccent = g_darkTheme ? RGB(96, 205, 255) : RGB(0, 95, 184);

    FillSolidRect(hdc, rcClient, clrBackground);
    SetDCBrushColor(hdc, clrBorder);
    FrameRect(hdc, &rcClient, (HBRUSH)GetStockObject(DC_BRUSH));

    // The fonts are created once per popup session and freed in HideMixer.
    if (!g_textFont) {
        g_textFont =
            CreateFont(-Scale(14), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    }

    if (!g_glyphFont) {
        g_glyphFont =
            CreateFont(-Scale(18), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe MDL2 Assets");
    }

    HFONT textFont = g_textFont;
    HFONT glyphFont = g_glyphFont;
    HFONT prevFont = (HFONT)SelectObject(hdc, textFont);
    SetBkMode(hdc, TRANSPARENT);

    if (g_entries.empty()) {
        RECT rc = MixerRowRect(0);
        SetTextColor(hdc, clrDimText);
        DrawText(hdc, L"No audio device", -1, &rc,
                 DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
    }

    int barLeft, barRight;
    MixerBarExtent(&barLeft, &barRight);

    for (int i = 0; i < (int)g_entries.size(); i++) {
        const MixerEntry& entry = g_entries[i];
        RECT rcRow = MixerRowRect(i);
        int centerY = (rcRow.top + rcRow.bottom) / 2;

        bool highlighted = g_hoverRow >= 0 ? g_hoverRow == i
                                           : entry.key == g_highlightKey;
        if (highlighted) {
            RECT rc = rcRow;
            InflateRect(&rc, -Scale(4), -Scale(1));
            FillSolidRect(hdc, rc, clrHover);
        }

        // Icon.
        RECT rcIcon;
        rcIcon.left = Scale(kPadX);
        rcIcon.right = rcIcon.left + Scale(kIconSize);
        rcIcon.top = centerY - Scale(kIconSize) / 2;
        rcIcon.bottom = rcIcon.top + Scale(kIconSize);
        if (entry.icon) {
            DrawIconEx(hdc, rcIcon.left, rcIcon.top, entry.icon,
                       Scale(kIconSize), Scale(kIconSize), 0, nullptr,
                       DI_NORMAL);
        } else {
            WCHAR glyph = entry.muted ? kGlyphMute : entry.glyph;
            SelectObject(hdc, glyphFont);
            SetTextColor(hdc, entry.muted ? clrDimText : clrText);
            DrawText(hdc, &glyph, 1, &rcIcon,
                     DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
            SelectObject(hdc, textFont);
        }

        // Name.
        RECT rcName = rcRow;
        rcName.left = rcIcon.right + Scale(kGap);
        rcName.right = rcName.left + Scale(kNameWidth);
        SetTextColor(hdc, entry.muted ? clrDimText : clrText);
        DrawText(hdc, entry.name.c_str(), -1, &rcName,
                 DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);

        // Bar.
        int fillRight = barLeft + MulDiv(barRight - barLeft, entry.pos, 100);
        int trackHalf = std::max(1, Scale(4) / 2);
        RECT rcTrack = {barLeft, centerY - trackHalf, barRight,
                        centerY + trackHalf};
        FillSolidRect(hdc, rcTrack, clrTrack);
        RECT rcFill = rcTrack;
        rcFill.right = fillRight;
        FillSolidRect(hdc, rcFill, entry.muted ? clrDimText : clrAccent);

        int thumbHalfWidth = std::max(1, Scale(4) / 2);
        int thumbHalfHeight = Scale(8);
        RECT rcThumb = {fillRight - thumbHalfWidth, centerY - thumbHalfHeight,
                        fillRight + thumbHalfWidth, centerY + thumbHalfHeight};
        FillSolidRect(hdc, rcThumb, entry.muted ? clrDimText : clrAccent);

        // Percentage, or a mute glyph for muted apps.
        RECT rcPercent = rcRow;
        rcPercent.right = Scale(kWidth - kPadX);
        rcPercent.left = rcPercent.right - Scale(kPercentWidth);
        if (entry.muted && entry.icon) {
            WCHAR glyph = kGlyphMute;
            SelectObject(hdc, glyphFont);
            SetTextColor(hdc, clrDimText);
            DrawText(hdc, &glyph, 1, &rcPercent,
                     DT_SINGLELINE | DT_RIGHT | DT_VCENTER | DT_NOPREFIX);
            SelectObject(hdc, textFont);
        } else {
            WCHAR text[16];
            wsprintf(text, L"%d%%", entry.pos);
            SetTextColor(hdc, entry.muted ? clrDimText : clrText);
            DrawText(hdc, text, -1, &rcPercent,
                     DT_SINGLELINE | DT_RIGHT | DT_VCENTER | DT_NOPREFIX);
        }
    }

    SelectObject(hdc, prevFont);
}

void DragMixerBarTo(int xClient) {
    MixerEntry* entry = FindEntry(g_dragKey);
    if (!entry) {
        return;
    }

    int barLeft, barRight;
    MixerBarExtent(&barLeft, &barRight);
    int pos = MulDiv(xClient - barLeft, 100, barRight - barLeft);
    pos = std::clamp(pos, 0, 100);

    if (g_settings.snapToFive) {
        int nearest = (pos + 2) / 5 * 5;
        if (abs(pos - nearest) <= 1) {
            pos = nearest;
        }
    }

    if (pos != entry->pos) {
        SetEntryPos(*entry, pos);
        InvalidateRect(g_mixerWnd, nullptr, FALSE);
    }
}

void OnMixerTimer() {
    POINT pt;
    GetCursorPos(&pt);
    RECT rc;
    GetWindowRect(g_mixerWnd, &rc);

    bool dragging = !g_dragKey.empty();
    if (dragging || PtInRect(&rc, pt)) {
        g_idleMs = 0;
    } else {
        g_idleMs += kMixerTimerInterval;
        if (g_idleMs >= g_settings.popupTimeout) {
            HideMixer();
            return;
        }
    }

    if (dragging) {
        return;
    }

    bool changed = false;

    // Read the description and icon of at most one new app per tick.
    if (LoadOnePendingAppInfo()) {
        changed = true;
    }

    // Refresh periodically to pick up new apps and volume changes made
    // elsewhere. Repaint only if something is different.
    g_tickCount++;
    if (g_tickCount % kRebuildEveryTicks == 0) {
        std::wstring prevSignature = EntriesSignature();
        size_t prevCount = g_entries.size();
        RebuildEntries();

        if (g_entries.size() != prevCount) {
            PositionMixer(false);

            POINT ptClient = pt;
            ScreenToClient(g_mixerWnd, &ptClient);
            GetWindowRect(g_mixerWnd, &rc);
            g_hoverRow = PtInRect(&rc, pt) ? MixerRowFromPoint(ptClient) : -1;
        }

        if (EntriesSignature() != prevSignature) {
            changed = true;
        }
    } else if (g_tickCount % kReadEveryTicks == 0) {
        for (MixerEntry& entry : g_entries) {
            int prevPos = entry.pos;
            bool prevMuted = entry.muted;
            ReadEntry(entry);
            if (entry.pos != prevPos || entry.muted != prevMuted) {
                changed = true;
            }
        }
    }

    if (changed) {
        InvalidateRect(g_mixerWnd, nullptr, FALSE);
    }
}

LRESULT CALLBACK MixerWndProc(HWND hWnd,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam) {
    switch (uMsg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);

            HDC memDc = CreateCompatibleDC(hdc);
            HBITMAP bitmap = CreateCompatibleBitmap(hdc, rcClient.right,
                                                    rcClient.bottom);
            HBITMAP prevBitmap = (HBITMAP)SelectObject(memDc, bitmap);
            PaintMixer(memDc, rcClient);
            BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, memDc, 0, 0,
                   SRCCOPY);
            SelectObject(memDc, prevBitmap);
            DeleteObject(bitmap);
            DeleteDC(memDc);

            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_TIMER:
            if (wParam == kMixerTimerId) {
                OnMixerTimer();
            }
            return 0;

        case WM_MOUSEMOVE: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

            if (!g_trackingMouse) {
                TRACKMOUSEEVENT tme{};
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hWnd;
                g_trackingMouse = TrackMouseEvent(&tme);
            }

            if (!g_dragKey.empty()) {
                DragMixerBarTo(pt.x);
                return 0;
            }

            int row = MixerRowFromPoint(pt);
            if (row != g_hoverRow) {
                g_hoverRow = row;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            g_trackingMouse = false;
            if (g_dragKey.empty() && g_hoverRow != -1) {
                g_hoverRow = -1;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;

        case WM_MOUSEWHEEL: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hWnd, &pt);
            int row = MixerRowFromPoint(pt);
            int clicks = WheelDeltaToClicks(GET_WHEEL_DELTA_WPARAM(wParam));
            if (row >= 0 && clicks) {
                MixerEntry& entry = g_entries[row];
                SetEntryPos(entry, StepPos(entry.pos, clicks));
                g_hoverRow = row;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int row = MixerRowFromPoint(pt);
            if (row < 0) {
                return 0;
            }

            int barLeft, barRight;
            MixerBarExtent(&barLeft, &barRight);
            int slack = Scale(6);
            if (pt.x >= barLeft - slack && pt.x <= barRight + slack) {
                g_dragKey = g_entries[row].key;
                g_hoverRow = row;
                SetCapture(hWnd);
                DragMixerBarTo(pt.x);
            } else if (pt.x < Scale(kPadX + kIconSize + kGap / 2) ||
                       pt.x > barRight + slack) {
                SetEntryMute(g_entries[row], !g_entries[row].muted);
            }

            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }

        case WM_LBUTTONUP:
            if (GetCapture() == hWnd) {
                ReleaseCapture();
            }
            return 0;

        case WM_CAPTURECHANGED:
            g_dragKey.clear();
            return 0;

        case WM_MBUTTONUP: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int row = MixerRowFromPoint(pt);
            if (row >= 0) {
                SetEntryMute(g_entries[row], !g_entries[row].muted);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool EnsureMixerWindow() {
    if (g_mixerWnd) {
        return true;
    }

    if (!g_mixerClassRegistered) {
        WNDCLASS wc{};
        wc.lpfnWndProc = MixerWndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = kMixerClassName;
        if (!RegisterClass(&wc)) {
            Wh_Log(L"RegisterClass failed: %u", GetLastError());
            return false;
        }

        g_mixerClassRegistered = true;
    }

    g_mixerWnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kMixerClassName,
        L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, GetModuleHandle(nullptr),
        nullptr);
    if (!g_mixerWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return false;
    }

    // Rounded corners on Windows 11 (DWMWA_WINDOW_CORNER_PREFERENCE,
    // DWMWCP_ROUND). Fails harmlessly on Windows 10.
    int cornerPreference = 2;
    DwmSetWindowAttribute(g_mixerWnd, 33, &cornerPreference,
                          sizeof(cornerPreference));

    return true;
}

// Shows the popup, or keeps it open if it's already shown.
bool ShowMixer(HWND hTaskbarWnd, POINT pt) {
    if (!EnsureMixerWindow()) {
        return false;
    }

    g_idleMs = 0;

    if (!IsWindowVisible(g_mixerWnd)) {
        g_mixerDpi = GetDpiForWindowWithFallback(hTaskbarWnd);
        g_anchorPt = pt;
        g_anchorTaskbarWnd = hTaskbarWnd;
        g_darkTheme = IsTaskbarDarkTheme();
        g_hoverRow = -1;
        g_tickCount = 0;
        g_highlightKey.clear();

        RebuildEntries();
        PositionMixer(true);
        SetTimer(g_mixerWnd, kMixerTimerId, kMixerTimerInterval, nullptr);
    }

    return true;
}

// Must be called from the taskbar thread.
void DestroyMixer() {
    if (g_mixerWnd) {
        HideMixer();
        DestroyWindow(g_mixerWnd);
        g_mixerWnd = nullptr;
    }

    ReleaseEntries();

    if (g_pDeviceEnumerator) {
        g_pDeviceEnumerator->Release();
        g_pDeviceEnumerator = nullptr;
    }

    for (auto& [key, info] : g_appInfoCache) {
        if (info.icon) {
            DestroyIcon(info.icon);
        }
    }

    g_appInfoCache.clear();
    g_pendingAppInfo.clear();
    g_pidPathCache.clear();
}

#pragma endregion  // mixer_popup

bool OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    if (GetCapture()) {
        return false;
    }

    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    if (!IsPointInsideScrollArea(hWnd, pt)) {
        return false;
    }

    ScrollTarget target = GetKeyState(VK_CONTROL) < 0
                              ? g_settings.ctrlScrollTarget
                              : g_settings.scrollTarget;
    if (target == ScrollTarget::disabled) {
        return false;
    }

    if (!ShowMixer(hWnd, pt)) {
        return false;
    }

    int clicks = WheelDeltaToClicks(GET_WHEEL_DELTA_WPARAM(wParam));

    MixerEntry* entry = nullptr;
    switch (target) {
        case ScrollTarget::master:
            entry = FindEntry(kMasterKey);
            break;

        case ScrollTarget::activeApp:
            entry = FindForegroundEntry();
            break;

        default:
            break;
    }

    if (entry) {
        g_highlightKey = entry->key;
        if (clicks) {
            SetEntryPos(*entry, StepPos(entry->pos, clicks));
        }
    } else {
        g_highlightKey.clear();
    }

    InvalidateRect(g_mixerWnd, nullptr, FALSE);
    return true;
}

LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ DWORD_PTR dwRefData) {
    LRESULT result = 0;

    switch (uMsg) {
        case WM_MOUSEWHEEL:
            if (!g_newTaskbar && OnMouseWheel(hWnd, wParam, lParam)) {
                result = 0;
            } else {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;

        case WM_NCDESTROY:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            if (hWnd == g_hTaskbarWnd) {
                DestroyMixer();
            } else {
                g_secondaryTaskbarWindows.erase(hWnd);
            }
            break;

        default:
            if (uMsg == g_uninitMsg && hWnd == g_hTaskbarWnd) {
                DestroyMixer();
                result = 0;
            } else {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;
    }

    return result;
}

WNDPROC InputSiteWindowProc_Original;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    switch (uMsg) {
        case WM_POINTERWHEEL:
            if (HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
                IsTaskbarWindow(hRootWnd) &&
                OnMouseWheel(hRootWnd, wParam, lParam)) {
                return 0;
            }
            break;
    }

    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
}

void SubclassTaskbarWindow(HWND hWnd) {
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
                                                  TaskbarWindowSubclassProc, 0);
}

void UnsubclassTaskbarWindow(HWND hWnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                     TaskbarWindowSubclassProc);
}

void HandleIdentifiedInputSiteWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    HWND hParentWnd = GetParent(hWnd);
    WCHAR szClassName[64];
    if (!hParentWnd ||
        !GetClassName(hParentWnd, szClassName, ARRAYSIZE(szClassName)) ||
        _wcsicmp(szClassName,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") != 0) {
        return;
    }

    hParentWnd = GetParent(hParentWnd);
    if (!hParentWnd || !IsTaskbarWindow(hParentWnd)) {
        return;
    }

    // The window can't be subclassed: the inputsite.dll code checks that the
    // window procedure wasn't changed, and crashes otherwise. Hook the
    // procedure itself instead.
    auto wndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    WindhawkUtils::SetFunctionHook(wndProc, InputSiteWindowProc_Hook,
                                   &InputSiteWindowProc_Original);

    if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    Wh_Log(L"Hooked InputSite wndproc %p", wndProc);
    g_inputSiteProcHooked = true;
}

void HookInputSiteOfTaskbar(HWND hTaskbarWnd) {
    if (!g_newTaskbar || g_inputSiteProcHooked) {
        return;
    }

    HWND hXamlIslandWnd = FindWindowEx(
        hTaskbarWnd, nullptr,
        L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
    if (!hXamlIslandWnd) {
        return;
    }

    HWND hInputSiteWnd =
        FindWindowEx(hXamlIslandWnd, nullptr,
                     L"Windows.UI.Input.InputSite.WindowClass", nullptr);
    if (hInputSiteWnd) {
        HandleIdentifiedInputSiteWindow(hInputSiteWnd);
    }
}

void HandleIdentifiedTaskbarWindow(HWND hWnd) {
    g_hTaskbarWnd = hWnd;
    g_dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    SubclassTaskbarWindow(hWnd);
    for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
        SubclassTaskbarWindow(hSecondaryWnd);
    }

    HookInputSiteOfTaskbar(hWnd);
}

void HandleIdentifiedSecondaryTaskbarWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    g_secondaryTaskbarWindows.insert(hWnd);
    SubclassTaskbarWindow(hWnd);

    HookInputSiteOfTaskbar(hWnd);
}

HWND FindCurrentProcessTaskbarWindows(
    std::unordered_set<HWND>* secondaryTaskbarWindows) {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
        std::unordered_set<HWND>* secondaryTaskbarWindows;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd, secondaryTaskbarWindows};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId())
                return TRUE;

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
                return TRUE;

            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                *param.hWnd = hWnd;
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                param.secondaryTaskbarWindows->insert(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
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
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    } else if (bTextualClassName &&
               _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
        Wh_Log(L"Secondary taskbar window created: %08X",
               (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedSecondaryTaskbarWindow(hWnd);
    }

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
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
                                           LPVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
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
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        Wh_Log(L"InputSite window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        if (g_newTaskbar && !g_inputSiteProcHooked) {
            HandleIdentifiedInputSiteWindow(hWnd);
        }
    }

    return hWnd;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    return _wcsnicmp(L"ep_taskbar.", moduleFileName,
                     sizeof("ep_taskbar.") - 1) == 0;
}

void HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                g_newTaskbar = false;
                break;
            }
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && IsExplorerPatcherModule(module)) {
        g_newTaskbar = false;
    }

    return module;
}

ScrollTarget ParseScrollTarget(PCWSTR value, ScrollTarget defaultTarget) {
    if (wcscmp(value, L"master") == 0) {
        return ScrollTarget::master;
    } else if (wcscmp(value, L"activeApp") == 0) {
        return ScrollTarget::activeApp;
    } else if (wcscmp(value, L"openOnly") == 0) {
        return ScrollTarget::openOnly;
    } else if (wcscmp(value, L"disabled") == 0) {
        return ScrollTarget::disabled;
    }

    return defaultTarget;
}

void LoadSettings() {
    PCWSTR scrollTarget = Wh_GetStringSetting(L"scrollTarget");
    g_settings.scrollTarget =
        ParseScrollTarget(scrollTarget, ScrollTarget::master);
    Wh_FreeStringSetting(scrollTarget);

    PCWSTR ctrlScrollTarget = Wh_GetStringSetting(L"ctrlScrollTarget");
    g_settings.ctrlScrollTarget =
        ParseScrollTarget(ctrlScrollTarget, ScrollTarget::activeApp);
    Wh_FreeStringSetting(ctrlScrollTarget);

    g_settings.volumeChangeStep =
        std::clamp(Wh_GetIntSetting(L"volumeChangeStep"), 1, 25);
    g_settings.snapToFive = Wh_GetIntSetting(L"snapToFive");
    g_settings.logarithmic = Wh_GetIntSetting(L"logarithmic");
    g_settings.logRangeDb = std::clamp(Wh_GetIntSetting(L"logRangeDb"), 10, 96);
    g_settings.showOnlyActive = Wh_GetIntSetting(L"showOnlyActive");
    g_settings.popupTimeout =
        std::clamp(Wh_GetIntSetting(L"popupTimeout"), 300, 60000);

    PCWSTR scrollArea = Wh_GetStringSetting(L"scrollArea");
    g_settings.scrollArea = ScrollArea::taskbar;
    if (wcscmp(scrollArea, L"notification_area") == 0) {
        g_settings.scrollArea = ScrollArea::notificationArea;
    } else if (wcscmp(scrollArea, L"taskbarWithoutNotificationArea") == 0) {
        g_settings.scrollArea = ScrollArea::taskbarWithoutNotificationArea;
    }
    Wh_FreeStringSetting(scrollArea);

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_newTaskbar = IsWindows11() && !g_settings.oldTaskbarOnWin11;

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                           CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }
    }

    HandleLoadedExplorerPatcher();

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    HandleLoadedExplorerPatcher();

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd =
            FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
        if (hWnd) {
            HandleIdentifiedTaskbarWindow(hWnd);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_hTaskbarWnd) {
        // The popup and the audio objects belong to the taskbar thread, let it
        // clean them up.
        SendMessage(g_hTaskbarWnd, g_uninitMsg, 0, 0);

        UnsubclassTaskbarWindow(g_hTaskbarWnd);

        for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }

    if (g_mixerClassRegistered) {
        UnregisterClass(kMixerClassName, GetModuleHandle(nullptr));
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}
