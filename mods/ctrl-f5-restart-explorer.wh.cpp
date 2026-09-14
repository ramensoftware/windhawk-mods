// ==WindhawkMod==
// @id              ctrl-f5-restart-explorer
// @name            Ctrl+F5 Restart Explorer
// @description     On desktop or in File Explorer windows, Ctrl+F5 restarts explorer.exe
// @version         1.1
// @author          wakhh
// @github          https://github.com/wakhh
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Restarts explorer.exe when **Ctrl+F5** is pressed on the desktop or in a File Explorer window.

### What it does
- Forcefully kills **all** `explorer.exe` processes in the session (including folder windows
  running in separate processes if "Launch folder windows in a separate process" is enabled).
- Restarts the shell.

### Trigger scope
- Only fires when the foreground window is the desktop (`Progman`/`WorkerW`) or a File Explorer
  frame (`CabinetWClass`).
- Does **not** fire when focus is inside a text edit control — the shortcut is blocked while
  renaming a file, typing in the address bar, or using the search box.
- Requires **exact** `Ctrl+F5`. Holding Shift/Alt/Win in addition to Ctrl will prevent the
  restart (so your browser's hard-refresh muscle memory stays intact).

### Notes
- Restart is forced (`taskkill /f`) — any unsaved edits in open Explorer windows will be lost
  immediately with no grace period.
- The restart process is not graceful. When `explorer.exe` is force-killed, Windows may auto-restart
  it via `AutoRestartShell`, which can race with the `start explorer.exe` launch and produce a
  stray "This PC" window. There is no clean way to avoid this in a force-kill restart flow.
*/
// ==/WindhawkModReadme==

#include <Windows.h>
#include <windhawk_utils.h>
#include <atomic>

static std::atomic<bool> g_restartInProgress{false};

static void RestartExplorer()
{
    if (g_restartInProgress.exchange(true)) return;

    WCHAR cmdLine[] = L"cmd.exe /c taskkill /f /im explorer.exe >nul 2>&1 & start \"\" explorer.exe";

    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = {0};
    if (CreateProcessW(NULL, cmdLine, NULL, NULL, FALSE,
                       CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        Wh_Log(L"CreateProcessW ok");
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    } else {
        Wh_Log(L"CreateProcessW failed: %lu", GetLastError());
    }

    Sleep(500);
    g_restartInProgress.store(false);
}

static bool IsDesktopOrExplorerWindow(HWND hWnd)
{
    if (!hWnd) return false;
    HWND root = GetAncestor(hWnd, GA_ROOT);
    if (!root) return false;

    WCHAR cls[64] = {0};
    GetClassNameW(root, cls, 64);

    return _wcsicmp(cls, L"Progman") == 0 ||
           _wcsicmp(cls, L"WorkerW") == 0 ||
           _wcsicmp(cls, L"CabinetWClass") == 0;
}

static bool IsFocusInTextControl()
{
    HWND focus = GetFocus();
    if (!focus) return false;

    WCHAR cls[64] = {0};
    GetClassNameW(focus, cls, 64);

    if (_wcsicmp(cls, L"Edit") == 0 ||
        _wcsicmp(cls, L"ComboBox") == 0) return true;

    HWND parent = focus;
    for (int i = 0; i < 16 && parent; i++) {
        parent = GetParent(parent);
        if (!parent) break;
        GetClassNameW(parent, cls, 64);
        if (_wcsicmp(cls, L"Edit") == 0 ||
            _wcsicmp(cls, L"ComboBox") == 0) return true;
    }

    return false;
}

using TranslateAcceleratorW_t = int(WINAPI*)(HWND, HACCEL, LPMSG);
static TranslateAcceleratorW_t TranslateAcceleratorW_Original = nullptr;

int WINAPI TranslateAcceleratorW_Hook(HWND hWnd, HACCEL hAccel, LPMSG lpMsg)
{
    if (lpMsg &&
        lpMsg->message == WM_KEYDOWN &&
        lpMsg->wParam == VK_F5 &&
        !(lpMsg->lParam & 0x40000000) &&
        (GetKeyState(VK_CONTROL) & 0x8000) &&
        !(GetKeyState(VK_SHIFT) & 0x8000) &&
        !(GetKeyState(VK_MENU) & 0x8000) &&
        !(GetKeyState(VK_LWIN) & 0x8000) &&
        !(GetKeyState(VK_RWIN) & 0x8000)) {
        Wh_Log(L"Ctrl+F5 hWnd=%p", hWnd);
        if (IsDesktopOrExplorerWindow(hWnd) && !IsFocusInTextControl()) {
            Wh_Log(L"desktop/explorer window, restarting");
            RestartExplorer();
            return 1;
        }
    }
    return TranslateAcceleratorW_Original(hWnd, hAccel, lpMsg);
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Mod init");

    if (!WindhawkUtils::SetFunctionHook(
            TranslateAcceleratorW,
            TranslateAcceleratorW_Hook,
            &TranslateAcceleratorW_Original)) {
        Wh_Log(L"Hook failed");
        return FALSE;
    }

    Wh_Log(L"Hook installed");
    return TRUE;
}