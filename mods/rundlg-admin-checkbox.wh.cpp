// ==WindhawkMod==
// @id              rundlg-admin-checkbox
// @name            Run Dialog Admin Checkbox
// @description     Puts the Create this task with administrative privileges checkbox on the Win+R Run dialog
// @version         1.0.0
// @author          repensky
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lshlwapi
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
- consentHotkey: false
  $name: Also accept Ctrl+Shift+Enter
  $description: Ask Windows to accept Ctrl+Shift+Enter as well as the checkbox. Unrelated to the checkbox, which works either way. Only useful on a Windows 7 shell, where the shortcut does nothing on its own, since every later shell already enables it.
- label: ""
  $name: Checkbox label
  $description: Wording for the checkbox. Leave empty to use the text Windows already ships in its own admin dialog.
- edgeMargin: 2
  $name: Gap from the screen edge
  $description: Gap kept between the dialog and the taskbar when the taller dialog has to be nudged back on screen. Measured in dialog units so it follows the display scaling, 3 is about 6 pixels at 100 percent. Set to 0 to sit flush against it.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shlwapi.h>

#include <atomic>
#include <mutex>
#include <string>

//---Dialog ids---------------------------------------------

// Control ids shared by every shell32 Run dialog template
#define IDD_RUN_ADMIN    1011
#define IDC_RUN_COMBO    12298
#define IDC_RUN_PROMPT   12305
#define IDC_RUN_SEPMEM   12306
#define IDC_RUN_ADMIN    12307
#define IDC_RUN_BROWSE   12288

// Asks the Run dialog to keep its Ctrl+Shift+Enter handling
// Community name, Microsoft does not publish one for this bit
#define RFD_CONSENTHOTKEY 0x100

// Template 1011 geometry in dialog units, measured from the checkbox top
#define SEPMEM_TOP_UNITS  12
#define BUTTON_TOP_UNITS  32
#define FOOTER_UNITS       9

//---Mod state----------------------------------------------

static std::wstring g_strLabel;
static std::mutex g_lockLabel;

static std::atomic<int> g_nEdgeMargin{ 3 };

static std::atomic<bool> g_fConsentHotkey{ false };

// Thread that is handing an OK press to shell32 with the box ticked
static std::atomic<DWORD> g_dwRunAsThread{ 0 };

// Asks a dialog on its own thread to put itself back the way it was
static UINT g_uRevertMsg = 0;

// Holds what one dialog looked like before the mod touched it
static const WCHAR kStateProp[] = L"WhRunDlgAdminCheckbox";

struct RUNDLG_STATE
{
    RECT rcWindow;
    int  nSepMemTop;
    int  nButtonTop;
    bool fCaptured;
};

//---Settings-----------------------------------------------

static void LoadSettings()
{
    // Wh_GetStringSetting hands back an empty string when nothing is set
    auto label = WindhawkUtils::StringSetting::make(L"label");

    int nMargin = Wh_GetIntSetting(L"edgeMargin");
    if (nMargin < 0)
        nMargin = 0;

    g_nEdgeMargin.store(nMargin);
    g_fConsentHotkey.store(Wh_GetIntSetting(L"consentHotkey") != 0);

    std::lock_guard<std::mutex> guard(g_lockLabel);
    g_strLabel = label.get();
}

//---Dialog template text-----------------------------------

// Steps over a name or ordinal field, never past the end of the resource
static const WORD *SkipNameOrOrdinal(const WORD *p, const WORD *pLimit)
{
    if (p >= pLimit)
        return pLimit;

    if (*p == 0)
        return p + 1;

    if (*p == 0xFFFF)
        return (pLimit - p >= 2) ? p + 2 : pLimit;

    while (p < pLimit && *p)
        p++;

    return (p < pLimit) ? p + 1 : pLimit;
}

