// ==WindhawkMod==
// @id hide-unc
// @name Hide Network UNC Paths in Explorer
// @description Removes UNC paths from network drives in Explorer and dialogs
// @version 1.0.5
// @author @danalec
// @github https://github.com/danalec
// @include explorer.exe
// @include *
// @exclude csrss.exe
// @exclude dwm.exe
// @exclude winlogon.exe
// @exclude services.exe
// @exclude lsass.exe
// @exclude smss.exe
// @exclude wininit.exe
// @exclude conhost.exe
// @exclude fontdrvhost.exe
// @exclude audiodg.exe
// @exclude wmic.exe
// @exclude wmiapsrv.exe
// @exclude wmiprvse.exe
// @exclude alg.exe
// @exclude nvcplui.exe
// @exclude nvcontainer.exe
// @compilerOptions -lgdi32 -lcomctl32 -lcomdlg32 -lshell32 -lole32 -lshlwapi -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
UNC (Universal Naming Convention) paths are standardized formats for accessing network resources in Windows, typically starting with \\ (e.g., \\server\share\file).
They specify the server, share, and file location without relying on local drive letters.

This mod hides the UNC paths (e.g., ` (\\\server\\share)`) from network drive labels in Windows Explorer, file open/save dialogs, and other UI elements.

It keeps the display clean by showing only the drive name or letter, reducing visual clutter in File Explorer and dialogs, making navigation faster and less overwhelming.
Improved Usability for Cryptomator: Hides lengthy encrypted vault paths (e.g., simplifying "\\\vault-network-drive\Vault\encrypted" to just "Vault (Z:)"), enhancing readability during file access or backups.
*/
// ==/WindhawkModReadme==

#define COBJMACROS
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <propkey.h>
#include <propvarutil.h>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <CommCtrl.h>
#include <windhawk_utils.h>

std::atomic<BOOL> g_GetDisplayNameHooked(FALSE);
std::atomic<BOOL> g_GetDisplayNameOfHooked(FALSE);
std::vector<HWND> g_subclassedWindows;
std::mutex g_subclassedWindowsMutex;

void CleanUNCPath(LPWSTR text, size_t maxLen) {
    if (!text || !*text || maxLen < 4) return;
    if (wcsncmp(text, L"(\\\\", 3) == 0) return;

    WCHAR* unc = wcsstr(text, L" (\\\\");
    if (!unc) return;

    WCHAR* closing = wcschr(unc, L')');
    if (closing) {
        size_t remaining = wcslen(closing + 1);
        size_t availableSpace = maxLen - (unc - text) - 1;
        size_t moveLen = std::min(remaining, availableSpace);
        wmemmove(unc, closing + 1, moveLen);
        unc[moveLen] = L'\0';
    } else {
        *unc = L'\0';
    }
}

typedef HRESULT (WINAPI *SHCreateItemFromParsingName_t)(PCWSTR pszPath, IBindCtx *pbc, REFIID riid, void **ppv);
SHCreateItemFromParsingName_t SHCreateItemFromParsingName_Original;

typedef HRESULT (STDMETHODCALLTYPE *GetDisplayName_t)(void* pThis, SIGDN sigdnName, LPWSTR *ppszName);
GetDisplayName_t GetDisplayName_Original = NULL;

HRESULT STDMETHODCALLTYPE GetDisplayName_Hook(void* pThis, SIGDN sigdnName, LPWSTR *ppszName) {
    HRESULT hr = GetDisplayName_Original(pThis, sigdnName, ppszName);
    if (SUCCEEDED(hr) && ppszName && *ppszName && wcsstr(*ppszName, L" (\\\\")) {
        size_t len = wcslen(*ppszName) + 1;
        WCHAR* cleanCopy = (WCHAR*)CoTaskMemAlloc(len * sizeof(WCHAR));
        if (cleanCopy) {
            wcscpy_s(cleanCopy, len, *ppszName);
            CleanUNCPath(cleanCopy, len);
            CoTaskMemFree(*ppszName);
            *ppszName = cleanCopy;
        }
    }
    return hr;
}

HRESULT WINAPI SHCreateItemFromParsingName_Hook(PCWSTR pszPath, IBindCtx *pbc, REFIID riid, void **ppv) {
    HRESULT hr = SHCreateItemFromParsingName_Original(pszPath, pbc, riid, ppv);
    if (SUCCEEDED(hr) && ppv && *ppv && !g_GetDisplayNameHooked.load()) {
        void*** vtbl = (void***)*ppv;
        if (vtbl && *vtbl) {
            if (Wh_SetFunctionHook((*vtbl)[5], (void*)GetDisplayName_Hook, (void**)&GetDisplayName_Original)) {
                if (Wh_ApplyHookOperations()) {
                    g_GetDisplayNameHooked.store(TRUE);
                }
            }
        }
    }
    return hr;
}

