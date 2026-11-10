// ==WindhawkMod==
// @id              task-manager-pause-button
// @name            Task Manager Pause Button
// @description     Add a native Pause/Resume button to Processes and Details in Windows 11 Task Manager.
// @version         1.0.0
// @author          Luiz Bizzio
// @github          https://github.com/luizbizzio
// @homepage        https://github.com/luizbizzio/task-manager-pause-button
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

Both pages share the same Pause / Resume state. The mod does not suspend apps,
stop Windows processes, or change the Real time update speed setting.

## Compatibility and limitations

- Works with the Windows 11 Task Manager XAML interface. Windows updates may
  change private Task Manager functions or controls.
- Uses Task Manager's Ctrl-key refresh hold through `GetKeyState` calls made
  directly by `Taskmgr.exe`. While paused, some Task Manager keyboard actions
  may behave as if Ctrl is held. This does not affect other applications.
- Detects the command bar by its button labels in English and Portuguese.
  Other Task Manager display languages may not show the button.
- The button uses English labels, even if Task Manager is in Portuguese.
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
#include <winrt/Windows.UI.Xaml.Media.h>
#include <atomic>
#include <mutex>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace mux = winrt::Windows::UI::Xaml;
namespace muxc = winrt::Windows::UI::Xaml::Controls;
namespace muxm = winrt::Windows::UI::Xaml::Media;
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
    bool clickDetached{};
    bool removedFromBar{};
};

std::mutex g_barsMutex;
std::vector<BarEntry> g_bars;
std::atomic<uint64_t> g_nextEntryId{1};
thread_local bool g_addingButton = false;

void TryInsertButton(muxc::CommandBar const& bar);
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

void TryInsertButton(muxc::CommandBar const& bar) {
    if (!bar || g_stopping.load(std::memory_order_acquire) || g_addingButton) return;
    struct Guard {
        Guard() { g_addingButton = true; }
        ~Guard() { g_addingButton = false; }
    } guard;

    muxc::AppBarButton button{nullptr};
    winrt::event_token clickToken{};
    bool clickAttached = false;
    bool inserted = false;
    try {
        auto commands = bar.PrimaryCommands();
        if (!commands) return;

        bool hasRun = false;
        bool hasEnd = false;
        uint32_t place = 0;
        for (uint32_t i = 0; i < commands.Size(); ++i) {
            auto item = commands.GetAt(i).try_as<muxc::AppBarButton>();
            if (!item) continue;
            if (item.Name() == kButtonName) return;
            auto caption = item.Label();
            if (caption == L"Run new task" || caption == L"Executar nova tarefa") {
                hasRun = true;
                place = i + 1;
            }
            if (caption == L"End task" || caption == L"Finalizar tarefa")
                hasEnd = true;
        }
        if (!hasRun || !hasEnd) return;

        button = muxc::AppBarButton();
        button.Name(kButtonName);
        UpdateButton(button);
        clickToken = button.Click([](auto const&, auto const&) {
            if (!g_stopping.load(std::memory_order_acquire)) TogglePause();
        });
        clickAttached = true;
        commands.InsertAt(place, button);
        inserted = true;

        BarEntry entry;
        entry.bar = winrt::make_weak(bar);
        entry.button = winrt::make_weak(button);
        entry.ownerThread = GetCurrentThreadId();
        entry.clickToken = clickToken;
        entry.entryId = g_nextEntryId.fetch_add(1, std::memory_order_relaxed);
        {
            std::lock_guard<std::mutex> lock(g_barsMutex);
            g_bars.push_back(std::move(entry));
        }
        const auto count = g_insertions.fetch_add(1) + 1;
        if (count <= 8) Wh_Log(L"[TMPB] Native Pause button inserted (%u)", count);
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        // Avoid leaving a live Click handler behind if tracking the button fails.
        try {
            if (button && clickAttached) button.Click(clickToken);
        } catch (...) {}
        if (inserted) {
            try {
                auto commands = bar.PrimaryCommands();
                if (commands) {
                    for (uint32_t i = 0; i < commands.Size(); ++i) {
                        auto candidate = commands.GetAt(i).try_as<muxc::AppBarButton>();
                        if (candidate && candidate == button) {
                            commands.RemoveAt(i);
                            break;
                        }
                    }
                }
            } catch (...) {}
        }
        Wh_Log(L"[TMPB] Button insertion failed: 0x%08X", error);
    }
}

