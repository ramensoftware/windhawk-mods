// ==WindhawkMod==
// @id              rundlg-admin-checkbox
// @name            Run Dialog Admin Checkbox
// @description     Puts the Create this task with administrative privileges checkbox on the Win+R Run dialog, and restores Ctrl+Shift+Enter
// @version         1.0.0
// @author          repensky
// @github          https://github.com/repensky
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Run Dialog Admin Checkbox

Task Manager's Create new task dialog has a checkbox to run the command as
administrator. Win+R does not, even though both dialogs are the same shell32
code. This mod puts that checkbox on Win+R. Ctrl+Shift+Enter hotkey is also restored.

Tested and works on 10 21H2 and 11 24H2.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- label: ""
  $name: Checkbox label
  $description: Wording for the checkbox. Leave empty to use the text Windows already ships in its own admin dialog.
- edgeMargin: 6
  $name: Gap from the screen edge
  $description: Pixels kept between the dialog and the taskbar when the taller dialog has to be nudged back on screen. Set to 0 to sit flush against it.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

//---Dialog ids---------------------------------------------

// Control ids shared by every shell32 Run dialog template
#define IDD_RUN_ADMIN    1011
#define IDC_RUN_COMBO    12298
#define IDC_RUN_PROMPT   12305
#define IDC_RUN_SEPMEM   12306
#define IDC_RUN_ADMIN    12307
#define IDC_RUN_BROWSE   12288

// Asks the Run dialog to keep its Ctrl+Shift+Enter handling
#define RFF_OPTRUNAS     0x100

// Template 1011 geometry in dialog units, measured from the checkbox top
#define SEPMEM_TOP_UNITS  12
#define BUTTON_TOP_UNITS  32
#define FOOTER_UNITS       9

//---Mod state----------------------------------------------

static WCHAR g_szLabel[256] = L"";

static int g_nEdgeMargin = 6;

// Thread that is handing an OK press to shell32 with the box ticked
static volatile LONG g_dwRunAsThread = 0;

static HWND g_hwndRunDlg = NULL;

// Marks a dialog the mod has already taken over
static const WCHAR kAdoptedProp[] = L"WhRunDlgAdminCheckbox";

//---Settings-----------------------------------------------

static void LoadSettings()
{
    g_szLabel[0] = 0;

    PCWSTR psz = Wh_GetStringSetting(L"label");
    if (psz) {
        lstrcpynW(g_szLabel, psz, ARRAYSIZE(g_szLabel));
        Wh_FreeStringSetting(psz);
    }

    g_nEdgeMargin = Wh_GetIntSetting(L"edgeMargin");
    if (g_nEdgeMargin < 0)
        g_nEdgeMargin = 0;
}

//---Dialog template text-----------------------------------

// Steps over a name or ordinal field in a dialog template
static const WORD *SkipNameOrOrdinal(const WORD *p)
{
    if (*p == 0)
        return p + 1;
    if (*p == 0xFFFF)
        return p + 2;

    while (*p)
        p++;
    return p + 1;
}

// Reads the admin checkbox wording out of shell32's own template
// The resource loader picks the right language file, so this stays localised
static bool ReadShellCheckboxText(WCHAR *pszOut, int cchOut)
{
    HMODULE hShell = GetModuleHandleW(L"shell32.dll");
    if (!hShell)
        return false;

    HRSRC hRes = FindResourceW(hShell, MAKEINTRESOURCEW(IDD_RUN_ADMIN),
                               (LPCWSTR)RT_DIALOG);
    if (!hRes)
        return false;

    HGLOBAL hMem = LoadResource(hShell, hRes);
    if (!hMem)
        return false;

    const WORD *p = (const WORD *)LockResource(hMem);
    if (!p)
        return false;

    // Only the extended template shape is used by shell32
    if (p[0] != 1 || p[1] != 0xFFFF)
        return false;

    DWORD style  = *(const DWORD *)(p + 6);
    WORD  cItems = p[8];

    p += 13;
    p = SkipNameOrOrdinal(p);
    p = SkipNameOrOrdinal(p);
    p = SkipNameOrOrdinal(p);

    if (style & DS_SETFONT) {
        p += 3;
        p = SkipNameOrOrdinal(p);
    }

    for (WORD i = 0; i < cItems; i++) {
        p = (const WORD *)(((ULONG_PTR)p + 3) & ~(ULONG_PTR)3);

        DWORD id = *(const DWORD *)(p + 10);
        const WORD *pTitle = SkipNameOrOrdinal(p + 12);
        const WORD *pEnd   = SkipNameOrOrdinal(pTitle);

        if (id == IDC_RUN_ADMIN && *pTitle != 0 && *pTitle != 0xFFFF) {
            lstrcpynW(pszOut, (LPCWSTR)pTitle, cchOut);
            return true;
        }

        p = pEnd + 1 + (*pEnd + 1) / 2;
    }

    return false;
}

