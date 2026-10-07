// ==WindhawkMod==
// @id              windows-spotlight
// @name            Windows Spotlight
// @description     Adds a macOS-like Spotlight search bar via a keyboard shortcut (Default: Alt+Space).
// @version         1.0.0
// @author          Adam Jame Morgan
// @github          https://github.com/your-username
// @include         explorer.exe
// @compilerOptions -lcomctl32 -ldwmapi -lgdi32
// @image           https://raw.githubusercontent.com/your-username/repo-name/main/screenshot1.png
// @image           https://raw.githubusercontent.com/your-username/repo-name/main/screenshot2.png
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- modifier: 1
  $name: "Hotkey Modifier"
  $description: "1 = Alt, 2 = Ctrl, 4 = Shift, 8 = Win. Add values together for combinations (e.g., 5 = Alt+Shift)."
- keycode: 32
  $name: "Hotkey Virtual Key Code"
  $description: "Default is 32 (Spacebar). Reference: https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes"
- defaultSearch: 0
  $name: "Default Search Behavior"
  $description: "0 = Native Windows Search, 1 = Google Search"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <string>

// Definitions for rounded corners on Windows 11
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif

#ifndef EM_SETCUEBANNER
#define EM_SETCUEBANNER 0x1501
#endif

#define WM_UPDATE_HOTKEY (WM_USER + 1)

// --- Globals ---
HANDLE g_hMutex = NULL;
HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;

HWND g_hwndMain = NULL;
HWND g_hwndEdit = NULL;
HFONT g_hFont = NULL;
HBRUSH g_hBrushBg = NULL;

int g_modifier = 1;
int g_keycode = VK_SPACE;
int g_defaultSearch = 0;

// --- Search Logic ---
bool IsUrlOrCommand(const std::wstring& q) {
    if (q.find(L"http://") == 0 || q.find(L"https://") == 0 || q.find(L"www.") == 0) return true;
    if (q.find(L"\\\\") == 0) return true; // UNC paths
    if (q.length() >= 2 && q[1] == L':') return true; // Hard drive paths
    
    // Check if the query refers to an executable, control panel item, or batch file in the system PATH
    wchar_t outPath[MAX_PATH];
    if (SearchPathW(NULL, q.c_str(), L".exe", MAX_PATH, outPath, NULL)) return true;
    if (SearchPathW(NULL, q.c_str(), L".msc", MAX_PATH, outPath, NULL)) return true;
    if (SearchPathW(NULL, q.c_str(), L".cpl", MAX_PATH, outPath, NULL)) return true;
    if (SearchPathW(NULL, q.c_str(), L".bat", MAX_PATH, outPath, NULL)) return true;
    if (SearchPathW(NULL, q.c_str(), NULL, MAX_PATH, outPath, NULL)) return true;
    
    return false;
}

