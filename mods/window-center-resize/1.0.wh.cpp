// ==WindhawkMod==
// @id              window-center-resize
// @name            Window Center & Resize
// @description     Center and resize the foreground window using global hotkeys. Cycle hotkey steps through custom sizes in order. Combine with center hotkey to resize+center in one press. Optionally auto-restores maximized windows before resizing.
// @version         1.0
// @author          Salyts
// @github          https://github.com/Salyts
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Center & Resize

Centers and resizes the foreground window using global hotkeys.

## Default Hotkeys

| Action                | Hotkey      | 
|-----------------------|-------------|
| Center window         | `Shift+F10` |
| Cycle size (1→2→…→1)  | `Shift+F9` |

Each press of the **cycle hotkey** applies the next size in the list, wrapping
back to the first after the last one.

## Auto-restore maximized windows

Enable **RestoreMaximized** so that when you press the cycle hotkey on a
maximized window, the mod automatically un-maximizes it and applies the target
size. If disabled, the hotkey is ignored for maximized windows.

## Custom sizes

Add as many sizes as you want in the **Sizes** list. Each entry has:
- **Width** — % of monitor work area width
- **Height** — % of monitor work area height

The cycle hotkey steps through all entries in order on each press.

## Hotkey format

`Modifier+Key` — modifiers: `Win`, `Alt`, `Ctrl`, `Shift`.
Keys: `F1`-`F24`, `A`-`Z`, `0`-`9`, `Space`, `Enter`, `Tab`, `Insert`,
`Delete`, `Home`, `End`, `PageUp`, `PageDown`, `Left`, `Right`, `Up`, `Down`.

Examples: `Shift+F9`, `Win+Alt+C`, `Ctrl+F1`
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- CenterHotkey: Shift+F10
  $name: Center hotkey
  $description: "Centers the active window on the current monitor."

- CycleHotkey: Shift+F9
  $name: Cycle size hotkey
  $description: "Each press applies the next size in the Sizes list (wraps back to first after last)."

- CenterOnCycle: true
  $name: Center window on cycle
  $description: "If enabled, the cycle hotkey also centers the window after resizing."

- RestoreMaximized: true
  $name: Auto-restore maximized windows
  $description: "If the window is maximized, automatically un-maximize it before applying the size."

- Sizes:
    - - Width: 40
        $name: "Width %"
      - Height: 50
        $name: "Height %"
    - - Width: 60
      - Height: 70
    - - Width: 80
      - Height: 85
  $name: Sizes
  $description: "List of sizes to cycle through. Width/Height are percentages of the monitor work area (1-100). Use 0 to keep that dimension unchanged."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <string>
#include <vector>

#define HK_CENTER 1
#define HK_CYCLE  2

constexpr UINT WM_APP_RELOAD = WM_APP + 1;

struct SizeStep { int w, h; };

struct WcrSettings {
    std::wstring          centerHotkey     = L"Shift+F10";
    std::wstring          cycleHotkey      = L"Shift+F9";
    bool                  centerOnCycle    = true;
    bool                  restoreMaximized = true;
    std::vector<SizeStep> sizes;
};

static WcrSettings g_cfg;
static HANDLE      g_hookThread  = nullptr;
static DWORD       g_hookTid     = 0;
static int         g_cycleIdx    = 0;
static bool        g_combined    = false;

static bool ParseHotkey(const std::wstring& raw, UINT& outMod, UINT& outVk)
{
    outMod = 0; outVk = 0;
    if (raw.empty()) return false;

    std::wstring up = raw;
    for (auto& c : up) c = (wchar_t)towupper(c);

    auto resolve = [&](const std::wstring& tok) {
        if (tok == L"WIN" || tok == L"WINDOWS")   { outMod |= MOD_WIN;     return; }
        if (tok == L"ALT")                         { outMod |= MOD_ALT;     return; }
        if (tok == L"CTRL" || tok == L"CONTROL")   { outMod |= MOD_CONTROL; return; }
        if (tok == L"SHIFT")                       { outMod |= MOD_SHIFT;   return; }
        if (tok.size() >= 2 && tok[0] == L'F' && iswdigit(tok[1])) {
            int n = _wtoi(tok.c_str() + 1);
            if (n >= 1 && n <= 24) { outVk = VK_F1 + n - 1; return; }
        }
        if (tok == L"SPACE")                       { outVk = VK_SPACE;    return; }
        if (tok == L"ENTER" || tok == L"RETURN")   { outVk = VK_RETURN;   return; }
        if (tok == L"TAB")                         { outVk = VK_TAB;      return; }
        if (tok == L"ESC"   || tok == L"ESCAPE")   { outVk = VK_ESCAPE;   return; }
        if (tok == L"INSERT"|| tok == L"INS")      { outVk = VK_INSERT;   return; }
        if (tok == L"DELETE"|| tok == L"DEL")      { outVk = VK_DELETE;   return; }
        if (tok == L"HOME")                        { outVk = VK_HOME;     return; }
        if (tok == L"END")                         { outVk = VK_END;      return; }
        if (tok == L"PAGEUP"|| tok == L"PGUP")     { outVk = VK_PRIOR;    return; }
        if (tok == L"PAGEDOWN"||tok == L"PGDN")    { outVk = VK_NEXT;     return; }
        if (tok == L"LEFT")                        { outVk = VK_LEFT;     return; }
        if (tok == L"RIGHT")                       { outVk = VK_RIGHT;    return; }
        if (tok == L"UP")                          { outVk = VK_UP;       return; }
        if (tok == L"DOWN")                        { outVk = VK_DOWN;     return; }
        if (tok == L"PRINTSCREEN")                 { outVk = VK_SNAPSHOT; return; }
        if (tok == L"PAUSE")                       { outVk = VK_PAUSE;    return; }
        if (tok.size() == 1) {
            wchar_t c = tok[0];
            if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9'))
                outVk = (UINT)c;
        }
    };

    std::wstring tok;
    for (wchar_t c : up) {
        if (c == L'+') { resolve(tok); tok.clear(); }
        else            tok += c;
    }
    resolve(tok);
    return outVk != 0;
}

