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

- Uses Task Manager's Processes and Details navigation symbols and public
  XAML APIs. Windhawk resolves symbols through its normal symbol cache.
  When both pages share the same navigation function, the mod hooks it only
  once. Windows updates may change private functions.
- Uses Task Manager's Ctrl-key refresh hold through `GetKeyState` calls made
  directly by `Taskmgr.exe`. While paused, some Task Manager keyboard actions
  may behave as if Ctrl is held, including Run new task (Ctrl+click can open
  Command Prompt instead of the normal dialog). This does not affect other apps.
- Recognizes the tested Task Manager command-bar layout independently of
  display language: one command bar with four native buttons, with Run new
  task in the first position. Falls back to English/Portuguese labels or
  recognizable control identifiers on other layouts. Visual-tree traversal
  skips unrelated Page subtrees; shared shell command bars remain eligible.
  Other Windows versions can have different layouts, so all languages/builds
  are not guaranteed.
- The native button inherits Task Manager's XAML theme, including its own
  Light/Dark appearance; no fixed foreground/background colors are applied.
  Pause/Resume labels are in English regardless of the Task Manager language.
- Keeps Pause active when switching between Processes and Details. Uses the
  public `Frame.Navigated` event to resume on other pages and to retry the
  command-bar search if the toolbar was not ready, without repeating successful
  scans. If a Windows build lacks the optional Details navigation symbol,
  visiting Processes first enables the Frame fallback.
- If the button is missing, close and reopen Task Manager with the mod enabled.
- No background applications are needed.

## Source and feedback

[GitHub](https://github.com/luizbizzio/task-manager-pause-button)

Author: Luiz Bizzio. License: Apache-2.0.
*/
// ==/WindhawkModReadme==

// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Luiz Bizzio

#include <windows.h>
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
    // Only true after the complete insertion sequence succeeds. A callback
    // retained after a failed rollback is tracked but is not a usable button.
    bool usable{};
};

std::mutex g_barsMutex;
std::vector<BarEntry> g_bars;
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
void TryAttachFrameNavigation(mux::DependencyObject object);
bool NeedsFrameNavigationScan(winrt::hstring const& pageType,
                              mux::FrameworkElement const& page);
void ForgetHookScanThread(DWORD threadId);

