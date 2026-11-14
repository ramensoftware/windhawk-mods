// ==WindhawkMod==
// @id              file-explorer-reopen-closed-tab
// @name            File Explorer Reopen Closed Tab
// @description     Reopens the last closed Explorer tab when Ctrl+Shift+T is pressed in an active Explorer window
// @version         1.0.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         explorer.exe
// @compilerOptions -lshlwapi -lole32 -loleaut32 -lshell32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer Reopen Closed Tab

Press `Ctrl+Shift+T` in any active File Explorer window to reopen the last closed tab — just like in a browser.

- Restored tabs open directly in the existing Explorer window, not as a new window.
- If no Explorer window is open, nothing happens.
- Up to 50 tabs are remembered per session.
- History clears when all Explorer windows are closed.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <exdisp.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <mutex>

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------

static HANDLE g_hPollThread    = NULL;
static bool   g_bRunning       = false;
static HHOOK  g_hKeyboardHook  = NULL;
static HANDLE g_hHookThread    = NULL;
static DWORD  g_dwHookThreadId = 0;

static std::mutex g_mutex;

// Key: HWND of the top-level browser — stable for the tab's lifetime.
// Value: last known path for that tab.
static std::map<HWND, std::wstring> g_knownTabs;

// Undo stack: back = most recently closed.
static std::vector<std::wstring> g_closedTabStack;

static const size_t MAX_HISTORY = 50;

// Restore thread — OpenAsTab runs here, not inside the keyboard hook
static HANDLE g_hRestoreThread    = NULL;
static DWORD  g_dwRestoreThreadId = 0;

// Foreground event hook — enables/disables polling
static HWINEVENTHOOK g_hForegroundHook = NULL;
static volatile bool g_bExplorerForeground = false;

// ---------------------------------------------------------------------------
// COM helper — enumerate all open Explorer tabs.
// Returns a map of HWND -> current path.
// ---------------------------------------------------------------------------

struct TabInfo {
    HWND         hwnd;
    std::wstring path;
};

static std::vector<TabInfo> GetCurrentTabs() {
    std::vector<TabInfo> tabs;

    IShellWindows* pSW = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr,
                                CLSCTX_ALL, IID_PPV_ARGS(&pSW)))) {
        return tabs;
    }

    long count = 0;
    pSW->get_Count(&count);

    for (long i = 0; i < count; i++) {
        VARIANT v;
        VariantInit(&v);
        v.vt   = VT_I4;
        v.lVal = i;

        IDispatch* pDisp = nullptr;
        if (FAILED(pSW->Item(v, &pDisp)) || !pDisp) continue;

        IServiceProvider* pSP = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_PPV_ARGS(&pSP)))) {
            IShellBrowser* pSB = nullptr;
            if (SUCCEEDED(pSP->QueryService(SID_STopLevelBrowser,
                                            IID_PPV_ARGS(&pSB)))) {
                // GetWindow() returns a child HWND unique per tab — use it
                // as the stable tab identity. Do NOT walk to root here, as all
                // tabs share the same root CabinetWClass HWND.
                HWND hwnd = nullptr;
                pSB->GetWindow(&hwnd);

                if (hwnd) {
                    IShellView* pSV = nullptr;
                    if (SUCCEEDED(pSB->QueryActiveShellView(&pSV))) {
                        IFolderView* pFV = nullptr;
                        if (SUCCEEDED(pSV->QueryInterface(IID_PPV_ARGS(&pFV)))) {
                            IPersistFolder2* pPF2 = nullptr;
                            if (SUCCEEDED(pFV->GetFolder(IID_PPV_ARGS(&pPF2)))) {
                                PIDLIST_ABSOLUTE pidl = nullptr;
                                if (SUCCEEDED(pPF2->GetCurFolder(&pidl))) {
                                    wchar_t path[MAX_PATH] = {};
                                    if (SHGetPathFromIDListW(pidl, path)) {
                                        tabs.push_back({ hwnd, path });
                                    }
                                    CoTaskMemFree(pidl);
                                }
                                pPF2->Release();
                            }
                            pFV->Release();
                        }
                        pSV->Release();
                    }
                }
                pSB->Release();
            }
            pSP->Release();
        }
        pDisp->Release();
    }

    pSW->Release();
    return tabs;
}

