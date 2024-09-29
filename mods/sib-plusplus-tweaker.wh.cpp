// ==WindhawkMod==
// @id              sib-plusplus-tweaker
// @name            StartIsBack++ Tweaker
// @description     Modify StartIsBack++'s features (2.9.20)
// @version         0.7.1
// @author          Erizur
// @github          https://github.com/Erizur
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lcomdlg32 -luser32 -lole32 -lgdi32 -lshell32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# StartIsBack++ Tweaker (2.9.20)
Modifies StartIsBack++'s internal functions for more customizable options.
Might not work if the StartIsBack++ DLL installed has been patched previously.
**Works only on StartIsBack++ 2.9.20. Functionality on older/newer versions is not guaranteed.**

## Features
**Remove Hottracking** - Removes the Item button glow present in StartIsBack++, which is used for 7/8 behavior.\
![Remove Hottracking](https://raw.githubusercontent.com/Erizur/imagehosting/main/nohottracking.png)

**Disable Start Menu Animations** - Disables the start menu animations, while leaving everything else untouched.
Similar to the behavior present in older versions of Windows.\
![Disable Start Menu Animations](https://raw.githubusercontent.com/Erizur/imagehosting/main/startanims.gif)

**Force DWM Composition** - Enable this option to force DWM blur instead of a custom one. 
Does NOT work with "Use custom start menu coloring" & "Use custom taskbar coloring" enabled.\
![Force DWM Composition](https://raw.githubusercontent.com/Erizur/imagehosting/main/forcedwm.png)

**Disable Custom Drawn Scrollbar** - In order to match custom themes, SIB uses a custom drawn scrollbar for the "All Programs" menu. 
Here you can force a native scrollbar instead.\
![Disable Custom Drawn Scrollbar](https://raw.githubusercontent.com/Erizur/imagehosting/main/nocdscroll.png)

**Disable Custom Orb** - This option disables StartIsBack's custom orb and tries to prevent Windows key hooking.\
![Disable Custom Orb](https://raw.githubusercontent.com/Erizur/imagehosting/main/nocustomorb.png)

**Fix "All Programs" Menu Padding** - This restores the smaller size used for the "All Programs" menu.\
![Menu Padding](https://raw.githubusercontent.com/Erizur/imagehosting/main/restoreappadding.png)

**Match Windows 7 Start Menu Links** - (This option requires the following links enabled to work properly: Computer/This PC, Connect To, Command Prompt) 
Tries to match as close as possible the Start Menu Links used in Windows 7's default Start Menu (Games, Help & Support).\
![Match Windows 7 Start Menu Links](https://raw.githubusercontent.com/Erizur/imagehosting/main/restoredlinks2.png)

**Fix User Folders On Corrupted Namespace** - Fixes the User Folders from opening up in a corrupted namespace if you used Aerexplorer or a registry hack to move them back from "This PC".\
![Fix User Folders On Corrupted Namespace](https://raw.githubusercontent.com/Erizur/imagehosting/main/fixfolders.png)

**Hide UWP Settings results** - Removes Windows 10's Immersive Settings from showing up on the search menu.\
![Hide UWP Settings results](https://raw.githubusercontent.com/Erizur/imagehosting/main/hideuwp.png)

## Special Thanks
- Wiktorwiktor12: FindPattern function & Custom Folder help.
- Aubymori: Additional help.
- TeknixStuff: "Hide UWP Settings results" setting contributor.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- SIBPath: "%PROGRAMFILES(X86)%\\StartIsBack\\StartIsBack64.dll"
  $name: StartIsBack64.dll Path Location
  $description: Place the path where your SIB++ dll (64-bit) is located. (Case Sensitive!)
- DHTracking: false
  $name: Remove Hottracking
  $description: Remove SIB's Hottracking (Windows 7 Style) method.
- DisableSMAnim: false
  $name: Disable Start Menu animations
  $description: Disable Start Menu's fade for accuracy to older Windows.
- ForceDWM: false
  $name: Force DWM Composition
  $description: Force Taskbar to use DWM to render the taskbar blur. Won't work with Custom Color enabled.
- DisableCDSB: false
  $name: Disable Custom Drawn Scrollbar
  $description: Enable this to draw the Start Menu scrollbar natively.
- DisableCustomOrb: false
  $name: Disable Custom Orb
  $description: Enable this to make SIB++ not hook into the Start Button.
- RestoreAPPadding: false
  $name: Fix "All Programs" Menu Padding
  $description: Windows 7 has smaller buttons for the "All Programs" menu. Use this to restore the old padding.
- MatchSevenFolders: false
  $name: Match Windows 7 Start Menu Links
  $description: Enable this to replace some of the start menu links to match Windows 7's Start Menu. (Swaps Computer's placement, Replace Connect To with Games, Replace Command Prompt with Help & Support)
- FixUserFolders: false
  $name: Fix User Folders On Corrupted Namespace
  $description: Based on a patch originally made by YukisCoffee. Fixes the User Folders from opening up in a corrupted namespace if you used Aerexplorer or a registry hack to move them back from "This PC".
- DisableImmersiveCPL: false
  $name: Hide UWP Settings results
  $description: Enable this to remove UWP settings from search results.
*/
// ==/WindhawkModSettings==

#include <corecrt.h>
#include <libloaderapi.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <windef.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <cstddef>
#include <cstring>
#include <dcomp.h>
#include <winerror.h>
#include <winnt.h>
#include <winuser.h>
#include <windows.h>
#include <psapi.h>
#include <combaseapi.h>
#include <processthreadsapi.h>
#include <psapi.h>
#include <shlobj.h>
#include <uxtheme.h>
#include <vector>

struct _settings {
    LPCWSTR SIBPath = L"%PROGRAMFILES(X86)%\\StartIsBack\\StartIsBack64.dll";
    BOOL DHTracking = FALSE;
    BOOL DisableSMAnim = FALSE;
    BOOL ForceDWM = FALSE;
    BOOL DisableCDSB = FALSE;
    BOOL DisableCustomOrb = FALSE;
    BOOL MatchSevenFolders = FALSE;
    BOOL RestoreAPPadding = FALSE;
    BOOL FixUserFolders = FALSE;
    BOOL DisableImmersiveCPL = FALSE;
} mod_settings;

enum SHELLMENUTYPE : __int32
{
  SHELLMENU_NONE = 0x0,
  SHELLMENU_STANDARD = 0x1,
  SHELLMENU_NOSUBFOLDERS = 0x2,
  SHELLMENU_RECENT = 0x3,
  SHELLMENU_FAVORITES = 0x4,
  SHELLMENU_ADMINTOOLS = 0x5,
};

struct __declspec(align(4)) FOLDERDEFINITION
{
  _GUID *folderID;
  const wchar_t *strParseName;
  const wchar_t *strSettingKey;
  int iShowDefault;
  const wchar_t *strPolicyOff;
  const wchar_t *strPolicyOn;
  const wchar_t *desktopIconCLSID;
  SHELLMENUTYPE iMenuType;
  unsigned int resNameId;
  unsigned int resTipId;
  wchar_t fontImage;
};

HANDLE g_restartExplorerPromptThread;
std::atomic<HWND> g_restartExplorerPromptWindow;

constexpr WCHAR kRestartExplorerPromptTitle[] =
    L"StartIsBack++ Tweaker - Windhawk";
constexpr WCHAR kRestartExplorerPromptText[] =
    L"Explorer needs to be restarted to apply the new changes. "
    L"Restart now?";

HMODULE g_hStartIsBackModule = nullptr;
ULONGLONG g_StartIsBackSize = 0;

FOLDERDEFINITION* SpecialFoldersList;
#define REL(addr, offset) ((addr + offset + 4) + *(int32_t*)(addr + offset))
#define OFF(addr, offset) addr = addr + offset;

static uintptr_t FindPattern(const char* signature, uintptr_t base_address, int Relative = 0, int Offset = 0)
{
    static auto patternToByte = [](const char* pattern)
    {
        auto bytes = std::vector<int>{};
        const auto start = const_cast<char*>(pattern);
        const auto end = const_cast<char*>(pattern) + strlen(pattern);
        for (auto current = start; current < end; ++current)
        {
            if (*current == '?')
            {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else { bytes.push_back(strtoul(current, &current, 16)); }
        }
        return bytes;
    };
    const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
    const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);
    const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto patternBytes = patternToByte(signature);
    const auto scanBytes = reinterpret_cast<std::uint8_t*>(base_address);
    const auto s = patternBytes.size();
    const auto d = patternBytes.data();
    for (auto i = 0ul; i < sizeOfImage - s; ++i)
    {
        bool found = true;
        for (auto j = 0ul; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            uintptr_t address = reinterpret_cast<uintptr_t>(&scanBytes[i]);
            if (Relative)
            {
                REL(address, Relative);
            }
            if (Offset)
            {
                OFF(address, Offset);
            }
            return address;
        }
    }
    return NULL;
}

void DoModulePatch()
{
    Wh_Log(L"Attempting to:");

    DWORD old;
    if(mod_settings.DHTracking == TRUE)
    {
        Wh_Log(L"- Disable Hottracking...");
        uint8_t* p_trackingBG = (uint8_t*)(FindPattern("48 8B C4 48 89 50 10 48 89 48 08",(uintptr_t)g_hStartIsBackModule));
        uint8_t* p_trackingBorder = (uint8_t*)(FindPattern("4C 8B DC 49 89 6B 18 49 89 73 20",(uintptr_t)g_hStartIsBackModule));

        if(p_trackingBG)
        {
            VirtualProtect(p_trackingBG, 1, PAGE_EXECUTE_READWRITE, &old);
            p_trackingBG[0] = 0xC3;
            VirtualProtect(p_trackingBG, 1, old, &old);
        }
        if(p_trackingBorder)
        {
            VirtualProtect(p_trackingBorder, 1, PAGE_EXECUTE_READWRITE, &old);
            p_trackingBorder[0] = 0xC3;
            VirtualProtect(p_trackingBorder, 1, old, &old);
        }
    }
    if(mod_settings.DisableCustomOrb == TRUE)
    {
        Wh_Log(L"- Disable Custom Orb...");
        uint8_t* p_createOrb = (uint8_t*)(FindPattern("40 56 48 83 EC 40 48 8D 15 03 70 01 00",(uintptr_t)g_hStartIsBackModule));
        uint8_t* p_winKeyHook = (uint8_t*)(FindPattern("40 53 48 81 EC A0 00 00 00",(uintptr_t)g_hStartIsBackModule));

        if(p_createOrb)
        {
            VirtualProtect(p_createOrb, 1, PAGE_EXECUTE_READWRITE, &old);
            p_createOrb[0] = 0xC3;
            VirtualProtect(p_createOrb, 1, old, &old);
        }
        if(p_winKeyHook)
        {
            VirtualProtect(p_winKeyHook, 1, PAGE_EXECUTE_READWRITE, &old);
            p_winKeyHook[0] = 0xC3;
            VirtualProtect(p_winKeyHook, 1, old, &old);
        }
    }
    if(mod_settings.DisableCDSB == TRUE)
    {
        Wh_Log(L"- Disable Custom Scrollbar...");
        uint8_t* p_cdScrollbar = (uint8_t*)(FindPattern("40 55 41 56 48 83 EC 28 4C 8B F1",(uintptr_t)g_hStartIsBackModule));

        if(p_cdScrollbar)
        {
            VirtualProtect(p_cdScrollbar, 2, PAGE_EXECUTE_READWRITE, &old);
            p_cdScrollbar[0] = 0xC3;
            p_cdScrollbar[1] = 0xC3;
            VirtualProtect(p_cdScrollbar, 2, old, &old);
        }
    }
    if(mod_settings.DisableSMAnim == TRUE)
    {
        Wh_Log(L"- Disable Start Menu Animations...");
        uint8_t* p_smAnimations = (uint8_t*)(FindPattern("41 8D 51 08 8D 4A 40 FF 15 CA 7B 05 00",(uintptr_t)g_hStartIsBackModule));

        if(p_smAnimations)
        {
            VirtualProtect(p_smAnimations, 7, PAGE_EXECUTE_READWRITE, &old);
            for(int i=0; i < 7; i++) p_smAnimations[i] = 0x90;
            VirtualProtect(p_smAnimations, 7, old, &old);
        }
    }
    if(mod_settings.ForceDWM == TRUE)
    {
        Wh_Log(L"- Force DWM Composition (Taskbar & Start Menu)...");
        uint8_t* p_dwmComposition = (uint8_t*)(FindPattern("48 8B CB E8 43 1D 03 00 84 C0 75 4E",(uintptr_t)g_hStartIsBackModule)) - 9;

        if(p_dwmComposition) {
            VirtualProtect(p_dwmComposition, 9, PAGE_EXECUTE_READWRITE, &old);
            for(int i=0; i < 9; i++) p_dwmComposition[i] = 0x90;
            VirtualProtect(p_dwmComposition, 9, old, &old);
        }
    }
    if(mod_settings.DisableImmersiveCPL == TRUE)
    {
        Wh_Log(L"- Remove UWP settings from search...");
        const wchar_t p_settingsR[] = L"shell:::{ED7BA470-8E54-465E-825C-99712043E01C}";

        uint8_t* p_settings1 = (uint8_t*)(FindPattern("73 00 68 00 65 00 6C 00 6C 00 3A 00 3A 00 3A 00 7B 00 39 00 39 00 45 00 32 00 42 00 33 00 36 00 32 00 2D 00 33 00 45 00 34 00 45 00 2D 00 34 00 32 00 35 00 35 00 2D 00 39 00 42 00 32 00 39 00 2D 00 34 00 31 00 41 00 37 00 46 00 34 00 30 00 37 00 37 00 37 00 42 00 41 00 7D 00 5C 00",(uintptr_t)g_hStartIsBackModule));
        uint8_t* p_settings2 = (uint8_t*)(FindPattern("73 00 68 00 65 00 6C 00 6C 00 3A 00 3A 00 3A 00 7B 00 39 00 39 00 45 00 32 00 42 00 33 00 36 00 32 00 2D 00 33 00 45 00 34 00 45 00 2D 00 34 00 32 00 35 00 35 00 2D 00 39 00 42 00 32 00 39 00 2D 00 34 00 31 00 41 00 37 00 46 00 34 00 30 00 37 00 37 00 37 00 42 00 42 00 7D 00 5C 00",(uintptr_t)g_hStartIsBackModule));
        uint8_t* p_settings3 = (uint8_t*)(FindPattern("73 00 68 00 65 00 6C 00 6C 00 3A 00 3A 00 3A 00 7B 00 39 00 39 00 45 00 32 00 42 00 33 00 36 00 32 00 2D 00 33 00 45 00 34 00 45 00 2D 00 34 00 32 00 35 00 35 00 2D 00 39 00 42 00 32 00 39 00 2D 00 34 00 31 00 41 00 37 00 46 00 34 00 30 00 37 00 37 00 37 00 42 00 42 00 7D 00 00 00",(uintptr_t)g_hStartIsBackModule));

        if(p_settings1)
        {
            VirtualProtect(p_settings1, 92, PAGE_EXECUTE_READWRITE, &old);
            memcpy(p_settings1, p_settingsR, 92);
            VirtualProtect(p_settings1, 92, old, &old);
        }

        if(p_settings2)
        {
            VirtualProtect(p_settings2, 92, PAGE_EXECUTE_READWRITE, &old);
            memcpy(p_settings2, p_settingsR, 92);
            VirtualProtect(p_settings2, 92, old, &old);
        }

        if(p_settings3)
        {
            VirtualProtect(p_settings3, 92, PAGE_EXECUTE_READWRITE, &old);
            memcpy(p_settings3, p_settingsR, 92);
            VirtualProtect(p_settings3, 92, old, &old);
        }
    }
    if(mod_settings.MatchSevenFolders == TRUE)
    {
        Wh_Log(L"- Match 7 Start Menu Links...");
        uintptr_t addr = (uintptr_t)(FindPattern("48 8D 35 ?? ?? ?? ?? 4C 8B F1 4C 8D 2D ?? ?? ?? ?? 33 FF",(uintptr_t)g_hStartIsBackModule)); // first check if pattern exists
        SpecialFoldersList = decltype(SpecialFoldersList)(REL(addr, 3) - 8); // match address and add - 8 as offset, then cast to struct.

        if(addr && SpecialFoldersList)
        {
            VirtualProtect(SpecialFoldersList, sizeof(FOLDERDEFINITION)*22, PAGE_EXECUTE_READWRITE, &old);

            FOLDERDEFINITION temp = SpecialFoldersList[10];
            SpecialFoldersList[10] = SpecialFoldersList[12];
            SpecialFoldersList[12] = temp;

            SpecialFoldersList[10].strParseName = L"shell:::{ED228FDF-9EA8-4870-83b1-96b02CFE0D52}"; // Games
            SpecialFoldersList[19].strParseName = L"shell:::{2559a1f1-21d7-11d4-bdaf-00c04f60b9f0}"; // Help & Support

            VirtualProtect(SpecialFoldersList, sizeof(FOLDERDEFINITION)*22, old, &old);
        }
    }

    return;
}

using SendMessageW_t = decltype(&SendMessageW);
SendMessageW_t SendMessageW_orig = nullptr;
LRESULT WINAPI SendMessageW_hook(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if(uMsg == TVM_SETITEMHEIGHT)
    {
        void *retaddr = __builtin_return_address(0);
        if (((ULONGLONG)retaddr >= (ULONGLONG)g_hStartIsBackModule) && ((ULONGLONG)retaddr < ((ULONGLONG)g_hStartIsBackModule + g_StartIsBackSize)))
        {
            return SendMessageW_orig(hWnd, uMsg, (int)wParam <= 26 ? 19 : wParam, lParam);
        }
        else return SendMessageW_orig(hWnd, uMsg, wParam, lParam);
    }
    else return SendMessageW_orig(hWnd, uMsg, wParam, lParam);
}

using ILCreateFromPathW_t = decltype(&ILCreateFromPathW);
ILCreateFromPathW_t ILCreateFromPathW_orig;
PIDLIST_ABSOLUTE ILCreateFromPathW_hook(PCWSTR pszPath)
{
    // Ensure that the caller is coming from SIB module:
    void *retaddr = __builtin_return_address(0);
    if ( ((ULONGLONG)retaddr >  (ULONGLONG)g_hStartIsBackModule) && ((ULONGLONG)retaddr < ( (ULONGLONG)g_hStartIsBackModule + g_StartIsBackSize )) )
    {
        WCHAR pszPathNew[1024];

        if (0 == wcscmp(pszPath, L"shell:::{A8CDFF1C-4878-43be-B5FD-F8091C1C60D0}"))
        {
            // Documents
            wcscpy(pszPathNew, L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}\\::{A8CDFF1C-4878-43be-B5FD-F8091C1C60D0}");
        }
        else if (0 == wcscmp(pszPath, L"shell:::{3ADD1653-EB32-4CB0-BBD7-DFA0ABB5ACCA}"))
        {
            // Pictures
            wcscpy(pszPathNew, L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}\\::{3ADD1653-EB32-4CB0-BBD7-DFA0ABB5ACCA}");
        }
        else if (0 == wcscmp(pszPath, L"shell:::{1CF1260C-4DD0-4EBB-811F-33C572699FDE}"))
        {
            // Music
            wcscpy(pszPathNew, L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}\\::{1CF1260C-4DD0-4EBB-811F-33C572699FDE}");
        }
        else if (0 == wcscmp(pszPath, L"shell:::{374DE290-123F-4565-9164-39C4925E467B}"))
        {
            // Downloads
            wcscpy(pszPathNew, L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}\\::{374DE290-123F-4565-9164-39C4925E467B}");
        }
        else
        {
            // Copy the original string.
            wcscpy(pszPathNew, pszPath);
        }

        PIDLIST_ABSOLUTE r = ILCreateFromPathW_orig(pszPathNew);
        return r;
    }

    // If we're not SIB, then just return.
    return ILCreateFromPathW_orig(pszPath);
}

using InflateRect_t = decltype(&InflateRect);
InflateRect_t InflateRect_orig = nullptr;
BOOL WINAPI InflateRect_hook(
    LPRECT lprc,
    int    dx,
    int    dy
)
{
    void *retaddr = __builtin_return_address(0);
    if (((ULONGLONG)retaddr >= (ULONGLONG)g_hStartIsBackModule) && ((ULONGLONG)retaddr < ((ULONGLONG)g_hStartIsBackModule + g_StartIsBackSize)))
    {
        //Wh_Log(L"retaddr = %p", retaddr);
        uint8_t* ptr = (uint8_t*)(FindPattern("33 D2 48 8B CE 44 8D 42 FF FF 15 72 98 03 00",(uintptr_t)g_hStartIsBackModule));

        if ((ULONGLONG)retaddr >= (ULONGLONG)(ptr) && (ULONGLONG)retaddr <= ((ULONGLONG)ptr + (ULONGLONG)16))
        {
            return TRUE;
        }
    }

    return InflateRect_orig(
        lprc,
        dx,
        dy
    );
}

// Taken from Taskbar height and icon size mod by Ramen Software (m417z)
// https://github.com/ramensoftware/windhawk-mods/blob/7e0bd27ae1d12ae639497fbc9b48bb791f98b078/mods/taskbar-icon-size.wh.cpp#L723
void PromptForExplorerRestart() {
    if (g_restartExplorerPromptThread) {
        if (WaitForSingleObject(g_restartExplorerPromptThread, 0) !=
            WAIT_OBJECT_0) {
            return;
        }

        CloseHandle(g_restartExplorerPromptThread);
    }

    g_restartExplorerPromptThread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParameter) WINAPI -> DWORD {
            TASKDIALOGCONFIG taskDialogConfig{
                .cbSize = sizeof(taskDialogConfig),
                .dwFlags = TDF_ALLOW_DIALOG_CANCELLATION,
                .dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
                .pszWindowTitle = kRestartExplorerPromptTitle,
                .pszMainIcon = TD_INFORMATION_ICON,
                .pszContent = kRestartExplorerPromptText,
                .pfCallback = [](HWND hwnd, UINT msg, WPARAM wParam,
                                 LPARAM lParam, LONG_PTR lpRefData)
                                  WINAPI -> HRESULT {
                    switch (msg) {
                        case TDN_CREATED:
                            g_restartExplorerPromptWindow = hwnd;
                            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                                         SWP_NOMOVE | SWP_NOSIZE);
                            break;

                        case TDN_DESTROYED:
                            g_restartExplorerPromptWindow = nullptr;
                            break;
                    }

                    return S_OK;
                },
            };

            int button;
            if (SUCCEEDED(TaskDialogIndirect(&taskDialogConfig, &button,
                                             nullptr, nullptr)) &&
                button == IDYES) {
                WCHAR commandLine[] =
                    L"cmd.exe /c "
                    L"\"taskkill /F /IM explorer.exe & start explorer\"";
                STARTUPINFO si = {
                    .cb = sizeof(si),
                };
                PROCESS_INFORMATION pi{};
                if (CreateProcess(nullptr, commandLine, nullptr, nullptr, FALSE,
                                  0, nullptr, nullptr, &si, &pi)) {
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
                }
            }

            return 0;
        },
        nullptr, 0, nullptr);
}

void LoadSettings(void)
{
    mod_settings.SIBPath = Wh_GetStringSetting(L"SIBPath");
    mod_settings.DHTracking = Wh_GetIntSetting(L"DHTracking");
    mod_settings.DisableCDSB = Wh_GetIntSetting(L"DisableCDSB");
    mod_settings.DisableSMAnim = Wh_GetIntSetting(L"DisableSMAnim");
    mod_settings.DisableCustomOrb = Wh_GetIntSetting(L"DisableCustomOrb");
    mod_settings.ForceDWM = Wh_GetIntSetting(L"ForceDWM");
    mod_settings.MatchSevenFolders = Wh_GetIntSetting(L"MatchSevenFolders");
    mod_settings.RestoreAPPadding = Wh_GetIntSetting(L"RestoreAPPadding");
    mod_settings.FixUserFolders = Wh_GetIntSetting(L"FixUserFolders");
    mod_settings.DisableImmersiveCPL = Wh_GetIntSetting(L"DisableImmersiveCPL");
}

BOOL CheckForStartIsBack()
{
    const wchar_t *SIB_PATH = mod_settings.SIBPath;
    wchar_t workingSibPath[1024] = { 0 };
    ExpandEnvironmentStringsW(SIB_PATH, workingSibPath, 1024);

    g_hStartIsBackModule = LoadLibraryW(workingSibPath);
    MODULEINFO miSib = { 0 };
    if (!GetModuleInformation(
        GetCurrentProcess(),
        g_hStartIsBackModule,
        &miSib,
        sizeof(MODULEINFO)
    ))
    {
        Wh_Log(L"Failed to get size of StartIsBack module! Exiting.");
        return FALSE;
    }
    g_StartIsBackSize = miSib.SizeOfImage;

    if (g_hStartIsBackModule) DoModulePatch();
    else {
        Wh_Log(L"StartIsBack DLL could NOT be found. Please make sure you properly placed the path.\nOr you're using Aerexplorer and this is getting called from the other explorer window. (heh...)");
        return FALSE;
    }

    return TRUE;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Initalize SIB++ Tweaker.");

    LoadSettings();
    if(!CheckForStartIsBack()) return FALSE;

    if(mod_settings.RestoreAPPadding == TRUE)
    {
        Wh_Log(L"- Fix 'All Programs' Menu Padding...");
        if (!Wh_SetFunctionHook(
            (void *)SendMessageW,
            (void *)SendMessageW_hook,
            (void **)&SendMessageW_orig
        ))
        {
            Wh_Log(L"Failed to hook SendMessageW");
            return FALSE;
        }
        
        if (!Wh_SetFunctionHook(
            (void *)InflateRect,
            (void *)InflateRect_hook,
            (void **)&InflateRect_orig
        ))
        {
            Wh_Log(L"Failed to hook InflateRect");
            return FALSE;
        }
    }

    if(mod_settings.FixUserFolders == TRUE)
    {
        Wh_Log(L"- Fix User Folders...");
        HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
        FARPROC pfnILCreateFromPathW = GetProcAddress(hShell32, "ILCreateFromPathW");

        Wh_SetFunctionHook(
            (void *)pfnILCreateFromPathW,
            (void *)ILCreateFromPathW_hook,
            (void **)&ILCreateFromPathW_orig
        );
    }

    Wh_Log(L"All done!");
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Exiting SIB++ Tweaker.");

    HWND restartExplorerPromptWindow = g_restartExplorerPromptWindow;
    if (restartExplorerPromptWindow) {
        PostMessage(restartExplorerPromptWindow, WM_CLOSE, 0, 0);
    }

    if (g_restartExplorerPromptThread) {
        WaitForSingleObject(g_restartExplorerPromptThread, INFINITE);
        CloseHandle(g_restartExplorerPromptThread);
        g_restartExplorerPromptThread = nullptr;
    }
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SIB++ Tweaker settings changed. Attempting to restart explorer.");

    PromptForExplorerRestart();
}