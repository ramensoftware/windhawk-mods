// ==WindhawkMod==
// @id              genie-minimize-animation
// @name            genie-minimize-animation
// @description     Definitely not inspired from MacOS.
// @version         1.0.0
// @author          lolstijl
// @github          https://github.com/lolstijl
// @include         *
// @compilerOptions -ldwmapi -luser32 -lgdi32 -lm -lwinmm
// ==/WindhawkMod==

#include <windows.h>
#include <dwmapi.h>
#include <math.h>
#include <mmsystem.h> 
#include <unordered_map>
#include <mutex>

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcW_t DefWindowProcW_Original;

typedef BOOL (WINAPI *ShowWindow_t)(HWND hWnd, int nCmdShow);
ShowWindow_t ShowWindow_Original;

struct GhostAnimData {
    HWND hRealWnd;
    HBITMAP hBitmap;
    RECT targetRect;
    int width;
    int height;
    int targetDockX; // The dynamically learned icon position
    BOOL isRising;
    LONG_PTR originalExStyle;
};

// --- THE VAULTS ---
std::unordered_map<HWND, HBITMAP> g_SnapshotCache;
std::unordered_map<HWND, int> g_IconPositions; // Remembers where icons live
std::mutex g_CacheMutex;

void SetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

// -------------------------------------------------------------------------
// Pseudo-Genie Animation Thread
// -------------------------------------------------------------------------
DWORD WINAPI GhostAnimationThread(LPVOID lpParam) {
    GhostAnimData* data = (GhostAnimData*)lpParam;
    
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"STATIC", NULL, WS_POPUP | WS_VISIBLE,
        data->targetRect.left, data->targetRect.top, data->width, data->height,
        NULL, NULL, NULL, NULL
    );

    HDC hScreenDC = GetDC(NULL);
    HDC hGhostDC = GetDC(hGhost);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, data->hBitmap);
    SetStretchBltMode(hGhostDC, COLORONCOLOR);

    // Use our dynamically learned X coordinate!
    int targetDockX = data->targetDockX;
    int targetDockY = screenHeight + data->height; 

    int startCenterX = data->targetRect.left + (data->width / 2);
    int startY = data->targetRect.top;

    int steps = 70; 
    
    timeBeginPeriod(1);

    for (int i = 0; i <= steps; i++) {
        float progress = (float)i / steps;
        float t = data->isRising ? (1.0f - progress) : progress;
        
        float invT = 1.0f - t;
        float moveX = 1.0f - (invT * invT * invT * invT * invT * invT);
        float moveY = (0.60f * (t * t)) + (0.20f * t); 
        
        float scaleX = 1.0f - (0.95f * (1.8 * t)); 
        if (scaleX < 0.05f) scaleX = 0.05f; 
        
        float scaleY = 1.0f - (0.70f * (t * t)); 
        
        int newW = (int)(data->width * scaleX);
        if (newW < 2) newW = 2; 
        int newH = (int)(data->height * scaleY);

        int currentCenterX = startCenterX + (int)((targetDockX - startCenterX) * moveX);
        int currentY = startY + (int)((targetDockY - startY) * moveY);
        int currentX = currentCenterX - (newW / 2);

        float alphaFloat = 1.0f;
        if (t > 0.6f) {
            alphaFloat = 1.0f - ((t - 0.6f) / 0.4f); 
        }
        if (alphaFloat < 0.0f) alphaFloat = 0.0f;
        BYTE alpha = (BYTE)(255 * alphaFloat);

        SetWindowPos(hGhost, NULL, currentX, currentY, newW, newH, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);
        StretchBlt(hGhostDC, 0, 0, newW, newH, hMemDC, 0, 0, data->width, data->height, SRCCOPY);
        SetLayeredWindowAttributes(hGhost, 0, alpha, LWA_ALPHA);
        
        Sleep(6); 
    }

    timeEndPeriod(1);

    if (data->isRising) {
        SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
        if (!(data->originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
        }
    }

    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(data->hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(hGhost, hGhostDC);
    ReleaseDC(NULL, hScreenDC);
    DestroyWindow(hGhost);
    delete data;
    return 0;
}

// -------------------------------------------------------------------------
// Core Setup Engine & Smart Tracking Logic
// -------------------------------------------------------------------------
void StartGenieAnim(HWND hWnd, BOOL rising) {
    RECT rect;
    GetWindowRect(hWnd, &rect);
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    if (w <= 0 || h <= 0) return; 

    // --- SMART ICON TRACKING ---
    POINT pt;
    GetCursorPos(&pt);
    RECT workArea;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int learnedTargetX = screenWidth / 2; // Default to center

    // If the mouse is outside the main desktop area (e.g. hovering over the taskbar)
    if (!PtInRect(&workArea, pt)) {
        learnedTargetX = pt.x; // Steal the mouse X coordinate!
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_IconPositions[hWnd] = learnedTargetX; // Save it to the vault
    } else {
        // Mouse is on the desktop (clicking the [-] title bar button)
        // Check the vault to see if we remember where this app's icon is!
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_IconPositions.count(hWnd)) {
            learnedTargetX = g_IconPositions[hWnd];
        }
    }

    GhostAnimData* data = new GhostAnimData();
    data->hRealWnd = hWnd;
    data->targetRect = rect;
    data->width = w;
    data->height = h;
    data->isRising = rising;
    data->targetDockX = learnedTargetX; // Assign the learned coordinate
    data->originalExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    data->hBitmap = CreateCompatibleBitmap(hScreenDC, w, h);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, data->hBitmap);

    if (rising) {
        BOOL fromCache = FALSE;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            if (g_SnapshotCache.count(hWnd)) {
                HDC hCacheDC = CreateCompatibleDC(hScreenDC);
                HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, g_SnapshotCache[hWnd]);
                BitBlt(hMemDC, 0, 0, w, h, hCacheDC, 0, 0, SRCCOPY);
                SelectObject(hCacheDC, hOldCacheBmp);
                DeleteDC(hCacheDC);
                
                DeleteObject(g_SnapshotCache[hWnd]);
                g_SnapshotCache.erase(hWnd);
                fromCache = TRUE;
            }
        }
        if (!fromCache) {
            PrintWindow(hWnd, hMemDC, PW_CLIENTONLY | 0x00000002);
        }
    } else {
        BitBlt(hMemDC, 0, 0, w, h, hScreenDC, rect.left, rect.top, SRCCOPY);

        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_SnapshotCache.count(hWnd)) {
            DeleteObject(g_SnapshotCache[hWnd]); 
        }
        g_SnapshotCache[hWnd] = CreateCompatibleBitmap(hScreenDC, w, h);
        HDC hCacheDC = CreateCompatibleDC(hScreenDC);
        HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, g_SnapshotCache[hWnd]);
        BitBlt(hCacheDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
        SelectObject(hCacheDC, hOldCacheBmp);
        DeleteDC(hCacheDC);
    }

    SelectObject(hMemDC, hOldBmp);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);
    CreateThread(NULL, 0, GhostAnimationThread, data, 0, NULL);
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_SHOWMINNOACTIVE) {
        SetDwmTransitions(hWnd, FALSE);
        StartGenieAnim(hWnd, FALSE);
        return ShowWindow_Original(hWnd, nCmdShow);
    }
    else if (nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL) {
        if (IsIconic(hWnd)) {
            SetDwmTransitions(hWnd, FALSE);
            LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            BOOL res = ShowWindow_Original(hWnd, nCmdShow);
            StartGenieAnim(hWnd, TRUE);
            return res;
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_DESTROY) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_SnapshotCache.count(hWnd)) {
            DeleteObject(g_SnapshotCache[hWnd]);
            g_SnapshotCache.erase(hWnd);
        }
        // Forget the icon position when the app closes
        if (g_IconPositions.count(hWnd)) {
            g_IconPositions.erase(hWnd);
        }
    }

    if (Msg == WM_SYSCOMMAND) {
        UINT cmd = wParam & 0xFFF0;
        if (cmd == SC_MINIMIZE) {
            SetDwmTransitions(hWnd, FALSE);
            StartGenieAnim(hWnd, FALSE);
            return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
        }
        else if (cmd == SC_RESTORE && IsIconic(hWnd)) {
            SetDwmTransitions(hWnd, FALSE);
            LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            LRESULT res = DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            StartGenieAnim(hWnd, TRUE);
            return res;
        }
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    return TRUE;
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (auto& pair : g_SnapshotCache) {
        DeleteObject(pair.second);
    }
    g_SnapshotCache.clear();
    g_IconPositions.clear();
}
