// ==WindhawkMod==
// @id              perform-speedtest-custom-command
// @name            Taskbar Speed Test - Custom Command Runner
// @description     Intercepts the taskbar "Perform speed test" click and runs any custom system command or executable path specified in the mod settings.
// @version         1.4.1
// @author          Kiegro
// @github          https://github.com/creeperguy5421
// @license         MIT
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         RuntimeBroker.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -lkernel32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*...*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <string>

#define LINK_ID L"linkid=2324916"
#define LOG(fmt, ...) Wh_Log(L"[speedtest_cmd] " fmt, ##__VA_ARGS__)

static bool HasLinkId(LPCWSTR s) {
    return s && wcsstr(s, LINK_ID);
}

// Independent Execution Thread to handle the custom command
static void RunCustomCommand() {
    PCWSTR rawCmdSetting = Wh_GetStringSetting(L"customCommand");
    if (!rawCmdSetting) {
        LOG(L"Error: Custom command text box is empty.");
        return;
    }

    std::wstring commandStr(rawCmdSetting);
    Wh_FreeStringSetting(rawCmdSetting);

    LOG(L"Executing user command: %s", commandStr.c_str());

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL; //Tells Windows to run the window normally and bring it to the front

    wchar_t* cmdLine = new wchar_t[commandStr.length() + 1];
    wcscpy_s(cmdLine, commandStr.length() + 1, commandStr.c_str());

    // CHANGED: Removed CREATE_NO_WINDOW so the execution runs visibly on top
    if (CreateProcessW(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        LOG(L"Failed to launch command. Error Code: %d", GetLastError());
    }

    delete[] cmdLine;
}

// ── ShellExecuteExW hook ─────────────────────────────────────────────────────

using ShellExecuteExW_t = decltype(&ShellExecuteExW);
ShellExecuteExW_t ShellExecuteExW_Original;

BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* pei) {
    if (pei && (HasLinkId(pei->lpFile) || HasLinkId(pei->lpParameters))) {
        RunCustomCommand();
        return TRUE; 
    }
    return ShellExecuteExW_Original(pei);
}

// ── ShellExecuteW hook ───────────────────────────────────────────────────────

using ShellExecuteW_t = decltype(&ShellExecuteW);
ShellExecuteW_t ShellExecuteW_Original;

HINSTANCE WINAPI ShellExecuteW_Hook(HWND hwnd, LPCWSTR op, LPCWSTR file,
                                    LPCWSTR params, LPCWSTR dir, INT show) {
    if (HasLinkId(file) || HasLinkId(params)) {
        RunCustomCommand();
        return (HINSTANCE)32; 
    }
    return ShellExecuteW_Original(hwnd, op, file, params, dir, show);
}

// ── CreateProcessW hook ──────────────────────────────────────────────────────

using CreateProcessW_t = decltype(&CreateProcessW);
CreateProcessW_t CreateProcessW_Original;

BOOL WINAPI CreateProcessW_Hook(LPCWSTR app, LPWSTR cmd,
                                LPSECURITY_ATTRIBUTES psa, LPSECURITY_ATTRIBUTES tsa,
                                BOOL inherit, DWORD flags, LPVOID env,
                                LPCWSTR dir, LPSTARTUPINFOW si, LPPROCESS_INFORMATION pi) {
    if (HasLinkId(cmd)) {
        RunCustomCommand();
        return TRUE; 
    }
    return CreateProcessW_Original(app, cmd, psa, tsa, inherit, flags, env, dir, si, pi);
}

// ── Init ─────────────────────────────────────────────────────────────────────

static bool HookFn(void* target, void* hook, void** orig, const wchar_t* name) {
    if (!target) return false;
    return Wh_SetFunctionHook(target, hook, orig);
}

BOOL Wh_ModInit() {
    HMODULE hShell32  = GetModuleHandleW(L"shell32.dll");
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");

    bool ok = true;
    ok &= HookFn(hShell32  ? (void*)GetProcAddress(hShell32,  "ShellExecuteExW") : nullptr, (void*)ShellExecuteExW_Hook, (void**)&ShellExecuteExW_Original, L"ShellExecuteExW");
    ok &= HookFn(hShell32  ? (void*)GetProcAddress(hShell32,  "ShellExecuteW")   : nullptr, (void*)ShellExecuteW_Hook,   (void**)&ShellExecuteW_Original, L"ShellExecuteW");
    ok &= HookFn(hKernel32 ? (void*)GetProcAddress(hKernel32, "CreateProcessW")  : nullptr, (void*)CreateProcessW_Hook,  (void**)&CreateProcessW_Original, L"CreateProcessW");

    return ok ? TRUE : FALSE;
}
