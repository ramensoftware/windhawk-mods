// ==WindhawkMod==
// @id              windows-sandbox-close-no-confirm
// @name            Windows Sandbox: close without confirmation
// @name:zh-CN      沙盒关闭免确认
// @description     Stops the sandbox with "wsb stop" when its window is closed
// @description:zh-CN 关闭沙盒窗口时直接执行 wsb stop，不再弹出确认
// @version         1.0
// @author          loliri
// @github          https://github.com/loliri
// @include         WindowsSandboxRemoteSession.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Sandbox: close without confirmation

Closing a Windows Sandbox window normally asks for confirmation before the sandbox is shut down. This mod replaces the close button with a clean `wsb stop`, so the window goes away without the prompt.

## Requirements

This mod targets the new Windows Sandbox, whose remote session process is `WindowsSandboxRemoteSession.exe`. That process is also what the mod's include scope matches: if it does not exist on the system, the sandbox is the older version and the mod has nothing to attach to. The `wsb` command line tool must also be available.

## Notes

The sandbox id is read from the process command line. If it cannot be found, the mod does nothing and the usual confirmation is shown.

Sandboxes which were already open when the mod was enabled are not affected. Close them the usual way, start a new one, and the close button works as described.
*/
// ==/WindhawkModReadme==

#include <atomic>
#include <string>

#include <commctrl.h>

namespace {

constexpr UINT_PTR kSubclassId = 1;

// Guards against a second stop while one is already running, which happens
// when the close button is clicked more than once.
std::atomic<bool> g_stopping;

bool IsHex(wchar_t c)
{
    return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') ||
           (c >= L'A' && c <= L'F');
}

// Looks for a 8-4-4-4-12 GUID in the string.
bool FindGuid(const std::wstring &s, std::wstring &out)
{
    const int lens[5] = {8, 4, 4, 4, 12};

    for (size_t i = 0; i + 36 <= s.size(); i++)
    {
        size_t p = i;
        bool ok = true;

        for (int g = 0; g < 5 && ok; g++)
        {
            for (int k = 0; k < lens[g]; k++, p++)
            {
                if (!IsHex(s[p]))
                {
                    ok = false;
                    break;
                }
            }

            if (ok && g < 4)
            {
                if (s[p] != L'-')
                {
                    ok = false;
                }
                else
                {
                    p++;
                }
            }
        }

        if (ok)
        {
            out = s.substr(i, 36);
            return true;
        }
    }

    return false;
}

// Runs the command without a window and waits for it to finish.
bool RunHidden(const std::wstring &commandLine)
{
    std::wstring commandLineCopy = commandLine;

    STARTUPINFOW si{.cb = sizeof(si)};
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi{};

    if (!CreateProcessW(nullptr, commandLineCopy.data(), nullptr, nullptr,
                        FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
    {
        Wh_Log(L"CreateProcessW failed: %u", GetLastError());
        return false;
    }

    DWORD waitResult = WaitForSingleObject(pi.hProcess, 15000);
    DWORD exitCode = 1;

    if (waitResult == WAIT_OBJECT_0)
    {
        GetExitCodeProcess(pi.hProcess, &exitCode);
    }
    else
    {
        Wh_Log(L"Timed out waiting for the command");
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    Wh_Log(L"Command exited with %u", exitCode);

    return waitResult == WAIT_OBJECT_0 && exitCode == 0;
}

// Stops the sandbox this process belongs to. Returns whether the sandbox was
// stopped, so that the close is only swallowed when it actually was.
bool StopSandbox()
{
    if (g_stopping.exchange(true))
    {
        return true;
    }

    std::wstring commandLine = GetCommandLineW();
    Wh_Log(L"Command line: %s", commandLine.c_str());

    std::wstring guid;
    bool stopped = false;

    if (!FindGuid(commandLine, guid))
    {
        Wh_Log(L"No sandbox id on the command line");
    }
    else
    {
        Wh_Log(L"Stopping sandbox %s", guid.c_str());
        stopped = RunHidden(L"cmd.exe /c wsb stop --id " + guid);
    }

    g_stopping.store(false);

    return stopped;
}

LRESULT CALLBACK SubclassProc(HWND hWnd,
                              UINT msg,
                              WPARAM wParam,
                              LPARAM lParam,
                              UINT_PTR,
                              DWORD_PTR)
{
    switch (msg)
    {
    case WM_CLOSE:
        if (StopSandbox())
        {
            return 0;
        }
        break;

    case WM_NCDESTROY:
        RemoveWindowSubclass(hWnd, SubclassProc, kSubclassId);
        break;
    }

    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

bool IsSandboxWindow(LPCWSTR lpClassName)
{
    return lpClassName && !IS_INTRESOURCE(lpClassName) &&
           wcscmp(lpClassName, L"WinUIDesktopWin32WindowClass") == 0;
}

}  // namespace

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam)
{
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);

    if (hWnd && !hWndParent && !(dwStyle & WS_CHILD) &&
        IsSandboxWindow(lpClassName))
    {
        SetWindowSubclass(hWnd, SubclassProc, kSubclassId, 0);
        Wh_Log(L"Subclassed %p", hWnd);
    }

    return hWnd;
}

BOOL Wh_ModInit()
{
    Wh_Log(L">");

    Wh_SetFunctionHook((void *)CreateWindowExW, (void *)CreateWindowExW_Hook,
                       (void **)&CreateWindowExW_Original);

    return TRUE;
}
