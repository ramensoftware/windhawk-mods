// ==WindhawkMod==
// @id              desktop-icons-toggle
// @name            Desktop Icons Toggle
// @description     Toggle desktop icons with a tray button or Ctrl+Alt+H
// @version         1.0.3
// @author          merpzzz
// @github          https://github.com/merpzzz
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icons Toggle

Quickly show or hide desktop icons without affecting your running windows or
deleting files. Left-click the tray icon or press **Ctrl+Alt+H** to toggle;
if that hotkey is taken, **Ctrl+Alt+F12** is tried instead. Right-click the
tray icon for explicit **Show** and **Hide** actions.

Unlike broad keyboard automation or desktop double-click/automatic-hide mods,
this one focuses on a dedicated, always-available tray control with an optional
keyboard shortcut and an explicit Show/Hide menu. The hotkey can be changed
in Windhawk's Settings tab (default **Ctrl+Alt+H**); set it to **None** to
use the tray button alone.

## Saved Windows setting and recovery

The toggle uses Explorer's own "Show desktop icons" command. **That command
changes Windows' saved desktop icon visibility preference.** When the mod
unloads normally, it restores the visibility observed on startup, provided the
mod's last change is still in effect.

To recover from Explorer crashes, sign-out, restarts, and unexpected exits,
the mod stores the previous visibility before changing the setting. It checks
this record on its next start and restores the prior preference when the
current preference still matches the mod's change. A change made separately
by the user is respected if detected.

Icons might remain hidden until the next successful mod start. If you uninstall
it or it does not restart, desktop right-click > View > Show desktop icons
can manually restore visibility.

The tray and hotkey run in a dedicated Windhawk tool process, not Explorer.
No network, telemetry, downloads, keylogging or elevated helpers are used.
Initial target: Windows 11. Windows Explorer internals may change.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey: Ctrl+Alt+H
  $name: Toggle hotkey
  $description: "Examples: Ctrl+Alt+H, Ctrl+Shift+K, Alt+Shift+F8, Ctrl+Alt+1. Enter None to disable the hotkey. If the default is occupied, Ctrl+Alt+F12 is tried."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <cwchar>
#include <atomic>
#include <cstdlib>
#include <string>
#include <cwctype>
#include <vector>
#include <sddl.h>

namespace {
constexpr wchar_t kClass[] = L"Merpzzz.DesktopIconsToggle.Tray";
constexpr UINT kTray = WM_APP + 1;
constexpr UINT kRebind = WM_APP + 2;
constexpr UINT_PTR kRefresh = 1;
HANDLE stopEvent = nullptr;
HANDLE worker = nullptr;
std::atomic<HWND> workerWindow{nullptr};
HINSTANCE module = nullptr;
UINT taskbarCreated = 0;
NOTIFYICONDATAW tray{};
int initialVisible = -1, lastApplied = -1;
int lastUiVisibility = -2;
bool recoveryChecked = false;
std::wstring recoveryPending;
std::wstring recoveryOriginal;
std::wstring recoveryApplied;
std::wstring shortcut = L"Tray button only";
HICON shownIcon = nullptr, hiddenIcon = nullptr;

// Win32 enumeration requires CALLBACK (__stdcall on 32-bit Windows).
// A plain lambda has an incompatible calling convention on x86.
BOOL CALLBACK FindDesktopWorker(HWND window, LPARAM param) {
    wchar_t className[32]{};
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        lstrcmpW(className, L"WorkerW") != 0)
        return TRUE;
    HWND candidate = FindWindowExW(
        window, nullptr, L"SHELLDLL_DefView", nullptr);
    if (!candidate) return TRUE;
    *reinterpret_cast<HWND*>(param) = candidate;
    return FALSE;
}

