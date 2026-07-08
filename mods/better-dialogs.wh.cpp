// ==WindhawkMod==
// @id              better-dialogs
// @name            Better Dialogs
// @description     Improves Windows dialog boxes.
// @version         1.1
// @author          FireBlade
// @github          https://github.com/FireBlade211
// @include         *
// @compilerOptions -lcomdlg32 -luser32 -lshell32 -lole32 -luuid -lcomctl32 -lgdi32 -ld2d1 -ldwrite
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Better Dialogs
This mod improves certain Windows dialogs. It also re-translates messages for those dialogs to make sure that apps that expect the original dialogs still work properly.

More dialogs coming soon!

## Dialogs changed
- Message boxes (W and A) changed to Task Dialogs
- Legacy folder picker replaced with modern directory selector
- Legacy Open/Save file dialogs (Windows XP-style) replaced with modern IFileDialog
  - Custom controls from legacy dialog templates are preserved via IFileDialogCustomize
  - OFN_READONLY checkbox is carried over to the modern dialog
- Legacy ChooseColor replaced with modern HSV color picker with spectrum, hue bar, RGB/Hex inputs, and custom color support
- Legacy ChooseFont replaced with modern WinUI-style font picker with DirectWrite preview

## Screenshots
![Improved Color Dialog](https://raw.githubusercontent.com/FireBlade211/FireBlade211/refs/heads/main/WindhawkModReadmeImages/BetterDialogs/colordlg.png)

![Improved Font Dialog](https://raw.githubusercontent.com/FireBlade211/FireBlade211/refs/heads/main/WindhawkModReadmeImages/BetterDialogs/fontdlg.png)

![A Message Box that was converted to a Task Dialog](https://raw.githubusercontent.com/FireBlade211/FireBlade211/refs/heads/main/WindhawkModReadmeImages/BetterDialogs/taskmsgbox.png)
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
  $name: Show dialog instructions in modern folder picker body
  $description: Show the application-provided instructions as a label inside the modern folder picker body. The instructions are always shown in the title bar regardless of this setting.
- modernFileDialog: true
  $name: Use modern file Open/Save dialog
  $description: Replace legacy (Windows XP-style) Open/Save file dialogs with the modern IFileDialog version.
- betterColorPicker: true
  $name: Use modern Color Picker
  $description: Replace the legacy ChooseColor dialog with a modern HSV color picker.
- modernFontPicker: true
  $name: Use modern Font Picker
  $description: Replace the legacy ChooseFont dialog with a modern WinUI-style font picker.
- localized: false
  $name: Use localized labels in Color and Font dialogs
  $description: Pull localized labels for the Color and Font dialogs. This may affect performance, because the built-in Windows dialog has to be created before the strings are captured.
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
#include <math.h>
#include <stdio.h>
#include <d2d1.h>
#include <dwrite.h>
#include <vector>
#include <mutex>
#include <atomic>

struct {
	bool messageTaskDlg;
	bool modernFolderBrowser;
	bool modernFbShowLpszTitle;
	bool modernFileDialog;
	bool betterColorPicker;
	bool modernFontPicker;
	bool localized;
	//bool betterPickIconDlg;
} settings;

using MessageBoxW_t = decltype(&MessageBoxW);
using MessageBoxIndirectW_t = decltype(&MessageBoxIndirectW);
using SHBrowseForFolderW_t = decltype(&SHBrowseForFolderW);
using ChooseColorW_t = decltype(&ChooseColorW);
using GetOpenFileNameW_t = decltype(&GetOpenFileNameW);
using GetSaveFileNameW_t = decltype(&GetSaveFileNameW);
using GetOpenFileNameA_t = decltype(&GetOpenFileNameA);
using GetSaveFileNameA_t = decltype(&GetSaveFileNameA);
using MessageBoxA_t = decltype(&MessageBoxA);
using MessageBoxIndirectA_t = decltype(&MessageBoxIndirectA);
using ChooseFontW_t = decltype(&ChooseFontW);
//using PickIconDlg_t = decltype(&PickIconDlg);

MessageBoxW_t MessageBoxW_Original;
MessageBoxIndirectW_t MessageBoxIndirectW_Original;
SHBrowseForFolderW_t SHBrowseForFolderW_Original;
ChooseColorW_t ChooseColorW_Original;
GetOpenFileNameW_t GetOpenFileNameW_Original;
GetSaveFileNameW_t GetSaveFileNameW_Original;
GetOpenFileNameA_t GetOpenFileNameA_Original;
GetSaveFileNameA_t GetSaveFileNameA_Original;
MessageBoxA_t MessageBoxA_Original;
MessageBoxIndirectA_t MessageBoxIndirectA_Original;
ChooseFontW_t ChooseFontW_Original;
//PickIconDlg_t PickIconDlg_Original;

// COM pattern: we reference count (even though this is technically instance-counting)
std::atomic<int> nFontDlgCount{ 0 };
std::atomic<int> nColorDlgCount{ 0 };

std::vector<HWND> vDlgs = {};
std::mutex vDlgsMutex;

void LoadSettings() {
	settings.messageTaskDlg = Wh_GetIntSetting(L"messageTaskDlg");
	settings.modernFolderBrowser = Wh_GetIntSetting(L"modernFolderBrowser");
	settings.modernFbShowLpszTitle = Wh_GetIntSetting(L"modernFbShowLpszTitle");
	settings.modernFileDialog = Wh_GetIntSetting(L"modernFileDialog");
	settings.betterColorPicker = Wh_GetIntSetting(L"betterColorPicker");
	settings.modernFontPicker = Wh_GetIntSetting(L"modernFontPicker");
	settings.localized = Wh_GetIntSetting(L"localized");
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

typedef HRESULT(WINAPI* TaskDialog_t)(
	HWND,
	HINSTANCE,
	PCWSTR,
	PCWSTR,
	PCWSTR,
	TASKDIALOG_COMMON_BUTTON_FLAGS,
	PCWSTR,
	int*
	);

typedef HRESULT(WINAPI* TaskDialogIndirect_t)(
	const TASKDIALOGCONFIG* pTaskConfig,
	int* pnButton,
	int* pnRadioButton,
	BOOL* pfVerificationFlagChecked
	);

HRESULT CALLBACK MessageBoxW_TaskDialogCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData);

typedef struct tagMSGBOXTASKDLGHELPINFO {
	MSGBOXCALLBACK callback;
	DWORD_PTR dwContextHelpId;
} MSGBOXTASKDLGHELPINFO, * LPMSGBOXTASKDLGHELPINFO;

int WINAPI MessageBoxW_Hook(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
	if (settings.messageTaskDlg) {
		int button = 0;

		// unsupported
		// if ((uType & MB_ICONQUESTION) != 0) {
		//     HRESULT hr = TaskDialog(hWnd, )
		// }

		PCWSTR pszIcon = NULL;
		UINT icon = uType & MB_ICONMASK;

		if (icon == MB_ICONERROR || icon == MB_ICONSTOP || icon == MB_ICONHAND)
			pszIcon = TD_ERROR_ICON;

		else if (icon == MB_ICONINFORMATION || icon == MB_ICONASTERISK)
			pszIcon = TD_INFORMATION_ICON;

		else if (icon == MB_ICONQUESTION)
			pszIcon = NULL;

		else if (icon == MB_ICONEXCLAMATION || icon == MB_ICONWARNING)
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

		HMODULE hComCtl = GetModuleHandleW(L"comctl32.dll");
		if (!hComCtl) hComCtl = LoadLibraryExW(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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
				LPMSGBOXTASKDLGHELPINFO pinfo = (LPMSGBOXTASKDLGHELPINFO)lpRefData;

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

int WINAPI MessageBoxIndirectW_Hook(const MSGBOXPARAMSW* lpmbp) {
	if (settings.messageTaskDlg) {
		int button = 0;

		// unsupported
		// if ((uType & MB_ICONQUESTION) != 0) {
		//     HRESULT hr = TaskDialog(hWnd, )
		// }

		PCWSTR pszIcon = NULL;
		UINT icon = lpmbp->dwStyle & MB_ICONMASK;

		if (icon == MB_ICONERROR || icon == MB_ICONSTOP || icon == MB_ICONHAND)
			pszIcon = TD_ERROR_ICON;

		else if (icon == MB_ICONINFORMATION || icon == MB_ICONASTERISK)
			pszIcon = TD_INFORMATION_ICON;

		else if (icon == MB_ICONQUESTION)
			pszIcon = NULL;

		else if (icon == MB_ICONEXCLAMATION || icon == MB_ICONWARNING)
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

		HMODULE hComCtl = GetModuleHandleW(L"comctl32.dll");
		if (!hComCtl) hComCtl = LoadLibraryExW(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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
			hi.callback = lpmbp->lpfnMsgBoxCallback;
			hi.dwContextHelpId = lpmbp->dwContextHelpId;

			tdc.lpCallbackData = (LONG_PTR)((LPMSGBOXTASKDLGHELPINFO)&hi);

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
		IOleWindow* pWindow;
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

	STDMETHODIMP OnFileOk(IFileDialog* pfd)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP OnFolderChange(IFileDialog* pfd)
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
					SetWindowSubclass(hwnd, SubclassProc, 101, (DWORD_PTR)pfd);
				}
			}
		}

		return S_OK;
	}

	STDMETHODIMP OnFolderChanging(IFileDialog* pfd, IShellItem* psiFolder)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP OnOverwrite(IFileDialog* pfd, IShellItem* psi, FDE_OVERWRITE_RESPONSE* pResponse)
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
						callback(hwnd, BFFM_SELCHANGED, (LPARAM)pidl, lpData);
					}

					ILFree(pidl);
				}

				psi->Release();
			}
		}

		return S_OK;
	}

	STDMETHODIMP OnShareViolation(IFileDialog* pfd, IShellItem* psi, FDE_SHAREVIOLATION_RESPONSE* pResponse)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP OnTypeChange(IFileDialog* pfd)
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
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
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
			pfd->SetOkButtonLabel((LPCWSTR)lParam);

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
		HRESULT hrCo = CoInitialize(0);

		if (SUCCEEDED(hrCo)) {
			BOOL weInitCom = (hrCo == S_OK);
			LPITEMIDLIST result = NULL;
			HRESULT hr;

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
					if (lpbi->lpszTitle) {
						// Always set the window title from the app-provided instructions (safe)
						pfd->SetTitle(lpbi->lpszTitle);

						// Optionally also show as body label for apps that rely on inline instructions
						if (settings.modernFbShowLpszTitle)
							pCustomize->AddText(1001, lpbi->lpszTitle);
					}

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

				}
				else result = NULL;

				pfd->Unadvise(dwEventCookie);
				delete events;
				pfd->Release();
			}

			if (weInitCom) CoUninitialize();

			return result;
		}
	}

	return SHBrowseForFolderW_Original(lpbi);
}

// --- Modern Color Picker (Paint-style, D2D) ---

// Layout
#define CP_PAD       20
#define CP_SPEC_W    320
#define CP_SPEC_H    200
#define CP_VBAR_W    14
#define CP_VBAR_GAP  14
#define CP_SPEC_X    CP_PAD
#define CP_SPEC_Y    CP_PAD
#define CP_VBAR_X    (CP_SPEC_X + CP_SPEC_W + CP_VBAR_GAP)
#define CP_VBAR_Y    CP_SPEC_Y
#define CP_INFO_X    (CP_VBAR_X + CP_VBAR_W + 20)
#define CP_CIRC_D    24
#define CP_CIRC_GAP  6
#define CP_CIRC_STEP (CP_CIRC_D + CP_CIRC_GAP)
#define CP_BCOLS     12
#define CP_BROWS     4
#define CP_ROUND     5.0f

#define IDC_CP_R    4001
#define IDC_CP_G    4002
#define IDC_CP_B    4003
#define IDC_CP_HEX  4004

static const COLORREF g_basicColors[48] = {
	RGB(235,150,170), RGB(230,100,120), RGB(200,80,80), RGB(180,50,50), RGB(140,30,30), RGB(100,20,20),
	RGB(180,130,100), RGB(200,160,110), RGB(140,200,220), RGB(100,180,220), RGB(60,140,200), RGB(30,90,160),
	RGB(240,200,80), RGB(240,180,50), RGB(240,150,30), RGB(220,120,20), RGB(200,80,10), RGB(180,50,10),
	RGB(230,100,180), RGB(200,60,160), RGB(160,40,180), RGB(120,30,160), RGB(80,40,140), RGB(60,60,180),
	RGB(130,220,100), RGB(80,200,60), RGB(40,170,40), RGB(20,140,30), RGB(10,100,20), RGB(30,80,10),
	RGB(160,200,40), RGB(200,220,60), RGB(240,100,140), RGB(220,50,120), RGB(180,20,140), RGB(140,0,120),
	RGB(80,220,200), RGB(40,190,180), RGB(20,150,140), RGB(10,100,100), RGB(0,0,0), RGB(64,64,64),
	RGB(128,128,128), RGB(160,160,160), RGB(192,192,192), RGB(210,210,210), RGB(235,235,235), RGB(255,255,255),
};

static int CP_GridY() { return CP_SPEC_Y + CP_SPEC_H + 48; }
static int CP_CustGridY() { return CP_GridY() + CP_BROWS * CP_CIRC_STEP + 38; }
static int CP_BtnY() { return CP_CustGridY() + 2 * CP_CIRC_STEP + 28; }
static int CP_ClientW() { return CP_INFO_X + 160; }
static int CP_ClientH() { return CP_BtnY() + 40 + CP_PAD; }

