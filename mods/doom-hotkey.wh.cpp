// ==WindhawkMod==
// @id              doom-hotkey
// @name            Can It Run Doom?
// @description     Launch Doom from anywhere with a customizable keyboard shortcut.
// @version         0.3.1
// @author          bardelyne
// @github          https://github.com/bardelyne
// @include         explorer.exe
// @architecture    x86-64
// @license         GPL-2.0-or-later
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Can It Run Doom?

Press **Ctrl+Alt+Shift+D** to launch Doom fullscreen. Press it again to return to the running game.

![Freedoom Phase 1 title screen and main menu](https://raw.githubusercontent.com/bardelyne/doom-hotkey/fcb4b5729067da4cd6110db02d3d3294b8234789/assets/doom-hotkey-freedoom-menu.png)

Use a Doom engine and game data installed on your PC. The [project repository](https://github.com/bardelyne/doom-hotkey) has instructions for building the offline edition with Crispy Doom and Freedoom.

1. Extract a Doom engine such as [Crispy Doom](https://github.com/fabiangreffrath/crispy-doom/releases).
2. In the mod settings, set **Doom executable** to the full path to an engine that accepts the -fullscreen argument.
3. Set **Game WAD** to your Doom WAD or a free [Freedoom](https://freedoom.github.io/download.html) WAD. You can leave this blank if your engine already finds its game data.
4. Set **Launch shortcut** to any combination of Ctrl, Alt, Shift or Win followed by a letter, number or function key. For example, Ctrl+Alt+Shift+D or Ctrl+Alt+F9.

Changes apply when saved. If Windows cannot register the new shortcut, the previous shortcut stays active. The mod uses Windows' global hotkey API and launches the game in a separate process.

## Credits

- **id Software** created DOOM and released its engine source. [DOOM source release](https://github.com/id-Software/DOOM)
- **Simon Howard and the Chocolate Doom contributors** developed the source port that Crispy Doom builds on. [Chocolate Doom](https://github.com/chocolate-doom/chocolate-doom)
- **Fabian Greffrath and the Crispy Doom contributors** maintain the engine used by this mod. [Crispy Doom 7.2.0](https://github.com/fabiangreffrath/crispy-doom/releases/tag/crispy-doom-7.2)
- **The Freedoom contributors** created the campaigns, graphics, sounds, and music bundled with the one-file build. [Freedoom 0.13.0](https://github.com/freedoom/freedoom/releases/tag/v0.13.0)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- shortcut: Ctrl+Alt+Shift+D
  $name: Launch shortcut
  $description: A modifier plus a letter, number or function key. Example Ctrl+Alt+F9.
- enginePath: ""
  $name: Doom executable
  $description: Full path to the Doom engine executable. Environment variables such as %LOCALAPPDATA% are supported.
- iwadPath: ""
  $name: Game WAD
  $description: Full path to your Doom or Freedoom IWAD file. Leave blank if the engine finds the game data itself.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <cwctype>
#include <string>
#include <string_view>

namespace {

struct Shortcut {
    UINT modifiers{};
    UINT key{};
    bool operator==(const Shortcut&) const = default;
};

std::wstring_view Trim(std::wstring_view text) {
    while (!text.empty() && iswspace(text.front())) text.remove_prefix(1);
    while (!text.empty() && iswspace(text.back())) text.remove_suffix(1);
    return text;
}

bool ParseShortcut(std::wstring_view text, Shortcut& result) {
    Shortcut shortcut;
    for (;;) {
        const auto split = text.find(L'+');
        const auto part = Trim(text.substr(0, split));
        std::wstring token(part);
        for (auto& ch : token) ch = static_cast<wchar_t>(towupper(ch));
        if (split == std::wstring_view::npos) {
            if (token.size() == 1 &&
                ((token[0] >= L'A' && token[0] <= L'Z') ||
                 (token[0] >= L'0' && token[0] <= L'9'))) {
                shortcut.key = token[0];
            } else if ((token.size() == 2 || token.size() == 3) && token[0] == L'F' &&
                       token[1] >= L'1' && token[1] <= L'9' &&
                       (token.size() == 2 || (token[2] >= L'0' && token[2] <= L'9'))) {
                UINT function = token[1] - L'0';
                if (token.size() == 3) function = function * 10 + token[2] - L'0';
                if (function <= 24 && function != 12) shortcut.key = VK_F1 + function - 1;
            }
            if (!shortcut.modifiers || !shortcut.key) return false;
            result = shortcut;
            return true;
        }
        const UINT modifier = token == L"CTRL" || token == L"CONTROL" ? MOD_CONTROL :
                              token == L"ALT" ? MOD_ALT :
                              token == L"SHIFT" ? MOD_SHIFT :
                              token == L"WIN" ? MOD_WIN : 0;
        if (!modifier || (shortcut.modifiers & modifier)) return false;
        shortcut.modifiers |= modifier;
        text.remove_prefix(split + 1);
    }
}

std::wstring GetSetting(PCWSTR key) {
    PCWSTR value = Wh_GetStringSetting(key);
    if (!value) return {};
    std::wstring result(value);
    Wh_FreeStringSetting(value);
    return result;
}

std::wstring ExpandPath(std::wstring_view value) {
    value = Trim(value);
    if (value.size() >= 2 && value.front() == L'"' && value.back() == L'"') {
        value.remove_prefix(1);
        value.remove_suffix(1);
    }
    if (value.empty() || value.find(L'"') != std::wstring_view::npos) return {};
    const std::wstring raw(value);
    DWORD needed = ExpandEnvironmentStringsW(raw.c_str(), nullptr, 0);
    if (!needed || needed > 32767) return {};
    std::wstring expanded(needed, L'\0');
    const DWORD written = ExpandEnvironmentStringsW(raw.c_str(), expanded.data(), needed);
    if (!written || written > needed) return {};
    expanded.resize(written - 1);
    return expanded;
}

bool IsFile(const std::wstring& path, std::wstring_view extension) {
    if (path.size() <= extension.size() || path.size() < 4 || path[1] != L':' ||
        (path[2] != L'\\' && path[2] != L'/') || path.find(L'"') != std::wstring::npos ||
        CompareStringOrdinal(path.data() + path.size() - extension.size(),
                             static_cast<int>(extension.size()), extension.data(),
                             static_cast<int>(extension.size()), TRUE) != CSTR_EQUAL) return false;
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

void FocusGame(DWORD pid) {
    EnumWindows([](HWND hwnd, LPARAM data) -> BOOL {
        DWORD windowPid{};
        GetWindowThreadProcessId(hwnd, &windowPid);
        if (windowPid != static_cast<DWORD>(data) || !IsWindowVisible(hwnd) ||
            GetWindow(hwnd, GW_OWNER)) return TRUE;
        if (IsIconic(hwnd)) ShowWindowAsync(hwnd, SW_RESTORE);
        SetForegroundWindow(hwnd);
        return FALSE;
    }, static_cast<LPARAM>(pid));
}

void LaunchGame(HANDLE& game) {
    if (game) {
        if (WaitForSingleObject(game, 0) == WAIT_TIMEOUT) {
            FocusGame(GetProcessId(game));
            return;
        }
        CloseHandle(game);
        game = nullptr;
    }

    const auto engineSetting = GetSetting(L"enginePath");
    const auto engine = ExpandPath(engineSetting);
    const auto wadSetting = GetSetting(L"iwadPath");
    const auto wad = ExpandPath(wadSetting);
    if (!IsFile(engine, L".exe")) {
        Wh_Log(L"Set Doom executable to the full path of a local Doom engine executable.");
        return;
    }
    if (!Trim(wadSetting).empty() && !IsFile(wad, L".wad")) {
        Wh_Log(L"Game WAD must point to an existing local .wad file, or be empty.");
        return;
    }

    const auto directory = engine.substr(0, engine.find_last_of(L"\\/") + 1);
    std::wstring command = L"\"" + engine + L"\"";
    if (!wad.empty()) command += L" -iwad \"" + wad + L"\"";
    command += L" -fullscreen";
    STARTUPINFOW startup{sizeof(startup)};
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(engine.c_str(), command.data(), nullptr, nullptr, FALSE, 0,
                        nullptr, directory.c_str(), &startup, &process)) {
        Wh_Log(L"Doom could not start (Windows error %lu).", GetLastError());
        return;
    }
    CloseHandle(process.hThread);
    game = process.hProcess;
}

HANDLE g_stop{};
HANDLE g_reload{};
HANDLE g_thread{};

DWORD WINAPI RunHotkey(LPVOID) {
    MSG message{};
    PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
    const HANDLE events[]{g_stop, g_reload};
    HANDLE game{};
    Shortcut current;
    int registeredId = 0;
    bool waitingForShell = true;
    bool apply = true;

    while (WaitForSingleObject(g_stop, 0) == WAIT_TIMEOUT) {
        if (waitingForShell) {
            DWORD shellPid{};
            GetWindowThreadProcessId(GetShellWindow(), &shellPid);
            if (shellPid && shellPid != GetCurrentProcessId()) break;
            waitingForShell = shellPid == 0;
        }
        if (apply && !waitingForShell) {
            apply = false;
            Shortcut desired;
            if (!ParseShortcut(GetSetting(L"shortcut"), desired)) {
                Wh_Log(L"Invalid Doom shortcut. Use a modifier and a letter, number or function key.");
            } else if (desired != current) {
                const int nextId = registeredId == 1 ? 2 : 1;
                if (RegisterHotKey(nullptr, nextId, desired.modifiers | MOD_NOREPEAT, desired.key)) {
                    if (registeredId) UnregisterHotKey(nullptr, registeredId);
                    registeredId = nextId;
                    current = desired;
                } else {
                    Wh_Log(L"Could not register the Doom shortcut (Windows error %lu).", GetLastError());
                }
            }
        }

        const DWORD signal = MsgWaitForMultipleObjects(ARRAYSIZE(events), events, FALSE,
                                                       waitingForShell ? 500 : INFINITE, QS_ALLINPUT);
        if (signal == WAIT_OBJECT_0 || signal == WAIT_FAILED) break;
        if (signal == WAIT_OBJECT_0 + 1) apply = true;
        if (signal == WAIT_OBJECT_0 + ARRAYSIZE(events)) {
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (WaitForSingleObject(g_stop, 0) != WAIT_TIMEOUT) break;
                if (registeredId && message.message == WM_HOTKEY && message.hwnd == nullptr &&
                    message.wParam == static_cast<WPARAM>(registeredId) &&
                    LOWORD(message.lParam) == current.modifiers && HIWORD(message.lParam) == current.key) {
                    LaunchGame(game);
                }
            }
        }
    }

    if (registeredId) UnregisterHotKey(nullptr, registeredId);
    if (game) CloseHandle(game);
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_reload = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_stop && g_reload) g_thread = CreateThread(nullptr, 0, RunHotkey, nullptr, 0, nullptr);
    if (g_thread) return TRUE;
    if (g_stop) CloseHandle(g_stop);
    if (g_reload) CloseHandle(g_reload);
    return FALSE;
}

void Wh_ModSettingsChanged() {
    SetEvent(g_reload);
}

void Wh_ModBeforeUninit() {
    SetEvent(g_stop);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_reload);
    CloseHandle(g_stop);
}
