// ==WindhawkMod==
// @id              force-desktop-icon-spacing
// @name            Force Desktop Icon Spacing
// @name:zh-CN      强制桌面图标间距
// @description     Set the desktop icon grid spacing (X/Y) and reserve a blank margin above the taskbar. Event-driven and DPI-aware; the original spacing and icon area height are restored when the mod is disabled.
// @description:zh-CN  设置桌面图标的水平/垂直网格间距，并在任务栏上方预留底部空白边距。事件驱动、支持 DPI 缩放；停用模组后自动还原原始间距与图标区域高度。
// @version         1.1.2
// @author          Koewen
// @github          https://github.com/Koewen
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Precisely control the desktop icon spacing: set the horizontal/vertical icon grid spacing and reserve a blank margin at the bottom of the icon area (above the taskbar).

## How it works

- The desktop list view (`SysListView32`) is subclassed (event-driven, no polling): every spacing request Explorer makes is transparently replaced with the configured values, and the icon area height is capped so it never enters the reserved bottom strip.
- Values are absolute and applied in one pass: the same settings always produce the same layout, never cumulative drift.
- The vertical grid is anchored at the top: increasing the vertical spacing stretches the grid downward, so the extra space is absorbed by the bottom region. No blank strip is created at the top, and the icon area is never shifted as a whole.
- The bottom margin caps the icon area height only (the top edge stays fixed), so icon columns wrap earlier and never enter the reserved bottom strip.
- All pixel values are scaled by the list view's DPI: the configured numbers apply at 100% scaling and scale up automatically on higher DPI.

## Settings

- **Horizontal spacing (pixels)**: width of each icon grid cell, at 100% scaling. Larger = more horizontal gap between icons.
- **Vertical spacing (pixels)**: height of each icon grid cell, at 100% scaling. Larger = taller rows; the grid grows downward and the extra space is absorbed by the bottom region.
- **Bottom margin (pixels)**: blank strip reserved at the bottom of the icon area, above the taskbar, at 100% scaling. The icon grid never enters it. 0 = no reservation.
- **Rearrange icons on startup**: arrange icons once when the mod loads (same as the desktop "Arrange icons" command) so the spacing/margin take effect immediately. Note this only applies when the mod is loaded into an already-running Explorer (enabling or updating the mod); when Explorer starts with the mod, Explorer places icons from its saved layout afterwards. Changes made in the settings always rearrange.

## Notes

- The grid cell size stays fixed when you change the icon size (Large/Medium/Small icons or Ctrl+wheel); adjust the spacing settings when you switch to Large icons, otherwise large icons may overlap their labels.
- This mod doesn't write any registry values. If you want free icon placement, turn off "Auto arrange icons" and "Align icons to grid" in the desktop right-click menu yourself.
- Disabling or unloading the mod restores the original icon spacing and the full icon area height. Icons stay where the mod arranged them (as with a manual "Arrange icons").
- The bottom margin applies to the whole icon area (the list view spans all monitors); on multi-monitor setups it lands above the taskbar of the bottom-most monitor.

## 简体中文说明

精确控制桌面图标间距：自由调节图标网格的水平/垂直间距，并在图标区域底部（任务栏上方）预留空白边距。

### 工作原理

- 通过子类化（事件驱动，无轮询）挂钩桌面列表视图（`SysListView32`）：资源管理器发出的每个间距设置请求都会被透明替换为配置值，图标区域高度被限制在预留边距之上。
- 所有数值均为绝对值、一次性应用——同一配置永远得到同一布局，不存在多次叠加漂移。
- 垂直网格顶端锚定：调大垂直间距时行距拉大、网格向下延伸，多余空间挤占底部区域——顶部不会产生空白，图标区域也不会被整体移动。
- 底部边距只限制图标区域高度（顶边固定），图标换列提前，永不进入底部预留区。
- 所有像素值按列表视图的 DPI 自动缩放：配置数值对应 100% 缩放，高 DPI 下自动放大。

### 设置项

