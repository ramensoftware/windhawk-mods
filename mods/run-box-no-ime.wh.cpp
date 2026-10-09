// ==WindhawkMod==
// @id          run-box-no-ime
// @name        Disable IME in Run Dialog
// @name:zh-CN  运行对话框自动关闭输入法
// @description Disable IME automatically when typing in the Run dialog, to prevent accidental input of non-English characters.
// @description:zh-CN  在资源管理器 Win+R 运行对话框输入时自动禁用输入法, 防止意外输入中文等非英文字符.
// @version     1.0
// @author      Joe Ye
// @github      https://github.com/JoeYe-233
// @include     explorer.exe
// @compilerOptions -limm32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
When typing in the Run dialog, disable IME (Input Method Editor) automatically to prevent accidental input of non-English characters.  
So you can type commands and hit `Enter` *confidently*, never have to worry about IME messing things up again.

*Bringing you an elegant, neat and streamlined Win+R experience.*
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <imm.h>

using SetFocus_t = HWND (WINAPI *)(HWND);
SetFocus_t Real_SetFocus;

HWND WINAPI My_SetFocus(HWND hWnd) {
    HWND hResult = Real_SetFocus(hWnd);

    if (hWnd) {
        WCHAR className[256];
        GetClassNameW(hWnd, className, ARRAYSIZE(className));

        // 1. Ensure the focused window is an Edit control
        if (wcscmp(className, L"Edit") == 0) {
            HWND hCombo = GetParent(hWnd);
            if (hCombo) {
                WCHAR comboClass[256];
                GetClassNameW(hCombo, comboClass, ARRAYSIZE(comboClass));

                // 2. Ensure the parent window is a ComboBox
                if (wcscmp(comboClass, L"ComboBox") == 0) {
                    HWND hDialog = GetParent(hCombo);
                    if (hDialog) {
                        WCHAR dialogClass[256];
                        GetClassNameW(hDialog, dialogClass, ARRAYSIZE(dialogClass));

                        // 3. Ensure the grandparent window is #32770
                        if (wcscmp(dialogClass, L"#32770") == 0) {
                            // Disassociate the Edit control from the IME context.
                            ImmAssociateContext(hWnd, NULL);
                        }
                    }
                }
            }
        }
    }
    return hResult;
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void *)SetFocus, (void *)My_SetFocus, (void **)&Real_SetFocus);
    return TRUE;
}