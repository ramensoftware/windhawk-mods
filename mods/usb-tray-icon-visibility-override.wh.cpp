// ==WindhawkMod==
// @id              usb-tray-icon-visibility-override
// @name            Always Show/Hide USB Tray Icon
// @description     Always shows or hides the "Safely Remove Hardware and Eject Media" icon in the system tray
// @version         1.0
// @author          Kitsune
// @github          https://github.com/AromaKitsune
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Always Show/Hide USB Tray Icon
By default, the "Safely Remove Hardware and Eject Media" icon in the system tray
is shown when any removable devices are plugged into the PC, and completely
hidden when none are plugged in.

This mod allows you to override the default visibility state of this tray icon:
always show it to keep the "Open Devices and Printers" menu item accessible, or
always hide it completely to declutter the system tray.

![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/usb-tray-icon-visibility-override.png)

Looking to declutter the system tray even further? Check out these mods:

| Mod | Author | Description |
| :-- | :----- | :---------- |
| [Taskbar tray system icon tweaks](https://windhawk.net/mods/taskbar-tray-system-icon-tweaks) | m417z | Hides system icons: volume, network, battery, etc. (Windows 11 only) |
| [Disable Tray Icons](https://windhawk.net/mods/disable-tray-icons) | aubymori | Disables all tray icons for specific apps |
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- trayIconVisibilityState: alwaysShown
  $name: Tray icon visibility state
  $options:
    - alwaysShown: Always shown
    - alwaysHidden: Always hidden
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <atomic>
#include <cwchar>

#define HOTPLUGSHOWNOTIFYICON  L"void __cdecl HotPlugShowNotifyIcon(int)"
#define UPDATEHOTPLUGICONSTATE L"void __cdecl UpdateHotPlugIconState(void)"

struct
{
    std::atomic<int> trayIconVisibilityState = 1;
} settings;

std::atomic<bool> g_unloading = false;

// Hook for HotPlugShowNotifyIcon
using HotPlugShowNotifyIcon_t = void(__cdecl*)(int);
HotPlugShowNotifyIcon_t HotPlugShowNotifyIcon_Original;
void __cdecl HotPlugShowNotifyIcon_Hook(int show)
{
    if (g_unloading.load())
    {
        HotPlugShowNotifyIcon_Original(show);
        return;
    }
    HotPlugShowNotifyIcon_Original(settings.trayIconVisibilityState.load());
}

// Hook for UpdateHotPlugIconState
using UpdateHotPlugIconState_t = void(__cdecl*)();
UpdateHotPlugIconState_t UpdateHotPlugIconState_Original;
void __cdecl UpdateHotPlugIconState_Hook()
{
    UpdateHotPlugIconState_Original();
}

const WindhawkUtils::SYMBOL_HOOK stobjectDllHooks[] =
{
    {
        { HOTPLUGSHOWNOTIFYICON },
        &HotPlugShowNotifyIcon_Original,
        HotPlugShowNotifyIcon_Hook,
        false
    },
    {
        { UPDATEHOTPLUGICONSTATE },
        &UpdateHotPlugIconState_Original,
        UpdateHotPlugIconState_Hook,
        false
    }
};

// Load settings
void LoadSettings()
{
    bool isTrayIconAlwaysHidden = wcscmp(WindhawkUtils::StringSetting::make(
        L"trayIconVisibilityState").get(), L"alwaysHidden") == 0;
    settings.trayIconVisibilityState.store(isTrayIconAlwaysHidden ? 0 : 1);
}

// Mod initialization
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    // Ensure the current Explorer process owns the taskbar
    HWND hTrayWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTrayWnd)
    {
        DWORD dwProcessId = 0;
        GetWindowThreadProcessId(hTrayWnd, &dwProcessId);
        if (dwProcessId != GetCurrentProcessId())
        {
            Wh_Log(
                L"Taskbar is owned by another Explorer process (PID: %lu), "
                L"skipping initialization",
                dwProcessId
            );
            return FALSE;
        }
    }

    HMODULE hStObject = LoadLibraryExW(L"stobject.dll", nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hStObject)
    {
        Wh_Log(L"Failed to load stobject.dll");
        return FALSE;
    }
    if (!WindhawkUtils::HookSymbols(hStObject, stobjectDllHooks,
            ARRAYSIZE(stobjectDllHooks)))
    {
        Wh_Log(L"Failed to hook symbols in stobject.dll");
        return FALSE;
    }

    if (HotPlugShowNotifyIcon_Original)
    {
        HotPlugShowNotifyIcon_Original(settings.trayIconVisibilityState.load());
    }

    return TRUE;
}

// Mod pre-uninitialization
void Wh_ModBeforeUninit()
{
    Wh_Log(L"BeforeUninit");

    g_unloading.store(true);

    if (UpdateHotPlugIconState_Original)
    {
        UpdateHotPlugIconState_Original();
    }
}

// Reload settings
void Wh_ModSettingsChanged()
{
    LoadSettings();

    if (HotPlugShowNotifyIcon_Original)
    {
        HotPlugShowNotifyIcon_Original(settings.trayIconVisibilityState.load());
    }
}
