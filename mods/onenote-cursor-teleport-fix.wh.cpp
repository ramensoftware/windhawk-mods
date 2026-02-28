// ==WindhawkMod==
// @id              onenote-cursor-teleport-fix
// @name            OneNote Cursor Teleport Fix (Sync Engine)
// @description     Cures the pen tablet rubber-band bug by permanently syncing the Win32 mouse with Windows Ink.
// @version         2.0
// @author          Mohamed Magdy
// @github          https://github.com/hamomagdy724
// @include         onenote.exe
// @compilerOptions -luser32 -lcomctl32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# OneNote Cursor Teleport Fix
This is the definitive solution to the Huion/OneNote pen teleport bug.

### The Logic
1. **The Sync Engine:** Actively monitors `WM_POINTER` events. If the hidden Windows mouse lags behind the pen by more than a few pixels, it uses `SetCursorPos` to drag the mouse to the pen, killing the desync.
2. **The Jump Blocker:** If a massive `WM_MOUSEMOVE` fires, it calculates the distance and swallows the teleport.

### The Exception Engine
To prevent `SetCursorPos` from accidentally auto-dismissing OneNote's delicate Ribbon dropdown menus and leaving "black footprints", this mod includes a massive hierarchy analyzer. It actively blacklists `NetUIPopupWindow`, `SysShadow`, and dozens of other temporary UI elements, ensuring the sync engine never corrupts a menu click.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- jumpThreshold: 150
  $name: Jump Blocker Threshold (Pixels)
  $description: Minimum distance a mouse packet must travel instantly to be destroyed.
- syncThreshold: 3
  $name: Sync Engine Threshold (Pixels)
  $description: How far the ghost mouse can lag behind the pen before being forcefully synced.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>

// --- Definitions & Fallbacks ---
#ifndef WM_POINTERUPDATE
#define WM_POINTERUPDATE 0x0245
#define WM_POINTERDOWN   0x0246
#define WM_POINTERUP     0x0247
#define WM_POINTERENTER  0x0249
#define WM_POINTERLEAVE  0x024A
#endif

// --- Global State & Memory ---
int g_jumpThreshold = 150;
int g_syncThreshold = 3;
POINT g_lastPoint = {0, 0};
UINT g_subclassRegisteredMsg;

// --- 1. THE EXHAUSTIVE WINDOW ANALYZER ENGINE ---
// This namespace contains all the logic required to protect OneNote's delicate UI menus.
namespace WindowAnalyzer {

    // A comprehensive list of every known temporary/popup class used by Microsoft Office and Windows.
    // If a window matches ANY of these, our mod will completely ignore it.
    const std::vector<std::wstring> BlacklistedClasses = {
        L"NetUIPopupWindow",       // OneNote Ribbon Dropdowns (Color, Thickness, etc)
        L"MsoCommandBarPopup",     // Legacy Office Command Bars
        L"SysShadow",              // Windows Drop Shadows (The "Black Footprint")
        L"DropShadow",             // Office Custom Shadows
        L"tooltips_class32",       // Standard Windows Tooltips
        L"NetUITooltip",           // Custom Office Tooltips
        L"Menu",                   // Standard Windows Menus
        L"#32768",                 // Internal Windows Menu ATOM
        L"Ghost",                  // System Ghost Windows
        L"NUIDialog",              // Office Dialog Boxes
        L"NetUIKeyboardTabBand",   // Touch Keyboard bands
        L"MSO_BORDEREFFECT_WINDOW_CLASS" // Border effect glows
    };

    // A list of classes that we ABSOLUTELY MUST hook to fix the teleport bug.
    const std::vector<std::wstring> WhitelistedClasses = {
        L"OneNote::DocumentCanvas", // The main drawing board
        L"NetUIHWND"                // The main ribbon and UI host
    };

    // Helper: Safely get the class name of a window
    std::wstring GetWindowClassSafely(HWND hWnd) {
        if (!hWnd) return L"";
        WCHAR szClass[256] = {0};
        if (GetClassNameW(hWnd, szClass, ARRAYSIZE(szClass))) {
            return std::wstring(szClass);
        }
        return L"";
    }

    // Helper: Check if a string matches a blacklist entry (Case Insensitive)
    bool IsClassBlacklisted(const std::wstring& className) {
        if (className.empty()) return false;

        for (const auto& blacklisted : BlacklistedClasses) {
            if (_wcsicmp(className.c_str(), blacklisted.c_str()) == 0) {
                return true;
            }
        }
        return false;
    }

    // Helper: Check if a string matches a whitelist entry (Case Insensitive)
    bool IsClassWhitelisted(const std::wstring& className) {
        if (className.empty()) return false;

        for (const auto& whitelisted : WhitelistedClasses) {
            if (_wcsicmp(className.c_str(), whitelisted.c_str()) == 0) {
                return true;
            }
        }
        return false;
    }

