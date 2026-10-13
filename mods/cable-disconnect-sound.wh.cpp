// ==WindhawkMod==
// @id              cable-disconnect-sound
// @name            Ethernet Cable Disconnect (unplug) Sound
// @description     Plays a sound when a wired or wifi network cable is disconnected or reconnected
// @version         1.2
// @author          Tretri
// @include         explorer.exe
// @compilerOptions -liphlpapi -lwinmm
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

Author: https://github.com/tretri

# Ethernet Cable Disconnect Sound

Plays a custom sound whenever a wired (Ethernet) network cable is
unplugged from the PC, and optionally another sound when it's plugged
back in / reconnects.

Wi-Fi and virtual/tunnel adapters are ignored by default — only physical
wired Ethernet adapters trigger the sound. This can be changed in the
mod's settings.

The mod runs in the context of `explorer.exe`, since that process is
always running while you're logged in. A lightweight background thread
polls the network interface state once per second and plays a sound on
Up -> Down (disconnect) and, if enabled, Down -> Up (reconnect)
transitions of a wired adapter.

## Settings

- **Disconnect sound file**: full path to a sound file to play on
  disconnect. `.wav` and `.mp3` are both supported (mp3 is played via
  the Windows Media Control Interface, so it depends on codecs already
  installed on the system — this works out of the box on a normal
  desktop Windows install). Leave empty to use the built-in Windows
  "Critical Stop" system sound.
- **Play sound on reconnect**: also play a sound when the cable is
  plugged back in.
- **Reconnect sound file**: full path to a sound file (.wav or .mp3) to
  play on reconnect. Leave empty to use the built-in Windows "Asterisk"
  system sound.
- **Also on Wi-Fi disconnect/reconnect**: also play the sounds when a
  Wi-Fi adapter loses/regains its connection.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- soundFileDisconnect: ""
  $name: Disconnect sound file
  $description: Full path to a .wav or .mp3 file to play on disconnect. Leave empty to use the default Windows sound.
- playOnConnect: true
  $name: Play sound on reconnect
  $description: Also play a sound when the cable is plugged back in.
- soundFileConnect: ""
  $name: Reconnect sound file
  $description: Full path to a .wav or .mp3 file to play on reconnect. Leave empty to use the default Windows sound.
- alsoWifi: false
  $name: Also trigger on Wi-Fi disconnect/reconnect
  $description: By default only a wired Ethernet cable triggers the sounds.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <iphlpapi.h>
#include <mmsystem.h>

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

// IANA ifType values (RFC 2863), stable across API versions.
constexpr DWORD kIfTypeEthernetCsmacd = 6;
constexpr DWORD kIfTypeIeee80211 = 71;

// MIB_IF_OPER_STATUS values (classic IP Helper API).
constexpr DWORD kOperStatusOperational = 5;

struct Settings {
    std::wstring soundFileDisconnect;
    std::wstring soundFileConnect;
    bool playOnConnect = true;
    bool alsoWifi = false;
} g_settings;

std::mutex g_stateMutex;
std::unordered_map<DWORD, DWORD> g_ifState;  // interface index -> dwOperStatus

HANDLE g_stopEvent = nullptr;
HANDLE g_pollThread = nullptr;

void LoadSettings() {
    PCWSTR soundDisconnect = Wh_GetStringSetting(L"soundFileDisconnect");
    g_settings.soundFileDisconnect = soundDisconnect ? soundDisconnect : L"";
    Wh_FreeStringSetting(soundDisconnect);

    PCWSTR soundConnect = Wh_GetStringSetting(L"soundFileConnect");
    g_settings.soundFileConnect = soundConnect ? soundConnect : L"";
    Wh_FreeStringSetting(soundConnect);

    g_settings.playOnConnect = Wh_GetIntSetting(L"playOnConnect") != 0;
    g_settings.alsoWifi = Wh_GetIntSetting(L"alsoWifi") != 0;
}

