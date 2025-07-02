// ==WindhawkMod==
// @id              legacy-shell-message-boxes
// @name            Legacy Shell Message Boxes
// @description     Makes ShellMessageBox use message boxes rather than task dialogs
// @version         1.0.1
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lshlwapi -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Legacy Shell Message Boxes
This mod makes message boxes opened by the `ShellMessageBox` API use regular message boxes like in
Windows XP and before instead of task dialogs, like they do in Windows Vista and later.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/legacy-shell-message-boxes-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/legacy-shell-message-boxes-after.png)
*/
// ==/WindhawkModReadme==

#include <shlwapi.h>
#include <windhawk_utils.h>

thread_local bool g_fInSMB = false;

LPWSTR (__fastcall *ConstructMessageStringW)(HINSTANCE hInst, LPCWSTR pszMsg, va_list *ArgList);
LPSTR (__fastcall *ConstructMessageStringA)(HINSTANCE hInst, LPCSTR pszMsg, va_list *ArgList);

HRESULT (WINAPI *TaskDialogIndirect_orig)(const TASKDIALOGCONFIG *, int *, int *, BOOL *);
HRESULT WINAPI TaskDialogIndirect_hook(
    const TASKDIALOGCONFIG *pTaskConfig,
    int                    *pnButton,
    int                    *pnRadioButton,
    BOOL                   *pfVerificationFlagChecked
)
{
    if (g_fInSMB)
    {
        g_fInSMB = false;
        return E_FAIL;
    }
    return TaskDialogIndirect_orig(pTaskConfig, pnButton, pnRadioButton, pfVerificationFlagChecked);
}

/* For both of these hooks, since they use functions with variadic args,
   we need to construct the message ourself. */

using ShellMessageBoxW_t = decltype(&ShellMessageBoxW);
ShellMessageBoxW_t ShellMessageBoxW_orig;
int WINAPI ShellMessageBoxW_hook(
    HINSTANCE hAppInst,
    HWND      hWnd,
    LPCWSTR   lpcText,
    LPCWSTR   lpcTitle,
    UINT      fuStyle,
    ...
)
{
    va_list args;
    va_start(args, fuStyle);
    LPWSTR pszMsg = ConstructMessageStringW(hAppInst, lpcText, &args);
    int nRet = 0;
    if (pszMsg)
    {
        g_fInSMB = true;
        nRet = ShellMessageBoxW_orig(hAppInst, hWnd, L"%1", lpcTitle, fuStyle, pszMsg);
        LocalFree(pszMsg);
    }
    else
    {
        SetLastError(ERROR_OUTOFMEMORY);
    }
    g_fInSMB = false;
    va_end(args);
    return nRet;
}

using ShellMessageBoxA_t = decltype(&ShellMessageBoxA);
ShellMessageBoxA_t ShellMessageBoxA_orig;
int WINAPI ShellMessageBoxA_hook(
    HINSTANCE hAppInst,
    HWND      hWnd,
    LPCSTR    lpcText,
    LPCSTR    lpcTitle,
    UINT      fuStyle,
    ...
)
{
    va_list args;
    va_start(args, fuStyle);
    LPSTR pszMsg = ConstructMessageStringA(hAppInst, lpcText, &args);
    int nRet = 0;
    if (pszMsg)
    {
        g_fInSMB = true;
        nRet = ShellMessageBoxA_orig(hAppInst, hWnd, "%1", lpcTitle, fuStyle, pszMsg);
        LocalFree(pszMsg);
    }
    else
    {
        SetLastError(ERROR_OUTOFMEMORY);
    }
    g_fInSMB = false;
    va_end(args);
    return nRet;
}

const WindhawkUtils::SYMBOL_HOOK shlwapiDllHooks[] = {
    {
        {
#ifdef _WIN64
            L"ConstructMessageStringW"
#else
            L"_ConstructMessageStringW@12"
#endif
        },
        &ConstructMessageStringW,
        nullptr,
        false
    },
    {
        {
#ifdef _WIN64
            L"ConstructMessageStringA"
#else
            L"_ConstructMessageStringA@12"
#endif
        },
        &ConstructMessageStringA,
        nullptr,
        false
    }
};

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen) 
{ 
    void *pFixedFileInfo = nullptr; 
    UINT uPtrLen = 0; 

    HRSRC hResource = 
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION); 
    if (hResource)
    { 
        HGLOBAL hGlobal = LoadResource(hModule, hResource); 
        if (hGlobal)
        { 
            void *pData = LockResource(hGlobal); 
            if (pData)
            { 
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen)
                || uPtrLen == 0)
                { 
                    pFixedFileInfo = nullptr; 
                    uPtrLen = 0; 
                } 
            } 
        } 
    } 

    if (puPtrLen)
    { 
        *puPtrLen = uPtrLen; 
    } 
  
     return (VS_FIXEDFILEINFO *)pFixedFileInfo; 
 } 

/**
  * Loads comctl32.dll, version 6.0.
  * This uses an activation context that uses shell32.dll's manifest
  * to load 6.0, even in apps which don't have the proper manifest for
  * it.
  */
HMODULE LoadComCtlModule(void)
{
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    ACTCTXW actCtx = { sizeof(actCtx) };
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    actCtx.hModule = hShell32;
    HANDLE hActCtx = CreateActCtxW(&actCtx);
    ULONG_PTR ulCookie;
    ActivateActCtx(hActCtx, &ulCookie);
    HMODULE hComCtl = LoadLibraryExW(L"comctl32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    /**
      * Certain processes will ignore the activation context and load
      * comctl32.dll 5.82 anyway. If that occurs, just reject it.
      */
    VS_FIXEDFILEINFO *pVerInfo = GetModuleVersionInfo(hComCtl, nullptr);
    if (!pVerInfo || HIWORD(pVerInfo->dwFileVersionMS) < 6)
    {
        FreeLibrary(hComCtl);
        hComCtl = NULL;
    }
    DeactivateActCtx(0, ulCookie);
    ReleaseActCtx(hActCtx);
    FreeLibrary(hShell32);
    return hComCtl;
}

#define HOOK(func)                        \
    if (!Wh_SetFunctionHook(              \
        (void *)func,                     \
        (void *)func ## _hook,            \
        (void **)&func ## _orig           \
    ))                                    \
    {                                     \
        Wh_Log(L"Failed to hook " #func); \
        return FALSE;                     \
    }

BOOL Wh_ModInit(void)
{
    HMODULE hShlwapi = LoadLibraryW(L"shlwapi.dll");
    if (!hShlwapi)
    {
        Wh_Log(L"Failed to load shlwapi.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hShlwapi,
        shlwapiDllHooks,
        ARRAYSIZE(shlwapiDllHooks)
    ))
    {
        Wh_Log(L"Failed to find ConstructMessageString(W|A) in shlwapi.dll");
        return FALSE;
    }

    HMODULE hComCtl = LoadComCtlModule();
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    void *TaskDialogIndirect = (void *)GetProcAddress(hComCtl, "TaskDialogIndirect");
    if (!TaskDialogIndirect)
    {
        Wh_Log(L"Failed to find TaskDialogIndirect in comctl32.dll");
        return FALSE;
    }

    HOOK(TaskDialogIndirect);
    HOOK(ShellMessageBoxW);
    HOOK(ShellMessageBoxA);
    return TRUE;
}