// ==WindhawkMod==
// @id              explorer-view-menu-unlocker
// @name            Explorer View Menu Unlocker
// @description     Unlocks the grayed-out view options in the Explorer/Control Panel menu bars and forcefully applies them using targeted window hierarchy traversal.
// @version         1.0.2
// @author          TheShadyRainbow4
// @github          https://github.com/TheShadyRainbow4
// @homepage        https://main.elitesoftwaretech.cc
// @include         explorer.exe
// @include         control.exe
// @compilerOptions -luser32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer View Menu Unlocker
Modern Windows intentionally disables (grays out) standard view options in the Menu Bar when navigating specific system folders like the Classic Control Panel. (This mostly applies to Windows 11)

### How it Works (v1.0.2 Update):
1. **Visual Unlock:** Intercepts `EnableMenuItem` and `SetMenuItemInfoW` to block the operating system from graying out the View options.
2. **Targeted Execution:** When you click a view option, the mod traverses the window tree specifically searching for `SHELLDLL_DefView` (the main file host) to ensure it targets the correct visible list.
3. **Dual-Strike Application:** It sends the native command to the Shell host, and then brute-forces the raw `LVM_SETVIEW` instruction directly onto the icon list control, ensuring the view changes regardless of Shell restrictions.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableMod: true
  $name: "Enable Menu Unlocker"
  $description: "Master switch to enable or disable the forced un-graying and application of menu items."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>

// -------------------------------------------------------------------------
// Global Settings
// -------------------------------------------------------------------------
struct {
    bool enableMod;
} settings;

// Standard Windows Explorer View Command IDs
const UINT CMD_LARGE_ICONS      = 28713;
const UINT CMD_SMALL_ICONS      = 28714;
const UINT CMD_LIST             = 28715;
const UINT CMD_DETAILS_OLD      = 28716;
const UINT CMD_THUMBNAILS       = 28717;
const UINT CMD_TILES            = 28718;
const UINT CMD_DETAILS          = 28747;
const UINT CMD_MEDIUM_ICONS     = 28749;
const UINT CMD_CONTENT          = 28750;
const UINT CMD_EXTRA_LARGE      = 28751;

// -------------------------------------------------------------------------
// Helper: Is View Command
// -------------------------------------------------------------------------
bool IsViewCommand(UINT id) {
    return (id >= CMD_LARGE_ICONS && id <= CMD_TILES) || 
           (id >= CMD_DETAILS && id <= CMD_EXTRA_LARGE);
}

// -------------------------------------------------------------------------
// Traversal Callbacks: Finding the CORRECT List View
// -------------------------------------------------------------------------

// 1. Finds the main Shell View host
BOOL CALLBACK FindShellViewProc(HWND hwnd, LPARAM lParam) {
    wchar_t className[256];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
            *(HWND*)lParam = hwnd;
            return FALSE; // Found it, stop searching
        }
    }
    return TRUE; 
}

// 2. Finds the actual List View inside the Shell View
BOOL CALLBACK FindListViewProc(HWND hwnd, LPARAM lParam) {
    wchar_t className[256];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        if (wcscmp(className, L"SysListView32") == 0) {
            *(HWND*)lParam = hwnd;
            return FALSE; // Found it, stop searching
        }
    }
    return TRUE; 
}

