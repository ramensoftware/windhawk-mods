// ==WindhawkMod==
// @id              force-desktop-icon-spacing
// @name            Force Desktop Icon Spacing (Ultimate)
// @name:zh-CN      强制桌面图标间距（终极版）
// @description     Safely disable desktop auto-arrange and precisely control the icon grid spacing (X/Y) and the bottom margin reserved above the taskbar. Every change applies in one absolute pass: no cumulative drift, no window shifting, no blank strip at the top.
// @description:zh-CN  安全禁用桌面自动排列，精准控制桌面图标的水平/垂直网格间距，以及在任务栏上方预留的底部空白边距。所有调整一次性绝对到位：不叠加累积、不整体移动图标区域、顶部不产生空白。
// @version         1.0.1
// @author          Koewen
// @github          https://github.com/Koewen
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lshell32 -lcomctl32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Force desktop icon spacing precisely: safely disable auto-arrange, control the horizontal/vertical icon grid spacing, and reserve a blank margin at the bottom of the icon area (above the taskbar).

## How it works

- Disables auto-arrange and snap-to-grid via the desktop bag registry (FFlags), without touching any other view settings.
- Sets the icon grid size via `LVM_SETICONSPACING` with absolute values: adjusting a value always produces the same layout, never cumulative drift.
- The vertical grid is anchored at the top: increasing the vertical spacing stretches the grid downward, so the extra space is absorbed by the bottom region. No blank strip is created at the top, and the icon area is never shifted as a whole.
- The bottom margin shrinks the icon area height only (the top edge stays fixed), so icon columns wrap earlier and never enter the reserved bottom strip.
- After applying spacing/margin changes, the icons are rearranged once (the same native mechanism as the desktop "Arrange icons" command) so the new layout takes effect immediately.

## Settings

- **Horizontal spacing (pixels)**: width of each icon grid cell. Larger = more horizontal gap between icons.
- **Vertical spacing (pixels)**: height of each icon grid cell. Larger = taller rows; the grid grows downward and the extra space is absorbed by the bottom region.
- **Bottom margin (pixels)**: blank strip reserved at the bottom of the icon area, above the taskbar. The icon grid never enters it. 0 = no reservation. At least 200 px of usable height is always kept.

## Notes

- Icons are rearranged when the mod loads and when settings are saved. Icons you drag afterwards are left alone by the background watchdog (which only re-asserts the spacing and margin values).
- Disabling or unloading the mod restores the original icon area geometry.

## 简体中文说明

精确控制桌面图标间距：安全禁用自动排列，自由调节图标网格的水平/垂直间距，并在图标区域底部（任务栏上方）预留空白边距。

### 工作原理

- 通过桌面注册表（FFlags）仅关闭自动排列与对齐网格，不改动其他视图设置。
- 使用 `LVM_SETICONSPACING` 以绝对值设置网格尺寸——同一数值永远得到同一布局，不存在多次应用叠加漂移。
- 垂直网格顶端锚定：调大垂直间距时行距拉大、网格向下延伸，多余空间挤占底部区域——顶部不会产生空白，图标区域也不会被整体移动。
- 底部边距只收缩图标区域高度（顶边固定），图标换列提前，永不进入底部预留区。
- 间距/边距改动后会触发一次图标重排（与桌面右键"排列图标"同源的原生机制），新布局立即生效。

### 设置项

- **水平间距（像素）**：图标网格单元宽度，数值越大图标水平间距越大。
- **垂直间距（像素）**：图标网格单元高度，数值越大行距越大，网格向下延伸，多余空间挤占底部区域。
- **底部边距（像素）**：图标区域底部（任务栏上方）预留的空白高度，图标网格不会进入该区域。0 = 不预留；始终保留至少 200 像素可用高度。

### 注意

- 模组加载和设置保存时会重排一次图标；之后手动拖放的图标不会被后台守护强行归位（守护轮只回写间距/边距的绝对值）。
- 停用或卸载模组会自动恢复图标区域原始几何。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- IconSpacingX: 95
  $name: Horizontal spacing (pixels)
  $name:zh-CN: 水平间距（X 像素）
  $description: Width of each icon grid cell. Larger = more horizontal gap between icons.
  $description:zh-CN: 图标网格单元宽度，数值越大图标水平间距越大。
- IconSpacingY: 100
  $name: Vertical spacing (pixels)
  $name:zh-CN: 垂直间距（Y 像素）
  $description: Height of each icon grid cell. Larger = taller rows; the grid grows downward and the extra space is absorbed by the bottom region.
  $description:zh-CN: 图标网格单元高度，数值越大行距越大，网格向下延伸，多余空间挤占底部区域。