// Synchronous work on the XAML thread, using the standard in-process
// WH_CALLWNDPROC approach. Tokens, not pointers, are passed in LPARAM.
// A timed-out request is canceled under the same lock the hook uses to start.
constexpr DWORD kUiTimeoutMs = 5000;
UINT g_dispatchMsg{};
struct UiRequest {
    uint64_t token{};
    DWORD threadId{};
    bool (*callback)(DWORD){};
    HANDLE completion{};
    bool started{};
    bool canceled{};
    bool callbackSucceeded{};
};
std::mutex g_requestMutex;
std::vector<UiRequest*> g_requests;
std::atomic<uint64_t> g_nextToken{1};

// A dead UI thread cannot invoke any of its XAML callbacks again.
// Treat an unknown or access-denied thread as live, for a safe unload.
bool UiThreadTerminated(DWORD threadId) {
    HANDLE thread = OpenThread(SYNCHRONIZE, FALSE, threadId);
    if (!thread) return GetLastError() == ERROR_INVALID_PARAMETER;
    DWORD status = WaitForSingleObject(thread, 0);
    CloseHandle(thread);
    return status == WAIT_OBJECT_0;
}

// A hook callback may remain callable if unhooking fails. Never return from
// unload while that callback could still point into the module.
void UnhookDispatchSafely(HHOOK hook, DWORD threadId) {
    if (!hook) return;
    unsigned attempts = 0;
    for (;;) {
        if (UnhookWindowsHookEx(hook)) return;
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HOOK_HANDLE || UiThreadTerminated(threadId))
            return;
        if (++attempts % 50 == 0) {
            Wh_Log(L"[TMPB] Waiting to remove UI dispatch hook thread=%lu error=%lu",
                   threadId, error);
        }
        Sleep(100);
    }
}

LRESULT CALLBACK DispatchHook(int code, WPARAM wp, LPARAM lp) {
    UiRequest* request = nullptr;
    if (code == HC_ACTION && g_dispatchMsg) {
        auto* msg = reinterpret_cast<CWPSTRUCT*>(lp);
        if (msg && msg->message == g_dispatchMsg) {
            std::lock_guard<std::mutex> lock(g_requestMutex);
            for (auto* candidate : g_requests) {
                if (candidate->token == static_cast<uint64_t>(msg->lParam) &&
                    candidate->threadId == GetCurrentThreadId() &&
                    !candidate->canceled && !candidate->started) {
                    candidate->started = true;
                    request = candidate;
                    break;
                }
            }
        }
    }
    if (request) {
        try {
            request->callbackSucceeded = request->callback(request->threadId);
        } catch (...) {
            request->callbackSucceeded = false;
            Wh_Log(L"[TMPB] UI callback exception");
        }
    }
    LRESULT next = CallNextHookEx(nullptr, code, wp, lp);
    // Signal only after the entire hook chain returned, so cleanup does not
    // consider a callback complete while CallNextHookEx is still executing.
    if (request) SetEvent(request->completion);
    return next;
}

