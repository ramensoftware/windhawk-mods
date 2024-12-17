// ==WindhawkMod==
// @id              mspaint-dark
// @name            Dark Paint
// @description     Forcing dark mode in the new Microsoft Paint
// @version         1.0.1
// @author          ahmed605
// @github          https://github.com/ahmed605
// @twitter         https://twitter.com/AhmedWalid605
// @homepage        https://ahmed-walid.tk/
// @include         mspaint.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dark Paint
This mod changes Microsoft Paint's theme to be dark, as in the screenshot below.
![Screenshot](https://i.imgur.com/Ce2PMzr.png)
*/
// ==/WindhawkModReadme==

#undef GetCurrentTime

#include <Windows.h>
#include <roapi.h>
#include <winstring.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

using RoGetActivationFactory_t = decltype(&RoGetActivationFactory);
RoGetActivationFactory_t origRoGetActivationFactory;

static bool ignoreHooking = false;

HRESULT RoGetActivationFactoryHook(HSTRING activatableClassId, REFIID iid, void **factory)
{
    if (!ignoreHooking && wcsicmp(WindowsGetStringRawBuffer(activatableClassId, NULL), L"Windows.UI.Xaml.Application") == 0) 
    {
        ignoreHooking = true;
        try
        {
            Application::Current().RequestedTheme(ApplicationTheme::Dark);
        } catch (...) { }
        ignoreHooking = false;
    }

    return origRoGetActivationFactory(activatableClassId, iid, factory);
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    HMODULE winrtModule = GetModuleHandle(L"api-ms-win-core-winrt-l1-1-0.dll");
    void* roAc = (void*)GetProcAddress(winrtModule, "RoGetActivationFactory");
    Wh_SetFunctionHook(roAc, (void*)RoGetActivationFactoryHook, (void**)&origRoGetActivationFactory);

    return TRUE;
}