- BottomMargin: 0
  $name: Bottom margin (pixels)
  $name:zh-CN: 底部边距（像素）
  $description: Blank strip reserved at the bottom of the icon area, above the taskbar. The icon grid never enters it. 0 = no reservation.
  $description:zh-CN: 在图标区域底部（任务栏上方）预留的空白高度，图标网格不会进入该区域。0 = 不预留。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>

static int g_spacingX = 95;
static int g_spacingY = 100;
static int g_bottomMargin = 0;
static HWND g_currentLV = NULL;
static HANDLE g_hThread = NULL;
static BOOL g_bStop = FALSE;

#ifndef LVM_SETICONSPACING
#define LVM_SETICONSPACING (LVM_FIRST + 53)
#endif
#ifndef LVM_GETITEMSPACING
#define LVM_GETITEMSPACING (LVM_FIRST + 51)
#endif
#ifndef LVM_ARRANGE
#define LVM_ARRANGE (LVM_FIRST + 22)
#endif
#ifndef LVA_DEFAULT
#define LVA_DEFAULT 0
#endif

// Find the desktop SysListView32 handle (Progman or WorkerW hosted).
static HWND FindDesktopListView()
{
    HWND progman = FindWindow(L"Progman", NULL);
    if (progman) {
        HWND defView = FindWindowEx(progman, NULL, L"SHELLDLL_DefView", NULL);
        if (defView) {
            HWND lv = FindWindowEx(defView, NULL, L"SysListView32", NULL);
            if (lv) return lv;
        }
    }
    HWND worker = NULL;
    while ((worker = FindWindowEx(NULL, worker, L"WorkerW", NULL)) != NULL) {
        HWND defView = FindWindowEx(worker, NULL, L"SHELLDLL_DefView", NULL);
        if (defView) {
            HWND lv = FindWindowEx(defView, NULL, L"SysListView32", NULL);
            if (lv) return lv;
        }
    }
    return NULL;
}

// Safely disable auto-arrange and snap-to-grid only, without touching any
// other desktop view settings.
static void SafeDisableAutoArrange()
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Shell\\Bags\\1\\Desktop", 0, KEY_READ | KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        DWORD fflags = 0;
        DWORD cbData = sizeof(fflags);

        if (RegQueryValueExW(hKey, L"FFlags", NULL, NULL, (LPBYTE)&fflags, &cbData) != ERROR_SUCCESS) {
            fflags = 0x40200224; // A common default base value if read fails.
        }

        // Clear FWF_AUTOARRANGE (0x00000001) and FWF_SNAPTOGRID (0x00000002) only.
        fflags &= ~0x00000001;
        fflags &= ~0x00000002;

        RegSetValueExW(hKey, L"FFlags", 0, REG_DWORD, (const BYTE*)&fflags, sizeof(fflags));
        RegCloseKey(hKey);
    }
}

// Bottom margin: stateless, absolute, one-shot.
//  - Re-evaluated from the parent window's (SHELLDLL_DefView) current client
//    size every time;
//  - The top edge is always pinned at (0,0) of the parent - the window is
//    never moved, so no blank strip can ever appear at the top;
//  - Only the height shrinks: icon columns wrap earlier and never enter the
//    reserved bottom strip;
//  - No history is recorded or relied upon, so repeated applies or repeated
//    setting changes never accumulate (no 1+1 drift).
static void ApplyBottomMargin(HWND hwndLV)
{
    HWND parent = GetParent(hwndLV);
    RECT rc;
    if (!parent || !GetClientRect(parent, &rc)) return;

    int availH = rc.bottom - g_bottomMargin;
    if (availH > rc.bottom) availH = rc.bottom;  // Guard against negative values.
    if (availH < 200) availH = 200;              // Keep at least 200 px of usable height.

    SetWindowPos(hwndLV, NULL, 0, 0, rc.right, availH, SWP_NOZORDER | SWP_NOACTIVATE);
}

