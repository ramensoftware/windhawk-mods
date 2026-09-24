// ==WindhawkMod==
// @id                  quick-hide-desktop-icons
// @name                Quick Hide Desktop Icons
// @name:zh-CN          快捷隐藏桌面图标
// @name:zh-TW          快速隱藏桌面圖標
// @name:ja-JP          デスクトップアイコンのクイック非表示
// @description         Quickly hide/show desktop icons with customizable hotkeys or mouse clicks
// @description:zh-CN   通过可自定义的热键或鼠标点击快速隐藏/显示桌面图标
// @description:zh-TW   透過可自訂的快捷鍵或滑鼠點擊快速隱藏/顯示桌面圖標
// @description:ja-JP   カスタマイズ可能なホットキーまたはマウスクリックでデスクトップアイコンを素早く非表示/表示
// @version             0.2.6
// @author              youlanan
// @github              https://github.com/youlanan
// @include             explorer.exe
// @compilerOptions     -luser32 -lshell32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quick Hide Desktop Icons 

## Introduction / 概要
- This mod allows you to temporarily hide desktop icons for a clean and clutter-free desktop experience.  
- 这个模块方便你暂时隐藏桌面图标，以获得干净整洁的桌面体验。

## Usage / 使用方法
- **Default**: Double-click the left mouse button on an empty desktop area to hide icons; repeat to show them again.  
  **默认**：在桌面空白处双击鼠标左键即可隐藏图标，再次双击恢复显示。  
- **Customization**: Configure custom hotkeys or mouse triggers in the mod settings.  
  **自定义**：您可以在模块设置中自定义热键或鼠标触发方式。  
- **Note**: Hidden state resets after a system restart.  
  **注意**：重启后隐藏状态失效。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- triggerMode: mouse
  $name: Trigger Mode
  $name:zh-CN: 触发模式
  $name:zh-TW: 觸發模式
  $name:ja-JP: トリガーモード
  $description: How to trigger the toggle (mouse/keyboard/both)
  $description:zh-CN: 如何触发切换 (鼠标/键盘/两者)
  $description:zh-TW: 如何觸發切換 (滑鼠/鍵盤/兩者)
  $description:ja-JP: トグルをトリガーする方法（マウス/キーボード/両方）
  $options:
  - mouse: Mouse only
  - keyboard: Keyboard only
  - both: Both
  $options:zh-CN:
  - mouse: 仅鼠标
  - keyboard: 仅键盘
  - both: 两者
  $options:zh-TW:
  - mouse: 僅滑鼠
  - keyboard: 僅鍵盤
  - both: 兩者
  $options:ja-JP:
  - mouse: マウスのみ
  - keyboard: キーボードのみ
  - both: 両方

- mouseButton: left
  $name: Mouse Button
  $name:zh-CN: 鼠标按键
  $name:zh-TW: 滑鼠按鍵
  $name:ja-JP: マウスボタン
  $description: Which mouse button to use
  $description:zh-CN: 使用哪个鼠标按键
  $description:zh-TW: 使用哪個滑鼠按鍵
  $description:ja-JP: 使用するマウスボタン
  $options:
  - left: Left button
  - middle: Middle button
  $options:zh-CN:
  - left: 左键
  - middle: 中键
  $options:zh-TW:
  - left: 左鍵
  - middle: 中鍵
  $options:ja-JP:
  - left: 左ボタン
  - middle: 中ボタン

- clickType: double
  $name: Click Type
  $name:zh-CN: 点击类型
  $name:zh-TW: 點擊類型
  $name:ja-JP: クリックタイプ
  $description: Single or double click
  $description:zh-CN: 单击或双击
  $description:zh-TW: 單擊或雙擊
  $description:ja-JP: シングルクリックまたはダブルクリック
  $options:
  - single: Single click
  - double: Double click
  $options:zh-CN:
  - single: 单击
  - double: 双击
  $options:zh-TW:
  - single: 單擊
  - double: 雙擊
  $options:ja-JP:
  - single: シングルクリック
  - double: ダブルクリック

- modifiers:
  - ctrl: false
  - alt: false
  - shift: false
  $name: Modifier Keys
  $name:zh-CN: 修饰键
  $name:zh-TW: 修飾鍵
  $name:ja-JP: 修飾キー
  $description: Require additional keys
  $description:zh-CN: 需要额外按键
  $description:zh-TW: 需要額外按鍵
  $description:ja-JP: 追加のキーが必要
