// ==WindhawkMod==
// @id              sysdm-general-tab
// @name            System Properties "General" Tab
// @description     Restores the "General" tab to the system properties dialog (sysdm.cpl).
// @version         1.1
// @author          Isabella Lulamoon (kawapure)
// @github          https://github.com/kawapure
// @twitter         https://twitter.com/kawaipure
// @homepage        https://kawapure.github.io
// @include         SystemPropertiesAdvanced.exe
// @include         SystemPropertiesComputerName.exe
// @include         SystemPropertiesHardware.exe
// @include         SystemPropertiesRemote.exe
// @include         SystemPropertiesProtection.exe
// @include         control.exe
// @compilerOptions -lcomctl32 -lshlwapi -lole32 -loleaut32 -lntdll -lwbemuuid -lversion -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# System Properties "General" Tab

This mod restores the "General" tab displaying system information in the system properties
Control Panel applet (`sysdm.cpl`).

#![Preview image](https://raw.githubusercontent.com/kawapure/images/refs/heads/main/%E7%84%A1%E9%A1%8C.png)

## ⚠ Required setup steps 

**You need to retrieve a copy of `sysdm.cpl` from Windows XP or Windows 2000 in order for the
dialog to show up correctly.** This mod requires resources available only in those binaries,
and those cannot be redistributed. It is recommended that you retrieve the files from a version
of Windows which is the same language as what you currently have installed for display consistency.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- xp_sysdm_path: C:\path\to\xp\sysdm.cpl
  $name: Path to a copy of `sysdm.cpl` from Windows XP or 2000
  $description: > 
    Grab this file from a Windows XP or 2000 ISO of the same language as your current Windows install.
- hide_at_compatible: true
  $name: Hide "AT/AT COMPATIBLE" system identifiers
  $description: This is done in Windows XP.
- hide_pae: true
  $name: Hide "Physical Address Extension"
  $description: This is only shown on 32-bit versions of Windows.
- coalesce_clock_speed_and_ram: true
  $name: Coalesce clock speed and RAM onto the same row
  $description: This is done since Windows XP Service Pack 2.
- ram_in_kilobytes: false
  $name: RAM in KB
  $description: RAM was always displayed in kilobytes prior to Windows XP.
- custom_os_name: Microsoft Windows XP
  $name: Override OS name
  $description: Replace the OS name with anything you want.
- custom_os_ver1: ""
  $name: "Override OS additional information row #1"
  $description: Replace the first additional-information row with anything you want (i.e. "Service Pack 1").
- custom_os_ver2: ""
  $name: "Override OS additional information row #2"
  $description: Replace the second additional-information row with anything you want.
- custom_os_ver3: ""
  $name: "Override OS additional information row #3 (XP-only)"
  $description: >
    Replace the second additional-information row with anything you want.
    This only works on Windows XP sysdm.cpl resources.
*/
// ==/WindhawkModSettings==

#include <libloaderapi.h>
#include <oleauto.h>
#include <processenv.h>
#include <prsht.h>
#include <rpcdce.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <winerror.h>
#include <winnt.h>
#include <winternl.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <wbemcli.h>
#include <winver.h>
#include <winnls.h>
#include <cstdlib>

HINSTANCE g_hInst = nullptr;
HINSTANCE g_hInstResource = nullptr;
DWORD g_dwResourceVersion = 0;
bool g_fHijackDefaultTab = false;

inline bool IsWindows2000Resource()
{
    return g_dwResourceVersion <= 0x00050000; // 5.0
}

inline bool IsWindowsXPResource()
{
    return g_dwResourceVersion >= 0x00050001; // 5.1+
}

struct
{
    LPCWSTR pszXpSysdm = nullptr;
    LPCWSTR pszCustomOsName = nullptr;
    LPCWSTR pszCustomOsVer1 = nullptr;
    LPCWSTR pszCustomOsVer2 = nullptr;
    LPCWSTR pszCustomOsVer3 = nullptr;
    bool fHideAtCompatible = false;
    bool fHidePae = false;
    bool fCoalesceClockSpeedAndRam = false;
    bool fRamInKilobytes = false;
} g_settings;

// Resources in sysdm.cpl
#define IDB_WINDOWS     1
#define IDB_WINDOWS_256 2
#define IDS_RAM         5
#define IDS_PROCESSOR_SPEED     33
#define IDS_PROCESSOR_SPEEDGHZ  34
#define IDD_GENERALTAB          101
#define IDD_PHONESUPPORT        102
#define IDC_PHONESUPPORTTEXT    70

#define IDC_GEN_WINDOWS_IMAGE 51
#define IDC_GEN_VERSION_0     52
#define IDC_GEN_REGISTERED_0  56
#define IDC_GEN_OEM_NUDGE     60
#define IDC_GEN_COMPUTER_LABEL 61
#define IDC_GEN_OEM_IMAGE     62
#define IDC_GEN_MACHINE_0     63
#define IDC_GEN_OEM_SUPPORT   69

#define IDC_GEN_2K_VERSION_0     52
#define IDC_GEN_2K_REGISTERED_0  54
#define IDC_GEN_2K_COMPUTER_LABEL 59
#define IDC_GEN_2K_MACHINE       61
#define IDC_GEN_2K_OEM_NUDGE     58
#define IDC_GEN_2K_OEM_IMAGE     60
#define IDC_GEN_2K_OEM_SUPPORT   67

#define MAX_PROCESSOR_NAME 2083

#define WM_SYSCPL_GEN_UPDATECPUINFO (WM_APP + 1)

BOOL IsLowColor(HWND hDlg)
{
    BOOL fIsLowColor = FALSE;
    HDC hdc = GetDC(hDlg);

    if (hdc)
    {
        INT iColors = GetDeviceCaps(hdc, NUMCOLORS);
        fIsLowColor = iColors != -1 && iColors <= 256;
        ReleaseDC(hDlg, hdc);
    }

    return fIsLowColor;
}

struct PROCESSOR_INFO
{
    WCHAR szProcessorName[MAX_PROCESSOR_NAME];
    WCHAR szProcessorClockSpeed[MAX_PROCESSOR_NAME]; // Microsoft were REALLY inefficient when designing this API.
};

