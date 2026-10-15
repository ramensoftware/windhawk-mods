// ==WindhawkMod==
// @id              altf4-preferences
// @name            Alt+F4 Dialog Preferences
// @description     Choose which action is preselected in the Alt+F4 "Shut Down Windows" dialog; optionally offer/hide Sleep and Hibernate (machine-wide, UAC prompt)

// @name:de-DE        Alt+F4-Dialog-Einstellungen
// @description:de-DE Wählt die Aktion, die im Alt+F4-Dialog „Windows herunterfahren“ vorausgewählt ist; bietet optional an, „Energie sparen“/„Ruhezustand“ ein- oder auszublenden (maschinenweit, UAC-Aufforderung).
// @version         1.1.0
// @author          Marco Kraus
// @github          https://github.com/MaKra
// @homepage        https://www.kraus.tk
// @include         explorer.exe
// @compilerOptions -lole32 -lwtsapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt+F4 Dialog Preferences

Sets the action which is preselected in the classic **"Shut Down Windows"**
dialog that appears when pressing `Alt+F4` on the desktop.

Instead of always starting with "Shut down", the dialog can preselect any of
its actions: Shut down, Restart, Sign out, Switch user, Sleep, or Hibernate.
Additionally, the availability of the Sleep and Hibernate actions in the
dialog can be controlled (see "Sleep and Hibernate availability" below).

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
  checkboxes) - or with this mod's availability switches (see below).
  If a state is not available, the dialog falls back to "Shut down".
- The mod runs inside Explorer. Apart from the optional availability
  switches (which start an elevated reg.exe / powercfg.exe via one UAC
  prompt per apply), it does not start additional processes.
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

## Sleep and Hibernate availability

Sleep and Hibernate only appear in the dialog when they are enabled
machine-wide. The switches control this visibility directly:

- "Offer Sleep / Hibernate in the Alt+F4 dialog": the switch is the
  desired state. ON shows the action, OFF hides it - in both power menus
  (the Alt+F4 dialog and the Start menu). When you flip a switch, the
  mod compares it with the live system state and applies the difference
  with one UAC prompt, then reports the result. If no change is needed
  you get a short confirmation instead.
- The Hibernate switch also enables the hibernation infrastructure when
  turned ON, if it is disabled (`powercfg /hibernate on`) - this can
  allocate hiberfil.sys (gigabytes) and makes Fast Startup available.
  Turning it OFF hides the action but never disables hibernation and
  never deletes hiberfil.sys. The Fast Startup setting itself is not
  touched.
- The switches are not a live status display: they only sync the system
  when flipped. Changes made elsewhere (Power Options, policy tools)
  are not overwritten between flips - the Alt+F4 dialog itself shows
  the current state at any time.

These changes are machine-wide (they apply to all users of this
computer). A Group Policy setting takes precedence and is reported
instead of being overridden (MDM-managed policies are not detected).
Switch states persist when the mod is disabled - they are regular
system settings and can be reverted in Power Options at any time. If
the default action (see the settings above) is an action that is
currently not offered, the dialog falls back to "Shut down" - the
apply popup points this out.

A pending availability change (for example made while an apply was
already running, or while the mod was disabled) is applied at the
next settings save. The popup language follows the Windows UI
language, while the settings UI follows the Windhawk UI language -
the two can differ on multilingual systems.

## Compatibility

Works on Windows 10 and Windows 11. Tested on Windows 11 26H2 (build 26300).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DefaultAction: shutdown
  $name: Default action
  $description: The action preselected in the Alt+F4 Shut Down Windows dialog. If the selected action is not offered (see the availability switches below), the dialog falls back to Shut down.
  $options:
    - shutdown: Shut down
    - restart: Restart
    - signout: Sign out
    - switchuser: Switch user
    - sleep: Sleep
    - hibernate: Hibernate

  $name:de-DE: Standardaktion
  $description:de-DE: Die Aktion, die im Alt+F4-Dialog „Windows herunterfahren“ vorausgewählt ist. Wird die gewählte Aktion nicht angeboten (siehe die Verfügbarkeits-Schalter unten), fällt der Dialog auf „Herunterfahren“ zurück.
  $options:de-DE:
    - shutdown: Herunterfahren
    - restart: Neu starten
    - signout: Abmelden
    - switchuser: Benutzer wechseln
    - sleep: Energie sparen
    - hibernate: Ruhezustand

- OfferSleep: true
  $name: Offer Sleep in the Alt+F4 dialog
  $description: Desired state - ON shows the Sleep action in the Alt+F4 dialog, OFF hides it (machine-wide, also the Start menu power menu). Applied on change with a UAC prompt when the state differs.

  $name:de-DE: „Energie sparen“ im Alt+F4-Dialog anbieten
  $description:de-DE: Soll-Zustand - AN zeigt „Energie sparen“ im Alt+F4-Dialog an, AUS blendet es aus (maschinenweit, auch im Startmenü-Power-Menü). Wird bei Änderung mit einer UAC-Aufforderung angewendet, wenn der Zustand abweicht.

- OfferHibernate: false
  $name: Offer Hibernate in the Alt+F4 dialog
  $description: Desired state - ON shows the Hibernate action and enables the hibernation infrastructure if disabled (can allocate hiberfil.sys, gigabytes); OFF hides the action but never disables hibernation. Machine-wide, UAC prompt on change.

  $name:de-DE: „Ruhezustand“ im Alt+F4-Dialog anbieten
  $description:de-DE: Soll-Zustand - AN zeigt „Ruhezustand“ an und aktiviert die Ruhezustand-Infrastruktur, falls deaktiviert (kann hiberfil.sys in Gigabyte-Größe allozieren); AUS blendet nur die Aktion aus und deaktiviert den Ruhezustand nie. Maschinenweit, UAC-Aufforderung bei Änderung.
*/
// ==/WindhawkModSettings==

#include <cstddef>
#include <new>
#include <objbase.h>
#include <powrprof.h>
#include <shellapi.h>
#include <sddl.h>
#include <string>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <wtsapi32.h>

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
        alignas(TOKEN_USER) BYTE
            tokenUserBuffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE] = {};
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
    // Reset the event BEFORE the in-flight increment: Wh_ModBeforeUninit
    // checks the counter first and then waits - an event still signaled
    // from the previous dialog would let the wait return immediately.
    if (g_dialogDoneEvent) {
        ResetEvent(g_dialogDoneEvent);
    }
    LONG inFlight = InterlockedIncrement(&g_inFlight);
    if (inFlight == 1) {
        // Outermost call only: overlapping calls on other threads must not
        // repoint the unload's dialog-close enumeration.
        g_dialogThreadId = GetCurrentThreadId();
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

    // Cross-process guard: while the dialog is open, other explorer
    // instances must not replay the pending-restore bookkeeping (they
    // would revert the transient value mid-dialog and delete the record -
    // see ApplyPendingRestore). Only the outermost call holds it.
    HANDLE dialogMutex = nullptr;
    if (outermost && !g_userSuffix.empty()) {
        dialogMutex = CreateMutexW(
            nullptr, TRUE,
            (L"Local\\altf4-preferences-dialog-" + g_userSuffix).c_str());
    }

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

    if (dialogMutex) {
        ReleaseMutex(dialogMutex);
        CloseHandle(dialogMutex);
    }

    Wh_Log(L"< Alt+F4 dialog closed");

    // The event is the last thing this function touches: Wh_ModBeforeUninit
    // may continue into the unload as soon as it is signaled, so no module
    // code may run after it (except the unavoidable return instruction).
    LONG left = InterlockedDecrement(&g_inFlight);
    if (left == 0 && g_dialogDoneEvent) {
        SetEvent(g_dialogDoneEvent);
    }
}

