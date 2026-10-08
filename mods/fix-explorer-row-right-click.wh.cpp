// ==WindhawkMod==
// @id              fix-explorer-row-right-click
// @name            Fix Explorer Row Right-Click
// @description     Fixes right-clicking on empty row space opening the folder background menu instead of the file menu.
// @version         1.0.0
// @author          iMAboud
// @include         explorer.exe
// @github          https://github.com/iMAboud
// @compilerOptions -loleacc -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Full Row Right-Click in Explorer

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
#include <windowsx.h>
#include <oleacc.h>
#include <windhawk_api.h>

static const IID My_IID_IAccessible = {
    0x618736E0, 0x3C3D, 0x11CF, { 0x81, 0x0C, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 }
};

using DispatchMessageW_t = LRESULT (WINAPI *)(const MSG *lpMsg);
DispatchMessageW_t pOriginalDispatchMessageW = nullptr;

bool IsExplorerFileView(HWND hWnd) {
    if (!hWnd) return false;

    WCHAR cls[64];
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        if (wcscmp(cls, L"SysHeader32") == 0 || wcscmp(cls, L"ScrollBar") == 0) {
            return false;
        }
    }

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

    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    WCHAR rootClass[64];
    if (GetClassNameW(hRoot, rootClass, ARRAYSIZE(rootClass))) {
        if (wcscmp(rootClass, L"Progman") == 0 || wcscmp(rootClass, L"WorkerW") == 0) {
            return false;
        }
    }

    return true;
}

bool CheckItemAtPoint(POINT ptScreen, bool* pIsItem, bool* pIsSelected) {
    *pIsItem = false;
    *pIsSelected = false;

    IAccessible* pAcc = nullptr;
    VARIANT varChild;
    VariantInit(&varChild);

    HRESULT hr = AccessibleObjectFromPoint(ptScreen, &pAcc, &varChild);
    if (FAILED(hr) || !pAcc) {
        return false;
    }

    VARIANT varRole;
    VariantInit(&varRole);
    VARIANT varState;
    VariantInit(&varState);

    auto inspect = [&](IAccessible* acc, const VARIANT& child) {
        if (SUCCEEDED(acc->get_accRole(child, &varRole))) {
            long role = (varRole.vt == VT_I4) ? varRole.lVal : 0;
            if (role == ROLE_SYSTEM_LISTITEM ||
                role == ROLE_SYSTEM_CELL ||
                role == ROLE_SYSTEM_TEXT ||
                role == ROLE_SYSTEM_GRAPHIC ||
                role == ROLE_SYSTEM_CHECKBUTTON) {
                *pIsItem = true;
            }
        }
        if (SUCCEEDED(acc->get_accState(child, &varState))) {
            long state = (varState.vt == VT_I4) ? varState.lVal : 0;
            if (state & STATE_SYSTEM_SELECTED) {
                *pIsSelected = true;
            }
        }
    };

    inspect(pAcc, varChild);

    if (*pIsItem && !(*pIsSelected)) {
        IDispatch* pParentDisp = nullptr;
        if (SUCCEEDED(pAcc->get_accParent(&pParentDisp)) && pParentDisp) {
            IAccessible* pParentAcc = nullptr;
            if (SUCCEEDED(pParentDisp->QueryInterface(My_IID_IAccessible, (void**)&pParentAcc))) {
                VARIANT selfChild;
                selfChild.vt = VT_I4;
                selfChild.lVal = CHILDID_SELF;
                inspect(pParentAcc, selfChild);
                pParentAcc->Release();
            }
            pParentDisp->Release();
        }
    }

    VariantClear(&varRole);
    VariantClear(&varState);
    VariantClear(&varChild);
    pAcc->Release();

    return true;
}

LRESULT WINAPI DispatchMessageWHook(const MSG *lpMsg) {
    if (lpMsg && lpMsg->message == WM_RBUTTONDOWN) {
        if ((GetKeyState(VK_CONTROL) & 0x8000) == 0 &&
            (GetKeyState(VK_SHIFT) & 0x8000) == 0) {

            if (IsExplorerFileView(lpMsg->hwnd)) {
                POINT ptClient = { (SHORT)LOWORD(lpMsg->lParam), (SHORT)HIWORD(lpMsg->lParam) };
                POINT ptScreen = ptClient;
                ClientToScreen(lpMsg->hwnd, &ptScreen);

                bool isItem = false;
                bool isSelected = false;

                if (CheckItemAtPoint(ptScreen, &isItem, &isSelected)) {
                    if (isItem && !isSelected) {
                        SendMessageW(lpMsg->hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lpMsg->lParam);
                        SendMessageW(lpMsg->hwnd, WM_LBUTTONUP, 0, lpMsg->lParam);
                    }
                }
            }
        }
    }

    return pOriginalDispatchMessageW(lpMsg);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Explorer Row Right-Click Fix loaded");

    if (!Wh_SetFunctionHook((void*)DispatchMessageW,
                            (void*)DispatchMessageWHook,
                            (void**)&pOriginalDispatchMessageW)) {
        Wh_Log(L"Failed to hook DispatchMessageW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Explorer Row Right-Click Fix unloaded");
}