// ==WindhawkMod==
// @id              per-monitor-scale-switcher
// @name            Per-Monitor Scale Switcher
// @description     Per-monitor DPI scaling based on friendly name, auto-applied on connect/disconnect.
// @version         1.0
// @author          @jtnqr
// @github          https://github.com/jtnqr
// @include         explorer.exe
// @compilerOptions -luser32 -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Automatically applies per-monitor DPI scaling based on monitor friendly names (e.g. "BOE0998").
Each monitor can have independent scales for single and multi-monitor setups.
Monitors not listed in settings are left untouched.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- monitors:
  - "BOE0998,100,125"
  $name: Per-monitor rules
  $description: >-
    One entry per monitor. Format: ID,scaleSingle,scaleMulti
 
 
    ID: the friendly name shown in Windhawk logs when a display change occurs (e.g. BOE0998).
 
 
    scaleSingle: scale % to use when this is the only active monitor.
 
 
    scaleMulti: scale % to use when other monitors are also active.
 
 
    Valid scale values: 100 125 150 175 200 225 250 300 350 400 450 500.
 
 
    Monitors not listed are left untouched.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellscalingapi.h>
#include <vector>
#include <string>
#include <unordered_map>

#define DISPLAYCONFIG_DEVICE_INFO_GET_DPI_SCALE ((DISPLAYCONFIG_DEVICE_INFO_TYPE)0xFFFFFFFD)
#define DISPLAYCONFIG_DEVICE_INFO_SET_DPI_SCALE ((DISPLAYCONFIG_DEVICE_INFO_TYPE)0xFFFFFFFC)
#define RETRY_TIMER_ID 1
#define RETRY_DELAY_MS 1500  // wait longer than Windows' own settle time

struct DISPLAYCONFIG_SOURCE_DPI_SCALE_GET {
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    int minScaleRel; // min steps relative to recommended
    int maxScaleRel; // max steps relative to recommended
    int curScaleRel; // current steps relative to recommended
};

struct DISPLAYCONFIG_SOURCE_DPI_SCALE_SET {
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    int scaleRel;
};

// All DPI scale percentages Windows supports, in order
static const int DPI_SCALE_STEPS[] = {
    100, 125, 150, 175, 200,
    225, 250, 300, 350, 400, 450, 500
};
static const int DPI_SCALE_STEP_COUNT = ARRAYSIZE(DPI_SCALE_STEPS);

struct MonitorConfig {
    std::wstring id;
    int scaleSingle; // % when alone
    int scaleMulti;  // % when with other monitors
};

CRITICAL_SECTION g_settingsLock;
std::vector<MonitorConfig> g_monitorConfigs;

HWND   g_hMessageWindow = NULL;
HANDLE g_hWindowThread  = NULL;
int    g_lastMonitorCount = -1;

// ── Settings ─────────────────────────────────────────────────────────────────

void LoadSettings() {
    EnterCriticalSection(&g_settingsLock);
    g_monitorConfigs.clear();

    for (int i = 0; ; i++) {
        wchar_t key[64];
        wsprintfW(key, L"monitors[%d]", i);

        PCWSTR val = Wh_GetStringSetting(key);
        if (!val || val[0] == L'\0') {
            Wh_FreeStringSetting(val);
            break;
        }

        // Parse "BOE0998,100,125"
        wchar_t buf[128];
        wcsncpy(buf, val, 127);
        buf[127] = L'\0';
        Wh_FreeStringSetting(val);

        wchar_t* ctx = nullptr;
        wchar_t* name   = wcstok(buf,    L",", &ctx);
        wchar_t* single = wcstok(nullptr, L",", &ctx);
        wchar_t* multi  = wcstok(nullptr, L",", &ctx);

        if (!name || !single || !multi) continue;

        MonitorConfig cfg;
        cfg.id          = name;
        cfg.scaleSingle = _wtoi(single);
        cfg.scaleMulti  = _wtoi(multi);
        g_monitorConfigs.push_back(std::move(cfg));
    }

    LeaveCriticalSection(&g_settingsLock);
}

// ── Display config helpers ────────────────────────────────────────────────────

struct MonitorPathInfo {
    LUID   adapterId;
    UINT32 sourceId;
    UINT32 targetId;
    LUID   targetAdapterId;
};

