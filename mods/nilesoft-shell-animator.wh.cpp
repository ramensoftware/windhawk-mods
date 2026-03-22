// ==WindhawkMod==
// @id              nilesoft-shell-animator
// @name            Nilesoft Shell Animator
// @description     Adds customizable animations to Nilesoft Shell.
// @version         0.2.6
// @author          Lockframe
// @github          https://www.github.com/Lockframe
// @include         explorer.exe
// @compilerOptions -lgdi32 -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Nilesoft Shell Animator

Adds customizable and position-aware animations to Nilesoft Shell's context menu without touching Windows' native menus.

![](https://i.imgur.com/rEZsF9Y.gif)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- AnimationMode: "slide"
  $name: Animation Mode
  $options:
  - "slide": Slide
  - "fade": Fade
  - "native": Native (WinUI Style Clip + Fade Shadow)
  - "fade_slide": Fade + Slide
- AnimationOffset: 20
  $name: Slide Distance (px)
  $description: Standard slide distance.
- NativeSlidePercentage: 0
  $name: Native Slide Percentage (%)
  $description: (Native Mode Only) Overrides slide distance. % of menu height to hide. Set to ~70-100 for WinUI style. Set to 0 to disable.
- CubicBezier: "0.1, 0.9, 0.2, 1.0"
  $name: Custom Cubic Bezier (x1, y1, x2, y2)
  $description: Define the easing curve (CSS format). Default is a snappy ease-out. Try "0.25, 0.1, 0.25, 1.0" for standard ease.
- AnimationDuration: 150
  $name: Animation Duration (ms)
  $description: How long the slide animation takes.
- CornerRadius: 8
  $name: Corner Radius (px)
  $description: Radius of the menu corners for Native mode clipping.
- ContextMenuDelay: 0
  $name: Context Menu Delay (ms)
  $description: Delay before the Main Context Menu appears.
- ForceDirection: "auto"
  $name: Animation Direction
  $description: (Slide mode only)
  $options:
  - "auto": Auto-detect
  - "down": Always Slide Down
  - "up": Always Slide Up
- MainMenuOffsetX: -1
  $name: Main Menu Offset X
  $description: Pixel shift for Main Menu text (Horizontal).
- MainMenuOffsetY: -1
  $name: Main Menu Offset Y
  $description: Pixel shift for Main Menu text (Vertical).
- SubMenuRightOffsetX: 1
  $name: Right Submenu Offset X
  $description: Pixel shift for Submenus opening to the Right.
- SubMenuRightOffsetY: 0
  $name: Right Submenu Offset Y
  $description: Pixel shift for Submenus opening to the Right (Vertical).
- SubMenuLeftOffsetX: -4
  $name: Left Submenu Offset X
  $description: Pixel shift for Submenus opening to the Left.
- SubMenuLeftOffsetY: 0
  $name: Left Submenu Offset Y
  $description: Pixel shift for Submenus opening to the Left (Vertical).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <timeapi.h>

// -------------------------------------------------------------------------
// Constants
// -------------------------------------------------------------------------
constexpr int FRAME_DELAY_MS = 10;
constexpr int DIRECTION_THRESHOLD = 100;
constexpr int MAX_ENUM_WINDOWS = 128;
constexpr int MAX_ANIM_WINDOWS = 16;
constexpr int MAX_QUEUE_DEPTH = 8;
constexpr int MIN_WINDOW_SIZE = 10;
constexpr int MIN_PARENT_WIDTH = 50;
constexpr int SAFE_ZONE_MARGIN = 50;
constexpr int THREAD_SHUTDOWN_TIMEOUT_MS = 2000;
constexpr int NATIVE_FADE_DURATION_MS = 200; 
constexpr SIZE_T ANIMATION_THREAD_STACK_SIZE = 64 * 1024;
constexpr UINT SWP_ANIMATE_FLAGS = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER;

static const wchar_t* HANDLED_PROP = L"Wh_NilesoftHandled";
static const wchar_t* SHADOW_HIDDEN_PROP = L"Wh_ShadowHidden";

// -------------------------------------------------------------------------
// Enums
// -------------------------------------------------------------------------
enum WindowType : unsigned char { 
    WINDOW_NONE = 0, 
    WINDOW_NILESOFT = 1, 
    WINDOW_MENU = 2
};

enum AnimMode : int {
    ANIM_SLIDE = 0,
    ANIM_FADE = 1,
    ANIM_NATIVE = 2,
    ANIM_FADE_SLIDE = 3
};

// -------------------------------------------------------------------------
// Structures
// -------------------------------------------------------------------------
struct WindowClassInfo {
    WindowType type;
    bool isBlacklisted;
};

struct Settings {
    int animMode;
    int animOffset;
    int animDuration;
    int cornerRadius;
    int nativeSlidePercentage;
    float bX1, bY1, bX2, bY2;
    int contextMenuDelay;
    int forceDirection;
    int fixMainX;
    int fixMainY;
    int fixSubRightX;
    int fixSubRightY;
    int fixSubLeftX;
    int fixSubLeftY;
};

// -------------------------------------------------------------------------
// Globals
// -------------------------------------------------------------------------
static Settings g_settings = {
    .animMode = ANIM_SLIDE,
    .animOffset = 20,
    .animDuration = 150,
    .cornerRadius = 8,
    .nativeSlidePercentage = 0,
    .bX1 = 0.1f, .bY1 = 0.9f, .bX2 = 0.2f, .bY2 = 1.0f,
    .contextMenuDelay = 0,
    .forceDirection = 0,
    .fixMainX = -1, .fixMainY = -1,
    .fixSubRightX = 1, .fixSubRightY = 0,
    .fixSubLeftX = -4, .fixSubLeftY = 0
};

static std::mutex g_settingsMutex;
static double g_perfFreqMsRecip; 
static std::atomic<bool> g_modRunning{true};
static HANDLE g_animationThread = NULL;
static std::atomic<bool> g_threadInitialized{false};
static std::mutex g_queueMutex;
static std::condition_variable g_queueCV;

// -------------------------------------------------------------------------
// Typedefs
// -------------------------------------------------------------------------
typedef BOOL (WINAPI *UpdateLayeredWindow_t)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);
typedef BOOL (WINAPI *SetWindowPos_t)(HWND, HWND, int, int, int, int, UINT);
typedef BOOL (WINAPI *ShowWindow_t)(HWND, int);
typedef BOOL (WINAPI *SetLayeredWindowAttributes_t)(HWND, COLORREF, BYTE, DWORD);

static UpdateLayeredWindow_t UpdateLayeredWindow_Original;
static SetWindowPos_t SetWindowPos_Original;
static ShowWindow_t ShowWindow_Original;
static SetLayeredWindowAttributes_t pSetLayeredWindowAttributes = NULL;

