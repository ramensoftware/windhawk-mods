// ==WindhawkMod==
// @id              start-menu-hide-all-apps-and-resize
// @name            Hide All Apps and Resize Start Menu
// @description     Сompact Start Menu without the 'All apps' list for a cleaner workspace.
// @version         1.4.4
// @author          pacukevich
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Minimalist Start Menu
This mod transforms the standard Windows 11 Start Menu into a compact and efficient workspace.

### Key Features:
* **Hide "All apps":** Completely removes the "All apps" section, leaving only your pinned icons for a cleaner look.
* **Compact Dimensions:** Forces the Start Menu into a fixed, streamlined size (640x580).

### How It Works:
The mod utilizes Windows Registry policies to manage element visibility and hooks the layout engine within `StartMenu.dll` to override the window dimensions.

### How To Make It Work:
To apply the changes after installation or updates, you must **restart explorer.exe** (via Task Manager) or **reboot your computer**. This ensures that the Start Menu host process reloads the new configuration.

---
**Author:** pacukevich
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

// --- Part 1: Compact Size (Static Dimensions) ---

struct WinSize {
    float Width;
    float Height;
};

using MeasureOverride_t = WinSize(WINAPI*)(void* pThis, WinSize availableSize);
MeasureOverride_t MeasureOverride_Original;

/**
 * HOOK: Overwrites the Start Menu layout size.
 */
WinSize WINAPI MeasureOverride_Hook(void* pThis, WinSize availableSize) {
    WinSize size = MeasureOverride_Original(pThis, availableSize);
    
    // Setting fixed compact dimensions: 640 width, 580 height
    size.Width = 640.0f;
    size.Height = 580.0f;
    
    return size;
}

// --- Part 2: Hiding "All Apps" Section (Registry Policy) ---

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;

/**
 * HOOK: Forces the system to believe the 'NoStartMenuMorePrograms' policy is active.
 * This effectively removes the "All apps" section from the Start Menu.
 */
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    if (lpValueName && lstrcmpiW(lpValueName, L"NoStartMenuMorePrograms") == 0) {
        if (lpType) *lpType = REG_DWORD;
        if (lpData && lpcbData && *lpcbData >= sizeof(DWORD)) {
            *(DWORD*)lpData = 1; // 1 means the policy is enabled (Hide All Apps)
            *lpcbData = sizeof(DWORD);
        }
        return ERROR_SUCCESS;
    }
    return RegQueryValueExW_Original(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

// --- Part 3: Initialization and Symbol Management ---

/**
 * Hooks the necessary layout functions in StartMenu.dll using symbol names.
 */
bool HookStartMenuSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                // Classic Windows 11 Start Menu Frame
                LR"(public: virtual struct Windows::Foundation::Size __cdecl winrt::StartMenu::implementation::StartInnerFrame::MeasureOverride(struct Windows::Foundation::Size))",
                // New Unified Start Menu Frame (2025/2026 builds)
                LR"(public: virtual struct Windows::Foundation::Size __cdecl winrt::StartMenu::implementation::UnifiedStartView::MeasureOverride(struct Windows::Foundation::Size))"
            },
            &MeasureOverride_Original,
            MeasureOverride_Hook
        }
    };
    return WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Minimalist Start Menu Mod by pacukevich...");

    // Hooking the layout size logic
    HMODULE startMenuDll = GetModuleHandle(L"StartMenu.dll");
    if (startMenuDll) {
        if (HookStartMenuSymbols(startMenuDll)) {
            Wh_ApplyHookOperations();
        }
    }

    // Hooking the registry check for "All apps" visibility in kernelbase.dll
    HMODULE kernelBase = GetModuleHandle(L"kernelbase.dll");
    if (kernelBase) {
        Wh_SetFunctionHook((void*)GetProcAddress(kernelBase, "RegQueryValueExW"), (void*)RegQueryValueExW_Hook, (void**)&RegQueryValueExW_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Mod Unloaded");

    
}
