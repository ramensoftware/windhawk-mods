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
the dialog is open and restores the previous state when it closes. The
registry change only exists while the dialog is open; if Explorer terminates
during that window, the configured value can remain in the registry - the
mod detects and reverts such a leftover on its next load.

## Notes

- The Start menu power button is not affected by the transient change: the
  preselection change exists only while the Alt+F4 dialog is open.
- Sleep and Hibernate only appear in the dialog if they are enabled under
  Control Panel -> Power Options -> System Settings ("Shutdown settings"
  checkboxes). If a state is not available, the dialog falls back to
  "Shut down".
- The mod runs inside Explorer and does not start additional processes.
- If Explorer terminates (crash, forced shutdown) while the dialog is open,
  the preselection value remains set. It can be removed by deleting (or
  changing) the `Start_PowerButtonAction` value under
  `HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced` in
  regedit - the mod also reverts such a leftover automatically on its next
  load.
- Related mods: [Custom Shutdown Dialog](https://windhawk.net/mods/custom-shutdown-dialog)
  replaces the dialog completely (both mods hook the same dialog entry
  point), so this mod has nothing to preselect when both are enabled.
  [Classic Taskbar and Start Menu Properties](https://windhawk.net/mods/classic-taskbar-properties)
  writes the same registry value: while this mod is enabled, the Alt+F4
  dialog always opens with this mod's configured action (changes made there
  apply outside the dialog); when the mod is disabled, changes made there
  are shown again.

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

#include <sddl.h>
#include <string>
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

// The shell32!ExitWindowsDialog (ordinal 60) hook is stable (verified in
// Explorer, including Alt+F4; hooking kernelbase!RegQueryValueExW instead
// hung Explorer - see the pull request description). Built on top of it: the
// transient write - while the dialog is open, the configured action is
// written so the dialog (which reads the value live) preselects it; on
// dialog close the recorded original state is restored. Net registry change:
// zero. No shared bookkeeping: the record/restore happens synchronously on
// the dialog thread of the logged-on user.
using ExitWindowsDialog_t = void(WINAPI *)(HWND);
ExitWindowsDialog_t ExitWindowsDialog_Original;

// In-flight dialog tracking (AI review round 3, optional hardening): a
// counter instead of a 0/1 flag handles overlapping dialog calls, and the
// "unloading" flag makes new calls return without opening the dialog while
// the mod is being unloaded (AI review finding 2).
volatile LONG g_inFlight = 0;
volatile LONG g_unloading = 0;
DWORD g_dialogThreadId = 0;
HANDLE g_dialogDoneEvent = nullptr;

// Windhawk mod storage is machine-wide while the registry value is per user,
// so the storage value names carry the user's SID (AI review round 3, gap 1).
std::wstring g_userSuffix;

std::wstring UserStorageName(PCWSTR base) {
    return std::wstring(base) + g_userSuffix;
}

std::wstring GetUserStorageSuffix() {
    std::wstring suffix;
    HANDLE token = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        BYTE tokenUserBuffer[128] = {};
        DWORD size = sizeof(tokenUserBuffer);
        if (GetTokenInformation(token, TokenUser, tokenUserBuffer, size, &size)) {
            auto *tokenUser = (TOKEN_USER *)tokenUserBuffer;
            PWSTR sidString = nullptr;
            if (ConvertSidToStringSidW(tokenUser->User.Sid, &sidString)) {
                suffix = sidString;
                LocalFree(sidString);
            }
        }
        CloseHandle(token);
    }
    return suffix;
}

