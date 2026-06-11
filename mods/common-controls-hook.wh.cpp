// ==WindhawkMod==
// @id              common-controls-hook
// @name            Common Controls Hook
// @description     Force-enable Common Controls v6 visual styles for legacy Win32 applications. Perfectly ported from LucidLabs/comctl32v6hook.
// @version         1.0.0
// @author          LucidLabs
// @github          https://github.com/LucidLabs
// @include         *
// @exclude         conhost.exe
// @exclude         cmd.exe
// @exclude         powershell.exe
// @exclude         explorer.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Common Controls Hook
Force-enable Common Controls v6 visual styles for legacy Win32 applications. Perfectly ported from LucidLabs/comctl32v6hook.

## Introduction
Common Controls Hook is a Windhawk mod designed to force-enable Common Controls v6 visual styles for legacy Win32 applications. By hooking relevant User32 APIs, it ensures that applications correctly load v6 styles when creating windows and dialog boxes, thereby improving interface aesthetics and consistency.

## Features
- Hooks functions such as `CreateWindowEx`, `DialogBoxParam`, `CreateDialogParam`, and `MessageBox` in User32 to ensure the v6 activation context is enabled upon invocation.
- Dynamically creates activation contexts to avoid modifying system files, maintaining a clean and stable system environment.

## Notes
- This mod functions by hooking User32 APIs and may conflict with certain security software; please ensure you trust the mod before enabling it.
- This mod targets applications using legacy Common Controls; it has no effect on applications that already utilize v6 styles.
- If you encounter any issues or have suggestions for improvement, please visit the GitHub repository to submit an issue or pull request.
- Special thanks to LucidLabs for the original project `comctl32v6hook`, which provided valuable references and implementation concepts.

## Changelog
- **1.0.0**: Initial release.

## Contact
- **GitHub**: [LucidLabs](https://github.com/LucidLabs)

## Support
If you need help or want to report a bug, please send a message in the official Windhawk Discord server or create an issue in the Windhawk Mods GitHub repository.

## Credits

| User | Contribution |
| --- | --- |
| **LucidLabs** | Original implementation of the Common Controls v6 hook, which served as the basis for this Windhawk mod. Their work on `comctl32v6hook` provided essential insights and code references that enabled the successful porting of this functionality to Windhawk. |
| **Windhawk Team** | For creating and maintaining the Windhawk modding framework, which allows for easy creation and distribution of mods like this one. |
| **Community Contributors** | For testing, feedback, and suggestions that helped improve the mod. |

*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <commctrl.h>

HANDLE g_hActCtx = INVALID_HANDLE_VALUE;

bool InitActCtx() {
    WCHAR tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    
    WCHAR manifestPath[MAX_PATH];
    lstrcpyW(manifestPath, tempPath);
    WCHAR pidStr[32];
    wsprintfW(pidStr, L"wh_v6_%u.manifest", GetCurrentProcessId());
    lstrcatW(manifestPath, pidStr);

    HANDLE hFile = CreateFileW(manifestPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        const char* manifestContent = 
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
            "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">\n"
            "  <dependency>\n"
            "    <dependentAssembly>\n"
            "      <assemblyIdentity type=\"win32\" name=\"Microsoft.Windows.Common-Controls\" version=\"6.0.0.0\" processorArchitecture=\"*\" publicKeyToken=\"6595b64144ccf1df\" language=\"*\" />\n"
            "    </dependentAssembly>\n"
            "  </dependency>\n"
            "</assembly>\n";
        DWORD written;
        WriteFile(hFile, manifestContent, lstrlenA(manifestContent), &written, NULL);
        CloseHandle(hFile);
    }

    ACTCTXW actCtx = { sizeof(ACTCTXW) };
    actCtx.lpSource = manifestPath;
    g_hActCtx = CreateActCtxW(&actCtx);
    
    DeleteFileW(manifestPath);

    return g_hActCtx != INVALID_HANDLE_VALUE;
}
class ActCtxGuard {
    ULONG_PTR cookie;
    BOOL activated;
public:
    ActCtxGuard() {
        activated = FALSE;
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
    typedef RET_TYPE (WINAPI *NAME##_t) ARGS; \
    NAME##_t NAME##_orig; \
    RET_TYPE WINAPI NAME##_hook ARGS { \
        ActCtxGuard guard; \
        return NAME##_orig ARGS_PASS; \
    }

DEFINE_HOOK(HWND, CreateWindowExW, 
    (DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam),
    (dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam))
DEFINE_HOOK(HWND, CreateWindowExA, 
    (DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam),
    (dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam))

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

BOOL Wh_ModInit() {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) {
        return FALSE;
    }

    if (!InitActCtx()) {
        Wh_Log(L"Failed to create activation context");
        return FALSE;
    }

    #define INSTALL_HOOK(name) \
        do { \
            void* addr = (void*)GetProcAddress(hUser32, #name); \
            if (addr) Wh_SetFunctionHook(addr, (void*)name##_hook, (void**)&name##_orig); \
        } while(0)

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

    return TRUE;
}