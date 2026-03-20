// ==WindhawkMod==
// @id              scroll-window-opacity
// @name            Scroll Window Opacity
// @description     Hold a key combination and scroll the mouse wheel to change the opacity of the window under the cursor
// @version         1.0.2
// @author          Sondre Myrmel
// @github          https://github.com/Sondre234
// @twitter         https://x.com/ShaolinLoL
// @include         windhawk.exe
// @architecture    x86
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Scroll Window Opacity

Hold a configurable key combination (default: **Ctrl + Alt**) and scroll the mouse
wheel to adjust the transparency of the top-level window under the cursor.

- **Scroll up** — decrease opacity (more transparent)
- **Scroll down** — increase opacity (more opaque)

The opacity step, minimum opacity, and modifier keys are all configurable in the
settings panel.

Common shell windows such as the taskbar and desktop are excluded. For windows
that this mod makes layered, the original window style is restored when returned
to 100% opacity.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifierKey:
  - value: ctrl+alt
    $name: Ctrl + Alt
  - value: ctrl+shift
    $name: Ctrl + Shift
  - value: alt+shift
    $name: Alt + Shift
  - value: ctrl
    $name: Ctrl only
  - value: alt
    $name: Alt only
  - value: shift
    $name: Shift only
  $name: Modifier Key(s)
  $description: Key(s) to hold while scrolling the mouse wheel to change opacity

- opacityStep: 1
  $name: Opacity Step (%)
  $description: How much opacity changes per wheel notch (1–50)

- minOpacity: 10
  $name: Minimum Opacity (%)
  $description: Lowest opacity a window can be set to (1–90)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>

#include <cstdlib>
#include <mutex>
#include <string>
#include <unordered_map>

struct Settings {
    bool needCtrl  = true;
    bool needAlt   = true;
    bool needShift = false;
    int  step      = 1;
    int  minPct    = 10;
} g_settings;

void LoadSettings() {
    PCWSTR raw = Wh_GetStringSetting(L"modifierKey");
    std::wstring key = (raw && raw[0] != L'\0') ? raw : L"ctrl+alt";
    Wh_FreeStringSetting(raw);

    g_settings.needCtrl  = key.find(L"ctrl")  != std::wstring::npos;
    g_settings.needAlt   = key.find(L"alt")   != std::wstring::npos;
    g_settings.needShift = key.find(L"shift") != std::wstring::npos;

    if (!g_settings.needCtrl && !g_settings.needAlt && !g_settings.needShift) {
        g_settings.needCtrl = true;
        g_settings.needAlt  = true;
    }

    int step = Wh_GetIntSetting(L"opacityStep");
    g_settings.step = (step >= 1 && step <= 50) ? step : 1;

    int minPct = Wh_GetIntSetting(L"minOpacity");
    g_settings.minPct = (minPct >= 1 && minPct <= 90) ? minPct : 10;

    Wh_Log(L"Settings: ctrl=%d alt=%d shift=%d step=%d minPct=%d",
           g_settings.needCtrl,
           g_settings.needAlt,
           g_settings.needShift,
           g_settings.step,
           g_settings.minPct);
}

struct WindowState {
    int  pct;
    bool hadLayered;
};

std::unordered_map<HWND, WindowState> g_states;
std::mutex g_mutex;

static bool IsModifierHeld() {
    if (g_settings.needCtrl  && !(GetAsyncKeyState(VK_CONTROL) & 0x8000)) return false;
    if (g_settings.needAlt   && !(GetAsyncKeyState(VK_MENU)    & 0x8000)) return false;
    if (g_settings.needShift && !(GetAsyncKeyState(VK_SHIFT)   & 0x8000)) return false;
    return true;
}

static bool IsSystemWindow(HWND hwnd) {
    WCHAR cls[256] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    return _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(cls, L"Progman") == 0 ||
           _wcsicmp(cls, L"WorkerW") == 0 ||
           _wcsicmp(cls, L"DV2ControlHost") == 0 ||
           _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
}

static HWND GetRootWindowAt(POINT pt) {
    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd) {
        return nullptr;
    }

    HWND root = GetAncestor(hwnd, GA_ROOT);
    return root ? root : hwnd;
}