// Returns all active display paths, keyed by GDI device name (e.g. "\\.\DISPLAY1")
static bool QueryAllDisplayPaths(std::unordered_map<std::wstring, MonitorPathInfo>& outPaths) {
    UINT32 pathCount = 0, modeCount = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS) {
        Wh_Log(L"[PerMonitorScaleSwitcher] GetDisplayConfigBufferSizes failed");
        return false;
    }

    std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(),
                           &modeCount, modes.data(), nullptr) != ERROR_SUCCESS) {
        Wh_Log(L"[PerMonitorScaleSwitcher] QueryDisplayConfig failed");
        return false;
    }

    for (UINT32 i = 0; i < pathCount; i++) {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME src = {};
        src.header.type      = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        src.header.size      = sizeof(src);
        src.header.adapterId = paths[i].sourceInfo.adapterId;
        src.header.id        = paths[i].sourceInfo.id;

        if (DisplayConfigGetDeviceInfo(&src.header) != ERROR_SUCCESS) {
            Wh_Log(L"[PerMonitorScaleSwitcher] GetSourceName failed for path %u", i);
            continue;
        }

        Wh_Log(L"[PerMonitorScaleSwitcher] Display path: GDI=%s", src.viewGdiDeviceName);

        MonitorPathInfo info;
        info.adapterId       = paths[i].sourceInfo.adapterId;
        info.sourceId        = paths[i].sourceInfo.id;
        info.targetId        = paths[i].targetInfo.id;
        info.targetAdapterId = paths[i].targetInfo.adapterId;
        outPaths[src.viewGdiDeviceName] = info;
    }

    Wh_Log(L"[PerMonitorScaleSwitcher] Total active paths: %zu", outPaths.size());
    return !outPaths.empty();
}

// Gets the monitor's friendly name (e.g. "BOE0998") from its HMONITOR
static bool GetMonitorFriendlyName(HMONITOR hMonitor,
                            const std::unordered_map<std::wstring, MonitorPathInfo>& paths,
                            std::wstring& outName,
                            MonitorPathInfo& outInfo) {
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMonitor, &mi)) {
        Wh_Log(L"[PerMonitorScaleSwitcher] GetMonitorInfoW failed");
        return false;
    }

    Wh_Log(L"[PerMonitorScaleSwitcher] GDI device = %s", mi.szDevice);

    auto it = paths.find(mi.szDevice);
    if (it == paths.end()) {
        Wh_Log(L"[PerMonitorScaleSwitcher] No display path for GDI device '%s'", mi.szDevice);
        return false;
    }
    outInfo = it->second;

    DISPLAYCONFIG_TARGET_DEVICE_NAME target = {};
    target.header.type      = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
    target.header.size      = sizeof(target);
    target.header.adapterId = outInfo.targetAdapterId;
    target.header.id        = outInfo.targetId;

    if (DisplayConfigGetDeviceInfo(&target.header) != ERROR_SUCCESS) {
        Wh_Log(L"[PerMonitorScaleSwitcher] GetTargetName failed for '%s'");
        return false;
    }

    outName = target.monitorFriendlyDeviceName;

    // Fallback: some internal panels (e.g. laptop screens) leave friendlyDeviceName
    // empty. Extract the model token from the device path instead.
    // monitorDevicePath looks like: \\?\DISPLAY#BOE0998#...
    if (outName.empty()) {
        std::wstring path = target.monitorDevicePath;
        Wh_Log(L"[PerMonitorScaleSwitcher] Friendly name empty, trying path: '%s'", path.c_str());

        // Find second '#', model ID is between 2nd and 3rd '#'
        size_t first = path.find(L'#');
        if (first != std::wstring::npos) {
            size_t second = path.find(L'#', first + 1);
            if (second != std::wstring::npos)
                outName = path.substr(first + 1, second - first - 1);
        }

        if (!outName.empty())
            Wh_Log(L"[PerMonitorScaleSwitcher] Resolved name from path: '%s'", outName.c_str());
        else
            Wh_Log(L"[PerMonitorScaleSwitcher] Could not resolve name from path either, skipping");
    }

    Wh_Log(L"[PerMonitorScaleSwitcher] Monitor friendly name = '%s'", outName.c_str());
    return !outName.empty();
}

// ── Scale math ───────────────────────────────────────────────────────────────

static int FindScaleStepIndex(int percent) {
    for (int i = 0; i < DPI_SCALE_STEP_COUNT; i++)
        if (DPI_SCALE_STEPS[i] == percent) return i;
    return -1;
}

