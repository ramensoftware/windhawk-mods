// ==WindhawkMod==
// @id              hide-search-bar
// @name            Hide Search Bar
// @description     Removes the search bar that is next to the file explorer
// @version         1.0.2
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Hide Search Bar
This mod removes the search bar in File Explorer.

Note: It expands itself to the right to take place of the search bar that was removed.

The code is based on the implementation in [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher).

![Screenshot](https://raw.githubusercontent.com/ItsProfessional/Screenshots/main/Windhawk/hide-search-bar.png)
*/
// ==/WindhawkModReadme==

#include <commctrl.h>
#include <ntstatus.h>
#include <unordered_set>

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) -> return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI -> return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) -> return_t
#endif


std::unordered_set<HWND> handles;

UINT g_subclassRegisteredMsg = RegisterWindowMessage(L"Windhawk_SetWindowSubclassFromAnyThread_hide-search-bar");

#define OSVERSION_INVALID 0xffffffff



using VnRtlGetVersionPtr = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW);
BOOL VnGetOSVersion(PRTL_OSVERSIONINFOW lpRovi)
{
    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");

    VnRtlGetVersionPtr fxPtr = reinterpret_cast<VnRtlGetVersionPtr>(GetProcAddress(hMod, "RtlGetVersion"));

    lpRovi->dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
    if (STATUS_SUCCESS == fxPtr(lpRovi)) return TRUE;

    return FALSE;
}


BOOL IsWindows11Version22H2OrHigher()
{
    RTL_OSVERSIONINFOW lpRovi; 
    VnGetOSVersion(&lpRovi);

    if (lpRovi.dwBuildNumber >= 22621) return TRUE;

    return FALSE;
}



HWND FindChildWindow(HWND hwndParent, wchar_t* lpszClass)
{
    HWND hwnd = FindWindowEx(hwndParent, NULL, lpszClass, NULL);

    if (hwnd == NULL)
    {
        HWND hwndChild = FindWindowEx(hwndParent, NULL, NULL, NULL);

        while (hwndChild != NULL && hwnd == NULL)
        {
            hwnd = FindChildWindow(hwndChild, lpszClass);
            if (hwnd == NULL) hwndChild = FindWindowEx(hwndParent, hwndChild, NULL, NULL);
        }
    }

    return hwnd;
}




VOID HideExplorerSearchBar(HWND hWnd)
{
    HWND band = NULL, rebar = NULL;
    band = FindChildWindow(hWnd, (wchar_t*)L"TravelBand");
    if (!band) return;

    rebar = GetParent(band);
    if (!rebar) return;

    int idx = 0;

    idx = (int)SendMessage(rebar, RB_IDTOINDEX, 4, 0);
    if (idx >= 0) SendMessage(rebar, RB_SHOWBAND, idx, FALSE);

    idx = (int)SendMessage( rebar, RB_IDTOINDEX, 5, 0);
    if (idx >= 0) SendMessage(rebar, RB_SHOWBAND, idx, TRUE);

    RedrawWindow(rebar, NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);
}


LRESULT CALLBACK HideExplorerSearchBarSubClass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_DESTROY)
        RemoveWindowSubclass(hWnd, HideExplorerSearchBarSubClass, 0);
    else if (uMsg == g_subclassRegisteredMsg && !wParam)
        RemoveWindowSubclass(hWnd, HideExplorerSearchBarSubClass, 0);

    else if (uMsg == WM_SIZE && IsWindows11Version22H2OrHigher()) HideExplorerSearchBar(hWnd);
    else if (uMsg == WM_PARENTNOTIFY && (WORD)wParam == 1) HideExplorerSearchBar(hWnd);

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}



BOOL SetWindowSubclassFromAnyThread(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
		SUBCLASSPROC pfnSubclass;
		UINT_PTR uIdSubclass;
		DWORD_PTR dwRefData;
		BOOL result;
	};

	DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
	if (dwThreadId == 0) return FALSE;

	if (dwThreadId == GetCurrentThreadId()) return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);

	HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI_LAMBDA_RETURN(LRESULT) {
		if (nCode == HC_ACTION) {
			const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
            
			if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
				SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param = (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
				param->result = SetWindowSubclass(cwp->hwnd, param->pfnSubclass, param->uIdSubclass, param->dwRefData);
			}
		}

		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}, nullptr, dwThreadId);

	if (!hook) return FALSE;

	SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
	param.pfnSubclass = pfnSubclass;
	param.uIdSubclass = uIdSubclass;
	param.dwRefData = dwRefData;
	param.result = FALSE;
	SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (WPARAM)&param);

	UnhookWindowsHookEx(hook);

	return param.result;
}



HWND(WINAPI *pOriginalSHCreateWorkerWindow)(WNDPROC wndProc, HWND hWndParent, DWORD dwExStyle, DWORD dwStyle, HMENU hMenu, LONG_PTR wnd_extra);
HWND WINAPI SHCreateWorkerWindowHook(WNDPROC wndProc, HWND hWndParent, DWORD dwExStyle, DWORD dwStyle, HMENU hMenu, LONG_PTR wnd_extra)
{
    HWND result = pOriginalSHCreateWorkerWindow(wndProc, hWndParent, dwExStyle, dwStyle, hMenu, wnd_extra);
    
    // Wh_Log("%x %x", dwExStyle, dwStyle);

    if (dwExStyle == 0x10000 && dwStyle == 0x46000000 && result) {
        handles.insert(hWndParent);
        SetWindowSubclassFromAnyThread(hWndParent, HideExplorerSearchBarSubClass, 0, 0);
    }

    return result;
}


BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    
    HMODULE hShcore = GetModuleHandle(L"shcore.dll");

    void* origFunc = (void*)GetProcAddress(hShcore, (LPCSTR)188);
    Wh_SetFunctionHook(origFunc, (void*)SHCreateWorkerWindowHook, (void**)&pOriginalSHCreateWorkerWindow);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    // Unsubclass
    for (const HWND& hWnd : handles) SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}
