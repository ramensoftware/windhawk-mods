// ==WindhawkMod==
// @id              windows-11-better-ultrawide-taskbar
// @name            Windows 11 Better Ultrawide Taskbar
// @description     Centers taskbar buttons and the system tray as one group while preserving native button animations
// @version         2.0
// @author          Molko
// @github          https://github.com/roeseth
// @include         explorer.exe
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldwmapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Windows 11 Better Ultrawide Taskbar

Centers the taskbar buttons, a 12-DIP gap, and the system tray as one group on
each monitor. Windows keeps control of button sizing and animations; the mod
follows Windows' existing compositor motion instead of polling or copying its
easing.

The mod supports native Windows labels and the Taskbar Labels mod in fixed and
adaptive width modes. Quick Settings, notifications, and the calendar follow
the moved tray. Windows taskbar alignment must be set to Center.

If the native controls do not fit, the visual tree is unsupported, taskbar
ownership changes, or more than 64 children are realized, that taskbar stays in
its native layout. A leading-app removal or reorder can also return the affected
taskbar to native layout until the mod is re-enabled.
*/
// ==/WindhawkModReadme==

// Geometry only: no width, label, button-transform, easing, background, divider,
// or appearance overrides.
#include <windhawk_utils.h>
#include <atomic>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <dwmapi.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>

using namespace winrt::Windows::UI::Xaml;
using winrt::Windows::Foundation::Size;
using winrt::Windows::Foundation::Numerics::float3;
using namespace winrt::Windows::UI::Composition;

constexpr double kGap = 12;
constexpr UINT kRestoreMessage = WM_APP + 43;
constexpr UINT kQueryTrayMessage = WM_APP + 44;
constexpr wchar_t kWindowClass[] = L"Windhawk.BetterUltrawideTaskbar.2";
std::atomic<bool> g_stopping{};
std::atomic<unsigned> g_active{};
std::atomic<DWORD> g_ownerThread{};
std::atomic<HWND> g_dispatch{};
ATOM g_windowClass{};
bool g_busy{};  // Owner thread only.
bool g_explorer{};
UINT g_discoverMessage{};

// Same taskbar.dll root lookup used by hide-start-button and
// taskbar-centered-start-split-icons. Resolve functions without hooking them.
void* g_taskBandVtable{};
void* g_secondaryTaskBandVtable{};
using GetTaskbarHost_t = void*(WINAPI*)(void*, void**);
GetTaskbarHost_t GetTaskbarHost_Original;
GetTaskbarHost_t GetSecondaryTaskbarHost_Original;
using Decref_t = void(WINAPI*)(void*);
Decref_t Decref_Original;
void* g_frameHeight{};
size_t g_hostElementOffset{};
bool ClassIs(HWND window, const wchar_t* expected);

template<typename T>
bool ReadLocal(const void* address, T& value) {
    SIZE_T bytes{};
    return address && ReadProcessMemory(GetCurrentProcess(), address, &value, sizeof(value), &bytes) &&
           bytes == sizeof(value);
}

bool HostElementOffset(const BYTE (&code)[8], size_t& offset) {
    // x64: sub rsp,imm8; add rcx,imm8. No guessed offset on a changed build.
    if (code[0] != 0x48 || code[1] != 0x83 || code[2] != 0xEC ||
        code[4] != 0x48 || code[5] != 0x83 || code[6] != 0xC1 ||
        code[7] < sizeof(void*) || code[7] > 0x7F || code[7] % sizeof(void*)) return false;
    offset = code[7];
    return true;
}

void* TaskBandInterface(void* object, void* expectedVtable) {
    if (!object || !expectedVtable) return nullptr;
    // Bounded interface search from the existing mods, using checked reads so
    // missing/recreated taskband data cannot cause an unchecked dereference.
    for (size_t i = 0; i < 20; ++i) {
        auto address = reinterpret_cast<uintptr_t>(object) + i * sizeof(void*);
        void* vtable{};
        if (!ReadLocal(reinterpret_cast<void*>(address), vtable)) return nullptr;
        if (vtable == expectedVtable) return reinterpret_cast<void*>(address);
    }
    return nullptr;
}

XamlRoot RootFromHost(void* (&host)[2]) {
    struct Release {
        void** host;
        ~Release() { if (host[1]) Decref_Original(host[1]); }
    } release{host};
    if (!host[0] || !host[1] || !g_hostElementOffset) return nullptr;
    IUnknown* element{};
    if (!ReadLocal(reinterpret_cast<BYTE*>(host[0]) + g_hostElementOffset, element) || !element)
        return nullptr;
    FrameworkElement frame{nullptr};
    if (FAILED(element->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(frame))))
        return nullptr;
    return frame ? frame.XamlRoot() : nullptr;
}

XamlRoot ExistingRoot(HWND taskbar) {
    DWORD pid{};
    DWORD thread = GetWindowThreadProcessId(taskbar, &pid);
    bool secondary = ClassIs(taskbar, L"Shell_SecondaryTrayWnd");
    if (pid != GetCurrentProcessId() || thread != GetCurrentThreadId() ||
        (!secondary && !ClassIs(taskbar, L"Shell_TrayWnd"))) return nullptr;
    HWND bandWindow = secondary ? FindWindowEx(taskbar, nullptr, L"WorkerW", nullptr)
                                : static_cast<HWND>(GetProp(taskbar, L"TaskbandHWND"));
    DWORD bandPid{};
    if (!bandWindow || GetWindowThreadProcessId(bandWindow, &bandPid) != thread || bandPid != pid)
        return nullptr;
    void* site = TaskBandInterface(reinterpret_cast<void*>(GetWindowLongPtr(bandWindow, 0)),
                                  secondary ? g_secondaryTaskBandVtable : g_taskBandVtable);
    if (!site) return nullptr;
    void* host[2]{};
    (secondary ? GetSecondaryTaskbarHost_Original : GetTaskbarHost_Original)(site, host);
    return RootFromHost(host);
}

// Numeric HWND properties only: no pointers shared between Shell processes.
constexpr wchar_t kTrayShift[] = L"Windhawk.UWPOS05.TrayShift";
constexpr wchar_t kTrayDispatcher[] = L"Windhawk.UWPOS08.Dispatcher";
constexpr wchar_t kPanelPosition[] = L"Windhawk.UWPOS05.PanelPosition";
constexpr wchar_t kPanelTaskbar[] = L"Windhawk.UWPOS05.PanelTaskbar";
#ifndef UWPANEL_PENDING_PROPERTY
#define UWPANEL_PENDING_PROPERTY L"Windhawk.UWPOS09.PanelPending"
#endif
constexpr wchar_t kPanelPending[] = UWPANEL_PENDING_PROPERTY;
decltype(&SetWindowPos) SetWindowPos_Original;
decltype(&DwmSetWindowAttribute) DwmSetWindowAttribute_Original;
decltype(&DestroyWindow) DestroyWindow_Original;
thread_local bool g_panelBusy{};
HWINEVENTHOOK g_panelLocationHook{};

HANDLE EncodeShift(int shift) {
    return reinterpret_cast<HANDLE>(uint64_t{static_cast<uint32_t>(shift)} + 1);
}
int DecodeShift(HANDLE value) {
    return static_cast<int32_t>(reinterpret_cast<uintptr_t>(value) - 1);
}
HANDLE EncodePosition(int nativeX, int appliedX) {
    return reinterpret_cast<HANDLE>((uint64_t{static_cast<uint32_t>(nativeX)} << 32) |
                                    static_cast<uint32_t>(appliedX));
}
void DecodePosition(HANDLE value, int& nativeX, int& appliedX) {
    auto bits = reinterpret_cast<uintptr_t>(value);
    nativeX = static_cast<int32_t>(bits >> 32);
    appliedX = static_cast<int32_t>(bits);
}

