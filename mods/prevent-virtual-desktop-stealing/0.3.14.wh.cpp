// ==WindhawkMod==
// @id              prevent-virtual-desktop-stealing
// @name            Prevent Window Activation from Stealing Virtual Desktop
// @description     Redirect cross-desktop window activation to the current desktop.
// @version         0.3.14
// @author          meteoni
// @github          https://github.com/Meteony
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Prevent Window Activation from Stealing Virtual Desktop

Take back control over your virtual desktops!

![Screenshot](https://raw.githubusercontent.com/Meteony/meteoni-assets/main/prevent-virtual-desktop-stealing/prevent-virtual-desktop-stealing.gif)
_Effect of redirecting window activation_

Windows can switch you to another virtual desktop just because a window that was left open on that
desktop is activated.

Clicking a flashing taskbar button, following a notification, opening an app through its tray icon,
or launching something that insists on reusing an existing window can all unexpectedly pull you away
from your current desktop. This mod keeps you where you are and brings the activated window to you instead.

If you don't know what "virtual desktop stealing" is, try this small experiment:

- Open Windhawk on your first virtual desktop.
- Switch to another virtual desktop.
- Click the Windhawk tray icon.

Normally, Windows jumps back to the first desktop - not anymore with this mod.

Intentional desktop navigation is left alone, including Win+Ctrl+Left/Right and
Task View. The mod also remains compatible with **Disable Virtual Desktop
Transition Animation**.

### Notes

Designed for Windows 11 24H2 and newer. This mod relies on undocumented Windows
shell interfaces, including the private IApplicationView layout, so future
Windows updates may require adjustments.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <objbase.h>
#include <servprov.h>
#include <shobjidl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <atomic>
#include <cstdint>
#include <wchar.h>

// -----------------------------------------------------------------------------
// Explicit COM IDs used by the minimal interface declarations below.
// -----------------------------------------------------------------------------

static const CLSID kClsidImmersiveShell = {
    0xC2F03A33, 0x21F5, 0x47FA,
    {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}
};

static const CLSID kClsidVirtualDesktopManagerInternal = {
    0xC5E0CDCA, 0x7B6E, 0x41B2,
    {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}
};

static const GUID kServiceVirtualDesktopNotification = {
    0xA501FDEC, 0x4A09, 0x464C,
    {0xAE, 0x4E, 0x1B, 0x9C, 0x21, 0xB8, 0x49, 0x18}
};

static const IID kIidVirtualDesktopNotification = {
    0xB9E5E94D, 0x233E, 0x49AB,
    {0xAF, 0x5C, 0x2B, 0x45, 0x41, 0xC3, 0xAA, 0xDE}
};

static const IID kIidVirtualDesktopNotificationService = {
    0x0CD45E71, 0xD927, 0x4F15,
    {0x8B, 0x0A, 0x8F, 0xEF, 0x52, 0x53, 0x37, 0xBF}
};

// Windows 11 24H2+ IVirtualDesktopManagerInternal / Internal2 IID.
static const IID kIidVirtualDesktopManagerInternal24H2 = {
    0x53F5CA0B, 0x158F, 0x4124,
    {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}
};

static const IID kIidApplicationViewCollection = {
    0x1841C6D7, 0x4F9D, 0x42C0,
    {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}
};

// -----------------------------------------------------------------------------
// Minimal undocumented/public interface declarations.
// -----------------------------------------------------------------------------

struct IVirtualDesktop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsViewVisible(
        IUnknown* view,
        BOOL* visible) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetId(
        GUID* desktopId) = 0;
};

struct IApplicationView : IUnknown {};

struct IVirtualDesktopManagerPrivate;
struct IVirtualDesktopSwitchAnimator;

struct IApplicationViewCollection : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetViews(
        IUnknown** objectArray) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetViewsByZOrder(
        IUnknown** objectArray) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetViewsByAppUserModelId(
        LPCWSTR appUserModelId,
        IUnknown** objectArray) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetViewForHwnd(
        HWND hwnd,
        IApplicationView** view) = 0;
};

struct IVirtualDesktopManagerInternal : IUnknown {};

// Windows 11 24H2 (build 26100) uses 14 total vtable slots for IID
// B9E5E94D-233E-49AB-AF5C-2B4541C3AADE: IUnknown at slots 0-2, the
// callbacks below at slots 3-13, CurrentVirtualDesktopChanged at slot 10,
// VirtualDesktopSwitched at slot 12, and RemoteVirtualDesktopConnected at
// slot 13. The older CD403E52-... monitor-aware interface has a different
// layout, including VirtualDesktopIsPerMonitorChanged, and is not used here.
struct IVirtualDesktopNotification : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(
        IVirtualDesktop* desktop) = 0;

    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(
        IVirtualDesktop* desktopDestroyed,
        IVirtualDesktop* desktopFallback) = 0;

    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(
        IVirtualDesktop* desktopDestroyed,
        IVirtualDesktop* desktopFallback) = 0;

    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(
        IVirtualDesktop* desktopDestroyed,
        IVirtualDesktop* desktopFallback) = 0;

    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopMoved(
        IVirtualDesktop* desktop,
        INT64 oldIndex,
        INT64 newIndex) = 0;

    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopNameChanged(
        IVirtualDesktop* desktop,
        void* name) = 0;

    virtual HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(
        IUnknown* view) = 0;

    virtual HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(
        IVirtualDesktop* desktopOld,
        IVirtualDesktop* desktopNew) = 0;

    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopWallpaperChanged(
        IVirtualDesktop* desktop,
        void* wallpaper) = 0;

    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopSwitched(
        IVirtualDesktop* desktop) = 0;

    virtual HRESULT STDMETHODCALLTYPE RemoteVirtualDesktopConnected(
        IVirtualDesktop* desktop) = 0;
};

struct IVirtualDesktopNotificationService : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Register(
        IVirtualDesktopNotification* notification,
        DWORD* cookie) = 0;

    virtual HRESULT STDMETHODCALLTYPE Unregister(
        DWORD cookie) = 0;
};

// -----------------------------------------------------------------------------
// Helpers.
// -----------------------------------------------------------------------------

template <typename T>
static T GetVTableFunction(void* object, int index) {
    return reinterpret_cast<T>(
        (*reinterpret_cast<void***>(object))[index]);
}

static HANDLE g_runtimeCancelEvent = nullptr;  // manual-reset, process lifetime
// Set only for a missing required QueryService interface. Early ImmersiveShell
// activation failures, including REGDB_E_CLASSNOTREG, remain retryable.
static std::atomic<bool> g_startUnsupported{false};
static std::atomic<HRESULT> g_startHr{E_PENDING};

static void ResetStartResult() {
    g_startHr.store(E_PENDING, std::memory_order_relaxed);
    g_startUnsupported.store(false, std::memory_order_release);
}

static void PublishStartResult(
    bool unsupported,
    HRESULT hr,
    HANDLE readyEvent) {

    g_startHr.store(hr, std::memory_order_relaxed);
    g_startUnsupported.store(unsupported, std::memory_order_release);
    SetEvent(readyEvent);
}

static bool RuntimeCancellationRequested() {
    return g_runtimeCancelEvent &&
        WaitForSingleObject(g_runtimeCancelEvent, 0) == WAIT_OBJECT_0;
}

static bool RecordStartFailure(HRESULT hr) {
    g_startHr.store(hr, std::memory_order_relaxed);
    return false;
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

static bool IsRescueCandidate(HWND hwnd) {
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

static DWORD GetWindowProcessIdForLog(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid;
}

static DWORD GetWindowThreadIdForLog(HWND hwnd) {
    DWORD pid = 0;
    return GetWindowThreadProcessId(hwnd, &pid);
}

static const wchar_t* GetWindowClassForLog(HWND hwnd) {
    static thread_local wchar_t className[256];
    className[0] = L'\0';
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    return className[0] ? className : L"?";
}

static const wchar_t* GetWindowTitleForLog(HWND hwnd) {
    static thread_local wchar_t title[512];
    title[0] = L'\0';
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));
    return title;
}

