// ==WindhawkMod==
// @id              compact-rdp-connection-bar
// @name            Compact RDP connection bar
// @description     Shrinks the top connection bar in Remote Desktop Connection
// @version         1.0.1
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
#include <commctrl.h>
#include <windhawk_utils.h>

static HWND g_bbar = nullptr;
static int g_gapWidth = 100;
static HWND g_leftToolbar = nullptr;
static HWND g_rightToolbar = nullptr;
static RECT g_textRect = {0};

// Original layout for restoration
static RECT g_originalBBarRect = {0};
static RECT g_originalLeftRect = {0};
static RECT g_originalRightRect = {0};
static bool g_haveOriginalLayout = false;

// Thread-local paint flag (reviewer fix #5)
static thread_local bool t_inBarPaint = false;

// Hook function types
using DrawTextW_t = int (WINAPI*)(HDC, LPCWSTR, int, LPRECT, UINT);
using DrawTextExW_t = int (WINAPI*)(HDC, LPWSTR, int, LPRECT, UINT, LPDRAWTEXTPARAMS);
using CreateWindowExW_t = HWND (WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);

DrawTextW_t DrawTextW_Original;
DrawTextExW_t DrawTextExW_Original;
CreateWindowExW_t CreateWindowExW_Original;

// Find toolbars in BBar (reviewer fix #3 - use locals, only commit when both found)
static bool FindToolbars(HWND bbar, HWND* leftOut, HWND* rightOut) {
    HWND child = NULL;
    wchar_t cls[64];
    int index = 0;
    HWND left = nullptr, right = nullptr;
    
    while ((child = FindWindowExW(bbar, child, NULL, NULL)) != NULL) {
        GetClassNameW(child, cls, 64);
        // Reviewer fix: exact match instead of wcsstr
        if (wcscmp(cls, L"ToolbarWindow32") == 0) {
            if (index == 0) right = child;
            else if (index == 1) left = child;
            index++;
        }
    }
    
    if (left && right) {
        *leftOut = left;
        *rightOut = right;
        return true;
    }
    return false;
}

// Apply compact layout (reviewer fix #6 - DPI scaling, fix #4 - save originals)
static bool ApplyLayout(HWND hwnd) {
    HWND left = nullptr, right = nullptr;
    if (!FindToolbars(hwnd, &left, &right)) {
        return false;
    }
    
    // Commit globals only when both toolbars found (reviewer fix #3)
    g_leftToolbar = left;
    g_rightToolbar = right;
    g_bbar = hwnd;
    
    // Save original layout on first time (reviewer fix #4)
    if (!g_haveOriginalLayout) {
        GetWindowRect(hwnd, &g_originalBBarRect);
        GetWindowRect(g_leftToolbar, &g_originalLeftRect);
        GetWindowRect(g_rightToolbar, &g_originalRightRect);
        g_haveOriginalLayout = true;
    }
    
    int dpi = GetDpiForWindow(hwnd);
    int scaledGap = MulDiv(g_gapWidth, dpi, 96);
    
    RECT leftRect, rightRect, bbarRect, clientRect;
    GetWindowRect(g_leftToolbar, &leftRect);
    GetWindowRect(g_rightToolbar, &rightRect);
    GetWindowRect(hwnd, &bbarRect);
    GetClientRect(hwnd, &clientRect);  // Reviewer fix #6 - real height
    
    int leftWidth = leftRect.right - leftRect.left;
    int rightWidth = rightRect.right - rightRect.left;
    int bbarHeight = bbarRect.bottom - bbarRect.top;
    
    int compactWidth = leftWidth + scaledGap + rightWidth;
    
    // Reviewer fix: center the bar instead of keeping right edge
    int bbarWidth = bbarRect.right - bbarRect.left;
    int newLeft = bbarRect.left + (bbarWidth - compactWidth) / 2;
    
    SetWindowPos(hwnd, NULL, newLeft, bbarRect.top, compactWidth, bbarHeight,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    
    SetWindowPos(g_leftToolbar, NULL, 0, 0, leftWidth, bbarHeight,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(g_rightToolbar, NULL, leftWidth + scaledGap, 0, rightWidth, bbarHeight,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    
    // Text rect - use real client height (reviewer fix #6)
    g_textRect.left = leftWidth;
    g_textRect.right = leftWidth + scaledGap;
    g_textRect.top = 0;
    g_textRect.bottom = clientRect.bottom;
    
    Wh_Log(L"Layout applied: %dx%d, text zone: %d-%d", compactWidth, bbarHeight, g_textRect.left, g_textRect.right);
    return true;
}

// Subclass proc - 5th param is dwRefData, not subclass ID (reviewer fix)
static LRESULT CALLBACK BBar_SubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
                                          DWORD_PTR dwRefData) {
    if (msg == WM_PAINT) {
        // Apply layout if toolbars exist and we haven't yet
        if (!g_haveOriginalLayout) {
            if (!ApplyLayout(hwnd)) {
                return DefSubclassProc(hwnd, msg, wp, lp);
            }
        }
        
        // Compute text rect from current toolbar positions
        if (g_leftToolbar && g_rightToolbar && IsWindow(g_leftToolbar) && IsWindow(g_rightToolbar)) {
            RECT leftRect, rightRect;
            GetWindowRect(g_leftToolbar, &leftRect);
            GetWindowRect(g_rightToolbar, &rightRect);
            
            POINT leftPt = {leftRect.right, 0};
            POINT rightPt = {rightRect.left, 0};
            ScreenToClient(hwnd, &leftPt);
            ScreenToClient(hwnd, &rightPt);
            
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            
            g_textRect.left = leftPt.x;
            g_textRect.right = rightPt.x;
            g_textRect.top = 0;
            g_textRect.bottom = clientRect.bottom;
        }
        
        t_inBarPaint = true;
        LRESULT r = DefSubclassProc(hwnd, msg, wp, lp);
        t_inBarPaint = false;
        return r;
    }
    
    if (msg == WM_DESTROY) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, BBar_SubclassProc);
        g_bbar = nullptr;
        g_leftToolbar = nullptr;
        g_rightToolbar = nullptr;
        g_haveOriginalLayout = false;
        return DefSubclassProc(hwnd, msg, wp, lp);
    }
    
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// Reviewer fix #1: Hook CreateWindowExW instead of timer
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                  DWORD dwStyle, int x, int y, int nWidth, int nHeight,
                                  HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                         x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    if (hwnd && lpClassName) {
        wchar_t className[64] = {0};
        if (GetClassNameW(hwnd, className, 64) && wcscmp(className, L"BBarWindowClass") == 0) {
            Wh_Log(L"BBar created: %p", hwnd);
            // Subclass immediately - layout will be applied on first paint
            WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, BBar_SubclassProc, 0);
        }
    }
    
    return hwnd;
}

