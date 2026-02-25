// ==WindhawkMod==
// @id win10-taskbar-context-menu-fix-24h2
// @name Windows 10 Taskbar Context Menu Fix for Win11 24H2+
// @description Fixes context menu on Windows 10 taskbar running on Windows 11 24H2, 25H2 and later
// @version 1.0
// @author Anixx
// @github          https://github.com/Anixx
// @include explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
The taskbar context menu (when clicking on the taskbar empty area) does not appear when
running the Windows 10 taskbar ob Windows 11 24H2 or Windows 11 25H2.
This mod restores the menu. If anyone knows where is stored the localization for
the string "Lock the taskbar", feel free to contact.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>

static HMODULE g_explorerModule;
static HMODULE g_shell32Module;
static HMODULE g_bthpropsModule;

#define IDM_CASCADE         0x193
#define IDM_SIDEBYSIDE      0x194
#define IDM_STACKED         0x195
#define IDM_SHOWDESKTOP     0x197
#define IDM_TASKMANAGER     0x1A4
#define IDM_LOCKTASKBAR     0x1A8
#define IDM_SETTINGS        0x19D

#define IDS_CASCADE         535    // explorer.exe
#define IDS_SIDEBYSIDE      536    // explorer.exe
#define IDS_STACKED         538    // explorer.exe
#define IDS_SHOWDESKTOP     10113  // shell32.dll
#define IDS_TASKMANAGER     24743  // shell32.dll
#define IDS_SETTINGS        2128   // bthprops.cpl

using LoadMenuW_t = decltype(&LoadMenuW);
LoadMenuW_t LoadMenuW_Original;

static wchar_t* LoadStr(HMODULE hMod, UINT id, wchar_t* buf, int size) {
    if (LoadStringW(hMod, id, buf, size) > 0) return buf;
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

    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    wchar_t* str = LoadStr(g_explorerModule, IDS_CASCADE, buf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_CASCADE, str ? str : L"Cascade windows");

    str = LoadStr(g_explorerModule, IDS_SIDEBYSIDE, buf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_SIDEBYSIDE, str ? str : L"Show windows side by side");

    str = LoadStr(g_explorerModule, IDS_STACKED, buf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_STACKED, str ? str : L"Show windows stacked");

    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    str = LoadStr(g_shell32Module, IDS_SHOWDESKTOP, buf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_SHOWDESKTOP, str ? str : L"Show the desktop");

    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    str = LoadStr(g_shell32Module, IDS_TASKMANAGER, buf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_TASKMANAGER, str ? str : L"Task Manager");

    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    AppendMenuW(hPopup, MF_STRING, IDM_LOCKTASKBAR, L"Lock the taskbar");

    str = LoadStr(g_bthpropsModule, IDS_SETTINGS, buf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_SETTINGS, str ? str : L"Taskbar settings");
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

BOOL Wh_ModInit() {
    g_explorerModule = GetModuleHandleW(nullptr);
    g_shell32Module = GetModuleHandleW(L"shell32.dll");
    g_bthpropsModule = LoadLibraryW(L"bthprops.cpl");

    return Wh_SetFunctionHook((void*)LoadMenuW, (void*)LoadMenuW_Hook, (void**)&LoadMenuW_Original);
}

void Wh_ModUninit() {
    if (g_bthpropsModule) FreeLibrary(g_bthpropsModule);
}
