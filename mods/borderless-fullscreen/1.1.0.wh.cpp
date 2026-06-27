// ==WindhawkMod==
// @id              borderless-fullscreen
// @name            Borderless Fullscreen
// @description     Removes the window border of selected apps/games and resizes them to the primary monitor's resolution (or a fixed size)
// @version         1.1.0
// @author          TomberWolf
// @github          https://github.com/TomberWolf
// @include         windhawk.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Borderless Fullscreen

Add an executable name (e.g. `game.exe`) to the target list and the app
will automatically run without a title bar / border, stretched to fill
the primary monitor's current resolution (multi monitor setup is planned
for the next version).

Optionally, set a fixed window size instead (the window will then be
centered on the primary monitor) - useful for older games with a low
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
    $description: 0 = automatically use the primary monitor's current width
  - customHeight: 0
    $name: Height
    $description: 0 = automatically use the primary monitor's current height
  $name: Window Size
  $description: >-
    Leave both at 0 to automatically stretch to the primary monitor's
    current resolution. Set fixed values only if you want a smaller,
    centered window instead.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

std::mutex g_settingsMutex;
std::vector<std::wstring> g_targetExeNames;
std::atomic<int> g_customWidth{0};
std::atomic<int> g_customHeight{0};

std::atomic<bool> g_stopWorker{false};
HANDLE g_workerThread = nullptr;

struct SavedWindowState {
    LONG_PTR style;
    LONG_PTR exStyle;
    RECT rect;
};

std::unordered_map<HWND, SavedWindowState> g_trackedWindows;

std::vector<std::wstring> GetTargetExeNames() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_targetExeNames;
}

void GetTargetSize(int& width, int& height) {
    width = g_customWidth.load();
    height = g_customHeight.load();
}

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

struct EnumContext {
    std::vector<std::wstring> targets;
    // pid -> best (largest) matching window found so far
    std::unordered_map<DWORD, HWND> bestByPid;
};

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<EnumContext*>(lParam);

    if (!IsEligibleWindow(hWnd)) {
        return TRUE;
    }

    std::wstring exeName;
    if (!GetExeNameForWindow(hWnd, exeName)) {
        return TRUE;
    }

    if (!IsTargetExe(exeName, ctx->targets)) {
        return TRUE;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);

    auto it = ctx->bestByPid.find(pid);
    if (it == ctx->bestByPid.end() ||
        WindowArea(hWnd) > WindowArea(it->second)) {
        ctx->bestByPid[pid] = hWnd;
    }

    return TRUE;
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

void ComputeTargetRect(int& x, int& y, int& width, int& height) {
    int customWidth, customHeight;
    GetTargetSize(customWidth, customHeight);

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    if (customWidth <= 0 || customHeight <= 0) {
        width = screenW;
        height = screenH;
        x = 0;
        y = 0;
    } else {
        width = customWidth;
        height = customHeight;
        x = (screenW - width) / 2;
        y = (screenH - height) / 2;
    }
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
    ComputeTargetRect(x, y, width, height);

    Wh_Log(L"Applying borderless to HWND %p (%dx%d at %d,%d)", hWnd, width,
           height, x, y);

    SetWindowPos(hWnd, nullptr, x, y, width, height,
                 SWP_NOZORDER | SWP_FRAMECHANGED);
}

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

void RestoreAllTrackedWindows() {
    for (const auto& entry : g_trackedWindows) {
        RestoreWindow(entry.first, entry.second);
    }
    g_trackedWindows.clear();
}

void PollAndApply() {
    EnumContext ctx;
    ctx.targets = GetTargetExeNames();
    if (ctx.targets.empty()) {
        // No targets configured - make sure nothing stays modified.
        RestoreAllTrackedWindows();
        return;
    }

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&ctx));

    // Build the current set of windows that should be borderless.
    std::unordered_map<HWND, bool> currentMatches;
    for (const auto& entry : ctx.bestByPid) {
        currentMatches[entry.second] = true;
    }

    // Restore windows that no longer match (closed, exe removed from the
    // list, or no longer the "best" window for their process).
    for (auto it = g_trackedWindows.begin(); it != g_trackedWindows.end();) {
        if (currentMatches.find(it->first) == currentMatches.end()) {
            RestoreWindow(it->first, it->second);
            it = g_trackedWindows.erase(it);
        } else {
            ++it;
        }
    }

    int x, y, width, height;
    ComputeTargetRect(x, y, width, height);

    // Apply borderless to all currently matching windows.
    for (const auto& entry : ctx.bestByPid) {
        HWND hWnd = entry.second;
        auto it = g_trackedWindows.find(hWnd);
        if (it == g_trackedWindows.end()) {
            // First time we see this window - remember its original state.
            SavedWindowState saved;
            saved.style = GetWindowLongPtrW(hWnd, GWL_STYLE);
            saved.exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            if (!GetWindowRect(hWnd, &saved.rect)) {
                continue;
            }
            g_trackedWindows[hWnd] = saved;

            ApplyBorderless(hWnd);
        } else if (!IsAlreadyBorderlessAndSized(hWnd, width, height, x, y)) {
            // Already tracked, but something changed the window back
            // (e.g. an in-game windowed-mode toggle) - reapply.
            ApplyBorderless(hWnd);
        }
        // else: already in the desired state, nothing to do.
    }
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    Sleep(1000);

    while (!g_stopWorker.load()) {
        PollAndApply();

        for (int i = 0; i < 50 && !g_stopWorker.load(); i++) {
            Sleep(100);
        }
    }

    return 0;
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

}  // namespace

BOOL WhTool_ModInit() {
    LoadSettings();

    g_stopWorker = false;
    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    g_stopWorker = true;
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
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
