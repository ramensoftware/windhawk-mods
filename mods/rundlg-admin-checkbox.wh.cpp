// ==WindhawkMod==
// @id              rundlg-admin-checkbox
// @name            Run Dialog Admin Checkbox
// @description     Puts the Create this task with administrative privileges checkbox on the Win+R Run dialog
// @version         1.0.0
// @author          repensky
// @github          https://github.com/repensky
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Run Dialog Admin Checkbox

Task Manager's Create new task dialog has a checkbox to run the command as administrator. Win+R does not, even though both dialogs are the same shell32 code. This mod puts that checkbox on Win+R.
The mod only adds the checkbox, it does not affect Ctrl+Shift+Enter. To enable the shortcut, please install the **Always Allow CTRL+SHIFT+ENTER** mod by aubymori.

Tested and works on 10 21H2 and 11 24H2.

##Notes:

* Ticking the box does what Ctrl+Shift+Enter does, and the shortcut keeps working
  as before. Like the shortcut it only elevates commands that can be run as
  administrator, so folders, URLs and most documents open as usual.
* When explorer itself runs elevated, Windows shows its own This task will be
  created with administrative privileges notice, and the mod leaves it alone.

![Picture](https://raw.githubusercontent.com/repensky/local-wh-mods/refs/heads/main/explorer_1789508130.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <shlwapi.h>

//---Dialog ids---------------------------------------------

// The plain Run template, and the variant that carries the checkbox
#define DLG_RUN          1003
#define DLG_RUN_ADMIN    1011

#define IDC_RUN_COMBO    12298
#define IDC_RUN_ADMIN    12307

//---Mod state----------------------------------------------

// Set only while this thread is inside RunFileDlg
static thread_local bool g_fInRunFileDlg = false;

static HMODULE g_hShell32 = NULL;
static HMODULE g_hStorage = NULL;

//---Template swap------------------------------------------

using FindResourceExW_t = decltype(&FindResourceExW);
static FindResourceExW_t FindResourceExW_Original;

HRSRC WINAPI FindResourceExW_Hook(HMODULE hModule, LPCWSTR lpType,
                                  LPCWSTR lpName, WORD wLanguage)
{
    // shell32 asks for the plain template when the caller is not elevated
    // Handing back the admin one is all it takes to get the checkbox
    if (g_fInRunFileDlg && hModule == g_hShell32 && lpType == RT_DIALOG
        && lpName == MAKEINTRESOURCEW(DLG_RUN)) {
        Wh_Log(L"Swapping the Run template for the admin one");
        lpName = MAKEINTRESOURCEW(DLG_RUN_ADMIN);
    }

    return FindResourceExW_Original(hModule, lpType, lpName, wLanguage);
}

//---Run dialog hook----------------------------------------

typedef int (WINAPI *RunFileDlg_t)(HWND hwndOwner, HICON hIcon,
                                   LPCWSTR pszWorkingDir, LPCWSTR pszTitle,
                                   LPCWSTR pszPrompt, DWORD dwFlags);
static RunFileDlg_t RunFileDlg_Original;

int WINAPI RunFileDlg_Hook(HWND hwndOwner, HICON hIcon, LPCWSTR pszWorkingDir,
                           LPCWSTR pszTitle, LPCWSTR pszPrompt, DWORD dwFlags)
{
    Wh_Log(L"RunFileDlg flags 0x%X", dwFlags);

    // Flags are passed through, the template swap does not need any of them
    g_fInRunFileDlg = true;
    int nResult = RunFileDlg_Original(hwndOwner, hIcon, pszWorkingDir, pszTitle,
                                      pszPrompt, dwFlags);
    g_fInRunFileDlg = false;

    return nResult;
}

//---Elevation----------------------------------------------

// True for the Run dialog once it carries the checkbox
static bool IsRunDialog(HWND hWnd)
{
    WCHAR szClass[16];

    return GetClassNameW(hWnd, szClass, ARRAYSIZE(szClass))
        && lstrcmpW(szClass, L"#32770") == 0
        && GetDlgItem(hWnd, IDC_RUN_COMBO)
        && GetDlgItem(hWnd, IDC_RUN_ADMIN);
}

struct FIND_RUN_DIALOG
{
    HWND hwndLaunch;
    HWND hDlg;
};

BOOL CALLBACK FindRunDialogProc(HWND hWnd, LPARAM lParam)
{
    FIND_RUN_DIALOG *pFind = (FIND_RUN_DIALOG *)lParam;

    if ((hWnd != pFind->hwndLaunch
         && GetWindow(hWnd, GW_OWNER) != pFind->hwndLaunch)
        || !IsRunDialog(hWnd))
        return TRUE;

    pFind->hDlg = hWnd;
    return FALSE;
}

// The dialog launches on behalf of its owner, or of itself when it has none
// It is hidden by launch time, so it has to be found by walking the thread
static bool RunDialogWantsElevation(HWND hwndLaunch)
{
    if (!hwndLaunch)
        return false;

    FIND_RUN_DIALOG find = { hwndLaunch, NULL };
    EnumThreadWindows(GetCurrentThreadId(), FindRunDialogProc, (LPARAM)&find);

    return find.hDlg
        && IsDlgButtonChecked(find.hDlg, IDC_RUN_ADMIN) == BST_CHECKED;
}

// Decides whether the runas verb makes sense for what the Run dialog launches
// A bare name never matches a file association, so it is judged by extension
static bool CanRunAs(PCWSTR pszFile)
{
    if (!pszFile || !*pszFile)
        return false;

    // Folders and URLs have no runas verb, asking for one would only fail
    if (UrlIsW(pszFile, URLIS_URL) || PathIsDirectoryW(pszFile))
        return false;

    PCWSTR pszExt = PathFindExtensionW(pszFile);
    if (!*pszExt) {
        // A bare command name resolves through App Paths or PATH
        return PathFindFileNameW(pszFile) == pszFile;
    }

    // The query shell32 makes, given the extension rather than the whole name
    DWORD cchOut = 0;
    HRESULT hr = AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_COMMAND, pszExt,
                                   L"runas", NULL, &cchOut);

    return SUCCEEDED(hr) && cchOut != 0;
}

