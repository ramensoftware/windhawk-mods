// ==WindhawkMod==
// @id              nvidia-keep-instant-replay-on
// @name            Keep Nvidia Instant Replay on
// @description     Turns Nvidia ShadowPlay's Instant Replay back on whenever something switches it off
// @version         1.0
// @author          Shahrukh
// @github          https://github.com/dixxi1208
// @homepage        https://github.com/dixxi1208/NvidiaInstantReplayFix
// @include         nvcontainer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Keep Nvidia Instant Replay on

Instant Replay has a habit of quietly switching itself off - after a driver update, a crash,
a stray hotkey, or a game that toggles it - and you only find out when you needed the clip
you didn't get.

This mod watches the Instant Replay state and switches it straight back on. It's the job
[AlwaysShadow](https://github.com/Verpous/AlwaysShadow) does, except it runs inside
NvContainer, so there's no extra program to install, no tray icon, and nothing to remember
to start.

It watches the registry key Nvidia stores the state in, so it reacts the moment the state
changes rather than waiting for the next poll. And if it finds itself fighting with something
that keeps switching Instant Replay back off, it backs off instead of flip-flopping forever.

## Requirements

The In-Game Overlay must be enabled, and you need a "Toggle Instant Replay" keyboard shortcut
configured - that shortcut is how this mod switches Instant Replay.

**Change that shortcut away from the Alt+Shift+F10 default.** Alt+Shift is also the Windows
shortcut for cycling the keyboard layout, so leaving it at the default means every automatic
re-enable may also change your input language. Ctrl+Shift+F10 works well. The new shortcut is
picked up automatically, no restart needed.

## Notes

Works with the Nvidia App and with the old GeForce Experience. AlwaysShadow prefers a local
HTTP server hosted by NvContainer and only falls back to the shortcut; that server came from
GeForce Experience's NvNode backend, which the Nvidia App doesn't ship, so this mod uses the
shortcut only - it's the path that works on both.

Windhawk injects into every nvcontainer.exe, but only one of them does the work. The instances
running under other accounts can't read your settings and stay out of the way on their own.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- pollIntervalSec: 10
  $name: Check interval (seconds)
  $description: >-
    How often to re-check the Instant Replay state. The state is also re-checked immediately
    whenever Nvidia writes to the ShadowPlay registry key, so this is mostly a safety net.
- conflictBackoffSec: 800
  $name: Conflict backoff (seconds)
  $description: >-
    If Instant Replay keeps turning itself back off, something is fighting us. After 3
    attempts in a row, stop trying for this long instead of flip-flopping. Set to 0 to keep
    retrying regardless.
- pauseWhileRunning: [""]
  $name: Pause while these programs run
  $description: >-
    Case-insensitive substrings matched against the full path of every running process,
    e.g. netflix.exe. While any of them is running, Instant Replay is left alone.
    Leave the single empty entry to disable this.
- onlyWhileRunning: [""]
  $name: Only keep it on while these programs run
  $description: >-
    Same matching. If this list is not empty, Instant Replay is forced ON while at least one
    of these is running and forced OFF the rest of the time.
    Leave the single empty entry to disable this.
*/
// ==/WindhawkModSettings==

#include <tlhelp32.h>
#include <windhawk_api.h>

#include <algorithm>
#include <cwctype>
#include <mutex>
#include <string>
#include <vector>

// Nvidia exposes two things this mod needs, both under the same registry key:
//
//   - The Instant Replay state, in the value {1B1D3DAA-...}. The key can be watched with
//     RegNotifyChangeKeyValue, so we notice a change the moment it happens.
//   - The "Toggle Instant Replay" shortcut, as IRToggleHKeyCount plus one IRToggleHKey<n>
//     per key. Pressing it is how the state gets changed.
//
// Everything in the key is stored as 4 byte REG_BINARY rather than REG_DWORD, which is why
// the reads below accept both.

static PCWSTR kShadowPlayRegKey = L"SOFTWARE\\NVIDIA Corporation\\Global\\ShadowPlay\\NVSPCAPS";
static PCWSTR kInstantReplayRegValue = L"{1B1D3DAA-601D-49E5-8508-81736CA28C6D}";

// Windhawk injects into every nvcontainer.exe, but only one of them should be driving this.
// The name is session local on purpose: the state we read lives in the per user hive.
static PCWSTR kWatchdogMutexName = L"Local\\WindhawkKeepInstantReplayOn";

// How many cycles in a row may want a toggle before we decide we're fighting someone.
static const int kMinStreakForConflict = 3;
static_assert(kMinStreakForConflict >= 2, "At least 2 attempts (1 retry) are needed to identify a conflict.");

// Don't press the shortcut faster than this, no matter how many notifications arrive.
static const DWORD kMinToggleIntervalMs = 3000;

// Nvidia writes several values when the state changes, so let it settle before reading back.
static const DWORD kRegistryNotifyDebounceMs = 750;

struct ModSettings {
    int pollIntervalSec = 10;
    int conflictBackoffSec = 800;
    std::vector<std::wstring> pauseWhileRunning;
    std::vector<std::wstring> onlyWhileRunning;
};

static std::mutex g_settingsMutex;
static ModSettings g_settings;

static HANDLE g_watchdogThread = NULL;
static HANDLE g_stopEvent = NULL;         // manual reset, tells the watchdog to quit
static HANDLE g_wakeEvent = NULL;         // auto reset, tells the watchdog settings changed
static HANDLE g_watchdogMutex = NULL;     // held by whichever nvcontainer.exe drives the watchdog

enum class InstantReplayState {
    Off,
    On,
    Unknown,
};

#pragma region Settings

static void LoadStringListSetting(PCWSTR nameFormat, std::vector<std::wstring>* out) {
    out->clear();

    for (int i = 0;; i++) {
        PCWSTR value = Wh_GetStringSetting(nameFormat, i);
        bool isEmpty = !*value;
        if (!isEmpty) {
            out->emplace_back(value);
        }
        Wh_FreeStringSetting(value);

        // An empty entry marks the end of the list, same as every other Windhawk mod.
        if (isEmpty) {
            break;
        }
    }
}

static int Clamp(int value, int low, int high) {
    return value < low ? low : (value > high ? high : value);
}

static void LoadSettings() {
    ModSettings settings;
    settings.pollIntervalSec = Clamp(Wh_GetIntSetting(L"pollIntervalSec"), 1, 3600);
    settings.conflictBackoffSec = Clamp(Wh_GetIntSetting(L"conflictBackoffSec"), 0, 86400);
    LoadStringListSetting(L"pauseWhileRunning[%d]", &settings.pauseWhileRunning);
    LoadStringListSetting(L"onlyWhileRunning[%d]", &settings.onlyWhileRunning);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(settings);
}

static ModSettings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

#pragma endregion // Settings

#pragma region Reading the state

// Returns Unknown when the value can't be read, which is the normal case for the
// nvcontainer.exe instances that don't run as the logged in user - their HKCU is a different
// hive. Treating that as Unknown (rather than as "off") keeps those instances from acting.
static InstantReplayState GetInstantReplayState() {
    DWORD type = 0;
    DWORD isActive = 0;
    DWORD size = sizeof(isActive);
    LSTATUS ret = RegGetValueW(HKEY_CURRENT_USER, kShadowPlayRegKey, kInstantReplayRegValue,
                               RRF_RT_ANY, &type, &isActive, &size);

    if (ret != ERROR_SUCCESS) {
        return InstantReplayState::Unknown;
    }

    if ((type != REG_DWORD && type != REG_BINARY) || size != sizeof(DWORD)) {
        return InstantReplayState::Unknown;
    }

    return isActive ? InstantReplayState::On : InstantReplayState::Off;
}

#pragma endregion // Reading the state

#pragma region Changing the state

static void AddKeyInput(std::vector<INPUT>* inputs, WORD vkey, bool isDown) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vkey;
    input.ki.dwFlags = isDown ? 0 : KEYEVENTF_KEYUP;
    inputs->push_back(input);
}

