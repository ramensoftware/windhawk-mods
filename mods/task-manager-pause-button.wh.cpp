// ==WindhawkMod==
// @id              task-manager-pause-button
// @name            Task Manager Pause Button
// @description     Add a native Pause/Resume button to Processes and Details in Windows 11 Task Manager.
// @version         1.1.0
// @author          Luiz Bizzio
// @github          https://github.com/luizbizzio
// @homepage        https://github.com/luizbizzio/task-manager-pause-button
// @donateUrl       https://github.com/sponsors/luizbizzio
// @include         Taskmgr.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         Apache-2.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task Manager Pause Button

Add a native **Pause / Resume** button next to **Run new task** in the
Windows 11 Task Manager on the Processes and Details pages.

![Task Manager Pause Button](https://raw.githubusercontent.com/luizbizzio/task-manager-pause-button/main/assets/task-manager-pause-button-cover.png)

## How to use

1. Open Task Manager and select **Processes** or **Details**.
2. Click **Pause** to stop the list from updating.
3. Click **Resume** to start updating again.

The button controls Task Manager's refresh hold on Processes and Details.
The mod does not suspend apps, stop Windows processes, or change the
Real time update speed setting.

## Compatibility and limitations

- Experimental XAML Diagnostics discovery for the Windows 11 Task Manager.
  It avoids private Task Manager PDB symbols and hardcoded function offsets,
  but Windows updates can still change the UI tree or refresh behavior.
  XAML Diagnostics can conflict with other consumers in the same process.
- Uses Task Manager's Ctrl-key refresh hold through `GetKeyState` calls made
  directly by `Taskmgr.exe`. While paused, some Task Manager keyboard actions
  may behave as if Ctrl is held, including Run new task (Ctrl+click can open
  Command Prompt instead of the normal dialog). This does not affect other apps.
- Recognizes the tested Task Manager command-bar layout independently of
  display language: one command bar with four native buttons, with Run new
  task in the first position. Falls back to English/Portuguese labels or
  recognizable control identifiers on other layouts. Other Windows versions
  can have different layouts, so all languages/builds are not guaranteed.
- Pause/Resume labels are in English regardless of the Task Manager language.
- Keeps Pause active when moving between Processes and Details. Subscribes to
  the public XAML Frame.Navigated event where available to resume on other
  pages; also recognizes visible non-supported Task Manager pages as fallback.
  Unrecognized custom navigation layouts may still require manual Resume.
- Uses the Windows.UI.Xaml Diagnostics connection to discover native controls.
  Only one diagnostics consumer can own the XAML connection in a process.
  If another tool owns it, this experimental build may not insert a button.
  No hardcoded function offsets are used.
- If the button is missing, close and reopen Task Manager with the mod enabled.
- A short-lived diagnostic worker is started inside Task Manager. XAML
  subscriptions and worker threads are cleaned up on disable. No external
  application or registry changes are needed.

## Source and feedback

[GitHub](https://github.com/luizbizzio/task-manager-pause-button)

Author: Luiz Bizzio. License: Apache-2.0.
*/
// ==/WindhawkModReadme==

// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Luiz Bizzio

#include <windows.h>
#include <xamlom.h>
#include <ocidl.h>
#include <inspectable.h>
#include <windhawk_utils.h>
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Navigation.h>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>
#include <cwctype>
#include <utility>

namespace mux = winrt::Windows::UI::Xaml;
namespace muxc = winrt::Windows::UI::Xaml::Controls;
namespace muxm = winrt::Windows::UI::Xaml::Media;
namespace muxa = winrt::Windows::UI::Xaml::Automation;
namespace wf = winrt::Windows::Foundation;

namespace {
constexpr wchar_t kButtonName[] = L"WindhawkTaskManagerPauseButton";

std::atomic<bool> g_stopping{false};
std::atomic<bool> g_paused{false};
std::atomic<unsigned> g_insertions{0};

uintptr_t g_taskBegin{};
uintptr_t g_taskEnd{};

using GetKeyState_t = decltype(&GetKeyState);
GetKeyState_t g_getKeyOriginal{};
// XAML Diagnostics is used instead of Window::Current(), which is often
// unavailable on XAML Islands. No Taskmgr.exe private symbol is required.
std::atomic<bool> g_diagnosticsStop{false};
std::atomic<unsigned> g_diagnosticCandidates{0};
std::atomic<unsigned> g_diagnosticErrors{0};
std::atomic<unsigned> g_diagnosticPages{0};
std::atomic<unsigned> g_diagnosticWatchers{0};
// XAML elements can disappear before the diagnostics callback looks them up.
std::atomic<unsigned> g_diagnosticExpiredHandles{0};
HANDLE g_diagnosticsWorker{};
HANDLE g_diagnosticsStopEvent{};

bool IsCtrl(int vk) noexcept {
    return vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL;
}

bool CalledByTaskManager(uintptr_t returnAddress) noexcept {
    return returnAddress >= g_taskBegin && returnAddress < g_taskEnd;
}

SHORT WINAPI GetKeyState_Hook(int vk) {
    SHORT value = g_getKeyOriginal(vk);
    if (g_paused.load(std::memory_order_relaxed) &&
        !g_stopping.load(std::memory_order_relaxed) && IsCtrl(vk) &&
        CalledByTaskManager(reinterpret_cast<uintptr_t>(__builtin_return_address(0)))) {
        value = static_cast<SHORT>(value | 0x8000);
    }
    return value;
}

struct BarEntry {
    winrt::weak_ref<muxc::CommandBar> bar;
    winrt::weak_ref<muxc::AppBarButton> button;
    DWORD ownerThread{};
    winrt::event_token clickToken{};
    uint64_t entryId{};
    bool clickAttached{};
};

std::mutex g_barsMutex;
// Prevent COM weak references from being destroyed by static shutdown after
// the Windhawk DLL and the XAML runtime have started tearing down.
[[clang::no_destroy]] std::vector<BarEntry> g_bars;
std::atomic<uint64_t> g_nextEntryId{1};
thread_local bool g_addingButton = false;

// Close admission to new insertions before taking a cleanup snapshot. Unlike
// dispatching only to the main HWND, this also covers a separate XAML thread.
std::mutex g_insertionMutex;
std::condition_variable g_insertionDone;
bool g_acceptingInsertions = true;
unsigned g_insertionCount = 0;

struct InsertionGuard {
    bool active{};
    InsertionGuard() {
        std::lock_guard<std::mutex> lock(g_insertionMutex);
        if (!g_acceptingInsertions || g_stopping.load(std::memory_order_acquire))
            return;
        ++g_insertionCount;
        active = true;
    }
    ~InsertionGuard() {
        if (!active) return;
        std::lock_guard<std::mutex> lock(g_insertionMutex);
        if (--g_insertionCount == 0) g_insertionDone.notify_all();
    }
};

void StopNewInsertions() {
    std::lock_guard<std::mutex> lock(g_insertionMutex);
    g_acceptingInsertions = false;
    g_stopping.store(true, std::memory_order_release);
    g_paused.store(false, std::memory_order_release);
}

HWND FindTaskManagerWindow();
void SyncButtons();
std::vector<DWORD> NavigationThreads();
bool RemoveThreadNavigationHooks(DWORD threadId);
void ForgetNavigationThread(DWORD threadId);

void UpdateButton(muxc::AppBarButton const& button) {
    const bool paused = g_paused.load(std::memory_order_relaxed);
    const wchar_t* caption = paused ? L"Resume" : L"Pause";
    if (button.Label() == caption) return;
    button.Label(caption);
    button.Icon(muxc::SymbolIcon(paused ? muxc::Symbol::Play : muxc::Symbol::Pause));
}

void TogglePause() {
    bool was = g_paused.load(std::memory_order_relaxed);
    while (!g_paused.compare_exchange_weak(was, !was, std::memory_order_relaxed)) {}
    SyncButtons();
}

// Identifiers are checked only for their semantic role, not an assumed
// Windows-specific AutomationId spelling. Both anchors must be found.
std::wstring NormalizeIdentifier(winrt::hstring const& value) {
    std::wstring normalized;
    for (wchar_t c : value) {
        if (std::iswalnum(c))
            normalized.push_back(static_cast<wchar_t>(std::towlower(c)));
    }
    return normalized;
}

bool MatchesRole(muxc::AppBarButton const& item, bool runNewTask) {
    const auto label = item.Label();
    if (runNewTask) {
        if (label == L"Run new task" || label == L"Executar nova tarefa")
            return true;
    } else if (label == L"End task" || label == L"Finalizar tarefa") {
        return true;
    }

    const auto matchesId = [runNewTask](winrt::hstring const& identifier) {
        const auto normalized = NormalizeIdentifier(identifier);
        return runNewTask
            ? normalized.find(L"runnewtask") != std::wstring::npos ||
                  normalized.find(L"createnewtask") != std::wstring::npos
            : normalized.find(L"endtask") != std::wstring::npos;
    };
    if (matchesId(item.Name())) return true;
    try {
        return matchesId(muxa::AutomationProperties::GetAutomationId(item));
    } catch (...) {
        // Some builds may not expose a usable AutomationId on this control.
        return false;
    }
}

struct BarCandidate {
    muxc::CommandBar bar{nullptr};
    uint32_t place{};
};

// Resolve native commands by labels/IDs when available. Do not check
// ancestor Visibility here: OnNavigatedTo can fire before page display.
bool InspectBar(muxc::CommandBar const& bar, BarCandidate& candidate) {
    if (!bar) return false;
    auto commands = bar.PrimaryCommands();
    if (!commands) return false;

    bool hasRun = false;
    bool hasEnd = false;
    uint32_t afterRun{};
    for (uint32_t i = 0; i < commands.Size(); ++i) {
        auto item = commands.GetAt(i).try_as<muxc::AppBarButton>();
        if (!item || item.Name() == kButtonName) continue;
        if (MatchesRole(item, true)) {
            hasRun = true;
            afterRun = i + 1;
        }
        if (MatchesRole(item, false)) hasEnd = true;
    }
    if (!hasRun || !hasEnd) return false;
    candidate.bar = bar;
    candidate.place = afterRun;
    return true;
}

// On the tested Task Manager layout, page navigation exposes one CommandBar
// with four native AppBarButtons and Run new task as its first native command.
//
// This is a conservative, language-independent fallback for that layout,
// not a universal identification of the semantic role of each button.
// In particular, don't infer a position from an arbitrary group of 2+ buttons.
// Other layouts can still use the semantic label/identifier matcher.
bool InspectSinglePageBarByStructure(muxc::CommandBar const& bar,
                                     BarCandidate& candidate) {
    if (!bar) return false;
    auto commands = bar.PrimaryCommands();
    if (!commands) return false;

    uint32_t nativeCount = 0;
    uint32_t otherCommands = 0;
    uint32_t ownButtons = 0;
    uint32_t firstNativeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < commands.Size(); ++i) {
        auto button = commands.GetAt(i).try_as<muxc::AppBarButton>();
        if (!button) {
            // A separator can exist in some builds, but do not match bars
            // with an otherwise different collection of commands.
            if (++otherCommands > 1) return false;
            continue;
        }
        if (button.Name() == kButtonName) {
            if (++ownButtons > 1) return false;
            continue;
        }
        if (firstNativeIndex == UINT32_MAX) firstNativeIndex = i;
        if (++nativeCount > 4) return false;
    }

    if (nativeCount != 4 || firstNativeIndex != 0) return false;
    candidate.bar = bar;
    candidate.place = firstNativeIndex + 1;
    return true;
}

// Prefer a verified structural layout when unique to this page-navigation
// scan. If the bar's known labels/IDs contradict the proposed position,
// trust the explicit semantic match instead of moving the native action.
bool InspectProductionBar(muxc::CommandBar const& bar,
                          bool uniquePageBar,
                          BarCandidate& candidate) {
    BarCandidate identified;
    const bool hasIdentifiedRoles = InspectBar(bar, identified);
    if (uniquePageBar) {
        BarCandidate structural;
        if (InspectSinglePageBarByStructure(bar, structural)) {
            if (hasIdentifiedRoles && structural.place != identified.place) {
                Wh_Log(L"Structural placement differs from identified Run new task; using verified label/ID");
                candidate = identified;
            } else {
                candidate = structural;
            }
            return true;
        }
    }
    if (hasIdentifiedRoles) {
        candidate = identified;
        return true;
    }
    return false;
}

void TryInsertButton(muxc::CommandBar const& bar, uint32_t place) {
    if (!bar || g_addingButton) return;
    InsertionGuard insertion;
    if (!insertion.active) return;
    struct Guard {
        Guard() { g_addingButton = true; }
        ~Guard() { g_addingButton = false; }
    } guard;

    muxc::AppBarButton button{nullptr};
    winrt::event_token clickToken{};
    bool clickAttached = false;
    bool inserted = false;
    bool tracked = false;
    uint64_t entryId{};
    try {
        auto commands = bar.PrimaryCommands();
        if (!commands || place > commands.Size()) return;

        // On a previous unload a button can remain visible after its Click
        // handler was successfully detached but XAML RemoveAt failed.
        // Remove that inert control before adding the new instance.
        for (uint32_t i = commands.Size(); i > 0; --i) {
            auto item = commands.GetAt(i - 1).try_as<muxc::AppBarButton>();
            if (!item || item.Name() != kButtonName) continue;
            std::vector<winrt::weak_ref<muxc::AppBarButton>> trackedButtons;
            {
                std::lock_guard<std::mutex> lock(g_barsMutex);
                for (const auto& entry : g_bars) {
                    if (entry.ownerThread == GetCurrentThreadId())
                        trackedButtons.push_back(entry.button);
                }
            }
            bool activeButton = false;
            for (const auto& weakButton : trackedButtons) {
                if (weakButton.get() == item) {
                    activeButton = true;
                    break;
                }
            }
            if (activeButton) return;
            commands.RemoveAt(i - 1);
            if (i - 1 < place) --place;
            Wh_Log(L"Removed detached button left by an earlier unload");
        }

        button = muxc::AppBarButton();
        button.Name(kButtonName);
        UpdateButton(button);
        commands.InsertAt(place, button);
        inserted = true;

        // Allocate bookkeeping BEFORE subscribing to Click. If allocation
        // fails, we can roll back a button that never had a mod callback.
        BarEntry entry;
        entry.bar = winrt::make_weak(bar);
        entry.button = winrt::make_weak(button);
        entry.ownerThread = GetCurrentThreadId();
        entry.entryId = g_nextEntryId.fetch_add(1, std::memory_order_relaxed);
        entryId = entry.entryId;
        {
            std::lock_guard<std::mutex> lock(g_barsMutex);
            g_bars.push_back(std::move(entry));
        }
        tracked = true;

        clickToken = button.Click([](auto const&, auto const&) {
            if (!g_stopping.load(std::memory_order_acquire)) TogglePause();
        });
        clickAttached = true;
        {
            std::lock_guard<std::mutex> lock(g_barsMutex);
            for (auto& saved : g_bars) {
                if (saved.entryId == entryId) {
                    saved.clickToken = clickToken;
                    saved.clickAttached = true;
                    break;
                }
            }
        }

        const auto count = g_insertions.fetch_add(1) + 1;
        if (count <= 8) Wh_Log(L"Native Pause button inserted (%u)", count);
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        bool safeToForget = !clickAttached;
        if (button && clickAttached) {
            try {
                button.Click(clickToken);
                safeToForget = true;
            } catch (...) {
                Wh_Log(L"WARNING: event rollback failed; keeping entry for unload retry");
            }
        }
        if (safeToForget) {
            if (tracked) {
                std::lock_guard<std::mutex> lock(g_barsMutex);
                g_bars.erase(std::remove_if(g_bars.begin(), g_bars.end(),
                    [entryId](BarEntry const& e) { return e.entryId == entryId; }),
                    g_bars.end());
            }
            if (inserted) {
                try {
                    auto commands = bar.PrimaryCommands();
                    if (commands) {
                        for (uint32_t i = commands.Size(); i > 0; --i) {
                            if (auto item = commands.GetAt(i - 1).try_as<muxc::AppBarButton>()) {
                                if (item == button) {
                                    commands.RemoveAt(i - 1);
                                    break;
                                }
                            }
                        }
                    }
                } catch (...) {
                    Wh_Log(L"Could not remove detached button during rollback");
                }
            }
        }
        Wh_Log(L"Button insertion failed: 0x%08X", error);
    }
}

// Synchronous UI dispatch, following Windhawk's RunFromWindowThread pattern.
// Keep a short-lived request registry to reject unrelated registered messages.
// Never hold the registry mutex while sending a message or invoking a callback.
UINT g_dispatchMsg{};

struct UiRequest {
    HWND hwnd{};
    DWORD threadId{};
    bool (*callback)(DWORD){};
    std::atomic<bool> invoked{false};
    std::atomic<bool> succeeded{false};
    UiRequest* next{};
};

std::mutex g_dispatchMutex;
UiRequest* g_activeRequests{}; // Protected by g_dispatchMutex.

bool UiThreadTerminated(DWORD threadId) {
    HANDLE thread = OpenThread(SYNCHRONIZE, FALSE, threadId);
    if (!thread) return GetLastError() == ERROR_INVALID_PARAMETER;
    const DWORD result = WaitForSingleObject(thread, 0);
    CloseHandle(thread);
    return result == WAIT_OBJECT_0;
}

HWND FindTaskManagerWindow() {
    HWND result = nullptr;
    EnumWindows([](HWND hwnd, LPARAM param) -> BOOL {
        DWORD pid{};
        wchar_t className[64]{};
        if (GetWindowThreadProcessId(hwnd, &pid) &&
            pid == GetCurrentProcessId() &&
            GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
            wcscmp(className, L"TaskManagerWindow") == 0) {
            *reinterpret_cast<HWND*>(param) = hwnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

LRESULT CALLBACK DispatchHook(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && g_dispatchMsg) {
        auto* msg = reinterpret_cast<const CWPSTRUCT*>(lp);
        if (msg && msg->message == g_dispatchMsg) {
            // Do not dereference an arbitrary lParam before verifying it is
            // one of our live requests. Multiple UI threads may dispatch at once.
            auto* candidate = reinterpret_cast<UiRequest*>(msg->lParam);
            bool run = false;
            {
                std::lock_guard<std::mutex> lock(g_dispatchMutex);
                for (auto* request = g_activeRequests; request; request = request->next) {
                    if (request == candidate && request->hwnd == msg->hwnd &&
                        request->threadId == GetCurrentThreadId() &&
                        !request->invoked.exchange(true, std::memory_order_acq_rel)) {
                        run = true;
                        break;
                    }
                }
            }
            if (run) {
                try {
                    candidate->succeeded.store(candidate->callback(candidate->threadId),
                                               std::memory_order_release);
                } catch (...) {
                    Wh_Log(L"UI dispatch callback exception");
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

void UnhookDispatchSafely(HHOOK hook, DWORD threadId) {
    if (!hook) return;
    unsigned attempts = 0;
    while (!UnhookWindowsHookEx(hook)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HOOK_HANDLE || UiThreadTerminated(threadId))
            return;
        if (++attempts % 50 == 0)
            Wh_Log(L"Waiting to remove UI hook: thread=%lu error=%lu", threadId, error);
        Sleep(100);
    }
}

// Use Task Manager's main window when possible, otherwise one of its XAML
// host descendants belonging to the requested thread. Do not send a hook
// message to an arbitrary IME or unrelated thread window.
HWND FindUiWindowForThread(DWORD threadId) {
    HWND mainWindow = FindTaskManagerWindow();
    if (mainWindow && GetWindowThreadProcessId(mainWindow, nullptr) == threadId)
        return mainWindow;

    struct FindWindowContext {
        DWORD threadId;
        HWND result;
    } context{threadId, nullptr};
    if (mainWindow) {
        EnumChildWindows(mainWindow, [](HWND hwnd, LPARAM param) -> BOOL {
            auto* search = reinterpret_cast<FindWindowContext*>(param);
            DWORD pid{};
            DWORD owner = GetWindowThreadProcessId(hwnd, &pid);
            if (pid == GetCurrentProcessId() && owner == search->threadId) {
                search->result = hwnd;
                return FALSE;
            }
            return TRUE;
        }, reinterpret_cast<LPARAM>(&context));
        if (context.result) return context.result;
    }

    // After a main window closes, an owned top-level XAML host may still
    // exist while its UI thread drains. Prefer it over an infinite wait with
    // no dispatch target. Exclude common IME helper windows.
    EnumThreadWindows(threadId, [](HWND hwnd, LPARAM param) -> BOOL {
        auto* search = reinterpret_cast<FindWindowContext*>(param);
        DWORD pid{};
        if (GetWindowThreadProcessId(hwnd, &pid) != search->threadId ||
            pid != GetCurrentProcessId()) return TRUE;
        wchar_t cls[64]{};
        if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) return TRUE;
        if (wcscmp(cls, L"Default IME") == 0 ||
            wcscmp(cls, L"MSCTFIME UI") == 0 ||
            wcscmp(cls, L"IME") == 0) return TRUE;
        search->result = hwnd;
        return FALSE;
    }, reinterpret_cast<LPARAM>(&context));
    return context.result;
}

bool RunOnUiThread(DWORD threadId, bool (*callback)(DWORD)) {
    if (threadId == GetCurrentThreadId()) {
        try {
            return callback(threadId);
        } catch (...) {
            Wh_Log(L"Same-thread UI callback exception");
            return false;
        }
    }
    HWND hwnd = FindUiWindowForThread(threadId);
    if (!hwnd) return false;

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, DispatchHook, nullptr, threadId);
    if (!hook) return false;

    UiRequest request;
    request.hwnd = hwnd;
    request.threadId = threadId;
    request.callback = callback;
    {
        std::lock_guard<std::mutex> lock(g_dispatchMutex);
        request.next = g_activeRequests;
        g_activeRequests = &request;
    }
    // SendMessageW waits for the target UI callback and the hook chain.
    // Other dispatches can proceed while we are blocked here.
    SendMessageW(hwnd, g_dispatchMsg, 0, reinterpret_cast<LPARAM>(&request));
    {
        std::lock_guard<std::mutex> lock(g_dispatchMutex);
        auto** link = &g_activeRequests;
        while (*link && *link != &request) link = &(*link)->next;
        if (*link) *link = request.next;
    }
    UnhookDispatchSafely(hook, threadId);
    return request.invoked.load(std::memory_order_acquire) &&
           request.succeeded.load(std::memory_order_acquire);
}

void PruneExpiredEntriesOnCurrentThread() {
    DWORD threadId = GetCurrentThreadId();
    std::vector<BarEntry> entries;
    {
        std::lock_guard<std::mutex> lock(g_barsMutex);
        for (auto const& entry : g_bars)
            if (entry.ownerThread == threadId) entries.push_back(entry);
    }
    std::vector<uint64_t> expired;
    for (auto const& entry : entries) {
        try {
            if (!entry.button.get()) expired.push_back(entry.entryId);
        } catch (...) {
            // If the reference cannot be resolved, preserve the event record.
        }
    }
    if (expired.empty()) return;
    std::lock_guard<std::mutex> lock(g_barsMutex);
    g_bars.erase(std::remove_if(g_bars.begin(), g_bars.end(),
        [&expired](BarEntry const& entry) {
            return std::find(expired.begin(), expired.end(), entry.entryId) !=
                expired.end();
        }), g_bars.end());
}

std::vector<DWORD> KnownThreads() {
    std::vector<DWORD> result;
    {
        std::lock_guard<std::mutex> lock(g_barsMutex);
        for (auto const& entry : g_bars) {
            if (std::find(result.begin(), result.end(), entry.ownerThread) == result.end())
                result.push_back(entry.ownerThread);
        }
    }
    for (DWORD threadId : NavigationThreads()) {
        if (std::find(result.begin(), result.end(), threadId) == result.end())
            result.push_back(threadId);
    }
    return result;
}

bool SyncThreadButtons(DWORD threadId) {
    std::vector<BarEntry> entries;
    {
        std::lock_guard<std::mutex> lock(g_barsMutex);
        for (auto const& e : g_bars)
            if (e.ownerThread == threadId) entries.push_back(e);
    }
    bool success = true;
    for (auto const& entry : entries) {
        try {
            if (auto button = entry.button.get()) UpdateButton(button);
        } catch (...) {
            success = false;
        }
    }
    return success;
}

void SyncButtons() {
    for (auto threadId : KnownThreads()) {
        if (g_stopping.load(std::memory_order_acquire)) return;
        if (!RunOnUiThread(threadId, SyncThreadButtons))
            Wh_Log(L"Could not sync thread %lu", threadId);
    }
}

std::atomic<unsigned> g_cleanupFailures{0};

// Called only on the owner XAML thread. A detached Click handler is sufficient
// for safe unload. Visual removal is best-effort and never keeps code alive.
bool RemoveThreadButtonsAndEvents(DWORD threadId) {
    std::vector<BarEntry> entries;
    {
        std::lock_guard<std::mutex> lock(g_barsMutex);
        for (auto const& entry : g_bars)
            if (entry.ownerThread == threadId) entries.push_back(entry);
    }

    bool allDetached = true;
    for (auto const& entry : entries) {
        bool detached = false;
        muxc::AppBarButton button{nullptr};
        try {
            button = entry.button.get();
            if (!button) {
                detached = true; // A destroyed event source cannot invoke us.
            } else if (!entry.clickAttached) {
                detached = true;
            } else {
                button.Click(entry.clickToken);
                detached = true;
            }
        } catch (...) {
            ++g_cleanupFailures;
            Wh_Log(L"Click handler detach failed: 0x%08X", winrt::to_hresult());
        }
        if (!detached) {
            allDetached = false;
            continue; // Never unload while a live handler remains.
        }

        if (button) {
            try {
                if (auto bar = entry.bar.get()) {
                    if (auto commands = bar.PrimaryCommands()) {
                        for (uint32_t i = commands.Size(); i > 0; --i) {
                            auto item = commands.GetAt(i - 1).try_as<muxc::AppBarButton>();
                            if (item && item == button) {
                                commands.RemoveAt(i - 1);
                                break;
                            }
                        }
                    }
                }
            } catch (...) {
                ++g_cleanupFailures;
                Wh_Log(L"Button removal failed (event already detached): 0x%08X",
                       winrt::to_hresult());
            }
        }
        std::lock_guard<std::mutex> lock(g_barsMutex);
        auto it = std::find_if(g_bars.begin(), g_bars.end(),
            [&entry](BarEntry const& value) { return value.entryId == entry.entryId; });
        if (it != g_bars.end()) g_bars.erase(it);
    }
    // Navigation event callbacks also point into this mod, and must be
    // detached on the owning XAML thread before unloading the DLL.
    return RemoveThreadNavigationHooks(threadId) && allDetached;
}

void ForgetTerminatedThread(DWORD threadId) {
    {
        std::lock_guard<std::mutex> lock(g_barsMutex);
        g_bars.erase(std::remove_if(g_bars.begin(), g_bars.end(),
            [threadId](BarEntry const& entry) { return entry.ownerThread == threadId; }),
            g_bars.end());
    }
    ForgetNavigationThread(threadId);
}

bool FindTaskManagerRange() {
    auto* start = reinterpret_cast<BYTE*>(GetModuleHandleW(nullptr));
    if (!start) return false;
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(start);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return false;
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(start + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return false;
    g_taskBegin = reinterpret_cast<uintptr_t>(start);
    g_taskEnd = g_taskBegin + nt->OptionalHeader.SizeOfImage;
    return g_taskBegin < g_taskEnd;
}


// Called on an owning XAML thread after a Diagnostics notification.
void TryAttachFrameNavigation(mux::DependencyObject object);

// Called only on the owning XAML thread, from diagnostics notifications.
void FindCommandBars(mux::FrameworkElement const& page, bool supportedPage) {
    if (g_stopping.load(std::memory_order_acquire)) return;
    try {
        if (supportedPage) TryAttachFrameNavigation(page);
        mux::DependencyObject root = page;
        for (unsigned i = 0; i < 48; ++i) {
            auto parent = muxm::VisualTreeHelper::GetParent(root);
            if (!parent) break;
            root = parent;
        }
        std::vector<mux::DependencyObject> stack{root};
        unsigned scanned = 0;
        unsigned barsFound = 0;
        unsigned matchingBars = 0;
        std::vector<muxc::CommandBar> pageBars;
        while (!stack.empty() && scanned < 5000 &&
               !g_stopping.load(std::memory_order_acquire)) {
            auto element = stack.back();
            stack.pop_back();
            ++scanned;
            if (auto fe = element.try_as<mux::FrameworkElement>()) {
                if (fe.Visibility() != mux::Visibility::Visible) continue;
            }
            if (auto bar = element.try_as<muxc::CommandBar>()) {
                ++barsFound;
                pageBars.push_back(bar);
            }
            int children = muxm::VisualTreeHelper::GetChildrenCount(element);
            for (int i = children - 1; i >= 0; --i)
                stack.push_back(muxm::VisualTreeHelper::GetChild(element, i));
        }

        // A complete scan is essential for the structural fallback. If the
        // traversal was cut short, only the semantic matcher may be trusted.
        const bool uniquePageBar =
            supportedPage && stack.empty() && pageBars.size() == 1;
        for (auto const& bar : pageBars) {
            if (g_stopping.load(std::memory_order_acquire)) break;
            try {
                BarCandidate candidate;
                if (InspectProductionBar(bar, uniquePageBar, candidate)) {
                    ++matchingBars;
                    TryInsertButton(candidate.bar, candidate.place);
                }
            } catch (...) {
                Wh_Log(L"Could not inspect command bar: 0x%08X",
                       winrt::to_hresult());
            }
        }
        // Bounded diagnostics for distinguishing missing hooks, missing
        // toolbar anchors, and insertion errors without spamming the log.
        static std::atomic<unsigned> searchesLogged{0};
        if (searchesLogged.fetch_add(1, std::memory_order_relaxed) < 6) {
            Wh_Log(L"Command bar scan: elements=%u bars=%u matches=%u",
                   scanned, barsFound, matchingBars);
        }
    } catch (...) {
        Wh_Log(L"Visual tree search failed: 0x%08X", winrt::to_hresult());
    }
}

// Identify navigation destinations from public XAML type names rather than
// TaskManagerApp private symbols. Unknown page names don't reset Pause.
enum class ActivePage { Unknown, Supported, Other };

bool EndsWith(std::wstring_view value, std::wstring_view suffix) {
    return value.size() >= suffix.size() &&
           value.substr(value.size() - suffix.size()) == suffix;
}

ActivePage ClassifyPageType(winrt::hstring const& typeName) {
    std::wstring_view name{typeName.c_str(), typeName.size()};
    if (EndsWith(name, L"ProcessesPage") || EndsWith(name, L"DetailsPage"))
        return ActivePage::Supported;
    // Only treat positively identified Task Manager pages as evidence that
    // Pause should reset. Generic shell/container frames aren't evidence.
    constexpr std::wstring_view otherPages[] = {
        L"PerformancePage", L"AppHistoryPage", L"StartupAppsPage",
        L"StartupPage", L"UsersPage", L"ServicesPage", L"SettingsPage"
    };
    for (auto suffix : otherPages)
        if (EndsWith(name, suffix)) return ActivePage::Other;
    return ActivePage::Unknown;
}

// A public Frame.Navigated event is more reliable for leaving Processes or
// Details than simply observing page Add events. No private Taskmgr symbols.
struct NavigationEntry {
    winrt::weak_ref<muxc::Frame> frame;
    DWORD ownerThread{};
    winrt::event_token token{};
    uint64_t id{};
    bool attached{};
};
std::mutex g_navigationMutex;
[[clang::no_destroy]] std::vector<NavigationEntry> g_navigationEntries;
std::atomic<uint64_t> g_nextNavigationId{1};
std::atomic<unsigned> g_navigationEvents{0};

void ResumeOnUnsupportedPage() {
    if (g_stopping.load(std::memory_order_acquire)) return;
    if (g_paused.exchange(false, std::memory_order_acq_rel)) {
        Wh_Log(L"Pause cleared after navigating away from Processes/Details");
        SyncButtons();
    }
}

std::vector<DWORD> NavigationThreads() {
    std::vector<DWORD> threads;
    std::lock_guard<std::mutex> lock(g_navigationMutex);
    for (const auto& entry : g_navigationEntries) {
        if (std::find(threads.begin(), threads.end(), entry.ownerThread) == threads.end())
            threads.push_back(entry.ownerThread);
    }
    return threads;
}

void ForgetNavigationThread(DWORD threadId) {
    std::lock_guard<std::mutex> lock(g_navigationMutex);
    std::erase_if(g_navigationEntries, [threadId](const NavigationEntry& e) {
        return e.ownerThread == threadId;
    });
}

bool RemoveThreadNavigationHooks(DWORD threadId) {
    std::vector<NavigationEntry> entries;
    {
        std::lock_guard<std::mutex> lock(g_navigationMutex);
        for (const auto& entry : g_navigationEntries)
            if (entry.ownerThread == threadId) entries.push_back(entry);
    }
    bool allDetached = true;
    for (const auto& entry : entries) {
        bool detached = false;
        try {
            auto frame = entry.frame.get();
            if (!frame || !entry.attached) {
                detached = true;
            } else {
                frame.Navigated(entry.token);
                detached = true;
            }
        } catch (...) {
            ++g_cleanupFailures;
            Wh_Log(L"Frame.Navigated detach failed: 0x%08X", winrt::to_hresult());
        }
        if (!detached) {
            allDetached = false; // Live callback: retry, don't unload the DLL.
            continue;
        }
        std::lock_guard<std::mutex> lock(g_navigationMutex);
        std::erase_if(g_navigationEntries, [&entry](const NavigationEntry& e) {
            return e.id == entry.id;
        });
    }
    return allDetached;
}

void TryAttachFrameNavigation(mux::DependencyObject object) {
    if (!object || g_stopping.load(std::memory_order_acquire)) return;
    InsertionGuard guard; // Keep unload from racing with event subscription.
    if (!guard.active) return;
    try {
    for (unsigned depth = 0; object && depth < 64; ++depth) {
        if (auto frame = object.try_as<muxc::Frame>()) {
            const DWORD owner = GetCurrentThreadId();
            {
                std::lock_guard<std::mutex> lock(g_navigationMutex);
                for (const auto& entry : g_navigationEntries) {
                    if (entry.ownerThread == owner && entry.frame.get() == frame)
                        return;
                }
            }
            NavigationEntry entry;
            entry.frame = winrt::make_weak(frame);
            entry.ownerThread = owner;
            entry.id = g_nextNavigationId.fetch_add(1);
            const auto id = entry.id;
            winrt::event_token token{};
            bool attached = false;
            bool tracked = false;
            try {
                {
                    std::lock_guard<std::mutex> lock(g_navigationMutex);
                    g_navigationEntries.push_back(entry);
                }
                tracked = true;
                token = frame.Navigated([](auto const&,
                    mux::Navigation::NavigationEventArgs const& args) {
                    if (g_stopping.load(std::memory_order_acquire)) return;
                    try {
                        const ActivePage page = ClassifyPageType(args.SourcePageType().Name);
                        if (g_navigationEvents.fetch_add(1) < 16)
                            Wh_Log(L"Frame.Navigated: page=%d", static_cast<int>(page));
                        if (page == ActivePage::Other) ResumeOnUnsupportedPage();
                    } catch (...) {
                        Wh_Log(L"Frame navigation identification failed: 0x%08X",
                               winrt::to_hresult());
                    }
                });
                attached = true;
                {
                    std::lock_guard<std::mutex> lock(g_navigationMutex);
                    for (auto& saved : g_navigationEntries) {
                        if (saved.id == id) {
                            saved.token = token;
                            saved.attached = true;
                            break;
                        }
                    }
                }
                Wh_Log(L"Frame.Navigated listener attached on thread %lu", owner);
            } catch (...) {
                const HRESULT hr = winrt::to_hresult();
                bool safe = !attached;
                if (attached) {
                    try { frame.Navigated(token); safe = true; } catch (...) {}
                }
                if (tracked && safe) {
                    std::lock_guard<std::mutex> lock(g_navigationMutex);
                    std::erase_if(g_navigationEntries, [id](const NavigationEntry& e) {
                        return e.id == id;
                    });
                }
                Wh_Log(L"Frame.Navigated attachment failed: 0x%08X", hr);
            }
            return;
        }
        object = muxm::VisualTreeHelper::GetParent(object);
    }
    } catch (...) {
        // Navigation detection is optional: never prevent Pause insertion if
        // an ancestor cannot be inspected on this Windows build.
        static std::atomic<unsigned> errorsLogged{0};
        if (errorsLogged.fetch_add(1) < 4)
            Wh_Log(L"Frame navigation discovery unavailable: 0x%08X",
                   winrt::to_hresult());
    }
}

// Fallback for XAML layouts without Frame.Navigated: use a visible, attached
// page only. If a Frame exists, also verify that the page is its live Content;
// a preloaded page is not evidence of navigation.
void HandleOtherPageAddition(mux::DependencyObject const& object) {
    if (!g_paused.load(std::memory_order_acquire) || !object) return;
    try {
        auto page = object.try_as<mux::FrameworkElement>();
        if (!page || page.Visibility() != mux::Visibility::Visible) return;
        mux::DependencyObject current = page;
        bool attached = false;
        for (unsigned depth = 0; current && depth < 64; ++depth) {
            auto parent = muxm::VisualTreeHelper::GetParent(current);
            if (!parent) break;
            attached = true;
            if (auto frame = parent.try_as<muxc::Frame>()) {
                // Ignore a page that was created but isn't the active frame content.
                if (frame.Content().try_as<mux::FrameworkElement>() != page)
                    return;
                break;
            }
            current = parent;
        }
        if (attached) ResumeOnUnsupportedPage();
    } catch (...) {
        // A detached or temporarily unavailable page isn't navigation proof.
    }
}

// The diagnostics callback receives UI objects in the target XAML visual
// tree. Unlike Window::Current, it can work with XAML Islands. It is also
// scoped to Taskmgr.exe, so it never scans other processes.
using InitializeXamlDiagnosticsEx_t = decltype(&InitializeXamlDiagnosticsEx);

class PauseVisualTreeWatcher;
std::mutex g_diagnosticWatchersMutex;
// Explicitly clear COM references while XAML is still alive. Do not let
// global destructors run at DLL/process teardown.
[[clang::no_destroy]] std::vector<winrt::com_ptr<PauseVisualTreeWatcher>> g_diagnosticWatchersList;
std::mutex g_diagnosticThreadsMutex;
std::vector<HANDLE> g_diagnosticAdviseThreads;

bool IsSupportedPageAncestor(mux::DependencyObject start) {
    for (unsigned depth = 0; start && depth < 64; ++depth) {
        try {
            if (auto fe = start.try_as<mux::FrameworkElement>()) {
                if (fe.Visibility() != mux::Visibility::Visible) return false;
                if (ClassifyPageType(winrt::get_class_name(fe)) ==
                    ActivePage::Supported) return true;
            }
            start = muxm::VisualTreeHelper::GetParent(start);
        } catch (...) {
            return false;
        }
    }
    return false;
}

// This path avoids a whole-tree search on every incoming UI element.
// A CommandBar is inspected only when XAML Diagnostics reports a relevant
// native control addition; re-entrant additions of our own button are ignored.
void TryProcessDiagnosticElement(mux::DependencyObject element) {
    if (!element || g_stopping.load(std::memory_order_acquire)) return;
    try {
        muxc::CommandBar bar{nullptr};
        auto current = element;
        for (unsigned depth = 0; current && depth < 48; ++depth) {
            if (auto candidate = current.try_as<muxc::CommandBar>()) {
                bar = candidate;
                break;
            }
            current = muxm::VisualTreeHelper::GetParent(current);
        }
        if (!bar) return;
        TryAttachFrameNavigation(bar);

        // Explicit role matching is safe on unknown pages. A four-button
        // structural fallback is used only after verifying the page type.
        BarCandidate candidate;
        const bool supportedPage = IsSupportedPageAncestor(bar);
        if (!InspectProductionBar(bar, supportedPage, candidate)) return;
        if (g_diagnosticPages.fetch_add(1, std::memory_order_relaxed) < 8)
            Wh_Log(L"XAML Diagnostics: matching Task Manager command bar, thread=%lu supported=%d",
                   GetCurrentThreadId(), supportedPage);
        PruneExpiredEntriesOnCurrentThread();
        TryInsertButton(candidate.bar, candidate.place);
    } catch (...) {
        if (g_diagnosticErrors.fetch_add(1, std::memory_order_relaxed) < 12)
            Wh_Log(L"XAML Diagnostics: command bar inspection failed: 0x%08X",
                   winrt::to_hresult());
    }
}

// Wait for every in-flight Diagnostics callback before DLL unload. The stop
// flag prevents new callbacks from entering this mod's XAML work.
std::mutex g_diagnosticCallbackMutex;
std::condition_variable g_diagnosticCallbackDone;
unsigned g_activeDiagnosticCallbacks = 0;
struct DiagnosticCallbackGuard {
    bool active = false;
    DiagnosticCallbackGuard() {
        std::lock_guard<std::mutex> lock(g_diagnosticCallbackMutex);
        if (!g_diagnosticsStop.load(std::memory_order_acquire) &&
            !g_stopping.load(std::memory_order_acquire)) {
            ++g_activeDiagnosticCallbacks;
            active = true;
        }
    }
    ~DiagnosticCallbackGuard() {
        if (!active) return;
        std::lock_guard<std::mutex> lock(g_diagnosticCallbackMutex);
        if (--g_activeDiagnosticCallbacks == 0) g_diagnosticCallbackDone.notify_all();
    }
};

class PauseVisualTreeWatcher
    : public winrt::implements<PauseVisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
public:
    explicit PauseVisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : m_diagnostics(site.as<IXamlDiagnostics>()) {
        // AdviseVisualTreeChange may synchronously call this object. Never
        // invoke it inside SetSite: perform subscription on a separate thread.
        // Start and register the thread under the SAME lock used by Stop.
        // Reserve a registry slot first: allocation failure cannot leave an
        // untracked thread executing mod code after unload.
        std::lock_guard<std::mutex> lock(g_diagnosticThreadsMutex);
        if (g_diagnosticsStop.load(std::memory_order_acquire))
            winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_CANCELLED));
        g_diagnosticAdviseThreads.push_back(nullptr);
        AddRef();
        HANDLE thread = CreateThread(nullptr, 0, [](LPVOID raw) -> DWORD {
            auto* self = static_cast<PauseVisualTreeWatcher*>(raw);
            HRESULT hr = E_FAIL;
            try {
                hr = self->m_diagnostics.as<IVisualTreeService3>()
                         ->AdviseVisualTreeChange(self);
            } catch (...) {
                hr = winrt::to_hresult();
            }
            if (SUCCEEDED(hr)) {
                self->m_advised.store(true, std::memory_order_release);
                Wh_Log(L"XAML Diagnostics: visual-tree watcher subscribed");
                if (self->m_unadviseRequested.load(std::memory_order_acquire))
                    self->Unadvise();
            } else if (!g_diagnosticsStop.load(std::memory_order_acquire)) {
                Wh_Log(L"XAML Diagnostics: AdviseVisualTreeChange failed: 0x%08X", hr);
            }
            self->Release();
            return 0;
        }, this, 0, nullptr);
        if (!thread) {
            const DWORD error = GetLastError();
            g_diagnosticAdviseThreads.pop_back();
            Release();
            winrt::throw_hresult(HRESULT_FROM_WIN32(error));
        }
        g_diagnosticAdviseThreads.back() = thread;
    }

    void Unadvise() noexcept {
        m_unadviseRequested.store(true, std::memory_order_release);
        if (!m_advised.exchange(false, std::memory_order_acq_rel)) return;
        try {
            HRESULT hr = m_diagnostics.as<IVisualTreeService3>()
                             ->UnadviseVisualTreeChange(this);
            if (FAILED(hr))
                Wh_Log(L"XAML Diagnostics: UnadviseVisualTreeChange failed: 0x%08X", hr);
        } catch (...) {
            Wh_Log(L"XAML Diagnostics: exception during unadvise");
        }
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation, VisualElement element,
        VisualMutationType mutationType) override try {
        if (mutationType != Add) return S_OK;
        DiagnosticCallbackGuard callback;
        if (!callback.active) return S_OK;
        const std::wstring_view type{element.Type ? element.Type : L""};
        const bool isButton = type.find(L"AppBarButton") != std::wstring_view::npos;
        const bool isBar = type.find(L"CommandBar") != std::wstring_view::npos;
        const ActivePage pageKind = ClassifyPageType(
            winrt::hstring{element.Type ? element.Type : L""});
        const bool isPage = pageKind != ActivePage::Unknown;
        if (!isButton && !isBar && !isPage) return S_OK;
        if (g_diagnosticCandidates.fetch_add(1, std::memory_order_relaxed) < 14)
            Wh_Log(L"XAML Diagnostics: candidate type=%s thread=%lu",
                   element.Type ? element.Type : L"", GetCurrentThreadId());
        wf::IInspectable inspectable{nullptr};
        const HRESULT hr = m_diagnostics->GetIInspectableFromHandle(
            element.Handle,
            reinterpret_cast<::IInspectable**>(winrt::put_abi(inspectable)));
        if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            // XAML element additions and removals can race with diagnostics
            // handle lookup. A removed element isn't an initialization error.
            g_diagnosticExpiredHandles.fetch_add(1, std::memory_order_relaxed);
            return S_OK;
        }
        if (FAILED(hr)) {
            if (g_diagnosticErrors.fetch_add(1, std::memory_order_relaxed) < 12)
                Wh_Log(L"XAML Diagnostics: GetIInspectableFromHandle failed 0x%08X", hr);
            return S_OK;
        }
        if (auto object = inspectable.try_as<mux::DependencyObject>()) {
            if (isPage) {
                if (auto page = object.try_as<mux::FrameworkElement>()) {
                    TryAttachFrameNavigation(page);
                    if (pageKind == ActivePage::Supported) {
                        // A page notification may arrive after the bar exists.
                        FindCommandBars(page, true);
                    } else {
                        HandleOtherPageAddition(page);
                    }
                }
            } else {
                TryProcessDiagnosticElement(object);
            }
        }
        return S_OK;
    } catch (...) {
        if (g_diagnosticErrors.fetch_add(1, std::memory_order_relaxed) < 12)
            Wh_Log(L"XAML Diagnostics: visual-tree callback error 0x%08X",
                   winrt::to_hresult());
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(
        InstanceHandle, VisualElementState, LPCWSTR) noexcept override {
        return S_OK;
    }

private:
    winrt::com_ptr<IXamlDiagnostics> m_diagnostics;
    std::atomic<bool> m_advised{false};
    std::atomic<bool> m_unadviseRequested{false};
};

void RemoveDiagnosticWatcher(PauseVisualTreeWatcher* watcher) {
    std::lock_guard<std::mutex> lock(g_diagnosticWatchersMutex);
    auto& active = g_diagnosticWatchersList;
    active.erase(std::remove_if(active.begin(), active.end(),
        [watcher](auto const& candidate) { return candidate.get() == watcher; }),
        active.end());
}

HMODULE CurrentModModule() {
    HMODULE module{};
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&CurrentModModule), &module)) {
        return nullptr;
    }
    return module;
}

class PauseXamlTap
    : public winrt::implements<PauseXamlTap, IObjectWithSite, winrt::non_agile> {
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) override try {
        if (m_watcher) {
            m_watcher->Unadvise();
            RemoveDiagnosticWatcher(m_watcher.get());
            m_watcher = nullptr;
        }
        m_site.copy_from(site);
        if (!site) return S_OK;

        // The XAML framework holds a separate LoadLibrary reference when it
        // loads this class. Balance that reference as in other Windhawk mods.
        if (HMODULE module = CurrentModModule()) FreeLibrary(module);
        if (g_diagnosticsStop.load(std::memory_order_acquire)) return S_OK;

        auto watcher = winrt::make_self<PauseVisualTreeWatcher>(m_site);
        {
            std::lock_guard<std::mutex> lock(g_diagnosticWatchersMutex);
            if (g_diagnosticsStop.load(std::memory_order_relaxed)) {
                watcher->Unadvise();
                return S_OK;
            }
            g_diagnosticWatchersList.push_back(watcher);
        }
        m_watcher = std::move(watcher);
        g_diagnosticWatchers.fetch_add(1, std::memory_order_release);
        Wh_Log(L"XAML Diagnostics: site received; watcher created");
        return S_OK;
    } catch (...) {
        Wh_Log(L"XAML Diagnostics: SetSite failed: 0x%08X", winrt::to_hresult());
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid, void** object) noexcept override {
        return m_site.as(iid, object);
    }
private:
    winrt::com_ptr<IUnknown> m_site;
    winrt::com_ptr<PauseVisualTreeWatcher> m_watcher;
};

template <typename T>
struct PauseClassFactory
    : winrt::implements<PauseClassFactory<T>, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID iid,
                                             void** object) override try {
        if (outer) return CLASS_E_NOAGGREGATION;
        *object = nullptr;
        return winrt::make<T>().as(iid, object);
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override { return S_OK; }
};

