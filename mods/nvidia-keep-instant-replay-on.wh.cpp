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
that keeps switching Instant Replay back off, it backs off instead of flip-flopping forever.

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

The In-Game Overlay must be enabled, and you need a "Toggle Instant Replay" keyboard shortcut
configured - that shortcut is how this mod switches Instant Replay.

**Change that shortcut away from the Alt+Shift+F10 default.** Alt+Shift is also the Windows
shortcut for cycling the keyboard layout, so leaving it at the default means every automatic
re-enable may also change your input language. Ctrl+Shift+F10 works well. The new shortcut is
picked up automatically, no restart needed.

Because the shortcut is replayed as real keystrokes, whatever window has focus also receives
the combination. That is inherent to this approach - the Nvidia App no longer ships the local
server that GeForce Experience used to expose, so the shortcut is the only way left to switch
Instant Replay from outside.

## Notes

Works with the Nvidia App and with the old GeForce Experience.

If you use **Only keep it on while these programs run** and then disable the mod at a moment
when none of those programs are running, Instant Replay is left off - the mod turned it off on
purpose and isn't around any more to turn it back on. That's the one case where switching the
mod off doesn't restore what you had before.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- pollIntervalSec: 10
  $name: Check interval (seconds)
  $description: >-
    How often to re-check the Instant Replay state. The state is also re-checked immediately
    whenever Nvidia writes to the ShadowPlay registry key, so this is mostly a safety net.
- conflictBackoffSec: 900
  $name: Maximum conflict backoff (seconds)
  $description: >-
    If Instant Replay keeps turning itself back off, something is fighting us. After 2
    attempts in a row that don't stick, wait before trying again instead of flip-flopping -
    30 seconds at first, doubling each round up to this limit, and reset as soon as Instant
    Replay is in the state it should be. Set to 0 to keep retrying and never back off.
- pauseWhileRunning: [""]
  $name: Pause while these programs run
  $description: >-
    While any of these is running, Instant Replay is left alone. An entry without a backslash
    matches a process by exact file name, e.g. netflix.exe. An entry containing a backslash is
    matched as a substring of the full image path, e.g. \Netflix\ - useful for covering a whole
    install folder. Matching ignores case either way.
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
// Everything in the key is stored as 4 byte REG_BINARY rather than REG_DWORD, which is why
// the reads below accept both.

static PCWSTR kShadowPlayRegKey = L"SOFTWARE\\NVIDIA Corporation\\Global\\ShadowPlay\\NVSPCAPS";
static PCWSTR kInstantReplayRegValue = L"{1B1D3DAA-601D-49E5-8508-81736CA28C6D}";

// How many toggles in a row may fail to stick before we conclude we're fighting someone.
static const int kMaxTogglesBeforeBackoff = 2;

// Coming out of a backoff, allow one more toggle before concluding we're still in conflict,
// so a conflict that has since resolved recovers on the very next attempt.
static const int kStreakAfterBackoff = kMaxTogglesBeforeBackoff - 1;

// Where the backoff starts before doubling towards the configured limit. Short enough that a
// one-off failure costs almost nothing, and it only grows if the fight is real.
static const int kInitialBackoffSec = 30;

// Don't press the shortcut faster than this, no matter how many notifications arrive.
static const DWORD kMinToggleIntervalMs = 3000;

// Nvidia writes several values when the state changes, so let it settle before reading back.
static const DWORD kRegistryNotifyDebounceMs = 750;

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
    settings.pollIntervalSec = std::clamp(Wh_GetIntSetting(L"pollIntervalSec"), 1, 3600);
    settings.conflictBackoffSec = std::clamp(Wh_GetIntSetting(L"conflictBackoffSec"), 0, 86400);
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

