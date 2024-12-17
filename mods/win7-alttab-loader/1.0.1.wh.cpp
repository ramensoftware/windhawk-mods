// ==WindhawkMod==
// @id              win7-alttab-loader
// @name            Windows 7 Alt+Tab Loader
// @description     Loads Windows 7 Alt+Tab on Windows 10.
// @version         1.0.1
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -ldwmapi -luxtheme -ldbghelp
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Alt+Tab Loader
This mod allows the Windows 7 Alt+Tab UI to work on Windows 10.

# ⚠ IMPORTANT: PREREQUISITES! ⚠
- You will need a copy of `AltTab.dll` from Windows 7 (x64). Once you have this, drop it into `C:\Windows`.
  - **DO NOT DROP IT INTO `C:\Windows\System32`!!!!!!!! THIS WILL REPLACE THE WINDOWS 10 VERSION OF `AltTab.dll` AND CAN POTENTIALLY BREAK THINGS!!!!!!!!**
- You will also need an msstyles theme with a proper `AltTab` class, or else it will not render properly (Windows 3.x System font, weird looking selection, transparent background on basic theme)
  - [Here is a Windows 7 theme with proper classes.](https://windows7themenew2.carrd.co/)
  - You can make one with Windows Style Builder, which allows you to add classes to msstyles.
- Once you have all these and install this mod, you will need to restart `explorer.exe` for the Windows 7 Alt+Tab UI to load.
  - You will also need to restart `explorer.exe` to apply any new settings.

![DWM (with thumbnails)](https://raw.githubusercontent.com/aubymori/images/main/win7-alt-tab-dwm.png)
![Basic (no thumbnails)](https://raw.githubusercontent.com/aubymori/images/main/win7-alt-tab-basic.png)

*Co-authored by ephemeralViolette and aubymori.*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- basic: false
  $name: Use basic theme
  $description: Enable this if you use basic theme. This will disable thumbnails and render the AltTab background instead of DWMWindow.
*/
// ==/WindhawkModSettings==

// For some reason, this isn't guaranteed to be imported. At least the mingw64 version of
// dwmapi.h doesn't define this, but the value seems to be constant so idk I'll just define
// it myself:
#ifndef DWM_E_COMPOSITIONDISABLED
    #define DWM_E_COMPOSITIONDISABLED 0x80263001
#endif

#include <dbghelp.h>
inline BOOL VnPatchIAT(HMODULE hMod, PSTR libName, PSTR funcName, uintptr_t hookAddr)
{
    // Increment module reference count to prevent other threads from unloading it while we're working with it
    HMODULE module;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)hMod, &module)) return FALSE;

    // Get a reference to the import table to locate the kernel32 entry
    ULONG size;
    PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToDataEx(module, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size, NULL);

    // In the import table find the entry that corresponds to kernel32
    BOOL found = FALSE;
    while (importDescriptor->Characteristics && importDescriptor->Name) {
        PSTR importName = (PSTR)((PBYTE)module + importDescriptor->Name);
        if (_stricmp(importName, libName) == 0) {
#ifdef _LIBVALINET_DEBUG_HOOKING_IATPATCH
            printf("[PatchIAT] Found %s in IAT.\n", libName);
#endif
            found = TRUE;
            break;
        }
        importDescriptor++;
    }
    if (!found) {
        FreeLibrary(module);
        return FALSE;
    }

    // From the kernel32 import descriptor, go over its IAT thunks to
    // find the one used by the rest of the code to call GetProcAddress
    PIMAGE_THUNK_DATA oldthunk = (PIMAGE_THUNK_DATA)((PBYTE)module + importDescriptor->OriginalFirstThunk);
    PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)((PBYTE)module + importDescriptor->FirstThunk);
    while (thunk->u1.Function) {
        PROC* funcStorage = (PROC*)&thunk->u1.Function;

        BOOL bFound = FALSE;
        if (oldthunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
        {
            bFound = (!(*((WORD*)&(funcName)+1)) && IMAGE_ORDINAL32(oldthunk->u1.Ordinal) == (INT64)funcName);
        }
        else
        {
            PIMAGE_IMPORT_BY_NAME byName = (PIMAGE_IMPORT_BY_NAME)((uintptr_t)module + oldthunk->u1.AddressOfData);
            bFound = ((*((WORD*)&(funcName)+1)) && !_stricmp((char*)byName->Name, funcName));
        }

        // Found it, now let's patch it
        if (bFound) {
            // Get the memory page where the info is stored
            MEMORY_BASIC_INFORMATION mbi;
            VirtualQuery(funcStorage, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

            // Try to change the page to be writable if it's not already
            if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &mbi.Protect)) {
                FreeLibrary(module);
                return FALSE;
            }

            // Store our hook
            *funcStorage = (PROC)hookAddr;
#ifdef _LIBVALINET_DEBUG_HOOKING_IATPATCH
            if ((*((WORD*)&(funcName)+1)))
            {
                printf("[PatchIAT] Patched %s in %s to 0x%p.\n", funcName, libName, hookAddr);
            }
            else
            {
                printf("[PatchIAT] Patched 0x%x in %s to 0x%p.\n", funcName, libName, hookAddr);
            }
#endif

            // Restore the old flag on the page
            DWORD dwOldProtect;
            VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwOldProtect);

            // Profit
            FreeLibrary(module);
            return TRUE;
        }

        thunk++;
        oldthunk++;
    }

    FreeLibrary(module);
    return FALSE;
}