struct ColorPickerData {
	COLORREF result, original;
	COLORREF* lpCustColors;
	float hue, sat, val;
	DWORD* specPixels;
	DWORD* vbarPixels;
	BOOL accepted, updating;
	BOOL trackingSpec, trackingVBar;
	int selectedCustom;
	ID2D1Factory* pFactory;
	ID2D1DCRenderTarget* pRT;
	ID2D1Bitmap* pSpecBmp;
	ID2D1Bitmap* pVBarBmp;
	ID2D1StrokeStyle* pDashed;
	IDWriteFactory* pDWFactory;
	IDWriteTextFormat* pTextFmt;
    DWORD flags;
	LPCCHOOKPROC pccHook;
};

// Localized strings (captured from real Windows ChooseColor dialog)
static WCHAR g_cpTitle[64] = L"Color";
static WCHAR g_cpBasic[64] = L"Basic colors";
static WCHAR g_cpCustom[64] = L"Custom colors";
static WCHAR g_cpCancel[32] = L"Cancel";
static BOOL g_cpStringsLoaded = FALSE;
static int g_cpStaticIdx = 0;

static BOOL CALLBACK CP_EnumStrings(HWND hChild, LPARAM) {

	WCHAR cls[32];
	GetClassNameW(hChild, cls, 32);

	if (_wcsicmp(cls, L"Static") == 0) {
		WCHAR text[64] = {};
		GetWindowTextW(hChild, text, 64);

		if (text[0] && wcslen(text) > 3) {

			// Strip & accelerator
			WCHAR clean[64];
			WCHAR* s = text;
			WCHAR* d = clean;

			while (*s)
			{
				if (*s != L'&')
					*d++ = *s;
				s++;
			}

			*d = 0;

			// Remove trailing ':'
			size_t len = wcslen(clean);
			if (len > 0 && clean[len - 1] == L':')
				clean[len - 1] = 0;

			if (g_cpStaticIdx == 0) wcscpy(g_cpBasic, clean);
			else if (g_cpStaticIdx == 1) wcscpy(g_cpCustom, clean);
			g_cpStaticIdx++;
		}
	}
	return TRUE;
}

static UINT_PTR CALLBACK CP_CaptureHook(HWND hwnd, UINT uMsg, WPARAM, LPARAM) {
	if (uMsg == WM_INITDIALOG) {
		// Move offscreen so user never sees it
		SetWindowPos(hwnd, NULL, -32000, -32000, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		// Capture title
		GetWindowTextW(hwnd, g_cpTitle, 64);
		// Capture Cancel button text
		HWND hCancel = GetDlgItem(hwnd, IDCANCEL);
		if (hCancel) GetWindowTextW(hCancel, g_cpCancel, 32);
		// Capture static labels (Basic Colors, Custom Colors)
		g_cpStaticIdx = 0;
		EnumChildWindows(hwnd, CP_EnumStrings, 0);
		// Close immediately
		EndDialog(hwnd, IDCANCEL);
		return TRUE;
	}
	return FALSE;
}

static void CP_LoadStrings() {
	if (g_cpStringsLoaded) return;
	g_cpStringsLoaded = TRUE;

	if (settings.localized) {
		// Briefly open the ORIGINAL ChooseColor dialog offscreen to capture localized strings
		COLORREF cust[16] = {};
		CHOOSECOLORW cc = {};
		cc.lStructSize = sizeof(cc);
		cc.hwndOwner = NULL;
		cc.lpCustColors = cust;
		cc.Flags = CC_ENABLEHOOK;
		cc.lpfnHook = CP_CaptureHook;
		ChooseColorW_Original(&cc);
	}
	else {
		wcscpy_s(g_cpTitle, 64, L"Color");
		wcscpy_s(g_cpCancel, 32, L"Cancel");

		wcscpy_s(g_cpBasic, 64, L"Basic colors");
		wcscpy_s(g_cpCustom, 64, L"Custom colors");
	}
}

static COLORREF CP_HSVtoRGB(float h, float s, float v) {
	int hi = (int)(h / 60.0f) % 6;
	float f = h / 60.0f - floorf(h / 60.0f);
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	float r, g, b;

	switch (hi) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	default:
		r = v;
		g = p;
		b = q;
		break;
	}
	return RGB((BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255));
}

static void CP_RGBtoHSV(COLORREF c, float& h, float& s, float& v) {
	float r = GetRValue(c) / 255.f;
	float g = GetGValue(c) / 255.f;
	float b = GetBValue(c) / 255.f;

	float mx = r > g ? (r > b ? r : b) : (g > b ? g : b), mn = r < g ? (r < b ? r : b) : (g < b ? g : b), d = mx - mn;
	v = mx; s = mx > 0 ? d / mx : 0;
	if (d == 0) h = 0;
	else if (mx == r) { h = 60 * (g - b) / d; if (h < 0)h += 360; }
	else if (mx == g) h = 60 * (b - r) / d + 120;
	else h = 60 * (r - g) / d + 240;
}

static void CP_RenderSpectrum(ColorPickerData* d) {
	for (int y = 0; y < CP_SPEC_H; y++) {
		float s = 1.f - (float)y / (CP_SPEC_H - 1);
		for (int x = 0; x < CP_SPEC_W; x++) {
			float h = (float)x * 360.f / (CP_SPEC_W - 1);
			COLORREF c = CP_HSVtoRGB(h, s, d->val);
			d->specPixels[y * CP_SPEC_W + x] = GetBValue(c) | (GetGValue(c) << 8) | (GetRValue(c) << 16) | 0xFF000000;
		}
	}
}

static void CP_RenderVBar(ColorPickerData* d) {
	for (int y = 0; y < CP_SPEC_H; y++) {
		float v = 1.f - (float)y / (CP_SPEC_H - 1);
		COLORREF c = CP_HSVtoRGB(d->hue, d->sat, v);
		DWORD px = GetBValue(c) | (GetGValue(c) << 8) | (GetRValue(c) << 16) | 0xFF000000;
		for (int x = 0; x < CP_VBAR_W; x++) d->vbarPixels[y * CP_VBAR_W + x] = px;
	}
}

static void CP_UploadBitmaps(ColorPickerData* d) {
	if (d->pSpecBmp)
	{
		D2D1_RECT_U r = { 0,0,(UINT32)CP_SPEC_W,(UINT32)CP_SPEC_H };
		d->pSpecBmp->CopyFromMemory(&r, d->specPixels, CP_SPEC_W * 4);
	}

	if (d->pVBarBmp)
	{
		D2D1_RECT_U r = { 0,0,(UINT32)CP_VBAR_W,(UINT32)CP_SPEC_H };
		d->pVBarBmp->CopyFromMemory(&r, d->vbarPixels, CP_VBAR_W * 4);
	}
}

static D2D1_COLOR_F CP_D2D(COLORREF c, float a = 1.f)
{
	return D2D1::ColorF(GetRValue(c) / 255.f, GetGValue(c) / 255.f, GetBValue(c) / 255.f, a);
}

static void CP_UpdateControls(HWND hwnd, ColorPickerData* d) {
	d->result = CP_HSVtoRGB(d->hue, d->sat, d->val);
	d->updating = TRUE;
	WCHAR b[16];
	wsprintfW(b, L"%d", GetRValue(d->result)); SetDlgItemTextW(hwnd, IDC_CP_R, b);
	wsprintfW(b, L"%d", GetGValue(d->result)); SetDlgItemTextW(hwnd, IDC_CP_G, b);
	wsprintfW(b, L"%d", GetBValue(d->result)); SetDlgItemTextW(hwnd, IDC_CP_B, b);
	wsprintfW(b, L"#%02X%02X%02X", GetRValue(d->result), GetGValue(d->result), GetBValue(d->result));
	SetDlgItemTextW(hwnd, IDC_CP_HEX, b);
	d->updating = FALSE;
	CP_RenderSpectrum(d); CP_RenderVBar(d); CP_UploadBitmaps(d);
	InvalidateRect(hwnd, NULL, FALSE);
}

static void CP_CreateD2D(HWND hwnd, ColorPickerData* d) {
	if (d->pRT) return;
	if (!d->pFactory) D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d->pFactory);
	if (!d->pFactory) return;

	D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	d->pFactory->CreateDCRenderTarget(&rtp, &d->pRT);

	if (!d->pRT) return;

	D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	d->pRT->CreateBitmap(D2D1::SizeU(CP_SPEC_W, CP_SPEC_H), bp, &d->pSpecBmp);
	d->pRT->CreateBitmap(D2D1::SizeU(CP_VBAR_W, CP_SPEC_H), bp, &d->pVBarBmp);
	D2D1_STROKE_STYLE_PROPERTIES ssp = {}; ssp.dashStyle = D2D1_DASH_STYLE_DASH;
	d->pFactory->CreateStrokeStyle(ssp, nullptr, 0, &d->pDashed);

	// DirectWrite for WinUI-style text
	if (!d->pDWFactory) {
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&d->pDWFactory);

		if (d->pDWFactory) {
			d->pDWFactory->CreateTextFormat(L"Segoe UI Variable", nullptr,
				DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
				13.0f, L"", &d->pTextFmt);
		}
	}
	CP_UploadBitmaps(d);
}

static void CP_DestroyD2D(ColorPickerData* d) {
	if (d->pTextFmt)
	{
		d->pTextFmt->Release();
		d->pTextFmt = nullptr;
	}
	if (d->pDWFactory)
	{
		d->pDWFactory->Release();
		d->pDWFactory = nullptr;
	}
	if (d->pDashed)
	{
		d->pDashed->Release();
		d->pDashed = nullptr;
	}
	if (d->pSpecBmp)
	{
		d->pSpecBmp->Release();
		d->pSpecBmp = nullptr;
	}
	if (d->pVBarBmp)
	{ 
		d->pVBarBmp->Release();
		d->pVBarBmp = nullptr;
	}
	if (d->pRT)
	{ 
		d->pRT->Release();
		d->pRT = nullptr;
	}
	if (d->pFactory)
	{
		d->pFactory->Release();
		d->pFactory = nullptr;
	}
}

static void CP_Paint(HWND hwnd, HDC hdc, ColorPickerData* d) {
	CP_CreateD2D(hwnd, d);

	if (!d->pRT) return;

	RECT rc; GetClientRect(hwnd, &rc);
	d->pRT->BindDC(hdc, &rc);
	d->pRT->BeginDraw();

	// No Clear() — system background (Mica/theme) is preserved
	ID2D1SolidColorBrush* br = nullptr;

	// Spectrum (rounded clip)
	if (d->pSpecBmp) {

		D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(D2D1::RectF((float)CP_SPEC_X, (float)CP_SPEC_Y, (float)(CP_SPEC_X + CP_SPEC_W), (float)(CP_SPEC_Y + CP_SPEC_H)), CP_ROUND, CP_ROUND);
		ID2D1RoundedRectangleGeometry* clip = nullptr;
		d->pFactory->CreateRoundedRectangleGeometry(rr, &clip);

		if (clip) {
			d->pRT->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip), nullptr);
			d->pRT->DrawBitmap(d->pSpecBmp, D2D1::RectF((float)CP_SPEC_X, (float)CP_SPEC_Y, (float)(CP_SPEC_X + CP_SPEC_W), (float)(CP_SPEC_Y + CP_SPEC_H)));
			d->pRT->PopLayer(); clip->Release();
		}

		d->pRT->CreateSolidColorBrush(D2D1::ColorF(0.4f, 0.4f, 0.4f, 0.4f), &br);
		if (br) { d->pRT->DrawRoundedRectangle(rr, br); br->Release(); br = nullptr; }
	}
	// Crosshair
	{
		float cx = d->hue / 360.f * (CP_SPEC_W - 1) + CP_SPEC_X, cy = (1.f - d->sat) * (CP_SPEC_H - 1) + CP_SPEC_Y;
		D2D1_ELLIPSE ell = D2D1::Ellipse(D2D1::Point2F(cx, cy), 7.f, 7.f);

		d->pRT->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.5f), &br);
		if (br) { d->pRT->DrawEllipse(ell, br, 2.5f); br->Release(); br = nullptr; }

		d->pRT->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.9f), &br);
		if (br) { d->pRT->DrawEllipse(ell, br, 1.5f); br->Release(); br = nullptr; }
	}

	// Value bar (rounded clip)
	if (d->pVBarBmp) {
		D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(D2D1::RectF((float)CP_VBAR_X, (float)CP_VBAR_Y, (float)(CP_VBAR_X + CP_VBAR_W), (float)(CP_VBAR_Y + CP_SPEC_H)), CP_ROUND, CP_ROUND);
		
		ID2D1RoundedRectangleGeometry* clip = nullptr;
		d->pFactory->CreateRoundedRectangleGeometry(rr, &clip);

		if (clip) {
			d->pRT->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip), nullptr);
			d->pRT->DrawBitmap(d->pVBarBmp, D2D1::RectF((float)CP_VBAR_X, (float)CP_VBAR_Y, (float)(CP_VBAR_X + CP_VBAR_W), (float)(CP_VBAR_Y + CP_SPEC_H)));
			d->pRT->PopLayer(); clip->Release();
		}

		d->pRT->CreateSolidColorBrush(D2D1::ColorF(0.4f, 0.4f, 0.4f, 0.4f), &br);
		if (br) { d->pRT->DrawRoundedRectangle(rr, br); br->Release(); br = nullptr; }

		// Value indicator
		float vy = (1.f - d->val) * (CP_SPEC_H - 1) + CP_VBAR_Y;
		D2D1_ELLIPSE ve = D2D1::Ellipse(D2D1::Point2F((float)(CP_VBAR_X + CP_VBAR_W / 2.f), vy), 7.f, 7.f);

		d->pRT->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.5f), &br);
		if (br) { d->pRT->DrawEllipse(ve, br, 2.5f); br->Release(); br = nullptr; }

		d->pRT->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &br);
		if (br) { d->pRT->DrawEllipse(ve, br, 1.5f); d->pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F((float)(CP_VBAR_X + CP_VBAR_W / 2.f), vy), 4.f, 4.f), br); br->Release(); br = nullptr; }
	}

	// Basic color circles
	for (int i = 0; i < 48; i++) {
		int col = i % CP_BCOLS, row = i / CP_BCOLS;
		float cx = CP_SPEC_X + col * CP_CIRC_STEP + CP_CIRC_D / 2.f, cy = (float)CP_GridY() + row * CP_CIRC_STEP + CP_CIRC_D / 2.f;
		
		D2D1_ELLIPSE ell = D2D1::Ellipse(D2D1::Point2F(cx, cy), CP_CIRC_D / 2.f, CP_CIRC_D / 2.f);
		d->pRT->CreateSolidColorBrush(CP_D2D(g_basicColors[i]), &br);

		if (br) { d->pRT->FillEllipse(ell, br); br->Release(); br = nullptr; }
		d->pRT->CreateSolidColorBrush(D2D1::ColorF(0.3f, 0.3f, 0.3f, 0.2f), &br);

		if (br) { d->pRT->DrawEllipse(ell, br, 0.8f); br->Release(); br = nullptr; }
	}

	// Custom color circles (dashed if white/empty, filled otherwise)
	for (int i = 0; i < 16; i++) {
		int col = i % 8, row = i / 8;
		float cx = CP_SPEC_X + col * CP_CIRC_STEP + CP_CIRC_D / 2.f, cy = (float)CP_CustGridY() + row * CP_CIRC_STEP + CP_CIRC_D / 2.f;

		D2D1_ELLIPSE ell = D2D1::Ellipse(D2D1::Point2F(cx, cy), CP_CIRC_D / 2.f, CP_CIRC_D / 2.f);
		COLORREF cc = d->lpCustColors ? d->lpCustColors[i] : 0xFFFFFF;

		if (cc == 0xFFFFFF && d->pDashed) {
			d->pRT->CreateSolidColorBrush(D2D1::ColorF(0.5f, 0.5f, 0.5f, 0.35f), &br);
			if (br) { d->pRT->DrawEllipse(ell, br, 1.2f, d->pDashed); br->Release(); br = nullptr; }
		}
		else {
			d->pRT->CreateSolidColorBrush(CP_D2D(cc), &br);
			if (br) { d->pRT->FillEllipse(ell, br); br->Release(); br = nullptr; }

			float ba = (i == d->selectedCustom) ? 0.8f : 0.2f;
			d->pRT->CreateSolidColorBrush(D2D1::ColorF(0.3f, 0.3f, 0.3f, ba), &br);

			if (br) { d->pRT->DrawEllipse(ell, br, (i == d->selectedCustom) ? 2.f : 0.8f); br->Release(); br = nullptr; }
		}
	}

	// Section labels (DirectWrite for WinUI-style rendering)
	if (d->pTextFmt) {
		d->pRT->CreateSolidColorBrush(CP_D2D(GetSysColor(COLOR_WINDOWTEXT)), &br);

		if (br) {
			d->pRT->DrawTextW(g_cpBasic, (UINT32)wcslen(g_cpBasic), d->pTextFmt,
				D2D1::RectF((float)CP_SPEC_X, (float)(CP_GridY() - 24), (float)(CP_SPEC_X + 300), (float)CP_GridY()), br);

			d->pRT->DrawTextW(g_cpCustom, (UINT32)wcslen(g_cpCustom), d->pTextFmt,
				D2D1::RectF((float)CP_SPEC_X, (float)(CP_CustGridY() - 26), (float)(CP_SPEC_X + 300), (float)CP_CustGridY()), br);

			br->Release(); br = nullptr;
		}
	}

	d->pRT->EndDraw();
}