void InstallHooks() {
    // GetModuleHandle first (explorer has shell32 as a static import, so
    // it is always loaded at this point); LoadLibraryEx fallback as
    // defence-in-depth - a NULL here would silently skip the v1.0 core
    // hook. The fallback reference is deliberately never released (a
    // single harmless pin, same pattern as powrprof).
    HMODULE shell32Module = GetModuleHandleW(L"shell32.dll");
    if (!shell32Module) {
        shell32Module = LoadLibraryExW(L"shell32.dll", nullptr,
                                       LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
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

    // Another explorer instance may have the Alt+F4 dialog open (its hook
    // holds the dialog mutex): replaying the pending restore now would
    // revert the transient value mid-dialog and delete the record. Keep
    // the record - the dialog's own restore clears it on success.
    HANDLE dialogMutex = OpenMutexW(
        SYNCHRONIZE, FALSE,
        (L"Local\\altf4-preferences-dialog-" + g_userSuffix).c_str());
    if (dialogMutex) {
        Wh_Log(L"Dialog open in another instance, deferring pending restore");
        CloseHandle(dialogMutex);
        return;
    }

    HKEY hKey;
    BOOL restoredOk = FALSE;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, kRegistrySubKey, 0, KEY_SET_VALUE,
                     &hKey) == ERROR_SUCCESS) {
        if (pendingExists == 1) {
            DWORD value = (DWORD)Wh_GetIntValue(pendingValueName.c_str(), 0);
            restoredOk = RegSetValueEx(hKey, kRegistryValue, 0, REG_DWORD,
                                       (const BYTE *)&value,
                                       sizeof(value)) == ERROR_SUCCESS;
            Wh_Log(L"Reverted leftover value to 0x%08lX (result: %s)", value,
                   restoredOk ? L"ok" : L"FAILED");
        } else {
            LONG deleteRes = RegDeleteValue(hKey, kRegistryValue);
            restoredOk = deleteRes == ERROR_SUCCESS ||
                         deleteRes == ERROR_FILE_NOT_FOUND;
            Wh_Log(L"Removed leftover value (result: %s)",
                   restoredOk ? L"ok" : L"FAILED");
        }
        RegCloseKey(hKey);
    }

    // Only clear the record when the restore succeeded - otherwise retry
    // on the next load (same discipline as the hook's restore).
    if (restoredOk) {
        Wh_DeleteValue(pendingExistsName.c_str());
        Wh_DeleteValue(pendingValueName.c_str());
    }
}

// ---------------------------------------------------------------------------
// v1.1 availability feature
//
// Sleep and Hibernate only appear in the Alt+F4 dialog when they are enabled
// machine-wide. The visibility switches live in
// HKLM\...\Explorer\FlyoutMenuSettings (ShowSleepOption /
// ShowHibernateOption; a missing value has no verified default semantics -
// observed: hidden for Hibernate). The mod therefore writes explicit 0/1
// in both directions. Hibernation can be disabled system-wide entirely
// (IsPwrHibernateAllowed). The dialog reads these values live, so writes
// take effect immediately.
//
// Explorer runs unelevated, so changes go through a single elevated cmd.exe
// chain: one UAC prompt per apply, and only when something actually needs to
// change (read-first, idempotent). The policy layer (Policies\...\Explorer)
// overrides FlyoutMenuSettings and is reported, not fought. HiberbootEnabled
// (fast startup) is deliberately not touched. Modern Standby (S0 idle) is
// covered via CallNtPowerInformation (see DetectAvailabilityState).
//
// NOTE (re-work candidate for Windhawk 2.0): tool mods on Windhawk 2.0 can
// request an elevated host process (windhawk-mod-elevated.exe). Once 2.0 is
// widespread, the UAC-on-apply flow below can be replaced by silent writes
// from that host context (deferred option in the v1.1 design, not
// abandoned). Status UX re-work candidate as well: 2.0's $dynamicSelect
// could surface the live availability state in the settings UI - 1.7.3
// settings cannot be written by the mod, so the switches are desired-
// state controllers: they never display the machine state, but every
// flip syncs it and reports the result. A $dynamicSelect DefaultAction
// could on 2.0 also label or drop options whose action is currently
// hidden (on 1.7.3 the fallback is documented in the setting description
// and reported in the apply popup instead).
// ---------------------------------------------------------------------------
constexpr PCWSTR kFlyoutSettingsSubKey =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FlyoutMenuSettings";
constexpr PCWSTR kExplorerPolicySubKey =
    L"SOFTWARE\\Policies\\Microsoft\\Windows\\Explorer";
constexpr PCWSTR kShowSleepOption = L"ShowSleepOption";
constexpr PCWSTR kShowHibernateOption = L"ShowHibernateOption";

// Correct prototypes per powrprof.h: BOOLEAN is an unsigned char, not BOOL.
// Resolved dynamically, so no powrprof.lib dependency is needed.
using IsPwrSuspendAllowed_t = BOOLEAN(WINAPI *)();
using IsPwrHibernateAllowed_t = BOOLEAN(WINAPI *)();
using CallNtPowerInformation_t = LONG(WINAPI *)(POWER_INFORMATION_LEVEL,
                                                PVOID, ULONG, PVOID, ULONG);
IsPwrSuspendAllowed_t IsPwrSuspendAllowed_Original = nullptr;
IsPwrHibernateAllowed_t IsPwrHibernateAllowed_Original = nullptr;
CallNtPowerInformation_t CallNtPowerInformation_Original = nullptr;

// Layout guards for the spare2 byte split: the mingw-w64
// struct predates the SDK split of spare2[3] into Hiberboot (18),
// WakeAlarmPresent (19) and AoAc (20).
static_assert(offsetof(SYSTEM_POWER_CAPABILITIES, spare2) == 18 &&
                  offsetof(SYSTEM_POWER_CAPABILITIES, DiskSpinDown) == 21,
              "SYSTEM_POWER_CAPABILITIES layout changed");