#pragma endregion  // Reading the state

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
                               RRF_RT_DWORD, nullptr, &keyCount, &size);

    if (ret == ERROR_SUCCESS && keyCount > 0 && keyCount <= 8) {
        // Each key of the shortcut is stored in its own value: IRToggleHKey0, IRToggleHKey1, ...
        for (DWORD i = 0; i < keyCount; i++) {
            std::wstring valueName = L"IRToggleHKey" + std::to_wstring(i);

            DWORD vkey = 0;
            size = sizeof(vkey);
            if (RegGetValueW(HKEY_CURRENT_USER, kShadowPlayRegKey, valueName.c_str(),
                             RRF_RT_DWORD, nullptr, &vkey, &size) != ERROR_SUCCESS) {
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
        Wh_Log(L"SendInput only sent %u of %zu inputs, error %u", sent, inputs.size(),
               GetLastError());
        return false;
    }

    return true;
}

#pragma endregion  // Changing the state

#pragma region Process matching

// CompareStringOrdinal rather than towlower, which only folds ASCII under the default locale
// and would quietly fail to match paths containing non-ASCII characters.
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

    for (size_t i = 0; i + needle.size() <= haystack.size(); i++) {
        if (EqualsNoCase(haystack.substr(i, needle.size()), needle)) {
            return true;
        }
    }

    return false;
}

// A pattern without a backslash is matched against the process file name exactly, so
// "notepad" doesn't quietly match ...\notepad++\notepad++.exe. A pattern with a backslash is
// matched as a substring of the full image path, which covers "everything in this folder".
static bool MatchesProcess(std::wstring_view imagePath,
                           std::wstring_view fileName,
                           const std::wstring& pattern) {
    if (pattern.find(L'\\') != std::wstring::npos) {
        return ContainsNoCase(imagePath, pattern);
    }

    return EqualsNoCase(fileName, pattern);
}

// Returns nullopt when the process list couldn't be read at all. That has to stay
// distinguishable from "nothing matched": with a non-empty onlyWhileRunning list, treating a
// failed enumeration as "nothing matched" would make the mod switch Instant Replay *off* while
// a game is running, which is the exact thing it exists to prevent. CreateToolhelp32Snapshot is
// documented to fail transiently with ERROR_BAD_LENGTH, so this isn't hypothetical.
static std::optional<bool> IsAnyProcessRunning(const std::vector<std::wstring>& patterns) {
    if (patterns.empty()) {
        return false;
    }

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

    bool found = false;

    do {
        std::wstring imagePath = entry.szExeFile;

        HANDLE process =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ProcessID);
        if (process) {
            WCHAR path[MAX_PATH];
            DWORD pathLen = ARRAYSIZE(path);
            if (QueryFullProcessImageNameW(process, 0, path, &pathLen)) {
                imagePath.assign(path, pathLen);
            }
            CloseHandle(process);
        }

        std::wstring_view fileName = imagePath;
        size_t lastSeparator = fileName.find_last_of(L'\\');
        if (lastSeparator != std::wstring_view::npos) {
            fileName = fileName.substr(lastSeparator + 1);
        }

        for (const std::wstring& pattern : patterns) {
            if (MatchesProcess(imagePath, fileName, pattern)) {
                Wh_Log(L"Process match: '%s' matches '%s'", imagePath.c_str(), pattern.c_str());
                found = true;
                break;
            }
        }
    } while (!found && Process32NextW(snapshot, &entry));

    CloseHandle(snapshot);
    return found;
}

#pragma endregion  // Process matching

#pragma region Watchdog

static bool StopRequested(DWORD waitMs) {
    return WaitForSingleObject(g_stopEvent, waitMs) == WAIT_OBJECT_0;
}

