// ==WindhawkMod==
// @id              notepad-multi-select
// @name            Notepad Multi-Select (Final Engine)
// @description     Adds Alt+Shift multi-caret, true blue highlighting, smart indent, and perfect \r\n handling.
// @version         4.9
// @author          Mohamed Magdy
// @github          https://github.com/hamomagdy724
// @include         notepad.exe
// @compilerOptions -luser32 -lcomctl32 -lgdi32
// ==/WindhawkMod==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <wctype.h> 

#define MAX_CARETS 50
#define BLINK_TIMER_ID 1001

inline int GetMin(int a, int b) { return (a < b) ? a : b; }
inline int GetMax(int a, int b) { return (a > b) ? a : b; }

// --- 1. STATE MEMORY ---
typedef struct {
    int positions[MAX_CARETS];
    int anchors[MAX_CARETS];
    int caretCount;
} MultiCursorState;

MultiCursorState g_state = {{0}, {0}, 0};

bool g_caretsVisible = true;
UINT_PTR g_blinkTimer = 0;
bool g_isNativeCaretHidden = false; 

void SortCaretsAscending() {
    for (int i = 0; i < g_state.caretCount - 1; i++) {
        for (int j = 0; j < g_state.caretCount - i - 1; j++) {
            int p1 = GetMin(g_state.positions[j], g_state.anchors[j]);
            int p2 = GetMin(g_state.positions[j+1], g_state.anchors[j+1]);
            if (p1 > p2) {
                int tp = g_state.positions[j];
                g_state.positions[j] = g_state.positions[j+1];
                g_state.positions[j+1] = tp;
                
                int ta = g_state.anchors[j];
                g_state.anchors[j] = g_state.anchors[j+1];
                g_state.anchors[j+1] = ta;
            }
        }
    }
}

void CancelMultiSelect(HWND hWnd) {
    if (g_state.caretCount > 0) {
        g_state.caretCount = 0;
        if (g_blinkTimer) {
            KillTimer(hWnd, BLINK_TIMER_ID);
            g_blinkTimer = 0;
        }
        if (g_isNativeCaretHidden) {
            ShowCaret(hWnd); 
            g_isNativeCaretHidden = false;
        }
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void ResetBlinkTimer(HWND hWnd) {
    if (g_blinkTimer) KillTimer(hWnd, BLINK_TIMER_ID);
    g_caretsVisible = true;
    g_blinkTimer = SetTimer(hWnd, BLINK_TIMER_ID, GetCaretBlinkTime(), NULL);
}

// Custom Coordinate Fetcher
bool GetCaretX(HWND hWnd, int pos, int* pX, int* pY, TEXTMETRIC* tm) {
    LRESULT coords = SendMessage(hWnd, EM_POSFROMCHAR, pos, 0);
    if (coords != -1) {
        *pX = GET_X_LPARAM(coords);
        *pY = GET_Y_LPARAM(coords);
        return true;
    }
    int textLen = SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);
    if (pos > 0 && pos >= textLen) {
        coords = SendMessage(hWnd, EM_POSFROMCHAR, pos - 1, 0);
        if (coords != -1) {
            *pX = GET_X_LPARAM(coords) + tm->tmAveCharWidth;
            *pY = GET_Y_LPARAM(coords);
            return true;
        }
    }
    return false;
}

// Memory-Safe Logical Paragraph Finder (Bypasses Word Wrap Issues!)
int GetLogicalLineStartIdx(HWND hWnd, int p) {
    int currIdx = SendMessage(hWnd, EM_LINEFROMCHAR, p, 0);
    while (currIdx > 0) {
        int prevStart = SendMessage(hWnd, EM_LINEINDEX, currIdx - 1, 0);
        int prevLen = SendMessage(hWnd, EM_LINELENGTH, prevStart, 0);
        int currStart = SendMessage(hWnd, EM_LINEINDEX, currIdx, 0);
        
        // If the gap between the end of previous visual line and start of current is >= 2,
        // it means we hit a hard \r\n break. Otherwise, it's just soft word-wrap.
        if (currStart - (prevStart + prevLen) >= 2) break; 
        
        currIdx--;
    }
    return currIdx;
}

// --- 2. CUSTOM WORD BOUNDARY PARSERS (SPACE-DELIMITED ONLY) ---
int FindWordBreakLeft(HWND hWnd, int pos) {
    if (pos <= 0) return 0;
    int lineIdx = SendMessage(hWnd, EM_LINEFROMCHAR, pos, 0);
    int lineStart = SendMessage(hWnd, EM_LINEINDEX, lineIdx, 0);
    if (pos == lineStart) return pos - 1; 
    
    int lineLen = SendMessage(hWnd, EM_LINELENGTH, pos, 0);
    if (lineLen <= 0) return pos - 1;
    
    WCHAR* buf = (WCHAR*)malloc((lineLen + 1) * sizeof(WCHAR));
    if (!buf) return pos - 1;
    
    *(WORD*)buf = (WORD)lineLen;
    int copied = SendMessage(hWnd, EM_GETLINE, lineIdx, (LPARAM)buf);
    int localPos = pos - lineStart;
    if (localPos > copied) localPos = copied;
    
    int i = localPos - 1;
    while (i > 0 && iswspace(buf[i])) i--; 
    while (i > 0 && !iswspace(buf[i - 1])) i--; 
    
    free(buf);
    return lineStart + i;
}

int FindWordBreakRight(HWND hWnd, int pos) {
    int textLen = SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);
    if (pos >= textLen) return textLen;
    int lineIdx = SendMessage(hWnd, EM_LINEFROMCHAR, pos, 0);
    int lineStart = SendMessage(hWnd, EM_LINEINDEX, lineIdx, 0);
    int lineLen = SendMessage(hWnd, EM_LINELENGTH, pos, 0);
    
    int localPos = pos - lineStart;
    if (localPos >= lineLen) return pos + 1; 
    
    WCHAR* buf = (WCHAR*)malloc((lineLen + 1) * sizeof(WCHAR));
    if (!buf) return pos + 1;
    
    *(WORD*)buf = (WORD)lineLen;
    int copied = SendMessage(hWnd, EM_GETLINE, lineIdx, (LPARAM)buf);
    
    int i = localPos;
    while (i < copied && !iswspace(buf[i])) i++; 
    while (i < copied && iswspace(buf[i])) i++; 
    
    free(buf);
    return lineStart + i;
}

