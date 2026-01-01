// ==WindhawkMod==
// @id              old-explorer-sysmenu-behavior
// @name            Old Explorer System Menu Behavior
// @description     Restores Windows XP behavior to the window icon in File Explorer
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Old Explorer System Menu Behavior
Before Windows Vista, the system menu (window icon) of File Explorer windows behaved differently:
- Could be dragged to create a shortcut to the currently folder
- Could be right clicked to show a context menu for the current folder
- Upon a left click, the system menu's appearance is delayed and shows up at the cursor position rather
  than the regular fixed position

This mod restores the pre-Vista behavior.
*/
// ==/WindhawkModReadme==

#include <initguid.h>
#include <shlguid.h>
#include <windhawk_utils.h>
#include <windowsx.h>
#include <shlobj.h>

//
// Globals
//

UINT_PTR g_sysmenuTimer = 0;
HMODULE g_hExplorerFrame = NULL;

//
// Imported functions
//

typedef HRESULT (WINAPI *SHDoDragDropWithPreferredEffect_t)(HWND hwnd, IShellFolder *psi, LPITEMIDLIST pidl, DWORD dwEffect, LPDWORD pdwEffect);
SHDoDragDropWithPreferredEffect_t SHDoDragDropWithPreferredEffect;

typedef HMENU (WINAPI *SHLoadMenuPopup_t)(HINSTANCE hinst, UINT id);
SHLoadMenuPopup_t SHLoadMenuPopup;

//
// Symbol functions
//

HRESULT (*BindCtx_RegisterUIWindow)(LPVOID unused, HWND hwnd, IBindCtx **ppbcOut);
HRESULT (*ContextMenu_DeleteCommandByName)(IContextMenu *pcm, HMENU hpopup, UINT idFirst, LPCWSTR pszCommand);

//
// Util functions
//

