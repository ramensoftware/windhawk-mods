// ==WindhawkMod==
// @id              taskbar-thumbnail-stats
// @name            Taskbar Thumbnail Stats
// @description     Shows RAM, CPU and version of the program on the Windows 11 taskbar thumbnails
// @version         1.2
// @author          HaVeN80
// @github          https://github.com/haven80
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -lshlwapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Parts of this mod (the taskbar thumbnail hooks) are based on the following
// mods by m417z, published under the GNU General Public License v3.0:
// - Taskbar Thumbnail Reorder
// - Taskbar Thumbnail Size
// - Taskbar Button Scroll
// https://github.com/ramensoftware/windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar Thumbnail Stats

Adds a line with live process information to the Windows 11 taskbar
thumbnails (the previews shown when hovering an open app), inside the
thumbnail, over the bottom of the preview image or below it:

- **RAM** in use (private working set, like Task Manager)
- **CPU** usage in percent, updated every second
- **version** of the executable

Example: `RAM 412 MB · CPU 3.4% · v131.0.2`

![Screenshot](https://i.imgur.com/qxWa9bC.png)

For multi-process programs (Firefox, Chrome, Edge, Teams…) the values can
include the child processes too, to get closer to what Task Manager shows.

The line can use the Windows accent color, a custom color or no background
at all, and the font size and text color can be customized.

Works with the new XAML thumbnails of Windows 11 (tested on 25H2). It does
not work with taskbar replacements such as StartAllBack or ExplorerPatcher.

The statistics are collected on a background thread, so the taskbar is never
slowed down, and only while a thumbnail is actually visible.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showRam: true
  $name: Show RAM
- showCpu: true
  $name: Show CPU
- showVersion: true
  $name: Show version
- includeChildren: true
  $name: Include child processes
  $description: >-
    Sum RAM and CPU of all the processes started by the program (useful for
    browsers, which use one process per tab)
- position: overlay
  $name: Position
  $options:
  - overlay: Over the preview image, at its bottom
  - below: Below the preview image
- fontSize: 10
  $name: Font size
  $description: In pixels (the thumbnail title is about 12)
- background: accent
  $name: Line background
  $options:
  - none: None (thumbnail background)
  - accent: Windows accent color
  - custom: Custom color
- backgroundColor: "#3A6EA5"
  $name: Custom background color
  $description: Format #RRGGBB, used with "Custom color"
- backgroundOpacity: 100
  $name: Background opacity (%)
- textColor: auto
  $name: Text color
  $options:
  - auto: Automatic
  - white: White
  - black: Black
  - custom: Custom
- textCustomColor: "#FFFFFF"
  $name: Custom text color
  $description: Format #RRGGBB, used with "Custom"
- updateInterval: 1000
  $name: Update interval (ms)
  $description: Minimum 250
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>
#include <shlwapi.h>
#include <tlhelp32.h>
#include <winver.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

using namespace winrt::Windows::UI::Xaml;
using winrt::Windows::UI::Color;
using winrt::Windows::UI::ColorHelper;

// ===========================================================================
// Settings
// ===========================================================================

enum class Position { overlay, below };
enum class BackgroundMode { none, accent, custom };
enum class TextColorMode { automatic, white, black, custom };

struct Settings {
    bool showRam = true;
    bool showCpu = true;
    bool showVersion = true;
    bool includeChildren = true;
    Position position = Position::overlay;
    double fontSize = 10;
    BackgroundMode background = BackgroundMode::accent;
    Color backgroundColor{255, 58, 110, 165};
    int backgroundOpacity = 100;
    TextColorMode textColor = TextColorMode::automatic;
    Color textCustomColor{255, 255, 255, 255};
    int updateInterval = 1000;
};

// Settings are replaced as a whole (never modified in place), so readers on
// other threads always see a consistent snapshot.
std::mutex g_settingsMutex;
std::shared_ptr<const Settings> g_settings = std::make_shared<Settings>();

std::shared_ptr<const Settings> GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// Incremented when the style of the injected line must be applied again
// (settings or Windows accent color changed).
std::atomic<int> g_styleGeneration{1};

std::atomic<bool> g_unloading;
std::atomic<bool> g_taskbarViewDllLoaded;

constexpr WCHAR kStatsBorderName[] = L"WhThumbnailStatsBorder";

// ===========================================================================
// taskbar.dll: map WindowsUdk TaskItemThumbnail -> ITaskItem -> HWND
// ===========================================================================

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow_Original;

void* CImmersiveTaskItem_vftable;

struct ThumbnailMapping {
    winrt::weak_ref<winrt::Windows::Foundation::IInspectable> thumbnail;
    void* identity;  // IUnknown* of the thumbnail, for comparisons only.
    void* taskItem;
};

std::mutex g_mappingMutex;
// Holds WinRT weak references: not destroyed at process exit (see
// https://github.com/ramensoftware/windhawk/wiki/Global-objects-and-process-shutdown),
// cleared in Wh_ModUninit instead.
[[clang::no_destroy]] std::vector<ThumbnailMapping> g_thumbnailMapping;

void* GetIdentity(IUnknown* object) {
    if (!object) {
        return nullptr;
    }

    IUnknown* identity = nullptr;
    if (FAILED(object->QueryInterface(__uuidof(IUnknown),
                                      (void**)&identity)) ||
        !identity) {
        return nullptr;
    }

    identity->Release();  // Only the address is used.
    return identity;
}

void AddThumbnailMapping(void* ctorResult, void* taskItem) {
    if (!ctorResult || !taskItem) {
        return;
    }

    // Same technique used by the "Taskbar Thumbnail Reorder" mod.
    winrt::Windows::Foundation::IInspectable obj = nullptr;
    ((IUnknown*)ctorResult + 2)
        ->QueryInterface(
            winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
            winrt::put_abi(obj));
    if (!obj) {
        return;
    }

    void* identity = GetIdentity((IUnknown*)winrt::get_abi(obj));

    std::lock_guard<std::mutex> lock(g_mappingMutex);

    std::erase_if(g_thumbnailMapping, [&](const ThumbnailMapping& item) {
        return !item.thumbnail.get() || item.identity == identity;
    });

    g_thumbnailMapping.push_back({winrt::make_weak(obj), identity, taskItem});
}

using TaskItemThumbnail_ctor_t = void*(WINAPI*)(void* param1,
                                                void* param2,
                                                void* taskGroup,
                                                void* taskItem,
                                                void* taskListUi,
                                                void* param6,
                                                void* param7,
                                                bool param8);
TaskItemThumbnail_ctor_t TaskItemThumbnail_ctor_Original;
void* WINAPI TaskItemThumbnail_ctor_Hook(void* param1,
                                         void* param2,
                                         void* taskGroup,
                                         void* taskItem,
                                         void* taskListUi,
                                         void* param6,
                                         void* param7,
                                         bool param8) {
    void* result = TaskItemThumbnail_ctor_Original(
        param1, param2, taskGroup, taskItem, taskListUi, param6, param7,
        param8);

    try {
        AddThumbnailMapping(result, taskItem);
    } catch (...) {
    }

    return result;
}

using TaskItemThumbnail_ctor2_t = void*(WINAPI*)(void* param1,
                                                 void* param2,
                                                 void* taskGroup,
                                                 void* taskItem,
                                                 void* taskListUi,
                                                 void* param6,
                                                 bool param7);
TaskItemThumbnail_ctor2_t TaskItemThumbnail_ctor2_Original;
void* WINAPI TaskItemThumbnail_ctor2_Hook(void* param1,
                                          void* param2,
                                          void* taskGroup,
                                          void* taskItem,
                                          void* taskListUi,
                                          void* param6,
                                          bool param7) {
    void* result = TaskItemThumbnail_ctor2_Original(
        param1, param2, taskGroup, taskItem, taskListUi, param6, param7);

    try {
        AddThumbnailMapping(result, taskItem);
    } catch (...) {
    }

    return result;
}

HWND GetWindowFromTaskItem(void* taskItem) {
    if (!taskItem) {
        return nullptr;
    }

    if (CImmersiveTaskItem_vftable &&
        *(void**)taskItem == CImmersiveTaskItem_vftable) {
        return CImmersiveTaskItem_GetWindow_Original
                   ? CImmersiveTaskItem_GetWindow_Original(taskItem)
                   : nullptr;
    }

    return CWindowTaskItem_GetWindow_Original
               ? CWindowTaskItem_GetWindow_Original(taskItem)
               : nullptr;
}

HWND GetWindowFromThumbnail(IUnknown* thumbnail) {
    void* identity = GetIdentity(thumbnail);
    if (!identity) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(g_mappingMutex);
    for (const auto& item : g_thumbnailMapping) {
        if (item.identity != identity) {
            continue;
        }

        // The task item is only used while the thumbnail is kept alive.
        auto alive = item.thumbnail.get();
        if (!alive) {
            return nullptr;
        }

        return GetWindowFromTaskItem(item.taskItem);
    }

    return nullptr;
}

// ===========================================================================
// Taskbar.View.dll: thumbnail view -> view model -> WindowsUdk thumbnail
// ===========================================================================

using TryGetItemFromContainer_TaskItemThumbnailViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskItemThumbnailViewModel_t
    TryGetItemFromContainer_TaskItemThumbnailViewModel_Original;

using TaskItemThumbnailViewModel_get_TaskItemThumbnail_t =
    int(WINAPI*)(void* pThis, void** taskItemThumbnail);
TaskItemThumbnailViewModel_get_TaskItemThumbnail_t
    TaskItemThumbnailViewModel_get_TaskItemThumbnail_Original;

HWND GetWindowFromThumbnailView(UIElement element) {
    if (!TryGetItemFromContainer_TaskItemThumbnailViewModel_Original ||
        !TaskItemThumbnailViewModel_get_TaskItemThumbnail_Original) {
        return nullptr;
    }

    winrt::com_ptr<IUnknown> viewModel;
    TryGetItemFromContainer_TaskItemThumbnailViewModel_Original(
        viewModel.put_void(), &element);
    if (!viewModel) {
        return nullptr;
    }

    winrt::com_ptr<IUnknown> thumbnail;
    TaskItemThumbnailViewModel_get_TaskItemThumbnail_Original(
        viewModel.get(), thumbnail.put_void());
    if (!thumbnail) {
        return nullptr;
    }

    return GetWindowFromThumbnail(thumbnail.get());
}

// ===========================================================================
// Process statistics (background thread only)
// ===========================================================================

// PROCESS_MEMORY_COUNTERS_EX2 (Windows 11 SDK), defined here to not depend on
// the SDK version.
struct ProcessMemoryCountersEx2 {
    DWORD cb;
    DWORD PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivateUsage;
    SIZE_T PrivateWorkingSetSize;
    ULONG64 SharedCommitUsage;
};

struct CpuSample {
    ULONGLONG creationTime;  // Tells apart processes with a reused PID.
    ULONGLONG cpuTime;       // 100ns units (kernel + user).
    ULONGLONG wallTime;      // 100ns units.
};

// Only accessed by the worker thread.
std::unordered_map<DWORD, CpuSample> g_cpuSamples;
std::unordered_map<std::wstring, std::wstring> g_versionCache;

ULONGLONG FileTimeToULL(const FILETIME& ft) {
    return ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}

ULONGLONG GetWallTime() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return FileTimeToULL(ft);
}

