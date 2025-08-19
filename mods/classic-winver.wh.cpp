// ==WindhawkMod==
// @id              classic-winver
// @name            Classic Winver
// @description     Allows you to restore the Winver dialog from various versions of Windows
// @version         1.1.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lshell32 -lversion -lgdi32 -lshlwapi -lntdll
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Winver
This mod allows you to replace the Winver dialog with one from an older version of Windows
and to customize certain things inside of it.

## How to use
You will need a copy of shell32.dll from the Windows version which you want to restore the about
dialog from, and if you are using a version from Windows Vista or above, you will also need its MUI
file (e.g. `en-US\shell32.dll.mui`). The following versions will also need extra files:
- **Windows XP SP1+** needs `xpsp1res.dll` for the updated dialog layout. Otherwise, the layout from
  Windows XP RTM will be used.
- **Windows XP Professional x64 Edition** and **Windows Server 2003** need `msgina.dll` for branding
  bitmaps. Otherwise, the dialog will fail to display.

Once you have the necessary files, enter their paths into the mod's settings. From here you can
configure the other options to your liking.

## Windows 9x dialog
This mod replaces the dialog opened by the `ShellAbout` API, which is what Windows NT's `winver.exe` calls.
If you want the Windows 9x Winver applet like the image below, you may use [winver9x](https://github.com/aubymori/winver9x)
alongside this mod.

![winver9x](https://raw.githubusercontent.com/aubymori/winver9x/refs/heads/main/screenshot.png)

## Branding for Vista+
If you are using a Vista+ dialog, and want to change the branding, this is the recommended method:

1. Make sure you have the System CPL from Brawllux's [Control Panel Restoration Pack](https://winclassic.net/thread/1779/restoring-control-panel-pages-links),
   otherwise the System CPL page will be broken.
2. Replace the following files with their counterparts from the version you want:
   - The entire `%SystemRoot%\Branding` folder
   - `%SystemRoot%\System32\winbrand.dll`
   - `%SystemRoot%\SysWOW64\winbrand.dll`

**NOTE**: winbrand.dll from certain beta builds may fail to load, and in that case, you should instead
use it from an adjacent retail build.

## Previews

| ![Windows 95](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/win95.png) | ![Windows NT 4.0](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/winnt4.png) |
|-|-|
| ![Windows Me](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/winme.png) | ![Windows 2000](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/win2k.png) |
| ![Windows XP](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/winxp.png) | ![Windows Server 2003](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/winsrv03.png) |
| ![Windows Vista](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/winvista.png) | ![Windows 7](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/win7.png) |
| ![Windows 8 Developer Preview](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/win8dp.png) | ![Windows 8.1](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/win81.png) |
| ![Windows 10 1507](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/win10_1507.png) | ![Windows 10 1809](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-winver/win10_1809.png) |
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- shell_dll_path: ""
  $name: Path to shell32.dll
  $description: Absolute path to shell32.dll from the Windows version which you want to restore the about dialog from.
    Leave blank to use shell32.dll from System32.
- xpsp1res_dll_path: ""
  $name: Path to xpsp1res.dll
  $description: "Absolute path to xpsp1res.dll from the Windows XP service pack which you want to restore the about dialog from.
    Only used for XP style; leave blank if you do not want the SP1+ about dialog."
- msgina_dll_path: ""
  $name: Path to msgina.dll
  $description: "Absolute path to msgina.dll from the Windows version which you want to restore the about dialog from.
    Only used for Server 2003 style."
- physical_memory: -1
  $name: Physical memory
  $description: "The physical memory, in KB, to display in the about dialog. Use -1 to get the actual value.
    Only displayed in Windows Vista style and before."
- system_resources_free: -1
  $name: System resources free
  $description: The percentage of system resources free to display in the about dialog. Use -1 to get the
    actual value. Only displayed in Windows 9x and Me styles.
- version_text: Windows 95
  $name: Version text
  $description: The version text to display in the about dialog. Only displayed in Windows 9x and Me styles.
- branding: professional
  $name: Branding
  $description: "Which branding bitmaps to use. This only applies on the Windows XP style.
    Your shell32.dll may not have the appropriate bitmaps for some of these options."
  $options:
  - professional: Professional
  - home: Home
  - embedded: Embedded
  - server: Server Standard Edition
  - srv_enterprise: Server Enterprise Edition
  - srv_datacenter: Server Datacenter Edition
  - sbs: Small Business Server
  - srv_web: Server Web Edition
- top_line_text: ""
  $name: Top line text
  $description: "The text to display on the top line of the dialog. Leave blank to use the default value.
    Only applies to the Windows Vista style and after."
- show_confidential_text: false
  $name: Show Microsoft Confidential text
  $description: Show the red Microsoft Confidential text in the about dialog. Only applies to the Windows 7/8 style
    and on shell32.dll from beta builds of Windows.
- copyright_years: 1981-2001
  $name: Copyright years
  $description: Copyright years to display in the about dialog. Only applies to Windows XP style.
- display_version: ""
  $name: Display version
  $description: Display version to use in the about dialog (e.g. 22H2). Leave blank to use the default value.
    Only applies to Windows 10 style.
- major_version: -1
  $name: Major version
  $description: The major version to display in the about dialog. Use -1 to get the actual value. Won't
    display in Windows 9x, Me, and 10 styles.
- minor_version: -1
  $name: Minor version
  $description: The minor version to display in the about dialog. Use -1 to get the actual value. Won't
    display in Windows 9x, Me, and 10 styles.
- build_number: -1
  $name: Build number
  $description: The build number to display in the about dialog. Use -1 to get the actual value. Won't
    display in Windows 9x, Me, and 10 styles.
- ubr: -1
  $name: Sub-build number
  $description: The sub-build number to display in the About dialog. Use -1 to get the actual value. Only
    displays in the Windows 10 style.
- build_lab: ""
  $name: Build lab
  $description: The build lab (e.g. xpclient.010817-1148) to display in the about dialog. Leave blank to
    use the default value. Only applies in Windows XP style or with BuildLabEx enabled. 
- use_build_lab_ex: false
  $name: Use BuildLabEx
  $description: If using the default build lab, use BuildLabEx instead of BuildLab, like beta builds of Windows.
    Also will show the build lab regardless of Winver style.
- csd_version: USE_DEFAULT
  $name: CSD version
  $description: The CSD version (service pack string) to display in the about dialog. Use "USE_DEFAULT"
    to get the actual value. Won't display in Windows 9x, Me, and 10 styles.
- debug_string: USE_DEFAULT
  $name: Debug string
  $description: The debug string to display in the about dialog. Use "USE_DEFAULT"
    to get the actual value. Won't display in Windows 9x, and Me styles.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <richedit.h>
#include <shlwapi.h>

EXTERN_C NTSYSAPI NTSTATUS NTAPI RtlGetVersion(PRTL_OSVERSIONINFOW lpVersionInformation);

// WinBrand functions we use to get branding bitmaps and strings for Vista+
LPWSTR (WINAPI *BrandingFormatString)(LPCWSTR pszFormat);
HANDLE (WINAPI *BrandingLoadImage)(LPCWSTR pszBrand, UINT uID, UINT type, int cx, int cy, UINT  fuLoad);

// Dialog IDs
#define DLG_ABOUT                   0x3810
#define DLG_EULA                    0x3815

// String IDs
#define IDS_VERSIONMSG        60
#define IDS_DEBUG             61
#define IDS_LDK               62
#define IDS_PERCENTFREE       63
#define IDS_CONFIDENTIAL      62000

// shell32.dll bitmap IDs
#define IDB_WINDOWS             130
#define IDB_ABOUT256            131
#define IDB_ABOUTBAND256        138
#define IDB_ABOUTPERSONAL256    147
#define IDB_ABOUTEMBEDDED256    149
#define IDB_ABOUTBLADE256       151
#define IDB_ABOUTSBS256         153
#define IDB_ABOUTSRV256         155
#define IDB_ABOUTENT256         157
#define IDB_ABOUTDCS256         159

// msgina.dll bitmap IDs
#define IDB_SMALL_PRO_8                 101
#define IDB_SMALL_SRV_8                 130
#define IDB_SMALL_ADV_8                 132
#define IDB_SMALL_DCS_8                 134
#define IDB_SMALL_SBS_8                 146
#define IDB_SMALL_BLA_8                 147

// basebrd.dll bitmap IDs
#define IDB_BASEBRD_ABOUT               101
#define IDB_BASEBRD_BAND                111
#define IDB_BASEBRD_ABOUT_NEW           121

// Control IDs
#define IDD_ICON                0x3009
#define IDD_LINE_1              0x3327
#define IDD_LINE_2              0x3328
#define IDD_LINE_3              0x3329
#define IDD_APPNAME                 0x3500
#define IDD_CONFIG                  0x3501
#define IDD_CONVTITLE               0x3502
#define IDD_CONVENTIONAL            0x3503
#define IDD_EMSFREE                 0x3504
#define IDD_SDTEXT                  0x3505
#define IDD_SDUSING                 0x3506
#define IDD_USERNAME                0x3507
#define IDD_COMPANYNAME             0x3508
#define IDD_SERIALNUM               0x3509
#define IDD_COPYRIGHTSTRING         0x350a
#define IDD_VERSION                 0x350b
#define IDD_EMSTEXT                 0x350c
#define IDD_OTHERSTUFF              0x350d
#define IDD_DOSVER                  0x350e
#define IDD_PROCESSOR               0x350f
#define IDD_PRODUCTID               0x3510
#define IDD_OEMID                   0x3511
#define IDD_EULA                    0x3512
#define IDD_TRADEMARKS              0x3513
#define IDD_CONFIDENTIAL            0x3514

HMODULE g_hmShell    = NULL;
HMODULE g_hmXPSP1Res = NULL;
HMODULE g_hmMSGina   = NULL;

enum WINVERVERSION
{
    WVV_UNKNOWN = 0,
    WVV_WIN9X,
    WVV_WINNT4,
    WVV_WINME,
    WVV_WIN2K,
    WVV_WINXP,
    WVV_WINSRV03,
    WVV_WINVISTA,
    WVV_WIN7_8_TH1,
    WVV_WIN10,
} g_winverVersion = WVV_UNKNOWN;

// Branding for XP/Server 2003 style
enum WINVERBRAND
{
    WVB_PROFESSIONAL = 0,
    WVB_SERVER,
    WVB_HOME,
    WVB_EMBEDDED,
    WVB_SRV_ENTERPRISE,
    WVB_SRV_DATACENTER,
    WVB_SBS,
    WVB_SRV_WEB,
} g_winverBrand = WVB_PROFESSIONAL;

// LARP values
DWORD g_dwPhysicalMemory      =    -1;
DWORD g_dwSystemResourcesFree =    -1;
DWORD g_dwUBR                 =    -1;
RTL_OSVERSIONINFOW g_osvi     = { 0 };

bool  g_fShowConfidentialText = false;
bool  g_fUseBuildLabEx        = false;

WindhawkUtils::StringSetting
    g_spszShellDllPath, g_spszXPSP1ResDllPath, g_spszMSGinaDllPath,
    g_spszCopyrightYears, g_spszDisplayVersion, g_spszBuildLab, g_spszTopLineText,
    g_spszDebugString, g_spszVersionText;

// From the shell DLL's version info, determine which version of Winver the user wants
WINVERVERSION DetermineWinverVersion(VS_FIXEDFILEINFO *pVerInfo)
{
    WORD wMajor = HIWORD(pVerInfo->dwFileVersionMS);
    WORD wMinor = LOWORD(pVerInfo->dwFileVersionMS);

    // shell32.dll's version from 98 through XP is weird in that it seems to follow
    // the Internet Explorer version instead of the OS version. This means the
    // following shell32.dll versions are mismatched from their OS versions:
    //
    // OS                                        | OS version | shell32.dll version
    // ------------------------------------------+------------+--------------------
    // Windows 95 and NT 4.0 with desktop update | 4.0        | 4.72
    // Windows 98                                | 4.10       | 4.72
    // Windows Me                                | 4.90       | 5.50
    // Windows XP                                | 5.1/5.2    | 6.0
    switch (wMajor)
    {
        case 4:
            // Pre-desktop update Windows 95/NT4 shell DLL
            if (wMinor == 0)
            {
                if (pVerInfo->dwFileOS & VOS_NT)
                    return WVV_WINNT4;
                else
                    return WVV_WIN9X;
            }
            // IE4 shell DLL, which can be either from Windows 95 or NT 4.0 with
            // the desktop update, as well as Windows 98. These all have VOS_NT set,
            // so we have to distinguish NT and 9x by dialog layout. See _InitAboutDlg.
            else
            {
                return WVV_WIN9X;
            }
            break;
        case 5:
            // We don't check for VOS_NT here because for some reason,
            // 98 and Me's shell32.dll both have it set. Luckily, Windows
            // 2000 has version 5.0 while Me has version 5.50.
            if (wMinor == 0)
                return WVV_WIN2K;
            else
                return WVV_WINME;
            break;
        case 6:
            if (wMinor == 0)
            {
                // We must resolve XP/Vista difference through
                // build number. See the comment at the beginning
                // of this function.
                if (HIWORD(pVerInfo->dwFileVersionLS) >= 6000)
                    return WVV_WINVISTA;
                else if (HIWORD(pVerInfo->dwFileVersionLS) <= 2900)
                    return WVV_WINXP;
                else
                    return WVV_WINSRV03;
            }
            else
            {
                // Windows 7 and 8.x do Winver pretty much the exact
                // same way.
                return WVV_WIN7_8_TH1;
            }
            break;
        case 10:
            // So does Windows 10 before 1511.
            // Source for build number used: https://betawiki.net/wiki/Windows_10_build_10563#Branding
            if (HIWORD(pVerInfo->dwFileVersionLS) < 10563)
                return WVV_WIN7_8_TH1;
            else
                return WVV_WIN10;
            break;
        default:
            return WVV_UNKNOWN;
    }

    return WVV_UNKNOWN;
}

typedef struct {
    HICON   hIcon;
    LPCWSTR szApp;
    LPCWSTR szOtherStuff;
    int     dpiWindow;
    HBITMAP hbmAbout;
    HBITMAP hbmBand;
    SIZE    sizeAbout;
    SIZE    sizeScaled;
    SIZE    sizeOffsetOld;
} ABOUT_PARAMS, *LPABOUT_PARAMS;

DWORD RegGetStringAndRealloc(HKEY hkey, LPCWSTR lpszValue, LPWSTR *lplpsz, LPDWORD lpSize)
{
    DWORD       err;
    DWORD       dwSize;
    DWORD       dwType;
    LPTSTR      lpszNew;

    *lplpsz[0] = L'\0';        // In case of error

    dwSize = *lpSize;
    err = SHQueryValueExW(hkey, (LPTSTR)lpszValue, 0, &dwType, (LPBYTE)*lplpsz, &dwSize);
    if (err == ERROR_MORE_DATA)
    {
        lpszNew = (LPWSTR)LocalReAlloc((HLOCAL)*lplpsz, dwSize, LMEM_MOVEABLE);
        if (lpszNew)
        {
            *lplpsz = lpszNew;
            *lpSize = dwSize;
            err = SHQueryValueExW(hkey, (LPTSTR)lpszValue, 0, &dwType, (LPBYTE)*lplpsz, &dwSize);
        }
    }

    return err;
}


#define BytesToK(pDW)   (*(pDW) = (*(pDW) + 512) / 1024)

#define MAX_INT64_SIZE  30
#define MAX_COMMA_NUMBER_SIZE   (MAX_INT64_SIZE + 10)

/**************************************************************************
// Converts 64 bit Int to Str
**************************************************************************/
void Int64ToStr( __int64 n, LPWSTR lpBuffer)
{
    WCHAR    szTemp[MAX_INT64_SIZE];
    __int64  iChr;

    iChr = 0;

    do {
        szTemp[iChr++] = L'0' + (WCHAR)(n % 10);
        n = n / 10;
    } while (n != 0);

    do {
        iChr--;
        *lpBuffer++ = szTemp[iChr];
    } while (iChr != 0);

    *lpBuffer++ = '\0';
}

//
//  Obtain NLS info about how numbers should be grouped.
//
//  The annoying thing is that LOCALE_SGROUPING and NUMBERFORMAT
//  have different ways of specifying number grouping.
//
//          LOCALE      NUMBERFMT      Sample   Country
//
//          3;0         3           1,234,567   United States
//          3;2;0       32          12,34,567   India
//          3           30           1234,567   ??
//
//  Not my idea.  That's the way it works.
//
//  Bonus treat - Win9x doesn't support complex number formats,
//  so we return only the first number.
//
UINT GetNLSGrouping(void)
{
    UINT grouping;
    LPWSTR psz;
    WCHAR szGrouping[32];

    // If no locale info, then assume Western style thousands
    if (!GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, szGrouping, ARRAYSIZE(szGrouping)))
        return 3;

    grouping = 0;
    psz = szGrouping;
    for (;;)
    {
        if (*psz == L'0') break;             // zero - stop

        else if ((UINT)(*psz - L'0') < 10)   // digit - accumulate it
            grouping = grouping * 10 + (UINT)(*psz - L'0');

        else if (*psz)                      // punctuation - ignore it
            { }

        else                                // end of string, no "0" found
        {
            grouping = grouping * 10;       // put zero on end (see examples)
            break;                          // and finished
        }

        psz++;
    }
    return grouping;
}

// takes a DWORD add commas etc to it and puts the result in the buffer
STDAPI_(LPWSTR) AddCommas64(LONGLONG n, LPWSTR pszResult, UINT cchResult)
{
    WCHAR  szTemp[MAX_COMMA_NUMBER_SIZE];
    WCHAR  szSep[5];
    NUMBERFMT nfmt;

    nfmt.NumDigits=0;
    nfmt.LeadingZero=0;
    nfmt.Grouping = GetNLSGrouping();
    GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, szSep, ARRAYSIZE(szSep));
    nfmt.lpDecimalSep = nfmt.lpThousandSep = szSep;
    nfmt.NegativeOrder= 0;

    Int64ToStr(n, szTemp);

    if (GetNumberFormatW(LOCALE_USER_DEFAULT, 0, szTemp, &nfmt, pszResult, cchResult) == 0)
    {
        wcscpy_s(pszResult, cchResult, szTemp);    // ok to truncate, for display only
    }

    return pszResult;
}

