// ==WindhawkMod==
// @id              force-chinese-ime
// @name            Force Chinese IME Mode
// @description     Forces Microsoft Pinyin IME to stay in Chinese mode
// @version         1.0.1
// @author          u3l6
// @github          https://github.com/u3l6
// @twitter         https://x.com/u_3l6
// @include         *
// @compilerOptions -limm32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Force Chinese IME Mode | 强制中文输入法

## ⚠️This mod is a work in progress and features may not work. It may change a lot in the future, so there is no need to provide feedback now.⚠️
## ⚠️该mod仍在开发中，可能无法实现功能。它以后可能会有重大变化，所以目前不需要提供反馈。⚠️

## Feature | 功能
This mod forces the Microsoft Pinyin IME to stay in Chinese input mode, preventing unwanted mode switches.  
这个 mod 强制微软拼音输入法保持在中文输入模式，防止出现不必要的模式切换。
### Before | 使用前
The input mode often switches irregular while using Microsoft Pinyin IME.  
使用微软拼音输入法时输入模式经常无规则切换。
![before](https://raw.githubusercontent.com/u3l6/force-chinese-ime/refs/heads/main/Before.png)
### After | 使用后
It can ensure that when using the Microsoft Pinyin IME, English will never be entered using the input box.  
可以确保在微软拼音输入法的时候，使用输入框永远不会输入英文。
![after](https://raw.githubusercontent.com/u3l6/force-chinese-ime/refs/heads/main/After.png)
*/
// ==/WindhawkModReadme==

#include <imm.h>
#include <windows.h>

// Define IME control constants if not defined
#ifndef IMC_SETCONVERSIONMODE
#define IMC_SETCONVERSIONMODE 0x0002
#endif

#ifndef IMC_OPENSTATUSWINDOW
#define IMC_OPENSTATUSWINDOW 0x0021
#endif

// Hook function prototype
using ImmSetConversionStatus_t = decltype(&ImmSetConversionStatus);
ImmSetConversionStatus_t ImmSetConversionStatus_Original;

// Conversion mode masks
const DWORD CONVERSION_MODE_MASK = IME_CMODE_ALPHANUMERIC | 
                                    IME_CMODE_NATIVE |
                                    IME_CMODE_KATAKANA |
                                    IME_CMODE_LANGUAGE |
                                    IME_CMODE_FULLSHAPE |
                                    IME_CMODE_ROMAN |
                                    IME_CMODE_CHARCODE |
                                    IME_CMODE_HANJACONVERT |
                                    IME_CMODE_SOFTKBD |
                                    IME_CMODE_NOCONVERSION |
                                    IME_CMODE_EUDC |
                                    IME_CMODE_SYMBOL |
                                    IME_CMODE_FIXED;

// Chinese IME mode constants
const DWORD CHN_FULL_SHAPE = 0x0008;  // Full-width mode
const DWORD CHN_NATIVE = 0x0001;      // Chinese mode
const DWORD FORCE_CHINESE_MODE = CHN_NATIVE | CHN_FULL_SHAPE;

// Hook IME conversion status changes
BOOL WINAPI ImmSetConversionStatus_Hook(
    HIMC hIMC,
    DWORD fdwConversion,
    DWORD fdwSentence
) {
    // Get current keyboard layout
    HKL hkl = GetKeyboardLayout(0);
    if (((UINT_PTR)hkl & 0x0000FFFF) == 0x0804) {
        // Force Chinese input mode:
        fdwConversion &= ~CONVERSION_MODE_MASK;
        fdwConversion |= IME_CMODE_NATIVE | IME_CMODE_CHINESE;
        
        HWND foreground = GetForegroundWindow();
        if (foreground) {
            HWND ime_hwnd = ImmGetDefaultIMEWnd(foreground);
            if (ime_hwnd) {
                SendMessageW(ime_hwnd, WM_IME_CONTROL, IMC_SETCONVERSIONMODE, FORCE_CHINESE_MODE);
                SendMessageW(ime_hwnd, WM_IME_NOTIFY, IMN_SETCONVERSIONMODE, 0);
            }
        }
        Wh_Log(L"Forcing Chinese IME mode: conversion=0x%x", fdwConversion);
    }
    return ImmSetConversionStatus_Original(hIMC, fdwConversion, fdwSentence);
}

// Module initialization
void ModInit() {
    // Hook the ImmSetConversionStatus function
    Wh_SetFunctionHook(
        (void*)ImmSetConversionStatus,
        (void*)ImmSetConversionStatus_Hook,
        (void**)&ImmSetConversionStatus_Original // Pass original function pointer here
    );

    Wh_Log(L"Force Chinese IME Mode mod initialized.");
}
