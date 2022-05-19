// ==WindhawkMod==
// @id           taskbar-volume-control
// @name         Taskbar Volume Control
// @description  Control the system volume by scrolling over the taskbar
// @version      1.0
// @author       m417z
// @github       https://github.com/m417z
// @twitter      https://twitter.com/m417z
// @homepage     https://m417z.com/
// @include      explorer.exe
// @compilerOptions -lcomctl32 -ldwmapi -lole32 -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Volume Control
Control the system volume by scrolling over the taskbar.

![demonstration](https://i.imgur.com/S55wMVn.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- volumeIndicator: modern
  $name: Volume control indicator
  $options:
  - modern: Modern
  - classic: Classic
  - tooltip: Tooltip
  - none: None
- scrollArea: taskbar
  $name: Scroll area
  $options:
  - taskbar: The taskbar
  - notification_area: The notification area
- noAutomaticMuteToggle: false
  $name: No automatic mute toggle
  $description: >-
    By default, the output device is muted once the volume reaches zero, and is
    unmuted on any change to a non-zero volume. Enabling this option turns off
    this functionality, such that the device mute status is not changed.
- volumeChangeStep: 0
  $name: Volume change step
  $description: >-
    Allows to configure the volume change that will occur with each notch of mouse
    wheel movement. If set to zero, the default system value, 2, is used. Note:
    this option has no effect for the classic volume control indicator.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <dwmapi.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <objbase.h>
#include <windowsx.h>

#include <unordered_set>

enum {
	VOLUME_INDICATOR_NONE,
	VOLUME_INDICATOR_TOOLTIP,
	VOLUME_INDICATOR_CLASSIC,
	VOLUME_INDICATOR_MODERN,
};

enum {
	SCROLL_AREA_TASKBAR,
	SCROLL_AREA_NOTIFICATION_AREA,
};

struct {
    int volumeIndicator;
    int scrollArea;
    bool noAutomaticMuteToggle;
    int volumeChangeStep;
} g_settings;

std::unordered_set<HWND> g_secondaryTaskbarWindows;

#define WIN_VERSION_UNSUPPORTED    (-1)
#define WIN_VERSION_7              0
#define WIN_VERSION_8              1
#define WIN_VERSION_81             2
#define WIN_VERSION_811            3
#define WIN_VERSION_10_T1          4  // 1507
#define WIN_VERSION_10_T2          5  // 1511
#define WIN_VERSION_10_R1          6  // 1607
#define WIN_VERSION_10_R2          7  // 1703
#define WIN_VERSION_10_R3          8  // 1709
#define WIN_VERSION_10_R4          9  // 1803
#define WIN_VERSION_10_R5          10 // 1809
#define WIN_VERSION_10_19H1        11 // 1903, 1909
#define WIN_VERSION_10_20H1        12 // 2004, 20H2, 21H1, 21H2
#define WIN_VERSION_SERVER_2022    13 // Server 2022
#define WIN_VERSION_11_21H2        14

#define MSG_DLL_CALLFUNC             1
#define MSG_DLL_MOUSE_HOOK_WND_IDENT 2

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) -> return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI -> return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) -> return_t
#endif

typedef struct _mouse_hook_wnd_ident_param {
	// In
	const POINT *ppt;
	HWND hMaybeTransWnd;

	// Out
	int nMaybeTransWndIdent;
	HWND hNonTransWnd;
} MOUSE_HOOK_WND_IDENT_PARAM;

static int nWinVersion;
static HWND hTaskbarWnd;
static DWORD dwTaskbarThreadId;

static UINT uTweakerMsg = RegisterWindowMessage(
	L"Windhawk_uTweakerMsg_taskbar-volume-control");

#pragma region functions

VS_FIXEDFILEINFO *GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen)
{
	HRSRC hResource;
	HGLOBAL hGlobal;
	void *pData;
	void *pFixedFileInfo;
	UINT uPtrLen;

	pFixedFileInfo = NULL;
	uPtrLen = 0;

	hResource = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if(hResource != NULL)
	{
		hGlobal = LoadResource(hModule, hResource);
		if(hGlobal != NULL)
		{
			pData = LockResource(hGlobal);
			if(pData != NULL)
			{
				if(!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) || uPtrLen == 0)
				{
					pFixedFileInfo = NULL;
					uPtrLen = 0;
				}
			}
		}
	}

	if(puPtrLen)
		*puPtrLen = uPtrLen;

	return (VS_FIXEDFILEINFO *)pFixedFileInfo;
}

BOOL WindowsVersionInit()
{
	nWinVersion = WIN_VERSION_UNSUPPORTED;

	VS_FIXEDFILEINFO *pFixedFileInfo = GetModuleVersionInfo(NULL, NULL);
	if(!pFixedFileInfo)
		return FALSE;

	WORD nMajor = HIWORD(pFixedFileInfo->dwFileVersionMS);
	WORD nMinor = LOWORD(pFixedFileInfo->dwFileVersionMS);
	WORD nBuild = HIWORD(pFixedFileInfo->dwFileVersionLS);
	WORD nQFE = LOWORD(pFixedFileInfo->dwFileVersionLS);

	switch(nMajor)
	{
	case 6:
		switch(nMinor)
		{
		case 1:
			nWinVersion = WIN_VERSION_7;
			break;

		case 2:
			nWinVersion = WIN_VERSION_8;
			break;

		case 3:
			if(nQFE < 17000)
				nWinVersion = WIN_VERSION_81;
			else
				nWinVersion = WIN_VERSION_811;
			break;

		case 4:
			nWinVersion = WIN_VERSION_10_T1;
			break;
		}
		break;

	case 10:
		if(nBuild <= 10240)
			nWinVersion = WIN_VERSION_10_T1;
		else if(nBuild <= 10586)
			nWinVersion = WIN_VERSION_10_T2;
		else if(nBuild <= 14393)
			nWinVersion = WIN_VERSION_10_R1;
		else if(nBuild <= 15063)
			nWinVersion = WIN_VERSION_10_R2;
		else if(nBuild <= 16299)
			nWinVersion = WIN_VERSION_10_R3;
		else if(nBuild <= 17134)
			nWinVersion = WIN_VERSION_10_R4;
		else if(nBuild <= 17763)
			nWinVersion = WIN_VERSION_10_R5;
		else if(nBuild <= 18362)
			nWinVersion = WIN_VERSION_10_19H1;
		else if(nBuild <= 19041)
			nWinVersion = WIN_VERSION_10_20H1;
		else if(nBuild <= 20348)
			nWinVersion = WIN_VERSION_SERVER_2022;
		else
			nWinVersion = WIN_VERSION_11_21H2;
		break;
	}

	if(nWinVersion == WIN_VERSION_UNSUPPORTED)
		return FALSE;

	return TRUE;
}

enum
{
	TASKBAR_WINDOW_UNKNOWN,

	TASKBAR_WINDOW_NOTIFY,
	TASKBAR_WINDOW_TASKBAR,
};

