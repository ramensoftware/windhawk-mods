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
// @version             0.2.4
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
  - right: Right button
  - middle: Middle button
  $options:zh-CN:
  - left: 左键
  - right: 右键
  - middle: 中键
  $options:zh-TW:
  - left: 左鍵
  - right: 右鍵
  - middle: 中鍵
  $options:ja-JP:
  - left: 左ボタン
  - right: 右ボタン
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

// Settings structure
struct ModSettings {
    int triggerMode; // 0=mouse, 1=keyboard, 2=both
    int mouseButton; // 0=left, 1=right, 2=middle
    int clickType;   // 0=single, 1=double
    bool useCtrl;
    bool useAlt;
    bool useShift;
    int hotkey;      // Virtual key code
    bool enableSound;
} g_settings;

// State variables
static bool g_iconsHidden = false;
static HWND g_desktopListView = nullptr;
static DWORD g_lastClickTime = 0;
static POINT g_lastClickPos = {0, 0};

// Find desktop icons window
HWND FindDesktopIconsWindow() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (progman) {
        HWND defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
        if (defView) {
            HWND listView = FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
            if (listView) {
                Wh_Log(L"Found desktop icons window via Progman: %p", listView);
                return listView;
            }
        }
    }

    HWND workerW = nullptr;
    while ((workerW = FindWindowExW(nullptr, workerW, L"WorkerW", nullptr))) {
        HWND defView = FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr);
        if (defView) {
            HWND listView = FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
            if (listView) {
                Wh_Log(L"Found desktop icons window via WorkerW: %p", listView);
                return listView;
            }
        }
    }

    Wh_Log(L"Failed to find desktop icons window");
    return nullptr;
}

// Toggle desktop icons visibility
void ToggleDesktopIcons() {
    // Always attempt to find desktop window if not set
    if (!g_desktopListView) {
        g_desktopListView = FindDesktopIconsWindow();
    }

    if (g_desktopListView) {
        ShowWindow(g_desktopListView, g_iconsHidden ? SW_SHOW : SW_HIDE);
        g_iconsHidden = !g_iconsHidden;
        Wh_Log(L"Desktop icons %s", g_iconsHidden ? L"hidden" : L"shown");

        if (g_settings.enableSound) {
            MessageBeep(MB_OK);
        }
    } else {
        Wh_Log(L"Desktop icons window not found");
    }
}

// Check if modifier keys match requirements
bool CheckModifiers() {
    bool result = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0) == g_settings.useCtrl &&
                  ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0) == g_settings.useAlt &&
                  ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0) == g_settings.useShift;
    Wh_Log(L"Modifiers check: ctrl=%d, alt=%d, shift=%d, result=%d",
           g_settings.useCtrl, g_settings.useAlt, g_settings.useShift, result);
    return result;
}

// Check if point is in desktop empty area
bool IsDesktopEmptyArea(HWND hWnd, POINT pt) {
    if (!g_desktopListView) {
        g_desktopListView = FindDesktopIconsWindow();
        if (!g_desktopListView) return false;
    }

    HWND defView = GetParent(g_desktopListView);
    if (!defView) {
        Wh_Log(L"Failed to get parent defView");
        return false;
    }

    ClientToScreen(hWnd, &pt);
    ScreenToClient(defView, &pt);

    if (hWnd == g_desktopListView && !g_iconsHidden) {
        LVHITTESTINFO ht = {pt};
        bool isEmpty = ListView_HitTest(g_desktopListView, &ht) == -1;
        Wh_Log(L"Hit test on ListView: isEmpty=%d, x=%ld, y=%ld", isEmpty, pt.x, pt.y);
        return isEmpty;
    } else if (hWnd == defView || hWnd == g_desktopListView) {
        Wh_Log(L"Point in defView or ListView: x=%ld, y=%ld", pt.x, pt.y);
        return true;
    }

    Wh_Log(L"Not in desktop empty area: hwnd=%p", hWnd);
    return false;
}