- **水平间距（像素）**：图标网格单元宽度（100% 缩放下），数值越大图标水平间距越大。
- **垂直间距（像素）**：图标网格单元高度（100% 缩放下），数值越大行距越大，网格向下延伸，多余空间挤占底部区域。
- **底部边距（像素）**：图标区域底部（任务栏上方）预留的空白高度（100% 缩放下），图标网格不会进入该区域。0 = 不预留。
- **启动时重排图标**：模组加载时执行一次"排列图标"，使间距/边距立即生效。注意：仅在模组加载到已运行的资源管理器时（启用/更新模组）起作用；资源管理器随模组启动时，系统会按保存的布局放置图标。在设置面板修改参数时始终会重排。

### 注意

- 切换图标大小（大/中/小图标或 Ctrl+滚轮）时网格单元尺寸保持不变；切换到大图标时请相应调大间距设置，否则大图标可能与文字标签重叠。
- 本模组不写入任何注册表值。如需自由摆放图标，请自行在桌面右键菜单中关闭"自动排列图标"和"将图标与网格对齐"。
- 停用或卸载模组会自动恢复原始图标间距与图标区域完整高度；图标停留在重排后的位置（与手动"排列图标"一致）。
- 底部边距作用于整个图标区域（列表视图横跨所有显示器）；多显示器下预留空白位于最底部显示器的任务栏上方。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- IconSpacingX: 95
  $name: Horizontal spacing (pixels, at 100% scaling)
  $name:zh-CN: 水平间距（X 像素，100% 缩放下）
  $description: Width of each icon grid cell. Larger = more horizontal gap between icons. Scaled automatically by display DPI.
  $description:zh-CN: 图标网格单元宽度，数值越大图标水平间距越大。按显示器 DPI 自动缩放。
- IconSpacingY: 100
  $name: Vertical spacing (pixels, at 100% scaling)
  $name:zh-CN: 垂直间距（Y 像素，100% 缩放下）
  $description: Height of each icon grid cell. Larger = taller rows; the grid grows downward and the extra space is absorbed by the bottom region. Scaled automatically by display DPI.
  $description:zh-CN: 图标网格单元高度，数值越大行距越大，网格向下延伸，多余空间挤占底部区域。按显示器 DPI 自动缩放。
- BottomMargin: 0
  $name: Bottom margin (pixels, at 100% scaling)
  $name:zh-CN: 底部边距（像素，100% 缩放下）
  $description: Blank strip reserved at the bottom of the icon area, above the taskbar. The icon grid never enters it. 0 = no reservation. Scaled automatically by display DPI.
  $description:zh-CN: 在图标区域底部（任务栏上方）预留的空白高度，图标网格不会进入该区域。0 = 不预留。按显示器 DPI 自动缩放。
- ArrangeOnStartup: true
  $name: Rearrange icons on startup
  $name:zh-CN: 启动时重排图标
  $description: Arrange icons once when the mod loads so the spacing/margin take effect immediately. Only applies when the mod is loaded into an already-running Explorer (enable/update); when Explorer starts with the mod, Explorer places icons from its saved layout afterwards. Disable to keep hand-drawn icon layouts.
  $description:zh-CN: 模组加载时执行一次"排列图标"，使间距/边距立即生效。仅在模组加载到已运行的资源管理器时（启用/更新模组）起作用；资源管理器随模组启动时，系统会按保存的布局放置图标。若希望保留手动拖放的图标布局，可关闭此项。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <windhawk_utils.h>
#include <atomic>

static std::atomic<int> g_spacingX{95};
static std::atomic<int> g_spacingY{100};
static std::atomic<int> g_bottomMargin{0};
static std::atomic<bool> g_arrangeOnStartup{true};

// The spacing Explorer last requested (captured from its own
// LVM_SETICONSPACING / read at subclass install; -1 means "reset to default").
// 0 = not captured yet. Restored on unload.
static std::atomic<LONG> g_originalSpacing{0};

// The currently subclassed desktop list view.
static std::atomic<HWND> g_subclassedLV{nullptr};

