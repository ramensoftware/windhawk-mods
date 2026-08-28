// ==WindhawkMod==
// @id              desktop-icons-toggle
// @name            Toggle Desktop Icons
// @description     Instantly show or hide desktop icons from a system tray icon or a global keyboard shortcut.
// @version         3.0
// @author          Aaron - KiivYx
// @github          https://github.com/KiivYx
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -ladvapi32 -lwtsapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Toggle Desktop Icons

Does what the Windows option "View -> Show desktop icons" does, but
**without** the context menu. It acts directly on the desktop's `SysListView32`
window (same as the native function).

This mod doesn't hook other processes, so it runs as a standalone tool in a
dedicated `windhawk.exe` process (it does not inject into `explorer.exe`).

## Ways to toggle

- **System tray icon**: left-click to show/hide the desktop icons. Right-click
  for a small menu. When the icons are hidden the tray icon shows a red mark.
- **Global keyboard shortcut** (default `Ctrl+Alt+D`).

## Usage

1. Install the mod.
2. A tray icon appears in the notification area. Left-click it to toggle, or use
   the keyboard shortcut.

When you disable the mod, the icons become visible again and the tray icon
disappears.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showIcons: true
  $name: Show desktop icons
  $description: Enables or disables the visibility of the desktop icons.
- hotkey: "Ctrl+Alt+D"
  $name: Toggle shortcut
  $description: >-
    Global combination. Examples: Ctrl+Alt+D, Win+I, Ctrl+Shift+F8. Leave it
    empty to disable the shortcut.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <wtsapi32.h>
#include <string>
#include <cwctype>
#include <cwchar>
#include <vector>

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------
#define WM_TRAYCB   (WM_APP + 1)
#define ID_TOGGLE   1
#define TRAY_UID    1

// ---------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------
static bool   g_settingShow = true;   // checkbox value
static bool   g_hidden      = false;  // runtime state
static UINT   g_mods        = 0;      // shortcut modifiers
static UINT   g_vk          = 0;      // shortcut key (0 = no shortcut)

static HANDLE g_thread      = nullptr;
static DWORD  g_threadId    = 0;
static HANDLE g_threadReady = nullptr;

static HWND   g_msgWnd          = nullptr;
static bool   g_classRegistered = false;
static UINT   g_taskbarCreated  = 0;
static HICON  g_iconShown       = nullptr;
static HICON  g_iconHidden      = nullptr;
static NOTIFYICONDATAW g_nid    = {};
static const wchar_t* kMsgClass = L"WhDesktopIconsToggleTray";

static void LogLastError(PCWSTR context) {
    DWORD err = GetLastError();
    PWSTR msg = nullptr;
    DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS;
    DWORD len = FormatMessageW(flags, nullptr, err,
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               (PWSTR)&msg, 0, nullptr);
    if (len && msg) {
        while (len && (msg[len - 1] == L'\r' || msg[len - 1] == L'\n')) {
            msg[--len] = 0;
        }
        Wh_Log(L"%s (err=%lu): %s", context, err, msg);
        LocalFree(msg);
    } else {
        Wh_Log(L"%s (err=%lu)", context, err);
    }
}

static bool CreateToolProcessAsActiveUser(PCWSTR applicationPath,
                                         PWSTR commandLine,
                                         PROCESS_INFORMATION* pi) {
    DWORD activeSession = WTSGetActiveConsoleSessionId();
    if (activeSession == 0xFFFFFFFF) {
        Wh_Log(L"WTSGetActiveConsoleSessionId returned 0xFFFFFFFF");
        return false;
    }

    HANDLE userToken = nullptr;
    if (!WTSQueryUserToken(activeSession, &userToken)) {
        LogLastError(L"WTSQueryUserToken failed");
        return false;
    }

    HANDLE primaryToken = nullptr;
    DWORD desiredAccess = TOKEN_ASSIGN_PRIMARY | TOKEN_DUPLICATE | TOKEN_QUERY |
                          TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID;
    if (!DuplicateTokenEx(userToken, desiredAccess, nullptr, SecurityImpersonation,
                          TokenPrimary, &primaryToken)) {
        LogLastError(L"DuplicateTokenEx failed");
        CloseHandle(userToken);
        return false;
    }
    CloseHandle(userToken);

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.lpDesktop = (LPWSTR)L"winsta0\\default";

    BOOL ok = CreateProcessAsUserW(primaryToken, applicationPath, commandLine,
                                  nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                  nullptr, nullptr, &si, pi);
    if (!ok) {
        LogLastError(L"CreateProcessAsUserW failed");
    } else {
        Wh_Log(L"Tool process created as active user: pid=%lu", pi->dwProcessId);
    }

    CloseHandle(primaryToken);
    return ok != FALSE;
}

