// ==WindhawkMod==
// @id              minimize-to-tray
// @name            Minimize to tray
// @description     Right-click a window's minimize button to hide it in the system tray.
// @version         1.0
// @author          0Allu
// @github          https://github.com/0Allu
// @homepage        https://github.com/0Allu/minimize-to-tray
// @include         windhawk.exe
// @compilerOptions -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Minimize to tray

Right-click a window's minimize button to hide the window in the system tray.

![Minimize to tray demo](https://raw.githubusercontent.com/0Allu/minimize-to-tray/main/assets/minimize-to-tray-demo.gif)

## Usage

- Right-click a window's minimize button to send it to the tray.
- Left-click its tray icon to restore it.
- Right-click its tray icon to restore or close the window.

The application continues running while its window is hidden.

## Custom title bars

Standard Windows minimize buttons work automatically.

Applications which draw their own title bar can be enabled with the **Custom
title bar rules** setting. Ready-made rules for Discord and Steam are included,
but custom title bar support is disabled by default to avoid intercepting
right-clicks in application toolbars or other client-area controls.

A custom rule uses this format:

`executable|window class|height|min right|max right`

The three numeric values are in DPI-independent pixels. `height` is the maximum
distance below the top of the window, and `min right` / `max right` define the
button's distance from the right edge.

## Compatibility

Applications running with administrator privileges are not supported when the
mod is running at normal user privileges. This is due to Windows User Interface
Privilege Isolation (UIPI).

## Safety

A tray icon is created before a window is hidden. If the tray icon cannot be
created, the window is left visible. If the mod is disabled or unloaded
normally, restore requests are posted for all windows hidden by the mod. Hidden
windows are also recovered if the dedicated mod process restarts.

If the dedicated tool process is forcibly terminated and the mod is then
disabled before it can restart, recovery code cannot run. In that unusual case,
the affected application may need to be restarted manually.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableCustomTitleBarRules: false
  $name: Enable custom title bar rules
  $description: >-
    Enables geometry-based minimize detection only for applications listed in
    Custom title bar rules. Leave disabled for the safest behavior.

- customTitleBarRules:
  - Discord.exe|*|48|90|150
  - steam.exe|SDL_app|48|68|115
  - steamwebhelper.exe|SDL_app|48|68|115
  $name: Custom title bar rules
  $description: >-
    One rule per application in the format executable|window class|height|min
    right|max right. Numeric values are DPI-independent pixels measured from
    the top and right edges of the window. Use * as the window class to match
    any class for the executable.
*/
// ==/WindhawkModSettings==

#include <shellapi.h>
#include <windows.h>

#include <algorithm>
#include <cstdio>
#include <cwchar>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr UINT WM_TRAY_ICON = WM_APP + 100;
constexpr UINT WM_HIDE_WINDOW = WM_APP + 101;
constexpr UINT WM_RELOAD_SETTINGS = WM_APP + 102;

constexpr UINT_PTR TIMER_CLEANUP = 1;

constexpr UINT MENU_RESTORE = 1;
constexpr UINT MENU_CLOSE = 2;

constexpr DWORD HIT_TEST_TIMEOUT_MS = 10;
constexpr DWORD WORKER_SHUTDOWN_TIMEOUT_MS = 5000;

constexpr wchar_t HIDDEN_WINDOW_PROPERTY[] =
    L"Windhawk.MinimizeToTray.HiddenWindow.v1";
constexpr wchar_t CONTROLLER_CLASS[] = L"WindhawkMinimizeToTrayController";

struct TrayItem {
    HWND hwnd = nullptr;
    DWORD processId = 0;
    UINT id = 0;
    HICON icon = nullptr;
    bool ownsIcon = false;
    bool iconAdded = false;
    std::wstring title;
};

struct CustomTitleBarRule {
    std::wstring executable;
    std::wstring windowClass;
    int heightDip = 0;
    int minRightDip = 0;
    int maxRightDip = 0;
};

struct CachedWindowIdentity {
    HWND hwnd = nullptr;
    DWORD processId = 0;
    std::wstring executable;
    std::wstring windowClass;
};

HWND g_controllerWindow = nullptr;
HHOOK g_mouseHook = nullptr;

HANDLE g_workerThread = nullptr;
HANDLE g_workerStartedEvent = nullptr;

bool g_workerStartedSuccessfully = false;

UINT g_taskbarCreatedMessage = 0;
UINT g_nextTrayId = 1;

HWND g_rightClickWindow = nullptr;
DWORD g_rightClickProcessId = 0;
POINT g_rightClickPoint = {};

bool g_enableCustomTitleBarRules = false;
int g_maxCustomRuleHeightDip = 0;
int g_maxCustomRuleRightDip = 0;
std::vector<CustomTitleBarRule> g_customTitleBarRules;
std::vector<CachedWindowIdentity> g_windowIdentityCache;
std::vector<TrayItem> g_trayItems;

DWORD GetWindowProcessId(HWND hwnd) {
    DWORD processId = 0;
    if (hwnd) {
        GetWindowThreadProcessId(hwnd, &processId);
    }
    return processId;
}

UINT GetWindowDpiSafe(HWND hwnd) {
    UINT dpi = GetDpiForWindow(hwnd);
    return dpi ? dpi : 96;
}

bool IsTrayItemWindowValid(const TrayItem& item) {
    return IsWindow(item.hwnd) &&
           GetWindowProcessId(item.hwnd) == item.processId;
}

TrayItem* FindTrayItemByWindow(HWND hwnd) {
    DWORD processId = GetWindowProcessId(hwnd);

    for (auto& item : g_trayItems) {
        if (item.hwnd == hwnd && item.processId == processId) {
            return &item;
        }
    }

    return nullptr;
}

TrayItem* FindTrayItemById(UINT id) {
    for (auto& item : g_trayItems) {
        if (item.id == id) {
            return &item;
        }
    }

    return nullptr;
}

std::wstring GetWindowTitleSafe(HWND hwnd) {
    wchar_t title[256] = {};
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));

    if (!title[0]) {
        return L"Minimized window";
    }

    return title;
}