DWORD GetProcessorCount() {
    static DWORD count = [] {
        DWORD n = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
        return n ? n : 1;
    }();
    return count;
}

std::wstring GetProcessImagePath(DWORD pid) {
    std::wstring result;
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return result;
    }

    WCHAR path[MAX_PATH * 2];
    DWORD size = ARRAYSIZE(path);
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        result = path;
    }

    CloseHandle(process);
    return result;
}

// UWP apps: the taskbar window belongs to ApplicationFrameHost.exe, the real
// app lives in a child CoreWindow owned by another process.
DWORD GetRealProcessId(HWND hWnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) {
        return 0;
    }

    std::wstring path = GetProcessImagePath(pid);
    if (_wcsicmp(PathFindFileNameW(path.c_str()),
                 L"ApplicationFrameHost.exe") != 0) {
        return pid;
    }

    struct Param {
        DWORD framePid;
        DWORD childPid;
    } param{pid, 0};

    EnumChildWindows(
        hWnd,
        [](HWND child, LPARAM lParam) -> BOOL {
            auto* p = (Param*)lParam;
            DWORD childPid = 0;
            GetWindowThreadProcessId(child, &childPid);
            if (childPid && childPid != p->framePid) {
                p->childPid = childPid;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&param);

    return param.childPid ? param.childPid : pid;
}

ULONGLONG GetProcessCreationTime(DWORD pid) {
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return 0;
    }

    FILETIME creation, exit, kernel, user;
    ULONGLONG result = 0;
    if (GetProcessTimes(process, &creation, &exit, &kernel, &user)) {
        result = FileTimeToULL(creation);
    }

    CloseHandle(process);
    return result;
}