typedef LRESULT (WINAPI *SendMessageW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
SendMessageW_t SendMessageW_Original;

LRESULT WINAPI SendMessageW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_NOTIFY) {
        NMHDR* pNmhdr = (NMHDR*)lParam;
        if (pNmhdr) {
            if (pNmhdr->code == LVN_GETDISPINFOW) {
                NMLVDISPINFOW* pDispInfo = (NMLVDISPINFOW*)pNmhdr;
                LRESULT result = SendMessageW_Original(hWnd, Msg, wParam, lParam);
                if (pDispInfo->item.mask & LVIF_TEXT && pDispInfo->item.pszText && pDispInfo->item.pszText != LPSTR_TEXTCALLBACKW) {
                    CleanUNCPath(pDispInfo->item.pszText, pDispInfo->item.cchTextMax);
                }
                return result;
            } else if (pNmhdr->code == TVN_GETDISPINFOW) {
                NMTVDISPINFOW* pDispInfo = (NMTVDISPINFOW*)pNmhdr;
                LRESULT result = SendMessageW_Original(hWnd, Msg, wParam, lParam);
                if (pDispInfo->item.mask & TVIF_TEXT && pDispInfo->item.pszText && pDispInfo->item.pszText != LPSTR_TEXTCALLBACKW) {
                    CleanUNCPath(pDispInfo->item.pszText, pDispInfo->item.cchTextMax);
                }
                return result;
            }
        }
    }

    WCHAR className[64];
    GetClassNameW(hWnd, className, 64);

    if (wcsstr(className, L"ListView") || wcscmp(className, L"SysListView32") == 0) {
        if (Msg == LVM_SETITEMTEXTW || Msg == LVM_SETITEMW || Msg == LVM_INSERTITEMW) {
            LVITEMW* pItem = (LVITEMW*)lParam;
            if (pItem && pItem->pszText && pItem->pszText != LPSTR_TEXTCALLBACKW && wcsstr(pItem->pszText, L" (\\\\")) {
                size_t len = wcslen(pItem->pszText) + 1;
                WCHAR* buffer = (WCHAR*)malloc(len * sizeof(WCHAR));
                if (buffer) {
                    wcscpy_s(buffer, len, pItem->pszText);
                    CleanUNCPath(buffer, len);
                    LVITEMW itemCopy = *pItem;
                    itemCopy.pszText = buffer;
                    LRESULT result = SendMessageW_Original(hWnd, Msg, wParam, (LPARAM)&itemCopy);
                    free(buffer);
                    return result;
                }
            }
        }
    } else if (wcsstr(className, L"TreeView") || wcscmp(className, L"SysTreeView32") == 0) {
        if (Msg == TVM_INSERTITEMW) {
            TVINSERTSTRUCTW* pInsert = (TVINSERTSTRUCTW*)lParam;
            if (pInsert && pInsert->item.pszText && pInsert->item.pszText != LPSTR_TEXTCALLBACKW && wcsstr(pInsert->item.pszText, L" (\\\\")) {
                size_t len = wcslen(pInsert->item.pszText) + 1;
                WCHAR* buffer = (WCHAR*)malloc(len * sizeof(WCHAR));
                if (buffer) {
                    wcscpy_s(buffer, len, pInsert->item.pszText);
                    CleanUNCPath(buffer, len);
                    TVINSERTSTRUCTW insertCopy = *pInsert;
                    insertCopy.item.pszText = buffer;
                    LRESULT result = SendMessageW_Original(hWnd, Msg, wParam, (LPARAM)&insertCopy);
                    free(buffer);
                    return result;
                }
            }
        } else if (Msg == TVM_SETITEMW) {
            TVITEMW* pItem = (TVITEMW*)lParam;
            if (pItem && pItem->pszText && pItem->pszText != LPSTR_TEXTCALLBACKW && wcsstr(pItem->pszText, L" (\\\\")) {
                size_t len = wcslen(pItem->pszText) + 1;
                WCHAR* buffer = (WCHAR*)malloc(len * sizeof(WCHAR));
                if (buffer) {
                    wcscpy_s(buffer, len, pItem->pszText);
                    CleanUNCPath(buffer, len);
                    TVITEMW itemCopy = *pItem;
                    itemCopy.pszText = buffer;
                    LRESULT result = SendMessageW_Original(hWnd, Msg, wParam, (LPARAM)&itemCopy);
                    free(buffer);
                    return result;
                }
            }
        }
    }
    
    LRESULT result = SendMessageW_Original(hWnd, Msg, wParam, lParam);

    if (Msg == LVM_GETITEMTEXTW || Msg == LVM_GETITEMW) {
        if (wcsstr(className, L"ListView") || wcscmp(className, L"SysListView32") == 0) {
            LVITEMW* pItem = (LVITEMW*)lParam;
            if (pItem && pItem->pszText && pItem->pszText != LPSTR_TEXTCALLBACKW) {
                CleanUNCPath(pItem->pszText, pItem->cchTextMax);
            }
        }
    } else if (Msg == TVM_GETITEMW) {
        if (wcsstr(className, L"TreeView") || wcscmp(className, L"SysTreeView32") == 0) {
            TVITEMW* pItem = (TVITEMW*)lParam;
            if (pItem && pItem->pszText && pItem->pszText != LPSTR_TEXTCALLBACKW) {
                CleanUNCPath(pItem->pszText, pItem->cchTextMax);
            }
        }
    }
    
    return result;
}