static int Clamp(int value, int minValue, int maxValue) {
    return value < minValue ? minValue : (value > maxValue ? maxValue : value);
}

static void LoadSettings() {
    g_spacingX = Clamp(Wh_GetIntSetting(L"IconSpacingX"), 40, 1000);
    g_spacingY = Clamp(Wh_GetIntSetting(L"IconSpacingY"), 35, 1000);
    g_bottomMargin = Clamp(Wh_GetIntSetting(L"BottomMargin"), 0, 1000);
    g_arrangeOnStartup = Wh_GetIntSetting(L"ArrangeOnStartup") != 0;
}

// Check that the window is the desktop's SysListView32 owned by THIS process.
// The DefView's host must be Progman or WorkerW, otherwise folder windows that
// use a native list view would be mistaken for the desktop.
static bool IsDesktopListView(HWND hWnd) {
    WCHAR cls[64];
    if (!GetClassName(hWnd, cls, ARRAYSIZE(cls)) || _wcsicmp(cls, L"SysListView32") != 0) {
        return false;
    }

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hWnd, &pid) || pid != GetCurrentProcessId()) {
        return false;
    }

    HWND defView = GetParent(hWnd);
    if (!defView || !GetClassName(defView, cls, ARRAYSIZE(cls)) ||
        _wcsicmp(cls, L"SHELLDLL_DefView") != 0) {
        return false;
    }

    HWND host = GetParent(defView);
    if (!host || !GetClassName(host, cls, ARRAYSIZE(cls)) ||
        (_wcsicmp(cls, L"Progman") != 0 && _wcsicmp(cls, L"WorkerW") != 0)) {
        return false;
    }

    return true;
}

static HWND FindDesktopListView() {
    HWND progman = FindWindow(L"Progman", NULL);
    if (progman) {
        HWND defView = FindWindowEx(progman, NULL, L"SHELLDLL_DefView", NULL);
        if (defView) {
            HWND lv = FindWindowEx(defView, NULL, L"SysListView32", NULL);
            if (lv && IsDesktopListView(lv)) return lv;
        }
    }

    HWND worker = NULL;
    while ((worker = FindWindowEx(NULL, worker, L"WorkerW", NULL)) != NULL) {
        HWND defView = FindWindowEx(worker, NULL, L"SHELLDLL_DefView", NULL);
        if (defView) {
            HWND lv = FindWindowEx(defView, NULL, L"SysListView32", NULL);
            if (lv && IsDesktopListView(lv)) return lv;
        }
    }

    return NULL;
}