struct INITDLGSTRUCT
{
    INT64 iMemoryAmount;
    PROCESSOR_INFO processorInfo;
    BOOL fShowProcessorName;
    BOOL fShowProcessorClockSpeed;
};

#define GETOEMFILE_GETDATA 0
#define GETOEMFILE_GETIMAGE 1

HRESULT _GetOemFile(LPWSTR szOemFile, UINT cchOemFile, DWORD dwFlags)
{
    HRESULT hr;
    LPCWSTR szFileName = (GETOEMFILE_GETDATA == dwFlags)
        ? L"OemInfo.Ini"
        : L"OemLogo.Bmp";

    szOemFile[0] = 0;

    // first look in system directory
    if (!GetSystemDirectoryW(szOemFile, cchOemFile))
    {
        hr = E_FAIL;
    }
    else
    {
        if (!PathAppendW(szOemFile, szFileName))
        {
            hr = E_FAIL;
        }
        else
        {
            if (PathFileExistsW(szOemFile))
            {
                // NT OEM information found successfully:
                hr = S_OK;
            }
            else
            {
                // 9X OEM information fallback:
                if (!GetWindowsDirectoryW(szOemFile, cchOemFile))
                {
                    hr = E_FAIL;
                }
                else
                {
                    if (PathAppendW(szOemFile, L"System\\") &&
                        PathAppendW(szOemFile, szFileName) &&
                        PathFileExistsW(szOemFile))
                    {
                        hr = S_OK;
                    }
                    else
                    {
                        hr = E_FAIL;
                    }
                }
            }
        }
    }
    
    return hr;
}

HRESULT SetWMISecurityBlanket(IUnknown *pUnk, IUnknown *pUnk2)
{
    IClientSecurity *pClientSecurity;
    HRESULT hr = pUnk->QueryInterface(IID_PPV_ARGS(&pClientSecurity));

    if (SUCCEEDED(hr))
    {
        hr = pClientSecurity->SetBlanket(pUnk, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, 
            RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
        pClientSecurity->Release();
    }

    return hr;
}

HRESULT GetWMI_Win32_Processor(IEnumWbemClassObject **ppEnumProcessors)
{
    HRESULT hr = E_NOTIMPL;

    *ppEnumProcessors = nullptr;
    IWbemLocator *pLocator = nullptr;

    hr = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pLocator));
    if (SUCCEEDED(hr))
    {
        hr = E_OUTOFMEMORY;
        BSTR bstrLocalMachine = SysAllocString(L"root\\cimv2");
        if (bstrLocalMachine)
        {
            IWbemServices *pWbemServices;

            hr = pLocator->ConnectServer(bstrLocalMachine, nullptr, nullptr, 0, 0, nullptr, nullptr, &pWbemServices);
            if (SUCCEEDED(hr))
            {
                hr = E_OUTOFMEMORY;
                BSTR bstrQueryLang = SysAllocString(L"WQL");
                BSTR bstrQuery = SysAllocString(L"select Name,CurrentClockSpeed,MaxClockSpeed from Win32_Processor");
                if (bstrQueryLang && bstrQuery)
                {
                    IEnumWbemClassObject *pEnum = nullptr;
                    hr = pWbemServices->ExecQuery(bstrQueryLang, bstrQuery, 
                        WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY, nullptr, &pEnum);

                    if (SUCCEEDED(hr))
                    {
                        hr = pEnum->QueryInterface(IID_PPV_ARGS(ppEnumProcessors));
                    }

                    pEnum->Release();
                }

                SysFreeString(bstrQuery);
                SysFreeString(bstrQueryLang);
                pWbemServices->Release();
            }

            SysFreeString(bstrLocalMachine);
        }

        pLocator->Release();
    }

    return hr;
}

HRESULT GetProcessorDescFromWMI(PROCESSOR_INFO *pProcessorInfo)
{
    IEnumWbemClassObject *pEnumProcessors;
    HRESULT hr = GetWMI_Win32_Processor(&pEnumProcessors);

    if (SUCCEEDED(hr))
    {
        IWbemClassObject *pProcessor;
        ULONG ulRet;

        hr = pEnumProcessors->Next(WBEM_INFINITE, 1, &pProcessor, &ulRet);
        if (SUCCEEDED(hr))
        {
            VARIANT varProcessorName = { 0 };

            hr = pProcessor->Get(L"Name", 0, &varProcessorName, nullptr, nullptr);

            // If we're working with a Windows 2000 resource, then the clock speed cannot
            // be displayed as 2000 doesn't have the necessary strings to format.
            if (SUCCEEDED(hr) && IsWindowsXPResource())
            {
                VARIANT varProcessorSpeed = { 0 };

                hr = pProcessor->Get(L"CurrentClockSpeed", 0, &varProcessorSpeed, nullptr, nullptr);
                if (SUCCEEDED(hr) && varProcessorSpeed.vt == VT_I4 && varProcessorSpeed.lVal < 50)
                {
                    hr = pProcessor->Get(L"MaxClockSpeed", 0, &varProcessorSpeed, nullptr, nullptr);
                }

                if (SUCCEEDED(hr))
                {
                    if (varProcessorName.vt == VT_BSTR && varProcessorSpeed.vt == VT_I4)
                    {
                        if (FAILED(StringCchCopy(pProcessorInfo->szProcessorName, ARRAYSIZE(pProcessorInfo->szProcessorName), varProcessorName.bstrVal)))
                        {
                            pProcessorInfo->szProcessorName[0] = L'\0';
                        }

                        WCHAR szTemplate[MAX_PATH] = { 0 };

                        if (varProcessorSpeed.lVal > 1000)
                        {
                            WCHAR szSpeed[20];
                            double dGhz = (varProcessorSpeed.lVal / (double)1000.0);

                            if (FAILED(StringCchPrintfW(szSpeed, ARRAYSIZE(szSpeed), L"%1.2f", dGhz)))
                            {
                                pProcessorInfo->szProcessorClockSpeed[0] = L'\0';
                            }
                            else
                            {
                                LoadStringW(g_hInstResource, IDS_PROCESSOR_SPEEDGHZ, szTemplate, ARRAYSIZE(szTemplate));
                                if (FAILED(StringCchPrintfW(pProcessorInfo->szProcessorClockSpeed, 
                                    ARRAYSIZE(pProcessorInfo->szProcessorClockSpeed), szTemplate, szSpeed)))
                                {
                                    pProcessorInfo->szProcessorClockSpeed[0] = L'\0';
                                }
                            }
                        }
                        else
                        {
                            LoadStringW(g_hInstResource, IDS_PROCESSOR_SPEED, szTemplate, ARRAYSIZE(szTemplate));
                            if (FAILED(StringCchPrintfW(pProcessorInfo->szProcessorClockSpeed, 
                                ARRAYSIZE(pProcessorInfo->szProcessorClockSpeed), szTemplate, varProcessorSpeed.lVal)))
                            {
                                pProcessorInfo->szProcessorClockSpeed[0] = L'\0';
                            }
                        }
                    }
                    else
                    {
                        hr = E_FAIL;
                    }
                }
                
                VariantClear(&varProcessorSpeed);
            }
            else if (varProcessorName.vt == VT_BSTR) // 2000 path, clock speed strings unavailable.
            {
                if (FAILED(StringCchCopy(pProcessorInfo->szProcessorName, ARRAYSIZE(pProcessorInfo->szProcessorName), varProcessorName.bstrVal)))
                {
                    pProcessorInfo->szProcessorName[0] = L'\0';
                }
            }

            VariantClear(&varProcessorName);
            pProcessor->Release();
        }

        pEnumProcessors->Release();
    }

    return hr;
}