// {B061FA10-A60B-46C9-9382-99C2AEAA5A01}
constexpr CLSID CLSID_PauseXamlTap = {
    0xb061fa10, 0xa60b, 0x46c9,
    {0x93, 0x82, 0x99, 0xc2, 0xae, 0xaa, 0x5a, 0x01}};

} // namespace

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
__declspec(dllexport) _Use_decl_annotations_
STDAPI DllGetClassObject(REFCLSID clsid, REFIID iid, void** object) try {
    if (clsid != CLSID_PauseXamlTap) return CLASS_E_CLASSNOTAVAILABLE;
    *object = nullptr;
    return winrt::make<PauseClassFactory<PauseXamlTap>>().as(iid, object);
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport) _Use_decl_annotations_
STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}
#pragma clang diagnostic pop

namespace {

HRESULT AttachToXamlDiagnostics(HMODULE xamlModule) {
    const auto initialize = reinterpret_cast<InitializeXamlDiagnosticsEx_t>(
        GetProcAddress(xamlModule, "InitializeXamlDiagnosticsEx"));
    if (!initialize) return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    const HMODULE module = CurrentModModule();
    if (!module) return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
    wchar_t modulePath[MAX_PATH]{};
    const DWORD len = GetModuleFileNameW(module, modulePath, ARRAYSIZE(modulePath));
    if (!len || len >= ARRAYSIZE(modulePath)) return E_FAIL;

    // Windows.UI.Xaml uses the VisualDiagConnectionN endpoints. The framework
    // ordinal is process-lifetime based, so don't assume endpoint number 1.
    for (int index = 1; index <= 10000; ++index) {
        if (g_diagnosticsStop.load(std::memory_order_acquire))
            return HRESULT_FROM_WIN32(ERROR_CANCELLED);
        wchar_t connectionName[80]{};
        _snwprintf_s(connectionName, _TRUNCATE,
                     L"VisualDiagConnection%d", index);
        const HRESULT hr = initialize(connectionName, GetCurrentProcessId(),
                                      L"", modulePath, CLSID_PauseXamlTap, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) return hr;
    }
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

DWORD WINAPI XamlDiagnosticsWorker(LPVOID) {
    const HRESULT com = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    unsigned failures = 0;
    unsigned emptyWalks = 0;
    bool moduleSeen = false;
    for (unsigned attempt = 0; attempt < 30; ++attempt) {
        if (g_diagnosticsStop.load(std::memory_order_acquire)) break;
        HMODULE xamlModule = GetModuleHandleW(L"Windows.UI.Xaml.dll");
        if (xamlModule) {
            moduleSeen = true;
            const unsigned previousWatchers =
                g_diagnosticWatchers.load(std::memory_order_acquire);
            const HRESULT hr = AttachToXamlDiagnostics(xamlModule);
            if (SUCCEEDED(hr)) {
                // SetSite may complete asynchronously after initialization
                // returns. Observe the actual watcher rather than reporting
                // a spurious competing-consumer warning immediately.
                // Poll briefly on the background worker, never on the UI
                // thread. The existing stop event interrupts the wait safely.
                for (unsigned i = 0; i < 30 &&
                     g_diagnosticWatchers.load(std::memory_order_acquire) ==
                         previousWatchers &&
                     !g_diagnosticsStop.load(std::memory_order_acquire); ++i) {
                    if (WaitForSingleObject(g_diagnosticsStopEvent, 25) !=
                        WAIT_TIMEOUT) break;
                }
                if (g_diagnosticsStop.load(std::memory_order_acquire))
                    break;
                if (g_diagnosticWatchers.load(std::memory_order_acquire) >
                    previousWatchers) {
                    Wh_Log(L"XAML Diagnostics: connected to Windows.UI.Xaml");
                } else {
                    Wh_Log(L"XAML Diagnostics: no watcher observed within 750 ms (handoff delayed or another consumer)");
                }
                break;
            }
            if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
                if (++emptyWalks >= 3) {
                    Wh_Log(L"XAML Diagnostics: no VisualDiagConnection endpoint after 3 attempts");
                    break;
                }
            } else if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
                Wh_Log(L"XAML Diagnostics: initialization failed: 0x%08X", hr);
                if (++failures >= 3) break;
            }
        }
        if (g_diagnosticsStopEvent &&
            WaitForSingleObject(g_diagnosticsStopEvent, 1000) == WAIT_OBJECT_0)
            break;
    }
    if (!moduleSeen && !g_diagnosticsStop.load(std::memory_order_acquire))
        Wh_Log(L"XAML Diagnostics: Windows.UI.Xaml.dll not found; no compatible XAML source");
    if (SUCCEEDED(com)) CoUninitialize();
    return 0;
}