typedef DWORD_PTR (WINAPI *SHGetFileInfoW_t)(LPCWSTR pszPath, DWORD dwFileAttributes, SHFILEINFOW *psfi, UINT cbFileInfo, UINT uFlags);
SHGetFileInfoW_t SHGetFileInfoW_Original;

DWORD_PTR WINAPI SHGetFileInfoW_Hook(LPCWSTR pszPath, DWORD dwFileAttributes, SHFILEINFOW *psfi, UINT cbFileInfo, UINT uFlags) {
    DWORD_PTR result = SHGetFileInfoW_Original(pszPath, dwFileAttributes, psfi, cbFileInfo, uFlags);
    if (result && psfi && (uFlags & SHGFI_DISPLAYNAME)) {
        CleanUNCPath(psfi->szDisplayName, sizeof(psfi->szDisplayName) / sizeof(WCHAR));
    }
    return result;
}

typedef HRESULT (STDMETHODCALLTYPE *GetDisplayNameOf_t)(void* pThis, PCUITEMID_CHILD pidl, SHGDNF uFlags, STRRET *pName);
GetDisplayNameOf_t GetDisplayNameOf_Original = NULL;

HRESULT STDMETHODCALLTYPE GetDisplayNameOf_Hook(void* pThis, PCUITEMID_CHILD pidl, SHGDNF uFlags, STRRET *pName) {
    HRESULT hr = GetDisplayNameOf_Original(pThis, pidl, uFlags, pName);
    if (SUCCEEDED(hr) && pName) {
        if (pName->uType == STRRET_WSTR && pName->pOleStr && wcsstr(pName->pOleStr, L" (\\\\")) {
            size_t len = wcslen(pName->pOleStr) + 1;
            WCHAR* buffer = (WCHAR*)CoTaskMemAlloc(len * sizeof(WCHAR));
            if (buffer) {
                wcscpy_s(buffer, len, pName->pOleStr);
                CleanUNCPath(buffer, len);
                CoTaskMemFree(pName->pOleStr);
                pName->pOleStr = buffer;
            }
        } else if (pName->uType == STRRET_CSTR) {
            WCHAR szDisplay[32768];
            int wideLen = MultiByteToWideChar(CP_ACP, 0, pName->cStr, -1, szDisplay, 32768);
            if (wideLen > 0 && wcsstr(szDisplay, L" (\\\\")) {
                CleanUNCPath(szDisplay, 32768);
                WideCharToMultiByte(CP_ACP, 0, szDisplay, -1, pName->cStr, sizeof(pName->cStr), NULL, NULL);
            }
        }
    }
    return hr;
}

typedef HRESULT (WINAPI *SHGetDesktopFolder_t)(IShellFolder **ppshf);
SHGetDesktopFolder_t SHGetDesktopFolder_Original;

HRESULT WINAPI SHGetDesktopFolder_Hook(IShellFolder **ppshf) {
    HRESULT hr = SHGetDesktopFolder_Original(ppshf);
    if (SUCCEEDED(hr) && ppshf && *ppshf && !g_GetDisplayNameOfHooked.load()) {
        void*** vtbl = (void***)*ppshf;
        if (vtbl && *vtbl) {
            if (Wh_SetFunctionHook((*vtbl)[11], (void*)GetDisplayNameOf_Hook, (void**)&GetDisplayNameOf_Original)) {
                if (Wh_ApplyHookOperations()) {
                    g_GetDisplayNameOfHooked.store(TRUE);
                }
            }
        }
    }
    return hr;
}

typedef int (WINAPI *DrawTextW_t)(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format);
DrawTextW_t DrawTextW_Original;

int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    if (lpchText && wcsstr(lpchText, L" (\\\\")) {
        int len = (cchText == -1) ? (int)wcslen(lpchText) : std::min(cchText, (int)wcslen(lpchText));
        WCHAR* buffer = (WCHAR*)malloc((len + 1) * sizeof(WCHAR));
        if (buffer) {
            wcsncpy_s(buffer, len + 1, lpchText, len);
            buffer[len] = L'\0';
            CleanUNCPath(buffer, len + 1);
            int result = DrawTextW_Original(hdc, buffer, -1, lprc, format);
            free(buffer);
            return result;
        }
    }
    return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
}

