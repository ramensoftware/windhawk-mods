// ==WindhawkMod==
// @id              uifile-override
// @name            UIFILE Override
// @description     Override UIFILE resources
// @version         1.0.2
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
#define THISCALL __cdecl
#define STHISCALL L"__cdecl"
#else
#define THISCALL __stdcall
#define STHISCALL L"__stdcall"
#endif

WCHAR g_pszCurrentThemePath[264];
HMODULE g_ShellStyleMod;

HMODULE getShellStyleHInstance() {
    WCHAR pszThemeFileName[264];
    WCHAR pszColorBuff[104];
    int pszSizeBuff[108];

    if (!GetCurrentThemeName(pszThemeFileName, 260, pszColorBuff, 100,
                             (LPWSTR)pszSizeBuff, MAX_PATH)) {
        PathRemoveFileSpecW(pszThemeFileName);
        PathAppendW(pszThemeFileName, L"Shell");
        PathAppendW(pszThemeFileName, pszColorBuff);
        PathAppendW(pszThemeFileName, L"shellstyle.dll");

        if (lstrcmpiW(g_pszCurrentThemePath, pszThemeFileName)) {
            lstrcpyW(g_pszCurrentThemePath, pszThemeFileName);
            FreeLibrary(g_ShellStyleMod);
            g_ShellStyleMod = LoadLibraryExW(pszThemeFileName, 0, 2);
            Wh_Log(L"shellstyle changed to %s = %p", pszThemeFileName,
                   g_ShellStyleMod);
        }
    }
    return g_ShellStyleMod;
}

HRESULT (*THISCALL AllocArray)(void*, char, int, LPWSTR*);

HRESULT(*THISCALL DUILoadUIFileFromResources_orig)
(HINSTANCE, unsigned int, LPWSTR*);
HRESULT THISCALL DUILoadUIFileFromResources_hook(HINSTANCE hInstance,
                                                 unsigned int uResId,
                                                 LPWSTR* pszOut) {
    HRSRC ResourceW;
    HGLOBAL Resource;
    const CHAR* resData;
    int cwchBuf;
    HRESULT result = 0;
    signed int LastError;
    HMODULE hShellStyleMod = getShellStyleHInstance();
    LPCWSTR lpType = L"UIFILE";

    if (HRSRC CustomResourceW =
            FindResourceW(hShellStyleMod, (LPCWSTR)uResId, L"SHELL32_UIFILE");
        CustomResourceW) {
        Wh_Log(L"found %d from SHELL32_UIFILE", uResId);
        ResourceW = CustomResourceW;
        hInstance = hShellStyleMod;
    } else {
        ResourceW = FindResourceW(hInstance, (LPCWSTR)uResId, lpType);
    }

    if (ResourceW && (Resource = LoadResource(hInstance, ResourceW)) != 0 &&
        (resData = (const CHAR*)LockResource(Resource)) != 0) {
        cwchBuf = SizeofResource(hInstance, ResourceW) + 1;
        result = AllocArray(NULL, 64, cwchBuf, pszOut);
        if (result >= 0)
            SHAnsiToUnicode(resData, *pszOut, cwchBuf);
    } else {
        LastError = GetLastError();
        result = LastError;
        if (LastError > 0)
            return (unsigned __int16)LastError | 0x80070000;
    }
    return result;
}

using SetXMLFromResource_t = HRESULT (*__thiscall)(void* pThis,
                                                   PCWSTR lpName,
                                                   PCWSTR lpType,
                                                   HMODULE hModule,
                                                   HINSTANCE param4,
                                                   HINSTANCE param5);
SetXMLFromResource_t SetXMLFromResource_orig;
HRESULT __thiscall SetXMLFromResource_hook(void* pThis,
                                           PCWSTR lpName,
                                           PCWSTR lpType,
                                           HMODULE hModule,
                                           HINSTANCE param4,
                                           HINSTANCE param5) {
    WCHAR Filename[MAX_PATH];
    WCHAR* FilenameW;
    HMODULE hShellStyleMod = getShellStyleHInstance();
    HRESULT result = 0;
    if (!lstrcmpW(lpType, L"UIFILE")) {
        GetModuleFileNameW(hModule, Filename, MAX_PATH);
        FilenameW = PathFindFileNameW(Filename);
        if (FilenameW) {
            PathRemoveExtensionW(FilenameW);
            CharUpperW(FilenameW);
            lstrcatW(FilenameW, L"_");
            StrNCatW(FilenameW, lpType, MAX_PATH);
            result = SetXMLFromResource_orig(pThis, lpName, FilenameW,
                                             hShellStyleMod, param4, param5);

            if (SUCCEEDED(result)) {
                Wh_Log(L"found %d from %s", lpName, FilenameW);
                // Fix for strings
                *((HINSTANCE*)pThis + 3) = hModule;
                return result;
            }
        }
    }
    return SetXMLFromResource_orig(pThis, lpName, lpType, hModule, param4,
                                   param5);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    HMODULE hMod = GetModuleHandleW(NULL);
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(hMod, exePath, MAX_PATH);

    HMODULE duiModule = LoadLibrary(L"dui70.dll");
    if (duiModule) {
        PCSTR procName =
            R"(?_SetXMLFromResource@DUIXmlParser@DirectUI@@IAEJPBG0PAUHINSTANCE__@@11@Z)";
        FARPROC pSetXMLFromResource = GetProcAddress(duiModule, procName);
        if (!pSetXMLFromResource) {
#ifdef _WIN64
            PCSTR procName_Win10_x64 =
                R"(?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z)";
            pSetXMLFromResource = GetProcAddress(duiModule, procName_Win10_x64);
#endif
        }

        if (pSetXMLFromResource) {
            Wh_SetFunctionHook((void*)pSetXMLFromResource,
                               (void*)SetXMLFromResource_hook,
                               (void**)&SetXMLFromResource_orig);
        } else {
            Wh_Log(L"Couldn't find SetXMLFromResource");
        }
    } else {
        Wh_Log(L"Failed to load dui70.dll");
    }

    if (lstrcmpiW(L"explorer.exe", PathFindFileNameW(exePath))) {
        return TRUE;
    }

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
        {{L"long " STHISCALL " DUI_LoadUIFileFromResources(struct HINSTANCE__ "
          "*,unsigned int,unsigned short * *)"},
         (void**)&DUILoadUIFileFromResources_orig,
         (void*)DUILoadUIFileFromResources_hook,
         FALSE},

        {{L"long " STHISCALL
          " _AllocArray<unsigned short,class CTGlobalAllocPolicy>(void "
          "*,unsigned long,unsigned __int64,unsigned short * *)"},
         (void**)&AllocArray,
         FALSE}

    };

    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, 2)) {
        Wh_Log(L"Failed to hook shell32.dll");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit(void) {
    if (g_ShellStyleMod) {
        FreeLibrary(g_ShellStyleMod);
    }
}