HRESULT GetProcessorInfoFromRegistry(HKEY hKey, PROCESSOR_INFO *pProcessorInfo)
{
    HRESULT hr = E_FAIL;
    WCHAR szBuffer[MAX_PROCESSOR_NAME] = { 0 };
    DWORD cbData = sizeof(szBuffer);
    
    if ((SHRegGetValue(hKey, nullptr, L"ProcessorNameString", SRRF_RT_REG_SZ | SRRF_RT_REG_EXPAND_SZ | SRRF_NOEXPAND, 
        nullptr, (LPBYTE)szBuffer, &cbData) == ERROR_SUCCESS) &&
        (*szBuffer != 0) && (cbData > 1))
    {
        hr = StringCchCopyW(pProcessorInfo->szProcessorName, ARRAYSIZE(pProcessorInfo->szProcessorName), szBuffer);
    }

    return hr;
}

BOOL _GetProcessorDescription(PROCESSOR_INFO *pProcessorInfo, BOOL *pfShowClockSpeed)
{
    BOOL fShowProcessorName = FALSE;
    *pfShowClockSpeed = IsWindowsXPResource() ? TRUE : FALSE;

    if (SHRegGetBoolUSValueW(L"HARDWARE\\DESCRIPTION\\System", L"UseWMI", FALSE, TRUE))
    {
        if (SUCCEEDED(GetProcessorDescFromWMI(pProcessorInfo)))
        {
            fShowProcessorName = TRUE;
        }
    }

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey))
    {
        if (FAILED(GetProcessorInfoFromRegistry(hKey, pProcessorInfo)))
        {
            if (!fShowProcessorName)
            {
                DWORD cbData = sizeof(pProcessorInfo->szProcessorName);
                if (SHRegGetValue(hKey, nullptr, L"Identifier", SRRF_RT_REG_SZ | SRRF_RT_REG_EXPAND_SZ | SRRF_NOEXPAND, 
                    nullptr, (LPBYTE)pProcessorInfo->szProcessorName, &cbData))
                {
                    fShowProcessorName = TRUE;
                    *pfShowClockSpeed = FALSE;
                }
            }
        }

        RegCloseKey(hKey);
    }

    return fShowProcessorName;
}

#define LW_MAX_URL_LENGTH   (2048 + 32 + sizeof("://"))

typedef struct tagLWITEMW {
    UINT        mask ;
    int         iLink ;
    UINT        state ;
    UINT        stateMask ;
    WCHAR       szID[MAX_LINKID_TEXT] ;
    WCHAR       szUrl[LW_MAX_URL_LENGTH] ;
} LWITEMW, *LPLWITEMW;

#define LWIF_ITEMINDEX  0x00000001
#define LWIF_STATE      0x00000002
#define LWIF_ITEMID     0x00000004
#define LWIF_URL        0x00000008

#define LWIS_FOCUSED    0x0001
#define LWIS_ENABLED    0x0002
#define LWIS_VISITED    0x0004
#define LWIS_SHELLEXECURL 0x00000008

//  LinkWindow messages
#define LWM_HITTEST         (WM_USER + 0x300)
#define LWM_GETIDEALHEIGHT  (WM_USER + 0x301)
#define LWM_SETITEM         (WM_USER + 0x302)
#define LWM_GETITEM         (WM_USER + 0x303)

HRESULT _SetMachineInfoLine(HWND hDlg, int idControl, LPCWSTR pszText, bool fSetTabStop)
{
    HRESULT hr = S_OK;

    HWND hWndControl = GetDlgItem(hDlg, idControl);
    SetDlgItemTextW(hDlg, idControl, pszText);

    if (fSetTabStop)
    {
        SetWindowLongPtrW(hWndControl, GWL_STYLE, (WS_TABSTOP | GetWindowLongPtr(hWndControl, GWL_STYLE)));
    }
    else
    {
        LWITEMW item = { 0 };
        item.mask = LWIF_ITEMINDEX | LWIF_STATE;
        item.stateMask = LWIS_ENABLED;
        item.state = 0;
        item.iLink = 0;

        hr = SendMessageW(hWndControl, LWM_SETITEM, 0, (LPARAM)&item) ? S_OK : E_FAIL;
    }

    return hr;
}

HRESULT _GetLinkInfo(HKEY hkey, LPCWSTR pszLanguageKey, int nIndex, LPWSTR pszLink, SIZE_T cchNameSize)
{
    DWORD cbSize = (DWORD)(cchNameSize * sizeof(pszLink[0]));
    WCHAR szIndex[10];
    
    HRESULT hr = StringCchPrintfW(szIndex, ARRAYSIZE(szIndex), L"%03d", nIndex);
    if (SUCCEEDED(hr))
    {
        DWORD dwError = SHRegGetValueW(hkey,
                                      pszLanguageKey,
                                      szIndex,
                                      SRRF_RT_REG_SZ | SRRF_RT_REG_EXPAND_SZ | SRRF_NOEXPAND,
                                      nullptr,
                                      (void *)pszLink, 
                                      &cbSize);

        hr = HRESULT_FROM_WIN32(dwError);
    }
    
    return hr;
}