typedef BOOL (WINAPI *ExtTextOutW_t)(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx);
ExtTextOutW_t ExtTextOutW_Original;

BOOL WINAPI ExtTextOutW_Hook(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx) {
    if (lpString && wcsstr(lpString, L" (\\\\")) {
        size_t len = c ? c : wcslen(lpString);
        WCHAR* buffer = (WCHAR*)malloc((len + 1) * sizeof(WCHAR));
        if (buffer) {
            wcsncpy_s(buffer, len + 1, lpString, len);
            buffer[len] = L'\0';
            CleanUNCPath(buffer, len + 1);
            BOOL result = ExtTextOutW_Original(hdc, x, y, options, lprect, buffer, (UINT)wcslen(buffer), lpDx);
            free(buffer);
            return result;
        }
    }
    return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
}

typedef BOOL (WINAPI *SetWindowTextW_t)(HWND hWnd, LPCWSTR lpString);
SetWindowTextW_t SetWindowTextW_Original;

BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR lpString) {
    if (lpString && wcsstr(lpString, L" (\\\\")) {
        size_t len = wcslen(lpString);
        WCHAR* buffer = (WCHAR*)malloc((len + 1) * sizeof(WCHAR));
        if (buffer) {
            wcscpy_s(buffer, len + 1, lpString);
            CleanUNCPath(buffer, len + 1);
            BOOL result = SetWindowTextW_Original(hWnd, buffer);
            free(buffer);
            return result;
        }
    }
    return SetWindowTextW_Original(hWnd, lpString);
}

LRESULT CALLBACK ComboBoxSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData
) {
    if (uMsg == CB_ADDSTRING || uMsg == CB_INSERTSTRING) {
        LPWSTR text = (LPWSTR)lParam;
        if (text && wcsstr(text, L" (\\\\")) {
            size_t len = wcslen(text) + 1;
            WCHAR* buffer = (WCHAR*)malloc(len * sizeof(WCHAR));
            if (buffer) {
                wcscpy_s(buffer, len, text);
                CleanUNCPath(buffer, len);
                LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, (LPARAM)buffer);
                free(buffer);
                return result;
            }
        }
    } else if (uMsg == WM_NCDESTROY) {
        std::lock_guard lock(g_subclassedWindowsMutex);
        g_subclassedWindows.erase(
            std::remove(g_subclassedWindows.begin(), g_subclassedWindows.end(), hWnd),
            g_subclassedWindows.end()
        );
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef HWND (WINAPI *CreateWindowExW_t)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                        DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && lpClassName) {
        WCHAR className[256] = {0};
        if (IS_INTRESOURCE(lpClassName)) {
            GetClassNameW(hWnd, className, 256);
        } else {
            wcscpy_s(className, 256, lpClassName);
        }

        if (wcscmp(className, L"ComboBox") == 0 && hWndParent) {
            WCHAR parentClass[256];
            GetClassNameW(hWndParent, parentClass, 256);
            if (wcsstr(parentClass, L"#32770")) {
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ComboBoxSubclassProc, 0)) {
                    std::lock_guard lock(g_subclassedWindowsMutex);
                    g_subclassedWindows.push_back(hWnd);
                }
            }
        }
    }
    return hWnd;
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)SHGetFileInfoW, (void*)SHGetFileInfoW_Hook, (void**)&SHGetFileInfoW_Original);
    Wh_SetFunctionHook((void*)SHCreateItemFromParsingName, (void*)SHCreateItemFromParsingName_Hook, (void**)&SHCreateItemFromParsingName_Original);
    Wh_SetFunctionHook((void*)SHGetDesktopFolder, (void*)SHGetDesktopFolder_Hook, (void**)&SHGetDesktopFolder_Original);
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    Wh_SetFunctionHook((void*)SendMessageW, (void*)SendMessageW_Hook, (void**)&SendMessageW_Original);
    Wh_SetFunctionHook((void*)DrawTextW, (void*)DrawTextW_Hook, (void**)&DrawTextW_Original);
    Wh_SetFunctionHook((void*)ExtTextOutW, (void*)ExtTextOutW_Hook, (void**)&ExtTextOutW_Original);
    Wh_SetFunctionHook((void*)SetWindowTextW, (void*)SetWindowTextW_Hook, (void**)&SetWindowTextW_Original);
    return TRUE;
}

void Wh_ModUninit() {
    std::vector<HWND> windowsToRemove;
    {
        std::lock_guard lock(g_subclassedWindowsMutex);
        windowsToRemove = g_subclassedWindows;
        g_subclassedWindows.clear();
    }

    for (HWND hWnd : windowsToRemove) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, ComboBoxSubclassProc);
    }
}