// Get branding bitmap ID for XP/Server 2003
int GetBrandingIDB(HMODULE *phm)
{
    if (g_winverVersion == WVV_WINSRV03)
    {
        *phm = g_hmMSGina;
        int idBrand = IDB_SMALL_PRO_8;
        switch (g_winverBrand)
        {
            case WVB_PROFESSIONAL:
                idBrand = IDB_SMALL_PRO_8;
                break;
            case WVB_SERVER:
                idBrand = IDB_SMALL_SRV_8;
                break;
            case WVB_SRV_ENTERPRISE:
                idBrand = IDB_SMALL_ADV_8;
                break;
            case WVB_SRV_DATACENTER:
                idBrand = IDB_SMALL_DCS_8;
                break;
            case WVB_SBS:
                idBrand = IDB_SMALL_SBS_8;
                break;
            case WVB_SRV_WEB:
                idBrand = IDB_SMALL_BLA_8;
                break;
        }

        // If the chosen branding doesn't exist in our msgina.dll, just display Professional.
        if (!FindResourceW(g_hmMSGina, MAKEINTRESOURCEW(idBrand), RT_BITMAP))
            return IDB_SMALL_PRO_8;
        return idBrand;
    }

    *phm = g_hmShell;
    int idBrand = IDB_ABOUT256;
    switch (g_winverBrand)
    {
        case WVB_PROFESSIONAL:
            idBrand = IDB_ABOUT256;
            break;
        case WVB_SERVER:
            idBrand = IDB_ABOUTSRV256;
            break;
        case WVB_HOME:
            idBrand = IDB_ABOUTPERSONAL256;
            break;
        case WVB_EMBEDDED:
            idBrand = IDB_ABOUTEMBEDDED256;
            break;
        case WVB_SRV_ENTERPRISE:
            idBrand = IDB_ABOUTENT256;
            break;
        case WVB_SRV_DATACENTER:
            idBrand = IDB_ABOUTDCS256;
    }

    // If the chosen branding doesn't exist in our shell32.dll, just display Professional.
    if (!FindResourceW(g_hmShell, MAKEINTRESOURCEW(idBrand), RT_BITMAP))
        return IDB_ABOUT256;
    return idBrand;
}

