// ==WindhawkMod==
// @id              hide-volume-indicator
// @name            Hide Volume Indicator
// @description     Hides the Windows on-screen volume slider
// @version         1.0.0
// @author          R.T.
// @github          https://github.com/RT23pt
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Volume Indicator
Hides the Windows 11 on-screen volume slider indicator. The brightness, airplane mode, camera, and microphone indicators will still appear normally.

## Compatibility
This mod depends on `Windows.Internal.HardwareConfirmator.dll`, which is the Windows 11 indicator host. If this DLL is missing from your system (e.g., on older Windows versions), the mod will safely do nothing. Tested on Windows 11.

## Relation to other mods
Note that this is a standalone, lightweight mod strictly for hiding the volume indicator. There is an existing mod called "On-Screen Indicator Position" which allows moving the indicators. While that mod might add a "hide" feature in the future, this mod exists for users who just want a simple, zero-configuration toggle to hide the volume OSD without extra settings.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

HMODULE g_module;

// Public entry point: do nothing, the indicator is never created.
int (WINAPI* ShowVolumeThunk_Original)(void*, int);
int WINAPI ShowVolumeThunk_Hook(void*, int) {
    return 0;
}

// Private coroutine (fire_and_forget uses a hidden return pointer): do nothing.
void* (WINAPI* ShowVolumeAsync_Original)(void*, void*, int);
void* WINAPI ShowVolumeAsync_Hook(void*, void* retval, int) {
    return retval;
}

BOOL Wh_ModInit() {
    g_module = LoadLibraryEx(L"Windows.Internal.HardwareConfirmator.dll",
                             nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_module) {
        Wh_Log(L"Couldn't load Windows.Internal.HardwareConfirmator.dll");
        return FALSE;
    }

    // Windows.Internal.HardwareConfirmator.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost,struct winrt::Windows::Internal::HardwareConfirmator::IHardwareConfirmatorHost>::ShowVolume(int))"},
            &ShowVolumeThunk_Original,
            ShowVolumeThunk_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowVolumeAsync(int))"},
            &ShowVolumeAsync_Original,
            ShowVolumeAsync_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(g_module, hooks, ARRAYSIZE(hooks)) ||
        (!ShowVolumeThunk_Original && !ShowVolumeAsync_Original)) {
        Wh_Log(L"No ShowVolume symbols were resolved");
        FreeLibrary(g_module);
        g_module = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_module) {
        FreeLibrary(g_module);
        g_module = nullptr;
    }
}