HICON CopyWindowIcon(HWND hwnd, bool* ownsIcon) {
    if (ownsIcon) {
        *ownsIcon = false;
    }

    const WPARAM iconTypes[] = {ICON_SMALL2, ICON_SMALL, ICON_BIG};

    for (WPARAM iconType : iconTypes) {
        DWORD_PTR result = 0;

        if (SendMessageTimeoutW(hwnd, WM_GETICON, iconType, 0,
                                SMTO_ABORTIFHUNG, 50, &result) &&
            result) {
            HICON copiedIcon = CopyIcon(reinterpret_cast<HICON>(result));
            if (copiedIcon) {
                if (ownsIcon) {
                    *ownsIcon = true;
                }
                return copiedIcon;
            }
        }
    }

    HICON classIcon =
        reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICONSM));
    if (!classIcon) {
        classIcon =
            reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICON));
    }

    if (classIcon) {
        HICON copiedIcon = CopyIcon(classIcon);
        if (copiedIcon) {
            if (ownsIcon) {
                *ownsIcon = true;
            }
            return copiedIcon;
        }
    }

    HICON fallback = LoadIconW(nullptr, IDI_APPLICATION);
    if (fallback) {
        HICON copiedIcon = CopyIcon(fallback);
        if (copiedIcon) {
            if (ownsIcon) {
                *ownsIcon = true;
            }
            return copiedIcon;
        }
    }

    return fallback;
}

bool IsExcludedWindow(HWND hwnd) {
    if (!hwnd || hwnd == g_controllerWindow || hwnd == GetDesktopWindow()) {
        return true;
    }

    wchar_t className[128] = {};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));

    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
           wcscmp(className, L"Progman") == 0 ||
           wcscmp(className, L"WorkerW") == 0;
}

bool HasMinimizeFrame(HWND hwnd) {
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if ((style & WS_CHILD) || (exStyle & WS_EX_NOACTIVATE)) {
        return false;
    }

    return (style & WS_MINIMIZEBOX) != 0 &&
           (style & WS_CAPTION) == WS_CAPTION;
}