- hotkey: F12
  $name: Hotkey
  $name:zh-CN: 热键
  $name:zh-TW: 快捷鍵
  $name:ja-JP: ホットキー
  $description: Keyboard shortcut (when keyboard mode enabled)
  $description:zh-CN: 键盘快捷键 (启用键盘模式时)
  $description:zh-TW: 鍵盤快捷鍵 (啟用鍵盤模式時)
  $description:ja-JP: キーボードショートカット（キーボードモード有効時）
- enableSound: false
  $name: Enable Sound
  $name:zh-CN: 启用声音
  $name:zh-TW: 啟用聲音
  $name:ja-JP: サウンドを有効にする
  $description: Play sound when toggling
  $description:zh-CN: 切换时播放声音
  $description:zh-TW: 切換時播放聲音
  $description:ja-JP: トグル時にサウンドを再生
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <windhawk_api.h>
#include <wchar.h>

// Debug logging
#ifndef QH_DEBUG
#define QH_DEBUG 0
#endif
#if QH_DEBUG
#define QH_LOG(...) Wh_Log(__VA_ARGS__)
#else
#define QH_LOG(...) (void)0
#endif

// Settings
struct ModSettings {
    int triggerMode; // 0=mouse, 1=keyboard, 2=both
    int mouseButton; // 0=left, 2=middle
    int clickType;   // 0=single, 1=double
    bool useCtrl;
    bool useAlt;
    bool useShift;
    int hotkey;
    bool enableSound;
} g_settings;

// System metrics cache
static int g_dblClickX = 0;
static int g_dblClickY = 0;
static DWORD g_dblClickTime = 0;

// State
static bool g_iconsHidden = false;
static HWND g_desktopListView = nullptr;
static HWND g_defView = nullptr;
static volatile LONG g_toggleLock = 0;

// Double-click detection
static DWORD g_lastClickTime = 0;
static POINT g_lastClickPos = {0, 0};

// Hook
using DispatchMessageW_t = decltype(&DispatchMessageW);
static DispatchMessageW_t DispatchMessageW_Original = nullptr;

// Shell process
static bool g_isShellProcess = false;
static DWORD g_shellPid = 0;
static HWND g_lastWorkerW = nullptr;

static DWORD GetShellProcessIdCached() {
    if (g_shellPid) return g_shellPid;
    HWND shellWnd = GetShellWindow();
    if (!shellWnd) return 0;
    GetWindowThreadProcessId(shellWnd, &g_shellPid);
    return g_shellPid;
}

// Convert string to virtual key code
int StringToVK(PCWSTR str) {
    if (!str || !*str) return VK_F12;
    wchar_t tmp[32];
    wcsncpy_s(tmp, _countof(tmp), str, _TRUNCATE);
    _wcsupr_s(tmp, _countof(tmp));

    if (tmp[0] == L'F' && wcslen(tmp) >= 2) {
        int n = _wtoi(tmp + 1);
        if (n >= 1 && n <= 24) return VK_F1 + (n - 1);
    }
    if (wcslen(tmp) == 1) {
        SHORT vk = VkKeyScanW(tmp[0]);
        if (vk != -1) return LOBYTE(vk);
    }
    if (wcscmp(tmp, L"ESC") == 0) return VK_ESCAPE;
    if (wcscmp(tmp, L"TAB") == 0) return VK_TAB;
    if (wcscmp(tmp, L"SPACE") == 0) return VK_SPACE;
    if (wcscmp(tmp, L"ENTER") == 0 || wcscmp(tmp, L"RETURN") == 0) return VK_RETURN;
    return VK_F12;
}

// Clear cached handles
static void InvalidateCachedDesktopHandles() {
    g_desktopListView = nullptr;
    g_defView = nullptr;
    g_lastWorkerW = nullptr;
}

// Find desktop DefView
HWND FindDefView() {
    if (g_defView && IsWindow(g_defView)) return g_defView;

    DWORD currentPid = GetCurrentProcessId();
    DWORD shellPid = GetShellProcessIdCached();
    HWND shellWnd = GetShellWindow();
    HWND defView = shellWnd ? FindWindowExW(shellWnd, nullptr, L"SHELLDLL_DefView", nullptr) : nullptr;

    if (defView) {
        DWORD pid;
        GetWindowThreadProcessId(defView, &pid);
        if (pid == currentPid || (shellPid && pid == shellPid)) {
            g_defView = defView;
            return g_defView;
        }
    }

    HWND worker = g_lastWorkerW && IsWindow(g_lastWorkerW) ? g_lastWorkerW : nullptr;
    DWORD start = GetTickCount();
    while ((worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr))) {
        if (GetTickCount() - start > 200) break;
        defView = FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr);
        if (defView) {
            DWORD pid;
            GetWindowThreadProcessId(defView, &pid);
            if (pid == currentPid || (shellPid && pid == shellPid)) {
                g_defView = defView;
                g_lastWorkerW = worker;
                return g_defView;
            }
        }
    }
    return nullptr;
}