// Picks the wording, the setting wins over what shell32 ships
static void ChooseCheckboxText(WCHAR *pszOut, int cchOut)
{
    if (g_szLabel[0]) {
        Wh_Log(L"Caption came from the mod setting");
        lstrcpynW(pszOut, g_szLabel, cchOut);
        return;
    }

    if (ReadShellCheckboxText(pszOut, cchOut)) {
        Wh_Log(L"Caption came from the shell32 template");
        return;
    }

    Wh_Log(L"Caption came from the built in English text");
    lstrcpynW(pszOut, L"Create this task with administrative privileges.",
              cchOut);
}

//---Checkbox-----------------------------------------------

// Reads a control rectangle in the coordinates of the dialog it sits on
static bool GetChildRect(HWND hDlg, HWND hCtl, RECT *prc)
{
    if (!hCtl || !GetWindowRect(hCtl, prc))
        return false;

    MapWindowPoints(NULL, hDlg, (LPPOINT)prc, 2);
    return true;
}

// Sets how far down the dialog one control sits, leaving its left edge alone
static void MoveChildTo(HWND hDlg, int id, int top)
{
    HWND hCtl = GetDlgItem(hDlg, id);

    RECT rc;
    if (!GetChildRect(hDlg, hCtl, &rc))
        return;

    SetWindowPos(hCtl, NULL, rc.left, top, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// Lifts the dialog back out of the taskbar after it has been made taller
// Explorer aims it at the tray corner, so the extra height lands on the edge
static void KeepInsideWorkArea(HWND hDlg)
{
    RECT rc;
    HMONITOR hMon = MonitorFromWindow(hDlg, MONITOR_DEFAULTTONEAREST);

    MONITORINFO mi = { sizeof(mi) };
    if (!hMon || !GetMonitorInfoW(hMon, &mi) || !GetWindowRect(hDlg, &rc))
        return;

    // The gap the dialog keeps off whichever edge the taskbar is on
    RECT rcFit = mi.rcWork;
    InflateRect(&rcFit, -g_nEdgeMargin, -g_nEdgeMargin);

    int x = rc.left;
    int y = rc.top;

    if (rc.bottom > rcFit.bottom)
        y -= rc.bottom - rcFit.bottom;
    if (y < rcFit.top)
        y = rcFit.top;

    if (rc.right > rcFit.right)
        x -= rc.right - rcFit.right;
    if (x < rcFit.left)
        x = rcFit.left;

    if (x == rc.left && y == rc.top)
        return;

    Wh_Log(L"Nudged the dialog back inside the work area");
    SetWindowPos(hDlg, NULL, x, y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// Places the rows under the checkbox exactly where template 1011 has them
// Every position is absolute, so running this twice lands in the same place
static void ApplyAdminLayout(HWND hDlg, const RECT *prcCheck)
{
    RECT rcRows = { 0, SEPMEM_TOP_UNITS, 0, BUTTON_TOP_UNITS };
    RECT rcFoot = { 0, 0, 0, FOOTER_UNITS };
    MapDialogRect(hDlg, &rcRows);
    MapDialogRect(hDlg, &rcFoot);

    int nButtonTop = prcCheck->top + rcRows.bottom;

    MoveChildTo(hDlg, IDC_RUN_SEPMEM, prcCheck->top + rcRows.top);
    MoveChildTo(hDlg, IDOK, nButtonTop);
    MoveChildTo(hDlg, IDCANCEL, nButtonTop);
    MoveChildTo(hDlg, IDC_RUN_BROWSE, nButtonTop);

    RECT rcOK;
    RECT rcClient;
    if (!GetChildRect(hDlg, GetDlgItem(hDlg, IDOK), &rcOK)
        || !GetClientRect(hDlg, &rcClient))
        return;

    int nGrow = rcOK.bottom + rcFoot.bottom - rcClient.bottom;
    Wh_Log(L"Layout, buttons want %d and landed %d, dialog grows %d",
           nButtonTop, (int)rcOK.top, nGrow);

    RECT rcWnd;
    if (nGrow && GetWindowRect(hDlg, &rcWnd))
        SetWindowPos(hDlg, NULL, 0, 0,
                     rcWnd.right - rcWnd.left,
                     rcWnd.bottom - rcWnd.top + nGrow,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    KeepInsideWorkArea(hDlg);
}

// Reads the checkbox back off the dialog and lays the rest out around it
static void RelayoutAdminDialog(HWND hDlg)
{
    RECT rc;
    if (GetChildRect(hDlg, GetDlgItem(hDlg, IDC_RUN_ADMIN), &rc))
        ApplyAdminLayout(hDlg, &rc);
}

// Builds the checkbox on the row the Run dialog keeps for the hidden option
static bool AddAdminCheckbox(HWND hDlg)
{
    if (GetDlgItem(hDlg, IDC_RUN_ADMIN))
        return false;

    HWND hSepMem = GetDlgItem(hDlg, IDC_RUN_SEPMEM);

    RECT rc;
    if (!GetChildRect(hDlg, hSepMem, &rc))
        return false;

    WCHAR szText[256];
    ChooseCheckboxText(szText, ARRAYSIZE(szText));

    HWND hCheck = CreateWindowExW(
        0, L"BUTTON", szText,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
        hDlg, (HMENU)(UINT_PTR)IDC_RUN_ADMIN, NULL, NULL);

    if (!hCheck)
        return false;

    HFONT hFont = (HFONT)SendMessageW(hDlg, WM_GETFONT, 0, 0);
    if (hFont)
        SendMessageW(hCheck, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Sits where shell32 puts it, so Tab reaches it before the buttons
    SetWindowPos(hCheck, hSepMem, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    ApplyAdminLayout(hDlg, &rc);
    return true;
}

//---Dialog subclass----------------------------------------

LRESULT CALLBACK RunDlgSubclass(HWND hWnd, UINT uMsg, WPARAM wParam,
                                LPARAM lParam, DWORD_PTR ref)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        // shell32 lays the dialog out first, the free row only shows up after
        LRESULT lr = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        AddAdminCheckbox(hWnd);
        return lr;
    }

    case WM_SHOWWINDOW:
        // Last chance before the dialog is painted, in case setup was undone
        if (wParam)
            RelayoutAdminDialog(hWnd);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK
            && IsDlgButtonChecked(hWnd, IDC_RUN_ADMIN) == BST_CHECKED) {

            // shell32 reads the keyboard while it works through this message
            InterlockedExchange(&g_dwRunAsThread, (LONG)GetCurrentThreadId());
            LRESULT lr = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            InterlockedExchange(&g_dwRunAsThread, 0);
            return lr;
        }
        break;

    case WM_NCDESTROY:
        if (g_hwndRunDlg == hWnd)
            g_hwndRunDlg = NULL;
        RemovePropW(hWnd, kAdoptedProp);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, RunDlgSubclass);
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

//---Dialog pickup------------------------------------------

// True for a shell32 Run dialog the mod has not taken over yet
// Windows draws its own checkbox on an elevated shell, so that one is left be
static bool IsPlainRunDialog(HWND hWnd)
{
    if (!hWnd || GetPropW(hWnd, kAdoptedProp))
        return false;

    if (GetDlgItem(hWnd, IDC_RUN_ADMIN))
        return false;

    return GetDlgItem(hWnd, IDC_RUN_COMBO)
        && GetDlgItem(hWnd, IDC_RUN_PROMPT)
        && GetDlgItem(hWnd, IDC_RUN_SEPMEM);
}

// Watches the thread that is inside RunFileDlg for its dialog appearing
LRESULT CALLBACK RunDlgWatchProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION) {
        CWPSTRUCT *pcw = (CWPSTRUCT *)lParam;

        if (pcw && IsPlainRunDialog(pcw->hwnd)) {
            SetPropW(pcw->hwnd, kAdoptedProp, (HANDLE)1);

            if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                    pcw->hwnd, RunDlgSubclass, 0)) {
                g_hwndRunDlg = pcw->hwnd;
                Wh_Log(L"Took over the Run dialog");

                // Setup is handled in the subclass, after shell32 has laid out
                if (pcw->message != WM_INITDIALOG)
                    AddAdminCheckbox(pcw->hwnd);
            } else {
                RemovePropW(pcw->hwnd, kAdoptedProp);
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

//---Key state hook-----------------------------------------

using GetKeyState_t = decltype(&GetKeyState);
static GetKeyState_t GetKeyState_Original;

SHORT WINAPI GetKeyState_Hook(int nVirtKey)
{
    if ((nVirtKey == VK_SHIFT || nVirtKey == VK_CONTROL)
        && (LONG)GetCurrentThreadId() == g_dwRunAsThread)
        return (SHORT)0x8000;

    return GetKeyState_Original(nVirtKey);
}

//---Run dialog hook----------------------------------------

typedef void (WINAPI *RunFileDlg_t)(HWND hwndParent, HICON hIcon,
                                    LPCWSTR pszWorkingDir, LPCWSTR pszTitle,
                                    LPCWSTR pszPrompt, DWORD dwFlags);
static RunFileDlg_t RunFileDlg_Original;

void WINAPI RunFileDlg_Hook(HWND hwndParent, HICON hIcon,
                            LPCWSTR pszWorkingDir, LPCWSTR pszTitle,
                            LPCWSTR pszPrompt, DWORD dwFlags)
{
    Wh_Log(L"RunFileDlg flags 0x%X", dwFlags);

    // The dialog is built and run on this thread, so watch it from here
    HHOOK hWatch = SetWindowsHookExW(WH_CALLWNDPROC, RunDlgWatchProc, NULL,
                                     GetCurrentThreadId());

    RunFileDlg_Original(hwndParent, hIcon, pszWorkingDir, pszTitle,
                        pszPrompt, dwFlags | RFF_OPTRUNAS);

    if (hWatch)
        UnhookWindowsHookEx(hWatch);
}

//---Mod lifetime-------------------------------------------

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE hShell = LoadLibraryW(L"shell32.dll");
    if (!hShell) {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    // shell32 exports the Run dialog by ordinal only, it has no name
    RunFileDlg_t pfnRunFileDlg =
        (RunFileDlg_t)GetProcAddress(hShell, MAKEINTRESOURCEA(61));
    if (!pfnRunFileDlg) {
        Wh_Log(L"shell32 ordinal 61 is missing");
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(pfnRunFileDlg, RunFileDlg_Hook,
                                   &RunFileDlg_Original);
    WindhawkUtils::SetFunctionHook(GetKeyState, GetKeyState_Hook,
                                   &GetKeyState_Original);

    return TRUE;
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    InterlockedExchange(&g_dwRunAsThread, 0);

    if (g_hwndRunDlg) {
        RemovePropW(g_hwndRunDlg, kAdoptedProp);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_hwndRunDlg,
                                                         RunDlgSubclass);
        g_hwndRunDlg = NULL;
    }
}
