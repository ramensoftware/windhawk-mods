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

![Screenshot](https://i.imgur.com/65uw69r.png)

This is an alternative to the status bar from Classic Explorer from Open-Shell project.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <objbase.h>
#include <propkey.h>
#include <initguid.h>

#if !(defined(__clang_major__) && __clang_major__ >= 20)
const PROPERTYKEY PKEY_Size = {
    { 0xB725F130, 0x47EF, 0x101A, { 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC } },
    12
};
#endif

#define PART_TEXT 0
#define PART_FREE 1
#define PART_SIZE 2

#define WM_UPDATE_STATUSBAR (WM_USER + 100)

static const wchar_t* STATUSBAR_DATA_PROP = L"ExplorerStatusBarData";

struct StatusBarData {
    HWND statusBar;
    HWND dUIView;
    HWND shellDefView;
    HWND explorerWnd;
    int fileSizeWidth;
    int freeSpaceWidth;
    IShellBrowser* pBrowser;
};

// Получает реальную высоту статус-бара
int GetStatusBarHeight(HWND statusBar) {
    RECT rc;
    GetWindowRect(statusBar, &rc);
    return rc.bottom - rc.top;
}

LRESULT CALLBACK SubclassShellViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                                        UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

IShellBrowser* GetShellBrowser(HWND hwndExplorer) {
    IShellBrowser* pBrowser = nullptr;
    
    IShellWindows* pShellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_LOCAL_SERVER, 
                                 IID_IShellWindows, (void**)&pShellWindows)) || !pShellWindows) {
        return nullptr;
    }
    
    long count = 0;
    pShellWindows->get_Count(&count);
    
    for (long i = 0; i < count; i++) {
        VARIANT vi;
        VariantInit(&vi);
        V_VT(&vi) = VT_I4;
        V_I4(&vi) = i;
        
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
                    pWB2->Release();
                    pDisp->Release();
                    break;
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
    if (!shellTab) return NULL;
    
    HWND duiView = FindWindowEx(shellTab, NULL, L"DUIViewWndClassName", NULL);
    if (!duiView) return NULL;
    
    HWND directUI = FindWindowEx(duiView, NULL, L"DirectUIHWND", NULL);
    if (!directUI) return NULL;
    
    HWND child = NULL;
    while ((child = FindWindowEx(directUI, child, NULL, NULL)) != NULL) {
        HWND defView = FindWindowEx(child, NULL, L"SHELLDLL_DefView", NULL);
        if (defView) return defView;
    }
    
    return NULL;
}