// Reviewer fix #1: Handle existing BBar in Wh_ModAfterInit
static void FindExistingBBar() {
    HWND desktop = GetDesktopWindow();
    HWND hwnd = NULL;
    DWORD myPid = GetCurrentProcessId();
    
    while ((hwnd = FindWindowExW(desktop, hwnd, L"BBarWindowClass", NULL)) != NULL) {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != myPid) continue;
        
        Wh_Log(L"Found existing BBar: %p", hwnd);
        WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, BBar_SubclassProc, 0);
        break;
    }
}

// Reviewer fix #5: Don't modify caller's rect, don't touch DT_CALCRECT, use thread_local
int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    if (t_inBarPaint && g_bbar && lprc && !(format & DT_CALCRECT)) {
        RECT rc = g_textRect;  // Local copy, not caller's rect
        format = (format & ~(DT_LEFT | DT_RIGHT)) | DT_CENTER;
        return DrawTextW_Original(hdc, lpchText, cchText, &rc, format);
    }
    return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
}

int WINAPI DrawTextExW_Hook(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, 
                             UINT format, LPDRAWTEXTPARAMS lpdtp) {
    if (t_inBarPaint && g_bbar && lprc && !(format & DT_CALCRECT)) {
        RECT rc = g_textRect;  // Local copy
        format = (format & ~(DT_LEFT | DT_RIGHT)) | DT_CENTER;
        return DrawTextExW_Original(hdc, lpchText, cchText, &rc, format, lpdtp);
    }
    return DrawTextExW_Original(hdc, lpchText, cchText, lprc, format, lpdtp);
}

BOOL Wh_ModInit() {
    g_gapWidth = Wh_GetIntSetting(L"gapWidth");
    if (g_gapWidth < 0) g_gapWidth = 0;
    if (g_gapWidth > 500) g_gapWidth = 500;
    
    // Reviewer fix: use WindhawkUtils::SetFunctionHook (type-safe)
    WindhawkUtils::SetFunctionHook(DrawTextW, DrawTextW_Hook, &DrawTextW_Original);
    WindhawkUtils::SetFunctionHook(DrawTextExW, DrawTextExW_Hook, &DrawTextExW_Original);
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);
    
    Wh_Log(L"Init: gap=%d", g_gapWidth);
    return TRUE;
}

void Wh_ModAfterInit() {
    FindExistingBBar();
}

// Reviewer fix #2 & #4: proper subclass removal + layout restore
void Wh_ModUninit() {
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
    g_haveOriginalLayout = false;
    Wh_Log(L"Unloaded");
}
