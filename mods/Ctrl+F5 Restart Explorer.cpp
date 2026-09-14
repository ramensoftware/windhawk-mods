// ==WindhawkMod==
// @id              ctrl-f5-restart-explorer
// @name            Ctrl+F5 Restart Explorer
// @description     On desktop or in File Explorer windows, Ctrl+F5 restarts explorer.exe
// @version         1.0
// @author          wakhh@qq.com
// @github          https://github.com/wakhh
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

#include <Windows.h>
#include <windhawk_utils.h>

static void RestartExplorer()
{
    // 通过 cmd 延迟杀掉并重启 explorer
    WCHAR cmd[] = L"cmd.exe /c timeout /t 1 /nobreak >nul & "
                  L"taskkill /f /im explorer.exe >nul 2>&1 & "
                  L"start \"\" explorer.exe";

    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = {0};
    if (CreateProcessW(NULL, cmd, NULL, NULL, FALSE,
                       CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

// 判断顶层窗口是不是桌面或文件夹窗口
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

using TranslateAcceleratorW_t = int(WINAPI*)(HWND, HACCEL, LPMSG);
static TranslateAcceleratorW_t TranslateAcceleratorW_Original = nullptr;

int WINAPI TranslateAcceleratorW_Hook(HWND hWnd, HACCEL hAccel, LPMSG lpMsg)
{
    if (lpMsg && lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_F5 &&
        (GetKeyState(VK_CONTROL) & 0x8000)) {
        Wh_Log(L"[DEBUG] Ctrl+F5 hWnd=%p", hWnd);
        if (IsDesktopOrExplorerWindow(hWnd)) {
            Wh_Log(L"[INFO] desktop/explorer window, restarting");
            RestartExplorer();
            return 1;  // 吞掉按键
        }
    }
    return TranslateAcceleratorW_Original(hWnd, hAccel, lpMsg);
}

BOOL Wh_ModInit()
{
    Wh_Log(L"[INFO] Ctrl+F5 Restart Explorer mod init");

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) {
        Wh_Log(L"[ERROR] user32.dll not found");
        return FALSE;
    }

    void* pTranslateAcceleratorW = (void*)GetProcAddress(hUser32, "TranslateAcceleratorW");
    if (!pTranslateAcceleratorW) {
        Wh_Log(L"[ERROR] TranslateAcceleratorW not found");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(pTranslateAcceleratorW,
                            (void*)TranslateAcceleratorW_Hook,
                            (void**)&TranslateAcceleratorW_Original)) {
        Wh_Log(L"[ERROR] hook TranslateAcceleratorW failed");
        return FALSE;
    }

    Wh_Log(L"[INFO] hook installed");
    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"[INFO] Ctrl+F5 Restart Explorer mod uninit");
}