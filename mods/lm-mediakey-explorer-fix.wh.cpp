// ==WindhawkMod==
// @id           lm-mediakey-explorer-fix
// @name         Play-Media-Key fix in Explorer
// @description  Fix the Media 'play' key being suppressed in Explorer when a file is selected.
// @version      1.0
// @author       Mark Jansen
// @github       https://github.com/learn-more
// @twitter      https://twitter.com/learn_more
// @include      explorer.exe
// @compilerOptions -luuid -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Play media key fix
When a file is selected in Explorer, and the 'play' media key is pressed (a physical key on some keyboards),
the play event never reaches a media player like Spotify.
This mod makes sure that the play event is not sent to the file, but to the media player instead.
*/
// ==/WindhawkModReadme==
#include <shlobj.h>


WNDPROC pSHELLDLL_DefViewProc = NULL;

LRESULT CALLBACK SHELLDLL_DefViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_APPCOMMAND && GET_APPCOMMAND_LPARAM(lParam) == APPCOMMAND_MEDIA_PLAY_PAUSE)
    {
        Wh_Log(L"Forwarding APPCOMMAND_MEDIA_PLAY_PAUSE");
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
    return pSHELLDLL_DefViewProc(hWnd, uMsg, wParam, lParam);
}


BOOL Wh_ModInit()
{
    // Check if the class is already registered (it's lazy-initialized on first use)
    WNDCLASSW SHELLDLL_DefView = {};
    BOOL bRet;
    if (!(bRet = GetClassInfoW(GetModuleHandleW(L"shell32.dll"), L"SHELLDLL_DefView", &SHELLDLL_DefView)))
    {
        Wh_Log(L"SHELLDLL_DefView not available yet, trying to register it..");
        // Initialize COM and open an apartment
        HRESULT hrInit = CoInitialize(NULL);
        if (SUCCEEDED(hrInit) || hrInit == RPC_E_CHANGED_MODE)
        {
            IExplorerBrowser* pExplorerBrowser;
            // Create a browser object
            HRESULT hr = SHCoCreateInstance(NULL, &CLSID_ExplorerBrowser, NULL, IID_PPV_ARGS(&pExplorerBrowser));
            if (SUCCEEDED(hr))
            {
                RECT rc = {};
                FOLDERSETTINGS fs = { FVM_DETAILS };
                hr = pExplorerBrowser->Initialize(GetDesktopWindow(), &rc, &fs);
                if (SUCCEEDED(hr))
                {
                    ITEMIDLIST desktop = {};
                    // Browsing to a folder will create the view
                    hr = pExplorerBrowser->BrowseToIDList(&desktop, SBSP_ABSOLUTE);
                    if (SUCCEEDED(hr))
                    {
                        // Now the class should be registered!
                        bRet = GetClassInfoW(GetModuleHandleW(L"shell32.dll"), L"SHELLDLL_DefView", &SHELLDLL_DefView);
                    }
                    else
                    {
                        Wh_Log(L"BrowseToIDList failed with 0x%p", hr);
                    }
                }
                else
                {
                    Wh_Log(L"Initialize failed with 0x%p", hr);
                }
                pExplorerBrowser->Destroy();
                pExplorerBrowser->Release();
            }
            else
            {
                Wh_Log(L"SHCoCreateInstance failed with 0x%p", hr);
            }
            if (hrInit != RPC_E_CHANGED_MODE)
            {
                CoUninitialize();
            }
        }
        else
        {
            Wh_Log(L"SHCoCreateInstance failed with 0x%p", hrInit);
        }
    }

    if (bRet)
    {
        Wh_Log(L"Hook WndProc");
        Wh_SetFunctionHook((VOID*)SHELLDLL_DefView.lpfnWndProc, (void*)SHELLDLL_DefViewProc, (void**)&pSHELLDLL_DefViewProc);
    }

    return TRUE;
}
