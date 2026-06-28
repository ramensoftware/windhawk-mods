// ==WindhawkMod==
// @id              borderless-fullscreen
// @name            Borderless Fullscreen
// @description     Removes the window border of selected apps/games and resizes them to fill the monitor they're running on (or a fixed size)
// @version         1.2.0
// @author          TomberWolf
// @github          https://github.com/TomberWolf
// @include         windhawk.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Borderless Fullscreen

Add an executable name (e.g. `game.exe`) to the target list and the app
will automatically run without a title bar / border, stretched to fill
the current resolution of whichever monitor it's running on.

Optionally, set a fixed window size instead (the window will then be
centered on that same monitor) - useful for older games with a low
native resolution.

## How it works

This mod runs in a separate, dedicated process and restores the original
window style and size automatically if the app is removed from the
target list, if the mod is disabled, or if the window is closed.

## Notes
- If a target doesn't react, double-check the *exact* process name in
  Task Manager's Details tab while it's running. Some launchers start a
  child process with a different executable name than expected.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- exeNames:
  - ""
  $name: Target Applications
  $description: >-
    Enter the executable file name (e.g. game.exe, no path) of each
    app/game that should run borderless. Click "+" to add more entries.
- windowSize:
  - customWidth: 0
    $name: Width
    $description: >-
      0 = automatically use the current width of the monitor the window
      is running on
  - customHeight: 0
    $name: Height
    $description: >-
      0 = automatically use the current height of the monitor the
      window is running on
  $name: Window Size
  $description: >-
    Leave both at 0 to automatically stretch to the full resolution of
    whichever monitor the window is running on. Set fixed values only
    if you want a smaller, centered window instead.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

std::mutex g_settingsMutex;
std::vector<std::wstring> g_targetExeNames;
std::atomic<int> g_customWidth{0};
std::atomic<int> g_customHeight{0};

std::vector<std::wstring> GetTargetExeNames() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_targetExeNames;
}

void GetTargetSize(int& width, int& height) {
    width = g_customWidth.load();
    height = g_customHeight.load();
}

void LoadSettings() {
    std::vector<std::wstring> exeNames;
    for (int i = 0;; i++) {
        PCWSTR value = Wh_GetStringSetting(L"exeNames[%d]", i);
        bool empty = !value || !value[0];
        if (!empty) {
            exeNames.push_back(value);
        }
        Wh_FreeStringSetting(value);
        if (empty) break;
    }

    int customWidth = Wh_GetIntSetting(L"windowSize.customWidth");
    int customHeight = Wh_GetIntSetting(L"windowSize.customHeight");

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_targetExeNames = std::move(exeNames);
    }
    g_customWidth = customWidth;
    g_customHeight = customHeight;
}

// ---------------------------------------------------------------------------
// Window/process helpers
// ---------------------------------------------------------------------------

bool GetExeNameForWindow(HWND hWnd, std::wstring& outName) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == 0) {
        return false;
    }

    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess) {
        return false;
    }

    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(hProcess, 0, path, &size);
    CloseHandle(hProcess);
    if (!ok) {
        return false;
    }

    WCHAR* fileName = wcsrchr(path, L'\\');
    outName = fileName ? (fileName + 1) : path;
    return true;
}

