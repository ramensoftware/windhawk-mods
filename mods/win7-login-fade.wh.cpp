// ==WindhawkMod==
// @id              win7-login-fade
// @name            Old Login Fade
// @description     Bring back the old login screen fade effect
// @version         1.0
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
* Will not work with Microsoft Basic Display Adapter, VMware SVGA 3D, and some other display drivers that do not support gamma adjustment.
## Known issues
* The fade animation may look broken on early boot when using the auto-login feature.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration: 1000
  $name: Fade duration (ms)
- fadeOnLock: true
  $name: Fade when locking
  $description: Whether to fade when locking the computer. Please disable the lock screen for the smoothest animations.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <thread>

int g_duration = 1000;
BOOL g_fadeOnLock = TRUE;
BOOL g_isLocking = FALSE;
BOOL g_isLockFadeInProgress = FALSE;
BOOL g_switchDesktopCalledDuringLockFade = FALSE;

// https://www.nirsoft.net/vc/change_screen_brightness.html
void SetBrightness(WORD brightness) { // 0: black, 256: normal, 256+: brighter
    WORD gammaArray[3][256];
    for (int i = 0; i < 256; i++)
    {
        int val = i * brightness;
        if (val > 65535) {
            val = 65535;
        }
        gammaArray[0][i] = (WORD)val;
        gammaArray[1][i] = (WORD)val;
        gammaArray[2][i] = (WORD)val;
        
    }
    // Call SetDeviceGammaRamp for each monitor, otherwise it'll not work properly in multi-monitor setup
    DISPLAY_DEVICEW dd;
    dd.cb = sizeof(dd);
    for (DWORD i = 0; EnumDisplayDevicesW(NULL, i, &dd, 0); i++) {
        if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE) {
            HDC hDC = CreateDCW(dd.DeviceName, NULL, NULL, NULL);
            if (hDC) {
                // Win32k also uses this function for the old fade
                SetDeviceGammaRamp(hDC, gammaArray);
                DeleteDC(hDC);
            }
        }
    }
}

