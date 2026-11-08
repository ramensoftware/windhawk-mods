// ==WindhawkMod==
// @id              explorer-fast-navigation
// @name            Explorer Fast Navigation
// @description     Improves File Explorer navigation latency and eliminates redraw flicker
// @version         1.0.0
// @author          gilnett
// @github          https://github.com/gilnett
// @donateUrl       https://buymeacoffee.com/gilnett
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Fast Navigation

Makes folder navigation faster and less visually disruptive in File Explorer by removing intermediate redraw flicker
and deferring toolbar updates until folder contents are loaded.

## How it works

In Windows 10 and Windows 11, switching folders triggers intermediate window clearing ("Working on it..." placeholder)
and multiple synchronous redraw events from the Ribbon and XAML command bar before files are enumerated.

This mod optimizes the navigation pipeline:
- **Redraw suppression**: Suppresses intermediate window clears and synchronous redraw flushes during folder transitions.
- **Deferred toolbar updates**: Defers Ribbon and XAML command bar state notifications while files are being enumerated,
  flushing them once file enumeration completes (`DISPID_FILELISTENUMDONE`) or batch processing settles.

## Compatibility

- Windows 11 (21H2, 22H2, 23H2, 24H2)
- Windows 10 (1809 and later)
- Architectures: 64-bit (x86-64, ARM64)

## Settings

- **Defer immediate navigation paints**: Defer immediate activation and layout paints while Explorer switches views.
- **Defer classic ribbon updates**: Delay classic ribbon refreshes during navigation.
- **Defer XAML command bar updates**: Delay modern command bar refreshes during navigation.
- **Defer navigation bar updates**: Delay navigation state updates in classic and modern navigation bars.
- **Safety fallback ceiling (ms)**: Maximum time (in ms) to wait before forcing a deferred UI update to flush if
  navigation stalls or on slow storage.

## Support