void UpdateButton(muxc::AppBarButton const& button) {
    const bool paused = g_paused.load(std::memory_order_relaxed);
    const wchar_t* caption = paused ? L"Resume" : L"Pause";
    if (button.Label() == caption) return;

    // The label and Click handler must remain usable even if creating or
    // assigning the native SymbolIcon fails on this Task Manager build.
    button.Label(caption);
    try {
        button.Icon(muxc::SymbolIcon(paused ? muxc::Symbol::Play : muxc::Symbol::Pause));
    } catch (...) {
        const HRESULT iconError = winrt::to_hresult();
        // Avoid showing a stale Play/Pause icon from an earlier state.
        try {
            button.Icon(nullptr);
        } catch (...) {
            // The text label remains correct if clearing the icon also fails.
        }
        Wh_Log(L"Pause/Resume icon unavailable (HRESULT=0x%08X); text label kept",
               static_cast<unsigned int>(iconError));
    }
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

// Distinguish a usable button from an incomplete insertion whose Click
// callback could not be revoked. The latter must remain tracked for safe
// unloading, but must NOT count as successful insertion.
enum class InsertStatus { Failed, Ready, RetainedCallback };

InsertStatus TryInsertButton(muxc::CommandBar const& bar, uint32_t place) {
    if (!bar || g_addingButton) return InsertStatus::Failed;
    InsertionGuard insertion;
    if (!insertion.active) return InsertStatus::Failed;
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
        if (!commands || place > commands.Size()) return InsertStatus::Failed;

        // Before examining visible commands, avoid inserting another button
        // if a previous rollback left a live Click callback on this bar.
        // Such an entry must be released by the unload path, not overwritten.
        std::vector<BarEntry> trackedEntries;
        {
            std::lock_guard<std::mutex> lock(g_barsMutex);
            for (const auto& entry : g_bars) {
                if (entry.ownerThread == GetCurrentThreadId() &&
                    entry.clickAttached)
                    trackedEntries.push_back(entry);
            }
        }
        for (const auto& entry : trackedEntries) {
            if (!entry.usable && entry.bar.get() == bar && entry.button.get()) {
                Wh_Log(L"Previous insertion has a retained Click callback; waiting for safe cleanup");
                return InsertStatus::RetainedCallback;
            }
        }

        // On a previous unload a button can remain visible after its Click
        // handler was successfully detached but XAML RemoveAt failed.
        // Remove that inert control before adding the new instance.
        for (uint32_t i = commands.Size(); i > 0; --i) {
            auto item = commands.GetAt(i - 1).try_as<muxc::AppBarButton>();
            if (!item || item.Name() != kButtonName) continue;
            for (const auto& entry : trackedEntries) {
                if (entry.button.get() == item)
                    return entry.usable ? InsertStatus::Ready :
                                          InsertStatus::RetainedCallback;
            }
            commands.RemoveAt(i - 1);
            if (i - 1 < place) --place;
            Wh_Log(L"Removed detached button left by an earlier unload");
        }

        button = muxc::AppBarButton();
        button.Name(kButtonName);
        // Keep the default native AppBarButton style. In particular, do not
        // set RequestedTheme, Foreground, Background, or a fixed Style: the
        // XAML tree inherits Task Manager's own Light/Dark/contrast resources.
        // The SymbolIcon inherits its brush from this native button as well.
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
                    saved.usable = true;
                    break;
                }
            }
        }

        const auto count = g_insertions.fetch_add(1) + 1;
        // Diagnostics must not turn a completed Click subscription into a
        // failed insertion. ActualTheme is optional and may throw.
        if (count <= 8) {
            try {
                const auto theme = button.ActualTheme();
                Wh_Log(L"Native Pause button inserted (%u, theme=%s)", count,
                       theme == mux::ElementTheme::Light ? L"light" :
                       theme == mux::ElementTheme::Dark ? L"dark" : L"default");
            } catch (...) {
                Wh_Log(L"Native Pause button inserted (%u, theme unavailable)", count);
            }
        }
        return InsertStatus::Ready;
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
        // If Click revocation failed, its callback may remain live. Keep a
        // non-usable entry with its revocation token for safe unload. Never
        // present this exceptional state as a successful button insertion.
        if (!safeToForget && clickAttached && inserted && tracked) {
            try {
                std::lock_guard<std::mutex> lock(g_barsMutex);
                for (auto& entry : g_bars) {
                    if (entry.entryId == entryId) {
                        entry.clickToken = clickToken;
                        entry.clickAttached = true;
                        entry.usable = false;
                        break;
                    }
                }
            } catch (...) {
                Wh_Log(L"WARNING: could not refresh retained callback state");
            }
            Wh_Log(L"Button insertion incomplete: 0x%08X; Click callback retained",
                   error);
            return InsertStatus::RetainedCallback;
        }
        Wh_Log(L"Button insertion failed: 0x%08X", error);
        return InsertStatus::Failed;
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
    // Do not substitute a naive SendMessageTimeoutW here: the stack-backed
    // UiRequest and module callback could remain in flight after timeout,
    // allowing use-after-free or unload-while-running. Safe cancellation needs
    // a separate lifetime protocol. Other dispatches can proceed meanwhile.
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
        for (const auto& entry : g_bars) {
            if (std::find(result.begin(), result.end(), entry.ownerThread) == result.end())
                result.push_back(entry.ownerThread);
        }
    }
    for (DWORD id : NavigationThreads()) {
        if (std::find(result.begin(), result.end(), id) == result.end())
            result.push_back(id);
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
    // The navigation listener is also a callback into this mod. Detach it
    // on its owning XAML thread before the DLL is unloaded.
    const bool navigationDetached = RemoveThreadNavigationHooks(threadId);
    if (navigationDetached && allDetached) ForgetHookScanThread(threadId);
    return navigationDetached && allDetached;
}

