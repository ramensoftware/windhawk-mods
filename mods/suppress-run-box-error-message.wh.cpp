// ==WindhawkMod==
// @id              suppress-run-box-error-message
// @name            Suppress Run Box Error Message
// @description     Prevents the error message from opening if invalid input is entered into the Run box
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Suppress Run Box Error Message
Prevents the error message from opening if invalid input is entered into the Run box

If Windows is unable to find the program/file/directory you enter in Run, then it shows the following error message:
![Screenshot](https://i.imgur.com/2M9szrY.png)

This mod suppresses this error message. Additionally, you can also choose to automatically close the Run box when an error occurs, which by default the Run box would just reopen.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- suppress: true
  $name: Suppress error message
  $description: Suppresses the error message in the Run box.
- autoclose: false
  $name: Autoclose the Run box
  $description: Automatically close the Run box when an error occurs, which by default the Run box would just reopen.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

bool g_flag = false;

struct {
    BOOL suppress;
    BOOL autoclose;
} settings;

typedef HRESULT (WINAPI *ShellExecCmdLineWithSite_t)(HINSTANCE, HWND, LPCWSTR, LPCWSTR, int, LPVOID, DWORD);
ShellExecCmdLineWithSite_t pOriginalShellExecCmdLineWithSite;
HRESULT WINAPI ShellExecCmdLineWithSite_hook(
    HINSTANCE hInstance,
    HWND hWnd,
    LPCWSTR pwszCommand,
    LPCWSTR pwszStartDir,
    int nShow,
    LPVOID pUnused,
    DWORD dwSeclFlags
) {
    HRESULT ret = pOriginalShellExecCmdLineWithSite(hInstance, hWnd, pwszCommand, pwszStartDir, nShow, pUnused, dwSeclFlags);

    if(g_flag && settings.autoclose) return S_OK; // return that ShellExecuteEx (called by ShellExecCmdLineWithSite) has succeeded (even if it has failed)
    return ret;
}

typedef int (__cdecl *IsSHExecuteErrorMessageBoxPresent_t)();
IsSHExecuteErrorMessageBoxPresent_t pOriginalIsSHExecuteErrorMessageBoxPresent;
int __cdecl IsSHExecuteErrorMessageBoxPresent_hook()
{
    if(g_flag && settings.suppress) return 0;
    return pOriginalIsSHExecuteErrorMessageBoxPresent();
}

typedef int (__cdecl *OKPushed_t)();
OKPushed_t pOriginalOKPushed;
int __cdecl OKPushed_hook()
{
    g_flag = true;
    int ret = pOriginalOKPushed();
    g_flag = false;

    return ret;
}

void LoadSettings()
{
    settings.suppress = Wh_GetIntSetting(L"suppress");
    settings.autoclose = Wh_GetIntSetting(L"autoclose");
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L">");

    LoadSettings();

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                L"ShellExecCmdLineWithSite"
            },
            &pOriginalShellExecCmdLineWithSite,
            ShellExecCmdLineWithSite_hook,
            false
        },
        {
            {
                L"private: int __cdecl CRunDlg::OKPushed(void)"
            },
            &pOriginalOKPushed,
            OKPushed_hook,
            false
        },
        {
            {
                L"IsSHExecuteErrorMessageBoxPresent"
            },
            &pOriginalIsSHExecuteErrorMessageBoxPresent,
            IsSHExecuteErrorMessageBoxPresent_hook,
            false
        },
    };

    if (!WindhawkUtils::HookSymbols(hShell32, hooks, ARRAYSIZE(hooks)))
    {
        Wh_Log(L"Failed to hook one or more functions");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
