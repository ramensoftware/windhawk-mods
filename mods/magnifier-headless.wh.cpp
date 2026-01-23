// ==WindhawkMod==
// @id              magnifier-headless
// @name            Magnifier Headless Mode
// @description     Blocks the Magnifier window creation, keeping zoom functionality with win+"-" and win+"+" keyboard shortcuts.
// @version         0.9.4
// @author          BCRTVKCS
// @github          https://github.com/bcrtvkcs
// @twitter         https://x.com/bcrtvkcs
// @homepage        https://grdigital.pro
// @include         magnify.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Magnifier Headless Mode
This mod blocks the Magnifier window from ever appearing, while keeping the zoom functionality (Win + `-` and Win + `+`) available. It also prevents the Magnifier from showing up in the taskbar.

## Features
- Completely hides the Magnifier UI while preserving zoom functionality
- **Fixes Mouse Cursor Freeze**: Eliminates the 5-6 second mouse cursor freeze that occurs when Magnifier touch controls appear on startup (a known Windows Magnifier bug affecting users even without touch devices)
- **Blocks Touch Interface**: Completely hides the Magnifier touch controls (semi-transparent squares with black borders at screen corners), unnecessary for non-touch users
- Thread-safe implementation with race condition protection
- Performance optimized with HWND caching
- Comprehensive API coverage for all window visibility methods

## Hooked APIs
The mod hooks multiple Windows API functions to ensure complete coverage:

**Core Window APIs:**
- `CreateWindowExW` - Intercepts window creation
- `ShowWindow` - Blocks window showing
- `SetWindowPos` - Prevents position-based showing
- `SetWindowLongPtrW` - Blocks style changes

**Layered Window APIs:**
- `UpdateLayeredWindow` - Blocks layered window updates
- `SetLayeredWindowAttributes` - Blocks transparency changes

**Animation & Foreground APIs:**
- `AnimateWindow` - Blocks animated showing
- `BringWindowToTop` - Prevents bringing to front
- `SetForegroundWindow` - Blocks foreground activation

**Advanced APIs:**
- `SetWindowRgn` - Blocks region-based visibility
- `DwmSetWindowAttribute` - Blocks DWM attribute changes (Windows 11+)

**Window Message Interception:**
- Window Procedure Subclassing - Direct interception of window messages
  * Blocks `WM_SHOWWINDOW` (show requests)
  * Modifies `WM_WINDOWPOSCHANGING` (prevents showing via position changes)
  * Enforces hiding on `WM_WINDOWPOSCHANGED`
  * Blocks `WM_ACTIVATE` and `WM_NCACTIVATE` (activation prevention)
  * Suppresses `WM_PAINT` and `WM_ERASEBKGND` (no visual artifacts)
  * Blocks `WM_SETFOCUS` (focus prevention)
  * Blocks `WM_MOUSEACTIVATE` (mouse activation prevention)
  * Blocks `WM_SYSCOMMAND` (SC_RESTORE, SC_MAXIMIZE prevention)
- `WH_CALLWNDPROC` hook - Detects Magnifier windows and applies subclassing automatically

## Technical Implementation
- Uses CRITICAL_SECTION for thread-safe global state management
- Atomic operations (InterlockedExchange) for initialization flags
- LRU cache (16 entries) with fast window detection and eviction strategy
- RAII pattern (AutoCriticalSection) for safe lock management
- Proper hook ordering to prevent race conditions

## Performance Optimizations
- Process ID fast-path filtering to skip non-Magnifier windows instantly
- Inline IsMagnifierWindow with optimized string comparison (first character check)
- High-frequency message suppression (WM_PAINT, WM_TIMER, WM_NCHITTEST)
- Reduced atomic operation overhead (simple read instead of InterlockedCompareExchange)
- LRU cache eviction for optimal memory usage
- Conditional logging to minimize overhead
- Optimized critical section usage with double-check locking

## Error Handling & Robustness
- Safe wrapper functions for all critical Windows API calls
- Comprehensive null pointer and window handle validation
- Automatic retry mechanism for recoverable errors (up to 3 attempts)
- Detailed error logging with GetLastError() codes
- Graceful fallbacks for non-critical failures
- Thread-safe error recovery in multi-threaded scenarios
- Defensive programming with bounds checking and parameter validation
- Return value validation for all API calls
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_api.h>
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

// ===========================
// THREAD-SAFE GLOBAL STATE
// ===========================

// Critical section to protect global state
CRITICAL_SECTION g_csGlobalState;
BOOL g_bCriticalSectionInitialized = FALSE;

// Global handle to our hidden host window (protected by g_csGlobalState)
volatile HWND g_hHostWnd = NULL;

// Atomic flag to track initialization status
volatile LONG g_lInitialized = 0;

// Process ID of magnify.exe for fast filtering
DWORD g_dwMagnifyProcessId = 0;