bool ClassIs(HWND window, const wchar_t* expected) {
    wchar_t name[64];
    return GetClassName(window, name, ARRAYSIZE(name)) && !wcscmp(name, expected);
}

bool IsPanel(HWND window) {
    bool controls = ClassIs(window, L"ControlCenterWindow");
    if (!controls && !ClassIs(window, L"Windows.UI.Core.CoreWindow")) return false;
    DWORD pid{};
    DWORD tid = GetWindowThreadProcessId(window, &pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    wchar_t path[MAX_PATH];
    DWORD length = ARRAYSIZE(path);
    bool read = QueryFullProcessImageName(process, 0, path, &length);
    CloseHandle(process);
    auto name = read ? wcsrchr(path, L'\\') : nullptr;
    if (!name || _wcsicmp(name + 1, controls ? L"ShellHost.exe" : L"ShellExperienceHost.exe")) return false;
    if (controls) return true;
    HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid);
    if (!thread) return false;
    PWSTR description{};
    bool actionCenter = SUCCEEDED(GetThreadDescription(thread, &description)) &&
                        description && !wcscmp(description, L"ActionCenter");
    LocalFree(description);
    CloseHandle(thread);
    return actionCenter;
}

// Require the observed full-height right-edge host, not every ActionCenter window.
bool PlanPanel(RECT work, RECT requested, int shift, HANDLE previous, int& nativeX, int& targetX) {
    int64_t width = int64_t{requested.right} - requested.left;
    int64_t workWidth = int64_t{work.right} - work.left;
    if (width <= 0 || width >= workWidth || requested.top != work.top ||
        requested.bottom != work.bottom || std::abs(int64_t{shift}) > workWidth) return false;
    nativeX = requested.left;
    int oldNative{}, oldApplied{};
    if (previous) {
        DecodePosition(previous, oldNative, oldApplied);
        if (requested.right != work.right && nativeX == oldApplied &&
            oldNative >= work.left && oldNative < work.right)
            nativeX = static_cast<int>(work.right - width);
    }
    if (int64_t{nativeX} + width != work.right) return false;
    targetX = static_cast<int>(std::clamp(int64_t{nativeX} + shift,
                                        int64_t{work.left}, int64_t{work.right} - width));
    return true;
}

HWND PublishedTaskbar(HMONITOR monitor) {
    HWND found{};
    // FindWindowEx includes the Shell windows omitted by EnumWindows here.
    HWND window{};
    for (int i = 0; i < 4096 && (window = FindWindowEx(nullptr, window, nullptr, nullptr)); ++i) {
        if (MonitorFromWindow(window, MONITOR_DEFAULTTONULL) != monitor ||
            !GetProp(window, kTrayShift) ||
            (!ClassIs(window, L"Shell_TrayWnd") && !ClassIs(window, L"Shell_SecondaryTrayWnd"))) continue;
        if (found) return nullptr;
        found = window;
    }
    return found;
}

HANDLE QueryTrayShift(HWND taskbar) {
    if (!taskbar) return nullptr;
    HWND dispatcher = static_cast<HWND>(GetProp(taskbar, kTrayDispatcher));
    if (!dispatcher) return GetProp(taskbar, kTrayShift); // Older/static publisher.
    DWORD barPid{}, dispatcherPid{};
    GetWindowThreadProcessId(taskbar, &barPid);
    GetWindowThreadProcessId(dispatcher, &dispatcherPid);
    if (!barPid || dispatcherPid != barPid || !ClassIs(dispatcher, kWindowClass)) return nullptr;
    DWORD_PTR result{};
    // No pointers cross process boundaries. Fail native if the UI thread is busy.
    if (!SendMessageTimeout(dispatcher, kQueryTrayMessage, reinterpret_cast<WPARAM>(taskbar), 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &result)) return nullptr;
    return reinterpret_cast<HANDLE>(result);
}

