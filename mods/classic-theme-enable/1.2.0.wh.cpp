// ==WindhawkMod==
// @id              classic-theme-enable
// @name            Classic Theme
// @description     Disables theming (enables Classic theme)
// @version         1.2.0
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
On Windows 11 version 21H2 to 23H2 it is recommended to enable the Windows 10 taskbar with the
[*Windows 10 taskbar on Windows 11*](https://windhawk.net/mods/win10-taskbar-on-win11) mod while
on Windows 11 24H2 install the Windows 10 taskbar using the
[*Win10 taskbar on Win11 24H2*](https://windhawk.net/mods/win10-taskbar-on-win11-24h2) mod. For
the later mod, also the mod [*Fake Explorer path*](https://windhawk.net/mods/fake-explorer-path) is needed.

5. The Windows 10 taskbar still needs a compatibility fix, so install the mod
[*Classic Theme Explorer Lite*](https://windhawk.net/mods/classic-theme-explorer-lite).

6. If you are using the Windows 10 taskbar, install the [OpenShell utility](https://github.com/Open-Shell/Open-Shell-Menu).
After installing the OpenShell, put the patched file [StartMenuDLL](https://github.com/valinet/ExplorerPatcher/files/13359466/StartMenuDLL.zip)
into the Open Shell folder, replacing the existing one. 
Alternatively, install the already [patched version of OpenShell](http://redirect.viglink.com/?key=71fe2139a887ad501313cd8cce3053c5&subId=6513581&u=https%3A//github.com/OrthodoxWindows/Open-Shell-Menu/blob/master/OpenShellSetup_4_4_191-hlvioynt.exe)
    
    Alternatively to patching OpenShell you can install the mod
    [*Classic Taskbar background fix*](https://windhawk.net/mods/classic-taskbar-background-fix),
    but read carefully the instructions there. The mod way will still leave vasual glitches when the task buttons are dragged,
    so the patched Open Shell way is recommended.

    After installation, import the following REG file with OpenShell settings:

    ```
    Windows Registry Editor Version 5.00
    [HKEY_CURRENT_USER\Software\OpenShell\StartMenu\Settings]
    "StartButtonType"="ClasicButton"
    "CustomTaskbar"=dword:00000001
    "TaskbarLook"="Opaque"
    "TaskbarColor"=dword:00000000
    "SkinC1"="Classic skin"
    "SkinOptionsC1"=hex(7):43,00,41,00,50,00,54,00,49,00,4f,00,4e,00,3d,00,30,00,\
    00,00,55,00,53,00,45,00,52,00,5f,00,49,00,4d,00,41,00,47,00,45,00,3d,00,30,\
    00,00,00,55,00,53,00,45,00,52,00,5f,00,4e,00,41,00,4d,00,45,00,3d,00,30,00,\
    00,00,43,00,45,00,4e,00,54,00,45,00,52,00,5f,00,4e,00,41,00,4d,00,45,00,3d,\
    00,30,00,00,00,53,00,4d,00,41,00,4c,00,4c,00,5f,00,49,00,43,00,4f,00,4e,00,\
    53,00,3d,00,30,00,00,00,54,00,48,00,49,00,43,00,4b,00,5f,00,42,00,4f,00,52,\
    00,44,00,45,00,52,00,3d,00,31,00,00,00,53,00,4f,00,4c,00,49,00,44,00,5f,00,\
    53,00,45,00,4c,00,45,00,43,00,54,00,49,00,4f,00,4e,00,3d,00,31,00,00,00,00,\
    00

    [HKEY_CURRENT_USER\Software\OpenShell\ClassicExplorer\Settings]
    "StatusBarFont"=""
    "ShowStatusBar"=dword:00000001
    "UseBigButtons"=dword:00000000
    "TreeItemSpacing"=dword:fffffffe
    "NoFadeButtons"=dword:00000001
    "FullIndent"=dword:00000001

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
and enable all options in its settings.

11. To fix the empty context menus in Explorer, install the mods
[*Disable Immersive Context Menus*](https://windhawk.net/mods/disable-immersive-context-menus)
and
[*Non Immersive Taskbar Context Menu*](https://windhawk.net/mods/classic-taskbar-context-menu),
alternatively, if you are on Windows 11 23H2 or below, you can install the mod
[*Eradicate Immersive Menus*](https://windhawk.net/mods/eradicate-immersive-menus)
instead of the both. If confused, you can install all three, they do not conflict.

12. To restore the dragged items image during drag-and-drop, install this mod:
[*Classic Explorer Drag-n-Drop Lite*](https://windhawk.net/mods/classic-explorer-dragdrop-lite).

13. To restore the pre-Windows7 appearance of the File Explorer icons, install this mod:
[*Enable SyslistView32*](https://windhawk.net/mods/syslistview32-enabler).

14. To get back the full Explorer context menus on Windows 11, install this mod:
[*Classic context menu on Windows 11*](https://windhawk.net/mods/explorer-context-menu-classic).

15. To get rid of the immersive Windows 7 Command bar and the ribbon interface in File Explorer, install this mod:
[*Windows 7 Command Bar*](https://windhawk.net/mods/win7-command-bar).

16. If you did the previous point but dislike the Windows 7 Command bar as well, install additionally this mod:
[*Remove Command Bar*](https://windhawk.net/mods/remove-command-bar).

17. If you also dislike the Navigation bar, install this mod:
[*Disable Navigation Bar*](https://windhawk.net/mods/disable-navigation-bar).

18. To have the classic 3D borders in Notepad and other programs, install this mod:
[*Clientedge Everywhere*](https://windhawk.net/mods/clientedge-in-apps).

19. To fix the ribbon appearance an apps that use it, install this mod:
[*Basic/Classic Theme Ribbon Fix*](https://windhawk.net/mods/basic-classic-theme-ribbon-fix).

20. To have the Windows XP-like file picker dialog, install this mod:
[*Classic File Picker dialog*](https://windhawk.net/mods/classic-file-picker-dialog).

21. To have the hanged windows to have the classic window frame, install this mod:
[*DWM Ghost Mods*](https://windhawk.net/mods/dwm-ghost-mods), but read the instructions there.

22. If you are using the Firefox browser of version greater than 128, install this mod:
[*Firefox border fix for Classic theme*](https://windhawk.net/mods/firefox-border-fix).

23. To fix the borders in Edge, Chromium and restore the classic titlebar in other applications, install this mod:
[*Titlebar For Everyone*](https://windhawk.net/mods/titlebar-for-everyone).

24. If you prefer the 3D buttons on taskbar, like in Windows 7 and before, install this mod:
[*Classic Taskbar 3D buttons Lite*](https://windhawk.net/mods/classic-taskbar-buttons-lite).

25. To restore the classic tray clock flyout, use this mod:
[*Legacy (Win32) clock flyout (Win11-friendly)*](https://windhawk.net/mods/legacy-clock-flyout).

26. To have the classic sound volume flyout, install this mod:
[*Legacy (Win32) sound volume flyout*](https://windhawk.net/mods/legacy-sound-flyout).

27. To restore the classic window titlebars in UWP apps, install this mod:
[*Remove UWP titlebars*](https://winclassic.net/post/32692).

28. To restore the legacy Alt+Tab task selector on Windows 10, Windows 11 23H2 or earlier, use this mod:
[*Legacy Alt+Tab dialog*](https://windhawk.net/mods/legacy-alt-tab).

29. To have the classic console windows, install this mod:
[*Classic Conhost [Deprecated]*](https://windhawk.net/mods/classic-conhost). 
Despite the deprecation notice, the mod works just as intended.

30. To fix a glitch in the Explorer's toolbar gripper, install this mod:
[*Explorer Unlocked Toolbars Fix (WINAPI)*](https://windhawk.net/mods/explorer-no-toolbars-bottom-gripper).

31. To get the classic directory tree view in File Explorer, install this mod:
[*Classic Explorer Treeview*](https://windhawk.net/mods/classic-explorer-treeview).

32. You may want to install the mod
[*Disable Taskbar Thumbnails*](https://windhawk.net/mods/taskbar-thumbnails) because taskbar thumbnails
do not work in classic theme anyway, so to remove the useless popup.

33. If you are using a Windows 10 taskbar (either under Windows 10 or Windows 11), 
you may prefer the classic taskbar button context menus. For this, install the mod
[*Taskbar classic context menu*](https://windhawk.net/mods/taskbar-classic-menu) mod 
and enable the support for Windows 10 taskbar in the mod's settings.

34. If you want the classic, 32px icon size in folders, install this mod:
[*Explorer 32px Icons*](https://windhawk.net/mods/explorer-32px-icons).

35. If you want the color of the Control Panel pages' sidebar to match the color scheme,
install this mod: [*Control Panel Color Fix*](https://windhawk.net/mods/control-panel-color-fix).
On Windhawk 1.7 or newer, manually choose the older version 1.0 of the mod during or after installation
because the later version 1.0.1 broke the compatibility with Classic theme and will have no effect.

36. To make File Explorer to remember positions of the folders' windows the way it was before Vista,
install this mod: [*Remember the folder windows' positions*](https://windhawk.net/mods/remember-folder-positions).

37. To make the Control Panel to look like a regular folder the way it was before Windows 7, install this mod:
[*Control Panel Classic View Lite*](https://windhawk.net/mods/cpl-classic-view-lite).

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
    const DWORD RETRY_DELAY = 500;

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