// Get desktop ListView
HWND GetDesktopListView() {
    if (g_desktopListView && IsWindow(g_desktopListView)) return g_desktopListView;
    HWND defView = FindDefView();
    if (!defView) return nullptr;

    HWND list = FindWindowExW(defView, nullptr, L"SysListView32", L"FolderView");
    if (!list) list = FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
    if (list) {
        g_desktopListView = list;
        g_defView = defView;
        return g_desktopListView;
    }
    return nullptr;
}

// Check if point is on desktop background
bool IsPointOnDesktopBackground(POINT ptScreen) {
    HWND h = WindowFromPoint(ptScreen);
    if (!h) return false;

    for (HWND cur = h; cur; cur = GetParent(cur)) {
        if (cur == g_desktopListView || cur == g_defView) return true;
        wchar_t cls[64];
        if (GetClassNameW(cur, cls, _countof(cls))) {
            if (_wcsicmp(cls, L"WorkerW") == 0 || _wcsicmp(cls, L"Progman") == 0 ||
                _wcsicmp(cls, L"SHELLDLL_DefView") == 0) return true;
        }
    }

    HWND shell = GetShellWindow();
    return shell && (h == shell || IsChild(shell, h));
}

// Toggle desktop icons
void ToggleDesktopIcons() {
    if (InterlockedCompareExchange(&g_toggleLock, 1, 0)) return;
    struct AutoUnlock { ~AutoUnlock() { InterlockedExchange(&g_toggleLock, 0); } } unlock;

    HWND lv = GetDesktopListView();
    if (!lv) return;

    ShowWindow(lv, g_iconsHidden ? SW_SHOW : SW_HIDE);
    g_iconsHidden = !g_iconsHidden;
    if (g_settings.enableSound) MessageBeep(MB_OK);
    QH_LOG(L"Icons toggled: %d", g_iconsHidden);
}

// Check modifier keys
bool CheckModifiers() {
    if (g_settings.useCtrl && !(GetAsyncKeyState(VK_CONTROL) & 0x8000)) return false;
    if (g_settings.useAlt && !(GetAsyncKeyState(VK_MENU) & 0x8000)) return false;
    if (g_settings.useShift && !(GetAsyncKeyState(VK_SHIFT) & 0x8000)) return false;
    return true;
}

// Check if point is in desktop empty area
bool IsDesktopEmptyArea(HWND hWnd, POINT ptScreen) {
    if (!hWnd || !IsPointOnDesktopBackground(ptScreen)) return false;

    HWND list = GetDesktopListView();
    if (list && !g_iconsHidden) {
        POINT ptList = ptScreen;
        ScreenToClient(list, &ptList);
        LVHITTESTINFO ht = { ptList };
        return ListView_HitTest(list, &ht) == -1;
    }
    return true;
}

// Handle mouse messages
bool HandleMouseMsg(const MSG* lpMsg) {
    if (!lpMsg || g_settings.triggerMode == 1) return false;

    UINT downMsg = g_settings.mouseButton == 0 ? WM_LBUTTONDOWN : WM_MBUTTONDOWN;
    UINT dblMsg = g_settings.mouseButton == 0 ? WM_LBUTTONDBLCLK : WM_MBUTTONDBLCLK;
    if (lpMsg->message != downMsg && lpMsg->message != dblMsg) return false;
    if (!CheckModifiers()) return false;

    POINT pt = {GET_X_LPARAM(lpMsg->lParam), GET_Y_LPARAM(lpMsg->lParam)};
    POINT ptScreen = pt;
    ClientToScreen(lpMsg->hwnd, &ptScreen);
    if (!IsDesktopEmptyArea(lpMsg->hwnd, ptScreen)) return false;

    DWORD now = GetTickCount();
    bool isSystemDbl = lpMsg->message == dblMsg;
    bool isManualDbl = false;
    if (!isSystemDbl) {
        DWORD dt = now - g_lastClickTime;
        int dx = abs(pt.x - g_lastClickPos.x);
        int dy = abs(pt.y - g_lastClickPos.y);
        if (dt <= g_dblClickTime && dx <= g_dblClickX && dy <= g_dblClickY) {
            isManualDbl = true;
        }
    }

    bool isDbl = isSystemDbl || isManualDbl;
    bool should = g_settings.clickType == 0 ? !isDbl : isDbl;

    g_lastClickTime = now;
    g_lastClickPos = pt;

    if (should) {
        ToggleDesktopIcons();
        return true;
    }
    return false;
}

