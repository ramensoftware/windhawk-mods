// ==WindhawkMod==
// @id              auto-hide-desktop-icons
// @name            Auto Hide Desktop Icons
// @description     Automatically hides Windows desktop icons after user inactivity.
// @version         1.0.0
// @author          JoshiMinh
// @github          JoshiMinh
// @twitter         https://twitter.com/JoshiMinh
// @homepage        https://your-personal-homepage.example.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Auto Hide Desktop Icons
This is a place for useful information about your mod. Use it to describe the
mod, explain why it's useful, and add any other relevant details. You can use
[Markdown](https://en.wikipedia.org/wiki/Markdown) to add links and
**formatting** to the readme.

This mod automatically hides your Windows desktop icons when you are inactive, 
and shows them again ONLY when you click on the desktop. To see the mod in action:
- Compile the mod with the button on the left or with Ctrl+B.
- Stop moving your mouse or pressing keys for the idle timeout duration (default is 5 seconds).
- Notice that your desktop icons automatically disappear and stay hidden even if you move the mouse.
- Click anywhere on the desktop and notice that the icons reappear.

# Getting started
Check out the documentation
[here](https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able
# to configure. Metadata values such as $name and $description are optional.
# Check out the documentation for more information:
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings
- idle_timeout: 5000
  $name: Idle Timeout (ms)
  $description: Time in milliseconds of inactivity before hiding the icons.
- polling_interval: 200
  $name: Polling Interval (ms)
  $description: How often to check for inactivity (in milliseconds).
*/
// ==/WindhawkModSettings==

// The source code of the mod starts here.

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <mutex>
#include <commctrl.h>

HANDLE g_hTimer = NULL;
HANDLE g_hClickTimer = NULL;
std::mutex g_timerMutex;

struct {
    DWORD idleTimeout;
    DWORD pollingInterval;
} settings;

HWND g_hwndDesktopList = NULL;
HWND g_hwndShellView = NULL;
bool g_iconsHidden = false;

HWND FindDesktopListView() {
    HWND hwndProgman = FindWindowW(L"Progman", NULL); // Removed hardcoded "Program Manager" for non-English Win11 support
    HWND hwndShellView = FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL);
    
    if (!hwndShellView) {
        HWND hwndWorkerW = NULL;
        while ((hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL)) != NULL) {
            hwndShellView = FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            if (hwndShellView) {
                break;
            }
        }
    }

    if (hwndShellView) {
        return FindWindowExW(hwndShellView, NULL, L"SysListView32", NULL);
    }

    return NULL;
}

void SetDesktopIconsVisibility(HWND hwndListView, bool show) {
    if (hwndListView) {
        ShowWindow(hwndListView, show ? SW_SHOW : SW_HIDE);
        g_iconsHidden = !show;
        if (show) {
            Wh_Log(L"Desktop icons shown");
        } else {
            Wh_Log(L"Desktop icons hidden");
        }
    }
}

VOID CALLBACK ClickTimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired) {
    static bool s_wasLDown = false;
    static bool s_wasRDown = false;
    bool isLDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    bool isRDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

    if ((isLDown && !s_wasLDown) || (isRDown && !s_wasRDown)) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            HWND hwndHover = WindowFromPoint(pt);
            if (hwndHover) {
                bool isDesktopClick = false;
                bool isMenu = false;
                
                // GetAncestor correctly identifies if a clicked control (like Rename box) belongs to the Desktop
                HWND hwndRoot = GetAncestor(hwndHover, GA_ROOT);
                if (hwndRoot) {
                    wchar_t className[256];
                    if (GetClassNameW(hwndRoot, className, 256)) {
                        if (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0) {
                            isDesktopClick = true;
                        } else if (wcscmp(className, L"#32768") == 0) {
                            isMenu = true;
                        }
                    }
                }

                std::lock_guard<std::mutex> lock(g_timerMutex);
                if (isDesktopClick) {
                    if (g_iconsHidden && g_hwndDesktopList) {
                        SetDesktopIconsVisibility(g_hwndDesktopList, true);
                    }
                } else if (!isMenu) {
                    // Clicked on something else (e.g., another app), instantly hide the icons!
                    if (!g_iconsHidden && g_hwndDesktopList) {
                        SetDesktopIconsVisibility(g_hwndDesktopList, false);
                    }
                }
            }
        }
    }
    
    s_wasLDown = isLDown;
    s_wasRDown = isRDown;
}