int g_idcFirstOemLink = 0;

HRESULT AddOEMHyperLinks(HWND hDlg, int *pIdCtrl)
{
    g_idcFirstOemLink = *pIdCtrl;
    HKEY hkey;
    DWORD dwError = RegOpenKeyExW(HKEY_LOCAL_MACHINE, 
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MyComputer\\OEMLinks", 
        0, KEY_READ, &hkey);
    HRESULT hr = HRESULT_FROM_WIN32(dwError);
    
    if (SUCCEEDED(hr))
    {
        int idcLastSlot = IsWindowsXPResource()
            ? IDC_GEN_MACHINE_0 + 3
            : IDC_GEN_2K_MACHINE + 3;

        for (int i = 0; ((i <= 3) && (*pIdCtrl <= idcLastSlot)); i++)
        {
            WCHAR szLink[2 * MAX_PROCESSOR_NAME];
            WCHAR szLanguageKey[10];

            hr = StringCchPrintfW(szLanguageKey, ARRAYSIZE(szLanguageKey), TEXT("%u"), GetACP());
            if (SUCCEEDED(hr))
            {
                hr = _GetLinkInfo(hkey, szLanguageKey, i, szLink, ARRAYSIZE(szLink));
                if (FAILED(hr) && (GetACP() != 1252))
                {
                    hr = _GetLinkInfo(hkey, L"1252", i, szLink, ARRAYSIZE(szLink));
                }
            }

            if (SUCCEEDED(hr))
            {
                _SetMachineInfoLine(hDlg, *pIdCtrl, szLink, TRUE);
            }

            (*pIdCtrl)++;
        }

        RegCloseKey(hkey);
    }

    return hr;
}

void Int64ToStr(INT64 n, LPTSTR lpBuffer)
{
    WCHAR szTemp[30] = { 0 };
    INT64 i = 0;

    do
    {
        szTemp[i++] = L'0' + (WCHAR)(n % 10);
        n = n / 10;
    }
    while (n != 0);

    do
    {
        *lpBuffer++ = szTemp[--i];
    }
    while (i != 0);

    *lpBuffer++ = '\0';
}

LPWSTR WINAPI AddCommas64(INT64 n, LPWSTR pszResult)
{
    WCHAR szTemp[40] = { 0 };
    WCHAR szSep[5] = { 0 };
    NUMBERFMT nfmt;

    nfmt.NumDigits = 0;
    nfmt.LeadingZero = 0;
    GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, szSep, ARRAYSIZE(szSep));
	swscanf(szSep, L"%d", &nfmt.Grouping);
    GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, szSep, ARRAYSIZE(szSep));
    nfmt.lpDecimalSep = nfmt.lpThousandSep = szSep;
    nfmt.NegativeOrder = 0;

    Int64ToStr(n, szTemp);

    if (GetNumberFormatW(LOCALE_USER_DEFAULT, 0, szTemp, &nfmt, pszResult, ARRAYSIZE(szTemp)) == 0)
        StringCchCopyW(pszResult, MAX_PATH, szTemp); // assuming MAX_PATH

    return pszResult;
}

HRESULT FormatMemoryAmountText(UINT64 iMemoryAmount, LPWSTR pszDest, int cchDest)
{
    WCHAR szBuffer[MAX_PATH];
    WCHAR szTemplate[MAX_PATH];

    if (!LoadStringW(g_hInstResource, IDS_RAM, szTemplate, ARRAYSIZE(szTemplate)))
    {
        StringCchCopyW(szTemplate, ARRAYSIZE(szTemplate), L"%s of RAM");
    }

    if (IsWindowsXPResource() && !g_settings.fRamInKilobytes)
    {
        StrFormatByteSizeW(iMemoryAmount, szBuffer, ARRAYSIZE(szBuffer));
        return StringCchPrintfW(pszDest, cchDest, szTemplate, szBuffer);
    }
    else
    {
        WCHAR szNumBuffer[MAX_PATH];
        return StringCchPrintfW(pszDest, cchDest, szTemplate, AddCommas64(iMemoryAmount / 1024, szNumBuffer));
    }

    return E_UNEXPECTED;
}

static int s_iIdCtrlClockSpeedLabel = -1;

void _SetProcessorDescription(HWND hDlg, PROCESSOR_INFO *pProcessorInfo, bool fShowClockSpeed, bool fShowProcessorInfo, int *piIdControl)
{
    if (fShowProcessorInfo)
    {
        WCHAR szProcessorLine1[MAX_PATH] = { 0 };
        WCHAR szProcessorLine2[MAX_PATH] = { 0 };

        StringCchCopyW(szProcessorLine1, ARRAYSIZE(szProcessorLine1), pProcessorInfo->szProcessorName);

        if (lstrlen(szProcessorLine1) >= 30)
        {
            // Word wrap:
            WCHAR *pszWrapPoint = StrRChrW(szProcessorLine1, szProcessorLine1 + 30, L' ');
            if (pszWrapPoint)
            {
                StringCchCopyW(szProcessorLine2, ARRAYSIZE(szProcessorLine2), pszWrapPoint + 1);
                *pszWrapPoint = L'\0';
            }
            else
            {
                StringCchCopyW(szProcessorLine2, ARRAYSIZE(szProcessorLine2), &szProcessorLine1[30]);
                szProcessorLine1[30] = L'\0';
            }
        }

        _SetMachineInfoLine(hDlg, (*piIdControl)++, szProcessorLine1, false);

        if (szProcessorLine2[0])
        {
            _SetMachineInfoLine(hDlg, (*piIdControl)++, szProcessorLine2, false);
        }

        if (fShowClockSpeed && IsWindowsXPResource())
        {
            s_iIdCtrlClockSpeedLabel = (*piIdControl)++;
            _SetMachineInfoLine(hDlg, s_iIdCtrlClockSpeedLabel, pProcessorInfo->szProcessorClockSpeed, false);
        }
    }
}