using ShellExecuteExW_t = decltype(&ShellExecuteExW);

// Build 19041 launches through shell32's copy, 26100 through windows.storage's
static ShellExecuteExW_t ShellExecuteExW_Shell32_Original;
static ShellExecuteExW_t ShellExecuteExW_Storage_Original;

// Shared by both copies, each passes in the original it wraps
static BOOL LaunchRunDlgCommand(SHELLEXECUTEINFOW *pExecInfo,
                                ShellExecuteExW_t pfnOriginal)
{
    // The dialog launches with no verb of its own
    // Once one is set, the nested call into the other copy passes through
    if (!g_fInRunFileDlg || !pExecInfo
        || (pExecInfo->lpVerb && *pExecInfo->lpVerb)
        || !RunDialogWantsElevation(pExecInfo->hwnd))
        return pfnOriginal(pExecInfo);

    if (!CanRunAs(pExecInfo->lpFile)) {
        Wh_Log(L"Not elevating %s, it has no runas verb",
               pExecInfo->lpFile ? pExecInfo->lpFile : L"(none)");
        return pfnOriginal(pExecInfo);
    }

    Wh_Log(L"Elevating %s", pExecInfo->lpFile);

    // A copy carries the verb, shell32 still owns the struct it passed in
    SHELLEXECUTEINFOW sei = *pExecInfo;
    sei.lpVerb = L"runas";

    BOOL fResult = pfnOriginal(&sei);
    pExecInfo->hInstApp = sei.hInstApp;
    pExecInfo->hProcess = sei.hProcess;
    return fResult;
}

BOOL WINAPI ShellExecuteExW_Shell32_Hook(SHELLEXECUTEINFOW *pExecInfo)
{
    return LaunchRunDlgCommand(pExecInfo, ShellExecuteExW_Shell32_Original);
}

BOOL WINAPI ShellExecuteExW_Storage_Hook(SHELLEXECUTEINFOW *pExecInfo)
{
    return LaunchRunDlgCommand(pExecInfo, ShellExecuteExW_Storage_Original);
}

//---Mod lifetime-------------------------------------------

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    g_hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!g_hShell32) {
        Wh_Log(L"shell32.dll is not loaded");
        return FALSE;
    }

    // shell32 exports the Run dialog by ordinal only, it has no name
    RunFileDlg_t pfnRunFileDlg =
        (RunFileDlg_t)GetProcAddress(g_hShell32, MAKEINTRESOURCEA(61));
    if (!pfnRunFileDlg) {
        Wh_Log(L"shell32 ordinal 61 is missing");
        return FALSE;
    }

    // FindResourceW is a jump into FindResourceExW, so this covers both
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    FindResourceExW_t pfnFindResourceExW = hKernelBase
        ? (FindResourceExW_t)GetProcAddress(hKernelBase, "FindResourceExW")
        : nullptr;
    if (!pfnFindResourceExW) {
        Wh_Log(L"FindResourceExW is missing");
        return FALSE;
    }

    // On 26100 shell32's export is only a jump into windows.storage
    ShellExecuteExW_t pfnShell32Exec =
        (ShellExecuteExW_t)GetProcAddress(g_hShell32, "ShellExecuteExW");

    // The copy the Run dialog really calls on 26100, from System32 only
    g_hStorage = LoadLibraryExW(L"windows.storage.dll", NULL,
                                LOAD_LIBRARY_SEARCH_SYSTEM32);
    ShellExecuteExW_t pfnStorageExec = g_hStorage
        ? (ShellExecuteExW_t)GetProcAddress(g_hStorage, "ShellExecuteExW")
        : nullptr;

    if (!pfnShell32Exec && !pfnStorageExec) {
        Wh_Log(L"ShellExecuteExW is missing");
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(pfnRunFileDlg, RunFileDlg_Hook,
                                   &RunFileDlg_Original);
    WindhawkUtils::SetFunctionHook(pfnFindResourceExW, FindResourceExW_Hook,
                                   &FindResourceExW_Original);

    if (pfnStorageExec)
        WindhawkUtils::SetFunctionHook(pfnStorageExec,
                                       ShellExecuteExW_Storage_Hook,
                                       &ShellExecuteExW_Storage_Original);

    if (pfnShell32Exec && pfnShell32Exec != pfnStorageExec)
        WindhawkUtils::SetFunctionHook(pfnShell32Exec,
                                       ShellExecuteExW_Shell32_Hook,
                                       &ShellExecuteExW_Shell32_Original);

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    if (g_hStorage) {
        FreeLibrary(g_hStorage);
        g_hStorage = NULL;
    }
}
