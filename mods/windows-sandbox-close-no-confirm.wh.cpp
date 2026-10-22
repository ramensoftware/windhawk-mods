// ==WindhawkMod==
// @id              windows-sandbox-close-no-confirm
// @name            Windows Sandbox: close without confirmation
// @name:zh-CN      沙盒关闭免确认
// @description     Stops the sandbox with "wsb stop" when its window is closed
// @description:zh-CN 关闭沙盒窗口时直接执行 wsb stop，不再弹出确认
// @version         1.1
// @author          loliri
// @github          https://github.com/loliri
// @include         WindowsSandboxRemoteSession.exe
// @architecture    x86-64
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
*/
// ==/WindhawkModReadme==

#include <string>

#include <commctrl.h>
#include <windhawk_utils.h>

namespace {

// Set once the sandbox has been stopped, so that the close which started the
// stop is the only one swallowed and later closes go through to the app.
bool g_stopped;

// Stops the sandbox this process belongs to. Returns whether the sandbox was
// stopped, so that the close is only swallowed when it actually was.
bool StopSandbox();

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

// Runs the command without a window and waits for it to finish. The executable
// is looked up in the PATH directories only: the command line carries no
// quoting, and letting CreateProcessW or SearchPathW fall back to the current
// directory would run whatever executable is there.
bool RunHidden(const std::wstring &executableName,
               const std::wstring &commandLine)
{
    DWORD pathSize = GetEnvironmentVariableW(L"PATH", nullptr, 0);
    std::wstring path(pathSize, L'\0');
    path.resize(GetEnvironmentVariableW(L"PATH", path.data(), pathSize));

    WCHAR szExecutablePath[MAX_PATH];
    DWORD len = SearchPathW(path.c_str(), executableName.c_str(), L".exe",
                            ARRAYSIZE(szExecutablePath), szExecutablePath,
                            nullptr);
    if (len == 0 || len >= ARRAYSIZE(szExecutablePath))
    {
        Wh_Log(L"Failed to find %s: %u", executableName.c_str(),
               GetLastError());
        return false;
    }

    std::wstring commandLineCopy = commandLine;

    STARTUPINFOW si{.cb = sizeof(si)};
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi{};

    if (!CreateProcessW(szExecutablePath, commandLineCopy.data(), nullptr,
                        nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si,
                        &pi))
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

bool StopSandbox()
{
    std::wstring commandLine = GetCommandLineW();
    Wh_Log(L"Command line: %s", commandLine.c_str());

    std::wstring guid;
    if (!FindGuid(commandLine, guid))
    {
        Wh_Log(L"No sandbox id on the command line");
        return false;
    }

    Wh_Log(L"Stopping sandbox %s", guid.c_str());

    if (!RunHidden(L"wsb", L"wsb.exe stop --id " + guid))
    {
        return false;
    }

    g_stopped = true;

    return true;
}

LRESULT CALLBACK SubclassProc(HWND hWnd,
                              UINT msg,
                              WPARAM wParam,
                              LPARAM lParam,
                              DWORD_PTR)
{
    // Only the close which started the stop is swallowed. Once the sandbox is
    // stopped, later closes are passed through, so a close the app sends to
    // itself after the session ends still closes the window.
    if (msg == WM_CLOSE && !g_stopped && StopSandbox())
    {
        return 0;
    }

    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

bool IsSandboxWindow(HWND hWnd)
{
    if (!hWnd || GetWindow(hWnd, GW_OWNER) ||
        (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CHILD))
    {
        return false;
    }

    WCHAR szClassName[64];
    return GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName)) &&
           wcscmp(szClassName, L"WinUIDesktopWin32WindowClass") == 0;
}

void SubclassSandboxWindow(HWND hWnd)
{
    if (IsSandboxWindow(hWnd) &&
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, 0))
    {
        Wh_Log(L"Subclassed %p", hWnd);
    }
}

BOOL CALLBACK EnumSandboxWindowsProc(HWND hWnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;
    if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
        dwProcessId == GetCurrentProcessId())
    {
        if (lParam)
        {
            SubclassSandboxWindow(hWnd);
        }
        else if (IsSandboxWindow(hWnd))
        {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                            SubclassProc);
        }
    }

    return TRUE;
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

    if (hWnd)
    {
        SubclassSandboxWindow(hWnd);
    }

    return hWnd;
}

BOOL Wh_ModInit()
{
    Wh_Log(L">");

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit()
{
    Wh_Log(L">");

    // Windows which existed before the mod was loaded.
    EnumWindows(EnumSandboxWindowsProc, TRUE);
}

void Wh_ModUninit()
{
    Wh_Log(L">");

    EnumWindows(EnumSandboxWindowsProc, FALSE);
}
