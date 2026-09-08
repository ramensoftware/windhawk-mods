// ==WindhawkMod==
// @id              custom-menu-height
// @name            Custom Menu Height
// @description     Control the height of Win32 context menu items and menu bars
// @version         1.1
// @author          Kitsune
// @github          https://github.com/AromaKitsune
// @include         *
// @compilerOptions -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Menu Height
Control the height of Win32 context menu items and menu bars. Make classic menus
as spacious as modern WinUI menus.

By default, non-immersive context menu items are shown with a height of 22
pixels, and menu bars with a height of 19 pixels. This mod allows increasing
both menu heights to any custom value.

## Context menu items
| Height: 22px (Windows default) | Height: 32px |
| :----------------------------: | :----------: |
| ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/custom-menu-height_contextMenuItems-22px.png) | ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/custom-menu-height_contextMenuItems-32px.png) |

## Menu bar
| Height: 19px (Windows default) | Height: 32px |
| :----------------------------: | :----------: |
| ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/custom-menu-height_menuBar-19px.png) | ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/custom-menu-height_menuBar-32px.png) |

## Recommended mods
Enhance the overall context menu experience with these complementary mods:
| Mod | Author | Note |
| :-- | :----- | :--- |
| [Classic context menu on Windows 11](https://windhawk.net/mods/explorer-context-menu-classic) | m417z | Requires Windows 11 or later |
| [Custom Window Corner Radius](https://windhawk.net/mods/custom-corner-radius) | m417z | Requires Windows 11 or later |
| [Dark mode context menus](https://windhawk.net/mods/dark-menus) | Mgg Sk | Requires Windows 10 version 1809 or later |
| [Remove Context Menu Items](https://windhawk.net/mods/remove-context-menu-items) | Armaninyow | - |

## Compatibility note: Immersive menus
To ensure the custom context menu item height functions properly within File
Explorer and the taskbar on Windows 10 and later, this mod eradicates immersive
menus system-wide, falling back to standard context menus.

The code for this implementation is based on the
"[Eradicate Immersive Menus](https://windhawk.net/mods/eradicate-immersive-menus)"
mod by **aubymori**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- popupMenuItemHeight: 32
  $name: Context menu item height
  $description: >-
    The height of context menu items in pixels

    Set to 0 to use the default system value and keep immersive menus.

    Set to -1 to use the default system value and eradicate immersive menus
    system-wide.
- menuBarHeight: 32
  $name: Menu bar height
  $description: >-
    The height of the menu bar in pixels

    Set to 0 to use the default system value.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <algorithm>
#include <mutex>
#include <unordered_map>

#ifdef _WIN64
#   define DRAW_IMMERSIVE_MENU L"bool __cdecl ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu(struct HMENU__ *,struct HWND__ *)"
#else
#   define DRAW_IMMERSIVE_MENU L"bool __stdcall ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu(struct HMENU__ *,struct HWND__ *)"
#endif

struct
{
    int popupMenuItemHeight;
    int menuBarHeight;
} settings;

constexpr UINT WM_UAHMEASUREMENUITEM = 0x0094;
constexpr DWORD MFISPOPUP            = 0x00000001;

UINT g_updateMenuBarHeightMsg = 0;

std::mutex g_offsetMutex;
std::unordered_map<HWND, int> g_offsetMap;

union UAHMENUITEMMETRICS
{
    struct
    {
        DWORD cx;
        DWORD cy;
    } rgsizeBar[2];
    struct
    {
        DWORD cx;
        DWORD cy;
    } rgsizePopup[4];
};

struct UAHMENUPOPUPMETRICS
{
    DWORD rgcx[4];
    DWORD fUpdateMaxWidths : 2;
};

struct UAHMENU
{
    HMENU hMenu;
    HDC hdc;
    DWORD dwFlags;
};

struct UAHMENUITEM
{
    int iPosition;
    UAHMENUITEMMETRICS uahMenuItemMetrics;
    UAHMENUPOPUPMETRICS uahMenuPopupMetrics;
};

struct UAHMEASUREMENUITEM
{
    MEASUREITEMSTRUCT measureItemStruct;
    UAHMENU uahMenu;
    UAHMENUITEM uahMenuItem;
};

// Helper: Get the current process name
LPCWSTR GetCurrentProcessName()
{
    static WCHAR szProcessPath[MAX_PATH];
    static LPCWSTR pszProcessName = nullptr;
    if (!pszProcessName)
    {
        GetModuleFileNameW(nullptr, szProcessPath, ARRAYSIZE(szProcessPath));
        pszProcessName = wcsrchr(szProcessPath, L'\\');
        pszProcessName = pszProcessName
            ? pszProcessName + 1
            : szProcessPath;
    }
    return pszProcessName;
}

// Helper: Get the system DPI
UINT GetSystemDpi()
{
    using GetDpiForSystem_t = decltype(&GetDpiForSystem);
    static auto pfnGetDpiForSystem = reinterpret_cast<GetDpiForSystem_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForSystem"));
    if (pfnGetDpiForSystem)
    {
        return pfnGetDpiForSystem();
    }

    // Fallback for Windows 8.1 and earlier
    UINT uDpi = 96;
    HDC hdc = GetDC(nullptr);
    if (hdc)
    {
        uDpi = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(nullptr, hdc);
    }

    return uDpi;
}

// Helper: Get the window DPI
UINT GetWindowDpi(HWND hWnd)
{
    if (!hWnd)
    {
        return GetSystemDpi();
    }

    using GetDpiForWindow_t = decltype(&GetDpiForWindow);
    static auto pfnGetDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
    if (pfnGetDpiForWindow)
    {
        return pfnGetDpiForWindow(hWnd);
    }

    // Fallback for Windows 8.1 and earlier
    UINT uDpi = 96;
    HDC hdc = GetDC(hWnd);
    if (hdc)
    {
        uDpi = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(hWnd, hdc);
    }

    return uDpi;
}

// Helper: Apply custom height metrics to UAH menus
void AdjustUahMenuItemMetrics(HWND hWnd, LPARAM lParam)
{
    auto* uahMeasureMenuItem = reinterpret_cast<UAHMEASUREMENUITEM*>(lParam);
    if (!uahMeasureMenuItem)
    {
        return;
    }

    bool isPopupMenu = (uahMeasureMenuItem->uahMenu.dwFlags & MFISPOPUP) != 0;
    int cyBaseHeight = isPopupMenu
        ? settings.popupMenuItemHeight
        : settings.menuBarHeight;

    // Leave the menu height at the system default if set to 0 (or -1 for popup
    // menu items)
    if (cyBaseHeight <= 0)
    {
        return;
    }

    // Ignore separator items inside popup menus
    MENUITEMINFOW menuItemInfo{ sizeof(menuItemInfo) };
    menuItemInfo.fMask = MIIM_FTYPE;
    if (isPopupMenu &&
        GetMenuItemInfoW(uahMeasureMenuItem->uahMenu.hMenu,
            uahMeasureMenuItem->uahMenuItem.iPosition, TRUE,
            &menuItemInfo) &&
        (menuItemInfo.fType & MFT_SEPARATOR))
    {
        return;
    }

    UINT uDpi = GetWindowDpi(hWnd);
    UINT cyTargetHeight = MulDiv(cyBaseHeight, uDpi, 96);
    if (uahMeasureMenuItem->measureItemStruct.itemHeight < cyTargetHeight)
    {
        uahMeasureMenuItem->measureItemStruct.itemHeight = cyTargetHeight;
    }
}

// Helper: Update the custom menu bar height and dynamically resize the window
bool UpdateMenuBarHeight(HWND hWnd, UINT uMsg)
{
    if (uMsg != g_updateMenuBarHeightMsg)
    {
        return false;
    }

    HMENU hMenuBar = GetMenu(hWnd);
    if (!hMenuBar)
    {
        return true;
    }

    // Cache the original client area dimensions before modifying the menu bar
    RECT rcOriginalClient;
    GetClientRect(hWnd, &rcOriginalClient);

    // Invalidate the UAH geometry cache for all menu bar items
    int cMenuBarItems = GetMenuItemCount(hMenuBar);
    for (int iMenuBarItem = 0; iMenuBarItem < cMenuBarItems; iMenuBarItem++)
    {
        MENUITEMINFOW menuItemInfo{ sizeof(menuItemInfo) };
        menuItemInfo.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_DATA;
        if (GetMenuItemInfoW(hMenuBar, iMenuBarItem, TRUE, &menuItemInfo))
        {
            SetMenuItemInfoW(hMenuBar, iMenuBarItem, TRUE, &menuItemInfo);
        }
    }

    // Force the non-client area to re-calculate its layout and borders
    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    // Calculate the increase in menu bar height by measuring the updated client
    // area dimensions
    RECT rcUpdatedClient;
    GetClientRect(hWnd, &rcUpdatedClient);
    int cyMenuBarGrowth = rcOriginalClient.bottom - rcUpdatedClient.bottom;

    // Resize the window to account for the increased menu bar height and cache
    // the offset
    if (cyMenuBarGrowth != 0 && !IsZoomed(hWnd))
    {
        RECT rcWindow;
        GetWindowRect(hWnd, &rcWindow);
        int cxWindow = rcWindow.right - rcWindow.left;
        int cyWindow = rcWindow.bottom - rcWindow.top;
        SetWindowPos(hWnd, nullptr, 0, 0, cxWindow, cyWindow + cyMenuBarGrowth,
            SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

        std::lock_guard<std::mutex> lock(g_offsetMutex);
        g_offsetMap[hWnd] += cyMenuBarGrowth;
    }

    // Force a re-draw of the menu bar
    DrawMenuBar(hWnd);

    return true;
}

// Enumeration callback: Force existing windows to re-calculate and re-draw
// their menu bars
BOOL CALLBACK UpdateEnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId())
    {
        return TRUE;
    }
    PostMessageW(hWnd, g_updateMenuBarHeightMsg, 0, 0);
    return TRUE;
}

// Enumeration callback: Revert the custom menu bar height and window dimensions
BOOL CALLBACK UninitEnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId())
    {
        return TRUE;
    }

    HMENU hMenuBar = GetMenu(hWnd);
    if (!hMenuBar)
    {
        return TRUE;
    }

    int cyCachedMenuBar = static_cast<int>(lParam);
    int cyMenuBarShrink = 0;

    {
        std::lock_guard<std::mutex> lock(g_offsetMutex);
        auto it = g_offsetMap.find(hWnd);
        if (it != g_offsetMap.end())
        {
            // Revert the menu bar height for dynamically resized windows using
            // the cached offset
            cyMenuBarShrink = it->second;
        }
        else if (cyCachedMenuBar != 0)
        {
            // Calculate and revert the menu bar height for natively created
            // windows
            UINT uDpi = GetWindowDpi(hWnd);
            int cyNativeMenuBar = MulDiv(19, uDpi, 96);
            int cyTargetMenuBar = MulDiv(cyCachedMenuBar, uDpi, 96);
            cyMenuBarShrink = cyTargetMenuBar - cyNativeMenuBar;
        }
    }

    // Windhawk removes hooks before uninitialization; invalidate the UAH
    // geometry cache inline for all menu bar items
    int cMenuBarItems = GetMenuItemCount(hMenuBar);
    for (int iMenuBarItem = 0; iMenuBarItem < cMenuBarItems; iMenuBarItem++)
    {
        MENUITEMINFOW menuItemInfo{ sizeof(menuItemInfo) };
        menuItemInfo.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_DATA;
        if (GetMenuItemInfoW(hMenuBar, iMenuBarItem, TRUE, &menuItemInfo))
        {
            SetMenuItemInfoW(hMenuBar, iMenuBarItem, TRUE, &menuItemInfo);
        }
    }

    // Resize the window to account for the reduced menu bar height
    if (cyMenuBarShrink != 0 && !IsZoomed(hWnd))
    {
        RECT rcWindow;
        GetWindowRect(hWnd, &rcWindow);
        int cxWindow = rcWindow.right - rcWindow.left;
        int cyWindow = rcWindow.bottom - rcWindow.top;
        SetWindowPos(hWnd, nullptr, 0, 0, cxWindow, cyWindow - cyMenuBarShrink,
            SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
    else
    {
        // Force the non-client area to re-calculate its layout and borders
        SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    // Force a re-draw of the menu bar
    DrawMenuBar(hWnd);

    return TRUE;
}

// Hook for DefWindowProcW
using DefWindowProcW_t = decltype(&DefWindowProcW);
DefWindowProcW_t DefWindowProcW_Original;
LRESULT CALLBACK DefWindowProcW_Hook(HWND hWnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam)
{
    if (UpdateMenuBarHeight(hWnd, uMsg))
    {
        return 0;
    }

    if (uMsg == WM_NCDESTROY)
    {
        std::lock_guard<std::mutex> lock(g_offsetMutex);
        g_offsetMap.erase(hWnd);
    }

    LRESULT lResult = DefWindowProcW_Original(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_UAHMEASUREMENUITEM)
    {
        AdjustUahMenuItemMetrics(hWnd, lParam);
    }

    return lResult;
}

// Hook for DefWindowProcA
using DefWindowProcA_t = decltype(&DefWindowProcA);
DefWindowProcA_t DefWindowProcA_Original;
LRESULT CALLBACK DefWindowProcA_Hook(HWND hWnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam)
{
    if (UpdateMenuBarHeight(hWnd, uMsg))
    {
        return 0;
    }

    if (uMsg == WM_NCDESTROY)
    {
        std::lock_guard<std::mutex> lock(g_offsetMutex);
        g_offsetMap.erase(hWnd);
    }

    LRESULT lResult = DefWindowProcA_Original(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_UAHMEASUREMENUITEM)
    {
        AdjustUahMenuItemMetrics(hWnd, lParam);
    }

    return lResult;
}

// Hook for DefFrameProcW
using DefFrameProcW_t = decltype(&DefFrameProcW);
DefFrameProcW_t DefFrameProcW_Original;
LRESULT CALLBACK DefFrameProcW_Hook(HWND hWnd, HWND hWndMdiClient, UINT uMsg,
    WPARAM wParam, LPARAM lParam)
{
    if (UpdateMenuBarHeight(hWnd, uMsg))
    {
        return 0;
    }

    if (uMsg == WM_NCDESTROY)
    {
        std::lock_guard<std::mutex> lock(g_offsetMutex);
        g_offsetMap.erase(hWnd);
    }

    LRESULT lResult = DefFrameProcW_Original(hWnd, hWndMdiClient, uMsg, wParam,
        lParam);

    if (uMsg == WM_UAHMEASUREMENUITEM)
    {
        AdjustUahMenuItemMetrics(hWnd, lParam);
    }

    return lResult;
}

// Hook for DefFrameProcA
using DefFrameProcA_t = decltype(&DefFrameProcA);
DefFrameProcA_t DefFrameProcA_Original;
LRESULT CALLBACK DefFrameProcA_Hook(HWND hWnd, HWND hWndMdiClient, UINT uMsg,
    WPARAM wParam, LPARAM lParam)
{
    if (UpdateMenuBarHeight(hWnd, uMsg))
    {
        return 0;
    }

    if (uMsg == WM_NCDESTROY)
    {
        std::lock_guard<std::mutex> lock(g_offsetMutex);
        g_offsetMap.erase(hWnd);
    }

    LRESULT lResult = DefFrameProcA_Original(hWnd, hWndMdiClient, uMsg, wParam,
        lParam);

    if (uMsg == WM_UAHMEASUREMENUITEM)
    {
        AdjustUahMenuItemMetrics(hWnd, lParam);
    }

    return lResult;
}

// Hook for DefDlgProcW
using DefDlgProcW_t = decltype(&DefDlgProcW);
DefDlgProcW_t DefDlgProcW_Original;
LRESULT CALLBACK DefDlgProcW_Hook(HWND hWnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam)
{
    if (UpdateMenuBarHeight(hWnd, uMsg))
    {
        return 0;
    }

    if (uMsg == WM_NCDESTROY)
    {
        std::lock_guard<std::mutex> lock(g_offsetMutex);
        g_offsetMap.erase(hWnd);
    }

    LRESULT lResult = DefDlgProcW_Original(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_UAHMEASUREMENUITEM)
    {
        AdjustUahMenuItemMetrics(hWnd, lParam);
    }

    return lResult;
}

// Hook for DefDlgProcA
using DefDlgProcA_t = decltype(&DefDlgProcA);
DefDlgProcA_t DefDlgProcA_Original;
LRESULT CALLBACK DefDlgProcA_Hook(HWND hWnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam)
{
    if (UpdateMenuBarHeight(hWnd, uMsg))
    {
        return 0;
    }

    if (uMsg == WM_NCDESTROY)
    {
        std::lock_guard<std::mutex> lock(g_offsetMutex);
        g_offsetMap.erase(hWnd);
    }

    LRESULT lResult = DefDlgProcA_Original(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_UAHMEASUREMENUITEM)
    {
        AdjustUahMenuItemMetrics(hWnd, lParam);
    }

    return lResult;
}

// Hook for GetSystemMetrics
using GetSystemMetrics_t = decltype(&GetSystemMetrics);
GetSystemMetrics_t GetSystemMetrics_Original;
int WINAPI GetSystemMetrics_Hook(int nIndex)
{
    if (nIndex == SM_CYMENU && settings.menuBarHeight != 0)
    {
        UINT uDpi = GetSystemDpi();
        int cyBaseMenuBar = GetSystemMetrics_Original(nIndex);
        int cyNativeMenuBar = MulDiv(19, uDpi, 96);
        int cyTargetMenuBar = MulDiv(settings.menuBarHeight, uDpi, 96);

        // Preserve the invisible window frame borders by injecting only the
        // menu bar height offset
        return cyBaseMenuBar + (cyTargetMenuBar - cyNativeMenuBar);
    }
    return GetSystemMetrics_Original(nIndex);
}

// Hook for GetSystemMetricsForDpi
using GetSystemMetricsForDpi_t = decltype(&GetSystemMetricsForDpi);
GetSystemMetricsForDpi_t GetSystemMetricsForDpi_Original;
int WINAPI GetSystemMetricsForDpi_Hook(int nIndex, UINT uDpi)
{
    if (nIndex == SM_CYMENU && settings.menuBarHeight != 0)
    {
        int cyBaseMenuBar = GetSystemMetricsForDpi_Original(nIndex, uDpi);
        int cyNativeMenuBar = MulDiv(19, uDpi, 96);
        int cyTargetMenuBar = MulDiv(settings.menuBarHeight, uDpi, 96);

        // Preserve the invisible window frame borders by injecting only the
        // menu bar height offset
        return cyBaseMenuBar + (cyTargetMenuBar - cyNativeMenuBar);
    }
    return GetSystemMetricsForDpi_Original(nIndex, uDpi);
}

// Hook for ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu
using DrawImmersiveMenu_t = bool(__fastcall*)(HMENU, HWND);
#define GENERATE_IMMERSIVE_MENU_HOOK(hookPrefix) \
    DrawImmersiveMenu_t hookPrefix##_Original = nullptr; \
    bool __fastcall hookPrefix##_Hook( \
        HMENU hPopupMenu, \
        HWND hWnd \
    ) \
    { \
        if (settings.popupMenuItemHeight == 0 && hookPrefix##_Original) \
        { \
            return hookPrefix##_Original(hPopupMenu, hWnd); \
        } \
        return false; \
    }

// Global shell
GENERATE_IMMERSIVE_MENU_HOOK(ExplorerFrame_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(shell32_DrawImmersiveMenu)

// Core shell
GENERATE_IMMERSIVE_MENU_HOOK(explorer_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(twinui_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(twinui_pcshell_DrawImmersiveMenu)

// Standalone applications
GENERATE_IMMERSIVE_MENU_HOOK(Narrator_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(Taskmgr_DrawImmersiveMenu)

// System tray icons
GENERATE_IMMERSIVE_MENU_HOOK(MoNotificationUx_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(museuxdocked_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(MusNotifyIcon_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(pnidui_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(SecurityHealthSSO_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(SecurityHealthSsoUdk_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(SecurityHealthSystray_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(SndVolSSO_DrawImmersiveMenu)
GENERATE_IMMERSIVE_MENU_HOOK(usoapi_DrawImmersiveMenu)

// Helper: Hook ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu in a
// specified module to eradicate immersive menus system-wide
inline bool ApplyImmersiveMenuHook(LPCWSTR pszModuleName,
    DrawImmersiveMenu_t* ppfnOriginal, void* pfnHook)
{
    HMODULE hModule;

    // Fall back to the host process if no target module is specified
    if (!pszModuleName)
    {
        hModule = GetModuleHandleW(nullptr);
    }
    else
    {
        hModule = LoadLibraryExW(pszModuleName, nullptr,
            LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    LPCWSTR pszTargetName = pszModuleName
        ? pszModuleName
        : GetCurrentProcessName();
    if (!hModule)
    {
        Wh_Log(L"Failed to load %s", pszTargetName);
        return false;
    }

    // explorer.exe, ExplorerFrame.dll, MoNotificationUx.exe, museuxdocked.dll, MusNotifyIcon.exe, Narrator.exe, pnidui.dll, SecurityHealthSSO.dll, SecurityHealthSsoUdk.dll, SecurityHealthSystray.exe, shell32.dll, SndVolSSO.dll, Taskmgr.exe, twinui.dll, twinui.pcshell.dll, usoapi.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] =
    {
        {
            { DRAW_IMMERSIVE_MENU },
            reinterpret_cast<void**>(ppfnOriginal),
            pfnHook,
            true
        }
    };
    if (!WindhawkUtils::HookSymbols(hModule, hooks, ARRAYSIZE(hooks)))
    {
        Wh_Log(
            L"Failed to hook "
            L"ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu in %s",
            pszTargetName
        );
        return false;
    }

    return true;
}

// Helper: Get the OS version
// Required to eradicate immersive menus system-wide on Windows 10 and later.
bool IsWindows10OrGreater()
{
    auto ulNtMajorVersion = *reinterpret_cast<volatile ULONG*>(0x7FFE026C);
    return ulNtMajorVersion >= 10;
}

// Load settings
void LoadSettings()
{
    settings.popupMenuItemHeight = Wh_GetIntSetting(L"popupMenuItemHeight");
    if (settings.popupMenuItemHeight > 0)
    {
        settings.popupMenuItemHeight =
            std::max(settings.popupMenuItemHeight, 22);
    }
    else if (settings.popupMenuItemHeight < -1)
    {
        settings.popupMenuItemHeight = -1;
    }

    settings.menuBarHeight = Wh_GetIntSetting(L"menuBarHeight");
    if (settings.menuBarHeight > 0)
    {
        settings.menuBarHeight = std::max(settings.menuBarHeight, 19);
    }
    else if (settings.menuBarHeight < 0)
    {
        settings.menuBarHeight = 0;
    }

    // Exclude Visual Studio from menu bar height modifications
    LPCWSTR pszProcessName = GetCurrentProcessName();
    if (_wcsicmp(pszProcessName, L"devenv.exe") == 0)
    {
        settings.menuBarHeight = 0;
    }
}

// Mod initialization
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    g_updateMenuBarHeightMsg =
        RegisterWindowMessageW(L"Windhawk_UpdateMenuBarHeight");

    // Initialize hooks to apply custom menu heights
    WindhawkUtils::SetFunctionHook(
        DefWindowProcW,
        DefWindowProcW_Hook,
        &DefWindowProcW_Original
    );

    WindhawkUtils::SetFunctionHook(
        DefWindowProcA,
        DefWindowProcA_Hook,
        &DefWindowProcA_Original
    );

    WindhawkUtils::SetFunctionHook(
        DefFrameProcW,
        DefFrameProcW_Hook,
        &DefFrameProcW_Original
    );

    WindhawkUtils::SetFunctionHook(
        DefFrameProcA,
        DefFrameProcA_Hook,
        &DefFrameProcA_Original
    );

    WindhawkUtils::SetFunctionHook(
        DefDlgProcW,
        DefDlgProcW_Hook,
        &DefDlgProcW_Original
    );

    WindhawkUtils::SetFunctionHook(
        DefDlgProcA,
        DefDlgProcA_Hook,
        &DefDlgProcA_Original
    );

    WindhawkUtils::SetFunctionHook(
        GetSystemMetrics,
        GetSystemMetrics_Hook,
        &GetSystemMetrics_Original
    );

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    auto pfnGetSystemMetricsForDpi = hUser32
        ? reinterpret_cast<GetSystemMetricsForDpi_t>(
            GetProcAddress(hUser32, "GetSystemMetricsForDpi"))
        : nullptr;
    if (pfnGetSystemMetricsForDpi)
    {
        WindhawkUtils::SetFunctionHook(
            pfnGetSystemMetricsForDpi,
            GetSystemMetricsForDpi_Hook,
            &GetSystemMetricsForDpi_Original
        );
    }

    // Initialize hooks to eradicate immersive menus system-wide
    // Required for Windows 10 and later.
    if (!IsWindows10OrGreater())
    {
        return TRUE;
    }

    // Exclude the current process from immersive menus hooks
    LPCWSTR pszProcessName = GetCurrentProcessName();
    bool shouldExcludeProcess =
        _wcsicmp(pszProcessName, L"consent.exe") == 0 ||
        _wcsicmp(pszProcessName, L"dwm.exe") == 0 ||
        _wcsicmp(pszProcessName, L"SearchIndexer.exe") == 0 ||
        _wcsicmp(pszProcessName, L"ShellExperienceHost.exe") == 0 ||
        _wcsicmp(pszProcessName, L"svchost.exe") == 0 ||
        _wcsicmp(pszProcessName, L"windhawk.exe") == 0 ||
        _wcsicmp(pszProcessName, L"wlanext.exe") == 0;
    if (shouldExcludeProcess)
    {
        return TRUE;
    }

    // Exclude isolated system tray processes from global shell hooks
    if (_wcsicmp(pszProcessName, L"MoNotificationUx.exe") != 0 &&
        _wcsicmp(pszProcessName, L"MusNotifyIcon.exe") != 0 &&
        _wcsicmp(pszProcessName, L"SecurityHealthSystray.exe") != 0)
    {
        // Global shell hooks
        ApplyImmersiveMenuHook(
            L"ExplorerFrame.dll",
            &ExplorerFrame_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(ExplorerFrame_DrawImmersiveMenu_Hook)
        );

        ApplyImmersiveMenuHook(
            L"shell32.dll",
            &shell32_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(shell32_DrawImmersiveMenu_Hook)
        );
    }

    // Process-specific hooks
    if (_wcsicmp(pszProcessName, L"explorer.exe") == 0)
    {
        ApplyImmersiveMenuHook(
            nullptr,
            &explorer_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(explorer_DrawImmersiveMenu_Hook)
        );

        ApplyImmersiveMenuHook(
            L"pnidui.dll",
            &pnidui_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(pnidui_DrawImmersiveMenu_Hook)
        );

        ApplyImmersiveMenuHook(
            L"SndVolSSO.dll",
            &SndVolSSO_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(SndVolSSO_DrawImmersiveMenu_Hook)
        );

        ApplyImmersiveMenuHook(
            L"twinui.dll",
            &twinui_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(twinui_DrawImmersiveMenu_Hook)
        );

        ApplyImmersiveMenuHook(
            L"twinui.pcshell.dll",
            &twinui_pcshell_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(twinui_pcshell_DrawImmersiveMenu_Hook)
        );
    }
    else if (_wcsicmp(pszProcessName, L"MoNotificationUx.exe") == 0)
    {
        ApplyImmersiveMenuHook(
            nullptr,
            &MoNotificationUx_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(MoNotificationUx_DrawImmersiveMenu_Hook)
        );

        ApplyImmersiveMenuHook(
            L"museuxdocked.dll",
            &museuxdocked_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(museuxdocked_DrawImmersiveMenu_Hook)
        );
    }
    else if (_wcsicmp(pszProcessName, L"MusNotifyIcon.exe") == 0)
    {
        ApplyImmersiveMenuHook(
            nullptr,
            &MusNotifyIcon_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(MusNotifyIcon_DrawImmersiveMenu_Hook)
        );

        ApplyImmersiveMenuHook(
            L"usoapi.dll",
            &usoapi_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(usoapi_DrawImmersiveMenu_Hook)
        );
    }
    else if (_wcsicmp(pszProcessName, L"Narrator.exe") == 0)
    {
        ApplyImmersiveMenuHook(
            nullptr,
            &Narrator_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(Narrator_DrawImmersiveMenu_Hook)
        );
    }
    else if (_wcsicmp(pszProcessName, L"SecurityHealthSystray.exe") == 0)
    {
        ApplyImmersiveMenuHook(
            nullptr,
            &SecurityHealthSystray_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(SecurityHealthSystray_DrawImmersiveMenu_Hook)
        );

        ApplyImmersiveMenuHook(
            L"SecurityHealthSSO.dll",
            &SecurityHealthSSO_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(SecurityHealthSSO_DrawImmersiveMenu_Hook)
        );

        ApplyImmersiveMenuHook(
            L"SecurityHealthSsoUdk.dll",
            &SecurityHealthSsoUdk_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(SecurityHealthSsoUdk_DrawImmersiveMenu_Hook)
        );
    }
    else if (_wcsicmp(pszProcessName, L"Taskmgr.exe") == 0)
    {
        ApplyImmersiveMenuHook(
            nullptr,
            &Taskmgr_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(Taskmgr_DrawImmersiveMenu_Hook)
        );

        ApplyImmersiveMenuHook(
            L"twinui.dll",
            &twinui_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(twinui_DrawImmersiveMenu_Hook)
        );

        ApplyImmersiveMenuHook(
            L"twinui.pcshell.dll",
            &twinui_pcshell_DrawImmersiveMenu_Original,
            reinterpret_cast<void*>(twinui_pcshell_DrawImmersiveMenu_Hook)
        );
    }

    return TRUE;
}

// Mod post-initialization
void Wh_ModAfterInit()
{
    Wh_Log(L"AfterInit");

    EnumWindows(UpdateEnumWindowsProc, 0);
}

// Mod uninitialization
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    int cyCachedMenuBar = settings.menuBarHeight;

    settings.popupMenuItemHeight = 0;
    settings.menuBarHeight = 0;

    EnumWindows(UninitEnumWindowsProc, static_cast<LPARAM>(cyCachedMenuBar));
}

// Reload settings
void Wh_ModSettingsChanged()
{
    LoadSettings();

    EnumWindows(UpdateEnumWindowsProc, 0);
}
