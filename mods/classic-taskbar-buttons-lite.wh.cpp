// ==WindhawkMod==
// @id classic-taskbar-buttons-lite
// @name Classic Taskbar 3D buttons Lite
// @description Lightweight mod restoring the 3D buttons in classic theme
// @version 1.4
// @author Anixx
// @github https://github.com/Anixx
// @include explorer.exe
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

Be warned that the progress indicator will not be displayed on the task buttons in the 3D mode, so if you need the progress bar, don't apply this mod.

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

typedef struct tagBUTTONRENDERINFOSTATES
{
    char data[12];
} BUTTONRENDERINFOSTATES, *PBUTTONRENDERINFOSTATES;

typedef void (*CTaskBtnGroup__DrawBar_t)(void *, HDC, void *, void *);
CTaskBtnGroup__DrawBar_t CTaskBtnGroup__DrawBar_orig;

static HWND FindTaskListForTray(HWND hTray)
{
    HWND hReBar = FindWindowExW(hTray, NULL, L"ReBarWindow32", NULL);
    if (!hReBar)
        return NULL;

    HWND hTaskSw = FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL);
    if (!hTaskSw)
        return NULL;

    return FindWindowExW(hTaskSw, NULL, L"MSTaskListWClass", NULL);
}

// Cache per taskbar thread to avoid window-manager lock in paint path.
// Refreshes when tray rect changes, which covers ABN_POSCHANGED / WM_SETTINGCHANGE / resolution change.
static thread_local HWND tl_hTray = NULL;
static thread_local RECT tl_lastTrayRect = {};
static thread_local bool tl_isHorizontal = true;
static thread_local bool tl_cacheValid = false;

void CALCON CTaskBtnGroup__DrawBar_hook(void *pThis, HDC hDC, void *pRenderInfo, PBUTTONRENDERINFOSTATES pRenderStates)
{
    LPRECT lprcDest = (LPRECT)((char *)pRenderInfo + 4);

    bool isHorizontal = true;

    DWORD curTid = GetCurrentThreadId();

    // Fast path: use cached tray geometry
    if (tl_cacheValid && tl_hTray && IsWindow(tl_hTray))
    {
        RECT rc;
        if (GetWindowRect(tl_hTray, &rc))
        {
            if (memcmp(&rc, &tl_lastTrayRect, sizeof(RECT))!= 0)
            {
                tl_isHorizontal = (rc.right - rc.left) > (rc.bottom - rc.top);
                tl_lastTrayRect = rc;
            }
            isHorizontal = tl_isHorizontal;
        }
        else
        {
            // GetWindowRect failed, keep last known value
            isHorizontal = tl_isHorizontal;
        }
    }
    else
    {
        // Slow path: resolve once
        HWND hTrays[16];
        int n = 0;

        HWND hMain = FindWindowW(L"Shell_TrayWnd", NULL);
        if (hMain)
            hTrays[n++] = hMain;

        HWND hSec = NULL;
        while ((hSec = FindWindowExW(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) && n < 16)
            hTrays[n++] = hSec;

        for (int i = 0; i < n; i++)
        {
            HWND hList = FindTaskListForTray(hTrays[i]);
            if (!hList)
                continue;

            if (GetWindowThreadProcessId(hList, NULL)!= curTid)
                continue;

            RECT rc;
            GetWindowRect(hTrays[i], &rc);
            tl_hTray = hTrays[i];
            tl_lastTrayRect = rc;
            tl_isHorizontal = (rc.right - rc.left) > (rc.bottom - rc.top);
            tl_cacheValid = true;
            isHorizontal = tl_isHorizontal;
            break;
        }
        // if not found, keep default horizontal and don't mark cache valid
        // so we will retry next time
    }

    // Visual gap only, no layout shift
    if (isHorizontal)
    {
        if ((lprcDest->right - lprcDest->left) > 2)
            lprcDest->right -= 2;
    }
    // Vertical: no cuts at all

    UINT uState = DFCS_BUTTONPUSH;

    if (pRenderStates->data[2])
        uState |= DFCS_CHECKED;
    else if (pRenderStates->data[4])
        uState |= DFCS_PUSHED;

    DrawFrameControl(hDC, lprcDest, DFC_BUTTON, uState);

    if (pRenderStates->data[2] || pRenderStates->data[4])
    {
        lprcDest->top++;
        lprcDest->bottom++;
        lprcDest->left++;
        lprcDest->right++;
    }
}

BOOL Wh_ModInit(void)
{
    HMODULE hExplorer = GetModuleHandleW(NULL);

    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {L"private: void " SCALCON L" CTaskBtnGroup::_DrawBar(struct HDC__ *,struct BUTTONRENDERINFO const &,struct BUTTONRENDERINFOSTATES const &)"},
            (void **)&CTaskBtnGroup__DrawBar_orig,
            (void *)CTaskBtnGroup__DrawBar_hook,
            FALSE,
        },
    };

    return WindhawkUtils::HookSymbols(hExplorer, explorerExeHooks, ARRAYSIZE(explorerExeHooks));
}