bool GetProcessImageName(HWND hwnd, std::wstring* fileName) {
    DWORD processId = GetWindowProcessId(hwnd);
    if (!processId || !fileName) {
        return false;
    }

    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return false;
    }

    wchar_t imagePath[1024] = {};
    DWORD imagePathLength = ARRAYSIZE(imagePath);
    bool success = false;

    if (QueryFullProcessImageNameW(process, 0, imagePath, &imagePathLength)) {
        const wchar_t* imageFileName = wcsrchr(imagePath, L'\\');
        imageFileName = imageFileName ? imageFileName + 1 : imagePath;
        *fileName = imageFileName;
        success = true;
    }

    CloseHandle(process);
    return success;
}

CachedWindowIdentity* FindCachedWindowIdentity(HWND hwnd) {
    DWORD processId = GetWindowProcessId(hwnd);
    if (!processId) {
        return nullptr;
    }

    for (auto& entry : g_windowIdentityCache) {
        if (entry.hwnd == hwnd && entry.processId == processId) {
            return &entry;
        }
    }

    return nullptr;
}

BOOL CALLBACK CacheWindowIdentityProc(HWND hwnd, LPARAM) {
    if (!IsWindowVisible(hwnd)) {
        return TRUE;
    }

    DWORD processId = GetWindowProcessId(hwnd);
    if (!processId) {
        return TRUE;
    }

    wchar_t className[128] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    std::wstring executable;

    // Reuse a process image name already obtained for another top-level
    // window owned by the same process.
    for (const auto& entry : g_windowIdentityCache) {
        if (entry.processId == processId && !entry.executable.empty()) {
            executable = entry.executable;
            break;
        }
    }

    if (executable.empty() && !GetProcessImageName(hwnd, &executable)) {
        return TRUE;
    }

    CachedWindowIdentity entry;
    entry.hwnd = hwnd;
    entry.processId = processId;
    entry.executable = std::move(executable);
    entry.windowClass = className;
    g_windowIdentityCache.push_back(std::move(entry));
    return TRUE;
}

void RefreshWindowIdentityCache() {
    g_windowIdentityCache.clear();

    if (g_enableCustomTitleBarRules) {
        EnumWindows(CacheWindowIdentityProc, 0);
    }
}

bool ParseCustomRule(const std::wstring& text, CustomTitleBarRule* rule) {
    std::vector<std::wstring> parts;
    size_t start = 0;

    while (true) {
        size_t separator = text.find(L'|', start);
        parts.push_back(text.substr(start, separator - start));
        if (separator == std::wstring::npos) {
            break;
        }
        start = separator + 1;
    }

    if (parts.size() != 5 || parts[0].empty() || parts[1].empty()) {
        return false;
    }

    wchar_t* end = nullptr;
    long height = wcstol(parts[2].c_str(), &end, 10);
    if (!end || *end) {
        return false;
    }

    end = nullptr;
    long minRight = wcstol(parts[3].c_str(), &end, 10);
    if (!end || *end) {
        return false;
    }

    end = nullptr;
    long maxRight = wcstol(parts[4].c_str(), &end, 10);
    if (!end || *end) {
        return false;
    }

    if (height <= 0 || minRight < 0 || maxRight <= minRight ||
        height > 256 || maxRight > 1024) {
        return false;
    }

    rule->executable = parts[0];
    rule->windowClass = parts[1];
    rule->heightDip = static_cast<int>(height);
    rule->minRightDip = static_cast<int>(minRight);
    rule->maxRightDip = static_cast<int>(maxRight);
    return true;
}

