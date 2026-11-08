// ==WindhawkMod==
// @id              mute-focused-window
// @name            Mute Focused Window
// @description     Press a hotkey to mute or unmute the audio of the window that currently has focus
// @version         1.0
// @author          Benevolent Anarchy
// @github          https://github.com/cicely-f
// @homepage        https://github.com/Benevolent-Anarchy/Mute-Focused-Window
// @include         windhawk.exe
// @compilerOptions -lole32
// @license         MIT
// ==/WindhawkMod==

// Source code is published under the MIT License.
//
// Development, bug reports and feature requests:
// https://github.com/Benevolent-Anarchy/Mute-Focused-Window

// ==WindhawkModReadme==
/*
# Mute Focused Window

Press **Ctrl+Alt+M** to mute the app whose window has focus. Press it again to
unmute it. Other apps keep playing.

This changes the same per-app mute as the Windows Volume Mixer, so the mixer
shows the result and can undo it.

## What gets muted

The mod mutes every audio session that belongs to the focused window's app, on
every active output device. If some of those sessions are muted and others are
not, the first press mutes them all.

Browsers and Electron apps such as Discord play sound from a helper process
instead of the process that owns the window. The mod therefore includes the
app's background child processes too. A child process with its own visible
window is treated as a separate app and left alone. For example, a media player
launched from a terminal keeps playing when you mute the terminal.

## Hotkey

Change the hotkey in the mod's settings. Write any modifiers and exactly one
key, joined by `+`, for example `Ctrl+Alt+M`, `Win+Shift+F9` or `Pause`.

* Modifiers: `Ctrl`, `Alt`, `Shift`, `Win`.
* Keys: `A`–`Z`, `0`–`9`, `F1`–`F24`, `Space`, `Tab`, `Enter`, `Esc`,
  `Backspace`, `Insert`, `Delete`, `Home`, `End`, `PageUp`, `PageDown`, `Up`,
  `Down`, `Left`, `Right`, `Pause`, `ScrollLock`, `PrintScreen`, `Numpad0`–
  `Numpad9`, `VolumeMute`, `VolumeDown`, `VolumeUp`, `MediaPlayPause`,
  `MediaNext`, `MediaPrev`, `MediaStop`.
* Any other key: its virtual-key code in hexadecimal, such as `0x4D`.

Windows gives a hotkey to only one program at a time, and other apps stop
receiving that key combination. If the hotkey does nothing, another program may
already own it. Turn on logging in the mod's Advanced tab to see why.

The default, Ctrl+Alt+M, also takes Word's Insert Comment shortcut. On keyboard
layouts with an AltGr key, Windows treats AltGr as Ctrl+Alt, so AltGr+M (`µ` on
German layouts) stops typing its character. Choose another hotkey if you need
either.

## Using the keyboard's Mute key

Most keyboards with a dedicated Mute key, and laptops with a mute function key,
send `VolumeMute`.

* Set the hotkey to `VolumeMute` to make the Mute key mute the focused app
  instead of everything. Also turn on **Mute the system when there's no app to
  mute**, so the key still mutes everything when you press it on the desktop,
  the taskbar, a File Explorer window or an app without audio.
* Set the hotkey to `Ctrl+VolumeMute` to keep the Mute key as it is and use
  Ctrl+Mute for the focused app.

Keyboard software that reassigns the Mute key can stop it from sending
`VolumeMute`.

## Limitations

* A browser is muted as a whole, not tab by tab. The Volume Mixer shows it the
  same way.
* An app started from the focused app and hidden in the tray, with no visible
  window, counts as part of the focused app. For example, a music player
  started from a launcher and minimized to the tray is muted along with the
  launcher. Windows gives no reliable way to tell such an app from the hidden
  helper processes that browsers and Electron apps play their sound through.
* Windows remembers an app's mute setting. An app you muted is still muted the
  next time it starts; press the hotkey or use the Volume Mixer to unmute it.
* Pressing the hotkey on the desktop, the taskbar or a File Explorer window does
  nothing, unless **Mute the system when there's no app to mute** is on.
* That system mute doesn't show Windows' volume indicator.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey: Ctrl+Alt+M
  $name: Hotkey
  $description: Modifier keys and one key joined by +, such as Ctrl+Alt+M, Win+Shift+F9 or VolumeMute
- systemMuteFallback: false
  $name: Mute the system when there's no app to mute
  $description: When the focused window has no app audio, as on the desktop or the taskbar, mute or unmute the default output device instead. Useful with VolumeMute as the hotkey.
*/
// ==/WindhawkModSettings==

