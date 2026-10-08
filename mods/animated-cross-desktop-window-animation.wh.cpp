// ==WindhawkMod==
// @id              animated-cross-desktop-window-animation
// @name            Animated Cross-Desktop Activation
// @description     Animates activation-driven virtual desktop switches and suppresses transient replacement-window switches.
// @version         0.6.0
// @author          meteoni
// @github          https://github.com/Meteoni
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Animated Cross-Desktop Activation

Windows can jump to another virtual desktop without the normal desktop-switch
animation when an application activates an existing window on that desktop.
This mod intercepts only those activation-driven switches and replays them
asynchronously through Windows' native virtual-desktop animation path.

Intentional desktop navigation is left alone, including Win+Ctrl+Left/Right,
Task View, touchpad gestures, and other normal virtual-desktop switch routes.

Some applications briefly activate an old window on another desktop while they
create the window that was actually requested on the current desktop. For apps
listed in Settings, the mod holds the pending animated switch for a short grace
period. If a new same-process window is added and becomes foreground on the
current desktop during that period, the stale cross-desktop switch is discarded.
No windows are moved between desktops.

Windows Terminal is enabled for transient replacement suppression by default.

Designed for Windows 11 24H2 and newer. Private shell interfaces and symbols can
change in future Windows builds.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- replacementSuppressionApps:
  - - executable: "WindowsTerminal.exe"
      $name: Executable name
      $description: >-
        Executable whose transient old-window activation should be held briefly
        while a replacement window is created.
    - gracePeriodMs: 500
      $name: Replacement grace period (ms)
      $description: >-
        Delay the pending animated desktop switch by this amount. If a newly
        added same-process window becomes foreground on the current desktop
        during the delay, the pending switch is canceled. Set to 0 to disable
        suppression for this entry.
  $name: Transient replacement suppression
  $description: >-
    Apps that can briefly activate an existing window on another desktop before
    creating the requested window on the current desktop.
*/
// ==/WindhawkModSettings==

#include <ObjectArray.h>
#include <objbase.h>
#include <servprov.h>
#include <shobjidl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>

#include <wchar.h>
#include <atomic>
#include <cstdint>

// -----------------------------------------------------------------------------
// Explicit COM IDs used by the minimal interface declarations below.
// -----------------------------------------------------------------------------

static const CLSID kClsidImmersiveShell = {
    0xC2F03A33,
    0x21F5,
    0x47FA,
    {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};

static const CLSID kClsidVirtualDesktopManagerInternal = {
    0xC5E0CDCA,
    0x7B6E,
    0x41B2,
    {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};

static const GUID kServiceVirtualDesktopNotification = {
    0xA501FDEC,
    0x4A09,
    0x464C,
    {0xAE, 0x4E, 0x1B, 0x9C, 0x21, 0xB8, 0x49, 0x18}};

static const IID kIidVirtualDesktopNotification = {
    0xB9E5E94D,
    0x233E,
    0x49AB,
    {0xAF, 0x5C, 0x2B, 0x45, 0x41, 0xC3, 0xAA, 0xDE}};

static const IID kIidVirtualDesktopNotificationService = {
    0x0CD45E71,
    0xD927,
    0x4F15,
    {0x8B, 0x0A, 0x8F, 0xEF, 0x52, 0x53, 0x37, 0xBF}};

// IVirtualDesktopManagerInternal slots for the 24H2 IID above.
static constexpr int kVtableGetCurrentDesktop = 6;
static constexpr int kVtableGetDesktops = 7;
static constexpr int kVtableSwitchDesktopWithAnimation = 22;
static constexpr int kVtableWaitForAnimationToComplete = 24;

// Windows 11 24H2+ IVirtualDesktopManagerInternal / Internal2 IID.
static const IID kIidVirtualDesktopManagerInternal24H2 = {
    0x53F5CA0B,
    0x158F,
    0x4124,
    {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}};

static const IID kIidVirtualDesktop24H2 = {
    0x3F07F4BE,
    0xB107,
    0x441A,
    {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}};

static const IID kIidApplicationViewCollection = {
    0x1841C6D7,
    0x4F9D,
    0x42C0,
    {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}};

// -----------------------------------------------------------------------------
// Minimal undocumented/public interface declarations.
// -----------------------------------------------------------------------------

struct IVirtualDesktop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IUnknown* view,
                                                    BOOL* visible) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetId(GUID* desktopId) = 0;
};

struct IApplicationView : IUnknown {};

struct IVirtualDesktopManagerPrivate;
struct IVirtualDesktopSwitchAnimator;

struct IApplicationViewCollection : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetViews(IUnknown** objectArray) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    GetViewsByZOrder(IUnknown** objectArray) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    GetViewsByAppUserModelId(LPCWSTR appUserModelId,
                             IUnknown** objectArray) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    GetViewForHwnd(HWND hwnd, IApplicationView** view) = 0;
};

struct IVirtualDesktopManagerInternal : IUnknown {};

struct IVirtualDesktopNotification : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE
    VirtualDesktopCreated(IVirtualDesktop* desktop) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    VirtualDesktopDestroyBegin(IVirtualDesktop* desktopDestroyed,
                               IVirtualDesktop* desktopFallback) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    VirtualDesktopDestroyFailed(IVirtualDesktop* desktopDestroyed,
                                IVirtualDesktop* desktopFallback) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    VirtualDesktopDestroyed(IVirtualDesktop* desktopDestroyed,
                            IVirtualDesktop* desktopFallback) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    VirtualDesktopMoved(IVirtualDesktop* desktop,
                        INT64 oldIndex,
                        INT64 newIndex) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    VirtualDesktopNameChanged(IVirtualDesktop* desktop, void* name) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    ViewVirtualDesktopChanged(IUnknown* view) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    CurrentVirtualDesktopChanged(IVirtualDesktop* desktopOld,
                                 IVirtualDesktop* desktopNew) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    VirtualDesktopWallpaperChanged(IVirtualDesktop* desktop,
                                   void* wallpaper) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    VirtualDesktopSwitched(IVirtualDesktop* desktop, int switchType) = 0;

    virtual HRESULT STDMETHODCALLTYPE
    RemoteVirtualDesktopConnected(IVirtualDesktop* desktop) = 0;
};

struct IVirtualDesktopNotificationService : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE
    Register(IVirtualDesktopNotification* notification, DWORD* cookie) = 0;

    virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD cookie) = 0;
};

// -----------------------------------------------------------------------------
// Settings.
// -----------------------------------------------------------------------------

static constexpr DWORD kMaxReplacementGraceMs = 2000;
static constexpr size_t kMaxReplacementApps = 16;

struct ReplacementAppPolicy {
    wchar_t executable[MAX_PATH] = {};
    DWORD gracePeriodMs = 0;
};

static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static ReplacementAppPolicy g_replacementApps[kMaxReplacementApps] = {};
static size_t g_replacementAppCount = 0;

static DWORD ClampNonNegativeSetting(int configured, DWORD maximum) {
    if (configured <= 0) {
        return 0;
    }

    DWORD value = static_cast<DWORD>(configured);
    return value > maximum ? maximum : value;
}

static void LoadSettings() {
    ReplacementAppPolicy apps[kMaxReplacementApps] = {};
    size_t appCount = 0;

    for (int i = 0; i < static_cast<int>(kMaxReplacementApps); ++i) {
        PCWSTR raw =
            Wh_GetStringSetting(L"replacementSuppressionApps[%d].executable", i);

        if (!raw || !*raw) {
            if (raw) {
                Wh_FreeStringSetting(raw);
            }
            break;
        }

        ReplacementAppPolicy& policy = apps[appCount];
        wcsncpy_s(policy.executable, raw, _TRUNCATE);
        Wh_FreeStringSetting(raw);

        if (!policy.executable[0]) {
            continue;
        }

        policy.gracePeriodMs = ClampNonNegativeSetting(
            Wh_GetIntSetting(
                L"replacementSuppressionApps[%d].gracePeriodMs", i),
            kMaxReplacementGraceMs);

        ++appCount;
    }

    AcquireSRWLockExclusive(&g_settingsLock);

    g_replacementAppCount = appCount;
    for (size_t i = 0; i < kMaxReplacementApps; ++i) {
        g_replacementApps[i] =
            i < appCount ? apps[i] : ReplacementAppPolicy{};
    }

    ReleaseSRWLockExclusive(&g_settingsLock);

    Wh_Log(L"Settings: transient replacement apps=%llu",
           static_cast<unsigned long long>(appCount));
}

// Resolves the executable path with only the access required for identity
// checks. Called only for the activation candidate that is about to be queued.
static bool QueryProcessImagePath(DWORD pid,
                                  wchar_t* imagePath,
                                  DWORD imagePathCount) {
    if (!pid || !imagePath || imagePathCount == 0) {
        return false;
    }

    imagePath[0] = L'\0';

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    DWORD imagePathLength = imagePathCount;
    const bool queried =
        QueryFullProcessImageNameW(process, 0, imagePath, &imagePathLength) !=
        FALSE;

    CloseHandle(process);
    return queried && imagePath[0] != L'\0';
}

static DWORD GetReplacementGracePeriod(DWORD pid) {
    if (!pid) {
        return 0;
    }

    wchar_t imagePath[32768] = {};
    if (!QueryProcessImagePath(pid, imagePath, ARRAYSIZE(imagePath))) {
        return 0;
    }

    const wchar_t* executable = wcsrchr(imagePath, L'\\');
    executable = executable ? executable + 1 : imagePath;

    DWORD gracePeriodMs = 0;

    AcquireSRWLockShared(&g_settingsLock);

    for (size_t i = 0; i < g_replacementAppCount; ++i) {
        if (_wcsicmp(executable, g_replacementApps[i].executable) == 0 ||
            _wcsicmp(imagePath, g_replacementApps[i].executable) == 0) {
            gracePeriodMs = g_replacementApps[i].gracePeriodMs;
            break;
        }
    }

    ReleaseSRWLockShared(&g_settingsLock);
    return gracePeriodMs;
}

// Process-lifetime cancellation used to keep late shell-host initialization,
// COM startup, and worker waits from racing Windhawk unload.
static HANDLE g_runtimeCancelEvent = nullptr;  // manual-reset
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_startUnsupported{false};
static std::atomic<HRESULT> g_startHr{E_PENDING};

static void ResetStartResult() {
    g_startHr.store(E_PENDING, std::memory_order_relaxed);
    g_startUnsupported.store(false, std::memory_order_release);
}

static void PublishStartResult(bool unsupported,
                               HRESULT hr,
                               HANDLE readyEvent) {
    g_startHr.store(hr, std::memory_order_relaxed);
    g_startUnsupported.store(unsupported, std::memory_order_release);

    if (readyEvent) {
        SetEvent(readyEvent);
    }
}

static bool RuntimeCancellationRequested() {
    return g_runtimeCancelEvent &&
           WaitForSingleObject(g_runtimeCancelEvent, 0) == WAIT_OBJECT_0;
}

static bool RecordStartFailure(HRESULT hr) {
    g_startHr.store(hr, std::memory_order_relaxed);
    return false;
}

// -----------------------------------------------------------------------------
// Helpers.
// -----------------------------------------------------------------------------