static bool SetMonitorScalePercent(HMONITOR hMonitor, const MonitorPathInfo& info, int targetPercent) {
    Sleep(500);

    DISPLAYCONFIG_SOURCE_DPI_SCALE_GET getInfo = {};
    getInfo.header.type      = DISPLAYCONFIG_DEVICE_INFO_GET_DPI_SCALE;
    getInfo.header.size      = sizeof(getInfo);
    getInfo.header.adapterId = info.adapterId;
    getInfo.header.id        = info.sourceId;
    if (DisplayConfigGetDeviceInfo(&getInfo.header) != ERROR_SUCCESS) return false;

    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    int currentPercent = static_cast<int>(dpiX * 100u / 96u);

    int currentAbsIdx = FindScaleStepIndex(currentPercent);
    int targetAbsIdx  = FindScaleStepIndex(targetPercent);
    if (currentAbsIdx < 0 || targetAbsIdx < 0) {
        Wh_Log(L"[PerMonitorScaleSwitcher] targetPercent=%d is not a valid DPI step", currentPercent, targetPercent);
        return false;
    }

    if (currentPercent == targetPercent) {
        Wh_Log(L"[PerMonitorScaleSwitcher] Already at %d%%, skipping", targetPercent);
        return true;
    }

    // Special case: Windows sometimes reports a collapsed range (min==max)
    // when the scale state is stale after a display change. In that case,
    // setting scaleRel=0 resets to "recommended" and usually unsticks things.
    if (getInfo.minScaleRel == getInfo.maxScaleRel && currentAbsIdx >= 0) {
        Wh_Log(L"[PerMonitorScaleSwitcher] Collapsed range (min==max==%d); "
            L"attempting scaleRel=0 to unstick", getInfo.minScaleRel);

        DISPLAYCONFIG_SOURCE_DPI_SCALE_SET setInfo = {};
        setInfo.header.type      = DISPLAYCONFIG_DEVICE_INFO_SET_DPI_SCALE;
        setInfo.header.size      = sizeof(setInfo);
        setInfo.header.adapterId = info.adapterId;
        setInfo.header.id        = info.sourceId;
        setInfo.scaleRel         = 0;
        DisplayConfigSetDeviceInfo(&setInfo.header);

        Sleep(500);
        GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
        int verifyPercent = static_cast<int>(dpiX * 100u / 96u);
        Wh_Log(L"[PerMonitorScaleSwitcher] Post-unstick DPI: %d%%", verifyPercent);
        if (verifyPercent == targetPercent) return true;

        // Re-query — range may have expanded after the reset.
        getInfo = {};
        getInfo.header.type      = DISPLAYCONFIG_DEVICE_INFO_GET_DPI_SCALE;
        getInfo.header.size      = sizeof(getInfo);
        getInfo.header.adapterId = info.adapterId;
        getInfo.header.id        = info.sourceId;
        if (DisplayConfigGetDeviceInfo(&getInfo.header) != ERROR_SUCCESS) {
            Wh_Log(L"[PerMonitorScaleSwitcher] Re-query after unstick failed");
            return false;
        }
    }

    int targetRel = INT_MAX;
    int usedAnchor = -1;

    auto tryAnchor = [&](int anchor, const wchar_t* strategy) -> bool {
        if (anchor < 0 || anchor >= DPI_SCALE_STEP_COUNT) return false;
        int rel = targetAbsIdx - anchor;
        if (rel < getInfo.minScaleRel || rel > getInfo.maxScaleRel) return false;
        targetRel  = rel;
        usedAnchor = anchor;
        Wh_Log(L"[PerMonitorScaleSwitcher] Anchor strategy: %s — anchor=%d rel=%d",
            strategy, anchor, rel);
        return true;
    };

    // Strategy 1: trust Windows' claimed anchor (currentAbsIdx - curScaleRel).
    if (currentAbsIdx >= 0)
        tryAnchor(currentAbsIdx - getInfo.curScaleRel, L"claimed");

    // Strategy 2: brute-force — find anchor where both current AND target fit.
    if (targetRel == INT_MAX) {
        for (int anchor = 0; anchor < DPI_SCALE_STEP_COUNT && targetRel == INT_MAX; anchor++) {
            if (currentAbsIdx >= 0) {
                int curRel = currentAbsIdx - anchor;
                if (curRel < getInfo.minScaleRel || curRel > getInfo.maxScaleRel) continue;
            }
            tryAnchor(anchor, L"brute-force");
        }
    }

    // Strategy 3: relaxed — find any anchor where the target fits.
    if (targetRel == INT_MAX) {
        for (int anchor = 0; anchor < DPI_SCALE_STEP_COUNT && targetRel == INT_MAX; anchor++)
            tryAnchor(anchor, L"relaxed");
    }

    if (targetRel == INT_MAX) {
        Wh_Log(L"[PerMonitorScaleSwitcher] No valid anchor found for currentPct=%d targetPct=%d "
               L"minRel=%d maxRel=%d", currentPercent, targetPercent,
               getInfo.minScaleRel, getInfo.maxScaleRel);
        return false;
    }

    Wh_Log(L"[PerMonitorScaleSwitcher] anchor=%d curRel=%d minRel=%d maxRel=%d "
           L"currentPct=%d targetPct=%d targetRel=%d",
           usedAnchor, getInfo.curScaleRel, getInfo.minScaleRel, getInfo.maxScaleRel,
           currentPercent, targetPercent, targetRel);

    DISPLAYCONFIG_SOURCE_DPI_SCALE_SET setInfo = {};
    setInfo.header.type      = DISPLAYCONFIG_DEVICE_INFO_SET_DPI_SCALE;
    setInfo.header.size      = sizeof(setInfo);
    setInfo.header.adapterId = info.adapterId;
    setInfo.header.id        = info.sourceId;
    setInfo.scaleRel         = targetRel;

    LONG result = DisplayConfigSetDeviceInfo(&setInfo.header);
    Wh_Log(L"[PerMonitorScaleSwitcher] SET_DPI_SCALE returned %ld", result);

    Sleep(500);
    GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    int verifyPercent = static_cast<int>(dpiX * 100u / 96u);
    Wh_Log(L"[PerMonitorScaleSwitcher] Verified post-set DPI: %d%%", verifyPercent);

    if (verifyPercent != targetPercent) {
        Wh_Log(L"[PerMonitorScaleSwitcher] Verification failed", verifyPercent, targetPercent);
        return false;
    }

    return true;
}