// Handle keyboard messages
bool HandleKeyboardMsg(const MSG* lpMsg) {
    if (!lpMsg || g_settings.triggerMode == 0) return false;
    if (lpMsg->message == WM_KEYDOWN && (int)lpMsg->wParam == g_settings.hotkey && CheckModifiers()) {
        ToggleDesktopIcons();
        return true;
    }
    return false;
}

// DispatchMessageW hook
LRESULT WINAPI DispatchMessageW_Hook(const MSG* lpMsg) {
    if (lpMsg && (lpMsg->message == WM_KEYDOWN ||
                  (lpMsg->message >= WM_MOUSEFIRST && lpMsg->message <= WM_MOUSELAST)) &&
        (HandleKeyboardMsg(lpMsg) || HandleMouseMsg(lpMsg))) {
        return 0;
    }
    return DispatchMessageW_Original(lpMsg);
}

// Load settings
void LoadSettings() {
    g_settings = {0, 0, 1, false, false, false, VK_F12, false};
    g_dblClickX = GetSystemMetrics(SM_CXDOUBLECLK) / 2;
    g_dblClickY = GetSystemMetrics(SM_CYDOUBLECLK) / 2;
    g_dblClickTime = GetDoubleClickTime();

    PCWSTR trigger = Wh_GetStringSetting(L"triggerMode");
    PCWSTR button = Wh_GetStringSetting(L"mouseButton");
    PCWSTR click = Wh_GetStringSetting(L"clickType");
    PCWSTR hotkey = Wh_GetStringSetting(L"hotkey");

    g_settings.triggerMode = trigger ? (wcscmp(trigger, L"keyboard") == 0 ? 1 : wcscmp(trigger, L"both") == 0 ? 2 : 0) : 0;
    g_settings.mouseButton = button && wcscmp(button, L"middle") == 0 ? 2 : 0;
    g_settings.clickType = click && wcscmp(click, L"single") == 0 ? 0 : 1;
    g_settings.hotkey = hotkey ? StringToVK(hotkey) : VK_F12;

    Wh_FreeStringSetting(trigger);
    Wh_FreeStringSetting(button);
    Wh_FreeStringSetting(click);
    Wh_FreeStringSetting(hotkey);

    g_settings.useCtrl = Wh_GetIntSetting(L"modifiers.ctrl") != 0;
    g_settings.useAlt = Wh_GetIntSetting(L"modifiers.alt") != 0;
    g_settings.useShift = Wh_GetIntSetting(L"modifiers.shift") != 0;
    g_settings.enableSound = Wh_GetIntSetting(L"enableSound") != 0;

    QH_LOG(L"Settings loaded");
}

// Update hook state
static void EnsureHookState() {
    DWORD currentPid = GetCurrentProcessId();
    DWORD shellPid = GetShellProcessIdCached();
    bool isShellNow = shellPid && shellPid == currentPid;

    if (isShellNow && !g_isShellProcess) {
        g_isShellProcess = true;
        if (!DispatchMessageW_Original) {
            Wh_SetFunctionHook((void*)DispatchMessageW, (void*)DispatchMessageW_Hook, (void**)&DispatchMessageW_Original);
            QH_LOG(L"Hook installed");
        }
    } else if (!isShellNow && g_isShellProcess) {
        g_isShellProcess = false;
        if (DispatchMessageW_Original) {
            #ifdef Wh_RemoveFunctionHook
            Wh_RemoveFunctionHook((void*)DispatchMessageW, (void*)DispatchMessageW_Hook);
            #endif
            DispatchMessageW_Original = nullptr;
            QH_LOG(L"Hook removed");
        }
    }
}

// Initialize
BOOL Wh_ModInit() {
    LoadSettings();
    g_shellPid = 0;
    EnsureHookState();
    QH_LOG(L"Initialized");
    return TRUE;
}

// Uninitialize
void Wh_ModUninit() {
    if (g_iconsHidden && g_desktopListView && IsWindow(g_desktopListView)) {
        ShowWindow(g_desktopListView, SW_SHOW);
        g_iconsHidden = false;
        QH_LOG(L"Icons restored");
    }
    if (DispatchMessageW_Original) {
        #ifdef Wh_RemoveFunctionHook
        Wh_RemoveFunctionHook((void*)DispatchMessageW, (void*)DispatchMessageW_Hook);
        #endif
        DispatchMessageW_Original = nullptr;
        QH_LOG(L"Hook removed");
    }
    InvalidateCachedDesktopHandles();
    QH_LOG(L"Uninitialized");
}

// Settings changed
void Wh_ModSettingsChanged() {
    LoadSettings();
    g_shellPid = 0;
    EnsureHookState();
    QH_LOG(L"Settings updated");
}
