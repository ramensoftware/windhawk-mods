// ==WindhawkMod==
// @id              explorer-name-windows
// @name            Name explorer windows
// @description     Assign custom names to explorer windows, just like in Chrome
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Name explorer windows

Assign custom names to explorer windows, just like in Chrome.

![Demonstration](https://i.imgur.com/sSI0Kh6.gif)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <optional>
#include <string>
#include <unordered_map>

struct CabinedWindowData {
    std::wstring originalText;
    std::wstring customText;
};

std::unordered_map<HWND, CabinedWindowData> g_cabinetWindows;

constexpr UINT_PTR IDM_MYSYSTEM = 1001;

constexpr WCHAR g_szClassName[] = L"Windhawk_explorer-name-windows";

constexpr int DIALOG_MARGIN = 7;
constexpr int DIALOG_WIDTH = 300;
constexpr int DIALOG_EDIT_HEIGHT = 20;
constexpr int DIALOG_BUTTON_HEIGHT = 20;
constexpr int DIALOG_BUTTON_WIDTH = 50;
constexpr int DIALOG_HEIGHT =
    (DIALOG_MARGIN + DIALOG_EDIT_HEIGHT + DIALOG_MARGIN + DIALOG_BUTTON_HEIGHT +
     DIALOG_MARGIN);

constexpr int IDC_MAIN_EDIT = 101;

void ClientResize(HWND hWnd, int nWidth, int nHeight) {
    RECT rcClient, rcWind;
    POINT ptDiff;
    GetClientRect(hWnd, &rcClient);
    GetWindowRect(hWnd, &rcWind);
    ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
    ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
    MoveWindow(hWnd, rcWind.left, rcWind.top, nWidth + ptDiff.x,
               nHeight + ptDiff.y, TRUE);
}

void CenterWindow(HWND hWnd, HWND hParentWnd) {
    RECT rcParent;
    GetWindowRect(hParentWnd, &rcParent);

    RECT rc;
    GetWindowRect(hWnd, &rc);

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    int x = (rcParent.left + rcParent.right) / 2 - width / 2;
    int y = (rcParent.top + rcParent.bottom) / 2 - height / 2;

    SetWindowPos(hWnd, nullptr, x, y, width, height, SWP_NOZORDER);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
            SetWindowLongPtr(hWnd, GWLP_USERDATA,
                             (LONG_PTR)createStruct->lpCreateParams);

            HWND hParentWnd = GetParent(hWnd);
            EnableWindow(hParentWnd, FALSE);

            HFONT hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            SendMessage(hWnd, WM_SETFONT, (WPARAM)hfDefault,
                        MAKELPARAM(FALSE, 0));

            ClientResize(hWnd, DIALOG_WIDTH, DIALOG_HEIGHT);

            HWND hEdit = CreateWindowEx(
                WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE,
                DIALOG_MARGIN, DIALOG_MARGIN, DIALOG_WIDTH - DIALOG_MARGIN * 2,
                DIALOG_EDIT_HEIGHT, hWnd, (HMENU)IDC_MAIN_EDIT,
                GetModuleHandle(nullptr), nullptr);

            SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault,
                        MAKELPARAM(FALSE, 0));

            HWND hButtonOK = CreateWindowEx(
                0, L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE,
                DIALOG_WIDTH - (DIALOG_MARGIN + DIALOG_BUTTON_WIDTH) * 2,
                DIALOG_MARGIN + DIALOG_EDIT_HEIGHT + DIALOG_MARGIN,
                DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT, hWnd, (HMENU)IDOK,
                GetModuleHandle(nullptr), nullptr);

            SendMessage(hButtonOK, WM_SETFONT, (WPARAM)hfDefault,
                        MAKELPARAM(FALSE, 0));

            HWND hButtonCancel = CreateWindowEx(
                0, L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE,
                DIALOG_WIDTH - (DIALOG_MARGIN + DIALOG_BUTTON_WIDTH),
                DIALOG_MARGIN + DIALOG_EDIT_HEIGHT + DIALOG_MARGIN,
                DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT, hWnd,
                (HMENU)IDCANCEL, GetModuleHandle(nullptr), nullptr);

            SendMessage(hButtonCancel, WM_SETFONT, (WPARAM)hfDefault,
                        MAKELPARAM(FALSE, 0));

            CenterWindow(hWnd, hParentWnd);

            SetFocus(hEdit);
        } break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK: {
                    WCHAR text[1024];
                    GetDlgItemText(hWnd, IDC_MAIN_EDIT, text, ARRAYSIZE(text));

                    auto* str = (std::optional<std::wstring>*)GetWindowLongPtr(
                        hWnd, GWLP_USERDATA);
                    *str = text;

                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                } break;

                case IDCANCEL:
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
            }
            break;

        case WM_CLOSE: {
            HWND hParentWnd = GetParent(hWnd);
            EnableWindow(hParentWnd, TRUE);

            DestroyWindow(hWnd);
        } break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
            break;
    }

    return 0;
}

