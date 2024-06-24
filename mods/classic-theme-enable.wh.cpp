// ==WindhawkMod==
// @id              classic-theme-enable
// @name            Enable Classic Theme by handle method
// @description     Disables theming by closing the handle
// @version         1.2.0
// @author          Anixx
// @github 			https://github.com/Anixx
// @include         winlogon.exe
// @compilerOptions -lntdll -lwtsapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Disables visual styles, effectively enabling the Windows Classic theme.
This mod uses the method of closing access to the memory area where
the theme is located. It is one of the multiple methods of enabling
Windows Classic theme.

The mod injects only in the process Winlogon.exe, and exits once
the handle of the memory area is closed. It does not hook
any functions. 

The mod affects only programs started after enabling the mod.

Windows Classic theme is not compatible with Windows 10 Task Manager.
Before enabling the mode, install the [classic task manager](https://winaero.com/get-classic-old-task-manager-in-windows-10/).

Enabling the mod will make UWP apps to hang. Install the *Classic UWP Fix* mod to mitigate this.

Some windows will manifest unwanted transparency. 
To fix this, install the *Classic theme transparency fix* mod.

Windows Classic theme is not compatible with Windows 11 and Windows 10 taskbars
out of the box. It is recommended to use [Explorer Patcher](https://github.com/valinet/ExplorerPatcher) 
utility, which includes the necessary fixes. A complete guide on setting up Windows Classic
theme is also [available](https://github.com/valinet/ExplorerPatcher/discussions/167).

Alternatively, if you are on Windows 10 or enabled the Windows 10 taskbar on Windows 11 via registry,
you can fix the taskbar compatibility using the *Classic Theme Explorer Lite* mod. 

You also would need to get rid of the immersive context menus in Explorer, which appear empty in Classic theme.
You can do it by installing the *Eradicate Immersive Menus* mod.

If you want the 3D borders in menus, import the following reg file:
```
Windows Registry Editor Version 5.00
[HKEY_CURRENT_USER\Control Panel\Desktop]
"UserPreferencesMask"=hex:9E,1E,05,80,12,01,00,00
```
Delete or rename this registry key:
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\DefaultColors
to prevent color scheme change after pressing Ctrl+Alt+Del or going to the logon screen and back.

To customize the color scheme, you can use the [Desktp Architect](https://www.themeworld.com/themes/utilities.html) 
utility, but make sure to install and run it in Windows 2000 or XP compatibility mode (in Windows XP mode
it will require UAC authorization).

You may consider installing other mods from Windhawk repository to make the look of the system
more classic, particularly, the *Enable SysListView32* mod and *Classic Taskbar 3D buttons Lite*.

![Windows Classic](https://i.imgur.com/gB7mwpp.png)

Authors: Anixx, levitation
*/
// ==/WindhawkModReadme==

#include <iostream>
#include <sddl.h>
#include <winnt.h>
#include <winternl.h>
#include <aclapi.h>
#include <securitybaseapi.h>
#include <wtsapi32.h>

// Define the prototype for the NtOpenSection function.
extern "C" NTSTATUS NTAPI NtOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
);


HANDLE g_initThread = NULL;
HANDLE g_initThreadStopSignal = NULL;


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

DWORD WINAPI InitThreadFunc(LPVOID param) {

    Wh_Log(L"InitThreadFunc enter");

    bool isRetry = false;
retry:  //If Windhawk loads the mod too early then the classic theme initialisation will fail. Therefore we need to loop until the initialisation succeeds. Also if the mod is loaded into a RDP session too early then for some reason that would block the RDP session from successfully connecting. So we need to wait for session "active" state in case of RDP sessions. This is another reason for having a loop here.
    if (isRetry) {
        if (WaitForSingleObject(g_initThreadStopSignal, 16) != WAIT_TIMEOUT) {
            Wh_Log(L"Shutting down InitThreadFunc before success");
            return FALSE;
        }
    }
    isRetry = true;

    bool abort = false;
    if (TryInit(&abort)) {
        return TRUE;  //hooks done
    }
    else if (abort) {
        return FALSE;   //a service session?
    }
    else {      //This winlogon.exe is associated with a non-console session that is not active yet. In this case we need to retry later, else the RDP connection will fail.
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
    else {      //This winlogon.exe is associated with a non-console session that is not active yet. In this case we need to retry later, else the RDP connection will fail.
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
            /*dwCreationFlags = */0, 	//The thread runs immediately after creation.
            /*lpThreadId = */NULL
        );

        if (g_initThread) {
            Wh_Log(L"InitThread started");
            return TRUE;
        }
        else {
            Wh_Log(L"CreateThread failed");
            CloseHandle(g_initThreadStopSignal);
            g_initThreadStopSignal = NULL;
            return FALSE;
        }
    }
}

void Wh_ModUninit() {

    Wh_Log(L"Uniniting...");

    if (g_initThread) {
        SetEvent(g_initThreadStopSignal);
        WaitForSingleObject(g_initThread, INFINITE);
        CloseHandle(g_initThread);
        g_initThread = NULL;
    }

    if (g_initThreadStopSignal) {
        CloseHandle(g_initThreadStopSignal);
        g_initThreadStopSignal = NULL;
    }

    Wh_Log(L"Uninit complete");
}