void FadeDesktop(int fps, int duration, BOOL fadeOut) {
    LARGE_INTEGER startTime;
    QueryPerformanceCounter(&startTime);
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    int frameStep = 1000 / fps;
    while (true) {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        double elapsedSeconds = (double)(currentTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
        if (elapsedSeconds >= duration / 1000.0) {
            break;
        }
        double progress = elapsedSeconds / (duration / 1000.0);
        if (fadeOut) {
            progress = 1.0 - progress;
        }
        WORD brightness = (WORD)(progress * 256);
        SetBrightness(brightness);
        Sleep(frameStep);
    }
}

int GetDeviceRefreshRate() {
    HDC hdc = GetDC(NULL);
    int refreshRate = GetDeviceCaps(hdc, VREFRESH);
    ReleaseDC(NULL, hdc);
    if (refreshRate <= 0) {
        refreshRate = 60;
    }
    return refreshRate;
}

using SwitchDesktop_t = decltype(&SwitchDesktop);
SwitchDesktop_t SwitchDesktop_original;
BOOL WINAPI SwitchDesktop_hook(HDESK hDesktop) {
    if (g_duration == 0) {
        return SwitchDesktop_original(hDesktop);
    }
    WCHAR desktopName[256];
    if (GetUserObjectInformationW(hDesktop, UOI_NAME, desktopName, sizeof(desktopName), NULL)) {
        Wh_Log(L"SwitchDesktop called, desktop=%s", desktopName);
        if (g_isLocking && _wcsicmp(desktopName, L"Winlogon") == 0) {
            g_isLocking = FALSE;
            std::thread([hDesktop] {
                // LogonUI starts after this function returns, so do the fade asynchronously to let it start earlier
                g_isLockFadeInProgress = TRUE;
                int refreshRate = GetDeviceRefreshRate();
                FadeDesktop(refreshRate, g_duration / 2, TRUE);
                if (g_switchDesktopCalledDuringLockFade) {
                    // On Win10+, SwitchDesktop is called twice during lock, once for the secure desktop and once for the normal desktop, which hosts LockApp.exe
                    // So, for the second call, just skip the first call, as the second call is switching to the normal desktop, which we are currently in when fading
                    // This does not look that good, as LockApp startup is visible during the fade, but doing it synchronously causes a quiet a long time of black screen before the lock screen appears
                    // SwitchDesktop is only called once when the lock screen is disabled with GPO/registry, so make sure it still works in that case
                    g_switchDesktopCalledDuringLockFade = FALSE;
                } else {
                    SwitchDesktop_original(hDesktop);
                }
                FadeDesktop(refreshRate, g_duration / 2, FALSE);
                g_isLockFadeInProgress = FALSE;
            }).detach();
            return TRUE;
        } else if (g_isLockFadeInProgress && _wcsicmp(desktopName, L"Default") == 0) {
            g_switchDesktopCalledDuringLockFade = TRUE;
        } else {
            // Includes locking from Ctrl+Alt+Del, which may disrupt the expected SwitchDesktop call order mentioned above
            g_isLocking = FALSE;
        }
    } else {
        Wh_Log(L"SwitchDesktop called, GetUserObjectInformationW failed");
    }
    return SwitchDesktop_original(hDesktop);
};

typedef __int64 (*SwitchDesktopWithFade_t)(HDESK hDesktop, DWORD duration);
SwitchDesktopWithFade_t SwitchDesktopWithFade_original;
__int64 SwitchDesktopWithFade_hook(HDESK hDesktop, DWORD duration) {
    Wh_Log(L"SwitchDesktopWithFade, duration=%d", duration);
    if (g_duration == 0) {
        return SwitchDesktop_original(hDesktop);
    }
    int refreshRate = GetDeviceRefreshRate();
    FadeDesktop(refreshRate, g_duration / 2, TRUE);
    BOOL result = SwitchDesktop_original(hDesktop);
    FadeDesktop(refreshRate, g_duration / 2, FALSE);
    return result;
}

// __int64 __fastcall WLGeneric_InitiateLock_Execute(struct _StateMachineCallContext *a1)
typedef __int64 __fastcall (*WLGeneric_InitiateLock_Execute_t)(void* a1);
WLGeneric_InitiateLock_Execute_t WLGeneric_InitiateLock_Execute_original;
__int64 __fastcall WLGeneric_InitiateLock_Execute_hook(void* a1) {
    if (g_fadeOnLock && g_duration != 0) {
        g_isLocking = TRUE;
        g_switchDesktopCalledDuringLockFade = FALSE;
    }
    return WLGeneric_InitiateLock_Execute_original(a1);
};

void LoadSettings() {
    g_duration = Wh_GetIntSetting(L"duration");
    g_fadeOnLock = Wh_GetIntSetting(L"fadeOnLock");

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

    if (!Wh_SetFunctionHook((void*)SwitchDesktop, (void*)SwitchDesktop_hook, (void**)&SwitchDesktop_original)) {
        Wh_Log(L"Wh_SetFunctionHook SwitchDesktop failed");
        return FALSE;
    }
    
    HMODULE winlogon = GetModuleHandleW(NULL);
    if (winlogon) {
        WindhawkUtils::SYMBOL_HOOK winlogonExeHooks[] = {
            {
                {
                    L"unsigned long __cdecl WLGeneric_InitiateLock_Execute(struct _StateMachineCallContext *)",
                },
                &WLGeneric_InitiateLock_Execute_original,
                WLGeneric_InitiateLock_Execute_hook,
                FALSE
            }
        };
        if (!WindhawkUtils::HookSymbols(winlogon, winlogonExeHooks, ARRAYSIZE(winlogonExeHooks))) {
            Wh_Log(L"HookSymbols WLGeneric_InitiateLock_Execute failed");
            // This is not critical, just missing the fade on Win+L
        }
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
