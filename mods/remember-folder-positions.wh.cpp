// ==WindhawkMod==
// @id              remember-folder-positions
// @name            Remember folder positions
// @description     Remembers the folder windows' positions the way it was pre-Vista (Win95-WinXP)
// @version         1.0
// @author          anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod re-implements the functionality removed from Windows Explorer after Windows XP, that is,
the remembering of the positions of the folder windows.

This mod makes the utility ShellFolderFix unnecessary.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <unordered_map>
#include <commctrl.h>

std::unordered_map<HWND, int> g_windowToBag;
HWND g_lastCreatedWindow = NULL;

typedef LONG (WINAPI *REGQUERYVALUEEXW)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
typedef HWND (WINAPI *CREATEWINDOWEXW)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);

REGQUERYVALUEEXW pOriginalRegQueryValueExW;
CREATEWINDOWEXW pOriginalCreateWindowExW;

void GetWinPosValueName(WCHAR* valueName, size_t size) {
    HDC hdc = GetDC(NULL);
    int width = GetDeviceCaps(hdc, HORZRES);
    int height = GetDeviceCaps(hdc, VERTRES);
    int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(NULL, hdc);
    
    wsprintfW(valueName, L"WinPos%dx%dx%d", width, height, dpi);
}

int ExtractBagNumber(HKEY hKey) {
    typedef LONG (WINAPI *NTQUERYKEY)(HANDLE, int, PVOID, ULONG, PULONG);
    static NTQUERYKEY pNtQueryKey = (NTQUERYKEY)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryKey");
    
    if (!pNtQueryKey) return -1;
    
    BYTE buffer[1024];
    ULONG resultLength;
    if (pNtQueryKey(hKey, 3, buffer, sizeof(buffer), &resultLength) != 0) return -1;
    
    WCHAR* keyPath = (WCHAR*)(buffer + sizeof(ULONG));
    const WCHAR* bags = wcsstr(keyPath, L"\\Bags\\");
    if (!bags) return -1;
    
    bags += 6;
    if (wcsstr(bags, L"AllFolders") == bags) return -1;
    
    return _wtoi(bags);
}

void ApplyWinPos(HWND hWnd, int bagNum) {
    WCHAR regPath[256];
    wsprintfW(regPath, L"Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\Bags\\%d\\Shell", bagNum);
    
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, regPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS) return;
    
    WCHAR valueName[64];
    GetWinPosValueName(valueName, 64);
    
    WINDOWPLACEMENT wp = {sizeof(WINDOWPLACEMENT)};
    DWORD dataSize = sizeof(wp);
    
    if (RegQueryValueExW(hKey, valueName, NULL, NULL, (BYTE*)&wp, &dataSize) == ERROR_SUCCESS && dataSize == sizeof(wp)) {
        wp.length = sizeof(WINDOWPLACEMENT);
        SetWindowPlacement(hWnd, &wp);
    }
    
    RegCloseKey(hKey);
}

void SaveWinPos(HWND hWnd, int bagNum) {
    WINDOWPLACEMENT wp = {sizeof(WINDOWPLACEMENT)};
    if (!GetWindowPlacement(hWnd, &wp)) return;
    
    WCHAR regPath[256];
    wsprintfW(regPath, L"Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\Bags\\%d\\Shell", bagNum);
    
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, regPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        WCHAR valueName[64];
        GetWinPosValueName(valueName, 64);
        
        RegSetValueExW(hKey, valueName, 0, REG_BINARY, (BYTE*)&wp, sizeof(wp));
        RegCloseKey(hKey);
    }
}

LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_DESTROY) {
        auto it = g_windowToBag.find(hWnd);
        if (it != g_windowToBag.end() && it->second > 0) {
            SaveWinPos(hWnd, it->second);
            g_windowToBag.erase(it);
        }
        RemoveWindowSubclass(hWnd, SubclassProc, 0);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, 
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = pOriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, 
                                          X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    if (hWnd && lpClassName && HIWORD(lpClassName) != 0 && wcscmp(lpClassName, L"CabinetWClass") == 0) {
        g_lastCreatedWindow = hWnd;
        g_windowToBag[hWnd] = -1;
        SetWindowSubclass(hWnd, SubclassProc, 0, 0);
    }
    
    return hWnd;
}

LONG WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {   
    int bagNum = ExtractBagNumber(hKey);
    
    if (bagNum > 0 && g_lastCreatedWindow && IsWindow(g_lastCreatedWindow)) {
        auto it = g_windowToBag.find(g_lastCreatedWindow);
        if (it != g_windowToBag.end() && it->second == -1) {
            it->second = bagNum;
            ApplyWinPos(g_lastCreatedWindow, bagNum);
            g_lastCreatedWindow = NULL;
        }
    }

    return pOriginalRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

BOOL Wh_ModInit(void) {
    Wh_Log(L"Init");

    HMODULE hKernelBase = LoadLibraryW(L"kernelbase.dll");
    HMODULE hUser32 = LoadLibraryW(L"user32.dll");

    Wh_SetFunctionHook((void*)GetProcAddress(hKernelBase, "RegQueryValueExW"), 
                       (void*)RegQueryValueExWHook, (void**)&pOriginalRegQueryValueExW);
    Wh_SetFunctionHook((void*)GetProcAddress(hUser32, "CreateWindowExW"), 
                       (void*)CreateWindowExWHook, (void**)&pOriginalCreateWindowExW);

    return TRUE;
}
