// ==WindhawkMod==
// @id              taskbar-fade
// @name            Taskbar Fade
// @description     Reduces visual clutter by automatically dimming or hiding the taskbar when idle. Ideal for focused workflows and preventing OLED burn-in.
// @version         2.1
// @author          Lukvbp
// @github          https://github.com/lukvbp
// @include         windhawk.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Fade
Automatically dims or hides the taskbar when the computer is idle.

This mod is designed to prevent OLED burn-in and reduce visual distractions by lowering the taskbar's opacity when it is not in use.

### Features
* **Idle Dimming:** The taskbar fades to a lower transparency level (configurable) when the mouse is away.
* **Idle Hiding:** Optionally hides the taskbar completely after a set timeout.
* **Instant Wake:** Moving the mouse to the taskbar or the bottom of the screen restores full opacity immediately.
* **Smart Idle:** (Optional) Only hide the taskbar if the desktop is focused. If windows are open, keep the taskbar dimmed instead.
* **Clean Desktop:** (Optional) Hide desktop icons when the taskbar hides for a completely black screen.
* **Customization:** Settings available for fade speed, transparency levels, and timeout duration.

### Configuration
* **Default Transparency:** The opacity level when the taskbar is dimmed (0-100%).
* **Enable Idle Hiding:** Check this to make the taskbar vanish completely after a timeout.
* **Smart Idle:** If checked, idle hiding is disabled when any window is open/focused.
* **Hide Desktop Icons:** If checked, desktop icons will also vanish when idle.
* **Idle Timeout:** Seconds of inactivity before the taskbar hides.
* **Fade Speed:** Duration of the fade animation in milliseconds.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DimTransparency: 50
  $name: Default Transparency (%)
  $description: How see-through the bar is when active. (0=Solid, 100=Invisible).
- EnableIdleFade: true
  $name: Enable Idle Hiding
  $description: If checked, the taskbar will vanish completely when you are away.
- SmartIdle: false
  $name: Smart Idle (Only hide on empty desktop)
  $description: Prevents the taskbar from fully hiding if you have a window (like a browser) open and focused.
- HideDesktopIcons: false
  $name: Hide Desktop Icons on Idle
  $description: When the taskbar fully hides, also hide desktop icons for a perfectly clean screen.
- IdleTimeout: 15
  $name: Idle Timeout (Seconds)
  $description: Time before the taskbar disappears (if Idle Hiding is enabled).
- FadeDuration: 250
  $name: Wake/Dim Speed (ms)
  $description: How fast it animates when you are using the PC.
- IdleFadeDuration: 2000
  $name: Idle Fade Speed (ms)
  $description: How fast it fades out when you walk away.
- TargetFPS: 60
  $name: Animation Smoothness (FPS)
  $description: Higher is smoother but uses more CPU. (Range 15 to 120).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>
#include <thread>
#include <vector>
#include <cmath>
#include <mutex>

// --- MOD LOGIC ---

// Global Coordination
std::atomic<bool> g_stopThread(false);
std::thread g_workerThread;
std::mutex g_settingsMutex;

// State Machine
enum class FadeState {
    Active, // Mouse is hovering
    Dimmed, // Mouse away, but not yet idle (or idle hiding disabled)
    Idle    // Idle timeout reached (Overrides hover)
};

// Runtime Settings Cache
struct {
    bool enableIdleFade;
    bool smartIdle;
    bool hideDesktopIcons;
    DWORD idleTimeoutMs;
    int fadeDuration;
    int idleFadeDuration;
    int frameTimeMs;
    double targetDimAlpha;
} g_cache;

// Window Tracking
struct TaskbarState {
    HWND hwnd;
    bool isLayered;     // CACHE
    bool isTransparent; // CACHE
    int lastAlpha;
    RECT rect;          // Cached geometry
};

