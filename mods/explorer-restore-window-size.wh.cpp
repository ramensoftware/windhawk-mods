// ==WindhawkMod==
// @id              explorer-restore-window-size
// @name            Explorer Restore Window Size
// @description     Remember File Explorer's normal window size and position before maximizing, and restore it when leaving maximized mode.
// @version         1.0.0
// @author          FavoriteClannad
// @github          https://github.com/FavoriteClannad
// @include         explorer.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Restore Window Size

File Explorer can sometimes return from maximized mode to a normal window that
is still nearly full-screen. This mod remembers the most recent normal size and
position of each Explorer window. The Restore button restores the saved size
and position. Dragging a maximized window by its title bar restores the saved
size while keeping the new position chosen by Windows from the mouse movement.
Each Explorer window is tracked independently.

A reproducible case is to resize Explorer to a small normal window, maximize
it, shut down Windows while it remains maximized, restart Windows, open
Explorer maximized, and then restore it. Without the persistent fallback, the
restored window can be nearly full-screen instead of returning to the small
pre-maximize size and position.

The confirmed failure is that per-window state is lost when `explorer.exe` or
Windows restarts, after which the normal placement available to Explorer can
already be nearly full-screen. The internal Windows or Explorer path that
saves or recalculates the incorrect `rcNormalPosition` can't be verified from
public source code.

Within one process lifetime, the mod stores a separate normal placement for
each Explorer window and never replaces it while the window is maximized or
minimized. Windhawk LocalStorage also persists the last confirmed valid normal
placement as a fallback for an Explorer window that starts maximized after an
Explorer or Windows restart.

The Remember Folder Positions mod saves and restores a `WINDOWPLACEMENT` per
folder through that folder's Shell Bag. Explorer Restore Window Size doesn't
restore placement by folder; it specifically repairs the normal placement for
maximize-to-restore transitions and provides a fallback across Explorer or
Windows restarts. If both mods are enabled, both can affect Explorer placement,
so their behavior may interact. Cross-restart matching of multiple old
Explorer windows isn't supported; only the most recently confirmed normal
placement is persisted.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <algorithm>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct ExplorerWindowState {
    WINDOWPLACEMENT lastNormalPlacement{};
    RECT lastNormalScreenRect{};
    bool hasValidNormalPlacement = false;
    bool hasValidNormalScreenRect = false;
    bool wasMaximized = false;
    bool processingMaximizeCommand = false;
    bool applyingRestore = false;
    bool inSizeMove = false;

    ExplorerWindowState() {
        lastNormalPlacement.length = sizeof(lastNormalPlacement);
    }
};

constexpr DWORD kPersistedPlacementVersion = 1;
constexpr WCHAR kPersistedPlacementValueName[] = L"LastNormalPlacement";

struct PersistedPlacementData {
    DWORD version;
    DWORD dataSize;
    RECT normalPosition;
    RECT screenRect;
};

std::mutex g_windowStatesMutex;
std::unordered_map<HWND, std::unique_ptr<ExplorerWindowState>> g_windowStates;
std::unordered_set<HWND> g_windowsBeingInitialized;

std::mutex g_persistedPlacementMutex;
PersistedPlacementData g_persistedPlacement{};
bool g_hasPersistedPlacement = false;
bool g_persistedFallbackClaimed = false;
HWND g_persistedFallbackReservation = nullptr;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

LRESULT CALLBACK ExplorerWindowSubclassProc(HWND hWnd,
                                            UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            DWORD_PTR dwRefData);

bool RectsMeaningfullyDifferent(const RECT& first, const RECT& second);

bool IsValidRect(const RECT& rect) {
    const LONGLONG width = static_cast<LONGLONG>(rect.right) - rect.left;
    const LONGLONG height = static_cast<LONGLONG>(rect.bottom) - rect.top;
    constexpr LONGLONG kMaxLong = 0x7FFFFFFFLL;
    return width > 0 && width <= kMaxLong && height > 0 &&
           height <= kMaxLong;
}

