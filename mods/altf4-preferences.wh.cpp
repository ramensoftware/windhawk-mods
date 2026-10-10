// ==WindhawkMod==
// @id              altf4-preferences
// @name            Alt+F4 Dialog Preferences
// @description     Choose which action is preselected in the Alt+F4 "Shut Down Windows" dialog
// @version         1.0.0
// @author          Marco Kraus
// @github          https://github.com/MaKra
// @homepage        https://www.kraus.tk
// @include         explorer.exe
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
- If Explorer terminates (crash, forced shutdown) while the dialog is open,
  the preselection value remains set; it can be reset by changing the power
  button action in Windows Settings.
- Related mods: [Custom Shutdown Dialog](https://windhawk.net/mods/custom-shutdown-dialog)
  replaces the dialog completely (both mods hook the same dialog entry
  point), so this mod has nothing to preselect when both are enabled.
  [Classic Taskbar and Start Menu Properties](https://windhawk.net/mods/classic-taskbar-properties)
  writes the same registry value: changes made there are shown by the Alt+F4
  dialog as well, but when the dialog opens, this mod's configured action is
  applied transiently and the previous value is restored when it closes.

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

// Tracks an in-flight dialog so Wh_ModBeforeUninit can close it and wait for
// the hook thread to leave the module before the engine unloads it (AI review
// finding 2).
volatile LONG g_inExitWindowsDialog = 0;
DWORD g_dialogThreadId = 0;
HANDLE g_dialogClosedEvent = nullptr;

void WINAPI ExitWindowsDialog_Hook(HWND hwndOwner) {
    Wh_Log(L"> Alt+F4 dialog opening");
    InterlockedExchange(&g_inExitWindowsDialog, 1);
    g_dialogThreadId = GetCurrentThreadId();
    if (g_dialogClosedEvent) {
        ResetEvent(g_dialogClosedEvent);
    }

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
        // RRF_RT_REG_DWORD: success implies a REG_DWORD value.
        originalExisted = true;
    } else if (res == ERROR_FILE_NOT_FOUND) {
        // Case A: value absent.
    } else {
        // Includes ERROR_UNSUPPORTED_TYPE for non-DWORD values.
        transientWriteSafe = false;
        Wh_Log(L"RegGetValue: res=%ld type=%lu, skipping transient write", res,
               type);
    }

    if (transientWriteSafe) {
        HKEY hKey;
        LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, kRegistrySubKey, 0,
                                    KEY_SET_VALUE, &hKey);
        if (openRes == ERROR_SUCCESS) {
            DWORD action = (DWORD)g_settings.action;
            LONG writeRes = RegSetValueEx(hKey, kRegistryValue, 0, REG_DWORD,
                                          (const BYTE *)&action, sizeof(action));
            RegCloseKey(hKey);
            if (writeRes == ERROR_SUCCESS) {
                Wh_Log(L"Transient write: configured action 0x%08lX (original: %s"
                       L" 0x%08lX)",
                       action, originalExisted ? L"present" : L"absent",
                       originalValue);
            } else {
                transientWriteSafe = false;
                Wh_Log(L"Transient write FAILED (%ld), skipping restore",
                       writeRes);
            }
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
                                    KEY_SET_VALUE, &hKey);
        if (openRes == ERROR_SUCCESS) {
            if (originalExisted) {
                LONG writeRes =
                    RegSetValueEx(hKey, kRegistryValue, 0, REG_DWORD,
                                  (const BYTE *)&originalValue,
                                  sizeof(originalValue));
                RegCloseKey(hKey);
                if (writeRes == ERROR_SUCCESS) {
                    Wh_Log(L"Restored original value 0x%08lX", originalValue);
                } else {
                    Wh_Log(L"Restore FAILED (%ld)", writeRes);
                }
            } else {
                LONG deleteRes = RegDeleteValue(hKey, kRegistryValue);
                RegCloseKey(hKey);
                if (deleteRes == ERROR_SUCCESS ||
                    deleteRes == ERROR_FILE_NOT_FOUND) {
                    Wh_Log(L"Restored original state: value removed");
                } else {
                    Wh_Log(L"Restore FAILED (%ld)", deleteRes);
                }
            }
        }
    }

    InterlockedExchange(&g_inExitWindowsDialog, 0);
    if (g_dialogClosedEvent) {
        SetEvent(g_dialogClosedEvent);
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
    g_dialogClosedEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    LoadSettings();
    InstallHooks();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
}

// Finds the Alt+F4 dialog window (created by the original call on the hook
// thread) and closes it, so the hook thread can finish the restore and leave
// the module before the engine unloads it (AI review finding 2).
static BOOL CALLBACK CloseDialogEnumProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (tid == g_dialogThreadId && pid == GetCurrentProcessId()) {
        wchar_t className[16];
        if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) > 0 &&
            wcscmp(className, L"#32770") == 0) {
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
        }
    }
    return TRUE;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    // AI review finding 2: if the Alt+F4 dialog is open, the hook thread is
    // blocked inside the original call and the restore is still pending.
    // Closing the dialog lets the hook thread finish (restore included), so
    // the engine can unload the module safely.
    if (InterlockedCompareExchange(&g_inExitWindowsDialog, 0, 0) != 0) {
        Wh_Log(L"Dialog is open - closing it so the hook can leave the module");

        EnumWindows(CloseDialogEnumProc, 0);

        if (g_dialogClosedEvent) {
            WaitForSingleObject(g_dialogClosedEvent, 10000);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}