static LRESULT CALLBACK CP_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	ColorPickerData* d = (ColorPickerData*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch (msg) {
    case WM_NCCREATE:
    {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
		d = (ColorPickerData*)cs->lpCreateParams;

		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)d);
        break;
    }
	case WM_CREATE: {
		nColorDlgCount++;

		std::lock_guard<std::mutex> lock(vDlgsMutex);
		vDlgs.push_back(hwnd);

		HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

		auto mk = [&](LPCWSTR cls, LPCWSTR text, DWORD style, DWORD exs, int x, int y, int w, int h, int id = 0) {
			HWND hc = CreateWindowExW(exs, cls, text, WS_CHILD | WS_VISIBLE | style, x, y, w, h, hwnd, (HMENU)(INT_PTR)id, NULL, NULL);
			SendMessageW(hc, WM_SETFONT, (WPARAM)hFont, 0); return hc; };

		int ix = CP_INFO_X;
		mk(L"EDIT", L"", ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, ix, CP_SPEC_Y, 100, 24, IDC_CP_HEX);

		int ry = CP_SPEC_Y + 44;

		mk(L"EDIT", L"", ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, ix, ry, 80, 24, IDC_CP_R);
		mk(L"STATIC", L"R", SS_LEFT, 0, ix + 86, ry + 4, 16, 16);
		mk(L"EDIT", L"", ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, ix, ry + 34, 80, 24, IDC_CP_G);
		mk(L"STATIC", L"G", SS_LEFT, 0, ix + 86, ry + 38, 16, 16);
		mk(L"EDIT", L"", ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, ix, ry + 68, 80, 24, IDC_CP_B);
		mk(L"STATIC", L"B", SS_LEFT, 0, ix + 86, ry + 72, 16, 16);

		// Section labels drawn by D2D/DirectWrite in CP_Paint
		// Measure custom label text for + button placement
		HDC hTmpDC = GetDC(hwnd); HFONT hOldF = (HFONT)SelectObject(hTmpDC, hFont);
		SIZE szCust;

		GetTextExtentPoint32W(hTmpDC, g_cpCustom, (int)wcslen(g_cpCustom), &szCust);

		SelectObject(hTmpDC, hOldF); ReleaseDC(hwnd, hTmpDC);
		mk(L"BUTTON", L"+", BS_PUSHBUTTON, 0, CP_SPEC_X + 212, CP_CustGridY() - 28, 24, 22, 4010);

		int btnX = CP_PAD + ix + 48, btnY = CP_BtnY() - 8;//, halfW = (CP_ClientW()-CP_PAD*2-10)/2;

		mk(L"BUTTON", L"OK", BS_DEFPUSHBUTTON, 0, btnX, btnY, 80, 24, IDOK);
		mk(L"BUTTON", g_cpCancel, BS_PUSHBUTTON, 0, btnX, btnY + 32, 80, 24, IDCANCEL);

		btnY = CP_BtnY();
		int halfW = (CP_ClientW() - CP_PAD * 2 - 10) / 2;

		d->specPixels = (DWORD*)calloc(CP_SPEC_W * CP_SPEC_H, 4);
		d->vbarPixels = (DWORD*)calloc(CP_VBAR_W * CP_SPEC_H, 4);

		CP_RenderSpectrum(d);
		CP_RenderVBar(d);
		CP_UpdateControls(hwnd, d);

		return 0;
	}
	case WM_ERASEBKGND: return TRUE; // we handle it in double-buffered WM_PAINT
	case WM_PAINT: {
		PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
		RECT rc; GetClientRect(hwnd, &rc);
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
		HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBmp);

		// Paint background to memory DC
		SendMessageW(hwnd, WM_PRINTCLIENT, (WPARAM)hdcMem, PRF_ERASEBKGND);
		FillRect(hdcMem, &rc, (HBRUSH)GetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND));
		// D2D draws on top of background
		CP_Paint(hwnd, hdcMem, d);
		// Single blit to screen (no flicker)
		BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hOldBmp); DeleteObject(hBmp); DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);

		return 0;
	}
	case WM_SIZE: return 0;
	case WM_LBUTTONDOWN: {
		int mx = (short)LOWORD(lParam), my = (short)HIWORD(lParam);

		if (mx >= CP_SPEC_X && mx < CP_SPEC_X + CP_SPEC_W && my >= CP_SPEC_Y && my < CP_SPEC_Y + CP_SPEC_H) {
			d->trackingSpec = TRUE; SetCapture(hwnd);
			d->hue = (float)(mx - CP_SPEC_X) * 360.f / (CP_SPEC_W - 1);
			d->sat = 1.f - (float)(my - CP_SPEC_Y) / (CP_SPEC_H - 1);

			d->hue = d->hue < 0 ? 0 : d->hue>360 ? 360 : d->hue;
			d->sat = d->sat < 0 ? 0 : d->sat>1 ? 1 : d->sat;

			CP_UpdateControls(hwnd, d); return 0;
		}
		if (mx >= CP_VBAR_X - 4 && mx < CP_VBAR_X + CP_VBAR_W + 12 && my >= CP_VBAR_Y && my < CP_VBAR_Y + CP_SPEC_H) {
			d->trackingVBar = TRUE; SetCapture(hwnd);

			d->val = 1.f - (float)(my - CP_VBAR_Y) / (CP_SPEC_H - 1);
			d->val = d->val < 0 ? 0 : d->val>1 ? 1 : d->val;

			CP_UpdateControls(hwnd, d);
			return 0;
		}
		for (int i = 0; i < 48; i++) {
			int col = i % CP_BCOLS, row = i / CP_BCOLS;
			float cx = CP_SPEC_X + col * CP_CIRC_STEP + CP_CIRC_D / 2.f, cy = (float)CP_GridY() + row * CP_CIRC_STEP + CP_CIRC_D / 2.f;
			float dx = mx - cx, dy = my - cy;
			if (dx * dx + dy * dy <= (CP_CIRC_D / 2.f) * (CP_CIRC_D / 2.f))
			{
				CP_RGBtoHSV(g_basicColors[i], d->hue, d->sat, d->val);
				CP_UpdateControls(hwnd, d);
				return 0;
			}
		}
		for (int i = 0; i < 16; i++) {
			int col = i % 8, row = i / 8;
			float cx = CP_SPEC_X + col * CP_CIRC_STEP + CP_CIRC_D / 2.f, cy = (float)CP_CustGridY() + row * CP_CIRC_STEP + CP_CIRC_D / 2.f;
			float dx = mx - cx, dy = my - cy;

			if (dx * dx + dy * dy <= (CP_CIRC_D / 2.f) * (CP_CIRC_D / 2.f)) {
				d->selectedCustom = i;
				if (d->lpCustColors)
				{
					CP_RGBtoHSV(d->lpCustColors[i], d->hue, d->sat, d->val);
					CP_UpdateControls(hwnd, d);
				}

				return 0;
			}
		}
		return 0;
	}
	case WM_MOUSEMOVE: {
		if (!(wParam & MK_LBUTTON)) return 0;

		int mx = (short)LOWORD(lParam), my = (short)HIWORD(lParam);

		if (d->trackingSpec) {
			d->hue = (float)(mx - CP_SPEC_X) * 360.f / (CP_SPEC_W - 1);
			d->sat = 1.f - (float)(my - CP_SPEC_Y) / (CP_SPEC_H - 1);

			d->hue = d->hue < 0 ? 0 : d->hue>360 ? 360 : d->hue;
			d->sat = d->sat < 0 ? 0 : d->sat>1 ? 1 : d->sat;
			CP_UpdateControls(hwnd, d);
		}
		else if (d->trackingVBar)
		{
			d->val = 1.f - (float)(my - CP_VBAR_Y) / (CP_SPEC_H - 1);
			d->val = d->val < 0 ? 0 : d->val>1 ? 1 : d->val;
			CP_UpdateControls(hwnd, d); 
		}

		return 0;
	}
	case WM_LBUTTONUP:
		d->trackingSpec = d->trackingVBar = FALSE;
		ReleaseCapture();
		return 0;
	case WM_COMMAND: {
		WORD id = LOWORD(wParam);
		WORD notif = HIWORD(wParam);
		if (id == IDOK)
		{
			d->accepted = TRUE;
            DestroyWindow(hwnd);
            return 0;
		}
		if (id == IDCANCEL)
		{
			d->accepted = FALSE;
            DestroyWindow(hwnd);
            return 0;
		}

		if (id == 4010 && d->lpCustColors)
		{
			int idx = d->selectedCustom >= 0 ? d->selectedCustom : 0;
			d->lpCustColors[idx] = d->result;
			d->selectedCustom = idx;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}

		if (notif == EN_CHANGE && !d->updating) {
			if (id == IDC_CP_HEX) {
				WCHAR buf[16]; GetDlgItemTextW(hwnd, IDC_CP_HEX, buf, 16); LPCWSTR p = buf; if (*p == L'#')p++; unsigned int hex = 0;
				if (swscanf(p, L"%x", &hex) == 1) {
					CP_RGBtoHSV(RGB((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF), d->hue, d->sat, d->val);
					d->result = CP_HSVtoRGB(d->hue, d->sat, d->val);
					d->updating = TRUE;

					WCHAR b[8];

					wsprintfW(b, L"%d", GetRValue(d->result)); SetDlgItemTextW(hwnd, IDC_CP_R, b);
					wsprintfW(b, L"%d", GetGValue(d->result)); SetDlgItemTextW(hwnd, IDC_CP_G, b);
					wsprintfW(b, L"%d", GetBValue(d->result)); SetDlgItemTextW(hwnd, IDC_CP_B, b);

					d->updating = FALSE;
					CP_RenderSpectrum(d);
					CP_RenderVBar(d);
					CP_UploadBitmaps(d);

					InvalidateRect(hwnd, NULL, FALSE);
				}
			}
			else if (id >= IDC_CP_R && id <= IDC_CP_B)
			{
				int r = GetDlgItemInt(hwnd, IDC_CP_R, NULL, FALSE);
				int g = GetDlgItemInt(hwnd, IDC_CP_G, NULL, FALSE);
				int b = GetDlgItemInt(hwnd, IDC_CP_B, NULL, FALSE);

				r = r > 255 ? 255 : r;
				g = g > 255 ? 255 : g;
				b = b > 255 ? 255 : b;

				CP_RGBtoHSV(RGB(r, g, b), d->hue, d->sat, d->val);

				d->result = CP_HSVtoRGB(d->hue, d->sat, d->val);
				d->updating = TRUE;

				WCHAR buf[16] = L"";

				wsprintfW(buf, L"#%02X%02X%02X", GetRValue(d->result), GetGValue(d->result), GetBValue(d->result));
				SetDlgItemTextW(hwnd, IDC_CP_HEX, buf);

				d->updating = FALSE;
				CP_RenderSpectrum(d);
				CP_RenderVBar(d);
				CP_UploadBitmaps(d);

				InvalidateRect(hwnd, NULL, FALSE);
			}
		}
		return 0;
	}
	case WM_DESTROY:
    {
		CP_DestroyD2D(d);
		if (d->specPixels)
		{
			free(d->specPixels);
			d->specPixels = nullptr;
		}
		if (d->vbarPixels)
		{
			free(d->vbarPixels);
			d->vbarPixels = nullptr;
		}

		nColorDlgCount--;

		std::lock_guard<std::mutex> lock(vDlgsMutex);
		std::erase(vDlgs, hwnd);

        HWND o = GetWindow(hwnd, GW_OWNER);
        if (o) EnableWindow(o, TRUE);

		//PostQuitMessage(0);
		return 0;
    }
	case WM_CLOSE:
		d->accepted = FALSE;
		DestroyWindow(hwnd);

		return 0;
	}

	if (d && FLAG(d->flags, CC_ENABLEHOOK))
		return d->pccHook(hwnd, msg, wParam, lParam);

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static ATOM g_cpClassAtom = 0;