void ForgetTerminatedThread(DWORD threadId) {
    {
        std::lock_guard<std::mutex> lock(g_barsMutex);
        g_bars.erase(std::remove_if(g_bars.begin(), g_bars.end(),
            [threadId](BarEntry const& entry) { return entry.ownerThread == threadId; }),
            g_bars.end());
    }
    ForgetNavigationThread(threadId);
    ForgetHookScanThread(threadId);
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


// Called on the XAML thread during Processes/Details page navigation.
// Returns true only after all matched bars have usable Pause buttons.
// Frame.Navigated may retry when the entry hook ran before the toolbar was
// created, or when the first insertion attempt failed.
bool FindCommandBars(mux::FrameworkElement const& page) {
    if (g_stopping.load(std::memory_order_acquire) || !page) return false;
    try {
        // Public XAML event, subscribed only on a page/navigation frame
        // obtained through the normal Task Manager symbol hook.
        TryAttachFrameNavigation(page);
        mux::DependencyObject root = page;
        std::vector<mux::DependencyObject> pagePath{root};
        for (unsigned i = 0; i < 48; ++i) {
            auto parent = muxm::VisualTreeHelper::GetParent(root);
            if (!parent) break;
            root = parent;
            pagePath.push_back(root);
        }
        std::vector<mux::DependencyObject> stack{root};
        unsigned scanned = 0;
        unsigned barsFound = 0;
        unsigned matchingBars = 0;
        unsigned buttonsReady = 0;
        unsigned retainedCallbacks = 0;
        unsigned skippedOtherPages = 0;
        std::vector<muxc::CommandBar> pageBars;
        while (!stack.empty() && scanned < 5000 &&
               !g_stopping.load(std::memory_order_acquire)) {
            auto element = stack.back();
            stack.pop_back();
            ++scanned;
            // A shell-level command bar can be a sibling of the active page,
            // so we must search the common root. Exclude *other* Page subtrees
            // (including cached/navigated-away pages) to avoid matching their
            // command bars. Preserve all ancestors of the active page.
            if (element.try_as<muxc::Page>() &&
                std::find(pagePath.begin(), pagePath.end(), element) == pagePath.end()) {
                ++skippedOtherPages;
                continue;
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
        const bool uniquePageBar = stack.empty() && pageBars.size() == 1;
        for (auto const& bar : pageBars) {
            if (g_stopping.load(std::memory_order_acquire)) break;
            try {
                TryAttachFrameNavigation(bar);
                BarCandidate candidate;
                if (InspectProductionBar(bar, uniquePageBar, candidate)) {
                    ++matchingBars;
                    const auto result = TryInsertButton(candidate.bar,
                                                        candidate.place);
                    if (result == InsertStatus::Ready)
                        ++buttonsReady;
                    else if (result == InsertStatus::RetainedCallback)
                        ++retainedCallbacks;
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
            Wh_Log(L"Command bar scan: elements=%u bars=%u matches=%u ready=%u retained=%u otherPages=%u",
                   scanned, barsFound, matchingBars, buttonsReady,
                   retainedCallbacks, skippedOtherPages);
        }
        return matchingBars != 0 && buttonsReady == matchingBars;
    } catch (...) {
        Wh_Log(L"Visual tree search failed: 0x%08X", winrt::to_hresult());
        return false;
    }
}

// Frame.Navigated is a public XAML event. We acquire the Frame through the
// Processes navigation hook, not through XAML Diagnostics.
// Unknown page types do not cause an automatic Resume.
enum class ActivePage { Unknown, Supported, Other };

bool EndsWith(std::wstring_view value, std::wstring_view suffix) {
    return value.size() >= suffix.size() &&
           value.substr(value.size() - suffix.size()) == suffix;
}

ActivePage ClassifyPageType(winrt::hstring const& fullName) {
    const std::wstring_view name{fullName.c_str(), fullName.size()};
    if (EndsWith(name, L"ProcessesPage") || EndsWith(name, L"DetailsPage"))
        return ActivePage::Supported;

    // Limit automatic Resume to positively identified Task Manager pages.
    // Avoid treating arbitrary XAML container navigation as leaving Task Manager.
    if (name.find(L"TaskManagerApp.") != std::wstring_view::npos &&
        EndsWith(name, L"Page"))
        return ActivePage::Other;
    return ActivePage::Unknown;
}

struct NavigationEntry {
    winrt::weak_ref<muxc::Frame> frame;
    DWORD ownerThread{};
    winrt::event_token token{};
    uint64_t id{};
    bool attached{};
};

std::mutex g_navigationMutex;
std::vector<NavigationEntry> g_navigationEntries; // Weak references only.
std::atomic<uint64_t> g_nextNavigationId{1};
std::atomic<unsigned> g_navigationEvents{0};

void ResumeOnUnsupportedPage() {
    if (g_stopping.load(std::memory_order_acquire)) return;
    if (g_paused.exchange(false, std::memory_order_acq_rel)) {
        Wh_Log(L"Pause cleared after navigating to another Task Manager page");
        SyncButtons();
    }
}

std::vector<DWORD> NavigationThreads() {
    std::vector<DWORD> result;
    std::lock_guard<std::mutex> lock(g_navigationMutex);
    for (const auto& entry : g_navigationEntries)
        if (std::find(result.begin(), result.end(), entry.ownerThread) == result.end())
            result.push_back(entry.ownerThread);
    return result;
}

void ForgetNavigationThread(DWORD threadId) {
    std::lock_guard<std::mutex> lock(g_navigationMutex);
    g_navigationEntries.erase(std::remove_if(g_navigationEntries.begin(),
        g_navigationEntries.end(), [threadId](const NavigationEntry& entry) {
            return entry.ownerThread == threadId;
        }), g_navigationEntries.end());
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
            allDetached = false;
            continue; // Never unload with a live callback still attached.
        }
        std::lock_guard<std::mutex> lock(g_navigationMutex);
        g_navigationEntries.erase(std::remove_if(g_navigationEntries.begin(),
            g_navigationEntries.end(), [&entry](const NavigationEntry& value) {
                return value.id == entry.id;
            }), g_navigationEntries.end());
    }
    return allDetached;
}

void TryAttachFrameNavigation(mux::DependencyObject object) {
    if (!object || g_stopping.load(std::memory_order_acquire)) return;
    InsertionGuard guard; // Wait for event subscriptions during unload.
    if (!guard.active) return;
    try {
        for (unsigned depth = 0; object && depth < 64; ++depth) {
            if (auto frame = object.try_as<muxc::Frame>()) {
                const DWORD owner = GetCurrentThreadId();
                {
                    std::lock_guard<std::mutex> lock(g_navigationMutex);
                    for (const auto& entry : g_navigationEntries)
                        if (entry.ownerThread == owner && entry.frame.get() == frame)
                            return;
                }

                NavigationEntry entry;
                entry.frame = winrt::make_weak(frame);
                entry.ownerThread = owner;
                entry.id = g_nextNavigationId.fetch_add(1);
                const uint64_t id = entry.id;
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
                            const auto pageType = args.SourcePageType().Name;
                            const ActivePage active = ClassifyPageType(pageType);
                            if (g_navigationEvents.fetch_add(1) < 12)
                                Wh_Log(L"Frame.Navigated: page=%d",
                                       static_cast<int>(active));
                            if (active == ActivePage::Other) {
                                ResumeOnUnsupportedPage();
                            } else if (active == ActivePage::Supported) {
                                auto page = args.Content().try_as<mux::FrameworkElement>();
                                if (page && NeedsFrameNavigationScan(pageType, page)) {
                                    // If there is no entry hook, the toolbar
                                    // was not ready, or insertion failed,
                                    // retry after navigation completes.
                                    PruneExpiredEntriesOnCurrentThread();
                                    FindCommandBars(page);
                                }
                            }
                        } catch (...) {
                            Wh_Log(L"Frame navigation callback failed: 0x%08X",
                                   winrt::to_hresult());
                        }
                    });
                    attached = true;
                    {
                        std::lock_guard<std::mutex> lock(g_navigationMutex);
                        for (auto& saved : g_navigationEntries)
                            if (saved.id == id) {
                                saved.token = token;
                                saved.attached = true;
                                break;
                            }
                    }
                    Wh_Log(L"Frame.Navigated attached on thread %lu", owner);
                } catch (...) {
                    const HRESULT error = winrt::to_hresult();
                    bool safe = !attached;
                    if (attached) {
                        try { frame.Navigated(token); safe = true; } catch (...) {}
                    }
                    if (tracked && safe) {
                        std::lock_guard<std::mutex> lock(g_navigationMutex);
                        g_navigationEntries.erase(std::remove_if(
                            g_navigationEntries.begin(), g_navigationEntries.end(),
                            [id](const NavigationEntry& e) { return e.id == id; }),
                            g_navigationEntries.end());
                    }
                    Wh_Log(L"Frame.Navigated attachment failed: 0x%08X", error);
                }
                return;
            }
            object = muxm::VisualTreeHelper::GetParent(object);
        }
    } catch (...) {
        static std::atomic<unsigned> logged{0};
        if (logged.fetch_add(1) < 4)
            Wh_Log(L"Frame navigation discovery unavailable: 0x%08X",
                   winrt::to_hresult());
    }
}

