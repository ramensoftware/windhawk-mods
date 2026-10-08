// ==WindhawkMod==
// @id           hide-extension-during-rename
// @name         Hide Extension during renaming
// @description  Hides file extensions in Save As dialogs and during File Explorer/Desktop renaming.
// @version      1.0
// @author       AuralSX
// @github       https://github.com/AuralSX
// @include      *
// @compilerOptions -luser32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Ever wanted to get rid of the extension when trying to save a file on your computer or rename something on your PC
well your days of suffering are over because I give you the solution ma dude
This mod hides file extensions during inline file renaming in File Explorer, Desktop and "save as" operations.

### Key Features
* **Save As Dialogs:**      Automatically hides file extensions from the default filename text box when saving files.
* **Inline File Renaming:** Hides file extensions while editing item names on the Desktop or inside File Explorer (`SHELLDLL_DefView`, `SysListView32`, `DirectUIHWND`).
* **Warning Prevention:**   Intercepts window messages (`WM_GETTEXT`) upon committing a rename to silently re-attach the original extension,
                            avoiding the native Windows "If you change a file name extension, the file might become unusable" warning prompt.

![Mod Demo](https://i.imgur.com/iy9dyzq.gif)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shlwapi.h>
#include <string>

#define PROP_ORIG_PROC L"Wh_OrigWndProc"
#define PROP_FILE_EXT  L"Wh_FileExt"

// --- Helper: Extracts extension from filename string (e.g., "file.txt" -> ".txt") ---
std::wstring GetExtension(const wchar_t* path) {
    if (!path || *path == L'\0') return L"";
    
    std::wstring str = path;
    size_t lastDot = str.find_last_of(L".");
    size_t lastSlash = str.find_last_of(L"/\\");

    if (lastDot != std::wstring::npos && 
       (lastSlash == std::wstring::npos || lastDot > lastSlash)) {
        if (lastDot > 0 && str[lastDot - 1] != L'/' && str[lastDot - 1] != L'\\') {
            return str.substr(lastDot);
        }
    }
    return L"";
}

// --- Helper: Strips extension from filename string ---
std::wstring StripExtension(const wchar_t* path) {
    if (!path || *path == L'\0') return L"";
    
    std::wstring result = path;
    size_t lastDot = result.find_last_of(L".");
    size_t lastSlash = result.find_last_of(L"/\\");

    if (lastDot != std::wstring::npos && 
       (lastSlash == std::wstring::npos || lastDot > lastSlash)) {
        if (lastDot > 0 && result[lastDot - 1] != L'/' && result[lastDot - 1] != L'\\') {
            result = result.substr(0, lastDot);
        }
    }
    return result;
}

// Custom Window Procedure for Inline Rename Edit Controls
LRESULT CALLBACK RenameEditWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WNDPROC origProc = (WNDPROC)GetPropW(hWnd, PROP_ORIG_PROC);
    if (!origProc) return DefWindowProcW(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_GETTEXT) {
        LRESULT res = CallWindowProcW(origProc, hWnd, uMsg, wParam, lParam);
        wchar_t* pExt = (wchar_t*)GetPropW(hWnd, PROP_FILE_EXT);
        if (res > 0 && pExt && *pExt != L'\0') {
            wchar_t* buf = reinterpret_cast<wchar_t*>(lParam);
            int maxLen = static_cast<int>(wParam);
            
            if (buf && GetExtension(buf).empty()) {
                std::wstring full = std::wstring(buf) + pExt;
                wcsncpy_s(buf, maxLen, full.c_str(), _TRUNCATE);
                return full.length();
            }
        }
        return res;
    }
    else if (uMsg == WM_GETTEXTLENGTH) {
        LRESULT res = CallWindowProcW(origProc, hWnd, uMsg, wParam, lParam);
        wchar_t* pExt = (wchar_t*)GetPropW(hWnd, PROP_FILE_EXT);
        if (pExt && *pExt != L'\0') {
            wchar_t buf[MAX_PATH] = {0};
            CallWindowProcW(origProc, hWnd, WM_GETTEXT, MAX_PATH, reinterpret_cast<LPARAM>(buf));
            if (GetExtension(buf).empty()) {
                return res + wcslen(pExt);
            }
        }
        return res;
    }
    else if (uMsg == WM_NCDESTROY) {
        wchar_t* pExt = (wchar_t*)GetPropW(hWnd, PROP_FILE_EXT);
        if (pExt) delete[] pExt;
        RemovePropW(hWnd, PROP_ORIG_PROC);
        RemovePropW(hWnd, PROP_FILE_EXT);
        SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)origProc);
        return CallWindowProcW(origProc, hWnd, uMsg, wParam, lParam);
    }

    return CallWindowProcW(origProc, hWnd, uMsg, wParam, lParam);
}