static BOOL CP_ShowDialog(HWND hwndOwner, ColorPickerData* data) {
	if (!g_cpClassAtom) {
		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(wc);
		wc.lpfnWndProc = CP_WndProc;
		wc.hInstance = GetModuleHandleW(NULL);

		wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
		wc.lpszClassName = L"BetterDialogs_ColorPicker";

		g_cpClassAtom = RegisterClassExW(&wc); if (!g_cpClassAtom) return FALSE;
	}

	CP_LoadStrings();
	RECT rw = { 0, 0, CP_ClientW(), CP_ClientH() };

	AdjustWindowRectEx(&rw, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE, WS_EX_DLGMODALFRAME);

	int ww = rw.right - rw.left;
	int wh = rw.bottom - rw.top;
	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;

	if (hwndOwner)
	{
		RECT ro;
		GetWindowRect(hwndOwner, &ro);
		x = ro.left + (ro.right - ro.left - ww) / 2;
		y = ro.top + (ro.bottom - ro.top - wh) / 2;
	}

	HWND hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME, L"BetterDialogs_ColorPicker", g_cpTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_VISIBLE, x, y, ww, wh, hwndOwner, NULL, GetModuleHandleW(NULL), data);

	if (!hwnd) return FALSE;
	if (hwndOwner) EnableWindow(hwndOwner, FALSE);

	MSG msg;
	while (IsWindow(hwnd) && GetMessageW(&msg, NULL, 0, 0))
	{
		if (IsDialogMessageW(hwnd, &msg))
			continue;

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return data->accepted;
}

BOOL WINAPI ChooseColorW_Hook(LPCHOOSECOLORW lpcc) {
	if (!settings.betterColorPicker || FLAG(lpcc->Flags, CC_ENABLETEMPLATE) || FLAG(lpcc->Flags, CC_ENABLEHOOK))
		return ChooseColorW_Original(lpcc);

	ColorPickerData data = {};
	data.original = lpcc->rgbResult;
    data.result = lpcc->rgbResult;
	data.lpCustColors = lpcc->lpCustColors;
    data.selectedCustom = -1;
    data.flags = lpcc->Flags;

	if (FLAG(lpcc->Flags, CC_ENABLEHOOK))
		data.pccHook = lpcc->lpfnHook;

	if (lpcc->Flags & CC_RGBINIT && lpcc->rgbResult != 0)
		CP_RGBtoHSV(lpcc->rgbResult, data.hue, data.sat, data.val);
	else
	{
		// default white
		data.hue = 0;
		data.sat = 0;
		data.val = 1.0f;
	}

	BOOL ret = CP_ShowDialog(lpcc->hwndOwner, &data);
	if (ret) lpcc->rgbResult = data.result;

	return ret;
}

// --- Modern Font Picker (WinUI-style, D2D + DirectWrite) ---

#define FP_PAD      24
#define FP_FAM_W    220
#define FP_STY_W    140
#define FP_SIZ_W    80
#define FP_GAP      12
#define FP_FAM_X    FP_PAD
#define FP_STY_X    (FP_FAM_X + FP_FAM_W + FP_GAP)
#define FP_SIZ_X    (FP_STY_X + FP_STY_W + FP_GAP)
#define FP_LABEL_Y  FP_PAD
#define FP_EDIT_Y   (FP_LABEL_Y + 22)
#define FP_LIST_Y   (FP_EDIT_Y + 32)
#define FP_LIST_H   160
#define FP_PREV_Y   (FP_LIST_Y + FP_LIST_H + 20)
#define FP_PREV_H   64
#define FP_EFF_Y    (FP_PREV_Y + FP_PREV_H + 16)
#define FP_BTN_Y    (FP_EFF_Y + 34)
#define FP_CLIENTW  (FP_SIZ_X + FP_SIZ_W + FP_PAD)
#define FP_CLIENTH  (FP_BTN_Y + 42 + FP_PAD)

#define IDC_FP_FAM_EDIT  5001
#define IDC_FP_FAM_LIST  5002
#define IDC_FP_STY_LIST  5003
#define IDC_FP_SIZ_EDIT  5004
#define IDC_FP_SIZ_LIST  5005
#define IDC_FP_STRIKE    5006
#define IDC_FP_UNDER     5007

static const int g_fpSizes[] = { 8,9,10,11,12,14,16,18,20,22,24,26,28,36,48,72 };

struct FontPickerData {
	LOGFONTW logFont;
	COLORREF color;
	DWORD flags;
	int pointSize10; // in 1/10 points
	BOOL accepted;
	BOOL updating;
	ID2D1Factory* pD2D;
	ID2D1DCRenderTarget* pRT;
	IDWriteFactory* pDW;
	IDWriteTextFormat* pLabelFmt;
	LPCFHOOKPROC pcfHook;
    LPCHOOSEFONTW lpcfOriginal;
};

// Localized font picker strings (captured from real ChooseFont dialog)
static WCHAR g_fpTitle[64] = L"Font";
static WCHAR g_fpFont[64] = L"Font:";
static WCHAR g_fpStyle[64] = L"Font style:";
static WCHAR g_fpSize[64] = L"Size:";
static WCHAR g_fpStrike[64] = L"Strikeout";
static WCHAR g_fpUnder[64] = L"Underline";
static WCHAR g_fpEffects[64] = L"Effects";
static BOOL g_fpStringsLoaded = FALSE;

// dlgs.h control IDs for ChooseFont
#define FP_STC_FONT   0x0440
#define FP_STC_STYLE  0x0441
#define FP_STC_SIZE   0x0442
#define FP_CHX_STRIKE 0x0410
#define FP_CHX_UNDER  0x0411
#define FP_GRP_EFF    0x0431

static UINT_PTR CALLBACK FP_CaptureHook(HWND hwnd, UINT uMsg, WPARAM, LPARAM) {
	if (uMsg == WM_INITDIALOG) {
		SetWindowPos(hwnd, NULL, -32000, -32000, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		GetWindowTextW(hwnd, g_fpTitle, 64);
		HWND h;

		if ((h = GetDlgItem(hwnd, FP_STC_FONT))) GetWindowTextW(h, g_fpFont, 64);
		if ((h = GetDlgItem(hwnd, FP_STC_STYLE))) GetWindowTextW(h, g_fpStyle, 64);
		if ((h = GetDlgItem(hwnd, FP_STC_SIZE))) GetWindowTextW(h, g_fpSize, 64);
		if ((h = GetDlgItem(hwnd, FP_CHX_STRIKE))) GetWindowTextW(h, g_fpStrike, 64);
		if ((h = GetDlgItem(hwnd, FP_CHX_UNDER))) GetWindowTextW(h, g_fpUnder, 64);
		if ((h = GetDlgItem(hwnd, FP_GRP_EFF))) GetWindowTextW(h, g_fpEffects, 64);

		HWND hCancel = GetDlgItem(hwnd, IDCANCEL);
		if (hCancel) GetWindowTextW(hCancel, g_cpCancel, 32); // reuse color picker cancel

		EndDialog(hwnd, IDCANCEL);
		return TRUE;
	}
	return FALSE;
}

// we dont know the string IDs, so it's going to stay english
//static void FP_LoadStrings() {
//	if (g_fpStringsLoaded) return;
//	g_fpStringsLoaded = TRUE;
//
//	LOGFONTW lf = {}; lf.lfCharSet = DEFAULT_CHARSET;
//	wcscpy(lf.lfFaceName, L"Arial");
//
//	lf.lfHeight = -16;
//	CHOOSEFONTW cf = {};
//
//	cf.lStructSize = sizeof(cf);
//	cf.lpLogFont = &lf;
//	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS | CF_ENABLEHOOK;
//	cf.lpfnHook = FP_CaptureHook;
//
//	COLORREF cc = 0;
//	cf.rgbColors = cc;
//	ChooseFontW_Original(&cf);
//
//	// Strip '&' accelerators
//	auto strip = [](WCHAR* s) { WCHAR* r = s; WCHAR* w = s; while (*r) { if (*r != L'&') *w++ = *r; r++; } *w = 0; };
//	strip(g_fpFont); strip(g_fpStyle); strip(g_fpSize);
//	strip(g_fpStrike); strip(g_fpUnder); strip(g_fpEffects);
//}

static void FP_LoadStrings() {
	if (g_fpStringsLoaded) return;
	g_fpStringsLoaded = TRUE;

	if (settings.localized) {
		LOGFONTW lf = {}; lf.lfCharSet = DEFAULT_CHARSET;
		wcscpy(lf.lfFaceName, L"Arial");
		
		lf.lfHeight = -16;
		CHOOSEFONTW cf = {};
		
		cf.lStructSize = sizeof(cf);
		cf.lpLogFont = &lf;
		cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS | CF_ENABLEHOOK;
		cf.lpfnHook = FP_CaptureHook;
		
		COLORREF cc = 0;
		cf.rgbColors = cc;
		ChooseFontW_Original(&cf);
		
		// Strip '&' accelerators
		auto strip = [](WCHAR* s) { WCHAR* r = s; WCHAR* w = s; while (*r) { if (*r != L'&') *w++ = *r; r++; } *w = 0; };
		strip(g_fpFont); strip(g_fpStyle); strip(g_fpSize);
		strip(g_fpStrike); strip(g_fpUnder); strip(g_fpEffects);
	}
	else {
		wcscpy_s(g_fpFont, L"Font");
		wcscpy_s(g_fpStyle, L"Font style");
		wcscpy_s(g_fpSize, L"Size");
		wcscpy_s(g_fpStrike, L"Strikethrough");
		wcscpy_s(g_fpUnder, L"Underline");
		wcscpy_s(g_fpEffects, L"Effects");
	}
}

static int CALLBACK FP_EnumProc(const LOGFONTW* lf, const TEXTMETRICW*, DWORD fontType, LPARAM lp) {
	HWND hList = (HWND)lp;
	if (lf->lfFaceName[0] == L'@') return 1; // skip vertical

	if (SendMessageW(hList, LB_FINDSTRINGEXACT, -1, (LPARAM)lf->lfFaceName) == LB_ERR)
		SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)lf->lfFaceName);

	return 1;
}

static void FP_UpdatePreview(HWND hwnd) {
	InvalidateRect(hwnd, NULL, FALSE);
}