void FreeAboutBitmaps(LPABOUT_PARAMS lpap)
{
    if (lpap->hbmAbout)
    {
        DeleteObject(lpap->hbmAbout);
        lpap->hbmAbout = NULL;
    }
    if (lpap->hbmBand)
    {
        DeleteObject(lpap->hbmBand);
        lpap->hbmBand = NULL;
    }
}

void LoadAboutBitmaps(LPABOUT_PARAMS lpap)
{
    FreeAboutBitmaps(lpap);

    switch (g_winverVersion)
    {
        case WVV_WIN9X:
        case WVV_WINNT4:
            // The Windows logo is only shown with no app icon
            if (!lpap->hIcon)
                lpap->hbmAbout = (HBITMAP)LoadImageW(g_hmShell, MAKEINTRESOURCE(IDB_WINDOWS),
                                IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
            break;
        case WVV_WINME:
        case WVV_WIN2K:
            lpap->hbmAbout = (HBITMAP)LoadImageW(g_hmShell, MAKEINTRESOURCEW(IDB_ABOUT256), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
            lpap->hbmBand = (HBITMAP)LoadImageW(g_hmShell, MAKEINTRESOURCEW(IDB_ABOUTBAND256), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
            break;
        case WVV_WINXP:
        case WVV_WINSRV03:
        {
            HMODULE hmBrand;
            int idBrand = GetBrandingIDB(&hmBrand);
            lpap->hbmAbout = (HBITMAP)LoadImageW(hmBrand, MAKEINTRESOURCEW(idBrand), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
            lpap->hbmBand = (HBITMAP)LoadImageW(g_hmShell, MAKEINTRESOURCEW(IDB_ABOUTBAND256), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
            break;
        }
        case WVV_WINVISTA:
        {
            HBITMAP hbmAbout = (HBITMAP)BrandingLoadImage(L"Basebrd", IDB_BASEBRD_ABOUT, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
            // If the Vista image doesn't exist, use the 7 one.
            if (!hbmAbout)
                hbmAbout = (HBITMAP)BrandingLoadImage(L"Basebrd", IDB_BASEBRD_ABOUT_NEW, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
            lpap->hbmAbout = hbmAbout;
            lpap->hbmBand = (HBITMAP)BrandingLoadImage(L"Basebrd", IDB_BASEBRD_BAND, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
            break;
        }
        case WVV_WIN7_8_TH1:
        case WVV_WIN10:
        {
            HBITMAP hbmAbout = (HBITMAP)BrandingLoadImage(L"Basebrd", IDB_BASEBRD_ABOUT_NEW, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
            if (hbmAbout)
            {
                lpap->hbmAbout = hbmAbout;
                BITMAP bm;
                GetObjectW(hbmAbout, sizeof(bm), &bm);
                lpap->sizeAbout.cx = bm.bmWidth;
                lpap->sizeAbout.cy = bm.bmHeight;

                int dpiSystem = GetDpiForSystem();
                if (dpiSystem == lpap->dpiWindow || !dpiSystem)
                {
                    lpap->sizeScaled = lpap->sizeAbout;
                }
                else
                {
                    lpap->sizeScaled.cx = MulDiv(lpap->sizeAbout.cx, lpap->dpiWindow, dpiSystem);
                    lpap->sizeScaled.cy = MulDiv(lpap->sizeAbout.cy, lpap->dpiWindow, dpiSystem);
                }
            }
            break;
        }
    }
}

VOID MoveChildren(HWND hWnd, INT dx, INT dy)
{
    HWND hWndSibling;
    RECT rc;

    //
    // walk all the children in the dialog adjusting their positions
    // by the delta.
    //

    for ( hWndSibling = GetWindow(hWnd, GW_CHILD) ; hWndSibling ; hWndSibling = GetWindow(hWndSibling, GW_HWNDNEXT))
    {
        GetWindowRect(hWndSibling, &rc);
        MapWindowPoints(NULL, GetParent(hWndSibling), (LPPOINT)&rc, 2);
        OffsetRect(&rc, dx, dy);

        SetWindowPos(hWndSibling, NULL,
                     rc.left, rc.top, 0, 0,
                     SWP_NOZORDER|SWP_NOSIZE);
    }

    //
    // having done that then lets adjust the parent size accordingl.
    //

    GetWindowRect(hWnd, &rc);
    MapWindowPoints(NULL, GetParent(hWnd), (LPPOINT)&rc, 2);

    SetWindowPos(hWnd, NULL,
                 0, 0, (rc.right-rc.left)+dx, (rc.bottom-rc.top)+dy,
                 SWP_NOZORDER|SWP_NOMOVE);
}

// Shift the dialog items to accommodate the about banner for 7+
void ApplyLayout(HWND hwnd, LPABOUT_PARAMS lpap)
{
    RECT rcClient, rcWindow;
    GetClientRect(hwnd, &rcClient);
    GetWindowRect(hwnd, &rcWindow);

    int dx = 0;
    int dy = lpap->sizeScaled.cy;
    if (lpap->sizeScaled.cx > rcWindow.right)
        dx = lpap->sizeScaled.cx - rcWindow.right;
    
    MoveChildren(hwnd, dx - lpap->sizeOffsetOld.cx, dy - lpap->sizeOffsetOld.cy);
    lpap->sizeOffsetOld.cx = dx;
    lpap->sizeOffsetOld.cy = dy;

    ShowWindow(
        GetDlgItem(hwnd, IDD_LINE_1),
        lpap->hbmAbout ? SW_NORMAL : SW_HIDE
    );
}

void _InitAboutDlg(HWND hwnd, LPABOUT_PARAMS lpap)
{
    WCHAR szBuffer[64];
    WCHAR szTemp[64];
    WCHAR szTitle[64];
    WCHAR szMessage[200];
    WCHAR szNumBuf1[32];

    // If we have a IE4 shell DLL, we can't determine whether it's from 9x or NT
    // based on version information alone, so we check for the presence of the
    // "System Resources" dialog item (only present on 9x) to determine if we're
    // 9x or NT.
    if ((g_winverVersion == WVV_WIN9X) && !GetDlgItem(hwnd, IDD_EMSFREE))
    {
        g_winverVersion = WVV_WINNT4;
    }

    lpap->dpiWindow = GetDpiForWindow(hwnd);

    // If you have a string like test1#test2, then the dialog's title will be
    // test1, and the appname line will say test2. The original XP code szApp
    // to be writable simply due to Microsoft's laziness, but I've modified it
    // to not do that.
    LPCWSTR pszApp = lpap->szApp;
    for (LPWSTR lpTemp = (LPTSTR)lpap->szApp; 1 ; lpTemp = CharNext(lpTemp))
    {
        if (*lpTemp == TEXT('\0'))
        {
            GetWindowTextW(hwnd, szBuffer, ARRAYSIZE(szBuffer));
            swprintf_s(szTitle, szBuffer, (LPTSTR)lpap->szApp);
            SetWindowTextW(hwnd, szTitle);
            break;
        }
        if (*lpTemp == L'#')
        {
            size_t cchAppTitle = ((size_t)lpTemp - (size_t)lpap->szApp) / sizeof(WCHAR);
            LPWSTR pszAppTitle = (LPWSTR)LocalAlloc(LPTR, (cchAppTitle + 1) * sizeof(WCHAR));
            if (pszAppTitle)
            {
                wcsncpy(pszAppTitle, lpap->szApp, cchAppTitle);
                SetWindowTextW(hwnd, pszAppTitle);
                LocalFree(pszAppTitle);
            }
            pszApp = ++lpTemp;
            break;
        }
    }

    // Set up app name text
    if (g_winverVersion <= WVV_WINSRV03)
    {
        GetDlgItemTextW(hwnd, IDD_APPNAME, szBuffer, ARRAYSIZE(szBuffer));
        swprintf_s(szTitle, szBuffer, pszApp);
        SetDlgItemTextW(hwnd, IDD_APPNAME, szTitle);
    }
    else if (lpap->szApp != pszApp)
    {
        SetDlgItemTextW(hwnd, IDD_APPNAME, pszApp);
    }

    SetDlgItemTextW(hwnd, IDD_OTHERSTUFF, lpap->szOtherStuff);

    SendDlgItemMessageW(hwnd, IDD_ICON, STM_SETICON, (WPARAM)lpap->hIcon, 0L);
    if (!lpap->hIcon)
        ShowWindow(GetDlgItem(hwnd, IDD_ICON), SW_HIDE);

    // Set up branding text for Vista+
    if (g_winverVersion >= WVV_WINVISTA)
    {
        if (lpap->szApp == pszApp)
        {
            if (g_spszTopLineText.get()[0])
            {
                SetDlgItemTextW(hwnd, IDD_APPNAME, g_spszTopLineText.get());
            }
            else
            {
                LPWSTR pszFormatted = BrandingFormatString(L"%MICROSOFT_COMPANYNAME% %WINDOWS_GENERIC%");
                if (pszFormatted)
                {
                    SetDlgItemTextW(hwnd, IDD_APPNAME, pszFormatted);
                    GlobalFree(pszFormatted);
                }
            }
        }

        LPWSTR pszFormatted = BrandingFormatString(L"%WINDOWS_COPYRIGHT%");
        if (pszFormatted)
        {
            SetDlgItemTextW(hwnd, IDD_COPYRIGHTSTRING, pszFormatted);
            GlobalFree(pszFormatted);
        }

        WCHAR szTrademarks[512];
        GetDlgItemTextW(hwnd, IDD_TRADEMARKS, szTrademarks, ARRAYSIZE(szTrademarks));
        pszFormatted = BrandingFormatString(szTrademarks);
        if (pszFormatted)
        {
            SetDlgItemTextW(hwnd, IDD_TRADEMARKS, pszFormatted);
            GlobalFree(pszFormatted);
        }
    }

    // Version text (e.g. Windows 95) for 9x versions
    if (g_winverVersion == WVV_WIN9X || g_winverVersion == WVV_WINME)
        SetDlgItemTextW(hwnd, IDD_VERSION, g_spszVersionText.get());

    // Legacy NT version text, e.g. Version 4.0 (Build 1381: Service Pack 6)
    if ((g_winverVersion == WVV_WINNT4 || g_winverVersion >= WVV_WIN2K)
    && g_winverVersion < WVV_WIN10)
    {
        LoadStringW(g_hmShell, IDS_VERSIONMSG, szBuffer, ARRAYSIZE(szBuffer));

        szTitle[0] = L'\0';
        if (g_osvi.szCSDVersion[0])
        {
            swprintf_s(szTitle, L": %s", g_osvi.szCSDVersion);
        }

        if (g_fUseBuildLabEx || g_winverVersion == WVV_WINXP || g_winverVersion == WVV_WINSRV03)
        {
            WCHAR szBuildLab[64];
            szBuildLab[0] = L'\0';
            if (g_spszBuildLab.get()[0])
            {
                wcscpy_s(szBuildLab, g_spszBuildLab.get());
            }
            else
            {
                HKEY hkey;
                if (ERROR_SUCCESS == RegOpenKeyExW(
                    HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                    0, KEY_READ, &hkey))
                {
                    LPWSTR lpszValue = NULL;
                    DWORD  cb, err;
                    cb = 256;
                    if (NULL != (lpszValue = (LPWSTR)LocalAlloc(LPTR, cb)))
                    {
                        err = RegGetStringAndRealloc(hkey, g_fUseBuildLabEx ? L"BuildLabEx" : L"BuildLab", &lpszValue, &cb);
                        if (!err)
                        {
                            LPWSTR pszDot = wcschr(lpszValue, L'.');
                            if (pszDot)
                                wcscpy_s(szBuildLab, pszDot + 1);
                        }
                        LocalFree(lpszValue);
                    }
                    RegCloseKey(hkey);
                }
            }

            if (szBuildLab[0])
            {
                wcscpy_s(szTemp, szTitle);
                swprintf_s(
                    szTitle,
                    szTemp[0] ? L".%s %s" : L".%s",
                    szBuildLab, szTemp
                );
            }
        }

        szNumBuf1[0] = L'\0';
        if (wcscmp(g_spszDebugString.get(), L"USE_DEFAULT"))
        {
            wcscpy_s(szNumBuf1, g_spszDebugString.get());
        }
        else if (GetSystemMetrics(SM_DEBUG))
        {
            szNumBuf1[0] = L' ';
            LoadStringW(g_hmShell, IDS_DEBUG, &szNumBuf1[1], ARRAYSIZE(szNumBuf1) - 1);
        }
        swprintf_s(
            szMessage, szBuffer,
            g_osvi.dwMajorVersion,
            g_osvi.dwMinorVersion,
            g_osvi.dwBuildNumber,
            szTitle,
            szNumBuf1
        );
        SetDlgItemTextW(hwnd, IDD_VERSION, szMessage);
    }
    // Modern Win10 version text, e.g. Version 21H2 (OS Build 19044.46651)
    else if (g_winverVersion == WVV_WIN10)
    {
        LoadStringW(g_hmShell, IDS_VERSIONMSG, szBuffer, ARRAYSIZE(szBuffer));

        DWORD dwUBR = g_dwUBR;

        HKEY hkey;
        if (ERROR_SUCCESS == RegOpenKeyExW(
            HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            0, KEY_READ, &hkey))
        {
            LPWSTR lpszValue = NULL;
            DWORD  cb, err;
            cb = 256;
            if (NULL != (lpszValue = (LPWSTR)LocalAlloc(LPTR, cb)))
            {
                DWORD cbUBR = sizeof(DWORD);
                if (dwUBR == (DWORD)-1)
                    RegQueryValueExW(hkey, L"UBR", nullptr, nullptr, (LPBYTE)&dwUBR, &cbUBR);

                szTitle[0] = L'\0';
                if (g_spszDisplayVersion.get()[0])
                {
                    wcscpy_s(szTitle, g_spszDisplayVersion.get());
                }
                else
                {
                    err = RegGetStringAndRealloc(hkey, L"DisplayVersion", &lpszValue, &cb);
                    if (err)
                        err = RegGetStringAndRealloc(hkey, L"ReleaseID", &lpszValue, &cb);

                    if (!err)
                        wcscpy_s(szTitle, lpszValue);
                }

                LocalFree(lpszValue);
            }

            swprintf_s(szTemp, L"%d", g_osvi.dwBuildNumber);

            RegCloseKey(hkey);
        }

        szNumBuf1[0] = L'\0';
        if (GetSystemMetrics(SM_DEBUG))
        {
            szNumBuf1[0] = L' ';
            LoadStringW(g_hmShell, IDS_DEBUG, &szNumBuf1[1], ARRAYSIZE(szNumBuf1) - 1);
        }

        swprintf_s(
            szMessage, szBuffer,
            szTitle,
            szTemp,
            dwUBR,
            szNumBuf1
        );
        SetDlgItemTextW(hwnd, IDD_VERSION, szMessage);
    }

    // Set up copyright string for XP and Srv03. In the real deal, this
    // string was a static string obtained from common.ver.
    if (g_winverVersion == WVV_WINXP
    || g_winverVersion == WVV_WINSRV03)
    {
        GetDlgItemTextW(hwnd, IDD_COPYRIGHTSTRING, szTemp, ARRAYSIZE(szTemp));
        swprintf_s(szBuffer, szTemp, g_spszCopyrightYears.get());
        SetDlgItemTextW(hwnd, IDD_COPYRIGHTSTRING, szBuffer);
    }

    // Set registered owner and organization
    HKEY hkey;
    if (ERROR_SUCCESS == RegOpenKeyExW(
        HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
        0, KEY_READ, &hkey))
    {
        LPWSTR lpszValue = NULL;
        DWORD  cb, err;

        cb = 256;
        if (NULL != (lpszValue = (LPWSTR)LocalAlloc(LPTR, cb)))
        {
            err = RegGetStringAndRealloc(hkey, L"RegisteredOwner", &lpszValue, &cb);
            if (!err)
                SetDlgItemText(hwnd, IDD_USERNAME, lpszValue);

            err = RegGetStringAndRealloc(hkey, L"RegisteredOrganization", &lpszValue, &cb);
            if (!err)
                SetDlgItemText(hwnd, IDD_COMPANYNAME, lpszValue);

            LocalFree(lpszValue);
        }

        RegCloseKey(hkey);
    }

    // Set up "Physical memory available to Windows" text
    if (g_winverVersion <= WVV_WINVISTA)
    {
        DWORDLONG ullTotalPhys = g_dwPhysicalMemory;
        if (g_dwPhysicalMemory == (DWORD)-1)
        {
            MEMORYSTATUSEX MemoryStatus;
            MemoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&MemoryStatus);
            ullTotalPhys = MemoryStatus.ullTotalPhys;
            BytesToK(&ullTotalPhys);
        }
        
        WCHAR szldK[16];
        AddCommas64(ullTotalPhys, szNumBuf1, ARRAYSIZE(szNumBuf1));

        if (g_winverVersion == WVV_WINVISTA)
        {
            LPWSTR pszWindows = BrandingFormatString(L"%WINDOWS_GENERIC%");
            if (pszWindows)
            {
                WCHAR szFormat[512];
                GetDlgItemTextW(hwnd, IDD_CONVENTIONAL, szFormat, ARRAYSIZE(szFormat));
                swprintf_s(szBuffer, szFormat, pszWindows, szNumBuf1);
                SetDlgItemTextW(hwnd, IDD_CONVENTIONAL, szBuffer);
                GlobalFree(pszWindows);
            }
        }
        else
        {
            LoadStringW(g_hmShell, IDS_LDK, szldK, ARRAYSIZE(szldK));
            swprintf_s(szBuffer, szldK, szNumBuf1);
            SetDlgItemTextW(hwnd, IDD_CONVENTIONAL, szBuffer);
        }
    }

    // Set up "System Resources: X% Free" (text)
    // (this is memory)
    if (g_winverVersion <= WVV_WINME && g_winverVersion != WVV_WINNT4)
    {
        DWORD dwSysResource = g_dwSystemResourcesFree;
        if (g_dwSystemResourcesFree == (DWORD)-1)
        {
            // Slight implementation fix here: the NT implementation of the function
            // 9x used (SHGetAboutInformation) does not invert the dwMemoryLoad
            // percentage, which would cause this dialog to display the amount of memory
            // used, and not free. I'm not sure this ever mattered since this metric wasn't
            // displayed in the NT dialog, but it's worth noting.
            MEMORYSTATUS MemoryStatus;
            GlobalMemoryStatus(&MemoryStatus);
            dwSysResource = 100 - MemoryStatus.dwMemoryLoad;
        }

        LoadStringW(g_hmShell, IDS_PERCENTFREE, szBuffer, ARRAYSIZE(szBuffer));
        swprintf_s(szMessage, szBuffer, dwSysResource);
        SetDlgItemTextW(hwnd, IDD_EMSFREE, szMessage);
    }

    if (g_fShowConfidentialText && g_winverVersion == WVV_WIN7_8_TH1)
    {
        WCHAR szConfidential[450];
        LoadStringW(g_hmShell, IDS_CONFIDENTIAL, szConfidential, ARRAYSIZE(szConfidential));
        SetDlgItemTextW(hwnd, IDD_CONFIDENTIAL, szConfidential);
    }

    LoadAboutBitmaps(lpap);
    if (g_winverVersion >= WVV_WIN7_8_TH1)
        ApplyLayout(hwnd, lpap);
}

INT_PTR CALLBACK LicenseDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
            _InitAboutDlg(hwnd, (LPABOUT_PARAMS)lParam);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)lParam);
            return TRUE;
        case WM_COMMAND:
            EndDialog(hwnd, TRUE);
            return TRUE;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            LPABOUT_PARAMS lpap = (LPABOUT_PARAMS)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

            if (g_winverVersion <= WVV_WINNT4)
            {
                if (lpap->hbmAbout)
                {
                    HDC hdcMem = CreateCompatibleDC(hdc);
                    if (hdcMem)
                    {
                        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, lpap->hbmAbout);
                        if (hbmOld)
                        {
                            // Some modified code here to "support" DPI, the bitmap
                            // ends up coming out super jagged and crappy looking,
                            // but I don't really care...
                            int x, y, cx, cy;
                            if (g_winverVersion == WVV_WINNT4)
                            {
                                // The windows/nt bitmap is a little bigger!
                                x = 8; y = 10; cx = 68; cy = 78;
                            }
                            else
                            {
                                x = 10; y = 10; cx = 64; cy = 64;
                            }

                            int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
                            x = MulDiv(x, dpi, 96);
                            y = MulDiv(y, dpi, 96);
                            int scx = MulDiv(cx, dpi, 96);
                            int scy = MulDiv(cy, dpi, 96);

                            StretchBlt(hdc, x, y, scx, scy, hdcMem, 0, 0, cx, cy, SRCCOPY);
                            SelectObject(hdcMem, hbmOld);
                        }
                        DeleteDC(hdcMem);
                    }
                }
            }
            else if (g_winverVersion >= WVV_WINME && g_winverVersion <= WVV_WINVISTA)
            {
                HDC hdcMem = CreateCompatibleDC(hdc);
                if (hdcMem)
                {
                    RECT rc;
                    GetClientRect(hwnd, &rc);
                    int cxDlg = rc.right;

                    int cxDest = MulDiv(413,cxDlg,413);
                    int cyDest = MulDiv(72,cxDlg,413);

                    if (lpap->hbmAbout)
                    {
                        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, lpap->hbmAbout);
                        if (hbmOld)
                        {
                            StretchBlt(hdc, 0, 0, cxDest, cyDest, hdcMem, 0,0,413,72, SRCCOPY);
                            SelectObject(hdcMem, hbmOld);
                        }
                    }

                    if (lpap->hbmBand)
                    {
                        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, lpap->hbmBand);
                        if (hbmOld)
                        {
                            StretchBlt(hdc, 0, cyDest, cxDest, MulDiv(5,cxDlg,413), hdcMem, 0,0,413,5, SRCCOPY);
                            SelectObject(hdcMem, hbmOld);
                        }
                    }

                    DeleteDC(hdcMem);
                }
            }
            else
            {
                if (lpap->hbmAbout)
                {
                    HDC hdcMem = CreateCompatibleDC(hdc);
                    if (hdcMem)
                    {
                        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, lpap->hbmAbout);
                        if (hbmOld)
                        {
                            int xDest = 0;
                            RECT rcClient;
                            GetClientRect(hwnd, &rcClient);
                            if (rcClient.right > lpap->sizeScaled.cx)
                                xDest = (rcClient.right - lpap->sizeScaled.cx) / 2;

                            HIGHCONTRASTW hc = { sizeof(hc) };
                            SystemParametersInfoW(SPI_GETHIGHCONTRAST, 0, &hc, FALSE);

                            if (hc.dwFlags & HCF_HIGHCONTRASTON)
                            {
                                StretchBlt(
                                    hdc,
                                    xDest, 0,
                                    lpap->sizeScaled.cx,
                                    lpap->sizeScaled.cy,
                                    hdcMem,
                                    0, 0,
                                    lpap->sizeAbout.cx,
                                    lpap->sizeAbout.cy,
                                    SRCCOPY
                                );
                            }
                            else
                            {
                                GdiTransparentBlt(
                                    hdc,
                                    xDest, 0,
                                    lpap->sizeScaled.cx,
                                    lpap->sizeScaled.cy,
                                    hdcMem,
                                    0, 0,
                                    lpap->sizeAbout.cx,
                                    lpap->sizeAbout.cy,
                                    RGB(255, 255, 255)
                                );
                            }

                            SelectObject(hdc, hbmOld);
                        }

                        DeleteDC(hdcMem);
                    }
                }
            }

            EndPaint(hwnd, &ps);
            return TRUE;
        }
        // Color the Microsoft Confidential text red.
        case WM_CTLCOLORSTATIC:
        {
            if ((HWND)lParam == GetDlgItem(hwnd, IDD_CONFIDENTIAL))
            {
                SetTextColor((HDC)wParam, RGB(255, 0, 0));
                SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNFACE));
                return (INT_PTR)(HBRUSH)(COLOR_BTNFACE + 1);
            }
            break;
        }
        case WM_NOTIFY:
            if (IDD_EULA == (int)wParam
            && NM_CLICK == ((LPNMHDR)lParam)->code)
            {
                if (g_winverVersion <= WVV_WINSRV03)
                {
                    SHELLEXECUTEINFOW sei = { 0 };
                    sei.cbSize = sizeof(sei);
                    sei.fMask = SEE_MASK_DOENVSUBST;
                    sei.hwnd = hwnd;
                    sei.nShow = SW_SHOWNORMAL;
                    sei.lpFile = L"%windir%\\system32\\eula.txt";
                    ShellExecuteExW(&sei);
                }
                else
                {
                    HMODULE hmEdit = LoadLibraryExW(L"msftedit.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
                    if (hmEdit)
                    {
                        DialogBoxParamW(g_hmShell, MAKEINTRESOURCEW(DLG_EULA), hwnd, LicenseDlgProc, NULL);
                        FreeLibrary(hmEdit);
                    }
                }
                return TRUE;
            }
            break;
        case WM_DPICHANGED:
        {
            if (g_winverVersion >= WVV_WIN7_8_TH1)
            {
                LPABOUT_PARAMS lpap = (LPABOUT_PARAMS)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
                lpap->dpiWindow = GetDpiForWindow(hwnd);
                LoadAboutBitmaps(lpap);
                ApplyLayout(hwnd, lpap);
            }
            return TRUE;
        }
        // Vista+ normally does this in _InitAboutDlg, but that doesn't seem to work there for me.
        case WM_SHOWWINDOW:
            if (g_winverVersion >= WVV_WINVISTA && wParam)
                SetFocus(GetDlgItem(hwnd, IDOK));
            return TRUE;
        case WM_DESTROY:
        {
            LPABOUT_PARAMS lpap = (LPABOUT_PARAMS)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            if (lpap)
                FreeAboutBitmaps(lpap);
            return TRUE;
        }
    }

    return FALSE;
}

