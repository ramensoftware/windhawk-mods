// ==WindhawkMod==
// @id              taskbar-monitor-switcher
// @name            Taskbar Monitor Switcher
// @description     Move the primary taskbar (tray icons, notifications, action center) to any monitor via settings, click, or keyboard shortcut. Fork of taskbar-primary-on-secondary-monitor by m417z.
// @version         1.0.0
// @author          TheSakyo, m417z
// @github          https://github.com/TheSakyo
// @include         explorer.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -loleaut32 -lruntimeobject -lversion -lwtsapi32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// This is a fork of the original mod by m417z:
//   https://github.com/m417z/my-windhawk-mods
//
// Additions over the original:
//   - Keyboard shortcut to move the taskbar to the monitor under the cursor.
//
// For bug reports on the original logic, open an issue at:
//   https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Taskbar Monitor Switcher

> **Fork** of [Primary taskbar on secondary monitor](https://windhawk.net/mods/taskbar-primary-on-secondary-monitor)
> by **m417z**. All original features are preserved; a **keyboard shortcut** has been added.

Move the primary taskbar — including tray icons, notifications, action center,
etc. — to another monitor.

The active monitor can be switched in three ways:
- **Settings** — pick a monitor by number or interface name (all versions).
- **Click** — double-click or middle-click the taskbar's empty space (Windows 11 only).
- **Keyboard shortcut** — press a configurable hotkey to instantly move the
taskbar to whichever monitor the mouse cursor is on.

![Demonstration](https://i.imgur.com/hFU9oyK.gif)

## Selecting a monitor

### By monitor number

Set the **Monitor** setting to the desired monitor number (1, 2, 3 …). Note
that this number may differ from the number shown in Windows Display Settings.

### By interface name

If monitor numbers change frequently (e.g. after locking your PC or restarting),
use the monitor's interface name instead. To find it:

1. Go to the mod's **Advanced** tab.
2. Set **Debug logging** to **Mod logs**.
3. Click **Show log output**.
4. Enter any text (e.g. `TEST`) in the **Monitor interface name** field and save.
5. In the log, look for lines like:
```
Found display device \\.\DISPLAY1, interface name: \\?\DISPLAY#DELA1D2#5&abc123#0#{e6f07b5f-…}
Found display device \\.\DISPLAY2, interface name: \\?\DISPLAY#GSM5B09#4&def456#0#{e6f07b5f-…}
```
6. Copy the relevant interface name (or a unique substring) into the
**Monitor interface name** setting.
7. Set **Debug logging** back to **None** when done.

**Monitor interface name** takes priority over the **Monitor** number when both
are set.

## Keyboard shortcut

When a keyboard shortcut is configured, pressing it moves the primary taskbar to
the monitor under the mouse cursor. If the cursor is **already on the taskbar's
monitor**, the shortcut is silently ignored — no unintended jumps.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- monitor: 2
  $name: Monitor
  $description: >-
    The monitor number to have the primary taskbar on.
- monitorInterfaceName: ""
  $name: Monitor interface name
  $description: >-
    If not empty, the given monitor interface name (or a unique substring) is
    used instead of the monitor number. Useful when monitor numbers change
    often. Enable mod logs and look for "Found display device" messages to
    discover available names.
- clickToSwitchMonitor: disabled
  $name: Click taskbar to switch monitor
  $description: >-
    How clicking on the taskbar's empty space switches the primary taskbar to
    the clicked monitor (Windows 11 only).
  $options:
    - disabled: Disabled
    - doubleClick: Double click
    - middleClick: Middle click
- keyboardShortcut: disabled
  $name: Keyboard shortcut
  $description: >-
    Shortcut that moves the primary taskbar to the monitor where the mouse
    cursor currently is. Does nothing when the cursor is already on the
    taskbar's monitor.
  $options:
    - disabled: Disabled
    - win_shift_f: Win + Shift + F
    - win_shift_x: Win + Shift + X
    - win_alt_x: Win + Alt + X
    - win_shift_comma: Win + Shift + <
    - ctrl_alt_comma: Ctrl + Alt + <
- moveAdditionalElements: false
  $name: Move additional elements
  $description: >-
    Move additional elements such as desktop icons to the target monitor.
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable when using ExplorerPatcher or a similar tool that restores the
    Windows 10 taskbar on Windows 11.
*/
// ==/WindhawkModSettings==

// ============================================================================
// Block: Includes
// Purpose: Import Windhawk utilities, Windows APIs, WinRT headers, and STL utilities.
// ============================================================================

#include <windhawk_utils.h>                  // <= Include Windhawk utility helpers for symbol hooking and settings.
#include <psapi.h>                           // <= Include PSAPI for process and module enumeration.
#include <windowsx.h>                        // <= Include Windowsx macros for Windows message handling helpers.
#include <wtsapi32.h>                        // <= Include WTS API for session information (lock state, etc.).
#undef GetCurrentTime                        // <= Undefine GetCurrentTime macro to avoid conflicts with other APIs.
#include <winrt/Windows.UI.Input.h>          // <= Include WinRT input APIs for pointer events.
#include <winrt/Windows.UI.Xaml.Controls.h>  // <= Include WinRT XAML controls for taskbar frame handling.
#include <winrt/Windows.UI.Xaml.Input.h>     // <= Include WinRT XAML input APIs for pointer routed events.
#include <atomic>                            // <= Include <atomic> for thread-safe atomic flags and state tracking.

// ============================================================================
// Enums
// ============================================================================

/*
 * Enum describing how clicking the taskbar can switch monitors.
 */
enum class ClickToSwitchMonitor {
    disabled,     // <= No click-to-switch behavior is enabled.
    doubleClick,  // <= Double-click on the taskbar triggers monitor switch.
    middleClick,  // <= Middle-click on the taskbar triggers monitor switch.
};

/*
 * Enum describing which host process the mod is running in.
 */
enum class Target {
    Explorer,     // <= The mod is running inside explorer.exe.
    ShellHost,    // <= The mod is running inside ShellHost.exe (Windows 11 24H2).
};

/*
 * Enum describing the effective Windows version for taskbar logic.
 */
enum class WinVersion {
    Unsupported,  // <= Windows version is unsupported by this mod.
    Win10,        // <= Windows 10 behavior.
    Win11,        // <= Windows 11 behavior (pre-24H2).
    Win11_24H2,   // <= Windows 11 24H2 and later behavior.
};

// ============================================================================
// Types
// ============================================================================

/*
 * Struct mirroring a WinRT Rect with float coordinates and size.
 */
struct WinrtRect { 
    float X, Y, Width, Height;                         // <= X/Y position and Width/Height of the rectangle.
};

/*
 * Struct holding all configurable mod settings.
 */
struct Settings {
    int monitor;                                       // <= Target monitor number (1-based index) for the primary taskbar.
    WindhawkUtils::StringSetting monitorInterfaceName; // <= Monitor interface name or substring for stable targeting.
    ClickToSwitchMonitor clickToSwitchMonitor;         // <= Click behavior for switching monitors.
    UINT hotkeyModifiers;                              // <= Modifier keys for the configured hotkey (e.g., Win, Shift).
    UINT hotkeyVk;                                     // <= Virtual-key code for the configured hotkey.
    bool moveAdditionalElements;                       // <= Whether to move additional desktop elements (icons, etc.).
    bool oldTaskbarOnWin11;                            // <= Whether to treat Windows 11 as Windows 10 taskbar (ExplorerPatcher).
};

// ============================================================================
// Globals
// ============================================================================

Settings g_settings;                                          // <= Global instance of Settings storing current configuration.

Target    g_target;                                           // <= Global variable indicating which process (Explorer/ShellHost) we are in.
WinVersion g_winVersion;                                      // <= Global variable describing the effective Windows version.

std::atomic<bool> g_taskbarViewDllLoaded;                     // <= Tracks whether Taskbar.View.dll (or equivalent) has been hooked.
std::atomic<bool> g_initialized;                              // <= Tracks whether the mod has completed initialization.
std::atomic<bool> g_explorerPatcherInitialized;               // <= Tracks whether ExplorerPatcher-related hooks have been set up.
std::atomic<bool> g_unloading;                                // <= Indicates that the mod is in the process of unloading.
std::atomic<bool> g_lastIsSessionLocked;                      // <= Stores the last known session lock state for change detection.

#define SHARED_SECTION __attribute__((section(".shared")))    // <= Macro to place variables into a shared section.
asm(".section .shared,\"dws\"\n");                            // <= Assembler directive to define the shared section.
volatile HMONITOR g_overrideMonitor SHARED_SECTION = nullptr; // <= Shared override monitor used to force taskbar monitor selection.

DWORD    g_lastPressTime    = 0;                              // <= Timestamp of the last relevant click for double-click detection.
HMONITOR g_lastPressMonitor = nullptr;                        // <= Monitor associated with the last click for double-click detection.

DWORD  g_hotkeyThreadId     = 0;                              // <= Thread ID of the hotkey listener thread.
HANDLE g_hotkeyThreadHandle = nullptr;                        // <= Handle to the hotkey listener thread.

constexpr UINT WM_APP_REREGISTER_HOTKEY = WM_APP + 1;         // <= Application-defined message used to re-register the hotkey.

// ============================================================================
// Function-pointer typedefs
// ============================================================================

// Typedef for pointer to MonitorFromPoint API.
using MonitorFromPoint_t = decltype(&MonitorFromPoint); 

// Global pointer to store original MonitorFromPoint.
MonitorFromPoint_t MonitorFromPoint_Original;           

////////////////////////////////////////////////////////////////////

// Typedef for pointer to MonitorFromRect API.
using MonitorFromRect_t = decltype(&MonitorFromRect);

// Global pointer to store original MonitorFromRect.
MonitorFromRect_t MonitorFromRect_Original;            

////////////////////////////////////////////////////////////////////

// Typedef for pointer to EnumDisplayDevicesW API.
using EnumDisplayDevicesW_t = decltype(&EnumDisplayDevicesW); 

// Global pointer to store original EnumDisplayDevicesW.
EnumDisplayDevicesW_t EnumDisplayDevicesW_Original;           

////////////////////////////////////////////////////////////////////

// Typedef for pointer to LoadLibraryExW API.
using LoadLibraryExW_t = decltype(&LoadLibraryExW);

// Global pointer to store original LoadLibraryExW.
LoadLibraryExW_t LoadLibraryExW_Original;

////////////////////////////////////////////////////////////////////

// Typedef for pointer to HardwareConfirmatorHost::GetPositionRect.
using HardwareConfirmatorHost_GetPositionRect_t = WinrtRect*(WINAPI*)(void*, WinrtRect*, const WinrtRect*);

// Global pointer to store original GetPositionRect implementation.
HardwareConfirmatorHost_GetPositionRect_t HardwareConfirmatorHost_GetPositionRect_Original;

////////////////////////////////////////////////////////////////////

// Typedef for pointer to TaskbarFrame::OnPointerPressed.
using TaskbarFrame_OnPointerPressed_t = int(WINAPI*)(void*, void*);

// Global pointer to store original OnPointerPressed.
TaskbarFrame_OnPointerPressed_t TaskbarFrame_OnPointerPressed_Original;

////////////////////////////////////////////////////////////////////

// Typedef for pointer to TrayUI::_SetStuckMonitor.
using TrayUI__SetStuckMonitor_t = HRESULT(WINAPI*)(void*, HMONITOR);

// Global pointer to store original _SetStuckMonitor.
TrayUI__SetStuckMonitor_t TrayUI__SetStuckMonitor_Original;

// ============================================================================
// Forward declarations
// ============================================================================

// Forward declaration: applies current monitor-related settings to the taskbar.
void ApplySettings();                            

// Forward declaration: handles ExplorerPatcher module detection and hooking.
void HandleLoadedModuleIfExplorerPatcher(HMODULE module);            

// Forward declaration: handles Taskbar.View-related module loading.
void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName); 

// ============================================================================
// Utility
// ============================================================================

/**
 * @brief Find the Shell_TrayWnd window belonging to the current process.
 *
 * This function enumerates all top-level windows and returns the handle of the
 * taskbar window (class "Shell_TrayWnd") that belongs to the current process.
 *
 * @return HWND Handle to the current process's taskbar window, or nullptr if not found.
 */
HWND FindCurrentProcessTaskbarWnd() { 

    // Initialize the result handle to nullptr.
    HWND hTaskbarWnd = nullptr;       

    /*
     * Enumerate all top-level windows in the system.
     */
    EnumWindows(                     

        /*
         * Lambda callback invoked for each window. 
         */
        [](HWND hWnd, LPARAM lParam) -> BOOL { 
            
            // Variable to store the process ID owning the window.
            DWORD dwProcessId;        

            // Buffer to store the window class name.
            WCHAR className[32];      

            /*
             * Check if the window belongs to the current process and matches the Taskbar class
             */
            if(GetWindowThreadProcessId(hWnd, &dwProcessId) && dwProcessId == GetCurrentProcessId()) {

                /*
                 * Retrieve the class name to verify if it's the specific shell window
                 */
                if(GetClassName(hWnd, className, ARRAYSIZE(className)) && _wcsicmp(className, L"Shell_TrayWnd") == 0) {

                    // Cast lParam back to an HWND pointer and store the found window handle
                    *reinterpret_cast<HWND*>(lParam) = hWnd;

                    // Return FALSE to terminate the EnumWindows/EnumThreadWindows callback
                    return FALSE;
                }
            }

            // Otheriwse, Return TRUE to continue enumeration for other windows.
            return TRUE;
        },

        // Pass the address of hTaskbarWnd as lParam to the callback.
        reinterpret_cast<LPARAM>(&hTaskbarWnd));                

    // Return the found taskbar window handle (or nullptr).
    return hTaskbarWnd;                                         
}

/**
 * @brief Check whether the current user session is locked.
 *
 * Uses WTSQuerySessionInformation with WTSSessionInfoEx to determine whether
 * the current session is in the locked state.
 *
 * @return true if the session is locked, false otherwise or on failure.
 */
bool IsSessionLocked() {                   

    // Pointer to receive extended session information.
    WTSINFOEX* sessionInfoEx = nullptr;                 

    // Variable to receive the size of the returned data.        
    DWORD bytesReturned = 0;                                    

    /* 
     * Query extended information for the current session to determine its status.
     * This fills the sessionInfoEx structure using the Remote Desktop Services API.
     */
    if(!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION, WTSSessionInfoEx, reinterpret_cast<LPWSTR*>(&sessionInfoEx), &bytesReturned))  {

        // Log the error code if the session query fails
        Wh_Log(L"WTSQuerySessionInformation failed: %d", GetLastError());
        
        // Terminate execution as session status cannot be determined
        return false;
    }

    // Determine if the session is locked by verifying the information level and session flags
    bool locked = (sessionInfoEx->Level == 1) && (sessionInfoEx->Data.WTSInfoExLevel1.SessionFlags == WTS_SESSIONSTATE_LOCK);

    // Free the memory allocated by WTSQuerySessionInformation.
    WTSFreeMemory(sessionInfoEx);                               

    // Return whether the session is locked.
    return locked;                                              
}

