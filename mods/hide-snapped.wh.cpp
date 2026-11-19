// ==WindhawkMod==
// @id              hide-snapped
// @name            Snap Groups: Alt+Tab and taskbar
// @description     Group-only Alt+Tab and separate taskbar buttons with tiled member icons.
// @version         0.4.26
// @author          coldfeudal
// @github          https://github.com/coldfeudal
// @include         explorer.exe
// @architecture    amd64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ladvapi32 -lshell32 -luuid -lgdi32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Snap Groups: Alt+Tab and taskbar

Keep native Snap Groups in Alt+Tab and give each group a separate taskbar button
with a grid of its application icons. Individual member buttons can be hidden;
windows outside groups remain accessible.

## Usage

Enable Snap windows and the option to show snapped groups in Alt+Tab under
Windows Settings > System > Multitasking. Arrange at least two windows into a
native Snap Group. The mod reads groups maintained by Windows.

A group button brings all members forward. With the default click setting,
clicking an active group minimizes it. If a member is already minimized, the
button restores the group. Closing a group button minimizes the group by default
and does not close the applications. Group buttons are temporary and cannot be
pinned; ordinary pinned application launchers remain.

The taskbar label uses the native localized group title. Hover previews use the
Windows group visual. Win+Tab is unchanged. Group commands use native Windows
activation/minimization; animation behavior follows those native commands.

## Settings

- Independently enable Alt+Tab filtering, group taskbar buttons and hiding of
  individual member buttons.
- Keep separate buttons for separate groups, including groups of the same apps.
- Display two to four equally sized icons in a two-column grid. Adjust icon size,
  spacing and incomplete-row alignment. The icon limit does not limit membership.
- Order icons by screen rows from top
  to bottom and centers from left to right. A tall window belongs to its first
  row (default), or use Windows' member order. Minimized windows use restored
  positions; only icon order changes.
- Choose restore-only or minimize-if-active behavior, and the close-button action.
- Adjust refresh interval and current-virtual-desktop filtering.
- Enable or disable native hover previews.
- Diagnostic mode keeps individual Alt+Tab cards and disables taskbar integration.

## Tested configuration and limitations

Tested interactively on Windows 11 25H2 (26200.8246, x64) with Windhawk 1.7.3:
group switching, independent taskbar buttons, native previews, icon layout,
Explorer restart, disable/enable and the support-trace toggle.

- **Independent taskbar per monitor 1.7.0:** group creation, moving groups
  between monitors and restoring minimized groups; no empty buttons or unload
  stalls in the tested disable/enable scenario.
- **Alt+Tab per monitor 1.2.1:** switching during a heavy game and after closing
  it, including disabling both mods without restarting Explorer.

Core taskbar integration has no Windows build-number or exact Taskbar.dll/
ShellCommon revision restriction. It requires internal shell symbols. Native
task-item enumeration additionally requires a recognized count-getter layout
and matching object type/count; otherwise that presentation step stays native.
Hidden members use their window icons without reading task-list object fields.
Native previews require an agile group and separately checked Taskbar.dll and
DesktopSwitcherDataModel.dll layouts. Unrecognized preview layouts disable only
previews. Detailed trace observers also retain separate DLL checks.
Shell interfaces are undocumented; other Windows versions and DLL revisions
remain unverified, and updates can require changes to the mod.
Longer sessions and other monitor/DPI/virtual desktop configurations have limited
coverage. The mod targets AMD64; native ARM64 Explorer is not targeted.

Report mod problems in https://github.com/ramensoftware/windhawk-mods/issues.
Include "hide-snapped" in the title and mention @coldfeudal in the report.

## Diagnostics and removal

Windhawk debug logging records phase codes and counts without window/document
titles. Disable this mod to restore the native taskbar and switcher behavior.
Enable Record support trace only when collecting diagnostics. Changing this
setting reloads the mod. Bounded journals are stored in Windhawk's per-mod
storage directory, in the traces subfolder. A background writer saves queued
records. Records may be dropped under load. Journals contain numeric identities,
counts and stack/module information, without window titles or AppID strings.

The Alt+Tab scoping approach was informed by
[Alt+Tab per monitor](https://windhawk.net/mods/alt-tab-per-monitor).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- altTabEnabled: true
  $name: Hide individual members in Alt+Tab
- taskbarEnabled: true
  $name: Show Snap Group buttons on the taskbar
- hideTaskbarMembers: true
  $name: Hide individual taskbar buttons for represented groups
- separateTaskbarGroups: true
  $name: Give each group an independent taskbar button
- maxIcons: 4
  $name: Maximum member icons (2 to 4)
  $description: Extra icons are hidden; all windows still belong to the group.
- iconOrder: screen
  $name: Member icon order
  $description: Detect rows top to bottom, then order centers left to right. A tall window belongs to its first row. Minimized windows use restored positions.
  $options:
  - screen: Screen position (rows top to bottom, left to right)
  - native: Windows Snap Group order
- iconSize: 14
  $name: Member icon size (4 to 15, in a 32 pixel icon)
- iconGap: 2
  $name: Gap between icons (0 to 8)
- centerLastRow: false
  $name: Center an incomplete last row
  $description: Off matches a two-column CSS grid. All icons have the same size.
- clickAction: toggle
  $name: Clicking a group
  $options:
  - restore: Bring all members to the front
  - toggle: Minimize if a member is active; otherwise bring the group forward
- closeAction: minimize
  $name: Closing a group button
  $description: Never closes application windows.
  $options:
  - minimize: Minimize group windows
  - ignore: Ignore
- refreshInterval: 500
  $name: Group refresh interval (100 to 5000 milliseconds)
- currentDesktopOnly: true
  $name: Show taskbar groups only on the current virtual desktop
- nativeGroupPreview: true
  $name: Native Snap Group preview on hover
  $description: Host the same system composition visual factory used by Alt+Tab; no screenshot copying.
- binaryTrace: false
  $name: Record support trace
  $description: Opt-in bounded journals in Windhawk mod storage. Changing this reloads the mod. Records may be dropped under load.
- diagnosticMode: false
  $name: Diagnostic mode (keep all windows, disable taskbar integration)
  $description: Count Alt+Tab candidates without hiding cards; restore individual taskbar buttons.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <inspectable.h>
#include <roapi.h>
#include <winstring.h>
#include <wrl/client.h>
#include <atomic>
#include <array>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <shobjidl.h>
#include <shlobj.h>
#include <propkey.h>
#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include <tlhelp32.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>

using Microsoft::WRL::ComPtr;

namespace {

// Opt-in support diagnostics. Fixed-size records contain only numeric object
// identities, counts and PCs; never titles, AppIDs or arbitrary process memory.
constexpr uint32_t kTraceCapacity = 4096;
struct TraceModule {
    uint64_t base;
    uint32_t size, timestamp;
    wchar_t name[96];
};
struct TraceHeader {
    char magic[8];
    uint32_t format, headerSize, recordSize, capacity, pid, moduleCount;
    char version[16];
    uint64_t startTime;
    TraceModule modules[256];
};
struct TraceRecord {
    uint64_t sequence, tick, span;
    uint32_t thread, event;
    uint64_t a, b, c, d;
    uint64_t stack[16];
    uint32_t frames, checksum;
};
static_assert(sizeof(TraceRecord) == 200 && sizeof(TraceModule) == 208);
enum TraceEvent : uint32_t {
    TraceStart = 1, TraceEnd, TraceFilter, TraceHide, TraceRestore, TraceSnapshot,
    TraceCheckBegin, TraceCheckEnd, TraceCounts, TraceViewReady, TraceNullAppId,
    TraceScan, TraceScanCounters, TraceProxyRetire, TraceWorker = 16,
    TraceInsert = 20, TraceRemove, TraceMove, TraceCreateButton, TraceNotify,
    TraceCreateTask, TraceInclusion, TraceMultiInclusion, TraceRefresh, TracePresentation,
};
std::mutex g_traceMutex;
bool g_binaryTraceEnabled = false; // changes request a mod reload
HANDLE g_traceStop = nullptr, g_traceThread = nullptr;
std::vector<TraceRecord> g_tracePending;
std::atomic<bool> g_traceFlush = false, g_traceModulesRequested = false;
HANDLE g_traceFile = INVALID_HANDLE_VALUE;
std::atomic<bool> g_traceActive = false;
std::atomic<bool> g_traceNativeReady = false;
uint64_t g_traceSequence = 0;
uint64_t g_traceStartTime = 0;
thread_local bool g_traceWriting = false;
thread_local uint64_t g_traceSpan = 0;
std::atomic<uint64_t> g_traceSpanSequence = 0;

uint32_t TraceChecksum(const TraceRecord& record) {
    const auto* bytes = reinterpret_cast<const unsigned char*>(&record);
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < offsetof(TraceRecord, checksum); ++i) hash = (hash ^ bytes[i]) * 16777619u;
    return hash;
}
uint64_t TracePointer(const void* value) { return reinterpret_cast<uintptr_t>(value); }
void TraceWrite(uint32_t event, uint64_t a = 0, uint64_t b = 0, uint64_t c = 0, uint64_t d = 0) {
    if (!g_traceActive.load(std::memory_order_relaxed) || g_traceWriting) return;
    DWORD error = GetLastError();
    g_traceWriting = true;
    TraceRecord record{};
    record.tick = GetTickCount64(); record.span = g_traceSpan;
    record.thread = GetCurrentThreadId(); record.event = event;
    record.a = a; record.b = b; record.c = c; record.d = d;
    record.frames = CaptureStackBackTrace(1, ARRAYSIZE(record.stack),
        reinterpret_cast<void**>(record.stack), nullptr);
    {
        // Hooks never wait for the writer or perform file I/O. A busy/full
        // queue drops the record; native behavior must continue immediately.
        std::unique_lock lock(g_traceMutex, std::try_to_lock);
        if (lock.owns_lock() && g_traceActive.load(std::memory_order_relaxed) &&
            g_tracePending.size() < kTraceCapacity) {
            record.sequence = ++g_traceSequence;
            record.checksum = TraceChecksum(record);
            g_tracePending.push_back(record); // capacity reserved before enabling
        }
    }
    g_traceWriting = false;
    SetLastError(error);
}
struct TraceScope {
    uint64_t previous = g_traceSpan;
    uint32_t event;
    uint64_t object;
    uint64_t result = UINT64_MAX; // unknown unless this wrapper recorded the native result
    TraceScope(uint32_t kind, const void* self, const void* item = nullptr, uint64_t value = 0)
        : event(kind), object(TracePointer(self)) {
        if (g_traceActive.load(std::memory_order_relaxed)) {
            g_traceSpan = ++g_traceSpanSequence;
            TraceWrite(event, object, TracePointer(item), value, previous);
        }
    }
    ~TraceScope() { TraceWrite(TraceEnd, event, object, result); g_traceSpan = previous; }
};
template<class T> bool TraceRead(const void* base, size_t offset, T& value) {
    DWORD error = GetLastError();
    SIZE_T read = 0;
    bool ok = base && ReadProcessMemory(GetCurrentProcess(), static_cast<const char*>(base) + offset,
                                    &value, sizeof(value), &read) && read == sizeof(value);
    SetLastError(error);
    return ok;
}
uint64_t TraceVectorCount(const void* base, size_t offset, size_t stride) {
    uintptr_t begin = 0, end = 0;
    if (!TraceRead(base, offset, begin) || !TraceRead(base, offset + sizeof(void*), end) ||
        end < begin || (end - begin) % stride || (end - begin) / stride > 10000) return UINT64_MAX;
    return (end - begin) / stride;
}
void TraceModules() {
    // Called only at diagnostic startup/observer attachment, never in a hook.
    if (g_traceFile == INVALID_HANDLE_VALUE) return;
    TraceHeader header{};
    memcpy(header.magic, "SNAPTRC", 8);
    header.format = 1; header.headerSize = sizeof(header); header.recordSize = sizeof(TraceRecord);
    header.capacity = kTraceCapacity; header.pid = GetCurrentProcessId();
    // Keep the journal format narrow while using Windhawk's version metadata.
    for (size_t i = 0; i + 1 < ARRAYSIZE(header.version) && WH_MOD_VERSION[i]; ++i)
        header.version[i] = static_cast<char>(WH_MOD_VERSION[i]);
    header.startTime = g_traceStartTime;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    MODULEENTRY32W entry{sizeof(entry)};
    if (snapshot != INVALID_HANDLE_VALUE) {
        if (Module32FirstW(snapshot, &entry)) do {
            if (header.moduleCount == ARRAYSIZE(header.modules)) break;
            auto& module = header.modules[header.moduleCount++];
            module.base = TracePointer(entry.modBaseAddr); module.size = entry.modBaseSize;
            wcsncpy_s(module.name, entry.szModule, _TRUNCATE);
            IMAGE_DOS_HEADER dos{}; IMAGE_NT_HEADERS64 pe{};
            if (TraceRead(entry.modBaseAddr, 0, dos) && dos.e_lfanew > 0 && dos.e_lfanew < 4096 &&
                TraceRead(entry.modBaseAddr, dos.e_lfanew, pe) && pe.Signature == IMAGE_NT_SIGNATURE)
                module.timestamp = pe.FileHeader.TimeDateStamp;
        } while (Module32NextW(snapshot, &entry));
        CloseHandle(snapshot);
    }
    LARGE_INTEGER zero{}; DWORD written = 0;
    if (!SetFilePointerEx(g_traceFile, zero, nullptr, FILE_BEGIN) ||
        !WriteFile(g_traceFile, &header, sizeof(header), &written, nullptr) || written != sizeof(header))
        g_traceActive = false;
}
void DrainTaskbarTrace() {
    std::vector<TraceRecord> records;
    records.reserve(kTraceCapacity);
    {
        std::lock_guard lock(g_traceMutex);
        records.swap(g_tracePending);
    }
    if (g_traceModulesRequested.exchange(false)) TraceModules();
    for (const auto& record : records) {
        LARGE_INTEGER offset{};
        offset.QuadPart = sizeof(TraceHeader) + ((record.sequence - 1) % kTraceCapacity) * sizeof(record);
        DWORD written = 0;
        if (!SetFilePointerEx(g_traceFile, offset, nullptr, FILE_BEGIN) ||
            !WriteFile(g_traceFile, &record, sizeof(record), &written, nullptr) ||
            written != sizeof(record)) {
            g_traceActive = false;
            Wh_Log(L"phase=trace write-failed code=%lu", GetLastError());
            break;
        }
    }
    if (g_traceFlush.exchange(false) && g_traceFile != INVALID_HANDLE_VALUE)
        FlushFileBuffers(g_traceFile);
}
DWORD WINAPI TaskbarTraceWriter(void*) {
    TraceModules();
    while (WaitForSingleObject(g_traceStop, 100) == WAIT_TIMEOUT) DrainTaskbarTrace();
    DrainTaskbarTrace();
    return 0;
}
void StartTaskbarTrace() {
    if (!g_binaryTraceEnabled) return;
    wchar_t root[32768]{};
    if (!Wh_GetModStoragePath(root, ARRAYSIZE(root))) return;
    std::wstring folder(root);
    if (!CreateDirectoryW(folder.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) return;
    folder += L"\\traces";
    if (!CreateDirectoryW(folder.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) return;
    // Retain seven older journals plus this process. Delete only our own
    // matching files, never recursively; locked journals are left untouched.
    struct OldTrace { std::wstring name; FILETIME time; };
    std::vector<OldTrace> old;
    WIN32_FIND_DATAW found{};
    HANDLE search = FindFirstFileW((folder + L"\\trace-*.bin").c_str(), &found);
    if (search != INVALID_HANDLE_VALUE) {
        do {
            if (!(found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) old.push_back({found.cFileName, found.ftLastWriteTime});
        } while (FindNextFileW(search, &found));
        FindClose(search);
    }
    std::sort(old.begin(), old.end(), [](const auto& a, const auto& b) { return CompareFileTime(&a.time, &b.time) < 0; });
    for (size_t i = 0; i + 7 < old.size(); ++i) DeleteFileW((folder + L"\\" + old[i].name).c_str());
    wchar_t suffix[96]{};
    swprintf_s(suffix, L"\\trace-%lu-%llu.bin", GetCurrentProcessId(), GetTickCount64());
    auto path = folder + suffix;
    {
        std::lock_guard lock(g_traceMutex);
        g_traceFile = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                                 CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        g_traceSequence = 0;
        g_tracePending.clear(); g_tracePending.reserve(kTraceCapacity);
        FILETIME now{}; GetSystemTimeAsFileTime(&now);
        memcpy(&g_traceStartTime, &now, sizeof(now));
    }
    g_traceActive = g_traceFile != INVALID_HANDLE_VALUE;
    g_traceStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_traceActive && g_traceStop)
        g_traceThread = CreateThread(nullptr, 0, TaskbarTraceWriter, nullptr, 0, nullptr);
    if (!g_traceThread) g_traceActive = false;
    TraceWrite(TraceStart);
    Wh_Log(L"phase=trace ready=%d path=%s capacity=%u", g_traceActive.load(), path.c_str(), kTraceCapacity);
}
void StopTaskbarTrace() {
    TraceWrite(TraceEnd, TraceStart);
    g_traceActive = false;
    if (g_traceStop) SetEvent(g_traceStop);
    if (g_traceThread) {
        WaitForSingleObject(g_traceThread, INFINITE);
        CloseHandle(g_traceThread); g_traceThread = nullptr;
    }
    if (g_traceStop) { CloseHandle(g_traceStop); g_traceStop = nullptr; }
    if (g_traceFile != INVALID_HANDLE_VALUE) { CloseHandle(g_traceFile); g_traceFile = INVALID_HANDLE_VALUE; }
}
void FlushTaskbarTrace() {
    g_traceFlush = true; // worker handles even urgent flush requests
}

struct RealityContext {
    void* controller;
    void* collection = nullptr;
    uint32_t sourceCount = 0;
    bool sourceCaptured = false;
};
thread_local RealityContext* g_realityContext = nullptr;
uintptr_t g_realitySizeReturn = 0;
uintptr_t g_realityCountReturn = 0;
using RealityCheck = void(WINAPI*)(void*);
using CollectionCount = uint32_t(WINAPI*)(void*);
RealityCheck g_realityCheck = nullptr;
CollectionCount g_collectionCount = nullptr, g_bindableSize = nullptr;
void CaptureRealitySize(uint32_t result, uintptr_t caller) {
    if (g_realityContext && caller == g_realitySizeReturn) {
        g_realityContext->sourceCount = result;
        g_realityContext->sourceCaptured = true;
    }
}
uint32_t WINAPI BindableSizeTraceHook(void* self) {
    auto result = g_bindableSize(self);
    CaptureRealitySize(result, reinterpret_cast<uintptr_t>(__builtin_return_address(0)));
    return result;
}
void CaptureRealityCount(void* self, uint32_t result, uintptr_t caller) {
    DWORD error = GetLastError();
    auto* context = g_realityContext;
    if (context && caller == g_realityCountReturn && context->collection == self && context->sourceCaptured) {
        TraceWrite(TraceCounts, context->sourceCount, result, TracePointer(self),
                   TraceVectorCount(self, 152, 40));
        if (context->sourceCount != result) {
            FlushTaskbarTrace();
            Wh_Log(L"phase=trace mismatch source=%u rendered=%u; native check retained",
                   context->sourceCount, result);
        }
    }
    SetLastError(error);
}
uint32_t WINAPI CollectionCountTraceHook(void* self) {
    auto result = g_collectionCount(self);
    CaptureRealityCount(self, result, reinterpret_cast<uintptr_t>(__builtin_return_address(0)));
    return result;
}
void WINAPI RealityCheckTraceHook(void* self) {
    RealityContext context{self};
    TraceRead(self, 648, context.collection);
    TraceScope operation(TraceCheckBegin, self, context.collection,
                        TraceVectorCount(context.collection, 152, 40));
    struct RestoreContext {
        RealityContext* previous = g_realityContext;
        ~RestoreContext() { g_realityContext = previous; }
    } restore;
    g_realityContext = &context;
    // No added COM calls, repair, exception handling or fail-fast suppression.
    // Capture values returned by the native calls in this exact check instead.
    g_realityCheck(self);
    TraceWrite(TraceCheckEnd, context.sourceCaptured, context.sourceCount);
    operation.result = 0;
}

bool CheckedNativeImage(PCWSTR name, DWORD timestamp, DWORD imageSize);
std::atomic<bool> g_viewTraceAttempted = false;
HANDLE g_viewTraceThread = nullptr;
bool HookTaskbarViewTrace() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module || !g_traceActive.load() || !g_traceNativeReady.load()) return false;
    if (g_viewTraceAttempted.exchange(true)) return true;
    wchar_t path[MAX_PATH]{};
    bool image = GetModuleFileNameW(module, path, ARRAYSIZE(path)) &&
                 CheckedNativeImage(path, 0x69a22f5e, 0xab6000);
    if (!image) { Wh_Log(L"phase=trace view-image-rejected=1"); return false; }
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{L"?CollectionRealityCheck@TaskbarController@implementation@Taskbar@winrt@@AEAAXXZ"}, &g_realityCheck, RealityCheckTraceHook},
        {{L"?NumTaskListGroups@TaskbarCollapsibleItemsCollection@implementation@Taskbar@winrt@@QEBAIXZ"}, &g_collectionCount, CollectionCountTraceHook},
        {{L"?Size@?$consume_Windows_UI_Xaml_Interop_IBindableVector@UIBindableVector@Interop@Xaml@UI@Windows@winrt@@@impl@winrt@@QEBA@XZ"}, &g_bindableSize, BindableSizeTraceHook},
    };
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options); options.noUndecoratedSymbols = TRUE;
    bool hooked = WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks), &options);
    g_realitySizeReturn = reinterpret_cast<uintptr_t>(module) + 0x37908d;
    g_realityCountReturn = reinterpret_cast<uintptr_t>(module) + 0x3790b2;
    // Observer is attached after ModInit; it must apply its own queued hooks.
    bool applied = hooked && Wh_ApplyHookOperations();
    g_traceModulesRequested = true;
    TraceWrite(TraceViewReady, image, hooked, applied);
    Wh_Log(L"phase=trace view-hooks=%d applied=%d", hooked, applied);
    return applied;
}