#define IDD_EULABOX 101

INT_PTR CALLBACK LicenseDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
        {
            // Newer versions of Windows use a function from winbrand to get the EULA filename
            // in the current language, but we can't do that since it doesn't exist in older versions
            // of winbrand.dll, so we just do what Vista did instead. I'm sure it's fine...
            WCHAR szEULAFile[MAX_PATH];
            ExpandEnvironmentStringsW(L"%SystemRoot%\\System32\\license.rtf", szEULAFile, ARRAYSIZE(szEULAFile));
            BOOL fSucceeded = FALSE;
            HANDLE hFile = CreateFileW(szEULAFile, GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                LARGE_INTEGER liFileSize;
                if (GetFileSizeEx(hFile, &liFileSize))
                {
                    LPVOID lpBuffer = LocalAlloc(LPTR, liFileSize.LowPart + sizeof(WCHAR));
                    if (lpBuffer)
                    {
                        DWORD dwRead;
                        if (ReadFile(hFile, lpBuffer, liFileSize.LowPart, &dwRead, 0) && dwRead == liFileSize.LowPart)
                        {
                            HWND hwndText = GetDlgItem(hwnd, IDD_EULABOX);
                            SETTEXTEX SetTextEx;
                            SetTextEx.flags = ST_SELECTION;
                            SetTextEx.codepage = CP_ACP;
                            SendMessageW(hwndText, EM_EXLIMITTEXT, 0, 0);
                            SendMessageW(hwndText, EM_SETTEXTEX, (WPARAM)&SetTextEx, (LPARAM)lpBuffer);
                            SendMessageW(hwndText, EM_SETSEL, 0, 0);
                            fSucceeded = TRUE;
                        }
                        LocalFree(lpBuffer);
                    }
                }
                CloseHandle(hFile);
            }
            if (!fSucceeded)
                PostMessageW(hwnd, WM_COMMAND, IDCANCEL, 0);
            return TRUE;
        }
        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hwnd, 0);
            }
            break;
    }

    return FALSE;
}