static void FP_SyncFromControls(HWND hwnd, FontPickerData* d) {
	if (d->updating) return;
	d->updating = TRUE;

	// Family
	GetDlgItemTextW(hwnd, IDC_FP_FAM_EDIT, d->logFont.lfFaceName, LF_FACESIZE);

	// Style
	int si = (int)SendDlgItemMessageW(hwnd, IDC_FP_STY_LIST, LB_GETCURSEL, 0, 0);
	d->logFont.lfWeight = (si == 1 || si == 3) ? FW_BOLD : FW_NORMAL;
	d->logFont.lfItalic = (si == 2 || si == 3) ? TRUE : FALSE;

	// Size
	WCHAR szBuf[16]; GetDlgItemTextW(hwnd, IDC_FP_SIZ_EDIT, szBuf, 16);
	int pts = _wtoi(szBuf);
	if (pts > 0) {
		d->pointSize10 = pts * 10;
		HDC hdc = GetDC(hwnd);

		d->logFont.lfHeight = -MulDiv(pts, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		ReleaseDC(hwnd, hdc);
	}

	// Effects
	if (d->flags & CF_EFFECTS) {
		d->logFont.lfStrikeOut = (BYTE)SendDlgItemMessageW(hwnd, IDC_FP_STRIKE, BM_GETCHECK, 0, 0);
		d->logFont.lfUnderline = (BYTE)SendDlgItemMessageW(hwnd, IDC_FP_UNDER, BM_GETCHECK, 0, 0);
	}

	d->updating = FALSE;
	FP_UpdatePreview(hwnd);
}

static void FP_PaintPreview(HWND hwnd, HDC hdc, FontPickerData* d) {
	if (!d->pD2D) D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d->pD2D);
	if (!d->pDW) DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&d->pDW);
	if (!d->pD2D || !d->pDW) return;

	if (!d->pRT) {
		D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

		d->pD2D->CreateDCRenderTarget(&rtp, &d->pRT);
	}
	if (!d->pLabelFmt) {
		d->pDW->CreateTextFormat(L"Segoe UI Variable", nullptr,
			DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
			13.0f, L"", &d->pLabelFmt);
	}
	if (!d->pRT) return;

	RECT rc; GetClientRect(hwnd, &rc);
	d->pRT->BindDC(hdc, &rc);
	d->pRT->BeginDraw();

	// Labels (DirectWrite)
	ID2D1SolidColorBrush* br = nullptr;
	d->pRT->CreateSolidColorBrush(D2D1::ColorF(GetRValue(GetSysColor(COLOR_WINDOWTEXT)) / 255.f,
		GetGValue(GetSysColor(COLOR_WINDOWTEXT)) / 255.f, GetBValue(GetSysColor(COLOR_WINDOWTEXT)) / 255.f), &br);

	if (br && d->pLabelFmt) {
		d->pRT->DrawTextW(g_fpFont, (UINT32)wcslen(g_fpFont), d->pLabelFmt, D2D1::RectF((float)FP_FAM_X, (float)FP_LABEL_Y, (float)(FP_FAM_X + FP_FAM_W), (float)(FP_LABEL_Y + 20)), br);
		//d->pRT->DrawTextW(g_fpStyle, (UINT32)wcslen(g_fpStyle), d->pLabelFmt, D2D1::RectF((float)FP_STY_X, ((float)FP_LABEL_Y) + 48, (float)(FP_STY_X+FP_STY_W), (float)(FP_LABEL_Y+32)), br);
		d->pRT->DrawTextW(g_fpStyle, (UINT32)wcslen(g_fpStyle), d->pLabelFmt, D2D1::RectF((float)FP_STY_X, (float)FP_LABEL_Y, (float)(FP_STY_X + FP_STY_W), (float)(FP_LABEL_Y + 20)), br);
		d->pRT->DrawTextW(g_fpSize, (UINT32)wcslen(g_fpSize), d->pLabelFmt, D2D1::RectF((float)FP_SIZ_X, (float)FP_LABEL_Y, (float)(FP_SIZ_X + FP_SIZ_W), (float)(FP_LABEL_Y + 20)), br);
	}

	// Preview area
	D2D1_ROUNDED_RECT prevRR = D2D1::RoundedRect(
		D2D1::RectF((float)FP_FAM_X, (float)FP_PREV_Y, (float)(FP_CLIENTW - FP_PAD), (float)(FP_PREV_Y + FP_PREV_H)), 5.f, 5.f);

	// Background
	ID2D1SolidColorBrush* bgBr = nullptr;
	d->pRT->CreateSolidColorBrush(D2D1::ColorF(GetRValue(GetSysColor(COLOR_WINDOW)) / 255.f,
		GetGValue(GetSysColor(COLOR_WINDOW)) / 255.f, GetBValue(GetSysColor(COLOR_WINDOW)) / 255.f), &bgBr);

	if (bgBr)
	{
		d->pRT->FillRoundedRectangle(prevRR, bgBr);
		bgBr->Release();
	}

	// Border
	ID2D1SolidColorBrush* bdrBr = nullptr;
	d->pRT->CreateSolidColorBrush(D2D1::ColorF(0.4f, 0.4f, 0.4f, 0.5f), &bdrBr);

	if (bdrBr)
	{
		d->pRT->DrawRoundedRectangle(prevRR, bdrBr);
		bdrBr->Release();
	}

	// Preview text with selected font
	float fontSize = d->pointSize10 > 0 ? d->pointSize10 / 10.0f : 12.0f;

	if (fontSize > 36) fontSize = 36; // cap for preview
	if (fontSize < 8) fontSize = 8;

	DWRITE_FONT_WEIGHT weight = d->logFont.lfWeight >= FW_BOLD ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR;
	DWRITE_FONT_STYLE style = d->logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;

	IDWriteTextFormat* pPrevFmt = nullptr;
	d->pDW->CreateTextFormat(d->logFont.lfFaceName[0] ? d->logFont.lfFaceName : L"Segoe UI Variable",
		nullptr, weight, style, DWRITE_FONT_STRETCH_NORMAL, fontSize * 1.2f, L"", &pPrevFmt);

	if (pPrevFmt) {
		pPrevFmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pPrevFmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		LPCWSTR sample = L"AaBbYyZz";

		if (br) {
			d->pRT->DrawTextW(sample, (UINT32)wcslen(sample), pPrevFmt,
				D2D1::RectF((float)FP_FAM_X + 4, (float)FP_PREV_Y + 2, (float)(FP_CLIENTW - FP_PAD - 4), (float)(FP_PREV_Y + FP_PREV_H - 2)), br);
		}

		pPrevFmt->Release();
	}
	if (br) br->Release();

	d->pRT->EndDraw();
}