/**
 * @brief Retrieve version information for a given module.
 *
 * This function locates the VS_VERSION_INFO resource in the specified module
 * and extracts the VS_FIXEDFILEINFO structure from it.
 *
 * @param hModule Handle to the module whose version info is requested (nullptr for current module).
 * @param puPtrLen Optional pointer to receive the size of the VS_FIXEDFILEINFO structure.
 * @return VS_FIXEDFILEINFO* Pointer to the version info structure, or nullptr on failure.
 */
VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {

    // Pointer to hold the fixed file info structure retrieved from resources
    void* pFixedFileInfo = nullptr;

    // Variable to store the size of the retrieved version information
    UINT uPtrLen = 0;

    // Locate the version resource (VS_VERSION_INFO) within the specified module
    HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);

    /*
     * Proceed if the version resource handle was successfully located
     */ 
    if(hResource) {

        // Load the resource into memory to prepare for data access
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        
        /*
         * Check if the resource was successfully loaded into global memory
         */
        if(hGlobal) {

            // Obtain a raw pointer to the resource data in memory
            void* pData = LockResource(hGlobal);
            
            /*
             * Ensure the data pointer is valid before proceeding
             */ 
            if(pData) {

                /*
                 * Query the root block for the VS_FIXEDFILEINFO structure  
                 */
                if(!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) || uPtrLen == 0) {

                    // Reset the pointer to indicate that the info was not found
                    pFixedFileInfo = nullptr;

                    // Set the length to zero as no valid data was returned
                    uPtrLen = 0;
                }
            }
        }
    }

    // If the caller provided a length pointer, store the retrieved size
    if(puPtrLen) *puPtrLen = uPtrLen;

    // Return the pointer to the fixed file info (or nullptr if not found)
    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

/**
 * @brief Determine the effective Windows version for Explorer.
 *
 * Uses the version information of the current module (Explorer) to classify
 * the environment as Windows 10, Windows 11, or Windows 11 24H2+.
 *
 * @return WinVersion Enum value representing the detected Windows version.
 */
WinVersion GetExplorerVersion() {

    // Get version info for the current module (Explorer)
    VS_FIXEDFILEINFO* fi = GetModuleVersionInfo(nullptr, nullptr);

    // If version info is unavailable, mark as unsupported
    if(!fi) return WinVersion::Unsupported;

    // Extract the major version from the high word of MS
    WORD major = HIWORD(fi->dwFileVersionMS);

    // Extract the minor version from the low word of MS
    WORD minor = LOWORD(fi->dwFileVersionMS);

    // Extract the build number from the high word of LS
    WORD build = HIWORD(fi->dwFileVersionLS);

    // Extract the QFE (revision) from the low word of LS
    WORD qfe = LOWORD(fi->dwFileVersionLS);

    // Log the full version for debugging purposes
    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    /*
     * Validate if the major version corresponds to Windows 10 or 11
     */
    if(major==10) {

        // Return Windows 10 for builds below 22000 if the build number is below the Windows 11 threshold
        if(build<22000)  return WinVersion::Win10; 

        // Return Windows 11 for builds between 22000 and 26099, if the build number is below the 24H2 release threshold
        if(build<26100) return WinVersion::Win11;

        // Return Windows 11 24H2 for all builds 26100 and above
        return WinVersion::Win11_24H2;
    }

    // Return unsupported if the major version is not 10
    return WinVersion::Unsupported;
}

// ============================================================================
// Monitor resolution helpers
// ============================================================================

