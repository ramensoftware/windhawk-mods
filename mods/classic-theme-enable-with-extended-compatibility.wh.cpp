// ==WindhawkMod==
// @id              classic-theme-enable-with-extended-compatibility
// @name            Classic Theme Enable with extended compatibility
// @description     Enables classic theme. Supports Remote Desktop sessions and is compatible with early / system start of Windhawk.
// @version         1.2.1
// @author          Roland Pihlakas
// @github          https://github.com/levitation
// @homepage        https://www.simplify.ee/
// @include         winlogon.exe
// @compilerOptions -lntdll -lkernel32 -lwtsapi32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/levitation-opensource/my-windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/levitation-opensource/my-windhawk-mods/
//
// This mod is based on Classic Theme Enable by @Anixx, available here:
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/classic-theme-enable.wh.cpp
// This original mod was implicitly released under MIT license, according 
// to the rules specified at https://github.com/ramensoftware/windhawk-mods

// ==WindhawkModReadme==
/*
# Classic Theme Enable with extended compatibility

Enables classic theme in Windows 10 and 11. **This mod version adds support for Remote Desktop sessions and compatibility with early / system start of Windhawk.**

[Click here](#6-how-to-configure-system-start-of-windhawk) if you want to see instructions for configuring system start of Windhawk. 

If you already used Classic Theme Enable mod earlier then you do not need to read the ["Instructions for setting up the classic theme"](#instructions-for-setting-up-the-classic-theme) section below in order to use this mod. You can just replace the previous mod with the current version, and you will get Remote Desktop support and early / system start compatibility.

More technical details about the update can be found at the end of this document.


## A screenshot

![A screenshot](https://raw.githubusercontent.com/levitation-opensource/my-windhawk-mods/main/screenshots/classic-theme-enable-with-extended-compatibility.png)


## General info about classic theme in Windows 10 and 11

Classic theme still seems to be native to Windows. Essentially this mod disables themes, and this in turn reactivates the default behaviour of Windows, which is actually classic theme. 

There are few programs that have minor visual glitches. Various Windhawk mods deal with this and so these are solved as well.

The most important problematic program is Taskbar. Fortunately there are a couple of programs and a number of Windhawk mods, each fixing a different problem of classic theme in Taskbar. All these are mentioned together with links in the instructions below.

The only totally problematic program incompatible with classic theme is Task Manager. There exists alternative software which is able to handle Ctrl-Alt-Del as well, also mentioned below.

Other programs have been running fine, I have been using classic theme for about a few months. Right now my systems look entirely classic (except for programs that have their own built-in themes). I am quite intensive user using many different programs.

In summary, there are certain additional steps you need to do in order for your computer to be fully adjusted for nicer classic theme UI appearance. See the next section ["Instructions for setting up the classic theme"](#instructions-for-setting-up-the-classic-theme) for detailed instructions. Quick summary here:
1) A few registry parameters that need to be adjusted.
2) Some additional software needs to be installed.
3) Windhawk process inclusion settings need updating.
4) Install the current mod.
5) Additional Windhawk mods need to be installed.
6) Configure system start of Windhawk.
7) You may want to adjust the colours and fonts with a program mentioned in the instructions.
8) Adjust Taskbar features.



# Instructions for setting up the classic theme

If you already used Classic Theme Enable mod earlier then you do not need to read this section in order to use this mod. You can just replace the previous mod with the current version, and you will get Remote Desktop support and early / system start compatibility.

Note, upon first start, the mod affects only programs started after enabling the mod. Therefore your system might look weird here and there during performing the following installation steps and until you reboot. **After you have finished the configuration steps below, you may want to restart your system.**



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
    [HKEY_CURRENT_USER\Control Panel\Desktop\WindowMetrics]
    "BorderWidth"="-15"
    "PaddedBorderWidth"="0"
    [HKEY_CURRENT_USER\Software\ExplorerPatcher]
    "OldTaskbar"=dword:00000001
    "SkinMenus"=dword:00000000
    "ToolbarSeparators"=dword:00000001
    "DisableImmersiveContextMenu"=dword:00000001
    "ClassicThemeMitigations"=dword:00000001
    [HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer]
    "AltTabSettings"=dword:00000001
    [HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced]
    "TaskbarGlomLevel"=dword:00000002
    "MMTaskbarGlomLevel"=dword:00000002
    "TaskbarSmallIcons"=dword:00000001
    "TaskbarAl"=dword:00000000
    "TaskbarSD"=dword:00000001
    [HKEY_CURRENT_USER\Software\OpenShell\StartMenu\Settings]
    "StartButtonType"="ClasicButton"	; yes, Clasic should be here with one "s"
    "CustomTaskbar"=dword:00000000    
    "SkinC1"="Classic skin"
    "SkinW7"="Classic skin"
    "EnableStartButton"=dword:00000001
    "StartButtonIcon"="%SystemRoot%\System32\slui.exe, 2"
    ```



## 2. Additional needed software

* [System Informer / former Process Hacker](https://systeminformer.sourceforge.io/) or alternatively, [Classic Task Manager](https://win7games.com/#taskmgr) - Classic theme is not compatible with built-in Windows Task Manager. I recommend Process Hacker / System Informer since Classic Task Manager can cause lagging of the system. Process Hacker / System Informer can be configured to handle Ctrl-Alt-Del as well. 
* [Explorer Patcher](https://github.com/valinet/ExplorerPatcher) - Together with a couple of mods listed in next section improves the Taskbar appearance. Until you install all necessary mods, your Taskbar may become black. Do not let that disturb you.
* [Open-Shell-Menu](https://open-shell.github.io/Open-Shell-Menu/) - Needed to show Start Button in classic theme.
* [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/) - Allows adjusting various Taskbar features.
* [Classic Notepad](https://win7games.com/#notepad) - (Win 11 only - works better than built-in Notepad under Win 11 in case of classic theme).

You can set Process Hacker as Ctrl-Alt-Del handler with the following steps. The steps for System Informer are probably similar in nature, though the labels of items might be a bit different. `Open Process Hacker -> open "Hacker" menu -> Options -> Advanced -> check "Replace Task Manager with Process Hacker" -> OK`.



## 3. Needed changes in Windhawk settings

**Note, enabling classic theme requires Windhawk to be installed, not just run as a portable version.** Portable version of Windhawk will have insufficient privileges to enable classic theme.

Before you start installing the current mod and additional classic theme mods listed below, you need to update Windhawk process inclusion list, accessible via `Windhawk -> Settings -> Advanced settings -> More advanced settings -> Process inclusion list`. Add the following rows:
```
conhost.exe
dllhost.exe
dwm.exe
winlogon.exe
```
Then click `"Save and restart Windhawk"` button.



## 4. Install the current mod

Click the "Install" button above the current mod's description.



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
* [Disable Immersive Context Menus](https://windhawk.net/mods/disable-immersive-context-menus)
* [Disable rounded corners in Windows 11](https://windhawk.net/mods/disable-rounded-corners) (Win 11 only)
* [DWM Ghost Mods](https://windhawk.net/mods/dwm-ghost-mods) - Under this mod's Settings enable `"Use classic theme"` option, then click `"Save settings"` button.
* [Fix browsers for Windows Classic theme](https://windhawk.net/mods/classic-browser-fix)
* [Fix Classic Theme Maximized Windows](https://windhawk.net/mods/classic-maximized-windows-fix)
* [Win32 Tray Clock Experience](https://windhawk.net/mods/win32-tray-clock-experience) (Win 10 only)

There are other classic theme related mods in Windhawk which I did not list here for one or other reason. Your experience and preferences may differ. After getting set up with above and feeling like exploring more, you may want to try the other mods out.

After installing the above mentioned mods, your Windhawk window should look similar to the following screenshot. Of course you may have other mods installed from before as well.

![A Windhawk screenshot with classic theme mods](https://raw.githubusercontent.com/levitation-opensource/my-windhawk-mods/main/screenshots/classic-theme-mods-in-windhawk.png)



## 6. How to configure system start of Windhawk

Starting Windhawk early improves the probability that classic theme is enabled by the time Taskbar process is launched during login. 

*In contrast, when Windhawk is activated normally then there is increased chance that the Taskbar process starts before classic theme is enabled - then the Taskbar would not have classic appearance and the user needs to restart the Taskbar manually later in order to apply classic theme to Taskbar.*


### A safe method

Steps to configure system start of Windhawk service:

1. Start Task Scheduler
2. Left click on "Task Scheduler Library" in the tree
3. Then right click on it
4. Choose "Create Task..."
5. Enter to "Name" field: "Start Windhawk service" (without quotes)
6. Under "Security options" choose "Run whether user is logged on or not"
7. Go to "Triggers" section
8. Press "New..." button
9. Under "Begin the task:" choose "At startup"
10. Press OK
11. Go to "Actions" section
12. Press "New..." button
13. Enter to "Program/script" field: "net start windhawk" (without quotes)
14. Press OK
15. A popup appears with a question "It appears as though arguments have been included ..."
16. Press "Yes" button
17. Press OK
18. Enter an username and password with admin permissions
19. Press OK

If this is not yet sufficient to get classic theme enabled by the right time during system boot, then there is one more thing you can try:

1. Open the Settings app
2. Search "Sign-in options"
3. Click "Sign-in options" in the results list. Click "Show all results" to see all search results if necessary.
4. Turn off the option which reads: 
    * In Windows 10: "Use my sign-in info to automatically finish setting up my device after an update or restart".
    * In Windows 11: "Use my sign-in info to automatically finish setting up after an update".
   
   The title of this option may vary across operation system versions.
5. Each time you boot your computer and the password prompt appears, wait a little before you log in.


### A more effective, but somewhat less safe method

If you are not happy with the results from the above instructions then there is a final method that should provide you the timely start of Windhawk 100% of time. This method guarantees that Windhawk will start even earlier.

This method sets Plug and Play service dependent on Windhawk service.

But there is a slight risk related to this method. If an antivirus removes Windhawk then your computer will not be able to detect hardware changes. It will probably still boot though and you will still be able to log in, but use this method at your own risk and responsibility.

Import the following registry file:

```
Windows Registry Editor Version 5.00
[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\PlugPlay]
; This is a REG_MULTI_SZ type with a value "Windhawk". This registry entry would not work with a REG_SZ type.
"DependOnService"=hex(7):57,00,69,00,6e,00,64,00,68,00,61,00,77,00,6b,00,00,00,00,00
```

It does not matter whether Plug and Play service is configured to Manual start or Automatic start. It will start immediately at the system boot regardless. By default, it is configured as Manual start and you can keep it like that.

If you ever uninstall Windhawk or your antivirus removes Windhawk, then remove/rename this `"DependOnService"` registry value from `HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\PlugPlay`



## 7. Adjusting the colours and fonts

For editing the colours and fonts I recommend the following program:\
[Desktop Themes v1.87](https://archive.org/details/desktop_themes_v187_ZIP).

In order to launch the program, right click on `Desktop Themes.exe` and select "Run as administrator". This program will not start if you do not launch it with elevated permissions.

Go to "Edit Theme" section. Enter the username and serial.

Edit fonts and colours.



## 8. Adjust Taskbar features

Open "7+ Taskbar Tweaker" and see what you want to change. 

Under "Hovering" section I recommend changing the setting to "Tooltip" or "Nothing". The other options do not seem to work very well under classic theme.

All done. Congratulations! 

If this is the first time you installed classic theme, then you may want to reboot your computer now in order for the classic theme styles to be fully updated and applied to all programs.



# Troubleshooting

In case the window borders become too thick or other dimensions of window elements become different than you prefer, then look under the registry key\
`HKEY_CURRENT_USER\Control Panel\Desktop\WindowMetrics`

You may want to import again the registry file provided in chapter ["Needed registry changes"](#1-needed-registry-changes), point (3) "Import the following reg file:".

You may need to reboot the computer after changing these values in the registry.



# Optional advanced reading

## More info about setting up classic theme

If you want, you can investigate the following older webpages which provide somewhat alternate instructions for setting up classic theme. The current instructions were partially based on these sources:
* [Enable Classic Theme by handle method](https://windhawk.net/mods/classic-theme-enable)
* [Tutorial: Use of Windows Classic theme with Windows 10, Windows 11 and Explorer Patcher](https://github.com/valinet/ExplorerPatcher/discussions/167)



## How this mod works

The mod disables visual styles, effectively enabling the Windows Classic theme. This mod uses the method of closing access to the memory area where the theme is located. It is one of the multiple methods of enabling Windows classic theme.

The mod injects only into the process winlogon.exe.



## Detailed description of the compatibility updates

This mod has the following two capabilities built on top of previous classic theme mod [Enable Classic Theme by handle method by @Anixx](https://windhawk.net/mods/classic-theme-enable): Improved support for Remote Desktop sessions and code for handling early mod load, including during system start.

1) If Windhawk loads too early during system startup with the original mod, then the classic theme initialisation would fail. At the same time, starting Windhawk early (during system startup, not during user login) will improve the chances that the classic theme is applied as soon as possible and no programs need to be restarted later to get classic theme applied. In order for the classic theme enable to succeed in these conditions, the mod needs to check for conditions, and if needed, wait a bit in case the system is not yet ready to apply classic theme.
2) With the original mod the Remote Desktop sessions often disconnected during connecting. This happened even if the session was already logged in and had classic theme already applied, but was currently in disconnected state. Each new Remote Desktop connection gets its own winlogon.exe process. The mod needs to wait for the session "active" state in case it is modding Remote Desktop session related winlogon.exe processes.



## Acknowledgements

I would like to mention @Anixx who is the author of the previous Classic Theme Enable mod. The current mod is built upon that work.
*/
// ==/WindhawkModReadme==


