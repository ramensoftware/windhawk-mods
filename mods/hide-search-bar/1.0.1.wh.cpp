// ==WindhawkMod==
// @id              hide-search-bar
// @name            Hide Search Bar
// @description     Removes the search bar that is next to the file explorer
// @version         1.0.1
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Hide Search Bar
This mod removes the search bar in File Explorer.

It also expands itself to the right to take place of the search bar that was removed.

The code is based on the implementation in [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher).

![Screenshot](https://raw.githubusercontent.com/ItsProfessional/Screenshots/main/Windhawk/hide-search-bar.png)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <objbase.h>
#include <initguid.h>
#include <commctrl.h>
#include <dwmapi.h>

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) -> return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI -> return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) -> return_t
#endif

DWORD g_uiThreadId;


#define UNIFIEDBUILDREVISION_KEY                        L"\\Registry\\Machine\\Software\\Microsoft\\Windows NT\\CurrentVersion"
#define UNIFIEDBUILDREVISION_VALUE                      L"UBR"

#define _LIBVALINET_HOOKING_OSVERSION_INVALID 0xffffffff
#define STATUS_SUCCESS (0x00000000)

using VnRtlGetVersionPtr = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW);

BOOL VnGetOSVersion(PRTL_OSVERSIONINFOW lpRovi)
{
    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
    if (hMod != nullptr)
    {
        VnRtlGetVersionPtr fxPtr = reinterpret_cast<VnRtlGetVersionPtr>(GetProcAddress(hMod, "RtlGetVersion"));

        if (fxPtr != nullptr)
        {
            lpRovi->dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
            if (STATUS_SUCCESS == fxPtr(lpRovi)) return TRUE;
        }
    }

    return FALSE;
}

inline DWORD32 VnGetUBR()
{
    DWORD32 ubr = 0, ubr_size = sizeof(DWORD32);
    HKEY hKey;

    LONG lRes = RegOpenKeyExW(
        HKEY_LOCAL_MACHINE,
        wcschr(
            wcschr(
                wcschr(
                    UNIFIEDBUILDREVISION_KEY,
                    '\\'
                ) + 1,
                '\\'
            ) + 1,
            '\\'
        ) + 1,
        0,
        KEY_READ,
        &hKey
    );


    if (lRes == ERROR_SUCCESS) RegQueryValueExW(hKey, UNIFIEDBUILDREVISION_VALUE, 0, nullptr, reinterpret_cast<LPBYTE>(&ubr), reinterpret_cast<LPDWORD>(&ubr_size));
    
    return ubr;
}

inline DWORD32 VnGetOSVersionAndUBR(PRTL_OSVERSIONINFOW lpRovi)
{
    if (!VnGetOSVersion(lpRovi)) return _LIBVALINET_HOOKING_OSVERSION_INVALID;

    return VnGetUBR();
}





// This allows compiling with older Windows SDKs as well
#ifndef NTDDI_WIN10_CO
#define DWMWA_USE_HOSTBACKDROPBRUSH 17 // [set] BOOL, Allows the use of host backdrop brushes for the window.
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20 // [set] BOOL, Allows a window to either use the accent color, or dark, according to the user Color Mode preferences.
#define DWMWA_WINDOW_CORNER_PREFERENCE 33 // [set] WINDOW_CORNER_PREFERENCE, Controls the policy that rounds top-level window corners
#define DWMWA_BORDER_COLOR 34 // [set] COLORREF, The color of the thin border around a top-level window
#define DWMWA_CAPTION_COLOR 35 // [set] COLORREF, The color of the caption
#define DWMWA_TEXT_COLOR 36 // [set] COLORREF, The color of the caption text
#define DWMWA_VISIBLE_FRAME_BORDER_THICKNESS 37 // [get] UINT, width of the visible border around a thick frame window
#define DWMWCP_DEFAULT 0
#define DWMWCP_DONOTROUND 1
#define DWMWCP_ROUND 2
#define DWMWCP_ROUNDSMALL 3
#endif

RTL_OSVERSIONINFOW global_rovi;
DWORD32 global_ubr;

void InitializeGlobalVersionAndUBR()
{
    global_ubr = VnGetOSVersionAndUBR(&global_rovi);
}

inline BOOL IsWindows11()
{
    InitializeGlobalVersionAndUBR();

    if (!global_rovi.dwMajorVersion) global_ubr = VnGetOSVersionAndUBR(&global_rovi);
    if (global_rovi.dwBuildNumber >= 21996) return TRUE;

    return FALSE;
}


