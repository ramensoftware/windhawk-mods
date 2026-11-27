// ==WindhawkMod==
// @id              usb-tray-icon-visibility-override
// @name            Always Show/Hide USB Tray Icon
// @description     Always shows or hides the "Safely Remove Hardware and Eject Media" icon in the system tray
// @version         1.0
// @author          Kitsune
// @github          https://github.com/AromaKitsune
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Always Show/Hide USB Tray Icon
By default, the "Safely Remove Hardware and Eject Media" icon in the system tray
is shown when any removable devices are plugged into the PC, and completely
hidden when none are plugged in.

This mod allows you to override the default visibility of this tray icon: always
show it to keep the "Open Devices and Printers" menu item accessible, or always
hide it completely to declutter the system tray.

![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/usb-tray-icon-visibility-override.png) \
*USB tray icon always shown even when no removable devices are plugged in*

Looking to declutter the system tray even further? Check out these mods:

| Mod | Author | Description |
| :-- | :----- | :---------- |
| [Taskbar tray system icon tweaks](https://windhawk.net/mods/taskbar-tray-system-icon-tweaks) | m417z | Hides system icons: volume, network, battery, etc. (Windows 11 only) |
| [Disable Tray Icons](https://windhawk.net/mods/disable-tray-icons) | aubymori | Disables all tray icons for specific apps |
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- trayIconVisibility: alwaysShown
  $name: Tray icon visibility
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
    std::atomic<int> trayIconVisibility = 1;
} settings;

std::atomic<bool> g_unloading = false;

// Helper: Find the system tray window in the current process
HWND FindSystemTrayWindow()
{
    HWND hSysTrayWnd = FindWindowW(L"SystemTray_Main", nullptr);
    DWORD dwProcessId = 0;
    if (!hSysTrayWnd || !GetWindowThreadProcessId(hSysTrayWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId())
    {
        return nullptr;
    }
    return hSysTrayWnd;
}

// Helper: Run a callback on the target window's thread
using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);
bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc,
    PVOID procParam)
{
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM
    {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0)
    {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId())
    {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
        {
            if (nCode == HC_ACTION)
            {
                const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg)
                {
                    auto* param =
                        reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(
                            cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr,
        dwThreadId
    );
    if (!hook)
    {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessageW(hWnd, runFromWindowThreadRegisteredMsg, 0,
        reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);
    return true;
}

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
    HotPlugShowNotifyIcon_Original(settings.trayIconVisibility.load());
}

// Pointer for UpdateHotPlugIconState
using UpdateHotPlugIconState_t = void(__cdecl*)();
UpdateHotPlugIconState_t UpdateHotPlugIconState_Original;

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
        nullptr,
        false
    }
};

// Thread callback: Apply the configured tray icon visibility
void WINAPI ApplyTrayIconStateThreadProc(PVOID)
{
    if (HotPlugShowNotifyIcon_Original)
    {
        HotPlugShowNotifyIcon_Original(settings.trayIconVisibility.load());
    }
}

// Thread callback: Restore the default tray icon visibility
void WINAPI RestoreTrayIconStateThreadProc(PVOID)
{
    if (UpdateHotPlugIconState_Original)
    {
        UpdateHotPlugIconState_Original();
    }
}

// Load settings
void LoadSettings()
{
    bool isAlwaysHidden = wcscmp(WindhawkUtils::StringSetting::make(
        L"trayIconVisibility").get(), L"alwaysHidden") == 0;
    settings.trayIconVisibility.store(isAlwaysHidden ? 0 : 1);
}

// Mod initialization
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    // Ensure the current Explorer process owns the taskbar
    HWND hTaskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD dwProcessId = 0;
    if (hTaskbarWnd && GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId) &&
        dwProcessId != GetCurrentProcessId())
    {
        Wh_Log(
            L"Taskbar is owned by another Explorer process (PID: %lu), "
            L"skipping initialization",
            dwProcessId
        );
        return FALSE;
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

    return TRUE;
}

// Mod post-initialization
void Wh_ModAfterInit()
{
    Wh_Log(L"AfterInit");

    if (HWND hSysTrayWnd = FindSystemTrayWindow())
    {
        RunFromWindowThread(hSysTrayWnd, ApplyTrayIconStateThreadProc, nullptr);
    }
}

// Mod pre-uninitialization
void Wh_ModBeforeUninit()
{
    Wh_Log(L"BeforeUninit");

    g_unloading.store(true);

    if (HWND hSysTrayWnd = FindSystemTrayWindow())
    {
        RunFromWindowThread(hSysTrayWnd, RestoreTrayIconStateThreadProc,
            nullptr);
    }
}

// Reload settings
void Wh_ModSettingsChanged()
{
    LoadSettings();

    if (HWND hSysTrayWnd = FindSystemTrayWindow())
    {
        RunFromWindowThread(hSysTrayWnd, ApplyTrayIconStateThreadProc, nullptr);
    }
}
