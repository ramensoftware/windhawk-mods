// ==WindhawkMod==
// @id startallback-jumplist-width
// @name StartAllBack Jump List Width
// @description Changes the width of StartAllBack jump lists and keeps their internal controls aligned.
// @version 1.2
// @author Murtuzoff
// @github https://github.com/Murtuzoff
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lcomctl32
// @license MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# StartAllBack Jump List Width

Changes the width of StartAllBack jump lists and resizes their internal controls to match.

The mod targets StartAllBack's `SIBJumpView` hierarchy in `explorer.exe`.
It subclasses the relevant jump-list window and its `SIBBarHost` /
`SysListView32` children, so the configured width and the internal right
margin remain stable even if StartAllBack performs another layout pass.
When the width changes, the jump list stays centered around the position
chosen by StartAllBack and is clamped to the monitor work area.

## Settings

- **Jump List width** - target width at 100% scaling (96 DPI). The value is
  limited to 120-600 pixels and is scaled automatically for the DPI of the
  monitor where the jump list is shown.
- The internal list uses a fixed 6-pixel right margin at 100% scaling, also
  scaled automatically for DPI.

Changes are applied immediately to existing StartAllBack jump-list windows.
When the mod is disabled, the original StartAllBack width and internal layout
are restored.

## Before and after

### Before

![StartAllBack Jump List before](https://raw.githubusercontent.com/Murtuzoff/windhawk-assets/main/before.png)

### After

![StartAllBack Jump List after](https://raw.githubusercontent.com/Murtuzoff/windhawk-assets/main/after.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- width: 225
  $name: Jump List width
  $description: Target width at 100% scaling (96 DPI). The value is scaled automatically for the current monitor DPI.
  $name:ru-RU: Ширина Jump List
  $description:ru-RU: Ширина при масштабе 100% (96 DPI). Значение автоматически масштабируется с учётом DPI текущего монитора.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>

#include <algorithm>
#include <atomic>
#include <cwchar>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

constexpr int kDefaultWidth96 = 225;
constexpr int kMinWidth96 = 120;
constexpr int kMaxWidth96 = 600;
constexpr int kEdgeMargin96 = 6;

// StartAllBack's stock list margin, used only while restoring the original
// layout on unload.
constexpr int kStockEdgeMargin96 = 4;

std::atomic<int> g_width96{kDefaultWidth96};
std::atomic_bool g_unloading{false};

struct JumpViewState {
    std::atomic<int> originalWidth{0};
    std::atomic<int> originalX{0};
    std::atomic_bool applyingModResize{false};
};

std::mutex g_statesMutex;
std::unordered_map<HWND, std::shared_ptr<JumpViewState>> g_states;
std::unordered_set<HWND> g_hostWindows;
std::unordered_set<HWND> g_listWindows;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

LRESULT CALLBACK JumpViewSubclassProc(HWND hWnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      DWORD_PTR);
LRESULT CALLBACK BarHostSubclassProc(HWND hWnd,
                                     UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR);
LRESULT CALLBACK ListViewSubclassProc(HWND hWnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      DWORD_PTR);

bool AttachBarHost(HWND host);
bool AttachListView(HWND list);
void AttachChildSubclasses(HWND jumpView);

void LoadSettings() {
    int width = Wh_GetIntSetting(L"width");
    width = std::clamp(width, kMinWidth96, kMaxWidth96);
    g_width96.store(width, std::memory_order_relaxed);
}

bool IsClass(HWND hWnd, const wchar_t* wanted) {
    if (!hWnd) {
        return false;
    }

    wchar_t className[128] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return wcscmp(className, wanted) == 0;
}

bool IsSIBJumpView(HWND hWnd) {
    return IsClass(hWnd, L"SIBJumpView");
}

bool IsSIBBarHost(HWND hWnd) {
    if (!IsClass(hWnd, L"SIBBarHost")) {
        return false;
    }

    return IsSIBJumpView(GetParent(hWnd));
}

bool IsSIBListView(HWND hWnd) {
    if (!IsClass(hWnd, L"SysListView32")) {
        return false;
    }

    HWND host = GetParent(hWnd);
    return IsSIBBarHost(host);
}

HWND GetOwningJumpView(HWND hWnd) {
    if (IsSIBJumpView(hWnd)) {
        return hWnd;
    }

    if (IsSIBBarHost(hWnd)) {
        return GetParent(hWnd);
    }

    if (IsSIBListView(hWnd)) {
        HWND host = GetParent(hWnd);
        return GetParent(host);
    }

    return nullptr;
}

UINT GetWindowDpiSafe(HWND hWnd) {
    UINT dpi = GetDpiForWindow(hWnd);
    return dpi ? dpi : 96;
}

int ScaleForWindow(HWND hWnd, int value96) {
    return MulDiv(value96, static_cast<int>(GetWindowDpiSafe(hWnd)), 96);
}

int GetTargetWidthForWindow(HWND hWnd) {
    return ScaleForWindow(hWnd, g_width96.load(std::memory_order_relaxed));
}

int GetEdgeMarginForWindow(HWND hWnd) {
    return ScaleForWindow(hWnd, kEdgeMargin96);
}

int GetStockEdgeMarginForWindow(HWND hWnd) {
    return ScaleForWindow(hWnd, kStockEdgeMargin96);
}

bool GetRelativeRect(HWND hWnd, HWND parent, RECT* rect) {
    if (!rect || !GetWindowRect(hWnd, rect)) {
        return false;
    }

    MapWindowPoints(HWND_DESKTOP, parent, reinterpret_cast<LPPOINT>(rect), 2);
    return true;
}

int GetRelativeX(HWND hWnd, HWND parent, const WINDOWPOS* windowPos) {
    if (windowPos && !(windowPos->flags & SWP_NOMOVE)) {
        return windowPos->x;
    }

    RECT rect = {};
    if (GetRelativeRect(hWnd, parent, &rect)) {
        return rect.left;
    }

    return windowPos ? windowPos->x : 0;
}

std::shared_ptr<JumpViewState> GetState(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_statesMutex);
    auto it = g_states.find(hWnd);
    return it != g_states.end() ? it->second : nullptr;
}

void RemoveState(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_statesMutex);
    g_states.erase(hWnd);
}