bool GetStatusText(IShellBrowser* pBrowser, wchar_t* textBuf, int textSize, wchar_t* freeBuf, int freeSize) {
    textBuf[0] = 0;
    freeBuf[0] = 0;
    if (!pBrowser) return false;

    bool res = false;
    IShellView* pView = nullptr;
    if (SUCCEEDED(pBrowser->QueryActiveShellView(&pView)) && pView) {
        IFolderView2* pView2 = nullptr;
        if (SUCCEEDED(pView->QueryInterface(IID_IFolderView2, (void**)&pView2)) && pView2) {
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
                        
                        IShellFolder* pShellFolder = nullptr;
                        if (SUCCEEDED(pFolder->QueryInterface(IID_IShellFolder, (void**)&pShellFolder)) && pShellFolder) {
                            IQueryInfo* pQueryInfo = nullptr;
                            if (SUCCEEDED(pShellFolder->GetUIObjectOf(NULL, 1, (PCUITEMID_CHILD*)&child, 
                                         IID_IQueryInfo, NULL, (void**)&pQueryInfo)) && pQueryInfo) {
                                LPWSTR pTip = nullptr;
                                if (SUCCEEDED(pQueryInfo->GetInfoTip(QITIPF_DEFAULT | QITIPF_SINGLELINE, &pTip)) && pTip) {
                                    wcsncpy_s(textBuf, textSize, pTip, _TRUNCATE);
                                    for (wchar_t* p = textBuf; *p; p++)
                                        if (*p == '\t') *p = ' ';
                                    CoTaskMemFree(pTip);
                                    res = true;
                                }
                                pQueryInfo->Release();
                            }
                            pShellFolder->Release();
                        }
                        ILFree(child);
                    }
                    if (pEnum) pEnum->Release();
                    
                } else {
                    NUMBERFMT numFmt = {0};
                    wchar_t decSep[10], thousandSep[10];
                    numFmt.NumDigits = 0;
                    numFmt.LeadingZero = 0;
                    numFmt.Grouping = 3;
                    GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, decSep, 10);
                    numFmt.lpDecimalSep = decSep;
                    GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, thousandSep, 10);
                    numFmt.lpThousandSep = thousandSep;
                    
                    wchar_t numBuf[64];
                    wchar_t formattedNum[64];
                    
                    if (selCount > 1) {
                        swprintf_s(numBuf, L"%d", selCount);
                        GetNumberFormat(LOCALE_USER_DEFAULT, 0, numBuf, &numFmt, formattedNum, _countof(formattedNum));
                        
                        wchar_t fmtStr[128];
                        if (LoadString(GetModuleHandle(L"shell32.dll"), 38194, fmtStr, _countof(fmtStr)) > 0) {
                            swprintf_s(textBuf, textSize, fmtStr, formattedNum);
                        } else {
                            swprintf_s(textBuf, textSize, L"%s selected", formattedNum);
                        }
                    } else {
                        swprintf_s(numBuf, L"%d", totalCount);
                        GetNumberFormat(LOCALE_USER_DEFAULT, 0, numBuf, &numFmt, formattedNum, _countof(formattedNum));
                        
                        wchar_t fmtStr[128];
                        if (LoadString(GetModuleHandle(L"shell32.dll"), 38192, fmtStr, _countof(fmtStr)) > 0) {
                            swprintf_s(textBuf, textSize, fmtStr, formattedNum);
                        } else {
                            swprintf_s(textBuf, textSize, L"%s items", formattedNum);
                        }
                    }
                    res = true;
                }
                
                // Свободное место
                PIDLIST_ABSOLUTE pidl = nullptr;
                wchar_t path[MAX_PATH];
                ULARGE_INTEGER diskSize;
                if (SUCCEEDED(pFolder->GetCurFolder(&pidl)) && pidl && 
                    SHGetPathFromIDList(pidl, path) && 
                    GetDiskFreeSpaceEx(path, NULL, NULL, &diskSize)) {
                    wchar_t sizeStr[64];
                    StrFormatByteSize64(diskSize.QuadPart, sizeStr, _countof(sizeStr));
                    
                    wchar_t freeLabel[64];
                    if (LoadString(GetModuleHandle(L"shell32.dll"), 9307, freeLabel, _countof(freeLabel)) > 0) {
                        swprintf_s(freeBuf, freeSize, L"%s: %s", freeLabel, sizeStr);
                    } else {
                        swprintf_s(freeBuf, freeSize, L"Free: %s", sizeStr);
                    }
                }
                if (pidl) ILFree(pidl);
                
                pFolder->Release();
            }
            pView2->Release();
        }
        pView->Release();
    }
    return res;
}

