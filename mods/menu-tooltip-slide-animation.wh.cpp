// ==WindhawkMod==
// @id              menu-tooltip-slide-animation
// @name            Menu/Tooltip Slide Animation
// @description     Makes menus and tooltips slide into view
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lgdi32 -lcomctl32 -lversion -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Menu/Tooltip Slide Animation
From Windows 98 through Windows XP, you could make menus and tooltips use a
slide/scroll animation. This mod restores those animations to modern versions
of Windows.

## Caption button tooltips
The tooltips you see when hovering a window's caption buttons (Close, Maximize, Minimize)
are implemented by the kernel-mode component of NTUSER. **It is impossible to make them
have a slide animation without issues.**

**Tooltips**:

![Tooltips](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/menu-tooltip-slide-animation/tooltip.gif)

**Menus (Windows 98/Me)**:

![Menus (Windows 98/Me)](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/menu-tooltip-slide-animation/menu-9x.gif)

**Menus (Windows 2000/XP)**:

![Menus (Windows 2000/XP)](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/menu-tooltip-slide-animation/menu-nt.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- nt_menu_animation: false
  $name: Windows 2000/XP menu animation
  $description: Makes the menu slide animation match that of Windows 2000/XP rather than 98/Me.
*/
// ==/WindhawkModSettings==

#include <ntdef.h>
#include <windowsx.h>
#include <windhawk_utils.h>
#include <uxtheme.h>
#include <commctrl.h>
#include <vector>

#pragma region "SPI stuff"

inline bool MenuAnimationEnabled(void)
{
    return Wh_GetIntValue(L"AnimateMenus", 1);
}

inline bool MenuFadeEnabled(void)
{
    return Wh_GetIntValue(L"FadeMenus", 0);
}

inline bool TooltipAnimationEnabled(void)
{
    return Wh_GetIntValue(L"AnimateTooltips", 1);
}

inline bool TooltipFadeEnabled(void)
{
    return Wh_GetIntValue(L"FadeTooltips", 0);
}

using SystemParametersInfo_t = decltype(&SystemParametersInfoW);
SystemParametersInfo_t SystemParametersInfoW_orig;

#define USPF_MENUS        0x1
#define USPF_TOOLTIPS     0x2
#define USPF_ININIT       0x4
#define USPF_INUNINIT     0x8

/* Updates the real SPI params.

   While the slide animation is enabled (fade is disabled), we set the
   actual animation bit off, to allow our custom menu animation to take
   place. The actual fade bit is always enabled, due to issues with the
   real slide animations that are mentioned below. */
void UpdateSystemParameters(DWORD dwFlags)
{
    SystemParametersInfo_t pfnSPI = (dwFlags & (USPF_ININIT | USPF_INUNINIT))
        ? SystemParametersInfoW : SystemParametersInfoW_orig;

    if (dwFlags & USPF_MENUS)
    {
        bool fAnimEnabled = (dwFlags & USPF_INUNINIT) ?
                Wh_GetIntValue(L"AnimateMenus", 0) :
                (MenuAnimationEnabled() && MenuFadeEnabled());
        pfnSPI(SPI_SETMENUANIMATION, 0, (LPVOID)fAnimEnabled, SPIF_UPDATEINIFILE);

        /* The fade bit should *always* be on, regardless if the user wants fades or not.
           Having it disabled with animations enabled will cause the real slide to be used
           for kernel-mode components, which is problematic.
           
           For menus in particular, the real slide animation's timer is bad on modern graphics
           drivers, causing the menu to pop in instantly. The animation will also have a solid
           background. */
        pfnSPI(SPI_SETMENUFADE, 0, (LPVOID)TRUE, SPIF_UPDATEINIFILE);
    }

    if (dwFlags & USPF_TOOLTIPS)
    {
        bool fAnimEnabled = (dwFlags & USPF_INUNINIT) ?
                Wh_GetIntValue(L"AnimateTooltips", 0) :
                (TooltipAnimationEnabled() && TooltipFadeEnabled());
        pfnSPI(SPI_SETTOOLTIPANIMATION, 0, (LPVOID)fAnimEnabled, SPIF_UPDATEINIFILE);

        /* Same as above. For tooltips in particular, there are three implementations of it:
           ComCtl v5 which works just fine, ComCtl v6 which we need to fix up, and the kernel
           mode implementation. Compared to menus, kernel-mode tooltips still have the solid
           background issue, lack the timing issue, and have a unique issue having to do with
           color depth that makes them show up in black and white (1-bit palette). */
        pfnSPI(SPI_SETTOOLTIPFADE, 0, (LPVOID)TRUE, SPIF_UPDATEINIFILE);
    }
}

bool SlideAnimSystemParametersInfo(
    UINT    uiAction,
    UINT    uiParam,
    LPVOID  pvParam,
    UINT    fWinIni,
    BOOL   *pfResult
)
{
#define VALIDATE_AND_SET_PVPARAM(val) \
    if (!pvParam)                     \
    {                                 \
        *pfResult = FALSE;            \
        return true;                  \
    }                                 \
                                      \
    *(BOOL *)pvParam = (val);         \
    *pfResult = TRUE;                 \
    return true;

#define SET_VALUE_AND_SEND_CHANGE(name, update) \
    Wh_SetIntValue(name, pvParam != 0);         \
                                                \
    UpdateSystemParameters(USPF_ ## update);    \
                                                \
    if (fWinIni & SPIF_SENDCHANGE)              \
    {                                           \
        ULONG_PTR dwResult;                     \
        SendMessageTimeoutW(HWND_BROADCAST,     \
                            WM_SETTINGCHANGE,   \
                            uiAction,           \
                            (LPARAM)L"",        \
                            SMTO_NORMAL,        \
                            100,                \
                            &dwResult);         \
    }

    switch (uiAction)
    {
        case SPI_GETMENUANIMATION:
            VALIDATE_AND_SET_PVPARAM(MenuAnimationEnabled());
        case SPI_GETMENUFADE:
            VALIDATE_AND_SET_PVPARAM(MenuFadeEnabled());
        case SPI_GETTOOLTIPANIMATION:
            VALIDATE_AND_SET_PVPARAM(TooltipAnimationEnabled());
        case SPI_GETTOOLTIPFADE:
            VALIDATE_AND_SET_PVPARAM(TooltipFadeEnabled());

        case SPI_SETMENUANIMATION:
            SET_VALUE_AND_SEND_CHANGE(L"AnimateMenus",        MENUS);
        case SPI_SETMENUFADE:
            SET_VALUE_AND_SEND_CHANGE(L"FadeMenus",           MENUS);
        case SPI_SETTOOLTIPANIMATION:
            SET_VALUE_AND_SEND_CHANGE(L"AnimateTooltips",  TOOLTIPS);
        case SPI_SETTOOLTIPFADE:
            SET_VALUE_AND_SEND_CHANGE(L"FadeTooltips",     TOOLTIPS);

        default:
            return false;
    }
}

BOOL WINAPI SystemParametersInfoW_hook(
    UINT    uiAction,
    UINT    uiParam,
    LPVOID  pvParam,
    UINT    fWinIni
)
{
    BOOL fResult;
    if (SlideAnimSystemParametersInfo(uiAction, uiParam, pvParam, fWinIni, &fResult))
        return fResult;
    return SystemParametersInfoW_orig(uiAction, uiParam, pvParam, fWinIni);
}

SystemParametersInfo_t SystemParametersInfoA_orig;
BOOL WINAPI SystemParametersInfoA_hook(
    UINT    uiAction,
    UINT    uiParam,
    LPVOID  pvParam,
    UINT    fWinIni
)
{
    BOOL fResult;
    if (SlideAnimSystemParametersInfo(uiAction, uiParam, pvParam, fWinIni, &fResult))
        return fResult;
    return SystemParametersInfoA_orig(uiAction, uiParam, pvParam, fWinIni);
}

#pragma endregion // "SPI stuff"

#pragma region "Menu animation implementation"

#define MENUCLASS               MAKEINTATOM(0x8000)

/* User API hook messages */
#define WM_UAHINITMENU          0x0093

/* Internal menu messages */
#define MN_SELECTITEM           0x1E0 + 5

/*
 * Internal menu flags stored in pMenu->fFlags.
 * These are replicated to puim->dwFlags.
 */
#define MFISPOPUP               0x00000001

/*
 * We don't need 64-bit intermediate precision so we use this macro
 * instead of calling MulDiv.
 */
#define MultDiv(x, y, z)        (((INT)(x) * (INT)(y) + (INT)(z) / 2) / (INT)(z))

// hmenu is the main window menu; hdc is the context to draw in
typedef struct tagUAHMENU
{
	HMENU hmenu;
	HDC   hdc;
	DWORD dwFlags;
} UAHMENU, *PUAHMENU;

HMODULE g_hmodUser = NULL;

bool g_fNTMenuAnimation = false;

int (WINAPI *GdiGetCharDimensions)(
    HDC hdc,
    TEXTMETRICW *lptm,
    LPINT lpcy);

bool g_fMenuAnimating = false;
thread_local ULONG g_uMenuDepth = 0;
thread_local std::vector<RECT> g_rgMenuRects;

thread_local bool g_fIsTpm = false;
thread_local POINT g_ptTpm = { 0, 0 };
thread_local UINT g_tpmFlags = 0;

thread_local POINT g_ptSysmenu = { -1, -1 };

thread_local HHOOK g_hMouseHook = NULL;
thread_local HHOOK g_hKeyboardHook = NULL;

thread_local HWND g_hwndMenuBar = NULL;
thread_local ULONGLONG g_ulLastMenuBarCloseTime = 0;

#define TPM_ANIMATIONBITS   0x3C00L
#define IDWH_MNANIMATE      0x00574DB1
#define CMS_QANIMATION      165
#define HRGN_FULL           ((HRGN)1)
#define DCX_USESTYLE        0x00010000L

#define MNA_LEFT       0x1
#define MNA_RIGHT      0x2
#define MNA_UP         0x4
#define MNA_DOWN       0x8
#define MNA_HORZ        (MNA_LEFT | MNA_RIGHT)
#define MNA_VERT        (MNA_UP | MNA_DOWN)

typedef struct _MNANIMATEINFO
{
    HWND      hwndAni;
    ULONGLONG ulAnimStartTime;
    int       iDropDir;
    HDC       hdcWndAni;
    HBITMAP   hbmAni;
    HDC       hdcFinalFrame;
    HBITMAP   hbmFinalFrame;
    HDC       hdcAni;
    int       ixAni;
    int       iyAni;
    int       cxAni;
    int       cyAni;
    int       cxMenuFontChar;
    int       cyMenuFontChar;
    bool      fMouseMoved;
    bool      fMouseIntersectingMenu;
    POINT     ptInitialMousePos;
    bool      fDeferSelectItem;
    WPARAM    wParamSelectItem;
    LPARAM    lParamSelectItem;
} MNANIMATEINFO;

thread_local MNANIMATEINFO g_mnAnimInfo = { 0 };

void MNAnimate(bool fIterate);
LRESULT WINAPI MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // If the mouse comes to intercept an animating menu, then its animation should be
    // cancelled.
    do
    if (nCode >= 0 && g_fMenuAnimating)
    {
        auto pmhs = (MOUSEHOOKSTRUCT *)lParam;
        POINT pt = pmhs->pt;
        RECT rcMenu = g_rgMenuRects.back();

        // We don't want to skip the animation if the menu was opened by the keyboard, even
        // if the mouse intercepts the middle of the menu. As such, we will check if the mouse
        // has remained stationary for the duration of the animation.
        if (!g_mnAnimInfo.fMouseMoved && pt.x == g_mnAnimInfo.ptInitialMousePos.x 
            && pt.y == g_mnAnimInfo.ptInitialMousePos.y)
        {
            break;
        }
        else
        {
            g_mnAnimInfo.fMouseMoved = true;
        }

        if (pt.x > rcMenu.left && pt.x < rcMenu.right && pt.y > rcMenu.top && pt.y < rcMenu.bottom)
        {
            g_mnAnimInfo.fMouseIntersectingMenu = true;
            MNAnimate(false);
        }
    }
    while (0);

    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

LRESULT WINAPI KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && g_fMenuAnimating)
    {
        MNAnimate(false);
    }

    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

void RegisterWindowsHooks()
{
    g_hMouseHook = SetWindowsHookExW(WH_MOUSE, MouseProc, NULL, GetCurrentThreadId());

    // The keyboard hook is only necessary for early cancellation with the keyboard on
    // first-level tracked popup menus, which get special handling to replicate perfectly-
    // accurate mouse behaviour.
    if (g_fNTMenuAnimation && g_uMenuDepth == 1)
    {
        g_hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD, KeyboardProc, NULL, GetCurrentThreadId());
    }
}