// --- SETTINGS ---
void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    bool enableIdle = Wh_GetIntSetting(L"EnableIdleFade") != 0;
    bool smartIdle = Wh_GetIntSetting(L"SmartIdle") != 0;
    bool hideIcons = Wh_GetIntSetting(L"HideDesktopIcons") != 0;

    int rawTimeout = Wh_GetIntSetting(L"IdleTimeout");
    int rawTransp = Wh_GetIntSetting(L"DimTransparency");
    int rawFade = Wh_GetIntSetting(L"FadeDuration");
    int rawIdleFade = Wh_GetIntSetting(L"IdleFadeDuration");
    int rawFPS = Wh_GetIntSetting(L"TargetFPS");

    if (rawTimeout < 1) {
        rawTimeout = 1;
    }
    if (rawTransp < 0) {
        rawTransp = 0;
    }
    if (rawTransp > 100) {
        rawTransp = 100;
    }

    // FPS Clamp: 15 to 120
    if (rawFPS < 15) {
        rawFPS = 15;
    }
    if (rawFPS > 120) {
        rawFPS = 120;
    }

    int calculatedFrameTime = 1000 / rawFPS;

    // Validate Durations
    if (rawFade < calculatedFrameTime) {
        rawFade = calculatedFrameTime;
    }
    if (rawIdleFade < calculatedFrameTime) {
        rawIdleFade = calculatedFrameTime;
    }

    g_cache.enableIdleFade = enableIdle;
    g_cache.smartIdle = smartIdle;
    g_cache.hideDesktopIcons = hideIcons;

    // Safety: (DWORD) cast prevents signed integer overflow during multiplication
    // if user enters a large duration (e.g. > 24 days)
    g_cache.idleTimeoutMs = (DWORD)rawTimeout * 1000;

    g_cache.fadeDuration = rawFade;
    g_cache.idleFadeDuration = rawIdleFade;
    g_cache.frameTimeMs = calculatedFrameTime;

    double opacityPercent = (100.0 - (double)rawTransp);
    g_cache.targetDimAlpha = (opacityPercent * 255.0) / 100.0;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

// --- WINDOW HELPERS ---

void RestoreTaskbarStyle(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }
    
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_LAYERED) || (exStyle & WS_EX_TRANSPARENT)) {
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~(WS_EX_LAYERED | WS_EX_TRANSPARENT));
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}