void CompleteGeneralDlgInitialization(HWND hDlg, INITDLGSTRUCT *pInitData)
{
    int idCtrlCur = IsWindowsXPResource() ? IDC_GEN_MACHINE_0 : IDC_GEN_2K_MACHINE;

    WCHAR szOemFile[MAX_PATH];
    if (SUCCEEDED(_GetOemFile(szOemFile, ARRAYSIZE(szOemFile), GETOEMFILE_GETDATA)))
    {
        WCHAR szBuffer1[64];
        WCHAR szBuffer2[64];
        if (GetPrivateProfileStringW(L"General", L"Manufacturer", L"",
                                    szBuffer1, ARRAYSIZE(szBuffer1), szOemFile))
        {
            _SetMachineInfoLine(hDlg, idCtrlCur++, szBuffer1, FALSE);

            if(GetPrivateProfileStringW(L"General", L"Model",
                                       L"", szBuffer1, ARRAYSIZE(szBuffer1), szOemFile))
            {
                _SetMachineInfoLine(hDlg, idCtrlCur++, szBuffer1, FALSE);
            }

            if (SUCCEEDED(StringCchCopyW(szBuffer2, ARRAYSIZE(szBuffer2), L"line")) &&
                SUCCEEDED(StringCchCatW(szBuffer2, ARRAYSIZE(szBuffer2), TEXT("1"))))
            {
                if(GetPrivateProfileStringW(L"Support Information",
                                           szBuffer2, L"", szBuffer1, ARRAYSIZE(szBuffer1), szOemFile))
                {
                    HWND hWndSupportBtn = GetDlgItem(hDlg, 
                        IsWindowsXPResource() ? IDC_GEN_OEM_SUPPORT : IDC_GEN_2K_OEM_SUPPORT);

                    EnableWindow(hWndSupportBtn, TRUE);
                    ShowWindow(hWndSupportBtn, SW_SHOW);
                }

                if (SUCCEEDED(_GetOemFile(szOemFile, ARRAYSIZE(szOemFile), GETOEMFILE_GETIMAGE)))
                {
                    HANDLE hImage = LoadImageW(nullptr, szOemFile, IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS | LR_LOADFROMFILE);

                    if (hImage)
                    {
                        int idcOemImage = IsWindowsXPResource()
                            ? IDC_GEN_OEM_IMAGE
                            : IDC_GEN_2K_OEM_IMAGE;
                        
                        int idcOemNudge = IsWindowsXPResource()
                            ? IDC_GEN_OEM_NUDGE
                            : IDC_GEN_2K_OEM_NUDGE;

                        int idcGenComputerLabel = IsWindowsXPResource()
                            ? IDC_GEN_COMPUTER_LABEL
                            : IDC_GEN_2K_COMPUTER_LABEL;

                        SendMessageW(GetDlgItem(hDlg, idcOemImage), STM_SETIMAGE, IMAGE_BITMAP, 
                            (LPARAM)hImage
                        );
                        ShowWindow(GetDlgItem(hDlg, idcOemNudge), SW_SHOWNA);
                        ShowWindow(GetDlgItem(hDlg, idcGenComputerLabel), SW_HIDE);
                    }
                }
            }
        }
    }

    _SetProcessorDescription(hDlg, &pInitData->processorInfo, pInitData->fShowProcessorClockSpeed, pInitData->fShowProcessorName, &idCtrlCur);

    // System identifier
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        WCHAR szBuffer[MAX_PATH];
        DWORD cbData = sizeof(szBuffer);
        if (SHRegGetValueW(hKey, NULL, L"Identifier", SRRF_RT_REG_SZ | SRRF_RT_REG_EXPAND_SZ | SRRF_NOEXPAND, 0, (LPBYTE)szBuffer, &cbData) == ERROR_SUCCESS)
        {
            if (!g_settings.fHideAtCompatible || StrCmpIW(szBuffer, L"AT/AT Compatible") != 0)
            {
                _SetMachineInfoLine(hDlg, idCtrlCur++, szBuffer, FALSE);
            }
        }

        RegCloseKey(hKey);
    }

    // RAM:
    WCHAR szMemoryAmount[MAX_PATH];
    if (SUCCEEDED(FormatMemoryAmountText(pInitData->iMemoryAmount, szMemoryAmount, ARRAYSIZE(szMemoryAmount))))
    {
        if (IsWindows2000Resource() || !g_settings.fCoalesceClockSpeedAndRam || s_iIdCtrlClockSpeedLabel == -1)
        {
            _SetMachineInfoLine(hDlg, idCtrlCur++, szMemoryAmount, FALSE);
        }
        else
        {
            // Coalesce the processor clock speed and RAM amount onto the same line, like Windows XP.
            WCHAR szCoalescenceBuffer[MAX_PATH];
            GetDlgItemTextW(hDlg, s_iIdCtrlClockSpeedLabel, szCoalescenceBuffer, ARRAYSIZE(szCoalescenceBuffer));
            StringCchCatW(szCoalescenceBuffer, ARRAYSIZE(szCoalescenceBuffer), L", ");
            StringCchCatW(szCoalescenceBuffer, ARRAYSIZE(szCoalescenceBuffer), szMemoryAmount);
            _SetMachineInfoLine(hDlg, s_iIdCtrlClockSpeedLabel, szCoalescenceBuffer, FALSE);
        }
    }

    // Physical address extension
    if (!g_settings.fHidePae)
    {
        LSTATUS lStatus = RegOpenKey(
            HKEY_LOCAL_MACHINE,
            L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Memory Management",
            &hKey
        );
        if (ERROR_SUCCESS == lStatus)
        {
            DWORD paeEnabled;
            DWORD cbData = sizeof(paeEnabled);
            lStatus = SHRegGetValueW(hKey, 
                                NULL,
                                L"PhysicalAddressExtension",
                                SRRF_RT_REG_DWORD,
                                NULL,
                                (LPBYTE)&paeEnabled,
                                &cbData);

            if (ERROR_SUCCESS == lStatus &&
                sizeof(paeEnabled) == cbData &&
                0 != paeEnabled)
            {
                _SetMachineInfoLine(hDlg, idCtrlCur++, L"Physical Address Extension", FALSE);
            }

            RegCloseKey(hKey);
        }
    }

    AddOEMHyperLinks(hDlg, &idCtrlCur);
}