// Serialized applies + the job handed to the worker. ALL Wh_* storage and
// settings access happens on the engine thread (in Wh_ModSettingsChanged):
// Wh_* value reads are not reliable from a raw CreateThread thread (they
// returned defaults there, which silently swallowed every flip), while
// Wh_Log works. The worker therefore only uses plain Win32 APIs.
//
// Unload safety: the engine thread pins the module for the worker's
// lifetime (GetModuleHandleExW) and keeps the thread handle; the worker
// ends with FreeLibraryAndExitThread. The worker can outlive the 10 s
// grace wait in Wh_ModBeforeUninit (UAC prompt / popup are unbounded) -
// with the pin that is safe even after the engine frees its mod context.
volatile LONG g_applyRunning = 0;
HANDLE g_applyThread = nullptr;
DWORD g_applyThreadId = 0;
// Watchdog: tick when the current apply was dispatched. A worker can get
// stuck inside ShellExecuteExW waiting for a UAC decision that never
// displays (non-foreground requester, see review Q5) - after 5 minutes a
// new dispatch overrides the stale one instead of being blocked forever.
volatile ULONG g_applyStartTick = 0;
// Generation guard: an old worker must not clear g_applyRunning when a
// newer dispatch took over (stale-apply watchdog) - each worker only
// clears the flag if its generation is still current.
volatile LONG g_applyGeneration = 0;

struct ApplyJob {
    HMODULE pin = nullptr;
    LONG generation = 0;
    // Window the UAC consent should attach to (captured on the engine
    // thread: usually the Windhawk window the user just saved in). The
    // consent process receives it as a value only - cross-process is fine
    // here, unlike the MessageBox owner that failed earlier.
    HWND consentOwner = nullptr;
    bool sleepShow = false;
    bool sleepHide = false;
    bool hibernateShow = false;
    bool hibernateHide = false;
    bool hibernateSystemwideEnable = false;
    // Lines for conditions that need no elevation ("already ...",
    // policy/hardware hints), composed on the engine thread.
    std::wstring prefix;
    // Whether the current default action references an action this apply
    // may hide - the worker appends the fallback note if the hide
    // verifies.
    bool defaultIsSleep = false;
    bool defaultIsHibernate = false;
};

bool IsUnloading() {
    return InterlockedCompareExchange(&g_unloading, 0, 0) != 0;
}