// ── Core logic ────────────────────────────────────────────────────────────────

static std::vector<HMONITOR> GetAllMonitors() {
    std::vector<HMONITOR> monitors;
    EnumDisplayMonitors(nullptr, nullptr,
        [](HMONITOR hMon, HDC, LPRECT, LPARAM lp) CALLBACK -> BOOL {
            reinterpret_cast<std::vector<HMONITOR>*>(lp)->push_back(hMon);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&monitors));
    return monitors;
}

// Returns true if all configured monitors are at their target scale
bool HandleDisplayChange(int monitorCount) {
    Wh_Log(L"[PerMonitorScaleSwitcher] HandleDisplayChange: monitorCount=%d", monitorCount);

    EnterCriticalSection(&g_settingsLock);
    std::vector<MonitorConfig> configs = g_monitorConfigs;
    LeaveCriticalSection(&g_settingsLock);

    if (configs.empty()) return true;

    std::unordered_map<std::wstring, const MonitorConfig*> configMap;
    for (const auto& c : configs)
        configMap[c.id] = &c;

    std::unordered_map<std::wstring, MonitorPathInfo> displayPaths;
    if (!QueryAllDisplayPaths(displayPaths)) {
        Wh_Log(L"[PerMonitorScaleSwitcher] Could not query display paths — aborting");
        return false;
    }

    bool allOk = true;
    for (HMONITOR hMon : GetAllMonitors()) {
        std::wstring friendlyName;
        MonitorPathInfo pathInfo;
        if (!GetMonitorFriendlyName(hMon, displayPaths, friendlyName, pathInfo))
            continue;

        auto it = configMap.find(friendlyName);
        if (it == configMap.end()) {
            Wh_Log(L"[PerMonitorScaleSwitcher] '%s' not in config, skipping", friendlyName.c_str());
            continue;
        }

        int targetScale = (monitorCount > 1)
            ? it->second->scaleMulti
            : it->second->scaleSingle;

        Wh_Log(L"[PerMonitorScaleSwitcher] Applying %d%% to '%s'", targetScale, friendlyName.c_str());
        if (!SetMonitorScalePercent(hMon, pathInfo, targetScale))
            allOk = false;
    }

    return allOk;
}

// ── Window thread (listens for WM_DISPLAYCHANGE) ──────────────────────────────

int g_pendingMonitorCount = -1; // monitor count to retry with

