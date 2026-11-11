// ==WindhawkMod==
// @id win10-taskbar-context-menu-fix-24h2
// @name Windows 10 Taskbar Context Menu Fix for Win11 24H2+
// @description Fixes context menu on Windows 10 taskbar running on Windows 11 24H2, 25H2 and later
// @version 1.4
// @author Anixx
// @github          https://github.com/Anixx
// @architecture    x86-64
// @include explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
The taskbar context menu (when clicking on the taskbar empty area) does not appear when
running the Windows 10 taskbar on Windows 11 24H2, Windows 11 25H2 and later.
This mod restores the menu.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>

static HMODULE g_explorerModule;
static HMODULE g_shell32Module;
static HMODULE g_bthpropsModule;
static HMODULE g_explorerframeModule;

#define IDM_SHOWDESKTOP     0x197
#define IDM_TASKMANAGER     0x1A4
#define IDM_LOCKTASKBAR     0x1A8
#define IDM_SETTINGS        0x19D
#define IDM_LOCKTOOLBARS    41484

#define IDS_SHOWDESKTOP     10113  // shell32.dll
#define IDS_TASKMANAGER     24743  // shell32.dll
#define IDS_SETTINGS        2128   // bthprops.cpl

#define TASKMGR_ITEM_MAGIC  0x5441534B // 'TASK'

using LoadMenuW_t = decltype(&LoadMenuW);
LoadMenuW_t LoadMenuW_Original;

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_Original;

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;

static thread_local bool g_inTrackPopupMenu = false;

static void LaunchTaskManager() {
    // taskmgr.exe запрашивает highestAvailable, поэтому CreateProcessW для
    // администратора падает с ERROR_ELEVATION_REQUIRED. ShellExecuteW с полным
    // путём покрывает оба случая.
    wchar_t szPath[MAX_PATH];
    UINT len = GetSystemDirectoryW(szPath, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
        return;
    }
    if (wcscat_s(szPath, L"\\taskmgr.exe") != 0) {
        return;
    }
    ShellExecuteW(nullptr, nullptr, szPath, nullptr, nullptr, SW_SHOWNORMAL);
}

static wchar_t* LoadStr(HMODULE hMod, UINT id, wchar_t* buf, int size) {
    if (LoadStringW(hMod, id, buf, size) > 0) return buf;
    return nullptr;
}

static wchar_t* GetLockToolbarsText(wchar_t* buf, int size) {
    // Пытаемся получить текст из explorerframe.dll menu resource
    if (g_explorerframeModule) {
        HMENU hMenu = LoadMenuW_Original(g_explorerframeModule, MAKEINTRESOURCEW(264));
        if (hMenu) {
            // Ищем в меню пункт с ID 41484
            for (int i = 0; i < GetMenuItemCount(hMenu); i++) {
                MENUITEMINFOW mii = {0};
                mii.cbSize = sizeof(mii);
                mii.fMask = MIIM_SUBMENU;

                if (GetMenuItemInfoW(hMenu, i, TRUE, &mii) && mii.hSubMenu) {
                    for (int j = 0; j < GetMenuItemCount(mii.hSubMenu); j++) {
                        MENUITEMINFOW subMii = {0};
                        subMii.cbSize = sizeof(subMii);
                        subMii.fMask = MIIM_ID | MIIM_STRING;
                        wchar_t text[256] = {0};
                        subMii.dwTypeData = text;
                        subMii.cch = 255;

                        if (GetMenuItemInfoW(mii.hSubMenu, j, TRUE, &subMii) &&
                            subMii.wID == IDM_LOCKTOOLBARS) {
                            wcsncpy_s(buf, size, text, _TRUNCATE);
                            DestroyMenu(hMenu);
                            return buf;
                        }
                    }
                }
            }
            DestroyMenu(hMenu);
        }
    }

    // Fallback на английский
    return nullptr;
}

static void EnhanceTaskbarMenu(HMENU hMenu) {
    HMENU hPopup = GetSubMenu(hMenu, 0);
    if (!hPopup) hPopup = hMenu;

    for (int i = GetMenuItemCount(hPopup) - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {sizeof(mii), MIIM_FTYPE | MIIM_ID | MIIM_SUBMENU};
        if (GetMenuItemInfoW(hPopup, i, TRUE, &mii) &&
            !(mii.fType & MFT_SEPARATOR) && !mii.hSubMenu) {
            DeleteMenu(hPopup, i, MF_BYPOSITION);
        }
    }

    wchar_t buf[256];

    wchar_t* str = LoadStr(g_shell32Module, IDS_SHOWDESKTOP, buf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_SHOWDESKTOP, str ? str : L"Show the desktop");

    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    str = LoadStr(g_shell32Module, IDS_TASKMANAGER, buf, 256);
    MENUITEMINFOW miiTaskMgr = { sizeof(miiTaskMgr) };
    miiTaskMgr.fMask = MIIM_ID | MIIM_STRING | MIIM_DATA;
    miiTaskMgr.wID = IDM_TASKMANAGER;
    miiTaskMgr.dwItemData = TASKMGR_ITEM_MAGIC;
    miiTaskMgr.dwTypeData = str ? str : (LPWSTR)L"Task Manager";
    InsertMenuItemW(hPopup, GetMenuItemCount(hPopup), TRUE, &miiTaskMgr);

    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    // Получаем локализованный текст для "Lock the toolbars"
    wchar_t lockBuf[256];
    str = GetLockToolbarsText(lockBuf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_LOCKTASKBAR, str ? str : L"Lock the taskbar");

    str = LoadStr(g_bthpropsModule, IDS_SETTINGS, buf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_SETTINGS, str ? str : L"Taskbar settings");
}