// ---------------------------------------------------------------------------
// Apply State via Native Windows Shell Messages (IPC Safe)
// ---------------------------------------------------------------------------
static void ApplyState() {
    // 1) Safely locate the desktop shell container
    HWND hWndProgman = FindWindowW(L"Progman", nullptr);
    HWND hWndShellView = FindWindowExW(hWndProgman, nullptr, L"SHELLDLL_DefView", nullptr);

    // Windows 10/11 compatibility when using dynamic wallpapers or multiple WorkerW
    if (!hWndShellView) {
        HWND hWndWorkerW = nullptr;
        while ((hWndWorkerW = FindWindowExW(nullptr, hWndWorkerW, L"WorkerW", nullptr)) != nullptr) {
            hWndShellView = FindWindowExW(hWndWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
            if (hWndShellView) break;
        }
    }

    if (!hWndShellView) {
        Wh_Log(L"SHELLDLL_DefView context window not found.");
        return;
    }

    // 2) Determine the current visual state of the desktop icons
    HWND hWndListView = FindWindowExW(hWndShellView, nullptr, L"SysListView32", nullptr);
    bool iconsCurrentlyVisible = true;
    
    if (hWndListView) {
        iconsCurrentlyVisible = (GetWindowLongPtrW(hWndListView, GWL_STYLE) & WS_VISIBLE) != 0;
    }

    // 3) Send the native toggle command (0x7402) only if it differs from the desired state
    // g_hidden == true means we want the icons hidden.
    if ((g_hidden && iconsCurrentlyVisible) || (!g_hidden && !iconsCurrentlyVisible)) {
        SendMessageW(hWndShellView, WM_COMMAND, 0x7402, 0);
    }
}

// ---------------------------------------------------------------------------
// Tray icon drawing (two 32x32 ARGB icons, generated at runtime)
// ---------------------------------------------------------------------------
static HICON MakeIcon(bool hidden) {
    const int S = 32;
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = S;
    bi.bmiHeader.biHeight      = -S;  // top-down
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP color = CreateDIBSection(mem, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    unsigned char* px = static_cast<unsigned char*>(bits);
    if (px) {
        for (int i = 0; i < S * S * 4; i++) px[i] = 0;  // transparent
    }

    auto setpx = [&](int x, int y, BYTE r, BYTE g, BYTE b, BYTE a) {
        if (!px || x < 0 || y < 0 || x >= S || y >= S) return;
        unsigned char* p = px + (y * S + x) * 4;
        p[0] = b; p[1] = g; p[2] = r; p[3] = a;  // BGRA, straight alpha
    };
    auto fillRect = [&](int x0, int y0, int x1, int y1, BYTE r, BYTE g, BYTE b) {
        for (int y = y0; y < y1; y++)
            for (int x = x0; x < x1; x++)
                setpx(x, y, r, g, b, 255);
    };

    // 2x2 grid of squares = desktop icons (white).
    const int s = 11, gap = 4, ox = 2, oy = 2;
    fillRect(ox,           oy,           ox + s,           oy + s,           255, 255, 255);
    fillRect(ox + s + gap, oy,           ox + s + gap + s, oy + s,           255, 255, 255);
    fillRect(ox,           oy + s + gap, ox + s,           oy + s + gap + s, 255, 255, 255);
    fillRect(ox + s + gap, oy + s + gap, ox + s + gap + s, oy + s + gap + s, 255, 255, 255);

    // Red diagonal slash when hidden.
    if (hidden) {
        for (int t = -2; t <= 2; t++)
            for (int i = 0; i < S; i++)
                setpx(i, (S - 1 - i) + t, 230, 60, 60, 255);
    }

    // Zeroed AND mask (ignored for 32bpp alpha icons, but required).
    const int stride = ((S + 15) / 16) * 2;
    std::vector<unsigned char> maskBits(stride * S, 0);
    HBITMAP mask = CreateBitmap(S, S, 1, 1, maskBits.data());

    ICONINFO ii = {};
    ii.fIcon    = TRUE;
    ii.hbmMask  = mask;
    ii.hbmColor = color;
    HICON icon = CreateIconIndirect(&ii);

    DeleteObject(color);
    DeleteObject(mask);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    return icon;
}

static void EnsureIcons() {
    if (!g_iconShown)  g_iconShown  = MakeIcon(false);
    if (!g_iconHidden) g_iconHidden = MakeIcon(true);
}

static void SetTip(NOTIFYICONDATAW* nid) {
    const wchar_t* tip = g_hidden ? L"Desktop icons: hidden"
                                  : L"Desktop icons: shown";
    wcsncpy(nid->szTip, tip, 127);
    nid->szTip[127] = 0;
}

static void AddTrayIcon(HWND hwnd) {
    EnsureIcons();
    g_nid = {};
    g_nid.cbSize           = sizeof(g_nid);
    g_nid.hWnd             = hwnd;
    g_nid.uID              = TRAY_UID;
    g_nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYCB;
    g_nid.hIcon            = g_hidden ? g_iconHidden : g_iconShown;
    SetTip(&g_nid);
    BOOL ok = Shell_NotifyIconW(NIM_ADD, &g_nid);
    Wh_Log(L"Tray NIM_ADD ok=%d hwnd=%p", (int)ok, (void*)hwnd);
}

static void UpdateTrayIcon() {
    if (!g_nid.hWnd) return;
    g_nid.uFlags = NIF_ICON | NIF_TIP;
    g_nid.hIcon  = g_hidden ? g_iconHidden : g_iconShown;
    SetTip(&g_nid);
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);
}

static void RemoveTrayIcon() {
    if (g_nid.hWnd) {
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
        g_nid.hWnd = nullptr;
    }
}

static void ToggleNow() {
    g_hidden = !g_hidden;
    Wh_SetIntValue(L"hidden", g_hidden ? 1 : 0);
    ApplyState();
    UpdateTrayIcon();
}

static void ShowTrayMenu(HWND hwnd) {
    POINT pt;
    GetCursorPos(&pt);
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    AppendMenuW(menu, MF_STRING | (g_hidden ? 0 : MF_CHECKED), ID_TOGGLE,
                L"Show desktop icons");
    SetForegroundWindow(hwnd);  // required so the menu closes on focus loss
    TrackPopupMenu(menu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
    PostMessageW(hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);
}

static LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_taskbarCreated && g_taskbarCreated != 0) {
        // Explorer/taskbar restarted: re-add our icon.
        g_nid.hWnd = nullptr;
        AddTrayIcon(hwnd);
        return 0;
    }

    switch (msg) {
        case WM_TRAYCB:
            switch (static_cast<UINT>(lp)) {
                case WM_LBUTTONUP:
                    ToggleNow();
                    break;
                case WM_RBUTTONUP:
                    ShowTrayMenu(hwnd);
                    break;
            }
            return 0;

        case WM_COMMAND:
            if (LOWORD(wp) == ID_TOGGLE)
                ToggleNow();
            return 0;

        case WM_DESTROY:
            RemoveTrayIcon();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// Keyboard shortcut parsing
// ---------------------------------------------------------------------------
static UINT KeyNameToVk(const std::wstring& t) {
    if (t.size() == 1) {
        wchar_t c = t[0];
        if (c >= L'a' && c <= L'z') return static_cast<UINT>(L'A' + (c - L'a'));
        if (c >= L'0' && c <= L'9') return static_cast<UINT>(c);
    }
    if (t.size() >= 2 && t[0] == L'f') {
        int n = 0;
        bool digits = true;
        for (size_t i = 1; i < t.size(); ++i) {
            wchar_t d = t[i];
            if (d < L'0' || d > L'9') { digits = false; break; }
            n = n * 10 + (d - L'0');
        }
        if (digits && n >= 1 && n <= 24) return static_cast<UINT>(VK_F1 + (n - 1));
    }
    if (t == L"space")                 return VK_SPACE;
    if (t == L"insert" || t == L"ins") return VK_INSERT;
    if (t == L"delete" || t == L"del") return VK_DELETE;
    if (t == L"home")                  return VK_HOME;
    if (t == L"end")                   return VK_END;
    if (t == L"pgup" || t == L"prior") return VK_PRIOR;
    if (t == L"pgdn" || t == L"next")  return VK_NEXT;
    return 0;
}

static bool ParseHotkey(PCWSTR s, UINT* mods, UINT* vk) {
    *mods = MOD_NOREPEAT;
    *vk = 0;
    if (!s) return false;

    std::wstring str = s;
    size_t start = 0;
    while (start <= str.size()) {
        size_t plus = str.find(L'+', start);
        std::wstring tok = (plus == std::wstring::npos)
                                ? str.substr(start)
                                : str.substr(start, plus - start);

        size_t a = tok.find_first_not_of(L" \t");
        if (a == std::wstring::npos) {
            tok.clear();
        } else {
            size_t b = tok.find_last_not_of(L" \t");
            tok = tok.substr(a, b - a + 1);
        }
        for (auto& c : tok) c = towlower(c);

        if (!tok.empty()) {
            if (tok == L"ctrl" || tok == L"control")               *mods |= MOD_CONTROL;
            else if (tok == L"alt")                                *mods |= MOD_ALT;
            else if (tok == L"shift")                              *mods |= MOD_SHIFT;
            else if (tok == L"win" || tok == L"windows" || tok == L"meta") *mods |= MOD_WIN;
            else {
                UINT k = KeyNameToVk(tok);
                if (k) *vk = k;
            }
        }

        if (plus == std::wstring::npos) break;
        start = plus + 1;
    }

    return *vk != 0;
}

// ---------------------------------------------------------------------------
// UI thread: tray icon window + keyboard shortcut + message loop
// ---------------------------------------------------------------------------
static DWORD WINAPI UiThreadProc(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    if (!g_taskbarCreated)
        g_taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");

    HINSTANCE hInst = GetModuleHandleW(nullptr);
    if (!g_classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize        = sizeof(wc);
        wc.lpfnWndProc   = MsgWndProc;
        wc.hInstance     = hInst;
        wc.lpszClassName = kMsgClass;
        RegisterClassExW(&wc);
        g_classRegistered = true;
    }

    // Hidden top-level window (not message-only, so it receives TaskbarCreated).
    g_msgWnd = CreateWindowExW(0, kMsgClass, L"", WS_POPUP,
                               0, 0, 0, 0, nullptr, nullptr, hInst, nullptr);
    Wh_Log(L"Tray window=%p", (void*)g_msgWnd);

    BOOL registered = FALSE;
    if (g_vk)
        registered = RegisterHotKey(nullptr, 1, g_mods, g_vk);
    if (g_vk && !registered)
        Wh_Log(L"Could not register the shortcut (might be in use)");

    if (g_msgWnd)
        AddTrayIcon(g_msgWnd);

    if (g_threadReady)
        SetEvent(g_threadReady);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY) {
            ToggleNow();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    RemoveTrayIcon();
    if (registered)
        UnregisterHotKey(nullptr, 1);
    if (g_msgWnd) {
        DestroyWindow(g_msgWnd);
        g_msgWnd = nullptr;
    }
    return 0;
}

static void StartUiThread() {
    g_threadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_threadId);
    if (g_threadReady) {
        if (g_thread)
            WaitForSingleObject(g_threadReady, 5000);
        CloseHandle(g_threadReady);
        g_threadReady = nullptr;
    }
}

static void StopUiThread() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
        g_threadId = 0;
    }
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
static void LoadSettings() {
    g_settingShow = Wh_GetIntSetting(L"showIcons") != 0;

    PCWSTR hk = Wh_GetStringSetting(L"hotkey");
    if (!ParseHotkey(hk, &g_mods, &g_vk)) {
        g_mods = 0;
        g_vk = 0;
    }
    Wh_FreeStringSetting(hk);
}