static LRESULT CALLBACK FP_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	FontPickerData* d = (FontPickerData*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
	switch (msg) {
	case WM_CREATE: {
		nFontDlgCount++;

		std::lock_guard<std::mutex> lock(vDlgsMutex);
		vDlgs.push_back(hwnd);
		
		CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
		d = (FontPickerData*)cs->lpCreateParams;
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)d);

		HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		auto mk = [&](LPCWSTR cls, LPCWSTR text, DWORD style, DWORD exs, int x, int y, int w, int h, int id = 0) {
			HWND hc = CreateWindowExW(exs, cls, text, WS_CHILD | WS_VISIBLE | style, x, y, w, h, hwnd, (HMENU)(INT_PTR)id, NULL, NULL);
			SendMessageW(hc, WM_SETFONT, (WPARAM)hFont, 0); return hc; };

		// Family
		mk(L"EDIT", d->logFont.lfFaceName, ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, FP_FAM_X, FP_EDIT_Y, FP_FAM_W, 26, IDC_FP_FAM_EDIT);
		HWND hFamList = mk(L"LISTBOX", L"", LBS_SORT | LBS_NOTIFY | WS_VSCROLL, WS_EX_CLIENTEDGE, FP_FAM_X, FP_LIST_Y, FP_FAM_W, FP_LIST_H, IDC_FP_FAM_LIST);

		HDC hdc = GetDC(hwnd);
		LOGFONTW lfEnum = {}; lfEnum.lfCharSet = DEFAULT_CHARSET;

		if (d->flags & CF_FIXEDPITCHONLY) lfEnum.lfPitchAndFamily = FIXED_PITCH;

		EnumFontFamiliesExW(hdc, &lfEnum, (FONTENUMPROCW)FP_EnumProc, (LPARAM)hFamList, 0);
		ReleaseDC(hwnd, hdc);

		int famIdx = (int)SendMessageW(hFamList, LB_FINDSTRINGEXACT, -1, (LPARAM)d->logFont.lfFaceName);
		if (famIdx != LB_ERR) SendMessageW(hFamList, LB_SETCURSEL, famIdx, 0);

		// Style
		HWND hStyList = mk(L"LISTBOX", L"", LBS_NOTIFY, WS_EX_CLIENTEDGE, FP_STY_X, FP_LIST_Y, FP_STY_W, FP_LIST_H, IDC_FP_STY_LIST);

		SendMessageW(hStyList, LB_ADDSTRING, 0, (LPARAM)L"Regular");
		SendMessageW(hStyList, LB_ADDSTRING, 0, (LPARAM)L"Bold");
		SendMessageW(hStyList, LB_ADDSTRING, 0, (LPARAM)L"Italic");
		SendMessageW(hStyList, LB_ADDSTRING, 0, (LPARAM)L"Bold Italic");

		int si = 0;
		if (d->logFont.lfWeight >= FW_BOLD && d->logFont.lfItalic) si = 3;
		else if (d->logFont.lfWeight >= FW_BOLD) si = 1;
		else if (d->logFont.lfItalic) si = 2;

		SendMessageW(hStyList, LB_SETCURSEL, si, 0);

		// Size
		WCHAR szBuf[16];
		wsprintfW(szBuf, L"%d", d->pointSize10 > 0 ? d->pointSize10 / 10 : 12);

		mk(L"EDIT", szBuf, ES_AUTOHSCROLL | ES_NUMBER, WS_EX_CLIENTEDGE, FP_SIZ_X, FP_EDIT_Y, FP_SIZ_W, 26, IDC_FP_SIZ_EDIT);

		HWND hSizList = mk(L"LISTBOX", L"", LBS_NOTIFY | WS_VSCROLL, WS_EX_CLIENTEDGE, FP_SIZ_X, FP_LIST_Y, FP_SIZ_W, FP_LIST_H, IDC_FP_SIZ_LIST);
		for (int i = 0; i < (int)(sizeof(g_fpSizes) / sizeof(g_fpSizes[0])); i++) {
			wsprintfW(szBuf, L"%d", g_fpSizes[i]);
			SendMessageW(hSizList, LB_ADDSTRING, 0, (LPARAM)szBuf);
		}

		int curSz = d->pointSize10 > 0 ? d->pointSize10 / 10 : 12;
		for (int i = 0; i < (int)(sizeof(g_fpSizes) / sizeof(g_fpSizes[0])); i++) {
			if (g_fpSizes[i] == curSz)
			{
				SendMessageW(hSizList, LB_SETCURSEL, i, 0);
				break;
			}
		}

		// Effects
		if (d->flags & CF_EFFECTS) {
			mk(L"BUTTON", g_fpStrike, BS_AUTOCHECKBOX, 0, FP_FAM_X, FP_EFF_Y, 130, 22, IDC_FP_STRIKE);
			mk(L"BUTTON", g_fpUnder, BS_AUTOCHECKBOX, 0, FP_FAM_X + 140, FP_EFF_Y, 130, 22, IDC_FP_UNDER);

			if (d->logFont.lfStrikeOut) SendDlgItemMessageW(hwnd, IDC_FP_STRIKE, BM_SETCHECK, BST_CHECKED, 0);
			if (d->logFont.lfUnderline) SendDlgItemMessageW(hwnd, IDC_FP_UNDER, BM_SETCHECK, BST_CHECKED, 0);
		}

		// Buttons
		//int halfW = (FP_CLIENTW-FP_PAD*2-FP_GAP)/2;
		// mk(L"BUTTON", L"OK", BS_DEFPUSHBUTTON, 0, FP_PAD + halfW, FP_BTN_Y, 80, 24, IDOK);
		// mk(L"BUTTON", g_cpCancel, BS_PUSHBUTTON, 0, FP_PAD+halfW+FP_GAP, FP_BTN_Y, 80, 24, IDCANCEL);
		mk(L"BUTTON", L"OK", BS_DEFPUSHBUTTON, 0, FP_SIZ_X + 16, FP_BTN_Y, 80, 24, IDOK);
		mk(L"BUTTON", g_cpCancel, BS_PUSHBUTTON, 0, FP_SIZ_X + 16, FP_BTN_Y + 32, 80, 24, IDCANCEL);
		
        if (FLAG(d->flags, CF_ENABLEHOOK))
            return d->pcfHook(hwnd, WM_INITDIALOG, wParam, (LPARAM)d->lpcfOriginal);
        
        return 0;
	}

	case WM_ERASEBKGND: return TRUE;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT rc;
		GetClientRect(hwnd, &rc);

		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
		HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmp);

		FillRect(hdcMem, &rc, (HBRUSH)GetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND));
		FP_PaintPreview(hwnd, hdcMem, d);
		BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hOld); DeleteObject(hBmp); DeleteDC(hdcMem);
		EndPaint(hwnd, &ps); return 0;
	}

	case WM_COMMAND: {
		WORD id = LOWORD(wParam);
		WORD notif = HIWORD(wParam);

		if (id == IDOK)
		{
			FP_SyncFromControls(hwnd, d);
			d->accepted = TRUE;
			DestroyWindow(hwnd);
			return 0;
		}

		if (id == IDCANCEL)
		{
			d->accepted = FALSE;
			DestroyWindow(hwnd);
			return 0;
		}

		// Family list selection
		if (id == IDC_FP_FAM_LIST && notif == LBN_SELCHANGE) {
			int sel = (int)SendDlgItemMessageW(hwnd, IDC_FP_FAM_LIST, LB_GETCURSEL, 0, 0);

			if (sel != LB_ERR) {
				WCHAR name[LF_FACESIZE];
				SendDlgItemMessageW(hwnd, IDC_FP_FAM_LIST, LB_GETTEXT, sel, (LPARAM)name);

				d->updating = TRUE;
				SetDlgItemTextW(hwnd, IDC_FP_FAM_EDIT, name);

				d->updating = FALSE;
				FP_SyncFromControls(hwnd, d);
			}
			return 0;
		}

		// Style list selection
		if (id == IDC_FP_STY_LIST && notif == LBN_SELCHANGE) {
			FP_SyncFromControls(hwnd, d); return 0;
		}

		// Size list selection
		if (id == IDC_FP_SIZ_LIST && notif == LBN_SELCHANGE) {
			int sel = (int)SendDlgItemMessageW(hwnd, IDC_FP_SIZ_LIST, LB_GETCURSEL, 0, 0);

			if (sel != LB_ERR) {
				WCHAR sz[16];
				SendDlgItemMessageW(hwnd, IDC_FP_SIZ_LIST, LB_GETTEXT, sel, (LPARAM)sz);
				d->updating = TRUE;

				SetDlgItemTextW(hwnd, IDC_FP_SIZ_EDIT, sz);
				d->updating = FALSE;

				FP_SyncFromControls(hwnd, d);
			}
			return 0;
		}
		// Edit changes
		if (notif == EN_CHANGE && !d->updating) {
			if (id == IDC_FP_FAM_EDIT) {
				// Auto-select in list
				WCHAR text[LF_FACESIZE]; GetDlgItemTextW(hwnd, IDC_FP_FAM_EDIT, text, LF_FACESIZE);
				int idx = (int)SendDlgItemMessageW(hwnd, IDC_FP_FAM_LIST, LB_FINDSTRING, -1, (LPARAM)text);

				if (idx != LB_ERR) SendDlgItemMessageW(hwnd, IDC_FP_FAM_LIST, LB_SETCURSEL, idx, 0);
			}
			FP_SyncFromControls(hwnd, d);
		}

		if (id == IDC_FP_STRIKE || id == IDC_FP_UNDER)
			FP_SyncFromControls(hwnd, d);

		return 0;
	}

	case WM_DESTROY:
    {
		if (d->pLabelFmt) { d->pLabelFmt->Release(); d->pLabelFmt = nullptr; }
		if (d->pRT) { d->pRT->Release(); d->pRT = nullptr; }
		if (d->pDW) { d->pDW->Release(); d->pDW = nullptr; }
		if (d->pD2D) { d->pD2D->Release(); d->pD2D = nullptr; }

		nFontDlgCount--;

		std::lock_guard<std::mutex> lock(vDlgsMutex);
		std::erase(vDlgs, hwnd);
		return 0;
    }
	case WM_CLOSE:
		d->accepted = FALSE;
		DestroyWindow(hwnd);
		return 0;
	}

	if (d && FLAG(d->flags, CF_ENABLEHOOK) && d->pcfHook)
		return d->pcfHook(hwnd, msg, wParam, lParam);

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static ATOM g_fpClassAtom = 0;
static BOOL FP_ShowDialog(HWND hwndOwner, FontPickerData* data) {
	if (!g_fpClassAtom) {
		WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = FP_WndProc;
		wc.hInstance = GetModuleHandleW(NULL); wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1); wc.lpszClassName = L"BetterDialogs_FontPicker";
		g_fpClassAtom = RegisterClassExW(&wc); if (!g_fpClassAtom) return FALSE;
	}

	FP_LoadStrings();
	RECT rw = { 0,0,FP_CLIENTW,FP_CLIENTH };

	AdjustWindowRectEx(&rw, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE, WS_EX_DLGMODALFRAME);
	int ww = rw.right - rw.left, wh = rw.bottom - rw.top, x = CW_USEDEFAULT, y = CW_USEDEFAULT;

	if (hwndOwner)
	{
		RECT ro;
		GetWindowRect(hwndOwner, &ro);
		x = ro.left + (ro.right - ro.left - ww) / 2;
		y = ro.top + (ro.bottom - ro.top - wh) / 2;
	}

	HWND hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME, L"BetterDialogs_FontPicker", g_fpTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_VISIBLE, x, y, ww, wh, hwndOwner, NULL, GetModuleHandleW(NULL), data);

	if (!hwnd) return FALSE;
	if (hwndOwner) EnableWindow(hwndOwner, FALSE);

    MSG msg;
    while (IsWindow(hwnd) && GetMessageW(&msg, NULL, 0, 0))
    {
        if (IsDialogMessageW(hwnd, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (hwndOwner) EnableWindow(hwndOwner, TRUE);

	return data->accepted;
}

BOOL WINAPI ChooseFontW_Hook(LPCHOOSEFONTW lpcf) {
	if (!settings.modernFontPicker) return ChooseFontW_Original(lpcf);
	if (FLAG(lpcf->Flags, CF_ENABLETEMPLATE) || FLAG(lpcf->Flags, CF_ENABLEHOOK))
        return ChooseFontW_Original(lpcf);

	FontPickerData data = {};
	data.flags = lpcf->Flags;
	data.color = lpcf->rgbColors;
    data.lpcfOriginal = lpcf;

	if (FLAG(lpcf->Flags, CF_ENABLEHOOK))
		data.pcfHook = lpcf->lpfnHook;

	if (lpcf->Flags & CF_INITTOLOGFONTSTRUCT && lpcf->lpLogFont)
		memcpy(&data.logFont, lpcf->lpLogFont, sizeof(LOGFONTW));

	data.pointSize10 = lpcf->iPointSize;

	if (data.pointSize10 == 0 && data.logFont.lfHeight != 0) {
		HDC hdc = GetDC(NULL);

		data.pointSize10 = MulDiv(-data.logFont.lfHeight, 720, GetDeviceCaps(hdc, LOGPIXELSY));
		ReleaseDC(NULL, hdc);
	}

	BOOL ret = FP_ShowDialog(lpcf->hwndOwner, &data);
	if (ret) {
		if (lpcf->lpLogFont) memcpy(lpcf->lpLogFont, &data.logFont, sizeof(LOGFONTW));

		lpcf->iPointSize = data.pointSize10;
		lpcf->rgbColors = data.color;
	}
	return ret;
}

// --- Helper: parse double-null-terminated filter string into COMDLG_FILTERSPEC array ---
static HRESULT ParseFilterString(LPCWSTR lpstrFilter, COMDLG_FILTERSPEC** ppSpecs, UINT* pCount) {
	*ppSpecs = nullptr;
	*pCount = 0;

	if (!lpstrFilter || !*lpstrFilter)
		return S_OK;

	// Count pairs
	UINT count = 0;
	LPCWSTR p = lpstrFilter;

	while (*p) {
		p += wcslen(p) + 1; // skip name
		if (!*p) break;
		p += wcslen(p) + 1; // skip pattern
		count++;
	}

	if (count == 0)
		return S_OK;

	COMDLG_FILTERSPEC* specs = (COMDLG_FILTERSPEC*)CoTaskMemAlloc(count * sizeof(COMDLG_FILTERSPEC));
	if (!specs)
		return E_OUTOFMEMORY;

	p = lpstrFilter;
	for (UINT i = 0; i < count; i++) {
		specs[i].pszName = p;
		p += wcslen(p) + 1;
		specs[i].pszSpec = p;
		p += wcslen(p) + 1;
	}

	*ppSpecs = specs;
	*pCount = count;
	return S_OK;
}

// --- DLGTEMPLATE parser: extracts custom controls into IFileDialogCustomize ---

#define IDC_MODERN_READONLY 0xBD01

// Align pointer up to DWORD boundary
static const BYTE* AlignToDword(const BYTE* p) {
	return (const BYTE*)(((ULONG_PTR)p + 3) & ~(ULONG_PTR)3);
}

// Skip a sz_Or_Ord field (menu, class, title in DLGTEMPLATE), return pointer past it
// Also optionally returns the string value
static const WORD* SkipSzOrOrd(const WORD* p, LPCWSTR* outStr = nullptr) {
	if (outStr) *outStr = nullptr;
	if (*p == 0x0000) {
		return p + 1;
	}
	else if (*p == 0xFFFF) {
		return p + 2;
	}
	else {
		if (outStr) *outStr = (LPCWSTR)p;
		return p + wcslen((LPCWSTR)p) + 1;
	}
}

// Get the DLGTEMPLATE pointer from OPENFILENAME flags
static const BYTE* GetTemplateData(LPOPENFILENAMEW lpofn) {
	if (lpofn->Flags & OFN_ENABLETEMPLATEHANDLE) {
		// hInstance is a global memory handle containing the template
		return (const BYTE*)lpofn->hInstance;
	}

	if (lpofn->Flags & OFN_ENABLETEMPLATE) {
		if (!lpofn->hInstance || !lpofn->lpTemplateName)
			return nullptr;

		HRSRC hRes = FindResourceW(lpofn->hInstance, lpofn->lpTemplateName, RT_DIALOG);

		if (!hRes) return nullptr;

		HGLOBAL hGlob = LoadResource(lpofn->hInstance, hRes);

		if (!hGlob) return nullptr;
		return (const BYTE*)LockResource(hGlob);
	}
	return nullptr;
}

static void ParseTemplateControls(const BYTE* pData, IFileDialogCustomize* pCustomize) {
	if (!pData || !pCustomize) return;

	const WORD* pw = (const WORD*)pData;

	// Detect DLGTEMPLATEEX (signature: dlgVer=1, signature=0xFFFF)
	BOOL isEx = (pw[0] == 1 && pw[1] == 0xFFFF);

	DWORD dlgStyle;
	WORD cdit;

	if (isEx) {
		// DLGTEMPLATEEX layout:
		// WORD dlgVer, WORD signature, DWORD helpID, DWORD exStyle, DWORD style,
		// WORD cDlgItems, short x, y, cx, cy
		dlgStyle = ((const DWORD*)pData)[4]; // offset 16
		cdit = pw[10];                        // offset 20
		pw = (const WORD*)(pData + 26);       // skip 26-byte header
	}
	else {
		dlgStyle = ((const DLGTEMPLATE*)pData)->style;
		cdit = ((const DLGTEMPLATE*)pData)->cdit;
		// DLGTEMPLATE: DWORD style(4) + DWORD exStyle(4) + WORD cdit(2) + short x(2) + y(2) + cx(2) + cy(2) = 18
		pw = (const WORD*)(pData + 18);
	}

	// Skip menu, class, title
	pw = SkipSzOrOrd(pw);
	pw = SkipSzOrOrd(pw);
	pw = SkipSzOrOrd(pw);

	// Skip font if DS_SETFONT or DS_SHELLFONT
	if (dlgStyle & DS_SETFONT) {
		pw++; // pointSize
		if (isEx) {
			pw++; // weight
			pw++; // italic(1) + charset(1) packed in WORD
		}
		pw += wcslen((LPCWSTR)pw) + 1; // font name
	}

	BOOL hasVisualGroup = FALSE;

	for (WORD i = 0; i < cdit; i++) {
		// Align to DWORD
		pw = (const WORD*)AlignToDword((const BYTE*)pw);

		DWORD itemStyle;
		WORD id;

		if (isEx) {
			// DLGITEMTEMPLATEEX: DWORD helpID(4), DWORD exStyle(4), DWORD style(4),
			// short x(2), y(2), cx(2), cy(2), DWORD id(4) = 24 bytes
			itemStyle = ((const DWORD*)pw)[2]; // style at offset 8
			id = (WORD)(((const DWORD*)pw)[5]); // id at offset 20 (DWORD, take low WORD)
			pw = (const WORD*)((const BYTE*)pw + 24);
		}
		else {
			// DLGITEMTEMPLATE: DWORD style(4), DWORD exStyle(4),
			// short x(2), y(2), cx(2), cy(2), WORD id(2) = 18 bytes
			itemStyle = *(const DWORD*)pw;
			id = *(const WORD*)((const BYTE*)pw + 16);
			pw = (const WORD*)((const BYTE*)pw + 18);
		}

		// Read class (sz_or_ord)
		WORD classOrd = 0;
		LPCWSTR className = nullptr;
		if (*pw == 0xFFFF) {
			classOrd = pw[1];
			pw += 2;
		}
		else {
			pw = SkipSzOrOrd(pw, &className);
		}

		// Read title (sz_or_ord)
		LPCWSTR title = nullptr;
		if (*pw == 0xFFFF) {
			pw += 2;
		}
		else {
			pw = SkipSzOrOrd(pw, &title);
		}

		// Skip extra data
		WORD extraBytes = *pw++;
		if (extraBytes > 0)
			pw = (const WORD*)((const BYTE*)pw + extraBytes);

		// Skip standard common dialog control IDs (0x0400-0x04FF range, plus IDOK/IDCANCEL/IDHELP/0)
		if (id == 0 || id == IDOK || id == IDCANCEL || id == IDHELP)
			continue;
		if (id >= 0x0400 && id <= 0x04FF)
			continue;

		// Skip invisible controls
		if (!(itemStyle & WS_VISIBLE))
			continue;

		WORD btnType = (WORD)(itemStyle & 0x0F);
		BOOL isButton = (classOrd == 0x0080) ||
			(className && _wcsicmp(className, L"BUTTON") == 0);

		BOOL isStatic = (classOrd == 0x0082) ||
			(className && _wcsicmp(className, L"STATIC") == 0);

		BOOL isEdit = (classOrd == 0x0081) ||
			(className && _wcsicmp(className, L"EDIT") == 0);

		BOOL isCombo = (classOrd == 0x0085) ||
			(className && _wcsicmp(className, L"COMBOBOX") == 0);

		LPCWSTR safeTitle = (title && title[0]) ? title : L"";

		if (isStatic) {
			if (safeTitle[0])
				pCustomize->AddText(id, safeTitle);
		}
		else if (isButton) {
			if (btnType == BS_CHECKBOX || btnType == BS_AUTOCHECKBOX ||
				btnType == BS_3STATE || btnType == BS_AUTO3STATE) {
				pCustomize->AddCheckButton(id, safeTitle, FALSE);
			}
			else if (btnType == BS_GROUPBOX) {
				if (hasVisualGroup)
					pCustomize->EndVisualGroup();
				pCustomize->StartVisualGroup(id, safeTitle);
				hasVisualGroup = TRUE;
			}
			else if (btnType == BS_PUSHBUTTON || btnType == BS_DEFPUSHBUTTON) {
				pCustomize->AddPushButton(id, safeTitle);
			}
		}
		else if (isEdit) {
			pCustomize->AddEditBox(id, safeTitle);
		}
		else if (isCombo) {
			pCustomize->AddComboBox(id);
		}
	}

	if (hasVisualGroup)
		pCustomize->EndVisualGroup();
}

#define OPENCHOICES 0
#define OPEN 0
#define OPEN_AS_READONLY 1

// --- Common implementation for both Open and Save ---
static BOOL ModernFileDialog_Impl(LPOPENFILENAMEW lpofn, BOOL bOpen) {
	HRESULT hrCo = CoInitialize(0);
	if (FAILED(hrCo))
		return bOpen ? GetOpenFileNameW_Original(lpofn) : GetSaveFileNameW_Original(lpofn);

	BOOL weInitCom = (hrCo == S_OK); // S_FALSE = already initialized by caller

	BOOL result = FALSE;
	HRESULT hr;
	IFileDialog* pfd = nullptr;

	if (bOpen) {
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS((IFileOpenDialog**)&pfd));
	}
	else {
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS((IFileSaveDialog**)&pfd));
	}

	if (FAILED(hr)) {
		if (weInitCom) CoUninitialize();
		return FALSE;
	}

	// Set options
	FILEOPENDIALOGOPTIONS fos = 0;
	pfd->GetOptions(&fos);

	if (lpofn->Flags & OFN_FILEMUSTEXIST)
		fos |= FOS_FILEMUSTEXIST;

	if (lpofn->Flags & OFN_PATHMUSTEXIST)
		fos |= FOS_PATHMUSTEXIST;

	if (lpofn->Flags & OFN_NOCHANGEDIR)
		fos |= FOS_NOCHANGEDIR;

	if (lpofn->Flags & OFN_ALLOWMULTISELECT)
		fos |= FOS_ALLOWMULTISELECT;

	if (lpofn->Flags & OFN_NODEREFERENCELINKS)
		fos |= FOS_NODEREFERENCELINKS;

	if (lpofn->Flags & OFN_FORCESHOWHIDDEN)
		fos |= FOS_FORCESHOWHIDDEN;

	if (lpofn->Flags & OFN_OVERWRITEPROMPT)
		fos |= FOS_OVERWRITEPROMPT;

	if (lpofn->Flags & OFN_CREATEPROMPT)
		fos |= FOS_CREATEPROMPT;

	pfd->SetOptions(fos);

	// Set title
	if (lpofn->lpstrTitle)
		pfd->SetTitle(lpofn->lpstrTitle);

	// Set filters
	COMDLG_FILTERSPEC* filterSpecs = nullptr;
	UINT filterCount = 0;

	ParseFilterString(lpofn->lpstrFilter, &filterSpecs, &filterCount);

	if (filterCount > 0) {
		pfd->SetFileTypes(filterCount, filterSpecs);
		if (lpofn->nFilterIndex > 0)
			pfd->SetFileTypeIndex(lpofn->nFilterIndex);
	}

	// Set default extension
	if (lpofn->lpstrDefExt)
		pfd->SetDefaultExtension(lpofn->lpstrDefExt);

	// Set initial directory
	if (lpofn->lpstrInitialDir) {
		IShellItem* psiFolder = nullptr;
		hr = SHCreateItemFromParsingName(lpofn->lpstrInitialDir, NULL, IID_PPV_ARGS(&psiFolder));

		if (SUCCEEDED(hr)) {
			pfd->SetFolder(psiFolder);
			psiFolder->Release();
		}
	}

	// Set initial filename
	if (lpofn->lpstrFile && lpofn->lpstrFile[0]) {
		pfd->SetFileName(lpofn->lpstrFile);
	}
	else if (lpofn->lpstrFileTitle && lpofn->lpstrFileTitle[0]) {
		pfd->SetFileName(lpofn->lpstrFileTitle);
	}

	// Add custom controls from legacy dialog template and OFN_READONLY
	IFileDialogCustomize* pCustomize = nullptr;
	hr = pfd->QueryInterface(IID_PPV_ARGS(&pCustomize));

	if (SUCCEEDED(hr)) {
		// OFN_READONLY checkbox
		if (!FLAG(lpofn->Flags, OFN_HIDEREADONLY))
		{
			hr = pCustomize->EnableOpenDropDown(OPENCHOICES);
			if (SUCCEEDED(hr))
			{
				hr = pCustomize->AddControlItem(OPENCHOICES, OPEN, L"&Open");
			}

			if (SUCCEEDED(hr))
			{
				hr = pCustomize->AddControlItem(OPENCHOICES,
					OPEN_AS_READONLY,
					L"Open as &read-only");
			}
		}

		// Parse legacy dialog template if present
		const BYTE* pTemplateData = GetTemplateData(lpofn);
		if (pTemplateData) {
			ParseTemplateControls(pTemplateData, pCustomize);
		}

		pCustomize->Release();
	}

	// Show
	hr = pfd->Show(lpofn->hwndOwner);

	if (SUCCEEDED(hr)) {
		// Handle multi-select for Open
		if (bOpen && (lpofn->Flags & OFN_ALLOWMULTISELECT)) {
			IFileOpenDialog* pfod = (IFileOpenDialog*)pfd;
			IShellItemArray* pResults = nullptr;

			hr = pfod->GetResults(&pResults);

			if (SUCCEEDED(hr)) {
				DWORD count = 0;
				pResults->GetCount(&count);

				if (count == 1) {
					// Single file - just fill the path
					IShellItem* psi = nullptr;
					hr = pResults->GetItemAt(0, &psi);

					if (SUCCEEDED(hr)) {
						LPWSTR pszPath = nullptr;
						hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);

						if (SUCCEEDED(hr)) {
							wcsncpy(lpofn->lpstrFile, pszPath, lpofn->nMaxFile);
							lpofn->lpstrFile[lpofn->nMaxFile - 1] = L'\0';

							// Calculate nFileOffset and nFileExtension
							LPCWSTR pSlash = wcsrchr(pszPath, L'\\');
							lpofn->nFileOffset = pSlash ? (WORD)(pSlash - pszPath + 1) : 0;
							LPCWSTR pDot = wcsrchr(pszPath, L'.');
							lpofn->nFileExtension = pDot ? (WORD)(pDot - pszPath + 1) : 0;

							CoTaskMemFree(pszPath);
							result = TRUE;
						}
						psi->Release();
					}
				}
				else if (count > 1) {
					// Multi-select: directory\0file1\0file2\0\0
					// First, get folder from first item
					IShellItem* psiFirst = nullptr;
					pResults->GetItemAt(0, &psiFirst);

					IShellItem* psiParent = nullptr;
					if (psiFirst) {
						psiFirst->GetParent(&psiParent);
						psiFirst->Release();
					}

					LPWSTR pszDir = nullptr;
					if (psiParent) {
						psiParent->GetDisplayName(SIGDN_FILESYSPATH, &pszDir);
						psiParent->Release();
					}

					if (pszDir) {
						WCHAR* buf = lpofn->lpstrFile;
						DWORD maxFile = lpofn->nMaxFile;
						size_t dirLen = wcslen(pszDir);

						// Write directory
						wcsncpy(buf, pszDir, maxFile);
						size_t pos = dirLen;
						buf[pos++] = L'\0';

						lpofn->nFileOffset = (WORD)pos;

						for (DWORD i = 0; i < count && pos < maxFile - 2; i++) {
							IShellItem* psi = nullptr;
							pResults->GetItemAt(i, &psi);

							if (psi) {
								LPWSTR pszName = nullptr;
								psi->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &pszName);

								if (pszName) {
									size_t nameLen = wcslen(pszName);

									if (pos + nameLen + 1 < maxFile - 1) {
										wcscpy(buf + pos, pszName);
										pos += nameLen;
										buf[pos++] = L'\0';
									}

									CoTaskMemFree(pszName);
								}

								psi->Release();
							}
						}
						buf[pos] = L'\0'; // final double-null

						CoTaskMemFree(pszDir);
						result = TRUE;
					}
				}

				pResults->Release();
			}
		}
		else {
			// Single result (or Save dialog)
			IShellItem* psi = nullptr;
			hr = pfd->GetResult(&psi);

			if (SUCCEEDED(hr)) {
				LPWSTR pszPath = nullptr;
				hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);

				if (SUCCEEDED(hr)) {
					wcsncpy(lpofn->lpstrFile, pszPath, lpofn->nMaxFile);
					lpofn->lpstrFile[lpofn->nMaxFile - 1] = L'\0';

					LPCWSTR pSlash = wcsrchr(pszPath, L'\\');
					lpofn->nFileOffset = pSlash ? (WORD)(pSlash - pszPath + 1) : 0;

					LPCWSTR pDot = wcsrchr(pszPath, L'.');

					lpofn->nFileExtension = pDot ? (WORD)(pDot - pszPath + 1) : 0;

					CoTaskMemFree(pszPath);
					result = TRUE;
				}

				// Write file title if requested
				if (result && lpofn->lpstrFileTitle && lpofn->nMaxFileTitle > 0) {
					LPWSTR pszName = nullptr;
					hr = psi->GetDisplayName(SIGDN_NORMALDISPLAY, &pszName);

					if (SUCCEEDED(hr)) {
						wcsncpy(lpofn->lpstrFileTitle, pszName, lpofn->nMaxFileTitle);

						lpofn->lpstrFileTitle[lpofn->nMaxFileTitle - 1] = L'\0';
						CoTaskMemFree(pszName);
					}
				}

				psi->Release();
			}
		}

		// Get the selected filter index
		if (filterCount > 0) {
			UINT idx = 0;

			if (SUCCEEDED(pfd->GetFileTypeIndex(&idx)))
				lpofn->nFilterIndex = idx;
		}

		// Read back OFN_READONLY checkbox state
		if (lpofn->Flags & OFN_READONLY) {
			IFileDialogCustomize* pCust = nullptr;

			if (SUCCEEDED(pfd->QueryInterface(IID_PPV_ARGS(&pCust)))) {
				DWORD dwItem = FALSE;

				if (SUCCEEDED(pCust->GetSelectedControlItem(OPENCHOICES, &dwItem))) {
					if (dwItem == OPEN_AS_READONLY)
						lpofn->Flags |= OFN_READONLY;
					else
						lpofn->Flags &= ~OFN_READONLY;
				}
				pCust->Release();
			}
		}
	}
	else {
		// User cancelled or error
		result = FALSE;
	}

	if (filterSpecs)
		CoTaskMemFree(filterSpecs);

	pfd->Release();
	if (weInitCom) CoUninitialize();

	if (!result && hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
		SetLastError(0);
		CommDlgExtendedError(); // clear
	}

	return result;
}