static void GetFrameInsets(HWND w, int& il, int& it, int& ir, int& ib)
{
    il = it = ir = ib = 0;
    RECT wr = {}, fr = {};
    if (!GetWindowRect(w, &wr)) return;
    if (SUCCEEDED(DwmGetWindowAttribute(w, DWMWA_EXTENDED_FRAME_BOUNDS,
                                        &fr, sizeof(fr)))) {
        il = fr.left   - wr.left;
        it = fr.top    - wr.top;
        ir = wr.right  - fr.right;
        ib = wr.bottom - fr.bottom;
    }
}

static void UnMaximize(HWND w)
{
    if (!IsZoomed(w)) return;
    WINDOWPLACEMENT wpl = {}; wpl.length = sizeof(wpl);
    if (!GetWindowPlacement(w, &wpl)) return;
    wpl.showCmd = SW_SHOWNORMAL;
    SetWindowPlacement(w, &wpl);
}

struct WindowGeometry {
    RECT windowRect;
    int  insetLeft, insetTop, insetRight, insetBottom;
    int  visibleWidth, visibleHeight;
    RECT workArea;
    int  monitorWidth, monitorHeight;
};

static bool GetWindowGeometry(HWND w, WindowGeometry& g)
{
    if (!GetWindowRect(w, &g.windowRect)) return false;

    GetFrameInsets(w, g.insetLeft, g.insetTop, g.insetRight, g.insetBottom);
    g.visibleWidth  = (g.windowRect.right  - g.windowRect.left) - g.insetLeft - g.insetRight;
    g.visibleHeight = (g.windowRect.bottom - g.windowRect.top)  - g.insetTop  - g.insetBottom;

    HMONITOR hm = MonitorFromWindow(w, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {}; mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(hm, &mi)) return false;

    g.workArea      = mi.rcWork;
    g.monitorWidth  = mi.rcWork.right  - mi.rcWork.left;
    g.monitorHeight = mi.rcWork.bottom - mi.rcWork.top;
    return true;
}