#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <tlhelp32.h>
#include <windhawk_utils.h>
#include <wrl/client.h>

#include <cwctype>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Microsoft::WRL::ComPtr;

////////////////////////////////////////////////////////////////////////////////
// Pure logic, covered by the unit tests in the development repository linked
// above.

struct Hotkey {
    UINT modifiers;  // MOD_* flags, without MOD_NOREPEAT.
    UINT virtualKey;
};

struct ProcessInfo {
    DWORD id;
    DWORD parentId;
    // Creation time as a FILETIME value, or 0 if the process can't be opened.
    ULONGLONG creationTime;
};

std::wstring_view TrimSpaces(std::wstring_view text) {
    while (!text.empty() && iswspace(text.front())) {
        text.remove_prefix(1);
    }
    while (!text.empty() && iswspace(text.back())) {
        text.remove_suffix(1);
    }
    return text;
}

bool EqualsIgnoreCase(std::wstring_view a, std::wstring_view b) {
    return CompareStringOrdinal(a.data(), static_cast<int>(a.size()), b.data(),
                                static_cast<int>(b.size()),
                                TRUE) == CSTR_EQUAL;
}

UINT ModifierFromName(std::wstring_view name) {
    static constexpr struct {
        std::wstring_view name;
        UINT modifier;
    } kModifiers[] = {
        {L"Ctrl", MOD_CONTROL}, {L"Control", MOD_CONTROL},
        {L"Alt", MOD_ALT},      {L"Shift", MOD_SHIFT},
        {L"Win", MOD_WIN},      {L"Windows", MOD_WIN},
    };
    for (const auto& entry : kModifiers) {
        if (EqualsIgnoreCase(name, entry.name)) {
            return entry.modifier;
        }
    }
    return 0;
}

std::optional<UINT> VirtualKeyFromName(std::wstring_view name) {
    if (name.size() == 1) {
        WCHAR c = name[0];
        if (c >= L'a' && c <= L'z') {
            c = c - L'a' + L'A';
        }
        // Letter and digit virtual-key codes are their ASCII codes.
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            return c;
        }
        return std::nullopt;
    }

    if ((name[0] == L'F' || name[0] == L'f') && name.size() <= 3) {
        UINT number = 0;
        for (WCHAR c : name.substr(1)) {
            if (c < L'0' || c > L'9') {
                return std::nullopt;
            }
            number = number * 10 + (c - L'0');
        }
        if (number >= 1 && number <= 24 && name[1] != L'0') {
            return VK_F1 + number - 1;
        }
        return std::nullopt;
    }

    if (name.size() > 2 && name[0] == L'0' &&
        (name[1] == L'x' || name[1] == L'X')) {
        UINT code = 0;
        for (WCHAR c : name.substr(2)) {
            UINT digit;
            if (c >= L'0' && c <= L'9') {
                digit = c - L'0';
            } else if (c >= L'a' && c <= L'f') {
                digit = c - L'a' + 10;
            } else if (c >= L'A' && c <= L'F') {
                digit = c - L'A' + 10;
            } else {
                return std::nullopt;
            }
            code = code * 16 + digit;
            if (code > 0xFE) {
                return std::nullopt;
            }
        }
        // Valid virtual-key codes are 1 to 254.
        return code ? std::optional<UINT>(code) : std::nullopt;
    }

    static constexpr struct {
        std::wstring_view name;
        UINT virtualKey;
    } kKeys[] = {
        {L"Space", VK_SPACE},
        {L"Tab", VK_TAB},
        {L"Enter", VK_RETURN},
        {L"Esc", VK_ESCAPE},
        {L"Escape", VK_ESCAPE},
        {L"Backspace", VK_BACK},
        {L"Insert", VK_INSERT},
        {L"Delete", VK_DELETE},
        {L"Home", VK_HOME},
        {L"End", VK_END},
        {L"PageUp", VK_PRIOR},
        {L"PageDown", VK_NEXT},
        {L"Up", VK_UP},
        {L"Down", VK_DOWN},
        {L"Left", VK_LEFT},
        {L"Right", VK_RIGHT},
        {L"Pause", VK_PAUSE},
        {L"ScrollLock", VK_SCROLL},
        {L"PrintScreen", VK_SNAPSHOT},
        {L"VolumeMute", VK_VOLUME_MUTE},
        {L"VolumeDown", VK_VOLUME_DOWN},
        {L"VolumeUp", VK_VOLUME_UP},
        {L"MediaPlayPause", VK_MEDIA_PLAY_PAUSE},
        {L"MediaNext", VK_MEDIA_NEXT_TRACK},
        {L"MediaPrev", VK_MEDIA_PREV_TRACK},
        {L"MediaStop", VK_MEDIA_STOP},
    };
    for (const auto& entry : kKeys) {
        if (EqualsIgnoreCase(name, entry.name)) {
            return entry.virtualKey;
        }
    }

    constexpr std::wstring_view kNumpad = L"Numpad";
    if (name.size() == kNumpad.size() + 1 &&
        EqualsIgnoreCase(name.substr(0, kNumpad.size()), kNumpad) &&
        name.back() >= L'0' && name.back() <= L'9') {
        return VK_NUMPAD0 + (name.back() - L'0');
    }

    return std::nullopt;
}