// ---------------------------------------------------------------------------
// Find the primary (foreground or first available) Explorer HWND.
// Returns NULL if no Explorer window is open.
// ---------------------------------------------------------------------------

static HWND GetPrimaryExplorerHwnd() {
    // Prefer the foreground window if it's Explorer
    HWND hwndFg = GetForegroundWindow();
    if (hwndFg) {
        wchar_t cls[64] = {};
        GetClassNameW(hwndFg, cls, ARRAYSIZE(cls));
        if (lstrcmpW(cls, L"CabinetWClass") == 0) {
            return hwndFg;
        }
    }

    // Otherwise find the first Explorer window
    HWND hwnd = FindWindowW(L"CabinetWClass", nullptr);
    return hwnd; // NULL if none
}

// ---------------------------------------------------------------------------
// Open a path as a new tab in the given Explorer window.
// Uses the same undocumented message the Explorer Tab Utility / AHK scripts
// use: WM_COMMAND 0xA21B opens a new tab, then we navigate it to the path.
// ---------------------------------------------------------------------------

static bool OpenAsTab(HWND hwndExplorer, const std::wstring& path) {
    // The message must go to ShellTabWindowClass1 (the tab strip child control),
    // not the top-level CabinetWClass window — this is what makes the new tab
    // appear in IShellWindows. Sending to the top-level opens a Home tab that
    // is invisible to IShellWindows.
    HWND hwndTabStrip = FindWindowExW(hwndExplorer, nullptr,
                                      L"ShellTabWindowClass", nullptr);
    if (!hwndTabStrip) {
        return false;
    }

    // Get count before opening the new tab
    IShellWindows* pSW = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr,
                                CLSCTX_ALL, IID_PPV_ARGS(&pSW)))) {
        return false;
    }

    long countBefore = 0;
    pSW->get_Count(&countBefore);

    // Send new-tab command to ShellTabWindowClass — this registers in IShellWindows
    SendMessageW(hwndTabStrip, 0x0111, 0xA21B, 0);

    // Wait for the new tab to appear in IShellWindows (up to 3s)
    const DWORD deadline = GetTickCount() + 3000;
    long countAfter = countBefore;
    while (countAfter <= countBefore && GetTickCount() < deadline) {
        Sleep(50);
        pSW->get_Count(&countAfter);
    }

    if (countAfter <= countBefore) {
        pSW->Release();
        return false;
    }

    // The new tab is at index countBefore (0-based)
    VARIANT vIdx;
    VariantInit(&vIdx);
    vIdx.vt   = VT_I4;
    vIdx.lVal = countBefore;

    bool ok = false;
    IDispatch* pDisp = nullptr;
    if (SUCCEEDED(pSW->Item(vIdx, &pDisp)) && pDisp) {
        IWebBrowser2* pWB2 = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_PPV_ARGS(&pWB2)))) {
            VARIANT vPath;
            VariantInit(&vPath);
            vPath.vt      = VT_BSTR;
            vPath.bstrVal = SysAllocString(path.c_str());
            VARIANT vEmpty;
            VariantInit(&vEmpty);

            HRESULT hr = pWB2->Navigate2(&vPath, &vEmpty, &vEmpty, &vEmpty, &vEmpty);
            ok = SUCCEEDED(hr);

            SysFreeString(vPath.bstrVal);
            pWB2->Release();
        }
        pDisp->Release();
    }

    pSW->Release();
    return ok;
}