#include <windowsx.h>
#include <windef.h>
#include <winternl.h>
#include <ntstatus.h>
#include <sddl.h>
#include <wtsapi32.h>

#include <mutex>
#include <wchar.h>


bool g_retryInitInAThread = false;
HANDLE g_initThread = NULL;
HANDLE g_initThreadStopSignal = NULL;
ULONG g_maximumResolution;
ULONG g_originalTimerResolution;
bool g_restoringOriginalResolution = false;
bool g_hookIsDisabledAndOriginalResolutionRestored = false;
std::mutex g_restoreTimerResolutionMutex;


typedef NTSTATUS(NTAPI* NtSetTimerResolution_t)(ULONG, BOOLEAN, PULONG);
NtSetTimerResolution_t pOriginalNtSetTimerResolution;

extern "C" NTSTATUS NTAPI NtOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
);


//this hook is needed to restore the resolution winlogon would have had if this mod would not have been installed
NTSTATUS WINAPI NtSetTimerResolutionHook(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution)
{
    if (
        !SetResolution
        || g_hookIsDisabledAndOriginalResolutionRestored    //when this stage is reached then g_restoreTimerResolutionMutex is not needed anymore
    ) {
        return pOriginalNtSetTimerResolution(DesiredResolution, SetResolution, CurrentResolution);
    }
    else if (g_restoringOriginalResolution) {     //the original resolution is being restored in a concurrent thread

        //If NtSetTimerResolution is called after g_restoringOriginalResolution is set and before the mutex lock is taken in RestoreTimerResolution(), then the hook code here ensures that g_originalTimerResolution will still contain the updated value, so the latest timer resolution value will be set just twice, but with a correct value.

        std::lock_guard<std::mutex> guard(g_restoreTimerResolutionMutex);

        NTSTATUS result = pOriginalNtSetTimerResolution(DesiredResolution, SetResolution, CurrentResolution);  
        if (NT_SUCCESS(result))
            g_originalTimerResolution = DesiredResolution;  //NB! still save the desired resolution to global variable in order to avoid race condition in RestoreTimerResolution()

        return result;
    }
    else {
        g_originalTimerResolution = DesiredResolution;  //unfortunately cannot check for success here, so lets just overwrite the previous desired resolution value
        return STATUS_SUCCESS;      //NB! lets keep the maximum timer resolution, do not actually change the timer resolution at the moment
    }
}

