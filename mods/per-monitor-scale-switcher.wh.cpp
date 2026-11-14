// ==WindhawkMod==
// @id              per-monitor-scale-switcher
// @name            Per-Monitor Scale Switcher
// @description     Per-monitor DPI scaling based on monitor friendly name.
// @version         1.0
// @author          jtnqr
// @github          https://github.com/jtnqr
// @include         windhawk.exe
// @compilerOptions -luser32 -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Automatically applies per-monitor DPI scaling based on monitor friendly names
(e.g. "BOE0998"). Each monitor can have independent scales for single and
multi-monitor setups. Monitors not listed in settings are left untouched.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- monitors:
  - - id: BOE0998
      $name: Monitor ID
      $description: Friendly name as shown in Windhawk logs (e.g. BOE0998).
    - scaleSingle: 100
      $name: Scale (single)
      $description: Scale % when this is the only active monitor.
    - scaleMulti: 125
      $name: Scale (multi)
      $description: Scale % when other monitors are also active.
  $name: Per-monitor rules
  $description: One entry per monitor.
*/
// ==/WindhawkModSettings==

#include <shellscalingapi.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <windows.h>

#define DISPLAYCONFIG_DEVICE_INFO_GET_DPI_SCALE                                \
  ((DISPLAYCONFIG_DEVICE_INFO_TYPE)0xFFFFFFFD)
#define DISPLAYCONFIG_DEVICE_INFO_SET_DPI_SCALE                                \
  ((DISPLAYCONFIG_DEVICE_INFO_TYPE)0xFFFFFFFC)
#define RETRY_TIMER_ID 1
#define RETRY_DELAY_MS 1500 // wait longer than Windows' own settle time

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
static const int DPI_SCALE_STEPS[] = {100, 125, 150, 175, 200, 225,
                                      250, 300, 350, 400, 450, 500};
static const int DPI_SCALE_STEP_COUNT = ARRAYSIZE(DPI_SCALE_STEPS);

struct MonitorConfig {
  std::wstring id;
  int scaleSingle; // % when alone
  int scaleMulti;  // % when with other monitors
};

CRITICAL_SECTION g_settingsLock;
std::vector<MonitorConfig> g_monitorConfigs;

HWND g_hMessageWindow = NULL;
HANDLE g_hWindowThread = NULL;
int g_lastMonitorCount = -1;

// ── Settings ─────────────────────────────────────────────────────────────────

// FIX 5: LoadSettings now reads structured fields (monitors[N].id,
// monitors[N].scaleSingle, monitors[N].scaleMulti) instead of parsing
// a comma-delimited string.
void LoadSettings() {
  EnterCriticalSection(&g_settingsLock);
  g_monitorConfigs.clear();

  for (int i = 0;; i++) {
    wchar_t idKey[64], singleKey[64], multiKey[64];
    wsprintfW(idKey, L"monitors[%d].id", i);
    wsprintfW(singleKey, L"monitors[%d].scaleSingle", i);
    wsprintfW(multiKey, L"monitors[%d].scaleMulti", i);

    PCWSTR id = Wh_GetStringSetting(idKey);
    if (!id || id[0] == L'\0') {
      Wh_FreeStringSetting(id);
      break;
    }

    MonitorConfig cfg;
    cfg.id = id;
    cfg.scaleSingle = Wh_GetIntSetting(singleKey);
    cfg.scaleMulti = Wh_GetIntSetting(multiKey);
    Wh_FreeStringSetting(id);
    g_monitorConfigs.push_back(std::move(cfg));
  }

  LeaveCriticalSection(&g_settingsLock);
}

// ── Display config helpers
// ────────────────────────────────────────────────────

struct MonitorPathInfo {
  LUID adapterId;
  UINT32 sourceId;
  UINT32 targetId;
  LUID targetAdapterId;
};

