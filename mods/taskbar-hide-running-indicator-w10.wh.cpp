// ==WindhawkMod==
// @id              taskbar-hide-running-indicator-w10
// @name            Hide running indicator for Windows 10
// @description     Hides taskbar running indicator for Windows 10
// @version         1.0.0
// @author          giedriuslt
// @github          https://github.com/giedriuslt
// @include         explorer.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide taskbar running indicator for Windows 10
Lightweight mod which hides the running indicator on taskbar buttons. For Windows 11, use taskbar styler to achieve same effect.

Before:

![Before](https://i.imgur.com/bbUWHuX.png)

![Before](https://i.imgur.com/gT1rgVv.png)

After:

![After](https://i.imgur.com/jBNOYMn.png)

![After](https://i.imgur.com/DxKL5Od.png)

There is a minor imperfection in that the focus rectangle is not covering the full size.

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#ifdef _WIN64
#define CALCON __cdecl
#define SCALCON L"__cdecl"
#else
#define CALCON __thiscall
#define SCALCON L"__thiscall"
#endif

typedef struct tagBUTTONRENDERINFOSTATES {
    char data[12];
} BUTTONRENDERINFOSTATES, *PBUTTONRENDERINFOSTATES;

typedef struct tagBUTTONRENDERINFO {
    char data[60];
} BUTTONRENDERINFO, *PBUTTONRENDERINFO;

/* Draw taskbar item */
typedef void (* CTaskBtnGroup__DrawBar_t)(void *, HDC, void *, void *);
CTaskBtnGroup__DrawBar_t CTaskBtnGroup__DrawBar_orig;
void CALCON CTaskBtnGroup__DrawBar_hook(
    void *pThis,
    HDC hDC,
    PBUTTONRENDERINFO pRenderInfo,
    PBUTTONRENDERINFOSTATES pRenderStates
)
{
    //do not draw indicator bar
    return;
}

BOOL Wh_ModInit(void)
{
    HMODULE hExplorer = GetModuleHandleW(NULL);

    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {
                L"private: void " 
                SCALCON 
                L" CTaskBtnGroup::_DrawBar(struct HDC__ *,struct BUTTONRENDERINFO const &,struct BUTTONRENDERINFOSTATES const &)"
            },
            (void **)&CTaskBtnGroup__DrawBar_orig,
            (void *)CTaskBtnGroup__DrawBar_hook,
            FALSE
        }
    };

    if (!WindhawkUtils::HookSymbols(hExplorer, explorerExeHooks, ARRAYSIZE(explorerExeHooks)))
    {
        Wh_Log(L"Failed to hook one or more functions");
        return FALSE;
    }
    Wh_Log(L"Mod init complete");
    return TRUE;
}
