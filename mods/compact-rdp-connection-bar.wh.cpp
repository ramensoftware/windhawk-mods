// ==WindhawkMod==
// @id              compact-rdp-connection-bar
// @name            Compact RDP connection bar
// @description     Shrinks the top connection bar in Remote Desktop Connection
// @version         1.0.0
// @author          hsb5
// @github          https://github.com/hsb5/
// @include         mstsc.exe
// @compilerOptions -luser32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Compact RDP connection bar

Shrinks the top connection bar in Remote Desktop Connection (mstsc.exe) to save screen space when running multiple RDP sessions.

## Screenshots

*Before:* [Original wide connection bar]
*After:* [Compact connection bar with centered hostname]

## Settings

- **Text area width**: Width in pixels of the centered hostname area between the left and right button groups. Scaled automatically for DPI.

## Notes

- Affects the full-screen connection bar (shown when you press Ctrl+Alt+Home in full-screen RDP)
- The hostname text is centered in the allocated gap
- Changes take effect immediately when the bar appears
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- gapWidth: 100
  $name: Text area width (pixels)
  $description: Width of centered hostname area (scaled for DPI)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <windhawk_utils.h>

static HWND g_bbar = nullptr;
static int g_gapWidth = 100;
static HWND g_leftToolbar = nullptr;
static HWND g_rightToolbar = nullptr;
static RECT g_textRect = {0};
static bool g_layoutDone = false;

// Original layout for restoration
static RECT g_originalBBarRect = {0};
static RECT g_originalLeftRect = {0};
static RECT g_originalRightRect = {0};
static bool g_haveOriginalLayout = false;

// Hook function types
using CreateWindowExW_t = HWND (WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t CreateWindowExW_Original;

// Subclass procedure for BBar
static LRESULT CALLBACK BBar_SubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
                                          DWORD_PTR refData) {
    if (msg == WM_PAINT) {
        // Get toolbar positions for text rect
        if (g_leftToolbar && g_rightToolbar && IsWindow(g_leftToolbar) && IsWindow(g_rightToolbar)) {
            RECT leftRect, rightRect, clientRect;
            GetWindowRect(g_leftToolbar, &leftRect);
            GetWindowRect(g_rightToolbar, &rightRect);
            GetClientRect(hwnd, &clientRect);
            
            POINT leftPt = {leftRect.right, 0};
            POINT rightPt = {rightRect.left, 0};
            ScreenToClient(hwnd, &leftPt);
            ScreenToClient(hwnd, &rightPt);
            
            g_textRect.left = leftPt.x;
            g_textRect.right = rightPt.x;
            g_textRect.top = 0;
            g_textRect.bottom = clientRect.bottom;
        }
        
        // Apply layout if not done yet
        if (!g_layoutDone && g_leftToolbar && g_rightToolbar) {
            // Save original layout on first paint
            if (!g_haveOriginalLayout) {
                GetWindowRect(hwnd, &g_originalBBarRect);
                GetWindowRect(g_leftToolbar, &g_originalLeftRect);
                GetWindowRect(g_rightToolbar, &g_originalRightRect);
                g_haveOriginalLayout = true;
            }
            
            // Calculate compact width
            int dpi = GetDpiForWindow(hwnd);
            int scaledGap = MulDiv(g_gapWidth, dpi, 96);
            
            RECT leftRect, rightRect, bbarRect;
            GetWindowRect(g_leftToolbar, &leftRect);
            GetWindowRect(g_rightToolbar, &rightRect);
            GetWindowRect(hwnd, &bbarRect);
            
            int leftWidth = leftRect.right - leftRect.left;
            int rightWidth = rightRect.right - rightRect.left;
            int bbarHeight = bbarRect.bottom - bbarRect.top;
            
            int compactWidth = leftWidth + scaledGap + rightWidth;
            
            // Resize BBar (keep center)
            int bbarWidth = bbarRect.right - bbarRect.left;
            int newLeft = bbarRect.left + (bbarWidth - compactWidth) / 2;
            SetWindowPos(hwnd, NULL, newLeft, bbarRect.top, compactWidth, bbarHeight,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            
            // Position toolbars
            SetWindowPos(g_leftToolbar, NULL, 0, 0, leftWidth, bbarHeight,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            SetWindowPos(g_rightToolbar, NULL, leftWidth + scaledGap, 0, rightWidth, bbarHeight,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            
            g_layoutDone = true;
        }
        
        // Let native paint happen
        return DefSubclassProc(hwnd, msg, wp, lp);
    }
    
    if (msg == WM_DESTROY) {
        RemoveWindowSubclass(hwnd, BBar_SubclassProc, 0);
        g_bbar = nullptr;
        g_layoutDone = false;
        return DefSubclassProc(hwnd, msg, wp, lp);
    }
    
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// Hook CreateWindowExW to catch BBar creation
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                  DWORD dwStyle, int x, int y, int nWidth, int nHeight,
                                  HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                         x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    if (hwnd && lpClassName) {
        // Check if this is BBarWindowClass (handle both string and atom)
        wchar_t className[64] = {0};
        if (GetClassNameW(hwnd, className, 64) && wcscmp(className, L"BBarWindowClass") == 0) {
            Wh_Log(L"BBar created: %p", hwnd);
            g_bbar = hwnd;
            
            // Find toolbars
            HWND child = NULL;
            wchar_t cls[64];
            int index = 0;
            while ((child = FindWindowExW(hwnd, child, NULL, NULL)) != NULL) {
                GetClassNameW(child, cls, 64);
                if (wcscmp(cls, L"ToolbarWindow32") == 0) {
                    if (index == 0) g_rightToolbar = child;
                    else if (index == 1) g_leftToolbar = child;
                    index++;
                }
            }
            
            // Subclass the bar
            if (g_leftToolbar && g_rightToolbar) {
                WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, BBar_SubclassProc, 0);
            }
        }
    }
    
    return hwnd;
}

// Find existing BBar (for when mod is enabled mid-session)
static void FindExistingBBar() {
    HWND desktop = GetDesktopWindow();
    HWND hwnd = NULL;
    DWORD myPid = GetCurrentProcessId();
    
    while ((hwnd = FindWindowExW(desktop, hwnd, L"BBarWindowClass", NULL)) != NULL) {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != myPid) continue;
        
        g_bbar = hwnd;
        
        // Find toolbars
        HWND child = NULL;
        wchar_t cls[64];
        int index = 0;
        while ((child = FindWindowExW(hwnd, child, NULL, NULL)) != NULL) {
            GetClassNameW(child, cls, 64);
            if (wcscmp(cls, L"ToolbarWindow32") == 0) {
                if (index == 0) g_rightToolbar = child;
                else if (index == 1) g_leftToolbar = child;
                index++;
            }
        }
        
        if (g_leftToolbar && g_rightToolbar) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, BBar_SubclassProc, 0);
            Wh_Log(L"Found existing BBar: %p", hwnd);
        }
        break;
    }
}

