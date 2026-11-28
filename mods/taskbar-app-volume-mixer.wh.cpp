// ==WindhawkMod==
// @id              taskbar-app-volume-mixer
// @name            Taskbar App Volume Mixer
// @description     Scroll over the taskbar to open a mixer of all apps and control the volume of each app separately
// @version         1.0.0
// @author          Sanekx Arcs
// @github          https://github.com/SanekxArcs
// @license         GPL-3.0-only
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @compilerOptions -lcomctl32 -ldwmapi -lgdi32 -lole32 -loleaut32 -lruntimeobject -lversion -lshell32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The taskbar scroll hooking is based on the "Taskbar Volume Control" mod by
// m417z, and the lookup of the app of a taskbar button is based on the
// "Taskbar Volume Control Per-App" mod by m417z:
// https://github.com/m417z/my-windhawk-mods
//
// The volume slider in the jump list is based on the "Taskbar Jump List Volume
// Control" mod by Ara:
// https://github.com/ara-hwang/windhawk-taskbar-jump-list-volume-control

// ==WindhawkModReadme==
/*
# Taskbar App Volume Mixer

Scroll the mouse wheel over the taskbar to open a small mixer and set the volume
of each app separately, without opening the Windows volume mixer.

![The mixer popup](https://i.imgur.com/Y65mlEM.png)

This mod started as a fork of
[Taskbar Volume Control](https://windhawk.net/mods/taskbar-volume-control) by
m417z, which changes only the system volume. Don't enable both mods at once:
they react to the same scroll.

## Controls

* **Scroll over the taskbar**: opens the popup and changes the system volume.
* **Ctrl + scroll over the taskbar**: opens the popup and changes the volume of
  the app in the active window.
* **Scroll over the taskbar button of an app** (Windows 11): opens the popup
  and changes the volume of that app, if the app has sound. Over other buttons
  the scroll works as usual.
* **Right-click the taskbar button of an app** (Windows 11): the jump list
  gets a volume slider and a mute button for that app, if the app has sound.
* **Scroll over a row in the popup**: changes the volume of that app only.
* **Click or drag on a bar**: sets the volume directly.
* **Click the icon, click the percentage, or middle-click a row**: mutes or
  unmutes the row.
* **Click the output device row** at the bottom of the popup: opens the list
  of playback devices. Click a device to make it the default one. Scrolling
  over the row switches to the next or previous device.
* **Click anywhere outside the popup**: closes it.

![The volume slider in the jump list](https://i.imgur.com/ADHhjLd.png)

Raising the volume of a muted row unmutes it. The popup also closes by itself
shortly after the mouse leaves it. Both taskbar scroll actions can be changed in
the settings.

## Features

* The popup lists the system volume and every app that has an audio session,
  on all playback devices, and follows the light or dark taskbar theme.
* Apps that use several processes, such as browsers, are shown as one row.
* A thin level meter under each bar shows which apps are playing sound right
  now, and how loud.
* New apps and volume changes made elsewhere show up while the popup is open.
* While the popup is closed, the mod runs no timers and does no polling, apart
  from a short check when a jump list opens.

## Snap to every 5th value

When the value gets within 1 of a multiple of 5, or would pass over one, it
lands exactly on that multiple. With a step of 1, scrolling up from 5 gives 6,
7, 8, 10. The snap applies to steps smaller than 5 and to dragging a bar in
the popup. The jump list slider snaps when you scroll on it, not when you drag
it.

## Logarithmic app volume

Windows treats an app's volume as a plain multiplier, so most of the audible
change is squeezed into the lowest part of the slider: 50% is only 6 dB quieter
than 100%. With this option on, the slider position maps to decibels instead,
so each step changes the loudness by the same amount. The range setting is the
distance in decibels between 1% and 100%.

With the option on, an app's percentage in the popup no longer matches the
Windows volume mixer. The system volume row is never affected: Windows already
applies its own curve there.

## Similar mods

This mod puts several ways of changing the volume into one mod, with one set of
settings (step, snap, logarithmic curve) that applies to all of them. Parts of
it are based on these mods, which stay the better choice if you want only that
one thing:

* [Taskbar Volume Control](https://windhawk.net/mods/taskbar-volume-control):
  system volume only. It also has scrolling anywhere with modifier keys,
  scrolling in full-screen apps, middle-click on the tray icon to mute and a
  choice of Windows volume indicators, which this mod doesn't have.
* [Taskbar Volume Control Per-App](https://windhawk.net/mods/taskbar-volume-control-per-app):
  scrolling over taskbar buttons, with a tooltip and touchpad support.
* [Taskbar Jump List Volume Control](https://windhawk.net/mods/taskbar-jump-list-volume-control):
  only the slider in the jump list.

Don't enable one of these together with the same feature of this mod. Turn the
feature off in the settings of this mod instead.

[Better Volume Mixer](https://windhawk.net/mods/better-volume-mixer) is another
mixer popup with per-app volume, mute and output device switching. It isn't
related to this mod and works in a different way: it opens from its own tray
icon and has pinned apps and themes, which this mod doesn't have. Better
Volume Mixer also changes the system volume when you scroll over its tray icon.
If you enable both mods, set "Scroll area" in this mod to "The taskbar without
the notification area", or both will react to that scroll.

## Limitations

* Only apps that have an audio session are listed (playing now or played
  recently), the same as in the Windows volume mixer.
* An app running as administrator is shown as "Process 1234" with a generic
  icon.
* Scrolling over a taskbar button and the jump list slider need the Windows 11
  taskbar on a 64-bit Intel or AMD processor. Windhawk downloads symbols for
  them the first time, which can take a while. If that fails, the rest of the
  mod still works.
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
- taskButtonScroll: true
  $name: Scrolling over an app's taskbar button changes that app
  $description: >-
    Windows 11 only. Applies if the app has sound, otherwise the scroll works
    as set above. The taskbar buttons must be inside the scroll area that is
    set below.
- jumpListSlider: true
  $name: Volume slider in the jump list of taskbar buttons
  $description: >-
    Windows 11 only. The jump list is the menu that opens with a right click
    on a taskbar button.
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
- showDeviceRow: true
  $name: Show the output device
  $description: >-
    A row at the bottom of the popup that shows the default playback device
    and lets you switch to another one.
- showPeakMeters: true
  $name: Show level meters
  $description: >-
    Draw a thin meter under each bar that moves with the sound the app is
    playing right now.
- popupTimeout: 1500
  $name: Popup timeout (ms)
  $description: How long the popup stays open after the mouse leaves it.
- closeOnClickOutside: true
  $name: Close the popup on a click outside it
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
#include <propsys.h>
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

// The taskbar button lookup uses names of functions of the 64-bit Intel/AMD
// build of Windows 11.
#ifdef _M_X64
#define TASK_BUTTONS_SUPPORTED

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wux = winrt::Windows::UI::Xaml;
#endif

// The toolchain headers only forward-declare this Core Audio interface.
#ifndef __IAudioMeterInformation_INTERFACE_DEFINED__
#define __IAudioMeterInformation_INTERFACE_DEFINED__
MIDL_INTERFACE("C02216F6-8C67-4B5B-9D00-D008E73E0064")
IAudioMeterInformation : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* peak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 count,
                                                            float* peaks) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* mask) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IAudioMeterInformation,
                0xC02216F6, 0x8C67, 0x4B5B,
                0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64)
#endif
#endif

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
    bool showDeviceRow;
    bool showPeakMeters;
    bool taskButtonScroll;
    bool jumpListSlider;
    int popupTimeout;
    bool closeOnClickOutside;
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
// The mod is also loaded into the process that shows the jump lists.
bool g_isShellExperienceHost;
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
    // Level meters of the device or of the sessions. Empty if they're off.
    std::vector<IAudioMeterInformation*> meters;
    float peak = 0.0f;  // The level that is drawn, 0-1.
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

struct OutputDevice {
    std::wstring id;
    std::wstring name;
};

// Playback devices, filled if the output device row is shown.
std::vector<OutputDevice> g_devices;
std::wstring g_defaultDeviceId;
// Device names, valid while the popup is open.
std::unordered_map<std::wstring, std::wstring> g_deviceNameCache;

// PKEY_Device_FriendlyName.
constexpr PROPERTYKEY kDeviceFriendlyNameKey = {
    {0xA45C254E, 0xDF1C, 0x4EFD,
     {0x80, 0x20, 0x67, 0xD1, 0x46, 0xA8, 0x50, 0xE0}},
    14};

const CLSID kCLSID_PolicyConfigClient = {
    0x870AF99C, 0x171D, 0x4F9E,
    {0xAF, 0x0D, 0xE6, 0x3D, 0xF4, 0x0C, 0x2B, 0xC9}};
const IID kIID_IPolicyConfig = {
    0xF8679F50, 0x850A, 0x41CF,
    {0x9C, 0x72, 0x43, 0x0F, 0x29, 0x02, 0x90, 0xC8}};

// The undocumented interface that the Windows sound settings use to change
// the default device. Only SetDefaultEndpoint is called, the other methods
// keep the layout right.
struct IPolicyConfig : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, void*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR,
                                                          INT,
                                                          PINT64,
                                                          PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR,
                                                       const PROPERTYKEY&,
                                                       PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR,
                                                       const PROPERTYKEY&,
                                                       PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR deviceId,
                                                         ERole role) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

const WCHAR kMasterKey[] = L"*master";
const WCHAR kSystemSoundsKey[] = L"*system";
const WCHAR kGlyphVolume = 0xE767;
const WCHAR kGlyphMute = 0xE74F;
const WCHAR kGlyphSystemSounds = 0xEA8F;
const WCHAR kGlyphDevice = 0xE7F5;
const WCHAR kGlyphCheck = 0xE73E;
const WCHAR kGlyphChevronDown = 0xE70D;
const WCHAR kGlyphChevronUp = 0xE70E;

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

void ReleaseEntry(MixerEntry& entry) {
    if (entry.endpoint) {
        entry.endpoint->Release();
        entry.endpoint = nullptr;
    }

    for (ISimpleAudioVolume* session : entry.sessions) {
        session->Release();
    }

    entry.sessions.clear();

    for (IAudioMeterInformation* meter : entry.meters) {
        meter->Release();
    }

    entry.meters.clear();
}

void ReleaseEntries() {
    for (MixerEntry& entry : g_entries) {
        ReleaseEntry(entry);
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

const OutputDevice* FindDefaultDevice() {
    for (const OutputDevice& device : g_devices) {
        if (device.id == g_defaultDeviceId) {
            return &device;
        }
    }

    return nullptr;
}

std::wstring GetDeviceId(IMMDevice* device) {
    std::wstring id;
    LPWSTR rawId = nullptr;
    if (SUCCEEDED(device->GetId(&rawId)) && rawId) {
        id = rawId;
        CoTaskMemFree(rawId);
    }

    return id;
}

std::wstring ReadDeviceName(IMMDevice* device) {
    std::wstring name;

    IPropertyStore* store = nullptr;
    if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, &store)) && store) {
        PROPVARIANT value;
        PropVariantInit(&value);
        if (SUCCEEDED(store->GetValue(kDeviceFriendlyNameKey, &value)) &&
            value.vt == VT_LPWSTR && value.pwszVal) {
            name = value.pwszVal;
        }

        PropVariantClear(&value);
        store->Release();
    }

    return name.empty() ? L"Audio device" : name;
}

void AddOutputDevice(IMMDevice* device) {
    if (!g_settings.showDeviceRow) {
        return;
    }

    OutputDevice outputDevice;
    outputDevice.id = GetDeviceId(device);
    if (outputDevice.id.empty()) {
        return;
    }

    auto it = g_deviceNameCache.find(outputDevice.id);
    if (it == g_deviceNameCache.end()) {
        it = g_deviceNameCache.emplace(outputDevice.id, ReadDeviceName(device))
                 .first;
    }

    outputDevice.name = it->second;
    g_devices.push_back(std::move(outputDevice));
}

// Makes the device the default one for playback. The device for calls
// (communications) is left as it is.
bool SetDefaultDevice(const std::wstring& id) {
    IPolicyConfig* policy = nullptr;
    HRESULT hr = CoCreateInstance(kCLSID_PolicyConfigClient, nullptr,
                                  CLSCTX_ALL, kIID_IPolicyConfig,
                                  (void**)&policy);
    if (SUCCEEDED(hr)) {
        hr = policy->SetDefaultEndpoint(id.c_str(), eConsole);
        if (SUCCEEDED(hr)) {
            hr = policy->SetDefaultEndpoint(id.c_str(), eMultimedia);
        }

        policy->Release();
    }

    if (FAILED(hr)) {
        Wh_Log(L"Setting the default device failed: %08X", hr);
    }

    return SUCCEEDED(hr);
}

// A cheap fingerprint of what the popup shows, to skip needless repaints.
std::wstring EntriesSignature() {
    std::wstring signature = g_defaultDeviceId;
    for (const OutputDevice& device : g_devices) {
        signature += L'|';
        signature += device.id;
    }

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

                    IAudioMeterInformation* meter = nullptr;
                    if (g_settings.showPeakMeters &&
                        SUCCEEDED(control->QueryInterface(
                            __uuidof(IAudioMeterInformation),
                            (void**)&meter))) {
                        it->meters.push_back(meter);
                    }

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
    g_devices.clear();
    g_defaultDeviceId.clear();

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
        g_defaultDeviceId = GetDeviceId(defaultDevice);

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

            IAudioMeterInformation* meter = nullptr;
            if (g_settings.showPeakMeters &&
                SUCCEEDED(defaultDevice->Activate(
                    __uuidof(IAudioMeterInformation), CLSCTX_ALL, nullptr,
                    (void**)&meter))) {
                entry.meters.push_back(meter);
            }

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
                AddOutputDevice(device);
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
            ReleaseEntry(app);
            continue;
        }

        g_entries.push_back(std::move(app));
    }

    for (MixerEntry& entry : g_entries) {
        ReadEntry(entry);
    }
}

// Returns the key of the mixer entry of the app that owns the given window,
// or an empty string.
std::wstring GetAppKeyForWindow(HWND hAppWnd) {
    if (!hAppWnd) {
        return {};
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hAppWnd, &pid);

    // Store apps are hosted in a frame window of another process, the real app
    // process owns one of the child windows.
    WCHAR szClassName[64];
    if (GetClassName(hAppWnd, szClassName, ARRAYSIZE(szClassName)) &&
        _wcsicmp(szClassName, L"ApplicationFrameWindow") == 0) {
        struct Param {
            DWORD hostPid;
            DWORD appPid;
        } param = {pid, 0};
        EnumChildWindows(
            hAppWnd,
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
        return {};
    }

    std::wstring path = GetProcessPath(pid);
    if (path.empty()) {
        return L"pid:" + std::to_wstring(pid);
    }

    return ToLower(path);
}

// Returns the mixer entry of the app that owns the given window.
MixerEntry* FindEntryForWindow(HWND hAppWnd) {
    std::wstring key = GetAppKeyForWindow(hAppWnd);
    return key.empty() ? nullptr : FindEntry(key);
}

// Collects the apps that have audio sessions, on all playback devices. The
// caller releases the entries.
void CollectAppEntries(std::vector<MixerEntry>& apps) {
    if (!g_pDeviceEnumerator) {
        HRESULT hr = CoCreateInstance(
            __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IMMDeviceEnumerator), (void**)&g_pDeviceEnumerator);
        if (FAILED(hr)) {
            g_pDeviceEnumerator = nullptr;
            return;
        }
    }

    IMMDeviceCollection* devices = nullptr;
    if (FAILED(g_pDeviceEnumerator->EnumAudioEndpoints(
            eRender, DEVICE_STATE_ACTIVE, &devices))) {
        return;
    }

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

// Returns the mixer entry of the app that owns the foreground window.
MixerEntry* FindForegroundEntry() {
    return FindEntryForWindow(GetForegroundWindow());
}

#pragma endregion  // audio

#pragma region mixer_popup

const WCHAR kMixerClassName[] = L"Windhawk_AppVolumeMixer_" WH_MOD_ID;

// Layout, in device-independent pixels.
constexpr int kWidth = 380;
constexpr int kRowHeight = 44;
constexpr int kFooterRowHeight = 32;
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

// Taskbar thread only. Set when the mod is being unloaded, so that a late
// scroll can't create the popup again.
bool g_unloading;

bool g_mixerClassRegistered;
HWND g_mixerWnd;
UINT g_mixerDpi = 96;
POINT g_anchorPt;
HWND g_anchorTaskbarWnd;
int g_hoverRow = -1;
// The output device rows under the apps: the devices if the list is open,
// then the row of the default device.
int g_hoverFooterRow = -1;
bool g_deviceListOpen;
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

// The foreground window at the time the popup was shown.
HWND g_foregroundWndAtShow;

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

int MixerFooterRowCount() {
    if (!g_settings.showDeviceRow || g_devices.empty()) {
        return 0;
    }

    return 1 + (g_deviceListOpen ? (int)g_devices.size() : 0);
}

int MixerFooterTop() {
    return Scale(kPadY) + Scale(kRowHeight) * MixerRowCount();
}

int MixerHeight() {
    return MixerFooterTop() + Scale(kFooterRowHeight) * MixerFooterRowCount() +
           Scale(kPadY);
}

RECT MixerFooterRowRect(int row) {
    RECT rc;
    rc.left = 0;
    rc.right = Scale(kWidth);
    rc.top = MixerFooterTop() + Scale(kFooterRowHeight) * row;
    rc.bottom = rc.top + Scale(kFooterRowHeight);
    return rc;
}

int MixerFooterRowFromPoint(POINT ptClient) {
    int y = ptClient.y - MixerFooterTop();
    if (y < 0 || ptClient.x < 0 || ptClient.x >= Scale(kWidth)) {
        return -1;
    }

    int row = y / Scale(kFooterRowHeight);
    return row < MixerFooterRowCount() ? row : -1;
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
    g_hoverFooterRow = -1;
    g_deviceListOpen = false;
    ReleaseEntries();
    g_devices.clear();
    g_defaultDeviceId.clear();
    g_deviceNameCache.clear();
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

// Width of the level meter of a row, for the given width of the filled bar.
int MixerMeterWidth(const MixerEntry& entry, int fillWidth) {
    if (entry.muted || entry.peak <= 0.0f) {
        return 0;
    }

    return std::clamp((int)lroundf(fillWidth * entry.peak), 0, fillWidth);
}

// Reads the level meters. Returns true if a meter has to be redrawn.
bool UpdatePeaks() {
    if (!g_settings.showPeakMeters) {
        return false;
    }

    int barLeft, barRight;
    MixerBarExtent(&barLeft, &barRight);

    bool changed = false;
    for (MixerEntry& entry : g_entries) {
        float peak = 0.0f;
        for (IAudioMeterInformation* meter : entry.meters) {
            float value = 0.0f;
            if (SUCCEEDED(meter->GetPeakValue(&value))) {
                peak = std::max(peak, value);
            }
        }

        int fillWidth = MulDiv(barRight - barLeft, entry.pos, 100);
        int prevWidth = MixerMeterWidth(entry, fillWidth);

        // Rise at once, fall slowly, so the meter doesn't flicker.
        entry.peak = std::clamp(std::max(peak, entry.peak * 0.6f), 0.0f, 1.0f);
        if (entry.peak < 0.01f) {
            entry.peak = 0.0f;
        }

        if (MixerMeterWidth(entry, fillWidth) != prevWidth) {
            changed = true;
        }
    }

    return changed;
}

// Returns true if the user clicked or switched to something else while the
// mouse is outside of the popup. Has to be called on each timer tick: reading
// the buttons also clears their "was pressed" state.
bool ClickedOutsideMixer(bool mouseInside) {
    bool pressed = false;
    for (int vk : {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON}) {
        if (GetAsyncKeyState(vk) & 0x8001) {
            pressed = true;
        }
    }

    if (!g_settings.closeOnClickOutside || mouseInside) {
        return false;
    }

    return pressed || GetForegroundWindow() != g_foregroundWndAtShow;
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
    COLORREF clrMeter = g_darkTheme ? RGB(108, 203, 95) : RGB(15, 123, 15);

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

        // Level meter, a thin line under the bar. It's as long as the sound
        // is loud, and never longer than the filled part of the bar.
        int meterWidth = MixerMeterWidth(entry, fillRight - barLeft);
        if (meterWidth > 0) {
            RECT rcMeter = {barLeft, rcTrack.bottom + Scale(3),
                            barLeft + meterWidth,
                            rcTrack.bottom + Scale(3) + std::max(1, Scale(2))};
            FillSolidRect(hdc, rcMeter, clrMeter);
        }

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

    // Output device rows.
    int footerRows = MixerFooterRowCount();
    if (footerRows > 0) {
        RECT rcLine = MixerFooterRowRect(0);
        rcLine.left = Scale(kPadX);
        rcLine.right = Scale(kWidth - kPadX);
        rcLine.bottom = rcLine.top + 1;
        FillSolidRect(hdc, rcLine, clrBorder);
    }

    for (int i = 0; i < footerRows; i++) {
        bool isDefaultRow = i == footerRows - 1;
        const OutputDevice* device =
            isDefaultRow ? FindDefaultDevice() : &g_devices[i];
        bool isDefault = device && device->id == g_defaultDeviceId;
        RECT rcRow = MixerFooterRowRect(i);

        if (i == g_hoverFooterRow) {
            RECT rc = rcRow;
            InflateRect(&rc, -Scale(4), -Scale(1));
            FillSolidRect(hdc, rc, clrHover);
        }

        RECT rcGlyph = rcRow;
        rcGlyph.left = Scale(kPadX);
        rcGlyph.right = rcGlyph.left + Scale(kIconSize);
        WCHAR glyph = isDefaultRow ? kGlyphDevice : isDefault ? kGlyphCheck : 0;
        if (glyph) {
            SelectObject(hdc, glyphFont);
            SetTextColor(hdc, isDefaultRow ? clrText : clrAccent);
            DrawText(hdc, &glyph, 1, &rcGlyph,
                     DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
            SelectObject(hdc, textFont);
        }

        RECT rcName = rcRow;
        rcName.left = rcGlyph.right + Scale(kGap);
        rcName.right = Scale(kWidth - kPadX - kIconSize - kGap);
        SetTextColor(hdc, isDefaultRow || isDefault ? clrText : clrDimText);
        DrawText(hdc, device ? device->name.c_str() : L"Output device", -1,
                 &rcName,
                 DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);

        if (isDefaultRow && g_devices.size() > 1) {
            RECT rcChevron = rcRow;
            rcChevron.right = Scale(kWidth - kPadX);
            rcChevron.left = rcChevron.right - Scale(kIconSize);
            WCHAR chevron =
                g_deviceListOpen ? kGlyphChevronDown : kGlyphChevronUp;
            SelectObject(hdc, glyphFont);
            SetTextColor(hdc, clrDimText);
            DrawText(hdc, &chevron, 1, &rcChevron,
                     DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
            SelectObject(hdc, textFont);
        }
    }

    SelectObject(hdc, prevFont);
}

void PositionMixer(bool show);

// Rebuilds and repaints the popup after the list of rows changed.
void RefreshMixerLayout() {
    g_hoverRow = -1;
    g_hoverFooterRow = -1;
    PositionMixer(false);
    InvalidateRect(g_mixerWnd, nullptr, FALSE);
}

// Switches to the next (1) or the previous (-1) playback device.
void StepDefaultDevice(int dir) {
    int count = (int)g_devices.size();
    for (int i = 0; i < count; i++) {
        if (g_devices[i].id != g_defaultDeviceId) {
            continue;
        }

        int next = std::clamp(i + dir, 0, count - 1);
        if (next != i) {
            std::wstring id = g_devices[next].id;
            SetDefaultDevice(id);
            RebuildEntries();
            RefreshMixerLayout();
        }

        return;
    }
}

// A click on one of the output device rows.
void OnMixerFooterClick(int row) {
    int footerRows = MixerFooterRowCount();
    if (row == footerRows - 1) {
        if (g_devices.size() > 1) {
            g_deviceListOpen = !g_deviceListOpen;
        }
    } else if (row >= 0 && row < (int)g_devices.size()) {
        std::wstring id = g_devices[row].id;
        g_deviceListOpen = false;
        if (id != g_defaultDeviceId) {
            SetDefaultDevice(id);
            RebuildEntries();
        }
    }

    RefreshMixerLayout();
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
    bool mouseInside = dragging || PtInRect(&rc, pt);
    if (ClickedOutsideMixer(mouseInside)) {
        HideMixer();
        return;
    }

    if (mouseInside) {
        g_idleMs = 0;
    } else {
        g_idleMs += kMixerTimerInterval;
        if (g_idleMs >= g_settings.popupTimeout) {
            HideMixer();
            return;
        }
    }

    if (dragging) {
        if (UpdatePeaks()) {
            InvalidateRect(g_mixerWnd, nullptr, FALSE);
        }

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
        int prevHeight = MixerHeight();

        // Keep the meter levels, so the meters don't jump on a rebuild.
        std::unordered_map<std::wstring, float> prevPeaks;
        for (const MixerEntry& entry : g_entries) {
            prevPeaks[entry.key] = entry.peak;
        }

        RebuildEntries();

        for (MixerEntry& entry : g_entries) {
            auto it = prevPeaks.find(entry.key);
            if (it != prevPeaks.end()) {
                entry.peak = it->second;
            }
        }

        if (MixerHeight() != prevHeight) {
            PositionMixer(false);

            POINT ptClient = pt;
            ScreenToClient(g_mixerWnd, &ptClient);
            GetWindowRect(g_mixerWnd, &rc);
            bool inside = PtInRect(&rc, pt);
            g_hoverRow = inside ? MixerRowFromPoint(ptClient) : -1;
            g_hoverFooterRow = inside ? MixerFooterRowFromPoint(ptClient) : -1;
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

    if (UpdatePeaks()) {
        changed = true;
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
            int footerRow = MixerFooterRowFromPoint(pt);
            if (row != g_hoverRow || footerRow != g_hoverFooterRow) {
                g_hoverRow = row;
                g_hoverFooterRow = footerRow;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            g_trackingMouse = false;
            if (g_dragKey.empty() &&
                (g_hoverRow != -1 || g_hoverFooterRow != -1)) {
                g_hoverRow = -1;
                g_hoverFooterRow = -1;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;

        case WM_MOUSEWHEEL: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hWnd, &pt);
            int row = MixerRowFromPoint(pt);
            int clicks = WheelDeltaToClicks(GET_WHEEL_DELTA_WPARAM(wParam));
            if (MixerFooterRowFromPoint(pt) >= 0) {
                if (clicks) {
                    StepDefaultDevice(clicks < 0 ? 1 : -1);
                }
                return 0;
            }

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
            if (int footerRow = MixerFooterRowFromPoint(pt); footerRow >= 0) {
                OnMixerFooterClick(footerRow);
                return 0;
            }

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

// The module of the mod itself, which owns MixerWndProc.
HINSTANCE GetModInstance() {
    HINSTANCE hInstance = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&MixerWndProc, &hInstance);
    return hInstance;
}

bool EnsureMixerWindow() {
    if (g_mixerWnd) {
        return true;
    }

    if (!g_mixerClassRegistered) {
        WNDCLASS wc{};
        wc.lpfnWndProc = MixerWndProc;
        wc.hInstance = GetModInstance();
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
        L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, GetModInstance(),
        nullptr);
    if (!g_mixerWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return false;
    }

    // Rounded corners on Windows 11. Fails harmlessly on Windows 10.
    DWM_WINDOW_CORNER_PREFERENCE cornerPreference = DWMWCP_ROUND;
    DwmSetWindowAttribute(g_mixerWnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                          &cornerPreference, sizeof(cornerPreference));

    return true;
}

// Shows the popup, or keeps it open if it's already shown.
bool ShowMixer(HWND hTaskbarWnd, POINT pt) {
    if (g_unloading || !EnsureMixerWindow()) {
        return false;
    }

    // Scrolling over the taskbar of another monitor: move the popup there.
    if (IsWindowVisible(g_mixerWnd) && hTaskbarWnd != g_anchorTaskbarWnd) {
        HideMixer();
    }

    g_idleMs = 0;

    if (!IsWindowVisible(g_mixerWnd)) {
        g_mixerDpi = GetDpiForWindowWithFallback(hTaskbarWnd);
        g_anchorPt = pt;
        g_anchorTaskbarWnd = hTaskbarWnd;
        g_darkTheme = IsTaskbarDarkTheme();
        g_hoverRow = -1;
        g_hoverFooterRow = -1;
        g_deviceListOpen = false;
        g_tickCount = 0;
        g_highlightKey.clear();

        // Forget the clicks made before the popup was shown.
        g_foregroundWndAtShow = GetForegroundWindow();
        ClickedOutsideMixer(true);

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

#pragma region task_buttons

// The InputSite window that is handling a wheel message right now.
HWND g_wheelInputSiteWnd;

#ifdef TASK_BUTTONS_SUPPORTED

// Finds the window of the taskbar button under the mouse, on the new taskbar
// of Windows 11. Based on the "Taskbar Volume Control Per-App" mod by m417z.

std::atomic<bool> g_taskbarDllHooked;
std::atomic<bool> g_taskButtonSymbolsReady;
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_taskbarViewDllHooked;

// The taskbar button under the mouse. Taskbar thread only.
void* g_hoverButtonThis;
winrt::weak_ref<wux::UIElement> g_hoverButton;

thread_local bool g_captureTaskGroup;
thread_local void* g_capturedTaskGroup;

// A fake click with this value as its options is caught by the
// CTaskListWnd::HandleClick hook, which gets the native task item or group.
WCHAR g_clickSentinel[] = L"click-sentinel";
void* g_clickSentinel_TaskGroup;
void* g_clickSentinel_TaskItem;

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow;

using CImmersiveTaskItem_GetAppWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetAppWindow_t CImmersiveTaskItem_GetAppWindow;

void* CImmersiveTaskItem_vftable;
void* CImmersiveTaskItem_vftable_ITaskItem;

using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void*(WINAPI*)(void** output, wux::UIElement* container);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

using TaskListWindowViewModel_get_TaskItem_t = int(WINAPI*)(void* pThis,
                                                            void** taskItem);
TaskListWindowViewModel_get_TaskItem_t
    TaskListWindowViewModel_get_TaskItem_Original;

using TryGetItemFromContainer_TaskListGroupViewModel_t =
    void*(WINAPI*)(void** output, wux::UIElement* container);
TryGetItemFromContainer_TaskListGroupViewModel_t
    TryGetItemFromContainer_TaskListGroupViewModel_Original;

using TaskListGroupViewModel_IsMultiWindow_t = bool(WINAPI*)(void* pThis);
TaskListGroupViewModel_IsMultiWindow_t
    TaskListGroupViewModel_IsMultiWindow_Original;

using ITaskGroup_IsRunning_t = bool(WINAPI*)(void* pThis);
ITaskGroup_IsRunning_t ITaskGroup_IsRunning_Original;
bool WINAPI ITaskGroup_IsRunning_Hook(void* pThis) {
    if (g_captureTaskGroup) {
        g_capturedTaskGroup = *(void**)pThis;
        return false;
    }

    return ITaskGroup_IsRunning_Original(pThis);
}

using CTaskGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems;

HDPA GetTaskItemsArray(void* taskGroup) {
    // CTaskGroup::GetNumItems is just DPA_GetPtrCount(this->taskItemsArray),
    // or in other words *(int*)this[taskItemsArrayOffset]. Called with an
    // array of pointers to ints instead of a real object, it returns the
    // offset of the array member.
    static size_t offset = []() {
        constexpr int kIntArraySize = 256;
        int arrayOfInts[kIntArraySize];
        int* arrayOfIntPtrs[kIntArraySize];
        for (int i = 0; i < kIntArraySize; i++) {
            arrayOfInts[i] = i;
            arrayOfIntPtrs[i] = &arrayOfInts[i];
        }

        return CTaskGroup_GetNumItems(arrayOfIntPtrs);
    }();

    return (HDPA)((void**)taskGroup)[offset];
}

using CTaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis,
                                                    void* taskGroup,
                                                    void* taskItem,
                                                    void** launcherOptions);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;
HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                             void* taskGroup,
                                             void* taskItem,
                                             void** launcherOptions) {
    if (*launcherOptions == &g_clickSentinel) {
        g_clickSentinel_TaskGroup = taskGroup;
        g_clickSentinel_TaskItem = taskItem;
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                             launcherOptions);
}

using TaskItem_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskItem_ReportClicked_t TaskItem_ReportClicked_Original;

using TaskGroup_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskGroup_ReportClicked_t TaskGroup_ReportClicked_Original;

HWND GetWindowFromTaskItem(void* taskItem) {
    if (!taskItem) {
        return nullptr;
    }

    if (*(void**)taskItem == CImmersiveTaskItem_vftable_ITaskItem) {
        void* immersiveTaskItem =
            QueryViaVtable(taskItem, CImmersiveTaskItem_vftable);
        return CImmersiveTaskItem_GetAppWindow(immersiveTaskItem);
    }

    return CWindowTaskItem_GetWindow(taskItem);
}

// Returns a window of the app of a taskbar button: the window of the button,
// or the first window of its group.
HWND GetWindowFromTaskListButton(wux::UIElement element) {
    winrt::com_ptr<IUnknown> windowViewModel;
    TryGetItemFromContainer_TaskListWindowViewModel_Original(
        windowViewModel.put_void(), &element);
    if (windowViewModel) {
        winrt::com_ptr<IUnknown> windowsUdkTaskItem;
        TaskListWindowViewModel_get_TaskItem_Original(
            windowViewModel.get(), windowsUdkTaskItem.put_void());
        if (windowsUdkTaskItem) {
            g_clickSentinel_TaskItem = nullptr;
            TaskItem_ReportClicked_Original(windowsUdkTaskItem.get(),
                                            &g_clickSentinel);
            if (HWND hWnd = GetWindowFromTaskItem(g_clickSentinel_TaskItem)) {
                return hWnd;
            }
        }
    }

    winrt::com_ptr<IUnknown> groupViewModel;
    TryGetItemFromContainer_TaskListGroupViewModel_Original(
        groupViewModel.put_void(), &element);
    if (!groupViewModel) {
        return nullptr;
    }

    g_capturedTaskGroup = nullptr;
    g_captureTaskGroup = true;
    TaskListGroupViewModel_IsMultiWindow_Original((void**)groupViewModel.get() -
                                                  1);
    g_captureTaskGroup = false;
    if (!g_capturedTaskGroup) {
        return nullptr;
    }

    g_clickSentinel_TaskGroup = nullptr;
    TaskGroup_ReportClicked_Original(g_capturedTaskGroup, &g_clickSentinel);
    if (!g_clickSentinel_TaskGroup) {
        return nullptr;
    }

    HDPA taskItemsArray = GetTaskItemsArray(g_clickSentinel_TaskGroup);
    if (!taskItemsArray || DPA_GetPtrCount(taskItemsArray) <= 0) {
        return nullptr;
    }

    return GetWindowFromTaskItem(DPA_GetPtr(taskItemsArray, 0));
}

void ClearHoveredTaskButton() {
    g_hoverButtonThis = nullptr;
    g_hoverButton = nullptr;
}

using TaskListButton_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskListButton_OnPointerMoved_t TaskListButton_OnPointerMoved_Original;
int WINAPI TaskListButton_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    // Remember the button only when the mouse enters another one.
    if (pThis != g_hoverButtonThis) {
        wux::UIElement element = nullptr;
        ((IUnknown*)pThis)
            ->QueryInterface(winrt::guid_of<wux::UIElement>(),
                             winrt::put_abi(element));
        if (element &&
            winrt::get_class_name(element) == L"Taskbar.TaskListButton") {
            g_hoverButtonThis = pThis;
            g_hoverButton = element;
        }
    }

    return TaskListButton_OnPointerMoved_Original(pThis, pArgs);
}

using TaskListButton_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskListButton_OnPointerExited_t TaskListButton_OnPointerExited_Original;
int WINAPI TaskListButton_OnPointerExited_Hook(void* pThis, void* pArgs) {
    if (pThis == g_hoverButtonThis) {
        ClearHoveredTaskButton();
    }

    return TaskListButton_OnPointerExited_Original(pThis, pArgs);
}

// Returns a window of the app whose taskbar button is under the given point,
// in screen coordinates. Returns nullptr if the point isn't over a button.
HWND GetHoveredTaskButtonWindow(POINT ptScreen) {
    if (!g_taskbarDllHooked || !g_taskbarViewDllHooked ||
        !g_wheelInputSiteWnd) {
        return nullptr;
    }

    try {
        auto element = g_hoverButton.get();
        auto frameworkElement =
            element ? element.try_as<wux::FrameworkElement>() : nullptr;
        auto xamlRoot = element ? element.XamlRoot() : nullptr;
        if (!frameworkElement || !xamlRoot) {
            return nullptr;
        }

        // The remembered button can be out of date, check that the point is
        // really inside of it. The InputSite window covers the XAML content.
        double scale = xamlRoot.RasterizationScale();
        POINT ptClient = ptScreen;
        if (scale <= 0 || !ScreenToClient(g_wheelInputSiteWnd, &ptClient)) {
            return nullptr;
        }

        auto bounds = element.TransformToVisual(nullptr).TransformBounds(
            winrt::Windows::Foundation::Rect{
                0,
                0,
                static_cast<float>(frameworkElement.ActualWidth()),
                static_cast<float>(frameworkElement.ActualHeight()),
            });
        double x = ptClient.x / scale;
        double y = ptClient.y / scale;
        if (x < bounds.X || y < bounds.Y || x >= bounds.X + bounds.Width ||
            y >= bounds.Y + bounds.Height) {
            return nullptr;
        }

        return GetWindowFromTaskListButton(element);
    } catch (...) {
        ClearHoveredTaskButton();
        return nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Volume slider in the jump list of a taskbar button. Based on the "Taskbar
// Jump List Volume Control" mod by Ara. The jump list is shown by
// ShellExperienceHost.exe, so the mod is loaded into both processes: explorer
// finds the app and changes its volume, ShellExperienceHost adds the slider.

// Window in explorer.exe. ShellExperienceHost sends volume changes to it.
constexpr WCHAR kJumpHostWndClass[] = L"Windhawk_JumpListVolumeHost_" WH_MOD_ID;
// Window in ShellExperienceHost.exe. explorer sends the app's volume to it.
constexpr WCHAR kJumpShellWndClass[] =
    L"Windhawk_JumpListVolumeShell_" WH_MOD_ID;

constexpr UINT kMsgSetVolume = WM_APP + 0x51;  // wParam: 0-100.
constexpr UINT kMsgSetMute = WM_APP + 0x52;    // wParam: 0/1.

constexpr ULONG_PTR kCopyDataMagic = 0x4D564A4C;

struct JumpListVolumeInfo {
    ULONGLONG hostWnd;
    DWORD hasAudio;
    DWORD volume;  // Slider position, 0-100.
    DWORD muted;
};

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{
        .proc = proc,
        .procParam = procParam,
    };
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

// explorer.exe, taskbar thread only: the audio sessions of the app whose jump
// list was opened last.
MixerEntry g_jumpListEntry;
HWND g_jumpHostWnd;
bool g_jumpHostWndClassRegistered;

LRESULT CALLBACK JumpHostWndProc(HWND hWnd,
                                 UINT uMsg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    switch (uMsg) {
        case kMsgSetVolume:
            SetEntryPos(g_jumpListEntry, wParam > 100 ? 100 : (int)wParam);
            return 0;

        case kMsgSetMute:
            SetEntryMute(g_jumpListEntry, wParam != 0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void EnsureJumpHostWindow() {
    if (g_jumpHostWnd) {
        return;
    }

    if (!g_jumpHostWndClassRegistered) {
        WNDCLASS wc{};
        wc.lpfnWndProc = JumpHostWndProc;
        wc.hInstance = GetModInstance();
        wc.lpszClassName = kJumpHostWndClass;
        if (!RegisterClass(&wc)) {
            Wh_Log(L"RegisterClass failed: %u", GetLastError());
            return;
        }

        g_jumpHostWndClassRegistered = true;
    }

    g_jumpHostWnd = CreateWindowEx(0, kJumpHostWndClass, nullptr, 0, 0, 0, 0, 0,
                                   HWND_MESSAGE, nullptr, GetModInstance(),
                                   nullptr);
    if (!g_jumpHostWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return;
    }

    // ShellExperienceHost runs in an AppContainer, allow its messages.
    ChangeWindowMessageFilterEx(g_jumpHostWnd, kMsgSetVolume, MSGFLT_ALLOW,
                                nullptr);
    ChangeWindowMessageFilterEx(g_jumpHostWnd, kMsgSetMute, MSGFLT_ALLOW,
                                nullptr);
}

// Must be called from the taskbar thread.
void DestroyJumpListHost() {
    ReleaseEntry(g_jumpListEntry);

    if (g_jumpHostWnd) {
        DestroyWindow(g_jumpHostWnd);
        g_jumpHostWnd = nullptr;
    }
}

void OnShowJumpView(void* taskGroup) {
    EnsureJumpHostWindow();
    if (!g_jumpHostWnd) {
        return;
    }

    ReleaseEntry(g_jumpListEntry);

    // The apps of the windows of the button, usually a single one.
    std::unordered_set<std::wstring> keys;
    if (HDPA taskItemsArray = taskGroup ? GetTaskItemsArray(taskGroup)
                                        : nullptr) {
        int count = DPA_GetPtrCount(taskItemsArray);
        for (int i = 0; i < count; i++) {
            std::wstring key = GetAppKeyForWindow(
                GetWindowFromTaskItem(DPA_GetPtr(taskItemsArray, i)));
            if (!key.empty()) {
                keys.insert(std::move(key));
            }
        }
    }

    if (!keys.empty()) {
        std::vector<MixerEntry> apps;
        CollectAppEntries(apps);
        for (MixerEntry& app : apps) {
            if (keys.contains(app.key)) {
                for (ISimpleAudioVolume* session : app.sessions) {
                    g_jumpListEntry.sessions.push_back(session);
                }

                app.sessions.clear();
            }

            ReleaseEntry(app);
        }
    }

    // The process paths are only kept while the popup is open.
    if (!g_mixerWnd || !IsWindowVisible(g_mixerWnd)) {
        g_pidPathCache.clear();
    }

    JumpListVolumeInfo info{};
    info.hostWnd = (ULONGLONG)(ULONG_PTR)g_jumpHostWnd;
    info.hasAudio = !g_jumpListEntry.sessions.empty();
    if (info.hasAudio) {
        ReadEntry(g_jumpListEntry);
        info.volume = g_jumpListEntry.pos;
        info.muted = g_jumpListEntry.muted;
    }

    HWND shellWnd =
        FindWindowEx(HWND_MESSAGE, nullptr, kJumpShellWndClass, nullptr);
    if (!shellWnd) {
        Wh_Log(L"ShellExperienceHost window not found");
        return;
    }

    // Sent synchronously so that the slider is in place before the jump list
    // is measured and shown.
    COPYDATASTRUCT cds{};
    cds.dwData = kCopyDataMagic;
    cds.cbData = sizeof(info);
    cds.lpData = &info;
    DWORD_PTR result = 0;
    if (!SendMessageTimeout(shellWnd, WM_COPYDATA, (WPARAM)g_jumpHostWnd,
                            (LPARAM)&cds, SMTO_ABORTIFHUNG, 500, &result)) {
        Wh_Log(L"SendMessageTimeout failed: %u", GetLastError());
    }
}

using CTaskListWnd_ShowJumpView_t = HRESULT(WINAPI*)(void* pThis,
                                                     void* taskGroup,
                                                     void* taskItem,
                                                     bool param3);
CTaskListWnd_ShowJumpView_t CTaskListWnd_ShowJumpView_Original;
HRESULT WINAPI CTaskListWnd_ShowJumpView_Hook(void* pThis,
                                              void* taskGroup,
                                              void* taskItem,
                                              bool param3) {
    if (g_settings.jumpListSlider && g_taskbarDllHooked && !g_unloading) {
        OnShowJumpView(taskGroup);
    }

    return CTaskListWnd_ShowJumpView_Original(pThis, taskGroup, taskItem,
                                              param3);
}

void UnregisterJumpHostClass() {
    if (g_jumpHostWndClassRegistered) {
        UnregisterClass(kJumpHostWndClass, GetModInstance());
        g_jumpHostWndClassRegistered = false;
    }
}

// ShellExperienceHost.exe: the slider in the jump list.
namespace JumpListShell {

using namespace winrt::Windows::UI::Xaml;

constexpr WCHAR kGlyphVolume[] = L"\uE767";
constexpr WCHAR kGlyphMute[] = L"\uE74F";

constexpr UINT_PTR kPollTimerId = 1;
constexpr UINT kPollIntervalMs = 50;
constexpr int kPollCount = 40;

HANDLE g_shellThread;
std::atomic<HWND> g_shellWnd;
std::atomic<HWND> g_volumeHostWnd;

// Accessed from the mod's window thread, and from the jump list UI thread
// while the former is blocked waiting for it.
struct {
    bool hasAudio = false;
    int volume = 0;
    bool muted = false;
    DWORD seq = 0;
} g_info;

// Only accessed from the mod's window thread.
HWND g_jumpListWnd;
int g_pollsLeft;
bool g_appliedForSeq;

enum class AttachMode {
    None,
    Header,  // Set as the header of the system items list.
    Panel,   // Inserted next to the list in its parent panel.
};

// Only accessed from the jump list UI thread. Holds strong XAML references,
// which are released on the UI thread by RemoveVolumeUi(), never by the
// automatic destructor at process exit.
struct UiState {
    AttachMode mode = AttachMode::None;
    winrt::weak_ref<Controls::ListViewBase> list;
    winrt::event_token layoutUpdatedToken;
    int layoutCount = 0;
    Controls::Grid root = nullptr;
    Controls::Slider slider = nullptr;
    Controls::Button muteButton = nullptr;
    Controls::TextBlock percentText = nullptr;
    Controls::FontIcon muteIcon = nullptr;
    winrt::event_token sliderValueChangedToken;
    winrt::event_token muteButtonClickToken;
    winrt::event_token wheelChangedToken;
    // Panel mode.
    Controls::Panel parentPanel = nullptr;
    Controls::StackPanel wrapper = nullptr;
    bool muted = false;
    bool updating = false;
    int wheelDeltaAccumulator = 0;
    DWORD appliedSeq = 0;
};
[[clang::no_destroy]] UiState g_ui;

// The thread which created the elements of g_ui, 0 if there are none. Only
// written on that thread, but read by the mod's window thread on unload.
std::atomic<DWORD> g_uiThreadId;

bool g_headerUnsupported;

void UpdateVolumeTexts() {
    int value = (int)std::lround(g_ui.slider.Value());
    g_ui.percentText.Text(winrt::to_hstring(value) + L"%");
    g_ui.muteIcon.Glyph(g_ui.muted || value == 0 ? kGlyphMute : kGlyphVolume);
}

void CreateVolumeUi() {
    Controls::FontIcon muteIcon;
    muteIcon.FontFamily(
        Media::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    muteIcon.FontSize(16);

    Controls::Button muteButton;
    muteButton.Content(muteIcon);
    muteButton.Width(32);
    muteButton.Height(32);
    muteButton.Padding(ThicknessHelper::FromUniformLength(0));
    muteButton.BorderThickness(ThicknessHelper::FromUniformLength(0));
    muteButton.Background(
        Media::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
    muteButton.VerticalAlignment(VerticalAlignment::Center);

    Controls::Slider slider;
    slider.Minimum(0);
    slider.Maximum(100);
    slider.StepFrequency(1);
    slider.MinWidth(150);
    slider.IsThumbToolTipEnabled(false);
    slider.VerticalAlignment(VerticalAlignment::Center);
    slider.HorizontalAlignment(HorizontalAlignment::Stretch);

    Controls::TextBlock percentText;
    percentText.FontSize(12);
    percentText.MinWidth(36);
    percentText.TextAlignment(TextAlignment::Right);
    percentText.VerticalAlignment(VerticalAlignment::Center);

    Controls::Grid root;
    root.Padding(ThicknessHelper::FromLengths(8, 4, 12, 4));
    root.ColumnSpacing(8);
    root.HorizontalAlignment(HorizontalAlignment::Stretch);
    // Lets the whole row receive mouse wheel events.
    root.Background(
        Media::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));

    Controls::ColumnDefinition column0;
    column0.Width(GridLengthHelper::Auto());
    Controls::ColumnDefinition column1;
    column1.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
    Controls::ColumnDefinition column2;
    column2.Width(GridLengthHelper::Auto());
    root.ColumnDefinitions().Append(column0);
    root.ColumnDefinitions().Append(column1);
    root.ColumnDefinitions().Append(column2);

    Controls::Grid::SetColumn(muteButton, 0);
    Controls::Grid::SetColumn(slider, 1);
    Controls::Grid::SetColumn(percentText, 2);
    root.Children().Append(muteButton);
    root.Children().Append(slider);
    root.Children().Append(percentText);

    g_ui.sliderValueChangedToken = slider.ValueChanged(
        [](auto&&,
           Controls::Primitives::RangeBaseValueChangedEventArgs const& e) {
            if (!g_ui.slider) {
                return;
            }

            int value = (int)std::lround(e.NewValue());
            if (!g_ui.updating) {
                if (value > 0) {
                    g_ui.muted = false;
                }

                if (!PostMessage(g_volumeHostWnd, kMsgSetVolume, value, 0)) {
                    Wh_Log(L"PostMessage failed: %u", GetLastError());
                }
            }

            UpdateVolumeTexts();
        });

    g_ui.muteButtonClickToken = muteButton.Click([](auto&&, auto&&) {
        if (!g_ui.slider) {
            return;
        }

        g_ui.muted = !g_ui.muted;
        if (!PostMessage(g_volumeHostWnd, kMsgSetMute, g_ui.muted, 0)) {
            Wh_Log(L"PostMessage failed: %u", GetLastError());
        }

        UpdateVolumeTexts();
    });

    g_ui.wheelChangedToken = root.PointerWheelChanged(
        [](auto&&, Input::PointerRoutedEventArgs const& e) {
            if (!g_ui.slider) {
                return;
            }

            int delta =
                e.GetCurrentPoint(nullptr).Properties().MouseWheelDelta();
            if (delta) {
                // Precision touchpads send many deltas smaller than
                // WHEEL_DELTA. Accumulate them, and step once per notch.
                if ((g_ui.wheelDeltaAccumulator > 0) != (delta > 0)) {
                    g_ui.wheelDeltaAccumulator = 0;
                }

                g_ui.wheelDeltaAccumulator += delta;
                int notches = g_ui.wheelDeltaAccumulator / WHEEL_DELTA;
                g_ui.wheelDeltaAccumulator -= notches * WHEEL_DELTA;

                if (notches) {
                    // The same step and snap as in the mixer popup.
                    int pos = (int)std::lround(g_ui.slider.Value());
                    g_ui.slider.Value(StepPos(pos, notches));
                }
            }

            e.Handled(true);
        });

    g_ui.root = root;
    g_ui.slider = slider;
    g_ui.muteButton = muteButton;
    g_ui.percentText = percentText;
    g_ui.muteIcon = muteIcon;
    g_uiThreadId = GetCurrentThreadId();
}

void DetachVolumeUi() {
    auto list = g_ui.list.get();

    if (list && g_ui.layoutUpdatedToken) {
        list.LayoutUpdated(g_ui.layoutUpdatedToken);
    }

    if (g_ui.mode == AttachMode::Header) {
        if (list && list.Header() == g_ui.root) {
            list.Header(nullptr);
        }
    } else if (g_ui.mode == AttachMode::Panel && g_ui.parentPanel) {
        auto children = g_ui.parentPanel.Children();
        uint32_t index = 0;
        if (g_ui.wrapper) {
            // Put the list back in place of the wrapper.
            if (list && children.IndexOf(g_ui.wrapper, index)) {
                g_ui.wrapper.Children().Clear();
                children.RemoveAt(index);
                Controls::Grid::SetRow(list,
                                       Controls::Grid::GetRow(g_ui.wrapper));
                Controls::Grid::SetColumn(
                    list, Controls::Grid::GetColumn(g_ui.wrapper));
                Controls::Grid::SetRowSpan(
                    list, Controls::Grid::GetRowSpan(g_ui.wrapper));
                Controls::Grid::SetColumnSpan(
                    list, Controls::Grid::GetColumnSpan(g_ui.wrapper));
                children.InsertAt(index, list);
            }
        } else if (children.IndexOf(g_ui.root, index)) {
            children.RemoveAt(index);
        }
    }

    g_ui.mode = AttachMode::None;
    g_ui.list = nullptr;
    g_ui.layoutUpdatedToken = {};
    g_ui.layoutCount = 0;
    g_ui.parentPanel = nullptr;
    g_ui.wrapper = nullptr;
}

// Must be called on the thread that created the elements, see g_uiThreadId.
void RemoveVolumeUi() {
    DetachVolumeUi();

    // XAML can keep a detached element alive for a while (focus, pointer
    // capture during a drag), the handlers must not outlive the mod.
    if (g_ui.slider && g_ui.sliderValueChangedToken) {
        g_ui.slider.ValueChanged(g_ui.sliderValueChangedToken);
    }
    if (g_ui.muteButton && g_ui.muteButtonClickToken) {
        g_ui.muteButton.Click(g_ui.muteButtonClickToken);
    }
    if (g_ui.root && g_ui.wheelChangedToken) {
        g_ui.root.PointerWheelChanged(g_ui.wheelChangedToken);
    }

    g_ui.sliderValueChangedToken = {};
    g_ui.muteButtonClickToken = {};
    g_ui.wheelChangedToken = {};
    g_ui.root = nullptr;
    g_ui.slider = nullptr;
    g_ui.muteButton = nullptr;
    g_ui.percentText = nullptr;
    g_ui.muteIcon = nullptr;
    g_ui.wheelDeltaAccumulator = 0;
    g_ui.appliedSeq = 0;
    g_uiThreadId = 0;
}

bool AttachVolumeUiToPanel(Controls::ListViewBase list) {
    auto parentPanel =
        Media::VisualTreeHelper::GetParent(list).try_as<Controls::Panel>();
    if (!parentPanel) {
        return false;
    }

    auto children = parentPanel.Children();
    uint32_t index = 0;
    if (!children.IndexOf(list, index)) {
        return false;
    }

    auto stackPanel = parentPanel.try_as<Controls::StackPanel>();
    if (stackPanel &&
        stackPanel.Orientation() == Controls::Orientation::Vertical) {
        children.InsertAt(index, g_ui.root);
    } else {
        // Wrap the list and the slider with a panel which takes the place of
        // the list.
        Controls::StackPanel wrapper;
        Controls::Grid::SetRow(wrapper, Controls::Grid::GetRow(list));
        Controls::Grid::SetColumn(wrapper, Controls::Grid::GetColumn(list));
        Controls::Grid::SetRowSpan(wrapper, Controls::Grid::GetRowSpan(list));
        Controls::Grid::SetColumnSpan(wrapper,
                                      Controls::Grid::GetColumnSpan(list));

        children.RemoveAt(index);
        wrapper.Children().Append(g_ui.root);
        wrapper.Children().Append(list);
        children.InsertAt(index, wrapper);

        g_ui.wrapper = wrapper;
    }

    g_ui.parentPanel = parentPanel;
    g_ui.mode = AttachMode::Panel;
    return true;
}

void AttachVolumeUi(Controls::ListViewBase list) {
    g_ui.list = list;

    if (!g_headerUnsupported && !list.Header()) {
        list.Header(g_ui.root);
        g_ui.mode = AttachMode::Header;
        g_ui.layoutCount = 0;
        g_ui.layoutUpdatedToken =
            list.LayoutUpdated([](auto&&, auto&&) { g_ui.layoutCount++; });
        Wh_Log(L"Attached as list header");
        return;
    }

    if (AttachVolumeUiToPanel(list)) {
        Wh_Log(L"Attached to the parent panel (%s)",
               winrt::get_class_name(g_ui.parentPanel).c_str());
    } else {
        Wh_Log(L"Couldn't attach, parent is %s",
               winrt::get_class_name(Media::VisualTreeHelper::GetParent(list))
                   .c_str());
    }
}

Controls::ListViewBase FindSystemItemList(DependencyObject object, int depth) {
    if (!object || depth > 24) {
        return nullptr;
    }

    if (winrt::get_class_name(object) == L"JumpViewUI.SystemItemListView") {
        return object.try_as<Controls::ListViewBase>();
    }

    int count = Media::VisualTreeHelper::GetChildrenCount(object);
    for (int i = 0; i < count; i++) {
        if (auto found = FindSystemItemList(
                Media::VisualTreeHelper::GetChild(object, i), depth + 1)) {
            return found;
        }
    }

    return nullptr;
}

Controls::ListViewBase FindSystemItemListInCurrentWindow() {
    auto window = Window::Current();
    if (!window) {
        return nullptr;
    }

    if (auto found = FindSystemItemList(window.Content(), 0)) {
        return found;
    }

    for (auto popup : Media::VisualTreeHelper::GetOpenPopups(window)) {
        if (auto found = FindSystemItemList(popup.Child(), 0)) {
            return found;
        }
    }

    return nullptr;
}

// Returns true if the current thread hosts the jump list.
bool ApplyOnUiThread() {
    auto list = FindSystemItemListInCurrentWindow();
    if (!list) {
        return false;
    }

    // The elements can't be touched from another thread, and can't be released
    // from it either. Leave them as they are, rather than crash.
    DWORD uiThreadId = g_uiThreadId;
    if (uiThreadId && uiThreadId != GetCurrentThreadId()) {
        Wh_Log(L"Jump list is on thread %u, the volume UI is on thread %u",
               GetCurrentThreadId(), uiThreadId);
        return false;
    }

    // The header is not shown by this list's template, use the other way.
    if (g_ui.mode == AttachMode::Header && g_ui.layoutCount >= 3 &&
        !Media::VisualTreeHelper::GetParent(g_ui.root)) {
        Wh_Log(L"List header isn't displayed, switching to panel mode");
        g_headerUnsupported = true;
        DetachVolumeUi();
    }

    bool attached = g_ui.mode != AttachMode::None && g_ui.list.get() == list &&
                    (g_ui.mode != AttachMode::Header ||
                     list.Header() == g_ui.root);
    if (!attached) {
        if (g_ui.root) {
            DetachVolumeUi();
        } else {
            CreateVolumeUi();
        }

        AttachVolumeUi(list);
        g_ui.appliedSeq = 0;
    }

    // Applied once per jump list, the user might be dragging the slider.
    if (g_ui.appliedSeq != g_info.seq) {
        g_ui.appliedSeq = g_info.seq;
        g_ui.muted = g_info.muted;
        g_ui.updating = true;
        g_ui.slider.Value(g_info.volume);
        g_ui.updating = false;
        UpdateVolumeTexts();
        g_ui.root.Visibility(g_info.hasAudio ? Visibility::Visible
                                              : Visibility::Collapsed);
    }

    return true;
}

bool ApplyOnWindowThread(HWND hWnd) {
    bool applied = false;
    RunFromWindowThread(
        hWnd,
        [](void* param) {
            try {
                *(bool*)param = ApplyOnUiThread();
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"Error %08X: %s", (UINT)e.code(), e.message().c_str());
            } catch (...) {
                Wh_Log(L"Unknown error");
            }
        },
        &applied);
    return applied;
}

std::vector<HWND> GetCoreWindows() {
    std::vector<HWND> windows;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& windows = *(std::vector<HWND>*)lParam;

            DWORD pid = 0;
            WCHAR className[64];
            DWORD threadId = GetWindowThreadProcessId(hWnd, &pid);
            if (!threadId || pid != GetCurrentProcessId() ||
                !GetClassName(hWnd, className, ARRAYSIZE(className)) ||
                _wcsicmp(className, L"Windows.UI.Core.CoreWindow") != 0) {
                return TRUE;
            }

            // One window per UI thread is enough.
            for (HWND other : windows) {
                if (GetWindowThreadProcessId(other, nullptr) == threadId) {
                    return TRUE;
                }
            }

            windows.push_back(hWnd);
            return TRUE;
        },
        (LPARAM)&windows);

    return windows;
}

bool ApplyToJumpList() {
    if (g_jumpListWnd && IsWindow(g_jumpListWnd) &&
        ApplyOnWindowThread(g_jumpListWnd)) {
        return true;
    }

    g_jumpListWnd = nullptr;

    for (HWND hWnd : GetCoreWindows()) {
        if (ApplyOnWindowThread(hWnd)) {
            g_jumpListWnd = hWnd;
            return true;
        }
    }

    return false;
}

// For troubleshooting, in case the jump list wasn't found.
void LogCoreWindows() {
    for (HWND hWnd : GetCoreWindows()) {
        WCHAR title[128] = L"";
        GetWindowText(hWnd, title, ARRAYSIZE(title));
        Wh_Log(L"CoreWindow %p: %s", hWnd, title);

        RunFromWindowThread(
            hWnd,
            [](void*) {
                try {
                    auto window = Window::Current();
                    auto content = window ? window.Content() : nullptr;
                    Wh_Log(L"  Content: %s",
                           content ? winrt::get_class_name(content).c_str()
                                   : L"(none)");
                } catch (...) {
                    Wh_Log(L"  No XAML window");
                }
            },
            nullptr);
    }
}

LRESULT CALLBACK ShellWndProc(HWND hWnd,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam) {
    switch (uMsg) {
        case WM_COPYDATA: {
            auto* cds = (const COPYDATASTRUCT*)lParam;
            if (cds->dwData != kCopyDataMagic ||
                cds->cbData != sizeof(JumpListVolumeInfo)) {
                return FALSE;
            }

            auto* info = (const JumpListVolumeInfo*)cds->lpData;
            g_volumeHostWnd = (HWND)(ULONG_PTR)info->hostWnd;
            g_info.hasAudio = info->hasAudio;
            g_info.volume = info->volume > 100 ? 100 : info->volume;
            g_info.muted = info->muted;
            g_info.seq++;

            // The jump list might be recreated when shown, keep applying for
            // a while.
            g_appliedForSeq = ApplyToJumpList();
            g_pollsLeft = kPollCount;
            SetTimer(hWnd, kPollTimerId, kPollIntervalMs, nullptr);
            return TRUE;
        }

        case WM_TIMER:
            if (wParam == kPollTimerId) {
                if (ApplyToJumpList()) {
                    g_appliedForSeq = true;
                }

                if (--g_pollsLeft <= 0) {
                    KillTimer(hWnd, kPollTimerId);
                    if (!g_appliedForSeq) {
                        Wh_Log(L"Jump list not found");
                        LogCoreWindows();
                    }
                }
            }
            return 0;

        case WM_CLOSE:
            KillTimer(hWnd, kPollTimerId);

            // Clean up on the thread which owns the elements. It's not
            // necessarily the thread of the last known jump list window, which
            // is reset whenever the jump list isn't found.
            if (DWORD uiThreadId = g_uiThreadId) {
                for (HWND coreWnd : GetCoreWindows()) {
                    if (GetWindowThreadProcessId(coreWnd, nullptr) ==
                        uiThreadId) {
                        RunFromWindowThread(
                            coreWnd,
                            [](void*) {
                                try {
                                    RemoveVolumeUi();
                                } catch (...) {
                                    Wh_Log(L"Removing the volume UI failed");
                                }
                            },
                            nullptr);
                        break;
                    }
                }
            }

            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// The parameter is an event which is signaled once the window is created, or
// once it's known that it can't be.
DWORD WINAPI ShellThreadProc(LPVOID readyEvent) {
    WNDCLASS wc{};
    wc.lpfnWndProc = ShellWndProc;
    wc.hInstance = GetModInstance();
    wc.lpszClassName = kJumpShellWndClass;
    if (!RegisterClass(&wc)) {
        Wh_Log(L"RegisterClass failed: %u", GetLastError());
        SetEvent((HANDLE)readyEvent);
        return 1;
    }

    HWND hWnd = CreateWindowEx(0, kJumpShellWndClass, nullptr, 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!hWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        UnregisterClass(kJumpShellWndClass, wc.hInstance);
        SetEvent((HANDLE)readyEvent);
        return 1;
    }

    g_shellWnd = hWnd;
    SetEvent((HANDLE)readyEvent);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    g_shellWnd = nullptr;
    UnregisterClass(kJumpShellWndClass, wc.hInstance);
    return 0;
}

}  // namespace JumpListShell

// Called from Wh_ModInit in ShellExperienceHost.exe.
void InitJumpListShell() {
    if (!g_settings.jumpListSlider) {
        return;
    }

    HANDLE readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        return;
    }

    JumpListShell::g_shellThread = CreateThread(
        nullptr, 0, JumpListShell::ShellThreadProc, readyEvent, 0, nullptr);
    if (JumpListShell::g_shellThread) {
        // Signaled by the thread after creating its window, or on failure.
        // This guarantees that the unload code can always reach the window.
        WaitForSingleObject(readyEvent, INFINITE);
    }

    CloseHandle(readyEvent);

    if (JumpListShell::g_shellThread && !JumpListShell::g_shellWnd) {
        // The thread failed to create its window and is exiting.
        WaitForSingleObject(JumpListShell::g_shellThread, INFINITE);
        CloseHandle(JumpListShell::g_shellThread);
        JumpListShell::g_shellThread = nullptr;
    }
}

// Called from Wh_ModUninit in ShellExperienceHost.exe.
void UninitJumpListShell() {
    if (HWND shellWnd = JumpListShell::g_shellWnd) {
        PostMessage(shellWnd, WM_CLOSE, 0, 0);
    }

    if (JumpListShell::g_shellThread) {
        // No timeout: the mod's code must not be unloaded while the thread is
        // still running.
        WaitForSingleObject(JumpListShell::g_shellThread, INFINITE);
        CloseHandle(JumpListShell::g_shellThread);
        JumpListShell::g_shellThread = nullptr;
    }
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskListButton_OnPointerMoved_Original,
            TaskListButton_OnPointerMoved_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &TaskListButton_OnPointerExited_Original,
            TaskListButton_OnPointerExited_Hook,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListWindowViewModel_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &TaskListWindowViewModel_get_TaskItem_Original,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListGroupViewModel_Original,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::IsMultiWindow(void)const )"},
            &TaskListGroupViewModel_IsMultiWindow_Original,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskGroup<struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::IsRunning(void)const )"},
            &ITaskGroup_IsRunning_Original,
            ITaskGroup_IsRunning_Hook,
        },
    };

    return WindhawkUtils::HookSymbols(module, symbolHooks,
                                      ARRAYSIZE(symbolHooks));
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return false;
    }

    // The first five are needed by both features. The others are optional,
    // so that one feature can work without the other.
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
            &CTaskGroup_GetNumItems,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetAppWindow(void))"},
            &CImmersiveTaskItem_GetAppWindow,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable')"},
            &CImmersiveTaskItem_vftable,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable_ITaskItem,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskGroup,struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::ReportClicked(void *))"},
            &TaskGroup_ReportClicked_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::ShowJumpView(struct ITaskGroup *,struct ITaskItem *,bool))"},
            &CTaskListWnd_ShowJumpView_Original,
            CTaskListWnd_ShowJumpView_Hook,
            true,
        },
    };

    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

// Hooks the taskbar view module once it's loaded. Returns true if new hooks
// were set and have to be applied.
bool HookTaskbarViewDllIfLoaded() {
    if (!g_taskbarDllHooked || !g_taskButtonSymbolsReady ||
        !g_settings.taskButtonScroll || g_taskbarViewDllLoaded) {
        return false;
    }

    HMODULE module = GetTaskbarViewModuleHandle();
    if (!module || g_taskbarViewDllLoaded.exchange(true)) {
        return false;
    }

    if (!HookTaskbarViewDllSymbols(module)) {
        Wh_Log(L"Taskbar button scrolling is off: no taskbar view symbols");
        return false;
    }

    g_taskbarViewDllHooked = true;
    return true;
}

// Called from Wh_ModInit. A failure only turns the features off.
void InitTaskButtons() {
    if (!g_newTaskbar ||
        (!g_settings.taskButtonScroll && !g_settings.jumpListSlider)) {
        return;
    }

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"Taskbar buttons and jump lists are off: no taskbar.dll "
               L"symbols");
        return;
    }

    g_taskButtonSymbolsReady = CTaskListWnd_HandleClick_Original &&
                               TaskItem_ReportClicked_Original &&
                               TaskGroup_ReportClicked_Original;
    if (!g_taskButtonSymbolsReady) {
        Wh_Log(L"Taskbar button scrolling is off: missing symbols");
    }

    if (!CTaskListWnd_ShowJumpView_Original) {
        Wh_Log(L"The jump list slider is off: missing symbols");
    }

    g_taskbarDllHooked = true;
    HookTaskbarViewDllIfLoaded();
}

#else  // !TASK_BUTTONS_SUPPORTED

void ClearHoveredTaskButton() {}

void DestroyJumpListHost() {}

void UnregisterJumpHostClass() {}

void InitJumpListShell() {}

void UninitJumpListShell() {}

HWND GetHoveredTaskButtonWindow(POINT ptScreen) {
    return nullptr;
}

bool HookTaskbarViewDllIfLoaded() {
    return false;
}

void InitTaskButtons() {}

#endif  // TASK_BUTTONS_SUPPORTED

#pragma endregion  // task_buttons

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

    // The app of the taskbar button under the mouse, if any.
    HWND hButtonAppWnd = GetHoveredTaskButtonWindow(pt);

    if (!ShowMixer(hWnd, pt)) {
        return false;
    }

    int clicks = WheelDeltaToClicks(GET_WHEEL_DELTA_WPARAM(wParam));

    // An app without sound has no row, the scroll then works as usual.
    MixerEntry* entry = FindEntryForWindow(hButtonAppWnd);
    if (!entry) {
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
                ClearHoveredTaskButton();
                DestroyJumpListHost();
                DestroyMixer();
            } else {
                g_secondaryTaskbarWindows.erase(hWnd);
            }
            break;

        default:
            if (uMsg == g_uninitMsg && hWnd == g_hTaskbarWnd) {
                g_unloading = true;
                ClearHoveredTaskButton();
                DestroyJumpListHost();
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
                IsTaskbarWindow(hRootWnd)) {
                g_wheelInputSiteWnd = hWnd;
                bool handled = OnMouseWheel(hRootWnd, wParam, lParam);
                g_wheelInputSiteWnd = nullptr;
                if (handled) {
                    return 0;
                }
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

    if (module && HookTaskbarViewDllIfLoaded()) {
        Wh_ApplyHookOperations();
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
    g_settings.showDeviceRow = Wh_GetIntSetting(L"showDeviceRow");
    g_settings.showPeakMeters = Wh_GetIntSetting(L"showPeakMeters");
    g_settings.taskButtonScroll = Wh_GetIntSetting(L"taskButtonScroll");
    g_settings.jumpListSlider = Wh_GetIntSetting(L"jumpListSlider");
    g_settings.popupTimeout =
        std::clamp(Wh_GetIntSetting(L"popupTimeout"), 300, 60000);
    g_settings.closeOnClickOutside = Wh_GetIntSetting(L"closeOnClickOutside");

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

    WCHAR processPath[MAX_PATH];
    if (GetModuleFileName(nullptr, processPath, ARRAYSIZE(processPath))) {
        PCWSTR processName = wcsrchr(processPath, L'\\');
        processName = processName ? processName + 1 : processPath;
        g_isShellExperienceHost =
            _wcsicmp(processName, L"ShellExperienceHost.exe") == 0;
    }

    if (g_isShellExperienceHost) {
        // Only the jump list slider lives in this process.
        InitJumpListShell();
        return TRUE;
    }

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

    InitTaskButtons();

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_isShellExperienceHost) {
        return;
    }

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    HandleLoadedExplorerPatcher();

    if (HookTaskbarViewDllIfLoaded()) {
        Wh_ApplyHookOperations();
    }

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

    if (g_isShellExperienceHost) {
        UninitJumpListShell();
        return;
    }

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
        UnregisterClass(kMixerClassName, GetModInstance());
    }

    UnregisterJumpHostClass();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;
    bool prevTaskButtonScroll = g_settings.taskButtonScroll;
    bool prevJumpListSlider = g_settings.jumpListSlider;

    LoadSettings();

    // The taskbar button and jump list hooks are only set when the mod is
    // loaded.
    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11 ||
               g_settings.taskButtonScroll != prevTaskButtonScroll ||
               g_settings.jumpListSlider != prevJumpListSlider;

    return TRUE;
}