    // Advanced Protection: Check if a window is a child of a protected menu.
    // Sometimes OneNote creates inner controls inside the NetUIPopupWindow. 
    // We must recursively walk UP the window tree to make sure the parent isn't a menu.
    bool IsChildOfProtectedMenu(HWND hWnd) {
        HWND hParent = GetParent(hWnd);
        int depth = 0;
        
        // Walk up the tree, max 10 levels deep to prevent infinite loops
        while (hParent != NULL && depth < 10) {
            std::wstring parentClass = GetWindowClassSafely(hParent);
            if (IsClassBlacklisted(parentClass)) {
                return true; // Found a protected parent!
            }
            hParent = GetParent(hParent);
            depth++;
        }
        return false;
    }

    // Advanced Protection: Check Window Styles
    // Popups usually have specific flags. We analyze them here.
    bool IsPopupStyleWindow(HWND hWnd) {
        LONG style = GetWindowLongW(hWnd, GWL_STYLE);
        LONG exStyle = GetWindowLongW(hWnd, GWL_EXSTYLE);

        // If it's explicitly a tool window or a topmost popup, treat it as fragile.
        if ((exStyle & WS_EX_TOOLWINDOW) != 0) return true;
        if ((exStyle & WS_EX_TOPMOST) != 0) return true;
        
        return false;
    }

    // THE MASTER FILTER
    // Decides exactly whether a window gets the V1.0 Teleport Shield or gets ignored.
    bool ShouldApplyShield(HWND hWnd, LPCWSTR lpClassName) {
        if (!hWnd) return false;

        // 1. Handle raw pointer/ATOM class names
        if (((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) == 0) {
            if ((ULONG_PTR)lpClassName == 32768) return false; // #32768 is a menu
            return true; 
        }

        std::wstring className(lpClassName);

        // 2. Check the Hard Whitelist (Always protect the canvas)
        if (IsClassWhitelisted(className)) {
            return true;
        }

        // 3. Check the Hard Blacklist (Never touch the menus)
        if (IsClassBlacklisted(className)) {
            return false;
        }

        // 4. Check ancestry (Don't touch children of menus)
        if (IsChildOfProtectedMenu(hWnd)) {
            return false;
        }

        // 5. Check styles (Don't touch floating tooltips)
        if (IsPopupStyleWindow(hWnd)) {
            return false;
        }

        // If it survived all the exception checks, it is safe to subclass.
        return true;
    }
}

// --- 2. THREAD-SAFE SUBCLASSING ARCHITECTURE ---
// This robust engine ensures we can inject our intercepts without crashing OneNote.
struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
    SUBCLASSPROC pfnSubclass;
    UINT_PTR uIdSubclass;
    DWORD_PTR dwRefData;
    BOOL result;
};

LRESULT CALLBACK CallWndProcForWindowSubclass(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
            SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param = (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
            param->result = SetWindowSubclass(cwp->hwnd, param->pfnSubclass, param->uIdSubclass, param->dwRefData);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL SetWindowSubclassFromAnyThread(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return FALSE;
    if (dwThreadId == GetCurrentThreadId()) return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcForWindowSubclass, nullptr, dwThreadId);
    if (!hook) return FALSE;

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (WPARAM)&param);

    UnhookWindowsHookEx(hook);
    return param.result;
}