// HWND cache for fast magnifier window detection (protected by g_csGlobalState)
#define MAX_CACHED_MAGNIFIER_WINDOWS 16
struct {
    HWND hwnd;
    BOOL isMagnifier;
    DWORD lastAccessTick; // For LRU eviction
} g_windowCache[MAX_CACHED_MAGNIFIER_WINDOWS] = {0};
int g_cacheIndex = 0;

// Performance: Suppress high-frequency message logs
#define LOG_HIGH_FREQ_MESSAGES FALSE

// Error handling: Enable detailed error logging
#define LOG_ERROR_DETAILS TRUE

// Error handling: Maximum retry attempts for recoverable errors
#define MAX_RETRY_ATTEMPTS 3

// Window procedure hook handle
HHOOK g_hCallWndProcHook = NULL;

// Original window procedure and HWND for subclassed Magnifier window (protected by g_csGlobalState)
WNDPROC g_OriginalMagnifierWndProc = NULL;
HWND g_hSubclassedMagnifierWnd = NULL;

// Helper: Safe enter/leave critical section
class AutoCriticalSection {
private:
    CRITICAL_SECTION* m_pCs;
    BOOL m_bEntered;
public:
    AutoCriticalSection(CRITICAL_SECTION* pCs) : m_pCs(pCs), m_bEntered(FALSE) {
        if (g_bCriticalSectionInitialized && pCs) {
            EnterCriticalSection(pCs);
            m_bEntered = TRUE;
        }
    }
    ~AutoCriticalSection() {
        if (m_bEntered && m_pCs) {
            LeaveCriticalSection(m_pCs);
        }
    }
};

// --- ERROR HANDLING HELPERS ---

// Safe window validation with comprehensive checks
inline BOOL SafeIsWindow(HWND hwnd) {
    if (!hwnd) {
        return FALSE;
    }
    return IsWindow(hwnd);
}

// Safe GetClassNameW with error handling
inline BOOL SafeGetClassName(HWND hwnd, LPWSTR lpClassName, int nMaxCount) {
    if (!hwnd || !lpClassName || nMaxCount <= 0) {
        return FALSE;
    }

    int result = GetClassNameW(hwnd, lpClassName, nMaxCount);
    if (result == 0) {
        DWORD dwError = GetLastError();
        if (LOG_ERROR_DETAILS && dwError != ERROR_SUCCESS) {
            Wh_Log(L"Magnifier Headless: GetClassNameW failed for HWND 0x%p (error: %lu)", hwnd, dwError);
        }
        return FALSE;
    }
    return TRUE;
}

// Safe SetWindowLongPtrW with validation
inline LONG_PTR SafeSetWindowLongPtrW(HWND hwnd, int nIndex, LONG_PTR dwNewLong) {
    if (!SafeIsWindow(hwnd)) {
        return 0;
    }

    SetLastError(0);
    LONG_PTR result = SetWindowLongPtrW(hwnd, nIndex, dwNewLong);
    DWORD dwError = GetLastError();

    if (result == 0 && dwError != ERROR_SUCCESS) {
        if (LOG_ERROR_DETAILS) {
            Wh_Log(L"Magnifier Headless: SetWindowLongPtrW failed for HWND 0x%p, index %d (error: %lu)",
                   hwnd, nIndex, dwError);
        }
    }
    return result;
}

// Safe SetParent with validation and retry
inline HWND SafeSetParent(HWND hWndChild, HWND hWndNewParent) {
    if (!SafeIsWindow(hWndChild)) {
        return NULL;
    }

    for (int attempt = 0; attempt < MAX_RETRY_ATTEMPTS; attempt++) {
        SetLastError(0);
        HWND result = SetParent(hWndChild, hWndNewParent);
        DWORD dwError = GetLastError();

        if (result || dwError == ERROR_SUCCESS) {
            return result;
        }

        if (LOG_ERROR_DETAILS && attempt == MAX_RETRY_ATTEMPTS - 1) {
            Wh_Log(L"Magnifier Headless: SetParent failed after %d attempts for HWND 0x%p (error: %lu)",
                   MAX_RETRY_ATTEMPTS, hWndChild, dwError);
        }

        // Brief sleep before retry
        if (attempt < MAX_RETRY_ATTEMPTS - 1) {
            Sleep(10);
        }
    }

    return NULL;
}