void UnregisterWindowsHooks()
{
    if (g_hMouseHook)
        UnhookWindowsHookEx(g_hMouseHook);
    g_hMouseHook = NULL;

    if (g_hKeyboardHook)
        UnhookWindowsHookEx(g_hKeyboardHook);
    g_hKeyboardHook = NULL;
}

void ResetMNAnimateInfo()
{
    if (g_mnAnimInfo.hdcWndAni)
        ReleaseDC(g_mnAnimInfo.hwndAni, g_mnAnimInfo.hdcWndAni);

    if (g_mnAnimInfo.hbmAni)
        DeleteObject(g_mnAnimInfo.hbmAni);

    if (g_mnAnimInfo.hdcAni)
        DeleteDC(g_mnAnimInfo.hdcAni);
    
    if (g_mnAnimInfo.hbmFinalFrame)
        DeleteObject(g_mnAnimInfo.hbmFinalFrame);
    
    if (g_mnAnimInfo.hdcFinalFrame)
        DeleteObject(g_mnAnimInfo.hdcFinalFrame);

    ZeroMemory(&g_mnAnimInfo, sizeof(g_mnAnimInfo));

    UnregisterWindowsHooks();
    g_fMenuAnimating = false;
}

void MNAnimateExit()
{
    // Clear the window region:
    HRGN hrgn = CreateRectRgn(
        0,
        0,
        g_mnAnimInfo.cxAni,
        g_mnAnimInfo.cyAni
    );
    SetWindowRgn(g_mnAnimInfo.hwndAni, hrgn, FALSE);
    DeleteObject(hrgn);

    // If we have a final frame DC, then paint that. This will usually (but not always)
    // be the same as the animation frame. The final frame can differ for the 9x
    // animation if another menu item was selected during the animation. The selected
    // item will not visually change during the animation, but will snap to the final
    // state.
    if (g_mnAnimInfo.hdcFinalFrame)
    {
        // To prevent flickering, the newly-selected item is deferred until the end of the animation.
        // Otherwise the OS would force paint over the animation, which causes a split second of the
        // newly-selected item being visible.
        if (g_mnAnimInfo.fDeferSelectItem)
        {
            SendMessageW(
                g_mnAnimInfo.hwndAni, MN_SELECTITEM,
                g_mnAnimInfo.wParamSelectItem, g_mnAnimInfo.lParamSelectItem);
        }

        SendMessageW(g_mnAnimInfo.hwndAni, WM_PRINT, (WPARAM)g_mnAnimInfo.hdcFinalFrame, PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND);
        BitBlt(g_mnAnimInfo.hdcWndAni, 0, 0, g_mnAnimInfo.cxAni, g_mnAnimInfo.cyAni,
            g_mnAnimInfo.hdcFinalFrame, 0, 0, SRCCOPY | NOMIRRORBITMAP);
        InvalidateRect(g_mnAnimInfo.hwndAni, nullptr, TRUE);
    }

    ResetMNAnimateInfo();
}