BOOL CheckForDragBegin(HWND hwnd, int x, int y)
{
    RECT rcDragRadius;
    int  cxDrag = GetSystemMetrics(SM_CXDRAG);
    int  cyDrag = GetSystemMetrics(SM_CYDRAG);

    // See if the user moves a certain number of pixels in any direction
    SetRect(&rcDragRadius,
            x - cxDrag,
            y - cyDrag,
            x + cxDrag,
            y + cyDrag);

    MapWindowRect(hwnd, NULL, &rcDragRadius);

    SetCapture(hwnd);

    do 
    {
        MSG msg;

        // NTRAID 610356: Sleep the thread waiting for mouse input. Prevents pegging the CPU in a
        // PeekMessage loop.
        switch (MsgWaitForMultipleObjectsEx(0, NULL, INFINITE, QS_MOUSE, MWMO_INPUTAVAILABLE))
        {
            case WAIT_OBJECT_0:
            {
                if (PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
                {
                    // See if the application wants to process the message...
                    if (CallMsgFilter(&msg, MSGF_COMMCTRL_BEGINDRAG) == 0)
                    {
                        switch (msg.message)
                        {
                            case WM_LBUTTONUP:
                            case WM_RBUTTONUP:
                            case WM_LBUTTONDOWN:
                            case WM_RBUTTONDOWN:
                            {
                                // Released the mouse without moving outside the
                                // drag radius, not beginning a drag.
                                ReleaseCapture();
                                return FALSE;
                            }
                            case WM_MOUSEMOVE:
                            {
                                if (!PtInRect(&rcDragRadius, msg.pt)) 
                                {
                                    // Moved outside the drag radius, beginning a drag.
                                    ReleaseCapture();
                                    return TRUE;
                                }

                                break;
                            }
                            default:
                            {
                                TranslateMessage(&msg);
                                DispatchMessage(&msg);

                                break;
                            }
                        }
                    }
                }
                break;
            }
            default:
                break;
        }

        // WM_CANCELMODE messages will unset the capture, in that
        // case I want to exit this loop
    } while (GetCapture() == hwnd);

    return FALSE;
}

// pbIsNamed is true if the i-th item in hm is a named separator
STDAPI_(BOOL) _SHIsMenuSeparator2(HMENU hm, int i, BOOL *pbIsNamed)
{
    MENUITEMINFO mii;
    BOOL bLocal;

    if (!pbIsNamed)
        pbIsNamed = &bLocal;
        
    *pbIsNamed = FALSE;

    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_TYPE | MIIM_ID;
    mii.cch = 0;    // WARNING: We MUST initialize it to 0!!!
    if (GetMenuItemInfo(hm, i, TRUE, &mii) && (mii.fType & MFT_SEPARATOR))
    {
        // NOTE that there is a bug in either 95 or NT user!!!
        // 95 returns 16 bit ID's and NT 32 bit therefore there is a
        // the following may fail, on win9x, to evaluate to false
        // without casting
        *pbIsNamed = ((WORD)mii.wID != (WORD)-1);
        return TRUE;
    }
    return FALSE;
}

//
// _SHPrettyMenu -- make this menu look darn purty
//
// Prune the separators in this hmenu to ensure there isn't one in the first or last
// position and there aren't any runs of >1 separator.
//
// Named separators take precedence over regular separators.
//
STDAPI_(void) _SHPrettyMenu(HMENU hm)
{
    BOOL bSeparated = TRUE;
    BOOL bWasNamed = TRUE;

    for (int i = GetMenuItemCount(hm) - 1; i > 0; --i)
    {
        BOOL bIsNamed;
        if (_SHIsMenuSeparator2(hm, i, &bIsNamed))
        {
            if (bSeparated)
            {
                // if we have two separators in a row, only one of which is named
                // remove the non named one!
                if (bIsNamed && !bWasNamed)
                {
                    DeleteMenu(hm, i+1, MF_BYPOSITION);
                    bWasNamed = bIsNamed;
                }
                else
                {
                    DeleteMenu(hm, i, MF_BYPOSITION);
                }
            }
            else
            {
                bWasNamed = bIsNamed;
                bSeparated = TRUE;
            }
        }
        else
        {
            bSeparated = FALSE;
        }
    }

    // The above loop does not handle the case of many separators at
    // the beginning of the menu
    while (_SHIsMenuSeparator2(hm, 0, NULL))
    {
        DeleteMenu(hm, 0, MF_BYPOSITION);
    }
}

//
// Types
//

class CShellBrowser
{
public:
    IWebBrowser2 *GetWebBrowser()
    {
        return *((IWebBrowser2 **)this + 42);
    }

    IShellItem *GetShellItem()
    {
        return *((IShellItem **)this + 46);
    }

    LPITEMIDLIST GetPIDL()
    {
        return *((LPITEMIDLIST *)this + 47);
    }
};

class CBrowserHost
{
public:
    CShellBrowser *GetShellBrowser()
    {
        return *((CShellBrowser **)this + 19);
    }
};

class CExplorerFrame
{
public:
    CBrowserHost *GetBrowserHost()
    {
        return *((CBrowserHost **)this + 32);
    }

    CShellBrowser *GetShellBrowser()
    {
        CBrowserHost *pbh = GetBrowserHost();
        if (pbh)
        {
            return pbh->GetShellBrowser();
        }
        return nullptr;
    }

    BOOL _OnTimer(HWND hwnd, UINT_PTR idTimer)
    {
        // HACK: _OnSysMenuClick uses the cursor coords as the timer ID.
        // So first check if g_sysmenuTimer is set before checking for
        // standard timer IDs.

        if (g_sysmenuTimer == idTimer)
        {
            KillTimer(hwnd, g_sysmenuTimer);
            g_sysmenuTimer = 0;

            // the timer ID is the lParam from the left click!
            #define WM_SYSMENU                      0x0313
            SendMessageW(hwnd, WM_SYSMENU, 0, idTimer);

            return TRUE;
        }

        return FALSE;
    }

    BOOL _OnSysMenuClick(HWND hwnd, BOOL bLeftClick, WPARAM wParam, LPARAM lParam)
    {
        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        DWORD dwStart = GetTickCount();

        MapWindowPoints(NULL, hwnd, &pt, 1);

        if (!CheckForDragBegin(hwnd, pt.x, pt.y))
        {
            if (bLeftClick)
            {
                DWORD dwDelta = (GetTickCount() - dwStart);
                DWORD dwDblClick = GetDoubleClickTime();

                if (dwDelta < dwDblClick)
                {
                    // HACK: use the lParam (coords) as the timer ID to communicate
                    // that to the WM_TIMER handler
                    //
                    // HACK: store the timer id in a global. Since there's only one
                    // double-click on a sysmenu at a time, this should be fine.
                    if (g_sysmenuTimer)
                        KillTimer(hwnd, g_sysmenuTimer);

                    // We are special casing 0 as meaning there is no timer, so if the coords come in at
                    // 0 then cheat them to 1.
                    if (lParam == 0)
                        lParam++;

                    g_sysmenuTimer = SetTimer(hwnd, lParam, dwDblClick - dwDelta, NULL);
                }
                else
                    DefWindowProcW(hwnd, WM_CONTEXTMENU, (WPARAM)hwnd, lParam);
            }
            else
                SendMessage(hwnd, WM_CONTEXTMENU, (WPARAM)hwnd, lParam);
            return FALSE;
        }

        CShellBrowser *psb = GetShellBrowser();
        LPITEMIDLIST pidl = NULL;
        if (psb)
            pidl = psb->GetPIDL();

        return SUCCEEDED(SHDoDragDropWithPreferredEffect(hwnd, nullptr, pidl, DROPEFFECT_LINK, 0));
    }

    BOOL _OnContextMenu(HWND hwnd, WPARAM wParam, LPARAM lParam)
    {
        BOOL fProcessed = FALSE;
        CShellBrowser *psb = GetShellBrowser();
        IBindCtx *pbc;
        LPITEMIDLIST pidl = NULL;
        if (psb)
            pidl = psb->GetPIDL();

        if (pidl
        && !ILIsEmpty(pidl)
        && SendMessageW(hwnd, WM_NCHITTEST, 0, lParam) == HTSYSMENU
        && !SHRestricted(REST_NOVIEWCONTEXTMENU)
        && SUCCEEDED(BindCtx_RegisterUIWindow(nullptr, hwnd, &pbc)))
        {
            IShellItem *psi = psb->GetShellItem();
            if (psi)
            {
                IContextMenu *pcm;
                if (SUCCEEDED(psi->BindToHandler(pbc, BHID_SFUIObject, IID_PPV_ARGS(&pcm))))
                {
                    HMENU hpopup = SHLoadMenuPopup(g_hExplorerFrame, 265);
                    if (hpopup)
                    {
                        UINT uFlags = 0;
                        if (GetKeyState(VK_SHIFT) < 0)
                            uFlags = CMF_EXTENDEDVERBS;
                        pcm->QueryContextMenu(hpopup, GetMenuItemCount(hpopup), 2, 0x7FFF, uFlags);

                        ContextMenu_DeleteCommandByName(pcm, hpopup, 2, L"open");
                        ContextMenu_DeleteCommandByName(pcm, hpopup, 2, L"delete");
                        ContextMenu_DeleteCommandByName(pcm, hpopup, 2, L"link");
                        _SHPrettyMenu(hpopup);

                        if (GetMenuItemCount(hpopup) > 1)
                        {
                            fProcessed = TRUE;

                            UINT idCmd = TrackPopupMenu(
                                hpopup, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_LEFTALIGN,
                                GET_X_LPARAM(lParam),
                                GET_Y_LPARAM(lParam),
                                0, hwnd, nullptr);
                            switch (idCmd)
                            {
                                case 0:
                                    break;

                                case 1:
                                    psb->GetWebBrowser()->Quit();
                                    break;

                                default:
                                {
                                    CHAR szPath[MAX_PATH];
                                    CMINVOKECOMMANDINFO ici = {
                                        sizeof(ici),
                                        0,
                                        hwnd,
                                        (LPCSTR)MAKEINTRESOURCE(idCmd - 2),
                                        NULL, NULL,
                                        SW_NORMAL
                                    };

                                    SHGetPathFromIDListA(pidl, szPath);
                                    ici.lpDirectory = szPath;
                                    ici.fMask |= CMIC_MASK_UNICODE;

                                    pcm->InvokeCommand(&ici);
                                }
                            }
                        }

                        DestroyMenu(hpopup);
                    }

                    pcm->Release();
                }
            }

            pbc->Release();
        }
        return fProcessed;
    }
};

LRESULT (CALLBACK *CExplorerFrame_v_WndProc_orig)(CExplorerFrame *, HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CExplorerFrame_v_WndProc_hook(
    CExplorerFrame *pThis,
    HWND            hwnd,
    UINT            uMsg,
    WPARAM          wParam,
    LPARAM          lParam
)
{
    switch (uMsg)
    {
        case WM_TIMER:
            if (pThis->_OnTimer(hwnd, wParam))
                return 0;
            break;
        
        case WM_NCLBUTTONDOWN:
        case WM_NCRBUTTONDOWN:
            if (wParam == HTSYSMENU)
            {
                pThis->_OnSysMenuClick(hwnd, uMsg == WM_NCLBUTTONDOWN, wParam, lParam);
                return 0;
            }
            break;

        case WM_CONTEXTMENU:
            if (pThis->_OnContextMenu(hwnd, wParam, lParam))
                return 0;
            break;
    }

    return CExplorerFrame_v_WndProc_orig(pThis, hwnd, uMsg, wParam, lParam);
}

const WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
    {
        {
            L"private: virtual __int64 __cdecl CExplorerFrame::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
        },
        &CExplorerFrame_v_WndProc_orig,
        CExplorerFrame_v_WndProc_hook,
        false
    },
    {
        {
            L"static  BindCtx_RegisterUIWindow()"
        },
        &BindCtx_RegisterUIWindow,
        nullptr,
        false
    },
    {
        {
            L"long __cdecl ContextMenu_DeleteCommandByName(struct IContextMenu *,struct HMENU__ *,unsigned int,unsigned short const *)"
        },
        &ContextMenu_DeleteCommandByName,
        nullptr,
        false
    },
};

BOOL Wh_ModInit(void)
{
    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    SHDoDragDropWithPreferredEffect = (SHDoDragDropWithPreferredEffect_t)GetProcAddress(hShell32, (LPCSTR)884);
    if (!SHDoDragDropWithPreferredEffect)
    {
        Wh_Log(L"Failed to get address of SHDoDragDropWithPreferredEffect in shell32.dll");
        return FALSE;
    }

    HMODULE hShlwapi = LoadLibraryExW(L"shlwapi.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShlwapi)
    {
        Wh_Log(L"Failed to load shlwapi.dll");
        return FALSE;
    }

    SHLoadMenuPopup = (SHLoadMenuPopup_t)GetProcAddress(hShlwapi, (LPCSTR)177);
    if (!SHLoadMenuPopup)
    {
        Wh_Log(L"Failed to get address of SHLoadMenuPopup in shlwapi.dll");
        return FALSE;
    }

    g_hExplorerFrame = LoadLibraryExW(L"ExplorerFrame.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hExplorerFrame)
    {
        Wh_Log(L"Failed to load ExplorerFrame.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        g_hExplorerFrame,
        explorerFrameDllHooks,
        ARRAYSIZE(explorerFrameDllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in ExplorerFrame.dll");
        return FALSE;
    }

    return TRUE;
}