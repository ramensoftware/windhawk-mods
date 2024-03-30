// ==WindhawkMod==
// @id              classic-theme-enable
// @name            Enable Classic Theme by handle method
// @description     Disables theming by closing the handle
// @version         1.0.3
// @author          Anixx
// @github 			https://github.com/Anixx
// @include         winlogon.exe
// @compilerOptions -lntdll
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

*/
// ==/WindhawkModReadme==

#include <iostream>
#include <sddl.h>
#include <winnt.h>
#include <winternl.h>
#include <aclapi.h>
#include <securitybaseapi.h>

// Define the prototype for the NtOpenSection function.
extern "C" NTSTATUS NTAPI NtOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
);

BOOL Wh_ModInit() {

    Wh_Log(L"Init");

    // Retrieve the current session ID for the process.
    DWORD sessionId;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);


    wchar_t sectionName[256];
    swprintf_s(sectionName, _countof(sectionName), L"\\Sessions\\%lu\\Windows\\ThemeSection", sessionId);

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

    // Define your SDDL string.
    LPCWSTR sddl = L"O:BAG:SYD:(A;;RC;;;IU)(A;;DCSWRPSDRCWDWO;;;SY)";
    PSECURITY_DESCRIPTOR psd = NULL;

    // Convert the SDDL string to a security descriptor.
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl, SDDL_REVISION_1, &psd, NULL)) {
        CloseHandle(hSection);
        return false;
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

    return result;
}