// Returns all active display paths, keyed by GDI device name (e.g.
// "\\.\DISPLAY1")
static bool QueryAllDisplayPaths(
    std::unordered_map<std::wstring, MonitorPathInfo> &outPaths) {
  UINT32 pathCount = 0, modeCount = 0;
  if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount,
                                  &modeCount) != ERROR_SUCCESS) {
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
    src.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
    src.header.size = sizeof(src);
    src.header.adapterId = paths[i].sourceInfo.adapterId;
    src.header.id = paths[i].sourceInfo.id;

    if (DisplayConfigGetDeviceInfo(&src.header) != ERROR_SUCCESS) {
      Wh_Log(L"[PerMonitorScaleSwitcher] GetSourceName failed for path %u", i);
      continue;
    }

    MonitorPathInfo info;
    info.adapterId = paths[i].sourceInfo.adapterId;
    info.sourceId = paths[i].sourceInfo.id;
    info.targetId = paths[i].targetInfo.id;
    info.targetAdapterId = paths[i].targetInfo.adapterId;
    outPaths[src.viewGdiDeviceName] = info;
  }

  return !outPaths.empty();
}

// Gets the monitor's friendly name (e.g. "BOE0998") from its HMONITOR
static bool GetMonitorFriendlyName(
    HMONITOR hMonitor,
    const std::unordered_map<std::wstring, MonitorPathInfo> &paths,
    std::wstring &outName, MonitorPathInfo &outInfo) {
  MONITORINFOEXW mi = {};
  mi.cbSize = sizeof(mi);
  if (!GetMonitorInfoW(hMonitor, &mi)) {
    Wh_Log(L"[PerMonitorScaleSwitcher] GetMonitorInfoW failed");
    return false;
  }

  auto it = paths.find(mi.szDevice);
  if (it == paths.end()) {
    return false;
  }
  outInfo = it->second;

  DISPLAYCONFIG_TARGET_DEVICE_NAME target = {};
  target.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
  target.header.size = sizeof(target);
  target.header.adapterId = outInfo.targetAdapterId;
  target.header.id = outInfo.targetId;

  if (DisplayConfigGetDeviceInfo(&target.header) != ERROR_SUCCESS) {
    // FIX 2: was missing mi.szDevice as the argument for '%s'
    Wh_Log(L"[PerMonitorScaleSwitcher] GetTargetName failed for '%s'",
           mi.szDevice);
    return false;
  }

  outName = target.monitorFriendlyDeviceName;

  // Fallback: some internal panels (e.g. laptop screens) leave
  // friendlyDeviceName empty. Extract the model token from the device path
  // instead. monitorDevicePath looks like: \\?\DISPLAY#BOE0998#...
  if (outName.empty()) {
    std::wstring path = target.monitorDevicePath;

    // Find second '#', model ID is between 2nd and 3rd '#'
    size_t first = path.find(L'#');
    if (first != std::wstring::npos) {
      size_t second = path.find(L'#', first + 1);
      if (second != std::wstring::npos)
        outName = path.substr(first + 1, second - first - 1);
    }

    if (!outName.empty())
      Wh_Log(L"[PerMonitorScaleSwitcher] Friendly name empty, resolved '%s' "
             L"from device path",
             outName.c_str());
    else
      Wh_Log(L"[PerMonitorScaleSwitcher] Could not resolve name from path "
             L"either, skipping");
  }

  return !outName.empty();
}

// ── Scale math ───────────────────────────────────────────────────────────────

static int FindScaleStepIndex(int percent) {
  for (int i = 0; i < DPI_SCALE_STEP_COUNT; i++)
    if (DPI_SCALE_STEPS[i] == percent)
      return i;
  return -1;
}

