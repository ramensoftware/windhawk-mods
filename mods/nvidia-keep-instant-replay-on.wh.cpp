// ==WindhawkMod==
// @id              nvidia-keep-instant-replay-on
// @name            Keep Nvidia Instant Replay on
// @description     Turns Nvidia ShadowPlay's Instant Replay back on whenever something switches it off
// @version         1.0
// @author          Shahrukh
// @github          https://github.com/dixxi1208
// @homepage        https://github.com/dixxi1208/NvidiaInstantReplayFix
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lshell32 -luser32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Keep Nvidia Instant Replay on

Instant Replay has a habit of quietly switching itself off - after a driver update, a crash,
a stray hotkey, or a game that toggles it - and you only find out when you needed the clip
you didn't get.

This mod watches the Instant Replay state and switches it straight back on. It's the job
[AlwaysShadow](https://github.com/Verpous/AlwaysShadow) does, except it runs as a Windhawk
mod, so there's no extra program to install, no tray icon, and nothing to remember to start.

It watches the registry key Nvidia stores the state in, so it reacts the moment the state
changes rather than waiting for the next poll. And if it finds itself fighting with something
that keeps switching Instant Replay back off, it backs off and eventually stops, rather than
trading keystrokes with it forever.

This is a tool mod: it hooks nothing and injects into nothing, it just runs in its own
dedicated process as the logged-in user.

## How is this different from "Shadowplay anti-disable"?

They solve opposite halves of the same annoyance and work well together:

- **Shadowplay anti-disable** stops ShadowPlay from switching *itself* off in the first place,
  for one specific cause - the driver refusing to record when it thinks DRM content or a
  capture-excluded window is on screen. Use it if your recording stops when you open Netflix
  or certain apps.
- **This mod** doesn't care *why* Instant Replay went off. It notices that it did, and turns
  it back on. Use it if you keep discovering Instant Replay was silently off.

Neither interferes with the other. That mod patches the driver inside NvContainer and never
changes the Instant Replay setting; this one runs in its own process, only reads the setting,
and presses your toggle shortcut.

## Requirements

The In-Game Overlay must be enabled, and you must have a "Toggle Instant Replay" keyboard
shortcut configured - that shortcut is how this mod switches Instant Replay, and the mod does
nothing at all if one isn't set.

**Set that shortcut to something without Alt+Shift in it.** Alt+Shift is also the Windows
shortcut for cycling the keyboard layout, so a shortcut containing it means every automatic
re-enable may also change your input language. Ctrl+Shift+F10 works well. The shortcut is
re-read every time, so changing it takes effect immediately.

Because the shortcut is replayed as real keystrokes, two things follow. Whatever window has
focus also receives the combination. And the key-ups the mod sends would release those keys if
you were physically holding one - so the mod waits for a cycle when none of the shortcut's keys
and no modifier is held down, and a shortcut built from keys you hold during normal use is
still a poor choice. This is inherent to the approach: the Nvidia App no longer ships the local
server GeForce Experience used to expose, so the shortcut is the only way left to switch
Instant Replay from outside.

Because those keystrokes are the mod's main cost to you, it also caps how often it is willing
to press at all. If something keeps switching Instant Replay off - even slowly, every few
minutes - the mod backs off progressively and stops after about an hour of that, rather than
trading keystrokes with it indefinitely. It starts again when the state settles, when you
change the toggle shortcut, or after a few hours.

## Notes

Works with the Nvidia App and with the old GeForce Experience.

Instant Replay is a persistent Nvidia setting, so whatever state the mod leaves it in stays
that way after the mod is disabled. In particular, if you use **Only keep it on while these
programs run** and then disable the mod at a moment when none of those programs are running,
Instant Replay is left off - the mod turned it off on purpose and isn't around any more to
turn it back on.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- pollIntervalSec: 10
  $name: Check interval (seconds)
  $description: >-
    How often to re-check the Instant Replay state, from 5 to 3600. The state is also
    re-checked immediately whenever Nvidia writes to the ShadowPlay registry key, so this is
    mostly a safety net.
- conflictBackoffSec: 900
  $name: Maximum conflict backoff (seconds)
  $description: >-
    If Instant Replay keeps turning itself back off, something is fighting us. Rather than
    trading keystrokes with it, the mod waits before trying again - 30 seconds at first,
    doubling each round up to this limit (from 30 to 86400). It resets once the state has been
    left alone for a while. After about an hour of continuous conflict the mod stops trying,
    and starts again when the state settles, when the toggle shortcut changes, or after a few
    hours.
- pauseWhileRunning: [""]
  $name: Pause while these programs run
  $description: >-
    While any of these is running, Instant Replay is left alone. An entry without a backslash
    matches a process by exact file name, e.g. netflix.exe. An entry containing a backslash is
    matched as a substring of the full image path, e.g. \Netflix\ - useful for covering a whole
    install folder, but it can only match processes this mod is allowed to open, so prefer the
    file name form unless you need the path. Matching ignores case either way.
    Leave the single empty entry to disable this.
- onlyWhileRunning: [""]
  $name: Only keep it on while these programs run
  $description: >-
    Same matching. If this list is not empty, Instant Replay is forced ON while at least one of
    these is running and forced OFF the rest of the time.
    Leave the single empty entry to disable this.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <shellapi.h>
#include <tlhelp32.h>

#include <algorithm>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Nvidia exposes two things this mod needs, both under the same registry key:
//
//   - The Instant Replay state, in the value {1B1D3DAA-...}. The key can be watched with
//     RegNotifyChangeKeyValue, so we notice a change the moment it happens.
//   - The "Toggle Instant Replay" shortcut, as IRToggleHKeyCount plus one IRToggleHKey<n>
//     per key. Pressing it is how the state gets changed.
//
// Everything in the key is stored as 4 byte REG_BINARY rather than REG_DWORD, which RRF_RT_DWORD
// accepts alongside REG_DWORD, so all the reads below use it.

static PCWSTR kShadowPlayRegKey = L"SOFTWARE\\NVIDIA Corporation\\Global\\ShadowPlay\\NVSPCAPS";
static PCWSTR kInstantReplayRegValue = L"{1B1D3DAA-601D-49E5-8508-81736CA28C6D}";

// How many toggles in a row may fail to stick before we conclude we're fighting someone.
static const int kMaxTogglesBeforeBackoff = 2;

// Coming out of a backoff, allow one more toggle before concluding we're still in conflict,
// so a conflict that has since resolved recovers on the very next attempt.
static const int kStreakAfterBackoff = kMaxTogglesBeforeBackoff - 1;

// A conflict slower than kToggleConfirmTimeoutMs confirms fine on every attempt, so the
// consecutive-failure streak never sees it - something switching Instant Replay off every
// minute would otherwise be answered forever. Bound how often we're willing to press at all.
static const int kMaxTogglesPerWindow = 5;
static const ULONGLONG kToggleWindowMs = 10ULL * 60 * 1000;

// Where the backoff starts before doubling towards the configured limit. Short enough that a
// one-off failure costs almost nothing, and it only grows if the fight is real.
static const int kInitialBackoffSec = 30;

// Stop pressing once a conflict has gone on this long. Measured as elapsed time rather than
// rounds spent at the maximum backoff, so lowering the maximum makes the mod retry more often
// - which is what the setting's name implies - instead of making it give up sooner.
static const ULONGLONG kGiveUpAfterConflictMs = 60ULL * 60 * 1000;

// Having given up, try again this long afterwards. The user may have fixed the cause in a way
// the mod cannot observe - switching the In-Game Overlay back on doesn't touch the registry
// key, so it produces no notification to wake us.
static const ULONGLONG kGiveUpRetryMs = 6ULL * 60 * 60 * 1000;

// How long to let Nvidia catch up after pressing the shortcut. Starting the Instant Replay
// ring buffer isn't instant, and the state value can still read stale for a moment after a
// notification fires for one of the other values in the key.
static const DWORD kToggleConfirmTimeoutMs = 8000;
static const DWORD kToggleConfirmPollMs = 500;

// Don't press the shortcut faster than this, no matter how many notifications arrive.
static const DWORD kMinToggleIntervalMs = 3000;

// Nvidia writes several values when the state changes, so let it settle before reading back.
static const DWORD kRegistryNotifyDebounceMs = 750;

// Short-lived obstacles - a UAC prompt, a held modifier, a failed snapshot - shouldn't cost a
// whole poll interval to recover from, which the user may have set high deliberately.
static const DWORD kTransientRetryMs = 2000;

struct ModSettings {
    int pollIntervalSec = 10;
    int conflictBackoffSec = 900;
    std::vector<std::wstring> pauseWhileRunning;
    std::vector<std::wstring> onlyWhileRunning;
};

// WhTool_ModSettingsChanged runs on Windhawk's thread while the watchdog thread is reading,
// so the settings are swapped under a lock and the watchdog takes a copy per cycle.
static std::mutex g_settingsMutex;
static ModSettings g_settings;

static HANDLE g_watchdogThread = nullptr;
static HANDLE g_stopEvent = nullptr;  // manual reset, tells the watchdog to quit
static HANDLE g_wakeEvent = nullptr;  // auto reset, tells the watchdog settings changed

enum class InstantReplayState {
    Off,
    On,
    Unknown,
};

#pragma region Settings

static void LoadStringListSetting(PCWSTR nameFormat, std::vector<std::wstring>* out) {
    out->clear();

    for (int i = 0;; i++) {
        auto value = WindhawkUtils::StringSetting::make(nameFormat, i);

        // An empty entry marks the end of the list, same as every other Windhawk mod.
        if (!*value.get()) {
            break;
        }

        out->emplace_back(value.get());
    }
}

static void LoadSettings() {
    ModSettings settings;

    // A one second poll would mean a full process enumeration every second once either list is
    // configured, which isn't worth it when registry notifications already carry the real work.
    settings.pollIntervalSec = std::clamp(Wh_GetIntSetting(L"pollIntervalSec"), 5, 3600);

    // No "never back off" option: the shortcut can fail in ways the mod cannot detect, and an
    // unbounded retry would inject keystrokes into the user's session indefinitely.
    settings.conflictBackoffSec =
        std::clamp(Wh_GetIntSetting(L"conflictBackoffSec"), kInitialBackoffSec, 86400);

    LoadStringListSetting(L"pauseWhileRunning[%d]", &settings.pauseWhileRunning);
    LoadStringListSetting(L"onlyWhileRunning[%d]", &settings.onlyWhileRunning);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(settings);
}

static ModSettings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

#pragma endregion  // Settings

#pragma region Reading the state

// Returns Unknown when the value can't be read, which is what happens when ShadowPlay isn't
// installed or has never been configured. Treating that as Unknown rather than as "off" keeps
// the mod from pressing the shortcut on a machine that has nothing to toggle.
static InstantReplayState GetInstantReplayState() {
    DWORD isActive = 0;
    DWORD size = sizeof(isActive);
    LSTATUS ret = RegGetValueW(HKEY_CURRENT_USER, kShadowPlayRegKey, kInstantReplayRegValue,
                               RRF_RT_DWORD, nullptr, &isActive, &size);

    if (ret != ERROR_SUCCESS) {
        return InstantReplayState::Unknown;
    }

    return isActive ? InstantReplayState::On : InstantReplayState::Off;
}

#pragma endregion  // Reading the state

#pragma region Changing the state

// Reads IRToggleHKeyCount + IRToggleHKey<n>. Empty when no usable shortcut is configured.
//
// Nvidia's Alt+Shift+F10 default is deliberately not assumed here: the case where these values
// are missing is the case with no evidence a shortcut exists at all, and guessing would mean
// repeatedly injecting the combination that cycles the keyboard layout.
static std::vector<WORD> ReadToggleHotkey() {
    DWORD keyCount = 0;
    DWORD size = sizeof(keyCount);
    LSTATUS ret = RegGetValueW(HKEY_CURRENT_USER, kShadowPlayRegKey, L"IRToggleHKeyCount",
                               RRF_RT_DWORD, nullptr, &keyCount, &size);

    if (ret != ERROR_SUCCESS || keyCount == 0 || keyCount > 8) {
        return {};
    }

    std::vector<WORD> keys;
    keys.reserve(keyCount);

    // Each key of the shortcut is stored in its own value: IRToggleHKey0, IRToggleHKey1, ...
    for (DWORD i = 0; i < keyCount; i++) {
        std::wstring valueName = L"IRToggleHKey" + std::to_wstring(i);

        DWORD vkey = 0;
        size = sizeof(vkey);
        if (RegGetValueW(HKEY_CURRENT_USER, kShadowPlayRegKey, valueName.c_str(), RRF_RT_DWORD,
                         nullptr, &vkey, &size) != ERROR_SUCCESS) {
            return {};
        }

        // Don't feed anything that isn't a plain virtual-key code to SendInput.
        if (vkey == 0 || vkey > 0xFE) {
            return {};
        }

        keys.push_back((WORD)vkey);
    }

    return keys;
}

// The keys that live on the extended part of the keyboard need KEYEVENTF_EXTENDEDKEY, or the
// injected event describes the wrong physical key.
static bool IsExtendedKey(WORD vkey) {
    switch (vkey) {
        case VK_RCONTROL:
        case VK_RMENU:
        case VK_INSERT:
        case VK_DELETE:
        case VK_HOME:
        case VK_END:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_LEFT:
        case VK_UP:
        case VK_RIGHT:
        case VK_DOWN:
        case VK_NUMLOCK:
        case VK_DIVIDE:
        case VK_SNAPSHOT:
        case VK_LWIN:
        case VK_RWIN:
        case VK_APPS:
            return true;
        default:
            return false;
    }
}

static void AddKeyInput(std::vector<INPUT>* inputs, WORD vkey, bool isDown) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vkey;
    input.ki.wScan = (WORD)MapVirtualKeyW(vkey, MAPVK_VK_TO_VSC);
    input.ki.dwFlags = (isDown ? 0 : KEYEVENTF_KEYUP) |
                       (IsExtendedKey(vkey) ? KEYEVENTF_EXTENDEDKEY : 0);
    inputs->push_back(input);
}

