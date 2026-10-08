// ==WindhawkMod==
// @id           hide-extension-during-rename
// @name         Hide Extension during renaming
// @description  Hides file extensions in Save As dialogs and during File Explorer/Desktop renaming.
// @version      1.0
// @author       AuralSX
// @github       https://github.com/AuralSX
// @include      *
// @compilerOptions -luser32 -lshlwapi -lcomctl32
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
#include <commctrl.h>
#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>

#include <windhawk_utils.h>

// Global tracking map for active subclassed edit controls
std::mutex g_editsMutex;
std::unordered_map<HWND, std::wstring> g_hiddenExt;

// Subclass procedure matching WindhawkUtils WH_SUBCLASSPROC (5 parameters)
LRESULT CALLBACK RenameEditSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData
) {
    switch (uMsg) {
        case WM_GETTEXT: {
            LRESULT len = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            if (len > 0 && wParam > 0 && lParam != 0) {
                wchar_t* buf = (wchar_t*)lParam;
                std::wstring ext;
                {
                    std::lock_guard<std::mutex> lock(g_editsMutex);
                    auto it = g_hiddenExt.find(hWnd);
                    if (it != g_hiddenExt.end()) {
                        ext = it->second;
                    }
                }

                if (!ext.empty()) {
                    std::wstring full = buf;
                    if (!full.empty()) {
                        // Re-attach extension if the text does not already end with it
                        bool alreadyHasExt = false;
                        if (full.length() >= ext.length()) {
                            std::wstring suffix = full.substr(full.length() - ext.length());
                            if (_wcsicmp(suffix.c_str(), ext.c_str()) == 0) {
                                alreadyHasExt = true;
                            }
                        }

                        if (!alreadyHasExt) {
                            full += ext;
                            wcsncpy_s(buf, wParam, full.c_str(), _TRUNCATE);
                            return wcslen(buf);
                        }
                    }
                }
            }
            return len;
        }

        case WM_GETTEXTLENGTH: {
            LRESULT len = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            if (len > 0) {
                std::wstring ext;
                {
                    std::lock_guard<std::mutex> lock(g_editsMutex);
                    auto it = g_hiddenExt.find(hWnd);
                    if (it != g_hiddenExt.end()) {
                        ext = it->second;
                    }
                }
                if (!ext.empty()) {
                    int bufLen = (int)len + 1;
                    std::wstring currentText(bufLen, L'\0');
                    DefSubclassProc(hWnd, WM_GETTEXT, (WPARAM)bufLen, (LPARAM)&currentText[0]);
                    currentText.resize(wcslen(currentText.c_str()));

                    if (!currentText.empty()) {
                        bool alreadyHasExt = false;
                        if (currentText.length() >= ext.length()) {
                            std::wstring suffix = currentText.substr(currentText.length() - ext.length());
                            if (_wcsicmp(suffix.c_str(), ext.c_str()) == 0) {
                                alreadyHasExt = true;
                            }
                        }

                        if (!alreadyHasExt) {
                            return len + ext.length();
                        }
                    }
                }
            }
            return len;
        }

        case WM_NCDESTROY: {
            {
                std::lock_guard<std::mutex> lock(g_editsMutex);
                g_hiddenExt.erase(hWnd);
            }
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, RenameEditSubclassProc);
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Window detection helpers
bool IsClassName(HWND hWnd, const wchar_t* targetClass) {
    wchar_t className[256];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return _wcsicmp(className, targetClass) == 0;
    }
    return false;
}

bool HasAncestorClass(HWND hWnd, const wchar_t* targetClass, int maxLevels = 8) {
    HWND curr = GetParent(hWnd);
    int level = 0;
    while (curr && level < maxLevels) {
        if (IsClassName(curr, targetClass)) return true;
        curr = GetParent(curr);
        level++;
    }
    return false;
}

bool IsSaveDialogEditControl(HWND hWnd) {
    if (!IsClassName(hWnd, L"Edit") && !IsClassName(hWnd, L"RichEdit20W")) {
        return false;
    }

    HWND topDialog = GetAncestor(hWnd, GA_ROOT);
    if (!topDialog || !IsClassName(topDialog, L"#32770")) {
        return false;
    }

    if (FindWindowExW(topDialog, NULL, L"DUIViewWndClassName", NULL) == NULL &&
        FindWindowExW(topDialog, NULL, L"SHELLDLL_DefView", NULL) == NULL) {
        return false;
    }

    if (HasAncestorClass(hWnd, L"Address Band Root") || HasAncestorClass(hWnd, L"TravelBand")) {
        return false;
    }

    int ctrlId = GetDlgCtrlID(hWnd);
    if (ctrlId == 1152 || ctrlId == 1001) {
        return true;
    }

    HWND parent = GetParent(hWnd);
    while (parent && parent != topDialog) {
        int pId = GetDlgCtrlID(parent);
        if (pId == 1148 || pId == 1001) {
            return true;
        }
        parent = GetParent(parent);
    }

    return false;
}

bool IsRenameEditControl(HWND hWnd) {
    if (!IsClassName(hWnd, L"Edit")) {
        return false;
    }

    if (HasAncestorClass(hWnd, L"SHELLDLL_DefView") ||
        HasAncestorClass(hWnd, L"NamespaceTreeControl") ||
        HasAncestorClass(hWnd, L"SysListView32") ||
        HasAncestorClass(hWnd, L"DirectUIHWND")) {

        if (!HasAncestorClass(hWnd, L"Address Band Root") &&
            !HasAncestorClass(hWnd, L"TravelBand") &&
            !HasAncestorClass(hWnd, L"ComboBox")) {

            HWND topWnd = GetAncestor(hWnd, GA_ROOT);
            if (topWnd) {
                if (IsClassName(topWnd, L"CabinetWClass") ||
                    IsClassName(topWnd, L"ExploreWClass") ||
                    IsClassName(topWnd, L"Progman") ||
                    IsClassName(topWnd, L"WorkerW") ||
                    IsClassName(topWnd, L"#32770")) {
                    return true;
                }
            }
        }
    }

    return false;
}

// SetWindowTextW Hook
using SetWindowTextW_t = decltype(&SetWindowTextW);
SetWindowTextW_t SetWindowTextW_Original = nullptr;

BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR lpString) {
    if (lpString && (IsSaveDialogEditControl(hWnd) || IsRenameEditControl(hWnd))) {
        LPCWSTR ext = PathFindExtensionW(lpString);
        if (ext && *ext != L'\0' && wcslen(ext) > 1) {
            std::wstring fullPath(lpString);
            std::wstring baseName = fullPath.substr(0, fullPath.length() - wcslen(ext));
            std::wstring extension(ext);

            {
                std::lock_guard<std::mutex> lock(g_editsMutex);
                g_hiddenExt[hWnd] = extension;
            }

            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, RenameEditSubclassProc, 0);

            return SetWindowTextW_Original(hWnd, baseName.c_str());
        }
    }

    return SetWindowTextW_Original(hWnd, lpString);
}

// Mod initialization and cleanup
BOOL Wh_ModInit() {
    WindhawkUtils::SetFunctionHook(
        SetWindowTextW,
        SetWindowTextW_Hook,
        &SetWindowTextW_Original
    );
    return TRUE;
}

void Wh_ModUninit() {
    std::vector<HWND> edits;
    {
        std::lock_guard<std::mutex> lock(g_editsMutex);
        for (const auto& [hWnd, ext] : g_hiddenExt) {
            edits.push_back(hWnd);
        }
    }

    for (HWND hWnd : edits) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, RenameEditSubclassProc);
    }
}
