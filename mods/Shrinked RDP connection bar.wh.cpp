// ==WindhawkMod==
// @id              shrinked-rdp-connection-bar
// @name            Shrinked RDP connection bar
// @description     Compact RDP bar with native text rendering centered
// @version         2.1.0
// @author          Kimi 2.8, Cherry Assistant and Sergey B
// @include         mstsc.exe
// @compilerOptions -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- gapWidth: 100
  $name: Text area width (pixels)
  $description: Width of centered hostname area
*/
// ==/WindhawkModSettings==

#include <windows.h>

static HWND g_bbar = nullptr;
static WNDPROC g_oldProc = nullptr;
static int g_gapWidth = 100;
static HWND g_leftToolbar = nullptr;
static HWND g_rightToolbar = nullptr;
static RECT g_textRect = {0};
static bool g_inPaint = false;

// Hook DrawTextW to modify rect during paint
using DrawTextW_t = int (WINAPI*)(HDC, LPCWSTR, int, LPRECT, UINT);
DrawTextW_t DrawTextW_Original;

int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    if (g_inPaint && g_bbar && lprc) {
        // Replace rect with our centered rect
        *lprc = g_textRect;
        // Force center alignment
        format = (format & ~(DT_LEFT | DT_RIGHT)) | DT_CENTER;
    }
    return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
}

using DrawTextExW_t = int (WINAPI*)(HDC, LPWSTR, int, LPRECT, UINT, LPDRAWTEXTPARAMS);
DrawTextExW_t DrawTextExW_Original;

int WINAPI DrawTextExW_Hook(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, 
                             UINT format, LPDRAWTEXTPARAMS lpdtp) {
    if (g_inPaint && g_bbar && lprc) {
        *lprc = g_textRect;
        format = (format & ~(DT_LEFT | DT_RIGHT)) | DT_CENTER;
    }
    return DrawTextExW_Original(hdc, lpchText, cchText, lprc, format, lpdtp);
}

static LRESULT CALLBACK BBar_Proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_PAINT) {
        // Calculate our text rect
        RECT leftRect, rightRect;
        GetWindowRect(g_leftToolbar, &leftRect);
        GetWindowRect(g_rightToolbar, &rightRect);
        
        POINT leftPt = {leftRect.right, 0};
        POINT rightPt = {rightRect.left, 0};
        ScreenToClient(hwnd, &leftPt);
        ScreenToClient(hwnd, &rightPt);
        
        g_textRect.left = leftPt.x;
        g_textRect.right = rightPt.x;
        g_textRect.top = 0;
        g_textRect.bottom = 32;
        
        // Let native paint happen, but with our rect
        g_inPaint = true;
        LRESULT r = CallWindowProcW(g_oldProc, hwnd, msg, wp, lp);
        g_inPaint = false;
        return r;
    }
    
    if (msg == WM_DESTROY) {
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)g_oldProc);
        g_bbar = nullptr;
        return CallWindowProcW(g_oldProc, hwnd, msg, wp, lp);
    }
    
    return CallWindowProcW(g_oldProc, hwnd, msg, wp, lp);
}

static void CALLBACK ScanTimer(HWND, UINT, UINT_PTR, DWORD) {
    if (g_bbar && IsWindow(g_bbar)) return;

    HWND desktop = GetDesktopWindow();
    HWND hwnd = NULL;
    while ((hwnd = FindWindowExW(desktop, hwnd, L"BBarWindowClass", NULL)) != NULL) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != GetCurrentProcessId()) continue;

        g_bbar = hwnd;
        g_oldProc = (WNDPROC)GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)BBar_Proc);
        
        // Find toolbars
        HWND child = NULL;
        wchar_t cls[64];
        int index = 0;
        while ((child = FindWindowExW(hwnd, child, NULL, NULL)) != NULL) {
            GetClassNameW(child, cls, 64);
            if (wcsstr(cls, L"ToolbarWindow32")) {
                if (index == 0) g_rightToolbar = child;
                else if (index == 1) g_leftToolbar = child;
                index++;
            }
        }
        
        if (!g_leftToolbar || !g_rightToolbar) {
            Wh_Log(L"Toolbars not found yet, retry...");
            break;
        }
        
        // Get sizes
        RECT leftRect, rightRect, bbarRect;
        GetWindowRect(g_leftToolbar, &leftRect);
        GetWindowRect(g_rightToolbar, &rightRect);
        GetWindowRect(hwnd, &bbarRect);
        
        int leftWidth = leftRect.right - leftRect.left;
        int rightWidth = rightRect.right - rightRect.left;
        int bbarHeight = bbarRect.bottom - bbarRect.top;
        
        int compactWidth = leftWidth + g_gapWidth + rightWidth;
        
        Wh_Log(L"BBar %p: left=%d, gap=%d, right=%d, compact=%d",
               hwnd, leftWidth, g_gapWidth, rightWidth, compactWidth);
        
        // Resize BBar
        int newLeft = bbarRect.right - compactWidth;
        SetWindowPos(hwnd, NULL, newLeft, bbarRect.top, compactWidth, bbarHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        
        // Position toolbars
        SetWindowPos(g_leftToolbar, NULL, 0, 0, leftWidth, bbarHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        SetWindowPos(g_rightToolbar, NULL, leftWidth + g_gapWidth, 0, rightWidth, bbarHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        
        // Force repaint
        InvalidateRect(hwnd, NULL, TRUE);
        
        Wh_Log(L"Final v2 layout done");
        break;
    }
}

bool HookFunction(LPCWSTR dllName, LPCSTR funcName, void* hookFunc, void** origFunc) {
    HMODULE hMod = GetModuleHandleW(dllName);
    if (!hMod) return false;
    void* target = (void*)GetProcAddress(hMod, funcName);
    if (!target) return false;
    return Wh_SetFunctionHook(target, hookFunc, origFunc);
}

BOOL Wh_ModInit() {
    g_gapWidth = Wh_GetIntSetting(L"gapWidth");
    if (g_gapWidth < 0) g_gapWidth = 0;
    if (g_gapWidth > 500) g_gapWidth = 500;
    
    // Hook text drawing functions
    HookFunction(L"user32.dll", "DrawTextW",
                 (void*)DrawTextW_Hook, (void**)&DrawTextW_Original);
    HookFunction(L"user32.dll", "DrawTextExW",
                 (void*)DrawTextExW_Hook, (void**)&DrawTextExW_Original);
    
    Wh_Log(L"Init: gap=%d", g_gapWidth);
    SetTimer(NULL, 0, 500, ScanTimer);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_bbar && IsWindow(g_bbar) && g_oldProc) {
        SetWindowLongPtrW(g_bbar, GWLP_WNDPROC, (LONG_PTR)g_oldProc);
    }
    Wh_Log(L"Unloaded");
}