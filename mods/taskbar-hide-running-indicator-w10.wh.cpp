// ==WindhawkMod==
// @id              taskbar-hide-running-indicator-w10
// @name            Hide running indicator for Windows 10
// @description     Hides taskbar running indicator for Windows 10
// @version         1.0.1
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

![After](https://i.imgur.com/ZbuRRXu.png)

![After](https://i.imgur.com/NSpjnHc.png)

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

typedef struct tagBUTTONRENDERINFO {
    unsigned char data[0x94]; // Unknown data
    RECT r1; // Size of the main running indicator rectangle 
    RECT r2; // Size of the additional running indicators
    RECT r3;
} BUTTONRENDERINFO, *PBUTTONRENDERINFO;

/* Compute sizes of running indicator rectangles */
typedef void (* CALCON CTaskBtnGroup__ComputeRenderPropsBar_t)(void *,  void *);
CTaskBtnGroup__ComputeRenderPropsBar_t CTaskBtnGroup__ComputeRenderPropsBar_orig;
void CALCON CTaskBtnGroup__ComputeRenderPropsBar_hook(
    void *pThis,
    PBUTTONRENDERINFO pRenderInfo
)
{
    PBUTTONRENDERINFO p = pRenderInfo;
    CTaskBtnGroup__ComputeRenderPropsBar_orig(pThis,  pRenderInfo);

    // Set rectanges to be zero width/height as appropriate.
    // Empty rectangles do not work, because then focus indicator is not drawn

    // Deduce where taskbar is based on indicator size ratio,
    // If it's longer than taller that means it's on top or botton, else it's on the side
    if ( (p->r1.bottom - p->r1.top) < (p->r1.right - p->r1.left) ) //top or bottom
    {
        if (p->r1.top == 0)
        {
            // Taskbar on the top
            p->r1.bottom = p->r1.top;
            p->r2.bottom = p->r2.top;
            p->r3.bottom = p->r3.top;
        }
        else {
            // Taskbar on the bottom
            p->r1.top = p->r1.bottom;
            p->r2.top = p->r2.bottom;
            p->r3.top = p->r3.bottom;
        }
    }
    else //side
    {
        if (p->r1.left == 0)
        {
            // Taskbar on the left
            p->r1.right = p->r1.left;
            p->r2.right = p->r2.left;
            p->r3.right = p->r3.left;
        }
        else {
            // Taskbar on the right
            p->r1.left = p->r1.right;
            p->r2.left = p->r2.right;
            p->r3.left = p->r3.right;
        }

    }
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
                L" CTaskBtnGroup::_ComputeRenderPropsBar(struct BUTTONRENDERINFO &)"
            },
            (void **)&CTaskBtnGroup__ComputeRenderPropsBar_orig,
            (void *)CTaskBtnGroup__ComputeRenderPropsBar_hook,
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