// Internal, versioned ABI. Only use these slots after QueryInterface succeeds.
// Slots were checked against twinui.pcshell.dll 10.0.26100.8117 public symbols
// and call sites. WindowId is a uint32_t containing the low 32 bits of HWND.
// Selection fields are decoded; unrecognized layouts preserve native selection.
constexpr GUID kDataModelStatics = {
    0xb9215acb, 0x5441, 0x5728, {0x93, 0x03, 0xed, 0xa3, 0xf4, 0x96, 0x83, 0xe6}};
constexpr GUID kDataModel = {
    0x908699e6, 0x4f5e, 0x5734, {0x84, 0x8b, 0x26, 0x97, 0x4b, 0x1c, 0x7d, 0x1d}};
constexpr GUID kTaskGroup = {
    0xd1faa115, 0x2bea, 0x5bd6, {0xa8, 0xee, 0x15, 0x8c, 0xee, 0x48, 0xdd, 0x8b}};
constexpr GUID kGroupAdapter = {
    0xfa7c57ad, 0x35e8, 0x5eb5, {0x89, 0x19, 0x3f, 0x3a, 0x76, 0x99, 0xb4, 0x8b}};
constexpr GUID kSwitchItem = {
    0x76593595, 0x88f6, 0x40c9, {0xbd, 0x90, 0x6f, 0x1a, 0x79, 0xae, 0xc5, 0xb8}};

template <typename Function>
Function AbiMethod(IUnknown* object, size_t slot) {
    return reinterpret_cast<Function>((*reinterpret_cast<void***>(object))[slot]);
}

struct AltTabContext {
    unsigned groupVisibilityDepth = 0;
    unsigned candidates = 0;
    unsigned hidden = 0;
    unsigned failures = 0;
    bool groupsEnabled = false;
    ComPtr<IUnknown> model;
};

// Keep only a trivial pointer in TLS: no DLL-owned TLS destructors may remain
// registered on Explorer threads after the mod is unloaded.
thread_local AltTabContext* g_context = nullptr;
thread_local unsigned g_cachedEvaluationDepth = 0;
struct AltTabMemberIdentity {
    HWND window;
    DWORD process, thread;
};
struct AltTabMembership {
    ULONGLONG updated = 0;
    std::vector<AltTabMemberIdentity> members;
};
struct AltTabMembershipCache {
    std::mutex mutex;
    std::shared_ptr<const AltTabMembership> value;
    auto load() { std::lock_guard lock(mutex); return value; }
    void store(std::shared_ptr<const AltTabMembership> next) {
        std::shared_ptr<const AltTabMembership> previous;
        { std::lock_guard lock(mutex); previous = std::move(value); value = std::move(next); }
    }
} g_altTabMembership;
HANDLE g_altTabSnapshotThread = nullptr;
bool CachedAltTabMember(HWND window) {
    auto snapshot = g_altTabMembership.load();
    if (!snapshot || GetTickCount64() - snapshot->updated > 2000) return false;
    DWORD process = 0;
    DWORD thread = GetWindowThreadProcessId(window, &process);
    return thread && std::any_of(snapshot->members.begin(), snapshot->members.end(), [&](const auto& member) {
        return member.window == window && member.process == process && member.thread == thread;
    });
}
std::atomic<bool> g_diagnosticMode = false;
std::atomic<bool> g_altTabEnabled = true;

bool NativeGroupsEnabled() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    LSTATUS result = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        L"EnableTaskGroups", RRF_RT_REG_DWORD, nullptr, &value, &size);
    // Missing value is the native default: enabled. Other errors fail open.
    return result == ERROR_FILE_NOT_FOUND || (result == ERROR_SUCCESS && value != 0);
}

HRESULT GetCurrentModel(ComPtr<IUnknown>& model) {
    constexpr wchar_t className[] =
        L"Windows.Internal.Shell.TaskGroups.TaskGroupDataModel";
    HSTRING name = nullptr;
    HRESULT hr = WindowsCreateString(className, ARRAYSIZE(className) - 1, &name);
    if (FAILED(hr)) return hr;
    ComPtr<IUnknown> factory;
    hr = RoGetActivationFactory(name, kDataModelStatics,
                               reinterpret_cast<void**>(factory.GetAddressOf()));
    WindowsDeleteString(name);
    if (FAILED(hr)) return hr;
    ComPtr<IUnknown> current;
    using GetCurrent = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, IUnknown**);
    hr = AbiMethod<GetCurrent>(factory.Get(), 6)(factory.Get(), current.GetAddressOf());
    if (FAILED(hr) || !current) return FAILED(hr) ? hr : E_UNEXPECTED;
    return current->QueryInterface(kDataModel,
                                  reinterpret_cast<void**>(model.GetAddressOf()));
}

bool g_cachedOnlyAltTab = false;

class AltTabScope {
public:
    explicit AltTabScope(bool acquireModel = true) {
        if (g_context) return;
        ownsContext_ = true;
        g_context = &context_;
        context_.groupsEnabled = NativeGroupsEnabled();
        if (context_.groupsEnabled && acquireModel && !g_cachedEvaluationDepth && !g_cachedOnlyAltTab) {
            HRESULT hr = GetCurrentModel(context_.model);
            if (FAILED(hr)) {
                ++context_.failures;
                Wh_Log(L"phase=model hr=%08X; keeping windows", hr);
            }
        }
    }
    ~AltTabScope() {
        if (!ownsContext_) return;
        g_context = nullptr;
        Wh_Log(L"phase=alt-tab candidates=%u hidden=%u failures=%u diagnostic=%d groups=%d",
               context_.candidates, context_.hidden, context_.failures,
               g_diagnosticMode.load(), context_.groupsEnabled);
    }
private:
    AltTabContext context_;
    bool ownsContext_ = false;
};

using GetWindowForView = HWND(WINAPI*)(void*);
GetWindowForView g_getWindowForView = nullptr;

ComPtr<IUnknown> FindGroup(HWND window) {
    if (!window || !g_context || !g_context->model || !g_context->groupsEnabled) return {};
    const auto id = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(window));
    using FindGroup = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, uint32_t, IUnknown**);
    ComPtr<IUnknown> candidate;
    HRESULT hr = AbiMethod<FindGroup>(g_context->model.Get(), 9)(
        g_context->model.Get(), id, candidate.GetAddressOf());
    if (FAILED(hr)) {
        ++g_context->failures;
        return {};
    }
    if (!candidate) return {};
    ComPtr<IUnknown> group;
    hr = candidate->QueryInterface(kTaskGroup,
                                   reinterpret_cast<void**>(group.GetAddressOf()));
    if (FAILED(hr)) {
        ++g_context->failures;
        return {};
    }

    // get_Windows returns a WinRT array (count + CoTaskMemAlloc buffer),
    // not IVectorView. Check membership as well as cardinality.
    using GetWindows = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, uint32_t*, uint32_t**);
    uint32_t count = 0;
    uint32_t* members = nullptr;
    hr = AbiMethod<GetWindows>(group.Get(), 7)(group.Get(), &count, &members);
    bool found = false;
    if (SUCCEEDED(hr) && members && count >= 2 && count <= 64) {
        for (uint32_t i = 0; i < count; ++i) found |= members[i] == id;
    } else if (FAILED(hr) || (count != 0 && !members) || count > 64) {
        ++g_context->failures;
    }
    CoTaskMemFree(members);
    return found ? group : ComPtr<IUnknown>{};
}

using IsViewVisible = HRESULT(WINAPI*)(void*, void*, BOOL*);
IsViewVisible g_isViewVisible = nullptr;
HRESULT WINAPI IsViewVisibleHook(void* desktop, void* view, BOOL* visible) {
    HRESULT hr = g_isViewVisible(desktop, view, visible);
    if (FAILED(hr) || !visible || !*visible || !view || !g_context ||
        g_context->groupVisibilityDepth != 0 || !g_context->groupsEnabled ||
        !g_altTabEnabled.load()) return hr;
    HWND window = g_getWindowForView(view);
    bool member = (g_cachedEvaluationDepth || g_cachedOnlyAltTab) ? CachedAltTabMember(window) : bool(FindGroup(window));
    if (member) {
        ++g_context->candidates;
        if (!g_diagnosticMode.load()) {
            *visible = FALSE;
            ++g_context->hidden;
        }
    }
    return hr;
}

// The native CachedDataSource EvaluateItem delegate runs for both initial
// evaluation and later invalidations (including re-insertion of the active view).
// Keep the filter active for the whole evaluation, not a guessed number of ms.
// If this compiler-generated symbol is absent, all filtering scopes use the
// background cache and skip model-based selection anchoring.
using EvaluateItem = void(WINAPI*)(void*, const void*, const void*);
EvaluateItem g_evaluateItem = nullptr;
void WINAPI EvaluateItemHook(void* self, const void* sender, const void* args) {
    // The native caller owns the switch-item list's exclusive SRW lock here.
    // Synchronous model COM can pump a reentrant Show which acquires that lock
    // shared on the same thread. Use only the worker's immutable identity data.
    struct EvaluationScope {
        EvaluationScope() { ++g_cachedEvaluationDepth; }
        ~EvaluationScope() { --g_cachedEvaluationDepth; }
    } evaluation;
    AltTabScope scope(false);
    g_evaluateItem(self, sender, args);
}

// Recover selection-only fields from the resolved constructor. An unrecognized
// instruction form leaves selection native; member filtering has no dependency
// on this layout. Only the r14=this/r15b=groupsEnabled compiler form is decoded.
struct CachedSourceLayout {
    size_t groupsEnabled = 0, groupAdapter = 0;
    explicit operator bool() const { return groupsEnabled && groupAdapter; }
};
CachedSourceLayout g_cachedSourceLayout;
void* g_cachedSourceConstructor = nullptr;
void* g_tryGetGroupAdapter = nullptr;
CachedSourceLayout DecodeCachedSourceLayout(const void* constructor, const void* adapterHelper,
        BOOL (*disasm)(void*, WH_DISASM_RESULT*) = Wh_Disasm) {
    if (!constructor || !adapterHelper) return {};
    std::array<unsigned char, 512> code{};
    if (!TraceRead(constructor, 0, code)) return {};
    bool thisSaved = false, argumentSaved = false;
    CachedSourceLayout layout;
    auto fieldOffset = [](const unsigned char* bytes, size_t length, unsigned char shortOpcode,
                          unsigned char longOpcode) -> size_t {
        int32_t offset = 0;
        if (length == 4 && bytes[2] == shortOpcode)
            offset = static_cast<int8_t>(bytes[3]);
        else if (length == 7 && bytes[2] == longOpcode)
            memcpy(&offset, bytes + 3, sizeof(offset));
        return offset > 0 && offset <= 4096 ? size_t(offset) : 0;
    };
    // Leave at least 16 readable bytes for the disassembler's maximum fetch.
    for (size_t cursor = 0; cursor + 16 <= code.size();) {
        auto* instruction = code.data() + cursor;
        WH_DISASM_RESULT decoded{};
        if (!disasm(instruction, &decoded) || !decoded.length || decoded.length > 15) return {};
        const size_t length = decoded.length;
        if (instruction[0] == 0xc3 || instruction[0] == 0xc2 ||
            instruction[0] == 0xe9 || instruction[0] == 0xeb) return {};
        if (length == 3 && !memcmp(instruction, "\x4c\x8b\xf1", 3)) thisSaved = true;
        if (length == 3 && !memcmp(instruction, "\x45\x8a\xf8", 3)) argumentSaved = true;
        if (thisSaved && argumentSaved && instruction[0] == 0x45 && instruction[1] == 0x88) {
            auto offset = fieldOffset(instruction, length, 0x7e, 0xbe);
            if (offset) {
                if (layout.groupsEnabled) return {}; // ambiguous assignments
                layout.groupsEnabled = offset;
            }
        }
        if (thisSaved && layout.groupsEnabled && instruction[0] == 0x49 && instruction[1] == 0x8d) {
            auto offset = fieldOffset(instruction, length, 0x5e, 0x9e);
            if (offset && cursor + length + 3 <= code.size() &&
                !memcmp(instruction + length, "\x48\x89\x0b", 3)) {
                if (layout.groupAdapter || offset % sizeof(void*)) return {};
                layout.groupAdapter = offset;
            }
        }
        // Match the groups-enabled branch and assignment of the result of the
        // independently resolved TryGetTaskGroupsDataSourceAdapter helper.
        if (layout && instruction[0] == 0x41 && instruction[1] == 0x80 &&
            (length == 5 || length == 8) && instruction[length - 1] == 0) {
            auto offset = fieldOffset(instruction, length - 1, 0x7e, 0xbe);
            if (offset == layout.groupsEnabled) {
                size_t branch = cursor + length;
                size_t jump = code[branch] == 0x74 ? 2 :
                    code[branch] == 0x0f && code[branch + 1] == 0x84 ? 6 : 0;
                size_t callBlock = branch + jump;
                if (!jump || callBlock + 24 > code.size()) return {};
                int32_t skip = 0;
                if (jump == 2) skip = static_cast<int8_t>(code[branch + 1]);
                else memcpy(&skip, code.data() + branch + 2, sizeof(skip));
                if (skip < 24 || callBlock + size_t(skip) > code.size()) return {};
                auto* block = code.data() + callBlock;
                if (memcmp(block, "\x49\x8b\x16\x48\x8d\x4c\x24", 7) ||
                    block[8] != 0xe8 || memcmp(block + 13, "\x48\x8b\xd0\x48\x8b\xcb\xe8", 7))
                    return {};
                int32_t displacement = 0;
                memcpy(&displacement, block + 9, sizeof(displacement));
                auto target = reinterpret_cast<uintptr_t>(constructor) + callBlock + 13 + displacement;
                if (target != reinterpret_cast<uintptr_t>(adapterHelper) ||
                    (layout.groupsEnabled >= layout.groupAdapter &&
                     layout.groupsEnabled < layout.groupAdapter + sizeof(void*))) return {};
                return layout;
            }
        }
        cursor += length;
    }
    return {};
}

using GetItemForView = IUnknown*(WINAPI*)(const void*, void*);
GetItemForView g_getItemForView = nullptr;
IUnknown* WINAPI GetItemForViewHook(const void* self, void* view) {
    if (g_cachedOnlyAltTab || !g_cachedSourceLayout || !self || !view ||
        g_diagnosticMode.load() || !g_altTabEnabled.load() || g_cachedEvaluationDepth)
        return g_getItemForView(self, view);
    unsigned char groupsEnabled = 0;
    IUnknown* groupAdapter = nullptr;
    if (!TraceRead(self, g_cachedSourceLayout.groupsEnabled, groupsEnabled) || groupsEnabled != 1 ||
        !TraceRead(self, g_cachedSourceLayout.groupAdapter, groupAdapter) || !groupAdapter)
        return g_getItemForView(self, view);
    ComPtr<IUnknown> adapter;
    // Retain the interface before model COM can pump a change to the source.
    HRESULT hr = groupAdapter->QueryInterface(kGroupAdapter,
        reinterpret_cast<void**>(adapter.GetAddressOf()));
    if (FAILED(hr) || !adapter) return g_getItemForView(self, view);
    AltTabScope scope;
    auto group = FindGroup(g_getWindowForView(view));
    if (!group) return g_getItemForView(self, view);
    ComPtr<IUnknown> source;
    using GetSource = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, IUnknown**);
    if (SUCCEEDED(hr)) {
        hr = AbiMethod<GetSource>(adapter.Get(), 7)(adapter.Get(), source.GetAddressOf());
    }
    ComPtr<IUnknown> item;
    using GetGroupItem = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, IUnknown*, IUnknown**);
    if (SUCCEEDED(hr) && source) {
        hr = AbiMethod<GetGroupItem>(source.Get(), 7)(source.Get(), group.Get(), item.GetAddressOf());
    }
    ComPtr<IUnknown> switchItem;
    if (SUCCEEDED(hr) && item) {
        hr = item->QueryInterface(kSwitchItem, reinterpret_cast<void**>(switchItem.GetAddressOf()));
    }
    if (FAILED(hr) || !switchItem) {
        Wh_Log(L"phase=selection group-item-unavailable hr=%08X", hr);
        // Do not point navigation at a filtered individual window card.
        return nullptr;
    }
    Wh_Log(L"phase=selection active-group-anchor");
    // Native Show first navigates to this anchor and then advances in the
    // requested direction before presenting the switcher.
    return switchItem.Detach();
}

using GroupVisible = bool(WINAPI*)(void*, const void*, void*);
GroupVisible g_groupVisible = nullptr;
bool WINAPI GroupVisibleHook(void* self, const void* group, void* desktop) {
    // Group eligibility internally queries its member windows. Keep that native
    // answer, otherwise filtering the members would also remove the group.
    if (!g_context) return g_groupVisible(self, group, desktop);
    ++g_context->groupVisibilityDepth;
    bool result = g_groupVisible(self, group, desktop);
    --g_context->groupVisibilityDepth;
    return result;
}

using Show = HRESULT(WINAPI*)(void*, void*, int, void*);
Show g_show = nullptr;
HRESULT WINAPI ShowHook(void* self, void* monitor, int flags, void* view) {
    AltTabScope scope;
    return g_show(self, monitor, flags, view);
}

using CreateInstance = HRESULT(WINAPI*)(const void*, REFIID, void**);
CreateInstance g_createInstance = nullptr;
HRESULT WINAPI CreateInstanceHook(const void* args, REFIID iid, void** result) {
    AltTabScope scope;
    return g_createInstance(args, iid, result);
}

