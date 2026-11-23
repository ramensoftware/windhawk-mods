// ==WindhawkMod==
// @id              set-super-topmost
// @name            Super Topmost Windows
// @description     Show ordinary windows on top of almost all apps and games
// @version         1.0
// @author          Ingan121
// @github          https://github.com/Ingan121
// @twitter         https://twitter.com/Ingan121
// @homepage        https://www.ingan121.com/
// @include         ctfmon.exe
// @compilerOptions -lwtsapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Super Topmost Windows
* This mod allows pinning any window on top of immersive UI components (Action Center, Start Menu, search, volume HUD, taskbar flyouts, etc.), fullscreen UWP apps, games in borderless or optimized fullscreen mode, Task Manager, and any other ordinary "Always On Top" windows.
* Press the configured hotkey (default: `Win+Ctrl+Shift+T`) to toggle the super-topmost state of the current foreground window.
* This mod absolutely requires the UIAccess privilege, so it may not work with a portable Windhawk installation running without admin.
* Known issue: It won't work with UWP windows.
* Fullscreen games might minimize when focusing the topmost window. You will need a workaround, such as the [Ignore Focus Loss](https://windhawk.net/mods/ignore-focus-loss) mod.
## ⚠ Important security notice ⚠
* Windows set as super-topmost by this mod may **show on top of Windows 10/11's default lock screen, which might expose sensitive information or allow logon bypass.**
* This mod tries its best to avoid this issue by temporarily restoring all super-topmost windows to the original state while locked, but it could go wrong (e.g., the mod process dies with super-topmost windows left).
* If this concerns you, either do not use this mod, or disable the lock screen entirely ([use the NoLockScreen registry/GPO](https://www.elevenforum.com/t/enable-or-disable-lock-screen-in-windows-11.1287/) or rename `LockApp.exe`)
## Hotkey
Change the hotkey in the mod's settings. Write any modifiers and exactly one
key, joined by `+`, for example `Ctrl+Alt+M`, `Win+Shift+F9` or `Pause`.

* Modifiers: `Ctrl`, `Alt`, `Shift`, `Win`.
* Keys: `A`-`Z`, `0`-`9`, `F1`-`F24`, `Space`, `Tab`, `Enter`, `Esc`,
  `Backspace`, `Insert`, `Delete`, `Home`, `End`, `PageUp`, `PageDown`, `Up`,
  `Down`, `Left`, `Right`, `Pause`, `ScrollLock`, `PrintScreen`, `Numpad0`-
  `Numpad9`, `VolumeMute`, `VolumeDown`, `VolumeUp`, `MediaPlayPause`,
  `MediaNext`, `MediaPrev`, `MediaStop`.
* Any other key: its virtual-key code in hexadecimal, such as `0x4D`.

Windows gives a hotkey to only one program at a time, and other apps stop
receiving that key combination. If the hotkey does nothing, another program may
already own it. Turn on logging in the mod's Advanced tab to see why.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/* 
- hotkey: "Win+Ctrl+Shift+T"
  $name: Hotkey
  $name:ko-KR: 핫키
  $description: Refer to the mod details for the hotkey syntax.
  $description:ko-KR: 모드 설명에서 핫키 구문을 참조하세요.
*/
// ==/WindhawkModSettings==

// This mod is a continuation of my old ass utility that nobody would remember: https://github.com/Ingan121/Overlayer
// It retained SetParent'd windows inside the overlay window till exit, instead of undocking it right away
// IIRC the double SetParent trick worked then (6 years ago) as well, and it still works on LTSC 2021 and 24H2

// Another possible way for changing the window band is using the long and tedious code to directly call SetWindowBand
// (https://gist.github.com/ADeltaX/80d220579400a95c336af0eec372ecb0)
// But that needs hooking the main explorer.exe to steal the immersive token, which makes managing admin windows impossible (without disabling UAC)
// The immersive token retrieval code is unreliable too (I tried that)

#include <windhawk_utils.h>
#include <optional>
#include <vector>
#include <wtsapi32.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#define OVERLAY_WIN_CLASS L"OverlayerContainer_{fbcacd1a-cc62-4c57-940f-8c10f83dd7a6}"
#define WM_APP_RELOAD (WM_APP + 1)

#define HK_DIRECT 1

struct StWindow {
    HWND hWnd;
    bool wasTopmost;
};

std::vector<StWindow> g_stWindows;

enum ZBID {
    ZBID_DEFAULT,
    ZBID_DESKTOP,
    ZBID_UIACCESS
};

typedef BOOL(WINAPI* GetWindowBand_t)(IN HWND hWnd, OUT PDWORD pdwBand);
GetWindowBand_t GetWindowBand;

// Hotkey logic mostly taken from https://github.com/ramensoftware/windhawk-mods/blob/5aba357fee3c0842e0bcb73d5ce770268466b8cb/mods/mute-focused-window.wh.cpp#L127 because I'm lazy
struct Hotkey {
    UINT modifiers;  // MOD_* flags, without MOD_NOREPEAT.
    UINT virtualKey;
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

void LoadSettings(HWND hwndListener) {
    UnregisterHotKey(hwndListener, HK_DIRECT);
    auto hotkeyStr = WindhawkUtils::StringSetting::make(L"hotkey");
    if (std::optional<Hotkey> hotkey = ParseHotkey(hotkeyStr.get())) {
        if (!RegisterHotKey(hwndListener, HK_DIRECT,
                            hotkey->modifiers | MOD_NOREPEAT,
                            hotkey->virtualKey)) {
            Wh_Log(L"Can't register hotkey %s, error %u", hotkeyStr.get(),
                   GetLastError());
        }
    } else {
        Wh_Log(L"Invalid hotkey: %s", hotkeyStr.get());
    }
}

void ToggleAllManagedWindows(HWND hOverlay, bool setSuperTopMost) {
    SetWindowPos(hOverlay, setSuperTopMost ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    for (StWindow stWnd : g_stWindows) {
        if (IsWindow(stWnd.hWnd)) {
            SetParent(stWnd.hWnd, hOverlay);
        }
    }
    SetWindowPos(hOverlay, setSuperTopMost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    for (StWindow stWnd : g_stWindows) {
        if (IsWindow(stWnd.hWnd)) {
            SetParent(stWnd.hWnd, NULL);
            if (!setSuperTopMost) {
                SetWindowPos(stWnd.hWnd, stWnd.wasTopmost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            }
        }
    }
}

LRESULT WINAPI OverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            Wh_Log(L"OverlayWndProc WM_CREATE");
            if (!WTSRegisterSessionNotification(hWnd, NOTIFY_FOR_THIS_SESSION)) {
                Wh_Log(L"WTSRegisterSessionNotification failed, GLE=%d", GetLastError());
            }
            return 0;
        case WM_HOTKEY:
            switch (wParam) {
                case HK_DIRECT: {
                    Wh_Log(L"HK_DIRECT");
                    HWND hwndForeground = GetForegroundWindow();
                    DWORD dwBand = ZBID_DEFAULT;
                    if (GetWindowBand(hwndForeground, &dwBand)) {
                        bool isOnUIAccessBand = dwBand == ZBID_UIACCESS;
                        if (dwBand != ZBID_DESKTOP && !isOnUIAccessBand) {
                            // We can't touch bands other than those two so ignore windows on other bands
                            return 0;
                        }
                        // UIAccess processes can freely set their windows' band between ZBID_DESKTOP and ZBID_UIACCESS simply by setting their topmost flag
                        // SetParent only works with parent window on the same band, so start by making our temp window belong in the same band
                        // Then move the parent window to the desired band with the topmost trick, then restore the SetParent'd window's original parent (desktop)
                        // The window still retains the modified band!
                        bool wasTopmost = (GetWindowLongW(hwndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
                        SetWindowPos(hWnd, isOnUIAccessBand ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                        SetParent(hwndForeground, hWnd);
                        SetWindowPos(hWnd, isOnUIAccessBand ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                        SetParent(hwndForeground, NULL);
                        if (isOnUIAccessBand) { // 'was'OnUIAccessBand
                            std::erase_if(g_stWindows, [=](StWindow stWnd) {
                                if (stWnd.hWnd == hwndForeground) {
                                    SetWindowPos(hwndForeground, stWnd.wasTopmost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                                    return true;
                                }
                                return false;
                            });
                        } else {
                            g_stWindows.push_back({ hwndForeground, wasTopmost });
                        }
                    }
                }
            }
            break;
        case WM_WTSSESSION_CHANGE:
            switch (wParam) {
                case WTS_SESSION_LOCK:
                    Wh_Log(L"Locked");
                    ToggleAllManagedWindows(hWnd, false);
                    break;
                case WTS_SESSION_UNLOCK:
                    Wh_Log(L"Unlocked");
                    ToggleAllManagedWindows(hWnd, true);
                    break;
            }
            break;
        case WM_APP_RELOAD:
            LoadSettings(hWnd);
            break;
        case WM_CLOSE:
            ToggleAllManagedWindows(hWnd, false);
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            Wh_Log(L"OverlayWndProc WM_DESTROY");
            WTSUnRegisterSessionNotification(hWnd);
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

HWND CreateOverlayWindow() {
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = HINST_THISCOMPONENT;
    wc.lpszClassName = OVERLAY_WIN_CLASS;
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed, GLE=%d", GetLastError());
        return NULL;
    }
    return CreateWindowExW(WS_EX_TOOLWINDOW, OVERLAY_WIN_CLASS, L"Overlayer", WS_POPUP, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
}

HANDLE g_stopEvent;

DWORD WINAPI HotkeyThreadProc(LPVOID lpParameter) {
    if (!g_stopEvent) {
        return 1;
    }

    if (!SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
    {
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
    }

    HWND overlayWnd = CreateOverlayWindow();
    LoadSettings(overlayWnd);

    // Unfortunately, monitor off APIs are all asynchronous and there is no reliable way to determine when the monitor is actually off
    // If we immediately restore the gamma right after calling the API, the screen will briefly flash back to normal brightness before turning off
    // On the other side, waiting enough seconds and setting the gamma ramps after screens are turned off, has random failures that causes the screen to be stuck at black
    // So just show an black overlay window between the fade and the monitor off to cover up the potential flash, and hide the overlay after a fixed amount of time
    MSG msg;
    if (overlayWnd) {
        HANDLE waitHandles[1] = { g_stopEvent };
        while (TRUE) {
            DWORD result = MsgWaitForMultipleObjects(
                1,
                waitHandles,
                FALSE,
                INFINITE,
                QS_ALLINPUT
            );
            if (result == WAIT_OBJECT_0) {
                UnregisterHotKey(overlayWnd, HK_DIRECT);
                DestroyWindow(overlayWnd);
                UnregisterClassW(OVERLAY_WIN_CLASS, HINST_THISCOMPONENT);
                return 0;
            } else if (result == WAIT_OBJECT_0 + 1) {
                while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                    if (msg.message == WM_QUIT) {
                        UnregisterHotKey(overlayWnd, HK_DIRECT);
                        UnregisterClassW(OVERLAY_WIN_CLASS, HINST_THISCOMPONENT);
                        return 0;
                    }
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            } else {
                Wh_Log(L"MsgWaitForMultipleObjects returned %d", result);
            }
        }
    } else {
        Wh_Log(L"Failed to create overlay window, GLE=%d", GetLastError());
    }
    UnregisterHotKey(overlayWnd, HK_DIRECT);
    UnregisterClassW(OVERLAY_WIN_CLASS, HINST_THISCOMPONENT);
    return 1;
}

HANDLE g_hThread;
DWORD g_threadId;

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) {
        Wh_Log(L"GetModuleHandleW user32.dll failed");
        return FALSE;
    }
    GetWindowBand = (GetWindowBand_t)GetProcAddress(hUser32, "GetWindowBand");
    if (!GetWindowBand) {
        Wh_Log(L"GetProcAddress GetWindowBand failed");
        return FALSE;
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEventW failed");
        return FALSE;
    }
    g_hThread = CreateThread(NULL, 0, HotkeyThreadProc, (LPVOID)0, 0, &g_threadId);
    if (!g_hThread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void WhTool_ModUninit() {
    HWND hWnd = FindWindowW(OVERLAY_WIN_CLASS, L"Overlayer");
    if (hWnd) {
        PostMessageW(hWnd, WM_CLOSE, 0, 0);
    } else {
        SetEvent(g_stopEvent);
    }
    WaitForSingleObject(g_hThread, INFINITE);
    CloseHandle(g_hThread);
    CloseHandle(g_stopEvent);
}

void WhTool_ModSettingsChanged() {
    PostMessageW(FindWindowW(OVERLAY_WIN_CLASS, L"Overlayer"), WM_APP_RELOAD, 0, 0);
}

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
    commandLine[(sizeof(L"-tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR))];
    swprintf_s(commandLine, L"-tool-mod \"%s\"", WH_MOD_ID);

    // Neither CreateProcessInternalW (used by the original WH wiki boilerplate) nor CreateProcessW can create a UIAccess child
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"open";
    sei.lpFile = currentProcessPath;
    sei.nShow = SW_SHOWNORMAL;
    sei.lpParameters = commandLine;
    ShellExecuteExW(&sei);
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
