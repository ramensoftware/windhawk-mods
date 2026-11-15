// ==WindhawkMod==
// @id              prevista-explorer-statusbar
// @name            Pre-Vista Explorer Status Bar
// @description     Classic status bar for Explorer with XP/2000/98/95 pane styles, zone display
// @version         2.0.0
// @author          arceus413
// @github          https://github.com/arceuss
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lshlwapi -lole32 -loleaut32 -luuid -lgdi32 -lurlmon
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Pre-Vista Explorer Status Bar

Adds a classic status bar to Windows Explorer with selectable pane styles
matching different Windows versions. Based on the other status bar mod by Anixx,
rewritten from XP SP1 / Win2K / 98 source code and decompilations.

Note: Windows already has a built-in status bar, Hide it in folder options for this mod to look proper.

## Pane Styles

- **Windows XP** — Per-filetype infotip on single select (reads `HKCR\{ext}\InfoTip`),
  dynamic pane widths, security zone icon + name
- **Windows 2000** — Simpler infotip (`Type;Author;Title;Subject;Comment;Size`),
  same layout as XP
- **Windows 98** — Fixed-width left/right panes, always shows object count in pane 0,
  file size or drive capacity in pane 1
- **Windows 95/NT4** — Two panes only (count + size), no zone pane, no infotip

## Screenshots

**XP/2000 style:**
![XP/2000 style](https://i.imgur.com/5ZmPXuc.png)

**98 style:**
![98 style](https://i.imgur.com/lpHSWgY.png)

**95/NT4 style:**
![95/NT4 style](https://i.imgur.com/dmcjYwW.png)

## Features

- Three-pane layout: object count / file size / security zone
- Locale-aware number formatting with thousands separators
- Hidden file count display (XP/2000 no-selection state)
- Disk free space in explorer mode (tree pane visible)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PaneStyle: xp
  $name: Pane style
  $description: Status bar pane layout and content style
  $options:
    - xp: Windows XP (per-type infotip, dynamic pane widths, zone)
    - 2000: Windows 2000 (basic infotip, dynamic pane widths, zone)
    - 98: Windows 98 (fixed L/R panes, selection count + file size, zone)
    - 95: Windows 95/NT4 (two panes, count + size, no zone)
- ShowHiddenCount: true
  $name: Show hidden file count
  $description: Show "plus N hidden" in the status bar when hidden files are present
- ObjectWord: objects
  $name: Object word
  $description: The word used for items in the status bar (e.g. "objects" for XP, "object(s)" for 2000/9x)
  $options:
    - objects: objects (Windows XP)
    - object(s): object(s) (Windows 2000/9x)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <propkey.h>
#include <urlmon.h>
#include <wininet.h>

#if !(defined(__clang_major__) && __clang_major__ >= 20)
#include <initguid.h>
const PROPERTYKEY PKEY_Size = {
    { 0xB725F130, 0x47EF, 0x101A, { 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC } }, 12
};
#endif

#define WM_UPDATE_STATUSBAR (WM_USER + 100)
#define WM_RESIZE_PANES     (WM_USER + 101)

// Timer IDs
#define TIMER_RETRY         1
#define TIMER_DEBOUNCE      2
#define DEBOUNCE_MS         50

// XP status bar: 3 panes [items/count | size | zone]
#define STATUS_PANE_ITEMS   0
#define STATUS_PANE_SIZE    1
#define STATUS_PANE_ZONE    2
#define STATUS_PANES        3

// Fallback zone pane width if zone enumeration fails (from XP browseui/globals.h)
#define ZONES_PANE_WIDTH    220

// Maximum number of zone icons to cache
#define MAX_ZONE_ICONS      8

// Maximum zone display name length (from urlmon ZONEATTRIBUTES)
#define MAX_ZONE_DISPLAYNAME 260

// Pane style enum
enum PaneStyle { PANESTYLE_XP = 0, PANESTYLE_2000, PANESTYLE_98, PANESTYLE_95 };
static PaneStyle g_ePaneStyle = PANESTYLE_XP;
static bool g_bShowHiddenCount = true;

// Format strings built from settings — "objects" for XP, "object(s)" for 2000/9x
static wchar_t FMT_OBJECTS[128];
static wchar_t FMT_OBJECTS_HIDDEN[128];
static wchar_t FMT_OBJECTS_SPACE[128];
static wchar_t FMT_OBJECTS_HIDDEN_SPACE[128];
static wchar_t FMT_SELECTED[128];
// 98-only: "Free Space: %s,  Capacity: %s" (IDS 6469)
static wchar_t FMT_FREE_CAPACITY[128];

void BuildFormatStrings() {
    PCWSTR word = Wh_GetStringSetting(L"ObjectWord");
    if (!word || !*word) {
        Wh_FreeStringSetting(word);
        word = nullptr;
    }
    const wchar_t* w = word ? word : L"objects";
    swprintf_s(FMT_OBJECTS,              _countof(FMT_OBJECTS),              L"%%s %s", w);
    swprintf_s(FMT_OBJECTS_HIDDEN,        _countof(FMT_OBJECTS_HIDDEN),        L"%%s %s (plus %%s hidden)", w);
    swprintf_s(FMT_OBJECTS_SPACE,          _countof(FMT_OBJECTS_SPACE),          L"%%s %s (Disk free space: %%s)", w);
    swprintf_s(FMT_OBJECTS_HIDDEN_SPACE,   _countof(FMT_OBJECTS_HIDDEN_SPACE),   L"%%s %s (plus %%s hidden) (Disk free space: %%s)", w);
    swprintf_s(FMT_SELECTED,              _countof(FMT_SELECTED),              L"%%s %s selected", w);
    // 98-style "Free Space: X,  Capacity: Y" for pane 1 (IDS 6469)
    wcscpy_s(FMT_FREE_CAPACITY, L"Free Space: %s,  Capacity: %s");
    if (word) Wh_FreeStringSetting(word);
}

struct ZoneCache {
    HICON icons[MAX_ZONE_ICONS];
    wchar_t names[MAX_ZONE_ICONS][MAX_ZONE_DISPLAYNAME];
    DWORD zoneIds[MAX_ZONE_ICONS];
    DWORD count;
    bool initialized;
};

struct StatusBarData {
    HWND statusBar;
    HWND explorerWnd;
    HWND shellDefView;
    IShellBrowser* pBrowser;
    int retryCount;
    int zonePaneWidth;
    ZoneCache zoneCache;
    HICON currentZoneIcon;
};

LRESULT CALLBACK SubclassShellViewProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

// ---------------------------------------------------------------------------
// Utility
// ---------------------------------------------------------------------------

int GetStatusBarHeight(HWND statusBar) {
    RECT rc;
    GetWindowRect(statusBar, &rc);
    return rc.bottom - rc.top;
}

// Format a number with locale-aware thousands separators (XP's AddCommas equivalent)
void AddCommas(int n, wchar_t* buf, int bufSize) {
    NUMBERFMT numFmt = {0, 0, 3};
    wchar_t decSep[10], thousandSep[10], numBuf[64];
    GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, decSep, 10);
    GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, thousandSep, 10);
    numFmt.lpDecimalSep = decSep;
    numFmt.lpThousandSep = thousandSep;
    swprintf_s(numBuf, L"%d", n);
    if (!GetNumberFormat(LOCALE_USER_DEFAULT, 0, numBuf, &numFmt, buf, bufSize))
        swprintf_s(buf, bufSize, L"%d", n);
}

// ---------------------------------------------------------------------------
// Shell browser acquisition
// ---------------------------------------------------------------------------

IShellBrowser* GetShellBrowser(HWND hwndExplorer) {
    IShellBrowser* pBrowser = nullptr;
    IShellWindows* pShellWindows = nullptr;

    if (FAILED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_LOCAL_SERVER,
                                 IID_IShellWindows, (void**)&pShellWindows)) || !pShellWindows)
        return nullptr;

    long count = 0;
    pShellWindows->get_Count(&count);

    for (long i = 0; i < count && !pBrowser; i++) {
        VARIANT vi = { VT_I4 };
        vi.lVal = i;

        IDispatch* pDisp = nullptr;
        if (SUCCEEDED(pShellWindows->Item(vi, &pDisp)) && pDisp) {
            IWebBrowser2* pWB2 = nullptr;
            if (SUCCEEDED(pDisp->QueryInterface(IID_IWebBrowser2, (void**)&pWB2)) && pWB2) {
                HWND hwnd = NULL;
                if (SUCCEEDED(pWB2->get_HWND((SHANDLE_PTR*)&hwnd)) && hwnd == hwndExplorer) {
                    IServiceProvider* pSP = nullptr;
                    if (SUCCEEDED(pWB2->QueryInterface(IID_IServiceProvider, (void**)&pSP)) && pSP) {
                        pSP->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&pBrowser);
                        pSP->Release();
                    }
                }
                pWB2->Release();
            }
            pDisp->Release();
        }
    }
    pShellWindows->Release();
    return pBrowser;
}