// Note this *toggles*, it doesn't set a state, so only call it when the current state is known.
static bool PressToggleHotkey(const std::vector<WORD>& keys) {
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
        Wh_Log(L"SendInput only sent %u of %zu inputs, error %u", sent, inputs.size(),
               GetLastError());
        return false;
    }

    return true;
}

// If the user is physically holding any of these, the combination Nvidia sees isn't the one
// that's configured, so the toggle would fail and be counted as a conflict - and the key-ups
// we send would steal the keys out from under them.
static bool IsAnyRelevantKeyHeld(const std::vector<WORD>& keys) {
    static const WORD kModifiers[] = {VK_SHIFT, VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN};

    for (WORD vkey : keys) {
        if (GetAsyncKeyState(vkey) & 0x8000) {
            return true;
        }
    }

    for (WORD vkey : kModifiers) {
        if (GetAsyncKeyState(vkey) & 0x8000) {
            return true;
        }
    }

    return false;
}

// A locked session or a secure desktop (UAC) means injected input can't reach anything. That's
// not the mod losing a fight, so it mustn't be allowed to drive the backoff.
static bool IsInputDesktopReachable() {
    HDESK inputDesktop = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
    if (!inputDesktop) {
        return false;
    }

    CloseDesktop(inputDesktop);
    return true;
}

