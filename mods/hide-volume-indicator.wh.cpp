// ==WindhawkMod==
// @id              hide-volume-indicator
// @name            Hide Volume Indicator
// @description     Hides the Windows on-screen volume slider
// @version         1.1.0
// @author          R.T.
// @github https://github.com/RT23pt
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
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
        return FALSE;
    }

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