// Parses "Modifier+...+Key", such as "Ctrl+Alt+M". Names are case-insensitive
// and may be surrounded by spaces. Exactly one non-modifier key is required.
std::optional<Hotkey> ParseHotkey(std::wstring_view text) {
    Hotkey hotkey{};
    bool hasKey = false;
    while (true) {
        size_t separator = text.find(L'+');
        std::wstring_view part = TrimSpaces(text.substr(0, separator));
        if (part.empty()) {
            return std::nullopt;
        }

        if (UINT modifier = ModifierFromName(part)) {
            hotkey.modifiers |= modifier;
        } else if (std::optional<UINT> key = VirtualKeyFromName(part);
                   key && !hasKey) {
            hotkey.virtualKey = *key;
            hasKey = true;
        } else {
            return std::nullopt;
        }

        if (separator == std::wstring_view::npos) {
            break;
        }
        text.remove_prefix(separator + 1);
    }

    if (!hasKey) {
        return std::nullopt;
    }
    return hotkey;
}

// Whether a top-level window shows that its process is an app of its own: one
// that would appear on the taskbar or in Alt+Tab, being visible, unowned and
// not a tool window. Hidden windows don't count, even captioned ones: the
// Chromium, Electron and WebView2 processes above an app's audio process own
// hidden captioned windows, so counting them would cut that audio off. An app
// hidden in the tray therefore counts as part of the app that started it.
bool IsAppWindow(bool visible, bool owned, LONG_PTR exStyle) {
    return visible && !owned && !(exStyle & WS_EX_TOOLWINDOW);
}

// Returns the processes whose audio counts as the focused app's: the root and
// its descendants, except that a descendant which owns an app window (see
// IsAppWindow) is a separate app and is skipped along with its own children.
// Browsers and Electron apps play audio from child processes without one.
std::unordered_set<DWORD> CollectProcessFamily(
    DWORD rootId,
    const std::vector<ProcessInfo>& processes,
    const std::unordered_set<DWORD>& windowOwnerIds) {
    std::unordered_map<DWORD, ULONGLONG> creationTimes;
    for (const ProcessInfo& process : processes) {
        creationTimes[process.id] = process.creationTime;
    }

    std::unordered_set<DWORD> family{rootId};
    std::vector<DWORD> pending{rootId};
    while (!pending.empty()) {
        DWORD parentId = pending.back();
        pending.pop_back();
        auto parent = creationTimes.find(parentId);
        if (parent == creationTimes.end() || !parent->second) {
            continue;
        }

        for (const ProcessInfo& process : processes) {
            if (process.parentId != parentId || family.contains(process.id) ||
                windowOwnerIds.contains(process.id)) {
                continue;
            }
            // Windows doesn't update parent IDs when a parent exits, and its
            // ID can be reused. A real child starts after its parent.
            if (!process.creationTime ||
                process.creationTime < parent->second) {
                continue;
            }
            family.insert(process.id);
            pending.push_back(process.id);
        }
    }
    return family;
}

// What a hotkey press did to the focused app.
enum class ToggleResult {
    kToggled,
    // The focused window has no app audio: it isn't an app window, it belongs
    // to Explorer, or a complete search found none of its sessions.
    kNoAppAudio,
    // An audio operation failed, so the app may still have audio.
    kFailed,
};