#pragma endregion  // Changing the state

#pragma region Process matching

// CompareStringOrdinal / FindNLSStringEx rather than towlower, which only folds ASCII under the
// default locale and would quietly fail to match paths containing non-ASCII characters.
static bool EqualsNoCase(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) {
        return false;
    }

    if (a.empty()) {
        return true;
    }

    return CompareStringOrdinal(a.data(), (int)a.size(), b.data(), (int)b.size(), TRUE) ==
           CSTR_EQUAL;
}

static bool ContainsNoCase(std::wstring_view haystack, std::wstring_view needle) {
    if (needle.empty() || needle.size() > haystack.size()) {
        return false;
    }

    return FindNLSStringEx(LOCALE_NAME_INVARIANT, FIND_FROMSTART | NORM_IGNORECASE,
                           haystack.data(), (int)haystack.size(), needle.data(),
                           (int)needle.size(), nullptr, nullptr, nullptr, 0) >= 0;
}

static bool PatternNeedsPath(const std::wstring& pattern) {
    return pattern.find(L'\\') != std::wstring::npos;
}

// A pattern without a backslash is matched against the process file name exactly, so
// "notepad" doesn't quietly match ...\notepad++\notepad++.exe. A pattern with a backslash is
// matched as a substring of the full image path, which covers "everything in this folder".
static bool MatchesAnyPattern(std::wstring_view imagePath,
                              std::wstring_view fileName,
                              const std::vector<std::wstring>& patterns) {
    for (const std::wstring& pattern : patterns) {
        bool matched = PatternNeedsPath(pattern) ? ContainsNoCase(imagePath, pattern)
                                                 : EqualsNoCase(fileName, pattern);
        if (matched) {
            Wh_Log(L"Process match: '%s' matches '%s'", std::wstring(imagePath).c_str(),
                   pattern.c_str());
            return true;
        }
    }

    return false;
}