int IdentifyTaskbarWindow(HWND hWnd)
{
	HWND hTrayNotifyWnd = FindWindowEx(hTaskbarWnd, NULL, L"TrayNotifyWnd", NULL);
	if(hWnd == hTrayNotifyWnd || IsChild(hTrayNotifyWnd, hWnd))
		return TASKBAR_WINDOW_NOTIFY;

	if(hWnd == hTaskbarWnd || IsChild(hTaskbarWnd, hWnd))
		return TASKBAR_WINDOW_TASKBAR;

	WCHAR szClassName[32];
	if(GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
		return TASKBAR_WINDOW_UNKNOWN;

	if(wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0)
		return TASKBAR_WINDOW_TASKBAR;

	if(nWinVersion >= WIN_VERSION_11_21H2)
	{
		if(wcsicmp(szClassName, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0)
		{
			HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
			WCHAR szParentClassName[32];
			if(GetClassName(hParentWnd, szParentClassName, ARRAYSIZE(szParentClassName)) &&
				wcsicmp(szParentClassName, L"Shell_SecondaryTrayWnd") == 0)
			{
				RECT rc, rcParent;
				GetWindowRect(hWnd, &rc);
				GetWindowRect(hParentWnd, &rcParent);
				if(rc.left != rcParent.left ||
					rc.top != rcParent.top ||
					rc.right != rcParent.right ||
					rc.bottom != rcParent.bottom)
				{
					return TASKBAR_WINDOW_NOTIFY;
				}
			}
		}
	}
	else if(nWinVersion >= WIN_VERSION_10_R1)
	{
		if(wcsicmp(szClassName, L"ClockButton") == 0)
		{
			HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
			WCHAR szParentClassName[32];
			if(GetClassName(hParentWnd, szParentClassName, ARRAYSIZE(szParentClassName)) &&
				wcsicmp(szParentClassName, L"Shell_SecondaryTrayWnd") == 0)
			{
				return TASKBAR_WINDOW_NOTIFY;
			}
		}
	}

	WCHAR szRootClassName[32];
	if(GetClassName(GetAncestor(hWnd, GA_ROOT), szRootClassName, ARRAYSIZE(szRootClassName)) &&
		wcsicmp(szRootClassName, L"Shell_SecondaryTrayWnd") == 0)
	{
		return TASKBAR_WINDOW_TASKBAR;
	}

	return TASKBAR_WINDOW_UNKNOWN;
}

#pragma endregion // functions

#pragma region sndvol

void SndVolInit();
void SndVolUninit();

BOOL OpenScrollSndVol(WPARAM wParam, LPARAM lMousePosParam);
BOOL IsSndVolOpen();
BOOL ScrollSndVol(WPARAM wParam, LPARAM lMousePosParam);
void SetSndVolTimer();
void ResetSndVolTimer();
void KillSndVolTimer();
void CleanupSndVol();

// Mouse hook functions
void OnSndVolMouseMove_MouseHook(POINT pt);
void OnSndVolMouseClick_MouseHook(POINT pt);
void OnSndVolMouseWheel_MouseHook(POINT pt);

// Tooltip indicator functions
void OnSndVolTooltipTimer();

// Other functions
BOOL GetSndVolTrayIconRect(RECT *prc);
BOOL IsDefaultAudioEndpointAvailable();
BOOL IsVolMuted(BOOL *pbMuted);
BOOL IsVolMutedAndNotZero(BOOL *pbResult);
BOOL ToggleVolMuted();
BOOL AddMasterVolumeLevelScalar(float fMasterVolumeAdd);

static BOOL AdjustVolumeLevelWithMouseWheel(int nWheelDelta);
static BOOL OpenScrollSndVolInternal(WPARAM wParam, LPARAM lMousePosParam, HWND hVolumeAppWnd, BOOL *pbOpened);
static BOOL ValidateSndVolProcess();
static BOOL ValidateSndVolWnd();
static void CALLBACK CloseSndVolTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static HWND GetSndVolDlg(HWND hVolumeAppWnd);
static BOOL CALLBACK EnumThreadFindSndVolWnd(HWND hWnd, LPARAM lParam);
static BOOL IsSndVolWndInitialized(HWND hWnd);
static BOOL MoveSndVolCenterMouse(HWND hWnd);

// Mouse hook functions
static void OnSndVolMouseLeaveClose();
static void OnSndVolMouseClick();
static void OnSndVolMouseWheel();

// Tooltip indicator functions
static BOOL ShowSndVolTooltip();
static BOOL HideSndVolTooltip();
static int GetSndVolTrayIconIndex(HWND *phTrayToolbarWnd);

// Modern indicator functions
static BOOL CanUseModernIndicator();
static BOOL ShowSndVolModernIndicator();
static BOOL HideSndVolModernIndicator();
static void EndSndVolModernIndicatorSession();
static BOOL IsCursorUnderSndVolModernIndicatorWnd();
static HWND GetOpenSndVolModernIndicatorWnd();
static HWND GetSndVolTrayControlWnd();
static BOOL CALLBACK EnumThreadFindSndVolTrayControlWnd(HWND hWnd, LPARAM lParam);

static IMMDeviceEnumerator *pDeviceEnumerator;
static HANDLE hSndVolProcess;
static HWND hSndVolWnd;
static UINT_PTR nCloseSndVolTimer;
static int nCloseSndVolTimerCount;
static volatile BOOL bCloseOnMouseLeave;
static BOOL bTooltipTimerOn;
static HWND hSndVolModernPreviousForegroundWnd;
static BOOL bSndVolModernLaunched;
static BOOL bSndVolModernAppeared;

const static GUID XIID_IMMDeviceEnumerator = { 0xA95664D2, 0x9614, 0x4F35, { 0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6 } };
const static GUID XIID_MMDeviceEnumerator = { 0xBCDE0395, 0xE52F, 0x467C, { 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E } };
const static GUID XIID_IAudioEndpointVolume = { 0x5CDF2C82, 0x841E, 0x4546, { 0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A } };

void SndVolInit()
{
	HRESULT hr = CoCreateInstance(XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, XIID_IMMDeviceEnumerator, (LPVOID *)&pDeviceEnumerator);
	if(FAILED(hr))
		pDeviceEnumerator = NULL;
}

void SndVolUninit()
{
	if(pDeviceEnumerator)
	{
		pDeviceEnumerator->Release();
		pDeviceEnumerator = NULL;
	}
}

BOOL OpenScrollSndVol(WPARAM wParam, LPARAM lMousePosParam)
{
	HANDLE hMutex;
	HWND hVolumeAppWnd;
	DWORD dwProcessId;
	WCHAR szCommandLine[sizeof("SndVol.exe -f 4294967295")];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	if(g_settings.volumeIndicator == VOLUME_INDICATOR_NONE)
	{
		return AdjustVolumeLevelWithMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
	}
	else if(g_settings.volumeIndicator == VOLUME_INDICATOR_TOOLTIP)
	{
		if(!AdjustVolumeLevelWithMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)))
			return FALSE;

		ShowSndVolTooltip();
		return TRUE;
	}
	else if(CanUseModernIndicator())
	{
		if(!AdjustVolumeLevelWithMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)))
			return FALSE;

		ShowSndVolModernIndicator();
		return TRUE;
	}

	if(!IsDefaultAudioEndpointAvailable())
		return FALSE;

	if(ValidateSndVolProcess())
	{
		if(WaitForInputIdle(hSndVolProcess, 0) == 0) // If not initializing
		{
			if(ValidateSndVolWnd())
			{
				ScrollSndVol(wParam, lMousePosParam);

				return FALSE; // False because we didn't open it, it was open
			}
			else
			{
				hVolumeAppWnd = FindWindow(L"Windows Volume App Window", L"Windows Volume App Window");
				if(hVolumeAppWnd)
				{
					GetWindowThreadProcessId(hVolumeAppWnd, &dwProcessId);

					if(GetProcessId(hSndVolProcess) == dwProcessId)
					{
						BOOL bOpened;
						if(OpenScrollSndVolInternal(wParam, lMousePosParam, hVolumeAppWnd, &bOpened))
							return bOpened;
					}
				}
			}
		}

		return FALSE;
	}

	hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"Windows Volume App Window");
	if(hMutex)
	{
		CloseHandle(hMutex);

		hVolumeAppWnd = FindWindow(L"Windows Volume App Window", L"Windows Volume App Window");
		if(hVolumeAppWnd)
		{
			GetWindowThreadProcessId(hVolumeAppWnd, &dwProcessId);

			hSndVolProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, dwProcessId);
			if(hSndVolProcess)
			{
				if(WaitForInputIdle(hSndVolProcess, 0) == 0) // if not initializing
				{
					if(ValidateSndVolWnd())
					{
						ScrollSndVol(wParam, lMousePosParam);

						return FALSE; // False because we didn't open it, it was open
					}
					else
					{
						BOOL bOpened;
						if(OpenScrollSndVolInternal(wParam, lMousePosParam, hVolumeAppWnd, &bOpened))
							return bOpened;
					}
				}
			}
		}

		return FALSE;
	}

	wsprintf(szCommandLine, L"SndVol.exe -f %u", (DWORD)lMousePosParam);

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if(!CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, ABOVE_NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED, NULL, NULL, &si, &pi))
		return FALSE;

	if(nWinVersion <= WIN_VERSION_7)
		SendMessage(hTaskbarWnd, WM_USER + 12, 0, 0); // Close start menu

	AllowSetForegroundWindow(pi.dwProcessId);
	ResumeThread(pi.hThread);

	CloseHandle(pi.hThread);
	hSndVolProcess = pi.hProcess;

	return TRUE;
}