// -------------------------------------------------------------------------
// Subclass Procedure: CabinetWClass (Main Explorer Frame)
// -------------------------------------------------------------------------
LRESULT CALLBACK CabinetSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_COMMAND && settings.enableMod) {
        WORD wID = LOWORD(wParam);
        
        // If the user clicked a View option from the top menu bar...
        if (IsViewCommand(wID)) {
            
            // Step 1: Find the actual Shell Host, not just any random child window
            HWND hShellView = NULL;
            EnumChildWindows(hWnd, FindShellViewProc, (LPARAM)&hShellView);
            
            if (hShellView) {
                // Strike 1: Ask the Shell to execute the command natively
                SendMessageW(hShellView, WM_COMMAND, wParam, lParam);

                // Strike 2: Brute force the underlying control to guarantee visual update
                int targetViewMode = -1;
                switch (wID) {
                    case CMD_EXTRA_LARGE: 
                    case CMD_LARGE_ICONS:  targetViewMode = LV_VIEW_ICON; break;
                    case CMD_TILES:        targetViewMode = LV_VIEW_TILE; break;
                    case CMD_MEDIUM_ICONS: 
                    case CMD_SMALL_ICONS:  targetViewMode = LV_VIEW_SMALLICON; break;
                    case CMD_LIST:         targetViewMode = LV_VIEW_LIST; break;
                    case CMD_DETAILS: 
                    case CMD_DETAILS_OLD:  targetViewMode = LV_VIEW_DETAILS; break;
                }

                if (targetViewMode != -1) {
                    HWND hListView = NULL;
                    // Search ONLY inside the Shell Host we just found
                    EnumChildWindows(hShellView, FindListViewProc, (LPARAM)&hListView);
                    
                    if (hListView) {
                        SendMessageW(hListView, LVM_SETVIEW, targetViewMode, 0);
                        Wh_Log(L"Successfully routed view change to visible SysListView32.");
                    }
                }
                return 0; // Completely block the shell's default rejection behavior
            }
        }
    }

    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, CabinetSubclassProc, uIdSubclass);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// -------------------------------------------------------------------------
// Hook: EnableMenuItem
// Blocks simple attempts to gray out our target menu items.
// -------------------------------------------------------------------------
using EnableMenuItem_t = decltype(&EnableMenuItem);
EnableMenuItem_t EnableMenuItem_Original;

BOOL WINAPI EnableMenuItem_Hook(HMENU hMenu, UINT uIDEnableItem, UINT uEnable) {
    if (settings.enableMod) {
        UINT id = uIDEnableItem;
        
        if (uEnable & MF_BYPOSITION) {
            id = GetMenuItemID(hMenu, uIDEnableItem);
        }

        if (IsViewCommand(id)) {
            uEnable &= ~(MF_DISABLED | MF_GRAYED);
            uEnable |= MF_ENABLED;
        }
    }
    return EnableMenuItem_Original(hMenu, uIDEnableItem, uEnable);
}

// -------------------------------------------------------------------------
// Hook: SetMenuItemInfoW
// Blocks complex attempts to gray out menu items.
// -------------------------------------------------------------------------
using SetMenuItemInfoW_t = decltype(&SetMenuItemInfoW);
SetMenuItemInfoW_t SetMenuItemInfoW_Original;

BOOL WINAPI SetMenuItemInfoW_Hook(HMENU hmenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmii) {
    if (settings.enableMod && lpmii) {
        UINT id = fByPosition ? GetMenuItemID(hmenu, item) : item;
        
        if (IsViewCommand(id)) {
            MENUITEMINFOW mii = *lpmii;
            
            if (mii.fMask & MIIM_STATE) {
                mii.fState &= ~(MFS_DISABLED | MFS_GRAYED);
                mii.fState |= MFS_ENABLED;
            }
            
            return SetMenuItemInfoW_Original(hmenu, item, fByPosition, &mii);
        }
    }
    return SetMenuItemInfoW_Original(hmenu, item, fByPosition, lpmii);
}

// -------------------------------------------------------------------------
// Hook: CreateWindowExW
// Injects our subclass into the main Explorer frame to catch menu clicks.
// -------------------------------------------------------------------------
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (settings.enableMod && hWnd != NULL && lpClassName != NULL) {
        if (((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xFFFF) != 0) {
            
            if (wcscmp(lpClassName, L"CabinetWClass") == 0) {
                SetWindowSubclass(hWnd, CabinetSubclassProc, 1001, 0);
            }
        }
    }

    return hWnd;
}

// -------------------------------------------------------------------------
// Windhawk Lifecycle Management
// -------------------------------------------------------------------------
void LoadSettings() {
    settings.enableMod = Wh_GetIntSetting(L"enableMod");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init Explorer View Menu Unlocker (Targeted)");
    LoadSettings();

    Wh_SetFunctionHook((void*)EnableMenuItem, (void*)EnableMenuItem_Hook, (void**)&EnableMenuItem_Original);
    Wh_SetFunctionHook((void*)SetMenuItemInfoW, (void*)SetMenuItemInfoW_Hook, (void**)&SetMenuItemInfoW_Original);
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit Explorer View Menu Unlocker");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}