template <typename T>
static T GetVTableFunction(void* object, int index) {
    return reinterpret_cast<T>((*reinterpret_cast<void***>(object))[index]);
}

static bool GuidEqual(const GUID& a, const GUID& b) {
    return IsEqualGUID(a, b) != FALSE;
}

static void GuidToString(const GUID& guid, wchar_t* buffer, int bufferCount) {
    if (!buffer || bufferCount <= 0) {
        return;
    }

    if (!StringFromGUID2(guid, buffer, bufferCount)) {
        buffer[0] = L'\0';
    }
}

static SRWLOCK g_currentDesktopLock = SRWLOCK_INIT;
static GUID g_currentDesktopId = {};
static bool g_currentDesktopValid = false;

static void StoreCurrentDesktopId(const GUID& id) {
    AcquireSRWLockExclusive(&g_currentDesktopLock);
    g_currentDesktopId = id;
    g_currentDesktopValid = true;
    ReleaseSRWLockExclusive(&g_currentDesktopLock);
}

static void ClearCurrentDesktopId() {
    AcquireSRWLockExclusive(&g_currentDesktopLock);
    g_currentDesktopId = {};
    g_currentDesktopValid = false;
    ReleaseSRWLockExclusive(&g_currentDesktopLock);
}

static bool LoadCurrentDesktopId(GUID* id) {
    if (!id) {
        return false;
    }

    AcquireSRWLockShared(&g_currentDesktopLock);
    bool valid = g_currentDesktopValid;
    if (valid) {
        *id = g_currentDesktopId;
    }
    ReleaseSRWLockShared(&g_currentDesktopLock);

    return valid;
}

static bool IsActivationWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if (!(style & WS_VISIBLE) || (style & WS_CHILD)) {
        return false;
    }

    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }

    // Same basic top-level-window filter used by Virtual Desktop Helper.
    if (GetAncestor(hwnd, GA_ROOTOWNER) != hwnd) {
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// Foreground-policy and navigation attribution.
// -----------------------------------------------------------------------------

// Cross-desktop activation switches were probed to originate synchronously
// inside CVirtualDesktopForegroundPolicy::ForegroundViewChanged. Deliberate
// low-level switches (including Win+Ctrl+Left/Right when Disable Virtual
// Desktop Transition Animation is installed) enter SwitchDesktopInternal with
// this depth at zero.
//
// This is the primary blast-radius guard: unknown SwitchDesktopInternal callers
// are always allowed. Only a positively attributed foreground-policy switch is
// eligible for redirect.
static thread_local int g_foregroundPolicyDepth = 0;

// Exact window identity supplied by the active foreground-policy view.
//
// The HWND is resolved only for the symbol-identified CWin32ApplicationView and
// CWinRTApplicationView implementations, using their symbol-resolved
// v_GetNativeWindow methods. Unknown IApplicationView implementations fail
// open.
//
// Keep the plain HWND plus an opaque IApplicationView identity in TLS.
// The raw pointer is never dereferenced outside the synchronous FVP stack; it
// is copied into pending-animation metadata only so a later OnViewAddedInternal callback
// can prove that a different shell view was created.
static thread_local HWND g_foregroundPolicyHwnd = nullptr;
static thread_local IApplicationView* g_foregroundPolicyViewIdentity = nullptr;

static const wchar_t* GuidToStringForLog(const GUID& guid);

// CVirtualDesktopForegroundPolicy::DesktopChanged is downstream of the
// navigation input mechanism. It is reached for an actual shell desktop change
// whether navigation originated from Task View, a hotkey, a touchpad gesture,
// Virtual Desktop Helper, or another shell/API route. Each outermost
// DesktopChanged call therefore owns one navigation generation. Activation requests
// and foreground-policy callbacks snapshot the generation so newer navigation
// invalidates older work even if the user later returns to the same desktop.
static std::atomic<uint64_t> g_navigationGeneration{1};

// DesktopChanged synchronously calls
// GetNewForegroundViewForDesktopSwitch before invoking
// IApplicationView::SetFocus on the selected view. Keep its generation in TLS
// so the nested hook can bind that exact view to the actual desktop-change
// event without relying on the particular SwitchDesktop* route used to initiate
// navigation.
static thread_local int g_desktopChangedDepth = 0;
static thread_local uint64_t g_activeNavigationGeneration = 0;

// Navigation-selected focus records are semantic causality tokens, not timers.
// Multiple records are required: testing reproduced a newer DesktopChanged
// completing before ForegroundViewChanged arrived for an older selected view.
// A single slot could therefore be overwritten before the delayed FVP consumes
// it. The IApplicationView pointer is stored only as opaque identity; a later
// FVP must also match the HWND and PID/TID, guarding against pointer reuse.
struct NavigationFocusRecord {
    bool valid = false;
    uint64_t sequence = 0;
    uint64_t generation = 0;
    ULONGLONG recordedAt = 0;
    GUID desktopId = {};
    IApplicationView* viewIdentity = nullptr;
    HWND hwnd = nullptr;
    DWORD pid = 0;
    DWORD tid = 0;
};

static constexpr size_t kNavigationFocusHistoryCapacity = 32;
// Garbage-collection lifetime only. Causal freshness for an older-generation
// token is checked separately: an old token may classify an FVP only while the
// immediately following DesktopChanged is actively executing on this thread.
static constexpr ULONGLONG kNavigationFocusRecordMaxAgeMs = 2000;
static SRWLOCK g_navigationFocusLock = SRWLOCK_INIT;
static NavigationFocusRecord
    g_navigationFocusHistory[kNavigationFocusHistoryCapacity] = {};
static size_t g_navigationFocusWriteIndex = 0;
static std::atomic<uint64_t> g_nextNavigationFocusSequence{1};

// FVP entry consumes at most one matching navigation-selected focus record and
// keeps that attribution synchronously in TLS for the nested
// SwitchDesktopInternal call. Nested FVP calls save/restore the snapshot.
static thread_local uint64_t g_foregroundPolicyNavigationGeneration = 0;
static thread_local NavigationFocusRecord g_foregroundPolicyNavigationFocus =
    {};

// QueueAnimationRequest reserves work while nested under ForegroundViewChanged, but the
// worker isn't released until that exact outer shell call returns. Nested FVP
// calls save/restore this sequence like the other attribution TLS.
static thread_local uint64_t g_foregroundPolicyRequestSequence = 0;

static void DiscardNavigationFocusGeneration(uint64_t generation) {
    if (!generation) {
        return;
    }

    AcquireSRWLockExclusive(&g_navigationFocusLock);

    for (auto& record : g_navigationFocusHistory) {
        if (record.valid && record.generation == generation) {
            record = {};
        }
    }

    ReleaseSRWLockExclusive(&g_navigationFocusLock);
}

static void RecordNavigationSelectedFocus(uint64_t generation,
                                          const GUID& desktopId,
                                          IApplicationView* view,
                                          HWND hwnd) {
    if (!generation || !view || !hwnd || !IsWindow(hwnd)) {
        return;
    }

    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);

    if (!pid || !tid) {
        return;
    }

    NavigationFocusRecord record;
    record.valid = true;
    record.sequence =
        g_nextNavigationFocusSequence.fetch_add(1, std::memory_order_relaxed);
    record.generation = generation;
    record.recordedAt = GetTickCount64();
    record.desktopId = desktopId;
    record.viewIdentity = view;
    record.hwnd = hwnd;
    record.pid = pid;
    record.tid = tid;

    AcquireSRWLockExclusive(&g_navigationFocusLock);

    g_navigationFocusHistory[g_navigationFocusWriteIndex] = record;

    g_navigationFocusWriteIndex =
        (g_navigationFocusWriteIndex + 1) % kNavigationFocusHistoryCapacity;

    ReleaseSRWLockExclusive(&g_navigationFocusLock);

    Wh_Log(
        L"Navigation focus #%llu recorded generation=%llu "
        L"desktop=%s view=%p hwnd=%p pid=%lu tid=%lu",
        static_cast<unsigned long long>(record.sequence),
        static_cast<unsigned long long>(record.generation),
        GuidToStringForLog(record.desktopId), record.viewIdentity, record.hwnd,
        record.pid, record.tid);
}

static bool ConsumeNavigationSelectedFocus(
    IApplicationView* view,
    HWND hwnd,
    NavigationFocusRecord* matchedRecord) {
    if (matchedRecord) {
        *matchedRecord = {};
    }

    if (!view || !hwnd || !IsWindow(hwnd)) {
        return false;
    }

    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);

    if (!pid || !tid) {
        return false;
    }

    size_t matchedIndex = kNavigationFocusHistoryCapacity;
    NavigationFocusRecord matched = {};

    const ULONGLONG now = GetTickCount64();

    AcquireSRWLockExclusive(&g_navigationFocusLock);

    // Consume the oldest still-pending focus selection for this exact view.
    // If the same view was selected by several rapid navigations, suppressing
    // an older delivery is always safe: the depth-zero navigation itself has
    // already performed the actual desktop transition. A later delivery can
    // consume the newer record.
    for (size_t i = 0; i < kNavigationFocusHistoryCapacity; ++i) {
        NavigationFocusRecord& record = g_navigationFocusHistory[i];

        if (!record.valid) {
            continue;
        }

        if (!record.recordedAt ||
            now - record.recordedAt > kNavigationFocusRecordMaxAgeMs) {
            record = {};
            continue;
        }

        if (record.viewIdentity != view) {
            continue;
        }

        if (record.hwnd != hwnd || record.pid != pid || record.tid != tid) {
            // The opaque view address or HWND identity was recycled. Discard
            // the stale token rather than letting it classify anything.
            record = {};
            continue;
        }

        const uint64_t currentGeneration =
            g_navigationGeneration.load(std::memory_order_acquire);

        if (record.generation > currentGeneration) {
            record = {};
            continue;
        }

        if (record.generation < currentGeneration) {
            const bool delayedFromImmediatelyPreviousNavigation =
                g_activeNavigationGeneration == currentGeneration &&
                record.generation + 1 == currentGeneration;

            if (!delayedFromImmediatelyPreviousNavigation) {
                // The same shell view can be activated again later. Once the
                // newer navigation has completed, an orphaned old token must
                // not classify that fresh activation as stale navigation.
                record = {};
                continue;
            }
        }

        if (!matched.valid || record.generation < matched.generation ||
            (record.generation == matched.generation &&
             record.sequence < matched.sequence)) {
            matched = record;
            matchedIndex = i;
        }
    }

    if (matchedIndex < kNavigationFocusHistoryCapacity) {
        g_navigationFocusHistory[matchedIndex] = {};
    }

    ReleaseSRWLockExclusive(&g_navigationFocusLock);

    if (!matched.valid) {
        return false;
    }

    if (matchedRecord) {
        *matchedRecord = matched;
    }

    Wh_Log(
        L"Navigation focus #%llu consumed by FVP "
        L"generation=%llu desktop=%s view=%p hwnd=%p",
        static_cast<unsigned long long>(matched.sequence),
        static_cast<unsigned long long>(matched.generation),
        GuidToStringForLog(matched.desktopId), matched.viewIdentity,
        matched.hwnd);

    return true;
}

