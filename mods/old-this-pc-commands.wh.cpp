// ==WindhawkMod==
// @id              old-this-pc-commands
// @name            Old This PC Commands
// @description     Makes "Open Settings", "System properties", etc. in This PC use Control Panel instead of Settings
// @version         1.0.0
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
#include <shlobj.h>
#include <shobjidl.h>
#include <ocidl.h>

DEFINE_GUID(IID_IUnknown,         0x00000000, 0x0000, 0x0000, 0xc0,0x00, 0x00,0x00,0x00,0x00,0x00,0x46);
DEFINE_GUID(IID_OpenControlPanel, 0xD11AD862, 0x66DE, 0x4DF4, 0xBF,0x6C, 0x1F,0x56,0x21,0x99,0x6A,0xF1);
DEFINE_GUID(IID_IServiceProvider, 0x6D5140C1, 0x7436, 0x11CE, 0x80,0x34, 0x00,0xAA,0x00,0x60,0x09,0xFA);
DEFINE_GUID(SID_STopLevelBrowser, 0x4c96be40, 0x915C, 0x11CF, 0x99,0xD3, 0x00,0xAA,0x00,0x4A,0xE8,0x37);

HRESULT (*COpenControlPanel_Open)(void *pThis, LPCWSTR lpPage, LPCWSTR lpSubPage, IUnknown *pu);

HRESULT OpenCplPage(IUnknown *pu, LPCWSTR lpPage, LPCWSTR lpSubPage)
{
    IObjectWithSite *pows;
    HRESULT hr = pu->QueryInterface(IID_IObjectWithSite, (void **)&pows);
    if (SUCCEEDED(hr))
    {
        IUnknown *pu2;
        hr = pows->GetSite(IID_IUnknown, (void **)&pu2);
        if (SUCCEEDED(hr))
        {
            void *pocp;
            hr = CoCreateInstance(
                CLSID_OpenControlPanel,
                NULL,
                CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER,
                IID_OpenControlPanel,
                &pocp
            );
            if (SUCCEEDED(hr))
            {
                hr = COpenControlPanel_Open(pocp, lpPage, lpSubPage, pu2);
            }
        }
    }
    return hr;
}

typedef HRESULT (*CDrivesViewCallback__OnEvent)(IUnknown *, IShellItemArray *, IBindCtx *);
typedef HRESULT (*CDrivesViewCallback__OnEventPunkSite)(IUnknown *, IUnknown *, IShellItemArray *, IBindCtx *);

CDrivesViewCallback__OnEventPunkSite CDrivesViewCallback__OnOpenSystemSettingsPunkSite_orig;
HRESULT CDrivesViewCallback__OnOpenSystemSettingsPunkSite_hook(
    IUnknown        *pu,
    IUnknown        *pu2,
    IShellItemArray *psia,
    IBindCtx        *pbctx
)
{
    HRESULT hr = OpenCplPage(pu, NULL, NULL);
    if (!SUCCEEDED(hr))
    {
        hr = OpenCplPage(pu2, NULL, NULL);
    }
    return hr;
}

CDrivesViewCallback__OnEventPunkSite CDrivesViewCallback__OnSystemPropertiesPunkSite_orig;
HRESULT CDrivesViewCallback__OnSystemPropertiesPunkSite_hook(
    IUnknown        *pu,
    IUnknown        *pu2,
    IShellItemArray *psia,
    IBindCtx        *pbctx
)
{
    HRESULT hr = OpenCplPage(pu, L"Microsoft.System", NULL);
    if (!SUCCEEDED(hr))
    {
        hr = OpenCplPage(pu2, L"Microsoft.System", NULL);
    }
    return hr;
}

CDrivesViewCallback__OnEventPunkSite CDrivesViewCallback__OnAddRemoveProgramsPunkSite_orig;
HRESULT CDrivesViewCallback__OnAddRemoveProgramsPunkSite_hook(
    IUnknown        *pu,
    IUnknown        *pu2,
    IShellItemArray *psia,
    IBindCtx        *pbctx
)
{
    HRESULT hr = OpenCplPage(pu, L"Microsoft.ProgramsAndFeatures", NULL);
    if (!SUCCEEDED(hr))
    {
        hr = OpenCplPage(pu2, L"Microsoft.ProgramsAndFeatures", NULL);
    }
    return hr;
}

BOOL Wh_ModInit(void)
{
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    const WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                L"public: virtual long __cdecl COpenControlPanel::Open(unsigned short const *,unsigned short const *,struct IUnknown *)"
            },
            &COpenControlPanel_Open,
            nullptr,
            false
        },
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

    if (!WindhawkUtils::HookSymbols(
        hShell32,
        hooks,
        ARRAYSIZE(hooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in shell32.dll");
        return FALSE;
    }

    return TRUE;
}