// Only a press that found no app audio may mute the whole system instead.
// After a failure the app may still be playing, and muting everything would
// affect unrelated apps.
bool ShouldFallBackToSystemMute(ToggleResult result, bool fallbackEnabled) {
    return fallbackEnabled && result == ToggleResult::kNoAppAudio;
}

// Finding none of the app's sessions proves it has no audio only if every
// device and session could be inspected.
ToggleResult ResultForNoSessions(bool searchComplete) {
    return searchComplete ? ToggleResult::kNoAppAudio : ToggleResult::kFailed;
}

struct SessionToggle {
    ToggleResult result;  // kToggled or kFailed.
    bool mute;            // The state set, when result is kToggled.
    int changed;          // Sessions whose mute was set.
    int failed;           // Sessions that couldn't be read or set.
};

// Toggles an app's sessions together: mute unless every session whose state
// could be read is already muted, so a partly muted app becomes fully muted
// rather than having each session flipped. A session whose state can't be
// read, such as one on a device that was just unplugged, is left out of both
// the decision and the change. Fails if no session could be set.
SessionToggle ToggleSessions(
    const std::vector<ComPtr<ISimpleAudioVolume>>& volumes) {
    std::vector<ISimpleAudioVolume*> readable;
    bool mute = false;
    for (const auto& volume : volumes) {
        BOOL muted;
        if (SUCCEEDED(volume->GetMute(&muted))) {
            readable.push_back(volume.Get());
            mute = mute || !muted;
        }
    }

    SessionToggle toggle{ToggleResult::kFailed, mute, 0,
                         static_cast<int>(volumes.size() - readable.size())};
    for (ISimpleAudioVolume* volume : readable) {
        if (SUCCEEDED(volume->SetMute(mute, nullptr))) {
            toggle.changed++;
        } else {
            toggle.failed++;
        }
    }
    if (toggle.changed) {
        toggle.result = ToggleResult::kToggled;
    }
    return toggle;
}

////////////////////////////////////////////////////////////////////////////////
// System queries and the hotkey thread.

constexpr int kHotkeyId = 1;
constexpr UINT kMsgReloadSettings = WM_APP + 1;
constexpr UINT kMsgQuit = WM_APP + 2;

HANDLE g_hotkeyThread;
DWORD g_hotkeyThreadId;
// Read and written only on the hotkey thread.
bool g_systemMuteFallback;

ULONGLONG GetProcessCreationTime(DWORD processId) {
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return 0;
    }
    FILETIME creation, exit, kernel, user;
    ULONGLONG result = 0;
    if (GetProcessTimes(process, &creation, &exit, &kernel, &user)) {
        result = (ULONGLONG{creation.dwHighDateTime} << 32) |
                 creation.dwLowDateTime;
    }
    CloseHandle(process);
    return result;
}

std::wstring GetProcessImagePath(DWORD processId) {
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return {};
    }
    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    std::wstring result;
    if (QueryFullProcessImageName(process, 0, path, &size)) {
        result.assign(path, size);
    }
    CloseHandle(process);
    return result;
}

std::vector<ProcessInfo> SnapshotProcesses() {
    std::vector<ProcessInfo> processes;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return processes;
    }
    PROCESSENTRY32W entry{.dwSize = sizeof(entry)};
    for (BOOL found = Process32FirstW(snapshot, &entry); found;
         found = Process32NextW(snapshot, &entry)) {
        processes.push_back({entry.th32ProcessID, entry.th32ParentProcessID,
                             GetProcessCreationTime(entry.th32ProcessID)});
    }
    CloseHandle(snapshot);
    return processes;
}

BOOL CALLBACK AddAppWindowOwnerId(HWND window, LPARAM ownerIds) {
    if (IsAppWindow(IsWindowVisible(window),
                    GetWindow(window, GW_OWNER) != nullptr,
                    GetWindowLongPtr(window, GWL_EXSTYLE))) {
        DWORD processId = 0;
        GetWindowThreadProcessId(window, &processId);
        reinterpret_cast<std::unordered_set<DWORD>*>(ownerIds)->insert(
            processId);
    }
    return TRUE;
}

// Processes owning an app window (see IsAppWindow).
std::unordered_set<DWORD> GetAppWindowOwnerIds() {
    std::unordered_set<DWORD> ownerIds;
    EnumWindows(AddAppWindowOwnerId, reinterpret_cast<LPARAM>(&ownerIds));
    return ownerIds;
}