bool IsTargetExe(const std::wstring& exeName,
                  const std::vector<std::wstring>& targets) {
    for (const auto& target : targets) {
        if (_wcsicmp(target.c_str(), exeName.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

bool IsEligibleWindow(HWND hWnd) {
    if (!IsWindow(hWnd)) return false;
    if (!IsWindowVisible(hWnd)) return false;
    if (GetWindowTextLengthW(hWnd) == 0) return false;

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;

    RECT rc;
    if (!GetWindowRect(hWnd, &rc)) return false;
    if ((rc.right - rc.left) < 200 || (rc.bottom - rc.top) < 150) return false;

    return true;
}

long WindowArea(HWND hWnd) {
    RECT rc;
    if (!GetWindowRect(hWnd, &rc)) return 0;
    return (long)(rc.right - rc.left) * (long)(rc.bottom - rc.top);
}

struct BestWindowCtx {
    std::wstring exeName;
    HWND best = nullptr;
};

BOOL CALLBACK FindBestWindowProc(HWND hWnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<BestWindowCtx*>(lParam);

    if (!IsEligibleWindow(hWnd)) return TRUE;

    std::wstring exeName;
    if (!GetExeNameForWindow(hWnd, exeName)) return TRUE;
    if (_wcsicmp(exeName.c_str(), ctx->exeName.c_str()) != 0) return TRUE;

    if (!ctx->best || WindowArea(hWnd) > WindowArea(ctx->best)) {
        ctx->best = hWnd;
    }

    return TRUE;
}

// Among all currently visible top-level windows belonging to exeName,
// returns the largest one - used to avoid mistakenly targeting a splash
// screen or secondary tool window instead of the real main window.
HWND GetBestWindowForExe(const std::wstring& exeName) {
    BestWindowCtx ctx;
    ctx.exeName = exeName;
    EnumWindows(FindBestWindowProc, reinterpret_cast<LPARAM>(&ctx));
    return ctx.best;
}

// Returns the bounds (in physical pixels) of the monitor a window is
// currently on, regardless of this process' own DPI awareness setting.
RECT GetMonitorRectForWindow(HWND hWnd) {
    DPI_AWARENESS_CONTEXT previousContext = SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    RECT result{0, 0, GetSystemMetrics(SM_CXSCREEN),
                GetSystemMetrics(SM_CYSCREEN)};
    if (hMonitor && GetMonitorInfo(hMonitor, &mi)) {
        result = mi.rcMonitor;
    }

    if (previousContext) {
        SetThreadDpiAwarenessContext(previousContext);
    }

    return result;
}

void ComputeTargetRect(HWND hWnd, int& x, int& y, int& width, int& height) {
    int customWidth, customHeight;
    GetTargetSize(customWidth, customHeight);

    RECT monitorRect = GetMonitorRectForWindow(hWnd);
    int monitorW = monitorRect.right - monitorRect.left;
    int monitorH = monitorRect.bottom - monitorRect.top;

    if (customWidth <= 0 || customHeight <= 0) {
        width = monitorW;
        height = monitorH;
        x = monitorRect.left;
        y = monitorRect.top;
    } else {
        width = customWidth;
        height = customHeight;
        x = monitorRect.left + (monitorW - width) / 2;
        y = monitorRect.top + (monitorH - height) / 2;
    }
}

bool IsAlreadyBorderlessAndSized(HWND hWnd, int targetWidth, int targetHeight,
                                  int targetX, int targetY) {
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & (WS_CAPTION | WS_THICKFRAME)) {
        return false;
    }

    RECT rc;
    if (!GetWindowRect(hWnd, &rc)) {
        return false;
    }

    return rc.left == targetX && rc.top == targetY &&
           (rc.right - rc.left) == targetWidth &&
           (rc.bottom - rc.top) == targetHeight;
}

void ApplyBorderless(HWND hWnd) {
    if (IsZoomed(hWnd) || IsIconic(hWnd)) {
        ShowWindow(hWnd, SW_RESTORE);
    }

    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

    style &= ~(WS_CAPTION | WS_THICKFRAME);
    exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE |
                 WS_EX_STATICEDGE);

    SetWindowLongPtrW(hWnd, GWL_STYLE, style);
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle);

    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    int x, y, width, height;
    ComputeTargetRect(hWnd, x, y, width, height);

    Wh_Log(L"Applying borderless to HWND %p (%dx%d at %d,%d)", hWnd, width,
           height, x, y);

    SetWindowPos(hWnd, nullptr, x, y, width, height,
                 SWP_NOZORDER | SWP_FRAMECHANGED);
}

// ---------------------------------------------------------------------------
// Tracked windows (for restoring the original state later)
// ---------------------------------------------------------------------------

struct SavedWindowState {
    LONG_PTR style;
    LONG_PTR exStyle;
    RECT rect;
};

std::mutex g_trackedWindowsMutex;
std::unordered_map<HWND, SavedWindowState> g_trackedWindows;

void RestoreWindow(HWND hWnd, const SavedWindowState& saved) {
    if (!IsWindow(hWnd)) {
        return;
    }

    Wh_Log(L"Restoring original style/size for HWND %p", hWnd);

    SetWindowLongPtrW(hWnd, GWL_STYLE, saved.style);
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, saved.exStyle);
    SetWindowPos(hWnd, nullptr, saved.rect.left, saved.rect.top,
                 saved.rect.right - saved.rect.left,
                 saved.rect.bottom - saved.rect.top,
                 SWP_NOZORDER | SWP_FRAMECHANGED);
}