// Taskbar integration uses shell APIs and internal hooks. Real application window
// styles and AppUserModelIDs are never changed. Only our proxy buttons have IDs.
struct TaskbarSettings {
    bool enabled = true;
    bool hideMembers = true;
    bool separateGroups = true;
    bool centerLastRow = false;
    bool toggleOnClick = true;
    bool minimizeOnClose = true;
    bool currentDesktopOnly = true;
    bool nativePreview = true;
    bool screenIconOrder = true;
    int maxIcons = 4;
    int iconSize = 14;
    int gap = 2;
    int interval = 500;
};
std::mutex g_taskbarSettingsMutex;
TaskbarSettings g_taskbarSettings;
std::atomic<unsigned> g_settingsGeneration = 0;
HANDLE g_taskbarThread = nullptr;
HANDLE g_taskbarStop = nullptr;
constexpr wchar_t kProxyClass[] = L"Windhawk.HideSnapped.GroupButton";
constexpr UINT kMemberDestroyed = WM_APP + 17;
std::atomic<HWND> g_taskbarHost = nullptr;
UINT g_taskbarCreated = 0;

struct WindowIdentity {
    HWND window{};
    DWORD process{};
    DWORD thread{};
    bool operator<(const WindowIdentity& other) const {
        if (window != other.window) return uintptr_t(window) < uintptr_t(other.window);
        if (process != other.process) return process < other.process;
        return thread < other.thread;
    }
    bool operator==(const WindowIdentity&) const = default;
};
using IsTaskAllowed = bool(WINAPI*)(void*, void*);
using MatchTaskWindow = HRESULT(WINAPI*)(void*, HWND, const void*, PCWSTR, int, IUnknown**, IUnknown**);
using InclusionChanged = HRESULT(WINAPI*)(void*, void*, void*);
using TaskCreated = HRESULT(WINAPI*)(void*, void*, void*, int);
TaskCreated g_taskCreated = nullptr;
using ActivateTask = HRESULT(WINAPI*)(void*, IUnknown*, IUnknown*);
using GetTaskIcon = HRESULT(WINAPI*)(void*, IUnknown*, IUnknown*, HICON*);
using UpdateTaskIcon = void(WINAPI*)(void*, IUnknown*, IUnknown*);
UpdateTaskIcon g_updateTaskIcon = nullptr;
GetTaskIcon g_getTaskIcon = nullptr;
using NativeItemCount = int(WINAPI*)(void*);
NativeItemCount g_nativeItemCount = nullptr;
void* g_nativeGroupVtable = nullptr;
size_t g_nativeItemsOffset = 0;
std::atomic<bool> g_nativePreviewAvailable = true;
HICON WindowIcon(HWND window);
IsTaskAllowed g_isTaskAllowed = nullptr;
IsTaskAllowed g_listIsTaskAllowed = nullptr;
MatchTaskWindow g_matchTaskWindow = nullptr;
InclusionChanged g_inclusionChanged = nullptr;
InclusionChanged g_multiInclusionChanged = nullptr;
ActivateTask g_activateTask = nullptr;
ActivateTask g_multiActivateTask = nullptr;
using ShouldAddWindow = int(WINAPI*)(HWND);
ShouldAddWindow g_shouldAddWindow = nullptr;
void* g_taskListVtable = nullptr;
void* g_multiTaskListVtable = nullptr;
std::mutex g_hiddenMembersMutex;
std::set<WindowIdentity> g_hiddenMembers;
std::map<WindowIdentity, HWND> g_groupButtons;
std::set<WindowIdentity> g_proxyWindows;
// Worker-published immutable routing. Monitor queries can occur while shell
// code holds our membership mutex, so this hook must not acquire that mutex.
using ProxyMonitors = std::map<WindowIdentity, HMONITOR>;
struct ProxyMonitorCache {
    std::shared_ptr<const ProxyMonitors> value;
    auto load() { return std::atomic_load(&value); }
    void store(std::shared_ptr<const ProxyMonitors> next) { std::atomic_store(&value, std::move(next)); }
} g_proxyMonitors;
decltype(&MonitorFromWindow) g_monitorFromWindow = nullptr;
// Native group references are acquired/released by the worker; UI preview use
// additionally requires IAgileObject. Process exit kills the worker before CRT
// teardown; never Release these references from a global destructor.
// The worker clears all references on controlled shutdown; ModUninit frees
// the empty container after Windhawk has removed the hooks that read it.
[[clang::no_destroy]] std::optional<std::map<WindowIdentity, ComPtr<IUnknown>>>
    g_proxyGroups{std::in_place};
// Caller holds g_hiddenMembersMutex; the returned reference must outlive its
// lock guard so replacement/removal cannot Release a group inside that lock.
ComPtr<IUnknown> ExchangeProxyGroupLocked(const WindowIdentity& identity, IUnknown* next) {
    auto found = g_proxyGroups->find(identity);
    if (found == g_proxyGroups->end()) {
        if (next) (*g_proxyGroups)[identity] = next;
        return {};
    }
    auto retired = std::move(found->second);
    if (next) found->second = next;
    else g_proxyGroups->erase(found); // moved-from element owns no COM reference
    return retired;
}
WindowIdentity g_lastAppForeground;
std::atomic<bool> g_taskbarFilterReady = false;
std::atomic<bool> g_taskbarInitAttempted = false;
thread_local const std::set<WindowIdentity>* g_taskbarEligibilitySnapshot = nullptr;
thread_local void* g_excludedPresentationItem = nullptr;
struct PresentationRefresh {
    void* group;
    void* task;
    IUnknown* realTask;
    HWND window;
};
thread_local const PresentationRefresh* g_presentationRefresh = nullptr;
bool SuppressTaskbarCreation(void* self, void* group, void* item);
void* FindTaskListInterface(void* object, void* vtable);
WindowIdentity IdentifyWindow(HWND window);
HMONITOR WINAPI MonitorFromWindowHook(HWND window, DWORD flags);

// The worker can change membership while native creation is between its first
// eligibility test and the synchronous XAML collection notification. Both must
// see one answer. Other taskbar mods can add work in this interval. Hold a
// per-thread snapshot, never the global mutex across native/COM calls.
class TaskbarMutationScope {
    std::set<WindowIdentity> members;
    const std::set<WindowIdentity>* previous = g_taskbarEligibilitySnapshot;
public:
    TaskbarMutationScope() {
        if (!previous) {
            {
                std::lock_guard lock(g_hiddenMembersMutex);
                members = g_hiddenMembers;
            }
            g_taskbarEligibilitySnapshot = &members;
            TraceWrite(TraceSnapshot, members.size());
        }
    }
    ~TaskbarMutationScope() { g_taskbarEligibilitySnapshot = previous; }
    TaskbarMutationScope(const TaskbarMutationScope&) = delete;
    TaskbarMutationScope& operator=(const TaskbarMutationScope&) = delete;
};
bool IsHiddenTaskbarMember(const WindowIdentity& member) {
    if (g_taskbarEligibilitySnapshot) return g_taskbarEligibilitySnapshot->contains(member);
    std::lock_guard lock(g_hiddenMembersMutex);
    return g_hiddenMembers.contains(member);
}
HRESULT WINAPI TaskCreatedHook(void* self, void* group, void* item, int flags) {
    TraceScope trace(TraceCreateTask, self, group, TracePointer(item));
    TaskbarMutationScope scope;
    if (SuppressTaskbarCreation(self, group, item)) {
        trace.result = S_OK;
        return S_OK; // no native button/source insertion for an excluded presentation
    }
    auto hr = g_taskCreated(self, group, item, flags);
    trace.result = uint32_t(hr);
    return hr;
}
HRESULT WINAPI InclusionChangedHook(void* self, void* group, void* item) {
    TraceScope trace(TraceInclusion, self, group, TracePointer(item));
    TaskbarMutationScope scope;
    void* previous = g_excludedPresentationItem;
    struct RestoreExclusion {
        void* previous;
        ~RestoreExclusion() { g_excludedPresentationItem = previous; }
    } restore{previous};
    if (g_presentationRefresh && g_presentationRefresh->group == group &&
        g_presentationRefresh->task == item) {
        // Query only concrete lists. Multi fans out here; its own policy
        // method is an unsupported-operation fail-fast stub on this build.
        if (!g_listIsTaskAllowed) return E_NOINTERFACE;
        bool allowed = !IsHiddenTaskbarMember(IdentifyWindow(g_presentationRefresh->window)) &&
            g_listIsTaskAllowed(self, g_presentationRefresh->realTask);
        g_excludedPresentationItem = allowed ? nullptr : item;
    }
    auto hr = g_inclusionChanged(self, group, item);
    trace.result = uint32_t(hr);
    return hr;
}
HRESULT WINAPI MultiInclusionChangedHook(void* self, void* group, void* item) {
    TraceScope trace(TraceMultiInclusion, self, group, TracePointer(item));
    TaskbarMutationScope scope;
    auto hr = g_multiInclusionChanged(self, group, item);
    trace.result = uint32_t(hr);
    return hr;
}
UINT g_refreshTask = 0;
constexpr UINT_PTR kTaskListSubclass = 0x534e4150;

bool IsProxyWindow(HWND window) {
    auto identity = IdentifyWindow(window);
    std::lock_guard lock(g_hiddenMembersMutex);
    return g_proxyWindows.contains(identity);
}
bool IsTaskbarSurface(HWND window) {
    HWND root = GetAncestor(window, GA_ROOT);
    wchar_t name[64]{};
    GetClassNameW(root ? root : window, name, ARRAYSIZE(name));
    return wcscmp(name, L"Shell_TrayWnd") == 0 || wcscmp(name, L"Shell_SecondaryTrayWnd") == 0;
}
void RememberAppForeground(HWND window) {
    if (!window || IsTaskbarSurface(window)) return;
    auto identity = IdentifyWindow(window);
    if (!identity.thread) return;
    std::lock_guard lock(g_hiddenMembersMutex);
    if (!g_proxyWindows.contains(identity)) g_lastAppForeground = identity;
}
constexpr UINT kTaskbarClick = WM_APP + 19;
using HandleTaskClick = HRESULT(WINAPI*)(void*, IUnknown*, IUnknown*, const void*);
HandleTaskClick g_handleTaskClick = nullptr, g_multiHandleTaskClick = nullptr;
using CreateWindowVisual = HRESULT(WINAPI*)(void*, HWND, SIZE);
CreateWindowVisual g_createWindowVisual = nullptr;
HRESULT WINAPI CreateWindowVisualHook(void*, HWND, SIZE);
using HandleButtonClick = void(WINAPI*)(void*, IUnknown*, int, int, int, int);
HandleButtonClick g_handleButtonClick = nullptr;
using UpdateThumbnailFactory = void(WINAPI*)(void*);
UpdateThumbnailFactory g_updateThumbnailFactory = nullptr;
void WINAPI UpdateThumbnailFactoryHook(void*);
bool PostProxyClick(IUnknown* item,
                    BOOL (WINAPI* post)(HWND, UINT, WPARAM, LPARAM) = PostMessageW) {
    using GetWindow = HWND(WINAPI*)(IUnknown*);
    HWND window = item ? AbiMethod<GetWindow>(item, 12)(item) : nullptr;
    if (!IsProxyWindow(window)) return false;
    // A failed post must not fall through to native proxy activation.
    BOOL sent = post(window, kTaskbarClick, 0, reinterpret_cast<LPARAM>(GetForegroundWindow()));
    Wh_Log(L"phase=taskbar click-source=button posted=%d", sent);
    return true;
}
void RouteButtonClick(HandleButtonClick original, void* self, IUnknown* buttonGroup,
                      int index, int action, int flags, int extra,
                      BOOL (WINAPI* post)(HWND, UINT, WPARAM, LPARAM) = PostMessageW) {
    // DoDefaultAction (the modern taskbar/accessibility path) bypasses HandleClick
    // and calls _HandleClick with action=0. Preserve launch/menu/other actions.
    if (buttonGroup && action == 0 && index >= 0) {
        using GetItem = IUnknown*(WINAPI*)(IUnknown*, int);
        IUnknown* item = AbiMethod<GetItem>(buttonGroup, 38)(buttonGroup, index);
        if (PostProxyClick(item, post)) return;
    }
    original(self, buttonGroup, index, action, flags, extra);
}
void WINAPI HandleButtonClickHook(void* self, IUnknown* group, int index, int action, int flags, int extra) {
    RouteButtonClick(g_handleButtonClick, self, group, index, action, flags, extra);
}
HRESULT RouteTaskClick(HandleTaskClick original, void* self, IUnknown* group,
                       IUnknown* item, const void* options,
                       BOOL (WINAPI* post)(HWND, UINT, WPARAM, LPARAM) = PostMessageW) {
    using GetWindow = HWND(WINAPI*)(IUnknown*);
    HWND window = item ? AbiMethod<GetWindow>(item, 12)(item) : nullptr;
    if (!IsProxyWindow(window)) return original(self, group, item, options);
    // A highlighted taskbar button need not own the foreground HWND. Intercept
    // the native click itself rather than relying on a second WM_ACTIVATE.
    return post(window, kTaskbarClick, 0, reinterpret_cast<LPARAM>(GetForegroundWindow())) ? S_OK : E_FAIL;
}
HRESULT WINAPI HandleTaskClickHook(void* self, IUnknown* group, IUnknown* item, const void* options) {
    return RouteTaskClick(g_handleTaskClick, self, group, item, options);
}
HRESULT WINAPI MultiHandleTaskClickHook(void* self, IUnknown* group, IUnknown* item, const void* options) {
    return RouteTaskClick(g_multiHandleTaskClick, self, group, item, options);
}
int WINAPI ShouldAddWindowHook(HWND window) {
    // The native helper rejects WS_EX_TOOLWINDOW, including AddTab requests.
    // Admit only our own live proxies; retain their switcher-excluding style.
    return IsProxyWindow(window) ? 1 : g_shouldAddWindow(window);
}
constexpr GUID kNativeTaskGroupItem = {
    0xf3ef3cde, 0x6457, 0x4083, {0x8f, 0x2b, 0x33, 0x6c, 0xdc, 0xfa, 0x58, 0x5a}};
HWND NativeGroupPrimaryWindow(IUnknown* item) {
    if (!item) return nullptr;
    ComPtr<IUnknown> nativeGroup;
    if (FAILED(item->QueryInterface(kNativeTaskGroupItem,
            reinterpret_cast<void**>(nativeGroup.GetAddressOf())))) return nullptr;
    // Native Snap Group task items have no GetWindow(). Their separate
    // ITaskGroupTaskItem interface exposes the primary member at checked slot 5.
    using PrimaryWindow = HWND(WINAPI*)(IUnknown*);
    return AbiMethod<PrimaryWindow>(nativeGroup.Get(), 5)(nativeGroup.Get());
}

// Recognize the complete x64 leaf getter: load this->DPA; return its count or
// zero for null. Both displacement forms must have the exact null/count flow.
// A changed compiler form fails open; no image identity or fixed field offset.
size_t DecodeNativeItemsOffset(const void* getter) {
    std::array<unsigned char, 17> code{};
    if (!getter || !TraceRead(getter, 0, code) || code[0] != 0x48 || code[1] != 0x8b) return 0;
    size_t length = 0;
    int32_t offset = 0;
    if (code[2] == 0x51) { length = 4; offset = static_cast<int8_t>(code[3]); }
    else if (code[2] == 0x91) { length = 7; memcpy(&offset, code.data() + 3, 4); }
    constexpr unsigned char suffix[] = {0x33,0xc0,0x48,0x85,0xd2,0x74,0x02,0x8b,0x02,0xc3};
    if (!length || memcmp(code.data() + length, suffix, sizeof(suffix)) ||
        offset < 8 || offset > 4096 || offset % sizeof(void*)) return 0;
    return size_t(offset);
}
bool HasNativeGroupVtable(void* group) {
    void* table = nullptr;
    return group && g_nativeGroupVtable && TraceRead(group, 0, table) && table == g_nativeGroupVtable;
}
// Retain interfaces only after the native type, decoded field, DPA and native
// count agree. Unknown/incomplete layouts leave creation/presentation native.
std::vector<ComPtr<IUnknown>> NativeTaskItems(void* group) {
    void* dpa = nullptr;
    int count = 0;
    void* entries = nullptr;
    std::vector<ComPtr<IUnknown>> items;
    if (!g_nativeItemsOffset || !g_nativeItemCount || !HasNativeGroupVtable(group) ||
        !TraceRead(group, g_nativeItemsOffset, dpa) || !dpa || !TraceRead(dpa, 0, count) ||
        count <= 0 || count > 4096 || count != g_nativeItemCount(group) ||
        !TraceRead(dpa, 8, entries) || !entries) return items;
    for (int i = 0; i < count; ++i) {
        IUnknown* item = nullptr;
        if (!TraceRead(entries, size_t(i) * sizeof(void*), item) || !item) return {};
        items.emplace_back(item);
    }
    return items;
}
HWND TaskItemWindow(IUnknown* item) {
    using GetWindow = HWND(WINAPI*)(IUnknown*);
    return item ? AbiMethod<GetWindow>(item, 12)(item) : nullptr;
}
bool SuppressTaskbarCreation(void* self, void* group, void* item) {
    if (item) {
        auto task = static_cast<IUnknown*>(item);
        // Ordinary window eligibility remains native. Only the additional
        // HWND-less group item needs a gate before _CreateTBGroup, not after it.
        if (TaskItemWindow(task)) return false;
        HWND primary = NativeGroupPrimaryWindow(task);
        if (!primary) return false;
        if (IsHiddenTaskbarMember(IdentifyWindow(primary))) {
            TraceWrite(TracePresentation, TracePointer(group), TracePointer(item), TracePointer(primary), 1);
            return true;
        }
        // Native group notifications arrive before our next worker scan too.
        // Per-monitor cannot classify their null HWND, but the model already
        // contains the real primary task. Apply that task's policy before any
        // _CreateTBGroup/source insertion, even with an empty hidden snapshot.
        void* list = FindTaskListInterface(self, g_taskListVtable);
        if (list && g_listIsTaskAllowed) for (const auto& candidate : NativeTaskItems(group)) {
            if (TaskItemWindow(candidate.Get()) != primary) continue;
            if (g_listIsTaskAllowed(list, candidate.Get())) return false;
            TraceWrite(TracePresentation, TracePointer(group), TracePointer(item), TracePointer(primary), 4);
            return true;
        }
        // Unknown native layout/primary membership retains native behavior.
        TraceWrite(TracePresentation, TracePointer(group), TracePointer(item), TracePointer(primary), 5);
        return false;
    }
    if (g_taskbarEligibilitySnapshot && g_taskbarEligibilitySnapshot->empty()) return false;
    // Per-monitor's pin pass retries with item=null after its window pass.
    // Suppress that retry only when every native task is already represented.
    // Empty pins, unknown tasks and any unrelated window retain native behavior.
    auto items = NativeTaskItems(group);
    if (items.empty()) return false;
    for (const auto& task : items) {
        HWND window = TaskItemWindow(task.Get());
        if (!window) window = NativeGroupPrimaryWindow(task.Get());
        if (!window || !IsHiddenTaskbarMember(IdentifyWindow(window))) return false;
    }
    TraceWrite(TracePresentation, TracePointer(group), 0, items.size(), 2);
    return true;
}
// Native GetIcon fail-fasts when a group has no button. Hiding member buttons
// intentionally violates that precondition; native Snap UI can still ask for
// their icons while replacing/resizing a group. Hidden members always use an
// owned copy of their window icon; no task-list/button object fields are read.
HRESULT RouteTaskIcon(GetTaskIcon original, void* self, IUnknown* group, IUnknown* item,
                      HICON* icon, HICON (*source)(HWND) = WindowIcon) {
    if (!icon) return E_POINTER;
    HWND window = TaskItemWindow(item);
    if (!window) window = NativeGroupPrimaryWindow(item);
    if (!IsHiddenTaskbarMember(IdentifyWindow(window))) return original(self, group, item, icon);
    *icon = nullptr;
    *icon = source(window); // CopyIcon: caller owns this HICON, as with native GetIcon.
    HRESULT hr = *icon ? S_OK : E_FAIL;
    static std::atomic<unsigned> reports = 0;
    if (reports.fetch_add(1) < 8)
        Wh_Log(L"phase=taskbar icon-source=window missing-button=1 hr=%08X", hr);
    return hr;
}
HRESULT WINAPI GetTaskIconHook(void* self, IUnknown* group, IUnknown* item, HICON* icon) {
    return RouteTaskIcon(g_getTaskIcon, self, group, item, icon);
}