// --- 3. THREAD-SAFE SUBCLASSING ---
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

// --- 4. THE MULTI-SELECT ENGINE ---
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    
    // -- GLOBALLY KILL THE CTRL+BACKSPACE BOX (ASCII 127) --
    if (uMsg == WM_CHAR && wParam == 127) {
        return 0; 
    }

    if (uMsg == WM_LBUTTONDOWN || (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)) {
        CancelMultiSelect(hWnd);
    }

    if (uMsg == WM_KEYDOWN && (wParam == VK_UP || wParam == VK_DOWN) && g_state.caretCount > 1) {
        bool isAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
        bool isShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        if (!isAlt || !isShift) CancelMultiSelect(hWnd);
    }

    if (uMsg == WM_TIMER && wParam == BLINK_TIMER_ID) {
        if (g_state.caretCount > 1) {
            g_caretsVisible = !g_caretsVisible;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0; 
    }

    // -- INTERCEPT SHIFT+ARROW HIGHLIGHTING & CTRL WORD JUMPING --
    if (uMsg == WM_KEYDOWN && (wParam == VK_LEFT || wParam == VK_RIGHT) && g_state.caretCount > 1) {
        bool isLeft = (wParam == VK_LEFT);
        bool isShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        bool isCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

        for (int i = 0; i < g_state.caretCount; i++) {
            int p = g_state.positions[i];
            int a = g_state.anchors[i];

            if (!isShift && p != a) {
                p = isLeft ? GetMin(a, p) : GetMax(a, p);
                g_state.positions[i] = p;
                g_state.anchors[i] = p;
                continue;
            }

            if (isCtrl) {
                p = isLeft ? FindWordBreakLeft(hWnd, p) : FindWordBreakRight(hWnd, p);
            } else {
                int textLen = SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);
                if (isLeft && p > 0) p--;
                else if (!isLeft && p < textLen) p++;
            }

            g_state.positions[i] = p;
            if (!isShift) g_state.anchors[i] = p;
        }
        
        int primaryPos = g_state.positions[g_state.caretCount - 1];
        int primaryAnchor = g_state.anchors[g_state.caretCount - 1];
        SendMessage(hWnd, EM_SETSEL, primaryAnchor, primaryPos);
        ResetBlinkTimer(hWnd);
        InvalidateRect(hWnd, NULL, FALSE);
        return 0; 
    }

    // -- TAB AND SHIFT+TAB (GLOBAL: SINGLE AND MULTI) --
    if (uMsg == WM_KEYDOWN && wParam == VK_TAB) {
        bool isShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        bool isSingleCaret = (g_state.caretCount <= 1);

        if (isSingleCaret) {
            DWORD start, end;
            SendMessage(hWnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
            g_state.anchors[0] = start;
            g_state.positions[0] = end;
            g_state.caretCount = 1; 
        } else {
            SortCaretsAscending();
        }

        int offset = 0;
        for (int i = 0; i < g_state.caretCount; i++) {
            int p = g_state.positions[i] + offset;
            int a = g_state.anchors[i] + offset;
            int s = GetMin(a, p);
            int e = GetMax(a, p);

            if (s != e) { 
                SendMessage(hWnd, EM_SETSEL, s, e);
                SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)L"");
                p = s;
                offset -= (e - s);
            }

            if (isShift) {
                // MOVE BACKWARD safely without deleting
                int logIdx = GetLogicalLineStartIdx(hWnd, p);
                int logLineStart = SendMessage(hWnd, EM_LINEINDEX, logIdx, 0);
                
                int diff = p - logLineStart;
                int toMove = (diff >= 4) ? 4 : (diff > 0 ? 1 : 0); 

                if (toMove > 0) {
                    g_state.positions[i] = p - toMove;
                    g_state.anchors[i] = p - toMove;
                } else {
                    g_state.positions[i] = p;
                    g_state.anchors[i] = p;
                }
            } else {
                // Native rigid Tab injection! (Deleted the Space hack!)
                SendMessage(hWnd, EM_SETSEL, p, p);
                SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)L"\t");
                g_state.positions[i] = p + 1;
                g_state.anchors[i] = p + 1;
                offset += 1;
            }
        }

        int primaryPos = g_state.positions[g_state.caretCount - 1];
        SendMessage(hWnd, EM_SETSEL, primaryPos, primaryPos); 

        if (isSingleCaret) {
            g_state.caretCount = 0; 
        } else {
            ResetBlinkTimer(hWnd);
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0; 
    }
    
    if (uMsg == WM_CHAR && wParam == VK_TAB) return 0;

    // -- PYTHON-STYLE AUTO-INDENT (VK_RETURN) GLOBAL: SINGLE AND MULTI --
    if (uMsg == WM_CHAR && wParam == VK_RETURN) {
        bool isSingleCaret = (g_state.caretCount <= 1);

        if (isSingleCaret) {
            DWORD start, end;
            SendMessage(hWnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
            g_state.anchors[0] = start;
            g_state.positions[0] = end;
            g_state.caretCount = 1; 
        } else {
            SortCaretsAscending();
        }

        int offset = 0;
        for (int i = 0; i < g_state.caretCount; i++) {
            int p = g_state.positions[i] + offset;
            int a = g_state.anchors[i] + offset;
            int s = GetMin(a, p);
            int e = GetMax(a, p);
            
            if (s != e) {
                SendMessage(hWnd, EM_SETSEL, s, e);
                SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)L"");
                p = s;
                offset -= (e - s);
            }

            // O(1) Memory Safe Paragraph Indent Tracer!
            int logIdx = GetLogicalLineStartIdx(hWnd, p);
            int logStart = SendMessage(hWnd, EM_LINEINDEX, logIdx, 0);
            int logLen = SendMessage(hWnd, EM_LINELENGTH, logStart, 0);

            int wsCount = 0;
            WCHAR* inject = NULL;

            if (logLen > 0) {
                WCHAR* buf = (WCHAR*)malloc((logLen + 1) * sizeof(WCHAR));
                *(WORD*)buf = (WORD)logLen;
                int copied = SendMessage(hWnd, EM_GETLINE, logIdx, (LPARAM)buf);
                int limit = GetMin(copied, p - logStart); 
                
                while (wsCount < limit && (buf[wsCount] == L' ' || buf[wsCount] == L'\t')) {
                    wsCount++;
                }

                inject = (WCHAR*)malloc((wsCount + 3) * sizeof(WCHAR));
                inject[0] = L'\r'; 
                inject[1] = L'\n';
                for (int k = 0; k < wsCount; k++) {
                    inject[2 + k] = buf[k];
                }
                inject[2 + wsCount] = L'\0';
                free(buf);
            } else {
                inject = (WCHAR*)malloc(3 * sizeof(WCHAR));
                inject[0] = L'\r'; 
                inject[1] = L'\n';
                inject[2] = L'\0';
            }

            SendMessage(hWnd, EM_SETSEL, p, p);
            SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)inject);
            
            int charsAdded = 2 + wsCount;
            g_state.positions[i] = p + charsAdded;
            g_state.anchors[i] = p + charsAdded; 
            offset += charsAdded; 
            
            free(inject);
        }
        
        int primaryPos = g_state.positions[g_state.caretCount - 1];
        SendMessage(hWnd, EM_SETSEL, primaryPos, primaryPos);
        
        if (isSingleCaret) {
            g_state.caretCount = 0; 
        } else {
            ResetBlinkTimer(hWnd);
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0; 
    }

    // -- TYPING OVER HIGHLIGHTED SELECTIONS --
    if (uMsg == WM_CHAR && g_state.caretCount > 1) {
        if (wParam == 8 || wParam == VK_RETURN) return 0; 
        if (wParam < 32 && wParam != VK_RETURN) return 0;

        SortCaretsAscending();
        WCHAR str[3] = {0};
        int charsAdded = 1;
        
        str[0] = (WCHAR)wParam; str[1] = L'\0';

        int offset = 0; 
        for (int i = 0; i < g_state.caretCount; i++) {
            int p = g_state.positions[i] + offset;
            int a = g_state.anchors[i] + offset;
            int s = GetMin(a, p);
            int e = GetMax(a, p);
            
            SendMessage(hWnd, EM_SETSEL, s, e);
            SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)str);
            
            g_state.positions[i] = s + charsAdded;
            g_state.anchors[i] = g_state.positions[i]; 
            offset += charsAdded - (e - s); 
        }
        
        int primaryPos = g_state.positions[g_state.caretCount - 1];
        SendMessage(hWnd, EM_SETSEL, primaryPos, primaryPos);
        ResetBlinkTimer(hWnd);
        InvalidateRect(hWnd, NULL, FALSE);
        return 0; 
    }

    // -- RIGID BACKSPACE (GLOBAL: SINGLE AND MULTI) --
    if (uMsg == WM_KEYDOWN && wParam == VK_BACK) {
        bool isCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        bool isSingleCaret = (g_state.caretCount <= 1);

        if (isSingleCaret) {
            DWORD start, end;
            SendMessage(hWnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
            g_state.anchors[0] = start;
            g_state.positions[0] = end;
            g_state.caretCount = 1; 
        } else {
            SortCaretsAscending();
        }

        int offset = 0;
        for (int i = 0; i < g_state.caretCount; i++) {
            int p = g_state.positions[i] + offset;
            int a = g_state.anchors[i] + offset;
            int s = GetMin(a, p);
            int e = GetMax(a, p);

            if (s != e) { 
                SendMessage(hWnd, EM_SETSEL, s, e);
                SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)L"");
                g_state.positions[i] = s;
                g_state.anchors[i] = s;
                offset -= (e - s);
            } else if (p > 0) { 
                int deleteCount = 1;
                
                if (isCtrl) {
                    int prevWordPos = FindWordBreakLeft(hWnd, p);
                    deleteCount = p - prevWordPos;
                    if (deleteCount < 0) deleteCount = 0;
                } else {
                    // Safe Double-Backspace Fix (O(1) memory lookup!)
                    int lineIdx = SendMessage(hWnd, EM_LINEFROMCHAR, p, 0);
                    int lineStart = SendMessage(hWnd, EM_LINEINDEX, lineIdx, 0);
                    if (p == lineStart && lineIdx > 0) {
                        int prevStart = SendMessage(hWnd, EM_LINEINDEX, lineIdx - 1, 0);
                        int prevLen = SendMessage(hWnd, EM_LINELENGTH, prevStart, 0);
                        if (p - (prevStart + prevLen) >= 2) {
                            deleteCount = 2; // Instantly kills \r\n
                        }
                    }
                }

                if (deleteCount > 0) {
                    SendMessage(hWnd, EM_SETSEL, p - deleteCount, p);
                    SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)L"");
                    g_state.positions[i] = p - deleteCount;
                    g_state.anchors[i] = g_state.positions[i];
                    offset -= deleteCount; 
                }
            }
        }
        
        int primaryPos = g_state.positions[g_state.caretCount - 1];
        SendMessage(hWnd, EM_SETSEL, primaryPos, primaryPos);
        
        if (isSingleCaret) {
            g_state.caretCount = 0; 
        } else {
            ResetBlinkTimer(hWnd);
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0; 
    }

    // -- DELETE --
    if (uMsg == WM_KEYDOWN && wParam == VK_DELETE && g_state.caretCount > 1) {
        bool isCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        SortCaretsAscending();
        int offset = 0;

        for (int i = 0; i < g_state.caretCount; i++) {
            int p = g_state.positions[i] + offset;
            int a = g_state.anchors[i] + offset;
            int s = GetMin(a, p);
            int e = GetMax(a, p);

            if (s != e) { 
                SendMessage(hWnd, EM_SETSEL, s, e);
                SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)L"");
                g_state.positions[i] = s;
                g_state.anchors[i] = s;
                offset -= (e - s);
            } else {
                int textLen = SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);
                if (p < textLen) { 
                    int deleteCount = 1;
                    if (isCtrl) {
                        int nextWordPos = FindWordBreakRight(hWnd, p);
                        deleteCount = nextWordPos - p;
                        if (deleteCount < 0) deleteCount = 0;
                    }
                    if (deleteCount > 0) {
                        SendMessage(hWnd, EM_SETSEL, p, p + deleteCount);
                        SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)L"");
                        g_state.positions[i] = p; 
                        g_state.anchors[i] = p;
                        offset -= deleteCount; 
                    }
                }
            }
        }
        int primaryPos = g_state.positions[g_state.caretCount - 1];
        SendMessage(hWnd, EM_SETSEL, primaryPos, primaryPos);
        ResetBlinkTimer(hWnd);
        InvalidateRect(hWnd, NULL, FALSE);
        return 0; 
    }

    // -- MULTI-SELECT SPAWNING --
    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
        bool isAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
        bool isShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

        if (isAlt && isShift && (wParam == VK_UP || wParam == VK_DOWN)) {
            if (g_state.caretCount == 0) {
                DWORD startPos, endPos;
                SendMessage(hWnd, EM_GETSEL, (WPARAM)&startPos, (LPARAM)&endPos);
                g_state.anchors[0] = startPos;
                g_state.positions[0] = endPos;
                g_state.caretCount = 1;
            }

            int lastCaretPos = g_state.positions[g_state.caretCount - 1];
            int currentLine = SendMessage(hWnd, EM_LINEFROMCHAR, lastCaretPos, 0);
            int lineStartChar = SendMessage(hWnd, EM_LINEINDEX, currentLine, 0);
            int column = lastCaretPos - lineStartChar; 

            int targetLine = (wParam == VK_UP) ? currentLine - 1 : currentLine + 1;
            int totalLines = SendMessage(hWnd, EM_GETLINECOUNT, 0, 0);

            if (targetLine >= 0 && targetLine < totalLines && g_state.caretCount < MAX_CARETS) {
                int targetLineStart = SendMessage(hWnd, EM_LINEINDEX, targetLine, 0);
                int targetLineLen = SendMessage(hWnd, EM_LINELENGTH, targetLineStart, 0);
                int newCaretPos = targetLineStart + (column < targetLineLen ? column : targetLineLen);
                
                g_state.positions[g_state.caretCount] = newCaretPos;
                g_state.anchors[g_state.caretCount] = newCaretPos;
                g_state.caretCount++;
                
                SendMessage(hWnd, EM_SETSEL, newCaretPos, newCaretPos);
                ResetBlinkTimer(hWnd);
                InvalidateRect(hWnd, NULL, FALSE); 
            }
            return 0; 
        }
    }

    // -- TRUE SELECTION AND CARET RENDERING --
    if (uMsg == WM_PAINT) {
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

        if (g_state.caretCount > 1) { 
            if (!g_isNativeCaretHidden) {
                HideCaret(hWnd); 
                g_isNativeCaretHidden = true;
            }

            HDC hdc = GetDC(hWnd);
            HFONT hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
            if (!hFont) hFont = (HFONT)GetStockObject(SYSTEM_FONT);
            
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
            
            TEXTMETRIC tm;
            GetTextMetrics(hdc, &tm);

            UINT caretWidth = 1;
            SystemParametersInfo(SPI_GETCARETWIDTH, 0, &caretWidth, 0);
            if (caretWidth < 1) caretWidth = 1;

            SetBkMode(hdc, OPAQUE);
            COLORREF oldBk = SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
            COLORREF oldFg = SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));

            // Draw Custom Blue Highlights
            for (int i = 0; i < g_state.caretCount - 1; i++) {
                int p = g_state.positions[i];
                int a = g_state.anchors[i];

                if (p != a) {
                    int s = GetMin(a, p);
                    int e = GetMax(a, p);
                    int lineS = SendMessage(hWnd, EM_LINEFROMCHAR, s, 0);
                    int lineE = SendMessage(hWnd, EM_LINEFROMCHAR, e, 0);

                    for (int currLine = lineS; currLine <= lineE; currLine++) {
                        int lineStart = SendMessage(hWnd, EM_LINEINDEX, currLine, 0);
                        int lineLen = SendMessage(hWnd, EM_LINELENGTH, lineStart, 0);
                        int drawS = (currLine == lineS) ? s : lineStart;
                        int drawE = (currLine == lineE) ? e : lineStart + lineLen;
                        
                        if (currLine < lineE && drawE == lineStart + lineLen) drawE++; 
                        
                        int selLen = drawE - drawS;
                        if (selLen > 0) {
                            WCHAR* lineBuf = (WCHAR*)malloc((lineLen + 2) * sizeof(WCHAR));
                            *(WORD*)lineBuf = (WORD)lineLen;
                            int copied = SendMessage(hWnd, EM_GETLINE, currLine, (LPARAM)lineBuf);
                            
                            int localS = drawS - lineStart;
                            int x1, y1, x2, y2;
                            
                            if (GetCaretX(hWnd, drawS, &x1, &y1, &tm)) {
                                if (!GetCaretX(hWnd, drawE, &x2, &y2, &tm) || y1 != y2) {
                                    x2 = x1 + selLen * tm.tmAveCharWidth;
                                }
                                
                                RECT rect = { x1, y1, x2, y1 + tm.tmHeight };
                                int charsToDraw = 0;
                                if (localS >= 0 && localS < copied) charsToDraw = GetMin(selLen, copied - localS);
                                
                                if (charsToDraw > 0) {
                                    ExtTextOutW(hdc, x1, y1, ETO_OPAQUE, &rect, lineBuf + localS, charsToDraw, NULL);
                                } else {
                                    ExtTextOutW(hdc, x1, y1, ETO_OPAQUE, &rect, L" ", 1, NULL);
                                }
                            }
                            free(lineBuf);
                        }
                    }
                }
            }

            SetBkColor(hdc, oldBk);
            SetTextColor(hdc, oldFg);

            // Draw Carets
            if (g_caretsVisible) {
                for (int i = 0; i < g_state.caretCount; i++) {
                    int p = g_state.positions[i];
                    int x, y;
                    if (GetCaretX(hWnd, p, &x, &y, &tm)) {
                        RECT rect = { x, y, x + (int)caretWidth, y + tm.tmHeight };
                        InvertRect(hdc, &rect);
                    }
                }
            }
            
            SelectObject(hdc, hOldFont);
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
    if (g_hEditWnd) {
        CancelMultiSelect(g_hEditWnd); 
        SendMessage(g_hEditWnd, g_subclassRegisteredMsg, FALSE, 0);
    }
}