// Note this *toggles*, it doesn't set a state, so only call it when the current state is known.
// The shortcut is read fresh every time, so changing it in the Nvidia App takes effect at once.
static bool ToggleInstantReplayWithHotkey() {
    std::vector<WORD> keys;

    DWORD keyCount = 0;
    DWORD size = sizeof(keyCount);
    LSTATUS ret = RegGetValueW(HKEY_CURRENT_USER, kShadowPlayRegKey, L"IRToggleHKeyCount",
                               RRF_RT_DWORD, NULL, &keyCount, &size);

    if (ret == ERROR_SUCCESS && keyCount > 0 && keyCount <= 8) {
        // Each key of the shortcut is stored in its own value: IRToggleHKey0, IRToggleHKey1, ...
        for (DWORD i = 0; i < keyCount; i++) {
            std::wstring valueName = L"IRToggleHKey" + std::to_wstring(i);

            DWORD vkey = 0;
            size = sizeof(vkey);
            if (RegGetValueW(HKEY_CURRENT_USER, kShadowPlayRegKey, valueName.c_str(), RRF_RT_DWORD,
                             NULL, &vkey, &size) != ERROR_SUCCESS) {
                Wh_Log(L"Couldn't read %s, can't press the toggle shortcut", valueName.c_str());
                return false;
            }

            keys.push_back((WORD)vkey);
        }
    } else {
        // Nvidia's default shortcut.
        Wh_Log(L"No toggle shortcut configured, assuming the default Alt+Shift+F10");
        keys = {VK_MENU, VK_SHIFT, VK_F10};
    }

    std::vector<INPUT> inputs;
    inputs.reserve(keys.size() * 2);

    for (size_t i = 0; i < keys.size(); i++) {
        AddKeyInput(&inputs, keys[i], true);
    }
    for (size_t i = keys.size(); i > 0; i--) {
        AddKeyInput(&inputs, keys[i - 1], false);
    }

    UINT sent = SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
    if (sent != inputs.size()) {
        Wh_Log(L"SendInput only sent %u of %zu inputs, error %u", sent, inputs.size(), GetLastError());
        return false;
    }

    return true;
}