HWND FindView() {
    HWND shell = GetShellWindow();
    if (!shell) return nullptr;
    HWND view = FindWindowExW(shell, nullptr, L"SHELLDLL_DefView", nullptr);
    if (view) return view;

    DWORD shellThreadId = GetWindowThreadProcessId(shell, nullptr);
    if (!shellThreadId) return nullptr;
    EnumThreadWindows(shellThreadId, FindDesktopWorker,
                      reinterpret_cast<LPARAM>(&view));
    return view;
}

// Windhawk's mod storage is machine-wide; Explorer's setting is per-user.
// The SID prevents a pending recovery for user A from modifying user B's
// desktop. If the token cannot be read, don't start the mod.
bool InitializeRecoveryKeys() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
        return false;

    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    std::vector<BYTE> buffer(size);
    bool ok = size && GetTokenInformation(
        token, TokenUser, buffer.data(), size, &size);
    CloseHandle(token);
    if (!ok) return false;

    LPWSTR sid = nullptr;
    if (!ConvertSidToStringSidW(
            reinterpret_cast<TOKEN_USER*>(buffer.data())->User.Sid, &sid))
        return false;

    const std::wstring suffix = std::wstring(L"_") + sid;
    LocalFree(sid);
    recoveryPending = L"recoveryPending" + suffix;
    recoveryOriginal = L"recoveryOriginal" + suffix;
    recoveryApplied = L"recoveryApplied" + suffix;
    return true;
}

// Registry value is Explorer's saved preference, not transient window
// visibility. This stays correct while Explorer creates/restarts its view
// or while another utility hides the ListView for rendering.
int SavedVisibility() {
    DWORD hide = 0;
    DWORD size = sizeof(hide);
    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        L"HideIcons", RRF_RT_REG_DWORD, nullptr, &hide, &size);
    if (status == ERROR_FILE_NOT_FOUND) return 1;
    if (status != ERROR_SUCCESS) return -1;
    return hide ? 0 : 1;
}

// Explorer command 0x7402 flips the saved preference. Never send based on
// IsWindowVisible, and avoid sending before the desktop is ready.
bool SendExplorerToggle(bool visible) {
    int current = SavedVisibility();
    if (current < 0) return false;
    if (current == static_cast<int>(visible)) return true;

    HWND view = FindView();
    if (!view || !IsWindowVisible(view)) return false;
    DWORD_PTR result = 0;
    if (!SendMessageTimeoutW(
            view, WM_COMMAND, 0x7402, 0,
            SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result))
        return false;
    return SavedVisibility() == static_cast<int>(visible);
}

// Replay only this user's pending change, and only if no newer choice
// has replaced it. The old unsuffixed records are deliberately ignored,
// because a global record may be from another user.
bool RecoverIfNeeded() {
    if (recoveryChecked) return true;
    int current = SavedVisibility();
    if (current < 0) return false;

    if (Wh_GetIntValue(recoveryPending.c_str(), 0)) {
        int original = Wh_GetIntValue(recoveryOriginal.c_str(), -1);
        int applied = Wh_GetIntValue(recoveryApplied.c_str(), -1);
        if (original >= 0 && original <= 1 &&
            applied >= 0 && applied <= 1 &&
            current == applied && current != original) {
            if (!SendExplorerToggle(original != 0)) return false;
            current = SavedVisibility();
            if (current != original) return false;
        }
        // If current != applied, respect an external setting change.
        if (!Wh_SetIntValue(recoveryPending.c_str(), 0)) return false;
        current = SavedVisibility();
        if (current < 0) return false;
    }

    initialVisible = current;
    recoveryChecked = true;
    return true;
}