// Helper to find the Desktop Icons window (SysListView32)
// Handles both standard Progman and WorkerW (wallpaper engine / task view) cases
HWND GetDesktopListView() {
    HWND hProgman = FindWindowW(L"Progman", L"Program Manager");
    HWND hDefView = FindWindowExW(hProgman, NULL, L"SHELLDLL_DefView", NULL);

    if (!hDefView) {
        // Fallback: iterate WorkerW windows
        HWND hWorkerW = NULL;
        while ((hWorkerW = FindWindowExW(NULL, hWorkerW, L"WorkerW", NULL)) != NULL) {
            hDefView = FindWindowExW(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            if (hDefView) {
                break;
            }
        }
    }

    if (hDefView) {
        return FindWindowExW(hDefView, NULL, L"SysListView32", L"FolderView");
    }
    return NULL;
}

// 5-Stage Pipeline: Safe Ordering for Style Changes
void UpdateBarOpacity(TaskbarState& bar, int alpha) {
    if (!IsWindow(bar.hwnd)) {
        return;
    }

    // Early-Out if state is already correct
    if (bar.lastAlpha == alpha) {
        return;
    }

    bool needLayered = (alpha < 255);
    bool needTransparent = (alpha == 0);

    // 1. PRE-CLEANUP: Remove Transparent
    if (bar.isTransparent && !needTransparent) {
        LONG_PTR exStyle = GetWindowLongPtr(bar.hwnd, GWL_EXSTYLE);
        SetWindowLongPtr(bar.hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
        bar.isTransparent = false;
    }

    // 2. SETUP: Enable Layered
    if (!bar.isLayered && needLayered) {
        LONG_PTR exStyle = GetWindowLongPtr(bar.hwnd, GWL_EXSTYLE);
        SetWindowLongPtr(bar.hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        bar.isLayered = true;
        // Force update on next step
        bar.lastAlpha = -1;
    }

    // 3. APPLY: Set Alpha
    if (needLayered) {
        if (SetLayeredWindowAttributes(bar.hwnd, 0, (BYTE)alpha, LWA_ALPHA)) {
            bar.lastAlpha = alpha;
        }
    }

    // 4. POST-POLISH: Enable Transparent
    if (!bar.isTransparent && needTransparent) {
        LONG_PTR exStyle = GetWindowLongPtr(bar.hwnd, GWL_EXSTYLE);
        SetWindowLongPtr(bar.hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
        bar.isTransparent = true;
    }

    // 5. CLEANUP: Remove Layered
    if (bar.isLayered && !needLayered) {
        LONG_PTR exStyle = GetWindowLongPtr(bar.hwnd, GWL_EXSTYLE);
        SetWindowLongPtr(bar.hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        bar.isLayered = false;
        SetWindowPos(bar.hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}

// --- WORKER LOOP ---
void WorkerLoop() {
    std::vector<TaskbarState> taskbars;
    LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };

    // Animation State
    double currentAlpha = 255.0;
    double lastTarget = -1.0;
    double animStartAlpha = 255.0;
    FadeState lastState = FadeState::Dimmed;

    // Timing State
    ULONGLONG animStartTime = 0;
    ULONGLONG lastGeometryUpdate = 0;

    // Local snapshot variables
    bool enableIdleFade;
    bool smartIdle;
    bool hideDesktopIcons;
    bool isDesktopFocused = true; // Assumed true if smart idle is off
    DWORD idleTimeoutMs;
    int fadeDuration;
    int idleFadeDuration;
    int frameTimeMs;
    double targetDimAlpha;

    while (!g_stopThread) {
        // 0. Settings Snapshot
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            enableIdleFade = g_cache.enableIdleFade;
            smartIdle = g_cache.smartIdle;
            hideDesktopIcons = g_cache.hideDesktopIcons;
            idleTimeoutMs = g_cache.idleTimeoutMs;
            fadeDuration = g_cache.fadeDuration;
            idleFadeDuration = g_cache.idleFadeDuration;
            frameTimeMs = g_cache.frameTimeMs;
            targetDimAlpha = g_cache.targetDimAlpha;
        }

        ULONGLONG now = GetTickCount64();

        // 1. Discovery & Recovery
        bool needsRefresh = taskbars.empty();
        if (!needsRefresh) {
            for (const auto& bar : taskbars) {
                if (!IsWindow(bar.hwnd)) {
                    needsRefresh = true;
                    break;
                }
            }
        }

        if (needsRefresh) {
            taskbars.clear();

            auto AddTaskbar = [&](HWND h) {
                if (h) {
                    RECT r = {0}; 
                    GetWindowRect(h, &r);

                    // Ignore empty rectangles (e.g. native auto-hide active)
                    if (!IsRectEmpty(&r)) {
                        LONG_PTR style = GetWindowLongPtr(h, GWL_EXSTYLE);
                        bool isLayered = (style & WS_EX_LAYERED) != 0;
                        bool isTrans = (style & WS_EX_TRANSPARENT) != 0;
                        taskbars.push_back({ h, isLayered, isTrans, -1, r });
                    }
                }
            };

            AddTaskbar(FindWindow(L"Shell_TrayWnd", NULL));
            HWND hSec = NULL;
            while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
                AddTaskbar(hSec);
            }

            if (taskbars.empty()) {
                // Prevent CPU spin if no taskbars found (e.g. Explorer restarting)
                Sleep(250);
                continue;
            }

            // Reset Animation State
            currentAlpha = 255.0;
            lastTarget = 255.0;
            animStartAlpha = 255.0;
            animStartTime = now;
            lastGeometryUpdate = now;
            lastState = FadeState::Dimmed;
        }

        // 2. Geometry & Smart Idle Refresh (1Hz Timer)
        if (now - lastGeometryUpdate > 1000) {
            for (auto& bar : taskbars) {
                if (IsWindow(bar.hwnd)) {
                    RECT r = {0};
                    GetWindowRect(bar.hwnd, &r);
                    // Only update if geometry is valid
                    if (!IsRectEmpty(&r)) {
                        bar.rect = r;
                    }
                }
            }

            // Only check foreground window if Smart Idle is enabled
            if (smartIdle) {
                HWND hForeground = GetForegroundWindow();
                if (hForeground) {
                    WCHAR szClass[64];
                    if (GetClassNameW(hForeground, szClass, 64)) {
                        // Check if focused window is the Desktop or Taskbar itself
                        isDesktopFocused = (wcscmp(szClass, L"Progman") == 0 || 
                                            wcscmp(szClass, L"WorkerW") == 0 || 
                                            wcscmp(szClass, L"Shell_TrayWnd") == 0);
                    } else {
                        isDesktopFocused = true;
                    }
                } else {
                    isDesktopFocused = true;
                }
            } else {
                isDesktopFocused = true; // Always allow idle if smart check disabled
            }

            lastGeometryUpdate = now;
        }

        // 3. Logic: Determine State
        GetLastInputInfo(&lii);
        // 32-bit subtraction to handle rollover
        DWORD idleTime = (DWORD)now - lii.dwTime;

        POINT pt = {0};
        GetCursorPos(&pt);

        bool isHovering = false;
        for (const auto& bar : taskbars) {
            if (PtInRect(&bar.rect, pt)) { 
                isHovering = true; 
                break; 
            }
        }

        FadeState targetState = FadeState::Dimmed;

        // Condition: Enabled AND Timeout reached AND (Smart Idle check passed)
        if (enableIdleFade && idleTime > idleTimeoutMs && isDesktopFocused) {
            targetState = FadeState::Idle;
        } else if (isHovering) {
            targetState = FadeState::Active;
        }

        // 4. Reactive Geometry & State Updates
        if (targetState != lastState) {
            // Hide Desktop Icons
            if (hideDesktopIcons) {
                HWND hIcons = GetDesktopListView();
                if (hIcons) {
                    if (targetState == FadeState::Idle) {
                        ShowWindow(hIcons, SW_HIDE);
                    } else if (lastState == FadeState::Idle) {
                        ShowWindow(hIcons, SW_SHOW);
                    }
                }
            }

            // Force geometry refresh on state change to ensure hover rects are accurate
            for (auto& bar : taskbars) {
                if (IsWindow(bar.hwnd)) {
                    RECT r = {0};
                    GetWindowRect(bar.hwnd, &r);
                    if (!IsRectEmpty(&r)) {
                        bar.rect = r;
                    }
                }
            }
            lastGeometryUpdate = now;
            lastState = targetState;
        }

        // 5. Target Alpha Calculation
        double target = 255.0;
        int duration = fadeDuration;

        switch (targetState) {
            case FadeState::Active: 
                target = 255.0; 
                duration = fadeDuration; 
                break;
            case FadeState::Idle:   
                target = 0.0;   
                duration = idleFadeDuration; 
                break;
            case FadeState::Dimmed: 
                target = targetDimAlpha; 
                duration = fadeDuration; 
                break;
        }

        // 6. Animation (Stateful Cubic Ease-Out)
        bool isAnimating = false;

        // Exact float comparison (target only takes predefined values)
        if (target != lastTarget) {
            lastTarget = target;
            animStartAlpha = currentAlpha;
            animStartTime = now;
        }

        double dist = std::abs(currentAlpha - target);

        // SNAP THRESHOLD: 1.0 (Prevents sub-pixel CPU churn)
        if (dist >= 1.0) {
            isAnimating = true;

            ULONGLONG elapsed = now - animStartTime;

            if (elapsed >= (ULONGLONG)duration) {
                currentAlpha = target;
            } else {
                double t = (double)elapsed / (double)duration;
                if (t < 0.0) {
                    t = 0.0;
                }
                if (t > 1.0) {
                    t = 1.0;
                }

                double invT = 1.0 - t;
                double ease = 1.0 - (invT * invT * invT);

                currentAlpha = animStartAlpha + ((target - animStartAlpha) * ease);
            }

            int applyAlpha = (int)currentAlpha;
            if (applyAlpha < 0) {
                applyAlpha = 0;
            }
            if (applyAlpha > 255) {
                applyAlpha = 255;
            }

            for (auto& bar : taskbars) {
                UpdateBarOpacity(bar, applyAlpha);
            }
        } else {
            // Snap to exact target
            if (currentAlpha != target) {
                currentAlpha = target;
                for (auto& bar : taskbars) {
                    UpdateBarOpacity(bar, (int)currentAlpha);
                }
            }
        }

        // 7. Dynamic Sleep
        if (isAnimating) {
            std::this_thread::sleep_for(std::chrono::milliseconds(frameTimeMs));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(66));
        }
    }
}

// --- ENTRY POINTS ---

BOOL WhTool_ModInit() {
    LoadSettings();
    g_stopThread = false;
    g_workerThread = std::thread(WorkerLoop);
    return TRUE;
}

void WhTool_ModUninit() {
    g_stopThread = true;
    if (g_workerThread.joinable()) {
        g_workerThread.join();
    }

    RestoreTaskbarStyle(FindWindow(L"Shell_TrayWnd", NULL));
    HWND hSec = NULL;
    while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        RestoreTaskbarStyle(hSec);
    }

    // Safety: Ensure desktop icons are visible when mod unloads
    HWND hIcons = GetDesktopListView();
    if (hIcons && !IsWindowVisible(hIcons)) {
        ShowWindow(hIcons, SW_SHOW);
    }
}

// --- WINDHAWK TOOL BOILERPLATE (DO NOT EDIT) ---

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isService) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
