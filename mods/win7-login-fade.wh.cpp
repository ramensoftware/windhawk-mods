// ==WindhawkMod==
// @id              win7-login-fade
// @name            Logon & Sleep Fade Restorer
// @description     Bring back the old logon screen and sleep fade effect
// @version         1.6
// @author          Ingan121
// @github          https://github.com/Ingan121
// @twitter         https://twitter.com/Ingan121
// @homepage        https://www.ingan121.com/
// @include         winlogon.exe
// @include         dwm.exe
// @include         LogonUI.exe
// @include         LockApp.exe
// @include         *
// @compilerOptions -lgdi32 -lntdll
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Logon & Sleep Fade Restorer
* This mod can restore the old fade effect from older Windows versions when logging on, logging out, unlocking, sleeping, hibernating, and turning off the monitor.
## Mod settings explanation
* The logon/unlock and logoff/shutdown fade types can be set independently, so you can have different fade types and durations for those two if you want.
* There are multiple logon and logoff fade types to choose from:
  * None: No fade effect; the screen will switch instantly.
  * Gamma: The classic gamma-ramp-based fade effect, which works by adjusting the screen brightness using the SetDeviceGammaRamp function, the same method used by the original fade effect. On Windows 10 1809 and below, this mode will use the existing kernel-mode fade logic instead of a reimplementation.
  * DWM (Original): Keep the original DWM fade effect to make the fade work as if this mod is disabled. Duration is not adjustable in this mode. Not available for logoff/shutdown.
* The gamma-based fade types will not work with Microsoft Basic Display Adapter, VMware SVGA 3D, and some other display drivers that do not support gamma adjustment. It's also not compatible with NVIDIA driver's reference color mode.
* To add fade effects to sleep and hibernation initiated by the idle timer or power button, enable the "Enable enhanced sleep/hibernate interception" option. This option has limited compatibility compared to the rest of the mod, and is only tested on Windows 10 LTSC 2021 (22H2) and 11 25H2. It may work on 21H2 and later versions; however, this mode is unlikely to work on Windows 10 1903 and earlier versions, unfortunately.
* The rest of the features have been tested on Windows 8.1, 10 LTSC 2021 (21H2, 22H2), 11 23H2, 24H2, and 25H2. This mod will not work on Windows 7.
## ⚠ Important usage note ⚠
* On Windows 10 1903 and newer versions, this mod needs a special procedure to get the fade effect to work naturally. Please do either one of the following:
  * Set the `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\ICM\GdiIcmGammaRange` registry value to `0x100`** (DWORD, 256 in decimal)
  * Navigate to Windhawk's Settings > Advanced settings > More advanced settings > Process inclusion list, and make sure that `dwm.exe` is in the list. (See the screenshot below)
* After doing so, do one of these: reboot, sign out and sign in again, enter the "switch user" screen (not lock) once, or restart DWM.
* Failure to do so may result in an unnatural fade effect that doesn't make the screen fully black.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)

* Neither of those is required on Windows 10 1809 or earlier.
* To allow fading in the logon screen when turning off the monitor with the idle timer, you must allow Windhawk to inject into **LogonUI.exe**. Follow the above `dwm.exe` steps for the instructions. (replace `dwm` with `LogonUI`)

## Presets
* Windows Vista/7 style (mod defaults)
  * Logon fade type: Gamma
  * Logon fade duration: 1000 ms
  * Logoff fade type: Gamma
  * Logoff fade duration: 1000 ms
  * Sleep fade enabled: true
  * Sleep fade duration: 500 ms
* Windows 8/8.1/10 1507-1809 style
  * Logon fade type: DWM (Original)
  * Logon fade duration: (ignored in this mode)
  * Logoff fade type: Gamma
  * Logoff fade duration: 334 ms
  * Sleep fade enabled: true
  * Sleep fade duration: 167 ms
## Known issues and limitations
* The logon fade animation may look broken on early boot when using the auto-login feature.
* Turning off the monitor with the power button action (or sleeping on Modern Standby devices) will not trigger the sleep fade effect, as it is handled internally by the kernel without notifying a user-mode component before initiating the action.
  * Monitor off initiated by the idle timer can have fade added as usual.
