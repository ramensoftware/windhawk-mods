// ==WindhawkMod==
// @id              notepad-multi-select-full
// @name            Notepad Multi-Select (Final Engine)
// @description     Adds Alt+Shift multi-caret typing, backspacing, and rendering.
// @version         3.1
// @author          Mohamed Magdy
// @github          hamomagdy724
// @include         notepad.exe
// @compilerOptions -luser32 -lcomctl32 -lgdi32
// ==/WindhawkMod==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdbool.h>

#define MAX_CARETS 50

// --- 1. STATE MEMORY ---
typedef struct {
    int caretPositions[MAX_CARETS];
    int caretCount;
} MultiCursorState;

MultiCursorState g_state = {{0}, 0};

// Helper function to sort carets from Top to Bottom
void SortCaretsAscending() {
    for (int i = 0; i < g_state.caretCount - 1; i++) {
        for (int j = 0; j < g_state.caretCount - i - 1; j++) {
            if (g_state.caretPositions[j] > g_state.caretPositions[j+1]) {
                int temp = g_state.caretPositions[j];
                g_state.caretPositions[j] = g_state.caretPositions[j+1];
                g_state.caretPositions[j+1] = temp;
            }
        }
    }
}

// --- THREAD-SAFE SUBCLASSING (m417z Framework) ---
UINT g_subclassRegisteredMsg;

struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
    SUBCLASSPROC pfnSubclass;
    UINT_PTR uIdSubclass;
    DWORD_PTR dwRefData;
    BOOL result;
};

LRESULT CALLBACK CallWndProcForWindowSubclass(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
            SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param = (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
            param->result = SetWindowSubclass(cwp->hwnd, param->pfnSubclass, param->uIdSubclass, param->dwRefData);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL SetWindowSubclassFromAnyThread(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return FALSE;
    if (dwThreadId == GetCurrentThreadId()) return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcForWindowSubclass, nullptr, dwThreadId);
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

// --- 2. THE MULTI-SELECT ENGINE ---
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    
    // -- 1. CANCEL MULTI-SELECT ON CLICK OR ESCAPE --
    if (uMsg == WM_LBUTTONDOWN || (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)) {
        if (g_state.caretCount > 0) {
            g_state.caretCount = 0;
            InvalidateRect(hWnd, NULL, TRUE);
        }
    }

    // -- 2. INTERCEPT TYPING (LETTERS, NUMBERS, ENTER) --
    if (uMsg == WM_CHAR && g_state.caretCount > 1) {
        // Ignore backspace here (ASCII 8), we handle it in WM_KEYDOWN
        if (wParam == 8) return 0; 
        // Ignore other unprintable control characters except Enter (ASCII 13)
        if (wParam < 32 && wParam != VK_RETURN) return 0;

        SortCaretsAscending();
        
        WCHAR str[3] = {0};
        int charsAdded = 1;
        
        // Notepad requires \r\n for a new line, not just \r
        if (wParam == VK_RETURN) {
            str[0] = L'\r'; str[1] = L'\n'; str[2] = L'\0';
            charsAdded = 2;
        } else {
            str[0] = (WCHAR)wParam; str[1] = L'\0';
        }

        int offset = 0; // Tracks the shifting text
        
        for (int i = 0; i < g_state.caretCount; i++) {
            int pos = g_state.caretPositions[i] + offset;
            
            // Move real cursor to this position and inject the text
            SendMessage(hWnd, EM_SETSEL, pos, pos);
            SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)str);
            
            // Update the memory array for the next keystroke
            g_state.caretPositions[i] = pos + charsAdded;
            offset += charsAdded; // Push subsequent cursors forward
        }
        
        InvalidateRect(hWnd, NULL, FALSE);
        return 0; // Kill default typing behavior
    }

    // -- 3. INTERCEPT BACKSPACE --
    if (uMsg == WM_KEYDOWN && wParam == VK_BACK && g_state.caretCount > 1) {
        SortCaretsAscending();
        int offset = 0;

        for (int i = 0; i < g_state.caretCount; i++) {
            int pos = g_state.caretPositions[i] + offset;
            
            if (pos > 0) { 
                // Highlight the 1 character to the left of the cursor, and replace it with nothing
                SendMessage(hWnd, EM_SETSEL, pos - 1, pos);
                SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)L"");
                
                g_state.caretPositions[i] = pos - 1;
                offset -= 1; // Pull subsequent cursors backward
            }
        }
        InvalidateRect(hWnd, NULL, FALSE);
        return 0; // Kill default backspace behavior
    }

    // -- 4. INTERCEPT ALT+SHIFT+UP/DOWN TO ADD CURSORS --
    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
        bool isAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
        bool isShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

        // Make sure we only trigger when it's exactly Alt+Shift+Up/Down
        if (isAlt && isShift && (wParam == VK_UP || wParam == VK_DOWN)) {
            
            if (g_state.caretCount == 0) {
                DWORD startPos, endPos;
                SendMessage(hWnd, EM_GETSEL, (WPARAM)&startPos, (LPARAM)&endPos);
                g_state.caretPositions[0] = startPos;
                g_state.caretCount = 1;
            }

            int lastCaretPos = g_state.caretPositions[g_state.caretCount - 1];
            int currentLine = SendMessage(hWnd, EM_LINEFROMCHAR, lastCaretPos, 0);
            int lineStartChar = SendMessage(hWnd, EM_LINEINDEX, currentLine, 0);
            int column = lastCaretPos - lineStartChar; 

            int targetLine = (wParam == VK_UP) ? currentLine - 1 : currentLine + 1;
            int totalLines = SendMessage(hWnd, EM_GETLINECOUNT, 0, 0);

            if (targetLine >= 0 && targetLine < totalLines && g_state.caretCount < MAX_CARETS) {
                int targetLineStart = SendMessage(hWnd, EM_LINEINDEX, targetLine, 0);
                int targetLineLen = SendMessage(hWnd, EM_LINELENGTH, targetLineStart, 0);
                
                int newCaretPos = targetLineStart + (column < targetLineLen ? column : targetLineLen);
                
                g_state.caretPositions[g_state.caretCount] = newCaretPos;
                g_state.caretCount++;
                
                InvalidateRect(hWnd, NULL, FALSE); 
            }
            return 0; // Kill default movement
        }
    }

    // -- 5. DRAW THE PHANTOM CURSORS ON SCREEN --
    if (uMsg == WM_PAINT) {
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

        if (g_state.caretCount > 1) { // Only draw if multi-select is active
            HDC hdc = GetDC(hWnd);
            TEXTMETRIC tm;
            GetTextMetrics(hdc, &tm);
            int fontHeight = tm.tmHeight;

            HBRUSH caretBrush = CreateSolidBrush(RGB(0, 120, 215)); // Standard Windows Blue

            for (int i = 0; i < g_state.caretCount; i++) {
                int pos = g_state.caretPositions[i];
                LRESULT coords = SendMessage(hWnd, EM_POSFROMCHAR, pos, 0);
                if (coords != -1) {
                    int x = GET_X_LPARAM(coords);
                    int y = GET_Y_LPARAM(coords);
                    RECT rect = { x, y, x + 2, y + fontHeight };
                    FillRect(hdc, &rect, caretBrush);
                }
            }
            DeleteObject(caretBrush);
            ReleaseDC(hWnd, hdc);
        }
        return result;
    }

    if (uMsg == g_subclassRegisteredMsg && !wParam) {
        RemoveWindowSubclass(hWnd, EditSubclassProc, 0);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// --- WINDOW INJECTION ---
HWND g_hNotepadWnd = NULL;
HWND g_hEditWnd = NULL;

BOOL CALLBACK FindCurrentProcessNotepadWindowEnumFunc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId()) return TRUE;
    WCHAR szClassName[16];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 || wcsicmp(szClassName, L"Notepad") != 0) return TRUE;
    *(HWND*)lParam = hWnd;
    return FALSE;
}