VOID CALLBACK TimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired) {
    std::unique_lock<std::mutex> lock(g_timerMutex, std::try_to_lock);
    if (!lock.owns_lock()) {
        return; // Skip if previous callback is still running
    }

    LASTINPUTINFO lii;
    lii.cbSize = sizeof(LASTINPUTINFO);
    
    if (GetLastInputInfo(&lii)) {
        DWORD idleTime = GetTickCount() - lii.dwTime;
        
        // Optimize: Avoid heavy window enumeration if we already have the valid list view
        HWND currentListView = g_hwndDesktopList;
        if (!currentListView || !IsWindow(currentListView)) {
            currentListView = FindDesktopListView();
        }
        
        if (currentListView) {
            g_hwndShellView = GetParent(currentListView);
        } else {
            g_hwndShellView = NULL;
        }

        if (currentListView != g_hwndDesktopList) {
            g_hwndDesktopList = currentListView;
            if (g_hwndDesktopList) {
                // Determine current state in case it was recreated visible
                LONG_PTR style = GetWindowLongPtrW(g_hwndDesktopList, GWL_STYLE);
                if (style) {
                    g_iconsHidden = !(style & WS_VISIBLE);
                }
            }
        }

        if (g_hwndDesktopList) {
            if (idleTime >= settings.idleTimeout) {
                if (!g_iconsHidden) {
                    SetDesktopIconsVisibility(g_hwndDesktopList, false);
                }
            }
            // Removed the 'else' branch so moving the mouse no longer unhides the icons. 
            // They will only be shown via the desktop click subclass.
        }
    }
}

void LoadSettings() {
    settings.idleTimeout = Wh_GetIntSetting(L"idle_timeout");
    if (settings.idleTimeout < 1000) {
        settings.idleTimeout = 1000; // Minimum 1 second
    }

    settings.pollingInterval = Wh_GetIntSetting(L"polling_interval");
    if (settings.pollingInterval < 50) {
        settings.pollingInterval = 50; // Minimum 50ms to prevent high CPU usage
    }
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    // Start Win32 timer queue timer (no dedicated polling thread or sleep loops needed)
    if (!CreateTimerQueueTimer(&g_hTimer, NULL, TimerCallback, NULL, 0, settings.pollingInterval, WT_EXECUTEDEFAULT)) {
        Wh_Log(L"Failed to create timer queue timer");
        return FALSE;
    }

    // Start a 10ms hyper-rapid timer explicitly for detecting clicks reliably and instantly
    CreateTimerQueueTimer(&g_hClickTimer, NULL, ClickTimerCallback, NULL, 0, 10, WT_EXECUTEDEFAULT);

    Wh_Log(L"Timer started: polling interval %lu ms, idle timeout %lu ms", settings.pollingInterval, settings.idleTimeout);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_hTimer) {
        // Stop timer and wait for pending callbacks to finish
        DeleteTimerQueueTimer(NULL, g_hTimer, INVALID_HANDLE_VALUE);
        g_hTimer = NULL;
    }

    if (g_hClickTimer) {
        DeleteTimerQueueTimer(NULL, g_hClickTimer, INVALID_HANDLE_VALUE);
        g_hClickTimer = NULL;
    }

    // Restore icons visibility before exiting
    if (g_iconsHidden) {
        HWND currentListView = FindDesktopListView();
        if (currentListView) {
            SetDesktopIconsVisibility(currentListView, true);
        }
    }
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();

    if (g_hTimer) {
        ChangeTimerQueueTimer(NULL, g_hTimer, settings.pollingInterval, settings.pollingInterval);
    }
}