HWND RepresentedButton(void* item) {
    if (!item) return nullptr;
    using GetWindow = HWND(WINAPI*)(void*);
    HWND window = reinterpret_cast<GetWindow>((*reinterpret_cast<void***>(item))[12])(item);
    DWORD process = 0;
    DWORD thread = GetWindowThreadProcessId(window, &process);
    std::lock_guard lock(g_hiddenMembersMutex);
    auto found = g_groupButtons.find({window, process, thread});
    return found == g_groupButtons.end() ? nullptr : found->second;
}
HRESULT RouteTaskActivation(ActivateTask original, void* self, IUnknown* group, IUnknown* item) {
    if (item) {
        using GetWindow = HWND(WINAPI*)(IUnknown*);
        RememberAppForeground(AbiMethod<GetWindow>(item, 12)(item));
    }
    HWND proxy = RepresentedButton(item);
    if (proxy) {
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        HWND bandWindow = reinterpret_cast<HWND>(GetPropW(tray, L"TaskbandHWND"));
        void* band = bandWindow ? reinterpret_cast<void*>(GetWindowLongPtrW(bandWindow, 0)) : nullptr;
        ComPtr<IUnknown> proxyGroup, proxyItem;
        if (band && SUCCEEDED(g_matchTaskWindow(band, proxy, nullptr, nullptr, 4,
                                               proxyGroup.GetAddressOf(), proxyItem.GetAddressOf())) &&
            proxyGroup && proxyItem) return original(self, proxyGroup.Get(), proxyItem.Get());
    }
    return original(self, group, item);
}
HRESULT WINAPI ActivateTaskHook(void* self, IUnknown* group, IUnknown* item) {
    return RouteTaskActivation(g_activateTask, self, group, item);
}
HRESULT WINAPI MultiActivateTaskHook(void* self, IUnknown* group, IUnknown* item) {
    return RouteTaskActivation(g_multiActivateTask, self, group, item);
}

bool WINAPI IsTaskAllowedHook(void* self, void* item) {
    if (!g_isTaskAllowed(self, item)) {
        TraceWrite(TraceFilter, TracePointer(item), 0, 0, 0);
        return false;
    }
    if (!item) return true;
    if (item == g_excludedPresentationItem) {
        TraceWrite(TraceFilter, TracePointer(item), 0, 1, 0);
        return false;
    }
    // ITaskItem::GetWindow slot 12 checked for both desktop/immersive vtables.
    using GetWindow = HWND(WINAPI*)(void*);
    HWND window = reinterpret_cast<GetWindow>((*reinterpret_cast<void***>(item))[12])(item);
    // A native Snap Group is not an individual member window. This predicate
    // also feeds native button creation and the XAML collection; hiding a group
    // here to remove a hover entry changes model eligibility. Preserve the
    // native answer, even when its primary window has a replacement button.
    if (!window) { TraceWrite(TraceFilter, TracePointer(item), 0, 1, 1); return true; }
    DWORD process = 0;
    DWORD thread = GetWindowThreadProcessId(window, &process);
    bool allowed = !IsHiddenTaskbarMember({window, process, thread});
    TraceWrite(TraceFilter, TracePointer(item), TracePointer(window), 1, allowed);
    return allowed;
}

using CreateButton = void*(WINAPI*)(void*, void*, int);
using NotifyInserted = void(WINAPI*)(void*, void*, int);
using ChangeSourceGroup = void(WINAPI*)(void*, void* const*, uint32_t);
using RemoveSourceGroup = void(WINAPI*)(void*, void* const*);
using NativeAppId = PCWSTR(WINAPI*)(void*);
CreateButton g_createButton = nullptr;
NotifyInserted g_notifyInserted = nullptr;
ChangeSourceGroup g_insertSourceGroup = nullptr, g_moveSourceGroup = nullptr;
RemoveSourceGroup g_removeSourceGroup = nullptr;
NativeAppId g_nativeAppId = nullptr;
uintptr_t g_nativeAppIdReturn = 0;
void* WINAPI CreateButtonTraceHook(void* self, void* group, int index) {
    TraceScope trace(TraceCreateButton, self, group, uint32_t(index));
    auto result = g_createButton(self, group, index);
    trace.result = TracePointer(result);
    return result;
}
void WINAPI NotifyInsertedTraceHook(void* self, void* group, int index) {
    TraceScope trace(TraceNotify, self, group, uint32_t(index));
    g_notifyInserted(self, group, index);
    trace.result = 0;
}
void WINAPI InsertSourceTraceHook(void* self, void* const* group, uint32_t index) {
    TraceScope trace(TraceInsert, self, group ? *group : nullptr, index);
    TraceWrite(TraceSnapshot, TraceVectorCount(self, 64, 8));
    g_insertSourceGroup(self, group, index);
    trace.result = TraceVectorCount(self, 64, 8);
}
void WINAPI RemoveSourceTraceHook(void* self, void* const* group) {
    TraceScope trace(TraceRemove, self, group ? *group : nullptr);
    TraceWrite(TraceSnapshot, TraceVectorCount(self, 64, 8));
    g_removeSourceGroup(self, group);
    trace.result = TraceVectorCount(self, 64, 8);
}
void WINAPI MoveSourceTraceHook(void* self, void* const* group, uint32_t index) {
    TraceScope trace(TraceMove, self, group ? *group : nullptr, index);
    TraceWrite(TraceSnapshot, TraceVectorCount(self, 64, 8));
    g_moveSourceGroup(self, group, index);
    trace.result = TraceVectorCount(self, 64, 8);
}
void CaptureNullAppId(void* self, PCWSTR result, uintptr_t caller) {
    // GetAppID shares code with CTaskItem::GetTabContainerItem. A null return
    // is AppID evidence only at this inspected get_AppId call site.
    if (!result && caller == g_nativeAppIdReturn) {
        TraceWrite(TraceNullAppId, TracePointer(self)); FlushTaskbarTrace();
    }
}
PCWSTR WINAPI NativeAppIdTraceHook(void* self) {
    auto result = g_nativeAppId(self);
    CaptureNullAppId(self, result, reinterpret_cast<uintptr_t>(__builtin_return_address(0)));
    return result; // don't read, replace or log the string
}

void* FindTaskListInterface(void* object, void* vtable) {
    // Bound the search to the inspected native prefix AND its readable region.
    MEMORY_BASIC_INFORMATION memory{};
    if (!object || !vtable || !VirtualQuery(object, &memory, sizeof(memory)) ||
        memory.State != MEM_COMMIT || (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS))) return nullptr;
    uintptr_t end = uintptr_t(memory.BaseAddress) + memory.RegionSize;
    for (size_t offset = 0; offset < 256 && uintptr_t(object) + offset + sizeof(void*) <= end;
         offset += sizeof(void*)) {
        auto current = reinterpret_cast<void**>(static_cast<char*>(object) + offset);
        if (*current == vtable) return current;
    }
    return nullptr;
}

constexpr WPARAM kRefreshTaskIcon = 2;
constexpr WPARAM kRestoreTask = 3;
HRESULT RefreshMatchedTask(WPARAM action, void* band, void* list, HWND window,
        IUnknown* group, IUnknown* item, InclusionChanged inclusion,
        ActivateTask activate, UpdateTaskIcon updateIcon) {
    if (!band || !list || !group || !item) return E_PENDING;
    if (action == kRefreshTaskIcon) {
        if (!updateIcon) return E_NOINTERFACE;
        {
            std::lock_guard lock(g_hiddenMembersMutex);
            if (!g_proxyWindows.contains(IdentifyWindow(window))) return E_ACCESSDENIED;
        }
        // Runs on the native task-list thread. Updating the icon container
        // triggers Win32AppIconLoaded's native cache/publication notifications.
        updateIcon(band, group, item);
        return S_OK; // request issued; rendering completes asynchronously
    }
    if (action > 1) return E_INVALIDARG;
    if (!inclusion || (action && !activate)) return E_NOINTERFACE;
    TaskbarMutationScope scope;
    HRESULT hr = inclusion(list, group, item);
    if (SUCCEEDED(hr) && action) hr = activate(list, group, item);
    return hr;
}

HRESULT RefreshTaskWindow(WPARAM action, void* band, void* list, HWND window,
        MatchTaskWindow match, InclusionChanged inclusion,
        ActivateTask activate, UpdateTaskIcon updateIcon) {
    TraceScope trace(TraceRefresh, list, window, action);
    if (action > kRestoreTask) return E_INVALIDARG;
    if (!band || !list || !match) return E_NOINTERFACE;
    // A sent message may arrive while the native task list pumps COM/window
    // calls inside creation. Defer our inclusion/icon/activation until the outer
    // native transaction returns; a successful no-op would lose the update.
    if (g_taskbarEligibilitySnapshot) {
        static std::atomic<unsigned> reports = 0;
        if (reports.fetch_add(1) < 8)
            Wh_Log(L"phase=taskbar refresh-deferred native-mutation=1");
        return E_PENDING;
    }
    ComPtr<IUnknown> group, item;
    HRESULT hr = match(band, window, nullptr, nullptr, 4,
                       group.GetAddressOf(), item.GetAddressOf());
    // Exact _MatchWindow initializes E_FAIL/null outputs and retains them when
    // no task matches. After unfiltering a dissolved group's member there is
    // nothing to refresh in that case: future native creation is already allowed.
    // Do not manufacture a task or retain the old proxy forever. Creation/icon
    // confirmation must still fail/retry, as must partial matches and other errors.
    if (action == kRestoreTask && hr == E_FAIL && !group && !item) {
        static std::atomic<unsigned> reports = 0;
        if (reports.fetch_add(1) < 8) Wh_Log(L"phase=taskbar restore-no-task=1");
        return S_FALSE;
    }
    if (FAILED(hr) || !group || !item) {
        static std::atomic<unsigned> reports = 0;
        if (action == kRestoreTask && reports.fetch_add(1) < 16)
            Wh_Log(L"phase=taskbar restore-failed stage=match hr=%08X group=%d item=%d",
                   hr, group ? 1 : 0, item ? 1 : 0);
        return FAILED(hr) ? hr : E_PENDING;
    }
    // _MatchWindow can already find a proxy's unresolved native task. AddTab
    // completion does not imply that CTaskGroup's AppID is initialized. Our
    // inclusion refresh would create its XAML button early; get_AppId then
    // dereferences the null native ID before it can return an error.
    // Use the resolved native getter after verifying the concrete group type;
    // never invoke the WinRT string wrapper or assume its vtable slot.
    if (!HasNativeGroupVtable(group.Get()) || !g_nativeAppId) return E_NOINTERFACE;
    if (!g_nativeAppId(group.Get())) {
        TraceWrite(TraceNullAppId, TracePointer(group.Get()), TracePointer(window), action, 1);
        return E_PENDING;
    }
    TaskbarMutationScope mutation;
    auto nativeItems = action == kRefreshTaskIcon ? std::vector<ComPtr<IUnknown>>{} : NativeTaskItems(group.Get());
    hr = S_OK;
    for (const auto& task : nativeItems) {
        if (TaskItemWindow(task.Get()) || NativeGroupPrimaryWindow(task.Get()) != window) continue;
        if (!inclusion) { hr = E_NOINTERFACE; break; }
        struct PresentationScope {
            PresentationRefresh context;
            const PresentationRefresh* previous = g_presentationRefresh;
            PresentationScope(void* group, void* task, IUnknown* realTask, HWND window)
                : context{group, task, realTask, window} { g_presentationRefresh = &context; }
            ~PresentationScope() { g_presentationRefresh = previous; }
        } presentation(group.Get(), task.Get(), item.Get(), window);
        TraceWrite(TracePresentation, TracePointer(group.Get()), TracePointer(task.Get()), TracePointer(window), 3);
        // Native inclusion removes/restores only this taskbar representation.
        // Global group eligibility and the shared shell model stay native.
        HRESULT presentationHr = inclusion(list, group.Get(), task.Get());
        if (FAILED(presentationHr)) { hr = presentationHr; break; }
    }
    // Remove group-item presentation while the source/button group still
    // exists. Removing the last real task first makes native inclusion return
    // a no-op for the group item, leaving its separate XAML representation.
    if (SUCCEEDED(hr)) hr = RefreshMatchedTask(action == kRestoreTask ? 0 : action,
        band, list, window, group.Get(), item.Get(), inclusion, activate, updateIcon);
    if (action == kRestoreTask && FAILED(hr)) {
        static std::atomic<unsigned> reports = 0;
        if (reports.fetch_add(1) < 16)
            Wh_Log(L"phase=taskbar restore-failed stage=inclusion hr=%08X", hr);
    }
    return hr;
}