// Окно-владелец меню должно быть панелью задач (основной или на дополнительном мониторе).
static bool IsTaskbarWindow(HWND hWnd) {
    if (!hWnd) return false;
    wchar_t cls[32];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    return wcscmp(cls, L"Shell_TrayWnd") == 0 ||
           wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

// Находит актуальный ID пункта диспетчера задач (с учётом смещения после
// Shell_MergeMenus, который копирует item data вместе со смещённым ID).
// Пункт определяется только по маркеру.
static UINT FindTaskManagerCmdId(HMENU hMenu) {
    if (!hMenu) return 0;
    int count = GetMenuItemCount(hMenu);
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii = { sizeof(mii) };
        mii.fMask = MIIM_ID | MIIM_DATA;
        if (GetMenuItemInfoW(hMenu, i, TRUE, &mii) &&
            mii.dwItemData == TASKMGR_ITEM_MAGIC) {
            return mii.wID;
        }
    }
    return 0;
}

// Общая логика для TrackPopupMenu / TrackPopupMenuEx.
static UINT GetTaskManagerIdForMenu(HMENU hMenu, HWND hWnd) {
    if (!IsTaskbarWindow(hWnd)) return 0;
    return FindTaskManagerCmdId(hMenu);
}

HMENU WINAPI LoadMenuW_Hook(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    if (IS_INTRESOURCE(lpMenuName) && (HMODULE)hInstance == g_explorerModule) {
        UINT menuId = (UINT)(ULONG_PTR)lpMenuName;
        if ((menuId == 205 || menuId == 206) && g_shell32Module) {
            HMENU result = LoadMenuW_Original(g_shell32Module, MAKEINTRESOURCEW(205));
            if (result) {
                EnhanceTaskbarMenu(result);
                return result;
            }
        }
    }
    return LoadMenuW_Original(hInstance, lpMenuName);
}

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT *prcRect) {
    if (g_inTrackPopupMenu) {
        return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    }

    UINT taskMgrId = GetTaskManagerIdForMenu(hMenu, hWnd);
    bool addedReturnCmd = false;
    if (taskMgrId != 0 && !(uFlags & TPM_RETURNCMD)) {
        uFlags |= TPM_RETURNCMD;
        addedReturnCmd = true;
    }

    g_inTrackPopupMenu = true;
    BOOL ret = TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    g_inTrackPopupMenu = false;

    if (taskMgrId != 0) {
        if ((UINT)ret == taskMgrId) {
            LaunchTaskManager();
            return addedReturnCmd ? TRUE : 0;
        } else if (addedReturnCmd) {
            if (ret != 0) {
                PostMessageW(hWnd, WM_COMMAND, MAKEWPARAM(ret, 0), 0);
            }
            // Без TPM_RETURNCMD закрытие меню без выбора — не ошибка.
            return TRUE;
        }
    }

    return ret;
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) {
    if (g_inTrackPopupMenu) {
        return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
    }

    UINT taskMgrId = GetTaskManagerIdForMenu(hMenu, hWnd);
    bool addedReturnCmd = false;
    if (taskMgrId != 0 && !(uFlags & TPM_RETURNCMD)) {
        uFlags |= TPM_RETURNCMD;
        addedReturnCmd = true;
    }

    g_inTrackPopupMenu = true;
    BOOL ret = TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
    g_inTrackPopupMenu = false;

    if (taskMgrId != 0) {
        if ((UINT)ret == taskMgrId) {
            LaunchTaskManager();
            return addedReturnCmd ? TRUE : 0;
        } else if (addedReturnCmd) {
            if (ret != 0) {
                PostMessageW(hWnd, WM_COMMAND, MAKEWPARAM(ret, 0), 0);
            }
            return TRUE;
        }
    }

    return ret;
}

BOOL Wh_ModInit() {
    g_explorerModule = GetModuleHandleW(nullptr);
    g_shell32Module = GetModuleHandleW(L"shell32.dll");
    g_bthpropsModule = LoadLibraryExW(L"bthprops.cpl", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    g_explorerframeModule = GetModuleHandleW(L"explorerframe.dll");

    Wh_SetFunctionHook((void*)LoadMenuW, (void*)LoadMenuW_Hook, (void**)&LoadMenuW_Original);
    Wh_SetFunctionHook((void*)TrackPopupMenu, (void*)TrackPopupMenu_Hook, (void**)&TrackPopupMenu_Original);
    Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&TrackPopupMenuEx_Original);

    return TRUE;
}

void Wh_ModUninit() {
    if (g_bthpropsModule) FreeLibrary(g_bthpropsModule);
}