// -------------------------------------------------------------------------
// Visual Capture Structure
// -------------------------------------------------------------------------
struct CachedVisuals {
    HDC hdcMemory;
    HBITMAP hBitmap;
    HGDIOBJ hOldBitmap;
    BLENDFUNCTION originalBlend;
    SIZE size;
    POINT srcPoint;
    bool isValid;
};

// -------------------------------------------------------------------------
// Animation Structures
// -------------------------------------------------------------------------
struct TargetWindow {
    HWND hWnd;
    DWORD threadId;
    WindowType type;
    int targetX;
    int targetY;
    int startY;
    int width;
    int height;
    float dpiScale;
    bool isShadow;
    bool cloaked;
    CachedVisuals visuals; 
};

struct AnimationGroup {
    TargetWindow windows[MAX_ANIM_WINDOWS];
    int windowCount;
    int animDuration;
    int animMode;
    int cornerRadius;
    float bX1, bY1, bX2, bY2; 
    int startupDelay;
};

static std::queue<AnimationGroup*> g_animationQueue;
static AnimationGroup g_groupPool[MAX_QUEUE_DEPTH];
static bool g_groupPoolUsed[MAX_QUEUE_DEPTH] = {false};
static std::mutex g_poolMutex;

// -------------------------------------------------------------------------
// Helpers
// -------------------------------------------------------------------------
static AnimationGroup* AllocateGroup() {
    std::lock_guard lock(g_poolMutex);
    for (int i = 0; i < MAX_QUEUE_DEPTH; i++) {
        if (!g_groupPoolUsed[i]) {
            g_groupPoolUsed[i] = true;
            g_groupPool[i].windowCount = 0;
            g_groupPool[i].startupDelay = 0;
            for(int k=0; k<MAX_ANIM_WINDOWS; k++) {
                g_groupPool[i].windows[k].visuals.isValid = false;
                g_groupPool[i].windows[k].isShadow = false;
                g_groupPool[i].windows[k].cloaked = false;
            }
            return &g_groupPool[i];
        }
    }
    return nullptr;
}

static void CleanupVisuals(CachedVisuals& vis) {
    if (vis.isValid) {
        if (vis.hdcMemory) {
            if (vis.hOldBitmap) SelectObject(vis.hdcMemory, vis.hOldBitmap);
            DeleteDC(vis.hdcMemory);
        }
        if (vis.hBitmap) DeleteObject(vis.hBitmap);
        vis.isValid = false;
        vis.hdcMemory = NULL;
        vis.hBitmap = NULL;
    }
}

static void FreeGroup(AnimationGroup* group) {
    if (!group) return;
    for (int i = 0; i < group->windowCount; i++) {
        CleanupVisuals(group->windows[i].visuals);
        if (IsWindow(group->windows[i].hWnd)) {
            RemovePropW(group->windows[i].hWnd, SHADOW_HIDDEN_PROP);
        }
    }
    std::lock_guard lock(g_poolMutex);
    for (int i = 0; i < MAX_QUEUE_DEPTH; i++) {
        if (&g_groupPool[i] == group) {
            g_groupPoolUsed[i] = false;
            return;
        }
    }
}

static Settings GetSettingsSnapshot() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

static void LoadSettings() {
    std::lock_guard lock(g_settingsMutex);
    PCWSTR modeSetting = Wh_GetStringSetting(L"AnimationMode");
    if (wcscmp(modeSetting, L"fade") == 0) g_settings.animMode = ANIM_FADE;
    else if (wcscmp(modeSetting, L"native") == 0) g_settings.animMode = ANIM_NATIVE;
    else if (wcscmp(modeSetting, L"fade_slide") == 0) g_settings.animMode = ANIM_FADE_SLIDE;
    else g_settings.animMode = ANIM_SLIDE;
    Wh_FreeStringSetting(modeSetting);

    g_settings.animOffset = Wh_GetIntSetting(L"AnimationOffset");
    g_settings.animDuration = Wh_GetIntSetting(L"AnimationDuration");
    g_settings.cornerRadius = Wh_GetIntSetting(L"CornerRadius");
    g_settings.nativeSlidePercentage = Wh_GetIntSetting(L"NativeSlidePercentage");
    if (g_settings.nativeSlidePercentage < 0) g_settings.nativeSlidePercentage = 0;
    if (g_settings.nativeSlidePercentage > 100) g_settings.nativeSlidePercentage = 100;

    PCWSTR bezierStr = Wh_GetStringSetting(L"CubicBezier");
    float x1 = 0.25f, y1 = 0.1f, x2 = 0.25f, y2 = 1.0f; 
    if (bezierStr) {
        if (swscanf_s(bezierStr, L"%f, %f, %f, %f", &x1, &y1, &x2, &y2) == 4) { }
        Wh_FreeStringSetting(bezierStr);
    }
    g_settings.bX1 = x1; g_settings.bY1 = y1;
    g_settings.bX2 = x2; g_settings.bY2 = y2;
    
    g_settings.contextMenuDelay = Wh_GetIntSetting(L"ContextMenuDelay");
    
    PCWSTR dirSetting = Wh_GetStringSetting(L"ForceDirection");
    if (wcscmp(dirSetting, L"down") == 0) g_settings.forceDirection = 1;
    else if (wcscmp(dirSetting, L"up") == 0) g_settings.forceDirection = 2;
    else g_settings.forceDirection = 0;
    Wh_FreeStringSetting(dirSetting);

    g_settings.fixMainX = Wh_GetIntSetting(L"MainMenuOffsetX");
    g_settings.fixMainY = Wh_GetIntSetting(L"MainMenuOffsetY");
    g_settings.fixSubRightX = Wh_GetIntSetting(L"SubMenuRightOffsetX");
    g_settings.fixSubRightY = Wh_GetIntSetting(L"SubMenuRightOffsetY");
    g_settings.fixSubLeftX = Wh_GetIntSetting(L"SubMenuLeftOffsetX");
    g_settings.fixSubLeftY = Wh_GetIntSetting(L"SubMenuLeftOffsetY");
}

static float GetDpiScale(HWND hWnd) {
    int dpi = GetDpiForWindow(hWnd);
    return (dpi > 0) ? ((float)dpi / 96.0f) : 1.0f;
}

