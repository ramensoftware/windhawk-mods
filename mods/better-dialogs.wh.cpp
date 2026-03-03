// ==WindhawkMod==
// @id              better-dialogs
// @name            Better Dialogs
// @description     Improves Windows dialog boxes.
// @version         1.0
// @author          FireBlade
// @github          https://github.com/FireBlade211
// @include         *
// @compilerOptions -lcomdlg32 -luser32 -lshell32 -lole32 -luuid -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Better Dialogs
This mod improves certain Windows dialogs. It also re-translates messages for those dialogs to make sure that apps that expect the original dialogs still work properly.


More dialogs coming soon! (or maybe not so soon, just in the future)

## Dialogs changed
- Message boxes changed to Task Dialogs
- Legacy folder picker replaced with modern directory selector
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able
# to configure. Metadata values such as $name and $description are optional.
# Check out the documentation for more information:
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings
- messageTaskDlg: true
  $name: Replace Message Boxes with Task Dialogs
  $description: Replace Windows message boxes with task dialogs.
- modernFolderBrowser: true
  $name: Use modern folder picker dialog
  $description: Replace the old folder picker dialog with the modern version.
- modernFbShowLpszTitle: false
  $name: Show dialog instructions in modern folder picker
  $description: Show the application-provided dialog instructions in the modern folder picker dialog (not recommended).
# - betterColorPicker: false
#   $name: Use improved Color Dialog
#   $description: Use the improved color picker dialog.
# - betterPickIconDlg: true
#   $name: Use improved Icon Picker
#   $description: Use the improved icon chooser dialog.
*/
// ==/WindhawkModSettings==

#include <commdlg.h>
#include <commctrl.h>
#include <winuser.h>
#include <shlobj.h>
#include <colordlg.h>

struct {
    bool messageTaskDlg;
    bool modernFolderBrowser;
    bool modernFbShowLpszTitle;
    //bool betterColorPicker;
    //bool betterPickIconDlg;
} settings;

using MessageBoxW_t = decltype(&MessageBoxW);
using MessageBoxIndirectW_t = decltype(&MessageBoxIndirectW);
using SHBrowseForFolderW_t = decltype(&SHBrowseForFolderW);
using ChooseColorW_t = decltype(&ChooseColorW);
//using PickIconDlg_t = decltype(&PickIconDlg);

MessageBoxW_t MessageBoxW_Original;
MessageBoxIndirectW_t MessageBoxIndirectW_Original;
SHBrowseForFolderW_t SHBrowseForFolderW_Original;
ChooseColorW_t ChooseColorW_Original;
//PickIconDlg_t PickIconDlg_Original;

void LoadSettings() {
    settings.messageTaskDlg = Wh_GetIntSetting(L"messageTaskDlg");
    settings.modernFolderBrowser = Wh_GetIntSetting(L"modernFolderBrowser");
    settings.modernFbShowLpszTitle = Wh_GetIntSetting(L"modernFbShowLpszTitle");
    //settings.betterColorPicker = Wh_GetIntSetting(L"betterColorPicker");
    //settings.betterPickIconDlg = Wh_GetIntSetting(L"betterPickIconDlg");
}

#define FLAG(v, f) ((v & f) != 0)

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#define TDCBF_ABORT 0x00010000
#define TDCBF_IGNORE 0x00020000
#define TDCBF_CONTINUE 0x00080000
#define TDCBF_HELP 0x00100000

typedef HRESULT (WINAPI *TaskDialog_t)(
    HWND,
    HINSTANCE,
    PCWSTR,
    PCWSTR,
    PCWSTR,
    TASKDIALOG_COMMON_BUTTON_FLAGS,
    PCWSTR,
    int*
);

typedef HRESULT (WINAPI *TaskDialogIndirect_t)(
  const TASKDIALOGCONFIG *pTaskConfig,
  int                    *pnButton,
  int                    *pnRadioButton,
  BOOL                   *pfVerificationFlagChecked
);

HRESULT CALLBACK MessageBoxW_TaskDialogCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData);