HWND FindThreadWindow(DWORD threadId) {
    HWND result = nullptr;
    EnumThreadWindows(threadId, [](HWND hwnd, LPARAM param) -> BOOL {
        *reinterpret_cast<HWND*>(param) = hwnd;
        return FALSE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

bool RunOnUiThread(DWORD threadId, bool (*callback)(DWORD)) {
    if (threadId == GetCurrentThreadId()) {
        try {
            return callback(threadId);
        } catch (...) {
            Wh_Log(L"[TMPB] Same-thread UI callback exception");
            return false;
        }
    }
    HWND hwnd = FindThreadWindow(threadId);
    if (!hwnd) return false;
    UiRequest request;
    request.token = g_nextToken.fetch_add(1, std::memory_order_relaxed);
    request.threadId = threadId;
    request.callback = callback;
    request.completion = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!request.completion) return false;

    {
        std::lock_guard<std::mutex> lock(g_requestMutex);
        g_requests.push_back(&request);
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, DispatchHook, nullptr, threadId);
    DWORD_PTR ignored{};
    bool sent = false;
    if (hook) {
        sent = SendMessageTimeoutW(hwnd, g_dispatchMsg, 0,
            static_cast<LPARAM>(request.token),
            SMTO_BLOCK | SMTO_ABORTIFHUNG, kUiTimeoutMs, &ignored) != 0;
    }
    bool started = false;
    {
        std::lock_guard<std::mutex> lock(g_requestMutex);
        request.canceled = true;
        started = request.started;
    }
    // If the callback started, its stack request cannot be freed until
    // execution completes. Also allow the owner thread to exit.
    if (started && WaitForSingleObject(request.completion, 0) != WAIT_OBJECT_0) {
        HANDLE owner = OpenThread(SYNCHRONIZE, FALSE, threadId);
        if (owner) {
            HANDLE handles[] = {request.completion, owner};
            const DWORD waitResult = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
            CloseHandle(owner);
            if (waitResult == WAIT_FAILED && !UiThreadTerminated(threadId))
                WaitForSingleObject(request.completion, INFINITE);
        } else if (!UiThreadTerminated(threadId)) {
            WaitForSingleObject(request.completion, INFINITE);
        }
    }
    const bool completed = started &&
        WaitForSingleObject(request.completion, 0) == WAIT_OBJECT_0;
    const bool callbackSucceeded = completed && request.callbackSucceeded;

    // SetEvent runs immediately before the hook returns. If the synchronous
    // send timed out, a subsequent successful message proves the first hook
    // invocation has fully unwound before the request and DLL are released.
    if (started && !sent && !UiThreadTerminated(threadId)) {
        unsigned attempts = 0;
        for (;;) {
            HWND barrierWindow = FindThreadWindow(threadId);
            DWORD_PTR barrierResult{};
            if (barrierWindow && SendMessageTimeoutW(barrierWindow, WM_NULL, 0, 0,
                    SMTO_BLOCK | SMTO_ABORTIFHUNG, kUiTimeoutMs, &barrierResult))
                break;
            if (UiThreadTerminated(threadId)) break;
            if (++attempts % 4 == 0)
                Wh_Log(L"[TMPB] Waiting for UI callback to finish thread=%lu", threadId);
            Sleep(250);
        }
    }
    UnhookDispatchSafely(hook, threadId);
    {
        std::lock_guard<std::mutex> lock(g_requestMutex);
        auto it = std::find(g_requests.begin(), g_requests.end(), &request);
        if (it != g_requests.end()) g_requests.erase(it);
    }
    CloseHandle(request.completion);
    if (!completed || !callbackSucceeded) {
        Wh_Log(L"[TMPB] UI dispatch incomplete thread=%lu sent=%d started=%d completed=%d callbackOK=%d",
               threadId, sent, started, completed, callbackSucceeded);
    } else if (!sent) {
        Wh_Log(L"[TMPB] UI message timed out but callback finished thread=%lu", threadId);
    }
    return callbackSucceeded;
}

void PruneExpiredEntriesOnCurrentThread() {
    DWORD threadId = GetCurrentThreadId();
    std::lock_guard<std::mutex> lock(g_barsMutex);
    g_bars.erase(std::remove_if(g_bars.begin(), g_bars.end(),
        [threadId](BarEntry const& entry) {
            if (entry.ownerThread != threadId) return false;
            try { return !entry.button.get(); }
            catch (...) { return false; }
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
            Wh_Log(L"[TMPB] Could not sync thread %lu", threadId);
    }
}

std::atomic<unsigned> g_cleanupFailures{0};

// Called only on the owner XAML thread. Preserve each entry until both its
// Click handler and the command-bar item have been removed successfully.
// That way a transient failure can be retried without losing the token.
bool RemoveThreadButtonsAndEvents(DWORD threadId) {
    std::vector<BarEntry> entries;
    {
        std::lock_guard<std::mutex> lock(g_barsMutex);
        for (auto const& entry : g_bars)
            if (entry.ownerThread == threadId) entries.push_back(entry);
    }

    bool allRemoved = true;
    for (auto const& entry : entries) {
        bool detached = entry.clickDetached;
        bool removed = entry.removedFromBar;
        muxc::AppBarButton button{nullptr};

        try {
            button = entry.button.get();
        } catch (...) {
            ++g_cleanupFailures;
            Wh_Log(L"[TMPB] Button lookup failed: 0x%08X", winrt::to_hresult());
            allRemoved = false;
            continue;
        }

        if (!button) {
            // No live button means no live Click event source.
            detached = true;
            removed = true;
        }

        if (!detached) {
            try {
                button.Click(entry.clickToken);
                detached = true;
            } catch (...) {
                ++g_cleanupFailures;
                Wh_Log(L"[TMPB] Click event detach failed: 0x%08X", winrt::to_hresult());
            }
        }

        // Do not remove UI objects while their callbacks remain attached.
        if (detached && !removed) {
            try {
                if (auto bar = entry.bar.get()) {
                    auto commands = bar.PrimaryCommands();
                    if (commands) {
                        for (uint32_t i = commands.Size(); i > 0; --i) {
                            auto item = commands.GetAt(i - 1).try_as<muxc::AppBarButton>();
                            if (item && item.Name() == kButtonName)
                                commands.RemoveAt(i - 1);
                        }
                    }
                }
                removed = true;
            } catch (...) {
                ++g_cleanupFailures;
                Wh_Log(L"[TMPB] Button removal failed: 0x%08X", winrt::to_hresult());
            }
        }

        // Store progress across attempts. Do not hold the mutex while
        // calling XAML APIs, which could trigger unrelated callbacks.
        {
            std::lock_guard<std::mutex> lock(g_barsMutex);
            auto it = std::find_if(g_bars.begin(), g_bars.end(),
                [&entry](BarEntry const& item) {
                    return item.entryId == entry.entryId;
                });
            if (it != g_bars.end()) {
                it->clickDetached = detached;
                it->removedFromBar = removed;
                if (detached && removed) g_bars.erase(it);
            }
        }
        if (!detached || !removed) allRemoved = false;
    }
    return allRemoved;
}

bool HasEntriesForThread(DWORD threadId) {
    std::lock_guard<std::mutex> lock(g_barsMutex);
    return std::any_of(g_bars.begin(), g_bars.end(),
        [threadId](BarEntry const& entry) {
            return entry.ownerThread == threadId;
        });
}

void ForgetTerminatedThread(DWORD threadId) {
    std::lock_guard<std::mutex> lock(g_barsMutex);
    g_bars.erase(std::remove_if(g_bars.begin(), g_bars.end(),
        [threadId](BarEntry const& entry) {
            return entry.ownerThread == threadId;
        }), g_bars.end());
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


// Called on the XAML thread during page navigation.
void FindCommandBars(mux::FrameworkElement const& page) {
    if (g_stopping.load(std::memory_order_relaxed)) return;
    try {
        mux::DependencyObject root = page;
        for (unsigned i = 0; i < 48; ++i) {
            auto parent = muxm::VisualTreeHelper::GetParent(root);
            if (!parent) break;
            root = parent;
        }
        std::vector<mux::DependencyObject> stack;
        stack.push_back(root);
        unsigned scanned = 0;
        while (!stack.empty() && scanned < 5000 && !g_stopping.load()) {
            auto element = stack.back();
            stack.pop_back();
            ++scanned;
            if (auto bar = element.try_as<muxc::CommandBar>()) {
                TryInsertButton(bar);
            }
            int children = muxm::VisualTreeHelper::GetChildrenCount(element);
            for (int i = children - 1; i >= 0; --i) {
                stack.push_back(muxm::VisualTreeHelper::GetChild(element, i));
            }
        }
    } catch (...) {
        Wh_Log(L"[TMPB] Visual tree search failed: 0x%08X", winrt::to_hresult());
    }
}

// ABI projection: IPageOverrides::OnNavigatedTo(void*) belongs to a
// winrt::impl::produce COM interface, not the native implementation class.
// Its self argument is a COM interface pointer, so query it rather than
// treating it as an implementation::ProcessesPage pointer.
using OnNavigatedTo_t = HRESULT(__cdecl*)(void*, void*);
OnNavigatedTo_t g_onNavigatedToOriginal = nullptr;

void CapturePageFromInterface(void* self) {
    if (!self || g_stopping.load(std::memory_order_acquire)) return;
    try {
        winrt::com_ptr<::IInspectable> object;
        HRESULT hr = reinterpret_cast<::IUnknown*>(self)->QueryInterface(
            __uuidof(::IInspectable), object.put_void());
        if (FAILED(hr) || !object) return;

        wf::IInspectable inspectable{nullptr};
        winrt::copy_from_abi(inspectable, object.get());
        auto page = inspectable.try_as<mux::FrameworkElement>();
        if (!page) return;
        PruneExpiredEntriesOnCurrentThread();
        FindCommandBars(page);
    } catch (...) {
        Wh_Log(L"[TMPB] Page capture failed: 0x%08X", winrt::to_hresult());
    }
}

HRESULT __cdecl OnNavigatedTo_Hook(void* self, void* args) {
    HRESULT result = g_onNavigatedToOriginal(self, args);
    if (SUCCEEDED(result) && !g_stopping.load()) CapturePageFromInterface(self);
    return result;
}

} // namespace

BOOL Wh_ModInit() {
    g_stopping = false;
    g_paused = false;
    g_insertions = 0;
    g_cleanupFailures = 0;
    g_dispatchMsg = RegisterWindowMessageW(
        L"Windhawk.TaskManagerPauseButton.SynchronousUI");
    if (!g_dispatchMsg || !FindTaskManagerRange()) return FALSE;

    HMODULE host = GetModuleHandleW(nullptr);
    WindhawkUtils::SYMBOL_HOOK taskmgrExeHooks[] = {
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::TaskManagerApp::implementation::ProcessesPage,struct winrt::Windows::UI::Xaml::Controls::IPageOverrides>::OnNavigatedTo(void *)"},
         &g_onNavigatedToOriginal, OnNavigatedTo_Hook},
    };
    if (!WindhawkUtils::HookSymbols(host, taskmgrExeHooks, ARRAYSIZE(taskmgrExeHooks))) {
        Wh_Log(L"[TMPB] Page navigation symbol unavailable. No UI was changed.");
        return FALSE;
    }

    // Restrict Ctrl simulation to the single API observed in the tested build,
    // with a return-address check for calls from Taskmgr.exe itself.
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(GetKeyState),
        reinterpret_cast<void*>(GetKeyState_Hook),
        reinterpret_cast<void**>(&g_getKeyOriginal))) return FALSE;

    Wh_Log(L"[TMPB] Ready: navigation hook and minimal button Click callback.");
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_stopping.store(true, std::memory_order_release);
    g_paused.store(false, std::memory_order_release);
}

void Wh_ModUninit() {
    g_stopping.store(true, std::memory_order_release);
    g_paused.store(false, std::memory_order_release);
    auto threads = KnownThreads();
    unsigned terminatedThreads = 0;
    for (auto threadId : threads) {
        unsigned attempts = 0;
        while (HasEntriesForThread(threadId)) {
            if (UiThreadTerminated(threadId)) {
                // Events from a terminated XAML thread can never fire again.
                ForgetTerminatedThread(threadId);
                ++terminatedThreads;
                break;
            }
            RunOnUiThread(threadId, RemoveThreadButtonsAndEvents);
            if (!HasEntriesForThread(threadId)) break;
            if (++attempts % 4 == 0) {
                Wh_Log(L"[TMPB] Waiting for safe UI cleanup thread=%lu attempts=%u; close a hung Task Manager",
                       threadId, attempts);
            }
            Sleep(250);
        }
    }
    Wh_Log(L"[TMPB] Uninit finished: inserted=%u terminatedThreads=%u cleanupErrors=%u",
           g_insertions.load(), terminatedThreads, g_cleanupFailures.load());
}
