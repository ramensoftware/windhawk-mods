// ==WindhawkMod==
// @id              win7-command-bar
// @name            Windows 7 Command Bar
// @description     Restores the Windows 7 file command bar in the file explorer
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Command Bar
This mod restores the Windows 7 file command bar in the file explorer.

The code is based on the implementation in [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher).

![Screenshot](https://raw.githubusercontent.com/ItsProfessional/Screenshots/main/Windhawk/win7-command-bar.png)
*/
// ==/WindhawkModReadme==

#include <initguid.h>

DEFINE_GUID(CLSID_UIRibbonFramework, 0x926749FA, 0x2615, 0x4987, 0x88, 0x45, 0xC3, 0x3E, 0x65, 0xF2, 0xB9, 0x57);
DEFINE_GUID(IID_UIRibbonFramework, 0xF4F0385D, 0x6872, 0x43A8, 0xAD, 0x09, 0x4C, 0x33, 0x9C, 0xB3, 0xF5, 0xC5);


HRESULT (*ExplorerFrame_CoCreateInstanceOrig)(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv);

HRESULT ExplorerFrame_CoCreateInstanceHook(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv)
{
    if (*(INT64*)&rclsid.Data1 == 0x4D1E5A836480100B && *(INT64*)rclsid.Data4 == 0x339A8EA8E58A699F)
    {
        return REGDB_E_CLASSNOTREG;
    }
    
    if (IsEqualCLSID(rclsid, CLSID_UIRibbonFramework) && IsEqualIID(riid, IID_UIRibbonFramework))
    {
        return REGDB_E_CLASSNOTREG;
    }

    return ExplorerFrame_CoCreateInstanceOrig(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");
    HMODULE hExplorerFrame =  GetModuleHandle(L"api-ms-win-core-com-l1-1-0.dll");

    void* origFunc = (void*)GetProcAddress(hExplorerFrame, "CoCreateInstance");
    Wh_SetFunctionHook(origFunc, (void*)ExplorerFrame_CoCreateInstanceHook, (void**)&ExplorerFrame_CoCreateInstanceOrig);

    return TRUE;
}
