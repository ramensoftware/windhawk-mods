// ==WindhawkMod==
// @id              taskbar-fade
// @name            Taskbar Fade
// @description     Reduces visual clutter by automatically dimming or hiding the taskbar when idle. Ideal for focused workflows and preventing OLED burn-in.
// @version         1.0
// @author          Lukvbp
// @github          https://github.com/lukvbp
// @include         explorer.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- DimTransparency: 50
  $name: Default Transparency (%)
  $description: How see-through the bar is when active. (0=Solid, 100=Invisible).
- EnableIdleFade: true
  $name: Enable Idle Hiding
  $description: If checked, the taskbar will vanish completely when you are away.
- IdleTimeout: 15
  $name: Idle Timeout (Seconds)
  $description: Time before the taskbar disappears (if Idle Hiding is enabled).
- FadeDuration: 200
  $name: Wake/Dim Speed (ms)
  $description: How fast it animates when you are using the PC.
- IdleFadeDuration: 2000
  $name: Idle Fade Speed (ms)
  $description: How fast it fades out when you walk away.
- TargetFPS: 30
  $name: Animation Smoothness (FPS)
  $description: Higher is smoother but uses more CPU. (Range 15 to 120).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>
#include <thread>
#include <vector>

// Global State
std::atomic<bool> g_stopThread(false);
std::thread g_workerThread;
double g_currentAlpha = 255.0;

// Settings Cache
struct {
    bool enableIdleFade;
    int idleTimeout;
    int dimTransparency;
    int fadeDuration;
    int idleFadeDuration;
    int frameTimeMs; 
} g_cache;

// Holds the handle and current style of a taskbar
struct TaskbarState {
    HWND hwnd;
    bool isLayered;
};

