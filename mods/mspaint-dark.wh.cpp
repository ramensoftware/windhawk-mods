// ==WindhawkMod==
// @id              mspaint-dark
// @name            Dark Paint
// @description     Forcing dark mode in the new Microsoft Paint
// @version         1.0.0
// @author          ahmed605
// @github          https://github.com/ahmed605
// @twitter         https://twitter.com/AhmedWalid605
// @homepage        https://ahmed-walid.tk/
// @include         mspaint.exe
// @compilerOptions -lole32 -lruntimeobject -lgdi32
// ==/WindhawkMod==

#include <Windows.h>
#include <wrl.h>
#include <windows.foundation.h>

using namespace Microsoft::WRL;

enum ApplicationTheme : int
{
    ApplicationTheme_Light = 0,
    ApplicationTheme_Dark = 1
};

MIDL_INTERFACE("74B861A1-7487-46A9-9A6E-C78B512726C5")
IApplication : IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE get_Resources(
        __RPC__deref_out_opt IInspectable** value
    ) PURE;
    virtual HRESULT STDMETHODCALLTYPE put_Resources(
        __RPC__in_opt IInspectable* value
    ) PURE;
    virtual HRESULT STDMETHODCALLTYPE get_DebugSettings(
        __RPC__deref_out_opt IInspectable** value
    ) PURE;
    virtual HRESULT STDMETHODCALLTYPE get_RequestedTheme(
        __RPC__out ApplicationTheme* value
    ) PURE;
    virtual HRESULT STDMETHODCALLTYPE put_RequestedTheme(
        ApplicationTheme value
    ) PURE;                 
};

__CRT_UUID_DECL(IApplication, 0x74B861A1,0x7487,0x46A9, 0x9A,0x6E,0xC7,0x8B,0x51,0x27,0x26,0xC5);

MIDL_INTERFACE("06499997-F7B4-45FE-B763-7577D1D3CB4A")
IApplicationStatics : IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE get_Current(
        __RPC__deref_out_opt IApplication** value
    ) PURE;
};

__CRT_UUID_DECL(IApplicationStatics, 0x06499997,0xF7B4,0x45FE, 0xB7,0x63,0x75,0x77,0xD1,0xD3,0xCB,0x4A);

using RoGetActivationFactory_t = decltype(&RoGetActivationFactory);
RoGetActivationFactory_t origRoGetActivationFactory;

HRESULT RoGetActivationFactoryHook(HSTRING activatableClassId, REFIID iid, void **factory)
{
    unsigned int length = WindowsGetStringLen(activatableClassId);;
    auto buffer = WindowsGetStringRawBuffer(activatableClassId, &length);
    if (wcsicmp(buffer, L"Windows.UI.Xaml.Application") == 0) 
    {
        ComPtr<IApplicationStatics> appStatics;
        origRoGetActivationFactory(activatableClassId, __uuidof(IApplicationStatics),(void**)appStatics.GetAddressOf());

        ComPtr<IApplication> app;
        appStatics->get_Current(app.GetAddressOf());

        if (app != NULL && app.Get() != nullptr)
        {
            app->put_RequestedTheme(ApplicationTheme_Dark);
        }
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