void ExecuteSearch(HWND hEdit) {
    wchar_t buf[1024];
    GetWindowTextW(hEdit, buf, 1024);
    std::wstring query = buf;
    if (query.empty()) return;

    if (IsUrlOrCommand(query)) {
        // Direct execution (Apps, URLs, Folders)
        ShellExecuteW(NULL, L"open", query.c_str(), NULL, NULL, SW_SHOWNORMAL);
    } else {
        // Fallback to phrase queries
        if (g_defaultSearch == 1) { 
            // Google Web Search
            std::wstring url = L"https://www.google.com/search?q=";
            for (wchar_t c : query) {
                if (c == L' ') url += L'+';
                else if (c == L'&') url += L"%26";
                else if (c == L'?') url += L"%3F";
                else if (c == L'#') url += L"%23";
                else if (c == L'+') url += L"%2B";
                else url += c;
            }
            ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
        } else { 
            // Native Windows Search Overlay via search-ms protocol
            std::wstring searchUrl = L"search-ms:query=" + query;
            ShellExecuteW(NULL, L"open", searchUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
    }
}

// --- UI Subclassing & Window Procedures ---
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_KEYDOWN) {
        if (wParam == VK_RETURN) {
            ExecuteSearch(hWnd);
            ShowWindow(GetParent(hWnd), SW_HIDE);
            // Clear text after search
            SetWindowTextW(hWnd, L"");
            return 0; // Handled
        } else if (wParam == VK_ESCAPE) {
            ShowWindow(GetParent(hWnd), SW_HIDE);
            SetWindowTextW(hWnd, L"");
            return 0; // Handled
        }
    }
    if (uMsg == WM_CHAR) {
        // Prevent annoying Windows default error 'beep' on single-line text boxes
        if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
            return 0; 
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE: {
            g_hBrushBg = CreateSolidBrush(RGB(28, 28, 28)); // Dark Theme background

            int hEditHeight = 46;
            int hWindowHeight = 70;
            int yEdit = (hWindowHeight - hEditHeight) / 2;
            int hWindowWidth = 600;
            int wEdit = hWindowWidth - 40; 

            // Create Borderless Edit Control
            g_hwndEdit = CreateWindowExW(0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                20, yEdit, wEdit, hEditHeight, hwnd, NULL, NULL, NULL);

            // Set sleek sizing and fonts
            g_hFont = CreateFontW(32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            
            SendMessageW(g_hwndEdit, WM_SETFONT, (WPARAM)g_hFont, TRUE);
            SendMessageW(g_hwndEdit, EM_SETCUEBANNER, FALSE, (LPARAM)L"Spotlight Search...");
            
            SetWindowSubclass(g_hwndEdit, EditSubclassProc, 1, 0);
            
            RegisterHotKey(hwnd, 1, g_modifier, g_keycode);
            return 0;
        }
        case WM_UPDATE_HOTKEY: {
            UnregisterHotKey(hwnd, 1);
            RegisterHotKey(hwnd, 1, g_modifier, g_keycode);
            return 0;
        }
        case WM_HOTKEY: {
            if (wParam == 1) { 
                if (IsWindowVisible(hwnd)) {
                    ShowWindow(hwnd, SW_HIDE);
                } else {
                    ShowWindow(hwnd, SW_SHOW);
                    SetForegroundWindow(hwnd);
                    SetFocus(g_hwndEdit);
                    SendMessage(g_hwndEdit, EM_SETSEL, 0, -1);
                }
            }
            return 0;
        }
        case WM_ACTIVATE: {
            // Hide the window if user clicks elsewhere
            if (LOWORD(wParam) == WA_INACTIVE) {
                ShowWindow(hwnd, SW_HIDE);
            }
            return 0;
        }
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, RGB(28, 28, 28)); // Dark Mode match
            SetTextColor(hdc, RGB(240, 240, 240)); // Almost white text
            return (LRESULT)g_hBrushBg;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, g_hBrushBg);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY: {
            UnregisterHotKey(hwnd, 1);
            RemoveWindowSubclass(g_hwndEdit, EditSubclassProc, 1);
            if (g_hFont) DeleteObject(g_hFont);
            if (g_hBrushBg) DeleteObject(g_hBrushBg);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// --- Lifecycle & Windhawk Triggers ---
DWORD WINAPI UIThread(LPVOID lpParam) {
    WNDCLASSW wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW; // Adds a subtle native shadow
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkSpotlightClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int w = 600;
    int h = 70;
    int x = (screenW - w) / 2;
    int y = (screenH / 3);

    g_hwndMain = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
        L"WindhawkSpotlightClass", L"Spotlight",
        WS_POPUP,
        x, y, w, h,
        NULL, NULL, wc.hInstance, NULL
    );

    SetLayeredWindowAttributes(g_hwndMain, 0, 245, LWA_ALPHA); // 245 alpha for slight transparency
    
    // Attempt Windows 11 DWM rounded corners
    DWORD preference = DWMWCP_ROUND;
    DwmSetWindowAttribute(g_hwndMain, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnregisterClassW(L"WindhawkSpotlightClass", wc.hInstance);
    return 0;
}

BOOL Wh_ModInit() {
    // Explorer runs multiple instances; ensure only ONE UI thread gets generated globally
    g_hMutex = CreateMutexW(NULL, TRUE, L"Windhawk_Spotlight_Mutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(g_hMutex);
        g_hMutex = NULL;
        return TRUE; 
    }

    g_modifier = Wh_GetIntSetting(L"modifier");
    g_keycode = Wh_GetIntSetting(L"keycode");
    g_defaultSearch = Wh_GetIntSetting(L"defaultSearch");

    g_hThread = CreateThread(NULL, 0, UIThread, NULL, 0, &g_dwThreadId);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hMutex) {
        if (g_hwndMain) {
            PostMessage(g_hwndMain, WM_CLOSE, 0, 0);
        }
        if (g_hThread) {
            WaitForSingleObject(g_hThread, 2000);
            CloseHandle(g_hThread);
            g_hThread = NULL;
        }
        ReleaseMutex(g_hMutex);
        CloseHandle(g_hMutex);
        g_hMutex = NULL;
    }
}

void Wh_ModSettingsChanged() {
    if (g_hMutex) {
        g_modifier = Wh_GetIntSetting(L"modifier");
        g_keycode = Wh_GetIntSetting(L"keycode");
        g_defaultSearch = Wh_GetIntSetting(L"defaultSearch");
        
        // Push the hotkey refresh to the thread safely
        if (g_hwndMain) {
            PostMessage(g_hwndMain, WM_UPDATE_HOTKEY, 0, 0);
        }
    }
}