/**
 * @brief Get a monitor handle by its zero-based index.
 *
 * Enumerates all display monitors and returns the monitor whose index matches
 * the specified monitorId (0-based).
 *
 * @param monitorId Zero-based index of the monitor to retrieve.
 * @return HMONITOR Handle to the requested monitor, or nullptr if not found.
 */
HMONITOR GetMonitorById(int monitorId) {

    // Initialize result to nullptr (no monitor found yet)
    HMONITOR result = nullptr;

    // Counter to track the current monitor index during enumeration
    int current = 0;

    /*
     * Lambda used as a callback to process each monitor
     */
    auto proc = [&](HMONITOR hMonitor) -> BOOL {

        // Compare the current loop index with the target ID
        bool isTargetMonitor = (current == monitorId);

        /*
         * Check if the current index matches the requested monitorId
         */
        if(isTargetMonitor) {

            // Store the matching monitor handle
            result = hMonitor;

            // Stop enumeration since we found the desired monitor
            return FALSE;
        }

        // Increment the current index for the next monitor
        ++current;

        // Continue enumeration
        return TRUE;
    };

    /*
     * Enumerate all display monitors
     */
    EnumDisplayMonitors(
        
        // No clipping rectangle; enumerate all monitors
        nullptr, 
        
        // No specific device context; use the entire virtual screen
        nullptr,
      
        /*
         * Static callback that forwards to the lambda
         */
        [](HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) -> BOOL {

            // Invoke the lambda stored in lParam
            return (*reinterpret_cast<decltype(proc)*>(lParam))(hMonitor);
        },

        // Pass the address of the lambda as lParam
        reinterpret_cast<LPARAM>(&proc)
    );

    // Return the found monitor handle (or nullptr)
    return result;
}

/**
 * @brief Get a monitor handle by matching a substring of its interface name.
 *
 * Enumerates all monitors, queries their interface names via EnumDisplayDevicesW,
 * and returns the first monitor whose interface name contains the given substring.
 *
 * @param substr Substring to search for within the monitor interface name.
 * @return HMONITOR Handle to the matching monitor, or nullptr if none match.
 */
HMONITOR GetMonitorByInterfaceNameSubstr(PCWSTR substr) {

    // Initialize result to nullptr (no monitor found yet)
    HMONITOR result = nullptr;

    // Lambda used as a callback for each monitor
    auto proc = [&](HMONITOR hMonitor) -> BOOL {

        // Structure to receive monitor information
        MONITORINFOEX mi = {};
        
        // Set the size of the structure before calling GetMonitorInfo
        mi.cbSize = sizeof(mi);

        // Check if the monitor info retrieval fails
        bool infoFailed = !GetMonitorInfo(hMonitor, &mi);

        // If monitor info retrieval fails, continue enumeration
        if(infoFailed) return TRUE;

        // DISPLAY_DEVICE structure initialized with its size
        DISPLAY_DEVICE dd = { .cb = sizeof(dd) };

        // Call the original EnumDisplayDevicesW for this monitor
        bool enumFailed = !EnumDisplayDevicesW_Original(mi.szDevice, 0, &dd, EDD_GET_DEVICE_INTERFACE_NAME);

        // If enumeration fails, continue to the next monitor
        if(enumFailed) return TRUE;

        // Log the device and its interface name for debugging
        Wh_Log(L"Found display device %s, interface name: %s", mi.szDevice, dd.DeviceID);

        // Check if the interface name contains the requested substring
        bool isMatch = wcsstr(dd.DeviceID, substr) != nullptr;

        /*
         * Check if the interface name contains the requested substring
         */
        if(isMatch) {

            // Log that a matching device was found
            Wh_Log(L"Matched display device");
            
            // Store the matching monitor handle
            result = hMonitor;
            
            // Stop enumeration since we found a match
            return FALSE;
        }

        // Continue enumeration if no match
        return TRUE;
    };

    /*
     * Enumerate all display monitors
     */
    EnumDisplayMonitors(

        // No clipping rectangle; enumerate all monitors
        nullptr,

        // No specific device context; use the entire virtual screen
        nullptr,

        /*
         * Static callback that forwards to the lambda
         */
        [](HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) -> BOOL {

            // Invoke the lambda stored in lParam
            return (*reinterpret_cast<decltype(proc)*>(lParam))(hMonitor);
        },

        // Pass the address of the lambda as lParam
        reinterpret_cast<LPARAM>(&proc)
    );

    // Return the found monitor handle (or nullptr)
    return result;
}

// ============================================================================
// Core logic
// ============================================================================

/**
 * @brief Broadcast a shell hook message indicating a display change.
 *
 * This triggers CImmersiveMonitorManager::_HandleDisplayChangeMessage in
 * twinui.dll, ensuring that the system reacts to display changes such as
 * monitor reconfiguration or lock screen placement.
 */
void BroadcastShellHookDisplayChange() {

    // Register the "SHELLHOOK" window message and get its ID
    UINT msg = RegisterWindowMessage(L"SHELLHOOK");

    // Broadcast the message with wParam=35 to all top-level windows
    PostMessage(HWND_BROADCAST, msg, 35, 0);
}

/**
 * @brief Parameters controlling how GetTargetMonitor behaves.
 *
 * retAddress is used to detect the caller module for certain behaviors,
 * and ignoreLockedState allows bypassing the session lock check.
 */
struct GetTargetMonitorParams {

    // Return address of the caller, used to detect shell32 calls
    void* retAddress = nullptr;

    // Whether to ignore the session locked state when choosing a monitor
    bool ignoreLockedState = false;
};

/**
 * @brief Determine the target monitor for the primary taskbar.
 *
 * This function encapsulates the logic for selecting which monitor should host
 * the primary taskbar. It considers:
 * - Session lock state and changes (to notify the system when needed).
 * - Whether additional elements should be moved.
 * - An override monitor set by click-to-switch or keyboard shortcut.
 * - The configured monitor interface name or monitor index.
 *
 * @param params Optional parameters controlling behavior (caller address, lock ignore).
 * @return HMONITOR Handle to the target monitor, or nullptr if no override is applied.
 */
HMONITOR GetTargetMonitor(GetTargetMonitorParams params = {}) {

    // If the mod is unloading, do not change monitors and return nullptr
    if(g_unloading) return nullptr;

    // Query the current session lock state
    bool sessionLocked = IsSessionLocked();

    // Detect if the lock state changed since last check
    bool lockStateChanged = g_lastIsSessionLocked.exchange(sessionLocked)!=sessionLocked;

    // Check if the lock state change requires a system notification
    bool shouldNotify = lockStateChanged && g_target==Target::Explorer && FindCurrentProcessTaskbarWnd();

    /*
     * If lock state changed and we are in Explorer with a valid taskbar
     */
    if(shouldNotify) {

        // Log the new lock state for debugging
        Wh_Log(L"Session lock state changed: %s", sessionLocked ? L"locked" : L"unlocked");

        // Broadcast a display change so the lock screen is placed correctly
        BroadcastShellHookDisplayChange();
    }

    // Determine if the current lock state should block the override
    bool isLockedAndRespected = !params.ignoreLockedState && sessionLocked;

    // Do not override the monitor while locked
    if(isLockedAndRespected) return nullptr;

    /*
     * Handle cases where additional elements should not be moved
     */
    if(!g_settings.moveAdditionalElements && params.retAddress) {

        // Get a handle to shell32.dll to compare against the caller
        HMODULE shell32 = GetModuleHandle(L"shell32.dll");

        /*
         * If shell32.dll is loaded, check the caller's module
         */
        if(shell32) {

            // Variable to receive the module of the caller
            HMODULE callerModule;

            // Get the module handle from the return address
            bool gotModule = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (PCWSTR)params.retAddress, &callerModule);

            // If the caller is shell32.dll, do not override the monitor
            if(gotModule && callerModule==shell32) return nullptr;
        }
    }

    // Start with the override monitor if one is set (click-to-switch or shortcut)
    HMONITOR monitor = g_overrideMonitor;

    /*
     * If no manual override is set, check the configuration settings
     */
    if(!monitor) {

        // Check if a monitor interface name is configured
        bool hasInterfaceName = *g_settings.monitorInterfaceName.get() != L'\0';

        // Resolve the monitor by interface name
        if(hasInterfaceName) monitor = GetMonitorByInterfaceNameSubstr(g_settings.monitorInterfaceName.get());
        
        // Otherwise, resolve the monitor by its numeric index
        else if(g_settings.monitor>=1) monitor = GetMonitorById(g_settings.monitor - 1);
    }

    // Return the chosen monitor (or nullptr if none)
    return monitor;
}

/**
 * @brief Get the monitor currently hosting the primary taskbar.
 *
 * This function returns the monitor that should be considered as hosting the
 * primary taskbar, falling back to the OS primary monitor when no override
 * or configuration is active.
 *
 * @return HMONITOR Handle to the current taskbar monitor.
 */
HMONITOR GetCurrentTaskbarMonitor() {

    // Get the target monitor ignoring lock state
    HMONITOR monitor = GetTargetMonitor({.ignoreLockedState = true});

    // If a target monitor is defined, return it
    if(monitor) return monitor;

    // Otherwise, fall back to the OS primary monitor
    return MonitorFromPoint_Original({0, 0}, MONITOR_DEFAULTTOPRIMARY);
}

// ============================================================================
// Keyboard shortcut — move taskbar to monitor under cursor
// ============================================================================

/**
 * @brief Moves the primary taskbar to the monitor where the mouse cursor currently is.
 */