// IPageOverrides::OnNavigatedTo belongs to a C++/WinRT produce interface.
// The supplied self pointer is an inspectable ABI interface pointer.
using PageNavigation_t = HRESULT(__cdecl*)(void*, void*);
PageNavigation_t g_processesToOriginal{};
PageNavigation_t g_detailsToOriginal{};
PageNavigation_t g_processesFromOriginal{};
PageNavigation_t g_detailsFromOriginal{};
// The compiler can fold identical Processes/Details navigation functions.
// In that case the Processes hook covers both page types.
bool g_detailsToUsesProcessesHook = false;

// Track whether the entry hook inserted or found a usable button on the
// specific page instance. The frame event retries failed/missing insertions,
// but avoids a duplicate scan if the button is already usable.
struct HookScanRecord {
    winrt::weak_ref<mux::FrameworkElement> page;
    DWORD ownerThread{};
    bool buttonReady{};
};
std::mutex g_hookScansMutex;
std::vector<HookScanRecord> g_hookScans;

void RememberHookScan(mux::FrameworkElement const& page, bool buttonReady) {
    const DWORD owner = GetCurrentThreadId();
    std::lock_guard<std::mutex> lock(g_hookScansMutex);
    auto it = std::find_if(g_hookScans.begin(), g_hookScans.end(),
        [owner](HookScanRecord const& item) { return item.ownerThread == owner; });
    HookScanRecord record{winrt::make_weak(page), owner, buttonReady};
    if (it != g_hookScans.end()) *it = std::move(record);
    else g_hookScans.push_back(std::move(record));
}

