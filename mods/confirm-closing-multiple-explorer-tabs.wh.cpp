// ==WindhawkMod==
// @id              confirm-closing-multiple-explorer-tabs
// @name            Confirm Closing Multiple Tabs in File Explorer
// @description     Shows a confirmation dialog when closing a File Explorer window with multiple tabs open
// @version         1.1
// @author          Kitsune
// @github          https://github.com/AromaKitsune
// @include         explorer.exe
// @compilerOptions -lcomctl32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Confirm Closing Multiple Tabs in File Explorer
This mod shows a confirmation dialog when you attempt to close a File Explorer
window with multiple tabs open, preventing accidental closure of all tabs.

![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/confirm-closing-multiple-explorer-tabs_2026-06-20.png)

## Configuration
* **Tab count threshold:** The minimum number of open tabs required to show
  the confirmation dialog.
* **Default button:** Choose whether "Close Tabs" or "Cancel" is the default
  button in the confirmation dialog.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- tabCountThreshold: 2
  $name: Tab count threshold
  $description: >-
    The minimum number of open tabs required to show the confirmation dialog
- defaultButton: cancel
  $name: Default button
  $description: >-
    Choose whether "Close Tabs" or "Cancel" is the default button in the
    confirmation dialog
  $options:
    - closeTabs: Close Tabs
    - cancel: Cancel
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <algorithm>
#include <commctrl.h>
#include <cwchar>

struct
{
    int tabCountThreshold;
    int defaultButton;
} settings;

// Enumeration callback: Count instances of ShellTabWindowClass
BOOL CALLBACK CountTabsEnumProc(HWND hChildWnd, LPARAM lParam)
{
    auto* pcTabs = reinterpret_cast<int*>(lParam);
    WCHAR szClassName[32];
    if (GetClassNameW(hChildWnd, szClassName, ARRAYSIZE(szClassName)) &&
        _wcsicmp(szClassName, L"ShellTabWindowClass") == 0)
    {
        (*pcTabs)++;
    }
    return TRUE;
}

// Helper: Get the total number of tabs
int CountExplorerTabs(HWND hExplorerWnd)
{
    int cTabs = 0;
    EnumChildWindows(hExplorerWnd, CountTabsEnumProc,
        reinterpret_cast<LPARAM>(&cTabs));
    return cTabs;
}

// Subclass procedure for File Explorer windows: Show a confirmation dialog when
// closing multiple tabs
LRESULT CALLBACK ExplorerSubclassProc(HWND hExplorerWnd, UINT uMsg,
    WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    if (uMsg == WM_CLOSE)
    {
        int cTabs = CountExplorerTabs(hExplorerWnd);

        // Prompt for confirmation if the tab count threshold is met
        if (cTabs >= settings.tabCountThreshold)
        {
            // Prepare the dynamic Main Instruction and button text
            WCHAR szMainInstructionBuffer[32];
            PCWSTR pszMainInstruction;
            PCWSTR pszCloseButtonText;
            if (cTabs == 1)
            {
                pszMainInstruction = L"Close 1 tab?";
                pszCloseButtonText = L"Close Tab";
            }
            else
            {
                swprintf_s(szMainInstructionBuffer,
                    ARRAYSIZE(szMainInstructionBuffer),
                    L"Close %d tabs?", cTabs);
                pszMainInstruction = szMainInstructionBuffer;
                pszCloseButtonText = L"Close Tabs";
            }

            // Define custom buttons
            const TASKDIALOG_BUTTON rgButtons[] =
            {
                { IDOK, pszCloseButtonText },
                { IDCANCEL, L"Cancel" }
            };

            // Task Dialog configuration
            TASKDIALOGCONFIG taskDialogConfig{};
            taskDialogConfig.cbSize = sizeof(taskDialogConfig);
            taskDialogConfig.hwndParent = hExplorerWnd;
            taskDialogConfig.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW;
            taskDialogConfig.pszWindowTitle = L"File Explorer";
            taskDialogConfig.pszMainIcon = TD_INFORMATION_ICON;
            taskDialogConfig.pszMainInstruction = pszMainInstruction;
            taskDialogConfig.cButtons = ARRAYSIZE(rgButtons);
            taskDialogConfig.pButtons = rgButtons;
            taskDialogConfig.nDefaultButton = settings.defaultButton;

            int idButton = 0;
            HRESULT hResult = TaskDialogIndirect(&taskDialogConfig, &idButton,
                nullptr, nullptr);
            if (SUCCEEDED(hResult) && idButton != IDOK)
            {
                return 0;
            }
        }
    }
    else if (uMsg == WM_NCDESTROY)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hExplorerWnd,
            ExplorerSubclassProc);
    }
    return DefSubclassProc(hExplorerWnd, uMsg, wParam, lParam);
}

// Hook for CreateWindowExW
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
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
    LPVOID lpParam
)
{
    HWND hWnd = CreateWindowExW_Original(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam
    );
    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName) &&
        _wcsicmp(lpClassName, L"CabinetWClass") == 0)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
            ExplorerSubclassProc, 0);
    }
    return hWnd;
}

// Enumeration callback: Attach the subclass to existing File Explorer windows
BOOL CALLBACK InitEnumExplorerWindowsProc(HWND hWnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;
    WCHAR szClassName[16];
    if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
        dwProcessId == GetCurrentProcessId() &&
        GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName)) &&
        _wcsicmp(szClassName, L"CabinetWClass") == 0)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
            ExplorerSubclassProc, 0);
    }
    return TRUE;
}

// Enumeration callback: Detach the subclass from existing File Explorer windows
BOOL CALLBACK UninitEnumExplorerWindowsProc(HWND hWnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;
    WCHAR szClassName[16];
    if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
        dwProcessId == GetCurrentProcessId() &&
        GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName)) &&
        _wcsicmp(szClassName, L"CabinetWClass") == 0)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
            ExplorerSubclassProc);
    }
    return TRUE;
}

// Load settings
void LoadSettings()
{
    settings.tabCountThreshold =
        std::max(1, Wh_GetIntSetting(L"tabCountThreshold"));

    settings.defaultButton = IDCANCEL;
    if (wcscmp(WindhawkUtils::StringSetting::make(L"defaultButton").get(),
            L"closeTabs") == 0)
    {
        settings.defaultButton = IDOK;
    }
}

// Mod initialization
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    WindhawkUtils::SetFunctionHook(
        CreateWindowExW,
        CreateWindowExW_Hook,
        &CreateWindowExW_Original
    );

    EnumWindows(InitEnumExplorerWindowsProc, 0);

    return TRUE;
}

// Mod uninitialization
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    EnumWindows(UninitEnumExplorerWindowsProc, 0);
}

// Reload settings
void Wh_ModSettingsChanged()
{
    LoadSettings();
}