void TrackAndApplyIfNeeded(HWND hWnd) {
    if (!IsWindow(hWnd)) return;

    std::lock_guard<std::mutex> lock(g_trackedWindowsMutex);

    auto it = g_trackedWindows.find(hWnd);
    if (it == g_trackedWindows.end()) {
        SavedWindowState saved;
        saved.style = GetWindowLongPtrW(hWnd, GWL_STYLE);
        saved.exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        if (!GetWindowRect(hWnd, &saved.rect)) return;
        g_trackedWindows[hWnd] = saved;
        ApplyBorderless(hWnd);
        return;
    }

    int x, y, width, height;
    ComputeTargetRect(hWnd, x, y, width, height);
    if (!IsAlreadyBorderlessAndSized(hWnd, width, height, x, y)) {
        ApplyBorderless(hWnd);
    }
}

void UntrackWithoutRestoring(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_trackedWindowsMutex);
    g_trackedWindows.erase(hWnd);
}

// If a different tracked window belongs to the same exe as hWnd (e.g. a
// splash screen that has since been replaced by the real main window),
// restore and stop tracking it.
void RestoreOtherTrackedWindowsForSameExe(HWND hWnd,
                                           const std::wstring& exeName) {
    std::lock_guard<std::mutex> lock(g_trackedWindowsMutex);
    for (auto it = g_trackedWindows.begin(); it != g_trackedWindows.end();) {
        if (it->first != hWnd) {
            std::wstring trackedExe;
            if (GetExeNameForWindow(it->first, trackedExe) &&
                _wcsicmp(trackedExe.c_str(), exeName.c_str()) == 0) {
                RestoreWindow(it->first, it->second);
                it = g_trackedWindows.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void RestoreAllTrackedWindows() {
    std::lock_guard<std::mutex> lock(g_trackedWindowsMutex);
    for (auto& entry : g_trackedWindows) {
        RestoreWindow(entry.first, entry.second);
    }
    g_trackedWindows.clear();
}

// ---------------------------------------------------------------------------
// Full rescan (used at startup and whenever settings change)
// ---------------------------------------------------------------------------

void RescanAllWindows() {
    auto targets = GetTargetExeNames();
    if (targets.empty()) {
        RestoreAllTrackedWindows();
        return;
    }

    std::unordered_map<HWND, bool> currentMatches;
    std::vector<HWND> bestWindows;
    for (const auto& exeName : targets) {
        if (HWND best = GetBestWindowForExe(exeName)) {
            currentMatches[best] = true;
            bestWindows.push_back(best);
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_trackedWindowsMutex);
        for (auto it = g_trackedWindows.begin();
             it != g_trackedWindows.end();) {
            if (currentMatches.find(it->first) == currentMatches.end()) {
                RestoreWindow(it->first, it->second);
                it = g_trackedWindows.erase(it);
            } else {
                ++it;
            }
        }
    }

    for (HWND hWnd : bestWindows) {
        TrackAndApplyIfNeeded(hWnd);
    }
}

// ---------------------------------------------------------------------------
// Event-driven detection
// ---------------------------------------------------------------------------

std::atomic<bool> g_shuttingDown{false};
std::atomic<int> g_pendingSettleCount{0};

void EvaluateSettledCandidate(HWND hWnd) {
    if (!IsWindow(hWnd)) return;
    if (!IsEligibleWindow(hWnd)) return;

    std::wstring exeName;
    if (!GetExeNameForWindow(hWnd, exeName)) return;
    if (!IsTargetExe(exeName, GetTargetExeNames())) return;

    // Make sure this is still the best (largest) window for that exe -
    // it might have been superseded by another window in the meantime.
    if (GetBestWindowForExe(exeName) != hWnd) return;

    RestoreOtherTrackedWindowsForSameExe(hWnd, exeName);
    TrackAndApplyIfNeeded(hWnd);
}

DWORD WINAPI SettleAndApplyThread(LPVOID param) {
    HWND hWnd = (HWND)param;

    // Give the window a moment to settle before touching it - acting on
    // it immediately after creation can interfere with the app's own
    // startup routine.
    Sleep(1000);

    if (!g_shuttingDown.load()) {
        EvaluateSettledCandidate(hWnd);
    }

    g_pendingSettleCount--;
    return 0;
}

void OnCandidateWindowEvent(HWND hWnd) {
    if (g_shuttingDown.load() || !hWnd) return;

    bool alreadyTracked;
    {
        std::lock_guard<std::mutex> lock(g_trackedWindowsMutex);
        alreadyTracked = g_trackedWindows.find(hWnd) != g_trackedWindows.end();
    }

    if (alreadyTracked) {
        // Already handled before - just make sure it's still in the
        // desired state (the app may have reset its own window style).
        TrackAndApplyIfNeeded(hWnd);
        return;
    }

    if (!IsEligibleWindow(hWnd)) return;

    std::wstring exeName;
    if (!GetExeNameForWindow(hWnd, exeName)) return;
    if (!IsTargetExe(exeName, GetTargetExeNames())) return;

    g_pendingSettleCount++;
    HANDLE hThread = CreateThread(nullptr, 0, SettleAndApplyThread,
                                   (LPVOID)hWnd, 0, nullptr);
    if (hThread) {
        CloseHandle(hThread);
    } else {
        g_pendingSettleCount--;
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                            LONG idObject, LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) {
        return;
    }

    if (event == EVENT_OBJECT_DESTROY) {
        UntrackWithoutRestoring(hwnd);
        return;
    }

    OnCandidateWindowEvent(hwnd);
}

HWINEVENTHOOK g_hWinEventHookA;
HWINEVENTHOOK g_hWinEventHookB;
HANDLE g_eventThread;
DWORD g_eventThreadId;

DWORD WINAPI EventThreadProc(LPVOID) {
    // CREATE / DESTROY / SHOW
    g_hWinEventHookA =
        SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    // STATECHANGE / LOCATIONCHANGE
    g_hWinEventHookB =
        SetWinEventHook(EVENT_OBJECT_STATECHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    if (!g_hWinEventHookA || !g_hWinEventHookB) {
        Wh_Log(L"SetWinEventHook failed");
    }

    // Catch apps that were already running when the mod started.
    RescanAllWindows();

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hWinEventHookA) {
        UnhookWinEvent(g_hWinEventHookA);
        g_hWinEventHookA = nullptr;
    }
    if (g_hWinEventHookB) {
        UnhookWinEvent(g_hWinEventHookB);
        g_hWinEventHookB = nullptr;
    }

    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    LoadSettings();
    g_eventThread = CreateThread(nullptr, 0, EventThreadProc, nullptr, 0,
                                  &g_eventThreadId);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    RescanAllWindows();
}

void WhTool_ModUninit() {
    g_shuttingDown = true;

    // Give in-flight "let it settle" threads a chance to notice the
    // shutdown flag and bail out before the mod is unloaded.
    for (int i = 0; i < 40 && g_pendingSettleCount.load() > 0; i++) {
        Sleep(50);
    }

    if (g_eventThreadId) {
        PostThreadMessage(g_eventThreadId, WM_QUIT, 0, 0);
    }
    if (g_eventThread) {
        WaitForSingleObject(g_eventThread, INFINITE);
        CloseHandle(g_eventThread);
        g_eventThread = nullptr;
    }

    RestoreAllTrackedWindows();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
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

    if (isExcluded) {
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