void ForgetHookScanThread(DWORD threadId) {
    std::lock_guard<std::mutex> lock(g_hookScansMutex);
    g_hookScans.erase(std::remove_if(g_hookScans.begin(), g_hookScans.end(),
        [threadId](HookScanRecord const& record) {
            return record.ownerThread == threadId;
        }), g_hookScans.end());
}

bool NeedsFrameNavigationScan(winrt::hstring const& pageType,
                              mux::FrameworkElement const& page) {
    const std::wstring_view name{pageType.c_str(), pageType.size()};
    bool hasEntryHook = false;
    if (EndsWith(name, L"ProcessesPage"))
        hasEntryHook = g_processesToOriginal != nullptr;
    else if (EndsWith(name, L"DetailsPage"))
        hasEntryHook = g_detailsToOriginal != nullptr ||
                       g_detailsToUsesProcessesHook;
    else
        return false;
    if (!hasEntryHook) return true;
    const DWORD owner = GetCurrentThreadId();
    std::lock_guard<std::mutex> lock(g_hookScansMutex);
    for (const auto& record : g_hookScans) {
        if (record.ownerThread == owner && record.page.get() == page)
            return !record.buttonReady;
    }
    return true; // A hook exists, but has not yet captured this page instance.
}

void CapturePageFromInterface(void* self) {
    if (!self || g_stopping.load(std::memory_order_acquire)) return;
    try {
        winrt::com_ptr<::IInspectable> object;
        HRESULT hr = reinterpret_cast<::IUnknown*>(self)->QueryInterface(
            __uuidof(::IInspectable), object.put_void());
        if (FAILED(hr) || !object) {
            Wh_Log(L"Page capture: IInspectable unavailable, hr=0x%08X", hr);
            return;
        }
        wf::IInspectable inspectable{nullptr};
        winrt::copy_from_abi(inspectable, object.get());
        auto page = inspectable.try_as<mux::FrameworkElement>();
        if (!page) {
            Wh_Log(L"Page capture: FrameworkElement unavailable");
            return;
        }
        // A shared navigation function can also be used by unrelated pages.
        // Never apply the structural command-bar fallback outside these two.
        if (ClassifyPageType(winrt::get_class_name(page)) !=
            ActivePage::Supported) return;
        PruneExpiredEntriesOnCurrentThread();
        const bool buttonReady = FindCommandBars(page);
        RememberHookScan(page, buttonReady);
    } catch (...) {
        Wh_Log(L"Page capture failed: 0x%08X", winrt::to_hresult());
    }
}