void UntrackBarHost(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_statesMutex);
    g_hostWindows.erase(hWnd);
}

void UntrackListView(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_statesMutex);
    g_listWindows.erase(hWnd);
}

int ClampXToMonitorWorkArea(HWND hWnd,
                            int x,
                            int y,
                            int width,
                            int height) {
    if (width <= 0) {
        return x;
    }

    if (height <= 0) {
        RECT currentRect = {};
        if (GetWindowRect(hWnd, &currentRect)) {
            height =
                static_cast<int>(currentRect.bottom - currentRect.top);
        }
    }

    RECT proposedRect = {
        x,
        y,
        x + width,
        y + std::max(1, height),
    };

    HMONITOR monitor =
        MonitorFromRect(&proposedRect, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return x;
    }

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return x;
    }

    const int workLeft =
        static_cast<int>(monitorInfo.rcWork.left);
    const int workRight =
        static_cast<int>(monitorInfo.rcWork.right);
    const int workWidth = workRight - workLeft;

    if (workWidth <= 0 || width >= workWidth) {
        return workLeft;
    }

    return std::clamp(
        x,
        workLeft,
        workRight - width);
}

int CenterAndClampX(HWND hWnd,
                    int baseX,
                    int baseWidth,
                    int targetWidth,
                    int y,
                    int height) {
    const int centeredX =
        baseX + (baseWidth - targetWidth) / 2;

    return ClampXToMonitorWorkArea(
        hWnd,
        centeredX,
        y,
        targetWidth,
        height);
}

int GetDesiredHostWidth(HWND host, HWND jumpView, const WINDOWPOS* windowPos) {
    const int hostX = GetRelativeX(host, jumpView, windowPos);
    return std::max(1, GetTargetWidthForWindow(jumpView) - hostX);
}

