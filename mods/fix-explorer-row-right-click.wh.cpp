// ==WindhawkMod==
// @id              fix-explorer-row-right-click
// @name            Fix Explorer Row Right-Click
// @description     Fixes right-clicking on empty row space opening the folder background menu instead of the file menu.
// @version         1.0.0
// @author          iMAboud
// @include         explorer.exe
// @github          https://github.com/iMAboud
// @compilerOptions -loleacc -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix Explorer Row Right-Click

Right-click anywhere on a file's row or item area to open its context menu, rather than having to click directly on the file name.

### Comparison

| Before | After |
| :---: | :---: |
| ![Before](https://i.imgur.com/ly9exsa.png) | ![After](https://i.imgur.com/PPW2Xtk.png) |

### Features
- **Full row support:** Right-clicking empty space across a row opens the selected file's context menu.
- **Selection safe:** Preserves existing multi-item selections.
- **Folder background intact:** Right-clicking below all files still opens the folder background menu.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <oleacc.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t DispatchMessageW_Original = nullptr;

// Verify that the window receiving the click is the File Explorer folder view
bool IsExplorerFileView(HWND hWnd) {
    if (!hWnd) return false;

    // Ignore header controls (e.g. column headers) and scrollbars
    WCHAR cls[64];
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        if (wcscmp(cls, L"SysHeader32") == 0 || wcscmp(cls, L"ScrollBar") == 0) {
            return false;
        }
    }

    // Must be hosted inside SHELLDLL_DefView (Explorer file listing)
    bool insideDefView = false;
    HWND hCurrent = hWnd;
    while (hCurrent) {
        WCHAR curClass[64];
        if (GetClassNameW(hCurrent, curClass, ARRAYSIZE(curClass))) {
            if (wcscmp(curClass, L"SHELLDLL_DefView") == 0) {
                insideDefView = true;
                break;
            }
        }
        hCurrent = GetParent(hCurrent);
    }

    if (!insideDefView) {
        return false;
    }

    // Exclude the Desktop (Progman or WorkerW)
    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    WCHAR rootClass[64];
    if (GetClassNameW(hRoot, rootClass, ARRAYSIZE(rootClass))) {
        if (wcscmp(rootClass, L"Progman") == 0 || wcscmp(rootClass, L"WorkerW") == 0) {
            return false;
        }
    }

    return true;
}

// Walks up the accessibility hierarchy to find a ROLE_SYSTEM_LISTITEM
bool FindRowAtPoint(POINT ptScreen, IAccessible** ppRowAcc, VARIANT* pRowChild, bool* pIsSelected) {
    *ppRowAcc = nullptr;
    VariantInit(pRowChild);
    *pIsSelected = false;

    IAccessible* pAcc = nullptr;
    VARIANT varChild;
    VariantInit(&varChild);

    if (FAILED(AccessibleObjectFromPoint(ptScreen, &pAcc, &varChild)) || !pAcc) {
        return false;
    }

    IAccessible* cur = pAcc;
    VARIANT curChild = varChild;

    for (int depth = 0; cur && depth < 5; depth++) {
        VARIANT role;
        VariantInit(&role);
        bool isRow = SUCCEEDED(cur->get_accRole(curChild, &role)) &&
                     role.vt == VT_I4 && role.lVal == ROLE_SYSTEM_LISTITEM;
        VariantClear(&role);

        if (isRow) {
            VARIANT state;
            VariantInit(&state);
            if (SUCCEEDED(cur->get_accState(curChild, &state)) && state.vt == VT_I4) {
                *pIsSelected = (state.lVal & STATE_SYSTEM_SELECTED) != 0;
            }
            VariantClear(&state);

            *ppRowAcc = cur; // Caller takes ownership
            *pRowChild = curChild;
            return true;
        }

        // When varChild is a simple child ID, its parent is `cur` itself
        if (curChild.lVal != CHILDID_SELF) {
            curChild.lVal = CHILDID_SELF;
            continue;
        }

        IDispatch* parentDisp = nullptr;
        IAccessible* parent = nullptr;
        if (SUCCEEDED(cur->get_accParent(&parentDisp)) && parentDisp) {
            parentDisp->QueryInterface(IID_PPV_ARGS(&parent));
            parentDisp->Release();
        }

        cur->Release();
        cur = parent;
    }

    if (cur) {
        cur->Release();
    }
    return false;
}

LRESULT WINAPI DispatchMessageWHook(const MSG *lpMsg) {
    if (lpMsg && lpMsg->message == WM_RBUTTONDOWN) {
        // Do not interfere if Ctrl or Shift is held (e.g. selection modifier shortcuts)
        if ((GetKeyState(VK_CONTROL) & 0x8000) == 0 &&
            (GetKeyState(VK_SHIFT) & 0x8000) == 0) {

            if (IsExplorerFileView(lpMsg->hwnd)) {
                IAccessible* pRowAcc = nullptr;
                VARIANT rowChild;
                bool isSelected = false;

                if (FindRowAtPoint(lpMsg->pt, &pRowAcc, &rowChild, &isSelected)) {
                    if (!isSelected) {
                        // Attempt to focus and select the item via MSAA directly
                        HRESULT hr = pRowAcc->accSelect(SELFLAG_TAKEFOCUS | SELFLAG_TAKESELECTION, rowChild);
                        if (FAILED(hr)) {
                            // Fallback to synthesizing a left click if accSelect is not implemented
                            SendMessageW(lpMsg->hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lpMsg->lParam);
                            SendMessageW(lpMsg->hwnd, WM_LBUTTONUP, 0, lpMsg->lParam);
                        }
                    }
                    pRowAcc->Release();
                }
            }
        }
    }

    return DispatchMessageW_Original(lpMsg);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Explorer Full Row Right-Click loaded");

    if (!WindhawkUtils::SetFunctionHook(DispatchMessageW,
                                        DispatchMessageWHook,
                                        &DispatchMessageW_Original)) {
        Wh_Log(L"Failed to hook DispatchMessageW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Explorer Full Row Right-Click unloaded");
}