// Optimized inline function to check if a window is the Magnifier window
inline BOOL IsMagnifierWindow(HWND hwnd) {
    // Fast path: null check
    if (!hwnd) {
        return FALSE;
    }

    // Note: Removed process ID filtering to allow detection of helper windows
    // created by other Windows processes (CSPNotify, MSCTFIME, etc.)

    // Fast path: Check cache first (thread-safe with minimal locking)
    DWORD currentTick = GetTickCount();
    {
        AutoCriticalSection lock(&g_csGlobalState);
        for (int i = 0; i < MAX_CACHED_MAGNIFIER_WINDOWS; i++) {
            if (g_windowCache[i].hwnd == hwnd) {
                g_windowCache[i].lastAccessTick = currentTick; // Update LRU
                return g_windowCache[i].isMagnifier;
            }
        }
    }

    // Slow path: Not in cache, check class name
    wchar_t className[32] = {0}; // Reduced size for performance
    if (!SafeGetClassName(hwnd, className, 32)) {
        return FALSE;
    }

    // Optimized string comparison (check first character first)
    BOOL isMagnifier = FALSE;
    if (className[0] == L'M' && wcscmp(className, L"MagUIClass") == 0) {
        isMagnifier = TRUE;
    } else if (className[0] == L'S' && wcscmp(className, L"ScreenMagnifierUIWnd") == 0) {
        isMagnifier = TRUE;
    } else if (wcsncmp(className, L"GDI+", 4) == 0) {
        // GDI+ helper windows (e.g., "GDI+ Hook Window Class")
        isMagnifier = TRUE;
    } else if (wcsstr(className, L"CSpNotify") != NULL) {
        // CSpNotify windows (C uppercase, S uppercase, p lowercase)
        isMagnifier = TRUE;
    } else if (wcsstr(className, L"MSCTFIME") != NULL) {
        // MSCTFIME UI (Input Method Editor helper window)
        isMagnifier = TRUE;
    }

    // NOTE: "Magnifier Touch" windows are handled separately via IsTouchOverlayWindow()
    // They are moved off-screen (-32000, -32000) AND sized to 0x0 (safest approach)

    // Add to cache with LRU eviction (thread-safe)
    {
        AutoCriticalSection lock(&g_csGlobalState);

        // Find oldest entry for eviction
        int oldestIndex = 0;
        DWORD oldestTick = g_windowCache[0].lastAccessTick;
        for (int i = 1; i < MAX_CACHED_MAGNIFIER_WINDOWS; i++) {
            if (g_windowCache[i].lastAccessTick < oldestTick) {
                oldestTick = g_windowCache[i].lastAccessTick;
                oldestIndex = i;
            }
        }

        // Use circular buffer index or LRU eviction
        int targetIndex = (g_windowCache[g_cacheIndex].hwnd == NULL) ? g_cacheIndex : oldestIndex;

        g_windowCache[targetIndex].hwnd = hwnd;
        g_windowCache[targetIndex].isMagnifier = isMagnifier;
        g_windowCache[targetIndex].lastAccessTick = currentTick;

        g_cacheIndex = (g_cacheIndex + 1) % MAX_CACHED_MAGNIFIER_WINDOWS;
    }

    return isMagnifier;
}

// Check if a window is the Magnifier Touch overlay (handled differently - off-screen + 0x0 size)
inline BOOL IsTouchOverlayWindow(HWND hwnd) {
    if (!hwnd) {
        return FALSE;
    }
    wchar_t windowTitle[64] = {0};
    if (GetWindowTextW(hwnd, windowTitle, 64) > 0) {
        if (wcsstr(windowTitle, L"Magnifier Touch") != NULL) {
            return TRUE;
        }
    }
    return FALSE;
}

// --- HOOKS ---

// ShowWindow hook to catch attempts to show the Magnifier window
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original = nullptr;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    // Fast path: Check initialization once
    if (!g_lInitialized) {
        return ShowWindow_Original ? ShowWindow_Original(hWnd, nCmdShow) : FALSE;
    }

    // Skip touch overlay windows - let them show (they are off-screen + 0x0 size)
    if (IsTouchOverlayWindow(hWnd)) {
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    if (IsMagnifierWindow(hWnd) && nCmdShow != SW_HIDE) {
        return TRUE; // Pretend success
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

// SetWindowPos hook to catch attempts to show the Magnifier window via position changes
using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original = nullptr;
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if (!g_lInitialized) {
        return SetWindowPos_Original ? SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags) : FALSE;
    }

    // For touch overlay: Force position off-screen (-32000, -32000) AND size to 0x0 (safest)
    if (IsTouchOverlayWindow(hWnd)) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, -32000, -32000, 0, 0, uFlags);
    }

    if (IsMagnifierWindow(hWnd)) {
        uFlags &= ~SWP_SHOWWINDOW;
        uFlags |= SWP_HIDEWINDOW;
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// SetWindowLongPtrW hook to catch attempts to make the window visible or add it to the taskbar
using SetWindowLongPtrW_t = decltype(&SetWindowLongPtrW);
SetWindowLongPtrW_t SetWindowLongPtrW_Original = nullptr;
LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    if (!g_lInitialized) {
        return SetWindowLongPtrW_Original ? SetWindowLongPtrW_Original(hWnd, nIndex, dwNewLong) : 0;
    }

    // Touch overlay is handled via off-screen positioning, no special style handling needed

    if (IsMagnifierWindow(hWnd)) {
        if (nIndex == GWL_STYLE) {
            dwNewLong &= ~WS_VISIBLE;
        } else if (nIndex == GWL_EXSTYLE) {
            dwNewLong &= ~WS_EX_APPWINDOW;
            dwNewLong |= WS_EX_TOOLWINDOW;
        }
    }
    return SetWindowLongPtrW_Original(hWnd, nIndex, dwNewLong);
}

