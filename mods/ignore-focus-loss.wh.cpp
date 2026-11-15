// ==WindhawkMod==
// @id              ignore-focus-loss
// @name            Ignore Focus Loss
// @description     Make games believe they're always on top
// @version         1.0.0
// @author          Vasher
// @github          https://github.com/VasherMC
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Ignore Focus Loss
Some video games will pause or stop playing sound if they lose focus,
e.g. when you alt-tab. This interruption can be annoying.

This mod will lie to a program that it is focused,
making it always think it is in front.

## How to enable the mod for a program
In Windhawk, go to the "Advanced" tab and scroll down to
"Custom process inclusion list". In that box, put the filename of the `.exe`.
The mod will immediately apply to those programs after you click "Save".

## Known Issues / Caveats
- Exclusive fullscreen programs will stay on top of the monitor and
  block windows behind them, even though a different program is focused.
- Programs that capture the mouse (for example, many first-person games)
  may continue to lock the mouse in the centre of the screen.
- Depending on the program, it may keep processing keyboard
  inputs, even when you are using a different program.
- Programs may instead stop processing keyboard inputs, resulting in things like
  "Continuing to run forward if the `W` key was held when you alt-tabbed".
- This mod makes **no guarantees** of anti-cheat compatibility,
  and has not been tested with any games that use anti-cheat software.
*/
// ==/WindhawkModReadme==

#include <commctrl.h>

// the "Main" window
// specifically, the first that receives focus/activation/is foregrounded
HWND g_thisProgramWindow = 0;
// Corresponding thread id
DWORD g_uiThreadId = 0;
bool g_enabledHooks = false;

// Set the "Main" window if not yet set.
// This window remains the Main window until it is closed.
void setMainWindowThread(HWND hwnd) {
    g_thisProgramWindow = hwnd;
    g_uiThreadId = GetWindowThreadProcessId(hwnd, nullptr);
    // Debug logging:
    WCHAR buf[50];
    GetWindowTextW(hwnd, buf, 50);
    Wh_Log(L"Main window set: %s", buf);
}

// ------------------ Get Current Window functions ------------------
// Overrides for functions returning some kind of focused/active/foreground window
// Necessary or useful for particular engines like:
// GLFW - see https://github.com/glfw/glfw/blob/master/src/win32_window.c
// SDL - see https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsevents.c
using getWindow_t = decltype(&GetForegroundWindow);

getWindow_t pOriginalGetForegroundWindow;
HWND WINAPI GetForegroundWindowHook(void) {
    if (g_enabledHooks) {
        if(g_thisProgramWindow!=0) {
            return g_thisProgramWindow;
        }
        // Check if the foreground window is from the current process; if so store it
        HWND fgwin = pOriginalGetForegroundWindow();
        DWORD owningProcessId;
        GetWindowThreadProcessId(fgwin, &owningProcessId);
        if (owningProcessId==GetCurrentProcessId()) { // it's our window
            Wh_Log(L"set main window");
            setMainWindowThread(fgwin);
        }
        return fgwin;
    }
    return pOriginalGetForegroundWindow();
}

getWindow_t pOriginalGetActiveWindow;
HWND WINAPI GetActiveWindowHook(void) {
    if (g_enabledHooks) {
        if (g_thisProgramWindow==0) {
            HWND actual = pOriginalGetActiveWindow();
            if (actual != 0) {
                Wh_Log(L"set main window");
                setMainWindowThread(actual);
            } else Wh_Log(L"no main window yet; none active for thread");
            return actual;
        } else {
            // We only return the main window if the current thread owns it
            // (i.e., if GetActiveWindow would return the main window, given it was active)
            if (g_uiThreadId == GetCurrentThreadId()) return g_thisProgramWindow;
            return NULL;
        }
    }
    return pOriginalGetActiveWindow();
}

getWindow_t pOriginalGetFocus;
HWND WINAPI GetFocusHook(void) {
    if (g_enabledHooks) {
        if (g_thisProgramWindow==0) {
            HWND actual = pOriginalGetFocus();
            if (actual != 0) {
                Wh_Log(L"set main window");
                setMainWindowThread(actual);
            } else Wh_Log(L"no main window yet; none active for thread");
            return actual;
        } else {
            // We only return the main window if the current thread owns it
            // (i.e., if GetFocus would return the main window, given it was focused)
            if (g_uiThreadId == GetCurrentThreadId()) return g_thisProgramWindow;
            return NULL;
        }
    }
    return pOriginalGetFocus();
}

// Unsure of whether this is important, included for completeness
using IsWindowEnabled_t = decltype(&IsWindowEnabled);
IsWindowEnabled_t pOriginalIsWindowEnabled;
BOOL WINAPI IsWindowEnabledHook(HWND hWnd) {
    // Wh_Log(L"IsEnabled check");
    if (g_enabledHooks && g_thisProgramWindow!=0 && g_thisProgramWindow==hWnd) {
        return TRUE;
    } else {
        return pOriginalIsWindowEnabled(hWnd);
    }
}

