// ==WindhawkMod==
// @id              classic-taskbar-buttons-lite
// @name            Classic Taskbar 3D buttons Lite
// @description     Lightweight mod, restoring the 3D buttons in classic theme
// @version         1.2
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Taskbar 3D buttons
Lightweight mod which restores 3D buttons on taskbar when using Windows Classic theme. 
The idea is based on the mod by Aubymori (https://github.com/aubymori).

Before:

![Before](https://i.imgur.com/jupSjfl.png)

After:

![After](https://i.imgur.com/Jz4EkRQ.png)

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

/* Draw taskbar item */
typedef void (* CTaskBtnGroup__DrawBar_t)(void *, HDC, void *, void *);
CTaskBtnGroup__DrawBar_t CTaskBtnGroup__DrawBar_orig;
void CALCON CTaskBtnGroup__DrawBar_hook(
    void *pThis,
    HDC   hDC,
    void *pRenderInfo,
    PBUTTONRENDERINFOSTATES pRenderStates
)
{
    LPRECT lprcDest = (LPRECT)((char *)pRenderInfo + 4);

    UINT uState = DFCS_BUTTONPUSH;
    if (pRenderStates->data[2])
    {
        uState |= DFCS_CHECKED;
    }
    else if (pRenderStates->data[4])
    {
        uState |= DFCS_PUSHED;
    }
	
	lprcDest->left++;
	lprcDest->right--;

    DrawFrameControl(
        hDC,
        lprcDest,
        DFC_BUTTON,
        uState
    ); 
   

    /* If button is pushed in, offset the rect for the icon and text draw */
    if (pRenderStates->data[2]
    ||  pRenderStates->data[4])
    {
        lprcDest->top++;
        lprcDest->bottom++;
        lprcDest->left++;
        lprcDest->right++;
    }

    return;
}

BOOL Wh_ModInit(void)
{
    HMODULE hExplorer = GetModuleHandleW(NULL);

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                L"private: void " 
                SCALCON 
                L" CTaskBtnGroup::_DrawBar(struct HDC__ *,struct BUTTONRENDERINFO const &,struct BUTTONRENDERINFOSTATES const &)"
            },
            (void **)&CTaskBtnGroup__DrawBar_orig,
            (void *)CTaskBtnGroup__DrawBar_hook,
            FALSE
        },

    };

    if (!WindhawkUtils::HookSymbols(hExplorer, hooks, ARRAYSIZE(hooks)))
    {
        Wh_Log(L"Failed to hook one or more functions");
        return FALSE;
    }

    return TRUE;
}
