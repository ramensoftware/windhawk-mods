// ==WindhawkMod==
// @id              linux-style-window-drag
// @name            Linux-style Window Drag
// @description     Enables moving windows by clicking anywhere while holding the Win key, similar to GNOME or KDE. Includes a fix for the Start Menu.
// @version         1.0
// @author          djestick
// @github          https://github.com/djestick/linux-style-window-drag-on-windows
// @include         explorer.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

#include <windows.h>

// Глобальные переменные
HHOOK g_hMouseHook = NULL;
HHOOK g_hKbdHook = NULL;
HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;

// Состояние драга
bool g_isDragging = false;
HWND g_hDragWindow = NULL;
POINT g_ptOffset = {0, 0};

// Таймер для кнопки Win (чтобы не открывался Пуск)
ULONGLONG g_winKeyDownTime = 0;
bool g_winKeyUsedForDrag = false;

// Убивалка меню Пуск
void KillStartMenu() {
    keybd_event(0xFF, 0, 0, 0);
    keybd_event(0xFF, 0, KEYEVENTF_KEYUP, 0);
}

// --- ХУК КЛАВИАТУРЫ (Фикс меню Пуск) ---
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKbd = (KBDLLHOOKSTRUCT*)lParam;
        if (pKbd->vkCode == VK_LWIN || pKbd->vkCode == VK_RWIN) {
            if (wParam == WM_KEYDOWN) {
                if (g_winKeyDownTime == 0) {
                    g_winKeyDownTime = GetTickCount64();
                    g_winKeyUsedForDrag = false;
                }
            } else if (wParam == WM_KEYUP) {
                ULONGLONG holdTime = GetTickCount64() - g_winKeyDownTime;
                g_winKeyDownTime = 0;
                // Блокируем Пуск, если тащили окно или держали кнопку долго (>300мс)
                if (g_winKeyUsedForDrag || holdTime > 300) {
                    KillStartMenu();
                }
            }
        }
    }
    return CallNextHookEx(g_hKbdHook, nCode, wParam, lParam);
}

// --- ХУК МЫШИ (Логика драга) ---
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;

        // 1. НАЧАЛО: WIN зажат + ЛКМ нажата
        if (wParam == WM_LBUTTONDOWN) {
            bool isWinDown = (GetAsyncKeyState(VK_LWIN) & 0x8000) || 
                             (GetAsyncKeyState(VK_RWIN) & 0x8000);

            if (isWinDown) {
                HWND hWnd = WindowFromPoint(pMouse->pt);
                if (hWnd) {
                    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
                    if (hRoot && hRoot != GetDesktopWindow()) {
                        RECT rcWindow;
                        if (GetWindowRect(hRoot, &rcWindow)) {
                            g_isDragging = true;
                            g_hDragWindow = hRoot;
                            // Считаем точку захвата
                            g_ptOffset.x = pMouse->pt.x - rcWindow.left;
                            g_ptOffset.y = pMouse->pt.y - rcWindow.top;
                            
                            g_winKeyUsedForDrag = true;
                            // БЛОКИРУЕМ клик (return 1), чтобы не нажалось в приложении
                            return 1; 
                        }
                    }
                }
            }
        }

        // 2. ДВИЖЕНИЕ
        if (wParam == WM_MOUSEMOVE && g_isDragging) {
            bool isWinDown = (GetAsyncKeyState(VK_LWIN) & 0x8000) || 
                             (GetAsyncKeyState(VK_RWIN) & 0x8000);
            
            // Если Win отпустили — стоп машина
            if (!isWinDown) {
                g_isDragging = false;
                g_hDragWindow = NULL;
            } else {
                // Двигаем окно
                int newX = pMouse->pt.x - g_ptOffset.x;
                int newY = pMouse->pt.y - g_ptOffset.y;
                
                SetWindowPos(g_hDragWindow, NULL, newX, newY, 0, 0, 
                             SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
                
                // ВАЖНО: Мы НЕ блокируем движение (CallNextHookEx), 
                // чтобы Windows отрисовала перемещение курсора!
            }
        }

        // 3. КОНЕЦ: ЛКМ отпущена
        if (wParam == WM_LBUTTONUP && g_isDragging) {
            g_isDragging = false;
            g_hDragWindow = NULL;
            // БЛОКИРУЕМ отпускание (return 1), чтобы приложение не словило клик
            return 1; 
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID lpParam) {
    g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
    g_hKbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_QUIT) break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (g_hMouseHook) UnhookWindowsHookEx(g_hMouseHook);
    if (g_hKbdHook) UnhookWindowsHookEx(g_hKbdHook);
    return 0;
}

BOOL Wh_ModInit() {
    g_hThread = CreateThread(NULL, 0, HookThread, NULL, 0, &g_dwThreadId);
    return (g_hThread != NULL);
}

void Wh_ModUninit() {
    if (g_dwThreadId) {
        PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, 2000);
        CloseHandle(g_hThread);
    }
}