bool IsPersistedPlacementValid(const PersistedPlacementData& data) {
    return data.version == kPersistedPlacementVersion &&
           data.dataSize ==
               static_cast<DWORD>(sizeof(PersistedPlacementData)) &&
           IsValidRect(data.normalPosition) && IsValidRect(data.screenRect);
}

void LoadPersistedPlacement() {
    PersistedPlacementData data{};
    const size_t bytesRead = Wh_GetBinaryValue(
        kPersistedPlacementValueName, &data, sizeof(data));

    if (bytesRead != sizeof(data) || !IsPersistedPlacementValid(data)) {
        Wh_Log(L"Persisted placement invalid or unavailable; ignored "
               L"(bytes=%zu)",
               bytesRead);
        return;
    }

    std::lock_guard<std::mutex> lock(g_persistedPlacementMutex);
    g_persistedPlacement = data;
    g_hasPersistedPlacement = true;
    Wh_Log(L"Persisted normal placement loaded: (%ld,%ld)-(%ld,%ld)",
           data.normalPosition.left, data.normalPosition.top,
           data.normalPosition.right, data.normalPosition.bottom);
}

bool PersistNormalPlacementIfChanged(const ExplorerWindowState& state,
                                     PCWSTR reason) {
    if (!state.hasValidNormalPlacement ||
        !state.hasValidNormalScreenRect ||
        !IsValidRect(state.lastNormalPlacement.rcNormalPosition) ||
        !IsValidRect(state.lastNormalScreenRect)) {
        return false;
    }

    PersistedPlacementData data{
        .version = kPersistedPlacementVersion,
        .dataSize = static_cast<DWORD>(sizeof(PersistedPlacementData)),
        .normalPosition = state.lastNormalPlacement.rcNormalPosition,
        .screenRect = state.lastNormalScreenRect,
    };

    std::lock_guard<std::mutex> lock(g_persistedPlacementMutex);
    if (g_hasPersistedPlacement &&
        EqualRect(&g_persistedPlacement.normalPosition,
                  &data.normalPosition) &&
        EqualRect(&g_persistedPlacement.screenRect, &data.screenRect)) {
        return true;
    }

    if (!Wh_SetBinaryValue(kPersistedPlacementValueName, &data,
                           sizeof(data))) {
        Wh_Log(L"Failed to save persisted normal placement (%s)", reason);
        return false;
    }

    g_persistedPlacement = data;
    g_hasPersistedPlacement = true;
    Wh_Log(L"Persisted normal placement saved (%s): "
           L"(%ld,%ld)-(%ld,%ld)",
           reason, data.normalPosition.left, data.normalPosition.top,
           data.normalPosition.right, data.normalPosition.bottom);
    return true;
}

bool TryInitializeFromPersistedPlacement(
    HWND hWnd,
    ExplorerWindowState* state,
    bool reserveUntilSubclassInstalled = false) {
    if (!state || state->hasValidNormalPlacement) {
        return false;
    }

    PersistedPlacementData data{};
    {
        std::lock_guard<std::mutex> lock(g_persistedPlacementMutex);
        if (!g_hasPersistedPlacement || g_persistedFallbackClaimed ||
            g_persistedFallbackReservation) {
            return false;
        }

        data = g_persistedPlacement;
        if (reserveUntilSubclassInstalled) {
            g_persistedFallbackReservation = hWnd;
        } else {
            g_persistedFallbackClaimed = true;
        }
    }

    state->lastNormalPlacement = {};
    state->lastNormalPlacement.length = sizeof(state->lastNormalPlacement);
    state->lastNormalPlacement.rcNormalPosition = data.normalPosition;
    state->lastNormalScreenRect = data.screenRect;
    state->hasValidNormalPlacement = true;
    state->hasValidNormalScreenRect = true;

    if (reserveUntilSubclassInstalled) {
        Wh_Log(L"Persisted fallback prepared for maximized startup window "
               L"%p: (%ld,%ld)-(%ld,%ld)",
               hWnd, data.normalPosition.left, data.normalPosition.top,
               data.normalPosition.right, data.normalPosition.bottom);
    } else {
        Wh_Log(L"Persisted fallback applied to maximized startup window %p: "
               L"(%ld,%ld)-(%ld,%ld)",
               hWnd, data.normalPosition.left, data.normalPosition.top,
               data.normalPosition.right, data.normalPosition.bottom);
    }
    return true;
}