// Returns the process that owns the focused window's content, or 0 if there is
// none. UWP apps are framed by ApplicationFrameHost.exe; the app's own process
// owns the CoreWindow child.
DWORD GetFocusedAppProcessId() {
    HWND window = GetForegroundWindow();
    if (!window) {
        return 0;
    }
    WCHAR className[64];
    if (GetClassName(window, className, ARRAYSIZE(className)) &&
        wcscmp(className, L"ApplicationFrameWindow") == 0) {
        if (HWND coreWindow = FindWindowEx(
                window, nullptr, L"Windows.UI.Core.CoreWindow", nullptr)) {
            Wh_Log(L"Using the CoreWindow of a framed UWP app");
            window = coreWindow;
        } else {
            Wh_Log(L"A framed UWP app has no CoreWindow");
        }
    }
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    return processId;
}

struct SessionSearch {
    std::vector<ComPtr<ISimpleAudioVolume>> volumes;
    // False if a device or session couldn't be inspected, so finding no
    // sessions doesn't prove the app has none.
    bool complete = true;
};

// Collects the volume controls of all sessions owned by the given processes,
// on every active output device.
SessionSearch FindSessionVolumes(const std::unordered_set<DWORD>& processIds) {
    SessionSearch search;
    ComPtr<IMMDeviceEnumerator> enumerator;
    ComPtr<IMMDeviceCollection> devices;
    UINT deviceCount = 0;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&enumerator))) ||
        FAILED(enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE,
                                              &devices)) ||
        FAILED(devices->GetCount(&deviceCount))) {
        Wh_Log(L"Can't list audio devices");
        search.complete = false;
        return search;
    }

    for (UINT i = 0; i < deviceCount; i++) {
        ComPtr<IMMDevice> device;
        ComPtr<IAudioSessionManager2> manager;
        ComPtr<IAudioSessionEnumerator> sessions;
        int sessionCount = 0;
        if (FAILED(devices->Item(i, &device)) ||
            FAILED(device->Activate(
                __uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
                reinterpret_cast<void**>(manager.GetAddressOf()))) ||
            FAILED(manager->GetSessionEnumerator(&sessions)) ||
            FAILED(sessions->GetCount(&sessionCount))) {
            Wh_Log(L"Can't list the sessions of audio device %u", i);
            search.complete = false;
            continue;
        }

        for (int j = 0; j < sessionCount; j++) {
            ComPtr<IAudioSessionControl> control;
            ComPtr<IAudioSessionControl2> control2;
            ComPtr<ISimpleAudioVolume> volume;
            DWORD processId = 0;
            if (FAILED(sessions->GetSession(j, &control)) ||
                FAILED(control.As(&control2))) {
                search.complete = false;
                continue;
            }
            // S_OK means the system sounds session, which belongs to no app.
            if (control2->IsSystemSoundsSession() == S_OK) {
                continue;
            }
            // A session shared by several processes returns a success code
            // with the ID of the process that created it, which is what to
            // match.
            if (FAILED(control2->GetProcessId(&processId))) {
                search.complete = false;
                continue;
            }
            if (!processIds.contains(processId)) {
                continue;
            }
            if (SUCCEEDED(control.As(&volume))) {
                search.volumes.push_back(std::move(volume));
            } else {
                search.complete = false;
            }
        }
    }
    return search;
}

// Toggles the default output device's mute, as a keyboard's Mute key does,
// but without Windows' volume indicator.
void ToggleSystemMute() {
    ComPtr<IMMDeviceEnumerator> enumerator;
    ComPtr<IMMDevice> device;
    ComPtr<IAudioEndpointVolume> endpointVolume;
    BOOL muted = FALSE;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&enumerator))) ||
        FAILED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                   &device)) ||
        FAILED(device->Activate(
            __uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
            reinterpret_cast<void**>(endpointVolume.GetAddressOf()))) ||
        FAILED(endpointVolume->GetMute(&muted)) ||
        FAILED(endpointVolume->SetMute(!muted, nullptr))) {
        Wh_Log(L"Can't toggle the system mute");
        return;
    }
    Wh_Log(L"%s the system", muted ? L"Unmuted" : L"Muted");
}

