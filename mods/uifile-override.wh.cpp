// ==WindhawkMod==
// @id              uifile-override
// @name            UIFILE Override
// @description     Override UIFILE resources
// @version         1.0.3
// @author          xalejandro
// @github          https://github.com/tetawaves
// @include         *
// @compilerOptions -luxtheme -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# UIFILE Override
Mod that overrides UIFILE resources, similar to OldNewExplorer, loading them
from current theme shellstyle.dll. Mainly used for explorer.
## How to use
For overriding a UIFILE resource with this mod, you need to add to your shellstyle.dll a resource named as the file you want to override:\
**shell32.dll UIFILE 21** -> **shellstyle.dll SHELL32_UIFILE 21**.\
This will override the original UIFILE resource from shell32 to the one in shellstyle.
### Resource UIFILE 21 loading from shellstyle.dll:
![Explorer loading resource shell32.dll UIFILE 21 from shellstye.dll](https://i.imgur.com/4sxHtUd.png)
*/
// ==/WindhawkModReadme==

#include <shlwapi.h>
#include <uxtheme.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#   define THISCALL  __cdecl
#   define STHISCALL L"__cdecl"
#else
#   define THISCALL  __thiscall
#   define STHISCALL L"__thiscall"
#endif

WCHAR g_szCurrentThemePath[MAX_PATH + 4];
HMODULE g_hShellStyle = nullptr;

HRESULT (THISCALL *_AllocArray)(void *, UINT, int, LPWSTR *);

HMODULE getShellStyleModule()
{
    WCHAR szThemeFileName[MAX_PATH + 4];
    WCHAR szColorBuff[MAX_PATH];
    WCHAR szSizeBuff[MAX_PATH];

    if (!GetCurrentThemeName(szThemeFileName, MAX_PATH, szColorBuff, MAX_PATH, szSizeBuff, MAX_PATH))
    {
        PathRemoveFileSpecW(szThemeFileName);
        PathAppendW(szThemeFileName, L"Shell");
        PathAppendW(szThemeFileName, szColorBuff);
        PathAppendW(szThemeFileName, L"shellstyle.dll");

        if (lstrcmpiW(g_szCurrentThemePath, szThemeFileName)) 
        {
            lstrcpyW(g_szCurrentThemePath, szThemeFileName);
            g_hShellStyle = LoadLibraryExW(szThemeFileName, nullptr, LOAD_LIBRARY_AS_DATAFILE);
            Wh_Log(L"shellstyle changed to %s = %p", szThemeFileName, g_hShellStyle);
        }
    }

    return g_hShellStyle;
}

HRESULT (THISCALL *DUILoadUIFileFromResources_orig)(HINSTANCE, UINT, LPWSTR *);
HRESULT THISCALL DUILoadUIFileFromResources_hook(HINSTANCE hInstance, UINT uResId, LPWSTR* ppszOut) 
{
    HRSRC hResInfo = nullptr;
    HGLOBAL hResData = nullptr;
    LPVOID lpData = nullptr;
    HRESULT hr = S_OK;

    HMODULE hShellStyle = getShellStyleModule();

    if (HRSRC hResInfoOverride = FindResourceW(hShellStyle, MAKEINTRESOURCEW(uResId), L"SHELL32_UIFILE"); hResInfoOverride)
    {
        Wh_Log(L"found %d from SHELL32_UIFILE", uResId);
        hResInfo = hResInfoOverride;
        hInstance = hShellStyle;        
    }
    else
    {
        hResInfo = FindResourceW(hInstance, MAKEINTRESOURCEW(uResId), L"UIFILE");
    }

    if (hResInfo && (hResData = LoadResource(hInstance, hResInfo)) && (lpData = LockResource(hResData))) 
    {
        int cwchBuf = SizeofResource(hInstance, hResInfo) + 1;
        hr = _AllocArray(nullptr, 64, cwchBuf, ppszOut);
        if (SUCCEEDED(hr))
        {
            SHAnsiToUnicode((const CHAR *)lpData, *ppszOut, cwchBuf);
        }
    } 
    else 
    {
        DWORD LastError = GetLastError();
        if (LastError)
        {
            return HRESULT_FROM_WIN32(LastError);
        }
    }

    return hr;
}

HRESULT (__thiscall *SetXMLFromResource_orig)(void* pThis, PCWSTR lpName, PCWSTR lpType, HMODULE hModule, HINSTANCE param4, HINSTANCE param5);
HRESULT __thiscall SetXMLFromResource_hook(void* pThis, PCWSTR lpName, PCWSTR lpType, HMODULE hModule, HINSTANCE param4, HINSTANCE param5) 
{
    WCHAR szModuleName[MAX_PATH];
    LPWSTR pszFilePath = nullptr;
    HMODULE hShellStyle = getShellStyleModule();

    if (!lstrcmpW(lpType, L"UIFILE"))
    {
        GetModuleFileNameW(hModule, szModuleName, MAX_PATH);
        pszFilePath = PathFindFileNameW(szModuleName);
        if (pszFilePath)
        {
            PathRemoveExtensionW(pszFilePath);
            CharUpperW(pszFilePath);
            lstrcatW(pszFilePath, L"_");
            StrNCatW(pszFilePath, lpType, MAX_PATH);
            if (HRESULT hr = SetXMLFromResource_orig(pThis, lpName, pszFilePath, hShellStyle, param4, param5); SUCCEEDED(hr))
            {
                Wh_Log(L"found %d from %s", lpName, pszFilePath);
                // Fix for strings
                *((HMODULE*)pThis + 3) = hModule;
                return hr;
            }          
        }
    }

    return SetXMLFromResource_orig(pThis, lpName, lpType, hModule, param4, param5);
}

BOOL Wh_ModInit() 
{
    Wh_Log(L"Init");

    HMODULE hModule = GetModuleHandleW(nullptr);
    wchar_t szModulePath[MAX_PATH];
    GetModuleFileNameW(hModule, szModulePath, MAX_PATH);

    HMODULE duiModule = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (duiModule) 
    {
        PCSTR procName =
            R"(?_SetXMLFromResource@DUIXmlParser@DirectUI@@IAEJPBG0PAUHINSTANCE__@@11@Z)";
        FARPROC pSetXMLFromResource = GetProcAddress(duiModule, procName);
        if (!pSetXMLFromResource) 
        {
            #ifdef _WIN64
            PCSTR procName_Win10_x64 =
                R"(?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z)";
            pSetXMLFromResource = GetProcAddress(duiModule, procName_Win10_x64);
            #endif
        }

        if (pSetXMLFromResource) 
        {
            Wh_SetFunctionHook((void*)pSetXMLFromResource,
                               (void*)SetXMLFromResource_hook,
                               (void**)&SetXMLFromResource_orig);
        } 
        else 
        {
            Wh_Log(L"Couldn't find SetXMLFromResource");
        }
    } 
    else 
    {
        Wh_Log(L"Failed to load dui70.dll");
    }

    if (lstrcmpiW(L"explorer.exe", PathFindFileNameW(szModulePath))) 
    {
        return TRUE;
    }

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32) 
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK shell32DllHooks[]
    {
        {
            {
                L"long " STHISCALL " DUI_LoadUIFileFromResources(struct HINSTANCE__ *,unsigned int,unsigned short * *)"
            },
            &DUILoadUIFileFromResources_orig,
            DUILoadUIFileFromResources_hook,
            false
        },
        {
            {
                L"long " STHISCALL " _AllocArray<unsigned short,class CTLocalAllocPolicy>(void *,unsigned long,unsigned __int64,unsigned short * *)"
            },
            &_AllocArray,
            nullptr,
            false
        }
    };

    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks,
                                    ARRAYSIZE(shell32DllHooks))) 
    {
        Wh_Log(L"Failed to hook shell32.dll");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit(void) 
{
    if (g_hShellStyle) 
    {
        FreeLibrary(g_hShellStyle);
    }
}