If you find this mod useful, you can support its development and maintenance:
- [Support on Buy Me a Coffee](https://buymeacoffee.com/gilnett)

## Credits

- **Vivy** ([@VivyVCCS](https://x.com/VivyVCCS) / [enginelesscc](https://github.com/enginelesscc)) for the original idea
  and concept.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- navigationPaint: true
  $name: Defer immediate navigation paints
  $description: Defer immediate activation and layout paints while Explorer switches views. Native loading and redraw
    protection remain active.
- classicRibbon: true
  $name: Defer classic ribbon updates
  $description: Delay classic ribbon refreshes during navigation.
- xamlCommandBar: true
  $name: Defer XAML command bar updates
  $description: Delay modern command bar refreshes during navigation. Navigation bar updates have a separate switch.
- navigationBar: true
  $name: Defer navigation bar updates
  $description: Delay navigation state updates in both the classic and modern navigation bars.
- safetyCeilingMs: 2000
  $name: Safety fallback ceiling (ms)
  $description: Maximum time in milliseconds before forcing pending UI updates to flush if navigation stalls or on slow network drives.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <oaidl.h>
#include <shdispid.h>
#include <shlobj.h>
#include <windhawk_utils.h>
#include <wrl/client.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

// Shared types.

struct Settings {
    bool navigationPaint = true;
    bool classicRibbon = true;
    bool xamlCommandBar = true;
    bool navigationBar = true;
    int safetyCeilingMs = 2000;
};

// Admit window work and stop it under the same lock. Never hold this lock
// across window/COM calls: they can reenter the mod on an owning UI thread.
class WindowLifetime {
    std::mutex mutex;
    std::condition_variable changed;
    std::atomic<bool> stopping{false};
    size_t operations = 0;
    std::atomic<size_t> callbacks{0};
    std::vector<HWND> windows;

  public:
    class Operation {
        WindowLifetime& owner;
        bool admitted;

      public:
        explicit Operation(WindowLifetime& owner) : owner(owner) {
            std::lock_guard lock(owner.mutex);
            admitted = !owner.stopping.load(std::memory_order_relaxed);
            if (admitted) {
                ++owner.operations;
            }
        }

        Operation(const Operation&) = delete;
        Operation& operator=(const Operation&) = delete;

        ~Operation() {
            if (admitted) {
                std::lock_guard lock(owner.mutex);
                --owner.operations;
                if (owner.stopping.load(std::memory_order_relaxed)) {
                    owner.changed.notify_all();
                }
            }
        }

        explicit operator bool() const {
            return admitted;
        }
    };

    class Callback {
        WindowLifetime& owner;

      public:
        explicit Callback(WindowLifetime& owner) : owner(owner) {
            owner.callbacks.fetch_add(1, std::memory_order_relaxed);
        }

        Callback(const Callback&) = delete;
        Callback& operator=(const Callback&) = delete;

        ~Callback() {
            if (owner.callbacks.fetch_sub(1, std::memory_order_acq_rel) == 1 &&
                owner.stopping.load(std::memory_order_acquire)) {
                std::lock_guard lock(owner.mutex);
                owner.changed.notify_all();
            }
        }
    };

    void Add(HWND hwnd) {
        std::lock_guard lock(mutex);
        windows.push_back(hwnd);
    }

    void Remove(HWND hwnd) {
        std::lock_guard lock(mutex);
        std::erase(windows, hwnd);
        changed.notify_all();
    }

    void Stop() {
        std::lock_guard lock(mutex);
        stopping.store(true, std::memory_order_release);
    }

    void CloseAndWait() {
        std::vector<HWND> copy;
        {
            std::unique_lock lock(mutex);
            changed.wait(lock, [this] {
                return operations == 0;
            });
            copy = windows;
        }
        for (HWND hwnd : copy) {
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
        }
        std::unique_lock lock(mutex);
        while (!changed.wait_for(lock, std::chrono::seconds(2), [this] {
            return windows.empty() && callbacks.load(std::memory_order_acquire) == 0;
        })) {
            Wh_Log(L"[LIFETIME] Waiting for %zu queue window(s) and %zu callback(s)",
                   windows.size(), callbacks.load(std::memory_order_relaxed));
        }
    }
};

using Callback = void (*)(void*);

struct Action { // GMA_ACTION ABI; checked before reading callback/parameters.
    DWORD size{};
    float startDelay{};
    float duration{};
    float repeatDelay{};
    int repeatCount{};
    int timeSlice{};
    Callback callback{};
    void* context{};
};

static_assert(sizeof(Action) == (sizeof(void*) == 8 ? 40 : 32) &&
              offsetof(Action, callback) == 24);

using CreateAction = uintptr_t (*)(const Action*);
using DeleteAction = BOOL(WINAPI*)(void*);

using RedrawFrame = void(__fastcall*)(void*);
using RenderSizer = HRESULT(__fastcall*)(void*);

struct PaintScope {
    unsigned& depth;

    explicit PaintScope(unsigned& d) : depth(d) {
        ++depth;
    }

    PaintScope(const PaintScope&) = delete;
    PaintScope& operator=(const PaintScope&) = delete;

    ~PaintScope() {
        --depth;
    }
};

// File-wide state and original function pointers.

static Settings settings;

static Callback batchCallback;
static RedrawFrame redrawFrameOriginal;
static RenderSizer renderSizerOriginal;
static thread_local unsigned redrawFrameDepth;
static thread_local unsigned renderSizerDepth;

static CreateAction createActionOriginal;
static DeleteAction deleteActionOriginal;
static bool trackBatchActions;
static decltype(&UpdateWindow) updateWindowOriginal;
static decltype(&RedrawWindow) redrawWindowOriginal;
static decltype(&SetWindowPos) setWindowPosOriginal;
static decltype(&LoadLibraryExW) loadLibraryExWOriginal;

static HMODULE frame;
static HMODULE shell;
static HMODULE duser;

// Shared helpers.

static void LoadSettings() {
    settings.navigationPaint = Wh_GetIntSetting(L"navigationPaint") != 0;
    settings.classicRibbon = Wh_GetIntSetting(L"classicRibbon") != 0;
    settings.xamlCommandBar = Wh_GetIntSetting(L"xamlCommandBar") != 0;
    settings.navigationBar = Wh_GetIntSetting(L"navigationBar") != 0;
    settings.safetyCeilingMs = Wh_GetIntSetting(L"safetyCeilingMs");
    if (settings.safetyCeilingMs < 200) {
        settings.safetyCeilingMs = 200;
    } else if (settings.safetyCeilingMs > 10000) {
        settings.safetyCeilingMs = 10000;
    }
}

static bool CanDeferInvoke(REFIID iid, WORD flags, const DISPPARAMS* args) {
    const GUID nullIid{};
    return IsEqualGUID(iid, nullIid) && flags == DISPATCH_METHOD && args &&
           args->cArgs == 0 && args->cNamedArgs == 0;
}

static BOOL IsShellDesktopOrTrayWindow(HWND hwnd) {
    if (!hwnd) {
        return FALSE;
    }
    wchar_t name[64];
    if (GetClassNameW(hwnd, name, ARRAYSIZE(name))) {
        if (wcscmp(name, L"Progman") == 0 || wcscmp(name, L"WorkerW") == 0 ||
            wcscmp(name, L"Shell_TrayWnd") == 0 ||
            wcscmp(name, L"Shell_SecondaryTrayWnd") == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL CALLBACK FindBrowserWindowProc(HWND hwnd, LPARAM param) {
    wchar_t name[64];
    if (!GetClassNameW(hwnd, name, ARRAYSIZE(name)) ||
        wcscmp(name, L"CabinetWClass") != 0) {
        return TRUE;
    }
    auto& found = *reinterpret_cast<HWND*>(param);
    if (found) {
        found = nullptr;
        return FALSE;
    }
    found = hwnd;
    return TRUE;
}

static HWND FindThreadBrowserWindow() {
    HWND browser = nullptr;
    EnumThreadWindows(GetCurrentThreadId(), FindBrowserWindowProc,
                      reinterpret_cast<LPARAM>(&browser));
    return browser;
}

// Cross-namespace notifications used by FileTransition.
namespace RibbonWork {
static bool DeferInvoke(void*, DISPID, REFIID, LCID, WORD, const DISPPARAMS*);
static void NavigationStarted();
static void ViewReady();
static bool Pending();
extern thread_local ULONGLONG started;
} // namespace RibbonWork

// Track view completion for deferred UI work without taking over file painting.
namespace FileTransition {
static constexpr UINT kAdaptiveCheckMs = 45;

struct State {
    void* itemsView{};
    void* batchAction{};
    ULONGLONG lastActivityTick = 0;
    ULONG generation = 0;
    size_t batchCount = 0;
    size_t lastCheckedBatchCount = 0;
    bool pending = false;
    bool enumerated = false;
};

using ResetRoot = HRESULT(__fastcall*)(void*, void*, void*);
using Navigate = HRESULT(__fastcall*)(void*, PCIDLIST_ABSOLUTE, ULONG, ULONG);
using ConnectionCreated = HRESULT(__fastcall*)(void*, IShellItem*, IUnknown*, ULONG, ULONG,
                                               IUnknown*, IUnknown*, REFGUID);
using EnsureBatching = HRESULT(__fastcall*)(void*);
using BatchTimer = void(__fastcall*)(void*, void*);
using DeleteBatchTimer = bool(__fastcall*)(void*);
using Invoke = HRESULT(__fastcall*)(void*, DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*,
                                    EXCEPINFO*, UINT*);

static thread_local State state;
static thread_local State* active;
static thread_local void* processingBatch;
static thread_local unsigned navigationDepth;
static bool completionTracking = true;

static ResetRoot resetOriginal;
static Navigate navigateOriginal;
static ConnectionCreated connectionOriginal;
static EnsureBatching ensureOriginal;
static BatchTimer batchOriginal;
static DeleteBatchTimer deleteOriginal;
static Invoke invokeOriginal;

static bool DeferNavigationPaint() {
    if (!settings.navigationPaint || !navigationDepth) {
        return false;
    }
    if (RibbonWork::started &&
        (GetTickCount64() - RibbonWork::started >=
         static_cast<ULONGLONG>(settings.safetyCeilingMs))) {
        return false;
    }
    return true;
}

static bool IsReady() {
    return completionTracking && active && active->itemsView && active->enumerated &&
           !active->pending && !navigationDepth && !processingBatch;
}

static bool HasActiveWork() {
    if (navigationDepth > 0 || processingBatch != nullptr) {
        return true;
    }
    if (!active || active->enumerated) {
        return false;
    }
    if (active->batchCount > active->lastCheckedBatchCount) {
        active->lastCheckedBatchCount = active->batchCount;
        return true;
    }
    if (GetTickCount64() - active->lastActivityTick <= 85) {
        return true;
    }
    active->pending = false;
    return false;
}

static void NotifyReady() {
    if (IsReady()) {
        RibbonWork::ViewReady();
    }
}

static void EnumerationDone() {
    if (active && active->itemsView) {
        active->enumerated = true;
        active->pending = false;
        NotifyReady();
    }
}

static void Begin(void* itemsView) {
    state = {itemsView, nullptr, GetTickCount64(), state.generation + 1, 0, 0, false, false};
    active = &state;
}

static HRESULT __fastcall NavigateHook(void* self, PCIDLIST_ABSOLUTE pidl, ULONG flags,
                                       ULONG extra) {
    RibbonWork::NavigationStarted();
    Begin(nullptr);
    HRESULT result;
    {
        PaintScope scope(navigationDepth);
        result = navigateOriginal(self, pidl, flags, extra);
    }
    if (FAILED(result)) {
        active = nullptr;
    } else {
        NotifyReady();
    }
    return result;
}

static HRESULT __fastcall ConnectionHook(void* self, IShellItem* item, IUnknown* connection,
                                         ULONG flags, ULONG extra, IUnknown* first,
                                         IUnknown* second, REFGUID id) {
    if (active) {
        active->lastActivityTick = GetTickCount64();
    }
    HRESULT result;
    {
        PaintScope scope(navigationDepth);
        result = connectionOriginal(self, item, connection, flags, extra, first, second, id);
    }
    if (active) {
        active->lastActivityTick = GetTickCount64();
    }
    NotifyReady();
    return result;
}

static HRESULT __fastcall ResetHook(void* self, void* item, void* collection) {
    if (!RibbonWork::Pending()) {
        RibbonWork::NavigationStarted();
    }
    Begin(self);
    ULONG generation = state.generation;
    HRESULT result;
    {
        PaintScope scope(navigationDepth);
        result = resetOriginal(self, item, collection);
    }
    if (active && active->generation == generation) {
        if (FAILED(result)) {
            active = nullptr;
        } else {
            active->lastActivityTick = GetTickCount64();
            NotifyReady();
        }
    }
    return result;
}

static HRESULT __fastcall EnsureHook(void* self) {
    ULONG generation = state.generation;
    HRESULT result = ensureOriginal(self);
    if (SUCCEEDED(result) && active && active->itemsView == self &&
        active->generation == generation) {
        active->lastActivityTick = GetTickCount64();
        if (!active->pending) {
            active->pending = true;
            ++active->batchCount;
        }
    }
    return result;
}

static bool __fastcall DeleteHook(void* self) {
    ULONG generation = state.generation;
    bool result = deleteOriginal(self);
    if (result && active && active->itemsView == self &&
        active->generation == generation) {
        active->pending = false;
        NotifyReady();
    }
    return result;
}

static void __fastcall BatchHook(void* self, void* info) {
    void* previous = processingBatch;
    processingBatch = self;
    if (active && active->itemsView == self) {
        ++active->batchCount;
        active->lastActivityTick = GetTickCount64();
    }
    batchOriginal(self, info);
    if (active && active->itemsView == self) {
        active->lastActivityTick = GetTickCount64();
    }
    processingBatch = previous;
    NotifyReady();
}

static HRESULT __fastcall InvokeHook(void* self, DISPID id, REFIID iid, LCID locale, WORD flags,
                                     DISPPARAMS* args, VARIANT* result, EXCEPINFO* exception,
                                     UINT* argError) {
    HRESULT hr;
    if (RibbonWork::DeferInvoke(self, id, iid, locale, flags, args)) {
        if (result) {
            *result = {};
        }
        hr = S_OK;
    } else {
        hr = invokeOriginal(self, id, iid, locale, flags, args, result, exception, argError);
    }
    if (id == DISPID_FILELISTENUMDONE) {
        EnumerationDone();
    }
    return hr;
}

} // namespace FileTransition

// Run ribbon state refreshes on the owning UI thread after native view painting.
namespace RibbonWork {
enum class TaskKind : unsigned char {
    ClassicInvoke,
    ClassicNavigated,
    ClassicNavState,
    XamlNavigated,
    XamlInvalidated,
    XamlNavState,
    XamlInvalidateCommands,
    XamlInvoke,
};

struct Task {
    Microsoft::WRL::ComPtr<IUnknown> object{};
    void* target{};
    TaskKind kind{};
    ULONG param = 0;
    std::function<void()> run{};
};

struct Queue {
    HWND hwnd{};
    HWND root{};
    HHOOK inputHook{};
    std::deque<Task> tasks;
    bool timerActive = false;
    bool draining = false;
    bool closing = false;
};

using Notification = HRESULT(__fastcall*)(void*);
using Destroy = HRESULT(__fastcall*)(void*, BOOL);
using NavState = HRESULT(__fastcall*)(void*, ULONG);

static constexpr wchar_t ClassName[] = L"Windhawk.ExplorerFastNavigation.RibbonWork";

static thread_local Queue* queue;
static thread_local void* connected;
thread_local ULONGLONG started = 0;
static thread_local bool ready;
static WindowLifetime lifetime;
static HINSTANCE instance;
static bool registered;

static Notification navigatedOriginal;
static Notification changedOriginal;
static Destroy destroyOriginal;
static NavState navStateOriginal;

static void Flush() {
    WindowLifetime::Callback callback(lifetime);
    Queue* q = queue;
    if (!q || q->draining) {
        return;
    }
    q->draining = true;
    if (q->timerActive) {
        KillTimer(q->hwnd, 1);
        q->timerActive = false;
    }
    while (!q->tasks.empty()) {
        Task task = std::move(q->tasks.front());
        q->tasks.pop_front();
        if (!q->closing) {
            task.run();
        }
    }
    q->draining = false;
    if (q->closing) {
        DestroyWindow(q->hwnd);
    }
}

static void DropTasksForTarget(void* target) {
    if (!queue || !target) {
        return;
    }
    std::erase_if(queue->tasks, [target](const Task& t) {
        return t.target == target;
    });
    if (queue->tasks.empty() && queue->timerActive) {
        KillTimer(queue->hwnd, 1);
        queue->timerActive = false;
    }
}

static LRESULT CALLBACK InputHook(int code, WPARAM wp, LPARAM lp) {
    WindowLifetime::Callback callback(lifetime);
    if (code >= 0 && wp == PM_REMOVE && queue && !queue->tasks.empty()) {
        const auto msg = reinterpret_cast<const MSG*>(lp);
        if (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN ||
            msg->message == WM_LBUTTONDOWN || msg->message == WM_RBUTTONDOWN ||
            msg->message == WM_MBUTTONDOWN || msg->message == WM_NCLBUTTONDOWN) {
            ready = true;
            Flush();
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

static LRESULT CALLBACK OwnerSubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR data) {
    WindowLifetime::Callback callback(lifetime);
    if (msg == WM_DESTROY) {
        auto q = reinterpret_cast<Queue*>(data);
        q->closing = true;
        Flush();
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK Proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    WindowLifetime::Callback callback(lifetime);
    if (msg == WM_APP) {
        if (ready && !FileTransition::navigationDepth && !FileTransition::processingBatch) {
            Flush();
        }
        return 0;
    }
    if (msg == WM_TIMER && wp == 1) {
        if ((ready && !FileTransition::navigationDepth && !FileTransition::processingBatch) ||
            !FileTransition::HasActiveWork() ||
            (GetTickCount64() - started >= static_cast<ULONGLONG>(settings.safetyCeilingMs))) {
            ready = true;
            Flush();
        }
        return 0;
    }
    if (msg == WM_CLOSE) {
        auto q = reinterpret_cast<Queue*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (!q) {
            DestroyWindow(hwnd);
            return 0;
        }
        if (!q->draining) {
            Flush();
        }
        q->closing = true;
        Flush();
        return 0;
    }
    if (msg == WM_NCDESTROY) {
        auto q = reinterpret_cast<Queue*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        if (q) {
            if (q->inputHook) {
                UnhookWindowsHookEx(q->inputHook);
            }
            if (q->root) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(q->root, OwnerSubclass);
            }
            if (queue == q) {
                queue = nullptr;
            }
            delete q;
        }
        lifetime.Remove(hwnd);
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static bool Enqueue(IUnknown* object, TaskKind kind, ULONG param, std::function<void()> run) {
    WindowLifetime::Operation operation(lifetime);
    if (!operation || (queue && queue->draining)) {
        return false;
    }
    if (!queue) {
        HWND root = FindThreadBrowserWindow();
        if (!root) {
            return false;
        }
        auto q = std::make_unique<Queue>();
        q->hwnd = CreateWindowExW(0, ClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                  instance, nullptr);
        if (!q->hwnd) {
            return false;
        }
        SetWindowLongPtrW(q->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(q.get()));
        q->root = root;
        q->inputHook = SetWindowsHookExW(WH_GETMESSAGE, InputHook, nullptr, GetCurrentThreadId());
        if (!q->inputHook || !WindhawkUtils::SetWindowSubclassFromAnyThread(
                                 root, OwnerSubclass, reinterpret_cast<DWORD_PTR>(q.get()))) {
            DestroyWindow(q->hwnd);
            return false;
        }
        lifetime.Add(q->hwnd);
        queue = q.release();
    }
    if (queue->closing) {
        Flush();
        return false;
    }
    for (Task& existing : queue->tasks) {
        if (existing.target == object && existing.kind == kind) {
            if (kind == TaskKind::ClassicNavState || kind == TaskKind::XamlNavState) {
                existing.param = param;
                existing.run = std::move(run);
                return true;
            }
            if (existing.param == param) {
                return true;
            }
        }
    }
    if (queue->tasks.size() >= 64) {
        Flush();
        return false;
    }
    if (!queue->timerActive) {
        if (!SetTimer(queue->hwnd, 1, FileTransition::kAdaptiveCheckMs, nullptr)) {
            Flush();
            return false;
        }
        queue->timerActive = true;
    }
    queue->tasks.push_back(
        {Microsoft::WRL::ComPtr<IUnknown>(object), object, kind, param, std::move(run)});
    return true;
}

static void NavigationStarted() {
    Flush();
    started = GetTickCount64();
    ready = false;
}

static void ViewReady() {
    ready = true;
    if (!FileTransition::navigationDepth && !FileTransition::processingBatch) {
        Flush();
    } else if (queue && queue->hwnd) {
        PostMessageW(queue->hwnd, WM_APP, 0, 0);
    }
}

static bool Pending() {
    return started && !ready &&
           (GetTickCount64() - started < static_cast<ULONGLONG>(settings.safetyCeilingMs));
}

// DISPID_FILELISTENUMDONE is deferred alongside other UI updates so that
// the Ribbon is notified only after the view settles, while
// EnumerationDone() uses the event internally as the trigger to flush.
static bool DeferInvoke(void* self, DISPID id, REFIID iid, LCID locale, WORD flags,
                        const DISPPARAMS* args) {
    if (!destroyOriginal || !settings.classicRibbon || !CanDeferInvoke(iid, flags, args) ||
        !Pending() ||
        (id != DISPID_SELECTIONCHANGED && id != DISPID_FILELISTENUMDONE &&
         id != DISPID_VIEWMODECHANGED && id != DISPID_CONTENTSCHANGED &&
         id != DISPID_COLUMNSCHANGED && id != DISPID_ICONSIZECHANGED &&
         id != DISPID_SELECTEDITEMCHANGED)) {
        return false;
    }
    return Enqueue(reinterpret_cast<IUnknown*>(self), TaskKind::ClassicInvoke,
                   static_cast<ULONG>(id), [self, id, iid, locale, flags] {
                       DISPPARAMS empty{};
                       FileTransition::invokeOriginal(self, id, iid, locale, flags, &empty, nullptr,
                                                      nullptr, nullptr);
                   });
}

static HRESULT __fastcall NavigatedHook(void* self) {
    if (destroyOriginal && settings.classicRibbon && self == connected && Pending() &&
        Enqueue(reinterpret_cast<IUnknown*>(self), TaskKind::ClassicNavigated, 0, [self] {
            navigatedOriginal(self);
        })) {
        return S_OK;
    }
    return navigatedOriginal(self);
}

static HRESULT __fastcall ChangedHook(void* self) {
    if (!Pending()) {
        Flush();
    }
    HRESULT hr = changedOriginal(self);
    connected = SUCCEEDED(hr) ? self : nullptr;
    return hr;
}

static HRESULT __fastcall DestroyHook(void* self, BOOL final) {
    if (queue) {
        if (!queue->draining) {
            Flush();
        } else {
            DropTasksForTarget(self);
        }
    }
    if (connected == self) {
        connected = nullptr;
    }
    return destroyOriginal(self, final);
}

static HRESULT __fastcall NavStateHook(void* self, ULONG flags) {
    if (settings.navigationBar && Pending() &&
        Enqueue(reinterpret_cast<IUnknown*>(self), TaskKind::ClassicNavState, flags,
                [self, flags] {
                    navStateOriginal(self, flags);
                })) {
        return S_OK;
    }
    return navStateOriginal(self, flags);
}

static bool Initialize() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(Proc), &instance);
    WNDCLASSW cls{};
    cls.lpfnWndProc = Proc;
    cls.hInstance = instance;
    cls.lpszClassName = ClassName;
    registered = RegisterClassW(&cls) != 0;
    return registered;
}

static void BeforeUninit() {
    lifetime.Stop();
    lifetime.CloseAndWait();
}

static void Uninit() {
    if (registered) {
        if (!UnregisterClassW(ClassName, instance)) {
            Wh_Log(L"[CORE] Failed to unregister window class: %lu", GetLastError());
        }
        registered = false;
    }
}

} // namespace RibbonWork

// The modern command surface has its own event sink and navigation-band implementation.
namespace XamlWork {
using Notification = HRESULT(__fastcall*)(void*);
using StateChange = HRESULT(__fastcall*)(void*, ULONG);
using Invoke = FileTransition::Invoke;

static constexpr ULONG NAVSTATE_LOCATION = 0x4;

static std::atomic<bool> enabled{false};
static std::atomic<bool> hooksInstalled{false};
static thread_local void* connected;
static Notification navigatedOriginal;
static Notification changedOriginal;
static Notification invalidatedOriginal;
static Notification cleanupOriginal;
static StateChange stateOriginal;
static StateChange invalidateOriginal;
static Invoke invokeOriginal;

static bool Pending() {
    return enabled.load(std::memory_order_relaxed) && RibbonWork::Pending();
}

static HRESULT Run(Notification original, void* self, const wchar_t* label) {
    HRESULT hr = original(self);
    if (FAILED(hr)) {
        Wh_Log(L"[XAML] Deferred refresh failed (%s, hr=0x%08X)", label, hr);
    }
    return hr;
}

static HRESULT __fastcall ChangedHook(void* self) {
    HRESULT hr = changedOriginal(self);
    connected = SUCCEEDED(hr) ? self : nullptr;
    return hr;
}

static HRESULT __fastcall NavigatedHook(void* self) {
    if (settings.xamlCommandBar && Pending() && self == connected &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), RibbonWork::TaskKind::XamlNavigated,
                            0, [self] {
                                Run(navigatedOriginal, self, L"xaml_navigated");
                            })) {
        return S_OK;
    }
    return navigatedOriginal(self);
}

static HRESULT __fastcall InvalidatedHook(void* self) {
    if (settings.xamlCommandBar && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self),
                            RibbonWork::TaskKind::XamlInvalidated, 0, [self] {
                                Run(invalidatedOriginal, self, L"xaml_invalidated");
                            })) {
        return S_OK;
    }
    return invalidatedOriginal(self);
}

static HRESULT __fastcall StateHook(void* self, ULONG flags) {
    if (settings.navigationBar && (flags & NAVSTATE_LOCATION) && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), RibbonWork::TaskKind::XamlNavState,
                            flags, [self, flags] {
                                HRESULT hr = stateOriginal(self, flags);
                                if (FAILED(hr)) {
                                    Wh_Log(L"[XAML] Deferred location failed (hr=0x%08X)", hr);
                                }
                            })) {
        return S_OK;
    }
    return stateOriginal(self, flags);
}

static HRESULT __fastcall InvalidateHook(void* self, ULONG commands) {
    if (settings.xamlCommandBar && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self),
                            RibbonWork::TaskKind::XamlInvalidateCommands, commands,
                            [self, commands] {
                                invalidateOriginal(self, commands);
                            })) {
        return S_OK;
    }
    return invalidateOriginal(self, commands);
}

static HRESULT __fastcall InvokeHook(void* self, DISPID id, REFIID iid, LCID locale, WORD flags,
                                     DISPPARAMS* args, VARIANT* result, EXCEPINFO* exception,
                                     UINT* argError) {
    // DISPID_FILELISTENUMDONE is deferred alongside other UI updates so that
    // the XAML command bar is notified only after the view settles, while
    // EnumerationDone() uses the event internally as the trigger to flush.
    bool invalidation = id == DISPID_SELECTIONCHANGED || id == DISPID_FILELISTENUMDONE ||
                        id == DISPID_VIEWMODECHANGED || id == DISPID_CONTENTSCHANGED ||
                        id == DISPID_VIEWPAINTDONE || id == DISPID_COLUMNSCHANGED ||
                        id == DISPID_ICONSIZECHANGED || id == DISPID_SELECTEDITEMCHANGED;
    HRESULT hr;
    if (settings.xamlCommandBar && invalidation && CanDeferInvoke(iid, flags, args) && Pending() &&
        RibbonWork::Enqueue(
            reinterpret_cast<IUnknown*>(self), RibbonWork::TaskKind::XamlInvoke,
            static_cast<ULONG>(id), [self, id, iid, locale, flags] {
                DISPPARAMS empty{};
                invokeOriginal(self, id, iid, locale, flags, &empty, nullptr, nullptr, nullptr);
            })) {
        if (result) {
            *result = {};
        }
        hr = S_OK;
    } else {
        hr = invokeOriginal(self, id, iid, locale, flags, args, result, exception, argError);
    }
    if (id == DISPID_FILELISTENUMDONE) {
        FileTransition::EnumerationDone();
    }
    return hr;
}

static HRESULT __fastcall CleanupHook(void* self) {
    if (RibbonWork::queue) {
        if (!RibbonWork::queue->draining) {
            RibbonWork::Flush();
        } else {
            RibbonWork::DropTasksForTarget(self);
        }
    }
    connected = nullptr;
    return cleanupOriginal(self);
}

static void HookWindowsUiFileExplorerIfLoaded(HMODULE mod) {
    if (!settings.xamlCommandBar && !settings.navigationBar) {
        return;
    }
    if (!mod) {
        mod = GetModuleHandleW(L"Windows.UI.FileExplorer.dll");
    }
    if (!mod) {
        return;
    }
    if (hooksInstalled.exchange(true)) {
        return;
    }

    // Windows.UI.FileExplorer.dll
    WindhawkUtils::SYMBOL_HOOK windowsUiFileExplorerHooks[] = {
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::OnBrowserNavigated(void))"},
            reinterpret_cast<void**>(&navigatedOriginal),
            reinterpret_cast<void*>(NavigatedHook),
            true,
        },
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::OnShellViewChanged(void))"},
            reinterpret_cast<void**>(&changedOriginal),
            reinterpret_cast<void*>(ChangedHook),
            true,
        },
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::OnCommandStateInvalidated(void))"},
            reinterpret_cast<void**>(&invalidatedOriginal),
            reinterpret_cast<void*>(InvalidatedHook),
            true,
        },
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::SetNavigationState(unsigned long))"},
            reinterpret_cast<void**>(&stateOriginal),
            reinterpret_cast<void*>(StateHook),
            true,
        },
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::InvalidateCommands(enum COMMAND_SET))"},
            reinterpret_cast<void**>(&invalidateOriginal),
            reinterpret_cast<void*>(InvalidateHook),
            true,
        },
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::Invoke(long,struct _GUID const &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *))"},
            reinterpret_cast<void**>(&invokeOriginal),
            reinterpret_cast<void*>(InvokeHook),
            true,
        },
        {
            {LR"(public: long __cdecl CommandBarViewAdapter::Cleanup(void))"},
            reinterpret_cast<void**>(&cleanupOriginal),
            reinterpret_cast<void*>(CleanupHook),
            true,
        },
    };

    bool hooked = WindhawkUtils::HookSymbols(mod, windowsUiFileExplorerHooks,
                                             ARRAYSIZE(windowsUiFileExplorerHooks));
    if (!hooked) {
        hooksInstalled.store(false, std::memory_order_release);
        return;
    }

    bool isEnabled = cleanupOriginal && ((navigatedOriginal && changedOriginal) ||
                                         invalidatedOriginal || stateOriginal ||
                                         invalidateOriginal || invokeOriginal);
    enabled.store(isEnabled, std::memory_order_release);
    Wh_Log(L"[XAML] Modern XAML deferral (enabled=%d)", isEnabled ? 1 : 0);
}

