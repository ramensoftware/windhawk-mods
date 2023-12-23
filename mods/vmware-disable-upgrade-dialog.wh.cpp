// ==WindhawkMod==
// @id              vmware-disable-upgrade-dialog
// @name            Disable VMware upgrade dialog
// @description     Disable the VMware Workstation "old encryption algorithm" upgrade dialog
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         vmware.exe
// @architecture    x86
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable VMware upgrade dialog

Disable the VMware Workstation "old encryption algorithm" upgrade dialog.

When the dialog is displayed, the mod automatically selects the action of
choice.

![Screenshot](https://i.imgur.com/vu6z164.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- action: upgrade
  $name: The action to take
  $options:
  - upgrade: Upgrade
  - dontUpgrade: Don't upgrade
*/
// ==/WindhawkModSettings==

enum class Action {
    upgrade,
    dontUpgrade,
};

struct {
    Action action;
} g_settings;

HWND g_dialog;
HWND g_staticControl;
HWND g_upgradeBtn;
HWND g_dontUpgradeBtn;

constexpr WCHAR kDialogText[] =
    L"The virtual machine is encrypted using old encryption algorithm. You can "
    L"upgrade VM to latest supported encryption mode. Please note that the "
    L"upgrade may break backward compatibility which means that the virtual "
    L"machine after upgrade may not work on older versions of Workstation. "
    L"Before upgrading, refer below KB for more details.\r\n\r\n "
    L"https://kb.vmware.com/s/article/93071";
constexpr WCHAR kUpgradeBtnText[] = L"Upgrade";
constexpr WCHAR kDontUpgradeBtnText[] = L"Don't Upgrade";

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
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !hWndParent) {
        return hWnd;
    }

    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return hWnd;
    }

    HWND* phWnd = nullptr;

    if (_wcsicmp(szClassName, L"VMwareStaticLink") == 0 && lpWindowName &&
        wcscmp(lpWindowName, kDialogText) == 0) {
        Wh_Log(L"Static control created: %08X", (DWORD)(ULONG_PTR)hWnd);
        phWnd = &g_staticControl;
    } else if (_wcsicmp(szClassName, L"button") == 0 && lpWindowName) {
        if (wcscmp(lpWindowName, kUpgradeBtnText) == 0) {
            Wh_Log(L"Upgrade button created: %08X", (DWORD)(ULONG_PTR)hWnd);
            phWnd = &g_upgradeBtn;
        } else if (wcscmp(lpWindowName, kDontUpgradeBtnText) == 0) {
            Wh_Log(L"Don't upgrade button created: %08X",
                   (DWORD)(ULONG_PTR)hWnd);
            phWnd = &g_dontUpgradeBtn;
        }
    }

    if (phWnd) {
        if (g_dialog != hWndParent) {
            g_dialog = hWndParent;
            g_staticControl = nullptr;
            g_upgradeBtn = nullptr;
            g_dontUpgradeBtn = nullptr;
        }

        *phWnd = hWnd;

        if (g_staticControl && g_upgradeBtn && g_dontUpgradeBtn) {
            if (g_settings.action == Action::upgrade) {
                PostMessage(hWndParent, WM_COMMAND,
                            (BN_CLICKED << 16) | GetDlgCtrlID(g_upgradeBtn),
                            (LPARAM)g_upgradeBtn);
            } else {
                PostMessage(hWndParent, WM_COMMAND,
                            (BN_CLICKED << 16) | GetDlgCtrlID(g_dontUpgradeBtn),
                            (LPARAM)g_dontUpgradeBtn);
            }

            g_dialog = nullptr;
            g_staticControl = nullptr;
            g_upgradeBtn = nullptr;
            g_dontUpgradeBtn = nullptr;
        }
    }

    return hWnd;
}

void LoadSettings() {
    PCWSTR action = Wh_GetStringSetting(L"action");
    g_settings.action = Action::upgrade;
    if (wcscmp(action, L"dontUpgrade") == 0) {
        g_settings.action = Action::dontUpgrade;
    }
    Wh_FreeStringSetting(action);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