// Handle mouse click
bool HandleMouseClick(const MSG* lpMsg) {
    if (g_settings.triggerMode == 1) return false; // Keyboard only

    UINT targetDown, targetDbl;
    switch (g_settings.mouseButton) {
        case 0: // Left
            targetDown = WM_LBUTTONDOWN;
            targetDbl = WM_LBUTTONDBLCLK;
            break;
        case 1: // Right
            targetDown = WM_RBUTTONDOWN;
            targetDbl = WM_RBUTTONDBLCLK;
            break;
        case 2: // Middle
            targetDown = WM_MBUTTONDOWN;
            targetDbl = WM_MBUTTONDBLCLK;
            break;
        default:
            Wh_Log(L"Invalid mouseButton: %d", g_settings.mouseButton);
            return false;
    }

    if (lpMsg->message != targetDown && lpMsg->message != targetDbl) {
        Wh_Log(L"Ignored message: %u", lpMsg->message);
        return false;
    }

    if (!CheckModifiers()) {
        Wh_Log(L"Modifiers check failed: ctrl=%d, alt=%d, shift=%d",
               g_settings.useCtrl, g_settings.useAlt, g_settings.useShift);
        return false;
    }

    POINT pt = {GET_X_LPARAM(lpMsg->lParam), GET_Y_LPARAM(lpMsg->lParam)};
    if (!IsDesktopEmptyArea(lpMsg->hwnd, pt)) {
        Wh_Log(L"Not in desktop empty area: hwnd=%p, x=%ld, y=%ld", lpMsg->hwnd, pt.x, pt.y);
        return false;
    }

    POINT ptScreen = pt;
    ClientToScreen(lpMsg->hwnd, &ptScreen);

    DWORD currentTime = GetTickCount();
    int cxDbl = GetSystemMetrics(SM_CXDOUBLECLK) / 2;
    int cyDbl = GetSystemMetrics(SM_CYDOUBLECLK) / 2;
    bool isWithinDouble = (currentTime - g_lastClickTime <= GetDoubleClickTime()) &&
                          (abs(ptScreen.x - g_lastClickPos.x) <= cxDbl) &&
                          (abs(ptScreen.y - g_lastClickPos.y) <= cyDbl);
    bool isDblMsg = (lpMsg->message == targetDbl);

    Wh_Log(L"Mouse event: msg=%u, isDblMsg=%d, isWithinDouble=%d, time=%u, x=%ld, y=%ld",
           lpMsg->message, isDblMsg, isWithinDouble, currentTime, ptScreen.x, ptScreen.y);

    bool shouldToggle = false;
    if (g_settings.clickType == 0) { // Single
        shouldToggle = !isDblMsg && !isWithinDouble;
    } else { // Double
        // For right button, only handle single clicks to avoid context menu
        if (g_settings.mouseButton == 1) {
            Wh_Log(L"Skipping right button double-click to preserve context menu");
            return false;
        }
        shouldToggle = isDblMsg || isWithinDouble;
    }

    g_lastClickTime = currentTime;
    g_lastClickPos = ptScreen;

    if (shouldToggle) {
        Wh_Log(L"Triggering toggle with %s %s click",
               (g_settings.mouseButton == 0 ? L"left" : g_settings.mouseButton == 1 ? L"right" : L"middle"),
               g_settings.clickType == 0 ? L"single" : L"double");
        ToggleDesktopIcons();
        return true;
    }

    return false;
}

// Handle keyboard input
bool HandleKeyboard(const MSG* lpMsg) {
    if (g_settings.triggerMode == 0) return false; // Mouse only

    if (lpMsg->message == WM_KEYDOWN &&
        lpMsg->wParam == g_settings.hotkey &&
        CheckModifiers()) {
        Wh_Log(L"Keyboard trigger: key=%d", lpMsg->wParam);
        ToggleDesktopIcons();
        return true;
    }

    return false;
}

// Hook function for DispatchMessageW
using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t DispatchMessageW_Original;
LRESULT WINAPI DispatchMessageW_Hook(const MSG* lpMsg) {
    if (lpMsg && (HandleKeyboard(lpMsg) || HandleMouseClick(lpMsg))) {
        return 0; // Message handled
    }
    return DispatchMessageW_Original(lpMsg);
}