DWORD WINAPI InitGeneralDlgThread(LPVOID lpParam)
{
    if (!lpParam)
        return -1;

    INITDLGSTRUCT *pData = (INITDLGSTRUCT *)LocalAlloc(LPTR, sizeof(INITDLGSTRUCT));

    if (pData)
    {
        SYSTEM_BASIC_INFORMATION basicInfo;
        NTSTATUS ntStatus = NtQuerySystemInformation(SystemBasicInformation, &basicInfo, sizeof(basicInfo), nullptr);

        if (NT_SUCCESS(ntStatus))
        {
            UINT64 nTotalBytes;
            if (GetPhysicallyInstalledSystemMemory(&nTotalBytes))
            {
                // The rest of the API expects bytes, and this returns kilobytes.
                nTotalBytes *= 1024;

                // XP rounds to the nearest 4 megabytes.
                if (IsWindowsXPResource() || !g_settings.fRamInKilobytes)
                {
                    constexpr int kOneMb = 1048576;
                    double nTotalMegabytes = (double)(nTotalBytes / kOneMb);
                    pData->iMemoryAmount = (INT64)((ceil(ceil(nTotalMegabytes) / 4.0) * 4.0) * kOneMb);
                }
                else
                {
                    pData->iMemoryAmount = nTotalBytes;
                }
            }
        }

        pData->fShowProcessorName = _GetProcessorDescription(&pData->processorInfo, &pData->fShowProcessorClockSpeed);

        PostMessage((HWND)lpParam, WM_SYSCPL_GEN_UPDATECPUINFO, (WPARAM)pData, 0);
    }

    return 0;
}

INT_PTR CALLBACK PhoneSupportProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg) {

        case WM_INITDIALOG:
        {
            HWND hwndEdit = GetDlgItem(hDlg, IDC_PHONESUPPORTTEXT);
            WCHAR oemfile[MAX_PATH];
            WCHAR szText[ 256 ];
            WCHAR szLine[ 12 ];
            LPWSTR pszEnd = szLine + lstrlenW(L"line");

            if (SUCCEEDED(_GetOemFile(oemfile, ARRAYSIZE(oemfile), GETOEMFILE_GETDATA)))
            {
                GetPrivateProfileStringW(L"General", L"Manufacturer", L"",
                                        szText, ARRAYSIZE(szText), oemfile);
                SetWindowTextW(hDlg, szText);

                if (SUCCEEDED(StringCchCopy(szLine, ARRAYSIZE(szLine), L"line")))
                {
                    SendMessageW(hwndEdit, WM_SETREDRAW, FALSE, 0);

                    HRESULT hr = S_OK;
                    for(UINT i = 1; SUCCEEDED(hr); i++)
                    {
                        hr = StringCchPrintfW(pszEnd, ARRAYSIZE(szLine) - lstrlenW(L"line"), TEXT("%u"), i);
                        if (SUCCEEDED(hr))
                        {                            
                            GetPrivateProfileStringW(L"Support Information",
                                                    szLine, L"@", szText, ARRAYSIZE(szText) - 2,
                                                    oemfile);
                            
                            if(!lstrcmpiW(szText, L"@"))
                            {
                                hr = E_FAIL;
                            }
                            else
                            {                                
                                hr = StringCchCatW(szText, ARRAYSIZE(szText), L"\r\n");
                                if (SUCCEEDED(hr))
                                {
                                    
                                    SendMessageW(hwndEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
                                    
                                    SendMessageW(hwndEdit, EM_REPLACESEL, 0, (LPARAM)szText);
                                }
                            }
                        }
                    }
                }

                SendMessageW(hwndEdit, WM_SETREDRAW, TRUE, 0);
            }
            break;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                 case IDOK:
                 case IDCANCEL:
                     EndDialog(hDlg, 0);
                     break;

                 default:
                     return FALSE;
            }
            break;
        }

        default:
            return FALSE;
    }

    return TRUE;
}