// THIS IS THE FUNCTION I FORGOT TO PASTE LAST TIME!
HWND FindCurrentProcessNotepadWindow() {
    HWND hNotepadWnd = NULL;
    EnumWindows(FindCurrentProcessNotepadWindowEnumFunc, (LPARAM)&hNotepadWnd);
    return hNotepadWnd;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pOriginalCreateWindowExW;

HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = pOriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (bTextualClassName && wcsicmp(lpClassName, L"Notepad") == 0) {
        g_hNotepadWnd = hWnd;
    }
    else if (g_hNotepadWnd && hWndParent == g_hNotepadWnd) {
        if (bTextualClassName && wcsicmp(lpClassName, L"Edit") == 0) {
            SetWindowSubclassFromAnyThread(hWnd, EditSubclassProc, 0, 0);
        }
    }
    return hWnd;
}

// --- INIT ---
BOOL Wh_ModInit(void) {
    g_subclassRegisteredMsg = RegisterWindowMessage(L"Windhawk_SetWindowSubclassFromAnyThread_MultiSelect");
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook, (void**)&pOriginalCreateWindowExW);
    
    g_hNotepadWnd = FindCurrentProcessNotepadWindow();
    if (g_hNotepadWnd) {
        g_hEditWnd = FindWindowEx(g_hNotepadWnd, NULL, L"Edit", NULL);
        if (g_hEditWnd) {
            SetWindowSubclassFromAnyThread(g_hEditWnd, EditSubclassProc, 0, 0);
        }
    }
    return TRUE;
}

void Wh_ModUninit(void) {
    if (g_hEditWnd) SendMessage(g_hEditWnd, g_subclassRegisteredMsg, FALSE, 0);
}