bool SetVisible(bool visible) {
    if (!RecoverIfNeeded()) return false;
    int before = SavedVisibility();
    if (before < 0) return false;

    if (lastApplied >= 0 && before != lastApplied) {
        // External changes become the new baseline.
        if (!Wh_SetIntValue(recoveryPending.c_str(), 0)) return false;
        initialVisible = before;
        lastApplied = -1;
    }
    if (before == static_cast<int>(visible)) return true;
    if (initialVisible < 0) initialVisible = before;

    if (static_cast<int>(visible) != initialVisible) {
        // Write the rollback record before Explorer persists the change.
        if (!Wh_SetIntValue(recoveryOriginal.c_str(), initialVisible) ||
            !Wh_SetIntValue(recoveryApplied.c_str(), visible ? 1 : 0) ||
            !Wh_SetIntValue(recoveryPending.c_str(), 1))
            return false;
    }
    if (!SendExplorerToggle(visible)) return false;

    lastApplied = visible ? 1 : 0;
    if (lastApplied == initialVisible)
        Wh_SetIntValue(recoveryPending.c_str(), 0);
    return true;
}


HICON MakeIcon(bool hidden) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = 32;
    bi.bmiHeader.biHeight = -32;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    void* pixels = nullptr;
    HBITMAP bitmap = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &pixels, nullptr, 0);
    if (!bitmap) return nullptr;
    auto data = static_cast<DWORD*>(pixels);
    ZeroMemory(data, 32 * 32 * sizeof(DWORD));
    for (int y = 0; y < 32; ++y) for (int x = 0; x < 32; ++x) {
        bool square = ((x >= 5 && x < 14) || (x >= 18 && x < 27)) &&
                      ((y >= 5 && y < 14) || (y >= 18 && y < 27));
        if (square) data[y * 32 + x] = hidden ? 0xFF6E7D8C : 0xFF5ACDF5;
        if (hidden && x >= 4 && x < 28 && y >= 4 && y < 28 && std::abs(x + y - 31) <= 1)
            data[y * 32 + x] = 0xFFFFCD55;
    }
    BYTE maskBits[32 * 4]{};
    HBITMAP mask = CreateBitmap(32, 32, 1, 1, maskBits);
    ICONINFO info{TRUE, 0, 0, mask, bitmap};
    HICON icon = mask ? CreateIconIndirect(&info) : nullptr;
    if (mask) DeleteObject(mask);
    DeleteObject(bitmap);
    return icon;
}

// Parse Ctrl/Alt/Shift/Win plus one A-Z, 0-9 or F1-F12 key.
// Reject empty, duplicate, modifier-only, and unknown combinations.
bool ParseShortcut(const std::wstring& source, UINT& modifiers,
                   UINT& key, std::wstring& label) {
    std::wstring value;
    for (wchar_t ch : source) {
        if (!std::iswspace(ch))
            value.push_back(static_cast<wchar_t>(std::towupper(ch)));
    }
    modifiers = 0;
    key = 0;
    size_t start = 0;
    while (start < value.size()) {
        size_t end = value.find(L'+', start);
        if (end == std::wstring::npos) end = value.size();
        std::wstring token = value.substr(start, end - start);
        if (token.empty()) return false;
        UINT mod = 0;
        if (token == L"CTRL" || token == L"CONTROL") mod = MOD_CONTROL;
        else if (token == L"ALT") mod = MOD_ALT;
        else if (token == L"SHIFT") mod = MOD_SHIFT;
        else if (token == L"WIN" || token == L"WINDOWS") mod = MOD_WIN;
        if (mod) {
            if (key || (modifiers & mod)) return false;
            modifiers |= mod;
        } else {
            if (key || end != value.size() || modifiers == 0) return false;
            if (token.size() == 1 &&
                ((token[0] >= L'A' && token[0] <= L'Z') ||
                 (token[0] >= L'0' && token[0] <= L'9'))) {
                key = token[0];
            } else if (token.size() >= 2 && token[0] == L'F') {
                int number = 0;
                for (size_t i = 1; i < token.size(); i++) {
                    if (token[i] < L'0' || token[i] > L'9') return false;
                    number = number * 10 + token[i] - L'0';
                    if (number > 12) return false;
                }
                if (number < 1) return false;
                key = VK_F1 + number - 1;
            } else {
                return false;
            }
        }
        if (end == value.size()) break;
        start = end + 1;
    }
    if (!key || !(modifiers & (MOD_ALT | MOD_CONTROL | MOD_WIN)))
        return false;
    label.clear();
    if (modifiers & MOD_CONTROL) label += L"Ctrl+";
    if (modifiers & MOD_ALT) label += L"Alt+";
    if (modifiers & MOD_SHIFT) label += L"Shift+";
    if (modifiers & MOD_WIN) label += L"Win+";
    if (key >= VK_F1 && key <= VK_F12) {
        label += L"F" + std::to_wstring(key - VK_F1 + 1);
    } else {
        label.push_back(static_cast<wchar_t>(key));
    }
    return true;
}

