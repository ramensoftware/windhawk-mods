// ==WindhawkMod==
// @id              explorer-restore-window-size
// @name            Explorer Restore Window Size
// @description     Remember File Explorer's normal window size and position before maximizing, and restore it when leaving maximized mode.
// @version         0.1
// @author          FavoriteClannad
// @github          https://github.com/FavoriteClannad
// @include         explorer.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Restore Window Size

File Explorer can sometimes return from maximized mode to a normal window that
is still nearly full-screen. This mod remembers each Explorer window's normal
size and position before maximizing, then restores that window's saved
placement when it leaves maximized mode. Each Explorer window is tracked
independently.

Version 0.1 doesn't persist placements across `explorer.exe` restarts. The
original issue is intermittent and remains under long-term observation.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

struct ExplorerWindowState {
    WINDOWPLACEMENT lastNormalPlacement{};
    RECT lastNormalScreenRect{};
    bool hasValidNormalPlacement = false;
    bool hasValidNormalScreenRect = false;
    bool wasMaximized = false;
    bool maximizePending = false;
    bool applyingRestore = false;

    ExplorerWindowState() {
        lastNormalPlacement.length = sizeof(lastNormalPlacement);
    }
};

std::mutex g_windowStatesMutex;
std::unordered_map<HWND, std::unique_ptr<ExplorerWindowState>> g_windowStates;
std::atomic_bool g_uninitializing = false;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

LRESULT CALLBACK ExplorerWindowSubclassProc(HWND hWnd,
                                            UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            DWORD_PTR dwRefData);

bool IsValidRect(const RECT& rect) {
    return rect.right > rect.left && rect.bottom > rect.top;
}

bool IsExplorerWindow(HWND hWnd) {
    if (!hWnd || GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return false;
    }

    DWORD processId = 0;
    if (!GetWindowThreadProcessId(hWnd, &processId) ||
        processId != GetCurrentProcessId()) {
        return false;
    }

    WCHAR className[32];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"CabinetWClass") == 0 ||
           _wcsicmp(className, L"ExploreWClass") == 0;
}

bool IsNormalWindowState(HWND hWnd) {
    return !IsZoomed(hWnd) && !IsIconic(hWnd);
}

bool CaptureNormalPlacement(HWND hWnd,
                            ExplorerWindowState* state,
                            PCWSTR reason,
                            bool logCapture) {
    if (!state || state->applyingRestore || !IsNormalWindowState(hWnd)) {
        return false;
    }

    WINDOWPLACEMENT placement{};
    placement.length = sizeof(placement);
    if (!GetWindowPlacement(hWnd, &placement) ||
        placement.showCmd == SW_SHOWMAXIMIZED ||
        placement.showCmd == SW_SHOWMINIMIZED ||
        !IsValidRect(placement.rcNormalPosition)) {
        return false;
    }

    state->lastNormalPlacement = placement;
    state->hasValidNormalPlacement = true;

    RECT screenRect{};
    state->hasValidNormalScreenRect =
        GetWindowRect(hWnd, &screenRect) && IsValidRect(screenRect);
    if (state->hasValidNormalScreenRect) {
        state->lastNormalScreenRect = screenRect;
    }

    if (logCapture) {
        const RECT& rect = placement.rcNormalPosition;
        Wh_Log(L"Saved normal placement for %p (%s): (%ld,%ld)-(%ld,%ld)",
               hWnd, reason, rect.left, rect.top, rect.right, rect.bottom);
    }

    return true;
}

bool RectIntersectsMonitorWorkArea(const RECT& rect) {
    HMONITOR monitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONULL);
    if (!monitor) {
        return false;
    }

    MONITORINFO monitorInfo{sizeof(monitorInfo)};
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    RECT intersection{};
    return IntersectRect(&intersection, &rect, &monitorInfo.rcWork) &&
           IsValidRect(intersection);
}

