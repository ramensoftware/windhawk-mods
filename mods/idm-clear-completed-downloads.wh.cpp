// ==WindhawkMod==
// @id              idm-clear-completed-downloads
// @name            IDM Clear Completed Downloads
// @description     This mode cleans up as soon as the IDM window appears on the screen.
// @version         0.5.4
// @author          BCRTVKCS
// @github          https://github.com/bcrtvkcs
// @twitter         https://x.com/bcrtvkcs
// @homepage        https://grdigital.pro
// @include         idman.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# IDM Clear Completed Downloads

This mod automatically cleans up completed downloads every time you open the [Internet Download Manager (IDM)](https://www.internetdownloadmanager.com/) window.

## What Does It Do?

When IDM's main window opens, the mod kicks in and automatically performs the following:

1. **Deletes completed downloads** - Triggers IDM's "Delete completed downloads" command
2. **Auto-dismisses the confirmation dialog** - Hides the confirmation popup and clicks "Yes"
3. **Targets only the main window** - Ignores other IDM windows like Settings, download dialogs, etc.

## How It Works

The mod hooks the `ShowWindow` function in the Windows API. When IDM makes a window visible:

- Checks if the window's class name is `#32770` (standard dialog class)
- Verifies the window is **ownerless** (no owner) and **has a menu bar**
- Only the IDM main window satisfies both conditions, so other windows are filtered out
- If conditions are met, the cleanup task is launched in a separate thread

## Target Process

- This mod is only for targets the `idman.exe` process and does not affect other programs.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <wchar.h>

// ---------------------------------------------------------
// SETTINGS
// ---------------------------------------------------------
#define IDM_CMD_DELETE_COMPLETED 32794
#define IDYES 6  // Standard Windows IDYES button ID

// TDM_CLICK_BUTTON: the official way to programmatically click a button
// in both native dialogs (#32770) and TaskDialog windows.
// Value: WM_USER + 102 = 0x0466
#define TDM_CLICK_BUTTON (WM_USER + 102)

// Concurrency guard: only one CleanupTask may run at a time
volatile LONG g_cleanupRunning = 0;

// ---------------------------------------------------------
// HELPER: Find a dialog owned by a specific window
// ---------------------------------------------------------
struct PopupSearch {
    HWND hOwner;
    HWND hResult;
};

BOOL CALLBACK FindOwnedDialog(HWND hWnd, LPARAM lParam) {
    PopupSearch* search = (PopupSearch*)lParam;

    if (GetWindow(hWnd, GW_OWNER) == search->hOwner) {
        char className[64];
        GetClassNameA(hWnd, className, sizeof(className));
        if (strcmp(className, "#32770") == 0) {
            search->hResult = hWnd;
            return FALSE;
        }
    }
    return TRUE;
}

// ---------------------------------------------------------
// HELPER THREAD: Handles Cleanup and Confirmation
// ---------------------------------------------------------
// This function runs right after the window is shown.
DWORD WINAPI CleanupTask(LPVOID lpParam) {
    HWND hMainWnd = (HWND)lpParam;

    if (InterlockedCompareExchange(&g_cleanupRunning, 1, 0) != 0) {
        Wh_Log(L"CleanupTask already running, skipping.");
        return 0;
    }

    Sleep(50);

    if (!IsWindowVisible(hMainWnd) || IsIconic(hMainWnd) ||
        GetForegroundWindow() != hMainWnd) {
        Wh_Log(L"Window not visible/foreground, aborting cleanup (likely auto-start).");
        InterlockedExchange(&g_cleanupRunning, 0);
        return 0;
    }

    PostMessageA(hMainWnd, WM_COMMAND, IDM_CMD_DELETE_COMPLETED, 0);

    for (int i = 0; i < 40; i++) {
        Sleep(50);

        PopupSearch search = { hMainWnd, NULL };
        EnumWindows(FindOwnedDialog, (LPARAM)&search);

        if (search.hResult != NULL) {
            PostMessageW(search.hResult, TDM_CLICK_BUTTON, IDYES, 0);
            PostMessageW(search.hResult, WM_COMMAND, IDYES, 0);

            Wh_Log(L"Window opened -> Cleanup done -> Confirmation dismissed.");
            break;
        }
    }

    InterlockedExchange(&g_cleanupRunning, 0);
    return 0;
}

// ---------------------------------------------------------
// HOOK: ShowWindow
// ---------------------------------------------------------
// This runs whenever IDM tries to make a window visible.
typedef BOOL (WINAPI *ShowWindow_t)(HWND hWnd, int nCmdShow);
ShowWindow_t ShowWindow_Original;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    // First, execute the original operation (let the window open)
    BOOL result = ShowWindow_Original(hWnd, nCmdShow);

    // If the window is being made visible (excluding SW_HIDE and SW_MINIMIZE)
    if (nCmdShow != SW_HIDE && nCmdShow != SW_MINIMIZE) {

        // Is this window the IDM main window?
        char className[256];
        GetClassNameA(hWnd, className, sizeof(className));

        if (strcmp(className, "#32770") == 0) {
            // IDM main window check:
            // 1. Must be ownerless - dialog windows are owned by the main window
            // 2. Must have a menu bar - the main window has Tasks/File/Downloads menus
            HWND hOwner = GetWindow(hWnd, GW_OWNER);
            HMENU hMenu = GetMenu(hWnd);

            if (hOwner == NULL && hMenu != NULL) {
                // Start the cleanup in a separate thread to avoid freezing the UI
                HANDLE hThread = CreateThread(NULL, 0, CleanupTask, (LPVOID)hWnd, 0, NULL);
                if (hThread) CloseHandle(hThread);
                Wh_Log(L"IDM main window detected, starting cleanup.");
            }
        }
    }

    return result;
}

// ---------------------------------------------------------
// INITIALIZATION
// ---------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"IDM Completed Downloads Cleaner initialized.");

    Wh_SetFunctionHook(
        (void*)ShowWindow,
        (void*)ShowWindow_Hook,
        (void**)&ShowWindow_Original
    );

    return TRUE;
}