// Reads the admin checkbox wording out of shell32's own template
// The resource loader picks the right language file, so this stays localised
static bool ReadShellCheckboxText(std::wstring &strOut)
{
    HMODULE hShell = GetModuleHandleW(L"shell32.dll");
    if (!hShell)
        return false;

    HRSRC hRes = FindResourceW(hShell, MAKEINTRESOURCEW(IDD_RUN_ADMIN),
                               (LPCWSTR)RT_DIALOG);
    if (!hRes)
        return false;

    DWORD cbRes = SizeofResource(hShell, hRes);
    HGLOBAL hMem = LoadResource(hShell, hRes);
    const WORD *p = hMem ? (const WORD *)LockResource(hMem) : nullptr;
    if (!p || cbRes < 2 * sizeof(WORD))
        return false;

    const WORD *pLimit = p + cbRes / sizeof(WORD);

    // Only the extended template shape is used by shell32
    if (pLimit - p < 13 || p[0] != 1 || p[1] != 0xFFFF)
        return false;

    DWORD dwStyle = *(const DWORD *)(p + 6);
    WORD  cItems  = p[8];

    p += 13;
    p = SkipNameOrOrdinal(p, pLimit);
    p = SkipNameOrOrdinal(p, pLimit);
    p = SkipNameOrOrdinal(p, pLimit);

    if (dwStyle & DS_SETFONT) {
        if (pLimit - p < 3)
            return false;
        p += 3;
        p = SkipNameOrOrdinal(p, pLimit);
    }

    for (WORD i = 0; i < cItems; i++) {
        p = (const WORD *)(((ULONG_PTR)p + 3) & ~(ULONG_PTR)3);
        if (p >= pLimit || pLimit - p < 13)
            return false;

        DWORD dwId = *(const DWORD *)(p + 10);
        const WORD *pTitle = SkipNameOrOrdinal(p + 12, pLimit);
        const WORD *pTail  = SkipNameOrOrdinal(pTitle, pLimit);

        if (dwId == IDC_RUN_ADMIN) {
            if (pTitle >= pLimit || *pTitle == 0 || *pTitle == 0xFFFF)
                return false;

            const WORD *q = pTitle;
            while (q < pLimit && *q)
                q++;

            // A caption running off the end means the walk went wrong
            if (q >= pLimit)
                return false;

            strOut.assign((PCWSTR)pTitle, (size_t)(q - pTitle));
            return true;
        }

        if (pTail >= pLimit)
            return false;

        p = pTail + 1 + (*pTail + 1) / 2;
    }

    return false;
}

// Picks the wording, the setting wins over what shell32 ships
static std::wstring ChooseCheckboxText()
{
    {
        std::lock_guard<std::mutex> guard(g_lockLabel);
        if (!g_strLabel.empty()) {
            Wh_Log(L"Caption came from the mod setting");
            return g_strLabel;
        }
    }

    std::wstring strText;
    if (ReadShellCheckboxText(strText)) {
        Wh_Log(L"Caption came from the shell32 template");
        return strText;
    }

    Wh_Log(L"Caption came from the built in English text");
    return L"Create this task with administrative privileges.";
}

//---Layout-------------------------------------------------

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

    // Dialog units keep the gap even across display scalings
    RECT rcMargin = { 0, 0, g_nEdgeMargin.load(), g_nEdgeMargin.load() };
    MapDialogRect(hDlg, &rcMargin);

    RECT rcFit = mi.rcWork;
    InflateRect(&rcFit, -rcMargin.right, -rcMargin.bottom);

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

//---Checkbox-----------------------------------------------

