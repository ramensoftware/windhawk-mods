// ==WindhawkMod==
// @id              disable-edge-window-tab-manager
// @name            Disable EdgeWindowTabManager
// @description     Blocks the WindowTabManager feature of Microsoft Edge
// @version         0.1
// @author          ilyfairy
// @github          https://github.com/ilyfairy
// @homepage        https://github.com/ilyfairy
// @include         msedge.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DisableEdgeWindowTabManager
see https://github.com/gexgd0419/EdgeWindowTabManagerBlock

But EdgeWindowTabManagerBlock has some problems,
for example, unable to start msedge through VSCode: `unable to attach to browser`
*/
// ==/WindhawkModReadme==


#include <windhawk_api.h>
#include <wrl.h>

using WindowsGetStringRawBufferFunc = PCWSTR (*) (HSTRING string, UINT32* length);
WindowsGetStringRawBufferFunc MyWindowsGetStringRawBuffer;

bool IsRoClassBlocked(HSTRING activatableClassId)
{
    PCWSTR pszClassId = MyWindowsGetStringRawBuffer(activatableClassId, nullptr);
    if (wcscmp(pszClassId, L"WindowsUdk.UI.Shell.WindowTabManager") == 0
        || wcscmp(pszClassId, L"Windows.UI.Shell.WindowTabManager") == 0) // case sensitive
        return true;

    return false;
}

using RoGetActivationFactoryFunc = HRESULT (*)(HSTRING activatableClassId, REFIID id, void** factory);
RoGetActivationFactoryFunc RoGetActivationFactoryOriginFunc;

HRESULT RoGetActivationFactoryHook(HSTRING activatableClassId, REFIID id, void** factory) {
    if(IsRoClassBlocked(activatableClassId)) {
        Wh_Log(L"IsRoClassBlocked");
        return E_ACCESSDENIED;
    }
    auto result = RoGetActivationFactoryOriginFunc(activatableClassId, id, factory);
    return result;
}


BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    auto comBase = LoadLibrary(L"combase.dll");
    MyWindowsGetStringRawBuffer = (WindowsGetStringRawBufferFunc)GetProcAddress(comBase, "WindowsGetStringRawBuffer");
    // Wh_Log(L"WindowsGetStringRawBuffer: 0x%X", MyWindowsGetStringRawBuffer);

    auto RoGetActivationFactory = GetProcAddress(comBase, "RoGetActivationFactory");
    // Wh_Log(L"RoGetActivationFactory: 0x%X", RoGetActivationFactory);
    Wh_SetFunctionHook((void*)RoGetActivationFactory, (void*)RoGetActivationFactoryHook, (void**)&RoGetActivationFactoryOriginFunc);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {

}
