// ==WindhawkMod==
// @id              universal-fullscreen
// @name            Universal Fullscreen Mode
// @description     Forces specified applications to open in fullscreen mode instead of windowed mode
// @version         1.0.2
// @author          mak7im01
// @github          https://github.com/mak7im01
// @include         *
// @architecture    x86-64
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Universal Fullscreen Mode

This mod automatically opens specified applications in fullscreen (maximized) mode.

## How it works

The mod intercepts the creation of application main windows and automatically applies the maximized window style, forcing applications to open in fullscreen.

System dialogs (Save/Open file dialogs, message boxes, etc.) are NOT affected.

## Configuration

Add executable names to the settings to enable fullscreen mode for specific applications. By default, the mod includes Windows Photos app.

Examples:
- Microsoft.Photos.exe
- Photos.exe
- notepad.exe
- mspaint.exe

## Compatibility

- Windows 10
- Windows 11
- Any Windows application

## Notes

After installing this mod, the specified applications will automatically open in fullscreen mode. You can still manually resize or minimize windows if needed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetExecutables:
  - Microsoft.Photos.exe
  - Photos.exe
  $name: Target executables
  $description: List of executable names that should open in fullscreen mode. The .exe extension is optional — both "notepad" and "notepad.exe" work.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <vector>

// Settings
std::vector<std::wstring> g_targetExecutables;

// Original function pointers
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

// Check if current process is in target list
bool IsTargetProcess() {
    WCHAR exePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, exePath, MAX_PATH) == 0) {
        return false;
    }

    WCHAR* exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;

    for (const auto& target : g_targetExecutables) {
        // Allow entries without .exe suffix: compare both as-is and with .exe appended
        if (_wcsicmp(exeName, target.c_str()) == 0) {
            return true;
        }
        // If the target has no .exe extension, append it for comparison
        const wchar_t* dot = wcsrchr(target.c_str(), L'.');
        if (!dot || _wcsicmp(dot, L".exe") != 0) {
            std::wstring withExt = target + L".exe";
            if (_wcsicmp(exeName, withExt.c_str()) == 0) {
                return true;
            }
        }
    }
    return false;
}

// Returns true if the window class or style indicates a system dialog
// (Save/Open dialogs, message boxes, combo popups, etc.)
bool IsSystemDialog(LPCWSTR lpClassName, DWORD dwStyle, DWORD dwExStyle) {
    // If the window has dialog frame style bits it's likely a dialog — check first,
    // before touching lpClassName at all.
    if ((dwStyle & DS_MODALFRAME) || (dwExStyle & WS_EX_DLGMODALFRAME)) {
        return true;
    }

    // Tool windows (small title bar) are typically popups/palettes, not main windows
    if (dwExStyle & WS_EX_TOOLWINDOW) {
        return true;
    }

    if (!lpClassName) {
        return false;
    }

    // lpClassName can be an integer atom — never call wcscmp on it directly.
    // IS_INTRESOURCE returns true when the value fits in 16 bits (i.e. it's an atom).
    if (IS_INTRESOURCE(lpClassName)) {
        // #32770 atom = 0x8002, #32768 = 0x8000, etc.
        ULONG_PTR atom = (ULONG_PTR)lpClassName;
        return (atom >= 0x8000 && atom <= 0x8004);
    }

    // #32770 is the class for all common dialogs (Save, Open, MessageBox, etc.)
    if (wcscmp(lpClassName, L"#32770") == 0 ||
        wcscmp(lpClassName, L"#32768") == 0 ||
        wcscmp(lpClassName, L"#32769") == 0 ||
        wcscmp(lpClassName, L"#32771") == 0 ||
        wcscmp(lpClassName, L"#32772") == 0) {
        return true;
    }

    // Common system/shell window classes
    static const wchar_t* systemClasses[] = {
        L"ComboLBox",
        L"tooltips_class32",
        L"SysShadow",
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"DV2ControlHost",
        L"MsgrIMEWindowClass",
        L"SysDragImage",
        nullptr
    };

    for (int i = 0; systemClasses[i]; i++) {
        if (_wcsicmp(lpClassName, systemClasses[i]) == 0) {
            return true;
        }
    }

    return false;
}

// Same check but by HWND (for ShowWindow hook)
bool IsSystemDialogHwnd(HWND hWnd) {
    WCHAR className[256] = {};
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) == 0) {
        return true; // can't determine class — skip to be safe
    }

    DWORD dwStyle   = (DWORD)GetWindowLongPtrW(hWnd, GWL_STYLE);
    DWORD dwExStyle = (DWORD)GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

    return IsSystemDialog(className, dwStyle, dwExStyle);
}

// Hook for CreateWindowExW
HWND WINAPI CreateWindowExW_Hook(
    DWORD     dwExStyle,
    LPCWSTR   lpClassName,
    LPCWSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
) {
    if (IsTargetProcess() &&
        hWndParent == NULL &&
        !IsSystemDialog(lpClassName, dwStyle, dwExStyle))
    {
        dwStyle |= WS_MAXIMIZE;
        Wh_Log(L"Intercepted main window creation, applying WS_MAXIMIZE");
    }

    return CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam
    );
}

// Hook for ShowWindow
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (IsTargetProcess() &&
        hWnd &&
        GetParent(hWnd) == NULL &&
        !IsSystemDialogHwnd(hWnd))
    {
        if (nCmdShow == SW_SHOW || nCmdShow == SW_SHOWNORMAL || nCmdShow == SW_RESTORE) {
            nCmdShow = SW_SHOWMAXIMIZED;
            Wh_Log(L"ShowWindow: forcing SW_SHOWMAXIMIZED");
        }
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

void LoadSettings() {
    g_targetExecutables.clear();

    for (int i = 0; ; i++) {
        PCWSTR executable = Wh_GetStringSetting(L"targetExecutables[%d]", i);
        if (!executable || !*executable) {
            Wh_FreeStringSetting(executable);
            break;
        }
        g_targetExecutables.push_back(executable);
        Wh_Log(L"Added target executable: %s", executable);
        Wh_FreeStringSetting(executable);
    }

    if (g_targetExecutables.empty()) {
        g_targetExecutables.push_back(L"Microsoft.Photos.exe");
        g_targetExecutables.push_back(L"Photos.exe");
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Universal Fullscreen Mode - Initializing");
    LoadSettings();

    if (!IsTargetProcess()) {
        Wh_Log(L"Not a target process, skipping hooks");
        return TRUE;
    }

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    Wh_SetFunctionHook((void*)ShowWindow,       (void*)ShowWindow_Hook,       (void**)&ShowWindow_Original);

    Wh_Log(L"Hooks installed");
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!IsTargetProcess()) return;

    HWND hWnd = GetTopWindow(NULL);
    while (hWnd) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);

        if (pid == GetCurrentProcessId() &&
            GetParent(hWnd) == NULL &&
            IsWindowVisible(hWnd) &&
            !IsSystemDialogHwnd(hWnd))
        {
            ShowWindow_Original(hWnd, SW_MAXIMIZE);
            Wh_Log(L"Maximized existing window");
        }

        hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing");
    g_targetExecutables.clear();
}