LRESULT CALLBACK TaskListSubclass(HWND window, UINT message, WPARAM wParam, LPARAM lParam,
                                   DWORD_PTR) {
    if (g_refreshTask && message == g_refreshTask) {
        HWND member = reinterpret_cast<HWND>(lParam);
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        HWND bandWindow = reinterpret_cast<HWND>(GetPropW(tray, L"TaskbandHWND"));
        void* band = bandWindow ? reinterpret_cast<void*>(GetWindowLongPtrW(bandWindow, 0)) : nullptr;
        void* object = reinterpret_cast<void*>(GetWindowLongPtrW(window, 0));
        void* list = FindTaskListInterface(object, g_taskListVtable);
        InclusionChanged inclusion = InclusionChangedHook;
        if (!list) {
            list = FindTaskListInterface(object, g_multiTaskListVtable);
            inclusion = MultiInclusionChangedHook;
        }
        if (!band || !list || !inclusion) return E_NOINTERFACE;
        auto activate = inclusion == MultiInclusionChangedHook ? g_multiActivateTask : g_activateTask;
        return RefreshTaskWindow(wParam, band, list, member, g_matchTaskWindow,
                                 inclusion, activate, g_updateTaskIcon);
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

bool HookTaskbarFilter() {
    StartTaskbarTrace();
    g_refreshTask = RegisterWindowMessageW(L"Windhawk.HideSnapped.RefreshTaskInclusion");
    if (!g_refreshTask) return false;
    HMODULE module = LoadLibraryExW(L"Taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;
    // This numeric return address identifies a diagnostic caller only. A new
    // image disables that observation, never the functional taskbar hooks.
    g_nativeAppIdReturn = g_binaryTraceEnabled &&
        CheckedNativeImage(L"Taskbar.dll", 0x4059c321, 0x30d000) ?
        reinterpret_cast<uintptr_t>(module) + 0x28a82 : 0;
    // Taskbar.dll
    WindhawkUtils::SYMBOL_HOOK taskbarHooks[] = {
        {{L"?IsTaskAllowed@TaskItemFilter@@UEAA_NPEAUITaskItem@@@Z"}, &g_isTaskAllowed, IsTaskAllowedHook},
        {{L"?IsTaskAllowed@CTaskListWnd@@UEAA_NPEAUITaskItem@@@Z"}, &g_listIsTaskAllowed},
        {{L"?_MatchWindow@CTaskBand@@IEAAJPEAUHWND__@@PEBU_ITEMIDLIST_ABSOLUTE@@PEBGW4WINDOWMATCHCONFIDENCE@@PEAPEAUITaskGroup@@PEAPEAUITaskItem@@@Z"}, &g_matchTaskWindow},
        {{L"?TaskInclusionChanged@CTaskListWnd@@UEAAJPEAUITaskGroup@@PEAUITaskItem@@@Z"}, &g_inclusionChanged, InclusionChangedHook},
        {{L"?TaskInclusionChanged@CTaskListWndMulti@@UEAAJPEAUITaskGroup@@PEAUITaskItem@@@Z"}, &g_multiInclusionChanged, MultiInclusionChangedHook},
        {{L"?_TaskCreated@CTaskListWnd@@IEAAJPEAUITaskGroup@@PEAUITaskItem@@H@Z"}, &g_taskCreated, TaskCreatedHook},
        {{L"??_7CTaskListWnd@@6BITaskListUI@@@"}, &g_taskListVtable},
        {{L"??_7CTaskListWndMulti@@6BITaskListUI@@@"}, &g_multiTaskListVtable},
        {{L"?ActivateTask@CTaskListWnd@@UEAAJPEAUITaskGroup@@PEAUITaskItem@@@Z"}, &g_activateTask, ActivateTaskHook},
        {{L"?ActivateTask@CTaskListWndMulti@@UEAAJPEAUITaskGroup@@PEAUITaskItem@@@Z"}, &g_multiActivateTask, MultiActivateTaskHook},
        {{L"?ShouldAddWindowToTrayHelper@@YAHPEAUHWND__@@@Z"}, &g_shouldAddWindow, ShouldAddWindowHook},
        {{L"?HandleClick@CTaskListWnd@@UEAAJPEAUITaskGroup@@PEAUITaskItem@@AEBULauncherOptions@System@Windows@winrt@@@Z"}, &g_handleTaskClick, HandleTaskClickHook},
        {{L"?HandleClick@CTaskListWndMulti@@UEAAJPEAUITaskGroup@@PEAUITaskItem@@AEBULauncherOptions@System@Windows@winrt@@@Z"}, &g_multiHandleTaskClick, MultiHandleTaskClickHook},
        {{L"?_CreateWindowVisual@CTaskThumbnail@@AEAAJPEAUHWND__@@UtagSIZE@@@Z"}, &g_createWindowVisual, CreateWindowVisualHook, true},
        {{L"?_HandleClick@CTaskListWnd@@IEAAXPEAUITaskBtnGroup@@HW4eCLICKACTION@1@HH@Z"}, &g_handleButtonClick, HandleButtonClickHook},
        {{L"?UpdateThumbnailFactory@TaskItemThumbnail@implementation@Shell@UI@WindowsUdk@winrt@@QEAAXXZ"}, &g_updateThumbnailFactory, UpdateThumbnailFactoryHook, true},
        {{L"?GetIcon@CTaskListWnd@@UEAAJPEAUITaskGroup@@PEAUITaskItem@@PEAPEAUHICON__@@@Z"}, &g_getTaskIcon, GetTaskIconHook},
        {{L"?_UpdateItemIcon@CTaskBand@@IEAAXPEAUITaskGroup@@PEAUITaskItem@@@Z"}, &g_updateTaskIcon},
        {{L"?GetAppID@CTaskGroup@@UEAAPEBGXZ"}, &g_nativeAppId, g_binaryTraceEnabled ? NativeAppIdTraceHook : nullptr},
        {{L"?GetNumItems@CTaskGroup@@UEAAHXZ"}, &g_nativeItemCount, nullptr, true},
        {{L"??_7CTaskGroup@@6BITaskGroup@@@"}, &g_nativeGroupVtable},
        // Optional trace-only hooks must remain the last five entries.
        {{L"?_CreateTBGroup@CTaskListWnd@@IEAAPEAUITaskBtnGroup@@PEAUITaskGroup@@H@Z"}, &g_createButton, CreateButtonTraceHook, true},
        {{L"?NotifyTaskGroupInserted@CTaskListWnd@@IEAAXPEAUITaskGroup@@H@Z"}, &g_notifyInserted, NotifyInsertedTraceHook, true},
        {{L"?InsertGroup@TaskbarAppsList@implementation@Shell@UI@WindowsUdk@winrt@@QEAAXAEBV?$com_ptr_t@UITaskGroup@@Uerr_exception_policy@wil@@@wil@@I@Z"}, &g_insertSourceGroup, InsertSourceTraceHook, true},
        {{L"?RemoveGroup@TaskbarAppsList@implementation@Shell@UI@WindowsUdk@winrt@@QEAAXAEBV?$com_ptr_t@UITaskGroup@@Uerr_exception_policy@wil@@@wil@@@Z"}, &g_removeSourceGroup, RemoveSourceTraceHook, true},
        {{L"?MoveGroup@TaskbarAppsList@implementation@Shell@UI@WindowsUdk@winrt@@QEAAXAEBV?$com_ptr_t@UITaskGroup@@Uerr_exception_policy@wil@@@wil@@I@Z"}, &g_moveSourceGroup, MoveSourceTraceHook, true},
    };
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE;
    constexpr size_t kTraceHookCount = 5;
    size_t count = ARRAYSIZE(taskbarHooks) - (g_binaryTraceEnabled ? 0 : kTraceHookCount);
    bool hooked = WindhawkUtils::HookSymbols(module, taskbarHooks, count, &options);
    g_nativeItemsOffset = DecodeNativeItemsOffset(reinterpret_cast<void*>(g_nativeItemCount));
    Wh_Log(L"phase=taskbar native-items-layout=%zu", g_nativeItemsOffset);
    g_nativePreviewAvailable = g_nativePreviewAvailable && g_createWindowVisual && g_updateThumbnailFactory;
    if (hooked) hooked = WindhawkUtils::SetFunctionHook(MonitorFromWindow,
        MonitorFromWindowHook, &g_monitorFromWindow);
    FreeLibrary(module);
    g_taskbarFilterReady = hooked;
    Wh_Log(L"phase=taskbar inclusion-filter symbols=%d", hooked);
    return hooked;
}
WindowIdentity IdentifyWindow(HWND window) {
    WindowIdentity result{window};
    result.thread = GetWindowThreadProcessId(window, &result.process);
    return result;
}
bool StillExists(const WindowIdentity& member) {
    return member.thread && IsWindow(member.window) && IdentifyWindow(member.window) == member;
}
// One writer: the taskbar worker owns proxy creation, updates and retirement.
bool PublishProxyMonitor(const WindowIdentity& proxy, HMONITOR monitor) {
    auto old = g_proxyMonitors.load();
    auto found = old ? old->find(proxy) : ProxyMonitors::const_iterator{};
    if (old && found != old->end() && found->second == monitor) return false;
    if (!monitor && (!old || found == old->end())) return false;
    auto next = old ? std::make_shared<ProxyMonitors>(*old) : std::make_shared<ProxyMonitors>();
    if (monitor) (*next)[proxy] = monitor;
    else next->erase(proxy);
    g_proxyMonitors.store(std::move(next));
    return true;
}
HMONITOR WINAPI MonitorFromWindowHook(HWND window, DWORD flags) {
    HMONITOR result = g_monitorFromWindow(window, flags);
    DWORD error = GetLastError();
    auto monitors = g_proxyMonitors.load();
    if (monitors && flags <= MONITOR_DEFAULTTONEAREST) {
        auto found = monitors->lower_bound(WindowIdentity{window});
        MONITORINFO info{sizeof(info)};
        if (found != monitors->end() && found->first.window == window &&
                found->first.thread && found->first == IdentifyWindow(window) &&
                GetMonitorInfoW(found->second, &info)) result = found->second;
    }
    SetLastError(error);
    return result;
}
bool TaskbarCandidate(HWND window) {
    if (!IsWindowVisible(window) || GetAncestor(window, GA_ROOT) != window) return false;
    auto style = GetWindowLongPtrW(window, GWL_EXSTYLE);
    if (style & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;
    if (GetWindow(window, GW_OWNER) && !(style & WS_EX_APPWINDOW)) return false;
    return true;
}

using GroupKey = std::vector<WindowIdentity>;
struct GroupSnapshot {
    GroupKey key;  // sorted identity, independent of the native member ordering
    std::vector<WindowIdentity> members;  // native layout order for icons/actions
    ComPtr<IUnknown> nativeGroup;
    std::wstring title;
};

bool WindowIconRect(HWND window, RECT& rect) {
    if (IsIconic(window)) {
        WINDOWPLACEMENT placement{sizeof(placement)};
        MONITORINFO monitor{sizeof(monitor)};
        if (!GetWindowPlacement(window, &placement) ||
            !GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor)) return false;
        rect = placement.rcNormalPosition;
        // Normal placement uses workspace coordinates for ordinary app windows.
        if (!(GetWindowLongPtrW(window, GWL_EXSTYLE) & WS_EX_TOOLWINDOW))
            OffsetRect(&rect, monitor.rcWork.left - monitor.rcMonitor.left,
                       monitor.rcWork.top - monitor.rcMonitor.top);
    } else if (!GetWindowRect(window, &rect)) return false;
    return rect.right > rect.left && rect.bottom > rect.top;
}

bool CachedIconRect(const WindowIdentity& member, bool minimized,
        std::map<WindowIdentity, RECT>& cache, RECT& rect,
        bool (*geometry)(HWND, RECT&) = WindowIconRect) {
    auto found = cache.find(member);
    if (minimized && found != cache.end()) { rect = found->second; return true; }
    if (!geometry(member.window, rect)) return false;
    cache[member] = rect;
    return true;
}

template <typename Geometry = decltype(&WindowIconRect)>
std::vector<WindowIdentity> OrderedIconMembers(const std::vector<WindowIdentity>& members,
        bool screenOrder, Geometry geometry = WindowIconRect) {
    if (!screenOrder) return members;
    struct PositionedMember { WindowIdentity member; RECT rect; int64_t x; };
    std::vector<PositionedMember> positioned;
    for (const auto& member : members) {
        PositionedMember value{member, {}, 0};
        // An incomplete layout must not mix unknown coordinates into the sort.
        if (!geometry(member.window, value.rect) || value.rect.right <= value.rect.left ||
            value.rect.bottom <= value.rect.top) return members;
        // Twice the center preserves half-pixel ties and avoids LONG overflow.
        value.x = int64_t(value.rect.left) + value.rect.right;
        positioned.push_back(value);
    }
    std::stable_sort(positioned.begin(), positioned.end(), [](const auto& a, const auto& b) {
        return a.rect.top != b.rect.top ? a.rect.top < b.rect.top : a.x < b.x;
    });
    std::vector<WindowIdentity> result;
    while (!positioned.empty()) {
        std::vector<PositionedMember> row{positioned.front()};
        int64_t top = row.front().rect.top, bottom = row.front().rect.bottom;
        int64_t minHeight = bottom - top;
        positioned.erase(positioned.begin());
        for (auto it = positioned.begin(); it != positioned.end();) {
            int64_t overlapTop = std::max(top, int64_t(it->rect.top));
            int64_t overlapBottom = std::min(bottom, int64_t(it->rect.bottom));
            int64_t height = std::min(minHeight, int64_t(it->rect.bottom) - it->rect.top);
            // Common overlap must cover at least half the shortest window.
            // Intersecting the entire row prevents a tall window bridging rows.
            if (overlapBottom > overlapTop && 2 * (overlapBottom - overlapTop) >= height) {
                top = overlapTop; bottom = overlapBottom; minHeight = height;
                row.push_back(*it);
                it = positioned.erase(it);
            } else ++it;
        }
        std::stable_sort(row.begin(), row.end(), [](const auto& a, const auto& b) { return a.x < b.x; });
        for (const auto& value : row) result.push_back(value.member);
    }
    return result;
}

HRESULT ReadGroupTitle(IUnknown* group, std::wstring& title) {
    if (!group) return E_POINTER;
    HSTRING text = nullptr;
    using GetTitle = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, HSTRING*);
    HRESULT hr = AbiMethod<GetTitle>(group, 10)(group, &text);
    if (SUCCEEDED(hr)) {
        UINT32 length = 0;
        const wchar_t* buffer = WindowsGetStringRawBuffer(text, &length);
        title.assign(buffer, length);
    }
    WindowsDeleteString(text);
    return hr;
}

struct IconCell { int x, y, size; };
std::vector<IconCell> IconLayout(size_t memberCount, const TaskbarSettings& settings) {
    int count = static_cast<int>(std::min(memberCount, size_t(settings.maxIcons)));
    if (count < 1) return {};
    const int rows = (count + 1) / 2;
    const int gap = std::clamp(settings.gap, 0, 8);
    const int size = std::clamp(settings.iconSize, 4, (32 - gap) / 2);
    const int top = (32 - (rows * size + (rows - 1) * gap)) / 2;
    std::vector<IconCell> cells;
    for (int i = 0; i < count; ++i) {
        int x = (32 - (2 * size + gap)) / 2 + (i % 2) * (size + gap);
        if (settings.centerLastRow && (count % 2) && i == count - 1) x = (32 - size) / 2;
        cells.push_back({x, top + (i / 2) * (size + gap), size});
    }
    return cells;
}

struct NativeSize { float width, height; };
class NativeGroupPreview {
public:
    ComPtr<IUnknown> device, factory;
    int width = 480, height = 270;

    HRESULT Initialize(IUnknown* group) {
        if (!g_nativePreviewAvailable) return E_NOINTERFACE;
        // UI hover uses this group on another apartment. Only expose groups
        // whose COM contract explicitly allows direct cross-apartment use.
        ComPtr<IAgileObject> agile;
        if (!group || FAILED(group->QueryInterface(IID_PPV_ARGS(&agile)))) return E_NOINTERFACE;
        constexpr wchar_t className[] = L"Windows.Internal.Shell.Multitasking.Desktop.DesktopThumbnailDevice";
        HSTRING name = nullptr;
        HRESULT hr = WindowsCreateString(className, ARRAYSIZE(className) - 1, &name);
        if (FAILED(hr)) return hr;
        ComPtr<IInspectable> activated;
        hr = RoActivateInstance(name, activated.GetAddressOf());
        WindowsDeleteString(name);
        if (FAILED(hr)) return hr;
        constexpr GUID iid = {0x096a23dc, 0xa2b9, 0x45f2, {0x9a,0xd3,0xcc,0x6f,0x69,0xdd,0xd7,0x00}};
        hr = activated->QueryInterface(iid, reinterpret_cast<void**>(device.GetAddressOf()));
        if (FAILED(hr)) return hr;
        using CreateFactory = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, IUnknown*, IUnknown**);
        hr = AbiMethod<CreateFactory>(device.Get(), 11)(device.Get(), group, factory.GetAddressOf());
        if (FAILED(hr) || !factory) return FAILED(hr) ? hr : E_UNEXPECTED;
        NativeSize source{};
        using GetSize = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, NativeSize*);
        hr = AbiMethod<GetSize>(factory.Get(), 6)(factory.Get(), &source);
        if (FAILED(hr) || source.width <= 0 || source.height <= 0) return FAILED(hr) ? hr : E_UNEXPECTED;
        float scale = std::min(480.0f / source.width, 300.0f / source.height);
        width = std::max(1, static_cast<int>(source.width * scale));
        height = std::max(1, static_cast<int>(source.height * scale));
        return S_OK;
    }
};

// Inspected Taskbar.dll CTaskThumbnail layout, relative to _CreateWindowVisual's
// concrete this pointer. Native destructor/unregistration owns these fields.
struct NativeTaskThumbnail {
    unsigned char prefix[104];
    IUnknown* host;
    unsigned char middle[80];
    IUnknown* wrappedVisual;
    void* reserved;
    IUnknown* duplicateVisual;
};
static_assert(offsetof(NativeTaskThumbnail, wrappedVisual) == 192);
static_assert(offsetof(NativeTaskThumbnail, duplicateVisual) == 208);
struct NativeThumbnailProperties { uint32_t words[3]{}; };
// Modern WindowsUdk hover bypasses CTaskThumbnail entirely. Its native factory
// owns source sizing and visual creation on the compositor supplied by XAML.
struct NativeModernThumbnail {
    unsigned char prefix[40];
    IUnknown* item;
    unsigned char middle[72];
    IUnknown* factory;
};
static_assert(offsetof(NativeModernThumbnail, factory) == 120);
ComPtr<IUnknown> ProxyPreviewGroup(IUnknown* item) {
    using GetWindow = HWND(WINAPI*)(IUnknown*);
    HWND window = item ? AbiMethod<GetWindow>(item, 12)(item) : nullptr;
    std::lock_guard lock(g_hiddenMembersMutex);
    auto found = g_proxyGroups->find(IdentifyWindow(window));
    return found == g_proxyGroups->end() ? ComPtr<IUnknown>{} : found->second;
}
void ReplaceThumbnailFactory(NativeModernThumbnail& thumbnail, ComPtr<IUnknown> factory) {
    IUnknown* old = thumbnail.factory;
    thumbnail.factory = factory.Detach();
    if (old) old->Release();
}
HRESULT CreateGroupPreviewFactory(IUnknown* group, IUnknown** factory) {
    NativeGroupPreview preview;
    HRESULT hr = preview.Initialize(group);
    if (SUCCEEDED(hr)) *factory = preview.factory.Detach();
    return hr;
}
void RouteThumbnailFactory(UpdateThumbnailFactory original, void* self,
                            HRESULT (*create)(IUnknown*, IUnknown**) = CreateGroupPreviewFactory) {
    original(self);
    if (!g_nativePreviewAvailable) return;
    { std::lock_guard lock(g_hiddenMembersMutex);
      if (g_proxyGroups->empty()) return; }
    auto& thumbnail = *static_cast<NativeModernThumbnail*>(self);
    auto group = ProxyPreviewGroup(thumbnail.item);
    if (!group) return;
    ComPtr<IUnknown> factory;
    HRESULT hr = create(group.Get(), factory.GetAddressOf());
    if (SUCCEEDED(hr) && !factory) hr = E_UNEXPECTED;
    if (SUCCEEDED(hr)) ReplaceThumbnailFactory(thumbnail, std::move(factory));
    Wh_Log(L"phase=taskbar hover-factory source=modern hr=%08X", hr);
}
void WINAPI UpdateThumbnailFactoryHook(void* self) {
    RouteThumbnailFactory(g_updateThumbnailFactory, self);
}
HRESULT AttachGroupHoverVisual(NativeTaskThumbnail& thumbnail, IUnknown* factory, SIZE available) {
    if (!thumbnail.host || !factory || available.cx <= 0 || available.cy <= 0) return E_INVALIDARG;
    using GetObject = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, IUnknown**);
    ComPtr<IUnknown> compositor, wrapperFactory;
    HRESULT hr = AbiMethod<GetObject>(thumbnail.host, 10)(thumbnail.host, compositor.GetAddressOf());
    if (FAILED(hr)) return hr;
    hr = AbiMethod<GetObject>(thumbnail.host, 11)(thumbnail.host, wrapperFactory.GetAddressOf());
    if (FAILED(hr)) return hr;
    if (!compositor || !wrapperFactory) return E_UNEXPECTED;
    NativeSize source{};
    using GetSize = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, NativeSize*);
    hr = AbiMethod<GetSize>(factory, 6)(factory, &source);
    if (FAILED(hr)) return hr;
    if (!(source.width > 0 && source.height > 0)) return E_UNEXPECTED;
    float scale = std::min(available.cx / source.width, available.cy / source.height);
    NativeSize size{source.width * scale, source.height * scale};
    NativeThumbnailProperties properties{};
    ComPtr<IUnknown> duplicate, visual, dcompVisual, wrapped;
    using CreateVisual = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, IUnknown*, NativeSize,
                                                    const NativeThumbnailProperties*, IUnknown**);
    hr = AbiMethod<CreateVisual>(factory, 8)(factory, compositor.Get(), size, &properties,
                                           duplicate.GetAddressOf());
    if (FAILED(hr) || !duplicate) return FAILED(hr) ? hr : E_UNEXPECTED;
    hr = AbiMethod<GetObject>(duplicate.Get(), 6)(duplicate.Get(), visual.GetAddressOf());
    if (FAILED(hr) || !visual) return FAILED(hr) ? hr : E_UNEXPECTED;
    constexpr GUID dcompVisualIid = {0x4d93059d,0x097b,0x4651,{0x9a,0x60,0xf0,0xf2,0x51,0x16,0xe2,0xf3}};
    hr = visual->QueryInterface(dcompVisualIid, reinterpret_cast<void**>(dcompVisual.GetAddressOf()));
    if (FAILED(hr) || !dcompVisual) return FAILED(hr) ? hr : E_UNEXPECTED;
    constexpr GUID wrappedVisualIid = {0x47267895,0xa4f8,0x4012,{0x9e,0x98,0x08,0x7c,0x61,0x9e,0x7c,0xfc}};
    using WrapVisual = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, DWORD, IUnknown*, SIZE, REFIID, IUnknown**);
    SIZE pixels{std::max(1L, static_cast<LONG>(size.width)), std::max(1L, static_cast<LONG>(size.height))};
    hr = AbiMethod<WrapVisual>(wrapperFactory.Get(), 15)(wrapperFactory.Get(), 0, dcompVisual.Get(),
                pixels, wrappedVisualIid, wrapped.GetAddressOf());
    if (FAILED(hr) || !wrapped) return FAILED(hr) ? hr : E_UNEXPECTED;
    // Publish only a complete pair; native cleanup releases it on the UI thread.
    if (thumbnail.wrappedVisual) thumbnail.wrappedVisual->Release();
    if (thumbnail.duplicateVisual) thumbnail.duplicateVisual->Release();
    thumbnail.wrappedVisual = wrapped.Detach();
    thumbnail.duplicateVisual = duplicate.Detach();
    return S_OK;
}
HRESULT WINAPI CreateWindowVisualHook(void* self, HWND window, SIZE size) {
    if (!g_nativePreviewAvailable) return g_createWindowVisual(self, window, size);
    ComPtr<IUnknown> group;
    { std::lock_guard lock(g_hiddenMembersMutex);
      auto found = g_proxyGroups->find(IdentifyWindow(window));
      if (found != g_proxyGroups->end()) group = found->second; }
    if (!group) return g_createWindowVisual(self, window, size);
    NativeGroupPreview preview;
    HRESULT hr = preview.Initialize(group.Get());
    if (SUCCEEDED(hr)) hr = AttachGroupHoverVisual(*static_cast<NativeTaskThumbnail*>(self), preview.factory.Get(), size);
    Wh_Log(L"phase=taskbar hover-visual hr=%08X", hr);
    return hr;
}

struct GroupButton {
    HWND window{};
    HICON icon{};
    std::vector<WindowIdentity> members;
    std::vector<WindowIdentity> iconMembers;
    std::map<WindowIdentity, RECT> iconRects;
    HWND activationSource{};
    ULONGLONG iconTime{};
    bool iconRedrawPending{};
    std::wstring title, appliedTitle;
    bool previewDirty = true;
    bool registered = false;
    bool registrationLost = false;
    bool registrationPending = false;
    bool monitorRefreshPending = false;
    HRESULT registrationResult = S_OK;
    bool activating = false;
    bool activationPending = false;
    bool nativeClickPending = false;
    TaskbarSettings settings;
    ComPtr<IUnknown> nativeGroup;
    std::unique_ptr<NativeGroupPreview> preview;
};

// Keep the lifecycle policy testable without a live taskbar. Failed button
// creation never hides members; restore members BEFORE removing old proxies.
struct TaskbarOperations {
    virtual bool Create(GroupButton&) = 0;
    virtual void Update(GroupButton&) = 0;
    virtual HRESULT Hide(const WindowIdentity&) = 0;
    virtual HRESULT Restore(const WindowIdentity&) = 0;
    virtual void Destroy(GroupButton&) = 0;
    virtual ~TaskbarOperations() = default;
};
class TaskbarState {
public:
    std::map<GroupKey, std::unique_ptr<GroupButton>> buttons;
    std::set<WindowIdentity> hidden;