LRESULT CALLBACK MessageWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DISPLAYCHANGE) {
        int count = GetSystemMetrics(SM_CMONITORS);
        Wh_Log(L"[PerMonitorScaleSwitcher] WM_DISPLAYCHANGE received, monitor count: %d", count);
        KillTimer(hwnd, RETRY_TIMER_ID);
        g_pendingMonitorCount = count;
        SetTimer(hwnd, RETRY_TIMER_ID, RETRY_DELAY_MS, nullptr);
    }
    else if (msg == WM_TIMER && wParam == RETRY_TIMER_ID) {
        KillTimer(hwnd, RETRY_TIMER_ID);
        Wh_Log(L"[PerMonitorScaleSwitcher] Timer fired, applying scale...");
        bool ok = HandleDisplayChange(g_pendingMonitorCount);
        if (!ok)
            Wh_Log(L"[PerMonitorScaleSwitcher] Apply incomplete — not retrying to avoid loop");
        // No re-arm — done either way
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD g_dwWindowThreadId = 0; // store thread ID for reliable PostThreadMessage

DWORD WINAPI WindowThread(LPVOID) {
    HINSTANCE hInst = GetModuleHandle(NULL);
 
    WNDCLASS wc      = {};
    wc.lpfnWndProc   = MessageWndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = L"WindhawkAutoDisplayScalerWnd";
    RegisterClass(&wc);
 
    // Must be a visible top-level window (not HWND_MESSAGE) to receive
    // WM_DISPLAYCHANGE broadcasts. We keep it hidden via SW_HIDE.
    g_hMessageWindow = CreateWindowEx(
        0, wc.lpszClassName, nullptr,
        WS_POPUP,
        0, 0, 0, 0,
        nullptr, nullptr, hInst, nullptr);
 
    if (!g_hMessageWindow) {
        Wh_Log(L"[PerMonitorScaleSwitcher] Failed to create message window");
        UnregisterClass(wc.lpszClassName, hInst);
        return 1;
    }
 
    ShowWindow(g_hMessageWindow, SW_HIDE);
 
    // Apply scale once at startup after the settle delay.
    Wh_Log(L"[PerMonitorScaleSwitcher] Window thread started — scheduling initial scale");
    g_pendingMonitorCount = GetSystemMetrics(SM_CMONITORS);
    SetTimer(g_hMessageWindow, RETRY_TIMER_ID, RETRY_DELAY_MS, nullptr);
 
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
 
    DestroyWindow(g_hMessageWindow);
    g_hMessageWindow = NULL;
    UnregisterClass(wc.lpszClassName, hInst);
    Wh_Log(L"[PerMonitorScaleSwitcher] Window thread exiting");
    return 0;
}

// ── Windhawk entry points ─────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"[PerMonitorScaleSwitcher] Initializing");
    InitializeCriticalSection(&g_settingsLock);
    LoadSettings();
 
    g_hWindowThread = CreateThread(NULL, 0, WindowThread, NULL, 0, &g_dwWindowThreadId);
    if (!g_hWindowThread) {
        Wh_Log(L"[PerMonitorScaleSwitcher] Failed to create window thread");
        DeleteCriticalSection(&g_settingsLock);
        return FALSE;
    }
 
    Wh_Log(L"[PerMonitorScaleSwitcher] Initialized (thread ID: %lu)", g_dwWindowThreadId);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[PerMonitorScaleSwitcher] Uninitializing");
 
    // PostThreadMessage works even before the window is created, unlike
    // PostMessage(g_hMessageWindow, ...) which requires a valid HWND.
    if (g_dwWindowThreadId)
        PostThreadMessage(g_dwWindowThreadId, WM_QUIT, 0, 0);
 
    if (g_hWindowThread) {
        if (WaitForSingleObject(g_hWindowThread, 3000) == WAIT_TIMEOUT)
            Wh_Log(L"[PerMonitorScaleSwitcher] Thread exit timed out");
        else
            Wh_Log(L"[PerMonitorScaleSwitcher] Thread exited cleanly");
 
        CloseHandle(g_hWindowThread);
        g_hWindowThread    = NULL;
        g_dwWindowThreadId = 0;
    }
 
    DeleteCriticalSection(&g_settingsLock);
    Wh_Log(L"[PerMonitorScaleSwitcher] Done");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[PerMonitorScaleSwitcher] Settings changed, reloading...");
    LoadSettings();
    HandleDisplayChange(GetSystemMetrics(SM_CMONITORS));
}
