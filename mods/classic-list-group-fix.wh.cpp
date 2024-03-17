// ==WindhawkMod==
// @id              classic-list-group-fix
// @name            Classic List Group Fix
// @description     Fixes the appearance of list group headers in classic theme.
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic List View Fix
The appearance of list group headers in classic theme is wrong. They have
incorrect margins and the font is not bolded. This mod fixes that.
*/
// ==/WindhawkModReadme==

#include <commctrl.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#    define THISCALL  __cdecl
#    define STHISCALL L"__cdecl"
#else
#    define THISCALL  __thiscall
#    define STHISCALL L"__thiscall"
#endif

#define Wh_LogRect(lprc)                         \
Wh_Log(                                          \
    L"Top: %d, Left: %d, Right: %d, Bottom: %d", \
    lprc->top,                                   \
    lprc->left,                                  \
    lprc->right,                                 \
    lprc->bottom                                 \
)

typedef INT_PTR (THISCALL *CListGroup_GetGroupRect_t)(void *, LPRECT);
CListGroup_GetGroupRect_t CListGroup_GetGroupRect_orig;
INT_PTR THISCALL CListGroup_GetGroupRect_hook(
    void  *pThis,
    LPRECT lprc
)
{
    INT_PTR res = CListGroup_GetGroupRect_orig(
        pThis, lprc
    );
    lprc->left = 0;

    if (lprc->top == 5)
    {
        OffsetRect(lprc, 0, -5);
    }
    return res;
}

typedef void (THISCALL *CListGroup__PaintHeader_t)(void *, ULONG, LPNMLVCUSTOMDRAW);
CListGroup__PaintHeader_t CListGroup__PaintHeader_orig;
void THISCALL CListGroup__PaintHeader_hook(
    void            *pThis,
    ULONG            uFlags,
    LPNMLVCUSTOMDRAW lpcd
)
{
    lpcd->rcText.left = 6;
    lpcd->rcText.top++;

    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);

    SystemParametersInfoW(
        SPI_GETNONCLIENTMETRICS,
        sizeof(NONCLIENTMETRICSW),
        &ncm,
        NULL
    );

    HFONT hfTitle = CreateFontIndirectW(
        &(ncm.lfCaptionFont)
    );
    SelectObject(lpcd->nmcd.hdc, hfTitle);

    CListGroup__PaintHeader_orig(
        pThis, uFlags, lpcd
    );

    DeleteObject(hfTitle);
}

typedef INT_PTR (THISCALL *CListGroup_HeaderHeight_t)(void *);
CListGroup_HeaderHeight_t CListGroup_HeaderHeight_orig;
INT_PTR THISCALL CListGroup_HeaderHeight_hook(
    void *pThis
)
{
    return 13;
}

BOOL Wh_ModInit(void)
{   
    HMODULE hComCtl = LoadLibraryW(L"comctl32.dll");
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                L"public: int "
                STHISCALL
                L" CListGroup::GetGroupRect(struct tagRECT *)const "
            },
            (void **)&CListGroup_GetGroupRect_orig,
            (void *)CListGroup_GetGroupRect_hook,
            false
        },
        {
            {
                L"private: void "
                STHISCALL
                L" CListGroup::_PaintHeader(unsigned long,struct tagNMLVCUSTOMDRAW *)"
            },
            (void **)&CListGroup__PaintHeader_orig,
            (void *)CListGroup__PaintHeader_hook,
            false
        },
        {
            {
                L"public: int "
                STHISCALL
                L" CListGroup::HeaderHeight(void)const "
            },
            (void **)&CListGroup_HeaderHeight_orig,
            (void *)CListGroup_HeaderHeight_hook,
            false
        }
    };

    if (!WindhawkUtils::HookSymbols(
        hComCtl,
        hooks,
        ARRAYSIZE(hooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions");
        return FALSE;
    }

    return TRUE;
}