// https://stackoverflow.com/questions/50973053/how-to-hook-delay-imports
inline BOOL VnPatchDelayIAT(HMODULE hMod, PSTR libName, PSTR funcName, uintptr_t hookAddr)
{
    // Increment module reference count to prevent other threads from unloading it while we're working with it
    HMODULE lib;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)hMod, &lib)) return FALSE;

    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)lib;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((uintptr_t)lib + dos->e_lfanew);
    PIMAGE_DELAYLOAD_DESCRIPTOR dload = (PIMAGE_DELAYLOAD_DESCRIPTOR)((uintptr_t)lib +
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress);
    while (dload->DllNameRVA)
    {
        char* dll = (char*)((uintptr_t)lib + dload->DllNameRVA);
        if (!_stricmp(dll, libName)) {
#ifdef _LIBVALINET_DEBUG_HOOKING_IATPATCH
            printf("[PatchDelayIAT] Found %s in IAT.\n", libName);
#endif

            PIMAGE_THUNK_DATA firstthunk = (PIMAGE_THUNK_DATA)((uintptr_t)lib + dload->ImportNameTableRVA);
            PIMAGE_THUNK_DATA functhunk = (PIMAGE_THUNK_DATA)((uintptr_t)lib + dload->ImportAddressTableRVA);
            while (firstthunk->u1.AddressOfData)
            {
                if (firstthunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
                {
                    if (!(*((WORD*)&(funcName)+1)) && IMAGE_ORDINAL32(firstthunk->u1.Ordinal) == (INT64)funcName)
                    {
                        DWORD oldProtect;
                        if (VirtualProtect(&functhunk->u1.Function, sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect)) 
                        {
                            functhunk->u1.Function = (uintptr_t)hookAddr;
                            VirtualProtect(&functhunk->u1.Function, sizeof(uintptr_t), oldProtect, &oldProtect);
#ifdef _LIBVALINET_DEBUG_HOOKING_IATPATCH
                            printf("[PatchDelayIAT] Patched 0x%x in %s to 0x%p.\n", funcName, libName, hookAddr);
#endif
                            FreeLibrary(lib);
                            return TRUE;
                        }
                        FreeLibrary(lib);
                        return FALSE;
                    }
                }
                else
                {
                    PIMAGE_IMPORT_BY_NAME byName = (PIMAGE_IMPORT_BY_NAME)((uintptr_t)lib + firstthunk->u1.AddressOfData);
                    if ((*((WORD*)&(funcName)+1)) && !_stricmp((char*)byName->Name, funcName))
                    {
                        DWORD oldProtect;
                        if (VirtualProtect(&functhunk->u1.Function, sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect))
                        {
                            functhunk->u1.Function = (uintptr_t)hookAddr;
                            VirtualProtect(&functhunk->u1.Function, sizeof(uintptr_t), oldProtect, &oldProtect);
#ifdef _LIBVALINET_DEBUG_HOOKING_IATPATCH
                            printf("[PatchDelayIAT] Patched %s in %s to 0x%p.\n", funcName, libName, hookAddr);
#endif
                            FreeLibrary(lib);
                            return TRUE;
                        }
                        FreeLibrary(lib);
                        return FALSE;
                    }
                }
                functhunk++;
                firstthunk++;
            }
        }
        dload++;
    }
    FreeLibrary(lib);
    return FALSE;
}