void StopDiagnosticsWorkers() {
    // The same lock is held while a watcher thread is created and registered.
    // After closing admission, a snapshot cannot miss a newly started thread.
    {
        std::lock_guard<std::mutex> lock(g_diagnosticThreadsMutex);
        g_diagnosticsStop.store(true, std::memory_order_release);
    }
    if (g_diagnosticsStopEvent) SetEvent(g_diagnosticsStopEvent);
    if (g_diagnosticsWorker) {
        WaitForSingleObject(g_diagnosticsWorker, INFINITE);
        CloseHandle(g_diagnosticsWorker);
        g_diagnosticsWorker = nullptr;
    }
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_diagnosticThreadsMutex);
        threads.swap(g_diagnosticAdviseThreads);
    }
    for (HANDLE thread : threads) {
        if (!thread) continue;
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

void UnadviseDiagnostics() {
    std::vector<winrt::com_ptr<PauseVisualTreeWatcher>> watchers;
    {
        std::lock_guard<std::mutex> lock(g_diagnosticWatchersMutex);
        watchers.swap(g_diagnosticWatchersList);
    }
    for (auto const& watcher : watchers) watcher->Unadvise();
    // Unadvising prevents future callbacks. Wait for any already inside
    // OnVisualTreeChange before releasing watchers or UI event handlers.
    {
        std::unique_lock<std::mutex> lock(g_diagnosticCallbackMutex);
        g_diagnosticCallbackDone.wait(lock, [] {
            return g_activeDiagnosticCallbacks == 0;
        });
    }
    watchers.clear();
    if (g_diagnosticsStopEvent) {
        CloseHandle(g_diagnosticsStopEvent);
        g_diagnosticsStopEvent = nullptr;
    }
}


} // namespace