bool AdjustPlacementIfCompletelyOffscreen(HWND hWnd,
                                          const ExplorerWindowState& state,
                                          RECT* placementRect,
                                          RECT* adjustedScreenRect) {
    if (!state.hasValidNormalScreenRect ||
        RectIntersectsMonitorWorkArea(state.lastNormalScreenRect)) {
        return false;
    }

    HMONITOR monitor = MonitorFromRect(&state.lastNormalScreenRect,
                                       MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    }

    MONITORINFO monitorInfo{sizeof(monitorInfo)};
    if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    const RECT& workArea = monitorInfo.rcWork;
    const LONG workWidth = workArea.right - workArea.left;
    const LONG workHeight = workArea.bottom - workArea.top;
    if (workWidth <= 0 || workHeight <= 0) {
        return false;
    }

    LONG width = state.lastNormalScreenRect.right -
                 state.lastNormalScreenRect.left;
    LONG height = state.lastNormalScreenRect.bottom -
                  state.lastNormalScreenRect.top;
    width = std::min(width, workWidth);
    height = std::min(height, workHeight);

    const LONG maxLeft = workArea.right - width;
    const LONG maxTop = workArea.bottom - height;
    const LONG screenLeft =
        std::min(std::max(state.lastNormalScreenRect.left, workArea.left),
                 maxLeft);
    const LONG screenTop =
        std::min(std::max(state.lastNormalScreenRect.top, workArea.top), maxTop);

    *adjustedScreenRect = {
        screenLeft,
        screenTop,
        screenLeft + width,
        screenTop + height,
    };

    // WINDOWPLACEMENT uses workspace coordinates. Convert the adjusted
    // screen-space rectangle by accounting for the monitor work-area origin.
    const LONG workspaceOffsetX =
        monitorInfo.rcWork.left - monitorInfo.rcMonitor.left;
    const LONG workspaceOffsetY =
        monitorInfo.rcWork.top - monitorInfo.rcMonitor.top;
    *placementRect = {
        adjustedScreenRect->left - workspaceOffsetX,
        adjustedScreenRect->top - workspaceOffsetY,
        adjustedScreenRect->right - workspaceOffsetX,
        adjustedScreenRect->bottom - workspaceOffsetY,
    };

    return true;
}

bool RectsMeaningfullyDifferent(const RECT& first, const RECT& second) {
    auto differs = [](LONG a, LONG b) {
        constexpr LONGLONG kTolerance = 8;
        const LONGLONG difference = static_cast<LONGLONG>(a) - b;
        return difference > kTolerance || difference < -kTolerance;
    };

    return differs(first.left, second.left) ||
           differs(first.top, second.top) ||
           differs(first.right, second.right) ||
           differs(first.bottom, second.bottom);
}

enum class ApplyPlacementResult {
    NotNeeded,
    Succeeded,
    Failed,
};

ApplyPlacementResult ApplySavedNormalPlacement(HWND hWnd,
                                                ExplorerWindowState* state,
                                                PCWSTR reason,
                                                bool onlyIfDifferent) {
    if (!state || !state->hasValidNormalPlacement ||
        state->applyingRestore) {
        return ApplyPlacementResult::NotNeeded;
    }

    WINDOWPLACEMENT currentPlacement{};
    currentPlacement.length = sizeof(currentPlacement);
    if (!GetWindowPlacement(hWnd, &currentPlacement)) {
        return ApplyPlacementResult::Failed;
    }

    RECT targetRect = state->lastNormalPlacement.rcNormalPosition;
    RECT adjustedScreenRect{};
    const bool movedOnscreen = AdjustPlacementIfCompletelyOffscreen(
        hWnd, *state, &targetRect, &adjustedScreenRect);

    if (onlyIfDifferent &&
        !RectsMeaningfullyDifferent(currentPlacement.rcNormalPosition,
                                     targetRect)) {
        return ApplyPlacementResult::NotNeeded;
    }

    // Preserve showCmd, flags, and the minimize/maximize positions. Only the
    // normal rectangle is replaced so Windows can run its native restore path.
    currentPlacement.rcNormalPosition = targetRect;

    state->applyingRestore = true;
    const BOOL succeeded = SetWindowPlacement(hWnd, &currentPlacement);
    state->applyingRestore = false;

    if (!succeeded) {
        Wh_Log(L"Failed to apply saved placement for %p (%s), error=%lu", hWnd,
               reason, GetLastError());
        return ApplyPlacementResult::Failed;
    }

    if (movedOnscreen) {
        state->lastNormalPlacement.rcNormalPosition = targetRect;
        state->lastNormalScreenRect = adjustedScreenRect;
    }

    Wh_Log(L"Applied saved placement for %p (%s): (%ld,%ld)-(%ld,%ld)%s",
           hWnd, reason, targetRect.left, targetRect.top, targetRect.right,
           targetRect.bottom, movedOnscreen ? L"; moved onscreen" : L"");
    return ApplyPlacementResult::Succeeded;
}

void HandleWindowPosChanged(HWND hWnd, ExplorerWindowState* state) {
    if (!state || state->applyingRestore) {
        return;
    }

    if (IsZoomed(hWnd)) {
        state->maximizePending = false;
        state->wasMaximized = true;
        return;
    }

    if (IsIconic(hWnd)) {
        return;
    }

    // SC_MAXIMIZE can be followed by position messages before IsZoomed starts
    // reporting true. Do not mistake that transition for a restore or capture
    // its near-maximized rectangle as a normal placement.
    if (state->maximizePending) {
        return;
    }

    if (state->wasMaximized) {
        const ApplyPlacementResult result = ApplySavedNormalPlacement(
            hWnd, state, L"post-restore fallback", true);
        state->wasMaximized = false;

        // Keep the pre-maximize snapshot if correction failed. Otherwise,
        // record the final placement that Windows actually applied.
        if (result != ApplyPlacementResult::Failed) {
            CaptureNormalPlacement(hWnd, state, L"restore complete", false);
        }
        return;
    }

    CaptureNormalPlacement(hWnd, state, L"normal window change", false);
}

