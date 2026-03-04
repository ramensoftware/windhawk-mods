// ==WindhawkMod==
// @id           legacy-sound-flyout
// @name         Legacy (Win32) sound volume flyout
// @description  Enables legacy sound volume flyout on Win10 taskbar
// @version      2.0.0
// @author       Anixx
// @github       https://github.com/Anixx
// @compilerOptions -lcomctl32 -lpsapi
// @include      explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

Enables legacy sound volume flyout on Windows 10 taskbar (running under either Windows 10 or Windows 11).

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>
#include <psapi.h>

static DWORD g_lastLaunchTick = 0;
static HWND g_hTrayToolbar = nullptr;
static BYTE* g_sndVolSSOBase = nullptr;
static BYTE* g_sndVolSSOEnd = nullptr;

static bool g_isDragging = false;
static POINT g_mouseDownPos = {0};
static int g_mouseDownButton = -1;

static UINT g_taskbarCreatedMsg = 0;

// -------------------------------------------------------
// sndvol
// -------------------------------------------------------

static DWORD WINAPI LaunchSndvolThread(LPVOID lpParam) {
    POINT pt = *(POINT*)lpParam;
    delete (POINT*)lpParam;
    
    wchar_t sys32[MAX_PATH]{};
    GetSystemDirectoryW(sys32, MAX_PATH);
    
    wchar_t cmdLine[1024]{};
    DWORD encoded = (DWORD)MAKELONG(pt.x, pt.y);
    wsprintfW(cmdLine, L"\"%s\\SndVol.exe\" -f %u", sys32, encoded);
    
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi{};
    
    if (CreateProcessW(nullptr, cmdLine, nullptr, nullptr, FALSE, 
                       0, nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    
    return 0;
}

static void LaunchClassicVolume() {
    DWORD now = GetTickCount();
    if (now - g_lastLaunchTick < 400) return;
    g_lastLaunchTick = now;
    
    POINT* pt = new POINT;
    GetCursorPos(pt);
    
    HANDLE hThread = CreateThread(nullptr, 0, LaunchSndvolThread, pt, 0, nullptr);
    if (hThread) {
        CloseHandle(hThread);
    } else {
        delete pt;
    }
}

// -------------------------------------------------------
// Инициализация SndVolSSO
// -------------------------------------------------------

static bool InitSndVolSSOInfo() {
    if (g_sndVolSSOBase) return true;
    
    HMODULE hSndVolSSO = GetModuleHandleW(L"SndVolSSO.dll");
    if (!hSndVolSSO) return false;
    
    MODULEINFO mi{};
    if (!GetModuleInformation(GetCurrentProcess(), hSndVolSSO, &mi, sizeof(mi))) {
        return false;
    }
    
    g_sndVolSSOBase = (BYTE*)mi.lpBaseOfDll;
    g_sndVolSSOEnd = g_sndVolSSOBase + mi.SizeOfImage;
    return true;
}

// -------------------------------------------------------
// Определение volume icon
// -------------------------------------------------------

static bool IsVolumeButton(HWND hToolbar, int buttonIndex) {
    if (buttonIndex < 0 || !g_sndVolSSOBase) return false;
    
    TBBUTTON tb{};
    if (!SendMessageW(hToolbar, TB_GETBUTTON, buttonIndex, (LPARAM)&tb)) {
        return false;
    }
    
    if (!tb.dwData) return false;
    
    HWND hIconWnd = *(HWND*)tb.dwData;
    if (!hIconWnd || !IsWindow(hIconWnd)) return false;
    
    wchar_t className[256]{};
    if (!GetClassNameW(hIconWnd, className, 256)) return false;
    
    if (wcsncmp(className, L"ATL:", 4) != 0) return false;
    
    const wchar_t* hexPart = className + 4;
    ULONG_PTR addr = 0;
    
    while (*hexPart) {
        wchar_t c = *hexPart;
        int digit = 0;
        
        if (c >= L'0' && c <= L'9') digit = c - L'0';
        else if (c >= L'A' && c <= L'F') digit = 10 + (c - L'A');
        else if (c >= L'a' && c <= L'f') digit = 10 + (c - L'a');
        else break;
        
        addr = (addr << 4) | digit;
        hexPart++;
    }
    
    return (addr >= (ULONG_PTR)g_sndVolSSOBase && addr < (ULONG_PTR)g_sndVolSSOEnd);
}

// -------------------------------------------------------
// Subclass proc
// -------------------------------------------------------

static LRESULT CALLBACK ToolbarSubclassProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    if (msg == WM_LBUTTONDOWN) {
        g_mouseDownPos.x = (int)(short)LOWORD(lParam);
        g_mouseDownPos.y = (int)(short)HIWORD(lParam);
        
        POINT pt = g_mouseDownPos;
        g_mouseDownButton = (int)SendMessageW(hwnd, TB_HITTEST, 0, (LPARAM)&pt);
        g_isDragging = false;
    }
    
    if (msg == WM_MOUSEMOVE && (wParam & MK_LBUTTON)) {
        int x = (int)(short)LOWORD(lParam);
        int y = (int)(short)HIWORD(lParam);
        
        int dx = x - g_mouseDownPos.x;
        int dy = y - g_mouseDownPos.y;
        
        if (dx*dx + dy*dy > 25) {
            g_isDragging = true;
        }
    }
    
    if (msg == WM_LBUTTONUP) {
        if (!g_isDragging) {
            int x = (int)(short)LOWORD(lParam);
            int y = (int)(short)HIWORD(lParam);
            POINT pt = {x, y};
            int hitIndex = (int)SendMessageW(hwnd, TB_HITTEST, 0, (LPARAM)&pt);
            
            if (hitIndex >= 0 && hitIndex == g_mouseDownButton && IsVolumeButton(hwnd, hitIndex)) {
                g_mouseDownButton = -1;
                g_isDragging = false;
                LaunchClassicVolume();
                return 0;
            }
        }
        
        g_mouseDownButton = -1;
        g_isDragging = false;
    }
    
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// -------------------------------------------------------
// Находим toolbar
// -------------------------------------------------------

static HWND FindTrayToolbar() {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTray) return nullptr;
    
    DWORD pid = 0;
    GetWindowThreadProcessId(hTray, &pid);
    if (pid != GetCurrentProcessId()) return nullptr;
    
    HWND hNotify = FindWindowExW(hTray, nullptr, L"TrayNotifyWnd", nullptr);
    if (!hNotify) return nullptr;
    
    HWND hSysPager = FindWindowExW(hNotify, nullptr, L"SysPager", nullptr);
    if (hSysPager) {
        HWND hToolbar = FindWindowExW(hSysPager, nullptr, L"ToolbarWindow32", nullptr);
        if (hToolbar) return hToolbar;
    }
    
    return FindWindowExW(hNotify, nullptr, L"ToolbarWindow32", nullptr);
}

// -------------------------------------------------------
// Subclass setup/remove
// -------------------------------------------------------

static void SetupSubclass() {
    if (g_hTrayToolbar) return;
    if (!InitSndVolSSOInfo()) return;
    
    HWND hToolbar = FindTrayToolbar();
    if (!hToolbar) return;
    
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hToolbar, ToolbarSubclassProc, 0)) {
        g_hTrayToolbar = hToolbar;
        Wh_Log(L"Subclass installed");
    }
}

static void RemoveSubclass() {
    if (g_hTrayToolbar) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_hTrayToolbar, ToolbarSubclassProc);
        g_hTrayToolbar = nullptr;
    }
}

// -------------------------------------------------------
// Хуки
// -------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    if (hwnd && !g_hTrayToolbar && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (wcscmp(lpClassName, L"ToolbarWindow32") == 0) {
            SetupSubclass();
        }
    }
    
    return hwnd;
}

using DefWindowProcW_t = decltype(&DefWindowProcW);
DefWindowProcW_t DefWindowProcW_Original;

LRESULT WINAPI DefWindowProcW_Hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        Wh_Log(L"TaskbarCreated");
        RemoveSubclass();
        g_sndVolSSOBase = nullptr;
        g_sndVolSSOEnd = nullptr;
    }
    return DefWindowProcW_Original(hwnd, msg, wParam, lParam);
}

// -------------------------------------------------------
// Init / Uninit
// -------------------------------------------------------

BOOL Wh_ModInit() {
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    
    SetupSubclass();
    
    Wh_Log(L"Initialized");
    return TRUE;
}

void Wh_ModUninit() {
    RemoveSubclass();
    Wh_Log(L"Uninitialized");
}
