// ==WindhawkMod==
// @id              explorer-vista-command-bar-native
// @name            Explorer Vista Command Bar Layout & Icons
// @description     Restores the Windows Vista layout of the command bar, forces native icons, and allows layout toggling. Forked from 'Aerexplorer' by aubymori and 'Windows 7 Command Bar' by ItsProfessional.
// @version         2.0.1.0
// @author          TheShadyRainbow4
// @github          https://github.com/TheShadyRainbow4
// @homepage        https://main.elitesoftwaretech.cc
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -luser32 -lole32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Vista Command Bar Layout & Icons

This mod utilizes native internal `shell32.dll` methods to safely restore the high-density visual style of the Windows Vista "FolderBand" command bar without corrupting standard system toolbars or menu bars.

### Acknowledgements & Forks
* **Aerexplorer (by aubymori):** The core logic for `shell32.dll` internal C++ symbol hooking to natively shift the "Views" alignment and force the Command Bar item icons to render is ported directly from Aerexplorer.
* **Windows 7 Command Bar (by ItsProfessional):** Includes the robust COM creation bypass necessary to suppress the Windows 10 Ribbon / Windows 11 Command Bar UI.

### Features
* **Native Icon Rendering:** Safely hooks `CSplitButton::UpdateIcon` to force standard icons next to labels.
* **Layout Adjustments:** Hooks `CCommandFolder` arrays to seamlessly shift elements like the Views dropdown from the far right to the left.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cmdbaricons: true
  $name: Command Bar Icons
  $description: "Shows icons on all command bar items natively, like Windows Vista."
- leftviews: true
  $name: Views Dropdown on Left
  $description: "Moves the views dropdown to the left after the 'Organize' button, mimicking the Vista layout."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <initguid.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

// --- COM CLSID Definitions for Ribbon/Modern UI Blocking (ItsProfessional Logic) ---
DEFINE_GUID(CLSID_UIRibbonFramework, 0x926749FA, 0x2615, 0x4987, 0x88, 0x45, 0xC3, 0x3E, 0x65, 0xF2, 0xB9, 0x57);
DEFINE_GUID(IID_UIRibbonFramework, 0xF4F0385D, 0x6872, 0x43A8, 0xAD, 0x09, 0x4C, 0x33, 0x9C, 0xB3, 0xF5, 0xC5);

// --- Architecture Specific Calling Conventions for Symbol Hooking ---
#ifdef _WIN64
#   define THISCALL  __cdecl
#   define STHISCALL L"__cdecl"
#   define STDCALL  __cdecl
#   define SSTDCALL L"__cdecl"
#else
#   define THISCALL  __thiscall
#   define STHISCALL L"__thiscall"
#   define STDCALL  __stdcall
#   define SSTDCALL L"__stdcall"
#endif

// --- Global Settings State ---
struct {
    bool cmdbaricons;
    bool leftviews;
} g_settings;

// --- Function Pointers ---
HRESULT (*ExplorerFrame_CoCreateInstanceOrig)(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv);

// --- Mod Setting Loader ---
void LoadModSettings() {
    g_settings.cmdbaricons = Wh_GetIntSetting(L"cmdbaricons");
    g_settings.leftviews = Wh_GetIntSetting(L"leftviews");
}