static void LogWindow(const wchar_t* label, HWND hwnd) {
    if (!hwnd) {
        Wh_Log(L"%s hwnd=null", label);
        return;
    }

    // Wh_Log evaluates its arguments only when logging is enabled, so all
    // window inspection stays behind the logging gate.
    Wh_Log(
        L"%s hwnd=%p pid=%lu tid=%lu class=%s "
        L"zoomed=%d iconic=%d title=\"%s\"",
        label,
        hwnd,
        GetWindowProcessIdForLog(hwnd),
        GetWindowThreadIdForLog(hwnd),
        GetWindowClassForLog(hwnd),
        IsZoomed(hwnd),
        IsIconic(hwnd),
        GetWindowTitleForLog(hwnd));
}

// -----------------------------------------------------------------------------
// Explicit user-switch attribution.
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
// The active IApplicationView is mapped to an HWND only through symbol-resolved
// concrete implementations whose vtable identity is known. Probing confirmed
// that this returns the HWND of the exact view driving cross-desktop activation.
//
// Keep only the plain HWND in TLS. Never carry the raw IApplicationView pointer
// into the rescue worker apartment.
static thread_local HWND g_foregroundPolicyHwnd = nullptr;

// Task View and direct shell/API desktop switches are asynchronous: their public
// SwitchDesktop* entry point can return before CurrentVirtualDesktopChanged is
// delivered. Keep an event-driven transaction alive until the notification
// service reports that the explicitly requested desktop finished switching.
//
// This state is process-global rather than thread_local because the public
// switch call, SwitchDesktopInternal, and virtual-desktop notifications can run
// on different Explorer threads.
struct ExplicitSwitchTransaction {
    bool active = false;
    GUID sourceDesktopId = {};
    GUID targetDesktopId = {};
    uint64_t sequence = 0;
    bool targetChangeObserved = false;
    ULONGLONG startedAt = 0;
};

static constexpr ULONGLONG kExplicitSwitchExpiryMs = 2000;
static SRWLOCK g_explicitSwitchLock = SRWLOCK_INIT;
static ExplicitSwitchTransaction g_explicitSwitch = {};
static std::atomic<uint64_t> g_nextExplicitSwitchSequence{1};

// IVirtualDesktopManagerInternal slots for the 24H2 IID above.
static constexpr int kVtableMoveViewToDesktop = 4;
static constexpr int kVtableGetCurrentDesktop = 6;

// SwitchDesktopWithAnimation synchronously calls SwitchDesktop on current
// builds. Only the outermost public switch entry should create a transaction.
static thread_local int g_explicitSwitchEntryDepth = 0;

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

static bool ExplicitSwitchExpired(
    const ExplicitSwitchTransaction& transaction,
    ULONGLONG now) {

    return transaction.active &&
        now - transaction.startedAt > kExplicitSwitchExpiryMs;
}

static void ClearExplicitSwitchTransaction(const wchar_t* reason) {
    ExplicitSwitchTransaction cleared = {};

    AcquireSRWLockExclusive(&g_explicitSwitchLock);
    if (g_explicitSwitch.active) {
        cleared = g_explicitSwitch;
        g_explicitSwitch = {};
    }
    ReleaseSRWLockExclusive(&g_explicitSwitchLock);

    if (cleared.active) {
        Wh_Log(
            L"Explicit switch transaction #%llu cleared: %s",
            static_cast<unsigned long long>(cleared.sequence),
            reason ? reason : L"<unknown>");
    }
}

static uint64_t BeginExplicitSwitchTransaction(
    IVirtualDesktop* targetDesktop,
    const wchar_t* route) {

    if (!targetDesktop) {
        return 0;
    }

    GUID targetId = {};
    if (FAILED(targetDesktop->GetId(&targetId))) {
        return 0;
    }

    GUID sourceId = {};
    if (!LoadCurrentDesktopId(&sourceId)) {
        return 0;
    }

    if (GuidEqual(sourceId, targetId)) {
        return 0;
    }

    const uint64_t sequence =
        g_nextExplicitSwitchSequence.fetch_add(
            1,
            std::memory_order_relaxed);

    AcquireSRWLockExclusive(&g_explicitSwitchLock);
    g_explicitSwitch.active = true;
    g_explicitSwitch.sourceDesktopId = sourceId;
    g_explicitSwitch.targetDesktopId = targetId;
    g_explicitSwitch.sequence = sequence;
    g_explicitSwitch.targetChangeObserved = false;
    g_explicitSwitch.startedAt = GetTickCount64();
    ReleaseSRWLockExclusive(&g_explicitSwitchLock);

    Wh_Log(
        L"Explicit switch transaction #%llu begin route=%s "
        L"source=%s target=%s",
        static_cast<unsigned long long>(sequence),
        route ? route : L"<unknown>",
        GuidToStringForLog(sourceId),
        GuidToStringForLog(targetId));

    return sequence;
}

static void CancelExplicitSwitchTransaction(
    uint64_t sequence,
    const wchar_t* reason) {

    if (!sequence) {
        return;
    }

    bool cancelled = false;

    AcquireSRWLockExclusive(&g_explicitSwitchLock);
    if (g_explicitSwitch.active &&
        g_explicitSwitch.sequence == sequence) {
        g_explicitSwitch = {};
        cancelled = true;
    }
    ReleaseSRWLockExclusive(&g_explicitSwitchLock);

    if (cancelled) {
        Wh_Log(
            L"Explicit switch transaction #%llu cancelled: %s",
            static_cast<unsigned long long>(sequence),
            reason ? reason : L"<unknown>");
    }
}

enum class ExplicitInternalSwitchDisposition {
    None,
    AllowTarget,
    SuppressStaleSource,
};

static ExplicitInternalSwitchDisposition
ClassifyInternalSwitchAgainstExplicitTransaction(
    const GUID& requestedId,
    ExplicitSwitchTransaction* snapshot) {

    ExplicitSwitchTransaction current = {};

    bool expired = false;

    AcquireSRWLockExclusive(&g_explicitSwitchLock);
    current = g_explicitSwitch;
    if (ExplicitSwitchExpired(current, GetTickCount64())) {
        g_explicitSwitch = {};
        expired = true;
    }
    ReleaseSRWLockExclusive(&g_explicitSwitchLock);

    if (expired) {
        Wh_Log(
            L"Explicit switch transaction #%llu expired",
            static_cast<unsigned long long>(current.sequence));
        current = {};
    }

    if (snapshot) {
        *snapshot = current;
    }

    if (!current.active) {
        return ExplicitInternalSwitchDisposition::None;
    }

    if (GuidEqual(requestedId, current.targetDesktopId)) {
        return ExplicitInternalSwitchDisposition::AllowTarget;
    }

    if (!GuidEqual(
            current.sourceDesktopId,
            current.targetDesktopId) &&
        GuidEqual(requestedId, current.sourceDesktopId)) {
        return ExplicitInternalSwitchDisposition::SuppressStaleSource;
    }

    return ExplicitInternalSwitchDisposition::None;
}

static void ObserveExplicitSwitchDesktopChange(const GUID& newDesktopId) {
    ExplicitSwitchTransaction transaction = {};
    bool expired = false;
    bool targetObserved = false;
    bool unexpected = false;

    AcquireSRWLockExclusive(&g_explicitSwitchLock);
    transaction = g_explicitSwitch;

    if (ExplicitSwitchExpired(transaction, GetTickCount64())) {
        g_explicitSwitch = {};
        expired = true;
    } else if (transaction.active &&
               GuidEqual(newDesktopId, transaction.targetDesktopId)) {
        g_explicitSwitch.targetChangeObserved = true;
        targetObserved = true;
    } else if (transaction.active &&
               !GuidEqual(newDesktopId, transaction.sourceDesktopId)) {
        // A change to neither endpoint can't be attributed to this
        // transaction. Disarm it rather than retaining ambiguous suppression.
        g_explicitSwitch = {};
        unexpected = true;
    }

    ReleaseSRWLockExclusive(&g_explicitSwitchLock);

    if (expired) {
        Wh_Log(
            L"Explicit switch transaction #%llu expired on desktop change",
            static_cast<unsigned long long>(transaction.sequence));
    } else if (targetObserved) {
        Wh_Log(
            L"Explicit switch transaction #%llu observed target change",
            static_cast<unsigned long long>(transaction.sequence));
    } else if (unexpected) {
        Wh_Log(
            L"Explicit switch transaction #%llu cleared by unexpected "
            L"desktop change",
            static_cast<unsigned long long>(transaction.sequence));
    }
}