BOOL TryInit(bool* abort) {

    // Retrieve the current session ID for the process.
    DWORD sessionId;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId)) {     //GetCurrentProcessId() does not fail, no need to check for that
#ifdef _DEBUG   //this function will run in a fast loop, therefore not logging failures by default
        Wh_Log(L"ProcessIdToSessionId failed");
#endif
        return FALSE;     //retry
    }


    if (sessionId == 0) {   //Interactive services session - This is rarely used in Win10+ but I have seen claims that it still happens. Microsoft has fully disabled Interactive Service Detection starting with Windows 10 Build 1803 and also from in Windows Server 2019 and above.

        //if the session is service session then quit the init thread AND do not retry
        Wh_Log(L"Interactive services session detected, not changing the theme");
        *abort = true;
        return FALSE;
    }
    else if (sessionId != WTSGetActiveConsoleSessionId()) {     //this function does not fail

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
#ifdef _DEBUG   //this function will run in a fast loop, therefore not logging by default
            Wh_Log(L"Session connected: %ls", sessionConnected ? L"Yes" : L"No");
#endif
        }
        else {
#ifdef _DEBUG   //this function will run in a fast loop, therefore not logging failures by default
            Wh_Log(L"WTSQuerySessionInformationW failed");
#endif
        }

        if (pConnectState)
            WTSFreeMemory(pConnectState);

        if (!sessionConnected)  //Modify RDP sessions only when they reach active state else RDP connections will fail
            return FALSE;     //retry
    }


    wchar_t sectionName[sizeof("\\Sessions\\4294967295\\Windows\\ThemeSection")];
    if (-1 == swprintf_s(sectionName, ARRAYSIZE(sectionName), L"\\Sessions\\%lu\\Windows\\ThemeSection", sessionId)) {
#ifdef _DEBUG   //this function will run in a fast loop, therefore not logging failures by default
        Wh_Log(L"swprintf failed");
#endif
        return FALSE;     //retry
    }
    else {
#ifdef _DEBUG   //this function will run in a fast loop, therefore not logging default
        Wh_Log(L"Section name: %s", sectionName);
#endif
    }


    // Define the name of the section object.
    UNICODE_STRING sectionObjectName;
    RtlInitUnicodeString(&sectionObjectName, sectionName);

    // Define the attributes for the section object.
    OBJECT_ATTRIBUTES objectAttributes;
    InitializeObjectAttributes(&objectAttributes, &sectionObjectName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    HANDLE hSection;
    NTSTATUS status = NtOpenSection(&hSection, WRITE_DAC, &objectAttributes);
    if (!NT_SUCCESS(status)) {
#ifdef _DEBUG   //this function will run in a fast loop, therefore not logging failures by default
        Wh_Log(L"NtOpenSection failed with: 0x%X", status);
#endif
        return FALSE;     //retry
    }


    // Define your SDDL string.
    PCWSTR sddl = L"O:BAG:SYD:(A;;RC;;;IU)(A;;DCSWRPSDRCWDWO;;;SY)";
    PSECURITY_DESCRIPTOR psd = NULL;

    // Convert the SDDL string to a security descriptor.
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl, SDDL_REVISION_1, &psd, NULL)) {
#ifdef _DEBUG   //this function will run in a fast loop, therefore not logging failures by default
        Wh_Log(L"ConvertStringSecurityDescriptorToSecurityDescriptorW failed");
#endif
        CloseHandle(hSection);
        return FALSE;     //retry
    }

    // Set the security descriptor for the object.
    BOOL result = SetKernelObjectSecurity
    (
        hSection,
        DACL_SECURITY_INFORMATION,
        psd
    );
    if (!result) {
#ifdef _DEBUG   //this function will run in a fast loop, therefore not logging failures by default
        Wh_Log(L"SetKernelObjectSecurity failed");
#endif
        //will retry after cleaning up current attempt's resources
    }
    else {
        Wh_Log(L"SetKernelObjectSecurity successful, section name: %s", sectionName);
    }


    // Cleanup: free allocated security descriptor memory and close the handle.
    LocalFree(psd);
    CloseHandle(hSection);


    return result;     //retry if SetKernelObjectSecurity failed
}

