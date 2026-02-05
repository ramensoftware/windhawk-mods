// ==WindhawkMod==
// @id              taskbar-fade
// @name            Taskbar Fade
// @description     Reduces visual clutter by automatically dimming or hiding the taskbar when idle. Ideal for focused workflows and preventing OLED burn-in.
// @version         1.5
// @author          Lukvbp
// @github          https://github.com/lukvbp
// @include         windhawk.exe
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
#include <cmath>
#include <cstdio>
#include <mutex>

// --- MOD LOGIC ---

// Global Coordination
std::atomic<bool> g_stopThread(false);
std::thread g_workerThread;
std::mutex g_settingsMutex;

// Runtime Settings Cache
struct {
    bool enableIdleFade;
    DWORD idleTimeoutMS;
    int fadeDuration;
    int idleFadeDuration;
    int frameTimeMs; 
    double targetDimAlpha;
} g_cache;

// Window Tracking
struct TaskbarState {
    HWND hwnd;
    bool isLayered;
    int lastAlpha;
};

// --- SETTINGS MANAGEMENT ---
void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    bool enableIdle = Wh_GetIntSetting(L"EnableIdleFade");
    int rawTimeout = Wh_GetIntSetting(L"IdleTimeout");
    int rawTransp = Wh_GetIntSetting(L"DimTransparency");
    int rawFade = Wh_GetIntSetting(L"FadeDuration");
    int rawIdleFade = Wh_GetIntSetting(L"IdleFadeDuration");
    int rawFPS = Wh_GetIntSetting(L"TargetFPS");

    // Validation
    if (rawTimeout < 1) rawTimeout = 1;
    if (rawTransp < 0) rawTransp = 0;
    if (rawTransp > 100) rawTransp = 100;
    if (rawFPS < 15) rawFPS = 15;
    if (rawFPS > 120) rawFPS = 120;

    int calculatedFrameTime = 1000 / rawFPS;

    // Ensure animations are at least one frame long
    if (rawFade < calculatedFrameTime) rawFade = calculatedFrameTime;
    if (rawIdleFade < calculatedFrameTime) rawIdleFade = calculatedFrameTime;

    g_cache.enableIdleFade = enableIdle;
    g_cache.idleTimeoutMS = (DWORD)(rawTimeout * 1000);
    g_cache.fadeDuration = rawFade;
    g_cache.idleFadeDuration = rawIdleFade;
    g_cache.frameTimeMs = calculatedFrameTime;

    // Pre-calculate target alpha for the "Dimmed" state
    double opacityPercent = (100.0 - (double)rawTransp);
    g_cache.targetDimAlpha = (opacityPercent * 255.0) / 100.0;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

// --- WINDOW HELPERS ---

bool IsMouseOverWindow(HWND hwnd, POINT cursorPt) {
    if (!hwnd || !IsWindowVisible(hwnd)) return false;
    RECT rect;
    if (!GetWindowRect(hwnd, &rect)) return false;
    return PtInRect(&rect, cursorPt);
}