void LoadSettings() {
    g_enableCustomTitleBarRules =
        Wh_GetIntSetting(L"enableCustomTitleBarRules") != 0;

    g_customTitleBarRules.clear();
    g_maxCustomRuleHeightDip = 0;
    g_maxCustomRuleRightDip = 0;

    for (int i = 0;; i++) {
        PCWSTR setting =
            Wh_GetStringSetting(L"customTitleBarRules[%d]", i);

        if (!setting || !*setting) {
            Wh_FreeStringSetting(setting);
            break;
        }

        CustomTitleBarRule rule;
        if (ParseCustomRule(setting, &rule)) {
            g_maxCustomRuleHeightDip =
                std::max(g_maxCustomRuleHeightDip, rule.heightDip);
            g_maxCustomRuleRightDip =
                std::max(g_maxCustomRuleRightDip, rule.maxRightDip);
            g_customTitleBarRules.push_back(std::move(rule));
        } else {
            Wh_Log(L"Ignoring invalid custom title bar rule: %s", setting);
        }

        Wh_FreeStringSetting(setting);
    }

    RefreshWindowIdentityCache();
}

const CustomTitleBarRule* FindCustomRule(HWND hwnd) {
    if (!g_enableCustomTitleBarRules) {
        return nullptr;
    }

    CachedWindowIdentity* identity = FindCachedWindowIdentity(hwnd);
    if (!identity) {
        return nullptr;
    }

    for (const auto& rule : g_customTitleBarRules) {
        if (_wcsicmp(identity->executable.c_str(), rule.executable.c_str()) != 0) {
            continue;
        }

        if (rule.windowClass != L"*" &&
            _wcsicmp(identity->windowClass.c_str(), rule.windowClass.c_str()) != 0) {
            continue;
        }

        return &rule;
    }

    return nullptr;
}

bool IsPointInCustomRule(HWND hwnd,
                         POINT screenPoint,
                         const CustomTitleBarRule& rule) {
    RECT windowRect = {};
    if (!GetWindowRect(hwnd, &windowRect)) {
        return false;
    }

    UINT dpi = GetWindowDpiSafe(hwnd);
    int relativeY = screenPoint.y - windowRect.top;
    int distanceFromRight = windowRect.right - screenPoint.x;

    return relativeY >= 0 && relativeY < MulDiv(rule.heightDip, dpi, 96) &&
           distanceFromRight >= MulDiv(rule.minRightDip, dpi, 96) &&
           distanceFromRight <= MulDiv(rule.maxRightDip, dpi, 96);
}

// Cheap pre-filter. It only decides whether a click is near enough to a
// caption-button area to justify further inspection. It never triggers hiding.
bool IsPotentialCaptionClick(HWND hwnd, POINT screenPoint) {
    RECT windowRect = {};
    if (!GetWindowRect(hwnd, &windowRect)) {
        return false;
    }

    UINT dpi = GetWindowDpiSafe(hwnd);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    int frameWidth = GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi);
    int frameHeight = GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi);
    int paddedBorder = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
    int buttonWidth = GetSystemMetricsForDpi(SM_CXSIZE, dpi);
    int captionHeight = GetSystemMetricsForDpi(SM_CYSIZE, dpi) + frameHeight +
                        paddedBorder;

    int candidateHeight = captionHeight;
    int candidateWidth = buttonWidth * 4 + frameWidth;

    if (g_enableCustomTitleBarRules) {
        candidateHeight =
            std::max(candidateHeight,
                     MulDiv(g_maxCustomRuleHeightDip, dpi, 96));
        candidateWidth =
            std::max(candidateWidth,
                     MulDiv(g_maxCustomRuleRightDip, dpi, 96) + frameWidth);
    }

    if (screenPoint.y < windowRect.top ||
        screenPoint.y >= windowRect.top + candidateHeight) {
        return false;
    }

    bool rightToLeft = (exStyle & WS_EX_LAYOUTRTL) != 0;
    if (rightToLeft && !g_enableCustomTitleBarRules) {
        return screenPoint.x >= windowRect.left &&
               screenPoint.x < windowRect.left + candidateWidth;
    }

    return screenPoint.x >= windowRect.right - candidateWidth &&
           screenPoint.x < windowRect.right;
}