static void SetWindowAlphaSimple(HWND hWnd, BYTE alpha) {
    if (pSetLayeredWindowAttributes) {
        LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        if (!(style & WS_EX_LAYERED)) SetWindowLongPtrW(hWnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
        pSetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
    }
}

static WindowClassInfo GetWindowClassInfo(HWND hWnd) {
    WindowClassInfo info = { WINDOW_NONE, false };
    wchar_t className[64];
    int len = GetClassNameW(hWnd, className, _countof(className));
    if (len == 0) return info;

    // Fast-path check using the first character to bypass heavy string matching on Explorer's hot path
    wchar_t firstChar = className[0];

    if (firstChar == L'#' && len == 6 && wcscmp(className, L"#32768") == 0) {
        info.type = WINDOW_MENU;
        return info;
    }

    if (firstChar == L'N' || firstChar == L'n') {
        if (wcsstr(className, L"Nilesoft")) {
            info.type = WINDOW_NILESOFT;
            return info;
        }
    }

    // Blacklist fast-paths
    if (firstChar == L't' && wcsstr(className, L"tooltips")) info.isBlacklisted = true;
    else if (firstChar == L'W' && wcsstr(className, L"Xaml")) info.isBlacklisted = true;
    else if (firstChar == L'I' && wcsstr(className, L"IME")) info.isBlacklisted = true;
    else if (firstChar == L'M' && wcsstr(className, L"MSCTFIME")) info.isBlacklisted = true;

    return info;
}

static bool CheckIsShadow(HWND hWnd) {
    wchar_t className[64];
    if (GetClassNameW(hWnd, className, 64)) {
        if (wcsstr(className, L"Shadow")) return true;
    }
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_LAYERED) && (exStyle & WS_EX_TRANSPARENT)) {
        return true;
    }
    HWND owner = GetWindow(hWnd, GW_OWNER);
    if (owner && IsWindowVisible(owner)) {
        wchar_t ownerClass[64];
        if (GetClassNameW(owner, ownerClass, 64)) {
             if (wcsstr(ownerClass, L"Nilesoft") || wcscmp(ownerClass, L"#32768") == 0) {
                 return true;
             }
        }
    }
    return false; 
}

static bool ValidateWindow(const TargetWindow& win) {
    if (!IsWindow(win.hWnd)) return false;
    DWORD currentThreadId = GetWindowThreadProcessId(win.hWnd, NULL);
    if (currentThreadId != win.threadId) return false;
    WindowClassInfo info = GetWindowClassInfo(win.hWnd);
    if (info.type != win.type) return false;
    return true;
}

static bool CaptureVisuals(TargetWindow& win, HDC hdcSrc, SIZE* size, BLENDFUNCTION* blend) {
    if (!hdcSrc || !size || !blend) return false;
    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen) return false;

    win.visuals.hdcMemory = CreateCompatibleDC(hdcScreen);
    win.visuals.hBitmap = CreateCompatibleBitmap(hdcScreen, size->cx, size->cy);
    ReleaseDC(NULL, hdcScreen);
    if (!win.visuals.hdcMemory || !win.visuals.hBitmap) {
        CleanupVisuals(win.visuals);
        return false;
    }

    win.visuals.hOldBitmap = SelectObject(win.visuals.hdcMemory, win.visuals.hBitmap);
    BitBlt(win.visuals.hdcMemory, 0, 0, size->cx, size->cy, hdcSrc, 0, 0, SRCCOPY);
    win.visuals.originalBlend = *blend;
    win.visuals.size = *size;
    win.visuals.srcPoint = {0,0};
    win.visuals.isValid = true;
    return true;
}

static float SolveBezier(float x1, float y1, float x2, float y2, float x) {
    if (x <= 0.0f) return 0.0f;
    if (x >= 1.0f) return 1.0f;
    if (x1 == y1 && x2 == y2) return x; // Linear optimization

    // Helper functions
    auto sampleCurveX = [&](float t) {
        float cx = 3.0f * x1;
        float bx = 3.0f * (x2 - x1) - cx;
        float ax = 1.0f - cx - bx;
        return ((ax * t + bx) * t + cx) * t;
    };
    auto sampleCurveY = [&](float t) {
        float cy = 3.0f * y1;
        float by = 3.0f * (y2 - y1) - cy;
        float ay = 1.0f - cy - by;
        return ((ay * t + by) * t + cy) * t;
    };
    auto sampleCurveDerivativeX = [&](float t) {
        float cx = 3.0f * x1;
        float bx = 3.0f * (x2 - x1) - cx;
        float ax = 1.0f - cx - bx;
        return (3.0f * ax * t + 2.0f * bx) * t + cx;
    };

    // 1. Try Newton's Method (Fast)
    float t = x; 
    bool newtonConverged = false;
    for (int i = 0; i < 8; i++) {
        float x2_est = sampleCurveX(t) - x;
        if (fabs(x2_est) < 1e-5) {
            newtonConverged = true;
            break; 
        }
        float d2 = sampleCurveDerivativeX(t);
        if (fabs(d2) < 1e-5) break; // Slope too small, possibly unstable
        t = t - x2_est / d2;
    }

    // 2. Fallback to Bisection (Stable) if Newton failed or diverged
    if (!newtonConverged || t < 0.0f || t > 1.0f) {
        float t0 = 0.0f;
        float t1 = 1.0f;
        t = x;
        while (t0 < t1) {
            float x2_est = sampleCurveX(t);
            if (fabs(x2_est - x) < 1e-5) break;
            if (x > x2_est) t0 = t;
            else t1 = t;
            t = (t1 - t0) * 0.5f + t0;
            if ((t1 - t0) < 1e-5) break; // Precision reached
        }
    }

    return sampleCurveY(t);
}

// -------------------------------------------------------------------------
// Logic Helpers
// -------------------------------------------------------------------------
struct AnimationParams {
    int animMode;
    int direction;
    int scaledOffset;
    float dpiScale;
    int cornerRadius;
    int nativeSlidePercentage;
    float bX1, bY1, bX2, bY2;
    int forceDirection;
    int animDuration;
    int contextMenuDelay;
    int fixMainX, fixMainY;
    int fixSubRightX, fixSubRightY;
    int fixSubLeftX, fixSubLeftY;
};

static AnimationParams CalculateAnimationParams(HWND hWnd, int targetY) {
    AnimationParams params;
    Settings settings = GetSettingsSnapshot();
    
    params.animMode = settings.animMode;
    params.dpiScale = GetDpiScale(hWnd);
    params.scaledOffset = (int)((float)settings.animOffset * params.dpiScale);
    params.cornerRadius = settings.cornerRadius;
    params.nativeSlidePercentage = settings.nativeSlidePercentage;
    params.bX1 = settings.bX1; params.bY1 = settings.bY1;
    params.bX2 = settings.bX2; params.bY2 = settings.bY2;
    params.forceDirection = settings.forceDirection;
    params.animDuration = settings.animDuration;
    params.contextMenuDelay = settings.contextMenuDelay;
    
    params.fixMainX = (int)((float)settings.fixMainX * params.dpiScale);
    params.fixMainY = (int)((float)settings.fixMainY * params.dpiScale);
    params.fixSubRightX = (int)((float)settings.fixSubRightX * params.dpiScale);
    params.fixSubRightY = (int)((float)settings.fixSubRightY * params.dpiScale);
    params.fixSubLeftX = (int)((float)settings.fixSubLeftX * params.dpiScale);
    params.fixSubLeftY = (int)((float)settings.fixSubLeftY * params.dpiScale);
    
    params.direction = 1;
    if (settings.forceDirection == 2) params.direction = -1;
    else if (settings.forceDirection == 0) {
        POINT cursor;
        GetCursorPos(&cursor);
        if ((cursor.y - targetY) > DIRECTION_THRESHOLD) params.direction = -1;
    }
    return params;
}

