// ==WindhawkMod==
// @id              prevent-virtual-desktop-stealing
// @name            Prevent Window Activation from Stealing Virtual Desktop
// @description     Redirect cross-desktop window activation to the current desktop.
// @version         0.3.7
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

Designed for Windows 11 22H2 and newer. This mod relies on undocumented Windows
shell interfaces, so future Windows updates may require adjustments.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <objbase.h>
#include <servprov.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <atomic>
#include <cstdint>

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

static const CLSID kClsidVirtualDesktopManager = {
    0xAA509086, 0x5CA9, 0x4C25,
    {0x8F, 0x95, 0x58, 0x9D, 0x3C, 0x07, 0xB4, 0x8A}
};

static const IID kIidIServiceProvider = {
    0x6D5140C1, 0x7436, 0x11CE,
    {0x80, 0x34, 0x00, 0xAA, 0x00, 0x60, 0x09, 0xFA}
};

static const IID kIidIUnknown = {
    0x00000000, 0x0000, 0x0000,
    {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
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

// Windows 11 22H2/23H2 IVirtualDesktopManagerInternal IID.
static const IID kIidVirtualDesktopManagerInternal22H2 = {
    0xA3175F2D, 0x239C, 0x4BD2,
    {0x8A, 0xA0, 0xEE, 0xBA, 0x8B, 0x0B, 0x13, 0x8E}
};

static const IID kIidApplicationViewCollection = {
    0x1841C6D7, 0x4F9D, 0x42C0,
    {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}
};

static const IID kIidVirtualDesktopManager = {
    0xA5CD92FF, 0x29BE, 0x454C,
    {0x8D, 0x04, 0xD8, 0x28, 0x79, 0xFB, 0x3F, 0x1B}
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

struct IVirtualDesktopManagerPublic : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsWindowOnCurrentVirtualDesktop(
        HWND topLevelWindow,
        BOOL* onCurrentDesktop) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetWindowDesktopId(
        HWND topLevelWindow,
        GUID* desktopId) = 0;

    virtual HRESULT STDMETHODCALLTYPE MoveWindowToDesktop(
        HWND topLevelWindow,
        REFGUID desktopId) = 0;
};

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

// Disable Virtual Desktop Transition Animation causes the native
// Win+Ctrl+Left/Right path to call SwitchDesktopInternal synchronously.
// The probe confirmed that the call occurs while _CycleInDirection is still
// active, so a thread-local nesting depth is sufficient; no timing grace
// period or physical-key heuristic is used.
static thread_local int g_hotkeyCycleDepth = 0;

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
};

static SRWLOCK g_explicitSwitchLock = SRWLOCK_INIT;
static ExplicitSwitchTransaction g_explicitSwitch = {};
static std::atomic<uint64_t> g_nextExplicitSwitchSequence{1};

// SwitchDesktopWithAnimation synchronously calls SwitchDesktop on current
// builds. Only the outermost public switch entry should create a transaction.
static thread_local int g_explicitSwitchEntryDepth = 0;

static bool IsExplicitHotkeySwitchContext() {
    return g_hotkeyCycleDepth > 0;
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

    const uint64_t sequence =
        g_nextExplicitSwitchSequence.fetch_add(
            1,
            std::memory_order_relaxed);

    AcquireSRWLockExclusive(&g_explicitSwitchLock);
    g_explicitSwitch.active = true;
    g_explicitSwitch.sourceDesktopId = sourceId;
    g_explicitSwitch.targetDesktopId = targetId;
    g_explicitSwitch.sequence = sequence;
    ReleaseSRWLockExclusive(&g_explicitSwitchLock);

    wchar_t sourceText[64] = {};
    wchar_t targetText[64] = {};
    GuidToString(sourceId, sourceText, ARRAYSIZE(sourceText));
    GuidToString(targetId, targetText, ARRAYSIZE(targetText));

    Wh_Log(
        L"Explicit switch transaction #%llu begin route=%s "
        L"source=%s target=%s",
        static_cast<unsigned long long>(sequence),
        route ? route : L"<unknown>",
        sourceText,
        targetText);

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

    AcquireSRWLockShared(&g_explicitSwitchLock);
    current = g_explicitSwitch;
    ReleaseSRWLockShared(&g_explicitSwitchLock);

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

static void FinishExplicitSwitchTransaction(
    IVirtualDesktop* switchedDesktop) {

    if (!switchedDesktop) {
        return;
    }

    GUID switchedId = {};
    if (FAILED(switchedDesktop->GetId(&switchedId))) {
        return;
    }

    ExplicitSwitchTransaction finished = {};
    bool hadTransaction = false;
    bool reachedTarget = false;

    AcquireSRWLockExclusive(&g_explicitSwitchLock);
    if (g_explicitSwitch.active) {
        finished = g_explicitSwitch;
        hadTransaction = true;
        reachedTarget =
            GuidEqual(
                switchedId,
                g_explicitSwitch.targetDesktopId);
        // Any completed desktop-switch notification ends the current
        // transaction. A non-target completion means it was superseded or the
        // shell chose another route; don't leave stale suppression armed.
        g_explicitSwitch = {};
    }
    ReleaseSRWLockExclusive(&g_explicitSwitchLock);

    if (!hadTransaction) {
        return;
    }

    wchar_t switchedText[64] = {};
    wchar_t targetText[64] = {};
    GuidToString(switchedId, switchedText, ARRAYSIZE(switchedText));
    GuidToString(
        finished.targetDesktopId,
        targetText,
        ARRAYSIZE(targetText));

    Wh_Log(
        L"Explicit switch transaction #%llu %s "
        L"switched=%s target=%s",
        static_cast<unsigned long long>(finished.sequence),
        reachedTarget ? L"complete" : L"ended on unexpected desktop",
        switchedText,
        targetText);
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

using CycleInDirection_t =
    HRESULT (*)(void* pThis, int direction);

static CycleInDirection_t g_cycleInDirectionOriginal = nullptr;

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

static HRESULT CycleInDirection_Hook(
    void* pThis,
    int direction) {

    ++g_hotkeyCycleDepth;

    Wh_Log(
        L"HOTKEY-CYCLE begin direction=%d depth=%d",
        direction,
        g_hotkeyCycleDepth);

    HRESULT hr =
        g_cycleInDirectionOriginal(pThis, direction);

    Wh_Log(
        L"HOTKEY-CYCLE end direction=%d hr=0x%08X depth=%d",
        direction,
        static_cast<unsigned int>(hr),
        g_hotkeyCycleDepth);

    --g_hotkeyCycleDepth;
    return hr;
}

// -----------------------------------------------------------------------------
// Current-desktop cache via the private VD notification service.
// -----------------------------------------------------------------------------

static HANDLE g_notificationThread = nullptr;
static DWORD g_notificationThreadId = 0;
static HANDLE g_notificationReadyEvent = nullptr;
static std::atomic<bool> g_notificationReady = false;

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

        if (IsEqualIID(riid, kIidIUnknown) ||
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

            wchar_t oldText[64] = {};
            wchar_t newText[64] = {};
            if (SUCCEEDED(oldHr)) {
                GuidToString(oldId, oldText, ARRAYSIZE(oldText));
            }
            GuidToString(newId, newText, ARRAYSIZE(newText));

            Wh_Log(
                L"Current desktop cache updated old=%s new=%s",
                SUCCEEDED(oldHr) ? oldText : L"<unknown>",
                newText);
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
    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool shouldUninitialize = SUCCEEDED(coHr);

    if (FAILED(coHr) && coHr != RPC_E_CHANGED_MODE) {
        Wh_Log(
            L"Notification CoInitializeEx failed hr=0x%08X",
            static_cast<unsigned int>(coHr));
        SetEvent(g_notificationReadyEvent);
        return 0;
    }

    // Ensure this STA owns a message queue before registration.
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    IServiceProvider* serviceProvider = nullptr;
    IVirtualDesktopManagerInternal* managerInternal = nullptr;
    IVirtualDesktopNotificationService* notificationService = nullptr;
    VirtualDesktopNotificationListener* listener = nullptr;
    DWORD cookie = 0;

    HRESULT hr = CoCreateInstance(
        kClsidImmersiveShell,
        nullptr,
        CLSCTX_LOCAL_SERVER,
        kIidIServiceProvider,
        reinterpret_cast<void**>(&serviceProvider));

    if (SUCCEEDED(hr) && serviceProvider) {
        hr = serviceProvider->QueryService(
            kClsidVirtualDesktopManagerInternal,
            kIidVirtualDesktopManagerInternal24H2,
            reinterpret_cast<void**>(&managerInternal));

        if (FAILED(hr) || !managerInternal) {
            managerInternal = nullptr;
            hr = serviceProvider->QueryService(
                kClsidVirtualDesktopManagerInternal,
                kIidVirtualDesktopManagerInternal22H2,
                reinterpret_cast<void**>(&managerInternal));
        }
    }

    if (SUCCEEDED(hr) && managerInternal) {
        auto getCurrentDesktop = GetVTableFunction<
            HRESULT(STDMETHODCALLTYPE*)(
                void*,
                IVirtualDesktop**)>(
            managerInternal,
            6);

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

    if (SUCCEEDED(hr) && serviceProvider) {
        hr = serviceProvider->QueryService(
            kServiceVirtualDesktopNotification,
            kIidVirtualDesktopNotificationService,
            reinterpret_cast<void**>(&notificationService));
    }

    if (SUCCEEDED(hr) && notificationService) {
        listener = new VirtualDesktopNotificationListener();
        hr = notificationService->Register(listener, &cookie);
    }

    if (SUCCEEDED(hr) && cookie) {
        GUID currentId = {};
        if (LoadCurrentDesktopId(&currentId)) {
            g_notificationReady.store(true);
        } else {
            hr = E_FAIL;
        }
    }

    Wh_Log(
        L"Notification/cache init hr=0x%08X cookie=%lu ready=%d",
        static_cast<unsigned int>(hr),
        cookie,
        g_notificationReady.load());

    SetEvent(g_notificationReadyEvent);

    if (g_notificationReady.load()) {
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
    g_notificationReadyEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_notificationReadyEvent) {
        Wh_Log(
            L"Create notification-ready event failed error=%lu",
            GetLastError());
        return false;
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
        Wh_Log(
            L"Notification CreateThread failed error=%lu",
            GetLastError());
        return false;
    }

    DWORD waitResult =
        WaitForSingleObject(g_notificationReadyEvent, 5000);

    if (waitResult != WAIT_OBJECT_0 ||
        !g_notificationReady.load()) {
        Wh_Log(
            L"Notification/current-desktop cache unavailable "
            L"(waitResult=%lu)",
            waitResult);
        return false;
    }

    return true;
}

static void StopNotificationCache() {
    g_notificationReady.store(false, std::memory_order_release);

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

struct WorkerComState {
    IServiceProvider* serviceProvider = nullptr;
    IVirtualDesktopManagerInternal* managerInternal = nullptr;
    IApplicationViewCollection* viewCollection = nullptr;
    IVirtualDesktopManagerPublic* publicManager = nullptr;
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
        kIidIServiceProvider,
        reinterpret_cast<void**>(&state->serviceProvider));

    if (FAILED(hr) || !state->serviceProvider) {
        Wh_Log(
            L"Worker: CoCreateInstance(ImmersiveShell) failed "
            L"hr=0x%08X",
            static_cast<unsigned int>(hr));
        return false;
    }

    hr = state->serviceProvider->QueryService(
        kClsidVirtualDesktopManagerInternal,
        kIidVirtualDesktopManagerInternal24H2,
        reinterpret_cast<void**>(&state->managerInternal));

    if (FAILED(hr) || !state->managerInternal) {
        state->managerInternal = nullptr;
        hr = state->serviceProvider->QueryService(
            kClsidVirtualDesktopManagerInternal,
            kIidVirtualDesktopManagerInternal22H2,
            reinterpret_cast<void**>(&state->managerInternal));
    }

    if (FAILED(hr) || !state->managerInternal) {
        Wh_Log(
            L"Worker: QueryService(managerInternal) failed "
            L"hr=0x%08X",
            static_cast<unsigned int>(hr));
        return false;
    }

    hr = state->serviceProvider->QueryService(
        kIidApplicationViewCollection,
        kIidApplicationViewCollection,
        reinterpret_cast<void**>(&state->viewCollection));

    if (FAILED(hr) || !state->viewCollection) {
        Wh_Log(
            L"Worker: QueryService(viewCollection) failed "
            L"hr=0x%08X",
            static_cast<unsigned int>(hr));
        return false;
    }

    hr = CoCreateInstance(
        kClsidVirtualDesktopManager,
        nullptr,
        CLSCTX_INPROC_SERVER,
        kIidVirtualDesktopManager,
        reinterpret_cast<void**>(&state->publicManager));

    if (FAILED(hr) || !state->publicManager) {
        Wh_Log(
            L"Worker: CoCreateInstance(public manager) failed "
            L"hr=0x%08X",
            static_cast<unsigned int>(hr));
        return false;
    }

    return true;
}

// These slots and signatures are shared by the version-gating IIDs above.
// Older monitor-aware interface versions aren't queried because their method
// signatures differ.
static constexpr int kVtableMoveViewToDesktop = 4;
static constexpr int kVtableGetCurrentDesktop = 6;

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

    wchar_t requestedText[64] = {};
    GuidToString(
        request.requestedDesktopId,
        requestedText,
        ARRAYSIZE(requestedText));

    Wh_Log(
        L"[#%llu] Rescue begin hwnd=%p requested=%s",
        static_cast<unsigned long long>(request.sequence),
        request.hwnd,
        requestedText);

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
        wchar_t expectedSourceText[64] = {};
        wchar_t actualSourceText[64] = {};
        GuidToString(
            request.sourceDesktopId,
            expectedSourceText,
            ARRAYSIZE(expectedSourceText));
        GuidToString(
            actualCurrentId,
            actualSourceText,
            ARRAYSIZE(actualSourceText));

        Wh_Log(
            L"[#%llu] Abort: current desktop changed before rescue "
            L"(source=%s now=%s)",
            static_cast<unsigned long long>(request.sequence),
            expectedSourceText,
            actualSourceText);

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
        SetForegroundWindow(request.hwnd);
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
        SetForegroundWindow(request.hwnd);
        return;
    }

    if (!GuidEqual(
            windowDesktopId,
            request.requestedDesktopId)) {
        wchar_t windowText[64] = {};
        GuidToString(
            windowDesktopId,
            windowText,
            ARRAYSIZE(windowText));

        Wh_Log(
            L"[#%llu] Abort: foreground window desktop "
            L"doesn't match requested switch target "
            L"(window=%s requested=%s)",
            static_cast<unsigned long long>(request.sequence),
            windowText,
            requestedText);

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
        ShowWindow(request.hwnd, SW_RESTORE);
    }

    SetForegroundWindow(request.hwnd);
}

static DWORD WINAPI WorkerThreadProc(void*) {
    HRESULT coHr =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    const bool shouldUninitialize =
        SUCCEEDED(coHr);

    if (FAILED(coHr) &&
        coHr != RPC_E_CHANGED_MODE) {
        Wh_Log(
            L"Worker: CoInitializeEx failed hr=0x%08X",
            static_cast<unsigned int>(coHr));

        SetEvent(g_workerReadyEvent);
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

    g_workerReady.store(true);
    SetEvent(g_workerReadyEvent);

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

    if (!g_workerReady.load() ||
        !pThis ||
        !requestedDesktop) {
        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    // Critical compatibility path:
    // When Disable Virtual Desktop Transition Animation is enabled, the native
    // Win+Ctrl+Left/Right handler can select SwitchDesktopInternal as its
    // no-animation path. Since we're inside (or immediately after) the actual
    // _CycleInDirection user action, this is intentional and must not be
    // converted into a teleport.
    if (IsExplicitHotkeySwitchContext()) {
        Wh_Log(
            L"ALLOW SwitchDesktopInternal: "
            L"explicit HOTKEY-CYCLE context depth=%d",
            g_hotkeyCycleDepth);

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

        wchar_t targetText[64] = {};
        GuidToString(
            explicitSnapshot.targetDesktopId,
            targetText,
            ARRAYSIZE(targetText));

        Wh_Log(
            L"SUPPRESS SwitchDesktopInternal: stale source rebound "
            L"during explicit transaction #%llu target=%s",
            static_cast<unsigned long long>(
                explicitSnapshot.sequence),
            targetText);

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

    HWND foreground = GetForegroundWindow();

    // If there isn't a plausible app window to rescue, don't interfere with
    // an unknown internal switch path.
    if (!IsRescueCandidate(foreground)) {
        Wh_Log(
            L"SwitchDesktopInternal allowed: "
            L"no eligible foreground rescue candidate");

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
        GetWindowThreadProcessId(foreground, &candidatePid);

    if (!candidatePid || !candidateTid) {
        Wh_Log(
            L"SwitchDesktopInternal allowed: "
            L"failed to capture candidate HWND identity");

        return g_switchDesktopInternalOriginal(
            pThis,
            requestedDesktop);
    }

    wchar_t sourceText[64] = {};
    wchar_t requestedText[64] = {};
    GuidToString(
        sourceDesktopId,
        sourceText,
        ARRAYSIZE(sourceText));
    GuidToString(
        requestedId,
        requestedText,
        ARRAYSIZE(requestedText));

    Wh_Log(
        L"Candidate SwitchDesktopInternal "
        L"source=%s requested=%s foreground=%p",
        sourceText,
        requestedText,
        foreground);

    LogWindow(L"candidate", foreground);

    if (!QueueRescue(
            foreground,
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
    // safely queued. Every failure before this point fails open.
    return S_OK;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle.
// -----------------------------------------------------------------------------

static bool StartWorker() {
    g_requestEvent =
        CreateEventW(
            nullptr,
            FALSE,
            FALSE,
            nullptr);

    g_stopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    g_workerReadyEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_requestEvent ||
        !g_stopEvent ||
        !g_workerReadyEvent) {
        Wh_Log(
            L"Failed to create worker events");
        return false;
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
        Wh_Log(
            L"CreateThread failed error=%lu",
            GetLastError());
        return false;
    }

    DWORD waitResult =
        WaitForSingleObject(
            g_workerReadyEvent,
            5000);

    if (waitResult != WAIT_OBJECT_0 ||
        !g_workerReady.load()) {
        Wh_Log(
            L"Worker initialization failed "
            L"(waitResult=%lu)",
            waitResult);
        return false;
    }

    return true;
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

static std::atomic<int> g_runtimeState{0};
// 0 = dormant/stopped, 1 = shell-host initialization in progress, 2 = ready

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_shellHostConfirmed{false};
static std::atomic<bool> g_virtualDesktopHooksInstalled{false};

static SRWLOCK g_runtimeInitLock = SRWLOCK_INIT;
static HANDLE g_runtimeInitThread = nullptr;

static BOOL CALLBACK FindCurrentProcessPrimaryTaskbarEnumProc(
    HWND hwnd,
    LPARAM lParam) {

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid) ||
        pid != GetCurrentProcessId()) {
        return TRUE;
    }

    wchar_t className[32] = {};
    if (!GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className))) {
        return TRUE;
    }

    // Deliberately accept only the primary taskbar. Secondary taskbars are
    // Shell_SecondaryTrayWnd and are not shell-process identity markers.
    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hwnd;
        return FALSE;
    }

    return TRUE;
}

static HWND FindCurrentProcessPrimaryTaskbar() {
    HWND result = nullptr;

    EnumWindows(
        FindCurrentProcessPrimaryTaskbarEnumProc,
        reinterpret_cast<LPARAM>(&result));

    return result;
}

static bool InstallVirtualDesktopHooks() {
    if (g_virtualDesktopHooksInstalled.load(
            std::memory_order_acquire)) {
        return true;
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

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK twinuiPcshellHooks[] = {
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
                L"private: long __cdecl "
                L"CVirtualDesktopHotkeyHandler::_CycleInDirection("
                L"enum VirtualDesktopSwitchDirection)"
            },
            reinterpret_cast<void**>(
                &g_cycleInDirectionOriginal),
            reinterpret_cast<void*>(
                CycleInDirection_Hook)
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
    Wh_ApplyHookOperations();

    g_virtualDesktopHooksInstalled.store(
        true,
        std::memory_order_release);

    Wh_Log(
        L"Shell host: virtual-desktop hooks installed");

    return true;
}

static bool WaitForShellHostRetryDelay() {
    // Keep unload latency short while allowing the shell's COM services time
    // to register after a fresh Explorer start.
    for (int i = 0; i < 10; ++i) {
        if (g_unloading.load(std::memory_order_acquire)) {
            return false;
        }

        Sleep(50);
    }

    return !g_unloading.load(std::memory_order_acquire);
}

static DWORD WINAPI ShellHostInitThreadProc(void*) {
    Wh_Log(
        L"Shell-host initialization begin");

    if (g_unloading.load(std::memory_order_acquire)) {
        g_runtimeState.store(0, std::memory_order_release);
        return 0;
    }

    if (!InstallVirtualDesktopHooks()) {
        g_runtimeState.store(0, std::memory_order_release);

        Wh_Log(
            L"Shell-host initialization failed: "
            L"VD hooks unavailable; remaining fail-open");
        return 0;
    }

    constexpr int kMaxRuntimeStartAttempts = 30;

    for (int attempt = 1; attempt <= kMaxRuntimeStartAttempts; ++attempt) {
        bool workerStarted = StartWorker();
        bool cacheStarted = false;

        if (workerStarted &&
            !g_unloading.load(std::memory_order_acquire)) {
            cacheStarted = StartNotificationCache();
        }

        if (workerStarted && cacheStarted &&
            !g_unloading.load(std::memory_order_acquire)) {
            g_runtimeState.store(2, std::memory_order_release);

            Wh_Log(
                L"Runtime ready after attempt %d: primary shell Explorer "
                L"confirmed, source-desktop cache active, bounded rescue "
                L"queue active",
                attempt);
            return 0;
        }

        StopNotificationCache();
        StopWorker();

        if (g_unloading.load(std::memory_order_acquire)) {
            g_runtimeState.store(0, std::memory_order_release);
            return 0;
        }

        if (attempt < kMaxRuntimeStartAttempts) {
            Wh_Log(
                L"Shell-host runtime unavailable on attempt %d; retrying",
                attempt);

            if (!WaitForShellHostRetryDelay()) {
                g_runtimeState.store(0, std::memory_order_release);
                return 0;
            }
        }
    }

    g_runtimeState.store(0, std::memory_order_release);
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

    if (g_runtimeState.load(std::memory_order_acquire) == 2) {
        return;
    }

    AcquireSRWLockExclusive(&g_runtimeInitLock);

    if (g_unloading.load(std::memory_order_relaxed) ||
        g_runtimeState.load(std::memory_order_relaxed) != 0) {
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

    g_runtimeState.store(1, std::memory_order_release);

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
        g_runtimeState.store(0, std::memory_order_release);
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
    if (g_shellHostConfirmed.load(std::memory_order_acquire) &&
        g_runtimeState.load(std::memory_order_acquire) == 2) {
        return hwnd;
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
    g_runtimeState.store(0, std::memory_order_release);
}

BOOL Wh_ModInit() {
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
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // Manual enable/reload: the shell Explorer may already have created its
    // primary taskbar before our CreateWindowExW observer was installed.
    HWND primaryTaskbar =
        FindCurrentProcessPrimaryTaskbar();

    if (primaryTaskbar) {
        Wh_Log(
            L"Existing primary Shell_TrayWnd=%p "
            L"belongs to this Explorer process",
            primaryTaskbar);

        PromoteCurrentProcessToShellHost(
            L"existing primary Shell_TrayWnd");
    } else {
        Wh_Log(
            L"No primary Shell_TrayWnd owned by this PID; "
            L"remaining inert unless one is created");
    }
}

void Wh_ModBeforeUninit() {
    StopRuntimeBeforeUninit();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    StopRuntime();
}
