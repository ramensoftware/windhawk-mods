// ==WindhawkMod==
// @id              win7-login-fade
// @name            Old Logon Fade
// @description     Bring back the old login screen fade effect
// @version         1.1
// @author          Ingan121
// @github          https://github.com/Ingan121
// @twitter         https://twitter.com/Ingan121
// @homepage        https://www.ingan121.com/
// @include         winlogon.exe
// @architecture    x86-64
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Old Login Fade
* This mod restores the old login screen fade effect found in Windows 7 and earlier.
* The fade effect is implemented by adjusting the screen brightness using the SetDeviceGammaRamp function, the same method used by the original fade effect.
* You need to set the `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\ICM\GdiIcmGammaRange` registry value to `0x100` (DWORD, 256 in decimal) to allow brightness values below the normal level, which is required for the fade effect to work naturally.
* Will not work with Microsoft Basic Display Adapter, VMware SVGA 3D, and some other display drivers that do not support gamma adjustment. Also not compatible with NVIDIA driver's reference color mode.
## Known issues
* The fade animation may look broken on early boot when using the auto-login feature.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration: 1000
  $name: Fade duration (ms)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

int g_duration = 1000;

typedef struct _MONITOR_INFO {
    HDC hDC = NULL;
    WORD origGamma[3][256];
} MONITOR_INFO;

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

void FadeDesktop(int fps, int duration, BOOL fadeOut, MONITOR_INFO* monitors, int monitorCount) {
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
    return refreshRate;
}

typedef __int64 __fastcall (*SwitchDesktopWithFade_t)(HDESK hDesktop, DWORD duration);
SwitchDesktopWithFade_t SwitchDesktopWithFade_original;
__int64 __fastcall SwitchDesktopWithFade_hook(HDESK hDesktop, DWORD duration) {
    Wh_Log(L"SwitchDesktopWithFade, duration=%d", duration);
    if (g_duration == 0) {
        return SwitchDesktop(hDesktop);
    }

    int refreshRate = GetDeviceRefreshRate();

    int monitorCount = 0;
    MONITOR_INFO* monitors = NULL;
    DISPLAY_DEVICEW dd;
    dd.cb = sizeof(dd);
    for (DWORD i = 0; EnumDisplayDevicesW(NULL, i, &dd, 0); i++) {
        if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE) {
            monitorCount++;
            MONITOR_INFO* monitorsRealloc = (MONITOR_INFO*)realloc(monitors, sizeof(MONITOR_INFO) * monitorCount);
            if (!monitorsRealloc) {
                Wh_Log(L"Failed to allocate memory for monitors");
                for (int j = 0; j < monitorCount - 1; j++) {
                    if (monitors[j].hDC) {
                        DeleteDC(monitors[j].hDC);
                    }
                }
                free(monitors);
                return SwitchDesktop(hDesktop);
            }
            monitors = monitorsRealloc;
            MONITOR_INFO* mi = &monitors[monitorCount - 1];
            memset(mi, 0, sizeof(MONITOR_INFO));
            HDC hDC = CreateDCW(dd.DeviceName, NULL, NULL, NULL);
            if (hDC) {
                mi->hDC = hDC;
                if (!GetDeviceGammaRamp(hDC, mi->origGamma)) {
                    DeleteDC(hDC);
                    mi->hDC = NULL;
                }
            }
        }
        dd.cb = sizeof(dd);
    }

    FadeDesktop(refreshRate, g_duration / 2, TRUE, monitors, monitorCount);
    BOOL result = SwitchDesktop(hDesktop);
    FadeDesktop(refreshRate, g_duration / 2, FALSE, monitors, monitorCount);

    for (int i = 0; i < monitorCount; i++) {
        if (monitors[i].hDC) {
            DeleteDC(monitors[i].hDC);
        }
    }
    free(monitors);

    // Original SwitchDesktopWithFade usually returns 1; probably it's also WINBOOL like SwitchDesktop
    return result;
}

void LoadSettings() {
    g_duration = Wh_GetIntSetting(L"duration");

    if (g_duration <= 0) {
        g_duration = 0;
    }
    if (g_duration > 10000) {
        g_duration = 1000;
    }
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

    SwitchDesktopWithFade_t SwitchDesktopWithFade = (SwitchDesktopWithFade_t)GetProcAddress(user32, "SwitchDesktopWithFade");
    if (!SwitchDesktopWithFade) {
        Wh_Log(L"GetProcAddress SwitchDesktopWithFade failed");
        return FALSE;
    }

    if (!Wh_SetFunctionHook((void*)SwitchDesktopWithFade, (void*)SwitchDesktopWithFade_hook, (void**)&SwitchDesktopWithFade_original)) {
        Wh_Log(L"Wh_SetFunctionHook SwitchDesktopWithFade failed");
        return FALSE;
    }
    Wh_Log(L"Init OK");

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