static bool SetMonitorScalePercent(HMONITOR hMonitor,
                                   const MonitorPathInfo &info,
                                   int targetPercent) {
  Sleep(500);

  DISPLAYCONFIG_SOURCE_DPI_SCALE_GET getInfo = {};
  getInfo.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_DPI_SCALE;
  getInfo.header.size = sizeof(getInfo);
  getInfo.header.adapterId = info.adapterId;
  getInfo.header.id = info.sourceId;
  if (DisplayConfigGetDeviceInfo(&getInfo.header) != ERROR_SUCCESS)
    return false;

  UINT dpiX = 96, dpiY = 96;
  GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
  int currentPercent = static_cast<int>(dpiX * 100u / 96u);

  int currentAbsIdx = FindScaleStepIndex(currentPercent);
  int targetAbsIdx = FindScaleStepIndex(targetPercent);
  if (currentAbsIdx < 0 || targetAbsIdx < 0) {
    // FIX 3: was passing currentPercent as an extra (stray) argument
    Wh_Log(
        L"[PerMonitorScaleSwitcher] targetPercent=%d is not a valid DPI step",
        targetPercent);
    return false;
  }

  if (currentPercent == targetPercent) {
    Wh_Log(L"[PerMonitorScaleSwitcher] Already at %d%%, skipping",
           targetPercent);
    return true;
  }

  // Special case: Windows sometimes reports a collapsed range (min==max)
  // when the scale state is stale after a display change. In that case,
  // setting scaleRel=0 resets to "recommended" and usually unsticks things.
  if (getInfo.minScaleRel == getInfo.maxScaleRel && currentAbsIdx >= 0) {
    Wh_Log(L"[PerMonitorScaleSwitcher] Collapsed range (min==max==%d); "
           L"attempting scaleRel=0 to unstick",
           getInfo.minScaleRel);

    DISPLAYCONFIG_SOURCE_DPI_SCALE_SET setInfo = {};
    setInfo.header.type = DISPLAYCONFIG_DEVICE_INFO_SET_DPI_SCALE;
    setInfo.header.size = sizeof(setInfo);
    setInfo.header.adapterId = info.adapterId;
    setInfo.header.id = info.sourceId;
    setInfo.scaleRel = 0;
    DisplayConfigSetDeviceInfo(&setInfo.header);

    Sleep(500);
    GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    int verifyPercent = static_cast<int>(dpiX * 100u / 96u);
    Wh_Log(L"[PerMonitorScaleSwitcher] Post-unstick DPI: %d%%", verifyPercent);
    if (verifyPercent == targetPercent)
      return true;

    // Re-query — range may have expanded after the reset.
    getInfo = {};
    getInfo.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_DPI_SCALE;
    getInfo.header.size = sizeof(getInfo);
    getInfo.header.adapterId = info.adapterId;
    getInfo.header.id = info.sourceId;
    if (DisplayConfigGetDeviceInfo(&getInfo.header) != ERROR_SUCCESS) {
      Wh_Log(L"[PerMonitorScaleSwitcher] Re-query after unstick failed");
      return false;
    }
  }

  int targetRel = INT_MAX;
  int usedAnchor = -1;

  auto tryAnchor = [&](int anchor, const wchar_t *strategy) -> bool {
    if (anchor < 0 || anchor >= DPI_SCALE_STEP_COUNT)
      return false;
    int rel = targetAbsIdx - anchor;
    if (rel < getInfo.minScaleRel || rel > getInfo.maxScaleRel)
      return false;
    targetRel = rel;
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
    for (int anchor = 0; anchor < DPI_SCALE_STEP_COUNT && targetRel == INT_MAX;
         anchor++) {
      if (currentAbsIdx >= 0) {
        int curRel = currentAbsIdx - anchor;
        if (curRel < getInfo.minScaleRel || curRel > getInfo.maxScaleRel)
          continue;
      }
      tryAnchor(anchor, L"brute-force");
    }
  }

  // Strategy 3: relaxed — find any anchor where the target fits.
  if (targetRel == INT_MAX) {
    for (int anchor = 0; anchor < DPI_SCALE_STEP_COUNT && targetRel == INT_MAX;
         anchor++)
      tryAnchor(anchor, L"relaxed");
  }

  if (targetRel == INT_MAX) {
    Wh_Log(L"[PerMonitorScaleSwitcher] No valid anchor found for currentPct=%d "
           L"targetPct=%d "
           L"minRel=%d maxRel=%d",
           currentPercent, targetPercent, getInfo.minScaleRel,
           getInfo.maxScaleRel);
    return false;
  }

  Wh_Log(L"[PerMonitorScaleSwitcher] anchor=%d curRel=%d minRel=%d maxRel=%d "
         L"currentPct=%d targetPct=%d targetRel=%d",
         usedAnchor, getInfo.curScaleRel, getInfo.minScaleRel,
         getInfo.maxScaleRel, currentPercent, targetPercent, targetRel);

  DISPLAYCONFIG_SOURCE_DPI_SCALE_SET setInfo = {};
  setInfo.header.type = DISPLAYCONFIG_DEVICE_INFO_SET_DPI_SCALE;
  setInfo.header.size = sizeof(setInfo);
  setInfo.header.adapterId = info.adapterId;
  setInfo.header.id = info.sourceId;
  setInfo.scaleRel = targetRel;

  LONG result = DisplayConfigSetDeviceInfo(&setInfo.header);
  Wh_Log(L"[PerMonitorScaleSwitcher] SET_DPI_SCALE returned %ld", result);

  Sleep(500);
  GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
  int verifyPercent = static_cast<int>(dpiX * 100u / 96u);
  Wh_Log(L"[PerMonitorScaleSwitcher] Verified post-set DPI: %d%%",
         verifyPercent);

  if (verifyPercent != targetPercent) {
    // FIX 4: was passing verifyPercent and targetPercent with no format
    // specifiers
    Wh_Log(L"[PerMonitorScaleSwitcher] Verification failed");
    return false;
  }

  return true;
}