// Set the icon grid spacing (absolute values) and verify by reading it back:
//  - The window geometry (bottom margin) must be applied BEFORE this call.
//    Resizing the window makes the list view recompute its grid from the
//    system WindowMetrics, which would otherwise overwrite the custom
//    spacing - the reason vertical spacing did not stick in earlier builds;
//  - After setting, read back with LVM_GETITEMSPACING; if the value was
//    reset, send it once more;
//  - Returns TRUE when the spacing had been reset and the resend restored
//    it (a rearrangement is needed so icons follow the new spacing);
//    returns FALSE when the spacing was fine all along, or the system
//    clamped it to its minimum (log only, no rearrange).
static BOOL SetIconSpacing(HWND hwndLV)
{
    BOOL needRearrange = FALSE;

    DWORD_PTR spacingParam = MAKELONG(g_spacingX, g_spacingY);
    SendMessageTimeout(hwndLV, LVM_SETICONSPACING, 0, spacingParam, SMTO_ABORTIFHUNG | SMTO_NORMAL, 1000, NULL);

    DWORD_PTR result = 0;
    if (SendMessageTimeout(hwndLV, LVM_GETITEMSPACING, FALSE, 0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 1000, &result) && result) {
        int cx = LOWORD(result);
        int cy = HIWORD(result);
        if (abs(cx - g_spacingX) > 1 || abs(cy - g_spacingY) > 1) {
            // Spacing got reset by the system: send it once more.
            SendMessageTimeout(hwndLV, LVM_SETICONSPACING, 0, spacingParam, SMTO_ABORTIFHUNG | SMTO_NORMAL, 1000, NULL);
            result = 0;
            if (SendMessageTimeout(hwndLV, LVM_GETITEMSPACING, FALSE, 0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 1000, &result) && result) {
                cx = LOWORD(result);
                cy = HIWORD(result);
                if (abs(cx - g_spacingX) <= 1 && abs(cy - g_spacingY) <= 1) {
                    needRearrange = TRUE;   // Restored; rearrange so icons follow.
                } else {
                    // Clamped by the system to the icon's minimum size
                    // (the configured value is too small); log for diagnosis.
                    Wh_Log(L"Icon spacing clamped by system: set (%d,%d), got (%d,%d)",
                           g_spacingX, g_spacingY, cx, cy);
                }
            }
        }
    }
    return needRearrange;
}

static void ApplyAllSettings(BOOL bRearrange)
{
    // 1. Safely update the registry.
    SafeDisableAutoArrange();

    HWND hwndLV = FindDesktopListView();
    if (!hwndLV) return;

    g_currentLV = hwndLV;

    // 2. Bottom margin must come before the spacing: resizing the window
    //    makes the list view recompute its grid from the system defaults,
    //    so the fixed order is: geometry -> spacing -> rearrange.
    ApplyBottomMargin(hwndLV);

    // 3. Icon grid spacing (absolute, idempotent, verified by read-back).
    //    The vertical spacing is the grid cell height (icon + label): a
    //    larger IconSpacingY stretches the rows downward and the extra
    //    space is absorbed by the bottom region, with no blank strip at
    //    the top.
    BOOL spacingRestored = SetIconSpacing(hwndLV);

    // 4. Rearrange: always on load/settings change; in watchdog ticks only
    //    when the spacing had been reset and was restored, so hand-dragged
    //    icons are left alone otherwise.
    if (bRearrange || spacingRestored) {
        SendMessageTimeout(hwndLV, LVM_ARRANGE, LVA_DEFAULT, 0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 1000, NULL);
    }

    InvalidateRect(hwndLV, NULL, TRUE);
    UpdateWindow(hwndLV);
}

static DWORD WINAPI BackgroundWorker(LPVOID lpParam)
{
    // Wait 2 seconds for Explorer to fully load; rearrange once on load so
    // the spacing/margin take effect immediately.
    Sleep(2000);
    ApplyAllSettings(TRUE);

    while (!g_bStop) {
        // Re-assert every 10 seconds in case the system redraws or resets
        // values. The watchdog only writes back the absolute spacing/margin
        // values and does not rearrange, so icons dragged by hand are left
        // alone.
        Sleep(10000);
        if (g_bStop) break;
        ApplyAllSettings(FALSE);
    }
    return 0;
}

BOOL Wh_ModInit()
{
    g_spacingX = Wh_GetIntSetting(L"IconSpacingX");
    g_spacingY = Wh_GetIntSetting(L"IconSpacingY");
    g_bottomMargin = Wh_GetIntSetting(L"BottomMargin");

    // Sanity clamps.
    if (g_spacingX < 40) g_spacingX = 40;
    if (g_spacingY < 35) g_spacingY = 35;
    if (g_bottomMargin < 0) g_bottomMargin = 0;

    g_bStop = FALSE;
    g_hThread = CreateThread(NULL, 0, BackgroundWorker, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModSettingsChanged()
{
    g_spacingX = Wh_GetIntSetting(L"IconSpacingX");
    g_spacingY = Wh_GetIntSetting(L"IconSpacingY");
    g_bottomMargin = Wh_GetIntSetting(L"BottomMargin");

    if (g_spacingX < 40) g_spacingX = 40;
    if (g_spacingY < 35) g_spacingY = 35;
    if (g_bottomMargin < 0) g_bottomMargin = 0;

    // Apply immediately and rearrange so the effect is visible right away.
    ApplyAllSettings(TRUE);
}

void Wh_ModUninit()
{
    g_bStop = TRUE;
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }
    // On unload: stateless restore - set the icon area back to the full
    // parent size, without relying on any recorded baseline.
    if (g_currentLV && IsWindow(g_currentLV)) {
        HWND parent = GetParent(g_currentLV);
        RECT rc;
        if (parent && GetClientRect(parent, &rc)) {
            SetWindowPos(g_currentLV, NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        InvalidateRect(g_currentLV, NULL, TRUE);
    }
}
