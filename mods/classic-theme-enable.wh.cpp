// ==WindhawkMod==
// @id              classic-theme-enable
// @name            Classic Theme
// @description     Disables theming (enables Classic theme)
// @version         1.2.3
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

![Windows Classic](https://i.imgur.com/gB7mwpp.png)

The mod injects only in the process Winlogon.exe, and exits once
the handle of the memory area is closed. It does not hook
any functions. 

The mod affects only programs started after enabling the mod.

The following additional steps are strongly recommended:

1. Windows Classic theme is not compatible with Windows 10 Task Manager.
Before enabling the mode, install the [classic task manager](https://winaero.com/get-classic-old-task-manager-in-windows-10/).

2. Rename this registry key:
`HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\DefaultColors\Standard`
to prevent the color scheme change after pressing Ctrl+Alt+Del or going to the logon screen and back.

3. If you want the 3D borders in menus, import the following reg file:
    ```
    Windows Registry Editor Version 5.00
    [HKEY_CURRENT_USER\Control Panel\Desktop]
    "UserPreferencesMask"=hex:9E,1E,05,80,12,01,00,00
    ```

4. Windows Classic theme is not compatible with Windows 11 taskbar. 

*On Windows 11 version 21H2 to 23H2* it is recommended to enable the Windows 10 taskbar with the
[*Windows 10 taskbar on Windows 11*](https://windhawk.net/mods/win10-taskbar-on-win11) mod.

*On Windows 11 24H2, 25H2 or later* install the Windows 10 taskbar using the
[*Win10 taskbar on Win11 24H2 or 25H2*](https://windhawk.net/mods/win10-taskbar-on-win11-24h2) mod. For
the later mod, also the mod [*Fake Explorer path*](https://windhawk.net/mods/fake-explorer-path) is needed.
After this, the tray language indicator (if you use one) will appear vertical, so to fix it install this mod:
[*Fix language indicator in Win10 taskbar under Win11 24H2+*](https://windhawk.net/mods/fix-legacy-taskbar-tray-input-indicator).
The labels of the taskbar buttons won't be visible by default but they can be enabled via the Settings app.

5. The Windows 10 taskbar still needs a compatibility fix, so install the mod
[*Classic Theme Explorer Lite*](https://windhawk.net/mods/classic-theme-explorer-lite). 
On 24H2 and later you are advised to import a classic color scheme (see the bottom of this page)
to get rid of black backgrounds.

6. If you are using the Windows 10 taskbar, install the [OpenShell utility](https://github.com/Open-Shell/Open-Shell-Menu).
If you want the Windows logo on the Start button, import this registry file:

    ```
    Windows Registry Editor Version 5.00
    [HKEY_CURRENT_USER\Software\OpenShell\StartMenu\Settings]
    "StartButtonIcon"="%SystemRoot%\\System32\\slui.exe, 2"
    ```

7. Enabling the classic theme will make UWP apps to hang on splash screen. Install the [*Classic UWP Fix*](https://windhawk.net/mods/classic-uwp-fix) mod 
to mitigate this.

8. Some Control Panel applets (like Accessibility) will also fail to load, so to fix it, install this mod:
[*Classic Theme UIFILE Fix*](https://windhawk.net/mods/classic-theme-uifile-fix).

9. Some windows will manifest the unwanted transparency. 
To fix this, install the [*Classic theme transparency fix*](https://windhawk.net/mods/classic-theme-transparency-fix) mod.

10. On Windows 11 22H2 and later it is recommended to install the mod 
[*Explorerframe fixes for Win11 22H2+*](https://windhawk.net/mods/explorerframe-fixes-for-win11-22h2plus) 
and enable all options in its settings. The mod does not restore the menubar on Windows versions
24H2 and later, so you have to install in addition the following mod:
[*Restore folder menubar in 24h2, 25h2*](https://windhawk.net/mods/restore-folder-menubar-25h2).

11. To fix the empty context menus in Explorer, install the mods
[*Disable Immersive Context Menus*](https://windhawk.net/mods/disable-immersive-context-menus),
[*Non Immersive Taskbar Context Menu Lite*](https://windhawk.net/mods/classic-taskbar-context-menu-lite).

12. On Windows 11 24H2 and later the taskbar classic context menu have been removed, to restore it
install this mod: 
[*Windows 10 Taskbar Context Menu Fix for Win11 24H2+*](https://windhawk.net/mods/win10-taskbar-context-menu-fix-24h2)

13. To restore the dragged items image during drag-and-drop, install this mod:
[*Classic Explorer Drag-n-Drop Lite*](https://windhawk.net/mods/classic-explorer-dragdrop-lite).

14. To restore the pre-Windows7 appearance of the File Explorer icons, install this mod:
[*Enable SyslistView32*](https://windhawk.net/mods/syslistview32-enabler).

15. To get back the full Explorer context menus on Windows 11, install this mod:
[*Classic context menu on Windows 11*](https://windhawk.net/mods/explorer-context-menu-classic).

16. To get rid of the immersive Windows 7 Command bar and the ribbon interface in File Explorer, install this mod:
[*Windows 7 Command Bar*](https://windhawk.net/mods/win7-command-bar).

17. If you did the previous point but dislike the Windows 7 Command bar as well, install additionally this mod:
[*Remove Command Bar*](https://windhawk.net/mods/remove-command-bar).

18. If you also dislike the Navigation bar, install this mod:
[*Disable Navigation Bar*](https://windhawk.net/mods/disable-navigation-bar).

19. To have the classic 3D borders in Notepad and other programs, install this mod:
[*Clientedge Everywhere*](https://windhawk.net/mods/clientedge-in-apps).

20. To fix the ribbon appearance an apps that use it, install this mod:
[*Basic/Classic Theme Ribbon Fix*](https://windhawk.net/mods/basic-classic-theme-ribbon-fix).

21. To have the Windows XP-like file picker dialog, install this mod:
[*Classic File Picker dialog*](https://windhawk.net/mods/classic-file-picker-dialog).

22. To have the hanged windows to have the classic window frame, install this mod:
[*DWM Ghost Mods*](https://windhawk.net/mods/dwm-ghost-mods), but read the instructions there.

23. If you are using the Firefox browser of version greater than 128, install this mod:
[*Firefox border fix for Classic theme*](https://windhawk.net/mods/firefox-border-fix).

24. To fix the borders in Edge, Chromium and restore the classic titlebar in other applications, install this mod:
[*Titlebar For Everyone*](https://windhawk.net/mods/titlebar-for-everyone).

25. If you prefer the 3D buttons on taskbar, like in Windows 7 and before, install this mod:
[*Classic Taskbar 3D buttons Lite*](https://windhawk.net/mods/classic-taskbar-buttons-lite).

26. To restore the classic tray clock flyout, use this mod:
[*Legacy (Win32) clock flyout (Win11-friendly)*](https://windhawk.net/mods/legacy-clock-flyout).

27. To have the classic sound volume flyout, install this mod:
[*Legacy (Win32) sound volume flyout*](https://windhawk.net/mods/legacy-sound-flyout).

28. To restore the classic window titlebars in UWP apps, install this mod:
[*Remove UWP titlebars Lite*](https://windhawk.net/mods/native-titlebars-uwp-lite).

29. To restore the legacy Alt+Tab task selector on Windows 10, Windows 11 23H2 or earlier, use this mod:
[*Legacy Alt+Tab dialog*](https://windhawk.net/mods/legacy-alt-tab). On later Windows versions this
leads to disabling of any task selector dialog and switching directly between windows.

30. To have the classic console windows, install this mod:
[*Classic Conhost [Deprecated]*](https://windhawk.net/mods/classic-conhost). 
Despite the deprecation notice, the mod works just as intended.

31. To fix a glitch in the Explorer's toolbar gripper, install this mod:
[*Explorer Unlocked Toolbars Fix (WINAPI)*](https://windhawk.net/mods/explorer-no-toolbars-bottom-gripper).

32. To get the classic directory tree view in File Explorer, install this mod:
[*Classic Explorer Treeview*](https://windhawk.net/mods/classic-explorer-treeview).

33. You may want to install the mod
[*Disable Taskbar Thumbnails*](https://windhawk.net/mods/taskbar-thumbnails) because taskbar thumbnails
do not work in classic theme anyway, so to remove the useless popup.

34. If you are using a Windows 10 taskbar (either under Windows 10 or Windows 11), 
you may prefer the classic taskbar button context menus. For this, install the mod
[*Taskbar classic context menu*](https://windhawk.net/mods/taskbar-classic-menu) mod 
and enable the support for Windows 10 taskbar in the mod's settings.

35. If you want the classic, 32px icon size in folders, install this mod:
[*Explorer 32px Icons*](https://windhawk.net/mods/explorer-32px-icons).

36. If you want the color of the Control Panel pages' sidebar to match the color scheme,
install this mod: [*Control Panel Color Fix*](https://windhawk.net/mods/control-panel-color-fix).
On Windhawk 1.7 or newer, manually choose the older version 1.0 of the mod during or after installation
because the later version 1.0.1 broke the compatibility with Classic theme and will have no effect.

37. To make File Explorer to remember positions of the folders' windows the way it was before Vista,
install this mod: [*Remember the folder windows' positions*](https://windhawk.net/mods/remember-folder-positions).

38. To make the Control Panel to look like a regular folder the way it was before Windows 7, install this mod:
[*Control Panel Classic View Lite*](https://windhawk.net/mods/cpl-classic-view-lite).

39. To male the Windows Tools (Administrative Tools) folder to remember its view mode, install this mod:
[Fix Windows Tools Folder View](https://windhawk.net/mods/fix-windows-tools-view).

To customize the color scheme, you can use the [Desktp Architect](https://www.themeworld.com/themes/utilities.html) 
utility, but make sure to install and run it in Windows 2000 or XP compatibility mode (in Windows XP mode
it will require the UAC authorization). Alternatively you can use the 
[New Classic Theme Configurator](https://gitlab.com/ftortoriello/WinClassicThemeConfig).

*/
// ==/WindhawkModReadme==

#include <iostream>
#include <sddl.h>
#include <winnt.h>
#include <winternl.h>
#include <aclapi.h>
#include <securitybaseapi.h>

extern "C" NTSTATUS NTAPI NtOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
);

HANDLE g_hThread = NULL;
volatile BOOL g_bStopThread = FALSE;

BOOL TrySetThemeSectionSecurity() {
    DWORD sessionId;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);

    wchar_t sectionName[256];
    swprintf_s(sectionName, _countof(sectionName), L"\\Sessions\\%lu\\Windows\\ThemeSection", sessionId);

    UNICODE_STRING sectionObjectName;
    RtlInitUnicodeString(&sectionObjectName, sectionName);

    OBJECT_ATTRIBUTES objectAttributes;
    InitializeObjectAttributes(&objectAttributes, &sectionObjectName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    HANDLE hSection;
    NTSTATUS status = NtOpenSection(&hSection, WRITE_DAC, &objectAttributes);

    if (!NT_SUCCESS(status)) {
        Wh_Log(L"NtOpenSection failed: 0x%X", status);
        return FALSE;
    }

    LPCWSTR sddl = L"O:BAG:SYD:(A;;RC;;;IU)(A;;DCSWRPSDRCWDWO;;;SY)";
    PSECURITY_DESCRIPTOR psd = NULL;

    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl, SDDL_REVISION_1, &psd, NULL)) {
        CloseHandle(hSection);
        return FALSE;
    }

    BOOL result = SetKernelObjectSecurity(
        hSection,
        DACL_SECURITY_INFORMATION,
        psd
    );

    Wh_Log(L"SetKernelObjectSecurity result: %u", result);

    LocalFree(psd);
    CloseHandle(hSection);

    return result;
}

DWORD WINAPI RetryThreadProc(LPVOID lpParam) {
    const int MAX_ATTEMPTS = 10;
    const DWORD RETRY_DELAY = 100;

    for (int attempt = 1; attempt <= MAX_ATTEMPTS && !g_bStopThread; attempt++) {
        Wh_Log(L"Attempt %d/%d", attempt, MAX_ATTEMPTS);
        
        if (TrySetThemeSectionSecurity()) {
            Wh_Log(L"Success on attempt %d", attempt);
            return 0;
        }
        
        if (attempt < MAX_ATTEMPTS && !g_bStopThread) {
            Sleep(RETRY_DELAY);
        }
    }

    Wh_Log(L"All attempts failed");
    return 1;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    // Сначала пробуем сразу — может, секция уже доступна
    if (TrySetThemeSectionSecurity()) {
        Wh_Log(L"Success on first try");
        return TRUE;
    }

    // Если не получилось, запускаем фоновый поток для повторных попыток
    g_bStopThread = FALSE;
    g_hThread = CreateThread(NULL, 0, RetryThreadProc, NULL, 0, NULL);
    
    if (g_hThread == NULL) {
        Wh_Log(L"Failed to create retry thread");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    
    if (g_hThread) {
        g_bStopThread = TRUE;
        WaitForSingleObject(g_hThread, 2000);
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }
}