void MNAnimate(bool fIterate)
{
    int x, y, xOff, yOff, xLast, yLast;
    ULONGLONG ulTimeElapsed = GetTickCount64() - g_mnAnimInfo.ulAnimStartTime;

    if (!fIterate
        || (ulTimeElapsed > CMS_QANIMATION))
    {
        MNAnimateExit();
        return;
    }

    xLast = g_mnAnimInfo.ixAni;
    yLast = g_mnAnimInfo.iyAni;
    if (g_mnAnimInfo.iDropDir & MNA_HORZ)
    {
        if (g_fNTMenuAnimation)
            g_mnAnimInfo.ixAni = MultDiv(g_mnAnimInfo.cxMenuFontChar, ulTimeElapsed, CMS_QANIMATION / 20);
        else
            g_mnAnimInfo.ixAni = MultDiv(g_mnAnimInfo.cxAni, ulTimeElapsed, CMS_QANIMATION);
        
        if (g_mnAnimInfo.ixAni > g_mnAnimInfo.cxAni)
        {
            g_mnAnimInfo.ixAni = g_mnAnimInfo.cxAni;
        }
    }

    if (g_mnAnimInfo.iDropDir & MNA_VERT)
    {
        if (g_fNTMenuAnimation)
            g_mnAnimInfo.iyAni = MultDiv(g_mnAnimInfo.cyMenuFontChar, ulTimeElapsed, CMS_QANIMATION / 10);
        else
            g_mnAnimInfo.iyAni = MultDiv(g_mnAnimInfo.cyAni, ulTimeElapsed, CMS_QANIMATION);

        if (g_mnAnimInfo.iyAni > g_mnAnimInfo.cyAni)
        {
            g_mnAnimInfo.iyAni = g_mnAnimInfo.cyAni;
        }
    }

    if ((g_mnAnimInfo.ixAni == xLast) && (g_mnAnimInfo.iyAni == yLast))
        return;

    if (g_mnAnimInfo.iDropDir & MNA_LEFT)
    {
        x = g_mnAnimInfo.cxAni - g_mnAnimInfo.ixAni;
        xOff = 0;
    }
    else
    {
        xOff = g_mnAnimInfo.cxAni - g_mnAnimInfo.ixAni;
        x = 0;
    }

    if (g_mnAnimInfo.iDropDir & MNA_UP)
    {
        y = g_mnAnimInfo.cyAni - g_mnAnimInfo.iyAni;
        yOff = 0;
    }
    else
    {
        yOff = g_mnAnimInfo.cyAni - g_mnAnimInfo.iyAni;
        y = 0;
    }

    HRGN hrgn = CreateRectRgn(
        x,
        y,
        x + g_mnAnimInfo.ixAni,
        y + g_mnAnimInfo.iyAni
    );
    SetWindowRgn(g_mnAnimInfo.hwndAni, hrgn, FALSE);
    DeleteObject(hrgn);

    BitBlt(g_mnAnimInfo.hdcWndAni, x, y, g_mnAnimInfo.ixAni, g_mnAnimInfo.iyAni,
           g_mnAnimInfo.hdcAni, xOff, yOff, SRCCOPY | NOMIRRORBITMAP);

    if ((g_mnAnimInfo.cxAni == g_mnAnimInfo.ixAni)
     && (g_mnAnimInfo.cyAni == g_mnAnimInfo.iyAni))
    {
        InvalidateRect(g_mnAnimInfo.hwndAni, nullptr, TRUE);
        MNAnimateExit();
    }
}