// Wh_Log gate for the worker thread: once the mod is unloading, the
// engine-side log context may already be gone.
#define WORKER_LOG(message, ...)                        \
    do {                                                \
        if (!IsUnloading()) {                           \
            Wh_Log(message, ##__VA_ARGS__);             \
        }                                               \
    } while (0)

// Reads a REG_DWORD from HKLM. Returns -1 when the value does not exist (the
// normal case for defaults) or is not a DWORD.
// WORKER_LOG instead of Wh_Log: this function also runs on the apply worker
// (via DetectAvailabilityState), where the raw Wh_Log needs the gate.
int GetMachineDword(PCWSTR subKey, PCWSTR valueName) {
    DWORD value = 0;
    DWORD size = sizeof(value);
    LONG res = RegGetValue(HKEY_LOCAL_MACHINE, subKey, valueName,
                           RRF_RT_REG_DWORD, nullptr, &value, &size);
    if (res == ERROR_SUCCESS) {
        return (int)value;
    }
    if (res != ERROR_FILE_NOT_FOUND) {
        WORKER_LOG(L"RegGetValue(HKLM\\%s\\%s) failed: %ld", subKey,
                   valueName, res);
    }
    return -1;
}

struct AvailabilityState {
    // FlyoutMenuSettings values, -1 = value not present (no verified
    // default semantics; observed: hidden for Hibernate). The mod writes
    // explicit 0/1 in both directions, so both semantics are handled.
    int sleepShown = -1;
    int hibernateShown = -1;
    // Policy layer (Policies\...\Explorer), -1 = not set.
    int sleepPolicy = -1;
    int hibernatePolicy = -1;
    // System-wide power state.
    bool hibernateEnabled = false;  // IsPwrHibernateAllowed
    bool suspendSupported = false;  // IsPwrSuspendAllowed
};

AvailabilityState DetectAvailabilityState() {
    AvailabilityState state;
    state.sleepShown = GetMachineDword(kFlyoutSettingsSubKey, kShowSleepOption);
    state.hibernateShown =
        GetMachineDword(kFlyoutSettingsSubKey, kShowHibernateOption);
    state.sleepPolicy = GetMachineDword(kExplorerPolicySubKey, kShowSleepOption);
    state.hibernatePolicy =
        GetMachineDword(kExplorerPolicySubKey, kShowHibernateOption);
    if (IsPwrHibernateAllowed_Original) {
        state.hibernateEnabled = IsPwrHibernateAllowed_Original() != 0;
    }
    if (CallNtPowerInformation_Original) {
        // Precise sleep-support check: also covers Modern Standby (S0 low
        // power idle, AoAc), which IsPwrSuspendAllowed (S1-S3) misses.
        // NOTE: the mingw-w64 header predates the SDK split of spare2[3]
        // into Hiberboot (18), WakeAlarmPresent (19) and AoAc (20) - AoAc
        // is spare2[2] (verified against a raw CallNtPowerInformation dump
        // on a Hiberboot-enabled S3 machine).
        SYSTEM_POWER_CAPABILITIES caps = {};
        if (CallNtPowerInformation_Original(SystemPowerCapabilities, nullptr,
                                            0, &caps,
                                            sizeof(caps)) == 0) {
            state.suspendSupported =
                caps.SystemS1 || caps.SystemS2 || caps.SystemS3 ||
                caps.spare2[2] != 0;
        } else if (IsPwrSuspendAllowed_Original) {
            state.suspendSupported = IsPwrSuspendAllowed_Original() != 0;
        }
    } else if (IsPwrSuspendAllowed_Original) {
        state.suspendSupported = IsPwrSuspendAllowed_Original() != 0;
    }
    return state;
}

void InstallPowerHelpers() {
    // GetModuleHandle first (no refcount growth on reloads); LoadLibraryEx
    // as fallback. The reference taken by the fallback is deliberately
    // never released: the pinned apply worker may still call the IsPwr*/
    // CallNtPowerInformation pointers after unload.
    HMODULE powrprof = GetModuleHandleW(L"powrprof.dll");
    if (!powrprof) {
        powrprof = LoadLibraryExW(L"powrprof.dll", nullptr,
                                  LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (powrprof) {
        IsPwrSuspendAllowed_Original =
            (IsPwrSuspendAllowed_t)GetProcAddress(powrprof,
                                                  "IsPwrSuspendAllowed");
        IsPwrHibernateAllowed_Original =
            (IsPwrHibernateAllowed_t)GetProcAddress(powrprof,
                                                    "IsPwrHibernateAllowed");
        CallNtPowerInformation_Original =
            (CallNtPowerInformation_t)GetProcAddress(
                powrprof, "CallNtPowerInformation");
    }
    if (!IsPwrSuspendAllowed_Original || !IsPwrHibernateAllowed_Original) {
        Wh_Log(L"powrprof IsPwr* helpers not available, system-wide "
               L"detection limited");
    }
}

// Multi-instance/multi-session safety: every explorer.exe process gets
// Wh_ModSettingsChanged, and the settings plus the machine-wide target
// values are shared. Only the session's shell process (GetShellWindow) of
// an ACTIVE session may apply - other instances (separate-process option,
// disconnected fast-user-switching/RDP sessions) must stay out, or the
// user gets duplicate or foreign UAC prompts. Known limitation: two
// SIMULTANEOUSLY active sessions (console + RDP on multi-session editions)
// are both designated and can both prompt for the same machine-wide flip
// (documented, not blocked).
bool IsDesignatedApplier() {
    HWND shellWindow = GetShellWindow();
    DWORD pid = 0;
    if (!shellWindow ||
        !GetWindowThreadProcessId(shellWindow, &pid) ||
        pid != GetCurrentProcessId()) {
        return false;
    }
    // Skip disconnected sessions (fast user switching): they would apply a
    // machine-wide change for a user who is not looking. Optimistic
    // default on query failure - the shell-window check already filtered
    // the common cases.
    WCHAR *sessionInfo = nullptr;
    DWORD bytes = 0;
    bool active = true;
    if (WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE,
                                    WTS_CURRENT_SESSION, WTSConnectState,
                                    &sessionInfo, &bytes)) {
        active = *((DWORD *)sessionInfo) == WTSActive;
        WTSFreeMemory(sessionInfo);
    }
    return active;
}

// Runtime popup language: the Windhawk metadata localization (de-DE
// blocks) covers the settings UI, these strings cover the apply popups.
// English is the base; German is used for any German UI language.
bool IsGermanUiLanguage() {
    return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_GERMAN;
}

struct FeedbackStrings {
    PCWSTR boxTitle;
    PCWSTR sleepPolicy;
    PCWSTR sleepDefaultFallbackNote;
    PCWSTR sleepUnsupported;
    PCWSTR sleepAlreadyOffered;
    PCWSTR sleepAlreadyHidden;
    PCWSTR sleepNowOffered;
    PCWSTR sleepNowHidden;
    PCWSTR sleepFailOffer;
    PCWSTR sleepFailHide;
    PCWSTR hibernatePolicy;
    PCWSTR hibernateDefaultFallbackNote;
    PCWSTR hibernateAlreadyOffered;
    PCWSTR hibernateAlreadyHidden;
    PCWSTR hibernateAlreadyEnabled;
    PCWSTR hibernateNowOffered;
    PCWSTR hibernateNowHidden;
    PCWSTR hibernateNowEnabled;
    PCWSTR hibernateFailOffer;
    PCWSTR hibernateFailHide;
    PCWSTR hibernateFailEnable;
    PCWSTR cancelled;
    PCWSTR elevationFailed;
    PCWSTR defaultActionNote;
    PCWSTR stillRunning;
    PCWSTR staleNote;
    PCWSTR defaultActionUnavailableNote;
};

// The LAST member of both tables must be non-null: a positional initializer
// gap shifts nullptr into the trailing members (the struct-size assert
// cannot catch that, and the build passes -Wno-missing-field-initializers).
static_assert(sizeof(FeedbackStrings) == 27 * sizeof(PCWSTR),
              "FeedbackStrings field mismatch");

constexpr FeedbackStrings kStringsEn = {
    L"Alt+F4 Dialog Preferences",
    L"Sleep visibility is controlled by a policy setting and cannot be "
    L"changed here.\n",
    L"Note: Sleep is the current default action, the dialog will fall "
    L"back to \"Shut down\".\n",
    L"Note: this system does not support any sleep state, Sleep cannot "
    L"be offered.\n",
    L"Sleep is already offered in the Alt+F4 dialog.\n",
    L"Sleep is already hidden from the Alt+F4 dialog.\n",
    L"Sleep is now offered in the Alt+F4 dialog.\n",
    L"Sleep is now hidden from the Alt+F4 dialog (machine-wide, also "
    L"affects the Start menu power menu).\n",
    L"FAILED: Sleep could not be offered.\n",
    L"FAILED: Sleep could not be hidden.\n",
    L"Hibernate visibility is controlled by a policy setting and cannot "
    L"be changed here.\n",
    L"Note: Hibernate is the current default action, the dialog will "
    L"fall back to \"Shut down\".\n",
    L"Hibernate is already offered in the Alt+F4 dialog.\n",
    L"Hibernate is already hidden from the Alt+F4 dialog.\n",
    L"Hibernate is already enabled system-wide.\n",
    L"Hibernate is now offered in the Alt+F4 dialog.\n",
    L"Hibernate is now hidden from the Alt+F4 dialog (machine-wide, "
    L"also affects the Start menu power menu).\n",
    L"Hibernate is now enabled system-wide.\n",
    L"FAILED: Hibernate could not be offered.\n",
    L"FAILED: Hibernate could not be hidden.\n",
    L"FAILED: Hibernate could not be enabled system-wide.\n",
    L"Cancelled (UAC prompt declined) - no changes were applied. Toggle "
    L"the switch back and forth to retry.\n",
    L"FAILED: the change could not be applied (elevation failed). Toggle "
    L"the switch back and forth to retry.\n",
    L"Note: the hidden action is the current default action. The dialog "
    L"will fall back to \"Shut down\" until you pick another default.\n",
    L"The change is still being applied (the elevated step is taking "
    L"unusually long) - check the Alt+F4 dialog in a moment.\n",
    L"An older availability request was completed out of order - check "
    L"which actions the Alt+F4 dialog shows and the switches in the mod "
    L"settings.\n",
    L"Note: Hibernate is the current default action but could not be "
    L"enabled system-wide. The dialog will fall back to \"Shut down\" "
    L"until it is available.\n",
};

constexpr FeedbackStrings kStringsDe = {
    L"Alt+F4-Dialog-Einstellungen",
    L"Die Sichtbarkeit von \u201EEnergie sparen\u201C wird durch eine "
    L"Richtlinie gesteuert und kann hier nicht ge\u00E4ndert werden.\n",
    L"Hinweis: \u201EEnergie sparen\u201C ist die aktuelle Standard"
    L"aktion, der Dialog f\u00E4llt auf \u201EHerunterfahren\u201C "
    L"zur\u00FCck.\n",
    L"Hinweis: Dieses System unterst\u00FCtzt keinen Standby-Zustand, "
    L"\u201EEnergie sparen\u201C kann nicht angeboten werden.\n",
    L"\u201EEnergie sparen\u201C wird im Alt+F4-Dialog bereits "
    L"angeboten.\n",
    L"\u201EEnergie sparen\u201C ist im Alt+F4-Dialog bereits "
    L"ausgeblendet.\n",
    L"\u201EEnergie sparen\u201C wird jetzt im Alt+F4-Dialog "
    L"angeboten.\n",
    L"\u201EEnergie sparen\u201C wird jetzt im Alt+F4-Dialog "
    L"ausgeblendet (maschinenweit, betrifft auch das "
    L"Startmen\u00FC-Power-Men\u00FC).\n",
    L"FEHLGESCHLAGEN: \u201EEnergie sparen\u201C konnte nicht angeboten "
    L"werden.\n",
    L"FEHLGESCHLAGEN: \u201EEnergie sparen\u201C konnte nicht "
    L"ausgeblendet werden.\n",
    L"Die Sichtbarkeit von \u201ERuhezustand\u201C wird durch eine "
    L"Richtlinie gesteuert und kann hier nicht ge\u00E4ndert werden.\n",
    L"Hinweis: \u201ERuhezustand\u201C ist die aktuelle Standardaktion, "
    L"der Dialog f\u00E4llt auf \u201EHerunterfahren\u201C zur\u00FCck."
    L"\n",
    L"\u201ERuhezustand\u201C wird im Alt+F4-Dialog bereits "
    L"angeboten.\n",
    L"\u201ERuhezustand\u201C ist im Alt+F4-Dialog bereits "
    L"ausgeblendet.\n",
    L"\u201ERuhezustand\u201C ist systemweit bereits aktiviert.\n",
    L"\u201ERuhezustand\u201C wird jetzt im Alt+F4-Dialog "
    L"angeboten.\n",
    L"\u201ERuhezustand\u201C wird jetzt im Alt+F4-Dialog ausgeblendet "
    L"(maschinenweit, betrifft auch das Startmen\u00FC-Power-Men\u00FC)."
    L"\n",
    L"\u201ERuhezustand\u201C ist jetzt systemweit aktiviert.\n",
    L"FEHLGESCHLAGEN: \u201ERuhezustand\u201C konnte nicht angeboten "
    L"werden.\n",
    L"FEHLGESCHLAGEN: \u201ERuhezustand\u201C konnte nicht ausgeblendet "
    L"werden.\n",
    L"FEHLGESCHLAGEN: \u201ERuhezustand\u201C konnte nicht systemweit "
    L"aktiviert werden.\n",
    L"Abgebrochen (UAC-Aufforderung verweigert) - es wurden keine "
    L"\u00C4nderungen vorgenommen. Schalte den Schalter einmal hin und "
    L"zur\u00FCck, um es erneut zu versuchen.\n",
    L"FEHLGESCHLAGEN: Die \u00C4nderung konnte nicht angewendet werden "
    L"(Elevation fehlgeschlagen). Schalte den Schalter einmal hin und "
    L"zur\u00FCck, um es erneut zu versuchen.\n",
    L"Hinweis: Die ausgeblendete Aktion ist die aktuelle Standardaktion. "
    L"Der Dialog f\u00E4llt auf \u201EHerunterfahren\u201C zur\u00FCck, "
    L"bis eine andere Standardaktion gew\u00E4hlt wird.\n",
    L"Die \u00C4nderung wird noch angewendet (der Schritt mit "
    L"Administratorrechten dauert ungew\u00F6hnlich lange) - pr\u00FCfe "
    L"gleich den Alt+F4-Dialog.\n",
    L"Ein \u00E4lterer Verf\u00FCgbarkeits-Auftrag wurde au\u00DFer der "
    L"Reihe abgeschlossen - pr\u00FCfe, welche Aktionen der Alt+F4-Dialog "
    L"anzeigt, und die Schalter in den Mod-Einstellungen.\n",
    L"Hinweis: \u201ERuhezustand\u201C ist die aktuelle Standardaktion, "
    L"konnte aber nicht systemweit aktiviert werden. Der Dialog f\u00E4llt "
    L"auf \u201EHerunterfahren\u201C zur\u00FCck, bis er verf\u00FCgbar "
    L"ist.\n",
};

// The LAST member of both tables must be non-null: a positional initializer
// gap shifts nullptr into the trailing members (the struct-size assert
// cannot catch that, and the build passes -Wno-missing-field-initializers).
static_assert(kStringsEn.defaultActionUnavailableNote != nullptr,
              "EN string table incomplete");
static_assert(kStringsDe.defaultActionUnavailableNote != nullptr,
              "DE string table incomplete");

const FeedbackStrings *g_text = &kStringsEn;

// The apply worker has no foreground rights, so the box needs explicit
// raising: system-modal + topmost + set-foreground makes it appear above
// everything with focus. No owner window: cross-process owners (e.g. the
// Windhawk window) made the box fail to display entirely.
void ShowApplyFeedback(PCWSTR text, bool error) {
    UINT flags = MB_OK | (error ? MB_ICONERROR : MB_ICONINFORMATION) |
                 MB_SETFOREGROUND | MB_TOPMOST | MB_SYSTEMMODAL;
    MessageBoxW(nullptr, text, g_text->boxTitle, flags);
}

// The worker body: only plain Win32 APIs (plus the gated WORKER_LOG), no
// Wh_* value access - see the ApplyJob comment. Kept as a helper so no
// destructors run after the FreeLibraryAndExitThread in the thread proc.
void ApplyAvailabilityWorker(ApplyJob *job) {
    // SEE_MASK_NOASYNC requires COM on the calling thread.
    HRESULT comInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED |
                                               COINIT_DISABLE_OLE1DDE);

    std::wstring message = job->prefix;
    bool anyFailure = false;
    bool sleepHidden = false;
    bool hibernateHidden = false;
    bool hibernateEnableFailed = false;
    bool stillRunning = false;
    bool generationStale = false;

    bool elevatedRan = false;
    bool cancelled = false;
    if (job->sleepShow || job->sleepHide || job->hibernateShow ||
        job->hibernateHide || job->hibernateSystemwideEnable) {
        // One elevated command chain. Absolute system paths plus /d /s
        // keep the elevated context free of user-writable configuration
        // (cmd AutoRun) and search-path resolution; /reg:64 forces the
        // 64-bit registry view as insurance (the key is shared anyway).
        wchar_t sysDirBuf[MAX_PATH];
        UINT sysLen = GetSystemDirectoryW(sysDirBuf, ARRAYSIZE(sysDirBuf));
        if (sysLen > 0 && sysLen < ARRAYSIZE(sysDirBuf)) {
            std::wstring sysDir(sysDirBuf, sysLen);
            std::wstring cmdPath = sysDir + L"\\cmd.exe";
            std::wstring reg = L"\"" + sysDir + L"\\reg.exe\"";
            std::wstring powercfg = L"\"" + sysDir + L"\\powercfg.exe\"";
            std::wstring chain;
            bool first = true;
            auto append = [&](const std::wstring &part) {
                if (!first) {
                    chain += L" & ";
                }
                first = false;
                chain += part;
            };
            if (job->sleepShow) {
                append(reg + L" add HKLM\\SOFTWARE\\Microsoft\\Windows\\"
                       L"CurrentVersion\\Explorer\\FlyoutMenuSettings /v "
                       L"ShowSleepOption /t REG_DWORD /d 1 /f /reg:64");
            }
            if (job->sleepHide) {
                append(reg + L" add HKLM\\SOFTWARE\\Microsoft\\Windows\\"
                       L"CurrentVersion\\Explorer\\FlyoutMenuSettings /v "
                       L"ShowSleepOption /t REG_DWORD /d 0 /f /reg:64");
            }
            if (job->hibernateShow) {
                append(reg + L" add HKLM\\SOFTWARE\\Microsoft\\Windows\\"
                       L"CurrentVersion\\Explorer\\FlyoutMenuSettings /v "
                       L"ShowHibernateOption /t REG_DWORD /d 1 /f "
                       L"/reg:64");
            }
            if (job->hibernateHide) {
                append(reg + L" add HKLM\\SOFTWARE\\Microsoft\\Windows\\"
                       L"CurrentVersion\\Explorer\\FlyoutMenuSettings /v "
                       L"ShowHibernateOption /t REG_DWORD /d 0 /f "
                       L"/reg:64");
            }
            if (job->hibernateSystemwideEnable) {
                append(powercfg + L" /hibernate on");
            }

            std::wstring params = L"/d /s /c \"" + chain + L"\"";
            WORKER_LOG(L"Elevated availability apply: %s", params.c_str());

            SHELLEXECUTEINFOW sei = {};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI |
                        SEE_MASK_NOASYNC;
            sei.lpVerb = L"runas";  // one UAC prompt for the whole chain
            sei.lpFile = cmdPath.c_str();
            sei.lpParameters = params.c_str();
            // Consent owner: without a foreground requester the UAC prompt
            // does not display - it lands as a flashing taskbar button and
            // ShellExecuteExW blocks forever (observed in v1.1.34; the
            // lpDirectory theory was a timing coincidence, lpDirectory
            // stays unset). The Windhawk window the user just saved in is
            // captured on the engine thread and passed as the consent
            // owner - cross-process is fine here, consent.exe receives it
            // as a value only (unlike the MessageBox owner, which failed
            // to display).
            sei.hwnd = job->consentOwner;
            sei.nShow = SW_HIDE;

            if (ShellExecuteExW(&sei)) {
                elevatedRan = true;
                generationStale =
                    InterlockedCompareExchange(&g_applyGeneration, 0, 0) !=
                    job->generation;
                if (sei.hProcess) {
                    DWORD wait = WaitForSingleObject(sei.hProcess, 60000);
                    DWORD exitCode = 0;
                    GetExitCodeProcess(sei.hProcess, &exitCode);
                    CloseHandle(sei.hProcess);
                    WORKER_LOG(L"Elevated apply exit code: %lu (wait: %lu)",
                               exitCode, wait);
                    if (wait != WAIT_OBJECT_0) {
                        // Timeout or abandoned wait: the chain may still
                        // be running - report "unknown", not "FAILED".
                        stillRunning = true;
                    }
                } else {
                    stillRunning = true;
                }
            } else {
                DWORD err = GetLastError();
                WORKER_LOG(L"ShellExecuteExW failed: %lu", err);
                cancelled = err == ERROR_CANCELLED;
            }
        } else {
            WORKER_LOG(L"GetSystemDirectoryW failed (%lu)", GetLastError());
        }
    }

    if (elevatedRan && generationStale) {
        // A newer dispatch took over while this one was pending - the
        // writes may still have happened, but the report would be out of
        // order. Stay neutral; the newer popup reports the current state.
        message += g_text->staleNote;
    } else if (elevatedRan && stillRunning) {
        // The chain may still succeed a moment later - report "unknown",
        // don't verify against a moving target.
        message += g_text->stillRunning;
    } else if (elevatedRan) {
        // Verify with plain APIs (no Windhawk context needed here).
        AvailabilityState after = DetectAvailabilityState();
        if (job->sleepShow) {
            if (after.sleepShown == 1) {
                message += g_text->sleepNowOffered;
            } else {
                message += g_text->sleepFailOffer;
                anyFailure = true;
            }
        }
        if (job->sleepHide) {
            if (after.sleepShown == 0) {
                message += g_text->sleepNowHidden;
                sleepHidden = true;
            } else {
                message += g_text->sleepFailHide;
                anyFailure = true;
            }
        }
        if (job->hibernateShow) {
            if (after.hibernateShown == 1) {
                message += g_text->hibernateNowOffered;
            } else {
                message += g_text->hibernateFailOffer;
                anyFailure = true;
            }
        }
        if (job->hibernateHide) {
            if (after.hibernateShown == 0) {
                message += g_text->hibernateNowHidden;
                hibernateHidden = true;
            } else {
                message += g_text->hibernateFailHide;
                anyFailure = true;
            }
        }
        if (job->hibernateSystemwideEnable) {
            if (after.hibernateEnabled) {
                message += g_text->hibernateNowEnabled;
            } else {
                message += g_text->hibernateFailEnable;
                anyFailure = true;
                hibernateEnableFailed = true;
            }
        }
    } else if (cancelled) {
        message += g_text->cancelled;
    } else if (job->sleepShow || job->sleepHide || job->hibernateShow ||
               job->hibernateHide || job->hibernateSystemwideEnable) {
        message += g_text->elevationFailed;
        anyFailure = true;
    }

    // Hiding an action that is also the preselected default makes the
    // dialog fall back to "Shut down" - say so explicitly instead of
    // changing anything implicitly (mod settings cannot be written by
    // the mod anyway).
    if ((sleepHidden && job->defaultIsSleep) ||
        (hibernateHidden && job->defaultIsHibernate)) {
        message += g_text->defaultActionNote;
    }
    if (hibernateEnableFailed && job->defaultIsHibernate) {
        message += g_text->defaultActionUnavailableNote;
    }

    if (!message.empty()) {
        ShowApplyFeedback(message.c_str(), anyFailure);
    }

    if (SUCCEEDED(comInit)) {
        CoUninitialize();
    }
}

DWORD WINAPI ApplyAvailabilityThreadProc(LPVOID parameter) {
    ApplyJob *job = (ApplyJob *)parameter;
    HMODULE pin = job->pin;
    LONG myGeneration = job->generation;
    ApplyAvailabilityWorker(job);
    // Module still pinned here - deleting the job (and running its string
    // destructor) is safe.
    delete job;
    if (InterlockedCompareExchange(&g_applyGeneration, 0, 0) == myGeneration) {
        // Only clear the busy flag if no newer dispatch took over (stale
        // watchdog override).
        InterlockedExchange(&g_applyRunning, 0);
    }
    FreeLibraryAndExitThread(pin, 0);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    // Defensive resets for a fast disable/re-enable cycle while a pinned
    // worker from the previous load may still run: a fresh mod instance
    // must not inherit stale state (in particular g_unloading, which would
    // make every hooked dialog call return immediately).
    InterlockedExchange(&g_unloading, 0);
    InterlockedExchange(&g_inFlight, 0);
    InterlockedExchange(&g_applyRunning, 0);
    g_applyThread = nullptr;
    g_applyThreadId = 0;

    g_userSuffix = GetUserStorageSuffix();
    if (g_userSuffix.empty()) {
        Wh_Log(L"Failed to get the user SID, leftover cleanup disabled");
    } else {
        ApplyPendingRestore();
    }
    g_dialogDoneEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    InstallPowerHelpers();
    g_text = IsGermanUiLanguage() ? &kStringsDe : &kStringsEn;
    // Seed the last-seen availability switch values ONLY when the
    // bookkeeping does not exist yet: an unconditional seed would wipe a
    // pending flip on every explorer restart, and Wh_ModInit runs in every
    // process (including non-designated instances). The avLast values are
    // machine-wide (no SID suffix - the switches and the HKLM targets are
    // machine-wide too). A flip made while the mod was disabled is applied
    // at the next settings save (desired-state model).
    if (Wh_GetIntValue(L"avLastSleep_", -1) == -1) {
        Wh_SetIntValue(L"avLastSleep_",
                       Wh_GetIntSetting(L"OfferSleep") != 0 ? 1 : 0);
    }
    if (Wh_GetIntValue(L"avLastHibernate_", -1) == -1) {
        Wh_SetIntValue(L"avLastHibernate_",
                       Wh_GetIntSetting(L"OfferHibernate") != 0 ? 1 : 0);
    }
    LoadSettings();
    InstallHooks();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();

    // Multi-instance/multi-session safety: every explorer.exe instance
    // receives this callback. Only the designated applier (see above) may
    // run the availability sync - and it must not touch the flip
    // bookkeeping either, because the storage is shared and a
    // non-designated instance writing it first would hide the flip from
    // the designated instance.
    if (!IsDesignatedApplier()) {
        Wh_Log(L"Not the designated applier, skipping availability sync");
        return;
    }

    // Everything Windhawk-related happens here on the engine thread: the
    // Wh_* value reads are not reliable from a raw worker thread (they
    // returned defaults there, which silently swallowed every flip - the
    // worker only uses plain Win32 APIs, see ApplyJob).
    //
    // The avLast* bookkeeping values are deliberately MACHINE-WIDE (no SID
    // suffix): the switches and the HKLM target values are machine-wide,
    // so all sessions/instances must share one bookkeeping - otherwise
    // another session would re-apply an already handled flip (R4.1). The
    // SID suffix exists only for the per-user HKCU restore values.
    bool offerSleep = Wh_GetIntSetting(L"OfferSleep") != 0;
    bool offerHibernate = Wh_GetIntSetting(L"OfferHibernate") != 0;
    std::wstring sleepSeenName = L"avLastSleep_";
    std::wstring hibernateSeenName = L"avLastHibernate_";
    int lastSleep = Wh_GetIntValue(sleepSeenName.c_str(), 0);
    int lastHibernate = Wh_GetIntValue(hibernateSeenName.c_str(), 0);

    Wh_Log(L"Availability flip check: offerSleep=%d offerHibernate=%d "
           L"lastSleep=%d lastHibernate=%d",
           offerSleep ? 1 : 0, offerHibernate ? 1 : 0, lastSleep,
           lastHibernate);

    bool sleepFlipped = (offerSleep ? 1 : 0) != lastSleep;
    bool hibernateFlipped = (offerHibernate ? 1 : 0) != lastHibernate;
    if (!sleepFlipped && !hibernateFlipped) {
        // A save without a flip: no sync, no prompt.
        Wh_Log(L"Availability apply: no switch flipped, staying silent");
        return;
    }

    AvailabilityState state = DetectAvailabilityState();

    ApplyJob *job = new (std::nothrow) ApplyJob();
    if (!job) {
        // Flip stays pending (bookkeeping not written) - retried on the
        // next save - no restore writes on skip paths, they
        // would poison the machine-wide bookkeeping with a stale read).
        return;
    }
    bool defaultIsSleep = g_settings.action == DefaultAction::Sleep;
    bool defaultIsHibernate = g_settings.action == DefaultAction::Hibernate;

    if (sleepFlipped) {
        // The policy layer wins over FlyoutMenuSettings in both
        // directions: report it, don't fight it.
        if (state.sleepPolicy != -1) {
            job->prefix += g_text->sleepPolicy;
            if (state.sleepPolicy == 0 && defaultIsSleep) {
                job->prefix += g_text->sleepDefaultFallbackNote;
            }
        } else if (offerSleep) {
            if (state.sleepShown != 1) {
                // Write an explicit 1 even when the value is MISSING: the
                // default semantics of a missing value is not verified for
                // every action (observed: missing = hidden for Hibernate).
                // An explicit 1 is correct either way.
                job->sleepShow = true;
                // Hardware without any standby state can never actually
                // offer Sleep (the dialog falls back to "Shut down").
                if (!state.suspendSupported) {
                    job->prefix += g_text->sleepUnsupported;
                }
            } else if (!state.suspendSupported) {
                // "Already offered" would be misleading: the action cannot
                // appear on this hardware.
                job->prefix += g_text->sleepUnsupported;
            } else {
                job->prefix += g_text->sleepAlreadyOffered;
            }
        } else {
            // OFF with a MISSING value deliberately prompts for UAC to
            // write an explicit 0 - consistent with the explicit-values
            // approach even where the action may already be hidden.
            if (state.sleepShown != 0) {
                job->sleepHide = true;
            } else {
                job->prefix += g_text->sleepAlreadyHidden;
                if (defaultIsSleep) {
                    job->prefix += g_text->sleepDefaultFallbackNote;
                }
            }
        }
    }
    if (hibernateFlipped) {
        if (state.hibernatePolicy != -1) {
            job->prefix += g_text->hibernatePolicy;
            if (state.hibernatePolicy == 0 && defaultIsHibernate) {
                job->prefix += g_text->hibernateDefaultFallbackNote;
            }
            // The infrastructure is independent of visibility - but when a
            // policy hides the action anyway, enabling it would allocate
            // hiberfil.sys (gigabytes) for an invisible result. Skip.
            if (offerHibernate && !state.hibernateEnabled &&
                state.hibernatePolicy != 0) {
                job->hibernateSystemwideEnable = true;
            }
        } else if (offerHibernate) {
            if (state.hibernateShown != 1) {
                // Write an explicit 1 even when the value is MISSING:
                // observed behavior is missing = hidden for Hibernate
                // (with the value deleted, the preselection fell back to
                // Shut down). An explicit 1 is correct either way.
                job->hibernateShow = true;
            } else if (state.hibernateEnabled) {
                job->prefix += g_text->hibernateAlreadyOffered;
            }
            // When the infrastructure is off, the action is not actually
            // offered yet - the enable result line reports it.
            if (!state.hibernateEnabled) {
                job->hibernateSystemwideEnable = true;
            } else if (state.hibernateShown == 1) {
                job->prefix += g_text->hibernateAlreadyEnabled;
            }
        } else {
            // OFF with a MISSING value deliberately prompts for UAC to
            // write an explicit 0 - consistent with the explicit-values
            // approach even where the action may already be hidden. The
            // hibernation infrastructure stays enabled either way.
            if (state.hibernateShown != 0) {
                job->hibernateHide = true;
            } else {
                job->prefix += g_text->hibernateAlreadyHidden;
                if (defaultIsHibernate) {
                    job->prefix += g_text->hibernateDefaultFallbackNote;
                }
            }
        }
    }

    if (!(job->sleepShow || job->sleepHide || job->hibernateShow ||
          job->hibernateHide || job->hibernateSystemwideEnable) &&
        job->prefix.empty()) {
        Wh_Log(L"Availability apply: nothing to do");
        // Handled (the popup would say nothing new): record the flip.
        Wh_SetIntValue(sleepSeenName.c_str(), offerSleep ? 1 : 0);
        Wh_SetIntValue(hibernateSeenName.c_str(), offerHibernate ? 1 : 0);
        delete job;
        return;
    }

    // Stale-apply watchdog: if a worker has been "running" for more than
    // 5 minutes it is almost certainly stuck inside ShellExecuteExW (UAC
    // decision that never displayed). Override it - the stuck worker stays
    // pinned and safe, it just must not block the feature forever.
    ULONG nowTick = GetTickCount();
    if (InterlockedCompareExchange(&g_applyRunning, 0, 0) != 0 &&
        nowTick - g_applyStartTick > 300000) {
        Wh_Log(L"Stale availability apply detected (>5 min), overriding");
        InterlockedExchange(&g_applyRunning, 0);
    }

    if (InterlockedCompareExchange(&g_applyRunning, 1, 0) != 0) {
        // An apply is still running: the flip stays PENDING (the
        // bookkeeping is not written), so the next settings save retries
        // it automatically. No popup - the running apply reports its own
        // result first - no cross-session restore writes,
        // they would poison the machine-wide bookkeeping with a stale
        // read).
        Wh_Log(L"Availability apply already running, flip stays pending");
        delete job;
        return;
    }
    g_applyStartTick = nowTick;

    // Pin this module for the worker's lifetime: the worker can outlive
    // the grace wait in Wh_ModBeforeUninit (UAC prompt and popup are
    // unbounded), so the module must stay mapped; the worker ends with
    // FreeLibraryAndExitThread.
    HMODULE self = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                            (LPCWSTR)&ApplyAvailabilityThreadProc, &self)) {
        Wh_Log(L"GetModuleHandleExW failed (%lu)", GetLastError());
        // Flip stays pending (bookkeeping not written) - retried on the
        // next save.
        delete job;
        InterlockedExchange(&g_applyRunning, 0);
        return;
    }
    job->pin = self;
    // Consent owner for the UAC prompt: the Windhawk window the user just
    // saved in (captured while this thread still runs in the save context).
    job->consentOwner = GetForegroundWindow();
    job->generation = InterlockedIncrement(&g_applyGeneration);

    DWORD threadId = 0;
    HANDLE thread = CreateThread(nullptr, 0, ApplyAvailabilityThreadProc,
                                 job, 0, &threadId);
    if (thread) {
        // The handle is kept so the unload can wait on the thread itself;
        // the PREVIOUS handle is closed here (else every apply leaks one
        // thread kernel object).
        if (g_applyThread) {
            CloseHandle(g_applyThread);
        }
        g_applyThread = thread;
        g_applyThreadId = threadId;
        // Flip bookkeeping is written only now: the flip is dispatched and
        // will be reported.
        Wh_SetIntValue(sleepSeenName.c_str(), offerSleep ? 1 : 0);
        Wh_SetIntValue(hibernateSeenName.c_str(), offerHibernate ? 1 : 0);
    } else {
        Wh_Log(L"CreateThread failed (%lu)", GetLastError());
        delete job;
        FreeLibrary(self);
        InterlockedExchange(&g_applyRunning, 0);
        // Flip stays pending (bookkeeping not written) - retried on the
        // next save.
    }
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