#pragma endregion // Changing the state

#pragma region Process matching

static bool ContainsNoCase(const std::wstring& haystack, const std::wstring& needle) {
    if (needle.empty() || needle.size() > haystack.size()) {
        return false;
    }

    auto match = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(),
                             [](wchar_t a, wchar_t b) { return towlower(a) == towlower(b); });
    return match != haystack.end();
}

// AlwaysShadow matches against WMI command lines. Spinning up COM inside NvContainer to do the
// same would be rude, so this matches against the full image path instead, which covers the
// realistic cases (an exe name, or a path fragment) without touching COM.
static bool IsAnyProcessRunning(const std::vector<std::wstring>& patterns) {
    if (patterns.empty()) {
        return false;
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        Wh_Log(L"CreateToolhelp32Snapshot failed, error %u", GetLastError());
        return false;
    }

    bool found = false;
    PROCESSENTRY32W entry = {};
    entry.dwSize = sizeof(entry);

    if (Process32FirstW(snapshot, &entry)) {
        do {
            std::wstring candidate = entry.szExeFile;

            HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ProcessID);
            if (process) {
                WCHAR path[MAX_PATH];
                DWORD pathLen = ARRAYSIZE(path);
                if (QueryFullProcessImageNameW(process, 0, path, &pathLen)) {
                    candidate.assign(path, pathLen);
                }
                CloseHandle(process);
            }

            for (const std::wstring& pattern : patterns) {
                if (ContainsNoCase(candidate, pattern)) {
                    Wh_Log(L"Process match: '%s' matches '%s'", candidate.c_str(), pattern.c_str());
                    found = true;
                    break;
                }
            }
        } while (!found && Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return found;
}

#pragma endregion // Process matching

#pragma region Watchdog

static bool StopRequested(DWORD waitMs) {
    return WaitForSingleObject(g_stopEvent, waitMs) == WAIT_OBJECT_0;
}

// Whichever instance gets here first owns the job; if it exits, the mutex object goes away and
// another instance picks it up on its next cycle.
static bool ClaimWatchdogRole() {
    if (g_watchdogMutex) {
        return true;
    }

    HANDLE mutex = CreateMutexW(NULL, FALSE, kWatchdogMutexName);
    if (!mutex) {
        Wh_Log(L"CreateMutex for the watchdog role failed, error %u", GetLastError());
        return false;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(mutex);
        return false;
    }

    g_watchdogMutex = mutex;
    Wh_Log(L"This nvcontainer.exe instance is now the Instant Replay watchdog");
    return true;
}

static DWORD WINAPI InstantReplayWatchdogThread(LPVOID) {
    Wh_Log(L"Instant Replay watchdog started");

    // Waiting on a registry notification means we react the moment something turns Instant
    // Replay off, instead of up to a whole poll interval later.
    HKEY notifyKey = NULL;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kShadowPlayRegKey, 0, KEY_NOTIFY, &notifyKey) != ERROR_SUCCESS) {
        notifyKey = NULL;
    }

    HANDLE notifyEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    bool notifyArmed = false;

    int toggleStreak = 0;
    ULONGLONG conflictUntil = 0;
    ULONGLONG nextToggleAllowed = 0;
    bool loggedUnreadableState = false;

    for (;;) {
        ModSettings settings = GetSettings();

        if (notifyKey && notifyEvent && !notifyArmed) {
            ResetEvent(notifyEvent);
            notifyArmed = RegNotifyChangeKeyValue(notifyKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                                                  notifyEvent, TRUE) == ERROR_SUCCESS;
        }

        HANDLE waitHandles[3];
        DWORD waitCount = 0;
        waitHandles[waitCount++] = g_stopEvent;
        waitHandles[waitCount++] = g_wakeEvent;
        if (notifyArmed) {
            waitHandles[waitCount++] = notifyEvent;
        }

        DWORD waited = WaitForMultipleObjects(waitCount, waitHandles, FALSE,
                                              (DWORD)settings.pollIntervalSec * 1000);

        if (waited == WAIT_OBJECT_0) {
            break;
        }

        if (waited == WAIT_FAILED) {
            // Shouldn't happen, but don't spin on it if it does.
            Wh_Log(L"WaitForMultipleObjects failed, error %u", GetLastError());
            if (StopRequested(1000)) {
                break;
            }
            continue;
        }

        if (waited == WAIT_OBJECT_0 + 1) {
            // Settings changed. Give the user's new configuration a clean slate.
            settings = GetSettings();
            toggleStreak = 0;
            conflictUntil = 0;
        } else if (waited == WAIT_OBJECT_0 + 2) {
            notifyArmed = false;
            if (StopRequested(kRegistryNotifyDebounceMs)) {
                break;
            }
        }

        ULONGLONG now = GetTickCount64();
        if (now < conflictUntil) {
            continue;
        }

        InstantReplayState state = GetInstantReplayState();
        if (state == InstantReplayState::Unknown) {
            if (!loggedUnreadableState) {
                loggedUnreadableState = true;
                Wh_Log(L"Can't read the Instant Replay state from HKCU. Either ShadowPlay isn't "
                       L"set up for this user, or this nvcontainer.exe runs under another account. "
                       L"Staying out of the way.");
            }
            continue;
        }
        loggedUnreadableState = false;

        if (!ClaimWatchdogRole()) {
            continue;
        }

        bool isOn = state == InstantReplayState::On;
        bool shouldBeOn = settings.onlyWhileRunning.empty() ||
                          IsAnyProcessRunning(settings.onlyWhileRunning);

        if (isOn == shouldBeOn) {
            toggleStreak = 0;
            continue;
        }

        if (IsAnyProcessRunning(settings.pauseWhileRunning)) {
            toggleStreak = 0;
            continue;
        }

        // A burst of registry notifications shouldn't turn into a burst of keystrokes.
        if (now < nextToggleAllowed) {
            if (StopRequested((DWORD)(nextToggleAllowed - now))) {
                break;
            }
            now = GetTickCount64();
        }

        // If we keep having to fix it, something is fixing it right back. Yield for a while
        // rather than flip-flopping forever.
        if (++toggleStreak >= kMinStreakForConflict) {
            conflictUntil = now + (ULONGLONG)settings.conflictBackoffSec * 1000;
            toggleStreak = kMinStreakForConflict - 2;
            Wh_Log(L"Something keeps changing Instant Replay back. Backing off for %d seconds.",
                   settings.conflictBackoffSec);
            continue;
        }

        nextToggleAllowed = now + kMinToggleIntervalMs;
        Wh_Log(L"Instant Replay is %s but should be %s, pressing the toggle shortcut",
               isOn ? L"on" : L"off", shouldBeOn ? L"on" : L"off");

        // Success here only means the input was injected - if Nvidia ignores it the state won't
        // change, the next cycles will notice, and the conflict backoff above stops us hammering
        // it forever.
        if (!ToggleInstantReplayWithHotkey()) {
            Wh_Log(L"Failed to send the Instant Replay toggle shortcut");
        }
    }

    if (notifyEvent) {
        CloseHandle(notifyEvent);
    }
    if (notifyKey) {
        RegCloseKey(notifyKey);
    }

    Wh_Log(L"Instant Replay watchdog stopped");
    return 0;
}

#pragma endregion // Watchdog

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    return TRUE;
}

void Wh_ModAfterInit() {
    // Started here rather than in Wh_ModInit to keep init itself cheap.
    g_stopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_wakeEvent = CreateEventW(NULL, FALSE, FALSE, NULL);

    if (!g_stopEvent || !g_wakeEvent) {
        Wh_Log(L"Failed to create the watchdog events, error %u", GetLastError());
        return;
    }

    g_watchdogThread = CreateThread(NULL, 0, InstantReplayWatchdogThread, NULL, 0, NULL);
    if (!g_watchdogThread) {
        Wh_Log(L"Failed to start the watchdog thread, error %u", GetLastError());
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();

    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

void Wh_ModBeforeUninit() {
    // The thread must be gone before this DLL is unloaded.
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_watchdogThread) {
        WaitForSingleObject(g_watchdogThread, INFINITE);
        CloseHandle(g_watchdogThread);
        g_watchdogThread = NULL;
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_watchdogMutex) {
        CloseHandle(g_watchdogMutex);
        g_watchdogMutex = NULL;
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = NULL;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = NULL;
    }
}
