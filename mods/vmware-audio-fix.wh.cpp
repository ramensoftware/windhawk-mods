// ==WindhawkMod==
// @id              vmware-audio-fix
// @name            VMware Audio Distortion Fix
// @name:zh-CN      VMware 音频失真修复
// @name:fr-FR      Correction de distorsion audio VMware
// @description     Fixes distorted audio in VMware virtual machines by setting high-resolution timers.
// @description:zh-CN 通过设置高分辨率定时器修复 VMware 虚拟机中的失真音频。
// @description:fr-FR Corrige la distorsion audio dans les machines virtuelles VMware en utilisant des temporiseurs à haute résolution.
// @version         1.0
// @author          Starfield
// @github          https://github.com/steveji2021
// @include         vmware-vmx.exe
// @include         vmware-vmx-debug.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# VMware Audio Distortion Fix

### This project is inspired by [VMAudioBackHost](https://github.com/Raymai97/VMAudioBackHost). Thanks to Raymai97

Fixes distorted audio in VMware virtual machines by setting a high-resolution
timer in the vmware-vmx.exe process.

## Background

Windows 10 build 2004+ made timer resolution per-process. VMware virtual
machines may experience distorted audio because the vmware-vmx.exe process
uses the default low timer resolution (15.625 ms). This mod injects into
vmware-vmx.exe and calls `NtSetTimerResolution` to set a high-resolution
timer, fixing the audio stuttering/distortion.

## Usage

1. Install this mod via [Windhawk](https://windhawk.net).
2. The mod will automatically inject into all running and future
   vmware-vmx.exe processes.
3. Configure the desired timer resolution in the mod settings.

The recommended setting is 1.0 ms, which provides smooth audio without
significant impact on power consumption.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- timerResolution: "1.0"
  $name: Timer resolution (ms)
  $description: >-
    The desired timer resolution in milliseconds. Lower values reduce audio
    stuttering but may slightly increase power consumption.
  $options:
    - "0.5": 0.5 ms
    - "1.0": 1.0 ms (Recommended)
    - "2.0": 2.0 ms
    - "5.0": 5.0 ms
    - "10.0": 10.0 ms
    - "15.625": 15.625 ms (System default)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

typedef LONG NTSTATUS;

static ULONG g_desiredTimerRes100ns = 10000;

static double ParseMsString(PCWSTR str)
{
    return wcstod(str, nullptr);
}

static void SetTimerResolution(void)
{
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll)
    {
        Wh_Log(L"Failed to get ntdll.dll handle");
        return;
    }

    NTSTATUS (WINAPI *NtQueryTimerResolution)(ULONG*, ULONG*, ULONG*) =
        (NTSTATUS (WINAPI*)(ULONG*, ULONG*, ULONG*))
        GetProcAddress(ntdll, "NtQueryTimerResolution");
    NTSTATUS (WINAPI *NtSetTimerResolution)(ULONG, BOOLEAN, ULONG*) =
        (NTSTATUS (WINAPI*)(ULONG, BOOLEAN, ULONG*))
        GetProcAddress(ntdll, "NtSetTimerResolution");

    if (!NtQueryTimerResolution || !NtSetTimerResolution)
    {
        Wh_Log(L"NtQueryTimerResolution or NtSetTimerResolution not found");
        return;
    }

    ULONG minRes, maxRes, currRes;
    NTSTATUS status = NtQueryTimerResolution(&minRes, &maxRes, &currRes);
    if (status != 0)
    {
        Wh_Log(L"NtQueryTimerResolution failed, status=%lx", status);
        return;
    }

    Wh_Log(L"Current: min=%lu max=%lu curr=%lu", minRes, maxRes, currRes);

    status = NtSetTimerResolution(g_desiredTimerRes100ns, TRUE, &currRes);
    if (status != 0)
    {
        Wh_Log(L"NtSetTimerResolution(%lu) failed, status=%lx",
               g_desiredTimerRes100ns, status);
        return;
    }

    Wh_Log(L"Timer resolution set to %lu (%.3f ms)",
           g_desiredTimerRes100ns,
           g_desiredTimerRes100ns / 10000.0);
}

static void ReleaseTimerResolution(void)
{
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll)
    {
        return;
    }

    NTSTATUS (WINAPI *NtSetTimerResolution)(ULONG, BOOLEAN, ULONG*) =
        (NTSTATUS (WINAPI*)(ULONG, BOOLEAN, ULONG*))
        GetProcAddress(ntdll, "NtSetTimerResolution");

    if (!NtSetTimerResolution)
    {
        return;
    }

    ULONG currRes;
    NtSetTimerResolution(g_desiredTimerRes100ns, FALSE, &currRes);
    Wh_Log(L"Timer resolution released");
}

static void LoadSettings(void)
{
    WindhawkUtils::StringSetting resStr = WindhawkUtils::StringSetting::make(L"timerResolution");
    double resMs = ParseMsString(resStr.get());
    if (resMs < 0.5)  resMs = 0.5;
    if (resMs > 15.625) resMs = 15.625;
    g_desiredTimerRes100ns = (ULONG)(resMs * 10000.0);

    Wh_Log(L"Settings: timerResolution=%s (%.3f ms, %lu 100ns units)",
           resStr.get(), resMs, g_desiredTimerRes100ns);
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Initializing");
    LoadSettings();
    SetTimerResolution();
    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    Wh_Log(L"Settings changed");
    LoadSettings();
    ReleaseTimerResolution();
    SetTimerResolution();
}

void Wh_ModUninit(void)
{
    Wh_Log(L"Uninitializing");
    ReleaseTimerResolution();
}