BOOL WINAPI GetOpenFileNameW_Hook(LPOPENFILENAMEW lpofn) {
	if (!settings.modernFileDialog)
		return GetOpenFileNameW_Original(lpofn);

	if (FLAG(lpofn->Flags, OFN_ENABLEHOOK))
		return GetOpenFileNameW_Original(lpofn);

	BOOL ret = ModernFileDialog_Impl(lpofn, TRUE);
	if (!ret && GetLastError() != 0)
		return GetOpenFileNameW_Original(lpofn);

	return ret;
}

BOOL WINAPI GetSaveFileNameW_Hook(LPOPENFILENAMEW lpofn) {
	if (!settings.modernFileDialog)
		return GetSaveFileNameW_Original(lpofn);

	if (FLAG(lpofn->Flags, OFN_ENABLEHOOK))
		return GetSaveFileNameW_Original(lpofn);

	BOOL ret = ModernFileDialog_Impl(lpofn, FALSE);
	if (!ret && GetLastError() != 0)
		return GetSaveFileNameW_Original(lpofn);

	return ret;
}

// --- ANSI helpers ---

static LPWSTR MultiByteToWideAlloc(LPCSTR str, int codePage = CP_ACP) {
	if (!str) return nullptr;
	int len = MultiByteToWideChar(codePage, 0, str, -1, nullptr, 0);

	if (len <= 0) return nullptr;
	LPWSTR buf = (LPWSTR)CoTaskMemAlloc(len * sizeof(WCHAR));

	if (buf) MultiByteToWideChar(codePage, 0, str, -1, buf, len);

	return buf;
}

// Convert double-null-terminated ANSI filter to wide
static LPWSTR FilterAtoW(LPCSTR filter) {
	if (!filter || !*filter) return nullptr;
	// Find total length including double null
	LPCSTR p = filter;
	while (*p)
	{
		p += strlen(p) + 1;
	}

	size_t totalBytes = (p - filter) + 1; // include final null
	int wlen = MultiByteToWideChar(CP_ACP, 0, filter, (int)totalBytes, nullptr, 0);

	if (wlen <= 0) return nullptr;

	LPWSTR buf = (LPWSTR)CoTaskMemAlloc(wlen * sizeof(WCHAR));

	if (buf)
		MultiByteToWideChar(CP_ACP, 0, filter, (int)totalBytes, buf, wlen);

	return buf;
}

BOOL WINAPI GetOpenFileNameA_Hook(LPOPENFILENAMEA lpofnA) {
	if (!settings.modernFileDialog)
		return GetOpenFileNameA_Original(lpofnA);

	if (FLAG(lpofnA->Flags, OFN_ENABLEHOOK))
		return GetOpenFileNameA_Original(lpofnA);

	// Build a wide OPENFILENAME
	OPENFILENAMEW ofnW = {};
	ofnW.lStructSize = sizeof(ofnW);
	ofnW.hwndOwner = lpofnA->hwndOwner;
	ofnW.Flags = lpofnA->Flags;
	ofnW.nFilterIndex = lpofnA->nFilterIndex;

	// Allocate wide file buffer
	DWORD nMaxFile = lpofnA->nMaxFile > 0 ? lpofnA->nMaxFile : MAX_PATH;
	WCHAR* wideFileBuf = (WCHAR*)CoTaskMemAlloc(nMaxFile * sizeof(WCHAR));

	if (!wideFileBuf)
		return GetOpenFileNameA_Original(lpofnA);

	wideFileBuf[0] = L'\0';

	if (lpofnA->lpstrFile && lpofnA->lpstrFile[0])
		MultiByteToWideChar(CP_ACP, 0, lpofnA->lpstrFile, -1, wideFileBuf, nMaxFile);

	ofnW.lpstrFile = wideFileBuf;
	ofnW.nMaxFile = nMaxFile;

	LPWSTR wideFilter = FilterAtoW(lpofnA->lpstrFilter);
	ofnW.lpstrFilter = wideFilter;

	LPWSTR wideTitle = MultiByteToWideAlloc(lpofnA->lpstrTitle);
	ofnW.lpstrTitle = wideTitle;

	LPWSTR wideInitDir = MultiByteToWideAlloc(lpofnA->lpstrInitialDir);
	ofnW.lpstrInitialDir = wideInitDir;

	LPWSTR wideDefExt = MultiByteToWideAlloc(lpofnA->lpstrDefExt);
	ofnW.lpstrDefExt = wideDefExt;

	WCHAR wideFileTitle[MAX_PATH] = {};
	if (lpofnA->lpstrFileTitle) {
		ofnW.lpstrFileTitle = wideFileTitle;
		ofnW.nMaxFileTitle = MAX_PATH;
	}

	BOOL ret = ModernFileDialog_Impl(&ofnW, TRUE);

	if (ret) {
		// Convert result back to ANSI
		if (lpofnA->Flags & OFN_ALLOWMULTISELECT) {
			// Find total length of double-null-terminated wide result
			WCHAR* p = wideFileBuf;
			while (*p)
			{
				p += wcslen(p) + 1;
			}

			size_t totalWChars = (p - wideFileBuf) + 1;

			WideCharToMultiByte(CP_ACP, 0, wideFileBuf, (int)totalWChars,
				lpofnA->lpstrFile, lpofnA->nMaxFile, NULL, NULL);
		}
		else {
			WideCharToMultiByte(CP_ACP, 0, wideFileBuf, -1,
				lpofnA->lpstrFile, lpofnA->nMaxFile, NULL, NULL);
		}
		lpofnA->nFileOffset = ofnW.nFileOffset;
		lpofnA->nFileExtension = ofnW.nFileExtension;
		lpofnA->nFilterIndex = ofnW.nFilterIndex;

		if (lpofnA->lpstrFileTitle && ofnW.lpstrFileTitle) {
			WideCharToMultiByte(CP_ACP, 0, ofnW.lpstrFileTitle, -1,
				lpofnA->lpstrFileTitle, lpofnA->nMaxFileTitle, NULL, NULL);
		}
	}

	CoTaskMemFree(wideFileBuf);
	if (wideFilter) CoTaskMemFree(wideFilter);
	if (wideTitle) CoTaskMemFree((void*)wideTitle);
	if (wideInitDir) CoTaskMemFree((void*)wideInitDir);
	if (wideDefExt) CoTaskMemFree((void*)wideDefExt);

	if (!ret && GetLastError() != 0)
		return GetOpenFileNameA_Original(lpofnA);

	return ret;
}