// ── Core logic
// ────────────────────────────────────────────────────────────────

static std::vector<HMONITOR> GetAllMonitors() {
  std::vector<HMONITOR> monitors;
  EnumDisplayMonitors(
      nullptr, nullptr,
      [](HMONITOR hMon, HDC, LPRECT, LPARAM lp) CALLBACK -> BOOL {
        reinterpret_cast<std::vector<HMONITOR> *>(lp)->push_back(hMon);
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&monitors));
  return monitors;
}

// Returns true if all configured monitors are at their target scale
bool HandleDisplayChange(int monitorCount) {
  Wh_Log(L"[PerMonitorScaleSwitcher] HandleDisplayChange: monitorCount=%d",
         monitorCount);

  EnterCriticalSection(&g_settingsLock);
  std::vector<MonitorConfig> configs = g_monitorConfigs;
  LeaveCriticalSection(&g_settingsLock);

  std::unordered_map<std::wstring, const MonitorConfig *> configMap;
  for (const auto &c : configs)
    configMap[c.id] = &c;

  std::unordered_map<std::wstring, MonitorPathInfo> displayPaths;
  if (!QueryAllDisplayPaths(displayPaths)) {
    Wh_Log(
        L"[PerMonitorScaleSwitcher] Could not query display paths — aborting");
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
      Wh_Log(L"[PerMonitorScaleSwitcher] Monitor friendly name = '%s' — not in "
             L"config, skipping",
             friendlyName.c_str());
      continue;
    }

    int targetScale =
        (monitorCount > 1) ? it->second->scaleMulti : it->second->scaleSingle;

    Wh_Log(L"[PerMonitorScaleSwitcher] Applying %d%% to '%s'", targetScale,
           friendlyName.c_str());
    if (!SetMonitorScalePercent(hMon, pathInfo, targetScale))
      allOk = false;
  }

  return allOk;
}

// Helper — call after any config check point
static bool HasConfig() {
  EnterCriticalSection(&g_settingsLock);
  bool empty = g_monitorConfigs.empty();
  LeaveCriticalSection(&g_settingsLock);
  return !empty;
}

// ── Window thread (listens for WM_DISPLAYCHANGE)
// ──────────────────────────────

int g_pendingMonitorCount = -1; // monitor count to retry with