    bool ApplySettings(const TaskbarSettings& settings, TaskbarOperations& operations) {
        // Only changes to proxy identity or preview resources need a new HWND.
        // Click policy/layout/interval saves must not restore/delete/recreate tasks.
        for (const auto& [key, button] : buttons) {
            if (button->settings.separateGroups != settings.separateGroups ||
                button->settings.nativePreview != settings.nativePreview) {
                Stop(operations);
                return true;
            }
        }
        for (auto& [key, button] : buttons) {
            const auto& old = button->settings;
            if (old.maxIcons != settings.maxIcons || old.iconSize != settings.iconSize ||
                old.gap != settings.gap || old.centerLastRow != settings.centerLastRow ||
                old.screenIconOrder != settings.screenIconOrder) button->iconTime = 0;
            button->settings = settings;
        }
        return false;
    }

    void Reconcile(const std::vector<GroupSnapshot>& snapshots,
                   const TaskbarSettings& settings,
                   TaskbarOperations& operations, bool snapshotComplete = true) {
        if (settings.enabled && !snapshotComplete) return;
        std::set<GroupKey> retained;
        std::set<WindowIdentity> desiredHidden;
        if (settings.enabled) for (const auto& snapshot : snapshots)
            if (snapshot.members.size() >= 2) retained.insert(snapshot.key);
        if (settings.enabled) for (const auto& snapshot : snapshots) {
            if (snapshot.members.size() < 2) continue;
            // Replacing a native group often shares windows with the old one.
            // Transfer its proxy instead of asynchronously deleting a task that
            // the shell may still be rendering. Never steal a surviving group.
            if (!buttons.contains(snapshot.key)) {
                auto replacement = buttons.end();
                size_t bestOverlap = 0;
                for (auto it = buttons.begin(); it != buttons.end(); ++it) {
                    if (retained.contains(it->first) || it->second->activationPending) continue;
                    size_t overlap = 0;
                    for (const auto& member : snapshot.members)
                        overlap += std::find(it->second->members.begin(), it->second->members.end(), member)
                            != it->second->members.end();
                    if (overlap > bestOverlap) { bestOverlap = overlap; replacement = it; }
                }
                if (replacement != buttons.end()) {
                    auto node = buttons.extract(replacement);
                    node.key() = snapshot.key;
                    buttons.insert(std::move(node));
                    Wh_Log(L"phase=taskbar group-replaced reused-proxy=1 shared=%u", unsigned(bestOverlap));
                }
            }
            auto& button = buttons[snapshot.key];
            if (!button) {
                button = std::make_unique<GroupButton>();
                button->members = snapshot.members;
                button->settings = settings;
                button->nativeGroup = snapshot.nativeGroup;
                button->title = snapshot.title;
                if (!operations.Create(*button)) {
                    operations.Destroy(*button);
                    buttons.erase(snapshot.key);
                    continue;
                }
            }
            if (button->members != snapshot.members || button->nativeGroup.Get() != snapshot.nativeGroup.Get()) {
                button->iconTime = 0;
                button->previewDirty = true;
                button->activationSource = nullptr;
            }
            button->members = snapshot.members;
            button->settings = settings;
            button->nativeGroup = snapshot.nativeGroup;
            if (!snapshot.title.empty()) button->title = snapshot.title;
            operations.Update(*button);
            if (button->registered && settings.hideMembers) {
                desiredHidden.insert(snapshot.members.begin(), snapshot.members.end());
            }
        }
        for (auto it = hidden.begin(); it != hidden.end();) {
            if (!desiredHidden.contains(*it)) {
                if (SUCCEEDED(operations.Restore(*it))) it = hidden.erase(it);
                else ++it; // keep recovery state for the next tick
            } else ++it;
        }
        for (const auto& member : desiredHidden) {
            // Hide is idempotent; unchanged members need no repeated refresh.
            if (SUCCEEDED(operations.Hide(member))) hidden.insert(member);
        }
        for (auto it = buttons.begin(); it != buttons.end();) {
            if (retained.contains(it->first)) { ++it; continue; }
            bool needsRecovery = false;
            for (const auto& member : it->second->members)
                needsRecovery |= hidden.contains(member) && !desiredHidden.contains(member);
            if (needsRecovery) { ++it; continue; }
            operations.Destroy(*it->second);
            it = buttons.erase(it);
        }
    }
    void Stop(TaskbarOperations& operations) {
        // A busy taskbar may time out the first inclusion request. Retry only
        // failed members before retiring their proxies; never block forever.
        for (unsigned attempt = 0; attempt < 3 && !hidden.empty(); ++attempt) {
            for (auto it = hidden.begin(); it != hidden.end();) {
                if (SUCCEEDED(operations.Restore(*it))) it = hidden.erase(it);
                else ++it;
            }
        }
        if (!hidden.empty()) Wh_Log(L"phase=taskbar cleanup restore-failures=%u", unsigned(hidden.size()));
        hidden.clear();
        for (auto& [key, button] : buttons) operations.Destroy(*button);
        buttons.clear();
    }
    void Forget(HWND destroyed) {
        // Destroy events distinguish a recycled HWND even inside the same process.
        std::erase_if(hidden, [destroyed](const auto& item) { return item.window == destroyed; });
        std::lock_guard lock(g_hiddenMembersMutex);
        std::erase_if(g_hiddenMembers, [destroyed](const auto& item) { return item.window == destroyed; });
        std::erase_if(g_groupButtons, [destroyed](const auto& item) { return item.first.window == destroyed; });
        if (g_lastAppForeground.window == destroyed) g_lastAppForeground = {};
    }
};

HICON WindowIcon(HWND window) {
    DWORD_PTR value = 0;
    SendMessageTimeoutW(window, WM_GETICON, ICON_SMALL2, 0,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 25, &value);
    HICON icon = reinterpret_cast<HICON>(value);
    if (!icon) icon = reinterpret_cast<HICON>(GetClassLongPtrW(window, GCLP_HICONSM));
    if (!icon) icon = reinterpret_cast<HICON>(GetClassLongPtrW(window, GCLP_HICON));
    if (!icon) icon = LoadIconW(nullptr, IDI_APPLICATION);
    return CopyIcon(icon);
}
HICON ComposeGroupIcon(const std::vector<WindowIdentity>& members,
                       const TaskbarSettings& settings,
                       HICON (*iconSource)(HWND) = WindowIcon) {
    // Render at 2x and let the taskbar scale the icon for its current DPI.
    constexpr int side = 64;
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER), side, -side, 1, 32, BI_RGB};
    void* pixels = nullptr;
    HBITMAP color = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
    HBITMAP mask = CreateBitmap(side, side, 1, 1, nullptr);
    HDC dc = CreateCompatibleDC(nullptr);
    if (!color || !mask || !dc) {
        if (dc) DeleteDC(dc);
        if (mask) DeleteObject(mask);
        if (color) DeleteObject(color);
        return nullptr;
    }
    memset(pixels, 0, side * side * 4);
    auto old = SelectObject(dc, color);
    auto cells = IconLayout(members.size(), settings);
    for (size_t i = 0; i < cells.size(); ++i) {
        HICON icon = iconSource(members[i].window);
        if (icon) {
            const auto& cell = cells[i];
            DrawIconEx(dc, cell.x * 2, cell.y * 2, icon, cell.size * 2,
                       cell.size * 2, 0, nullptr, DI_NORMAL);
            GdiFlush(); // synchronize GDI writes before direct DIB alpha access
            auto data = static_cast<uint32_t*>(pixels);
            bool hasAlpha = false;
            for (int y = cell.y * 2; y < (cell.y + cell.size) * 2; ++y)
                for (int x = cell.x * 2; x < (cell.x + cell.size) * 2; ++x)
                    hasAlpha |= (data[y * side + x] & 0xff000000) != 0;
            if (!hasAlpha) {
                // Classic bitmap/monochrome icons use an AND mask, not alpha.
                // Preserve opaque black pixels too; RGB alone cannot identify them.
                void* maskPixels = nullptr;
                HBITMAP coverage = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS,
                                                     &maskPixels, nullptr, 0);
                if (coverage) {
                    memset(maskPixels, 0xff, side * side * 4);
                    SelectObject(dc, coverage);
                    DrawIconEx(dc, cell.x * 2, cell.y * 2, icon, cell.size * 2,
                               cell.size * 2, 0, nullptr, DI_MASK);
                    GdiFlush();
                    SelectObject(dc, color);
                    auto coverageData = static_cast<uint32_t*>(maskPixels);
                    for (int y = cell.y * 2; y < (cell.y + cell.size) * 2; ++y)
                        for (int x = cell.x * 2; x < (cell.x + cell.size) * 2; ++x)
                            if (!(coverageData[y * side + x] & 0xffffff))
                                data[y * side + x] |= 0xff000000;
                    DeleteObject(coverage);
                }
            }
            DestroyIcon(icon);
        }
    }
    SelectObject(dc, old);
    ICONINFO iconInfo{TRUE, 0, 0, mask, color};
    HICON result = CreateIconIndirect(&iconInfo);
    DeleteDC(dc);
    DeleteObject(mask);
    DeleteObject(color);
    return result;
}

HRESULT NativeGroupCommand(IUnknown* group, bool minimize) {
    if (!group) return E_POINTER;
    using Command = HRESULT(STDMETHODCALLTYPE*)(IUnknown*);
    return AbiMethod<Command>(group, minimize ? 18 : 16)(group);
}
bool ShouldMinimizeGroup(const TaskbarSettings& settings, bool forceRestore,
                         bool activeMember, bool anyMinimized) {
    return settings.toggleOnClick && !forceRestore && activeMember && !anyMinimized;
}
bool ActiveGroupSource(const std::vector<WindowIdentity>& members, WindowIdentity source,
                        bool transientSource, WindowIdentity remembered) {
    // Taskbar/proxy activation is an intermediate focus transfer. Preserve the
    // last application, including an unrelated app; never fall back merely
    // because the currently focused application is outside this group.
    if (transientSource) source = remembered;
    return source.window && std::find(members.begin(), members.end(), source) != members.end();
}
void ActivateGroup(GroupButton& button, bool forceRestore = false) {
    if (button.activating) return;
    button.activating = true;
    HWND foreground = GetForegroundWindow();
    if (button.nativeClickPending || foreground == button.window) foreground = button.activationSource;
    bool transientSource = !foreground || IsProxyWindow(foreground) || IsTaskbarSurface(foreground);
    if (!transientSource) RememberAppForeground(foreground);
    WindowIdentity remembered;
    { std::lock_guard lock(g_hiddenMembersMutex); remembered = g_lastAppForeground; }
    if (!StillExists(remembered)) remembered = {};
    bool activeMember = ActiveGroupSource(button.members, IdentifyWindow(foreground), transientSource, remembered);
    bool anyMinimized = false;
    for (const auto& member : button.members) {
        if (!StillExists(member)) continue;
        anyMinimized |= IsIconic(member.window) != FALSE;
    }
    // A partially minimized group is restored, never minimized further.
    bool minimize = ShouldMinimizeGroup(button.settings, forceRestore, activeMember, anyMinimized);
    HRESULT hr = NativeGroupCommand(button.nativeGroup.Get(), minimize);
    Wh_Log(L"phase=taskbar group-action minimize=%d toggle=%d active=%d iconic=%d remembered=%d hr=%08X",
           minimize, button.settings.toggleOnClick, activeMember, anyMinimized, transientSource, hr);
    button.activating = false;
}
void MinimizeGroup(const GroupButton& button) {
    HRESULT hr = NativeGroupCommand(button.nativeGroup.Get(), true);
    Wh_Log(L"phase=taskbar group-minimize hr=%08X", hr);
}
constexpr UINT kInvokeGroup = WM_APP + 18;
void QueueGroupClick(GroupButton& button) {
    if (button.activationPending) return;
    button.activationPending = true;
    if (!PostMessageW(button.window, kInvokeGroup, 0, 0)) button.activationPending = false;
}
LRESULT CALLBACK GroupButtonProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    auto button = reinterpret_cast<GroupButton*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        button = static_cast<GroupButton*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(button));
    }
    if (!button) return DefWindowProcW(window, message, wParam, lParam);
    switch (message) {
    case kTaskbarClick:
        button->activationSource = reinterpret_cast<HWND>(lParam);
        button->nativeClickPending = true;
        Wh_Log(L"phase=taskbar click-source=native");
        QueueGroupClick(*button);
        return 0;
    case WM_WINDOWPOSCHANGING: {
        auto position = reinterpret_cast<WINDOWPOS*>(lParam);
        // A taskbar activation must never reveal a blank proxy window.
        position->x = -32000; position->y = -32000;
        position->cx = button->preview ? button->preview->width : 1;
        position->cy = button->preview ? button->preview->height : 1;
        return 0;
    }
    case WM_ACTIVATE:
        if (LOWORD(wParam) != WA_INACTIVE && button->registered) {
            Wh_Log(L"phase=taskbar click-source=window-activate");
            button->activationSource = reinterpret_cast<HWND>(lParam);
            QueueGroupClick(*button);
        }
        return 0;
    case kInvokeGroup:
        button->activationPending = false;
        ActivateGroup(*button);
        button->nativeClickPending = false;
        return 0;
    case WM_SYSCOMMAND:
        switch (wParam & 0xfff0) {
        case SC_RESTORE: case SC_MAXIMIZE:
            Wh_Log(L"phase=taskbar click-source=system-restore");
            if (GetForegroundWindow() != window) {
                button->activationSource = GetForegroundWindow();
                SetForegroundWindow(window);
            }
            // Also queue if foreground transfer was denied; the native group
            // command can report its own result. WM_ACTIVATE coalesces here.
            QueueGroupClick(*button);
            return 0;
        case SC_MINIMIZE:
            Wh_Log(L"phase=taskbar click-source=system-minimize");
            // The taskbar sends SC_MINIMIZE for an active button even if only
            // part of the group is visible. Apply the same group click policy.
            if (GetForegroundWindow() != window) button->activationSource = GetForegroundWindow();
            QueueGroupClick(*button);
            return 0;
        }
        break;
    case WM_CLOSE:
        if (button->settings.minimizeOnClose) MinimizeGroup(*button);
        return 0;
    case WM_CONTEXTMENU: {
        HMENU menu = CreatePopupMenu();
        if (!menu) return 0;
        AppendMenuW(menu, MF_STRING, 1, L"Restore group");
        AppendMenuW(menu, MF_STRING, 2, L"Minimize group");
        POINT cursor; GetCursorPos(&cursor);
        UINT action = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY,
                                     cursor.x, cursor.y, 0, window, nullptr);
        DestroyMenu(menu);
        if (action == 1) ActivateGroup(*button, true);
        if (action == 2) MinimizeGroup(*button);
        return 0;
    }
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

