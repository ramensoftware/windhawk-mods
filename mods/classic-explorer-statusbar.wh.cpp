// ==WindhawkMod==
// @id              classic-explorer-statusbar
// @name            Classic Explorer Status Bar
// @description     Shows free disk space and file size info in the Explorer's status bar
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lshlwapi -lole32 -loleaut32 -luuid -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Explorer Status Bar

This mod adds a classic status bar to Windows Explorer, showing:
- Item count / selection count
- Free disk space
- Total size of selected files
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <propkey.h>

#if !(defined(__clang_major__) && __clang_major__ >= 20)
#include <initguid.h>
const PROPERTYKEY PKEY_Size = {
    { 0xB725F130, 0x47EF, 0x101A, { 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC } }, 12
};
#endif

#define WM_UPDATE_STATUSBAR (WM_USER + 100)

struct StatusBarData {
    HWND statusBar;
    HWND explorerWnd;
    HWND shellDefView;
    int fileSizeWidth;
    int freeSpaceWidth;
    IShellBrowser* pBrowser;
    int retryCount;
};

LRESULT CALLBACK SubclassShellViewProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

int GetStatusBarHeight(HWND statusBar) {
    RECT rc;
    GetWindowRect(statusBar, &rc);
    return rc.bottom - rc.top;
}

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

void GetStatusText(IShellBrowser* pBrowser, wchar_t* textBuf, int textSize, wchar_t* freeBuf, int freeSize) {
    textBuf[0] = freeBuf[0] = 0;
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
    if (SUCCEEDED(pView2->GetFolder(IID_IPersistFolder2, (void**)&pFolder)) && pFolder) {
        if (selCount == 1) {
            IEnumIDList* pEnum = nullptr;
            PITEMID_CHILD child;
            if (SUCCEEDED(pView2->Items(SVGIO_SELECTION, IID_IEnumIDList, (void**)&pEnum)) && pEnum && 
                pEnum->Next(1, &child, NULL) == S_OK) {
                IShellFolder* pSF = nullptr;
                if (SUCCEEDED(pFolder->QueryInterface(IID_IShellFolder, (void**)&pSF)) && pSF) {
                    IQueryInfo* pQI = nullptr;
                    if (SUCCEEDED(pSF->GetUIObjectOf(NULL, 1, (PCUITEMID_CHILD*)&child, IID_IQueryInfo, NULL, (void**)&pQI)) && pQI) {
                        LPWSTR pTip = nullptr;
                        if (SUCCEEDED(pQI->GetInfoTip(QITIPF_DEFAULT | QITIPF_SINGLELINE, &pTip)) && pTip) {
                            wcsncpy_s(textBuf, textSize, pTip, _TRUNCATE);
                            for (wchar_t* p = textBuf; *p; p++) if (*p == '\t') *p = ' ';
                            CoTaskMemFree(pTip);
                        }
                        pQI->Release();
                    }
                    pSF->Release();
                }
                ILFree(child);
            }
            if (pEnum) pEnum->Release();
        } else {
            NUMBERFMT numFmt = {0, 0, 3};
            wchar_t decSep[10], thousandSep[10], numBuf[64], fmtNum[64], fmtStr[128];
            GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, decSep, 10);
            GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, thousandSep, 10);
            numFmt.lpDecimalSep = decSep;
            numFmt.lpThousandSep = thousandSep;
            
            int count = selCount > 1 ? selCount : totalCount;
            UINT strId = selCount > 1 ? 38194 : 38192;
            
            swprintf_s(numBuf, L"%d", count);
            GetNumberFormat(LOCALE_USER_DEFAULT, 0, numBuf, &numFmt, fmtNum, _countof(fmtNum));
            
            if (LoadString(GetModuleHandle(L"shell32.dll"), strId, fmtStr, _countof(fmtStr)) > 0)
                swprintf_s(textBuf, textSize, fmtStr, fmtNum);
            else
                swprintf_s(textBuf, textSize, selCount > 1 ? L"%s selected" : L"%s items", fmtNum);
        }
        
        PIDLIST_ABSOLUTE pidl = nullptr;
        wchar_t path[MAX_PATH];
        ULARGE_INTEGER diskFree;
        if (SUCCEEDED(pFolder->GetCurFolder(&pidl)) && pidl) {
            if (SHGetPathFromIDList(pidl, path) && GetDiskFreeSpaceEx(path, NULL, NULL, &diskFree)) {
                wchar_t sizeStr[64], freeLabel[64];
                StrFormatByteSize64(diskFree.QuadPart, sizeStr, _countof(sizeStr));
                if (LoadString(GetModuleHandle(L"shell32.dll"), 9307, freeLabel, _countof(freeLabel)) > 0)
                    swprintf_s(freeBuf, freeSize, L"%s: %s", freeLabel, sizeStr);
                else
                    swprintf_s(freeBuf, freeSize, L"Free: %s", sizeStr);
            }
            ILFree(pidl);
        }
        pFolder->Release();
    }
    pView2->Release();
    pView->Release();
}