BOOL Wh_ModInit() {
    {
        std::lock_guard<std::mutex> lock(g_insertionMutex);
        g_acceptingInsertions = true;
        g_insertionCount = 0;
    }
    g_stopping = false;
    g_diagnosticsStop = false;
    g_diagnosticCandidates = 0;
    g_diagnosticErrors = 0;
    g_diagnosticPages = 0;
    g_diagnosticWatchers = 0;
    g_diagnosticExpiredHandles = 0;
    g_navigationEvents = 0;
    g_paused = false;
    g_insertions = 0;
    g_cleanupFailures = 0;
    g_dispatchMsg = RegisterWindowMessageW(
        L"Windhawk.TaskManagerPauseButton.SynchronousUI");
    if (!g_dispatchMsg || !FindTaskManagerRange()) return FALSE;
    if (!WindhawkUtils::SetFunctionHook(
            GetKeyState, GetKeyState_Hook, &g_getKeyOriginal)) {
        Wh_Log(L"Required GetKeyState hook failed; no UI was changed");
        return FALSE;
    }
    g_diagnosticsStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_diagnosticsStopEvent) return FALSE;

    Wh_Log(L"Ready: XAML Diagnostics discovery (experimental; no private Taskmgr symbols)");
    return TRUE;
}

void Wh_ModAfterInit() {
    std::lock_guard<std::mutex> lock(g_diagnosticThreadsMutex);
    if (g_diagnosticsStop.load(std::memory_order_acquire)) return;
    g_diagnosticsWorker = CreateThread(nullptr, 0, XamlDiagnosticsWorker,
                                      nullptr, 0, nullptr);
    if (!g_diagnosticsWorker)
        Wh_Log(L"XAML Diagnostics: couldn't start worker: %lu", GetLastError());
}