static void FinishExplicitSwitchTransaction(
    IVirtualDesktop* switchedDesktop) {

    if (!switchedDesktop) {
        return;
    }

    GUID switchedId = {};
    if (FAILED(switchedDesktop->GetId(&switchedId))) {
        return;
    }

    ExplicitSwitchTransaction transaction = {};
    bool expired = false;
    bool completed = false;
    bool ignored = false;

    AcquireSRWLockExclusive(&g_explicitSwitchLock);
    transaction = g_explicitSwitch;

    if (ExplicitSwitchExpired(transaction, GetTickCount64())) {
        g_explicitSwitch = {};
        expired = true;
    } else if (transaction.active &&
               transaction.targetChangeObserved &&
               GuidEqual(switchedId, transaction.targetDesktopId)) {
        g_explicitSwitch = {};
        completed = true;
    } else if (transaction.active) {
        ignored = true;
    }
    ReleaseSRWLockExclusive(&g_explicitSwitchLock);

    if (!transaction.active) {
        return;
    }

    if (expired) {
        Wh_Log(
            L"Explicit switch transaction #%llu expired on completion",
            static_cast<unsigned long long>(transaction.sequence));
    } else if (completed) {
        Wh_Log(
            L"Explicit switch transaction #%llu complete switched=%s",
            static_cast<unsigned long long>(transaction.sequence),
            GuidToStringForLog(switchedId));
    } else if (ignored) {
        Wh_Log(
            L"Explicit switch transaction #%llu ignored stale completion "
            L"switched=%s target=%s targetChangeObserved=%d",
            static_cast<unsigned long long>(transaction.sequence),
            GuidToStringForLog(switchedId),
            GuidToStringForLog(transaction.targetDesktopId),
            transaction.targetChangeObserved);
    }
}

// -----------------------------------------------------------------------------
// Symbol-resolved shell functions.
// -----------------------------------------------------------------------------

using SwitchDesktopInternal_t =
    HRESULT (*)(void* pThis, IVirtualDesktop* desktop);

static SwitchDesktopInternal_t g_switchDesktopInternalOriginal = nullptr;

using SwitchDesktop_t =
    HRESULT (*)(void* pThis, IVirtualDesktop* desktop);

static SwitchDesktop_t g_switchDesktopOriginal = nullptr;

using SwitchDesktopWithAnimation_t =
    HRESULT (*)(void* pThis, IVirtualDesktop* desktop);

static SwitchDesktopWithAnimation_t
    g_switchDesktopWithAnimationOriginal = nullptr;

using ForegroundViewChanged_t =
    HRESULT (*)(
        void* pThis,
        IVirtualDesktopManagerPrivate* manager,
        IVirtualDesktopSwitchAnimator* animator,
        IApplicationView* view);

static ForegroundViewChanged_t
    g_foregroundViewChangedOriginal = nullptr;

static void* g_win32ApplicationViewVtable = nullptr;
static void* g_winRtApplicationViewVtable = nullptr;

using ApplicationViewGetNativeWindow_t =
    HRESULT (WINAPI*)(void* pThis, HWND* windowHandle);

static ApplicationViewGetNativeWindow_t
    g_win32ApplicationViewGetNativeWindow = nullptr;
static ApplicationViewGetNativeWindow_t
    g_winRtApplicationViewGetNativeWindow = nullptr;

static HRESULT SwitchDesktop_Hook(
    void* pThis,
    IVirtualDesktop* desktop) {

    ++g_explicitSwitchEntryDepth;

    uint64_t sequence = 0;
    if (g_explicitSwitchEntryDepth == 1) {
        sequence =
            BeginExplicitSwitchTransaction(
                desktop,
                L"SwitchDesktop");
    }

    HRESULT hr =
        g_switchDesktopOriginal(
            pThis,
            desktop);

    if (FAILED(hr) && sequence) {
        CancelExplicitSwitchTransaction(
            sequence,
            L"SwitchDesktop failed");
    }

    --g_explicitSwitchEntryDepth;
    return hr;
}

static HRESULT SwitchDesktopWithAnimation_Hook(
    void* pThis,
    IVirtualDesktop* desktop) {

    ++g_explicitSwitchEntryDepth;

    uint64_t sequence = 0;
    if (g_explicitSwitchEntryDepth == 1) {
        sequence =
            BeginExplicitSwitchTransaction(
                desktop,
                L"SwitchDesktopWithAnimation");
    }

    HRESULT hr =
        g_switchDesktopWithAnimationOriginal(
            pThis,
            desktop);

    if (FAILED(hr) && sequence) {
        CancelExplicitSwitchTransaction(
            sequence,
            L"SwitchDesktopWithAnimation failed");
    }

    --g_explicitSwitchEntryDepth;
    return hr;
}

static HWND GetForegroundPolicyViewHwnd(
    IApplicationView* view) {

    if (!view) {
        return nullptr;
    }

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
            L"ForegroundViewChanged: unknown IApplicationView vtable=%p; "
            L"failing open",
            vtable);
        return nullptr;
    }

    HWND hwnd = nullptr;
    HRESULT hr =
        getNativeWindow(
            view,
            &hwnd);

    if (FAILED(hr)) {
        Wh_Log(
            L"ForegroundViewChanged: "
            L"v_GetNativeWindow failed hr=0x%08X",
            static_cast<unsigned int>(hr));
        return nullptr;
    }

    return hwnd;
}