int GetDesiredListWidth(HWND list, HWND host, const WINDOWPOS* windowPos) {
    HWND jumpView = GetParent(host);
    if (!IsSIBJumpView(jumpView)) {
        return 0;
    }

    RECT hostRect = {};
    if (!GetRelativeRect(host, jumpView, &hostRect)) {
        return 0;
    }

    const int hostWidth =
        std::max(1, GetTargetWidthForWindow(jumpView) - static_cast<int>(hostRect.left));
    const int listX = GetRelativeX(list, host, windowPos);
    const int edgeMargin = GetEdgeMarginForWindow(jumpView);

    return std::max(1, hostWidth - listX - edgeMargin);
}

LRESULT CALLBACK BarHostSubclassProc(HWND hWnd,
                                     UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR) {
    if (uMsg == WM_NCDESTROY) {
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        UntrackBarHost(hWnd);
        return result;
    }

    if (uMsg == WM_WINDOWPOSCHANGING &&
        !g_unloading.load(std::memory_order_relaxed)) {
        auto* windowPos = reinterpret_cast<WINDOWPOS*>(lParam);
        HWND jumpView = GetParent(hWnd);

        if (windowPos && !(windowPos->flags & SWP_NOSIZE) &&
            IsSIBJumpView(jumpView)) {
            windowPos->cx = GetDesiredHostWidth(hWnd, jumpView, windowPos);
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ListViewSubclassProc(HWND hWnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      DWORD_PTR) {
    if (uMsg == WM_NCDESTROY) {
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        UntrackListView(hWnd);
        return result;
    }

    if (uMsg == WM_WINDOWPOSCHANGING &&
        !g_unloading.load(std::memory_order_relaxed)) {
        auto* windowPos = reinterpret_cast<WINDOWPOS*>(lParam);
        HWND host = GetParent(hWnd);

        if (windowPos && !(windowPos->flags & SWP_NOSIZE) &&
            IsSIBBarHost(host)) {
            const int width = GetDesiredListWidth(hWnd, host, windowPos);
            if (width > 0) {
                windowPos->cx = width;
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

bool AttachBarHost(HWND host) {
    if (!IsWindow(host) || !IsSIBBarHost(host)) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(g_statesMutex);
        const bool inserted = g_hostWindows.insert(host).second;
        if (!inserted) {
            return true;
        }
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            host, BarHostSubclassProc, 0)) {
        UntrackBarHost(host);
        Wh_Log(L"Failed to subclass SIBBarHost %p", host);
        return false;
    }

    return true;
}

bool AttachListView(HWND list) {
    if (!IsWindow(list) || !IsSIBListView(list)) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(g_statesMutex);
        const bool inserted = g_listWindows.insert(list).second;
        if (!inserted) {
            return true;
        }
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            list, ListViewSubclassProc, 0)) {
        UntrackListView(list);
        Wh_Log(L"Failed to subclass SysListView32 %p", list);
        return false;
    }

    return true;
}

void AttachChildSubclasses(HWND jumpView) {
    if (!IsWindow(jumpView) || !IsSIBJumpView(jumpView)) {
        return;
    }

    for (HWND host = GetWindow(jumpView, GW_CHILD); host;
         host = GetWindow(host, GW_HWNDNEXT)) {
        if (!IsClass(host, L"SIBBarHost")) {
            continue;
        }

        AttachBarHost(host);

        for (HWND list = GetWindow(host, GW_CHILD); list;
             list = GetWindow(list, GW_HWNDNEXT)) {
            if (IsClass(list, L"SysListView32")) {
                AttachListView(list);
            }
        }
    }
}

void ResizeChildren(HWND jumpView) {
    if (g_unloading.load(std::memory_order_relaxed) || !IsSIBJumpView(jumpView)) {
        return;
    }

    const int targetWidth = GetTargetWidthForWindow(jumpView);
    const int edgeMargin = GetEdgeMarginForWindow(jumpView);

    for (HWND host = GetWindow(jumpView, GW_CHILD); host;
         host = GetWindow(host, GW_HWNDNEXT)) {
        if (!IsClass(host, L"SIBBarHost")) {
            continue;
        }

        AttachBarHost(host);

        RECT hostRect = {};
        if (!GetRelativeRect(host, jumpView, &hostRect)) {
            continue;
        }

        const int hostX = hostRect.left;
        const int hostY = hostRect.top;
        const int hostHeight = hostRect.bottom - hostRect.top;
        if (hostHeight <= 0) {
            continue;
        }

        const int newHostWidth = std::max(1, targetWidth - hostX);

        SetWindowPos(host, nullptr, hostX, hostY, newHostWidth, hostHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

        for (HWND list = GetWindow(host, GW_CHILD); list;
             list = GetWindow(list, GW_HWNDNEXT)) {
            if (!IsClass(list, L"SysListView32")) {
                continue;
            }

            AttachListView(list);

            RECT listRect = {};
            if (!GetRelativeRect(list, host, &listRect)) {
                continue;
            }

            const int listX = listRect.left;
            const int listY = listRect.top;
            const int listHeight = listRect.bottom - listRect.top;
            if (listHeight <= 0) {
                continue;
            }

            const int newListWidth =
                std::max(1, newHostWidth - listX - edgeMargin);

            SetWindowPos(list, nullptr, listX, listY, newListWidth, listHeight,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        }
    }
}

void ApplyTargetWidth(HWND jumpView) {
    if (g_unloading.load(std::memory_order_relaxed) || !IsWindow(jumpView) ||
        !IsSIBJumpView(jumpView)) {
        return;
    }

    auto state = GetState(jumpView);
    if (!state) {
        return;
    }

    AttachChildSubclasses(jumpView);

    RECT rect = {};
    if (!GetWindowRect(jumpView, &rect)) {
        return;
    }

    const int currentWidth =
        static_cast<int>(rect.right - rect.left);
    const int height =
        static_cast<int>(rect.bottom - rect.top);
    if (currentWidth <= 0 || height <= 0) {
        return;
    }

    const int targetWidth =
        GetTargetWidthForWindow(jumpView);

    if (currentWidth != targetWidth) {
        const int newX = CenterAndClampX(
            jumpView,
            static_cast<int>(rect.left),
            currentWidth,
            targetWidth,
            static_cast<int>(rect.top),
            height);

        state->applyingModResize.store(
            true, std::memory_order_release);

        SetWindowPos(
            jumpView,
            nullptr,
            newX,
            static_cast<int>(rect.top),
            targetWidth,
            height,
            SWP_NOZORDER |
                SWP_NOACTIVATE |
                SWP_NOOWNERZORDER);

        state->applyingModResize.store(
            false, std::memory_order_release);
    }

    ResizeChildren(jumpView);
}

LRESULT CALLBACK JumpViewSubclassProc(HWND hWnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      DWORD_PTR) {
    if (uMsg == WM_NCDESTROY) {
        LRESULT result =
            DefSubclassProc(hWnd, uMsg, wParam, lParam);
        RemoveState(hWnd);
        return result;
    }

    if (uMsg == WM_WINDOWPOSCHANGING &&
        !g_unloading.load(std::memory_order_relaxed)) {
        auto state = GetState(hWnd);
        auto* windowPos =
            reinterpret_cast<WINDOWPOS*>(lParam);

        if (state && windowPos) {
            const int targetWidth =
                GetTargetWidthForWindow(hWnd);

            const bool applyingModResize =
                state->applyingModResize.load(
                    std::memory_order_acquire);

            int referenceWidth =
                state->originalWidth.load(
                    std::memory_order_relaxed);

            if (!applyingModResize &&
                !(windowPos->flags & SWP_NOSIZE) &&
                windowPos->cx > 0 &&
                windowPos->cx != targetWidth) {
                referenceWidth = windowPos->cx;

                state->originalWidth.store(
                    referenceWidth,
                    std::memory_order_relaxed);
            }

            if (!applyingModResize &&
                !(windowPos->flags & SWP_NOMOVE)) {
                state->originalX.store(
                    windowPos->x,
                    std::memory_order_relaxed);

                if (referenceWidth <= 0) {
                    referenceWidth =
                        (!(windowPos->flags & SWP_NOSIZE) &&
                         windowPos->cx > 0)
                            ? windowPos->cx
                            : targetWidth;
                }

                RECT currentRect = {};
                const bool haveCurrentRect =
                    GetWindowRect(hWnd, &currentRect) != FALSE;

                const int proposedY =
                    windowPos->y;

                const int proposedHeight =
                    (!(windowPos->flags & SWP_NOSIZE) &&
                     windowPos->cy > 0)
                        ? windowPos->cy
                        : (haveCurrentRect
                               ? static_cast<int>(
                                     currentRect.bottom -
                                     currentRect.top)
                               : 1);

                windowPos->x = CenterAndClampX(
                    hWnd,
                    windowPos->x,
                    referenceWidth,
                    targetWidth,
                    proposedY,
                    proposedHeight);
            }

            if (!(windowPos->flags & SWP_NOSIZE)) {
                windowPos->cx = targetWidth;
            }
        }
    }

    LRESULT result =
        DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (g_unloading.load(std::memory_order_relaxed)) {
        return result;
    }

    switch (uMsg) {
        case WM_SIZE:
            AttachChildSubclasses(hWnd);
            ResizeChildren(hWnd);
            break;

        case WM_DPICHANGED:
            ApplyTargetWidth(hWnd);
            break;
    }

    return result;
}

bool AttachJumpView(HWND jumpView) {
    if (!IsWindow(jumpView) || !IsSIBJumpView(jumpView)) {
        return false;
    }

    bool alreadyTracked = false;
    {
        std::lock_guard<std::mutex> lock(g_statesMutex);
        alreadyTracked = g_states.find(jumpView) != g_states.end();
    }

    if (alreadyTracked) {
        AttachChildSubclasses(jumpView);
        return true;
    }

    auto state = std::make_shared<JumpViewState>();

    RECT originalRect = {};
    if (GetWindowRect(jumpView, &originalRect)) {
        state->originalWidth.store(
            static_cast<int>(
                originalRect.right - originalRect.left),
            std::memory_order_relaxed);

        state->originalX.store(
            static_cast<int>(originalRect.left),
            std::memory_order_relaxed);
    }

    bool inserted = false;
    {
        std::lock_guard<std::mutex> lock(g_statesMutex);
        inserted = g_states.emplace(jumpView, state).second;
    }

    if (!inserted) {
        AttachChildSubclasses(jumpView);
        return true;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            jumpView, JumpViewSubclassProc, 0)) {
        RemoveState(jumpView);
        Wh_Log(L"Failed to subclass SIBJumpView %p", jumpView);
        return false;
    }

    AttachChildSubclasses(jumpView);
    Wh_Log(L"Subclassed SIBJumpView %p", jumpView);
    return true;
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
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd || g_unloading.load(std::memory_order_relaxed)) {
        return hWnd;
    }

    HWND jumpView = GetOwningJumpView(hWnd);
    if (!jumpView) {
        return hWnd;
    }

    if (!AttachJumpView(jumpView)) {
        return hWnd;
    }

    if (IsSIBBarHost(hWnd)) {
        AttachBarHost(hWnd);
    } else if (IsSIBListView(hWnd)) {
        AttachListView(hWnd);
    }

    if (hWnd == jumpView) {
        ApplyTargetWidth(jumpView);
    } else {
        ResizeChildren(jumpView);
    }

    return hWnd;
}

BOOL CALLBACK EnumJumpViewsProc(HWND hWnd, LPARAM) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != GetCurrentProcessId() || !IsSIBJumpView(hWnd)) {
        return TRUE;
    }

    if (AttachJumpView(hWnd)) {
        ApplyTargetWidth(hWnd);
    }

    return TRUE;
}

std::vector<HWND> GetTrackedJumpViews() {
    std::lock_guard<std::mutex> lock(g_statesMutex);

    std::vector<HWND> windows;
    windows.reserve(g_states.size());
    for (const auto& item : g_states) {
        windows.push_back(item.first);
    }

    return windows;
}

std::vector<HWND> GetTrackedBarHosts() {
    std::lock_guard<std::mutex> lock(g_statesMutex);
    return std::vector<HWND>(g_hostWindows.begin(), g_hostWindows.end());
}

std::vector<HWND> GetTrackedListViews() {
    std::lock_guard<std::mutex> lock(g_statesMutex);
    return std::vector<HWND>(g_listWindows.begin(), g_listWindows.end());
}

void RestoreChildLayout(HWND jumpView) {
    if (!IsWindow(jumpView) || !IsSIBJumpView(jumpView)) {
        return;
    }

    const int stockEdgeMargin = GetStockEdgeMarginForWindow(jumpView);

    RECT jumpRect = {};
    if (!GetWindowRect(jumpView, &jumpRect)) {
        return;
    }

    const int jumpWidth = jumpRect.right - jumpRect.left;
    if (jumpWidth <= 0) {
        return;
    }

    for (HWND host = GetWindow(jumpView, GW_CHILD); host;
         host = GetWindow(host, GW_HWNDNEXT)) {
        if (!IsClass(host, L"SIBBarHost")) {
            continue;
        }

        RECT hostRect = {};
        if (!GetRelativeRect(host, jumpView, &hostRect)) {
            continue;
        }

        const int hostX = hostRect.left;
        const int hostY = hostRect.top;
        const int hostHeight = hostRect.bottom - hostRect.top;
        if (hostHeight <= 0) {
            continue;
        }

        const int hostWidth = std::max(1, jumpWidth - hostX);
        SetWindowPos(host, nullptr, hostX, hostY, hostWidth, hostHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

        for (HWND list = GetWindow(host, GW_CHILD); list;
             list = GetWindow(list, GW_HWNDNEXT)) {
            if (!IsClass(list, L"SysListView32")) {
                continue;
            }

            RECT listRect = {};
            if (!GetRelativeRect(list, host, &listRect)) {
                continue;
            }

            const int listX = listRect.left;
            const int listY = listRect.top;
            const int listHeight = listRect.bottom - listRect.top;
            if (listHeight <= 0) {
                continue;
            }

            const int listWidth =
                std::max(1, hostWidth - listX - stockEdgeMargin);

            SetWindowPos(list, nullptr, listX, listY, listWidth, listHeight,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        }
    }
}

void RestoreJumpViewGeometry(HWND hWnd) {
    auto state = GetState(hWnd);
    if (!state || !IsWindow(hWnd)) {
        return;
    }

    const int originalWidth =
        state->originalWidth.load(
            std::memory_order_relaxed);

    const int originalX =
        state->originalX.load(
            std::memory_order_relaxed);

    RECT rect = {};
    if (originalWidth > 0 &&
        GetWindowRect(hWnd, &rect)) {
        const int height =
            static_cast<int>(
                rect.bottom - rect.top);

        if (height > 0) {
            SetWindowPos(
                hWnd,
                nullptr,
                originalX,
                static_cast<int>(rect.top),
                originalWidth,
                height,
                SWP_NOZORDER |
                    SWP_NOACTIVATE |
                    SWP_NOOWNERZORDER);
        }
    }

    RestoreChildLayout(hWnd);

    RedrawWindow(hWnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
}

void DetachAllSubclasses() {
    for (HWND hWnd : GetTrackedListViews()) {
        if (IsWindow(hWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, ListViewSubclassProc);
        }
    }

    for (HWND hWnd : GetTrackedBarHosts()) {
        if (IsWindow(hWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, BarHostSubclassProc);
        }
    }

    for (HWND hWnd : GetTrackedJumpViews()) {
        if (IsWindow(hWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, JumpViewSubclassProc);
        }
    }

    std::lock_guard<std::mutex> lock(g_statesMutex);
    g_listWindows.clear();
    g_hostWindows.clear();
    g_states.clear();
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing, version %s", WH_MOD_VERSION);

    LoadSettings();

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumJumpViewsProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(EnumJumpViewsProc, 0);
}

void Wh_ModUninit() {
    g_unloading.store(true, std::memory_order_relaxed);

    for (HWND hWnd : GetTrackedJumpViews()) {
        RestoreJumpViewGeometry(hWnd);
    }

    DetachAllSubclasses();
}