static void InvalidateNavigationState(const wchar_t* reason) {
    const uint64_t generation =
        g_navigationGeneration.fetch_add(1, std::memory_order_acq_rel) + 1;

    AcquireSRWLockExclusive(&g_navigationFocusLock);

    for (auto& record : g_navigationFocusHistory) {
        record = {};
    }

    g_navigationFocusWriteIndex = 0;

    ReleaseSRWLockExclusive(&g_navigationFocusLock);

    Wh_Log(L"Navigation state invalidated generation=%llu reason=%s",
           static_cast<unsigned long long>(generation),
           reason ? reason : L"<unknown>");
}

// Formatting helper used only by diagnostics/logging.

static const wchar_t* GuidToStringForLog(const GUID& guid) {
    // A ring keeps multiple GUID arguments in one Wh_Log call distinct. Since
    // this helper is called only from Wh_Log arguments, formatting is skipped
    // entirely when logging is disabled.
    static thread_local wchar_t buffers[8][64];
    static thread_local size_t nextBuffer = 0;

    wchar_t* buffer = buffers[nextBuffer++ % ARRAYSIZE(buffers)];
    GuidToString(guid, buffer, ARRAYSIZE(buffers[0]));
    return buffer[0] ? buffer : L"<invalid>";
}

// -----------------------------------------------------------------------------
// Symbol-resolved shell functions.
// -----------------------------------------------------------------------------

using SwitchDesktopInternal_t = HRESULT (*)(void* pThis,
                                            IVirtualDesktop* desktop);

static SwitchDesktopInternal_t g_switchDesktopInternalOriginal = nullptr;

using ForegroundViewChanged_t =
    HRESULT (*)(void* pThis,
                IVirtualDesktopManagerPrivate* manager,
                IVirtualDesktopSwitchAnimator* animator,
                IApplicationView* view);

static ForegroundViewChanged_t g_foregroundViewChangedOriginal = nullptr;

static void* g_win32ApplicationViewVtable = nullptr;
static void* g_winRtApplicationViewVtable = nullptr;

using ApplicationViewGetNativeWindow_t = HRESULT(WINAPI*)(void* pThis,
                                                          HWND* windowHandle);

static ApplicationViewGetNativeWindow_t g_win32ApplicationViewGetNativeWindow =
    nullptr;
static ApplicationViewGetNativeWindow_t g_winRtApplicationViewGetNativeWindow =
    nullptr;

using DesktopChanged_t = HRESULT (*)(void* pThis, IVirtualDesktop* desktop);

static DesktopChanged_t g_desktopChangedOriginal = nullptr;

using GetNewForegroundViewForDesktopSwitch_t =
    HRESULT (*)(void* pThis,
                IVirtualDesktop* desktop,
                IApplicationView** view,
                bool* shouldFocus);

static GetNewForegroundViewForDesktopSwitch_t
    g_getNewForegroundViewForDesktopSwitchOriginal = nullptr;

using OnViewAddedInternal_t = HRESULT (*)(void* pThis, IApplicationView* view);

static OnViewAddedInternal_t g_onViewAddedInternalOriginal = nullptr;

static std::atomic<bool> g_viewAddedObservationAvailable{false};

static void ObserveForegroundViewForPendingAnimations(IApplicationView* view,
                                                    HWND hwnd);
static void PublishForegroundPolicyRequestCompletion(uint64_t sequence);

static HWND GetForegroundPolicyViewHwnd(IApplicationView* view) {
    if (!view) {
        return nullptr;
    }

    // Only call a native-window accessor after positively identifying the
    // concrete IApplicationView implementation by its symbol-resolved vtable.
    // Unknown view implementations fail open rather than relying on a private
    // slot number which could silently change across shell builds.
    void* vtable = *reinterpret_cast<void**>(view);

    ApplicationViewGetNativeWindow_t getNativeWindow = nullptr;

    if (vtable == g_win32ApplicationViewVtable &&
        g_win32ApplicationViewGetNativeWindow) {
        getNativeWindow = g_win32ApplicationViewGetNativeWindow;
    } else if (vtable == g_winRtApplicationViewVtable &&
               g_winRtApplicationViewGetNativeWindow) {
        getNativeWindow = g_winRtApplicationViewGetNativeWindow;
    } else {
        Wh_Log(
            L"IApplicationView: unknown vtable=%p; "
            L"failing open",
            vtable);
        return nullptr;
    }

    HWND hwnd = nullptr;
    HRESULT hr = getNativeWindow(view, &hwnd);

    if (FAILED(hr)) {
        Wh_Log(
            L"IApplicationView::v_GetNativeWindow failed "
            L"hr=0x%08X",
            static_cast<unsigned int>(hr));
        return nullptr;
    }

    return hwnd;
}

static HRESULT DesktopChanged_Hook(void* pThis, IVirtualDesktop* desktop) {
    const bool outermost = g_desktopChangedDepth == 0;

    const uint64_t previousGeneration = g_activeNavigationGeneration;

    uint64_t generation = previousGeneration;

    if (outermost) {
        generation =
            g_navigationGeneration.fetch_add(1, std::memory_order_acq_rel) + 1;

        g_activeNavigationGeneration = generation;

        GUID desktopId = {};
        const bool desktopIdValid =
            desktop && SUCCEEDED(desktop->GetId(&desktopId));

        Wh_Log(
            L"DesktopChanged navigation generation #%llu begin "
            L"target=%s",
            static_cast<unsigned long long>(generation),
            desktopIdValid ? GuidToStringForLog(desktopId) : L"<unknown>");
    }

    ++g_desktopChangedDepth;

    HRESULT hr = g_desktopChangedOriginal(pThis, desktop);

    --g_desktopChangedDepth;

    if (outermost) {
        if (FAILED(hr)) {
            DiscardNavigationFocusGeneration(generation);
        }

        Wh_Log(
            L"DesktopChanged navigation generation #%llu return "
            L"hr=0x%08X",
            static_cast<unsigned long long>(generation),
            static_cast<unsigned int>(hr));

        g_activeNavigationGeneration = previousGeneration;
    }

    return hr;
}

static HRESULT GetNewForegroundViewForDesktopSwitch_Hook(
    void* pThis,
    IVirtualDesktop* desktop,
    IApplicationView** view,
    bool* shouldFocus) {
    HRESULT hr = g_getNewForegroundViewForDesktopSwitchOriginal(
        pThis, desktop, view, shouldFocus);

    // Only selections made synchronously inside DesktopChanged belong to the
    // shell's own foreground selection for an actual desktop navigation.
    const uint64_t generation = g_activeNavigationGeneration;

    if (!generation || FAILED(hr) || !desktop || !view || !*view ||
        !shouldFocus || !*shouldFocus) {
        return hr;
    }

    GUID desktopId = {};
    if (FAILED(desktop->GetId(&desktopId))) {
        return hr;
    }

    HWND hwnd = GetForegroundPolicyViewHwnd(*view);

    RecordNavigationSelectedFocus(generation, desktopId, *view, hwnd);

    return hr;
}

static HRESULT ForegroundViewChanged_Hook(
    void* pThis,
    IVirtualDesktopManagerPrivate* manager,
    IVirtualDesktopSwitchAnimator* animator,
    IApplicationView* view) {
    HWND previousHwnd = g_foregroundPolicyHwnd;
    IApplicationView* previousViewIdentity = g_foregroundPolicyViewIdentity;
    uint64_t previousNavigationGeneration =
        g_foregroundPolicyNavigationGeneration;
    NavigationFocusRecord previousNavigationFocus =
        g_foregroundPolicyNavigationFocus;
    uint64_t previousRequestSequence = g_foregroundPolicyRequestSequence;

    ++g_foregroundPolicyDepth;
    g_foregroundPolicyRequestSequence = 0;

    g_foregroundPolicyNavigationGeneration =
        g_navigationGeneration.load(std::memory_order_acquire);

    // Resolve the exact view synchronously, while the IApplicationView pointer
    // is valid on this shell call stack. Only plain Win32 identity and the
    // semantic navigation-focus snapshot cross into nested classification.
    g_foregroundPolicyHwnd = GetForegroundPolicyViewHwnd(view);
    g_foregroundPolicyViewIdentity = view;

    ObserveForegroundViewForPendingAnimations(view, g_foregroundPolicyHwnd);

    g_foregroundPolicyNavigationFocus = {};
    ConsumeNavigationSelectedFocus(view, g_foregroundPolicyHwnd,
                                   &g_foregroundPolicyNavigationFocus);

    HRESULT hr =
        g_foregroundViewChangedOriginal(pThis, manager, animator, view);

    const uint64_t completedRequestSequence = g_foregroundPolicyRequestSequence;
    if (completedRequestSequence) {
        PublishForegroundPolicyRequestCompletion(completedRequestSequence);
    }

    g_foregroundPolicyHwnd = previousHwnd;
    g_foregroundPolicyViewIdentity = previousViewIdentity;
    g_foregroundPolicyNavigationGeneration = previousNavigationGeneration;
    g_foregroundPolicyNavigationFocus = previousNavigationFocus;
    g_foregroundPolicyRequestSequence = previousRequestSequence;

    --g_foregroundPolicyDepth;
    return hr;
}

// -----------------------------------------------------------------------------
// Current-desktop cache via the private VD notification service.
// -----------------------------------------------------------------------------

static HANDLE g_notificationThread = nullptr;
static DWORD g_notificationThreadId = 0;
static HANDLE g_notificationReadyEvent = nullptr;
static HANDLE g_notificationStopEvent = nullptr;
static std::atomic<bool> g_notificationReady = false;

static bool NotificationStopRequested() {
    return RuntimeCancellationRequested() ||
           (g_notificationStopEvent &&
            WaitForSingleObject(g_notificationStopEvent, 0) == WAIT_OBJECT_0);
}