HWND FindMinimizeButtonWindow(POINT screenPoint) {
    HWND hwnd = WindowFromPoint(screenPoint);
    if (!hwnd) {
        return nullptr;
    }

    hwnd = GetAncestor(hwnd, GA_ROOT);
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) ||
        IsExcludedWindow(hwnd)) {
        return nullptr;
    }

    if (!IsPotentialCaptionClick(hwnd, screenPoint)) {
        return nullptr;
    }

    // Native caption buttons are the only unconditional trigger.
    if (HasMinimizeFrame(hwnd)) {
        LPARAM pointParam = MAKELPARAM(static_cast<SHORT>(screenPoint.x),
                                       static_cast<SHORT>(screenPoint.y));
        DWORD_PTR hitTestResult = HTNOWHERE;

        if (SendMessageTimeoutW(hwnd, WM_NCHITTEST, 0, pointParam,
                                SMTO_ABORTIFHUNG, HIT_TEST_TIMEOUT_MS,
                                &hitTestResult)) {
            LRESULT hitTest = static_cast<LRESULT>(hitTestResult);
            if (hitTest == HTMINBUTTON) {
                return hwnd;
            }

            // A custom rule is only allowed to override client-area hit
            // testing, never another explicit non-client result.
            if (hitTest != HTCLIENT) {
                return nullptr;
            }
        } else if (!g_enableCustomTitleBarRules) {
            return nullptr;
        }
    } else if (!g_enableCustomTitleBarRules) {
        return nullptr;
    }

    const CustomTitleBarRule* rule = FindCustomRule(hwnd);
    return rule && IsPointInCustomRule(hwnd, screenPoint, *rule) ? hwnd
                                                                 : nullptr;
}

bool IsRightClickReleaseValid(HWND hwnd,
                              DWORD processId,
                              POINT downPoint,
                              POINT upPoint) {
    if (!IsWindow(hwnd) || GetWindowProcessId(hwnd) != processId) {
        return false;
    }

    HWND windowAtPoint = WindowFromPoint(upPoint);
    if (!windowAtPoint || GetAncestor(windowAtPoint, GA_ROOT) != hwnd) {
        return false;
    }

    RECT windowRect = {};
    if (!GetWindowRect(hwnd, &windowRect)) {
        return false;
    }

    if (upPoint.x < windowRect.left || upPoint.x >= windowRect.right ||
        upPoint.y < windowRect.top || upPoint.y >= windowRect.bottom) {
        return false;
    }

    int tolerance = MulDiv(16, GetWindowDpiSafe(hwnd), 96);
    LONG deltaX = upPoint.x - downPoint.x;
    LONG deltaY = upPoint.y - downPoint.y;

    return deltaX >= -tolerance && deltaX <= tolerance &&
           deltaY >= -tolerance && deltaY <= tolerance;
}

void FillNotificationIconData(const TrayItem& item, NOTIFYICONDATAW* nid) {
    *nid = {};
    nid->cbSize = sizeof(*nid);
    nid->hWnd = g_controllerWindow;
    nid->uID = item.id;
    nid->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid->uCallbackMessage = WM_TRAY_ICON;
    nid->hIcon = item.icon;
    wcsncpy_s(nid->szTip, ARRAYSIZE(nid->szTip), item.title.c_str(), _TRUNCATE);
}

bool AddNotificationIcon(TrayItem& item) {
    NOTIFYICONDATAW nid = {};
    FillNotificationIconData(item, &nid);

    item.iconAdded = Shell_NotifyIconW(NIM_ADD, &nid) != FALSE;
    return item.iconAdded;
}

bool VerifyNotificationIcon(TrayItem& item) {
    if (!item.iconAdded) {
        return false;
    }

    NOTIFYICONDATAW nid = {};
    FillNotificationIconData(item, &nid);

    if (Shell_NotifyIconW(NIM_MODIFY, &nid)) {
        return true;
    }

    item.iconAdded = false;
    return false;
}

void DeleteNotificationIcon(TrayItem& item) {
    if (!item.iconAdded) {
        return;
    }

    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_controllerWindow;
    nid.uID = item.id;

    Shell_NotifyIconW(NIM_DELETE, &nid);
    item.iconAdded = false;
}

