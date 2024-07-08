// ==WindhawkMod==
// @id              edge-window-tab-manager-block
// @name            Prevent Edge from lagging your Taskbar process
// @description     Blocks Edge window Alt-Tab manager
// @version         1.0
// @author          Roland Pihlakas
// @github          https://github.com/levitation
// @homepage        https://www.simplify.ee/
// @compilerOptions -lkernel32
// @include         msedge.exe
// ==/WindhawkMod==

// Source code is published under The MIT License.
//
// This mod is built as a variation of the code developed by gexgd0419
// https://github.com/gexgd0419/EdgeWindowTabManagerBlock
//
// For mod bug reports and feature requests, please open an issue here:
// https://github.com/levitation-opensource/my-windhawk-mods/issues
//
// For mod pull requests, development takes place here:
// https://github.com/levitation-opensource/my-windhawk-mods/

// ==WindhawkModReadme==
/*
# Prevent Edge from lagging your Taskbar process

This mod is for tab hoarders using Edge browser. 

When you set Alt-Tab to show open windows only (and not individual tabs), it should turn off a problematic feature in the Taskbar process. However, in reality, when you have many tabs open in Edge, the existence of this "Alt-Tab showing browser tabs" feature in Windows still causes issues - the Taskbar will lag and start to consume lot of CPU. This regardless whether the browser tabs are shown in Alt-Tab. There was an experimental Edge flag `#edge-window-tab-manager` that you could set to "off" in order to disable the problematic feature. However, the flag has been removed. This mod solves the issue in a different way.


## Installation

The mod takes effect next time you launch Edge. In order for the mod to take effect immediately, ensure that no Edge processes remain in memory when first installing this mod. Alternatively, restart your computer after installing the mod.


## Troubleshooting

If the above installation procedure does not help, look in Edge's system settings at the following url:\
`edge://settings/system`\
Ensure that the following options are disabled:
- "Startup boost" 
- "Continue running background extensions and apps when Microsoft Edge is closed"


## How it works

It operates by blocking the problematic Edge window Alt-Tab manager functionality, which in turn would affect the Taskbar operation. For time being, only Edge needs this mod, other Chromium-based browsers do not need it.


## What would be the alternatives to using Edge altogether?

As a problem-free alternative, I recommend Brave browser. It has same vertical tabs functionality as Edge does, so they are quite similar. Brave does not hog the Taskbar process.


## More info

See also the following Reddit post:
[Having too many (1,000+) Microsoft Edge tabs open can break File Explorer in Windows 10](https://www.reddit.com/r/edge/comments/1090h93/having_too_many_1000_microsoft_edge_tabs_open_can/).

This mod is built as a variation of the code of a loader program developed by gexgd0419:
[EdgeWindowTabManagerBlock: A program that blocks the WindowTabManager feature of Microsoft Edge](https://github.com/gexgd0419/EdgeWindowTabManagerBlock).

The benefit of this small mod is that you can conveniently install or uninstall the fix directly via Windhawk UI with fewer steps and less attention to technical details.
*/
// ==/WindhawkModReadme==


#include <roapi.h>


template <typename T>
BOOL Wh_SetFunctionHookT(
    FARPROC targetFunction,
    T hookFunction,
    T* originalFunction
) {
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction, (void**)originalFunction);
}


HMODULE hCombase = NULL;


typedef PCWSTR(WINAPI* WindowsGetStringRawBuffer_t)(HSTRING, UINT32*);
WindowsGetStringRawBuffer_t pWindowsGetStringRawBuffer;

typedef HRESULT(WINAPI* RoGetActivationFactory_t)(HSTRING, REFIID, void**);
RoGetActivationFactory_t pOriginalRoGetActivationFactory;
typedef HRESULT(WINAPI* RoActivateInstance_t)(HSTRING, IInspectable**);
RoActivateInstance_t pOriginalRoActivateInstance;


bool IsRoClassBlocked(HSTRING activatableClassId) {

    PCWSTR pszClassId = pWindowsGetStringRawBuffer(activatableClassId, nullptr);
    if (
        wcscmp(pszClassId, L"WindowsUdk.UI.Shell.WindowTabManager") == 0
        || wcscmp(pszClassId, L"Windows.UI.Shell.WindowTabManager") == 0   //case sensitive
    ) {
        return true;
    }
    else {
        return false;
    }
}


HRESULT WINAPI RoGetActivationFactoryHook(
    IN  HSTRING activatableClassId,
    IN  REFIID  iid,
    OUT void**  factory
) {
    if (IsRoClassBlocked(activatableClassId))
        return E_ACCESSDENIED;
    else
        return pOriginalRoGetActivationFactory(activatableClassId, iid, factory);
}

HRESULT WINAPI RoActivateInstanceHook(
    IN  HSTRING         activatableClassId,
    OUT IInspectable**  instance
) {
    if (IsRoClassBlocked(activatableClassId))
        return E_ACCESSDENIED;
    else
        return pOriginalRoActivateInstance(activatableClassId, instance);
}


BOOL Wh_ModInit() {

    Wh_Log(L"Init");

    LPCWSTR commandLine = GetCommandLineW();    //the lifetime of the returned value is managed by the system, applications should not free or modify this value
    if (wcsstr(commandLine, L"--type=")) {     //no need to hook Edge subprocesses
        Wh_Log(L"Skipping Edge subprocess");
        return FALSE;
    }


    hCombase = LoadLibraryW(L"combase.dll");    //ensure that the library is loaded by the time we do the hooking
    if (!hCombase) {
        Wh_Log(L"Loading combase.dll failed");
        return FALSE;
    }

    pWindowsGetStringRawBuffer = (WindowsGetStringRawBuffer_t)GetProcAddress(hCombase, "WindowsGetStringRawBuffer");
    if (!pWindowsGetStringRawBuffer) {
        Wh_Log(L"Finding helper functions from combase.dll failed");
        return FALSE;
    }

    FARPROC pRoGetActivationFactory = GetProcAddress(hCombase, "RoGetActivationFactory");
    FARPROC pRoActivateInstance = GetProcAddress(hCombase, "RoActivateInstance");
    if (
        !pRoGetActivationFactory
        || !pRoActivateInstance
    ) {
        Wh_Log(L"Finding hookable functions from combase.dll failed");
        return FALSE;
    }

    Wh_Log(L"Activating the hooks");

    Wh_SetFunctionHookT(pRoGetActivationFactory, RoGetActivationFactoryHook, &pOriginalRoGetActivationFactory);
    Wh_SetFunctionHookT(pRoActivateInstance, RoActivateInstanceHook, &pOriginalRoActivateInstance);

    return TRUE;
}

void Wh_ModUninit() {

    Wh_Log(L"Uninit");

    if (hCombase) {
        FreeLibrary(hCombase);
        hCombase = NULL;
        pWindowsGetStringRawBuffer = NULL;
    }
}