void RemoveWindowState(HWND hWnd, ExplorerWindowState* expectedState) {
    std::lock_guard<std::mutex> lock(g_windowStatesMutex);
    auto it = g_windowStates.find(hWnd);
    if (it != g_windowStates.end() && it->second.get() == expectedState) {
        g_windowStates.erase(it);
    }
}

LRESULT CALLBACK ExplorerWindowSubclassProc(HWND hWnd,
                                            UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            DWORD_PTR dwRefData) {
    auto* state = reinterpret_cast<ExplorerWindowState*>(dwRefData);

    switch (uMsg) {
        case WM_SYSCOMMAND: {
            const UINT command = static_cast<UINT>(wParam & 0xFFF0);
            if (command == SC_MAXIMIZE) {
                CaptureNormalPlacement(hWnd, state, L"before maximize", true);
                state->wasMaximized = true;
                state->maximizePending = true;
                Wh_Log(L"Received maximize for %p", hWnd);
            } else if (command == SC_RESTORE) {
                const bool restoringFromMaximized =
                    IsZoomed(hWnd) && !IsIconic(hWnd);
                Wh_Log(L"Received restore for %p (fromMaximized=%d)", hWnd,
                       restoringFromMaximized);
                if (restoringFromMaximized) {
                    ApplySavedNormalPlacement(hWnd, state,
                                              L"before native restore", false);
                }
            }

            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        case WM_SIZE:
            if (wParam == SIZE_MAXIMIZED) {
                state->maximizePending = false;
                state->wasMaximized = true;
            }
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        case WM_WINDOWPOSCHANGED: {
            const LRESULT result =
                DefSubclassProc(hWnd, uMsg, wParam, lParam);
            HandleWindowPosChanged(hWnd, state);
            return result;
        }

        case WM_EXITSIZEMOVE: {
            const LRESULT result =
                DefSubclassProc(hWnd, uMsg, wParam, lParam);
            CaptureNormalPlacement(hWnd, state, L"manual move/resize", true);
            return result;
        }

        case WM_NCDESTROY:
            Wh_Log(L"Cleaning up Explorer window %p", hWnd);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, ExplorerWindowSubclassProc);
            RemoveWindowState(hWnd, state);
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        default:
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
}

void InstallExplorerWindowSubclass(HWND hWnd) {
    if (g_uninitializing || !IsExplorerWindow(hWnd)) {
        return;
    }

    auto state = std::make_unique<ExplorerWindowState>();
    state->wasMaximized = IsZoomed(hWnd) != FALSE;
    ExplorerWindowState* statePtr = state.get();

    {
        std::lock_guard<std::mutex> lock(g_windowStatesMutex);
        if (g_windowStates.find(hWnd) != g_windowStates.end()) {
            return;
        }
        g_windowStates.emplace(hWnd, std::move(state));
    }

    Wh_Log(L"Found Explorer window %p", hWnd);
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, ExplorerWindowSubclassProc,
            reinterpret_cast<DWORD_PTR>(statePtr))) {
        Wh_Log(L"Failed to subclass Explorer window %p, error=%lu", hWnd,
               GetLastError());
        RemoveWindowState(hWnd, statePtr);
        return;
    }

    Wh_Log(L"Subclass installed for Explorer window %p", hWnd);
    CaptureNormalPlacement(hWnd, statePtr, L"initial state", true);
}

BOOL CALLBACK EnumExplorerWindows(HWND hWnd, LPARAM) {
    InstallExplorerWindowSubclass(hWnd);
    return TRUE;
}

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);
    if (hWnd) {
        InstallExplorerWindowSubclass(hWnd);
    }
    return hWnd;
}

BOOL Wh_ModInit() {
    g_uninitializing = false;
    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(CreateWindowExW),
            reinterpret_cast<void*>(CreateWindowExW_Hook),
            reinterpret_cast<void**>(&CreateWindowExW_Original))) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumExplorerWindows, 0);
}

void Wh_ModUninit() {
    g_uninitializing = true;

    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> lock(g_windowStatesMutex);
        windows.reserve(g_windowStates.size());
        for (const auto& [hWnd, state] : g_windowStates) {
            windows.push_back(hWnd);
        }
    }

    for (HWND hWnd : windows) {
        if (IsWindow(hWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, ExplorerWindowSubclassProc);
        }
    }

    std::lock_guard<std::mutex> lock(g_windowStatesMutex);
    g_windowStates.clear();
}
