// ==WindhawkMod==
// @id              virtual-desktop-popups-disable
// @name            Disable Virtual Desktop Popups
// @description     Disables "Desktop N" popups on virtual desktop change.
// @version         1.0.0
// @author          Amrsatrio
// @github          https://github.com/Amrsatrio
// @twitter         https://twitter.com/amrsatrio
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Virtual Desktop Popups

![Screenshot of Desktop 1 popup with the popup crossed out](https://i.imgur.com/uena3MS.png)

This simple mod disables the popups saying "Desktop N" that appear when the current virtual desktop is changed on Windows 11 build 226xx.2050+ and 23493+.

Tested on Windows 11 22H2 22621.4317 x64 and 24H2 26100.6899 ARM64.

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

interface IVirtualDesktop;
interface IVirtualDesktopNotification;

using CAudioFlyoutController_VirtualDesktopSwitched_t = HRESULT (STDMETHODCALLTYPE *)(
    IVirtualDesktopNotification* This, IVirtualDesktop* pVirtualDesktop);
CAudioFlyoutController_VirtualDesktopSwitched_t CAudioFlyoutController_VirtualDesktopSwitched_Original;
HRESULT STDMETHODCALLTYPE CAudioFlyoutController_VirtualDesktopSwitched_Hook(
    IVirtualDesktopNotification* This, IVirtualDesktop* pVirtualDesktop)
{
    return S_OK;
}

enum VirtualDesktopSwitchType : int;

using CAudioFlyoutController_VirtualDesktopSwitched23H2_t = HRESULT (STDMETHODCALLTYPE *)(
    IVirtualDesktopNotification* This, IVirtualDesktop* pVirtualDesktop, VirtualDesktopSwitchType switchType);
CAudioFlyoutController_VirtualDesktopSwitched23H2_t CAudioFlyoutController_VirtualDesktopSwitched23H2_Original;
HRESULT STDMETHODCALLTYPE CAudioFlyoutController_VirtualDesktopSwitched23H2_Hook(
    IVirtualDesktopNotification* This, IVirtualDesktop* pVirtualDesktop, VirtualDesktopSwitchType switchType)
{
    return S_OK;
}

BOOL Wh_ModInit()
{
    // twinui.dll
    static const WindhawkUtils::SYMBOL_HOOK c_rgTwinUIHooks[] =
    {
        {
            {
                L"public: virtual long __cdecl CAudioFlyoutController::VirtualDesktopSwitched(struct IVirtualDesktop *)",
            },
            &CAudioFlyoutController_VirtualDesktopSwitched_Original,
            CAudioFlyoutController_VirtualDesktopSwitched_Hook,
            true
        },
        {
            {
                L"public: virtual long __cdecl CAudioFlyoutController::VirtualDesktopSwitched(struct IVirtualDesktop *,enum VirtualDesktopSwitchType)",
            },
            &CAudioFlyoutController_VirtualDesktopSwitched23H2_Original,
            CAudioFlyoutController_VirtualDesktopSwitched23H2_Hook,
            true
        },
    };

    HMODULE hTwinUI = LoadLibraryExW(L"twinui.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hTwinUI == nullptr)
    {
        Wh_Log(L"Failed to load twinui.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(hTwinUI, c_rgTwinUIHooks, ARRAYSIZE(c_rgTwinUIHooks))
        || (CAudioFlyoutController_VirtualDesktopSwitched_Original == nullptr
            && CAudioFlyoutController_VirtualDesktopSwitched23H2_Original == nullptr))
    {
        Wh_Log(L"Failed to hook one or more functions in twinui.dll");
        return FALSE;
    }

    return TRUE;
}
