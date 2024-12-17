// ==WindhawkMod==
// @id              old-this-pc-commands
// @name            Old This PC Commands
// @description     Makes "Open Settings", "System properties", etc. in This PC use Control Panel instead of Settings
// @version         1.0.1
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Old Explorer Commands
This mod makes the following commands in This PC, which are normally hardcoded to open Settings, open Control Panel instead.
- Open Settings
- System properties
- Uninstall or change a program

# !!IMPORTANT!!

Beginning with Iron (Windows Server 2022), a redirect from the System page to Settings is **hardcoded** into `ExplorerFrame.dll`.
As such, this mod will not work on Windows Server 2022, Windows 11, or greater to restore the old System page unless a modification
to `ExplorerFrame.dll` takes place to remove the redirect.
*/
// ==/WindhawkModReadme==

#include <initguid.h>
#include <windhawk_utils.h>
#include <shobjidl.h>

HRESULT OpenCplPage(IUnknown *punk, LPCWSTR lpPage, LPCWSTR lpSubPage)
{
    IObjectWithSite *pows;
    HRESULT hr = punk->QueryInterface(IID_IObjectWithSite, (void **)&pows);
    if (SUCCEEDED(hr))
    {
        IUnknown *punk2;
        hr = pows->GetSite(IID_PPV_ARGS(&punk2));
        if (SUCCEEDED(hr))
        {
            IOpenControlPanel *pocp = nullptr;
            hr = CoCreateInstance(
                CLSID_OpenControlPanel,
                NULL,
                CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER,
                IID_PPV_ARGS(&pocp)
            );
            if (SUCCEEDED(hr))
            {
                hr = pocp->Open(lpPage, lpSubPage, punk2);
                pocp->Release();
            }
            punk2->Release();
        }
        pows->Release();
    }
    return hr;
}

typedef HRESULT (*CDrivesViewCallback__OnEvent)(IUnknown *, IShellItemArray *, IBindCtx *);
typedef HRESULT (*CDrivesViewCallback__OnEventPunkSite)(IUnknown *, IUnknown *, IShellItemArray *, IBindCtx *);

CDrivesViewCallback__OnEventPunkSite CDrivesViewCallback__OnOpenSystemSettingsPunkSite_orig;
HRESULT CDrivesViewCallback__OnOpenSystemSettingsPunkSite_hook(
    IUnknown        *punk,
    IUnknown        *punkRibbon,
    IShellItemArray *psia,
    IBindCtx        *pbctx
)
{
    HRESULT hr = OpenCplPage(punk, NULL, NULL);
    if (!SUCCEEDED(hr))
    {
        hr = OpenCplPage(punkRibbon, NULL, NULL);
    }
    return hr;
}

CDrivesViewCallback__OnEventPunkSite CDrivesViewCallback__OnSystemPropertiesPunkSite_orig;
HRESULT CDrivesViewCallback__OnSystemPropertiesPunkSite_hook(
    IUnknown        *punk,
    IUnknown        *punkRibbon,
    IShellItemArray *psia,
    IBindCtx        *pbctx
)
{
    HRESULT hr = OpenCplPage(punk, L"Microsoft.System", NULL);
    if (!SUCCEEDED(hr))
    {
        hr = OpenCplPage(punkRibbon, L"Microsoft.System", NULL);
    }
    return hr;
}

CDrivesViewCallback__OnEventPunkSite CDrivesViewCallback__OnAddRemoveProgramsPunkSite_orig;
HRESULT CDrivesViewCallback__OnAddRemoveProgramsPunkSite_hook(
    IUnknown        *punk,
    IUnknown        *punkRibbon,
    IShellItemArray *psia,
    IBindCtx        *pbctx
)
{
    HRESULT hr = OpenCplPage(punk, L"Microsoft.ProgramsAndFeatures", NULL);
    if (!SUCCEEDED(hr))
    {
        hr = OpenCplPage(punkRibbon, L"Microsoft.ProgramsAndFeatures", NULL);
    }
    return hr;
}

const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        {
            L"public: static long __cdecl CDrivesViewCallback::_OnOpenSystemSettingsPunkSite(struct IUnknown *,struct IUnknown *,struct IShellItemArray *,struct IBindCtx *)"
        },
        &CDrivesViewCallback__OnOpenSystemSettingsPunkSite_orig,
        CDrivesViewCallback__OnOpenSystemSettingsPunkSite_hook,
        false
    },
    {
        {
            L"public: static long __cdecl CDrivesViewCallback::_OnSystemPropertiesPunkSite(struct IUnknown *,struct IUnknown *,struct IShellItemArray *,struct IBindCtx *)"
        },
        &CDrivesViewCallback__OnSystemPropertiesPunkSite_orig,
        CDrivesViewCallback__OnSystemPropertiesPunkSite_hook,
        false
    },
    {
        {
            L"public: static long __cdecl CDrivesViewCallback::_OnAddRemoveProgramsPunkSite(struct IUnknown *,struct IUnknown *,struct IShellItemArray *,struct IBindCtx *)"
        },
        &CDrivesViewCallback__OnAddRemoveProgramsPunkSite_orig,
        CDrivesViewCallback__OnAddRemoveProgramsPunkSite_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hShell32,
        shell32DllHooks,
        ARRAYSIZE(shell32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in shell32.dll");
        return FALSE;
    }

    return TRUE;
}