void SwitchToMouseMonitor() {

    // Abort if the mod is currently in the process of unloading
    if(g_unloading) return;

    // Define a point structure to store the current cursor coordinates
    POINT pt;

    // Retrieve the current position of the mouse cursor on the screen
    GetCursorPos(&pt);

    // Identify the monitor handle located at the current cursor position
    HMONITOR mouseMonitor = MonitorFromPoint_Original(pt, MONITOR_DEFAULTTONEAREST);

    // Ensure a valid monitor handle was retrieved before proceeding
    if(!mouseMonitor) return;

    // Get the handle of the monitor where the taskbar is currently located
    HMONITOR currentMonitor = GetCurrentTaskbarMonitor();

    /*
     * Silently ignore the request if the cursor is already on the taskbar's monitor
     */
    if (mouseMonitor == currentMonitor) {

        // Log the skip operation to indicate no move was necessary
        Wh_Log(L"SwitchToMouseMonitor: cursor already on taskbar monitor, skip");

        // Exit the function without making changes
        return;
    }

    // Log the successful trigger of the monitor switch
    Wh_Log(L"SwitchToMouseMonitor: switching to cursor monitor");

    // Update the global override variable with the new target monitor handle
    g_overrideMonitor = mouseMonitor;

    // Apply the updated monitor settings to move the taskbar
    ApplySettings();
}

/**
 * @brief Thread procedure that listens for the configured hotkey.
 *
 * This function:
 * - Creates a message-only window.
 * - Registers the configured hotkey (if any).
 * - Runs a message loop to handle WM_HOTKEY and custom re-register messages.
 *
 * @param Unused LPVOID parameter required by the thread signature.
 * @return DWORD Exit code of the thread (0 on normal exit, 1 on failure).
 */
DWORD WINAPI HotkeyThreadProc(LPVOID) {

    // Create a hidden message-only window
    HWND hwnd = CreateWindowEx(0, L"STATIC", L"WH_HotkeyListener", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr);

    /*
     * Check if the message-only window was successfully created
     */
    if(!hwnd) {

        // Log the error code if window creation failed
        Wh_Log(L"HotkeyThreadProc: CreateWindowEx failed: %d", GetLastError());
        
        // Return non-zero to indicate failure
        return 1;
    }

    /*
     * Lambda to (re)register the configured hotkey
     */
    auto registerHotkey = [hwnd]() {

        // Unregister any previously registered hotkey with ID 1
        UnregisterHotKey(hwnd, 1);
        
        /*
         * Check if both modifiers and virtual key are configured in settings
         */
        if(g_settings.hotkeyModifiers && g_settings.hotkeyVk) {

            // Attempt to register the hotkey with the system using the configured modifiers and virtual key
            if(!RegisterHotKey(hwnd, 1, g_settings.hotkeyModifiers, g_settings.hotkeyVk)) Wh_Log(L"RegisterHotKey failed: %d", GetLastError());

            // If registration succeeds, log the specific modifier and key codes to confirm the active hotkey
            else Wh_Log(L"Hotkey registered (mods=%u vk=%u)", g_settings.hotkeyModifiers, g_settings.hotkeyVk);
    
        // Handle cases where the hotkey settings are empty or invalid by logging that no shortcut will be active
        } else Wh_Log(L"No hotkey configured");
    };

    // Perform initial hotkey registration
    registerHotkey();

    // MSG structure to receive messages from the message queue
    MSG msg;

    /*
     * Standard message loop: retrieve messages until WM_QUIT
     */
    while(GetMessage(&msg, nullptr, 0, 0)>0) {

        /*
        * Check if the message is our registered hotkey with ID 1
        */
        if(msg.message==WM_HOTKEY && msg.wParam==1) {

            // Log that the hotkey was triggered
            Wh_Log(L"Hotkey triggered");

            // Switch the taskbar to the monitor currently under the mouse cursor
            SwitchToMouseMonitor();

        // Check if we received a custom application request to re-register the hotkey
        } else if(msg.message==WM_APP_REREGISTER_HOTKEY) registerHotkey();

        /*
         * Handle all other standard system messages by processing them normally
         */
        else {

            // Translate virtual-key messages into character messages for the system
            TranslateMessage(&msg);

            // Dispatch the message to the appropriate window procedure for execution
            DispatchMessage(&msg);
        }
    }

    // Unregister the hotkey before exiting the thread
    UnregisterHotKey(hwnd, 1);
    
    // Destroy the message-only window
    DestroyWindow(hwnd);
    
    // Log that the hotkey thread is exiting
    Wh_Log(L"Hotkey thread exiting");
    
    // Return 0 to indicate normal termination
    return 0;
}

/**
 * @brief Start the hotkey listener thread if it is not already running.
 */
void StartHotkeyThread() {

    // Exit if the hotkey thread is already running
    if(g_hotkeyThreadHandle) return;

    // Create the hotkey listener thread
    g_hotkeyThreadHandle = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0, nullptr);

    /*
     * Check if the thread creation was successful by verifying the handle
     */
    if(g_hotkeyThreadHandle) {

        // Store the unique thread identifier required for posting messages
        g_hotkeyThreadId = GetThreadId(g_hotkeyThreadHandle);

        // Log the successful initialization of the hotkey monitoring thread
        Wh_Log(L"Hotkey thread started (id=%lu)", g_hotkeyThreadId);

    // Handle cases where CreateThread failed to initialize the background process
    } else Wh_Log(L"StartHotkeyThread: CreateThread failed: %d", GetLastError());
}

/**
 * @brief Stop the hotkey listener thread if it is running.
 */
void StopHotkeyThread() {

    // Exit if the hotkey thread is not running
    if(!g_hotkeyThreadHandle) return;

    // Post WM_QUIT to the thread's message queue to stop the loop
    PostThreadMessage(g_hotkeyThreadId, WM_QUIT, 0, 0);
    
    // Wait up to 5 seconds for the thread to exit gracefully
    WaitForSingleObject(g_hotkeyThreadHandle, 5000);
    
    // Close the thread handle
    CloseHandle(g_hotkeyThreadHandle);
    
    // Reset the thread handle to nullptr
    g_hotkeyThreadHandle = nullptr;
    
    // Reset the stored thread ID
    g_hotkeyThreadId = 0;
    
    // Log that the hotkey thread has been stopped
    Wh_Log(L"Hotkey thread stopped");
}

/**
 * @brief Request the hotkey thread to re-register the hotkey with updated settings.
 */
void ReregisterHotkey() {

    // Determine if the background thread identifier is valid
    bool isThreadActive = (g_hotkeyThreadId != 0);

    // Check if the hotkey thread is active before attempting to post a message
    if(isThreadActive) PostThreadMessage(g_hotkeyThreadId, WM_APP_REREGISTER_HOTKEY, 0, 0);
}

// ============================================================================
// Hooks
// ============================================================================

/**
 * @brief Hook for MonitorFromPoint to override the primary monitor.
 *
 * When Explorer calls MonitorFromPoint({0,0}), it is asking:
 * “Which monitor is the primary one?”
 *
 * This hook intercepts that specific call and returns the mod-selected monitor.
 *
 * @param pt The point being queried.
 * @param dwFlags Flags controlling monitor selection behavior.
 * @return HMONITOR The overridden monitor or the original result.
 */
HMONITOR WINAPI MonitorFromPoint_Hook(POINT pt, DWORD dwFlags) {

    // Check if the query is specifically for the virtual origin point (0,0)
    bool isPrimaryQuery = (pt.x == 0 && pt.y == 0);

    // For any point other than (0,0), use the original system behavior
    if(!isPrimaryQuery) return MonitorFromPoint_Original(pt, dwFlags);

    // Log hook activation for debugging purposes
    Wh_Log(L">");

    // Initialize the target monitor by calling our custom resolution logic
    HMONITOR monitor = GetTargetMonitor({.retAddress = __builtin_return_address(0)});

    // Return the overridden monitor handle if available, otherwise fall back to original
    return monitor ? monitor : MonitorFromPoint_Original(pt, dwFlags);
}

/**
 * @brief Hook for MonitorFromRect to override primary monitor detection.
 *
 * Explorer sometimes queries the primary monitor using a zero rectangle.
 *
 * @param lprc Rectangle being queried.
 * @param dwFlags Flags controlling monitor selection.
 * @return HMONITOR The overridden monitor or the original result.
 */
HMONITOR WINAPI MonitorFromRect_Hook(LPCRECT lprc, DWORD dwFlags) {

    // Verify if the pointer is valid and if all rectangle coordinates are zero
    bool isZeroRect = (lprc && lprc->left == 0 && lprc->top == 0 && lprc->right == 0 && lprc->bottom == 0);

    // If the rectangle is invalid or not the zero rect, use the original system function
    if(!isZeroRect) return MonitorFromRect_Original(lprc, dwFlags);

    // Log hook activation for debugging purposes
    Wh_Log(L">");

    // Retrieve the target monitor based on the caller's address for this rect query
    HMONITOR monitor = GetTargetMonitor({.retAddress = __builtin_return_address(0)});

    // Return the mod-selected monitor or the original system result as a fallback
    return monitor ? monitor : MonitorFromRect_Original(lprc, dwFlags);
}