HWND FindShellDefView(HWND hwndExplorer) {
    HWND shellTab = FindWindowEx(hwndExplorer, NULL, L"ShellTabWindowClass", NULL);
    HWND duiView = shellTab ? FindWindowEx(shellTab, NULL, L"DUIViewWndClassName", NULL) : NULL;
    HWND directUI = duiView ? FindWindowEx(duiView, NULL, L"DirectUIHWND", NULL) : NULL;

    if (!directUI) return NULL;

    for (HWND child = NULL; (child = FindWindowEx(directUI, child, NULL, NULL)) != NULL; ) {
        HWND defView = FindWindowEx(child, NULL, L"SHELLDLL_DefView", NULL);
        if (defView) return defView;
    }
    return NULL;
}

int GetItemCount(IShellBrowser* pBrowser) {
    int itemCount = 0;
    if (!pBrowser) return 0;

    IShellView* pView = nullptr;
    if (SUCCEEDED(pBrowser->QueryActiveShellView(&pView)) && pView) {
        IFolderView2* pView2 = nullptr;
        if (SUCCEEDED(pView->QueryInterface(IID_IFolderView2, (void**)&pView2)) && pView2) {
            pView2->ItemCount(SVGIO_ALLVIEW, &itemCount);
            pView2->Release();
        }
        pView->Release();
    }
    return itemCount;
}

// XP's IsExplorerModeBrowser — checks if folder tree (navigation pane) is visible
bool IsExplorerMode(IShellBrowser* pBrowser) {
    if (!pBrowser) return false;
    HWND hwndTree = NULL;
    // FCW_TREE = 3 — standard control window ID for the tree view
    return SUCCEEDED(pBrowser->GetControlWindow(3 /*FCW_TREE*/, &hwndTree)) && hwndTree != NULL;
}

// ---------------------------------------------------------------------------
// Zone support — ported from XP shlwapi/security.cpp and browseui/shbrows2.cpp
// ---------------------------------------------------------------------------

void InitZoneCache(ZoneCache* zc, HWND hwndStatus) {
    if (zc->initialized) return;
    zc->initialized = true;
    zc->count = 0;
    memset(zc->icons, 0, sizeof(zc->icons));

    IInternetZoneManager* pizm = nullptr;
    if (FAILED(CoCreateInstance(CLSID_InternetZoneManager, NULL, CLSCTX_INPROC_SERVER,
                                 IID_IInternetZoneManager, (void**)&pizm)) || !pizm)
        return;

    DWORD dwZoneEnum = 0, dwZoneCount = 0;
    if (SUCCEEDED(pizm->CreateZoneEnumerator(&dwZoneEnum, &dwZoneCount, 0))) {
        for (DWORD i = 0; i < dwZoneCount && zc->count < MAX_ZONE_ICONS; i++) {
            DWORD dwZone = 0;
            pizm->GetZoneAt(dwZoneEnum, i, &dwZone);

            ZONEATTRIBUTES za = {};
            za.cbSize = sizeof(ZONEATTRIBUTES);
            pizm->GetZoneAttributes(dwZone, &za);

            DWORD idx = zc->count;
            zc->zoneIds[idx] = dwZone;
            wcsncpy_s(zc->names[idx], za.szDisplayName, _TRUNCATE);

            // Parse icon path — format is "dll#resid" or direct icon path
            // Ported from XP browseui/commonsb.cpp _GetCachedZoneIconAndName
            HICON hIcon = NULL;
            wchar_t* pHash = wcschr(za.szIconPath, L'#');
            if (pHash) {
                *pHash = L'\0';
                int iIcon = _wtoi(pHash + 1);
                ExtractIconExW(za.szIconPath, -iIcon, NULL, &hIcon, 1);
            }
            zc->icons[idx] = hIcon;
            zc->count++;
        }
        pizm->DestroyZoneEnumerator(dwZoneEnum);
    }
    pizm->Release();
}

void DestroyZoneCache(ZoneCache* zc) {
    for (DWORD i = 0; i < zc->count; i++) {
        if (zc->icons[i])
            DestroyIcon(zc->icons[i]);
    }
    memset(zc, 0, sizeof(ZoneCache));
}

// Compute the zone pane width — ported from XP shlwapi/security.cpp ZoneComputePaneSize
int ComputeZonePaneWidth(HWND hwndStatus, ZoneCache* zc) {
    HDC hdc = GetDC(hwndStatus);
    HFONT hf = (HFONT)SendMessage(hwndStatus, WM_GETFONT, 0, 0);
    HFONT hfPrev = (HFONT)SelectObject(hdc, hf);

    // XP measures the "(Mixed)" string and adds it to the max zone name width
    // (IDS_MIXED from shlwapi resources)
    SIZE sizMixed;
    const wchar_t* szMixed = L" (Mixed)";
    GetTextExtentPoint32W(hdc, szMixed, (int)wcslen(szMixed), &sizMixed);

    int cxZone = 0;
    for (DWORD i = 0; i < zc->count; i++) {
        SIZE siz;
        GetTextExtentPoint32W(hdc, zc->names[i], (int)wcslen(zc->names[i]), &siz);
        if (cxZone < siz.cx)
            cxZone = siz.cx;
    }

    SelectObject(hdc, hfPrev);
    ReleaseDC(hwndStatus, hdc);

    if (cxZone == 0)
        return ZONES_PANE_WIDTH;

    // XP: cxZone + sizMixed.cx + icon + gripper + 4 edges
    return cxZone + sizMixed.cx +
           GetSystemMetrics(SM_CXSMICON) +
           GetSystemMetrics(SM_CXVSCROLL) +
           GetSystemMetrics(SM_CXEDGE) * 4;
}

