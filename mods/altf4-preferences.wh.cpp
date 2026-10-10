// ==WindhawkMod==
// @id              altf4-preferences
// @name            Alt+F4 Dialog Preferences
// @description     Choose which action is preselected in the Alt+F4 "Shut Down Windows" dialog
// @version         1.0.0
// @author          Marco Kraus
// @github          https://github.com/MaKra
// @homepage        https://www.kraus.tk
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt+F4 Dialog Preferences

Sets the action which is preselected in the classic **"Shut Down Windows"**
dialog that appears when pressing `Alt+F4` on the desktop.

Instead of always starting with "Shut down", the dialog can preselect any of
its actions: Shut down, Restart, Sign out, Switch user, Sleep, or Hibernate.

## How it works

The dialog's preselected action follows the Start menu power button action,
stored per user in the registry value
`HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced\Start_PowerButtonAction`.
The mod makes the configured action visible to the dialog transiently while
the dialog is open and restores the previous state when it closes. Nothing is
written persistently, and disabling the mod takes effect immediately.

## Notes

- The Start menu power button is not affected by the transient change: the
  preselection change exists only while the Alt+F4 dialog is open.
- Sleep and Hibernate only appear in the dialog if they are enabled under
  Control Panel -> Power Options -> System Settings ("Shutdown settings"
  checkboxes). If a state is not available, the dialog falls back to
  "Shut down".
- The mod runs inside Explorer and does not start additional processes.

## Compatibility

Works on Windows 10 and Windows 11. Tested on Windows 11 26H2 (build 26300).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DefaultAction: shutdown
  $name: Default action
  $description: The action preselected in the Alt+F4 Shut Down Windows dialog
  $options:
    - shutdown: Shut down
    - restart: Restart
    - signout: Sign out
    - switchuser: Switch user
    - sleep: Sleep
    - hibernate: Hibernate
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

// Enum values are the registry values of Start_PowerButtonAction (hex).
enum class DefaultAction : DWORD {
    SignOut    = 0x1,
    ShutDown   = 0x2,
    Restart    = 0x4,
    Sleep      = 0x10,
    Hibernate  = 0x40,
    SwitchUser = 0x100,
};

constexpr PCWSTR kRegistrySubKey =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
constexpr PCWSTR kRegistryValue = L"Start_PowerButtonAction";

struct {
    DefaultAction action;
} g_settings;

DefaultAction ParseDefaultAction(PCWSTR action) {
    if (wcscmp(action, L"restart") == 0)
        return DefaultAction::Restart;
    if (wcscmp(action, L"signout") == 0)
        return DefaultAction::SignOut;
    if (wcscmp(action, L"switchuser") == 0)
        return DefaultAction::SwitchUser;
    if (wcscmp(action, L"sleep") == 0)
        return DefaultAction::Sleep;
    if (wcscmp(action, L"hibernate") == 0)
        return DefaultAction::Hibernate;
    return DefaultAction::ShutDown;
}

void LoadSettings() {
    WindhawkUtils::StringSetting action =
        WindhawkUtils::StringSetting::make(L"DefaultAction");
    g_settings.action = ParseDefaultAction(action);
    Wh_Log(L"Settings: DefaultAction=%s (0x%08lX)", (PCWSTR)action,
           (unsigned long)g_settings.action);
}

// Bisection result: the shell32!ExitWindowsDialog (ordinal 60) hook is stable
// (verified in Explorer, including Alt+F4). Built on top of it: the transient
// write - while the dialog is open, the configured action is written so the
// dialog (which reads the value live) preselects it; on dialog close the
// recorded original state is restored. Net registry change: zero. No shared
// bookkeeping: the record/restore happens synchronously on the dialog thread
// of the logged-on user.
using ExitWindowsDialog_t = void(WINAPI *)(HWND);
ExitWindowsDialog_t ExitWindowsDialog_Original;

void WINAPI ExitWindowsDialog_Hook(HWND hwndOwner) {
    Wh_Log(L"> Alt+F4 dialog opening");

    // Record the current state. Case A: value absent. Case B: value present
    // (REG_DWORD). A non-DWORD value is left untouched entirely (the
    // transient write is skipped).
    DWORD originalValue = 0;
    bool originalExisted = false;
    bool transientWriteSafe = true;
    DWORD type = 0;
    DWORD size = sizeof(DWORD);
    LONG res = RegGetValue(HKEY_CURRENT_USER, kRegistrySubKey, kRegistryValue,
                           RRF_RT_REG_DWORD, &type, (PVOID)&originalValue,
                           &size);
    if (res == ERROR_SUCCESS) {
        if (type == REG_DWORD) {
            originalExisted = true;
        } else {
            transientWriteSafe = false;
            Wh_Log(L"Unexpected value type (%lu), skipping transient write", type);
        }
    } else if (res != ERROR_FILE_NOT_FOUND) {
        transientWriteSafe = false;
        Wh_Log(L"RegGetValue failed (%ld), skipping transient write", res);
    }

    if (transientWriteSafe) {
        HKEY hKey;
        LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, kRegistrySubKey, 0,
                                    KEY_SET_VALUE, &hKey);
        if (openRes == ERROR_SUCCESS) {
            DWORD action = (DWORD)g_settings.action;
            RegSetValueEx(hKey, kRegistryValue, 0, REG_DWORD,
                          (const BYTE *)&action, sizeof(action));
            RegCloseKey(hKey);
            Wh_Log(L"Transient write: configured action 0x%08lX (original: %s"
                   L" 0x%08lX)",
                   action, originalExisted ? L"present" : L"absent",
                   originalValue);
        } else {
            transientWriteSafe = false;
            Wh_Log(L"RegOpenKeyEx failed (%ld), skipping transient write",
                   openRes);
        }
    }

    ExitWindowsDialog_Original(hwndOwner);

    if (transientWriteSafe) {
        HKEY hKey;
        LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, kRegistrySubKey, 0,
                                    KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
        if (openRes == ERROR_SUCCESS) {
            if (originalExisted) {
                RegSetValueEx(hKey, kRegistryValue, 0, REG_DWORD,
                              (const BYTE *)&originalValue, sizeof(originalValue));
                Wh_Log(L"Restored original value 0x%08lX", originalValue);
            } else {
                RegDeleteValue(hKey, kRegistryValue);
                Wh_Log(L"Restored original state: value removed");
            }
            RegCloseKey(hKey);
        }
    }

    Wh_Log(L"< Alt+F4 dialog closed");
}

void InstallHooks() {
    HMODULE shell32Module = GetModuleHandle(L"shell32.dll");
    if (shell32Module) {
        auto pExitWindowsDialog = (ExitWindowsDialog_t)GetProcAddress(
            shell32Module, (LPCSTR)(uintptr_t)60);
        if (pExitWindowsDialog) {
            WindhawkUtils::SetFunctionHook(pExitWindowsDialog,
                                           ExitWindowsDialog_Hook,
                                           &ExitWindowsDialog_Original);
            Wh_Log(L"ExitWindowsDialog hook installed (shell32 ordinal 60)");
        } else {
            Wh_Log(L"ExitWindowsDialog (shell32 ordinal 60) not found");
        }
    } else {
        Wh_Log(L"shell32.dll not loaded");
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();
    InstallHooks();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}