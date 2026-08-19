// ==WindhawkMod==
// @id              prevista-file-copy
// @name            Pre-Vista File Operation Dialogs
// @description     Replaces file transfer progress and confirmation dialogs with pre-Vista versions
// @version         2.2.0
// @author          arceus413
// @github          https://github.com/arceuss
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldbghelp -luxtheme -lcomctl32 -lshlwapi -lgdi32 -luuid
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Pre-Vista File Operation Dialogs
Replaces Windows' modern file-operation dialogs with their classic pre-Vista
counterparts: the animated copy/move/delete **progress** dialog and the old
**confirmation** dialogs (delete, replace/overwrite, and their variants).

The progress dialog reproduces the Windows XP `CProgressDialog` from browseui —
copy, move, delete, recycle and empty-recycle-bin operations — with the original
AVI animations, per-file progress text, and time-remaining estimates. It hooks
`CoCreateInstance` to intercept `CLSID_ProgressDialog` and supply its own
implementation.

The confirmation dialogs reproduce the classic shell32 prompts, shown in place of
the modern ones:
- Confirm file, folder and multiple-item delete (to the Recycle Bin)
- Confirm permanent delete (Shift+Delete)
- Confirm file and folder replace (overwrite) — the file-replace prompt shows
  each file's size, date and icon
- The special warnings for deleting a shortcut or a program

Everything — AVI animations, dialog layouts, strings and icons — is loaded from a
real pre-Vista `shell32.dll` that you point the mod at. **Any shell32 from Windows
95 through Windows XP is fully supported**, whether it stores its dialogs in the
classic (95/98/Me/NT4) or extended (2000/XP) template format. The mod falls back
to the system dialog for anything a particular shell32 doesn't include.