class VirtualDesktopNotificationListener final
    : public IVirtualDesktopNotification {
   public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                             void** object) override {
        if (!object) {
            return E_POINTER;
        }

        *object = nullptr;

        if (IsEqualIID(riid, __uuidof(IUnknown)) ||
            IsEqualIID(riid, kIidVirtualDesktopNotification)) {
            *object = static_cast<IVirtualDesktopNotification*>(this);
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override { return ++m_refCount; }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG value = --m_refCount;
        if (!value) {
            delete this;
        }
        return value;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(IVirtualDesktop*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    VirtualDesktopDestroyBegin(IVirtualDesktop*, IVirtualDesktop*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    VirtualDesktopDestroyFailed(IVirtualDesktop*, IVirtualDesktop*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    VirtualDesktopDestroyed(IVirtualDesktop*, IVirtualDesktop*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopMoved(IVirtualDesktop*,
                                                  INT64,
                                                  INT64) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopNameChanged(IVirtualDesktop*,
                                                        void*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(IUnknown*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    CurrentVirtualDesktopChanged(IVirtualDesktop* desktopOld,
                                 IVirtualDesktop* desktopNew) override {
        GUID oldId = {};
        GUID newId = {};

        HRESULT oldHr = desktopOld ? desktopOld->GetId(&oldId) : E_POINTER;
        HRESULT newHr = desktopNew ? desktopNew->GetId(&newId) : E_POINTER;

        if (SUCCEEDED(newHr)) {
            StoreCurrentDesktopId(newId);

            Wh_Log(L"Current desktop cache updated old=%s new=%s",
                   SUCCEEDED(oldHr) ? GuidToStringForLog(oldId) : L"<unknown>",
                   GuidToStringForLog(newId));
        } else {
            ClearCurrentDesktopId();
            Wh_Log(
                L"Current desktop cache invalidated: "
                L"desktopNew->GetId failed hr=0x%08X",
                static_cast<unsigned int>(newHr));
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopWallpaperChanged(IVirtualDesktop*,
                                                             void*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    VirtualDesktopSwitched(IVirtualDesktop*, int) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    RemoteVirtualDesktopConnected(IVirtualDesktop*) override {
        return S_OK;
    }

   private:
    std::atomic<ULONG> m_refCount{1};
};

static DWORD WINAPI NotificationThreadProc(void*) {
    // Own a message queue before anything that can block so a concurrent stop
    // can always post WM_QUIT. The durable stop event covers the earlier race.
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    if (NotificationStopRequested()) {
        PublishStartResult(false, HRESULT_FROM_WIN32(ERROR_CANCELLED),
                           g_notificationReadyEvent);
        return 0;
    }

    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    const bool shouldUninitialize = SUCCEEDED(coHr);

    if (FAILED(coHr) && coHr != RPC_E_CHANGED_MODE) {
        Wh_Log(
            L"Notification CoInitializeEx failed "
            L"hr=0x%08X",
            static_cast<unsigned int>(coHr));

        PublishStartResult(false, coHr, g_notificationReadyEvent);
        return 0;
    }

    if (NotificationStopRequested()) {
        PublishStartResult(false, HRESULT_FROM_WIN32(ERROR_CANCELLED),
                           g_notificationReadyEvent);

        if (shouldUninitialize) {
            CoUninitialize();
        }

        return 0;
    }

    IServiceProvider* serviceProvider = nullptr;
    IVirtualDesktopManagerInternal* managerInternal = nullptr;
    IVirtualDesktopNotificationService* notificationService = nullptr;
    VirtualDesktopNotificationListener* listener = nullptr;
    DWORD cookie = 0;
    bool unsupported = false;

    HRESULT hr = CoCreateInstance(
        kClsidImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
        __uuidof(IServiceProvider), reinterpret_cast<void**>(&serviceProvider));

    if (SUCCEEDED(hr) && serviceProvider) {
        hr = serviceProvider->QueryService(
            kClsidVirtualDesktopManagerInternal,
            kIidVirtualDesktopManagerInternal24H2,
            reinterpret_cast<void**>(&managerInternal));

        if (SUCCEEDED(hr) && !managerInternal) {
            hr = E_NOINTERFACE;
        }

        unsupported = hr == E_NOINTERFACE;
    }

    if (SUCCEEDED(hr) && serviceProvider) {
        hr = serviceProvider->QueryService(
            kServiceVirtualDesktopNotification,
            kIidVirtualDesktopNotificationService,
            reinterpret_cast<void**>(&notificationService));

        if (SUCCEEDED(hr) && !notificationService) {
            hr = E_NOINTERFACE;
        }

        unsupported = unsupported || hr == E_NOINTERFACE;
    }

    // Register before seeding the cache so a desktop change during GetCurrent
    // cannot fall into a gap between the seed and notification subscription.
    if (SUCCEEDED(hr) && notificationService) {
        listener = new VirtualDesktopNotificationListener();

        hr = notificationService->Register(listener, &cookie);
    }

    if (SUCCEEDED(hr) && managerInternal) {
        auto getCurrentDesktop = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(
            void*, IVirtualDesktop**)>(managerInternal,
                                       kVtableGetCurrentDesktop);

        IVirtualDesktop* currentDesktop = nullptr;

        HRESULT currentHr = getCurrentDesktop(managerInternal, &currentDesktop);

        if (SUCCEEDED(currentHr) && currentDesktop) {
            GUID id = {};
            if (SUCCEEDED(currentDesktop->GetId(&id))) {
                StoreCurrentDesktopId(id);
            }

            currentDesktop->Release();
        }
    }

    if (SUCCEEDED(hr) && cookie) {
        GUID currentId = {};
        if (LoadCurrentDesktopId(&currentId)) {
            g_notificationReady.store(true, std::memory_order_release);
        } else {
            hr = E_FAIL;
        }
    }

    if (NotificationStopRequested()) {
        g_notificationReady.store(false, std::memory_order_release);
        hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
    } else if (g_notificationReady.load(std::memory_order_acquire)) {
        hr = S_OK;
    } else if (SUCCEEDED(hr)) {
        hr = E_FAIL;
    }

    Wh_Log(
        L"Notification/cache init hr=0x%08X "
        L"cookie=%lu ready=%d",
        static_cast<unsigned int>(hr), cookie,
        g_notificationReady.load(std::memory_order_acquire));

    PublishStartResult(unsupported, hr, g_notificationReadyEvent);

    if (g_notificationReady.load(std::memory_order_acquire) &&
        !NotificationStopRequested()) {
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    g_notificationReady.store(false, std::memory_order_release);
    ClearCurrentDesktopId();

    if (notificationService && cookie) {
        notificationService->Unregister(cookie);
    }

    if (listener) {
        listener->Release();
    }

    if (notificationService) {
        notificationService->Release();
    }

    if (managerInternal) {
        managerInternal->Release();
    }

    if (serviceProvider) {
        serviceProvider->Release();
    }

    if (shouldUninitialize) {
        CoUninitialize();
    }

    Wh_Log(L"Notification thread exiting");
    return 0;
}

static bool StartNotificationCache() {
    g_notificationReady.store(false, std::memory_order_release);
    ResetStartResult();

    if (RuntimeCancellationRequested()) {
        return RecordStartFailure(HRESULT_FROM_WIN32(ERROR_CANCELLED));
    }

    g_notificationReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    g_notificationStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_notificationReadyEvent || !g_notificationStopEvent) {
        Wh_Log(L"Failed to create notification events");
        return RecordStartFailure(E_FAIL);
    }

    g_notificationThread = CreateThread(nullptr, 0, NotificationThreadProc,
                                        nullptr, 0, &g_notificationThreadId);

    if (!g_notificationThread) {
        DWORD error = GetLastError();

        Wh_Log(
            L"Notification CreateThread failed "
            L"error=%lu",
            error);

        return RecordStartFailure(HRESULT_FROM_WIN32(error));
    }

    HANDLE waits[] = {
        g_runtimeCancelEvent,
        g_notificationReadyEvent,
    };

    DWORD waitResult =
        WaitForMultipleObjects(ARRAYSIZE(waits), waits, FALSE, 5000);

    if (waitResult == WAIT_OBJECT_0) {
        return RecordStartFailure(HRESULT_FROM_WIN32(ERROR_CANCELLED));
    }

    if (waitResult == WAIT_OBJECT_0 + 1) {
        return g_notificationReady.load(std::memory_order_acquire);
    }

    HRESULT waitHr = waitResult == WAIT_TIMEOUT
                         ? HRESULT_FROM_WIN32(ERROR_TIMEOUT)
                         : HRESULT_FROM_WIN32(GetLastError());

    Wh_Log(
        L"Notification/current-desktop cache "
        L"unavailable (waitResult=%lu "
        L"hr=0x%08X)",
        waitResult, static_cast<unsigned int>(waitHr));

    return RecordStartFailure(waitHr);
}

static void StopNotificationCache() {
    g_notificationReady.store(false, std::memory_order_release);

    if (g_notificationStopEvent) {
        SetEvent(g_notificationStopEvent);
    }

    if (g_notificationThread) {
        if (g_notificationThreadId) {
            PostThreadMessageW(g_notificationThreadId, WM_QUIT, 0, 0);
        }

        WaitForSingleObject(g_notificationThread, INFINITE);

        CloseHandle(g_notificationThread);
        g_notificationThread = nullptr;
        g_notificationThreadId = 0;
    }

    if (g_notificationReadyEvent) {
        CloseHandle(g_notificationReadyEvent);
        g_notificationReadyEvent = nullptr;
    }

    if (g_notificationStopEvent) {
        CloseHandle(g_notificationStopEvent);
        g_notificationStopEvent = nullptr;
    }

    ClearCurrentDesktopId();
}

// -----------------------------------------------------------------------------
// Deferred animated activation worker.
// -----------------------------------------------------------------------------

struct AnimationRequest {
    uint64_t sequence = 0;
    uint64_t navigationGeneration = 0;
    bool foregroundPolicyReturned = false;
    ULONGLONG foregroundPolicyReturnedAt = 0;

    DWORD replacementGraceMs = 0;

    IApplicationView* viewIdentity = nullptr;  // opaque, never dereferenced
    HWND hwnd = nullptr;
    DWORD pid = 0;
    DWORD tid = 0;

    GUID sourceDesktopId = {};
    GUID targetDesktopId = {};

    IApplicationView* replacementViewIdentity = nullptr;  // opaque
    HWND replacementHwnd = nullptr;
    bool replacementForegroundConfirmed = false;
};

static constexpr size_t kAnimationQueueCapacity = 16;
static SRWLOCK g_requestLock = SRWLOCK_INIT;
static AnimationRequest g_animationQueue[kAnimationQueueCapacity] = {};
static size_t g_animationQueueHead = 0;
static size_t g_animationQueueCount = 0;
static AnimationRequest g_activeAnimationRequest = {};
static bool g_activeAnimationRequestValid = false;
static std::atomic<uint64_t> g_nextSequence{1};

static HANDLE g_requestEvent = nullptr;      // auto-reset
static HANDLE g_replacementEvent = nullptr;  // auto-reset
static HANDLE g_stopEvent = nullptr;         // manual-reset
static HANDLE g_workerThread = nullptr;
static HANDLE g_workerReadyEvent = nullptr;
static std::atomic<bool> g_workerReady{false};

static bool WorkerStopRequested() {
    return RuntimeCancellationRequested() ||
           (g_stopEvent &&
            WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0);
}

struct WorkerComState {
    IServiceProvider* serviceProvider = nullptr;
    IVirtualDesktopManagerInternal* managerInternal = nullptr;
    IApplicationViewCollection* viewCollection = nullptr;
    IVirtualDesktopManager* publicManager = nullptr;
};

static void ReleaseWorkerComState(WorkerComState* state) {
    if (!state) {
        return;
    }

    if (state->publicManager) {
        state->publicManager->Release();
        state->publicManager = nullptr;
    }

    if (state->viewCollection) {
        state->viewCollection->Release();
        state->viewCollection = nullptr;
    }

    if (state->managerInternal) {
        state->managerInternal->Release();
        state->managerInternal = nullptr;
    }

    if (state->serviceProvider) {
        state->serviceProvider->Release();
        state->serviceProvider = nullptr;
    }
}

static bool InitializeWorkerComState(WorkerComState* state) {
    HRESULT hr = CoCreateInstance(
        kClsidImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
        __uuidof(IServiceProvider),
        reinterpret_cast<void**>(&state->serviceProvider));

    if (FAILED(hr) || !state->serviceProvider) {
        if (SUCCEEDED(hr)) {
            hr = E_FAIL;
        }
        Wh_Log(L"Worker: ImmersiveShell creation failed hr=0x%08X",
               static_cast<unsigned int>(hr));
        g_startHr.store(hr, std::memory_order_relaxed);
        return false;
    }

    hr = state->serviceProvider->QueryService(
        kClsidVirtualDesktopManagerInternal,
        kIidVirtualDesktopManagerInternal24H2,
        reinterpret_cast<void**>(&state->managerInternal));

    if (FAILED(hr) || !state->managerInternal) {
        if (SUCCEEDED(hr)) {
            hr = E_NOINTERFACE;
        }
        Wh_Log(L"Worker: virtual desktop manager unavailable hr=0x%08X",
               static_cast<unsigned int>(hr));
        g_startHr.store(hr, std::memory_order_relaxed);
        g_startUnsupported.store(hr == E_NOINTERFACE,
                                 std::memory_order_release);
        return false;
    }

    hr = state->serviceProvider->QueryService(
        kIidApplicationViewCollection, kIidApplicationViewCollection,
        reinterpret_cast<void**>(&state->viewCollection));

    if (FAILED(hr) || !state->viewCollection) {
        if (SUCCEEDED(hr)) {
            hr = E_NOINTERFACE;
        }
        Wh_Log(L"Worker: application view collection unavailable hr=0x%08X",
               static_cast<unsigned int>(hr));
        g_startHr.store(hr, std::memory_order_relaxed);
        g_startUnsupported.store(hr == E_NOINTERFACE,
                                 std::memory_order_release);
        return false;
    }

    hr = CoCreateInstance(__uuidof(VirtualDesktopManager), nullptr,
                          CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&state->publicManager));

    if (FAILED(hr) || !state->publicManager) {
        if (SUCCEEDED(hr)) {
            hr = E_FAIL;
        }
        Wh_Log(L"Worker: public virtual desktop manager unavailable hr=0x%08X",
               static_cast<unsigned int>(hr));
        g_startHr.store(hr, std::memory_order_relaxed);
        return false;
    }

    return true;
}

static HRESULT WorkerGetCurrentDesktop(WorkerComState* state,
                                       IVirtualDesktop** desktop) {
    auto fn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(
        void*, IVirtualDesktop**)>(state->managerInternal,
                                   kVtableGetCurrentDesktop);
    return fn(state->managerInternal, desktop);
}

static HRESULT WorkerSwitchDesktopWithAnimation(WorkerComState* state,
                                                IVirtualDesktop* desktop) {
    if (!state || !state->managerInternal || !desktop) {
        return E_POINTER;
    }

    auto fn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(
        void*, IVirtualDesktop*)>(state->managerInternal,
                                  kVtableSwitchDesktopWithAnimation);
    return fn(state->managerInternal, desktop);
}

static HRESULT WorkerWaitForAnimationToComplete(WorkerComState* state) {
    if (!state || !state->managerInternal) {
        return E_POINTER;
    }

    auto fn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*)>(
        state->managerInternal, kVtableWaitForAnimationToComplete);
    return fn(state->managerInternal);
}

static HRESULT WorkerGetDesktops(WorkerComState* state,
                                 IObjectArray** desktops) {
    if (!state || !state->managerInternal || !desktops) {
        return E_POINTER;
    }

    *desktops = nullptr;
    auto fn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(
        void*, IObjectArray**)>(state->managerInternal, kVtableGetDesktops);
    return fn(state->managerInternal, desktops);
}

static IVirtualDesktop* WorkerFindDesktopById(WorkerComState* state,
                                              const GUID& desktopId) {
    IObjectArray* desktops = nullptr;
    HRESULT hr = WorkerGetDesktops(state, &desktops);
    if (FAILED(hr) || !desktops) {
        return nullptr;
    }

    UINT count = 0;
    hr = desktops->GetCount(&count);
    if (FAILED(hr)) {
        desktops->Release();
        return nullptr;
    }

    IVirtualDesktop* found = nullptr;
    for (UINT i = 0; i < count; ++i) {
        IVirtualDesktop* desktop = nullptr;
        hr = desktops->GetAt(i, kIidVirtualDesktop24H2,
                             reinterpret_cast<void**>(&desktop));
        if (FAILED(hr) || !desktop) {
            continue;
        }

        GUID id = {};
        if (SUCCEEDED(desktop->GetId(&id)) && GuidEqual(id, desktopId)) {
            found = desktop;
            break;
        }

        desktop->Release();
    }

    desktops->Release();
    return found;
}

static bool IsAnimationGenerationCurrent(const AnimationRequest& request) {
    return request.navigationGeneration ==
           g_navigationGeneration.load(std::memory_order_acquire);
}

static bool CurrentDesktopMatches(WorkerComState* state,
                                  const GUID& expectedDesktopId) {
    IVirtualDesktop* currentDesktop = nullptr;
    HRESULT hr = WorkerGetCurrentDesktop(state, &currentDesktop);

    GUID currentId = {};
    const bool matches =
        SUCCEEDED(hr) && currentDesktop &&
        SUCCEEDED(currentDesktop->GetId(&currentId)) &&
        GuidEqual(currentId, expectedDesktopId);

    if (currentDesktop) {
        currentDesktop->Release();
    }

    return matches;
}

static bool ValidateReplacementCandidate(WorkerComState* state,
                                         const AnimationRequest& request,
                                         IApplicationView* viewIdentity,
                                         HWND hwnd) {
    if (!state || !viewIdentity || !hwnd || !IsWindow(hwnd) ||
        viewIdentity == request.viewIdentity || hwnd == request.hwnd) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || pid != request.pid) {
        return false;
    }

    GUID desktopId = {};
    HRESULT hr = state->publicManager->GetWindowDesktopId(hwnd, &desktopId);
    if (FAILED(hr) || !GuidEqual(desktopId, request.sourceDesktopId)) {
        return false;
    }

    IApplicationView* resolvedView = nullptr;
    hr = state->viewCollection->GetViewForHwnd(hwnd, &resolvedView);
    if (FAILED(hr) || !resolvedView) {
        return false;
    }

    resolvedView->Release();
    return true;
}

static bool GetConfirmedReplacement(uint64_t sequence,
                                    IApplicationView** viewIdentity,
                                    HWND* hwnd) {
    if (viewIdentity) {
        *viewIdentity = nullptr;
    }
    if (hwnd) {
        *hwnd = nullptr;
    }

    bool found = false;
    AcquireSRWLockShared(&g_requestLock);

    if (g_activeAnimationRequestValid &&
        g_activeAnimationRequest.sequence == sequence &&
        g_activeAnimationRequest.replacementViewIdentity &&
        g_activeAnimationRequest.replacementHwnd &&
        g_activeAnimationRequest.replacementForegroundConfirmed) {
        if (viewIdentity) {
            *viewIdentity = g_activeAnimationRequest.replacementViewIdentity;
        }
        if (hwnd) {
            *hwnd = g_activeAnimationRequest.replacementHwnd;
        }
        found = true;
    }

    ReleaseSRWLockShared(&g_requestLock);
    return found;
}

// Returns false when a confirmed replacement makes the pending switch stale.
static bool WaitForReplacementGrace(WorkerComState* state,
                                    const AnimationRequest& request) {
    if (!request.replacementGraceMs ||
        !g_viewAddedObservationAvailable.load(std::memory_order_acquire)) {
        return true;
    }

    const ULONGLONG startedAt = request.foregroundPolicyReturnedAt
                                    ? request.foregroundPolicyReturnedAt
                                    : GetTickCount64();

    while (true) {
        if (!IsAnimationGenerationCurrent(request)) {
            return false;
        }

        IApplicationView* replacementView = nullptr;
        HWND replacementHwnd = nullptr;

        bool confirmed = request.replacementViewIdentity &&
                         request.replacementHwnd &&
                         request.replacementForegroundConfirmed;

        if (confirmed) {
            replacementView = request.replacementViewIdentity;
            replacementHwnd = request.replacementHwnd;
        } else {
            confirmed = GetConfirmedReplacement(request.sequence,
                                                &replacementView,
                                                &replacementHwnd);
        }

        if (confirmed &&
            ValidateReplacementCandidate(state, request, replacementView,
                                         replacementHwnd)) {
            Wh_Log(
                L"[#%llu] Suppressed transient cross-desktop activation: "
                L"replacement hwnd=%p became foreground on source desktop",
                static_cast<unsigned long long>(request.sequence),
                replacementHwnd);
            return false;
        }

        const ULONGLONG now = GetTickCount64();
        const ULONGLONG elapsed = now >= startedAt ? now - startedAt : 0;
        if (elapsed >= request.replacementGraceMs) {
            return true;
        }

        const DWORD remaining =
            request.replacementGraceMs - static_cast<DWORD>(elapsed);

        HANDLE waits[] = {
            g_runtimeCancelEvent,
            g_stopEvent,
            g_replacementEvent,
        };

        DWORD waitResult =
            WaitForMultipleObjects(ARRAYSIZE(waits), waits, FALSE, remaining);

        if (waitResult == WAIT_TIMEOUT) {
            return true;
        }

        if (waitResult == WAIT_OBJECT_0 + 2) {
            continue;
        }

        return false;
    }
}

static bool TakePendingAnimationRequest(AnimationRequest* request) {
    if (!request) {
        return false;
    }

    AcquireSRWLockExclusive(&g_requestLock);

    if (!g_animationQueueCount ||
        !g_animationQueue[g_animationQueueHead].foregroundPolicyReturned) {
        ReleaseSRWLockExclusive(&g_requestLock);
        return false;
    }

    *request = g_animationQueue[g_animationQueueHead];
    g_animationQueue[g_animationQueueHead] = {};
    g_animationQueueHead =
        (g_animationQueueHead + 1) % kAnimationQueueCapacity;
    --g_animationQueueCount;

    g_activeAnimationRequest = *request;
    g_activeAnimationRequestValid = true;

    ReleaseSRWLockExclusive(&g_requestLock);
    return true;
}

static void ClearActiveAnimationRequest(uint64_t sequence) {
    AcquireSRWLockExclusive(&g_requestLock);

    if (g_activeAnimationRequestValid &&
        g_activeAnimationRequest.sequence == sequence) {
        g_activeAnimationRequest = {};
        g_activeAnimationRequestValid = false;
    }

    ReleaseSRWLockExclusive(&g_requestLock);
}

static void PublishForegroundPolicyRequestCompletion(uint64_t sequence) {
    if (!sequence) {
        return;
    }

    bool published = false;
    AcquireSRWLockExclusive(&g_requestLock);

    for (size_t i = 0; i < g_animationQueueCount; ++i) {
        size_t index =
            (g_animationQueueHead + i) % kAnimationQueueCapacity;
        AnimationRequest& request = g_animationQueue[index];

        if (request.sequence == sequence) {
            request.foregroundPolicyReturned = true;
            request.foregroundPolicyReturnedAt = GetTickCount64();
            published = true;
            break;
        }
    }

    if (published && g_requestEvent) {
        SetEvent(g_requestEvent);
    }

    ReleaseSRWLockExclusive(&g_requestLock);
}

static void ProcessAnimationRequest(WorkerComState* state,
                                    const AnimationRequest& request) {
    Wh_Log(L"[#%llu] Animated activation begin hwnd=%p target=%s generation=%llu",
           static_cast<unsigned long long>(request.sequence), request.hwnd,
           GuidToStringForLog(request.targetDesktopId),
           static_cast<unsigned long long>(request.navigationGeneration));

    if (!IsAnimationGenerationCurrent(request) ||
        !IsActivationWindow(request.hwnd)) {
        return;
    }

    DWORD currentPid = 0;
    DWORD currentTid = GetWindowThreadProcessId(request.hwnd, &currentPid);
    if (currentPid != request.pid || currentTid != request.tid) {
        return;
    }

    if (!WaitForReplacementGrace(state, request)) {
        return;
    }

    if (!IsAnimationGenerationCurrent(request) ||
        !CurrentDesktopMatches(state, request.sourceDesktopId)) {
        return;
    }

    GUID windowDesktopId = {};
    HRESULT hr = state->publicManager->GetWindowDesktopId(
        request.hwnd, &windowDesktopId);
    if (FAILED(hr) || !GuidEqual(windowDesktopId, request.targetDesktopId)) {
        return;
    }

    // Keep the animation API out of ForegroundViewChanged/SwitchDesktopInternal.
    // It can wait on shell animation state and is safe on this worker apartment.
    hr = WorkerWaitForAnimationToComplete(state);
    if (FAILED(hr)) {
        Wh_Log(L"[#%llu] WaitForAnimationToComplete(pre) failed hr=0x%08X",
               static_cast<unsigned long long>(request.sequence),
               static_cast<unsigned int>(hr));
        return;
    }

    if (!IsAnimationGenerationCurrent(request) ||
        !CurrentDesktopMatches(state, request.sourceDesktopId)) {
        return;
    }

    windowDesktopId = {};
    hr = state->publicManager->GetWindowDesktopId(request.hwnd,
                                                  &windowDesktopId);
    if (FAILED(hr) || !GuidEqual(windowDesktopId, request.targetDesktopId)) {
        return;
    }

    IVirtualDesktop* targetDesktop =
        WorkerFindDesktopById(state, request.targetDesktopId);
    if (!targetDesktop) {
        return;
    }

    // Narrow the final commit race: navigation can change while resolving the
    // target desktop, so validate the request once more immediately before the
    // private animated-switch call.
    if (!IsAnimationGenerationCurrent(request) ||
        !CurrentDesktopMatches(state, request.sourceDesktopId)) {
        targetDesktop->Release();
        return;
    }

    hr = WorkerSwitchDesktopWithAnimation(state, targetDesktop);
    targetDesktop->Release();

    if (FAILED(hr)) {
        Wh_Log(L"[#%llu] SwitchDesktopWithAnimation failed hr=0x%08X",
               static_cast<unsigned long long>(request.sequence),
               static_cast<unsigned int>(hr));
        return;
    }

    Wh_Log(L"[#%llu] Animated activation committed target=%s",
           static_cast<unsigned long long>(request.sequence),
           GuidToStringForLog(request.targetDesktopId));

    hr = WorkerWaitForAnimationToComplete(state);
    if (FAILED(hr)) {
        Wh_Log(L"[#%llu] WaitForAnimationToComplete(post) failed hr=0x%08X",
               static_cast<unsigned long long>(request.sequence),
               static_cast<unsigned int>(hr));
    }
}

static DWORD WINAPI WorkerThreadProc(void*) {
    g_workerReady.store(false, std::memory_order_release);

    HRESULT coHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool shouldUninitialize = SUCCEEDED(coHr);

    if (FAILED(coHr)) {
        g_startHr.store(coHr, std::memory_order_relaxed);
        SetEvent(g_workerReadyEvent);
        return 0;
    }

    if (WorkerStopRequested()) {
        g_startHr.store(HRESULT_FROM_WIN32(ERROR_CANCELLED),
                        std::memory_order_relaxed);
        SetEvent(g_workerReadyEvent);
        if (shouldUninitialize) {
            CoUninitialize();
        }
        return 0;
    }

    WorkerComState state;
    if (!InitializeWorkerComState(&state)) {
        ReleaseWorkerComState(&state);
        if (shouldUninitialize) {
            CoUninitialize();
        }
        SetEvent(g_workerReadyEvent);
        return 0;
    }

    if (WorkerStopRequested()) {
        g_startHr.store(HRESULT_FROM_WIN32(ERROR_CANCELLED),
                        std::memory_order_relaxed);
        ReleaseWorkerComState(&state);
        if (shouldUninitialize) {
            CoUninitialize();
        }
        SetEvent(g_workerReadyEvent);
        return 0;
    }

    g_workerReady.store(true, std::memory_order_release);
    SetEvent(g_workerReadyEvent);
    Wh_Log(L"Animation worker ready");

    HANDLE waits[] = {
        g_runtimeCancelEvent,
        g_stopEvent,
        g_requestEvent,
    };

    while (true) {
        DWORD waitResult =
            WaitForMultipleObjects(ARRAYSIZE(waits), waits, FALSE, INFINITE);

        if (waitResult == WAIT_OBJECT_0 || waitResult == WAIT_OBJECT_0 + 1) {
            break;
        }

        if (waitResult == WAIT_OBJECT_0 + 2) {
            AnimationRequest request;
            while (TakePendingAnimationRequest(&request)) {
                if (WorkerStopRequested()) {
                    ClearActiveAnimationRequest(request.sequence);
                    break;
                }

                ProcessAnimationRequest(&state, request);
                ClearActiveAnimationRequest(request.sequence);

                if (WorkerStopRequested()) {
                    break;
                }
            }
        }
    }

    g_workerReady.store(false, std::memory_order_release);
    ReleaseWorkerComState(&state);

    if (shouldUninitialize) {
        CoUninitialize();
    }

    Wh_Log(L"Animation worker exiting");
    return 0;
}

static bool QueueAnimationRequest(IApplicationView* viewIdentity,
                                  HWND hwnd,
                                  DWORD pid,
                                  DWORD tid,
                                  const GUID& sourceDesktopId,
                                  const GUID& targetDesktopId,
                                  uint64_t navigationGeneration,
                                  DWORD replacementGraceMs) {
    if (g_unloading.load(std::memory_order_acquire)) {
        return false;
    }

    if (g_foregroundPolicyRequestSequence) {
        Wh_Log(L"Animation already reserved in this foreground-policy call; "
               L"failing open");
        return false;
    }

    AnimationRequest request;
    request.sequence =
        g_nextSequence.fetch_add(1, std::memory_order_relaxed);
    request.navigationGeneration = navigationGeneration;
    request.replacementGraceMs = replacementGraceMs;
    request.viewIdentity = replacementGraceMs ? viewIdentity : nullptr;
    request.hwnd = hwnd;
    request.pid = pid;
    request.tid = tid;
    request.sourceDesktopId = sourceDesktopId;
    request.targetDesktopId = targetDesktopId;

    bool queued = false;
    AcquireSRWLockExclusive(&g_requestLock);

    if (!g_unloading.load(std::memory_order_acquire) &&
        g_workerReady.load(std::memory_order_acquire) && g_requestEvent &&
        g_animationQueueCount < kAnimationQueueCapacity) {
        size_t index =
            (g_animationQueueHead + g_animationQueueCount) %
            kAnimationQueueCapacity;
        g_animationQueue[index] = request;
        ++g_animationQueueCount;
        queued = true;
    }

    ReleaseSRWLockExclusive(&g_requestLock);

    if (queued) {
        g_foregroundPolicyRequestSequence = request.sequence;
        Wh_Log(L"[#%llu] Animated activation reserved until FVP returns "
               L"replacementGraceMs=%lu",
               static_cast<unsigned long long>(request.sequence),
               replacementGraceMs);
    } else {
        Wh_Log(L"Animation queue unavailable or full; failing open");
    }

    return queued;
}

static bool AnimationRequestCanObserveReplacement(
    const AnimationRequest& request,
    DWORD pid) {
    return request.replacementGraceMs != 0 && request.viewIdentity != nullptr &&
           request.pid == pid &&
           request.navigationGeneration ==
               g_navigationGeneration.load(std::memory_order_acquire);
}

static bool HasPendingReplacementWatchForPid(DWORD pid) {
    bool found = false;
    AcquireSRWLockShared(&g_requestLock);

    if (g_activeAnimationRequestValid &&
        AnimationRequestCanObserveReplacement(g_activeAnimationRequest, pid)) {
        found = true;
    }

    for (size_t i = 0; !found && i < g_animationQueueCount; ++i) {
        size_t index =
            (g_animationQueueHead + i) % kAnimationQueueCapacity;
        if (AnimationRequestCanObserveReplacement(g_animationQueue[index],
                                                   pid)) {
            found = true;
        }
    }

    ReleaseSRWLockShared(&g_requestLock);
    return found;
}

static bool ReplacementMatchesRequest(const AnimationRequest& request,
                                      IApplicationView* viewIdentity,
                                      HWND hwnd,
                                      DWORD pid) {
    return AnimationRequestCanObserveReplacement(request, pid) &&
           viewIdentity && hwnd && viewIdentity != request.viewIdentity &&
           hwnd != request.hwnd;
}

static void ObserveViewAddedForPendingAnimations(IApplicationView* viewIdentity,
                                                 HWND hwnd) {
    if (g_unloading.load(std::memory_order_acquire) || !viewIdentity || !hwnd ||
        !IsWindow(hwnd)) {
        return;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || !HasPendingReplacementWatchForPid(pid)) {
        return;
    }

    AcquireSRWLockExclusive(&g_requestLock);

    if (g_activeAnimationRequestValid &&
        ReplacementMatchesRequest(g_activeAnimationRequest, viewIdentity, hwnd,
                                  pid)) {
        g_activeAnimationRequest.replacementViewIdentity = viewIdentity;
        g_activeAnimationRequest.replacementHwnd = hwnd;
        g_activeAnimationRequest.replacementForegroundConfirmed = false;
    }

    for (size_t i = 0; i < g_animationQueueCount; ++i) {
        size_t index =
            (g_animationQueueHead + i) % kAnimationQueueCapacity;
        AnimationRequest& request = g_animationQueue[index];

        if (ReplacementMatchesRequest(request, viewIdentity, hwnd, pid)) {
            request.replacementViewIdentity = viewIdentity;
            request.replacementHwnd = hwnd;
            request.replacementForegroundConfirmed = false;
        }
    }

    ReleaseSRWLockExclusive(&g_requestLock);
}

static void ObserveForegroundViewForPendingAnimations(IApplicationView* view,
                                                      HWND hwnd) {
    if (g_unloading.load(std::memory_order_acquire) || !view || !hwnd) {
        return;
    }

    bool signaled = false;
    AcquireSRWLockExclusive(&g_requestLock);

    if (g_activeAnimationRequestValid &&
        g_activeAnimationRequest.replacementViewIdentity == view &&
        g_activeAnimationRequest.replacementHwnd == hwnd) {
        g_activeAnimationRequest.replacementForegroundConfirmed = true;
        signaled = true;
    }

    for (size_t i = 0; i < g_animationQueueCount; ++i) {
        size_t index =
            (g_animationQueueHead + i) % kAnimationQueueCapacity;
        AnimationRequest& request = g_animationQueue[index];

        if (request.replacementViewIdentity == view &&
            request.replacementHwnd == hwnd) {
            request.replacementForegroundConfirmed = true;
            signaled = true;
        }
    }

    if (signaled && g_replacementEvent) {
        SetEvent(g_replacementEvent);
    }

    ReleaseSRWLockExclusive(&g_requestLock);
}

static HRESULT OnViewAddedInternal_Hook(void* pThis, IApplicationView* view) {
    if (view) {
        view->AddRef();
    }

    HRESULT hr = g_onViewAddedInternalOriginal(pThis, view);

    if (SUCCEEDED(hr) && view) {
        HWND hwnd = GetForegroundPolicyViewHwnd(view);
        ObserveViewAddedForPendingAnimations(view, hwnd);
    }

    if (view) {
        view->Release();
    }

    return hr;
}

// -----------------------------------------------------------------------------
// SwitchDesktopInternal hook.
// -----------------------------------------------------------------------------

static HRESULT SwitchDesktopInternal_Hook(void* pThis,
                                          IVirtualDesktop* requestedDesktop) {
    if (!pThis || !requestedDesktop ||
        g_unloading.load(std::memory_order_acquire)) {
        return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
    }

    // Only switches synchronously caused by ForegroundViewChanged are eligible.
    // Every other SwitchDesktopInternal caller is left untouched.
    if (g_foregroundPolicyDepth <= 0) {
        return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
    }

    GUID requestedId = {};
    HRESULT requestedIdHr = requestedDesktop->GetId(&requestedId);
    if (FAILED(requestedIdHr)) {
        return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
    }

    const uint64_t currentNavigationGeneration =
        g_navigationGeneration.load(std::memory_order_acquire);
    const NavigationFocusRecord navigationFocus =
        g_foregroundPolicyNavigationFocus;

    if (navigationFocus.valid &&
        GuidEqual(requestedId, navigationFocus.desktopId)) {
        if (navigationFocus.generation < currentNavigationGeneration) {
            Wh_Log(L"SUPPRESS stale navigation-selected foreground switch "
                   L"generation=%llu current=%llu",
                   static_cast<unsigned long long>(navigationFocus.generation),
                   static_cast<unsigned long long>(currentNavigationGeneration));
            return S_OK;
        }

        if (navigationFocus.generation == currentNavigationGeneration) {
            return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
        }

        return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
    }

    if (navigationFocus.valid) {
        return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
    }

    if (currentNavigationGeneration !=
        g_foregroundPolicyNavigationGeneration) {
        Wh_Log(L"SUPPRESS activation switch from superseded FVP");
        return S_OK;
    }

    GUID sourceDesktopId = {};
    if (!g_notificationReady.load(std::memory_order_acquire) ||
        !LoadCurrentDesktopId(&sourceDesktopId)) {
        return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
    }

    HWND candidate = g_foregroundPolicyHwnd;
    if (!IsActivationWindow(candidate)) {
        return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
    }

    if (GuidEqual(sourceDesktopId, requestedId)) {
        return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
    }

    DWORD candidatePid = 0;
    DWORD candidateTid = GetWindowThreadProcessId(candidate, &candidatePid);
    if (!candidatePid || !candidateTid) {
        return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
    }

    const uint64_t navigationGeneration =
        g_foregroundPolicyNavigationGeneration;
    if (g_navigationGeneration.load(std::memory_order_acquire) !=
        navigationGeneration) {
        return S_OK;
    }

    if (!g_workerReady.load(std::memory_order_acquire)) {
        return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
    }

    const DWORD replacementGraceMs =
        GetReplacementGracePeriod(candidatePid);

    Wh_Log(L"Activation-driven cross-desktop switch source=%s target=%s "
           L"hwnd=%p pid=%lu replacementGraceMs=%lu generation=%llu",
           GuidToStringForLog(sourceDesktopId), GuidToStringForLog(requestedId),
           candidate, candidatePid, replacementGraceMs,
           static_cast<unsigned long long>(navigationGeneration));

    if (!QueueAnimationRequest(g_foregroundPolicyViewIdentity, candidate,
                               candidatePid, candidateTid, sourceDesktopId,
                               requestedId, navigationGeneration,
                               replacementGraceMs)) {
        return g_switchDesktopInternalOriginal(pThis, requestedDesktop);
    }

    // Suppress the synchronous jump. The worker issues the native animated
    // switch after this exact foreground-policy call has unwound.
    return S_OK;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle.
// -----------------------------------------------------------------------------

static bool StartWorker() {
    g_workerReady.store(false, std::memory_order_release);
    ResetStartResult();

    if (RuntimeCancellationRequested()) {
        return RecordStartFailure(HRESULT_FROM_WIN32(ERROR_CANCELLED));
    }

    g_requestEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_replacementEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_workerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_requestEvent || !g_replacementEvent || !g_stopEvent ||
        !g_workerReadyEvent) {
        Wh_Log(L"Failed to create animation worker events");
        return RecordStartFailure(E_FAIL);
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        DWORD error = GetLastError();
        return RecordStartFailure(HRESULT_FROM_WIN32(error));
    }

    HANDLE waits[] = {
        g_runtimeCancelEvent,
        g_workerReadyEvent,
    };

    DWORD waitResult =
        WaitForMultipleObjects(ARRAYSIZE(waits), waits, FALSE, 5000);

    if (waitResult == WAIT_OBJECT_0) {
        return RecordStartFailure(HRESULT_FROM_WIN32(ERROR_CANCELLED));
    }

    if (waitResult == WAIT_OBJECT_0 + 1) {
        return g_workerReady.load(std::memory_order_acquire);
    }

    HRESULT waitHr = waitResult == WAIT_TIMEOUT
                         ? HRESULT_FROM_WIN32(ERROR_TIMEOUT)
                         : HRESULT_FROM_WIN32(GetLastError());
    return RecordStartFailure(waitHr);
}

static void StopWorker() {
    InvalidateNavigationState(L"animation worker stopping");

    AcquireSRWLockExclusive(&g_requestLock);
    g_workerReady.store(false, std::memory_order_release);
    g_animationQueueHead = 0;
    g_animationQueueCount = 0;
    g_activeAnimationRequest = {};
    g_activeAnimationRequestValid = false;
    ReleaseSRWLockExclusive(&g_requestLock);

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_replacementEvent) {
        SetEvent(g_replacementEvent);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_workerReadyEvent) {
        CloseHandle(g_workerReadyEvent);
        g_workerReadyEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_replacementEvent) {
        CloseHandle(g_replacementEvent);
        g_replacementEvent = nullptr;
    }
    if (g_requestEvent) {
        CloseHandle(g_requestEvent);
        g_requestEvent = nullptr;
    }
}

// -----------------------------------------------------------------------------
// Explorer / shell-host lifecycle.
// -----------------------------------------------------------------------------
//
// Windhawk's @include explorer.exe can inject this mod into more than one
// explorer.exe when folder windows are hosted in separate processes.
//
// TrayUI::primary-taskbar startup is the primary positive signal that this
// process is the shell Explorer and that taskbar startup has completed far
// enough for deferred virtual-desktop runtime initialization. Wh_ModAfterInit
// also checks for an already-existing primary Shell_TrayWnd so manual
// enable/reload and late injection don't miss the one-shot primary-taskbar
// startup call.
//
// Heavy twinui symbol resolution and COM initialization remain deferred to the
// transient shell-host initialization thread.

enum class RuntimeState {
    Stopped,
    Initializing,
    Ready,
    Unsupported,
};

static std::atomic<RuntimeState> g_runtimeState{RuntimeState::Stopped};

static std::atomic<bool> g_virtualDesktopHooksInstalled{false};

static SRWLOCK g_runtimeInitLock = SRWLOCK_INIT;

static HANDLE g_runtimeInitThread = nullptr;

static HWND FindCurrentProcessPrimaryTaskbar() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);

    if (!taskbar) {
        return nullptr;
    }

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(taskbar, &pid) ||
        pid != GetCurrentProcessId()) {
        return nullptr;
    }

    return taskbar;
}

static HWND FindCurrentProcessShellWindow() {
    HWND shellWindow = GetShellWindow();

    if (!shellWindow) {
        return nullptr;
    }

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(shellWindow, &pid) ||
        pid != GetCurrentProcessId()) {
        return nullptr;
    }

    return shellWindow;
}

static bool InstallVirtualDesktopHooks() {
    if (g_virtualDesktopHooksInstalled.load(std::memory_order_acquire)) {
        return true;
    }

    if (RuntimeCancellationRequested()) {
        return false;
    }

    HMODULE twinui = GetModuleHandleW(L"twinui.pcshell.dll");

    if (!twinui) {
        twinui = LoadLibraryExW(L"twinui.pcshell.dll", nullptr,
                                LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    if (!twinui) {
        Wh_Log(
            L"Shell host: failed to obtain "
            L"twinui.pcshell.dll error=%lu",
            GetLastError());
        return false;
    }

    if (RuntimeCancellationRequested()) {
        return false;
    }

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK twinuiPcshellHooks[] = {
        {{LR"(const CWin32ApplicationView::`vftable'{for `IApplicationView'})"},
         reinterpret_cast<void**>(&g_win32ApplicationViewVtable)},
        {{LR"(private: virtual long __cdecl CWin32ApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
         reinterpret_cast<void**>(&g_win32ApplicationViewGetNativeWindow)},
        {{LR"(const CWinRTApplicationView::`vftable'{for `IApplicationView'})"},
         reinterpret_cast<void**>(&g_winRtApplicationViewVtable)},
        {{LR"(private: virtual long __cdecl CWinRTApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
         reinterpret_cast<void**>(&g_winRtApplicationViewGetNativeWindow)},
        {{L"public: virtual long __cdecl "
          L"CVirtualDesktopManager::"
          L"SwitchDesktopInternal("
          L"struct IVirtualDesktop *)"},
         reinterpret_cast<void**>(&g_switchDesktopInternalOriginal),
         reinterpret_cast<void*>(SwitchDesktopInternal_Hook)},
        {{L"public: virtual long __cdecl "
          L"CVirtualDesktopManager::"
          L"OnViewAddedInternal("
          L"struct IApplicationView *)"},
         reinterpret_cast<void**>(&g_onViewAddedInternalOriginal),
         reinterpret_cast<void*>(OnViewAddedInternal_Hook),
         true},
        {{L"public: virtual long __cdecl "
          L"CVirtualDesktopForegroundPolicy::"
          L"DesktopChanged("
          L"struct IVirtualDesktop *)"},
         reinterpret_cast<void**>(&g_desktopChangedOriginal),
         reinterpret_cast<void*>(DesktopChanged_Hook)},
        {{L"public: virtual long __cdecl "
          L"CVirtualDesktopForegroundPolicy::"
          L"ForegroundViewChanged("
          L"struct IVirtualDesktopManagerPrivate *,"
          L"struct IVirtualDesktopSwitchAnimator *,"
          L"struct IApplicationView *)"},
         reinterpret_cast<void**>(&g_foregroundViewChangedOriginal),
         reinterpret_cast<void*>(ForegroundViewChanged_Hook)},
        {{L"public: virtual long __cdecl "
          L"CVirtualDesktopForegroundPolicy::"
          L"GetNewForegroundViewForDesktopSwitch("
          L"struct IVirtualDesktop *,"
          L"struct IApplicationView * *,"
          L"bool *)"},
         reinterpret_cast<void**>(
             &g_getNewForegroundViewForDesktopSwitchOriginal),
         reinterpret_cast<void*>(GetNewForegroundViewForDesktopSwitch_Hook)},
    };

    if (!WindhawkUtils::HookSymbols(twinui, twinuiPcshellHooks,
                                    ARRAYSIZE(twinuiPcshellHooks))) {
        Wh_Log(
            L"Shell host: failed to resolve/"
            L"install virtual-desktop symbols");
        return false;
    }

    const bool viewAddedObservationResolved =
        g_onViewAddedInternalOriginal != nullptr;

    if (!viewAddedObservationResolved) {
        Wh_Log(
            L"Shell host: OnViewAddedInternal symbol unavailable; "
            L"transient replacement suppression disabled");
    }

    if (RuntimeCancellationRequested()) {
        return false;
    }

    // These hooks are registered after Wh_ModInit.
    SetLastError(ERROR_SUCCESS);

    if (!Wh_ApplyHookOperations()) {
        DWORD error = GetLastError();

        Wh_Log(
            L"Shell host: "
            L"Wh_ApplyHookOperations failed "
            L"error=%lu",
            error);
        return false;
    }

    g_viewAddedObservationAvailable.store(viewAddedObservationResolved,
                                          std::memory_order_release);

    g_virtualDesktopHooksInstalled.store(true, std::memory_order_release);

    Wh_Log(
        L"Shell host: virtual-desktop "
        L"hooks installed");

    return !RuntimeCancellationRequested();
}

static bool WaitForShellHostRetryDelay() {
    DWORD waitResult = WaitForSingleObject(g_runtimeCancelEvent, 500);

    if (waitResult == WAIT_TIMEOUT) {
        return !g_unloading.load(std::memory_order_acquire);
    }

    if (waitResult == WAIT_FAILED) {
        Wh_Log(
            L"Shell-host retry wait failed "
            L"error=%lu",
            GetLastError());
    }

    return false;
}

static DWORD WINAPI ShellHostInitThreadProc(void*) {
    Wh_Log(L"Shell-host initialization begin");

    if (g_unloading.load(std::memory_order_acquire) ||
        RuntimeCancellationRequested()) {
        g_runtimeState.store(RuntimeState::Stopped, std::memory_order_release);
        return 0;
    }

    constexpr int kMaxRuntimeStartAttempts = 30;

    for (int attempt = 1; attempt <= kMaxRuntimeStartAttempts; ++attempt) {
        bool hookInstallFailed = false;
        bool started = StartWorker();

        if (started && !RuntimeCancellationRequested()) {
            started = StartNotificationCache();
        }

        if (started && !RuntimeCancellationRequested()) {
            if (InstallVirtualDesktopHooks() &&
                !RuntimeCancellationRequested()) {
                g_runtimeState.store(RuntimeState::Ready,
                                     std::memory_order_release);

                Wh_Log(
                    L"Runtime ready after attempt %d: "
                    L"source-desktop cache active, "
                    L"animation queue active",
                    attempt);
                return 0;
            }

            if (!RuntimeCancellationRequested()) {
                g_startHr.store(E_FAIL, std::memory_order_relaxed);
                hookInstallFailed = true;
            }
        }

        StopNotificationCache();
        StopWorker();

        if (hookInstallFailed) {
            g_runtimeState.store(RuntimeState::Stopped,
                                 std::memory_order_release);

            Wh_Log(
                L"Shell-host hook installation "
                L"failed; remaining fail-open");
            return 0;
        }

        const bool unsupported =
            g_startUnsupported.load(std::memory_order_acquire);

        const HRESULT failureHr = g_startHr.load(std::memory_order_relaxed);

        if (g_unloading.load(std::memory_order_acquire) ||
            RuntimeCancellationRequested()) {
            g_runtimeState.store(RuntimeState::Stopped,
                                 std::memory_order_release);
            return 0;
        }

        if (unsupported) {
            g_runtimeState.store(RuntimeState::Unsupported,
                                 std::memory_order_release);

            Wh_Log(
                L"Shell-host runtime unsupported "
                L"hr=0x%08X; remaining fail-open",
                static_cast<unsigned int>(failureHr));
            return 0;
        }

        if (attempt < kMaxRuntimeStartAttempts) {
            Wh_Log(
                L"Shell-host runtime unavailable "
                L"on attempt %d hr=0x%08X; "
                L"retrying",
                attempt, static_cast<unsigned int>(failureHr));

            if (!WaitForShellHostRetryDelay()) {
                g_runtimeState.store(RuntimeState::Stopped,
                                     std::memory_order_release);
                return 0;
            }
        }
    }

    g_runtimeState.store(RuntimeState::Stopped, std::memory_order_release);

    Wh_Log(
        L"Shell-host initialization failed "
        L"after %d attempts; remaining "
        L"fail-open",
        kMaxRuntimeStartAttempts);

    return 0;
}

static void PromoteCurrentProcessToShellHost(const wchar_t* reason) {
    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    RuntimeState state = g_runtimeState.load(std::memory_order_acquire);

    if (state == RuntimeState::Ready || state == RuntimeState::Unsupported) {
        return;
    }

    AcquireSRWLockExclusive(&g_runtimeInitLock);

    if (g_unloading.load(std::memory_order_relaxed) ||
        g_runtimeState.load(std::memory_order_relaxed) !=
            RuntimeState::Stopped) {
        ReleaseSRWLockExclusive(&g_runtimeInitLock);
        return;
    }

    if (g_runtimeInitThread) {
        if (WaitForSingleObject(g_runtimeInitThread, 0) == WAIT_OBJECT_0) {
            CloseHandle(g_runtimeInitThread);
            g_runtimeInitThread = nullptr;
        } else {
            ReleaseSRWLockExclusive(&g_runtimeInitLock);
            return;
        }
    }

    g_runtimeState.store(RuntimeState::Initializing, std::memory_order_release);

    Wh_Log(
        L"Primary shell Explorer confirmed; "
        L"promoting process reason=%s",
        reason ? reason : L"<unknown>");

    g_runtimeInitThread =
        CreateThread(nullptr, 0, ShellHostInitThreadProc, nullptr, 0, nullptr);

    if (!g_runtimeInitThread) {
        Wh_Log(
            L"Shell-host init CreateThread "
            L"failed error=%lu",
            GetLastError());

        g_runtimeState.store(RuntimeState::Stopped, std::memory_order_release);
    }

    ReleaseSRWLockExclusive(&g_runtimeInitLock);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);

static CreateWindowExW_t g_createWindowExWOriginal = nullptr;

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                        LPCWSTR lpClassName,
                                        LPCWSTR lpWindowName,
                                        DWORD dwStyle,
                                        int X,
                                        int Y,
                                        int nWidth,
                                        int nHeight,
                                        HWND hWndParent,
                                        HMENU hMenu,
                                        HINSTANCE hInstance,
                                        LPVOID lpParam) {
    HWND hwnd = g_createWindowExWOriginal(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);

    if (!hwnd || g_unloading.load(std::memory_order_acquire)) {
        return hwnd;
    }

    RuntimeState state = g_runtimeState.load(std::memory_order_acquire);

    if (state == RuntimeState::Ready || state == RuntimeState::Unsupported) {
        return hwnd;
    }

    bool isPrimaryTaskbar = false;

    if (lpClassName && !IS_INTRESOURCE(lpClassName)) {
        isPrimaryTaskbar = _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0;
    } else {
        wchar_t className[32] = {};

        if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
            isPrimaryTaskbar = _wcsicmp(className, L"Shell_TrayWnd") == 0;
        }
    }

    if (!isPrimaryTaskbar) {
        return hwnd;
    }

    DWORD ownerPid = 0;
    GetWindowThreadProcessId(hwnd, &ownerPid);

    if (ownerPid != GetCurrentProcessId()) {
        return hwnd;
    }

    // Never resolve symbols or initialize COM on the CreateWindowExW stack.
    PromoteCurrentProcessToShellHost(L"primary Shell_TrayWnd created");

    return hwnd;
}

static void StopRuntimeBeforeUninit() {
    g_unloading.store(true, std::memory_order_release);

    g_viewAddedObservationAvailable.store(false, std::memory_order_release);
    g_virtualDesktopHooksInstalled.store(false, std::memory_order_release);

    if (g_runtimeCancelEvent) {
        SetEvent(g_runtimeCancelEvent);
    }

    HANDLE initThread = nullptr;

    AcquireSRWLockExclusive(&g_runtimeInitLock);

    initThread = g_runtimeInitThread;
    g_runtimeInitThread = nullptr;

    ReleaseSRWLockExclusive(&g_runtimeInitLock);

    if (initThread) {
        WaitForSingleObject(initThread, INFINITE);

        CloseHandle(initThread);
    }

    // Delayed animation work must stop before Windhawk removes the
    // shell hooks whose causal state the worker consumes.
    StopNotificationCache();
    StopWorker();

    g_runtimeState.store(RuntimeState::Stopped, std::memory_order_release);
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_unloading.store(false, std::memory_order_release);
    g_viewAddedObservationAvailable.store(false, std::memory_order_release);
    g_virtualDesktopHooksInstalled.store(false, std::memory_order_release);

    g_runtimeCancelEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_runtimeCancelEvent) {
        Wh_Log(
            L"Failed to create runtime "
            L"cancellation event error=%lu",
            GetLastError());
        return FALSE;
    }

    Wh_Log(
        L"Init: installing lightweight "
        L"primary-taskbar observer");

    // @include explorer.exe can match separate folder processes. Keep those
    // inert: don't load twinui.pcshell.dll until this PID proves it owns the
    // shell's primary taskbar.
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &g_createWindowExWOriginal)) {
        Wh_Log(L"Failed to hook CreateWindowExW");

        CloseHandle(g_runtimeCancelEvent);
        g_runtimeCancelEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    HWND primaryTaskbar = FindCurrentProcessPrimaryTaskbar();

    if (primaryTaskbar) {
        Wh_Log(
            L"Existing primary taskbar=%p "
            L"belongs to this Explorer process",
            primaryTaskbar);

        PromoteCurrentProcessToShellHost(L"existing primary Shell_TrayWnd");
        return;
    }

    HWND shellWindow = FindCurrentProcessShellWindow();

    if (shellWindow) {
        Wh_Log(
            L"Existing shell window=%p "
            L"belongs to this Explorer process",
            shellWindow);

        PromoteCurrentProcessToShellHost(L"existing shell window");
    } else {
        Wh_Log(
            L"No primary taskbar or shell window "
            L"owned by this PID; remaining inert "
            L"unless Shell_TrayWnd is created");
    }
}

void Wh_ModBeforeUninit() {
    StopRuntimeBeforeUninit();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_runtimeCancelEvent) {
        CloseHandle(g_runtimeCancelEvent);
        g_runtimeCancelEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