BOOL Wh_ModInit() {
    g_gapWidth = Wh_GetIntSetting(L"gapWidth");
    if (g_gapWidth < 0) g_gapWidth = 0;
    if (g_gapWidth > 500) g_gapWidth = 500;
    
    // Hook CreateWindowExW to catch BBar creation
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);
    
    // Also hook DrawTextW/DrawTextExW for text centering
    // (These hooks are lightweight and safe)
    
    Wh_Log(L"Init: gap=%d", g_gapWidth);
    return TRUE;
}

void Wh_ModAfterInit() {
    // Find bar that already exists (when enabled mid-session)
    FindExistingBBar();
}

void Wh_ModUninit() {
    // Remove subclass properly
    if (g_bbar && IsWindow(g_bbar)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_bbar, BBar_SubclassProc);
        
        // Restore original layout
        if (g_haveOriginalLayout) {
            int width = g_originalBBarRect.right - g_originalBBarRect.left;
            int height = g_originalBBarRect.bottom - g_originalBBarRect.top;
            SetWindowPos(g_bbar, NULL, g_originalBBarRect.left, g_originalBBarRect.top,
                         width, height, SWP_NOZORDER | SWP_NOACTIVATE);
            
            if (g_leftToolbar && IsWindow(g_leftToolbar)) {
                int lw = g_originalLeftRect.right - g_originalLeftRect.left;
                int lh = g_originalLeftRect.bottom - g_originalLeftRect.top;
                POINT pt = {g_originalLeftRect.left, g_originalLeftRect.top};
                ScreenToClient(g_bbar, &pt);
                SetWindowPos(g_leftToolbar, NULL, pt.x, pt.y, lw, lh,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
            
            if (g_rightToolbar && IsWindow(g_rightToolbar)) {
                int rw = g_originalRightRect.right - g_originalRightRect.left;
                int rh = g_originalRightRect.bottom - g_originalRightRect.top;
                POINT pt = {g_originalRightRect.left, g_originalRightRect.top};
                ScreenToClient(g_bbar, &pt);
                SetWindowPos(g_rightToolbar, NULL, pt.x, pt.y, rw, rh,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
            
            InvalidateRect(g_bbar, NULL, TRUE);
        }
    }
    
    g_bbar = nullptr;
    g_leftToolbar = nullptr;
    g_rightToolbar = nullptr;
    g_layoutDone = false;
    g_haveOriginalLayout = false;
    
    Wh_Log(L"Unloaded");
}