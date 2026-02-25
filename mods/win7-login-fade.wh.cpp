// ==WindhawkMod==
// @id              win7-login-fade
// @name            Logon & Sleep Fade Restorer
// @description     Bring back the old logon screen and sleep fade effect
// @version         1.2
// @author          Ingan121
// @github          https://github.com/Ingan121
// @twitter         https://twitter.com/Ingan121
// @homepage        https://www.ingan121.com/
// @include         winlogon.exe
// @include         *
// @architecture    x86-64
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Logon & Sleep Fade Restorer
* This mod can restore the old fade effect from older Windows versions when logging on, logging out, unlocking, sleeping, hibernating, and turning off the monitor.
## Mod settings explanation
* The logon/unlock and logoff/shutdown fade types can be set independently, so you can have different fade types and durations for those two if you want.
* There are multiple logon and logoff fade types to choose from:
  * None: No fade effect, the screen will switch instantly.
  * Gamma (Reimplemented): The classic fade effect reimplemented in this mod, which works by adjusting the screen brightness using the SetDeviceGammaRamp function, the same method used by the original fade effect. It has better compatibility than the kernel-mode fade type.
  * Gamma (Kernel): Use the existing kernel-mode gamma fade logic, instead of a user-mode reimplementation. **This is broken on Windows 10 1903 and later**, and only an extra delay during an invisible fade will be present. **Do not use this mode on recent versions of Windows.**
  * DWM (Original): Keep the original DWM fade effect to make the fade work as if this mod is disabled. Duration is not adjustable in this mode. Not available for logoff/shutdown.
* The gamma-based fade types will not work with Microsoft Basic Display Adapter, VMware SVGA 3D, and some other display drivers that do not support gamma adjustment. It's also not compatible with NVIDIA driver's reference color mode.
* To use the `Gamma (Reimplemented)` mode (which is the default), you'll need to set the `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\ICM\GdiIcmGammaRange` registry value to `0x100` (DWORD, 256 in decimal) to allow brightness values below the normal level, which is required for the fade effect to work naturally.
* To add fade effects to sleep and hibernation initiated by the idle timer or power button, enable the "Enable enhanced sleep/hibernate interception" option. This option has limited compatibility compared to the rest of the mod, and is only tested on Windows 10 LTSC 2021 (22H2). It might work on 21H2 and later versions; however, this mode is not likely to work on Windows 10 1903 and earlier, unfortunately.
## Presets
* Windows Vista/7 (mod defaults)
  * Logon fade type: Gamma (Reimplemented)
  * Logon fade duration: 1000 ms
  * Logoff fade type: Gamma (Reimplemented)
  * Logoff fade duration: 1000 ms
  * Sleep fade enabled: true
  * Sleep fade duration: 500 ms
* Windows 8/8.1/10 1507-1809
  * Logon fade type: DWM (Original)
  * Logon fade duration: (ignored in this mode)
  * Logoff fade type: Gamma (Reimplemented)
  * Logoff fade duration: 334 ms
  * Sleep fade enabled: true
  * Sleep fade duration: 167 ms
## Known issues and limitations
* The logon fade animation may look broken on early boot when using the auto-login feature.
* Turning off the monitor with the power button action (or sleeping on Modern Standby devices) will not trigger the sleep fade effect, as it is handled internally by the kernel without informing a user-mode component.
  * Monitor off initiated by the idle timer can have fade added as usual.