std::vector<DWORD> GetProcessTree(DWORD rootPid, bool includeChildren) {
    std::vector<DWORD> result{rootPid};
    if (!includeChildren) {
        return result;
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return result;
    }

    std::unordered_multimap<DWORD, DWORD> children;
    PROCESSENTRY32W entry{sizeof(entry)};
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (entry.th32ProcessID != entry.th32ParentProcessID) {
                children.emplace(entry.th32ParentProcessID,
                                 entry.th32ProcessID);
            }
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);

    // Guard against PID reuse: a child must be newer than its parent.
    std::unordered_set<DWORD> seen{rootPid};
    for (size_t i = 0; i < result.size() && result.size() < 512; i++) {
        DWORD parent = result[i];
        ULONGLONG parentCreation = GetProcessCreationTime(parent);
        auto range = children.equal_range(parent);
        for (auto it = range.first; it != range.second; ++it) {
            DWORD child = it->second;
            if (seen.count(child)) {
                continue;
            }
            ULONGLONG childCreation = GetProcessCreationTime(child);
            if (parentCreation && childCreation &&
                childCreation < parentCreation) {
                continue;
            }
            seen.insert(child);
            result.push_back(child);
        }
    }

    return result;
}

std::wstring GetFileVersionString(const std::wstring& path) {
    auto it = g_versionCache.find(path);
    if (it != g_versionCache.end()) {
        return it->second;
    }

    std::wstring version;
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeW(path.c_str(), &handle);
    if (size) {
        std::string data(size, '\0');
        if (GetFileVersionInfoW(path.c_str(), 0, size, data.data())) {
            struct LANGANDCODEPAGE {
                WORD language;
                WORD codePage;
            }* translations = nullptr;
            UINT translationsSize = 0;
            if (VerQueryValueW(data.data(), L"\\VarFileInfo\\Translation",
                               (void**)&translations, &translationsSize) &&
                translationsSize >= sizeof(LANGANDCODEPAGE)) {
                WCHAR subBlock[128];
                swprintf_s(subBlock,
                           L"\\StringFileInfo\\%04x%04x\\ProductVersion",
                           translations[0].language, translations[0].codePage);
                PWSTR value = nullptr;
                UINT valueLen = 0;
                if (VerQueryValueW(data.data(), subBlock, (void**)&value,
                                   &valueLen) &&
                    value && valueLen > 1) {
                    version = value;
                }
            }

            if (version.empty()) {
                VS_FIXEDFILEINFO* fixed = nullptr;
                UINT fixedLen = 0;
                if (VerQueryValueW(data.data(), L"\\", (void**)&fixed,
                                   &fixedLen) &&
                    fixed && fixedLen) {
                    WCHAR buf[64];
                    swprintf_s(buf, L"%u.%u.%u.%u",
                               HIWORD(fixed->dwFileVersionMS),
                               LOWORD(fixed->dwFileVersionMS),
                               HIWORD(fixed->dwFileVersionLS),
                               LOWORD(fixed->dwFileVersionLS));
                    version = buf;
                }
            }
        }
    }

    while (!version.empty() && iswspace(version.back())) {
        version.pop_back();
    }

    if (g_versionCache.size() > 256) {
        g_versionCache.clear();
    }
    g_versionCache[path] = version;
    return version;
}

std::wstring FormatPercent(double value) {
    WCHAR decimal[8] = L".";
    GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, decimal,
                    ARRAYSIZE(decimal));

    WCHAR buf[32];
    if (value >= 10) {
        swprintf_s(buf, L"%.0f", value);
    } else {
        swprintf_s(buf, L"%.1f", value);
    }

    std::wstring result = buf;
    size_t dot = result.find(L'.');
    if (dot != std::wstring::npos) {
        result.replace(dot, 1, decimal);
    }

    return result + L"%";
}

