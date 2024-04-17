// ==WindhawkMod==
// @id              remove-command-bar
// @name            Remove Command Bar
// @description     Removes the Command Bar from file explorer.
// @version         1.0
// @author          Waldemar
// @github          https://github.com/CyprinusCarpio
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Command Bar
This mod removes the Vista or 7 Command Bar from file explorer windows without having to patch shellstyle.dll using external tools.
This is meant to enable accurate looks on various pre-Vista file explorer mods.
If your shellstyle.dll is already patched, restore it using the following command in a elevated command prompt:

    sfc /scanfile=c:\windows\system32\shellstyle.dll

and reboot your system. If this is not done, the patch from the dll will take precedence over this mod.
After the mod is enabled, kill the explorer.exe process and restart it for the changes to take place.

The mod works by interjecting the request for a textual description of the file explorer window layout, and injecting a offset token into it.
The default offset is -40, but you may want to change this in the mod settings.

For issue reports, contact waldemar3194 on Discord, or file a report at my [github repository](https://github.com/CyprinusCarpio/windhawk-mods).

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Offset: -40
  $name: Command Bar offset
*/
// ==/WindhawkModSettings==

#include <string>
#include <windhawk_utils.h>

#ifdef _WIN64
#define CALCON __cdecl
#define SCALCON L"__cdecl"
#else
#define CALCON __stdcall
#define SCALCON L"__stdcall"
#endif

const std::wstring toLookFor = L"<style resid=\"FolderBandStyle\">";

int g_offsetSetting;

typedef HRESULT(* CALCON DUILoadUIFileFromResources_t)(HINSTANCE, unsigned int, LPWSTR*);
DUILoadUIFileFromResources_t DUILoadUIFileFromResourcesOriginal;
HRESULT CALCON DUILoadUIFileFromResourcesHook(HINSTANCE hInstance, unsigned int uResId, LPWSTR *pszOut)
{
    // We don't check the module name, only the resource id. It seems that the only resource with the id 1
    // that explorer loads is the one we want, and even if that's not true the wstring::find function prevents
    // us from modifying anything other than what we need to.
    HRESULT hRes = DUILoadUIFileFromResourcesOriginal(hInstance, uResId, pszOut);
    if (uResId == 1)
    {
        std::wstring uifile = *pszOut;
        size_t pos = uifile.find(toLookFor);

        if (pos == std::wstring::npos) 
        {
            return hRes;
        }
        std::wstring toAdd = L"\n<Element padding=\"rect(0rp,0rp,0rp," + std::to_wstring(g_offsetSetting) + L"rp)\"/>\n";
        
        uifile.insert(pos + toLookFor.length(), toAdd);
        *pszOut = (LPWSTR)LocalAlloc(LPTR, (uifile.length() + 1) * sizeof(WCHAR));
        if (*pszOut)
        {
            wcscpy(*pszOut, uifile.c_str());
        }
    }

    return hRes;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Remove Command Bar Init");

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] =
    {
        {   {
                L"long " SCALCON " DUI_LoadUIFileFromResources(struct HINSTANCE__ *,unsigned int,unsigned short * *)"
            },
            (void**)&DUILoadUIFileFromResourcesOriginal,
            (void*)DUILoadUIFileFromResourcesHook,
            FALSE
        }
    };

    if (!WindhawkUtils::HookSymbols(hShell32, hooks, 1))
    {
        Wh_Log(L"Failed to hook the member function.");
        return FALSE;
    }

    g_offsetSetting = Wh_GetIntSetting(L"Offset");

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Remove Command Bar Uninit");
}
