// ==WindhawkMod==
// @id              taskmgr-taller-tab
// @name            Make Task Manager Tabs Taller
// @description     Windows 10 Task Manager tab labels are too small on high DPI screens, making them difficult to click, this mod makes them taller.
// @name:zh-CN            增大任务管理器选项卡尺寸
// @description:zh-CN   Windows 10 任务管理器选项卡标签在高分屏上实在太小, 根本不好点击, 安装后会增大选项卡标签的高度.让其更易于使用.
// @version         1.0
// @author          Joe Ye
// @github          https://github.com/JoeYe-233
// @include         Taskmgr.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Make Task Manager Tabs Taller
This mod makes the tab labels in Windows 10 Task Manager taller on high DPI screens, improving clickability.

"An awesome ~~MobileSubstrate~~ WindHawk tweak!!"

![Before-And-After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/taskmgr-taller-tab-before-after.png)

## Features
- Increases the height of the tab labels in Task Manager.
- Enhances the clickability of the tab labels on high DPI screens.
- Retains the original functionality of Task Manager.

## Usage
1. Compile and inject this mod into the Task Manager process.
2. Run Task Manager and observe the taller tab labels.
* No options to configure, simply install and use.

## Author
Joe Ye
*/
// ==/WindhawkModReadme==
#include <Windows.h>
#include <commctrl.h>

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int x, int y,
    int nWidth, int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
)
{
    HWND hWnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    //Ref: windhawk-mods/mods/explorer-name-windows.wh.cpp

    if (bTextualClassName && _wcsicmp(lpClassName, L"SysTabControl32") == 0) {
        //ensuring target control is a tab control "SysTabControl32"
        TabCtrl_SetItemSize(hWnd, 100, 37); // width= 100，height= 37
    }

    return hWnd;
}

template<typename ProtoType>
BOOL Wh_SetFunctionHookT(ProtoType targetFunction, ProtoType hookFunction, ProtoType* originalFunction)
{
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction, (void**)originalFunction);
}

BOOL Wh_ModInit()
{
    // Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    Wh_SetFunctionHookT(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);

    return TRUE;
}