// Finds the availability popup (message box) of the apply worker thread and
// closes it, so the worker finishes quickly when the mod unloads. A pure
// MB_OK box treats close as OK.
static BOOL CALLBACK CloseApplyPopupEnumProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (tid == g_applyThreadId && pid == GetCurrentProcessId()) {
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

    // Unconditional join on the dialog hook thread, per the Windhawk
    // unload contract ("Global objects and process shutdown" wiki: all
    // mod threads must be joined before Wh_ModUninit returns). The
    // WM_CLOSE post runs INSIDE the loop: a dialog created after the
    // first enumeration (open-race, nested message boxes) would
    // otherwise leave this join waiting until a user closes it by hand.
    // Re-posting WM_CLOSE to a gone or closing window is harmless.
    ULONGLONG startTick = GetTickCount64();
    ULONGLONG lastPost = 0;
    bool warnedLongWait = false;
    while (InterlockedCompareExchange(&g_inFlight, 0, 0) > 0) {
        ULONGLONG now = GetTickCount64();
        if (now - lastPost >= 250) {
            EnumWindows(CloseDialogEnumProc, 0);
            lastPost = now;
        }
        if (!warnedLongWait && now - startTick > 10000) {
            Wh_Log(L"Still waiting for the Alt+F4 dialog to close");
            warnedLongWait = true;
        }
        if (g_dialogDoneEvent) {
            ResetEvent(g_dialogDoneEvent);
            WaitForSingleObject(g_dialogDoneEvent, 100);
        } else {
            Sleep(10);
        }
    }

    // Worker: the by-the-book variant would be an unconditional join too,
    // but the UAC consent cannot be cancelled by an unelevated process -
    // an unconditional wait could block this engine thread indefinitely.
    // EXPLICIT DEVIATION from the unload contract (also stated in the PR
    // text): the popup is closed below, the elevated process wait is
    // bounded at 60 s, and the module is PINNED for the worker's lifetime
    // (FreeLibraryAndExitThread) - so a worker that outlives this short
    // grace wait cannot crash explorer when the module is unloaded while
    // it waits.
    if (InterlockedCompareExchange(&g_applyRunning, 0, 0) != 0) {
        Wh_Log(L"Availability apply in progress");
        if (g_applyThreadId) {
            EnumThreadWindows(g_applyThreadId, CloseApplyPopupEnumProc, 0);
        }
        if (g_applyThread) {
            WaitForSingleObject(g_applyThread, 3000);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    if (g_dialogDoneEvent) {
        CloseHandle(g_dialogDoneEvent);
        g_dialogDoneEvent = nullptr;
    }
    if (g_applyThread) {
        CloseHandle(g_applyThread);
        g_applyThread = nullptr;
    }
}