// Plays an arbitrary audio file (wav, mp3, etc.) via the Windows Media
// Control Interface. MCI figures out which driver to use based on the
// file extension, so this works for formats PlaySound alone can't
// handle, such as mp3. `alias` must be a short, mod-unique identifier
// so that repeated calls (e.g. disconnect vs. reconnect) don't collide.
void PlayFileViaMci(const std::wstring& path, PCWSTR alias) {
    wchar_t cmd[1024];

    // Close any previous instance of this alias, ignoring errors (it's
    // fine if nothing was open).
    wsprintfW(cmd, L"close %s", alias);
    mciSendStringW(cmd, nullptr, 0, nullptr);

    wsprintfW(cmd, L"open \"%s\" alias %s", path.c_str(), alias);
    MCIERROR err = mciSendStringW(cmd, nullptr, 0, nullptr);
    if (err != 0) {
        wchar_t errText[256];
        mciGetErrorStringW(err, errText, ARRAYSIZE(errText));
        Wh_Log(L"MCI open failed for %s: %s", path.c_str(), errText);
        return;
    }

    wsprintfW(cmd, L"play %s", alias);
    mciSendStringW(cmd, nullptr, 0, nullptr);
}

void PlaySoundFileOrDefault(const std::wstring& customFile,
                            PCWSTR defaultAlias,
                            PCWSTR mciAlias) {
    if (!customFile.empty()) {
        PlayFileViaMci(customFile, mciAlias);
    } else {
        PlaySoundW(defaultAlias, nullptr, SND_ALIAS | SND_ASYNC);
    }
}

void PlayDisconnectSound() {
    // Built-in Windows system sound used as the default: "Critical Stop".
    PlaySoundFileOrDefault(g_settings.soundFileDisconnect, L"SystemHand",
                           L"whcablediscon");
}

void PlayConnectSound() {
    // Built-in Windows system sound used as the default: "Asterisk".
    PlaySoundFileOrDefault(g_settings.soundFileConnect, L"SystemAsterisk",
                           L"whcableconn");
}

bool IsRelevantAdapterType(DWORD type) {
    if (type == kIfTypeEthernetCsmacd) {
        return true;
    }
    if (g_settings.alsoWifi && type == kIfTypeIeee80211) {
        return true;
    }
    return false;
}

void CheckInterfacesOnce(bool initial) {
    ULONG size = 0;
    if (GetIfTable(nullptr, &size, FALSE) != ERROR_INSUFFICIENT_BUFFER ||
        size == 0) {
        return;
    }

    std::vector<BYTE> buffer(size);
    MIB_IFTABLE* table = reinterpret_cast<MIB_IFTABLE*>(buffer.data());
    if (GetIfTable(table, &size, FALSE) != NO_ERROR) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_stateMutex);

    for (DWORD i = 0; i < table->dwNumEntries; i++) {
        const MIB_IFROW& row = table->table[i];

        if (!IsRelevantAdapterType(row.dwType)) {
            continue;
        }

        auto it = g_ifState.find(row.dwIndex);

        // On the very first check, use the current status as the
        // baseline, so we don't fire a false alarm for whatever state
        // the cable happened to be in before the mod was loaded.
        DWORD prevStatus = (it != g_ifState.end()) ? it->second
                                                    : row.dwOperStatus;

        bool wasUp = (prevStatus == kOperStatusOperational);
        bool isUp = (row.dwOperStatus == kOperStatusOperational);

        if (!initial) {
            if (wasUp && !isUp) {
                Wh_Log(L"Network cable disconnected: %s", row.wszName);
                PlayDisconnectSound();
            } else if (!wasUp && isUp && g_settings.playOnConnect) {
                Wh_Log(L"Network cable reconnected: %s", row.wszName);
                PlayConnectSound();
            }
        }

        g_ifState[row.dwIndex] = row.dwOperStatus;
    }
}

DWORD WINAPI PollThreadProc(LPVOID) {
    // Establish baseline state without triggering a sound.
    CheckInterfacesOnce(/*initial=*/true);

    while (WaitForSingleObject(g_stopEvent, 1000) == WAIT_TIMEOUT) {
        CheckInterfacesOnce(/*initial=*/false);
    }

    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_pollThread = CreateThread(nullptr, 0, PollThreadProc, nullptr, 0, nullptr);
    if (!g_pollThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_pollThread) {
        WaitForSingleObject(g_pollThread, 5000);
        CloseHandle(g_pollThread);
        g_pollThread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    // Make sure any MCI devices we opened are released.
    mciSendStringW(L"close whcablediscon", nullptr, 0, nullptr);
    mciSendStringW(L"close whcableconn", nullptr, 0, nullptr);
}

BOOL Wh_ModSettingsChanged() {
    LoadSettings();
    return TRUE;
}