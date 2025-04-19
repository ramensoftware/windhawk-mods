// ==WindhawkMod==
// @id              chinese-ime-fixed-mode
// @name            Chinese IME fixed mode
// @description     Forces Microsoft Pinyin IME to stay in Chinese mode
// @version         1.0.4
// @author          barry
// @github          https://github.com/barrypp
// @include         *
// @compilerOptions -limm32
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Force Chinese IME Mode | 强制中文输入法
## base on https://windhawk.net/mods/force-chinese-ime v1.0.1

set_IME_mode trigger by MSUIM.Msg.Private

## Feature | 功能
This mod forces the Microsoft Pinyin IME to stay in Chinese input mode, preventing unwanted mode switches.  
这个 mod 强制微软拼音输入法保持在中文输入模式，防止出现不必要的模式切换。
### Before | 使用前
The input mode often switches irregular while using Microsoft Pinyin IME.  
使用微软拼音输入法时输入模式经常无规则切换。
### After | 使用后
It can ensure that when using the Microsoft Pinyin IME, English will never be entered using the input box.  
可以确保在微软拼音输入法的时候，使用输入框永远不会输入英文。
*/
// ==/WindhawkModReadme==

#include <imm.h>
#include <minwindef.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>

// Define IME control constants if not defined
#ifndef IMC_SETCONVERSIONMODE
#define IMC_SETCONVERSIONMODE 0x0002
#endif

UINT WM_MSUIM_Msg_Private = RegisterWindowMessageW(L"MSUIM.Msg.Private");
void set_IME_mode()
{
    HKL hkl = GetKeyboardLayout(0);
    if (((UINT_PTR)hkl & 0x0000FFFF) == 0x0804) {
        HWND foreground = GetForegroundWindow();
        if (foreground) {
            HWND ime_hwnd = ImmGetDefaultIMEWnd(foreground);
            if (ime_hwnd) {
                SendMessageW(ime_hwnd, WM_IME_CONTROL, IMC_SETCONVERSIONMODE, IME_CMODE_CHINESE );//IME_CMODE_ALPHANUMERIC //IME_CMODE_CHINESE
                SendMessageW(ime_hwnd, WM_IME_NOTIFY, IMN_SETCONVERSIONMODE, 0);        
                //Wh_Log(L"set_IME_mode done");
            }
        }  
    }
}


void do_IME_mode(CONST MSG* lpMsg)
{
    // if (lpMsg->message != 0x0060 &&
    //     lpMsg->message != 0x0113 &&
    //     lpMsg->message != 0x0200 &&
    //     lpMsg->message != 0x0118 &&
    //     lpMsg->message != 0x05fd
    //     ) {
    //     Wh_Log(L"MSG, %x",lpMsg->message);
    // }

    // if (lpMsg->message == 0xc05a
    //     ) {
    //     Wh_Log(L"msg, %x, wP, %x, lP, %x",lpMsg->message,lpMsg->wParam,lpMsg->lParam);
    // } 

    if (lpMsg->message == WM_IME_NOTIFY) 
    {
        //set_IME_mode();
        Wh_Log(L"WM_IME_NOTIFY msg, %x, wP, %x, lP, %x",lpMsg->message,lpMsg->wParam,lpMsg->lParam);
    }  
    if (lpMsg->message == WM_MSUIM_Msg_Private && lpMsg->wParam == 0x250013)
    {
        set_IME_mode();
        Wh_Log(L"set_IME_mode");
    }     
    if (lpMsg->message == WM_MSUIM_Msg_Private)
    {
        Wh_Log(L"WM_MSUIM_Msg_Private wP, %x, lP, %x",lpMsg->wParam,lpMsg->lParam);  
    } 
}


// using SetFocus_t = decltype(&SetFocus);
// SetFocus_t SetFocus_Original;
// HWND WINAPI SetFocus_Hook(
//     HWND hWnd
// ) {
//     set_IME_mode();
//     return SetFocus_Original(hWnd);
// }

using DispatchMessageA_t = decltype(&DispatchMessageA);
DispatchMessageA_t DispatchMessageA_Original;
LRESULT WINAPI DispatchMessageA_Hook(CONST MSG* lpMsg) 
{
    do_IME_mode(lpMsg);
    return DispatchMessageA_Original(lpMsg);
}

using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t DispatchMessageW_Original;
LRESULT WINAPI DispatchMessageW_Hook(CONST MSG* lpMsg)
{
    do_IME_mode(lpMsg);         
    return DispatchMessageW_Original(lpMsg);
}

// using RegisterWindowMessageW_t = decltype(&RegisterWindowMessageW);
// RegisterWindowMessageW_t RegisterWindowMessageW_Original;
// UINT WINAPI RegisterWindowMessageW_Hook(LPCWSTR lpString) 
// {
//     UINT r = RegisterWindowMessageW_Original(lpString);
//     if (0 == lstrcmpW(lpString,L"MSUIM.Msg.Private"))
//     {
//         Wh_Log(L"r, %x, msg, %s",r,lpString);
//         WM_MSUIM_Msg_Private = r;
//         //Wh_RemoveFunctionHook((void*)RegisterWindowMessageW);
//     }
//     return r;
// }

void Wh_ModUninit()
{
}

// Module initialization
BOOL Wh_ModInit() {
    WindhawkUtils::SetFunctionHook(
        DispatchMessageA,
        DispatchMessageA_Hook,
        &DispatchMessageA_Original // Pass original function pointer here
    );    

    WindhawkUtils::SetFunctionHook(
        DispatchMessageW,
        DispatchMessageW_Hook,
        &DispatchMessageW_Original // Pass original function pointer here
    );    

    // WindhawkUtils::SetFunctionHook(
    //     RegisterWindowMessageW,
    //     RegisterWindowMessageW_Hook,
    //     &RegisterWindowMessageW_Original // Pass original function pointer here
    // );        

    //Wh_Log(L"WindhawkUtils::SetFunctionHook, r, %d", r);
    return TRUE;
}
