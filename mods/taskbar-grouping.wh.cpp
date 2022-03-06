// ==WindhawkMod==
// @id              taskbar-grouping
// @name            Disable grouping on the taskbar
// @description     Causes a separate button to be created on the taskbar for each new window
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable grouping on the taskbar

Causes a separate button to be created on the taskbar for each new window.
For example, notice the two separate buttons for Notepad on the screenshot:

![Demonstration](https://i.imgur.com/uLITliK.png)

## Supported Windows versions

Windows 10 64-bit and Windows 11.

## Limitations

This is an early implementation which has several limitations:
* Pinned items are separated as well.
* The jump list menu might be missing items such as recent files.
* The mod has no effect on UWP/Store apps.

For a more complete solution for Windows 7, 8 and 10,
check out [7+ Taskbar Tweaker](https://rammichael.com/7-taskbar-tweaker).
*/
// ==/WindhawkModReadme==

typedef HRESULT (WINAPI *GetAppIDForWindow_t)(
    LPVOID pThis,
    _In_ HWND hwnd,
    _Outptr_ PWSTR *ppszAppID,
    _Out_opt_ BOOL *pfPinningPrevented,
    _Out_opt_ BOOL *pfExplicitAppID,
    _Out_opt_ BOOL *pfEmbeddedShortcutValid
);
typedef HRESULT (WINAPI *GetShortcutForProcess_t)(
    LPVOID pThis,
    _In_ ULONG dwProcessID,
    _Outptr_ /*IShellItem*/VOID **ppsi
);
typedef HRESULT (WINAPI *GetBestShortcutForAppID_t)(
    LPVOID pThis,
    _In_ PCWSTR pszAppID,
    _Outptr_ /*IShellItem*/VOID **ppsi
);

GetAppIDForWindow_t pOriginalGetAppIDForWindow;
GetShortcutForProcess_t pOriginalGetShortcutForProcess;
GetBestShortcutForAppID_t pOriginalGetBestShortcutForAppID;

HRESULT WINAPI GetAppIDForWindowHook(
    LPVOID pThis,
    _In_ HWND hwnd,
    _Outptr_ PWSTR *ppszAppID,
    _Out_opt_ BOOL *pfPinningPrevented,
    _Out_opt_ BOOL *pfExplicitAppID,
    _Out_opt_ BOOL *pfEmbeddedShortcutValid
)
{
    Wh_Log(L"GetAppIDForWindowHook");

    HRESULT ret = pOriginalGetAppIDForWindow(
        pThis,
        hwnd,
        ppszAppID,
        pfPinningPrevented,
        pfExplicitAppID,
        pfEmbeddedShortcutValid
    );

    if (SUCCEEDED(ret)) {
        size_t len = wcslen(*ppszAppID);
        size_t newLen = len + 9;
        if (newLen < MAX_PATH) {
            PWSTR realloc = (PWSTR)CoTaskMemRealloc(*ppszAppID, (newLen + 1) * sizeof(WCHAR));
            if (realloc) {
                wsprintf(realloc + len, L"_%08X", hwnd);
                Wh_Log(L"New AppId: %s", realloc);
                *ppszAppID = realloc;
            }
            else {
                CoTaskMemFree(*ppszAppID);
                ret = E_FAIL;
            }
        }
    }

    return ret;
}

HRESULT WINAPI GetShortcutForProcessHook(
    LPVOID pThis,
    _In_ ULONG dwProcessID,
    _Outptr_ /*IShellItem*/VOID **ppsi
)
{
    Wh_Log(L"GetShortcutForProcessHook");

    return E_FAIL;
}

HRESULT WINAPI GetBestShortcutForAppIDHook(
    LPVOID pThis,
    _In_ PCWSTR pszAppID,
    _Outptr_ /*IShellItem*/VOID **ppsi
)
{
    Wh_Log(L"GetBestShortcutForAppIDHook");

    return E_FAIL;
}

struct SYMBOLHOOKS {
    PCWSTR symbolName;
    void* hookFunction;
    void** pOriginalFunction;
};

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    WH_FIND_SYMBOL symbol;
    HANDLE find_symbol;

    SYMBOLHOOKS taskbarHooks[] = {
        {
            L"public: virtual long __cdecl CAppResolver::GetAppIDForWindow(struct HWND__ * __ptr64,unsigned short * __ptr64 * __ptr64,int * __ptr64,int * __ptr64,int * __ptr64) __ptr64",
            (void*)GetAppIDForWindowHook,
            (void**)&pOriginalGetAppIDForWindow
        },
        {
            L"public: virtual long __cdecl CAppResolver::GetShortcutForProcess(unsigned long,struct IShellItem * __ptr64 * __ptr64) __ptr64",
            (void*)GetShortcutForProcessHook,
            (void**)&pOriginalGetShortcutForProcess
        },
        {
            L"public: virtual long __cdecl CAppResolver::GetBestShortcutForAppID(unsigned short const * __ptr64,struct IShellItem * __ptr64 * __ptr64) __ptr64",
            (void*)GetBestShortcutForAppIDHook,
            (void**)&pOriginalGetBestShortcutForAppID
        }
    };

    HMODULE module = LoadLibrary(L"appresolver.dll");
    if (!module) {
        return FALSE;
    }

    find_symbol = Wh_FindFirstSymbol(module, NULL, &symbol);
    if (find_symbol) {
        do {
            for (size_t i = 0; i < ARRAYSIZE(taskbarHooks); i++) {
                if (!*taskbarHooks[i].pOriginalFunction && wcscmp(symbol.symbol, taskbarHooks[i].symbolName) == 0) {
                    if (taskbarHooks[i].hookFunction) {
                        Wh_SetFunctionHook(symbol.address, taskbarHooks[i].hookFunction, taskbarHooks[i].pOriginalFunction);
                        Wh_Log(L"Hooked %p (%s)", symbol.address, taskbarHooks[i].symbolName);
                    }
                    else {
                        *taskbarHooks[i].pOriginalFunction = symbol.address;
                        Wh_Log(L"Found %p (%s)", symbol.address, taskbarHooks[i].symbolName);
                    }
                    break;
                }
            }
        } while (Wh_FindNextSymbol(find_symbol, &symbol));

        Wh_FindCloseSymbol(find_symbol);
    }

    for (size_t i = 0; i < ARRAYSIZE(taskbarHooks); i++) {
        if (!*taskbarHooks[i].pOriginalFunction) {
            return FALSE;
        }
    }

    return TRUE;
}
