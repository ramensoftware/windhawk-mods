// ==WindhawkMod==
// @id              sndvol-minimize
// @name            Volume Mixer Minimize Button
// @description     Adds a Minimize button to the Volume Mixer
// @version         1
// @author          Jevil7452
// @github          https://github.com/Jevil7452
// @include         sndvol.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod adds the Minimize button to the Volume Mixer and Volume Control Options dialogs.

Before:

![1](https://i.imgur.com/MG6eeLL.png)
![2](https://i.imgur.com/2ZCGB2t.png)

After:

![3](https://i.imgur.com/1pNiTNi.png)
![4](https://i.imgur.com/cu6j0JR.png)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <tlhelp32.h>
#include <vector>

std::vector<HHOOK> g_hooks;

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) 
        return CallNextHookEx(NULL, nCode, wParam, lParam);

    CWPSTRUCT* pwp = (CWPSTRUCT*)lParam;
    if (pwp->message == WM_CREATE) {
        LONG_PTR style = GetWindowLongPtrW(pwp->hwnd, GWL_STYLE);
        if (!(style & WS_CHILD)) {
            SetWindowLongPtrW(pwp->hwnd, GWL_STYLE, style | WS_MINIMIZEBOX);
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }

    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (!(style & WS_CHILD)) {
        SetWindowLongPtrW(hWnd, GWL_STYLE, style | WS_MINIMIZEBOX);
    }

    return TRUE;
}

void HookAllProcessThreads() {
    DWORD pid = GetCurrentProcessId();
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        return;
    }

    THREADENTRY32 te{};
    te.dwSize = sizeof(te);
    if (Thread32First(hSnap, &te)) {
        do {
            if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
                                  sizeof(te.th32OwnerProcessID) &&
                te.th32OwnerProcessID == pid) {
                HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, CallWndProc,
                                                NULL, te.th32ThreadID);
                if (hook) {
                    g_hooks.push_back(hook);
                }
            }
            te.dwSize = sizeof(te);
        } while (Thread32Next(hSnap, &te));
    }

    CloseHandle(hSnap);
}

BOOL Wh_ModInit(void) {
    HookAllProcessThreads();
    return TRUE;
}

void Wh_ModAfterInit(void) {
    EnumWindows(EnumWindowsProc, 0);
}

void Wh_ModUninit(void) {
    for (HHOOK hook : g_hooks) {
        UnhookWindowsHookEx(hook);
    }
    g_hooks.clear();
}
