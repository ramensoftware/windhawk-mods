// ==WindhawkMod==
// @id              common-controls-hook
// @name            Common Controls Hook
// @description     Force-enable Common Controls v6 visual styles for legacy Win32 applications
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Common Controls Hook

This mod forces legacy GUI processes into the Common Controls version 6
activation context at the moment they create windows, dialogs, and message
boxes.

The practical goal is simple: make old Win32 UI surfaces, including VBScript
`MsgBox` and classic dialog-based applications, use themed visual styles instead
of the pre-XP `comctl32.dll` version 5 look.

![Screenshot](https://i.imgur.com/4oVsYi7.png)

## Credits

This mod is a Windhawk port of
[comctl32v6hook](https://github.com/LuicdLabs/comctl32v6hook), a standalone
user-mode DLL by LuicdLabs.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

HANDLE g_hActCtx = INVALID_HANDLE_VALUE;

bool InitActCtx() {
    // shell32.dll ships the Common Controls v6 manifest as resource 124, and
    // it's already loaded in GUI processes, so we can build the activation
    // context straight from it without writing a temporary manifest file to
    // disk.
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        return false;
    }

    ACTCTXW actCtx = {sizeof(actCtx)};
    actCtx.dwFlags =
        ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    actCtx.hModule = hShell32;
    g_hActCtx = CreateActCtxW(&actCtx);

    return g_hActCtx != INVALID_HANDLE_VALUE;
}

class ActCtxGuard {
    ULONG_PTR cookie = 0;
    BOOL activated = FALSE;

    ActCtxGuard(const ActCtxGuard&) = delete;
    ActCtxGuard& operator=(const ActCtxGuard&) = delete;
    ActCtxGuard(ActCtxGuard&&) = delete;
    ActCtxGuard& operator=(ActCtxGuard&&) = delete;

   public:
    ActCtxGuard() {
        if (g_hActCtx != INVALID_HANDLE_VALUE) {
            activated = ActivateActCtx(g_hActCtx, &cookie);
        }
    }
    ~ActCtxGuard() {
        if (activated) {
            DeactivateActCtx(0, cookie);
        }
    }
};

#define DEFINE_HOOK(RET_TYPE, NAME, ARGS, ARGS_PASS) \
    typedef RET_TYPE(WINAPI* NAME##_t) ARGS;         \
    NAME##_t NAME##_orig;                            \
    RET_TYPE WINAPI NAME##_hook ARGS {               \
        ActCtxGuard guard;                           \
        return NAME##_orig ARGS_PASS;                \
    }

// clang-format off

// CreateWindowEx is not defined through DEFINE_HOOK because the activation
// context is only applied to top-level windows. Child windows inherit their
// parent's activation context, so activating ours for them is unnecessary, and
// it also avoids churning the activation stack for the many child controls a
// window creates.
typedef HWND(WINAPI* CreateWindowExW_t)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
CreateWindowExW_t CreateWindowExW_orig;
HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    if (dwStyle & WS_CHILD) {
        return CreateWindowExW_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }

    ActCtxGuard guard;
    return CreateWindowExW_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

typedef HWND(WINAPI* CreateWindowExA_t)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
CreateWindowExA_t CreateWindowExA_orig;
HWND WINAPI CreateWindowExA_hook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    if (dwStyle & WS_CHILD) {
        return CreateWindowExA_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }

    ActCtxGuard guard;
    return CreateWindowExA_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

DEFINE_HOOK(INT_PTR, DialogBoxParamW, 
    (HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam))
DEFINE_HOOK(INT_PTR, DialogBoxParamA, 
    (HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam))

DEFINE_HOOK(INT_PTR, DialogBoxIndirectParamW, 
    (HINSTANCE hInstance, LPCDLGTEMPLATEW hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam))
DEFINE_HOOK(INT_PTR, DialogBoxIndirectParamA, 
    (HINSTANCE hInstance, LPCDLGTEMPLATEA hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam))

DEFINE_HOOK(HWND, CreateDialogParamW, 
    (HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam))
DEFINE_HOOK(HWND, CreateDialogParamA, 
    (HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam))

DEFINE_HOOK(HWND, CreateDialogIndirectParamW, 
    (HINSTANCE hInstance, LPCDLGTEMPLATEW hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam))
DEFINE_HOOK(HWND, CreateDialogIndirectParamA, 
    (HINSTANCE hInstance, LPCDLGTEMPLATEA hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam))

DEFINE_HOOK(int, MessageBoxW, 
    (HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType),
    (hWnd, lpText, lpCaption, uType))
DEFINE_HOOK(int, MessageBoxA, 
    (HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType),
    (hWnd, lpText, lpCaption, uType))

DEFINE_HOOK(int, MessageBoxExW, 
    (HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId),
    (hWnd, lpText, lpCaption, uType, wLanguageId))
DEFINE_HOOK(int, MessageBoxExA, 
    (HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, WORD wLanguageId),
    (hWnd, lpText, lpCaption, uType, wLanguageId))

DEFINE_HOOK(int, MessageBoxIndirectW, 
    (const MSGBOXPARAMSW* lpmbp), (lpmbp))
DEFINE_HOOK(int, MessageBoxIndirectA, 
    (const MSGBOXPARAMSA* lpmbp), (lpmbp))

DEFINE_HOOK(int, MessageBoxTimeoutW, 
    (HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds),
    (hWnd, lpText, lpCaption, uType, wLanguageId, dwMilliseconds))
DEFINE_HOOK(int, MessageBoxTimeoutA, 
    (HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds),
    (hWnd, lpText, lpCaption, uType, wLanguageId, dwMilliseconds))

// clang-format on

BOOL Wh_ModInit() {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) {
        return FALSE;
    }

    if (!InitActCtx()) {
        Wh_Log(L"Failed to create activation context");
        return FALSE;
    }

#define INSTALL_HOOK(name)                                         \
    do {                                                           \
        auto addr = (name##_t)GetProcAddress(hUser32, #name);      \
        if (addr) {                                                \
            if (!WindhawkUtils::SetFunctionHook(addr, name##_hook, \
                                                &name##_orig)) {   \
                Wh_Log(L"Failed to set hook for " TEXT(#name));    \
            }                                                      \
        }                                                          \
    } while (0)

    INSTALL_HOOK(CreateWindowExW);
    INSTALL_HOOK(CreateWindowExA);
    INSTALL_HOOK(DialogBoxParamW);
    INSTALL_HOOK(DialogBoxParamA);
    INSTALL_HOOK(DialogBoxIndirectParamW);
    INSTALL_HOOK(DialogBoxIndirectParamA);
    INSTALL_HOOK(CreateDialogParamW);
    INSTALL_HOOK(CreateDialogParamA);
    INSTALL_HOOK(CreateDialogIndirectParamW);
    INSTALL_HOOK(CreateDialogIndirectParamA);
    INSTALL_HOOK(MessageBoxW);
    INSTALL_HOOK(MessageBoxA);
    INSTALL_HOOK(MessageBoxExW);
    INSTALL_HOOK(MessageBoxExA);
    INSTALL_HOOK(MessageBoxIndirectW);
    INSTALL_HOOK(MessageBoxIndirectA);
    INSTALL_HOOK(MessageBoxTimeoutW);
    INSTALL_HOOK(MessageBoxTimeoutA);

#undef INSTALL_HOOK

    return TRUE;
}

void Wh_ModUninit() {
    if (g_hActCtx != INVALID_HANDLE_VALUE) {
        ReleaseActCtx(g_hActCtx);
    }
}
