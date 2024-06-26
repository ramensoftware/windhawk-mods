// ==WindhawkMod==
// @id              basic-themer
// @name            Basic Themer
// @description     Applies the Windows Basic theme to desktop windows
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         dwm.exe
// @compilerOptions -ldwmapi -lwtsapi32
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Basic Themer
This mod applies non-DWM frames (Windows Basic theme) to desktop windows when DWM is
enabled.

# IMPORTANT: READ!
This mod needs to hook into `dwm.exe` to work. Please navigate to Windhawk's
Settings, Advanced settings, More advanced settings, and make sure that
`dwm.exe` is in the Process inclusion list.

## Application issues
Certain issues with applications (e.g. Ribbon applications) may arise when using this
mod. This is because these applications still believe that DWM is enabled despite using
basic frames. To circumvent this, use the "DWM Unextend Frames" mod.

## Features
* Blacklist, whitelist
* Apply to secure desktop (UAC, LogonUI) only
  * Useful for Windows 7 Aero theme
* Disable animations on affected windows

## Images

**Preview**:

![Preview](https://raw.githubusercontent.com/aubymori/images/main/basic-themer-preview.png)

**Secure Desktop only (with classic UAC and Windows 7 Style UAC Dim mod)**:

![Secure Desktop only](https://raw.githubusercontent.com/aubymori/images/main/basic-themer-secure-desktop.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- whitelistmode: false
  $name: Whitelist mode
  $description: Only include processes listed in the whitelist
- blacklist:
  - - process: ""
      $name: Process name/path
  $name: Blacklist
  $description: Processes to exclude
- whitelist:
  - - process: ""
      $name: Process name/path
  $name: Whitelist
  $description: Processes to include
- secureonly: false
  $name: Apply to Secure Desktop only
  $description: Only apply basic theme to windows on the Secure Desktop (UAC, LogonUI)
- noanims: false
  $name: Disable animations
  $description: Disable animations on every window that basic theme is applied to
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <dwmapi.h>
#include <wtsapi32.h>

struct
{
    bool whitelistmode;
    std::vector<std::wstring> blacklist;
    std::vector<std::wstring> whitelist;
    bool secureonly;
    bool noanims;
} settings;

void ApplyBasicThemeInternal(HWND hWnd, BOOL bEnable)
{
    DWMNCRENDERINGPOLICY ncrp = bEnable ? DWMNCRP_DISABLED : DWMNCRP_ENABLED;
    DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(DWMNCRENDERINGPOLICY));

    BOOL bNoAnims = bEnable && settings.noanims;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &bNoAnims, sizeof(BOOL));
}

void ApplyBasicTheme(HWND hWnd)
{
    /* Running this function on windows without WS_CAPTION
       (menus) will mess them up */
    DWORD dwStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if ((dwStyle & WS_CAPTION) != WS_CAPTION)
    {
        return;
    }

    DWORD pid = 0;
    DWORD threadId = GetWindowThreadProcessId(hWnd, &pid);

    if (settings.secureonly)
    {
        HDESK hDesk = GetThreadDesktop(threadId);
        if (hDesk)
        {
            WCHAR szDeskName[256] = { 0 };
            if (GetUserObjectInformationW(
                hDesk,
                UOI_NAME,
                &szDeskName,
                sizeof(szDeskName),
                nullptr
            ))
            {
                if (0 != wcsicmp(szDeskName, L"winlogon"))
                {
                    ApplyBasicThemeInternal(hWnd, FALSE);
                    return;
                }
            }
        }
    }

    if (settings.blacklist.empty() && !settings.whitelistmode)
    {
        ApplyBasicThemeInternal(hWnd, TRUE);
    }

    WTS_PROCESS_INFOW *processes = nullptr;
    DWORD processCount = 0;
    if (!WTSEnumerateProcessesW(WTS_CURRENT_SERVER_HANDLE, 0, 1, &processes, &processCount))
    {
        return;
    }

    for (DWORD i = 0; i < processCount; i++)
    {
        if (processes[i].ProcessId == pid)
        {
            for (auto path: settings.blacklist)
            {
                if (!path.find(processes[i].pProcessName))
                {
                    ApplyBasicThemeInternal(hWnd, FALSE);
                    WTSFreeMemory(processes);
                    return;
                }
            }

            for (auto path: settings.whitelist)
            {
                if (!path.find(processes[i].pProcessName))
                {
                    ApplyBasicThemeInternal(hWnd, TRUE);
                }
            }
        }
    }

    if (!settings.whitelistmode)
    {
        ApplyBasicThemeInternal(hWnd, TRUE);
    }

    WTSFreeMemory(processes);
}