BOOL WINAPI GetSaveFileNameA_Hook(LPOPENFILENAMEA lpofnA) {
	if (!settings.modernFileDialog)
		return GetSaveFileNameA_Original(lpofnA);


	if (FLAG(lpofnA->Flags, OFN_ENABLEHOOK))
		return GetSaveFileNameA_Original(lpofnA);

	OPENFILENAMEW ofnW = {};
	ofnW.lStructSize = sizeof(ofnW);
	ofnW.hwndOwner = lpofnA->hwndOwner;
	ofnW.Flags = lpofnA->Flags;
	ofnW.nFilterIndex = lpofnA->nFilterIndex;

	DWORD nMaxFile = lpofnA->nMaxFile > 0 ? lpofnA->nMaxFile : MAX_PATH;
	WCHAR* wideFileBuf = (WCHAR*)CoTaskMemAlloc(nMaxFile * sizeof(WCHAR));

	if (!wideFileBuf)
		return GetSaveFileNameA_Original(lpofnA);

	wideFileBuf[0] = L'\0';
	if (lpofnA->lpstrFile && lpofnA->lpstrFile[0])
		MultiByteToWideChar(CP_ACP, 0, lpofnA->lpstrFile, -1, wideFileBuf, nMaxFile);

	ofnW.lpstrFile = wideFileBuf;
	ofnW.nMaxFile = nMaxFile;

	LPWSTR wideFilter = FilterAtoW(lpofnA->lpstrFilter);
	ofnW.lpstrFilter = wideFilter;

	LPWSTR wideTitle = MultiByteToWideAlloc(lpofnA->lpstrTitle);
	ofnW.lpstrTitle = wideTitle;

	LPWSTR wideInitDir = MultiByteToWideAlloc(lpofnA->lpstrInitialDir);
	ofnW.lpstrInitialDir = wideInitDir;

	LPWSTR wideDefExt = MultiByteToWideAlloc(lpofnA->lpstrDefExt);
	ofnW.lpstrDefExt = wideDefExt;

	WCHAR wideFileTitle[MAX_PATH] = {};
	if (lpofnA->lpstrFileTitle) {
		ofnW.lpstrFileTitle = wideFileTitle;
		ofnW.nMaxFileTitle = MAX_PATH;
	}

	BOOL ret = ModernFileDialog_Impl(&ofnW, FALSE);

	if (ret) {
		WideCharToMultiByte(CP_ACP, 0, wideFileBuf, -1,
			lpofnA->lpstrFile, lpofnA->nMaxFile, NULL, NULL);

		lpofnA->nFileOffset = ofnW.nFileOffset;
		lpofnA->nFileExtension = ofnW.nFileExtension;
		lpofnA->nFilterIndex = ofnW.nFilterIndex;

		if (lpofnA->lpstrFileTitle && ofnW.lpstrFileTitle) {
			WideCharToMultiByte(CP_ACP, 0, ofnW.lpstrFileTitle, -1,
				lpofnA->lpstrFileTitle, lpofnA->nMaxFileTitle, NULL, NULL);
		}
	}

	CoTaskMemFree(wideFileBuf);

	if (wideFilter) CoTaskMemFree(wideFilter);
	if (wideTitle) CoTaskMemFree((void*)wideTitle);
	if (wideInitDir) CoTaskMemFree((void*)wideInitDir);
	if (wideDefExt) CoTaskMemFree((void*)wideDefExt);

	if (!ret && GetLastError() != 0)
		return GetSaveFileNameA_Original(lpofnA);

	return ret;
}

// --- MessageBoxA / MessageBoxIndirectA → TaskDialog (ANSI wrappers) ---

int WINAPI MessageBoxA_Hook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
	if (!settings.messageTaskDlg)
		return MessageBoxA_Original(hWnd, lpText, lpCaption, uType);

	int lenText = lpText ? MultiByteToWideChar(CP_ACP, 0, lpText, -1, nullptr, 0) : 0;
	int lenCap = lpCaption ? MultiByteToWideChar(CP_ACP, 0, lpCaption, -1, nullptr, 0) : 0;

	// Safety: fall back for very long strings to avoid stack overflow
	if (lenText > 4096 || lenCap > 1024)
		return MessageBoxA_Original(hWnd, lpText, lpCaption, uType);

	WCHAR* wText = lenText > 0 ? (WCHAR*)_alloca(lenText * sizeof(WCHAR)) : nullptr;
	WCHAR* wCap = lenCap > 0 ? (WCHAR*)_alloca(lenCap * sizeof(WCHAR)) : nullptr;

	if (wText) MultiByteToWideChar(CP_ACP, 0, lpText, -1, wText, lenText);
	if (wCap) MultiByteToWideChar(CP_ACP, 0, lpCaption, -1, wCap, lenCap);

	return MessageBoxW_Hook(hWnd, wText, wCap, uType);
}

int WINAPI MessageBoxIndirectA_Hook(const MSGBOXPARAMSA* lpmbpA) {
	if (!settings.messageTaskDlg)
		return MessageBoxIndirectA_Original(lpmbpA);

	MSGBOXPARAMSW mbpW = {};
	mbpW.cbSize = sizeof(mbpW);
	mbpW.hwndOwner = lpmbpA->hwndOwner;
	mbpW.hInstance = lpmbpA->hInstance;
	mbpW.dwStyle = lpmbpA->dwStyle;
	mbpW.dwContextHelpId = lpmbpA->dwContextHelpId;
	mbpW.lpfnMsgBoxCallback = lpmbpA->lpfnMsgBoxCallback;
	mbpW.dwLanguageId = lpmbpA->dwLanguageId;

	int lenText = lpmbpA->lpszText ? MultiByteToWideChar(CP_ACP, 0, lpmbpA->lpszText, -1, nullptr, 0) : 0;
	int lenCap = lpmbpA->lpszCaption ? MultiByteToWideChar(CP_ACP, 0, lpmbpA->lpszCaption, -1, nullptr, 0) : 0;

	WCHAR* wText = lenText > 0 ? (WCHAR*)_malloca(lenText * sizeof(WCHAR)) : nullptr;
	WCHAR* wCap = lenCap > 0 ? (WCHAR*)_malloca(lenCap * sizeof(WCHAR)) : nullptr;

	if (wText) MultiByteToWideChar(CP_ACP, 0, lpmbpA->lpszText, -1, wText, lenText);
	if (wCap) MultiByteToWideChar(CP_ACP, 0, lpmbpA->lpszCaption, -1, wCap, lenCap);

	mbpW.lpszText = wText;
	mbpW.lpszCaption = wCap;

	// Icon: if it's a resource ID (MAKEINTRESOURCE), pass through; if string, convert
	if (lpmbpA->lpszIcon && !IS_INTRESOURCE(lpmbpA->lpszIcon)) {
		int lenIcon = MultiByteToWideChar(CP_ACP, 0, lpmbpA->lpszIcon, -1, nullptr, 0);
		WCHAR* wIcon = (WCHAR*)_alloca(lenIcon * sizeof(WCHAR));

		MultiByteToWideChar(CP_ACP, 0, lpmbpA->lpszIcon, -1, wIcon, lenIcon);
		mbpW.lpszIcon = wIcon;
	}
	else {
		mbpW.lpszIcon = (LPCWSTR)lpmbpA->lpszIcon;
	}

	return MessageBoxIndirectW_Hook(&mbpW);
}

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

	MessageBoxA_t pMessageBoxA =
		(MessageBoxA_t)GetProcAddress(hUser32, "MessageBoxA");
	MessageBoxIndirectA_t pMessageBoxIndirectA =
		(MessageBoxIndirectA_t)GetProcAddress(hUser32, "MessageBoxIndirectA");

	if (pMessageBoxA) {
		Wh_SetFunctionHook((void*)pMessageBoxA,
			(void*)MessageBoxA_Hook,
			(void**)&MessageBoxA_Original);
	}
	if (pMessageBoxIndirectA) {
		Wh_SetFunctionHook((void*)pMessageBoxIndirectA,
			(void*)MessageBoxIndirectA_Hook,
			(void**)&MessageBoxIndirectA_Original);
	}

	HMODULE hShell32 = LoadLibraryExW(L"Shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

	SHBrowseForFolderW_t pSHBrowseForFolderW = (SHBrowseForFolderW_t)GetProcAddress(hShell32, "SHBrowseForFolderW");

	Wh_SetFunctionHook((void*)pSHBrowseForFolderW,
		(void*)SHBrowseForFolderW_Hook,
		(void**)&SHBrowseForFolderW_Original);

	HMODULE hComDlg32 = LoadLibraryExW(L"Comdlg32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hComDlg32) {
		GetOpenFileNameW_t pGetOpenFileNameW =
			(GetOpenFileNameW_t)GetProcAddress(hComDlg32, "GetOpenFileNameW");

		GetSaveFileNameW_t pGetSaveFileNameW =
			(GetSaveFileNameW_t)GetProcAddress(hComDlg32, "GetSaveFileNameW");

		if (pGetOpenFileNameW) {
			Wh_SetFunctionHook((void*)pGetOpenFileNameW,
				(void*)GetOpenFileNameW_Hook,
				(void**)&GetOpenFileNameW_Original);
		}

		if (pGetSaveFileNameW) {
			Wh_SetFunctionHook((void*)pGetSaveFileNameW,
				(void*)GetSaveFileNameW_Hook,
				(void**)&GetSaveFileNameW_Original);
		}

		GetOpenFileNameA_t pGetOpenFileNameA =
			(GetOpenFileNameA_t)GetProcAddress(hComDlg32, "GetOpenFileNameA");

		GetSaveFileNameA_t pGetSaveFileNameA =
			(GetSaveFileNameA_t)GetProcAddress(hComDlg32, "GetSaveFileNameA");

		if (pGetOpenFileNameA) {
			Wh_SetFunctionHook((void*)pGetOpenFileNameA,
				(void*)GetOpenFileNameA_Hook,
				(void**)&GetOpenFileNameA_Original);
		}

		if (pGetSaveFileNameA) {
			Wh_SetFunctionHook((void*)pGetSaveFileNameA,
				(void*)GetSaveFileNameA_Hook,
				(void**)&GetSaveFileNameA_Original);
		}
	}

	// PickIconDlg_t pPickIconDlg = (PickIconDlg_t)GetProcAddress(hShell32, "PickIconDlg");

	// Wh_SetFunctionHook((void*)pPickIconDlg,
	//  (void*)PickIconDlg_Hook,
	//  (void**)&PickIconDlg_Original);

	HMODULE hComDlg = LoadLibraryExW(L"Comdlg32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hComDlg) {
		ChooseColorW_t pChooseColorW = (ChooseColorW_t)GetProcAddress(hComDlg, "ChooseColorW");

		if (pChooseColorW) {
			Wh_SetFunctionHook((void*)pChooseColorW, (void*)ChooseColorW_Hook, (void**)&ChooseColorW_Original);
		}

		ChooseFontW_t pChooseFontW = (ChooseFontW_t)GetProcAddress(hComDlg, "ChooseFontW");

		if (pChooseFontW) {
			Wh_SetFunctionHook((void*)pChooseFontW, (void*)ChooseFontW_Hook, (void**)&ChooseFontW_Original);
		}
	}

	return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
	Wh_Log(L"Uninit");

	std::lock_guard<std::mutex> lock(vDlgsMutex);
	for (HWND& hwnd : vDlgs)
		SendMessageTimeoutW(hwnd, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 2500, NULL);

	if (g_cpClassAtom && nColorDlgCount == 0)
		UnregisterClassW((LPWSTR)MAKEINTATOM(g_cpClassAtom), GetModuleHandleW(NULL));

	if (g_fpClassAtom && nFontDlgCount == 0)
		UnregisterClassW((LPWSTR)MAKEINTATOM(g_fpClassAtom), GetModuleHandleW(NULL));
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
	Wh_Log(L"SettingsChanged");

	LoadSettings();
}