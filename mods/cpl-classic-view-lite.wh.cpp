// ==WindhawkMod==
// @id              cpl-classic-view-lite
// @name            Control Panel Classic View Lite
// @description     Makes the Control Panel folder to appear as any other
// @version         1.0.1
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Control Panel Classic View Lite

This mod makes the Control Panel to look like a regular folder, the way it was before Windows 7.

The mod is based on the finding by user Tetawaves and realized by them in a more extensive mod [Control Panel Classic View](https://winclassic.net/thread/3416/classic-control-panel-view).

This mod differs in that

* It hooks only one function instead of 3.
* It does not provide the mechanism to use a custom UIFILE for Control Panel layout.
* It affects both the `all items` view and the `category view`.

It has also an option to make the individual category folders to look like regular folders (this option is disabled by default).

![All items view](https://i.imgur.com/etljMic.png)

There is an empty, non-functional icon in the Control panel folder, so to remove it, execute this command with elevated rights:

```
reg delete "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\ControlPanel\NameSpace\{98F2AB62-0E29-4E4C-8EE7-B542E66740B1}" /f
```
To create a shortcut to the `all Items` view, create a folder with this name extension: 
`{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}`, 

or, to open it once, run a command `explorer shell:::{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}`.

To create a shortcut to the `category view`, create a folder with this name extension:
`{26EE0668-A00A-44D7-9371-BEB064C98683}`, 

or, to open it once, run a command `explorer shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}`.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- categ: false
  $name: Display the Control Panel categories as normal folders as well
  $description: Determines whether the Control Panel categories should also be displayed as usual folders.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#ifdef _WIN64
#define CALL_SYM L"__cdecl"
#else
#define CALL_SYM L"__stdcall"
#endif

HRESULT (__stdcall *CreateCommonFrame_orig)(void *, UINT, const GUID*, void **);
HRESULT __stdcall CreateCommonFrame_hook(void * psf, UINT uLayoutType, const GUID* guid, void **ppv)
{  
    if ((uLayoutType == 4) || ((uLayoutType == 3) && Wh_GetIntSetting(L"categ"))) 
        uLayoutType = 1;
    return CreateCommonFrame_orig(psf, uLayoutType, guid, ppv);
}

BOOL Wh_ModInit() 
{
    WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
        {
            {
                L"long " CALL_SYM L" CreateCommonFrame(struct IShellFolder *,enum tagLAYOUTTYPE,struct _GUID const &,void * *)"
            },
            &CreateCommonFrame_orig,
            CreateCommonFrame_hook,
            false
        }
    };

    return WindhawkUtils::HookSymbols(LoadLibraryW(L"shell32.dll"), shell32DllHooks, ARRAYSIZE(shell32DllHooks));
}