std::wstring BuildStatsText(HWND hWnd, const Settings& settings) {
    if (!hWnd || !IsWindow(hWnd)) {
        return {};
    }

    DWORD rootPid = GetRealProcessId(hWnd);
    if (!rootPid) {
        return {};
    }

    std::wstring rootPath = GetProcessImagePath(rootPid);

    // Explorer is the parent of almost everything started from the desktop:
    // never add its children.
    bool isExplorer =
        _wcsicmp(PathFindFileNameW(rootPath.c_str()), L"explorer.exe") == 0;
    std::vector<DWORD> pids =
        GetProcessTree(rootPid, settings.includeChildren && !isExplorer);

    ULONGLONG memory = 0;
    double cpuPercent = 0;
    bool cpuReady = false;
    int openedCount = 0;
    ULONGLONG now = GetWallTime();

    for (DWORD pid : pids) {
        HANDLE process =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!process) {
            continue;
        }
        openedCount++;

        if (settings.showRam) {
            ProcessMemoryCountersEx2 counters{};
            counters.cb = sizeof(counters);
            if (GetProcessMemoryInfo(process,
                                     (PROCESS_MEMORY_COUNTERS*)&counters,
                                     sizeof(counters)) &&
                counters.PrivateWorkingSetSize) {
                memory += counters.PrivateWorkingSetSize;
            } else {
                PROCESS_MEMORY_COUNTERS basic{};
                basic.cb = sizeof(basic);
                if (GetProcessMemoryInfo(process, &basic, sizeof(basic))) {
                    memory += basic.WorkingSetSize;
                }
            }
        }

        if (settings.showCpu) {
            FILETIME creation, exit, kernel, user;
            if (GetProcessTimes(process, &creation, &exit, &kernel, &user)) {
                ULONGLONG creationTime = FileTimeToULL(creation);
                ULONGLONG cpuTime =
                    FileTimeToULL(kernel) + FileTimeToULL(user);

                auto it = g_cpuSamples.find(pid);
                if (it != g_cpuSamples.end() &&
                    it->second.creationTime == creationTime &&
                    now > it->second.wallTime &&
                    cpuTime >= it->second.cpuTime) {
                    double wallDelta = (double)(now - it->second.wallTime);
                    double cpuDelta = (double)(cpuTime - it->second.cpuTime);
                    cpuPercent +=
                        cpuDelta * 100.0 / (wallDelta * GetProcessorCount());
                    cpuReady = true;
                }
                g_cpuSamples[pid] = {creationTime, cpuTime, now};
            }
        }

        CloseHandle(process);
    }

    // Drop stale samples (closed processes).
    std::erase_if(g_cpuSamples, [now](const auto& item) {
        return now - item.second.wallTime > 60ULL * 10000000ULL;
    });

    std::wstring result;
    auto append = [&](const std::wstring& part) {
        if (part.empty()) {
            return;
        }
        if (!result.empty()) {
            result += L" · ";
        }
        result += part;
    };

    // Processes that can't be opened (e.g. protected ones): show nothing
    // rather than misleading values.
    if (openedCount > 0) {
        if (settings.showRam && memory) {
            WCHAR sizeText[64];
            if (StrFormatByteSizeW((LONGLONG)memory, sizeText,
                                   ARRAYSIZE(sizeText))) {
                append(std::wstring(L"RAM ") + sizeText);
            }
        }

        if (settings.showCpu) {
            append(L"CPU " + (cpuReady ? FormatPercent(cpuPercent)
                                       : std::wstring(L"…")));
        }
    }

    if (settings.showVersion && !rootPath.empty()) {
        std::wstring version = GetFileVersionString(rootPath);
        if (!version.empty()) {
            append(L"v" + version);
        }
    }

    if (openedCount > 1 && (settings.showRam || settings.showCpu)) {
        append(std::to_wstring(openedCount) + L" processes");
    }

    return result;
}

// ===========================================================================
// Worker thread: computes the statistics for the windows whose thumbnails are
// visible. The UI thread only posts requests and reads the results.
// ===========================================================================

struct StatsEntry {
    ULONGLONG lastRequestTick = 0;
    bool ready = false;
    int computeCount = 0;
    std::wstring text;
};

std::mutex g_statsMutex;
std::condition_variable g_statsCondition;
std::unordered_map<HWND, StatsEntry> g_statsEntries;
bool g_statsStop = false;
bool g_statsNewRequest = false;
[[clang::no_destroy]] std::optional<std::thread> g_statsThread;

// Number of thumbnail timers currently running (diagnostics).
std::atomic<int> g_activeTimers;

constexpr ULONGLONG kRequestTimeoutMs = 3000;
constexpr ULONGLONG kEntryLifetimeMs = 5 * 60 * 1000;

// Called from the UI thread. Returns the latest text, if any.
std::optional<std::wstring> RequestStats(HWND hWnd) {
    std::optional<std::wstring> result;
    bool notify = false;
    {
        std::lock_guard<std::mutex> lock(g_statsMutex);
        auto& entry = g_statsEntries[hWnd];
        ULONGLONG now = GetTickCount64();
        // New, or the thumbnail was closed for a while: compute at once. The
        // last known text is still returned meanwhile.
        notify = entry.lastRequestTick == 0 ||
                 now - entry.lastRequestTick > kRequestTimeoutMs;
        if (notify) {
            entry.computeCount = 0;
        }
        entry.lastRequestTick = now;
        if (entry.ready) {
            result = entry.text;
        }
        if (notify) {
            g_statsNewRequest = true;
        }
    }

    if (notify) {
        g_statsCondition.notify_one();
    }

    return result;
}

void StatsThreadProc() {
    ULONGLONG lastDiagnosticsTick = 0;

    std::unique_lock<std::mutex> lock(g_statsMutex);
    while (!g_statsStop) {
        ULONGLONG now = GetTickCount64();

        std::vector<HWND> windows;
        std::erase_if(g_statsEntries, [now](const auto& item) {
            // Kept for a while after the thumbnail closes, so that it shows
            // the last values at once when it opens again.
            return now - item.second.lastRequestTick > kEntryLifetimeMs;
        });
        for (const auto& [hWnd, entry] : g_statsEntries) {
            if (now - entry.lastRequestTick <= kRequestTimeoutMs) {
                windows.push_back(hWnd);
            }
        }
        g_statsNewRequest = false;

        auto settings = GetSettings();

        lock.unlock();

        std::vector<std::pair<HWND, std::wstring>> results;
        for (HWND hWnd : windows) {
            try {
                results.push_back({hWnd, BuildStatsText(hWnd, *settings)});
            } catch (...) {
                results.push_back({hWnd, std::wstring()});
            }
        }

        if (now - lastDiagnosticsTick >= 60000) {
            Wh_Log(L"Active thumbnail timers: %d, tracked windows: %d",
                   g_activeTimers.load(), (int)windows.size());
            lastDiagnosticsTick = now;
        }

        lock.lock();

        bool quickSecondPass = false;
        for (auto& [hWnd, text] : results) {
            auto it = g_statsEntries.find(hWnd);
            if (it != g_statsEntries.end()) {
                it->second.text = std::move(text);
                it->second.ready = true;
                // The first pass can't know the CPU usage yet (it needs two
                // samples): take the second one soon.
                if (++it->second.computeCount == 1) {
                    quickSecondPass = true;
                }
            }
        }

        int interval = quickSecondPass ? 250 : settings->updateInterval;
        g_statsCondition.wait_for(lock, std::chrono::milliseconds(interval),
                                  [] { return g_statsStop || g_statsNewRequest; });
    }
}

void StartStatsThread() {
    {
        std::lock_guard<std::mutex> lock(g_statsMutex);
        g_statsStop = false;
    }
    g_statsThread.emplace(StatsThreadProc);
}

void StopStatsThread() {
    {
        std::lock_guard<std::mutex> lock(g_statsMutex);
        g_statsStop = true;
    }
    g_statsCondition.notify_all();
    if (g_statsThread) {
        g_statsThread->join();
        g_statsThread.reset();
    }
}

// ===========================================================================
// Windows accent color (with change notifications)
// ===========================================================================