// ------------------ Hijacking Messages by Subclassing ------------------
// Subclass every window in the process to prevent certain messages (eg WM_KILLFOCUS)
//
// ----- Subclass/Unsubclass Window from Any Thread
// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_ignore-focus-loss");

struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
    SUBCLASSPROC pfnSubclass;
    UINT_PTR uIdSubclass;
    DWORD_PTR dwRefData;
    BOOL result;
};

LRESULT CALLBACK CallWndProcForWindowSubclass(int nCode,
                                              WPARAM wParam,
                                              LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
            SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param =
                (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
            param->result =
                SetWindowSubclass(cwp->hwnd, param->pfnSubclass,
                                  param->uIdSubclass, param->dwRefData);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL SetWindowSubclassFromAnyThread(HWND hWnd,
                                    SUBCLASSPROC pfnSubclass,
                                    UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return FALSE;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcForWindowSubclass,
                                  nullptr, dwThreadId);
    if (!hook) {
        return FALSE;
    }

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (WPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}

// Suppress the messages we want to ignore
// and prevent them from reaching the main windowProc
LRESULT CALLBACK GameWindowSubclassProc(_In_ HWND hWnd,
                                        _In_ UINT uMsg,
                                        _In_ WPARAM wParam,
                                        _In_ LPARAM lParam,
                                        _In_ UINT_PTR uIdSubclass,
                                        _In_ DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_KILLFOCUS:
            if(g_enabledHooks && g_thisProgramWindow == hWnd) {
                Wh_Log(L"KILLFOCUS (ignored)");
                return 0; // ignore KILLFOCUS message
            }
            break;
        case WM_SETFOCUS:
            if (g_enabledHooks && g_thisProgramWindow == 0) {
                setMainWindowThread(hWnd);
                Wh_Log(L"SETFOCUS (set %08X)", (DWORD)(ULONG_PTR)hWnd);
            } else {
                Wh_Log(L"SETFOCUS");
            }
            break;
        case WM_ACTIVATEAPP: // Changed windows to/from different application
            if(wParam == FALSE && g_enabledHooks) return 0; // Deactivation: ignore
            break;
        case WM_ACTIVATE: // Changed windows within same application
            if (((wParam & 0xffff) == WA_INACTIVE) && g_enabledHooks) return 0; //ignore
            break;
        case WM_NCACTIVATE: // Non-client area activation (e.g. titlebar)
            // SDL will check the focus here (ok since we hook GetForegroundWindow)
            // If we don't pass on the message, minimization may break; so don't mess with it
            break;

        default:
            if (uMsg == g_subclassRegisteredMsg && !wParam) {
                WCHAR buf[50];
                GetWindowTextW(hWnd, buf, 50);
                Wh_Log(L"Unsubclassing %08X ... (%s)", (DWORD)(UINT_PTR)hWnd, buf);
                RemoveWindowSubclass(hWnd, GameWindowSubclassProc, 0);
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ------------------ Dealing with 'Message-only' (hidden) windows ------------------
// https://learn.microsoft.com/en-us/windows/win32/winmsg/window-features#message-only-windows

// Don't subclass message-only windows, which may not
// be reached by EnumWindows if the mod is disabled
bool IsGameWindow(HWND hwnd) {
    return GetParent(hwnd)!=HWND_MESSAGE;
}

// Check windows changing 'message-only' status.
// Is this necessary? Probably not, but it's a good idea to try cover all the bases.
using SetParent_t = decltype(&SetParent);
SetParent_t pOriginalSetParent;
HWND WINAPI SetParentHook(HWND hWndChild, HWND hWndNewParent) {
    bool now_messageOnly = (hWndNewParent == HWND_MESSAGE);
    HWND oldparent = pOriginalSetParent(hWndChild, hWndNewParent);
    bool was_messageOnly = (oldparent == HWND_MESSAGE);
    if (now_messageOnly && !was_messageOnly) {
        Wh_Log(L"Disabling subclass %08X", (DWORD)(UINT_PTR)hWndChild);
        SendMessage(hWndChild, g_subclassRegisteredMsg, FALSE, 0);
    }
    if (was_messageOnly && !now_messageOnly) {
        Wh_Log(L"Enabling subclass %08X", (DWORD)(UINT_PTR)hWndChild);
        SetWindowSubclassFromAnyThread(hWndChild, GameWindowSubclassProc, 0, 0);
    }
    return oldparent;
}

// ------------------ Dealing with newly created windows ------------------
// Hooks for creating and destroying new windows to ensure that
// new windows created after the mod is initially enabled get subclassed
using CreateWindowExA_t = decltype(&CreateWindowExA);
CreateWindowExA_t pOriginalCreateWindowExA;
HWND WINAPI CreateWindowExAHook(DWORD dwExStyle,LPCSTR lpClassName,LPCSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam)
{
    HWND hWnd = pOriginalCreateWindowExA(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
    if (!hWnd) return hWnd;
    if (hWndParent == HWND_MESSAGE) return hWnd; // do not subclass message-only window
    Wh_Log(L"New window subclassed: %08X (%s)", (DWORD)(UINT_PTR)hWnd, lpWindowName);
    SetWindowSubclassFromAnyThread(hWnd, GameWindowSubclassProc, 0, 0);
    return hWnd;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pOriginalCreateWindowExW;
HWND WINAPI CreateWindowExWHook(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam)
{
    HWND hWnd = pOriginalCreateWindowExW(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
    if (!hWnd) return hWnd;
    if (hWndParent == HWND_MESSAGE) return hWnd; // do not subclass message-only window
    SetWindowSubclassFromAnyThread(hWnd, GameWindowSubclassProc, 0, 0);
    Wh_Log(L"New window subclassed: %08X (%s)", (DWORD)(UINT_PTR)hWnd, lpWindowName);
    return hWnd;
}

// ------------------ Handle destruction of main window ------------------
// prevent returning handles to the dead window
using DestroyWindow_t = decltype(&DestroyWindow);
DestroyWindow_t pOriginalDestroyWindow;
BOOL DestroyWindowHook(HWND hWnd) {
    if (g_thisProgramWindow == hWnd) {
        Wh_Log(L"Main window destroyed");
        g_thisProgramWindow = 0;
    }
    return pOriginalDestroyWindow(hWnd);
}

// ------------------ Dealing with existing windows ------------------
// We subclass all existing windows when the mod is enabled
// while the program is already running
BOOL CALLBACK InitialEnumGameWindowsFunc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
        return TRUE;
    }

    if (IsGameWindow(hWnd)) {
        WCHAR buf[50];
        GetWindowTextW(hWnd, buf, 50); // sends a WM_GETTEXT msg, returns chars written
        Wh_Log(L"Existing window subclassed: %08X (%s)", (DWORD)(UINT_PTR)hWnd, buf);
        SetWindowSubclassFromAnyThread(hWnd, GameWindowSubclassProc, 0, 0);
    }

    return TRUE;
}

// ------------------ Clean up when mod is disabled ------------------
// Send a message to each windowProc in the process
// If it was subclassed, it will process the message and unsubclass itself
BOOL CALLBACK EnumGameWindowsUnsubclassFunc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
        return TRUE;
    }

    if (IsGameWindow(hWnd)) {
        Wh_Log(L"Unsubclass: %08X", (DWORD)(UINT_PTR)hWnd);
        SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
    }

    return TRUE;
}

// We delegate checking whether the mod should load / apply hooks to the Windhawk runtime.
// Program list is modifiable by "Custom program include list" under Advanced.
// void LoadSettings() {
// }

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    // LoadSettings();

    // Ensure subclass will be added to newly created windows
    Wh_SetFunctionHook((void*)CreateWindowExA, (void*)CreateWindowExAHook,
                       (void**)&pOriginalCreateWindowExA);
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook,
                       (void**)&pOriginalCreateWindowExW);
    // Ensure if the 'main window' is destroyed, we will no longer return handles to it
    // and can instead set a new 'main window'
    Wh_SetFunctionHook((void*)DestroyWindow, (void*)DestroyWindowHook,
                       (void**)&pOriginalDestroyWindow);
    // Handle window changing 'message-only' status
    Wh_SetFunctionHook((void*)SetParent, (void*)SetParentHook,
                       (void**)&pOriginalSetParent);


    // Add subclass to existing windows
    EnumWindows(InitialEnumGameWindowsFunc, 0);

    // Add hooks for other functions
    Wh_SetFunctionHook((void*)GetForegroundWindow, (void*)GetForegroundWindowHook,
                       (void**)&pOriginalGetForegroundWindow);
    Wh_SetFunctionHook((void*)GetActiveWindow, (void*)GetActiveWindowHook,
                       (void**)&pOriginalGetActiveWindow);
    Wh_SetFunctionHook((void*)GetFocus, (void*)GetFocusHook,
                       (void**)&pOriginalGetFocus);
    //Wh_SetFunctionHook((void*)IsWindowEnabled, (void*)IsWindowEnabledHook,
    //                   (void**)&pOriginalIsWindowEnabled);
    g_enabledHooks = true;

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit...");
    
    g_thisProgramWindow = 0;
    g_enabledHooks = false;
    // Unsubclass all windows
    EnumWindows(EnumGameWindowsUnsubclassFunc, 0);
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    // Wh_Log(L"SettingsChanged");
    // LoadSettings();
}

