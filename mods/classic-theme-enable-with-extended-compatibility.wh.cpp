// ==WindhawkMod==
// @id              classic-theme-enable-with-extended-compatibility
// @name            Classic Theme Enable with extended compatibility
// @description     Enables classic theme, supports RDP sessions, and is compatible with early / system start of Windhawk
// @version         1.1
// @author          Roland Pihlakas
// @github          https://github.com/levitation-opensource/
// @homepage        https://www.simplify.ee/
// @include         winlogon.exe
// @compilerOptions -lntdll -lkernel32 -lwtsapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Theme Enable with extended compatibility

Enables classic theme in Windows 10 and 11. **This mod version adds support for RDP sessions and compatibility with early / system start of Windhawk.**

[Click here](#6-how-to-configure-system-start-of-windhawk) if you want to see instructions for configuring system start of Windhawk. More technical details about the update can be found at the end of this document.



## General info about classic theme in Windows 10 and 11

Classic theme still seems to be native to Windows. Essentially this mod disables themes, and this in turn reactivates the default behaviour of Windows, which is actually classic theme. 

There are few programs that have minor visual glitches. Various Windhawk mods deal with this and so these are solved as well.

The most important problematic program is Taskbar. Fortunately there are a couple of programs and a number of Windhawk mods, each fixing a different problem of classic theme in Taskbar. All these are mentioned together with links in the instructions below.

The only totally problematic program uncompatible with classic theme is Task Manager. There exists alternative software which is able to handle Ctrl-Alt-Del as well, also mentioned below.

Other programs have been running fine, I have been using classic theme for about a few months. Right now my systems look entirely classic (except for programs that have their own built-in themes). I am quite intensive user using many different programs.

In summary, there are certain additional steps you need to do in order for your computer to be fully adjusted for nicer classic theme UI appearance. See the next section ["Instructions for setting up the classic theme"](#instructions-for-setting-up-the-classic-theme) for detailed instructions. Quick summary here:
1) A few registry parameters that need to be adjusted.
2) Some additional software needs to be installed.
3) Windhawk process inclusion settings need updating.
4) Install the current mod.
5) Additional Windhawk mods need to be installed.
6) Configure system start of Windhawk.
7) You may want to adjust the colours and fonts with a program mentioned in instructions.



# Instructions for setting up the classic theme

Note, upon first start, the mod affects only programs started after enabling the mod. **After you have finished the configuration steps below, you may want to restart your system.**



## 1. Needed registry changes

