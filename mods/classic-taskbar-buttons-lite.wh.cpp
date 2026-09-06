// ==WindhawkMod==
// @id classic-taskbar-buttons-lite
// @name Classic Taskbar 3D buttons Lite
// @description Lightweight mod, restoring the 3D buttons in classic theme
// @version 1.4
// @author Anixx
// @github https://github.com/Anixx
// @include explorer.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

#include <windhawk_utils.h>
#ifdef _WIN64
#define CALCON __cdecl
#define SCALCON L"__cdecl"
#else
#define CALCON __thiscall
#define SCALCON L"__thiscall"
#endif
typedef struct tagBUTTONRENDERINFOSTATES { char data[12]; } BUTTONRENDERINFOSTATES, *PBUTTONRENDERINFOSTATES;

typedef void (* CTaskBtnGroup__DrawBar_t)(void *, HDC, void *, void *);
CTaskBtnGroup__DrawBar_t CTaskBtnGroup__DrawBar_orig;

static HWND FindTaskListForTray(HWND hTray)
{
    HWND hReBar = FindWindowExW(hTray, NULL, L"ReBarWindow32", NULL);
    if (!hReBar) return NULL;
    HWND hTaskSw = FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL);
    if (!hTaskSw) return NULL;
    return FindWindowExW(hTaskSw, NULL, L"MSTaskListWClass", NULL);
}

void CALCON CTaskBtnGroup__DrawBar_hook(void *pThis, HDC hDC, void *pRenderInfo, PBUTTONRENDERINFOSTATES pRenderStates)
{
    LPRECT lprcDest = (LPRECT)((char *)pRenderInfo + 4);

    DWORD curTid = GetCurrentThreadId();
    bool isHorizontal = true;
    HWND hMain = FindWindowW(L"Shell_TrayWnd", NULL);
    HWND hTrays[16]; int n=0;
    if (hMain) hTrays[n++] = hMain;
    HWND hSec=NULL;
    while ((hSec = FindWindowExW(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) && n<16) hTrays[n++] = hSec;
    for (int i=0;i<n;i++) {
        HWND hList = FindTaskListForTray(hTrays[i]);
        if (!hList) continue;
        if (GetWindowThreadProcessId(hList, NULL)!=curTid) continue;
        RECT rc; GetWindowRect(hTrays[i], &rc);
        isHorizontal = (rc.right - rc.left) > (rc.bottom - rc.top);
        break;
    }

    if (isHorizontal) {
        if ((lprcDest->right - lprcDest->left) > 2) lprcDest->right -= 2;
    }
    // вертикальный - ничего не режем

    UINT uState = DFCS_BUTTONPUSH;
    if (pRenderStates->data[2]) uState |= DFCS_CHECKED;
    else if (pRenderStates->data[4]) uState |= DFCS_PUSHED;
    DrawFrameControl(hDC, lprcDest, DFC_BUTTON, uState);
    if (pRenderStates->data[2] || pRenderStates->data[4]) { lprcDest->top++; lprcDest->bottom++; lprcDest->left++; lprcDest->right++; }
}

typedef long (* CTaskBtnGroup_SetLocation_t)(void *, int, int, LPRECT);
CTaskBtnGroup_SetLocation_t CTaskBtnGroup_SetLocation_orig;
long __cdecl CTaskBtnGroup_SetLocation_hook(void *pThis, int i1, int i2, LPRECT lprc)
{
    return CTaskBtnGroup_SetLocation_orig(pThis, i1, i2, lprc);
}

BOOL Wh_ModInit(void)
{
    HMODULE hExplorer = GetModuleHandleW(NULL);
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        { { L"private: void " SCALCON L" CTaskBtnGroup::_DrawBar(struct HDC__ *,struct BUTTONRENDERINFO const &,struct BUTTONRENDERINFOSTATES const &)" }, (void **)&CTaskBtnGroup__DrawBar_orig, (void *)CTaskBtnGroup__DrawBar_hook, FALSE },
        { { L"public: virtual long __cdecl CTaskBtnGroup::SetLocation(int,int,struct tagRECT const *)" }, (void **)&CTaskBtnGroup_SetLocation_orig, (void*)CTaskBtnGroup_SetLocation_hook, FALSE }
    };
    return WindhawkUtils::HookSymbols(hExplorer, hooks, ARRAYSIZE(hooks));
}