bool EnsureLayeredStyle(HWND hwnd, bool enable) {
    if (!hwnd || !IsWindow(hwnd)) return false;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    bool isCurrentlyLayered = (exStyle & WS_EX_LAYERED) != 0;

    if (enable && !isCurrentlyLayered) {
        SetLastError(0);
        LONG_PTR result = SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        if (result == 0 && GetLastError() != 0) {
            return false;
        }
    } else if (!enable && isCurrentlyLayered) {
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        // Force a redraw when removing transparency
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
    return true;
}

void UpdateBarOpacity(TaskbarState& bar, int alpha) {
    if (!bar.hwnd || !IsWindow(bar.hwnd)) return;

    bool needLayered = (alpha < 255);

    // 1. Manage Window Style (WS_EX_LAYERED)
    if (bar.isLayered != needLayered) {
        if (EnsureLayeredStyle(bar.hwnd, needLayered)) {
            bar.isLayered = needLayered;
            bar.lastAlpha = -1; // Style changed, force alpha update
        }
    }

    // 2. Apply Alpha (Only if needed AND value actually changed)
    if (needLayered) {
        if (bar.lastAlpha != alpha) {
            if (SetLayeredWindowAttributes(bar.hwnd, 0, (BYTE)alpha, LWA_ALPHA)) {
                bar.lastAlpha = alpha;
            }
        }
    }
}

// --- WORKER LOOP ---
void WorkerLoop() {
    std::vector<TaskbarState> taskbars;
    LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
    
    double currentAlpha = 255.0; 

    // Local settings snapshot for thread safety
    struct {
        bool enableIdleFade;
        DWORD idleTimeoutMS;
        int fadeDuration;
        int idleFadeDuration;
        int frameTimeMs; 
        double targetDimAlpha;
    } settings;

    while (!g_stopThread) {
        // --- 0. SNAPSHOT SETTINGS ---
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            settings.enableIdleFade = g_cache.enableIdleFade;
            settings.idleTimeoutMS = g_cache.idleTimeoutMS;
            settings.fadeDuration = g_cache.fadeDuration;
            settings.idleFadeDuration = g_cache.idleFadeDuration;
            settings.frameTimeMs = g_cache.frameTimeMs;
            settings.targetDimAlpha = g_cache.targetDimAlpha;
        }

        // --- 1. DISCOVERY / RECOVERY ---
        // Verify taskbar handles are still valid (handles Explorer crashes/restarts)
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
            HWND hMain = FindWindow(L"Shell_TrayWnd", NULL);
            if (hMain) {
                // Initialize lastAlpha to -1 to ensure first paint occurs
                taskbars.push_back({ hMain, false, -1 });
            }

            HWND hSec = NULL;
            while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
                taskbars.push_back({ hSec, false, -1 });
            }
            
            if (taskbars.empty()) {
                Sleep(1000); // Explorer likely not running, wait and retry
                continue;
            }
            currentAlpha = 255.0; 
        }

        // --- 2. INPUT DETECTION ---
        GetLastInputInfo(&lii);
        ULONGLONG tick = GetTickCount64();
        ULONGLONG lastInput = (ULONGLONG)lii.dwTime; 
        ULONGLONG idleTime = tick - lastInput;

        POINT pt;
        GetCursorPos(&pt);
        
        bool isHovering = false;
        for (const auto& bar : taskbars) {
            if (IsMouseOverWindow(bar.hwnd, pt)) {
                isHovering = true;
                break; 
            }
        }

        // --- 3. TARGET CALCULATION ---
        double target = 255.0;
        int activeDuration = settings.fadeDuration; 

        if (settings.enableIdleFade && (idleTime > settings.idleTimeoutMS)) {
            target = 0.0; // Idle State
            activeDuration = settings.idleFadeDuration; 
        } else if (isHovering) {
            target = 255.0; // Active State
            activeDuration = settings.fadeDuration;
        } else {
            target = settings.targetDimAlpha; // Dimmed State
            activeDuration = settings.fadeDuration; 
        }

        // --- 4. ANIMATION LOGIC ---
        bool isAnimating = false;
        
        if (std::abs(currentAlpha - target) > 0.5) {
            isAnimating = true;
            
            double stepSize = (255.0 * (double)settings.frameTimeMs) / (double)activeDuration;

            if (currentAlpha < target) {
                currentAlpha += stepSize;
                if (currentAlpha > target) currentAlpha = target;
            } else {
                currentAlpha -= stepSize;
                if (currentAlpha < target) currentAlpha = target;
            }

            // Clamp to byte range
            int applyAlpha = (int)currentAlpha;
            if (applyAlpha < 0) applyAlpha = 0;
            if (applyAlpha > 255) applyAlpha = 255;

            for (auto& bar : taskbars) {
                UpdateBarOpacity(bar, applyAlpha);
            }
        }

        // --- 5. DYNAMIC SLEEP ---
        if (isAnimating) {
            // High refresh rate only when visual updates are needed
            std::this_thread::sleep_for(std::chrono::milliseconds(settings.frameTimeMs));
        } else {
            // Low power polling (~15 checks/sec) to save CPU when static
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
    if (g_workerThread.joinable()) g_workerThread.join();
    
    // Cleanup: Restore original opacity
    HWND hMain = FindWindow(L"Shell_TrayWnd", NULL);
    if (hMain) EnsureLayeredStyle(hMain, false);

    HWND hSec = NULL;
    while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        EnsureLayeredStyle(hSec, false);
    }
}

// --- WINDHAWK TOOL BOILERPLATE (DO NOT EDIT) ---
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process

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