void GetFileSize(IShellBrowser* pBrowser, wchar_t* buf, int size) {
    buf[0] = 0;
    if (!pBrowser) return;

    __int64 fileSize = -1;
    bool bMore = false;
    DWORD time0 = GetTickCount();

    IShellView* pView = nullptr;
    if (SUCCEEDED(pBrowser->QueryActiveShellView(&pView)) && pView) {
        IFolderView* pView2 = nullptr;
        if (SUCCEEDED(pView->QueryInterface(IID_IFolderView, (void**)&pView2)) && pView2) {
            IPersistFolder2* pFolder = nullptr;
            if (SUCCEEDED(pView2->GetFolder(IID_IPersistFolder2, (void**)&pFolder)) && pFolder) {
                IShellFolder2* pFolder2 = nullptr;
                if (SUCCEEDED(pFolder->QueryInterface(IID_IShellFolder2, (void**)&pFolder2)) && pFolder2) {
                    int selCount = 0, totalCount = 0;
                    pView2->ItemCount(SVGIO_SELECTION, &selCount);
                    pView2->ItemCount(SVGIO_ALLVIEW, &totalCount);
                    
                    UINT type = (selCount > 0) ? SVGIO_SELECTION : SVGIO_ALLVIEW;
                    
                    IEnumIDList* pEnum = nullptr;
                    if ((totalCount < 10000 || selCount < 1000) && 
                        SUCCEEDED(pView2->Items(type, IID_IEnumIDList, (void**)&pEnum)) && pEnum) {
                        
                        PITEMID_CHILD child;
                        SHCOLUMNID column;
                        column.fmtid = PKEY_Size.fmtid;
                        column.pid = PKEY_Size.pid;
                        
                        int index = 0;
                        while (pEnum->Next(1, &child, NULL) == S_OK) {
                            index++;
                            if ((index % 100) == 0 && (GetTickCount() - time0) > 500) {
                                ILFree(child);
                                bMore = true;
                                break;
                            }
                            VARIANT var;
                            VariantInit(&var);
                            if (SUCCEEDED(pFolder2->GetDetailsEx(child, &column, &var)) && var.vt == VT_UI8) {
                                if (fileSize < 0)
                                    fileSize = var.ullVal;
                                else
                                    fileSize += var.ullVal;
                            }
                            VariantClear(&var);
                            ILFree(child);
                        }
                        pEnum->Release();
                    }
                    pFolder2->Release();
                }
                pFolder->Release();
            }
            pView2->Release();
        }
        pView->Release();
    }
    
    if (fileSize >= 0) {
        StrFormatByteSize64(fileSize, buf, size);
        if (bMore)
            wcscat_s(buf, size, L"+");
    }
}