inline BOOL IsWindows11Version22H2OrHigher()
{
    InitializeGlobalVersionAndUBR();

    if (!global_rovi.dwMajorVersion) global_ubr = VnGetOSVersionAndUBR(&global_rovi);
    if (global_rovi.dwBuildNumber >= 22621) return TRUE;
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





inline LSTATUS SHRegGetValueFromHKCUHKLMWithOpt(PCWSTR pwszKey, PCWSTR pwszValue, REGSAM samDesired, void* pvData, DWORD* pcbData)
{
    LSTATUS lRes = ERROR_FILE_NOT_FOUND;
    HKEY hKey = NULL;

    RegOpenKeyExW(HKEY_CURRENT_USER, pwszKey, 0, samDesired, &hKey);
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE) hKey = NULL;
    if (hKey)
    {
        lRes = RegQueryValueExW(hKey, pwszValue, 0, NULL, (LPBYTE) pvData, (LPDWORD) pcbData);

        RegCloseKey(hKey);
        if (lRes == ERROR_SUCCESS || lRes == ERROR_MORE_DATA) return lRes;
    }

    RegOpenKeyExW(HKEY_LOCAL_MACHINE, pwszKey, 0, samDesired, &hKey);
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE) hKey = NULL;

    if (hKey)
    {
        lRes = RegQueryValueExW(hKey, pwszValue, 0, NULL, (LPBYTE) pvData, (LPDWORD) pcbData);
        RegCloseKey(hKey);

        if (lRes == ERROR_SUCCESS || lRes == ERROR_MORE_DATA) return lRes;
    }

    return lRes;
}






UINT g_subclassRegisteredMsg = RegisterWindowMessage(L"Windhawk_SetWindowSubclassFromAnyThread_hide-search-bar");

LRESULT CALLBACK HideExplorerSearchBarSubClass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_DESTROY) RemoveWindowSubclass(hWnd, HideExplorerSearchBarSubClass, 0);

    else if (uMsg == WM_SIZE || uMsg == WM_PARENTNOTIFY)
    {
        if (uMsg == WM_SIZE && IsWindows11Version22H2OrHigher()) HideExplorerSearchBar(hWnd);
        else if (uMsg == WM_PARENTNOTIFY && (WORD)wParam == 1) HideExplorerSearchBar(hWnd);
    }
    else if (uMsg == g_subclassRegisteredMsg && !wParam) RemoveWindowSubclass(hWnd, HideExplorerSearchBarSubClass, 0);

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

BOOL CALLBACK EnumBrowserWindowsUnsubclassFunc(HWND hWnd, LPARAM lParam) {
    SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);

    return TRUE;
}


HWND(WINAPI *pOriginalSHCreateWorkerWindow)(WNDPROC wndProc, HWND hWndParent, DWORD dwExStyle, DWORD dwStyle, HMENU hMenu, LONG_PTR wnd_extra);

HWND WINAPI SHCreateWorkerWindowHook(WNDPROC wndProc, HWND hWndParent, DWORD dwExStyle, DWORD dwStyle, HMENU hMenu, LONG_PTR wnd_extra)
{
    HWND result;
    LSTATUS lRes = ERROR_FILE_NOT_FOUND;
    DWORD dwSize = 0;
    
    // Wh_Log("%x %x", dwExStyle, dwStyle);

    if (g_uiThreadId && g_uiThreadId != GetCurrentThreadId()) return pOriginalSHCreateWorkerWindow(wndProc, hWndParent, dwExStyle, dwStyle, hMenu, wnd_extra);
    if (!g_uiThreadId) g_uiThreadId = GetCurrentThreadId();


    if (SHRegGetValueFromHKCUHKLMWithOpt(
        TEXT("SOFTWARE\\Classes\\CLSID\\{056440FD-8568-48e7-A632-72157243B55B}\\InProcServer32"),
        TEXT(""),
        KEY_READ | KEY_WOW64_64KEY,
        NULL,
        (LPDWORD)(&dwSize)
    ) == ERROR_SUCCESS && (dwSize < 4) && dwExStyle == 0x10000 && dwStyle == 1174405120) result = 0;

    else result = pOriginalSHCreateWorkerWindow(wndProc, hWndParent, dwExStyle, dwStyle, hMenu, wnd_extra);

    if (dwExStyle == 0x10000 && dwStyle == 0x46000000 && result) SetWindowSubclassFromAnyThread(hWndParent, HideExplorerSearchBarSubClass, 0, 0);

    return result;
}



HWND hTrayClockWWnd;

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    
    HMODULE hShcore = GetModuleHandle(L"shcore.dll");

    void* origFunc = (void*)GetProcAddress(hShcore, (LPCSTR)188);
    Wh_SetFunctionHook(origFunc, (void*)SHCreateWorkerWindowHook, (void**)&pOriginalSHCreateWorkerWindow);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_uiThreadId != 0) EnumThreadWindows(g_uiThreadId, EnumBrowserWindowsUnsubclassFunc, 0);
}