/**
 * @brief Hook for EnumDisplayDevicesW to fake which monitor is primary.
 *
 * Explorer enumerates monitors and checks the DISPLAY_DEVICE_PRIMARY_DEVICE flag.
 * This hook rewrites that flag so Explorer believes the chosen monitor is primary.
 *
 * @param lpDevice Device name or nullptr for top-level enumeration.
 * @param iDevNum Device index.
 * @param lpDisplayDevice Output structure.
 * @param dwFlags Flags controlling enumeration.
 * @return BOOL TRUE on success, FALSE on failure.
 */
BOOL WINAPI EnumDisplayDevicesW_Hook(LPCWSTR lpDevice, DWORD iDevNum, PDISPLAY_DEVICEW lpDisplayDevice, DWORD dwFlags) {

    // Execute the original enumeration to populate the display device data structure
    BOOL result = EnumDisplayDevicesW_Original(lpDevice, iDevNum, lpDisplayDevice, dwFlags);

    // Ensure the call succeeded, the output pointer is valid, and we are at the top level
    bool isTopLevel = (result && lpDisplayDevice && !lpDevice);

    // Only modify flags during top-level device enumeration to avoid recursive issues
    if(!isTopLevel) return result;

    // Log hook activation for debugging purposes
    Wh_Log(L">");

    // Get the target override monitor using the current context
    HMONITOR monitor = GetTargetMonitor({.retAddress = __builtin_return_address(0)});

    // If no override is active, proceed with the original result immediately
    if(!monitor) return result;

    // Prepare a structure to retrieve detailed information for the target monitor
    MONITORINFOEX mi = {};
    
    // Initialize the size of the structure before calling the API
    mi.cbSize = sizeof(mi);

    // Continue with original result if monitor information cannot be retrieved from the handle
    if(!GetMonitorInfo(monitor, &mi)) return result;

    // Compare the current enumerated device name with our target monitor device name
    bool isTargetDevice = (wcscmp(lpDisplayDevice->DeviceName, mi.szDevice) == 0);

    /*
     * Adjust the primary device flag to deceive Explorer's primary monitor detection
     */
    if(isTargetDevice) {

        // Explicitly set the primary flag for our selected monitor
        lpDisplayDevice->StateFlags |= DISPLAY_DEVICE_PRIMARY_DEVICE;

    /*
     * Ensure that all other monitors are not marked as primary to maintain exclusivity
     */
    } else {

        // Remove the primary flag from all other monitors in the enumeration
        lpDisplayDevice->StateFlags &= ~DISPLAY_DEVICE_PRIMARY_DEVICE;
    }

    // Return the result of the original function call
    return result;
}

/**
 * @brief Hook for HardwareConfirmatorHost::GetPositionRect.
 *
 * The original function expects coordinates relative to (0,0). We temporarily
 * shift the rect to (0,0), call the original, then shift the result back.
 *
 * @param pThis COM instance pointer.
 * @param retval Output rectangle.
 * @param rect Input rectangle.
 * @return WinrtRect* Adjusted rectangle.
 */
WinrtRect* WINAPI HardwareConfirmatorHost_GetPositionRect_Hook(void* pThis, WinrtRect* retval, const WinrtRect* rect) {

    // Log hook activation for debugging purposes
    Wh_Log(L">");

    // Create a local copy of the input rectangle to modify it safely
    WinrtRect shifted = *rect;

    // Save the original X coordinate offset for restoration later
    float ox = shifted.X;

    // Save the original Y coordinate offset for restoration later
    float oy = shifted.Y;

    // Reset X to zero to perform calculations relative to the origin
    shifted.X = 0;

    // Reset Y to zero to perform calculations relative to the origin
    shifted.Y = 0;

    // Invoke the original function using the shifted origin to get the relative position
    WinrtRect* result = HardwareConfirmatorHost_GetPositionRect_Original(pThis, retval, &shifted);

    // Add the original X offset back to the result to restore absolute positioning
    result->X += ox;

    // Add the original Y offset back to the result to restore absolute positioning
    result->Y += oy;

    // Return the adjusted pointer to the caller
    return result;
}

/**
 * @brief Hook for TaskbarFrame::OnPointerPressed.
 *
 * Detects middle-click or double-click on the taskbar frame and switches the
 * taskbar to the clicked monitor.
 *
 * @param pThis COM instance pointer.
 * @param pArgs Pointer event arguments.
 * @return int Original return value or 0 if handled.
 */
int WINAPI TaskbarFrame_OnPointerPressed_Hook(void* pThis, void* pArgs) {

    // Log hook activation for debugging purposes
    Wh_Log(L">");

    // Define a lambda to easily call the original pointer handler when needed
    auto original = [=] { return TaskbarFrame_OnPointerPressed_Original(pThis, pArgs); };

    // Initialize a WinRT UIElement pointer to store the taskbar frame instance
    winrt::Windows::UI::Xaml::UIElement taskbarFrame = nullptr;

    // Request the UIElement interface from the COM object passed in pThis
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<winrt::Windows::UI::Xaml::UIElement>(), winrt::put_abi(taskbarFrame));

    // Exit and call original if the taskbar frame interface is not available
    if(!taskbarFrame) return original();

    // Retrieve the XAML class name of the current UI element
    auto className = winrt::get_class_name(taskbarFrame);

    // Only proceed if the class specifically matches the taskbar frame type
    if(className != L"Taskbar.TaskbarFrame") return original();

    // Convert the raw pArgs pointer into a usable WinRT event arguments object
    winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs args{nullptr};

    // Copy the ABI pointer into our WinRT smart object
    winrt::copy_from_abi(args, pArgs);

    // If the arguments couldn't be parsed, revert to the original handler
    if(!args) return original();

    // Fetch the specific pointer properties for the current interaction point
    auto properties = args.GetCurrentPoint(taskbarFrame).Properties();

    // Prepare a POINT structure to receive the current cursor location
    POINT pt;

    // Get the global mouse cursor position on the virtual screen
    GetCursorPos(&pt);

    // Find the monitor handle at the cursor location, using the nearest if off-screen
    HMONITOR pressedMonitor = MonitorFromPoint_Original(pt, MONITOR_DEFAULTTONEAREST);

    // Check if the monitor is invalid or already the current override
    bool isSameMonitor = (!pressedMonitor || pressedMonitor == g_overrideMonitor);

    // Stop processing if the user clicked on the monitor already hosting the taskbar
    if(isSameMonitor) return original();

    // Boolean flag to track if a switch condition has been met
    bool shouldSwitch = false;

    // Evaluate switching logic for middle-click or standard double-click methods
    if(g_settings.clickToSwitchMonitor == ClickToSwitchMonitor::middleClick) shouldSwitch = properties.IsMiddleButtonPressed();

    /*
     * Check if the user has configured the double-click interaction instead
     */
    else if(g_settings.clickToSwitchMonitor == ClickToSwitchMonitor::doubleClick) {

        // Capture the current system tick count in milliseconds
        DWORD now = GetTickCount();

        // Check if the click is on the same monitor and within the double-click time limit
        bool isFastClick = (g_lastPressMonitor == pressedMonitor && now - g_lastPressTime <= GetDoubleClickTime());

        /*
         * Execute logic to confirm a successful double-click sequence
         */
        if(isFastClick) {

            // Reset time to prevent triple-click triggers
            g_lastPressTime = 0;

            // Clear the tracked monitor
            g_lastPressMonitor = nullptr;

            // Confirm a valid switch trigger
            shouldSwitch = true;

        /*
         * Handle the first click of a potential double-click sequence
         */
        } else {

            // Update the last click time for the next comparison
            g_lastPressTime = now;

            // Store the monitor that was clicked
            g_lastPressMonitor = pressedMonitor;
        }
    }

    // If no valid switch trigger was confirmed, fall back to the original function
    if(!shouldSwitch) return original();

    // Assign the clicked monitor as the new global taskbar override
    g_overrideMonitor = pressedMonitor;

    // Update the system and refresh settings to reflect the new monitor
    ApplySettings();

    // Set the event as handled to stop further propagation in the XAML tree
    args.Handled(true);

    // Return zero to indicate to the caller that the message was processed
    return 0;
}

/**
 * @brief Hook for TrayUI::_SetStuckMonitor.
 *
 * Forces the tray UI to use the overridden monitor.
 *
 * @param pThis COM instance pointer.
 * @param monitor Original monitor.
 * @return HRESULT Result of original function.
 */
HRESULT WINAPI TrayUI__SetStuckMonitor_Hook(void* pThis, HMONITOR monitor) {

    // Log hook activation for debugging purposes
    Wh_Log(L">");

    // Determine the target monitor while allowing overrides even during session lock
    HMONITOR target = GetTargetMonitor({.ignoreLockedState = true});

    // If a valid target override exists, swap the original monitor handle
    if(target) monitor = target;

    // Call the original internal function with the final monitor destination
    return TrayUI__SetStuckMonitor_Original(pThis, monitor);
}