void RestorePanel(HWND window) {
    auto dpiContext = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    struct RestoreDpi {
        DPI_AWARENESS_CONTEXT value;
        ~RestoreDpi() { if (value) SetThreadDpiAwarenessContext(value); }
    } restoreDpi{dpiContext};
    HANDLE record = GetProp(window, kPanelPosition);
    if (!record) return;
    int nativeX{}, appliedX{};
    DecodePosition(record, nativeX, appliedX);
    RECT rect{};
    bool owned = IsPanel(window) && GetWindowRect(window, &rect) && rect.left == appliedX;
    // Remove first so a synchronous reentrant call cannot apply the offset again.
    RemoveProp(window, kPanelPosition);
    RemoveProp(window, kPanelTaskbar);
    if (owned) {
        bool busy = g_panelBusy;
        g_panelBusy = true;
        BOOL moved = SetWindowPos_Original(window, nullptr, nativeX, rect.top, 0, 0,
                                          SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        g_panelBusy = busy;
        RECT after{};
        Wh_Log(L"[UWPANEL] restore hwnd=%p nativeX=%d ok=%d verified=%d", window, nativeX,
               moved, moved && GetWindowRect(window, &after) && after.left == nativeX);
    }
}

void RestorePanels(HWND taskbar = nullptr, bool ownProcessOnly = false) {
    HWND window{};
    for (int i = 0; i < 4096 && (window = FindWindowEx(nullptr, window, nullptr, nullptr)); ++i) {
        if (GetProp(window, kPanelPending)) RemoveProp(window, kPanelPending);
        if (!GetProp(window, kPanelPosition) ||
            (taskbar && GetProp(window, kPanelTaskbar) != taskbar)) continue;
        DWORD pid{};
        GetWindowThreadProcessId(window, &pid);
        if (!ownProcessOnly || pid == GetCurrentProcessId()) RestorePanel(window);
    }
}

BOOL WINAPI PanelSetWindowPos(HWND window, HWND after, int x, int y, int cx, int cy, UINT flags) {
    ++g_active;
    struct Finish { ~Finish() { --g_active; } } finish;
    auto original = [&] { return SetWindowPos_Original(window, after, x, y, cx, cy, flags); };
    if (g_stopping || g_panelBusy || (flags & (SWP_HIDEWINDOW | SWP_ASYNCWINDOWPOS)) ||
        !IsPanel(window))
        return original();
    // Keep native coordinate units. Only consume physical pixels in a PM-aware call.
    if (GetAwarenessFromDpiAwarenessContext(GetThreadDpiAwarenessContext()) != DPI_AWARENESS_PER_MONITOR_AWARE)
        return original();
    RECT current{};
    if (!GetWindowRect(window, &current)) return original();
    int left = flags & SWP_NOMOVE ? current.left : x;
    int top = flags & SWP_NOMOVE ? current.top : y;
    int width = flags & SWP_NOSIZE ? current.right - current.left : cx;
    int height = flags & SWP_NOSIZE ? current.bottom - current.top : cy;
    if (width <= 0 || height <= 0 || int64_t{left} + width > INT_MAX ||
        int64_t{top} + height > INT_MAX) return original();
    RECT requested{left, top, left + width, top + height};
    HMONITOR monitor = MonitorFromRect(&requested, MONITOR_DEFAULTTONULL);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    HWND taskbar = PublishedTaskbar(monitor);
    HANDLE publication = QueryTrayShift(taskbar);
    if (!publication || !GetMonitorInfo(monitor, &info)) return original();
    HANDLE previous = GetProp(window, kPanelPosition);
    HANDLE previousTaskbar = GetProp(window, kPanelTaskbar);
    int nativeX{}, targetX{};
    if (!PlanPanel(info.rcWork, requested, DecodeShift(publication), previous, nativeX, targetX)) {
        Wh_Log(L"[UWPANEL] native fallback hwnd=%p rect=%ld,%ld,%ld,%ld flags=%x", window,
               requested.left, requested.top, requested.right, requested.bottom, flags);
        return original();
    }
    HANDLE record = EncodePosition(nativeX, targetX);
    if (nativeX == targetX) {
        if (previous) {
            x = targetX;
            y = top;
            flags &= ~SWP_NOMOVE;
        }
        BOOL result = original();
        RECT actual{};
        if (result && GetWindowRect(window, &actual) && actual.left == nativeX) {
            RemoveProp(window, kPanelPosition);
            RemoveProp(window, kPanelTaskbar);
        }
        return result;
    }
    if (!SetProp(window, kPanelTaskbar, taskbar)) return original();
    if (!SetProp(window, kPanelPosition, record)) {
        if (previousTaskbar) SetProp(window, kPanelTaskbar, previousTaskbar);
        else RemoveProp(window, kPanelTaskbar);
        return original();
    }
    x = targetX;
    y = top;
    flags &= ~SWP_NOMOVE;
    g_panelBusy = true;
    BOOL result = original();
    g_panelBusy = false;
    RECT readback{};
    bool verified = result && GetWindowRect(window, &readback) && readback.left == targetX;
    Wh_Log(L"[UWPANEL] position hwnd=%p taskbar=%p nativeX=%d targetX=%d shift=%d flags=%x ok=%d verified=%d",
           window, taskbar, nativeX, targetX, DecodeShift(publication), flags, result, verified);
    if (!verified) {
        // A failed call retains the preceding owned position when it still matches.
        int oldNative{}, oldApplied{};
        DecodePosition(previous, oldNative, oldApplied);
        if (previous && GetWindowRect(window, &readback) && readback.left == oldApplied) {
            SetProp(window, kPanelPosition, previous);
            if (previousTaskbar) SetProp(window, kPanelTaskbar, previousTaskbar);
        } else {
            RemoveProp(window, kPanelPosition);
            RemoveProp(window, kPanelTaskbar);
        }
    } else {
        RemoveProp(window, kPanelPending);
    }
    if (verified && (!GetProp(taskbar, kTrayShift) || g_stopping)) {
        // Publication may be withdrawn while the native cross-process call runs.
        if (!GetProp(window, kPanelPosition)) {
            SetProp(window, kPanelTaskbar, taskbar);
            SetProp(window, kPanelPosition, record);
        }
        RestorePanel(window);
    }
    return result;
}

HRESULT WINAPI PanelDwmAttribute(HWND window, DWORD attribute, LPCVOID value, DWORD size) {
    ++g_active;
    struct Finish { ~Finish() { --g_active; } } finish;
    if (!g_stopping && attribute == DWMWA_CLOAK && value && size == sizeof(BOOL) &&
        !*static_cast<const BOOL*>(value) && IsPanel(window)) {
        // Existing taskbar-vertical precedent: position BEFORE native uncloaking.
        Wh_Log(L"[UWPANEL] before-uncloak hwnd=%p", window);
        RECT rect{};
        if (GetWindowRect(window, &rect) &&
            (rect.right - rect.left <= 1 || rect.bottom - rect.top <= 1)) {
            SetProp(window, kPanelPending, reinterpret_cast<HANDLE>(1));
            Wh_Log(L"[UWPANEL] transient-uncloak hwnd=%p rect=%ld,%ld,%ld,%ld",
                   window, rect.left, rect.top, rect.right, rect.bottom);
        }
        PanelSetWindowPos(window, nullptr, 0, 0, 0, 0,
                          SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    return DwmSetWindowAttribute_Original(window, attribute, value, size);
}

void CALLBACK PanelLocationEvent(HWINEVENTHOOK, DWORD event, HWND window,
                                 LONG object, LONG child, DWORD, DWORD) {
    if (g_stopping || event != EVENT_OBJECT_LOCATIONCHANGE || object != OBJID_WINDOW ||
        child != CHILDID_SELF || !window || !GetProp(window, kPanelPending)) return;
    RECT rect{};
    if (!IsPanel(window) || !GetWindowRect(window, &rect)) {
        RemoveProp(window, kPanelPending);
        return;
    }
    if (rect.right - rect.left <= 1 || rect.bottom - rect.top <= 1) return;
    Wh_Log(L"[UWPANEL] final-location hwnd=%p rect=%ld,%ld,%ld,%ld", window,
           rect.left, rect.top, rect.right, rect.bottom);
    PanelSetWindowPos(window, nullptr, 0, 0, 0, 0,
                      SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL WINAPI PanelDestroyWindow(HWND window) {
    ++g_active;
    struct Finish { ~Finish() { --g_active; } } finish;
    // Property records belong to this HWND lifetime; never carry them to reuse.
    RemoveProp(window, kPanelPosition);
    RemoveProp(window, kPanelTaskbar);
    RemoveProp(window, kPanelPending);
    RemoveProp(window, kTrayDispatcher);
    if (RemoveProp(window, kTrayShift)) RestorePanels(window);
    return DestroyWindow_Original(window);
}

struct Placement { double buttonsX; double trayX; };

bool Plan(double host, double buttons, double tray, double nativeCenter,
          double nativeTrayLeft, double nativeStartDelta, Placement& result) {
    if (!std::isfinite(host) || !std::isfinite(buttons) ||
        !std::isfinite(tray) || !std::isfinite(nativeCenter) ||
        !std::isfinite(nativeTrayLeft) || !std::isfinite(nativeStartDelta) ||
        host <= 0 || buttons <= 0 || tray <= 0 || buttons + kGap + tray > host ||
        std::abs(nativeCenter - host / 2) > 1 || nativeStartDelta > 1 ||
        nativeCenter + nativeStartDelta - buttons / 2 < -1 ||
        nativeCenter + nativeStartDelta + buttons / 2 > host + 1) {
        return false;
    }
    double left = (host - buttons - kGap - tray) / 2;
    // Windows can shift the native button strip left near the tray even while
    // its repeater remains screen-centered. Use its measured slot.
    result = {left + buttons / 2 - nativeCenter - nativeStartDelta,
              left + buttons + kGap - nativeTrayLeft};
    return true;
}

struct OwnedTranslation {
    FrameworkElement element{nullptr};
    float3 original{};
    float3 applied{};
    bool installed{};
};
struct Root {
    FrameworkElement frame{nullptr};
    winrt::event_token layoutToken{};
    OwnedTranslation buttonsTranslation;
    OwnedTranslation trayTranslation;
    bool blocked{};
    bool fallbackReported{};
    HWND taskbar{};
    HANDLE publishedShift{};
    bool mappingReported{};
    struct Source {
        uintptr_t identity{};
        winrt::weak_ref<FrameworkElement> element;
        Visual visual{nullptr};
        enum class Life { Active, Hiding, Retired } life{};
    };
    std::vector<Source> sources;
    std::vector<uintptr_t> expressionSources;
    ExpressionAnimation follower{nullptr};
    ExpressionAnimation buttonsFollower{nullptr};
    uintptr_t anchorIdentity{};
    float anchorPrefix{};
    CompositionPropertySet metrics{nullptr};
    CompositionPropertySet edges{nullptr};
    unsigned edgeGroups{};
    bool followerInstalled{};
    double nativeRightOverhang{NAN};
    double hostWidth{};
};
std::vector<Root> g_roots;  // Confined to the one observed taskbar UI thread.
void MotionEvent(void* abi, Root::Source::Life life, bool cancelled = false) noexcept;
void Refresh(Root& root);

// ABI boundaries: raw IElementAnimatorOverrides arguments versus one-pointer
// C++/WinRT projected objects passed by const reference.
using HideAnimation_t = int(WINAPI*)(void*, void*, unsigned);
using ProjectedAnimation_t = void(WINAPI*)(void*, const void*);
HideAnimation_t HideAnimation_Original;
ProjectedAnimation_t StartAnimation_Original;
ProjectedAnimation_t HideCompleted_Original;

uintptr_t CanonicalIdentity(FrameworkElement const& element) {
    winrt::com_ptr<::IUnknown> identity;
    winrt::check_hresult(static_cast<::IUnknown*>(winrt::get_abi(element))->QueryInterface(
        __uuidof(::IUnknown), identity.put_void()));
    return reinterpret_cast<uintptr_t>(identity.get());
}

int WINAPI HideAnimation_Hook(void* self, void* element, unsigned context) {
    ++g_active;
    struct Finish { ~Finish() { --g_active; } } finish;
    MotionEvent(element, Root::Source::Life::Hiding);
    int result = HideAnimation_Original(self, element, context);
    if (FAILED(result)) MotionEvent(element, Root::Source::Life::Active, true);
    return result;
}

void WINAPI StartAnimation_Hook(void* projectedElement, const void* animation) {
    ++g_active;
    struct Finish { ~Finish() { --g_active; } } finish;
    void* abi{};
    if (!g_stopping && GetCurrentThreadId() == g_ownerThread && ReadLocal(projectedElement, abi))
    {
        MotionEvent(abi, Root::Source::Life::Active);
    }
    StartAnimation_Original(projectedElement, animation);
}

void WINAPI HideCompleted_Hook(void* projectedAnimator, const void* projectedElement) {
    ++g_active;
    struct Finish { ~Finish() { --g_active; } } finish;
    void* abi{};
    if (!g_stopping && GetCurrentThreadId() == g_ownerThread && ReadLocal(projectedElement, abi))
    {
        MotionEvent(abi, Root::Source::Life::Retired);
    }
    // Observe before the native completion call can recycle/park the element.
    HideCompleted_Original(projectedAnimator, projectedElement);
}

FrameworkElement Child(FrameworkElement parent, const wchar_t* name, bool byClass = false);

void WithdrawTray(Root& root) {
    if (root.taskbar && GetProp(root.taskbar, kTrayDispatcher) == g_dispatch)
        RemoveProp(root.taskbar, kTrayDispatcher);
    if (root.taskbar && root.publishedShift &&
        GetProp(root.taskbar, kTrayShift) == root.publishedShift) {
        RemoveProp(root.taskbar, kTrayShift);
        RestorePanels(root.taskbar);
    }
    root.publishedShift = nullptr;
}

void PublishTray(Root& root, double host, double shift) {
    // ponytail: UWP lacks WinUI's ContentIslandEnvironment HWND accessor. Match
    // one native taskbar by owner thread, full width and raster scale; equal-size
    // candidates or multiple XAML roots for that HWND must remain unpublished.
    double scale = root.frame.XamlRoot().RasterizationScale();
    HWND match{}, window{};
    for (int i = 0; i < 4096 && (window = FindWindowEx(nullptr, window, nullptr, nullptr)); ++i) {
        if ((!ClassIs(window, L"Shell_TrayWnd") && !ClassIs(window, L"Shell_SecondaryTrayWnd")) ||
            GetWindowThreadProcessId(window, nullptr) != g_ownerThread) continue;
        RECT rect{};
        if (!GetWindowRect(window, &rect) || std::abs(GetDpiForWindow(window) / 96.0 - scale) > 0.01 ||
            std::abs(rect.right - rect.left - host * scale) > 2) continue;
        if (match) { match = nullptr; break; }
        match = window;
    }
    for (auto& other : g_roots) {
        if (&other != &root && other.taskbar && other.taskbar == match) {
            WithdrawTray(other);
            match = nullptr;
            break;
        }
    }
    if (match != root.taskbar) WithdrawTray(root);
    if (!match || !std::isfinite(scale * shift) || std::abs(scale * shift) > INT_MAX) {
        if (!root.mappingReported) Wh_Log(L"[UWPANEL] no unambiguous taskbar mapping frame=%p host=%.3f scale=%.3f",
                                          winrt::get_abi(root.frame), host, scale);
        root.mappingReported = true;
        return;
    }
    root.mappingReported = false;
    root.taskbar = match;
    HANDLE value = EncodeShift(static_cast<int>(std::lround(scale * shift)));
    if (value == root.publishedShift && GetProp(match, kTrayShift) == value &&
        (!root.followerInstalled || GetProp(match, kTrayDispatcher) == g_dispatch)) return;
    if (SetProp(match, kTrayShift, value)) {
        if (root.followerInstalled && !SetProp(match, kTrayDispatcher, g_dispatch)) {
            root.publishedShift = value;
            WithdrawTray(root);
            return;
        }
        root.publishedShift = value;
    } else {
        WithdrawTray(root);
        Wh_Log(L"[UWPANEL] publication failed taskbar=%p error=%lu", match, GetLastError());
    }
}

FrameworkElement Child(FrameworkElement parent, const wchar_t* name,
                       bool byClass) {
    if (parent) {
        int count = Media::VisualTreeHelper::GetChildrenCount(parent);
        for (int i = 0; i < count; i++) {
            auto child = Media::VisualTreeHelper::GetChild(parent, i)
                             .try_as<FrameworkElement>();
            if (child && (byClass ? winrt::get_class_name(child) == name
                                  : child.Name() == name)) {
                return child;
            }
        }
    }
    return nullptr;
}

bool StillOwned(OwnedTranslation const& owned) {
    // ponytail: value comparison detects changed writes, not equal-value writes
    // or hidden animation ownership. Broader compatibility needs an ownership trace.
    return owned.element.Translation() == owned.applied;
}

void Restore(OwnedTranslation& owned) {
    if (!owned.installed) return;
    if (StillOwned(owned)) {
        owned.element.Translation(owned.original);
        auto restored = owned.element.Translation();
        Wh_Log(L"[UWPOS] restored translation element=%p value=%.3f,%.3f,%.3f verified=%d",
               winrt::get_abi(owned.element), restored.x, restored.y, restored.z,
               restored == owned.original);
    } else {
        Wh_Log(L"[UWPOS] ownership lost element=%p; external value retained",
               winrt::get_abi(owned.element));
    }
    owned.installed = false;
}

bool Prepare(FrameworkElement element, OwnedTranslation& owned) {
    if (owned.installed) {
        return owned.element == element && StillOwned(owned);
    }
    auto original = element.Translation();
    // Only try an idle translation channel. Never replace RenderTransform.
    if (original != float3{}) {
        Wh_Log(L"[UWPOS] existing translation element=%p value=%.3f,%.3f,%.3f",
               winrt::get_abi(element), original.x, original.y, original.z);
        return false;
    }
    owned.element = element;
    owned.original = original;
    owned.applied = original;
    return true;
}

void DescribeTransform(const wchar_t* role, FrameworkElement element) {
    try {
        auto property = UIElement::RenderTransformProperty();
        auto local = element.ReadLocalValue(property);
        auto effective = element.RenderTransform();
        auto base = element.GetAnimationBaseValue(property);
        auto describe = [](winrt::Windows::Foundation::IInspectable const& value) -> winrt::hstring {
            if (value == DependencyProperty::UnsetValue()) return L"unset";
            return value ? winrt::get_class_name(value) : winrt::hstring(L"null");
        };
        auto origin = element.RenderTransformOrigin();
        Wh_Log(L"[UWPOS] transform role=%s element=%p local=%s effective=%s base=%s origin=%.3f,%.3f",
               role, winrt::get_abi(element), describe(local).c_str(),
               describe(effective).c_str(), describe(base).c_str(), origin.X, origin.Y);
        if (effective) {
            auto zero = effective.TransformPoint({0, 0});
            auto x = effective.TransformPoint({1, 0});
            auto y = effective.TransformPoint({0, 1});
            Wh_Log(L"[UWPOS] transform-points role=%s zero=%.6f,%.6f x=%.6f,%.6f y=%.6f,%.6f",
                   role, zero.X, zero.Y, x.X, x.Y, y.X, y.Y);
        }
    } catch (...) {
        Wh_Log(L"[UWPOS] transform-inspection-error role=%s", role);
    }
}

bool EligibleSlot(double x, double width, double host) {
    return std::isfinite(x) && std::isfinite(width) && std::isfinite(host) &&
           width > 0 && host > 0 && x >= -1 && x + width <= host + 1;
}

Root::Source::Life NextLife(Root::Source::Life current, Root::Source::Life event, bool cancelled) {
    // Exit animations also call StartAnimation. They must not revive a hiding source.
    if (event == Root::Source::Life::Active && current == Root::Source::Life::Hiding && !cancelled)
        return current;
    return event;
}

std::wstring MaxExpression(size_t count, bool groups) {
    std::vector<std::wstring> terms{groups ? L"anchor.Offset.X - geometry.prefix + geometry.floor" : L"0"};
    for (size_t i = 0; i < count; ++i) {
        auto name = L"b" + std::to_wstring(i);
        terms.push_back(groups ? L"edges.e" + std::to_wstring(i)
                               : name + L".Offset.X + " + name + L".Size.X");
    }
    // Balanced Max avoids a long nested expression on a wide populated taskbar.
    while (terms.size() > 1) {
        std::vector<std::wstring> next;
        for (size_t i = 0; i < terms.size(); i += 2)
            next.push_back(i + 1 == terms.size() ? terms[i]
                : L"Max(" + terms[i] + L", " + terms[i + 1] + L")");
        terms = std::move(next);
    }
    return terms[0];
}

bool IncludeSource(bool eligible, Root::Source::Life life) {
    return life != Root::Source::Life::Retired && (eligible || life == Root::Source::Life::Hiding);
}

bool WholeGenerationReplaced(std::vector<uintptr_t> const& before,
                             std::vector<uintptr_t> const& after,
                             float oldPrefix, float newPrefix) {
    return !before.empty() && before.size() == after.size() &&
           std::abs(oldPrefix - newPrefix) <= 1 &&
           std::none_of(after.begin(), after.end(), [&](uintptr_t identity) {
               return std::find(before.begin(), before.end(), identity) != before.end();
           });
}

constexpr unsigned kEdgesPerGroup = 16; // Windows rejects a single 64-source expression as too long.
unsigned EdgeGroups(size_t count) { return static_cast<unsigned>((count + kEdgesPerGroup - 1) / kEdgesPerGroup); }

double BoundaryThreshold(double& overhang, double start, double buttons,
                         double width, double localTrayLeft) {
    // Native left = min((width-buttons)/2, rightLimit-buttons). The two
    // branches meet at left = width-rightLimit. Learn rightLimit only where
    // native layout is constrained; retain its tray-relative offset on exit.
    if (start - (width - buttons) / 2 < -1)
        overhang = start + buttons - localTrayLeft;
    return std::isfinite(overhang) ? width - localTrayLeft - overhang : -width;
}

std::wstring EdgeExpression(size_t count, bool buttons = false) {
    // No retiring right edge feeds the parent: it would add a second reflow.
    std::wstring correction = L"Max(0, (geometry.threshold - (anchor.Offset.X - geometry.prefix)) / 2)";
    if (buttons) return L"Vector3(geometry.shift + " + correction + L", 0, 0)";
    return L"Vector3(Clamp(geometry.trayBase + " + MaxExpression(EdgeGroups(count), true) +
           L" + " + correction + L", geometry.minX, geometry.maxX), 0, 0)";
}

CompositionPropertySet BindEdges(Compositor const& compositor, std::vector<Visual> const& sources) {
    if (sources.size() > 64) throw winrt::hresult_error(E_BOUNDS);
    auto edges = compositor.CreatePropertySet();
    for (unsigned group = 0; group < EdgeGroups(sources.size()); ++group) {
        size_t start = group * kEdgesPerGroup;
        size_t count = std::min(size_t{kEdgesPerGroup}, sources.size() - start);
        auto expression = compositor.CreateExpressionAnimation(MaxExpression(count, false));
        for (size_t i = 0; i < count; ++i)
            expression.SetReferenceParameter(L"b" + std::to_wstring(i), sources[start + i]);
        auto name = L"e" + std::to_wstring(group);
        edges.InsertScalar(name, 0);
        edges.StartAnimation(name, expression);
    }
    return edges;
}

void StopEdges(CompositionPropertySet const& edges, unsigned groups) {
    if (edges) for (unsigned i = 0; i < groups; ++i) edges.StopAnimation(L"e" + std::to_wstring(i));
}

void StopFollower(Root& root) {
    if (root.followerInstalled) {
        root.trayTranslation.element.StopAnimation(root.follower);
        root.buttonsTranslation.element.StopAnimation(root.buttonsFollower);
        root.followerInstalled = false;
        root.trayTranslation.applied = root.trayTranslation.element.Translation();
        root.buttonsTranslation.applied = root.buttonsTranslation.element.Translation();
    }
    root.follower = nullptr;
    root.buttonsFollower = nullptr;
    root.anchorIdentity = 0;
    root.anchorPrefix = 0;
    root.nativeRightOverhang = NAN;
    StopEdges(root.edges, root.edgeGroups);
    root.edges = nullptr;
    root.edgeGroups = 0;
    root.metrics = nullptr;
    root.expressionSources.clear();
    root.sources.clear();
}

void UpdateFollower(Root& root, FrameworkElement const& repeater, double origin,
                    double startLeft, double trayLeft, double trayWidth,
                    double shift, double buttonsWidth) {
    int count = Media::VisualTreeHelper::GetChildrenCount(repeater);
    // ponytail: bound the experiment to 64 realized children; larger trees fall
    // back to native geometry instead of producing an unbounded expression.
    if (count > 64) throw winrt::hresult_error(E_BOUNDS);
    std::erase_if(root.sources, [](auto const& source) { return !source.element.get(); });
    std::vector<uintptr_t> seen, selected;
    bool acquired = false;
    float floor = 0;
    uintptr_t anchorIdentity{};
    float anchorX{};
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
        if (!child) continue;
        auto slot = Controls::Primitives::LayoutInformation::GetLayoutSlot(child);
        bool eligible = child.Visibility() == Visibility::Visible &&
                        EligibleSlot(slot.X, slot.Width, repeater.ActualWidth());
        if (winrt::get_class_name(child) != L"Taskbar.TaskListButton") {
            if (eligible) floor = std::max(floor, slot.X + slot.Width);
            continue;
        }
        auto identity = CanonicalIdentity(child);
        seen.push_back(identity);
        auto source = std::find_if(root.sources.begin(), root.sources.end(),
            [&](auto const& value) { return value.identity == identity; });
        if (source == root.sources.end()) {
            if (!eligible) continue;
            // Windows uses handout Visuals for these outer buttons. Obtain that
            // same Visual for startup too; never request
            // a handout for the tray, repeater, icon or label facade targets.
            auto visual = Hosting::ElementCompositionPreview::GetElementVisual(child);
            root.sources.push_back({identity, winrt::make_weak(child), visual, Root::Source::Life::Active});
            acquired = true;
            source = root.sources.end() - 1;
        }
        if (IncludeSource(eligible, source->life)) selected.push_back(identity);
        if (eligible && source->life == Root::Source::Life::Active &&
            (!anchorIdentity || slot.X < anchorX)) {
            anchorIdentity = identity;
            anchorX = slot.X;
        }
        // A completed button remains excluded until parking or a new native
        // start proves reuse, even while its old slot still looks visible.
        if (!eligible && source->life != Root::Source::Life::Hiding) seen.pop_back();
    }
    std::erase_if(root.sources, [&](auto const& source) {
        return std::find(seen.begin(), seen.end(), source.identity) == seen.end();
    });
    float prefix = anchorX - static_cast<float>(startLeft);
    bool replaced = root.anchorIdentity && WholeGenerationReplaced(
        root.expressionSources, selected, root.anchorPrefix, prefix);
    // A partial identity change is an app reorder/removal. A complete same-size
    // replacement is the native element rebuild observed from Taskbar Labels.
    if (!anchorIdentity || prefix < 0 ||
        (root.anchorIdentity && (root.anchorIdentity != anchorIdentity ||
                                 std::abs(root.anchorPrefix - prefix) > 1) && !replaced))
        throw winrt::hresult_error(E_NOTIMPL, L"Leading app reference changed or unavailable");
    if (replaced) Wh_Log(L"[UWFOLLOW] button generation replaced root=%p sources=%zu",
                         winrt::get_abi(root.frame), selected.size());
    auto anchor = std::find_if(root.sources.begin(), root.sources.end(),
        [&](auto const& source) { return source.identity == anchorIdentity; });
    if (!root.metrics) {
        if (root.sources.empty()) throw winrt::hresult_error(E_PENDING);
        root.metrics = root.sources.front().visual.Compositor().CreatePropertySet();
    }
    bool knownBoundary = std::isfinite(root.nativeRightOverhang);
    double threshold = BoundaryThreshold(root.nativeRightOverhang, startLeft, buttonsWidth,
                                         repeater.ActualWidth(), trayLeft - origin);
    if (!knownBoundary && std::isfinite(root.nativeRightOverhang))
        Wh_Log(L"[UWFOLLOW] boundary root=%p threshold=%.3f overhang=%.3f tick=%llu",
               winrt::get_abi(root.frame), threshold, root.nativeRightOverhang, GetTickCount64());
    root.metrics.InsertScalar(L"threshold", static_cast<float>(threshold));
    root.metrics.InsertScalar(L"shift", static_cast<float>(shift));
    root.metrics.InsertScalar(L"trayBase", static_cast<float>(origin + shift + kGap - trayLeft));
    root.metrics.InsertScalar(L"prefix", prefix);
    root.metrics.InsertScalar(L"floor", std::max(0.0f, floor - static_cast<float>(startLeft)));
    root.metrics.InsertScalar(L"minX", static_cast<float>(-trayLeft));
    root.metrics.InsertScalar(L"maxX", static_cast<float>(root.hostWidth - trayWidth - trayLeft));
    if (root.followerInstalled && !acquired && selected == root.expressionSources) return;
    auto animation = root.metrics.Compositor().CreateExpressionAnimation(EdgeExpression(selected.size()));
    auto buttonsAnimation = root.metrics.Compositor().CreateExpressionAnimation(EdgeExpression(selected.size(), true));
    animation.Target(L"Translation");
    buttonsAnimation.Target(L"Translation");
    animation.SetReferenceParameter(L"geometry", root.metrics);
    buttonsAnimation.SetReferenceParameter(L"geometry", root.metrics);
    animation.SetReferenceParameter(L"anchor", anchor->visual);
    buttonsAnimation.SetReferenceParameter(L"anchor", anchor->visual);
    std::vector<Visual> visuals;
    for (size_t i = 0; i < selected.size(); ++i) {
        auto source = std::find_if(root.sources.begin(), root.sources.end(),
            [&](auto const& value) { return value.identity == selected[i]; });
        visuals.push_back(source->visual);
    }
    auto edges = BindEdges(root.metrics.Compositor(), visuals);
    animation.SetReferenceParameter(L"edges", edges);
    buttonsAnimation.SetReferenceParameter(L"edges", edges);
    // Keep cleanup armed before starting; a failed start is restored by Refresh's caller.
    StopEdges(root.edges, root.edgeGroups);
    root.edges = edges;
    root.edgeGroups = EdgeGroups(selected.size());
    root.follower = animation;
    root.buttonsFollower = buttonsAnimation;
    root.anchorIdentity = anchorIdentity;
    root.anchorPrefix = prefix;
    root.followerInstalled = true;
    root.trayTranslation.installed = true;
    root.buttonsTranslation.installed = true;
    root.buttonsTranslation.element.StartAnimation(buttonsAnimation);
    root.trayTranslation.element.StartAnimation(animation);
    root.expressionSources = std::move(selected);
}

void RestoreRoot(Root& root) {
    WithdrawTray(root);
    try { StopFollower(root); }
    catch (...) { Wh_Log(L"[UWFOLLOW] stop failed hr=%08X", winrt::to_hresult().value); }
    // Attempt both even if one XAML object is already closed.
    for (auto* owned : {&root.buttonsTranslation, &root.trayTranslation}) {
        try { Restore(*owned); }
        catch (...) { Wh_Log(L"[UWPOS] restoration-error element=%p", winrt::get_abi(owned->element)); }
    }
}

void Refresh(Root& root) {
    if (root.blocked) return;
    auto frame = root.frame;
    auto parent = Media::VisualTreeHelper::GetParent(frame).try_as<FrameworkElement>();
    auto grid = Child(frame, L"RootGrid");
    auto repeater = Child(grid, L"TaskbarFrameRepeater");
    auto start = Child(repeater, L"LaunchListButton");
    auto tray = Child(parent, L"SystemTray.SystemTrayFrame", true);
    if (!parent || !repeater || !start || !tray) {
        RestoreRoot(root);
        if (!root.fallbackReported) Wh_Log(L"[UWPOS] native fallback: required tree element missing");
        root.fallbackReported = true;
        return;
    }
    if ((root.buttonsTranslation.installed && root.buttonsTranslation.element != repeater) ||
        (root.trayTranslation.installed && root.trayTranslation.element != tray) ||
        (root.buttonsTranslation.installed && !root.followerInstalled && !StillOwned(root.buttonsTranslation)) ||
        (root.trayTranslation.installed && !root.followerInstalled && !StillOwned(root.trayTranslation))) {
        RestoreRoot(root);
        root.blocked = true;
        Wh_Log(L"[UWPOS] root blocked after ownership/tree change; re-enable to retry");
        return;
    }
    double host = parent.ActualWidth();
    double buttons = repeater.DesiredSize().Width;
    double trayWidth = tray.ActualWidth();
    auto overflow = Child(repeater, L"OverflowButton");
    bool overflowing = overflow &&
        Controls::Primitives::LayoutInformation::GetLayoutSlot(overflow).Width > 0;
    // ActualOffset excludes our facade Translation, so an animated parent
    // cannot feed back into the source geometry.
    double origin = frame.ActualOffset().x + grid.ActualOffset().x + repeater.ActualOffset().x;
    if (!root.followerInstalled && std::abs(origin - frame.ActualOffset().x -
        repeater.TransformToVisual(frame).TransformPoint({0, 0}).X) > 1)
        throw winrt::hresult_error(E_NOTIMPL);
    double repeaterWidth = repeater.ActualWidth();
    double nativeCenter = origin + repeaterWidth / 2;
    double startTarget = Controls::Primitives::LayoutInformation::GetLayoutSlot(start).X;
    double expectedStart = (repeaterWidth - buttons) / 2;
    double trayLeft = tray.ActualOffset().x;
    Placement target{};
    // Keep the full centered repeater budget. Account for the native strip's
    // layout offset without reading per-button animation transforms.
    bool supported = !overflowing && std::abs(frame.ActualWidth() - host) <= 1 &&
        Plan(host, buttons, trayWidth, nativeCenter, trayLeft, startTarget - expectedStart, target);
    if (!supported) {
        RestoreRoot(root);
        if (!root.fallbackReported) Wh_Log(L"[UWPOS] native fallback host=%.3f frame=%.3f buttons=%.3f tray=%.3f center=%.3f start=%.3f overflow=%d root=%p repeaterWidth=%.3f expectedStart=%.3f trayLeft=%.3f",
            host, frame.ActualWidth(), buttons, trayWidth, nativeCenter, startTarget, overflowing,
            winrt::get_abi(frame), repeaterWidth, expectedStart, trayLeft);
        root.fallbackReported = true;
        return;
    }
    root.fallbackReported = false;
    if (!root.followerInstalled && (!Prepare(repeater, root.buttonsTranslation) ||
                                   !Prepare(tray, root.trayTranslation))) {
        RestoreRoot(root);
        root.blocked = true;
        Wh_Log(L"[UWPOS] translation unavailable; root left native");
        DescribeTransform(L"buttons", repeater);
        DescribeTransform(L"tray", tray);
        return;
    }
    root.hostWidth = host;
    UpdateFollower(root, repeater, origin, startTarget, trayLeft, trayWidth,
                   (host - trayWidth - kGap) / 2 - nativeCenter, buttons);
    // Publication marks availability; popup hooks query current transformed
    // tray bounds on the owner thread instead of consuming this layout estimate.
    PublishTray(root, host, target.trayX);
}

void OnLayout(winrt::weak_ref<FrameworkElement> const& weak) {
    if (g_stopping) return;
    ++g_active;
    struct Finish { ~Finish() { --g_active; } } finish;
    if (g_stopping || GetCurrentThreadId() != g_ownerThread || g_busy) return;
    FrameworkElement frame{nullptr};
    try { frame = weak.get(); }
    catch (...) { Wh_Log(L"[UWPOS] layout owner unavailable"); return; }
    if (!frame) return;
    g_busy = true;
    struct Unbusy { ~Unbusy() { g_busy = false; } } unbusy;
    for (auto& root : g_roots) {
        if (root.frame != frame) continue;
        try { Refresh(root); }
        catch (...) {
            auto error = winrt::to_hresult();
            RestoreRoot(root);
            root.blocked = true;
            Wh_Log(L"[UWPOS] refresh-error hr=%08X; root blocked", error.value);
        }
        break;
    }
}

void MotionEvent(void* abi, Root::Source::Life life, bool cancelled) noexcept {
    if (g_stopping || GetCurrentThreadId() != g_ownerThread || !abi) return;
    try {
        winrt::Windows::Foundation::IUnknown borrowed{nullptr};
        winrt::copy_from_abi(borrowed, abi);
        auto node = borrowed.try_as<FrameworkElement>();
        for (unsigned depth = 0; node && depth < 32; ++depth) {
            if (winrt::get_class_name(node) == L"Taskbar.TaskListButton") break;
            node = Media::VisualTreeHelper::GetParent(node).try_as<FrameworkElement>();
        }
        if (!node || winrt::get_class_name(node) != L"Taskbar.TaskListButton") return;
        auto identity = CanonicalIdentity(node);
        for (auto& root : g_roots) {
            auto source = std::find_if(root.sources.begin(), root.sources.end(),
                [&](auto const& value) { return value.identity == identity && value.element.get() == node; });
            if (source == root.sources.end()) continue;
            auto next = NextLife(source->life, life, cancelled);
            if (next == source->life) return;
            source->life = next;
            // Hide completion is observed before native recycling. Rebuild the
            // reference set now; normal layout will then supply the new slots.
            if (!g_busy) OnLayout(winrt::make_weak(root.frame));
            return;
        }
    } catch (...) { Wh_Log(L"[UWFOLLOW] lifecycle failed hr=%08X", winrt::to_hresult().value); }
}

LRESULT QueryCurrentTray(HWND taskbar) {
    if (g_stopping || g_busy || GetCurrentThreadId() != g_ownerThread) return 0;
    ++g_active;
    struct Finish { ~Finish() { --g_active; } } finish;
    for (auto& root : g_roots) {
        if (root.taskbar != taskbar || !root.followerInstalled || !GetProp(taskbar, kTrayShift)) continue;
        try {
            auto tray = root.trayTranslation.element;
            auto parent = Media::VisualTreeHelper::GetParent(tray).try_as<FrameworkElement>();
            if (!parent) return 0;
            double shift = tray.TransformToVisual(parent).TransformPoint({0, 0}).X - tray.ActualOffset().x;
            double scale = root.frame.XamlRoot().RasterizationScale();
            if (!std::isfinite(shift * scale) || std::abs(shift) > root.hostWidth ||
                std::abs(shift * scale) > INT_MAX) return 0;
            // TransformToVisual is an on-demand XAML observation, not a promise
            // of exact compositor-frame readback during concurrent motion.
            return reinterpret_cast<LRESULT>(EncodeShift(static_cast<int>(std::lround(shift * scale))));
        } catch (...) { Wh_Log(L"[UWFOLLOW] popup sample failed hr=%08X", winrt::to_hresult().value); }
    }
    return 0;
}

void DiscoverExisting(HWND taskbar);

LRESULT CALLBACK DispatchProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    if (message == kQueryTrayMessage) return QueryCurrentTray(reinterpret_cast<HWND>(wp));
    if (message == kRestoreMessage) {
        if (g_panelLocationHook) {
            UnhookWinEvent(g_panelLocationHook);
            g_panelLocationHook = nullptr;
        }
        for (auto& root : g_roots) {
            try { root.frame.LayoutUpdated(root.layoutToken); }
            catch (...) { Wh_Log(L"[UWPOS] unsubscribe-error frame=%p", winrt::get_abi(root.frame)); }
            RestoreRoot(root);
        }
        g_roots.clear();
        DestroyWindow(window);
        g_dispatch = nullptr;
        return 0;
    }
    return DefWindowProc(window, message, wp, lp);
}

bool EnsureDispatcher() {
    DWORD current = GetCurrentThreadId(), empty = 0;
    g_ownerThread.compare_exchange_strong(empty, current);
    // ponytail: one taskbar UI thread, as observed on this machine. Additional
    // threads need separate dispatchers before extending the runtime matrix.
    if (g_ownerThread != current) return false;
    if (!g_dispatch) {
        g_dispatch = CreateWindowEx(0, kWindowClass, L"", 0, 0, 0, 0, 0,
                                    HWND_MESSAGE, nullptr, GetModuleHandle(nullptr), nullptr);
    }
    if (g_dispatch && !g_panelLocationHook) {
        g_panelLocationHook = SetWinEventHook(
            EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
            PanelLocationEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        if (!g_panelLocationHook)
            Wh_Log(L"[UWPANEL] location hook failed error=%lu", GetLastError());
    }
    return g_dispatch != nullptr;
}

void WatchFrame(FrameworkElement frame) {
    if (!frame || !EnsureDispatcher()) return;
    for (auto& root : g_roots) if (root.frame == frame) return;
    g_busy = true;
    struct Unbusy { ~Unbusy() { g_busy = false; } } unbusy;
    Root root;
    root.frame = frame;
    auto weak = winrt::make_weak(frame);
    root.layoutToken = frame.LayoutUpdated([weak](auto&&, auto&&) { OnLayout(weak); });
    try { g_roots.push_back(root); }
    catch (...) { frame.LayoutUpdated(root.layoutToken); throw; }
    Wh_Log(L"[UWPOS] watching frame=%p thread=%lu", winrt::get_abi(frame), GetCurrentThreadId());
}

void DiscoverExisting(HWND taskbar) {
    ++g_active;
    struct Finish { ~Finish() { --g_active; } } finish;
    if (g_stopping) return;
    if (g_ownerThread && g_ownerThread != GetCurrentThreadId()) {
        Wh_Log(L"[UWPOS] startup unsupported UI thread taskbar=%p thread=%lu", taskbar, GetCurrentThreadId());
        return;
    }
    if (g_busy) return;
    try {
        auto xamlRoot = ExistingRoot(taskbar);
        auto frame = xamlRoot ? Child(xamlRoot.Content().try_as<FrameworkElement>(),
                                      L"Taskbar.TaskbarFrame", true) : nullptr;
        if (!frame) {
            Wh_Log(L"[UWPOS] startup root unavailable taskbar=%p; native layout retained", taskbar);
            return;
        }
        WatchFrame(frame);
        // The existing root is already arranged. Use the same guarded Refresh
        // path as later LayoutUpdated events without invalidating its geometry.
        OnLayout(winrt::make_weak(frame));
        Wh_Log(L"[UWPOS] startup discovered taskbar=%p frame=%p published=%d", taskbar,
               winrt::get_abi(frame), GetProp(taskbar, kTrayShift) != nullptr);
    } catch (...) {
        Wh_Log(L"[UWPOS] startup discovery failed taskbar=%p hr=%08X", taskbar, winrt::to_hresult().value);
    }
}

LRESULT CALLBACK DiscoverHook(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && lp) {
        const auto& message = *reinterpret_cast<CWPSTRUCT*>(lp);
        if (g_discoverMessage && message.message == g_discoverMessage &&
            message.wParam == reinterpret_cast<WPARAM>(&g_discoverMessage)) DiscoverExisting(message.hwnd);
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

bool RequestDiscovery(HWND taskbar) {
    DWORD pid{};
    DWORD thread = GetWindowThreadProcessId(taskbar, &pid);
    if (!g_discoverMessage || g_stopping || !thread || pid != GetCurrentProcessId() ||
        (!ClassIs(taskbar, L"Shell_TrayWnd") && !ClassIs(taskbar, L"Shell_SecondaryTrayWnd"))) return false;
    if (thread == GetCurrentThreadId()) {
        DiscoverExisting(taskbar);
        return true;
    }
    // Same-process, one-shot UI-thread dispatch used by hide-start-button.
    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, DiscoverHook, nullptr, thread);
    if (!hook) return false;
    SendMessage(taskbar, g_discoverMessage, reinterpret_cast<WPARAM>(&g_discoverMessage), 0);
    BOOL removed = UnhookWindowsHookEx(hook);
    if (!removed) Wh_Log(L"[UWPOS] startup dispatch unhook failed error=%lu", GetLastError());
    return removed;
}

using Arrange_t = int(WINAPI*)(void*, Size, Size*);
Arrange_t Arrange_Original;
int WINAPI Arrange_Hook(void* self, Size size, Size* result) {
    ++g_active;
    struct Finish { ~Finish() { --g_active; } } finish;
    int hr = Arrange_Original(self, size, result);
    if (g_stopping || FAILED(hr) || g_busy) return hr;
    try {
        if (!EnsureDispatcher()) return hr;
        FrameworkElement frame{nullptr};
        winrt::check_hresult(static_cast<IUnknown*>(self)->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(frame)));
        WatchFrame(frame);
    } catch (...) { Wh_Log(L"[UWPOS] discovery-error; no positioning for this pass"); }
    return hr;
}

bool ResolveStartupSymbols() {
    HMODULE module = GetModuleHandle(L"taskbar.dll");
    if (!module) return false;
    WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_taskBandVtable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_secondaryTaskBandVtable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &GetTaskbarHost_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &GetSecondaryTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &g_frameHeight},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &Decref_Original},
    };
    if (!WindhawkUtils::HookSymbols(module, symbols, ARRAYSIZE(symbols))) return false;
    BYTE code[8]{};
    if (!ReadLocal(g_frameHeight, code) || !HostElementOffset(code, g_hostElementOffset)) {
        Wh_Log(L"[UWPOS] unsupported TaskbarHost::FrameHeight; startup lookup refused");
        return false;
    }
    g_discoverMessage = RegisterWindowMessage(L"Windhawk.UWPOS06.DiscoverExisting");
    Wh_Log(L"[UWPOS] startup symbols resolved hostElementOffset=%zu", g_hostElementOffset);
    return g_discoverMessage != 0;
}