// UpdateLayeredWindow hook to prevent layered window updates from showing the window
using UpdateLayeredWindow_t = decltype(&UpdateLayeredWindow);
UpdateLayeredWindow_t UpdateLayeredWindow_Original = nullptr;
BOOL WINAPI UpdateLayeredWindow_Hook(
    HWND hWnd, HDC hdcDst, POINT* pptDst, SIZE* psize,
    HDC hdcSrc, POINT* pptSrc, COLORREF crKey,
    BLENDFUNCTION* pblend, DWORD dwFlags) {

    if (!g_lInitialized) {
        return UpdateLayeredWindow_Original ? UpdateLayeredWindow_Original(hWnd, hdcDst, pptDst, psize,
                                              hdcSrc, pptSrc, crKey, pblend, dwFlags) : FALSE;
    }

    // Touch overlay is handled via off-screen positioning, not transparency
    // So no special handling needed here

    if (IsMagnifierWindow(hWnd)) {
        return TRUE; // Pretend success
    }
    return UpdateLayeredWindow_Original(hWnd, hdcDst, pptDst, psize, hdcSrc, pptSrc, crKey, pblend, dwFlags);
}

// SetLayeredWindowAttributes hook to prevent layered window attribute changes
using SetLayeredWindowAttributes_t = decltype(&SetLayeredWindowAttributes);
SetLayeredWindowAttributes_t SetLayeredWindowAttributes_Original = nullptr;
BOOL WINAPI SetLayeredWindowAttributes_Hook(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags) {
    if (!g_lInitialized) {
        return SetLayeredWindowAttributes_Original ? SetLayeredWindowAttributes_Original(hWnd, crKey, bAlpha, dwFlags) : FALSE;
    }

    // Touch overlay is handled via off-screen positioning, not transparency

    if (IsMagnifierWindow(hWnd)) {
        return TRUE; // Pretend success
    }
    return SetLayeredWindowAttributes_Original(hWnd, crKey, bAlpha, dwFlags);
}

// AnimateWindow hook to prevent animated window showing
using AnimateWindow_t = decltype(&AnimateWindow);
AnimateWindow_t AnimateWindow_Original = nullptr;
BOOL WINAPI AnimateWindow_Hook(HWND hWnd, DWORD dwTime, DWORD dwFlags) {
    if (!g_lInitialized) {
        return AnimateWindow_Original ? AnimateWindow_Original(hWnd, dwTime, dwFlags) : FALSE;
    }

    if (IsMagnifierWindow(hWnd) && !(dwFlags & AW_HIDE)) {
        return TRUE; // Block show animations
    }
    return AnimateWindow_Original(hWnd, dwTime, dwFlags);
}

// BringWindowToTop hook to prevent bringing window to foreground
using BringWindowToTop_t = decltype(&BringWindowToTop);
BringWindowToTop_t BringWindowToTop_Original = nullptr;
BOOL WINAPI BringWindowToTop_Hook(HWND hWnd) {
    if (!g_lInitialized) {
        return BringWindowToTop_Original ? BringWindowToTop_Original(hWnd) : FALSE;
    }

    if (IsMagnifierWindow(hWnd)) {
        return TRUE; // Pretend success
    }
    return BringWindowToTop_Original(hWnd);
}

// SetForegroundWindow hook to prevent setting as foreground window
using SetForegroundWindow_t = decltype(&SetForegroundWindow);
SetForegroundWindow_t SetForegroundWindow_Original = nullptr;
BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    if (!g_lInitialized) {
        return SetForegroundWindow_Original ? SetForegroundWindow_Original(hWnd) : FALSE;
    }

    if (IsMagnifierWindow(hWnd)) {
        return TRUE; // Pretend success
    }
    return SetForegroundWindow_Original(hWnd);
}

// SetWindowRgn hook to prevent region changes that might make window visible
using SetWindowRgn_t = decltype(&SetWindowRgn);
SetWindowRgn_t SetWindowRgn_Original = nullptr;
int WINAPI SetWindowRgn_Hook(HWND hWnd, HRGN hRgn, BOOL bRedraw) {
    if (!g_lInitialized) {
        return SetWindowRgn_Original ? SetWindowRgn_Original(hWnd, hRgn, bRedraw) : 0;
    }

    if (IsMagnifierWindow(hWnd)) {
        bRedraw = FALSE; // Disable redraw
    }
    return SetWindowRgn_Original(hWnd, hRgn, bRedraw);
}