/**
 * @brief Hook for LoadLibraryExW to detect module loads.
 *
 * When Explorer loads a module, this hook checks whether it is:
 * - ExplorerPatcher (ep_taskbar.*)
 * - Taskbar.View.dll / ExplorerExtensions.dll
 *
 * and applies hooks accordingly.
 *
 * @param lpLibFileName Name of the module being loaded.
 * @param hFile Reserved.
 * @param dwFlags Load flags.
 * @return HMODULE Handle to the loaded module.
 */
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {

    // Perform the actual library loading operation using the original API
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    /*
     * If the module handle is valid, check if it needs additional hooking
     */
    if(module) {

        // Scan for ExplorerPatcher components to apply compatibility fixes
        HandleLoadedModuleIfExplorerPatcher(module);

        // Scan for the main Taskbar View module to apply core functionality hooks
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    // Return the handle of the loaded module to the system
    return module;
}

// ============================================================================
// ExplorerPatcher support
// ============================================================================

/**
 * @brief Structure describing a symbol hook for ExplorerPatcher.
 *
 * Each entry contains:
 * - symbol: The mangled symbol name to locate.
 * - pOriginalFunction: Pointer to store the original function address.
 * - hookFunction: The hook function to install.
 * - optional: Whether the symbol is optional (missing symbol does not fail).
 */
struct EXPLORER_PATCHER_HOOK {

    // Mangled symbol name used to locate the function address in the module
    PCSTR symbol;

    // Pointer to the storage location for the original function address
    void** pOriginalFunction;

    // Pointer to the custom hook function implementation
    void* hookFunction = nullptr;

    // Boolean flag indicating if the symbol is required for the mod to function
    bool optional = false;

    // Template declaration to handle various function prototypes with type safety
    template <typename Prototype>

    /*
     * Constructor initialization for the ExplorerPatcher hook structure
     */
    EXPLORER_PATCHER_HOOK(PCSTR symbol, Prototype** originalFunction, std::type_identity_t<Prototype*> hookFunction = nullptr, bool optional = false) :

        // Assign the symbol string to the internal member
        symbol(symbol),

        // Convert the prototype-specific pointer to a generic void pointer
        pOriginalFunction(reinterpret_cast<void**>(originalFunction)),

        // Convert the hook function pointer to a generic void pointer
        hookFunction(reinterpret_cast<void*>(hookFunction)),

        // Store the optional flag for later validation
        optional(optional) {/* Body of the constructor remains empty as initialization is handled above */}
};

/**
 * @brief Hook ExplorerPatcher's internal TrayUI::_SetStuckMonitor function.
 *
 * This allows the mod to override ExplorerPatcher's own logic for determining
 * which monitor the taskbar should be "stuck" to.
 *
 * @param explorerPatcherModule Handle to the ExplorerPatcher module.
 * @return true if hooking succeeded or was already done, false otherwise.
 */
bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {

    // Atomically check and set the initialization flag to prevent concurrent setup
    if(g_explorerPatcherInitialized.exchange(true)) return true;

    // Adjust version logic since ExplorerPatcher relies on Windows 10 taskbar behavior
    if(g_winVersion >= WinVersion::Win11) g_winVersion = WinVersion::Win10; 

    /*
     * Define the list of symbols to intercept within the ExplorerPatcher module
     */
    EXPLORER_PATCHER_HOOK explorerPatcherHooks[] = {
        {
            // The specific mangled name for the TrayUI::_SetStuckMonitor method
            R"(?_SetStuckMonitor@TrayUI@@QEAAJPEAUHMONITOR__@@@Z)",

            // The variable where we store the original function for calling back
            &TrayUI__SetStuckMonitor_Original,

            // Our custom implementation that overrides the monitor selection
            TrayUI__SetStuckMonitor_Hook
        }
    };

    // Track the overall success of the hooking operation
    bool succeeded = true;

    /*
     * Iterate through each defined hook to locate and intercept the symbols
     */
    for(const auto& hook : explorerPatcherHooks) {

        // Retrieve the memory address of the symbol from the loaded module
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);

        /*
         * Handle cases where the requested symbol cannot be found in the module
         */
        if(!ptr) {

            // Log the missing symbol and indicate if it was marked as optional
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S", hook.optional ? L" (optional)" : L"", hook.symbol);

            // Update the success status if a non-optional symbol is missing
            if(!hook.optional) succeeded = false;

            // Skip to the next hook in the array
            continue;
        }

        // Install the hook if an implementation is provided, otherwise just store the address
        if(hook.hookFunction) Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);

        // Store the original address for modules that only need to call the function
        else *hook.pOriginalFunction = ptr;
    }

    // Log the failure to the Windhawk console if the hooking process failed
    if(!succeeded) Wh_Log(L"HookExplorerPatcherSymbols failed");

    // Commit all registered hook operations if the mod is already initialized
    else if(g_initialized) Wh_ApplyHookOperations();

    // Return the final success state of the symbol hooking
    return succeeded;
}

/**
 * @brief Check whether a module is an ExplorerPatcher taskbar module.
 *
 * ExplorerPatcher modules follow the naming pattern: "ep_taskbar.*"
 *
 * @param module Module handle to inspect.
 * @return true if the module is an ExplorerPatcher taskbar module.
 */
bool IsExplorerPatcherModule(HMODULE module) {

    // Buffer to store the full file system path of the module
    WCHAR path[MAX_PATH];

    /*
     * Retrieve the file name and validate the result size
     */
    switch(GetModuleFileName(module, path, ARRAYSIZE(path))) {

        // Handle retrieval failure
        case 0:

        // Return false if the path is invalid or incomplete due to truncation
        case ARRAYSIZE(path): return false;
    }

    // Locate the last backslash to isolate the filename from the path
    PCWSTR name = wcsrchr(path, L'\\');

    // Return false if the path formatting is unexpected
    if(!name) return false;

    // Increment the pointer to skip the backslash character itself
    ++name;

    // Compare the start of the filename against the known ExplorerPatcher prefix
    bool isMatch = (_wcsnicmp(L"ep_taskbar.", name, sizeof("ep_taskbar.") - 1) == 0);

    /*
     * Log and confirm if the module matches the ExplorerPatcher pattern
     */
    if(isMatch) {

        // Log the detected ExplorerPatcher module name
        Wh_Log(L"ExplorerPatcher taskbar module: %s", name);

        // Return true for a positive identification
        return true;
    }

    // Return false if no match was found
    return false;
}

/**
 * @brief Scan loaded modules to detect ExplorerPatcher and hook it.
 *
 * This is used both during initialization and after module loads to ensure
 * ExplorerPatcher is hooked even if it loads late.
 *
 * @return true if scanning succeeded.
 */
bool HandleLoadedExplorerPatcher() {

    // Array to hold handles for all modules currently loaded in the process
    HMODULE hMods[1024];

    // Variable to receive the actual number of bytes written to the buffer
    DWORD cbNeeded;

    // Enumerate all modules associated with the current process
    bool enumerated = EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded);

    /*
     * If enumeration succeeds, iterate through the list to find ExplorerPatcher
     */
    if(enumerated) {

        /*
         * Calculate the number of modules found based on the byte count
         */
        for(size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {

            // Attempt to install hooks if the current module matches ExplorerPatcher
            if(IsExplorerPatcherModule(hMods[i])) return HookExplorerPatcherSymbols(hMods[i]);
        }
    }

    // Return true by default even if no specific module was found
    return true;
}

/**
 * @brief Handle module load event and hook ExplorerPatcher if detected.
 *
 * @param module Newly loaded module.
 */
void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {

    // Verify the module pointer is valid and properly aligned
    bool isValid = (module && !((ULONG_PTR)module & 3));

    /*
     * Check if the module is valid and if initialization hasn't occurred yet
     */
    if(isValid && !g_explorerPatcherInitialized) {

        // Initialize symbol hooks if the newly loaded module is ExplorerPatcher
        if(IsExplorerPatcherModule(module)) HookExplorerPatcherSymbols(module);
    }
}

// ============================================================================
// Taskbar.View.dll support
// ============================================================================

/**
 * @brief Retrieve the module handle for Taskbar.View.dll or ExplorerExtensions.dll.
 *
 * Windows 11 uses a modern XAML-based taskbar implementation. Depending on
 * the build, the implementation lives in:
 * - Taskbar.View.dll
 * - ExplorerExtensions.dll
 *
 * @return HMODULE Handle to the module if loaded, otherwise nullptr.
 */
HMODULE GetTaskbarViewModuleHandle() {

    // Try primary module name
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");

    // Try fallback module name if primary is not found
    if(!module) module = GetModuleHandle(L"ExplorerExtensions.dll");

    // Return whichever handle was found or nullptr
    return module;
}

/**
 * @brief Hook WinRT TaskbarFrame::OnPointerPressed to support click-to-switch.
 *
 * This hook allows the mod to detect pointer presses on the taskbar frame
 * (middle-click or double-click) and switch monitors accordingly.
 *
 * @param module Handle to Taskbar.View.dll or ExplorerExtensions.dll.
 * @return true if hooking succeeded, false otherwise.
 */
bool HookTaskbarViewDllSymbols(HMODULE module) {
    
    /*************************************************************/
    /* Define the list of symbols to be hooked within the module */
    /*************************************************************/
  
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbar_hooks[] = {
        {
            /*
             * Windows 11 XAML TaskbarFrame OnPointerPressed override
             */
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))"
            },

            // Store original function pointer
            &TaskbarFrame_OnPointerPressed_Original,   

            // Hook implementation
            TaskbarFrame_OnPointerPressed_Hook
        }
    };

    /*
     * Finalize the symbol hooking process by logging any errors
     */
    if(!HookSymbols(module, taskbar_hooks, ARRAYSIZE(taskbar_hooks))) {

        // Log the specific failure to the Windhawk console
        Wh_Log(L"HookTaskbarViewDllSymbols: HookSymbols failed");

        // Stop execution and signal that hooking was not successful
        return false;
    }

    // Return true to indicate successful hook installation
    return true;
}

