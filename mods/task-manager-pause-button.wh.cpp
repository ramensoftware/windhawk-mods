// ==WindhawkMod==
// @id              task-manager-pause-button
// @name            Task Manager Pause Button
// @description     Add a native Pause/Resume button to Processes and Details in Windows 11 Task Manager.
// @version         1.0.0
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

- Works with the Windows 11 Task Manager XAML interface. Windows updates may
  change private Task Manager functions or controls.
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
- Keeps Pause active when moving between Processes and Details. Automatically
  resumes when navigating to another page if the navigation-exit symbol is
  available on your Windows build.
- If the button is missing, close and reopen Task Manager with the mod enabled.
- No settings or background applications are needed.

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
    std::lock_guard<std::mutex> lock(g_barsMutex);
    for (auto const& entry : g_bars) {
        if (std::find(result.begin(), result.end(), entry.ownerThread) == result.end())
            result.push_back(entry.ownerThread);
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
    return allDetached;
}

void ForgetTerminatedThread(DWORD threadId) {
    std::lock_guard<std::mutex> lock(g_barsMutex);
    g_bars.erase(std::remove_if(g_bars.begin(), g_bars.end(),
        [threadId](BarEntry const& entry) { return entry.ownerThread == threadId; }),
        g_bars.end());
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
void FindCommandBars(mux::FrameworkElement const& page) {
    if (g_stopping.load(std::memory_order_acquire)) return;
    try {
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

// IPageOverrides::OnNavigatedTo belongs to a C++/WinRT produce interface.
// The supplied self pointer is an inspectable ABI interface pointer.
using PageNavigation_t = HRESULT(__cdecl*)(void*, void*);
PageNavigation_t g_processesToOriginal{};
PageNavigation_t g_detailsToOriginal{};
PageNavigation_t g_processesFromOriginal{};
PageNavigation_t g_detailsFromOriginal{};

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
        PruneExpiredEntriesOnCurrentThread();
        FindCommandBars(page);
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
    g_dispatchMsg = RegisterWindowMessageW(
        L"Windhawk.TaskManagerPauseButton.SynchronousUI");
    if (!g_dispatchMsg || !FindTaskManagerRange()) return FALSE;

    HMODULE host = GetModuleHandleW(nullptr);
    // Resolve the Taskmgr.exe symbols together so Windhawk can cache one
    // complete symbol set per build. The extra navigation hooks are optional.
    // Resolve optional targets without hooking them here: if an optional hook
    // can't be installed, it must not disable the required Processes hook.
    WindhawkUtils::SYMBOL_HOOK taskmgrExeHooks[] = {
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::TaskManagerApp::implementation::ProcessesPage,struct winrt::Windows::UI::Xaml::Controls::IPageOverrides>::OnNavigatedTo(void *)"},
         &g_processesToOriginal, ProcessesNavigatedTo_Hook},
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::TaskManagerApp::implementation::DetailsPage,struct winrt::Windows::UI::Xaml::Controls::IPageOverrides>::OnNavigatedTo(void *)"},
         &g_detailsToOriginal, nullptr, true},
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::TaskManagerApp::implementation::ProcessesPage,struct winrt::Windows::UI::Xaml::Controls::IPageOverrides>::OnNavigatedFrom(void *)"},
         &g_processesFromOriginal, nullptr, true},
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::TaskManagerApp::implementation::DetailsPage,struct winrt::Windows::UI::Xaml::Controls::IPageOverrides>::OnNavigatedFrom(void *)"},
         &g_detailsFromOriginal, nullptr, true},
    };
    if (!WindhawkUtils::HookSymbols(host, taskmgrExeHooks, ARRAYSIZE(taskmgrExeHooks))) {
        Wh_Log(L"Required ProcessesPage::OnNavigatedTo hook failed. No UI was changed.");
        return FALSE;
    }
    Wh_Log(L"Required ProcessesPage::OnNavigatedTo hook installed");

    // Install each resolved optional target independently. This preserves the
    // working Processes button even if a particular build rejects one target.
    const bool detailsFound = g_detailsToOriginal != nullptr;
    const bool detailsHooked = detailsFound && WindhawkUtils::SetFunctionHook(
        g_detailsToOriginal, DetailsNavigatedTo_Hook, &g_detailsToOriginal);
    if (!detailsHooked) g_detailsToOriginal = nullptr;
    Wh_Log(L"Optional DetailsPage::OnNavigatedTo: symbol=%s hook=%s",
           detailsFound ? L"found" : L"missing", detailsHooked ? L"ok" : L"unavailable");

    const bool processesExitFound = g_processesFromOriginal != nullptr;
    const bool processesExitHooked = processesExitFound && WindhawkUtils::SetFunctionHook(
        g_processesFromOriginal, ProcessesNavigatedFrom_Hook, &g_processesFromOriginal);
    if (!processesExitHooked) g_processesFromOriginal = nullptr;
    Wh_Log(L"Optional ProcessesPage::OnNavigatedFrom: symbol=%s hook=%s",
           processesExitFound ? L"found" : L"missing",
           processesExitHooked ? L"ok" : L"unavailable");

    const bool detailsExitFound = g_detailsFromOriginal != nullptr;
    const bool detailsExitHooked = detailsExitFound && WindhawkUtils::SetFunctionHook(
        g_detailsFromOriginal, DetailsNavigatedFrom_Hook, &g_detailsFromOriginal);
    if (!detailsExitHooked) g_detailsFromOriginal = nullptr;
    Wh_Log(L"Optional DetailsPage::OnNavigatedFrom: symbol=%s hook=%s",
           detailsExitFound ? L"found" : L"missing",
           detailsExitHooked ? L"ok" : L"unavailable");

    // Intentionally unchanged: preserve the tested GetKeyState-based refresh
    // hold, including its Taskmgr.exe return-address restriction.
    if (!WindhawkUtils::SetFunctionHook(
            GetKeyState, GetKeyState_Hook, &g_getKeyOriginal)) return FALSE;

    Wh_Log(L"Ready: XAML navigation and native Pause/Resume button");
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
        // require the user to close Task Manager; no finite wait is safe.
        Sleep(250);
    }
    Wh_Log(L"Uninit finished: inserted=%u terminatedThreads=%u cleanupErrors=%u",
           g_insertions.load(), terminatedThreads, g_cleanupFailures.load());
}