std::mutex g_uiSettingsMutex;
[[clang::no_destroy]] winrt::Windows::UI::ViewManagement::UISettings
    g_uiSettings{nullptr};
winrt::event_token g_colorValuesChangedToken{};

std::optional<Color> GetAccentColor() {
    try {
        std::lock_guard<std::mutex> lock(g_uiSettingsMutex);
        if (!g_uiSettings) {
            g_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
            g_colorValuesChangedToken =
                g_uiSettings.ColorValuesChanged([](auto&&, auto&&) {
                    // Any thread: the views re-apply their style on the next
                    // tick.
                    g_styleGeneration++;
                });
        }
        return g_uiSettings.GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Accent);
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Couldn't get the accent color: %08X", hr);
    }
    return std::nullopt;
}

void ReleaseAccentColorWatcher() {
    std::lock_guard<std::mutex> lock(g_uiSettingsMutex);
    if (g_uiSettings) {
        try {
            g_uiSettings.ColorValuesChanged(g_colorValuesChangedToken);
        } catch (...) {
        }
        g_uiSettings = nullptr;
    }
}

// ===========================================================================
// XAML injection (UI threads)
// ===========================================================================

struct ViewState {
    winrt::weak_ref<FrameworkElement> element;
    DWORD threadId = 0;  // The UI thread owning the view.

    // Injected elements.
    winrt::weak_ref<Controls::Grid> grid;
    Controls::Border border{nullptr};
    Controls::TextBlock textBlock{nullptr};
    std::vector<Controls::RowDefinition> addedRows;
    Position injectedPosition = Position::overlay;

    DispatcherTimer timer{nullptr};
    bool timerRunning = false;
    int timerInterval = 0;
    int styleGeneration = 0;
    bool fastPolling = false;  // Waiting for the first result.

    winrt::event_token loadedToken{};
    winrt::event_token unloadedToken{};
    winrt::event_token tickToken{};
};

std::mutex g_viewStatesMutex;
// Holds strong XAML references: not destroyed at process exit, released on
// the owning UI threads in Wh_ModUninit.
[[clang::no_destroy]] std::optional<std::vector<std::shared_ptr<ViewState>>>
    g_viewStates{std::in_place};

// Breadth-first search in the visual tree, limited depth.
DependencyObject FindDescendant(
    DependencyObject root,
    int maxDepth,
    const std::function<bool(DependencyObject const&)>& predicate) {
    std::vector<std::pair<DependencyObject, int>> queue{{root, 0}};
    for (size_t i = 0; i < queue.size(); i++) {
        auto [node, depth] = queue[i];
        if (i > 0 && predicate(node)) {
            return node;
        }
        if (depth >= maxDepth) {
            continue;
        }
        int count = Media::VisualTreeHelper::GetChildrenCount(node);
        for (int c = 0; c < count; c++) {
            queue.push_back(
                {Media::VisualTreeHelper::GetChild(node, c), depth + 1});
        }
    }
    return nullptr;
}

Controls::Grid FindRootGrid(FrameworkElement element) {
    auto found = FindDescendant(element, 3, [](DependencyObject const& o) {
        return (bool)o.try_as<Controls::Grid>();
    });
    return found ? found.as<Controls::Grid>() : nullptr;
}

// True if the element is actually on screen (not collapsed, not in a hidden
// flyout).
bool IsElementShown(FrameworkElement const& element) {
    if (!element.IsLoaded() || element.ActualWidth() <= 0 ||
        element.ActualHeight() <= 0) {
        return false;
    }

    DependencyObject current = element;
    for (int i = 0; i < 64 && current; i++) {
        if (auto uiElement = current.try_as<UIElement>()) {
            if (uiElement.Visibility() == Visibility::Collapsed ||
                uiElement.Opacity() <= 0) {
                return false;
            }
        }
        current = Media::VisualTreeHelper::GetParent(current);
    }

    return true;
}

void ApplyStyle(ViewState& state, const Settings& settings) {
    auto& border = state.border;
    auto& textBlock = state.textBlock;
    if (!border || !textBlock) {
        return;
    }

    textBlock.FontSize(settings.fontSize);

    // Reset what a previous style may have set.
    border.ClearValue(Controls::Border::BackgroundProperty());
    textBlock.ClearValue(Controls::TextBlock::ForegroundProperty());
    textBlock.Opacity(1.0);

    // Background.
    std::optional<Color> background;
    if (settings.background == BackgroundMode::accent) {
        background = GetAccentColor();
    } else if (settings.background == BackgroundMode::custom) {
        background = settings.backgroundColor;
    }
    if (background) {
        background->A = (uint8_t)(255 * settings.backgroundOpacity / 100);
        border.Background(Media::SolidColorBrush(*background));
    }

    // Text color.
    switch (settings.textColor) {
        case TextColorMode::white:
            textBlock.Foreground(
                Media::SolidColorBrush(ColorHelper::FromArgb(255, 255, 255, 255)));
            break;
        case TextColorMode::black:
            textBlock.Foreground(
                Media::SolidColorBrush(ColorHelper::FromArgb(255, 0, 0, 0)));
            break;
        case TextColorMode::custom:
            textBlock.Foreground(
                Media::SolidColorBrush(settings.textCustomColor));
            break;
        case TextColorMode::automatic:
            if (background && background->A >= 128) {
                // Readable on the chosen background.
                double luminance = 0.299 * background->R +
                                   0.587 * background->G +
                                   0.114 * background->B;
                uint8_t c = luminance > 150 ? 0 : 255;
                textBlock.Foreground(
                    Media::SolidColorBrush(ColorHelper::FromArgb(255, c, c, c)));
            } else {
                // Same color as the thumbnail title, which follows the theme.
                textBlock.Opacity(0.85);
            }
            break;
    }
}

// Removes exactly what was injected (the line and our own row definitions).
void RemoveInjected(ViewState& state) {
    if (auto grid = state.grid.get()) {
        try {
            if (state.border) {
                uint32_t index;
                if (grid.Children().IndexOf(state.border, index)) {
                    grid.Children().RemoveAt(index);
                }
            }

            auto rows = grid.RowDefinitions();
            for (auto it = state.addedRows.rbegin();
                 it != state.addedRows.rend(); ++it) {
                uint32_t index;
                if (rows.IndexOf(*it, index)) {
                    rows.RemoveAt(index);
                }
            }
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"RemoveInjected error %08X", hr);
        }
    }

    state.grid = nullptr;
    state.border = nullptr;
    state.textBlock = nullptr;
    state.addedRows.clear();
}