typedef struct tagMSGBOXTASKDLGHELPINFO {
    MSGBOXCALLBACK callback;
    DWORD_PTR dwContextHelpId;
} MSGBOXTASKDLGHELPINFO, *LPMSGBOXTASKDLGHELPINFO;

int WINAPI MessageBoxW_Hook(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
    if (settings.messageTaskDlg) {
        int button = 0;

        // unsupported
        // if ((uType & MB_ICONQUESTION) != 0) {
        //     HRESULT hr = TaskDialog(hWnd, )
        // }

        PCWSTR pszIcon = NULL;

        if (FLAG(uType, MB_ICONERROR) || FLAG(uType, MB_ICONSTOP)
        || FLAG(uType, MB_ICONHAND)) pszIcon = TD_ERROR_ICON;

        else if (FLAG(uType, MB_ICONINFORMATION) || FLAG(uType, MB_ICONASTERISK))
            pszIcon = TD_INFORMATION_ICON;

        else if (FLAG(uType, MB_ICONQUESTION)) // task dialogs do not have a question icon
                                               // we could fix this with SHGetStockIconInfo but we'd have to rewrite
            pszIcon = NULL;

        else if (FLAG(uType, MB_ICONEXCLAMATION) || FLAG(uType, MB_ICONWARNING))
            pszIcon = TD_WARNING_ICON;

        
        if (FLAG(uType, MB_DEFBUTTON1)) button = 0;
        else if (FLAG(uType, MB_DEFBUTTON2)) button = 1;
        else if (FLAG(uType, MB_DEFBUTTON3)) button = 2;
        else if (FLAG(uType, MB_DEFBUTTON4)) button = 3;

        int nButtonId = 0;
        BOOL help = FLAG(uType, MB_HELP);

        TASKDIALOG_COMMON_BUTTON_FLAGS buttons = NULL;
        
        switch (uType & MB_TYPEMASK) {
        case MB_ABORTRETRYIGNORE:
            buttons = TDCBF_ABORT | TDCBF_RETRY_BUTTON | TDCBF_IGNORE;

            switch (button) {
                case 0:
                    nButtonId = IDABORT;
                    break;
                case 1:
                    nButtonId = IDRETRY;
                    break;
                case 2:
                    nButtonId = IDIGNORE;
                    break;
                case 3:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        case MB_CANCELTRYCONTINUE:
            buttons = TDCBF_CANCEL_BUTTON | TDCBF_RETRY_BUTTON | TDCBF_CONTINUE;
            switch (button) {
                case 0:
                    nButtonId = IDCANCEL;
                    break;
                case 1:
                    nButtonId = IDRETRY;
                    break;
                case 2:
                    nButtonId = IDCONTINUE;
                    break;
                case 3:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        case MB_YESNOCANCEL:
            buttons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON | TDCBF_CANCEL_BUTTON;
            switch (button) {
                case 0:
                    nButtonId = IDYES;
                    break;
                case 1:
                    nButtonId = IDNO;
                    break;
                case 2:
                    nButtonId = IDCANCEL;
                    break;
                case 3:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        case MB_OKCANCEL:
            buttons = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
            switch (button) {
                case 0:
                    nButtonId = IDOK;
                    break;
                case 1:
                    nButtonId = IDCANCEL;
                    break;
                case 2:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        case MB_RETRYCANCEL:
            buttons = TDCBF_RETRY_BUTTON | TDCBF_CANCEL_BUTTON;
            switch (button) {
                case 0:
                    nButtonId = IDRETRY;
                    break;
                case 1:
                    nButtonId = IDCANCEL;
                    break;
                case 2:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        case MB_YESNO:
            buttons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
            switch (button) {
                case 0:
                    nButtonId = IDYES;
                    break;
                case 1:
                    nButtonId = IDNO;
                    break;
                case 2:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        default: // MB_OK
            buttons = TDCBF_OK_BUTTON;
            switch (button) {
                case 0:
                    nButtonId = IDOK;
                    break;
                case 1:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        }

        if (help)
            buttons |= TDCBF_HELP;

        PCWSTR caption = lpCaption != NULL ? lpCaption : L"Error";

        HMODULE hComCtl = LoadLibraryW(L"comctl32.dll");
        if (hComCtl) {
            // TaskDialog_t pTaskDialog = (TaskDialog_t)GetProcAddress(hComCtl, "TaskDialog");

            // if (!pTaskDialog)
            //     return MessageBoxW_Original(hWnd, lpText, lpCaption, uType);

            // HRESULT hr = pTaskDialog(
            //     hWnd,
            //     NULL,
            //     caption,
            //     caption,
            //     lpText,
            //     buttons,
            //     pszIcon,
            //     &button
            // );

            // if (SUCCEEDED(hr))
            // {
            //     return button;
            // }

            TaskDialogIndirect_t pTaskDialogIndirect = (TaskDialogIndirect_t)GetProcAddress(hComCtl, "TaskDialogIndirect");

            if (!pTaskDialogIndirect)
                return MessageBoxW_Original(hWnd, lpText, lpCaption, uType);

            TASKDIALOGCONFIG tdc = {};
            tdc.cbSize = sizeof(tdc);
            tdc.dwCommonButtons = buttons;
            tdc.pszMainIcon = pszIcon;
            tdc.hwndParent = hWnd;
            tdc.pszContent = lpText;
            tdc.pszWindowTitle = caption;
            tdc.pszMainInstruction = caption;
            tdc.dwFlags = TDF_SIZE_TO_CONTENT;
            tdc.pfCallback = MessageBoxW_TaskDialogCallback;
            tdc.nDefaultButton = nButtonId;
            
            int nButton = 0;
            HRESULT hr = pTaskDialogIndirect(&tdc, &nButton, NULL, NULL);

            if (SUCCEEDED(hr))
            {
                return nButton;
            }
        }

        return MessageBoxW_Original(hWnd, lpText, lpCaption, uType);
    }
    else return MessageBoxW_Original(hWnd, lpText, lpCaption, uType);
}

HRESULT CALLBACK MessageBoxW_TaskDialogCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData) {
    switch (uMsg) {
        case TDN_BUTTON_CLICKED:
        {
            if (wParam == IDHELP) {
                HELPINFO hi = { 0 };
                hi.cbSize = sizeof(hi);
                hi.iContextType = HELPINFO_WINDOW;
                hi.iCtrlId = 0;
                hi.hItemHandle = hwnd;

                POINT pt;
                GetCursorPos(&pt);
                hi.MousePos = pt;

                if (lpRefData) {
                    LPMSGBOXTASKDLGHELPINFO pinfo = (LPMSGBOXTASKDLGHELPINFO) lpRefData;

                    hi.dwContextId = pinfo->dwContextHelpId;

                    if (pinfo->callback) {
                        pinfo->callback(&hi);
                        return S_FALSE;
                    }
                }

                HWND hwndOwner = GetWindow(hwnd, GW_OWNER);

                SendMessageW(hwndOwner, WM_HELP, (WPARAM)hwnd, (LPARAM)&hi);

                return S_FALSE;
            }

            break;
        }
    }

    return S_OK;
}

int WINAPI MessageBoxIndirectW_Hook(const MSGBOXPARAMSW *lpmbp) {
    if (settings.messageTaskDlg) {
        int button = 0;

        // unsupported
        // if ((uType & MB_ICONQUESTION) != 0) {
        //     HRESULT hr = TaskDialog(hWnd, )
        // }

        PCWSTR pszIcon = NULL;

        if (FLAG(lpmbp->dwStyle, MB_ICONERROR) || FLAG(lpmbp->dwStyle, MB_ICONSTOP)
        || FLAG(lpmbp->dwStyle, MB_ICONHAND)) pszIcon = TD_ERROR_ICON;

        else if (FLAG(lpmbp->dwStyle, MB_ICONINFORMATION) || FLAG(lpmbp->dwStyle, MB_ICONASTERISK))
            pszIcon = TD_INFORMATION_ICON;

        else if (FLAG(lpmbp->dwStyle, MB_ICONQUESTION))
            pszIcon = NULL;

        else if (FLAG(lpmbp->dwStyle, MB_ICONEXCLAMATION) || FLAG(lpmbp->dwStyle, MB_ICONWARNING))
            pszIcon = TD_WARNING_ICON;
        
        if (FLAG(lpmbp->dwStyle, MB_DEFBUTTON1)) button = 0;
        else if (FLAG(lpmbp->dwStyle, MB_DEFBUTTON2)) button = 1;
        else if (FLAG(lpmbp->dwStyle, MB_DEFBUTTON3)) button = 2;
        else if (FLAG(lpmbp->dwStyle, MB_DEFBUTTON4)) button = 3;

        int nButtonId = 0;
        BOOL help = FLAG(lpmbp->dwStyle, MB_HELP);

        TASKDIALOG_COMMON_BUTTON_FLAGS buttons = NULL;
        
        switch (lpmbp->dwStyle & MB_TYPEMASK) {
        case MB_ABORTRETRYIGNORE:
            buttons = TDCBF_ABORT | TDCBF_RETRY_BUTTON | TDCBF_IGNORE;

            switch (button) {
                case 0:
                    nButtonId = IDABORT;
                    break;
                case 1:
                    nButtonId = IDRETRY;
                    break;
                case 2:
                    nButtonId = IDIGNORE;
                    break;
                case 3:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        case MB_CANCELTRYCONTINUE:
            buttons = TDCBF_CANCEL_BUTTON | TDCBF_RETRY_BUTTON | TDCBF_CONTINUE;
            switch (button) {
                case 0:
                    nButtonId = IDCANCEL;
                    break;
                case 1:
                    nButtonId = IDRETRY;
                    break;
                case 2:
                    nButtonId = IDCONTINUE;
                    break;
                case 3:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        case MB_YESNOCANCEL:
            buttons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON | TDCBF_CANCEL_BUTTON;
            switch (button) {
                case 0:
                    nButtonId = IDYES;
                    break;
                case 1:
                    nButtonId = IDNO;
                    break;
                case 2:
                    nButtonId = IDCANCEL;
                    break;
                case 3:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        case MB_OKCANCEL:
            buttons = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
            switch (button) {
                case 0:
                    nButtonId = IDOK;
                    break;
                case 1:
                    nButtonId = IDCANCEL;
                    break;
                case 2:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        case MB_RETRYCANCEL:
            buttons = TDCBF_RETRY_BUTTON | TDCBF_CANCEL_BUTTON;
            switch (button) {
                case 0:
                    nButtonId = IDRETRY;
                    break;
                case 1:
                    nButtonId = IDCANCEL;
                    break;
                case 2:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        case MB_YESNO:
            buttons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
            switch (button) {
                case 0:
                    nButtonId = IDYES;
                    break;
                case 1:
                    nButtonId = IDNO;
                    break;
                case 2:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        default: // MB_OK
            buttons = TDCBF_OK_BUTTON;
            switch (button) {
                case 0:
                    nButtonId = IDOK;
                    break;
                case 1:
                    if (help)
                        nButtonId = IDHELP;
                    break;
            }
            break;
        }

        if (help)
            buttons |= TDCBF_HELP;

        PCWSTR caption = lpmbp->lpszCaption != NULL ? lpmbp->lpszCaption : L"Error";

        HMODULE hComCtl = LoadLibraryW(L"comctl32.dll");
        if (hComCtl) {
            // TaskDialog_t pTaskDialog = (TaskDialog_t)GetProcAddress(hComCtl, "TaskDialog");

            // if (!pTaskDialog)
            //     return MessageBoxW_Original(hWnd, lpText, lpCaption, uType);

            // HRESULT hr = pTaskDialog(
            //     hWnd,
            //     NULL,
            //     caption,
            //     caption,
            //     lpText,
            //     buttons,
            //     pszIcon,
            //     &button
            // );

            // if (SUCCEEDED(hr))
            // {
            //     return button;
            // }

            TaskDialogIndirect_t pTaskDialogIndirect = (TaskDialogIndirect_t)GetProcAddress(hComCtl, "TaskDialogIndirect");

            if (!pTaskDialogIndirect)
                return MessageBoxIndirectW_Original(lpmbp);

            TASKDIALOGCONFIG tdc = {};
            tdc.cbSize = sizeof(tdc);
            tdc.dwCommonButtons = buttons;
            tdc.pszMainIcon = pszIcon;
            tdc.hwndParent = lpmbp->hwndOwner;
            tdc.pszContent = lpmbp->lpszText;
            tdc.pszWindowTitle = caption;
            tdc.pszMainInstruction = caption;
            tdc.dwFlags = TDF_SIZE_TO_CONTENT;
            tdc.pfCallback = MessageBoxW_TaskDialogCallback;
            tdc.nDefaultButton = nButtonId;

            MSGBOXTASKDLGHELPINFO hi = {};
            hi.callback        = lpmbp->lpfnMsgBoxCallback;
            hi.dwContextHelpId = lpmbp->dwContextHelpId;

            tdc.lpCallbackData = (LONG_PTR) ((LPMSGBOXTASKDLGHELPINFO) &hi);
            
            int nButton = 0;
            HRESULT hr = pTaskDialogIndirect(&tdc, &nButton, NULL, NULL);

            if (SUCCEEDED(hr))
            {
                return nButton;
            }
        }

        return MessageBoxIndirectW_Original(lpmbp);
    }
    else return MessageBoxIndirectW_Original(lpmbp);
}

class FolderBrowserModernEvents : public IFileDialogEvents {
    private:
        HRESULT GetHWnd(IFileDialog* pfd, HWND* phwnd) {
            IOleWindow *pWindow;
            HRESULT hr = pfd->QueryInterface(IID_PPV_ARGS(&pWindow));
            
            if (SUCCEEDED(hr))
            {
                hr = pWindow->GetWindow(phwnd);
                pWindow->Release();

                return hr;
            }

            return hr;
        }
        
        BOOL isFirstChange = TRUE;

    public:
        virtual ~FolderBrowserModernEvents() = default;

        BFFCALLBACK callback;
        LPARAM lpData;
        LONG _refCount = 1;

        STDMETHODIMP OnFileOk(IFileDialog *pfd)
        {
            return E_NOTIMPL;
        }

        STDMETHODIMP OnFolderChange(IFileDialog *pfd)
        {
            if (isFirstChange) {
                isFirstChange = FALSE;
                
                if (callback) {
                    HWND hwnd;
                    HRESULT hr = GetHWnd(pfd, &hwnd);

                    if (SUCCEEDED(hr)) {
                        callback(hwnd, BFFM_INITIALIZED, NULL, lpData);
                        callback(hwnd, BFFM_IUNKNOWN, (LPARAM)pfd, lpData);

                        pfd->AddRef();
                        SetWindowSubclass(hwnd, SubclassProc, 101, (DWORD_PTR) pfd);
                    }
                }
            }

            return S_OK;
        }

        STDMETHODIMP OnFolderChanging(IFileDialog *pfd, IShellItem* psiFolder)
        {
            return E_NOTIMPL;
        }

        STDMETHODIMP OnOverwrite(IFileDialog *pfd, IShellItem *psi, FDE_OVERWRITE_RESPONSE *pResponse)
        {
            return E_NOTIMPL;
        }

        STDMETHODIMP OnSelectionChange(IFileDialog* pfd) {
            if (callback) {
                IShellItem* psi;
                HRESULT hr = pfd->GetCurrentSelection(&psi);

                if (SUCCEEDED(hr)) {
                    LPITEMIDLIST pidl;
                    hr = SHGetIDListFromObject(psi, &pidl);

                    if (SUCCEEDED(hr)) {
                        HWND hwnd;
                        hr = GetHWnd(pfd, &hwnd); // helper function

                        if (SUCCEEDED(hr)) {
                            callback(hwnd, BFFM_SELCHANGED, (LPARAM) pidl, lpData);
                        }

                        ILFree(pidl);
                    }

                    psi->Release();
                }
            }

            return S_OK;
        }

        STDMETHODIMP OnShareViolation(IFileDialog *pfd, IShellItem *psi, FDE_SHAREVIOLATION_RESPONSE *pResponse)
        {
            return E_NOTIMPL;
        }
        
        STDMETHODIMP OnTypeChange(IFileDialog *pfd)
        {
            return E_NOTIMPL;
        }

        IFACEMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&_refCount); }
        IFACEMETHODIMP_(ULONG) Release()
        {
            ULONG c = InterlockedDecrement(&_refCount);
            if (c == 0) delete this;
            return c;
        }
        IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv)
        {
            if (!ppv) return E_POINTER;
            if (riid == __uuidof(IUnknown) || riid == __uuidof(IFileDialogEvents)) {
                *ppv = static_cast<IFileDialogEvents*>(this);
                AddRef();
                return S_OK;
            }
            *ppv = nullptr;
            return E_NOINTERFACE;
        }

        static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uidSubclass, DWORD_PTR dwRefData) {
            IFileDialog* pfd = (IFileDialog*)dwRefData;

            switch (uMsg)
            {
                case BFFM_SETOKTEXT:
                {
                    Wh_Log(L"SET OK RECEIVED");
                    pfd->SetOkButtonLabel((LPCWSTR) lParam);

                    // IFileDialogCustomize* pCustomize;

                    // HRESULT hr = pfd->QueryInterface(IID_PPV_ARGS(&pCustomize));

                    // if (SUCCEEDED(hr)) {
                    //     pCustomize->SetControlLabel(1002, (LPCWSTR) lParam);
                    //     pCustomize->Release();
                    // }

                    break;
                }
                case WM_NCDESTROY:
                    RemoveWindowSubclass(hwnd, SubclassProc, uidSubclass);
                    pfd->Release(); // balance the AddRef
                    break;
            }

            return DefSubclassProc(hwnd, uMsg, wParam, lParam);
        }
};

LPITEMIDLIST WINAPI SHBrowseForFolderW_Hook(LPBROWSEINFOW lpbi) {
    if (settings.modernFolderBrowser && !FLAG(lpbi->ulFlags, BIF_BROWSEFORCOMPUTER) && !FLAG(lpbi->ulFlags, BIF_BROWSEFORPRINTER)) {
        HRESULT hr = CoInitialize(0);

        if (SUCCEEDED(hr)) {
            LPITEMIDLIST result = NULL;

            IFileOpenDialog* pfd;
            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

            if (SUCCEEDED(hr)) {
                FolderBrowserModernEvents* events = new FolderBrowserModernEvents();
                events->callback = lpbi->lpfn;
                events->lpData = lpbi->lParam;

                DWORD dwEventCookie = 0;
                hr = pfd->Advise(events, &dwEventCookie);

                FILEOPENDIALOGOPTIONS fos = FOS_PICKFOLDERS;

                if (FLAG(lpbi->ulFlags, BIF_RETURNONLYFSDIRS))
                    fos |= FOS_FORCEFILESYSTEM;

                FILEOPENDIALOGOPTIONS existingOptions = 0;
                hr = pfd->GetOptions(&existingOptions);

                if (SUCCEEDED(hr))
                    hr = pfd->SetOptions(existingOptions | fos);
            

                IFileDialogCustomize* pCustomize;

                hr = pfd->QueryInterface(IID_PPV_ARGS(&pCustomize));

                if (SUCCEEDED(hr)) {
                    if (lpbi->lpszTitle && settings.modernFbShowLpszTitle) 
                        pCustomize->AddText(1001, lpbi->lpszTitle);

                    // hr = pCustomize->EnableOpenDropDown(0);
                    // if (SUCCEEDED(hr)) {
                    //     hr = pCustomize->AddControlItem(0, 1002, L"Select folder");
                    // }

                    pCustomize->Release();  
                }

                hr = pfd->Show(lpbi->hwndOwner);

                if (SUCCEEDED(hr)) {
                    IShellItem* psi;
                    hr = pfd->GetResult(&psi);

                    if (SUCCEEDED(hr)) {
                        if (lpbi->pszDisplayName) {
                            LPWSTR pszDisplayName;

                            hr = psi->GetDisplayName(SIGDN_NORMALDISPLAY, &pszDisplayName);

                            if (SUCCEEDED(hr)) {
                                wcscpy_s(lpbi->pszDisplayName, MAX_PATH, pszDisplayName);

                                CoTaskMemFree(pszDisplayName);
                            }
                        }

                        PIDLIST_ABSOLUTE pidl = nullptr;
                        hr = SHGetIDListFromObject(psi, &pidl);

                        if (SUCCEEDED(hr))
                        {
                            // caller is responsible for CoTaskMemFree
                            result = pidl;
                        }

                        psi->Release();
                    }

                } else result = NULL;

                pfd->Unadvise(dwEventCookie);
                delete events;
                pfd->Release();
            }

            CoUninitialize();

            return result;
        }
    }

    return SHBrowseForFolderW_Original(lpbi);
}

// #define OLD_CC_PROP L"BETTER-DIALOGS_OldCC"

// #define DMOVE_Helper(v) v == -1 ? v : 0

// #define DMOVE(hw, i, x, y, w, h) { HWND hItem = GetDlgItem(hw, i); if (hItem) SetWindowPos(hItem, NULL, DMOVE_Helper(x), DMOVE_Helper(y), DMOVE_Helper(w),\
//  DMOVE_Helper(h), ((x == -1 && y == -1) ? SWP_NOMOVE : 0) | ((w == -1 && h == -1) ? SWP_NOSIZE : 0)); }

// #define IDC_PICKSCREENCOLOR 8001
// #define IDC_STATICHEX 8002
// #define IDC_EDITHEX 8003

// UINT_PTR CALLBACK BetterColorDialogHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//     Wh_Log(L"HOOK");
//     switch (uMsg) {
//         case WM_INITDIALOG:
//         {
//             Wh_Log(L"INIT DIALOG");

//             LPCHOOSECOLORW lpcc = (LPCHOOSECOLORW)lParam;
//             LPCHOOSECOLORW lpoldcc = (LPCHOOSECOLORW)lpcc->lCustData;

//             SetPropW(hwnd, OLD_CC_PROP, lpoldcc);

//             RECT rcWindow;
//             if (GetWindowRect(hwnd, &rcWindow)) {
//                 // TODO: Adjust this height properly once the dialog actually shows up
//                 SetWindowPos(hwnd, NULL, 0, 0, rcWindow.right, 400, SWP_NOMOVE);
//             }

//             CreateWindowExW(0, WC_BUTTONW, L"Pick Screen Color", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 7, 109, 131, 14, hwnd, (HMENU)IDC_PICKSCREENCOLOR,
//             GetModuleHandleW(nullptr), NULL);

//             DMOVE(hwnd, 65535, 4, 133, -1, -1);
//             DMOVE(hwnd, COLOR_CUSTOM1, 4, 143, 140, 28);
//             DMOVE(hwnd, COLOR_CURRENT, -1, -1, 40, 56);
//             DMOVE(hwnd, COLOR_SOLID_LEFT, 152, 184, -1, -1);
//             DMOVE(hwnd, COLOR_SOLID_RIGHT, 172, 184, -1, -1);
//             DMOVE(hwnd, COLOR_MIX, 4, 177, -1, -1);
//             DMOVE(hwnd, IDOK, 4, 193, -1, -1);
//             DMOVE(hwnd, IDCANCEL, 52, 193, -1, -1);
//             DMOVE(hwnd, pshHelp, 100, 193, -1, -1);
//             DMOVE(hwnd, COLOR_ADD, 152, 193, -1, -1);

//             CreateWindowExW(0, WC_STATICW, L"HEX:", SS_LEFT | WS_CHILD | WS_VISIBLE, 197, 172, 16, 8,
//             hwnd, (HMENU) IDC_STATICHEX, GetModuleHandleW(nullptr), NULL);

//             CreateWindowExW(0, WC_EDITW, L"FFFFFF", ES_LEFT | ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE,
//             218, 170, 70, 12, hwnd, (HMENU) IDC_EDITHEX, GetModuleHandleW(nullptr), NULL);

//             if (LPCHOOSECOLORW lpoldcc = (LPCHOOSECOLORW) GetPropW(hwnd, OLD_CC_PROP)) {
//                 if (FLAG(lpoldcc->Flags, CC_ENABLEHOOK)) {
//                     return lpoldcc->lpfnHook(hwnd, uMsg, wParam, lParam);
//                 }
//             }

//             return TRUE;
//         }
//         // TODO: Actually process the control messages
//     }

//     if (LPCHOOSECOLORW lpoldcc = (LPCHOOSECOLORW) GetPropW(hwnd, OLD_CC_PROP)) {
//         if (FLAG(lpoldcc->Flags, CC_ENABLEHOOK)) {
//             return lpoldcc->lpfnHook(hwnd, uMsg, wParam, lParam);
//         }
//     }

//     return FALSE;
// }

// BOOL WINAPI ChooseColorW_Hook(LPCHOOSECOLORW lpcc) {
//     if (FLAG(lpcc->Flags, CC_ENABLETEMPLATE) || !settings.betterColorPicker) return ChooseColorW_Original(lpcc);

//     CHOOSECOLORW cc = {};
//     memcpy(&cc, lpcc, lpcc->lStructSize);

//     cc.lStructSize = sizeof(cc);

//     if (!FLAG(lpcc->Flags, CC_ENABLEHOOK))
//         cc.Flags = lpcc->Flags | CC_ENABLEHOOK;
//     else
//         cc.Flags = lpcc->Flags;

//     cc.lpfnHook = BetterColorDialogHookProc;
//     cc.lCustData = (LPARAM) lpcc;

//     return ChooseColorW_Original(&cc);
// }

// #define WC_MODERNICONPICKERDLGW L"BETTER-DIALOGS_ModernPickIconDlg"

// int WINAPI PickIconDlg_Hook(HWND hwnd, PWSTR pszIconPath, UINT cchIconPath, int* piIconIndex)
// {
//     if (settings.betterPickIconDlg) {
//         HWND hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME, )
//     }

//     return PickIconDlg_Original(hwnd, pszIconPath, cchIconPath, piIconIndex);
// }

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    // register the class
    // WNDCLASSW wc = {};
    // wc.lpszClassName = WC_MODERNICONPICKERDLGW;
    // wc.hInstance = GetModuleHandleW(nullptr);

    HMODULE hUser32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    MessageBoxW_t pMessageBoxW =
        (MessageBoxW_t)GetProcAddress(hUser32,
                                                "MessageBoxW");

    MessageBoxIndirectW_t pMessageBoxIndirectW = (MessageBoxIndirectW_t)
        GetProcAddress(hUser32, "MessageBoxIndirectW");

    Wh_SetFunctionHook((void*)pMessageBoxW,
                       (void*)MessageBoxW_Hook,
                       (void**)&MessageBoxW_Original);

    Wh_SetFunctionHook((void*)pMessageBoxIndirectW,
                        (void*)MessageBoxIndirectW_Hook,
                        (void**)&MessageBoxIndirectW_Original);

    HMODULE hShell32 = LoadLibraryExW(L"Shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    SHBrowseForFolderW_t pSHBrowseForFolderW = (SHBrowseForFolderW_t)GetProcAddress(hShell32, "SHBrowseForFolderW");

    Wh_SetFunctionHook((void*)pSHBrowseForFolderW,
     (void*)SHBrowseForFolderW_Hook,
     (void**)&SHBrowseForFolderW_Original);

    // PickIconDlg_t pPickIconDlg = (PickIconDlg_t)GetProcAddress(hShell32, "PickIconDlg");

    // Wh_SetFunctionHook((void*)pPickIconDlg,
    //  (void*)PickIconDlg_Hook,
    //  (void**)&PickIconDlg_Original);

    // HMODULE hComDlg = LoadLibraryExW(L"Comdlg32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    // ChooseColorW_t pChooseColorW = (ChooseColorW_t)GetProcAddress(hComDlg, "ChooseColorW");

    // Wh_SetFunctionHook((void*)pChooseColorW, (void*)ChooseColorW_Hook, (void**)&ChooseColorW_Original);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