std::optional<std::wstring> PromptForNewTitle(HWND hParentWnd) {
    WNDCLASSEX wc{
        .cbSize = sizeof(WNDCLASSEX),
        .style = 0,
        .lpfnWndProc = WndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = GetModuleHandle(nullptr),
        .hIcon = LoadIcon(nullptr, IDI_APPLICATION),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1),
        .lpszMenuName = nullptr,
        .lpszClassName = g_szClassName,
        .hIconSm = LoadIcon(nullptr, IDI_APPLICATION),
    };
    if (!RegisterClassEx(&wc)) {
        return std::nullopt;
    }

    std::optional<std::wstring> result;
    HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, g_szClassName,
                               L"Name this window", WS_POPUPWINDOW | WS_CAPTION,
                               CW_USEDEFAULT, CW_USEDEFAULT, 100, 100,
                               hParentWnd, nullptr, wc.hInstance, &result);
    if (!hwnd) {
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return std::nullopt;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return result;
}

LRESULT CALLBACK CabinetWindowSubclassProc(HWND hWnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam,
                                           DWORD_PTR dwRefData) {
    LRESULT result = 0;

    switch (uMsg) {
        case WM_SYSCOMMAND:
            if (wParam == IDM_MYSYSTEM) {
                auto newTitle = PromptForNewTitle(hWnd);
                if (newTitle) {
                    auto it = g_cabinetWindows.find(hWnd);
                    if (it != g_cabinetWindows.end()) {
                        it->second.customText.clear();

                        if (!newTitle->empty()) {
                            if (it->second.originalText.empty()) {
                                WCHAR text[1024];
                                GetWindowText(hWnd, text, ARRAYSIZE(text));

                                it->second.originalText = text;
                            }

                            SetWindowText(hWnd, newTitle->c_str());
                            it->second.customText = *newTitle;
                        } else if (!it->second.originalText.empty()) {
                            SetWindowText(hWnd,
                                          it->second.originalText.c_str());
                            it->second.originalText.clear();
                        }
                    }
                }

                result = 0;
            } else {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;

        case WM_SETTEXT: {
            auto it = g_cabinetWindows.find(hWnd);
            if (it != g_cabinetWindows.end()) {
                PCWSTR str = it->second.customText.c_str();
                if (*str != L'\0') {
                    it->second.originalText = lParam ? (PCWSTR)lParam : L"";

                    lParam = (LPARAM)str;
                }
            }

            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        } break;

        case WM_NCDESTROY:
            g_cabinetWindows.erase(hWnd);

            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            break;

        default:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            break;
    }

    return result;
}

void HandleIdentifiedCabinetWindow(HWND hWnd) {
    HMENU hMenu = GetSystemMenu(hWnd, FALSE);
    InsertMenu(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, IDM_MYSYSTEM,
               L"Name window...");
    InsertMenu(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_SEPARATOR, 0, nullptr);

    g_cabinetWindows.try_emplace(hWnd);
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
                                                  CabinetWindowSubclassProc, 0);
}

int GetMenuItemPositionFromCommandID(HMENU hMenu, UINT_PTR commandID) {
    int itemCount = GetMenuItemCount(hMenu);
    for (int i = 0; i < itemCount; i++) {
        MENUITEMINFO menuItemInfo{
            .cbSize = sizeof(MENUITEMINFO),
            .fMask = MIIM_FTYPE | MIIM_ID,
        };
        if (GetMenuItemInfo(hMenu, i, TRUE, &menuItemInfo) &&
            (menuItemInfo.wID == commandID)) {
            return i;
        }
    }

    return -1;
}

void ResetIdentifiedCabinetWindow(HWND hWnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                     CabinetWindowSubclassProc);

    HMENU hMenu = GetSystemMenu(hWnd, FALSE);
    int pos = GetMenuItemPositionFromCommandID(hMenu, IDM_MYSYSTEM);
    if (pos != -1) {
        DeleteMenu(hMenu, pos, MF_BYPOSITION);

        MENUITEMINFO menuItemInfo{
            .cbSize = sizeof(MENUITEMINFO),
            .fMask = MIIM_FTYPE,
        };
        if (GetMenuItemInfo(hMenu, pos, TRUE, &menuItemInfo) &&
            (menuItemInfo.fType & MFT_SEPARATOR)) {
            DeleteMenu(hMenu, pos, MF_BYPOSITION);
        }
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) {
        return hWnd;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && _wcsicmp(lpClassName, L"CabinetWClass") == 0) {
        Wh_Log(L"CabinetWClass window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedCabinetWindow(hWnd);
    }

    return hWnd;
}

void HandleCurrentProcessCabinetWindows() {
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"CabinetWClass") == 0) {
                Wh_Log(L"CabinetWClass window found: %08X",
                       (DWORD)(ULONG_PTR)hWnd);
                HandleIdentifiedCabinetWindow(hWnd);
            }

            return TRUE;
        },
        0);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HandleCurrentProcessCabinetWindows();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    for (const auto& [hWnd, _] : g_cabinetWindows) {
        ResetIdentifiedCabinetWindow(hWnd);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
}