static void DoCenterWindow(HWND w)
{
    if (!w || !IsWindow(w) || IsIconic(w) || IsZoomed(w)) return;

    WindowGeometry g;
    if (!GetWindowGeometry(w, g)) return;

    int x = g.workArea.left + (g.monitorWidth  - g.visibleWidth)  / 2 - g.insetLeft;
    int y = g.workArea.top  + (g.monitorHeight - g.visibleHeight) / 2 - g.insetTop;

    SetWindowPos(w, nullptr, x, y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    Wh_Log(L"Center hwnd=%p -> (%d,%d)", w, x, y);
}

static int Clamp(int v, int lo, int hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

static void DoResizeCenter(HWND w, int pctW, int pctH, bool doCenter)
{
    if (!w || !IsWindow(w) || IsIconic(w)) return;

    if (IsZoomed(w)) {
        if (!g_cfg.restoreMaximized) {
            Wh_Log(L"Skipping maximized hwnd=%p", w);
            return;
        }
        UnMaximize(w);
    }

    WindowGeometry g;
    if (!GetWindowGeometry(w, g)) return;

    pctW = Clamp(pctW, 0, 100);
    pctH = Clamp(pctH, 0, 100);

    int newVisW = (pctW > 0) ? (g.monitorWidth  * pctW / 100) : g.visibleWidth;
    int newVisH = (pctH > 0) ? (g.monitorHeight * pctH / 100) : g.visibleHeight;
    int newWinW = newVisW + g.insetLeft + g.insetRight;
    int newWinH = newVisH + g.insetTop  + g.insetBottom;

    int x = doCenter ? (g.workArea.left + (g.monitorWidth  - newVisW) / 2 - g.insetLeft) : g.windowRect.left;
    int y = doCenter ? (g.workArea.top  + (g.monitorHeight - newVisH) / 2 - g.insetTop)  : g.windowRect.top;

    SetWindowPos(w, nullptr, x, y, newWinW, newWinH,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    Wh_Log(L"Resize hwnd=%p %d%%x%d%% -> (%d,%d) %dx%d",
           w, pctW, pctH, x, y, newWinW, newWinH);
}

static void LoadAllSettings()
{
    auto readStr = [](PCWSTR n, std::wstring& out, const wchar_t* def) {
        PCWSTR v = Wh_GetStringSetting(n);
        out = (v && *v) ? v : def;
        if (v) Wh_FreeStringSetting(v);
    };

    readStr(L"CenterHotkey", g_cfg.centerHotkey, L"Shift+F10");
    readStr(L"CycleHotkey",  g_cfg.cycleHotkey,  L"Shift+F9");
    g_cfg.centerOnCycle    = (Wh_GetIntSetting(L"CenterOnCycle")    != 0);
    g_cfg.restoreMaximized = (Wh_GetIntSetting(L"RestoreMaximized") != 0);

    g_cfg.sizes.clear();
    for (int i = 0; i < 64; i++) {
        int sw = Wh_GetIntSetting(L"Sizes[%d].Width",  i);
        int sh = Wh_GetIntSetting(L"Sizes[%d].Height", i);
        if (sw == 0 && sh == 0) break;
        g_cfg.sizes.push_back({ sw, sh });
        Wh_Log(L"  Sizes[%d] = %d%% x %d%%", i, sw, sh);
    }

    if (g_cfg.sizes.empty()) {
        g_cfg.sizes = { {40, 50}, {60, 70}, {80, 85} };
        Wh_Log(L"Sizes empty — using defaults");
    }

    int count = (int)g_cfg.sizes.size();
    if (g_cycleIdx >= count) g_cycleIdx = 0;

    g_combined = (!g_cfg.centerHotkey.empty() &&
                  !g_cfg.cycleHotkey.empty()  &&
                  g_cfg.centerHotkey == g_cfg.cycleHotkey);

    Wh_Log(L"Settings: combined=%d centerOnCycle=%d restoreMax=%d sizes=%d cycleIdx=%d",
           (int)g_combined, (int)g_cfg.centerOnCycle, (int)g_cfg.restoreMaximized, count, g_cycleIdx);
}

static void RegisterHotkeys()
{
    UnregisterHotKey(nullptr, HK_CENTER);
    UnregisterHotKey(nullptr, HK_CYCLE);

    auto reg = [](int id, const std::wstring& key) {
        if (key.empty()) return;
        UINT mod = 0, vk = 0;
        if (!ParseHotkey(key, mod, vk)) {
            Wh_Log(L"Bad hotkey id=%d '%s'", id, key.c_str());
            return;
        }
        if (RegisterHotKey(nullptr, id, mod | MOD_NOREPEAT, vk))
            Wh_Log(L"HK id=%d '%s' OK", id, key.c_str());
        else
            Wh_Log(L"HK id=%d '%s' FAIL err=%lu", id, key.c_str(), GetLastError());
    };

    if (g_combined)
        reg(HK_CYCLE, g_cfg.cycleHotkey);
    else {
        reg(HK_CENTER, g_cfg.centerHotkey);
        reg(HK_CYCLE,  g_cfg.cycleHotkey);
    }
}

static DWORD WINAPI HookThreadProc(LPVOID)
{
    Wh_Log(L"Hook thread started");

    RegisterHotkeys();

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        if (msg.hwnd == nullptr) {
            if (msg.message == WM_HOTKEY) {
                int id = (int)msg.wParam;
                HWND fg = GetForegroundWindow();
                if (fg) {
                    if (id == HK_CENTER) {
                        DoCenterWindow(fg);
                    } else if (id == HK_CYCLE) {
                        int count = (int)g_cfg.sizes.size();
                        if (count > 0) {
                            if (g_cycleIdx >= count) g_cycleIdx = 0;
                            int idx = g_cycleIdx;
                            g_cycleIdx = (g_cycleIdx + 1) % count;
                            Wh_Log(L"Cycle -> size[%d] %d%%x%d%%",
                                   idx, g_cfg.sizes[idx].w, g_cfg.sizes[idx].h);
                            bool doCenter = g_combined || g_cfg.centerOnCycle;
                            DoResizeCenter(fg, g_cfg.sizes[idx].w,
                                               g_cfg.sizes[idx].h, doCenter);
                        }
                    }
                }
                continue;
            }

            if (msg.message == WM_APP_RELOAD) {
                Wh_Log(L"WM_APP_RELOAD: reloading settings and hotkeys");
                LoadAllSettings();
                RegisterHotkeys();
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnregisterHotKey(nullptr, HK_CENTER);
    UnregisterHotKey(nullptr, HK_CYCLE);
    Wh_Log(L"Hook thread exiting");
    return 0;
}

BOOL WhTool_ModInit()
{
    LoadAllSettings();

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookTid);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed %lu", GetLastError());
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged()
{
    if (g_hookTid)
        PostThreadMessageW(g_hookTid, WM_APP_RELOAD, 0, 0);
}

void WhTool_ModUninit()
{
    if (g_hookThread) {
        if (g_hookTid && WaitForSingleObject(g_hookThread, 0) != WAIT_OBJECT_0)
            PostThreadMessageW(g_hookTid, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, 2000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookTid    = 0;
    }
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