// Load settings from Windhawk
void LoadSettings() {
    // Initialize with default values
    g_settings = {0, 0, 1, false, false, false, VK_F12, false};

    // Buffer for string settings
    WCHAR buffer[64];

    // Load triggerMode
    PCWSTR triggerMode = Wh_GetStringSetting(L"triggerMode");
    if (wcscmp(triggerMode, L"keyboard") == 0) g_settings.triggerMode = 1;
    else if (wcscmp(triggerMode, L"both") == 0) g_settings.triggerMode = 2;
    else g_settings.triggerMode = 0; // Default to mouse
    Wh_FreeStringSetting(triggerMode);

    // Load mouseButton
    PCWSTR mouseButton = Wh_GetStringSetting(L"mouseButton");
    if (wcscmp(mouseButton, L"right") == 0) g_settings.mouseButton = 1;
    else if (wcscmp(mouseButton, L"middle") == 0) g_settings.mouseButton = 2;
    else g_settings.mouseButton = 0; // Default to left
    Wh_FreeStringSetting(mouseButton);

    // Load clickType
    PCWSTR clickType = Wh_GetStringSetting(L"clickType");
    g_settings.clickType = (wcscmp(clickType, L"single") == 0) ? 0 : 1; // Default to double
    Wh_FreeStringSetting(clickType);

    // Load modifiers
    g_settings.useCtrl = Wh_GetIntSetting(L"modifiers.ctrl");
    g_settings.useAlt = Wh_GetIntSetting(L"modifiers.alt");
    g_settings.useShift = Wh_GetIntSetting(L"modifiers.shift");

    // Load hotkey
    PCWSTR hotkey = Wh_GetStringSetting(L"hotkey");
    if (wcslen(hotkey) > 0) {
        if (wcscmp(hotkey, L"F1") == 0) g_settings.hotkey = VK_F1;
        else if (wcscmp(hotkey, L"F2") == 0) g_settings.hotkey = VK_F2;
        else if (wcscmp(hotkey, L"F3") == 0) g_settings.hotkey = VK_F3;
        else if (wcscmp(hotkey, L"F4") == 0) g_settings.hotkey = VK_F4;
        else if (wcscmp(hotkey, L"F5") == 0) g_settings.hotkey = VK_F5;
        else if (wcscmp(hotkey, L"F6") == 0) g_settings.hotkey = VK_F6;
        else if (wcscmp(hotkey, L"F7") == 0) g_settings.hotkey = VK_F7;
        else if (wcscmp(hotkey, L"F8") == 0) g_settings.hotkey = VK_F8;
        else if (wcscmp(hotkey, L"F9") == 0) g_settings.hotkey = VK_F9;
        else if (wcscmp(hotkey, L"F10") == 0) g_settings.hotkey = VK_F10;
        else if (wcscmp(hotkey, L"F11") == 0) g_settings.hotkey = VK_F11;
        else if (wcscmp(hotkey, L"F12") == 0) g_settings.hotkey = VK_F12;
        else g_settings.hotkey = (int)hotkey[0]; // Fallback to first char
    }
    Wh_FreeStringSetting(hotkey);

    // Load enableSound
    g_settings.enableSound = Wh_GetIntSetting(L"enableSound");

    Wh_Log(L"Settings loaded: triggerMode=%d, mouseButton=%d, clickType=%d, hotkey=%d, ctrl=%d, alt=%d, shift=%d, sound=%d",
           g_settings.triggerMode, g_settings.mouseButton, g_settings.clickType, g_settings.hotkey,
           g_settings.useCtrl, g_settings.useAlt, g_settings.useShift, g_settings.enableSound);
}

// Initialize mod
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");

    LoadSettings();

    Wh_SetFunctionHook((void*)DispatchMessageW, (void*)DispatchMessageW_Hook, (void**)&DispatchMessageW_Original);

    // Initialize g_desktopListView lazily in ToggleDesktopIcons
    Wh_Log(L"Deferring desktop window search to first toggle");

    Wh_Log(L"Initialized");
    return TRUE;
}

// Uninitialize mod
void Wh_ModUninit() {
    Wh_Log(L"Uninitializing");

    if (g_iconsHidden && g_desktopListView) {
        ShowWindow(g_desktopListView, SW_SHOW);
        g_iconsHidden = false;
        Wh_Log(L"Restored desktop icons");
    }

    g_desktopListView = nullptr;
    Wh_Log(L"Uninitialized");
}

// Settings changed
void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
    Wh_Log(L"Settings reloaded");
}