/**
 * @brief Determine whether Taskbar.View.dll hooks should be applied.
 *
 * Hooks are only needed when:
 * - Running on Windows 11 or later.
 * - Click-to-switch is enabled.
 *
 * @return true if hooks should be installed.
 */
bool ShouldHookTaskbarViewDllSymbols() {

    // Hooks are only needed on Windows 11+ with click-to-switch enabled
    return g_winVersion >= WinVersion::Win11 && g_settings.clickToSwitchMonitor != ClickToSwitchMonitor::disabled;
}


/**
 * @brief Handle module load event and hook Taskbar.View.dll if appropriate.
 *
 * This is triggered by LoadLibraryExW_Hook whenever Explorer loads a module.
 *
 * @param module Newly loaded module.
 * @param lpLibFileName Name of the module being loaded.
 */
void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {

    /*
     * Initialize hooking if all conditions for Taskbar.View are met
     */
    if(ShouldHookTaskbarViewDllSymbols() && !g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module && !g_taskbarViewDllLoaded.exchange(true)) {
        
        // Log the module detection
        Wh_Log(L"Loaded %s", lpLibFileName);

        // Apply all registered hook operations if symbol hooking succeeds
        if(HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
    }
}

// ============================================================================
// Taskbar / HardwareConfirmator symbol hooking
// ============================================================================

/**
 * @brief Hook taskbar-related symbols depending on Windows version.
 *
 * Windows 10:
 *   - Taskbar code lives inside explorer.exe.
 *
 * Windows 11:
 *   - Taskbar code lives inside taskbar.dll.
 *
 * This function loads the appropriate module and installs hooks for:
 *   - TrayUI::_SetStuckMonitor
 *
 * @return true if hooking succeeded, false otherwise.
 */
bool HookTaskbarSymbols() {

    // Handle to the target module for symbol hooking
    HMODULE module;

    // Use the current process handle for Windows 10, otherwise load the taskbar DLL
    if(g_winVersion <= WinVersion::Win10) module = GetModuleHandle(nullptr);

    /*
     * Attempt to load the taskbar module on Windows 11 and handle potential failure
     */
    else {

        // Dynamically load taskbar.dll from the System32 directory
        module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

        // Check if the module handle is valid before proceeding
        if(!module) {

            // Log the loading error to the Windhawk console
            Wh_Log(L"Couldn't load taskbar.dll");

            // Terminate the function as the required DLL is missing
            return false;
        }
    }

    /***********************************************************/
    /* Define the list of taskbar-related symbols to be hooked */
    /***********************************************************/
  
    // explorer.exe, taskbar.dll
    WindhawkUtils::SYMBOL_HOOK explorer_hooks[] = {
        {
            /*
             * TrayUI::_SetStuckMonitor signatures for Windows 10 and 11
             */
            {
                // Windows 11 signature
                LR"(public: long __cdecl TrayUI::_SetStuckMonitor(struct HMONITOR__ *))",

                // Windows 10 signature
                LR"(public: void __cdecl TrayUI::_SetStuckMonitor(struct HMONITOR__ *))"
            },

            // Store original function pointer
            &TrayUI__SetStuckMonitor_Original,

            // Hook implementation
            TrayUI__SetStuckMonitor_Hook
        }
    };

    /*
     * Finalize the symbol hooking process by logging any errors
     */
    if(!HookSymbols(module, explorer_hooks, ARRAYSIZE(explorer_hooks))) {

        // Log the specific failure to the Windhawk console
        Wh_Log(L"HookTaskbarSymbols: HookSymbols failed");

        // Stop execution and signal that hooking was not successful
        return false;
    }

    // Return true to indicate successful hook installation
    return true;
}

/**
 * @brief Hook HardwareConfirmatorHost::GetPositionRect.
 *
 * This ensures WinRT confirmation popups (volume, brightness, etc.) appear
 * on the overridden primary monitor.
 *
 * @return true if hooking succeeded, false otherwise.
 */
bool HookHardwareConfirmatorSymbols() {

    // Load the library responsible for hardware confirmation popups
    HMODULE module = LoadLibraryEx(L"Windows.Internal.HardwareConfirmator.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    /*
     * Log failure and return false if the DLL cannot be loaded
     */
    if(!module) {

        // Log the loading error to the Windhawk console
        Wh_Log(L"Couldn't load Windows.Internal.HardwareConfirmator.dll");

        // Stop execution as the required module is missing
        return false;
    }

    /*****************************************************************/
    /* Define the list of symbols to be hooked for popup positioning */
    /*****************************************************************/

    // Windows.Internal.HardwareConfirmator.dll
    WindhawkUtils::SYMBOL_HOOK hardware_confirmator_hooks[] = {
        {
            /*
             * Windows Internal HardwareConfirmator GetPositionRect override
             */
            {
                LR"(private: struct winrt::Windows::Foundation::Rect __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::GetPositionRect(struct winrt::Windows::Foundation::Rect const &))"
            },

            // Store original function pointer
            &HardwareConfirmatorHost_GetPositionRect_Original,

            // Hook implementation
            HardwareConfirmatorHost_GetPositionRect_Hook
        }
    };

    /*
     * Finalize the symbol hooking process by logging any errors
     */
    if(!HookSymbols(module, hardware_confirmator_hooks, ARRAYSIZE(hardware_confirmator_hooks))) {

        // Log the specific failure to the Windhawk console
        Wh_Log(L"HookHardwareConfirmatorSymbols: HookSymbols failed");

        // Stop execution and signal that hooking was not successful
        return false;
    }

    // Return true to indicate successful hook installation
    return true;
}

// ============================================================================
// Settings & lifecycle
// ============================================================================

/**
 * @brief Apply the current monitor override settings to the taskbar.
 *
 * This function:
 * - Locates the taskbar window for the current process.
 * - Sends a private message (0x5B8) to trigger CTray::_HandleDisplayChange.
 * - Broadcasts a shell hook display change event.
 */
void ApplySettings() {

    // Locate the taskbar window associated with the current process
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();

    // If no taskbar window exists, there is nothing to apply
    if(!hTaskbarWnd) return;

    // Send a private message (0x5B8) to trigger CTray::_HandleDisplayChange internally
    SendMessage(hTaskbarWnd, 0x5B8, 0, 0);

    // Notify the system and shell listeners that a display change has occurred
    BroadcastShellHookDisplayChange();
}

/**
 * @brief Load all mod settings from Windhawk.
 *
 * Reads:
 * - Monitor number
 * - Monitor interface name
 * - Click-to-switch mode
 * - Keyboard shortcut
 * - Move additional elements
 * - Old taskbar mode (ExplorerPatcher compatibility)
 */
void LoadSettings() {

    // Fetch the target monitor index from Windhawk settings
    g_settings.monitor = Wh_GetIntSetting(L"monitor");

    // Initialize the monitor interface name using the Windhawk utility string wrapper
    g_settings.monitorInterfaceName = WindhawkUtils::StringSetting::make(L"monitorInterfaceName");

    // ----------------------- //
    // --- Click-to-switch --- //
    // ----------------------- //

    // Retrieve the string representation of the click-to-switch behavior
    PCWSTR ctm = Wh_GetStringSetting(L"clickToSwitchMonitor");

    // Set the default state to disabled before parsing the setting
    g_settings.clickToSwitchMonitor = ClickToSwitchMonitor::disabled;

    // Check if the double-click option was selected by the user
    if (wcscmp(ctm, L"doubleClick") == 0) g_settings.clickToSwitchMonitor = ClickToSwitchMonitor::doubleClick;

    // Check if the middle-click option was selected by the user
    else if (wcscmp(ctm, L"middleClick") == 0) g_settings.clickToSwitchMonitor = ClickToSwitchMonitor::middleClick;

    // Release the memory allocated for the click-to-switch string setting
    Wh_FreeStringSetting(ctm);

    // ------------------------- //
    // --- Keyboard shortcut --- //
    // ------------------------- //

    // Define a structure to map human-readable names to virtual key modifiers and codes
    struct HotkeyOption { PCWSTR name; UINT mods, vk; };

     /*
      *  Initialize a constant array of available hotkey configurations
      */
    constexpr HotkeyOption kOptions[] = {

        // Option for Windows + Shift + F
        {L"win_shift_f",      MOD_WIN     | MOD_SHIFT | MOD_NOREPEAT, 'F'},

        // Option for Windows + Shift + X
        {L"win_shift_x",      MOD_WIN     | MOD_SHIFT | MOD_NOREPEAT, 'X'},

        // Option for Windows + Alt + X
        {L"win_alt_x",        MOD_WIN     | MOD_ALT   | MOD_NOREPEAT, 'X'},

        // Option for Windows + Shift + Comma (using the specific OEM virtual key)
        {L"win_shift_comma",  MOD_WIN     | MOD_SHIFT | MOD_NOREPEAT, VK_OEM_102},

        // Option for Ctrl + Alt + Comma (using the specific OEM virtual key)
        {L"ctrl_alt_comma",   MOD_CONTROL | MOD_ALT   | MOD_NOREPEAT, VK_OEM_102}
    };

    // Load the keyboard shortcut string from the mod settings
    PCWSTR ks = Wh_GetStringSetting(L"keyboardShortcut");

    // Reset the active hotkey modifiers to zero
    g_settings.hotkeyModifiers = 0;

    // Reset the active virtual key code to zero
    g_settings.hotkeyVk = 0;

    /*
     * Iterate through the predefined options to find a match for the current setting
     */
    for(const auto& opt : kOptions) {

        /*
         * Compare the current setting string with the predefined option name
         */
        if(wcscmp(ks, opt.name) == 0) {

            // Assign the corresponding modifier keys to the global settings
            g_settings.hotkeyModifiers = opt.mods;

            // Assign the corresponding virtual key code to the global settings
            g_settings.hotkeyVk = opt.vk;

            // Exit the loop early as the matching shortcut has been found
            break;
        }
    }

    // Release the memory allocated for the keyboard shortcut string setting
    Wh_FreeStringSetting(ks);

    // Load the preference for moving additional UI elements (like the tray)
    g_settings.moveAdditionalElements = Wh_GetIntSetting(L"moveAdditionalElements");

    // Load the compatibility flag for using the Windows 10 taskbar style on Windows 11
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

/**
 * @brief Windhawk mod initialization function.
 *
 * @return TRUE on success, FALSE on failure.
 */
BOOL Wh_ModInit() {
    
    // Log the entry into the initialization function
    Wh_Log(L">");

    // Load the mod configuration from Windhawk settings
    LoadSettings();

    // Set the default execution target to the Explorer process
    g_target = Target::Explorer;

    // Define a buffer to store the file path of the current process executable
    WCHAR moduleFilePath[MAX_PATH];

    /*
     * Retrieve the full path of the executable that loaded the mod
     */
    switch (GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {

        // Handle the case where the function fails and returns zero
        case 0:

        /*
         * Handle the case where the path is too long for the buffer
         */
        case ARRAYSIZE(moduleFilePath):

            // Log the retrieval failure to the Windhawk console
            Wh_Log(L"GetModuleFileName failed");

            // Exit the switch block
            break;

        /*
         * Process the retrieved file path to identify the executable name
         */
        default:

            /*
             * Locate the last backslash in the path to isolate the filename
             */ 
            if(PCWSTR name = wcsrchr(moduleFilePath, L'\\')) {

                // Update global target to ShellHost if the filename matches (case-insensitive)
                if(_wcsicmp(++name, L"ShellHost.exe") == 0) g_target = Target::ShellHost;

            // Handle scenarios where the path format is not recognized
            } else Wh_Log(L"GetModuleFileName returned an unsupported path");

            // Exit the switch block
            break;
    }

    /*
     * Configure hooks specific to the Explorer process
     */
    if(g_target == Target::Explorer) {

        // Detect the specific version of the Explorer process
        g_winVersion = GetExplorerVersion();

        /*
         * Abort initialization if the Windows version is not supported 
         */
        if(g_winVersion == WinVersion::Unsupported) {

            // Log the compatibility error to the Windhawk console
            Wh_Log(L"Unsupported Windows version");

            // Stop execution as the mod cannot function on this version of Windows
            return FALSE;
        }

        /*
         * Handle ExplorerPatcher compatibility or standard Windows 11 hooking
         */
        if(g_settings.oldTaskbarOnWin11) {

            // Check for the presence of the legacy taskbar code based on Windows version
            bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

            // Force the internal version to Win10 to utilize legacy logic paths
            if(g_winVersion >= WinVersion::Win11) g_winVersion = WinVersion::Win10;

            // Attempt to hook legacy taskbar symbols if the required code is present
            if(hasWin10Taskbar && !HookTaskbarSymbols()) return FALSE;

        /*
         * Fallback to standard Windows 11 XAML and taskbar.dll hooking logic
         */
        } else {

            /*
             * Determine if XAML-based taskbar symbols should be targeted
             */
            if(ShouldHookTaskbarViewDllSymbols()) {

                /*
                 * Retrieve the module handle for the TaskbarView component
                 */
                if(HMODULE m = GetTaskbarViewModuleHandle()) {

                    // Mark the TaskbarView DLL as loaded in the global state
                    g_taskbarViewDllLoaded = true;

                    // Execute the hooking process for TaskbarView symbols
                    HookTaskbarViewDllSymbols(m);
                }
            }

            // Install the standard taskbar hooks and abort on failure
            if(!HookTaskbarSymbols()) return FALSE;
        }

        /*
         * Initialize specific support for ExplorerPatcher environments
         */
        if(!HandleLoadedExplorerPatcher()) {

            // Log the initialization failure for ExplorerPatcher compatibility
            Wh_Log(L"HandleLoadedExplorerPatcher failed");

            // Abort the mod initialization as a required compatibility layer failed
            return FALSE;
        }

        // Retrieve the handle for kernelbase.dll
        HMODULE kernelBase = GetModuleHandle(L"kernelbase.dll");

        // Get the address of LoadLibraryExW to intercept dynamic module loading
        auto pLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(kernelBase, "LoadLibraryExW");

        // Install the function hook for LoadLibraryExW
        WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }

    // Install hooks for Windows Internal HardwareConfirmator symbols
    HookHardwareConfirmatorSymbols();

    // Install the function hook for MonitorFromPoint
    WindhawkUtils::SetFunctionHook(MonitorFromPoint, MonitorFromPoint_Hook, &MonitorFromPoint_Original);

    // Install the function hook for MonitorFromRect
    WindhawkUtils::SetFunctionHook(MonitorFromRect, MonitorFromRect_Hook, &MonitorFromRect_Original);

    // Install the function hook for EnumDisplayDevicesW
    WindhawkUtils::SetFunctionHook(EnumDisplayDevicesW, EnumDisplayDevicesW_Hook, &EnumDisplayDevicesW_Original);

    // Mark the mod as fully initialized
    g_initialized = true;

    // Return true to indicate successful initialization
    return TRUE;
}

/**
 * @brief Called after initialization is complete.
 */
void Wh_ModAfterInit() {

    // Log the entry into the post-initialization phase
    Wh_Log(L">");

    /*
     * Finalize late-loading hooks and apply initial UI state
     */
    if(g_target == Target::Explorer) {

        /*
         * Check if XAML hooks are required but the DLL wasn't loaded during Init
         */
        if(ShouldHookTaskbarViewDllSymbols() && !g_taskbarViewDllLoaded) {

            /*
             * Attempt to retrieve the handle for the TaskbarView module again
             */
            if(HMODULE m = GetTaskbarViewModuleHandle()) {

                /*
                 * Perform an atomic check-and-set to ensure we only hook once
                 */
                if(!g_taskbarViewDllLoaded.exchange(true)) {

                    // Log that the late-loading DLL has been successfully located
                    Wh_Log(L"Got Taskbar.View.dll");

                    // Apply the hooks and commit the operations if symbol hooking succeeds
                    if(HookTaskbarViewDllSymbols(m)) Wh_ApplyHookOperations();
                }
            }
        }

        // Retry ExplorerPatcher initialization if it wasn't handled previously
        if(!g_explorerPatcherInitialized) HandleLoadedExplorerPatcher();

        // Apply the current monitor override settings to the taskbar
        ApplySettings();

        // Launch the background thread for monitoring keyboard shortcuts
        StartHotkeyThread();
    }
}

/**
 * @brief Called before the mod is unloaded.
 */
void Wh_ModBeforeUninit() {

    // Log the entry into the pre-uninstallation phase
    Wh_Log(L">");

    // Set the global flag to signal that the mod is currently unloading
    g_unloading = true;

    /*
     * Stop background tasks and restore original monitor settings
     */
    if(g_target == Target::Explorer) {

        // Terminate the background thread responsible for keyboard shortcuts
        StopHotkeyThread();

        // Revert any changes made to the monitor configuration
        ApplySettings();
    }
}

/**
 * @brief Called when the mod is fully unloaded.
 */
void Wh_ModUninit() {

    // Log the final cleanup phase before the DLL is released
    Wh_Log(L">");
}

/**
 * @brief Called when settings change in the Windhawk UI.
 *
 * @param bReload Output flag indicating whether the mod must reload.
 * @return TRUE always.
 */
BOOL Wh_ModSettingsChanged(BOOL* bReload) {

    // Log the entry into the settings change handler
    Wh_Log(L">");

    // Store the previous state of the legacy taskbar setting to detect changes
    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    // Store whether the click-to-switch feature was previously enabled
    bool prevClickEnabled = g_settings.clickToSwitchMonitor != ClickToSwitchMonitor::disabled;

    // Capture the previous hotkey modifier keys
    UINT prevHotkeyMods = g_settings.hotkeyModifiers;

    // Capture the previous hotkey virtual key code
    UINT prevHotkeyVk = g_settings.hotkeyVk;

    // Refresh the global settings structure with the new values from Windhawk
    LoadSettings();

    // Determine the new enabled state of the click-to-switch feature
    bool clickEnabled = g_settings.clickToSwitchMonitor != ClickToSwitchMonitor::disabled;

    /*
     * Update operational state based on the newly loaded settings
     */
    if(g_target == Target::Explorer) {

        // Reset the monitor override if the click feature has been disabled
        if(!clickEnabled) g_overrideMonitor = nullptr;

        // Re-register the system hotkey if the key combination was modified
        if(g_settings.hotkeyModifiers != prevHotkeyMods || g_settings.hotkeyVk != prevHotkeyVk) ReregisterHotkey();

        // Set the reload flag if structural settings (like taskbar type) have changed
        *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11 || clickEnabled != prevClickEnabled;

        // Apply visual or behavioral settings immediately if a full reload isn't required
        if(!*bReload) ApplySettings();
    }

    // Return TRUE to indicate the settings change was processed
    return TRUE;
}