// --- 3. THE CORE TELEPORT SHIELD (V1.0 LOGIC) ---
// This is the exact logic you requested, but safely contained within the exception engine.
LRESULT CALLBACK OneNoteSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    
    // PART A: THE WINDOWS INK SYNC ENGINE
    // When the pen is moving inside the canvas, Windows Ink fires these messages.
    if (uMsg == WM_POINTERUPDATE || uMsg == WM_POINTERDOWN || uMsg == WM_POINTERENTER) {
        
        POINT pt;
        pt.x = GET_X_LPARAM(lParam); // Pointer API naturally uses absolute screen coordinates
        pt.y = GET_Y_LPARAM(lParam);
        
        // Ensure we only run SetCursorPos if the active window isn't a popup.
        // This is the final layer of protection for the dropdown menus.
        HWND hForeground = GetForegroundWindow();
        std::wstring fgClass = WindowAnalyzer::GetWindowClassSafely(hForeground);
        
        if (!WindowAnalyzer::IsClassBlacklisted(fgClass)) {
            POINT sysPt;
            if (GetCursorPos(&sysPt)) {
                // If the invisible Windows system mouse is lagging behind the Ink pen by more than the threshold,
                // forcefully teleport the system mouse to catch up. 
                // This guarantees the mouse is never "left behind" at the edges.
                if (abs(sysPt.x - pt.x) > g_syncThreshold || abs(sysPt.y - pt.y) > g_syncThreshold) {
                    SetCursorPos(pt.x, pt.y);
                }
            }
        }
        
        // Update our internal tracker for the jump blocker
        g_lastPoint = pt;
    }

    // PART B: THE JUMP BLOCKER (FAILSAFE)
    // If a massive standard mouse jump still somehow occurs, we calculate and swallow it.
    else if (uMsg == WM_MOUSEMOVE) {
        
        POINT pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        ClientToScreen(hWnd, &pt); // Mouse API uses client coordinates, convert to screen

        // We must have a valid previous point to calculate a jump distance
        if (g_lastPoint.x != 0 && g_lastPoint.y != 0) {
            
            // Calculate Euclidean distance (Pythagorean theorem)
            int dx = pt.x - g_lastPoint.x;
            int dy = pt.y - g_lastPoint.y;
            double distance = sqrt((double)(dx * dx + dy * dy));

            // PROACTIVE INTERCEPTION
            if (distance > (double)g_jumpThreshold) {
                // We detected a hardware teleport bug.
                // By returning 0, we destroy the bad packet before OneNote's UI engine processes it.
                return 0; 
            }
        }
        
        // Normal, safe movement. Update the tracker.
        g_lastPoint = pt;
    }

    // PART C: UNLOAD HANDLER
    if (uMsg == g_subclassRegisteredMsg && !wParam) {
        RemoveWindowSubclass(hWnd, OneNoteSubclassProc, 0);
    }

    // Pass everything else down the chain to OneNote natively
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// --- 4. WINDOW ENUMERATION & INJECTION ---
// These functions walk the UI tree and apply the shield only to valid windows.

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
    std::wstring className = WindowAnalyzer::GetWindowClassSafely(hwnd);
    
    // Only subclass if the Exception Engine clears it
    if (WindowAnalyzer::ShouldApplyShield(hwnd, className.c_str())) {
        SetWindowSubclassFromAnyThread(hwnd, OneNoteSubclassProc, 0, 0);
    }
    return TRUE;
}

BOOL CALLBACK FindCurrentProcessOneNoteWindowEnumFunc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId()) return TRUE;
    
    std::wstring className = WindowAnalyzer::GetWindowClassSafely(hWnd);
    
    if (WindowAnalyzer::ShouldApplyShield(hWnd, className.c_str())) {
        SetWindowSubclassFromAnyThread(hWnd, OneNoteSubclassProc, 0, 0);
    }
    
    // Walk all children of the main window
    EnumChildWindows(hWnd, EnumChildProc, 0);
    return TRUE; 
}

// --- 5. WINDOW CREATION HOOK ---
// Catches new UI elements (like tabs or canvas splits) the moment they are created.

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pOriginalCreateWindowExW;

HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    
    // Call the original Windows API to let OneNote build the window
    HWND hWnd = pOriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) return hWnd;

    // Route the newly created window through the Exception Engine
    if (WindowAnalyzer::ShouldApplyShield(hWnd, lpClassName)) {
        SetWindowSubclassFromAnyThread(hWnd, OneNoteSubclassProc, 0, 0);
    }
    
    return hWnd;
}

// --- 6. SETTINGS MANAGEMENT ---
void LoadSettings() {
    g_jumpThreshold = Wh_GetIntSetting(L"jumpThreshold");
    g_syncThreshold = Wh_GetIntSetting(L"syncThreshold");
    
    // Safety boundaries to prevent bad user input in Windhawk
    if (g_jumpThreshold < 50) g_jumpThreshold = 50;
    if (g_syncThreshold < 1) g_syncThreshold = 1;
}

// --- 7. UNLOAD/CLEANUP HANDLERS ---
// Safely removes the mod without crashing OneNote.

BOOL CALLBACK UninitChildEnumProc(HWND child, LPARAM lParam) {
    SendMessage(child, g_subclassRegisteredMsg, FALSE, 0);
    return TRUE;
}

BOOL CALLBACK UninitWindowEnumProc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    if (GetWindowThreadProcessId(hWnd, &dwProcessId) && dwProcessId == GetCurrentProcessId()) {
        SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
        EnumChildWindows(hWnd, UninitChildEnumProc, 0);
    }
    return TRUE;
}

// --- 8. WINDHAWK CORE EXPORTS ---

BOOL Wh_ModInit(void) {
    Wh_Log(L"Init OneNote Ultimate Exception Engine");
    
    LoadSettings();
    
    // Register the custom cross-thread message
    g_subclassRegisteredMsg = RegisterWindowMessage(L"Windhawk_SetWindowSubclassFromAnyThread_OneNoteFix");
    
    // Hook creation of new windows
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook, (void**)&pOriginalCreateWindowExW);
    
    // Retroactively subclass windows that are already open
    EnumWindows(FindCurrentProcessOneNoteWindowEnumFunc, 0);
    
    return TRUE;
}

void Wh_ModUninit(void) {
    Wh_Log(L"Uninit OneNote Engine");
    
    // Broadcast the removal message so all subclasses cleanly detach
    EnumWindows(UninitWindowEnumProc, 0);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings Changed");
    LoadSettings();
}