static DWORD WINAPI InstantReplayWatchdogThread(LPVOID) {
    Wh_Log(L"Instant Replay watchdog started");

    // Waiting on a registry notification means we react the moment something turns Instant
    // Replay off, instead of up to a whole poll interval later.
    HKEY notifyKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kShadowPlayRegKey, 0, KEY_NOTIFY, &notifyKey) !=
        ERROR_SUCCESS) {
        notifyKey = nullptr;
    }

    HANDLE notifyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    bool notifyArmed = false;

    int toggleStreak = 0;
    int currentBackoffSec = 0;
    ULONGLONG conflictUntil = 0;
    ULONGLONG nextToggleAllowed = 0;
    bool loggedUnreadableState = false;
    bool loggedEnumerationFailure = false;

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
            // Settings changed. Give the new configuration a clean slate.
            toggleStreak = 0;
            currentBackoffSec = 0;
            conflictUntil = 0;
        } else if (waited == WAIT_OBJECT_0 + 2) {
            notifyArmed = false;
            if (StopRequested(kRegistryNotifyDebounceMs)) {
                break;
            }
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

        bool shouldBeOn = true;
        if (!settings.onlyWhileRunning.empty()) {
            std::optional<bool> running = IsAnyProcessRunning(settings.onlyWhileRunning);
            if (!running) {
                if (!loggedEnumerationFailure) {
                    loggedEnumerationFailure = true;
                    Wh_Log(L"Couldn't enumerate processes, skipping this cycle rather than "
                           L"guessing at the state Instant Replay should be in");
                }
                continue;
            }
            loggedEnumerationFailure = false;
            shouldBeOn = *running;
        }

        bool isOn = state == InstantReplayState::On;

        // Whatever we were fighting with has stopped, or never existed. Note this is reached
        // even while backing off: a backoff only holds off the toggle, it never stops the mod
        // noticing that the situation has resolved.
        if (isOn == shouldBeOn) {
            toggleStreak = 0;
            currentBackoffSec = 0;
            conflictUntil = 0;
            continue;
        }

        if (!settings.pauseWhileRunning.empty()) {
            std::optional<bool> paused = IsAnyProcessRunning(settings.pauseWhileRunning);
            if (!paused) {
                if (!loggedEnumerationFailure) {
                    loggedEnumerationFailure = true;
                    Wh_Log(L"Couldn't enumerate processes, skipping this cycle rather than "
                           L"acting while a paused program might be running");
                }
                continue;
            }
            loggedEnumerationFailure = false;
            if (*paused) {
                toggleStreak = 0;
                continue;
            }
        }

        ULONGLONG now = GetTickCount64();
        if (now < conflictUntil) {
            continue;
        }

        // If our toggles keep failing to stick, something is undoing them. Yield for a while
        // rather than flip-flopping forever, growing the wait only as the fight continues.
        if (settings.conflictBackoffSec > 0 && toggleStreak >= kMaxTogglesBeforeBackoff) {
            currentBackoffSec = currentBackoffSec == 0
                                    ? std::min(kInitialBackoffSec, settings.conflictBackoffSec)
                                    : std::min(currentBackoffSec * 2, settings.conflictBackoffSec);
            conflictUntil = now + (ULONGLONG)currentBackoffSec * 1000;
            toggleStreak = kStreakAfterBackoff;
            Wh_Log(L"Something keeps changing Instant Replay back. Backing off for %d seconds.",
                   currentBackoffSec);
            continue;
        }

        // A burst of registry notifications shouldn't turn into a burst of keystrokes.
        if (now < nextToggleAllowed) {
            if (StopRequested((DWORD)(nextToggleAllowed - now))) {
                break;
            }
            now = GetTickCount64();
        }

        toggleStreak++;
        nextToggleAllowed = now + kMinToggleIntervalMs;
        Wh_Log(L"Instant Replay is %s but should be %s, pressing the toggle shortcut",
               isOn ? L"on" : L"off", shouldBeOn ? L"on" : L"off");

        // Success here only means the input was injected - if Nvidia ignores it the state won't
        // change, the next cycles will notice, and the backoff above stops us hammering it.
        if (!ToggleInstantReplayWithHotkey()) {
            Wh_Log(L"Failed to send the Instant Replay toggle shortcut");
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
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
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
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
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

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
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

    STARTUPINFOW si{
        .cb = sizeof(STARTUPINFOW),
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