void Wh_ModBeforeUninit() {
    StopNewInsertions();
    StopDiagnosticsWorkers();
}

void Wh_ModUninit() {
    StopNewInsertions();
    StopDiagnosticsWorkers();
    UnadviseDiagnostics();
    {
        std::unique_lock<std::mutex> lock(g_insertionMutex);
        g_insertionDone.wait(lock, [] { return g_insertionCount == 0; });
    }

    // Admission is closed and every started insertion has finished, so
    // KnownThreads can no longer miss a late first Click subscription.
    unsigned terminatedThreads = 0;
    unsigned attempts = 0;
    for (;;) {
        const auto threads = KnownThreads();
        if (threads.empty()) break;
        for (DWORD threadId : threads) {
            if (UiThreadTerminated(threadId)) {
                ForgetTerminatedThread(threadId);
                ++terminatedThreads;
                continue;
            }
            if (!RunOnUiThread(threadId, RemoveThreadButtonsAndEvents)) {
                if (++attempts % 4 == 0) {
                    Wh_Log(L"Awaiting safe UI cleanup: thread=%lu attempts=%u; close a hung Task Manager",
                           threadId, attempts);
                }
            }
        }
        if (KnownThreads().empty()) break;
        // Never unload a module while a live XAML button still has a Click
        // handler pointing into it. A dead or inaccessible UI thread may
        // require the user to close Task Manager; no finite wait is safe.
        Sleep(250);
    }
    Wh_Log(L"XAML Diagnostics summary: expiredHandlesIgnored=%u diagnosticErrors=%u",
           g_diagnosticExpiredHandles.load(), g_diagnosticErrors.load());
    Wh_Log(L"Uninit finished: inserted=%u terminatedThreads=%u cleanupErrors=%u",
           g_insertions.load(), terminatedThreads, g_cleanupFailures.load());
}