INT_PTR CALLBACK GeneralPageDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
        {
            Wh_Log(L"Initialising general page dialog.");

            LPCWSTR idbWindowsImage = IsLowColor(hWnd)
                ? MAKEINTRESOURCEW(IDB_WINDOWS_256)
                : MAKEINTRESOURCEW(IDB_WINDOWS);

            HANDLE hImage = LoadImageW(g_hInstResource, idbWindowsImage, 
                    IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS);

            if (!hImage)
            {
                Wh_Log(L"Failed to load Windows image.");
            }

            SendMessageW(GetDlgItem(hWnd, IDC_GEN_WINDOWS_IMAGE), STM_SETIMAGE, IMAGE_BITMAP, 
                (LPARAM)hImage
            );

            // Fill OS version information:
            int idCtrlCur = IDC_GEN_VERSION_0;
            if (g_settings.pszCustomOsName && g_settings.pszCustomOsName[0])
            {
                if (IsWindowsXPResource())
                {
                    SetDlgItemTextW(hWnd, idCtrlCur++, g_settings.pszCustomOsName);
                }
                else
                {
                    // Windows 2000 used a fixed string at control ID #11.
                    SetDlgItemTextW(hWnd, 11, g_settings.pszCustomOsName);
                }
            }
            else if (IsWindowsXPResource())
            {
                // Otherwise fill the operating system name automatically so that
                // something would display there. Rather than do bloat this codebase
                // any more to make it query the actual current OS name, I'll just
                // hardcode this default value to one from XP.
                SetDlgItemTextW(hWnd, idCtrlCur++, L"Microsoft Windows XP");
            }

            if (g_settings.pszCustomOsVer1 && g_settings.pszCustomOsVer1[0])
            {
                SetDlgItemTextW(hWnd, idCtrlCur++, g_settings.pszCustomOsVer1);
            }

            if (g_settings.pszCustomOsVer2 && g_settings.pszCustomOsVer2[0])
            {
                SetDlgItemTextW(hWnd, idCtrlCur++, g_settings.pszCustomOsVer2);
            }

            if (IsWindowsXPResource() && g_settings.pszCustomOsVer3 && g_settings.pszCustomOsVer3[0])
            {
                SetDlgItemTextW(hWnd, idCtrlCur++, g_settings.pszCustomOsVer3);
            }

            HKEY hkey;
            if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hkey) == ERROR_SUCCESS)
            {
                // Do registered user info
                UINT idCtrlCur = IsWindowsXPResource() ? IDC_GEN_REGISTERED_0 : IDC_GEN_2K_REGISTERED_0;
                WCHAR szBuffer[MAX_PATH] = { 0 };
                DWORD cbData = 0;

                cbData = sizeof(szBuffer);
                if((SHRegGetValueW(hkey, NULL, L"RegisteredOwner",
                    SRRF_RT_REG_SZ | SRRF_RT_REG_EXPAND_SZ | SRRF_NOEXPAND, NULL, (LPBYTE)szBuffer, &cbData) == ERROR_SUCCESS) &&
                    (cbData > 1))
                {
                    SetDlgItemTextW(hWnd, idCtrlCur++, szBuffer);
                }

                cbData = sizeof(szBuffer);
                if((SHRegGetValueW(hkey, NULL, L"RegisteredOrganization",
                    SRRF_RT_REG_SZ | SRRF_RT_REG_EXPAND_SZ | SRRF_NOEXPAND, NULL, (LPBYTE)szBuffer, &cbData) == ERROR_SUCCESS) &&
                    (cbData > 1))
                {
                    SetDlgItemTextW(hWnd, idCtrlCur++, szBuffer);
                }

                cbData = sizeof(szBuffer);
                if((SHRegGetValueW(hkey, NULL, L"ProductId",
                    SRRF_RT_REG_SZ | SRRF_RT_REG_EXPAND_SZ | SRRF_NOEXPAND, NULL, (LPBYTE)szBuffer, &cbData) == ERROR_SUCCESS) &&
                    (cbData > 1))
                {
                    SetDlgItemTextW(hWnd, idCtrlCur++, szBuffer);
                }

                cbData = sizeof(szBuffer);
                if((SHRegGetValueW(hkey, NULL, L"Plus! ProductId",
                                SRRF_RT_REG_SZ | SRRF_RT_REG_EXPAND_SZ | SRRF_NOEXPAND, NULL, (LPBYTE)szBuffer, &cbData) == ERROR_SUCCESS) &&
                    (cbData > 1))
                {
                    SetDlgItemTextW(hWnd, idCtrlCur++, szBuffer);
                }

                RegCloseKey(hkey);
            }

            SHCreateThread(InitGeneralDlgThread, hWnd, CTF_COINIT | CTF_FREELIBANDEXIT, nullptr);

            return TRUE;
        }

        case WM_SYSCPL_GEN_UPDATECPUINFO:
        {
            if (wParam)
            {
                CompleteGeneralDlgInitialization(hWnd, (INITDLGSTRUCT *)wParam);
                LocalFree((HLOCAL)wParam);
            }

            return TRUE;
        }

        case WM_COMMAND:
        {
            int idcGenOemSupport = IsWindowsXPResource()
                ? IDC_GEN_OEM_SUPPORT
                : IDC_GEN_2K_OEM_SUPPORT;

            if (wParam == idcGenOemSupport)
            {
                DialogBoxW(g_hInstResource, MAKEINTRESOURCE(IDD_PHONESUPPORT), GetParent(hWnd), 
                    PhoneSupportProc);
            }

            return TRUE;
        }
    }

    return FALSE;
}

INT_PTR CALLBACK NoGeneralResourcePageDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
        {
            return TRUE;
        }
    }

    return FALSE;
}

struct NoResourcePageDlgTemplate
{
    DLGTEMPLATE dlgTemplate = {
        .style = DS_SETFONT | WS_CHILD | WS_DISABLED | WS_CAPTION,
        .dwExtendedStyle = 0,
        .cdit = 1,
        .x = 0,
        .y = 0,
        .cx = 256,
        .cy = 226,
    };

    WORD idMenu = 0;
    WORD idDialogClass = 0;
    WCHAR szTitle[23] = L"General (needs setup!)";
    WORD sFontPoint = 8;
    WCHAR szFont[7] = L"Tahoma";

    DLGITEMTEMPLATE labelItem = {
        .x = 20,
        .y = 20,
        .cx = 256 - 20,
        .cy = 226 - 20,
        .style = WS_VISIBLE | WS_CHILD,
        .id = (WORD)-1, // IDC_STATIC
    };

    WORD labelIdClass = 0xFFFF;
    WORD labelIdControl = 0x0082; // Static control
    WCHAR label_szTitle[177] = L"In order to display the contents of this page correctly, "
                               L"you need to configure the mod settings as detailed in the README "
                               L"of the \"System Properties 'General' Tab\" Windhawk mod.";
    WORD dummy = 0;
    WORD cd = 0;
};

HPROPSHEETPAGE CreateGeneralPage()
{
    PROPSHEETPAGE psp;
    psp.dwSize = sizeof(psp);
    psp.dwFlags = PSP_DEFAULT;
    psp.hInstance = g_hInstResource;
    psp.pszTemplate = MAKEINTRESOURCEW(IDD_GENERALTAB);
    psp.pfnDlgProc = GeneralPageDlgProc;

    if (!FindResourceW(g_hInstResource, MAKEINTRESOURCE(IDD_GENERALTAB), RT_DIALOG))
    {
        // Tell the user that they don't have the mod set up sufficiently.
        NoResourcePageDlgTemplate *dlgTemplate = new NoResourcePageDlgTemplate();

        psp.dwFlags = PSP_DLGINDIRECT;
        psp.hInstance = nullptr;
        psp.pResource = (DLGTEMPLATE *)dlgTemplate;
        psp.pszTitle = L"General (needs setup!)";
        psp.pfnDlgProc = NoGeneralResourcePageDlgProc;
    }

    return CreatePropertySheetPageW(&psp);
}

bool g_fHasAlreadyOpened = false;