#include <initializer_list>
namespace YukisCoffee::WindhawkUtils
{
    struct SymbolHooks
    {
        PCWSTR symbolName;
        void *hookFunction;
        void **pOriginalFunction;
    };

    bool hookWithSymbols(HMODULE module, std::initializer_list<SymbolHooks> hooks, PCWSTR server = NULL)
    {
        WH_FIND_SYMBOL symbol;
        HANDLE findSymbol;

        if (!module)
        {
            Wh_Log(L"Loaded invalid module");
            return false;
        }

        // These functions are terribly named, which is why I wrote this wrapper
        // in the first place.
        findSymbol = Wh_FindFirstSymbol(module, server, &symbol);

        if (findSymbol)
        {
            do
            {
                for (SymbolHooks hook : hooks)
                {
                    // If the symbol is unknown, then learn it.
                    if (!*hook.pOriginalFunction && 0 == wcscmp(symbol.symbol, hook.symbolName))
                    {
                        if (hook.hookFunction)
                        {
                            // This is unsafe if you make any typo at all.
                            Wh_SetFunctionHook(
                                symbol.address,
                                hook.hookFunction,
                                hook.pOriginalFunction
                            );

                            Wh_Log(
                                L"Installed hook for symbol %s at %p.", 
                                hook.symbolName,
                                symbol.address
                            );
                        }
                        else
                        {
                            *hook.pOriginalFunction = symbol.address;

                            Wh_Log(
                                L"Found symbol %s for %p.", 
                                hook.symbolName,
                                symbol.address
                            );
                        }
                    }
                }
            }
            while (Wh_FindNextSymbol(findSymbol, &symbol));

            Wh_FindCloseSymbol(findSymbol);
        }
        else
        {
            Wh_Log(L"Unable to find symbols for module.");
            return false;
        }

        // If a requested symbol is not found at all, then error as such.
        for (SymbolHooks hook : hooks)
        {
            if (!*hook.pOriginalFunction)
            {
                Wh_Log(
                    L"Original function for symbol hook %s not found.",
                    hook.symbolName
                );

                return false;
            }
        }

        return true;
    }
}

//=========================================================================================================================================
// ALT TAB:
//

#include <stdio.h>
#include <initguid.h>
#include <docobj.h>
#include <shlguid.h>
#include <uxtheme.h>
#include <dwmapi.h>

DEFINE_GUID(
    CLSID_AltTabSSO, 
    0xA1607060, 0x5D4C, 0x467A, 0xB7, 0x11, 0x2B, 0x59, 0xA6, 0xF2, 0x59, 0x57
);

struct {
    BOOL bBasic;
} settings;

HMODULE g_hAltTab = NULL;
IOleCommandTarget *g_pAltTabSSO = NULL;

void LoadSettings()
{
    settings.bBasic = Wh_GetIntSetting(L"basic");
}

// https://undoc.airesoft.co.uk/user32.dll/GetWindowCompositionAttribute.php
struct WINCOMPATTRDATA
{
    DWORD attribute; // the attribute to query, see below
    PVOID pData; // buffer to store the result
    ULONG dataSize; // size of the pData buffer
};

// https://undoc.airesoft.co.uk/user32.dll/GhostWindowFromHungWindow.php
typedef HWND (WINAPI *GhostWindowFromHungWindow_t)(HWND hWndGhost);
GhostWindowFromHungWindow_t pGhostWindowFromHungWindow;

// https://github.com/valinet/sws/blob/586fe7d6bdbab4eec0c4b0999efe7c8602636a0b/SimpleWindowSwitcher/sws_WindowHelpers.c#L539C57-L539C57
void GetDesktopText(wchar_t *wszTitle)
{
    HMODULE hExplorerFrame = GetModuleHandleW(L"ExplorerFrame.dll");

    if (hExplorerFrame)
    {
        LoadStringW(hExplorerFrame, 13140, wszTitle, MAX_PATH);
    }
    else
    {
        wcscat_s(wszTitle, MAX_PATH, L"Desktop");
    }
}