static int CalculateStartY(const AnimationParams& params, int targetY, int height) {
    int offset = params.scaledOffset;
    if (params.animMode == ANIM_NATIVE && params.nativeSlidePercentage > 0) {
        offset = (int)((float)height * (float)params.nativeSlidePercentage / 100.0f);
    }
    return targetY - (offset * params.direction);
}

// -------------------------------------------------------------------------
// Animation Logic
// -------------------------------------------------------------------------
static void ApplyNativeClipping(const TargetWindow& win, int currentY, int baseRadius) {
    int clipTop = 0;
    int clipBottom = 0;

    if (win.startY < win.targetY) { // Sliding Down
        if (currentY < win.targetY) {
            clipTop = win.targetY - currentY;
        }
    } else { // Sliding Up
        int currentBottom = currentY + win.height;
        int floorY = win.targetY + win.height;
        if (currentBottom > floorY) {
            clipBottom = currentBottom - floorY;
        }
    }

    if (clipTop < 0) clipTop = 0;
    if (clipBottom < 0) clipBottom = 0;

    if (clipTop + clipBottom >= win.height) {
        HRGN empty = CreateRectRgn(0, 0, 0, 0);
        // Prevent leak if SetWindowRgn fails to take ownership
        if (SetWindowRgn(win.hWnd, empty, TRUE) == 0) {
            DeleteObject(empty);
        }
        return;
    }

    // Fast-path: Skip the heavy CombineRgn math if the menu is fully unclipped
    if (clipTop == 0 && clipBottom == 0) {
        int r = (int)((float)baseRadius * win.dpiScale);
        HRGN rgnBase = CreateRoundRectRgn(0, 0, win.width + 1, win.height + 1, r, r);
        if (SetWindowRgn(win.hWnd, rgnBase, TRUE) == 0) {
            DeleteObject(rgnBase);
        }
        return;
    }

    int r = (int)((float)baseRadius * win.dpiScale);
    
    HRGN rgnBase = CreateRoundRectRgn(0, 0, win.width + 1, win.height + 1, r, r);
    HRGN rgnClip = CreateRectRgn(0, clipTop, win.width + 1, win.height - clipBottom + 1);

    HRGN rgnFinal = CreateRectRgn(0, 0, 0, 0); 
    int result = CombineRgn(rgnFinal, rgnBase, rgnClip, RGN_AND);

    DeleteObject(rgnBase);
    DeleteObject(rgnClip);

    if (result == ERROR) {
        DeleteObject(rgnFinal);
        return; 
    }

    // Prevent leak if SetWindowRgn fails to take ownership
    if (SetWindowRgn(win.hWnd, rgnFinal, TRUE) == 0) {
        DeleteObject(rgnFinal);
    }
}