// Builds the checkbox on the row the Run dialog keeps for the hidden option
static bool AddAdminCheckbox(HWND hDlg)
{
    if (GetDlgItem(hDlg, IDC_RUN_ADMIN))
        return false;

    RUNDLG_STATE *pState = (RUNDLG_STATE *)GetPropW(hDlg, kStateProp);
    if (!pState)
        return false;

    HWND hSepMem = GetDlgItem(hDlg, IDC_RUN_SEPMEM);

    RECT rc;
    RECT rcOK;
    if (!GetChildRect(hDlg, hSepMem, &rc)
        || !GetChildRect(hDlg, GetDlgItem(hDlg, IDOK), &rcOK)
        || !GetWindowRect(hDlg, &pState->rcWindow))
        return false;

    // Remembered so an unload can put the dialog back
    pState->nSepMemTop = rc.top;
    pState->nButtonTop = rcOK.top;
    pState->fCaptured = true;

    std::wstring strText = ChooseCheckboxText();

    HWND hCheck = CreateWindowExW(
        0, L"BUTTON", strText.c_str(),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
        hDlg, (HMENU)(UINT_PTR)IDC_RUN_ADMIN, NULL, NULL);

    if (!hCheck) {
        pState->fCaptured = false;
        return false;
    }

    HFONT hFont = (HFONT)SendMessageW(hDlg, WM_GETFONT, 0, 0);
    if (hFont)
        SendMessageW(hCheck, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Sits where shell32 puts it, so Tab reaches it before the buttons
    SetWindowPos(hCheck, hSepMem, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    ApplyAdminLayout(hDlg, &rc);
    return true;
}

// Puts the dialog back the way it was found, runs on the dialog's own thread
static void RevertDialog(HWND hDlg)
{
    RUNDLG_STATE *pState = (RUNDLG_STATE *)GetPropW(hDlg, kStateProp);
    RemovePropW(hDlg, kStateProp);
    if (!pState)
        return;

    if (pState->fCaptured) {
        HWND hCheck = GetDlgItem(hDlg, IDC_RUN_ADMIN);
        if (hCheck) {
            // Focus would be left nowhere if the box still held it
            if (GetFocus() == hCheck)
                SetFocus(GetDlgItem(hDlg, IDC_RUN_COMBO));
            DestroyWindow(hCheck);
        }

        MoveChildTo(hDlg, IDC_RUN_SEPMEM, pState->nSepMemTop);
        MoveChildTo(hDlg, IDOK, pState->nButtonTop);
        MoveChildTo(hDlg, IDCANCEL, pState->nButtonTop);
        MoveChildTo(hDlg, IDC_RUN_BROWSE, pState->nButtonTop);

        SetWindowPos(hDlg, NULL,
                     pState->rcWindow.left, pState->rcWindow.top,
                     pState->rcWindow.right - pState->rcWindow.left,
                     pState->rcWindow.bottom - pState->rcWindow.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    delete pState;
    Wh_Log(L"Put the Run dialog back");
}

//---Dialog subclass----------------------------------------

LRESULT CALLBACK RunDlgSubclass(HWND hWnd, UINT uMsg, WPARAM wParam,
                                LPARAM lParam, DWORD_PTR ref)
{
    if (g_uRevertMsg && uMsg == g_uRevertMsg) {
        RevertDialog(hWnd);
        return 0;
    }

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
        if (LOWORD(wParam) == IDOK) {
            // shell32 does the whole launch inside this one message
            DWORD dwWant = 0;
            if (IsDlgButtonChecked(hWnd, IDC_RUN_ADMIN) == BST_CHECKED)
                dwWant = GetCurrentThreadId();

            // Saved and put back, so a nested OK cannot clear the outer one
            DWORD dwPrev = g_dwRunAsThread.exchange(dwWant);
            LRESULT lr = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            g_dwRunAsThread.store(dwPrev);
            return lr;
        }
        break;

    case WM_NCDESTROY:
    {
        // Windhawk drops the subclass itself here, only the state is ours
        RUNDLG_STATE *pState = (RUNDLG_STATE *)GetPropW(hWnd, kStateProp);
        RemovePropW(hWnd, kStateProp);
        delete pState;
        break;
    }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

//---Dialog watch-------------------------------------------

struct WATCH_SLOT
{
    DWORD dwThreadId;
    HHOOK hHook;
};

static WATCH_SLOT g_watch[8] = {};
static std::mutex g_lockWatch;

// Notes the message hook a thread has just put up
static bool AddWatch(DWORD dwThreadId, HHOOK hHook)
{
    std::lock_guard<std::mutex> guard(g_lockWatch);

    for (WATCH_SLOT &slot : g_watch) {
        if (!slot.hHook) {
            slot.dwThreadId = dwThreadId;
            slot.hHook = hHook;
            return true;
        }
    }

    return false;
}

// Takes one thread's hook down, safe to call when there is nothing to take
static void DropWatch(DWORD dwThreadId)
{
    HHOOK hHook = NULL;

    {
        std::lock_guard<std::mutex> guard(g_lockWatch);
        for (WATCH_SLOT &slot : g_watch) {
            if (slot.hHook && slot.dwThreadId == dwThreadId) {
                hHook = slot.hHook;
                slot.hHook = NULL;
                break;
            }
        }
    }

    if (hHook)
        UnhookWindowsHookEx(hHook);
}

// Leaves no callback of ours registered anywhere, used on the way out
static void DropAllWatches()
{
    HHOOK rgHooks[ARRAYSIZE(g_watch)] = {};
    int cHooks = 0;

    {
        std::lock_guard<std::mutex> guard(g_lockWatch);
        for (WATCH_SLOT &slot : g_watch) {
            if (slot.hHook) {
                rgHooks[cHooks++] = slot.hHook;
                slot.hHook = NULL;
            }
        }
    }

    for (int i = 0; i < cHooks; i++)
        UnhookWindowsHookEx(rgHooks[i]);
}

//---Dialog pickup------------------------------------------

// True for a shell32 Run dialog the mod has not taken over yet
// Windows draws its own checkbox on an elevated shell, so that one is left be
static bool IsPlainRunDialog(HWND hWnd)
{
    if (!hWnd)
        return false;

    // Cheapest test first, this runs for every message on a busy shell thread
    WCHAR szClass[16];
    if (!GetClassNameW(hWnd, szClass, ARRAYSIZE(szClass))
        || lstrcmpW(szClass, L"#32770") != 0)
        return false;

    if (GetPropW(hWnd, kStateProp) || GetDlgItem(hWnd, IDC_RUN_ADMIN))
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
            RUNDLG_STATE *pState = new RUNDLG_STATE{};
            SetPropW(pcw->hwnd, kStateProp, (HANDLE)pState);

            if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                    pcw->hwnd, RunDlgSubclass, 0)) {
                Wh_Log(L"Took over the Run dialog");

                // Nothing left to watch for, so stop running on every message
                DropWatch(GetCurrentThreadId());

                // Setup is handled in the subclass, after shell32 has laid out
                if (pcw->message != WM_INITDIALOG)
                    AddAdminCheckbox(pcw->hwnd);
            } else {
                RemovePropW(pcw->hwnd, kStateProp);
                delete pState;
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

//---Launch hook--------------------------------------------

// The test shell32 makes before it trusts the verb, same call and same rules
// A folder or a URL has no runas verb, and asking for one there would fail
static bool CanRunAs(PCWSTR pszFile)
{
    if (!pszFile || !*pszFile)
        return false;

    DWORD cchOut = 0;
    HRESULT hr = AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_COMMAND, pszFile,
                                   L"runas", NULL, &cchOut);

    return SUCCEEDED(hr) && cchOut != 0;
}

using ShellExecuteExW_t = decltype(&ShellExecuteExW);
static ShellExecuteExW_t ShellExecuteExW_Original;

BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW *pExecInfo)
{
    // Only the one launch the ticked Run dialog is in the middle of making
    if (pExecInfo && g_dwRunAsThread.load() == GetCurrentThreadId()
        && (!pExecInfo->lpVerb || !*pExecInfo->lpVerb)
        && CanRunAs(pExecInfo->lpFile)) {

        Wh_Log(L"Elevating the Run dialog command");
        pExecInfo->lpVerb = L"runas";
    }

    return ShellExecuteExW_Original(pExecInfo);
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
    DWORD dwThreadId = GetCurrentThreadId();
    HHOOK hWatch = SetWindowsHookExW(WH_CALLWNDPROC, RunDlgWatchProc, NULL,
                                     dwThreadId);

    if (hWatch && !AddWatch(dwThreadId, hWatch)) {
        UnhookWindowsHookEx(hWatch);
        Wh_Log(L"No free watch slot, this dialog gets no checkbox");
    }

    // Only ever added, so leaving the setting off cannot disable the shortcut
    if (g_fConsentHotkey.load())
        dwFlags |= RFD_CONSENTHOTKEY;

    RunFileDlg_Original(hwndParent, hIcon, pszWorkingDir, pszTitle,
                        pszPrompt, dwFlags);

    // Usually gone already, the watcher drops itself once it finds the dialog
    DropWatch(dwThreadId);
}

//---Mod lifetime-------------------------------------------

// Reverts and releases every dialog the mod is still holding
static BOOL CALLBACK ReleaseDialogProc(HWND hWnd, LPARAM lParam)
{
    DWORD dwPid = 0;
    GetWindowThreadProcessId(hWnd, &dwPid);

    if (dwPid != GetCurrentProcessId() || !GetPropW(hWnd, kStateProp))
        return TRUE;

    DWORD_PTR dwResult = 0;
    SendMessageTimeoutW(hWnd, g_uRevertMsg, 0, 0,
                        SMTO_ABORTIFHUNG | SMTO_NORMAL, 2000, &dwResult);

    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, RunDlgSubclass);
    return TRUE;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    g_uRevertMsg = RegisterWindowMessageW(L"WhRunDlgAdminCheckboxRevert");

    // shell32 is always mapped in explorer, so no reference is taken
    HMODULE hShell = GetModuleHandleW(L"shell32.dll");
    if (!hShell) {
        Wh_Log(L"shell32.dll is not loaded");
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
    WindhawkUtils::SetFunctionHook(ShellExecuteExW, ShellExecuteExW_Hook,
                                   &ShellExecuteExW_Original);

    return TRUE;
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
}

void Wh_ModBeforeUninit()
{
    Wh_Log(L"Before uninit");

    // Message hooks go first, so no new dialog can be taken over meanwhile
    DropAllWatches();

    EnumWindows(ReleaseDialogProc, 0);

    g_dwRunAsThread.store(0);
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}
