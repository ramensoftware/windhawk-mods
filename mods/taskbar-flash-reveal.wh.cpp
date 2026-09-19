// ==WindhawkMod==
// @id              taskbar-flash-reveal
// @name            Taskbar flash reveal
// @description     Reveal the auto-hidden taskbar when an app flashes, then restore auto-hide after you view the app, leave the taskbar, or open Start
// @version         1.0
// @author          coderLyon
// @github          https://github.com/coderLyon
// @license         GPL-3.0-only
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion -lole32
// ==/WindhawkMod==

// SPDX-License-Identifier: GPL-3.0-only
// Taskbar integration adapted from m417z's Taskbar auto-hide fine tuning mod,
// published under GNU GPL v3.0:
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-auto-hide-keyboard-only.wh.cpp
// License text: https://www.gnu.org/licenses/gpl-3.0.html

// ==WindhawkModReadme==
/*
# Taskbar flash reveal

Keep app notifications visible when you use an auto-hidden taskbar. When an
app such as WeChat flashes its taskbar button, this mod reveals the taskbar
on that app's monitor and keeps it visible until you respond.

Requires the standard Windows 11 taskbar on x64 Windows, with **Automatically
hide the taskbar** enabled in Windows Settings.

## When the taskbar hides again

- Move the pointer onto the taskbar, then away from it.
- Switch to the app that requested attention, including with a keyboard
  shortcut. Other apps' pending reminders continue to keep the taskbar visible.
- Open the Start menu. This ends the current hold and lets Windows handle
  visibility during and after the menu. Notifications do not start a new hold
  while Start is open.
- Wait for **Max visible time**, which defaults to 10 minutes. Set it to 0
  to disable this timeout.

These actions restore Windows' normal auto-hide behavior. If the pointer is
still on the taskbar, Windows can keep it visible until the pointer leaves.
The mod does not move the pointer or bring a notification app to the foreground.

## Choose which apps can reveal the taskbar

By default, all apps requesting attention can trigger a reveal. Select
**Chat apps (white list)** to restrict triggering to **App white list**.

Use **Excluded processes** to block specific apps in either mode. For example,
`explorer.exe, Feishu.exe` excludes Feishu while retaining the default Explorer
exclusion. Enter executable names separated by commas; matching ignores case.
This setting affects notification triggers, not Windhawk's injection exclusions.

Disable **Keep visible until the mouse leaves** to use **Fallback reveal
duration** instead. Opening Start or viewing all pending apps still ends the hold.

## Compatibility and limitations

- A real Windows taskbar flash is required. A notification-area icon animation,
  an unread badge, or a toast alone does not trigger this mod.
- An app is considered viewed when a window in the same process becomes the
  foreground window. Flashing stopping in the background does not end the hold.
- Explorer's internal taskbar interfaces can change with Windows updates.
  Mods that also change taskbar auto-hide may conflict with this one.
- Enable **Debug logging** when reporting a problem. Include your Windows and
  Windhawk versions, the app's executable name, and the steps to reproduce it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable the mod
  $description: >-
    Master switch. When disabled, the mod does nothing and the taskbar behaves
    exactly as it does without the mod.
- triggerScope: allApplications
  $name: Trigger scope
  $description: >-
    Which apps may trigger the reveal. "All applications that ask for attention"
    matches the flashing itself. "Chat apps (white list)" limits the reveal to
    the processes listed below, so unrelated flashes (a finished download, a
    finished build) do not pop the taskbar up.
  $options:
  - allApplications: All applications that ask for attention
  - chatAppsWhitelist: Chat apps (white list)
- appWhitelist: WeChat.exe, Weixin.exe, WeChatApp.exe, QQ.exe, QQNT.exe
  $name: App white list
  $description: >-
    Comma-separated process names (case-insensitive, spaces are trimmed), used
    only with the "Chat apps (white list)" trigger scope. WeChat 4.x uses
    Weixin.exe while WeChat 3.x uses WeChat.exe - both are listed.
- excludeProcesses: explorer.exe
  $name: Excluded processes
  $description: >-
    Comma-separated process names that never trigger the reveal, preventing the
    mod from reacting to itself. csrss.exe and dwm.exe are always ignored.
- hideOnPointerLeave: true
  $name: Keep visible until the mouse leaves
  $description: >-
    Keep the taskbar visible until the pointer enters and leaves it. Opening
    Start, viewing all pending apps, or reaching Max visible time also ends
    the hold. When disabled, use Fallback reveal duration instead.
- maxVisibleMs: 600000
  $name: Max visible time (ms)
  $description: >-
    Safety valve: the maximum time in milliseconds the taskbar may stay visible
    after a reveal, for the case where the mouse never goes near it. Set to 0 to
    never time out.
- fallbackRevealDurationMs: 3000
  $name: Fallback reveal duration (ms)
  $description: >-
    Used only when "Keep visible until the mouse leaves" is disabled.
- minIntervalMs: 800
  $name: Minimum interval per window (ms)
  $description: >-
    Minimum time in milliseconds between two reveals caused by the same window,
    so a burst of messages does not retrigger the animation repeatedly.
- debugLog: false
  $name: Debug logging
  $description: >-
    Log every attention signal and every decision. Enable this when reporting a
    problem, or when checking why an app did not trigger the reveal.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellapi.h>
#include <shobjidl.h>
#include <windows.h>

#include <algorithm>
#include <atomic>
#include <new>
#include <string>
#include <unordered_map>
#include <vector>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct {
    bool enabled;
    bool chatAppsWhitelist;
    std::wstring appWhitelist;
    std::wstring excludeProcesses;
    bool hideOnPointerLeave;
    int maxVisibleMs;
    int fallbackRevealDurationMs;
    int minIntervalMs;
    bool debugLog;
} g_settings;

// ---------------------------------------------------------------------------
// Constants and state
// ---------------------------------------------------------------------------

// Taskbar timer ids used by Explorer's auto-hide implementation. Same values
// as the official windhawk-mods.
enum {
    kTrayUITimerHide = 2,
    kTrayUITimerUnhide = 3,
};

static const UINT kWatchdogTimerId = 0x5F1E;
static const UINT kAttentionPollMs = 300;
static const UINT kWarnIntervalMs = 60000;

// HWND -> TrayUI pThis, captured from TrayUI::WndProc and
// CSecondaryTray::v_WndProc.
std::unordered_map<HWND, void*> g_hwndToTrayPThis;
std::unordered_map<HWND, void*> g_hwndToSecondaryPThis;
// HWND -> ViewCoordinator pThis, captured from ShouldTaskbarBeExpanded.
std::unordered_map<HWND, void*> g_hwndToViewCoordinator;

// Symbols that we only read (no hook installed). These must be plain `void*`
// targets paired with a nullptr hook, which is the pattern used by the official
// windhawk-mods; the SYMBOL_HOOK template then deduces Prototype = void.
void* g_trayVftableITrayComponentHost = nullptr;
void* g_trayUnhideAddr = nullptr;
void* g_secondaryUnhideAddr = nullptr;
void* g_viewCoordinatorIsExpandedAddr = nullptr;
void* g_viewCoordinatorPointerOverAddr = nullptr;

// Armed state: the mod revealed the taskbar and is holding it visible.
bool g_armed = false;
std::vector<HWND> g_armedTaskbars;
DWORD g_armedAt = 0;
DWORD g_pointerOverSince = 0;
UINT g_nativeHideBlockCount = 0;
// Flash source HWND -> process ID. App activation acknowledges that process;
// other apps' pending reminders still hold the taskbar. Taskbar-thread only.
std::unordered_map<HWND, DWORD> g_pendingAttention;

// Per-window cooldown for reveal triggers.
std::unordered_map<HWND, DWORD> g_lastTriggerByWindow;
DWORD g_lastCooldownSweep = 0;

// Rate-limited diagnostics.
DWORD g_lastSymbolWarning = 0;
DWORD g_lastExcludedLog = 0;

// Guard against reacting to our own actions.
bool g_inAttentionHandling = false;

// UI-thread plumbing.
std::atomic<DWORD> g_taskbarThreadId{0};
std::atomic<bool> g_viewSymbolsAttempted{false};
std::atomic<bool> g_viewSymbolsHooked{false};
std::atomic_flag g_viewHookBusy = ATOMIC_FLAG_INIT;
HWND g_receiverWnd = nullptr;  // Owned and destroyed on the taskbar thread.
std::atomic<HWND> g_receiverForUnload{nullptr};
UINT g_shellHookMsg = 0;
UINT g_shutdownMsg = 0;
HINSTANCE g_modInstance = nullptr;
constexpr PCWSTR kReceiverClass = L"WindhawkFlashRevealReceiver_" WH_MOD_ID;
std::atomic<bool> g_stopping{false};
HANDLE g_initWorker = nullptr;
HANDLE g_stopEvent = nullptr;
struct NativeHideTimer {
    UINT interval;
    TIMERPROC callback;
};
std::unordered_map<HWND, NativeHideTimer> g_hideIntervals;
UINT g_captureThisMsg = 0;
UINT g_launcherVisibilityMsg = 0;
// Accessed only on the taskbar thread; COM callbacks post to its receiver.
bool g_launcherVisible = false;
IAppVisibility* g_appVisibility = nullptr;
IAppVisibilityEvents* g_launcherEvents = nullptr;
DWORD g_launcherCookie = 0;
bool g_launcherAdvised = false;
bool g_launcherComInitialized = false;

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------

void ApplySettings();
void InitializeReceiver(HWND hTaskbarWnd);
void HandleLauncherVisibility(bool visible);
void RefreshLauncherVisibility();
void StartLauncherWatch();
void StopLauncherWatch();
void OnAttention(HWND hWnd, const wchar_t* source);
void OnAttentionPid(HWND hWnd, DWORD dwProcessId, const wchar_t* source);
bool HookTaskbarViewSymbols();
HMODULE GetTaskbarViewModuleHandle();

using TrayUI_Unhide_t =
    void(WINAPI*)(void* pThis, int trayUnhideFlags, int unhideRequest);
using ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_t =
    void(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, bool isPointerOver,
                  int inputDeviceKind);
using ViewCoordinator_IsExpanded_t = bool(WINAPI*)(void* pThis,
                                                   HWND hMMTaskbarWnd);

// The addresses are stored as void* because that is what SYMBOL_HOOK expects for
// symbols without a hook; cast to the real prototype only where they are called.
TrayUI_Unhide_t TrayUI_Unhide() {
    return (TrayUI_Unhide_t)g_trayUnhideAddr;
}

ViewCoordinator_IsExpanded_t ViewCoordinator_IsExpanded() {
    return g_viewSymbolsHooked.load() ?
        (ViewCoordinator_IsExpanded_t)g_viewCoordinatorIsExpandedAddr : nullptr;
}

ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_t
ViewCoordinator_HandlePointerOver() {
    return g_viewSymbolsHooked.load() ?
        (ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_t)
        g_viewCoordinatorPointerOverAddr : nullptr;
}

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[64];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

HMONITOR GetPrimaryMonitor() {
    POINT pt = {0, 0};
    return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId = 0;
            WCHAR szClassName[64];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) &&
                _wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

void FindAllTaskbarWindows(std::vector<HWND>* taskbars) {
    taskbars->clear();

    HWND hPrimary = FindCurrentProcessTaskbarWnd();
    if (!hPrimary) {
        return;
    }

    taskbars->push_back(hPrimary);

    DWORD taskbarThreadId = GetWindowThreadProcessId(hPrimary, nullptr);
    if (!taskbarThreadId) {
        return;
    }

    EnumThreadWindows(
        taskbarThreadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto* pTaskbars = reinterpret_cast<std::vector<HWND>*>(lParam);
            WCHAR szClassName[64];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) &&
                _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                pTaskbars->push_back(hWnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(taskbars));
}

// Returns nullptr when the vtable layout is unexpected, instead of looping
// forever or dereferencing garbage. Walks the object's leading pointers looking
// for the address of the target vtable, which is how the official windhawk-mods
// navigate from TrayUI to its ITrayComponentHost sub-object.
void* QueryViaVtable(void* object, void* vtable) {
    if (!object || !vtable) {
        return nullptr;
    }

    void* ptr = object;
    for (int i = 0; i < 16; i++) {
        if (*(void**)ptr == vtable) {
            return ptr;
        }
        ptr = (void**)ptr + 1;
    }
    return nullptr;
}

std::wstring GetProcessFileName(DWORD dwProcessId) {
    std::wstring result;
    if (!dwProcessId) {
        return result;
    }

    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return result;
    }

    WCHAR szPath[MAX_PATH];
    DWORD dwSize = ARRAYSIZE(szPath);
    if (QueryFullProcessImageName(hProcess, 0, szPath, &dwSize)) {
        PCWSTR pszFileName = wcsrchr(szPath, L'\\');
        result = pszFileName ? pszFileName + 1 : szPath;
    }

    CloseHandle(hProcess);
    return result;
}

void SplitList(PCWSTR text, std::vector<std::wstring>* out) {
    out->clear();
    if (!text) {
        return;
    }

    std::wstring current;
    auto flush = [&]() {
        // Trim spaces and stray quotes so "A.exe, B.exe" works as expected.
        size_t begin = current.find_first_not_of(L" \t\"'");
        if (begin == std::wstring::npos) {
            current.clear();
            return;
        }
        size_t end = current.find_last_not_of(L" \t\"'");
        out->push_back(current.substr(begin, end - begin + 1));
        current.clear();
    };

    for (PCWSTR p = text; *p; p++) {
        if (*p == L',' || *p == L';' || *p == L'\r' || *p == L'\n' ||
            *p == L'\t') {
            flush();
        } else {
            current.push_back(*p);
        }
    }
    flush();
}

bool ProcessNameInList(const std::wstring& name, const std::wstring& list) {
    if (name.empty() || list.empty()) {
        return false;
    }

    std::vector<std::wstring> parts;
    SplitList(list.c_str(), &parts);
    for (auto& part : parts) {
        if (_wcsicmp(part.c_str(), name.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

bool IsAlwaysIgnoredProcess(const std::wstring& name) {
    return _wcsicmp(name.c_str(), L"csrss.exe") == 0 ||
           _wcsicmp(name.c_str(), L"dwm.exe") == 0;
}

bool HasTaskbarAutoHide() {
    APPBARDATA abd = {sizeof(abd)};
    return (SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE) != 0;
}

bool IsTaskbarExpanded(HWND hTaskbarWnd) {
    auto it = g_hwndToViewCoordinator.find(hTaskbarWnd);
    auto isExpanded = ViewCoordinator_IsExpanded();
    if (it != g_hwndToViewCoordinator.end() && isExpanded) {
        return isExpanded(it->second, hTaskbarWnd);
    }
    // Auto-hidden taskbars retain WS_VISIBLE and may be moved off-screen.
    // Unknown state must attempt Unhide, never skip it as "already visible".
    return false;
}

// ---------------------------------------------------------------------------
// Reveal / hold / release
// ---------------------------------------------------------------------------

DWORD GetForegroundProcessId() {
    DWORD pid = 0;
    if (HWND foreground = GetForegroundWindow()) {
        GetWindowThreadProcessId(foreground, &pid);
    }
    return pid;
}

bool RevealTaskbar(HWND hTaskbarWnd, const wchar_t* reason,
                   HWND attentionWindow = nullptr, DWORD attentionPid = 0) {
    RefreshLauncherVisibility();
    if (g_launcherVisible) return false;
    // Ignore a queued flash from an app the user has already switched to.
    if (attentionPid && attentionPid == GetForegroundProcessId()) return false;
    if (!HasTaskbarAutoHide()) return false;
    bool newlyArmed = !g_armed;
    if (std::find(g_armedTaskbars.begin(), g_armedTaskbars.end(), hTaskbarWnd) ==
        g_armedTaskbars.end()) g_armedTaskbars.push_back(hTaskbarWnd);
    // Set the hold before calling Explorer: its callbacks may run synchronously.
    if (newlyArmed) {
        g_armed = true;
        g_armedAt = GetTickCount();
        g_pointerOverSince = 0;
        g_nativeHideBlockCount = 0;
    }
    // IsExpanded can be true while the legacy auto-hide layer is still hidden.
    // Match the native show-temporarily sequence: Unhide, THEN pointer-enter.
    const bool reportedExpanded = IsTaskbarExpanded(hTaskbarWnd);
    bool nativeCalled = false;
    bool viewCalled = false;
    if (auto trayUnhide = TrayUI_Unhide()) {
        auto it = g_hwndToTrayPThis.find(hTaskbarWnd);
        if (it != g_hwndToTrayPThis.end() && IsWindow(hTaskbarWnd)) {
            void* host = QueryViaVtable(it->second, g_trayVftableITrayComponentHost);
            if (host) {
                trayUnhide(host, 0, 0);
                nativeCalled = true;
            }
        }
    }
    if (g_secondaryUnhideAddr) {
        auto it = g_hwndToSecondaryPThis.find(hTaskbarWnd);
        if (it != g_hwndToSecondaryPThis.end() && IsWindow(hTaskbarWnd)) {
            reinterpret_cast<TrayUI_Unhide_t>(g_secondaryUnhideAddr)(it->second, 0, 0);
            nativeCalled = true;
        }
    }
    if (auto pointerChanged = ViewCoordinator_HandlePointerOver()) {
        auto it = g_hwndToViewCoordinator.find(hTaskbarWnd);
        if (it != g_hwndToViewCoordinator.end()) {
            pointerChanged(it->second, hTaskbarWnd, true, 0);
            viewCalled = true;
        }
    }
    Wh_Log(L"[reveal] hwnd=%08X request=%s nativeUnhide=%d viewPointerEnter=%d "
           L"IsExpandedBefore=%d (not a visibility guarantee)",
           (DWORD)(ULONG_PTR)hTaskbarWnd, reason, nativeCalled, viewCalled,
           reportedExpanded);
    if (!nativeCalled && !viewCalled) {
        g_armedTaskbars.erase(std::remove(g_armedTaskbars.begin(),
            g_armedTaskbars.end(), hTaskbarWnd), g_armedTaskbars.end());
        if (g_armedTaskbars.empty()) g_armed = false;
        return false;
    }

    if (g_armed && attentionWindow && attentionPid) {
        g_pendingAttention[attentionWindow] = attentionPid;
    }
    if (newlyArmed) Wh_Log(L"[arm] holding the taskbar visible");
    return true;
}

bool IsPointerOnArmedTaskbar() {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    for (HWND hWnd : g_armedTaskbars) {
        RECT rc;
        if (GetWindowRect(hWnd, &rc) && PtInRect(&rc, pt)) {
            return true;
        }
    }
    return false;
}

// Stop interfering and let the stock auto-hide behavior collapse the taskbar.
void ReleaseTaskbar(const wchar_t* reason) {
    if (!g_armed) return;
    Wh_Log(L"[release] %s", reason);
    Wh_Log(L"[release] blocked %u native hide request(s) while held",
           g_nativeHideBlockCount);
    // Stop holding BEFORE callbacks can schedule a normal hide.
    g_armed = false;
    g_pendingAttention.clear();
    auto taskbars = std::move(g_armedTaskbars);
    g_armedTaskbars.clear();
    g_pointerOverSince = 0;
    for (HWND hWnd : taskbars) {
        if (!IsWindow(hWnd)) continue;
        POINT pt{};
        RECT rc{};
        bool pointerOver = GetCursorPos(&pt) && GetWindowRect(hWnd, &rc) &&
                           PtInRect(&rc, pt);
        auto it = g_hwndToViewCoordinator.find(hWnd);
        auto pointerChanged = ViewCoordinator_HandlePointerOver();
        if (it != g_hwndToViewCoordinator.end() && pointerChanged) {
            pointerChanged(it->second, hWnd, pointerOver, 0);
        }
        // Restore Explorer's last requested interval, including any timer it
        // killed while being held. Never hide while the pointer is still here.
        auto timer = g_hideIntervals.find(hWnd);
        if (!pointerOver && timer != g_hideIntervals.end()) {
            SetTimer(hWnd, kTrayUITimerHide, timer->second.interval,
                     timer->second.callback);
        }
    }
}

void AcknowledgeForegroundApp() {
    if (!g_armed || g_pendingAttention.empty()) return;
    const DWORD foregroundPid = GetForegroundProcessId();
    for (auto it = g_pendingAttention.begin(); it != g_pendingAttention.end();) {
        DWORD currentPid = 0;
        GetWindowThreadProcessId(it->first, &currentPid);
        // A closed/reused window must not keep a stale reminder alive.
        if (currentPid != it->second ||
            (foregroundPid && foregroundPid == it->second)) {
            if (g_settings.debugLog) {
                Wh_Log(L"[ack] hwnd=%08X pid=%u foreground=%d",
                       (DWORD)(ULONG_PTR)it->first, it->second,
                       foregroundPid && foregroundPid == it->second);
            }
            it = g_pendingAttention.erase(it);
        } else {
            ++it;
        }
    }
    if (g_pendingAttention.empty()) {
        ReleaseTaskbar(L"attention apps activated or closed; native control restored");
    }
}

// Opening Start acknowledges the existing notification hold. Cancel immediately,
// not only after a pointer visit, so Win -> Win/Escape behaves like stock Windows.
void HandleLauncherVisibility(bool visible) {
    const bool changed = visible != g_launcherVisible;
    g_launcherVisible = visible;
    if (changed) Wh_Log(L"[start] launcher %s", visible ? L"opened" : L"closed");
    if (visible && g_armed) ReleaseTaskbar(L"Start menu opened; native control restored");
}

void RefreshLauncherVisibility() {
    if (!g_appVisibility) return;
    BOOL visible = FALSE;
    // On failure retain the last known state, rather than inventing a close.
    if (SUCCEEDED(g_appVisibility->IsLauncherVisible(&visible))) {
        HandleLauncherVisibility(visible != FALSE);
    }
}

class LauncherVisibilitySink final : public IAppVisibilityEvents {
    std::atomic<ULONG> m_refs{1};
    HWND m_receiver;
public:
    explicit LauncherVisibilitySink(HWND receiver) : m_receiver(receiver) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** result) override {
        if (!result) return E_POINTER;
        *result = nullptr;
        if (iid == __uuidof(IUnknown) || iid == __uuidof(IAppVisibilityEvents)) {
            *result = static_cast<IAppVisibilityEvents*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++m_refs; }
    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG refs = --m_refs;
        if (!refs) delete this;
        return refs;
    }
    HRESULT STDMETHODCALLTYPE AppVisibilityOnMonitorChanged(
        HMONITOR, MONITOR_APP_VISIBILITY, MONITOR_APP_VISIBILITY) override {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE LauncherVisibilityChange(BOOL visible) override {
        if (!g_stopping.load()) {
            PostMessage(m_receiver, g_launcherVisibilityMsg, visible != FALSE, 0);
        }
        return S_OK;
    }
};

void StartLauncherWatch() {
    const HRESULT initHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_launcherComInitialized = SUCCEEDED(initHr);
    if (FAILED(initHr) && initHr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"[start] COM initialization failed: 0x%08X", (UINT)initHr);
        return;
    }
    const HRESULT createHr = CoCreateInstance(__uuidof(AppVisibility), nullptr,
        CLSCTX_INPROC_SERVER, __uuidof(IAppVisibility), (void**)&g_appVisibility);
    if (FAILED(createHr)) {
        Wh_Log(L"[start] visibility service unavailable: 0x%08X", (UINT)createHr);
        return;
    }
    g_launcherEvents = new (std::nothrow) LauncherVisibilitySink(g_receiverWnd);
    if (g_launcherEvents) {
        const HRESULT hr = g_appVisibility->Advise(g_launcherEvents, &g_launcherCookie);
        g_launcherAdvised = SUCCEEDED(hr);
        if (FAILED(hr)) Wh_Log(L"[start] event subscription failed: 0x%08X", (UINT)hr);
    }
    RefreshLauncherVisibility();
    Wh_Log(L"[start] visibility watch ready: events=%d polling=1", g_launcherAdvised);
}

void StopLauncherWatch() {
    if (g_appVisibility && g_launcherAdvised) {
        g_appVisibility->Unadvise(g_launcherCookie);
    }
    g_launcherAdvised = false;
    if (g_launcherEvents) g_launcherEvents->Release();
    g_launcherEvents = nullptr;
    if (g_appVisibility) g_appVisibility->Release();
    g_appVisibility = nullptr;
    if (g_launcherComInitialized) CoUninitialize();
    g_launcherComInitialized = false;
}

// ---------------------------------------------------------------------------
// Guarded timer hook - the "hold" mechanism
// ---------------------------------------------------------------------------

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original = nullptr;

UINT_PTR WINAPI SetTimer_Hook(HWND hWnd,
                              UINT_PTR nIDEvent,
                              UINT uElapse,
                              TIMERPROC lpTimerFunc) {
    // Observe the native timing without extending it. The taskbar WndProc
    // suppresses hide-timer messages only while this specific bar is armed.
    if (GetCurrentThreadId() == g_taskbarThreadId.load() &&
        g_armed && nIDEvent == kTrayUITimerHide &&
        GetWindowThreadProcessId(hWnd, nullptr) == GetCurrentThreadId() &&
        IsTaskbarWindow(hWnd)) {
        g_hideIntervals[hWnd] = {uElapse, lpTimerFunc};
    }

    if (SetTimer_Original) {
        return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
    }
    return SetTimer(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

// Cooldown cleanup on the taskbar thread.

void SweepCooldownMap() {
    DWORD now = GetTickCount();
    if (g_lastCooldownSweep && now - g_lastCooldownSweep < 60000) {
        return;
    }
    g_lastCooldownSweep = now;

    for (auto it = g_lastTriggerByWindow.begin();
         it != g_lastTriggerByWindow.end();) {
        if (!IsWindow(it->first) || now - it->second > 60000) {
            it = g_lastTriggerByWindow.erase(it);
        } else {
            ++it;
        }
    }
}

// ---------------------------------------------------------------------------
// Watchdog: hold / release decision
// ---------------------------------------------------------------------------

void WatchdogTick() {
    RefreshLauncherVisibility();
    AcknowledgeForegroundApp();
    if (g_settings.enabled) {
        SweepCooldownMap();
    }

    if (!g_armed) {
        return;
    }

    // Drop taskbars that are gone (e.g. Explorer restarted).
    for (auto it = g_armedTaskbars.begin(); it != g_armedTaskbars.end();) {
        it = IsWindow(*it) ? it + 1 : g_armedTaskbars.erase(it);
    }
    if (g_armedTaskbars.empty()) {
        g_armed = false;
        g_pendingAttention.clear();
        g_pointerOverSince = 0;
        return;
    }

    DWORD now = GetTickCount();

    if (!g_settings.hideOnPointerLeave) {
        // Fixed-duration mode: reveal, show for a while, then release.
        DWORD limit = (DWORD)(g_settings.fallbackRevealDurationMs > 0
                                  ? g_settings.fallbackRevealDurationMs
                                  : 3000);
        if (now - g_armedAt >= limit) {
            ReleaseTaskbar(L"fallback reveal duration elapsed");
        }
        return;
    }

    bool pointerOver = IsPointerOnArmedTaskbar();

    if (pointerOver) {
        if (!g_pointerOverSince) {
            g_pointerOverSince = now;
            Wh_Log(L"[hold] pointer is on the taskbar");
        }
    } else if (g_pointerOverSince) {
        // The pointer was on the taskbar and has now left: that is the signal
        // to hand control back to the normal auto-hide logic.
        ReleaseTaskbar(L"pointer left the taskbar");
        return;
    }

    // Safety valve for the case where the pointer never reaches the taskbar.
    if (g_settings.maxVisibleMs > 0 &&
        now - g_armedAt >= (DWORD)g_settings.maxVisibleMs) {
        ReleaseTaskbar(L"max visible time elapsed");
    }
}

// ---------------------------------------------------------------------------
// Attention handling
// ---------------------------------------------------------------------------

void OnAttention(HWND hWnd, const wchar_t* source) {
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }

    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hWnd, &dwProcessId);
    OnAttentionPid(hWnd, dwProcessId, source);
}

void OnAttentionPid(HWND hWnd, DWORD dwProcessId, const wchar_t* source) {
    if (!g_settings.enabled || !HasTaskbarAutoHide() || g_inAttentionHandling) {
        return;
    }

    if (!dwProcessId) return;

    g_inAttentionHandling = true;

    do {
        std::wstring processName = GetProcessFileName(dwProcessId);

        if (processName.empty()) {
            if (g_settings.debugLog) {
                Wh_Log(L"[skip] hwnd=%08X pid=%u: process name unavailable "
                       L"(source=%s)",
                       (DWORD)(ULONG_PTR)hWnd, dwProcessId, source);
            }
            break;
        }

        if (IsAlwaysIgnoredProcess(processName)) {
            break;
        }

        if (ProcessNameInList(processName, g_settings.excludeProcesses)) {
            DWORD now = GetTickCount();
            if (g_settings.debugLog &&
                now - g_lastExcludedLog > kWarnIntervalMs) {
                g_lastExcludedLog = now;
                Wh_Log(L"[skip] hwnd=%08X app=%s is excluded (source=%s)",
                       (DWORD)(ULONG_PTR)hWnd, processName.c_str(), source);
            }
            break;
        }

        if (g_settings.chatAppsWhitelist &&
            !ProcessNameInList(processName, g_settings.appWhitelist)) {
            if (g_settings.debugLog) {
                Wh_Log(L"[skip] hwnd=%08X app=%s is not in the white list "
                       L"(source=%s)",
                       (DWORD)(ULONG_PTR)hWnd, processName.c_str(), source);
            }
            break;
        }

        DWORD now = GetTickCount();
        auto cooldown = g_lastTriggerByWindow.find(hWnd);
        if (cooldown != g_lastTriggerByWindow.end() &&
            now - cooldown->second < (DWORD)g_settings.minIntervalMs) {
            if (g_settings.debugLog) {
                Wh_Log(L"[skip] hwnd=%08X app=%s: cooldown (source=%s)",
                       (DWORD)(ULONG_PTR)hWnd, processName.c_str(), source);
            }
            break;
        }
        bool revealed = false;

        // Reveal the taskbar of the monitor the attention window is on.
        HMONITOR attentionMonitor =
            MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

        std::vector<HWND> taskbars;
        FindAllTaskbarWindows(&taskbars);

        for (HWND hTaskbarWnd : taskbars) {
            HMONITOR taskbarMonitor =
                MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST);
            if (taskbarMonitor != attentionMonitor) continue;

            Wh_Log(L"[attention] hwnd=%08X app=%s source=%s -> taskbar=%08X",
                   (DWORD)(ULONG_PTR)hWnd, processName.c_str(), source,
                   (DWORD)(ULONG_PTR)hTaskbarWnd);
            revealed |= RevealTaskbar(hTaskbarWnd, source, hWnd, dwProcessId);
        }
        if (revealed) g_lastTriggerByWindow[hWnd] = now;
    } while (false);

    g_inAttentionHandling = false;
}

// Private shell receiver. Never register/deregister Explorer's own shell window.
LRESULT CALLBACK ReceiverWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (g_launcherVisibilityMsg && msg == g_launcherVisibilityMsg) {
        HandleLauncherVisibility(wp != 0);
        return 0;
    }
    if (g_shellHookMsg && msg == g_shellHookMsg) {
        if (wp == HSHELL_FLASH) OnAttention((HWND)lp, L"HSHELL_FLASH");
        if (wp == HSHELL_WINDOWACTIVATED || wp == HSHELL_RUDEAPPACTIVATED) {
            AcknowledgeForegroundApp();
        }
        return 0;
    }
    if (msg == WM_TIMER && wp == kWatchdogTimerId) {
        WatchdogTick();
        return 0;
    }
    if (g_shutdownMsg && msg == g_shutdownMsg) {
        StopLauncherWatch();
        ReleaseTaskbar(L"mod unloading");
        DeregisterShellHookWindow(hwnd);
        KillTimer(hwnd, kWatchdogTimerId);
        DestroyWindow(hwnd);
        g_receiverWnd = nullptr;
        g_receiverForUnload = nullptr;
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void InitializeReceiver(HWND hTaskbarWnd) {
    if (g_receiverWnd || g_stopping ||
        GetWindowThreadProcessId(hTaskbarWnd, nullptr) != GetCurrentThreadId()) {
        return;
    }
    // A hidden top-level tool window, on the same thread as the taskbar.
    HWND receiver = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kReceiverClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
        g_modInstance, nullptr);
    if (!receiver) {
        Wh_Log(L"[init] CreateWindowEx failed: %u", GetLastError());
        return;
    }
    if (!RegisterShellHookWindow(receiver)) {
        Wh_Log(L"[init] RegisterShellHookWindow failed: %u", GetLastError());
        DestroyWindow(receiver);
        return;
    }
    if (!SetTimer(receiver, kWatchdogTimerId, kAttentionPollMs, nullptr)) {
        Wh_Log(L"[init] SetTimer failed: %u", GetLastError());
        DeregisterShellHookWindow(receiver);
        DestroyWindow(receiver);
        return;
    }
    g_receiverWnd = receiver;
    g_receiverForUnload = receiver;
    g_taskbarThreadId = GetCurrentThreadId();
    StartLauncherWatch();
    Wh_Log(L"[init] shell flash receiver and watchdog ready, thread=%u",
           GetCurrentThreadId());
}

bool IsHeldTaskbar(HWND hwnd) {
    return GetCurrentThreadId() == g_taskbarThreadId.load() &&
        g_armed && std::find(g_armedTaskbars.begin(), g_armedTaskbars.end(),
                               hwnd) != g_armedTaskbars.end();
}

// The classic auto-hide state machine can call these functions directly.
// Filtering WM_TIMER alone does not prevent those direct native calls.
bool IsPrimaryTaskbarHeld() {
    if (GetCurrentThreadId() != g_taskbarThreadId.load() || !g_armed) return false;
    // There is one primary TrayUI per shell process. Its ITrayComponentHost
    // subobject can have a different address, so do not compare pThis blindly.
    for (HWND hwnd : g_armedTaskbars) {
        if (g_hwndToTrayPThis.contains(hwnd)) return true;
    }
    return false;
}

bool IsSecondaryTaskbarHeld(void* pThis) {
    if (GetCurrentThreadId() != g_taskbarThreadId.load() || !g_armed) return false;
    for (HWND hwnd : g_armedTaskbars) {
        auto it = g_hwndToSecondaryPThis.find(hwnd);
        if (it != g_hwndToSecondaryPThis.end() && it->second == pThis) return true;
    }
    return false;
}

void LogBlockedNativeHide(PCWSTR path) {
    if (g_nativeHideBlockCount++ == 0) {
        Wh_Log(L"[hold] blocked native hide: %s", path);
    }
}

using TrayUI_Hide_t = void(WINAPI*)(void* pThis);
TrayUI_Hide_t TrayUI_Hide_Original = nullptr;
void WINAPI TrayUI_Hide_Hook(void* pThis) {
    if (IsPrimaryTaskbarHeld()) {
        LogBlockedNativeHide(L"TrayUI::_Hide");
        return;
    }
    TrayUI_Hide_Original(pThis);
}

using CSecondaryTray_AutoHide_t = void(WINAPI*)(void* pThis, bool param);
CSecondaryTray_AutoHide_t CSecondaryTray_AutoHide_Original = nullptr;
void WINAPI CSecondaryTray_AutoHide_Hook(void* pThis, bool param) {
    if (IsSecondaryTaskbarHeld(pThis)) {
        LogBlockedNativeHide(L"CSecondaryTray::_AutoHide");
        return;
    }
    CSecondaryTray_AutoHide_Original(pThis, param);
}

using TrayUI_WndProc_t =
    LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam,
                     LPARAM lParam, bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original = nullptr;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis,
                                   HWND hWnd,
                                   UINT Msg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   bool* flag) {
    if (Msg == WM_NCCREATE || (g_captureThisMsg && Msg == g_captureThisMsg)) {
        g_hwndToTrayPThis[hWnd] = pThis;
        InitializeReceiver(hWnd);

    } else if (Msg == WM_NCDESTROY) {
        g_hwndToTrayPThis.erase(hWnd);
        g_hwndToViewCoordinator.erase(hWnd);
        g_hideIntervals.erase(hWnd);
    }

    if (Msg == WM_TIMER && wParam == kTrayUITimerHide && IsHeldTaskbar(hWnd)) {
        if (flag) *flag = true;
        return 0;
    }
    return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);
}

using CSecondaryTray_v_WndProc_t =
    LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam,
                     LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original = nullptr;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis,
                                             HWND hWnd,
                                             UINT Msg,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    if (Msg == WM_NCCREATE || (g_captureThisMsg && Msg == g_captureThisMsg)) {
        g_hwndToSecondaryPThis[hWnd] = pThis;
    } else if (Msg == WM_NCDESTROY) {
        g_hwndToSecondaryPThis.erase(hWnd);
        g_hwndToViewCoordinator.erase(hWnd);
        g_hideIntervals.erase(hWnd);
    }

    if (Msg == WM_TIMER && wParam == kTrayUITimerHide && IsHeldTaskbar(hWnd)) {
        return 0;
    }
    return CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
}

using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, bool expanded);
ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original = nullptr;
bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(void* pThis,
                                                         HWND hMMTaskbarWnd,
                                                         bool expanded) {
    g_hwndToViewCoordinator[hMMTaskbarWnd] = pThis;

    if (IsHeldTaskbar(hMMTaskbarWnd)) return true;
    return ViewCoordinator_ShouldTaskbarBeExpanded_Original(pThis,
                                                            hMMTaskbarWnd,
                                                            expanded);
}

using CTaskBand_v_WndProc_t =
    LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam,
                     LPARAM lParam);
CTaskBand_v_WndProc_t CTaskBand_v_WndProc_Original = nullptr;
LRESULT WINAPI CTaskBand_v_WndProc_Hook(void* pThis,
                                        HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam) {
    if (Msg == WM_CREATE || (g_captureThisMsg && Msg == g_captureThisMsg)) {
        // Also runs on Explorer's UI thread. TrayUI::WndProc normally gets us
        // there first; this is a second, optional opportunity.
        InitializeReceiver(hWnd);
    }

    return CTaskBand_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
}

bool HookTaskbarSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"[init] WARNING: couldn't load taskbar.dll");
        return false;
    }

    // Symbols we only read, plus the symbols we patch. Each entry has to use the
    // matching SYMBOL_HOOK constructor:
    //   - symbols without a hook: a void* target and a nullptr hook, so the
    //     template deduces Prototype = void;
    //   - patched symbols: the typed original pointer and the typed hook, whose
    //     signatures must match so the template deduces the real prototype.
    // All entries are optional, so a partial match still succeeds.
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: void __cdecl TrayUI::_Hide(void))"},
            &TrayUI_Hide_Original,
            TrayUI_Hide_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
            &CSecondaryTray_AutoHide_Original,
            CSecondaryTray_AutoHide_Hook,
            true,
        },
        {
            {LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})"},
            (void**)&g_trayVftableITrayComponentHost,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            (void**)&g_trayUnhideAddr,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &g_secondaryUnhideAddr,
            nullptr,
            true,
        },
        {
            {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CSecondaryTray_v_WndProc_Original,
            CSecondaryTray_v_WndProc_Hook,
            true,
        },
        {
            {LR"(protected: virtual __int64 __cdecl CTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CTaskBand_v_WndProc_Original,
            CTaskBand_v_WndProc_Hook,
            true,
        },
    };

    bool ok = HookSymbols(module, taskbarDllHooks,
                          ARRAYSIZE(taskbarDllHooks));
    if (!ok) {
        Wh_Log(L"[init] WARNING: HookSymbols(taskbar.dll) failed");
    }

    Wh_Log(L"[init] taskbar.dll symbols: Unhide=%s vftable=%s WndProc=%s "
           L"CTaskBand=%s SecondaryTray=%s",
           TrayUI_Unhide() ? L"ok" : L"missing",
           g_trayVftableITrayComponentHost ? L"ok" : L"missing",
           TrayUI_WndProc_Original ? L"ok" : L"missing",
           CTaskBand_v_WndProc_Original ? L"ok" : L"missing",
           CSecondaryTray_v_WndProc_Original ? L"ok" : L"missing");

    Wh_Log(L"[init] native hide hooks: primary=%s secondary=%s",
           TrayUI_Hide_Original ? L"ok" : L"missing",
           CSecondaryTray_AutoHide_Original ? L"ok" : L"missing");
    // Do not claim a working hold mode if the primary native hide hook is absent.
    return ok && TrayUI_WndProc_Original && TrayUI_Hide_Original;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

bool HookTaskbarViewSymbols() {
    // Late module resolution runs once, avoiding duplicate optional hooks.
    if (g_viewSymbolsAttempted.load()) return g_viewSymbolsHooked.load();

    if (g_viewHookBusy.test_and_set()) return false;
    struct ClearBusy { ~ClearBusy() { g_viewHookBusy.clear(); } } clearBusy;
    HMODULE module = GetTaskbarViewModuleHandle();
    if (!module) {
        return false;
    }

    g_viewSymbolsAttempted = true;
    WCHAR szModulePath[MAX_PATH] = L"?";
    GetModuleFileName(module, szModulePath, ARRAYSIZE(szModulePath));
    Wh_Log(L"[init] hooking the taskbar view module: %s", szModulePath);

    // Read-only symbols use a void* target with a nullptr hook; the patched one
    // uses the typed original pointer and hook.
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK viewHooks[] = {
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::IsExpanded(unsigned __int64))"},
            (void**)&g_viewCoordinatorIsExpandedAddr,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged(unsigned __int64,bool,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
            (void**)&g_viewCoordinatorPointerOverAddr,
            nullptr,
            true,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))"},
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
            true,
        },
    };

    if (!HookSymbols(module, viewHooks, ARRAYSIZE(viewHooks))) {
        Wh_Log(L"[init] WARNING: HookSymbols(Taskbar.View.dll) failed");
        return false;
    }

    Wh_Log(L"[init] Taskbar.View symbols: pointerOver=%s IsExpanded=%s "
           L"ShouldExpand=%s",
           g_viewCoordinatorPointerOverAddr ? L"ok" : L"missing",
           g_viewCoordinatorIsExpandedAddr ? L"ok" : L"missing",
           ViewCoordinator_ShouldTaskbarBeExpanded_Original ? L"ok"
                                                            : L"missing");

    // HookSymbols can succeed when optional symbols are absent.
    g_viewSymbolsHooked = g_viewCoordinatorPointerOverAddr &&
        g_viewCoordinatorIsExpandedAddr && ViewCoordinator_ShouldTaskbarBeExpanded_Original;
    return g_viewSymbolsHooked;
}

// This worker only resolves late symbols and posts initialization messages.
// It never calls Explorer objects, reads settings, or waits on the taskbar.
DWORD WINAPI InitWorkerProc(LPVOID) {
    while (WaitForSingleObject(g_stopEvent, 500) == WAIT_TIMEOUT) {
        // Other Explorer processes have no taskbar; do not resolve view symbols
        // or flood their log with a failed initialization every half second.
        HWND taskbar = FindCurrentProcessTaskbarWnd();
        if (!taskbar) continue;
        if (!g_viewSymbolsAttempted.load()) {
            HookTaskbarViewSymbols();
            if (g_viewSymbolsAttempted.load()) Wh_ApplyHookOperations();
        }
        if (!g_receiverForUnload.load()) {
            PostMessage(taskbar, g_captureThisMsg, 0, 0);
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Module lifecycle
// ---------------------------------------------------------------------------

void ApplySettings() {
    g_settings.enabled = Wh_GetIntSetting(L"enabled") != 0;

    PCWSTR triggerScope = Wh_GetStringSetting(L"triggerScope");
    g_settings.chatAppsWhitelist =
        triggerScope && _wcsicmp(triggerScope, L"chatAppsWhitelist") == 0;
    Wh_FreeStringSetting(triggerScope);

    PCWSTR appWhitelist = Wh_GetStringSetting(L"appWhitelist");
    g_settings.appWhitelist = appWhitelist ? appWhitelist : L"";
    Wh_FreeStringSetting(appWhitelist);

    PCWSTR excludeProcesses = Wh_GetStringSetting(L"excludeProcesses");
    g_settings.excludeProcesses = excludeProcesses ? excludeProcesses : L"";
    Wh_FreeStringSetting(excludeProcesses);

    g_settings.hideOnPointerLeave =
        Wh_GetIntSetting(L"hideOnPointerLeave") != 0;
    g_settings.maxVisibleMs = Wh_GetIntSetting(L"maxVisibleMs");
    g_settings.fallbackRevealDurationMs =
        Wh_GetIntSetting(L"fallbackRevealDurationMs");
    g_settings.minIntervalMs = Wh_GetIntSetting(L"minIntervalMs");

    if (g_settings.minIntervalMs < 0) {
        g_settings.minIntervalMs = 0;
    }
    if (g_settings.maxVisibleMs < 0) {
        g_settings.maxVisibleMs = 0;
    }

    g_settings.debugLog = Wh_GetIntSetting(L"debugLog") != 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"[init] taskbar-flash-reveal 1.0 loading");
    ApplySettings();
    g_captureThisMsg = RegisterWindowMessage(L"Windhawk_captureThis_" WH_MOD_ID);
    g_shellHookMsg = RegisterWindowMessage(L"SHELLHOOK");
    g_shutdownMsg = RegisterWindowMessage(L"Windhawk_shutdown_" WH_MOD_ID);
    g_launcherVisibilityMsg = RegisterWindowMessage(L"Windhawk_launcherVisibility_" WH_MOD_ID);
    if (!g_captureThisMsg || !g_shellHookMsg || !g_shutdownMsg ||
        !g_launcherVisibilityMsg) return FALSE;
    if (!HookTaskbarSymbols()) {
        Wh_Log(L"[init] required taskbar WndProc or native Hide symbol missing");
        return FALSE;
    }
    HookTaskbarViewSymbols();
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (PCWSTR)&ReceiverWndProc, &g_modInstance);
    WNDCLASS wc{};
    wc.hInstance = g_modInstance;
    wc.lpfnWndProc = ReceiverWndProc;
    wc.lpszClassName = kReceiverClass;
    if (!RegisterClass(&wc)) return FALSE;
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        UnregisterClass(kReceiverClass, g_modInstance);
        return FALSE;
    }
    WindhawkUtils::SetFunctionHook(SetTimer, SetTimer_Hook, &SetTimer_Original);
    if (!HasTaskbarAutoHide()) {
        Wh_Log(L"[init] auto-hide is disabled; attention events will be ignored");
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    HookTaskbarViewSymbols();
    Wh_ApplyHookOperations();
    std::vector<HWND> taskbars;
    FindAllTaskbarWindows(&taskbars);
    for (HWND hwnd : taskbars) SendMessage(hwnd, g_captureThisMsg, 0, 0);
    g_initWorker = CreateThread(nullptr, 0, InitWorkerProc, nullptr, 0, nullptr);
    if (!g_initWorker) Wh_Log(L"[init] retry worker failed: %u", GetLastError());
}

void Wh_ModBeforeUninit() {
    g_stopping = true;
    if (g_stopEvent) SetEvent(g_stopEvent);
    // All receiver teardown and Explorer calls must run on its owning thread.
    if (HWND hwnd = g_receiverForUnload.load()) SendMessage(hwnd, g_shutdownMsg, 0, 0);
}

void Wh_ModUninit() {
    // Never unload executable code while the initialization worker still uses it.
    if (g_initWorker) {
        WaitForSingleObject(g_initWorker, INFINITE);
        CloseHandle(g_initWorker);
        g_initWorker = nullptr;
    }
    if (HWND hwnd = g_receiverForUnload.load()) SendMessage(hwnd, g_shutdownMsg, 0, 0);
    if (g_stopEvent) CloseHandle(g_stopEvent);
    UnregisterClass(kReceiverClass, g_modInstance);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    // A reload avoids changing strings/state concurrently with the taskbar thread.
    *bReload = TRUE;
    return TRUE;

}