bool InjectStats(ViewState& state) {
    auto element = state.element.get();
    if (!element) {
        return false;
    }

    Controls::Grid grid = FindRootGrid(element);
    if (!grid) {
        return false;
    }

    Position position = GetSettings()->position;

    if (state.border) {
        // Still in place? The template may have been applied again, with a
        // new grid, or the position setting may have changed.
        uint32_t index;
        if (state.grid.get() == grid && state.injectedPosition == position &&
            grid.Children().IndexOf(state.border, index)) {
            return true;
        }
        RemoveInjected(state);
    }

    // Leftover from a previous instance of the mod.
    auto children = grid.Children();
    for (uint32_t i = 0; i < children.Size(); i++) {
        auto fe = children.GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == kStatsBorderName) {
            children.RemoveAt(i);
            break;
        }
    }

    Controls::TextBlock textBlock;
    textBlock.TextWrapping(TextWrapping::Wrap);
    textBlock.TextAlignment(TextAlignment::Center);

    Controls::Border border;
    border.Name(kStatsBorderName);
    border.Child(textBlock);
    border.IsHitTestVisible(false);
    border.Padding(ThicknessHelper::FromLengths(6, 2, 6, 3));
    border.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
    border.HorizontalAlignment(HorizontalAlignment::Stretch);
    border.Visibility(Visibility::Collapsed);

    std::vector<Controls::RowDefinition> addedRows;
    auto rows = grid.RowDefinitions();
    if (position == Position::below) {
        // A new row under the image, inside the thumbnail.
        if (rows.Size() == 0) {
            // Without row definitions all the content is in an implicit
            // single row: make it explicit so that ours goes below it.
            Controls::RowDefinition starRow;
            starRow.Height(
                GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
            rows.Append(starRow);
            addedRows.push_back(starRow);
        }
        Controls::RowDefinition autoRow;
        autoRow.Height(GridLengthHelper::Auto());
        rows.Append(autoRow);
        addedRows.push_back(autoRow);

        Controls::Grid::SetRow(border, (int32_t)rows.Size() - 1);
        border.Margin(ThicknessHelper::FromLengths(4, 2, 4, 4));
    } else {
        // Over the bottom of the image: span all the rows, aligned to the
        // bottom. The layout of the thumbnail is not changed.
        Controls::Grid::SetRow(border, 0);
        Controls::Grid::SetRowSpan(border,
                                   std::max<int32_t>(1, (int32_t)rows.Size()));
        border.VerticalAlignment(VerticalAlignment::Bottom);
        border.Margin(ThicknessHelper::FromLengths(6, 0, 6, 6));
    }
    Controls::Grid::SetColumnSpan(border, 100);

    grid.Children().Append(border);

    state.grid = winrt::make_weak(grid);
    state.border = border;
    state.textBlock = textBlock;
    state.addedRows = std::move(addedRows);
    state.injectedPosition = position;

    ApplyStyle(state, *GetSettings());
    state.styleGeneration = g_styleGeneration;

    return true;
}

void StopTimer(ViewState& state) {
    if (state.timer && state.timerRunning) {
        state.timer.Stop();
        state.timerRunning = false;
        g_activeTimers--;
    }
}

void ReleaseTimer(ViewState& state) {
    StopTimer(state);
    if (state.timer) {
        state.timer.Tick(state.tickToken);
        state.timer = nullptr;
    }
}

void UpdateView(const std::shared_ptr<ViewState>& state) {
    if (g_unloading) {
        return;
    }

    auto element = state->element.get();
    if (!element) {
        return;
    }

    auto settings = GetSettings();

    // The template may have been re-applied, or never injected yet.
    if (!InjectStats(*state)) {
        return;
    }

    if (state->styleGeneration != g_styleGeneration) {
        state->styleGeneration = g_styleGeneration;
        ApplyStyle(*state, *settings);
    }

    // Until the first result arrives, poll quickly so that the line shows up
    // together with the thumbnail.
    int interval = state->fastPolling ? 30
                                      : std::max(settings->updateInterval, 250);
    if (state->timer && state->timerInterval != interval) {
        state->timer.Interval(std::chrono::milliseconds(interval));
        state->timerInterval = interval;
    }

    // Hidden thumbnail: nothing to compute.
    if (!IsElementShown(element)) {
        return;
    }

    HWND hWnd = GetWindowFromThumbnailView(element);
    std::optional<std::wstring> text;
    if (hWnd) {
        text = RequestStats(hWnd);
    } else {
        text = std::wstring();
    }

    bool waiting = !text.has_value();
    if (waiting != state->fastPolling) {
        state->fastPolling = waiting;
        int newInterval =
            waiting ? 30 : std::max(settings->updateInterval, 250);
        if (state->timer && state->timerInterval != newInterval) {
            state->timer.Interval(std::chrono::milliseconds(newInterval));
            state->timerInterval = newInterval;
        }
    }

    if (!text) {
        return;  // Not computed yet: keep what is shown.
    }

    state->textBlock.Text(*text);
    state->border.Visibility(text->empty() ? Visibility::Collapsed
                                           : Visibility::Visible);
}

void StartTimer(const std::shared_ptr<ViewState>& state) {
    if (!state->timer) {
        DispatcherTimer timer;
        int interval = std::max(GetSettings()->updateInterval, 250);
        timer.Interval(std::chrono::milliseconds(interval));
        state->timerInterval = interval;

        std::weak_ptr<ViewState> weakState = state;
        state->tickToken =
            timer.Tick([weakState](auto const& sender, auto&&) {
                auto s = weakState.lock();
                if (!s || !s->element.get() || g_unloading) {
                    // The view is gone: a running timer would keep calling
                    // into this mod forever, stop it.
                    if (s) {
                        ReleaseTimer(*s);
                    } else if (auto timer =
                                   sender.template try_as<DispatcherTimer>()) {
                        timer.Stop();
                    }
                    return;
                }

                try {
                    UpdateView(s);
                } catch (...) {
                    HRESULT hr = winrt::to_hresult();
                    Wh_Log(L"UpdateView error %08X", hr);
                }
            });
        state->timer = timer;
    }

    UpdateView(state);

    if (!state->timerRunning) {
        state->timer.Start();
        state->timerRunning = true;
        g_activeTimers++;
    }
}