struct ProcessListResults {
    bool onlyRunning = false;
    bool pauseRunning = false;
};

// Answers both lists from a single snapshot. Returns nullopt when the process list couldn't be
// read at all, which has to stay distinguishable from "nothing matched": with a non-empty
// onlyWhileRunning list, treating a failed enumeration as "nothing matched" would make the mod
// switch Instant Replay *off* while a game is running, which is the exact thing it exists to
// prevent. CreateToolhelp32Snapshot is documented to fail transiently with ERROR_BAD_LENGTH.
static std::optional<ProcessListResults> EvaluateProcessLists(
    const std::vector<std::wstring>& onlyList,
    const std::vector<std::wstring>& pauseList) {
    ProcessListResults results;

    const bool needOnly = !onlyList.empty();
    const bool needPause = !pauseList.empty();
    if (!needOnly && !needPause) {
        return results;
    }

    // Resolving full paths costs an OpenProcess for every process on the system. Only pay it
    // when a pattern actually asks for a path - entry.szExeFile is already the file name that
    // the exact-match form compares against.
    const bool needsPath = std::any_of(onlyList.begin(), onlyList.end(), PatternNeedsPath) ||
                           std::any_of(pauseList.begin(), pauseList.end(), PatternNeedsPath);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        Wh_Log(L"CreateToolhelp32Snapshot failed, error %u", GetLastError());
        return std::nullopt;
    }

    PROCESSENTRY32W entry = {};
    entry.dwSize = sizeof(entry);

    if (!Process32FirstW(snapshot, &entry)) {
        Wh_Log(L"Process32FirstW failed, error %u", GetLastError());
        CloseHandle(snapshot);
        return std::nullopt;
    }

    do {
        std::wstring imagePath = entry.szExeFile;

        if (needsPath) {
            HANDLE process =
                OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ProcessID);
            if (process) {
                // Generous enough that a long path doesn't silently fall back to the bare file
                // name, which a \folder\ pattern then couldn't match.
                WCHAR path[1024];
                DWORD pathLen = ARRAYSIZE(path);
                if (QueryFullProcessImageNameW(process, 0, path, &pathLen)) {
                    imagePath.assign(path, pathLen);
                }
                CloseHandle(process);
            }
        }

        std::wstring_view fileName = imagePath;
        size_t lastSeparator = fileName.find_last_of(L'\\');
        if (lastSeparator != std::wstring_view::npos) {
            fileName = fileName.substr(lastSeparator + 1);
        }

        if (needOnly && !results.onlyRunning) {
            results.onlyRunning = MatchesAnyPattern(imagePath, fileName, onlyList);
        }
        if (needPause && !results.pauseRunning) {
            results.pauseRunning = MatchesAnyPattern(imagePath, fileName, pauseList);
        }

        if ((!needOnly || results.onlyRunning) && (!needPause || results.pauseRunning)) {
            break;
        }
    } while (Process32NextW(snapshot, &entry));

    CloseHandle(snapshot);
    return results;
}