static void RunAnimation(AnimationGroup* group) {
    if (group->windowCount == 0) return;
    if (group->startupDelay > 0) {
        Sleep(group->startupDelay);
        if (!g_modRunning.load(std::memory_order_acquire)) return;
        // Restore Visibility
        for (int i = 0; i < group->windowCount; i++) {
            TargetWindow& win = group->windows[i];
            if (!ValidateWindow(win)) continue;
            
            if (group->animMode == ANIM_SLIDE || group->animMode == ANIM_NATIVE) {
                 if (group->animMode == ANIM_NATIVE && win.isShadow) {
                     // Keep Shadow hidden
                 } else {
                     SetWindowAlphaSimple(win.hWnd, 255);
                 }
            }
        }
    }
    
    LARGE_INTEGER start, now;
    QueryPerformanceCounter(&start);
    
    float slideDuration = (float)group->animDuration;
    float totalDuration = slideDuration;
    
    if (group->animMode == ANIM_NATIVE) {
        totalDuration += (float)NATIVE_FADE_DURATION_MS;
    }

    bool interrupted = false;
    float bX1 = group->bX1; float bY1 = group->bY1;
    float bX2 = group->bX2; float bY2 = group->bY2;

    while (g_modRunning.load(std::memory_order_acquire)) {
        QueryPerformanceCounter(&now);
        float elapsed = (float)(now.QuadPart - start.QuadPart) * (float)g_perfFreqMsRecip;
        if (elapsed >= totalDuration) break;

        bool anyValid = false;
        // Hoist Bezier calculation out of the window loop to save CPU cycles per frame
        float t = elapsed / slideDuration;
        if (t > 1.0f) t = 1.0f;
        float ease = SolveBezier(bX1, bY1, bX2, bY2, t);
        if (group->animMode == ANIM_NATIVE) {
            if (ease > 1.0f) ease = 1.0f;
            if (ease < 0.0f) ease = 0.0f;
        }

        for (int i = 0; i < group->windowCount; i++) {
            TargetWindow& win = group->windows[i];
            if (!ValidateWindow(win)) continue;
            anyValid = true;

            // Dynamically update size to fix clipping issues if Nilesoft resizes the menu late (heavy files)
            RECT currentR;
            if (GetWindowRect(win.hWnd, &currentR)) {
                win.width = currentR.right - currentR.left;
                win.height = currentR.bottom - currentR.top;
            }

            if (group->animMode == ANIM_FADE) {
                if (win.visuals.isValid) {
                    BLENDFUNCTION curBlend = win.visuals.originalBlend;
                    curBlend.SourceConstantAlpha = (BYTE)((float)curBlend.SourceConstantAlpha * ease);
                    POINT pt = { win.targetX, win.targetY };
                    SIZE sz = win.visuals.size;
                    POINT ptSrc = win.visuals.srcPoint;
                    UpdateLayeredWindow_Original(win.hWnd, NULL, &pt, &sz, 
                                               win.visuals.hdcMemory, &ptSrc, 
                                               0, &curBlend, ULW_ALPHA);
                } else {
                    BYTE currentAlpha = (BYTE)(255.0f * ease);
                    SetWindowAlphaSimple(win.hWnd, currentAlpha);
                }
            }
            else if (group->animMode == ANIM_FADE_SLIDE) {
                int totalDistance = win.targetY - win.startY;
                int currentY = win.startY + (int)((float)totalDistance * ease);

                if (win.visuals.isValid) {
                    BLENDFUNCTION curBlend = win.visuals.originalBlend;
                    curBlend.SourceConstantAlpha = (BYTE)((float)curBlend.SourceConstantAlpha * ease);
                    POINT pt = { win.targetX, currentY };
                    SIZE sz = win.visuals.size;
                    POINT ptSrc = win.visuals.srcPoint;
                    UpdateLayeredWindow_Original(win.hWnd, NULL, &pt, &sz, 
                                               win.visuals.hdcMemory, &ptSrc, 
                                               0, &curBlend, ULW_ALPHA);
                } else {
                    BYTE currentAlpha = (BYTE)(255.0f * ease);
                    SetWindowAlphaSimple(win.hWnd, currentAlpha);
                    SetWindowPos_Original(win.hWnd, NULL, win.targetX, currentY, 0, 0, SWP_ANIMATE_FLAGS);
                }
            }
            else if (group->animMode == ANIM_NATIVE) {
                if (!win.isShadow) {
                    // Content Animation (Text/Icons)
                    int totalDistance = win.targetY - win.startY;
                    int currentY = win.startY + (int)((float)totalDistance * ease);
                    
                    ApplyNativeClipping(win, currentY, group->cornerRadius);
                    SetWindowPos_Original(win.hWnd, NULL, win.targetX, currentY, 0, 0, SWP_ANIMATE_FLAGS);
                    
                    // Drop cloak
                    if (win.cloaked) {
                        SetWindowAlphaSimple(win.hWnd, 255);
                        win.cloaked = false;
                    }
                } else {
                    // Shadow Animation
                    if (win.visuals.isValid) {
                        float alphaScale = 0.0f;
                        if (elapsed >= slideDuration) {
                            float tFade = (elapsed - slideDuration) / (float)NATIVE_FADE_DURATION_MS;
                            if (tFade > 1.0f) tFade = 1.0f;
                            alphaScale = 1.0f - (1.0f - tFade) * (1.0f - tFade);
                        }

                        BLENDFUNCTION curBlend = win.visuals.originalBlend;
                        curBlend.SourceConstantAlpha = (BYTE)((float)curBlend.SourceConstantAlpha * alphaScale);
                        POINT pt = { win.targetX, win.targetY };
                        SIZE sz = win.visuals.size;
                        POINT ptSrc = win.visuals.srcPoint;
                        UpdateLayeredWindow_Original(win.hWnd, NULL, &pt, &sz, win.visuals.hdcMemory, &ptSrc, 0, &curBlend, ULW_ALPHA);
                    }
                }
            }
            else { // ANIM_SLIDE
                int totalDistance = win.targetY - win.startY;
                int currentY = win.startY + (int)((float)totalDistance * ease);
                
                // 1. Move the window to the correct coordinate FIRST
                SetWindowPos_Original(win.hWnd, NULL, win.targetX, currentY, 0, 0, SWP_ANIMATE_FLAGS);

                // 2. Then drop the cloak and force the repaint
                if (win.cloaked) {
                    SetWindowAlphaSimple(win.hWnd, 255);
                    // Force a synchronous repaint on the first visible frame to render text
                    RedrawWindow(win.hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
                    win.cloaked = false;
                }
            }
        }
        
        if (!anyValid) {
            interrupted = true;
            break;
        }
        
        QueryPerformanceCounter(&now);
        float frameElapsed = (float)(now.QuadPart - start.QuadPart) * (float)g_perfFreqMsRecip;
        float nextFrame = elapsed + FRAME_DELAY_MS;
        if (nextFrame > frameElapsed) Sleep((DWORD)(nextFrame - frameElapsed));
    }

    if (!interrupted && g_modRunning.load(std::memory_order_acquire)) {
        for (int i = 0; i < group->windowCount; i++) {
            TargetWindow& win = group->windows[i];
            if (ValidateWindow(win)) {
                RemovePropW(win.hWnd, SHADOW_HIDDEN_PROP);
                if (group->animMode == ANIM_NATIVE && !win.isShadow) {
                    ApplyNativeClipping(win, win.targetY, group->cornerRadius);
                    SetWindowPos_Original(win.hWnd, NULL, win.targetX, win.targetY, 0, 0, SWP_ANIMATE_FLAGS);
                    SetWindowAlphaSimple(win.hWnd, 255);
                }
                else if (group->animMode == ANIM_FADE || group->animMode == ANIM_FADE_SLIDE) {
                     if (win.visuals.isValid) {
                        POINT pt = { win.targetX, win.targetY };
                        SIZE sz = win.visuals.size;
                        POINT ptSrc = win.visuals.srcPoint;
                        UpdateLayeredWindow_Original(win.hWnd, NULL, &pt, &sz, 
                                                   win.visuals.hdcMemory, &ptSrc, 
                                                   0, &win.visuals.originalBlend, ULW_ALPHA);
                     } else {
                        SetWindowAlphaSimple(win.hWnd, 255);
                        if (group->animMode == ANIM_FADE_SLIDE) SetWindowPos_Original(win.hWnd, NULL, win.targetX, win.targetY, 0, 0, SWP_ANIMATE_FLAGS);
                     }
                } 
                else if (group->animMode == ANIM_NATIVE && win.isShadow) {
                     if (win.visuals.isValid) {
                        POINT pt = { win.targetX, win.targetY };
                        SIZE sz = win.visuals.size;
                        POINT ptSrc = win.visuals.srcPoint;
                        UpdateLayeredWindow_Original(win.hWnd, NULL, &pt, &sz, 
                                                   win.visuals.hdcMemory, &ptSrc, 
                                                   0, &win.visuals.originalBlend, ULW_ALPHA);
                     } else {
                        SetWindowAlphaSimple(win.hWnd, 255);
                     }
                }
                else {
                    SetWindowPos_Original(win.hWnd, NULL, win.targetX, win.targetY, 0, 0, SWP_ANIMATE_FLAGS);
                    if (group->animMode == ANIM_SLIDE && !win.isShadow) {
                        // Final safety invalidate to guarantee hover states and rendering are perfectly synced
                        RedrawWindow(win.hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
                    }
                }
            }
        }
    }
}

static DWORD WINAPI PersistentAnimationThread(LPVOID) {
    // Bump to HIGHEST to combat CPU starvation and ensure smooth UI scheduling
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST); 
    
    // Bind 1ms timer resolution to the thread lifecycle to prevent kernel scheduler thrashing
    timeBeginPeriod(1); 
    
    while (g_modRunning.load(std::memory_order_acquire)) {
        AnimationGroup* group = nullptr;
        {
            std::unique_lock lock(g_queueMutex);
            g_queueCV.wait(lock, [] { 
                return !g_animationQueue.empty() || !g_modRunning.load(std::memory_order_acquire); 
            });
            if (!g_modRunning.load(std::memory_order_acquire)) break;
            if (!g_animationQueue.empty()) {
                group = g_animationQueue.front();
                g_animationQueue.pop();
            }
        }
        if (group) {
            RunAnimation(group);
            FreeGroup(group);
        }
    }
    
    timeEndPeriod(1); // Release global timer resolution only when the mod unloads
    return 0;
}

static void EnsureAnimationThread() {
    // Use an atomic exchange instead of std::once_flag for safe mod reloading
    bool expected = false;
    if (g_threadInitialized.compare_exchange_strong(expected, true)) {
        if (g_modRunning.load(std::memory_order_acquire)) {
            g_animationThread = CreateThread(NULL, ANIMATION_THREAD_STACK_SIZE, PersistentAnimationThread, NULL, 0, NULL);
        }
    }
}

static bool QueueAnimation(AnimationGroup* group) {
    if (!group || group->windowCount == 0) {
        FreeGroup(group);
        return false;
    }

    bool shouldAbort = false;

    EnsureAnimationThread();
    {
        // Strictly scope the mutex to queue operations only
        std::lock_guard lock(g_queueMutex);
        if (!g_modRunning.load(std::memory_order_acquire) || g_animationQueue.size() >= MAX_QUEUE_DEPTH) {
            shouldAbort = true;
        } else {
            g_animationQueue.push(group);
        }
    }

    // Execute UI changes OUTSIDE the lock to prevent Explorer deadlocks
    if (shouldAbort) {
        for (int i = 0; i < group->windowCount; i++) {
            TargetWindow& win = group->windows[i];
            if (IsWindow(win.hWnd)) {
                SetWindowAlphaSimple(win.hWnd, 255);
                RemovePropW(win.hWnd, HANDLED_PROP);
                if (group->animMode == ANIM_NATIVE && !win.isShadow) {
                    ApplyNativeClipping(win, win.targetY, group->cornerRadius);
                }
                SetWindowPos_Original(win.hWnd, NULL, win.targetX, win.targetY, 0, 0, SWP_ANIMATE_FLAGS);
            }
        }
        FreeGroup(group);
        return false;
    }

    g_queueCV.notify_one();
    return true;
}

// -------------------------------------------------------------------------
// Enumeration
// -------------------------------------------------------------------------
struct EnumData {
    DWORD targetThreadId;
    HWND sourceWnd;
    HWND foundWindows[MAX_ENUM_WINDOWS];
    WindowClassInfo windowInfo[MAX_ENUM_WINDOWS];
    int windowCount;
    HWND parentMenu;
    bool hasNilesoft;
};

static BOOL CALLBACK EnumThreadWndProc(HWND hWnd, LPARAM lParam) {
    EnumData* pData = (EnumData*)lParam;
    
    WindowClassInfo info = GetWindowClassInfo(hWnd);
    
    // Track if thread has a Nilesoft window
    if (info.type == WINDOW_NILESOFT) pData->hasNilesoft = true;

    // Only consume array slots for valid, non-blacklisted windows
    if (info.type != WINDOW_NONE && !info.isBlacklisted) {
        if (pData->windowCount < MAX_ENUM_WINDOWS) {
            pData->foundWindows[pData->windowCount] = hWnd;
            pData->windowInfo[pData->windowCount] = info;
            pData->windowCount++;
        }
    }

    // Find the parent menu reference
    if (IsWindowVisible(hWnd) && hWnd != pData->sourceWnd && info.type == WINDOW_NILESOFT) {
        if (pData->parentMenu == NULL) {
            RECT r; 
            if (GetWindowRect(hWnd, &r) && (r.right - r.left) > MIN_PARENT_WIDTH) {
                pData->parentMenu = hWnd; 
            }
        }
    }
    
    return TRUE; // ALWAYS return TRUE to ensure the entire thread is scanned
}

// -------------------------------------------------------------------------
// Trigger Logic
// -------------------------------------------------------------------------
static void TriggerAnimation(HWND sourceWnd, int x, int y, const AnimationParams& params, 
                             HDC hdcSrc, SIZE* sizeSrc, BLENDFUNCTION* blendSrc) {
    EnumData data = {};
    data.targetThreadId = GetWindowThreadProcessId(sourceWnd, NULL);
    data.sourceWnd = sourceWnd;
    EnumThreadWindows(data.targetThreadId, EnumThreadWndProc, (LPARAM)&data);

    if (!data.hasNilesoft) return;

    enum MenuType { TYPE_MAIN, TYPE_SUB_RIGHT, TYPE_SUB_LEFT };
    MenuType currentType = TYPE_MAIN;

    if (data.parentMenu != NULL) {
        RECT parentRect;
        if (GetWindowRect(data.parentMenu, &parentRect)) {
            currentType = (x >= parentRect.left) ? TYPE_SUB_RIGHT : TYPE_SUB_LEFT;
        }
    }

    int actualDelay = (currentType == TYPE_MAIN) ? params.contextMenuDelay : 0;

    RECT shadowRect; 
    if (!GetWindowRect(sourceWnd, &shadowRect)) return;
    int shadowW = shadowRect.right - shadowRect.left;
    int shadowH = shadowRect.bottom - shadowRect.top;

    AnimationGroup* group = AllocateGroup();
    if (!group) return;
    
    group->animDuration = params.animDuration;
    group->animMode = params.animMode;
    group->cornerRadius = params.cornerRadius;
    group->bX1 = params.bX1; group->bY1 = params.bY1;
    group->bX2 = params.bX2; group->bY2 = params.bY2;
    group->startupDelay = actualDelay;

    RECT safeZone = { 
        x - SAFE_ZONE_MARGIN, y - SAFE_ZONE_MARGIN, 
        x + shadowW + SAFE_ZONE_MARGIN, y + shadowH + SAFE_ZONE_MARGIN 
    };

    auto AddWindow = [&](HWND wnd, int tx, int ty, bool isSource) {
        if (group->windowCount >= MAX_ANIM_WINDOWS) return;
        
        TargetWindow& win = group->windows[group->windowCount++];
        win.hWnd = wnd;
        win.threadId = data.targetThreadId;
        WindowClassInfo info = GetWindowClassInfo(wnd);
        win.type = info.type;
        win.targetX = tx;
        win.targetY = ty;
        win.dpiScale = params.dpiScale;
        
        RECT wR; GetWindowRect(wnd, &wR);
        win.width = wR.right - wR.left;
        win.height = wR.bottom - wR.top;
        
        win.isShadow = CheckIsShadow(wnd);

        // --- Logic for Animation Modes ---

        if (params.animMode == ANIM_FADE) {
            win.startY = ty;
            if (isSource) CaptureVisuals(win, hdcSrc, sizeSrc, blendSrc);
            else SetWindowAlphaSimple(wnd, 0);
            if (!isSource) SetWindowPos_Original(wnd, NULL, tx, ty, 0, 0, SWP_ANIMATE_FLAGS);
        } 
        else if (params.animMode == ANIM_FADE_SLIDE) {
            win.startY = CalculateStartY(params, ty, win.height);
            if (isSource) CaptureVisuals(win, hdcSrc, sizeSrc, blendSrc);
            else SetWindowAlphaSimple(wnd, 0);
            if (!isSource) SetWindowPos_Original(wnd, NULL, tx, win.startY, 0, 0, SWP_ANIMATE_FLAGS);
        }
        else if (params.animMode == ANIM_NATIVE) {
            if (win.isShadow) {
                win.startY = ty;
                if (isSource) {
                    CaptureVisuals(win, hdcSrc, sizeSrc, blendSrc);
                    if (win.visuals.isValid) {
                        BLENDFUNCTION hiddenBlend = win.visuals.originalBlend;
                        hiddenBlend.SourceConstantAlpha = 0;
                        POINT pt = { win.targetX, win.startY };
                        SIZE sz = win.visuals.size;
                        POINT ptSrc = win.visuals.srcPoint;
                        UpdateLayeredWindow_Original(wnd, NULL, &pt, &sz, win.visuals.hdcMemory, &ptSrc, 0, &hiddenBlend, ULW_ALPHA);
                    }
                } 
            } else {
                win.startY = CalculateStartY(params, ty, win.height);
                
                HRGN empty = CreateRectRgn(0, 0, 0, 0);
                if (SetWindowRgn(wnd, empty, TRUE) == 0) { DeleteObject(empty); } // Force instant region update
                SetWindowAlphaSimple(wnd, 0); 
                win.cloaked = true;
                
                // Spawn far off-screen to absolutely prevent a 1-frame flash before the thread grabs it
                SetWindowPos_Original(wnd, NULL, -32000, -32000, 0, 0, SWP_ANIMATE_FLAGS | SWP_NOREDRAW | SWP_SHOWWINDOW);
            }
        }
        else { // ANIM_SLIDE
             win.startY = CalculateStartY(params, ty, win.height);
             
             if (!win.isShadow) {
                 if (params.cornerRadius > 0) {
                     int r = (int)((float)params.cornerRadius * win.dpiScale);
                     HRGN rgn = CreateRoundRectRgn(0, 0, win.width + 1, win.height + 1, r, r);
                     if (SetWindowRgn(wnd, rgn, TRUE) == 0) { DeleteObject(rgn); } // Force instant region update
                 }
                 SetWindowAlphaSimple(wnd, 0);
                 win.cloaked = true;
             }
             if (!isSource) SetWindowPos_Original(wnd, NULL, -32000, -32000, 0, 0, SWP_ANIMATE_FLAGS);
        }

        SetPropW(wnd, HANDLED_PROP, (HANDLE)1);
    };

    AddWindow(sourceWnd, x, y, true);

    for (int i = 0; i < data.windowCount && group->windowCount < MAX_ANIM_WINDOWS; i++) {
        HWND sibling = data.foundWindows[i];
        if (sibling == sourceWnd) continue;
        if (GetPropW(sibling, HANDLED_PROP)) continue;
        
        WindowClassInfo sibInfo = data.windowInfo[i];
        if (sibInfo.isBlacklisted || sibInfo.type == WINDOW_NONE) continue;

        RECT sibR; 
        if (!GetWindowRect(sibling, &sibR)) continue;
        int sibW = sibR.right - sibR.left;
        int sibH = sibR.bottom - sibR.top;
        if (sibW < MIN_WINDOW_SIZE || sibH < MIN_WINDOW_SIZE) continue;

        if (IsWindowVisible(sibling)) {
            RECT intersect;
            if (!IntersectRect(&intersect, &safeZone, &sibR)) continue;
        }

        int centerX = (shadowW - sibW) / 2;
        int centerY = (shadowH - sibH) / 2;
        int fixX = 0, fixY = 0;
        
        if (sibInfo.type == WINDOW_MENU) {
            switch (currentType) {
                case TYPE_MAIN: fixX = params.fixMainX; fixY = params.fixMainY; break;
                case TYPE_SUB_RIGHT: fixX = params.fixSubRightX; fixY = params.fixSubRightY; break;
                case TYPE_SUB_LEFT: fixX = params.fixSubLeftX; fixY = params.fixSubLeftY; break;
            }
        }
        AddWindow(sibling, x + centerX + fixX, y + centerY + fixY, false);
    }

    QueueAnimation(group);
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------
static BOOL WINAPI UpdateLayeredWindow_Hook(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, 
                                            HDC hdcSrc, POINT *pptSrc, COLORREF crKey, 
                                            BLENDFUNCTION *pblend, DWORD dwFlags) {
    if (GetPropW(hWnd, HANDLED_PROP)) {
        return UpdateLayeredWindow_Original(hWnd, hdcDst, NULL, psize, hdcSrc, pptSrc, crKey, pblend, dwFlags);
    }

    if (!IsWindowVisible(hWnd) && !GetPropW(hWnd, HANDLED_PROP)) {
        WindowClassInfo info = GetWindowClassInfo(hWnd);
        if (info.type != WINDOW_NONE && !info.isBlacklisted) {
            int finalX, finalY;
            if (pptDst) { finalX = pptDst->x; finalY = pptDst->y; } 
            else { 
                RECT r; 
                if (!GetWindowRect(hWnd, &r)) return UpdateLayeredWindow_Original(hWnd, hdcDst, pptDst, psize, hdcSrc, pptSrc, crKey, pblend, dwFlags);
                finalX = r.left; finalY = r.top;
            }
            if (finalX == 0 && finalY == 0) return UpdateLayeredWindow_Original(hWnd, hdcDst, pptDst, psize, hdcSrc, pptSrc, crKey, pblend, dwFlags);

            AnimationParams params = CalculateAnimationParams(hWnd, finalY);
            
            bool needsCapture = (params.animMode == ANIM_FADE || params.animMode == ANIM_FADE_SLIDE);
            if (params.animMode == ANIM_NATIVE && CheckIsShadow(hWnd)) needsCapture = true;

            if (needsCapture) {
                if (hdcSrc && psize && pblend) {
                    TriggerAnimation(hWnd, finalX, finalY, params, hdcSrc, psize, pblend);
                    BLENDFUNCTION hiddenBlend = *pblend;
                    hiddenBlend.SourceConstantAlpha = 0;
                    return UpdateLayeredWindow_Original(hWnd, hdcDst, pptDst, psize, hdcSrc, pptSrc, crKey, &hiddenBlend, dwFlags);
                }
            } 
            else { 
                POINT modPt;
                modPt.x = finalX;
                RECT r; GetWindowRect(hWnd, &r);
                modPt.y = CalculateStartY(params, finalY, r.bottom - r.top);
                
                // For Native mode, we must trigger first to handle clipping/shadow setup
                if (params.animMode == ANIM_NATIVE) {
                    TriggerAnimation(hWnd, finalX, finalY, params, NULL, NULL, NULL);
                    
                    // Zero out the alpha on the original call to prevent a 1-frame shadow flash
                    BLENDFUNCTION hiddenBlend;
                    if (pblend) {
                        hiddenBlend = *pblend;
                        hiddenBlend.SourceConstantAlpha = 0;
                    }
                    
                    // Pass the original coordinates, not modPt, to prevent the 50% vertical offset snap
                    POINT targetPt = { finalX, finalY };
                    return UpdateLayeredWindow_Original(hWnd, hdcDst, pptDst ? pptDst : &targetPt, psize, hdcSrc, pptSrc, crKey, pblend ? &hiddenBlend : NULL, dwFlags);
                } 
                
                // For Slide mode, we modify the call parameters first
                // then trigger the animation thread.
                BOOL res = UpdateLayeredWindow_Original(hWnd, hdcDst, &modPt, psize, hdcSrc, pptSrc, crKey, pblend, dwFlags);
                if (res) TriggerAnimation(hWnd, finalX, finalY, params, NULL, NULL, NULL);
                return res;
            }
        }
    }
    return UpdateLayeredWindow_Original(hWnd, hdcDst, pptDst, psize, hdcSrc, pptSrc, crKey, pblend, dwFlags);
}

static BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    // Reset handled state when the menu is hidden so cached HWNDs can animate again later
    if (uFlags & SWP_HIDEWINDOW) {
        RemovePropW(hWnd, HANDLED_PROP);
    }

    if (GetPropW(hWnd, HANDLED_PROP)) {
        UINT newFlags = uFlags | SWP_NOMOVE | SWP_NOSIZE;
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, newFlags);
    }

    if ((uFlags & SWP_SHOWWINDOW) && !IsWindowVisible(hWnd) && !GetPropW(hWnd, HANDLED_PROP)) {
        WindowClassInfo info = GetWindowClassInfo(hWnd);
        if (info.type != WINDOW_NONE && !info.isBlacklisted) {
            if (X == 0 && Y == 0) return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

            AnimationParams params = CalculateAnimationParams(hWnd, Y);

            if (params.animMode == ANIM_NATIVE) {
                if (CheckIsShadow(hWnd)) {
                    SetWindowAlphaSimple(hWnd, 0);
                    BOOL res = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
                    if (res) TriggerAnimation(hWnd, X, Y, params, NULL, NULL, NULL);
                    return res;
                } 
                else {
                    // CONTENT WINDOWS
                    TriggerAnimation(hWnd, X, Y, params, NULL, NULL, NULL);
                    return TRUE; // Suppress original call
                }
            }
        }
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

static BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    // Reset handled state on hide
    if (nCmdShow == SW_HIDE || nCmdShow == SW_MINIMIZE) {
        RemovePropW(hWnd, HANDLED_PROP);
    }
    
    if (nCmdShow != SW_HIDE && nCmdShow != SW_MINIMIZE && 
        !IsWindowVisible(hWnd) && !GetPropW(hWnd, HANDLED_PROP)) {
        WindowClassInfo info = GetWindowClassInfo(hWnd);
        if (info.type != WINDOW_NONE && !info.isBlacklisted) {
            RECT r; 
            if (!GetWindowRect(hWnd, &r) || (r.left == 0 && r.top == 0)) return ShowWindow_Original(hWnd, nCmdShow);
            
            AnimationParams params = CalculateAnimationParams(hWnd, r.top);

            if (params.animMode == ANIM_NATIVE) {
                if (CheckIsShadow(hWnd)) {
                    // Let shadows proceed normally (TriggerAnimation handles the hiding of the Source)
                    SetWindowAlphaSimple(hWnd, 0);
                    SetWindowPos_Original(hWnd, NULL, r.left, r.top, 0, 0, SWP_ANIMATE_FLAGS);
                    TriggerAnimation(hWnd, r.left, r.top, params, NULL, NULL, NULL);
                    return ShowWindow_Original(hWnd, nCmdShow);
                } else {
                     // CONTENT WINDOWS
                     // 1. Trigger setup (includes SWP_SHOWWINDOW with Alpha 0)
                     TriggerAnimation(hWnd, r.left, r.top, params, NULL, NULL, NULL);
                     
                     // 2. SUPPRESS ORIGINAL CALL
                     // Prevents the "Flash" of the final frame.
                     return TRUE;
                }
            }
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

// -------------------------------------------------------------------------
// Init / Uninit
// -------------------------------------------------------------------------
void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();
}

BOOL Wh_ModInit() {
    Wh_Log(L"Animated Nilesoft Shell loaded");
    
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    g_perfFreqMsRecip = 1000.0 / (double)freq.QuadPart;
    
    LoadSettings();
    g_modRunning.store(true, std::memory_order_release);
    g_threadInitialized.store(false);
    
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (!hUser32) return FALSE;
    
    void* pUpdateLayeredWindow = (void*)GetProcAddress(hUser32, "UpdateLayeredWindow");
    void* pSetWindowPos = (void*)GetProcAddress(hUser32, "SetWindowPos");
    void* pShowWindow = (void*)GetProcAddress(hUser32, "ShowWindow");
    pSetLayeredWindowAttributes = (SetLayeredWindowAttributes_t)GetProcAddress(hUser32, "SetLayeredWindowAttributes");
    
    if (!pUpdateLayeredWindow || !pSetWindowPos || !pShowWindow) return FALSE;
    
    Wh_SetFunctionHook(pUpdateLayeredWindow, (void*)UpdateLayeredWindow_Hook, (void**)&UpdateLayeredWindow_Original);
    Wh_SetFunctionHook(pSetWindowPos, (void*)SetWindowPos_Hook, (void**)&SetWindowPos_Original);
    Wh_SetFunctionHook(pShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    
    return TRUE;
}

// Cleanup lingering tags on cached menus so reloaded mods don't ignore them
static BOOL CALLBACK UnloadEnumProc(HWND hWnd, LPARAM) {
    RemovePropW(hWnd, HANDLED_PROP);
    RemovePropW(hWnd, SHADOW_HIDDEN_PROP);
    return TRUE;
}

void Wh_ModUninit() {
    g_modRunning.store(false, std::memory_order_release);
    g_queueCV.notify_all();
    if (g_animationThread) {
        WaitForSingleObject(g_animationThread, THREAD_SHUTDOWN_TIMEOUT_MS);
        CloseHandle(g_animationThread);
    }
    
    // Clean up abandoned animations to prevent memory and GDI handle leaks
    std::lock_guard lock(g_queueMutex);
    while (!g_animationQueue.empty()) {
        FreeGroup(g_animationQueue.front());
        g_animationQueue.pop();
    }

    // Strip tags from all cached Nilesoft menus on unload
    EnumWindows(UnloadEnumProc, 0);
}