class ShellTaskbarOperations : public TaskbarOperations {
public:
    ComPtr<ITaskbarList> taskbar;
    HINSTANCE instance{};
    std::set<HRESULT> reportedHideErrors;
    std::vector<HWND> subclassedLists;
    bool PrepareLists() {
        struct Discovery { std::vector<HWND> windows; DWORD process; } discovery{{}, GetCurrentProcessId()};
        EnumWindows([](HWND top, LPARAM context) -> BOOL {
            auto data = reinterpret_cast<Discovery*>(context);
            DWORD process = 0;
            GetWindowThreadProcessId(top, &process);
            if (process != data->process) return TRUE;
            EnumChildWindows(top, [](HWND child, LPARAM nested) -> BOOL {
                auto data = reinterpret_cast<Discovery*>(nested);
                wchar_t name[48]{};
                GetClassNameW(child, name, ARRAYSIZE(name));
                if (wcscmp(name, L"MSTaskListWClass") == 0) data->windows.push_back(child);
                return TRUE;
            }, context);
            return TRUE;
        }, reinterpret_cast<LPARAM>(&discovery));
        for (HWND list : discovery.windows) {
            if (std::find(subclassedLists.begin(), subclassedLists.end(), list) != subclassedLists.end()) continue;
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(list, TaskListSubclass, kTaskListSubclass))
                subclassedLists.push_back(list);
        }
        std::erase_if(subclassedLists, [](HWND list) { return !IsWindow(list); });
        return !subclassedLists.empty();
    }
    virtual bool MemberExists(const WindowIdentity& member) { return StillExists(member); }
    virtual HMONITOR GroupMonitor(const GroupButton& button) {
        for (const auto& member : button.members) {
            if (!MemberExists(member)) continue;
            // Windows uses the restore rectangle for minimized windows.
            HMONITOR monitor = g_monitorFromWindow(member.window, MONITOR_DEFAULTTONULL);
            if (monitor) return monitor;
        }
        return nullptr;
    }
    void UpdateProxyMonitor(GroupButton& button) {
        if (PublishProxyMonitor(IdentifyWindow(button.window), GroupMonitor(button)))
            button.monitorRefreshPending = true;
        if (button.registered && button.monitorRefreshPending)
            button.monitorRefreshPending = FAILED(RefreshMember(button.window));
    }
    virtual HRESULT NotifyIconChanged(HWND window) {
        return DispatchTaskRefresh(window, kRefreshTaskIcon);
    }
    virtual HRESULT RefreshMember(HWND member, WPARAM action = 0) {
        return DispatchTaskRefresh(member, action);
    }
    HRESULT DispatchTaskRefresh(HWND member, WPARAM action) {
        if (!PrepareLists()) return E_NOINTERFACE;
        HRESULT result = S_OK;
        for (HWND list : subclassedLists) {
            DWORD_PTR returned = 0;
            // Permit sent-message reentrancy: inclusion may request WM_GETICON
            // from a proxy owned by this worker. SMTO_BLOCK would deadlock it.
            if (!SendMessageTimeoutW(list, g_refreshTask, action, reinterpret_cast<LPARAM>(member),
                                     SMTO_ABORTIFHUNG, 500, &returned)) result = HRESULT_FROM_WIN32(ERROR_TIMEOUT);
            else if (FAILED(static_cast<HRESULT>(returned))) result = static_cast<HRESULT>(returned);
            // All lists share this task band/icon container; issue one load.
            else if (action == kRefreshTaskIcon) return static_cast<HRESULT>(returned);
        }
        return result;
    }
    void DetachLists() {
        for (HWND list : subclassedLists)
            if (IsWindow(list)) WindhawkUtils::RemoveWindowSubclassFromAnyThread(list, TaskListSubclass);
        subclassedLists.clear();
    }
    bool Create(GroupButton& button) override {
        std::vector<ComPtr<IUnknown>> retiredGroups;
        button.window = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOREDIRECTIONBITMAP,
            kProxyClass, button.title.c_str(), WS_POPUP | WS_SYSMENU,
            -32000, -32000, 1, 1, nullptr, nullptr, instance, &button);
        if (!button.window) return false;
        ComPtr<IPropertyStore> properties;
        HRESULT hr = SHGetPropertyStoreForWindow(button.window, IID_PPV_ARGS(&properties));
        GUID id{};
        wchar_t guidText[40]{};
        if (SUCCEEDED(hr)) hr = CoCreateGuid(&id);
        if (SUCCEEDED(hr)) StringFromGUID2(id, guidText, ARRAYSIZE(guidText));
        std::wstring appId = L"Windhawk.HideSnapped.";
        appId += button.settings.separateGroups ? guidText : L"Combined";
        if (SUCCEEDED(hr)) {
            PROPVARIANT value{};
            value.vt = VT_BOOL; value.boolVal = VARIANT_TRUE;
            hr = properties->SetValue(PKEY_AppUserModel_PreventPinning, value);
            if (SUCCEEDED(hr)) {
                value.vt = VT_LPWSTR; value.pwszVal = appId.data();
                hr = properties->SetValue(PKEY_AppUserModel_ID, value);
            }
        }
        if (FAILED(hr)) {
            Wh_Log(L"phase=taskbar create-properties-failed hr=%08X", hr);
            return false;
        }
        Update(button);
        if (!button.icon) {
            Wh_Log(L"phase=taskbar icon-failed; keeping member buttons");
            return false;
        }
        // Register before showing: shell eligibility queries must see the proxy.
        { std::lock_guard lock(g_hiddenMembersMutex);
          auto identity = IdentifyWindow(button.window);
          g_proxyWindows.insert(identity);
          if (button.preview) retiredGroups.push_back(ExchangeProxyGroupLocked(identity, button.nativeGroup.Get())); }
        ShowWindow(button.window, SW_SHOWNOACTIVATE);
        hr = taskbar->AddTab(button.window);
        button.registrationPending = SUCCEEDED(hr);
        Wh_Log(L"phase=taskbar create members=%u hr=%08X", unsigned(button.members.size()), hr);
        return button.registrationPending;
    }
    void Update(GroupButton& button) override {
        std::vector<ComPtr<IUnknown>> retiredGroups;
        // Publish before AddTab/confirmation and before any synchronous shell
        // refresh. Per-monitor mods query this HWND, not the group members.
        UpdateProxyMonitor(button);
        bool confirmedNow = false;
        bool previewChanged = false;
        if (button.previewDirty && button.settings.nativePreview && button.nativeGroup) {
            auto preview = std::make_unique<NativeGroupPreview>();
            HRESULT hr = preview->Initialize(button.nativeGroup.Get());
            Wh_Log(L"phase=taskbar native-preview hr=%08X", hr);
            // Never retain metadata/factory for the previous native group.
            button.preview = SUCCEEDED(hr) ? std::move(preview) : nullptr;
            button.previewDirty = FAILED(hr);
            previewChanged = true;
        }
        {
            std::lock_guard lock(g_hiddenMembersMutex);
            // Shared members are retargeted before any synchronous shell refresh.
            std::erase_if(g_groupButtons, [&button](const auto& item) { return item.second == button.window; });
            if (button.registered)
                for (const auto& member : button.members) g_groupButtons[member] = button.window;
            auto identity = IdentifyWindow(button.window);
            if (g_proxyWindows.contains(identity)) {
                retiredGroups.push_back(ExchangeProxyGroupLocked(identity,
                    button.preview ? button.nativeGroup.Get() : nullptr));
            }
        }
        if (previewChanged) SetWindowPos(button.window, nullptr, -32000, -32000,
            button.preview ? button.preview->width : 1,
            button.preview ? button.preview->height : 1, SWP_NOZORDER | SWP_NOACTIVATE);
        ULONGLONG now = GetTickCount64();
        std::erase_if(button.iconRects, [&button](const auto& item) {
            return std::find(button.members.begin(), button.members.end(), item.first) == button.members.end();
        });
        auto iconMembers = OrderedIconMembers(button.members, button.settings.screenIconOrder,
            [&button](HWND window, RECT& rect) {
                return CachedIconRect(IdentifyWindow(window), IsIconic(window), button.iconRects, rect);
            });
        if (button.iconMembers != iconMembers) button.iconTime = 0;
        if (!button.icon || !button.iconTime || now - button.iconTime >= 5000) {
            HICON icon = ComposeGroupIcon(iconMembers, button.settings);
            if (icon) {
                SendMessageW(button.window, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
                SendMessageW(button.window, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
                if (button.icon) DestroyIcon(button.icon);
                button.icon = icon;
                button.iconTime = now;
                button.iconMembers = std::move(iconMembers);
                button.iconRedrawPending = true;
            }
        }
        if (button.appliedTitle != button.title) {
            SetWindowTextW(button.window, button.title.c_str());
            button.appliedTitle = button.title;
        }
        if (button.registrationLost) {
            button.registrationPending = SUCCEEDED(taskbar->AddTab(button.window));
            button.registrationLost = !button.registrationPending;
        }
        if (button.registrationPending) {
            // AddTab accepts an asynchronous request; only an exact native item
            // match and inclusion refresh permit hiding real member buttons.
            HRESULT hr = RefreshMember(button.window);
            if (hr != button.registrationResult) {
                Wh_Log(L"phase=taskbar button-confirmation hr=%08X", hr);
                button.registrationResult = hr;
            }
            if (SUCCEEDED(hr)) {
                button.registered = true;
                button.registrationPending = false;
                button.monitorRefreshPending = false;
                confirmedNow = true;
                Wh_Log(L"phase=taskbar button-confirmed=1");
            }
        }
        if (button.registered) {
            std::lock_guard lock(g_hiddenMembersMutex);
            for (const auto& member : button.members) g_groupButtons[member] = button.window;
            if (button.preview) retiredGroups.push_back(ExchangeProxyGroupLocked(
                IdentifyWindow(button.window), button.nativeGroup.Get()));
        }
        if (button.registered && button.iconRedrawPending) {
            HRESULT hr = NotifyIconChanged(button.window);
            Wh_Log(L"phase=taskbar icon-update source=native hr=%08X members=%u", hr, unsigned(button.members.size()));
            // Failed dispatch/match retries without allocating another HICON.
            button.iconRedrawPending = FAILED(hr);
        }
        HWND foreground = GetForegroundWindow();
        RememberAppForeground(foreground);
        for (const auto& member : button.members) if (member.window == foreground) {
            // Initialize once if the group was already active at registration.
            // Subsequent activity uses native hooks, never timer-driven ActivateTab.
            if (confirmedNow) RefreshMember(button.window, true);
        }
    }
    HRESULT Hide(const WindowIdentity& member) override {
        if (!MemberExists(member)) return S_FALSE;
        {
            std::lock_guard lock(g_hiddenMembersMutex);
            if (g_hiddenMembers.contains(member)) return S_OK;
            g_hiddenMembers.insert(member);
        }
        TraceWrite(TraceHide, TracePointer(member.window), member.process, member.thread, 1);
        HRESULT hr = RefreshMember(member.window);
        if (FAILED(hr)) {
            { std::lock_guard lock(g_hiddenMembersMutex); g_hiddenMembers.erase(member); }
            TraceWrite(TraceHide, TracePointer(member.window), member.process, member.thread, 0);
            RefreshMember(member.window);
        }
        if (FAILED(hr) && reportedHideErrors.insert(hr).second)
            Wh_Log(L"phase=taskbar hide-failed hr=%08X", hr);
        return hr;
    }
    HRESULT Restore(const WindowIdentity& member) override {
        { std::lock_guard lock(g_hiddenMembersMutex); g_hiddenMembers.erase(member); }
        TraceWrite(TraceRestore, TracePointer(member.window), member.process, member.thread);
        if (!MemberExists(member)) return S_FALSE;
        HRESULT hr = RefreshMember(member.window, kRestoreTask);
        return hr;
    }
    void Destroy(GroupButton& button) override {
        ComPtr<IUnknown> retiredGroup;
        auto proxy = IdentifyWindow(button.window);
        TraceWrite(TraceProxyRetire, TracePointer(button.window), button.registered,
                   button.registrationPending, button.members.size());
        {
            std::lock_guard lock(g_hiddenMembersMutex);
            std::erase_if(g_groupButtons, [&button](const auto& item) { return item.second == button.window; });
        }
        if (button.registered || button.registrationPending) taskbar->DeleteTab(button.window);
        button.registered = false;
        button.preview.reset();
        // DeleteTab is asynchronous. The native XAML TaskGroup can still read
        // its AppId after this request. Keep proxy properties valid until HWND
        // destruction; clearing PKEY_AppUserModel_ID here can expose a null ID.
        { std::lock_guard lock(g_hiddenMembersMutex);
          auto identity = IdentifyWindow(button.window);
          g_proxyWindows.erase(identity);
          retiredGroup.Swap(ExchangeProxyGroupLocked(identity, nullptr)); }
        if (button.window) DestroyWindow(button.window);
        // Retain routing through DeleteTab and HWND destruction callbacks.
        PublishProxyMonitor(proxy, nullptr);
        button.window = nullptr;
        if (button.icon) DestroyIcon(button.icon);
        button.icon = nullptr;
    }
};

struct TaskbarScanCounts {
    unsigned candidates = 0, queryFailures = 0, invalid = 0, desktopRejected = 0;
};
HRESULT ReadNativeGroups(IUnknown* model, std::vector<ComPtr<IUnknown>>& groups) {
    using GetGroups = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, uint32_t*, IUnknown***);
    uint32_t count = 0;
    IUnknown** items = nullptr;
    HRESULT hr = AbiMethod<GetGroups>(model, 6)(model, &count, &items);
    if (count && !items) return E_UNEXPECTED;
    bool oversized = count > 4096;
    if (oversized) hr = E_UNEXPECTED;
    for (uint32_t i = 0; i < count; ++i) {
        ComPtr<IUnknown> item;
        item.Attach(items[i]); // array entries carry owned WinRT references
        ComPtr<IUnknown> group;
        if (SUCCEEDED(hr) && item && SUCCEEDED(item->QueryInterface(kTaskGroup,
                reinterpret_cast<void**>(group.GetAddressOf())))) groups.push_back(std::move(group));
    }
    CoTaskMemFree(items);
    return hr;
}
void PublishAltTabMembership(IUnknown* model, bool enabled) {
    auto snapshot = std::make_shared<AltTabMembership>();
    bool complete = true;
    std::vector<ComPtr<IUnknown>> groups;
    if (enabled && (!model || FAILED(ReadNativeGroups(model, groups)))) complete = false;
    for (const auto& group : groups) {
        using Windows = HRESULT(WINAPI*)(IUnknown*, uint32_t*, uint32_t**);
        uint32_t count = 0; uint32_t* windows = nullptr;
        HRESULT hr = AbiMethod<Windows>(group.Get(), 7)(group.Get(), &count, &windows);
        if (FAILED(hr) || count > 64 || (count && !windows)) complete = false;
        if (SUCCEEDED(hr) && count >= 2 && count <= 64 && windows) {
            for (uint32_t i = 0; i < count; ++i) {
                HWND window = reinterpret_cast<HWND>(static_cast<intptr_t>(static_cast<int32_t>(windows[i])));
                DWORD process = 0;
                DWORD thread = GetWindowThreadProcessId(window, &process);
                if (thread) snapshot->members.push_back({window, process, thread});
            }
        }
        CoTaskMemFree(windows);
    }
    if (!complete) snapshot->members.clear(); // no stale hiding after an incomplete model read
    snapshot->updated = GetTickCount64();
    g_altTabMembership.store(std::move(snapshot));
}
DWORD TaskbarProcessId();

DWORD WINAPI AltTabSnapshotWorker(void*) {
    HRESULT apartment = RoInitialize(RO_INIT_MULTITHREADED);
    if (FAILED(apartment)) return 0;
    CoEnableCallCancellation(nullptr);
    while (WaitForSingleObject(g_taskbarStop, 0) == WAIT_TIMEOUT) {
        bool enabled = TaskbarProcessId() == GetCurrentProcessId() &&
                       g_altTabEnabled.load() && !g_diagnosticMode.load() && NativeGroupsEnabled();
        if (!enabled) {
            g_altTabMembership.store(nullptr);
            if (WaitForSingleObject(g_taskbarStop, 1000) != WAIT_TIMEOUT) break;
            continue;
        }
        ComPtr<IUnknown> model;
        GetCurrentModel(model);
        PublishAltTabMembership(model.Get(), true);
        if (WaitForSingleObject(g_taskbarStop, 500) != WAIT_TIMEOUT) break;
    }
    g_altTabMembership.store(nullptr);
    CoDisableCallCancellation(nullptr);
    RoUninitialize();
    return 0;
}
std::vector<GroupSnapshot> ReadTaskbarGroups(IUnknown* model, IVirtualDesktopManager* desktops,
                                           bool currentDesktopOnly, TaskbarScanCounts& counts) {
    std::vector<ComPtr<IUnknown>> nativeGroups;
    if (FAILED(ReadNativeGroups(model, nativeGroups))) { ++counts.queryFailures; return {}; }
    counts.candidates = static_cast<unsigned>(nativeGroups.size());
    std::vector<GroupSnapshot> result;
    for (const auto& group : nativeGroups) {
        uint32_t count = 0;
        uint32_t* members = nullptr;
        using Windows = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, uint32_t*, uint32_t**);
        HRESULT hr = AbiMethod<Windows>(group.Get(), 7)(group.Get(), &count, &members);
        if (FAILED(hr)) ++counts.queryFailures;
        GroupSnapshot snapshot;
        snapshot.nativeGroup = group;
        // The shell supplies localization, window title and plural forms.
        // A title failure must not turn a valid group into missing membership.
        ReadGroupTitle(group.Get(), snapshot.title);
        bool valid = SUCCEEDED(hr) && members && count >= 2 && count <= 64;
        if (valid) for (uint32_t i = 0; i < count; ++i) {
            HWND member = reinterpret_cast<HWND>(static_cast<intptr_t>(static_cast<int32_t>(members[i])));
            if (!TaskbarCandidate(member)) { valid = false; break; }
            if (currentDesktopOnly) {
                BOOL current = FALSE;
                if (!desktops || FAILED(desktops->IsWindowOnCurrentVirtualDesktop(member, &current)) || !current) {
                    ++counts.desktopRejected;
                    valid = false; break;
                }
            }
            snapshot.members.push_back(IdentifyWindow(member));
        }
        CoTaskMemFree(members);
        if (!valid) { ++counts.invalid; continue; }
        snapshot.key = snapshot.members;
        std::sort(snapshot.key.begin(), snapshot.key.end());
        if (std::adjacent_find(snapshot.key.begin(), snapshot.key.end()) != snapshot.key.end()) continue;
        result.push_back(std::move(snapshot));
    }
    return result;
}

LRESULT CALLBACK TaskbarHostProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (g_taskbarCreated && message == g_taskbarCreated) {
        auto state = reinterpret_cast<TaskbarState*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (state) for (auto& [key, button] : state->buttons) {
            button->registered = false;
            button->registrationLost = true;
        }
        return 0;
    }
    if (message == kMemberDestroyed) {
        auto state = reinterpret_cast<TaskbarState*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (state) state->Forget(reinterpret_cast<HWND>(lParam));
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}
void CALLBACK TaskbarWinEvent(HWINEVENTHOOK, DWORD, HWND window, LONG object,
                             LONG child, DWORD, DWORD) {
    if (object == OBJID_WINDOW && child == CHILDID_SELF && window) {
        HWND host = g_taskbarHost.load();
        if (host) PostMessageW(host, kMemberDestroyed, 0, reinterpret_cast<LPARAM>(window));
    }
}
void CALLBACK ForegroundWinEvent(HWINEVENTHOOK, DWORD, HWND window, LONG, LONG, DWORD, DWORD) {
    RememberAppForeground(window);
}
bool InitializeTaskbarIntegration(bool deferred);
DWORD TaskbarProcessId() {
    DWORD process = 0;
    GetWindowThreadProcessId(FindWindowW(L"Shell_TrayWnd", nullptr), &process);
    return process;
}
bool WaitForShellOwnership(HANDLE stop, DWORD process,
        DWORD (*owner)() = TaskbarProcessId,
        DWORD (WINAPI* wait)(HANDLE, DWORD) = WaitForSingleObject) {
    bool waiting = false;
    for (;;) {
        if (wait(stop, 0) != WAIT_TIMEOUT) return false;
        DWORD shell = owner();
        if (shell == process) break;
        if (!waiting) {
            Wh_Log(L"phase=taskbar startup-wait=%s", shell ? L"shell-ownership" : L"shell-window");
            waiting = true;
        }
        // Cancellation, not a fixed startup sleep: Explorer may create its tray
        // after the mod's Init/AfterInit callbacks have already returned.
        // A previous Explorer may still own the tray during replacement. Folder
        // processes only keep this lightweight waiter; never install taskbar
        // hooks or create proxy buttons until this process owns the shell.
        if (wait(stop, shell ? 1000 : 100) != WAIT_TIMEOUT) return false;
    }
    return true;
}
bool PrepareTaskbarWorker(HANDLE stop, DWORD process,
        DWORD (*owner)() = TaskbarProcessId,
        DWORD (WINAPI* wait)(HANDLE, DWORD) = WaitForSingleObject,
        bool (*initialize)(bool) = InitializeTaskbarIntegration) {
    if (!WaitForShellOwnership(stop, process, owner, wait)) return false;
    if (g_taskbarFilterReady.load()) return true;
    return initialize(true);
}

DWORD WINAPI WaitForTaskbarRetry(HANDLE stop, DWORD timeout) {
    if (!timeout) return WaitForSingleObject(stop, 0);
    ULONGLONG deadline = GetTickCount64() + timeout;
    for (;;) {
        ULONGLONG now = GetTickCount64();
        if (now >= deadline) return WaitForSingleObject(stop, 0);
        DWORD result = MsgWaitForMultipleObjects(1, &stop, FALSE,
            static_cast<DWORD>(deadline - now), QS_ALLINPUT);
        if (result != WAIT_OBJECT_0 + 1) return result;
        // The STA's dispatcher/COM can need messages while shell services
        // start. Bound each batch so messages cannot starve stop/deadline.
        MSG message;
        for (unsigned i = 0; i < 32 && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE); ++i) {
            DWORD stopped = WaitForSingleObject(stop, 0);
            if (stopped != WAIT_TIMEOUT) return stopped;
            TranslateMessage(&message); DispatchMessageW(&message);
        }
    }
}
HRESULT InitializeTaskbarApi(HANDLE stop, ComPtr<ITaskbarList>& taskbar,
        decltype(&CoCreateInstance) create = CoCreateInstance,
        DWORD (WINAPI* wait)(HANDLE, DWORD) = WaitForTaskbarRetry) {
    taskbar.Reset();
    HRESULT lastFailure = S_OK;
    unsigned lastStage = 0, attempts = 0;
    auto cancelled = [](DWORD status) {
        return status == WAIT_OBJECT_0 ? HRESULT_FROM_WIN32(ERROR_CANCELLED) : E_FAIL;
    };
    for (;;) {
        DWORD status = wait(stop, 0);
        if (status != WAIT_TIMEOUT) return cancelled(status);
        ++attempts;
        ComPtr<ITaskbarList> candidate;
        HRESULT hr = create(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&candidate));
        unsigned stage = 1; // COM creation
        if (SUCCEEDED(hr) && !candidate) hr = E_UNEXPECTED;
        if (SUCCEEDED(hr)) { stage = 2; hr = candidate->HrInit(); }
        if (SUCCEEDED(hr)) {
            status = wait(stop, 0);
            if (status != WAIT_TIMEOUT) return cancelled(status);
            taskbar = std::move(candidate);
            Wh_Log(L"phase=taskbar shell-api-ready attempts=%u", attempts);
            TraceWrite(TraceWorker, 4, 0, attempts);
            return S_OK;
        }
        // A failed object may retain failed initialization state. Release it
        // before waiting and retry with a fresh instance, never hide members.
        candidate.Reset();
        if (hr != lastFailure || stage != lastStage) {
            Wh_Log(L"phase=taskbar shell-api-retry stage=%u hr=%08X", stage, hr);
            TraceWrite(TraceWorker, 2, uint32_t(hr), stage);
            lastFailure = hr; lastStage = stage;
        }
        status = wait(stop, 1000);
        if (status != WAIT_TIMEOUT) return cancelled(status);
    }
}