static void Uninit() {
    enabled.store(false, std::memory_order_release);
    hooksInstalled.store(false, std::memory_order_release);
}

} // namespace XamlWork

// Native paint and batching hooks.

static void __fastcall RedrawFrameHook(void* self) {
    PaintScope scope(redrawFrameDepth);
    redrawFrameOriginal(self);
}

static HRESULT __fastcall RenderSizerHook(void* self) {
    PaintScope scope(renderSizerDepth);
    return renderSizerOriginal(self);
}

static BOOL WINAPI SetWindowPosHook(HWND hwnd, HWND after, int x, int y, int cx, int cy,
                                    UINT flags) {
    if (!FileTransition::navigationDepth || !settings.navigationPaint) {
        return setWindowPosOriginal(hwnd, after, x, y, cx, cy, flags);
    }
    if (IsShellDesktopOrTrayWindow(hwnd)) {
        return setWindowPosOriginal(hwnd, after, x, y, cx, cy, flags);
    }
    bool defer = FileTransition::DeferNavigationPaint() && !(flags & SWP_NOREDRAW) &&
                 GetWindowThreadProcessId(hwnd, nullptr) == GetCurrentThreadId() &&
                 (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD);
    if (!defer) {
        return setWindowPosOriginal(hwnd, after, x, y, cx, cy, flags);
    }

    HWND parent = GetParent(hwnd);
    RECT beforeRect{};
    bool hasBefore = false;
    if (parent && GetWindowRect(hwnd, &beforeRect)) {
        MapWindowPoints(HWND_DESKTOP, parent, reinterpret_cast<LPPOINT>(&beforeRect), 2);
        hasBefore = true;
    }

    BOOL result = setWindowPosOriginal(hwnd, after, x, y, cx, cy, flags | SWP_NOREDRAW);

    if (result && hasBefore && parent) {
        RECT afterRect{};
        if (GetWindowRect(hwnd, &afterRect)) {
            MapWindowPoints(HWND_DESKTOP, parent, reinterpret_cast<LPPOINT>(&afterRect), 2);
            RECT dirty{};
            UnionRect(&dirty, &beforeRect, &afterRect);
            RedrawWindow(parent, &dirty, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
        } else {
            RedrawWindow(parent, &beforeRect, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
        }
    }

    return result;
}

static BOOL WINAPI UpdateWindowHook(HWND hwnd) {
    if (!redrawFrameDepth || !settings.navigationPaint) {
        return updateWindowOriginal(hwnd);
    }
    return FileTransition::DeferNavigationPaint() ? TRUE : updateWindowOriginal(hwnd);
}

static BOOL WINAPI RedrawWindowHook(HWND hwnd, const RECT* rect, HRGN region, UINT flags) {
    if (!renderSizerDepth || !settings.navigationPaint) {
        return redrawWindowOriginal(hwnd, rect, region, flags);
    }
    if (FileTransition::DeferNavigationPaint()) {
        flags &= ~static_cast<UINT>(RDW_UPDATENOW | RDW_ERASENOW);
    }
    return redrawWindowOriginal(hwnd, rect, region, flags);
}

static HMODULE WINAPI LoadLibraryExWHook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = loadLibraryExWOriginal(lpLibFileName, hFile, dwFlags);
    if (hModule && !XamlWork::hooksInstalled.load(std::memory_order_relaxed)) {
        if (GetModuleHandleW(L"Windows.UI.FileExplorer.dll") == hModule) {
            XamlWork::HookWindowsUiFileExplorerIfLoaded(hModule);
            Wh_ApplyHookOperations();
        }
    }
    return hModule;
}

static uintptr_t CreateActionHook(const Action* action) {
    bool batch = action && action->size == sizeof(Action) && batchCallback &&
                 action->callback == batchCallback;
    auto state = FileTransition::active;
    bool observe = trackBatchActions && batch && state && state->itemsView == action->context;
    ULONG generation = observe ? state->generation : 0;
    uintptr_t handleVal = createActionOriginal(action);
    void* handle = reinterpret_cast<void*>(handleVal);
    if (observe && handle && FileTransition::active == state &&
        state->generation == generation) {
        state->batchAction = handle;
        if (!state->pending) {
            state->pending = true;
            ++state->batchCount;
        }
    }
    return handleVal;
}

static BOOL WINAPI DeleteActionHook(void* handle) {
    auto state = FileTransition::active;
    bool observe = state && handle && state->batchAction == handle;
    ULONG generation = observe ? state->generation : 0;
    BOOL result = deleteActionOriginal(handle);
    if (observe && result && FileTransition::active == state &&
        state->generation == generation && state->batchAction == handle) {
        state->batchAction = nullptr;
        state->pending = false;
        state->enumerated = true;
        FileTransition::NotifyReady();
    }
    return result;
}

// Module initialization and cleanup.

static void ReleaseModules() {
    XamlWork::Uninit();
    if (duser) {
        FreeLibrary(duser);
        duser = nullptr;
    }
    if (shell) {
        FreeLibrary(shell);
        shell = nullptr;
    }
    if (frame) {
        FreeLibrary(frame);
        frame = nullptr;
    }
}

static BOOL InitializeMod() {
    LoadSettings();

    frame = LoadLibraryExW(L"ExplorerFrame.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    shell = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (frame) {
        // ExplorerFrame.dll
        WindhawkUtils::SYMBOL_HOOK explorerFrameHooks[] = {
            {
                {LR"(private: static void __cdecl UIItemsView::s_BatchTimerCallback(struct GMA_ACTIONINFO *))"},
                reinterpret_cast<void**>(&batchCallback),
                nullptr,
                true,
            },
            {
                {LR"(private: long __cdecl CShellBrowser::_NavigateToPidl(struct _ITEMIDLIST_ABSOLUTE const *,unsigned long,unsigned long))"},
                reinterpret_cast<void**>(&FileTransition::navigateOriginal),
                reinterpret_cast<void*>(FileTransition::NavigateHook),
                true,
            },
            {
                {LR"(private: long __cdecl CShellBrowser::_OnConnectionCreated(struct IShellItem *,struct IUnknown *,unsigned long,unsigned long,struct IUnknown *,struct IUnknown *,struct _GUID const &))"},
                reinterpret_cast<void**>(&FileTransition::connectionOriginal),
                reinterpret_cast<void*>(FileTransition::ConnectionHook),
                true,
            },
            {
                {LR"(private: long __cdecl UIItemsView::_ResetRoot(struct IItem *,struct IItemCollection *))"},
                reinterpret_cast<void**>(&FileTransition::resetOriginal),
                reinterpret_cast<void*>(FileTransition::ResetHook),
                true,
            },
            {
                {LR"(private: long __cdecl UIItemsView::_EnsureBatching(void))"},
                reinterpret_cast<void**>(&FileTransition::ensureOriginal),
                reinterpret_cast<void*>(FileTransition::EnsureHook),
                true,
            },
            {
                {LR"(private: void __cdecl UIItemsView::_OnBatchTimer(struct GMA_ACTIONINFO *))"},
                reinterpret_cast<void**>(&FileTransition::batchOriginal),
                reinterpret_cast<void*>(FileTransition::BatchHook),
                true,
            },
            {
                {LR"(private: bool __cdecl UIItemsView::DeleteBatchTimer(void))"},
                reinterpret_cast<void**>(&FileTransition::deleteOriginal),
                reinterpret_cast<void*>(FileTransition::DeleteHook),
                true,
            },
            {
                {LR"(public: virtual long __cdecl CExplorerRibbon::Invoke(long,struct _GUID const &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *))"},
                reinterpret_cast<void**>(&FileTransition::invokeOriginal),
                reinterpret_cast<void*>(FileTransition::InvokeHook),
                true,
            },
            {
                {LR"(public: virtual long __cdecl CExplorerRibbon::OnBrowserNavigated(void))"},
                reinterpret_cast<void**>(&RibbonWork::navigatedOriginal),
                reinterpret_cast<void*>(RibbonWork::NavigatedHook),
                true,
            },
            {
                {LR"(public: virtual long __cdecl CExplorerRibbon::OnShellViewChanged(void))"},
                reinterpret_cast<void**>(&RibbonWork::changedOriginal),
                reinterpret_cast<void*>(RibbonWork::ChangedHook),
                true,
            },
            {
                {LR"(public: virtual long __cdecl CExplorerRibbon::DestroyRibbonUI(int))"},
                reinterpret_cast<void**>(&RibbonWork::destroyOriginal),
                reinterpret_cast<void*>(RibbonWork::DestroyHook),
                true,
            },
            {
                {LR"(public: virtual long __cdecl CNavBar::SetNavigationState(unsigned long))"},
                reinterpret_cast<void**>(&RibbonWork::navStateOriginal),
                reinterpret_cast<void*>(RibbonWork::NavStateHook),
                true,
            },
        };
        WindhawkUtils::HookSymbols(frame, explorerFrameHooks,
                                   ARRAYSIZE(explorerFrameHooks));
    }

    if (shell) {
        // shell32.dll
        WindhawkUtils::SYMBOL_HOOK shell32Hooks[] = {
            {
                {LR"(private: void __cdecl CDUIViewFrame::_RedrawFrame(void))"},
                reinterpret_cast<void**>(&redrawFrameOriginal),
                reinterpret_cast<void*>(RedrawFrameHook),
                true,
            },
            {
                {LR"(private: long __cdecl CDUISizerElement::_Render(void))"},
                reinterpret_cast<void**>(&renderSizerOriginal),
                reinterpret_cast<void*>(RenderSizerHook),
                true,
            },
        };
        WindhawkUtils::HookSymbols(shell, shell32Hooks, ARRAYSIZE(shell32Hooks));
    }

    if (settings.xamlCommandBar || settings.navigationBar) {
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
        if (kernelbase) {
            auto pfnLoadLibraryExW = reinterpret_cast<decltype(&LoadLibraryExW)>(
                GetProcAddress(kernelbase, "LoadLibraryExW"));
            if (pfnLoadLibraryExW) {
                WindhawkUtils::SetFunctionHook(pfnLoadLibraryExW, LoadLibraryExWHook,
                                               &loadLibraryExWOriginal);
            }
        }
        XamlWork::HookWindowsUiFileExplorerIfLoaded(nullptr);
    }

    if (!FileTransition::navigateOriginal && !FileTransition::resetOriginal &&
        !RibbonWork::navigatedOriginal && !RibbonWork::navStateOriginal &&
        !XamlWork::enabled.load(std::memory_order_relaxed) &&
        !((settings.xamlCommandBar || settings.navigationBar) && loadLibraryExWOriginal)) {
        Wh_Log(L"[CORE] No usable hooks on this build");
        return FALSE;
    }

    bool paint = false;
    bool layoutPaint = false;
    bool positionPaint = false;
    if (settings.navigationPaint) {
        paint = WindhawkUtils::SetFunctionHook(UpdateWindow, UpdateWindowHook,
                                               &updateWindowOriginal);
        layoutPaint = WindhawkUtils::SetFunctionHook(RedrawWindow, RedrawWindowHook,
                                                     &redrawWindowOriginal);
        positionPaint = WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPosHook,
                                                       &setWindowPosOriginal);
    }

    if (!RibbonWork::Initialize()) {
        Wh_Log(L"[CORE] Failed to initialize RibbonWork window class");
        return FALSE;
    }

    bool nativeBatchTracking =
        FileTransition::ensureOriginal && FileTransition::deleteOriginal;
    bool batches = nativeBatchTracking;
    if (!nativeBatchTracking && batchCallback) {
        duser = LoadLibraryExW(L"DUser.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (duser) {
            createActionOriginal =
                reinterpret_cast<CreateAction>(GetProcAddress(duser, "CreateAction"));
            deleteActionOriginal =
                reinterpret_cast<DeleteAction>(GetProcAddress(duser, "DeleteHandle"));
            if (createActionOriginal && deleteActionOriginal &&
                WindhawkUtils::SetFunctionHook(createActionOriginal, CreateActionHook,
                                               &createActionOriginal) &&
                WindhawkUtils::SetFunctionHook(deleteActionOriginal, DeleteActionHook,
                                               &deleteActionOriginal)) {
                trackBatchActions = true;
                batches = true;
            }
        }
    }

    FileTransition::completionTracking =
        FileTransition::batchOriginal && (nativeBatchTracking || trackBatchActions);

    Wh_Log(L"[CORE] Initialization complete (batching=%d, activationPaint=%d, layoutPaint=%d, positionPaint=%d)",
           batches, paint, layoutPaint, positionPaint);

    return TRUE;
}

// Windhawk lifecycle entry points.

BOOL Wh_ModInit() {
    if (InitializeMod()) {
        return TRUE;
    }
    RibbonWork::Uninit();
    ReleaseModules();
    return FALSE;
}

void Wh_ModAfterInit() {
    if (settings.xamlCommandBar || settings.navigationBar) {
        XamlWork::HookWindowsUiFileExplorerIfLoaded(nullptr);
        Wh_ApplyHookOperations();
    }
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}

void Wh_ModBeforeUninit() {
    RibbonWork::BeforeUninit();
}

void Wh_ModUninit() {
    RibbonWork::Uninit();
    ReleaseModules();
}