/* Simulated timer using an alternate thread because real SetTimer
   has issues with freezing up. */
DWORD WINAPI MNAnimateThreadProc(LPVOID lpParam)
{
    while (g_fMenuAnimating)
    {
        SendMessageW((HWND)lpParam, WM_TIMER, IDWH_MNANIMATE, 0);
        Sleep(1);
    }
    return 0;
}

LRESULT CALLBACK MenuSubclassProc(
    HWND      hwnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    UINT_PTR  uIDSubclass,
    DWORD_PTR dwRefData
)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            g_uMenuDepth--;
            g_rgMenuRects.pop_back();
            ResetMNAnimateInfo();
            if (g_hwndMenuBar == hwnd)
            {
                g_ulLastMenuBarCloseTime = GetTickCount64();
            }
            goto DWP;
        case MN_SELECTITEM:
            // If we're not the animated menu then we don't need to do anything
            // here...
            if (g_mnAnimInfo.hwndAni != hwnd)
                goto DWP;

            // The animation should always be cancelled upon selecting an item when using the NT
            // animation. However, in order to prevent context menus invoked with the mouse from
            // closing when the mouse is moved away from the menu, top-level tracked popup menus
            // will only be closed if the mouse intersects them. This case is managed by MouseProc
            // rather than this case.
            if (g_fNTMenuAnimation)
            {
                if (g_fIsTpm && g_uMenuDepth == 1)
                    goto DWP;
                MNAnimate(false);
            }
            else
            {
                static thread_local bool s_fSelectingFirstItem   = false;
                static thread_local bool s_fHasSelectedFirstItem = false;

                // For 9x, in order to prevent flickering when the selected menu item is changed
                // during the animation's playback, the selection will actually be deferred.
                // However, we want submenus to show their first item as selected during the
                // animation.
                //
                // Selecting the first item for the submenu case actually does require sending
                // this message, but it's not blocked because the animation isn't fully set up at
                // the time. Notably, the animation start time is 0, so we can rely on that to check
                // if the animation setup is finished.
                if (!s_fSelectingFirstItem && g_mnAnimInfo.ulAnimStartTime != 0)
                {
                    // When invoked with keyboard, the first item of a submenu
                    // should be selected.
                    if (g_uMenuDepth > 1)
                    {
                        s_fSelectingFirstItem = true;
                        SendMessageW(g_mnAnimInfo.hwndAni, MN_SELECTITEM, 0, 0);
                        SendMessageW(g_mnAnimInfo.hwndAni, WM_PRINT, (WPARAM)g_mnAnimInfo.hdcAni, PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND);
                        s_fSelectingFirstItem = false;
                        s_fHasSelectedFirstItem = true;
                    }

                    if (!s_fHasSelectedFirstItem)
                    {
                        g_mnAnimInfo.fDeferSelectItem = true;
                        g_mnAnimInfo.wParamSelectItem = wParam;
                        g_mnAnimInfo.lParamSelectItem = lParam;
                    }
                    return 0;
                }
            }
            goto DWP;
        case WM_PAINT:
            if (g_fMenuAnimating)
                return 0;
            goto DWP;
        case WM_TIMER:
            if (wParam == IDWH_MNANIMATE)
            {
                MNAnimate(true);
                return 0;
            }
            goto DWP;
        case WM_WINDOWPOSCHANGED:
        {
            LPWINDOWPOS pwp = (LPWINDOWPOS)lParam;
            if ((pwp->flags & SWP_SHOWWINDOW))
            {
                // If a menu is currently animating and a new menu opens, then kill the ongoing
                // animation. This can occur if a submenu is opened while its parent menu is
                // animating.
                MNAnimate(false);

                HWND hwndParent = GetParent(hwnd);
                g_uMenuDepth++;
                RECT rcMenu = {
                    pwp->x,
                    pwp->y,
                    pwp->x + pwp->cx,
                    pwp->y + pwp->cy,
                };
                g_rgMenuRects.push_back(rcMenu);

                g_fMenuAnimating = true;
                RegisterWindowsHooks();

                DWORD dwAnimate = 0;
                if (hwndParent)
                {
                    // Submenu
                    if (g_uMenuDepth > 1)
                    {
                        if (rcMenu.left < g_rgMenuRects[g_uMenuDepth - 2].left)
                        {
                            dwAnimate = MNA_LEFT;
                        }
                        else
                        {
                            dwAnimate = MNA_RIGHT;
                        }
                    }
                    // Either a menubar menu or system menu invoked from
                    // system icon or Alt+Space
                    else
                    {
                        // Menu bars shouldn't animate when moving to another
                        // menu within the same menu
                        if (g_fNTMenuAnimation)
                        {
                            g_hwndMenuBar = hwnd;
                            ULONGLONG ulCurrentTime = GetTickCount64();
                            // 30 milliseconds is a low enough time for switching between menubar menus.
                            // The only other way this can be hit is if the user somehow manages to close
                            // and open the same menu within 30ms, which is very implausible...
                            if ((ulCurrentTime - g_ulLastMenuBarCloseTime) < 30)
                            {
                                goto CancelAnimation;
                            }
                        }                   

                        RECT rcClient;
                        GetClientRect(hwndParent, &rcClient);
                        MapWindowPoints(hwndParent, HWND_DESKTOP, (LPPOINT)&rcClient, 2);

                        if (rcMenu.bottom <= rcClient.top)
                        {
                            dwAnimate = MNA_UP;
                        }
                        else
                        {
                            dwAnimate = MNA_DOWN;
                        }
                    }
                }
                // Menu invoked by TrackPopupMenu(Ex)
                else if (g_fIsTpm)
                {
                    if (g_tpmFlags & TPM_NOANIMATION)
                        goto CancelAnimation;
                    
                    if (g_tpmFlags & TPM_ANIMATIONBITS)
                    {
                        if (g_tpmFlags & TPM_HORPOSANIMATION)
                        {
                            if ((g_tpmFlags & TPM_RIGHTALIGN) || g_ptTpm.x <= pwp->x)
                                dwAnimate |= MNA_RIGHT;
                            else
                                dwAnimate |= MNA_LEFT;
                        }

                        if (g_tpmFlags & TPM_VERPOSANIMATION)
                        {
                            if ((g_tpmFlags & TPM_BOTTOMALIGN) || g_ptTpm.y <= pwp->y)
                                dwAnimate |= MNA_DOWN;
                            else
                                dwAnimate |= MNA_UP;
                        }

                        if (g_tpmFlags & TPM_HORNEGANIMATION)
                        {
                            if (!(g_tpmFlags & TPM_RIGHTALIGN) || g_ptTpm.x > pwp->x)
                                dwAnimate |= MNA_LEFT;
                            else
                                dwAnimate |= MNA_RIGHT;
                        }

                        if (g_tpmFlags & TPM_VERNEGANIMATION)
                        {
                            if (!(g_tpmFlags & TPM_BOTTOMALIGN) || g_ptTpm.y > pwp->y)
                                dwAnimate |= MNA_UP;
                            else
                                dwAnimate |= MNA_DOWN;
                        }
                    }
                    else
                    {
                        if ((g_tpmFlags & TPM_CENTERALIGN) && (g_tpmFlags & TPM_VCENTERALIGN))
                            goto CancelAnimation;

                        if (!(g_tpmFlags & TPM_CENTERALIGN))
                        {
                            if (g_ptTpm.x <= pwp->x)
                                dwAnimate |= MNA_RIGHT;
                            else
                                dwAnimate |= MNA_LEFT;
                        }

                        if (!(g_tpmFlags & TPM_VCENTERALIGN))
                        {
                            if (g_ptTpm.y <= pwp->y)
                                dwAnimate |= MNA_DOWN;
                            else
                                dwAnimate |= MNA_UP;
                        }
                    }
                }
                // System menu invoked by right clicking titlebar or
                // WM_SYSMENU message. this does not have any special TPM
                // flags ever so we can position purely based on cursor.
                //
                // (TODO: Somehow handle WM_SYSMENU not invoked by mouse?
                //  Probably impossible. Whatever, it's an undocumented
                //  message. Really only matters for the old Explorer sysmenu
                //  behavior mod...)
                else
                {
                    POINT ptCursor;
                    if (g_ptSysmenu.x == -1)
                        GetCursorPos(&ptCursor);
                    else
                        ptCursor = g_ptSysmenu;

                    if (ptCursor.x <= pwp->x)
                        dwAnimate |= MNA_RIGHT;
                    else
                        dwAnimate |= MNA_LEFT;

                    if (ptCursor.y <= pwp->y)
                        dwAnimate |= MNA_DOWN;
                    else
                        dwAnimate |= MNA_UP;
                }

                g_ptSysmenu = { -1, -1 };

                g_mnAnimInfo.hwndAni = hwnd;
                g_mnAnimInfo.iDropDir = dwAnimate;
                g_mnAnimInfo.cxAni = pwp->cx;
                g_mnAnimInfo.cyAni = pwp->cy;
                g_mnAnimInfo.ixAni = (dwAnimate & MNA_HORZ) ? 0 : pwp->cx;
                g_mnAnimInfo.iyAni = (dwAnimate & MNA_VERT) ? 0 : pwp->cy;
                g_mnAnimInfo.hdcWndAni = GetDCEx(hwnd, NULL, DCX_WINDOW | DCX_USESTYLE);
                g_mnAnimInfo.hdcAni = CreateCompatibleDC(NULL);
                g_mnAnimInfo.hbmAni = CreateCompatibleBitmap(g_mnAnimInfo.hdcWndAni, pwp->cx, pwp->cy);
                g_mnAnimInfo.hdcFinalFrame = CreateCompatibleDC(NULL);
                g_mnAnimInfo.hbmFinalFrame = CreateCompatibleBitmap(g_mnAnimInfo.hdcWndAni, pwp->cx, pwp->cy);
                GetCursorPos(&g_mnAnimInfo.ptInitialMousePos);
                
                /* Get font metrics */
                if (g_fNTMenuAnimation)
                {
                    NONCLIENTMETRICSW ncm;
                    ZeroMemory(&ncm, sizeof(ncm));
                    ncm.cbSize = sizeof(ncm);

                    using SystemParametersInfoForDpi_t = decltype(&SystemParametersInfoForDpi);
                    static SystemParametersInfoForDpi_t
                        pfnSystemParametersInfoForDpi = (SystemParametersInfoForDpi_t)GetProcAddress(g_hmodUser, "SystemParametersInfoForDpi");
                    if (pfnSystemParametersInfoForDpi)
                    {
                        int dpi = GetDeviceCaps(g_mnAnimInfo.hdcWndAni, LOGPIXELSY);
                        pfnSystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi);
                    }
                    else
                    {
                        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
                    }

                    HFONT hfontMenu, hfontOld;

                    hfontMenu = CreateFontIndirectW(&ncm.lfMenuFont);
                    hfontOld = (HFONT)SelectObject(g_mnAnimInfo.hdcAni, hfontMenu);

                    g_mnAnimInfo.cxMenuFontChar = GdiGetCharDimensions(
                        g_mnAnimInfo.hdcAni, nullptr, &g_mnAnimInfo.cyMenuFontChar);

                    SelectObject(g_mnAnimInfo.hdcAni, hfontOld);
                    DeleteObject(hfontMenu);
                }

                /* Get menu image */
                SelectObject(g_mnAnimInfo.hdcAni, g_mnAnimInfo.hbmAni);
                SendMessageW(hwnd, WM_PRINT, (WPARAM)g_mnAnimInfo.hdcAni, PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND);

                SelectObject(g_mnAnimInfo.hdcFinalFrame, g_mnAnimInfo.hbmFinalFrame);
                SendMessageW(hwnd, WM_PRINT, (WPARAM)g_mnAnimInfo.hdcFinalFrame, PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND);

                /* Start animation */
                g_mnAnimInfo.ulAnimStartTime = GetTickCount64();
                CreateThread(nullptr, 0, MNAnimateThreadProc, (LPVOID)hwnd, 0, nullptr);
                goto DWP;
                
CancelAnimation:
                /* If we cancel we want the menu to be visible again... */
                SetWindowRgn(hwnd, NULL, FALSE);
                RedrawWindow(hwnd, nullptr, NULL, RDW_FRAME | RDW_INVALIDATE);
                InvalidateRect(hwnd, nullptr, TRUE);
                g_fMenuAnimating = false;
                UnregisterWindowsHooks();
                goto DWP;
            }
            [[fallthrough]];
        }
        default:
        DWP:
            return DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }
}

