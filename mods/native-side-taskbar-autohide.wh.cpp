// ==WindhawkMod==
// @id              native-side-taskbar-autohide
// @name            Native Side Taskbar Auto-Hide
// @description     Enable Windows 11's built-in auto-hide for the native taskbar when docked to a side or the top.
// @version         1.0.0
// @author          Durst
// @github          https://github.com/DurstAuric
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// Source code is published under the GNU General Public License v3.0.
// Based on the auto-hide feature hook from "Vertical Taskbar for Windows 11"
// by m417z, also licensed under GPL-3.0:
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-vertical.wh.cpp

// ==WindhawkModReadme==
/*
# Native Side Taskbar Auto-Hide

Enables Windows 11's **native taskbar auto-hide** when the taskbar is positioned
at the **left, right, or top** of the screen using Windows' own positioning
feature. This mod doesn't move the taskbar or implement custom animations.

## How to use

1. Position the taskbar using Windows' **native** taskbar-position setting.
2. Open **Settings > Personalization > Taskbar > Taskbar behaviours** and turn on
   **Automatically hide the taskbar**.
3. Enable this mod. If auto-hide does not activate immediately, toggle the
   Windows auto-hide setting off and on again, or restart Windows Explorer.

## Compatibility and limitations

- Intended for Windows 11 builds that support **native non-bottom taskbar positioning**.
  This mod **does not** add taskbar positioning to unsupported builds.
- Only the native primary taskbar's restriction has been identified in the
  upstream source. Multi-monitor/secondary-taskbar behaviour is not guaranteed.
- An internal feature-check symbol in `taskbar.dll` must be present. A Windows
  update could rename or remove it, in which case this mod may fail to load.
- Avoid running this alongside other mods that hook the same internal function.
- The mod currently overrides the feature check whenever it is called, including
  when the taskbar is at the bottom. That is the minimal implementation which
  has been user-tested; the effects on other configurations are not fully known.

## Troubleshooting

If the taskbar behaves unexpectedly, disable this mod in Windhawk and restart
Windows Explorer. Check Windhawk's mod log for symbol-resolution errors.

## Credits and license

The hook and internal feature identification are adapted from
[Vertical Taskbar for Windows 11](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-vertical.wh.cpp)
by **m417z**. This derivative is distributed under **GPL-3.0**.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_utils.h>

using FeatureIsEnabled_t = bool(WINAPI*)(void* self);
static FeatureIsEnabled_t FeatureIsEnabled_Original = nullptr;

// The upstream mod identifies this feature check as the restriction on native
// primary-taskbar auto-hide when the taskbar is positioned away from the bottom.
static bool WINAPI FeatureIsEnabled_Hook(void* self) {
    return false;
}

BOOL Wh_ModInit() {
    HMODULE taskbarDll = LoadLibraryExW(
        L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbarDll) {
        Wh_Log(L"Failed to load taskbar.dll (error %lu)", GetLastError());
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_61970341>::__private_IsEnabled(void))"},
            &FeatureIsEnabled_Original,
            FeatureIsEnabled_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(taskbarDll, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"Could not hook native taskbar auto-hide feature check");
        return FALSE;
    }

    Wh_Log(L"Native side taskbar auto-hide hook installed");
    return TRUE;
}