void WINAPI ExitWindowsDialog_Hook(HWND hwndOwner) {
    LONG inFlight = InterlockedIncrement(&g_inFlight);
    g_dialogThreadId = GetCurrentThreadId();
    if (g_dialogDoneEvent) {
        ResetEvent(g_dialogDoneEvent);
    }

    if (InterlockedCompareExchange(&g_unloading, 0, 0) != 0) {
        // The mod is being unloaded: don't open the dialog and don't touch
        // the registry (AI review round 3, optional hardening).
        LONG left = InterlockedDecrement(&g_inFlight);
        if (left == 0 && g_dialogDoneEvent) {
            SetEvent(g_dialogDoneEvent);
        }
        return;
    }

    Wh_Log(L"> Alt+F4 dialog opening");

    // Only the outermost call records, writes and restores (AI review round
    // 3: overlapping dialog calls).
    bool outermost = inFlight == 1;

    DWORD originalValue = 0;
    bool originalExisted = false;
    bool transientWriteSafe = outermost;
    bool restored = false;

    if (outermost) {
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
    }

    if (transientWriteSafe) {
        // AI review round 3, gap 1: save the pending restore before the
        // write, so a leftover from an unclean exit is reverted on the next
        // mod load.
        std::wstring pendingExistsName = UserStorageName(L"pendingExists_");
        std::wstring pendingValueName = UserStorageName(L"pendingValue_");
        Wh_SetIntValue(pendingExistsName.c_str(), originalExisted ? 1 : 0);
        Wh_SetIntValue(pendingValueName.c_str(), (int)originalValue);

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
                    restored = true;
                } else {
                    Wh_Log(L"Restore FAILED (%ld)", writeRes);
                }
            } else {
                LONG deleteRes = RegDeleteValue(hKey, kRegistryValue);
                RegCloseKey(hKey);
                if (deleteRes == ERROR_SUCCESS ||
                    deleteRes == ERROR_FILE_NOT_FOUND) {
                    Wh_Log(L"Restored original state: value removed");
                    restored = true;
                } else {
                    Wh_Log(L"Restore FAILED (%ld)", deleteRes);
                }
            }
        }
    }

    if (outermost && restored) {
        // AI review round 3, gap 1: the registry is verified back to the
        // recorded state - clear the pending record. If the restore failed,
        // the record stays and the next mod load reverts the leftover.
        Wh_DeleteValue(UserStorageName(L"pendingExists_").c_str());
        Wh_DeleteValue(UserStorageName(L"pendingValue_").c_str());
    }

    LONG left = InterlockedDecrement(&g_inFlight);
    if (left == 0 && g_dialogDoneEvent) {
        SetEvent(g_dialogDoneEvent);
    }

    Wh_Log(L"< Alt+F4 dialog closed");
}

void InstallHooks() {
    HMODULE shell32Module =
        LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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

// AI review round 3, gap 1: apply a pending restore that was saved before a
// transient write but never completed (Explorer crashed, was killed or lost
// power while the dialog was open).
void ApplyPendingRestore() {
    std::wstring pendingExistsName = UserStorageName(L"pendingExists_");
    std::wstring pendingValueName = UserStorageName(L"pendingValue_");
    int pendingExists = Wh_GetIntValue(pendingExistsName.c_str(), -1);
    if (pendingExists == -1) {
        return;
    }

    Wh_Log(L"Pending restore from an unclean exit (exists=%d)", pendingExists);
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, kRegistrySubKey, 0, KEY_SET_VALUE,
                     &hKey) == ERROR_SUCCESS) {
        if (pendingExists == 1) {
            DWORD value = (DWORD)Wh_GetIntValue(pendingValueName.c_str(), 0);
            RegSetValueEx(hKey, kRegistryValue, 0, REG_DWORD,
                          (const BYTE *)&value, sizeof(value));
            Wh_Log(L"Reverted leftover value to 0x%08lX", value);
        } else {
            RegDeleteValue(hKey, kRegistryValue);
            Wh_Log(L"Removed leftover value");
        }
        RegCloseKey(hKey);
    }

    Wh_DeleteValue(pendingExistsName.c_str());
    Wh_DeleteValue(pendingValueName.c_str());
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    g_userSuffix = GetUserStorageSuffix();
    if (g_userSuffix.empty()) {
        Wh_Log(L"Failed to get the user SID, leftover cleanup disabled");
    } else {
        ApplyPendingRestore();
    }
    g_dialogDoneEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
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
    // Setting the "unloading" flag makes new dialog calls return immediately,
    // and closing the dialog lets the in-flight hook finish (restore
    // included) before the engine unloads the module.
    InterlockedExchange(&g_unloading, 1);
    if (InterlockedCompareExchange(&g_inFlight, 0, 0) > 0) {
        Wh_Log(L"Dialog is open - closing it so the hook can leave the module");

        EnumWindows(CloseDialogEnumProc, 0);

        if (g_dialogDoneEvent) {
            WaitForSingleObject(g_dialogDoneEvent, 10000);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    if (g_dialogDoneEvent) {
        CloseHandle(g_dialogDoneEvent);
        g_dialogDoneEvent = nullptr;
    }
}