void RecreateNotificationIcons() {
    for (auto& item : g_trayItems) {
        item.iconAdded = false;
        AddNotificationIcon(item);
    }
}

void AddTrayItem(HWND hwnd, bool hideWindow) {
    if (!IsWindow(hwnd)) {
        return;
    }

    if (TrayItem* existing = FindTrayItemByWindow(hwnd)) {
        if (!VerifyNotificationIcon(*existing)) {
            AddNotificationIcon(*existing);
        }

        if (hideWindow && existing->iconAdded) {
            ShowWindowAsync(hwnd, SW_HIDE);
        }
        return;
    }

    TrayItem item;
    item.hwnd = hwnd;
    item.processId = GetWindowProcessId(hwnd);
    item.id = g_nextTrayId++;
    item.title = GetWindowTitleSafe(hwnd);
    item.icon = CopyWindowIcon(hwnd, &item.ownsIcon);

    if (hideWindow) {
        // Never hide a new window unless recovery is marked first.
        if (!SetPropW(hwnd, HIDDEN_WINDOW_PROPERTY,
                      reinterpret_cast<HANDLE>(static_cast<INT_PTR>(1)))) {
            if (item.icon && item.ownsIcon) {
                DestroyIcon(item.icon);
            }
            return;
        }
    }

    g_trayItems.push_back(item);
    TrayItem& addedItem = g_trayItems.back();

    if (!AddNotificationIcon(addedItem)) {
        if (hideWindow) {
            // A new hide request is cancelled if there is no restore path.
            RemovePropW(hwnd, HIDDEN_WINDOW_PROPERTY);

            if (addedItem.icon && addedItem.ownsIcon) {
                DestroyIcon(addedItem.icon);
            }

            g_trayItems.pop_back();
            Wh_Log(L"Failed to create tray icon for window %p", hwnd);
        }

        // Recovery keeps the marker and item. The timer and TaskbarCreated
        // handler will continue trying to recreate the icon.
        return;
    }

    if (hideWindow) {
        ShowWindowAsync(hwnd, SW_HIDE);
    }
}

void RemoveTrayItemByIndex(size_t index, bool restoreWindow) {
    if (index >= g_trayItems.size()) {
        return;
    }

    TrayItem item = g_trayItems[index];

    if (IsTrayItemWindowValid(item)) {
        RemovePropW(item.hwnd, HIDDEN_WINDOW_PROPERTY);

        if (restoreWindow) {
            // Cross-process restore must not block the tool's shutdown.
            ShowWindowAsync(item.hwnd, SW_SHOW);
        }
    }

    DeleteNotificationIcon(g_trayItems[index]);

    if (item.icon && item.ownsIcon) {
        DestroyIcon(item.icon);
    }

    g_trayItems.erase(g_trayItems.begin() + index);
}

void RestoreTrayItem(UINT id) {
    for (size_t i = 0; i < g_trayItems.size(); i++) {
        if (g_trayItems[i].id != id) {
            continue;
        }

        HWND hwnd = g_trayItems[i].hwnd;
        DWORD processId = g_trayItems[i].processId;

        RemoveTrayItemByIndex(i, true);

        if (IsWindow(hwnd) && GetWindowProcessId(hwnd) == processId) {
            SetForegroundWindow(hwnd);
        }
        return;
    }
}

void CloseTrayItem(UINT id) {
    TrayItem* item = FindTrayItemById(id);
    if (!item || !IsTrayItemWindowValid(*item)) {
        return;
    }

    PostMessageW(item->hwnd, WM_CLOSE, 0, 0);
}

void RestoreAllWindows() {
    // Post every restore first. Even if Explorer is busy while tray icons are
    // removed, no hidden window is left waiting behind that shell operation.
    for (auto& item : g_trayItems) {
        if (IsTrayItemWindowValid(item)) {
            RemovePropW(item.hwnd, HIDDEN_WINDOW_PROPERTY);
            ShowWindowAsync(item.hwnd, SW_SHOW);
        }
    }

    while (!g_trayItems.empty()) {
        TrayItem item = g_trayItems.back();
        DeleteNotificationIcon(g_trayItems.back());

        if (item.icon && item.ownsIcon) {
            DestroyIcon(item.icon);
        }

        g_trayItems.pop_back();
    }
}