void ReloadHotkey(HWND window) {
    UnregisterHotKey(window, 1);
    shortcut = L"Tray button only";
    PCWSTR setting = Wh_GetStringSetting(L"hotkey");
    std::wstring requested = setting ? setting : L"Ctrl+Alt+H";
    if (setting) Wh_FreeStringSetting(setting);

    std::wstring upper;
    for (wchar_t ch : requested) {
        if (!std::iswspace(ch))
            upper.push_back(static_cast<wchar_t>(std::towupper(ch)));
    }
    if (upper == L"NONE" || upper == L"OFF" ||
        upper == L"DISABLED") return;

    UINT modifiers = 0, key = 0;
    std::wstring label;
    if (!ParseShortcut(requested, modifiers, key, label)) {
        shortcut = L"Invalid hotkey (tray only)";
        Wh_Log(L"Invalid hotkey setting: %s", requested.c_str());
        return;
    }
    if (RegisterHotKey(window, 1, modifiers | MOD_NOREPEAT, key)) {
        shortcut = label;
    } else if (modifiers == (MOD_CONTROL | MOD_ALT) && key == L'H' &&
               RegisterHotKey(window, 1, MOD_CONTROL | MOD_ALT |
                              MOD_NOREPEAT, VK_F12)) {
        shortcut = L"Ctrl+Alt+F12";
    } else {
        shortcut = label + L" unavailable (tray only)";
        Wh_Log(L"Hotkey unavailable: %s", label.c_str());
    }
}

void Refresh(bool add = false) {
    int visible = -1;
    if (RecoverIfNeeded()) {
        visible = SavedVisibility();
        if (lastApplied >= 0 && visible >= 0 &&
            visible != lastApplied) {
            // A change external to this mod wins.
            initialVisible = visible;
            lastApplied = -1;
            Wh_SetIntValue(recoveryPending.c_str(), 0);
        }
    }

    if (!add && visible == lastUiVisibility) return;
    lastUiVisibility = visible;
    tray.hIcon = visible == 0 ? hiddenIcon : shownIcon;
    std::swprintf(tray.szTip, ARRAYSIZE(tray.szTip),
                  L"Desktop icons: %ls | %ls",
                  visible < 0 ? L"waiting" : visible ? L"shown" : L"hidden",
                  shortcut.c_str());
    Shell_NotifyIconW(add ? NIM_ADD : NIM_MODIFY, &tray);
}

LRESULT CALLBACK WindowProc(HWND h, UINT message, WPARAM w, LPARAM l) {
    if (message == taskbarCreated && taskbarCreated) { Refresh(true); return 0; }
    if (message == WM_TIMER && w == kRefresh) { Refresh(); return 0; }
    if (message == kRebind) {
        ReloadHotkey(h);
        lastUiVisibility = -2;
        Refresh();
        return 0;
    }
    if (message == WM_HOTKEY && w == 1) {
        int current = SavedVisibility();
        if (current >= 0) SetVisible(!current);
        Refresh(); return 0;
    }
    if (message == kTray) {
        if (l == WM_LBUTTONUP) {
            int current = SavedVisibility(); if (current >= 0) SetVisible(!current);
            Refresh();
        } else if (l == WM_RBUTTONUP || l == WM_CONTEXTMENU) {
            HMENU menu = CreatePopupMenu();
            if (!menu) return 0;
            int current = SavedVisibility();
            AppendMenuW(menu, MF_STRING | (current < 0 ? MF_GRAYED : 0), 1, L"Show desktop icons");
            AppendMenuW(menu, MF_STRING | (current < 0 ? MF_GRAYED : 0), 2, L"Hide desktop icons");
            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(menu, MF_STRING | MF_GRAYED, 3, shortcut.c_str());
            POINT p{}; GetCursorPos(&p); SetForegroundWindow(h);
            UINT command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                          p.x, p.y, 0, h, nullptr);
            DestroyMenu(menu); PostMessageW(h, WM_NULL, 0, 0);
            if (command == 1 || command == 2) SetVisible(command == 1);
            Refresh();
        }
        return 0;
    }
    return DefWindowProcW(h, message, w, l);
}