* Only `Gamma (Kernel)` and `DWM (Original)` logon/logoff fade types are supported on Windows 8 for now, to avoid critical issues that I have observed. Early Windows 10 versions are not tested, so avoid `None` and `Gamma (Reimplemented)` modes on those versions as well to be safe, until I can confirm their stability.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/* 
- type: gamma
  $name: Logon fade type
  $name:ko-KR: 로그인 페이드 유형
  $options:
    - none: None
    - gamma: Gamma (Reimplemented)
    - kernel: Gamma (Kernel)
    - dwm: DWM (Original)
  $options:ko-KR:
    - none: 없음
    - gamma: 감마 (재구현)
    - kernel: 감마 (커널)
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
    - gamma: Gamma (Reimplemented)
    - kernel: Gamma (Kernel)
  $options:ko-KR:
    - none: 없음
    - gamma: 감마 (재구현)
    - kernel: 감마 (커널)
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
- enhancedSleepIntercept: false
  $name: Enable enhanced sleep/hibernate interception
  $name:ko-KR: 향상된 절전/최대 절전 모드 가로채기 활성화
  $description: "Adds fade effects to sleep and hibernation by intercepting the messages between Winlogon and the kernel, which allows the fade effect to be applied to every sleep/hibernate action, including the kernel-initiated idle timer and power button actions. Only tested on Windows 10 LTSC 2021 (22H2), but it might work on 21H2 and later versions. Unfortunately, this mode is not likely to work on Windows 10 1903 and earlier."
  $description:ko-KR: "Winlogon과 커널 간의 메시지를 가로채 절전/최대 절전 모드 진입 시 페이드 효과를 추가합니다. 이를 통해 커널에서 시작된 유휴 타이머 및 전원 버튼 동작을 포함하여 모든 절전/최대 절전 모드 작업에 페이드 효과를 적용할 수 있습니다. Windows 10 LTSC 2021(22H2)에서만 테스트되었지만 21H2 및 이후 버전에서 작동할 수도 있습니다. 본 모드는 Windows 10 1903 및 이전 버전에서는 작동하지 않을 가능성이 높습니다."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <atomic>
#include <sddl.h>
#include <versionhelpers.h>

enum FadeType {
    None,
    Gamma,
    Kernel,
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
HANDLE g_fadeMutex = NULL;
std::atomic<bool> g_isFadeInProgress = false;
std::atomic<bool> g_isExiting = false;
std::atomic<HANDLE> g_monitorOffThread = NULL;

typedef struct _MONITOR_INFO {
    HDC hDC = NULL;
    WORD origGamma[3][256];
} MONITOR_INFO;

struct WinlogonSleepFadeData {
    int monitorCount;
    MONITOR_INFO* monitors;
} g_winlogonSleepFadeData;

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
// SetDeviceGammaRamp in user32.dll is now redirected to the 'modern' gamma manager in mscms.dll, which requires DWM to work
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
    bool isLogonUnlock = duration == 1000 && flags != 0;
    int durationToUse = isLogonUnlock ? g_settings.logonDuration : g_settings.logoffDuration;
    if (isLogonUnlock) {
        switch (g_settings.logonType) {
            case Gamma: // Gamma (Reimplemented)
                if (!BeginFade()) {
                    return SwitchDesktop(hDesktop);
                }
                // Do the fade in this hook function
                break;
            case Kernel: // Gamma (Kernel)
                // Force the original kernel mode gamma fade by setting flags to 0
                return SwitchDesktopWithFade_original(hDesktop, duration, 0);
            case DWM: // DWM (Original)
                // Pass it as-is
                return SwitchDesktopWithFade_original(hDesktop, duration, flags);
            case None: // None
            default:
                return SwitchDesktop(hDesktop);
        }
    } else {
        switch (g_settings.logoffType) {
            case Gamma:
                if (!BeginFade()) {
                    return SwitchDesktop(hDesktop);
                }
                break;
            case Kernel:
                return SwitchDesktopWithFade_original(hDesktop, duration, 0);
            case None:
            default:
                return SwitchDesktop(hDesktop);
        }
    }
    if (durationToUse <= 0) {
        EndFade();
        return SwitchDesktop(hDesktop);
    }

    int refreshRate = GetDeviceRefreshRate();
    int monitorCount = 0;
    MONITOR_INFO* monitors = NULL;
    if (!GetMonitorsInfo(&monitors, &monitorCount)) {
        EndFade();
        return SwitchDesktop(hDesktop);
    }

    FadeDesktop(refreshRate, durationToUse / 2, true, monitors, monitorCount);
    BOOL result = SwitchDesktop(hDesktop);
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

// SetSuspendState and SetSystemPowerState both internally call NtInitiatePowerAction synchronously
// Fading in asynchronous mode is not yet supported as it makes determining the correct timing to restore gamma ramps difficult without causing extra delay or screen flash,
// and we would also need to delegate the fading to a separate process or thread to avoid blocking the caller (and process exit), which adds more complexity and potential issues
// Asynchronous NtInitiatePowerAction usage in user mode seems to be pretty rare anyway
// Currently this hook is only used if the Winlogon kernel power message handler hook below has failed or is disabled
typedef NTSTATUS (NTAPI* NtInitiatePowerAction_t)(POWER_ACTION, SYSTEM_POWER_STATE, ULONG, BOOLEAN);
NtInitiatePowerAction_t NtInitiatePowerAction_original;
NTSTATUS NTAPI NtInitiatePowerAction_hook(POWER_ACTION SystemAction, SYSTEM_POWER_STATE LightestSystemState, ULONG Flags, BOOLEAN Asynchronous) {
    Wh_Log(L"NtInitiatePowerAction, SystemAction=%d, LightestSystemState=%d, Flags=0x%X, Asynchronous=%d", SystemAction, LightestSystemState, Flags, Asynchronous);
    if (g_settings.sleepFadeEnabled && (!g_settings.enhancedSleepIntercept || Wh_GetIntValue(L"WinlogonHookFailed", 0)) &&
        !Asynchronous && (SystemAction == PowerActionSleep || SystemAction == PowerActionHibernate)
    ) {
        if (BeginFade()) {
            Wh_Log(L"Initiating sleep/hibernate fade from individual process...");
            int refreshRate = GetDeviceRefreshRate();
            int monitorCount = 0;
            MONITOR_INFO* monitors = NULL;
            if (!GetMonitorsInfo(&monitors, &monitorCount)) {
                EndFade();
                return NtInitiatePowerAction_original(SystemAction, LightestSystemState, Flags, FALSE);
            }

            FadeDesktop(refreshRate, g_settings.sleepDuration, true, monitors, monitorCount);
            NTSTATUS result = NtInitiatePowerAction_original(SystemAction, LightestSystemState, Flags, FALSE);

            for (int i = 0; i < monitorCount; i++) {
                if (monitors[i].hDC) {
                    SetDeviceGammaRamp(monitors[i].hDC, monitors[i].origGamma); // Restore gamma
                    DeleteDC(monitors[i].hDC);
                }
            }
            free(monitors);
            EndFade();
            return result;
        }
    }
    return NtInitiatePowerAction_original(SystemAction, LightestSystemState, Flags, Asynchronous);
};

typedef NTSTATUS (NTAPI* NtPowerInformation_t)(POWER_INFORMATION_LEVEL, PVOID, ULONG, PVOID, ULONG);
NtPowerInformation_t NtPowerInformation_original;

DWORD MonitorOffThreadProc(LPVOID lpParameter) {
    if (BeginFade()) {
        Wh_Log(L"Initiating monitor off fade...");
        int refreshRate = GetDeviceRefreshRate();
        int monitorCount = 0;
        MONITOR_INFO* monitors = NULL;
        if (!GetMonitorsInfo(&monitors, &monitorCount)) {
            EndFade();
            NtPowerInformation_original(ScreenOff, NULL, 0, NULL, 0);
            return 1;
        }

        FadeDesktop(refreshRate, g_settings.sleepDuration, true, monitors, monitorCount);
        NtPowerInformation_original(ScreenOff, NULL, 0, NULL, 0);
        Sleep(1500); // Arbitrary delay to ensure the display is fully off before restoring gamma, as this undocumented API is asynchronous and otherwise it'll cause a screen flash

        for (int i = 0; i < monitorCount; i++) {
            if (monitors[i].hDC) {
                SetDeviceGammaRamp(monitors[i].hDC, monitors[i].origGamma);
                DeleteDC(monitors[i].hDC);
            }
        }
        free(monitors);
        EndFade();
        return 0;
    } else {
        NtPowerInformation_original(ScreenOff, NULL, 0, NULL, 0);
    }
    return 1;
}

NTSTATUS NTAPI NtPowerInformation_hook(POWER_INFORMATION_LEVEL InformationLevel, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength) {
    // Undocumented API introduced in Windows 8, known usermode use cases include Open-Shell Menu
    // DefWindowProc WM_SYSCOMMAND SC_MONITORPOWER has it's own kernel mode routine and does not use this API, so these two must be hooked separately
    if (InformationLevel == ScreenOff && g_settings.sleepFadeEnabled && !IsFadeInProgress()) {
        // Both NtPowerInformation and DefWindowProc WM_SYSCOMMAND is asynchronous so do it in a separate thread
        HANDLE monitorOffThread = g_monitorOffThread.exchange(NULL);
        if (monitorOffThread) {
            // cleanup handle (it shouldn't be running at least as IsFadeInProgress is checked above)
            DWORD result = WaitForSingleObject(monitorOffThread, 0);
            if (result != WAIT_OBJECT_0 && result != WAIT_ABANDONED) {
                Wh_Log(L"Monitor off thread is still running, wait failed with GLE=%d", GetLastError());
                return NtPowerInformation_original(ScreenOff, NULL, 0, NULL, 0);
            }
            CloseHandle(monitorOffThread);
        }
        monitorOffThread = CreateThread(NULL, 0, MonitorOffThreadProc, NULL, 0, NULL);
        if (!monitorOffThread) {
            Wh_Log(L"Failed to create monitor off thread, GLE=%d", GetLastError());
            return NtPowerInformation_original(ScreenOff, NULL, 0, NULL, 0);
        }
        g_monitorOffThread.store(monitorOffThread);
        return 0; // STATUS_SUCCESS
    }
    return NtPowerInformation_original(InformationLevel, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength);
};

// From aubymori's MinMax mod
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
                        Wh_Log(L"WM_SYSCOMMAND SC_MONITORPOWER received");
                        NtPowerInformation_hook(ScreenOff, NULL, 0, NULL, 0);
                        return true;
                    }
                }
            }
            return false;
        }
        default:
            return false;
    }
}

#define DWP_HOOK_(name, defArgs, callArgs) \
LRESULT (CALLBACK *name ## _orig) defArgs; \
LRESULT CALLBACK name ## _hook  defArgs \
{ \
    if (SleepFadeWndProc(hWnd, uMsg, wParam, lParam)) { \
        return 0; \
    } \
    return name ## _orig  callArgs; \
}

#define DWP_HOOK(name, defArgs, callArgs)  \
    DWP_HOOK_(name ## A, defArgs, callArgs) \
    DWP_HOOK_(name ## W, defArgs, callArgs) \

DWP_HOOK(
    DefWindowProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
DWP_HOOK(
    DefFrameProc,
    (HWND hWnd, HWND hWndMDIClient, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, hWndMDIClient, uMsg, wParam, lParam))
DWP_HOOK(
    DefMDIChildProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
DWP_HOOK(
    DefDlgProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))

// For some command line utilities that initiates sleep/hibernate/monitor off asynchronously then exit immediately
using ExitProcess_t = void (WINAPI*)(UINT uExitCode);
ExitProcess_t ExitProcess_original;
void WINAPI ExitProcess_hook(UINT uExitCode) {
    if (IsFadeInProgressInThisProcess()) {
        Wh_Log(L"Deferring process exit until fade is finished...");
        WaitForFade();
    }
    ExitProcess_original(uExitCode);
};

// Internal Winlogon function for handling power messages sent by win32k (xxxSendWinlogonPowerMessage)
// a1 known values:
// 263: Sent before sleeping/hibernating (can block)
// 262: Sent twice after waking up from sleep/hibernate
// 259: Sent when resuming from hibernation
// 261: Sent after turning off the monitor (cannot block, sent on monitor off/sleep/hibernate)
// 260: Sent after turning on the monitor (cannot block, sent on monitor on/wake up from sleep/hibernate)
// Tested only on Win10 LTSC 2021 22H2, and from my static analysis it might work from 10 21H2 to 11 25H2. Doubt it'll work on 1903 and below. 1909-20H2 were not inspected
// The symbol itself seems to be present in all Windows versions between 8.0 and 11 25H2
typedef int (__cdecl *WMsgPSPHandler_t)(unsigned long a1, void* a2, void* a3, long* a4);
WMsgPSPHandler_t WMsgPSPHandler_original;
int __cdecl WMsgPSPHandler_hook(unsigned long a1, void* a2, void* a3, long* a4) {
    Wh_Log(L"WMsgPSPHandler called, a1=%lu", a1);
    if (g_settings.sleepFadeEnabled && g_settings.enhancedSleepIntercept) {
        if (a1 == 263) { // Pre-sleep/hibernate message
            if (BeginFade()) {
                int refreshRate = GetDeviceRefreshRate();
                if (GetMonitorsInfo(&g_winlogonSleepFadeData.monitors, &g_winlogonSleepFadeData.monitorCount)) {
                    FadeDesktop(refreshRate, g_settings.sleepDuration, true, g_winlogonSleepFadeData.monitors, g_winlogonSleepFadeData.monitorCount);
                }
                EndFade();
            }
        } else if (a1 == 260) { // Post-monitor-on/wake-up message
            int refreshRate = GetDeviceRefreshRate();
            if (g_winlogonSleepFadeData.monitors) {
                FadeDesktop(refreshRate, g_settings.sleepDuration, false, g_winlogonSleepFadeData.monitors, g_winlogonSleepFadeData.monitorCount);
                for (int i = 0; i < g_winlogonSleepFadeData.monitorCount; i++) {
                    if (g_winlogonSleepFadeData.monitors[i].hDC) {
                        SetDeviceGammaRamp(g_winlogonSleepFadeData.monitors[i].hDC, g_winlogonSleepFadeData.monitors[i].origGamma); // Restore gamma
                        DeleteDC(g_winlogonSleepFadeData.monitors[i].hDC);
                    }
                }
                free(g_winlogonSleepFadeData.monitors);
                g_winlogonSleepFadeData.monitors = NULL;
                g_winlogonSleepFadeData.monitorCount = 0;
            }
        }
    }
    return WMsgPSPHandler_original(a1, a2, a3, a4);
};

HANDLE CreateFadeMutex() {
    static const wchar_t* kMutexName = L"Local\\LsfFadeMutex";
    static const wchar_t* kMutexSddl = L"D:(A;;GA;;;IU)(A;;GA;;;SY)(A;;GA;;;BA)";

    SECURITY_ATTRIBUTES sa = {};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = FALSE;

    PSECURITY_DESCRIPTOR securityDescriptor = NULL;
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(
            kMutexSddl,
            SDDL_REVISION_1,
            &securityDescriptor,
            NULL)) {
        sa.lpSecurityDescriptor = securityDescriptor;
    } else {
        Wh_Log(L"ConvertStringSecurityDescriptorToSecurityDescriptorW failed, GLE=%d", GetLastError());
    }

    HANDLE mutex = CreateMutexExW(
        sa.lpSecurityDescriptor ? &sa : NULL,
        kMutexName,
        0,
        SYNCHRONIZE | MUTEX_MODIFY_STATE
    );

    if (!mutex) {
        DWORD createError = GetLastError();
        Wh_Log(L"CreateMutexExW with custom/default security failed, GLE=%d", createError);

        if (sa.lpSecurityDescriptor) {
            // Retry without explicit security descriptor
            mutex = CreateMutexExW(NULL, kMutexName, 0, SYNCHRONIZE | MUTEX_MODIFY_STATE);
            if (!mutex) {
                Wh_Log(L"CreateMutexExW fallback failed, GLE=%d", GetLastError());
            }
        }
    }

    if (securityDescriptor) {
        LocalFree(securityDescriptor);
    }

    return mutex;
}

FadeType FadeTypeStringToEnum(LPCWSTR str) {
    if (wcscmp(str, L"none") == 0) {
        return None;
    } else if (wcscmp(str, L"gamma") == 0) {
        return Gamma;
    } else if (wcscmp(str, L"kernel") == 0) {
        return Kernel;
    } else if (wcscmp(str, L"dwm") == 0) {
        return DWM;
    } else {
        return None;
    }
}

void LoadSettings() {
    bool isWin10 = IsWindows10OrGreater();

    LPCWSTR logonTypeStr = Wh_GetStringSetting(L"type");
    FadeType logonType = FadeTypeStringToEnum(logonTypeStr);
    Wh_FreeStringSetting(logonTypeStr);
    if (logonType < None || logonType > DWM) {
        logonType = Gamma;
    }
    if (!isWin10 && logonType < Kernel) {
        // On my Win8 machine, somehow calling SwitchDesktop causes weird behavior like repeated SwitchDesktopWithFade calls or winlogon crashes
        // So only allow modes that call SwitchDesktopWithFade_original for now
        logonType = Kernel;
    }
    int logonDuration = Wh_GetIntSetting(L"duration");
    if (logonDuration < 0) {
        logonDuration = 0;
        logonType = None;
    }
    if (logonDuration > 10000) {
        logonDuration = 10000;
    }

    LPCWSTR logoffTypeStr = Wh_GetStringSetting(L"logoffType");
    FadeType logoffType = FadeTypeStringToEnum(logoffTypeStr);
    Wh_FreeStringSetting(logoffTypeStr);
    if (logoffType < None || logoffType > DWM) {
        logoffType = Gamma;
    }
    if (!isWin10 && logoffType < Kernel) {
        logoffType = Kernel;
    }
    int logoffDuration = Wh_GetIntSetting(L"logoffDuration");
    if (logoffDuration < 0) {
        logoffDuration = 0;
        logoffType = None;
    }
    if (logoffDuration > 10000) {
        logoffDuration = 10000;
    }

    bool sleepFadeEnabled = Wh_GetIntSetting(L"sleepFadeEnabled") != 0;
    int sleepDuration = Wh_GetIntSetting(L"sleepDuration");
    if (sleepDuration < 0) {
        sleepDuration = 0;
        sleepFadeEnabled = false;
    }
    if (sleepDuration > 10000) {
        sleepDuration = 10000;
    }
    bool enhancedSleepIntercept = Wh_GetIntSetting(L"enhancedSleepIntercept") != 0;

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

    LoadSettings();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        Wh_Log(L"GetModuleHandleW user32.dll failed");
        return FALSE;
    }

    wchar_t exeName[MAX_PATH];
    GetModuleFileNameW(NULL, exeName, MAX_PATH);
    g_isWinlogon = wcsstr(_wcsupr(exeName), L"\\WINLOGON.EXE") != NULL;
    if (g_isWinlogon) {
        Wh_Log(L"Running in winlogon.exe");
        SwitchDesktopWithFade_t SwitchDesktopWithFade = (SwitchDesktopWithFade_t)GetProcAddress(user32, "SwitchDesktopWithFade");
        if (!SwitchDesktopWithFade) {
            Wh_Log(L"GetProcAddress SwitchDesktopWithFade failed");
            return FALSE;
        }
        if (!Wh_SetFunctionHook((void*)SwitchDesktopWithFade, (void*)SwitchDesktopWithFade_hook, (void**)&SwitchDesktopWithFade_original)) {
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
    } else if (!g_settings.sleepFadeEnabled) {
        Wh_Log(L"Sleep fade is disabled and not running in winlogon.exe, unloading...");
        return FALSE;
    } else {
        if (!Wh_SetFunctionHook((void*)ExitProcess, (void*)ExitProcess_hook, (void**)&ExitProcess_original)) {
            Wh_Log(L"Wh_SetFunctionHook ExitProcess failed");
            // Not that critical, everything that doesn't immediately exit after initiating sleep/hibernate/monitor off will still work fine
        }
    }

    g_fadeMutex = CreateFadeMutex();
    if (!g_fadeMutex) {
        // Likely services, but I think they would barely initiate the sleep fade anyway
        Wh_Log(L"Failed to create/open fade mutex");
        if (!g_isWinlogon) {
            // Allow winlogon to proceed without mutex to allow the single process mode (no sleep fade) to work at least, but not for others
            return FALSE;
        }
    }

#define CLEANUP                   \
    if (g_fadeMutex) {            \
        CloseHandle(g_fadeMutex); \
        g_fadeMutex = NULL;       \
    }                             \
    return FALSE;

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) {
        Wh_Log(L"GetModuleHandleW ntdll.dll failed");
        CLEANUP
    }

    NtInitiatePowerAction_t NtInitiatePowerAction = (NtInitiatePowerAction_t)GetProcAddress(ntdll, "NtInitiatePowerAction");
    if (!NtInitiatePowerAction) {
        Wh_Log(L"GetProcAddress NtInitiatePowerAction failed");
        CLEANUP
    }
    if (!Wh_SetFunctionHook((void*)NtInitiatePowerAction, (void*)NtInitiatePowerAction_hook, (void**)&NtInitiatePowerAction_original)) {
        Wh_Log(L"Wh_SetFunctionHook NtInitiatePowerAction failed");
        CLEANUP
    }

    NtPowerInformation_t NtPowerInformation = (NtPowerInformation_t)GetProcAddress(ntdll, "NtPowerInformation");
    if (!NtPowerInformation) {
        Wh_Log(L"GetProcAddress NtPowerInformation failed");
        CLEANUP
    }
    if (!Wh_SetFunctionHook((void*)NtPowerInformation, (void*)NtPowerInformation_hook, (void**)&NtPowerInformation_original)) {
        Wh_Log(L"Wh_SetFunctionHook NtPowerInformation failed");
        CLEANUP
    }

#define HOOK(func)                                                                         \
    if (!Wh_SetFunctionHook((void *)func, (void *)func ## _hook, (void **)&func ## _orig)) \
    {                                                                                      \
        Wh_Log(L"Failed to hook %s", L ## #func);                                          \
        CLEANUP                                                                            \
    }

#define HOOK_A_W(func) HOOK(func ## A) HOOK(func ## W)

    HOOK_A_W(DefWindowProc)
    HOOK_A_W(DefFrameProc)
    HOOK_A_W(DefMDIChildProc)
    HOOK_A_W(DefDlgProc)

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_isExiting.store(true);
    if (IsFadeInProgressInThisProcess()) {
        Wh_Log(L"Fade is in progress during uninit, waiting for it to complete...");
        WaitForFade();
    }
    if (g_fadeMutex) {
        CloseHandle(g_fadeMutex);
        g_fadeMutex = NULL;
    }
    HANDLE monitorOffThread = g_monitorOffThread.exchange(NULL);
    if (monitorOffThread) {
        WaitForSingleObject(monitorOffThread, INFINITE);
        CloseHandle(monitorOffThread);
    }
}

void Wh_ModSettingsChanged(BOOL* bReload) {
    BOOL prevSleepFadeEnabled = g_settings.sleepFadeEnabled;
    LoadSettings();
    if (!g_isWinlogon && prevSleepFadeEnabled != g_settings.sleepFadeEnabled) {
        // The global injection is enabled or disabled, need to reload the mod to unload it
        // (Reloading unloaded mod on mod setting change is automatically handled by Windhawk)
        *bReload = TRUE;
        g_isExiting.store(true); // Prevent new fades
        if (IsFadeInProgressInThisProcess()) {
            Wh_Log(L"Fade is in progress during settings change, waiting for it to complete...");
            WaitForFade();
        }
    }
}
