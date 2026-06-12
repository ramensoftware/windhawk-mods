// ==WindhawkMod==
// @id              prevista-file-copy
// @name            Pre-Vista File Copy Dialog
// @description     Replaces the file copy dialog with a pre-Vista style progress dialog
// @version         2.1.0
// @author          arceus413
// @github          https://github.com/arceuss
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldbghelp -luxtheme -lcomctl32 -lshlwapi -lgdi32 -luuid
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Pre-Vista File Copy Dialog
This mod replaces the modern file copy/move/delete progress dialog with a
pre-Vista style classic progress dialog.

Based on the original Windows XP CProgressDialog implementation from browseui.
Hooks CoCreateInstance to intercept CLSID_ProgressDialog and provide a custom
implementation that reproduces XP's behavior for copy, move, delete,
recycle, and empty recycle bin operations, including animations, per-file
progress text, and time estimates.

Works with shell32.dll from Windows 95 up to XP for loading the original AVI
animations and strings.

![Copying](https://i.imgur.com/wWq5rhx.png)
![Moving](https://i.imgur.com/Tiey5JK.png)
![Deleting](https://i.imgur.com/5uFWHay.png)
![Emptying the Recycle Bin](https://i.imgur.com/nBMB7wB.png)

## Settings
- **Shell32 Path**: Point this to a pre-Vista shell32.dll to load the original
  AVI animations (filecopy, filemove, filedel, filenuke, filedelreal). If not set
  or the file doesn't exist, animations will be skipped gracefully.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- xpShell32Path: ""
  $name: Shell32.dll path
  $description: >-
    Full path to a pre-Vista shell32.dll for loading AVI animations and strings.
    Leave empty to skip animations.
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
#include <uxtheme.h>
#include <dbghelp.h>
#include <strsafe.h>

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

// Dialog control IDs matching XP's resource.h
#define IDD_PROGDLG_ANIMATION   0x150
#define IDD_PROGDLG_LINE1       0x151
#define IDD_PROGDLG_LINE2       0x152
#define IDD_PROGDLG_LINE3       0x153
#define IDD_PROGDLG_PROGRESSBAR 0x154

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
// Dialog creation: Build XP dialog template in memory
// ============================================================================

#pragma pack(push, 2)
struct DLGTEMPLATEEX_HEADER {
    WORD dlgVer;
    WORD signature;
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    WORD cDlgItems;
    short x, y, cx, cy;
};

struct DLGITEMTEMPLATEEX_HEADER {
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    short x, y, cx, cy;
    DWORD id;
};
#pragma pack(pop)

// Helper to write a WORD-aligned wide string into a buffer
static size_t WriteWStr(BYTE* buf, const WCHAR* str)
{
    size_t len = (wcslen(str) + 1) * sizeof(WCHAR);
    memcpy(buf, str, len);
    return len;
}

static size_t AlignTo(size_t offset, size_t alignment)
{
    return (offset + alignment - 1) & ~(alignment - 1);
}

HWND CXPProgressDialog::_CreateXPDialog(HWND hwndParent)
{
    // Build a DLGTEMPLATEEX in memory matching XP's DLG_PROGRESSDIALOG:
    // 250x84 DLU, with Animation, 3 static lines, progress bar, cancel button
    BYTE buf[2048];
    memset(buf, 0, sizeof(buf));
    size_t offset = 0;

    // DLGTEMPLATEEX header
    DLGTEMPLATEEX_HEADER* hdr = (DLGTEMPLATEEX_HEADER*)buf;
    hdr->dlgVer = 1;
    hdr->signature = 0xFFFF;
    hdr->helpID = 0;
    hdr->exStyle = 0;
    hdr->style = DS_MODALFRAME | DS_SETFONT | DS_NOIDLEMSG |
                 WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    hdr->cDlgItems = 6; // animation, line1, line2, progressbar, cancel, line3
    hdr->x = 20; hdr->y = 20;
    hdr->cx = 250; hdr->cy = 84;
    offset = sizeof(DLGTEMPLATEEX_HEADER);

    // Menu (none)
    *(WORD*)(buf + offset) = 0; offset += 2;
    // Window class (default)
    *(WORD*)(buf + offset) = 0; offset += 2;
    // Title (empty initially)
    *(WORD*)(buf + offset) = 0; offset += 2;

    // Font (DS_SETFONT): size, weight, italic, charset, typeface
    *(WORD*)(buf + offset) = 8; offset += 2;    // point size
    *(WORD*)(buf + offset) = FW_NORMAL; offset += 2; // weight
    *(BYTE*)(buf + offset) = FALSE; offset += 1; // italic
    *(BYTE*)(buf + offset) = DEFAULT_CHARSET; offset += 1; // charset
    offset += WriteWStr(buf + offset, L"MS Shell Dlg");

    // ---- Control items ----
    // Each control must be DWORD-aligned

    // 1) Animation control: (7,0) 236x25 ANIMATE_CLASS ACS_TRANSPARENT|ACS_AUTOPLAY|ACS_TIMER
    offset = AlignTo(offset, 4);
    {
        DLGITEMTEMPLATEEX_HEADER* item = (DLGITEMTEMPLATEEX_HEADER*)(buf + offset);
        item->helpID = 0;
        item->exStyle = 0;
        // ACS_CENTER=0x0001, ACS_TRANSPARENT=0x0002, ACS_AUTOPLAY=0x0004, ACS_TIMER=0x0008
        item->style = WS_CHILD | WS_VISIBLE | 0x0002 | 0x0004 | 0x0008;
        item->x = 7; item->y = 0; item->cx = 236; item->cy = 25;
        item->id = IDD_PROGDLG_ANIMATION;
        offset += sizeof(DLGITEMTEMPLATEEX_HEADER);
        // Window class as string
        offset += WriteWStr(buf + offset, ANIMATE_CLASSW);
        // Title (none)
        *(WORD*)(buf + offset) = 0; offset += 2;
        // Extra data size
        *(WORD*)(buf + offset) = 0; offset += 2;
    }

    // 2) Line 1: Static (7,38) 236x10 SS_LEFTNOWORDWRAP|SS_NOPREFIX
    offset = AlignTo(offset, 4);
    {
        DLGITEMTEMPLATEEX_HEADER* item = (DLGITEMTEMPLATEEX_HEADER*)(buf + offset);
        item->helpID = 0; item->exStyle = 0;
        item->style = WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP | SS_NOPREFIX;
        item->x = 7; item->y = 38; item->cx = 236; item->cy = 10;
        item->id = IDD_PROGDLG_LINE1;
        offset += sizeof(DLGITEMTEMPLATEEX_HEADER);
        // class: Static = 0x0082
        *(WORD*)(buf + offset) = 0xFFFF; offset += 2;
        *(WORD*)(buf + offset) = 0x0082; offset += 2;
        *(WORD*)(buf + offset) = 0; offset += 2; // title
        *(WORD*)(buf + offset) = 0; offset += 2; // extra
    }

    // 3) Line 2: Static (7,48) 236x10 SS_LEFTNOWORDWRAP|SS_NOPREFIX
    offset = AlignTo(offset, 4);
    {
        DLGITEMTEMPLATEEX_HEADER* item = (DLGITEMTEMPLATEEX_HEADER*)(buf + offset);
        item->helpID = 0; item->exStyle = 0;
        item->style = WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP | SS_NOPREFIX;
        item->x = 7; item->y = 48; item->cx = 236; item->cy = 10;
        item->id = IDD_PROGDLG_LINE2;
        offset += sizeof(DLGITEMTEMPLATEEX_HEADER);
        *(WORD*)(buf + offset) = 0xFFFF; offset += 2;
        *(WORD*)(buf + offset) = 0x0082; offset += 2;
        *(WORD*)(buf + offset) = 0; offset += 2;
        *(WORD*)(buf + offset) = 0; offset += 2;
    }

    // 4) Progress bar: (7,63) 190x8 PROGRESS_CLASS
    offset = AlignTo(offset, 4);
    {
        DLGITEMTEMPLATEEX_HEADER* item = (DLGITEMTEMPLATEEX_HEADER*)(buf + offset);
        item->helpID = 0; item->exStyle = 0;
        item->style = WS_CHILD | WS_VISIBLE;
        item->x = 7; item->y = 63; item->cx = 190; item->cy = 8;
        item->id = IDD_PROGDLG_PROGRESSBAR;
        offset += sizeof(DLGITEMTEMPLATEEX_HEADER);
        offset += WriteWStr(buf + offset, PROGRESS_CLASSW);
        *(WORD*)(buf + offset) = 0; offset += 2; // title
        *(WORD*)(buf + offset) = 0; offset += 2; // extra
    }

    // 5) Cancel button: (202,63) 40x14 BS_DEFPUSHBUTTON
    offset = AlignTo(offset, 4);
    {
        DLGITEMTEMPLATEEX_HEADER* item = (DLGITEMTEMPLATEEX_HEADER*)(buf + offset);
        item->helpID = 0; item->exStyle = 0;
        item->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON;
        item->x = 202; item->y = 63; item->cx = 40; item->cy = 14;
        item->id = IDCANCEL;
        offset += sizeof(DLGITEMTEMPLATEEX_HEADER);
        // class: Button = 0x0080
        *(WORD*)(buf + offset) = 0xFFFF; offset += 2;
        *(WORD*)(buf + offset) = 0x0080; offset += 2;
        offset += WriteWStr(buf + offset, L"Cancel");
        *(WORD*)(buf + offset) = 0; offset += 2; // extra
    }

    // 6) Line 3: Static (7,74) 192x10 SS_LEFTNOWORDWRAP|SS_NOPREFIX
    offset = AlignTo(offset, 4);
    {
        DLGITEMTEMPLATEEX_HEADER* item = (DLGITEMTEMPLATEEX_HEADER*)(buf + offset);
        item->helpID = 0; item->exStyle = 0;
        item->style = WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP | SS_NOPREFIX;
        item->x = 7; item->y = 74; item->cx = 192; item->cy = 10;
        item->id = IDD_PROGDLG_LINE3;
        offset += sizeof(DLGITEMTEMPLATEEX_HEADER);
        *(WORD*)(buf + offset) = 0xFFFF; offset += 2;
        *(WORD*)(buf + offset) = 0x0082; offset += 2;
        *(WORD*)(buf + offset) = 0; offset += 2;
        *(WORD*)(buf + offset) = 0; offset += 2;
    }

    return CreateDialogIndirectParamW(
        NULL, (LPCDLGTEMPLATEW)buf, hwndParent,
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
        StrSetW(&_pwzCancelMsg, L"Cancelling...");

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
    // Start the deferred show timer on first location update
    if (!_fOperationStarted && _hwndProgress)
    {
        _fOperationStarted = TRUE;
        PostMessage(_hwndProgress, PDM_STARTTIMER, 0, 0);
    }

    // For emptying recycle bin, XP uses FOF_SIMPLEPROGRESS
    // which suppresses per-file line updates — just shows static text
    if (_fIsEmptyRecycleBin)
        return S_OK;

    LPWSTR pszName = NULL;

    // Line 1: show the current item's filename, compacted to fit control width
    // (XP does PathStripPath + PathCompactPath on the source path)
    if (psiItem)
    {
        if (SUCCEEDED(psiItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pszName)))
        {
            SetLine(1, pszName, TRUE, NULL);
            CoTaskMemFree(pszName);
            pszName = NULL;
        }
    }

    // Line 2: XP derives folder names from full FILE paths using
    // PathRemoveFileSpec + PathStripPath on BOTH source and destination file paths.
    // IOperationsProgressDialog gives us psiSource (top-level source folder),
    // psiTarget (top-level dest folder), and psiItem (current source file).
    // To reconstruct the destination file path (which XP has natively), we compute:
    //   relPath = psiItem path minus psiSource prefix
    //   dstFilePath = psiTarget + "\" + relPath
    // Then PathRemoveFileSpec + PathStripPath on both source and dest file paths.

    WCHAR szSrcFile[MAX_PATH] = {0};
    WCHAR szSrcFolder[MAX_PATH] = {0};
    WCHAR szSrcParent[MAX_PATH] = {0};

    // Get psiItem full path and derive source parent folder name
    if (psiItem)
    {
        LPWSTR pszItemPath = NULL;
        if (SUCCEEDED(psiItem->GetDisplayName(SIGDN_FILESYSPATH, &pszItemPath)) && pszItemPath)
        {
            StringCchCopyW(szSrcFile, ARRAYSIZE(szSrcFile), pszItemPath);
            CoTaskMemFree(pszItemPath);

            StringCchCopyW(szSrcParent, ARRAYSIZE(szSrcParent), szSrcFile);
            PathRemoveFileSpecW(szSrcParent);   // "E:\shaders\file.txt" → "E:\shaders"
            PathStripPathW(szSrcParent);        // "E:\shaders" → "shaders"
        }
    }
    // Fallback: use psiSource display name
    if (!szSrcParent[0] && psiSource)
    {
        LPWSTR pszPath = NULL;
        if (SUCCEEDED(psiSource->GetDisplayName(SIGDN_NORMALDISPLAY, &pszPath)) && pszPath)
        {
            StringCchCopyW(szSrcParent, ARRAYSIZE(szSrcParent), pszPath);
            CoTaskMemFree(pszPath);
        }
    }

    // Get psiSource path for computing relative paths
    if (psiSource)
    {
        LPWSTR pszPath = NULL;
        if (SUCCEEDED(psiSource->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)) && pszPath)
        {
            StringCchCopyW(szSrcFolder, ARRAYSIZE(szSrcFolder), pszPath);
            CoTaskMemFree(pszPath);
        }
    }

    if (szSrcParent[0])
    {
        if (_spaction != SPACTION_RECYCLING && _spaction != SPACTION_DELETING)
        {
            // Derive destination folder name
            WCHAR szDstParent[MAX_PATH] = {0};

            if (psiTarget)
            {
                LPWSTR pszDstPath = NULL;
                if (SUCCEEDED(psiTarget->GetDisplayName(SIGDN_FILESYSPATH, &pszDstPath)) && pszDstPath)
                {
                    if (szSrcFile[0] && szSrcFolder[0])
                    {
                        // Compute relative path of psiItem within psiSource
                        size_t srcFolderLen = wcslen(szSrcFolder);
                        if (srcFolderLen > 0 && szSrcFolder[srcFolderLen - 1] != L'\\')
                            srcFolderLen++; // skip the separator
                        LPCWSTR relPath = szSrcFile + srcFolderLen;

                        // Build full destination file path
                        WCHAR szDstFile[MAX_PATH];
                        StringCchCopyW(szDstFile, ARRAYSIZE(szDstFile), pszDstPath);
                        PathAppendW(szDstFile, relPath);

                        // XP approach: PathRemoveFileSpec + PathStripPath
                        PathRemoveFileSpecW(szDstFile);
                        PathStripPathW(szDstFile);
                        StringCchCopyW(szDstParent, ARRAYSIZE(szDstParent), szDstFile);
                    }
                    else
                    {
                        StringCchCopyW(szDstParent, ARRAYSIZE(szDstParent), pszDstPath);
                        PathStripPathW(szDstParent);
                    }
                    CoTaskMemFree(pszDstPath);
                    pszDstPath = NULL;
                }
                if (!szDstParent[0])
                {
                    if (SUCCEEDED(psiTarget->GetDisplayName(SIGDN_NORMALDISPLAY, &pszDstPath)) && pszDstPath)
                    {
                        StringCchCopyW(szDstParent, ARRAYSIZE(szDstParent), pszDstPath);
                        CoTaskMemFree(pszDstPath);
                    }
                }
            }

            // Fallback: for copy/move, the destination folder name is usually
            // identical to the source folder name (folder structure is preserved)
            if (!szDstParent[0])
                StringCchCopyW(szDstParent, ARRAYSIZE(szDstParent), szSrcParent);

            // XP compacts each folder name individually before formatting
            WCHAR szSrcCompact[MAX_PATH], szDstCompact[MAX_PATH];
            _CompactFolderNames(szSrcParent, szDstParent,
                                szSrcCompact, ARRAYSIZE(szSrcCompact),
                                szDstCompact, ARRAYSIZE(szDstCompact));

            WCHAR wzLine2[MAX_PATH * 2];
            FormatFromTo(IDS_FROMTO, L"From '%1!ls!' to '%2!ls!'",
                         szSrcCompact, szDstCompact, wzLine2, ARRAYSIZE(wzLine2));
            SetLine(2, wzLine2, FALSE, NULL);
        }
        else
        {
            // Recycle: only source folder, compacted to full width
            WCHAR szSrcCompact[MAX_PATH];
            _CompactFolderNames(szSrcParent, NULL,
                                szSrcCompact, ARRAYSIZE(szSrcCompact),
                                NULL, 0);

            WCHAR wzLine2[MAX_PATH * 2];
            FormatFromTo(IDS_FROM, L"From '%1!ls!'",
                         szSrcCompact, NULL, wzLine2, ARRAYSIZE(wzLine2));
            SetLine(2, wzLine2, FALSE, NULL);
        }
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
    if (rclsid == CLSID_ProgressDialog && !pUnkOuter)
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

    g_showTimeEstimate = Wh_GetIntSetting(L"showTimeEstimate");
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init - Pre-Vista File Copy Dialog");
    InitializeCriticalSection(&g_cs);

    LoadSettings();

    // Load XP shell32.dll for animations if path is configured
    if (g_xpShell32Path[0])
    {
        g_hXPShell32 = LoadLibraryExW(g_xpShell32Path, NULL,
            LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
        if (g_hXPShell32)
            Wh_Log(L"Loaded XP shell32.dll for animations");
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