void RestoreTimerResolution() {

    Wh_Log(L"About to restore timer resolution...");

    //If NtSetTimerResolution is called after g_restoringOriginalResolution is set and before the mutex lock is taken here, then the hook code in NtSetTimerResolutionHook() ensures that g_originalTimerResolution will still contain the updated value, so the latest timer resolution value will be set just twice, but with a correct value.

    g_restoringOriginalResolution = true;   //first set the flag, then take mutex

    {
        std::lock_guard<std::mutex> guard(g_restoreTimerResolutionMutex);

        ULONG CurrentResolution;
        if (NT_SUCCESS(pOriginalNtSetTimerResolution(g_originalTimerResolution, TRUE, &CurrentResolution))) {
            Wh_Log(L"NtSetTimerResolution success");
        }
        else {
            Wh_Log(L"NtSetTimerResolution failed");
        }

        g_hookIsDisabledAndOriginalResolutionRestored = true;   //in my experiments there seemed to be issues with Wh_RemoveFunctionHook() so not using it for time being
    }

    g_restoringOriginalResolution = false;  //Setting this variable is not essential, but if it is set, then in order to not miss any resolution updates, it should be set after g_hookIsDisabledAndOriginalResolutionRestored is set. Therefore I am setting this variable outside of mutex block, that will ensure the correct write ordering without having to manually specify memory fence instructions here.

    Wh_Log(L"Timer resolution restored");
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
    if (TryInit(&abort)) {
        RestoreTimerResolution();
        return TRUE;    //classic theme enable done
    }
    else if (abort) {
        RestoreTimerResolution();
        return FALSE;   //a service session
    }
    else {      //If Windhawk loads the mod too early then the classic theme initialisation will fail. Therefore we need to loop until the initialisation succeeds. Also if the mod is loaded into a RDP session too early then for some reason that would block the RDP session from successfully connecting. So we need to wait for session "active" state in case of RDP sessions. This is another reason for having a loop here.
        goto retry;
    }
}