![Copying](https://i.imgur.com/wWq5rhx.png)
![Replacing](https://i.imgur.com/4BSwhbv.png)
![Moving](https://i.imgur.com/Tiey5JK.png)
![Deleting](https://i.imgur.com/5uFWHay.png)
![Deleting Several Items](https://i.imgur.com/0kvoLTV.png)
![Deleting a Shortcut](https://i.imgur.com/vc7AsZf.png)
![Emptying the Recycle Bin](https://i.imgur.com/nBMB7wB.png)

## Settings
- **Shell32.dll path**: full path to a pre-Vista shell32.dll (Windows 95–XP)
  supplying the animations, dialog layouts, strings and icons. If it is unset or
  missing, the system dialogs are used.
- **Use classic confirmations**: replace the modern delete and replace/overwrite
  prompts with the classic ones (turn off to keep only the classic progress
  dialog).
- **XP shortcut / 98 program delete dialogs**: show the special classic warnings
  when deleting a shortcut to a program, or a program file itself.
- **Show time estimate**: show the estimated time remaining on the progress
  dialog; disable for a Windows 9x look.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- xpShell32Path: ""
  $name: Shell32.dll path
  $description: >-
    Full path to a pre-Vista shell32.dll for loading AVI animations, progress
    dialog layouts, transfer confirmation dialogs, and strings. Leave empty to
    keep the system dialogs.
- showShortcutDeleteDialog: true
  $name: Use XP shortcut delete dialog
  $description: >-
    Show the Windows XP-style special dialog when deleting a shortcut to a
    program.
- showProgramDeleteDialog: true
  $name: Use Windows 98 program delete dialog
  $description: >-
    Show the classic special warning when deleting a program file (.exe/.com).
- useClassicConfirmations: true
  $name: Use classic confirmation dialogs
  $description: >-
    Replace the modern delete and replace/overwrite confirmation dialogs with
    pre-Vista classic versions. Requires an XP/2000 shell32.dll to be set above;
    falls back to the system dialogs when it is missing or incompatible. Turn
    off to keep the modern confirmation dialogs.
- showTimeEstimate: true
  $name: Show time estimate
  $description: >-
    Show estimated time remaining on the progress dialog.
    Disable for a Windows 9x style appearance.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shlguid.h>
#include <uxtheme.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <shellapi.h>

#ifndef MapWindowRect
static inline void MapWindowRect(HWND hWndFrom, HWND hWndTo, LPRECT lpRect)
{
    MapWindowPoints(hWndFrom, hWndTo, (LPPOINT)lpRect, 2);
}
#endif

// Define shell GUIDs not provided by uuid.lib
#include <initguid.h>
DEFINE_GUID(CLSID_ProgressDialog, 0xF8383852, 0xFCD3, 0x11d1, 0xA6, 0xB9, 0x00, 0x60, 0x97, 0xDF, 0x5B, 0xD4);
DEFINE_GUID(IID_IProgressDialog, 0xEBBC7C04, 0x315E, 0x11d2, 0xB6, 0x2F, 0x00, 0x60, 0x97, 0xDF, 0x5B, 0xD4);
DEFINE_GUID(IID_IActionProgressDialog, 0x49ff1173, 0xeadc, 0x446d, 0x92, 0x85, 0x15, 0x64, 0x53, 0xa6, 0x43, 0x1c);
DEFINE_GUID(IID_IActionProgress, 0x49ff1172, 0xeadc, 0x446d, 0x92, 0x85, 0x15, 0x64, 0x53, 0xa6, 0x43, 0x1c);
DEFINE_GUID(IID_IOperationsProgressDialog, 0x0C9FB851, 0xE5C9, 0x43EB, 0xA3, 0x70, 0xF0, 0x67, 0x7B, 0x13, 0x87, 0x4C);

// ============================================================================
// Constants matching XP's progress.cpp
// ============================================================================
#define MIN_MINTIME4FEEDBACK    5
#define MS_TIMESLICE            2000
#define SHOW_PROGRESS_TIMEOUT   1000
#define MINSHOWTIME             2000

#define PDM_SHUTDOWN    WM_APP
#define PDM_TERMTHREAD  (WM_APP + 1)
#define PDM_UPDATE      (WM_APP + 2)
#define PDM_STARTTIMER  (WM_APP + 3)

#define ID_SHOWTIMER    1

// Dialog control IDs matching XP shell32's ids.h
#define IDD_PROGDLG_LINE1       102
#define IDD_PROGDLG_LINE2       103
#define IDD_PROGDLG_PROGRESSBAR 104
#define IDD_PROGDLG_LINE3       105
#define IDD_PROGDLG_ANIMATION   106

// AVI resource IDs from XP shell32
#define IDA_FILEMOVE    160
#define IDA_FILECOPY    161
#define IDA_FILEDEL     162
#define IDA_FILENUKE    163
#define IDA_FILEDELREAL 164
#define IDA_APPLYATTRIBS 165

// String resource IDs from XP shell32 (ids.h)
#define IDS_ACTIONTITLE     0x1740
#define IDS_ACTIONTITLEMOVE (IDS_ACTIONTITLE + 1)  // "Moving..."
#define IDS_ACTIONTITLECOPY (IDS_ACTIONTITLE + 2)  // "Copying..."
#define IDS_ACTIONTITLEDEL  (IDS_ACTIONTITLE + 3)  // "Deleting..."
#define IDS_ACTIONTITLEREN  (IDS_ACTIONTITLE + 4)  // "Renaming..."
#define IDS_FROMTO          0x1750  // "From '%1!ls!' to '%2!ls!'"
#define IDS_FROM            0x1751  // "From '%1!ls!'"
#define IDS_BB_EMPTYINGWASTEBASKET 0x2341  // "Emptying the Recycle Bin"

// Classic transfer confirmation dialog IDs/controls/strings (XP ids.h)
#define DLG_DELETE_FILE         1011
#define DLG_DELETE_FOLDER       1012
#define DLG_DELETE_MULTIPLE     1013
#define DLG_REPLACE_FILE        1014
#define DLG_REPLACE_FOLDER      1015
#define DLG_MOVECOPYPROGRESS    1020
#define DLG_WONT_RECYCLE_FOLDER 1021
#define DLG_WONT_RECYCLE_FILE   1022
#define DLG_DELETE_FILE_ARP     1025
#define DLG_PATH_TOO_LONG       1026

#define IDD_TEXT                0x3003
#define IDD_TEXT1               0x3004
#define IDD_TEXT2               0x3005
#define IDD_TEXT3               0x3006
#define IDD_TEXT4               0x3007
#define IDD_ARPWARNINGTEXT      0x3008
#define IDD_ICON_OLD            0x300C
#define IDD_ICON_NEW            0x300D
#define IDD_FILEINFO_OLD        0x300E
#define IDD_FILEINFO_NEW        0x300F
#define IDD_ICON_WASTEBASKET    0x3010
#define IDD_ARPLINKWINDOW       0x3025
#define IDD_YESTOALL            0x3207

#define IDS_DATESIZELINE        64
#define IDS_FILEDELETEWARNING   65
#define IDS_FOLDERDELETEWARNING 66
#define IDS_FILERECYCLEWARNING  67
#define IDS_FOLDERRECYCLEWARNING 68
#define IDS_SELECTEDFILES       0x2221
#define IDS_UNKNOWNAPPLICATION  0x711c

#define IDI_NUKEFILE            161

// SPBEGINF extra flags not in all headers
#ifndef SPBEGINF_AUTOTIME
#define SPBEGINF_AUTOTIME         0x00000002
#endif
#ifndef SPBEGINF_NOPROGRESSBAR
#define SPBEGINF_NOPROGRESSBAR    0x00000010
#endif
#ifndef SPBEGINF_MARQUEEPROGRESS
#define SPBEGINF_MARQUEEPROGRESS  0x00000020
#endif

// SPINITF extra flags
#ifndef SPINITF_MODAL
#define SPINITF_MODAL       0x00000001
#endif
#ifndef SPINITF_NOMINIMIZE
#define SPINITF_NOMINIMIZE  0x00000002
#endif

// ============================================================================
// Global state
// ============================================================================
static WCHAR g_xpShell32Path[MAX_PATH] = {0};
static HMODULE g_hXPShell32 = NULL;
static BOOL g_showTimeEstimate = TRUE;
static BOOL g_showShortcutDeleteDialog = TRUE;
static BOOL g_showProgramDeleteDialog = TRUE;
static BOOL g_useClassicConfirmations = TRUE;
static CRITICAL_SECTION g_cs;
static BOOL g_fEmptyingRecycleBin = FALSE;

// ============================================================================
// SHEmptyRecycleBinW hook — detects empty recycle bin operations
// ============================================================================
typedef HRESULT(WINAPI* SHEmptyRecycleBinW_t)(HWND, LPCWSTR, DWORD);
SHEmptyRecycleBinW_t SHEmptyRecycleBinWOrig = NULL;

HRESULT WINAPI SHEmptyRecycleBinWHook(HWND hwnd, LPCWSTR pszRootPath, DWORD dwFlags)
{
    g_fEmptyingRecycleBin = TRUE;
    HRESULT hr = SHEmptyRecycleBinWOrig(hwnd, pszRootPath, dwFlags);
    g_fEmptyingRecycleBin = FALSE;
    return hr;
}

// ============================================================================
// Helper: Message-pumping wait — prevents deadlock when the calling thread
// owns windows that receive cross-thread SendMessage during our wait.
// ============================================================================
static DWORD PumpWaitForSingleObject(HANDLE hHandle, DWORD dwTimeout)
{
    DWORD dwStart = GetTickCount();
    for (;;)
    {
        DWORD dwElapsed = GetTickCount() - dwStart;
        if (dwElapsed >= dwTimeout)
            return WAIT_TIMEOUT;

        DWORD dwWait = MsgWaitForMultipleObjects(
            1, &hHandle, FALSE,
            dwTimeout - dwElapsed, QS_SENDMESSAGE);

        if (dwWait == WAIT_OBJECT_0)
            return WAIT_OBJECT_0;
        if (dwWait == WAIT_OBJECT_0 + 1)
        {
            // Dispatch pending cross-thread sent messages
            MSG msg;
            PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
        }
        else
            return dwWait;
    }
}

// ============================================================================
// Helper: Load a string resource from the XP shell32.dll with English fallback
// ============================================================================
static BOOL LoadXPString(UINT uID, LPWSTR szBuf, int cchBuf, LPCWSTR wzFallback)
{
    if (g_hXPShell32 && LoadStringW(g_hXPShell32, uID, szBuf, cchBuf) > 0)
        return TRUE;
    if (wzFallback)
        StringCchCopyW(szBuf, cchBuf, wzFallback);
    else
        szBuf[0] = L'\0';
    return FALSE;
}

// Format a shell32 "From/To" string using FormatMessage (handles %1!ls! syntax)
static void FormatFromTo(UINT uFmtID, LPCWSTR wzFallbackFmt,
                         LPCWSTR wzArg1, LPCWSTR wzArg2,
                         LPWSTR wzOut, DWORD cchOut)
{
    WCHAR wzFmt[256];
    LoadXPString(uFmtID, wzFmt, ARRAYSIZE(wzFmt), wzFallbackFmt);

    DWORD_PTR args[2] = { (DWORD_PTR)wzArg1, (DWORD_PTR)(wzArg2 ? wzArg2 : L"") };
    DWORD cch = FormatMessageW(
        FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
        wzFmt, 0, 0, wzOut, cchOut, (va_list*)args);
    if (!cch)
        StringCchPrintfW(wzOut, cchOut, wzFallbackFmt, wzArg1, wzArg2 ? wzArg2 : L"");
}

// ============================================================================
// Helper: Duplicate a wide string (heap-allocated)
// ============================================================================
static LPWSTR XPStrDupW(LPCWSTR src)
{
    if (!src) return NULL;
    size_t len = wcslen(src) + 1;
    LPWSTR dst = (LPWSTR)CoTaskMemAlloc(len * sizeof(WCHAR));
    if (dst) memcpy(dst, src, len * sizeof(WCHAR));
    return dst;
}

static void StrFreeW(LPWSTR* pp)
{
    if (pp && *pp) { CoTaskMemFree(*pp); *pp = NULL; }
}

static void StrSetW(LPWSTR* pp, LPCWSTR src)
{
    StrFreeW(pp);
    if (src) *pp = XPStrDupW(src);
}

// ============================================================================
// Classic transfer confirmation dialog support
// ============================================================================
#define STCONFIRM_RECYCLE_STREAM                 0x05
#define STCONFIRM_RECYCLE_STORAGE                0x06
#define STCONFIRM_RECYCLE_SYSTEM_STREAM          0x0B
#define STCONFIRM_RECYCLE_PROGRAM_STREAM         0x0F
#define STCONFIRM_REPLACE_STREAM                 0x10
#define STCONFIRM_REPLACE_STORAGE                0x11
#define STCONFIRM_REPLACE_READONLY_STREAM        0x14
#define STCONFIRM_REPLACE_SYSTEM_STREAM          0x16
#define STCONFIRM_PERMANENTDELETE_STREAM         0x1B
#define STCONFIRM_PERMANENTDELETE_STORAGE        0x1C
#define STCONFIRM_PERMANENTDELETE_TOOLARGE_STREAM 0x1D
#define STCONFIRM_PERMANENTDELETE_TOOLARGE_STORAGE 0x1E
#define STCONFIRM_PERMANENTDELETE_TOOLARGE_MULTIPLE 0x1F
#define STCONFIRM_RECYCLE_MULTIPLE               0x31
#define STCONFIRM_PERMANENTDELETE_MULTIPLE       0x32
#define STCONFIRM_RECYCLE_ARP                    0x33
#define STCONFIRM_PERMANENTDELETE_PATH_TOO_LONG  0x34
#define STCONFIRM_PERMANENTDELETE_SYSTEM_STREAM  0x39
#define STCONFIRM_PERMANENTDELETE_PROGRAM_STREAM 0x3B
#define STCONFIRM_PERMANENTDELETE_ARP            0x3C
#define STCONFIRM_RECYCLE_SHORTCUT               0x73
#define STCONFIRM_PERMANENTDELETE_SHORTCUT       0x74

enum CONFIRMATIONRESPONSE_MIN : int
{
    CONFIRMRESP_YES = 0,
    CONFIRMRESP_NO = 1,
    CONFIRMRESP_YES_TO_ALL = 2,
    CONFIRMRESP_KEEP_BOTH = 3,
    CONFIRMRESP_CANCEL = 4,
};

struct CONFIRMOP_MIN
{
    DWORD flags;                // +0
    DWORD stgop;                // +4
    DWORD unk8;                 // +8
    DWORD stconfirm;            // +12
    DWORD stconfirm1;           // +16
    DWORD stconfirm2;           // +20
    DWORD stconfirm3;           // +24
    DWORD unk1C;                // +28
    DWORD unk20;                // +32
    DWORD itemCount;            // +36
    IShellItem* psiSource;      // +40
    IShellItem* psiTarget;      // +48
    void* unk38;                // +56
    void* unk40;                // +64
    void* unk48;                // +72
    HWND hwndParent;            // +80
    DWORD unk58;                // +88
    DWORD unk5C;                // +92
};

typedef HRESULT(__fastcall* CTransferConfirmation_Confirm_t)(
    void* pThis,
    const CONFIRMOP_MIN* pConfirmOp,
    CONFIRMATIONRESPONSE_MIN* pResponse,
    int* pfApplyToAll,
    unsigned int* pLastMessageId);

struct CONFIRM_CONFLICT_ITEM_MIN
{
    IShellItem* pShellItem;
    LPWSTR pszOriginalName;
    LPWSTR pszAlternateName;
    LPWSTR pszLocationShort;
    LPWSTR pszLocationFull;
    DWORD nType;
};

struct CONFIRM_CONFLICT_RESULT_INFO_MIN
{
    LPWSTR pszNewName;
    UINT iItemIndex;
};

struct CONFIRM_CONFLICT_RESULT_MIN
{
    INT iResult;
    INT fApplyToAll;
};

struct CONFIRM_CONFLICT_PARAMS_HINT
{
    HWND hwndParent;
    BYTE flags;
    BYTE reserved1[3];
    DWORD conflictType;
    DWORD reserved2;
    DWORD reserved3;
    DWORD cRemainingConflicts;
};

struct ISyncMgrConflictItemsMin : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetCount(UINT* pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetItem(UINT iIndex, CONFIRM_CONFLICT_ITEM_MIN* pItemInfo) = 0;
};

struct ISyncMgrConflictResolutionItemsMin : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetCount(UINT* pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetItem(UINT iIndex, CONFIRM_CONFLICT_RESULT_INFO_MIN* pItemInfo) = 0;
};

typedef HRESULT(__fastcall* CConflictResolutionDlg_ShowDialog_t)(
    void* pConflictParams,
    ISyncMgrConflictItemsMin* pConflictItems,
    CONFIRM_CONFLICT_RESULT_MIN* pResult,
    ISyncMgrConflictResolutionItemsMin** ppResolutionItems);

static CTransferConfirmation_Confirm_t CTransferConfirmation_Confirm_Orig = NULL;
static CConflictResolutionDlg_ShowDialog_t CConflictResolutionDlg_ShowDialog_Orig = NULL;

struct CLASSIC_CONFIRM_CONTEXT
{
    UINT dlgId;
    UINT stconfirm;
    UINT activeTextId;
    UINT itemCount;
    UINT mainIconOrdinal;   // icon resource ordinal from the dialog template
    BOOL recycle;
    BOOL showCancel;
    BOOL showYesToAll;
    BOOL showDates;
    BOOL shrinkDialog;
    BOOL applyToAll;
    BOOL treatNoAsCancel;
    HWND hwndParent;
    DWORD sourceAttrs;
    DWORD destAttrs;
    LPWSTR pszSourcePath;
    LPWSTR pszTargetPath;
    LPWSTR pszSourceName;
    LPWSTR pszTargetName;
    LPWSTR pszWarningText;
    LPWSTR pszShortcutProgramName;
};

static void FreeClassicConfirmContext(CLASSIC_CONFIRM_CONTEXT* pCtx)
{
    if (!pCtx)
        return;

    StrFreeW(&pCtx->pszSourcePath);
    StrFreeW(&pCtx->pszTargetPath);
    StrFreeW(&pCtx->pszSourceName);
    StrFreeW(&pCtx->pszTargetName);
    StrFreeW(&pCtx->pszWarningText);
    StrFreeW(&pCtx->pszShortcutProgramName);
}

static void FreeConfirmConflictItemMin(CONFIRM_CONFLICT_ITEM_MIN* pcci)
{
    if (!pcci)
        return;

    if (pcci->pShellItem)
        pcci->pShellItem->Release();
    CoTaskMemFree(pcci->pszOriginalName);
    CoTaskMemFree(pcci->pszAlternateName);
    CoTaskMemFree(pcci->pszLocationShort);
    CoTaskMemFree(pcci->pszLocationFull);
    ZeroMemory(pcci, sizeof(*pcci));
}

static const GUID kIID_ISyncMgrConflictResolutionItemsMin =
{ 0x458725B9, 0x129D, 0x4135, { 0xA9, 0x98, 0x9C, 0xEA, 0xFE, 0xC2, 0x70, 0x07 } };

class CSingleConflictResolutionItems : public ISyncMgrConflictResolutionItemsMin
{
public:
    CSingleConflictResolutionItems(UINT iItemIndex)
        : m_ref(1)
    {
        m_item.pszNewName = NULL;
        m_item.iItemIndex = iItemIndex;
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        if (!ppv)
            return E_POINTER;
        *ppv = NULL;

        if (riid == IID_IUnknown || IsEqualIID(riid, kIID_ISyncMgrConflictResolutionItemsMin))
            *ppv = static_cast<ISyncMgrConflictResolutionItemsMin*>(this);
        else
            return E_NOINTERFACE;

        AddRef();
        return S_OK;
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_ref);
    }

    STDMETHODIMP_(ULONG) Release()
    {
        ULONG ref = InterlockedDecrement(&m_ref);
        if (!ref)
            delete this;
        return ref;
    }

    STDMETHODIMP GetCount(UINT* pCount)
    {
        if (!pCount)
            return E_POINTER;
        *pCount = 1;
        return S_OK;
    }

    STDMETHODIMP GetItem(UINT iIndex, CONFIRM_CONFLICT_RESULT_INFO_MIN* pItemInfo)
    {
        if (!pItemInfo)
            return E_POINTER;
        if (iIndex != 0)
            return E_INVALIDARG;

        pItemInfo->iItemIndex = m_item.iItemIndex;
        pItemInfo->pszNewName = NULL;
        return S_OK;
    }

private:
    ~CSingleConflictResolutionItems() = default;

    LONG m_ref;
    CONFIRM_CONFLICT_RESULT_INFO_MIN m_item;
};

static HICON ReplaceDialogIcon(HWND hDlg, UINT id, HICON hIcon)
{
    HICON hOld = (HICON)SendDlgItemMessageW(hDlg, id, STM_SETICON, (WPARAM)hIcon, 0);
    if (hOld)
        DestroyIcon(hOld);
    return hOld;
}

static BOOL HasXPDialogResource(UINT dlgId)
{
    if (!g_hXPShell32)
        return FALSE;
    return FindResourceW(g_hXPShell32, MAKEINTRESOURCEW(dlgId), RT_DIALOG) != NULL;
}

static LPCDLGTEMPLATEW GetXPDialogTemplate(UINT dlgId, DWORD* pcbOut = NULL)
{
    if (pcbOut)
        *pcbOut = 0;

    if (!g_hXPShell32)
        return NULL;

    HRSRC hRes = FindResourceW(g_hXPShell32, MAKEINTRESOURCEW(dlgId), RT_DIALOG);
    if (!hRes)
        return NULL;

    // Accept both template formats: 2000/XP shell32 uses DLGTEMPLATEEX
    // (dlgVer == 1, signature == 0xFFFF), while 95/98/NT4 ships classic
    // DLGTEMPLATE resources. CreateDialogIndirectParamW handles either, and
    // the 9x dialogs use the same ordinals and control IDs as XP's. Only
    // require the resource to be large enough for its header so a truncated
    // or foreign resource can't walk us off the mapping.
    DWORD cbRes = SizeofResource(g_hXPShell32, hRes);
    if (cbRes < sizeof(DLGTEMPLATE))
        return NULL;

    HGLOBAL hResData = LoadResource(g_hXPShell32, hRes);
    if (!hResData)
        return NULL;

    const WORD* pw = (const WORD*)LockResource(hResData);
    if (!pw)
        return NULL;

    BOOL fExtended = (pw[0] == 1 && pw[1] == 0xFFFF);
    if (fExtended && cbRes < 26 /* DLGTEMPLATEEX fixed header */)
        return NULL;

    if (pcbOut)
        *pcbOut = cbRes;
    return (LPCDLGTEMPLATEW)pw;
}

static DWORD GetPathAttributesOrDefault(LPCWSTR pszPath, DWORD dwDefault)
{
    if (!pszPath || !*pszPath)
        return dwDefault;

    DWORD dwAttrs = GetFileAttributesW(pszPath);
    return dwAttrs != INVALID_FILE_ATTRIBUTES ? dwAttrs : dwDefault;
}

static BOOL GetShellItemStrings(IShellItem* psi, LPWSTR* ppszPath, LPWSTR* ppszName)
{
    if (ppszPath)
        *ppszPath = NULL;
    if (ppszName)
        *ppszName = NULL;

    if (!psi)
        return FALSE;

    LPWSTR pszPath = NULL;
    if (ppszPath)
    {
        if (FAILED(psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)))
            pszPath = NULL;
    }

    LPWSTR pszName = NULL;
    if (ppszName)
    {
        if (FAILED(psi->GetDisplayName(SIGDN_NORMALDISPLAY, &pszName)))
            pszName = NULL;
    }

    if (ppszPath)
        *ppszPath = pszPath;
    else if (pszPath)
        CoTaskMemFree(pszPath);

    if (ppszName)
        *ppszName = pszName;
    else if (pszName)
        CoTaskMemFree(pszName);

    return pszPath || pszName;
}

static BOOL GetShellItemPath(IShellItem* psi, LPWSTR pszPath, size_t cchPath)
{
    if (!pszPath || cchPath == 0)
        return FALSE;

    pszPath[0] = L'\0';
    if (!psi)
        return FALSE;

    // Try the filesystem path first, then the absolute parsing name. Items
    // handed to UpdateLocations (notably the destination) frequently fail
    // SIGDN_FILESYSPATH yet resolve via SIGDN_DESKTOPABSOLUTEPARSING. Accept
    // only real drive/UNC paths, never shell parsing names like "::{CLSID}".
    const SIGDN forms[2] = { SIGDN_FILESYSPATH, SIGDN_DESKTOPABSOLUTEPARSING };
    for (int i = 0; i < 2; i++)
    {
        LPWSTR pszTemp = NULL;
        if (SUCCEEDED(psi->GetDisplayName(forms[i], &pszTemp)) && pszTemp && pszTemp[0])
        {
            BOOL bFsPath = (pszTemp[1] == L':') ||
                           (pszTemp[0] == L'\\' && pszTemp[1] == L'\\');
            if (bFsPath)
            {
                StringCchCopyW(pszPath, cchPath, pszTemp);
                CoTaskMemFree(pszTemp);
                return TRUE;
            }
        }
        if (pszTemp)
            CoTaskMemFree(pszTemp);
    }
    return FALSE;
}

static BOOL IsShellItemFolder(IShellItem* psi)
{
    if (!psi)
        return FALSE;

    SFGAOF attrs = SFGAO_FOLDER;
    if (FAILED(psi->GetAttributes(SFGAO_FOLDER, &attrs)))
        return FALSE;

    return (attrs & SFGAO_FOLDER) != 0;
}

static BOOL GetShellItemDisplayName(IShellItem* psi, LPWSTR pszName, size_t cchName)
{
    if (!pszName || cchName == 0)
        return FALSE;

    pszName[0] = L'\0';
    if (!psi)
        return FALSE;

    LPWSTR pszTemp = NULL;
    if (FAILED(psi->GetDisplayName(SIGDN_NORMALDISPLAY, &pszTemp)) || !pszTemp || !pszTemp[0])
    {
        if (pszTemp)
            CoTaskMemFree(pszTemp);
        return FALSE;
    }

    StringCchCopyW(pszName, cchName, pszTemp);
    CoTaskMemFree(pszTemp);
    return TRUE;
}

static BOOL GetParentFolderDisplayNameFromPath(LPCWSTR pszPath, LPWSTR pszOut, size_t cchOut)
{
    if (!pszOut || cchOut == 0)
        return FALSE;

    pszOut[0] = L'\0';
    if (!pszPath || !pszPath[0])
        return FALSE;

    WCHAR szTemp[MAX_PATH] = {};
    StringCchCopyW(szTemp, ARRAYSIZE(szTemp), pszPath);
    PathRemoveFileSpecW(szTemp);
    PathStripPathW(szTemp);

    if (!szTemp[0])
        return FALSE;

    StringCchCopyW(pszOut, cchOut, szTemp);
    return TRUE;
}

static BOOL GetShellItemParentDisplayName(IShellItem* psi, LPWSTR pszOut, size_t cchOut)
{
    if (!pszOut || cchOut == 0)
        return FALSE;

    pszOut[0] = L'\0';
    if (!psi)
        return FALSE;

    WCHAR szPath[MAX_PATH] = {};
    if (GetShellItemPath(psi, szPath, ARRAYSIZE(szPath)) &&
        GetParentFolderDisplayNameFromPath(szPath, pszOut, cchOut))
    {
        return TRUE;
    }

    IShellItem* psiParent = NULL;
    if (SUCCEEDED(psi->GetParent(&psiParent)) && psiParent)
    {
        LPWSTR pszName = NULL;
        if (SUCCEEDED(psiParent->GetDisplayName(SIGDN_NORMALDISPLAY, &pszName)) &&
            pszName && pszName[0])
        {
            StringCchCopyW(pszOut, cchOut, pszName);
            CoTaskMemFree(pszName);
            psiParent->Release();
            return TRUE;
        }

        if (pszName)
            CoTaskMemFree(pszName);
        psiParent->Release();
    }

    return FALSE;
}

static BOOL IsProgramPath(LPCWSTR pszPath)
{
    if (!pszPath || !*pszPath)
        return FALSE;

    LPCWSTR pszExt = PathFindExtensionW(pszPath);
    return pszExt &&
        (_wcsicmp(pszExt, L".exe") == 0 || _wcsicmp(pszExt, L".com") == 0);
}

static BOOL ResolveShortcutTarget(LPCWSTR pszShortcut, LPWSTR pszTarget, DWORD cchTarget)
{
    if (!pszShortcut || !*pszShortcut || !pszTarget || cchTarget == 0)
        return FALSE;

    pszTarget[0] = L'\0';

    IShellLinkW* pShellLink = NULL;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&pShellLink));
    if (FAILED(hr) || !pShellLink)
        return FALSE;

    IPersistFile* pPersistFile = NULL;
    hr = pShellLink->QueryInterface(IID_PPV_ARGS(&pPersistFile));
    if (SUCCEEDED(hr) && pPersistFile)
    {
        hr = pPersistFile->Load(pszShortcut, STGM_READ);
        if (SUCCEEDED(hr))
        {
            WIN32_FIND_DATAW wfd = {};
            hr = pShellLink->GetPath(pszTarget, (int)cchTarget, &wfd, SLGP_RAWPATH);
        }
        pPersistFile->Release();
    }

    pShellLink->Release();
    return SUCCEEDED(hr) && pszTarget[0] != L'\0';
}

static BOOL IsShortcutToProgram(LPCWSTR pszShortcut, LPWSTR pszProgramName, DWORD cchProgramName)
{
    if (!pszShortcut || !*pszShortcut)
        return FALSE;

    LPCWSTR pszExt = PathFindExtensionW(pszShortcut);
    if (!pszExt || _wcsicmp(pszExt, L".lnk") != 0)
        return FALSE;

    WCHAR szTarget[MAX_PATH] = {};
    if (!ResolveShortcutTarget(pszShortcut, szTarget, ARRAYSIZE(szTarget)))
        return FALSE;

    LPCWSTR pszTargetExt = PathFindExtensionW(szTarget);
    if (!pszTargetExt)
        return FALSE;

    BOOL bProgram =
        _wcsicmp(pszTargetExt, L".exe") == 0 ||
        _wcsicmp(pszTargetExt, L".com") == 0 ||
        _wcsicmp(pszTargetExt, L".bat") == 0 ||
        _wcsicmp(pszTargetExt, L".cmd") == 0 ||
        _wcsicmp(pszTargetExt, L".pif") == 0;

    if (!bProgram)
        return FALSE;

    if (pszProgramName && cchProgramName)
    {
        StringCchCopyW(pszProgramName, cchProgramName, PathFindFileNameW(szTarget));
    }

    return TRUE;
}

static void UIntToCommaString(UINT value, LPWSTR pszBuf, size_t cchBuf)
{
    WCHAR szTemp[32];
    StringCchPrintfW(szTemp, ARRAYSIZE(szTemp), L"%u", value);

    NUMBERFMTW fmt = {};
    fmt.NumDigits = 0;
    fmt.LeadingZero = 0;
    fmt.Grouping = 3;
    fmt.lpDecimalSep = (LPWSTR)L".";
    fmt.lpThousandSep = (LPWSTR)L",";
    fmt.NegativeOrder = 1;

    if (!GetNumberFormatW(LOCALE_USER_DEFAULT, 0, szTemp, &fmt, pszBuf, (int)cchBuf))
        StringCchCopyW(pszBuf, cchBuf, szTemp);
}

static void ExpandPercentTokens(LPCWSTR pszTemplate,
                                LPCWSTR pszArg1,
                                LPCWSTR pszArg2,
                                LPCWSTR pszArg3,
                                LPWSTR pszOut,
                                size_t cchOut)
{
    if (!pszOut || !cchOut)
        return;

    pszOut[0] = L'\0';
    if (!pszTemplate)
        return;

    size_t cchWritten = 0;
    for (const WCHAR* p = pszTemplate; *p && cchWritten + 1 < cchOut;)
    {
        LPCWSTR pszInsert = NULL;
        if (p[0] == L'%' && p[1] >= L'1' && p[1] <= L'3')
        {
            switch (p[1])
            {
            case L'1': pszInsert = pszArg1; break;
            case L'2': pszInsert = pszArg2; break;
            case L'3': pszInsert = pszArg3; break;
            }
            p += 2;
        }
        else
        {
            pszOut[cchWritten++] = *p++;
            pszOut[cchWritten] = L'\0';
            continue;
        }

        if (!pszInsert)
            pszInsert = L"";

        size_t cchRemaining = cchOut - cchWritten;
        HRESULT hr = StringCchCopyNW(pszOut + cchWritten, cchRemaining,
                                     pszInsert, cchRemaining - 1);
        if (FAILED(hr))
            break;
        cchWritten = wcslen(pszOut);
    }
}

static void GetDisplayNameForDialog(HWND hDlg,
                                    UINT idTextControl,
                                    LPCWSTR pszPath,
                                    LPCWSTR pszFallbackName,
                                    DWORD dwAttrs,
                                    LPWSTR pszOut,
                                    size_t cchOut)
{
    pszOut[0] = L'\0';
    if (!pszPath && !pszFallbackName)
        return;

    if (pszPath && *pszPath)
    {
        SHFILEINFOW sfi = {};
        DWORD dwFlags = SHGFI_DISPLAYNAME;
        if (dwAttrs != INVALID_FILE_ATTRIBUTES)
            dwFlags |= SHGFI_USEFILEATTRIBUTES;

        if (SHGetFileInfoW(pszPath,
                           dwAttrs == INVALID_FILE_ATTRIBUTES ? 0 : dwAttrs,
                           &sfi, sizeof(sfi), dwFlags) && sfi.szDisplayName[0])
        {
            StringCchCopyW(pszOut, cchOut, sfi.szDisplayName);
        }
        else
        {
            StringCchCopyW(pszOut, cchOut, PathFindFileNameW(pszPath));
        }
    }
    else if (pszFallbackName)
    {
        StringCchCopyW(pszOut, cchOut, pszFallbackName);
    }

    HWND hText = GetDlgItem(hDlg, idTextControl);
    if (hText && pszOut[0])
    {
        RECT rc;
        GetWindowRect(hText, &rc);
        MapWindowRect(NULL, hDlg, &rc);

        HDC hdc = GetDC(hDlg);
        HFONT hFont = (HFONT)SendMessageW(hDlg, WM_GETFONT, 0, 0);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        PathCompactPathW(hdc, pszOut, rc.right - rc.left - 8);
        SelectObject(hdc, hOldFont);
        ReleaseDC(hDlg, hdc);
    }
}

// Count printf "%s"/"%S" placeholders in a template (ignoring "%%"). Used to
// pick the right argument set for a date/size line whose placeholder count
// varies by shell32 version.
static int CountFormatStringArgs(LPCWSTR pszFmt)
{
    int count = 0;
    for (const WCHAR* p = pszFmt; *p; )
    {
        if (p[0] == L'%')
        {
            if (p[1] == L'%') { p += 2; continue; }
            if (p[1] == L's' || p[1] == L'S')
                count++;
            p += p[1] ? 2 : 1;
        }
        else
        {
            p++;
        }
    }
    return count;
}

// Safe string-only template expander. Handles "%s"/"%S" (sequential args),
// "%1".."%9" (positional args), and "%%" (literal percent); any other
// specifier is copied verbatim. Unlike wsprintf/StringCchPrintf it never
// dereferences a missing argument, so a version-mismatched resource template
// cannot fault the process.
static void ExpandStringTemplate(LPCWSTR pszFmt, LPCWSTR* args, int nArgs,
                                 LPWSTR pszOut, size_t cchOut)
{
    if (!pszOut || !cchOut)
        return;

    pszOut[0] = L'\0';
    if (!pszFmt)
        return;

    size_t w = 0;
    int nextSeq = 0;
    for (const WCHAR* p = pszFmt; *p && w + 1 < cchOut; )
    {
        LPCWSTR pszInsert = NULL;
        WCHAR ch[2] = { 0, 0 };

        if (p[0] == L'%' && (p[1] == L's' || p[1] == L'S'))
        {
            pszInsert = (nextSeq < nArgs) ? args[nextSeq] : L"";
            nextSeq++;
            p += 2;
        }
        else if (p[0] == L'%' && p[1] >= L'1' && p[1] <= L'9')
        {
            int idx = p[1] - L'1';
            pszInsert = (idx < nArgs) ? args[idx] : L"";
            p += 2;
        }
        else if (p[0] == L'%' && p[1] == L'%')
        {
            ch[0] = L'%';
            pszInsert = ch;
            p += 2;
        }
        else
        {
            pszOut[w++] = *p++;
            pszOut[w] = L'\0';
            continue;
        }

        if (pszInsert && *pszInsert)
        {
            size_t rem = cchOut - w;
            if (FAILED(StringCchCopyNW(pszOut + w, rem, pszInsert, rem - 1)))
                break;
            w = wcslen(pszOut);
        }
    }
}

static void BuildDateLine(LPCWSTR pszPath, LPWSTR pszBuf, size_t cchBuf)
{
    pszBuf[0] = L'\0';
    if (!pszPath || !*pszPath)
        return;

    WIN32_FIND_DATAW wfd = {};
    HANDLE hFind = FindFirstFileW(pszPath, &wfd);
    if (hFind == INVALID_HANDLE_VALUE)
        return;
    FindClose(hFind);

    ULONGLONG ullSize = ((ULONGLONG)wfd.nFileSizeHigh << 32) | wfd.nFileSizeLow;
    WCHAR szSize[64] = {};
    StrFormatByteSizeW((LONGLONG)ullSize, szSize, ARRAYSIZE(szSize));

    if (!(wfd.ftLastWriteTime.dwLowDateTime || wfd.ftLastWriteTime.dwHighDateTime))
    {
        StringCchCopyW(pszBuf, cchBuf, szSize);
        return;
    }

    // XP's IDS_DATESIZELINE is "%s\nmodified: %s" (size, combined date+time);
    // Windows 9x uses "%s\nmodified on %s, %s" (size, date, time). The template
    // is loaded from whichever shell32 the user configured, so build every
    // piece and let the placeholder count decide which arguments to feed.
    WCHAR szDateTime[128] = {}, szDate[128] = {}, szTime[128] = {};
    DWORD dwDateTime = FDTF_LONGDATE | FDTF_RELATIVE | FDTF_LONGTIME;
    DWORD dwDate = FDTF_LONGDATE | FDTF_RELATIVE;
    DWORD dwTime = FDTF_LONGTIME;
    SHFormatDateTimeW(&wfd.ftLastWriteTime, &dwDateTime, szDateTime, ARRAYSIZE(szDateTime));
    SHFormatDateTimeW(&wfd.ftLastWriteTime, &dwDate, szDate, ARRAYSIZE(szDate));
    SHFormatDateTimeW(&wfd.ftLastWriteTime, &dwTime, szTime, ARRAYSIZE(szTime));

    WCHAR szTemplate[128] = {};
    LoadXPString(IDS_DATESIZELINE, szTemplate, ARRAYSIZE(szTemplate), L"%s\nmodified: %s");

    // Never hand this foreign template to wsprintf/StringCchPrintf: the 9x
    // string carries three "%s" but XP's code (which this mirrors) passes only
    // two arguments, so the extra "%s" reads a bogus stack pointer and faults
    // in the CRT. Expand it ourselves and pick the arguments by placeholder
    // count — three placeholders get (size, date, time), otherwise
    // (size, date+time) as XP intends.
    int cArgs = CountFormatStringArgs(szTemplate);
    LPCWSTR args[3];
    if (cArgs >= 3)
    {
        args[0] = szSize; args[1] = szDate; args[2] = szTime;
    }
    else
    {
        args[0] = szSize; args[1] = szDateTime; args[2] = szTime;
    }
    ExpandStringTemplate(szTemplate, args, 3, pszBuf, cchBuf);
}

static HICON LoadFileDialogIcon(LPCWSTR pszPath, DWORD dwAttrs)
{
    SHFILEINFOW sfi = {};
    DWORD dwFlags = SHGFI_ICON | SHGFI_LARGEICON;
    if (dwAttrs != INVALID_FILE_ATTRIBUTES)
        dwFlags |= SHGFI_USEFILEATTRIBUTES;

    if (SHGetFileInfoW(pszPath ? pszPath : L"",
                       dwAttrs == INVALID_FILE_ATTRIBUTES ? 0 : dwAttrs,
                       &sfi, sizeof(sfi), dwFlags))
    {
        return sfi.hIcon;
    }

    return NULL;
}

static HICON LoadRecycleBinIcon()
{
    LPITEMIDLIST pidl = NULL;
    if (FAILED(SHGetSpecialFolderLocation(NULL, CSIDL_BITBUCKET, &pidl)) || !pidl)
        return NULL;

    SHFILEINFOW sfi = {};
    BOOL ok = SHGetFileInfoW((LPCWSTR)pidl, 0, &sfi, sizeof(sfi),
                             SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON);
    ILFree(pidl);
    return ok ? sfi.hIcon : NULL;
}

static HICON LoadXPDialogIcon(UINT id)
{
    if (!g_hXPShell32)
        return NULL;

    return (HICON)LoadImageW(g_hXPShell32, MAKEINTRESOURCEW(id), IMAGE_ICON,
                             0, 0, LR_DEFAULTCOLOR);
}

// Skip a sz_Or_Ord field in a dialog template; returns the new offset, or 0 on
// overrun of the resource bounds.
static size_t SkipSzOrOrd(const BYTE* p, size_t off, size_t cb)
{
    if (off + sizeof(WORD) > cb)
        return 0;
    WORD w = *(const WORD*)(p + off);
    if (w == 0xFFFF)
        return (off + 4 <= cb) ? off + 4 : 0;

    // string: skip through the terminating NUL word
    while (off + sizeof(WORD) <= cb)
    {
        w = *(const WORD*)(p + off);
        off += 2;
        if (w == 0)
            return off;
    }
    return 0;
}

// Walk a DLGTEMPLATE/DLGTEMPLATEEX and return the icon resource ordinal
// referenced by the first SS_ICON static whose title is an ordinal (the
// dialog's stock icon, e.g. IDI_REPLACE_FOLDER). The dialog is created with
// hInstance=NULL, so the dialog manager cannot resolve this ordinal itself;
// we load it from the configured shell32 instead. Returns 0 if none found.
static UINT GetTemplateMainIconOrdinal(LPCDLGTEMPLATEW pTemplate, DWORD cb)
{
    const BYTE* p = (const BYTE*)pTemplate;
    if (!p || cb < sizeof(DLGTEMPLATE))
        return 0;

    const WORD* pw = (const WORD*)p;
    BOOL fEx = (pw[0] == 1 && pw[1] == 0xFFFF);
    DWORD dlgStyle;
    UINT cItems;
    size_t off;

    if (fEx)
    {
        if (cb < 26) return 0;
        dlgStyle = *(const DWORD*)(p + 12);
        cItems = *(const WORD*)(p + 16);
        off = 26;
    }
    else
    {
        dlgStyle = *(const DWORD*)p;
        cItems = *(const WORD*)(p + 8);
        off = 18;
    }

    // menu, window class, title
    for (int i = 0; i < 3 && off; i++)
        off = SkipSzOrOrd(p, off, cb);
    if (!off)
        return 0;

    if (dlgStyle & DS_SETFONT)
    {
        off += fEx ? 6 : 2;            // point size (+ weight/italic/charset)
        off = SkipSzOrOrd(p, off, cb); // face name
        if (!off)
            return 0;
    }

    for (UINT i = 0; i < cItems; i++)
    {
        off = (off + 3) & ~(size_t)3;
        size_t cbItemHdr = fEx ? 24 : 18;
        if (off + cbItemHdr > cb)
            return 0;

        DWORD ctrlStyle = fEx ? *(const DWORD*)(p + off + 8)
                              : *(const DWORD*)(p + off);
        off += cbItemHdr;

        // control class
        BOOL fStatic = FALSE;
        if (off + sizeof(WORD) > cb)
            return 0;
        const WORD* pwClass = (const WORD*)(p + off);
        if (pwClass[0] == 0xFFFF)
            fStatic = (off + 4 <= cb) && (pwClass[1] == 0x0082);
        else
            fStatic = (wcsncmp((LPCWSTR)pwClass, L"Static",
                               (cb - off) / sizeof(WCHAR)) == 0);
        off = SkipSzOrOrd(p, off, cb);
        if (!off)
            return 0;

        // control title (ordinal for template icons)
        if (off + sizeof(WORD) > cb)
            return 0;
        const WORD* pwTitle = (const WORD*)(p + off);
        UINT iconOrdinal = (pwTitle[0] == 0xFFFF && off + 4 <= cb) ? pwTitle[1] : 0;
        off = SkipSzOrOrd(p, off, cb);
        if (!off || off + sizeof(WORD) > cb)
            return 0;

        // creation data
        WORD cbExtra = *(const WORD*)(p + off);
        off += 2 + cbExtra;
        if (off > cb)
            return 0;

        if (fStatic && (ctrlStyle & SS_TYPEMASK) == SS_ICON && iconOrdinal)
            return iconOrdinal;
    }

    return 0;
}

// Find the template's stock icon static (the replace dialogs give it no
// usable control id, so locate it by class and style instead).
static HWND FindTemplateIconStatic(HWND hDlg)
{
    for (HWND hChild = ::GetWindow(hDlg, GW_CHILD); hChild;
         hChild = ::GetWindow(hChild, GW_HWNDNEXT))
    {
        int id = GetDlgCtrlID(hChild);
        if (id == IDD_ICON_OLD || id == IDD_ICON_NEW || id == IDD_ICON_WASTEBASKET)
            continue;

        WCHAR szClass[16];
        if (!GetClassNameW(hChild, szClass, ARRAYSIZE(szClass)) ||
            lstrcmpiW(szClass, L"Static") != 0)
            continue;

        if ((GetWindowLongW(hChild, GWL_STYLE) & SS_TYPEMASK) == SS_ICON)
            return hChild;
    }
    return NULL;
}

static int MoveDlgItem(HWND hDlg, UINT id, int y)
{
    RECT rc;
    HWND hwnd = GetDlgItem(hDlg, id);
    if (hwnd)
    {
        GetWindowRect(hwnd, &rc);
        MapWindowRect(NULL, hDlg, &rc);
        SetWindowPos(hwnd, NULL, rc.left, y, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        return rc.top - y;
    }
    return 0;
}

static void ShrinkDialog(HWND hDlg, UINT idText)
{
    HWND hwndText = GetDlgItem(hDlg, idText);
    if (!hwndText)
        return;

    RECT rc;
    GetWindowRect(hwndText, &rc);
    MapWindowRect(NULL, hDlg, &rc);
    int y = rc.bottom + 12;

    MoveDlgItem(hDlg, IDNO, y);
    MoveDlgItem(hDlg, IDCANCEL, y);
    MoveDlgItem(hDlg, IDD_YESTOALL, y);
    y = MoveDlgItem(hDlg, IDYES, y);

    GetWindowRect(hDlg, &rc);
    SetWindowPos(hDlg, NULL, 0, 0,
                 rc.right - rc.left,
                 rc.bottom - y - rc.top,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

static void HideConfirmButtons(HWND hDlg, int idHide)
{
    HWND hwndCancel = GetDlgItem(hDlg, IDCANCEL);
    HWND hwndYesToAll = GetDlgItem(hDlg, IDD_YESTOALL);
    if (!hwndCancel)
        return;

    RECT rcCancel;
    GetWindowRect(hwndCancel, &rcCancel);

    HWND hwndNo = GetDlgItem(hDlg, IDNO);
    if (hwndNo)
    {
        RECT rcNo;
        GetWindowRect(hwndNo, &rcNo);
        MapWindowRect(NULL, hDlg, &rcCancel);
        SetWindowPos(hwndNo, NULL, rcCancel.left, rcCancel.top, 0, 0,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);

        HWND hwndYes = GetDlgItem(hDlg, IDYES);
        if (hwndYes)
        {
            MapWindowRect(NULL, hDlg, &rcNo);
            SetWindowPos(hwndYes, NULL, rcNo.left, rcNo.top, 0, 0,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
        }
    }

    if (hwndYesToAll)
        DestroyWindow(hwndYesToAll);
    DestroyWindow(GetDlgItem(hDlg, idHide));
}

#define HideYesToAllAndCancel(hdlg) HideConfirmButtons(hdlg, IDCANCEL)

static void OpenAddRemovePrograms()
{
    ShellExecuteW(NULL, L"open", L"control.exe", L"appwiz.cpl", NULL, SW_SHOWNORMAL);
}

static UINT PickDeleteTextControl(UINT stconfirm, DWORD dwAttrs, BOOL bProgram)
{
    BOOL bPermanent =
        stconfirm == STCONFIRM_PERMANENTDELETE_STREAM ||
        stconfirm == STCONFIRM_PERMANENTDELETE_STORAGE ||
        stconfirm == STCONFIRM_PERMANENTDELETE_TOOLARGE_STREAM ||
        stconfirm == STCONFIRM_PERMANENTDELETE_TOOLARGE_STORAGE ||
        stconfirm == STCONFIRM_PERMANENTDELETE_SYSTEM_STREAM ||
        stconfirm == STCONFIRM_PERMANENTDELETE_PROGRAM_STREAM ||
        stconfirm == STCONFIRM_PERMANENTDELETE_SHORTCUT ||
        stconfirm == STCONFIRM_PERMANENTDELETE_ARP;

    if (bProgram)
        return IDD_TEXT3;
    if ((dwAttrs & FILE_ATTRIBUTE_SYSTEM) != 0)
        return IDD_TEXT2;
    if ((dwAttrs & FILE_ATTRIBUTE_READONLY) != 0)
        return IDD_TEXT1;
    if (bPermanent)
        return IDD_TEXT4;
    return IDD_TEXT;
}

static UINT PickFolderDeleteTextControl(DWORD dwAttrs)
{
    if ((dwAttrs & FILE_ATTRIBUTE_SYSTEM) != 0)
        return IDD_TEXT2;
    if ((dwAttrs & FILE_ATTRIBUTE_READONLY) != 0)
        return IDD_TEXT1;
    return IDD_TEXT;
}

static BOOL IsDeleteSingleConfirm(UINT stconfirm)
{
    switch (stconfirm)
    {
    case STCONFIRM_RECYCLE_STREAM:
    case STCONFIRM_RECYCLE_STORAGE:
    case STCONFIRM_RECYCLE_SYSTEM_STREAM:
    case STCONFIRM_RECYCLE_PROGRAM_STREAM:
    case STCONFIRM_RECYCLE_ARP:
    case STCONFIRM_RECYCLE_SHORTCUT:
    case STCONFIRM_PERMANENTDELETE_STREAM:
    case STCONFIRM_PERMANENTDELETE_STORAGE:
    case STCONFIRM_PERMANENTDELETE_TOOLARGE_STREAM:
    case STCONFIRM_PERMANENTDELETE_TOOLARGE_STORAGE:
    case STCONFIRM_PERMANENTDELETE_SYSTEM_STREAM:
    case STCONFIRM_PERMANENTDELETE_PROGRAM_STREAM:
    case STCONFIRM_PERMANENTDELETE_ARP:
    case STCONFIRM_PERMANENTDELETE_SHORTCUT:
    case STCONFIRM_PERMANENTDELETE_PATH_TOO_LONG:
        return TRUE;
    }

    return FALSE;
}

static BOOL IsDeleteMultipleConfirm(UINT stconfirm)
{
    return stconfirm == STCONFIRM_RECYCLE_MULTIPLE ||
           stconfirm == STCONFIRM_PERMANENTDELETE_MULTIPLE ||
           stconfirm == STCONFIRM_PERMANENTDELETE_TOOLARGE_MULTIPLE;
}

static BOOL IsReplaceConfirm(UINT stconfirm)
{
    return stconfirm == STCONFIRM_REPLACE_STREAM ||
           stconfirm == STCONFIRM_REPLACE_STORAGE ||
           stconfirm == STCONFIRM_REPLACE_READONLY_STREAM ||
           stconfirm == STCONFIRM_REPLACE_SYSTEM_STREAM;
}

static BOOL BuildClassicConfirmContext(const CONFIRMOP_MIN* pConfirmOp,
                                       CLASSIC_CONFIRM_CONTEXT* pCtx)
{
    ZeroMemory(pCtx, sizeof(*pCtx));
    pCtx->hwndParent = pConfirmOp->hwndParent;
    pCtx->stconfirm = pConfirmOp->stconfirm;
    pCtx->itemCount = pConfirmOp->itemCount;

    if (IsDeleteMultipleConfirm(pConfirmOp->stconfirm))
    {
        pCtx->dlgId = DLG_DELETE_MULTIPLE;
        pCtx->recycle = (pConfirmOp->stconfirm == STCONFIRM_RECYCLE_MULTIPLE);
        pCtx->activeTextId = pCtx->recycle ? IDD_TEXT : IDD_TEXT4;
        pCtx->showCancel = FALSE;
        pCtx->showYesToAll = FALSE;
        pCtx->treatNoAsCancel = TRUE;
        if (pCtx->itemCount == 0)
            pCtx->itemCount = 2;
        return HasXPDialogResource(pCtx->dlgId);
    }

    if (IsReplaceConfirm(pConfirmOp->stconfirm))
    {
        LPWSTR pszSourcePath = NULL, pszSourceName = NULL;
        LPWSTR pszTargetPath = NULL, pszTargetName = NULL;
        GetShellItemStrings(pConfirmOp->psiSource, &pszSourcePath, &pszSourceName);
        GetShellItemStrings(pConfirmOp->psiTarget, &pszTargetPath, &pszTargetName);
        if (!pszSourcePath || !pszTargetPath)
        {
            if (pszSourcePath) CoTaskMemFree(pszSourcePath);
            if (pszSourceName) CoTaskMemFree(pszSourceName);
            if (pszTargetPath) CoTaskMemFree(pszTargetPath);
            if (pszTargetName) CoTaskMemFree(pszTargetName);
            return FALSE;
        }

        StrSetW(&pCtx->pszSourcePath, pszSourcePath);
        StrSetW(&pCtx->pszSourceName, pszSourceName);
        StrSetW(&pCtx->pszTargetPath, pszTargetPath);
        StrSetW(&pCtx->pszTargetName, pszTargetName);
        CoTaskMemFree(pszSourcePath);
        CoTaskMemFree(pszSourceName);
        CoTaskMemFree(pszTargetPath);
        CoTaskMemFree(pszTargetName);

        pCtx->sourceAttrs = GetPathAttributesOrDefault(pCtx->pszSourcePath, FILE_ATTRIBUTE_NORMAL);
        pCtx->destAttrs = GetPathAttributesOrDefault(pCtx->pszTargetPath, FILE_ATTRIBUTE_NORMAL);
        BOOL bFolder = ((pCtx->destAttrs & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
                       pConfirmOp->stconfirm == STCONFIRM_REPLACE_STORAGE;

        pCtx->dlgId = bFolder ? DLG_REPLACE_FOLDER : DLG_REPLACE_FILE;
        pCtx->activeTextId = IDD_TEXT;
        pCtx->showCancel = TRUE;
        pCtx->showYesToAll = TRUE;
        pCtx->showDates = !bFolder;
        pCtx->shrinkDialog = FALSE;

        if (!bFolder)
        {
            if (pConfirmOp->stconfirm == STCONFIRM_REPLACE_SYSTEM_STREAM ||
                (pCtx->destAttrs & FILE_ATTRIBUTE_SYSTEM) != 0)
            {
                pCtx->activeTextId = IDD_TEXT2;
            }
            else if (pConfirmOp->stconfirm == STCONFIRM_REPLACE_READONLY_STREAM ||
                     (pCtx->destAttrs & FILE_ATTRIBUTE_READONLY) != 0)
            {
                pCtx->activeTextId = IDD_TEXT1;
            }
        }

        return HasXPDialogResource(pCtx->dlgId);
    }

    if (!IsDeleteSingleConfirm(pConfirmOp->stconfirm))
        return FALSE;

    LPWSTR pszSourcePath = NULL, pszSourceName = NULL;
    GetShellItemStrings(pConfirmOp->psiSource, &pszSourcePath, &pszSourceName);
    StrSetW(&pCtx->pszSourcePath, pszSourcePath);
    StrSetW(&pCtx->pszSourceName, pszSourceName);
    if (pszSourcePath) CoTaskMemFree(pszSourcePath);
    if (pszSourceName) CoTaskMemFree(pszSourceName);

    if (!pCtx->pszSourcePath && !pCtx->pszSourceName)
        return FALSE;

    pCtx->sourceAttrs = GetPathAttributesOrDefault(
        pCtx->pszSourcePath,
        (pConfirmOp->stconfirm == STCONFIRM_RECYCLE_STORAGE ||
         pConfirmOp->stconfirm == STCONFIRM_PERMANENTDELETE_STORAGE ||
         pConfirmOp->stconfirm == STCONFIRM_PERMANENTDELETE_TOOLARGE_STORAGE ||
         pConfirmOp->stconfirm == STCONFIRM_PERMANENTDELETE_PATH_TOO_LONG)
            ? FILE_ATTRIBUTE_DIRECTORY
            : FILE_ATTRIBUTE_NORMAL);

    BOOL bFolder = (pCtx->sourceAttrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
    BOOL bRecycle =
        pConfirmOp->stconfirm == STCONFIRM_RECYCLE_STREAM ||
        pConfirmOp->stconfirm == STCONFIRM_RECYCLE_STORAGE ||
        pConfirmOp->stconfirm == STCONFIRM_RECYCLE_SYSTEM_STREAM ||
        pConfirmOp->stconfirm == STCONFIRM_RECYCLE_PROGRAM_STREAM ||
        pConfirmOp->stconfirm == STCONFIRM_RECYCLE_ARP ||
        pConfirmOp->stconfirm == STCONFIRM_RECYCLE_SHORTCUT;

    pCtx->recycle = bRecycle;
    pCtx->showCancel = FALSE;
    pCtx->showYesToAll = FALSE;
    pCtx->showDates = FALSE;
    pCtx->shrinkDialog = TRUE;

    if (pConfirmOp->stconfirm == STCONFIRM_PERMANENTDELETE_PATH_TOO_LONG)
    {
        pCtx->dlgId = DLG_PATH_TOO_LONG;
        pCtx->activeTextId = IDD_TEXT;
        pCtx->treatNoAsCancel = TRUE;
        return HasXPDialogResource(pCtx->dlgId);
    }

    if (!bFolder && g_showShortcutDeleteDialog && pCtx->pszSourcePath &&
        (pConfirmOp->stconfirm == STCONFIRM_RECYCLE_ARP ||
         pConfirmOp->stconfirm == STCONFIRM_PERMANENTDELETE_ARP ||
         pConfirmOp->stconfirm == STCONFIRM_RECYCLE_SHORTCUT ||
         pConfirmOp->stconfirm == STCONFIRM_PERMANENTDELETE_SHORTCUT))
    {
        WCHAR szProgramName[MAX_PATH] = {};
        if (IsShortcutToProgram(pCtx->pszSourcePath, szProgramName, ARRAYSIZE(szProgramName)) &&
            HasXPDialogResource(DLG_DELETE_FILE_ARP))
        {
            pCtx->dlgId = DLG_DELETE_FILE_ARP;
            pCtx->activeTextId = IDD_ARPWARNINGTEXT;
            pCtx->shrinkDialog = FALSE;
            pCtx->treatNoAsCancel = TRUE;
            StrSetW(&pCtx->pszShortcutProgramName,
                    szProgramName[0] ? szProgramName : L"the program");
            return TRUE;
        }
    }

    if (bFolder)
    {
        if (pConfirmOp->stconfirm == STCONFIRM_PERMANENTDELETE_TOOLARGE_STORAGE)
            pCtx->dlgId = DLG_WONT_RECYCLE_FOLDER;
        else
            pCtx->dlgId = DLG_DELETE_FOLDER;

        pCtx->activeTextId = PickFolderDeleteTextControl(pCtx->sourceAttrs);
        pCtx->treatNoAsCancel = TRUE;

        WCHAR szWarning[128];
        LoadXPString(bRecycle ? IDS_FOLDERRECYCLEWARNING : IDS_FOLDERDELETEWARNING,
                     szWarning, ARRAYSIZE(szWarning),
                     bRecycle ? L"all of its contents to the Recycle Bin?"
                              : L"all of its contents?");
        StrSetW(&pCtx->pszWarningText, szWarning);
    }
    else
    {
        BOOL bProgram = g_showProgramDeleteDialog && pCtx->pszSourcePath &&
                        IsProgramPath(pCtx->pszSourcePath);

        if (pConfirmOp->stconfirm == STCONFIRM_PERMANENTDELETE_TOOLARGE_STREAM)
            pCtx->dlgId = DLG_WONT_RECYCLE_FILE;
        else
            pCtx->dlgId = DLG_DELETE_FILE;

        pCtx->activeTextId = PickDeleteTextControl(pConfirmOp->stconfirm,
                                                   pCtx->sourceAttrs,
                                                   bProgram);
        pCtx->treatNoAsCancel = TRUE;

        WCHAR szWarning[128];
        LoadXPString(bRecycle ? IDS_FILERECYCLEWARNING : IDS_FILEDELETEWARNING,
                     szWarning, ARRAYSIZE(szWarning),
                     bRecycle ? L"Are you sure you want to send it to the Recycle Bin?"
                              : L"Are you sure you want to delete it?");
        StrSetW(&pCtx->pszWarningText, szWarning);
    }

    return HasXPDialogResource(pCtx->dlgId);
}

static BOOL BuildClassicConflictConfirmContext(
    HWND hwndParent,
    ISyncMgrConflictItemsMin* pConflictItems,
    CLASSIC_CONFIRM_CONTEXT* pCtx,
    UINT* pSourceIndex,
    UINT* pTargetIndex)
{
    if (!pConflictItems || !pCtx || !pSourceIndex || !pTargetIndex)
        return FALSE;

    BOOL bSuccess = FALSE;
    UINT cItems = 0;
    CONFIRM_CONFLICT_ITEM_MIN items[2] = {};
    UINT sourceIndex = 0;
    UINT targetIndex = 0;
    LPWSTR pszSourcePath = NULL;
    LPWSTR pszSourceName = NULL;
    LPWSTR pszTargetPath = NULL;
    LPWSTR pszTargetName = NULL;
    BOOL bFolder = FALSE;

    ZeroMemory(pCtx, sizeof(*pCtx));
    pCtx->hwndParent = hwndParent;

    if (FAILED(pConflictItems->GetCount(&cItems)) || cItems != 2)
        goto Cleanup;

    if (FAILED(pConflictItems->GetItem(0, &items[0])) ||
        FAILED(pConflictItems->GetItem(1, &items[1])))
    {
        goto Cleanup;
    }

    if (items[0].pszAlternateName && !items[1].pszAlternateName)
        sourceIndex = 0;
    else if (!items[0].pszAlternateName && items[1].pszAlternateName)
        sourceIndex = 1;
    else
        sourceIndex = 0;

    targetIndex = sourceIndex ^ 1;

    GetShellItemStrings(items[sourceIndex].pShellItem, &pszSourcePath, &pszSourceName);
    GetShellItemStrings(items[targetIndex].pShellItem, &pszTargetPath, &pszTargetName);

    if (!pszSourcePath || !pszTargetPath)
        goto Cleanup;

    StrSetW(&pCtx->pszSourcePath, pszSourcePath);
    StrSetW(&pCtx->pszTargetPath, pszTargetPath);
    StrSetW(&pCtx->pszSourceName,
            items[sourceIndex].pszOriginalName ? items[sourceIndex].pszOriginalName : pszSourceName);
    StrSetW(&pCtx->pszTargetName,
            items[targetIndex].pszOriginalName ? items[targetIndex].pszOriginalName : pszTargetName);

    pCtx->sourceAttrs = GetPathAttributesOrDefault(pCtx->pszSourcePath, FILE_ATTRIBUTE_NORMAL);
    pCtx->destAttrs = GetPathAttributesOrDefault(pCtx->pszTargetPath, FILE_ATTRIBUTE_NORMAL);

    bFolder = (pCtx->destAttrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
    pCtx->dlgId = bFolder ? DLG_REPLACE_FOLDER : DLG_REPLACE_FILE;
    pCtx->activeTextId = IDD_TEXT;
    pCtx->showCancel = FALSE;
    pCtx->showYesToAll = TRUE;
    pCtx->showDates = !bFolder;
    pCtx->shrinkDialog = FALSE;

    if (!bFolder)
    {
        if ((pCtx->destAttrs & FILE_ATTRIBUTE_SYSTEM) != 0)
            pCtx->activeTextId = IDD_TEXT2;
        else if ((pCtx->destAttrs & FILE_ATTRIBUTE_READONLY) != 0)
            pCtx->activeTextId = IDD_TEXT1;
    }

    *pSourceIndex = sourceIndex;
    *pTargetIndex = targetIndex;
    bSuccess = HasXPDialogResource(pCtx->dlgId);

Cleanup:
    CoTaskMemFree(pszSourcePath);
    CoTaskMemFree(pszSourceName);
    CoTaskMemFree(pszTargetPath);
    CoTaskMemFree(pszTargetName);
    FreeConfirmConflictItemMin(&items[0]);
    FreeConfirmConflictItemMin(&items[1]);
    if (!bSuccess)
        FreeClassicConfirmContext(pCtx);
    return bSuccess;
}

static void InitClassicConfirmDialog(HWND hDlg, CLASSIC_CONFIRM_CONTEXT* pCtx)
{
    if (!pCtx)
        return;

    if (!GetDlgItem(hDlg, pCtx->activeTextId))
    {
        if (GetDlgItem(hDlg, IDD_TEXT))
            pCtx->activeTextId = IDD_TEXT;
    }

    if (!pCtx->showCancel && GetDlgItem(hDlg, IDCANCEL) && GetDlgItem(hDlg, IDD_YESTOALL))
        HideYesToAllAndCancel(hDlg);

    WCHAR szArg1[512] = {};
    WCHAR szArg2[512] = {};
    WCHAR szArg3[512] = {};

    if (pCtx->dlgId == DLG_DELETE_MULTIPLE)
    {
        UIntToCommaString(pCtx->itemCount, szArg1, ARRAYSIZE(szArg1));
    }
    else if (pCtx->dlgId == DLG_DELETE_FILE_ARP)
    {
        StringCchCopyW(szArg1, ARRAYSIZE(szArg1),
                       pCtx->pszShortcutProgramName ? pCtx->pszShortcutProgramName : L"the program");
    }
    else
    {
        GetDisplayNameForDialog(hDlg, pCtx->activeTextId,
                                pCtx->pszSourcePath,
                                pCtx->pszSourceName,
                                pCtx->sourceAttrs,
                                szArg1, ARRAYSIZE(szArg1));
    }

    if (pCtx->pszTargetPath || pCtx->pszTargetName)
    {
        GetDisplayNameForDialog(hDlg, pCtx->activeTextId,
                                pCtx->pszTargetPath,
                                pCtx->pszTargetName,
                                pCtx->destAttrs,
                                szArg2, ARRAYSIZE(szArg2));
    }

    if (pCtx->pszWarningText)
        StringCchCopyW(szArg3, ARRAYSIZE(szArg3), pCtx->pszWarningText);

    if (pCtx->showDates)
    {
        WCHAR szOld[256] = {};
        WCHAR szNew[256] = {};
        BuildDateLine(pCtx->pszTargetPath, szOld, ARRAYSIZE(szOld));
        BuildDateLine(pCtx->pszSourcePath, szNew, ARRAYSIZE(szNew));
        SetDlgItemTextW(hDlg, IDD_FILEINFO_OLD, szOld);
        SetDlgItemTextW(hDlg, IDD_FILEINFO_NEW, szNew);

        HICON hOldIcon = LoadFileDialogIcon(pCtx->pszTargetPath, pCtx->destAttrs);
        HICON hNewIcon = LoadFileDialogIcon(pCtx->pszSourcePath, pCtx->sourceAttrs);
        ReplaceDialogIcon(hDlg, IDD_ICON_OLD, hOldIcon);
        ReplaceDialogIcon(hDlg, IDD_ICON_NEW, hNewIcon);
    }

    WCHAR szTemplate[1024] = {};
    GetDlgItemTextW(hDlg, pCtx->activeTextId, szTemplate, ARRAYSIZE(szTemplate));
    WCHAR szFormatted[2048] = {};
    ExpandPercentTokens(szTemplate, szArg1, szArg2, szArg3,
                        szFormatted, ARRAYSIZE(szFormatted));
    SetDlgItemTextW(hDlg, pCtx->activeTextId, szFormatted);

    if (GetDlgItem(hDlg, IDD_ICON_WASTEBASKET))
    {
        HICON hMainIcon = NULL;
        BOOL bDeleteDialog =
            pCtx->dlgId == DLG_DELETE_FILE ||
            pCtx->dlgId == DLG_DELETE_FOLDER ||
            pCtx->dlgId == DLG_DELETE_MULTIPLE ||
            pCtx->dlgId == DLG_WONT_RECYCLE_FILE ||
            pCtx->dlgId == DLG_WONT_RECYCLE_FOLDER ||
            pCtx->dlgId == DLG_DELETE_FILE_ARP ||
            pCtx->dlgId == DLG_PATH_TOO_LONG;

        if (bDeleteDialog)
        {
            if (pCtx->recycle)
                hMainIcon = LoadRecycleBinIcon();
            else
                hMainIcon = LoadXPDialogIcon(IDI_NUKEFILE);

            // Fall back to the icon the template itself references
            if (!hMainIcon && pCtx->mainIconOrdinal)
                hMainIcon = LoadXPDialogIcon(pCtx->mainIconOrdinal);

            if (hMainIcon)
                ReplaceDialogIcon(hDlg, IDD_ICON_WASTEBASKET, hMainIcon);
        }
    }
    else if (pCtx->mainIconOrdinal)
    {
        // Replace dialogs: the template's stock icon control has no usable id
        // and the dialog is created with hInstance=NULL, so the dialog manager
        // could not load the icon from the configured shell32 — set it here.
        HWND hwndIcon = FindTemplateIconStatic(hDlg);
        if (hwndIcon)
        {
            HICON hIcon = LoadXPDialogIcon(pCtx->mainIconOrdinal);
            if (hIcon)
            {
                HICON hPrev = (HICON)SendMessageW(hwndIcon, STM_SETICON, (WPARAM)hIcon, 0);
                if (hPrev)
                    DestroyIcon(hPrev);
            }
        }
    }

    for (UINT id = IDD_TEXT; id <= IDD_TEXT4; id++)
    {
        HWND hwndCtl = GetDlgItem(hDlg, id);
        if (hwndCtl && id != pCtx->activeTextId)
            ShowWindow(hwndCtl, SW_HIDE);
    }
    if (pCtx->activeTextId == IDD_ARPWARNINGTEXT)
    {
        for (UINT id = IDD_TEXT; id <= IDD_TEXT4; id++)
        {
            HWND hwndCtl = GetDlgItem(hDlg, id);
            if (hwndCtl)
                ShowWindow(hwndCtl, SW_HIDE);
        }
    }

    if (pCtx->shrinkDialog)
        ShrinkDialog(hDlg, pCtx->activeTextId);
}

static INT_PTR CALLBACK ClassicConfirmDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CLASSIC_CONFIRM_CONTEXT* pCtx = (CLASSIC_CONFIRM_CONTEXT*)GetWindowLongPtrW(hDlg, DWLP_USER);

    switch (uMsg)
    {
    case WM_INITDIALOG:
        SetWindowLongPtrW(hDlg, DWLP_USER, lParam);
        pCtx = (CLASSIC_CONFIRM_CONTEXT*)lParam;
        InitClassicConfirmDialog(hDlg, pCtx);
        return TRUE;

    case WM_DESTROY:
        ReplaceDialogIcon(hDlg, IDD_ICON_OLD, NULL);
        ReplaceDialogIcon(hDlg, IDD_ICON_NEW, NULL);
        ReplaceDialogIcon(hDlg, IDD_ICON_WASTEBASKET, NULL);
        {
            HWND hwndIcon = FindTemplateIconStatic(hDlg);
            if (hwndIcon)
            {
                HICON hPrev = (HICON)SendMessageW(hwndIcon, STM_SETICON, 0, 0);
                if (hPrev)
                    DestroyIcon(hPrev);
            }
        }
        return TRUE;

    case WM_COMMAND:
        if (!pCtx)
            break;

        switch (LOWORD(wParam))
        {
        case IDD_YESTOALL:
            pCtx->applyToAll = TRUE;
            EndDialog(hDlg, IDYES);
            return TRUE;

        case IDYES:
        case IDNO:
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;

    case WM_NOTIFY:
        if (((LPNMHDR)lParam)->code == NM_CLICK || ((LPNMHDR)lParam)->code == NM_RETURN)
        {
            OpenAddRemovePrograms();
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, IDCANCEL);
        return TRUE;
    }

    return FALSE;
}

static INT_PTR ShowClassicConfirmDialog(CLASSIC_CONFIRM_CONTEXT* pCtx)
{
    DWORD cbTemplate = 0;
    LPCDLGTEMPLATEW pTemplate = GetXPDialogTemplate(pCtx->dlgId, &cbTemplate);
    if (!pTemplate)
        return -1;

    pCtx->mainIconOrdinal = GetTemplateMainIconOrdinal(pTemplate, cbTemplate);

    HWND hwndParent = pCtx->hwndParent;
    if (!hwndParent)
        hwndParent = GetActiveWindow();
    if (!hwndParent)
        hwndParent = GetForegroundWindow();

    // hInstance must NOT be the datafile-mapped g_hXPShell32: that handle is
    // bit-tagged and is not a valid module for window creation. The template
    // bytes are self-contained, so NULL is the correct instance here.
    return DialogBoxIndirectParamW(NULL, pTemplate, hwndParent,
                                   ClassicConfirmDlgProc, (LPARAM)pCtx);
}

HRESULT __fastcall CTransferConfirmation_Confirm_Hook(
    void* pThis,
    const CONFIRMOP_MIN* pConfirmOp,
    CONFIRMATIONRESPONSE_MIN* pResponse,
    int* pfApplyToAll,
    unsigned int* pLastMessageId)
{
    if (!CTransferConfirmation_Confirm_Orig)
        return E_FAIL;

    if (!g_useClassicConfirmations || !g_hXPShell32 || !pConfirmOp || !pResponse)
    {
        return CTransferConfirmation_Confirm_Orig(
            pThis, pConfirmOp, pResponse, pfApplyToAll, pLastMessageId);
    }

    CLASSIC_CONFIRM_CONTEXT ctx = {};
    HRESULT hr = E_FAIL;
    INT_PTR nRet = -1;
    if (!BuildClassicConfirmContext(pConfirmOp, &ctx))
        goto CleanupAndFallback;

    nRet = ShowClassicConfirmDialog(&ctx);
    if (nRet == -1)
        goto CleanupAndFallback;

    if (pfApplyToAll)
        *pfApplyToAll = ctx.applyToAll ? 1 : 0;
    if (pLastMessageId)
        *pLastMessageId = 0;

    switch (nRet)
    {
    case IDYES:
        *pResponse = CONFIRMRESP_YES;
        hr = S_OK;
        break;

    case IDCANCEL:
        *pResponse = CONFIRMRESP_CANCEL;
        hr = S_OK;
        break;

    case IDNO:
    default:
        *pResponse = ctx.treatNoAsCancel ? CONFIRMRESP_CANCEL : CONFIRMRESP_NO;
        hr = S_OK;
        break;
    }

    ((DWORD*)pThis)[30] = (DWORD)*pResponse;
    ((DWORD*)pThis)[31] = ctx.applyToAll ? 1 : 0;
    ((DWORD*)pThis)[44] = 0;

    FreeClassicConfirmContext(&ctx);
    return hr;

CleanupAndFallback:
    FreeClassicConfirmContext(&ctx);
    return CTransferConfirmation_Confirm_Orig(
        pThis, pConfirmOp, pResponse, pfApplyToAll, pLastMessageId);
}

HRESULT __fastcall CConflictResolutionDlg_ShowDialog_Hook(
    void* pConflictParams,
    ISyncMgrConflictItemsMin* pConflictItems,
    CONFIRM_CONFLICT_RESULT_MIN* pResult,
    ISyncMgrConflictResolutionItemsMin** ppResolutionItems)
{
    if (!CConflictResolutionDlg_ShowDialog_Orig)
        return E_FAIL;

    if (!g_useClassicConfirmations || !g_hXPShell32 || !pConflictItems || !pResult ||
        !ppResolutionItems)
    {
        return CConflictResolutionDlg_ShowDialog_Orig(
            pConflictParams, pConflictItems, pResult, ppResolutionItems);
    }

    const CONFIRM_CONFLICT_PARAMS_HINT* pHint =
        (const CONFIRM_CONFLICT_PARAMS_HINT*)pConflictParams;
    HWND hwndParent = pHint ? pHint->hwndParent : NULL;

    CLASSIC_CONFIRM_CONTEXT ctx = {};
    UINT sourceIndex = 0;
    UINT targetIndex = 0;
    if (!BuildClassicConflictConfirmContext(hwndParent, pConflictItems, &ctx,
                                            &sourceIndex, &targetIndex))
    {
        FreeClassicConfirmContext(&ctx);
        return CConflictResolutionDlg_ShowDialog_Orig(
            pConflictParams, pConflictItems, pResult, ppResolutionItems);
    }

    if (pHint && (pHint->flags & 1) && pHint->cRemainingConflicts > 0)
    {
        ctx.showCancel = TRUE;
        ctx.showYesToAll = TRUE;
    }

    INT_PTR nRet = ShowClassicConfirmDialog(&ctx);
    if (nRet == -1)
    {
        FreeClassicConfirmContext(&ctx);
        return CConflictResolutionDlg_ShowDialog_Orig(
            pConflictParams, pConflictItems, pResult, ppResolutionItems);
    }

    *ppResolutionItems = NULL;
    pResult->fApplyToAll = ctx.applyToAll ? 1 : 0;

    if (nRet == IDCANCEL)
    {
        pResult->iResult = 0x80000000;
        FreeClassicConfirmContext(&ctx);
        return S_OK;
    }

    CSingleConflictResolutionItems* pItems =
        new CSingleConflictResolutionItems(nRet == IDYES ? sourceIndex : targetIndex);
    if (!pItems)
    {
        FreeClassicConfirmContext(&ctx);
        return E_OUTOFMEMORY;
    }

    *ppResolutionItems = pItems;
    pResult->iResult = 0;
    FreeClassicConfirmContext(&ctx);
    return S_OK;
}


// ============================================================================
// CXPProgressDialog: XP-style IProgressDialog COM implementation
// ============================================================================
class CXPProgressDialog
    : public IProgressDialog
    , public IOleWindow
    , public IActionProgressDialog
    , public IActionProgress
    , public IOperationsProgressDialog
{
public:
    CXPProgressDialog();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IProgressDialog
    STDMETHODIMP StartProgressDialog(HWND hwndParent, IUnknown* punkEnableModless, DWORD dwFlags, LPCVOID pvReserved);
    STDMETHODIMP StopProgressDialog();
    STDMETHODIMP SetTitle(LPCWSTR pwzTitle);
    STDMETHODIMP SetAnimation(HINSTANCE hInstAnimation, UINT idAnimation);
    STDMETHODIMP_(BOOL) HasUserCancelled();
    STDMETHODIMP SetProgress(DWORD dwCompleted, DWORD dwTotal);
    STDMETHODIMP SetProgress64(ULONGLONG ullCompleted, ULONGLONG ullTotal);
    STDMETHODIMP SetLine(DWORD dwLineNum, LPCWSTR pwzString, BOOL fCompactPath, LPCVOID pvReserved);
    STDMETHODIMP SetCancelMsg(LPCWSTR pwzCancelMsg, LPCVOID pvReserved);
    STDMETHODIMP Timer(DWORD dwAction, LPCVOID pvReserved);

    // IOleWindow
    STDMETHODIMP GetWindow(HWND* phwnd);
    STDMETHODIMP ContextSensitiveHelp(BOOL) { return E_NOTIMPL; }

    // IActionProgressDialog
    STDMETHODIMP Initialize(SPINITF flags, LPCWSTR pszTitle, LPCWSTR pszCancel);
    STDMETHODIMP Stop();

    // IActionProgress
    STDMETHODIMP Begin(SPACTION action, SPBEGINF flags);
    STDMETHODIMP UpdateProgress(ULONGLONG ulCompleted, ULONGLONG ulTotal);
    STDMETHODIMP UpdateText(SPTEXT sptext, LPCWSTR pszText, BOOL fMayCompact);
    STDMETHODIMP QueryCancel(BOOL* pfCancelled);
    STDMETHODIMP ResetCancel();
    STDMETHODIMP End();

    // IOperationsProgressDialog
    // Note: StartProgressDialog/StopProgressDialog have different signatures from IProgressDialog
    // so C++ correctly resolves them to separate vtable slots.
    // UpdateProgress has a different signature (6 args vs 2) so it also resolves correctly.
    STDMETHODIMP StartProgressDialog(HWND hwndOwner, OPPROGDLGF flags);
    STDMETHODIMP SetOperation(SPACTION action);
    STDMETHODIMP SetMode(PDMODE mode);
    STDMETHODIMP UpdateProgress(
        ULONGLONG ullPointsCurrent, ULONGLONG ullPointsTotal,
        ULONGLONG ullSizeCurrent, ULONGLONG ullSizeTotal,
        ULONGLONG ullItemsCurrent, ULONGLONG ullItemsTotal);
    STDMETHODIMP UpdateLocations(
        IShellItem *psiSource, IShellItem *psiTarget, IShellItem *psiItem);
    STDMETHODIMP ResetTimer();
    STDMETHODIMP PauseTimer();
    STDMETHODIMP ResumeTimer();
    STDMETHODIMP GetMilliseconds(
        ULONGLONG *pullElapsed, ULONGLONG *pullRemaining);
    STDMETHODIMP GetOperationStatus(PDOPSTATUS *popstatus);

    // Dialog procedures
    static INT_PTR CALLBACK ProgressDialogProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam);
    static DWORD CALLBACK UIThreadProc(LPVOID pv);

private:
    virtual ~CXPProgressDialog();

    INT_PTR _ProgressDialogProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam);
    BOOL _OnInit(HWND hDlg);
    void _DisplayDialog();
    void _UpdateProgressDialog();
    void _AsyncUpdate();
    void _SetProgressTime();
    void _SetProgressTimeEst(DWORD dwSecondsLeft);
    void _UserCancelled();
    void _PauseAnimation(BOOL bStop);
    void _CompactProgressPath(LPCWSTR pwzIn, BOOL fCompact, UINT idItem, LPWSTR pwzOut, DWORD cch);
    void _CompactFolderNames(LPCWSTR pszSrc, LPCWSTR pszDst, LPWSTR pszSrcOut, DWORD cchSrc, LPWSTR pszDstOut, DWORD cchDst);
    HRESULT _SetLineHelper(LPCWSTR pwzNew, LPWSTR* ppwzDest, UINT idItem, BOOL fCompact);
    void _SetTitleBarProgress(DWORD dwCompleted, DWORD dwTotal);
    HRESULT _BeginAction(DWORD flags);
    void _SetModeless(BOOL fModeless);
    HWND _CreateXPDialog(HWND hwndParent);
    void _DisableThemeForWindow(HWND hwnd);

    LONG    _cRef;

    // Cached values for before dialog creation
    LPWSTR  _pwzTitle;
    UINT    _idAnimation;
    HINSTANCE _hInstAnimation;
    LPWSTR  _pwzLine1;
    LPWSTR  _pwzLine2;
    LPWSTR  _pwzLine3;
    LPWSTR  _pwzCancelMsg;
    LPWSTR  _pwzSourceFolder;
    LPWSTR  _pwzDestFolder;
    LPWSTR  _pwzSrcRootPath;   // operation source root (cached from psiSource)
    LPWSTR  _pwzDstRootPath;   // operation destination root (cached from psiTarget)

    // Window handles
    HWND    _hwndDlgParent;
    HWND    _hwndProgress;
    DWORD   _dwFirstShowTime;

    // Flags
    DWORD   _spinitf;
    DWORD   _spbeginf;
    HINSTANCE _hinstFree;
    SPACTION _spaction;
    PDMODE  _pdmode;
    DWORD   _dwOpsFlags;
    DWORD   _dwStartTime;

    BOOL    _fCompletedChanged;
    BOOL    _fTotalChanged;
    BOOL    _fChangePosted;
    BOOL    _fCancel;
    BOOL    _fTermThread;
    BOOL    _fThreadRunning;
    BOOL    _fInAction;
    BOOL    _fMinimized;
    BOOL    _fScaleBug;
    BOOL    _fNoTime;
    BOOL    _fInitialized;
    BOOL    _fIsEmptyRecycleBin;
    BOOL    _fOperationStarted;

    // Progress values
    DWORD   _dwCompleted;
    DWORD   _dwTotal;
    DWORD   _dwPrevRate;
    DWORD   _dwPrevTickCount;
    DWORD   _dwPrevCompleted;
    DWORD   _dwLastUpdatedTimeRemaining;
    DWORD   _dwLastUpdatedTickCount;
    UINT    _iNumTimesSetProgressCalled;

    HANDLE  _hThread;
    HANDLE  _hReadyEvent;
};

// ============================================================================
// Constructor / Destructor
// ============================================================================

CXPProgressDialog::CXPProgressDialog()
    : _cRef(1)
    , _pwzTitle(NULL), _idAnimation(0), _hInstAnimation(NULL)
    , _pwzLine1(NULL), _pwzLine2(NULL), _pwzLine3(NULL), _pwzCancelMsg(NULL)
    , _pwzSourceFolder(NULL), _pwzDestFolder(NULL)
    , _pwzSrcRootPath(NULL), _pwzDstRootPath(NULL)
    , _hwndDlgParent(NULL), _hwndProgress(NULL), _dwFirstShowTime(0)
    , _spinitf(0), _spbeginf(0), _hinstFree(NULL)
    , _spaction(SPACTION_NONE), _pdmode(PDM_DEFAULT), _dwOpsFlags(0), _dwStartTime(0)
    , _fCompletedChanged(FALSE), _fTotalChanged(FALSE), _fChangePosted(FALSE)
    , _fCancel(FALSE), _fTermThread(FALSE), _fThreadRunning(FALSE)
    , _fInAction(FALSE), _fMinimized(FALSE), _fScaleBug(FALSE)
    , _fNoTime(FALSE), _fInitialized(FALSE)
    , _fIsEmptyRecycleBin(FALSE), _fOperationStarted(FALSE)
    , _dwCompleted(0), _dwTotal(1), _dwPrevRate(0)
    , _dwPrevTickCount(0), _dwPrevCompleted(0)
    , _dwLastUpdatedTimeRemaining(0), _dwLastUpdatedTickCount(0)
    , _iNumTimesSetProgressCalled(0)
    , _hThread(NULL), _hReadyEvent(NULL)
{
}

CXPProgressDialog::~CXPProgressDialog()
{
    StrFreeW(&_pwzTitle);
    StrFreeW(&_pwzLine1);
    StrFreeW(&_pwzLine2);
    StrFreeW(&_pwzLine3);
    StrFreeW(&_pwzCancelMsg);
    StrFreeW(&_pwzSourceFolder);
    StrFreeW(&_pwzDestFolder);
    StrFreeW(&_pwzSrcRootPath);
    StrFreeW(&_pwzDstRootPath);

    if (_hinstFree)
        FreeLibrary(_hinstFree);
    if (_hThread)
        CloseHandle(_hThread);
    if (_hReadyEvent)
        CloseHandle(_hReadyEvent);
}

// ============================================================================
// IUnknown
// ============================================================================

STDMETHODIMP CXPProgressDialog::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = NULL;

    if (riid == IID_IUnknown || riid == IID_IProgressDialog)
        *ppv = static_cast<IProgressDialog*>(this);
    else if (riid == IID_IOleWindow)
        *ppv = static_cast<IOleWindow*>(this);
    else if (riid == IID_IActionProgressDialog)
        *ppv = static_cast<IActionProgressDialog*>(this);
    else if (riid == IID_IActionProgress)
        *ppv = static_cast<IActionProgress*>(this);
    else if (riid == IID_IOperationsProgressDialog)
        *ppv = static_cast<IOperationsProgressDialog*>(this);
    else
    {
        Wh_Log(L"QI miss: {%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            riid.Data1, riid.Data2, riid.Data3,
            riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3],
            riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) CXPProgressDialog::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

STDMETHODIMP_(ULONG) CXPProgressDialog::Release()
{
    ULONG cRef = InterlockedDecrement(&_cRef);
    if (cRef == 0)
    {
        if (_fThreadRunning)
        {
            _fTermThread = TRUE;
            if (_hwndProgress)
                PostMessage(_hwndProgress, PDM_TERMTHREAD, 0, 0);
            if (_hThread)
            {
                PumpWaitForSingleObject(_hThread, 5000);
            }
        }
        delete this;
    }
    return cRef;
}

// ============================================================================
// Dialog creation: Load XP dialog template from the configured shell32.dll
// ============================================================================

HWND CXPProgressDialog::_CreateXPDialog(HWND hwndParent)
{
    LPCDLGTEMPLATEW pTemplate = GetXPDialogTemplate(DLG_MOVECOPYPROGRESS);
    if (!pTemplate)
        return NULL;

    // Pass NULL (not the datafile-mapped g_hXPShell32) as the instance; the
    // template is self-contained and a datafile handle is not a valid module.
    return CreateDialogIndirectParamW(
        NULL, pTemplate, hwndParent,
        ProgressDialogProc, (LPARAM)this);
}

// ============================================================================
// Theme stripping: Make dialog look like classic XP
// ============================================================================

void CXPProgressDialog::_DisableThemeForWindow(HWND hwnd)
{
    SetWindowTheme(hwnd, L"", L"");
    HWND hChild = ::GetWindow(hwnd, GW_CHILD);
    while (hChild)
    {
        SetWindowTheme(hChild, L"", L"");
        hChild = ::GetWindow(hChild, GW_HWNDNEXT);
    }
}

// ============================================================================
// UI Thread
// ============================================================================

DWORD CALLBACK CXPProgressDialog::UIThreadProc(LPVOID pv)
{
    CXPProgressDialog* pThis = (CXPProgressDialog*)pv;

    // Initialize common controls for animation
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_ANIMATE_CLASS | ICC_PROGRESS_CLASS };
    InitCommonControlsEx(&icc);

    pThis->_hwndProgress = pThis->_CreateXPDialog(pThis->_hwndDlgParent);
    pThis->_fThreadRunning = (pThis->_hwndProgress != NULL);

    Wh_Log(L"UIThreadProc: CreateXPDialog returned hwnd=0x%p, running=%d",
        pThis->_hwndProgress, pThis->_fThreadRunning);

    // Signal that dialog is ready
    SetEvent(pThis->_hReadyEvent);

    if (!pThis->_hwndProgress)
    {
        Wh_Log(L"UIThreadProc: Dialog creation failed, GetLastError=%lu", GetLastError());
        return 0;
    }

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    // Don't start show timer yet — wait for actual file operation data
    // (avoids showing dialog during Win10's confirmation dialog)
    // Timer will be started by PDM_STARTTIMER when UpdateProgress/UpdateLocations is called

    MSG msg;
    BOOL fTermTimerSet = FALSE;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (pThis->_fTermThread)
        {
            if (pThis->_dwFirstShowTime == 0 ||
                (GetTickCount() - pThis->_dwFirstShowTime) > MINSHOWTIME)
            {
                break;
            }
            // MINSHOWTIME not yet elapsed — set a wake-up timer so
            // GetMessage doesn't block us past the target time.
            if (!fTermTimerSet)
            {
                DWORD dwRemain = MINSHOWTIME - (GetTickCount() - pThis->_dwFirstShowTime);
                SetTimer(pThis->_hwndProgress, 2, dwRemain + 10, NULL);
                fTermTimerSet = TRUE;
            }
        }

        if (!IsDialogMessage(pThis->_hwndProgress, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    DestroyWindow(pThis->_hwndProgress);

    EnterCriticalSection(&g_cs);
    pThis->_hwndProgress = NULL;
    pThis->_fThreadRunning = FALSE;
    LeaveCriticalSection(&g_cs);

    return 0;
}

// ============================================================================
// Dialog procedure
// ============================================================================

INT_PTR CALLBACK CXPProgressDialog::ProgressDialogProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    CXPProgressDialog* ppd = (CXPProgressDialog*)GetWindowLongPtr(hDlg, DWLP_USER);

    if (wMsg == WM_INITDIALOG)
    {
        SetWindowLongPtr(hDlg, DWLP_USER, lParam);
        ppd = (CXPProgressDialog*)lParam;
    }

    if (ppd)
        return ppd->_ProgressDialogProc(hDlg, wMsg, wParam, lParam);

    return FALSE;
}

BOOL CXPProgressDialog::_OnInit(HWND hDlg)
{
    // Remove minimize box if modal
    if ((_spinitf & SPINITF_MODAL) || (_spinitf & SPINITF_NOMINIMIZE))
    {
        LONG style = GetWindowLong(hDlg, GWL_STYLE);
        SetWindowLong(hDlg, GWL_STYLE, style & ~WS_MINIMIZEBOX);
    }

    // Show/hide progress bar
    HWND hwndBar = GetDlgItem(hDlg, IDD_PROGDLG_PROGRESSBAR);
    if (hwndBar)
    {
        if (_spbeginf & SPBEGINF_NOPROGRESSBAR)
            ShowWindow(hwndBar, SW_HIDE);
        else
            ShowWindow(hwndBar, SW_SHOW);
    }

    // Hide time estimate line if disabled (9x style)
    if (!g_showTimeEstimate)
    {
        HWND hwndLine3 = GetDlgItem(hDlg, IDD_PROGDLG_LINE3);
        if (hwndLine3)
            ShowWindow(hwndLine3, SW_HIDE);
    }

    return FALSE;
}

INT_PTR CXPProgressDialog::_ProgressDialogProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg)
    {
    case WM_INITDIALOG:
        return _OnInit(hDlg);

    case WM_SHOWWINDOW:
        if (wParam)
        {
            // Disable parent if modal
            _SetModeless(FALSE);

            // Set animation
            SetAnimation(_hInstAnimation, _idAnimation);

            // Set initial text values
            if (_pwzTitle)
                SetTitle(_pwzTitle);
            if (_pwzLine1)
                SetLine(1, _pwzLine1, FALSE, NULL);
            if (_pwzLine2)
                SetLine(2, _pwzLine2, FALSE, NULL);
            if (_pwzLine3)
                SetLine(3, _pwzLine3, FALSE, NULL);
        }
        break;

    case WM_DESTROY:
        _SetModeless(TRUE);
        if (_hwndDlgParent)
        {
            HWND hFocus = GetFocus();
            if (hFocus && (hFocus == _hwndProgress || IsChild(_hwndProgress, hFocus)))
                SetForegroundWindow(_hwndDlgParent);
        }
        break;

    case WM_ENABLE:
        if (wParam)
            _dwPrevTickCount = GetTickCount();
        _PauseAnimation(wParam == 0);
        break;

    case WM_TIMER:
        if (wParam == ID_SHOWTIMER)
        {
            KillTimer(hDlg, ID_SHOWTIMER);
            _DisplayDialog();
            _dwFirstShowTime = GetTickCount();
        }
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
            _UserCancelled();
        break;

    case PDM_TERMTHREAD:
        break;

    case WM_SYSCOMMAND:
        switch (wParam & 0xFFF0)
        {
        case SC_MINIMIZE:
            _fMinimized = TRUE;
            break;
        case SC_RESTORE:
            if (_pwzTitle)
                SetWindowTextW(_hwndProgress, _pwzTitle);
            _fMinimized = FALSE;
            break;
        }
        return FALSE; // let DefDlgProc handle

    case PDM_UPDATE:
        if (!_fCancel && IsWindowEnabled(hDlg))
        {
            if (g_showTimeEstimate && !_fIsEmptyRecycleBin)
                _SetProgressTime();
            _UpdateProgressDialog();
        }
        _fChangePosted = FALSE;
        break;

    case PDM_STARTTIMER:
        // Deferred timer start — only begin the 1-second show countdown
        // when actual file operation data arrives (not during confirmation dialogs)
        if (!_dwFirstShowTime)
            SetTimer(hDlg, ID_SHOWTIMER, SHOW_PROGRESS_TIMEOUT, NULL);
        break;

    case WM_QUERYENDSESSION:
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, FALSE);
        return TRUE;

    default:
        return FALSE;
    }

    return TRUE;
}

// ============================================================================
// Display / Update helpers
// ============================================================================

void CXPProgressDialog::_DisplayDialog()
{
    if (!_hwndProgress) return;

    // Don't steal focus from our own child windows (e.g. confirm dialogs)
    HWND hwndCurrent = GetForegroundWindow();
    BOOL fChildIsForeground = FALSE;
    while (hwndCurrent)
    {
        hwndCurrent = GetParent(hwndCurrent);
        if (hwndCurrent == _hwndProgress)
        {
            fChildIsForeground = TRUE;
            break;
        }
    }

    if (fChildIsForeground)
        ShowWindow(_hwndProgress, SW_SHOWNOACTIVATE);
    else
    {
        ShowWindow(_hwndProgress, SW_SHOW);
        SetForegroundWindow(_hwndProgress);
    }

    SetFocus(GetDlgItem(_hwndProgress, IDCANCEL));
}

void CXPProgressDialog::_AsyncUpdate()
{
    if (!_fChangePosted && _hwndProgress)
    {
        _fChangePosted = TRUE;
        if (!PostMessage(_hwndProgress, PDM_UPDATE, 0, 0))
            _fChangePosted = FALSE;
    }
}

void CXPProgressDialog::_UpdateProgressDialog()
{
    HWND hwndBar = GetDlgItem(_hwndProgress, IDD_PROGDLG_PROGRESSBAR);
    if (!hwndBar) return;

    if (_fTotalChanged)
    {
        _fTotalChanged = FALSE;
        if (0x80000000 & _dwTotal)
            _fScaleBug = TRUE;
        SendMessage(hwndBar, PBM_SETRANGE32, 0,
            (LPARAM)(_fScaleBug ? (_dwTotal >> 1) : _dwTotal));
    }

    if (_fCompletedChanged)
    {
        _fCompletedChanged = FALSE;
        SendMessage(hwndBar, PBM_SETPOS,
            (WPARAM)(_fScaleBug ? (_dwCompleted >> 1) : _dwCompleted), 0);
    }
}

void CXPProgressDialog::_PauseAnimation(BOOL bStop)
{
    if (!_hwndProgress) return;
    HWND hwndAni = GetDlgItem(_hwndProgress, IDD_PROGDLG_ANIMATION);
    if (!hwndAni) return;

    if (bStop)
        Animate_Stop(hwndAni);
    else
        Animate_Play(hwndAni, -1, -1, -1);
}

void CXPProgressDialog::_UserCancelled()
{
    _fCancel = TRUE;
    EnableWindow(GetDlgItem(_hwndProgress, IDCANCEL), FALSE);

    if (!_pwzCancelMsg)
        StrSetW(&_pwzCancelMsg, L"Canceling...");

    SetLine(1, L"", FALSE, NULL);
    SetLine(2, L"", FALSE, NULL);
    SetLine(3, _pwzCancelMsg, FALSE, NULL);
}

void CXPProgressDialog::_SetModeless(BOOL fModeless)
{
    if ((_spinitf & SPINITF_MODAL) && _hwndDlgParent)
        EnableWindow(_hwndDlgParent, fModeless);
}

// ============================================================================
// Time estimation (from XP's CProgressDialog::_SetProgressTime)
// ============================================================================

void CXPProgressDialog::_SetProgressTimeEst(DWORD dwSecondsLeft)
{
    WCHAR szOut[170] = {0};
    DWORD dwTime;
    DWORD dwTickCount = GetTickCount();

    // Throttle updates: every 10s if >1min, every 2s if <1min
    if (_dwLastUpdatedTimeRemaining &&
        dwTickCount - _dwLastUpdatedTimeRemaining <
        (DWORD)((dwSecondsLeft > 60) ? 10000 : 2000))
        return;

    if (_fNoTime)
    {
        szOut[0] = L'\0';
    }
    else if (dwSecondsLeft > 24 * 60 * 60)
    {
        // Days + hours (matching XP's browselc.rc)
        DWORD dwDays = dwSecondsLeft / (24 * 60 * 60);
        DWORD dwHours = (dwSecondsLeft % (24 * 60 * 60)) / 3600;
        if (dwDays == 1 && dwHours == 1)
            StringCchPrintfW(szOut, ARRAYSIZE(szOut),
                L"%lu Day and %lu hour Remaining", dwDays, dwHours);
        else if (dwDays == 1)
            StringCchPrintfW(szOut, ARRAYSIZE(szOut),
                L"%lu Day and %lu hours Remaining", dwDays, dwHours);
        else if (dwHours == 1)
            StringCchPrintfW(szOut, ARRAYSIZE(szOut),
                L"%lu Days and %lu hour Remaining", dwDays, dwHours);
        else
            StringCchPrintfW(szOut, ARRAYSIZE(szOut),
                L"%lu Days and %lu hours Remaining", dwDays, dwHours);
    }
    else if (dwSecondsLeft > 3600)
    {
        // Hours + minutes (matching XP's browselc.rc)
        DWORD dwHours = dwSecondsLeft / 3600;
        DWORD dwMinutes = (dwSecondsLeft % 3600) / 60;
        if (dwHours == 1 && dwMinutes == 1)
            StringCchPrintfW(szOut, ARRAYSIZE(szOut),
                L"%lu Hour and %lu Minute Remaining", dwHours, dwMinutes);
        else if (dwHours == 1)
            StringCchPrintfW(szOut, ARRAYSIZE(szOut),
                L"%lu Hour and %lu Minutes Remaining", dwHours, dwMinutes);
        else if (dwMinutes == 1)
            StringCchPrintfW(szOut, ARRAYSIZE(szOut),
                L"%lu Hours and %lu Minute Remaining", dwHours, dwMinutes);
        else
            StringCchPrintfW(szOut, ARRAYSIZE(szOut),
                L"%lu Hours and %lu Minutes Remaining", dwHours, dwMinutes);
    }
    else if (dwSecondsLeft > 60)
    {
        dwTime = (dwSecondsLeft / 60) + 1;
        StringCchPrintfW(szOut, ARRAYSIZE(szOut),
            L"%lu Minutes Remaining", dwTime);
    }
    else
    {
        // Round up to 5 seconds
        dwTime = ((dwSecondsLeft + 4) / 5) * 5;
        if (dwTime == 0) dwTime = 5;
        StringCchPrintfW(szOut, ARRAYSIZE(szOut),
            L"%lu Seconds Remaining", dwTime);
    }

    _dwLastUpdatedTimeRemaining = dwTickCount;

    if (_hwndProgress)
        SetDlgItemTextW(_hwndProgress, IDD_PROGDLG_LINE3, szOut);
}

void CXPProgressDialog::_SetProgressTime()
{
    DWORD dwTotal, dwCompleted, dwCurrentTickCount;

    EnterCriticalSection(&g_cs);
    dwTotal = _dwTotal;
    dwCompleted = _dwCompleted;
    dwCurrentTickCount = _dwLastUpdatedTickCount;
    LeaveCriticalSection(&g_cs);

    if (!dwTotal || !dwCompleted)
        return;

    DWORD dwLeft = dwTotal - dwCompleted;
    DWORD dwTickDelta = dwCurrentTickCount - _dwPrevTickCount;

    if (dwTickDelta < 100)
        return;

    _iNumTimesSetProgressCalled++;

    if (dwTotal < dwCompleted)
    {
        _fNoTime = TRUE;
        dwTotal = dwCompleted + (dwCompleted >> 3);
    }

    DWORD dwCurrentRate;
    if (dwCompleted <= _dwPrevCompleted)
    {
        dwCurrentRate = (_dwPrevRate ? _dwPrevRate : 2);
    }
    else
    {
        DWORD dwTickTenths = dwTickDelta / 100;
        if (dwTickTenths == 0) dwTickTenths = 1;
        dwCurrentRate = (dwCompleted - _dwPrevCompleted) / dwTickTenths;
    }

    // Running average — use light smoothing early, heavier smoothing once stable
    DWORD dwWeight = (_iNumTimesSetProgressCalled < 10) ? _iNumTimesSetProgressCalled : 10;
    DWORD dwAverageRate = (DWORD)(((ULONGLONG)dwCurrentRate +
        (ULONGLONG)_dwPrevRate * dwWeight) /
        (dwWeight + 1));

    if (dwAverageRate == 0) dwAverageRate = 1;

    DWORD dwSecondsLeft = (dwLeft / dwAverageRate) / 10;

    if (dwSecondsLeft >= MIN_MINTIME4FEEDBACK && _iNumTimesSetProgressCalled >= 3)
        _SetProgressTimeEst(dwSecondsLeft);

    _dwPrevRate = dwAverageRate;
    _dwPrevTickCount = dwCurrentTickCount;
    _dwPrevCompleted = dwCompleted;
}

void CXPProgressDialog::_SetTitleBarProgress(DWORD dwCompleted, DWORD dwTotal)
{
    if (!_hwndProgress || !dwTotal) return;

    int nPercent;
    if (dwTotal >= 10000)
        nPercent = (int)(dwCompleted / (dwTotal / 100));
    else
        nPercent = (int)((100 * dwCompleted) / dwTotal);

    WCHAR szTitle[MAX_PATH];
    StringCchPrintfW(szTitle, ARRAYSIZE(szTitle), L"%d%% Completed", nPercent);
    SetWindowTextW(_hwndProgress, szTitle);
}

// ============================================================================
// Path compacting
// ============================================================================

void CXPProgressDialog::_CompactProgressPath(LPCWSTR pwzIn, BOOL fCompact, UINT idItem, LPWSTR pwzOut, DWORD cch)
{
    WCHAR wzFinal[MAX_PATH];
    LPCWSTR pwzUse = pwzIn;

    if (fCompact && _hwndProgress)
    {
        StringCchCopyW(wzFinal, ARRAYSIZE(wzFinal), pwzIn ? pwzIn : L"");
        HWND hwnd = GetDlgItem(_hwndProgress, idItem);
        if (hwnd)
        {
            RECT rc;
            GetWindowRect(hwnd, &rc);
            int cxWidth = rc.right - rc.left;

            HDC hdc = GetDC(_hwndProgress);
            HFONT hfont = (HFONT)SendMessage(_hwndProgress, WM_GETFONT, 0, 0);
            HFONT hfontSave = (HFONT)SelectObject(hdc, hfont);
            PathCompactPathW(hdc, wzFinal, cxWidth);
            SelectObject(hdc, hfontSave);
            ReleaseDC(_hwndProgress, hdc);
        }
        pwzUse = wzFinal;
    }

    StringCchCopyW(pwzOut, cch, pwzUse ? pwzUse : L"");
}

// XP compacts folder names individually before inserting into the template.
// It measures the template overhead ("From '' to ''"), subtracts from control width,
// then gives each folder name half the remaining space (or full space for single).
void CXPProgressDialog::_CompactFolderNames(
    LPCWSTR pszSrc, LPCWSTR pszDst,
    LPWSTR pszSrcOut, DWORD cchSrc,
    LPWSTR pszDstOut, DWORD cchDst)
{
    StringCchCopyW(pszSrcOut, cchSrc, pszSrc ? pszSrc : L"");
    if (pszDstOut)
        StringCchCopyW(pszDstOut, cchDst, pszDst ? pszDst : L"");

    if (!_hwndProgress) return;

    HWND hwndCtrl = GetDlgItem(_hwndProgress, IDD_PROGDLG_LINE2);
    if (!hwndCtrl) return;

    RECT rc;
    GetWindowRect(hwndCtrl, &rc);
    int cxWidth = rc.right - rc.left;

    HDC hdc = GetDC(_hwndProgress);
    HFONT hfont = (HFONT)SendMessage(_hwndProgress, WM_GETFONT, 0, 0);
    HFONT hfontSave = (HFONT)SelectObject(hdc, hfont);

    // Measure template overhead with empty folder names
    WCHAR wzTemplate[MAX_PATH];
    if (pszDst)
        FormatFromTo(IDS_FROMTO, L"From '%1!ls!' to '%2!ls!'",
                     L"", L"", wzTemplate, ARRAYSIZE(wzTemplate));
    else
        FormatFromTo(IDS_FROM, L"From '%1!ls!'",
                     L"", NULL, wzTemplate, ARRAYSIZE(wzTemplate));

    SIZE sizeTemplate;
    GetTextExtentPoint32W(hdc, wzTemplate, (int)wcslen(wzTemplate), &sizeTemplate);
    cxWidth -= sizeTemplate.cx;

    if (cxWidth > 0)
    {
        if (pszDst && pszDstOut)
        {
            PathCompactPathW(hdc, pszSrcOut, cxWidth / 2);
            PathCompactPathW(hdc, pszDstOut, cxWidth / 2);
        }
        else
        {
            PathCompactPathW(hdc, pszSrcOut, cxWidth);
        }
    }

    SelectObject(hdc, hfontSave);
    ReleaseDC(_hwndProgress, hdc);
}

HRESULT CXPProgressDialog::_SetLineHelper(LPCWSTR pwzNew, LPWSTR* ppwzDest, UINT idItem, BOOL fCompact)
{
    WCHAR wzFinal[MAX_PATH];
    _CompactProgressPath(pwzNew, fCompact, idItem, wzFinal, ARRAYSIZE(wzFinal));
    StrSetW(ppwzDest, wzFinal);

    if (_hwndProgress)
        SetDlgItemTextW(_hwndProgress, idItem, wzFinal);

    return S_OK;
}

// ============================================================================
// _BeginAction: Start the UI thread
// ============================================================================

HRESULT CXPProgressDialog::_BeginAction(DWORD flags)
{
    Wh_Log(L"_BeginAction called, flags=0x%X", flags);
    _spbeginf = flags;
    _fTermThread = FALSE;
    _fTotalChanged = TRUE;

    if (!_fThreadRunning)
    {
        if (_hReadyEvent) CloseHandle(_hReadyEvent);
        _hReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        AddRef(); // prevent destruction while thread is running
        _hThread = CreateThread(NULL, 0, UIThreadProc, this, 0, NULL);
        if (_hThread)
        {
            Wh_Log(L"UI thread created, waiting for ready event");
            PumpWaitForSingleObject(_hReadyEvent, 10000);
        }
        else
        {
            Wh_Log(L"CreateThread failed: %lu", GetLastError());
            Release();
            return E_OUTOFMEMORY;
        }
    }

    if (_fThreadRunning)
    {
        _fInAction = TRUE;
        _dwStartTime = GetTickCount();

        // Show/hide progress bar
        if (_hwndProgress)
        {
            HWND hwndBar = GetDlgItem(_hwndProgress, IDD_PROGDLG_PROGRESSBAR);
            if (hwndBar)
            {
                if (_spbeginf & SPBEGINF_NOPROGRESSBAR)
                    ShowWindow(hwndBar, SW_HIDE);
                else
                    ShowWindow(hwndBar, SW_SHOW);
            }
        }

        _dwPrevRate = 0;
        _dwPrevCompleted = 0;
        _dwPrevTickCount = GetTickCount();
        return S_OK;
    }

    return E_OUTOFMEMORY;
}

// ============================================================================
// IProgressDialog methods
// ============================================================================

STDMETHODIMP CXPProgressDialog::StartProgressDialog(HWND hwndParent, IUnknown*, DWORD dwFlags, LPCVOID)
{
    if (_fInAction) return S_OK;

    HRESULT hr = Initialize(dwFlags & 0x3, NULL, NULL); // SPINITF_MODAL | SPINITF_NOMINIMIZE
    if (SUCCEEDED(hr))
    {
        _fNoTime = (dwFlags & PROGDLG_NOTIME) != 0;
        _hwndDlgParent = hwndParent;
        if (!_idAnimation && g_hXPShell32)
            SetAnimation(g_hXPShell32, IDA_APPLYATTRIBS);

        hr = _BeginAction(dwFlags & 0x1F);
    }
    return hr;
}

STDMETHODIMP CXPProgressDialog::StopProgressDialog()
{
    if (_fInAction)
        End();
    return Stop();
}

STDMETHODIMP CXPProgressDialog::SetTitle(LPCWSTR pwzTitle)
{
    if (_hwndProgress)
    {
        SetWindowTextW(_hwndProgress, pwzTitle ? pwzTitle : L"");
    }

    StrSetW(&_pwzTitle, pwzTitle);
    return S_OK;
}

struct ActionAnimEntry {
    DWORD action;
    UINT idAnim;
};

static const ActionAnimEntry s_actionAnims[] = {
    { SPACTION_MOVING,    IDA_FILEMOVE },
    { SPACTION_COPYING,   IDA_FILECOPY },
    { SPACTION_RECYCLING, IDA_FILEDEL },
    { SPACTION_DELETING,  IDA_FILEDELREAL },
    { SPACTION_APPLYINGATTRIBS, IDA_APPLYATTRIBS },
};

STDMETHODIMP CXPProgressDialog::SetAnimation(HINSTANCE hInstAnimation, UINT idAnimation)
{
    // If the caller passes a non-XP HINSTANCE (e.g. Win10 shell32) for a known
    // XP animation ID, redirect to our loaded XP shell32 which has the AVIs.
    if (g_hXPShell32 && hInstAnimation != g_hXPShell32)
    {
        if (idAnimation >= IDA_FILEMOVE && idAnimation <= IDA_APPLYATTRIBS)
        {
            hInstAnimation = g_hXPShell32;
        }
    }

    _hInstAnimation = hInstAnimation;
    _idAnimation = idAnimation;

    if (_hwndProgress)
    {
        HWND hwndAni = GetDlgItem(_hwndProgress, IDD_PROGDLG_ANIMATION);
        if (hwndAni)
            Animate_OpenEx(hwndAni, _hInstAnimation, MAKEINTRESOURCEW(_idAnimation));
    }
    return S_OK;
}

STDMETHODIMP_(BOOL) CXPProgressDialog::HasUserCancelled()
{
    if (!_fCancel && _hwndProgress)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (!IsDialogMessage(_hwndProgress, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        if (_fTotalChanged || _fCompletedChanged)
            _AsyncUpdate();
    }
    return _fCancel;
}

STDMETHODIMP CXPProgressDialog::SetProgress(DWORD dwCompleted, DWORD dwTotal)
{
    // Start the deferred show timer on first real progress data
    if (!_fOperationStarted && _hwndProgress && (dwCompleted > 0 || dwTotal > 0))
    {
        _fOperationStarted = TRUE;
        PostMessage(_hwndProgress, PDM_STARTTIMER, 0, 0);
    }

    DWORD dwTickCount = GetTickCount();

    EnterCriticalSection(&g_cs);
    if (_dwCompleted != dwCompleted)
    {
        _dwCompleted = dwCompleted;
        _fCompletedChanged = TRUE;
    }
    if (_dwTotal != dwTotal)
    {
        _dwTotal = dwTotal;
        _fTotalChanged = TRUE;
    }
    if (_fCompletedChanged || _fTotalChanged)
        _dwLastUpdatedTickCount = dwTickCount;
    LeaveCriticalSection(&g_cs);

    if (_fCompletedChanged || _fTotalChanged)
        _AsyncUpdate();

    if (_fMinimized)
        _SetTitleBarProgress(dwCompleted, dwTotal);

    return S_OK;
}

STDMETHODIMP CXPProgressDialog::SetProgress64(ULONGLONG ullCompleted, ULONGLONG ullTotal)
{
    ULARGE_INTEGER uliCompleted, uliTotal;
    uliCompleted.QuadPart = ullCompleted;
    uliTotal.QuadPart = ullTotal;

    while (uliTotal.HighPart)
    {
        uliCompleted.QuadPart >>= 1;
        uliTotal.QuadPart >>= 1;
    }

    return SetProgress(uliCompleted.LowPart, uliTotal.LowPart);
}

STDMETHODIMP CXPProgressDialog::SetLine(DWORD dwLineNum, LPCWSTR pwzString, BOOL fCompactPath, LPCVOID)
{
    switch (dwLineNum)
    {
    case 1:
        return _SetLineHelper(pwzString, &_pwzLine1, IDD_PROGDLG_LINE1, fCompactPath);
    case 2:
        return _SetLineHelper(pwzString, &_pwzLine2, IDD_PROGDLG_LINE2, fCompactPath);
    case 3:
        if ((_spbeginf & SPBEGINF_AUTOTIME) && !_fCancel)
            return S_OK; // line 3 auto-managed
        return _SetLineHelper(pwzString, &_pwzLine3, IDD_PROGDLG_LINE3, fCompactPath);
    }
    return E_INVALIDARG;
}

STDMETHODIMP CXPProgressDialog::SetCancelMsg(LPCWSTR pwzCancelMsg, LPCVOID)
{
    StrSetW(&_pwzCancelMsg, pwzCancelMsg);
    return S_OK;
}

STDMETHODIMP CXPProgressDialog::Timer(DWORD dwAction, LPCVOID)
{
    if (dwAction == PDTIMER_RESET)
    {
        _dwPrevTickCount = GetTickCount();
        return S_OK;
    }
    return E_NOTIMPL;
}

// ============================================================================
// IOleWindow
// ============================================================================

STDMETHODIMP CXPProgressDialog::GetWindow(HWND* phwnd)
{
    if (!phwnd) return E_POINTER;
    *phwnd = _hwndProgress;
    return _hwndProgress ? S_OK : E_FAIL;
}

// ============================================================================
// IActionProgressDialog
// ============================================================================

STDMETHODIMP CXPProgressDialog::Initialize(SPINITF flags, LPCWSTR pszTitle, LPCWSTR pszCancel)
{
    if (!_fInitialized)
    {
        _spinitf = flags;
        if (pszTitle)
            SetTitle(pszTitle);
        if (pszCancel)
            SetCancelMsg(pszCancel, NULL);
        _fInitialized = TRUE;
        return S_OK;
    }
    return S_OK; // be lenient on re-init
}

STDMETHODIMP CXPProgressDialog::Stop()
{
    if (_fThreadRunning && _hwndProgress)
    {
        _fTermThread = TRUE;
        PostMessage(_hwndProgress, PDM_TERMTHREAD, 0, 0);

        // Wait for the UI thread to finish (pump messages to avoid deadlock)
        if (_hThread)
        {
            PumpWaitForSingleObject(_hThread, 5000);
            CloseHandle(_hThread);
            _hThread = NULL;
        }
        Release(); // match AddRef from _BeginAction
    }
    return S_OK;
}

// ============================================================================
// IActionProgress
// ============================================================================

STDMETHODIMP CXPProgressDialog::Begin(SPACTION action, SPBEGINF flags)
{
    if (_fInAction || !_fInitialized)
        return E_FAIL;

    _spaction = action;
    HRESULT hr = S_OK;

    // Detect empty recycle bin via SHEmptyRecycleBinW hook
    if (action == SPACTION_DELETING && g_fEmptyingRecycleBin)
    {
        _fIsEmptyRecycleBin = TRUE;
        if (g_hXPShell32)
            hr = SetAnimation(g_hXPShell32, IDA_FILENUKE);
    }
    else
    {
        // Try to load animation from XP shell32
        for (int i = 0; i < ARRAYSIZE(s_actionAnims); i++)
        {
            if (s_actionAnims[i].action == (DWORD)action)
            {
                HINSTANCE hinst = g_hXPShell32;
                if (hinst)
                {
                    hr = SetAnimation(hinst, s_actionAnims[i].idAnim);
                }
                break;
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = _BeginAction(flags);
    }

    return hr;
}

STDMETHODIMP CXPProgressDialog::UpdateProgress(ULONGLONG ulCompleted, ULONGLONG ulTotal)
{
    if (_fInitialized && _fInAction)
        return SetProgress64(ulCompleted, ulTotal);
    return E_UNEXPECTED;
}

STDMETHODIMP CXPProgressDialog::UpdateText(SPTEXT sptext, LPCWSTR pszText, BOOL fMayCompact)
{
    if (_fInitialized)
        return SetLine((DWORD)sptext, pszText, fMayCompact, NULL);
    return E_UNEXPECTED;
}

STDMETHODIMP CXPProgressDialog::QueryCancel(BOOL* pfCancelled)
{
    if (!pfCancelled) return E_POINTER;
    *pfCancelled = HasUserCancelled();
    return S_OK;
}

STDMETHODIMP CXPProgressDialog::ResetCancel()
{
    _fCancel = FALSE;
    if (_hwndProgress)
        EnableWindow(GetDlgItem(_hwndProgress, IDCANCEL), TRUE);
    if (_pwzLine1) SetLine(1, _pwzLine1, FALSE, NULL);
    if (_pwzLine2) SetLine(2, _pwzLine2, FALSE, NULL);
    if (_pwzLine3) SetLine(3, _pwzLine3, FALSE, NULL);
    return S_OK;
}

STDMETHODIMP CXPProgressDialog::End()
{
    _fInAction = FALSE;
    _spbeginf = 0;
    _fIsEmptyRecycleBin = FALSE;
    _fOperationStarted = FALSE;
    return S_OK;
}

// ============================================================================
// IOperationsProgressDialog
// ============================================================================

STDMETHODIMP CXPProgressDialog::StartProgressDialog(HWND hwndOwner, OPPROGDLGF flags)
{
    Wh_Log(L"IOperationsProgressDialog::StartProgressDialog hwnd=0x%p flags=0x%X", hwndOwner, flags);
    _dwOpsFlags = flags;
    _hwndDlgParent = hwndOwner;
    StrFreeW(&_pwzSourceFolder);
    StrFreeW(&_pwzDestFolder);
    StrFreeW(&_pwzSrcRootPath);
    StrFreeW(&_pwzDstRootPath);

    // Map to our internal Initialize + BeginAction flow
    // XP's SHFileOperation uses a modeless progress dialog — explorer stays interactive
    DWORD initFlags = 0;

    if (!_fInitialized)
    {
        _spinitf = initFlags;
        _fInitialized = TRUE;
    }

    // Set the animation based on the stored operation
    SetOperation(_spaction);

    // Start the UI thread
    return _BeginAction(SPBEGINF_AUTOTIME);
}

STDMETHODIMP CXPProgressDialog::SetOperation(SPACTION action)
{
    Wh_Log(L"SetOperation: %d", (int)action);
    _spaction = action;

    // Load title from XP shell32 string resources (IDS_ACTIONTITLE + FO_*)
    WCHAR wzTitle[128];
    switch (action)
    {
    case SPACTION_MOVING:
        LoadXPString(IDS_ACTIONTITLEMOVE, wzTitle, ARRAYSIZE(wzTitle), L"Moving...");
        SetTitle(wzTitle);
        break;
    case SPACTION_COPYING:
        LoadXPString(IDS_ACTIONTITLECOPY, wzTitle, ARRAYSIZE(wzTitle), L"Copying...");
        SetTitle(wzTitle);
        break;
    case SPACTION_RECYCLING:
    case SPACTION_DELETING:
        LoadXPString(IDS_ACTIONTITLEDEL, wzTitle, ARRAYSIZE(wzTitle), L"Deleting...");
        SetTitle(wzTitle);
        // Check if this is an empty recycle bin operation (detected via SHEmptyRecycleBinW hook)
        // XP uses FOF_SIMPLEPROGRESS for empty recycle bin: static text, no per-file updates
        if (action == SPACTION_DELETING && g_fEmptyingRecycleBin)
        {
            _fIsEmptyRecycleBin = TRUE;
            WCHAR wzEmpty[128];
            LoadXPString(IDS_BB_EMPTYINGWASTEBASKET, wzEmpty, ARRAYSIZE(wzEmpty),
                         L"Emptying the Recycle Bin");
            SetLine(1, wzEmpty, FALSE, NULL);
            SetLine(2, L"", FALSE, NULL);
            SetLine(3, L"", FALSE, NULL);
            // Override animation to IDA_FILENUKE (empty recycle bin)
            if (g_hXPShell32)
                SetAnimation(g_hXPShell32, IDA_FILENUKE);
        }
        break;
    default: break;
    }

    // Set animation from XP shell32 (skip if already set for empty recycle bin)
    if (!_fIsEmptyRecycleBin)
    {
        for (int i = 0; i < ARRAYSIZE(s_actionAnims); i++)
        {
            if (s_actionAnims[i].action == (DWORD)action)
            {
                if (g_hXPShell32)
                    SetAnimation(g_hXPShell32, s_actionAnims[i].idAnim);
                break;
            }
        }
    }
    return S_OK;
}

STDMETHODIMP CXPProgressDialog::SetMode(PDMODE mode)
{
    _pdmode = mode;
    return S_OK;
}

STDMETHODIMP CXPProgressDialog::UpdateProgress(
    ULONGLONG ullPointsCurrent, ULONGLONG ullPointsTotal,
    ULONGLONG ullSizeCurrent, ULONGLONG ullSizeTotal,
    ULONGLONG ullItemsCurrent, ULONGLONG ullItemsTotal)
{
    // Use size-based progress (most meaningful for file copy)
    if (ullSizeTotal > 0)
        return SetProgress64(ullSizeCurrent, ullSizeTotal);
    else if (ullPointsTotal > 0)
        return SetProgress64(ullPointsCurrent, ullPointsTotal);
    else if (ullItemsTotal > 0)
        return SetProgress64(ullItemsCurrent, ullItemsTotal);
    return S_OK;
}

STDMETHODIMP CXPProgressDialog::UpdateLocations(
    IShellItem *psiSource, IShellItem *psiTarget, IShellItem *psiItem)
{
    if (!_fOperationStarted && _hwndProgress)
    {
        _fOperationStarted = TRUE;
        PostMessage(_hwndProgress, PDM_STARTTIMER, 0, 0);
    }

    if (_fIsEmptyRecycleBin)
        return S_OK;

    WCHAR szItemName[MAX_PATH] = {};
    if (psiItem)
    {
        LPWSTR pszName = NULL;
        if (SUCCEEDED(psiItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pszName)) &&
            pszName && pszName[0])
        {
            StringCchCopyW(szItemName, ARRAYSIZE(szItemName), pszName);
            SetLine(1, pszName, TRUE, NULL);
        }
        if (pszName)
            CoTaskMemFree(pszName);
    }

    WCHAR szSrcFile[MAX_PATH] = {};
    WCHAR szDstFile[MAX_PATH] = {};
    WCHAR szSrcParent[MAX_PATH] = {};
    WCHAR szDstParent[MAX_PATH] = {};

    if (!GetShellItemPath(psiItem, szSrcFile, ARRAYSIZE(szSrcFile)))
        GetShellItemPath(psiSource, szSrcFile, ARRAYSIZE(szSrcFile));

    if (!szItemName[0] && szSrcFile[0])
        StringCchCopyW(szItemName, ARRAYSIZE(szItemName), PathFindFileNameW(szSrcFile));

    // XP's SetProgressText (shell32 copy.c:2978) sets Line 2 to the parent-
    // directory NAME of the current source file and of the current destination
    // file. On Win10, psiSource/psiTarget are non-NULL only on the operation's
    // first UpdateLocations call(s); every per-item call passes them NULL. So we
    // cache the source and destination ROOT paths from those first calls and
    // reconstruct each item's destination path = dstRoot + (item relative to
    // srcRoot). Its parent folder name yields "From 'foo' to 'foo'" for items
    // nested in a copied folder (structure is preserved) and the real
    // destination folder name for top-level items — matching XP.
    {
        WCHAR szRoot[MAX_PATH] = {};
        if (GetShellItemPath(psiSource, szRoot, ARRAYSIZE(szRoot)) && szRoot[0])
            StrSetW(&_pwzSrcRootPath, szRoot);
        szRoot[0] = L'\0';
        if (GetShellItemPath(psiTarget, szRoot, ARRAYSIZE(szRoot)) && szRoot[0])
            StrSetW(&_pwzDstRootPath, szRoot);
    }

    if (!GetParentFolderDisplayNameFromPath(szSrcFile, szSrcParent, ARRAYSIZE(szSrcParent)))
        GetShellItemParentDisplayName(psiSource, szSrcParent, ARRAYSIZE(szSrcParent));

    if (szSrcParent[0])
        StrSetW(&_pwzSourceFolder, szSrcParent);
    else if (_pwzSourceFolder)
        StringCchCopyW(szSrcParent, ARRAYSIZE(szSrcParent), _pwzSourceFolder);

    if (_spaction != SPACTION_RECYCLING && _spaction != SPACTION_DELETING)
    {
        // Reconstruct the destination file path from the cached roots, then take
        // its parent folder name (same helper as the source side, so nested
        // items collapse to the identical "foo" on both sides).
        if (szSrcFile[0] && _pwzDstRootPath)
        {
            LPCWSTR pszRel = PathFindFileNameW(szSrcFile);
            if (_pwzSrcRootPath)
            {
                size_t cchRoot = wcslen(_pwzSrcRootPath);
                if (cchRoot > 0 && _wcsnicmp(szSrcFile, _pwzSrcRootPath, cchRoot) == 0)
                {
                    LPCWSTR p = szSrcFile + cchRoot;
                    while (*p == L'\\')
                        p++;
                    if (*p)
                        pszRel = p;
                }
            }

            StringCchCopyW(szDstFile, ARRAYSIZE(szDstFile), _pwzDstRootPath);
            PathAppendW(szDstFile, pszRel);
            GetParentFolderDisplayNameFromPath(szDstFile, szDstParent, ARRAYSIZE(szDstParent));
        }

        // Fallback if the destination root was never reported: preserve XP's
        // structure semantics by mirroring the source parent name.
        if (!szDstParent[0])
        {
            WCHAR szTargetName[MAX_PATH] = {};
            if (GetShellItemDisplayName(psiTarget, szTargetName, ARRAYSIZE(szTargetName)) &&
                (!szItemName[0] || _wcsicmp(szTargetName, szItemName) != 0))
            {
                StringCchCopyW(szDstParent, ARRAYSIZE(szDstParent), szTargetName);
            }
        }

        if (!szDstParent[0] && szSrcParent[0])
            StringCchCopyW(szDstParent, ARRAYSIZE(szDstParent), szSrcParent);

        if (szDstParent[0])
            StrSetW(&_pwzDestFolder, szDstParent);
        else if (_pwzDestFolder)
            StringCchCopyW(szDstParent, ARRAYSIZE(szDstParent), _pwzDestFolder);

        if (szSrcParent[0] || szDstParent[0])
        {
            WCHAR szSrcCompact[MAX_PATH] = {};
            WCHAR szDstCompact[MAX_PATH] = {};
            WCHAR wzLine2[MAX_PATH * 2] = {};

            if (szDstParent[0])
            {
                _CompactFolderNames(szSrcParent, szDstParent,
                                    szSrcCompact, ARRAYSIZE(szSrcCompact),
                                    szDstCompact, ARRAYSIZE(szDstCompact));
                FormatFromTo(IDS_FROMTO, L"From '%1!ls!' to '%2!ls!'",
                             szSrcCompact, szDstCompact, wzLine2, ARRAYSIZE(wzLine2));
            }
            else
            {
                _CompactFolderNames(szSrcParent, NULL,
                                    szSrcCompact, ARRAYSIZE(szSrcCompact),
                                    NULL, 0);
                FormatFromTo(IDS_FROM, L"From '%1!ls!'",
                             szSrcCompact, NULL, wzLine2, ARRAYSIZE(wzLine2));
            }

            SetLine(2, wzLine2, FALSE, NULL);
        }
    }
    else if (szSrcParent[0])
    {
        WCHAR szSrcCompact[MAX_PATH] = {};
        WCHAR wzLine2[MAX_PATH * 2] = {};
        _CompactFolderNames(szSrcParent, NULL,
                            szSrcCompact, ARRAYSIZE(szSrcCompact),
                            NULL, 0);
        FormatFromTo(IDS_FROM, L"From '%1!ls!'",
                     szSrcCompact, NULL, wzLine2, ARRAYSIZE(wzLine2));
        SetLine(2, wzLine2, FALSE, NULL);
    }

    return S_OK;
}

STDMETHODIMP CXPProgressDialog::ResetTimer()
{
    return Timer(PDTIMER_RESET, NULL);
}

STDMETHODIMP CXPProgressDialog::PauseTimer()
{
    // Not critical for XP dialog, just stop animation
    _PauseAnimation(TRUE);
    return S_OK;
}

STDMETHODIMP CXPProgressDialog::ResumeTimer()
{
    _PauseAnimation(FALSE);
    return S_OK;
}

STDMETHODIMP CXPProgressDialog::GetMilliseconds(
    ULONGLONG *pullElapsed, ULONGLONG *pullRemaining)
{
    if (pullElapsed)
        *pullElapsed = (_dwStartTime > 0) ? (GetTickCount() - _dwStartTime) : 0;
    if (pullRemaining)
        *pullRemaining = 0; // we don't track this separately
    return S_OK;
}

STDMETHODIMP CXPProgressDialog::GetOperationStatus(PDOPSTATUS *popstatus)
{
    if (!popstatus) return E_POINTER;
    if (_fCancel)
        *popstatus = PDOPS_CANCELLED;
    else if (_fInAction)
        *popstatus = PDOPS_RUNNING;
    else
        *popstatus = PDOPS_STOPPED;
    return S_OK;
}

// ============================================================================
// CoCreateInstance hook
// ============================================================================

typedef HRESULT(WINAPI* CoCreateInstance_t)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
CoCreateInstance_t CoCreateInstanceOrig = NULL;

HRESULT WINAPI CoCreateInstanceHook(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
    DWORD dwClsContext, REFIID riid, LPVOID* ppv)
{
    if (rclsid == CLSID_ProgressDialog && !pUnkOuter &&
        g_hXPShell32 && HasXPDialogResource(DLG_MOVECOPYPROGRESS))
    {
        Wh_Log(L"CoCreateInstance intercepted for CLSID_ProgressDialog");
        CXPProgressDialog* pDlg = new CXPProgressDialog();
        if (!pDlg)
            return E_OUTOFMEMORY;

        HRESULT hr = pDlg->QueryInterface(riid, ppv);
        pDlg->Release();
        if (FAILED(hr))
            Wh_Log(L"QueryInterface failed: 0x%08X", hr);
        return hr;
    }

    return CoCreateInstanceOrig(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}

// ============================================================================
// SHELL32_CanDisplayWin8CopyDialog hook (keep forcing legacy path)
// ============================================================================

typedef BOOL(*SHELL32_CanDisplayWin8CopyDialogFunc)();
BOOL(*SHELL32_CanDisplayWin8CopyDialogOrig)();
BOOL SHELL32_CanDisplayWin8CopyDialogHook()
{
    return FALSE;
}

// ============================================================================
// Windhawk mod entry points
// ============================================================================

void LoadSettings()
{
    LPCWSTR path = Wh_GetStringSetting(L"xpShell32Path");
    if (path && path[0])
        StringCchCopyW(g_xpShell32Path, ARRAYSIZE(g_xpShell32Path), path);
    else
        g_xpShell32Path[0] = L'\0';
    Wh_FreeStringSetting(path);

    g_showShortcutDeleteDialog = Wh_GetIntSetting(L"showShortcutDeleteDialog");
    g_showProgramDeleteDialog = Wh_GetIntSetting(L"showProgramDeleteDialog");
    g_useClassicConfirmations = Wh_GetIntSetting(L"useClassicConfirmations");
    g_showTimeEstimate = Wh_GetIntSetting(L"showTimeEstimate");
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init - Pre-Vista File Copy Dialog");
    InitializeCriticalSection(&g_cs);

    LoadSettings();

    // Load XP shell32.dll for animations/dialogs if path is configured
    if (g_xpShell32Path[0])
    {
        g_hXPShell32 = LoadLibraryExW(g_xpShell32Path, NULL,
            LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
        if (g_hXPShell32)
            Wh_Log(L"Loaded XP shell32.dll for animations/dialogs");
        else
            Wh_Log(L"Failed to load XP shell32.dll: %s", g_xpShell32Path);
    }

    // Hook SHEmptyRecycleBinW to detect empty recycle bin operations
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (hShell32)
    {
        void* pEmpty = (void*)GetProcAddress(hShell32, "SHEmptyRecycleBinW");
        if (pEmpty)
        {
            Wh_SetFunctionHook(pEmpty,
                (void*)SHEmptyRecycleBinWHook,
                (void**)&SHEmptyRecycleBinWOrig);
            Wh_Log(L"Hooked SHEmptyRecycleBinW");
        }
    }

    // Hook SHELL32_CanDisplayWin8CopyDialog to force legacy code path
    HMODULE hExports = LoadLibraryW(L"ext-ms-win-shell-exports-internal-l1-1-0.dll");
    if (hExports)
    {
        void* origFunc = (void*)GetProcAddress(hExports, "SHELL32_CanDisplayWin8CopyDialog");
        if (origFunc)
        {
            Wh_SetFunctionHook(origFunc,
                (void*)SHELL32_CanDisplayWin8CopyDialogHook,
                (void**)&SHELL32_CanDisplayWin8CopyDialogOrig);
        }
    }

    if (hShell32)
    {
        WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
            {
                {
                    LR"(public: virtual long __cdecl CTransferConfirmation::Confirm(struct CONFIRMOP const *,enum CONFIRMATIONRESPONSE *,int *,unsigned int *))"
                },
                &CTransferConfirmation_Confirm_Orig,
                CTransferConfirmation_Confirm_Hook
            },
            {
                {
                    LR"(public: static long __cdecl CConflictResolutionDlg::ShowDialog(struct CONFIRM_CONFLICT_PARAMS const *,struct ISyncMgrConflictItems *,struct CONFIRM_CONFLICT_RESULT *,struct ISyncMgrConflictResolutionItems * *))"
                },
                &CConflictResolutionDlg_ShowDialog_Orig,
                CConflictResolutionDlg_ShowDialog_Hook
            },
        };

        if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks)))
        {
            Wh_Log(L"Failed to hook shell32 transfer confirmation symbols");
        }
        else
        {
            Wh_Log(L"Hooked CTransferConfirmation::Confirm");
            Wh_Log(L"Hooked CConflictResolutionDlg::ShowDialog");
        }
    }

    // Hook CoCreateInstance to intercept CLSID_ProgressDialog
    // On Win10+, CoCreateInstance lives in combase.dll (ole32.dll forwards to it)
    HMODULE hComBase = GetModuleHandleW(L"combase.dll");
    if (!hComBase) hComBase = LoadLibraryW(L"combase.dll");
    if (hComBase)
    {
        void* pCoCreate = (void*)GetProcAddress(hComBase, "CoCreateInstance");
        if (pCoCreate)
        {
            Wh_SetFunctionHook(pCoCreate,
                (void*)CoCreateInstanceHook,
                (void**)&CoCreateInstanceOrig);
            Wh_Log(L"Hooked CoCreateInstance in combase.dll");
        }
        else
        {
            Wh_Log(L"CoCreateInstance not found in combase.dll");
        }
    }
    else
    {
        // Fallback: try ole32.dll
        HMODULE hOle32 = GetModuleHandleW(L"ole32.dll");
        if (!hOle32) hOle32 = LoadLibraryW(L"ole32.dll");
        if (hOle32)
        {
            void* pCoCreate = (void*)GetProcAddress(hOle32, "CoCreateInstance");
            if (pCoCreate)
            {
                Wh_SetFunctionHook(pCoCreate,
                    (void*)CoCreateInstanceHook,
                    (void**)&CoCreateInstanceOrig);
                Wh_Log(L"Hooked CoCreateInstance in ole32.dll (fallback)");
            }
        }
    }

    return TRUE;
}

void Wh_ModUninit(void)
{
    Wh_Log(L"Uninit - Pre-Vista File Copy Dialog");

    if (g_hXPShell32)
    {
        FreeLibrary(g_hXPShell32);
        g_hXPShell32 = NULL;
    }

    DeleteCriticalSection(&g_cs);
}

void Wh_ModSettingsChanged(void)
{
    Wh_Log(L"Settings changed");

    // Reload XP shell32 path
    if (g_hXPShell32)
    {
        FreeLibrary(g_hXPShell32);
        g_hXPShell32 = NULL;
    }

    LoadSettings();

    if (g_xpShell32Path[0])
    {
        g_hXPShell32 = LoadLibraryExW(g_xpShell32Path, NULL,
            LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    }
}
