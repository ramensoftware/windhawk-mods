// ==WindhawkMod==
// @id              classic-language-indicator-flyout
// @name            Non-immersive (classic) language indicator flyout for Win10 taskbar
// @description     Replaces the immersive language indicator flyout of Windows 10 taskbar with a classic popup menu
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Language Indicator Menu

This mod replaces the immersive flyout of Windows 10 taskbar that appears when clicking the keyboard
layout indicator in the taskbar with a classic popup menu.

![Screenshot](https://i.imgur.com/U1WSpQG.png)

*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <vector>
#include <string>
#include <map>

#define IDM_LAYOUT_BASE 1000

std::map<HWND, WNDPROC> g_originalWndProcs;
bool g_subclassingNow = false;

std::vector<HKL> GetLayouts() {
    std::vector<HKL> layouts;
    int count = GetKeyboardLayoutList(0, nullptr);
    if (count > 0) {
        layouts.resize(count);
        GetKeyboardLayoutList(count, layouts.data());
    }
    return layouts;
}

HKL GetCurrentLayout() {
    HWND hwndForeground = GetForegroundWindow();
    if (!hwndForeground) {
        return GetKeyboardLayout(0);
    }
    DWORD threadId = GetWindowThreadProcessId(hwndForeground, nullptr);
    return GetKeyboardLayout(threadId);
}

std::wstring GetLayoutName(HKL hkl) {
    wchar_t displayName[256] = {0};
    LANGID langId = LOWORD((DWORD_PTR)hkl);
    LCID lcid = MAKELCID(langId, SORT_DEFAULT);
    
    if (GetLocaleInfoW(lcid, LOCALE_SLANGUAGE, displayName, 256)) {
        return std::wstring(displayName);
    }
    
    wchar_t buffer[32];
    swprintf_s(buffer, L"Layout 0x%08X", (UINT)(UINT_PTR)hkl);
    return std::wstring(buffer);
}

void SwitchToLayout(HKL hkl) {
    HWND hwndForeground = GetForegroundWindow();
    if (hwndForeground) {
        PostMessage(hwndForeground, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)hkl);
    }
    ActivateKeyboardLayout(hkl, KLF_SETFORPROCESS);
}

void ShowClassicMenu(HWND hwnd) {
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;
    
    auto layouts = GetLayouts();
    HKL currentHkl = GetCurrentLayout();
    
    for (size_t i = 0; i < layouts.size(); i++) {
        UINT flags = MF_STRING;
        if (layouts[i] == currentHkl) {
            flags |= MF_CHECKED;
        }
        std::wstring name = GetLayoutName(layouts[i]);
        AppendMenuW(hMenu, flags, IDM_LAYOUT_BASE + i, name.c_str());
    }
    
    POINT pt;
    GetCursorPos(&pt);
    
    HWND hwndTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hwndTray) hwndTray = hwnd;
    
    SetForegroundWindow(hwndTray);
    
    UINT cmd = TrackPopupMenu(
        hMenu,
        TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RETURNCMD,
        pt.x, pt.y,
        0,
        hwndTray,
        nullptr
    );
    
    if (cmd >= IDM_LAYOUT_BASE && cmd < IDM_LAYOUT_BASE + layouts.size()) {
        size_t index = cmd - IDM_LAYOUT_BASE;
        SwitchToLayout(layouts[index]);
    }
    
    DestroyMenu(hMenu);
}

LRESULT CALLBACK InputIndicatorSubclassProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
) {
    auto it = g_originalWndProcs.find(hwnd);
    if (it == g_originalWndProcs.end()) {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    WNDPROC originalProc = it->second;
    
    switch (uMsg) {
        case WM_LBUTTONDOWN:
            return 0;
            
        case WM_LBUTTONUP:
            ShowClassicMenu(hwnd);
            return 0;
            
        case WM_NCDESTROY:
            g_originalWndProcs.erase(hwnd);
            return CallWindowProc(originalProc, hwnd, uMsg, wParam, lParam);
    }
    
    return CallWindowProc(originalProc, hwnd, uMsg, wParam, lParam);
}

void SubclassWindowNow(HWND hwnd);

using SetWindowLongPtrW_t = LONG_PTR(WINAPI*)(HWND, int, LONG_PTR);
SetWindowLongPtrW_t SetWindowLongPtrW_Original;

LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    if (nIndex == GWLP_WNDPROC && g_originalWndProcs.count(hWnd) && !g_subclassingNow) {
        g_originalWndProcs[hWnd] = (WNDPROC)dwNewLong;
        return (LONG_PTR)InputIndicatorSubclassProc;
    }
    
    LONG_PTR result = SetWindowLongPtrW_Original(hWnd, nIndex, dwNewLong);
    
    if (nIndex == GWLP_WNDPROC && !g_subclassingNow && hWnd) {
        wchar_t className[256] = {0};
        if (GetClassNameW(hWnd, className, 256)) {
            if (wcscmp(className, L"InputIndicatorButton") == 0 && 
                !g_originalWndProcs.count(hWnd)) {
                SubclassWindowNow(hWnd);
            }
        }
    }
    
    return result;
}

void SubclassWindowNow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    if (g_originalWndProcs.count(hwnd)) return;
    
    g_subclassingNow = true;
    WNDPROC oldProc = (WNDPROC)SetWindowLongPtrW_Original(
        hwnd,
        GWLP_WNDPROC,
        (LONG_PTR)InputIndicatorSubclassProc
    );
    g_subclassingNow = false;
    
    if (oldProc && oldProc != InputIndicatorSubclassProc) {
        g_originalWndProcs[hwnd] = oldProc;
    }
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
    wchar_t className[256] = {0};
    GetClassNameW(hwnd, className, 256);
    
    if (wcscmp(className, L"InputIndicatorButton") == 0) {
        SubclassWindowNow(hwnd);
    }
    
    EnumChildWindows(hwnd, EnumChildProc, 0);
    return TRUE;
}

void FindAndSubclassIndicator() {
    HWND hShellTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hShellTray) {
        EnumChildWindows(hShellTray, EnumChildProc, 0);
    }
    
    HWND hSecondary = nullptr;
    while ((hSecondary = FindWindowExW(nullptr, hSecondary, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
        EnumChildWindows(hSecondary, EnumChildProc, 0);
    }
}

void RemoveSubclassing() {
    g_subclassingNow = true;
    for (auto& pair : g_originalWndProcs) {
        if (IsWindow(pair.first)) {
            SetWindowLongPtrW_Original(pair.first, GWLP_WNDPROC, (LONG_PTR)pair.second);
        }
    }
    g_subclassingNow = false;
    g_originalWndProcs.clear();
}

BOOL Wh_ModInit() {
    
    Wh_SetFunctionHook(
        (void*)SetWindowLongPtrW,
        (void*)SetWindowLongPtrW_Hook,
        (void**)&SetWindowLongPtrW_Original
    );
    
    FindAndSubclassIndicator();
    
    return TRUE;
}

void Wh_ModUninit() {
    RemoveSubclassing();
}