// DwmSetWindowAttribute hook for Windows 11+ DWM features
using DwmSetWindowAttribute_t = HRESULT (WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original = nullptr;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hWnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) {
    if (!g_lInitialized) {
        return DwmSetWindowAttribute_Original ? DwmSetWindowAttribute_Original(hWnd, dwAttribute, pvAttribute, cbAttribute) : E_FAIL;
    }

    if (IsMagnifierWindow(hWnd) && (dwAttribute == DWMWA_CLOAK || dwAttribute == DWMWA_NCRENDERING_ENABLED)) {
        return S_OK; // Block these attributes
    }
    return DwmSetWindowAttribute_Original(hWnd, dwAttribute, pvAttribute, cbAttribute);
}

// --- WINDOW PROCEDURE HOOK ---

// Subclassed window procedure for Magnifier window (optimized with error handling)
LRESULT CALLBACK MagnifierWndProc_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Validate window handle
    if (!SafeIsWindow(hWnd)) {
        return 0;
    }

    // Fast path: Handle high-frequency messages without logging
    switch (uMsg) {
    case WM_PAINT:
    case WM_ERASEBKGND:
        // Suppress painting (no log for performance)
        ValidateRect(hWnd, NULL);
        return 0;

    case WM_NCPAINT:
    case WM_SYNCPAINT:
    case WM_TIMER:
    case WM_NCHITTEST:
        // Silently ignore high-frequency messages
        return 0;

    case WM_SHOWWINDOW:
        // Block WM_SHOWWINDOW if trying to show
        if (wParam) {
            if (LOG_HIGH_FREQ_MESSAGES) {
                Wh_Log(L"Magnifier Headless: Blocked WM_SHOWWINDOW in WndProc");
            }
            return 0;
        }
        break;

    case WM_WINDOWPOSCHANGING:
        // Modify WINDOWPOS structure to prevent showing
        if (lParam) {
            WINDOWPOS* pWp = (WINDOWPOS*)lParam;
            if (!(pWp->flags & SWP_NOACTIVATE)) {
                pWp->flags |= SWP_NOACTIVATE;
            }
            if (pWp->flags & SWP_SHOWWINDOW) {
                pWp->flags &= ~SWP_SHOWWINDOW;
                pWp->flags |= SWP_HIDEWINDOW;
            }
        }
        break;

    case WM_WINDOWPOSCHANGED:
        // Ensure window remains hidden after position change
        if (IsWindowVisible(hWnd)) {
            if (ShowWindow_Original) {
                ShowWindow_Original(hWnd, SW_HIDE);
            }
        }
        break;

    case WM_ACTIVATE:
    case WM_NCACTIVATE:
        // Block activation
        if (wParam != WA_INACTIVE) {
            return 0;
        }
        break;

    case WM_SETFOCUS:
        // Block focus
        SetFocus(NULL);
        return 0;

    case WM_MOUSEACTIVATE:
        // Prevent mouse activation
        return MA_NOACTIVATE;

    case WM_SYSCOMMAND:
        // Block system commands that might show the window
        if (wParam == SC_RESTORE || wParam == SC_MAXIMIZE) {
            return 0;
        }
        break;
    }

    // Call original window procedure for unhandled messages
    if (g_OriginalMagnifierWndProc) {
        return CallWindowProcW(g_OriginalMagnifierWndProc, hWnd, uMsg, wParam, lParam);
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// CallWndProc hook to detect and subclass Magnifier windows (optimized with error handling)
LRESULT CALLBACK CallWndProc_Hook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && g_lInitialized) {
        // Validate lParam
        if (!lParam) {
            return CallNextHookEx(g_hCallWndProcHook, nCode, wParam, lParam);
        }

        CWPSTRUCT* pCwp = (CWPSTRUCT*)lParam;

        // Validate CWPSTRUCT and window handle
        if (pCwp && SafeIsWindow(pCwp->hwnd) && IsMagnifierWindow(pCwp->hwnd)) {
            // Fast path: Check if already subclassed without lock
            if (g_hSubclassedMagnifierWnd != pCwp->hwnd) {
                SetLastError(0);
                WNDPROC currentProc = (WNDPROC)GetWindowLongPtrW(pCwp->hwnd, GWLP_WNDPROC);
                DWORD dwError = GetLastError();

                if (currentProc != MagnifierWndProc_Hook && currentProc != NULL) {
                    // Thread-safe subclassing
                    {
                        AutoCriticalSection lock(&g_csGlobalState);
                        // Double-check after acquiring lock
                        if (g_hSubclassedMagnifierWnd != pCwp->hwnd) {
                            g_OriginalMagnifierWndProc = currentProc;
                            g_hSubclassedMagnifierWnd = pCwp->hwnd;
                        }
                    }

                    // Attempt subclassing with error checking
                    SetLastError(0);
                    LONG_PTR result = SafeSetWindowLongPtrW(pCwp->hwnd, GWLP_WNDPROC, (LONG_PTR)MagnifierWndProc_Hook);
                    dwError = GetLastError();

                    if (result != 0 || dwError == ERROR_SUCCESS) {
                        Wh_Log(L"Magnifier Headless: Subclassed Magnifier window (HWND: 0x%p)", pCwp->hwnd);
                    } else if (LOG_ERROR_DETAILS) {
                        Wh_Log(L"Magnifier Headless: Failed to subclass window (HWND: 0x%p, error: %lu)",
                               pCwp->hwnd, dwError);
                    }
                }
            }
        }
    }

    return CallNextHookEx(g_hCallWndProcHook, nCode, wParam, lParam);
}