// Get the zone for the current folder — ported from XP browseui/shbrows2.cpp _GetTempZone
DWORD GetCurrentZone(IShellBrowser* pBrowser) {
    if (!pBrowser) return URLZONE_LOCAL_MACHINE;

    IShellView* pView = nullptr;
    if (FAILED(pBrowser->QueryActiveShellView(&pView)) || !pView)
        return URLZONE_LOCAL_MACHINE;

    IFolderView2* pFV2 = nullptr;
    if (FAILED(pView->QueryInterface(IID_IFolderView2, (void**)&pFV2)) || !pFV2) {
        pView->Release();
        return URLZONE_LOCAL_MACHINE;
    }

    IPersistFolder2* pFolder = nullptr;
    DWORD dwZone = URLZONE_LOCAL_MACHINE;

    if (SUCCEEDED(pFV2->GetFolder(IID_IPersistFolder2, (void**)&pFolder)) && pFolder) {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(pFolder->GetCurFolder(&pidl)) && pidl) {
            wchar_t szPath[MAX_PATH];
            if (SHGetPathFromIDList(pidl, szPath)) {
                // For local filesystem paths, use MapUrlToZone to determine zone
                IInternetSecurityManager* pism = nullptr;
                if (SUCCEEDED(CoCreateInstance(CLSID_InternetSecurityManager, NULL,
                    CLSCTX_INPROC_SERVER, IID_IInternetSecurityManager, (void**)&pism)) && pism) {
                    // Convert path to file:// URL for MapUrlToZone
                    wchar_t szUrl[INTERNET_MAX_URL_LENGTH];
                    DWORD cchUrl = INTERNET_MAX_URL_LENGTH;
                    if (SUCCEEDED(UrlCreateFromPathW(szPath, szUrl, &cchUrl, 0))) {
                        pism->MapUrlToZone(szUrl, &dwZone, 0);
                    }
                    pism->Release();
                }
            }
            ILFree(pidl);
        }
        pFolder->Release();
    }
    pFV2->Release();
    pView->Release();
    return dwZone;
}

// Find zone info in cache and set the zone pane text + icon
void UpdateZonePane(StatusBarData* pData) {
    if (!pData || !pData->statusBar) return;

    InitZoneCache(&pData->zoneCache, pData->statusBar);

    DWORD dwZone = GetCurrentZone(pData->pBrowser);
    const wchar_t* szName = L"";
    HICON hIcon = NULL;

    for (DWORD i = 0; i < pData->zoneCache.count; i++) {
        if (pData->zoneCache.zoneIds[i] == dwZone) {
            szName = pData->zoneCache.names[i];
            hIcon = pData->zoneCache.icons[i];
            break;
        }
    }

    pData->currentZoneIcon = hIcon;
    SendMessage(pData->statusBar, SB_SETTEXTW, STATUS_PANE_ZONE, (LPARAM)szName);
    SendMessage(pData->statusBar, SB_SETICON, STATUS_PANE_ZONE, (LPARAM)hIcon);
}

// ---------------------------------------------------------------------------
// Property mapping — ported from XP shell32/prop.cpp and infotip.cpp
// Maps InfoTip registry "prop:Name;Name;..." to SCIDs and XP display names
// ---------------------------------------------------------------------------

struct PropMapping {
    const wchar_t* name;
    SHCOLUMNID scid;
    const wchar_t* label;
    bool noLabel;
};

// XP's c_rgPropUIInfo table (shell32/prop.cpp) — registry InfoTip property names
// mapped to their SHCOLUMNID (SCID) and XP-era display label strings
static const PropMapping s_propMap[] = {
    // PSGUID_STORAGE {B725F130-47EF-101A-A5F1-02608C9EEBAC}
    { L"Type",       {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 4},  L"Type",          false },
    { L"Size",       {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 12}, L"Size",          false },
    { L"Write",      {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 14}, L"Date Modified", false },
    { L"Create",     {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 15}, L"Date Created",  false },
    { L"Access",     {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 16}, L"Date Accessed", false },
    { L"Attributes", {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 13}, L"Attributes",    false },
    // PSGUID_SUMMARYINFORMATION {F29F85E0-4FF9-1068-AB91-08002B27B3D9}
    { L"DocTitle",    {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 2},  L"Title",    false },
    { L"DocSubject",  {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 3},  L"Subject",  false },
    { L"DocAuthor",   {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 4},  L"Author",   false },
    { L"DocComments", {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 6},  L"",         true  },
    { L"Comments",    {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 6},  L"",         true  },
    // PSFMTID_VERSION {0CEF7D53-FA64-11D1-A203-0000F81FEDEE}
    { L"FileDescription", {{0x0CEF7D53,0xFA64,0x11D1,{0xA2,0x03,0x00,0x00,0xF8,0x1F,0xED,0xEE}}, 3},  L"Description",      false },
    { L"FileVersion",     {{0x0CEF7D53,0xFA64,0x11D1,{0xA2,0x03,0x00,0x00,0xF8,0x1F,0xED,0xEE}}, 4},  L"File Version",     false },
    { L"ProductName",     {{0x0CEF7D53,0xFA64,0x11D1,{0xA2,0x03,0x00,0x00,0xF8,0x1F,0xED,0xEE}}, 7},  L"Product Name",     false },
    { L"ProductVersion",  {{0x0CEF7D53,0xFA64,0x11D1,{0xA2,0x03,0x00,0x00,0xF8,0x1F,0xED,0xEE}}, 8},  L"Product Version",  false },
    // PSGUID_DOCUMENTSUMMARYINFORMATION {D5CDD502-2E9C-101B-9397-08002B2CF9AE}
    { L"Company",     {{0xD5CDD502,0x2E9C,0x101B,{0x93,0x97,0x08,0x00,0x2B,0x2C,0xF9,0xAE}}, 15}, L"Company",  false },
    { L"DocCategory", {{0xD5CDD502,0x2E9C,0x101B,{0x93,0x97,0x08,0x00,0x2B,0x2C,0xF9,0xAE}}, 2},  L"Category", false },
    // Win2K property names (from selfreg.inx: "prop:Type;Author;Title;Subject;Comment;Size")
    { L"Author",      {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 4},  L"Author",   false },
    { L"Title",       {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 2},  L"Title",    false },
    { L"Subject",     {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 3},  L"Subject",  false },
    { L"Comment",     {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 6},  L"",         true  },

    // Modern canonical property names (Windows Vista+) — same SCIDs, XP labels
    { L"System.ItemTypeText",           {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 4},  L"Type",          false },
    { L"System.Size",                   {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 12}, L"Size",          false },
    { L"System.DateModified",           {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 14}, L"Date Modified", false },
    { L"System.DateCreated",            {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 15}, L"Date Created",  false },
    { L"System.DateAccessed",           {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 16}, L"Date Accessed", false },
    { L"System.FileAttributes",         {{0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC}}, 13}, L"Attributes",    false },
    { L"System.Title",                  {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 2},  L"Title",         false },
    { L"System.Subject",                {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 3},  L"Subject",       false },
    { L"System.Author",                 {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 4},  L"Author",        false },
    { L"System.Comment",                {{0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}, 6},  L"",              true  },
    { L"System.FileDescription",        {{0x0CEF7D53,0xFA64,0x11D1,{0xA2,0x03,0x00,0x00,0xF8,0x1F,0xED,0xEE}}, 3},  L"Description",   false },
    { L"System.FileVersion",            {{0x0CEF7D53,0xFA64,0x11D1,{0xA2,0x03,0x00,0x00,0xF8,0x1F,0xED,0xEE}}, 4},  L"File Version",  false },
    { L"System.Software.ProductName",   {{0x0CEF7D53,0xFA64,0x11D1,{0xA2,0x03,0x00,0x00,0xF8,0x1F,0xED,0xEE}}, 7},  L"Product Name",  false },
    { L"System.Software.ProductVersion",{{0x0CEF7D53,0xFA64,0x11D1,{0xA2,0x03,0x00,0x00,0xF8,0x1F,0xED,0xEE}}, 8},  L"Product Version",false },
    { L"System.Company",                {{0xD5CDD502,0x2E9C,0x101B,{0x93,0x97,0x08,0x00,0x2B,0x2C,0xF9,0xAE}}, 15}, L"Company",       false },
    { L"System.Category",               {{0xD5CDD502,0x2E9C,0x101B,{0x93,0x97,0x08,0x00,0x2B,0x2C,0xF9,0xAE}}, 2},  L"Category",      false },
};