BOOL Wh_ModInit() {

    Wh_Log(L"Init");


    bool abort = false;
    if (TryInit(&abort)) {
        return TRUE;    //classic theme enable done
    }
    else if (abort) {
        return FALSE;   //a service session
    }
    else {      //If Windhawk loads the mod too early then the classic theme initialisation will fail. Therefore we need to loop until the initialisation succeeds. Also if the mod is loaded into a RDP session too early then for some reason that would block the RDP session from successfully connecting. So we need to wait for session "active" state in case of RDP sessions. This is another reason for creating a separate thread with a loop.

        HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
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

        //Query original timer resolution since we will temporarily set the resolution to maximum until we poll in a separate thread. Maximum resolution is needed so that the mod can enable the classic theme at earliest suitable moment. Delay in enabling classic theme would result in some programs starting with no classic theme enabled.
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

        //set timer resolution hook so that if the program changes its desired resolution then we can 1) override that and 2) later restore the new desired resolution
        Wh_SetFunctionHook((void*)pNtSetTimerResolution, (void*)NtSetTimerResolutionHook, (void**)&pOriginalNtSetTimerResolution);  //NB! we do not check result of hooking, lets proceed with init thread creation in any case


        g_initThreadStopSignal = CreateEventW(
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

            //Ensure that the high timer resolution has actual effect by also setting maximum thread priority
            if (!SetThreadPriority(g_initThread, THREAD_PRIORITY_TIME_CRITICAL)) {
                Wh_Log(L"SetThreadPriority failed, proceeding regardless");
            }

            if (!SetThreadPriorityBoost(g_initThread, /*disablePriorityBoost*/FALSE)) {
                Wh_Log(L"SetThreadPriorityBoost failed, proceeding regardless");
            }

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

    if (g_retryInitInAThread) {   //Are we in the process of creating the init thread?

        //Temporarily set the resolution to maximum until we poll in a separate thread. Maximum resolution is needed so that the mod can enable the classic theme at earliest suitable moment.Delay in enabling classic theme would result in some programs starting with no classic theme enabled.

        //The mod needs to know the desired resolution set by the program. In order to avoid any race conditions, force timer resolution update only after the hook is activated. Else there might be a situation that the mod overrides the current resolution while hook is not yet set, and then the program changes it again before the hook is finally set, but the mod does not know that a new desired resolution was set by the program and therefore cannot restore it when init thread finishes.

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
            else    //normally the thread itself calls RestoreTimerResolution()
                RestoreTimerResolution();   
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
        //we could close the signal handle regardless whether the thread was successfully resumed since if the thread resume failed then the program will crash anyway IF the mod is unloaded AND the thread is manually resumed later by somebody. But just in case hoping that maybe the mod DLL will not be unloaded as long as it has threads, then lets keep the signal handle alive as well.

        CloseHandle(g_initThreadStopSignal);
        g_initThreadStopSignal = NULL;
    }

    Wh_Log(L"Uninit complete");
}