void ShutdownTaskbarDispatcher(winrt::Windows::System::DispatcherQueueController& dispatcher) {
    if (!dispatcher) return;
    try {
        auto shutdown = dispatcher.ShutdownQueueAsync();
        ULONGLONG deadline = GetTickCount64() + 5000;
        while (shutdown.Status() == winrt::Windows::Foundation::AsyncStatus::Started && GetTickCount64() < deadline) {
            MSG message;
            for (unsigned count = 0; count < 32 && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE); ++count) {
                TranslateMessage(&message); DispatchMessageW(&message);
            }
            MsgWaitForMultipleObjects(0, nullptr, FALSE, 10, QS_ALLINPUT);
        }
        if (shutdown.Status() == winrt::Windows::Foundation::AsyncStatus::Started)
            Wh_Log(L"phase=taskbar dispatcher-shutdown timeout=1");
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"phase=taskbar dispatcher-shutdown hr=%08X", int32_t(error.code()));
    }
    dispatcher = nullptr;
}
DWORD WINAPI TaskbarThread(void*) {
    if (!WaitForShellOwnership(g_taskbarStop, GetCurrentProcessId())) return 0;
    g_altTabSnapshotThread = CreateThread(nullptr, 0, AltTabSnapshotWorker, nullptr, 0, nullptr);
    if (!PrepareTaskbarWorker(g_taskbarStop, GetCurrentProcessId())) return 0;
    Wh_Log(L"phase=taskbar worker-started=1");
    TraceWrite(TraceWorker, 1);
    HRESULT initialized = RoInitialize(RO_INIT_SINGLETHREADED);
    if (FAILED(initialized)) {
        Wh_Log(L"phase=taskbar apartment-failed hr=%08X", initialized);
        return 0;
    }
    // Retain the existing worker queue for native device/factory activation.
    // Hover visuals themselves use the taskbar host's compositor and thread.
    winrt::Windows::System::DispatcherQueueController dispatcher{nullptr};
    HMODULE coreMessaging = LoadLibraryExW(L"CoreMessaging.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (coreMessaging) {
        struct QueueOptions { DWORD size; int threadType; int apartmentType; };
        using CreateQueue = HRESULT(WINAPI*)(QueueOptions, void**);
        auto create = reinterpret_cast<CreateQueue>(GetProcAddress(coreMessaging, "CreateDispatcherQueueController"));
        void* raw = nullptr;
        if (create && SUCCEEDED(create({sizeof(QueueOptions), 2, 0}, &raw)))
            dispatcher = {raw, winrt::take_ownership_from_abi};
    }
    Wh_Log(L"phase=taskbar preview-dispatcher=%d", bool(dispatcher));
    ShellTaskbarOperations operations;
    HRESULT hr = InitializeTaskbarApi(g_taskbarStop, operations.taskbar);
    if (FAILED(hr)) {
        Wh_Log(L"phase=taskbar shell-api-stopped hr=%08X", hr);
        operations.taskbar.Reset();
        ShutdownTaskbarDispatcher(dispatcher);
        if (coreMessaging) FreeLibrary(coreMessaging);
        RoUninitialize(); return 0;
    }
    ComPtr<IVirtualDesktopManager> desktops;
    hr = CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&desktops));
    if (FAILED(hr)) Wh_Log(L"phase=taskbar desktop-manager-failed hr=%08X", hr);
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&TaskbarThread), &operations.instance);
    WNDCLASSW proxy{};
    proxy.hInstance = operations.instance;
    proxy.lpfnWndProc = GroupButtonProc;
    proxy.lpszClassName = kProxyClass;
    WNDCLASSW hostClass = proxy;
    hostClass.lpfnWndProc = TaskbarHostProc;
    hostClass.lpszClassName = L"Windhawk.HideSnapped.Host";
    ATOM proxyAtom = RegisterClassW(&proxy), hostAtom = RegisterClassW(&hostClass);
    HWND host = hostAtom ? CreateWindowExW(0, hostClass.lpszClassName, L"", 0,
                                         0, 0, 0, 0, nullptr, nullptr, operations.instance, nullptr) : nullptr;
    TaskbarState state;
    if (proxyAtom && host) {
        g_taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
        SetWindowLongPtrW(host, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&state));
        g_taskbarHost = host;
        HWINEVENTHOOK destroyed = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY,
            nullptr, TaskbarWinEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD);
        HWINEVENTHOOK foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
            nullptr, ForegroundWinEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
        RememberAppForeground(GetForegroundWindow());
        unsigned generation = g_settingsGeneration.load();
        unsigned lastButtons = ~0u, lastHidden = ~0u;
        bool lastScanFailed = false;
        ULONGLONG nextRefresh = 0;
        while (WaitForSingleObject(g_taskbarStop, 0) != WAIT_OBJECT_0) {
            TaskbarSettings settings;
            unsigned currentGeneration;
            { std::lock_guard lock(g_taskbarSettingsMutex);
              settings = g_taskbarSettings; currentGeneration = g_settingsGeneration.load(); }
            if (!dispatcher || !g_nativePreviewAvailable) settings.nativePreview = false;
            if (currentGeneration != generation) {
                bool rebuilt = state.ApplySettings(settings, operations);
                Wh_Log(L"phase=taskbar settings-applied rebuild=%d", rebuilt);
                generation = currentGeneration;
                nextRefresh = 0;
            }
            // Shell replies and icon requests must not turn this into a busy
            // loop. Input dispatch is independent of the snapshot deadline.
            if (GetTickCount64() >= nextRefresh) {
                if (g_diagnosticMode.load()) settings.enabled = false;
                std::vector<GroupSnapshot> snapshots;
                TaskbarScanCounts scan;
                if (settings.enabled) {
                    ComPtr<IUnknown> model;
                    hr = GetCurrentModel(model);
                    if (SUCCEEDED(hr)) snapshots = ReadTaskbarGroups(model.Get(), desktops.Get(), settings.currentDesktopOnly, scan);
                    else Wh_Log(L"phase=taskbar model-failed hr=%08X", hr);
                }
                // Failure is not an authoritative empty Groups collection.
                bool scanFailed = settings.enabled && (FAILED(hr) || scan.queryFailures);
                if (scanFailed != lastScanFailed) {
                    Wh_Log(L"phase=taskbar scan-failed=%d retaining-buttons=%u", scanFailed, unsigned(state.buttons.size()));
                    lastScanFailed = scanFailed;
                }
                uint64_t scanFlags = uint64_t(settings.enabled) | (uint64_t(scanFailed) << 1);
                TraceWrite(TraceScan, snapshots.size(), state.buttons.size(), state.hidden.size(), scanFlags);
                TraceWrite(TraceScanCounters, scan.candidates, 0, scan.queryFailures,
                           uint64_t(scan.invalid) | (uint64_t(scan.desktopRejected) << 32));
                state.Reconcile(snapshots, settings, operations, !scanFailed);
                TraceWrite(TraceScan, snapshots.size(), state.buttons.size(), state.hidden.size(), scanFlags | 4);
                if (lastButtons != state.buttons.size() || lastHidden != state.hidden.size()) {
                    lastButtons = static_cast<unsigned>(state.buttons.size());
                    lastHidden = static_cast<unsigned>(state.hidden.size());
                    Wh_Log(L"phase=taskbar buttons=%u hidden=%u", lastButtons, lastHidden);
                    Wh_Log(L"phase=taskbar scan candidates=%u query-failures=%u invalid=%u desktop-rejected=%u",
                           scan.candidates, scan.queryFailures, scan.invalid, scan.desktopRejected);
                }
                nextRefresh = GetTickCount64() + settings.interval;
            }
            ULONGLONG now = GetTickCount64();
            DWORD timeout = nextRefresh > now ? static_cast<DWORD>(nextRefresh - now) : 0;
            DWORD wait = MsgWaitForMultipleObjects(1, &g_taskbarStop, FALSE, timeout, QS_ALLINPUT);
            if (wait == WAIT_OBJECT_0) break;
            MSG message;
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message); DispatchMessageW(&message);
            }
        }
        if (destroyed) UnhookWinEvent(destroyed);
        if (foregroundHook) UnhookWinEvent(foregroundHook);
        g_taskbarHost = nullptr;
    } else Wh_Log(L"phase=taskbar window-init-failed code=%lu", GetLastError());
    state.Stop(operations);
    std::map<WindowIdentity, ComPtr<IUnknown>> retiredGroups;
    { std::lock_guard lock(g_hiddenMembersMutex);
      retiredGroups.swap(*g_proxyGroups); }
    retiredGroups.clear(); // release remaining refs on the worker, outside the lock
    operations.DetachLists();
    if (host) DestroyWindow(host);
    if (hostAtom) UnregisterClassW(hostClass.lpszClassName, operations.instance);
    if (proxyAtom) UnregisterClassW(kProxyClass, operations.instance);
    desktops.Reset(); operations.taskbar.Reset();
    ShutdownTaskbarDispatcher(dispatcher);
    if (coreMessaging) FreeLibrary(coreMessaging);
    RoUninitialize();
    Wh_Log(L"phase=taskbar stopped");
    TraceWrite(TraceWorker, 3);
    return 0;
}
void LoadSettings() {
    g_diagnosticMode = Wh_GetIntSetting(L"diagnosticMode") != 0;
    g_altTabEnabled = Wh_GetIntSetting(L"altTabEnabled") != 0;
    TaskbarSettings settings;
    settings.enabled = Wh_GetIntSetting(L"taskbarEnabled") != 0;
    settings.hideMembers = Wh_GetIntSetting(L"hideTaskbarMembers") != 0;
    settings.separateGroups = Wh_GetIntSetting(L"separateTaskbarGroups") != 0;
    settings.centerLastRow = Wh_GetIntSetting(L"centerLastRow") != 0;
    settings.currentDesktopOnly = Wh_GetIntSetting(L"currentDesktopOnly") != 0;
    settings.nativePreview = Wh_GetIntSetting(L"nativeGroupPreview") != 0;
    settings.maxIcons = std::clamp(Wh_GetIntSetting(L"maxIcons"), 2, 4);
    settings.iconSize = std::clamp(Wh_GetIntSetting(L"iconSize"), 4, 15);
    settings.gap = std::clamp(Wh_GetIntSetting(L"iconGap"), 0, 8);
    WindhawkUtils::StringSetting order(Wh_GetStringSetting(L"iconOrder"));
    settings.screenIconOrder = wcscmp(order, L"native") != 0;
    settings.interval = std::clamp(Wh_GetIntSetting(L"refreshInterval"), 100, 5000);
    WindhawkUtils::StringSetting click(Wh_GetStringSetting(L"clickAction"));
    settings.toggleOnClick = wcscmp(click, L"toggle") == 0;
    Wh_Log(L"phase=settings version=%s toggle=%d native-preview=%d icon-order=%s",
           WH_MOD_VERSION, settings.toggleOnClick, settings.nativePreview, settings.screenIconOrder ? L"screen" : L"native");
    WindhawkUtils::StringSetting close(Wh_GetStringSetting(L"closeAction"));
    settings.minimizeOnClose = wcscmp(close, L"ignore") != 0;
    { std::lock_guard lock(g_taskbarSettingsMutex);
      g_taskbarSettings = settings; ++g_settingsGeneration; }
}

bool CheckedNativeImage(PCWSTR name, DWORD timestamp, DWORD imageSize) {
    wchar_t system[MAX_PATH]{};
    if (!GetSystemDirectoryW(system, ARRAYSIZE(system))) return false;
    std::wstring path = name[0] && name[1] == L':' ? std::wstring(name) : std::wstring(system) + L"\\" + name;
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    alignas(8) BYTE header[4096]{};
    DWORD read = 0;
    bool ok = ReadFile(file, header, sizeof(header), &read, nullptr) != FALSE;
    CloseHandle(file);
    auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(header);
    if (!ok || read < sizeof(*dos) || dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0 ||
        size_t(dos->e_lfanew) + sizeof(IMAGE_NT_HEADERS64) > read) return false;
    auto pe = reinterpret_cast<const IMAGE_NT_HEADERS64*>(header + dos->e_lfanew);
    return pe->Signature == IMAGE_NT_SIGNATURE && pe->FileHeader.TimeDateStamp == timestamp &&
           pe->OptionalHeader.SizeOfImage == imageSize;
}

bool ActivateTaskbarFilter(bool deferred, bool (*hook)() = HookTaskbarFilter,
                          BOOL (*apply)() = Wh_ApplyHookOperations) {
    if (!hook()) return false;
    // Windhawk automatically applies only hooks queued during ModInit. A late
    // startup worker must publish its operations before hiding any members.
    if (deferred && !apply()) {
        g_taskbarFilterReady = false;
        Wh_Log(L"phase=taskbar deferred-hooks-applied=0");
        return false;
    }
    if (deferred) Wh_Log(L"phase=taskbar deferred-hooks-applied=1");
    return true;
}
bool InitializeTaskbarIntegration(bool deferred) {
    if (g_taskbarInitAttempted.exchange(true)) return g_taskbarFilterReady.load();
    // Preview ABI failure disables previews independently of taskbar buttons.
    g_nativePreviewAvailable = CheckedNativeImage(L"Taskbar.dll", 0x4059c321, 0x30d000) &&
        CheckedNativeImage(L"DesktopSwitcherDataModel.dll", 0xf9a924c5, 0x56000);
    Wh_Log(L"phase=taskbar preview-abi=%d", g_nativePreviewAvailable.load());
    // The user permits unverified revisions. Required symbols still have to
    // resolve, but matching names do not validate our native fields/slots.
    Wh_Log(L"phase=taskbar native-layout-version-gate=0");
    bool ready = ActivateTaskbarFilter(deferred);
    g_traceNativeReady = ready;
    return ready;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    g_binaryTraceEnabled = Wh_GetIntSetting(L"binaryTrace") != 0;
    HMODULE module = LoadLibraryExW(L"twinui.pcshell.dll", nullptr,
                                     LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return FALSE;
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{L"?GetWindowForView@ApplicationViewHelpers@@YAPEAUHWND__@@PEAUIApplicationView@@@Z"},
         &g_getWindowForView},
        {{L"?IsViewVisible@CVirtualDesktop@@UEAAJPEAUIApplicationView@@PEAH@Z"},
         &g_isViewVisible, IsViewVisibleHook},
        {{L"?IsGroupVisibleOnDesktop@DesktopTaskGroupsSwitchItemController@implementation@DesktopTaskGroups@Shell@Internal@Windows@winrt@@AEAA_NAEBUTaskGroup@TaskGroups@4567@PEAUIVirtualDesktop@@@Z"},
         &g_groupVisible, GroupVisibleHook},
        {{L"?Show@XamlAltTabViewHost@@UEAAJPEAUIImmersiveMonitor@@W4ALT_TAB_VIEW_FLAGS@@PEAUIApplicationView@@@Z"},
         &g_show, ShowHook},
        {{L"?XamlAltTabViewHost_CreateInstance@@YAJAEBUXamlViewHostInitializeArgs@@AEBU_GUID@@PEAPEAX@Z"},
         &g_createInstance, CreateInstanceHook},
        {{L"<lambda_d61b288df26b99e0330e99e4b6525b65>::operator()"},
         &g_evaluateItem, EvaluateItemHook, true},
        {{L"?GetItemForView@CachedDataSource@@QEBAPE$AAUISwitchItem@Multitasking@ComposableShell@Internal@Windows@@PEAUIApplicationView@@@Z"},
         &g_getItemForView, GetItemForViewHook},
        {{L"??0CachedDataSource@@QEAA@_N0PEAUIAppViewDataSourceAdapter@@AEBUIVirtualDesktopController@Multitasking@ComposableShell@Internal@Windows@winrt@@AEBUIVirtualDesktopDataSource@34567@@Z"},
         &g_cachedSourceConstructor, nullptr, true},
        {{L"?TryGetTaskGroupsDataSourceAdapter@@YA?AUTaskGroupDataSourceAdapter@TaskGroups@Shell@Internal@Windows@winrt@@PEAUIAppViewDataSourceAdapter@@@Z"},
         &g_tryGetGroupAdapter, nullptr, true},
    };
    // DIA and DbgHelp disagree on some undecorated function names, including
    // the local EvaluateItem lambda. Match exact PDB names instead.
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE;
    const bool hooked = WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks), &options);
    g_cachedOnlyAltTab = !g_evaluateItem;
    g_cachedSourceLayout = DecodeCachedSourceLayout(g_cachedSourceConstructor, g_tryGetGroupAdapter);
    Wh_Log(L"phase=selection layout=%d groups-offset=%zu adapter-offset=%zu",
           bool(g_cachedSourceLayout), g_cachedSourceLayout.groupsEnabled, g_cachedSourceLayout.groupAdapter);
    // Keep the reference for the process lifetime. Hooks are queued until Init
    // returns and their trampolines can outlive BeforeUninit.
    Wh_Log(L"phase=init lookup=raw symbols=%d", hooked);
    if (!hooked) {
        Wh_Log(L"phase=init hook-install-failed; enable Windhawk debug logging for symbol details");
    }
    if (hooked) {
        DWORD shellProcess = TaskbarProcessId();
        if (shellProcess == GetCurrentProcessId()) InitializeTaskbarIntegration(false);
        else Wh_Log(L"phase=taskbar startup-deferred shell-present=%d", shellProcess != 0);
    }
    return hooked;
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = g_binaryTraceEnabled != (Wh_GetIntSetting(L"binaryTrace") != 0);
    LoadSettings();
    return TRUE;
}

void Wh_ModAfterInit() {
    // Explorer can have many folder processes; only the real taskbar process
    // owns proxy buttons. A disabled taskbar setting can be enabled later.
    g_taskbarStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_taskbarStop && g_binaryTraceEnabled) g_viewTraceThread = CreateThread(nullptr, 0, [](void*) -> DWORD {
        for (unsigned attempt = 0; attempt < 300; ++attempt) {
            if (WaitForSingleObject(g_taskbarStop, 100) != WAIT_TIMEOUT) break;
            if (g_traceActive.load() && g_traceNativeReady.load() && GetModuleHandleW(L"Taskbar.View.dll")) {
                HookTaskbarViewTrace(); return 0;
            }
        }
        if (g_traceActive.load() && WaitForSingleObject(g_taskbarStop, 0) == WAIT_TIMEOUT) {
            TraceWrite(TraceViewReady, 0, 0, 0);
            Wh_Log(L"phase=trace view-wait-timeout=1");
        }
        return 0;
    }, nullptr, 0, nullptr);
    if (g_taskbarStop) g_taskbarThread = CreateThread(nullptr, 0, TaskbarThread, nullptr, 0, nullptr);
    if (!g_taskbarThread && !g_viewTraceThread && g_taskbarStop) {
        CloseHandle(g_taskbarStop); g_taskbarStop = nullptr;
    }
    Wh_Log(L"phase=taskbar startup-thread-started=%d", g_taskbarThread != nullptr);
}

void Wh_ModBeforeUninit() {
    if (g_taskbarStop) SetEvent(g_taskbarStop);
    if (g_viewTraceThread) {
        WaitForSingleObject(g_viewTraceThread, INFINITE);
        CloseHandle(g_viewTraceThread); g_viewTraceThread = nullptr;
    }
    if (g_taskbarThread) {
        WaitForSingleObject(g_taskbarThread, INFINITE);
        CloseHandle(g_taskbarThread); g_taskbarThread = nullptr;
    }
    if (g_altTabSnapshotThread) {
        if (WaitForSingleObject(g_altTabSnapshotThread, 1000) == WAIT_TIMEOUT)
            CoCancelCall(GetThreadId(g_altTabSnapshotThread), 0);
        WaitForSingleObject(g_altTabSnapshotThread, INFINITE);
        CloseHandle(g_altTabSnapshotThread); g_altTabSnapshotThread = nullptr;
    }
    if (g_taskbarStop) { CloseHandle(g_taskbarStop); g_taskbarStop = nullptr; }
}

void Wh_ModUninit() {
    StopTaskbarTrace();
    g_proxyGroups.reset();
    Wh_Log(L"phase=uninit");
}