* The monitor off fade effect in the Windows 10+ lock screen may have a brief flash at the end of the fade when turning off the monitor.
* On 32-bit systems, only the monitor off and sleep/hibernation fades are supported, as this mod was never tested on 32-bit Windows systems.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/* 
- type: gamma
  $name: Logon fade type
  $name:ko-KR: 로그인 페이드 유형
  $options:
    - none: None
    - gamma: Gamma
    - dwm: DWM (Original)
  $options:ko-KR:
    - none: 없음
    - gamma: 감마
    - dwm: DWM (원본)
  $description: "Choose the type of fade effect to use for logon and unlock.\nSee the mod details page for details on each type."
  $description:ko-KR: "로그온 및 잠금 해제에 사용할 페이드 효과 유형을 선택합니다.\n각 유형에 대한 자세한 내용은 모드 세부 정보 페이지를 참조하세요."
- duration: 1000
  $name: Logon fade duration (ms)
  $name:ko-KR: 로그인 페이드 지속 시간 (ms)
  $description: Set the duration of the fade effect when logging on and unlocking.
  $description:ko-KR: 로그온 및 잠금 해제 시 페이드 효과의 지속 시간을 설정합니다.
- logoffType: gamma
  $name: Logoff fade type
  $name:ko-KR: 로그오프 페이드 유형
  $options:
    - none: None
    - gamma: Gamma
  $options:ko-KR:
    - none: 없음
    - gamma: 감마
  $description: "Choose the type of fade effect to use for logoff and lock.\nSee the mod details page for details on each type."
  $description:ko-KR: "로그오프 및 잠금에 사용할 페이드 효과 유형을 선택합니다.\n각 유형에 대한 자세한 내용은 모드 세부 정보 페이지를 참조하세요."
- logoffDuration: 1000
  $name: Logoff fade duration (ms)
  $name:ko-KR: 로그오프 페이드 지속 시간 (ms)
  $description: Set the duration of the fade effect when logging off and locking.
  $description:ko-KR: 로그오프 및 잠금 시 페이드 효과의 지속 시간을 설정합니다.
- sleepFadeEnabled: true
  $name: Sleep fade
  $name:ko-KR: 절전 페이드
  $description: Enable the fade effect for sleeping, hibernating, and turning off the monitor. Turning this off will also disable the global injection of this mod, making it only run in winlogon.exe.
  $description:ko-KR: 절전, 최대 절전 모드, 모니터 끄기에 대한 페이드 효과를 활성화합니다. 이 옵션을 끄면 이 모드의 글로벌 인젝션도 비활성화되어 winlogon.exe에서만 실행됩니다.
- sleepDuration: 500
  $name: Sleep fade duration (ms)
  $name:ko-KR: 절전 페이드 지속 시간 (ms)
  $description: Set the duration of the fade effect when sleeping, hibernating, and turning off the monitor.
  $description:ko-KR: 절전, 최대 절전 모드, 모니터 끄기 시 페이드 효과의 지속 시간을 설정합니다.
- enhancedSleepIntercept: auto
  $name: Enable enhanced sleep/hibernate interception
  $name:ko-KR: 향상된 절전/최대 절전 모드 가로채기 활성화
  $description: "Adds fade effects to sleep and hibernation by intercepting the messages between Winlogon and the kernel, which allows the fade effect to be applied to every sleep/hibernate action, including the kernel-initiated idle timer and power button actions. This option has limited compatibility; see mod details page for more information."
  $description:ko-KR: "Winlogon과 커널 간의 메시지를 가로채 절전/최대 절전 모드 진입 시 페이드 효과를 추가합니다. 이를 통해 커널에서 시작된 유휴 타이머 및 전원 버튼 동작을 포함하여 모든 절전/최대 절전 모드 작업에 페이드 효과를 적용할 수 있습니다. 이 옵션은 호환성이 높지 않으므로 모드 세부 정보 페이지를 확인하시기 바랍니다."
  $options:
    - auto: Automatic (enabled on 21H2 and newer)
    - always: Enabled
    - never: Disabled
  $options:ko-KR:
    - auto: 자동 (21H2 이후 버전에서 활성화)
    - always: 활성화
    - never: 비활성화
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <atomic>
#include <sddl.h>

#ifdef _WIN64
#define ENABLE_WINLOGON_HOOKS
#endif

#define WM_MONOFFTASK (WM_USER + 1)
#define OVERLAY_WIN_CLASS L"LogonSleepFadeOverlay"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

EXTERN_C NTSYSAPI NTSTATUS NTAPI RtlGetVersion(PRTL_OSVERSIONINFOW lpVersionInformation);

enum FadeType {
    None,
    Gamma,
    DWM
};

struct settings {
    FadeType logonType = Gamma;
    int logonDuration = 1000;
    FadeType logoffType = Gamma;
    int logoffDuration = 1000;
    bool sleepFadeEnabled = true;
    int sleepDuration = 500;
    bool enhancedSleepIntercept = false;
} g_settings;

bool g_isWinlogon = false;
bool g_isDwm = false;
bool g_isModernGamma = false;
bool g_is21H2OrNewer = false;
HANDLE g_fadeMutex = NULL;
HANDLE g_fadeEvent = NULL;
HANDLE g_stopEvent = NULL;
std::atomic<bool> g_isFadeInProgress = false;
std::atomic<bool> g_isExiting = false;
std::atomic<HANDLE> g_monitorOffThread = NULL;
std::atomic<HANDLE> g_asyncFadeThread = NULL;

typedef struct _MONITOR_INFO {
    HDC hDC = NULL;
    WORD origGamma[3][256];
} MONITOR_INFO;

struct GlobalFadeData {
    int monitorCount;
    MONITOR_INFO* monitors;
} g_monitorOffFadeData, g_sleepFadeData;

bool IsFadeInProgressInThisProcess() {
    return g_isFadeInProgress.load();
}

bool IsFadeInProgress() {
    if (IsFadeInProgressInThisProcess()) {
        return true;
    }
    if (!g_fadeMutex) {
        return false;
    }
    DWORD result = WaitForSingleObject(g_fadeMutex, 0);
    if (result == WAIT_OBJECT_0 || result == WAIT_ABANDONED) {
        ReleaseMutex(g_fadeMutex);
        return false;
    } else {
        if (result != WAIT_TIMEOUT) {
            Wh_Log(L"WaitForSingleObject failed, GLE=%d", GetLastError());
        }
        return true;
    }
}

bool BeginFade() {
    if (g_isExiting.load() || IsFadeInProgressInThisProcess()) {
        return false;
    }
    if (!g_fadeMutex) {
        if (g_isWinlogon) {
            g_isFadeInProgress.store(true);
            return true;
        } else {
            // Should not be reachable I think
            return false;
        }
    }
    DWORD result = WaitForSingleObject(g_fadeMutex, 0);
    if (result == WAIT_OBJECT_0 || result == WAIT_ABANDONED) {
        g_isFadeInProgress.store(true);
        return true;
    } else {
        return false;
    }
}

void EndFade() {
    g_isFadeInProgress.store(false);
    if (g_fadeMutex) {
        ReleaseMutex(g_fadeMutex);
    }
}

void WaitForFade() {
    if (IsFadeInProgressInThisProcess()) {
        HANDLE monitorOffThread = g_monitorOffThread.exchange(NULL);
        if (monitorOffThread) {
            WaitForSingleObject(monitorOffThread, INFINITE);
            CloseHandle(monitorOffThread);
        } else {
            Sleep(std::max({g_settings.logonDuration, g_settings.logoffDuration, g_settings.sleepDuration}) + 1000);
        }
    } else if (g_fadeMutex) {
        DWORD result = WaitForSingleObject(g_fadeMutex, INFINITE);
        if (result == WAIT_OBJECT_0 || result == WAIT_ABANDONED) {
            ReleaseMutex(g_fadeMutex);
        }
    }
}

bool GetMonitorsInfo(MONITOR_INFO** monitors, int* monitorCount) {
    int monitorCountLocal = 0;
    MONITOR_INFO* monitorsLocal = NULL;
    DISPLAY_DEVICEW dd;
    dd.cb = sizeof(dd);
    for (DWORD i = 0; EnumDisplayDevicesW(NULL, i, &dd, 0); i++) {
        if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE) {
            monitorCountLocal++;
            MONITOR_INFO* monitorsRealloc = (MONITOR_INFO*)realloc(monitorsLocal, sizeof(MONITOR_INFO) * monitorCountLocal);
            if (!monitorsRealloc) {
                Wh_Log(L"Failed to allocate memory for monitors");
                if (monitorsLocal) {
                    for (int j = 0; j < monitorCountLocal - 1; j++) {
                        if (monitorsLocal[j].hDC) {
                            DeleteDC(monitorsLocal[j].hDC);
                        }
                    }
                    free(monitorsLocal);
                }
                return false;
            }
            monitorsLocal = monitorsRealloc;
            MONITOR_INFO* mi = &monitorsLocal[monitorCountLocal - 1];
            memset(mi, 0, sizeof(MONITOR_INFO));
            HDC hDC = CreateDCW(dd.DeviceName, NULL, NULL, NULL);
            if (hDC) {
                mi->hDC = hDC;
                if (!GetDeviceGammaRamp(hDC, mi->origGamma)) {
                    Wh_Log(L"GetDeviceGammaRamp failed for monitor %d", i);
                    DeleteDC(hDC);
                    mi->hDC = NULL;
                }
            } else {
                Wh_Log(L"CreateDCW failed for monitor %d", i);
            }
        }
        memset(&dd, 0, sizeof(dd));
        dd.cb = sizeof(dd);
    }
    *monitors = monitorsLocal;
    *monitorCount = monitorCountLocal;
    return true;
}

// brightness: 0: black, 256: normal, 256+: brighter
void SetBrightness(WORD brightness, MONITOR_INFO* monitors, int monitorCount) {
    for (int i = 0; i < monitorCount; i++) {
        if (monitors[i].hDC) {
            MONITOR_INFO* mi = &monitors[i];
            WORD newGamma[3][256];
            for (int j = 0; j < 256; j++) {
                newGamma[0][j] = (WORD)((mi->origGamma[0][j] * brightness) / 256);
                newGamma[1][j] = (WORD)((mi->origGamma[1][j] * brightness) / 256);
                newGamma[2][j] = (WORD)((mi->origGamma[2][j] * brightness) / 256);
            }
            SetDeviceGammaRamp(mi->hDC, newGamma);
        }
    }
}

void FadeDesktop(int fps, int duration, bool fadeOut, MONITOR_INFO* monitors, int monitorCount) {
    LARGE_INTEGER startTime;
    QueryPerformanceCounter(&startTime);
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    int frameStep = 1000 / fps;
    while (true) {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        double elapsedSeconds = (double)(currentTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
        double durationSeconds = duration / 1000.0;
        if (elapsedSeconds >= durationSeconds) {
            break;
        }
        double progress = elapsedSeconds / durationSeconds;
        if (fadeOut) {
            progress = 1.0 - progress;
        }
        WORD brightness = (WORD)(progress * 256);
        SetBrightness(brightness, monitors, monitorCount);
        Sleep(frameStep);
    }
    SetBrightness(fadeOut ? 0 : 256, monitors, monitorCount);
}

int GetDeviceRefreshRate() {
    HDC hdc = GetDC(NULL);
    if (!hdc) {
        Wh_Log(L"GetDC failed");
        return 60;
    }
    int refreshRate = GetDeviceCaps(hdc, VREFRESH);
    ReleaseDC(NULL, hdc);
    if (refreshRate <= 0) {
        refreshRate = 60;
    }
    // idk if this high refresh rate is possible but it would cause Sleep(0) above
    if (refreshRate >= 1000) {
        refreshRate = 500;
    }
    return refreshRate;
}

#ifdef ENABLE_WINLOGON_HOOKS
// flags: this third argument was introduced in Windows 8, and it apparently controls whether to use the DWM fade or not,
// but I did not find any code responsible for parsing the flags; only seen it being used by winlogon.exe. Known values:
// 53: 8.0, 8.1 login
// 64: 8.0, 8.1 lock/unlock
// 55: 10 1507, 1607, 1809, 1903, 21H2, 22H2, 11 22H2, 24H2, 25H2 login
// 65: 10 1507, 1607, 1809, 1903, 21H2, 22H2, 11 22H2, 24H2, 25H2 unlock
// 65: 10 1507, 1607 lock
// 0 or any other invalid/unsupported value: original kernel mode gamma fade
// (All of these are found from x64 iso/installation; idk about 32bit ones)
//
// The old kernel gamma is broken on 1903+ because of a new color management system borking [Gre|NtGdi]SetDeviceGammaRamp
// SetDeviceGammaRamp in gdi32[full].dll is now redirected to the 'modern' gamma manager in mscms.dll, which requires DWM to work
// This effectively nuked the kernel mode and DWMless gamma ramp functionality lol
//
// By the way, Windows Vista and later allowed processes running as SYSTEM to bypass the GdiIcmGammaRange check when calling SetDeviceGammaRamp,
// to allow LogonUI.exe (authui.dll) to perform the post-boot-orb fade effect on its own in user mode
// This behavior is also removed on 1903+ and now all processes must go through the gamma range check, so the registry tweak is necessary
//
// Instead of messing with winlogon.exe codes, this mod differentiates logon/unlock fade from logoff/shutdown one with the following condition:
// duration == 1000 (hardcoded in winlogon) and flags != 0 (Win8+ all use DWM fade for logon/unlock)
typedef __int64 __fastcall (*SwitchDesktopWithFade_t)(HDESK hDesktop, DWORD duration, DWORD flags);
SwitchDesktopWithFade_t SwitchDesktopWithFade_original;
__int64 __fastcall SwitchDesktopWithFade_hook(HDESK hDesktop, DWORD duration, DWORD flags) {
    Wh_Log(L"SwitchDesktopWithFade, duration=%d, flags=%d", duration, flags);
    // A zero duration is a plain SwitchDesktop call, which can reach this hook too under some configurations
    // (Windhawk 2.0 alpha 6+, or old Windows versions like 8.x), since both share the same underlying function.
    if (duration == 0) {
        return SwitchDesktopWithFade_original(hDesktop, duration, flags);
    }
    bool isLogonUnlock = duration == 1000 && flags != 0;
    int durationToUse = isLogonUnlock ? g_settings.logonDuration : g_settings.logoffDuration;
    if (isLogonUnlock) {
        switch (g_settings.logonType) {
            case Gamma: // Gamma
                if (!g_isModernGamma) {
                    // Use existing kernel mode gamma fade on pre-1903 systems
                    // Force the original kernel mode gamma fade by setting flags to 0
                    return SwitchDesktopWithFade_original(hDesktop, duration, 0);
                }
                if (!BeginFade()) {
                    return SwitchDesktopWithFade_original(hDesktop, 0, 0);
                }
                // Do the fade in this hook function
                break;
            case DWM: // DWM (Original)
                // Pass it as-is
                return SwitchDesktopWithFade_original(hDesktop, duration, flags);
            case None: // None
            default:
                return SwitchDesktopWithFade_original(hDesktop, 0, 0);
        }
    } else {
        switch (g_settings.logoffType) {
            case Gamma:
                if (!g_isModernGamma) {
                    return SwitchDesktopWithFade_original(hDesktop, duration, 0);
                }
                if (!BeginFade()) {
                    return SwitchDesktopWithFade_original(hDesktop, 0, 0);
                }
                break;
            case None:
            default:
                return SwitchDesktopWithFade_original(hDesktop, 0, 0);
        }
    }
    if (durationToUse <= 0) {
        EndFade();
        return SwitchDesktopWithFade_original(hDesktop, 0, 0);
    }

    int refreshRate = GetDeviceRefreshRate();
    int monitorCount = 0;
    MONITOR_INFO* monitors = NULL;
    if (!GetMonitorsInfo(&monitors, &monitorCount)) {
        EndFade();
        return SwitchDesktopWithFade_original(hDesktop, 0, 0);
    }

    FadeDesktop(refreshRate, durationToUse / 2, true, monitors, monitorCount);
    __int64 result = SwitchDesktopWithFade_original(hDesktop, 0, 0);
    FadeDesktop(refreshRate, durationToUse / 2, false, monitors, monitorCount);

    for (int i = 0; i < monitorCount; i++) {
        if (monitors[i].hDC) {
            DeleteDC(monitors[i].hDC);
        }
    }
    free(monitors);
    EndFade();

    // Original SwitchDesktopWithFade usually returns 1; probably it's also WINBOOL like SwitchDesktop
    return result;
}
#endif

#pragma region Hooks and functions for monitor off fading
EXTERN_C NTSYSAPI NTSTATUS NTAPI NtPowerInformation(POWER_INFORMATION_LEVEL, PVOID, ULONG, PVOID, ULONG);
using NtPowerInformation_t = decltype(&NtPowerInformation);
NtPowerInformation_t NtPowerInformation_original;

// Restore the pre-fade gamma ramps and turn off the monitor, and also clean up the monitor info data.
void RestoreGammaAndMonitorOff() {
    if (g_monitorOffFadeData.monitors) {
        for (int i = 0; i < g_monitorOffFadeData.monitorCount; i++) {
            if (g_monitorOffFadeData.monitors[i].hDC) {
                SetDeviceGammaRamp(g_monitorOffFadeData.monitors[i].hDC, g_monitorOffFadeData.monitors[i].origGamma);
                DeleteDC(g_monitorOffFadeData.monitors[i].hDC);
            }
        }
        free(g_monitorOffFadeData.monitors);
        g_monitorOffFadeData.monitors = NULL;
        g_monitorOffFadeData.monitorCount = 0;
    }
    NtPowerInformation_original(ScreenOff, NULL, 0, NULL, 0);
}

LRESULT WINAPI OverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            Wh_Log(L"OverlayWndProc WM_CREATE");
            return 0;
        case WM_MONOFFTASK: {
            Wh_Log(L"OverlayWndProc WM_MONOFFTASK");
            int refreshRate = GetDeviceRefreshRate();
            if (!GetMonitorsInfo(&g_monitorOffFadeData.monitors, &g_monitorOffFadeData.monitorCount)) {
                EndFade();
                NtPowerInformation_original(ScreenOff, NULL, 0, NULL, 0);
                return 1;
            }

            FadeDesktop(refreshRate, g_settings.sleepDuration, true, g_monitorOffFadeData.monitors, g_monitorOffFadeData.monitorCount);

            // Show the overlay after the fade, and schedule it to hide after 1.5 seconds
            SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
            if (!SetTimer(hWnd, 1, 1500, NULL)) {
                Wh_Log(L"SetTimer failed, GLE=%d", GetLastError());
                RestoreGammaAndMonitorOff();
                EndFade();
                return 1;
            }

            Sleep(100); // Needed to prevent flashing when monitor fade is started multiple times in the same process
            RestoreGammaAndMonitorOff();
            EndFade();
            return 0;
        }
        case WM_LBUTTONDOWN:
            Wh_Log(L"OverlayWndProc WM_LBUTTONDOWN");
            ShowWindow(hWnd, SW_HIDE);
            return 0;
        case WM_SETCURSOR:
            SetCursor(NULL);
            return 1;
        case WM_TIMER:
            KillTimer(hWnd, 1);
            ShowWindow(hWnd, SW_HIDE);
            return 0;
        case WM_DESTROY:
            Wh_Log(L"OverlayWndProc WM_DESTROY");
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

HWND CreateOverlayWindow() {
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = HINST_THISCOMPONENT;
    wc.lpszClassName = OVERLAY_WIN_CLASS;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed, GLE=%d", GetLastError());
        return NULL;
    }
    return CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, OVERLAY_WIN_CLASS, L"", WS_POPUP, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
}

DWORD WINAPI AsyncFadeThreadProc(LPVOID lpParameter) {
    if (!g_fadeEvent || !g_stopEvent) {
        Wh_SetIntValue(L"NoAsyncFade", 1);
        return 1;
    }

    Wh_DeleteValue(L"NoAsyncFade");

    HDESK hDefDesktop = OpenDesktopW(L"default", 0, FALSE, DESKTOP_CREATEWINDOW | DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS);
    if (hDefDesktop) {
        Wh_Log(L"OpenDesktopW OK");
        SetThreadDesktop(hDefDesktop);
    }

    HWND overlayWnd = CreateOverlayWindow();

    // Unfortunately, monitor off APIs are all asynchronous and there is no reliable way to determine when the monitor is actually off
    // If we immediately restore the gamma right after calling the API, the screen will briefly flash back to normal brightness before turning off
    // On the other side, waiting enough seconds and setting the gamma ramps after screens are turned off, has random failures that causes the screen to be stuck at black
    // Moreover, listening for monitor wake events with RegisterPowerSettingNotification still does not work,
    // as the exact timing for WM_POWERBROADCAST messages is pretty random and thus the gamma restore logic may run while the monitor is still off
    // So just show an black overlay window between the fade and the monitor off to cover up the potential flash, and hide the overlay after a fixed amount of time
    MSG msg;
    if (overlayWnd) {
        HANDLE waitHandles[2] = { g_fadeEvent, g_stopEvent };
        while (TRUE) {
            DWORD result = MsgWaitForMultipleObjects(
                2,
                waitHandles,
                FALSE,
                INFINITE,
                QS_ALLINPUT
            );
            if (result == WAIT_OBJECT_0) {
                Wh_Log(L"Received fade request");
                if (BeginFade()) PostMessageW(overlayWnd, WM_MONOFFTASK, 0, 0);
            } else if (result == WAIT_OBJECT_0 + 1) {
                DestroyWindow(overlayWnd);
            } else if (result == WAIT_OBJECT_0 + 2) {
                while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    if (msg.message == WM_QUIT) {
                        if (hDefDesktop) {
                            CloseDesktop(hDefDesktop);
                        }
                        return 0;
                    }
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            } else {
                Wh_Log(L"MsgWaitForMultipleObjects returned %d", result);
            }
        }
    } else {
        Wh_Log(L"Failed to create overlay window, GLE=%d", GetLastError());
    }
    Wh_SetIntValue(L"NoAsyncFade", 1);
    if (hDefDesktop) {
        CloseDesktop(hDefDesktop);
    }
    return 1;
}

bool CreateAsyncFadeThread() {
    if (!g_fadeEvent || !g_stopEvent) {
        Wh_SetIntValue(L"NoAsyncFade", 1);
        return false;
    }

    HANDLE asyncFadeThread = g_asyncFadeThread.exchange(NULL);
    if (asyncFadeThread) {
        // Only allow single startup
        return false;
    }
    asyncFadeThread = CreateThread(NULL, 0, AsyncFadeThreadProc, 0, 0, NULL);
    if (!asyncFadeThread) {
        Wh_Log(L"Failed to create async fade thread, GLE=%d", GetLastError());
        Wh_SetIntValue(L"NoAsyncFade", 1);
        return false;
    }
    g_asyncFadeThread.store(asyncFadeThread);
    Wh_Log(L"Created async fade thread");
    return true;
}

DWORD WINAPI MonitorOffThreadProc(LPVOID lpParameter) {
    if (BeginFade()) {
        Wh_Log(L"Initiating monitor off fade...");

        HWND overlayWnd = CreateOverlayWindow();
        if (overlayWnd) {
            PostMessageW(overlayWnd, WM_MONOFFTASK, 0, 0);

            // I don't want extra hassle of managing another thread for the overlay window lol; just make sure the following order of operations is correct:
            // Fade out -> Show overlay -> Restore gamma -> Monitor off -> Wait for monitor off with arbitrary delay -> Hide overlay
            MSG msg;
            while (GetMessageW(&msg, NULL, 0, 0) > 0) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (UnregisterClassW(OVERLAY_WIN_CLASS, HINST_THISCOMPONENT) == 0) {
                Wh_Log(L"UnregisterClassW failed, GLE=%d", GetLastError());
            }

            if (g_monitorOffFadeData.monitors) {
                // Somehow window message stuff failed
                Wh_Log(L"Overlay window message loop ended unexpectedly, restoring gamma ramps...");
                NtPowerInformation_original(ScreenOff, NULL, 0, NULL, 0);
            }
        } else {
            Wh_Log(L"Failed to create overlay window, GLE=%d", GetLastError());
            // Just do the fade without the overlay, which will make the screen flash
            FadeDesktop(GetDeviceRefreshRate(), g_settings.sleepDuration, true, g_monitorOffFadeData.monitors, g_monitorOffFadeData.monitorCount);
            RestoreGammaAndMonitorOff();
        }
        return 0;
    } else {
        NtPowerInformation_original(ScreenOff, NULL, 0, NULL, 0);
    }
    return 1;
}

bool CreateMonitorOffThread() {
    if (g_settings.sleepFadeEnabled && !IsFadeInProgress()) {
        // Both NtPowerInformation and DefWindowProc WM_SYSCOMMAND is asynchronous so do it in a separate thread
        HANDLE monitorOffThread = g_monitorOffThread.exchange(NULL);
        if (monitorOffThread) {
            // Clean up remaining handle (it shouldn't be running at least as IsFadeInProgress is checked above)
            DWORD result = WaitForSingleObject(monitorOffThread, 0);
            if (result != WAIT_OBJECT_0 && result != WAIT_ABANDONED) {
                if (result != WAIT_TIMEOUT) {
                    Wh_Log(L"WaitForSingleObject failed, GLE=%d", GetLastError());
                } else {
                    Wh_Log(L"Monitor off thread is still running");
                }
                g_monitorOffThread.store(monitorOffThread);
                return false;
            }
            CloseHandle(monitorOffThread);
        }
        monitorOffThread = CreateThread(NULL, 0, MonitorOffThreadProc, 0, 0, NULL);
        if (!monitorOffThread) {
            Wh_Log(L"Failed to create monitor off thread, GLE=%d", GetLastError());
            return false;
        }
        g_monitorOffThread.store(monitorOffThread);
        Wh_Log(L"Created monitor off thread");
        return true;
    }
    return false;
}

bool RequestFade() {
    HDESK hDesktop = GetThreadDesktop(GetCurrentThreadId());
    if (hDesktop) {
        wchar_t desktopName[256] = {};
        if (GetUserObjectInformationW(hDesktop, UOI_NAME, desktopName, sizeof(desktopName), nullptr)) {
            // Keep original in-process fade for secure desktop processes (e.g. LogonUI), as the async fade thread's overlay window is only shown on the default desktop
            // And secure desktop processes all run as SYSTEM which can also bypass the gamma range check on legacy gamma (1809-) systems
            if (_wcsicmp(desktopName, L"winlogon") == 0) {
                return CreateMonitorOffThread();
            }
        }
    }
    if (g_settings.sleepFadeEnabled && g_fadeEvent && !IsFadeInProgress() && !Wh_GetIntValue(L"NoAsyncFade", 0)) {
        return SetEvent(g_fadeEvent);
    }
    return false;
}

NTSTATUS NTAPI NtPowerInformation_hook(POWER_INFORMATION_LEVEL InformationLevel, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength) {
    // Undocumented API introduced in Windows 8, known usermode use cases include StartMenuExperienceHost and Open-Shell Menu (used for the sleep action on Modern Standby devices)
    // DefWindowProc WM_SYSCOMMAND SC_MONITORPOWER has it's own kernel mode routine and does not use this API, so these two must be hooked separately
    if (InformationLevel == ScreenOff) {
        Wh_Log(L"NtPowerInformation called with ScreenOff");
        if (RequestFade()) {
            Wh_Log(L"Requested fade to Winlogon");
            return 0; // STATUS_SUCCESS
        }
    }
    return NtPowerInformation_original(InformationLevel, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength);
};

// From aubymori's MinMax mod
// Return controls the DWP hook behavior below
bool SleepFadeWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!g_settings.sleepFadeEnabled) {
        return false;
    }

    switch (uMsg)
    {
        case WM_SYSCOMMAND:
        {
            UINT cmd = wParam & 0xFFF0;
            switch (cmd)
            {
                case SC_MONITORPOWER:
                {
                    if (lParam == 2) { // Monitor off
                        Wh_Log(L"WM_SYSCOMMAND SC_MONITORPOWER 2 received");
                        if (RequestFade()) {
                            return true;
                        }
                        return false;
                    }
                }
            }
            return false;
        }
        default:
            return false;
    }
}

#define DWP_HOOK_(type, name, defArgs, callArgs) \
type (CALLBACK *name ## _orig) defArgs; \
type CALLBACK name ## _hook  defArgs \
{ \
    if (SleepFadeWndProc(hWnd, uMsg, wParam, lParam)) { \
        return 0; \
    } \
    return name ## _orig  callArgs; \
}

#define DWP_HOOK(type, name, defArgs, callArgs)  \
    DWP_HOOK_(type, name ## A, defArgs, callArgs) \
    DWP_HOOK_(type, name ## W, defArgs, callArgs) \

DWP_HOOK(
    LRESULT,
    DefWindowProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
DWP_HOOK(
    LRESULT,
    DefFrameProc,
    (HWND hWnd, HWND hWndMDIClient, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, hWndMDIClient, uMsg, wParam, lParam))
DWP_HOOK(
    LRESULT,
    DefMDIChildProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
DWP_HOOK(
    LRESULT,
    DefDlgProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))

// Prevent unexpected behavior like monitor turning off during an ongoing fade, when applications use [Send|Post]Message with HWND_BROADCAST to turn off the monitor
// This is a common hack used by a lot of programs to turn the monitor off, but it makes the SC_MONITORPOWER handler to run in a lot of processes in a short amount of time
// The message can also reach processes excluded by Windhawk and unhookable/protected processes, which we cannot intercept (to ignore the request) once the message reaches them
// So just use this hacky workaround to initiate the fade directly from the call, without letting the message out of the caller process,
// so that we can at least do proper fade on hacky monitor off requests from included/hooked processes
// There are bunch of Win32 APIs to post messages to other processes, but only these two are commonly used by those hacky monitor off implementations
DWP_HOOK(
    LRESULT,
    SendMessage,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
DWP_HOOK(
    BOOL,
    PostMessage,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
#pragma endregion

#pragma region Hooks for sleep and hibernation fading
// SetSuspendState and SetSystemPowerState both internally call NtInitiatePowerAction synchronously
// Fading in asynchronous mode is not yet supported as it makes determining the correct timing to restore gamma ramps difficult without causing extra delay or screen flash,
// and we would also need to delegate the fading to a separate process or thread to avoid blocking the caller (and process exit), which adds more complexity and potential issues
// Asynchronous NtInitiatePowerAction usage in user mode seems to be pretty rare anyway
// Currently this hook is only used if the Winlogon kernel power message handler hook below has failed or is disabled
EXTERN_C NTSYSAPI NTSTATUS NTAPI NtInitiatePowerAction(POWER_ACTION, SYSTEM_POWER_STATE, ULONG, BOOLEAN);
using NtInitiatePowerAction_t = decltype(&NtInitiatePowerAction);
NtInitiatePowerAction_t NtInitiatePowerAction_original;
NTSTATUS NTAPI NtInitiatePowerAction_hook(POWER_ACTION SystemAction, SYSTEM_POWER_STATE LightestSystemState, ULONG Flags, BOOLEAN Asynchronous) {
    Wh_Log(L"NtInitiatePowerAction, SystemAction=%d, LightestSystemState=%d, Flags=0x%X, Asynchronous=%d", SystemAction, LightestSystemState, Flags, Asynchronous);
    if (g_settings.sleepFadeEnabled && (!g_settings.enhancedSleepIntercept || Wh_GetIntValue(L"WinlogonHookFailed", 0)) &&
        !Asynchronous && (SystemAction == PowerActionSleep || SystemAction == PowerActionHibernate)
    ) {
        if (RequestFade()) {
            Wh_Log(L"Requested sleep/hibernate fade from individual process");
            for (int i = 0; i < 5; i++) {
                if (IsFadeInProgress()) {
                    Wh_Log(L"Waited %d times for the fade to begin", i);
                    break;
                }
                Sleep(100);
            }
            WaitForFade();
        }
    }
    return NtInitiatePowerAction_original(SystemAction, LightestSystemState, Flags, Asynchronous);
};

#ifdef ENABLE_WINLOGON_HOOKS
// Internal Winlogon function for handling power messages sent by win32k (xxxSendWinlogonPowerMessage)
// a1 known values:
// 263: Sent before sleeping/hibernating (can block)
// 262: Sent twice after waking up from sleep/hibernate
// 259: Sent when resuming from hibernation
// 261: Sent after turning off the monitor (cannot block, sent on monitor off/sleep/hibernate)
// 260: Sent after turning on the monitor (cannot block, sent on monitor on/wake up from sleep/hibernate)
// Tested only on Win10 LTSC 2021 22H2 and 11 25H2, and from my static analysis it might work from 10 21H2 to 11 25H2
// Doubt it'll work on 1903 and below. 1909-20H2 were not inspected
// The symbol string itself seems to be present in all x64 Windows versions between 7 and 11 25H2
typedef int (__cdecl *WMsgPSPHandler_t)(unsigned long a1, void* a2, void* a3, long* a4);
WMsgPSPHandler_t WMsgPSPHandler_original;
int __cdecl WMsgPSPHandler_hook(unsigned long a1, void* a2, void* a3, long* a4) {
    Wh_Log(L"WMsgPSPHandler called, a1=%lu", a1);
    if (g_settings.sleepFadeEnabled && g_settings.enhancedSleepIntercept) {
        if (a1 == 263) { // Pre-sleep/hibernate message
            if (BeginFade()) {
                int refreshRate = GetDeviceRefreshRate();
                if (GetMonitorsInfo(&g_sleepFadeData.monitors, &g_sleepFadeData.monitorCount)) {
                    FadeDesktop(refreshRate, g_settings.sleepDuration, true, g_sleepFadeData.monitors, g_sleepFadeData.monitorCount);
                }
                EndFade();
            }
        } else if (a1 == 260) { // Post-monitor-on/wake-up message
            if (g_sleepFadeData.monitors) {
                for (int i = 0; i < g_sleepFadeData.monitorCount; i++) {
                    if (g_sleepFadeData.monitors[i].hDC) {
                        SetDeviceGammaRamp(g_sleepFadeData.monitors[i].hDC, g_sleepFadeData.monitors[i].origGamma); // Restore gamma
                        DeleteDC(g_sleepFadeData.monitors[i].hDC);
                    }
                }
                free(g_sleepFadeData.monitors);
                g_sleepFadeData.monitors = NULL;
                g_sleepFadeData.monitorCount = 0;
            }
        }
    }
    return WMsgPSPHandler_original(a1, a2, a3, a4);
};
#endif
#pragma endregion

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_original;
LONG WINAPI RegGetValueW_hook(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    if (hkey == HKEY_LOCAL_MACHINE && lpSubKey && lpValue && pvData) {
        if (wcscmp(lpSubKey, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ICM") == 0 && wcscmp(lpValue, L"GdiIcmGammaRange") == 0) {
            Wh_Log(L"Hooked DWM GdiIcmGammaRange");
            *(DWORD*)pvData = 256;
            return ERROR_SUCCESS;
        }
    }
    return RegGetValueW_original(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

bool CreateFadeMutexAndEvent() {
    static const wchar_t* kMutexName = L"Local\\LsfFadeMutex";
    static const wchar_t* kEventName = L"Local\\LsfFadeEvent";
    static const wchar_t* kSddl = L"D:(A;;GA;;;IU)(A;;GA;;;SY)(A;;GA;;;BA)";

    SECURITY_ATTRIBUTES sa = {};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = FALSE;

    PSECURITY_DESCRIPTOR securityDescriptor = NULL;
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(
            kSddl,
            SDDL_REVISION_1,
            &securityDescriptor,
            NULL)) {
        sa.lpSecurityDescriptor = securityDescriptor;
    } else {
        Wh_Log(L"ConvertStringSecurityDescriptorToSecurityDescriptorW failed, GLE=%d", GetLastError());
    }

    g_fadeMutex = CreateMutexExW(
        sa.lpSecurityDescriptor ? &sa : NULL,
        kMutexName,
        0,
        SYNCHRONIZE | MUTEX_MODIFY_STATE
    );

    if (!g_fadeMutex) {
        DWORD createError = GetLastError();
        Wh_Log(L"CreateMutexExW with custom/default security failed, GLE=%d", createError);

        if (sa.lpSecurityDescriptor) {
            // Retry without explicit security descriptor
            g_fadeMutex = CreateMutexExW(NULL, kMutexName, 0, SYNCHRONIZE | MUTEX_MODIFY_STATE);
            if (!g_fadeMutex) {
                Wh_Log(L"CreateMutexExW fallback failed, GLE=%d", GetLastError());
            }
        }
    }

    g_fadeEvent = CreateEventW(
        sa.lpSecurityDescriptor ? &sa : NULL,
        FALSE,
        FALSE,
        kEventName
    );

    if (!g_fadeEvent) {
        DWORD createError = GetLastError();
        Wh_Log(L"CreateEventW with custom/default security failed, GLE=%d", createError);

        if (sa.lpSecurityDescriptor) {
            // Retry without explicit security descriptor
            g_fadeEvent = CreateEventW(NULL, FALSE, FALSE, kEventName);
            if (!g_fadeEvent) {
                Wh_Log(L"CreateEventW fallback failed, GLE=%d", GetLastError());
            }
        }
    }

    if (securityDescriptor) {
        LocalFree(securityDescriptor);
    }

    return g_fadeMutex && g_fadeEvent;
}

FadeType FadeTypeStringToEnum(LPCWSTR str) {
    if (wcscmp(str, L"gamma") == 0) {
        return Gamma;
    } else if (wcscmp(str, L"dwm") == 0) {
        return DWM;
    } else {
        return None;
    }
}

inline int ClampInt(int value, int min, int max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}

void LoadSettings() {
    auto logonTypeStr = WindhawkUtils::StringSetting::make(L"type");
    FadeType logonType = Gamma;
    if (*logonTypeStr) {
        logonType = FadeTypeStringToEnum(logonTypeStr);
    }

    int logonDuration = Wh_GetIntSetting(L"duration");
    logonDuration = ClampInt(logonDuration, 0, 10000);
    if (logonDuration == 0) {
        logonType = None;
    }

    auto logoffTypeStr = WindhawkUtils::StringSetting::make(L"logoffType");
    FadeType logoffType = Gamma;
    if (*logoffTypeStr) {
        FadeTypeStringToEnum(logoffTypeStr);
    }

    int logoffDuration = Wh_GetIntSetting(L"logoffDuration");
    logoffDuration = ClampInt(logoffDuration, 0, 10000);
    if (logoffDuration == 0) {
        logoffType = None;
    }

    bool sleepFadeEnabled = Wh_GetIntSetting(L"sleepFadeEnabled") != 0;
    int sleepDuration = Wh_GetIntSetting(L"sleepDuration");
    sleepDuration = ClampInt(sleepDuration, 0, 10000);
    if (sleepDuration == 0) {
        sleepFadeEnabled = false;
    }

    auto enhancedSleepInterceptStr = WindhawkUtils::StringSetting::make(L"enhancedSleepIntercept");
    bool enhancedSleepIntercept = g_is21H2OrNewer;
    if (wcscmp(enhancedSleepInterceptStr, L"always") == 0) {
        enhancedSleepIntercept = true;
    } else if (wcscmp(enhancedSleepInterceptStr, L"never") == 0) {
        enhancedSleepIntercept = false;
    }

    g_settings.logonType = logonType;
    g_settings.logonDuration = logonDuration;
    g_settings.logoffType = logoffType;
    g_settings.logoffDuration = logoffDuration;
    g_settings.sleepFadeEnabled = sleepFadeEnabled;
    g_settings.sleepDuration = sleepDuration;
    g_settings.enhancedSleepIntercept = enhancedSleepIntercept;

    // Wh_Log(L"Settings loaded: logonType=%d, logonDuration=%d, logoffType=%d, logoffDuration=%d, sleepFadeEnabled=%d, sleepDuration=%d, enhancedSleepIntercept=%d",
    //     g_settings.logonType, g_settings.logonDuration, g_settings.logoffType, g_settings.logoffDuration, g_settings.sleepFadeEnabled, g_settings.sleepDuration, g_settings.enhancedSleepIntercept);
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    OSVERSIONINFOW osInfo;
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    RtlGetVersion(&osInfo);
    g_is21H2OrNewer = osInfo.dwBuildNumber >= 19044;

    LoadSettings();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        Wh_Log(L"GetModuleHandleW user32.dll failed");
        return FALSE;
    }

    wchar_t exeName[MAX_PATH];
    GetModuleFileNameW(NULL, exeName, MAX_PATH);
    g_isWinlogon = wcsstr(_wcsupr(exeName), L"\\WINLOGON.EXE") != NULL;
    g_isDwm = wcsstr(exeName, L"\\DWM.EXE") != NULL;
    if (g_isWinlogon) {
#ifdef ENABLE_WINLOGON_HOOKS
        Wh_Log(L"Running in winlogon.exe");
        SwitchDesktopWithFade_t SwitchDesktopWithFade = (SwitchDesktopWithFade_t)GetProcAddress(user32, "SwitchDesktopWithFade");
        if (!SwitchDesktopWithFade) {
            Wh_Log(L"GetProcAddress SwitchDesktopWithFade failed");
            return FALSE;
        }
        if (!WindhawkUtils::SetFunctionHook(SwitchDesktopWithFade, SwitchDesktopWithFade_hook, &SwitchDesktopWithFade_original)) {
            Wh_Log(L"Wh_SetFunctionHook SwitchDesktopWithFade failed");
            return FALSE;
        }

        HMODULE winlogon = GetModuleHandleW(NULL);
        if (winlogon) {
            WindhawkUtils::SYMBOL_HOOK winlogonExeHooks[] = {
                {
                    {
                        L"int __cdecl WMsgPSPHandler(unsigned long,struct tagPOWERSTATEPARAMS *,struct _RPC_ASYNC_STATE *,long *)",
                    },
                    (void**)&WMsgPSPHandler_original,
                    (void*)WMsgPSPHandler_hook,
                    FALSE
                }
            };
            if (!WindhawkUtils::HookSymbols(winlogon, winlogonExeHooks, ARRAYSIZE(winlogonExeHooks))) {
                Wh_Log(L"HookSymbols WMsgPSPHandler failed");
                Wh_SetIntValue(L"WinlogonHookFailed", 1);
            } else {
                Wh_SetIntValue(L"WinlogonHookFailed", 0);
            }
        }

        // Check whether this system uses the modern gamma manager (mscms.dll then coloradapterclient.dll, 1903+) or not (original NtGdi syscall, 1809-)
        // by checking if mscms.dll exports a GDI gamma redirector function
        HMODULE hMsCms = LoadLibraryExW(L"mscms.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hMsCms) {
            g_isModernGamma = GetProcAddress(hMsCms, "InternalSetDeviceGDIGammaRamp") != NULL;
            FreeLibrary(hMsCms);
        }
#else
        Wh_Log(L"Running in winlogon.exe, but winlogon hooks are disabled in this build");
        Wh_SetIntValue(L"WinlogonHookFailed", 1);
        return FALSE;
#endif
    } else if (g_isDwm) {
        HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
        if (hKernelBase) {
            RegGetValueW_t pRegGetValueW = (RegGetValueW_t)GetProcAddress(hKernelBase, "RegGetValueW");
            if (WindhawkUtils::SetFunctionHook(pRegGetValueW, RegGetValueW_hook, &RegGetValueW_original)) {
                return TRUE;
            }
        }
        Wh_Log(L"Wh_SetFunctionHook RegGetValueW failed");
        return FALSE;
    } else if (!g_settings.sleepFadeEnabled) {
        Wh_Log(L"Sleep fade is disabled and not running in winlogon.exe, unloading...");
        return FALSE;
    }

    if (!CreateFadeMutexAndEvent()) {
        // Likely services, but I think they would barely initiate the sleep fade anyway
        Wh_Log(L"Failed to create/open fade mutex and event");
        if (!g_isWinlogon) {
            // Allow winlogon to proceed without mutex or event to allow the single process mode (no sleep fade) to work at least, but not for others
            return FALSE;
        }
    }

#define CLEANUP                   \
    if (g_fadeMutex) {            \
        CloseHandle(g_fadeMutex); \
        g_fadeMutex = NULL;       \
    }                             \
    return FALSE;

    if (!WindhawkUtils::SetFunctionHook(NtInitiatePowerAction, NtInitiatePowerAction_hook, &NtInitiatePowerAction_original)) {
        Wh_Log(L"Wh_SetFunctionHook NtInitiatePowerAction failed");
        CLEANUP
    }

    if (g_isWinlogon) {
        // It has neither interactive window nor NtPowerInformation(ScreenOff) usage
        NtPowerInformation_original = NtPowerInformation;
        g_stopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (!g_stopEvent) {
            Wh_Log(L"CreateEventW stopEvent failed, GLE=%d", GetLastError());
            Wh_SetIntValue(L"NoAsyncFade", 1);
        } else {
            CreateAsyncFadeThread();
        }
        return TRUE;
    }

    if (!WindhawkUtils::SetFunctionHook(NtPowerInformation, NtPowerInformation_hook, &NtPowerInformation_original)) {
        Wh_Log(L"Wh_SetFunctionHook NtPowerInformation failed");
        CLEANUP
    }

#define HOOK(func)                                                                         \
    if (!WindhawkUtils::SetFunctionHook(func, func ## _hook, &func ## _orig)) \
    {                                                                                      \
        Wh_Log(L"Failed to hook %s", L ## #func);                                          \
        CLEANUP                                                                            \
    }

#define HOOK_A_W(func) HOOK(func ## A) HOOK(func ## W)

    HOOK_A_W(DefWindowProc)
    HOOK_A_W(DefFrameProc)
    HOOK_A_W(DefMDIChildProc)
    HOOK_A_W(DefDlgProc)
    HOOK_A_W(SendMessage)
    HOOK_A_W(PostMessage)

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_isExiting.store(true); // Prevent new fades
    if (IsFadeInProgressInThisProcess()) {
        Wh_Log(L"Fade is in progress during uninit, waiting for it to complete...");
        WaitForFade();
    }
    if (g_fadeMutex) {
        CloseHandle(g_fadeMutex);
        g_fadeMutex = NULL;
    }
    HANDLE asyncFadeThread = g_asyncFadeThread.exchange(NULL);
    if (asyncFadeThread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(asyncFadeThread, INFINITE);
        CloseHandle(asyncFadeThread);
        if (UnregisterClassW(OVERLAY_WIN_CLASS, HINST_THISCOMPONENT) == 0) {
            Wh_Log(L"UnregisterClassW failed, GLE=%d", GetLastError());
        }
    }
    HANDLE monitorOffThread = g_monitorOffThread.exchange(NULL);
    if (monitorOffThread) {
        WaitForSingleObject(monitorOffThread, INFINITE);
        CloseHandle(monitorOffThread);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    LoadSettings();
    if (!g_isWinlogon && !g_settings.sleepFadeEnabled) {
        // The global injection is enabled or disabled, need to unload the mod
        // (Reloading unloaded mod on mod setting change is automatically handled by Windhawk)
        return FALSE;
    }
    return TRUE;
}