// ---------------------------------------------------------------------------
// Tool mod callbacks
// ---------------------------------------------------------------------------
BOOL WhTool_ModInit() {
    Wh_Log(L"WhTool_ModInit");

    LoadSettings();

    int defaultHidden = g_settingShow ? 0 : 1;
    g_hidden = Wh_GetIntValue(L"hidden", defaultHidden) != 0;

    ApplyState();
    StartUiThread();
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"WhTool_ModUninit");

    StopUiThread();

    if (g_iconShown)  { DestroyIcon(g_iconShown);  g_iconShown = nullptr; }
    if (g_iconHidden) { DestroyIcon(g_iconHidden); g_iconHidden = nullptr; }

    // Restore visible icons when disabling the mod.
    g_hidden = false;
    ApplyState();
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"WhTool_ModSettingsChanged");

    StopUiThread();
    LoadSettings();

    // The checkbox takes priority when editing settings.
    g_hidden = !g_settingShow;
    Wh_SetIntValue(L"hidden", g_hidden ? 1 : 0);

    ApplyState();
    StartUiThread();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId)) {
        LogLastError(L"ProcessIdToSessionId failed");
    }
    Wh_Log(L"Wh_ModInit pid=%lu session=%lu cmd='%s'", GetCurrentProcessId(), sessionId,
           GetCommandLineW());

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
        if (wcscmp(argv[i], L"-service-start") == 0 ||
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
        Wh_Log(L"Excluded instance (service-start/stop)");
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        if (sessionId == 0) {
            Wh_Log(L"Tool process started in session 0 - cannot run UI/hotkey here");
            ExitProcess(1);
        }

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
        Wh_Log(L"Other tool-mod instance detected - skipping");
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    Wh_Log(L"Launcher instance");
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    HANDLE existing = OpenMutexW(SYNCHRONIZE, FALSE, L"windhawk-tool-mod_" WH_MOD_ID);
    if (existing) {
        Wh_Log(L"Tool mod already running (mutex exists)");
        CloseHandle(existing);
        return;
    }

    DWORD sessionId = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId)) {
        LogLastError(L"ProcessIdToSessionId failed (AfterInit)");
    }
    Wh_Log(L"Wh_ModAfterInit pid=%lu session=%lu", GetCurrentProcessId(), sessionId);

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

    PROCESS_INFORMATION pi = {};

    if (sessionId == 0) {
        // On some systems windhawk.exe only runs as a service (Session 0).
        // Spawn the tool process into the active user session.
        Wh_Log(L"Launching tool process in active user session");
        if (!CreateToolProcessAsActiveUser(currentProcessPath, commandLine, &pi)) {
            Wh_Log(L"Failed to create tool process as active user");
            return;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return;
    }

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
    STARTUPINFO si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;

    if (pCreateProcessInternalW) {
        if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                     nullptr, nullptr, FALSE,
                                     NORMAL_PRIORITY_CLASS, nullptr, nullptr,
                                     &si, &pi, nullptr)) {
            LogLastError(L"CreateProcessInternalW failed");
            return;
        }
    } else {
        Wh_Log(L"CreateProcessInternalW not exported; falling back to CreateProcessW");
        if (!CreateProcessW(currentProcessPath, commandLine, nullptr, nullptr, FALSE,
                            NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi)) {
            LogLastError(L"CreateProcessW failed");
            return;
        }
    }

    Wh_Log(L"Tool process created: pid=%lu", pi.dwProcessId);

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