HRESULT __cdecl ProcessesNavigatedTo_Hook(void* self, void* args) {
    HRESULT result = g_processesToOriginal(self, args);
    if (SUCCEEDED(result) && !g_stopping.load(std::memory_order_acquire))
        CapturePageFromInterface(self);
    return result;
}

HRESULT __cdecl DetailsNavigatedTo_Hook(void* self, void* args) {
    HRESULT result = g_detailsToOriginal(self, args);
    if (SUCCEEDED(result) && !g_stopping.load(std::memory_order_acquire))
        CapturePageFromInterface(self);
    return result;
}

// SourcePageType identifies the navigation destination. Preserve Pause when
// switching Processes <-> Details. An unknown event/argument is not evidence
// of a real departure (OnNavigatedFrom can also run for GetNavigationState).
bool IsSupportedDestination(void* args, bool& known) {
    known = false;
    if (!args) return false;
    try {
        mux::Navigation::NavigationEventArgs navigation{nullptr};
        winrt::copy_from_abi(navigation, args);
        const auto name = navigation.SourcePageType().Name;
        if (name.empty()) return false;
        known = true;
        const std::wstring_view value{name.c_str(), name.size()};
        const auto endsWith = [value](std::wstring_view suffix) {
            return value.size() >= suffix.size() &&
                value.substr(value.size() - suffix.size()) == suffix;
        };
        return value == L"ProcessesPage" || value == L"DetailsPage" ||
               endsWith(L".ProcessesPage") || endsWith(L".DetailsPage");
    } catch (...) {
        Wh_Log(L"Could not inspect navigation destination; preserving pause state");
        return false;
    }
}

void OnLeavingSupportedPage(void* args) {
    if (g_stopping.load(std::memory_order_acquire)) return;
    bool known = false;
    const bool supported = IsSupportedDestination(args, known);
    if (!known || supported) return;
    if (g_paused.exchange(false, std::memory_order_acq_rel))
        SyncButtons();
}

HRESULT __cdecl ProcessesNavigatedFrom_Hook(void* self, void* args) {
    HRESULT result = g_processesFromOriginal(self, args);
    if (SUCCEEDED(result)) OnLeavingSupportedPage(args);
    return result;
}

HRESULT __cdecl DetailsNavigatedFrom_Hook(void* self, void* args) {
    HRESULT result = g_detailsFromOriginal(self, args);
    if (SUCCEEDED(result)) OnLeavingSupportedPage(args);
    return result;
}


} // namespace

