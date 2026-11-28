// ==WindhawkMod==
// @id              desktop-icons-toggle
// @name            Desktop Icons Toggle (Middle-Click)
// @description     Middle-click the desktop to instantly show/hide desktop icons
// @version         1.0
// @author          Guhapriyan
// @github          https://github.com/Guhapriyan-GP
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*...*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shlguid.h>
#include <exdisp.h>

// ---------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------

struct {
    bool enableMiddleClick;
} g_settings;

void LoadSettings() {
    g_settings.enableMiddleClick = Wh_GetIntSetting(L"enableMiddleClick") != 0;
}

// ---------------------------------------------------------------------
// Desktop window discovery
// ---------------------------------------------------------------------

// Icons visible: the child SysListView32 covers the desktop and gets
// clicks first. Icons hidden: the listview shrinks/hides, so its parent
// SHELLDLL_DefView gets clicks instead. We hook both so a click is caught
// in either state.
HWND g_hListView = nullptr;
WNDPROC g_ListViewOriginalProc = nullptr;

HWND g_hDefView = nullptr;
WNDPROC g_DefViewOriginalProc = nullptr;

HANDLE g_hRehookThread = nullptr;
volatile bool g_unloading = false;
volatile bool g_iconsHidden = false;

HWND FindDesktopDefView() {
    HWND hProgman = FindWindowW(L"Progman", nullptr);
    HWND hDefView = FindWindowExW(hProgman, nullptr, L"SHELLDLL_DefView", nullptr);

    if (!hDefView) {
        HWND hWorkerW = nullptr;
        while ((hWorkerW = FindWindowExW(nullptr, hWorkerW, L"WorkerW", nullptr))) {
            hDefView = FindWindowExW(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
            if (hDefView) {
                break;
            }
        }
    }

    return hDefView;
}

HWND FindDesktopListView(HWND hDefView) {
    if (!hDefView) {
        return nullptr;
    }
    return FindWindowExW(hDefView, nullptr, L"SysListView32", nullptr);
}

// ---------------------------------------------------------------------
// Toggle implementation
// ---------------------------------------------------------------------

bool ToggleDesktopIconsViaCOM() {
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool comInitializedHere = (hrInit == S_OK || hrInit == S_FALSE);

    bool result = false;
    IShellWindows* psw = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                   IID_IShellWindows, (void**)&psw);
    if (SUCCEEDED(hr) && psw) {
        VARIANT vEmpty;
        VariantInit(&vEmpty);
        long hwndLong = 0;
        IDispatch* pdisp = nullptr;

        hr = psw->FindWindowSW(&vEmpty, &vEmpty, SWC_DESKTOP, &hwndLong,
                                SWFO_NEEDDISPATCH, &pdisp);
        if (SUCCEEDED(hr) && pdisp) {
            IServiceProvider* psp = nullptr;
            if (SUCCEEDED(pdisp->QueryInterface(IID_IServiceProvider, (void**)&psp))) {
                IShellBrowser* psb = nullptr;
                if (SUCCEEDED(psp->QueryService(SID_STopLevelBrowser, IID_IShellBrowser,
                                                 (void**)&psb))) {
                    IShellView* psv = nullptr;
                    if (SUCCEEDED(psb->QueryActiveShellView(&psv))) {
                        IFolderView2* pfv2 = nullptr;
                        if (SUCCEEDED(psv->QueryInterface(IID_IFolderView2, (void**)&pfv2))) {
                            bool wantHidden = !g_iconsHidden;
                            DWORD newMask = wantHidden ? FWF_NOICONS : 0;

                            pfv2->SetRedraw(FALSE);
                            HRESULT hrSet = pfv2->SetCurrentFolderFlags(FWF_NOICONS, newMask);
                            pfv2->SetRedraw(TRUE);

                            if (SUCCEEDED(hrSet)) {
                                g_iconsHidden = wantHidden;
                                result = true;

                                if (g_hDefView) {
                                    InvalidateRect(g_hDefView, nullptr, TRUE);
                                    UpdateWindow(g_hDefView);
                                }
                                if (g_hListView) {
                                    InvalidateRect(g_hListView, nullptr, TRUE);
                                    UpdateWindow(g_hListView);
                                }
                            } else {
                                Wh_Log(L"SetCurrentFolderFlags failed, hr=0x%08X", hrSet);
                            }
                            pfv2->Release();
                        }
                        psv->Release();
                    }
                    psb->Release();
                }
                psp->Release();
            }
            pdisp->Release();
        } else {
            Wh_Log(L"FindWindowSW failed, hr=0x%08X", hr);
        }
        psw->Release();
    } else {
        Wh_Log(L"CoCreateInstance(ShellWindows) failed, hr=0x%08X", hr);
    }

    if (comInitializedHere) {
        CoUninitialize();
    }
    return result;
}