#pragma endregion  // Process matching

#pragma region Watchdog

static bool StopRequested(DWORD waitMs) {
    return WaitForSingleObject(g_stopEvent, waitMs) == WAIT_OBJECT_0;
}

// Returns true if the state reached `expected` before the timeout. `*stop` is set when the mod
// is shutting down, in which case the caller must break out of its loop.
//
// Without this the mod would go straight back to waiting after pressing the shortcut, read a
// state value that hasn't caught up yet, and press again - undoing its own toggle and then
// counting the result as a conflict.
static bool WaitForState(InstantReplayState expected, DWORD timeoutMs, bool* stop) {
    ULONGLONG deadline = GetTickCount64() + timeoutMs;

    for (;;) {
        if (GetInstantReplayState() == expected) {
            return true;
        }

        ULONGLONG now = GetTickCount64();
        if (now >= deadline) {
            return false;
        }

        if (StopRequested((DWORD)std::min<ULONGLONG>(kToggleConfirmPollMs, deadline - now))) {
            *stop = true;
            return false;
        }
    }
}

static DWORD WINAPI InstantReplayWatchdogThread(LPVOID) {
    Wh_Log(L"Instant Replay watchdog started");

    // Waiting on a registry notification means we react the moment something turns Instant
    // Replay off, instead of up to a whole poll interval later.
    HKEY notifyKey = nullptr;
    HANDLE notifyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    bool notifyArmed = false;
    bool loggedNotifyUnavailable = false;

    if (!notifyEvent) {
        Wh_Log(L"CreateEvent for the registry notification failed, error %u. Falling back to "
               L"polling only.",
               GetLastError());
    }

    int toggleStreak = 0;
    int currentBackoffSec = 0;
    int togglesInWindow = 0;
    ULONGLONG toggleWindowStart = 0;
    ULONGLONG conflictSince = 0;
    bool gaveUp = false;
    std::vector<WORD> hotkeyAtGiveUp;
    ULONGLONG giveUpRetryAt = 0;
    ULONGLONG conflictUntil = 0;
    ULONGLONG nextToggleAllowed = 0;
    bool loggedUnreadableState = false;
    bool loggedEnumerationFailure = false;
    bool loggedMissingHotkey = false;

    for (;;) {
        ModSettings settings = GetSettings();

        // Opened here rather than once before the loop so that configuring ShadowPlay after the
        // mod is already running still gets the immediate-reaction behaviour, instead of
        // silently degrading to polling for the rest of the session.
        if (notifyEvent && !notifyKey) {
            LSTATUS openRet =
                RegOpenKeyExW(HKEY_CURRENT_USER, kShadowPlayRegKey, 0, KEY_NOTIFY, &notifyKey);
            if (openRet != ERROR_SUCCESS) {
                notifyKey = nullptr;
                if (!loggedNotifyUnavailable) {
                    loggedNotifyUnavailable = true;
                    Wh_Log(L"Can't open the ShadowPlay key to watch it (error %d), polling "
                           L"every %d seconds until it appears",
                           openRet, settings.pollIntervalSec);
                }
            } else {
                loggedNotifyUnavailable = false;
            }
        }

        if (notifyKey && notifyEvent && !notifyArmed) {
            ResetEvent(notifyEvent);
            notifyArmed = RegNotifyChangeKeyValue(notifyKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                                                  notifyEvent, TRUE) == ERROR_SUCCESS;
            if (!notifyArmed) {
                // The key may have been deleted from under us; reopen it next time round.
                RegCloseKey(notifyKey);
                notifyKey = nullptr;
            }
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

        ULONGLONG now = GetTickCount64();

        // The toggle budget is per rolling window and is deliberately *not* cleared when the
        // state matches - that branch is reached after every successful round, which is exactly
        // what would hide a slow conflict from the ladder.
        if (now - toggleWindowStart >= kToggleWindowMs) {
            toggleWindowStart = now;
            togglesInWindow = 0;
        }

        if (waited == WAIT_OBJECT_0 + 1) {
            // Settings changed. Give the new configuration a clean slate.
            toggleStreak = 0;
            currentBackoffSec = 0;
            conflictSince = 0;
            conflictUntil = 0;
            togglesInWindow = 0;
            toggleWindowStart = now;
            gaveUp = false;
        } else if (waited == WAIT_OBJECT_0 + 2) {
            notifyArmed = false;
            if (StopRequested(kRegistryNotifyDebounceMs)) {
                break;
            }
            now = GetTickCount64();
        }

        InstantReplayState state = GetInstantReplayState();
        if (state == InstantReplayState::Unknown) {
            if (!loggedUnreadableState) {
                loggedUnreadableState = true;
                Wh_Log(L"Can't read the Instant Replay state. ShadowPlay is probably not set up "
                       L"on this machine. Staying out of the way.");
            }
            continue;
        }
        loggedUnreadableState = false;

        std::optional<ProcessListResults> processes =
            EvaluateProcessLists(settings.onlyWhileRunning, settings.pauseWhileRunning);
        if (!processes) {
            if (!loggedEnumerationFailure) {
                loggedEnumerationFailure = true;
                Wh_Log(L"Couldn't enumerate processes, skipping this cycle rather than guessing "
                       L"at the state Instant Replay should be in");
            }
            if (StopRequested(kTransientRetryMs)) {
                break;
            }
            continue;
        }
        loggedEnumerationFailure = false;

        bool shouldBeOn = settings.onlyWhileRunning.empty() || processes->onlyRunning;
        bool isOn = state == InstantReplayState::On;

        // Whatever we were fighting with has stopped, or never existed. Reached even while
        // backing off or after giving up: neither state stops the mod noticing that things
        // resolved. The backoff ladder itself only clears once a whole window has passed
        // without any toggle being needed.
        if (isOn == shouldBeOn) {
            toggleStreak = 0;
            conflictUntil = 0;
            gaveUp = false;
            if (togglesInWindow == 0) {
                currentBackoffSec = 0;
                conflictSince = 0;
            }
            continue;
        }

        if (processes->pauseRunning) {
            toggleStreak = 0;
            continue;
        }

        // Nothing we send can land right now, and that isn't a failure on our part.
        if (!IsInputDesktopReachable()) {
            if (StopRequested(kTransientRetryMs)) {
                break;
            }
            continue;
        }

        std::vector<WORD> hotkey = ReadToggleHotkey();
        if (hotkey.empty()) {
            // Not a failed attempt - there was nothing to attempt - so this doesn't count
            // towards the backoff.
            if (!loggedMissingHotkey) {
                loggedMissingHotkey = true;
                Wh_Log(L"No usable Toggle Instant Replay shortcut is configured "
                       L"(IRToggleHKeyCount), so there's nothing to press. Set one in the "
                       L"Nvidia App.");
            }
            continue;
        }
        loggedMissingHotkey = false;

        // Pressing the shortcut failed persistently enough that we stopped. Re-arm when the
        // user acts on what the log asked for - the shortcut changing is observable, and a
        // periodic retry covers the causes that aren't, like the overlay being switched on.
        if (gaveUp) {
            bool hotkeyChanged = hotkey != hotkeyAtGiveUp;
            if (!hotkeyChanged && now < giveUpRetryAt) {
                continue;
            }

            Wh_Log(L"Trying again after stopping (%s)",
                   hotkeyChanged ? L"the toggle shortcut changed" : L"periodic retry");
            gaveUp = false;
            toggleStreak = 0;
            currentBackoffSec = 0;
            conflictSince = 0;
            conflictUntil = 0;
            togglesInWindow = 0;
            toggleWindowStart = now;
        }

        if (now < conflictUntil) {
            continue;
        }

        // Two ways to conclude we're in a fight: our toggles keep failing to stick, or they
        // keep sticking and something keeps undoing them slowly. Both feed the same ladder.
        bool overToggleBudget = togglesInWindow >= kMaxTogglesPerWindow;
        if (toggleStreak >= kMaxTogglesBeforeBackoff || overToggleBudget) {
            if (currentBackoffSec == 0) {
                conflictSince = now;
                currentBackoffSec = std::min(kInitialBackoffSec, settings.conflictBackoffSec);
            } else {
                currentBackoffSec =
                    std::min(currentBackoffSec * 2, settings.conflictBackoffSec);
            }
            conflictUntil = now + (ULONGLONG)currentBackoffSec * 1000;
            toggleStreak = kStreakAfterBackoff;

            if (now - conflictSince >= kGiveUpAfterConflictMs) {
                gaveUp = true;
                hotkeyAtGiveUp = hotkey;
                giveUpRetryAt = now + kGiveUpRetryMs;
                Wh_Log(L"Instant Replay has been in conflict for over an hour. Pausing until "
                       L"the state settles, the toggle shortcut changes, or a few hours pass. "
                       L"Check that the In-Game Overlay is on and that a Toggle Instant Replay "
                       L"shortcut is set.");
            } else {
                Wh_Log(L"Instant Replay keeps changing back (%s). Backing off for %d seconds.",
                       overToggleBudget ? L"toggling too often" : L"toggles aren't sticking",
                       currentBackoffSec);
            }
            continue;
        }

        // Pressing now would send the wrong combination and steal the keys being held.
        if (IsAnyRelevantKeyHeld(hotkey)) {
            if (StopRequested(kTransientRetryMs)) {
                break;
            }
            continue;
        }

        // A burst of registry notifications shouldn't turn into a burst of keystrokes.
        if (now < nextToggleAllowed) {
            if (StopRequested((DWORD)(nextToggleAllowed - now))) {
                break;
            }
            now = GetTickCount64();
        }

        nextToggleAllowed = now + kMinToggleIntervalMs;
        togglesInWindow++;
        Wh_Log(L"Instant Replay is %s but should be %s, pressing the toggle shortcut",
               isOn ? L"on" : L"off", shouldBeOn ? L"on" : L"off");

        if (!PressToggleHotkey(hotkey)) {
            toggleStreak++;
            continue;
        }

        // Wait for the press to actually take effect before deciding whether it worked. A slow
        // but successful toggle then costs nothing, and toggleStreak counts real failures only.
        // The backoff ladder is deliberately not cleared here - only a quiet window clears it.
        bool stop = false;
        InstantReplayState expected =
            shouldBeOn ? InstantReplayState::On : InstantReplayState::Off;
        if (WaitForState(expected, kToggleConfirmTimeoutMs, &stop)) {
            toggleStreak = 0;
        } else {
            if (stop) {
                break;
            }
            Wh_Log(L"Instant Replay didn't change within %u ms of the toggle shortcut",
                   kToggleConfirmTimeoutMs);
            toggleStreak++;
        }
    }

    // Close the key first, so the pending notification is unregistered before the event it
    // refers to goes away.
    if (notifyKey) {
        RegCloseKey(notifyKey);
    }
    if (notifyEvent) {
        CloseHandle(notifyEvent);
    }

    Wh_Log(L"Instant Replay watchdog stopped");
    return 0;
}

#pragma endregion  // Watchdog

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    // Created signalled so the first loop iteration evaluates the state immediately instead of
    // waiting out a poll interval, which the user may have set as high as an hour.
    g_wakeEvent = CreateEventW(nullptr, FALSE, TRUE, nullptr);

    if (!g_stopEvent || !g_wakeEvent) {
        Wh_Log(L"Failed to create the watchdog events, error %u", GetLastError());
        return FALSE;
    }

    g_watchdogThread =
        CreateThread(nullptr, 0, InstantReplayWatchdogThread, nullptr, 0, nullptr);
    if (!g_watchdogThread) {
        Wh_Log(L"Failed to start the watchdog thread, error %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed");

    LoadSettings();

    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_watchdogThread) {
        WaitForSingleObject(g_watchdogThread, INFINITE);
        CloseHandle(g_watchdogThread);
        g_watchdogThread = nullptr;
    }

    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