void UpdateStatusBar(StatusBarData* pData) {
    if (!pData || !pData->statusBar) return;
    
    if (!pData->pBrowser) {
        pData->pBrowser = GetShellBrowser(pData->explorerWnd);
    }
    
    if (!pData->pBrowser) return;
    
    wchar_t textBuf[512];
    wchar_t freeBuf[128];
    wchar_t sizeBuf[128];
    
    GetStatusText(pData->pBrowser, textBuf, _countof(textBuf), freeBuf, _countof(freeBuf));
    GetFileSize(pData->pBrowser, sizeBuf, _countof(sizeBuf));
    
    bool hasFree = freeBuf[0] != 0;
    bool hasSize = sizeBuf[0] != 0;
    
    RECT rc;
    GetClientRect(pData->statusBar, &rc);
    int width = rc.right;
    int height = GetStatusBarHeight(pData->statusBar);
    
    if (hasSize && hasFree) {
        int parts[] = {width - height - pData->fileSizeWidth - pData->freeSpaceWidth, 
                       width - height - pData->fileSizeWidth, -1};
        SendMessage(pData->statusBar, SB_SETPARTS, 3, (LPARAM)parts);
        SendMessage(pData->statusBar, SB_SETTEXT, PART_TEXT, (LPARAM)textBuf);
        SendMessage(pData->statusBar, SB_SETTEXT, PART_FREE, (LPARAM)freeBuf);
        SendMessage(pData->statusBar, SB_SETTEXT, PART_SIZE, (LPARAM)sizeBuf);
    } else if (hasFree) {
        int parts[] = {width - height - pData->freeSpaceWidth, -1};
        SendMessage(pData->statusBar, SB_SETPARTS, 2, (LPARAM)parts);
        SendMessage(pData->statusBar, SB_SETTEXT, 0, (LPARAM)textBuf);
        SendMessage(pData->statusBar, SB_SETTEXT, 1, (LPARAM)freeBuf);
    } else if (hasSize) {
        int parts[] = {width - height - pData->fileSizeWidth, -1};
        SendMessage(pData->statusBar, SB_SETPARTS, 2, (LPARAM)parts);
        SendMessage(pData->statusBar, SB_SETTEXT, 0, (LPARAM)textBuf);
        SendMessage(pData->statusBar, SB_SETTEXT, 1, (LPARAM)sizeBuf);
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
        Wh_Log(L"Timer fired, updating status bar");
        
        if (!pData->shellDefView) {
            pData->shellDefView = FindShellDefView(pData->explorerWnd);
            if (pData->shellDefView) {
                Wh_Log(L"Found SHELLDLL_DefView: %p", pData->shellDefView);
                SetWindowSubclass(pData->shellDefView, SubclassShellViewProc, (UINT_PTR)pData->shellDefView, (DWORD_PTR)pData);
            }
        }
        
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
            int height = GetStatusBarHeight(pData->statusBar);
            pPos->cy -= height;
            SetWindowPos(pData->statusBar, NULL, pPos->x, pPos->y + pPos->cy, pPos->cx, height, SWP_NOZORDER);
            
            // Запрашиваем полное обновление статус-бара после изменения размера
            PostMessage(pData->statusBar, WM_UPDATE_STATUSBAR, 0, 0);
        }
    }
    
    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, SubclassDUIViewProc, uIdSubclass);
        if (pData->shellDefView) {
            RemoveWindowSubclass(pData->shellDefView, SubclassShellViewProc, (UINT_PTR)pData->shellDefView);
        }
        if (pData->pBrowser) {
            pData->pBrowser->Release();
        }
        RemoveProp(pData->explorerWnd, STATUSBAR_DATA_PROP);
        delete pData;
    }
    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK SubclassShellViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                                        UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    StatusBarData* pData = (StatusBarData*)dwRefData;
    
    if (uMsg == WM_NOTIFY) {
        NMHDR* pNMHdr = (NMHDR*)lParam;
        if (pNMHdr->code == LVN_ITEMCHANGED) {
            NMLISTVIEW* pNMLV = (NMLISTVIEW*)lParam;
            if (pNMLV->uChanged & LVIF_STATE) {
                if ((pNMLV->uOldState & LVIS_SELECTED) != (pNMLV->uNewState & LVIS_SELECTED)) {
                    PostMessage(pData->statusBar, WM_CLEAR, 0, 0);
                }
            }
        }
    }
    
    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, SubclassShellViewProc, uIdSubclass);
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
    
    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"DUIViewWndClassName")) {
        HWND explorerWnd = GetAncestor(hWnd, GA_ROOT);
        wchar_t name[256];
        if (GetClassName(explorerWnd, name, _countof(name)) && _wcsicmp(name, L"CabinetWClass") == 0) {
            Wh_Log(L"Found DUIViewWndClassName: %p, Explorer: %p", hWnd, explorerWnd);
            
            HWND parentWnd = GetParent(hWnd);
            
            RECT rc;
            GetClientRect(parentWnd, &rc);
            
            HWND statusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL,
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SBARS_SIZEGRIP,
                0, 0, rc.right, 0,
                parentWnd, NULL, NULL, NULL);
            
            if (statusBar) {
                SendMessage(statusBar, WM_SIZE, 0, 0);
                
                int height = GetStatusBarHeight(statusBar);
                
                SetWindowPos(statusBar, NULL, 0, rc.bottom - height, 
                            rc.right, height, SWP_NOZORDER);
                
                Wh_Log(L"Created status bar: %p, parent: %p, height: %d", statusBar, parentWnd, height);
                
                HDC hdc = GetDC(statusBar);
                SIZE size = {0};
                
                GetTextExtentPoint32(hdc, L"999 MB", 6, &size);
                int fileSizeWidth = size.cx;
                if (fileSizeWidth < 50) fileSizeWidth = 50;
                
                GetTextExtentPoint32(hdc, L"xxxxxxxxx: 999 GB", 17, &size);
                int freeSpaceWidth = size.cx;
                if (freeSpaceWidth < 70) freeSpaceWidth = 70;
                
                ReleaseDC(statusBar, hdc);
                
                StatusBarData* pData = new StatusBarData();
                pData->statusBar = statusBar;
                pData->dUIView = hWnd;
                pData->shellDefView = NULL;
                pData->explorerWnd = explorerWnd;
                pData->fileSizeWidth = fileSizeWidth;
                pData->freeSpaceWidth = freeSpaceWidth;
                pData->pBrowser = nullptr;
                
                SetProp(explorerWnd, STATUSBAR_DATA_PROP, (HANDLE)pData);
                
                SetWindowSubclass(statusBar, SubclassStatusProc, (UINT_PTR)statusBar, (DWORD_PTR)pData);
                SetWindowSubclass(hWnd, SubclassDUIViewProc, (UINT_PTR)hWnd, (DWORD_PTR)pData);
                
                SetTimer(statusBar, 1, 200, NULL);
            }
        }
        
    }
    return hWnd;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, 
                        (void**)&CreateWindowExW_Original);
    
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