BOOL CALLBACK BasicEnumProc(HWND hWnd, LPARAM lParam)
{
    ApplyBasicTheme(hWnd);
    return TRUE;
}

BOOL CALLBACK UninitEnumProc(HWND hWnd, LPARAM lParam)
{
    ApplyBasicThemeInternal(hWnd, FALSE);
    return TRUE;
}

#define CWindowData_Window(pThis) *((HWND *)pThis + 5)

DWORD WINAPI ApplyBasicThreadProc(LPVOID lpParam)
{
    ApplyBasicTheme((HWND)lpParam);
    return 0;
}

HRESULT (*CWindowList_SyncWindowData_orig)(void *, void *, void *);
HRESULT CWindowList_SyncWindowData_hook(
    void *pThis,
    void *pDwmWindow,
    void *pWindowData
)
{
    HWND hWnd = CWindowData_Window(pWindowData);
    if (hWnd)
    {
        /* This must be run in a separate thread or else some windows will
           have their frames go completely white */
        HANDLE hThread = CreateThread(
            NULL,
            0,
            ApplyBasicThreadProc,
            hWnd,
            0,
            nullptr
        );
        CloseHandle(hThread);
    }
    return CWindowList_SyncWindowData_orig(pThis, pDwmWindow, pWindowData);
}

#define LoadIntSetting(NAME) settings.NAME = Wh_GetIntSetting(L ## #NAME)

void LoadSettings(void)
{
    settings.blacklist.clear();
    settings.whitelist.clear();

    LoadIntSetting(whitelistmode);
    LoadIntSetting(secureonly);
    LoadIntSetting(noanims);

    for (int i = 0;; i++)
    {
        PCWSTR lpszPath = Wh_GetStringSetting(L"blacklist[%d].process", i);
        if (!lpszPath || !*lpszPath)
            break;
        settings.blacklist.push_back(lpszPath);
        Wh_FreeStringSetting(lpszPath);
    }

    for (int i = 0;; i++)
    {
        PCWSTR lpszPath = Wh_GetStringSetting(L"whitelist[%d].process", i);
        if (!lpszPath || !*lpszPath)
            break;
        settings.whitelist.push_back(lpszPath);
        Wh_FreeStringSetting(lpszPath);
    }
}

const WindhawkUtils::SYMBOL_HOOK hooks[] = {
    {
        {
            L"public: long __cdecl CWindowList::SyncWindowData(struct IDwmWindow *,class CWindowData *)"
        },
        &CWindowList_SyncWindowData_orig,
        CWindowList_SyncWindowData_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    LoadSettings();

    HMODULE uDWM = LoadLibraryW(L"uDWM.dll");
    if (!uDWM)
    {
        Wh_Log(L"Failed to load uDWM.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        uDWM,
        hooks,
        ARRAYSIZE(hooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in uDWM.dll");
        return FALSE;
    }

    EnumDesktopWindows(NULL, BasicEnumProc, NULL);

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
    EnumDesktopWindows(NULL, BasicEnumProc, NULL);
}

void Wh_ModUninit()
{
    EnumDesktopWindows(NULL, UninitEnumProc, NULL);
}