void GetFileSize(IShellBrowser* pBrowser, wchar_t* buf, int size) {
    buf[0] = 0;
    if (!pBrowser) return;

    __int64 fileSize = -1;

    IShellView* pView = nullptr;
    if (FAILED(pBrowser->QueryActiveShellView(&pView)) || !pView) return;
    
    IFolderView* pFV = nullptr;
    if (FAILED(pView->QueryInterface(IID_IFolderView, (void**)&pFV)) || !pFV) {
        pView->Release();
        return;
    }
    
    IPersistFolder2* pFolder = nullptr;
    IShellFolder2* pFolder2 = nullptr;
    if (SUCCEEDED(pFV->GetFolder(IID_IPersistFolder2, (void**)&pFolder)) && pFolder &&
        SUCCEEDED(pFolder->QueryInterface(IID_IShellFolder2, (void**)&pFolder2)) && pFolder2) {
        
        int selCount = 0;
        pFV->ItemCount(SVGIO_SELECTION, &selCount);
        
        UINT type = selCount > 0 ? SVGIO_SELECTION : SVGIO_ALLVIEW;
        IEnumIDList* pEnum = nullptr;
        
        if (SUCCEEDED(pFV->Items(type, IID_IEnumIDList, (void**)&pEnum)) && pEnum) {
            PITEMID_CHILD child;
            SHCOLUMNID column = { PKEY_Size.fmtid, PKEY_Size.pid };
            
            while (pEnum->Next(1, &child, NULL) == S_OK) {
                VARIANT var = {};
                if (SUCCEEDED(pFolder2->GetDetailsEx(child, &column, &var)) && var.vt == VT_UI8)
                    fileSize = fileSize < 0 ? var.ullVal : fileSize + var.ullVal;
                VariantClear(&var);
                ILFree(child);
            }
            pEnum->Release();
        }
        pFolder2->Release();
    }
    if (pFolder) pFolder->Release();
    pFV->Release();
    pView->Release();
    
    if (fileSize >= 0)
        StrFormatByteSize64(fileSize, buf, size);
}

void UpdateStatusBar(StatusBarData* pData) {
    if (!pData || !pData->statusBar) return;
    if (!pData->pBrowser) pData->pBrowser = GetShellBrowser(pData->explorerWnd);
    if (!pData->pBrowser) return;
    
    wchar_t textBuf[512], freeBuf[128], sizeBuf[128];
    GetStatusText(pData->pBrowser, textBuf, _countof(textBuf), freeBuf, _countof(freeBuf));
    GetFileSize(pData->pBrowser, sizeBuf, _countof(sizeBuf));
    
    RECT rc;
    GetClientRect(pData->statusBar, &rc);
    int w = rc.right, h = GetStatusBarHeight(pData->statusBar);
    bool hasFree = freeBuf[0] != 0;
    bool hasSize = sizeBuf[0] != 0;
    
    if (hasSize && hasFree) {
        int parts[] = {w - h - pData->fileSizeWidth - pData->freeSpaceWidth, w - h - pData->fileSizeWidth, -1};
        SendMessage(pData->statusBar, SB_SETPARTS, 3, (LPARAM)parts);
        SendMessage(pData->statusBar, SB_SETTEXT, 0, (LPARAM)textBuf);
        SendMessage(pData->statusBar, SB_SETTEXT, 1, (LPARAM)freeBuf);
        SendMessage(pData->statusBar, SB_SETTEXT, 2, (LPARAM)sizeBuf);
    } else if (hasFree || hasSize) {
        int parts[] = {w - h - (hasFree ? pData->freeSpaceWidth : pData->fileSizeWidth), -1};
        SendMessage(pData->statusBar, SB_SETPARTS, 2, (LPARAM)parts);
        SendMessage(pData->statusBar, SB_SETTEXT, 0, (LPARAM)textBuf);
        SendMessage(pData->statusBar, SB_SETTEXT, 1, (LPARAM)(hasFree ? freeBuf : sizeBuf));
    } else {
        int parts[] = {-1};
        SendMessage(pData->statusBar, SB_SETPARTS, 1, (LPARAM)parts);
        SendMessage(pData->statusBar, SB_SETTEXT, 0, (LPARAM)textBuf);
    }
}