1. If you want the 3D borders in menus, import the following reg file:
    ```
    Windows Registry Editor Version 5.00
    [HKEY_CURRENT_USER\Control Panel\Desktop]
    "UserPreferencesMask"=hex:9E,1E,05,80,12,01,00,00
    ```

    This registry file import is needed because it sets some bits that cannot be configured via Windows control panel settings. Importing this reg file may slightly change the rest of your visual experience as a side effect. You can later further adjust the visual experience affected by this registry entry change via `Advanced System Settings control panel -> Performance -> Settings -> Visual Effects`. 

    *If you are curious, then the official though incomplete documentation of this registry entry [can be found here](https://learn.microsoft.com/en-us/previous-versions/windows/it-pro/windows-2000-server/cc957204%28v=technet.10%29). I have not found a more complete description, but the value provided above originates from the time-tested [tutorial here](https://github.com/valinet/ExplorerPatcher/discussions/167) and according to my testing, the above reg file results in a reasonable initial visual appearance.*

2. Delete or rename this registry key:\
    `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\DefaultColors`\
    to prevent colour scheme change after pressing Ctrl-Alt-Del or going to the logon screen and back.

3. Import the following reg file:
    ```
    Windows Registry Editor Version 5.00
    [HKEY_CURRENT_USER\Software\ExplorerPatcher]
    "OldTaskbar"=dword:00000001
    "SkinMenus"=dword:00000000
    "ToolbarSeparators"=dword:00000001
    "DisableImmersiveContextMenu"=dword:00000001
    "ClassicThemeMitigations"=dword:00000001
    [HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced]
    "TaskbarGlomLevel"=dword:00000002
    "MMTaskbarGlomLevel"=dword:00000002
    "TaskbarSmallIcons"=dword:00000001
    "TaskbarAl"=dword:00000000
    "TaskbarSD"=dword:00000001
    [HKEY_CURRENT_USER\Software\OpenShell\StartMenu\Settings]
    "StartButtonType"="ClasicButton"
    "CustomTaskbar"=dword:00000000
    "SkinC1"="Classic skin"
    "StartButtonIcon"="%SystemRoot%\System32\slui.exe, 2"
    ```



## 2. Additional needed software

* [System Informer / former Process Hacker](https://systeminformer.sourceforge.io/) or alternatively, [Classic Task Manager](https://win7games.com/#taskmgr) - Classic theme is not compatible with built-in Windows Task Manager. I recommend Process Hacker / System Informer since Classic Task Manager can cause lagging of the system. Process Hacker / System Informer can be configured to handle Ctrl-Alt-Del as well.
* [Explorer Patcher](https://github.com/valinet/ExplorerPatcher) - Together with a couple of mods listed in next section improves the Taskbar appearance.
* [Open-Shell-Menu](https://open-shell.github.io/Open-Shell-Menu/) - Needed to show Start Button in classic theme.
* [Classic Notepad](https://win7games.com/#notepad) - (Win 11 only - works better than built-in Notepad in Win 11).



## 3. Needed changes in Windhawk settings

Before you start installing the current mod and additional classic theme mods listed below, you need to update Windhawk process inclusion list, accessible via `Settings -> Advanced settings -> More advanced settings -> Process inclusion list`. Add the following rows:
```
conhost.exe
dllhost.exe
dwm.exe
winlogon.exe
```
Then click `"Save and restart Windhawk"` button.



## 4. Install the current mod

Click the "Install" button above.



## 5. Additional needed classic theme related mods

I recommend installing the following classic theme related mods in order to get full classic theme experience.

* [Aero Tray](https://windhawk.net/mods/aero-tray)
* [Classic Conhost](https://windhawk.net/mods/classic-conhost)
* [Classic Task Dialog Fix](https://windhawk.net/mods/classic-taskdlg-fix)
* [Classic Taskbar 3D buttons with extended compatibility](https://windhawk.net/mods/classic-taskbar-buttons-lite-vs-without-spacing) or alternatively, [Classic Taskbar 3D buttons Lite](https://windhawk.net/mods/classic-taskbar-buttons-lite)
* [Classic Taskbar background fix](https://windhawk.net/mods/classic-taskbar-background-fix) (simple to install) or alternatively, [@valinet's modified version of OpenShell StartMenuDLL.dll](https://github.com/valinet/ExplorerPatcher/discussions/167#discussioncomment-1616107) (requires more manual work to install)
* [Classic theme transparency fix](https://windhawk.net/mods/classic-theme-transparency-fix)
* [Classic UWP Fix](https://windhawk.net/mods/classic-uwp-fix)
* [Clientedge Everywhere](https://windhawk.net/mods/clientedge-in-apps)
* [Disable rounded corners in Windows 11](https://windhawk.net/mods/disable-rounded-corners) (Win 11 only)
* [DWM Ghost Mods](https://windhawk.net/mods/dwm-ghost-mods)
* [Fix browsers for Windows Classic theme](https://windhawk.net/mods/classic-browser-fix)
* [Fix Classic Theme Maximized Windows](https://windhawk.net/mods/classic-maximized-windows-fix)
* [Win32 Tray Clock Experience](https://windhawk.net/mods/win32-tray-clock-experience) (Win 10 only)

There are other classic theme related mods in Windhawk which I did not list here for one or other reason. Your experience and preferences may differ. After getting set up with above and feeling like exploring more, you may want to try the other mods out.



## 6. How to configure system start of Windhawk

Starting Windhawk early improves the probability that classic theme is enabled by the time Taskbar process is launched during login. 

*In contrast, when Windhawk is activated normally then there is increased chance that the Taskbar process starts before classic theme is enabled - then the Taskbar would not have classic appearance and the user needs to restart the Taskbar manually later in order to apply classic theme to Taskbar.*

Steps to enable system start of Windhawk:

1. Start Task Scheduler
2. Under "Task Scheduler Library" section find the row titled "WindhawkRunUITask", open it.
3. Go to Triggers
4. Click "New..." button
5. Select Begin the task: "At startup"
6. OK
7. OK



## 7. Adjusting the colours and fonts

For editing the colours and fonts I recommend the following program:\
[Desktop Themes v1.87](https://archive.org/details/desktop_themes_v187_ZIP).

All done. Congratulations! 

If this is the first time you installed classic theme, then you may want to reboot your computer now in order for the classic theme styles to be applied to all programs.



# Optional advanced reading

## More info about setting up classic theme

If you want, you can investigate the following older webpages which provide somewhat alternate instructions for setting up classic theme. The current instructions were partially based on these sources:
* [Enable Classic Theme by handle method](https://windhawk.net/mods/classic-theme-enable)
* [Tutorial: Use of Windows Classic theme with Windows 10, Windows 11 and Explorer Patcher](https://github.com/valinet/ExplorerPatcher/discussions/167)



## How this mod works

The mod disables visual styles, effectively enabling the Windows Classic theme. This mod uses the method of closing access to the memory area where the theme is located. It is one of the multiple methods of enabling Windows classic theme.

The mod injects only into the process winlogon.exe.



## Detailed description of the compatibility updates

This mod has the following two capabilities built on top of previous classic theme mod [Enable Classic Theme by handle method by @Anixx](https://windhawk.net/mods/classic-theme-enable): Improved support for RDP sessions and code for handling early mod load, including during system start.

1) If Windhawk loads too early during system startup with the original mod, then the classic theme initialisation would fail. At the same time, starting Windhawk early (during system startup, not during user login) will improve the chances that the classic theme is applied as soon as possible and no programs need to be restarted later to get classic theme applied. In order for the classic theme enable to succeed in these conditions, the mod needs to check for conditions, and if needed, wait a bit in case the system is not yet ready to apply classic theme.
2) With the original mod the RDP sessions often disconnected during connecting. This happened even if the session was already logged in and had classic theme already applied, but was currently in disconnected state. Each new RDP connection gets its own winlogon.exe process. The mod needs to wait for the session "active" state in case it is modding RDP session related winlogon.exe processes.
*/
// ==/WindhawkModReadme==

#include <iostream>
#include <sddl.h>
#include <winnt.h>
#include <winternl.h>
#include <aclapi.h>
#include <securitybaseapi.h>
#include <wtsapi32.h>


#ifndef WH_MOD
#define WH_MOD
#include <mods_api.h>
#endif


// Define the prototype for the NtOpenSection function.
extern "C" NTSTATUS NTAPI NtOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
);


bool g_retryInitInAThread = false;
HANDLE g_initThread = NULL;
HANDLE g_initThreadStopSignal = NULL;
ULONG g_maximumResolution;
ULONG g_originalTimerResolution;


typedef NTSTATUS(WINAPI* NtSetTimerResolution_t)(ULONG, BOOLEAN, PULONG);
NtSetTimerResolution_t pOriginalNtSetTimerResolution;

//this hook is needed to restore the resolution winlogon would have had if this mod would not have been installed
NTSTATUS WINAPI NtSetTimerResolutionHook(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution)
{
    if (!SetResolution) {
        return pOriginalNtSetTimerResolution(DesiredResolution, SetResolution, CurrentResolution);
    }

    g_originalTimerResolution = DesiredResolution;

    return pOriginalNtSetTimerResolution(DesiredResolution, SetResolution, CurrentResolution);
}

BOOL TryInit(bool* abort) {

    // Retrieve the current session ID for the process.
    DWORD sessionId;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId))
        return FALSE;     //retry


    if (sessionId != WTSGetActiveConsoleSessionId()) {

        //TODO: if the session is service session then quit the init thread AND do not retry - set the abort flag to true

        WTS_CONNECTSTATE_CLASS* pConnectState = NULL;
        DWORD bytesReturned;
        bool sessionConnected = false;
        if (
            WTSQuerySessionInformationW(
                WTS_CURRENT_SERVER_HANDLE,
                WTS_CURRENT_SESSION,
                WTSConnectState,
                (LPWSTR*)&pConnectState,
                &bytesReturned
            )
            && pConnectState
            && bytesReturned == sizeof(WTS_CONNECTSTATE_CLASS)
        ) {
            sessionConnected = (*pConnectState == WTSActive);
            //Wh_Log(L"Session connected: %ls", sessionConnected ? L"Yes" : L"No");
        }

        if (pConnectState)
            WTSFreeMemory(pConnectState);

        if (!sessionConnected)  //Modify RDP sessions only when they reach active state else RDP connections will fail
            return FALSE;     //retry
    }


    wchar_t sectionName[256];
    if (swprintf_s(sectionName, _countof(sectionName), L"\\Sessions\\%lu\\Windows\\ThemeSection", sessionId) == -1)
        return FALSE;     //retry

    // Define the name of the section object.
    UNICODE_STRING sectionObjectName;
    RtlInitUnicodeString(&sectionObjectName, sectionName);

    // Define the attributes for the section object.
    OBJECT_ATTRIBUTES objectAttributes;
    InitializeObjectAttributes(&objectAttributes, &sectionObjectName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    HANDLE hSection;
    NTSTATUS status = NtOpenSection(&hSection, WRITE_DAC, &objectAttributes);

    Wh_Log("status: %u\n", status);
    Wh_Log("%s", sectionName);

    if (!NT_SUCCESS(status))
        return FALSE;     //retry

    // Define your SDDL string.
    LPCWSTR sddl = L"O:BAG:SYD:(A;;RC;;;IU)(A;;DCSWRPSDRCWDWO;;;SY)";
    PSECURITY_DESCRIPTOR psd = NULL;

    // Convert the SDDL string to a security descriptor.
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl, SDDL_REVISION_1, &psd, NULL)) {
        CloseHandle(hSection);
        return FALSE;     //retry
    }

    // Set the security descriptor for the object.
    BOOL result = SetKernelObjectSecurity(
        hSection,
        DACL_SECURITY_INFORMATION,
        psd
    );

    Wh_Log("result: %u\n", result);

    // Cleanup: free allocated security descriptor memory and close the handle.
    LocalFree(psd);
    CloseHandle(hSection);


    return result;     //retry if SetKernelObjectSecurity failed
}

void RestoreTimerResolution() {

    ULONG CurrentResolution;
    if (NT_SUCCESS(pOriginalNtSetTimerResolution(g_originalTimerResolution, TRUE, &CurrentResolution))) {
        Wh_Log(L"NtSetTimerResolution success");
    }
    else {
        Wh_Log(L"NtSetTimerResolution failed");
    }

    //TODO: call Wh_RemoveFunctionHook as well
}

DWORD WINAPI InitThreadFunc(LPVOID param) {

    Wh_Log(L"InitThreadFunc enter");

    bool isRetry = false;
retry:  //If Windhawk loads the mod too early then the classic theme initialisation will fail. Therefore we need to loop until the initialisation succeeds. Also if the mod is loaded into a RDP session too early then for some reason that would block the RDP session from successfully connecting. So we need to wait for session "active" state in case of RDP sessions. This is another reason for having a loop here.
    if (isRetry) {
        if (WaitForSingleObject(g_initThreadStopSignal, 1) != WAIT_TIMEOUT) {
            Wh_Log(L"Shutting down InitThreadFunc before success");
            RestoreTimerResolution();
            return FALSE;
        }
    }
    isRetry = true;

    bool abort = false;
    BOOL result;
    if (TryInit(&abort)) {
        RestoreTimerResolution();
        return TRUE;  //hooks done
    }
    else if (abort) {
        RestoreTimerResolution();
        return FALSE;   //a service session?
    }
    else {      //If Windhawk loads the mod too early then the classic theme initialisation will fail. Therefore we need to loop until the initialisation succeeds. Also if the mod is loaded into a RDP session too early then for some reason that would block the RDP session from successfully connecting. So we need to wait for session "active" state in case of RDP sessions. This is another reason for having a loop here.
        goto retry;
    }
}

BOOL Wh_ModInit() {

    Wh_Log(L"Init");


    bool abort = false;
    if (TryInit(&abort)) {
        return TRUE;  //hooks done
    }
    else if (abort) {
        return FALSE;   //a service session?
    }
    else {      //If Windhawk loads the mod too early then the classic theme initialisation will fail. Therefore we need to loop until the initialisation succeeds. Also if the mod is loaded into a RDP session too early then for some reason that would block the RDP session from successfully connecting. So we need to wait for session "active" state in case of RDP sessions. This is another reason for creating a separate thread with a loop.

        HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
        if (!hNtdll) {
            return FALSE;   //TODO: ignore this failure
        }

        FARPROC pNtSetTimerResolution = GetProcAddress(hNtdll, "NtSetTimerResolution");
        FARPROC pNtQueryTimerResolution = GetProcAddress(hNtdll, "NtQueryTimerResolution");
        if (
            !pNtSetTimerResolution
            || !pNtQueryTimerResolution
            ) {
            return FALSE;   //TODO: ignore this failure
        }

        ULONG MinimumResolution;
        ULONG MaximumResolution;
        ULONG CurrentResolution;
        NTSTATUS status = ((NTSTATUS(WINAPI*)(PULONG, PULONG, PULONG))pNtQueryTimerResolution)(
            &MinimumResolution, &MaximumResolution, &CurrentResolution);
        if (!NT_SUCCESS(status)) {
            Wh_Log(L"NtQueryTimerResolution failed with status: 0x%X", status);
            return FALSE;   //TODO: ignore this failure
        }

        Wh_Log(L"NtQueryTimerResolution: min=%f, max=%f, current=%f",
            (double)MinimumResolution / 10000.0,
            (double)MaximumResolution / 10000.0,
            (double)CurrentResolution / 10000.0);
        g_maximumResolution = MaximumResolution;
        g_originalTimerResolution = CurrentResolution;

        Wh_SetFunctionHook((void*)pNtSetTimerResolution, (void*)NtSetTimerResolutionHook, (void**)&pOriginalNtSetTimerResolution);  //NB! we do not check result of hooking, lets proceed with init thread creation in any case


        g_initThreadStopSignal = CreateEvent(
            /*lpEventAttributes = */NULL,           // default security attributes
            /*bManualReset = */TRUE,				// manual-reset event
            /*bInitialState = */FALSE,              // initial state is nonsignaled
            /*lpName = */NULL						// object name
        );

        if (!g_initThreadStopSignal) {
            Wh_Log(L"CreateEvent failed");
            return FALSE;
        }

        g_initThread = CreateThread(
            /*lpThreadAttributes = */NULL,
            /*dwStackSize = */0,
            InitThreadFunc,
            /*lpParameter = */NULL,
            /*dwCreationFlags = */CREATE_SUSPENDED, 	//The thread does NOT run immediately after creation. This is in order to avoid any race conditions in calling Wh_ApplyHookOperations() before Wh_ModInit() has completed and Wh_ModAfterInit() has started
            /*lpThreadId = */NULL
        );

        if (g_initThread) {
            Wh_Log(L"InitThread created");
            g_retryInitInAThread = true;
            return TRUE;    //activate NtSetTimerResolution hook, then proceed activating the thread
        }
        else {
            Wh_Log(L"CreateThread failed");
            CloseHandle(g_initThreadStopSignal);
            g_initThreadStopSignal = NULL;
            return FALSE;
        }
    }
}

void Wh_ModAfterInit(void) {

    if (g_retryInitInAThread) {   //Are we in the proces of creating the init thread? If g_initThreadStopSignal is NULL then we are not going to create init thread.

        //The mod needs to know the desired resolution set by the program. In order to avoid any race conditions, force timer resolution update only after the hook is activated. Else there might be a situation that the mod overrides the current resolution while hook is not yet set, and then the program changes it again before the hook is finally set, but the mod does not know that a new desired resolution was set by the program and therfore cannot restore it when init thread finishes.

        ULONG CurrentResolution;
        if (NT_SUCCESS(pOriginalNtSetTimerResolution(g_maximumResolution, TRUE, &CurrentResolution))) {
            Wh_Log(L"NtSetTimerResolution success");
            g_originalTimerResolution = CurrentResolution;
        }
        else {
            Wh_Log(L"NtSetTimerResolution failed");
        }

        if (ResumeThread(g_initThread)) {
            Wh_Log(L"ResumeThread successful");
            g_retryInitInAThread = false;
        }
        else {
            Wh_Log(L"ResumeThread failed");
        }
    }
}

void Wh_ModUninit() {

    Wh_Log(L"Uniniting...");

    if (g_initThread) {

        if (g_retryInitInAThread) {     //was the thread successfully resumed?
            if (ResumeThread(g_initThread))
                g_retryInitInAThread = false;
        }

        if (!g_retryInitInAThread) {     //was the thread successfully resumed?
            SetEvent(g_initThreadStopSignal);
            WaitForSingleObject(g_initThread, INFINITE);
            CloseHandle(g_initThread);
            g_initThread = NULL;
        }
    }

    if (
        g_initThreadStopSignal 
        && !g_retryInitInAThread     //was the thread successfully resumed?
    ) {
        CloseHandle(g_initThreadStopSignal);
        g_initThreadStopSignal = NULL;
    }

    Wh_Log(L"Uninit complete");
}