BOOL IsSndVolOpen()
{
	return ValidateSndVolProcess() && ValidateSndVolWnd();
}

BOOL ScrollSndVol(WPARAM wParam, LPARAM lMousePosParam)
{
	GUITHREADINFO guithreadinfo;

	guithreadinfo.cbSize = sizeof(GUITHREADINFO);

	if(!GetGUIThreadInfo(GetWindowThreadProcessId(hSndVolWnd, NULL), &guithreadinfo))
		return FALSE;

	PostMessage(guithreadinfo.hwndFocus, WM_MOUSEWHEEL, wParam, lMousePosParam);
	return TRUE;
}

void SetSndVolTimer()
{
	nCloseSndVolTimer = SetTimer(NULL, nCloseSndVolTimer, 100, CloseSndVolTimerProc);
	nCloseSndVolTimerCount = 0;
}

void ResetSndVolTimer()
{
	if(nCloseSndVolTimer != 0)
		SetSndVolTimer();
}

void KillSndVolTimer()
{
	if(nCloseSndVolTimer != 0)
	{
		KillTimer(NULL, nCloseSndVolTimer);
		nCloseSndVolTimer = 0;
	}
}

void CleanupSndVol()
{
	KillSndVolTimer();

	if(hSndVolProcess)
	{
		CloseHandle(hSndVolProcess);
		hSndVolProcess = NULL;
		hSndVolWnd = NULL;
	}
}

static BOOL AdjustVolumeLevelWithMouseWheel(int nWheelDelta)
{
	int nStep = 2;
	if(g_settings.volumeChangeStep)
	{
		nStep = g_settings.volumeChangeStep;
	}

	return AddMasterVolumeLevelScalar((float)nWheelDelta * nStep * ((float)0.01 / 120));
}