ToggleResult ToggleFocusedAppMute() {
    DWORD rootId = GetFocusedAppProcessId();
    if (!rootId || rootId == GetCurrentProcessId()) {
        Wh_Log(L"The focused window isn't an app window");
        return ToggleResult::kNoAppAudio;
    }
    // Explorer hosts the desktop, the taskbar and folder windows, and starts
    // nearly every app, so its descendants include unrelated background apps.
    // Folder windows can run in a separate explorer.exe, hence the path check.
    DWORD shellId = 0;
    GetWindowThreadProcessId(GetShellWindow(), &shellId);
    std::wstring rootPath = GetProcessImagePath(rootId);
    if (rootId == shellId ||
        (!rootPath.empty() &&
         EqualsIgnoreCase(rootPath, GetProcessImagePath(shellId)))) {
        Wh_Log(L"The focused window belongs to Explorer");
        return ToggleResult::kNoAppAudio;
    }

    SessionSearch search = FindSessionVolumes(
        CollectProcessFamily(rootId, SnapshotProcesses(),
                             GetAppWindowOwnerIds()));
    if (search.volumes.empty()) {
        ToggleResult result = ResultForNoSessions(search.complete);
        if (result == ToggleResult::kFailed) {
            Wh_Log(L"Can't tell whether %s (process %u) has audio sessions",
                   rootPath.c_str(), rootId);
        } else {
            Wh_Log(L"%s (process %u) has no audio sessions", rootPath.c_str(),
                   rootId);
        }
        return result;
    }

    SessionToggle toggle = ToggleSessions(search.volumes);
    if (toggle.result != ToggleResult::kToggled) {
        Wh_Log(L"Can't change the %d session(s) of %s (process %u)",
               static_cast<int>(search.volumes.size()), rootPath.c_str(),
               rootId);
        return toggle.result;
    }
    Wh_Log(L"%s %d session(s) of %s (process %u); %d failed",
           toggle.mute ? L"Muted" : L"Unmuted", toggle.changed,
           rootPath.c_str(), rootId, toggle.failed);
    return ToggleResult::kToggled;
}

// Reads the settings and registers the hotkey for the calling thread,
// replacing any earlier registration. WM_HOTKEY arrives in this thread's
// message queue.
void LoadSettings() {
    g_systemMuteFallback = Wh_GetIntSetting(L"systemMuteFallback") != 0;

    UnregisterHotKey(nullptr, kHotkeyId);
    auto text = WindhawkUtils::StringSetting::make(L"hotkey");
    if (std::optional<Hotkey> hotkey = ParseHotkey(text.get())) {
        if (!RegisterHotKey(nullptr, kHotkeyId,
                            hotkey->modifiers | MOD_NOREPEAT,
                            hotkey->virtualKey)) {
            Wh_Log(L"Can't register hotkey %s, error %u", text.get(),
                   GetLastError());
        }
    } else {
        Wh_Log(L"Invalid hotkey: %s", text.get());
    }
}

DWORD WINAPI HotkeyThreadProc(LPVOID readyEvent) {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // Create the message queue before signalling, so messages posted by the
    // settings and uninit callbacks are never lost.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    LoadSettings();
    SetEvent(static_cast<HANDLE>(readyEvent));

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        switch (msg.message) {
            case WM_HOTKEY:
                if (msg.wParam == kHotkeyId &&
                    ShouldFallBackToSystemMute(ToggleFocusedAppMute(),
                                               g_systemMuteFallback)) {
                    ToggleSystemMute();
                }
                break;
            case kMsgReloadSettings:
                LoadSettings();
                break;
            case kMsgQuit:
                PostQuitMessage(0);
                break;
        }
    }

    UnregisterHotKey(nullptr, kHotkeyId);
    if (SUCCEEDED(comResult)) {
        CoUninitialize();
    }
    return 0;
}

BOOL WhTool_ModInit() {
    HANDLE readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        return FALSE;
    }
    g_hotkeyThread = CreateThread(nullptr, 0, HotkeyThreadProc, readyEvent, 0,
                                  &g_hotkeyThreadId);
    if (!g_hotkeyThread) {
        CloseHandle(readyEvent);
        return FALSE;
    }
    // Returns once the thread is ready, or if it exits before getting there.
    HANDLE handles[] = {readyEvent, g_hotkeyThread};
    WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, INFINITE);
    CloseHandle(readyEvent);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    PostThreadMessage(g_hotkeyThreadId, kMsgReloadSettings, 0, 0);
}

void WhTool_ModUninit() {
    PostThreadMessage(g_hotkeyThreadId, kMsgQuit, 0, 0);
    // A toggle in progress finishes quickly; the process exits after this.
    WaitForSingleObject(g_hotkeyThread, 5000);
    CloseHandle(g_hotkeyThread);
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