// ---------------------------------------------------------------------------
// Reopen one tab from the undo stack — must NOT be called from inside
// LowLevelKeyboardProc. Called from the dedicated restore thread instead.
// ---------------------------------------------------------------------------

static void ReopenLastClosedTab() {
    std::wstring path;
    {
        std::lock_guard<std::mutex> lk(g_mutex);

        if (g_closedTabStack.empty()) {
            return;
        }
        path = g_closedTabStack.back();
        g_closedTabStack.pop_back();
    }

    HWND hwndTarget = GetPrimaryExplorerHwnd();
    if (!hwndTarget) {
        // No Explorer window open — do nothing, push the path back
        std::lock_guard<std::mutex> lk(g_mutex);
        g_closedTabStack.push_back(path);
        return;
    }

    bool ok = OpenAsTab(hwndTarget, path);
}

// ---------------------------------------------------------------------------
// Restore thread — processes restore requests posted by the keyboard hook.
// Runs OpenAsTab (COM + SendMessage + wait loop) safely off the hook.
// ---------------------------------------------------------------------------

static DWORD WINAPI RestoreThread(LPVOID) {
    HRESULT hr = CoInitializeEx(nullptr,
                                COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        return 1;
    }

    MSG msg;
    while (g_bRunning && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP) {
            ReopenLastClosedTab();
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Check whether an Explorer (CabinetWClass) window is in the foreground
// ---------------------------------------------------------------------------

static bool IsExplorerWindowForeground() {
    HWND hwndFg = GetForegroundWindow();
    if (!hwndFg) return false;
    wchar_t cls[64] = {};
    GetClassNameW(hwndFg, cls, ARRAYSIZE(cls));
    return lstrcmpW(cls, L"CabinetWClass") == 0;
}

// ---------------------------------------------------------------------------
// Low-level keyboard hook
// ---------------------------------------------------------------------------

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        auto* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (kb->vkCode == 'T') {
            bool ctrl  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool shift = (GetAsyncKeyState(VK_SHIFT)   & 0x8000) != 0;
            bool alt   = (GetAsyncKeyState(VK_MENU)    & 0x8000) != 0;
            if (ctrl && shift && !alt && IsExplorerWindowForeground()) {
                if (g_dwRestoreThreadId) {
                    PostThreadMessageW(g_dwRestoreThreadId, WM_APP, 0, 0);
                }
                return 1; // suppress keystroke
            }
        }
    }
    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Hook thread
// ---------------------------------------------------------------------------

static void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD);

static DWORD WINAPI HookThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    g_hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL,
                                        LowLevelKeyboardProc,
                                        GetModuleHandleW(nullptr), 0);
    if (!g_hKeyboardHook) {
        CoUninitialize();
        return 1;
    }

    // Check if Explorer is already foreground at startup
    HWND hwndFg = GetForegroundWindow();
    if (hwndFg) {
        wchar_t cls[64] = {};
        GetClassNameW(hwndFg, cls, ARRAYSIZE(cls));
        g_bExplorerForeground = (lstrcmpW(cls, L"CabinetWClass") == 0);
    }

    // Install foreground event hook on THIS thread — WINEVENT_OUTOFCONTEXT
    // requires the installing thread to pump messages, which we do here.
    g_hForegroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        nullptr, ForegroundEventProc,
        0, 0, WINEVENT_OUTOFCONTEXT);

    MSG msg;
    while (g_bRunning && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hForegroundHook) {
        UnhookWinEvent(g_hForegroundHook);
        g_hForegroundHook = nullptr;
    }
    if (g_hKeyboardHook) {
        UnhookWindowsHookEx(g_hKeyboardHook);
        g_hKeyboardHook = nullptr;
    }
    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Poll thread — tracks tab opens/closes/navigations
// ---------------------------------------------------------------------------

