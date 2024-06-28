// ==WindhawkMod==
// @id              sib-plusplus-tweaker
// @name            StartIsBack++ Tweaker
// @description     Modify StartIsBack++'s features (2.9.20)
// @version         0.4.1
// @author          Erizur
// @github          https://github.com/Erizur
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomdlg32 -luser32 -lole32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# StartIsBack++ Tweaker (2.9.20)
Modifies StartIsBack++'s internal functions for more customizable options.
Might not work if the StartIsBack++ DLL installed has been patched previously.
**Works only on StartIsBack++ 2.9.20. Functionality on older/newer versions is not guaranteed.**

## Features
As of now, the mod can do interesting things as restoring Windows 7's links, disable Hottracking or disable Start Menu animations.
Read the options or the [WinClassic post](https://winclassic.net/thread/2377/startisback-tweaker-2-9-20) for more info.

## Special Thanks
- Wiktorwiktor12: FindPattern function & Custom Folder help.
- Aubymori: Additional help.
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
- DisableCustomOrb: FALSE
  $name: Disable Custom Orb
  $description: Enable this to make SIB++ not hook into the Start Button.
- RestoreAPPadding: FALSE
  $name: Fix "All Programs" Menu Padding
  $description: Windows 7 has smaller buttons for the "All Programs" menu. Use this to restore the old padding.
- MatchSevenFolders: FALSE
  $name: Match Windows 7 Start Menu Links
  $description: Enable this to replace some of the start menu links to match Windows 7's Start Menu. (Swaps Computer's placement, Replace Connect To with Games, Replace Command Prompt with Help & Support)
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

struct _settings {
    LPCWSTR SIBPath = L"%PROGRAMFILES(X86)%\\StartIsBack\\StartIsBack64.dll";
    BOOL DHTracking = FALSE;
    BOOL DisableSMAnim = FALSE;
    BOOL ForceDWM = FALSE;
    BOOL DisableCDSB = FALSE;
    BOOL DisableCustomOrb = FALSE;
    BOOL MatchSevenFolders = FALSE;
    BOOL RestoreAPPadding = FALSE;
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

            SpecialFoldersList[10].strParseName = L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\0\\::{ED228FDF-9EA8-4870-83b1-96b02CFE0D52}"; // Games
            SpecialFoldersList[19].strParseName = L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\0\\::{2559a1f1-21d7-11d4-bdaf-00c04f60b9f0}"; // Help & Support

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

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_orig = nullptr;
BOOL WINAPI SetWindowPos_hook(
    HWND hWnd,
    HWND hWndInsertAfter,
    int  X,
    int  Y,
    int  cx,
    int  cy,
    UINT uFlags
)
{
    void *retaddr = __builtin_return_address(0);
    if (((ULONGLONG)retaddr >= (ULONGLONG)g_hStartIsBackModule) && ((ULONGLONG)retaddr < ((ULONGLONG)g_hStartIsBackModule + g_StartIsBackSize)))
    {
        WCHAR className[MAX_PATH];
        GetClassName(hWnd, className, MAX_PATH);
        if(0 == wcscmp(className, L"NamespaceTreeControl"))
        {
            HDC dc = GetDC(hWnd);
            return SetWindowPos_orig(
                hWnd,
                hWndInsertAfter,
                X,
                Y,
                cx,
                cy + MulDiv(13, GetDeviceCaps(dc, LOGPIXELSY), 96), //losing values...
                uFlags
            );
        }
    }
    return SetWindowPos_orig(
        hWnd,
        hWndInsertAfter,
        X,
        Y,
        cx,
        cy,
        uFlags
    );
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
    #ifdef _WIN64
        const size_t OFFSET_SAME_TEB_FLAGS = 0x17EE;
    #else
        const size_t OFFSET_SAME_TEB_FLAGS = 0x0FCA;
    #endif
        bool isInitialThread = *(USHORT*)((BYTE*)NtCurrentTeb() + OFFSET_SAME_TEB_FLAGS) & 0x0400;
        if (!isInitialThread) {
            system("taskkill /F /IM explorer.exe & start explorer");
            return FALSE;
        }

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
            (void *)SetWindowPos,
            (void *)SetWindowPos_hook,
            (void **)&SetWindowPos_orig
        ))
        {
            Wh_Log(L"Failed to hook SetWindowPos");
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

    Wh_Log(L"All done!");
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Exiting SIB++ Tweaker.");

    system("taskkill /F /IM explorer.exe & start explorer");
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SIB++ Tweaker settings changed. Attempting to restart explorer.");

    system("taskkill /F /IM explorer.exe & start explorer");
}