static HRESULT ForegroundViewChanged_Hook(
    void* pThis,
    IVirtualDesktopManagerPrivate* manager,
    IVirtualDesktopSwitchAnimator* animator,
    IApplicationView* view) {

    HWND previousHwnd =
        g_foregroundPolicyHwnd;

    ++g_foregroundPolicyDepth;

    // Resolve the exact view synchronously, while the IApplicationView pointer
    // is valid on this shell call stack. Only the HWND crosses into the nested
    // SwitchDesktopInternal hook and, later, the worker queue.
    g_foregroundPolicyHwnd =
        GetForegroundPolicyViewHwnd(view);

    HRESULT hr =
        g_foregroundViewChangedOriginal(
            pThis,
            manager,
            animator,
            view);

    g_foregroundPolicyHwnd =
        previousHwnd;

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
    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid,
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

    ULONG STDMETHODCALLTYPE AddRef() override {
        return ++m_refCount;
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG value = --m_refCount;
        if (!value) {
            delete this;
        }
        return value;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(
        IVirtualDesktop*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(
        IVirtualDesktop*,
        IVirtualDesktop*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(
        IVirtualDesktop*,
        IVirtualDesktop*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(
        IVirtualDesktop*,
        IVirtualDesktop*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopMoved(
        IVirtualDesktop*,
        INT64,
        INT64) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopNameChanged(
        IVirtualDesktop*,
        void*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(
        IUnknown*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(
        IVirtualDesktop* desktopOld,
        IVirtualDesktop* desktopNew) override {

        GUID oldId = {};
        GUID newId = {};

        HRESULT oldHr = desktopOld ? desktopOld->GetId(&oldId) : E_POINTER;
        HRESULT newHr = desktopNew ? desktopNew->GetId(&newId) : E_POINTER;

        if (SUCCEEDED(newHr)) {
            StoreCurrentDesktopId(newId);
            ObserveExplicitSwitchDesktopChange(newId);

            Wh_Log(
                L"Current desktop cache updated old=%s new=%s",
                SUCCEEDED(oldHr)
                    ? GuidToStringForLog(oldId)
                    : L"<unknown>",
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

    HRESULT STDMETHODCALLTYPE VirtualDesktopWallpaperChanged(
        IVirtualDesktop*,
        void*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopSwitched(
        IVirtualDesktop* desktop) override {

        FinishExplicitSwitchTransaction(desktop);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE RemoteVirtualDesktopConnected(
        IVirtualDesktop*) override {
        return S_OK;
    }

private:
    std::atomic<ULONG> m_refCount{1};
};

static DWORD WINAPI NotificationThreadProc(void*) {
    // Create the thread message queue before doing anything that can block. If
    // StopNotificationCache posted too early for PostThreadMessage to succeed,
    // the durable stop event below still prevents entry into GetMessage.
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    if (NotificationStopRequested()) {
        PublishStartResult(
            false,
            HRESULT_FROM_WIN32(ERROR_CANCELLED),
            g_notificationReadyEvent);
        return 0;
    }

    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool shouldUninitialize = SUCCEEDED(coHr);

    if (FAILED(coHr) && coHr != RPC_E_CHANGED_MODE) {
        Wh_Log(
            L"Notification CoInitializeEx failed hr=0x%08X",
            static_cast<unsigned int>(coHr));

        PublishStartResult(
            false,
            coHr,
            g_notificationReadyEvent);
        return 0;
    }

    if (NotificationStopRequested()) {
        PublishStartResult(
            false,
            HRESULT_FROM_WIN32(ERROR_CANCELLED),
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
        kClsidImmersiveShell,
        nullptr,
        CLSCTX_LOCAL_SERVER,
        __uuidof(IServiceProvider),
        reinterpret_cast<void**>(&serviceProvider));

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

    if (SUCCEEDED(hr) && notificationService) {
        listener = new VirtualDesktopNotificationListener();
        hr = notificationService->Register(listener, &cookie);
    }

    // Register before seeding so a desktop change during the seed isn't lost.
    if (SUCCEEDED(hr) && managerInternal) {
        auto getCurrentDesktop = GetVTableFunction<
            HRESULT(STDMETHODCALLTYPE*)(
                void*,
                IVirtualDesktop**)>(
            managerInternal,
            kVtableGetCurrentDesktop);

        IVirtualDesktop* currentDesktop = nullptr;
        HRESULT currentHr =
            getCurrentDesktop(managerInternal, &currentDesktop);

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
            g_notificationReady.store(true);
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
        L"Notification/cache init hr=0x%08X cookie=%lu ready=%d",
        static_cast<unsigned int>(hr),
        cookie,
        g_notificationReady.load());

    PublishStartResult(
        unsupported,
        hr,
        g_notificationReadyEvent);

    if (g_notificationReady.load(std::memory_order_acquire) &&
        !NotificationStopRequested()) {
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    g_notificationReady.store(false);
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
    // Each start attempt owns its readiness result. Don't inherit a true value
    // from a previous notification thread that exited during startup retry.
    g_notificationReady.store(false, std::memory_order_release);
    ResetStartResult();

    if (RuntimeCancellationRequested()) {
        return RecordStartFailure(HRESULT_FROM_WIN32(ERROR_CANCELLED));
    }

    g_notificationReadyEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);

    g_notificationStopEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_notificationReadyEvent || !g_notificationStopEvent) {
        Wh_Log(L"Failed to create notification events");
        return RecordStartFailure(E_FAIL);
    }

    g_notificationThread =
        CreateThread(
            nullptr,
            0,
            NotificationThreadProc,
            nullptr,
            0,
            &g_notificationThreadId);

    if (!g_notificationThread) {
        DWORD error = GetLastError();
        Wh_Log(
            L"Notification CreateThread failed error=%lu",
            error);
        return RecordStartFailure(HRESULT_FROM_WIN32(error));
    }

    HANDLE waits[] = {
        g_runtimeCancelEvent,
        g_notificationReadyEvent,
    };

    DWORD waitResult = WaitForMultipleObjects(
        ARRAYSIZE(waits),
        waits,
        FALSE,
        5000);

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
        L"Notification/current-desktop cache unavailable "
        L"(waitResult=%lu hr=0x%08X)",
        waitResult,
        static_cast<unsigned int>(waitHr));

    return RecordStartFailure(waitHr);
}

static void StopNotificationCache() {
    g_notificationReady.store(false, std::memory_order_release);
    ClearExplicitSwitchTransaction(L"notification cache stopping");

    if (g_notificationStopEvent) {
        SetEvent(g_notificationStopEvent);
    }

    if (g_notificationThread) {
        if (g_notificationThreadId) {
            PostThreadMessageW(
                g_notificationThreadId,
                WM_QUIT,
                0,
                0);
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
// Rescue worker.
// -----------------------------------------------------------------------------

struct RescueRequest {
    uint64_t sequence = 0;
    HWND hwnd = nullptr;
    DWORD pid = 0;
    DWORD tid = 0;
    GUID sourceDesktopId = {};
    GUID requestedDesktopId = {};
};

static constexpr size_t kRescueQueueCapacity = 16;
static SRWLOCK g_requestLock = SRWLOCK_INIT;
static RescueRequest g_rescueQueue[kRescueQueueCapacity] = {};
static size_t g_rescueQueueHead = 0;
static size_t g_rescueQueueCount = 0;
static std::atomic<uint64_t> g_nextSequence = 1;

static HANDLE g_requestEvent = nullptr;  // auto-reset
static HANDLE g_stopEvent = nullptr;     // manual-reset
static HANDLE g_workerThread = nullptr;
static HANDLE g_workerReadyEvent = nullptr;
static std::atomic<bool> g_workerReady = false;

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
        kClsidImmersiveShell,
        nullptr,
        CLSCTX_LOCAL_SERVER,
        __uuidof(IServiceProvider),
        reinterpret_cast<void**>(&state->serviceProvider));

    if (FAILED(hr) || !state->serviceProvider) {
        if (SUCCEEDED(hr)) {
            hr = E_FAIL;
        }

        Wh_Log(
            L"Worker: CoCreateInstance(ImmersiveShell) failed "
            L"hr=0x%08X",
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

        Wh_Log(
            L"Worker: QueryService(managerInternal) failed "
            L"hr=0x%08X",
            static_cast<unsigned int>(hr));
        g_startHr.store(hr, std::memory_order_relaxed);
        g_startUnsupported.store(
            hr == E_NOINTERFACE,
            std::memory_order_release);
        return false;
    }

    hr = state->serviceProvider->QueryService(
        kIidApplicationViewCollection,
        kIidApplicationViewCollection,
        reinterpret_cast<void**>(&state->viewCollection));

    if (FAILED(hr) || !state->viewCollection) {
        if (SUCCEEDED(hr)) {
            hr = E_NOINTERFACE;
        }

        Wh_Log(
            L"Worker: QueryService(viewCollection) failed "
            L"hr=0x%08X",
            static_cast<unsigned int>(hr));
        g_startHr.store(hr, std::memory_order_relaxed);
        g_startUnsupported.store(
            hr == E_NOINTERFACE,
            std::memory_order_release);
        return false;
    }

    hr = CoCreateInstance(
        __uuidof(VirtualDesktopManager),
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&state->publicManager));

    if (FAILED(hr) || !state->publicManager) {
        if (SUCCEEDED(hr)) {
            hr = E_FAIL;
        }

        Wh_Log(
            L"Worker: CoCreateInstance(public manager) failed "
            L"hr=0x%08X",
            static_cast<unsigned int>(hr));
        g_startHr.store(hr, std::memory_order_relaxed);
        return false;
    }

    return true;
}

// Older monitor-aware interface versions aren't queried because their method
// signatures differ from the 24H2 interface used here.
static HRESULT WorkerGetCurrentDesktop(
    WorkerComState* state,
    IVirtualDesktop** desktop) {

    auto fn = GetVTableFunction<
        HRESULT(STDMETHODCALLTYPE*)(
            void*,
            IVirtualDesktop**)>(
        state->managerInternal,
        kVtableGetCurrentDesktop);

    return fn(state->managerInternal, desktop);
}

static HRESULT WorkerMoveViewToDesktop(
    WorkerComState* state,
    IApplicationView* view,
    IVirtualDesktop* desktop) {

    auto fn = GetVTableFunction<
        HRESULT(STDMETHODCALLTYPE*)(
            void*,
            IApplicationView*,
            IVirtualDesktop*)>(
        state->managerInternal,
        kVtableMoveViewToDesktop);

    return fn(state->managerInternal, view, desktop);
}

static bool TakePendingRequest(RescueRequest* request) {
    if (!request) {
        return false;
    }

    AcquireSRWLockExclusive(&g_requestLock);

    if (!g_rescueQueueCount) {
        ReleaseSRWLockExclusive(&g_requestLock);
        return false;
    }

    *request = g_rescueQueue[g_rescueQueueHead];
    g_rescueQueue[g_rescueQueueHead] = {};
    g_rescueQueueHead =
        (g_rescueQueueHead + 1) % kRescueQueueCapacity;
    --g_rescueQueueCount;

    ReleaseSRWLockExclusive(&g_requestLock);
    return true;
}

static void ProcessRescueRequest(
    WorkerComState* state,
    const RescueRequest& request) {

    // Abort paths below intentionally don't replay the suppressed switch.
    // By the time this worker runs, a newer user navigation may have superseded
    // the request; switching here would override it and would re-enter the
    // shell hooks from this worker apartment. Keep every abort fail-stationary
    // and logged rather than manufacturing a stale cross-thread switch.

    Wh_Log(
        L"[#%llu] Rescue begin hwnd=%p requested=%s",
        static_cast<unsigned long long>(request.sequence),
        request.hwnd,
        GuidToStringForLog(request.requestedDesktopId));

    if (!IsRescueCandidate(request.hwnd)) {
        Wh_Log(
            L"[#%llu] Abort: HWND no longer eligible",
            static_cast<unsigned long long>(request.sequence));
        return;
    }

    DWORD currentPid = 0;
    DWORD currentTid =
        GetWindowThreadProcessId(request.hwnd, &currentPid);

    if (currentPid != request.pid ||
        currentTid != request.tid) {
        Wh_Log(
            L"[#%llu] Abort: HWND identity changed "
            L"(expected pid=%lu tid=%lu, got pid=%lu tid=%lu)",
            static_cast<unsigned long long>(request.sequence),
            request.pid,
            request.tid,
            currentPid,
            currentTid);
        return;
    }

    // Read the real current desktop after the intercepted switch returned.
    // Since SwitchDesktopInternal was suppressed, this is the desktop we stayed on.
    IVirtualDesktop* currentDesktop = nullptr;
    HRESULT hr =
        WorkerGetCurrentDesktop(state, &currentDesktop);

    if (FAILED(hr) || !currentDesktop) {
        Wh_Log(
            L"[#%llu] Abort: GetCurrentDesktop failed "
            L"hr=0x%08X",
            static_cast<unsigned long long>(request.sequence),
            static_cast<unsigned int>(hr));
        return;
    }

    GUID actualCurrentId = {};
    hr = currentDesktop->GetId(&actualCurrentId);

    if (FAILED(hr)) {
        Wh_Log(
            L"[#%llu] Abort: currentDesktop->GetId failed "
            L"hr=0x%08X",
            static_cast<unsigned long long>(request.sequence),
            static_cast<unsigned int>(hr));
        currentDesktop->Release();
        return;
    }

    if (!GuidEqual(actualCurrentId, request.sourceDesktopId)) {
        Wh_Log(
            L"[#%llu] Abort: current desktop changed before rescue "
            L"(source=%s now=%s)",
            static_cast<unsigned long long>(request.sequence),
            GuidToStringForLog(request.sourceDesktopId),
            GuidToStringForLog(actualCurrentId));

        currentDesktop->Release();
        return;
    }

    BOOL onCurrentDesktop = FALSE;
    HRESULT onCurrentHr =
        state->publicManager->IsWindowOnCurrentVirtualDesktop(
            request.hwnd,
            &onCurrentDesktop);

    if (SUCCEEDED(onCurrentHr) && onCurrentDesktop) {
        Wh_Log(
            L"[#%llu] Nothing to do: window is already visible "
            L"on the current desktop (moved/pinned meanwhile)",
            static_cast<unsigned long long>(request.sequence));
        currentDesktop->Release();
        return;
    }

    // Verify that the foreground window really belongs to the desktop Windows
    // wanted to switch to. This makes the teleport much less likely to grab an
    // unrelated foreground window.
    GUID windowDesktopId = {};
    hr = state->publicManager->GetWindowDesktopId(
        request.hwnd,
        &windowDesktopId);

    if (FAILED(hr)) {
        Wh_Log(
            L"[#%llu] Abort: GetWindowDesktopId failed "
            L"hr=0x%08X",
            static_cast<unsigned long long>(request.sequence),
            static_cast<unsigned int>(hr));
        currentDesktop->Release();
        return;
    }

    if (GuidEqual(windowDesktopId, actualCurrentId)) {
        Wh_Log(
            L"[#%llu] Nothing to do: window is already on "
            L"current desktop",
            static_cast<unsigned long long>(request.sequence));
        currentDesktop->Release();
        return;
    }

    if (!GuidEqual(
            windowDesktopId,
            request.requestedDesktopId)) {
        Wh_Log(
            L"[#%llu] Abort: foreground window desktop "
            L"doesn't match requested switch target "
            L"(window=%s requested=%s)",
            static_cast<unsigned long long>(request.sequence),
            GuidToStringForLog(windowDesktopId),
            GuidToStringForLog(request.requestedDesktopId));

        currentDesktop->Release();
        return;
    }

    IApplicationView* view = nullptr;
    hr = state->viewCollection->GetViewForHwnd(
        request.hwnd,
        &view);

    if (FAILED(hr) || !view) {
        Wh_Log(
            L"[#%llu] Abort: GetViewForHwnd failed "
            L"hr=0x%08X",
            static_cast<unsigned long long>(request.sequence),
            static_cast<unsigned int>(hr));
        currentDesktop->Release();
        return;
    }

    // Re-check the source desktop immediately before committing the move.
    // This prevents a legitimate desktop switch that happened while resolving
    // the IApplicationView from dragging the rescued window along with it.
    IVirtualDesktop* commitDesktop = nullptr;
    HRESULT commitHr =
        WorkerGetCurrentDesktop(state, &commitDesktop);

    GUID commitDesktopId = {};
    if (FAILED(commitHr) ||
        !commitDesktop ||
        FAILED(commitDesktop->GetId(&commitDesktopId)) ||
        !GuidEqual(commitDesktopId, request.sourceDesktopId)) {

        Wh_Log(
            L"[#%llu] Abort: current desktop changed during rescue",
            static_cast<unsigned long long>(request.sequence));

        if (commitDesktop) {
            commitDesktop->Release();
        }
        view->Release();
        currentDesktop->Release();
        return;
    }

    currentDesktop->Release();
    currentDesktop = commitDesktop;

    hr = WorkerMoveViewToDesktop(
        state,
        view,
        currentDesktop);

    view->Release();
    currentDesktop->Release();

    if (FAILED(hr)) {
        Wh_Log(
            L"[#%llu] MoveViewToDesktop failed "
            L"hr=0x%08X",
            static_cast<unsigned long long>(request.sequence),
            static_cast<unsigned int>(hr));
        return;
    }

    Wh_Log(
        L"[#%llu] Teleported hwnd=%p to current desktop",
        static_cast<unsigned long long>(request.sequence),
        request.hwnd);

    // Virtual Desktop Helper similarly restores/focuses the moved window.
    if (IsIconic(request.hwnd)) {
        ShowWindowAsync(request.hwnd, SW_RESTORE);
    }

    SetForegroundWindow(request.hwnd);
}

static DWORD WINAPI WorkerThreadProc(void*) {
    g_workerReady.store(false, std::memory_order_release);

    HRESULT coHr =
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    const bool shouldUninitialize =
        SUCCEEDED(coHr);

    if (FAILED(coHr)) {
        Wh_Log(
            L"Worker: CoInitializeEx failed hr=0x%08X",
            static_cast<unsigned int>(coHr));

        PublishStartResult(
            false,
            coHr,
            g_workerReadyEvent);
        return 0;
    }

    if (WorkerStopRequested()) {
        PublishStartResult(
            false,
            HRESULT_FROM_WIN32(ERROR_CANCELLED),
            g_workerReadyEvent);

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

        PublishStartResult(
            g_startUnsupported.load(std::memory_order_acquire),
            g_startHr.load(std::memory_order_relaxed),
            g_workerReadyEvent);
        return 0;
    }

    g_workerReady.store(true, std::memory_order_release);
    PublishStartResult(
        false,
        S_OK,
        g_workerReadyEvent);

    Wh_Log(L"Worker ready");

    HANDLE waits[] = {
        g_stopEvent,
        g_requestEvent,
    };

    while (true) {
        DWORD waitResult =
            WaitForMultipleObjects(
                ARRAYSIZE(waits),
                waits,
                FALSE,
                INFINITE);

        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        if (waitResult == WAIT_OBJECT_0 + 1) {
            RescueRequest request;
            while (TakePendingRequest(&request)) {
                ProcessRescueRequest(
                    &state,
                    request);

                if (WaitForSingleObject(g_stopEvent, 0) ==
                    WAIT_OBJECT_0) {
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

    Wh_Log(L"Worker exiting");
    return 0;
}

static bool QueueRescue(
    HWND hwnd,
    DWORD pid,
    DWORD tid,
    const GUID& sourceDesktopId,
    const GUID& requestedDesktopId) {

    RescueRequest request;
    request.sequence = g_nextSequence.fetch_add(1);
    request.hwnd = hwnd;
    request.pid = pid;
    request.tid = tid;
    request.sourceDesktopId = sourceDesktopId;
    request.requestedDesktopId = requestedDesktopId;

    bool queued = false;

    AcquireSRWLockExclusive(&g_requestLock);

    if (g_workerReady.load(std::memory_order_acquire) &&
        g_requestEvent &&
        g_rescueQueueCount < kRescueQueueCapacity) {
        size_t index =
            (g_rescueQueueHead + g_rescueQueueCount) %
            kRescueQueueCapacity;
        g_rescueQueue[index] = request;
        ++g_rescueQueueCount;
        if (SetEvent(g_requestEvent)) {
            queued = true;
        } else {
            --g_rescueQueueCount;
        }
    }

    ReleaseSRWLockExclusive(&g_requestLock);

    if (!queued) {
        Wh_Log(
            L"Rescue queue unavailable or full; failing open");
    }

    return queued;
}

// -----------------------------------------------------------------------------
// SwitchDesktopInternal hook.
// -----------------------------------------------------------------------------

static HRESULT SwitchDesktopInternal_Hook(
    void* pThis,
    IVirtualDesktop* requestedDesktop) {

    if (!pThis || !requestedDesktop) {
        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    // Hard blast-radius boundary: if the foreground policy didn't synchronously
    // cause this switch, don't inspect or classify it at all.
    if (g_foregroundPolicyDepth <= 0) {
        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    // From here down, the switch is positively attributed to
    // CVirtualDesktopForegroundPolicy::ForegroundViewChanged.
    if (!g_workerReady.load()) {
        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    GUID requestedId = {};
    HRESULT requestedIdHr =
        requestedDesktop->GetId(&requestedId);

    if (FAILED(requestedIdHr)) {
        Wh_Log(
            L"SwitchDesktopInternal allowed: "
            L"requestedDesktop->GetId failed hr=0x%08X",
            static_cast<unsigned int>(requestedIdHr));

        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    // Task View can briefly try to restore/reactivate the foreground window
    // from the desktop we just left (especially a fullscreen app) after the
    // explicit SwitchDesktop* entry point has already returned. During the
    // event-bounded explicit transaction, suppress exactly that request back
    // to the captured source desktop. This is shell transition cleanup, so do
    // NOT queue a teleport.
    ExplicitSwitchTransaction explicitSnapshot = {};
    ExplicitInternalSwitchDisposition explicitDisposition =
        ClassifyInternalSwitchAgainstExplicitTransaction(
            requestedId,
            &explicitSnapshot);

    if (explicitDisposition ==
        ExplicitInternalSwitchDisposition::AllowTarget) {
        Wh_Log(
            L"ALLOW SwitchDesktopInternal: "
            L"explicit transaction #%llu target",
            static_cast<unsigned long long>(
                explicitSnapshot.sequence));

        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    if (explicitDisposition ==
        ExplicitInternalSwitchDisposition::SuppressStaleSource) {

        // Rapid Task View switching can make foreground policy try to restore
        // the view from the desktop we just left. Suppress that exact rebound,
        // but don't queue a rescue/teleport.
        Wh_Log(
            L"SUPPRESS SwitchDesktopInternal: stale source rebound "
            L"during explicit transaction #%llu target=%s "
            L"foregroundPolicyDepth=%d",
            static_cast<unsigned long long>(
                explicitSnapshot.sequence),
            GuidToStringForLog(explicitSnapshot.targetDesktopId),
            g_foregroundPolicyDepth);

        return S_OK;
    }

    GUID sourceDesktopId = {};
    if (!g_notificationReady.load() ||
        !LoadCurrentDesktopId(&sourceDesktopId)) {
        Wh_Log(
            L"SwitchDesktopInternal allowed: "
            L"current-desktop cache unavailable");

        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    HWND candidate =
        g_foregroundPolicyHwnd;

    // Even inside the positively attributed activation path, fail open unless
    // the exact foreground-policy view resolved to a plausible top-level app
    // window. Do not fall back to GetForegroundWindow(): doing so would throw
    // away the precise view identity that ForegroundViewChanged supplied.
    if (!IsRescueCandidate(candidate)) {
        Wh_Log(
            L"SwitchDesktopInternal allowed: "
            L"no eligible exact-view rescue candidate");

        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    if (GuidEqual(sourceDesktopId, requestedId)) {
        Wh_Log(
            L"SwitchDesktopInternal allowed: "
            L"requested desktop is already current");

        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    DWORD candidatePid = 0;
    DWORD candidateTid =
        GetWindowThreadProcessId(candidate, &candidatePid);

    if (!candidatePid || !candidateTid) {
        Wh_Log(
            L"SwitchDesktopInternal allowed: "
            L"failed to capture candidate HWND identity");

        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    Wh_Log(
        L"Candidate activation-driven SwitchDesktopInternal "
        L"source=%s requested=%s exactViewHwnd=%p "
        L"foregroundPolicyDepth=%d",
        GuidToStringForLog(sourceDesktopId),
        GuidToStringForLog(requestedId),
        candidate,
        g_foregroundPolicyDepth);

    LogWindow(L"candidate", candidate);

    if (!QueueRescue(
            candidate,
            candidatePid,
            candidateTid,
            sourceDesktopId,
            requestedId)) {
        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    Wh_Log(
        L"BLOCK SwitchDesktopInternal: rescue queued");

    // Pretend the internal switch succeeded only after the rescue has been
    // safely queued. Every failure before this point fails open. If the worker
    // later aborts, it deliberately doesn't replay this request: the desktop
    // may have changed meanwhile, and replay from the worker apartment would
    // risk overriding newer navigation and re-entering this hook.
    return S_OK;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle.
// -----------------------------------------------------------------------------

static bool StartWorker() {
    // Each start attempt owns its readiness result. A slow previous worker can
    // otherwise publish true after StopWorker already cleared the flag.
    g_workerReady.store(false, std::memory_order_release);
    ResetStartResult();

    if (RuntimeCancellationRequested()) {
        return RecordStartFailure(HRESULT_FROM_WIN32(ERROR_CANCELLED));
    }

    g_requestEvent =
        CreateEventW(nullptr, FALSE, FALSE, nullptr);

    g_stopEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);

    g_workerReadyEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_requestEvent || !g_stopEvent || !g_workerReadyEvent) {
        Wh_Log(L"Failed to create worker events");
        return RecordStartFailure(E_FAIL);
    }

    g_workerThread =
        CreateThread(
            nullptr,
            0,
            WorkerThreadProc,
            nullptr,
            0,
            nullptr);

    if (!g_workerThread) {
        DWORD error = GetLastError();
        Wh_Log(
            L"CreateThread failed error=%lu",
            error);
        return RecordStartFailure(HRESULT_FROM_WIN32(error));
    }

    HANDLE waits[] = {
        g_runtimeCancelEvent,
        g_workerReadyEvent,
    };

    DWORD waitResult = WaitForMultipleObjects(
        ARRAYSIZE(waits),
        waits,
        FALSE,
        5000);

    if (waitResult == WAIT_OBJECT_0) {
        return RecordStartFailure(HRESULT_FROM_WIN32(ERROR_CANCELLED));
    }

    if (waitResult == WAIT_OBJECT_0 + 1) {
        return g_workerReady.load(std::memory_order_acquire);
    }

    HRESULT waitHr = waitResult == WAIT_TIMEOUT
        ? HRESULT_FROM_WIN32(ERROR_TIMEOUT)
        : HRESULT_FROM_WIN32(GetLastError());

    Wh_Log(
        L"Worker initialization failed "
        L"(waitResult=%lu hr=0x%08X)",
        waitResult,
        static_cast<unsigned int>(waitHr));

    return RecordStartFailure(waitHr);
}

static void StopWorker() {
    AcquireSRWLockExclusive(&g_requestLock);
    g_workerReady.store(false, std::memory_order_release);
    g_rescueQueueHead = 0;
    g_rescueQueueCount = 0;
    ReleaseSRWLockExclusive(&g_requestLock);

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_workerThread) {
        WaitForSingleObject(
            g_workerThread,
            INFINITE);

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

    AcquireSRWLockExclusive(&g_requestLock);
    if (g_requestEvent) {
        CloseHandle(g_requestEvent);
        g_requestEvent = nullptr;
    }
    ReleaseSRWLockExclusive(&g_requestLock);

}

// -----------------------------------------------------------------------------
// Explorer / shell-host lifecycle.
// -----------------------------------------------------------------------------
//
// Windhawk's @include explorer.exe can inject this mod into more than one
// explorer.exe when folder windows are hosted in separate processes.
//
// Therefore Wh_ModInit deliberately installs only one lightweight process-local
// observer: CreateWindowExW. The process is promoted to "shell Explorer" only
// after it creates (or already owns) the PRIMARY taskbar window:
//
//     Shell_TrayWnd
//
// Shell_SecondaryTrayWnd is intentionally ignored. Once shell ownership has
// been proven, the twinui virtual-desktop hooks and COM runtime are installed
// exactly once. Non-shell explorer.exe processes never load twinui.pcshell.dll
// for this mod and never start the virtual-desktop worker/cache.

enum class RuntimeState {
    Stopped,
    Initializing,
    Ready,
    Unsupported,
};

static std::atomic<RuntimeState> g_runtimeState{RuntimeState::Stopped};

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_shellHostConfirmed{false};
static std::atomic<bool> g_virtualDesktopHooksInstalled{false};

static SRWLOCK g_runtimeInitLock = SRWLOCK_INIT;
static HANDLE g_runtimeInitThread = nullptr;

static HWND FindCurrentProcessPrimaryTaskbar() {
    HWND taskbar = FindWindowW(
        L"Shell_TrayWnd",
        nullptr);

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
    if (g_virtualDesktopHooksInstalled.load(
            std::memory_order_acquire)) {
        return true;
    }

    if (RuntimeCancellationRequested()) {
        return false;
    }

    HMODULE twinui = LoadLibraryExW(
        L"twinui.pcshell.dll",
        nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!twinui) {
        Wh_Log(
            L"Shell host: "
            L"LoadLibrary(twinui.pcshell.dll) failed error=%lu",
            GetLastError());
        return false;
    }

    if (RuntimeCancellationRequested()) {
        return false;
    }

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK twinuiPcshellHooks[] = {
        {
            {
                LR"(const CWin32ApplicationView::`vftable'{for `IApplicationView'})"
            },
            reinterpret_cast<void**>(
                &g_win32ApplicationViewVtable)
        },
        {
            {
                LR"(private: virtual long __cdecl CWin32ApplicationView::v_GetNativeWindow(struct HWND__ * *))"
            },
            reinterpret_cast<void**>(
                &g_win32ApplicationViewGetNativeWindow)
        },
        {
            {
                LR"(const CWinRTApplicationView::`vftable'{for `IApplicationView'})"
            },
            reinterpret_cast<void**>(
                &g_winRtApplicationViewVtable)
        },
        {
            {
                LR"(private: virtual long __cdecl CWinRTApplicationView::v_GetNativeWindow(struct HWND__ * *))"
            },
            reinterpret_cast<void**>(
                &g_winRtApplicationViewGetNativeWindow)
        },
        {
            {
                L"public: virtual long __cdecl "
                L"CVirtualDesktopManager::SwitchDesktopInternal("
                L"struct IVirtualDesktop *)"
            },
            reinterpret_cast<void**>(
                &g_switchDesktopInternalOriginal),
            reinterpret_cast<void*>(
                SwitchDesktopInternal_Hook)
        },
        {
            {
                L"public: virtual long __cdecl "
                L"CVirtualDesktopManager::SwitchDesktop("
                L"struct IVirtualDesktop *)"
            },
            reinterpret_cast<void**>(
                &g_switchDesktopOriginal),
            reinterpret_cast<void*>(
                SwitchDesktop_Hook)
        },
        {
            {
                L"public: virtual long __cdecl "
                L"CVirtualDesktopManager::SwitchDesktopWithAnimation("
                L"struct IVirtualDesktop *)"
            },
            reinterpret_cast<void**>(
                &g_switchDesktopWithAnimationOriginal),
            reinterpret_cast<void*>(
                SwitchDesktopWithAnimation_Hook)
        },
        {
            {
                L"public: virtual long __cdecl "
                L"CVirtualDesktopForegroundPolicy::ForegroundViewChanged("
                L"struct IVirtualDesktopManagerPrivate *,"
                L"struct IVirtualDesktopSwitchAnimator *,"
                L"struct IApplicationView *)"
            },
            reinterpret_cast<void**>(
                &g_foregroundViewChangedOriginal),
            reinterpret_cast<void*>(
                ForegroundViewChanged_Hook)
        },
    };

    if (!WindhawkUtils::HookSymbols(
            twinui,
            twinuiPcshellHooks,
            ARRAYSIZE(twinuiPcshellHooks))) {
        Wh_Log(
            L"Shell host: failed to resolve/install "
            L"virtual-desktop symbols");
        return false;
    }

    // These hooks are installed after Wh_ModInit, so explicitly apply the
    // pending hook operations now.
    SetLastError(ERROR_SUCCESS);
    if (!Wh_ApplyHookOperations()) {
        DWORD error = GetLastError();
        Wh_Log(
            L"Shell host: Wh_ApplyHookOperations failed error=%lu",
            error);
        return false;
    }

    g_virtualDesktopHooksInstalled.store(
        true,
        std::memory_order_release);

    Wh_Log(
        L"Shell host: virtual-desktop hooks installed");

    return !RuntimeCancellationRequested();
}

static bool WaitForShellHostRetryDelay() {
    // Keep shell-host startup retries cancellable, fixed, and bounded.
    DWORD waitResult =
        WaitForSingleObject(g_runtimeCancelEvent, 500);

    if (waitResult == WAIT_TIMEOUT) {
        return !g_unloading.load(std::memory_order_acquire);
    }

    if (waitResult == WAIT_FAILED) {
        Wh_Log(
            L"Shell-host retry wait failed error=%lu",
            GetLastError());
    }

    return false;
}

static DWORD WINAPI ShellHostInitThreadProc(void*) {
    Wh_Log(
        L"Shell-host initialization begin");

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
                g_runtimeState.store(
                    RuntimeState::Ready,
                    std::memory_order_release);
                Wh_Log(L"Runtime ready after attempt %d", attempt);
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
            g_runtimeState.store(
                RuntimeState::Stopped,
                std::memory_order_release);
            Wh_Log(
                L"Shell-host hook installation failed; remaining fail-open");
            return 0;
        }

        bool unsupported =
            g_startUnsupported.load(std::memory_order_acquire);
        HRESULT failureHr = g_startHr.load(std::memory_order_relaxed);

        if (g_unloading.load(std::memory_order_acquire) ||
            RuntimeCancellationRequested()) {
            g_runtimeState.store(
                RuntimeState::Stopped,
                std::memory_order_release);
            return 0;
        }

        if (unsupported) {
            g_runtimeState.store(
                RuntimeState::Unsupported,
                std::memory_order_release);

            Wh_Log(
                L"Shell-host runtime unsupported hr=0x%08X; "
                L"remaining fail-open",
                static_cast<unsigned int>(failureHr));
            return 0;
        }

        if (attempt < kMaxRuntimeStartAttempts) {
            Wh_Log(
                L"Shell-host runtime unavailable on attempt %d "
                L"hr=0x%08X; retrying",
                attempt,
                static_cast<unsigned int>(failureHr));

            if (!WaitForShellHostRetryDelay()) {
                g_runtimeState.store(
                    RuntimeState::Stopped,
                    std::memory_order_release);
                return 0;
            }
        }
    }

    g_runtimeState.store(RuntimeState::Stopped, std::memory_order_release);
    Wh_Log(
        L"Shell-host initialization failed after %d attempts; "
        L"remaining fail-open",
        kMaxRuntimeStartAttempts);
    return 0;
}

static void PromoteCurrentProcessToShellHost(
    const wchar_t* reason) {

    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    g_shellHostConfirmed.store(
        true,
        std::memory_order_release);

    RuntimeState state =
        g_runtimeState.load(std::memory_order_acquire);
    if (state == RuntimeState::Ready ||
        state == RuntimeState::Unsupported) {
        return;
    }

    AcquireSRWLockExclusive(&g_runtimeInitLock);

    if (g_unloading.load(std::memory_order_relaxed) ||
        g_runtimeState.load(std::memory_order_relaxed) !=
            RuntimeState::Stopped) {
        ReleaseSRWLockExclusive(&g_runtimeInitLock);
        return;
    }

    // A previous runtime attempt may have failed after the VD hooks were
    // installed. If the primary taskbar is recreated in the same Explorer
    // process, allow another runtime attempt without installing the hooks twice.
    if (g_runtimeInitThread) {
        if (WaitForSingleObject(
                g_runtimeInitThread,
                0) == WAIT_OBJECT_0) {
            CloseHandle(g_runtimeInitThread);
            g_runtimeInitThread = nullptr;
        } else {
            ReleaseSRWLockExclusive(&g_runtimeInitLock);
            return;
        }
    }

    g_runtimeState.store(RuntimeState::Initializing, std::memory_order_release);

    Wh_Log(
        L"Primary Shell_TrayWnd confirmed; "
        L"promoting this Explorer process reason=%s",
        reason ? reason : L"<unknown>");

    g_runtimeInitThread =
        CreateThread(
            nullptr,
            0,
            ShellHostInitThreadProc,
            nullptr,
            0,
            nullptr);

    if (!g_runtimeInitThread) {
        Wh_Log(
            L"Shell-host init CreateThread failed error=%lu",
            GetLastError());
        g_runtimeState.store(RuntimeState::Stopped, std::memory_order_release);
    }

    ReleaseSRWLockExclusive(&g_runtimeInitLock);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t g_createWindowExWOriginal = nullptr;

static HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
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

    HWND hwnd =
        g_createWindowExWOriginal(
            dwExStyle,
            lpClassName,
            lpWindowName,
            dwStyle,
            X,
            Y,
            nWidth,
            nHeight,
            hWndParent,
            hMenu,
            hInstance,
            lpParam);

    if (!hwnd ||
        g_unloading.load(std::memory_order_acquire)) {
        return hwnd;
    }

    // Once the runtime is healthy, the observer becomes effectively free.
    // If initialization failed, keep watching for a recreated primary taskbar
    // so the same Explorer process gets a natural retry opportunity.
    if (g_shellHostConfirmed.load(std::memory_order_acquire)) {
        RuntimeState state =
            g_runtimeState.load(std::memory_order_acquire);
        if (state == RuntimeState::Ready ||
            state == RuntimeState::Unsupported) {
            return hwnd;
        }
    }

    bool isPrimaryTaskbar = false;

    if (lpClassName &&
        !IS_INTRESOURCE(lpClassName)) {
        isPrimaryTaskbar =
            _wcsicmp(
                lpClassName,
                L"Shell_TrayWnd") == 0;
    } else {
        // Registered classes may be passed to CreateWindowExW by atom. Resolve
        // the actual class from the resulting HWND in that case.
        wchar_t className[32] = {};
        if (GetClassNameW(
                hwnd,
                className,
                ARRAYSIZE(className))) {
            isPrimaryTaskbar =
                _wcsicmp(
                    className,
                    L"Shell_TrayWnd") == 0;
        }
    }

    if (!isPrimaryTaskbar) {
        // Shell_SecondaryTrayWnd intentionally lands here and is ignored.
        return hwnd;
    }

    DWORD ownerPid = 0;
    GetWindowThreadProcessId(
        hwnd,
        &ownerPid);

    if (ownerPid != GetCurrentProcessId()) {
        return hwnd;
    }

    // Never do symbol resolution / COM initialization while still inside
    // CreateWindowExW. Only schedule the process promotion here.
    PromoteCurrentProcessToShellHost(
        L"primary Shell_TrayWnd created");

    return hwnd;
}

static void StopRuntimeBeforeUninit() {
    g_unloading.store(true, std::memory_order_release);

    if (g_runtimeCancelEvent) {
        SetEvent(g_runtimeCancelEvent);
    }

    HANDLE initThread = nullptr;

    AcquireSRWLockExclusive(&g_runtimeInitLock);
    initThread = g_runtimeInitThread;
    g_runtimeInitThread = nullptr;
    ReleaseSRWLockExclusive(&g_runtimeInitLock);

    if (initThread) {
        WaitForSingleObject(
            initThread,
            INFINITE);
        CloseHandle(initThread);
    }
}

static void StopRuntime() {
    StopNotificationCache();
    StopWorker();
    g_runtimeState.store(RuntimeState::Stopped, std::memory_order_release);
}

BOOL Wh_ModInit() {
    g_runtimeCancelEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_runtimeCancelEvent) {
        Wh_Log(
            L"Failed to create runtime cancellation event error=%lu",
            GetLastError());
        return FALSE;
    }

    Wh_Log(
        L"Init: installing lightweight "
        L"primary-taskbar observer");

    // Deliberately don't load twinui.pcshell.dll or taskbar.dll here. This code
    // executes in every explorer.exe matched by @include. Non-shell Explorer
    // processes should remain as inert as possible.
    if (!WindhawkUtils::SetFunctionHook(
            CreateWindowExW,
            CreateWindowExW_Hook,
            &g_createWindowExWOriginal)) {
        Wh_Log(
            L"Failed to hook CreateWindowExW");

        CloseHandle(g_runtimeCancelEvent);
        g_runtimeCancelEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // Injection/reload can happen after the primary taskbar was already created,
    // so don't rely solely on observing CreateWindowExW. Detect Shell_TrayWnd
    // directly first; GetShellWindow is retained as an additional shell-host
    // signal for manual reloads and unusual startup ordering.
    HWND primaryTaskbar =
        FindCurrentProcessPrimaryTaskbar();

    if (primaryTaskbar) {
        Wh_Log(
            L"Existing primary taskbar=%p belongs to this Explorer process",
            primaryTaskbar);

        PromoteCurrentProcessToShellHost(
            L"existing primary Shell_TrayWnd");
        return;
    }

    HWND shellWindow =
        FindCurrentProcessShellWindow();

    if (shellWindow) {
        Wh_Log(
            L"Existing shell window=%p belongs to this Explorer process",
            shellWindow);

        PromoteCurrentProcessToShellHost(
            L"existing shell window");
    } else {
        Wh_Log(
            L"No primary taskbar or shell window owned by this PID; "
            L"remaining inert unless Shell_TrayWnd is created");
    }
}

void Wh_ModBeforeUninit() {
    StopRuntimeBeforeUninit();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    StopRuntime();

    if (g_runtimeCancelEvent) {
        CloseHandle(g_runtimeCancelEvent);
        g_runtimeCancelEvent = nullptr;
    }
}