// Foreground event callback — sets g_bExplorerForeground so the poll
// thread only runs when Explorer is the active window.
static void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD,
                                         HWND hwnd, LONG, LONG,
                                         DWORD, DWORD) {
    if (!hwnd) {
        g_bExplorerForeground = false;
        return;
    }
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    g_bExplorerForeground = (lstrcmpW(cls, L"CabinetWClass") == 0);
}

static DWORD WINAPI PollThread(LPVOID) {
    HRESULT hr = CoInitializeEx(nullptr,
                                COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        return 1;
    }

    // Seed initial state. Retry for up to 30s in case the tool process
    // launched before Explorer's shell windows were ready.
    {
        const DWORD deadline = GetTickCount() + 30000;
        while (g_bRunning && GetTickCount() < deadline) {
            auto seed = GetCurrentTabs();
            if (!seed.empty()) {
                std::lock_guard<std::mutex> lk(g_mutex);
                for (const auto& t : seed) {
                    g_knownTabs[t.hwnd] = t.path;
                }
                break;
            }
            Sleep(1000);
        }
    }

    while (g_bRunning) {
        // Only poll when Explorer is in the foreground — saves CPU otherwise
        if (!g_bExplorerForeground) {
            Sleep(150);
            continue;
        }
        Sleep(750);

        auto currentVec = GetCurrentTabs();

        std::map<HWND, std::wstring> currentMap;
        for (const auto& t : currentVec) {
            currentMap[t.hwnd] = t.path;
        }

        std::lock_guard<std::mutex> lk(g_mutex);

        // Detect closed tabs
        std::vector<std::wstring> closedPaths;
        for (const auto& kv : g_knownTabs) {
            if (currentMap.find(kv.first) == currentMap.end()) {
                closedPaths.push_back(kv.second);
            }
        }
        for (const auto& p : closedPaths) {
            g_closedTabStack.push_back(p);
            if (g_closedTabStack.size() > MAX_HISTORY) {
                g_closedTabStack.erase(g_closedTabStack.begin());
            }
        }
        if (!closedPaths.empty()) {
        }

        // Detect navigations (log only, no stack effect)
        for (const auto& kv : currentMap) {
            auto it = g_knownTabs.find(kv.first);
            if (it != g_knownTabs.end() && it->second != kv.second) {
            }
        }

        // All windows closed → new session, clear stack
        if (currentMap.empty() && !g_knownTabs.empty()) {
            g_closedTabStack.clear();
        }

        g_knownTabs = currentMap;
    }

    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"Explorer Reopen Closed Tab: initialized.");

    g_bRunning = true;

    g_hRestoreThread = CreateThread(nullptr, 0, RestoreThread, nullptr, 0,
                                    &g_dwRestoreThreadId);
    if (!g_hRestoreThread) {
        return FALSE;
    }

    g_hPollThread = CreateThread(nullptr, 0, PollThread, nullptr, 0, nullptr);
    if (!g_hPollThread) {
        return FALSE;
    }

    g_hHookThread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_dwHookThreadId);
    if (!g_hHookThread) {
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    g_bRunning = false;

    if (g_dwHookThreadId) {
        PostThreadMessageW(g_dwHookThreadId, WM_QUIT, 0, 0);
    }
    if (g_hHookThread) {
        WaitForSingleObject(g_hHookThread, 3000);
        CloseHandle(g_hHookThread);
        g_hHookThread    = nullptr;
        g_dwHookThreadId = 0;
    }

    if (g_dwRestoreThreadId) {
        PostThreadMessageW(g_dwRestoreThreadId, WM_QUIT, 0, 0);
    }
    if (g_hRestoreThread) {
        WaitForSingleObject(g_hRestoreThread, 3000);
        CloseHandle(g_hRestoreThread);
        g_hRestoreThread    = nullptr;
        g_dwRestoreThreadId = 0;
    }

    if (g_hPollThread) {
        WaitForSingleObject(g_hPollThread, 3000);
        CloseHandle(g_hPollThread);
        g_hPollThread = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated explorer.exe process.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
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
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
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
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