VS_FIXEDFILEINFO *GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen) 
{ 
    void *pFixedFileInfo = nullptr; 
    UINT uPtrLen = 0; 

    HRSRC hResource = 
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION); 
    if (hResource)
    { 
        HGLOBAL hGlobal = LoadResource(hModule, hResource); 
        if (hGlobal)
        { 
            void *pData = LockResource(hGlobal); 
            if (pData)
            { 
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen)
                || uPtrLen == 0)
                { 
                    pFixedFileInfo = nullptr; 
                    uPtrLen = 0; 
                } 
            } 
        } 
    } 

    if (puPtrLen)
    { 
        *puPtrLen = uPtrLen; 
    } 

    return (VS_FIXEDFILEINFO *)pFixedFileInfo; 
}

using ShellAboutW_t = decltype(&ShellAboutW);
ShellAboutW_t ShellAboutW_orig;
INT WINAPI ShellAboutW_hook(HWND hWnd, LPCWSTR szApp, LPCWSTR szOtherStuff, HICON hIcon)
{
    BOOL fRet = TRUE;
    VS_FIXEDFILEINFO *pVerInfo;
    HMODULE hmDialog;
    ACTCTXW actCtx = { 0 };
    HANDLE hActCtx;
    ULONG_PTR ulCookie;
    HMODULE hComCtl;

    if (g_spszShellDllPath.get()[0])
        g_hmShell = LoadLibraryExW(g_spszShellDllPath.get(), NULL, LOAD_LIBRARY_AS_DATAFILE);
    else
        g_hmShell = GetModuleHandleW(L"shell32.dll");
    if (!g_hmShell)
    {
        WCHAR szError[256];
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
            GetLastError(), NULL, szError, 256, nullptr);
        WCHAR szMessage[256];
        swprintf_s(
            szMessage,
            L"Failed to load shell32.dll: %s"
            L"\n"
            L"Make sure your shell32.dll path is correct and that the file there "
            L"is not corrupt.",
            szError);
        MessageBoxW(
            hWnd, szMessage, L"Windhawk: Classic Winver", MB_ICONERROR);
        fRet = FALSE;
        goto cleanup;
    }

    pVerInfo = GetModuleVersionInfo(g_hmShell, nullptr);
    if (!pVerInfo)
    {
        MessageBoxW(
            hWnd,
            L"Failed to get version info from shell32.dll.",
            L"Windhawk: Classic Winver",
            MB_ICONERROR
        );
        fRet = FALSE;
        goto cleanup;
    }

    g_winverVersion = DetermineWinverVersion(pVerInfo);
    if (g_winverVersion == WVV_UNKNOWN)
    {
        WCHAR szMessage[256];
        swprintf_s(
            szMessage,
            L"Failed to determine Winver version from file version %u.%u.%u.%u",
            HIWORD(pVerInfo->dwFileVersionMS),
            LOWORD(pVerInfo->dwFileVersionMS),
            HIWORD(pVerInfo->dwFileVersionLS),
            LOWORD(pVerInfo->dwFileVersionLS)
        );
        MessageBoxW(
            hWnd,
            szMessage,
            L"Windhawk: Classic Winver",
            MB_ICONERROR
        );
        fRet = FALSE;
        goto cleanup;
    }

    if (g_winverVersion == WVV_WINXP)
        g_hmXPSP1Res = LoadLibraryExW(g_spszXPSP1ResDllPath.get(), NULL, LOAD_LIBRARY_AS_DATAFILE);

    hmDialog = g_hmShell;
    if (g_winverVersion == WVV_WINXP && g_hmXPSP1Res)
    {
        hmDialog = g_hmXPSP1Res;
    }

    if (g_winverVersion == WVV_WINSRV03)
    {
        g_hmMSGina = LoadLibraryExW(g_spszMSGinaDllPath.get(), NULL, LOAD_LIBRARY_AS_DATAFILE);
        if (!g_hmMSGina)
        {
            WCHAR szError[256];
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
                GetLastError(), NULL, szError, 256, nullptr);
            WCHAR szMessage[256];
            swprintf_s(
                szMessage,
                L"Failed to load msgina.dll: %s"
                L"\n"
                L"Make sure your msgina.dll path is correct and that the file there "
                L"is not corrupt.",
                szError);
            MessageBoxW(
                hWnd, szMessage, L"Windhawk: Classic Winver", MB_ICONERROR);
            fRet = FALSE;
            goto cleanup;
        }
    }

    if (!FindResourceW(hmDialog, MAKEINTRESOURCEW(DLG_ABOUT), RT_DIALOG))
    {
        WCHAR szMessage[256];
        swprintf_s(
            szMessage,
            L"Failed to find the about dialog in the specified copy of %s.dll",
            (hmDialog == g_hmXPSP1Res) ? L"xpsp1res" : L"shell32"
        );
        MessageBoxW(
            hWnd,
            szMessage,
            L"Windhawk: Classic Winver",
            MB_ICONERROR
        );
        fRet = FALSE;
        goto cleanup;
    }

    ABOUT_PARAMS ap;
    ZeroMemory(&ap, sizeof(ap));
    ap.hIcon = hIcon;
    ap.szApp = szApp;
    ap.szOtherStuff = szOtherStuff;

    // We need to use an activation context to act as shell32.dll or else
    // comctl32 v5 apps will crash/fail when attempting to display the dialog
    actCtx.cbSize = sizeof(actCtx);
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    actCtx.hModule = GetModuleHandleW(L"shell32.dll");
    hActCtx = CreateActCtxW(&actCtx);
    if (hActCtx != INVALID_HANDLE_VALUE)
    {
        ActivateActCtx(hActCtx, &ulCookie);
        hComCtl = LoadLibraryExW(L"comctl32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    // Also set DPI awareness like modern ShellAbout does
    DPI_AWARENESS_CONTEXT dpiOld;
    dpiOld = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    fRet = DialogBoxParamW(
        hmDialog, MAKEINTRESOURCEW(DLG_ABOUT), hWnd, AboutDlgProc,
        (LPARAM)&ap
    );
    SetThreadDpiAwarenessContext(dpiOld);

    if (hActCtx != INVALID_HANDLE_VALUE)
    {
        DeactivateActCtx(0, ulCookie);
        ReleaseActCtx(hActCtx);
        FreeLibrary(hComCtl);
    }

cleanup:
    if (g_hmShell)
    {
        FreeLibrary(g_hmShell);
        g_hmShell = NULL;
    }
    if (g_hmXPSP1Res)
    {
        FreeLibrary(g_hmXPSP1Res);
        g_hmXPSP1Res = NULL;
    }
    if (g_hmMSGina)
    {
        FreeLibrary(g_hmMSGina);
        g_hmMSGina = NULL;
    }
    if (actCtx.hModule)
    {
        FreeLibrary(actCtx.hModule);
        actCtx.hModule = NULL;
    }

    g_winverVersion = WVV_UNKNOWN;

    return fRet;
}

void Wh_ModSettingsChanged(void)
{
    g_spszShellDllPath    = WindhawkUtils::StringSetting::make(L"shell_dll_path");
    g_spszXPSP1ResDllPath = WindhawkUtils::StringSetting::make(L"xpsp1res_dll_path");
    g_spszMSGinaDllPath   = WindhawkUtils::StringSetting::make(L"msgina_dll_path");
    g_spszCopyrightYears  = WindhawkUtils::StringSetting::make(L"copyright_years");
    g_spszDisplayVersion  = WindhawkUtils::StringSetting::make(L"display_version");
    g_spszBuildLab        = WindhawkUtils::StringSetting::make(L"build_lab");
    g_spszTopLineText     = WindhawkUtils::StringSetting::make(L"top_line_text");
    g_spszDebugString     = WindhawkUtils::StringSetting::make(L"debug_string");
    g_spszVersionText     = WindhawkUtils::StringSetting::make(L"version_text");

    g_dwPhysicalMemory      = Wh_GetIntSetting(L"physical_memory");
    g_dwSystemResourcesFree = Wh_GetIntSetting(L"system_resources_free");
    g_dwUBR                 = Wh_GetIntSetting(L"ubr");
    g_fShowConfidentialText = Wh_GetIntSetting(L"show_confidential_text");
    g_fUseBuildLabEx        = Wh_GetIntSetting(L"use_build_lab_ex");

    g_osvi.dwOSVersionInfoSize = sizeof(g_osvi);
    RtlGetVersion(&g_osvi);

    DWORD dwMajor = Wh_GetIntSetting(L"major_version");
    if (dwMajor != (DWORD)-1)
        g_osvi.dwMajorVersion = dwMajor;

    DWORD dwMinor = Wh_GetIntSetting(L"minor_version");
    if (dwMinor != (DWORD)-1)
        g_osvi.dwMinorVersion = dwMinor;

    DWORD dwBuild = Wh_GetIntSetting(L"build_number");
    if (dwBuild != (DWORD)-1)
        g_osvi.dwBuildNumber = dwBuild;

    LPCWSTR pszCSDVer = Wh_GetStringSetting(L"csd_version");
    if (wcscmp(pszCSDVer, L"USE_DEFAULT"))
        wcscpy_s(g_osvi.szCSDVersion, pszCSDVer);
    Wh_FreeStringSetting(pszCSDVer);

    LPCWSTR pszBranding = Wh_GetStringSetting(L"branding");
    const struct
    {
        LPCWSTR pszString;
        WINVERBRAND wvbValue;
    } c_rgBrandMap[] = {
        { L"professional",   WVB_PROFESSIONAL   },
        { L"home",           WVB_HOME           },
        { L"embedded",       WVB_EMBEDDED       },
        { L"server",         WVB_SERVER         },
        { L"srv_enterprise", WVB_SRV_ENTERPRISE },
        { L"srv_datacenter", WVB_SRV_DATACENTER },
        { L"sbs",            WVB_SBS            },
        { L"srv_web",        WVB_SRV_WEB        },
    };
    for (const auto &mapping : c_rgBrandMap)
    {
        if (!wcscmp(pszBranding, mapping.pszString))
        {
            g_winverBrand = mapping.wvbValue;
            break;
        }
    }
    Wh_FreeStringSetting(pszBranding);
}

BOOL Wh_ModInit(void)
{
    Wh_ModSettingsChanged();

    HMODULE hmWinBrand = LoadLibraryExW(L"winbrand.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hmWinBrand)
    {
        Wh_Log(L"Failed to load winbrand.dll");
        return FALSE;
    }

#define LOAD_WINBRAND_FUNC(name)                                              \
    *(FARPROC *)&name = GetProcAddress(hmWinBrand, #name);                    \
    if (!name)                                                                \
    {                                                                         \
        Wh_Log(L"Failed to get address of %s from winbrand.dll", L ## #name); \
        return FALSE;                                                         \
    }

    LOAD_WINBRAND_FUNC(BrandingFormatString);
    LOAD_WINBRAND_FUNC(BrandingLoadImage);

    // FYI: There is no need to hook ShellAboutA; it is simply
    // a proxy function that converts the strings to UTF-16 LE
    // and calls ShellAboutW

    if (!Wh_SetFunctionHook(
        (void *)ShellAboutW,
        (void *)ShellAboutW_hook,
        (void **)&ShellAboutW_orig
    ))
    {
        Wh_Log(L"Failed to hook ShellAboutW");
        return FALSE;
    }

    return TRUE;
}