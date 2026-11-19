// ==WindhawkMod==
// @id                  disable-taskmgr-navigation-border
// @name                Disable Task Manager Navigation Border
// @name:zh-CN          禁用任务管理器的导航边框
// @description         Disable Task Manager navigation and generic focus borders.
// @description:zh-CN   禁用任务管理器的导航边框和焦点边框。
// @version             0.1
// @author              GF-Huang
// @github              https://github.com/GF-Huang
// @include             Taskmgr.exe
// @architecture        x86-64
// @license             MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Task Manager Navigation Border

![Compared](https://i.imgur.com/00TkdHI.png)

Disables the ugly focus borders in Windows 11 Task Manager.

It works by replacing the following functions with empty implementations:

- `AtmView::SetBorderUponNavigationForColumnHeader`
- `AtmView::SetBorderUponNavigationForListRow`
- `AtmView::SetBorderUponNavigationForListRowWithColumnTraversal`
- `SetBorderUponNavigatingIntoElement`

Functions are located using symbols for the running Taskmgr.exe version, not
fixed addresses. First use may require internet access to download symbols.
Initialization fails if any required symbol is unavailable.

This MOD targets x64 Task Manager versions providing these methods. No system
files or signatures are modified. 

------------------------------------

禁用 Windows 11 任务管理器上丑陋的焦点边框。

实现原理是将以下方法替换为空实现：

- `AtmView::SetBorderUponNavigationForColumnHeader`
- `AtmView::SetBorderUponNavigationForListRow`
- `AtmView::SetBorderUponNavigationForListRowWithColumnTraversal`
- `SetBorderUponNavigatingIntoElement`

这些函数是通过当前运行的 Taskmgr.exe 版本的符号信息进行定位，而非固定内存地址。首次使用可能需要联网下载符号文件。若缺少任何所需符号，初始化将失败。

此 MOD 适用于包含上述方法的 x64 版本任务管理器。不会修改任何系统文件或签名。
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

namespace {

// x64 non-static member functions receive the AtmView* in RCX.
using SetBorder_t = void(__cdecl *)(void *view);

SetBorder_t SetBorderUponNavigationForColumnHeader_Original;
SetBorder_t SetBorderUponNavigationForListRow_Original;
SetBorder_t SetBorderUponNavigationForListRowWithColumnTraversal_Original;
SetBorder_t SetBorderUponNavigatingIntoElement_Original;

// Intentionally do nothing and never call the original functions.
void __cdecl SetBorderUponNavigationForColumnHeader_Hook(void * /*view*/) {}
void __cdecl SetBorderUponNavigationForListRow_Hook(void * /*view*/) {}
void __cdecl SetBorderUponNavigationForListRowWithColumnTraversal_Hook(void * /*view*/) {}
void __cdecl SetBorderUponNavigatingIntoElement_Hook(void * /*element*/) {}

} // namespace

BOOL Wh_ModInit() {
    HMODULE taskmgr = GetModuleHandleW(nullptr);
    if (!taskmgr) {
        Wh_Log(L"Failed to get the Taskmgr.exe module");
        return FALSE;
    }

     // taskmgr.exe
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            { L"private: void __cdecl AtmView::SetBorderUponNavigationForColumnHeader(void)" },
            &SetBorderUponNavigationForColumnHeader_Original,
            SetBorderUponNavigationForColumnHeader_Hook,
        },
        {
            { L"private: void __cdecl AtmView::SetBorderUponNavigationForListRow(void)" },
            &SetBorderUponNavigationForListRow_Original,
            SetBorderUponNavigationForListRow_Hook,
        },
        {
            { L"private: void __cdecl AtmView::SetBorderUponNavigationForListRowWithColumnTraversal(void)" },
            &SetBorderUponNavigationForListRowWithColumnTraversal_Original,
            SetBorderUponNavigationForListRowWithColumnTraversal_Hook,
        },
        {
            { L"void __cdecl SetBorderUponNavigatingIntoElement(class DirectUI::Element *)" },
            &SetBorderUponNavigatingIntoElement_Original,
            SetBorderUponNavigatingIntoElement_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(taskmgr, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to resolve/hook the required Task Manager border methods");
        return FALSE;
    }

    Wh_Log(L"Hooks installed");
    return TRUE;
}