// Detect if an edit control is inside a Save As / Open file dialog
BOOL IsSaveDialogEditControl(HWND hWnd) {
    if (!hWnd) return FALSE;

    wchar_t className[64] = {0};
    GetClassNameW(hWnd, className, 64);
    if (wcsicmp(className, L"Edit") != 0 && wcsnicmp(className, L"RichEdit", 8) != 0) {
        return FALSE;
    }

    int controlId = GetDlgCtrlID(hWnd);
    HWND hParent = GetParent(hWnd);
    while (hParent) {
        wchar_t parentClass[64] = {0};
        GetClassNameW(hParent, parentClass, 64);

        if (controlId == 1152 || controlId == 1001) {
            if (wcscmp(parentClass, L"#32770") == 0 || wcscmp(parentClass, L"DirectUIHWND") == 0) {
                return TRUE;
            }
        }
        hParent = GetParent(hParent);
    }
    return FALSE;
}

// Detect if an edit control is an inline file/desktop rename box
BOOL IsRenameEditControl(HWND hWnd) {
    if (!hWnd) return FALSE;

    wchar_t className[64] = {0};
    GetClassNameW(hWnd, className, 64);
    if (wcsicmp(className, L"Edit") != 0 && wcsnicmp(className, L"RichEdit", 8) != 0) {
        return FALSE;
    }

    HWND hParent = GetParent(hWnd);
    while (hParent) {
        wchar_t parentClass[64] = {0};
        GetClassNameW(hParent, parentClass, 64);

        // EXCLUDE: Address bar or search box controls in File Explorer
        if (wcscmp(parentClass, L"ComboBoxEx32") == 0 ||
            wcscmp(parentClass, L"ReBarWindow32") == 0 ||
            wcscmp(parentClass, L"Address Band Root") == 0) {
            return FALSE;
        }

        // INCLUDE: File Explorer item containers, tree controls, and Desktop
        if (wcscmp(parentClass, L"SHELLDLL_DefView") == 0 ||
            wcscmp(parentClass, L"SysListView32") == 0 ||
            wcscmp(parentClass, L"DirectUIHWND") == 0 ||
            wcscmp(parentClass, L"UIPropertyMainClass") == 0 ||
            wcscmp(parentClass, L"NamespaceTreeControl") == 0) {
            return TRUE;
        }
        hParent = GetParent(hParent);
    }
    return FALSE;
}

// --- Hook: SetWindowTextW ---
typedef BOOL (WINAPI *SetWindowTextW_t)(HWND hWnd, LPCWSTR lpString);
SetWindowTextW_t SetWindowTextW_Original = nullptr;

BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR lpString) {
    if (lpString) {
        // CASE 1: Save As Dialogs -> Strip extension completely
        if (IsSaveDialogEditControl(hWnd)) {
            std::wstring stripped = StripExtension(lpString);
            return SetWindowTextW_Original(hWnd, stripped.c_str());
        }
        
        // CASE 2: File Explorer / Desktop Renaming -> Subclass window to intercept WM_GETTEXT
        if (IsRenameEditControl(hWnd)) {
            std::wstring ext = GetExtension(lpString);
            if (!ext.empty()) {
                if (!GetPropW(hWnd, PROP_ORIG_PROC)) {
                    WNDPROC origProc = (WNDPROC)GetWindowLongPtrW(hWnd, GWLP_WNDPROC);
                    if (origProc && origProc != RenameEditWndProc) {
                        SetPropW(hWnd, PROP_ORIG_PROC, (HANDLE)origProc);
                        
                        wchar_t* pExtCopy = new wchar_t[ext.length() + 1];
                        wcscpy_s(pExtCopy, ext.length() + 1, ext.c_str());
                        SetPropW(hWnd, PROP_FILE_EXT, (HANDLE)pExtCopy);

                        SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)RenameEditWndProc);
                    }
                }

                std::wstring stripped = StripExtension(lpString);
                return SetWindowTextW_Original(hWnd, stripped.c_str());
            }
        }
    }
    return SetWindowTextW_Original(hWnd, lpString);
}

// --- Windhawk Entry Points ---
BOOL Wh_ModInit() {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        void* pSetWindowTextW = (void*)GetProcAddress(hUser32, "SetWindowTextW");
        if (pSetWindowTextW) {
            Wh_SetFunctionHook(pSetWindowTextW, (void*)SetWindowTextW_Hook, (void**)&SetWindowTextW_Original);
        }
    }
    return TRUE;
}

void Wh_ModUninit() {
    // Hooks automatically removed by Windhawk engine
}