static bool AdjustOpacity(HWND hwnd, int delta) {
    if (!hwnd || !IsWindow(hwnd) || IsSystemWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_mutex);

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

    auto it = g_states.find(hwnd);
    if (it == g_states.end()) {
        bool hadLayered = (exStyle & WS_EX_LAYERED) != 0;
        int curPct = 100;

        if (hadLayered) {
            BYTE alpha = 255;
            DWORD flags = 0;
            if (GetLayeredWindowAttributes(hwnd, nullptr, &alpha, &flags) &&
                (flags & LWA_ALPHA)) {
                curPct = (int)((alpha * 100) / 255);
                if (curPct < 1) curPct = 1;
                if (curPct > 100) curPct = 100;
            }
        }

        g_states[hwnd] = {curPct, hadLayered};
        it = g_states.find(hwnd);
    }

    int oldPct = it->second.pct;
    int newPct = oldPct + delta;

    if (newPct < g_settings.minPct) newPct = g_settings.minPct;
    if (newPct > 100) newPct = 100;

    if (newPct == oldPct) {
        return false;
    }

    if (newPct >= 100 && !it->second.hadLayered) {
        SetLastError(ERROR_SUCCESS);
        LONG_PTR result = SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        if (result == 0 && GetLastError() != ERROR_SUCCESS) {
            Wh_Log(L"SetWindowLongPtr(remove layered) failed for 0x%p: %u",
                   (void*)hwnd, GetLastError());
            return false;
        }

        g_states.erase(it);
        Wh_Log(L"Restored window 0x%p to full opacity", (void*)hwnd);
        return true;
    }

    BYTE alpha = (BYTE)((newPct * 255) / 100);

    if (!(exStyle & WS_EX_LAYERED)) {
        SetLastError(ERROR_SUCCESS);
        LONG_PTR result = SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        if (result == 0 && GetLastError() != ERROR_SUCCESS) {
            Wh_Log(L"SetWindowLongPtr(add layered) failed for 0x%p: %u",
                   (void*)hwnd, GetLastError());
            return false;
        }
    }

    if (!SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA)) {
        Wh_Log(L"SetLayeredWindowAttributes failed for 0x%p: %u",
               (void*)hwnd, GetLastError());
        return false;
    }

    it->second.pct = newPct;
    Wh_Log(L"Window 0x%p opacity -> %d%%", (void*)hwnd, newPct);
    return true;
}

static void RestoreAllWindows() {
    std::lock_guard<std::mutex> lock(g_mutex);

    for (auto& [hwnd, state] : g_states) {
        if (!IsWindow(hwnd)) continue;

        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

        if (state.hadLayered) {
            if (!SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA)) {
                Wh_Log(L"Restore alpha failed for 0x%p: %u",
                       (void*)hwnd, GetLastError());
            }
        } else {
            SetLastError(ERROR_SUCCESS);
            LONG_PTR result = SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
            if (result == 0 && GetLastError() != ERROR_SUCCESS) {
                Wh_Log(L"Restore style failed for 0x%p: %u",
                       (void*)hwnd, GetLastError());
            }
        }
    }

    g_states.clear();
}

HHOOK  g_mouseHook      = nullptr;
HANDLE g_hookThread     = nullptr;
DWORD  g_hookThreadId   = 0;
int    g_wheelRemainder = 0;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL && IsModifierHeld()) {
        auto* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

        g_wheelRemainder += GET_WHEEL_DELTA_WPARAM(ms->mouseData);

        int notches = g_wheelRemainder / WHEEL_DELTA;
        g_wheelRemainder %= WHEEL_DELTA;

        if (notches != 0) {
            int delta = (notches > 0 ? -1 : 1) * g_settings.step * std::abs(notches);

            HWND target = GetRootWindowAt(ms->pt);
            if (AdjustOpacity(target, delta)) {
                return 1;
            }
        }
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
    if (!g_mouseHook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
        return 1;
    }

    Wh_Log(L"Low-level mouse hook installed");

    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnhookWindowsHookEx(g_mouseHook);
    g_mouseHook = nullptr;
    Wh_Log(L"Low-level mouse hook removed");
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Scroll Window Opacity: init");
    LoadSettings();

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Scroll Window Opacity: settings changed");
    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"Scroll Window Opacity: uninit");

    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }

    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, 5000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }

    RestoreAllWindows();
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
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

    WCHAR commandLine[MAX_PATH + 2 +
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