BOOL Wh_ModInit() {
    wchar_t path[MAX_PATH];
    if (!GetModuleFileName(nullptr, path, ARRAYSIZE(path))) return FALSE;
    auto name = wcsrchr(path, L'\\');
    g_explorer = name && !_wcsicmp(name + 1, L"explorer.exe");
    if (!WindhawkUtils::SetFunctionHook(SetWindowPos, PanelSetWindowPos, &SetWindowPos_Original) ||
        !WindhawkUtils::SetFunctionHook(DestroyWindow, PanelDestroyWindow, &DestroyWindow_Original)) return FALSE;
    if (!g_explorer) {
        Wh_Log(L"[UWPANEL] initialized v2.0 Shell positioning hooks");
        return TRUE;
    }
    if (!WindhawkUtils::SetFunctionHook(DwmSetWindowAttribute, PanelDwmAttribute,
                                        &DwmSetWindowAttribute_Original)) return FALSE;
    DWORD alignment = 0, bytes = sizeof(alignment);
    if (RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                    L"TaskbarAl", RRF_RT_REG_DWORD, nullptr, &alignment, &bytes) != ERROR_SUCCESS || alignment != 1) {
        Wh_Log(L"[UWPOS] centered native alignment required");
        return FALSE;
    }
    auto module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) return FALSE;
    if (!ResolveStartupSymbols()) return FALSE;
    WindhawkUtils::SYMBOL_HOOK hooks[] = {{
        {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::ArrangeOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
        &Arrange_Original, Arrange_Hook,
    }, {
        {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarElementAnimator,struct winrt::Microsoft::UI::Xaml::Controls::IElementAnimatorOverrides>::StartHideAnimation(void *,unsigned int))"},
        &HideAnimation_Original, HideAnimation_Hook,
    }, {
        {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IUIElement9<struct winrt::Windows::UI::Xaml::Controls::Image>::StartAnimation(struct winrt::Windows::UI::Composition::ICompositionAnimationBase const &)const )"},
        &StartAnimation_Original, StartAnimation_Hook,
    }, {
        {LR"(public: __cdecl winrt::impl::consume_Microsoft_UI_Xaml_Controls_IElementAnimatorProtected<struct winrt::Taskbar::implementation::TaskbarElementAnimator>::OnHideAnimationCompleted(struct winrt::Windows::UI::Xaml::UIElement const &)const )"},
        &HideCompleted_Original, HideCompleted_Hook,
    }};
    WNDCLASS wc{};
    wc.lpfnWndProc = DispatchProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = kWindowClass;
    g_windowClass = RegisterClass(&wc);
    if (!g_windowClass) return FALSE;
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        UnregisterClass(kWindowClass, wc.hInstance);
        g_windowClass = 0;
        return FALSE;
    }
    Wh_Log(L"[UWPOS] initialized v2.0; native animation follower, complete button-generation replacement supported, gap=12");
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_explorer || g_stopping) return;
    for (auto name : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"}) {
        HWND window{};
        for (int i = 0; i < 16 && (window = FindWindowEx(nullptr, window, name, nullptr)); ++i) {
            DWORD pid{};
            GetWindowThreadProcessId(window, &pid);
            if (pid != GetCurrentProcessId()) continue;
            if (!RequestDiscovery(window)) Wh_Log(L"[UWPOS] startup dispatch failed taskbar=%p", window);
        }
    }
}

void Wh_ModBeforeUninit() {
    g_stopping = true;
    // Wait for mutation/discovery to finish before dispatching UI-thread cleanup.
    while (g_active) Sleep(1);
    if (HWND window = g_dispatch) SendMessage(window, kRestoreMessage, 0, 0);
    RestorePanels(nullptr, !g_explorer);
}

void Wh_ModUninit() {
    if (g_windowClass) UnregisterClass(kWindowClass, GetModuleHandle(nullptr));
}
