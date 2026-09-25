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

The mod targets StartAllBack's `SIBJumpView` hierarchy in `explorer.exe`. Instead of globally hooking window-resize APIs, it subclasses only the relevant jump-list window and adjusts its size when Windows is about to resize that window.

## Settings

- **Jump List width** - target width at 100% scaling (96 DPI). The value is limited to 120-600 pixels and is scaled automatically for the DPI of the monitor where the jump list is shown.
- The internal list uses a fixed 6-pixel right margin at 100% scaling, also scaled automatically for DPI.

Changes are applied immediately to existing StartAllBack jump-list windows. When the mod is disabled, the original StartAllBack width is restored.

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
#include <vector>

namespace {

constexpr int kDefaultWidth96 = 225;
constexpr int kMinWidth96 = 120;
constexpr int kMaxWidth96 = 600;
constexpr int kEdgeMargin96 = 6;

std::atomic<int> g_width96{kDefaultWidth96};
std::atomic_bool g_unloading{false};

struct JumpViewState {
    std::atomic<int> originalWidth{0};
    std::atomic_bool applyingModResize{false};
};

std::mutex g_statesMutex;
std::unordered_map<HWND, std::shared_ptr<JumpViewState>> g_states;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

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

bool GetRelativeRect(HWND hWnd, HWND parent, RECT* rect) {
    if (!rect || !GetWindowRect(hWnd, rect)) {
        return false;
    }

    MapWindowPoints(HWND_DESKTOP, parent, reinterpret_cast<LPPOINT>(rect), 2);
    return true;
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

int GetCurrentWidth(HWND hWnd) {
    RECT rect = {};
    if (!GetWindowRect(hWnd, &rect)) {
        return 0;
    }

    return rect.right - rect.left;
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

    RECT rect = {};
    if (!GetWindowRect(jumpView, &rect)) {
        return;
    }

    const int height = rect.bottom - rect.top;
    if (height <= 0) {
        return;
    }

    const int targetWidth = GetTargetWidthForWindow(jumpView);
    const int currentWidth = rect.right - rect.left;

    if (currentWidth != targetWidth) {
        state->applyingModResize.store(true, std::memory_order_release);
        SetWindowPos(jumpView, nullptr, 0, 0, targetWidth, height,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE |
                         SWP_NOOWNERZORDER);
        state->applyingModResize.store(false, std::memory_order_release);
    } else {
        ResizeChildren(jumpView);
    }
}

LRESULT CALLBACK JumpViewSubclassProc(HWND hWnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      DWORD_PTR) {
    if (uMsg == WM_NCDESTROY) {
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        RemoveState(hWnd);
        return result;
    }

    if (uMsg == WM_WINDOWPOSCHANGING &&
        !g_unloading.load(std::memory_order_relaxed)) {
        auto state = GetState(hWnd);
        auto* windowPos = reinterpret_cast<WINDOWPOS*>(lParam);

        if (state && windowPos && !(windowPos->flags & SWP_NOSIZE)) {
            if (!state->applyingModResize.load(std::memory_order_acquire) &&
                windowPos->cx > 0) {
                state->originalWidth.store(windowPos->cx,
                                           std::memory_order_relaxed);
            }

            windowPos->cx = GetTargetWidthForWindow(hWnd);
        }
    }

    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (g_unloading.load(std::memory_order_relaxed)) {
        return result;
    }

    switch (uMsg) {
        case WM_SIZE:
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

    {
        std::lock_guard<std::mutex> lock(g_statesMutex);
        if (g_states.find(jumpView) != g_states.end()) {
            return true;
        }
    }

    auto state = std::make_shared<JumpViewState>();
    state->originalWidth.store(GetCurrentWidth(jumpView),
                               std::memory_order_relaxed);

    {
        std::lock_guard<std::mutex> lock(g_statesMutex);
        auto [it, inserted] = g_states.emplace(jumpView, state);
        if (!inserted) {
            return true;
        }
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            jumpView, JumpViewSubclassProc, 0)) {
        RemoveState(jumpView);
        Wh_Log(L"Failed to subclass SIBJumpView %p", jumpView);
        return false;
    }

    Wh_Log(L"Subclassed SIBJumpView %p", jumpView);
    return true;
}

HWND GetOwningJumpView(HWND hWnd) {
    if (IsSIBJumpView(hWnd)) {
        return hWnd;
    }

    if (IsClass(hWnd, L"SIBBarHost")) {
        HWND parent = GetParent(hWnd);
        return IsSIBJumpView(parent) ? parent : nullptr;
    }

    if (IsClass(hWnd, L"SysListView32")) {
        HWND host = GetParent(hWnd);
        if (!IsClass(host, L"SIBBarHost")) {
            return nullptr;
        }

        HWND jumpView = GetParent(host);
        return IsSIBJumpView(jumpView) ? jumpView : nullptr;
    }

    return nullptr;
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

    if (AttachJumpView(jumpView)) {
        if (hWnd == jumpView) {
            ApplyTargetWidth(jumpView);
        } else {
            ResizeChildren(jumpView);
        }
    }

    return hWnd;
}

BOOL CALLBACK EnumJumpViewsProc(HWND hWnd, LPARAM lParam) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != GetCurrentProcessId() || !IsSIBJumpView(hWnd)) {
        return TRUE;
    }

    const bool applyLayout = lParam != 0;
    if (AttachJumpView(hWnd) && applyLayout) {
        ApplyTargetWidth(hWnd);
    }

    return TRUE;
}

std::vector<HWND> GetTrackedWindows() {
    std::lock_guard<std::mutex> lock(g_statesMutex);

    std::vector<HWND> windows;
    windows.reserve(g_states.size());
    for (const auto& [hWnd, state] : g_states) {
        (void)state;
        windows.push_back(hWnd);
    }

    return windows;
}

void RestoreAndDetachJumpView(HWND hWnd) {
    auto state = GetState(hWnd);
    if (!state) {
        return;
    }

    if (IsWindow(hWnd)) {
        const int originalWidth =
            state->originalWidth.load(std::memory_order_relaxed);

        RECT rect = {};
        if (originalWidth > 0 && GetWindowRect(hWnd, &rect)) {
            const int height = rect.bottom - rect.top;
            if (height > 0) {
                SetWindowPos(hWnd, nullptr, 0, 0, originalWidth, height,
                             SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE |
                                 SWP_NOOWNERZORDER);
            }
        }

        RedrawWindow(hWnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);

        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hWnd, JumpViewSubclassProc);
    }

    RemoveState(hWnd);
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing, version %s", WH_MOD_VERSION);

    LoadSettings();
    g_unloading.store(false, std::memory_order_relaxed);

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumJumpViewsProc, 1);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(EnumJumpViewsProc, 1);
}

void Wh_ModUninit() {
    g_unloading.store(true, std::memory_order_relaxed);

    for (HWND hWnd : GetTrackedWindows()) {
        RestoreAndDetachJumpView(hWnd);
    }
}