void CleanupTrayItems() {
    RefreshWindowIdentityCache();

    for (size_t i = g_trayItems.size(); i > 0; i--) {
        size_t index = i - 1;
        TrayItem& item = g_trayItems[index];

        if (!IsTrayItemWindowValid(item)) {
            RemoveTrayItemByIndex(index, false);
            continue;
        }

        // Applications such as Steam and Discord can restore themselves using
        // their own tray icon. Remove our redundant icon and marker.
        if (IsWindowVisible(item.hwnd)) {
            RemoveTrayItemByIndex(index, false);
            continue;
        }

        // Verify the shell still knows about every icon. This recovers even if
        // TaskbarCreated was missed or filtered.
        if (!VerifyNotificationIcon(item)) {
            AddNotificationIcon(item);
        }
    }
}

BOOL CALLBACK RecoverWindowProc(HWND hwnd, LPARAM) {
    if (!GetPropW(hwnd, HIDDEN_WINDOW_PROPERTY)) {
        return TRUE;
    }

    if (IsWindowVisible(hwnd)) {
        RemovePropW(hwnd, HIDDEN_WINDOW_PROPERTY);
        return TRUE;
    }

    AddTrayItem(hwnd, false);
    return TRUE;
}

void ShowTrayMenu(UINT id) {
    if (!FindTrayItemById(id)) {
        return;
    }

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }

    AppendMenuW(menu, MF_STRING, MENU_RESTORE, L"Restore");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, MENU_CLOSE, L"Close");

    POINT cursor = {};
    GetCursorPos(&cursor);

    SetForegroundWindow(g_controllerWindow);

    UINT command =
        TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON,
                       cursor.x, cursor.y, 0, g_controllerWindow, nullptr);

    DestroyMenu(menu);
    PostMessageW(g_controllerWindow, WM_NULL, 0, 0);

    switch (command) {
        case MENU_RESTORE:
            RestoreTrayItem(id);
            break;

        case MENU_CLOSE:
            CloseTrayItem(id);
            break;
    }
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    const auto* mouseInfo = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);

    if (mouseInfo->flags & LLMHF_INJECTED) {
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    if (wParam == WM_RBUTTONDOWN) {
        HWND hwnd = FindMinimizeButtonWindow(mouseInfo->pt);

        if (hwnd) {
            g_rightClickWindow = hwnd;
            g_rightClickProcessId = GetWindowProcessId(hwnd);
            g_rightClickPoint = mouseInfo->pt;
            return 1;
        }

        g_rightClickWindow = nullptr;
        g_rightClickProcessId = 0;
    } else if (wParam == WM_RBUTTONUP && g_rightClickWindow) {
        HWND originalWindow = g_rightClickWindow;
        DWORD originalProcessId = g_rightClickProcessId;
        POINT downPoint = g_rightClickPoint;

        g_rightClickWindow = nullptr;
        g_rightClickProcessId = 0;

        if (IsRightClickReleaseValid(originalWindow, originalProcessId,
                                     downPoint, mouseInfo->pt)) {
            PostMessageW(g_controllerWindow, WM_HIDE_WINDOW,
                         reinterpret_cast<WPARAM>(originalWindow), 0);
        }

        return 1;
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK ControllerWindowProc(HWND hwnd,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    if (g_taskbarCreatedMessage && message == g_taskbarCreatedMessage) {
        RecreateNotificationIcons();
        return 0;
    }

    switch (message) {
        case WM_HIDE_WINDOW:
            AddTrayItem(reinterpret_cast<HWND>(wParam), true);
            return 0;

        case WM_RELOAD_SETTINGS:
            LoadSettings();
            return 0;

        case WM_TRAY_ICON: {
            UINT id = static_cast<UINT>(wParam);

            switch (LOWORD(lParam)) {
                case WM_LBUTTONUP:
                    RestoreTrayItem(id);
                    break;

                case WM_RBUTTONUP:
                    ShowTrayMenu(id);
                    break;
            }

            return 0;
        }

        case WM_TIMER:
            if (wParam == TIMER_CLEANUP) {
                CleanupTrayItems();
            }
            return 0;

        case WM_CLOSE:
            RestoreAllWindows();
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, TIMER_CLEANUP);
            g_controllerWindow = nullptr;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    // Low-level mouse-hook points are physical pixels. Make every window and
    // cursor coordinate queried on this thread use the same coordinate space.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HINSTANCE instance = GetModuleHandleW(nullptr);
    bool classRegistered = false;
    bool startedEventSignaled = false;
    DWORD exitCode = 1;

    LoadSettings();
    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = ControllerWindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = CONTROLLER_CLASS;

    if (!RegisterClassW(&windowClass)) {
        Wh_Log(L"RegisterClassW failed: %u", GetLastError());
        goto Cleanup;
    }
    classRegistered = true;

    // This is a hidden 0x0 tool window. It can become foreground briefly when
    // showing tray menus, which makes the standard popup-menu dismissal
    // behavior reliable.
    g_controllerWindow = CreateWindowExW(
        WS_EX_TOOLWINDOW, CONTROLLER_CLASS, L"", WS_POPUP, 0, 0, 0, 0, nullptr,
        nullptr, instance, nullptr);

    if (!g_controllerWindow) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        goto Cleanup;
    }

    if (g_taskbarCreatedMessage) {
        ChangeWindowMessageFilterEx(g_controllerWindow, g_taskbarCreatedMessage,
                                    MSGFLT_ALLOW, nullptr);
    }

    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
    if (!g_mouseHook) {
        Wh_Log(L"SetWindowsHookExW failed: %u", GetLastError());
        goto Cleanup;
    }

    SetTimer(g_controllerWindow, TIMER_CLEANUP, 2000, nullptr);

    // Recover windows left hidden if the tool process was restarted. If the
    // notification area isn't ready, their icons remain pending and the timer
    // keeps retrying without dropping the recovery marker.
    EnumWindows(RecoverWindowProc, 0);

    g_workerStartedSuccessfully = true;
    SetEvent(g_workerStartedEvent);
    startedEventSignaled = true;

    Wh_Log(L"Minimize to tray started");

    {
        MSG message;
        BOOL result;

        while ((result = GetMessageW(&message, nullptr, 0, 0)) != 0) {
            if (result == -1) {
                Wh_Log(L"GetMessageW failed: %u", GetLastError());
                break;
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    exitCode = 0;

Cleanup:
    if (!startedEventSignaled && g_workerStartedEvent) {
        SetEvent(g_workerStartedEvent);
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }

    if (g_controllerWindow) {
        RestoreAllWindows();
        DestroyWindow(g_controllerWindow);
        g_controllerWindow = nullptr;
    } else if (!g_trayItems.empty()) {
        RestoreAllWindows();
    }

    if (classRegistered) {
        UnregisterClassW(CONTROLLER_CLASS, instance);
    }

    return exitCode;
}

}  // namespace

BOOL WhTool_ModInit() {
    g_workerStartedSuccessfully = false;

    g_workerStartedEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerStartedEvent) {
        Wh_Log(L"CreateEventW failed: %u", GetLastError());
        return FALSE;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_workerStartedEvent);
        g_workerStartedEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_workerStartedEvent, INFINITE);
    CloseHandle(g_workerStartedEvent);
    g_workerStartedEvent = nullptr;

    if (!g_workerStartedSuccessfully) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_controllerWindow) {
        PostMessageW(g_controllerWindow, WM_RELOAD_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_controllerWindow) {
        PostMessageW(g_controllerWindow, WM_CLOSE, 0, 0);
    }

    if (g_workerThread) {
        DWORD waitResult =
            WaitForSingleObject(g_workerThread, WORKER_SHUTDOWN_TIMEOUT_MS);
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"Worker thread did not stop within %u ms",
                   WORKER_SHUTDOWN_TIMEOUT_MS);
        }

        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
}

// clang-format off
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
// clang-format on
