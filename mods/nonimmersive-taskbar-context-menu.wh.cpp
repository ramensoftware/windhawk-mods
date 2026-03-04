// ==WindhawkMod==
// @id              nonimmersive-taskbar-context-menu
// @name            Non Immersive Taskbar Context Menu (submenus friendly)
// @description     Restores the non-immersive taskbar context menu, handling for the submenus
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Non Immersive Taskbar Context Menu (submenus friendly)
This is a fork of the mod [*Non Immersive Taskbar Context Menu*](https://windhawk.net/mods/classic-taskbar-context-menu), 
which in turn is based on [Taskbar-Context-Menu-Tweaker](https://github.com/rikka0w0/Taskbar-Context-Menu-Tweaker).
I tried to contact the author to push this as an update but received no response.

This mod includes a fix for correct display of the second-level menus, which with the original mod are not always
populated properly on the first appearance.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showIcons: false
  $name: Show icons
  $description: Enable this if you want icons to be shown in the context menu of the taskbar.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <unordered_set>

struct {
    bool showIcons;
} settings;

#define MYICON_SETTING 0
#define MYICON_TASKMGR 1
#define MYICON_SHOWDESKTOP 2
#define MYICON_COUNT 3

#define ICON_SETTING_INDEX 21
#define ICON_SHOWDESKTOP_INDEX 34

static HBITMAP g_icons[MYICON_COUNT];
static HWND g_taskBar = NULL;
static HWND g_taskBarSecondary = NULL;
static HWND g_atlWnd1 = NULL;
static HWND g_atlWnd2 = NULL;

std::unordered_set<HWND> g_subclassedWindows;

UINT g_subclassMsg = RegisterWindowMessage(L"Windhawk_classic-taskbar-context-menu");
const UINT WM_TASKBARCREATED = RegisterWindowMessage(L"TaskbarCreated");

int GetDPI() {
    HDC hdc = GetDC(NULL);
    int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(NULL, hdc);
    return dpi;
}

HBITMAP IconToBitmap(HICON hIcon, int size) {
    if (!hIcon) return NULL;

    ICONINFO info = {0};
    if (!GetIconInfo(hIcon, &info) || !info.fIcon) return NULL;

    if (info.hbmColor) DeleteObject(info.hbmColor);
    if (info.hbmMask) DeleteObject(info.hbmMask);

    if (size <= 0) return NULL;

    int pixelCount = size * size;
    HDC dc = GetDC(NULL);
    HDC dcMem = CreateCompatibleDC(dc);
    if (!dcMem) {
        ReleaseDC(NULL, dc);
        return NULL;
    }

    BITMAPINFOHEADER bi = {0};
    bi.biSize = sizeof(bi);
    bi.biWidth = size;
    bi.biHeight = -size;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    INT* pData = NULL;
    HBITMAP dib = CreateDIBSection(dc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pData, NULL, 0);
    
    if (!dib) {
        DeleteDC(dcMem);
        ReleaseDC(NULL, dc);
        return NULL;
    }

    memset(pData, 0, pixelCount * 4);
    HBITMAP hBmpOld = (HBITMAP)SelectObject(dcMem, dib);
    
    DrawIconEx(dcMem, 0, 0, hIcon, size, size, 0, NULL, DI_MASK);
    
    bool* opaque = (bool*)malloc(pixelCount * sizeof(bool));
    if (opaque) {
        for (int i = 0; i < pixelCount; i++)
            opaque[i] = !pData[i];

        memset(pData, 0, pixelCount * 4);
        DrawIconEx(dcMem, 0, 0, hIcon, size, size, 0, NULL, DI_NORMAL);

        bool hasAlpha = false;
        for (int i = 0; i < pixelCount; i++) {
            if (((UINT*)pData)[i] & 0xFF000000) {
                hasAlpha = true;
                break;
            }
        }

        if (!hasAlpha) {
            for (int i = 0; i < pixelCount; i++) {
                if (opaque[i])
                    ((UINT*)pData)[i] |= 0xFF000000;
                else
                    ((UINT*)pData)[i] &= 0x00FFFFFF;
            }
        }
        free(opaque);
    }

    SelectObject(dcMem, hBmpOld);
    DeleteDC(dcMem);
    ReleaseDC(NULL, dc);

    return dib;
}

void LoadIcons() {
    int size = 16 * GetDPI() / 96;
    HICON hIcon;

    ExtractIconEx(L"shell32.dll", ICON_SETTING_INDEX, NULL, &hIcon, 1);
    g_icons[MYICON_SETTING] = IconToBitmap(hIcon, size);
    DestroyIcon(hIcon);

    ExtractIconEx(L"taskmgr.exe", 0, NULL, &hIcon, 1);
    g_icons[MYICON_TASKMGR] = IconToBitmap(hIcon, size);
    DestroyIcon(hIcon);

    ExtractIconEx(L"shell32.dll", ICON_SHOWDESKTOP_INDEX, NULL, &hIcon, 1);
    g_icons[MYICON_SHOWDESKTOP] = IconToBitmap(hIcon, size);
    DestroyIcon(hIcon);
}

void FreeIcons() {
    for (int i = 0; i < MYICON_COUNT; i++) {
        if (g_icons[i]) {
            DeleteObject(g_icons[i]);
            g_icons[i] = NULL;
        }
    }
}

void ApplyClassicMenu(HMENU hMenu) {
    if (!hMenu) return;

    MENUINFO mi = {sizeof(mi), MIM_BACKGROUND};
    GetMenuInfo(hMenu, &mi);
    mi.hbrBack = NULL;
    SetMenuInfo(hMenu, &mi);

    int count = GetMenuItemCount(hMenu);
    for (int i = 0; i < count; i++) {
        MENUITEMINFO mii = {sizeof(mii), MIIM_FTYPE};
        if (GetMenuItemInfo(hMenu, i, TRUE, &mii)) {
            mii.fType &= ~MFT_OWNERDRAW;
            SetMenuItemInfo(hMenu, i, TRUE, &mii);
        }
    }
}

void ApplyMenuIcons(HMENU hMenu, UINT settingId, UINT taskmgrId, UINT desktopId) {
    if (!settings.showIcons) return;
    SetMenuItemBitmaps(hMenu, settingId, MF_BYCOMMAND, g_icons[MYICON_SETTING], g_icons[MYICON_SETTING]);
    SetMenuItemBitmaps(hMenu, taskmgrId, MF_BYCOMMAND, g_icons[MYICON_TASKMGR], g_icons[MYICON_TASKMGR]);
    SetMenuItemBitmaps(hMenu, desktopId, MF_BYCOMMAND, g_icons[MYICON_SHOWDESKTOP], g_icons[MYICON_SHOWDESKTOP]);
}

// Hook InsertMenuItemW
using InsertMenuItemW_t = decltype(&InsertMenuItemW);
InsertMenuItemW_t pOriginalInsertMenuItemW;

BOOL WINAPI InsertMenuItemW_Hook(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmi) {
    BOOL result = pOriginalInsertMenuItemW(hMenu, item, fByPosition, lpmi);
    if (result && lpmi) {
        MENUITEMINFOW mii = {sizeof(mii), MIIM_FTYPE};
        if (GetMenuItemInfoW(hMenu, item, fByPosition, &mii) && (mii.fType & MFT_OWNERDRAW)) {
            mii.fType &= ~MFT_OWNERDRAW;
            SetMenuItemInfoW(hMenu, item, fByPosition, &mii);
        }
    }
    return result;
}

// Hook SetMenuItemInfoW
using SetMenuItemInfoW_t = decltype(&SetMenuItemInfoW);
SetMenuItemInfoW_t pOriginalSetMenuItemInfoW;

BOOL WINAPI SetMenuItemInfoW_Hook(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmii) {
    if (lpmii && (lpmii->fMask & MIIM_FTYPE) && (lpmii->fType & MFT_OWNERDRAW)) {
        MENUITEMINFOW copy = *lpmii;
        copy.fType &= ~MFT_OWNERDRAW;
        return pOriginalSetMenuItemInfoW(hMenu, item, fByPosition, &copy);
    }
    return pOriginalSetMenuItemInfoW(hMenu, item, fByPosition, lpmii);
}

// Taskbar subclass
LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
    if (uMsg == g_subclassMsg && !wParam) {
        RemoveWindowSubclass(hWnd, TaskbarSubclassProc, 0);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_INITMENUPOPUP) {
        HMENU hMenu = (HMENU)wParam;
        ApplyClassicMenu(hMenu);

        if (settings.showIcons) {
            HWND notifyWnd = FindWindowEx(hWnd, NULL, L"TrayNotifyWnd", NULL);
            HWND clockWnd = FindWindowEx(notifyWnd, NULL, L"TrayClockWClass", NULL);

            RECT rect;
            POINT pt;
            GetWindowRect(clockWnd, &rect);
            GetCursorPos(&pt);

            if (pt.x > rect.left && pt.x < rect.right && pt.y > rect.top && pt.y < rect.bottom) {
                ApplyMenuIcons(hMenu, 413, 420, 407);
            } else {
                ApplyMenuIcons(hMenu, 414, 421, 408);
                ApplyMenuIcons(hMenu, 413, 420, 407);
                ApplyMenuIcons(hMenu, 0x019E, 0x01A5, 0x0198);
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK SecondaryTaskbarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
    if (uMsg == g_subclassMsg && !wParam) {
        RemoveWindowSubclass(hWnd, SecondaryTaskbarSubclassProc, 0);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_INITMENUPOPUP) {
        ApplyClassicMenu((HMENU)wParam);
        ApplyMenuIcons((HMENU)wParam, 0x19D, 0x1A4, 0x197);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// TrackPopupMenu hooks
using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t pOriginalTrackPopupMenu;

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, CONST RECT* prcRect) {
    if (IsWindow(hWnd)) {
        char cls[64];
        GetClassNameA(hWnd, cls, sizeof(cls));
        if (!strcmp(cls, "TrayShowDesktopButtonWClass") ||
            !strcmp(cls, "NotificationsMenuOwner") ||
            !strcmp(cls, "LauncherTipWnd") ||
            !strcmp(cls, "MultitaskingViewFrame") ||
            hWnd == g_atlWnd1 || hWnd == g_atlWnd2) {
            ApplyClassicMenu(hMenu);
            if (!strcmp(cls, "TrayShowDesktopButtonWClass") && settings.showIcons)
                SetMenuItemBitmaps(hMenu, 0x1A2D, MF_BYCOMMAND, g_icons[MYICON_SHOWDESKTOP], g_icons[MYICON_SHOWDESKTOP]);
        }
    }
    return pOriginalTrackPopupMenu(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
}

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t pOriginalTrackPopupMenuEx;

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) {
    if (IsWindow(hWnd)) {
        char cls[64];
        GetClassNameA(hWnd, cls, sizeof(cls));
        if (!strcmp(cls, "TrayShowDesktopButtonWClass") ||
            !strcmp(cls, "NotificationsMenuOwner") ||
            !strcmp(cls, "LauncherTipWnd") ||
            !strcmp(cls, "MultitaskingViewFrame") ||
            hWnd == g_atlWnd1 || hWnd == g_atlWnd2) {
            ApplyClassicMenu(hMenu);
            if (!strcmp(cls, "TrayShowDesktopButtonWClass") && settings.showIcons)
                SetMenuItemBitmaps(hMenu, 0x1A2D, MF_BYCOMMAND, g_icons[MYICON_SHOWDESKTOP], g_icons[MYICON_SHOWDESKTOP]);
        }
    }
    return pOriginalTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
}

// Find ATL windows for network/volume
BOOL CALLBACK EnumATLWindows(HWND hwnd, LPARAM lParam) {
    if (GetWindowThreadProcessId(hwnd, NULL) == (DWORD)lParam) {
        char cls[8];
        GetClassNameA(hwnd, cls, sizeof(cls));
        if (cls[0] == 'A' && cls[1] == 'T' && cls[2] == 'L') {
            if (GetWindowTextLengthA(hwnd) == 0)
                g_atlWnd1 = hwnd;
            else
                g_atlWnd2 = hwnd;
        }
    }
    return TRUE;
}

void SubclassTaskbars() {
    g_taskBar = FindWindowW(L"Shell_TrayWnd", NULL);
    if (IsWindow(g_taskBar)) {
        g_subclassedWindows.insert(g_taskBar);
        SetWindowSubclass(g_taskBar, TaskbarSubclassProc, 0, 0);
    }

    g_taskBarSecondary = FindWindowW(L"Shell_SecondaryTrayWnd", NULL);
    if (IsWindow(g_taskBarSecondary)) {
        g_subclassedWindows.insert(g_taskBarSecondary);
        SetWindowSubclass(g_taskBarSecondary, SecondaryTaskbarSubclassProc, 0, 0);
    }

    HWND pniWnd = FindWindowA("PNIHiddenWnd", NULL);
    if (pniWnd)
        EnumWindows(EnumATLWindows, GetWindowThreadProcessId(pniWnd, NULL));
}

LRESULT CALLBACK ShellWindowSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
    if (uMsg == g_subclassMsg && !wParam) {
        RemoveWindowSubclass(hWnd, ShellWindowSubclassProc, 0);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_TASKBARCREATED)
        SubclassTaskbars();

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void LoadSettings() {
    settings.showIcons = Wh_GetIntSetting(L"showIcons");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();
    LoadIcons();

    Wh_SetFunctionHook((void*)TrackPopupMenu, (void*)TrackPopupMenu_Hook, (void**)&pOriginalTrackPopupMenu);
    Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&pOriginalTrackPopupMenuEx);
    Wh_SetFunctionHook((void*)InsertMenuItemW, (void*)InsertMenuItemW_Hook, (void**)&pOriginalInsertMenuItemW);
    Wh_SetFunctionHook((void*)SetMenuItemInfoW, (void*)SetMenuItemInfoW_Hook, (void**)&pOriginalSetMenuItemInfoW);

    HWND shellWnd = GetShellWindow();
    if (shellWnd) {
        g_subclassedWindows.insert(shellWnd);
        SetWindowSubclass(shellWnd, ShellWindowSubclassProc, 0, 0);
        SubclassTaskbars();
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    for (HWND hWnd : g_subclassedWindows)
        SendMessage(hWnd, g_subclassMsg, FALSE, 0);

    FreeIcons();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