#define WM_SYSMENU                      0x0313

void SlideAnimWndProc(
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (uMsg)
    {
        case WM_UAHINITMENU:
        {
            if (!MenuAnimationEnabled() || MenuFadeEnabled())
                return;

            PUAHMENU puim = (PUAHMENU)lParam;
            if (!(puim->dwFlags & MFISPOPUP))
                return;

            HWND hwndMenu = NULL;
            while ((hwndMenu = FindWindowExW(NULL, hwndMenu, MENUCLASS, nullptr)))
            {
                HMENU hmenu = (HMENU)SendMessageW(hwndMenu, MN_GETHMENU, 0, 0);
                if (hmenu == puim->hmenu)
                    break;
            }

            /* Hack to prevent the window from flickering on before animation plays */
            HRGN hrgn = CreateRectRgn(0, 0, 0, 0);
            SetWindowRgn(hwndMenu, hrgn, FALSE);
            DeleteObject(hrgn);

            SetWindowSubclass(hwndMenu, MenuSubclassProc, 0, (DWORD_PTR)puim->hmenu);
            return;
        }
        case WM_CONTEXTMENU:
        case WM_SYSMENU:
        {
            g_ptSysmenu = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            return;
        }
    }
}

#define DWP_HOOK_(name, defArgs, callArgs) \
LRESULT (CALLBACK *name ## _orig) defArgs; \
LRESULT CALLBACK name ## _hook  defArgs \
{ \
    SlideAnimWndProc(hWnd, uMsg, wParam, lParam); \
    return name ## _orig  callArgs; \
}

#define DWP_HOOK(name, defArgs, callArgs)  \
    DWP_HOOK_(name ## A, defArgs, callArgs) \
    DWP_HOOK_(name ## W, defArgs, callArgs) \
    
DWP_HOOK(
    DefWindowProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
DWP_HOOK(
    DefFrameProc,
    (HWND hWnd, HWND hWndMDIClient, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, hWndMDIClient, uMsg, wParam, lParam))
DWP_HOOK(
    DefMDIChildProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
DWP_HOOK(
    DefDlgProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_orig;
BOOL WINAPI TrackPopupMenu_hook(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    int         nReserved,
    HWND        hWnd,
    const RECT *prcRect
)
{
    g_fIsTpm = true;
    g_tpmFlags = uFlags;
    g_ptTpm = { x, y };
    BOOL fResult = TrackPopupMenu_orig(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    g_fIsTpm = false;
    g_tpmFlags = 0;
    g_ptTpm = { 0, 0};
    return fResult;
}

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_orig;
BOOL WINAPI TrackPopupMenuEx_hook(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    HWND        hwnd,
    LPTPMPARAMS lptpm
)
{
    g_fIsTpm = true;
    g_tpmFlags = uFlags;
    g_ptTpm = { x, y };
    BOOL fResult = TrackPopupMenuEx_orig(hMenu, uFlags, x, y, hwnd, lptpm);
    g_fIsTpm = false;
    g_tpmFlags = 0;
    g_ptTpm = { 0, 0};
    return fResult;
}

#pragma endregion // "Menu animation implementation"

#pragma region "Tooltip fix (ComCtl v6)"

struct DPISCALEINFO
{
    UINT m_uDpiX;
    UINT m_uDpiY;
    bool m_fDPIAware : 1;
    bool m_fIsThemingEnabled : 1;
    bool m_fIsIgnoringDpiChanges : 1;
};

typedef struct tagControlInfo
{
    HWND hwnd;
    HWND hwndParent;
    UINT style;
    DWORD dwCustom;
#ifdef _WIN64
    BOOL bUnicode;
    BOOL bInFakeCustomDraw;
#else
    BOOL bUnicode : 1;
    BOOL bInFakeCustomDraw : 1;
#endif
    UINT uiCodePage;
    DWORD dwExStyle;
    int iVersion;
    WORD wUIState;
    DPISCALEINFO dpi;
} CCONTROLINFO, *LPCCONTROLINFO;



class CToolTipsMgr
{
public:
    void         *__vftable;
    CCONTROLINFO  _ci;
    int           _cRef;
    int           _iNumTools;
    int           _iDelayTime;
    int           _iReshowTime;
    int           _iAutoPopTime;
    LPTOOLINFOW  _tools;
    LPTOOLINFOW  _pCurTool;
    HFONT        _hFont;
    HFONT        _hFontUnderline;
    DWORD        _dwFlags;
    UINT         _idTimer;
    POINT        _pt;
    UINT         _idtAutoPop;
    LPWSTR       _lpTipTitle;
    DWORD        _cchTipTitle;
    UINT         _iOneBasedTitleIconIndex;
    HIMAGELIST   _himlTitleIcons_Small;
    HIMAGELIST   _himlTitleIcons_Large;
    HIMAGELIST   _himlTitleIcons;
    POINT        _ptTrack;
#ifdef _WIN64
    BOOL         _fMyFont            : 1;
    BOOL         _fBkColorSet        : 1;
    BOOL         _fTextColorSet      : 1;
    BOOL         _fUnderStem         : 1;
    BOOL         _fInWindowFromPoint : 1;
    BOOL         _fEverShown         : 1;
    BOOL         _fDrawThemedStem    : 1;
    BOOL         _fCanReuseWindowRgn : 1;
    char         _unknown[8];
#else
    int          _unknown;
#endif
    COLORREF     _clrTipBk;
    COLORREF     _clrTipText;
    COLORREF     _clrTipTextUnthemed;
    int          _iMaxTipWidth;
    UINT         _uDefDrawFlags;
    RECT         _rcMargin;
    RECT         _rcMarginUnthemed;
    int          _iStemHeight;
    DWORD        _dwLastDisplayTime;
    int          _iBalloonIconWidth;
    HTHEME       _hTheme;
};

#define TTT_SLIDE           0x7569736B
#define CMS_TOOLTIP         135
#define TIMEBETWEENANIMATE  2000

thread_local bool g_fTooltipWasShown  = false;
thread_local bool g_fAnimatingTooltip = false;

LRESULT __fastcall (*CToolTipsMgr_ToolTipsWndProc_orig)(CToolTipsMgr *, HWND, UINT, WPARAM, LPARAM);
LRESULT __fastcall CToolTipsMgr_ToolTipsWndProc_hook(
    CToolTipsMgr *pThis,
    HWND          hwnd, 
    UINT          uMsg,
    WPARAM        wParam,
    LPARAM        lParam
)
{
    switch (uMsg)
    {
        case WM_WINDOWPOSCHANGING:
        {
            LPWINDOWPOS pwp = (LPWINDOWPOS)lParam;
            if (TooltipAnimationEnabled() && !TooltipFadeEnabled()
            && !g_fAnimatingTooltip && (pwp->flags & SWP_SHOWWINDOW))
            {
                g_fTooltipWasShown = true;
                pwp->flags &= ~SWP_SHOWWINDOW;
            }
            goto DoDefault;
        }
        // HACK: For whatever reason we don't get a frame on tooltips from this, which
        // is used by AnimateWindow below... Put it in ourselves.
        case WM_PRINTCLIENT:
        {
            LRESULT lres = CToolTipsMgr_ToolTipsWndProc_orig(pThis, hwnd, uMsg, wParam, lParam);
            if (!pThis->_hTheme && !(pThis->_ci.style & (TTS_BALLOON | TTS_NOANIMATE)))
            {
                RECT rc;
                GetClientRect(hwnd, &rc);
                HBRUSH hbr = CreateSolidBrush(pThis->_clrTipText);
                FrameRect((HDC)wParam, &rc, hbr);
                DeleteObject(hbr);
            }
            return lres;
        }
        default:
        DoDefault:
            return CToolTipsMgr_ToolTipsWndProc_orig(pThis, hwnd, uMsg, wParam, lParam);
    }
}

void (__thiscall *CToolTipsMgr_DoShowBubble_orig)(CToolTipsMgr *);
void __thiscall CToolTipsMgr_DoShowBubble_hook(
    CToolTipsMgr *pThis
)
{
    DWORD dwLastDisplayTime = pThis->_dwLastDisplayTime;
    CToolTipsMgr_DoShowBubble_orig(pThis);
    // If the tooltip isn't left visible from the original function,
    // bail out.
    if (g_fTooltipWasShown && TooltipAnimationEnabled() && !TooltipFadeEnabled())
    {
        g_fTooltipWasShown = false;
        
        HWND hwnd = pThis->_ci.hwnd;
        
        DWORD dwCurrentTime = (dwLastDisplayTime == 0)
            ? TIMEBETWEENANIMATE
            : GetTickCount();
        DWORD dwDelta = dwCurrentTime - dwLastDisplayTime;
        if ((pThis->_ci.style & (TTS_BALLOON | TTS_NOANIMATE)) || dwDelta < TIMEBETWEENANIMATE)
        {
            g_fAnimatingTooltip = true;
            SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            g_fAnimatingTooltip = false;
            return;
        }

        ShowWindow(hwnd, SW_HIDE);
        
        RECT rc;
        GetWindowRect(hwnd, &rc);

        DWORD dwPos = GetMessagePos();
        DWORD dwFlags;
        if (GET_Y_LPARAM(dwPos) > rc.top + (rc.bottom - rc.top) / 2)
        {
            dwFlags = AW_VER_NEGATIVE;
        }
        else
        {
            dwFlags = AW_VER_POSITIVE;
        }

        g_fAnimatingTooltip = true;
        AnimateWindow(hwnd, CMS_TOOLTIP, dwFlags | AW_SLIDE);
        g_fAnimatingTooltip = false;
    }
}

#pragma endregion // "Tooltip fix (ComCtl v6)"

void Wh_ModSettingsChanged(void)
{
    g_fNTMenuAnimation = Wh_GetIntSetting(L"nt_menu_animation");
}

#ifdef _WIN64
#   define THISCALL_STR L"__cdecl"
#else
#   define THISCALL_STR L"__thiscall"
#endif

const WindhawkUtils::SYMBOL_HOOK comctl32DllHooks[] = {
    {
        {
            L"private: void " THISCALL_STR L" CToolTipsMgr::DoShowBubble(void)"
        },
        &CToolTipsMgr_DoShowBubble_orig,
        CToolTipsMgr_DoShowBubble_hook,
        false
    },
    {
        {
#ifdef _WIN64
            L"public: __int64 __cdecl CToolTipsMgr::ToolTipsWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
#else
            L"public: long __stdcall CToolTipsMgr::ToolTipsWndProc(struct HWND__ *,unsigned int,unsigned int,long)"
#endif
        },
        &CToolTipsMgr_ToolTipsWndProc_orig,
        CToolTipsMgr_ToolTipsWndProc_hook,
        false
    }
};

VS_FIXEDFILEINFO *GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen) 
{ 
    void *pFixedFileInfo = nullptr; 
    UINT uPtrLen = 0; 

    HRSRC hResource = 
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION); 
    if (hResource)
    { 
        HGLOBAL hGlobal = LoadResource(hModule, hResource); 
        if (hGlobal)
        { 
            void *pData = LockResource(hGlobal); 
            if (pData)
            { 
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen)
                || uPtrLen == 0)
                { 
                    pFixedFileInfo = nullptr; 
                    uPtrLen = 0; 
                } 
            } 
        } 
    } 

    if (puPtrLen)
    { 
        *puPtrLen = uPtrLen; 
    } 
  
     return (VS_FIXEDFILEINFO *)pFixedFileInfo; 
 } 

/**
  * Loads comctl32.dll, version 6.0.
  * This uses an activation context that uses shell32.dll's manifest
  * to load 6.0, even in apps which don't have the proper manifest for
  * it.
  */
HMODULE LoadComCtlModule(void)
{
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    ACTCTXW actCtx = { sizeof(actCtx) };
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    actCtx.hModule = hShell32;
    HANDLE hActCtx = CreateActCtxW(&actCtx);
    ULONG_PTR ulCookie;
    ActivateActCtx(hActCtx, &ulCookie);
    HMODULE hComCtl = LoadLibraryW(L"comctl32.dll");
    /**
      * Certain processes will ignore the activation context and load
      * comctl32.dll 5.82 anyway. If that occurs, just reject it.
      */
    VS_FIXEDFILEINFO *pVerInfo = GetModuleVersionInfo(hComCtl, nullptr);
    if (!pVerInfo || HIWORD(pVerInfo->dwFileVersionMS) < 6)
    {
        FreeLibrary(hComCtl);
        hComCtl = NULL;
    }
    DeactivateActCtx(0, ulCookie);
    ReleaseActCtx(hActCtx);
    FreeLibrary(hShell32);
    return hComCtl;
}

BOOL Wh_ModInit(void)
{
    Wh_ModSettingsChanged();

    /* We need to do this so we can restore the original state on uninit. */
    BOOL fEnabled;
    if (SystemParametersInfoW(SPI_GETMENUANIMATION, 0, (LPVOID)&fEnabled, 0)
    && fEnabled)
    {
        Wh_SetIntValue(L"AnimateMenus", 1);
    }
    if (SystemParametersInfoW(SPI_GETTOOLTIPANIMATION, 0, (LPVOID)&fEnabled, 0)
    && fEnabled)
    {
        Wh_SetIntValue(L"AnimateTooltips", 1);
    }

    UpdateSystemParameters(USPF_MENUS | USPF_TOOLTIPS | USPF_ININIT);

    HMODULE hmodGdi = LoadLibraryExW(L"gdi32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hmodGdi)
    {
        Wh_Log(L"Failed to load gdi32.dll");
        return FALSE;
    }

    *(void **)&GdiGetCharDimensions = (void *)GetProcAddress(hmodGdi, "GdiGetCharDimensions");
    if (!GdiGetCharDimensions)
    {
        Wh_Log(L"Failed to get address of GdiGetCharDimensions");
        return FALSE;
    }

    HMODULE hmodComCtl = LoadComCtlModule();
    if (!hmodComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hmodComCtl,
        comctl32DllHooks,
        ARRAYSIZE(comctl32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in comctl32.dll");
        return FALSE;
    }

    g_hmodUser = GetModuleHandleW(L"user32.dll");
    if (!g_hmodUser)
    {
        Wh_Log(L"Failed to get handle to user32.dll (???)");
        return FALSE;
    }

#define HOOK(func)                                                                         \
    if (!Wh_SetFunctionHook((void *)func, (void *)func ## _hook, (void **)&func ## _orig)) \
    {                                                                                      \
        Wh_Log(L"Failed to hook %s", L ## #func);                                          \
        return FALSE;                                                                      \
    }

#define HOOK_A_W(func) HOOK(func ## A) HOOK(func ## W)

    HOOK_A_W(DefWindowProc)
    HOOK_A_W(DefFrameProc)
    HOOK_A_W(DefMDIChildProc)
    HOOK_A_W(DefDlgProc)

    HOOK(TrackPopupMenu)
    HOOK(TrackPopupMenuEx)

    HOOK_A_W(SystemParametersInfo)

    return TRUE;
}

void Wh_ModUninit(void)
{
    UpdateSystemParameters(USPF_MENUS | USPF_TOOLTIPS | USPF_INUNINIT);
}