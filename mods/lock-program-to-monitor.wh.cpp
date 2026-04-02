// ==WindhawkMod==
// @id             lock-program-to-monitor
// @name            Lock Program to Specific Monitor
// @description     Pick any running app and lock it to a specific display monitor; its windows will be moved to that monitor whenever they open.
// @version         0.2
// @homepage        http://www.EveryVerse.ca
// @author          Jeremy 
// @include         explorer.exe
// @include         *
// @github          https://github.com/EveryVerseDOTca
// @compilerOptions -ladvapi32 -lcomctl32
// @license         CC-BY-NC-SA-4.0 
// ==/WindhawkMod==
//
// Notes:
// - The Win+Ctrl+Shift+F12 UI runs from explorer.exe (so the hotkey is stable).
// - Per-app rules are stored in HKCU\Software\OpenAI\Windhawk\ProgramMonitorPin\Rules
// - Each injected process periodically enforces its own windows' monitor placement.

// ==WindhawkModReadme==
/*
# Program Monitor Pin (Win+Ctrl+Shift+F12)

Press **Win+Ctrl+Shift+F12** to pick an open program (by EXE name) and choose a monitor/display.
From then on, whenever that program opens a window, the mod moves it to the chosen
monitor/display (best-effort; some apps reposition themselves after showing).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- pollIntervalMs: 500
  $name: Poll interval (ms)
  $description: How often each process checks for new windows to move.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <cwctype>
#include <commctrl.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

static struct {
    int pollIntervalMs;
} settings;

static HANDLE g_stopEvent = nullptr;
static HANDLE g_enforceThread = nullptr;
static HANDLE g_hotkeyThread = nullptr;
static bool g_hotkeyErrorShown = false;

static constexpr UINT kHotkeyId = 0xA11A;
static constexpr UINT kHotkeyModifiers = MOD_WIN | MOD_CONTROL | MOD_SHIFT;
static constexpr UINT kHotkeyVk = VK_F12;

static const wchar_t* kRegRulesPath =
    L"Software\\OpenAI\\Windhawk\\ProgramMonitorPin\\Rules";

static std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](wchar_t c) { return (wchar_t)towlower(c); });
    return s;
}

static std::wstring BasenameFromPath(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    return (slash == std::wstring::npos) ? path : path.substr(slash + 1);
}

static std::wstring GetCurrentProcessExePath() {
    wchar_t buf[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    if (!len || len >= MAX_PATH)
        return L"";
    return std::wstring(buf, len);
}

static std::wstring GetCurrentProcessExeNameLower() {
    return ToLower(BasenameFromPath(GetCurrentProcessExePath()));
}

static bool IsExplorerProcess() {
    return GetCurrentProcessExeNameLower() == L"explorer.exe";
}

struct MonitorDesc {
    HMONITOR hmon{};
    std::wstring device;
    RECT work{};
    bool primary{};
};

static BOOL CALLBACK EnumMonitorsProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    auto* out = (std::vector<MonitorDesc>*)lParam;

    MONITORINFOEXW mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMon, &mi))
        return TRUE;

    MonitorDesc desc;
    desc.hmon = hMon;
    desc.device = mi.szDevice;
    desc.work = mi.rcWork;
    desc.primary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
    out->push_back(std::move(desc));
    return TRUE;
}

static std::vector<MonitorDesc> EnumerateMonitors() {
    std::vector<MonitorDesc> monitors;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsProc, (LPARAM)&monitors);
    return monitors;
}

static bool LoadRuleForExe(const std::wstring& exeNameLower,
                           std::wstring* outMonitorDevice) {
    HKEY key{};
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRegRulesPath, 0, KEY_READ, &key) !=
        ERROR_SUCCESS) {
        return false;
    }

    DWORD type = 0;
    DWORD cbData = 0;
    LONG res = RegQueryValueExW(key, exeNameLower.c_str(), nullptr, &type,
                               nullptr, &cbData);
    if (res != ERROR_SUCCESS || type != REG_SZ || cbData < sizeof(wchar_t)) {
        RegCloseKey(key);
        return false;
    }

    std::wstring value;
    value.resize(cbData / sizeof(wchar_t));
    res = RegQueryValueExW(key, exeNameLower.c_str(), nullptr, &type,
                           (LPBYTE)value.data(), &cbData);
    RegCloseKey(key);
    if (res != ERROR_SUCCESS || type != REG_SZ)
        return false;

    if (!value.empty() && value.back() == L'\0')
        value.pop_back();

    *outMonitorDevice = std::move(value);
    return !outMonitorDevice->empty();
}

static void SaveRuleForExe(const std::wstring& exeNameLower,
                           const std::wstring& monitorDevice) {
    HKEY key{};
    DWORD disposition = 0;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, kRegRulesPath, 0, nullptr, 0,
                        KEY_WRITE, nullptr, &key, &disposition) !=
        ERROR_SUCCESS) {
        return;
    }

    const DWORD cbData =
        (DWORD)((monitorDevice.size() + 1) * sizeof(wchar_t));
    RegSetValueExW(key, exeNameLower.c_str(), 0, REG_SZ,
                   (const BYTE*)monitorDevice.c_str(), cbData);
    RegCloseKey(key);
}

static const MonitorDesc* FindMonitorByDevice(
    const std::vector<MonitorDesc>& monitors,
    const std::wstring& device) {
    for (const auto& m : monitors) {
        if (_wcsicmp(m.device.c_str(), device.c_str()) == 0)
            return &m;
    }
    return nullptr;
}

static const MonitorDesc* FindPrimaryMonitor(
    const std::vector<MonitorDesc>& monitors) {
    for (const auto& m : monitors) {
        if (m.primary)
            return &m;
    }
    return monitors.empty() ? nullptr : &monitors[0];
}

static void MoveWindowToMonitor(HWND hwnd, const MonitorDesc& monitor) {
    RECT r{};
    if (!GetWindowRect(hwnd, &r))
        return;

    const int w = r.right - r.left;
    const int h = r.bottom - r.top;
    const int workW = monitor.work.right - monitor.work.left;
    const int workH = monitor.work.bottom - monitor.work.top;

    int x = monitor.work.left + (workW - w) / 2;
    int y = monitor.work.top + (workH - h) / 2;

    if (w > workW)
        x = monitor.work.left;
    else if (x < monitor.work.left)
        x = monitor.work.left;
    else if (x + w > monitor.work.right)
        x = monitor.work.right - w;

    if (h > workH)
        y = monitor.work.top;
    else if (y < monitor.work.top)
        y = monitor.work.top;
    else if (y + h > monitor.work.bottom)
        y = monitor.work.bottom - h;

    SetWindowPos(hwnd, nullptr, x, y, 0, 0,
                 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

static bool ShouldConsiderTopLevelWindow(HWND hwnd) {
    if (!IsWindow(hwnd))
        return false;
    if (!IsWindowVisible(hwnd))
        return false;
    if (GetWindow(hwnd, GW_OWNER) != nullptr)
        return false;

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW)
        return false;

    return true;
}

static bool GetExeNameLowerForHwnd(HWND hwnd, std::wstring* outExeLower) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid)
        return false;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess)
        return false;

    wchar_t pathBuf[32768];
    DWORD size = (DWORD)(sizeof(pathBuf) / sizeof(pathBuf[0]));
    bool ok = QueryFullProcessImageNameW(hProcess, 0, pathBuf, &size) != 0;
    CloseHandle(hProcess);
    if (!ok || !size)
        return false;

    *outExeLower = ToLower(BasenameFromPath(std::wstring(pathBuf, size)));
    return true;
}

struct ApplyRuleCtx {
    const std::wstring* exeLower;
    const MonitorDesc* monitor;
};

static BOOL CALLBACK ApplyRuleEnumProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = (const ApplyRuleCtx*)lParam;
    if (!ShouldConsiderTopLevelWindow(hwnd))
        return TRUE;

    std::wstring wexe;
    if (!GetExeNameLowerForHwnd(hwnd, &wexe))
        return TRUE;

    if (_wcsicmp(wexe.c_str(), ctx->exeLower->c_str()) == 0) {
        MoveWindowToMonitor(hwnd, *ctx->monitor);
    }
    return TRUE;
}

static void ApplyRuleToAllWindowsForExe(const std::wstring& exeLower,
                                       const MonitorDesc& monitor) {
    ApplyRuleCtx ctx{&exeLower, &monitor};
    EnumWindows(ApplyRuleEnumProc, (LPARAM)&ctx);
}

static void LoadSettings() {
    settings.pollIntervalMs = Wh_GetIntSetting(L"pollIntervalMs");
    if (settings.pollIntervalMs < 100)
        settings.pollIntervalMs = 100;
    if (settings.pollIntervalMs > 10000)
        settings.pollIntervalMs = 10000;
}

// -------------------- Enforcement (per process) --------------------

struct EnforceCtx {
    DWORD pid;
    std::unordered_set<HWND>* processed;
    const MonitorDesc* target;
};

static BOOL CALLBACK EnforceEnumProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = (EnforceCtx*)lParam;

    if (!ShouldConsiderTopLevelWindow(hwnd))
        return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != ctx->pid)
        return TRUE;

    if (ctx->processed->find(hwnd) != ctx->processed->end())
        return TRUE;

    MoveWindowToMonitor(hwnd, *ctx->target);
    ctx->processed->insert(hwnd);
    return TRUE;
}

static DWORD WINAPI EnforceThreadProc(LPVOID) {
    const DWORD myPid = GetCurrentProcessId();
    std::unordered_set<HWND> processed;
    processed.reserve(64);

    std::wstring myExeLower = GetCurrentProcessExeNameLower();

    while (WaitForSingleObject(g_stopEvent, (DWORD)settings.pollIntervalMs) ==
           WAIT_TIMEOUT) {
        std::wstring targetDevice;
        if (!LoadRuleForExe(myExeLower, &targetDevice))
            continue;

        auto monitors = EnumerateMonitors();
        const MonitorDesc* target =
            FindMonitorByDevice(monitors, targetDevice);
        if (!target)
            target = FindPrimaryMonitor(monitors);
        if (!target)
            continue;

        EnforceCtx ctx{myPid, &processed, target};
        EnumWindows(EnforceEnumProc, (LPARAM)&ctx);
    }

    return 0;
}

// -------------------- Hotkey UI (runs in explorer.exe) --------------------

static constexpr wchar_t kUiClassName[] = L"WhProgramMonitorPinUI";
static HWND g_uiHwnd = nullptr;
static HWND g_listHwnd = nullptr;
static HWND g_comboHwnd = nullptr;
static HWND g_applyBtnHwnd = nullptr;
static HWND g_refreshBtnHwnd = nullptr;

static std::vector<std::wstring> g_programs;     // exeLower list
static std::vector<MonitorDesc> g_uiMonitors;    // monitor list

static std::wstring GetSavedMonitorColumnText(const std::wstring& exeLower) {
    std::wstring device;
    if (!LoadRuleForExe(exeLower, &device))
        return L"";

    for (int i = 0; i < (int)g_uiMonitors.size(); i++) {
        if (_wcsicmp(g_uiMonitors[i].device.c_str(), device.c_str()) == 0) {
            return std::to_wstring(i + 1);
        }
    }

    return L"?";
}

struct RefreshProgramsCtx {
    std::unordered_set<std::wstring>* seen;
};

static BOOL CALLBACK RefreshProgramsEnumProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = (RefreshProgramsCtx*)lParam;
    if (!ShouldConsiderTopLevelWindow(hwnd))
        return TRUE;

    std::wstring exeLower;
    if (!GetExeNameLowerForHwnd(hwnd, &exeLower))
        return TRUE;
    if (exeLower.empty())
        return TRUE;
    if (_wcsicmp(exeLower.c_str(), L"windhawk.exe") == 0)
        return TRUE;

    if (ctx->seen->insert(exeLower).second) {
        g_programs.push_back(exeLower);
    }
    return TRUE;
}

static void RefreshProgramsList() {
    g_programs.clear();

    std::unordered_set<std::wstring> seen;
    seen.reserve(64);

    RefreshProgramsCtx ctx{&seen};
    EnumWindows(RefreshProgramsEnumProc, (LPARAM)&ctx);

    std::sort(g_programs.begin(), g_programs.end(),
              [](const std::wstring& a, const std::wstring& b) {
                  return _wcsicmp(a.c_str(), b.c_str()) < 0;
              });

    if (g_listHwnd) {
        ListView_DeleteAllItems(g_listHwnd);
        for (int i = 0; i < (int)g_programs.size(); i++) {
            const auto& exeLower = g_programs[i];

            LVITEMW item{};
            item.mask = LVIF_TEXT;
            item.iItem = i;
            item.iSubItem = 0;
            item.pszText = (LPWSTR)exeLower.c_str();
            ListView_InsertItem(g_listHwnd, &item);

            std::wstring saved = GetSavedMonitorColumnText(exeLower);
            ListView_SetItemText(g_listHwnd, i, 1, (LPWSTR)saved.c_str());
        }

        if (!g_programs.empty()) {
            ListView_SetItemState(g_listHwnd, 0, LVIS_SELECTED | LVIS_FOCUSED,
                                  LVIS_SELECTED | LVIS_FOCUSED);
            ListView_EnsureVisible(g_listHwnd, 0, FALSE);
        }
    }
}

static std::wstring MonitorLabel(int index, const MonitorDesc& m) {
    const int w = m.work.right - m.work.left;
    const int h = m.work.bottom - m.work.top;

    wchar_t buf[256];
    wsprintfW(buf, L"Monitor %d%s (%s, %dx%d)", index + 1,
              m.primary ? L" (Primary)" : L"", m.device.c_str(), w, h);
    return buf;
}

static void RefreshMonitors() {
    g_uiMonitors = EnumerateMonitors();

    if (g_comboHwnd) {
        SendMessageW(g_comboHwnd, CB_RESETCONTENT, 0, 0);
        for (int i = 0; i < (int)g_uiMonitors.size(); i++) {
            std::wstring label = MonitorLabel(i, g_uiMonitors[i]);
            LRESULT idx =
                SendMessageW(g_comboHwnd, CB_ADDSTRING, 0, (LPARAM)label.c_str());
            SendMessageW(g_comboHwnd, CB_SETITEMDATA, idx, (LPARAM)i);
        }

        int selectIndex = 0;
        for (int i = 0; i < (int)g_uiMonitors.size(); i++) {
            if (g_uiMonitors[i].primary) {
                selectIndex = i;
                break;
            }
        }
        SendMessageW(g_comboHwnd, CB_SETCURSEL, selectIndex, 0);
    }
}

static void ApplySelectedRuleFromUi() {
    if (!g_listHwnd || !g_comboHwnd)
        return;

    const int progSel = ListView_GetNextItem(g_listHwnd, -1, LVNI_SELECTED);
    const int monSel = (int)SendMessageW(g_comboHwnd, CB_GETCURSEL, 0, 0);
    if (progSel < 0 || monSel == CB_ERR)
        return;
    if (progSel < 0 || progSel >= (int)g_programs.size())
        return;

    const int monIndex =
        (int)SendMessageW(g_comboHwnd, CB_GETITEMDATA, monSel, 0);
    if (monIndex < 0 || monIndex >= (int)g_uiMonitors.size())
        return;

    const std::wstring exeLower = g_programs[progSel];
    const std::wstring device = g_uiMonitors[monIndex].device;

    SaveRuleForExe(exeLower, device);
    ApplyRuleToAllWindowsForExe(exeLower, g_uiMonitors[monIndex]);
    RefreshProgramsList();
}

static LRESULT CALLBACK UiWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                  LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            const int margin = 10;
            const int w = 520;
            const int h = 370;

            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, w, h,
                         SWP_NOMOVE | SWP_SHOWWINDOW);

            CreateWindowExW(0, L"STATIC", L"Programs (open windows):",
                            WS_CHILD | WS_VISIBLE, margin, margin, 250, 18,
                            hwnd, nullptr, nullptr, nullptr);
            CreateWindowExW(0, L"STATIC", L"Open on monitor:",
                            WS_CHILD | WS_VISIBLE, 280, margin, 220, 18, hwnd,
                            nullptr, nullptr, nullptr);

            INITCOMMONCONTROLSEX icc{};
            icc.dwSize = sizeof(icc);
            icc.dwICC = ICC_LISTVIEW_CLASSES;
            InitCommonControlsEx(&icc);

            g_listHwnd =
                CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, nullptr,
                                WS_CHILD | WS_VISIBLE | WS_VSCROLL |
                                    LVS_REPORT | LVS_SINGLESEL |
                                    LVS_SHOWSELALWAYS,
                                margin, margin + 22, 250, 260, hwnd,
                                (HMENU)1001, nullptr, nullptr);

            ListView_SetExtendedListViewStyle(
                g_listHwnd, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

            LVCOLUMNW col{};
            col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

            col.iSubItem = 0;
            col.cx = 160;
            col.pszText = (LPWSTR)L"Program";
            ListView_InsertColumn(g_listHwnd, 0, &col);

            col.iSubItem = 1;
            col.cx = 80;
            col.pszText = (LPWSTR)L"Saved";
            ListView_InsertColumn(g_listHwnd, 1, &col);
            g_comboHwnd =
                CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                                280, margin + 22, 220, 200, hwnd,
                                (HMENU)1002, nullptr, nullptr);

            g_applyBtnHwnd = CreateWindowExW(
                0, L"BUTTON", L"Apply", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                280, margin + 62, 100, 28, hwnd, (HMENU)1003, nullptr, nullptr);

            g_refreshBtnHwnd = CreateWindowExW(
                0, L"BUTTON", L"Refresh", WS_CHILD | WS_VISIBLE,
                400, margin + 62, 100, 28, hwnd, (HMENU)1004, nullptr, nullptr);

            CreateWindowExW(
                0, L"STATIC",
                L"Tip: This moves windows after they open (best-effort).",
                WS_CHILD | WS_VISIBLE, margin, margin + 290, 490, 36, hwnd,
                nullptr, nullptr, nullptr);

            RefreshMonitors();
            RefreshProgramsList();
            return 0;
        }
        case WM_COMMAND: {
            const int id = LOWORD(wParam);
            const int code = HIWORD(wParam);
            if (id == 1003 && code == BN_CLICKED) {
                ApplySelectedRuleFromUi();
                return 0;
            }
            if (id == 1004 && code == BN_CLICKED) {
                RefreshMonitors();
                RefreshProgramsList();
                return 0;
            }
            return 0;
        }
        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        case WM_DESTROY:
            g_uiHwnd = nullptr;
            g_listHwnd = nullptr;
            g_comboHwnd = nullptr;
            g_applyBtnHwnd = nullptr;
            g_refreshBtnHwnd = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void ShowUi() {
    if (g_uiHwnd) {
        ShowWindow(g_uiHwnd, SW_SHOW);
        SetForegroundWindow(g_uiHwnd);
        RefreshMonitors();
        RefreshProgramsList();
        return;
    }

    WNDCLASSW wc{};
    wc.lpfnWndProc = UiWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kUiClassName;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    g_uiHwnd = CreateWindowExW(WS_EX_TOOLWINDOW, kUiClassName,
                               L"Program Monitor Pin (Win+Ctrl+Shift+F12)",
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                               CW_USEDEFAULT, CW_USEDEFAULT, 520, 330, nullptr,
                               nullptr, wc.hInstance, nullptr);
    if (g_uiHwnd) {
        ShowWindow(g_uiHwnd, SW_SHOW);
        SetForegroundWindow(g_uiHwnd);
    }
}

static DWORD WINAPI HotkeyThreadProc(LPVOID) {
    if (!RegisterHotKey(nullptr, kHotkeyId, kHotkeyModifiers, kHotkeyVk)) {
        if (!g_hotkeyErrorShown) {
            g_hotkeyErrorShown = true;
            MessageBoxW(
                nullptr,
                L"Program Monitor Pin: Win+Ctrl+Shift+F12 hotkey registration failed.\n\n"
                L"Most likely another app/mod is already using Win+Ctrl+Shift+F12.",
                L"Windhawk Mod",
                MB_OK | MB_ICONWARNING);
        }
        return 0;
    }

    MSG msg{};
    HANDLE handles[1] = {g_stopEvent};
    while (true) {
        DWORD wait =
            MsgWaitForMultipleObjects(1, handles, FALSE, INFINITE, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) {
            break;
        }

        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyId) {
                ShowUi();
            } else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    UnregisterHotKey(nullptr, kHotkeyId);
    if (g_uiHwnd)
        DestroyWindow(g_uiHwnd);
    return 0;
}

// -------------------- Windhawk entrypoints --------------------

BOOL Wh_ModInit() {
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent)
        return FALSE;

    g_enforceThread =
        CreateThread(nullptr, 0, EnforceThreadProc, nullptr, 0, nullptr);

    if (IsExplorerProcess()) {
        g_hotkeyThread =
            CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0, nullptr);
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_stopEvent)
        SetEvent(g_stopEvent);

    if (g_hotkeyThread) {
        WaitForSingleObject(g_hotkeyThread, 3000);
        CloseHandle(g_hotkeyThread);
        g_hotkeyThread = nullptr;
    }

    if (g_enforceThread) {
        WaitForSingleObject(g_enforceThread, 3000);
        CloseHandle(g_enforceThread);
        g_enforceThread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