const PropMapping* FindPropMapping(const wchar_t* name) {
    for (size_t i = 0; i < _countof(s_propMap); i++)
        if (_wcsicmp(s_propMap[i].name, name) == 0) return &s_propMap[i];
    return nullptr;
}

// Clean up a VARIANT that may contain PROPVARIANT types (VT_LPWSTR/VT_LPSTR)
// from IShellFolder2::GetDetailsEx
void ClearShellVariant(VARIANT& var) {
    if (var.vt == VT_LPWSTR || var.vt == VT_LPSTR) {
        CoTaskMemFree(var.bstrVal); // same union offset as PROPVARIANT::pwszVal
        var.vt = VT_EMPTY;
    } else {
        VariantClear(&var);
    }
}

// Format a VARIANT value for XP-style display (infotip.cpp IPropertyUI::FormatForDisplay)
bool FormatPropValue(const VARIANT& var, wchar_t* buf, int bufSize) {
    buf[0] = 0;
    switch (var.vt) {
    case VT_BSTR:
        if (var.bstrVal && var.bstrVal[0]) {
            wcsncpy_s(buf, bufSize, var.bstrVal, _TRUNCATE);
            return true;
        }
        return false;
    case VT_LPWSTR: {
        // PROPVARIANT string type; shell GetDetailsEx often returns this
        LPCWSTR psz = (LPCWSTR)var.bstrVal;
        if (psz && psz[0]) {
            wcsncpy_s(buf, bufSize, psz, _TRUNCATE);
            return true;
        }
        return false;
    }
    case VT_UI8:
        StrFormatByteSize64(var.ullVal, buf, bufSize);
        return buf[0] != 0;
    case VT_I8:
        StrFormatByteSize64(var.llVal, buf, bufSize);
        return buf[0] != 0;
    case VT_UI4:
        StrFormatByteSize64((ULONGLONG)var.ulVal, buf, bufSize);
        return buf[0] != 0;
    case VT_DATE: {
        SYSTEMTIME st;
        if (VariantTimeToSystemTime(var.date, &st)) {
            wchar_t d[64], t[64];
            GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, d, 64);
            GetTimeFormatW(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, t, 64);
            swprintf_s(buf, bufSize, L"%s %s", d, t);
            return true;
        }
        return false;
    }
    case VT_FILETIME: {
        // VT_FILETIME is a PROPVARIANT type; decode from ullVal at same offset
        FILETIME ft;
        ft.dwLowDateTime = (DWORD)(var.ullVal & 0xFFFFFFFF);
        ft.dwHighDateTime = (DWORD)(var.ullVal >> 32);
        SYSTEMTIME st, stLocal;
        if (FileTimeToSystemTime(&ft, &st) &&
            SystemTimeToTzSpecificLocalTime(NULL, &st, &stLocal)) {
            wchar_t d[64], t[64];
            GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &stLocal, NULL, d, 64);
            GetTimeFormatW(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &stLocal, NULL, t, 64);
            swprintf_s(buf, bufSize, L"%s %s", d, t);
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

// ---------------------------------------------------------------------------
// Status text — ported from XP shell32 sfvcmpt.cpp
// ---------------------------------------------------------------------------

// Build the pane 0 text (object count, hidden count, free space)
// and the pane 1 text (file size)
// Matching XP's _ShowNoSelectionState and ViewShowSelectionState
void GetStatusInfo(IShellBrowser* pBrowser, wchar_t* pane0Buf, int pane0Size,
                   wchar_t* pane1Buf, int pane1Size) {
    pane0Buf[0] = pane1Buf[0] = 0;
    if (!pBrowser) return;

    IShellView* pView = nullptr;
    if (FAILED(pBrowser->QueryActiveShellView(&pView)) || !pView) return;

    IFolderView2* pView2 = nullptr;
    if (FAILED(pView->QueryInterface(IID_IFolderView2, (void**)&pView2)) || !pView2) {
        pView->Release();
        return;
    }

    int selCount = 0, totalCount = 0;
    pView2->ItemCount(SVGIO_SELECTION, &selCount);
    pView2->ItemCount(SVGIO_ALLVIEW, &totalCount);

    IPersistFolder2* pFolder = nullptr;
    IShellFolder2* pFolder2 = nullptr;
    IShellFolder* pSF = nullptr;

    bool haveFolder = SUCCEEDED(pView2->GetFolder(IID_IPersistFolder2, (void**)&pFolder)) && pFolder;
    bool haveFolder2 = haveFolder && SUCCEEDED(pFolder->QueryInterface(IID_IShellFolder2, (void**)&pFolder2));
    bool haveSF = haveFolder && SUCCEEDED(pFolder->QueryInterface(IID_IShellFolder, (void**)&pSF));

    // --- Pane 0: Status text ---
    if (g_ePaneStyle == PANESTYLE_98 || g_ePaneStyle == PANESTYLE_95) {
        // Windows 98/95 style: pane 0 always shows count
        // Selection: "N object(s) selected", No selection: "N object(s)"
        wchar_t szNum[64];
        if (selCount > 0) {
            AddCommas(selCount, szNum, _countof(szNum));
            swprintf_s(pane0Buf, pane0Size, FMT_SELECTED, szNum);
        } else {
            AddCommas(totalCount, szNum, _countof(szNum));
            swprintf_s(pane0Buf, pane0Size, FMT_OBJECTS, szNum);
        }
    } else if (selCount == 1 && haveFolder2 && haveSF) {
        // Single selection: XP reads InfoTip "prop:" string from registry per file type
        // (HKCR\{ext}\InfoTip), then iterates property names through IPropertyUI.
        // We replicate this with AssocQueryString + our XP prop name -> SCID table.
        IEnumIDList* pEnum = nullptr;
        PITEMID_CHILD child;
        if (SUCCEEDED(pView2->Items(SVGIO_SELECTION, IID_IEnumIDList, (void**)&pEnum)) && pEnum &&
            pEnum->Next(1, &child, NULL) == S_OK) {
            // Determine association class: "Directory" for folders, file extension for files
            wchar_t szAssoc[MAX_PATH] = L"*";
            SFGAOF attrs = SFGAO_FOLDER;
            if (SUCCEEDED(pSF->GetAttributesOf(1, (PCUITEMID_CHILD*)&child, &attrs)) &&
                (attrs & SFGAO_FOLDER)) {
                wcscpy_s(szAssoc, L"Directory");
            } else {
                STRRET str;
                wchar_t szName[MAX_PATH] = {};
                if (SUCCEEDED(pSF->GetDisplayNameOf(child, SHGDN_INFOLDER | SHGDN_FORPARSING, &str)))
                    StrRetToBufW(&str, child, szName, MAX_PATH);
                const wchar_t* ext = PathFindExtensionW(szName);
                if (ext && *ext)
                    wcscpy_s(szAssoc, ext);
            }

            // Get the InfoTip property string
            wchar_t szPropStr[512] = {};
            if (g_ePaneStyle == PANESTYLE_2000) {
                // Win2K: always use the generic * default (no per-type overrides)
                wcscpy_s(szPropStr, L"prop:Type;Author;Title;Subject;Comment;Size");
            } else {
                // XP: read per-type InfoTip from registry (HKCR\{ext}\InfoTip)
                DWORD cch = _countof(szPropStr);
                if (FAILED(AssocQueryStringW(ASSOCF_INIT_DEFAULTTOSTAR, ASSOCSTR_INFOTIP,
                                             szAssoc, NULL, szPropStr, &cch)) || !szPropStr[0]) {
                    // Fallback: XP's default for * (all files) from selfreg.inx
                    wcscpy_s(szPropStr, L"prop:Type;DocAuthor;DocTitle;DocSubject;DocComments;Write;Size");
                }
            }

            // Strip "prop:" prefix
            wchar_t* pProps = szPropStr;
            if (_wcsnicmp(pProps, L"prop:", 5) == 0)
                pProps += 5;

            // Iterate properties and build "Label: Value  Label: Value  ..."
            // Matches XP's CInfoTip::_GetInfoTipFromItem + CleanTipForSingleLine
            wchar_t* p = pane0Buf;
            int rem = pane0Size;
            bool any = false;

            wchar_t propsCopy[512];
            wcscpy_s(propsCopy, pProps);
            wchar_t* ctx = nullptr;
            for (wchar_t* token = wcstok_s(propsCopy, L";", &ctx);
                 token && rem > 1;
                 token = wcstok_s(NULL, L";", &ctx)) {
                const PropMapping* pmap = FindPropMapping(token);
                if (!pmap) continue;

                VARIANT var = {};
                if (SUCCEEDED(pFolder2->GetDetailsEx(child, &pmap->scid, &var)) &&
                    var.vt != VT_EMPTY) {
                    wchar_t szVal[256] = {};
                    if (FormatPropValue(var, szVal, _countof(szVal))) {
                        int n;
                        if (pmap->noLabel)
                            n = swprintf_s(p, rem, L"%s%s", any ? L"  " : L"", szVal);
                        else
                            n = swprintf_s(p, rem, L"%s%s: %s", any ? L"  " : L"", pmap->label, szVal);
                        if (n > 0) { p += n; rem -= n; any = true; }
                    }
                }
                ClearShellVariant(var);
            }

            // Fallback: if property-based approach yielded nothing, try IQueryInfo
            if (!pane0Buf[0]) {
                IQueryInfo* pqi = nullptr;
                if (SUCCEEDED(pSF->GetUIObjectOf(NULL, 1, (PCUITEMID_CHILD*)&child,
                    IID_IQueryInfo, NULL, (void**)&pqi)) && pqi) {
                    LPWSTR pszTip = nullptr;
                    if (SUCCEEDED(pqi->GetInfoTip(0, &pszTip)) && pszTip) {
                        // XP's CleanTipForSingleLine: \r\n -> "  "
                        wchar_t* dst = pane0Buf;
                        int rem = pane0Size;
                        for (const wchar_t* s = pszTip; *s && rem > 1; ) {
                            if (s[0] == L'\r' && s[1] == L'\n') {
                                if (rem > 2) { *dst++ = L' '; *dst++ = L' '; rem -= 2; }
                                s += 2;
                            } else if (s[0] == L'\n') {
                                if (rem > 2) { *dst++ = L' '; *dst++ = L' '; rem -= 2; }
                                s++;
                            } else {
                                *dst++ = *s++;
                                rem--;
                            }
                        }
                        *dst = 0;
                        CoTaskMemFree(pszTip);
                    }
                    pqi->Release();
                }
            }

            ILFree(child);
        }
        if (pEnum) pEnum->Release();
    } else if (selCount > 1) {
        // Multiple selection: "N objects selected" (XP IDS_FSSTATUSSELECTED)
        wchar_t szNum[64];
        AddCommas(selCount, szNum, _countof(szNum));
        swprintf_s(pane0Buf, pane0Size, FMT_SELECTED, szNum);
    } else {
        // No selection: "N objects" + optional hidden + optional free space
        wchar_t szNum[64];
        AddCommas(totalCount, szNum, _countof(szNum));

        // Check for hidden files — skip for large folders (>2000 items) to avoid
        // blocking the UI thread with a full re-enumeration via EnumObjects
        int cHidden = 0;
        if (g_bShowHiddenCount && totalCount <= 2000 && haveFolder && haveSF) {
            PIDLIST_ABSOLUTE pidlFolder = nullptr;
            if (SUCCEEDED(pFolder->GetCurFolder(&pidlFolder)) && pidlFolder) {
                IShellFolder* psfFolder = nullptr;
                if (SUCCEEDED(SHBindToObject(NULL, pidlFolder, NULL, IID_IShellFolder, (void**)&psfFolder)) && psfFolder) {
                    // Count all items including hidden
                    IEnumIDList* pEnumAll = nullptr;
                    int totalWithHidden = 0;
                    if (SUCCEEDED(psfFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN, &pEnumAll)) && pEnumAll) {
                        PITEMID_CHILD pidlChild;
                        while (pEnumAll->Next(1, &pidlChild, NULL) == S_OK) {
                            totalWithHidden++;
                            ILFree(pidlChild);
                        }
                        pEnumAll->Release();
                    }
                    cHidden = totalWithHidden - totalCount;
                    if (cHidden < 0) cHidden = 0;
                    psfFolder->Release();
                }
                ILFree(pidlFolder);
            }
        }

        // Check for disk free space (only in explorer mode — tree pane visible)
        wchar_t szFree[64] = {};
        bool showFree = false;
        if (IsExplorerMode(pBrowser) && haveFolder) {
            PIDLIST_ABSOLUTE pidl = nullptr;
            if (SUCCEEDED(pFolder->GetCurFolder(&pidl)) && pidl) {
                wchar_t path[MAX_PATH];
                ULARGE_INTEGER diskFree;
                if (SHGetPathFromIDList(pidl, path) && GetDiskFreeSpaceEx(path, NULL, NULL, &diskFree)) {
                    StrFormatByteSize64(diskFree.QuadPart, szFree, _countof(szFree));
                    showFree = true;
                }
                ILFree(pidl);
            }
        }

        // Build the compound string matching XP's format
        wchar_t szHidden[64];
        if (cHidden > 0) AddCommas(cHidden, szHidden, _countof(szHidden));

        if (cHidden > 0 && showFree)
            swprintf_s(pane0Buf, pane0Size, FMT_OBJECTS_HIDDEN_SPACE, szNum, szHidden, szFree);
        else if (showFree)
            swprintf_s(pane0Buf, pane0Size, FMT_OBJECTS_SPACE, szNum, szFree);
        else if (cHidden > 0)
            swprintf_s(pane0Buf, pane0Size, FMT_OBJECTS_HIDDEN, szNum, szHidden);
        else
            swprintf_s(pane0Buf, pane0Size, FMT_OBJECTS, szNum);
    }

    // --- Pane 1: File size / drive info ---
    if (g_ePaneStyle == PANESTYLE_98 || g_ePaneStyle == PANESTYLE_95) {
        // Windows 98 style pane 1:
        // Drive selected in My Computer: "Free Space: X,  Capacity: Y"
        // Files/folders selected: total file size of selected
        // No selection: total size of all files in current view
        bool driveInfoShown = false;

        // Check if a drive root is selected → show free space / capacity
        if (selCount == 1 && haveSF) {
            IEnumIDList* pEnum = nullptr;
            PITEMID_CHILD child;
            if (SUCCEEDED(pView2->Items(SVGIO_SELECTION, IID_IEnumIDList, (void**)&pEnum)) && pEnum &&
                pEnum->Next(1, &child, NULL) == S_OK) {
                STRRET str;
                wchar_t szPath[MAX_PATH] = {};
                if (SUCCEEDED(pSF->GetDisplayNameOf(child, SHGDN_FORPARSING, &str)))
                    StrRetToBufW(&str, child, szPath, MAX_PATH);
                // Drive root: "C:\" (letter + colon + backslash + null)
                if (szPath[0] && szPath[1] == L':' && szPath[2] == L'\\' && szPath[3] == 0) {
                    ULARGE_INTEGER diskFree, diskTotal;
                    if (GetDiskFreeSpaceEx(szPath, NULL, &diskTotal, &diskFree)) {
                        wchar_t szFree[64], szCap[64];
                        StrFormatByteSize64(diskFree.QuadPart, szFree, _countof(szFree));
                        StrFormatByteSize64(diskTotal.QuadPart, szCap, _countof(szCap));
                        swprintf_s(pane1Buf, pane1Size, FMT_FREE_CAPACITY, szFree, szCap);
                        driveInfoShown = true;
                    }
                }
                ILFree(child);
            }
            if (pEnum) pEnum->Release();
        }

        // Otherwise show file size (selected items, or all items if no selection)
        if (!driveInfoShown && haveFolder2) {
            __int64 fileSize = -1;
            IFolderView* pFV = nullptr;
            if (SUCCEEDED(pView->QueryInterface(IID_IFolderView, (void**)&pFV)) && pFV) {
                UINT type = selCount > 0 ? SVGIO_SELECTION : SVGIO_ALLVIEW;
                IEnumIDList* pEnum = nullptr;
                if (SUCCEEDED(pFV->Items(type, IID_IEnumIDList, (void**)&pEnum)) && pEnum) {
                    PITEMID_CHILD child;
                    SHCOLUMNID column = { PKEY_Size.fmtid, PKEY_Size.pid };
                    while (pEnum->Next(1, &child, NULL) == S_OK) {
                        VARIANT var = {};
                        if (SUCCEEDED(pFolder2->GetDetailsEx(child, &column, &var)) && var.vt == VT_UI8)
                            fileSize = fileSize < 0 ? (long long)var.ullVal : fileSize + (long long)var.ullVal;
                        ClearShellVariant(var);
                        ILFree(child);
                    }
                    pEnum->Release();
                }
                pFV->Release();
            }
            if (fileSize >= 0)
                StrFormatByteSize64(fileSize, pane1Buf, pane1Size);
        }
    } else {
    // XP/2000 style pane 1:
    // XP shows: no selection = total size of all; selection = total of selected (only non-folders)
    {
        __int64 fileSize = -1;
        bool hasNonFolder = false;

        IFolderView* pFV = nullptr;
        if (SUCCEEDED(pView->QueryInterface(IID_IFolderView, (void**)&pFV)) && pFV) {
            int sc = 0;
            pFV->ItemCount(SVGIO_SELECTION, &sc);
            UINT type = sc > 0 ? SVGIO_SELECTION : SVGIO_ALLVIEW;

            if (haveFolder2) {
                IEnumIDList* pEnum = nullptr;
                if (SUCCEEDED(pFV->Items(type, IID_IEnumIDList, (void**)&pEnum)) && pEnum) {
                    PITEMID_CHILD child;
                    SHCOLUMNID column = { PKEY_Size.fmtid, PKEY_Size.pid };

                    while (pEnum->Next(1, &child, NULL) == S_OK) {
                        // Check if it's a folder (XP skipped folders for size in selection mode)
                        if (sc > 0 && haveSF) {
                            SFGAOF attrs = SFGAO_FOLDER;
                            if (SUCCEEDED(pSF->GetAttributesOf(1, (PCUITEMID_CHILD*)&child, &attrs)) &&
                                (attrs & SFGAO_FOLDER)) {
                                ILFree(child);
                                continue;
                            }
                            hasNonFolder = true;
                        }

                        VARIANT var = {};
                        if (SUCCEEDED(pFolder2->GetDetailsEx(child, &column, &var)) && var.vt == VT_UI8) {
                            fileSize = fileSize < 0 ? (long long)var.ullVal : fileSize + (long long)var.ullVal;
                            if (sc == 0) hasNonFolder = true;
                        }
                        ClearShellVariant(var);
                        ILFree(child);
                    }
                    pEnum->Release();
                }
            }
            pFV->Release();
        }

        // XP only showed size for selection if there were non-folder items
        if (fileSize >= 0 && (selCount == 0 || hasNonFolder))
            StrFormatByteSize64(fileSize, pane1Buf, pane1Size);
    }
    } // end XP style

    if (pSF) pSF->Release();
    if (pFolder2) pFolder2->Release();
    if (pFolder) pFolder->Release();
    pView2->Release();
    pView->Release();
}

// ---------------------------------------------------------------------------
// Pane sizing — ported from XP shell32 sfvcmpt.cpp ResizeStatus
// ---------------------------------------------------------------------------

void ResizeStatusPanes(StatusBarData* pData) {
    if (!pData || !pData->statusBar) return;

    RECT rc;
    GetClientRect(pData->statusBar, &rc);

    int ciBorders[3] = {};
    SendMessage(pData->statusBar, SB_GETBORDERS, 0, (LPARAM)ciBorders);

    // Windows 95/NT4: 2 panes (no zone) — from NT4 shell32 fstreex.c FSInitializeStatus
    if (g_ePaneStyle == PANESTYLE_95) {
        HDC hdc = GetDC(NULL);
        int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(NULL, hdc);
        int ciParts[2] = { 3 * dpi / 2, -1 };
        SendMessage(pData->statusBar, SB_SETPARTS, 2, (LPARAM)ciParts);
        return;
    }

    int ciParts[STATUS_PANES];

    if (g_ePaneStyle == PANESTYLE_98) {
        // Windows 98 layout — from decompiled shell32 sub_7FCCEDCD
        // Pane 0: fixed 5*DPI/2 (240px @96dpi)
        // Pane 1: fills middle (statusbar_right - 220, min = pane0)
        // Pane 2: -1 (last ~220px)
        HDC hdc = GetDC(NULL);
        int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(NULL, hdc);

        ciParts[0] = 5 * dpi / 2;
        ciParts[1] = rc.right - 220;
        if (ciParts[1] < ciParts[0])
            ciParts[1] = ciParts[0];
        ciParts[2] = -1;
    } else {
        // 2000/XP dynamic layout — from shell32/sfvcmpt.cpp ResizeStatus()
        ciParts[2] = -1;

        int cxZone = ciBorders[0] + pData->zonePaneWidth + ciBorders[2];
        ciParts[1] = rc.right - cxZone;

        // Size pane = 13 char widths of "0"
        HDC hdc = GetDC(pData->statusBar);
        HFONT hf = (HFONT)SendMessage(pData->statusBar, WM_GETFONT, 0, 0);
        HFONT hfPrev = (HFONT)SelectObject(hdc, hf);
        SIZE siz;
        GetTextExtentPoint32W(hdc, L"0", 1, &siz);
        SelectObject(hdc, hfPrev);
        ReleaseDC(pData->statusBar, hdc);

        int cxSize = ciBorders[0] + siz.cx * 13;
        ciParts[0] = ciParts[1] - cxSize;

        if (ciParts[0] < 0) {
            ciParts[0] = rc.right / 3;
            ciParts[1] = 2 * ciParts[0];
        }
    }

    SendMessage(pData->statusBar, SB_SETPARTS, STATUS_PANES, (LPARAM)ciParts);
}

// ---------------------------------------------------------------------------
// Main update — orchestrates all panes
// ---------------------------------------------------------------------------

void UpdateStatusBar(StatusBarData* pData) {
    if (!pData || !pData->statusBar) return;
    if (!pData->pBrowser) pData->pBrowser = GetShellBrowser(pData->explorerWnd);
    if (!pData->pBrowser) return;

    // Resize panes
    ResizeStatusPanes(pData);

    // Update pane 0 and 1 text
    wchar_t pane0Buf[512], pane1Buf[128];
    GetStatusInfo(pData->pBrowser, pane0Buf, _countof(pane0Buf), pane1Buf, _countof(pane1Buf));

    SendMessage(pData->statusBar, SB_SETTEXTW, STATUS_PANE_ITEMS, (LPARAM)pane0Buf);
    SendMessage(pData->statusBar, SB_SETTEXTW, STATUS_PANE_SIZE, (LPARAM)pane1Buf);

    // Update zone pane (not present on 95/NT4)
    if (g_ePaneStyle != PANESTYLE_95)
        UpdateZonePane(pData);
}

// ---------------------------------------------------------------------------
// Explorer window subclass — menu help text (XP's CShellBrowser::_OnMenuSelect)
// Ported from browseui/shbrows2.cpp and comctl32/menuhelp.c
// ---------------------------------------------------------------------------

#define EXPLORER_SUBCLASS_ID 0xC1A5
static const wchar_t PROP_STATUSBAR[] = L"ClassicStatusBar";

// XP browseui menu help resource ID offsets (browseui/resource.h)
// MH_ITEMS:  added to command IDs to get the help string resource ID
// MH_POPUPS: added to menu bar position index for top-level popup help
#define FCIDM_FIRST   0x8000
#define MH_ITEMS      (800 - FCIDM_FIRST)   // -31968
#define MH_POPUPS     700

LRESULT CALLBACK SubclassExplorerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                       UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_ENTERMENULOOP) {
        HWND sb = (HWND)GetProp(hWnd, PROP_STATUSBAR);
        if (sb) SendMessage(sb, SB_SIMPLE, TRUE, 0);
    }

    if (uMsg == WM_MENUSELECT) {
        HWND sb = (HWND)GetProp(hWnd, PROP_STATUSBAR);
        if (sb) {
            UINT uItem = LOWORD(wParam);
            UINT fuFlags = HIWORD(wParam);
            HMENU hMenu = (HMENU)lParam;

            if (fuFlags == 0xFFFF && hMenu == NULL) {
                // Menu closed
                SendMessage(sb, SB_SIMPLE, FALSE, 0);
                PostMessage(sb, WM_UPDATE_STATUSBAR, 0, 0);
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }

            wchar_t szHelp[256] = {};

            if (fuFlags & MF_SEPARATOR) {
                // Separator: show blank
            } else if (fuFlags & MF_POPUP) {
                // Popup menu: XP's _OnMenuSelect converts popup items to regular
                // items by looking up the submenu's wID via GetMenuItemInfo, then
                // applies the standard MH_ITEMS offset.
                MENUITEMINFOW mii = { sizeof(mii), MIIM_ID };
                if (GetMenuItemInfoW(hMenu, uItem, TRUE, &mii) && mii.wID) {
                    int stringId = (int)mii.wID + MH_ITEMS;
                    if (stringId > 0) {
                        HMODULE hMod = GetModuleHandle(L"explorerframe.dll");
                        if (hMod) LoadStringW(hMod, (UINT)stringId, szHelp, _countof(szHelp));
                    }
                }
                // Fallback: top-level menu bar index + MH_POPUPS
                if (!szHelp[0]) {
                    HMODULE hMod = GetModuleHandle(L"explorerframe.dll");
                    if (hMod) LoadStringW(hMod, uItem + MH_POPUPS, szHelp, _countof(szHelp));
                }
            } else {
                // Regular menu item: command_id + MH_ITEMS (XP's MenuHelp formula)
                int stringId = (int)uItem + MH_ITEMS;
                if (stringId > 0) {
                    HMODULE hMod = GetModuleHandle(L"explorerframe.dll");
                    if (hMod) LoadStringW(hMod, (UINT)stringId, szHelp, _countof(szHelp));
                    // Fallback: shell32 for view/context menu commands
                    if (!szHelp[0]) {
                        hMod = GetModuleHandle(L"shell32.dll");
                        if (hMod) LoadStringW(hMod, (UINT)stringId, szHelp, _countof(szHelp));
                    }
                }
            }

            SendMessage(sb, SB_SETTEXTW, 255, (LPARAM)szHelp);
        }
    }

    if (uMsg == WM_EXITMENULOOP) {
        HWND sb = (HWND)GetProp(hWnd, PROP_STATUSBAR);
        if (sb) {
            SendMessage(sb, SB_SIMPLE, FALSE, 0);
            PostMessage(sb, WM_UPDATE_STATUSBAR, 0, 0);
        }
    }

    if (uMsg == WM_NCDESTROY) {
        RemoveProp(hWnd, PROP_STATUSBAR);
        RemoveWindowSubclass(hWnd, SubclassExplorerProc, uIdSubclass);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Window subclass procedures
// ---------------------------------------------------------------------------

LRESULT CALLBACK SubclassStatusProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                     UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    StatusBarData* pData = (StatusBarData*)dwRefData;

    if (uMsg == WM_CLEAR || uMsg == WM_UPDATE_STATUSBAR) {
        // Debounce: collapse rapid-fire selection changes into one update.
        // LVN_ITEMCHANGED fires per-item, so shift-selecting 100 files would
        // otherwise trigger 100 full COM enumerations.
        SetTimer(hWnd, TIMER_DEBOUNCE, DEBOUNCE_MS, NULL);
        return 0;
    }

    if (uMsg == WM_RESIZE_PANES) {
        ResizeStatusPanes(pData);
        return 0;
    }

    if (uMsg == WM_TIMER && wParam == TIMER_DEBOUNCE) {
        KillTimer(hWnd, TIMER_DEBOUNCE);
        UpdateStatusBar(pData);
        return 0;
    }

    if (uMsg == WM_TIMER && wParam == TIMER_RETRY) {
        KillTimer(hWnd, TIMER_RETRY);

        if (!pData->shellDefView) {
            pData->shellDefView = FindShellDefView(pData->explorerWnd);
            if (pData->shellDefView)
                SetWindowSubclass(pData->shellDefView, SubclassShellViewProc, (UINT_PTR)pData->shellDefView, (DWORD_PTR)pData);
        }

        if (!pData->pBrowser)
            pData->pBrowser = GetShellBrowser(pData->explorerWnd);

        int itemCount = GetItemCount(pData->pBrowser);

        if (itemCount == 0 && pData->retryCount < 50) {
            pData->retryCount++;
            SetTimer(hWnd, TIMER_RETRY, 100, NULL);
            return 0;
        }

        pData->retryCount = 0;
        UpdateStatusBar(pData);

        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        KillTimer(hWnd, TIMER_RETRY);
        KillTimer(hWnd, TIMER_DEBOUNCE);
        RemoveWindowSubclass(hWnd, SubclassStatusProc, uIdSubclass);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK SubclassDUIViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                      UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    StatusBarData* pData = (StatusBarData*)dwRefData;

    if (uMsg == WM_WINDOWPOSCHANGING && pData->statusBar) {
        WINDOWPOS* pPos = (WINDOWPOS*)lParam;
        if (!(pPos->flags & SWP_NOSIZE)) {
            int h = GetStatusBarHeight(pData->statusBar);
            pPos->cy -= h;
            SetWindowPos(pData->statusBar, NULL, pPos->x, pPos->y + pPos->cy, pPos->cx, h, SWP_NOZORDER);
            PostMessage(pData->statusBar, WM_RESIZE_PANES, 0, 0);
        }
    }

    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, SubclassDUIViewProc, uIdSubclass);
        RemoveProp(pData->explorerWnd, PROP_STATUSBAR);
        if (pData->shellDefView)
            RemoveWindowSubclass(pData->shellDefView, SubclassShellViewProc, (UINT_PTR)pData->shellDefView);
        if (pData->pBrowser) pData->pBrowser->Release();
        DestroyZoneCache(&pData->zoneCache);
        delete pData;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK SubclassShellViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                        UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    StatusBarData* pData = (StatusBarData*)dwRefData;

    if (uMsg == WM_NOTIFY) {
        NMHDR* pNM = (NMHDR*)lParam;
        if (pNM->code == LVN_ITEMCHANGED) {
            NMLISTVIEW* pLV = (NMLISTVIEW*)lParam;
            if ((pLV->uChanged & LVIF_STATE) &&
                ((pLV->uOldState ^ pLV->uNewState) & LVIS_SELECTED))
                PostMessage(pData->statusBar, WM_CLEAR, 0, 0);
        }
    }

    if (uMsg == WM_NCDESTROY) {
        pData->shellDefView = NULL;
        pData->retryCount = 0;
        if (pData->pBrowser) {
            pData->pBrowser->Release();
            pData->pBrowser = nullptr;
        }
        RemoveWindowSubclass(hWnd, SubclassShellViewProc, uIdSubclass);
        SetTimer(pData->statusBar, TIMER_RETRY, 100, NULL);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// CreateWindowExW hook — intercepts Explorer window creation
// ---------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, LPVOID lpParam) {

    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd || ((ULONG_PTR)lpClassName <= 0xffff) || wcscmp(lpClassName, L"DUIViewWndClassName"))
        return hWnd;

    HWND explorerWnd = GetAncestor(hWnd, GA_ROOT);
    wchar_t cls[64];
    if (!GetClassName(explorerWnd, cls, _countof(cls)) || _wcsicmp(cls, L"CabinetWClass"))
        return hWnd;

    HWND parentWnd = GetParent(hWnd);
    RECT rc;
    GetClientRect(parentWnd, &rc);

    HWND statusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SBARS_SIZEGRIP | SBT_TOOLTIPS,
        0, 0, rc.right, 0, parentWnd, NULL, NULL, NULL);

    if (!statusBar) return hWnd;

    SendMessage(statusBar, WM_SIZE, 0, 0);
    SendMessage(statusBar, SB_SETMINHEIGHT,
        GetSystemMetrics(SM_CYSMICON) + GetSystemMetrics(SM_CYBORDER) * 2, 0);
    SendMessage(statusBar, WM_SIZE, 0, 0);

    int h = GetStatusBarHeight(statusBar);
    SetWindowPos(statusBar, NULL, 0, rc.bottom - h, rc.right, h, SWP_NOZORDER);

    StatusBarData* pData = new StatusBarData{};
    pData->statusBar = statusBar;
    pData->explorerWnd = explorerWnd;
    pData->shellDefView = NULL;
    pData->pBrowser = nullptr;
    pData->retryCount = 0;
    pData->currentZoneIcon = NULL;
    memset(&pData->zoneCache, 0, sizeof(ZoneCache));

    // Initialize zone cache and compute zone pane width
    InitZoneCache(&pData->zoneCache, statusBar);
    pData->zonePaneWidth = ComputeZonePaneWidth(statusBar, &pData->zoneCache);

    // Set initial 3-pane layout
    ResizeStatusPanes(pData);

    // Store status bar handle as property on explorer window for menu help support
    SetProp(explorerWnd, PROP_STATUSBAR, statusBar);
    SetWindowSubclass(explorerWnd, SubclassExplorerProc, EXPLORER_SUBCLASS_ID, 0);

    SetWindowSubclass(statusBar, SubclassStatusProc, (UINT_PTR)statusBar, (DWORD_PTR)pData);
    SetWindowSubclass(hWnd, SubclassDUIViewProc, (UINT_PTR)hWnd, (DWORD_PTR)pData);

    SetTimer(statusBar, 1, 100, NULL);

    return hWnd;
}