// ==============================================================================
// SECTION 1: COM Hook to Force Legacy Command Bar (ItsProfessional)
// ==============================================================================
HRESULT ExplorerFrame_CoCreateInstanceHook(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv){
    // Block Windows 11 Modern Command Bar
    if (*(INT64*)&rclsid.Data1 == 0x4D1E5A836480100B && *(INT64*)rclsid.Data4 == 0x339A8EA8E58A699F) {
        return REGDB_E_CLASSNOTREG;
    }
    // Block Windows 8/10 Ribbon
    if (IsEqualCLSID(rclsid, CLSID_UIRibbonFramework) && IsEqualIID(riid, IID_UIRibbonFramework)) {
        return REGDB_E_CLASSNOTREG;
    }
    return ExplorerFrame_CoCreateInstanceOrig(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}


// ==============================================================================
// SECTION 2: Layout & Alignment Modifiers (Aerexplorer Logic)
// ==============================================================================

// Re-routes Right Side commands (like Views) based on user settings
BOOL (__fastcall *IsRightSideCommand_orig)(REFGUID);
BOOL __fastcall IsRightSideCommand_hook(REFGUID rguidCmd) {
    return g_settings.leftviews ? FALSE : IsRightSideCommand_orig(rguidCmd);
}

HRESULT (THISCALL *CCommandFolder__AppendRightSideCommands_orig)(void *, IUnknown *);
HRESULT THISCALL CCommandFolder__AppendRightSideCommands_hook(void *pThis, IUnknown *punk) {
    // If leftviews is true, skip appending them to the right side natively
    return g_settings.leftviews ? S_OK : CCommandFolder__AppendRightSideCommands_orig(pThis, punk);
}

HRESULT (THISCALL *CCommandFolder__AppendOrganizeCommand_orig)(void *, IUnknown *);
HRESULT THISCALL CCommandFolder__AppendOrganizeCommand_hook(void *pThis, IUnknown *punk) {
    HRESULT hr = CCommandFolder__AppendOrganizeCommand_orig(pThis, punk);
    // Append the commands immediately after 'Organize' on the left
    if (g_settings.leftviews && SUCCEEDED(hr)) {
        hr = CCommandFolder__AppendRightSideCommands_orig(pThis, punk);
    }
    return hr;
}


// ==============================================================================
// SECTION 3: Command Bar Icon Enforcer (Aerexplorer Logic)
// ==============================================================================

// Memory offsets to locate the internal Icon Index for Explorer items
#ifdef _WIN64
#   define ExplorerCommandItem_iconIndex(pThis)  *((DWORD *)pThis + 10)
#else
#   define ExplorerCommandItem_iconIndex(pThis)  *((DWORD *)pThis + 8)
#endif

void (THISCALL *CSplitButton_UpdateIcon_orig)(void *, int);
void THISCALL CSplitButton_UpdateIcon_hook(void *pThis, int nIndex) {
    if (!g_settings.cmdbaricons) {
        CSplitButton_UpdateIcon_orig(pThis, nIndex);
    }
}

// Hook the display updater to intercept and inject the icon before rendering
void (THISCALL *CSplitButton__UpdateDisplay_orig)(void *pThis, const void *);
void THISCALL CSplitButton__UpdateDisplay_hook(void *pThis, const void *pItem) {
    if (g_settings.cmdbaricons) {
        int nIndex = ExplorerCommandItem_iconIndex(pItem);
        
        // Handle items with missing native icons (e.g. "Open" on executables)
        if (nIndex == -1) {
            using Shell_GetCachedImageIndexW_t = int (WINAPI *)(LPCWSTR, int, UINT);
            static Shell_GetCachedImageIndexW_t pShell_GetCachedImageIndexW = 
                (Shell_GetCachedImageIndexW_t)GetProcAddress(GetModuleHandleW(L"shell32.dll"), "Shell_GetCachedImageIndexW");
            
            if (pShell_GetCachedImageIndexW) {
                nIndex = pShell_GetCachedImageIndexW(L"shell32.dll", -1, 0);
            }
        }
        
        // Force the icon generation
        CSplitButton_UpdateIcon_orig(pThis, nIndex);
    }
    // Continue with the standard Win32 routine
    CSplitButton__UpdateDisplay_orig(pThis, pItem);
}


// ==============================================================================
// SECTION 4: Windhawk Initialization & Hook Registry
// ==============================================================================

// Shell32 internal C++ definitions mapping
const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        { L"int " SSTDCALL L" IsRightSideCommand(struct _GUID const &)" },
        &IsRightSideCommand_orig,
        IsRightSideCommand_hook,
        false
    },
    {
        { L"private: long " STHISCALL L" CCommandFolder::_AppendRightSideCommands(struct IUnknown *)" },
        &CCommandFolder__AppendRightSideCommands_orig,
        CCommandFolder__AppendRightSideCommands_hook,
        false
    },
    {
        { L"private: long " STHISCALL L" CCommandFolder::_AppendOrganizeCommand(struct IUnknown *)" },
        &CCommandFolder__AppendOrganizeCommand_orig,
        CCommandFolder__AppendOrganizeCommand_hook,
        false
    },
    {
        { L"private: void " STHISCALL L" CSplitButton::UpdateIcon(int)" },
        &CSplitButton_UpdateIcon_orig,
        CSplitButton_UpdateIcon_hook,
        false
    },
    {
        { L"private: void " STHISCALL L" CSplitButton::_UpdateDisplay(struct ExplorerCommandItem const *)" },
        &CSplitButton__UpdateDisplay_orig,
        CSplitButton__UpdateDisplay_hook,
        false
    }
};

BOOL Wh_ModInit(void) {
    Wh_Log(L"Initializing Native Vista Command Bar Mod");
    LoadModSettings();

    // 1. Hook COM creation to disable Ribbon/Modern UI (ItsProfessional)
    HMODULE hExplorerFrame = GetModuleHandleW(L"api-ms-win-core-com-l1-1-0.dll");
    if (!hExplorerFrame) {
        hExplorerFrame = GetModuleHandleW(L"combase.dll");
    }

    if (hExplorerFrame) {
        void* origFunc = (void*)GetProcAddress(hExplorerFrame, "CoCreateInstance");
        Wh_SetFunctionHook(origFunc, (void*)ExplorerFrame_CoCreateInstanceHook, (void**)&ExplorerFrame_CoCreateInstanceOrig);
    }

    // 2. Initialize Symbol Hooks for shell32.dll (Aerexplorer)
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (hShell32) {
        if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
            Wh_Log(L"Failed to hook one or more shell32 symbol functions.");
            return FALSE;
        }
    } else {
        Wh_Log(L"shell32.dll handle not found.");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloaded Native Vista Command Bar Mod");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed. Reloading parameters...");
    LoadModSettings();
    // Refresh active windows naturally
}