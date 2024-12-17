// ==WindhawkMod==
// @id              classic-clock-button-behavior
// @name            Classic Clock Button Behavior
// @description     Makes the tray clock open the Date and Time CPL on double click
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Clock Button Behavior
In Windows XP and before, the tray clock did nothing on single click, and opened
the Date and Time CPL on double click. This mod makes the Windows 10 tray clock
button do that.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>

constexpr UINT_PTR DOUBLE_CLICK_TIMER_ID = 5500;

bool g_bDoubleClicked = false;

void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
    g_bDoubleClicked = false;
    KillTimer(hWnd, nIDEvent);
}

#define ClockButton_Window(pThis) *((HWND *)pThis + 1)

HRESULT (*ClockButton_v_OnClick_orig)(void *, UINT);
HRESULT ClockButton_v_OnClick_hook(
    void *pThis,
    UINT  uClickDevice
)
{
    if (g_bDoubleClicked)
    {
        g_bDoubleClicked = false;
        ShellExecuteW(
            ClockButton_Window(pThis),
            NULL,
            L"timedate.cpl",
            NULL,
            NULL,
            SW_SHOWNORMAL
        );
    }
    else
    {
        g_bDoubleClicked = true;
        SetTimer(
            NULL,
            DOUBLE_CLICK_TIMER_ID,
            GetDoubleClickTime(),
            TimerProc
        );
    }
    return S_OK;
}

const WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
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
    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        explorerExeHooks,
        ARRAYSIZE(explorerExeHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in explorer.exe");
        return FALSE;
    }
    return TRUE;
}