void CommitPersistedFallbackReservation(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_persistedPlacementMutex);
    if (g_persistedFallbackReservation != hWnd) {
        return;
    }

    g_persistedFallbackReservation = nullptr;
    g_persistedFallbackClaimed = true;
    Wh_Log(L"Persisted fallback claimed by maximized startup window %p", hWnd);
}

void ReleasePersistedFallbackReservation(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_persistedPlacementMutex);
    if (g_persistedFallbackReservation == hWnd) {
        g_persistedFallbackReservation = nullptr;
    }
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
    return IsWindowVisible(hWnd) && !IsZoomed(hWnd) && !IsIconic(hWnd);
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

    const bool normalPositionChanged =
        !state->hasValidNormalPlacement ||
        RectsMeaningfullyDifferent(
            state->lastNormalPlacement.rcNormalPosition,
            placement.rcNormalPosition);

    state->lastNormalPlacement = placement;
    state->hasValidNormalPlacement = true;

    RECT screenRect{};
    const bool hasValidScreenRect =
        GetWindowRect(hWnd, &screenRect) && IsValidRect(screenRect);
    const bool screenRectChanged =
        hasValidScreenRect &&
        (!state->hasValidNormalScreenRect ||
         RectsMeaningfullyDifferent(state->lastNormalScreenRect, screenRect));
    state->hasValidNormalScreenRect = hasValidScreenRect;
    if (hasValidScreenRect) {
        state->lastNormalScreenRect = screenRect;
    }

    // Interactive move/resize can generate many position messages. Keep the
    // per-window snapshot current, but defer LocalStorage until EXITSIZEMOVE.
    if ((!state->inSizeMove || logCapture) &&
        (normalPositionChanged || screenRectChanged || logCapture)) {
        PersistNormalPlacementIfChanged(*state, reason);
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
                                                PCWSTR reason) {
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

ApplyPlacementResult ApplySavedNormalSizeAtCurrentPosition(
    HWND hWnd,
    ExplorerWindowState* state,
    PCWSTR reason) {
    if (!state || !state->hasValidNormalPlacement ||
        state->applyingRestore) {
        return ApplyPlacementResult::NotNeeded;
    }

    WINDOWPLACEMENT currentPlacement{};
    currentPlacement.length = sizeof(currentPlacement);
    if (!GetWindowPlacement(hWnd, &currentPlacement) ||
        !IsValidRect(currentPlacement.rcNormalPosition)) {
        return ApplyPlacementResult::Failed;
    }

    const RECT& savedRect = state->lastNormalPlacement.rcNormalPosition;
    const LONG savedWidth = savedRect.right - savedRect.left;
    const LONG savedHeight = savedRect.bottom - savedRect.top;
    const RECT& currentRect = currentPlacement.rcNormalPosition;
    const LONG currentWidth = currentRect.right - currentRect.left;
    const LONG currentHeight = currentRect.bottom - currentRect.top;

    if (currentWidth == savedWidth && currentHeight == savedHeight) {
        return ApplyPlacementResult::NotNeeded;
    }

    currentPlacement.rcNormalPosition.right = currentRect.left + savedWidth;
    currentPlacement.rcNormalPosition.bottom = currentRect.top + savedHeight;

    state->applyingRestore = true;
    const BOOL succeeded = SetWindowPlacement(hWnd, &currentPlacement);
    state->applyingRestore = false;

    if (!succeeded) {
        Wh_Log(L"Failed to apply saved size for %p (%s), error=%lu", hWnd,
               reason, GetLastError());
        return ApplyPlacementResult::Failed;
    }

    const RECT& targetRect = currentPlacement.rcNormalPosition;
    Wh_Log(L"Applied saved size at native drag position for %p (%s): "
           L"(%ld,%ld)-(%ld,%ld)",
           hWnd, reason, targetRect.left, targetRect.top, targetRect.right,
           targetRect.bottom);
    return ApplyPlacementResult::Succeeded;
}

void ConfirmMaximizedState(HWND hWnd, ExplorerWindowState* state) {
    state->wasMaximized = true;
    if (!state->hasValidNormalPlacement) {
        TryInitializeFromPersistedPlacement(hWnd, state);
    }
}

bool CompleteStrictRestore(HWND hWnd,
                           ExplorerWindowState* state,
                           PCWSTR reason) {
    if (!state || !state->wasMaximized || state->applyingRestore ||
        state->inSizeMove || IsZoomed(hWnd) || IsIconic(hWnd)) {
        return false;
    }

    const ApplyPlacementResult result =
        ApplySavedNormalPlacement(hWnd, state, reason);
    if (result == ApplyPlacementResult::Failed) {
        return false;
    }

    state->wasMaximized = false;
    CaptureNormalPlacement(hWnd, state, L"strict restore complete", true);
    return true;
}

bool CompleteDragRestore(HWND hWnd,
                         ExplorerWindowState* state,
                         PCWSTR reason) {
    if (!state || !state->wasMaximized || state->applyingRestore ||
        state->inSizeMove || IsZoomed(hWnd) || IsIconic(hWnd)) {
        return false;
    }

    const ApplyPlacementResult result =
        ApplySavedNormalSizeAtCurrentPosition(hWnd, state, reason);
    if (result == ApplyPlacementResult::Failed) {
        return false;
    }

    state->wasMaximized = false;
    CaptureNormalPlacement(hWnd, state, L"drag restore complete", true);
    return true;
}

void HandleWindowPosChanged(HWND hWnd, ExplorerWindowState* state) {
    if (!state || state->applyingRestore) {
        return;
    }

    if (IsZoomed(hWnd)) {
        ConfirmMaximizedState(hWnd, state);
        return;
    }

    if (IsIconic(hWnd)) {
        return;
    }

    // Position messages can be sent reentrantly while SC_MAXIMIZE is still
    // being processed or while the user is in the modal move/size loop. Do not
    // capture a transitional rectangle or run the post-restore correction
    // while the mouse button is still held.
    if (state->processingMaximizeCommand || state->inSizeMove) {
        return;
    }

    if (state->wasMaximized) {
        CompleteStrictRestore(hWnd, state, L"post-restore correction");
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
                const bool attemptingMaximize = !IsZoomed(hWnd);
                if (attemptingMaximize) {
                    CaptureNormalPlacement(hWnd, state, L"before maximize",
                                           true);
                    state->processingMaximizeCommand = true;
                    Wh_Log(L"Received maximize for %p", hWnd);
                }

                const LRESULT result =
                    DefSubclassProc(hWnd, uMsg, wParam, lParam);

                if (attemptingMaximize) {
                    state->processingMaximizeCommand = false;
                }

                if (IsZoomed(hWnd)) {
                    ConfirmMaximizedState(hWnd, state);
                } else if (attemptingMaximize) {
                    Wh_Log(L"Maximize didn't complete for %p", hWnd);
                }

                return result;
            }

            if (command == SC_RESTORE) {
                const bool restoringFromMaximized =
                    IsZoomed(hWnd) && !IsIconic(hWnd);
                Wh_Log(L"Received restore for %p (fromMaximized=%d)", hWnd,
                       restoringFromMaximized);
                if (restoringFromMaximized) {
                    ApplySavedNormalPlacement(hWnd, state,
                                              L"before native restore");
                }
            }

            if (command == SC_MOVE && IsZoomed(hWnd) && !IsIconic(hWnd)) {
                ConfirmMaximizedState(hWnd, state);
                ApplySavedNormalPlacement(
                    hWnd, state, L"before title-bar drag restore");
            }

            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        case WM_SIZE: {
            const LRESULT result =
                DefSubclassProc(hWnd, uMsg, wParam, lParam);
            if (wParam == SIZE_MAXIMIZED) {
                ConfirmMaximizedState(hWnd, state);
            } else if (wParam == SIZE_RESTORED) {
                CompleteStrictRestore(hWnd, state,
                                      L"SIZE_RESTORED correction");
            }
            return result;
        }

        case WM_WINDOWPOSCHANGED: {
            const LRESULT result =
                DefSubclassProc(hWnd, uMsg, wParam, lParam);
            HandleWindowPosChanged(hWnd, state);
            return result;
        }

        case WM_ENTERSIZEMOVE:
            state->inSizeMove = true;
            if (IsZoomed(hWnd) && !IsIconic(hWnd)) {
                ConfirmMaximizedState(hWnd, state);
                ApplySavedNormalPlacement(
                    hWnd, state, L"before maximized move loop");
            }
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        case WM_EXITSIZEMOVE: {
            const LRESULT result =
                DefSubclassProc(hWnd, uMsg, wParam, lParam);
            state->inSizeMove = false;
            if (IsZoomed(hWnd)) {
                ConfirmMaximizedState(hWnd, state);
            } else if (!IsIconic(hWnd)) {
                if (state->wasMaximized) {
                    CompleteDragRestore(hWnd, state,
                                        L"drag restore size correction");
                } else {
                    CaptureNormalPlacement(hWnd, state, L"manual move/resize",
                                           true);
                }
            }
            return result;
        }

        case WM_NCDESTROY:
            Wh_Log(L"Cleaning up Explorer window %p", hWnd);
            PersistNormalPlacementIfChanged(*state, L"window destroy");
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, ExplorerWindowSubclassProc);
            RemoveWindowState(hWnd, state);
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        default:
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
}

