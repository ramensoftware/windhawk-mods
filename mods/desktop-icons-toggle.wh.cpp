// ==WindhawkMod==
// @id              desktop-icons-toggle
// @name            Desktop Icons Toggle
// @description     Toggle desktop icons with a tray button or Ctrl+Alt+H
// @version         1.0.1
// @author          merpzzz
// @github          https://github.com/merpzzz
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32
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
keyboard shortcut and an explicit Show/Hide menu.

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

#include <windows.h>
#include <shellapi.h>
#include <cwchar>
#include <atomic>
#include <cstdlib>

namespace {
constexpr wchar_t kClass[] = L"Merpzzz.DesktopIconsToggle.Tray";
constexpr UINT kTray = WM_APP + 1;
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
constexpr wchar_t kRecoveryPending[] = L"recoveryPending";
constexpr wchar_t kRecoveryOriginal[] = L"recoveryOriginal";
constexpr wchar_t kRecoveryApplied[] = L"recoveryApplied";
const wchar_t* shortcut = L"Tray button only";
HICON shownIcon = nullptr, hiddenIcon = nullptr;

HWND FindView() {
    HWND shell = GetShellWindow();
    if (!shell) return nullptr;
    HWND view = FindWindowExW(shell, nullptr, L"SHELLDLL_DefView", nullptr);
    if (view) return view;

    DWORD shellThreadId = GetWindowThreadProcessId(shell, nullptr);
    if (!shellThreadId) return nullptr;
    EnumThreadWindows(shellThreadId, [](HWND window, LPARAM param) -> BOOL {
        wchar_t className[32]{};
        if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
            lstrcmpW(className, L"WorkerW") != 0)
            return TRUE;
        HWND candidate = FindWindowExW(
            window, nullptr, L"SHELLDLL_DefView", nullptr);
        if (!candidate) return TRUE;
        *reinterpret_cast<HWND*>(param) = candidate;
        return FALSE;
    }, reinterpret_cast<LPARAM>(&view));
    return view;
}

int Visibility() {
    HWND view = FindView();
    HWND icons = view ? FindWindowExW(view, nullptr, L"SysListView32", nullptr) : nullptr;
    return icons ? (IsWindowVisible(icons) ? 1 : 0) : -1;
}

// Sends Explorer's command, which persists the chosen visibility.
bool SendExplorerToggle(bool visible) {
    int current = Visibility();
    if (current < 0) return false;
    if (current == static_cast<int>(visible)) return true;
    HWND view = FindView();
    DWORD_PTR result = 0;
    return view && SendMessageTimeoutW(
        view, WM_COMMAND, 0x7402, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK,
        1000, &result);
}

// Only capture the baseline after restoring any change from an earlier crash.
bool RecoverIfNeeded() {
    if (recoveryChecked) return true;
    int current = Visibility();
    if (current < 0) return false;

    if (Wh_GetIntValue(kRecoveryPending, 0)) {
        int original = Wh_GetIntValue(kRecoveryOriginal, -1);
        int applied = Wh_GetIntValue(kRecoveryApplied, -1);
        if (original >= 0 && original <= 1 &&
            applied >= 0 && applied <= 1 &&
            current == applied && current != original) {
            if (!SendExplorerToggle(original != 0)) return false;
        }
        // If current != applied, a user or Windows component changed it.
        // Never overwrite that newer setting.
        if (!Wh_SetIntValue(kRecoveryPending, 0)) return false;
        current = Visibility();
        if (current < 0) return false;
    }

    initialVisible = current;
    recoveryChecked = true;
    return true;
}

bool SetVisible(bool visible) {
    if (!RecoverIfNeeded()) return false;
    int before = Visibility();
    if (before < 0) return false;

    if (lastApplied >= 0 && before != lastApplied) {
        initialVisible = before;
        lastApplied = -1;
        Wh_SetIntValue(kRecoveryPending, 0);
    }

    if (before == static_cast<int>(visible)) return true;
    if (initialVisible < 0) initialVisible = before;

    if (static_cast<int>(visible) != initialVisible) {
        // Commit the recovery record *before* changing Explorer's setting.
        if (!Wh_SetIntValue(kRecoveryOriginal, initialVisible) ||
            !Wh_SetIntValue(kRecoveryApplied, visible ? 1 : 0) ||
            !Wh_SetIntValue(kRecoveryPending, 1))
            return false;
    }

    if (!SendExplorerToggle(visible)) return false;
    lastApplied = visible ? 1 : 0;
    if (lastApplied == initialVisible)
        Wh_SetIntValue(kRecoveryPending, 0);
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

void Refresh(bool add = false) {
    int visible = -1;
    if (RecoverIfNeeded()) {
        visible = Visibility();
        if (lastApplied >= 0 && visible >= 0 &&
            visible != lastApplied) {
            // A change external to this mod wins.
            initialVisible = visible;
            lastApplied = -1;
            Wh_SetIntValue(kRecoveryPending, 0);
        }
    }

    if (!add && visible == lastUiVisibility) return;
    lastUiVisibility = visible;
    tray.hIcon = visible == 0 ? hiddenIcon : shownIcon;
    std::swprintf(tray.szTip, ARRAYSIZE(tray.szTip),
                  L"Desktop icons: %ls | %ls",
                  visible < 0 ? L"waiting" : visible ? L"shown" : L"hidden",
                  shortcut);
    Shell_NotifyIconW(add ? NIM_ADD : NIM_MODIFY, &tray);
}

LRESULT CALLBACK WindowProc(HWND h, UINT message, WPARAM w, LPARAM l) {
    if (message == taskbarCreated && taskbarCreated) { Refresh(true); return 0; }
    if (message == WM_TIMER && w == kRefresh) { Refresh(); return 0; }
    if (message == WM_HOTKEY && w == 1) {
        int current = Visibility();
        if (current >= 0) SetVisible(!current);
        Refresh(); return 0;
    }
    if (message == kTray) {
        if (l == WM_LBUTTONUP) {
            int current = Visibility(); if (current >= 0) SetVisible(!current);
            Refresh();
        } else if (l == WM_RBUTTONUP || l == WM_CONTEXTMENU) {
            HMENU menu = CreatePopupMenu();
            if (!menu) return 0;
            int current = Visibility();
            AppendMenuW(menu, MF_STRING | (current < 0 ? MF_GRAYED : 0), 1, L"Show desktop icons");
            AppendMenuW(menu, MF_STRING | (current < 0 ? MF_GRAYED : 0), 2, L"Hide desktop icons");
            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(menu, MF_STRING | MF_GRAYED, 3, shortcut);
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
    shortcut = L"Tray button only";
    if (RegisterHotKey(h, 1, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'H')) shortcut = L"Ctrl+Alt+H";
    else if (RegisterHotKey(h, 1, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F12)) shortcut = L"Ctrl+Alt+F12";
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
        Visibility() == lastApplied) {
        if (SendExplorerToggle(initialVisible != 0))
            Wh_SetIntValue(kRecoveryPending, 0);
    }
    workerWindow = nullptr; DestroyWindow(h); DestroyIcon(shownIcon); DestroyIcon(hiddenIcon);
    UnregisterClassW(kClass, module); return 0;
}
}

BOOL WhTool_ModInit() {
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
    // No settings yet.
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