// Apply our values: the LVM_SETICONSPACING lParam is replaced by the subclass,
// and the SetWindowPos height is capped by WM_WINDOWPOSCHANGING in the
// subclass. Both adjustments are therefore centralized in one place and stay
// idempotent no matter how often this runs.
static void ApplySettings(HWND hWnd, BOOL bRearrange) {
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }

    // The subclass replaces the lParam with our values; the incoming lParam
    // (possibly 0 when Explorer's spacing is not captured yet) never reaches
    // the list view.
    SendMessage(hWnd, LVM_SETICONSPACING, 0, (LPARAM)g_originalSpacing.load(std::memory_order_relaxed));

    HWND parent = GetParent(hWnd);
    RECT rc;
    if (parent && GetClientRect(parent, &rc)) {
        SetWindowPos(hWnd, NULL, 0, 0, rc.right, rc.bottom,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    if (bRearrange) {
        SendMessage(hWnd, LVM_ARRANGE, LVA_DEFAULT, 0);
    }

    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
}

static LRESULT CALLBACK DesktopListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                                    LPARAM lParam, DWORD_PTR uIdSubclass,
                                                    DWORD_PTR dwRefData) {
    switch (uMsg) {
        case LVM_SETICONSPACING: {
            // Remember what Explorer last sent (any value, including -1 for
            // "reset to default", so unload restores exactly that), then
            // substitute our values and LET THE MESSAGE REACH THE LIST VIEW -
            // returning early here would swallow it and the spacing would
            // never be applied.
            g_originalSpacing.store((LONG)lParam, std::memory_order_relaxed);

            UINT dpi = GetDpiForWindow(hWnd);
            lParam = MAKELPARAM(MulDiv(g_spacingX.load(std::memory_order_relaxed), (int)dpi, 96),
                                MulDiv(g_spacingY.load(std::memory_order_relaxed), (int)dpi, 96));
            break;
        }

        case WM_WINDOWPOSCHANGING: {
            auto* wp = (WINDOWPOS*)lParam;
            if (!(wp->flags & SWP_NOSIZE)) {
                HWND parent = GetParent(hWnd);
                RECT rc;
                if (parent && GetClientRect(parent, &rc)) {
                    UINT dpi = GetDpiForWindow(hWnd);
                    int target = rc.bottom - MulDiv(g_bottomMargin.load(std::memory_order_relaxed), (int)dpi, 96);
                    int minHeight = MulDiv(200, (int)dpi, 96);
                    if (target < minHeight) target = minHeight;
                    if (wp->cy > target) wp->cy = target;
                }
            }
            break;
        }

        case WM_DPICHANGED_AFTERPARENT:
            // Re-apply on DPI changes so spacing/margin follow the new scale.
            ApplySettings(hWnd, FALSE);
            break;

        case WM_NCDESTROY: {
            // Use a local copy: when the comparison fails,
            // compare_exchange_strong writes the current value into the
            // expected argument, which must not be the hWnd we still need
            // for DefSubclassProc below.
            HWND expected = hWnd;
            g_subclassedLV.compare_exchange_strong(expected, nullptr, std::memory_order_relaxed);
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void SubclassDesktopListView(HWND hWnd) {
    HWND prev = g_subclassedLV.exchange(hWnd);
    if (prev == hWnd) {
        return;  // already subclassed
    }

    if (prev && prev != hWnd && IsWindow(prev)) {
        // The desktop list view was recreated; drop the stale subclass.
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(prev, DesktopListViewSubclassProc, 0);
    }

    // Capture Explorer's current spacing before the subclass is installed,
    // so it can be restored on unload.
    g_originalSpacing = (LONG)SendMessage(hWnd, LVM_GETITEMSPACING, FALSE, 0);

    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopListViewSubclassProc, 0);
    Wh_Log(L"Desktop list view subclassed: %08X", (DWORD)(ULONG_PTR)hWnd);

    ApplySettings(hWnd, g_arrangeOnStartup ? TRUE : FALSE);
}

static void UnsubclassDesktopListView(HWND hWnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, DesktopListViewSubclassProc, 0);

    // Restore the spacing Explorer last sent (including -1 = "reset to
    // default"), then the full height.
    LONG original = g_originalSpacing.load(std::memory_order_relaxed);
    if (original != 0) {
        SendMessage(hWnd, LVM_SETICONSPACING, 0, (LPARAM)original);
    }

    HWND parent = GetParent(hWnd);
    RECT rc;
    if (parent && GetClientRect(parent, &rc)) {
        SetWindowPos(hWnd, NULL, 0, 0, rc.right, rc.bottom,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    InvalidateRect(hWnd, NULL, TRUE);
    Wh_Log(L"Desktop list view restored: %08X", (DWORD)(ULONG_PTR)hWnd);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                         X, Y, nWidth, nHeight, hWndParent, hMenu,
                                         hInstance, lpParam);
    if (hWnd && IsDesktopListView(hWnd)) {
        SubclassDesktopListView(hWnd);
    }
    return hWnd;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hWnd = FindDesktopListView();
    if (hWnd) {
        SubclassDesktopListView(hWnd);
    } else {
        Wh_Log(L"Desktop list view not found; waiting for CreateWindowExW");
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    HWND hWnd = g_subclassedLV.exchange(nullptr);
    if (hWnd && IsWindow(hWnd)) {
        UnsubclassDesktopListView(hWnd);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    HWND hWnd = g_subclassedLV.load();
    if (hWnd && IsWindow(hWnd)) {
        // Settings changes always rearrange so the effect is immediate.
        ApplySettings(hWnd, TRUE);
    }
}