bool IsShellFrameWindow(HWND hWnd)
{
    // what in god's fucking holy name...
    BOOL (WINAPI *pIsShellFrameWindow)(HWND) = (BOOL (WINAPI *)(HWND))GetProcAddress(LoadLibrary(L"user32.dll"), (LPCSTR)2573);

    if (pIsShellFrameWindow)
    {
        return pIsShellFrameWindow(hWnd);
    }
    else
    {
        return FALSE;
    }
}

// even worse than above
bool IsShellManagedWindow(HWND hWnd)
{
    // what in god's fucking holy name...
    BOOL (WINAPI *pIsShellFrameWindow)(HWND) = (BOOL (WINAPI *)(HWND))GetProcAddress(LoadLibrary(L"user32.dll"), (LPCSTR)2574);

    if (pIsShellFrameWindow)
    {
        return pIsShellFrameWindow(hWnd);
    }
    else
    {
        return FALSE;
    }
}

int LoadStringW_Hook(HINSTANCE hInstance, UINT uId, LPWSTR lpBuffer, int cchBufferMax)
{
    if (uId == 0x3E8)
    {
        swprintf_s(lpBuffer, cchBufferMax, L"AltTab");
        return 6;
    }
    else if (uId == 0x3EA)
    {
        if (cchBufferMax < MAX_PATH)
            return 0;
        
        GetDesktopText(lpBuffer);

        int len = wcslen(lpBuffer);
        for (int i = 0; i < len; i++)
            if (lpBuffer[i] == L'&')
                lpBuffer[i] = L'\u200E';
        
        return len;
    }

    return LoadStringW(hInstance, uId, lpBuffer, cchBufferMax);
}

BOOL PostMessageW_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (hWnd == FindWindowW(L"Shell_TrayWnd", NULL) && uMsg == 0x5B7 && wParam == 0 && lParam == 0)
    {
        return PostMessageW(hWnd, WM_COMMAND, 407, 0);
    }

    return PostMessageW(hWnd, uMsg, wParam, lParam);
}

HTHEME OpenThemeData_Hook(HWND hWnd, LPCWSTR pszClassList)
{
    if (wcscmp(pszClassList, L"AltTab") == 0)
    {
        if (HTHEME data = OpenThemeData(hWnd, L"AltTab"))
        {
            return data;
        }

        return OpenThemeData(hWnd, L"WINDOW");
    }

    return OpenThemeData(hWnd, pszClassList);
}

BOOL IsWindowEnabled_Hook(HWND hWnd)
{
    if (!IsWindowEnabled(hWnd))
        return FALSE;

    BOOL isCloaked;
    DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &isCloaked, sizeof(BOOL));
    if (isCloaked)
        return FALSE;

    if (IsShellFrameWindow(hWnd) && !pGhostWindowFromHungWindow(hWnd))
        return TRUE;

    if (IsShellManagedWindow(hWnd) && GetPropW(hWnd, L"Microsoft.Windows.ShellManagedWindowAsNormalWindow") == NULL)
        return FALSE;

    return TRUE;
}

HRESULT DwmpActivateLivePreview_Hook(int s, HWND hWnd, int c, int d)
{
    return S_OK;
}

HRESULT DwmIsCompositionEnabled_Hook(BOOL *pfEnabled)
{
    HRESULT result = DwmIsCompositionEnabled(pfEnabled);

    if (S_OK == result)
    {
        *pfEnabled = FALSE;
    }

    return result;
}

HRESULT DwmExtendFrameIntoClientArea_Hook(HWND hWnd, const MARGINS *pMarInset)
{
    return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DwmEnableBlurBehindWindow_Hook(HWND hWnd, const DWM_BLURBEHIND *pBlurBehind)
{
    return DWM_E_COMPOSITIONDISABLED;
}

HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hWnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
    if (dwAttribute != DWMWA_EXTENDED_FRAME_BOUNDS)
    {
        if (
            (dwAttribute > 15) &&
            (dwAttribute != 65537)
            )
        {
            Wh_Log(L"SWA:    %i\r\n", dwAttribute);
        }

        return DwmSetWindowAttribute(hWnd, dwAttribute, pvAttribute, cbAttribute);
    }
    else
    {
        Wh_Log(L"SWA:    DWMWA_EXTENDED_FRAME_BOUNDS\r\n");
        return S_OK;
    }
}

BOOL IsCompositionActive_Hook()
{
    return FALSE;
}

