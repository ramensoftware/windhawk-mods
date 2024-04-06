// ==WindhawkMod==
// @id              classic-taskbar-buttons-lite-vs-without-spacing
// @name            Classic Taskbar 3D buttons with extended compatibility
// @description     Restoring the 3D taskbar buttons with support for visual styles and the taskbar without labels
// @version         1.0
// @author          OrthodoxWin32
// @github          https://github.com/OrthodoxWindows
// @include         explorer.exe
// @compilerOptions -lgdi32 -lUxTheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Taskbar Fix
Restores 3D buttons on taskbar when using Windows Classic theme or Windows Visual Style. Based on mod by Anixx (https://github.com/anixx), itself founded from a mod by Aubymori (https://github.com/aubymori).
Known issue : Under the classic theme, icons have no space between them. It is possible to add space, but this prevents the taskbar from functioning correctly if the labels are hidden.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <uxtheme.h>
#include <vssym32.h>

typedef struct tagBUTTONRENDERINFOSTATES {
    char data[12];
} BUTTONRENDERINFOSTATES, *PBUTTONRENDERINFOSTATES;

/* Remove clock hover effect */
typedef void (*__cdecl GDIHelpers_FillRectARGB_t)(HDC, LPCRECT, BYTE, DWORD, bool);
GDIHelpers_FillRectARGB_t GDIHelpers_FillRectARGB_orig;
void __cdecl GDIHelpers_FillRectARGB_hook(
    HDC     hDC,
    LPCRECT lprc,
    BYTE    btUnknown,
    DWORD   dwUnknown,
    bool    bUnknown
)
{
    return;
}

/* Draw taskbar item */
typedef void (*__cdecl CTaskBtnGroup__DrawBar_t)(void *, HDC, void *, PBUTTONRENDERINFOSTATES);
CTaskBtnGroup__DrawBar_t CTaskBtnGroup__DrawBar_orig;
void __cdecl CTaskBtnGroup__DrawBar_hook(
    void *pThis,
    HDC   hDC,
    void *pRenderInfo,
    PBUTTONRENDERINFOSTATES pRenderStates
)
{
    LPRECT lprcDest = (LPRECT)((char *)pRenderInfo + 4);

    HTHEME hTheme = OpenThemeData(NULL, L"BUTTON");

    if (hTheme) {
        int iStateId = PBS_NORMAL; 

        if (pRenderStates->data[2]) {

            iStateId = PBS_PRESSED; // or use PBS_HOT if you prefer

        } else if (pRenderStates->data[4]) {

            iStateId = PBS_PRESSED;

        }

        // Draw the button with the current theme.

        DrawThemeBackground(hTheme, hDC, BP_PUSHBUTTON, iStateId, lprcDest, NULL);


        // Close the theme data handle when done.

        CloseThemeData(hTheme);
    } else
    {
        // Fallback to non-themed drawing if themes are not available.

        UINT uState = DFCS_BUTTONPUSH;
        if (pRenderStates->data[2])
        {
            uState |= DFCS_CHECKED;
        }
        else if (pRenderStates->data[4])
        {
            uState |= DFCS_PUSHED;
        }

        DrawFrameControl(hDC, lprcDest, DFC_BUTTON, uState);
    };


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
                L"void __cdecl GDIHelpers::FillRectARGB(struct HDC__ *,struct tagRECT const *,unsigned char,unsigned long,bool)"
            },
            &GDIHelpers_FillRectARGB_orig,
            GDIHelpers_FillRectARGB_hook,
            FALSE
        },
        {
            {
                L"private: void __cdecl CTaskBtnGroup::_DrawBar(struct HDC__ *,struct BUTTONRENDERINFO const &,struct BUTTONRENDERINFOSTATES const &)"
            },
            &CTaskBtnGroup__DrawBar_orig,
            CTaskBtnGroup__DrawBar_hook,
            FALSE
        }
    };

    if (!WindhawkUtils::HookSymbols(hExplorer, hooks, ARRAYSIZE(hooks)))
    {
        Wh_Log(L"Failed to hook one or more functions");
        return FALSE;
    }

    return TRUE;
}