LRESULT CALLBACK MessageWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
  if (msg == WM_DISPLAYCHANGE) {
    int count = GetSystemMetrics(SM_CMONITORS);
    Wh_Log(L"[PerMonitorScaleSwitcher] WM_DISPLAYCHANGE received, monitor "
           L"count: %d",
           count);
    KillTimer(hwnd, RETRY_TIMER_ID);
    if (!HasConfig()) {
      Wh_Log(L"[PerMonitorScaleSwitcher] No config, ignoring display change");
      return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    g_pendingMonitorCount = count;
    SetTimer(hwnd, RETRY_TIMER_ID, RETRY_DELAY_MS, nullptr);
  } else if (msg == WM_TIMER && wParam == RETRY_TIMER_ID) {
    KillTimer(hwnd, RETRY_TIMER_ID);
    bool ok = HandleDisplayChange(g_pendingMonitorCount);
    if (!ok)
      Wh_Log(L"[PerMonitorScaleSwitcher] Apply incomplete — not retrying to "
             L"avoid loop");
    // No re-arm — done either way
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD g_dwWindowThreadId = 0; // store thread ID for reliable PostThreadMessage

DWORD WINAPI WindowThread(LPVOID) {
  HINSTANCE hInst = GetModuleHandle(NULL);

  WNDCLASS wc = {};
  wc.lpfnWndProc = MessageWndProc;
  wc.hInstance = hInst;
  wc.lpszClassName = L"WindhawkAutoDisplayScalerWnd";
  RegisterClass(&wc);

  // Must be a visible top-level window (not HWND_MESSAGE) to receive
  // WM_DISPLAYCHANGE broadcasts. We keep it hidden via SW_HIDE.
  g_hMessageWindow = CreateWindowEx(0, wc.lpszClassName, nullptr, WS_POPUP, 0,
                                    0, 0, 0, nullptr, nullptr, hInst, nullptr);

  if (!g_hMessageWindow) {
    Wh_Log(L"[PerMonitorScaleSwitcher] Failed to create message window");
    UnregisterClass(wc.lpszClassName, hInst);
    return 1;
  }

  ShowWindow(g_hMessageWindow, SW_HIDE);

  // Apply scale once at startup after the settle delay.
  Wh_Log(L"[PerMonitorScaleSwitcher] Window thread started — enumerating "
         L"monitors");
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

// ── WhTool entry points (renamed from Wh_Mod* for tool-mod pattern) ──────────

// FIX 1: Renamed to WhTool_* so the tool-mod boilerplate below can wrap them.

BOOL WhTool_ModInit() {
  Wh_Log(L"[PerMonitorScaleSwitcher] Initializing");
  InitializeCriticalSection(&g_settingsLock);
  LoadSettings();

  g_hWindowThread =
      CreateThread(NULL, 0, WindowThread, NULL, 0, &g_dwWindowThreadId);
  if (!g_hWindowThread) {
    Wh_Log(L"[PerMonitorScaleSwitcher] Failed to create window thread");
    DeleteCriticalSection(&g_settingsLock);
    return FALSE;
  }

  Wh_Log(L"[PerMonitorScaleSwitcher] Initialized (thread ID: %lu)",
         g_dwWindowThreadId);
  return TRUE;
}

void WhTool_ModUninit() {
  Wh_Log(L"[PerMonitorScaleSwitcher] Uninitializing");

  if (g_dwWindowThreadId)
    PostThreadMessage(g_dwWindowThreadId, WM_QUIT, 0, 0);

  if (g_hWindowThread) {
    if (WaitForSingleObject(g_hWindowThread, 3000) == WAIT_TIMEOUT)
      Wh_Log(L"[PerMonitorScaleSwitcher] Thread exit timed out");
    else
      Wh_Log(L"[PerMonitorScaleSwitcher] Thread exited cleanly");

    CloseHandle(g_hWindowThread);
    g_hWindowThread = NULL;
    g_dwWindowThreadId = 0;
  }

  DeleteCriticalSection(&g_settingsLock);
  Wh_Log(L"[PerMonitorScaleSwitcher] Done");
}

void WhTool_ModSettingsChanged() {
  Wh_Log(L"[PerMonitorScaleSwitcher] Settings changed, reloading...");
  LoadSettings();
  if (!HasConfig()) {
    Wh_Log(L"[PerMonitorScaleSwitcher] No config, skipping apply");
    return;
  }
  HandleDisplayChange(GetSystemMetrics(SM_CMONITORS));
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
  DWORD sessionId;
  if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
      sessionId == 0) {
    return FALSE;
  }

  bool isExcluded = false;
  bool isToolModProcess = false;
  bool isCurrentToolModProcess = false;
  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
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

    IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)GetModuleHandle(nullptr);
    IMAGE_NT_HEADERS *ntHeaders =
        (IMAGE_NT_HEADERS *)((BYTE *)dosHeader + dosHeader->e_lfanew);

    DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
    void *entryPoint = (BYTE *)dosHeader + entryPointRVA;

    Wh_SetFunctionHook(entryPoint, (void *)EntryPoint_Hook, nullptr);
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

  using CreateProcessInternalW_t = BOOL(WINAPI *)(
      HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
      LPSECURITY_ATTRIBUTES lpProcessAttributes,
      LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
      DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
      LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
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