BOOL Wh_ModInit() {
    {
        std::lock_guard<std::mutex> lock(g_insertionMutex);
        g_acceptingInsertions = true;
        g_insertionCount = 0;
    }
    g_stopping = false;
    g_paused = false;
    g_insertions = 0;
    g_cleanupFailures = 0;
    g_navigationEvents = 0;
    g_dispatchMsg = RegisterWindowMessageW(
        L"Windhawk.TaskManagerPauseButton.SynchronousUI");
    if (!g_dispatchMsg || !FindTaskManagerRange()) return FALSE;

    HMODULE host = GetModuleHandleW(nullptr);
    if (!host) return FALSE;

    // Resolve all four navigation symbols, but do not hook until we can
    // compare addresses. Identical C++/WinRT produce<> bodies may be folded
    // into one function by the linker; hook that function only once.
    PageNavigation_t processesTo{};
    PageNavigation_t detailsTo{};
    PageNavigation_t processesFrom{};
    PageNavigation_t detailsFrom{};
    WindhawkUtils::SYMBOL_HOOK taskmgrExeHooks[] = {
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::TaskManagerApp::implementation::ProcessesPage,struct winrt::Windows::UI::Xaml::Controls::IPageOverrides>::OnNavigatedTo(void *)"},
         &processesTo},
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::TaskManagerApp::implementation::DetailsPage,struct winrt::Windows::UI::Xaml::Controls::IPageOverrides>::OnNavigatedTo(void *)"},
         &detailsTo, nullptr, true},
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::TaskManagerApp::implementation::ProcessesPage,struct winrt::Windows::UI::Xaml::Controls::IPageOverrides>::OnNavigatedFrom(void *)"},
         &processesFrom, nullptr, true},
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::TaskManagerApp::implementation::DetailsPage,struct winrt::Windows::UI::Xaml::Controls::IPageOverrides>::OnNavigatedFrom(void *)"},
         &detailsFrom, nullptr, true},
    };
    if (!WindhawkUtils::HookSymbols(host, taskmgrExeHooks,
                                    ARRAYSIZE(taskmgrExeHooks))) {
        Wh_Log(L"Navigation symbol resolution failed (cache or PDB unavailable)");
        return FALSE;
    }
    if (!processesTo ||
        !WindhawkUtils::SetFunctionHook(processesTo, ProcessesNavigatedTo_Hook,
                                       &g_processesToOriginal)) {
        Wh_Log(L"Required ProcessesPage::OnNavigatedTo hook unavailable");
        return FALSE;
    }
    Wh_Log(L"Required ProcessesPage::OnNavigatedTo hook installed via Windhawk symbols");

    g_detailsToUsesProcessesHook = detailsTo && detailsTo == processesTo;
    const bool detailsToFound = detailsTo != nullptr;
    const bool detailsToHooked = g_detailsToUsesProcessesHook ||
        (detailsToFound && WindhawkUtils::SetFunctionHook(
            detailsTo, DetailsNavigatedTo_Hook, &g_detailsToOriginal));
    if (!detailsToHooked) g_detailsToOriginal = nullptr;
    Wh_Log(L"Optional DetailsPage::OnNavigatedTo: symbol=%s hook=%s",
           detailsToFound ? L"found" : L"missing",
           g_detailsToUsesProcessesHook ? L"shared" :
           detailsToHooked ? L"ok" : L"unavailable");

    // Optional exit hooks. One hook handles both page types when the linker
    // gave Processes and Details the same OnNavigatedFrom address.
    const bool processesExitFound = processesFrom != nullptr;
    const bool processesExitHooked = processesExitFound &&
        WindhawkUtils::SetFunctionHook(processesFrom,
                                      ProcessesNavigatedFrom_Hook,
                                      &g_processesFromOriginal);
    if (!processesExitHooked) g_processesFromOriginal = nullptr;
    Wh_Log(L"Optional ProcessesPage::OnNavigatedFrom: symbol=%s hook=%s",
           processesExitFound ? L"found" : L"missing",
           processesExitHooked ? L"ok" : L"unavailable");

    const bool detailsExitFound = detailsFrom != nullptr;
    const bool sharedExit = processesExitHooked &&
                            detailsFrom && detailsFrom == processesFrom;
    const bool detailsExitHooked = sharedExit ||
        (detailsExitFound && detailsFrom != processesFrom &&
         WindhawkUtils::SetFunctionHook(detailsFrom,
                                       DetailsNavigatedFrom_Hook,
                                       &g_detailsFromOriginal));
    if (!detailsExitHooked) g_detailsFromOriginal = nullptr;
    Wh_Log(L"Optional DetailsPage::OnNavigatedFrom: symbol=%s hook=%s",
           detailsExitFound ? L"found" : L"missing",
           sharedExit ? L"shared" :
           detailsExitHooked ? L"ok" : L"unavailable");

    // Intentionally unchanged: preserve the tested GetKeyState-based refresh
    // hold, including its Taskmgr.exe return-address restriction.
    if (!WindhawkUtils::SetFunctionHook(
            GetKeyState, GetKeyState_Hook, &g_getKeyOriginal)) return FALSE;

    Wh_Log(L"Ready: Processes hook, Details hook=%s, Frame.Navigated fallback",
           g_detailsToUsesProcessesHook ? L"shared" :
           g_detailsToOriginal ? L"separate" : L"unavailable");
    return TRUE;
}

void Wh_ModBeforeUninit() {
    StopNewInsertions();
}


void Wh_ModUninit() {
    StopNewInsertions();
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
        // require the user to close Task Manager; a simple timeout would be
        // unsafe without first revoking all callbacks.
        Sleep(250);
    }
    Wh_Log(L"Uninit finished: inserted=%u terminatedThreads=%u cleanupErrors=%u",
           g_insertions.load(), terminatedThreads, g_cleanupFailures.load());
}