// ===========================================================================
// Running code on a given UI thread (synchronously)
// ===========================================================================

HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

HWND GetTaskbarUiWnd() {
    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd) {
        return nullptr;
    }

    return FindWindowExW(taskbarWnd, nullptr,
                         L"Windows.UI.Composition.DesktopWindowContentBridge",
                         nullptr);
}

// Any window of the given thread, to send it a message.
HWND FindThreadWindow(DWORD threadId) {
    if (GetWindowThreadProcessId(GetTaskbarUiWnd(), nullptr) == threadId) {
        return GetTaskbarUiWnd();
    }

    HWND result = nullptr;
    EnumThreadWindows(
        threadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            *reinterpret_cast<HWND*>(lParam) = hWnd;
            return FALSE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

using RunFromWindowThreadProc_t = void (*)(void* param);

// Runs proc on the thread of hWnd and returns only after it has finished.
bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessageW(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

void SetupThumbnailView(FrameworkElement element) {
    std::shared_ptr<ViewState> state;

    {
        std::lock_guard<std::mutex> lock(g_viewStatesMutex);

        if (!g_viewStates) {
            return;
        }

        // Drop the states of destroyed views. Only the ones of this thread:
        // their XAML references must be released on their own thread. States
        // with a timer are kept until the timer is released (see the Tick
        // and Unloaded handlers).
        DWORD currentThreadId = GetCurrentThreadId();
        std::erase_if(*g_viewStates, [currentThreadId](const auto& s) {
            return s->threadId == currentThreadId && !s->element.get() &&
                   !s->timer;
        });

        for (const auto& s : *g_viewStates) {
            if (s->element.get() == element) {
                state = s;
                break;
            }
        }

        if (!state) {
            state = std::make_shared<ViewState>();
            state->element = winrt::make_weak(element);
            state->threadId = currentThreadId;
            g_viewStates->push_back(state);
        }
    }

    static std::atomic<bool> loggedThread;
    if (!loggedThread.exchange(true)) {
        Wh_Log(L"Thumbnail view thread: %u, taskbar UI thread: %u",
               GetCurrentThreadId(),
               GetWindowThreadProcessId(GetTaskbarUiWnd(), nullptr));
    }

    InjectStats(*state);

    if (!state->loadedToken) {
        std::weak_ptr<ViewState> weakState = state;
        state->loadedToken = element.Loaded([weakState](auto&&, auto&&) {
            auto s = weakState.lock();
            if (!s || g_unloading) {
                return;
            }
            try {
                StartTimer(s);
            } catch (...) {
                HRESULT hr = winrt::to_hresult();
                Wh_Log(L"Loaded error %08X", hr);
            }
        });
        state->unloadedToken = element.Unloaded([weakState](auto&&, auto&&) {
            // Release (not just stop) the timer: a stopped timer never ticks,
            // so it could never notice that the view was destroyed. It's
            // created again on the next Loaded event.
            if (auto s = weakState.lock()) {
                ReleaseTimer(*s);
            }
        });
    }

    if (element.IsLoaded()) {
        StartTimer(state);
    }
}

void CleanupViewState(const std::shared_ptr<ViewState>& state) {
    ReleaseTimer(*state);

    if (auto element = state->element.get()) {
        if (state->loadedToken) {
            element.Loaded(state->loadedToken);
        }
        if (state->unloadedToken) {
            element.Unloaded(state->unloadedToken);
        }
    }
    state->loadedToken = {};
    state->unloadedToken = {};

    RemoveInjected(*state);
}

using TaskItemThumbnailView_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
TaskItemThumbnailView_OnApplyTemplate_t
    TaskItemThumbnailView_OnApplyTemplate_Original;
void WINAPI TaskItemThumbnailView_OnApplyTemplate_Hook(void* pThis) {
    TaskItemThumbnailView_OnApplyTemplate_Original(pThis);

    if (g_unloading) {
        return;
    }

    // Same technique used by the "Taskbar Thumbnail Size" mod.
    IUnknown* unknownPtr = *((IUnknown**)pThis + 1);
    if (!unknownPtr) {
        return;
    }

    FrameworkElement element = nullptr;
    unknownPtr->QueryInterface(winrt::guid_of<FrameworkElement>(),
                               winrt::put_abi(element));
    if (!element) {
        return;
    }

    try {
        SetupThumbnailView(element);
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"SetupThumbnailView error %08X", hr);
    }
}

// ===========================================================================
// Hooking
// ===========================================================================

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow_Original,
            nullptr,
            true,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable,
            nullptr,
            true,
        },
        {
            // Older variant.
            {LR"(public: __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskItemThumbnail::TaskItemThumbnail(struct winrt::WindowsUdk::UI::Shell::TaskItem const &,struct ITaskGroup *,struct ITaskItem *,struct ITaskListUI *,struct IWICImagingFactory *,struct ITaskListAcc *,bool))"},
            &TaskItemThumbnail_ctor_Original,
            TaskItemThumbnail_ctor_Hook,
            true,
        },
        {
            // Newer variant (10.0.26100.8328 and later).
            {LR"(public: __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskItemThumbnail::TaskItemThumbnail(struct winrt::WindowsUdk::UI::Shell::TaskItem const &,struct ITaskGroup *,struct ITaskItem *,struct ITaskListUI *,struct IWICImagingFactory *,bool))"},
            &TaskItemThumbnail_ctor2_Original,
            TaskItemThumbnail_ctor2_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols (taskbar.dll) failed");
        return false;
    }

    if (!TaskItemThumbnail_ctor_Original && !TaskItemThumbnail_ctor2_Original) {
        Wh_Log(L"No TaskItemThumbnail constructor found");
        return false;
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskItemThumbnailView::OnApplyTemplate(void))"},
            &TaskItemThumbnailView_OnApplyTemplate_Original,
            TaskItemThumbnailView_OnApplyTemplate_Hook,
        },
        {
            {LR"(struct winrt::Taskbar::TaskItemThumbnailViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskItemThumbnailViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskItemThumbnailViewModel_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskItemThumbnailViewModel,struct winrt::Taskbar::ITaskItemThumbnailViewModel>::get_TaskItemThumbnail(void * *))"},
            &TaskItemThumbnailViewModel_get_TaskItemThumbnail_Original,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols (Taskbar.View.dll) failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

// ===========================================================================
// Mod lifecycle
// ===========================================================================

// "#RRGGBB" or "RRGGBB", spaces allowed around.
std::optional<Color> ParseColor(PCWSTR text) {
    if (!text) {
        return std::nullopt;
    }

    std::wstring value = text;
    while (!value.empty() && iswspace(value.back())) {
        value.pop_back();
    }
    size_t start = 0;
    while (start < value.size() && iswspace(value[start])) {
        start++;
    }
    if (start < value.size() && value[start] == L'#') {
        start++;
    }
    value = value.substr(start);

    if (value.size() != 6 ||
        value.find_first_not_of(L"0123456789abcdefABCDEF") !=
            std::wstring::npos) {
        return std::nullopt;
    }

    unsigned long rgb = wcstoul(value.c_str(), nullptr, 16);
    return ColorHelper::FromArgb(255, (uint8_t)(rgb >> 16),
                                 (uint8_t)(rgb >> 8), (uint8_t)rgb);
}

void LoadSettings() {
    auto settings = std::make_shared<Settings>();

    settings->showRam = Wh_GetIntSetting(L"showRam");
    settings->showCpu = Wh_GetIntSetting(L"showCpu");
    settings->showVersion = Wh_GetIntSetting(L"showVersion");
    settings->includeChildren = Wh_GetIntSetting(L"includeChildren");

    PCWSTR position = Wh_GetStringSetting(L"position");
    settings->position = wcscmp(position, L"below") == 0 ? Position::below
                                                         : Position::overlay;
    Wh_FreeStringSetting(position);

    int fontSize = Wh_GetIntSetting(L"fontSize");
    settings->fontSize = (fontSize >= 6 && fontSize <= 40) ? fontSize : 10;

    PCWSTR background = Wh_GetStringSetting(L"background");
    settings->background = wcscmp(background, L"none") == 0
                               ? BackgroundMode::none
                           : wcscmp(background, L"custom") == 0
                               ? BackgroundMode::custom
                               : BackgroundMode::accent;
    Wh_FreeStringSetting(background);

    PCWSTR backgroundColor = Wh_GetStringSetting(L"backgroundColor");
    if (auto color = ParseColor(backgroundColor)) {
        settings->backgroundColor = *color;
    } else if (settings->background == BackgroundMode::custom) {
        Wh_Log(L"Invalid background color \"%s\", using the default",
               backgroundColor);
    }
    Wh_FreeStringSetting(backgroundColor);

    int opacity = Wh_GetIntSetting(L"backgroundOpacity");
    settings->backgroundOpacity = (opacity >= 0 && opacity <= 100) ? opacity
                                                                  : 100;

    PCWSTR textColor = Wh_GetStringSetting(L"textColor");
    settings->textColor = wcscmp(textColor, L"white") == 0
                              ? TextColorMode::white
                          : wcscmp(textColor, L"black") == 0
                              ? TextColorMode::black
                          : wcscmp(textColor, L"custom") == 0
                              ? TextColorMode::custom
                              : TextColorMode::automatic;
    Wh_FreeStringSetting(textColor);

    PCWSTR textCustomColor = Wh_GetStringSetting(L"textCustomColor");
    if (auto color = ParseColor(textCustomColor)) {
        settings->textCustomColor = *color;
    } else if (settings->textColor == TextColorMode::custom) {
        Wh_Log(L"Invalid text color \"%s\", using the default",
               textCustomColor);
    }
    Wh_FreeStringSetting(textCustomColor);

    int interval = Wh_GetIntSetting(L"updateInterval");
    settings->updateInterval = std::max(interval > 0 ? interval : 1000, 250);

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = std::move(settings);
    }
    g_styleGeneration++;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    StartStatsThread();

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_unloading = true;

    StopStatsThread();
    ReleaseAccentColorWatcher();

    {
        std::lock_guard<std::mutex> lock(g_mappingMutex);
        g_thumbnailMapping.clear();
    }

    // Group the views by their UI thread.
    std::unordered_map<DWORD, std::vector<std::shared_ptr<ViewState>>>
        statesByThread;
    {
        std::lock_guard<std::mutex> lock(g_viewStatesMutex);
        if (g_viewStates) {
            for (auto& state : *g_viewStates) {
                statesByThread[state->threadId].push_back(std::move(state));
            }
            g_viewStates.reset();
        }
    }

    // Remove the injected elements, release the timers and all the XAML
    // references on the owning thread, synchronously: when RunFromWindowThread
    // returns, no mod code runs on that thread anymore.
    int cleaned = 0;
    for (auto& [threadId, states] : statesByThread) {
        HWND hWnd = FindThreadWindow(threadId);
        if (!hWnd) {
            // The thread has no window (it's probably gone): retain the
            // states rather than releasing XAML references from this thread.
            Wh_Log(L"No window for thread %u, retaining %d views", threadId,
                   (int)states.size());
            new std::vector<std::shared_ptr<ViewState>>(std::move(states));
            continue;
        }

        bool ran = RunFromWindowThread(
            hWnd,
            [](void* param) {
                auto& states =
                    *(std::vector<std::shared_ptr<ViewState>>*)param;
                for (const auto& state : states) {
                    try {
                        CleanupViewState(state);
                    } catch (...) {
                        HRESULT hr = winrt::to_hresult();
                        Wh_Log(L"Cleanup error %08X", hr);
                    }
                }
                // Last references: the states are destroyed on this thread.
                states.clear();
            },
            &states);
        if (!ran) {
            Wh_Log(L"Couldn't run on thread %u, retaining %d views", threadId,
                   (int)states.size());
            new std::vector<std::shared_ptr<ViewState>>(std::move(states));
            continue;
        }
        cleaned++;
    }

    Wh_Log(L"Cleaned up the views of %d UI threads", cleaned);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    // Apply the new settings in place, without unloading the mod: the views
    // pick up the new style and interval on their next update.
    LoadSettings();
    *bReload = FALSE;

    return TRUE;
}
