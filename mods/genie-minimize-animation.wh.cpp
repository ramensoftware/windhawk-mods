// ==WindhawkMod==
// @id              genie-minimize-animation
// @name            Genie Animation Mod
// @description     Definitely not inspired from MacOS.
// @version         1.0.0
// @author          lolstijl
// @github          https://github.com/lolstijl
// @include         *
// @compilerOptions -ldwmapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Genie Animation Mod
This is a mod that adds a genie like minimize animation.
Since I don't know C++ yet, this mode was written by gemini and claude.
This mod isn't perfect but windows isn't giving me a lot to work with. (windows animation api is almost 20 years old)
Keep in mind that these animation can use a lot of cpu power but from my experience this only spikes when you spam the animations.

I'll try to add settings for this mod later.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration_ms: 450
  $name: Animation duration (ms)
  $description: >-
    How long the genie animation takes, in milliseconds. Lower is snappier,
    higher is more deliberate. Typical range: 200 (snappy) to 700 (slow).
    Clamped to 50-2000.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <math.h>
#include <atomic>
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

// --- SETTINGS ---
std::atomic<int> g_durationMs{450};

void LoadSettings() {
    int ms = Wh_GetIntSetting(L"duration_ms");
    if (ms < 50) ms = 50;
    if (ms > 2000) ms = 2000;
    g_durationMs.store(ms, std::memory_order_relaxed);
}

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

    // Create the ghost without WS_VISIBLE so it isn't briefly shown as an
    // uninitialized layered window. UpdateLayeredWindow below configures it
    // before we make it visible.
    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"STATIC", NULL, WS_POPUP,
        data->targetRect.left, data->targetRect.top, data->width, data->height,
        NULL, NULL, NULL, NULL
    );

    // Two memory DCs:
    //   hOrigDC   - holds the captured window snapshot at original size.
    //   hScaledDC - holds a per-frame down-stretched copy, fed to UpdateLayeredWindow.
    // Allocating the scaled bitmap once at original size avoids the per-frame
    // CreateCompatibleBitmap from the old SetWindowPos+StretchBlt+SetLayeredWindowAttributes
    // path. Each frame just reuses a sub-region (0,0)-(newW,newH).
    HDC hScreenDC = GetDC(NULL);
    HDC hOrigDC = CreateCompatibleDC(hScreenDC);
    HDC hScaledDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hScaledBitmap = CreateCompatibleBitmap(hScreenDC, data->width, data->height);
    HBITMAP hOldOrig = (HBITMAP)SelectObject(hOrigDC, data->hBitmap);
    HBITMAP hOldScaled = (HBITMAP)SelectObject(hScaledDC, hScaledBitmap);
    // HALFTONE averages source pixels into each destination pixel instead of
    // picking one (COLORONCOLOR), so extreme downscales like 1920x1080 -> 50x30
    // come out smooth instead of jagged. HALFTONE requires SetBrushOrgEx per
    // GDI docs or the resampling brush can be misaligned.
    SetStretchBltMode(hScaledDC, HALFTONE);
    SetBrushOrgEx(hScaledDC, 0, 0, NULL);

    // Use our dynamically learned X coordinate!
    int targetDockX = data->targetDockX;
    int targetDockY = screenHeight + data->height;

    int startCenterX = data->targetRect.left + (data->width / 2);
    int startY = data->targetRect.top;

    // Snapshot the cached setting once at the start of the animation. The cache
    // is refreshed by Wh_ModSettingsChanged, so user edits take effect on the
    // next minimize without us hitting Wh_GetIntSetting from a hot loop.
    const double totalMs = (double)g_durationMs.load(std::memory_order_relaxed);

    // Time-based progress synced to the DWM compose cycle via DwmFlush.
    // Rationale: rendering at a fixed step count (e.g. 60 frames in 450ms = 133fps)
    // into a 60Hz monitor means DWM coalesces 2-3 of our frames per displayed frame,
    // and which frames get coalesced is jittery. Driving progress by real elapsed
    // time and gating each frame on the next compose cycle means every frame we
    // render is exactly one frame the user sees, perfectly aligned with vsync.
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    BOOL firstFrame = TRUE;
    for (;;) {
        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= totalMs);
        float progress = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);
        float t = data->isRising ? (1.0f - progress) : progress;

        float invT = 1.0f - t;
        float moveX = 1.0f - (invT * invT * invT * invT * invT * invT);
        float moveY = (0.70f * (t * t)) + (0.10f * t);

        float scaleX = 1.0f - (0.95f * (1.8 * t));
        if (scaleX < 0.05f) scaleX = 0.05f;

        float scaleY = 1.0f - (0.70f * (t * t));

        int newW = (int)(data->width * scaleX);
        if (newW < 2) newW = 2;
        int newH = (int)(data->height * scaleY);
        if (newH < 1) newH = 1;

        int currentCenterX = startCenterX + (int)((targetDockX - startCenterX) * moveX);
        int currentY = startY + (int)((targetDockY - startY) * moveY);
        int currentX = currentCenterX - (newW / 2);

        float alphaFloat = 1.0f;
        if (t > 0.6f) {
            alphaFloat = 1.0f - ((t - 0.6f) / 0.4f);
        }
        if (alphaFloat < 0.0f) alphaFloat = 0.0f;
        BYTE alpha = (BYTE)(255 * alphaFloat);

        // Stretch the snapshot into the top-left sub-region of the scaled bitmap.
        // Pixels outside (0,0)-(newW,newH) are stale, but UpdateLayeredWindow only
        // reads exactly that rect via the SIZE we pass.
        StretchBlt(hScaledDC, 0, 0, newW, newH,
                   hOrigDC, 0, 0, data->width, data->height, SRCCOPY);

        // Single atomic update: position + size + bitmap + per-window alpha,
        // all committed in one composition pass. Replaces the old
        // SetWindowPos + StretchBlt + SetLayeredWindowAttributes triple which
        // caused visible per-step tearing on the layered ghost.
        POINT ptDst = { currentX, currentY };
        SIZE  sz    = { newW, newH };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = alpha;
        bf.AlphaFormat = 0; // source bitmap is opaque BGR, not premultiplied BGRA
        UpdateLayeredWindow(hGhost, NULL, &ptDst, &sz, hScaledDC, &ptSrc, 0, &bf, ULW_ALPHA);

        if (firstFrame) {
            ShowWindow(hGhost, SW_SHOWNOACTIVATE);
            firstFrame = FALSE;
        }

        if (lastFrame) break;

        // Block until the next DWM compose cycle. This is the vsync sync point -
        // every iteration past this corresponds to exactly one frame the user sees.
        DwmFlush();
    }

    if (data->isRising) {
        SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
        if (!(data->originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
        }
    }

    // Re-enable DWM transitions on the real window now that our animation is done.
    // Without this, transitions stay force-disabled permanently for any window the
    // mod has ever touched - so future system minimize/maximize/show animations
    // (or another tool's transitions) would never play.
    SetDwmTransitions(data->hRealWnd, TRUE);

    SelectObject(hScaledDC, hOldScaled);
    SelectObject(hOrigDC, hOldOrig);
    DeleteObject(hScaledBitmap);
    DeleteObject(data->hBitmap);
    DeleteDC(hScaledDC);
    DeleteDC(hOrigDC);
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
    LoadSettings();
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (auto& pair : g_SnapshotCache) {
        DeleteObject(pair.second);
    }
    g_SnapshotCache.clear();
    g_IconPositions.clear();
}