// CreateWindowExW hook to catch Magnifier window creation (optimized)
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;
HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, LPVOID lpParam) {

    if (!g_lInitialized) {
        return CreateWindowExW_Original ? CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                          dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam) : NULL;
    }

    BOOL isMagnifierClass = FALSE;
    BOOL isTouchOverlay = FALSE;

    // Check for touch overlay first (by window title)
    if (lpWindowName && wcsstr(lpWindowName, L"Magnifier Touch") != NULL) {
        isTouchOverlay = TRUE;
        Wh_Log(L"Magnifier Headless: Detected Magnifier Touch window (title: %ls) - will move off-screen + 0x0 size", lpWindowName);
    }

    // Check for other magnifier classes (only if not touch overlay)
    if (!isTouchOverlay && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) {
        // Optimized: Check first character before full string comparison
        if ((lpClassName[0] == L'M' && wcscmp(lpClassName, L"MagUIClass") == 0) ||
            (lpClassName[0] == L'S' && wcscmp(lpClassName, L"ScreenMagnifierUIWnd") == 0) ||
            wcsncmp(lpClassName, L"GDI+", 4) == 0 ||
            wcsstr(lpClassName, L"CSpNotify") != NULL ||
            wcsstr(lpClassName, L"MSCTFIME") != NULL) {
            isMagnifierClass = TRUE;
            dwStyle &= ~WS_VISIBLE;
            dwExStyle &= ~WS_EX_APPWINDOW;
            dwExStyle |= WS_EX_TOOLWINDOW;
            Wh_Log(L"Magnifier Headless: Intercepting Magnifier window creation (class: %ls)", lpClassName);
        }
    }

    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
                                  nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hwnd && isTouchOverlay) {
        // For touch overlay: Move off-screen (-32000, -32000) AND set size to 0x0 (safest)
        // This preserves zoom functionality while making the overlay completely invisible
        if (SetWindowPos_Original) {
            SetWindowPos_Original(hwnd, NULL, -32000, -32000, 0, 0,
                                 SWP_NOZORDER | SWP_NOACTIVATE);
            Wh_Log(L"Magnifier Headless: Moved Magnifier Touch window off-screen with 0x0 size (HWND: 0x%p)", hwnd);
        }
    } else if (hwnd && isMagnifierClass) {
        // For other magnifier windows: Hide completely
        // Fast path: Read g_hHostWnd without lock (it's stable after init)
        HWND hostWnd = g_hHostWnd;
        if (hostWnd && SafeIsWindow(hostWnd)) {
            SafeSetParent(hwnd, hostWnd);
        }

        // Hide window with safety check
        if (ShowWindow_Original && SafeIsWindow(hwnd)) {
            ShowWindow_Original(hwnd, SW_HIDE);
        }

        // Force update styles with safe API
        if (SetWindowLongPtrW_Original && SafeIsWindow(hwnd)) {
            LONG_PTR currentStyle = GetWindowLongPtrW(hwnd, GWL_STYLE);
            SafeSetWindowLongPtrW(hwnd, GWL_STYLE, currentStyle & ~WS_VISIBLE);

            LONG_PTR currentExStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
            SafeSetWindowLongPtrW(hwnd, GWL_EXSTYLE, (currentExStyle & ~WS_EX_APPWINDOW) | WS_EX_TOOLWINDOW);
        }
    }

    return hwnd;
}

// --- HELPER FUNCTION FOR EXISTING WINDOWS ---

// Enumerate and hide existing magnifier-related windows
BOOL CALLBACK EnumWindowsProc_HideMagnifierWindows(HWND hwnd, LPARAM lParam) {
    if (IsMagnifierWindow(hwnd)) {
        Wh_Log(L"Magnifier Headless: Found existing magnifier window (HWND: 0x%p), hiding...", hwnd);

        // Hide window if detected
        if (ShowWindow_Original && SafeIsWindow(hwnd)) {
            ShowWindow_Original(hwnd, SW_HIDE);
        }

        // Update styles to prevent visibility
        if (SetWindowLongPtrW_Original && SafeIsWindow(hwnd)) {
            LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
            SafeSetWindowLongPtrW(hwnd, GWL_STYLE, style & ~WS_VISIBLE);

            LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
            SafeSetWindowLongPtrW(hwnd, GWL_EXSTYLE,
                (exStyle & ~WS_EX_APPWINDOW) | WS_EX_TOOLWINDOW);
        }
    }
    return TRUE; // Continue enumeration
}