LRESULT CALLBACK SubclassStatusProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                                     UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    StatusBarData* pData = (StatusBarData*)dwRefData;
    
    if (uMsg == WM_CLEAR || uMsg == WM_UPDATE_STATUSBAR) {
        UpdateStatusBar(pData);
        return 0;
    }
    
    if (uMsg == WM_TIMER && wParam == 1) {
        KillTimer(hWnd, 1);
        
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
            SetTimer(hWnd, 1, 100, NULL);
            return 0;
        }
        
        pData->retryCount = 0;
        UpdateStatusBar(pData);
        
        return 0;
    }
    
    if (uMsg == WM_NCDESTROY) {
        KillTimer(hWnd, 1);
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
            PostMessage(pData->statusBar, WM_UPDATE_STATUSBAR, 0, 0);
        }
    }
    
    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, SubclassDUIViewProc, uIdSubclass);
        if (pData->shellDefView)
            RemoveWindowSubclass(pData->shellDefView, SubclassShellViewProc, (UINT_PTR)pData->shellDefView);
        if (pData->pBrowser) pData->pBrowser->Release();
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
        SetTimer(pData->statusBar, 1, 100, NULL);
    }
    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

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
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SBARS_SIZEGRIP,
        0, 0, rc.right, 0, parentWnd, NULL, NULL, NULL);
    
    if (!statusBar) return hWnd;
    
    SendMessage(statusBar, WM_SIZE, 0, 0);
    int h = GetStatusBarHeight(statusBar);
    SetWindowPos(statusBar, NULL, 0, rc.bottom - h, rc.right, h, SWP_NOZORDER);
    
    HDC hdc = GetDC(statusBar);
    HFONT hFont = (HFONT)SendMessage(statusBar, WM_GETFONT, 0, 0);
    HFONT hOld = (HFONT)SelectObject(hdc, hFont);
    SIZE sz;
    
    GetTextExtentPoint32(hdc, L"999.99 MB", 9, &sz);
    int fileW = sz.cx < 50 ? 50 : sz.cx;
    
    wchar_t freeLabel[64];
    LoadString(GetModuleHandle(L"shell32.dll"), 9307, freeLabel, _countof(freeLabel));
    wcscat_s(freeLabel, L": 999.99 GB");
    GetTextExtentPoint32(hdc, freeLabel, (int)wcslen(freeLabel), &sz);
    int freeW = sz.cx < 70 ? 70 : sz.cx;
    
    SelectObject(hdc, hOld);
    ReleaseDC(statusBar, hdc);
    
    StatusBarData* pData = new StatusBarData{statusBar, explorerWnd, NULL, fileW, freeW, nullptr, 0};
    
    SetWindowSubclass(statusBar, SubclassStatusProc, (UINT_PTR)statusBar, (DWORD_PTR)pData);
    SetWindowSubclass(hWnd, SubclassDUIViewProc, (UINT_PTR)hWnd, (DWORD_PTR)pData);
    
    SetTimer(statusBar, 1, 100, NULL);
    
    return hWnd;
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    return TRUE;
}

void Wh_ModUninit() {}
