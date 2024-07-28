// ==WindhawkMod==
// @id              win32-tray-clock-experience
// @name            Win32 Tray Clock Experience
// @description     Use the Win32 clock flyout instead of the XAML one
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Win32 Tray Clock Experience
This mod makes the clock button on the Windows 10 taskbar open the
Win32 tray clock from Windows 8.1 and before instead of the XAML one
from Windows 10 and up. This *should* work on Windows 11 with the legacy
taskbar, but I have not tested it.

Based on the ExplorerPatcher implementation.

**Preview**:

![Preview](https://raw.githubusercontent.com/aubymori/images/main/win32-tray-clock-experience.png)
*/
// ==/WindhawkModReadme==

#include <initguid.h>
#include <windhawk_utils.h>
#include <winnt.h>

#define STATUS_SUCCESS 0x00000000

typedef NTSTATUS (NTAPI *RtlGetVersion_t)(PRTL_OSVERSIONINFOW);
RtlGetVersion_t RtlGetVersion;

DEFINE_GUID(GUID_Win32Clock,
    0x0A323554A,
    0x0FE1, 0x4E49, 0xae, 0xe1,
    0x67, 0x22, 0x46, 0x5d, 0x79, 0x9f
);
DEFINE_GUID(IID_Win32Clock,
    0x7A5FCA8A,
    0x76B1, 0x44C8, 0xa9, 0x7c,
    0xe7, 0x17, 0x3c, 0xca, 0x5f, 0x4f
);
typedef interface Win32Clock Win32Clock;

typedef struct Win32ClockVtbl
{
    BEGIN_INTERFACE

        HRESULT(STDMETHODCALLTYPE* QueryInterface)(
            Win32Clock* This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */
            _COM_Outptr_  void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        Win32Clock* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        Win32Clock* This);

    HRESULT(STDMETHODCALLTYPE* ShowWin32Clock)(
        Win32Clock* This,
        /* [in] */ HWND hWnd,
        /* [in] */ LPRECT lpRect);

    END_INTERFACE
} Win32ClockVtbl;

interface Win32Clock
{
    CONST_VTBL struct Win32ClockVtbl* lpVtbl;
};

void (*ClockButton_StartTicking)(void *pThis);

#define ClockButton_Window(pThis) *((HWND *)pThis + 1)

HRESULT (*ClockButton_v_OnClick_orig)(void *, UINT);
HRESULT ClockButton_v_OnClick_hook(
    void *pThis,
    UINT  uClickDevice
)
{
    HRESULT hr = S_OK;
    if (!FindWindowW(L"ClockFlyoutWindow", NULL))
    {
        Win32Clock *pClock = NULL;
        hr = CoCreateInstance(
            GUID_Win32Clock,
            NULL,
            CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
            IID_Win32Clock,
            (void **)&pClock
        );
        if (SUCCEEDED(hr))
        {
            HWND hClock = ClockButton_Window(pThis);
            RECT rc;
            GetWindowRect(hClock, &rc);
            hr = pClock->lpVtbl->ShowWin32Clock(
                pClock,
                hClock,
                &rc
            );
            if (SUCCEEDED(hr))
            {
                ClockButton_StartTicking(pThis);
            }
            pClock->lpVtbl->Release(pClock);
        }
    }
    return hr;
}

const WindhawkUtils::SYMBOL_HOOK hooks[] = {
    {
        {
            L"public: void __cdecl ClockButton::StartTicking(void)"
        },
        &ClockButton_StartTicking,
        nullptr,
        false
    },
    {
        {
            L"protected: virtual long __cdecl ClockButton::v_OnClick(enum ClickDevice)"
        },
        &ClockButton_v_OnClick_orig,
        ClockButton_v_OnClick_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
    if (!hNtDll)
    {
        Wh_Log(L"Failed to load ntdll.dll");
        return FALSE;
    }

    RtlGetVersion = (RtlGetVersion_t)GetProcAddress(hNtDll, "RtlGetVersion");
    if (!RtlGetVersion)
    {
        Wh_Log(L"Failed to load RtlGetVersion from ntdll.dll");
        return FALSE;
    }

    HMODULE hTaskbar = NULL;
    RTL_OSVERSIONINFOW osv = { sizeof(RTL_OSVERSIONINFOW) };
    if (STATUS_SUCCESS == RtlGetVersion(&osv))
    {
        hTaskbar = (osv.dwBuildNumber >= 22000) ? LoadLibraryW(L"Taskbar.dll") : GetModuleHandleW(NULL);
    }
    else
    {
        Wh_Log(L"RtlGetVersion failed");
        return FALSE;
    }

    if (!hTaskbar)
    {
        Wh_Log(L"Failed to load Taskbar.dll");
        return FALSE;
    }
    
    if (!WindhawkUtils::HookSymbols(
        hTaskbar,
        hooks,
        ARRAYSIZE(hooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions");
        return FALSE;
    }

    return TRUE;
}