DWORD WINAPI Run(void*) {
    initialVisible = -1;
    lastApplied = -1;
    recoveryChecked = false;
    WNDCLASSW wc{}; wc.hInstance = module; wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = kClass;
    if (!RegisterClassW(&wc)) return 1;
    // A hidden top-level window receives TaskbarCreated after Explorer reinitializes.
    HWND h = CreateWindowExW(WS_EX_TOOLWINDOW, kClass, L"Desktop Icons Toggle", 0,
                             0, 0, 0, 0, nullptr, nullptr, module, nullptr);
    if (!h) { UnregisterClassW(kClass, module); return 2; }
    workerWindow = h;
    shownIcon = MakeIcon(false); hiddenIcon = MakeIcon(true);
    if (!shownIcon || !hiddenIcon) {
        if (shownIcon) DestroyIcon(shownIcon);
        if (hiddenIcon) DestroyIcon(hiddenIcon);
        workerWindow = nullptr; DestroyWindow(h); UnregisterClassW(kClass, module); return 3;
    }
    ReloadHotkey(h);
    tray = {}; tray.cbSize = sizeof(tray); tray.hWnd = h; tray.uID = 1;
    tray.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; tray.uCallbackMessage = kTray;
    Refresh(true); SetTimer(h, kRefresh, 1000, nullptr);
    bool done = false;
    while (!done) {
        DWORD wait = MsgWaitForMultipleObjects(1, &stopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (wait != WAIT_OBJECT_0 + 1) break;
        MSG msg{};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { done = true; break; }
            TranslateMessage(&msg); DispatchMessageW(&msg);
        }
    }
    KillTimer(h, kRefresh); UnregisterHotKey(h, 1); Shell_NotifyIconW(NIM_DELETE, &tray);
    if (recoveryChecked && initialVisible >= 0 && lastApplied >= 0 &&
        SavedVisibility() == lastApplied) {
        if (SendExplorerToggle(initialVisible != 0))
            Wh_SetIntValue(recoveryPending.c_str(), 0);
    }
    workerWindow = nullptr; DestroyWindow(h); DestroyIcon(shownIcon); DestroyIcon(hiddenIcon);
    UnregisterClassW(kClass, module); return 0;
}
}

BOOL WhTool_ModInit() {
    if (!InitializeRecoveryKeys()) {
        Wh_Log(L"Couldn't determine the current user's SID");
        return FALSE;
    }
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&WhTool_ModInit), &module))
        return FALSE;

    taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent) return FALSE;

    worker = CreateThread(nullptr, 0, Run, nullptr, 0, nullptr);
    if (!worker) {
        CloseHandle(stopEvent);
        stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (HWND h = workerWindow.load())
        PostMessageW(h, kRebind, 0, 0);
}

void WhTool_ModUninit() {
    if (HWND h = workerWindow.load())
        PostMessageW(h, WM_CANCELMODE, 0, 0);
    if (stopEvent) SetEvent(stopEvent);
    if (worker) {
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
        worker = nullptr;
    }
    if (stopEvent) {
        CloseHandle(stopEvent);
        stopEvent = nullptr;
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