BOOL Wh_ModInit() {
    PCWSTR style = Wh_GetStringSetting(L"PaneStyle");
    if (style) {
        if (_wcsicmp(style, L"2000") == 0) g_ePaneStyle = PANESTYLE_2000;
        else if (_wcsicmp(style, L"98") == 0) g_ePaneStyle = PANESTYLE_98;
        else if (_wcsicmp(style, L"95") == 0) g_ePaneStyle = PANESTYLE_95;
        else g_ePaneStyle = PANESTYLE_XP;
        Wh_FreeStringSetting(style);
    } else {
        g_ePaneStyle = PANESTYLE_XP;
    }
    g_bShowHiddenCount = Wh_GetIntSetting(L"ShowHiddenCount");
    BuildFormatStrings();
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    return TRUE;
}

void Wh_ModUninit() {
}

void Wh_ModSettingsChanged() {
    PCWSTR style = Wh_GetStringSetting(L"PaneStyle");
    if (style) {
        if (_wcsicmp(style, L"2000") == 0) g_ePaneStyle = PANESTYLE_2000;
        else if (_wcsicmp(style, L"98") == 0) g_ePaneStyle = PANESTYLE_98;
        else if (_wcsicmp(style, L"95") == 0) g_ePaneStyle = PANESTYLE_95;
        else g_ePaneStyle = PANESTYLE_XP;
        Wh_FreeStringSetting(style);
    } else {
        g_ePaneStyle = PANESTYLE_XP;
    }
    g_bShowHiddenCount = Wh_GetIntSetting(L"ShowHiddenCount");
    BuildFormatStrings();
}