using PropertySheetW_t = decltype(&PropertySheetW);
PropertySheetW_t PropertySheetW_orig;
INT_PTR WINAPI PropertySheetW_hook(LPCPROPSHEETHEADERW pPropSheet)
{
    Wh_Log(L"Entered function."); // This is useful for debugging purposes, even by the end user.

    // If we've marked that we're already open, then let other property
    // sheets be created without modification. This is to avoid redoing
    // the changes extraneously on child dialogs.
    if (g_fHasAlreadyOpened)
    {
        return PropertySheetW_orig(pPropSheet);
    }

    // Create the "general" tab.
    HPROPSHEETPAGE hpspGeneral = CreateGeneralPage();

    // Clone the PROPSHEETHEADER to edit configuration:
    PROPSHEETHEADERW psh = *pPropSheet;

    // Prepare a copy of the pages array.
    HPROPSHEETPAGE rghpsps[16] = { 0 };
    rghpsps[0] = hpspGeneral;

    // Copy the original original contents to our new array.
    for (UINT i = 0; i < psh.nPages; i++)
        rghpsps[i + 1] = psh.phpage[i];

    psh.phpage = rghpsps;
    psh.nPages += 1;

    // Set the default if we're being told to do that at startup.
    if (g_fHijackDefaultTab)
    {
        psh.pStartPage = 0;
    }

    g_fHasAlreadyOpened = true;
    return PropertySheetW_orig(&psh);
}

void LoadSettings()
{
    g_settings.pszXpSysdm = Wh_GetStringSetting(L"xp_sysdm_path");
    g_settings.pszCustomOsName = Wh_GetStringSetting(L"custom_os_name");
    g_settings.pszCustomOsVer1 = Wh_GetStringSetting(L"custom_os_ver1");
    g_settings.pszCustomOsVer2 = Wh_GetStringSetting(L"custom_os_ver2");
    g_settings.pszCustomOsVer3 = Wh_GetStringSetting(L"custom_os_ver3");
    g_settings.fHideAtCompatible = Wh_GetIntSetting(L"hide_at_compatible");
    g_settings.fHidePae = Wh_GetIntSetting(L"hide_pae");
    g_settings.fCoalesceClockSpeedAndRam = Wh_GetIntSetting(L"coalesce_clock_speed_and_ram");
    g_settings.fRamInKilobytes = Wh_GetIntSetting(L"ram_in_kilobytes");
}

bool g_fExecutingInControlExe = false;

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    WCHAR szLoadedModule[MAX_PATH];
    GetModuleFileNameW(GetModuleHandle(0), szLoadedModule, sizeof(szLoadedModule));
    Wh_Log(L"Loaded into module: %s", szLoadedModule);

    if (StrStrI(szLoadedModule, L"\\control.exe") != nullptr)
    {
        // We're loading into control.exe, so do a hack to see what control.exe is trying
        // to bring up and take over if we need to. Otherwise, bail out - we have no
        // work to do in a control.exe instance other than redirection.
        LPCWSTR szCommandLine = GetCommandLineW();
        if (StrStrI(szCommandLine, L"sysdm.cpl") != nullptr)
        {
            Wh_Log(L"Since we're executing in control.exe, we're deferring a process creation "
                   L"for Wh_ModAfterInit.");
            g_fExecutingInControlExe = true;
            return TRUE;
        }

        return FALSE;
    }

    g_hInst = GetModuleHandleW(L"sysdm.cpl");

    if (!g_hInst)
    {
        Wh_Log(L"For some reason, sysdm.cpl isn't loaded. Bailing!");
        return FALSE;
    }

    LPCWSTR szCommandLine = GetCommandLineW();
    if (StrStrIW(szCommandLine, L"/opengeneraltab"))
    {
        g_fHijackDefaultTab = true;
    }

    LoadSettings();

    g_hInstResource = LoadLibraryExW(g_settings.pszXpSysdm, nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE);

    // Get the version of the resource DLL to determine which version of Windows
    // it is from:
    DWORD dwVersionSize = GetFileVersionInfoSizeW(g_settings.pszXpSysdm, nullptr);
    BYTE *pbVersionInfo = new BYTE[dwVersionSize];
    VS_FIXEDFILEINFO *pFileInfo;
    UINT uLenFileInfo = 0;
    if (GetFileVersionInfoW(g_settings.pszXpSysdm, 0, dwVersionSize, pbVersionInfo))
    {
        if (VerQueryValueW(pbVersionInfo, L"\\", (void **)&pFileInfo, &uLenFileInfo))
        {
            g_dwResourceVersion = pFileInfo->dwFileVersionMS;
        }
        else
        {
            Wh_Log(L"Query for file version information failed.");
        }
    }
    else
    {
        Wh_Log(L"Failed to get file version size.");
    }
    delete[] pbVersionInfo;

    WindhawkUtils::Wh_SetFunctionHookT(
        PropertySheetW,
        PropertySheetW_hook,
        &PropertySheetW_orig
    );

    return TRUE;
}

void Wh_ModAfterInit()
{
    if (g_fExecutingInControlExe)
    {
        /*
         * In order to be 100% certain that the Windhawk engine has installed its internal
         * process creation hooks, spawning processes needs to be done from Wh_ModAfterInit.
         *
         * Barring this case, the mod still works for the most part, but there are cases
         * where it will consistently fail to inject, such as in the case of running the
         * program as administrator.
         */

        Wh_Log(L"Opening sysdm.cpl from control.exe.");

        // control.exe is hardcoded to open SystemPropertiesComputerName.exe from the
        // system folder in Windows 10. We'll repeat this behaviour, but pass an
        // argument so we know to change the default tab.
        WCHAR szPath[MAX_PATH];
        GetSystemDirectoryW(szPath, ARRAYSIZE(szPath));
        StringCchCatW(szPath, ARRAYSIZE(szPath), L"\\SystemPropertiesComputerName.exe");
        ShellExecuteW(
            nullptr,
            L"open",
            szPath,
            L"/opengeneraltab",
            nullptr,
            SW_SHOW
        );

        TerminateProcess(GetCurrentProcess(), 0);
    }
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
    Wh_FreeStringSetting(g_settings.pszXpSysdm);
    Wh_FreeStringSetting(g_settings.pszCustomOsName);
    Wh_FreeStringSetting(g_settings.pszCustomOsVer1);
    Wh_FreeStringSetting(g_settings.pszCustomOsVer2);
    Wh_FreeStringSetting(g_settings.pszCustomOsVer3);
}
