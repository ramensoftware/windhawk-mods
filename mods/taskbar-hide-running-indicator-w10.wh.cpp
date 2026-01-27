// ==WindhawkMod==
// @id              taskbar-hide-running-indicator-w10
// @name            Hide running indicator for Windows 10
// @description     Hides taskbar running indicator for Windows 10
// @version         1.0.2
// @author          giedriuslt
// @github          https://github.com/giedriuslt
// @include         explorer.exe
// @compilerOptions -lgdi32 -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide taskbar running indicator for Windows 10
Lightweight mod which hides the running indicator on taskbar buttons. ExplorerPatcher's reimplemented taskbar is supported. For native Windows 11 taskbar, use taskbar styler to achieve same effect.

Before:

![Before](https://i.imgur.com/bbUWHuX.png)

![Before](https://i.imgur.com/gT1rgVv.png)

After:

![After](https://i.imgur.com/ZbuRRXu.png)

![After](https://i.imgur.com/NSpjnHc.png)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <psapi.h>

#include <atomic>

#ifdef _WIN64
#define CALCON __cdecl
#define SCALCON L"__cdecl"
#else
#define CALCON __thiscall
#define SCALCON L"__thiscall"
#endif

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

typedef struct tagBUTTONRENDERINFO {
    unsigned char data[0x94]; // Unknown data
    RECT r1; // Size of the main running indicator rectangle 
    RECT r2; // Size of the additional running indicators
    RECT r3;
} BUTTONRENDERINFO, *PBUTTONRENDERINFO;

/* Compute sizes of running indicator rectangles */
typedef void (* CALCON CTaskBtnGroup__ComputeRenderPropsBar_t)(void *, PBUTTONRENDERINFO);
CTaskBtnGroup__ComputeRenderPropsBar_t CTaskBtnGroup__ComputeRenderPropsBar_orig;
void CALCON CTaskBtnGroup__ComputeRenderPropsBar_hook(
    void *pThis,
    PBUTTONRENDERINFO pRenderInfo
)
{
    PBUTTONRENDERINFO p = pRenderInfo;
    CTaskBtnGroup__ComputeRenderPropsBar_orig(pThis,  pRenderInfo);

    // Set rectanges to be zero width/height as appropriate.
    // Empty rectangles do not work, because then focus indicator is not drawn

    // Deduce where taskbar is based on indicator size ratio,
    // If it's longer than taller that means it's on top or botton, else it's on the side
    if ( (p->r1.bottom - p->r1.top) < (p->r1.right - p->r1.left) ) //top or bottom
    {
        if (p->r1.top == 0)
        {
            // Taskbar on the top
            p->r1.bottom = p->r1.top;
            p->r2.bottom = p->r2.top;
            p->r3.bottom = p->r3.top;
        }
        else
        {
            // Taskbar on the bottom
            p->r1.top = p->r1.bottom;
            p->r2.top = p->r2.bottom;
            p->r3.top = p->r3.bottom;
        }
    }
    else //side
    {
        if (p->r1.left == 0)
        {
            // Taskbar on the left
            p->r1.right = p->r1.left;
            p->r2.right = p->r2.left;
            p->r3.right = p->r3.left;
        }
        else
        {
            // Taskbar on the right
            p->r1.left = p->r1.right;
            p->r2.left = p->r2.right;
            p->r3.left = p->r3.right;
        }

    }
    return;
}

bool HookWin10TaskbarSymbols()
{
    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {
                L"private: void " 
                SCALCON 
                L" CTaskBtnGroup::_ComputeRenderPropsBar(struct BUTTONRENDERINFO &)"
            },
            (void **)&CTaskBtnGroup__ComputeRenderPropsBar_orig,
            (void *)CTaskBtnGroup__ComputeRenderPropsBar_hook,
            FALSE
        }
    };

    return WindhawkUtils::HookSymbols(GetModuleHandle(nullptr), explorerExeHooks, ARRAYSIZE(explorerExeHooks));
}

void RefreshTaskListWnd()
{
    HWND hwndTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hwndTray)
    {
        HWND hwndRebar = FindWindowExW(hwndTray, nullptr, L"RebarWindow32", nullptr);
        if (hwndRebar)
        {
            HWND hwndTaskBand = FindWindowExW(hwndRebar, nullptr, L"MSTaskSwWClass", nullptr);
            if (hwndTaskBand)
            {
                HWND hwndTaskListWnd = FindWindowExW(hwndTaskBand, nullptr, L"MSTaskListWClass", nullptr);
                if (hwndTaskListWnd)
                {
                    InvalidateRect(hwndTaskListWnd, nullptr, false);
                }
            }
        }
    }
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen)
{
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource)
    {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal)
        {
            void* pData = LockResource(hGlobal);
            if (pData)
            {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) || uPtrLen == 0)
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

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion()
{
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo)
    {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major)
    {
        case 10:
            if (build < 22000)
            {
                return WinVersion::Win10;
            }
            else if (build < 26100)
            {
                return WinVersion::Win11;
            }
            else
            {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional)
          {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule)
{
    if (g_explorerPatcherInitialized.exchange(true))
    {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11)
    {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?_ComputeRenderPropsBar@CTaskBtnGroup@@AEAAXAEAUBUTTONRENDERINFO@@@Z)",
         &CTaskBtnGroup__ComputeRenderPropsBar_orig, CTaskBtnGroup__ComputeRenderPropsBar_hook},
    };

    bool succeeded = true;

    for (const auto& hook : hooks)
    {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr)
        {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional)
            {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction)
        {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        }
        else
        {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded)
    {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    }
    else if (g_initialized)
    {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module)
{
    WCHAR moduleFilePath[MAX_PATH];
    switch (GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath)))
    {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName)
    {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) == 0)
    {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher()
{
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded))
    {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++)
        {
            if (IsExplorerPatcherModule(hMods[i]))
            {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized)
    {
        if (IsExplorerPatcherModule(module))
        {
            HookExplorerPatcherSymbols(module);
        }
    }

    return module;
}

BOOL Wh_ModInit(void)
{
    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported)
    {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

    if (g_winVersion >= WinVersion::Win11)
    {
        g_winVersion = WinVersion::Win10;
    }

    if (hasWin10Taskbar && !HookWin10TaskbarSymbols())
    {
        return FALSE;
    }

    if (!HandleLoadedExplorerPatcher())
    {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    Wh_SetFunctionHook((void*)pKernelBaseLoadLibraryExW,
                       (void*)LoadLibraryExW_Hook,
                       (void**)&LoadLibraryExW_Original);

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit()
{
    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized)
    {
        HandleLoadedExplorerPatcher();
    }

    RefreshTaskListWnd();
}

void Wh_ModUninit()
{
    RefreshTaskListWnd();
}