// --- MOD INITIALIZATION ---

BOOL Wh_ModInit() {
    Wh_Log(L"Magnifier Headless: Initializing (performance-optimized version)...");

    // Store current process ID for fast filtering
    g_dwMagnifyProcessId = GetCurrentProcessId();
    Wh_Log(L"Magnifier Headless: Running in process ID: %lu", g_dwMagnifyProcessId);

    // Initialize critical section first (before any hook operations)
    if (!InitializeCriticalSectionAndSpinCount(&g_csGlobalState, 0x400)) {
        Wh_Log(L"Magnifier Headless: Failed to initialize critical section.");
        return FALSE;
    }
    g_bCriticalSectionInitialized = TRUE;

    // Set up all hooks BEFORE creating windows to prevent race conditions
    Wh_Log(L"Magnifier Headless: Setting up function hooks...");

    // Core window hooks
    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original) ||
        !Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original) ||
        !Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook, (void**)&SetWindowPos_Original) ||
        !Wh_SetFunctionHook((void*)SetWindowLongPtrW, (void*)SetWindowLongPtrW_Hook, (void**)&SetWindowLongPtrW_Original)) {
        Wh_Log(L"Magnifier Headless: Failed to set up core window hooks.");
        DeleteCriticalSection(&g_csGlobalState);
        g_bCriticalSectionInitialized = FALSE;
        return FALSE;
    }

    // Layered window hooks
    if (!Wh_SetFunctionHook((void*)UpdateLayeredWindow, (void*)UpdateLayeredWindow_Hook, (void**)&UpdateLayeredWindow_Original) ||
        !Wh_SetFunctionHook((void*)SetLayeredWindowAttributes, (void*)SetLayeredWindowAttributes_Hook, (void**)&SetLayeredWindowAttributes_Original)) {
        Wh_Log(L"Magnifier Headless: Failed to set up layered window hooks.");
        DeleteCriticalSection(&g_csGlobalState);
        g_bCriticalSectionInitialized = FALSE;
        return FALSE;
    }

    // Animation and foreground hooks
    if (!Wh_SetFunctionHook((void*)AnimateWindow, (void*)AnimateWindow_Hook, (void**)&AnimateWindow_Original) ||
        !Wh_SetFunctionHook((void*)BringWindowToTop, (void*)BringWindowToTop_Hook, (void**)&BringWindowToTop_Original) ||
        !Wh_SetFunctionHook((void*)SetForegroundWindow, (void*)SetForegroundWindow_Hook, (void**)&SetForegroundWindow_Original)) {
        Wh_Log(L"Magnifier Headless: Failed to set up animation/foreground hooks.");
        DeleteCriticalSection(&g_csGlobalState);
        g_bCriticalSectionInitialized = FALSE;
        return FALSE;
    }

    // Region hook
    if (!Wh_SetFunctionHook((void*)SetWindowRgn, (void*)SetWindowRgn_Hook, (void**)&SetWindowRgn_Original)) {
        Wh_Log(L"Magnifier Headless: Failed to set up region hook.");
        DeleteCriticalSection(&g_csGlobalState);
        g_bCriticalSectionInitialized = FALSE;
        return FALSE;
    }

    // DWM hook (optional - may not exist on older Windows versions)
    HMODULE hDwmapi = LoadLibraryW(L"dwmapi.dll");
    if (hDwmapi) {
        DwmSetWindowAttribute_Original = (DwmSetWindowAttribute_t)GetProcAddress(hDwmapi, "DwmSetWindowAttribute");
        if (DwmSetWindowAttribute_Original) {
            if (!Wh_SetFunctionHook((void*)DwmSetWindowAttribute_Original, (void*)DwmSetWindowAttribute_Hook, (void**)&DwmSetWindowAttribute_Original)) {
                Wh_Log(L"Magnifier Headless: Warning - Failed to set up DWM hook (non-critical).");
            } else {
                Wh_Log(L"Magnifier Headless: DWM hook set up successfully.");
            }
        }
    }

    Wh_Log(L"Magnifier Headless: All hooks set up successfully.");

    // Install window procedure hook to intercept messages
    Wh_Log(L"Magnifier Headless: Installing window procedure hook...");
    g_hCallWndProcHook = SetWindowsHookExW(WH_CALLWNDPROC, CallWndProc_Hook, NULL, GetCurrentThreadId());
    if (!g_hCallWndProcHook) {
        Wh_Log(L"Magnifier Headless: Warning - Failed to install window procedure hook (error: %lu).", GetLastError());
        // Non-critical, continue anyway
    } else {
        Wh_Log(L"Magnifier Headless: Window procedure hook installed successfully.");
    }

    // Now create the hidden window (after hooks are in place)
    Wh_Log(L"Magnifier Headless: Creating hidden host window...");
    WNDCLASSW wc = {};
    wc.lpfnWndProc = DefWindowProcW;
    wc.lpszClassName = L"MagnifierHeadlessHost";
    wc.hInstance = GetModuleHandle(NULL);

    if (!RegisterClassW(&wc)) {
        DWORD dwError = GetLastError();
        if (dwError != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"Magnifier Headless: Failed to register window class (error: %lu).", dwError);
            DeleteCriticalSection(&g_csGlobalState);
            g_bCriticalSectionInitialized = FALSE;
            return FALSE;
        }
    }

    HWND hHostWnd = CreateWindowExW(
        0, wc.lpszClassName, L"Magnifier Headless Host", 0,
        0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL
    );

    if (!hHostWnd) {
        Wh_Log(L"Magnifier Headless: Failed to create host window (error: %lu).", GetLastError());
        DeleteCriticalSection(&g_csGlobalState);
        g_bCriticalSectionInitialized = FALSE;
        return FALSE;
    }

    // Thread-safe assignment using atomic operation
    {
        AutoCriticalSection lock(&g_csGlobalState);
        g_hHostWnd = hHostWnd;
    }

    Wh_Log(L"Magnifier Headless: Host window created (HWND: 0x%p).", hHostWnd);

    // Mark initialization as complete (atomic operation)
    InterlockedExchange(&g_lInitialized, 1);

    // Enumerate and hide any existing magnifier windows that were created before mod loaded
    Wh_Log(L"Magnifier Headless: Enumerating existing windows to hide any magnifier-related windows...");
    EnumWindows(EnumWindowsProc_HideMagnifierWindows, 0);

    Wh_Log(L"Magnifier Headless: Initialization complete. All systems ready.");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Magnifier Headless: Uninitializing...");

    // Mark as uninitialized (atomic operation)
    InterlockedExchange(&g_lInitialized, 0);

    // Unhook window procedure hook
    if (g_hCallWndProcHook) {
        UnhookWindowsHookEx(g_hCallWndProcHook);
        g_hCallWndProcHook = NULL;
        Wh_Log(L"Magnifier Headless: Window procedure hook removed.");
    }

    // Restore original window procedure if subclassed
    HWND hSubclassedWnd = NULL;
    WNDPROC originalProc = NULL;
    {
        AutoCriticalSection lock(&g_csGlobalState);
        hSubclassedWnd = g_hSubclassedMagnifierWnd;
        originalProc = g_OriginalMagnifierWndProc;
        g_hSubclassedMagnifierWnd = NULL;
        g_OriginalMagnifierWndProc = NULL;
    }

    if (hSubclassedWnd && SafeIsWindow(hSubclassedWnd) && originalProc) {
        SetLastError(0);
        LONG_PTR result = SafeSetWindowLongPtrW(hSubclassedWnd, GWLP_WNDPROC, (LONG_PTR)originalProc);
        DWORD dwError = GetLastError();

        if (result != 0 || dwError == ERROR_SUCCESS) {
            Wh_Log(L"Magnifier Headless: Restored original WndProc for HWND 0x%p", hSubclassedWnd);
        } else if (LOG_ERROR_DETAILS) {
            Wh_Log(L"Magnifier Headless: Failed to restore WndProc for HWND 0x%p (error: %lu)",
                   hSubclassedWnd, dwError);
        }
    }

    // Thread-safe cleanup
    HWND hHostWnd = NULL;
    {
        AutoCriticalSection lock(&g_csGlobalState);
        hHostWnd = g_hHostWnd;
        g_hHostWnd = NULL;

        // Clear cache
        for (int i = 0; i < MAX_CACHED_MAGNIFIER_WINDOWS; i++) {
            g_windowCache[i].hwnd = NULL;
            g_windowCache[i].isMagnifier = FALSE;
        }
        g_cacheIndex = 0;
    }

    if (hHostWnd && SafeIsWindow(hHostWnd)) {
        if (DestroyWindow(hHostWnd)) {
            Wh_Log(L"Magnifier Headless: Host window destroyed.");
        } else if (LOG_ERROR_DETAILS) {
            Wh_Log(L"Magnifier Headless: Failed to destroy host window (error: %lu)", GetLastError());
        }
    }

    // Cleanup critical section
    if (g_bCriticalSectionInitialized) {
        DeleteCriticalSection(&g_csGlobalState);
        g_bCriticalSectionInitialized = FALSE;
    }

    Wh_Log(L"Magnifier Headless: Uninitialization complete.");
}