// --- SETTINGS ---
void LoadSettings() {
    g_cache.enableIdleFade = Wh_GetIntSetting(L"EnableIdleFade");
    
    int rawTimeout = Wh_GetIntSetting(L"IdleTimeout");
    int rawTransp = Wh_GetIntSetting(L"DimTransparency");
    int rawFade = Wh_GetIntSetting(L"FadeDuration");
    int rawIdleFade = Wh_GetIntSetting(L"IdleFadeDuration");
    int rawFPS = Wh_GetIntSetting(L"TargetFPS");

    if (rawTimeout < 1) rawTimeout = 1;
    if (rawTransp < 0) rawTransp = 0;
    if (rawTransp > 100) rawTransp = 100;
    if (rawFPS < 15) rawFPS = 15;
    if (rawFPS > 120) rawFPS = 120;

    int calculatedFrameTime = 1000 / rawFPS;

    // Ensure animation is not faster than one frame
    if (rawFade < calculatedFrameTime) rawFade = calculatedFrameTime;
    if (rawIdleFade < calculatedFrameTime) rawIdleFade = calculatedFrameTime;

    g_cache.idleTimeout = rawTimeout;
    g_cache.dimTransparency = rawTransp;
    g_cache.fadeDuration = rawFade;
    g_cache.idleFadeDuration = rawIdleFade;
    g_cache.frameTimeMs = calculatedFrameTime;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

// --- HELPERS ---

bool IsMouseOverWindow(HWND hwnd, POINT cursorPt) {
    if (!hwnd || !IsWindowVisible(hwnd)) return false;
    RECT rect;
    GetWindowRect(hwnd, &rect);
    return PtInRect(&rect, cursorPt);
}

// Resets the window style to ensure it is ready for transparency commands
void KickWindow(HWND hwnd) {
    if (!hwnd) return;
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    
    // Toggle the Layered style ON and OFF to refresh the window
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
    
    // Trigger a frame redraw
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

// Updates the window opacity and switches modes (Native vs Transparent)
void UpdateBarOpacity(TaskbarState& bar, int alpha) {
    if (!bar.hwnd) return;

    bool needLayered = (alpha < 255);

    // If the required mode changed, update the window style
    if (bar.isLayered != needLayered) {
        LONG_PTR exStyle = GetWindowLongPtr(bar.hwnd, GWL_EXSTYLE);
        
        if (needLayered) {
            SetWindowLongPtr(bar.hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        } else {
            SetWindowLongPtr(bar.hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        }

        SetWindowPos(bar.hwnd, NULL, 0, 0, 0, 0, 
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            
        bar.isLayered = needLayered;
    }

    // Apply the specific alpha value if in transparent mode
    if (needLayered) {
        SetLayeredWindowAttributes(bar.hwnd, 0, (BYTE)alpha, LWA_ALPHA);
    }
}

// --- WORKER LOOP ---
void WorkerLoop() {
    Sleep(500); 

    std::vector<TaskbarState> taskbars;

    // Find the Main Taskbar
    HWND hMain = FindWindow(L"Shell_TrayWnd", NULL);
    if (hMain) {
        KickWindow(hMain);
        taskbars.push_back({ hMain, false });
    }

    // Find all Secondary Taskbars (Multi-Monitor)
    HWND hSec = NULL;
    while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        KickWindow(hSec);
        taskbars.push_back({ hSec, false });
    }

    LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };

    while (!g_stopThread) {
        // Calculate transparency values
        int opacityPercent = 100 - g_cache.dimTransparency;
        int defaultDim = (opacityPercent * 255) / 100;

        // Check for User Idle
        GetLastInputInfo(&lii);
        DWORD timeoutMS = g_cache.idleTimeout * 1000;
        DWORD idleTime = GetTickCount() - lii.dwTime;

        // Check for Mouse Hover
        POINT pt;
        GetCursorPos(&pt);
        
        bool isHovering = false;
        for (const auto& bar : taskbars) {
            if (IsMouseOverWindow(bar.hwnd, pt)) {
                isHovering = true;
                break; 
            }
        }

        // Determine Target Opacity and Speed
        double target = 255.0;
        int activeDuration = g_cache.fadeDuration; 

        // LOGIC: Only trigger idle fade if the setting is Enabled AND we are idle.
        if (g_cache.enableIdleFade && (idleTime > timeoutMS)) {
            target = 0.0;   // Idle Mode
            activeDuration = g_cache.idleFadeDuration; 
        } else if (isHovering) {
            target = 255.0; // Active Mode
            activeDuration = g_cache.fadeDuration;
        } else {
            target = (double)defaultDim; // Dimmed Mode
            activeDuration = g_cache.fadeDuration; 
        }

        // Perform Animation Step
        double stepSize = (255.0 * (double)g_cache.frameTimeMs) / (double)activeDuration;

        if (g_currentAlpha < target) {
            g_currentAlpha += stepSize;
            if (g_currentAlpha > target) g_currentAlpha = target;
        } else if (g_currentAlpha > target) {
            g_currentAlpha -= stepSize;
            if (g_currentAlpha < target) g_currentAlpha = target;
        }

        // Clamp value to byte range (0-255)
        int applyAlpha = (int)g_currentAlpha;
        if (applyAlpha < 0) applyAlpha = 0;
        if (applyAlpha > 255) applyAlpha = 255;

        // Apply new opacity to all bars
        for (auto& bar : taskbars) {
            UpdateBarOpacity(bar, applyAlpha);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(g_cache.frameTimeMs));
    }
}

// --- INIT ---
BOOL Wh_ModInit() {
    LoadSettings();
    g_stopThread = false;
    g_workerThread = std::thread(WorkerLoop);
    return TRUE;
}

void Wh_ModUninit() {
    g_stopThread = true;
    if (g_workerThread.joinable()) g_workerThread.join();
    
    // Restore all taskbars to their original native style
    HWND hMain = FindWindow(L"Shell_TrayWnd", NULL);
    if (hMain) {
        LONG_PTR exStyle = GetWindowLongPtr(hMain, GWL_EXSTYLE);
        SetWindowLongPtr(hMain, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        SetWindowPos(hMain, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    HWND hSec = NULL;
    while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        LONG_PTR exStyle = GetWindowLongPtr(hSec, GWL_EXSTYLE);
        SetWindowLongPtr(hSec, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        SetWindowPos(hSec, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