static BOOL OpenScrollSndVolInternal(WPARAM wParam, LPARAM lMousePosParam, HWND hVolumeAppWnd, BOOL *pbOpened)
{
	HWND hSndVolDlg = GetSndVolDlg(hVolumeAppWnd);
	if(hSndVolDlg)
	{
		if(GetWindowTextLength(hSndVolDlg) == 0) // Volume control
		{
			if(IsSndVolWndInitialized(hSndVolDlg) && MoveSndVolCenterMouse(hSndVolDlg))
			{
				if(nWinVersion <= WIN_VERSION_7)
					SendMessage(hTaskbarWnd, WM_USER + 12, 0, 0); // Close start menu

				SetForegroundWindow(hVolumeAppWnd);
				PostMessage(hVolumeAppWnd, WM_USER + 35, 0, 0);

				*pbOpened = TRUE;
				return TRUE;
			}
		}
		else if(IsWindowVisible(hSndVolDlg)) // Another dialog, e.g. volume mixer
		{
			if(nWinVersion <= WIN_VERSION_7)
				SendMessage(hTaskbarWnd, WM_USER + 12, 0, 0); // Close start menu

			SetForegroundWindow(hVolumeAppWnd);
			PostMessage(hVolumeAppWnd, WM_USER + 35, 0, 0);

			*pbOpened = FALSE;
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL ValidateSndVolProcess()
{
	if(!hSndVolProcess)
		return FALSE;

	if(WaitForSingleObject(hSndVolProcess, 0) != WAIT_TIMEOUT)
	{
		CloseHandle(hSndVolProcess);
		hSndVolProcess = NULL;
		hSndVolWnd = NULL;

		return FALSE;
	}

	return TRUE;
}

static BOOL ValidateSndVolWnd()
{
	HWND hForegroundWnd;
	DWORD dwProcessId;
	WCHAR szClass[sizeof("#32770") + 1];

	hForegroundWnd = GetForegroundWindow();

	if(hSndVolWnd == hForegroundWnd)
		return TRUE;

	GetWindowThreadProcessId(hForegroundWnd, &dwProcessId);

	if(GetProcessId(hSndVolProcess) == dwProcessId)
	{
		GetClassName(hForegroundWnd, szClass, sizeof("#32770") + 1);

		if(lstrcmp(szClass, L"#32770") == 0)
		{
			hSndVolWnd = hForegroundWnd;
			bCloseOnMouseLeave = FALSE;

			return TRUE;
		}
	}

	hSndVolWnd = NULL;

	return FALSE;
}

static void CALLBACK CloseSndVolTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	POINT pt;
	RECT rc;

	if(g_settings.volumeIndicator == VOLUME_INDICATOR_TOOLTIP)
	{
		nCloseSndVolTimerCount++;
		if(nCloseSndVolTimerCount < 10)
			return;

		HideSndVolTooltip();
	}
	else if(CanUseModernIndicator())
	{
		if(!bSndVolModernAppeared)
		{
			if(GetOpenSndVolModernIndicatorWnd())
			{
				bSndVolModernAppeared = TRUE;
				nCloseSndVolTimerCount = 1;
				return;
			}
			else
			{
				nCloseSndVolTimerCount++;
				if(nCloseSndVolTimerCount < 10)
					return;
			}
		}
		else if(GetOpenSndVolModernIndicatorWnd())
		{
			if(!IsCursorUnderSndVolModernIndicatorWnd())
				nCloseSndVolTimerCount++;
			else
				nCloseSndVolTimerCount = 0;

			if(nCloseSndVolTimerCount < 10)
				return;

			HideSndVolModernIndicator();
		}

		EndSndVolModernIndicatorSession();
	}
	else
	{
		if(ValidateSndVolProcess())
		{
			if(WaitForInputIdle(hSndVolProcess, 0) != 0)
				return;

			if(ValidateSndVolWnd())
			{
				nCloseSndVolTimerCount++;
				if(nCloseSndVolTimerCount < 10)
					return;

				GetCursorPos(&pt);
				GetWindowRect(hSndVolWnd, &rc);

				if(!PtInRect(&rc, pt))
					PostMessage(hSndVolWnd, WM_ACTIVATE, MAKEWPARAM(WA_INACTIVE, FALSE), (LPARAM)NULL);
				else
					bCloseOnMouseLeave = TRUE;
			}
		}
	}

	KillTimer(NULL, nCloseSndVolTimer);
	nCloseSndVolTimer = 0;
}

static HWND GetSndVolDlg(HWND hVolumeAppWnd)
{
	HWND hWnd = NULL;
	EnumThreadWindows(GetWindowThreadProcessId(hVolumeAppWnd, NULL), EnumThreadFindSndVolWnd, (LPARAM)&hWnd);
	return hWnd;
}

static BOOL CALLBACK EnumThreadFindSndVolWnd(HWND hWnd, LPARAM lParam)
{
	WCHAR szClass[16];

	GetClassName(hWnd, szClass, _countof(szClass));
	if(lstrcmp(szClass, L"#32770") == 0)
	{
		*(HWND *)lParam = hWnd;
		return FALSE;
	}

	return TRUE;
}

static BOOL IsSndVolWndInitialized(HWND hWnd)
{
	HWND hChildDlg;

	hChildDlg = FindWindowEx(hWnd, NULL, L"#32770", NULL);
	if(!hChildDlg)
		return FALSE;

	if(!(GetWindowLong(hChildDlg, GWL_STYLE) & WS_VISIBLE))
		return FALSE;

	return TRUE;
}

static BOOL MoveSndVolCenterMouse(HWND hWnd)
{
	NOTIFYICONIDENTIFIER notifyiconidentifier;
	BOOL bCompositionEnabled;
	POINT pt;
	SIZE size;
	RECT rc, rcExclude, rcInflate;
	int nInflate;

	ZeroMemory(&notifyiconidentifier, sizeof(NOTIFYICONIDENTIFIER));
	notifyiconidentifier.cbSize = sizeof(NOTIFYICONIDENTIFIER);
	memcpy(&notifyiconidentifier.guidItem, "\x73\xAE\x20\x78\xE3\x23\x29\x42\x82\xC1\xE4\x1C\xB6\x7D\x5B\x9C", sizeof(GUID));

	if(Shell_NotifyIconGetRect(&notifyiconidentifier, &rcExclude) != S_OK)
		SetRectEmpty(&rcExclude);

	GetCursorPos(&pt);
	GetWindowRect(hWnd, &rc);

	nInflate = 0;

	if(DwmIsCompositionEnabled(&bCompositionEnabled) == S_OK && bCompositionEnabled)
	{
		memcpy(&notifyiconidentifier.guidItem, "\x43\x65\x4B\x96\xAD\xBB\xEE\x44\x84\x8A\x3A\x95\xD8\x59\x51\xEA", sizeof(GUID));

		if(Shell_NotifyIconGetRect(&notifyiconidentifier, &rcInflate) == S_OK)
		{
			nInflate = rcInflate.bottom - rcInflate.top;
			InflateRect(&rc, nInflate, nInflate);
		}
	}

	size.cx = rc.right - rc.left;
	size.cy = rc.bottom - rc.top;

	if(!CalculatePopupWindowPosition(&pt, &size,
		TPM_CENTERALIGN | TPM_VCENTERALIGN | TPM_VERTICAL | TPM_WORKAREA, &rcExclude, &rc))
		return FALSE;

	SetWindowPos(hWnd, NULL, rc.left + nInflate, rc.top + nInflate,
		0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);

	return TRUE;
}

// Mouse hook functions

void OnSndVolMouseMove_MouseHook(POINT pt)
{
	HWND hWnd;

	if(bCloseOnMouseLeave)
	{
		hWnd = WindowFromPoint(pt);

		if(hWnd == hSndVolWnd || IsChild(hSndVolWnd, hWnd))
		{
			bCloseOnMouseLeave = FALSE;
			PostMessage(hTaskbarWnd, uTweakerMsg, (LPARAM)OnSndVolMouseLeaveClose, MSG_DLL_CALLFUNC);
		}
	}
}

void OnSndVolMouseClick_MouseHook(POINT pt)
{
	PostMessage(hTaskbarWnd, uTweakerMsg, (LPARAM)OnSndVolMouseClick, MSG_DLL_CALLFUNC);
}

void OnSndVolMouseWheel_MouseHook(POINT pt)
{
	PostMessage(hTaskbarWnd, uTweakerMsg, (LPARAM)OnSndVolMouseWheel, MSG_DLL_CALLFUNC);
}

static void OnSndVolMouseLeaveClose()
{
	SetSndVolTimer();
}

static void OnSndVolMouseClick()
{
	bCloseOnMouseLeave = FALSE;
	KillSndVolTimer();
}

static void OnSndVolMouseWheel()
{
	ResetSndVolTimer();
}

// Tooltip indicator functions

void OnSndVolTooltipTimer()
{
	HWND hTrayToolbarWnd;
	int index;
	HWND hTooltipWnd;

	if(!bTooltipTimerOn)
		return;

	bTooltipTimerOn = FALSE;

	index = GetSndVolTrayIconIndex(&hTrayToolbarWnd);
	if(index < 0)
		return;

	hTooltipWnd = (HWND)SendMessage(hTrayToolbarWnd, TB_GETTOOLTIPS, 0, 0);
	if(hTooltipWnd)
		ShowWindow(hTooltipWnd, SW_HIDE);
}

static BOOL ShowSndVolTooltip()
{
	HWND hTrayToolbarWnd;
	int index;

	index = GetSndVolTrayIconIndex(&hTrayToolbarWnd);
	if(index < 0)
		return FALSE;

	SendMessage(hTrayToolbarWnd, TB_SETHOTITEM2, -1, 0);
	SendMessage(hTrayToolbarWnd, TB_SETHOTITEM2, index, 0);

	// Show tooltip
	bTooltipTimerOn = TRUE;
	SetTimer(hTrayToolbarWnd, 0, 0, NULL);

	return TRUE;
}

static BOOL HideSndVolTooltip()
{
	HWND hTrayToolbarWnd;
	int index;

	index = GetSndVolTrayIconIndex(&hTrayToolbarWnd);
	if(index < 0)
		return FALSE;

	if(SendMessage(hTrayToolbarWnd, TB_GETHOTITEM, 0, 0) == index)
		SendMessage(hTrayToolbarWnd, TB_SETHOTITEM2, -1, 0);

	return TRUE;
}

static int GetSndVolTrayIconIndex(HWND *phTrayToolbarWnd)
{
	HWND hTrayNotifyWnd = FindWindowEx(hTaskbarWnd, NULL, L"TrayNotifyWnd", NULL);
	if(!hTrayNotifyWnd)
		return -1;

	HWND hSysPagerWnd = FindWindowEx(hTrayNotifyWnd, NULL, L"SysPager", NULL);
	if(!hSysPagerWnd)
		return -1;

	HWND hTrayToolbarWnd = FindWindowEx(hSysPagerWnd, NULL, L"ToolbarWindow32", NULL);
	if(!hTrayToolbarWnd)
		return -1;

	RECT rcSndVolIcon;
	if(!GetSndVolTrayIconRect(&rcSndVolIcon))
		return -1;

	POINT pt;
	pt.x = rcSndVolIcon.left + (rcSndVolIcon.right - rcSndVolIcon.left) / 2;
	pt.y = rcSndVolIcon.top + (rcSndVolIcon.bottom - rcSndVolIcon.top) / 2;

	MapWindowPoints(NULL, hTrayToolbarWnd, &pt, 1);

	int index = (int)SendMessage(hTrayToolbarWnd, TB_HITTEST, 0, (LPARAM)&pt);
	if(index >= 0 && phTrayToolbarWnd)
		*phTrayToolbarWnd = hTrayToolbarWnd;

	return index;
}

// Modern indicator functions

static BOOL CanUseModernIndicator()
{
	if(nWinVersion < WIN_VERSION_10_T1 || g_settings.volumeIndicator == VOLUME_INDICATOR_CLASSIC)
		return FALSE;

	DWORD dwEnabled = 1;
	DWORD dwValueSize = sizeof(dwEnabled);
	DWORD dwError = RegGetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\MTCUVC",
		L"EnableMTCUVC", RRF_RT_REG_DWORD, NULL, &dwEnabled, &dwValueSize);

	// We don't check dwError just like Microsoft doesn't at SndVolSSO.dll.

	return dwEnabled != 0;
}

static BOOL ShowSndVolModernIndicator()
{
	if(bSndVolModernLaunched)
		return TRUE; // already launched

	HWND hSndVolModernIndicatorWnd = GetOpenSndVolModernIndicatorWnd();
	if(hSndVolModernIndicatorWnd)
		return TRUE; // already shown

	HWND hForegroundWnd = GetForegroundWindow();
	if(hForegroundWnd && hForegroundWnd != hTaskbarWnd)
		hSndVolModernPreviousForegroundWnd = hForegroundWnd;

	HWND hSndVolTrayControlWnd = GetSndVolTrayControlWnd();
	if(!hSndVolTrayControlWnd)
		return FALSE;

	if(!PostMessage(hSndVolTrayControlWnd, 0x460, 0, MAKELPARAM(NIN_SELECT, 100)))
		return FALSE;

	bSndVolModernLaunched = TRUE;
	return TRUE;
}

static BOOL HideSndVolModernIndicator()
{
	HWND hSndVolModernIndicatorWnd = GetOpenSndVolModernIndicatorWnd();
	if(hSndVolModernIndicatorWnd)
	{
		if(!hSndVolModernPreviousForegroundWnd || !SetForegroundWindow(hSndVolModernPreviousForegroundWnd))
			SetForegroundWindow(hTaskbarWnd);
	}

	return TRUE;
}

static void EndSndVolModernIndicatorSession()
{
	hSndVolModernPreviousForegroundWnd = NULL;
	bSndVolModernLaunched = FALSE;
	bSndVolModernAppeared = FALSE;
}

static BOOL IsCursorUnderSndVolModernIndicatorWnd()
{
	HWND hSndVolModernIndicatorWnd = GetOpenSndVolModernIndicatorWnd();
	if(!hSndVolModernIndicatorWnd)
		return FALSE;

	POINT pt;
	GetCursorPos(&pt);
	return WindowFromPoint(pt) == hSndVolModernIndicatorWnd;
}

static HWND GetOpenSndVolModernIndicatorWnd()
{
	HWND hForegroundWnd = GetForegroundWindow();
	if(!hForegroundWnd)
		return NULL;

	// Check class name
	WCHAR szBuffer[32];
	if(!GetClassName(hForegroundWnd, szBuffer, 32) ||
		wcscmp(szBuffer, L"Windows.UI.Core.CoreWindow") != 0)
		return NULL;

	// Check that the MtcUvc prop exists
	WCHAR szVerifyPropName[sizeof("ApplicationView_CustomWindowTitle#1234567890#MtcUvc")];
	wsprintf(szVerifyPropName, L"ApplicationView_CustomWindowTitle#%u#MtcUvc", (DWORD)(DWORD_PTR)hForegroundWnd);

	SetLastError(0);
	GetProp(hForegroundWnd, szVerifyPropName);
	if(GetLastError() != 0)
		return NULL;

	return hForegroundWnd;
}

static HWND GetSndVolTrayControlWnd()
{
	// The window we're looking for has a class name similar to "ATL:00007FFAECBBD280".
	// It shares a thread with the bluetooth window, which is easier to find by class,
	// so we use that.

	HWND hBluetoothNotificationWnd = FindWindow(L"BluetoothNotificationAreaIconWindowClass", NULL);
	if(!hBluetoothNotificationWnd)
		return NULL;

	HWND hWnd = NULL;
	EnumThreadWindows(GetWindowThreadProcessId(hBluetoothNotificationWnd, NULL), EnumThreadFindSndVolTrayControlWnd, (LPARAM)&hWnd);
	return hWnd;
}

static BOOL CALLBACK EnumThreadFindSndVolTrayControlWnd(HWND hWnd, LPARAM lParam)
{
	HMODULE hInstance = (HMODULE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	if(hInstance && hInstance == GetModuleHandle(L"sndvolsso.dll"))
	{
		*(HWND *)lParam = hWnd;
		return FALSE;
	}

	return TRUE;
}

// Other functions

BOOL GetSndVolTrayIconRect(RECT *prc)
{
	NOTIFYICONIDENTIFIER notifyiconidentifier;

	ZeroMemory(&notifyiconidentifier, sizeof(NOTIFYICONIDENTIFIER));
	notifyiconidentifier.cbSize = sizeof(NOTIFYICONIDENTIFIER);
	memcpy(&notifyiconidentifier.guidItem, "\x73\xAE\x20\x78\xE3\x23\x29\x42\x82\xC1\xE4\x1C\xB6\x7D\x5B\x9C", sizeof(GUID));

	return Shell_NotifyIconGetRect(&notifyiconidentifier, prc) == S_OK;
}

BOOL IsDefaultAudioEndpointAvailable()
{
	IMMDevice *defaultDevice = NULL;
	HRESULT hr;
	BOOL bSuccess = FALSE;

	if(pDeviceEnumerator)
	{
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		if(SUCCEEDED(hr))
		{
			bSuccess = TRUE;

			defaultDevice->Release();
		}
	}

	return bSuccess;
}

BOOL IsVolMuted(BOOL *pbMuted)
{
	IMMDevice *defaultDevice = NULL;
	IAudioEndpointVolume *endpointVolume = NULL;
	HRESULT hr;
	BOOL bSuccess = FALSE;

	if(pDeviceEnumerator)
	{
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		if(SUCCEEDED(hr))
		{
			hr = defaultDevice->Activate(XIID_IAudioEndpointVolume, CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
			if(SUCCEEDED(hr))
			{
				if(SUCCEEDED(endpointVolume->GetMute(pbMuted)))
					bSuccess = TRUE;

				endpointVolume->Release();
			}

			defaultDevice->Release();
		}
	}

	return bSuccess;
}

BOOL IsVolMutedAndNotZero(BOOL *pbResult)
{
	IMMDevice *defaultDevice = NULL;
	IAudioEndpointVolume *endpointVolume = NULL;
	HRESULT hr;
	float fMasterVolume;
	BOOL bMuted;
	BOOL bSuccess = FALSE;

	if(pDeviceEnumerator)
	{
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		if(SUCCEEDED(hr))
		{
			hr = defaultDevice->Activate(XIID_IAudioEndpointVolume, CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
			if(SUCCEEDED(hr))
			{
				if(SUCCEEDED(endpointVolume->GetMasterVolumeLevelScalar(&fMasterVolume)) &&
					SUCCEEDED(endpointVolume->GetMute(&bMuted)))
				{
					*pbResult = bMuted && (fMasterVolume > 0.005);
					bSuccess = TRUE;
				}

				endpointVolume->Release();
			}

			defaultDevice->Release();
		}
	}

	return bSuccess;
}

BOOL ToggleVolMuted()
{
	IMMDevice *defaultDevice = NULL;
	IAudioEndpointVolume *endpointVolume = NULL;
	HRESULT hr;
	BOOL bMuted;
	BOOL bSuccess = FALSE;

	if(pDeviceEnumerator)
	{
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		if(SUCCEEDED(hr))
		{
			hr = defaultDevice->Activate(XIID_IAudioEndpointVolume, CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
			if(SUCCEEDED(hr))
			{
				if(SUCCEEDED(endpointVolume->GetMute(&bMuted)))
				{
					if(SUCCEEDED(endpointVolume->SetMute(!bMuted, NULL)))
						bSuccess = TRUE;
				}

				endpointVolume->Release();
			}

			defaultDevice->Release();
		}
	}

	return bSuccess;
}

BOOL AddMasterVolumeLevelScalar(float fMasterVolumeAdd)
{
	IMMDevice *defaultDevice = NULL;
	IAudioEndpointVolume *endpointVolume = NULL;
	HRESULT hr;
	float fMasterVolume;
	BOOL bSuccess = FALSE;

	if(pDeviceEnumerator)
	{
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		if(SUCCEEDED(hr))
		{
			hr = defaultDevice->Activate(XIID_IAudioEndpointVolume, CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
			if(SUCCEEDED(hr))
			{
				if(SUCCEEDED(endpointVolume->GetMasterVolumeLevelScalar(&fMasterVolume)))
				{
					fMasterVolume += fMasterVolumeAdd;

					if(fMasterVolume < 0.0)
						fMasterVolume = 0.0;
					else if(fMasterVolume > 1.0)
						fMasterVolume = 1.0;

					if(SUCCEEDED(endpointVolume->SetMasterVolumeLevelScalar(fMasterVolume, NULL)))
					{
						bSuccess = TRUE;

						if(!g_settings.noAutomaticMuteToggle)
						{
							// Windows displays the volume rounded to the nearest percentage.
							// The range [0, 0.005) is displayed as 0%, [0.005, 0.015) as 1%, etc.
							// It also mutes the volume when it becomes zero, we do the same.

							if(fMasterVolume < 0.005)
								endpointVolume->SetMute(TRUE, NULL);
							else
								endpointVolume->SetMute(FALSE, NULL);
						}
					}
				}

				endpointVolume->Release();
			}

			defaultDevice->Release();
		}
	}

	return bSuccess;
}

#pragma endregion // sndvol

#pragma region mouse_hook

static DWORD WINAPI MouseHookThread(void *pParameter);
static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
static BOOL CaptureMouseWheel(const MSLLHOOKSTRUCT *msllHookStruct);

static volatile HANDLE hMouseHookThread;
static DWORD dwMouseHookThreadId;
static HHOOK hLowLevelMouseHook;
static ATOM wTaskSwitcherClass;

BOOL MouseHook_Init()
{
	if(hMouseHookThread)
	{
		return TRUE;
	}

	BOOL bSuccess = FALSE;

	WNDCLASS wndclass;
	wTaskSwitcherClass = (ATOM)GetClassInfo(GetModuleHandle(NULL), L"TaskSwitcherWnd", &wndclass);

	HANDLE hThreadReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(hThreadReadyEvent)
	{
		hMouseHookThread = CreateThread(NULL, 0, MouseHookThread, (void *)hThreadReadyEvent, CREATE_SUSPENDED, &dwMouseHookThreadId);
		if(hMouseHookThread)
		{
			SetThreadPriority(hMouseHookThread, THREAD_PRIORITY_TIME_CRITICAL);
			ResumeThread(hMouseHookThread);

			WaitForSingleObject(hThreadReadyEvent, INFINITE);

			bSuccess = TRUE;
		}

		CloseHandle(hThreadReadyEvent);
	}

	return bSuccess;
}

void MouseHook_Exit()
{
	HANDLE hThread = InterlockedExchangePointer(&hMouseHookThread, NULL);
	if(hThread)
	{
		PostThreadMessage(dwMouseHookThreadId, WM_APP, 0, 0);
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
}

static DWORD WINAPI MouseHookThread(void *pParameter)
{
	HANDLE hThreadReadyEvent;
	MSG msg;
	BOOL bRet;
	HANDLE hThread;

	hThreadReadyEvent = (HANDLE)pParameter;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	SetEvent(hThreadReadyEvent);

	hLowLevelMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
	if(hLowLevelMouseHook)
	{
		while((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
		{
			if(bRet == -1)
			{
				msg.wParam = 0;
				break;
			}

			if(msg.hwnd == NULL && msg.message == WM_APP)
			{
				PostQuitMessage(0);
				continue;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		UnhookWindowsHookEx(hLowLevelMouseHook);
	}
	else
		msg.wParam = 0;

	hThread = InterlockedExchangePointer(&hMouseHookThread, NULL);
	if(hThread)
		CloseHandle(hThread);

	return (DWORD)msg.wParam;
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode == HC_ACTION)
	{
		MSLLHOOKSTRUCT *msllHookStruct = (MSLLHOOKSTRUCT *)lParam;

		if(wParam == WM_MOUSEWHEEL)
		{
			if(CaptureMouseWheel(msllHookStruct))
			{
				return 1;
			}
		}

		// Pass events for classic sndvol handling.
		if(IsSndVolOpen())
		{
			switch(wParam)
			{
			case WM_MOUSEMOVE:
				OnSndVolMouseMove_MouseHook(msllHookStruct->pt);
				break;

			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
				OnSndVolMouseClick_MouseHook(msllHookStruct->pt);
				break;

			case WM_MOUSEWHEEL:
				OnSndVolMouseWheel_MouseHook(msllHookStruct->pt);
				break;
			}
		}
	}

	return CallNextHookEx(hLowLevelMouseHook, nCode, wParam, lParam);
}

static BOOL CaptureMouseWheel(const MSLLHOOKSTRUCT *msllHookStruct)
{
	if(wTaskSwitcherClass)
	{
		HWND hForegroundWnd = GetForegroundWindow();
		if(hForegroundWnd)
		{
			ATOM wClassAtom = (ATOM)GetClassLong(hForegroundWnd, GCW_ATOM);
			if(wClassAtom && wClassAtom == wTaskSwitcherClass)
			{
				return FALSE;
			}
		}
	}

	HWND hMaybeTransWnd = WindowFromPoint(msllHookStruct->pt);
	DWORD dwThreadId, dwProcessId;
	dwThreadId = GetWindowThreadProcessId(hMaybeTransWnd, &dwProcessId);

	if(dwProcessId != GetCurrentProcessId() || dwThreadId != dwTaskbarThreadId)
	{
		return FALSE;
	}

	// We send MSG_DLL_MOUSE_HOOK_WND_IDENT here, because we want to run WindowFromPoint on the
	// original thread, which in turn will skip transparent windows, by sending WM_NCHITTEST.
	// Reference: http://blogs.msdn.com/b/oldnewthing/archive/2010/12/30/10110077.aspx
	// Also, we call IdentifyTaskbarWindow from the original thread.

	MOUSE_HOOK_WND_IDENT_PARAM identParam;
	identParam.ppt = &msllHookStruct->pt;
	identParam.hMaybeTransWnd = hMaybeTransWnd;

	DWORD_PTR dwMsgResult;
	LRESULT lSendMessageResult = SendMessageTimeout(hTaskbarWnd, uTweakerMsg,
		(WPARAM)&identParam, MSG_DLL_MOUSE_HOOK_WND_IDENT, SMTO_ABORTIFHUNG, 500, &dwMsgResult);

	HWND hNonTransWnd = NULL;
	int nMaybeTransWndIdent = TASKBAR_WINDOW_UNKNOWN;

	if(lSendMessageResult && dwMsgResult)
	{
		dwThreadId = GetWindowThreadProcessId(identParam.hNonTransWnd, &dwProcessId);
		if(dwProcessId == GetCurrentProcessId() && dwThreadId == dwTaskbarThreadId)
		{
			hNonTransWnd = identParam.hNonTransWnd;
			nMaybeTransWndIdent = identParam.nMaybeTransWndIdent;
		}
	}

	if(!hNonTransWnd)
	{
		return FALSE;
	}

	BOOL bCaptureWheel = FALSE;
	switch(nMaybeTransWndIdent)
	{
	case TASKBAR_WINDOW_NOTIFY:
		if(g_settings.scrollArea == SCROLL_AREA_NOTIFICATION_AREA || g_settings.scrollArea == SCROLL_AREA_TASKBAR)
			bCaptureWheel = TRUE;
		break;

	case TASKBAR_WINDOW_TASKBAR:
		if(g_settings.scrollArea == SCROLL_AREA_TASKBAR)
			bCaptureWheel = TRUE;
		break;
	}

	if(!bCaptureWheel)
	{
		return FALSE;
	}

	WORD wVirtualKeys = 0;

	if(GetKeyState(VK_LBUTTON) < 0)
		wVirtualKeys |= MK_LBUTTON;

	if(GetKeyState(VK_RBUTTON) < 0)
		wVirtualKeys |= MK_RBUTTON;

	if(GetKeyState(VK_SHIFT) < 0)
		wVirtualKeys |= MK_SHIFT;

	if(GetKeyState(VK_CONTROL) < 0)
		wVirtualKeys |= MK_CONTROL;

	if(GetKeyState(VK_MBUTTON) < 0)
		wVirtualKeys |= MK_MBUTTON;

	if(GetKeyState(VK_XBUTTON1) < 0)
		wVirtualKeys |= MK_XBUTTON1;

	if(GetKeyState(VK_XBUTTON2) < 0)
		wVirtualKeys |= MK_XBUTTON2;

	PostMessage(GetAncestor(hNonTransWnd, GA_ROOT), WM_MOUSEWHEEL,
		MAKEWPARAM(wVirtualKeys, HIWORD(msllHookStruct->mouseData)),
		MAKELPARAM(msllHookStruct->pt.x, msllHookStruct->pt.y));

	return TRUE;
}

#pragma endregion // mouse_hook

////////////////////////////////////////////////////////////

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
	L"Windhawk_SetWindowSubclassFromAnyThread_taskbar-volume-control");

BOOL SetWindowSubclassFromAnyThread(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
		SUBCLASSPROC pfnSubclass;
		UINT_PTR uIdSubclass;
		DWORD_PTR dwRefData;
		BOOL result;
	};

	DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
	if (dwThreadId == 0) {
		return FALSE;
	}

	if (dwThreadId == GetCurrentThreadId()) {
		return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
	}

	HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI_LAMBDA_RETURN(LRESULT) {
		if (nCode == HC_ACTION) {
			const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
			if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
				SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param =
					(SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
				param->result = SetWindowSubclass(
					cwp->hwnd, param->pfnSubclass, param->uIdSubclass, param->dwRefData);
			}
		}

		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}, nullptr, dwThreadId);
	if (!hook) {
		return FALSE;
	}

	SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
	param.pfnSubclass = pfnSubclass;
	param.uIdSubclass = uIdSubclass;
	param.dwRefData = dwRefData;
	param.result = FALSE;
	SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (WPARAM)&param);

	UnhookWindowsHookEx(hook);

	return param.result;
}

LRESULT CALLBACK TaskbarWindowSubclassProc(
	_In_ HWND hWnd,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam,
	_In_ UINT_PTR uIdSubclass,
	_In_ DWORD_PTR dwRefData
	)
{
	switch (uMsg) {
	case WM_MOUSEWHEEL: {
		if (GetCapture() != NULL)
			break;

		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		RECT rc = {0};
		if (g_settings.scrollArea == SCROLL_AREA_TASKBAR) {
			GetWindowRect(hWnd, &rc);
		}
		else if (g_settings.scrollArea == SCROLL_AREA_NOTIFICATION_AREA) {
			if (hWnd == hTaskbarWnd) {
				HWND hTrayNotifyWnd = FindWindowEx(hWnd, NULL, L"TrayNotifyWnd", NULL);
				if (hTrayNotifyWnd)
					GetWindowRect(hTrayNotifyWnd, &rc);
			}
			else if (nWinVersion >= WIN_VERSION_11_21H2) {
				RECT rcTaskbar;
				GetWindowRect(hWnd, &rcTaskbar);
				HWND hBridgeWnd = FindWindowEx(hWnd, NULL, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
				while (hBridgeWnd) {
					RECT rcBridge;
					GetWindowRect(hBridgeWnd, &rcBridge);
					if(rcBridge.left != rcTaskbar.left ||
						rcBridge.top != rcTaskbar.top ||
						rcBridge.right != rcTaskbar.right ||
						rcBridge.bottom != rcTaskbar.bottom) {
						CopyRect(&rc, &rcBridge);
						break;
					}

					hBridgeWnd = FindWindowEx(hWnd, hBridgeWnd, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
				}
			}
			else if (nWinVersion >= WIN_VERSION_10_R1) {
				HWND hClockButtonWnd = FindWindowEx(hWnd, NULL, L"ClockButton", NULL);
				if (hClockButtonWnd)
					GetWindowRect(hClockButtonWnd, &rc);
			}
		}

		if (!PtInRect(&rc, pt))
			break;

		if (!IsSndVolOpen()) {
			// Allows to steal focus
			INPUT input;
			ZeroMemory(&input, sizeof(INPUT));
			SendInput(1, &input, sizeof(INPUT));

			if (OpenScrollSndVol(wParam, lParam)) {
				SetSndVolTimer();
			}
		}
		else {
			BOOL bMutedAndNotZero;
			if (IsVolMutedAndNotZero(&bMutedAndNotZero) && !bMutedAndNotZero)
				ScrollSndVol(wParam, lParam);

			ResetSndVolTimer();
		}

		return 0;
	}

	case WM_DESTROY:
		if (hWnd != hTaskbarWnd) {
			g_secondaryTaskbarWindows.erase(hWnd);
		}
		break;

	default:
		if (uMsg == uTweakerMsg && hWnd == hTaskbarWnd) {
			switch(lParam) {
			case MSG_DLL_CALLFUNC:
				return ((LONG_PTR(*)())wParam)();

			case MSG_DLL_MOUSE_HOOK_WND_IDENT:
				MOUSE_HOOK_WND_IDENT_PARAM* pParam = (MOUSE_HOOK_WND_IDENT_PARAM*)wParam;
				pParam->nMaybeTransWndIdent = IdentifyTaskbarWindow(pParam->hMaybeTransWnd);
				pParam->hNonTransWnd = WindowFromPoint(*pParam->ppt);
				return 1;
			}
		}

		if (uMsg == g_subclassRegisteredMsg && !wParam)
			RemoveWindowSubclass(hWnd, TaskbarWindowSubclassProc, 0);
		break;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void SubclassTaskbarWindow(HWND hWnd)
{
	SetWindowSubclassFromAnyThread(hWnd, TaskbarWindowSubclassProc, 0, 0);
}

void UnsubclassTaskbarWindow(HWND hWnd)
{
	SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}

void HandleIdentifiedTaskbarWindow(HWND hWnd)
{
	hTaskbarWnd = hWnd;
	dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);
	SndVolInit();
	SubclassTaskbarWindow(hWnd);
	for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
		SubclassTaskbarWindow(hSecondaryWnd);
	}
	MouseHook_Init();
}

HWND FindCurrentProcessTaskbarWindows(std::unordered_set<HWND>* secondaryTaskbarWindows)
{
	struct ENUM_WINDOWS_PARAM {
		HWND* hWnd;
		std::unordered_set<HWND>* secondaryTaskbarWindows;
	};

	HWND hWnd = nullptr;
	ENUM_WINDOWS_PARAM param = { &hWnd, secondaryTaskbarWindows };
	EnumWindows([](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
		ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

		DWORD dwProcessId = 0;
		if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId())
			return TRUE;

		WCHAR szClassName[32];
		if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
			return TRUE;

		if (wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
			*param.hWnd = hWnd;
		}
		else if (wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
			param.secondaryTaskbarWindows->insert(hWnd);
		}

		return TRUE;
	}, (LPARAM)&param);

	return hWnd;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pOriginalCreateWindowExW;
HWND WINAPI CreateWindowExWHook(
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
	LPVOID lpParam)
{
	HWND hWnd = pOriginalCreateWindowExW(
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

	if (!hWnd)
		return hWnd;

	BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

	// if (bTextualClassName)
	// 	Wh_Log(L"Created window of class %s", lpClassName);
	// else
	// 	Wh_Log(L"Created window of class atom %X", (DWORD)(ULONG_PTR)lpClassName);

	if (bTextualClassName && wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
		Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
		HandleIdentifiedTaskbarWindow(hWnd);
	}
	else if (bTextualClassName && wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
		Wh_Log(L"Secondary taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
		g_secondaryTaskbarWindows.insert(hWnd);
		SubclassTaskbarWindow(hWnd);
	}

	return hWnd;
}

void LoadSettings()
{
	PCWSTR volumeIndicator = Wh_GetStringSetting(L"volumeIndicator");
	g_settings.volumeIndicator = VOLUME_INDICATOR_MODERN;
	if (wcscmp(volumeIndicator, L"classic") == 0) {
		g_settings.volumeIndicator = VOLUME_INDICATOR_CLASSIC;
	}
	else if (wcscmp(volumeIndicator, L"tooltip") == 0) {
		g_settings.volumeIndicator = VOLUME_INDICATOR_TOOLTIP;
	}
	else if (wcscmp(volumeIndicator, L"none") == 0) {
		g_settings.volumeIndicator = VOLUME_INDICATOR_NONE;
	}
	Wh_FreeStringSetting(volumeIndicator);

	PCWSTR scrollArea = Wh_GetStringSetting(L"scrollArea");
	g_settings.scrollArea = SCROLL_AREA_TASKBAR;
	if (wcscmp(scrollArea, L"notification_area") == 0) {
		g_settings.scrollArea = SCROLL_AREA_NOTIFICATION_AREA;
	}
	Wh_FreeStringSetting(scrollArea);

    g_settings.noAutomaticMuteToggle = Wh_GetIntSetting(L"noAutomaticMuteToggle");
    g_settings.volumeChangeStep = Wh_GetIntSetting(L"volumeChangeStep");
}

BOOL Wh_ModInit()
{
	Wh_Log(L"Init");

	if (!WindowsVersionInit()) {
		return FALSE;
	}

	LoadSettings();

	Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook, (void**)&pOriginalCreateWindowExW);

	WNDCLASS wndclass;
	if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
		HWND hWnd = FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
		if (hWnd) {
			HandleIdentifiedTaskbarWindow(hWnd);
		}
	}

	return TRUE;
}

void Wh_ModUninit()
{
	Wh_Log(L"Uninit");

	MouseHook_Exit();

	if (hTaskbarWnd) {
		UnsubclassTaskbarWindow(hTaskbarWnd);

		for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
			UnsubclassTaskbarWindow(hSecondaryWnd);
		}
	}

	CleanupSndVol();
	SndVolUninit();
}

void Wh_ModSettingsChanged()
{
	Wh_Log(L"SettingsChanged");

	LoadSettings();
}