void InstallExplorerWindowSubclass(HWND hWnd) {
    if (!IsExplorerWindow(hWnd)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_windowStatesMutex);
        if (g_windowStates.find(hWnd) != g_windowStates.end() ||
            !g_windowsBeingInitialized.insert(hWnd).second) {
            return;
        }
    }

    Wh_Log(L"Found Explorer window %p", hWnd);

    auto state = std::make_unique<ExplorerWindowState>();
    const bool initiallyMaximized = IsZoomed(hWnd) != FALSE;
    state->wasMaximized = initiallyMaximized;
    const bool hasPersistedFallbackReservation =
        initiallyMaximized && TryInitializeFromPersistedPlacement(
                                  hWnd, state.get(), true);
    if (!initiallyMaximized) {
        CaptureNormalPlacement(hWnd, state.get(), L"initial state", true);
    }

    if (!IsExplorerWindow(hWnd)) {
        if (hasPersistedFallbackReservation) {
            ReleasePersistedFallbackReservation(hWnd);
        }
        std::lock_guard<std::mutex> lock(g_windowStatesMutex);
        g_windowsBeingInitialized.erase(hWnd);
        return;
    }

    ExplorerWindowState* statePtr = state.get();
    bool duplicateState = false;

    {
        std::lock_guard<std::mutex> lock(g_windowStatesMutex);
        g_windowsBeingInitialized.erase(hWnd);
        if (g_windowStates.find(hWnd) != g_windowStates.end()) {
            duplicateState = true;
        } else {
            g_windowStates.emplace(hWnd, std::move(state));
        }
    }

    if (duplicateState) {
        if (hasPersistedFallbackReservation) {
            ReleasePersistedFallbackReservation(hWnd);
        }
        return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, ExplorerWindowSubclassProc,
            reinterpret_cast<DWORD_PTR>(statePtr))) {
        Wh_Log(L"Failed to subclass Explorer window %p, error=%lu", hWnd,
               GetLastError());
        RemoveWindowState(hWnd, statePtr);
        if (hasPersistedFallbackReservation) {
            ReleasePersistedFallbackReservation(hWnd);
        }
        return;
    }

    if (hasPersistedFallbackReservation) {
        CommitPersistedFallbackReservation(hWnd);
    }

    Wh_Log(L"Subclass installed for Explorer window %p", hWnd);
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
    {
        std::lock_guard<std::mutex> lock(g_persistedPlacementMutex);
        g_persistedPlacement = {};
        g_hasPersistedPlacement = false;
        g_persistedFallbackClaimed = false;
        g_persistedFallbackReservation = nullptr;
    }
    LoadPersistedPlacement();

    if (!WindhawkUtils::SetFunctionHook(
            CreateWindowExW, CreateWindowExW_Hook,
            &CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumExplorerWindows, 0);
}

void Wh_ModUninit() {
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
    g_windowsBeingInitialized.clear();
}