void HandleToggleRequest(const wchar_t* source) {
    Wh_Log(L"Toggle requested via %s", source);
    ToggleDesktopIconsViaCOM();
}

// ---------------------------------------------------------------------
// Window subclassing
// ---------------------------------------------------------------------

LRESULT CALLBACK ListViewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_MBUTTONDOWN && g_settings.enableMiddleClick) {
        HandleToggleRequest(L"middle-click (SysListView32)");
    }
    if (hWnd == g_hListView && g_ListViewOriginalProc) {
        return CallWindowProcW(g_ListViewOriginalProc, hWnd, uMsg, wParam, lParam);
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DefViewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_MBUTTONDOWN && g_settings.enableMiddleClick) {
        HandleToggleRequest(L"middle-click (SHELLDLL_DefView)");
    }
    if (hWnd == g_hDefView && g_DefViewOriginalProc) {
        return CallWindowProcW(g_DefViewOriginalProc, hWnd, uMsg, wParam, lParam);
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

bool HookWindow(HWND hWnd, WNDPROC newProc, HWND* pStoredHwnd, WNDPROC* pStoredOriginal,
                 const wchar_t* label) {
    if (!hWnd) {
        return false;
    }
    if (hWnd == *pStoredHwnd) {
        return true;  // Already hooked.
    }

    WNDPROC original = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)newProc);
    if (!original) {
        Wh_Log(L"SetWindowLongPtr failed for %s, last error = %lu", label, GetLastError());
        return false;
    }

    *pStoredOriginal = original;
    *pStoredHwnd = hWnd;
    Wh_Log(L"Hooked %s", label);
    return true;
}

void UnhookWindow(HWND* pStoredHwnd, WNDPROC* pStoredOriginal, WNDPROC ourProc) {
    if (*pStoredHwnd && *pStoredOriginal) {
        WNDPROC current = (WNDPROC)GetWindowLongPtrW(*pStoredHwnd, GWLP_WNDPROC);
        if (current == ourProc) {
            SetWindowLongPtrW(*pStoredHwnd, GWLP_WNDPROC, (LONG_PTR)*pStoredOriginal);
        }
    }
    *pStoredHwnd = nullptr;
    *pStoredOriginal = nullptr;
}

void HookDesktopWindows() {
    HWND hDefView = FindDesktopDefView();
    HookWindow(hDefView, DefViewWndProc, &g_hDefView, &g_DefViewOriginalProc,
               L"SHELLDLL_DefView");

    HWND hListView = FindDesktopListView(hDefView);
    HookWindow(hListView, ListViewWndProc, &g_hListView, &g_ListViewOriginalProc,
               L"SysListView32");
}

DWORD WINAPI RehookThreadProc(LPVOID) {
    while (!g_unloading) {
        HookDesktopWindows();
        Sleep(1000);
    }
    return 0;
}

// ---------------------------------------------------------------------
// Mod entry points
// ---------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();
    HookDesktopWindows();

    g_unloading = false;
    g_hRehookThread = CreateThread(nullptr, 0, RehookThreadProc, nullptr, 0, nullptr);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    g_unloading = true;
    if (g_hRehookThread) {
        WaitForSingleObject(g_hRehookThread, 5000);
        CloseHandle(g_hRehookThread);
        g_hRehookThread = nullptr;
    }

    UnhookWindow(&g_hListView, &g_ListViewOriginalProc, ListViewWndProc);
    UnhookWindow(&g_hDefView, &g_DefViewOriginalProc, DefViewWndProc);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