void Cleanup()
{
    if (g_pAltTabSSO)
    {
        g_pAltTabSSO->Release();
    }
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    LoadSettings();

    g_hAltTab = LoadLibraryW(L"AltTab.dll");

    pGhostWindowFromHungWindow = (GhostWindowFromHungWindow_t)GetProcAddress(LoadLibrary(L"user32.dll"), "GhostWindowFromHungWindow");

    VnPatchIAT(
        g_hAltTab,
        (char *)"dwmapi.dll",
        (char *)113,
        (uintptr_t)DwmpActivateLivePreview_Hook
    );

    VnPatchIAT(
        g_hAltTab,
        (char *)"user32.dll",
        (char *)"PostMessageW",
        (uintptr_t)PostMessageW_Hook
    );

    VnPatchIAT(
        g_hAltTab,
        (char *)"user32.dll",
        (char *)"LoadStringW",
        (uintptr_t)LoadStringW_Hook
    );

    VnPatchIAT(
        g_hAltTab,
        (char *)"user32.dll",
        (char *)"IsWindowEnabled",
        (uintptr_t)IsWindowEnabled_Hook
    );

    VnPatchDelayIAT(
        g_hAltTab, 
        (char *)"uxtheme.dll", 
        (char *)"OpenThemeData", 
        (uintptr_t)OpenThemeData_Hook
    );

    if (settings.bBasic)
    {
        VnPatchDelayIAT(
            g_hAltTab, 
            (char *)"dwmapi.dll", 
            (char *)"DwmIsCompositionEnabled", 
            (uintptr_t)DwmIsCompositionEnabled_Hook
        );

        VnPatchDelayIAT(
            g_hAltTab, 
            (char *)"dwmapi.dll", 
            (char *)"DwmExtendFrameIntoClientArea", 
            (uintptr_t)DwmExtendFrameIntoClientArea_Hook
        );

        VnPatchDelayIAT(
            g_hAltTab, 
            (char *)"dwmapi.dll", 
            (char *)"DwmEnableBlurBehindWindow", 
            (uintptr_t)DwmEnableBlurBehindWindow_Hook
        );

        VnPatchDelayIAT(
            g_hAltTab, 
            (char *)"dwmapi.dll", 
            (char *)"DwmSetWindowAttribute", 
            (uintptr_t)DwmSetWindowAttribute_Hook
        );

        VnPatchDelayIAT(
            g_hAltTab, 
            (char *)"uxtheme.dll", 
            (char *)"IsCompositionActive", 
            (uintptr_t)IsCompositionActive_Hook
        );
    }

    if (g_hAltTab)
    {
        // I FUCKING HATE C++ I FUCKING HATE C++ I FUCKING HATE C++
        // THAT CAST WOULD NOT BE NECESSARY IN C
        HRESULT(*pDllGetClassObject)(REFCLSID, REFIID, LPVOID) = 
            (decltype(pDllGetClassObject))GetProcAddress(g_hAltTab, "DllGetClassObject");

        IClassFactory *pFactory = NULL;
        if (
            pDllGetClassObject &&
            SUCCEEDED(
                pDllGetClassObject(CLSID_AltTabSSO, IID_IClassFactory, &pFactory)
            ) &&
            pFactory
        )
        {
            if (SUCCEEDED(pFactory->CreateInstance(NULL, IID_IOleCommandTarget, (void **)&g_pAltTabSSO)) && g_pAltTabSSO)
            {
                if (SUCCEEDED(g_pAltTabSSO->Exec(&CGID_ShellServiceObject, 2, 0, NULL, NULL)))
                {
                    Wh_Log(L"Using Windows 7 AltTab!!");
                }
                else
                {
                    Wh_Log(L"Failed at SSO->Exec");
                }
            }
            else
            {
                Wh_Log(L"Failed at CreateInstance");
            }

            pFactory->Release();
        }
        else
        {
            Wh_Log(L"Failed at DllGetClassObject");
        }

        //FreeLibrary(g_hAltTab);
    }
    else
    {
        wchar_t emsg[1024];
        wsprintf(emsg, L"Failed to load AltTab.dll. %d", GetLastError());

        MessageBoxW(
            NULL,
            emsg,
            L"Windhawk : Alt-Tab Loader",
            MB_OK | MB_ICONERROR
        );

        return FALSE;
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
    Cleanup();
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged()
{
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
